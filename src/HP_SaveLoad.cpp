#include "common.h"
#include "device/Sound.h"
#include "window/UIFrameWnd.h"
#include "GameMode.h"
#include "Session.h"
#include "GameActor.h"
#include "GameWorld.h"
#include "RagEffect.h"
#include "pdb_enums.h"
#include "resource/Skybox.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <mmsystem.h>
#include "resource/Ground.h"

extern CModeMgr g_modeMgr;
extern CSession g_session;
extern CRenderer* g_renderer;
extern UIWindowMgr g_windowMgr;
extern char g_savedata[][256];
extern int g_curStance;
extern int g_curScreen;
extern short g_curStory;
extern short g_scroll_Y;
extern short g_Aco_Job;
extern char g_ClearedGame;
extern int Ealpha;
extern int Ealpha2;


int Ealpha2;
int Ealpha;
extern char g_curSave;
extern unsigned long g_TimeStorage;
extern char g_TIME_hr;
extern char g_TIME_min;
extern short g_Aco_Head;
extern short g_Aco_HeadPal;
extern short g_Aco_BodyPal;
extern short game_TetrisPlayed;
extern short game_ShopOpenNumber;
extern short Boy_friendly;
extern short Boy_talked;
extern bool game_ItemRentEvent;
extern short Boy_Job;
extern char g_Aco_Name[];
extern char g_Aco_Hair;
extern short g_BS_Job;
extern char g_BS_Hair;
extern char g_Aco_Hair_Color;
extern char g_BS_Hair_Color;
extern char g_curHardness;
extern char g_curEXPmode;


void UIWindowMgr::HP_SaveGame(char F1)
{
	ITEM_INFO itemInfo;
	short sData;
	int iData;
	unsigned long timeCalc;
	char msgZ[256];

	CGameActor* actor = g_session.GetGameActorByAid(1);
	FILE* fp;
	if (g_curSave == 1) {
		fp = fopen("SaveData1.KSY", "wb");
	} else if (g_curSave == 2) {
		fp = fopen("SaveData2.KSY", "wb");
	} else {
		fp = fopen("SaveData3.KSY", "wb");
	}

	timeCalc = timeGetTime() - g_TimeStorage;
	if (timeCalc >= 60000) {
		g_TimeStorage = timeGetTime();
		timeCalc /= 60000;
		do {
			++g_TIME_min;
			if (g_TIME_min >= 60) {
				++g_TIME_hr;
				g_TIME_min -= 60;
			}
		} while (--timeCalc);
	}

	if (fp) {
		fwrite(&g_session.m_DayCount, 2, 1, fp);
		iData = g_session.GetGold();
		fwrite(&iData, 4, 1, fp);
		fwrite(&g_Aco_Job, 2, 1, fp);
		fwrite(&g_Aco_Head, 2, 1, fp);
		fwrite(&g_Aco_HeadPal, 2, 1, fp);
		fwrite(&g_Aco_BodyPal, 2, 1, fp);
		fwrite(&actor->m_MaxHp, 2, 1, fp);
		fwrite(&actor->m_Hp, 2, 1, fp);
		fwrite(&actor->m_MaxSp, 2, 1, fp);
		fwrite(&actor->m_Sp, 2, 1, fp);
		fwrite(&actor->m_clevel, 2, 1, fp);
		fwrite(&actor->m_Exp, 4, 1, fp);
		fwrite(&actor->m_Str, 2, 1, fp);
		fwrite(&actor->m_Int, 2, 1, fp);
		fwrite(&actor->m_Dex, 2, 1, fp);
		fwrite(&actor->m_Vit, 2, 1, fp);
		fwrite(&actor->m_Luk, 2, 1, fp);
		fwrite(&actor->m_Agi, 2, 1, fp);

		sData = (short)g_session.GetSkillItemCount();
		fwrite(&sData, 2, 1, fp);
		int forNum = sData;
		for (int skillIdx = 0; skillIdx < forNum; ++skillIdx) {
			SKILL_INFO skillInfo;
			skillInfo = g_session.GetSkillItemInfoBySequence(skillIdx);
			fwrite(&skillInfo.SKID, 2, 1, fp);
			fwrite(&skillInfo.level, 2, 1, fp);
		}

		fwrite(&game_TetrisPlayed, 2, 1, fp);
		fwrite(&g_curStory, 2, 1, fp);
		fwrite(&game_ShopOpenNumber, 2, 1, fp);
		fwrite(&Boy_friendly, 2, 1, fp);
		fwrite(&Boy_talked, 2, 1, fp);

		sData = (short)g_session.GetNumItem();
		fwrite(&sData, 2, 1, fp);
		forNum = sData;
		for (int itemIdx = 0; itemIdx < forNum; ++itemIdx) {
			itemInfo = g_session.GetItemInfoBySequence(itemIdx);
			sData = atoi(itemInfo.m_itemName.c_str());
			fwrite(&sData, 2, 1, fp);
		}

		itemInfo = g_session.GetEquipedItems(1);
		sData = atoi(itemInfo.m_itemName.c_str());
		fwrite(&sData, 2, 1, fp);
		itemInfo = g_session.GetEquipedItems(8);
		sData = atoi(itemInfo.m_itemName.c_str());
		fwrite(&sData, 2, 1, fp);
		itemInfo = g_session.GetEquipedItems(9);
		sData = atoi(itemInfo.m_itemName.c_str());
		fwrite(&sData, 2, 1, fp);
		itemInfo = g_session.GetEquipedItems(0);
		sData = atoi(itemInfo.m_itemName.c_str());
		fwrite(&sData, 2, 1, fp);

		fwrite(&game_ItemRentEvent, 1, 1, fp);
		fwrite(&Boy_Job, 2, 1, fp);
		fwrite(g_Aco_Name, 0x20, 1, fp);
		fwrite(&g_Aco_Hair, 1, 1, fp);
		fwrite(&g_BS_Job, 2, 1, fp);
		fwrite(&g_BS_Hair, 1, 1, fp);
		fwrite(&g_TIME_hr, 1, 1, fp);
		fwrite(&g_TIME_min, 1, 1, fp);
		fwrite(&g_Aco_Hair_Color, 1, 1, fp);
		fwrite(&g_BS_Hair_Color, 1, 1, fp);
		fwrite(&g_curHardness, 1, 1, fp);
		fwrite(&g_curEXPmode, 1, 1, fp);
		fwrite(&g_ClearedGame, 1, 1, fp);
		fclose(fp);

		sprintf(msgZ, "[SAVE] \xb5\xa5\xc0\xcc\xc5\xb8\xb0\xa1 \xc0\xfa\xc0\xe5 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9");
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msgZ, 0xfac8c8, 0, 0);
	}
}


void UIWindowMgr::HP_LoadGame(char F1)
{
	short val2;
	int iData;
	short val1;
	CGameMode* gameMode;
	short forNum;
	CGameActor* BS_Actor;

	gameMode = g_modeMgr.GetGameMode();
	CGameActor* actor = g_session.GetGameActorByAid(1);
	BS_Actor = g_session.GetGameActorByAid(2);

	FILE* fp;
	if (g_curSave == 1) {
		fp = fopen("SaveData1.KSY", "rb");
	} else if (g_curSave == 2) {
		fp = fopen("SaveData2.KSY", "rb");
	} else {
		fp = fopen("SaveData3.KSY", "rb");
	}

	if (fp) {
		fread(&g_session.m_DayCount, 2, 1, fp);
		fread(&iData, 4, 1, fp);
		g_session.SetGold(iData);

		fread(&g_Aco_Job, 2, 1, fp);
		fread(&g_Aco_Head, 2, 1, fp);
		fread(&g_Aco_HeadPal, 2, 1, fp);
		fread(&g_Aco_BodyPal, 2, 1, fp);
		fread(&actor->m_MaxHp, 2, 1, fp);
		fread(&actor->m_Hp, 2, 1, fp);
		fread(&actor->m_MaxSp, 2, 1, fp);
		fread(&actor->m_Sp, 2, 1, fp);
		fread(&actor->m_clevel, 2, 1, fp);
		if (actor->m_MaxHp < 0 && actor->m_clevel > 120) {
			actor->m_MaxHp = 32000;
			actor->m_Hp = 1;
		}
		fread(&actor->m_Exp, 4, 1, fp);
		g_session.SetNextexp(gameMode->GetNextExpValue(actor->m_clevel) -
			gameMode->GetNextExpValue(actor->m_clevel - 1));
		fread(&actor->m_Str, 2, 1, fp);
		fread(&actor->m_Int, 2, 1, fp);
		fread(&actor->m_Dex, 2, 1, fp);
		fread(&actor->m_Vit, 2, 1, fp);
		fread(&actor->m_Luk, 2, 1, fp);
		fread(&actor->m_Agi, 2, 1, fp);
		actor->SetJob(g_Aco_Job);

		g_session.ClearSkillItem();

		fread(&forNum, 2, 1, fp);
		for (int skillIdx = 0; skillIdx < forNum; ++skillIdx) {
			fread(&val1, 2, 1, fp);
			fread(&val2, 2, 1, fp);
			g_session.GainSkill(val1, val2, false);
		}

		fread(&game_TetrisPlayed, 2, 1, fp);
		fread(&g_curStory, 2, 1, fp);
		fread(&game_ShopOpenNumber, 2, 1, fp);
		fread(&Boy_friendly, 2, 1, fp);
		fread(&Boy_talked, 2, 1, fp);

		{
			ITEM_INFO itemInfo;
			itemInfo = g_session.GetEquipedItems(8);
			g_session.MoveItemFromEquipWndToItemWnd(itemInfo.m_location);
			itemInfo = g_session.GetEquipedItems(9);
			g_session.MoveItemFromEquipWndToItemWnd(itemInfo.m_location);
			itemInfo = g_session.GetEquipedItems(0);
			g_session.MoveItemFromEquipWndToItemWnd(itemInfo.m_location);
		}

		g_session.ClearItem();
		g_session.ClearEquipItem();

		fread(&forNum, 2, 1, fp);
		for (int itemIdx = 0; itemIdx < forNum; ++itemIdx) {
			fread(&val1, 2, 1, fp);
			g_session.HP_AddItem(val1, false);
		}

		for (int equipIdx = 0; equipIdx < 4; ++equipIdx) {
			fread(&val1, 2, 1, fp);
			if (val1 > 0) {
				g_session.HP_AddItem(val1, false);
				g_session.HP_EquipItemById(val1);
			}
		}

		fread(&game_ItemRentEvent, 1, 1, fp);
		fread(&Boy_Job, 2, 1, fp);
		fread(g_Aco_Name, 0x20, 1, fp);
		g_session.SetCharName(g_Aco_Name);
		fread(&g_Aco_Hair, 1, 1, fp);
		fread(&g_BS_Job, 2, 1, fp);
		BS_Actor->SetJob(g_BS_Job);
		fread(&g_BS_Hair, 1, 1, fp);
		fread(&g_TIME_hr, 1, 1, fp);
		fread(&g_TIME_min, 1, 1, fp);
		fread(&g_Aco_Hair_Color, 1, 1, fp);
		fread(&g_BS_Hair_Color, 1, 1, fp);
		fread(&g_curHardness, 1, 1, fp);
		fread(&g_curEXPmode, 1, 1, fp);
		if (g_curEXPmode != 5) {
			g_curEXPmode = 1;
		}
		fread(&g_ClearedGame, 1, 1, fp);

		actor->SetSprHead(g_Aco_Hair);
		BS_Actor->SetSprHead(g_BS_Hair);
		actor->SetHeadPalette(g_Aco_Hair_Color);
		BS_Actor->SetHeadPalette(g_BS_Hair_Color);
		g_session.m_head = g_Aco_Hair;
		g_session.m_headPalette = g_Aco_Hair_Color;
		g_session.SetStatusData(actor);

		fclose(fp);
		g_TimeStorage = timeGetTime();
	}
}

unsigned long __stdcall EndingStrike(void * pvoid)
{
	vector3d E_pos;
	int curEF;

	g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(EF_COLORPAPER);

	for (int count = 400; count; --count) {
		E_pos.x = (float)(rand() % 121 - 492);
		int z = rand();
		E_pos.y = 0.0f;
		E_pos.z = (float)(z % 101 - 547);

		int ef = rand() % 23;
		if (ef == 0) {
			curEF = EF_ANGEL;
		} else if (ef == 1) {
			curEF = EF_ANGEL2;
		} else if (ef == 2) {
			curEF = EF_ANGEL3;
		} else if (ef == 3) {
			curEF = EF_ANGELUS;
		} else if (ef == 4) {
			curEF = EF_RESURRECTION;
		} else if (ef == 5) {
			curEF = EF_RUWACH;
		} else if (ef == 6) {
			curEF = EF_BLESSING;
		} else if (ef == 7) {
			curEF = EF_CURE;
		} else if (ef == 8) {
			curEF = EF_MVP;
		} else if (ef == 9) {
			curEF = EF_GLORIA;
		} else if (ef == 10) {
			curEF = EF_MAGNIFICAT;
		} else if (ef == 11) {
			curEF = EF_SANCTUARY;
		} else if (ef == 12) {
			curEF = EF_SUFFRAGIUM;
		} else if (ef == 13) {
			curEF = EF_BENEDICTIO;
		} else if (ef == 14) {
			curEF = EF_KYRIE;
		} else if (ef == 15) {
			curEF = EF_GLORIA;
		} else if (ef >= 16 && ef <= 22) {
			curEF = ef + 451;
		}

		g_modeMgr.GetGameMode()->PlaceEffect(&E_pos, curEF, 600, 0);
		Sleep(100);
	}

	return 0;
}

void UIWindowMgr::RenderEnding()
{
	unsigned long threadId;
	char msg[256];

	if (g_curStance == 0) {
		g_scroll_Y = 800;
		g_curStance = 1;
		if (g_curStory == 100) {
			g_curStory = 101;
		}
		Ealpha2 = 0x33;
		Ealpha = 0x44;
	} else if (g_curStance != 1) {
		return;
	}

	--g_scroll_Y;
	if (g_scroll_Y < 150) {
		g_scroll_Y = 150;
		if (g_curScreen == 101) {
			SetcurScreen(102);
		} else if (g_curScreen == 50) {
			SetcurScreen(52);
			g_ClearedGame = 1;
			HP_SaveGame(1);
		} else if (g_curScreen == 52) {
			if (g_Aco_Job != 0xfa9) {
				if (Ealpha > 0) {
					--Ealpha;
				}
				if (Ealpha2 > 0) {
					--Ealpha2;
				}
				if (Ealpha == 0) {
					CreateThread(NULL, 0, EndingStrike, this, 0, &threadId);
					SetcurScreen(53);
				}
			}
		}
	}

	if (g_curScreen >= 101) {
		RenderWhiteBlueRect(10, 90, 1004, 500, 0x44880000, 0x33880000);
	} else {
		RenderWhiteBlueRect(10, 90, 1004, 500, Ealpha << 24, Ealpha2 << 24);
	}

	if (!g_renderer->DrawScene()) {
		return;
	}

	if (g_curScreen >= 101) {
		sprintf(msg, "\xbd\xc3\xb0\xa3\xc0\xcc \xb3\xaf\xb6\xa7 \xc6\xb4\xc6\xb4\xc8\xf7 \xb8\xb8\xb5\xe7 \xb9\xcc\xb4\xcf RPG \xc0\xd4\xb4\xcf\xb4\xd9. \xc1\xa6\xb0\xa1 \xb1\xd7\xb7\xa1\xc7\xc8\xc0\xba \xc0\xfc\xc7\xf4 \xc7\xd2\xc1\xd9 \xb8\xf0\xb8\xa3\xb4\xc2 \xb0\xfc\xb0\xe8\xb7\xce...");
		g_renderer->TextOutScreen(40, g_scroll_Y, msg, 0xffffff, 20, 1);
		sprintf(msg, "\xc5\xb8\xc0\xcc\xc6\xb2\xc8\xad\xb8\xe9\xc0\xba \xc8\xa8\xc7\xc7\xbf\xa1 \xbf\xc3\xb6\xf3\xbf\xd4\xb4\xf8 \xc6\xd2\xbe\xc6\xc6\xae\xc1\xdf\xbf\xa1 \xc7\xcf\xb3\xaa \xc2\xef\xbe\xfa\xc0\xb8\xb8\xe7.. orz");
		g_renderer->TextOutScreen(40, g_scroll_Y + 60, msg, 0xffffff, 20, 1);
		sprintf(msg, "\xb1\xd7\xb7\xa1\xc7\xc8\xc0\xba \xb6\xf3\xb1\xd7\xb3\xaa\xb7\xce\xc5\xa9 \xbf\xc2\xb6\xf3\xc0\xce\xc0\xc7 \xb1\xd7\xb7\xa1\xc7\xc8 \xb1\xd7\xb4\xeb\xb7\xce \xc0\xd4\xb4\xcf\xb4\xd9");
		g_renderer->TextOutScreen(40, g_scroll_Y + 120, msg, 0xffffff, 20, 1);
		sprintf(msg, "\xc8\xa4\xbd\xc3\xb6\xf3\xb5\xb5 \xc3\xeb\xc7\xe2\xbf\xa1 \xbe\xca\xb8\xc2\xc0\xb8\xbc\xc5\xb5\xb5 \xc3\xb9\xc6\xc7 \xba\xb8\xbd\xba\xb1\xee\xc1\xf6\xb4\xc2 \xb1\xfa\xc1\xd6\xbd\xc3\xb8\xe9 \xb0\xa8\xbb\xe7\xc7\xcf\xb0\xda\xbd\xc0\xb4\xcf\xb4\xd9 \xa4\xd0_\xa4\xd0");
		g_renderer->TextOutScreen(40, g_scroll_Y + 180, msg, 0xffffff, 20, 1);
		sprintf(msg, "                                Game Designed & Programmed by Si-Yeon Kim (\xb1\xe8\xbd\xc3\xbf\xac) tei2000kim@hanmail.net");
		g_renderer->TextOutScreen(40, g_scroll_Y + 310, msg, 0xffffff, 20, 1);
		sprintf(msg, "                                                                                     in Ragnarok Division of GRAVITY 2008");
		g_renderer->TextOutScreen(40, g_scroll_Y + 340, msg, 0xffffff, 20, 1);
		sprintf(msg, "\xbd\xba\xc5\xa9\xb7\xd1\xc0\xcc \xb3\xa1\xb3\xad\xc8\xc4 A\xb9\xf6\xc6\xb0\xc0\xbb \xb4\xa9\xb8\xa3\xb8\xe9 \xc5\xb8\xc0\xcc\xc6\xb2 \xc8\xad\xb8\xe9\xc0\xb8\xb7\xce \xb5\xb9\xbe\xc6\xb0\xa9\xb4\xcf\xb4\xd9");
		g_renderer->TextOutScreen(524, g_scroll_Y + 372, msg, 0xffffff, 16, 1);
		return;
	}

	if (g_Aco_Job == 0xfa9) {
		sprintf(msg, "\xb0\xe1\xb1\xb9 \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb8\xa6 \xc5\xf0\xc4\xa1\xc7\xd2\xbc\xf6\xc0\xd6\xbe\xfa\xb0\xed \xc7\xc1\xb7\xd0\xc5\xd7\xb6\xf3\xb8\xa6 \xba\xf1\xb7\xd4\xc7\xd1 \xb8\xf0\xb5\xe7 \xb8\xb6\xc0\xbb\xbf\xa1 \xc6\xf2\xc8\xad\xb0\xa1 \xc3\xa3\xbe\xc6\xbf\xc9\xb4\xcf\xb4\xd9");
		g_renderer->TextOutScreen(40, g_scroll_Y, msg, 0xebebeb, 20, 1);
		sprintf(msg, "\xc0\xcc\xb7\xb8\xb0\xd4 \xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae %s\xc0\xc7 \xb8\xf0\xc7\xe8\xc0\xba \xbf\xa9\xb1\xe2\xbc\xad \xb3\xa1\xb3\xb3\xb4\xcf\xb4\xd9", g_session.GetCharName());
		g_renderer->TextOutScreen(40, g_scroll_Y + 60, msg, 0xebebeb, 20, 1);
		sprintf(msg, "\xb1\xd7\xb7\xb1\xb5\xa5 \xb2\xc0 \xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb1\xba\xc0\xc7 \xc8\xf1\xbb\xfd\xc0\xcc \xc7\xca\xbf\xe4\xc7\xdf\xc0\xbb\xb1\xee\xbf\xe4?");
		g_renderer->TextOutScreen(40, g_scroll_Y + 120, msg, 0xebebeb, 20, 1);
		sprintf(msg, "\xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb1\xba\xb0\xfa\xc0\xc7 \xc8\xa3\xb0\xa8\xb5\xb5\xb8\xa6 \xc3\xd6\xb4\xeb\xc4\xa1\xb7\xce \xb8\xb8\xb5\xe9\xb8\xe9 \xc1\xbb \xb4\xd9\xb8\xa5 \xc0\xcf\xc0\xcc \xb9\xfa\xbe\xee\xc1\xfa\xc1\xf6\xb5\xb5 \xb8\xf0\xb8\xa8\xb4\xcf\xb4\xd9");
		g_renderer->TextOutScreen(40, g_scroll_Y + 180, msg, 0xebebeb, 20, 1);
			sprintf(msg, "\xc5\xac\xb8\xae\xbe\xee\xc6\xc4\xc0\xcf\xb7\xce \xbb\xf5\xb7\xce \xbd\xc3\xc0\xdb\xc7\xcf\xbd\xc3\xb8\xe9 \xb7\xbe" "120 \xb1\xee\xc1\xf6\xb4\xc2 \xb0\xe6\xc7\xe8\xc4\xa1 5\xb9\xe8\xb8\xf0\xb5\xe5 \xc0\xd4\xb4\xcf\xb4\xd9.");
		g_renderer->TextOutScreen(40, g_scroll_Y + 240, msg, 0xebebeb, 20, 1);
		sprintf(msg, "                                Game Designed & Programmed by Si-Yeon Kim (\xb1\xe8\xbd\xc3\xbf\xac) tei2000kim@hanmail.net");
		g_renderer->TextOutScreen(40, g_scroll_Y + 340, msg, 0xebebeb, 20, 1);
		sprintf(msg, "                                                                                     in Ragnarok Division of GRAVITY 2008");
		g_renderer->TextOutScreen(40, g_scroll_Y + 370, msg, 0xebebeb, 20, 1);
		return;
	}

	sprintf(msg, "\xb1\xd7\xb7\xb8\xb0\xd4 \xc8\xfb\xb5\xe9\xb0\xd4 \xb5\xd1\xc0\xba \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb8\xa6 \xc5\xf0\xc4\xa1\xc7\xdf\xb0\xed \xc7\xc1\xb7\xd0\xc5\xd7\xb6\xf3\xb8\xa6 \xba\xf1\xb7\xd4\xc7\xd1 \xb8\xf0\xb5\xe7 \xb8\xb6\xc0\xbb\xbf\xa1 \xc6\xf2\xc8\xad\xb0\xa1 \xc3\xa3\xbe\xc6\xbf\xc9\xb4\xcf\xb4\xd9");
	g_renderer->TextOutScreen(40, g_scroll_Y, msg, 0xebebeb, 20, 1);
	sprintf(msg, "\xc0\xcc\xb7\xb8\xb0\xd4 %s\xb0\xfa \xc4\xab\xc4\xab\xb7\xce\xc6\xae\xb1\xba\xc0\xc7 \xb8\xf0\xc7\xe8\xc0\xba \xbf\xa9\xb1\xe2\xbc\xad \xc7\xe0\xba\xb9\xc7\xcf\xb0\xd4 \xb3\xa1\xb3\xb3\xb4\xcf\xb4\xd9", g_session.GetCharName());
	g_renderer->TextOutScreen(40, g_scroll_Y + 60, msg, 0xebebeb, 20, 1);
	sprintf(msg, "\xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\xb7\xce \xc0\xfc\xc1\xf7\xb5\xb5 \xc7\xcf\xc1\xf6\xbe\xca\xb0\xed \xb5\xd1\xc0\xc7 \xc8\xfb\xc0\xbb \xc7\xd5\xc7\xd8\xbc\xad \xc5\xb8\xb3\xaa\xc5\xe4\xbd\xba\xb8\xa6 \xc3\xb3\xc4\xa1\xc7\xcf\xb4\xd9\xb4\xcf...");
	g_renderer->TextOutScreen(40, g_scroll_Y + 120, msg, 0xebebeb, 20, 1);
	sprintf(msg, "\xc0\xfa \xbe\xc6\xb0\xa1\xbe\xbe\xc0\xc7 \xba\xce\xb8\xf0\xb4\xd4\xc0\xba \xc1\xa4\xb8\xbb \xb4\xeb\xb4\xdc\xc7\xcf\xbd\xc5\xb0\xc5 \xb0\xb0\xb3\xd7\xbf\xe4 ^^;   \xb8\xda\xc1\xae\xbf\xe4 !!");
	g_renderer->TextOutScreen(40, g_scroll_Y + 180, msg, 0xebebeb, 20, 1);
	sprintf(msg, "                                Game Designed & Programmed by Si-Yeon Kim (\xb1\xe8\xbd\xc3\xbf\xac) tei2000kim@hanmail.net");
	g_renderer->TextOutScreen(40, g_scroll_Y + 310, msg, 0xebebeb, 20, 1);
	sprintf(msg, "                                                                                     in Ragnarok Division of GRAVITY 2008");
	g_renderer->TextOutScreen(40, g_scroll_Y + 340, msg, 0xebebeb, 20, 1);
	sprintf(msg, "                  \xb3\xb2\xb4\xc2 \xbd\xc3\xb0\xa3\xbf\xa1 \xc6\xb4\xc6\xb4\xc8\xf7 \xb8\xb8\xb5\xe7 \xb9\xcc\xb4\xcfRPG \xc0\xce\xc1\xf6\xb6\xf3 \xc1\xbb \xb4\xf5 \xb8\xda\xc1\xf8 \xbf\xa3\xb5\xf9 \xc1\xd8\xba\xf1 \xb8\xf8\xc7\xd8\xbc\xad \xc1\xcb\xbc\xdb\xc7\xd5\xb4\xcf\xb4\xd9 \xa4\xd0_\xa4\xd0");
	g_renderer->TextOutScreen(40, g_scroll_Y + 375, msg, 0xebebeb, 20, 1);
}

void UIWindowMgr::MakeSaveFileName(char F1) {
	g_modeMgr.GetGameMode();
	MakeSaveFileName_IN(1, F1);
	MakeSaveFileName_IN(2, F1);
	MakeSaveFileName_IN(3, F1);
}

char UIWindowMgr::MakeSaveFileName_IN(char fileNum, char F1)
{
	char rName[32];
	char Thard[3];
	char Tword[2];
	short rDay;
	short rLv;
	short forNum;
	char rHard;
	char rHr;
	char rMin;
	char rClear;

	FILE* fp;
	if (fileNum == 1) {
		fp = fopen("SaveData1.KSY", "rb");
	} else if (fileNum == 2) {
		fp = fopen("SaveData2.KSY", "rb");
	} else {
		fp = fopen("SaveData3.KSY", "rb");
	}

	if (fp) {
		fread(&rDay, 2, 1, fp);
		fseek(fp, 0x14, 1);
		fread(&rLv, 2, 1, fp);
		fseek(fp, 0x10, 1);
		fread(&forNum, 2, 1, fp);
		fseek(fp, forNum * 4, 1);
		fseek(fp, 0x0a, 1);
		fread(&forNum, 2, 1, fp);
		fseek(fp, forNum * 2, 1);
		fseek(fp, 0x0b, 1);
		fread(rName, 0x20, 1, fp);
		fseek(fp, 4, 1);
		fread(&rHr, 1, 1, fp);
		fread(&rMin, 1, 1, fp);
		fseek(fp, 2, 1);
		fread(&rHard, 1, 1, fp);
		fseek(fp, 1, 1);
		fread(&rClear, 1, 1, fp);
		if (rClear != 1) {
			rClear = 0;
		}
		fclose(fp);

		if (F1 == 0) {
			sprintf(Tword, "N");
		} else {
			sprintf(Tword, "%d", fileNum);
		}

		if (rHard == 0) {
			sprintf(Thard, "\xf9\xbb");
		} else if (rHard == 1) {
			sprintf(Thard, "\xf1\xe9");
		} else {
			sprintf(Thard, "\xdf\xbe");
		}

		if (rClear == 1) {
			sprintf(g_savedata[fileNum - 1],
				"[CLEAR]  Day  %d  %s  Lv.%d  %02d:%02d  \xb3\xad\xc0\xcc\xb5\xb5 %s",
				rDay, rName, rLv, rHr, rMin, Thard);
			return rClear;
		} else {
			sprintf(g_savedata[fileNum - 1],
				"[%s]  Day  %d  %s  Lv.%d  %02d:%02d  \xb3\xad\xc0\xcc\xb5\xb5 %s",
				Tword, rDay, rName, rLv, rHr, rMin, Thard);
			return rClear;
		}
	}

	return 0;
}
