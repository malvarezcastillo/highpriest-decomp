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

		sprintf(msgZ, "[SAVE] Data has been saved");
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
		sprintf(msg, "A mini RPG I made in my spare time. Since I can't do graphics at all...");
		g_renderer->TextOutScreen(40, g_scroll_Y, msg, 0xffffff, 20, 1);
		sprintf(msg, "I took the title screen from fan art on the homepage.. orz");
		g_renderer->TextOutScreen(40, g_scroll_Y + 60, msg, 0xffffff, 20, 1);
		sprintf(msg, "The graphics are Ragnarok Online's own graphics");
		g_renderer->TextOutScreen(40, g_scroll_Y + 120, msg, 0xffffff, 20, 1);
		sprintf(msg, "Even if it's not your taste, please beat at least the first boss T_T");
		g_renderer->TextOutScreen(40, g_scroll_Y + 180, msg, 0xffffff, 20, 1);
		sprintf(msg, "                                Game Designed & Programmed by Si-Yeon Kim (\xb1\xe8\xbd\xc3\xbf\xac) tei2000kim@hanmail.net");
		g_renderer->TextOutScreen(40, g_scroll_Y + 310, msg, 0xffffff, 20, 1);
		sprintf(msg, "                                                                                     in Ragnarok Division of GRAVITY 2008");
		g_renderer->TextOutScreen(40, g_scroll_Y + 340, msg, 0xffffff, 20, 1);
		sprintf(msg, "When the scroll ends, press A to return to the title screen");
		g_renderer->TextOutScreen(524, g_scroll_Y + 372, msg, 0xffffff, 16, 1);
		return;
	}

	if (g_Aco_Job == 0xfa9) {
		sprintf(msg, "Thanatos was finally defeated, and peace came to Prontera and every town");
		g_renderer->TextOutScreen(40, g_scroll_Y, msg, 0xebebeb, 20, 1);
		sprintf(msg, "And so the adventure of High Priest %s ends here", g_session.GetCharName());
		g_renderer->TextOutScreen(40, g_scroll_Y + 60, msg, 0xebebeb, 20, 1);
		sprintf(msg, "But was Kakarot's sacrifice really necessary?");
		g_renderer->TextOutScreen(40, g_scroll_Y + 120, msg, 0xebebeb, 20, 1);
		sprintf(msg, "Max out your affection with Kakarot and something different may happen");
		g_renderer->TextOutScreen(40, g_scroll_Y + 180, msg, 0xebebeb, 20, 1);
			sprintf(msg, "Start a new game from a clear file for 5x EXP mode up to Lv 120.");
		g_renderer->TextOutScreen(40, g_scroll_Y + 240, msg, 0xebebeb, 20, 1);
		sprintf(msg, "                                Game Designed & Programmed by Si-Yeon Kim (\xb1\xe8\xbd\xc3\xbf\xac) tei2000kim@hanmail.net");
		g_renderer->TextOutScreen(40, g_scroll_Y + 340, msg, 0xebebeb, 20, 1);
		sprintf(msg, "                                                                                     in Ragnarok Division of GRAVITY 2008");
		g_renderer->TextOutScreen(40, g_scroll_Y + 370, msg, 0xebebeb, 20, 1);
		return;
	}

	sprintf(msg, "After a hard fight the two beat Thanatos, and peace came to Prontera and every town");
	g_renderer->TextOutScreen(40, g_scroll_Y, msg, 0xebebeb, 20, 1);
	sprintf(msg, "And so %s and Kakarot's adventure ends happily here", g_session.GetCharName());
	g_renderer->TextOutScreen(40, g_scroll_Y + 60, msg, 0xebebeb, 20, 1);
	sprintf(msg, "Defeating Thanatos together without even becoming a High Priest...");
	g_renderer->TextOutScreen(40, g_scroll_Y + 120, msg, 0xebebeb, 20, 1);
	sprintf(msg, "That young lady's parents must be amazing ^^;   Awesome !!");
	g_renderer->TextOutScreen(40, g_scroll_Y + 180, msg, 0xebebeb, 20, 1);
	sprintf(msg, "                                Game Designed & Programmed by Si-Yeon Kim (\xb1\xe8\xbd\xc3\xbf\xac) tei2000kim@hanmail.net");
	g_renderer->TextOutScreen(40, g_scroll_Y + 310, msg, 0xebebeb, 20, 1);
	sprintf(msg, "                                                                                     in Ragnarok Division of GRAVITY 2008");
	g_renderer->TextOutScreen(40, g_scroll_Y + 340, msg, 0xebebeb, 20, 1);
	sprintf(msg, "                  This mini RPG was made in spare time, so sorry I couldn't make a nicer ending T_T");
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
	char Thard[8];
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
			sprintf(Thard, "Easy");
		} else if (rHard == 1) {
			sprintf(Thard, "Normal");
		} else {
			sprintf(Thard, "Hard");
		}

		if (rClear == 1) {
			sprintf(g_savedata[fileNum - 1],
				"[CLEAR]  Day  %d  %s  Lv.%d  %02d:%02d  Mode %s",
				rDay, rName, rLv, rHr, rMin, Thard);
			return rClear;
		} else {
			sprintf(g_savedata[fileNum - 1],
				"[%s]  Day  %d  %s  Lv.%d  %02d:%02d  Mode %s",
				Tword, rDay, rName, rLv, rHr, rMin, Thard);
			return rClear;
		}
	}

	return 0;
}
