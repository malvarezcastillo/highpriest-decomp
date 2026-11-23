#include "common.h"
#include "device/Sound.h"
#include "resource/Wave.h"
#include "window/UIFrameWnd.h"
#include "GameMode.h"
#include "Session.h"
#include "GameActor.h"
#include "GameWorld.h"
#include "Mode.h"
#include "HP_Tetris.h"
#include "ItemInfo.h"
#include <string.h>
#include <stdio.h>
#include <mmsystem.h>
#include "resource/Ground.h"

extern char g_Name[];
extern UIWindowMgr g_windowMgr;
extern CSession g_session;
extern CModeMgr g_modeMgr;
extern short g_BS_phase;
extern unsigned long g_BS_NextActTime;
extern short map_Number;
extern Tetris* TT;
extern int g_curStance_B;
extern short game_TetrisPlayed;
extern short game_ShopOpenNumber;
extern short g_menuSelect;
extern short g_menuFreeze;
extern short g_selected_item;
extern short g_selected_item_ID;
extern short g_curStory;
extern short BuyerID;

short BuyerID = 10;
char g_Name[64];
extern int g_curStance;
extern bool g_WordDisplay;

void UIWindowMgr::SetcurStance_B(int F1)
{
	char msg[256];

	g_curStance_B = F1;
	CGameActor* actor = g_session.GetGameActorByAid(2);

	if (TT) {
		delete TT;
		TT = NULL;
	}

	if (F1 == 0) {
		actor->SetState(ST_STAND);
		return;
	}

	if (F1 == 1) {
		TT = new Tetris;
		g_WordDisplay = false;
		if (game_TetrisPlayed == 0 || game_TetrisPlayed == 2) {
			sprintf(msg, "HELP : \xb8\xf0\xbe\xe7\xc0\xbb \xb8\xc2\xc3\xe7\xbc\xad \xc7\xd1\xc1\xd9\xc0\xbb \xc3\xa4\xbf\xf6\xbe\xdf\xc7\xd5\xb4\xcf\xb4\xd9");
			actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
			sprintf(msg, "\xb9\xab\xb1\xe2\xb8\xa6 \xc1\xa6\xc0\xdb \xc7\xcf\xb7\xc1\xb8\xe9 10\xc1\xd9\xc0\xbb \xbc\xd2\xb8\xea\xbd\xc3\xc4\xd1\xbe\xdf\xc7\xd5\xb4\xcf\xb4\xd9.");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffc8c8, 0, 0);
			sprintf(msg, "\xb0\xb0\xc0\xba \xb8\xf0\xbe\xe7\xc0\xb8\xb7\xce \xc7\xd1\xc1\xd9\xc0\xbb \xc3\xa4\xbf\xf6\xbe\xdf \xbc\xd2\xb8\xea\xb5\xc7\xb8\xe7 \xc7\xd1\xc1\xd9\xc0\xba \xbc\xd2\xb8\xea\xb5\xc9\xb6\xa7 \xc1\xd6\xc0\xa7\xb8\xa6 \xc0\xfc\xbf\xb0\xbd\xc3\xc5\xb5\xb4\xcf\xb4\xd9");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffc8c8, 0, 0);
		}
		actor->SetState(ST_ATTACKWAIT);
		return;
	}

	if (F1 == 11) {
		g_BS_phase = 0;
		g_BS_NextActTime = timeGetTime() + 200;
		return;
	}

	if (F1 == 2) {
		g_BS_phase = 0;
		if (actor) {
			++game_ShopOpenNumber;
			sprintf(msg, "%s \xbe\xc6\xba\xfc\xc0\xc7 \xbb\xf3\xc1\xa1", g_session.GetCharName());
			actor->SendMsg(NULL, AM_MAKE_SHOP_TITLE, (int)msg, 0, 0);
			actor->SetState(ST_SIT);
			g_BS_NextActTime = timeGetTime() + ((rand() % 20 + 20) * 1000);
			if (game_ShopOpenNumber <= F1) {
				sprintf(msg, "HINT : \xbc\xd5\xb4\xd4\xc0\xba 1\xba\xd0\xc1\xa4\xb5\xb5 \xb1\xe2\xb4\xd9\xb8\xae\xb8\xe9 \xb3\xaa\xc5\xb8\xb3\xb3\xb4\xcf\xb4\xd9.");
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffc8c8, 0, 0);
			}
		}
		return;
	}

	if (F1 == 3) {
		g_BS_phase = 0;
		g_BS_NextActTime = timeGetTime() + 200;
		g_menuSelect = 1;
	}
}

void UIWindowMgr::BS_Selling()
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	CGameActor* merchant = g_session.GetGameActorByAid(2);
	CGameActor* buyer = g_session.GetGameActorByAid(BuyerID);
	ITEM_INFO itemInfo;
	char PASS = 0;
	char iName[64];
	char msg[256];
	short price = 0;

	if (merchant) {
		if (g_BS_phase == 0) {
			if (g_BS_NextActTime < timeGetTime()) {
				if (g_curStory == 42) {
					BuyerID = 5;
					gameMode->Zc_Notify_Entry_HP(5, 10, 3);
				} else {
					BuyerID = 10;
					gameMode->Zc_Notify_Entry_HP(10, 100, 100);
				}
				g_BS_NextActTime = timeGetTime() + 1000;
				++g_BS_phase;
				buyer = g_session.GetGameActorByAid(BuyerID);
			} else if (g_curStance >= 3 && g_curStance <= 4) {
				g_BS_phase = 10;
				g_BS_NextActTime = timeGetTime() + 1000;
			}
		} else if (g_BS_phase == 1) {
			if (g_BS_NextActTime < timeGetTime()) {
				for (int i = 0; i < g_session.m_MaxItemIndex; ++i) {
					itemInfo = g_session.GetItemInfoByIndex(i);
					if (itemInfo.m_itemType == 5) {
						g_selected_item = (short)i;
						break;
					}
				}
				itemInfo = g_session.GetItemInfoByIndex(g_selected_item);
				g_selected_item_ID = itemInfo.GetItemId();
				itemInfo.GetItemName(iName);
				sprintf(msg, "%s \xbe\xf3\xb8\xb6\xc0\xce\xb0\xa1\xbf\xe4?", iName);
				buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				buyer->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				g_BS_NextActTime = timeGetTime() + 1000;
				++g_BS_phase;
			}
		} else if (g_BS_phase == 2) {
			if (g_BS_NextActTime < timeGetTime()) {
				g_BS_NextActTime = timeGetTime() + 5000;
				++g_BS_phase;
				g_menuFreeze = 0;
			}
		} else if (g_BS_phase == 3) {
			if (g_BS_NextActTime < timeGetTime()) {
				++g_BS_phase;
			}
		} else if (g_BS_phase == 4) {
			itemInfo = g_session.GetItemInfoByIndex(g_selected_item);
			g_BS_NextActTime = timeGetTime() + 1000;
			++g_BS_phase;
			if (g_menuFreeze == 0) {
				g_BS_NextActTime = timeGetTime();
				return;
			} else if (g_menuFreeze == 1) {
				price = itemInfo.m_price * 1.5;
			} else if (g_menuFreeze == 2) {
				price = itemInfo.m_price;
			} else if (g_menuFreeze == 3) {
				price = itemInfo.m_price * 0.5;
			}
			sprintf(msg, "%d \xc1\xa6\xb4\xcf \xc0\xd4\xb4\xcf\xb4\xd9", price);
			merchant->SendMsg(NULL, AM_SET_HEAD_DIR, 2, 0, 0);
			merchant->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
			merchant->SendMsg(NULL, AM_EMOTION, 8, 0, 0);
		} else if (g_BS_phase == 5) {
			if (g_BS_NextActTime < timeGetTime()) {
				itemInfo = g_session.GetItemInfoByIndex(g_selected_item);
				++g_BS_phase;
				if (g_selected_item_ID != itemInfo.GetItemId()) {
					sprintf(msg, "\xb0\xa9\xc0\xda\xb1\xe2 \xbc\xee\xc7\xce\xc7\xcf\xb1\xe2 \xbd\xc8\xbe\xee\xc1\xb3\xbe\xee\xbf\xe4..");
					buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					return;
				} else if (g_menuFreeze == 0) {
					g_BS_NextActTime = timeGetTime() + 3000;
					if (buyer->GetJob() == JT_THIEF) {
						sprintf(msg, "\xa4\xbb\xa4\xbb\xa4\xbb\xa4\xbb\xa4\xbb \xc1\xa4\xbd\xc5\xc0\xbb \xb4\xd9\xb8\xa5\xb5\xa5 \xc6\xc8\xb0\xed\xc0\xd6\xb1\xba. \xc0\xcc\xb0\xc7 \xb3\xbb\xb0\xa1 \xb1\xd7\xb3\xc9 \xb0\xa1\xc1\xae\xb0\xa1\xb8\xb6");
						buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						buyer->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
						g_session.DecItem(g_selected_item, 1);
					} else {
						sprintf(msg, "\xb8\xbb\xc0\xcc \xbe\xf8\xb3\xd7... \xc0\xe5\xbb\xe7\xc7\xcf\xb1\xe2 \xbd\xc8\xc0\xba\xb0\xa1\xba\xc1");
						buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						buyer->SendMsg(NULL, AM_EMOTION, 4, 0, 0);
					}
					return;
				} else if (g_menuFreeze >= 1) {
					g_BS_NextActTime = timeGetTime() + 3000;
					if (buyer->GetJob() == JT_NOVICE) {
						PASS = 1;
					} else if (buyer->GetJob() == JT_THIEF) {
						if (g_menuFreeze == 3) PASS = 1;
					} else if (g_menuFreeze >= 2) {
						PASS = 1;
					}
					if (buyer->GetJob() == JT_LINKER) {
						if (g_menuFreeze == 3) {
							sprintf(msg, "\xbd\xce\xb0\xd4 \xc6\xc8\xbe\xc6\xc1\xd6\xbc\xc5\xbc\xad \xb0\xed\xb8\xb6\xbf\xf6\xbf\xe4 \xa4\xd0_\xa4\xd0 \xc0\xcc\xc1\xa6 \xb8\xf0\xb7\xce\xc5\xa9\xb7\xce \xb5\xb9\xbe\xc6\xb0\xa1\xbe\xdf\xb0\xda\xb3\xd7\xbf\xe4 ^^;");
							buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
							buyer->SendMsg(NULL, AM_EMOTION, 0x12, 0, 0);
							price = itemInfo.m_price * 0.5;
							g_session.DecItem(g_selected_item, 1);
							PlayWave("effect\\rog_steal coin.wav");
							g_session.SetGold(g_session.GetGold() + price);
							g_curStory = 43;
						} else {
							sprintf(msg, "\xc1\xa6\xb0\xa1 \xb5\xb7\xc0\xcc \xba\xb0\xb7\xce \xbe\xf8\xbe\xee\xbf\xe4 \xa4\xd0_\xa4\xd0 \xbd\xce\xb0\xd4 \xc6\xc8\xbe\xc6\xc1\xd6\xb4\xc2 \xb0\xa1\xb0\xd4 \xbe\xee\xb5\xf0\xbe\xf8\xb3\xaa\xbf\xe4.. \xc8\xe6");
							buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
							buyer->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
						}
					} else if (PASS == 1) {
						sprintf(msg, "\xbb\xec\xb2\xb2\xbf\xe4~");
						buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						buyer->SendMsg(NULL, AM_EMOTION, 0x12, 0, 0);
						if (g_menuFreeze == 1) {
							price = itemInfo.m_price * 1.5;
						} else if (g_menuFreeze == 2) {
							price = itemInfo.m_price;
						} else if (g_menuFreeze == 3) {
							price = itemInfo.m_price * 0.5;
						}
						g_session.DecItem(g_selected_item, 1);
						PlayWave("effect\\rog_steal coin.wav");
						g_session.SetGold(g_session.GetGold() + price);
					} else if (buyer->GetJob() == JT_THIEF) {
						sprintf(msg, "\xc1\xbb \xb4\xf5 \xbd\xce\xc1\xf6\xb8\xe9 \xb4\xd9\xbd\xc3 \xbf\xc3\xb2\xb2\xbf\xe4");
						buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						buyer->SendMsg(NULL, AM_EMOTION, 6, 0, 0);
					} else {
						sprintf(msg, "\xb3\xca\xb9\xab \xba\xf1\xbd\xce\xb3\xd7\xbf\xe4;");
						buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						buyer->SendMsg(NULL, AM_EMOTION, 0x17, 0, 0);
					}
				}
			}
		} else if (g_BS_phase == 6) {
			if (g_BS_NextActTime < timeGetTime()) {
				merchant->SendMsg(NULL, AM_SET_HEAD_DIR, 0, 0, 0);
				if (g_menuFreeze == 0 && buyer->GetJob() == JT_THIEF) {
					MOVESTRUCT ms;
					gameMode = g_modeMgr.GetGameMode();
					gameMode->GetServerCoor(buyer->m_pos.x, buyer->m_pos.z, ms.sx, ms.sy);
					ms.dx = -5;
					ms.dy = 3;
					ms.cellX = 0;
					ms.cellY = 0;
					buyer->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);
					PlayWave("effect\\rog_intimidate.wav");
				} else {
					gameMode->PlaceEffect(&buyer->m_pos, 0x130, 0x12c, 0);
					gameMode->GetWorld()->RemoveGameActor(BuyerID);
				}
				g_selected_item = 0;
				for (int i = 0; i < g_session.m_MaxItemIndex; ++i) {
					itemInfo = g_session.GetItemInfoByIndex(i);
					if (itemInfo.m_itemType == 5) {
						g_selected_item = (short)i;
						break;
					}
				}
				if (g_selected_item == 0) {
					g_BS_phase = 10;
					g_BS_NextActTime = timeGetTime() + 1000;
				} else {
					g_BS_phase = 0;
					g_BS_NextActTime = timeGetTime() + ((rand() % 20 + 20) * 1000);
				}
			}
		} else if (g_BS_NextActTime < timeGetTime()) {
			g_curStance_B = 0;
			merchant->SendMsg(NULL, AM_DELETE_SHOP_TITLE, 0, 0, 0);
			merchant->SetState(ST_STAND);
		}
	}
}

void UIWindowMgr::BS_Buying()
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	ITEM_INFO itemInfo;
	char msg[256];
	CGameActor* merchant = g_session.GetGameActorByAid(2);
	CGameActor* actor = g_session.GetGameActorByAid(11);

	if (merchant) {
		if (g_BS_phase == 0) {
			if (g_BS_NextActTime < timeGetTime()) {
				gameMode->Zc_Notify_Entry_HP(11, 100, 100);
				g_BS_NextActTime = timeGetTime() + 1000;
				++g_BS_phase;
			}
		} else if (g_BS_phase == 1) {
			if (g_BS_NextActTime < timeGetTime()) {
				g_BS_NextActTime = timeGetTime() + 1000;
				++g_BS_phase;
				sprintf(msg, "\xc5\xda\xb7\xb9\xc6\xf7\xc6\xae \xbb\xf3\xc0\xce \xc0\xd4\xb4\xcf\xb4\xd9 ^_^  \xb9\xab\xbe\xf3 \xbb\xe7\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?");
				actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 8, 0, 0);
			}
		} else if (g_BS_phase == 2) {
			if (g_BS_NextActTime < timeGetTime()) {
				g_BS_NextActTime = timeGetTime() + 10000;
				++g_BS_phase;
				g_menuFreeze = 0;
			}
		} else if (g_BS_phase == 3) {
			if (g_BS_NextActTime < timeGetTime()) {
				++g_BS_phase;
			}
		} else if (g_BS_phase == 4) {
			if (g_menuFreeze == 1) {
				if (g_curStory < 10) {
					g_selected_item = 0x64a;
				} else if (g_curStory < 20) {
					g_selected_item = 0x5e0;
				} else if (g_curStory < 30) {
					g_selected_item = 0x64e;
				} else if (g_curStory < 40) {
					g_selected_item = 0x5ec;
				} else if (g_curStory < 50) {
					g_selected_item = 0x64f;
				} else if (g_curStory < 60) {
					g_selected_item = 0x5ef;
				} else if (g_curStory < 70) {
					g_selected_item = 0x5f4;
				} else if (g_curStory < 80) {
					g_selected_item = 0x5f9;
				} else {
					g_selected_item = 0x64d;
				}
				GetitemName_HP(g_selected_item);
				sprintf(msg, "\xb0\xf8\xb0\xdd\xb7\xc2 %d \xc0\xce %s \xb0\xa1 %d \xc1\xa6\xb4\xcf \xc0\xd4\xb4\xcf\xb4\xd9~",
					gameMode->GetWeaponPower(g_selected_item), g_Name, gameMode->GetWeaponCost(g_selected_item));
				actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				g_BS_NextActTime = timeGetTime() + 1000;
				++g_BS_phase;
			} else if (g_menuFreeze == 2) {
				short itemSelect;
				if (g_curStory < 40) {
					itemSelect = rand() % 8;
				} else if (g_curStory < 80) {
					itemSelect = rand() % 15;
				} else {
					itemSelect = rand() % 20;
				}
				if (itemSelect == 0) {
					g_selected_item = 0x1435;
				} else if (itemSelect == 1) {
					g_selected_item = 0x14b4;
				} else if (itemSelect == 2) {
					g_selected_item = 0x145c;
				} else if (itemSelect == 3) {
					g_selected_item = 0x148f;
				} else if (itemSelect == 4) {
					g_selected_item = 0x8c2;
				} else if (itemSelect == 5) {
					g_selected_item = 0x8e4;
				} else if (itemSelect == 6) {
					g_selected_item = 0x1437;
				} else if (itemSelect == 7) {
					g_selected_item = 0x140f;
				} else if (itemSelect == 8) {
					g_selected_item = 0x145e;
				} else if (itemSelect == 9) {
					g_selected_item = 0x1411;
				} else if (itemSelect == 10) {
					g_selected_item = 0x1408;
				} else if (itemSelect == 11) {
					g_selected_item = 0x1467;
				} else if (itemSelect == 12) {
					g_selected_item = 0x14ed;
				} else if (itemSelect == 13) {
					g_selected_item = 0x8ee;
				} else if (itemSelect == 14) {
					g_selected_item = 0x14c3;
				} else if (itemSelect == 15) {
					g_selected_item = 0x145a;
				} else if (itemSelect == 16) {
					g_selected_item = 0x1504;
				} else if (itemSelect == 17) {
					g_selected_item = 0x1505;
				} else if (itemSelect == 18) {
					g_selected_item = 0x14cd;
				} else if (itemSelect == 19) {
					g_selected_item = 0x13ee;
				}
				GetitemName_HP(g_selected_item);
				sprintf(msg, "\xbf\xc0\xb4\xc3\xc0\xba %s \xb0\xa1\xc1\xae\xbf\xd4\xbe\xee\xbf\xe4, %d \xc1\xa6\xb4\xcf \xbe\xee\xb6\xb0\xbc\xbc\xbf\xe4?",
					g_Name, gameMode->GetWeaponCost(g_selected_item));
				actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				g_BS_NextActTime = timeGetTime() + 1000;
				++g_BS_phase;
			} else if (g_menuFreeze == 3) {
				BS_NotBuying();
			} else {
				BS_NotAnswering();
			}
		} else if (g_BS_phase == 5) {
			if (g_BS_NextActTime < timeGetTime()) {
				g_BS_NextActTime = timeGetTime() + 10000;
				++g_BS_phase;
				g_menuFreeze = 0;
			}
		} else if (g_BS_phase == 6) {
			if (g_BS_NextActTime < timeGetTime()) {
				++g_BS_phase;
			}
		} else if (g_BS_phase == 7) {
			BS_Buying_Weapon();
		} else if (g_BS_phase == 0x14) {
			if (g_BS_NextActTime < timeGetTime()) {
				gameMode->PlaceEffect(&actor->m_pos, 0x130, 0x12c, 0);
				gameMode->GetWorld()->RemoveGameActor(11);
				g_BS_NextActTime = timeGetTime() + 1000;
				++g_BS_phase;
			}
		} else if (g_BS_NextActTime < timeGetTime()) {
			g_curStance_B = 0;
		}
	}
}

void UIWindowMgr::BS_Buying_Weapon()
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	ITEM_INFO itemInfo;
	char msg[256];

	g_session.GetGameActorByAid(2);
	CGameActor* actor = g_session.GetGameActorByAid(11);

	if (g_BS_NextActTime < timeGetTime()) {
		int cost = gameMode->GetWeaponCost(g_selected_item);
		actor->SetState(ST_STAND);
		actor->SendMsg(NULL, AM_SET_HEAD_DIR, 2, 0, 0);

		if (g_menuFreeze == 1 || g_menuFreeze == 2) {
			if (g_session.GetGold() - cost < 0) {
				sprintf(msg, "\xb5\xb7\xb5\xb5 \xbe\xf8\xb4\xc2 \xc1\xd6\xc1\xa6\xbf\xa1 -_- \xbb\xe7\xb1\xe4 \xb9\xbb \xbb\xe7\xb7\xc1\xb0\xed");
				actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 0x17, 0, 0);
				g_BS_phase = 0x14;
				g_BS_NextActTime = timeGetTime() + 3000;
				return;
			}
		}

		if (g_menuFreeze == 1) {
			PlayWave("effect\\rog_steal coin.wav");
			g_session.SetGold(g_session.GetGold() - cost);
			g_session.HP_AddItem(g_selected_item, true);
			sprintf(msg, "%d \xc1\xa6\xb4\xcf  \xb0\xa8\xbb\xe7\xc7\xd5\xb4\xcf\xb4\xd9~", cost);
			actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 0x12, 0, 0);
			g_BS_phase = 0x14;
			g_BS_NextActTime = timeGetTime() + 3000;
		} else if (g_menuFreeze == 2) {
			if (actor->GetJob() == JT_ALCHEMIST&& rand() % 10 <= 1) {
				cost /= 2;
				PlayWave("effect\\rog_steal coin.wav");
				g_session.SetGold(g_session.GetGold() - cost);
				g_session.HP_AddItem(g_selected_item, true);
				sprintf(msg, "\xbf\xc0\xb4\xc3\xc0\xba \xc6\xaf\xba\xb0\xc8\xf7 \xb9\xdd\xb0\xaa\xc0\xce %d \xc1\xa6\xb4\xcf\xbf\xa1 \xc6\xc8\xb2\xb2\xbf\xe4 ^^;", cost);
				actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 0x12, 0, 0);
				g_BS_phase = 0x14;
				g_BS_NextActTime = timeGetTime() + 3000;
				return;
			}

			sprintf(msg, "\xbf\xa9\xb1\xe2\xbc\xad \xb4\xf5 \xb1\xef\xc0\xb8\xb8\xe9 \xc0\xfa\xc8\xf1 \xb8\xc1\xc7\xd8\xbf\xe4.");
			actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 0x20, 0, 0);
			g_BS_phase = 0x14;
			g_BS_NextActTime = timeGetTime() + 3000;
		} else if (g_menuFreeze == 3) {
			BS_NotBuying();
		} else {
			BS_NotAnswering();
		}
	}
}

void UIWindowMgr::BS_NotBuying()
{
	CGameActor* actor = g_session.GetGameActorByAid(11);
	PlayWave("effect\\rog_steal coin.wav");
	g_session.SetGold(g_session.GetGold() - 100);
	char msg[256];
	sprintf(msg, "\xc5\xda\xb7\xb9\xc6\xf7\xc6\xae \xbb\xf3\xc1\xa1 \xc3\xe2\xc0\xe5\xba\xf1\xbf\xeb\xc0\xba %d \xc1\xa6\xb4\xcf \xc0\xd4\xb4\xcf\xb4\xd9 ^_^", 100);
	actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
	actor->SendMsg(NULL, AM_EMOTION, 0xf, 0, 0);
	g_BS_phase = 0x14;
	g_BS_NextActTime = timeGetTime() + 0xbb8;
}

void UIWindowMgr::BS_NotAnswering()
{
	CGameActor* actor = g_session.GetGameActorByAid(11);
	PlayWave("effect\\rog_steal coin.wav");
	g_session.SetGold(g_session.GetGold() - 1000);
	char msg[256];
	sprintf(msg, "\xbb\xe7\xb6\xf7\xc0\xbb \xba\xd2\xb7\xb6\xc0\xb8\xb8\xe9 \xb8\xbb\xc0\xbb \xc7\xd8\xbe\xdf\xc1\xd2... \xc3\xe2\xc0\xe5\xba\xf1 %d \xc1\xa6\xb4\xcf\xb4\xc2 \xb0\xa1\xc1\xae\xb0\xa9\xb4\xcf\xb4\xd9 -_-", 1000);
	actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
	actor->SendMsg(NULL, AM_EMOTION, 6, 0, 0);
	g_BS_phase = 0x14;
	g_BS_NextActTime = timeGetTime() + 0xbb8;
}

void UIWindowMgr::BS_Making()
{
	g_modeMgr.GetGameMode();
	CGameActor* actor = g_session.GetGameActorByAid(2);
	ITEM_INFO itemInfo;

	if (actor) {
		if (g_BS_phase == 0) {
			if (g_BS_NextActTime < timeGetTime()) {
				actor->SendMsg(NULL, AM_EMOTION, 9, 0, 0);
				g_BS_NextActTime = timeGetTime() + 2000;
				++g_BS_phase;
			}
		} else if (g_BS_phase <= 7) {
			if (g_BS_NextActTime < timeGetTime()) {
				actor->SendMsg(NULL, AM_EMOTION, 0x13, 0, 0);
				g_BS_NextActTime = timeGetTime() + 4000;
				if (actor->m_stateId != 11) {
					actor->SetState(ST_ATTACKWAIT);
				}
				++g_BS_phase;
			}
		} else if (g_BS_phase == 8) {
			if (g_BS_NextActTime < timeGetTime()) {
				actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
				g_BS_NextActTime = timeGetTime() + 1500;
				++g_BS_phase;
			}
		} else if (g_BS_phase == 9) {
			if (g_BS_NextActTime < timeGetTime()) {
				if (rand() % 2 == 0) {
					actor->LaunchEffect(0x9a);
					g_session.HP_MakingItem();
				} else {
					actor->LaunchEffect(0x9b);
				}
				g_BS_NextActTime = timeGetTime() + 2000;
				++g_BS_phase;
			}
		} else if (g_BS_NextActTime < timeGetTime()) {
			g_curStance_B = 0;
			actor->SetState(ST_STAND);
		}
	}
}

char * UIWindowMgr::GetitemName_HP(int ITID)
{
	ITEM_INFO itemInfo;
	itemInfo.SetItemId(ITID);
	itemInfo.m_isIdentified = 1;
	itemInfo.GetItemName(g_Name);
	return g_Name;
}

char * UIWindowMgr::GetmapName_HP(int A_Pos)
{
	if (map_Number == 10) {
		if (A_Pos < 2000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xd4\xc7\xc1\xc7\xca\xb5\xe5""480.jpg");
		} else if (A_Pos < 3000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xd4\xc7\xc1\xc7\xca\xb5\xe5""480_2.jpg");
		} else {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xd4\xc7\xc1\xc7\xca\xb5\xe5""480_3.jpg");
		}
	} else if (map_Number == 11) {
		if (A_Pos < 2000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xc6\xe4\xc0\xcc\xbf\xe6\xb4\xf8\xc1\xaf""480.jpg");
		} else if (A_Pos < 3000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xc6\xe4\xc0\xcc\xbf\xe6\xb4\xf8\xc1\xaf""480_3.jpg");
		} else {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xc6\xe4\xc0\xcc\xbf\xe6\xb4\xf8\xc1\xaf""480_2.jpg");
		}
	} else if (map_Number == 12) {
		if (A_Pos < 2000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb9\xa6\xb8\xa3\xb4\xd2\xbb\xea\xb8\xc6""480.jpg");
		} else if (A_Pos < 3000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb9\xa6\xb8\xa3\xb4\xd2\xbb\xea\xb8\xc6""480_3.jpg");
		} else {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb9\xa6\xb8\xa3\xb4\xd2\xbb\xea\xb8\xc6""480_2.jpg");
		}
	} else if (map_Number == 13) {
		if (A_Pos < 2000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xc5\xba\xcf\xbc\xb6""480_4.jpg");
		} else if (A_Pos < 3000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xc5\xba\xcf\xbc\xb6""480.jpg");
		} else {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xc5\xba\xcf\xbc\xb6""480_3.jpg");
		}
	} else if (map_Number == 14) {
		if (A_Pos < 2000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xbe\xee\xba\xf1\xbd\xba\xc8\xa3\xbc\xf6""480.jpg");
		} else if (A_Pos < 3000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xbe\xee\xba\xf1\xbd\xba\xc8\xa3\xbc\xf6""480_2.jpg");
		} else {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xbe\xee\xba\xf1\xbd\xba\xc8\xa3\xbc\xf6""480_3.jpg");
		}
	} else if (map_Number == 15) {
		if (A_Pos < 2000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xc1\xd7\xc0\xbd\xc0\xc7\xc5\xbe""480.jpg");
		} else if (A_Pos < 3000) {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xc1\xd7\xc0\xbd\xc0\xc7\xc5\xbe""480_2.jpg");
		} else {
			sprintf(g_Name, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xc1\xd7\xc0\xbd\xc0\xc7\xc5\xbe""480_3.jpg");
		}
	}

	return g_Name;
}
