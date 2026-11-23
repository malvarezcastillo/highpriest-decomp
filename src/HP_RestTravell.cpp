#include "common.h"
#include "device/Sound.h"
#include "resource/Wave.h"
#include "window/UIFrameWnd.h"
#include "GameMode.h"
#include "Session.h"
#include "GameActor.h"
#include "GameWorld.h"
#include "Mode.h"
#include "base/Res.h"
#include "device/Control.h"
#include "3dlib/Renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include "resource/Ground.h"
#include "pdb_shared.h"

extern CSession g_session;
extern CModeMgr g_modeMgr;
extern int g_curScreen;
extern short g_ScreenConv_Alpha;
extern short map_Number;
extern CMouse g_mouse;
extern int g_curStance;
extern short g_menuFreeze;
extern short g_curStory;
extern unsigned long g_MainNextActTime;
extern char g_TravelMap_Count;
extern int g_Frame;
extern HP_TRAVEL_AVAILABLE HP_TravelMap[];
extern short map_firstway;
extern short map_secondstart;
extern short map_secondway;
extern short map_thirdstart;
extern short map_thirdway;
extern short g_PrevMob;
extern short map_Heal_Used;
extern short map_ComboF_Used;
extern short game_TetrisPlayed;
extern bool game_ItemRentEvent;
extern short g_BS_Job;
extern UIWindowMgr g_windowMgr;
extern short g_Aco_Job;
extern bool Boy_frame;
extern short Boy_talked;
extern short Boy_friendly;
extern char g_curEXPmode;
extern short Boy_Job;
extern char g_curHardness;

#include "rag_budget_sites.h"
void UIWindowMgr::HP_Resting()
{
	ITEM_INFO itemInfo; RAG_BUDGET_SITES(1)
	CGameActor* actor = g_session.GetGameActorByAid(1);
	CGameActor* bsActor = g_session.GetGameActorByAid(2);
	int i;
	int count;

	if (g_curStance == 0) {
		if (g_session.m_DayCount >= 5 && !game_ItemRentEvent) {
			game_ItemRentEvent = true;
			g_curStance = 10;
			return;
		}

		if (g_session.GetGold() <= -10000 && game_TetrisPlayed >= 100 && g_BS_Job == JT_BLACKSMITH) {
			g_curStance = 20;
			return;
		}

		if (g_curStory == 11) {
			g_curStory = 12;
		}
		if (g_curStory == 30) {
			g_curStory = 31;
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Dear, if you ever get frozen, shake the mouse left and right to thaw faster.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
		} else if (g_PrevMob == JT_DEVILING&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"At level 55 you can endure Deviling's SP Breaker.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_TURTLE_GENERAL&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Turtle General leaves after a while if you don't attack it.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_ALIZA&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Aliza heals herself whenever you use Heal.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_BANSHEE&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Banshee casts Thunder Storm right after Aco Kaboom.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_GARM_BABY&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Emote with Cure Poison and Hatii Bebe uses Aid Potion on you.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_GARM&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Hatii's Jupitel Thunder stuns you 20% of the time..", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_ICE_TITAN&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Ice Titan uses Eske whenever you use Heal.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_MOONLIGHT&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Moonlight Flower casts Meteor Storm if you use Aco Kaboom.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_MOROCC&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"You must be level 84 to face this boss.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_MAYA_PUPLE&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Maya Purple's Lord of Vermilion varies with its target's level.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_PHREEONI&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Phreeoni Endures your Fury and Frost Dives when you Heal.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_ORK_HERO&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Orc Hero answers Aco Kaboom with Hammer Fall.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_OWL_BARON&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Owl Baron attacks with Fire Wall if you Heal.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_ATROCE&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Atroce is weak to Aco Kaboom, so be sure to use it.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_KIEL&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Hit Kiel D-01 with Aco Kaboom mid-cast to cancel its Guillotine Fist.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_GOPINICH&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Against Fire Wall, keep the item window open and double-click your weapon to re-equip.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_LORD_OF_DEATH&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"When Lord of the Dead readies a counter, use Heal or Cure instead.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_RANDGRIS&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Valkyrie Randgris heals too when you Heal. Fight defensively.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_CHIMERA&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Chimera uses Eske once it has taken enough damage.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (g_PrevMob == JT_THANATOS&& actor->m_Hp <= 1) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Remember: Thanatos tends to use Guillotine Fist every few turns.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (map_Heal_Used == 0 && actor->m_Hp <= 1) {
			actor->SendMsg(NULL, AM_CHAT, (int)"Use Heal, Dad -_-+ Open the skill window and right-click to read the skill info.", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 6, 0, 0);
		} else if (map_ComboF_Used == 0 && actor->m_Hp <= 1) {
			actor->SendMsg(NULL, AM_CHAT, (int)"Fights are hard without Aco Kaboom T_T", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else if (actor->m_Hp <= 1) {
			actor->SendMsg(NULL, AM_CHAT, (int)"Hunt more monsters to level up and we can beat the boss~ ^^*", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
		} else if (actor->m_Hp == actor->m_MaxHp && actor->m_Sp == actor->m_MaxSp) {
			actor->SendMsg(NULL, AM_CHAT, (int)"HP and SP are full.. why rest? T_T What a waste of money", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		} else {
			actor->SendMsg(NULL, AM_CHAT, (int)"I'll do my best tomorrow too~ Earn lots of money, Dad ^^*", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
		}
		g_MainNextActTime = timeGetTime() + 1000;
		++g_curStance;
	} else if (g_curStance == 1) {
		if (g_MainNextActTime <= timeGetTime()) {
			actor->SendMsg(NULL, AM_MAKE_NUMBER, actor->m_MaxHp, 0xff00ff00, 114);
			actor->LaunchEffect(331);
			actor->m_Hp = actor->m_MaxHp;
			actor->SetAttrState(0, 0, 0, true);
			g_session.SetStatusData(actor);
			g_MainNextActTime = timeGetTime() + 500;
			++g_curStance;
		}
	} else if (g_curStance == 2) {
		if (g_MainNextActTime <= timeGetTime()) {
			actor->SendMsg(NULL, AM_MAKE_NUMBER, actor->m_MaxSp, 0xff0000ff, 114);
			actor->LaunchEffect(332);
			actor->m_Sp = actor->m_MaxSp;
			g_session.SetStatusData(actor);
			g_MainNextActTime = timeGetTime() + 500;
			++g_curStance;
		}
	} else if (g_curStance == 3) {
		if (g_MainNextActTime <= timeGetTime()) {
			HP_SaveGame(1);
			HP_DayPassing();
			g_MainNextActTime = timeGetTime() + 2000;
			++g_curStance;
		}
	} else if (g_curStance == 4) {
		if (g_MainNextActTime <= timeGetTime()) {
			SetcurScreen(1);
		}
	}

	if (g_curStance == 10) {
		bsActor->SendMsg(NULL, AM_CHAT, (int)"Hmm.. your borrowed gear has to go back to my friend.", 0, 0);
		bsActor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
		g_MainNextActTime = timeGetTime() + 2500;
		++g_curStance;
	} else if (g_curStance == 11) {
		if (g_MainNextActTime <= timeGetTime()) {
			actor->SendMsg(NULL, AM_CHAT, (int)"Aww.. my Santa Poring Hat T_T Elven Ears T_T Romantic Flower T_T", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
			g_MainNextActTime = timeGetTime() + 2500;
			++g_curStance;
		}
	} else if (g_curStance == 12) {
		if (g_MainNextActTime <= timeGetTime()) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"I'm sorry, but if we work a bit harder we'll get even better items.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
			g_MainNextActTime = timeGetTime() + 2500;
			++g_curStance;
		}
	} else if (g_curStance == 13) {
		if (g_MainNextActTime <= timeGetTime()) {
			actor->SendMsg(NULL, AM_CHAT, (int)"OK T_T", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
			g_MainNextActTime = timeGetTime() + 2500;
			g_curStance = 1;

			itemInfo = g_session.GetEquipedItems(8);
			if (atoi(itemInfo.m_itemName.c_str()) == 5381) {
				g_session.MoveItemFromEquipWndToItemWnd(itemInfo.m_location);
			}
			itemInfo = g_session.GetEquipedItems(9);
			if (atoi(itemInfo.m_itemName.c_str()) == 2286) {
				g_session.MoveItemFromEquipWndToItemWnd(itemInfo.m_location);
			}
			itemInfo = g_session.GetEquipedItems(0);
			if (atoi(itemInfo.m_itemName.c_str()) == 2269) {
				g_session.MoveItemFromEquipWndToItemWnd(itemInfo.m_location);
			}

			count = 3;
			while (count) {
				short numItem = g_session.GetNumItem();
				for (i = 0; i < numItem; ++i) {
					itemInfo = g_session.GetItemInfoBySequence(i);
					if (atoi(itemInfo.m_itemName.c_str()) == 5381
						|| atoi(itemInfo.m_itemName.c_str()) == 2286
						|| atoi(itemInfo.m_itemName.c_str()) == 2269) {
						g_session.DecItem(itemInfo.m_itemIndex, 1);
						break;
					}
				}
				--count;
			}
			PlayWave("effect\\_blind.wav");
		}
	}

	if (g_curStance == 20) {
		bsActor->SendMsg(NULL, AM_CHAT, (int)"Your father finally became a Mastersmith.", 0, 0);
		bsActor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
		g_MainNextActTime = timeGetTime() + 2500;
		++g_curStance;
	} else if (g_curStance == 21) {
		if (g_MainNextActTime <= timeGetTime()) {
			g_BS_Job = 4011;
			bsActor->SetJob(4011);
			bsActor->LaunchEffect(158);
			g_MainNextActTime = timeGetTime() + 2500;
			++g_curStance;
		}
	} else if (g_curStance == 22) {
		if (g_MainNextActTime <= timeGetTime()) {
			bsActor->SendMsg(NULL, AM_CHAT, (int)"Now I can forge faster, so we'll be a bit better off.", 0, 0);
			bsActor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
			g_MainNextActTime = timeGetTime() + 2500;
			g_curStance = 1;
		}
	}
}

void UIWindowMgr::HP_DayPassing()
{
	CGameActor* actor = g_session.GetGameActorByAid(1);
	if (actor) {
		char msg[256];
		short cost;
		if (actor->m_Hp <= 1) {
			cost = 2000;
			sprintf(msg, "   Spent %d Zeny on treatment   ", cost);
		} else {
			cost = 1000;
			sprintf(msg, "   Living costs took %d Zeny   ", cost);
		}
		PlayWave("effect\\rog_steal coin.wav");
		++g_session.m_DayCount;
		g_session.SetGold(g_session.GetGold() - cost);
		g_modeMgr.GetGameMode()->Zc_BroadCast_HP((int)msg);
	}
}

void UIWindowMgr::RenderTravelMap()
{
	int i;
	char temp[256];
	char helpText[50];
	RECT rect = {0, 0, 0xff, 0xff};
	int alpha = 0xff;

	if (g_curStance == 0) {
		g_ScreenConv_Alpha += 5;
		if (g_ScreenConv_Alpha >= 0xff) {
			g_ScreenConv_Alpha = 0xff;
			g_curStance = 1;
			g_TravelMap_Count = 0;

			if (g_menuFreeze == 3) {
				if (g_curStory >= 0) {
					HP_TravelMap[g_TravelMap_Count].X = 696;
					HP_TravelMap[g_TravelMap_Count].Y = 471;
					HP_TravelMap[g_TravelMap_Count].Map = 1;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 41) {
					HP_TravelMap[g_TravelMap_Count].X = 518;
					HP_TravelMap[g_TravelMap_Count].Y = 518;
					HP_TravelMap[g_TravelMap_Count].Map = 2;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 51) {
					HP_TravelMap[g_TravelMap_Count].X = 537;
					HP_TravelMap[g_TravelMap_Count].Y = 147;
					HP_TravelMap[g_TravelMap_Count].Map = 3;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 92) {
					HP_TravelMap[g_TravelMap_Count].X = 285;
					HP_TravelMap[g_TravelMap_Count].Y = 364;
					HP_TravelMap[g_TravelMap_Count].Map = 4;
					++g_TravelMap_Count;
				}
				sprintf(temp, "<<   Click where you want to go   >>");
			} else if (g_menuFreeze == 1) {
				if (g_curStory >= 0) {
					HP_TravelMap[g_TravelMap_Count].X = 450;
					HP_TravelMap[g_TravelMap_Count].Y = 360;
					HP_TravelMap[g_TravelMap_Count].Map = 10;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 10) {
					HP_TravelMap[g_TravelMap_Count].X = 715;
					HP_TravelMap[g_TravelMap_Count].Y = 440;
					HP_TravelMap[g_TravelMap_Count].Map = 11;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 40) {
					HP_TravelMap[g_TravelMap_Count].X = 600;
					HP_TravelMap[g_TravelMap_Count].Y = 300;
					HP_TravelMap[g_TravelMap_Count].Map = 12;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 50) {
					HP_TravelMap[g_TravelMap_Count].X = 798;
					HP_TravelMap[g_TravelMap_Count].Y = 575;
					HP_TravelMap[g_TravelMap_Count].Map = 13;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 60) {
					HP_TravelMap[g_TravelMap_Count].X = 624;
					HP_TravelMap[g_TravelMap_Count].Y = 133;
					HP_TravelMap[g_TravelMap_Count].Map = 14;
					++g_TravelMap_Count;
				}
				if (g_curStory >= 90) {
					HP_TravelMap[g_TravelMap_Count].X = 555;
					HP_TravelMap[g_TravelMap_Count].Y = 96;
					HP_TravelMap[g_TravelMap_Count].Map = 15;
					++g_TravelMap_Count;
				}
				sprintf(temp, "<<   Click where you want to train   >>");
			}
			g_modeMgr.GetGameMode()->Zc_BroadCast_HP((int)temp);
		}
		alpha = g_ScreenConv_Alpha;
	} else if (g_curStance == 2) {
		g_ScreenConv_Alpha += 10;
		if (g_ScreenConv_Alpha >= 0xff) {
			g_ScreenConv_Alpha = 0xff;
			g_curStance = 3;
		}
		alpha = 0xff - g_ScreenConv_Alpha;
	} else if (g_curStance == 3) {
		if (g_menuFreeze == 1) {
			SetcurScreen(10);
		} else if (g_menuFreeze == 3) {
			SetcurScreen(3);
		}
		alpha = 0;
	}

	sprintf(temp, "effect\\white02.bmp");
	rect.left = 180;
	rect.top = 80;
	rect.right = 860;
	rect.bottom = 600;
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.01f,
		(alpha << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0,0,1,1));

	sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\worldmap.bmp");
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.01f,
		(((alpha * 136) / 256) << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0,0,1,1));
	rect.left = 190;
	rect.top = 90;
	rect.right = 850;
	rect.bottom = 590;
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.01f,
		(alpha << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0,0,1,1));

	if (g_curStance == 1) {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xbf\xa5\xba\xea\xb8\xae\xbf\xc0.bmp");
		for (i = 0; i < g_TravelMap_Count; ++i) {
			short pulse = (short)(g_Frame % 110);

			if (pulse < 100) {
				short pulseAlpha = 60;
				if (pulse >= 40) {
					pulseAlpha = 100 - pulse;
					if (pulseAlpha < 0) {
						pulseAlpha = 0;
					}
				}

				rect.left = HP_TravelMap[i].X - pulse - 6;
				rect.top = HP_TravelMap[i].Y - pulse - 6;
				rect.right = rect.left + pulse * 2 + 12;
				rect.bottom = rect.top + pulse * 2 + 12;
				g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.01f,
					((pulseAlpha / 2) << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0,0,1,1));

				short smallPulse = (short)((float)pulse * 0.6f);
				rect.left = HP_TravelMap[i].X - smallPulse - 6;
				rect.top = HP_TravelMap[i].Y - smallPulse - 6;
				rect.right = rect.left + smallPulse * 2 + 12;
				rect.bottom = rect.top + smallPulse * 2 + 12;
				g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.01f,
					((pulseAlpha / 4) << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0,0,1,1));
			}

			rect.left = HP_TravelMap[i].X - 6;
			rect.top = HP_TravelMap[i].Y - 6;
			rect.right = rect.left + 12;
			rect.bottom = rect.top + 12;
			g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.01f,
				(((alpha * 240) / 256) << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0,0,1,1));

			if (g_mouse.GetXPos() >= HP_TravelMap[i].X - 12 && g_mouse.GetXPos() <= HP_TravelMap[i].X + 12
				&& g_mouse.GetYPos() >= HP_TravelMap[i].Y - 12 && g_mouse.GetYPos() <= HP_TravelMap[i].Y + 12) {
				if (HP_TravelMap[i].Map == 1) {
					sprintf(helpText, "Payon");
				} else if (HP_TravelMap[i].Map == 2) {
					sprintf(helpText, "Morroc");
				} else if (HP_TravelMap[i].Map == 3) {
					sprintf(helpText, "Juno");
				} else if (HP_TravelMap[i].Map == 4) {
					sprintf(helpText, "Veins");
				} else if (HP_TravelMap[i].Map == 10) {
					sprintf(helpText, "Geffen Field");
				} else if (HP_TravelMap[i].Map == 11) {
					sprintf(helpText, "Payon Cave");
				} else if (HP_TravelMap[i].Map == 12) {
					sprintf(helpText, "Mt. Mjolnir");
				} else if (HP_TravelMap[i].Map == 13) {
					sprintf(helpText, "Turtle Island");
				} else if (HP_TravelMap[i].Map == 14) {
					sprintf(helpText, "Abyss Lake");
				} else if (HP_TravelMap[i].Map == 15) {
					sprintf(helpText, "Death Tower");
				}

				char textX = (char)((float)(char)strlen(helpText) * 3.5f);
				g_modeMgr.GetCurMode()->MakeHelpBalloon(helpText, HP_TravelMap[i].X - textX, HP_TravelMap[i].Y - 28, 0xffffffff);

				if (g_mouse.GetLBtn() == BTN_DOWN) {
					map_Number = HP_TravelMap[i].Map;
					++g_curStance;
					g_ScreenConv_Alpha = 0;

					if (map_Number == 10) {
						map_firstway = 7;
						map_secondstart = 4;
						map_secondway = 5;
						map_thirdstart = 7;
						map_thirdway = 8;
					} else if (map_Number == 11) {
						map_firstway = 8;
						map_secondstart = 3;
						map_secondway = 12;
						map_thirdstart = 5;
						map_thirdway = 5;
					} else if (map_Number == 12) {
						map_firstway = 11;
						map_secondstart = 2;
						map_secondway = 11;
						map_thirdstart = 4;
						map_thirdway = 11;
					} else if (map_Number == 13) {
						map_firstway = 15;
						map_secondstart = 4;
						map_secondway = 9;
						map_thirdstart = 8;
						map_thirdway = 7;
					} else if (map_Number == 14) {
						map_firstway = 13;
						map_secondstart = 1;
						map_secondway = 13;
						map_thirdstart = 2;
						map_thirdway = 13;
					} else if (map_Number == 15) {
						map_firstway = 10;
						map_secondstart = 2;
						map_secondway = 8;
						map_thirdstart = 4;
						map_thirdway = 11;
					}
				}
			}
		}
	}
}

void UIWindowMgr::RenderBackGround2D_Travel()
{
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};
	int alpha = 0xff;

	if (g_curScreen == 4) {
		alpha = g_ScreenConv_Alpha;
	} else if (g_curScreen == 6) {
		alpha = 0xff - g_ScreenConv_Alpha;
	}

	sprintf(temp, "effect\\white02.bmp");
	rect.left = 190;
	rect.top = 90;
	rect.right = 850;
	rect.bottom = 590;
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		(alpha << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	if (map_Number == 1) {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\HP_Payon.bmp");
	} else if (map_Number == 2) {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\HP_Morocc.bmp");
	} else if (map_Number == 3) {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\HP_Yuno.bmp");
	} else if (map_Number == 4) {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\HP_Veins.bmp");
	} else {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xd4\xc7\xc1\xc7\xca\xb5\xe5""480.jpg");
	}

	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		(((alpha * 136) / 256) << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	rect.left = 200;
	rect.top = 100;
	rect.right = 840;
	rect.bottom = 580;
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		(alpha << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
}


void UIWindowMgr::Aco_Travel()
{
	ITEM_INFO itemInfo;
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	CGameActor* actor = g_session.GetGameActorByAid(1);
	char msg[256];
	short waitTime = 2500;

	if (g_MainNextActTime < timeGetTime()) {


		short map;
		if (g_curStance == 0) {
			g_menuFreeze = 0;
			map = map_Number;
		} else {
			map = map_Number;
		}

		if (map == 1) {
			CGameActor* boy = g_session.GetGameActorByAid(3);
			if (g_curStance == 0) {
				if (g_Aco_Job != JT_PRIEST_H) {
					gameMode->Zc_Notify_Entry_HP(3, 18, -3);
					waitTime = 1000;
					Boy_frame = false;
				}

				if (g_curStory == 71 && g_session.DoPCHaveItem(2266)) {
					if (g_session.DoPCHaveItem(1219))
						g_curStory = 72;
					if (g_session.DoPCHaveItem(1223))
						g_curStory = 72;
					if (g_session.DoPCHaveItem(1234))
						g_curStory = 72;
				}
			} else if (g_Aco_Job == JT_PRIEST_H) {
				if (g_curStance == 1) {
					sprintf(msg, "I miss..  him..");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else {
					SetcurScreen(6);
				}
			} else if (Boy_talked == 0) {
				if (g_curStance == 1) {
					sprintf(msg, "Hello~ Is this your first time in Payon?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Yes ^_^ You must come here often?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "I usually rest here ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "I'd better go level up again. See you~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "OK, bye bye~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					SetcurScreen(6);
					Boy_talked = 1;
				}
			} else if (Boy_talked == 2) {
				if (g_curStance == 1) {
					sprintf(msg, "Hello~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "You helped me in Payon Cave?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "I went to train and saw you there ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "I'm %s~ I hope we get along ^^;", g_session.GetCharName());
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "I'm Kakarot, a Swordman out to protect the world. Hehe~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you again~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					SetcurScreen(6);
					Boy_talked = 3;
				}
			} else if (Boy_talked == 3 && g_curStory == 20) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "Try the 2nd and 3rd areas of Geffen Field.");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "Hmm...");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "Just the right leveling spot for you now, %s", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "OK ^^;");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else {
					Boy_talked = 4;
					SetcurScreen(6);
				}
			} else if (g_curStory == 40) {
				if (g_curStance == 1) {
					sprintf(msg, "%s, have you been to Mt. Mjolnir?", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "An Incubus shows up on Mt. Mjolnir and warps people away...");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "Go to Morroc and talk to the Soul Linker there.");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "Morroc? OK~ I'll go there ^^;");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
				} else {
					g_curStory = 41;
					SetcurScreen(6);
				}
			} else if (g_curStory == 50) {
				if (g_curStance == 1) {
					sprintf(msg, "%s, I hear you're leveling on Turtle Island?", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "If you meet a Wooden Golem there, you'll need a Monk's help~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 4000;
				} else if (g_curStance == 3) {
					sprintf(msg, "A Monk in Juno has mastered Occult Impaction.. go see him ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 4000;
				} else if (g_curStance == 4) {
					sprintf(msg, "Yes, I've always wanted to visit Juno anyway~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
				} else {
					g_curStory = 51;
					SetcurScreen(6);
				}
			} else if (g_curStory == 90) {
				if (g_curStance == 1) {
					sprintf(msg, "Thanatos has finally revived in the Death Tower");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Gasp");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "Thanatos went for Alberta first.. I went with the knights to stop him, but...");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "Thanatos was too strong.. all we could do was flee by portal... ");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 5) {
					sprintf(msg, "In the end all of Alberta was destroyed...");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "Alberta? T_T");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 7) {
					sprintf(msg, "Our hope now: find the Valkyrie and make %s a High Priest", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 4500;
				} else if (g_curStance == 8) {
					sprintf(msg, "Would the Valkyrie really approve me?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 9) {
					sprintf(msg, "Thanatos made the Valkyrie flee somewhere.. once we find where, it should be fine.");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 4500;
				} else if (g_curStance == 10) {
					sprintf(msg, "Not even the Priests have anyone as gifted as you, %s.", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 11) {
					sprintf(msg, "I want to be a High Priest");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 12) {
					sprintf(msg, "I'll find the Valkyrie who can make you a High Priest! Just wait");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else {
					g_curStory = 91;
					SetcurScreen(6);
				}
			} else if (g_curStory == 91 || g_curStory == 92) {
				if (g_curStance == 1) {
					sprintf(msg, "I found where the Valkyrie is!");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Wow~ really?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "She seems to be in Veins, to the west");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "But.. her magic is drained right now, so to change your job..");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 5) {
					sprintf(msg, "So I need..?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "No.. I think I can take care of that myself..");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 7) {
					sprintf(msg, "Let's go together!! To Veins! To make you a High Priest!");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 8) {
					sprintf(msg, "Yes! I want to be a High Priest ^^*");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else {
					g_curStory = 92;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 4 && g_curStory >= 30 && g_curStory < 40) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 4;
					sprintf(msg, "Which monster do you like the most?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 2) {
						sprintf(msg, "Porings are so cute ^^;");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "Oh... I see~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 5;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 5 && g_curStory >= 40 && g_curStory < 50) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hello Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 5;
					sprintf(msg, "Which 1st job do you like the most?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 1) {
						sprintf(msg, "Hehe, Swordmen rule, right? ^^;");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "Oh... I see~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 6;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 6 && g_curStory >= 50 && g_curStory < 60) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hello ^^;");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 6;
					sprintf(msg, "Remember my name?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 3) {
						sprintf(msg, "I'm glad you remember ^^;");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "No T_T My name is Kakarot.. sob");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 7;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 7 && g_curStory >= 60 && g_curStory < 70) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 7;
					sprintf(msg, "What would you like for your birthday?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 2) {
						sprintf(msg, "Romantic Flower suits a lady well ^^;");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "Oh... I see~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 8;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 8 && g_curStory >= 70 && g_curStory < 80) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 8;
					sprintf(msg, "How do you feel about your hardworking Blacksmith dad?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 1) {
						sprintf(msg, "He must be worn out earning money");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "Oh... I see~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 9;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 9 && g_curStory >= 70 && g_curStory < 80) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 9;
					sprintf(msg, "If you could get rid of just one monster, which would you kill?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 2) {
						sprintf(msg, "Right. If Thanatos isn't stopped, Prontera is doomed");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "Oh... I see~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 10;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 10 && g_curStory >= 80 && g_curStory < 90) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 10;
					sprintf(msg, "Which companion helped you the most?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 3) {
						sprintf(msg, "Hehehe~ thank you ^^*");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "Oh... I see~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 11;
					SetcurScreen(6);
				}
			} else if (Boy_talked <= 11 && g_curStory >= 80 && g_curStory < 90) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					Boy_frame = true;
					Boy_talked = 11;
					sprintf(msg, "Which do you think matters more?");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 10000;
				} else if (g_curStance == 4) {
					if (g_menuFreeze == 1) {
						sprintf(msg, "So that's what you think. I'm not so sure T_T");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						++Boy_friendly;
					} else {
						sprintf(msg, "Hmmmm... ..");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					}
				} else if (g_curStance == 5) {
					sprintf(msg, "Hehe.. I have to go now~ See you again");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "See you~ ^^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					Boy_talked = 12;
					SetcurScreen(6);
				}
			} else if (g_curStory == 70) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi %s~", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xb1\xe2\xbb\xe7\xb7\xce \xc0\xfc\xc1\xf7\xc0\xbb \xc7\xcf\xb7\xc1\xb0\xed \xc7\xcf\xb4\xc2\xb5\xa5.. \xc0\xfc\xc1\xf7\xc7\xcf\xb7\xc1\xb8\xe9 \xb8\xee\xb8\xee \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xc7\xca\xbf\xe4\xc7\xcf\xb3\xd7\xbf\xe4...");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xbe\xee\xb6\xb2 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc\xbf\xb9\xbf\xe4? \xc1\xa6\xb0\xa1 \xb1\xb8\xc7\xd2\xbc\xf6\xc0\xd6\xb4\xc2\xb0\xc5\xb8\xe9 \xc0\xfa\xb5\xb5 \xb5\xb5\xbf\xcd\xb5\xe5\xb8\xb1\xb2\xb2\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xc1\xf6\xb1\xdd \xb3\xb2\xc0\xba\xb0\xd4 \xbe\xc6\xc0\xcc\xbe\xf0\xc4\xc9\xc0\xce \xc7\xcf\xb0\xed \xc1\xc1\xc0\xba \xb0\xcb \xc0\xcc\xb3\xd7\xbf\xe4.");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 6) {
					sprintf(msg, "\xc8\xec.. \xb1\xb8\xc7\xcf\xb0\xd4 \xb5\xc7\xb8\xe9 \xb0\xa1\xc1\xae\xb4\xd9\xb5\xe5\xb8\xb1\xb2\xb2\xbf\xe4 ^^;");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 7) {
					sprintf(msg, "\xb3\xd7 \xba\xce\xc5\xb9\xb5\xe5\xb7\xc1\xbf\xe4 \xa4\xd0_\xa4\xd0");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else {
					g_curStory = 71;
					SetcurScreen(6);
				}
			} else if (g_curStory == 71) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xbe\xc6\xc0\xcc\xbe\xf0\xc4\xc9\xc0\xce \xc7\xcf\xb0\xed \xc1\xc1\xc0\xba \xb0\xcb \xbe\xc6\xc1\xf7 \xb8\xf8\xb1\xb8\xc7\xdf\xbe\xee\xbf\xe4 \xa4\xd0_\xa4\xd0");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xc8\xe6.. \xc0\xfa\xb5\xb5 \xb1\xb8\xc7\xcf\xb0\xed\xc0\xd6\xb4\xc2\xb5\xa5 \xc0\xdf \xbe\xca\xb1\xb8\xc7\xd8\xc1\xf6\xb3\xd7\xbf\xe4 \xa4\xd0_\xa4\xd0");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xc0\xcc\xb4\xeb\xb7\xce \xb0\xe8\xbc\xd3 \xb0\xcb\xbb\xe7\xb7\xce \xbb\xec\xbe\xc6\xbe\xdf\xc7\xcf\xb3\xaa orz");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xa4\xd0_\xa4\xd0");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else {
					SetcurScreen(6);
				}
			} else if (g_curStory == 72) {
				if (g_curStance == 1) {
					sprintf(msg, "Hi Kakarot~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xbe\xc6\xc0\xcc\xbe\xf0\xc4\xc9\xc0\xce \xc7\xcf\xb0\xed \xc1\xc1\xc0\xba \xb0\xcb \xbf\xa9\xb1\xe2 \xb1\xb8\xc7\xdf\xbe\xee\xbf\xe4");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xc1\xa4\xb8\xbb \xb0\xed\xb8\xb6\xbf\xf6\xbf\xe4 ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);

					short numItem = g_session.GetNumItem();
					short i;
					for (i = 0; i < numItem; ++i) {
						itemInfo = g_session.GetItemInfoBySequence(i);
						if (atoi(itemInfo.m_itemName.c_str()) == 1219
							|| atoi(itemInfo.m_itemName.c_str()) == 1223
							|| atoi(itemInfo.m_itemName.c_str()) == 1234) {
							char msg[256];
							char itemName[64];
							itemInfo.GetItemName(itemName);
							sprintf(msg, "%s\xb8\xa6 \xc4\xab\xc4\xab\xb7\xce\xc6\xae\xbf\xa1\xb0\xd4 \xc0\xfc\xb4\xde\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9", itemName);
							g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffff00, 0, 0);
							g_session.DecItem(itemInfo.m_itemIndex, 1);
							break;
						}
					}

					numItem = g_session.GetNumItem();
					for (i = 0; i < numItem; ++i) {
						itemInfo = g_session.GetItemInfoBySequence(i);
						if (atoi(itemInfo.m_itemName.c_str()) == 2266) {
							char msg[256];
							char itemName[64];
							itemInfo.GetItemName(itemName);
							sprintf(msg, "%s\xb8\xa6 \xc4\xab\xc4\xab\xb7\xce\xc6\xae\xbf\xa1\xb0\xd4 \xc0\xfc\xb4\xde\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9", itemName);
							g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffff00, 0, 0);
							g_session.DecItem(itemInfo.m_itemIndex, 1);
							break;
						}
					}
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xb1\xd7\xb7\xb3 \xc0\xfc \xc0\xfc\xc1\xf7\xc7\xcf\xb7\xaf \xb0\xa1\xba\xbc\xb2\xb2\xbf\xe4 ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xb3\xdc~ \xb8\xda\xc1\xf8 \xb1\xe2\xbb\xe7 \xb5\xc7\xbc\xbc\xbf\xe4 ^^*");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else {
					if (g_session.m_DayCount < 130 && g_curEXPmode == 1) {
						g_curStory = 74;
						Boy_Job = 4008;
					} else {
						g_curStory = 73;
						Boy_Job = 7;
					}
					SetcurScreen(6);
				}
			} else if (g_curStory == 73) {
				if (g_curStance == 1) {
					sprintf(msg, "%s\xb4\xd4, \xc0\xfa \xb1\xe2\xbb\xe7\xb7\xce \xc0\xfc\xc1\xf7\xc7\xdf\xbe\xee\xbf\xe4", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 2) {
					sprintf(msg, "\xb8\xda\xc1\xae\xbf\xe4 ^^; \xc3\xe0\xc7\xcf\xb5\xe5\xb7\xc1\xbf\xe4");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 3) {
					sprintf(msg, "\xc7\xec\xc7\xec.. \xc0\xcc\xc1\xa6 \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb0\xa1 \xba\xce\xc8\xb0\xc7\xdf\xc0\xbb\xb6\xa7 \xc8\xb0\xbe\xe0\xc7\xcf\xb1\xe2\xb8\xb8 \xc7\xcf\xb8\xe9 \xb5\xc9\xb0\xc5 \xb0\xb0\xbe\xc6\xbf\xe4");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xc8\xad\xc0\xcc\xc6\xc3 ^^*");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else {
					g_curStory = 75;
					SetcurScreen(6);
				}
			} else if (g_curStory == 74) {
				if (g_curStance == 1) {
					sprintf(msg, "%s\xb4\xd4, \xc0\xfa \xb7\xce\xb5\xe5\xb3\xaa\xc0\xcc\xc6\xae\xb7\xce \xc0\xfc\xc1\xf7\xc7\xdf\xbe\xee\xbf\xe4", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 2) {
					sprintf(msg, "\xbf\xec\xbf\xcd.. \xb7\xce\xb5\xe5\xb3\xaa\xc0\xcc\xc6\xae\xb4\xc2 \xbe\xc6\xb9\xab\xb3\xaa \xc0\xfc\xc1\xf7 \xb8\xf8\xc7\xcf\xb4\xc2\xb0\xc9\xb7\xce \xbe\xcb\xb0\xed\xc0\xd6\xbe\xfa\xb4\xc2\xb5\xa5.. ");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 3) {
					sprintf(msg, "\xc0\xfc\xc1\xf7\xbd\xc3\xc7\xe8\xb0\xfc\xc0\xcc \xc0\xfa\xc7\xd1\xc5\xd7 \xc0\xe7\xb4\xc9\xc0\xcc \xc0\xd6\xb4\xd9\xb0\xed \xc7\xcf\xbd\xc3\xb4\xf5\xb4\xcf.. \xb7\xce\xb5\xe5\xb3\xaa\xc0\xcc\xc6\xae\xb7\xce \xc0\xfc\xc1\xf7\xc0\xbb \xc7\xcf\xb6\xf3\xb0\xed \xc7\xcf\xbd\xc3\xb4\xf5\xb6\xf3\xb1\xb8\xbf\xe4");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xbf\xcd~ \xc3\xe0\xc7\xcf\xb5\xe5\xb7\xc1\xbf\xe4");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xc0\xcc\xc1\xa6 \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb0\xa1 \xba\xce\xc8\xb0\xc7\xdf\xc0\xbb\xb6\xa7 \xc3\xb3\xb8\xae\xc7\xcf\xb1\xe2\xb8\xb8 \xc7\xcf\xb8\xe9 \xb5\xc9\xb0\xc5\xb0\xb0\xbe\xc6\xbf\xe4");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 6) {
					sprintf(msg, "\xc8\xad\xc0\xcc\xc6\xc3 ^^*");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else {
					g_curStory = 75;
					SetcurScreen(6);
				}
			} else if (g_curStory >= 94) {
				if (g_curStance == 1) {
					sprintf(msg, "%s\xb4\xd4 \xbf\xc0\xbc\xcc\xb3\xd7\xbf\xe4 ^^*", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 2) {
					sprintf(msg, "\xc7\xec\xc7\xec.. \xba\xb8\xb0\xed\xbd\xcd\xbe\xee\xbc\xad \xbf\xd4\xbe\xee\xbf\xe4 ^^*");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 3) {
					sprintf(msg, "%s\xb4\xd4 \xbb\xe7\xb6\xfb\xc7\xd8\xbf\xe4...", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 40, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xbe\xee\xb8\xd3\xb3\xaa... ");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xb1\xe2\xbb\xb5\xbf\xe4 ^^*");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 40, 0, 0);
					waitTime = 3500;
				} else {
					SetcurScreen(6);
				}
			} else {
				if (g_curStance == 1) {
					if (Boy_talked >= 2) {
						sprintf(msg, "%s\xb4\xd4 \xb6\xc7 \xba\xcb\xb3\xd7\xbf\xe4 ^^;", g_session.GetCharName());
					} else {
						sprintf(msg, "\xb6\xc7 \xba\xcb\xb3\xd7\xbf\xe4 ^^;");
					}
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					if (Boy_talked >= 2) {
						sprintf(msg, "\xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4~ \xc1\xb6\xbe\xf0 \xc7\xcf\xb3\xaa \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4~");
					} else {
						sprintf(msg, "\xb3\xd7~ \xc1\xb6\xbe\xf0\xc7\xcf\xb3\xaa \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4~");
					}
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 3) {
					short rnd = rand() % 2;
					if (rnd == 0) {
						sprintf(msg, "\xc0\xfc\xc5\xf5 \xc7\xd2\xb6\xa7\xb4\xc2 \xc8\xfa\xb0\xfa \xbe\xc6\xc4\xda\xc4\xe2\xb1\xa4\xc0\xbb \xc0\xdf \xbe\xb2\xb4\xc2\xb0\xd4 \xc1\xdf\xbf\xe4\xc7\xd8\xbf\xe4");
					} else {
						sprintf(msg, "\xc1\xa6\xc1\xb6\xb8\xa6 \xc7\xd8\xbc\xad \xb9\xab\xb1\xe2\xb8\xa6 \xb8\xb8\xb5\xe7\xb4\xd9\xc0\xbd\xbf\xa1 \xc6\xc7\xb8\xc5\xb8\xa6 \xc7\xd8\xbc\xad \xbb\xfd\xc8\xb0\xba\xf1\xb8\xa6 \xb9\xfa\xbe\xee\xbe\xdf\xc7\xcf\xc1\xd2");
					}
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "\xc1\xb6\xbe\xf0 \xb0\xed\xb8\xb6\xbf\xf6\xbf\xe4 ^^;");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else {
					SetcurScreen(6);
				}
			}

			g_MainNextActTime = timeGetTime() + waitTime;
			++g_curStance;
			if (g_curScreen == 6) {
				if (boy) {
					gameMode->PlaceEffect(&boy->m_pos, 304, 300, 0);
					gameMode->GetWorld()->RemoveGameActor(3);
				}
			}
		} else if (map == 2) {
			short boyAid;
			if (g_curStory > 42)
				boyAid = 5;
			else
				boyAid = 4;

			CGameActor* boy = g_session.GetGameActorByAid(boyAid);
			if (g_curStance == 0) {
				gameMode->Zc_Notify_Entry_HP(boyAid, 18, -3);
				waitTime = 1000;
			} else if (g_curStory == 41) {
				if (g_curStance == 1) {
					sprintf(msg, "\xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~ \xb8\xf0\xb7\xce\xc5\xa9\xbf\xa1 \xbf\xc0\xbd\xc5\xb0\xc9 \xc8\xaf\xbf\xb5\xc7\xd5\xb4\xcf\xb4\xd9");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					boy->SetState(ST_ATTACKWAIT);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xbc\xd2\xbf\xef\xb8\xb5\xc4\xbf\xb4\xd4 \xc0\xce\xc5\xa5\xb9\xf6\xbd\xba\xb8\xa6 \xc3\xb3\xc4\xa1\xc7\xd8\xbe\xdf\xc7\xcf\xb4\xc2\xb5\xa5.. \xb5\xb5\xbf\xcd\xc1\xd6\xbc\xbc\xbf\xe4 \xa4\xd0_\xa4\xd0");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xc0\xfa\xb4\xc2 \xbf\xb5\xc8\xa5\xb0\xe8\xbf\xad\xc0\xc7 \xbd\xba\xc5\xb3\xc0\xbb \xc2\xef\xc1\xf6 \xbe\xca\xbe\xd2\xb4\xe4\xb4\xcf\xb4\xd9; \xc1\xa6 \xb5\xbf\xb7\xe1\xc1\xdf\xbf\xa1 \xb9\xab\xb3\xab\xc0\xcc\xb0\xa1 \xbf\xb5\xc8\xa5\xb0\xe8\xbf\xad \xb8\xb6\xbd\xba\xc5\xcd \xc7\xdf\xb4\xc2\xb5\xa5..");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xc1\xf6\xb1\xdd \xc7\xc1\xb7\xd0\xc5\xd7\xb6\xf3\xbf\xa1 \xb5\xbf\xbb\xfd \xc1\xd9 \xb9\xab\xb1\xe2\xb8\xa6 \xbb\xe7\xb0\xda\xb4\xd9\xb0\xed \xbf\xa9\xc7\xe0\xc0\xbb \xb6\xb0\xb3\xb5\xb3\xd7\xbf\xe4");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 5) {
					sprintf(msg, "\xc7\xc1\xb7\xd0\xc5\xd7\xb6\xf3\xb8\xe9 \xbe\xc6\xba\xfc\xb0\xa1\xb0\xd4\xbf\xa1\xb5\xb5 \xbf\xc3\xbc\xf6\xc0\xd6\xb0\xda\xb3\xd7\xbf\xe4... ^^; \xc1\xa4\xba\xb8 \xb0\xed\xb8\xb6\xbf\xf6\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else {
					g_curStory = 42;
					SetcurScreen(6);
				}
			} else if (g_curStory == 42) {
				if (g_curStance == 1) {
					sprintf(msg, "\xb6\xc7 \xbf\xc0\xbc\xcc\xb3\xd7\xbf\xe4~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					boy->SetState(ST_ATTACKWAIT);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xbc\xd2\xbf\xef\xb8\xb5\xc4\xbf \xb9\xab\xb3\xab\xb4\xd4 \xbe\xc6\xc1\xf7 \xbe\xca\xbf\xc0\xbc\xcc\xb3\xaa\xbf\xe4?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xbf\xa9\xc0\xfc\xc8\xf7 \xc7\xc1\xb7\xd0\xc5\xd7\xb6\xf3\xbf\xa1 \xc0\xd6\xb4\xc2\xb0\xc5 \xb0\xb0\xb3\xd7\xbf\xe4..");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "\xb3\xd7~ \xbe\xc6\xba\xfc \xb0\xa1\xb0\xd4\xbf\xa1 \xbb\xa1\xb8\xae \xbf\xcd\xc1\xe0\xbe\xdf \xc7\xcf\xb4\xc2\xb5\xa5 \xa4\xd0_\xa4\xd0");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xb5\xce\xba\xd0\xc0\xcc \xbb\xa1\xb8\xae \xb8\xb8\xb3\xaa\xbd\xc3\xb1\xe6 \xb9\xd9\xb6\xf6\xb2\xb2\xbf\xe4~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else {
					SetcurScreen(6);
				}
			} else if (g_curStory == 43) {
				if (g_curStance == 1) {
					sprintf(msg, "\xbc\xd2\xbf\xef\xb8\xb5\xc4\xbf \xb9\xab\xb3\xab\xb4\xd4 \xc0\xcc\xbd\xc3\xc1\xd2?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xb3\xd7 \xc1\xa6\xb0\xa1 \xc0\xcc\xbb\xdb\xc0\xcc~ \xb9\xab\xb3\xab\xc0\xcc\xbe\xee\xbf\xe4 ^^*");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xb9\xa6\xb8\xa3\xb4\xd2 \xbb\xea\xb8\xc6\xc0\xc7 \xc0\xce\xc5\xa5\xb9\xf6\xbd\xba \xb6\xa7\xb9\xae\xbf\xa1 \xb1\xd7\xb7\xaf\xb4\xc2\xb5\xa5 \xc1\xbb \xb5\xb5\xbf\xcd\xc1\xd6\xbc\xbc\xbf\xe4 \xa4\xd0_\xa4\xd0");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "\xc8\xe5\xc0\xbd.. 1\xc2\xf7\xc1\xf7\xbe\xf7\xbf\xa1\xb0\xd4 \xbe\xb5\xbc\xf6\xc0\xd6\xb4\xc2 \xbf\xb5\xc8\xa5\xbd\xba\xc5\xb3\xc0\xcc \xbe\xf8\xb1\xe4\xc7\xd1\xb5\xa5.. \xc7\xd1\xb9\xf8 \xba\xb9\xbb\xe7\xb4\xd4\xc0\xc7 \xc0\xe7\xb4\xc9\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8\xba\xbc\xb2\xb2\xbf\xe4");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 5) {
					sprintf(msg, "\xbc\xbc\xbb\xf3\xc0\xc7 \xbf\xb5\xc8\xa5\xb5\xe9\xc0\xcc\xbf\xa9.. \xb1\xd7\xb3\xe0\xbf\xa1 \xb4\xeb\xc7\xd8...");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
					PlayWave("effect\\T_\xb8\xb6\xb9\xfd\xb9\xdd\xbb\xe7.wav");
					actor->LaunchEffect(419);
				} else if (g_curStance == 6) {
					sprintf(msg, "\xb3\xee\xb6\xf8\xb3\xd7\xbf\xe4... \xb8\xf9\xc5\xa9\xc0\xc7 \xbf\xb5\xc8\xa5\xb0\xfa \xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xc0\xc7 \xbf\xb5\xc8\xa5\xb0\xfa 1\xc2\xf7\xbb\xf3\xc0\xa7\xc1\xf7\xbe\xf7\xc0\xc7 \xbf\xb5\xc8\xa5\xc0\xcc \xc0\xcc\xbe\xdf\xb1\xe2 \xc7\xd8\xbf\xc0\xb4\xd9\xb4\xcf..");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
					waitTime = 4500;
				} else if (g_curStance == 7) {
					sprintf(msg, "\xb5\xb5\xbf\xcd\xb5\xe5\xb8\xb1\xbc\xf6 \xc0\xd6\xc0\xbb\xb2\xa8\xb0\xb0\xbe\xc6\xbf\xe4.. \xc1\xa6\xb0\xa1 \xb8\xb8\xb3\xb5\xb4\xf8 \xbb\xe7\xb6\xf7\xb5\xe9\xc1\xdf \xc0\xe1\xc0\xe7\xb4\xc9\xb7\xc2\xc0\xcc \xc1\xa6\xc0\xcf\xc0\xcc\xbd\xc3\xb3\xd7\xbf\xe4~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 4500;
				} else if (g_curStance == 8) {
					sprintf(msg, "\xb0\xed\xb8\xbf\xbd\xc0\xb4\xcf\xb4\xd9 ^^* \xb1\xd7\xb7\xb3 \xb9\xa6\xb8\xa3\xb4\xd2 \xbb\xea\xb8\xc6\xbf\xa1\xbc\xad \xba\xc1\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					actor->SendMsg(NULL, AM_DELETE_EFFECT, 419, 0, 0);
					waitTime = 3500;
				} else {
					g_curStory = 44;
					SetcurScreen(6);
				}
			} else if (actor->m_clevel % 3 == 0) {
				if (g_curStance == 1) {
					sprintf(msg, "\xb9\xab\xb3\xab\xb4\xd4 \xc7\xcf\xc0\xcc\xbf\xe4 ^^/");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "%s\xb4\xd4 \xba\xb0\xc0\xcf \xbe\xf8\xc1\xd2?", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xb3\xd7~~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "\xc0\xfc \xb5\xbf\xbb\xfd \xb9\xe4 \xc7\xd8\xc1\xd6\xb7\xaf \xb8\xd5\xc0\xfa \xb0\xa1\xbf\xe4~");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xb9\xd9\xb9\xd9\xc0\xcc~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					SetcurScreen(6);
				}
			} else if (actor->m_clevel % 3 == 1) {
				if (g_curStance == 1) {
					sprintf(msg, "\xb9\xab\xb3\xab\xb4\xd4 \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hello ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xb8\xf9\xc5\xa9\xc0\xc7\xbf\xb5\xc8\xa5\xc0\xcc \xc1\xa6\xb0\xd4 \xb5\xe9\xbe\xee\xbf\xc0\xb8\xe9 \xbe\xee\xb6\xb2 \xc8\xbf\xb4\xc9\xc0\xcc \xc0\xd6\xb4\xc2\xb0\xc5\xbf\xb9\xbf\xe4?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "\xbe\xc6\xc4\xda\xc4\xe2\xb1\xa4 \xb4\xeb\xbd\xc5 \xbe\xc6\xbc\xf6\xb6\xf3\xc6\xd0\xc8\xb2\xb1\xc7\xc0\xbb \xbe\xb2\xbd\xc7\xbc\xf6 \xc0\xd6\xb0\xd4\xb5\xc7\xb4\xc2\xb0\xc5 \xb0\xb0\xb3\xd7\xbf\xe4 ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xb1\xd7\xb7\xb8\xb1\xba\xbf\xe4 ^^; \xb0\xa1\xb8\xa3\xc3\xc4\xc1\xd6\xbc\xc5\xbc\xad \xb0\xed\xb8\xb6\xbf\xf6\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "\xb9\xd9\xb9\xd9\xc0\xcc~ ^_^/");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					SetcurScreen(6);
				}
			} else {
				if (g_curStance == 1) {
					sprintf(msg, "\xb9\xab\xb3\xab\xb4\xd4 \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "Hello ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 3) {
					sprintf(msg, "\xbf\xa1\xbd\xba\xbf\xec\xc0\xc7 \xc8\xbf\xb4\xc9\xc0\xba \xbe\xee\xb6\xb2\xb0\xc5\xbf\xb9\xbf\xe4?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
				} else if (g_curStance == 4) {
					sprintf(msg, "\xb8\xf3\xbd\xba\xc5\xcd\xb0\xa1 \xc0\xdb\xbe\xc6\xc1\xae\xbc\xad \xb0\xf8\xb0\xdd\xb7\xc2\xc0\xcc 1/3 \xbc\xf6\xc1\xd8\xc0\xb8\xb7\xce \xb3\xb7\xbe\xc6\xc1\xae\xbf\xe4");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
				} else if (g_curStance == 5) {
					sprintf(msg, "\xc7\xec\xbf\xa1 \xb1\xd7\xb7\xb8\xb1\xb8\xb3\xaa~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
				} else if (g_curStance == 6) {
					sprintf(msg, "\xb1\xd7\xb7\xb3 \xbf\xad\xb7\xbe \xb5\xe6\xc5\xdb \xc7\xcf\xbc\xbc\xbf\xe4 ^^;");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
				} else {
					SetcurScreen(6);
				}
			}

			g_MainNextActTime = timeGetTime() + waitTime;
			++g_curStance;
			if (g_curScreen == 6) {
				if (boy) {
					gameMode->PlaceEffect(&boy->m_pos, 304, 300, 0);
					gameMode->GetWorld()->RemoveGameActor(boyAid);
				}
			}
		} else if (map == 3) {
			CGameActor* boy = g_session.GetGameActorByAid(6);
			if (g_curStance == 0) {
				gameMode->Zc_Notify_Entry_HP(6, 18, -3);
				if (g_curStory == 52) {
					waitTime = 1000;
					if (g_session.DoPCHaveItem(1813))
						g_curStory = 53;
					if (g_session.DoPCHaveItem(1814))
						g_curStory = 53;
					if (g_session.DoPCHaveItem(1815))
						g_curStory = 53;
				} else {
					waitTime = 1000;
				}
			} else {

				short story;
				if (g_curStance > 0)
					story = *(volatile short*)&g_curStory;
				else
					story = *(volatile short*)&g_curStory;

				if (story == 51) {
					if (g_curStance == 1) {
						sprintf(msg, "Hello~");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					} else if (g_curStance == 2) {
						sprintf(msg, "\xbe\xc6.. \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4..");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					} else if (g_curStance == 3) {
						sprintf(msg, "\xc8\xa4\xbd\xc3 \xc4\xa7\xc5\xf5\xb0\xe6 \xbe\xb2\xbd\xc7\xbc\xf6 \xc0\xd6\xc0\xb8\xbd\xc5\xb0\xa1\xbf\xe4?_?");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					} else if (g_curStance == 4) {
						sprintf(msg, "\xc0\xbd.. \xbf\xb9");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					} else if (g_curStance == 5) {
						sprintf(msg, "\xbf\xec\xb5\xe7\xb0\xed\xb7\xbd \xb6\xa7\xb9\xae\xbf\xa1 \xb1\xd7\xb7\xaf\xb4\xc2\xb5\xa5 \xc0\xfa \xc1\xbb \xb5\xb5\xbf\xcd\xc1\xd6\xbd\xc7\xbc\xf6\xc0\xd6\xc0\xb8\xbc\xbc\xbf\xe4?");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
						waitTime = 3500;
					} else if (g_curStance == 6) {
						sprintf(msg, "\xbf\xec\xb5\xe7\xb0\xed\xb7\xbd \xc0\xcc\xb8\xe9 SP\xbc\xd2\xb8\xf0\xb0\xa1 \xbd\xc9\xc7\xcf\xb1\xe4 \xc7\xcf\xc1\xf6\xb8\xb8 \xc4\xa7\xc5\xf5\xb0\xe6\xc0\xcc \xc8\xbf\xb0\xfa\xc0\xfb\xc0\xcc\xc1\xd2.");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						waitTime = 3500;
					} else if (g_curStance == 7) {
						sprintf(msg, "\xb2\xc7\xc2\xa5\xb7\xce\xb4\xc2 \xc8\xfb\xb5\xe9\xb0\xed\xbf\xe4, \xb8\xf9\xc5\xa9\xbf\xeb \xc1\xc1\xc0\xba\xb9\xab\xb1\xe2\xb8\xa6 \xc7\xcf\xb3\xaa \xc1\xd6\xbd\xc3\xb8\xe9 \xb5\xb5\xbf\xcd\xb5\xe5\xb8\xb1\xb2\xb2\xbf\xe4~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
						waitTime = 3500;
					} else if (g_curStance == 8) {
						sprintf(msg, "\xb8\xf9\xc5\xa9\xbf\xeb \xc1\xc1\xc0\xba \xb9\xab\xb1\xe2\xbf\xe4?.. \xc8\xec.. \xbe\xc6\xba\xfc\xc7\xd1\xc5\xd7 \xba\xce\xc5\xb9\xc7\xd8\xbe\xdf\xb0\xda\xb3\xd7\xbf\xe4 \xa4\xd0_\xa4\xd0");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					} else if (g_curStance == 9) {
						boy->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
					} else {
						g_curStory = 52;
						SetcurScreen(6);
					}
				} else if (story == 52) {
					if (g_curStance == 1) {
						sprintf(msg, "Hello~");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					} else if (g_curStance == 2) {
						sprintf(msg, "\xb8\xf9\xc5\xa9\xbf\xeb \xc1\xc1\xc0\xba \xb9\xab\xb1\xe2 \xb1\xb8\xc7\xcf\xbc\xcc\xbe\xee\xbf\xe4?");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					} else if (g_curStance == 3) {
						sprintf(msg, "\xbe\xc6\xc1\xf7\xbf\xe4 \xa4\xd0_\xa4\xd0  \xbe\xc6\xba\xfc\xb0\xa1 \xbf\xad\xbd\xc9\xc8\xf7 \xc1\xa6\xc0\xdb\xc1\xdf\xc0\xcc\xbd\xc7\xb0\xc5\xbf\xb9\xbf\xe4.");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					} else if (g_curStance == 4) {
						sprintf(msg, "OK T_T");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					} else if (g_curStance == 5) {
						actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
					} else {
						SetcurScreen(6);
					}
				} else if (story == 53) {
					if (g_curStance == 1) {
						sprintf(msg, "\xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~ \xb9\xab\xb1\xe2 \xb0\xa1\xc1\xae\xbf\xd4\xbe\xee\xbf\xe4 ^_^");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					} else if (g_curStance == 2) {
						sprintf(msg, "\xbf\xc0.. \xb0\xa8\xbb\xe7\xc7\xd5\xb4\xcf\xb4\xd9.");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);

						short numItem = g_session.GetNumItem();
						for (short i = 0; i < numItem; ++i) {
							itemInfo = g_session.GetItemInfoBySequence(i);
							if (atoi(itemInfo.m_itemName.c_str()) == 1813
								|| atoi(itemInfo.m_itemName.c_str()) == 1814
								|| atoi(itemInfo.m_itemName.c_str()) == 1815) {
								char msg[256];
								char itemName[64];
								itemInfo.GetItemName(itemName);
								sprintf(msg, "%s\xb8\xa6 \xb8\xcd\xb7\xe6\xbf\xa1\xb0\xd4 \xc0\xfc\xb4\xde\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9", itemName);
								g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffff00, 0, 0);
								g_session.DecItem(itemInfo.m_itemIndex, 1);
								break;
							}
						}
					} else if (g_curStance == 3) {
						sprintf(msg, "\xb1\xd7\xb7\xb3 \xbe\xd5\xc0\xb8\xb7\xce\xb4\xc2 \xb0\xc5\xba\xcf\xbc\xb6\xbf\xa1 \xb0\xa1\xbd\xc7\xb6\xa7 \xc1\xa6\xb0\xa1 \xb5\xb5\xbf\xcd\xb5\xe5\xb8\xb1\xb2\xb2\xbf\xe4 ^_^");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
					} else if (g_curStance == 4) {
						sprintf(msg, "\xc0\xdf\xba\xce\xc5\xb9\xb5\xe5\xb7\xc1\xbf\xe4~ \xc0\xfa\xb4\xc2 %s \xbf\xb9\xbf\xe4", g_session.GetCharName());
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					} else if (g_curStance == 5) {
						sprintf(msg, "\xc0\xfc \xb8\xcd\xb7\xe6 \xc0\xcc\xb6\xf3\xb0\xed \xc7\xd5\xb4\xcf\xb4\xd9~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					} else if (g_curStance == 6) {
						sprintf(msg, "\xb0\xc5\xba\xcf\xbc\xb6\xbf\xa1\xbc\xad \xba\xc6\xbf\xe4 ^^; \xb9\xd9\xb9\xd9\xc0\xcc~");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
					} else {
						g_curStory = 54;
						SetcurScreen(6);
					}
				} else {
					if (g_curStance == 1) {
						sprintf(msg, "\xb8\xcd\xb7\xe6\xb4\xd4 \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					} else if (g_curStance == 2) {
						sprintf(msg, "Hello ^^;");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					} else if (g_curStance == 3) {
						sprintf(msg, "\xb5\xbf\xb1\xdb\xb5\xbf\xb1\xdb\xc7\xd1 \xb1\xe2\xb1\xb8\xc3\xbc\xc0\xc7 \xc8\xbf\xb4\xc9\xc0\xba \xbe\xee\xb6\xb2\xb0\xc5\xbf\xb9\xbf\xe4?");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					} else if (g_curStance == 4) {
						sprintf(msg, "\xc7\xcf\xb3\xaa\xb4\xe7 \xb9\xab\xb1\xe2\xc0\xc7 \xb0\xf8\xb0\xdd\xb7\xc2\xc0\xc7 1/5 \xc1\xa4\xb5\xb5\xc0\xc7 \xb0\xf8\xb0\xdd\xb7\xc2 \xc1\xf5\xb0\xa1\xc8\xbf\xb0\xfa\xb0\xa1 \xc0\xd6\xbe\xee\xbf\xe4");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
					} else if (g_curStance == 5) {
						sprintf(msg, "\xc7\xec\xbf\xa1 \xb1\xd7\xb7\xb8\xb1\xb8\xb3\xaa~");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 33, 0, 0);
					} else if (g_curStance == 6) {
						sprintf(msg, "\xb1\xd7\xb7\xb1\xb0\xc5\xc1\xf6\xbf\xe4~");
						boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
					} else if (g_curStance == 7) {
						sprintf(msg, "\xb1\xd7\xb7\xb3 \xc0\xfc \xc7\xc1\xb7\xd0\xc5\xd7\xb6\xf3\xb7\xce \xb5\xb9\xbe\xc6\xb0\xa1\xbf\xe4~ \xb9\xd9\xb9\xd9\xc0\xcc");
						actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
						actor->SendMsg(NULL, AM_EMOTION, 12, 0, 0);
					} else {
						SetcurScreen(6);
					}
				}
			}

			g_MainNextActTime = timeGetTime() + waitTime;
			++g_curStance;
			if (g_curScreen == 6) {
				gameMode->PlaceEffect(&boy->m_pos, 304, 300, 0);
				gameMode->GetWorld()->RemoveGameActor(6);
			}
		} else if (map == 4) {
			CGameActor* boy = g_session.GetGameActorByAid(3);
			CGameActor* mob = g_session.GetGameActorByAid(100);
			if (g_curStance == 0) {
				gameMode->Zc_Notify_Entry_Mob(811, 100, 18, -3);
				waitTime = 1000;
				if (g_Aco_Job != JT_PRIEST_H)
					gameMode->Zc_Notify_Entry_HP(3, 10, -4);
				if (g_curStory <= 92) {
					if (Boy_friendly < 8)
						g_curStory = 95;
					else
						g_curStory = 93;
				}
			} else if (g_Aco_Job == JT_PRIEST_H) {
				if (g_curStance == 1) {
					sprintf(msg, "\xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb8\xa6 \xb2\xc0 \xc3\xb3\xc4\xa1\xc7\xd8\xbe\xdf\xb5\xc8\xb4\xd9");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xb9\xb0\xb7\xd0\xc0\xcc\xbf\xb9\xbf\xe4.. \xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4\xc0\xcc \xc0\xfd \xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb7\xce \xb8\xb8\xb5\xe9\xb1\xe2\xc0\xa7\xc7\xd8 \xc8\xf1\xbb\xfd\xc7\xcf\xbc\xcc\xb4\xc2\xb5\xa5..");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
					waitTime = 4000;
				} else {
					SetcurScreen(6);
				}
			} else if (g_curStory == 95) {
				if (g_curStance == 1) {
					sprintf(msg, "\xb9\xdf\xc5\xb0\xb8\xae\xb4\xd4 \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb0\xa1 \xb5\xc7\xb0\xed\xbd\xcd\xc0\xba\xb0\xa1?...");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					mob->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 3) {
					sprintf(msg, "\xb3\xd7! \xc7\xcf\xc7\xc1\xb0\xa1 \xb5\xc7\xbc\xad \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb7\xce\xba\xce\xc5\xcd \xc0\xfa\xc8\xf1 \xb8\xb6\xc0\xbb\xb5\xe9\xc0\xbb \xc1\xf6\xc5\xb0\xb0\xed \xbd\xcd\xbe\xee\xbf\xe4.");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xb3\xca\xb4\xc2 \xc7\xcf\xc7\xc1\xb0\xa1 \xb5\xc9\xbc\xf6\xc0\xd6\xb4\xc2 \xc0\xe7\xb4\xc9\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xb4\xd9.");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 5) {
					sprintf(msg, "\xb3\xb2\xc0\xba\xb0\xc7 \xc4\xab\xc4\xab\xb7\xce\xc6\xae \xb3\xca\xc0\xc7 \xb0\xe1\xbd\xc9\xc0\xcc\xb4\xd9. \xc0\xfc\xbf\xa1 \xbf\xd4\xc0\xbb\xb6\xa7 \xc0\xcc\xbe\xdf\xb1\xe2 \xc7\xdf\xbe\xfa\xc1\xf6.. \xb3\xaa\xc0\xc7 \xb8\xb6\xb7\xc2\xbf\xa1 \xb0\xfc\xc7\xd8.. ");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 6) {
					sprintf(msg, "\xb3\xd7, \xc0\xfc \xc1\xd8\xba\xf1\xb0\xa1 \xb5\xc7\xbe\xee\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xc0\xfa\xba\xb8\xb4\xd9\xb4\xc2 \xc0\xfa\xc8\xf1 \xb8\xb6\xc0\xbb\xc0\xcc, \xc0\xfa\xc8\xf1 \xbc\xbc\xb0\xe8\xb0\xa1 \xc1\xdf\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 7) {
					sprintf(msg, "\xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4\xc0\xba \xbe\xee\xb6\xb2 \xc1\xd8\xba\xf1\xb8\xa6 \xc7\xcf\xbc\xc5\xbe\xdf \xb5\xc7\xb4\xc2\xb0\xc5\xbf\xb9\xbf\xe4?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 8) {
					sprintf(msg, "\xba\xb0\xc0\xcf \xbe\xc6\xb4\xcf\xbf\xb9\xbf\xe4. %s\xb4\xd4\xc0\xcc \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb8\xb8 \xc3\xb3\xb8\xae\xc7\xd8\xc1\xd6\xbd\xc3\xb8\xe9 \xc0\xfa\xb4\xc2 \xc7\xe0\xba\xb9\xc7\xd2\xb0\xcc\xb4\xcf\xb4\xd9 ^^*", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 9) {
					sprintf(msg, "Job Change !!!");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
					mob->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 441, 250, 0);
					boy->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 441, 250, 0);
				} else if (g_curStance == 10) {
					boy->SetState(ST_DEAD);
					g_Aco_Job = 4009;
					actor->SetJob(4009);
					actor->LaunchEffect(158);
					actor->m_MaxHp *= 2;
					actor->m_MaxSp *= 2;
					g_session.GainSkill(270, 1, 1);
					g_session.GainSkill(28, 3, 1);
					g_session.SetStatusData(actor);
				} else if (g_curStance == 11) {
					sprintf(msg, "\xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4!!");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 12) {
					sprintf(msg, "\xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb6\xa7\xb9\xae\xbf\xa1 \xba\xd2\xbf\xcf\xc0\xfc\xc7\xd1 \xb3\xaa\xc0\xc7 \xb8\xb6\xb7\xc2\xc0\xbb \xb1\xd7\xb0\xa1 \xc0\xda\xbd\xc5\xc0\xc7 \xbb\xfd\xb8\xed\xb7\xc2\xc0\xb8\xb7\xce \xba\xb8\xc3\xe6\xc7\xd8\xc1\xd6\xbe\xfa\xb4\xdc\xb4\xd9.");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					mob->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 13) {
					sprintf(msg, "\xb3\xca\xb4\xc2 \xb1\xd7\xb8\xa6 \xc0\xa7\xc7\xd8\xbc\xad\xb6\xf3\xb5\xb5 \xb2\xc0 \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb8\xa6 \xbe\xb2\xb7\xaf\xc6\xae\xb7\xc1\xbe\xdf\xc7\xcf\xb0\xda\xb1\xb8\xb3\xaa. \xb0\xa1\xb0\xc5\xb6\xf3! \xc1\xd7\xc0\xbd\xc0\xc7\xc5\xbe\xc0\xb8\xb7\xce");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 14) {
					sprintf(msg, "\xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4.. \xc0\xcc\xb7\xb2\xbc\xf6\xb0\xa1.. \xc0\xcc\xb7\xb2\xc1\xd9\xc0\xba...");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else {
					g_curStory = 96;
					SetcurScreen(6);
				}
			} else if (g_curStory == 93) {
				if (g_curStance == 1) {
					sprintf(msg, "\xb9\xdf\xc5\xb0\xb8\xae\xb4\xd4 \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "\xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb0\xa1 \xb5\xc7\xb0\xed\xbd\xcd\xc0\xba\xb0\xa1?...");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					mob->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 3) {
					sprintf(msg, "\xb3\xd7! \xc7\xcf\xc7\xc1\xb0\xa1 \xb5\xc7\xbc\xad \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb7\xce\xba\xce\xc5\xcd \xc0\xfa\xc8\xf1 \xb8\xb6\xc0\xbb\xb5\xe9\xc0\xbb \xc1\xf6\xc5\xb0\xb0\xed \xbd\xcd\xbe\xee\xbf\xe4.");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xb3\xca\xb4\xc2 \xc7\xcf\xc7\xc1\xb0\xa1 \xb5\xc9\xbc\xf6\xc0\xd6\xb4\xc2 \xc0\xe7\xb4\xc9\xc0\xcc \xc3\xe6\xba\xd0\xc7\xcf\xb4\xd9.");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 5) {
					sprintf(msg, "\xb3\xb2\xc0\xba\xb0\xc7 \xc4\xab\xc4\xab\xb7\xce\xc6\xae \xb3\xca\xc0\xc7 \xb0\xe1\xbd\xc9\xc0\xcc\xb4\xd9. \xc0\xfc\xbf\xa1 \xbf\xd4\xc0\xbb\xb6\xa7 \xc0\xcc\xbe\xdf\xb1\xe2 \xc7\xdf\xbe\xfa\xc1\xf6.. \xb3\xaa\xc0\xc7 \xb8\xb6\xb7\xc2\xbf\xa1 \xb0\xfc\xc7\xd8.. ");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 6) {
					sprintf(msg, "\xb3\xd7.... ");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 7) {
					sprintf(msg, "\xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4\xc0\xba \xbe\xee\xb6\xb2 \xc1\xd8\xba\xf1\xb8\xa6 \xc7\xcf\xbc\xc5\xbe\xdf \xb5\xc7\xb4\xc2\xb0\xc5\xbf\xb9\xbf\xe4?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 8) {
					sprintf(msg, "\xc5\xa9\xc8\xe6... \xb9\xdf\xc5\xb0\xb8\xae\xb4\xd4 \xc1\xcb\xbc\xdb\xc7\xd5\xb4\xcf\xb4\xd9. \xc0\xfa \xbe\xc6\xb9\xab\xb7\xa1\xb5\xb5 %s\xb4\xd4\xc0\xbb \xc1\xbb \xb4\xf5 \xba\xb8\xb0\xed\xbd\xcd\xb3\xd7\xbf\xe4", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 28, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 9) {
					sprintf(msg, "\xc1\xd7\xbe\xee\xb9\xf6\xb8\xae\xb8\xe9 %s\xb4\xd4\xc0\xc7 \xbb\xe7\xb6\xfb\xbd\xba\xb7\xaf\xbf\xee \xbe\xf3\xb1\xbc\xc0\xbb \xb4\xf5 \xb8\xf8\xba\xbb\xb4\xd9\xb4\xc2\xb0\xc9 \xbb\xfd\xb0\xa2\xc7\xcf\xb4\xcf\xb1\xee...", g_session.GetCharName());
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 10) {
					sprintf(msg, "\xc1\xd7\xb4\xc2\xb4\xd9\xb4\xcf \xb9\xab\xbd\xbc \xb6\xe6\xc0\xcc\xbf\xb9\xbf\xe4?");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 11) {
					sprintf(msg, "\xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb6\xa7\xb9\xae\xbf\xa1 \xb3\xaa\xc0\xc7 \xb8\xb6\xb7\xc2\xc0\xcc \xbf\xcf\xc0\xfc\xc4\xa1 \xbe\xca\xb1\xe2\xbf\xa1 \xb3\xca\xb8\xa6 \xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb7\xce \xc0\xfc\xc1\xf7\xbd\xc3\xc5\xb0\xb7\xc1\xb8\xe9 \xc3\xdf\xb0\xa1\xb7\xce \xbb\xfd\xb8\xed\xb7\xc2\xc0\xcc \xc7\xca\xbf\xe4\xc7\xcf\xb4\xdc\xb4\xd9");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 4500;
				} else if (g_curStance == 12) {
					sprintf(msg, "\xc7\xe4! \xc0\xfc \xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4\xc0\xbb \xc8\xf1\xbb\xfd\xc7\xd8\xbc\xad\xb1\xee\xc1\xf6 \xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb0\xa1 \xb5\xc7\xb0\xed\xbd\xcd\xc1\xf6\xb4\xc2 \xbe\xca\xbe\xc6\xbf\xe4.");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 23, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 13) {
					sprintf(msg, "\xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb0\xa1 \xb5\xc7\xc1\xf6 \xbe\xca\xb0\xed\xb5\xb5 \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xbf\xa1\xb0\xd4 \xc0\xcc\xb1\xe6\xbc\xf6\xc0\xd6\xb4\xc2 \xb9\xe6\xb9\xfd\xc0\xcc \xc0\xd6\xc0\xbb\xb2\xa8\xbf\xb9\xbf\xe4. \xbf\xec\xb8\xae \xb0\xb0\xc0\xcc \xc8\xfb\xb3\xbb\xbf\xe4");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 40, 0, 0);
					waitTime = 4500;
				} else if (g_curStance == 14) {
					sprintf(msg, "\xb3\xd7 \xc1\xcb\xbc\xdb\xc7\xd8\xbf\xe4, \xb1\xd7\xb8\xae\xb0\xed ...");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
				} else if (g_curStance == 15) {
					sprintf(msg, "\xbb\xe7\xb6\xfb\xc7\xd5\xb4\xcf\xb4\xd9 ^^*");
					boy->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					boy->SendMsg(NULL, AM_EMOTION, 40, 0, 0);
					waitTime = 4500;
				} else if (g_curStance == 16) {
					actor->SendMsg(NULL, AM_EMOTION, 31, 0, 0);
				} else {
					if (g_curHardness == 1)
						g_curHardness = 2;
					g_curStory = 94;
					SetcurScreen(6);
				}
			} else if (g_curStory == 94 && g_session.GetLevel() >= 151) {
				if (g_curStance == 1) {
					sprintf(msg, "\xb9\xdf\xc5\xb0\xb8\xae\xb4\xd4 \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "%s \xc0\xcc\xbf\xa9 \xb9\xab\xbd\xbc\xc0\xcf\xb7\xce \xb4\xd9\xbd\xc3 \xb3\xaa\xb8\xa6 \xc3\xa3\xbe\xc6\xbf\xd4\xb4\xc2\xb0\xa1?", g_session.GetCharName());
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					mob->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 3) {
					sprintf(msg, "\xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb4\xc2 \xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb4\xd4\xc0\xbb \xc8\xf1\xbb\xfd\xc7\xd8\xbe\xdf \xb5\xc7\xb1\xe2\xb6\xa7\xb9\xae\xbf\xa1 \xb0\xef\xb6\xf5\xc7\xcf\xc1\xf6\xb8\xb8, \xba\xb8\xc5\xeb \xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb7\xce \xc0\xfc\xc1\xf7\xc7\xcf\xb0\xed \xbd\xcd\xbe\xee\xbc\xad \xbf\xd4\xbe\xee\xbf\xe4");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 4) {
					sprintf(msg, "\xb0\xa1\xb4\xc9\xc7\xcf\xb4\xdc\xb4\xd9");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 5) {
					sprintf(msg, "Job Change !!!");
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
					mob->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 441, 250, 0);
				} else if (g_curStance == 6) {
					g_Aco_Job = 8;
					actor->SetJob(8);
					actor->LaunchEffect(158);
					actor->m_MaxHp = actor->m_MaxHp * 1.3f;
					actor->m_MaxSp = actor->m_MaxSp * 1.3f;
					g_session.GainSkill(28, 3, 1);
					boy->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
					g_session.SetStatusData(actor);
				} else if (g_curStance == 7) {
					sprintf(msg, "\xb0\xa8\xbb\xe7\xc7\xd5\xb4\xcf\xb4\xd9 ^^*");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
					waitTime = 3500;
				} else {
					g_curStory = 96;
					SetcurScreen(6);
				}
			} else {
				if (g_curStance == 1) {
					sprintf(msg, "\xb9\xdf\xc5\xb0\xb8\xae\xb4\xd4 \xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4~");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 18, 0, 0);
				} else if (g_curStance == 2) {
					sprintf(msg, "%s \xc0\xcc\xbf\xa9 \xb8\xf0\xb5\xce\xb0\xa1 \xb3\xd7\xb0\xa1 \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb8\xa6 \xc3\xb3\xc4\xa1\xc7\xd8\xc1\xd6\xb1\xe6 \xb9\xd9\xb6\xf3\xb0\xed \xc0\xd6\xb4\xdc\xb4\xd9", g_session.GetCharName());
					mob->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					waitTime = 3500;
				} else if (g_curStance == 3) {
					sprintf(msg, "\xb3\xd7!");
					actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					actor->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
					waitTime = 3500;
				} else {
					SetcurScreen(6);
				}
			}

			g_MainNextActTime = timeGetTime() + waitTime;
			++g_curStance;
			if (g_curScreen == 6) {
				if (boy) {
					gameMode->PlaceEffect(&boy->m_pos, 304, 300, 0);
					gameMode->GetWorld()->RemoveGameActor(3);
				}
				if (mob) {
					gameMode->PlaceEffect(&mob->m_pos, 304, 300, 0);
					gameMode->GetWorld()->RemoveGameActor(100);
				}
			}
		} else {
			SetcurScreen(6);
		}
		return;
	}
}
