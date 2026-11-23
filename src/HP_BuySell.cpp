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
			sprintf(msg, "HELP : Match shapes to fill a line");
			actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
			sprintf(msg, "Clear 10 lines to forge a weapon.");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffc8c8, 0, 0);
			sprintf(msg, "Lines of one shape clear, and a clearing line infects its neighbors");
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
			sprintf(msg, "%s's Dad's Shop", g_session.GetCharName());
			actor->SendMsg(NULL, AM_MAKE_SHOP_TITLE, (int)msg, 0, 0);
			actor->SetState(ST_SIT);
			g_BS_NextActTime = timeGetTime() + ((rand() % 20 + 20) * 1000);
			if (game_ShopOpenNumber <= F1) {
				sprintf(msg, "HINT : Customers come after about 1 minute.");
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
				sprintf(msg, "How much for %s?", iName);
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
			sprintf(msg, "%d Zeny", price);
			merchant->SendMsg(NULL, AM_SET_HEAD_DIR, 2, 0, 0);
			merchant->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
			merchant->SendMsg(NULL, AM_EMOTION, 8, 0, 0);
		} else if (g_BS_phase == 5) {
			if (g_BS_NextActTime < timeGetTime()) {
				itemInfo = g_session.GetItemInfoByIndex(g_selected_item);
				++g_BS_phase;
				if (g_selected_item_ID != itemInfo.GetItemId()) {
					sprintf(msg, "Suddenly I don't want to shop..");
					buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					return;
				} else if (g_menuFreeze == 0) {
					g_BS_NextActTime = timeGetTime() + 3000;
					if (buyer->GetJob() == JT_THIEF) {
						sprintf(msg, "Hehehe, your mind's elsewhere. I'll just take this");
						buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						buyer->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
						g_session.DecItem(g_selected_item, 1);
					} else {
						sprintf(msg, "No answer... guess he won't sell");
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
							sprintf(msg, "Thanks for selling it cheap T_T Now I can go back to Morroc ^^;");
							buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
							buyer->SendMsg(NULL, AM_EMOTION, 0x12, 0, 0);
							price = itemInfo.m_price * 0.5;
							g_session.DecItem(g_selected_item, 1);
							PlayWave("effect\\rog_steal coin.wav");
							g_session.SetGold(g_session.GetGold() + price);
							g_curStory = 43;
						} else {
							sprintf(msg, "I'm short on money T_T Isn't there a shop that sells cheap.. sob");
							buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
							buyer->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
						}
					} else if (PASS == 1) {
						sprintf(msg, "I'll take it~");
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
						sprintf(msg, "I'll be back when it's cheaper");
						buyer->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						buyer->SendMsg(NULL, AM_EMOTION, 6, 0, 0);
					} else {
						sprintf(msg, "Too expensive;");
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
				sprintf(msg, "Teleport Merchant here ^_^  What would you like?");
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
				sprintf(msg, "ATK %d %s, just %d Zeny~",
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
				sprintf(msg, "Today I brought %s, how about %d Zeny?",
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
				sprintf(msg, "Got no money -_- and you wanna buy?");
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
			sprintf(msg, "%d Zeny, thank you~", cost);
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
				sprintf(msg, "Today only, half price: %d Zeny ^^;", cost);
				actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 0x12, 0, 0);
				g_BS_phase = 0x14;
				g_BS_NextActTime = timeGetTime() + 3000;
				return;
			}

			sprintf(msg, "Any lower and we'll go broke.");
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
	sprintf(msg, "Teleport Shop call-out fee: %d Zeny ^_^", 100);
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
	sprintf(msg, "You called me, so talk... I'm keeping the %d Zeny fee -_-", 1000);
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
