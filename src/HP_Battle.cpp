#include "common.h"
#include "device/Sound.h"


#include "resource/Wave.h"
#include "Session.h"
#include "window/UIFrameWnd.h"
#include "GameMode.h"
#include "GameActor.h"
#include "GameWorld.h"
#include "Mode.h"
#include "HP_Tetris.h"
#include "RagEffect.h"
#include "SkillInfo.h"
#include "base/Res.h"
#include "device/Control.h"
#include "resource/Skybox.h"
#include <algorithm>
#include <mmsystem.h>
#include "resource/Ground.h"

extern CSession g_session;
extern CModeMgr g_modeMgr;
extern UIWindowMgr g_windowMgr;
extern CMouse g_mouse;

extern unsigned long g_HelpNextActTime;
extern unsigned long g_MainNextActTime;
extern unsigned long g_MobNextOutTime;
extern int g_curStance;
extern int g_curScreen;
extern int g_BattleEvent;
extern int g_NextSkill;
extern short map_BNPC_ID;
extern short g_ScreenConv_Alpha;
extern short g_FieldConv_Alpha;
extern short g_AcoPosition;
extern short g_curStory;
extern short map_Number;
extern int g_used_Skill;
extern int g_Mob_used_Skill;
extern CRenderer* g_renderer;
extern short map_firstway;
extern short map_BNPC_Pos;
extern bool map_BNPC_done;
extern bool map_BNPC_event;
extern short Boy_talked;
extern short map_secondstart;
extern short map_secondway;
extern short map_thirdstart;
extern short map_thirdway;
extern bool map_BOSS_done;
extern short map_Heal_Used;
extern short map_ComboF_Used;
extern int g_Frame;
extern short g_PoringCurX;
extern short g_PoringCurY;
extern short g_PoringTarY;
extern short g_PoringTarX;
extern short g_menuSelect;
extern int g_curRectTop;
extern int g_curStance_B;
extern int g_isSkipFrame;
extern short g_SpellSeek;
extern unsigned long g_SpellSeekTime;
extern char g_curHardness;
extern char g_curSave;
extern short g_menuFreeze;
extern short g_selected_item;
extern short g_BS_phase;
extern unsigned long g_BS_NextActTime;
extern char g_curEnding;
extern bool g_MobNextTimeFixed;
extern short g_PrevMob;
extern bool Boy_frame;
extern char bat_ChookGi;
extern short bat_STUN_turn;
extern short bat_mobskill_turn;
extern short bat_mouse_pos;
extern short g_Aco_Job;
extern short Boy_friendly;
extern short Boy_Job;
extern unsigned long g_BoyAttackedTime;
extern unsigned long g_used_Skill_Time;
extern bool g_WordDisplay;
extern Tetris* TT;
extern char g_savedata[][256];
extern void PlayWave(const char* waveFileName, float x, float y, float z,
	int volumeMaxDist, int volumeMinDist, float vFactor);

void UIWindowMgr::RenderBackGround2D()
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};

	if (g_curScreen < 100) {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xb0\xd4\xc7\xc1\xc7\xca\xb5\xe5.jpg");
	} else {
		sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xc7\xcf\xc7\xc1""0206.jpg");
	}

	rect.left = 0;
	rect.top = 0;
	rect.right = 1024;
	rect.bottom = 768;
	gameMode->DrawBackGround2D(&rect, 0, 0.002f,
		0xffffffff, g_texMgr.GetTexture(temp, false), UVRECT(0.0f, 0.0f, 1.0f, 1.0f));

	if (g_isSkipFrame > 0) {
		++g_Frame;
	}

	if (g_curScreen == 0) {
		RenderBackGround2D_House();
		HP_Resting();
	} else if (g_curScreen == 1) {
		RenderBackGround2D_House();
	} else if (g_curScreen >= 50 && g_curScreen < 60) {
		RenderBackGround2D_House();
	} else if (g_curScreen == 2) {
		RenderBackGround2D_House();
		RenderTravelMap();
	} else if (g_curScreen == 3) {
		RenderBackGround2D_House();
		g_ScreenConv_Alpha += 2;
		if (g_ScreenConv_Alpha > 253) {
			g_ScreenConv_Alpha = 0;
			SetcurScreen(4);
		}
	} else if (g_curScreen == 4) {
		RenderBackGround2D_Travel();
		g_ScreenConv_Alpha += 2;
		if (g_ScreenConv_Alpha > 253) {
			g_ScreenConv_Alpha = 0;
			SetcurScreen(5);
			g_MainNextActTime = timeGetTime() + 1000;
			g_curStance = 0;
		}
	} else if (g_curScreen == 5) {
		RenderBackGround2D_Travel();
		Aco_Travel();
	} else if (g_curScreen == 6) {
		RenderBackGround2D_Travel();
		g_ScreenConv_Alpha += 2;
		if (g_ScreenConv_Alpha > 253) {
			g_ScreenConv_Alpha = 0;
			SetcurScreen(21);
		}
	} else if (g_curScreen == 10) {
		RenderBackGround2D_House();
		g_ScreenConv_Alpha += 2;
		if (g_ScreenConv_Alpha > 253) {
			g_ScreenConv_Alpha = 0;
			SetcurScreen(11);
		}
	} else if (g_curScreen == 11) {
		RenderBackGround2D_Battle();
		g_ScreenConv_Alpha += 2;
		if (g_ScreenConv_Alpha > 253) {
			g_ScreenConv_Alpha = 0;
			SetcurScreen(12);
		}
	} else if (g_curScreen == 12) {
		RenderBackGround2D_Battle();
		All_CheckInput();
		if (g_curStance == 0) {
			Aco_ActinField();
			Mob_ActinField();
			Boy_ActinField();
		} else if (g_curStance == 1) {
			All_CheckPos();
		} else if (g_curStance == 2) {
			Aco_Battle();
			Mob_Battle();
			Help_Battle();
		} else if (g_curStance >= 3 && g_curStance <= 4
			&& g_curStance_B == 0) {
			if (g_MainNextActTime < timeGetTime()) {
				Aco_Revive(1);
				SetcurScreen(20);
			}
		} else if (g_curStance == 5) {
			Aco_DecideinField();
		} else if (g_curStance == 6 || g_curStance == 7) {
			Aco_MoveByDec();
		}

		if (g_curStance_B == 1) {
			BS_Tetris();
		} else if (g_curStance_B == 2) {
			BS_Selling();
		} else if (g_curStance_B == 3) {
			BS_Buying();
		} else if (g_curStance_B == 11) {
			BS_Making();
		}
	} else if (g_curScreen == 20) {
		RenderBackGround2D_Battle();
		g_ScreenConv_Alpha += 2;
		if (g_ScreenConv_Alpha > 253) {
			g_ScreenConv_Alpha = 0;
			SetcurScreen(21);
		}
	} else if (g_curScreen == 21) {
		RenderBackGround2D_House();
		g_ScreenConv_Alpha += 2;
		if (g_ScreenConv_Alpha > 253) {
			g_ScreenConv_Alpha = 0;
			SetcurScreen(1);
			CGameActor* actor = g_session.GetGameActorByAid(1);
			if (actor) {
				actor->SetState(6);
			}
		}
	}
}

void UIWindowMgr::Aco_Battle()
{
	SKILLSTRUCT ss;
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	SKILL_INFO sInfo;
	char PASS;
	int AttackDamage;
	int BasicDamage;
	int MobDefense;
	int HealAmount;

	CGameActor* MobActor = g_session.GetGameActorByAid(100);
	CGameActor* AcoActor = g_session.GetGameActorByAid(1);
	CGameActor* HelpActor = g_session.GetGameActorByAid(map_BNPC_ID);

	PASS = 0;
	AttackDamage = 0;
	ss.skillId = 0;
	if (!MobActor || !AcoActor)
		goto Cleanup;

	if (!AcoActor->IsEffectExist(0x2c1)
		&& !AcoActor->IsEffectExist(0x2c2)
		&& !AcoActor->IsEffectExist(0x2c3)) {
		stateId.erase(std::remove(stateId.begin(), stateId.end(), 10), stateId.end());
		gameMode->GetWorld()->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
	}

	if (!AcoActor->IsEffectExist(0x2c4)) {
		AcoActor->SendMsg(NULL, AM_DELETE_EFFECT, 0xcb, 0, 0);
	}

	if (AcoActor->GetBodyState() == ST_ATTACK) {
		int mouse = g_mouse.GetXPos();
		short delta = (short)(mouse - bat_mouse_pos);
		if (delta < 0)
			delta = (short)-delta;
		short turn = (short)(delta / 20);
		bat_STUN_turn = (short)(bat_STUN_turn + turn);
		bat_mouse_pos = (short)mouse;
		if ((short)(delta / 20) > 0) {
			AcoActor->SendMsg(NULL, AM_SHAKE_ICEEFFECT, 0, 0, 0);
		}
	}

	if (g_MainNextActTime >= timeGetTime() || AcoActor->m_stateId == ST_DEAD)
		goto Cleanup;

	g_MobNextOutTime = timeGetTime() + 1000;
	g_MainNextActTime = timeGetTime() + 5000;
	AcoActor->DeleteChatBalloon();

	BasicDamage = gameMode->GetWeaponPower(AcoActor->GetWeapon())
		+ rand() % 11
		+ AcoActor->m_Str * 10
		- 5;
	BasicDamage += (gameMode->GetWeaponPower(AcoActor->GetWeapon()) * bat_ChookGi) / 5;

	if (AcoActor->IsEffectExist(0x2c1)) {
		BasicDamage = (int)(BasicDamage * 1.3f);
	} else if (AcoActor->IsEffectExist(0x2c2)) {
		BasicDamage = (int)(BasicDamage * 1.4f);
	} else if (AcoActor->IsEffectExist(0x2c3)) {
		BasicDamage = (int)(BasicDamage * 1.5f);
	}
	MobDefense = MobActor->m_Vit * 5;
	MOVESTRUCT ms;

	if (AcoActor->GetBodyState() == ST_DEAD) {
		++bat_STUN_turn;
		if (bat_STUN_turn >= 3) {
			bat_STUN_turn = 0;
			AcoActor->SetAttrState(0, AcoActor->GetEffectState(), AcoActor->GetHealthState(), true);
		} else {
			PASS = 10;
			g_MobNextOutTime = timeGetTime() + 500;
		}
	}

	if (AcoActor->GetBodyState() == ST_ATTACK) {
		bat_STUN_turn = (short)(bat_STUN_turn + 100);
		if (bat_STUN_turn >= 400) {
			bat_STUN_turn = 0;
			AcoActor->SetAttrState(0, AcoActor->GetEffectState(), AcoActor->GetHealthState(), true);
		} else {
			PASS = 10;
			g_MobNextOutTime = timeGetTime() + 500;
		}
	}

	if (g_used_Skill == 0x406) {
		ss.skillId = 0x21;
		ss.startTime = timeGetTime() + 300;
		ss.level = 1;
		PASS = 3;
		g_NextSkill = 0;
	} else if (g_used_Skill == 0x3ff) {
		AttackDamage = BasicDamage * 20 - MobDefense;
		++map_ComboF_Used;
		ss.skillId = 0x10f;
		ss.action = 8;
		ss.damage = AttackDamage;
		ss.startTime = timeGetTime();
		ss.targetGID = 100;
		ss.level = 1;
		ss.count = 1;
		ss.tCellX = -1;
		ss.tCellY = -1;
		ss.attackMT = 0x118;
		ss.attackedMT = 1;
		PASS = 1;
	} else if (g_NextSkill > 0 && PASS < 10) {
		sInfo = g_session.GetSkillItemInfoBySkillId(g_NextSkill);

		if (g_NextSkill == 0x111) {
			if (sInfo.level <= 2) {
				AttackDamage = BasicDamage * 2 - MobDefense;
			} else if (sInfo.level == 3) {
				AttackDamage = BasicDamage * 4 - MobDefense;
			}
			if (AcoActor->IsEffectExist(0x2c4)) {
				AttackDamage = (int)(AttackDamage * 3.0f);
			}

			++map_ComboF_Used;
			ss.skillId = 0x111;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime();
			ss.targetGID = 100;
			ss.level = 1;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			if (sInfo.level >= 2) {
				MobActor->SendMsg(NULL, AM_UPDATEGAGE, MobActor->m_Hp, MobActor->m_MaxHp, 0);
			}
		} else if (g_NextSkill == 0x10f) {
			ss.skillId = 0x3ff;
			PASS = 4;
			g_MainNextActTime = timeGetTime() + 2500;
			g_MobNextOutTime = timeGetTime() + 5000;
		} else if (g_NextSkill == 0x1c) {
			AttackDamage = -1;
			HealAmount = rand() % 10 + (AcoActor->m_Int + 19) * sInfo.level * 20;
			if (AcoActor->IsEffectExist(0x2c1)) {
				HealAmount = (int)(HealAmount * 1.3f);
			} else if (AcoActor->IsEffectExist(0x2c2)) {
				HealAmount = (int)(HealAmount * 1.4f);
			} else if (AcoActor->IsEffectExist(0x2c3)) {
				HealAmount = (int)(HealAmount * 1.5f);
			}
			++map_Heal_Used;
			ss.skillId = 0x1c;
			ss.startTime = timeGetTime() + 300;
			ss.level = 1;
			PASS = 2;
		} else if (g_NextSkill == 0x35) {
			ss.skillId = g_NextSkill;
			ss.startTime = timeGetTime() + 300;
			ss.level = 1;
			if (AcoActor->GetHealthState() & 1) {
				PASS = 3;
			} else {
				PASS = 10;
				AcoActor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
			}
		} else if (g_NextSkill == 0x1b) {
			ss.skillId = g_NextSkill;
			PASS = 10;
			AcoActor->SendMsg(NULL, AM_EMOTION, 3, 0, 0);
		} else if (g_NextSkill == 0x21) {
			ss.skillId = 0x406;
			PASS = 4;
		} else if (g_NextSkill == 0x49) {
			ss.skillId = g_NextSkill;
			ss.startTime = timeGetTime() + 300;
			ss.level = sInfo.level;
			PASS = 3;
		} else if (g_NextSkill == 0x19d) {
			if (sInfo.level == 1) {
				AttackDamage = BasicDamage * 2 - MobDefense;
			} else if (sInfo.level == 2) {
				AttackDamage = BasicDamage * 4 - MobDefense;
			}
			++map_ComboF_Used;
			ss.skillId = 0x19d;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime();
			ss.targetGID = 100;
			ss.level = 5;
			ss.count = 2;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
		} else if (g_NextSkill == 0x22) {
			ss.skillId = g_NextSkill;
			ss.startTime = timeGetTime() + 300;
			ss.level = sInfo.level;
			PASS = 3;
		} else if (g_NextSkill == 0x10e) {
			ss.skillId = g_NextSkill;
			ss.startTime = timeGetTime() + 300;
			ss.level = sInfo.level;
			PASS = 3;
		}
		g_NextSkill = 0;
	}

	if (PASS == 0) {
		if (g_session.HaveThisSkill(0x107) >= 1) {
			sInfo = g_session.GetSkillItemInfoBySkillId(0x107);
			if (rand() % 10 < sInfo.level + 2) {
				AttackDamage = BasicDamage * 2 - MobDefense;
				PASS = 1;
				ss.skillId = 0x107;
				ss.action = 8;
				ss.damage = AttackDamage;
				ss.startTime = timeGetTime() + 300;
				ss.targetGID = 100;
				ss.level = 1;
				ss.count = 3;
				ss.tCellX = -1;
				ss.tCellY = -1;
				ss.attackMT = 0x118;
				ss.attackedMT = 1;
			}
		}
	}

	if ((MobActor->GetJob() == 0x5d9 || MobActor->m_isSprArgbFixed == 1)
		&& ss.skillId != 0x19d
		&& AttackDamage > 1) {
		AttackDamage = 1;
		ss.damage = (ss.skillId == 0x107) ? 3 : 1;
	}
	if (MobActor->GetJob() == 0x509) {
		if (ss.skillId == 0x111) {
			ss.damage = 1;
			g_session.SetDamageSituation(timeGetTime() + 100, 0x65, 0x150, true);
		}
	}
	if (MobActor->GetJob() == 0x6ac) {
		AttackDamage /= 10;
		ss.damage /= 10;
	}

	if (PASS > 0 && PASS < 10) {
		sInfo = g_session.GetSkillItemInfoBySkillId(ss.skillId);
		if (ss.skillId == 0x10f) {
			sInfo.spcost = 0;
		}
		if (PASS == 4) {
			sInfo.spcost = 0;
		}

		if (AcoActor->m_Sp - sInfo.spcost >= 0) {
			AcoActor->m_Sp = (short)(AcoActor->m_Sp - sInfo.spcost);
			if (ss.skillId == 0x19d) {
				if (MobActor->m_isSprArgbFixed == 1) {
					MobActor->m_isSprArgbFixed = 0;
				}
			}

			if (PASS == 1) {
				AcoActor->SendMsg(NULL, AM_SKILL, (int)&ss, 0, 0);
				gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)AcoActor, ss.level);
			} else if (PASS == 2) {
				if (ss.skillId == 0x1c) {
					AcoActor->SetState(ST_SKILL);
					AcoActor->SendMsg(NULL, AM_MAKE_NUMBER, HealAmount, 0xff00ff00, 0x72);
					g_session.SetDamageSituation(timeGetTime() + 1, 1, -HealAmount, false);
				}
				gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)AcoActor, ss.level);
			} else if (PASS == 3) {
				AcoActor->SetState(ST_SKILL);
				if (ss.skillId == 0x35) {
					AcoActor->LaunchEffect(0x15, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					AcoActor->SetAttrState(AcoActor->GetBodyState(),
						AcoActor->GetEffectState(), AcoActor->GetHealthState() - 1, true);
				} else if (ss.skillId == 0x21) {
					AcoActor->LaunchEffect(0x29, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					stateId.erase(std::remove(stateId.begin(), stateId.end(), 9), stateId.end());
					stateId.push_back(9);
					gameMode->GetWorld()->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
				} else if (ss.skillId == 0x49) {
					AcoActor->LaunchEffect(0x70, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					g_MobNextOutTime = timeGetTime() + 1500;
					stateId.erase(std::remove(stateId.begin(), stateId.end(), 0x13), stateId.end());
					stateId.push_back(0x13);
					gameMode->GetWorld()->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
				} else if (ss.skillId == 0x22) {
					AcoActor->LaunchEffect(0x2a, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					g_MobNextOutTime = timeGetTime() + 1500;
					stateId.erase(std::remove(stateId.begin(), stateId.end(), 10), stateId.end());
					stateId.push_back(10);
					gameMode->GetWorld()->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
					if (ss.level == 1) {
						AcoActor->LaunchEffect(0x2c1, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					} else if (ss.level == 2) {
						AcoActor->LaunchEffect(0x2c2, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					} else if (ss.level == 3) {
						AcoActor->LaunchEffect(0x2c3, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					}
				} else if (ss.skillId == 0x10e) {
					g_modeMgr.GetGameMode()->SendMsg(MM_QUAKE, 0, 0, 0);
					PlayWave("effect\\mon_\xc6\xf8\xb1\xe2.wav",
						0.0f, 0.0f, 0.0f, 250, 40, 1.0f);
					AcoActor->SendMsg(NULL, AM_DELETE_EFFECT, 0xcb, 0, 0);
					AcoActor->LaunchEffect(0xcb, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					AcoActor->LaunchEffect(0x105, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
					g_MobNextOutTime = timeGetTime() + 1500;
					AcoActor->LaunchEffect(0x2c4, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
				}
				gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)AcoActor, ss.level);
			} else if (PASS == 4) {
				if (ss.skillId == 0x406) {
					AcoActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x3a, 0xfa, 0);
				} else if (ss.skillId == 0x3ff) {
					AcoActor->LaunchEffect(0x1fe, vector3d(0.0f, 0.0f, 0.0f), 0.0f);
				}
			}
			g_session.SetStatusData(AcoActor);
		} else {
			char msg[256];
			sprintf(msg, "Not enough SP for your daughter's skill");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xc8c8ff, 0, 0);
			PASS = 0;
		}
	}

	if (PASS == 0) {
		AcoActor->SetState(ST_ATTACK);
		AttackDamage = BasicDamage - MobDefense;
		if (MobActor->GetJob() == 0x6ac) {
			AttackDamage /= 10;
		}
		if ((MobActor->GetJob() == 0x5d9 || MobActor->m_isSprArgbFixed == 1)
			&& AttackDamage > 1) {
			AttackDamage = 1;
		}
		if (AttackDamage > 0) {
			g_session.SetDamageSituation(timeGetTime() + 300, MobActor->GetGID(), AttackDamage, true);
		}
	} else if (AttackDamage > 0) {
		g_session.SetDamageSituation(timeGetTime() + 300, MobActor->GetGID(), AttackDamage, false);
	}

	if (AcoActor->GetHealthState() & 1) {
		AcoActor->SendMsg(NULL, AM_MAKE_NUMBER, AcoActor->m_MaxHp / 20, 0xffff0000, 0x72);
		AcoActor->m_Hp = (short)(AcoActor->m_Hp - AcoActor->m_MaxHp / 20);
		if (AcoActor->m_Hp < 1)
			AcoActor->m_Hp = 1;
		g_session.SetStatusData(AcoActor);
	}

	g_used_Skill = ss.skillId;
	g_used_Skill_Time = ss.startTime;

	if (HelpActor) {
		if (HelpActor->GetGID() == 3) {
			gameMode->GetServerCoor(HelpActor->m_pos.x, HelpActor->m_pos.z, ms.sx, ms.sy);
			if (ms.sx >= 0x13 && HelpActor->m_stateId != ST_ATTACK) {
				ms.dx = 0x0f;
				ms.dy = rand() % 7 - 3;
				ms.cellX = 0;
				ms.cellY = 0;
				HelpActor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);
			}
		}
	}
Cleanup:
	;
}

void UIWindowMgr::Help_Battle()
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	SKILLSTRUCT ss;
	int AttackDamage;

	CGameActor* HelpActor = g_session.GetGameActorByAid(map_BNPC_ID);
	CGameActor* AcoActor = g_session.GetGameActorByAid(1);
	CGameActor* MobActor = g_session.GetGameActorByAid(100);

	if (!HelpActor || !MobActor)
		return;

	if (map_BNPC_ID == 5) {
		if (HelpActor->m_stateId == ST_STAND) {
			HelpActor->SetState(ST_ATTACKWAIT);
		}
	}

	if (g_HelpNextActTime < timeGetTime()) {
	if (map_BNPC_ID == 3) {
		if (map_Number == 15) {
			g_HelpNextActTime = timeGetTime() + rand() % 9 * 1000 + 1500;
		} else {
			g_HelpNextActTime = timeGetTime() + rand() % 20 * 1000 + 1500;
		}

		g_BoyAttackedTime = timeGetTime() + 2000;

		AttackDamage = gameMode->GetWeaponPower(AcoActor->GetWeapon())
			+ rand() % 11
			+ AcoActor->m_Str * 10
			- 5;

		if (Boy_Job == 1) {
			ss.skillId = 5;
		} else if (Boy_Job == 7) {
			AttackDamage = (int)(AttackDamage * 1.5f);
			ss.skillId = 0x3e;
		} else {
			AttackDamage *= 2;
			ss.skillId = 0x3e;
		}

		if (MobActor->GetJob() == 0x6ac)
			AttackDamage /= 10;

		ss.action = 8;
		ss.damage = AttackDamage;
		ss.startTime = timeGetTime() + 300;
		ss.targetGID = 100;
		ss.level = 1;
		ss.count = 1;
		ss.tCellX = -1;
		ss.tCellY = -1;
		ss.attackMT = 0x118;
		ss.attackedMT = 1;

		HelpActor->SendMsg(NULL, AM_SKILL, (int)&ss, 0, 0);
		gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)HelpActor, ss.level);

		if (AttackDamage > 0) {
			g_session.SetDamageSituation(timeGetTime() + 300,
				MobActor->GetGID(), AttackDamage, false);
		}
	} else if (map_BNPC_ID == 5) {
		g_HelpNextActTime = timeGetTime() + 999000;
		short action;
		if (g_curStory >= 60)
			action = rand() % 5;
		else
			action = rand() % 10;

		if (MobActor->m_clevel >= 90)
			action = 10;
		if (MobActor->GetJob() == 0x55e)
			action = 0;
		if (MobActor->GetJob() == 0x781 && action == 0)
			action = 1;

		if ((MobActor->GetJob() == 0x691 && action == 1) || action == 0) {
			ss.skillId = 0x1bf;
			ss.level = 1;
			HelpActor->SetState(ST_SKILL);
			gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)HelpActor, ss.level);
			HelpActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x1b9, 0xfa, 0);

			vector3d pos;
			pos.y = (float)AcoActor->GetGID();
			CRagEffect* effect = AcoActor->LaunchEffect(0x1f7);
			if (effect)
				effect->SendMsg(NULL, AM_SETPOS, (int)&pos, 0, 0);

			if (AcoActor->IsEffectExist(0x1a8))
				AcoActor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1a8, 0, 0);
			AcoActor->LaunchEffect(0x1a8);
		} else if (action == 1) {
			ss.skillId = 0x1d6;
			ss.level = 1;
			HelpActor->SetState(ST_SKILL);
			gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)HelpActor, ss.level);
			HelpActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x1b9, 0xfa, 0);
			PlayWave("effect\\T_\xbd\xb4\xbf\xf4.wav", 0, -50.0f, 0, 250, 40, 1.0f);
			if (MobActor->IsEffectExist(0x1a4))
				MobActor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1a4, 0, 0);
			MobActor->LaunchEffect(0x1a4);
			MobActor->LaunchEffect(0x24d);
		} else if (action == 10) {
			HelpActor->SendMsg(NULL, AM_CHAT,
				(int)"Bosses are scary T_T",
				0, 0);
			HelpActor->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
		}
	} else if (map_BNPC_ID == 6) {
	g_HelpNextActTime = timeGetTime() + rand() % 5 * 1000 + 2500;

	if (HelpActor->m_stateId != ST_ATTACKWAIT) {
		HelpActor->SetState(ST_ATTACKWAIT);
	} else if (MobActor->GetJob() == 0x5d9) {
		g_MainNextActTime = timeGetTime() + 5000;
		AttackDamage = rand() % 11 + MobActor->m_Vit * 10 - 5;
		ss.skillId = 0x10a;

		ss.action = 8;
		ss.damage = AttackDamage;
		ss.startTime = timeGetTime() + 300;
		ss.targetGID = 100;
		ss.level = 1;
		ss.count = 1;
		ss.tCellX = -1;
		ss.tCellY = -1;
		ss.attackMT = 0x118;
		ss.attackedMT = 1;

		HelpActor->SendMsg(NULL, AM_SKILL, (int)&ss, 0, 0);
		gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)HelpActor, ss.level);

		if (AttackDamage > 0) {
			g_session.SetDamageSituation(timeGetTime() + 300,
				MobActor->GetGID(), AttackDamage, false);
		}
	} else if (!HelpActor->IsEffectExist(0xe4)) {
		ss.skillId = 0x105;
		ss.level = 1;
		HelpActor->SetState(ST_SKILL);
		HelpActor->SetForceAnimation(0x60, 0, 0, 0x60, 0, FALSE, 0, FALSE);
		gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)HelpActor, ss.level);
		HelpActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x0c, 0xbb, 0);
		vector3d pos;
		CRagEffect* effect = HelpActor->LaunchEffect(0xe4);
		pos.x = 1.0f;
		if (effect)
			effect->SendMsg(NULL, AM_SETPOS, (int)&pos, 0, 0);
	} else if (bat_ChookGi < 5) {
		ss.skillId = 0x3f7;
		ss.level = 1;
		HelpActor->SetState(ST_SKILL);
		HelpActor->SetForceAnimation(0x60, 0, 0, 0x60, 0, FALSE, 0, FALSE);
		gameMode->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)HelpActor, ss.level);
		HelpActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x1b9, 0xbb, 0);
		AcoActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x1b9, 0x3e, 0);
		HelpActor->SendMsg(NULL, AM_DELETE_EFFECT, 0xe4, 0, 0);
		AcoActor->SendMsg(NULL, AM_DELETE_EFFECT, 0xe4, 0, 0);

		vector3d pos;
		CRagEffect* effect = AcoActor->LaunchEffect(0xe4);
		++bat_ChookGi;
		if (bat_ChookGi > 5)
			bat_ChookGi = 5;

		pos.x = (float)bat_ChookGi;
		if (effect)
			effect->SendMsg(NULL, AM_SETPOS, (int)&pos, 0, 0);
	}
	}
	}
}

void UIWindowMgr::Mob_Battle()
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	CGameActor* MobActor = g_session.GetGameActorByAid(100);
	CGameActor* AcoActor = g_session.GetGameActorByAid(1);
	CGameActor* HelpActor = g_session.GetGameActorByAid(map_BNPC_ID);
	char PASS = 0;
	int HealAmount = 0;
	int Attacked_ID = 1;
	MOVESTRUCT ms;
	SKILLSTRUCT ss;
	int AcoDefense;
	int Mob_SKID;
	int BasicDamage;
	int AttackDamage;

	if (!MobActor || !AcoActor)
		return;

	if (g_MobNextOutTime < timeGetTime() && MobActor->m_stateId != ST_DEAD) {
		MobActor->DeleteChatBalloon();
		g_MainNextActTime = timeGetTime() + 1000;
		g_MobNextOutTime = timeGetTime() + 5000;

		BasicDamage = rand() % 11 + MobActor->m_Str * 10 - 5;
		AcoDefense = AcoActor->m_Vit * 5;
		if (MobActor->IsEffectExist(0x1ab))
			BasicDamage *= 2;
		if (MobActor->IsEffectExist(0x1a4))
			BasicDamage = (int)(BasicDamage * 0.3f);
		if (std::find(stateId.begin(), stateId.end(), 9) != stateId.end())
			BasicDamage = (int)(BasicDamage * 0.9f);

		AttackDamage = BasicDamage - AcoDefense;
		Mob_SKID = g_session.Mob_SkillReturn(MobActor);
		++g_BattleEvent;
		ss.skillId = 0;

		if (MobActor->m_isSprArgbFixed == 1) {
			HealAmount = MobActor->m_MaxHp / 20;
			g_session.SetDamageSituation(timeGetTime(), 100, -HealAmount, false);
			PASS = 4;
			++bat_mobskill_turn;
			if (bat_mobskill_turn >= 6) {
				bat_mobskill_turn = 0;
				MobActor->m_isSprArgbFixed = 0;
			}
		} else if (g_Mob_used_Skill == 0x3fe) {
			AttackDamage = BasicDamage * 3;
			ss.skillId = 0x14;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 10;
			ss.count = 10;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 3000;
		} else if (g_Mob_used_Skill == 0x40b) {
			AttackDamage = BasicDamage * 9;
			ss.skillId = 0x15;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 10;
			ss.count = 10;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
		} else if (g_Mob_used_Skill == 0x40c) {
			AttackDamage = BasicDamage * 10 - AcoDefense;
			ss.skillId = 0xfe;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 10;
			ss.count = 5;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
		} else if (g_Mob_used_Skill == 0x3ff || g_Mob_used_Skill == 0x40d) {
			AttackDamage = AcoActor->m_Hp - 1;
			ss.skillId = 0x10f;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime();
			ss.targetGID = 1;
			ss.level = 1;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x98) {
			AttackDamage = BasicDamage * 2 - AcoDefense;
			ss.skillId = 0x98;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 1;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
		} else if (Mob_SKID == 0x13) {
			AttackDamage = BasicDamage * 3;
			ss.skillId = 0x13;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 10;
			ss.count = 10;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 3000;
		} else if (Mob_SKID == 0x3fd) {
			AttackDamage = 0;
			PASS = 3;
			if (MobActor->GetJob() == 0x6c5) {
				MobActor->SendMsg(NULL, AM_CHAT, (int)"Argh.. Guillotine Fist canceled !!", 0, 0);
				MobActor->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
			} else if (MobActor->GetJob() == 0x6f9) {
				MobActor->SendMsg(NULL, AM_CHAT, (int)"Aco Kaboom, ouch T_T", 0, 0);
				MobActor->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
			} else {
				MobActor->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
			}
		} else if (Mob_SKID == 0x40e) {
			AttackDamage = 0;
			PASS = 3;
			MobActor->SendMsg(NULL, AM_CHAT, (int)"Your accessories catch my eye~", 0, 0);
			MobActor->SendMsg(NULL, AM_EMOTION, 0x1f, 0, 0);
		} else if (Mob_SKID == 0x403) {
			AttackDamage = 0;
			PASS = 4;
			MobActor->SendMsg(NULL, AM_CHAT, (int)"Guillotine Fist ...", 0, 0);
		} else if (Mob_SKID == 0x404) {
			AttackDamage = 0;
			PASS = 4;
			MobActor->SendMsg(NULL, AM_CHAT, (int)"Haaaaah", 0, 0);
		} else if (Mob_SKID == 0x1c) {
			PASS = 2;
			AttackDamage = -1;
			HealAmount = rand() % 11 + MobActor->m_Int * 50 - 5;
			ss.skillId = 0x1c;
			ss.startTime = timeGetTime();
			ss.level = 1;
			ss.targetGID = 100;
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0xe7) {
			AttackDamage = 0;
			PASS = 2;
			CRagEffect* effect = MobActor->LaunchEffect(0x12b);
			if (effect) {
				vector3d tempPos = AcoActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, (int)&tempPos, 0, 0);
				effect->SendMsg(NULL, AM_SETCOUNT, 3, 0, 0);
			}
			ss.skillId = 0xe7;
			ss.startTime = timeGetTime();
			ss.level = 1;
			ss.targetGID = 1;
			HealAmount = rand() % 11 + MobActor->m_Int * 50 - 5;
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x88) {
			AttackDamage = BasicDamage * 3 - AcoDefense;
			ss.skillId = 0x88;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 1;
			ss.count = 4;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			MobActor->SetState(ST_ATTACK);
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x3fe || Mob_SKID == 0x40b) {
			AttackDamage = 0;
			MobActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x37, 0xbb, 0);
			PASS = 4;
			g_MainNextActTime = timeGetTime() + 1000;
		} else if (Mob_SKID == 0x3ff) {
			AttackDamage = 0;
			MobActor->LaunchEffect(0x1fe);
			PASS = 4;
			g_MainNextActTime = timeGetTime() + 5000;
			g_MobNextOutTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x40d) {
			AttackDamage = 0;
			MobActor->LaunchEffect(0x1fe);
			PASS = 4;
		} else if (Mob_SKID == 0x400) {
			AttackDamage = 0;
			PASS = 4;
			MobActor->SendMsg(NULL, AM_CHAT, (int)"Hehe, that tickles...", 0, 0);
			g_MainNextActTime = timeGetTime() + 5000;
			g_MobNextOutTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x401) {
			AttackDamage = 0;
			PASS = 4;
			gameMode->GetServerCoor(MobActor->m_pos.x, MobActor->m_pos.z, ms.sx, ms.sy);
			ms.dx = 0x28;
			ms.dy = 1;
			ms.cellX = 0;
			ms.cellY = 0;
			MobActor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);
			g_MainNextActTime = timeGetTime() + 5000;
			g_MobNextOutTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x402) {
			EndFight();
			g_MainNextActTime = timeGetTime();
			gameMode->GetWorld()->RemoveGameActor(100);
			return;
		} else if (Mob_SKID == 0x34) {
			AttackDamage = -100;
			ss.skillId = 0x34;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime();
			ss.targetGID = 1;
			ss.level = 1;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			if (!(AcoActor->GetHealthState() & 1)) {
				AcoActor->SetAttrState(AcoActor->GetBodyState(),
					AcoActor->GetEffectState(), AcoActor->GetHealthState() + 1, true);
			}
			PASS = 1;
		} else if (Mob_SKID == 0x0d) {
			AttackDamage = BasicDamage * 4;
			ss.skillId = 0x0d;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 5;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 1800;
		} else if (Mob_SKID == 0x1d7) {
			AttackDamage = 0;
			PlayWave("effect\\T_\xb0\xf8\xb0\xdd\xb7\xc2.wav", 0, 0, 0, 250, 40, 1.0f);
			MobActor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1ab, 0, 0);
			MobActor->LaunchEffect(0x1ab);
			g_modeMgr.GetGameMode()->SendMsg(MM_MAKE_SKILL_USED_MSG, 0x1d7, (int)MobActor, 1);
			PASS = 4;
			g_MainNextActTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x293) {
			AttackDamage = BasicDamage - AcoDefense;
			ss.skillId = 0x293;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 1;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			bat_STUN_turn = 0;
			if (AcoActor->GetBodyState() != 3) {
				AcoActor->SetAttrState(3,
					AcoActor->GetEffectState(), AcoActor->GetHealthState(), true);
			}
			PASS = 1;
		} else if (Mob_SKID == 0x12) {
			AttackDamage = BasicDamage * 3;
			ss.skillId = 0x12;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 3;
			ss.count = 3;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PlayWave("effect\\EF_FireWall.wav", 0, 0, 0, 250, 40, 1.0f);
			gameMode->GetWorld()->MakeSkill(0x7f, 0x10, -rand() % 2);
			gameMode->GetWorld()->MakeSkill(0x7f, 0x11, -rand() % 2);
			gameMode->GetWorld()->MakeSkill(0x7f, 0x12, -rand() % 2);
			g_session.MoveItemFromEquipWndToItemWnd(2);
			AcoActor->SendMsg(NULL, AM_CHAT, (int)"So hot, I dropped my weapon. I need to re-equip it... ", 0, 0);
			AcoActor->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x0e) {
			AttackDamage = BasicDamage * 3;
			ss.skillId = 0x0e;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 5;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
			if (rand() % 10 == 0) {
				if (AcoActor->GetBodyState() != 2) {
					bat_STUN_turn = 0;
					AcoActor->SetAttrState(2,
						AcoActor->GetEffectState(), AcoActor->GetHealthState(), true);
				}
			}
		} else if (Mob_SKID == 0x0f) {
			AttackDamage = BasicDamage;
			ss.skillId = 0x0f;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			if (AcoActor->GetBodyState() != 2) {
				bat_STUN_turn = 0;
				AcoActor->SetAttrState(2,
					AcoActor->GetEffectState(), AcoActor->GetHealthState(), true);
			}
		} else if (Mob_SKID == 0x405) {
			AttackDamage = 0;
			PASS = 4;
			CRagEffect* effect = MobActor->LaunchEffect(0x83);
			if (effect)
				effect->SendMsg(MobActor, AM_SET_MASTER_POINTER, 0, 0, 0);
			MobActor->SendMsg(NULL, AM_CHAT, (int)"Counter ready", 0, 0);
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x17b) {
			AttackDamage = BasicDamage * 9 - AcoDefense;
			ss.skillId = 0x17b;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 850;
			ss.targetGID = 1;
			ss.level = 1;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			MobActor->SetState(ST_ATTACK);
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x407) {
			AttackDamage = 0;
			PASS = 4;
			gameMode->GetWorld()->MakeSkill(0x81, 0x11, 0);
			MobActor->SendMsg(NULL, AM_CHAT, (int)"Off you go, somewhere Valkyrie Randgris isn't~", 0, 0);
			MobActor->SendMsg(NULL, AM_EMOTION, 2, 0, 0);
			g_MainNextActTime = timeGetTime() + 5000;
			g_MobNextOutTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x408) {
			AttackDamage = 0;
			PASS = 4;
			gameMode->GetWorld()->MakeSkill(0x80, 0x11, 0);
			gameMode->GetWorld()->RemoveSkillbyJT(0x81);
			gameMode->PlaceEffect(&AcoActor->m_pos, 0x130, 0x12c, 0);
			gameMode->GetClientCoor(-3, 0, AcoActor->m_pos.x, AcoActor->m_pos.z);
			if (HelpActor) {
				gameMode->PlaceEffect(&HelpActor->m_pos, 0x130, 0x12c, 0);
				gameMode->GetClientCoor(-3, 0, HelpActor->m_pos.x, HelpActor->m_pos.z);
			}
			g_MainNextActTime = timeGetTime() + 5000;
			g_MobNextOutTime = timeGetTime() + 1000;
		} else if (Mob_SKID == 0x409) {
			AttackDamage = 0;
			PASS = 4;
			gameMode->GetWorld()->RemoveSkillbyJT(0x80);
			g_AcoPosition += 1000;
			g_PoringTarY += 48;
			g_FieldConv_Alpha = 1;
			g_MainNextActTime = timeGetTime() + 5000;
			g_MobNextOutTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0x40a) {
			AttackDamage = 0;
			PASS = 4;
			gameMode->GetClientCoor(20, 0, AcoActor->m_pos.x, AcoActor->m_pos.z);
			AcoActor->LaunchEffect(0x158);
			if (HelpActor) {
				gameMode->GetClientCoor(0x10, 3, HelpActor->m_pos.x, HelpActor->m_pos.z);
				HelpActor->LaunchEffect(0x158);
			}
			g_MainNextActTime = timeGetTime() + 5000;
			g_MobNextOutTime = timeGetTime() + 2000;
		} else if (Mob_SKID == 0xfb) {
			AttackDamage = BasicDamage * 2 - AcoDefense;
			ss.skillId = 0xfb;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 5;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			if (!(AcoActor->GetHealthState() & 1)) {
				AcoActor->SetAttrState(AcoActor->GetBodyState(),
					AcoActor->GetEffectState(), AcoActor->GetHealthState() + 1, true);
			}
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0xfd) {
			AttackDamage = BasicDamage * 4 - AcoDefense;
			ss.skillId = 0xfd;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 2;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
		} else if (Mob_SKID == 0x40c) {
			AttackDamage = 0;
			MobActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, 0x3a, 0xfa, 0);
			PASS = 4;
			g_MainNextActTime = timeGetTime() + 1500;
			MobActor->SendMsg(NULL, AM_MAKE_NUMBER, MobActor->m_MaxHp / 10, 0xffff0000, 0x72);
			MobActor->m_Hp += MobActor->m_MaxHp / -10;
			if (MobActor->m_Hp < 1)
				MobActor->m_Hp = 1;
			MobActor->SendMsg(NULL, AM_UPDATEGAGE_HP, MobActor->m_Hp, MobActor->m_MaxHp, 0);
		} else if (Mob_SKID == 0xc7) {
			AttackDamage = BasicDamage * 2;
			ss.skillId = 0xc7;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_session.SetDamageSituation(timeGetTime() + 500, 100, -AttackDamage, false);
		} else if (Mob_SKID == 0x6e) {
			AttackDamage = BasicDamage * 2;
			ss.skillId = 0x6e;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_session.SetDamageSituation(timeGetTime() + 550, 0, -10, true);
			gameMode->PlaceEffect(&AcoActor->m_pos, 0x66, 0x12c, 0);
			MobActor->SetState(ST_ATTACK);
		} else if (Mob_SKID == 0x59) {
			AttackDamage = BasicDamage * 4;
			ss.skillId = 0x59;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 5;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
			if (rand() % 5 == 0 && AcoActor->GetBodyState() != 2) {
				bat_STUN_turn = 0;
				AcoActor->SetAttrState(2,
					AcoActor->GetEffectState(), AcoActor->GetHealthState(), true);
			}
		} else if (Mob_SKID == 0x08) {
			ss.skillId = 0x08;
			AttackDamage = -100;
			PASS = 1;
			MobActor->LaunchEffect(0x0b);
			MobActor->m_isSprArgbFixed = 1;
			MobActor->SetArgb(-1, 0xff, 0xff, 0x69);
			bat_mobskill_turn = 0;
		} else if (Mob_SKID == 0x115) {
			ss.skillId = 0x115;
			AttackDamage = -100;
			PASS = 1;
			AcoActor->LaunchEffect(0xea);
			short bLv = 54;
			if (MobActor->GetJob() == 0x77c)
				bLv = 83;
			if (AcoActor->m_clevel <= bLv) {
				AcoActor->m_Sp = 0;
				g_session.SetStatusData(AcoActor);
			} else {
				g_session.SetDamageSituation(timeGetTime() + 100, 0, 0x150, true);
			}
		} else if (Mob_SKID == 0x53) {
			vector3d tempPos = AcoActor->m_pos;
			AttackDamage = BasicDamage * 8;
			ss.skillId = 0x53;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 10;
			ss.count = 10;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
			tempPos.x -= (float)(rand() % 11 + 5);
			tempPos.z -= (float)(rand() % 11 + 5);
			gameMode->PlaceEffect(&tempPos, 0x5c, 0x12c, 0);
			tempPos.x += (float)(rand() % 21 + 10);
			tempPos.z += (float)(rand() % 21 + 10);
			gameMode->PlaceEffect(&tempPos, 0x5c, 0x12c, 0);
		} else if (Mob_SKID == 0x5b) {
			AttackDamage = BasicDamage * 6;
			ss.skillId = 0x5b;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 4;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 1500;
			gameMode->PlaceEffect(&AcoActor->m_pos, 0x8e, 0x12c, 0);
		} else if (Mob_SKID == 0x16f) {
			AttackDamage = rand() % 100 + 5000;
			ss.skillId = 0x16f;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 3;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x170) {
			AttackDamage = BasicDamage * 5 - AcoDefense;
			ss.skillId = 0x170;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 400;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 2;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			MobActor->SetState(ST_ATTACK);
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x54) {
			AttackDamage = BasicDamage * 6;
			ss.skillId = 0x54;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 10;
			ss.count = 5;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
			bat_STUN_turn = 0;
			if (rand() % 5 == 0) {
				if (AcoActor->GetBodyState() != 3) {
					AcoActor->SetAttrState(3,
						AcoActor->GetEffectState(), AcoActor->GetHealthState(), true);
				}
			}
		} else if (Mob_SKID == 0x173) {
			AttackDamage = BasicDamage * 7 - AcoDefense;
			ss.skillId = 0x173;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 3;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x1dc) {
			ss.skillId = 0x1dc;
			AttackDamage = -100;
			PASS = 1;
			CRagEffect* effect = AcoActor->LaunchEffect(0x1ef);
			if (effect) {
				vector3d tempPos = AcoActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, (int)&tempPos, 0, 0);
			}
			stateId.erase(std::remove(stateId.begin(), stateId.end(), 9), stateId.end());
			gameMode->GetWorld()->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
			g_session.MoveItemFromEquipWndToItemWnd(2);
			g_session.MoveItemFromEquipWndToItemWnd(1);
			g_session.MoveItemFromEquipWndToItemWnd(0x100);
			g_session.MoveItemFromEquipWndToItemWnd(0x200);
			AcoActor->SendMsg(NULL, AM_CHAT, (int)"Ah, my weapon T_T Accessories T_T Angelus T_T", 0, 0);
			AcoActor->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
		} else if (Mob_SKID == 0x172) {
			AttackDamage = BasicDamage * 6 - AcoDefense;
			ss.skillId = 0x172;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 600;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 3;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 1500;
		} else if (Mob_SKID == 0x176) {
			AttackDamage = BasicDamage * 3;
			ss.skillId = 0x176;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 1;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_session.SetDamageSituation(timeGetTime() + 500, 100, -AttackDamage, false);
			short spLoss = rand() % 10 + 20;
			AcoActor->m_Sp -= spLoss;
			AcoActor->SendMsg(NULL, AM_MAKE_NUMBER, spLoss, 0xffff00ff, 0x72);
			g_session.SetStatusData(AcoActor);
			{
				vector3d pos;
				pos.y = (float)AcoActor->GetGID();
				CRagEffect* effect = MobActor->LaunchEffect(0x180);
				if (effect)
					effect->SendMsg(NULL, AM_SETPOS, (int)&pos, 0, 0);
			}
			MobActor->LaunchEffect(0x181);
			AcoActor->LaunchEffect(0x181);
			MobActor->LaunchEffect(0x197);
			AcoActor->LaunchEffect(0x197);
		} else if (Mob_SKID == 0x55) {
			if (AcoActor->m_clevel < 0x69)
				AttackDamage = BasicDamage * 12;
			else
				AttackDamage = BasicDamage * 8;
			ss.skillId = 0x55;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 10;
			ss.count = 10;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
			gameMode->PlaceEffect(&AcoActor->m_pos, 0x5a, 0x12c, 0);
		} else if (Mob_SKID == 0x174) {
			AttackDamage = BasicDamage * 10 - AcoDefense;
			ss.skillId = 0x174;
			ss.action = 8;
			ss.damage = AttackDamage;
			ss.startTime = timeGetTime() + 300;
			ss.targetGID = 1;
			ss.level = 5;
			ss.count = 5;
			ss.tCellX = -1;
			ss.tCellY = -1;
			ss.attackMT = 0x118;
			ss.attackedMT = 1;
			PASS = 1;
			g_MainNextActTime = timeGetTime() + 2000;
			short spLoss = rand() % 10 + 30;
			AcoActor->m_Sp -= spLoss;
			AcoActor->SendMsg(NULL, AM_MAKE_NUMBER, spLoss, 0xffff00ff, 0x72);
			g_session.SetStatusData(AcoActor);
		}

		if (ss.skillId > 0)
			Mob_SKID = ss.skillId;
		g_Mob_used_Skill = Mob_SKID;

		if (HelpActor) {
			if (HelpActor->GetGID() == 3) {
				short bonus = 0;
				if (map_Number == 15)
					bonus = 5;
				if (rand() % 20 <= bonus) {
					if (AttackDamage > 1 && PASS <= 1) {
						Attacked_ID = 3;
						ss.targetGID = 3;
					}
				}
			}
		}

		if (AttackDamage > 0) {
			if (std::find(stateId.begin(), stateId.end(), 0x13) != stateId.end()) {
				ss.damage = 0;
				AttackDamage = -100;
				stateId.erase(std::remove(stateId.begin(), stateId.end(), 0x13), stateId.end());
				gameMode->GetWorld()->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
				g_session.SetDamageSituation(timeGetTime() + 300, 0, 0x150, true);
			}
		}

		if (PASS == 0) {
			MobActor->SetState(ST_ATTACK);
			if (AttackDamage <= 0 && AttackDamage >= -50)
				AttackDamage = 1;
			if (AttackDamage > 0) {
				if (MobActor->GetJob() == 0x5e8 || MobActor->GetJob() == 0x55d) {
					g_session.SetDamageSituation(timeGetTime() + 850, Attacked_ID,
						AttackDamage, true);
					g_MainNextActTime = timeGetTime() + 1200;
				} else if (MobActor->GetJob() == 0x5e1
					|| MobActor->GetJob() == 0x55a
					|| MobActor->GetJob() == 0x561
					|| MobActor->GetJob() == 0x62e
					|| MobActor->GetJob() == 0x520
					|| MobActor->GetJob() == 0x6f1
					|| MobActor->GetJob() == 0x509
					|| MobActor->GetJob() == 0x6ac) {
					g_session.SetDamageSituation(timeGetTime() + 600, Attacked_ID,
						AttackDamage, true);
				} else {
					g_session.SetDamageSituation(timeGetTime() + 300, Attacked_ID,
						AttackDamage, true);
				}
			}
		} else if (PASS == 1) {
			MobActor->SendMsg(NULL, AM_SKILL, (int)&ss, 0, 0);
			g_modeMgr.GetGameMode()->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)MobActor, ss.level);
			if (AttackDamage <= 0 && AttackDamage >= -50)
				AttackDamage = 1;
			if (AttackDamage > 0) {
				if (Attacked_ID == 3) {
					g_session.SetDamageSituation(timeGetTime(), 3, AttackDamage, false);
				} else {
					g_session.SetDamageSituation(ss.startTime, Attacked_ID, AttackDamage, false);
				}
			}
		} else if (PASS == 2) {
			g_session.SetDamageSituation(timeGetTime() + 250, ss.targetGID, -HealAmount, false);
			g_modeMgr.GetGameMode()->SendMsg(MM_MAKE_SKILL_USED_MSG, ss.skillId, (int)MobActor, ss.level);
		} else if (PASS == 3) {
			MobActor->SetState(ST_DAMAGE);
		}
	}

	if (MobActor->m_stateId == ST_DEAD) {
		EndFight();
	}
}

void UIWindowMgr::Aco_Revive(int HP)
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	CGameActor* actor = g_session.GetGameActorByAid(1);
	if (actor) {
		if (actor->m_stateId == 3) {
			actor->SendMsg(NULL, AM_RESURRECTION, 0, 0, 0);
			if (HP == 0x270f) {
				actor->m_Hp = actor->m_MaxHp;
			} else {
				actor->m_Hp = (short)HP;
				if (HP == 1) {
					CGameActor* helper = g_session.GetGameActorByAid(0x64);
					if (helper) {
						MOVESTRUCT ms;
						gameMode->GetServerCoor(helper->m_pos.x, helper->m_pos.z, ms.sx, ms.sy);
						ms.dx = 0x28;
						ms.dy = 0;
						ms.cellX = 0;
						ms.cellY = 0;
						helper->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);
					}
				}
			}
			g_session.SetStatusData(actor);
		}
	}
}

void UIWindowMgr::All_CheckPos()
{
	g_modeMgr.GetGameMode();

	CGameActor* aco = g_session.GetGameActorByAid(0x64);
	CGameActor* mob = g_session.GetGameActorByAid(1);
	CGameActor* helper = g_session.GetGameActorByAid(map_BNPC_ID);

	if (!aco || !mob)
		return;

	if (aco->m_stateId || mob->m_stateId)
		return;

	if (helper) {
		if (helper->m_stateId)
			return;

		helper->m_roty = 450.0f;
		helper->SetState(ST_ATTACKWAIT);
	}

	g_curStance = 2;
	aco->SendMsg(NULL, AM_SET_TARGET_AID, mob->GetGID(), 0, 0);
	mob->SendMsg(NULL, AM_SET_TARGET_AID, aco->GetGID(), 0, 0);

	aco->m_roty = 315.0f;
	mob->m_roty = 450.0f;

	if (aco->m_Agi < mob->m_Agi) {
		g_MainNextActTime = timeGetTime() + 1000;
		g_MobNextOutTime = timeGetTime() + 4000;
	} else {
		g_MainNextActTime = timeGetTime() + 4000;
		g_MobNextOutTime = timeGetTime() + 1000;
	}

	if (helper) {
		helper->SendMsg(NULL, AM_SET_TARGET_AID, aco->GetGID(), 0, 0);

		if (map_BNPC_ID == 3) {
			g_HelpNextActTime = timeGetTime() + ((rand() % 19 + 1) * 1000);
		} else if (map_BNPC_ID == 5) {
			g_HelpNextActTime = timeGetTime() + 1500;
		} else if (map_BNPC_ID == 6) {
			if (aco->GetJob() == JT_WOODEN_GOLEM) {
				g_HelpNextActTime = timeGetTime() + 11500;
			} else {
				g_HelpNextActTime = timeGetTime() + ((rand() % 4 + 2) * 1000);
			}
		} else {
			g_HelpNextActTime = timeGetTime() + 999999;
		}
	}

	g_used_Skill = 0;
	g_Mob_used_Skill = 0;
}

void UIWindowMgr::Aco_ActinField()
{
	char msg[256];
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	CGameActor* actor = g_session.GetGameActorByAid(1);

	if (g_BattleEvent > 0)
		return;

	if (g_MainNextActTime >= timeGetTime())
		return;

	int action = rand() % 10;
	int delay = rand() % 2000;
	g_MainNextActTime = timeGetTime() + delay + 1000;

	if (action <= 1) {
		if (actor) {
			actor->SetState(6);
			if (g_session.HaveThisSkill(0x104) >= 1) {
				short sp = g_session.HaveThisSkill(0x104) * (actor->m_MaxSp / 50);
				actor->SendMsg(NULL, AM_MAKE_NUMBER, sp, 0xff0000ff, 0x72);
				actor->m_Sp += sp;
				actor->LaunchEffect(0x14c);
				g_session.SetStatusData(actor);
			}
		}
		g_MainNextActTime += 2000;
		return;
	}

	if (action == 2) {
		if (actor) {
			actor->SetState(11);
		}
		g_MainNextActTime += 1000;
		return;
	}

	if (action < 3)
		return;

	if (!actor)
		return;

	{
		MOVESTRUCT ms;
		gameMode->GetServerCoor(actor->m_pos.x, actor->m_pos.z, ms.sx, ms.sy);
		ms.dx = rand() % 12 + 14;
		ms.dy = rand() % 13 - 6;
		ms.cellX = 0;
		ms.cellY = 0;
		actor->SendMsg(NULL, AM_MOVE, (int)&ms);
	}

	g_AcoPosition += 2;
	if (g_AcoPosition < 2000) {
		if (g_AcoPosition == (map_secondstart + 100) * 10) {
			g_curStance = 5;
		}
	} else if (g_AcoPosition < 3000) {
		if (g_AcoPosition == (map_thirdstart + 200) * 10) {
			g_curStance = 5;
		}
	}

	if (g_curStance == 5) {
		g_MainNextActTime = timeGetTime() + 1000;
		g_MobNextOutTime = 0;

		sprintf(msg, "Roll the mouse wheel up to keep going the same way.");
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffffff, 0, 0);
		sprintf(msg, "Roll the mouse wheel down to take the side path.");
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffffff, 0, 0);

		MOVESTRUCT ms;
		gameMode->GetServerCoor(actor->m_pos.x, actor->m_pos.z, ms.sx, ms.sy);
		ms.dx = 17;
		ms.dy = 0;
		actor->SendMsg(NULL, AM_MOVE, (int)&ms);
	}

	int pos = g_AcoPosition;
	if (map_BOSS_done == true) {
		if (pos < 2000) {
			if (map_firstway * 10 <= pos - 1000) {
				g_curStance = 3;
			}
		} else if (pos < 3000) {
			if ((map_secondway + map_secondstart) * 10 <= pos - 2000) {
				g_curStance = 3;
			}
		} else if (pos < 4000) {
			if ((map_thirdway + map_thirdstart) * 10 <= pos - 3000) {
				g_curStance = 3;
			}
		}
	}

	if (g_curStance == 3) {
		g_MainNextActTime = timeGetTime() + 4000;
		BS_StartFasterTetris();
	}

	pos = g_AcoPosition;
	g_PoringTarY = 42;
	if (pos >= 2000) {
		unsigned int row = (unsigned int)(pos - 1000) / 1000;
		pos -= row * 1000;
		g_PoringTarY = row * 48 + 42;
	}
	g_PoringTarX = ((pos * 3 - 3000) * 8) / 10 + 580;
}

void UIWindowMgr::Boy_ActinField()
{
	MOVESTRUCT ms;
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	CGameActor* boy = g_session.GetGameActorByAid(map_BNPC_ID);
	CGameActor* actor = g_session.GetGameActorByAid(1);

	if (!map_BNPC_done) {
		if (map_BNPC_Pos <= g_AcoPosition) {
			map_BNPC_done = true;
			gameMode->Zc_Notify_Entry_HP(map_BNPC_ID, 17, -5);
			if (Boy_talked == 1) {
				Boy_talked = 2;
			}
			g_HelpNextActTime = timeGetTime() + 2000;
			return;
		}
	} else if (boy && g_HelpNextActTime < timeGetTime()) {
		unsigned char event = 0;
		if (Boy_talked == 3) {
			if ((int)g_AcoPosition >= map_firstway * 10 + 986
				&& g_AcoPosition < 2000
				&& !map_BNPC_event
				&& actor->m_clevel >= 13
				&& g_curStory == 12) {
				event = 1;
			}
		}

		if ((map_Number == 14
				&& actor->m_clevel >= 70
				&& (int)g_AcoPosition >= map_firstway * 10 + 986
				&& g_AcoPosition < 2000
				&& !map_BNPC_event)
			|| event == 1) {
			map_BNPC_event = true;
			boy->SetState(12);
			CRagEffect* effect = boy->LaunchEffect(0x12b);
			if (effect) {
				vector3d pos;
				memcpy(&pos, &actor->m_pos, sizeof(pos));
				((CRenderObject*)effect)->SendMsg(NULL, AM_SETPOS, (int)&pos, 0, 0);
				((CRenderObject*)effect)->SendMsg(NULL, AM_SETCOUNT, 5, 0, 0);
				g_HelpNextActTime = timeGetTime() + 3000;
				g_session.SetDamageSituation(timeGetTime() + 250, actor->GetGID(), -8888, true);
				g_modeMgr.GetGameMode()->SendMsg(MM_MAKE_SKILL_USED_MSG, 0xe7, (int)boy, 5);
				return;
			}
			} else {
				gameMode->GetServerCoor(boy->m_pos.x, boy->m_pos.z, ms.sx, ms.sy);
				ms.dx = rand() % 12 + 14;
				ms.dy = rand() % 13 - 6;
				ms.cellX = 0;
				ms.cellY = 0;
				boy->SendMsg(NULL, AM_MOVE, (int)&ms);
				g_HelpNextActTime = timeGetTime() + 3000;
			}
	}
}


void UIWindowMgr::Aco_DecideinField()
{
	g_modeMgr.GetGameMode();
	if (g_MainNextActTime < timeGetTime()) {
		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor->m_stateId == 0) {
			actor->m_roty = 360.0f;
			g_MainNextActTime = timeGetTime() + 0x1b58;
			if (g_MobNextOutTime == 0) {
				actor->SendMsg(NULL, AM_CHAT,
					(int)"Dad~ Keep going, or take the side path?",
					0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 1, 0, 0);
			} else if (g_MobNextOutTime == 1) {
				actor->SendMsg(NULL, AM_CHAT,
					(int)"Roll the wheel up and I'll keep going~",
					0, 0);
			} else if (g_MobNextOutTime == 2) {
				actor->SendMsg(NULL, AM_CHAT,
					(int)"Roll the wheel down and I'll take the side path~",
					0, 0);
			}
			++g_MobNextOutTime;
			if (g_MobNextOutTime > 2) {
				g_MobNextOutTime = 0;
			}
		}
	}
}

void UIWindowMgr::Aco_MoveByDec()
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	if (g_MainNextActTime < timeGetTime()) {
		CGameActor* actor = g_session.GetGameActorByAid(1);
		MOVESTRUCT ms;
		gameMode->GetServerCoor(actor->m_pos.x, actor->m_pos.z, ms.sx, ms.sy);
		if (g_curStance == 6) {
			ms.dx = 0x18;
			ms.dy = 0;
		} else {
			ms.dx = 0x11;
			ms.dy = -6;
		}
		actor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);

		EndFight();
	}
}

void UIWindowMgr::Mob_ActinField()
{
	MOVESTRUCT ms;
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	g_session.GetGameActorByAid(100);
	CGameActor* actor = g_session.GetGameActorByAid(1);
	CGameActor* HelpActor = g_session.GetGameActorByAid(map_BNPC_ID);
	CGameActor* theActor;

	SKILL_INFO sInfo;

	if (g_curStance != 0)
		return;

	if (g_BattleEvent >= 1)
	{
		if (g_MobNextOutTime <= timeGetTime())
		{
			if (g_BattleEvent == 1)
			{
				gameMode->GetServerCoor(actor->m_pos.x, actor->m_pos.z, ms.sx, ms.sy);
				ms.dx = 17; ms.dy = 0;
				ms.cellX = 0; ms.cellY = 0;
				actor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);

				gameMode->GetWorld()->MakeSkill(0x81, 0x17, 0);

				g_BattleEvent = 2;
				g_MobNextOutTime = timeGetTime() + 1000;
			}
			else if (g_BattleEvent == 2)
			{
				gameMode->GetWorld()->MakeSkill(0x80, 0x17, 0);
				g_BattleEvent = 3;
				g_MobNextOutTime = timeGetTime() + 3000;

				actor->m_roty = 450.0f;
			}
			else if (g_BattleEvent == 3)
			{
				gameMode->GetServerCoor(actor->m_pos.x, actor->m_pos.z, ms.sx, ms.sy);
				ms.dx = 23; ms.dy = 0;
				ms.cellX = 0; ms.cellY = 0;
				actor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);

				g_BattleEvent = 4;
				g_MobNextOutTime = timeGetTime();
			}

			else if (g_BattleEvent == 4)
			{
				gameMode->GetServerCoor(actor->m_pos.x, actor->m_pos.z, ms.sx, ms.sy);
				if (ms.sx == 23 && actor->m_stateId == 0)
				{
					gameMode->GetWorld()->RemoveSkillbyJT(0x81);
					gameMode->PlaceEffect(&actor->m_pos, 0x130, 0x12c, 0);
					gameMode->GetClientCoor(-3, 0, actor->m_pos.x, actor->m_pos.z);

					g_BattleEvent = 5;
					g_MobNextOutTime = timeGetTime() + 1000;
				}
			}
			else if (g_BattleEvent == 5)
			{
				gameMode->GetWorld()->RemoveSkillbyJT(0x80);

				g_curStance = 3;
				g_MainNextActTime = timeGetTime() + 4000;
				g_BattleEvent = 0;
				g_MobNextOutTime = timeGetTime() + 3000;
			}
		}
	}
	else if (!g_MobNextTimeFixed)
	{
		g_MobNextOutTime = timeGetTime() + random(7000) + 7000;

		g_MobNextTimeFixed = true;

		g_BattleEvent = 0;
	}
	else if (g_MobNextOutTime <= timeGetTime())
	{
		g_MobNextTimeFixed = false;

		if (g_NextSkill == 0x1b)
		{
			sInfo = g_session.GetSkillItemInfoBySkillId(0x1b);
			if (actor->m_Sp - sInfo.spcost >= 0)
			{
				actor->m_Sp -= sInfo.spcost;
				g_BattleEvent = 1;
				g_NextSkill = 0;
				g_session.SetStatusData(actor);
				return;
			}
		}

		short mob = Mob_WhichOne();

		g_PrevMob = mob;
		g_modeMgr.GetGameMode()->Zc_Notify_Entry_Mob(mob, 100, 100, 100);

		theActor = g_session.GetGameActorByAid(100);

		if (actor && theActor)
		{
			gameMode->GetServerCoor(theActor->m_pos.x, theActor->m_pos.z, ms.sx, ms.sy);

			ms.dx = 23;
			ms.dy = random(2) ? -1 : 1;
			if (theActor->m_clevel >= 90)
				ms.dy = 1;

			ms.cellX = 0; ms.cellY = 0;

			if (theActor->GetJob() < 0x52c || theActor->GetJob() > 0x553)
			{
				theActor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);
			}

			gameMode->GetServerCoor(actor->m_pos.x, actor->m_pos.z, ms.sx, ms.sy);
			ms.dx = 17; ms.dy = 0;
			ms.cellX = 0; ms.cellY = 0;
			actor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);

			if (HelpActor)
			{
				gameMode->GetServerCoor(HelpActor->m_pos.x, HelpActor->m_pos.z, ms.sx, ms.sy);
				ms.dx = 15; ms.dy = ms.sy;
				ms.cellX = 0; ms.cellY = 0;
				HelpActor->SendMsg(NULL, AM_MOVE, (int)&ms, 0, 0);
			}

			g_curStance = 1;
		}
	}
}

short UIWindowMgr::Mob_WhichOne()
{
	g_BattleEvent = 0;

	if (map_Number == 10) {
		short randNum = rand() % 3;
		if (!map_BOSS_done) {
			if (g_AcoPosition >= map_firstway * 10 + 996 && g_AcoPosition < 2000) {
				map_BOSS_done = true;
				return 0x6f9;
			}
			if (g_AcoPosition >= (map_secondstart + map_secondway) * 10 + 1996 && g_AcoPosition < 3000) {
				map_BOSS_done = true;
				return 0x75d;
			}
			if (g_AcoPosition >= (map_thirdstart + map_thirdway) * 10 + 2996) {
				map_BOSS_done = true;
				return 0x52c;
			}
		}
		if (g_AcoPosition < 2000) {
			if (randNum == 0)
				return 0x72c;
			if (randNum == 1)
				return 0x48f;
			return 0x48a;
		}
		if (randNum == 0)
			return 0x698;
		if (randNum == 1)
			return 0x501;
		return 0x510;
	}

	if (map_Number == 11) {
		short randNum = rand() % 3;
		if (g_curStory == 10 && g_AcoPosition >= 1010) {
			g_curStory = 11;
			return 0x6c6;
		}
		if (!map_BOSS_done) {
			if (g_AcoPosition >= map_firstway * 10 + 996 && g_AcoPosition < 2000) {
				map_BOSS_done = true;
				return 0x6c5;
			}
			if (g_AcoPosition >= (map_secondstart + map_secondway) * 10 + 1996 && g_AcoPosition < 3000) {
				map_BOSS_done = true;
				return 0x52d;
			}
			if (g_AcoPosition >= (map_thirdstart + map_thirdway) * 10 + 2996) {
				map_BOSS_done = true;
				return 0x55d;
			}
		}
		if (g_AcoPosition < 2000) {
			if (randNum == 0)
				return 0x5ef;
			if (randNum == 1)
				return 0x5d5;
			return 0x402;
		}
		if (randNum == 0)
			return 0x492;
		if (randNum == 1)
			return 0x5e8;
		return 0x5e6;
	}

	if (map_Number == 12) {
		short randNum = rand() % 3;
		if (g_curStory <= 44 && g_AcoPosition >= 1050 && g_AcoPosition < 2000)
			return 0x55e;
		if (!map_BOSS_done) {
			if (g_AcoPosition >= map_firstway * 10 + 996 && g_AcoPosition < 2000) {
				map_BOSS_done = true;
				return 0x6d7;
			}
			if (g_AcoPosition >= (map_secondstart + map_secondway) * 10 + 1996 && g_AcoPosition < 3000) {
				map_BOSS_done = true;
				return 0x52e;
			}
			if (g_AcoPosition >= (map_thirdstart + map_thirdway) * 10 + 2996) {
				map_BOSS_done = true;
				return 0x52f;
			}
		}
		if (randNum == 0)
			return 0x781;
		if (randNum == 1)
			return 0x691;
		return 0x5e1;
	}

	if (map_Number == 13) {
		short randNum = rand() % 3;
		if (g_curStory <= 54 && g_AcoPosition >= 3100 && g_AcoPosition < 4000)
			return 0x5d9;
		if (!map_BOSS_done) {
			if (g_AcoPosition >= map_firstway * 10 + 996 && g_AcoPosition < 2000) {
				map_BOSS_done = true;
				return 0x530;
			}
			if (g_AcoPosition >= (map_secondstart + map_secondway) * 10 + 1996 && g_AcoPosition < 3000) {
				map_BOSS_done = true;
				return 0x531;
			}
			if (g_AcoPosition >= (map_thirdstart + map_thirdway) * 10 + 2996) {
				map_BOSS_done = true;
				return 0x4da;
			}
		}
		if (randNum == 0)
			return 0x55a;
		if (randNum == 1)
			return 0x561;
		return 0x6b2;
	}

	if (map_Number == 14) {
		short randNum = rand() % 10;
		if (!map_BOSS_done) {
			if (g_AcoPosition >= map_firstway * 10 + 996 && g_AcoPosition < 2000) {
				map_BOSS_done = true;
				return 0x503;
			}
			if (g_AcoPosition >= (map_secondstart + map_secondway) * 10 + 1996 && g_AcoPosition < 3000) {
				map_BOSS_done = true;
				return 0x77c;
			}
			if (g_AcoPosition >= (map_thirdstart + map_thirdway) * 10 + 2996) {
				map_BOSS_done = true;
				return 0x509;
			}
		}
		if (g_AcoPosition < 2000) {
			if (randNum <= 2)
				return 0x6c7;
			if (randNum <= 5)
				return 0x6c9;
			if (randNum <= 8)
				return 0x74b;
			return 0x520;
		}
		if (g_AcoPosition < 3000) {
			if (randNum <= 2)
				return 0x4e4;
			if (randNum <= 5)
				return 0x5eb;
			if (randNum <= 6)
				return 0x6c7;
			if (randNum <= 7)
				return 0x6c9;
			if (randNum <= 8)
				return 0x74b;
			return 0x520;
		}
		if (randNum <= 3)
			return 0x6f1;
		if (randNum <= 7)
			return 0x47e;
		if (randNum <= 8)
			return 0x4e4;
		return 0x520;
	}

	if (map_Number == 15) {
		short randNum = rand() % 3;
		if (!map_BOSS_done) {
			if (g_AcoPosition >= map_firstway * 10 + 996 && g_AcoPosition < 2000) {
				map_BOSS_done = true;
				return 0x532;
			}
			if (g_AcoPosition >= (map_secondstart + map_secondway) * 10 + 1996 && g_AcoPosition < 3000) {
				map_BOSS_done = true;
				return 0x533;
			}
			if (g_AcoPosition >= (map_thirdstart + map_thirdway) * 10 + 2996) {
				map_BOSS_done = true;
				return 0x6ac;
			}
		}
		if (randNum == 0)
			return 0x487;
		if (randNum == 1)
			return 0x43f;
		return 0x50f;
	}

	return 0x5e1;
}

void UIWindowMgr::RenderBackGround2D_House()
{
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};
	int alpha = 0xff;

	if (g_curScreen == 10 || g_curScreen == 3) {
		alpha = 0xff - g_ScreenConv_Alpha;
	} else if (g_curScreen == 21) {
		alpha = g_ScreenConv_Alpha;
	}

	sprintf(temp, "effect\\white02.bmp");
	rect.left = 190;
	rect.top = 90;
	rect.right = 850;
	rect.bottom = 590;
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		(alpha << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\HP_Back1.bmp");
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		(((alpha * 136) / 256) << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	rect.left = 200;
	rect.top = 100;
	rect.right = 840;
	rect.bottom = 580;
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		(alpha << 24) + 0xffffff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
}

void UIWindowMgr::RenderBackGround2D_Battle()
{
	CMode* gameMode = g_modeMgr.GetGameMode();
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};
	int alpha = 0xff;
	int color;

	if (g_curScreen == 11) {
		alpha = g_ScreenConv_Alpha;
	} else if (g_curScreen == 20) {
		alpha = 0xff - g_ScreenConv_Alpha;
	}

	sprintf(temp, "effect\\white02.bmp");
	rect.left = 515;
	rect.top = 90;
	rect.right = 1015;
	rect.bottom = 590;
	color = (alpha << 24) + 0xffffff;
	gameMode->DrawBackGround2D(&rect, 0, 0.002f,
		color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	if (g_FieldConv_Alpha > 0) {
		++g_FieldConv_Alpha;
		if (g_FieldConv_Alpha >= 0xff) {
			g_FieldConv_Alpha = 0;
		} else if (g_FieldConv_Alpha > 0) {
			sprintf(temp, "%s", GetmapName_HP(g_AcoPosition - 1000));
			rect.left = 515;
			rect.top = 90;
			rect.right = 1015;
			rect.bottom = 590;
			color = (((0xff - g_FieldConv_Alpha) * 136) / 256 << 24) + 0xffffff;
			gameMode->DrawBackGround2D(&rect, 0, 0.002f,
				color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

			rect.left = 525;
			rect.top = 100;
			rect.right = 1005;
			rect.bottom = 580;
			color = ((0xff - g_FieldConv_Alpha) << 24) + 0xffffff;
			gameMode->DrawBackGround2D(&rect, 0, 0.002f,
				color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
			color = (alpha << 24) + 0xffffff;
		}
	}

	sprintf(temp, "%s", GetmapName_HP(g_AcoPosition));
	if (g_FieldConv_Alpha > 0) {
		rect.left = 515;
		rect.top = 90;
		rect.right = 1015;
		rect.bottom = 590;
		color = ((g_FieldConv_Alpha * 136) / 256 << 24) + 0xffffff;
		gameMode->DrawBackGround2D(&rect, 0, 0.002f,
			color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

		rect.left = 525;
		rect.top = 100;
		rect.right = 1005;
		rect.bottom = 580;
		color = (g_FieldConv_Alpha << 24) + 0xffffff;
		gameMode->DrawBackGround2D(&rect, 0, 0.002f,
			color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
		color = (alpha << 24) + 0xffffff;
	} else {
		color = ((alpha * 136) / 256 << 24) + 0xffffff;
		gameMode->DrawBackGround2D(&rect, 0, 0.002f,
			color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

		rect.left = 525;
		rect.top = 100;
		rect.right = 1005;
		rect.bottom = 580;
		color = (alpha << 24) + 0xffffff;
		gameMode->DrawBackGround2D(&rect, 0, 0.002f,
			color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
	}

	sprintf(temp, "effect\\white02.bmp");
	rect.left = 10;
	rect.top = 90;
	rect.right = 510;
	rect.bottom = 590;
	gameMode->DrawBackGround2D(&rect, 0, 0.002f,
		color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xba\xed\xbd\xba\xb0\xa1\xb0\xd4""480.jpg");
	gameMode->DrawBackGround2D(&rect, 0, 0.002f,
		((alpha * 136) / 256 << 24) + 0xffffff,
		g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	rect.left = 20;
	rect.top = 100;
	rect.right = 500;
	rect.bottom = 580;
	gameMode->DrawBackGround2D(&rect, 0, 0.002f,
		color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	RenderAcoPosition();
}

void UIWindowMgr::RenderNextSkill()
{
	CMode* gameMode = g_modeMgr.GetGameMode();
	char msg[256];
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};

	if (g_NextSkill > 0) {
		memset(msg, 0, sizeof(msg));
		sprintf(msg, "NEXT : %s", SKILL_INFO::GetSkillNameBySkillId(g_NextSkill));

		short len = (short)strlen(msg);
		short x = (short)((20 - len) * 5);
		sprintf(temp, "effect\\white02.bmp");

		rect.left = x + 630;
		rect.right = rect.left + (len * 5 + 40) * 2;
		rect.top = 540;
		rect.bottom = 620;
		gameMode->DrawBackGround2D(&rect, 0, 0.01f,
			0x22ffffff, g_texMgr.GetTexture(temp, false), UVRECT(0.0f, 0.0f, 1.0f, 1.0f));

		rect.left += 10;
		rect.top += 10;
		rect.right = rect.left + (len * 5 + 30) * 2;
		rect.bottom = rect.top + 60;
		gameMode->DrawBackGround2D(&rect, 0, 0.01f,
			0x770000ff, g_texMgr.GetTexture(temp, false), UVRECT(0.0f, 0.0f, 1.0f, 1.0f));

		if (g_renderer->DrawScene()) {
			g_renderer->TextOutScreen(x + 670, 570, msg, 0xffffff, 20, 1);
		}
	}
}

void UIWindowMgr::RenderAcoPosition()
{
	CMode* gameMode = g_modeMgr.GetGameMode();
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};
	int alpha = 0xff;
	int i;

	if (g_curScreen == 11) {
		alpha = g_ScreenConv_Alpha;
	}

	sprintf(temp, "effect\\white02.bmp");
	rect.left = 554;
	rect.top = 17;
	rect.right = 974;
	rect.bottom = 173;
	gameMode->DrawBackGround2D(&rect, 0, 0.002f,
		((alpha / 3) << 24) + 0x3737ff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	sprintf(temp, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\\xbf\xa5\xba\xea\xb8\xae\xbf\xc0.bmp");
	int color = ((alpha * 125) << 17) + 0xffffff;

	for (i = 0; i <= map_firstway; i++) {
		rect.right = 578 + i * 24 + 12;
		rect.left = 578 + i * 24;
		rect.top = 41;
		rect.bottom = 53;
		gameMode->DrawBackGround2D(&rect, 0, 0.002f,
			color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
	}

	if (map_secondstart > 0) {
		rect.left = 578 + map_secondstart * 24;
		rect.right = rect.left + 12;
		rect.top = 65;
		rect.bottom = 77;
		gameMode->DrawBackGround2D(&rect, 0, 0.002f,
			color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

		for (i = map_secondstart; i <= map_secondstart + map_secondway; i++) {
			rect.left = 578 + i * 24;
			rect.right = rect.left + 12;
			rect.top = 89;
			rect.bottom = 101;
			gameMode->DrawBackGround2D(&rect, 0, 0.002f,
				color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
		}

		if (map_thirdstart > 0) {
			rect.left = 578 + map_thirdstart * 24;
			rect.right = rect.left + 12;
			rect.top = 113;
			rect.bottom = 125;
			gameMode->DrawBackGround2D(&rect, 0, 0.002f,
				color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

			for (i = map_thirdstart; i <= map_thirdstart + map_thirdway; i++) {
				rect.left = 578 + i * 24;
				rect.right = rect.left + 12;
				rect.top = 137;
				rect.bottom = 149;
				gameMode->DrawBackGround2D(&rect, 0, 0.002f,
					color, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
			}
		}
	}

	if (g_Frame % 3 == 0) {
		if (g_PoringCurY != g_PoringTarY) {
			if (g_PoringCurY < g_PoringTarY) {
				++g_PoringCurY;
			} else {
				--g_PoringCurY;
			}
		} else if (g_PoringCurX != g_PoringTarX) {
			if (g_PoringCurX < g_PoringTarX) {
				++g_PoringCurX;
			} else {
				--g_PoringCurX;
			}
		}
	}

	CActRes* act = (CActRes*)g_resMgr().Get("\xb8\xf3\xbd\xba\xc5\xcd\\poring.act", false);
	CSprRes* spr = (CSprRes*)g_resMgr().Get("\xb8\xf3\xbd\xba\xc5\xcd\\poring.spr", false);
	gameMode->DrawSprite(g_PoringCurX, g_PoringCurY, act, spr,
		15, (g_Frame / 6) % 10, 0.6f, 0.0f, (alpha << 24) + 0xffffff);
}

void UIWindowMgr::RenderMenu()
{
	CMode* gameMode = g_modeMgr.GetGameMode();
	char temp[256];
	char msg[256];
	RECT rect = {0, 0, 0xff, 0xff};

	if (g_curScreen >= 100) {
		if (g_curScreen == 100) {
			RenderThreeWay(780, 190,
				(char*)"New Game",
				(char*)"Continue",
				(char*)"Credits", 180);
		} else if (g_curScreen == 110 || g_curScreen == 120) {
			RenderThreeWay(420, 190,
				g_savedata[0],
				g_savedata[1],
				g_savedata[2], 550);
		}

		RenderWhiteBlueRect(300, 15, 400, 60, 0x44ee0000, 0x33ee0000);
		if (g_renderer->DrawScene()) {
			sprintf(msg, "I want to be a High Priest");
			g_renderer->TextOutScreen(378, 35, msg, 0xffffff, 20, 1);
		}

		if (g_curScreen == 101 || g_curScreen == 102) {
			RenderEnding();
		}
		return;
	} else {
		if (g_curScreen >= 50 && g_curScreen < 60) {
			RenderEnding();
		} else if (g_curScreen == 11 || g_curScreen == 12 || g_curScreen == 20) {
			RenderNextSkill();
		}
	}

	if (g_curScreen == 1) {
		RenderThreeWay(780, 190,
			(char*)"Battle",
			(char*)"Rest",
			(char*)"Travel", 180);
	} else if (g_curScreen == 5) {
		if (Boy_frame == 1 && g_curStance == 4) {
			if (Boy_talked == 4) {
				RenderThreeWay(40, 260,
					(char*)"Vadon",
					(char*)"Poring",
					(char*)"Ghostring", 180);
			} else if (Boy_talked == 5) {
				RenderThreeWay(40, 260,
					(char*)"Swordman",
					(char*)"Taekwon Kid",
					(char*)"Acolyte", 180);
			} else if (Boy_talked == 6) {
				RenderThreeWay(40, 260,
					(char*)"Son Goku",
					(char*)"Carrot",
					(char*)"Kakarot", 180);
			} else if (Boy_talked == 7) {
				RenderThreeWay(40, 260,
					(char*)"Romantic Leaf",
					(char*)"Romantic Flower",
					(char*)"Blank Eyes", 180);
			} else if (Boy_talked == 8) {
				RenderThreeWay(40, 260,
					(char*)"I'm grateful",
					(char*)"It's natural",
					(char*)"I dislike him", 180);
			} else if (Boy_talked == 9) {
				RenderThreeWay(40, 260,
					(char*)"Baphomet",
					(char*)"Thanatos",
					(char*)"Arc Angeling", 180);
			} else if (Boy_talked == 10) {
				RenderThreeWay(40, 260,
					(char*)"Munak",
					(char*)"Maengryong",
					(char*)"Kakarot", 180);
			} else if (Boy_talked == 11) {
				RenderThreeWay(40, 260,
					(char*)"Love",
					(char*)"Peace",
					(char*)"Gold", 180);
			}
		}
	} else if (g_curScreen == 12) {
		if (g_curStance >= 3 && g_curStance <= 4) {
			sprintf(temp, "effect\\white02.bmp");
			rect.left = 570;
			rect.top = 290;
			rect.right = 950;
			rect.bottom = 370;
			gameMode->DrawBackGround2D(&rect, 0, 0.01f,
				0x22ffffff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

			rect.left = 580;
			rect.top = 300;
			rect.right = 940;
			rect.bottom = 360;
			if (g_curStance == 3) {
				gameMode->DrawBackGround2D(&rect, 0, 0.01f,
					0x660000ff, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
			} else if (g_curStance == 4) {
				gameMode->DrawBackGround2D(&rect, 0, 0.01f,
					0x66ff0000, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
			}
		}

		if (g_curStance_B == 0) {
			RenderThreeWay(40, 190,
				(char*)"Forge",
				(char*)"Sell",
				(char*)"Shop", 180);
		} else if (g_curStance_B == 1) {
			if (g_WordDisplay == 1) {
				RenderWhiteBlueRect(110, 290, 300, 80, 0x660000ff, 0x22ffffff);
			}
		} else if (g_curStance_B == 2) {
			if (g_BS_phase == 3) {
				RenderThreeWay(40, 380,
					(char*)"Sell high",
					(char*)"Sell fair",
					(char*)"Sell cheap", 180);
			}
		} else if (g_curStance_B == 3) {
			if (g_BS_phase == 3) {
				RenderThreeWay(40, 380,
					(char*)"Good weapon",
					(char*)"Accessory",
					(char*)"No thanks", 180);
			} else if (g_BS_phase == 6) {
				RenderThreeWay(40, 380,
					(char*)"Buy",
					(char*)"Haggle",
					(char*)"Don't buy", 180);
			}
		}
	}

	if (g_session.m_DayCount < 10) {
		RenderWhiteBlueRect(40, 15, 140, 60, 0x44000000, 0x33000000);
	} else if (g_session.m_DayCount < 100) {
		RenderWhiteBlueRect(40, 15, 150, 60, 0x44000000, 0x33000000);
	} else {
		RenderWhiteBlueRect(40, 15, 160, 60, 0x44000000, 0x33000000);
	}

	if (g_renderer->DrawScene()) {
		if (g_curScreen == 12) {
			if (g_curStance >= 3 && g_curStance <= 4) {
				sprintf(msg, "Battle Completed !!!");
				g_renderer->TextOutScreen(670, 320, msg, 0xffffff, 20, 1);
			}

			if (g_curStance_B == 1) {
				if (g_WordDisplay == 1) {
					sprintf(msg, "%d%% Completed !!!", TT->Percent);
					g_renderer->TextOutScreen(180, 320, msg, 0xffffff, 20, 1);
					if (TT->Word_displaytime <= timeGetTime()) {
						g_WordDisplay = false;
					}
				}
			}
		}

		if (g_mouse.GetLBtn() == BTN_PRESSED && g_mouse.GetRBtn() == BTN_PRESSED)
		{
			sprintf(msg, "X=%d, Y=%d, STORY = %d, TALK = %d, AFFECTION = %d",
				g_mouse.GetXPos(), g_mouse.GetYPos(), g_curStory, Boy_talked, Boy_friendly);
			g_renderer->TextOutScreen(0, 0, msg, 0xe1e1e1, 20, 1);
		}

		sprintf(msg, "DAY  %d", g_session.m_DayCount);
		g_renderer->TextOutScreen(80, 35, msg, 0xebebeb, 20, 1);
	}
}

void UIWindowMgr::RenderWhiteBlueRect(int X, int Y, int DX, int DY, int C, int CB)
{
	int x = X;
	int y = Y;
	CMode* gameMode = g_modeMgr.GetGameMode();
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};

	sprintf(temp, "effect\\white02.bmp");

	rect.right = x + DX;
	rect.bottom = y + DY;
	rect.left = x;
	rect.top = y;
	gameMode->DrawBackGround2D(&rect, 0, 0.01f, CB, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));

	x += 10;
	y += 10;
	rect.right = x + DX - 20;
	rect.bottom = y + DY - 20;
	rect.left = x;
	rect.top = y;
	gameMode->DrawBackGround2D(&rect, 0, 0.01f, C, g_texMgr.GetTexture(temp, false), UVRECT(0, 0, 1, 1));
}

void UIWindowMgr::RenderThreeWay(int X, int Y, char* msg1, char* msg2, char* msg3, short xwidth)
{
	g_modeMgr.GetGameMode();
	char msg[256];
	char temp[256];
	RECT rect = {0, 0, 0xff, 0xff};

	sprintf(temp, "effect\\white02.bmp");
	short width = xwidth;

	rect.left = X;
	rect.top = Y;
	rect.right = X + width;
	rect.bottom = Y + 180;
	if (width > 500) {
		rect.left = X - 24;
	}
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		0x440000ff, g_texMgr.GetTexture(temp, false), UVRECT(0.0f, 0.0f, 1.0f, 1.0f));

	rect.left = X + 10;
	rect.top = Y + 10;
	rect.right = X + width - 10;
	rect.bottom = Y + 170;
	if (width > 500) {
		rect.left -= 24;
	}
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		0x440000ff, g_texMgr.GetTexture(temp, false), UVRECT(0.0f, 0.0f, 1.0f, 1.0f));

	rect.left = X - 10;
	rect.right = X + width + 10;
	if (width > 500) {
		rect.left -= 24;
	}

	int targetTop;
	if (g_menuSelect == 1) {
		targetTop = Y + 20;
	} else if (g_menuSelect == 2) {
		targetTop = Y + 70;
	} else {
		targetTop = Y + 120;
	}

	if (g_curRectTop > targetTop) {
		g_curRectTop -= 20;
		if (g_curRectTop < targetTop) {
			g_curRectTop = targetTop;
		}
	} else if (g_curRectTop < targetTop) {
		g_curRectTop += 20;
		if (g_curRectTop > targetTop) {
			g_curRectTop = targetTop;
		}
	}

	rect.top = g_curRectTop;
	rect.bottom = g_curRectTop + 40;
	g_modeMgr.GetGameMode()->DrawBackGround2D(&rect, 0, 0.002f,
		0x77ffffff, g_texMgr.GetTexture(temp, false), UVRECT(0.0f, 0.0f, 1.0f, 1.0f));

	if (g_renderer->DrawScene()) {
		sprintf(msg, msg1);
		width /= 2;
		short textX = (short)(width - strlen(msg) * 5);
		if (g_menuSelect == 1) {
			g_renderer->TextOutScreen(X + textX, Y + 30, msg, 0xe13737, 20, 1);
		} else {
			g_renderer->TextOutScreen(X + textX, Y + 30, msg, 0xffcdcd, 20, 1);
		}

		sprintf(msg, msg2);
		textX = (short)(width - strlen(msg) * 5);
		if (g_menuSelect == 2) {
			g_renderer->TextOutScreen(X + textX, Y + 80, msg, 0xe13737, 20, 1);
		} else {
			g_renderer->TextOutScreen(X + textX, Y + 80, msg, 0xffcdcd, 20, 1);
		}

		sprintf(msg, msg3);
		textX = (short)(width - strlen(msg) * 5);
		if (g_menuSelect == 3) {
			g_renderer->TextOutScreen(X + textX, Y + 130, msg, 0xe13737, 20, 1);
		} else {
			g_renderer->TextOutScreen(X + textX, Y + 130, msg, 0xffcdcd, 20, 1);
		}
	}
}

void UIWindowMgr::SetcurScreen(int F1)
{
	char msg[256];
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	g_curScreen = F1;

	if (F1 == 1) {
		g_ScreenConv_Alpha = 0;
		g_curStance = 0;
		g_curStance_B = 0;

		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			gameMode->GetClientCoor(14, -5, actor->m_pos.x, actor->m_pos.z);
			actor->m_roty = 360.0f;
		}

		actor = g_session.GetGameActorByAid(2);
		if (actor) {
			gameMode->GetClientCoor(7, -5, actor->m_pos.x, actor->m_pos.z);
		}

		gameMode->GetWorld()->RemoveGameActor(0x64);
		if (g_curEnding > 0) {
			SetcurScreen(0x32);
		}
		return;
	}

	if (F1 == 0x32 || F1 == 0x65) {
		g_curStance = 0;
		g_curStance_B = 0;
	}

	if (F1 == 10 || F1 == 20 || F1 == 3 || F1 == 6) {
		g_ScreenConv_Alpha = 0;
		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			if (actor->m_Hp <= 1 && F1 == 10) {
				actor->SendMsg(NULL, AM_CHAT, (int)"No~ T_T  I want to rest", 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 0x2f, 0, 0);
				SetcurScreen(1);
				return;
			}
			if (g_curStory <= 0x5c && map_Number == 15) {
				actor->SendMsg(NULL, AM_CHAT, (int)"I must change jobs before fighting Thanatos T_T", 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
				SetcurScreen(1);
				return;
			}

			actor->SetState(0x13);
			if (actor->IsEffectExist(0xe4)) {
				actor->SendMsg(NULL, AM_DELETE_EFFECT, 0xe4, 0, 0);
				bat_ChookGi = 0;
			}

			actor = g_session.GetGameActorByAid(2);
			if (actor) {
				actor->SetState(0x13);
			}
		}

		actor = g_session.GetGameActorByAid(map_BNPC_ID);
		if (actor) {
			gameMode->PlaceEffect(&actor->m_pos, 0x130, 0x12c, 0);
			gameMode->GetWorld()->RemoveGameActor(actor->GetGID());
		}
		g_curStance = 0;
	}

	if (F1 == 4) {
		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			gameMode->GetClientCoor(13, -2, actor->m_pos.x, actor->m_pos.z);
			while (actor->IsEffectExist(0x1bd)) {
				actor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1bd, 0, 0);
			}
			actor->SetState(0x14);
			actor->LaunchEffect(0x1be);
		}

		actor = g_session.GetGameActorByAid(2);
		if (actor) {
			gameMode->GetClientCoor(-3, 0, actor->m_pos.x, actor->m_pos.z);
			while (actor->IsEffectExist(0x1bd)) {
				actor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1bd, 0, 0);
			}
			actor->SetState(0x14);
			actor->LaunchEffect(0x1be);
		}
		return;
	}

	if (F1 == 5) {
		g_modeMgr.GetGameMode()->DeleteBroadcastBalloon();
		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			actor->SetState(0);
		}
		return;
	}

	if (F1 == 6) {
		Boy_frame = 0;
		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			actor->DeleteChatBalloon();
		}
		return;
	}

	if (F1 == 11) {
		gameMode->DeleteBroadcastBalloon();

		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			gameMode->GetClientCoor(20, 0, actor->m_pos.x, actor->m_pos.z);
			while (actor->IsEffectExist(0x1bd)) {
				actor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1bd, 0, 0);
			}
			actor->SetState(0x14);
			actor->LaunchEffect(0x1be);
		}

		actor = g_session.GetGameActorByAid(2);
		if (actor) {
			gameMode->GetClientCoor(6, 0, actor->m_pos.x, actor->m_pos.z);
			while (actor->IsEffectExist(0x1bd)) {
				actor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1bd, 0, 0);
			}
			actor->SetState(0x14);
			actor->LaunchEffect(0x1be);
		}

		map_BOSS_done = false;
		map_Heal_Used = 0;
		map_ComboF_Used = 0;
		map_BNPC_done = false;
		map_BNPC_event = false;
		map_BNPC_Pos = 9999;

		if (map_Number == 11 || map_Number == 10) {
			if (g_curStory >= 12 && Boy_talked >= 1) {
				int pos = rand() % 41;
				map_BNPC_Pos = (short)(pos + 1000);
				map_BNPC_ID = 3;
			}
		} else if (map_Number == 12) {
			if (g_curStory >= 44) {
				int pos = rand() % 41;
				map_BNPC_Pos = (short)(pos + 1000);
				map_BNPC_ID = 5;
			}
		} else if (map_Number == 13) {
			if (g_curStory >= 54) {
				int pos = rand() % 31;
				map_BNPC_Pos = (short)(pos + 1010);
				map_BNPC_ID = 6;
			}
		} else {
			map_BNPC_Pos = (short)(rand() % 11 + 1020);
			if (rand() % 3 == 0) {
				map_BNPC_ID = 3;
			} else map_BNPC_ID = (short)((rand() % 2 != 0) + 5);
			if (g_curStory >= 80) {
				if (g_Aco_Job != 0xfa9) {
					map_BNPC_ID = 3;
					if (map_Number == 15) {
						map_BNPC_Pos = (short)(rand() % 11 + 1000);
					}
				} else {
					map_BNPC_ID = (short)((rand() % 2 != 0) + 5);
				}
			}
		}
		g_AcoPosition = 1000;
		g_PoringCurX = 580;
		g_PoringCurY = 42;
		g_PoringTarX = 580;
		g_PoringTarY = 42;
		return;
	}

	if (F1 == 12) {
		if (g_session.GetLevel() != 1) {
			return;
		}

		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			sprintf(msg, "The Poring above shows where I am on the field~");
			actor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
			sprintf(msg, "Battles happen automatically.");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffffff, 0, 0);
			sprintf(msg, "But skills are only used when the player orders them,");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffffff, 0, 0);
			sprintf(msg, "Leveling up early teaches essential skills like Heal and Aco Kaboom.");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffffff, 0, 0);
			sprintf(msg, "The Poring atop the battle screen shows where your daughter is.");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffffff, 0, 0);
			sprintf(msg, "Bored while your daughter fights? Forge to earn money ^^;");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xffffff, 0, 0);
		}
		return;
	}

	if (F1 == 13) {
		g_MainNextActTime = timeGetTime() + 3000;
		return;
	}

	if (F1 == 21) {
		stateId.erase(std::remove(stateId.begin(), stateId.end(), 9), stateId.end());
		gameMode->GetWorld()->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);

		CGameActor* actor = g_session.GetGameActorByAid(1);
		if (actor) {
			g_modeMgr.GetGameMode()->GetClientCoor(14, -5, actor->m_pos.x, actor->m_pos.z);
			while (actor->IsEffectExist(0x1bd)) {
				actor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1bd, 0, 0);
			}
			actor->SetState(0x14);
			actor->LaunchEffect(0x1be);
			HP_DayPassing();
		}

		actor = g_session.GetGameActorByAid(2);
		if (actor) {
			g_modeMgr.GetGameMode()->GetClientCoor(7, -5, actor->m_pos.x, actor->m_pos.z);
			while (actor->IsEffectExist(0x1bd)) {
				actor->SendMsg(NULL, AM_DELETE_EFFECT, 0x1bd, 0, 0);
			}
			actor->SetState(0x14);
			actor->LaunchEffect(0x1be);
		}
		return;
	}

	if (F1 == 110) {
		MakeSaveFileName(0);
		return;
	}

	if (F1 == 111) {
		MakeWindow(WID_MAKECHARWND);
		return;
	}

	if (F1 == 120) {
		MakeSaveFileName(1);
	}
}

void UIWindowMgr::EndFight()
{
	g_HelpNextActTime = timeGetTime() + 3000;
	g_MainNextActTime = timeGetTime() + 2000;
	g_curStance = 0;

	EndFight_Status();
}

void UIWindowMgr::EndFight_Status()
{
	CGameActor* actor = g_session.GetGameActorByAid(1);
	if (actor) {
		if (actor->GetBodyState() > 0) {
			actor->SetAttrState(0, actor->GetEffectState(), actor->GetHealthState(), 1);
		}
		if (actor->IsEffectExist(424)) {
			actor->SendMsg(NULL, AM_DELETE_EFFECT, 424, 0, 0);
		}
	}

	g_BattleEvent = 0;
}

void UIWindowMgr::ExeMenu()
{
	ITEM_INFO itemInfo;
	char msg[256];
	CGameActor* TheActor;

	if (g_curScreen == 1) {
		if (g_menuSelect == 1) {
			SetcurScreen(2);
			g_curStance = 0;
			g_menuFreeze = g_menuSelect;
		} else if (g_menuSelect == 2) {
			SetcurScreen(0);
			g_curStance = 0;
		} else if (g_menuSelect == 3) {
			SetcurScreen(2);
			g_curStance = 0;
			g_menuFreeze = g_menuSelect;
		}
	} else if (g_curScreen == 5) {
		if (g_curStance == 4 && Boy_talked >= 4 && Boy_talked <= 11) {
			g_menuFreeze = g_menuSelect;
			g_MainNextActTime = timeGetTime();
		}
	} else if (g_curScreen == 12) {
		if (g_curStance_B == 0) {
			if (g_menuSelect == 1) {
				if (g_curHardness >= 1) {
					SetcurStance_B(1);
				} else {
					SetcurStance_B(11);
				}
			} else if (g_menuSelect == 2) {
				TheActor = g_session.GetGameActorByAid(2);
				g_selected_item = 0;
				for (short i = 0; i < g_session.m_MaxItemIndex; ++i) {
					itemInfo = g_session.GetItemInfoByIndex(i);
					if (itemInfo.m_itemType == 5) {
						g_selected_item = i;
						break;
					}
				}
				if (g_selected_item > 0) {
					SetcurStance_B(2);
				} else {
					sprintf(msg, "Nothing worth selling");
					TheActor->SendMsg(NULL, AM_CHAT, (int)msg, 0, 0);
					TheActor->SendMsg(NULL, AM_EMOTION, 0x14, 0, 0);
				}
			} else if (g_menuSelect == 3) {
				SetcurStance_B(3);
			}
		} else if (g_curStance_B == 2) {
			if (g_BS_phase == 3) {
				g_BS_phase = 4;
				g_menuFreeze = g_menuSelect;
			}
		} else if (g_curStance_B == 3) {
			if (g_BS_phase == 3) {
				g_BS_phase = 4;
				g_menuFreeze = g_menuSelect;
			} else if (g_BS_phase == 6) {
				g_menuFreeze = g_menuSelect;
				g_BS_phase = 7;
				g_session.GetGameActorByAid(2);
				if (g_menuFreeze == 2) {
					CGameActor* actor = g_session.GetGameActorByAid(11);
					actor->SendMsg(NULL, AM_EMOTION, 9, 0, 0);
					actor->SetState(6);
					g_BS_NextActTime = timeGetTime() + 3000;
				} else {
					g_BS_NextActTime = timeGetTime() + 200;
				}
			}
		}
	} else if (g_curScreen == 100) {
		if (g_menuSelect == 1) {
			SetcurScreen(110);
		} else if (g_menuSelect == 2) {
			SetcurScreen(120);
		} else if (g_menuSelect == 3) {
			SetcurScreen(101);
		}
	} else if (g_curScreen == 102) {
		SetcurScreen(100);
	} else if (g_curScreen == 110) {
		g_curSave = (char)g_menuSelect;
		SetcurScreen(111);
	} else if (g_curScreen == 120) {
		g_curSave = (char)g_menuSelect;
		HP_LoadGame(g_curSave);
		SetcurScreen(1);
	}
}

void UIWindowMgr::All_CheckInput()
{
	if (g_SpellSeek == 1) {
		if (g_mouse.GetYPos() <= 170) {
			if (g_SpellSeekTime + 3000 >= timeGetTime()) {
				if (g_session.HaveThisSkill(0x1b) >= 1) {
					g_NextSkill = 0x1b;
					g_SpellSeek = 0;
				}
			}
		}
	}

	if (g_mouse.GetLBtn() == BTN_DOWN
		&& g_mouse.GetXPos() <= 650 && g_mouse.GetYPos() >= 200 && g_mouse.GetYPos() <= 450
		&& (g_curStance == 2 || g_SpellSeek != 1)) {
		g_SpellSeek = 10;
	}

	if (g_SpellSeek == 10) {
		if (g_mouse.GetXPos() >= 880 && g_mouse.GetYPos() >= 200 && g_mouse.GetYPos() <= 450)
			g_SpellSeek = 11;
	} else if (g_SpellSeek == 11) {
		if (g_mouse.GetXPos() >= 690 && g_mouse.GetXPos() <= 840 && g_mouse.GetYPos() <= 175)
			g_SpellSeek = 12;
	} else if (g_SpellSeek == 12) {
		if (g_mouse.GetXPos() >= 690 && g_mouse.GetXPos() <= 840 && g_mouse.GetYPos() >= 470) {
			if (g_session.HaveThisSkill(0x49) >= 1) {
				g_NextSkill = 0x49;
				g_SpellSeek = 0;
			}
		}
	}

	if (g_SpellSeek == 10) {
		if (g_mouse.GetXPos() >= 690 && g_mouse.GetXPos() <= 840 && g_mouse.GetYPos() <= 175)
			g_SpellSeek = 20;
	} else if (g_SpellSeek == 20) {
		if (g_mouse.GetXPos() >= 880 && g_mouse.GetYPos() >= 200 && g_mouse.GetYPos() <= 450)
			g_SpellSeek = 21;
	} else if (g_SpellSeek == 21) {
		if (g_mouse.GetXPos() >= 690 && g_mouse.GetXPos() <= 840 && g_mouse.GetYPos() >= 470)
			g_SpellSeek = 22;
	} else if (g_SpellSeek == 22) {
		if (g_mouse.GetXPos() <= 650 && g_mouse.GetYPos() >= 200 && g_mouse.GetYPos() <= 450) {
			if (g_session.HaveThisSkill(0x22) >= 1) {
				if (g_session.GetHp() <= g_session.GetMaxhp() / 10 * 6) {
					g_NextSkill = 0x22;
					g_SpellSeek = 0;
				}
			}
		}
	}

	if (g_SpellSeek == 2) {
		if (g_mouse.GetYPos() <= 170) {
			if (g_SpellSeekTime + 3000 >= timeGetTime()) {
				if (g_session.HaveThisSkill(0x21) >= 1) {
					g_NextSkill = 0x21;
					g_SpellSeek = 0;
				}
			}
		}
	}

	if (g_mouse.GetLBtn() == BTN_UP)
		g_SpellSeek = 0;
	if (g_mouse.GetRBtn() == BTN_UP)
		g_SpellSeek = 0;
}
