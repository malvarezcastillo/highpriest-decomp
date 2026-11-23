#include "common.h"
#include "MsgStrings.h"
#include "framework/Locale.h"
#include "packet.h"
#include "PaddingPacket.h"
#include "resource/Wave.h"
#include <stdio.h>
#include <time.h>
#include "GameMode.h"
#include "GameWorld.h"
#include "GameActor.h"
#include "Mode.h"
#include "Session.h"
#include "GuildInfo.h"
#include "resource/Attr.h"
#include "window/UISys.h"
#include "window/UIFrameWnd.h"
#include "UIGuildWnd.h"
#include "pdb_enums.h"
#include "device/Connection.h"
#include "resource/Ground.h"

int g_encCount = 2;
int g_decCount = 2;

extern CSession      g_session;
extern UIWindowMgr   g_windowMgr;
extern CRenderer*    g_renderer;
extern CModeMgr      g_modeMgr;
extern char          g_MessageHi;
extern short         g_Aco_Job;
extern short         g_BS_Job;
extern short         Boy_Job;
extern char          g_Aco_Hair;
extern char          g_BS_Hair;
extern char          g_Aco_Hair_Color;
extern char          g_BS_Hair_Color;


#include "gs.h"
#include "RagEffect.h"

#include "framework/Language.h"

extern int       g_loginStartMode;
extern ServiceType  g_serviceType;
extern bool         g_pingLog;
extern std::list<DWORD> g_pingLogList;
extern DWORD        g_pingLogStart;
extern ServerAddress g_zoneServerAddr;
extern char         g_checkBOT;
extern short        g_BOT_x;
extern short        g_BOT_y;
extern short        g_curStory;
extern short        bat_mobskill_turn;
extern const char*  MsgStr(MSGSTRINGID id);
extern bool         IsGravityAid(DWORD aid);
extern int          IsNameSpecific(DWORD aid);
extern bool         IsNameYellow(DWORD aid);
extern bool         IsEffectStatePushCart(DWORD effectState);
extern bool         IsEffectStateSpecialHiding(DWORD effectState);
extern bool         IsEffectStateHiding(DWORD effectState);
extern bool         IsEffectStateBurrow(DWORD effectState);
extern const char*  GetUserCountString(WORD usercount);
static inline void DecodePosDirNewentry(BYTE *packet, int &rsx, int &rsy, int &rdir);

void	CGameMode :: Zc_Use_Skill(const char *buf)
{
	PACKET_ZC_USE_SKILL *	p = (PACKET_ZC_USE_SKILL *)buf;
#if defined(__REASSEMBLY_PACKET)
	p->Crypt();
#endif

	sAssert(p->SKID >= SKID_NV_BASIC && p->SKID < SKID_LAST);



	int skillId = p->SKID;
	CGameActor *	sActor = NULL;
	if (p->srcAID == g_session.GetAid())
		sActor = m_world->m_player;
	else
		sActor = m_world->GetGameActorByAID(p->srcAID);
#ifdef	MOB_SKILL
	int act = -1, loopType;
#endif












#ifdef	MOB_SKILL
	if (sActor)
	{
#ifdef	SECONDJOB_SKILL2

		if (p->SKID == SKID_CR_AUTOGUARD || p->SKID == SKID_ML_AUTOGUARD)
		{
			sActor->SetState(ST_ATTACK);
		}
#endif

		if (!g_session.IsMonster(sActor) && SKID_MG_SRECOVERY != p->SKID)
		{
#ifdef WEDDING_PATCH
			if (p->SKID == SKID_WE_MALE || p->SKID == SKID_WE_FEMALE )
			{
				SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->targetAID);

			}
			else
#endif
				SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->level);
		}
		else
		{
			g_session.GetMonsterSkillInfo((char*)g_session.m_jobNameTable[sActor->GetJob()], p->SKID, act, loopType);

			if (act != -1)
			{
				if (sActor)
				{
					if (loopType == 0)
						loopType = MT_ONETIME;
					else loopType = 0;

					sActor->SetAction((act - 1) * 8, 1, loopType);
				}
			}
		}
	}
#else
	SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->level);
#endif

#ifdef	MOB_SKILL
	if (act == -1)
	{
#ifdef	SECONDJOB_SKILL2
		if (skillId != SKID_MO_TRIPLEATTACK && skillId != SKID_MO_CHAINCOMBO && skillId != SKID_MO_COMBOFINISH && skillId != SKID_CH_TIGERFIST )
		{
			int beginEffectId, skillMotionType;
			g_session.GetSkillActionInfo(p->SKID, beginEffectId, skillMotionType);
			if (sActor)
			{
				if (sActor->m_stateId != ST_MOVE && skillMotionType == ST_MOVE )
					sActor->SetState(ST_SKILL);
				else
					sActor->SetState(skillMotionType);
			}
		}
#else
		int beginEffectId, skillMotionType;
		g_session.GetSkillActionInfo(p->SKID, beginEffectId, skillMotionType);
		if (sActor)
		{
			sActor->SetState(skillMotionType);
		}
#endif
	}
#else
	int beginEffectId, skillMotionType;
	g_session.GetSkillActionInfo(p->SKID, beginEffectId, skillMotionType);
	if (sActor) {
		sActor->SetState(skillMotionType);
	}
#endif

	CGameActor *	tActor = NULL;
	switch (p->SKID)
	{
	case	SKID_MG_FIREWALL:
	case	SKID_MG_SAFETYWALL:
	case	SKID_MG_THUNDERSTORM:
	case	SKID_WZ_VERMILION:
	case	SKID_WZ_STORMGUST:
	case	SKID_WZ_METEOR:
	case	SKID_PR_SANCTUARY:
	case	SKID_PR_MAGNUS:
	case	SKID_BS_HAMMERFALL:
	case	SKID_CR_SLIMPITCHER:

		{
			if (sActor)
			{
				DWORD posData = p->targetAID;
				WORD cellX = posData >> 16;
				WORD cellY = posData & 0x0000ffff;
				float tPosX, tPosZ;
				GetClientCoor(cellX, cellY, tPosX, tPosZ);
				sActor->CalcDir(tPosX, tPosZ);
			}
			break;
		}
	case	SKID_HT_REMOVETRAP:
	case	SKID_HT_SPRINGTRAP:
		{
			CSkill *	theSkill;
			if (p->targetAID == g_session.GetAid())
				theSkill = m_world->GetSkillByAID(p->targetAID);
			else
				theSkill = m_world->GetSkillByAID(p->targetAID);
			if (theSkill && sActor)
				sActor->CalcDir(theSkill->m_pos.x, theSkill->m_pos.z);
			break;
		}
#if defined (_HOMUN)
	case	SKID_AM_REST:
		{
			if ( p->srcAID == g_session.GetAid() )
			{
				if (g_windowMgr.GetHomunInfoWnd())
					g_windowMgr.DeleteWindow(WID_HOMUNINFOWND);
				if (g_windowMgr.GetHomunSkillListWnd())
					g_windowMgr.DeleteWindow(WID_HOMUNSKILLLISTWND);
				g_session.InitHomunTargetInfo ( ) ;
				g_session.m_homunAlive = false ;
			}
		}
#endif
	default:
		{
			if (p->targetAID == g_session.GetAid())
				tActor = m_world->m_player;
			else
				tActor = m_world->GetGameActorByAID(p->targetAID);
			if (sActor && tActor && (sActor!=tActor))
				sActor->CalcDir(tActor->m_pos.x, tActor->m_pos.z);
		}
		break;
	}

	if (p->result)
	{
		int		level = p->level;
		if (level < 1)
			level = 1;
		if (level > 10 && SKID_MG_SRECOVERY != p->SKID)
			level = 10;

		switch (p->SKID)
		{





		case	SKID_PR_GLORIA:
			if (sActor)
			{
				sActor->LaunchEffect(EF_GLORIA);
			}
			break;
		case	SKID_MER_MAGNIFICAT:
		case	SKID_PR_MAGNIFICAT:
			if (sActor)
			{
					sActor->LaunchEffect(EF_MAGNIFICAT);
			}
			break;
		case	SKID_PR_KYRIE:
			if (sActor)
			{
				sActor->LaunchEffect(EF_KYRIE);
			}
			break;
		case	SKID_PR_SLOWPOISON:
			if ( g_serviceType == ServiceKorea )
			{
				if (tActor)
				{

					tActor->LaunchEffect(EF_SLOWPOISON);
				}
			}
			break;
		case	SKID_BS_ADRENALINE:
		case	SKID_BS_ADRENALINE2:
			if (sActor)
			{
				sActor->LaunchEffect(EF_HASTEUP);
			}
			break;
		case	SKID_BS_WEAPONPERFECT:
			if (tActor)
			{
				tActor->LaunchEffect(EF_PERFECTION);
			}
			break;
		case	SKID_BS_MAXIMIZE:
			if (sActor)
			{
				sActor->LaunchEffect(EF_MAXPOWER);
			}
			break;
		case	SKID_BS_OVERTHRUST:
		case	SKID_WS_OVERTHRUSTMAX:
			if (sActor)
			{
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_OVERTHRUST);
				sActor->LaunchEffect(EF_OVERTHRUST);
			}
			break;
		case	SKID_BS_GREED:
			{
				if (sActor)
					PlayWave("effect\\ef_entry.wav");
			}
			break;
		case	SKID_ALL_CATCRY:
			{
				if (sActor)
					PlayWave("effect\\2008cat.wav");
			}
			break;
		case	SKID_NPC_PICK_ITEM:
			{
				if (sActor)
					PlayWave("dream_device_edq.wav");
			}
			break;
		case	SKID_ALL_PARTYFLEE:
			{
				if (tActor)
					tActor->LaunchEffect(EF_FLOWERLEAF);
			}
			break;
		case	SKID_ALL_ANGEL_PROTECT:
			{
				if (tActor)
					tActor->LaunchEffect(EF_BENEDICTIO);

				break;
			}
		case	SKID_AS_ENCHANTPOISON:
			if (tActor) {


				tActor->LaunchEffect(EF_ENCHANTPOISON);
			}
			break;
		case	SKID_AS_POISONREACT:
			if (tActor)
			{
				tActor->LaunchEffect(EF_POISONREACT);
			}
			break;
		case	SKID_AS_SPLASHER:
			if (tActor)
			{
#if defined (_UPDATE2004B)
				tActor->LaunchEffect(EF_ENCHANTPOISON);
#else
				tActor->LaunchEffect(EF_SPLASHER);
#endif
			}
			break;
		case	SKID_KN_TWOHANDQUICKEN:
			if (sActor)
			{
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_TWOHANDQUICKEN);
				sActor->LaunchEffect(EF_TWOHANDQUICKEN);
			}
			break;
		case	SKID_KN_ONEHAND:
			if (sActor)
			{

				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_TWOHANDQUICKEN);
				sActor->LaunchEffect(EF_TWOHANDQUICKEN);
			}
			break;
		case	SKID_KN_AUTOCOUNTER:
			if (sActor)
			{
#if defined(BALANCE_PATCH)
				sActor->SendMsg(NULL, AM_MAKE_SKILL_RECHARGEGAGE, p->level * 400, 0, 0);
				sActor->SetState(ST_AUTOCOUNTER);
				if (p->srcAID != g_session.GetAid())
				{
					sActor->SendMsg(NULL, AM_DELETE_SKILL_RECHARGEGAGE);
				}
#else
				sActor->SetState(ST_AUTOCOUNTER);
#endif
			}
			break;
		case	SKID_PR_IMPOSITIO:
			if (tActor)
			{
				tActor->LaunchEffect(EF_IMPOSITIO);
			}
			break;
		case	SKID_PR_SUFFRAGIUM:
			if (tActor)
			{
				tActor->LaunchEffect(EF_SUFFRAGIUM);
			}
			break;
		case	SKID_PR_ASPERSIO:
			if (tActor)
			{
				tActor->LaunchEffect(EF_ASPERSIO);
			}
			break;
		case	SKID_AL_CURE:
			if (tActor)
			{
				tActor->LaunchEffect(EF_CURE);
			}
			break;
		case	SKID_PR_STRECOVERY:
			if (tActor)
			{
				tActor->LaunchEffect(EF_RECOVERY);
			}
			break;
		case	SKID_PR_LEXDIVINA:
			if (tActor)
			{
				tActor->LaunchEffect(EF_LEXDIVINA);
			}
			break;
		case	SKID_PR_LEXAETERNA:
			if (tActor)
			{
				tActor->LaunchEffect(EF_LEXAETERNA);
			}
			break;
		case	SKID_WZ_SIGHTRASHER:
			if (sActor) {
				sActor->LaunchEffect(EF_SIGHTRASHER);
			}
			break;
		case	SKID_WZ_FROSTNOVA:
			if (tActor)
			{
				tActor->LaunchEffect(EF_FROSTDIVER2);
			}
			break;
		case	SKID_BS_REPAIRWEAPON:
			if (tActor)
			{
				tActor->LaunchEffect(EF_REPAIRWEAPON);
			}
			break;
		case	SKID_HT_SPRINGTRAP:
			{
				if (sActor)
				{
					CSkill *	theSkill = m_world->GetSkillByAID(p->targetAID);
					if (theSkill) {
						theSkill->LaunchEffect(EF_SPRINGTRAP);
						if (sActor->GetBird())
							sActor->GetBird()->SendMsg(NULL, AM_REQ_ATTACK, int(&theSkill->m_pos));
					}
				}
			}
			break;
		case	SKID_HT_REMOVETRAP:
			{
				CSkill *	theSkill = m_world->GetSkillByAID(p->targetAID);
				if (theSkill)
				{
					theSkill->LaunchEffect(EF_REMOVETRAP);
				}
			}
			break;
		case	SKID_ALL_RESURRECTION:
			if (tActor)
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_RESURRECTION);
				if (g_session.IsThisPC(tActor->GetJob()))
					effect = tActor->LaunchEffect(EF_REVIVE);

			}
			break;





		case	SKID_TF_DETOXIFY:
			if (tActor)
			{
				tActor->LaunchEffect(EF_DETOXICATION);
			}
			break;
		case	SKID_TF_STEAL:
			if (tActor)
			{
				tActor->LaunchEffect(EF_STEAL);
			}
			break;
		case	SKID_MG_FIREWALL:
			break;
		case	SKID_MG_SAFETYWALL:
			break;
		case	SKID_MG_FROSTDIVER:
			break;
		case	SKID_MG_STONECURSE:
			{
				if (tActor)
				{
					switch (p->SKID)
					{
					case	SKID_MG_STONECURSE:
						{
							tActor->LaunchEffect(EF_STONECURSE);
							vector3d &	pPos = m_world->m_player->m_pos;
							PlayWave("_stonecurse.wav", tActor->m_pos.x-pPos.x, 0, tActor->m_pos.z-pPos.z);
							break;
						}
					}
				}
			}
			break;
		case	SKID_AL_HEAL:
#ifdef WEDDING_PATCH
		case	SKID_WE_MALE:
#endif
#if defined (_HOMUN)
		case	SKID_HLIF_HEAL:
#endif
			{
				if (tActor)
				{
					if ( p->level >= 4000 )
					{
#ifdef SECONDJOB_SKILL2
						tActor->LaunchEffect(EF_HEAL4);
#else
						tActor->LaunchEffect(EF_EXIT);
#endif
					}
					else if ( p->level >= 2000 )
					{
#ifdef SECONDJOB_SKILL2
						tActor->LaunchEffect(EF_HEAL2);
#else
						tActor->LaunchEffect(EF_EXIT);
#endif
					}
					else if ( p->level >= 200 )
					{
#ifdef SECONDJOB_SKILL2
						tActor->LaunchEffect(EF_HEAL);
#else
						tActor->LaunchEffect(EF_EXIT);
#endif
					}
					else
					{
						tActor->LaunchEffect(EF_EXIT);
					}

					tActor->SendMsg(NULL, AM_MAKE_NUMBER, p->level, 0xff00ff00, MET_NUMBER + 100);

				}
				break;
			}
		case	SKID_AL_INCAGI:
		case	SKID_CASH_INCAGI:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_INCAGILITY);
				}
				break;
			}
		case	SKID_AL_DECAGI:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_DECAGILITY);
				}
				break;
			}
		case	SKID_AL_HOLYWATER:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_AQUA);
				}
				break;
			}
		case	SKID_AL_RUWACH:
			break;
		case	SKID_AL_CRUCIS:
			if (sActor)
			{
				sActor->LaunchEffect(EF_SIGNUM);
			}
			break;
		case	SKID_AL_ANGELUS:
			if (sActor)
			{
				sActor->LaunchEffect(EF_ANGELUS);
			}
			break;
		case	SKID_AL_BLESSING:
		case	SKID_CASH_BLESSING:
			if (tActor)
			{
				tActor->LaunchEffect(EF_BLESSING);
			}
			break;






		case	SKID_SM_ENDURE:
			if (sActor)
			{
				sActor->LaunchEffect(EF_ENDURE);
			}
			break;
		case	SKID_SM_PROVOKE:
			if (tActor)
			{
				tActor->LaunchEffect(EF_PROVOKE);
			}
			break;
		case	SKID_SM_MAGNUM:
			if (sActor)
			{
					sActor->LaunchEffect(EF_MAGNUMBREAK);
			}
			break;
		case	SKID_AC_CONCENTRATION:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CONCENTRATION);
			}
			break;
#if defined(BALANCE_PATCH)
		case	SKID_MG_ENERGYCOAT:
			if (sActor)
			{
				sActor->LaunchEffect(EF_ENERGYCOAT);
			}
			break;
#endif
#ifdef	MOB_SKILL
		case	SKID_NPC_CHANGEWATER:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHANGECOLD);
			}
			break;
		case	SKID_NPC_CHANGEGROUND:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHANGEEARTH);
			}
			break;
		case	SKID_NPC_CHANGEFIRE:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHANGEFIRE);
			}
			break;
		case	SKID_NPC_CHANGEWIND:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHANGEWIND);
			}
			break;
		case	SKID_NPC_CHANGEPOISON:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHANGEPOISON);
			}
			break;
		case	SKID_NPC_CHANGEHOLY:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHAINGEHOLY);
			}
			break;
		case	SKID_NPC_CHANGEDARKNESS:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHANGEDARK);
			}
			break;
		case	SKID_NPC_CHANGETELEKINESIS:
			if (sActor)
			{
				sActor->LaunchEffect(EF_CHANGEFLAME);
			}
			break;
		case	SKID_NPC_SUICIDE:
			if (sActor)
			{
				CRagEffect *suicideEffect = m_world->m_player->LaunchEffect(EF_SUICIDE);
				if (suicideEffect)
				{
					suicideEffect->SendMsg(NULL, AM_SETPOS, int(&sActor->m_pos));
				}
			}
			break;
		case	SKID_NPC_SELFDESTRUCTION:
			if (sActor)
				sActor->LaunchEffect(EF_SUI_EXPLOSION);

			break;
		case	SKID_NPC_SUMMONSLAVE:
			if (sActor)
				sActor->LaunchEffect(EF_SUMMONSLAVE);

			break;
		case	SKID_NPC_KEEPING:
			if (sActor)
				sActor->LaunchEffect(EF_KEEPING);

			break;
		case	SKID_NPC_DEFENDER:
			if (sActor)
				sActor->LaunchEffect(EF_DEFFENDER);
			break;
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_STOP:
			{
				if (tActor)
					tActor->LaunchEffect(EF_NPC_STOP);
			}
			break;
#endif
#endif

#ifdef	SECONDJOB_SKILL2
		case	SKID_SA_SPELLBREAKER:
			if (tActor)
			{
				tActor->LaunchEffect(EF_SPELLBREAKER);
			}
			break;
		case	SKID_SA_DISPELL:
			if (tActor)
			{
				tActor->LaunchEffect(EF_DISPELL);
			}
			break;
		case	SKID_SA_MAGICROD:
			if (sActor)
			{
				sActor->LaunchEffect(EF_MAGICROD);
			}
			break;
		case	SKID_SA_FLAMELAUNCHER:
		case	SKID_SA_ELEMENTFIRE:
			if (tActor)
			{
				tActor->LaunchEffect(EF_FLAMELAUNCHER);
			}
			break;
		case	SKID_SA_FROSTWEAPON:
		case	SKID_SA_ELEMENTWATER:
			if (tActor)
			{
				tActor->LaunchEffect(EF_FROSTWEAPON);
			}
			break;
		case	SKID_SA_LIGHTNINGLOADER:
		case	SKID_SA_ELEMENTWIND:
			if (tActor)
			{
				tActor->LaunchEffect(EF_LIGHTNINGLOADER);
			}
			break;
		case	SKID_SA_SEISMICWEAPON:
		case	SKID_SA_ELEMENTGROUND:
			if (tActor)
				tActor->LaunchEffect(EF_SEISMICWEAPON);
			break;
		case	SKID_ML_AUTOGUARD:
		case	SKID_CR_AUTOGUARD:
			if (sActor)
				sActor->SendMsg(NULL, AM_MAKE_GUARD);
			break;
		case	SKID_CR_SHRINK:
			if (sActor)
				sActor->LaunchEffect(EF_SHRINK);
			break;
		case	SKID_LK_PARRYING:
		case	SKID_MS_PARRYING:
			if (sActor)
			{
				sActor->SendMsg(NULL, AM_MAKE_GUARD);
			}
			break;
		case	SKID_CR_PROVIDENCE:
			if (tActor)
				tActor->LaunchEffect(EF_PROVIDENCE);
			break;
		case	SKID_CR_DEVOTION:
			if (sActor)
				sActor->LaunchEffect(EF_DEVOTION);
			break;
		case	SKID_CR_SPEARQUICKEN:
			if (sActor)
			{
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_SPEARQUICKEN);
				sActor->LaunchEffect(EF_SPEARQUICKEN);
			}
			break;
		case	SKID_CR_REFLECTSHIELD :
			if (sActor)
				sActor->LaunchEffect(EF_REFLECTSHIELD);
			break;
		case	SKID_ML_DEFENDER :
		case	SKID_CR_DEFENDER :
			if (sActor)
				sActor->LaunchEffect(EF_DEFENDER);
			break;
		case	SKID_MO_ABSORBSPIRITS :
			if (sActor)
				sActor->LaunchEffect(EF_ABSORBSPIRITS);
			break;
#ifdef WEDDING_PATCH
		case	SKID_WE_FEMALE :
			if (tActor)
			{
				tActor->LaunchEffect(EF_ABSORBSPIRITS);
				tActor->SendMsg(NULL, AM_MAKE_NUMBER, p->level, 0xff0000ff, MET_NUMBER + 100);
			}
			break;
		case	SKID_WE_BABY :
			if (tActor)
			{
				tActor->LaunchEffect(EF_BABY);
			}
			break;
#endif
		case	SKID_MO_BLADESTOP:
			if (sActor)
				sActor->SetForceAnimation(96, 3, 3,96, 3, TRUE, 3000, FALSE);
			break;
		case	SKID_NV_FIRSTAID:
			if (sActor)
				sActor->LaunchEffect(EF_FIRSTAID);
			break;
		case	SKID_MC_LOUD:
#ifdef SECONDJOB_SKILL2
			if (sActor)
			{
				sActor->LaunchEffect(EF_LOUD);
			}
#endif
			break;
		case	SKID_MO_EXPLOSIONSPIRITS :
			if (sActor)
			{
				CGameMode * gameMode = g_modeMgr.GetGameMode();
				gameMode->SendMsg(MM_QUAKE);

				PlayWave("effect\\mon_\xc6\xf8\xb1\xe2.wav");

				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_GUMGANG);
				sActor->LaunchEffect(EF_GUMGANG);
				sActor->LaunchEffect(EF_GUMGANG2);

				sActor->SetForceAnimSpeed(30);
				sActor->SetForceAnimation(96, 1, 2,96, 2, TRUE, 1000, FALSE);
			}
			break;
		case	SKID_MO_STEELBODY :
			if (sActor)
			{

				PlayWave("effect\\mon_\xb8\xcd\xb7\xe6\xb0\xfa\xb0\xad.wav");

				CGameMode * gameMode = g_modeMgr.GetGameMode();
				gameMode->SendMsg(MM_QUAKE);

				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_STEELBODY);
				sActor->LaunchEffect(EF_STEELBODY);
				sActor->LaunchEffect(EF_GUMGANG2);
				sActor->SetForceAnimation(96, 0, 0,96, 0, TRUE, 20, FALSE);
			}
			break;
		case	SKID_LK_AURABLADE :
			{
				if (sActor)
				{
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_AURABLADE);
					sActor->LaunchEffect(EF_AURABLADE);
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_AURABLADE2);
					sActor->LaunchEffect(EF_AURABLADE2);
					sActor->SetForceAnimation(32, 0, 5,32, 5, TRUE, 1000, 0);

					PlayWave("effect\\\xbf\xc0\xb6\xf3 \xba\xed\xb7\xb9\xc0\xcc\xb5\xe5.wav");
				}
			}
			break;
		case	SKID_LK_CONCENTRATION :
			{
				if (sActor)
				{
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_LKCONCENTRATION);
					sActor->LaunchEffect(EF_LKCONCENTRATION);
				}
			}
			break;
		case	SKID_MS_BERSERK:
		case	SKID_LK_BERSERK :
		case	SKID_LK_FURY :
			{
				if (sActor)
				{
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_REDBODY);
					sActor->LaunchEffect(EF_REDBODY);

					CGameMode * gameMode = g_modeMgr.GetGameMode();
					gameMode->SendMsg(MM_QUAKE);

					PlayWave("effect\\\xb9\xf6\xbc\xad\xc5\xa9.wav");
				}
			}
			break;
		case	SKID_MO_BODYRELOCATION:
			{
			}
			break;
		case	SKID_RG_STEALCOIN:
			{
				if (tActor)
				{
					CRagEffect *	effect = tActor->LaunchEffect(EF_RG_COIN);
					if (effect)
					{
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
					tActor->LaunchEffect(EF_STEALCOIN);
				}
			}
			break;
		case	SKID_ST_FULLSTRIP:
			{
				if (tActor)
				{
					CRagEffect *	effect = tActor->LaunchEffect(EF_RG_COIN2);
					if (effect)
					{
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_ST_PRESERVE:
			{
				if (sActor)
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_GUARD2);
				}
			}
			break;
		case	SKID_RG_RAID:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_TEIHIT3);
				}
			}
			break;
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_POWERUP:
			if (sActor)
			{

				PlayWave("effect\\mon_\xc6\xf8\xb1\xe2.wav");

				sActor->LaunchEffect(EF_GUMGANGNPC);
			}
			break;
#endif
		case	SKID_RG_STRIPWEAPON:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_WEAPONBRAKER:
#endif
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_STRIPWEAPON);
				}
			}
			break;
		case	SKID_RG_STRIPSHIELD:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_STRIPSHIELD);
				}
			}
			break;
		case	SKID_RG_STRIPARMOR:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_STRIPARMOR);
				}
			}
			break;
		case	SKID_RG_STRIPHELM:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_STRIPHELM);
				}
			}
			break;
		case	SKID_RG_CLOSECONFINE:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_QUAKEBODY4);
				}
			}
			break;
		case	SKID_DC_SCREAM:
			{
				if (sActor)
				{
					sActor->SetForceAnimation(96, 0 , 0, 96, 0, TRUE, 1200, FALSE);
					if ( sActor == m_world->m_player )
						sActor->LaunchEffect(EF_TALK_SCREAM);
				}
			}
			break;
		case	SKID_DC_WINKCHARM:
			{
				if (sActor && tActor)
				{
					sActor->SendMsg(NULL, AM_EMOTION, ET_CHUP);
					PlayWave("effect\\vallentine.wav");

					CRagEffect *	effect2 = sActor->LaunchEffect(EF_WINK);
					if (effect2)
					{
						vector3d	pos = tActor->m_pos;
						effect2->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_BA_FROSTJOKER:
			{
				if (sActor)
				{
					sActor->SetForceAnimation(96, 4 , 5, 96, 4, TRUE, 1800, TRUE);
					if ( sActor == m_world->m_player )
						sActor->LaunchEffect(EF_TALK_FROSTJOKE);
				}
			}
			break;
		case	SKID_BA_PANGVOICE:
			{
				if (sActor && tActor)
				{
					sActor->SetForceAnimation(96, 4 , 5, 96, 4, TRUE, 1800, TRUE);
					PlayWave("amon_ra_die01.wav");

					{
						CRagEffect *	effect2 = sActor->LaunchEffect(EF_FVOICE);
						if (effect2)
						{
							vector3d	pos = tActor->m_pos;
							effect2->SendMsg(NULL, AM_SETPOS, int(&pos));
						}
					}
				}
			}
			break;

		case	SKID_BA_ASSASSINCROSS:
			{
				PlayWave("effect\\\xbc\xae\xbe\xe7\xc0\xc7 \xbe\xee\xbd\xd8\xbd\xc5.wav");
			}
			break;
		case	SKID_BA_POEMBRAGI:
			{
				PlayWave("effect\\\xba\xea\xb6\xf3\xb1\xe2\xc0\xc7 \xbd\xc3.wav");
			}
			break;
		case	SKID_BA_APPLEIDUN:
			{
				PlayWave("effect\\\xc0\xcc\xb5\xd0\xc0\xc7 \xbb\xe7\xb0\xfa.wav");
			}
			break;
		case	SKID_DC_HUMMING:
			{
				PlayWave("effect\\\xc8\xef\xbe\xf3\xb0\xc5\xb8\xb2.wav");
			}
			break;
		case	SKID_DC_DONTFORGETME:
			{
				PlayWave("effect\\\xb3\xaa\xb8\xa6\xc0\xd8\xc1\xf6\xb8\xbb\xbe\xc6\xbf\xe4.wav");
			}
			break;
		case	SKID_DC_SERVICEFORYOU:
			{
				PlayWave("effect\\\xb4\xe7\xbd\xc5\xc0\xbb \xc0\xa7\xc7\xd1 \xbc\xad\xba\xf1\xbd\xba.wav");
			}
			break;
		case	SKID_BD_LULLABY:
			{
				PlayWave("effect\\\xc0\xda\xc0\xe5\xb0\xa1.wav");
			}
			break;
		case	SKID_CG_HERMODE:
			{
				PlayWave("effect\\\xc7\xec\xb8\xa3\xb8\xf0\xb5\xe5\xc0\xc7 \xc1\xf6\xc6\xce\xc0\xcc.wav");
			}
			break;
		case	SKID_CG_TAROTCARD:
			{
				if (tActor)
				{
					PlayWave("effect\\priest_slowpoison.wav");
					tActor->LaunchEffect(EF_CHEMICALBODY);
				}
			}
			break;
		case	SKID_CG_LONGINGFREEDOM:
			{
				if (sActor)
					sActor->LaunchEffect(EF_CHEMICALBODY);
				PlayWave("effect\\ac_concentration.wav");
			}
			break;
		case	SKID_BD_ETERNALCHAOS:
			{
				PlayWave("effect\\\xbf\xb5\xbf\xf8\xc0\xc7 \xc8\xa5\xb5\xb7.wav");
			}
			break;
		case	SKID_BD_DRUMBATTLEFIELD:
			{
#if defined (_UPDATE2004B)
				PlayWave("effect\\\xc0\xfc\xc0\xe5\xc0\xc7.wav");
#endif
			}
			break;
		case	SKID_BD_RINGNIBELUNGEN:
			{
				PlayWave("effect\\\xb4\xcf\xba\xa7\xb7\xee\xb0\xd5\xc0\xc7 \xb9\xdd\xc1\xf6.wav");
			}
			break;
		case	SKID_BD_INTOABYSS:
			{
				PlayWave("effect\\\xbd\xc9\xbf\xac\xbc\xd3\xc0\xb8\xb7\xce.wav");
			}
			break;
		case	SKID_BD_SIEGFRIED:
			{
#if defined (_UPDATE2004B)
				PlayWave("effect\\\xba\xd2\xbb\xe7\xbd\xc5.wav");
#endif
			}
			break;
		case	SKID_BD_RICHMANKIM:
			{
#if defined (_UPDATE2004B)
				PlayWave("effect\\\xb1\xe8\xbc\xad\xb9\xe6\xb5\xb7.wav");
#endif
			}
			break;
		case	SKID_BD_ROKISWEIL:
			{
#if defined (_UPDATE2004B)
				PlayWave("effect\\\xb7\xce\xc5\xb0.wav");
#endif
			}
			break;
		case	SKID_DC_FORTUNEKISS:
			{
#if defined (_UPDATE2004B)
				PlayWave("effect\\\xc7\xe0\xbf\xee\xc0\xc7.wav");
#endif
			}
			break;

		case	SKID_BA_WHISTLE:
			{
			}
			break;
		case	SKID_AM_CP_WEAPON:
		case	SKID_AM_CP_SHIELD:
		case	SKID_AM_CP_ARMOR:
		case	SKID_AM_CP_HELM:
			{
				if (tActor)
					CRagEffect *	effect = tActor->LaunchEffect(EF_CHEMICALPROTECTION);
			}
			break;
		case	SKID_CR_FULLPROTECTION:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_CHEMICALPROTECTION);
					tActor->LaunchEffect(EF_CHEMICALBODY);
				}
			}
			break;
		case	SKID_AM_POTIONPITCHER:
			{
				if (sActor && tActor)
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM2);
					if (effect)
					{
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
						effect->SendMsg(NULL, AM_SETCOUNT, level);
					}
				}
			}
			break;
		case	SKID_AM_BERSERKPITCHER:
			{
				if (sActor && tActor)
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM5);
					if (effect)
					{
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
						effect->SendMsg(NULL, AM_SETCOUNT, 4);
					}

					tActor->LaunchEffect(EF_POTION_BERSERK);
				}
			}
			break;
		case	SKID_DE_BERSERKAIZER:
			{
				if (sActor)
					sActor->LaunchEffect(EF_POTION_BERSERK);
			}
			break;
		case	SKID_MG_SRECOVERY:
			if (tActor)
			{
				tActor->SendMsg(NULL, AM_MAKE_NUMBER, level, 0xff0000ff, MET_NUMBER + 100);
			}
			break;
		case	SKID_HP_ASSUMPTIO:
		case	SKID_CASH_ASSUMPTIO:
			{
				if (tActor)
				{
					CRagEffect *	effect = tActor->LaunchEffect(EF_ASSUMPTIO);
					CRagEffect *	effect2 = tActor->LaunchEffect(EF_ASSUMPTIO2);
				}
			}
			break;
		case	SKID_HW_SOULDRAIN:
			{
				if (sActor && tActor)
				{
					sActor->SetForceAnimation(88, 0 , 3, 88, 3, TRUE, 1000, FALSE);

					CRagEffect *	effect = sActor->LaunchEffect(EF_ENERGYDRAIN2);
					if (effect)
					{
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}

					tActor->LaunchEffect(EF_TRANSBLUEBODY);
				}
			}
			break;
		case	SKID_HW_MAGICPOWER:
			{
				if (tActor)
				{
					tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_LIGHTBLADE);
					tActor->LaunchEffect(EF_LIGHTBLADE);

					PlayWave("effect\\\xb8\xb6\xb9\xfd\xb7\xc2 \xc1\xf5\xc6\xf8.wav");

				}
			}
			break;
		case	SKID_PF_HPCONVERSION:
			{
				if (sActor)
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_ENERGYDRAIN3);
				}
			}
			break;
		case	SKID_PF_SOULCHANGE:
			{
				if (sActor && tActor)
				{
					vector3d	pos;
					pos.y = tActor->GetGID();
					CRagEffect *	effect = sActor->LaunchEffect(EF_LINELINK2);
					if (effect)
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					sActor->LaunchEffect(EF_LINKLIGHT);
					tActor->LaunchEffect(EF_LINKLIGHT);

					sActor->LaunchEffect(EF_SOULCHANGE);
					tActor->LaunchEffect(EF_SOULCHANGE);
				}
			}
			break;
		case	SKID_PF_DOUBLECASTING:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_DOUBLECASTBODY);

					PlayWave("effect\\\xb8\xb6\xb9\xfd\xb7\xc2 \xc1\xf5\xc6\xf8.wav");
				}
			}
			break;
		case	SKID_PF_SOULBURN:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_SOULBURN);
					tActor->LaunchEffect(EF_MAGICCRASHER);
				}
			}
			break;
		case	SKID_PF_MINDBREAKER:
			{
				if (tActor)
					tActor->LaunchEffect(EF_MAGICCRASHER2);
			}
			break;
		case	SKID_PF_MEMORIZE:
			{
#if defined (_UPDATE2005A)
				if (sActor)
					sActor->LaunchEffect(EF_MEMORIZE);
#endif
			}
			break;
		case	SKID_SN_SIGHT:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_TRUESIGHT);
				}
			}
			break;
		case	SKID_ASC_METEORASSAULT:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_SOULBREAKER2);
				}
			}
			break;
		case	SKID_ASC_EDP:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_EDP);
				}
			}
			break;
		case	SKID_SN_WINDWALK:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_PORTAL4);
				}
			}
			break;
		case	SKID_WS_MELTDOWN:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_MELTDOWN);
				}
			}
			break;
		case	SKID_WS_CARTBOOST:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_CARTBOOST);
				}
			}
			break;

		case SKID_SM_BFHEAL:
			{



			}
			break ;

#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_AGIUP:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_AGIUP);
				}
			}
			break;
#endif
		case	SKID_ST_REJECTSWORD:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_REJECTSWORD);
				}
			}
			break;
		case	SKID_ST_CHASEWALK:
			{
				if (sActor)
					sActor->SendMsg(NULL, AM_MAKE_SMOKE_FFECT);
			}
			break;
		case	SKID_CG_MOONLIT:
			{
				if (sActor)
				{
					if (!sActor->IsEffectExist(EF_SPHEREWIND2))
						sActor->LaunchEffect(EF_SPHEREWIND2);
				}

				PlayWave("effect\\\xb4\xde\xba\xfb.wav");
			}
			break;
		case	SKID_CG_MARIONETTE:
			{
				if (sActor && tActor)
				{
					vector3d	pos;
					pos.y = tActor->GetGID();
					CRagEffect *	effect = sActor->LaunchEffect(EF_LINELINK3);
					if (effect)
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					sActor->LaunchEffect(EF_PINKBODY);
					tActor->LaunchEffect(EF_PINKBODY);
				}
			}
			break;
		case	SKID_TK_RUN:
			{
				if (sActor)
				{
					if (!sActor->IsEffectExist(EF_STOPRUN))
					{
						if (!sActor->IsEffectExist(EF_RUN))
						{
							sActor->LaunchEffect(EF_RUN);
						}
					}
					else
					{
						sActor->LaunchEffect(EF_STOPEFFECT);
					}
				}
			}
			break;
		case	SKID_TK_HIGHJUMP:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_LANDBODY);
					while (sActor->IsEffectExist(EF_JUMPBODY))
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_JUMPBODY);
				}
			}
			break;
		case	SKID_TK_STORMKICK:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_STORMKICK);
				}
			}
			break;
		case	SKID_TK_DODGE:
			{

				PlayWave("effect\\T_\xb3\xab\xb9\xfd.wav",0,-150,0);
			}
			break;
		case	SKID_TK_SEVENWIND:
			{
				if (sActor)
				{

					PlayWave("effect\\T_\xb9\xd9\xb6\xf7\xb9\xe6\xc3\xe2.wav");
					while (sActor->IsEffectExist(EF_STORMKICK3))
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_STORMKICK3);
					sActor->LaunchEffect(EF_STORMKICK3);

					while (sActor->IsEffectExist(EF_BEGINASURA1+(level-1)))
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_BEGINASURA1+(level-1));
					sActor->LaunchEffect(EF_BEGINASURA1+(level-1));
				}
			}
			break;
		case	SKID_TK_MISSION:
			{
				PlayWave("effect\\T_\xc7\xc7\xb8\xb5.wav",0,-100,0);
			}
			break;
		case	SKID_SG_FUSION :
			{
				if (sActor)
				{
					PlayWave("effect\\T_\xba\xaf\xbd\xc5.wav");

					CGameMode * gameMode = g_modeMgr.GetGameMode();
					gameMode->SendMsg(MM_QUAKE);
				}
			}
			break;
		case	SKID_SG_SUN_WARM :
		case	SKID_SG_MOON_WARM :
		case	SKID_SG_STAR_WARM :
			{
				if (sActor && tActor)
				{
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_DOUBLEGUMGANG);
					sActor->LaunchEffect(EF_DOUBLEGUMGANG);
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_REDLIGHTBODY);
					sActor->LaunchEffect(EF_REDLIGHTBODY);
					sActor->LaunchEffect(EF_HATED2);
				}
			}
			break;
		case	SKID_SG_HATE :
			{
				if (tActor)
				{

					PlayWave("effect\\T_\xb5\xee\xb7\xcf.wav",0,0,0);
					tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_HATED);
					tActor->LaunchEffect(EF_HATED);
				}
			}
			break;
		case	SKID_SG_SUN_COMFORT :
		case	SKID_SG_MOON_COMFORT :
		case	SKID_SG_STAR_COMFORT :
			{
				if (sActor)
				{
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_FLOWERCAST);
					sActor->LaunchEffect(EF_FLOWERCAST);
					sActor->LaunchEffect(EF_HATED);
				}
			}
			break;
		case	SKID_SL_SWOO :
			if (tActor)
			{

				PlayWave("effect\\T_\xbd\xb4\xbf\xf4.wav",0,-50,0);
				tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_BABYBODY);
				tActor->LaunchEffect(EF_BABYBODY);
				tActor->LaunchEffect(EF_M07);
			}
			break;
		case	SKID_SL_SKE :
			if (tActor)
			{

				PlayWave("effect\\T_\xb0\xf8\xb0\xdd\xb7\xc2.wav",0,0,0);
				tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_ASURABODY_MONSTER);
				tActor->LaunchEffect(EF_ASURABODY_MONSTER);
			}
			break;
		case	SKID_SL_SKA :
			if (tActor)
			{

				PlayWave("effect\\T_\xb9\xe6\xbe\xee\xc7\xfc.wav");
				CGameMode * gameMode = g_modeMgr.GetGameMode();
				gameMode->SendMsg(MM_QUAKE);

				tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_STEELBODY);
				tActor->LaunchEffect(EF_STEELBODY);
				tActor->LaunchEffect(EF_GUMGANG2);
			}
			break;
		case	SKID_SL_KAIZEL :
			if (tActor)
			{
				PlayWave("effect\\priest_resurrection.wav",0,0,0);
				tActor->LaunchEffect(EF_HATED);
				tActor->LaunchEffect(EF_KAIZEL);
			}
			break;
		case	SKID_SL_KAAHI :
			if (tActor)
			{

				PlayWave("effect\\T_\xba\xb8\xc1\xb6\xb8\xb6\xb9\xfd.wav",0,0,0);
				tActor->LaunchEffect(EF_HATED);
			}
			break;
		case	SKID_SL_KAUPE :
			if (tActor)
			{

				PlayWave("effect\\T_\xc4\xa1\xc0\xd7.wav",0,0,0);
				tActor->LaunchEffect(EF_BLUEBODY);
			}
			break;
		case	SKID_SL_KAITE :
			if (tActor)
			{

				PlayWave("effect\\T_\xb8\xb6\xb9\xfd\xb9\xdd\xbb\xe7.wav",0,0,0);
				tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_REFLECTBODY);
				tActor->LaunchEffect(EF_REFLECTBODY);
				tActor->LaunchEffect(EF_BLUEBODY);
			}
			break;
		case	SKID_SL_ALCHEMIST:
		case	SKID_SL_MONK:
		case	SKID_SL_STAR:
		case	SKID_SL_SAGE:
		case	SKID_SL_CRUSADER:
		case	SKID_SL_SUPERNOVICE:
		case	SKID_SL_KNIGHT:
		case	SKID_SL_WIZARD:
		case	SKID_SL_PRIEST:
		case	SKID_SL_BARDDANCER:
		case	SKID_SL_ROGUE:
		case	SKID_SL_ASSASIN:
		case	SKID_SL_BLACKSMITH:
		case	SKID_SL_HUNTER:
		case	SKID_SL_SOULLINKER:
		case	SKID_SL_HIGH:

		case	SKID_SL_DEATHKNIGHT:
		case	SKID_SL_COLLECTOR:
		case	SKID_SL_NINJA:
		case	SKID_SL_GUNNER:
			{
				if (tActor)
				{
					vector3d	pos;
					pos.y = tActor->GetGID();
					CRagEffect *	effect = tActor->LaunchEffect(EF_SOULLINK);
					if (effect)
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_ASURABODY);
					tActor->LaunchEffect(EF_ASURABODY);
				}
			}
			break;
		case	SKID_HLIF_AVOID:
			{
				if (sActor)
					sActor->LaunchEffect(EF_AGIUP);
				if (tActor)
					tActor->LaunchEffect(EF_AGIUP);
			}
			break;
		case	SKID_HLIF_CHANGE:
			{
				if (sActor)
					sActor->LaunchEffect(EF_MEMORIZE);
			}
			break;
		case	SKID_HVAN_EXPLOSION:
			{
				if (sActor)
					sActor->LaunchEffect(EF_SUI_EXPLOSION);
			}
			break;
		case	SKID_HFLI_SPEED:
		case	SKID_HFLI_FLEET:
			{
				if (sActor)
					sActor->LaunchEffect(EF_HOMUNCASTING);
			}
			break;
		case	SKID_HAMI_DEFENCE:
			{
				PlayWave("effect\\h_defence.wav");
				if (sActor)
					sActor->LaunchEffect(EF_HAMIDEFENCE);
				if (tActor)
					tActor->LaunchEffect(EF_HAMIDEFENCE);
			}
			break;
		case	SKID_HAMI_CASTLE:
			{
				PlayWave("effect\\h_castling.wav");
				if (sActor)
					sActor->LaunchEffect(EF_HAMICASTLE);
				if (tActor)
					tActor->LaunchEffect(EF_HAMICASTLE);
			}
			break;
		case	SKID_HAMI_BLOODLUST:
			{
				if (sActor)
					sActor->LaunchEffect(EF_HAMIBLOOD);
			}
			break;
		case	SKID_NJ_RAIGEKISAI:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_THUNDERSTORM2);
				}
			}
			break;
		case	SKID_NJ_BAKUENRYU:
			{
				if (tActor)
				{
					tActor->LaunchEffect(EF_BAKU);
				}
			}
			break;
		case	SKID_NJ_HYOUSYOURAKU:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_HYOUSYOURAKU);
				}
			}
			break;
		case	SKID_NJ_TATAMIGAESHI:
			{
				PlayWave("effect\\\xb4\xd9\xb4\xd9\xb9\xcc\xb5\xda\xc1\xfd\xb1\xe2.wav");
			}
			break;
		case	SKID_GS_CRACKER:
			{
				PlayWave("effect\\\xc5\xa9\xb7\xa1\xc4\xbf.wav");
			}
			break;
		case	SKID_GS_MADNESSCANCEL:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_MADNESS_BLUE);
				}
			}
			break;
		case	SKID_GS_ADJUSTMENT:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_MADNESS_RED);
				}
			}
			break;
		case	SKID_GS_INCREASING:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_AGIUP);
				}
			}
			break;
		case	SKID_GS_DISARM:
			{
				if (tActor)
				{
					CRagEffect *	effect = tActor->LaunchEffect(EF_RG_COIN3);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_GS_GATLINGFEVER:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_MADNESS_RED);
				}
			}
			break;
		case	SKID_GS_GLITTERING:
			{
				PlayWave("effect\\\xc7\xc3\xb8\xb3.wav");
			}
			break;
		case	SKID_GS_DESPERADO:
			{
				if (sActor)
				{
					sActor->LaunchEffect(EF_DESPERADO);
				}
			}
			break;
		case	SKID_MB_M_TELEPORT:
			{
				if (sActor)
				{
					sActor->SendMsg(NULL, AM_MAKE_SMOKE_FFECT);
				}

				if (tActor)
				{
					tActor->SendMsg(NULL, AM_MAKE_SMOKE_FFECT);
					tActor->LaunchEffect(EF_ENTRY2);
				}
			}
			break;
		case	SKID_DE_INSTANT:
			break;
#endif
		}
	}

}


void CGameMode::Zc_Notify_Standentry(char const *buf)
{
	PACKET_ZC_NOTIFY_STANDENTRY *p = (PACKET_ZC_NOTIFY_STANDENTRY *)buf;
	STANDENTRYSTRUCT s;

	s.shield = p->shield;
	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;
	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.state = p->state;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;
	s.shield = p->shield;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;
	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		if (theActor->GetJob() > JT_WARPNPC && theActor->GetJob() < JT_2004_JOB_BEGIN) {
			DWORD gdid = (DWORD)(p->accessory2 << 16);
			gdid |= p->accessory3;
			DWORD emblemVer = p->shield;
			if (gdid > 0 && theActor->GetGdid() == 0)
				theActor->SetGuildInfo(gdid, emblemVer);
		}

		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY, (int)(&s), 0, 0);

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) ||
			p->objecttype == NPC_MERSOL_TYPE ||
			g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		if (p->objecttype == NPC_MERSOL_TYPE)
			newGameActor->m_objectType = p->objecttype;

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		if (newGameActor) {
			newGameActor->SendMsg(NULL, AM_INITSTANDENTRY, (int)(&s), 0, 0);

			if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
				if (!g_session.IsPc(newGameActor->GetJob())) {
					if (g_session.IsGuardian(newGameActor->GetJob()))
						MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				} else {
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				}
			}

			if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
				DWORD gdid = (DWORD)(p->accessory2 << 16);
				gdid |= p->accessory3;
				if (gdid > 0)
					newGameActor->SetGuildInfo(gdid, p->shield);
			}
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}


void CGameMode::Zc_Notify_Standentry_Npc(char const *buf)
{
	PACKET_ZC_NOTIFY_STANDENTRY_NPC *p = (PACKET_ZC_NOTIFY_STANDENTRY_NPC *)buf;
	STANDENTRYSTRUCT s;

	s.state = 0;
	s.guildId = 0;
	s.guildEmblemVersion = 0;
	s.honor = 0;
	s.virtue = 0;
	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;
	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;
	s.shield = p->shield;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	DWORD GID = p->GID;
	s.x = p->PosDir[0];
	s.x = (s.x << 2) | ((p->PosDir[1] & 0xc0) >> 6);
	s.GID = GID;
	s.y = p->PosDir[1] & 0x3f;
	s.y = (s.y << 4) | ((p->PosDir[2] & 0xf0) >> 4);
	s.dir = p->PosDir[2] & 0xf;

	CGameActor *newGameActor = m_world->GetGameActorByAID(GID);
	if (newGameActor) {
		newGameActor->SendMsg(NULL, AM_STANDENTRY, int(&s), 0, 0);
		if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
			DWORD gdid = (p->accessory2 << 16) | p->accessory3;
			int emblemVer = p->shield;
			if (gdid > 0 && newGameActor->GetGdid() == 0)
				newGameActor->SetGuildInfo(gdid, emblemVer);
		}
		return;
	}

	newGameActor = NewNpc(p->job);
	s.sex = SEX_FEMALE;
	if (p->objecttype == NPC_MERSOL_TYPE)
		newGameActor->m_objectType = p->objecttype;

	m_world->m_gameObjectList.push_back(newGameActor);
	m_world->m_actorList.push_back(newGameActor);
	newGameActor->SendMsg(NULL, AM_INITSTANDENTRY, int(&s), 1, 0);
	if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
		DWORD gdid = (p->accessory2 << 16) | p->accessory3;
		int emblemVer = p->shield;
		if (gdid > 0)
			newGameActor->SetGuildInfo(gdid, emblemVer);
	}
}

void	CGameMode :: Zc_Refuse_Quit(const char *buf)
{
}

void	CGameMode :: Zc_Notify_Move (const char *buf)
{
	PACKET_ZC_NOTIFY_MOVE *		p = (PACKET_ZC_NOTIFY_MOVE *)buf;
	MOVESTRUCT		ms;
	ms.startTime = p->moveStartTime;
	DecodeSrcDst(p->MoveData, ms.sx, ms.sy, ms.dx, ms.dy, ms.cellX, ms.cellY);
	CGameActor *	theActor = GetWorld()->GetGameActorByAID(p->GID);
	g_session.UpdateServerTime(ms.startTime);

	if (theActor)
		theActor->SendMsg(NULL, AM_MOVE, (int)(&ms), 0, 0);
}


void CGameMode::Zc_Notify_Playermove(char const *buf)
{
	PACKET_ZC_NOTIFY_PLAYERMOVE *p = (PACKET_ZC_NOTIFY_PLAYERMOVE *)buf;
	MOVESTRUCT ms;

	ms.startTime = p->moveStartTime;
	DecodeSrcDst(p->MoveData, ms.sx, ms.sy, ms.dx, ms.dy, ms.cellX, ms.cellY);

	g_session.UpdateServerTime(ms.startTime);
	m_world->m_player->SendMsg(NULL, AM_MOVE, int(&ms), 0, 0);

	char PASS = 0;
	for (int a = 0; a < stateId.size(); ++a) {
		if (stateId[a] == EFST_RUN) {
			g_checkBOT = 0;
			PASS = 1;
			break;
		}
	}

	if (PASS == 0) {
		if (g_BOT_x != ms.dx || g_BOT_y != ms.dy)
			g_checkBOT++;

		g_BOT_x = ms.dx;
		g_BOT_y = ms.dy;

		if (g_checkBOT >= 10)
			g_checkBOT = 0;
	}
}


void CGameMode::Zc_Notify_Act(unsigned long GID, unsigned long targetGID, unsigned long startTime, int attackMT, int attackedMT, int damage, short count, unsigned char action, int leftDamage)
{
	if (damage == -30000)
		return;

	ACTSTRUCT as;
	as.damage = damage;
	as.leftDamage = leftDamage;
	as.action = action;
	as.targetGID = targetGID;
	as.startTime = startTime;
	as.count = count;
	as.tCellX = -1;
	as.tCellY = -1;
	as.attackMT = attackMT;
	as.attackedMT = attackedMT;

	if (as.attackMT == 0.0f)
		as.attackMT = 288 * 5.0f;
	if (as.attackedMT == 0.0f && !g_session.IsAttack(as.action))
		as.attackedMT = 288 * 5.0f;

	if (GID == g_session.GetAid())
	{
		CPlayer *player = m_world->m_player;

		if (player->m_stateId == ST_STAND && player->GetTargetGid() != targetGID)
		{
			if (as.action == ACTION_ITEMPICKUP || as.action == ACTION_SIT || as.action == ACTION_STAND)
			{
				player->SendMsg(NULL, AM_ACTION, int(&as), 0, 0);
				return;
			}

			CGameActor *pet = m_world->GetGameActorByAID(g_session.m_petGID);
			if (pet)
			{
				int h = g_session.GetPetHungryState(g_session.m_petHungry);
				int f = g_session.GetPetFriendlyState(g_session.m_petFriendly);
				int emotion = g_session.m_petEmotionTable[h][f][PM_HUNTING];
				if (rand() % 100 < 25)
				{
					int talk = g_session.GetPetTalkNumber(g_session.m_petJob, PM_HUNTING, h);
					SendMsg(MM_PET_ACT, talk);
					SendMsg(MM_PET_ACT, emotion);
				}
			}
		}

		player->SendMsg(NULL, AM_ACTION, int(&as), 0, 0);
	}
	else
	{
		CGameActor *theActor = m_world->GetGameActorByAID(GID);
		if (theActor)
		{
			if (theActor->m_stateId == ST_STAND && theActor->GetTargetGid() != targetGID)
			{
				if (as.action == ACTION_ITEMPICKUP || as.action == ACTION_SIT || as.action == ACTION_STAND)
				{
					theActor->SendMsg(NULL, AM_ACTION, int(&as), 0, 0);
					return;
				}

				MonsterTalk(theActor, theActor->GetJob(), MAT_ATTACK, 0, GID);
			}
			theActor->SendMsg(NULL, AM_ACTION, int(&as), 0, 0);
		}
	}
}

void	CGameMode :: Zc_Notify_Act_Position(const char *buf)
{
	PACKET_ZC_NOTIFY_ACT_POSITION *p = (PACKET_ZC_NOTIFY_ACT_POSITION *)buf;

	if (p->damage == (short)0x8ad0)
		return;

	ACTSTRUCT as;
	as.damage     = (int)p->damage;
	as.leftDamage = 0;
	as.action     = p->action;
	as.targetGID  = p->tgtGID;
	as.startTime  = p->startTime;
	as.count      = (int)p->count;
	as.tCellX     = (int)p->tCellX;
	as.tCellY     = (int)p->tCellY;
	as.attackMT   = 432;
	as.attackedMT = 288;

	if (p->srcGID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_ACTION, int(&as), 0, 0);
		return;
	}

	CGameActor *actor = m_world->GetGameActorByAID(p->srcGID);
	if (actor)
		actor->SendMsg(NULL, AM_ACTION, int(&as), 0, 0);
}

void	CGameMode :: Zc_Notify_Chat (const char *buf)
{
	PACKET_ZC_NOTIFY_CHAT *	p = (PACKET_ZC_NOTIFY_CHAT *)buf;
	if (g_session.IsExAid(p->GID))
	{
		return;
	}

	char	chatMsg[256];
	int		msgLen = p->PacketLength - sizeof(PACKET_ZC_NOTIFY_CHAT);
	memcpy(chatMsg, buf + sizeof(PACKET_ZC_NOTIFY_CHAT), msgLen);

	{
		if (g_windowMgr.GetChatRoomWnd())
		{
			g_windowMgr.SendMsg(UIM_PUSH_INTO_CHATROOM, int(chatMsg), 0x222222, 0, 0);
		}
		else
		{
			if (IsNameYellow(p->GID))
			{
				if (g_serviceType == (ServiceType)5)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xffff00, 1, 0);
				else if (g_serviceType == (ServiceType)10)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0x1d8dff, 1, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xffff, 1, 0);
			}
			else if (IsNameSpecific(p->GID))
			{
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), IsNameSpecific(p->GID), 0, 0);
			}
			else
			{
				if (g_session.m_isNoTalkMsg == TRUE)
				{
				}
				else if (g_session.m_isNoTalkMsg2 == TRUE)
				{
				}
				else
				{
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xffffff, 1, 0);
				}
			}

			CGameActor *	theActor = GetWorld()->GetGameActorByAID(p->GID);
			if (theActor)
			{
				theActor->SendMsg(NULL, AM_CHAT, int(chatMsg));
			}
		}
		if (g_session.m_isSaveChat)
		{
			WriteChat(chatMsg);
		}
	}
}

void	CGameMode :: Zc_Notify_Playerchat (const char *buf)
{
	PACKET_ZC_NOTIFY_PLAYERCHAT *	p = (PACKET_ZC_NOTIFY_PLAYERCHAT *)buf;
	char	chatMsg[256];
	int		msgLen = p->PacketLength - sizeof(PACKET_ZC_NOTIFY_PLAYERCHAT);
	memcpy(chatMsg, buf + sizeof(PACKET_ZC_NOTIFY_PLAYERCHAT), msgLen);

	if (g_windowMgr.GetChatRoomWnd())
	{
		g_windowMgr.SendMsg(UIM_PUSH_INTO_CHATROOM, int(chatMsg), 0x7800, 0, 0);
	}
	else
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xff00, 1, 0);
		m_world->m_player->SendMsg(NULL, AM_CHAT, int(chatMsg), 0, 0);
	}

	if (g_session.m_isSaveChat)
	{
		WriteChat(chatMsg);
	}
}

void	CGameMode :: Zc_Notify_Time (const char *buf)
{
	if (g_pingLog)
	{
		FILE *file = fopen("PingLog.txt", "at");
		if (file)
		{
			fprintf(file, "%10.2f\t%10d\n",
				(g_pingLogList.front() - g_pingLogStart) / 1000.0f,
				timeGetTime() - g_pingLogList.front());
			fclose(file);
		}
		g_pingLogList.pop_front();
	}

	PACKET_ZC_NOTIFY_TIME *		p = (PACKET_ZC_NOTIFY_TIME *)buf;
	if (m_numNotifyTime == 0)
		g_session.SetServerTime(p->time);
	else
		g_session.UpdateServerTime(p->time);

	m_receiveSyneRequestTime = 1;
	++m_numNotifyTime;
	g_session.RecalcAveragePingTime(timeGetTime() - m_syncRequestTime);
}

void CGameMode::Zc_Npcack_Mapmove(char const *buf)
{
	PACKET_ZC_NPCACK_MAPMOVE *p = (PACKET_ZC_NPCACK_MAPMOVE *)buf;
	g_session.m_posX = p->xPos;
	g_session.m_posY = p->yPos;

	char mapName[17];
	memcpy(mapName, p->mapName, 16);
	mapName[16] = 0;
	Trace("mapmove mapname == %s", mapName);

	std::string curMap = mapName;
	curMap.resize(curMap.size() - 4);
	if (strcmp(curMap.c_str(), g_session.m_curMap))
	{
		strcpy(g_session.m_curMap, curMap.c_str());
		curMap += ".rsw";
		Trace("curMap == %s", curMap.c_str());
		g_modeMgr.Switch(MT_GAME, curMap.c_str());
		UIWaitWnd *waitWindow = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
		waitWindow->SetMsg(MsgStr(MSI_PLEASE_BE_PATIENT), 17, 1);
	}
	else
	{
		RemoveGameActorAllAndMake();
		Initialize();
		RunFadeOut(FALSE);
	}
}

void	CGameMode :: Zc_Npcack_Enable(const char *buf)
{
	m_noMove = false;
}


void CGameMode::Zc_Accept_Enter(char const *buf)
{
	const unsigned char *p = (const unsigned char *)buf;
	g_session.SetServerTime(*(DWORD *)(buf + 2));

	int dir = p[8] & 0xf;
	int y = ((p[7] & 0x3f) << 4) | (p[8] >> 4);
	int x = (p[6] << 2) | (p[7] >> 6);
	g_session.SetPlayerPosDir(x, y, dir);

	char curMap[35];
	sprintf(curMap, "%s.rsw", g_session.m_curMap);
	g_modeMgr.Switch(MT_GAME, curMap);

	CRagConnection::instanceR().SetBlock(false);

	if (g_pingLog) {
		FILE *file = fopen("PingLog.txt", "at");
		if (file) {
			time_t currentTime;
			time(&currentTime);
			fprintf(file, "\n");
			fprintf(file, "server:%s\n", g_session.m_selectedServerName.c_str());
			fprintf(file, "ip:%s\n", g_zoneServerAddr.ip);
			fprintf(file, "map:%s.gat\n", g_session.m_curMap);
			fprintf(file, "time:%s\n\n", ctime(&currentTime));
			fclose(file);
		}
		g_pingLogList.clear();
		g_pingLogStart = 0;
	}
}


void CGameMode::Zc_Accept_Enter2(char const *buf)
{
	const unsigned char *p = (const unsigned char *)buf;
	g_session.SetServerTime(*(DWORD *)(buf + 2));

	int dir = p[8] & 0xf;
	int y = ((p[7] & 0x3f) << 4) | (p[8] >> 4);
	int x = (p[6] << 2) | (p[7] >> 6);
	g_session.SetPlayerPosDir(x, y, dir);

	g_session.m_charfont = p[11];

	char curMap[35];
	sprintf(curMap, "%s.rsw", g_session.m_curMap);
	g_modeMgr.Switch(MT_GAME, curMap);

	CRagConnection::instanceR().SetBlock(false);

	if (g_pingLog) {
		FILE *file = fopen("PingLog.txt", "at");
		if (file) {
			time_t currentTime;
			time(&currentTime);
			fprintf(file, "\n");
			fprintf(file, "server:%s\n", g_session.m_selectedServerName.c_str());
			fprintf(file, "ip:%s\n", g_zoneServerAddr.ip);
			fprintf(file, "map:%s.gat\n", g_session.m_curMap);
			fprintf(file, "time:%s\n\n", ctime(&currentTime));
			fclose(file);
		}
		g_pingLogList.clear();
		g_pingLogStart = 0;
	}
}


void CGameMode::Zc_Npcack_Servermove(char const *buf)
{
	PACKET_ZC_NPCACK_SERVERMOVE *packet = (PACKET_ZC_NPCACK_SERVERMOVE *)buf;

	g_session.InitWhenServerMove();

	char mapName[17];
	memcpy(mapName, packet->mapName, 16);
	mapName[16] = 0;

	std::string curMap = mapName;
	curMap.resize(curMap.size() - 4);
	strcpy(g_session.m_curMap, curMap.c_str());

	g_session.m_posX = packet->xPos;
	g_session.m_posY = packet->yPos;

	in_addr addr;
	addr.S_un.S_addr = packet->addr.ip;
	strcpy(g_zoneServerAddr.ip, inet_ntoa(addr));
	g_zoneServerAddr.port = packet->addr.port;

	CRagConnection::instanceR().Disconnect();
	stateId.erase(stateId.begin(), stateId.end());

	m_noMove = TRUE;
	m_noMoveStartTick = timeGetTime();
	m_world->m_player->SendMsg(NULL, AM_FREEZE, 0, 0, 0);
	m_isConnected = CRagConnection::instanceR().Connect(&g_zoneServerAddr);
	if (m_isConnected)
	{
		CRagConnection::instanceR().SetDelay(1000);

		PACKET_CZ_ENTER p;
		p.PacketType = 0x9b;
		SetPaddingValue(p.pad, 2);
		SetPaddingValue(p.pad2, 1);
		SetPaddingValue(p.pad3, 4);
		p.clientTime = timeGetTime();
		p.AID = g_session.GetAid();
		p.GID = g_session.GetGid();
		p.AuthCode = g_session.m_authCode;
		p.Sex = g_session.GetSex();

		CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(p.PacketType), (const char *)&p);
		CRagConnection::instanceR().SetBlock(true);

		UIWaitWnd *waitWindow = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
		waitWindow->SetMsg(MsgStr(MSI_WAITING_RESPONSE_FROM_SERVER), 16, 1);
	}
	else
	{
		CRagConnection::instanceR().Disconnect();
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED), ME_OKCANCEL, 1, 1, 0) != ID_MODAL_PASSED)
			SendMsg(MM_QUIT_TO_IDENTRY, 0, 0, 0);
	}
}


void CGameMode::Zc_Ack_Reqname(char const *buf)
{
	PACKET_ZC_ACK_REQNAME *p = (PACKET_ZC_ACK_REQNAME *)buf;
	m_actorNameList[p->AID].cName.assign(p->CName, NAME_LENGTH);
	RemoveActorNameRequestTimer(p->AID);

	if (p->AID == g_session.m_petGID) {
		sprintf(g_session.m_petName, p->CName);
		if (g_windowMgr.GetPetInfoWnd()) {
			g_windowMgr.GetPetInfoWnd()->m_newName->SetText(p->CName);
		}
	}
}

void	CGameMode :: Zc_Ack_Reqname_bygid(const char *buf)
{
	PACKET_ZC_ACK_REQNAME_BYGID *	p = (PACKET_ZC_ACK_REQNAME_BYGID *) buf;
	m_actorNameListByGID[p->GID].cName.assign(p->CName, NAME_LENGTH);
	RemoveActorNameByGIDRequestTimer(p->GID);
	CheckGuildMemberStatusCache(p->GID, m_actorNameListByGID[p->GID]);
}


void CGameMode::Zc_Ack_Reqnameall(char const *buf)
{
	PACKET_ZC_ACK_REQNAMEALL *p = (PACKET_ZC_ACK_REQNAMEALL *)buf;
	m_actorNameList[p->AID].cName.assign(p->CName, 24);
	m_actorNameList[p->AID].pName.assign(p->PName, 24);
	m_actorNameList[p->AID].gName.assign(p->GName, 24);
	m_actorNameList[p->AID].rName.assign(p->RName, 24);
	RemoveActorNameRequestTimer(p->AID);
}

#ifndef MIN
#define MIN( x, y ) (x > y ? y: x )
#endif

void CGameMode::Zc_Broadcast(char const *buf)
{
	PACKET_ZC_BROADCAST *p = (PACKET_ZC_BROADCAST *)buf;

	bool isSiegeMsg = false;
	char talkerName[NAME_LENGTH + 1] = { 0 };
	char chatMsg[1024];
	char temp[1024];
	char temp2[1024];
	char temp3[5] = { 0 };

	int msgLen = min(sizeof(chatMsg) - 1, p->PacketLength - sizeof(PACKET_ZC_BROADCAST));
	memcpy(chatMsg, buf + sizeof(PACKET_ZC_BROADCAST), msgLen);
	chatMsg[msgLen] = 0;
	int len = strlen(chatMsg);

	unsigned long chatColor = RGB(255, 255, 0);
	std::string wMsg = chatMsg;
	memcpy(temp3, chatMsg, (strlen(chatMsg) > 4) ? 4 : strlen(chatMsg));
	temp3[4] = 0;
	strlwr(temp3);

	int subStrSize1 = wMsg.find('#');
	int subStrSize2 = wMsg.find(':');
	wMsg.find("blue");

	if (!strcmp(temp3, "tool"))
	{
		memcpy(temp, chatMsg + 4, 6);
		temp[6] = 0;

		unsigned long value;
		sscanf(temp, "%lx", &value);

		unsigned long colorRef = RGB(GetBValue(value), GetGValue(value), GetRValue(value));

		memcpy(temp2, chatMsg + 10, len - 10);
		temp2[len - 10] = 0;
		wMsg = temp2;
		chatColor = colorRef;
	}
	else if (!strcmp(temp3, "blue") || !strcmp(temp3, "\xba\xed\xb7\xe7"))
	{
		memcpy(temp, chatMsg + 4, len - 4);
		temp[len - 4] = 0;
		wMsg = temp;
		chatColor = RGB(0, 255, 255);
	}
	else if (!strcmp(temp3, "micc"))
	{
		memcpy(talkerName, chatMsg + 4, NAME_LENGTH);
		talkerName[NAME_LENGTH] = 0;

		memcpy(temp, chatMsg + 4 + NAME_LENGTH, 6);
		temp[6] = 0;

		unsigned long value;
		sscanf(temp, "%lx", &value);

		unsigned long colorRef = RGB(GetBValue(value), GetGValue(value), GetRValue(value));

		memcpy(temp2, chatMsg + 4 + NAME_LENGTH + 6, msgLen - 4 - NAME_LENGTH - 6);
		temp2[msgLen - 4 - NAME_LENGTH - 6] = 0;
		wMsg = temp2;
		chatColor = colorRef;
	}
	else if (subStrSize1 > 0 && subStrSize1 < len)
	{
		memcpy(temp, chatMsg, subStrSize1);
		temp[subStrSize1] = 0;
		wMsg = temp;

		if (subStrSize2 > 0 && subStrSize2 < len)
		{
			memcpy(temp2, chatMsg + subStrSize2, len - subStrSize2 + 1);
			wMsg += temp2;
		}
	}
	else if (!strcmp(temp3, "ssss"))
	{
		memcpy(temp2, chatMsg + 4, len - 4);
		temp2[len - 4] = 0;
		wMsg = temp2;
		isSiegeMsg = true;
	}

	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	if (isSiegeMsg)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(wMsg.c_str()), int(chatColor), 13, int(talkerName));
	}
	else
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(wMsg.c_str()), int(chatColor), 0, int(talkerName));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(wMsg.c_str()), int(chatColor), 0, int(talkerName));
	}

	m_broadCastTick = timeGetTime();
	if (m_broadcastBalloon == NULL)
	{
		m_broadcastBalloon = new UITransBalloonText;
		m_broadcastBalloon->Create(8, 8);
		g_windowMgr.AddWindowFront(m_broadcastBalloon);
	}
	m_broadcastBalloon->SetText(wMsg.c_str(), 72);
	m_broadcastBalloon->Move((g_renderer->m_width - 640) / 2 - m_broadcastBalloon->GetWidth() / 2 + 320, 51);
	m_broadcastBalloon->SetFntColor(chatColor, 0);
}

void CGameMode::Zc_Whisper(char const *buf)
{
	PACKET_ZC_WHISPER *p = (PACKET_ZC_WHISPER *)buf;

	char cName[NAME_LENGTH + 1];
	memcpy(cName, p->sender, NAME_LENGTH);
	cName[NAME_LENGTH] = 0;

	if (g_session.IsExName(cName))
		return;

	char chatMsg[256];
	int msgLen = p->PacketLength - sizeof(PACKET_ZC_WHISPER);
	memcpy(chatMsg, buf + sizeof(PACKET_ZC_WHISPER), msgLen);
	chatMsg[255] = 0;

	{
		std::string imMsg = cName;
		imMsg.append(" : ");
		imMsg.append(chatMsg);

		if (g_windowMgr.AddWhisperChatToWhisperWnd(cName, imMsg.c_str(), 0x222222) != 0)
		{
			if (g_session.m_onAutoMSG != 0)
			{
				char comp[NAME_LENGTH];
				char pdest[NAME_LENGTH];
				sprintf(comp, MsgStr(MSI_AUTO_MSG2));
				char len = strlen(comp);
				memcpy(pdest, chatMsg, len);
				pdest[len] = 0;
				if (strcmp(pdest, comp))
				{
					std::string tmpMsg = MsgStr(MSI_AUTO_MSG2);
					tmpMsg.append(g_session.m_strAutoMSG.c_str());
					g_windowMgr.SetChatMsg(tmpMsg.c_str());
					g_modeMgr.GetCurMode()->SendMsg(MM_WHISPERMSG, int(cName), 0, 0);
				}
			}
			return;
		}

		std::string wMsg;
		if (g_windowMgr.IsFriendName(cName))
		{
			wMsg = "[ Friend ";
			wMsg.append(cName);
			wMsg = wMsg + " ] : " + chatMsg;
		}
		else if (g_session.IsGuildMember(cName))
		{
			wMsg = "[ Member ";
			wMsg.append(cName);
			wMsg = wMsg + " ] : " + chatMsg;
		}
		else
		{
			if (g_session.IsCheatName(cName))
			{
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ID_WARNING)), 0xffff, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_ID_WARNING)), 0xffff, 0, 0);
			}
			else if (g_session.IsCheatGuildName(cName))
			{
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ID_WARNING_GUILD)), 0xffff, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_ID_WARNING_GUILD)), 0xffff, 0, 0);
			}

			wMsg = "( From ";
			wMsg.append(cName);
			wMsg = wMsg + " ) : " + chatMsg;
		}

		if (strcmp(cName, g_session.GetCharName()))
		{
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(wMsg.c_str()), 0xffff, 2, int(cName));
			g_windowMgr.AddWhisperCharName(cName);

			if (g_session.m_isSaveChat)
				WriteChat((char *)wMsg.c_str());
		}
	}

	if (g_session.m_onAutoMSG != 0)
	{
		char comp[NAME_LENGTH];
		char pdest[NAME_LENGTH];
		sprintf(comp, MsgStr(MSI_AUTO_MSG2));
		char len = strlen(comp);
		memcpy(pdest, chatMsg, len);
		pdest[len] = 0;
		if (strcmp(pdest, comp))
		{
			std::string tmpMsg = MsgStr(MSI_AUTO_MSG2);
			tmpMsg.append(g_session.m_strAutoMSG.c_str());
			g_windowMgr.SetChatMsg(tmpMsg.c_str());
			g_modeMgr.GetCurMode()->SendMsg(MM_WHISPERMSG, int(cName), 0, 0);
		}
	}
}


void CGameMode::Zc_Ack_Whisper(char const *buf)
{
	PACKET_ZC_ACK_WHISPER *p = (PACKET_ZC_ACK_WHISPER *)buf;

	switch (p->result)
	{
	case WHISPER_SEND_SUCCEED:
	{
		std::string imMsg = g_session.GetCharName();
		imMsg += " : ";
		imMsg += m_lastWhisper;

		if (g_windowMgr.AddWhisperChatToWhisperWnd(m_lastWhisperName.c_str(), imMsg.c_str(), 0x7800) == 0)
		{
			std::string msg;
			if (g_windowMgr.IsFriendName(m_lastWhisperName.c_str()))
			{
				msg = std::string("[ To " + m_lastWhisperName + " ] : " + m_lastWhisper).c_str();
			}
			else if (g_session.IsGuildMember(m_lastWhisperName.c_str()))
			{
				msg = std::string("[ To " + m_lastWhisperName + " ] : " + m_lastWhisper).c_str();
			}
			else
			{
				msg = std::string("( To " + m_lastWhisperName + " ) : " + m_lastWhisper).c_str();
			}

			if (g_MessageHi > 0)
			{
				--g_MessageHi;
			}
			else
			{
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg.c_str()), 0xffff, 2, int(m_lastWhisperName.c_str()));
			}

			if (g_session.m_isSaveChat)
				WriteChat((char *)msg.c_str());
		}
		break;
	}
	case WHISPER_SEND_FAILED_PC_ABSENCE:
	{
		std::string msg = std::string("(" + m_lastWhisperName + ")" + MsgStr(MSI_NOT_EXIST_CHARACTER)).c_str();
		if (g_MessageHi > 0)
		{
			--g_MessageHi;
		}
		else
		{
			if (g_windowMgr.AddWhisperChatToWhisperWnd(m_lastWhisperName.c_str(), msg.c_str(), 0x7800) == 0)
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg.c_str()), 0xffff, 2, 0);
		}
		break;
	}
	case WHISPER_SEND_FAILED_PC_REFUSAL:
	{
		std::string msg = std::string("(" + m_lastWhisperName + ")" + MsgStr(MSI_NO_RECEIVE_MODE)).c_str();
		if (g_windowMgr.AddWhisperChatToWhisperWnd(m_lastWhisperName.c_str(), msg.c_str(), 0x7800) == 0)
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg.c_str()), 0xffff, 2, 0);
		break;
	}
	case WHISPER_SEND_FAILED_PC_REFUSAL_ALL:
	{
		std::string msg = std::string("(" + m_lastWhisperName + ")" + MsgStr(MSI_NO_RECEIVE_MODE_FOR_ALL)).c_str();
		if (g_windowMgr.AddWhisperChatToWhisperWnd(m_lastWhisperName.c_str(), msg.c_str(), 0x7800) == 0)
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg.c_str()), 0xffff, 2, 0);
		break;
	}
	default:
	{
		std::string msg = std::string("(" + m_lastWhisperName + ")" + MsgStr(MSI_NOT_EXIST_CHARACTER)).c_str();
		if (g_windowMgr.AddWhisperChatToWhisperWnd(m_lastWhisperName.c_str(), msg.c_str(), 0x7800) == 0)
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg.c_str()), 0xffff, 2, 0);

		if (g_session.m_isSaveChat)
			WriteChat((char *)msg.c_str());
		break;
	}
	}
}



void	CGameMode :: Zc_Change_Direction(const char *buf)
{
	PACKET_ZC_CHANGE_DIRECTION *	p = (PACKET_ZC_CHANGE_DIRECTION *)buf;

	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->m_roty = DirToDeg(p->dir);
		m_world->m_player->SendMsg(NULL, AM_SET_HEAD_DIR, p->headDir, 0, 0);
	}
	else
	{
		CGameActor *	theActor = GetWorld()->GetGameActorByAID(p->AID);
		if (theActor)
		{
			theActor->m_roty = DirToDeg(p->dir);
			theActor->SendMsg(NULL, AM_SET_HEAD_DIR, p->headDir, 0, 0);
		}
	}
}

void	CGameMode :: Zc_Ack_Disconnect_Character(const char *buf)
{
	PACKET_ZC_ACK_DISCONNECT_CHARACTER *	p = (PACKET_ZC_ACK_DISCONNECT_CHARACTER *)buf;
	char	msg[80];
	if (p->result)
		sprintf(msg, MsgStr(MSI_SUCCESS_DISCONNECT_CHARACTER));
	else
		sprintf(msg, MsgStr(MSI_FAIL_DISCONNECT_CHARACTER));
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(255, 50, 0)), 0, 0);
}


void CGameMode::Zc_Notify_Moveentry(char const *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY *p = (PACKET_ZC_NOTIFY_MOVEENTRY *)buf;
	MOVEENTRYSTRUCT ms;
	ms.GID = p->GID;
	ms.job = p->job;
	ms.moveStartTime = p->moveStartTime;
	ms.sex = p->sex;
	ms.speed = p->speed;
	ms.head = p->head;
	ms.weapon = p->weapon;
	ms.accessory = p->accessory;
	ms.healthState = p->healthState;
	ms.bodyState = p->bodyState;
	ms.effectState = p->effectState;
	ms.isPKModeON = p->isPKModeON;
	ms.shield = p->shield;
	ms.accessory2 = p->accessory2;
	ms.accessory3 = p->accessory3;
	ms.headpalette = p->headpalette;
	ms.bodypalette = p->bodypalette;
	ms.headDir = p->headDir;
	ms.guildId = p->GUID;
	ms.guildEmblemVersion = p->GEmblemVer;
	ms.honor = p->honor;
	ms.virtue = p->virtue;
	DecodeSrcDst(p->MoveData, ms.sx, ms.sy, ms.dx, ms.dy, ms.cellX, ms.cellY);

	g_session.UpdateServerTime(ms.moveStartTime);

	CGameActor *newGameActor = GetWorld()->GetGameActorByAID(p->GID);
	if (newGameActor) {
		newGameActor->SendMsg(NULL, AM_MOVEENTRY, (int)(&ms));

		if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
			DWORD gdid = (DWORD)(p->accessory2 << 16);
			gdid |= p->accessory3;
			if (gdid > 0) {
				DWORD emblemVer = p->shield;
				newGameActor->SetGuildInfo(gdid, emblemVer);
			}
		}

		if (ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0 && newGameActor->GetGdid() == 0)
				newGameActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			ms.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITMOVEENTRY, (int)(&ms));

		if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
			DWORD gdid = (DWORD)(p->accessory2 << 16);
			gdid |= p->accessory3;
			DWORD emblemVer = p->shield;
			if (gdid > 0 && newGameActor->GetGdid() == 0) {
				newGameActor->SetGuildInfo(gdid, emblemVer);
			}
		}

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}

void CGameMode::Zc_Notify_Vanish(char const *buf)
{
	PACKET_ZC_NOTIFY_VANISH *p = (PACKET_ZC_NOTIFY_VANISH *)buf;

	{
		std::map<DWORD, NamePair>::iterator it = m_actorNameList.find(p->GID);
		if (it != m_actorNameList.end())
			m_actorNameList.erase(it);
	}

	switch (p->type)
	{
	case VT_OUTOFSIGHT:
	{
		CGameActor *theActor = m_world->GetGameActorByAID(p->GID);
		if (theActor)
		{
			theActor->SendMsg(NULL, AM_OUTOFSIGHT, 0, 0, 0);
			SendMsg(MM_DELETEREGISTERPOS, p->GID, 0, 0);
		}

		if (GetWorld()->RemoveGameActor(p->GID) == true)
			SendMsg(MM_DELETEREGISTERPOS, p->GID, 0, 0);

		UIFrameWnd *messengerGroupWnd = (UIFrameWnd *)g_windowMgr.GetMessengerGroupWnd();
		if (messengerGroupWnd)
			messengerGroupWnd->SendMsg(NULL, UM_COMMAND, 0x22, p->GID, 0, 0);
		break;
	}
	case VT_DEAD:
	{
		CGameActor *theActor = m_world->GetGameActorByAID(p->GID);
		if (theActor)
		{
			if (p->GID == m_world->m_player->m_proceedTargetGid)
				m_world->m_player->m_proceedTargetGid = 0;

			theActor->SendMsg(NULL, AM_WILL_BE_DEAD, 0, 0, 0);
			if ((theActor->GetJob() > JT_MON_BEGIN && theActor->GetJob() < JT_MONSTER_LAST) ||
				theActor->m_objectType == NPC_MERSOL_TYPE ||
				g_session.IsMerOrHomun(theActor->GetJob()))
			{
				SendMsg(MM_DELETEREGISTERPOS, p->GID, 0, 0);
			}
		}

		if (p->GID == g_session.GetAid())
		{
			m_world->m_player->SendMsg(NULL, AM_CANCELATTACK, 0, 0, 0);
			m_world->m_player->SendMsg(NULL, AM_WILL_BE_DEAD, 0, 0, 0);

			m_isOnQuest = true;
			m_isPlayerDead = true;
			g_windowMgr.DeleteWindow(WID_QUITWND);
			g_windowMgr.DeleteWindow(WID_SAYDIALOGWND);
			g_windowMgr.DeleteWindow(WID_CHOOSEWND);
			g_windowMgr.DeleteWindow(WID_CHOOSESELLBUYWND);
			g_windowMgr.DeleteWindow(WID_ITEMSHOPWND);
			g_windowMgr.DeleteWindow(WID_ITEMPURCHASEWND);
			g_windowMgr.DeleteWindow(WID_ITEMSELLWND);

			CGameActor *pet = m_world->GetGameActorByAID(g_session.m_petGID);
			if (pet)
			{
				int h = g_session.GetPetHungryState(g_session.m_petHungry);
				int f = g_session.GetPetFriendlyState(g_session.m_petFriendly);
				int emotion = g_session.m_petEmotionTable[h][f][PM_DEAD];
				int talk = g_session.GetPetTalkNumber(g_session.m_petJob, PM_DEAD, h);
				SendMsg(MM_PET_ACT, talk, 0, 0);
				SendMsg(MM_PET_ACT, emotion, 0, 0);
			}

			if (g_serviceType != ServiceTaiwan)
			{
				if (HaveSiegfriedItem())
					g_windowMgr.MakeWindow(WID_REBIRTHWND);
				else
					g_windowMgr.MakeWindow(WID_RESTARTWND);
			}
			else
			{
				g_windowMgr.ErrorMsg(MsgStr(MSI_PRESS_ESC_TO_RESTART), ME_OK, 1, 0, 0);
			}
		}
		break;
	}
	case VT_EXIT:
	{
		CGameActor *theActor = m_world->GetGameActorByAID(p->GID);
		if (theActor)
			PlaceEffect(&theActor->m_pos, EF_EXIT2, 10 * 60, 0);

		if (GetWorld()->RemoveGameActor(p->GID) == true)
			SendMsg(MM_DELETEREGISTERPOS, p->GID, 0, 0);
		break;
	}
	case VT_TELEPORT:
	{
		CGameActor *theActor = m_world->GetGameActorByAID(p->GID);
		if (theActor)
		{
			if (!IsEffectStateSpecialHiding(theActor->GetEffectState()))
				PlaceEffect(&theActor->m_pos, EF_TELEPORTATION2, 5 * 60, 0);
		}

		if (GetWorld()->RemoveGameActor(p->GID) == true)
			SendMsg(MM_DELETEREGISTERPOS, p->GID, 0, 0);

		UIFrameWnd *messengerGroupWnd = (UIFrameWnd *)g_windowMgr.GetMessengerGroupWnd();
		if (messengerGroupWnd)
			messengerGroupWnd->SendMsg(NULL, UM_COMMAND, 0x22, p->GID, 0, 0);
		break;
	}
	default:
		GetWorld()->GetGameActorByAID(p->GID);
		if (GetWorld()->RemoveGameActor(p->GID) == true)
			SendMsg(MM_DELETEREGISTERPOS, p->GID, 0, 0);
		break;
	}
}


void CGameMode::Sc_Notify_Ban(char const *buf)
{

	PACKET_SC_NOTIFY_BAN *p = (PACKET_SC_NOTIFY_BAN *)buf;
	int errorCode = p->ErrorCode;
	CRagConnection::instanceR().Disconnect();

	if (g_session.IsKorean() && g_session.GetAid() == 100025) {
		switch (errorCode) {
		case BAN_UNFAIR:
			g_windowMgr.ErrorMsg("BAN_UNFAIR", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_SERVER_SHUTDOWN:
			g_windowMgr.ErrorMsg("BAN_SERVER_SHUTDOWN", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_ANOTHER_LOGIN:
			g_windowMgr.ErrorMsg("BAN_ANOTHER_LOGIN", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_DETECTED_SPEEDHACK:
			g_windowMgr.ErrorMsg("BAN_DETECTED_SPEEDHACK", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_PC_OVERFLOW:
			g_windowMgr.ErrorMsg("BAN_PC_OVERFLOW", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_UNDER_AGE:
			g_windowMgr.ErrorMsg("BAN_UNDER_AGE", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_IP_OVERFLOW:
			g_windowMgr.ErrorMsg("BAN_IP_OVERFLOW", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_PAY_OUT:
			g_windowMgr.ErrorMsg("BAN_PAY_OUT", ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_OP_FORCE:
			if (g_serviceType == ServiceTaiwan)
				g_windowMgr.ErrorMsg("BAN_OP_FORCE", ME_OK, TRUE, TRUE, 0);
			break;
		default:
			g_windowMgr.ErrorMsg("No matching error code.", ME_OK, TRUE, TRUE, 0);
			break;
		}
	} else {
		switch (errorCode) {
		case BAN_UNFAIR:
			g_windowMgr.ErrorMsg(MsgStr(MSI_BANNED), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_SERVER_SHUTDOWN:
			g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_OFF), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_ANOTHER_LOGIN:
			g_windowMgr.ErrorMsg(MsgStr(MSI_DOUBLE_LOGIN_PROHIBITED), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_DETECTED_SPEEDHACK:
			g_windowMgr.ErrorMsg(MsgStr(MSI_SPEEDHACK), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_PC_OVERFLOW:
			g_windowMgr.ErrorMsg(MsgStr(MSI_PC_OVERFLOW), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_UNDER_AGE:
			g_windowMgr.ErrorMsg(MsgStr(MSI_UNDER_AGE), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_IP_OVERFLOW:
			g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_IP_OVERFLOW), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_PAY_OUT:
			g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PAY_OUT), ME_OK, TRUE, TRUE, 0);
			break;
		case BAN_OP_FORCE:
			if (g_serviceType == ServiceTaiwan)
				g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_OP_FORCE), ME_OK, TRUE, TRUE, 0);
			break;
		default:
			g_windowMgr.ErrorMsg(MsgStr(MSI_BANNED), ME_OK, TRUE, TRUE, 0);
			break;
		}
	}

	SendMsg(MM_QUIT_TO_IDENTRY);
}

void	CGameMode :: Zc_Accept_Quit(const char *buf)
{
}


void CGameMode::Zc_Item_Entry(char const *buf)
{
	PACKET_ZC_ITEM_ENTRY *p = (PACKET_ZC_ITEM_ENTRY *)buf;

	ITEMSTANDENTRYSTRUCT s;
	memset(&s, 0, sizeof(s));
	s.count = p->count;
	s.ITAID = p->ITAID;
	s.x = p->xPos;
	s.y = p->yPos;
	s.subX = p->subX;
	s.subY = p->subY;
	itoa(p->ITID, s.name, 10);
	s.isJumpEntry = false;
	s.isIdentified = p->IsIdentified;

	CItem *newItem = m_world->GetItemByAID(p->ITAID);
	if (newItem == NULL)
	{
		newItem = new CItem;
		GetWorld()->m_gameObjectList.push_back(newItem);
		GetWorld()->m_itemList.push_back(newItem);
	}
	newItem->SendMsg(NULL, AM_INITSTANDENTRY, (int)(&s), 0, 0);
}


void CGameMode::Zc_Item_Fall_Entry(char const *buf)
{
	PACKET_ZC_ITEM_FALL_ENTRY *p = (PACKET_ZC_ITEM_FALL_ENTRY *)buf;

	ITEMSTANDENTRYSTRUCT s;
	memset(&s, 0, sizeof(s));
	s.count = p->count;
	s.ITAID = p->ITAID;
	s.x = p->xPos;
	s.y = p->yPos;
	s.subX = p->subX;
	s.subY = p->subY;
	itoa(p->ITID, s.name, 10);
	s.isJumpEntry = true;
	s.isIdentified = p->IsIdentified;

	CItem *newItem = m_world->GetItemByAID(p->ITAID);
	if (newItem == NULL)
	{
		newItem = new CItem;
		GetWorld()->m_gameObjectList.push_back(newItem);
		GetWorld()->m_itemList.push_back(newItem);
	}
	newItem->SendMsg(NULL, AM_INITSTANDENTRY, (int)(&s), 0, 0);
}

void CGameMode::Zc_Item_Pickup_Ack(char const *buf)
{
	PACKET_ZC_ITEM_PICKUP_ACK *p = (PACKET_ZC_ITEM_PICKUP_ACK *)buf;
	m_pickupReqItemNaidList.erase(m_pickupReqItemNaidList.begin(), m_pickupReqItemNaidList.end());

	switch (p->result)
	{
	case PICKUP_SUCCEED:
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = p->type;
		itemInfo.m_location = p->location;
		itemInfo.m_itemIndex = p->Index;
		itemInfo.m_num = p->count;
		itemInfo.m_wearLocation = 0;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_refiningLevel = p->refiningLevel;

		if (atoi(itemInfo.m_itemName.c_str()) == 607)
			m_isMakeYgdrasil = FALSE;

		if (p->type == TYPE_ARROW || p->type == TYPE_AMMO || p->type == TYPE_THROWWEAPON)
			itemInfo.m_wearLocation = LOCATION_ARROW;

		g_session.AddItem(&itemInfo);

		char itemName[64];
		itemInfo.GetItemName(itemName);
		char pickMsg[128];
		sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 5, 0);

		int pickupCount = p->count;
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NOTIFYITEMOBTAINWND);
		wnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, itemInfo.m_isIdentified, 0, 0, 0);
		wnd->SendMsg(NULL, UM_SETTITLE, 0, pickupCount, (int)itemInfo.m_itemName.c_str(), 0);
		wnd->SendMsg(NULL, UM_SETINFO, (int)&itemInfo, 0, 0, 0);
		break;
	}
	case RECEIVE_ITEM:
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = p->type;
		itemInfo.m_location = p->location;
		itemInfo.m_itemIndex = p->Index;
		itemInfo.m_wearLocation = 0;
		itemInfo.m_num = p->count;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_refiningLevel = p->refiningLevel;

		if (atoi(itemInfo.m_itemName.c_str()) == 607)
			m_isMakeYgdrasil = FALSE;

		if (p->type == TYPE_ARROW || p->type == TYPE_AMMO || p->type == TYPE_THROWWEAPON)
			itemInfo.m_wearLocation = LOCATION_ARROW;

		switch (p->type)
		{
		case TYPE_ARMOR:
		case TYPE_ARMORTM:
		case TYPE_ARMORTB:
		case TYPE_ARMORMB:
		case TYPE_ARMORTMB:
		case TYPE_WEAPON:
		case TYPE_BOW:
		case TYPE_BOTHHAND:
		case TYPE_GUN:
			break;
		default:
			itemInfo.m_location = 0;
			break;
		}

		ITEM_INFO curItemInfo = g_session.GetItemInfoByIndex(p->Index);
		char itemName[64];
		char pickMsg[128];
		int gainCount = p->count;
		if (curItemInfo.m_itemIndex)
		{
			itemInfo.GetItemName(itemName);
			sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count - curItemInfo.m_num);
			gainCount = p->count - curItemInfo.m_num;
		}
		else
		{
			itemInfo.GetItemName(itemName);
			sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count);
			gainCount = p->count;
		}

		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 5, 0);

		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NOTIFYITEMOBTAINWND);
		wnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, itemInfo.m_isIdentified, 0, 0, 0);
		wnd->SendMsg(NULL, UM_SETTITLE, 0, gainCount, (int)itemInfo.m_itemName.c_str(), 0);
		wnd->SendMsg(NULL, UM_SETINFO, (int)&itemInfo, 0, 0, 0);

		if (m_isWaitingCancelExchangeItem)
		{
			m_isWaitingCancelExchangeItem = FALSE;
			g_session.CancelExchangeNoRefresh();
		}

		g_session.SetItem(&itemInfo);
		break;
	}
	case PICKUP_OVERCOUNT_ONEITEM:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_OVERCOUNT_ONEITEM)), 0xff, 0, 0);
		break;
	case PICKUP_OVERCOUNT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_BECAUSE_COUNT)), 0xff, 0, 0);
		break;
	case PICKUP_OVERWEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_BECAUSE_WEIGHT)), 0xff, 0, 0);
		break;
	case PICKUP_FAILED:
	case PICKUP_REFUSED_TIME:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM)), 0xff, 0, 0);
		break;
	}
}

void	CGameMode :: Zc_Item_Disappear(const char *buf)
{
	PACKET_ZC_ITEM_DISAPPEAR *	p = (PACKET_ZC_ITEM_DISAPPEAR *)buf;
	m_world->RemoveItem(p->ITAID);
	RemovePickupItemNaid(p->ITAID);
}

void CGameMode::Zc_Normal_Itemlist(char const *buf)
{
	g_session.ClearAntiEquipItemInInventory();

	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO);
	NORMALITEM_EXTRAINFO *dumpStr = (NORMALITEM_EXTRAINFO *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = 0;
		itemInfo.m_slot[1] = 0;
		itemInfo.m_slot[2] = 0;
		itemInfo.m_slot[3] = 0;
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		g_session.SetItem(&itemInfo);
	}
}


void CGameMode::Zc_Normal_Itemlist2(char const *buf)
{
	g_session.ClearAntiEquipItemInInventory();

	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO2);
	NORMALITEM_EXTRAINFO2 *dumpStr = (NORMALITEM_EXTRAINFO2 *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		g_session.SetItem(&itemInfo);
	}
}

void CGameMode::Zc_Normal_Itemlist3(char const *buf)
{
	g_session.ClearAntiEquipItemInInventory();

	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO3);
	NORMALITEM_EXTRAINFO3 *dumpStr = (NORMALITEM_EXTRAINFO3 *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		g_session.SetItem(&itemInfo);
	}
}

void CGameMode::Zc_Equipment_Itemlist(char const *buf)
{
	g_session.ClearEquipItemInInventory();
	g_session.ClearEquipItem();

	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO);
	EQUIPMENTITEM_EXTRAINFO *dumpStr = (EQUIPMENTITEM_EXTRAINFO *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;

		if (g_session.IsPetEgg(atoi(itemInfo.m_itemName.c_str())))
			if (itemInfo.m_isDamaged)
				g_session.m_petEggIndex = itemInfo.m_itemIndex;

		if (itemInfo.m_wearLocation) {
			g_session.AddEquipItem(&itemInfo);
		} else {
			g_session.AddItem(&itemInfo);
			g_session.SetItem(&itemInfo);
		}
	}
}

void	CGameMode :: Zc_Stopmove(const char *buf)
{
	PACKET_ZC_STOPMOVE *	p = (PACKET_ZC_STOPMOVE *)buf;
	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_STOPMOVE, p->xPos, p->yPos, 0);
	}
	else
	{
		CGameActor *			theActor = GetWorld()->GetGameActorByAID(p->AID);
		if (theActor)
		{
			theActor->SendMsg(NULL, AM_STOPMOVE, p->xPos, p->yPos, 0);
		}
	}
}

void	CGameMode :: Zc_HighJump(const char *buf)
{
	PACKET_ZC_HIGHJUMP *	p = (PACKET_ZC_HIGHJUMP *)buf;
	if (p->AID == g_session.GetAid())
	{
		g_modeMgr.GetGameMode()->GetClientCoor(p->xPos, p->yPos, m_world->m_player->m_pos.x, m_world->m_player->m_pos.z);
		m_world->m_player->m_pos.y = m_world->m_attr->GetHeight(m_world->m_player->m_pos.x, m_world->m_player->m_pos.z);
	}
	else
	{
		CGameActor *			theActor = GetWorld()->GetGameActorByAID(p->AID);
		if (theActor)
		{
			g_modeMgr.GetGameMode()->GetClientCoor(p->xPos, p->yPos, theActor->m_pos.x, theActor->m_pos.z);
			theActor->m_pos.y = m_world->m_attr->GetHeight(theActor->m_pos.x, theActor->m_pos.z);
		}
	}
}


void CGameMode::Zc_Par_Change(char const *buf)
{
	PACKET_ZC_PAR_CHANGE *p = (PACKET_ZC_PAR_CHANGE *)buf;

	switch (p->varID) {
	case VAR_SPEED:
		if (g_session.GetSpeed() != p->count) {
			g_session.SetSpeed(p->count);
		}
		break;

	case VAR_VIRTUE:
		g_session.SetVirtue(p->count);
		break;

	case VAR_HONOR:
		if (p->count < 0) {
			char msg[256];
			sprintf(msg, MsgStr(MSI_BAD_MANNER_REMAIN), -p->count);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff, 0, 0);
			m_world->m_player->SendMsg(NULL, AM_EMOTION, ET_CHAT_PROHIBIT, 0, 0);
		} else {
			char msg[256];
			sprintf(msg, MsgStr(MSI_BAD_MANNER_END));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff, 0, 0);
			m_world->m_player->SendMsg(NULL, AM_DELETE_PROHIBITEFFECT, 0, 0, 0);
		}
		g_session.SetHonor(p->count);
		break;

	case VAR_PKHONOR:
		if (p->count < -60) {
			char msg[256];
			sprintf(msg, MsgStr(MSI_PK_HONOR));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff, 0, 0);
		}
		break;

	case VAR_HP:
	{
		g_session.ChangeXorValue();
		int hp = g_session.GetHp();
		int maxhp = g_session.GetMaxhp();
		if ((hp * 100) / maxhp < 25) {
			g_session.SetHp(p->count);
			return;
		}
		if ((p->count * 100) / g_session.GetMaxhp() < 25) {
			CGameActor *pet = m_world->GetGameActorByAID(g_session.m_petGID);
			if (pet && rand() % 100 < 50) {
				int h = g_session.GetPetHungryState(g_session.m_petHungry);
				int f = g_session.GetPetFriendlyState(g_session.m_petFriendly);
				int talk = g_session.GetPetTalkNumber(g_session.m_petJob, PM_DANGER, h);
				SendMsg(MM_PET_ACT, talk, 0, 0);
				SendMsg(MM_PET_ACT, g_session.m_petEmotionTable[h][f][PM_DANGER], 0, 0);
			}
		}
		g_session.SetHp(p->count);
		break;
	}

	case VAR_MAXHP:
	{
		g_session.ChangeXorValue();
		g_session.SetMaxhp(p->count);
		UIFrameWnd *messengerGroupWnd = (UIFrameWnd *)g_windowMgr.GetMessengerGroupWnd();
		if (messengerGroupWnd)
			messengerGroupWnd->SendMsg(NULL, UM_COMMAND, 0x22, g_session.GetAid(), 0, 0);
		break;
	}

	case VAR_SP:
	{
		g_session.ChangeXorValue();
		g_session.SetSp(p->count);
		break;
	}

	case VAR_MAXSP:
	{
		g_session.ChangeXorValue();
		g_session.SetMaxsp(p->count);
		break;
	}

	case VAR_POINT:
		g_session.SetPoint(p->count);
		if (g_windowMgr.GetStatusWnd())
			((UIFrameWnd *)g_windowMgr.GetStatusWnd())->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		break;

	case VAR_HAIRCOLOR:
		return;

	case VAR_CLEVEL:
	{
		g_session.SetLevel(p->count);
		PlayWave("levelup.wav");
		switch (g_session.GetJob()) {
		case JT_SUPERNOVICE:
		case JT_SUPERNOVICE_B:
			m_world->m_player->LaunchEffect(0x152);
			break;
		case JT_TAEKWON:
		case JT_STAR:
		case JT_STAR2:
		case JT_LINKER:
			m_world->m_player->LaunchEffect(0x246);
			break;
		case JT_GANGSI:
		case JT_DEATHKNIGHT:
		case JT_COLLECTOR:
			m_world->m_player->LaunchEffect(0x152);
			break;
		default:
			m_world->m_player->LaunchEffect(0x173);
			break;
		}
		g_windowMgr.MakeWindow((WINDOWID)0x15);
		CGameActor *pet = m_world->GetGameActorByAID(g_session.m_petGID);
		if (pet) {
			int h = g_session.GetPetHungryState(g_session.m_petHungry);
			int f = g_session.GetPetFriendlyState(g_session.m_petFriendly);
			int talk = g_session.GetPetTalkNumber(g_session.m_petJob, PM_LEVELUP, h);
			SendMsg(MM_PET_ACT, talk, 0, 0);
			SendMsg(MM_PET_ACT, g_session.m_petEmotionTable[h][f][PM_LEVELUP], 0, 0);
		}
		break;
	}

	case VAR_SPPOINT:
		g_session.SetSkillPoint(p->count);
		g_session.RefreshSkillItemWnd();
		break;

	case VAR_STR:
		g_session.SetStr(p->count);
		break;
	case VAR_AGI:
		g_session.SetAgi(p->count);
		break;
	case VAR_VIT:
		g_session.SetVit(p->count);
		break;
	case VAR_INT:
		g_session.SetInt(p->count);
		break;
	case VAR_DEX:
		g_session.SetDex(p->count);
		break;
	case VAR_LUK:
		g_session.SetLuk(p->count);
		break;

	case VAR_WEIGHT:
	{
		p->count /= 10;
		int prevPercent = (g_session.GetWeight() * 100) / (g_session.GetMaxWeight() > 1 ? g_session.GetMaxWeight() : 1);
		g_session.SetWeight(p->count);
		if (g_session.GetMaxWeight() != 0) {
			int percent = (g_session.GetWeight() * 100) / (g_session.GetMaxWeight() > 1 ? g_session.GetMaxWeight() : 1);
			if (prevPercent < percent) {
				if (percent >= 50 && prevPercent < 50) {
					g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NO_RECOVER_OVERWEIGHT)), 0xff, 7, 0);
				}
				if (percent >= 90 && prevPercent < 90) {
					g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NO_ATTACK_OVERWEIGHT)), 0xff, 7, 0);
				}
			} else if (percent < prevPercent) {
				if (percent < 50 && prevPercent >= 50) {
					g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NO_RECOVER_OVERWEIGHT_RELEASED)), 0xffff00, 7, 0);
				}
				if (percent < 90 && prevPercent >= 90) {
					g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NO_ATTACK_OVERWEIGHT_RELEASED)), 0xffff00, 7, 0);
				}
			}
		}
		break;
	}

	case VAR_MAXWEIGHT:
	{
		p->count /= 10;
		int prevPercent = (g_session.GetWeight() * 100) / (g_session.GetMaxWeight() > 1 ? g_session.GetMaxWeight() : 1);
		int prevMaxWeight = g_session.GetMaxWeight();
		g_session.SetMaxWeight(p->count);
		if (prevMaxWeight != 0) {
			if (prevMaxWeight < g_session.GetMaxWeight()) {
				int percent = (g_session.GetWeight() * 100) / (g_session.GetMaxWeight() > 1 ? g_session.GetMaxWeight() : 1);
				if (percent < 50 && prevPercent >= 50) {
					g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NO_RECOVER_OVERWEIGHT_RELEASED)), 0xffff00, 7, 0);
				}
				if (percent < 90 && prevPercent >= 90) {
					g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NO_ATTACK_OVERWEIGHT_RELEASED)), 0xffff00, 7, 0);
				}
			}
		}
		break;
	}

	case VAR_ATTPOWER:
		g_session.SetAttPower(p->count);
		break;
	case VAR_REFININGPOWER:
		g_session.SetRefiningPower(p->count);
		break;
	case VAR_MAX_MATTPOWER:
		g_session.SetMax_mattPower(p->count);
		break;
	case VAR_MIN_MATTPOWER:
		g_session.SetMin_mattPower(p->count);
		break;
	case VAR_ITEMDEFPOWER:
		g_session.SetItemDefPower(p->count);
		break;
	case VAR_PLUSDEFPOWER:
		g_session.SetPlusdefPower(p->count);
		break;
	case VAR_MDEFPOWER:
		g_session.SetMdefPower(p->count);
		break;
	case VAR_PLUSMDEFPOWER:
		g_session.SetPlusmdefPower(p->count);
		break;
	case VAR_HITSUCCESSVALUE:
		g_session.SetHitSuccessValue(p->count);
		break;
	case VAR_AVOIDSUCCESSVALUE:
		g_session.SetAvoidSuccessValue(p->count);
		break;
	case VAR_PLUSAVOIDSUCCESSVALUE:
		g_session.SetPlusAvoidSuccessValue(p->count);
		break;
	case VAR_CRITICALSUCCESSVALUE:
		g_session.SetCriticalSuccessValue(p->count);
		break;
	case VAR_ASPD:
		g_session.SetASPD(p->count);
		break;
	case VAR_PLUSASPD:
		g_session.SetPlusASPD(p->count);
		break;

	case VAR_JOBLEVEL:
		g_session.SetJoblevel(p->count);
		if (p->count <= 50)
			m_world->m_player->LaunchEffect(0x9e);
		else
			m_world->m_player->LaunchEffect(0x151);
		g_windowMgr.MakeWindow((WINDOWID)0x31);
		break;

	case VAR_PLUSARROWPOWEROFITEM:
		return;
	case VAR_PLUSATTREFININGPOWEROFITEM:
		return;
	}
}


void CGameMode::Zc_Longpar_Change(char const *buf)
{
	char msg[256];
	PACKET_ZC_LONGPAR_CHANGE* p = (PACKET_ZC_LONGPAR_CHANGE*)buf;

	switch (p->varID) {
	case VAR_EXP:
		if (g_session.IsMasterAid(2)) {
			int exp = p->amount - g_session.GetExp();
			if (p->amount && exp) {
				int cnt = (g_session.GetNextExp() - g_session.GetExp()) / exp;
				sprintf(msg, "%d : %d/%d More : %d", exp, p->amount, g_session.GetNextExp(), cnt);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, RGB(245, 30, 245), 0, 0);
			}
		}
		g_session.SetExp(p->amount);
		break;

	case VAR_JOBEXP:
		g_session.SetJobexp(p->amount);
		break;

	case VAR_MONEY:
		if (m_isWaitingCancelExchangeItem) {
			m_isWaitingCancelExchangeItem = FALSE;
			g_session.CancelExchangeNoRefresh();
		}
		g_session.SetGold(p->amount);
		break;

	case VAR_MAXEXP:
		g_session.SetNextexp(p->amount);
		break;

	case VAR_MAXJOBEXP:
		g_session.SetJobnextexp(p->amount);
		break;
	}
}

void	CGameMode :: Zc_Use_Item_Ack(const char *buf)
{
	PACKET_ZC_USE_ITEM_ACK *	p = (PACKET_ZC_USE_ITEM_ACK *)buf;
	if (p->result)
	{
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(p->index);
		if (itemInfo.m_itemIndex)
		{
			g_session.DecItem(p->index, itemInfo.m_num - p->count);
			if (itemInfo.IsRecoveryItem())
				m_world->m_player->SendMsg(NULL, AM_USEITEM_EFFECT, 0, 0, 0);
		}
	}
	m_waitingUseItemAck = false;
}

void	CGameMode :: Zc_Req_Wear_Equip_Ack(const char *buf)
{
	PACKET_ZC_REQ_WEAR_EQUIP_ACK *	p = (PACKET_ZC_REQ_WEAR_EQUIP_ACK *)buf;
	m_waitingWearEquipAck = timeGetTime() - 2000;
	switch (p->result) {
	case 0:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CAN_NOT_EQUIP_ITEM)), 0xff, 6, 0);
		break;
	case 1:
		{
			ITEM_INFO	itemInfo = g_session.GetItemInfoByIndex(p->index);
			if (itemInfo.m_itemIndex) {
				itemInfo.m_wearLocation = p->wearLocation;
				g_session.MoveItemFromItemWndToEquipWnd(&itemInfo);
			}
		}
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CAN_NOT_EQUIP_ITEM)), 0xff, 6, 0);
		break;
	}
}


void	CGameMode :: Zc_Req_Takeoff_Equip_Ack(const char *buf)
{
	PACKET_ZC_REQ_TAKEOFF_EQUIP_ACK *	p = (PACKET_ZC_REQ_TAKEOFF_EQUIP_ACK *)buf;
	m_waitingTakeoffEquipAck = timeGetTime() - 2000;
	if (p->result)
	{
		p->index;
		sAssert(p->wearLocation);
		if (p->wearLocation == 0)
			return;
		g_session.MoveItemFromEquipWndToItemWnd(p->wearLocation);

	}
}

void	CGameMode :: Zc_Item_Throw_Ack(const char *buf)
{
	if (m_isWaitingCancelExchangeItem)
	{
		m_isWaitingCancelExchangeItem = FALSE;
		g_session.CancelExchangeNoRefresh();
	}
	PACKET_ZC_ITEM_THROW_ACK *	p = (PACKET_ZC_ITEM_THROW_ACK *)buf;
	g_session.DecItem(p->Index, p->count);
	m_waitingItemThrowAck = 0;
	Trace("PACKET_ZC_ITEM_THROW_ACK - index==%d, count==%d", p->Index, p->count);
}

extern int GetCharsetFromLang(WORD langid);
void	CGameMode :: Zc_Say_Dialog(const char *buf)
{
	PACKET_ZC_SAY_DIALOG *p = (PACKET_ZC_SAY_DIALOG *)buf;

	m_isOnQuest = 1;
	m_lastNaid  = p->NAID;

	char dialogSay[2048 + 3];
	int  sayLength = p->PacketLength - sizeof(PACKET_ZC_SAY_DIALOG);

	if (g_serviceType == ServiceGermany)
	{
		sprintf(dialogSay, "|%02x", GetCharsetFromLang(0x1f));
		memcpy(dialogSay + 3, buf + 8, sayLength);
		dialogSay[sayLength + 3] = 0;
	}
	else
	{
		memcpy(dialogSay, buf + 8, sayLength);
		dialogSay[sayLength] = 0;
	}

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_SAYDIALOGWND);
	wnd->SendMsg(NULL, UM_SETSAYDIALOG, int(dialogSay), p->NAID, 0, 0);
}


void	CGameMode :: Zc_Wait_Dialog(const char *buf)
{
	PACKET_ZC_WAIT_DIALOG *		p = (PACKET_ZC_WAIT_DIALOG *)buf;
	m_isOnQuest = true;
	m_lastNaid = p->NAID;

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_SAYDIALOGWND);
	wnd->SendMsg(NULL, UM_ENABLENEXTBUTTON, 0, 0, 0, 0);
}

void	CGameMode :: Zc_Close_Dialog(const char *buf)
{
	UISayDialogWnd *	wnd = g_windowMgr.GetSayDialogWnd();
	if (wnd)
	{
		wnd->SendMsg(NULL, UM_DISABLENEXTBUTTON, 0, 0, 0, 0);
		wnd->SendMsg(NULL, UM_ENABLECLOSEBUTTON, 0, 0, 0, 0);
	}
	else
	{
		PACKET_ZC_CLOSE_DIALOG *	p = (PACKET_ZC_CLOSE_DIALOG *)buf;
		m_isOnQuest = false;
		m_lastNaid = p->NAID;
		g_windowMgr.DeleteWindow(WID_SAYDIALOGWND);
		g_windowMgr.DeleteWindow(WID_CHOOSEWND);
	}
}

void	CGameMode :: Zc_Menu_List(const char *buf)
{
	PACKET_ZC_MENU_LIST *	p = (PACKET_ZC_MENU_LIST *)buf;
	char	dialogSay[2048];
	m_isOnQuest = 1;
	int		sayLength = p->PacketLength - sizeof(PACKET_ZC_MENU_LIST);
	memcpy(dialogSay, buf + sizeof(PACKET_ZC_MENU_LIST), sayLength);
	dialogSay[sayLength] = 0;
	g_windowMgr.DeleteWindow(WID_CHOOSEWND);
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_CHOOSEWND);
	char *	token = strtok(dialogSay, ":");
	while (token) {
		wnd->SendMsg(NULL, UM_SETCHOOSE, int(token), 0, 0, 0);
		token = strtok(NULL, ":");
	}
	wnd->SendMsg(NULL, UM_SETNAID, p->NAID, 0, 0, 0);
	UISayDialogWnd *sayDialogWnd = g_windowMgr.GetSayDialogWnd();
	if (sayDialogWnd)
		sayDialogWnd->SendMsg(NULL, UM_DISABLENEXTBUTTON, 0, 0, 0, 0);
}


void	CGameMode :: Zc_Restart_Ack(const char *buf)
{
	PACKET_ZC_RESTART_ACK *	p = (PACKET_ZC_RESTART_ACK *)buf;

	if (p->type == 1) {
		CRagConnection::instanceR().Disconnect();
		g_loginStartMode = 5;
		g_modeMgr.Switch(0, "login.rsw");
	}
}


void CGameMode::Zc_Status_Change_Ack(char const *buf)
{
	PACKET_ZC_STATUS_CHANGE_ACK *p = (PACKET_ZC_STATUS_CHANGE_ACK *)buf;
	m_waitingReqStatusAck = FALSE;
	UIStatusWnd *statusWnd = g_windowMgr.GetStatusWnd();
	if (!p->result)
		return;

	switch (p->statusID)
	{
	case VAR_STR:
		g_session.SetStr(p->value);
		if (statusWnd)
			statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		return;
	case VAR_AGI:
		g_session.SetAgi(p->value);
		if (statusWnd)
			statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		return;
	case VAR_VIT:
		g_session.SetVit(p->value);
		if (statusWnd)
			statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		return;
	case VAR_INT:
		g_session.SetInt(p->value);
		if (statusWnd)
			statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		return;
	case VAR_DEX:
		g_session.SetDex(p->value);
		if (statusWnd)
			statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		return;
	case VAR_LUK:
		g_session.SetLuk(p->value);
		if (statusWnd)
			statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		return;
	case VAR_ATTPOWER:
		g_session.SetAttPower(p->value);
		return;
	case VAR_REFININGPOWER:
		g_session.SetRefiningPower(p->value);
		return;
	case VAR_MAX_MATTPOWER:
		g_session.SetMax_mattPower(p->value);
		return;
	case VAR_MIN_MATTPOWER:
		g_session.SetMin_mattPower(p->value);
		return;
	case VAR_ITEMDEFPOWER:
		g_session.SetItemDefPower(p->value);
		return;
	case VAR_PLUSDEFPOWER:
		g_session.SetPlusdefPower(p->value);
		return;
	case VAR_MDEFPOWER:
		g_session.SetMdefPower(p->value);
		return;
	case VAR_PLUSMDEFPOWER:
		g_session.SetPlusmdefPower(p->value);
		return;
	case VAR_HITSUCCESSVALUE:
		g_session.SetHitSuccessValue(p->value);
		return;
	case VAR_AVOIDSUCCESSVALUE:
		g_session.SetAvoidSuccessValue(p->value);
		return;
	case VAR_PLUSAVOIDSUCCESSVALUE:
		g_session.SetPlusAvoidSuccessValue(p->value);
		return;
	case VAR_CRITICALSUCCESSVALUE:
		Trace("VAR_CRITICALSUCCESSVALUE");
		g_session.SetCriticalSuccessValue(p->value);
		return;
	case VAR_ASPD:
		g_session.SetASPD(p->value);
		return;
	case VAR_PLUSASPD:
		g_session.SetPlusASPD(p->value);
		return;
	case VAR_JOBLEVEL:
		g_session.SetJoblevel(p->value);
		g_windowMgr.MakeWindow(WID_NOTIFYJOBLEVELUPWND);
		return;
	}
}


void CGameMode::Zc_Status(char const *buf)
{
	PACKET_ZC_STATUS *p = (PACKET_ZC_STATUS *)buf;

	g_session.SetStr(p->str);
	g_session.SetAgi(p->agi);
	g_session.SetVit(p->vit);
	g_session.SetInt(p->Int);
	g_session.SetDex(p->dex);
	g_session.SetLuk(p->luk);
	g_session.SetStandardStr(p->standardStr);
	g_session.SetStandardAgi(p->standardAgi);
	g_session.SetStandardVit(p->standardVit);
	g_session.SetStandardInt(p->standardInt);
	g_session.SetStandardDex(p->standardDex);
	g_session.SetStandardLuk(p->standardLuk);
	g_session.SetPoint(p->point);

	g_session.SetAttPower(p->attPower);
	g_session.SetRefiningPower(p->refiningPower);
	g_session.SetMax_mattPower(p->max_mattPower);
	g_session.SetMin_mattPower(p->min_mattPower);
	g_session.SetItemDefPower(p->itemdefPower);
	g_session.SetPlusdefPower(p->plusdefPower);
	g_session.SetMdefPower(p->mdefPower);
	g_session.SetPlusmdefPower(p->plusmdefPower);
	g_session.SetHitSuccessValue(p->hitSuccessValue);
	g_session.SetAvoidSuccessValue(p->avoidSuccessValue);
	g_session.SetPlusAvoidSuccessValue(p->plusAvoidSuccessValue);
	g_session.SetCriticalSuccessValue(p->criticalSuccessValue);
	g_session.SetASPD(p->ASPD);
	g_session.SetPlusASPD(p->plusASPD);

	UIStatusWnd *statusWnd = g_windowMgr.GetStatusWnd();
	if (statusWnd)
		statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
}

void	CGameMode :: Zc_Status_Change(const char *buf)
{
	PACKET_ZC_STATUS_CHANGE *	p = (PACKET_ZC_STATUS_CHANGE *)buf;
	switch (p->statusID)
	{
	case VAR_STANDARD_STR:
		g_session.SetStandardStr((unsigned char)p->value);
		break;
	case VAR_STANDARD_AGI:
		g_session.SetStandardAgi((unsigned char)p->value);
		break;
	case VAR_STANDARD_VIT:
		g_session.SetStandardVit((unsigned char)p->value);
		break;
	case VAR_STANDARD_INT:
		g_session.SetStandardInt((unsigned char)p->value);
		break;
	case VAR_STANDARD_DEX:
		g_session.SetStandardDex((unsigned char)p->value);
		break;
	case VAR_STANDARD_LUK:
		g_session.SetStandardLuk((unsigned char)p->value);
		break;
	}
	UIStatusWnd *statusWnd = g_windowMgr.GetStatusWnd();
	if (statusWnd)
		statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
}

void	CGameMode :: Zc_Emotion(const char *buf)
{
	PACKET_ZC_EMOTION *		p = (PACKET_ZC_EMOTION *)buf;

	if (g_session.IsExAid(p->GID))
		return;

	if (p->GID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_EMOTION, p->type, 0, 0);
	}
	else
	{
		CGameActor *	theActor = GetWorld()->GetGameActorByAID(p->GID);
		if (theActor)
			theActor->SendMsg(NULL, AM_EMOTION, p->type, 0, 0);
	}
}


void	CGameMode :: Zc_User_Count(const char *buf)
{
	char msg[128];

	if (g_serviceType == ServiceChina && !IsGravityAid(g_session.GetAid()))
	{
		const char *userCount = GetUserCountString(*(WORD *)(buf + 2));
		strcpy(msg, userCount);
	}
	else
	{
		PACKET_ZC_USER_COUNT *p = (PACKET_ZC_USER_COUNT *)buf;
		sprintf(msg, MsgStr(MSI_NUMPEOPLE), p->count);
	}
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff, 0, 0);
}

void	CGameMode :: ZC_BLACKSMITH_RANK(const char *buf)
{
	int i;
	PACKET_ZC_BLACKSMITH_RANK *	p = (PACKET_ZC_BLACKSMITH_RANK *)buf;

	char msg[256];
	sprintf(msg,"========== BLACKSMITH RANK ============");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9b9b9b, 0, 0);
	for (i=0;i<10;i++)
	{
		if (i==9)
			sprintf(msg,"[ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		else
			sprintf(msg," [ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xcdcdcd, 0, 0);
	}

	sprintf(msg,"===========================================");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9b9b9b, 0, 0);
}

void	CGameMode :: ZC_ALCHEMIST_RANK(const char *buf)
{
	int i;
	PACKET_ZC_ALCHEMIST_RANK *	p = (PACKET_ZC_ALCHEMIST_RANK *)buf;

	char msg[256];
	sprintf(msg,"=========== ALCHEMIST RANK =============");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9bcd9b, 0, 0);
	for (i=0;i<10;i++)
	{
		if (i==9)
			sprintf(msg,"[ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		else
			sprintf(msg," [ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xcdffcd, 0, 0);
	}

	sprintf(msg,"===========================================");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9bcd9b, 0, 0);
}

void	CGameMode :: ZC_TAEKWON_RANK(const char *buf)
{
	int i;
	PACKET_ZC_TAEKWON_RANK *	p = (PACKET_ZC_TAEKWON_RANK *)buf;

	char msg[256];
	sprintf(msg,"============ TAEKWON RANK =============");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xcd9b9b, 0, 0);
	for (i=0;i<10;i++)
	{
		if (i==9)
			sprintf(msg,"[ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		else
			sprintf(msg," [ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffcdcd, 0, 0);
	}

	sprintf(msg,"===========================================");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xcd9b9b, 0, 0);
}


void CGameMode::ZC_GANGSI_RANK(char const *buf)
{
	int i, r, g, b, plus;
	char msg[256];
	PACKET_ZC_GANGSI_RANK *p = (PACKET_ZC_GANGSI_RANK *)buf;

	if (p->PacketSwitch == JT_GANGSI)
	{
		sprintf(msg, "============ TAIMING RANK ===============");
		r = 205;
		g = 155;
		b = 155;
		plus = 50;
	}
	else if (p->PacketSwitch == JT_DEATHKNIGHT)
	{
		sprintf(msg, "========== DEATHKNIGHT RANK ===========");
		r = 5;
		g = 5;
		b = 5;
		plus = 150;
	}
	else if (p->PacketSwitch == JT_COLLECTOR)
	{
		sprintf(msg, "========== COLLECTOR RANK ===========");
		r = 205;
		g = 205;
		b = 155;
		plus = 50;
	}

	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), RGB(r, g, b), 0, 0);

	for (i = 0; i < 10; i++)
	{
		if (i == 9)
			sprintf(msg, "[ %d ] %16s  :  %4d POINT", i + 1, p->Name[i], p->Point[i]);
		else
			sprintf(msg, " [ %d ] %16s  :  %4d POINT", i + 1, p->Name[i], p->Point[i]);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), RGB(r + plus, g + plus, b + plus), 0, 0);
	}

	sprintf(msg, "===========================================");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), RGB(r, g, b), 0, 0);
}

void	CGameMode :: ZC_BLACKSMITH_POINT(const char *buf)
{
	PACKET_ZC_BLACKSMITH_POINT *	p = (PACKET_ZC_BLACKSMITH_POINT *)buf;

	char msg[256];
	sprintf(msg, MsgStr(MSI_BLACKSMITH_POINT),p->Point,p->TotalPoint);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, (int)RGB(205, 205, 205), 0, 0);
}

void	CGameMode :: ZC_ALCHEMIST_POINT(const char *buf)
{
	PACKET_ZC_ALCHEMIST_POINT *	p = (PACKET_ZC_ALCHEMIST_POINT *)buf;

	char msg[256];
	sprintf(msg, MsgStr(MSI_ALCHEMIST_POINT),p->Point,p->TotalPoint);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, (int)RGB(205, 255, 205), 0, 0);
}

void	CGameMode :: ZC_TAEKWON_POINT(const char *buf)
{
	PACKET_ZC_TAEKWON_POINT *	p = (PACKET_ZC_TAEKWON_POINT *)buf;

	char msg[256];
	sprintf(msg, MsgStr(MSI_TAEKWON_POINT),p->Point,p->TotalPoint);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, (int)RGB(205, 205, 255), 0, 0);
}

void	CGameMode :: ZC_GANGSI_POINT(const char *buf)
{
	PACKET_ZC_GANGSI_POINT *	p = (PACKET_ZC_GANGSI_POINT *)buf;
	char msg[256];

	if ( p->PacketSwitch == JT_GANGSI )
		sprintf(msg, MsgStr(MSI_GANGSI_POINT),p->Point,p->TotalPoint);
	else if ( p->PacketSwitch == JT_DEATHKNIGHT )
		sprintf(msg, MsgStr(MSI_DEATH_POINT),p->Point,p->TotalPoint);
	else if ( p->PacketSwitch == JT_COLLECTOR )
		sprintf(msg, MsgStr(MSI_COLLECTOR_POINT),p->Point,p->TotalPoint);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffcdcd, 0, 0);
}


void CGameMode::ZC_DEATH_QUESTION(char const *buf)
{
	PACKET_ZC_DEATH_QUESTION *p = (PACKET_ZC_DEATH_QUESTION *)buf;

	if (p->Qcategory == 100) {
		g_session.m_MightyGauge = p->Qnum;
		return;
	}

	char skillName[64];
	char msg2[256];
	memset(skillName, 0, 64);
	m_broadCastTick = timeGetTime();
	if (m_broadcastBalloon == NULL) {
		m_broadcastBalloon = new UITransBalloonText;
		m_broadcastBalloon->Create(8, 8);
		g_windowMgr.AddWindowFront(m_broadcastBalloon);
	}

	if (p->Qcategory == 1) {
		strcpy(skillName, SKILL_INFO::GetSkillNameBySkillId(p->Qnum));
		sprintf(msg2, MsgStr(MSI_DEATH_Q01), skillName);
		m_broadcastBalloon->SetText(msg2, 72);
	} else if (p->Qcategory == 2) {
		sprintf(msg2, MsgStr(MSI_DEATH_Q02));
		m_broadcastBalloon->SetText(msg2, 72);
	} else if (p->Qcategory == 3) {
		sprintf(msg2, MsgStr(MSI_DEATH_Q03));
		m_broadcastBalloon->SetText(msg2, 72);
	} else if (p->Qcategory == 4) {
		if (p->Qnum == 0)
			sprintf(skillName, "STR");
		else if (p->Qnum == 1)
			sprintf(skillName, "AGI");
		else if (p->Qnum == 2)
			sprintf(skillName, "VIT");
		else if (p->Qnum == 3)
			sprintf(skillName, "INT");
		else if (p->Qnum == 4)
			sprintf(skillName, "DEX");
		else
			sprintf(skillName, "LUK");

		sprintf(msg2, MsgStr(MSI_DEATH_Q04), skillName);
		m_broadcastBalloon->SetText(msg2, 72);
	} else if (p->Qcategory == 5) {
		int MSI_num = 0x4ac + p->Qnum;
		sprintf(msg2, MsgStr((MSGSTRINGID)MSI_num));
		m_broadcastBalloon->SetText(msg2, 72);
	} else if (p->Qcategory == 11) {
		sprintf(msg2, MsgStr(MSI_DEATH_R01));
		m_broadcastBalloon->SetText(msg2, 72);
	} else if (p->Qcategory == 12) {
		sprintf(msg2, MsgStr(MSI_DEATH_R02));
		m_broadcastBalloon->SetText(msg2, 72);
	} else if (p->Qcategory == 15) {
		sprintf(msg2, MsgStr(MSI_DEATH_R05));
		m_broadcastBalloon->SetText(msg2, 72);
	} else {
		sprintf(msg2, "???");
		m_broadcastBalloon->SetText(msg2, 72);
	}

	m_broadcastBalloon->SetFntSize(14);
	m_broadcastBalloon->Move((g_renderer->m_width - 640) / 2 - m_broadcastBalloon->GetWidth() / 2 + 320, 51);
	m_broadcastBalloon->SetFntColor(RGB(255, 200, 200), RGB(0, 0, 0));
}


void CGameMode::Zc_Sprite_Change(char const *buf)
{
	PACKET_ZC_SPRITE_CHANGE *p = (PACKET_ZC_SPRITE_CHANGE *)buf;

	CGameActor *tActor = NULL;
	if (p->GID == g_session.GetAid()) {
		tActor = m_world->m_player;
		switch (p->type) {
		case SPRITETYPE_JOB:
			if (p->value > JT_2004_JOB_BEGIN && p->value < JT_NOVICE_B) {
				g_session.m_bodyPalette = 1;
#if defined(_UPDATE2004B)
				if (p->value == JT_HUNTER_H || p->value == JT_PRIEST_H)
					g_session.m_bodyPalette = 0;
				if (p->value == JT_BLACKSMITH_H || p->value == JT_ASSASSIN_H)
					g_session.m_bodyPalette = 0;
				if (p->value == JT_KNIGHT_H)
					g_session.m_bodyPalette = 0;
				if (p->value == JT_WIZARD_H)
					g_session.m_bodyPalette = 0;
#endif
#if defined(_UPDATE2005A)
				g_session.m_bodyPalette = 0;
				if (p->value > JT_2004_JOB_BEGIN && p->value <= JT_THIEF_H)
					g_session.m_bodyPalette = 1;
#endif
			} else {
				g_session.m_bodyPalette = 0;
			}
			g_session.SetJob(p->value);
			break;
		case SPRITETYPE_HEAD:
			g_session.m_head = p->value;
			break;
		case SPRITETYPE_WEAPON:
			g_session.m_weapon = p->value;
			break;
		case SPRITETYPE_ACCESSORY:
			g_session.m_accessory = p->value;
			break;
		case SPRITETYPE_HEADPALETTE:
			g_session.m_headPalette = p->value;
			break;
		case SPRITETYPE_BODYPALATTE:
			g_session.m_bodyPalette = p->value;
			break;
		case SPRITETYPE_ACCESSORY2:
			g_session.m_accessory2 = p->value;
			break;
		case SPRITETYPE_ACCESSORY3:
			g_session.m_accessory3 = p->value;
			break;
		case SPRITETYPE_SHIELD:
			g_session.m_shield = p->value;
			break;
		}
	} else if (tActor = m_world->GetGameActorByAID(p->GID)) {
	} else if (tActor = m_world->GetSkillByAID(p->GID)) {
		if (p->type == SPRITETYPE_JOB)
			tActor->SendMsg(NULL, AM_SETJOB, p->value);
		return;
	} else {
		return;
	}

	switch (p->type) {
	case SPRITETYPE_JOB:
		tActor->SetJob(p->value);
		if (p->value > JT_2004_JOB_BEGIN && p->value < JT_NOVICE_B) {
			tActor->SetBodyPalette(1);
#if defined(_UPDATE2004B)
			if (p->value == JT_HUNTER_H || p->value == JT_PRIEST_H)
				tActor->SetBodyPalette(0);
			if (p->value == JT_BLACKSMITH_H || p->value == JT_ASSASSIN_H)
				tActor->SetBodyPalette(0);
			if (p->value == JT_KNIGHT_H)
				tActor->SetBodyPalette(0);
			if (p->value == JT_WIZARD_H)
				tActor->SetBodyPalette(0);
#endif
#if defined(_UPDATE2005A)
			tActor->SetBodyPalette(0);
			if (p->value > JT_2004_JOB_BEGIN && p->value <= JT_THIEF_H)
				tActor->SetBodyPalette(1);
#endif
		} else {
			tActor->SetBodyPalette(0);
		}
		tActor->SetImfFileName();
		tActor->SetHeadPaletteName();
		tActor->SetBodyPaletteName();
		break;
	case SPRITETYPE_HEAD:
		tActor->SetSprHead(p->value);
		tActor->SetImfFileName();
		tActor->SetHeadPaletteName();
		break;
	case SPRITETYPE_WEAPON:
		tActor->SetSprWeapon(p->value);
		break;
	case SPRITETYPE_ACCESSORY:
		tActor->SetSprAccessory(p->value);
		break;
	case SPRITETYPE_HEADPALETTE:
		tActor->SetHeadPalette(p->value);
		break;
	case SPRITETYPE_BODYPALATTE:
		tActor->SetBodyPalette(p->value);
		break;
	case SPRITETYPE_ACCESSORY2:
		tActor->SetSprAccessory2(p->value);
		break;
	case SPRITETYPE_ACCESSORY3:
		tActor->SetSprAccessory3(p->value);
		break;
	case SPRITETYPE_SHIELD:
		tActor->SetSprShield(p->value);
		break;
	}
}

void	CGameMode :: Zc_Select_Dealtype(const char *buf)
{
	PACKET_ZC_SELECT_DEALTYPE *		p = (PACKET_ZC_SELECT_DEALTYPE *)buf;

	m_isOnQuest = true;
	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_CHOOSESELLBUYWND);
	wnd->SendMsg(NULL, UM_SETNAID, p->NAID, 0, 0, 0);
	g_windowMgr.DeleteWindow(WID_ITEMSHOPWND);
	g_windowMgr.DeleteWindow(WID_ITEMPURCHASEWND);
	g_windowMgr.DeleteWindow(WID_ITEMSELLWND);
}


void CGameMode::Zc_Pc_Purchase_Itemlist(char const *buf)
{
	g_session.ClearDealItems();
	PACKET_ZC_PC_PURCHASE_ITEMLIST *p = (PACKET_ZC_PC_PURCHASE_ITEMLIST *)buf;

	int packetStrLen = sizeof(PACKET_ZC_PC_PURCHASE_ITEMLIST);
	int numStr = (p->PacketLength - packetStrLen) / sizeof(PURCHASE_ITEM);
	for (int i = 0; i < numStr; i++)
	{
		PURCHASE_ITEM p_item;
		memcpy(&p_item, buf + packetStrLen + i * sizeof(PURCHASE_ITEM), sizeof(PURCHASE_ITEM));

		SHOP_ITEM shop_item;
		memset(&shop_item, 0, sizeof(SHOP_ITEM));
		itoa(p_item.ITID, shop_item.itemName, 10);

		shop_item.index = -1;
		shop_item.price = p_item.price;
		shop_item.realPrice = p_item.discountprice;
		shop_item.type = p_item.type;
		shop_item.isIdentified = 1;
		g_session.AddShopItem(shop_item, 0);
	}

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_ITEMSHOPWND);
	wnd->SendMsg(NULL, UM_SETTITLE, 0, 0, 0, 0);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);

	wnd = g_windowMgr.MakeWindow(WID_ITEMPURCHASEWND);
	wnd->SendMsg(NULL, UM_SETTITLE, 0, 0, 0, 0);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}


void CGameMode::Zc_Pc_Sell_Itemlist(char const *buf)
{
	g_session.ClearDealItems();
	PACKET_ZC_PC_SELL_ITEMLIST *p = (PACKET_ZC_PC_SELL_ITEMLIST *)buf;

	int packetStrLen = sizeof(PACKET_ZC_PC_SELL_ITEMLIST);
	int numStr = (p->PacketLength - packetStrLen) / sizeof(SELL_ITEM);
	for (int i = 0; i < numStr; i++)
	{
		SELL_ITEM sell_item;
		memcpy(&sell_item, buf + packetStrLen + i * sizeof(SELL_ITEM), sizeof(SELL_ITEM));

		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(sell_item.index);
		if (itemInfo.m_itemIndex)
		{
			SHOP_ITEM shop_item;
			memset(&shop_item, 0, sizeof(SHOP_ITEM));
			shop_item.index = sell_item.index;
			strcpy(shop_item.itemName, itemInfo.m_itemName.c_str());
			shop_item.price = sell_item.price;
			shop_item.realPrice = sell_item.overchargeprice;
			shop_item.type = itemInfo.m_itemType;
			shop_item.isIdentified = itemInfo.m_isIdentified;
			g_session.AddShopItem(shop_item, 1);
		}
	}

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_ITEMSHOPWND);
	wnd->SendMsg(NULL, UM_SETTITLE, 1, 0, 0, 0);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
	wnd = g_windowMgr.MakeWindow(WID_ITEMSELLWND);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void	CGameMode :: Zc_Pc_Purchase_Result(const char *buf)
{
	PACKET_ZC_PC_PURCHASE_RESULT *	p = (PACKET_ZC_PC_PURCHASE_RESULT *)buf;
	switch (p->result)
	{
	case	PURCHASE_SUCCEED:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DEAL_SECCESS)), 0xffff00, 0, 0);
		break;
	case	PURCHASE_FAIL_MONEY:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_INSUFFICIENT_MONEY)), 0xff, 0, 0);
		break;
	case	PURCHASE_FAIL_COUNT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_INSUFFICIENT_OVER_COUNT)), 0xff, 0, 0);
		break;
	case	PURCHASE_FAIL_WEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OVER_WEIGHT)), 0xff, 0, 0);
		break;
	}

	g_windowMgr.DeleteWindow(WID_ITEMSHOPWND);
	g_windowMgr.DeleteWindow(WID_ITEMPURCHASEWND);
}

void	CGameMode :: Zc_Pc_Sell_Result(const char *buf)
{
	PACKET_ZC_PC_SELL_RESULT *	p = (PACKET_ZC_PC_SELL_RESULT *)buf;
	switch (p->result)
	{
	case	SELL_SUCCEED:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DEAL_SECCESS)), int(RGB(0, 255, 255)), 0, 0);
		break;
	case	SELL_FAIL:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DEAL_FAIL)), int(RGB(255, 0, 0)), 0, 0);
		break;
	}
	g_session.ClearDealItems();
	g_windowMgr.DeleteWindow(WID_ITEMSHOPWND);
	g_windowMgr.DeleteWindow(WID_ITEMSELLWND);
}


void CGameMode::Zc_Setting_Whisper_Pc(char const *buf)
{
	PACKET_ZC_SETTING_WHISPER_PC *p = (PACKET_ZC_SETTING_WHISPER_PC *)buf;

	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	m_isWaitingWhisperSetting = FALSE;
	switch (p->type)
	{
	case WT_EXCLUDE_WHISPER:
		switch (p->result)
		{
		case WHISPER_SETTING_SUCCEED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(std::string(m_refuseWhisperName + MsgStr(MSI_WHISPERIGNORESUCCESS)).c_str()), 0xffff, 0, 0);
			break;
		case WHIPSER_SETTING_FAILED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(std::string(m_refuseWhisperName + MsgStr(MSI_WHISPERIGNOREFAILED1)).c_str()), 0xffff, 0, 0);
			break;
		case WHISPER_SETTING_FAILED_OVERCOUNT:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(std::string(m_refuseWhisperName + MsgStr(MSI_WHISPERIGNOREFAILED2)).c_str()), 0xffff, 0, 0);
			break;
		}
		break;
	case WT_INCLUDE_WHISPER:
		switch (p->result)
		{
		case WHISPER_SETTING_SUCCEED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(std::string(m_refuseWhisperName + MsgStr(MSI_WHISPERNOIGNORESUCCESS)).c_str()), 0xffff, 0, 0);
			break;
		case WHIPSER_SETTING_FAILED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(std::string(m_refuseWhisperName + MsgStr(MSI_WHISPERNOIGNOREFAILED1)).c_str()), 0xffff, 0, 0);
			break;
		case WHISPER_SETTING_FAILED_OVERCOUNT:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(std::string(m_refuseWhisperName + MsgStr(MSI_WHISPERNOIGNOREFAILED2)).c_str()), 0xffff, 0, 0);
			break;
		}
		break;
	}
}

void	CGameMode :: Zc_Setting_Whisper_State(const char *buf)
{
	PACKET_ZC_SETTING_WHISPER_STATE *	p = (PACKET_ZC_SETTING_WHISPER_STATE *)buf;
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	m_isWaitingWhisperSetting = FALSE;
	switch (p->type)
	{
	case	WT_EXCLUDE_WHISPER:
		switch (p->result)
		{
		case	WHISPER_SETTING_SUCCEED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REFUSE_HEAR_ALL)), 0xffff, 0, 0);
			break;
		case	WHIPSER_SETTING_FAILED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REQ_REFUSE_HEAR_ALL_FAIL)), 0xffff, 0, 0);
			break;
		}
		break;
	case	WT_INCLUDE_WHISPER:
		switch (p->result)
		{
		case	WHISPER_SETTING_SUCCEED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_HEAR_ALL)), 0xffff, 0, 0);
			break;
		case	WHIPSER_SETTING_FAILED:
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REQ_HEAR_ALL_FAIL)), 0xffff, 0, 0);
			break;
		}
		break;
	}
}

void	CGameMode :: Zc_Whisper_List(const char *buf)
{
	char name[NAME_LENGTH + 1];
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	name[NAME_LENGTH] = 0;
	int numName = (*(short *)(buf + 2) - 4) / NAME_LENGTH;
	if (numName == 0)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NO_REFUSE_HEAR_LIST)), 0xffff00, 0, 0);
	else
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REFUSE_HEAR_LIST)), 0xffff00, 0, 0);
	for (int i = 0; i < numName; ++i)
	{
		if (4 + i * NAME_LENGTH > *(short *)(buf + 2))
			break;
		memcpy(name, buf + 4 + i * NAME_LENGTH, NAME_LENGTH);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(name), 0xffff00, 0, 0);
	}
}


void CGameMode::Zc_Ack_Create_Chatroom(char const *buf)
{
	PACKET_ZC_ACK_CREATE_CHATROOM *p = (PACKET_ZC_ACK_CREATE_CHATROOM *)buf;

	m_isWaitingEnterRoom = FALSE;

	switch (p->result)
	{
	case 0:
	{
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ROOM_IS_MADE)), 0xffff00, 0, 0);
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_CHATROOMWND);
		m_lastChatroomInfo.numPeople = 1;
		wnd->SendMsg(NULL, UM_SETINFO, int(&m_lastChatroomInfo), 0, 0, 0);
		wnd->SendMsg(NULL, UM_CLEARMEMBER, 0, 0, 0, 0);
		wnd->SendMsg(NULL, UM_ADDMEMBER, int(g_session.GetCharName()), 0, 0, 0);
		break;
	}
	case 1:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_MANY_ROOM)), 0xff, 0, 0);
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SAME_ROOM_TITLE)), 0xff, 0, 0);
		break;
	default:
		Trace("PACKET_ZC_ACK_CREATE_CHATROOM invalid result p->result==%d", p->result);
		break;
	}
}


void CGameMode::Zc_Room_Newentry(char const *buf)
{
	PACKET_ZC_ROOM_NEWENTRY *p = (PACKET_ZC_ROOM_NEWENTRY *)buf;

	m_aidList[p->roomID] = p->AID;
	CGameActor *theActor = m_world->GetGameActorByAID(p->AID);
	if (theActor)
	{
		ChatRoomInfo info;
		info.title.assign((const char *)p + sizeof(PACKET_ZC_ROOM_NEWENTRY), p->PacketLength - sizeof(PACKET_ZC_ROOM_NEWENTRY));
		info.roomType = p->type;
		info.numPeople = p->curcount;
		info.maxNumPeople = p->maxcount;
		info.roomId = p->roomID;
		if (info.title.size() > 60) info.title.resize(60);
		theActor->SendMsg(NULL, AM_MAKE_CHAT_TITLE, int(&info), 0, 0);
	}
}

void	CGameMode :: Zc_Destroy_Room(const char *buf)
{
	PACKET_ZC_DESTROY_ROOM *	p = (PACKET_ZC_DESTROY_ROOM *)buf;

	CGameActor *theActor = m_world->GetGameActorByAID(m_aidList[p->roomID]);
	if (theActor)
		theActor->SendMsg(NULL, AM_DELETE_CHAT_TITLE);
	if (g_windowMgr.GetChatRoomWnd()) {
		if (g_windowMgr.GetChatRoomWnd()->GetRoomId() == p->roomID) {
			g_windowMgr.DeleteWindow(WID_CHATROOMWND);
			g_windowMgr.DeleteWindow(WID_CHATROOMCHANGEWND);
		}
	}
}


inline int UIChatRoomTitle::GetRoomId() { return m_roomInfo.roomId; }

void CGameMode::Zc_Enter_Room(char const *buf)
{
	PACKET_ZC_ENTER_ROOM *p = (PACKET_ZC_ENTER_ROOM *)buf;
	m_isWaitingEnterRoom = FALSE;

	CGameActor *theActor = m_world->GetGameActorByAID(m_aidList[p->roomID]);
	ChatRoomInfo roomInfo;
	bool isRoomTitleExist = false;
	if (theActor)
	{
		UIChatRoomTitle *chatRoomTitle = theActor->GetChatRoomTitle();
		if (chatRoomTitle)
		{
			roomInfo = chatRoomTitle->m_roomInfo;
			if (p->roomID == chatRoomTitle->GetRoomId())
				isRoomTitleExist = true;
		}
		theActor->SendMsg(NULL, AM_DELETE_CHAT_TITLE, 0, 0, 0);
	}
	if (!isRoomTitleExist)
		return;

	int numMember = (p->PacketLength - sizeof(PACKET_ZC_ENTER_ROOM)) / sizeof(ROOM_MEMBER_DATA);
	std::list<RoomMemberData> memberDataList;
	for (int i = 0; i < numMember; ++i)
	{
		ROOM_MEMBER_DATA *member = (ROOM_MEMBER_DATA *)(buf + sizeof(PACKET_ZC_ENTER_ROOM) + sizeof(ROOM_MEMBER_DATA) * i);
		RoomMemberData memData;
		char name[NAME_LENGTH + 1];
		memcpy(name, member->name, NAME_LENGTH);
		name[NAME_LENGTH] = 0;
		memData.name = name;
		memData.role = member->role;
		memberDataList.push_back(memData);
	}

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_CHATROOMWND);
	wnd->SendMsg(NULL, UM_CLEARMEMBER, 0, 0, 0, 0);
	roomInfo.numPeople = memberDataList.size();
	wnd->SendMsg(NULL, UM_SETINFO, int(&roomInfo), 0, 0, 0);

	for (std::list<RoomMemberData>::iterator it = memberDataList.begin(); it != memberDataList.end(); ++it)
	{
		RoomMemberData &data = *it;
		wnd->SendMsg(NULL, UM_ADDMEMBER, int(data.name.c_str()), data.role, 0, 0);
	}

	m_lastChatroomInfo = roomInfo;
}


void CGameMode::Zc_Member_Newentry(char const *buf)
{
	PACKET_ZC_MEMBER_NEWENTRY *p = (PACKET_ZC_MEMBER_NEWENTRY *)buf;

	char memberName[NAME_LENGTH + 1];
	memcpy(memberName, p->name, NAME_LENGTH);
	memberName[NAME_LENGTH] = 0;

	if (strcmp(memberName, g_session.GetCharName()) == 0)
	{
	}
	else
	{
	}

	UIChatRoomWnd *wnd = (UIChatRoomWnd *)g_windowMgr.MakeWindow(WID_CHATROOMWND);
	ChatRoomInfo roomInfo = wnd->m_roomInfo;
	roomInfo.numPeople = p->curcount;
	wnd->SendMsg(NULL, UM_SETINFO, int(&roomInfo), 0, 0, 0);
	wnd->SendMsg(NULL, UM_ADDMEMBER, int(memberName), TRUE, 0, 0);

	char msg[128];
	sprintf(msg, MsgStr(MSI_USERGETINTHEROOM), memberName);
	g_windowMgr.SendMsg(UIM_PUSH_INTO_CHATROOM, int(msg), 0x808000, 0, 0);
}


void CGameMode::Zc_Member_Exit(char const *buf)
{
	PACKET_ZC_MEMBER_EXIT *p = (PACKET_ZC_MEMBER_EXIT *)buf;

	char memberName[NAME_LENGTH + 1];
	memcpy(memberName, p->name, NAME_LENGTH);
	memberName[NAME_LENGTH] = 0;

	if (strcmp(g_session.GetCharName(), memberName) == 0)
	{
		g_windowMgr.DeleteWindow(WID_CHATROOMWND);
		g_windowMgr.DeleteWindow(WID_CHATROOMCHANGEWND);
	}
	else
	{
		UIChatRoomWnd *wnd = (UIChatRoomWnd *)g_windowMgr.MakeWindow(WID_CHATROOMWND);
		ChatRoomInfo roomInfo = wnd->m_roomInfo;
		roomInfo.numPeople = p->curcount;
		wnd->SendMsg(NULL, UM_SETINFO, int(&roomInfo), 0, 0, 0);
		wnd->SendMsg(NULL, UM_MEMBER_EXIT, int(memberName), 0, 0, 0);

		char msg[128];
		switch (p->type)
		{
		case ROOMEXITTYPE_EXIT:
			sprintf(msg, MsgStr(MSI_USERGETOUTOFTHEROOM), memberName);
			g_windowMgr.SendMsg(UIM_PUSH_INTO_CHATROOM, int(msg), 0x80, 0, 0);
			break;
		case ROOMEXITTYPE_EXPEL:
			sprintf(msg, MsgStr(MSI_USERGETOUTOFTHEROOMBYFORCE), memberName);
			g_windowMgr.SendMsg(UIM_PUSH_INTO_CHATROOM, int(msg), 0x80, 0, 0);
			break;
		}
	}
}

void	CGameMode :: Zc_Role_Change(const char *buf)
{
	PACKET_ZC_ROLE_CHANGE *		p = (PACKET_ZC_ROLE_CHANGE *)buf;
	UIWindow *	wnd = g_windowMgr.MakeWindow(WID_CHATROOMWND);
	char memberName[NAME_LENGTH + 1];
	memcpy(memberName, p->name, NAME_LENGTH);
	memberName[NAME_LENGTH] = 0;
	wnd->SendMsg(NULL, UM_ROLE_CHANGE, int(memberName), p->role, 0, 0);
}

void	CGameMode :: Zc_Refuse_Enter_Room(const char *buf)
{
	PACKET_ZC_REFUSE_ENTER_ROOM *	p = (PACKET_ZC_REFUSE_ENTER_ROOM *)buf;
	m_isWaitingEnterRoom = FALSE;
	switch (p->result)
	{
	case	REFUSETYPE_OVERFLOW:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_MANY_PEOPLE_IN_ROOM)), 0xff, 0, 0);
		break;
	case	REFUSETYPE_WRONGPASSWD:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_INCORRECT_PASSWORD)), 0xff, 0, 0);
		break;
	case	REFUSETYPE_EXPEL:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_YOU_HAVE_BANNED_FROM_THE_ROOM)), 0xff, 0, 0);
		break;
	case	REFUSETYPE_ZENY:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_INSUFFICIENT_MONEY)), 0xff, 0, 0);
		break;
	case	REFUSETYPE_LOWLEVEL:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_LEVEL)), 0xff, 0, 0);
		break;
	case	REFUSETYPE_HIGHLEVEL:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_HIGH_LEVEL)), 0xff, 0, 0);
		break;
	case	REFUSETYPE_JOB:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ACCEPTABLE_JOB)), 0xff, 0, 0);
		break;
	}
}


void CGameMode::Zc_Change_Chatroom(char const *buf)
{
	PACKET_ZC_CHANGE_CHATROOM *p = (PACKET_ZC_CHANGE_CHATROOM *)buf;

	m_aidList[p->roomID] = p->AID;
	ChatRoomInfo info;
	info.title.assign((const char *)p + sizeof(PACKET_ZC_CHANGE_CHATROOM), p->PacketLength - sizeof(PACKET_ZC_CHANGE_CHATROOM));
	info.roomType = p->type;
	info.numPeople = p->curcount;
	info.maxNumPeople = p->maxcount;
	info.roomId = p->roomID;
	if (info.title.size() > 60) info.title.resize(60);
	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_CHATROOMWND);
	wnd->SendMsg(NULL, UM_SETINFO, int(&info), 0, 0, 0);
}

void CGameMode::Zc_Req_Exchange_Item(char const *buf)
{
	if (g_session.m_isNoTrade == FALSE)
	{
		PACKET_ZC_REQ_EXCHANGE_ITEM *	p = (PACKET_ZC_REQ_EXCHANGE_ITEM *)buf;
		char name[NAME_LENGTH + 1];
		memcpy(name, p->name, NAME_LENGTH);
		name[NAME_LENGTH] = 0;

		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_EXCHANGEACCEPTWND);
		wnd->SendMsg(NULL, UM_SET_EXCHANGE_CHAR_NAME, int(name), 0, 0, 0);
		m_lastExchangeCharacterName = name;
	}
	else
	{
		CMode *curMode = g_modeMgr.GetCurMode();
		curMode->SendMsg(MM_ACK_EXCHANGE_ITEM, EXI_REQ_REFUSE, 0, 0);
	}
}


void CGameMode::Zc_Req_Exchange_Item2(char const *buf)
{
	if (g_session.m_isNoTrade == FALSE)
	{
		PACKET_ZC_REQ_EXCHANGE_ITEM2 *	p = (PACKET_ZC_REQ_EXCHANGE_ITEM2 *)buf;
		char name[NAME_LENGTH + 1];
		memcpy(name, p->name, NAME_LENGTH);
		name[NAME_LENGTH] = 0;

		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_EXCHANGEACCEPTWND);
		wnd->SendMsg(NULL, UM_SET_EXCHANGE_CHAR_NAME, int(name), p->GID, p->level, 0);
		m_lastExchangeCharacterName = name;
	}
	else
	{
		CMode *curMode = g_modeMgr.GetCurMode();
		curMode->SendMsg(MM_ACK_EXCHANGE_ITEM, EXI_REQ_REFUSE, 0, 0);
	}
}


void CGameMode::Zc_Ack_Exchange_Item(char const *buf)
{
	PACKET_ZC_ACK_EXCHANGE_ITEM *p = (PACKET_ZC_ACK_EXCHANGE_ITEM *)buf;

	switch (p->result)
	{
	case EXI_REQ_ACCEPT:
	{
		SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
		m_isWaitingCancelExchangeItem = TRUE;
		g_session.ClearExchangeItemList();
		g_session.ClearOtherExchangeItemList();
		g_windowMgr.DeleteWindow(WID_EXCHANGEWND);
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_EXCHANGEWND);
		wnd->SendMsg(NULL, UM_SET_EXCHANGE_CHAR_NAME, int(m_lastExchangeCharacterName.c_str()), 0, 0, 0);
		break;
	}
	case EXI_REQ_REFUSE:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REJECT_DEAL)), 0xff, 0, 0);
		break;
	case EXI_REQ_FAILED_DISTANCE:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_FAR_TO_DEAL)), 0xff, 0, 0);
		break;
	case EXI_REQ_FAILED_NOTEXIST:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_NOT_EXIST)), 0xff, 0, 0);
		break;
	case EXI_REQ_FAILED_BE_EXCHANGING:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_DEALING)), 0xff, 0, 0);
		break;
	}
}


void CGameMode::Zc_Ack_Exchange_Item2(char const *buf)
{
	PACKET_ZC_ACK_EXCHANGE_ITEM2 *p = (PACKET_ZC_ACK_EXCHANGE_ITEM2 *)buf;

	switch (p->result)
	{
	case EXI_REQ_ACCEPT:
	{
		SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
		m_isWaitingCancelExchangeItem = TRUE;
		g_session.ClearExchangeItemList();
		g_session.ClearOtherExchangeItemList();
		g_windowMgr.DeleteWindow(WID_EXCHANGEWND);
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_EXCHANGEWND);
		wnd->SendMsg(NULL, UM_SET_EXCHANGE_CHAR_NAME, int(m_lastExchangeCharacterName.c_str()), p->GID, p->level, 0);
		break;
	}
	case EXI_REQ_REFUSE:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REJECT_DEAL)), 0xff, 0, 0);
		break;
	case EXI_REQ_FAILED_DISTANCE:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_FAR_TO_DEAL)), 0xff, 0, 0);
		break;
	case EXI_REQ_FAILED_NOTEXIST:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_NOT_EXIST)), 0xff, 0, 0);
		break;
	case EXI_REQ_FAILED_BE_EXCHANGING:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_DEALING)), 0xff, 0, 0);
		break;
	}
}


void CGameMode::Zc_Add_Exchange_Item(char const *buf)
{
	PACKET_ZC_ADD_EXCHANGE_ITEM *p = (PACKET_ZC_ADD_EXCHANGE_ITEM *)buf;

	if (p->count < 0)
		SendMsg(MM_CANCEL_EXCHANGE_ITEM, 0, 0, 0);

	if (p->ITID == 0)
	{
		g_session.SetOtherExchangeMoney(p->count);
	}
	else
	{
		ITEM_INFO itemInfo;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_itemIndex = -1;
		itemInfo.m_num = p->count;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_refiningLevel = p->refiningLevel;
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		g_session.SetToOtherExchangeList(itemInfo);
	}
}


void CGameMode::Zc_Ack_Add_Exchange_Item(char const *buf)
{
	PACKET_ZC_ACK_ADD_EXCHANGE_ITEM *p = (PACKET_ZC_ACK_ADD_EXCHANGE_ITEM *)buf;
	m_isWaitingAddExchangeItem = FALSE;

	switch (p->result)
	{
	case EXITEM_ADD_SUCCEED:
		if (p->Index)
		{
			ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(p->Index);
			itemInfo.m_num = m_exchangeItemCnt;
			g_session.MoveItemToExchangeList(itemInfo);
		}
		else
		{
			g_session.SetGold(g_session.GetGold() - m_exchangeItemCnt);
			g_session.SetMyExchangeMoney(g_session.GetMyExchangeMoney() + m_exchangeItemCnt);
		}
		break;
	case EXITEM_ADD_FAILED_OVERWEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_OVER_WEIGHT)), int(RGB(255, 0, 0)), 0, 0);
		break;
	}
}

void	CGameMode :: Zc_Conclude_Exchange_Item(const char *buf)
{
	if (!g_windowMgr.GetExchangeWnd())
		return;
	PACKET_ZC_CONCLUDE_EXCHANGE_ITEM *p = (PACKET_ZC_CONCLUDE_EXCHANGE_ITEM *)buf;
	switch (p->who)
	{
	case 0:
		g_windowMgr.GetExchangeWnd()->SendMsg(0, UM_MYDRAGCLOSED, 0, 0, 0, 0);
		break;
	case 1:
		g_windowMgr.GetExchangeWnd()->SendMsg(0, UM_OTHERDRAGCLOSED, 0, 0, 0, 0);
		break;
	}
}

void	CGameMode :: Zc_Cancel_Exchange_Item(const char *buf)
{
	m_isWaitingAddExchangeItem = FALSE;
	SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
	g_windowMgr.DeleteWindow(WID_EXCHANGEWND);
	m_isWaitingCancelExchangeItem = FALSE;
	g_session.CancelExchange();
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DEAL_IS_CANCELED)), 0xff, 0, 0);
}

void	CGameMode :: Zc_Exec_Exchange_Item(const char *buf)
{
	SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
	g_windowMgr.DeleteWindow(WID_EXCHANGEWND);
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

	PACKET_ZC_EXEC_EXCHANGE_ITEM *	p = (PACKET_ZC_EXEC_EXCHANGE_ITEM *)buf;
	switch (p->result)
	{
	case EXITEM_EXCHANGE_SUCCEED:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DEAL_SUCCESS)), int(RGB(0, 255, 255)), 0, 0);
		break;
	case EXITEM_EXCHANGE_FAILED:
		m_isWaitingCancelExchangeItem = FALSE;
		g_session.CancelExchange();
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DEAL_FAILED)), int(RGB(255, 0, 0)), 0, 0);
		break;
	}
}

void	CGameMode :: Zc_Exchangeitem_Undo(const char *buf)
{
	m_isWaitingCancelExchangeItem = FALSE;
	g_session.CancelExchangeNoRefresh();
}


void CGameMode::Zc_Store_Normal_Itemlist(char const *buf)
{
	g_windowMgr.MakeWindow(WID_ITEMSTOREWND);

	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO);
	NORMALITEM_EXTRAINFO *dumpStr = (NORMALITEM_EXTRAINFO *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = 0;
		itemInfo.m_slot[1] = 0;
		itemInfo.m_slot[2] = 0;
		itemInfo.m_slot[3] = 0;
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		g_session.AddStoreItem(itemInfo);
	}

	g_session.RefreshStoreItemWnd();
}

void CGameMode::Zc_Store_Normal_Itemlist2(char const *buf)
{
	g_windowMgr.MakeWindow(WID_ITEMSTOREWND);

	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO2);
	NORMALITEM_EXTRAINFO2 *dumpStr = (NORMALITEM_EXTRAINFO2 *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		g_session.AddStoreItem(itemInfo);
	}

	g_session.RefreshStoreItemWnd();
}

void CGameMode::Zc_Store_Normal_Itemlist3(char const *buf)
{
	g_windowMgr.MakeWindow(WID_ITEMSTOREWND);

	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO3);
	NORMALITEM_EXTRAINFO3 *dumpStr = (NORMALITEM_EXTRAINFO3 *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		g_session.AddStoreItem(itemInfo);
	}

	g_session.RefreshStoreItemWnd();
}

void CGameMode::Zc_Store_Equipment_Itemlist(char const *buf)
{
	g_windowMgr.MakeWindow(WID_ITEMSTOREWND);

	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO);
	EQUIPMENTITEM_EXTRAINFO *dumpStr = (EQUIPMENTITEM_EXTRAINFO *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		g_session.AddStoreItem(itemInfo);
	}

	g_session.RefreshStoreItemWnd();
}

void CGameMode::Zc_Add_Item_To_Store(char const *buf)
{
	const unsigned char* p = (const unsigned char*)buf;
	ITEM_INFO incItem;

	incItem.m_itemIndex = *(const short*)(p + 2);
	incItem.m_num = *(const int*)(p + 4);
	incItem.SetItemId(*(const unsigned short*)(p + 8));
	incItem.m_isIdentified = p[10];
	incItem.m_isDamaged = p[11];
	incItem.m_refiningLevel = p[12];
	incItem.SetType();
	incItem.m_slot[0] = *(const unsigned short*)(p + 13);
	incItem.m_slot[1] = *(const unsigned short*)(p + 15);
	incItem.m_slot[2] = *(const unsigned short*)(p + 17);
	incItem.m_slot[3] = *(const unsigned short*)(p + 19);

	g_session.AddStoreItem(incItem);
}

void	CGameMode :: Zc_Delete_Item_From_Store(const char *buf)
{

	PACKET_ZC_DELETE_ITEM_FROM_STORE *	p = (PACKET_ZC_DELETE_ITEM_FROM_STORE *)buf;
	ITEM_INFO	decItem;
	decItem.m_itemIndex	= p->index;
	decItem.m_num		= p->count;
	g_session.DecStoreItem(decItem);
}

void	CGameMode :: Zc_Close_Store(const char *buf)
{
	g_windowMgr.DeleteWindow(WID_ITEMSTOREWND);
	g_session.ClearStoreItem();
}

void	CGameMode :: Zc_Notify_Storeitem_Countinfo (const char *buf)
{
	PACKET_ZC_NOTIFY_STOREITEM_COUNTINFO *	p = (PACKET_ZC_NOTIFY_STOREITEM_COUNTINFO *)buf;

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_ITEMSTOREWND);
	wnd->SendMsg(NULL, UM_SETCOUNTINFO, p->curCount, p->maxCount, 0, 0);
}

void	CGameMode :: Zc_Ack_Make_Group(const char *buf)
{
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	PACKET_ZC_ACK_MAKE_GROUP *	p = (PACKET_ZC_ACK_MAKE_GROUP *)buf;
	switch (p->result)
	{
	case	GROUP_MAKE_SUCCEED:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_PARTY_MAKE_SUCCEED)), 0xffff00, 0, 0);
		g_windowMgr.MakeWindow(WID_MESSENGERGROUPWND);
		break;
	case	GROUP_MAKE_FAILED_SAMENAME:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SAME_PARTY_NAME)), 0xff, 0, 0);
		break;
	case	GROUP_MAKE_FAILED_ALEADY_GROUP_MEMBER:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_YOU_ARE_ALREADY_IN_PARTY)), 0xff, 0, 0);
		break;
	}
}


void CGameMode::Zc_Group_List(char const *buf)
{
	g_session.ClearParty();

	PACKET_ZC_GROUP_LIST *p = (PACKET_ZC_GROUP_LIST *)buf;
	int packetStrLen = sizeof(PACKET_ZC_GROUP_LIST);

	char groupName[NAME_LENGTH + 1];
	memcpy(groupName, p->groupName, NAME_LENGTH);
	groupName[NAME_LENGTH] = 0;
	g_session.m_partyName = groupName;

	if (p->PacketLength == packetStrLen)
		return;

	int groupInfoLen = sizeof(GROUPMEMBER_INFO);
	int loopCnt = (p->PacketLength - packetStrLen) / groupInfoLen;

	for (int i = 0; i < loopCnt; ++i) {
		GROUPMEMBER_INFO info;
		memcpy(&info, buf + packetStrLen + groupInfoLen * i, sizeof(GROUPMEMBER_INFO));

		char characterName[NAME_LENGTH + 1];
		char mapName[17];
		memcpy(characterName, info.characterName, NAME_LENGTH);
		memcpy(mapName, info.mapName, 16);
		characterName[NAME_LENGTH] = 0;
		mapName[16] = 0;

		FRIEND_INFO fInfo;
		fInfo.AID = info.AID;
		fInfo.characterName = characterName;
		fInfo.mapName = mapName;
		fInfo.role = info.role;
		fInfo.state = info.state;

		g_session.AddMemberToParty(fInfo);

		if (fInfo.AID == g_session.GetAid() && fInfo.role == 0)
			g_session.m_amIPartyMaster = true;
	}

	g_session.RefreshPartyUI();
}

void	CGameMode :: Zc_Ack_Req_Join_Group(const char *buf)
{
	PACKET_ZC_PARTY_JOIN_REQ_ACK *	p = (PACKET_ZC_PARTY_JOIN_REQ_ACK *)buf;
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

	switch (p->answer)
	{
	case 0:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_ALREADY_IN_PARTY)),  0x0000ff, 0, 0);
		break;
	case 1:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_REJECT_JOIN)),  0x0000ff, 0, 0);
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_ACCEPT_JOIN)),  0xffff00, 0, 0);
		break;
	case 3:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_MANY_PEOPLE_IN_PARTY)),  0x0000ff, 0, 0);
		break;
	case 4:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ALREADY_SAME_AID_JOINED)), 0x0000ff, 0, 0);
		break;
	}

	switch (*(int *)(buf + 0x1a))
	{
	case 5:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_JOINMSG_REFUSE)), 0x0000ff, 0, 0);
		break;
	case 7:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_NOT_EXIST)),  0x0000ff, 0, 0);
		break;
	}
}


void CGameMode::Zc_Req_Join_Group(char const *buf)
{
	PACKET_ZC_REQ_JOIN_GROUP* p = (PACKET_ZC_REQ_JOIN_GROUP*)buf;

	char groupName[NAME_LENGTH + 1];
	strncpy(groupName, p->groupName, NAME_LENGTH);
	groupName[NAME_LENGTH] = 0;

	unsigned int nameLength = strlen(groupName);
	g_session.m_partyName.assign(groupName, nameLength);

	UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_JOINPARTYACCEPTWND);
	wnd->SendMsg(NULL, UM_SET_JOIN_PARTY_MSG, p->GRID, (int)groupName, 0, 0);
}

void	CGameMode :: Zc_Req_Couple(const char *buf)
{
	PACKET_ZC_REQ_COUPLE *		p = (PACKET_ZC_REQ_COUPLE *)buf;
	char	TheName[NAME_LENGTH+1];
	strncpy(TheName, p->name, NAME_LENGTH);
	m_lastCouplePacketAid = p->AID;
	TheName[NAME_LENGTH] = 0;
	m_lastCouplePacketGid = p->GID;
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_COUPLEACCEPTWND);
	wnd->SendMsg(NULL, UM_SET_COUPLE_MSG, p->GID, (int)TheName, 0, 0);
}

void	CGameMode :: Zc_Req_Baby(const char *buf)
{
	PACKET_ZC_REQ_BABY *		p = (PACKET_ZC_REQ_BABY *)buf;
	char	TheName[NAME_LENGTH+1];
	strncpy(TheName, p->name, NAME_LENGTH);
	TheName[NAME_LENGTH] = 0;
	m_lastCouplePacketAid = p->AID;
	m_lastCouplePacketGid = p->GID;
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_BABYACCEPTWND);
	wnd->SendMsg(NULL, UM_SET_BABY_MSG, p->GID, (int)TheName, 0, 0);
}

void	CGameMode :: Zc_StarPlace(const char *buf)
{
	PACKET_ZC_STARPLACE *		p = (PACKET_ZC_STARPLACE *)buf;

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_STARPLACE_ACCEPTWND);
	wnd->SendMsg(NULL, UM_SET_BABY_MSG, p->which, 0, 0, 0);
}

void	CGameMode :: Zc_NotifyBindOnEquip(const char *buf)
{
	PACKET_ZC_NOTIFY_BIND_ON_EQUIP *	p = (PACKET_ZC_NOTIFY_BIND_ON_EQUIP *)buf;
	ITEM_INFO	indexItem = g_session.GetItemInfoByIndex(p->index);
	if (indexItem.m_itemIndex) {
		char	nItemName[128];
		indexItem.GetItemName(nItemName);
		char	msg[256];
		sprintf(msg, MsgStr(MSI_YOURITEM_EQUIPED), nItemName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x6effff, 0, 0);
	}
}


void	CGameMode :: Zc_Battlefield_Notify_Campinfo(const char *buf)
{
	PACKET_ZC_BATTLEFIELD_NOTIFY_CAMPINFO *		p = (PACKET_ZC_BATTLEFIELD_NOTIFY_CAMPINFO *)buf;

	if (p->accountID == g_session.GetAid())
	{
		g_session.m_camp = p->camp;
	}
	else
	{
		CGameActor *	theActor = m_world->GetGameActorByAID(p->accountID);
		if (theActor)
		{
			theActor->m_camp = p->camp;
			if (theActor->m_camp != g_session.m_camp)
			{
				MakeWordDisplayWnd(theActor);
			}
		}
	}
}

void	CGameMode :: Zc_Notify_Font(const char *buf)
{
	PACKET_ZC_NOTIFY_FONT *		p = (PACKET_ZC_NOTIFY_FONT *)buf;

	if (p->AID == g_session.GetAid())
	{
		g_session.m_charfont = p->font;
		return;
	}

	CGameActor *	theActor = m_world->GetGameActorByAID(p->AID);
	if (theActor)
	{
		theActor->m_charfont = p->font;
	}
}

void	CGameMode :: Zc_Progress(const char *buf)
{
	PACKET_ZC_PROGRESS *		p = (PACKET_ZC_PROGRESS *)buf;

	CGameActor *	sActor = NULL;
	sActor = m_world->m_player;
	if (sActor != NULL)
	{
		sActor->SendMsg(NULL, AM_MAKE_SKILL_RECHARGEGAGE, p->time * 1000, 0, 0);
		g_session.m_GaugePacket=1;
	}
}

void	CGameMode :: Zc_Progress_Cancel(const char *buf)
{
	CGameActor *	sActor = m_world->m_player;
	if (sActor != NULL)
	{
		sActor->SendMsg(NULL, AM_DELETE_SKILL_RECHARGEGAGE, 0, 0, 0);
	}
}


void CGameMode::Zc_Battlefield_Chat(char const *buf)
{
	PACKET_ZC_BATTLEFIELD_CHAT* p = (PACKET_ZC_BATTLEFIELD_CHAT*)buf;

	if (g_session.IsExAid(p->accountID))
		return;

	char chatMsg[256];
	int msgLen = p->PacketLength - sizeof(PACKET_ZC_BATTLEFIELD_CHAT);
	memcpy(chatMsg, buf + sizeof(PACKET_ZC_BATTLEFIELD_CHAT), msgLen);

	if (IsNameSpecific(p->accountID))
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), IsNameSpecific(p->accountID), 0, 0);
	}
	else
	{
		if (g_session.m_isNoTalkMsg == TRUE)
		{
		}
		else if (g_session.m_isNoTalkMsg2 == TRUE)
		{
		}
		else
		{
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), int(RGB(255, 0, 155)), NMSG_CHAT, 0);
		}
	}

	CGameActor* theActor = GetWorld()->GetGameActorByAID(p->accountID);
	if (theActor)
	{
		theActor->SendMsg(NULL, AM_CHAT, int(chatMsg), 0, 0);
	}

	if (g_session.m_isSaveChat)
	{
		WriteChat(chatMsg);
	}
}

void	CGameMode :: Zc_Battlefield_Notify_Point(const char *buf)
{
	PACKET_ZC_BATTLEFIELD_NOTIFY_POINT *		p = (PACKET_ZC_BATTLEFIELD_NOTIFY_POINT *)buf;

	g_session.m_camp_A = p->pointCampA;
	g_session.m_camp_B = p->pointCampB;

	char	msgZ[100];
	sprintf(msgZ, "[ Camp A = %d , Camp B = %d ]", p->pointCampA, p->pointCampB);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msgZ), 0xc8c8c8, 0, 0);
}

void	CGameMode :: Zc_Battlefield_Notify_Position(const char *buf)
{
	PACKET_ZC_BATTLEFIELD_NOTIFY_POSITION *		p = (PACKET_ZC_BATTLEFIELD_NOTIFY_POSITION *)buf;

	if (p->accountID != g_session.GetAid())
		SendMsg(MM_ADDPARTYPOS, p->accountID, p->x, p->y);
}

void	CGameMode :: Zc_Battlefield_Notify_Hp(const char *buf)
{
	PACKET_ZC_BATTLEFIELD_NOTIFY_HP *		p = (PACKET_ZC_BATTLEFIELD_NOTIFY_HP *)buf;

	if (p->accountID == g_session.m_merGID)
		return;

	CGameActor *	theActor = m_world->GetGameActorByAID(p->accountID);
	if (theActor)
	{
		theActor->SendMsg(NULL, AM_UPDATEGAGE, p->hp, p->maxHp, 0);
	}
}


void CGameMode::Zc_StarSkill(char const *buf)
{
	char msg2[256];
	PACKET_ZC_STARSKILL *p = (PACKET_ZC_STARSKILL *)buf;
	CGameActor *player = m_world->m_player;

	m_broadCastTick = timeGetTime();
	if (m_broadcastBalloon == NULL)
	{
		m_broadcastBalloon = new UITransBalloonText;
		m_broadcastBalloon->Create(8, 8);
		g_windowMgr.AddWindowFront(m_broadcastBalloon);
	}

	if (p->result == 0)
	{
		if (p->star == 0)
			sprintf(msg2, MsgStr(MSI_STARPLACE1), g_session.GetCharName(), p->mapName);
		else if (p->star == 1)
			sprintf(msg2, MsgStr(MSI_STARPLACE2), g_session.GetCharName(), p->mapName);
		else if (p->star == 2)
			sprintf(msg2, MsgStr(MSI_STARPLACE3), g_session.GetCharName(), p->mapName);

		m_broadcastBalloon->SetText(msg2, 72);
	}
	else if (p->result == 1)
	{
		if (p->star == 0)
			sprintf(msg2, MsgStr(MSI_STARPLACE4), g_session.GetCharName(), p->mapName);
		else if (p->star == 1)
			sprintf(msg2, MsgStr(MSI_STARPLACE5), g_session.GetCharName(), p->mapName);
		else if (p->star == 2)
			sprintf(msg2, MsgStr(MSI_STARPLACE6), g_session.GetCharName(), p->mapName);

		m_broadcastBalloon->SetText(msg2, 72);
		player->LaunchEffect(EF_HATED);
		PlayWave("effect\\T_\xc7\xc7\xb8\xb5.wav", 0, -250, 0);
	}
	else if (p->result == 10)
	{
		if (p->mapName[0] == 0)
			sprintf(p->mapName, "%s", g_session.GetJobName(p->monsterID));

		if (p->star == 0)
			sprintf(msg2, MsgStr(MSI_STARMONSTER1), g_session.GetCharName(), p->mapName);
		else if (p->star == 1)
			sprintf(msg2, MsgStr(MSI_STARMONSTER2), g_session.GetCharName(), p->mapName);
		else if (p->star == 2)
			sprintf(msg2, MsgStr(MSI_STARMONSTER3), g_session.GetCharName(), p->mapName);

		m_broadcastBalloon->SetText(msg2, 72);
	}
	else if (p->result == 11)
	{
		if (p->mapName[0] == 0)
			sprintf(p->mapName, "%s", g_session.GetJobName(p->monsterID));

		if (p->star == 0)
			sprintf(msg2, MsgStr(MSI_STARMONSTER4), g_session.GetCharName(), p->mapName);
		else if (p->star == 1)
			sprintf(msg2, MsgStr(MSI_STARMONSTER5), g_session.GetCharName(), p->mapName);
		else if (p->star == 2)
			sprintf(msg2, MsgStr(MSI_STARMONSTER6), g_session.GetCharName(), p->mapName);

		m_broadcastBalloon->SetText(msg2, 72);
		player->LaunchEffect(EF_HATED);
		PlayWave("effect\\T_\xc7\xc7\xb8\xb5.wav", 0, -250, 0);
	}
	else if (p->result == 20)
	{
		sprintf(msg2, MsgStr(MSI_TAEKWON_MISSION), p->mapName, p->star);
		m_broadcastBalloon->SetText(msg2, 72);
		player->LaunchEffect(EF_HATED);
	}
	else if (p->result == 21)
	{
		sprintf(msg2, MsgStr(MSI_GANGSI_MISSION), p->mapName);
		m_broadcastBalloon->SetText(msg2, 72);
		player->LaunchEffect(EF_HATED);
	}
	else if (p->result == 22)
	{
		ITEM_INFO item;
		char itemDisName[256];
		item.SetItemId(p->monsterID);
		item.GetIdDisplayName(itemDisName);

		sprintf(msg2, MsgStr(MSI_COLLECTOR_MISSION), itemDisName);
		m_broadcastBalloon->SetText(msg2, 72);
		player->LaunchEffect(EF_HATED);
	}
	else if (p->result == 30)
	{
		sprintf(msg2, MsgStr(MSI_STAR_ANGEL));
		m_broadcastBalloon->SetText(msg2, 72);
		player->LaunchEffect(EF_ELECTRIC);
	}
	else if (p->result == 40)
	{
		sprintf(msg2, "Target HP : %d", p->monsterID);
		m_broadcastBalloon->SetText(msg2, 72);
	}

	m_broadcastBalloon->SetFntSize(14);
	m_broadcastBalloon->Move((g_renderer->m_width - 640) / 2 - m_broadcastBalloon->GetWidth() / 2 + 320, 51);
	m_broadcastBalloon->SetFntColor(RGB(200, 200, 255), RGB(0, 0, 0));
}


void CGameMode::Zc_Ack_Status_GM(char const *buf)
{
	PACKET_ZC_ACK_STATUS_GM *	p = (PACKET_ZC_ACK_STATUS_GM *)buf;
	char msg2[256];

	sprintf(msg2, "             STR=%3d      AGI=%3d      VIT=%3d      INT=%3d      DEX=%3d      LUK=%d",
		p->str, p->agi, p->vit,
		p->Int, p->dex, p->luk);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xffff, 0, 0);

	sprintf(msg2, "standard STR=%3d      AGI=%3d      VIT=%3d      INT=%3d      DEX=%3d      LUK=%2d",
		p->standardStr, p->standardAgi, p->standardVit,
		p->standardInt, p->standardDex, p->standardLuk);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xff37, 0, 0);

	sprintf(msg2, "    attPower=%3d    refiningPow=%3d    MAXmatPow=%3d    MINmatPower=%3d       ASPD=%3d",
		p->attPower, p->refiningPower, p->max_mattPower,
		p->min_mattPower, p->ASPD);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xffff, 0, 0);

	sprintf(msg2, "itemdefPow=%3d     plusdefPow=%3d    mdefPower=%3d    plusmdefPow=%3d    plusASPD=%3d",
		p->itemdefPower, p->plusdefPower, p->mdefPower,
		p->plusmdefPower, p->plusASPD);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xff37, 0, 0);

	sprintf(msg2, "hitSuccessVal=%3d    avoidSuccessVal=%3d    plusAvoidSuccessValue=%3d",
		p->hitSuccessValue, p->avoidSuccessValue, p->plusAvoidSuccessValue);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xffff, 0, 0);
}

void CGameMode::Zc_Add_Member_To_Group(char const *buf)
{
	PACKET_ZC_ADD_MEMBER_TO_GROUP *p = (PACKET_ZC_ADD_MEMBER_TO_GROUP *)buf;
	char characterName[NAME_LENGTH + 1];
	memcpy(characterName, p->characterName, NAME_LENGTH);
	characterName[NAME_LENGTH] = 0;

	char groupName[NAME_LENGTH + 1];
	strncpy(groupName, p->groupName, NAME_LENGTH);
	groupName[NAME_LENGTH] = 0;
	g_session.m_partyName = groupName;

	char mapName[17];
	memcpy(mapName, p->mapName, 16);
	mapName[16] = 0;

	FRIEND_INFO fInfo;
	fInfo.characterName = characterName;
	fInfo.AID = p->AID;
	fInfo.mapName = mapName;
	fInfo.role = p->Role;
	fInfo.state = p->state;

	if (fInfo.AID == g_session.GetAid())
	{
		if (fInfo.role == GROUPMEMBER_ROLE_MASTER)
			g_session.m_amIPartyMaster = true;
	}
	else
	{
		if (fInfo.state == GROUPMEMBER_STATE_DISCONNEDTED)
		{
			SendMsg(MM_DELETEPARTYPOS, p->AID);
		}
		else
		{
			std::string mapName = fInfo.mapName;
			std::string mapName2 = m_rswName;

			if (mapName.size() > 4)
				mapName.resize(mapName.size() - 3);
			if (mapName2.size() > 4)
				mapName2.resize(mapName2.size() - 3);

			if (mapName != mapName2)
				SendMsg(MM_DELETEPARTYPOS, p->AID);
		}
	}

	g_session.AddMemberToParty(fInfo);
	g_session.RefreshPartyUI();
}

void CGameMode::Zc_Add_Member_To_Group2(char const *buf)
{
	PACKET_ZC_ADD_MEMBER_TO_GROUP2 *p = (PACKET_ZC_ADD_MEMBER_TO_GROUP2 *)buf;
	char characterName[NAME_LENGTH + 1];
	memcpy(characterName, p->characterName, NAME_LENGTH);
	characterName[NAME_LENGTH] = 0;

	char groupName[NAME_LENGTH + 1];
	strncpy(groupName, p->groupName, NAME_LENGTH);
	groupName[NAME_LENGTH] = 0;
	g_session.m_partyName = groupName;

	char mapName[17];
	memcpy(mapName, p->mapName, 16);
	mapName[16] = 0;

	FRIEND_INFO fInfo;
	fInfo.characterName = characterName;
	fInfo.AID = p->AID;
	fInfo.mapName = mapName;
	fInfo.role = p->Role;
	fInfo.state = p->state;

	if (fInfo.AID == g_session.GetAid())
	{
		if (fInfo.role == GROUPMEMBER_ROLE_MASTER)
			g_session.m_amIPartyMaster = true;
	}
	else
	{
		if (fInfo.state == GROUPMEMBER_STATE_DISCONNEDTED)
		{
			SendMsg(MM_DELETEPARTYPOS, p->AID);
			SetMemberVisibleState(p->AID, FALSE);
		}
		else
		{
			std::string mapName = fInfo.mapName;
			std::string mapName2 = m_rswName;

			if (mapName.size() > 4)
				mapName.resize(mapName.size() - 3);
			if (mapName2.size() > 4)
				mapName2.resize(mapName2.size() - 3);

			if (mapName != mapName2)
			{
				SendMsg(MM_DELETEPARTYPOS, p->AID);
				SetMemberVisibleState(p->AID, FALSE);
			}
			else
			{
				SetMemberVisibleState(p->AID, TRUE);
			}
		}
	}

	g_session.m_itemDivType = p->ItemDivisionRule != 0;
	g_session.m_itemCollectType = p->ItemPickupRule != 0;
	if (g_session.GetNumParty())
	{
	}

	g_session.AddMemberToParty(fInfo);
	g_session.RefreshPartyUI();
	SetMemberVisibleState(p->AID, TRUE);
}


void CGameMode::Zc_Delete_Member_From_Group(char const *buf)
{
	PACKET_ZC_DELETE_MEMBER_FROM_GROUP *p = (PACKET_ZC_DELETE_MEMBER_FROM_GROUP *)buf;
	char characterName[NAME_LENGTH + 1];
	memcpy(characterName, p->characterName, NAME_LENGTH);
	characterName[NAME_LENGTH] = 0;

	SendMsg(MM_DELETEPARTYPOS, g_session.GetMemberAidFromParty(characterName));

	switch (p->result)
	{
	case GROUPMEMBER_DELETE_LEAVE:
		g_session.DeleteMemberFromParty(characterName);
		break;
	case GROUPMEMBER_DELETE_EXPEL:
		g_session.DeleteMemberFromParty(characterName);
		break;
	}

	if (strcmp(g_session.GetCharName(), characterName) == 0)
	{
		g_session.m_amIPartyMaster = false;
		g_session.ClearParty();
		g_session.m_partyName = "";
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_YOU_LEAVE_FROM_PARTY)), int(RGB(0, 255, 255)), 0, 0);
		g_windowMgr.DeleteWindow(WID_PARTYSETTINGWND);
		SendMsg(MM_DELETEPARTYPOSALL);
		if (g_windowMgr.GetMemorialDunWnd())
			g_windowMgr.DeleteWindow(WID_MEMORIAL_DUN_WND);
	}

	g_session.RefreshPartyUI();
	SetMemberVisibleState(p->AID, FALSE);
}

void	CGameMode :: Zc_Notify_Hp_To_Groupm (const char *buf)
{
	PACKET_ZC_NOTIFY_HP_TO_GROUPM *	p = (PACKET_ZC_NOTIFY_HP_TO_GROUPM *)buf;
	if (p->AID == g_session.m_merGID)
		return;

	CGameActor *	theActor = m_world->GetGameActorByAID(p->AID);
	if (theActor)
		theActor->SendMsg(NULL, AM_UPDATEGAGE, p->hp, p->maxhp, 0);

	UIWindow *	groupWnd = (UIWindow *)g_windowMgr.GetMessengerGroupWnd();
	if (groupWnd)
		groupWnd->SendMsg(NULL, UM_COMMAND, AM_UPDATEGAGE, p->AID, p->hp, p->maxhp);
}

void	CGameMode :: Zc_Notify_Position_To_Groupm(const char *buf)
{
	PACKET_ZC_NOTIFY_POSITION_TO_GROUPM *	p = (PACKET_ZC_NOTIFY_POSITION_TO_GROUPM *)buf;
	SendMsg(MM_ADDPARTYPOS, p->AID, p->xPos, p->yPos);
}

void	CGameMode :: Zc_Notify_Position_To_Guildm(const char *buf)
{
	PACKET_ZC_NOTIFY_POSITION_TO_GUILDM *	p = (PACKET_ZC_NOTIFY_POSITION_TO_GUILDM *)buf;

	if (p->AID != g_session.GetAid())
		SendMsg(MM_ADDGUILDPOS, p->AID, p->xPos, p->yPos);
}

void	CGameMode :: Zc_Guild_Member_Map_Change(const char *buf)
{
	PACKET_ZC_GUILD_MEMBER_MAP_CHANGE *	p = (PACKET_ZC_GUILD_MEMBER_MAP_CHANGE *)buf;
	if (0 != strcmp(g_session.m_curMap, p->mapName))
	{
		SendMsg(MM_DELETEGUILDPOS, p->AID);
		SetMemberVisibleState(p->AID, FALSE);
	}
	else
	{
		SetMemberVisibleState(p->AID, TRUE);
	}
}
void	CGameMode :: SetMemberVisibleState(DWORD AID, BOOL bIsVisible)
{
	CGameActor *	theActor = GetWorld()->GetGameActorByAID(AID);
	if (theActor) {
		theActor->SetGroupMemberAndVisible(bIsVisible);
	}
}

void	CGameMode :: Zc_Notify_Chat_Party(const char *buf)
{
	PACKET_ZC_NOTIFY_CHAT_PARTY *	p = (PACKET_ZC_NOTIFY_CHAT_PARTY *)buf;
	char	chatMsg[256];
	int		msgLen = p->PacketLength - sizeof(PACKET_ZC_NOTIFY_CHAT_PARTY);
	memcpy(chatMsg, buf + sizeof(PACKET_ZC_NOTIFY_CHAT_PARTY), msgLen);
	if (p->AID == g_session.GetAid()) {
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xc8ff, 3, 0);
		m_world->m_player->SendMsg(NULL, AM_CHAT, int(chatMsg), 0, 0);
	}
	else {
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xc8c8ff, 3, 0);
		CGameActor *	theActor = GetWorld()->GetGameActorByAID(p->AID);
		if (theActor)
			theActor->SendMsg(NULL, AM_CHAT, int(chatMsg), 0, 0);
	}
	if (g_session.m_isSaveChat)
		WriteChat(chatMsg);
}


void CGameMode::Zc_Mvp_Getting_Item(char const *buf)
{
	PACKET_ZC_MVP_GETTING_ITEM *p = (PACKET_ZC_MVP_GETTING_ITEM *)buf;
	char itemName[NAME_LENGTH + 1];
	itoa(p->ITID, itemName, 10);
	itemName[NAME_LENGTH] = 0;

	char msg[256];
	ITEM_INFO dummyItemInfo;
	dummyItemInfo.m_itemName = itemName;
	dummyItemInfo.m_refiningLevel = 0;
	dummyItemInfo.m_isIdentified = 0;

	char removedItemName[64];
	dummyItemInfo.GetItemName(removedItemName);
	sprintf(msg, "%s%s", MsgStr(MSI_YOU_RECEIVE_MVP_ITEM), removedItemName, MsgStr(MSI_YOU_RECEIVE_MVP_ITEM2));
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(0, 255, 255)), 0, 0);
}

void	CGameMode :: Zc_Mvp_Getting_Special_Exp(const char *buf)
{
	PACKET_ZC_MVP_GETTING_SPECIAL_EXP *		p = (PACKET_ZC_MVP_GETTING_SPECIAL_EXP *)buf;
	char	msg[256];
	sprintf(msg, "%s%d%s", MsgStr(MSI_YOU_RECEIVE_MVP_EXP), p->exp, MsgStr(MSI_YOU_RECEIVE_MVP_EXP2));
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 0, 0);
}

void	CGameMode :: Zc_Mvp(const char *buf)
{
	PACKET_ZC_MVP *		p = (PACKET_ZC_MVP *)buf;
	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_MVP, 0, 0, 0);
	}
	else
	{
		CGameActor *theActor = GetWorld()->GetGameActorByAID(p->AID);
		if (theActor)
			theActor->SendMsg(NULL, AM_MVP, 0, 0, 0);
	}
}

void	CGameMode :: Zc_Throw_Mvpitem(const char *buf)
{
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_YOU_THROW_MVPITEM)), 0xff, 0, 0);
}


void CGameMode::Zc_Skillinfo_Update(char const *buf)
{
	m_isReqUpgradeSkillLevel = FALSE;
	PACKET_ZC_SKILLINFO_UPDATE *p = (PACKET_ZC_SKILLINFO_UPDATE *)buf;
	SKILL_INFO skillInfo;

	if (p->PacketType == HEADER_ZC_SKILLINFO_UPDATE)
		skillInfo = g_session.GetSkillItemInfoBySkillId(p->SKID);
	else if (p->PacketType == HEADER_ZC_HOSKILLINFO_UPDATE)
		skillInfo = g_session.GetHomunSkillItemInfoBySkillId(p->SKID);
	else if (p->PacketType == HEADER_ZC_MER_SKILLINFO_UPDATE)
		skillInfo = g_session.GetMerSkillItemInfoBySkillId(p->SKID);

	skillInfo.level = p->level;
	skillInfo.SKID = p->SKID;
	skillInfo.spcost = p->spcost;
	skillInfo.upgradable = p->upgradable;
	skillInfo.attackRange = p->attackRange;

	if (p->PacketType == HEADER_ZC_SKILLINFO_UPDATE)
	{
		g_session.AddSkillItem(skillInfo, true);
		g_session.RefreshSkillItemWnd();
	}
	else if (p->PacketType == HEADER_ZC_HOSKILLINFO_UPDATE)
	{
		g_session.AddHomunSkillItem(skillInfo, true);
		g_session.RefreshHomunSkillItemWnd();
	}
	else if (p->PacketType == HEADER_ZC_MER_SKILLINFO_UPDATE)
	{
		g_session.AddMerSkillItem(skillInfo, true);
		g_session.RefreshMerSkillItemWnd();
	}
}


void CGameMode::Zc_Skillinfo_List(char const *buf)
{
	char PASS;

	if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_SKILLINFO_LIST)
		g_session.ClearSkillItem();
#if defined (_HOMUN)
	else if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_HOSKILLINFO_LIST)
		g_session.ClearHomunSkillItem();
#endif
#if defined (_MER_R2)
	else if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_MER_SKILLINFO_LIST)
		g_session.ClearMerSkillItem();
#endif

	sAssert((((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketLength - sizeof(PACKET_ZC_SKILLINFO_LIST)) % sizeof(SKILLINFO) == 0);
	SKILLINFO *skill = (SKILLINFO *)(buf + sizeof(PACKET_ZC_SKILLINFO_LIST));

	PASS = 0;

	for (int i = 0; i < (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketLength - sizeof(PACKET_ZC_SKILLINFO_LIST)) / sizeof(SKILLINFO); ++i)
	{
		SKILL_INFO skillInfo;
		if ((skill[i].SKID >= SKID_NV_BASIC && skill[i].SKID < 0x40f) ||
			(skill[i].SKID >= SKID_HOMUN_BEGIN && skill[i].SKID < SKID_HOMUN_LAST) ||
			(skill[i].SKID >= SKID_MERCENARY_BEGIN && skill[i].SKID < SKID_MERCENARY_LAST))
		{
			skillInfo.SKID = skill[i].SKID;
			skillInfo.type = skill[i].type;
			skillInfo.level = skill[i].level;
			skillInfo.spcost = skill[i].spcost;
			skillInfo.upgradable = skill[i].upgradable;
			skillInfo.attackRange = skill[i].attackRange;
			skillInfo.skillName = skillInfo.GetSkillIdName();

			if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_SKILLINFO_LIST)
				g_session.AddSkillItem(skillInfo, false);
#if defined (_HOMUN)
			else if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_HOSKILLINFO_LIST)
				g_session.AddHomunSkillItem(skillInfo, false);
#endif
#if defined (_MER_R2)
			else if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_MER_SKILLINFO_LIST)
				g_session.AddMerSkillItem(skillInfo, false);
#endif

#if defined(WEDDING_PATCH)
			if (skill[i].SKID == SKID_WE_CALLPARTNER && PASS == 0)
				PASS = 1;
			if (skill[i].SKID == SKID_WE_CALLBABY || skill[i].SKID == SKID_WE_CALLPARENT)
				PASS = 2;
#endif
		}
	}

	if (PASS == 1)
		g_session.m_amIParent = true;
	else
		g_session.m_amIParent = false;

	if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_SKILLINFO_LIST)
		g_session.RefreshSkillItemWnd();
#if defined (_HOMUN)
	else if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_HOSKILLINFO_LIST)
		g_session.RefreshHomunSkillItemWnd();
#endif
#if defined (_MER_R2)
	else if (((PACKET_ZC_SKILLINFO_LIST *)buf)->PacketType == HEADER_ZC_MER_SKILLINFO_LIST)
		g_session.RefreshMerSkillItemWnd();
#endif
}

void	CGameMode :: Zc_All_Quest_List(const char *buf)
{
	PACKET_ZC_ALL_QUEST_LIST *p = (PACKET_ZC_ALL_QUEST_LIST *)buf;
	PACKET_ZC_QUEST_INFO *quest = (PACKET_ZC_QUEST_INFO *)(buf + sizeof(PACKET_ZC_ALL_QUEST_LIST));

	g_session.m_QuestList.clear();

	for (int i = 0; i < (p->PacketLength - sizeof(PACKET_ZC_ALL_QUEST_LIST)) / sizeof(PACKET_ZC_QUEST_INFO); ++i)
	{
		QUEST_INFO questinfo;
		questinfo.questID = quest[i].questID;
		questinfo.show = quest[i].active;

		g_session.AddQuestList(questinfo, false);
	}

	g_session.RefreshQuestWnd();
}


void CGameMode::Zc_All_Quest_Mission(char const *buf)
{
	PACKET_ZC_ALL_QUEST_MISSION *p = (PACKET_ZC_ALL_QUEST_MISSION *)buf;
	PACKET_ZC_QUEST_MISSION_INFO *quest = (PACKET_ZC_QUEST_MISSION_INFO *)(buf + sizeof(PACKET_ZC_ALL_QUEST_MISSION));

	for (int i = 0; i < (p->PacketLength - sizeof(PACKET_ZC_ALL_QUEST_MISSION)) / sizeof(PACKET_ZC_QUEST_MISSION_INFO); ++i)
	{
		std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
		while (it != g_session.m_QuestList.end())
		{
			if (it->questID == quest[i].questID)
			{
				struct tm *qtime, *qtime2;
				qtime = localtime(&quest[i].quest_endTime);

				it->m_mobTime_Year = qtime->tm_year + 1900;
				it->m_mobTime_Month = qtime->tm_mon + 1;
				it->m_mobTime_Day = qtime->tm_mday;
				it->m_mobTime_Hour = qtime->tm_hour;
				it->m_mobTime_Minute = qtime->tm_min;

				if (quest[i].quest_endTime == 0)
				{
					it->m_mobTime_Hour = 0;
					it->m_mobTime_Minute = 0;
				}

				qtime2 = localtime(&quest[i].quest_svrTime);

				it->mobCount = quest[i].count;
				for (int j = 0; j < quest[i].count; ++j)
				{
					it->mobID[j] = quest[i].hunt[j].mobGID;
					it->mobHunt[j] = quest[i].hunt[j].huntCount;
					memcpy(it->mobName[j], quest[i].hunt[j].mobName, 51);
				}
				break;
			}
			it++;
		}
	}
}

void	CGameMode :: Zc_Update_Mission_Hunt(const char *buf)
{
	PACKET_ZC_UPDATE_MISSION_HUNT *	p = (PACKET_ZC_UPDATE_MISSION_HUNT *)buf;

	PACKET_MOB_HUNTING *	mobhunting = (PACKET_MOB_HUNTING *)(buf + sizeof(PACKET_ZC_UPDATE_MISSION_HUNT));

	for (int i = 0; i < (p->PacketLength - sizeof(PACKET_ZC_UPDATE_MISSION_HUNT)) / sizeof(PACKET_MOB_HUNTING); ++i)
	{
		std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
		while (it != g_session.m_QuestList.end())
		{
			if (it->questID == mobhunting[i].questID)
			{
				for (int j = 0; j < 3; j++)
				{
					if (it->mobID[j] == mobhunting[i].mobGID)
					{
						it->mobHunt[j] = mobhunting[i].count;
						break;
					}
				}
			}
			it++;
		}
	}
	g_session.RefreshQuestDetailWnd();
}

void	CGameMode :: Zc_Active_Quest(const char *buf)
{
	PACKET_CZ_ACTIVE_QUEST *	p = (PACKET_CZ_ACTIVE_QUEST *)buf;

	std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
	while (it != g_session.m_QuestList.end())
	{
		if (it->questID == p->questID)
		{
			it->show = p->active;
			break;
		}
		it++;
	}

	g_session.RefreshQuestWnd();
}


void CGameMode::Zc_Add_Quest(char const *buf)
{
	PACKET_ZC_ADD_QUEST *p = (PACKET_ZC_ADD_QUEST *)buf;

	std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
	while (it != g_session.m_QuestList.end())
	{
		if (it->questID == p->questID)
		{
			return;
		}
		it++;
	}

	QUEST_INFO questinfo;
	questinfo.questID = p->questID;
	questinfo.show = p->active;
	questinfo.mobCount = p->count;

	struct tm *qtime, *qtime2;
	qtime = localtime(&p->quest_endTime);

	questinfo.m_mobTime_Year = qtime->tm_year + 1900;
	questinfo.m_mobTime_Month = qtime->tm_mon + 1;
	questinfo.m_mobTime_Day = qtime->tm_mday;
	questinfo.m_mobTime_Hour = qtime->tm_hour;
	questinfo.m_mobTime_Minute = qtime->tm_min;

	if (p->quest_endTime == 0)
	{
		questinfo.m_mobTime_Hour = 0;
		questinfo.m_mobTime_Minute = 0;
	}

	qtime2 = localtime(&p->quest_svrTime);

	for (int i = 0; i < p->count; ++i)
	{
		questinfo.mobID[i] = p->hunt[i].mobGID;
		questinfo.mobHunt[i] = p->hunt[i].huntCount;
		memcpy(questinfo.mobName[i], p->hunt[i].mobName, 51);
	}

	g_session.AddQuestList(questinfo, false);

	g_session.RefreshQuestWnd();
}

void	CGameMode :: Zc_Del_Quest(const char *buf)
{
	PACKET_ZC_DEL_QUEST *	p = (PACKET_ZC_DEL_QUEST *)buf;

	std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
	while (it != g_session.m_QuestList.end())
	{
		if (it->questID == p->questID)
		{
			g_session.m_QuestList.erase(it);
			break;
		}
		it++;
	}

	g_session.RefreshQuestWnd();
}


void CGameMode::Zc_Ack_Touseskill(char const *buf)
{
	PACKET_ZC_ACK_TOUSESKILL *p = (PACKET_ZC_ACK_TOUSESKILL *)buf;

	m_world->m_player->m_isMotionFreezed = false;

	if (p->result != 0)
		return;

	switch (p->cause)
	{
	case USESKILL_FAIL:
		if (g_session.m_isSkillFail == TRUE)
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_USESKILL_FAIL)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_REDJAMSTONE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NEED_REDJAMSTONE)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_BLUEJAMSTONE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NEED_BLUEJAMSTONE)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_THIS_WEAPON:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_USESKILL_FAIL_NOT_SUITABLE_WEAPON)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_SP_INSUFFICIENT:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_USESKILL_FAIL_SP_INSUFFICIENT)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_HP_INSUFFICIENT:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_USESKILL_FAIL_HP_INSUFFICIENT)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_MONEY:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SKILL_FAIL_MONEY)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_SKILLINTERVAL:
		if (g_session.m_isSkillFail == TRUE)
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SKILLINTERVAL)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_STUFF_INSUFFICIENT:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SOURCE)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	case USESKILL_FAIL_LEVEL:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		if (p->SKID == SKID_NV_BASIC)
		{
			switch (p->NUM)
			{
			case ZCSK_ITEMEXCHANGE:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE1)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case ZCSK_EMOTION:
				m_isReqEmotion = false;
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE2)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case ZCSK_SIT:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE3)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case ZCSK_CHAT:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE4)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case ZCSK_PARTY:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE5)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case ZCSK_SHOUT:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE6)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case ZCSK_PK:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE7)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case ZCSK_MANNERPOINT:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_FOR_MANNER_POINT)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
			case ZCSK_JOINPARTY:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE5_2)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			}
		}
		else
		{
			switch (p->SKID)
			{
			case SKID_AL_WARP:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTENOUGHSKILLLEVEL)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case SKID_TF_STEAL:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FAIL_STEAL)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			case SKID_TF_POISON:
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FAIL_POISON)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			default:
				if (g_session.m_isSkillFail == TRUE)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOT_ENOUGH_SKILLLEVE8)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
				break;
			}
		}
		break;
	case USESKILL_FAIL_WEIGHTOVER:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_USESKILL_FAIL_WEIGHTOVER)), int(RGB(255, 0, 0)), NMSG_FAIL_SKILL, 0);
		break;
	}
}


void CGameMode::Zc_SKILLMSG(char const *buf)
{
	PACKET_ZC_SKILLMSG *	p = (PACKET_ZC_SKILLMSG *)buf;
	switch (p->MsgNo)
	{
	case 21:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL1)), 0x9b9bff, 7, 0);
		break;
	case 22:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL2)), 0x9b9bff, 7, 0);
		break;
	case 23:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL3)), 0x9b9bff, 7, 0);
		break;
	case 24:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL4)), 0x9b9bff, 7, 0);
		break;
	case 25:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL5)), 0x9b9bff, 7, 0);
		break;
	case 28:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL6)), 0x9b9bff, 7, 0);
		break;
	case 29:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL7)), 0x9b9bff, 7, 0);
		break;
	case 30:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL8)), 0x9b9bff, 7, 0);
		break;
	case 31:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL9)), 0x9b9bff, 7, 0);
		break;
	case 32:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GOSPEL10)), 0x9b9bff, 7, 0);
		break;
	case 40:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FULLSTRIP)), 0x9b9bff, 7, 0);
		break;
	}
}


void	CGameMode :: Zc_BABYMSG(const char *buf)
{
	PACKET_ZC_BABYMSG *		p = (PACKET_ZC_BABYMSG *)buf;

	switch (p->MsgNo)
	{
	case 0:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BABY1)), 0x9b9bff, 0, 0);
		break;
	case 1:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BABY2)), 0x9b9bff, 0, 0);
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BABY3)), 0x9b9bff, 0, 0);
		break;
	}
}

void	CGameMode :: Zc_Add_Skill(const char *buf)
{
	PACKET_ZC_ADD_SKILL *	p = (PACKET_ZC_ADD_SKILL *)buf;

	SKILL_INFO	skillInfo;
	skillInfo.level			= p->data.level;
	skillInfo.type			= p->data.type;
	skillInfo.SKID			= p->data.SKID;
	skillInfo.upgradable	= p->data.upgradable;
	skillInfo.skillName		= skillInfo.GetSkillIdName();
	skillInfo.spcost		= p->data.spcost;
	skillInfo.attackRange	= p->data.attackRange;

	g_session.AddSkillItem(skillInfo, true);
	g_session.RefreshSkillItemWnd();
}

void CGameMode::Zc_Notify_Skill(char const *buf)
{
	PACKET_ZC_NOTIFY_SKILL *p = (PACKET_ZC_NOTIFY_SKILL *)buf;

	SKILLSTRUCT ss;
	ss.skillId = p->SKID;
	ss.action = p->action;
	ss.damage = p->damage;
	ss.startTime = p->startTime;
	ss.targetGID = p->targetID;
	ss.level = p->level;
	ss.count = p->count;
	ss.tCellX = -1;
	ss.tCellY = -1;
	ss.attackMT = p->attackMT;
	ss.attackedMT = p->attackedMT;

	CGameActor *sActor = NULL;
	CGameActor *theActor = NULL;
	int job = -1;

	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
		sActor = m_world->m_player;
		job = sActor->GetJob();
	}
	else
	{
		theActor = m_world->GetGameActorByAID(p->AID);
		if (theActor)
		{
			theActor->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
			sActor = theActor;
			job = theActor->GetJob();
		}
		else
		{
			CSkill *theSkill = m_world->GetSkillByAID(p->AID);
			if (theSkill)
				theSkill->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
		}
	}

	if (!g_session.IsMonster(sActor))
	{
		SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->level);
	}
	else
	{
		int act;
		int loopType;
		g_session.GetMonsterSkillInfo((char *)g_session.m_jobNameTable[job], ss.skillId, act, loopType);

		if (act != -1)
		{
			if (theActor)
			{
				if (loopType == 0)
					loopType = MT_ONETIME;
				else
					loopType = 0;
				theActor->SetAction((act - 1) * 8, 1, loopType);
			}
		}
	}
}


void CGameMode::Zc_Notify_Skill_Position(char const *buf)
{
	PACKET_ZC_NOTIFY_SKILL_POSITION *p = (PACKET_ZC_NOTIFY_SKILL_POSITION *)buf;

	SKILLSTRUCT ss;
	ss.skillId = p->SKID;
	ss.action = p->action;
	ss.damage = p->damage;
	ss.startTime = p->startTime;
	ss.targetGID = p->targetID;
	ss.level = p->level;
	ss.count = p->count;
	ss.tCellX = p->xPos;
	ss.tCellY = p->yPos;
	ss.attackMT = p->attackMT;
	ss.attackedMT = p->attackedMT;

	CGameActor *sActor = NULL;
	CGameActor *theActor = NULL;
	int job = -1;

	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
		sActor = m_world->m_player;
		job = sActor->GetJob();
	}
	else
	{
		theActor = m_world->GetGameActorByAID(p->AID);
		if (theActor)
		{
			theActor->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
			sActor = theActor;
			job = theActor->GetJob();
		}
		else
		{
			CSkill *theSkill = m_world->GetSkillByAID(p->AID);
			if (theSkill)
				theSkill->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
		}
	}

	if (!g_session.IsMonster(sActor))
	{
		SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->level);
	}
	else
	{
		int act;
		int loopType;
		g_session.GetMonsterSkillInfo((char *)g_session.m_jobNameTable[job], ss.skillId, act, loopType);
	}
}



void CGameMode::Zc_Notify_Groundskill(char const *buf)
{
	PACKET_ZC_NOTIFY_GROUNDSKILL *p = (PACKET_ZC_NOTIFY_GROUNDSKILL *)buf;

	GROUNDSKILLSTRUCT gss;
	gss.skillId = p->SKID;
	gss.startTime = p->startTime;
	gss.cellX = p->xPos;
	gss.cellY = p->yPos;
	gss.level = p->level;

	CGameActor *sActor = NULL;
	int job = -1;
	if (p->AID == g_session.GetAid()) {
		GetWorld()->m_player->SendMsg(NULL, AM_GROUNDSKILL, int(&gss), 0, 0);
		sActor = GetWorld()->m_player;
	} else {
		CGameActor *theActor = m_world->GetGameActorByAID(p->AID);
		if (theActor) {
			theActor->SendMsg(NULL, AM_GROUNDSKILL, int(&gss), 0, 0);
			sActor = theActor;
		}
	}

	if (sActor)
		job = sActor->GetJob();

	if (!g_session.IsMonster(sActor)) {
		SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->level);

		switch (p->SKID) {
		case SKID_PA_GOSPEL:
			PlayWave("effect\\\xb0\xa1\xbd\xba\xc6\xe7.wav");
			break;
		case SKID_HP_BASILICA:
			PlayWave("effect\\\xb9\xd9\xbd\xc7\xb8\xae\xc4\xab.wav");
			break;
		case SKID_AC_SHOWER:
			{
				CGameMode *gameMode = g_modeMgr.GetGameMode();
				if (sActor) {
					gameMode->MakeArrowEffectPos(sActor->m_pos, sActor->m_pos, 18);

					int cellX;
					int cellY;
					cellX = p->xPos;
					cellY = p->yPos;
					int offset[8][2] = {
						{ 1, 0 }, { 1, 1 }, { 1, -1 }, { -1, 0 },
						{ -1, 1 }, { -1, -1 }, { 0, 1 }, { 0, -1 }
					};

					for (int i = 0; i < 8; ++i) {
						float destX;
						float destZ;
						float destY;
						gameMode->GetClientCoor(cellX + offset[i][0], cellY + offset[i][1], destX, destZ);
						destY = gameMode->GetWorld()->m_attr->GetHeight(destX, destZ);
						gameMode->MakeArrowEffectPos(sActor->m_pos, vector3d(destX, destY, destZ), 18);
					}
				}
			}
			break;
		}
	} else {
		int act;
		int loopType;
		g_session.GetMonsterSkillInfo((char *)g_session.m_jobNameTable[job], p->SKID, act, loopType);
	}
}

void	CGameMode :: Zc_State_Change(const char *buf)
{
	PACKET_ZC_STATE_CHANGE *	p = (PACKET_ZC_STATE_CHANGE *)buf;

	ATTRSTATESTRUCT		s;
	s.bodyState		= p->bodyState;
	s.effectState	= p->effectState;
	s.healthState	= p->healthState;
	s.isPKModeON	= p->isPKModeON;

	CGameActor *	tActor;
	if (p->AID == g_session.GetAid())
	{
		tActor = m_world->m_player;
		g_session.m_healthState	= s.healthState;
		g_session.m_bodyState	= s.bodyState;
		g_session.m_effectState	= s.effectState;
		UIEquipWnd *equipWnd = g_windowMgr.GetEquipWnd();
		if (equipWnd)
			equipWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		if (!IsEffectStatePushCart(g_session.m_effectState))
		{
			g_session.ClearMerchantItem();
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMWND);
		}
	}
	else tActor = m_world->GetGameActorByAID(p->AID);

	if (tActor)
		tActor->SendMsg(NULL, AM_ATTR_STATE_CHANGE, int(&s), 0, 0);
}


void	CGameMode :: Zc_State_Change3(const char *buf)
{
	PACKET_ZC_STATE_CHANGE3 *	p = (PACKET_ZC_STATE_CHANGE3 *)buf;

	ATTRSTATESTRUCT		s;
	s.bodyState		= p->bodyState;
	s.effectState	= p->effectState;
	s.healthState	= p->healthState;
	s.isPKModeON	= p->isPKModeON;

	CGameActor *	tActor;
	if (p->AID == g_session.GetAid())
	{
		tActor = m_world->m_player;
		g_session.m_healthState	= s.healthState;
		g_session.m_bodyState	= s.bodyState;
		g_session.m_effectState	= s.effectState;
		UIEquipWnd *equipWnd = g_windowMgr.GetEquipWnd();
		if (equipWnd)
			equipWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		if (!IsEffectStatePushCart(g_session.m_effectState))
		{
			g_session.ClearMerchantItem();
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMWND);
		}
	}
	else tActor = m_world->GetGameActorByAID(p->AID);

	if (tActor)
		tActor->SendMsg(NULL, AM_ATTR_STATE_CHANGE, int(&s), 0, 0);
}


void CGameMode::Zc_Skill_Entry(char const *buf)
{
	PACKET_ZC_SKILL_ENTRY *p = (PACKET_ZC_SKILL_ENTRY *)buf;
	SKILLSTANDENTRYSTRUCT ss;
	ss.AID = p->AID;
	ss.job = p->job;
	ss.xPos = p->xPos;
	ss.yPos = p->yPos;
	ss.isVisible = p->isVisible = 1;
	ss.creatorAID = p->creatorAID;

	CSkill *skill = m_world->GetSkillByAID(p->AID);
	if (ss.creatorAID != g_session.GetAid()) {
		std::list<DWORD>::iterator i = m_visibleTrapList.begin();
		while (i != m_visibleTrapList.end()) {
			if (p->AID == (*i)) {
				i = m_visibleTrapList.erase(i);
				ss.isVisible = p->isVisible = 0;
				break;
			}
			i++;
		}
	}
	if (ss.isVisible)
		m_visibleTrapList.push_back(p->AID);

	if (skill == NULL) {
		if (p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) {
			skill = new CSkill;
		} else {
			sAssert(false);
			return;
		}
		GetWorld()->m_gameObjectList.push_back(skill);
		GetWorld()->m_skillList.push_back(skill);
	}
	skill->SendMsg(NULL, AM_SKILLSTANDENTRY, (int)(&ss));
}

void	CGameMode :: Zc_Skill_Disappear(const char *buf)
{
	PACKET_ZC_SKILL_DISAPPEAR *	p = (PACKET_ZC_SKILL_DISAPPEAR *)buf;
	m_world->RemoveSkill(p->AID);
}

void	CGameMode :: Zc_Warplist(const char *buf)
{
	PACKET_ZC_WARPLIST *	p = (PACKET_ZC_WARPLIST *)buf;
	g_windowMgr.DeleteWindow(WID_CHOOSEWARPWND);
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_CHOOSEWARPWND);
	wnd->SendMsg(NULL, UM_SETSKID, p->SKID, 0, 0, 0);
	for (int i = 0; i < 4; ++i)
	{
		if (strlen(p->mapName[i]))
			wnd->SendMsg(NULL, UM_SETCHOOSE, int(p->mapName[i]), 0, 0, 0);
	}
	wnd->SendMsg(NULL, UM_SETCHOOSE, int("cancel"), 0, 0, 0);
}

void	CGameMode :: Zc_Ack_Remember_Warppoint(const char *buf)
{
	PACKET_ZC_ACK_REMEMBER_WARPPOINT *	p = (PACKET_ZC_ACK_REMEMBER_WARPPOINT *)buf;
	switch (p->errorCode)
	{
	case 0:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,  int(MsgStr(MSI_WARPPOINTSTORED)),     0xffff00, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_WARPPOINTSTORED)),     0xffff00, 0, 0);
		break;
	case 1:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,  int(MsgStr(MSI_NOTENOUGHSKILLLEVEL)), 0x0000ff, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_NOTENOUGHSKILLLEVEL)), 0x0000ff, 0, 0);
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,  int(MsgStr(MSI_NOWARPSKILL)),         0x0000ff, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_NOWARPSKILL)),         0x0000ff, 0, 0);
		break;
	}
}


void	CGameMode :: Zc_Notify_Cartitem_Countinfo(const char *buf)
{
	PACKET_ZC_NOTIFY_CARTITEM_COUNTINFO *	p = (PACKET_ZC_NOTIFY_CARTITEM_COUNTINFO *)buf;

	g_session.m_cartCurCount	= p->curCount;
	g_session.m_cartMaxCount	= p->maxCount;
	g_session.m_cartCurWeight	= p->curWeight / 10;
	g_session.m_cartMaxWeight	= p->maxWeight / 10;

	UIMerchantItemWnd *merchantItemWnd = g_windowMgr.GetMerchantItemWnd();
	if (merchantItemWnd)
		merchantItemWnd->SendMsg(NULL, UM_SETCOUNTINFO, 0, 0, 0, 0);
}


void CGameMode::Zc_Cart_Normal_Itemlist(char const *buf)
{
	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO);
	NORMALITEM_EXTRAINFO *dumpStr = (NORMALITEM_EXTRAINFO *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = 0;
		itemInfo.m_slot[1] = 0;
		itemInfo.m_slot[2] = 0;
		itemInfo.m_slot[3] = 0;
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		g_session.AddMerchantItem(itemInfo);
	}

	g_session.RefreshMerchantItemWnd();
}

void CGameMode::Zc_Cart_Normal_Itemlist2(char const *buf)
{
	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO2);
	NORMALITEM_EXTRAINFO2 *dumpStr = (NORMALITEM_EXTRAINFO2 *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		g_session.AddMerchantItem(itemInfo);
	}

	g_session.RefreshMerchantItemWnd();
}

void CGameMode::Zc_Cart_Normal_Itemlist3(char const *buf)
{
	PACKET_ZC_NORMAL_ITEMLIST *p = (PACKET_ZC_NORMAL_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_NORMAL_ITEMLIST)) / sizeof(NORMALITEM_EXTRAINFO3);
	NORMALITEM_EXTRAINFO3 *dumpStr = (NORMALITEM_EXTRAINFO3 *)(buf + sizeof(PACKET_ZC_NORMAL_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = 0;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = dumpStr[i].count;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = 0;
		itemInfo.m_refiningLevel = 0;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		g_session.AddMerchantItem(itemInfo);
	}

	g_session.RefreshMerchantItemWnd();
}

void CGameMode::Zc_Cart_Equipment_Itemlist(char const *buf)
{
	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO);
	EQUIPMENTITEM_EXTRAINFO *dumpStr = (EQUIPMENTITEM_EXTRAINFO *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		g_session.AddMerchantItem(itemInfo);
	}

	g_session.RefreshMerchantItemWnd();
}

void	CGameMode :: Zc_Cartoff(const char *buf)
{
	g_session.ClearMerchantItem();
	g_windowMgr.DeleteWindow(WID_MERCHANTITEMWND);
}

void	CGameMode :: Zc_Ack_Additem_To_Cart(const char *buf)
{
	PACKET_ZC_ACK_ADDITEM_TO_CART *		p = (PACKET_ZC_ACK_ADDITEM_TO_CART *)buf;
	switch (p->result)
	{
	case ADDITEM_TO_CART_FAIL_WEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OVER_WEIGHT)), 0xff, 0, 0);
		break;
	case ADDITEM_TO_CART_FAIL_COUNT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_INSUFFICIENT_OVER_COUNT)), 0xff, 0, 0);
		break;
	}
}


void CGameMode::Zc_Add_Item_To_Cart(char const *buf)
{
	const unsigned char* p = (const unsigned char*)buf;
	ITEM_INFO itemInfo;

	itemInfo.m_itemIndex = *(const short*)(p + 2);
	itemInfo.m_num = *(const int*)(p + 4);
	itemInfo.SetItemId(*(const unsigned short*)(p + 8));
	itemInfo.m_isIdentified = p[10];
	itemInfo.m_isDamaged = p[11];
	itemInfo.m_refiningLevel = p[12];
	itemInfo.m_slot[0] = *(const unsigned short*)(p + 13);
	itemInfo.m_slot[1] = *(const unsigned short*)(p + 15);
	itemInfo.m_slot[2] = *(const unsigned short*)(p + 17);
	itemInfo.m_slot[3] = *(const unsigned short*)(p + 19);

	g_session.AddMerchantItem(itemInfo);
}

void	CGameMode :: Zc_Delete_Item_From_Cart(const char *buf)
{
	PACKET_ZC_DELETE_ITEM_FROM_CART *	p = (PACKET_ZC_DELETE_ITEM_FROM_CART *) buf;
	ITEM_INFO	itemInfo;
	itemInfo.m_itemIndex	= p->index;
	itemInfo.m_num			= p->count;
	g_session.DecMerchantItem(itemInfo);
}




void	CGameMode :: Zc_openstore(const char *buf)
{
	PACKET_ZC_OPENSTORE *	p = (PACKET_ZC_OPENSTORE *)buf;
	g_session.m_maxNumOfSellItemOfMerchant = p->itemcount;
	if (g_session.m_maxNumOfSellItemOfMerchant < 3)
		g_session.m_maxNumOfSellItemOfMerchant = 3;
	else if (g_session.m_maxNumOfSellItemOfMerchant > 13)
		g_session.m_maxNumOfSellItemOfMerchant = 13;
	g_session.ClearMerchantHopeToSellItem();
	if (g_windowMgr.GetChatRoomWnd() == 0 && g_windowMgr.GetChatRoomMakeWnd() == 0)
	{
		g_windowMgr.MakeWindow(WID_MERCHANTSHOPMAKEWND);
	}
	else
	{
		g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_OPEN_STORE_WHILE_CHAT), ME_OK, 1, 0, 0);
		SendMsg(MM_REQ_CLOSESTORE, 0, 0, 0);
	}
}

void	CGameMode :: Zc_store_entry(const char *buf)
{
	PACKET_ZC_STORE_ENTRY *	p = (PACKET_ZC_STORE_ENTRY *)buf;

	char	storeName[81];
	strncpy(storeName, p->storeName, 80);
	storeName[80] = 0;
	CGameActor *	theActor = m_world->GetGameActorByAID(p->makerAID);
	if (theActor)
	{
		theActor->SendMsg(NULL, AM_MAKE_SHOP_TITLE, (int)storeName, 0, 0);
	}
}


void	CGameMode :: Zc_disappear_entry(const char *buf)
{
	PACKET_ZC_DISAPPEAR_ENTRY *		p = (PACKET_ZC_DISAPPEAR_ENTRY *)buf;
	CGameActor *	theActor = m_world->GetGameActorByAID(p->makerAID);
	if (theActor)
	{
		theActor->SendMsg(NULL, AM_DELETE_SHOP_TITLE, 0, 0, 0);
	}
}


void CGameMode::Zc_pc_purchase_itemlist_frommc(char const *buf)
{
	PACKET_ZC_PC_PURCHASE_ITEMLIST_FROMMC *p = (PACKET_ZC_PC_PURCHASE_ITEMLIST_FROMMC *)buf;
	g_session.ClearMerchantShopItem();

	int numItem = (p->PacketLength - sizeof(PACKET_ZC_PC_PURCHASE_ITEMLIST_FROMMC)) / sizeof(PURCHASE_ITEM_FROMMC);
	for (int i = 0; i < numItem; ++i)
	{
		PURCHASE_ITEM_FROMMC pItem;
		memcpy(&pItem, buf + sizeof(PACKET_ZC_PC_PURCHASE_ITEMLIST_FROMMC) + i * sizeof(PURCHASE_ITEM_FROMMC), sizeof(PURCHASE_ITEM_FROMMC));

		ITEM_INFO itemInfo;
		itemInfo.m_itemIndex = pItem.index;
		itemInfo.m_price = pItem.price;
		itemInfo.m_realPrice = pItem.price;
		itemInfo.m_itemType = pItem.type;
		itemInfo.m_num = pItem.count;
		itemInfo.m_isIdentified = pItem.IsIdentified;
		itemInfo.m_isDamaged = pItem.IsDamaged;
		itemInfo.m_refiningLevel = pItem.refiningLevel;
		itemInfo.SetItemId(pItem.ITID);

		itemInfo.m_slot[0] = pItem.slot.info[0];
		itemInfo.m_slot[1] = pItem.slot.info[1];
		itemInfo.m_slot[2] = pItem.slot.info[2];
		itemInfo.m_slot[3] = pItem.slot.info[3];

		g_session.AddMerchantShopItem(itemInfo);
	}

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_MERCHANTITEMSHOPWND);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
	wnd->SendMsg(NULL, UM_SETNAID, p->AID, 0, 0, 0);

	wnd = g_windowMgr.MakeWindow(WID_MERCHANTITEMPURCHASEWND);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
	wnd->SendMsg(NULL, UM_SETNAID, p->AID, 0, 0, 0);

	m_isOnQuest = true;
}


void CGameMode::Zc_pc_purchase_myitemlist(char const *buf)
{
	g_session.ClearMerchantMyShopItem();
	PACKET_ZC_PC_PURCHASE_MYITEMLIST *p = (PACKET_ZC_PC_PURCHASE_MYITEMLIST *)buf;

	int numItem = (p->PacketLength - sizeof(PACKET_ZC_PC_PURCHASE_MYITEMLIST)) / sizeof(PURCHASE_MYITEM);
	for (int i = 0; i < numItem; ++i)
	{
		PURCHASE_MYITEM myItem;
		memcpy(&myItem, buf + sizeof(PACKET_ZC_PC_PURCHASE_MYITEMLIST) + i * sizeof(PURCHASE_MYITEM), sizeof(PURCHASE_MYITEM));

		ITEM_INFO itemInfo;
		itemInfo.m_itemIndex = myItem.index;
		itemInfo.m_price = myItem.price;
		itemInfo.m_realPrice = myItem.price;
		itemInfo.m_num = myItem.count;
		itemInfo.m_itemType = myItem.type;
		itemInfo.SetItemId(myItem.ITID);
		itemInfo.m_isIdentified = myItem.IsIdentified;
		itemInfo.m_isDamaged = myItem.IsDamaged;
		itemInfo.m_refiningLevel = myItem.refiningLevel;

		itemInfo.m_slot[0] = myItem.slot.info[0];
		itemInfo.m_slot[1] = myItem.slot.info[1];
		itemInfo.m_slot[2] = myItem.slot.info[2];
		itemInfo.m_slot[3] = myItem.slot.info[3];

		g_session.AddMerchantMyShopItem(itemInfo);
	}

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_MERCHANTITEMMYSHOPWND);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
	wnd->SendMsg(NULL, UM_SETTITLE, 0, 0, 0, 0);
	m_isOnQuest = true;
}


void CGameMode::Zc_pc_purchase_result_frommc(char const *buf)
{
	Trace("PACKET_ZC_PC_PURCHASE_RESULT_FROMMC");
	PACKET_ZC_PC_PURCHASE_RESULT_FROMMC *p = (PACKET_ZC_PC_PURCHASE_RESULT_FROMMC *)buf;

	char itemName[64];
	ITEM_INFO itemInfo = g_session.GetMerchantPurchaseItemSaved(p->index);
	if (!itemInfo.m_itemIndex)
		return;

	itemInfo.GetItemName(itemName);
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

	char msg[512];
	switch (p->result)
	{
	case PURCHASE_SUCCEED:
		break;
	case PURCHASE_FAIL_MONEY:
		sprintf(msg, MsgStr(MSI_FAIL_BUY_ITEM_FROM_MERCHANT), itemName, MsgStr(MSI_INSUFFICIENT_MONEY));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(255, 100, 100)), 0, 0);
		break;
	case PURCHASE_FAIL_WEIGHT:
		sprintf(msg, MsgStr(MSI_FAIL_BUY_ITEM_FROM_MERCHANT), itemName, MsgStr(MSI_OVER_WEIGHT));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(255, 100, 100)), 0, 0);
		break;
	case PURCHASE_FAIL_STOCK:
		sprintf(msg, MsgStr(MSI_FAIL_BUY_ITEM_FROM_MERCHANT_NO_STOCK), itemName, p->curcount);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(255, 100, 100)), 0, 0);
		break;
	case PURCHASE_FAIL_ITEM_EXCHANGING:
		sprintf(msg, MsgStr(MSI_FAIL_BUY_ITEM_ITEM_EXCHANGING), itemName, p->curcount);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(255, 100, 100)), 0, 0);
		break;
	default:
		Trace("p->result == %d", p->result);
		break;
	}
}


void CGameMode::Zc_deleteitem_from_mcstore(char const *buf)
{
	PACKET_ZC_DELETEITEM_FROM_MCSTORE *p = (PACKET_ZC_DELETEITEM_FROM_MCSTORE *)buf;

	ITEM_INFO itemInfo;
	itemInfo.m_itemIndex = p->index;
	itemInfo.m_num = p->count;
	itemInfo.m_isIdentified = true;
	g_session.DecMerchantMyShopItem(itemInfo);

	char itemName[64];
	itemInfo.GetItemName(itemName);
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

	char msg[512];
	sprintf(msg, MsgStr(MSI_ITEM_IS_SOLD), itemName, itemInfo.m_num);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(100, 255, 255)), 0, 0);

	UIFrameWnd *merchantItemWnd = g_windowMgr.GetMerchantItemWnd();
	UIFrameWnd *merchantItemMyShopWnd = g_windowMgr.GetMerchantItemMyShopWnd();
	if (merchantItemWnd)
		merchantItemWnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);

	if (g_session.GetNumMerchantMyShopItem() == 0) {
		sprintf(msg, MsgStr(MSI_ITEM_IS_SOLD_OUT));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(0, 255, 0)), 0, 0);
		if (merchantItemMyShopWnd)
		{
			merchantItemMyShopWnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
			merchantItemMyShopWnd->SendMsg(NULL, UM_COMMAND, ID_CLOSE, 0, 0, 0);
		}
		return;
	}

	if (merchantItemMyShopWnd)
		merchantItemMyShopWnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void	CGameMode :: Zc_attack_failure_for_distance (const char *buf)
{
	Trace("PACKET_ZC_ATTACK_FAILURE_FOR_DISTANCE");
	PACKET_ZC_ATTACK_FAILURE_FOR_DISTANCE *		p = (PACKET_ZC_ATTACK_FAILURE_FOR_DISTANCE *)buf;
	g_session.m_attackRange = p->currentAttRange;

	CGameActor *	targetActor = m_world->GetGameActorByAID(p->targetAID);
	if (targetActor)
		targetActor->SendMsg(NULL, AM_CORRECT_POS, p->targetXPos, p->targetYPos, 0);
	m_world->m_player->SendMsg(NULL, AM_CORRECT_POS, p->xPos, p->yPos, 0);
	m_world->m_player->SendMsg(NULL, AM_RESUMEPROCEEDATTACK, p->targetAID, 0, 0);
}

void	CGameMode :: Zc_attack_range(const char *buf)
{
	PACKET_ZC_ATTACK_RANGE *	p = (PACKET_ZC_ATTACK_RANGE *)buf;
	g_session.m_attackRange = p->currentAttRange;
}


void CGameMode::Zc_action_failure(char const *buf)
{
	PACKET_ZC_ACTION_FAILURE *p = (PACKET_ZC_ACTION_FAILURE *)buf;
	Trace("PACKET_ZC_ACTION_FAILURE");

	switch (p->errorCode)
	{
	case AF_ATTACK_ARROW:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ERR_ATTACK_ARROW)), int(RGB(255, 0, 0)), 0, 0);
		break;
	case AF_ATTACK_WEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ERR_ATTACK_WEIGHT)), int(RGB(100, 255, 255)), 0, 0);
		break;
	case AF_SKILL_WEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ERR_SKILL_WEIGHT)), int(RGB(100, 255, 255)), 0, 0);
		break;
	case AF_ARROW_EQUIPMENT_SUCCESS:
		if (g_session.GetJob() == JT_ASSASSIN || g_session.GetJob() == JT_ASSASSIN_B || g_session.GetJob() == JT_ASSASSIN_H)
		{
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MSG_KNIFE_EQUIPMENT_SUCCESS)), int(RGB(100, 255, 255)), 0, 0);
		}
		else if (g_session.GetJob() == JT_GUNSLINGER)
		{
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MSG_BULLET_EQUIPMENT_SUCCESS)), int(RGB(100, 255, 255)), 0, 0);
		}
		else if (g_session.GetJob() == JT_NINJA)
		{
		}
		else
		{
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MSG_ARROW_EQUIPMENT_SUCCESS)), int(RGB(100, 255, 255)), 0, 0);
		}
		break;
	default:
		break;
	}
}

void	CGameMode :: Zc_equip_arrow(const char *buf)
{
	PACKET_ZC_EQUIP_ARROW *	p = (PACKET_ZC_EQUIP_ARROW *)buf;
	g_session.SetEquipArrowIndwx(p->index);
	m_waitingWearEquipAck = timeGetTime() - 2000;
}


void CGameMode::Zc_recovery(char const *buf)
{
	PACKET_ZC_RECOVERY *p = (PACKET_ZC_RECOVERY *)buf;
	CGameActor *player = m_world->m_player;

	Trace("PACKET_ZC_RECOVERY p->varID==%d p->amount==%d", p->varID, p->amount);
	switch (p->varID)
	{
	case VAR_HP:
	{
		g_session.ChangeXorValue();
		int prevHp = g_session.GetHp();
		g_session.SetHp(prevHp + p->amount);

		player->SendMsg(NULL, AM_MAKE_NUMBER, p->amount, 0xff00ff00, MET_NUMBER + 100);
		player->LaunchEffect(EF_HPTIME);
		break;
	}
	case VAR_SP:
	{
		g_session.ChangeXorValue();
		int sp = g_session.GetSp();
		int spRecovery = p->amount + sp;
		g_session.SetSp(spRecovery);

		player->SendMsg(NULL, AM_MAKE_NUMBER, p->amount, 0xff0000ff, MET_NUMBER + 100);
		player->LaunchEffect(EF_SPTIME);
		break;
	}
	}
}


void	CGameMode :: Zc_useskill_ack(const char *buf)
{
	PACKET_ZC_USESKILL_ACK *	p = (PACKET_ZC_USESKILL_ACK *)buf;

	CGameActor *	sActor = NULL;
	if (p->AID == g_session.GetAid())
		sActor = m_world->m_player;
	else
		sActor = m_world->GetGameActorByAID(p->AID);

	if (sActor==NULL)
		return;

	int		beginEffectId, skillMotionType;
	g_session.GetSkillActionInfo2(p->SKID, beginEffectId, skillMotionType, p->property , sActor->GetJob());

	if (p->SKID == SKID_KN_BOWLINGBASH || p->SKID == SKID_KN_BRANDISHSPEAR)
	{
		sActor->SetState(skillMotionType);
		sActor->m_isMotionFreezed = true;
		sActor->SetFreezeEndTick(timeGetTime() + p->delayTime);
	}
	else
	{
		sActor->SetState(skillMotionType);
		sActor->SendMsg(NULL, AM_MAKE_BEGIN_SPELL, beginEffectId, p->delayTime/16);
		sActor->SendMsg(NULL, AM_MAKE_SKILL_RECHARGEGAGE, p->delayTime);

#ifdef	MOB_SKILL
		if (!g_session.IsMonster(sActor))
		{
#ifdef WEDDING_PATCH
			if (p->SKID == SKID_WE_MALE || p->SKID == SKID_WE_FEMALE )
			{
				SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->targetID);
			}
			else
#endif
				SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor));
		}
		else
		{
			int act, loopType;
			g_session.GetMonsterSkillInfo((char*)g_session.m_jobNameTable[sActor->GetJob()], p->SKID, act, loopType);

			if (act != -1)
			{
				if (loopType == 0)
					loopType = MT_ONETIME;
				else loopType = 0;
				sActor->SetAction((act - 1) * 8, 1, loopType);
				if (p->SKID == SKID_NPC_METAMORPHOSIS)
				{
					sActor->m_motionSpeed = (p->delayTime / 300);
				}
			}
		}
#else
		SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor));
#endif
	}

	if (p->targetID)
	{
		CGameActor *	tActor = NULL;
		if (p->targetID == g_session.GetAid())
			tActor = m_world->m_player;
		else
			tActor = m_world->GetGameActorByAID(p->targetID);
		if (sActor && tActor && (sActor!=tActor))
		{
			bool	canSetTargetID = true ;

			if ( g_session.IsMer( tActor->GetJob() )
				|| tActor->m_objectType == NPC_MERSOL_TYPE
				)
			{
				canSetTargetID = false ;

				if (m_world->IsPKZone() || m_world->IsSiegeMode())
					canSetTargetID = true ;

				if ( !SKILL_INFO::IsGoodSkillForActor(p->SKID) )
					canSetTargetID = true ;
			}

			if (canSetTargetID)
				sActor->SendMsg(NULL, AM_SET_TARGET_AID, p->targetID);

			sActor->CalcDir(tActor->m_pos.x, tActor->m_pos.z);
			tActor->SendMsg(NULL, AM_MAKE_MAGIC_TARGET, EF_LOCKON , p->delayTime/16 );

			if ( p->SKID == SKID_CR_ACIDDEMONSTRATION )
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM4);
				if (effect)
				{
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
		}
	}
	else
	{
		float destX, destZ;
		g_modeMgr.GetGameMode()->GetClientCoor(p->xPos, p->yPos, destX, destZ);
		sActor->CalcDir(destX, destZ);

		CGroundScope* groundScope = new CGroundScope;
		groundScope->SendMsg(NULL, AM_SETMASTERGID, p->AID, 0, 0);
		groundScope->SendMsg(NULL, AM_SETPOS, p->xPos, p->yPos, 0);
		groundScope->SendMsg(NULL, AM_SETTIME, timeGetTime() + p->delayTime, 0, 0);
		groundScope->SendMsg(NULL, AM_SET_SKILL, p->SKID, 0, 0);
		GetWorld()->m_gameObjectList.push_back(groundScope);
	}

#ifdef	SECONDJOB_SKILL2
	{
		CGameActor *	tActor = NULL;
		if (p->targetID == g_session.GetAid())
			tActor = m_world->m_player;
		else
			tActor = m_world->GetGameActorByAID(p->targetID);

		switch (p->SKID)
		{
		case	SKID_MO_CALLSPIRITS:
		case	SKID_MO_KITRANSLATION:
			if (sActor)
			{
				sActor->SetForceAnimation(96, 0, 0, 96, 0, FALSE, 0, FALSE);
			}
			break;
		case	SKID_CH_SOULCOLLECT:
			if (sActor)
			{
				sActor->SetForceAnimation(96, 0, 0, 96, 0, FALSE, 0, FALSE);
				sActor->LaunchEffect(EF_PORTAL5);
			}
			break;
		case	SKID_MO_STEELBODY:
			if (sActor)
			{
				sActor->SetForceAnimation(96, 0, 0, 96, 0, TRUE, p->delayTime + 1000, FALSE);
			}
			break;
		}
	}
#endif
}

void	CGameMode :: Zc_groupinfo_change(const char *buf)
{
	PACKET_ZC_GROUPINFO_CHANGE *	p = (PACKET_ZC_GROUPINFO_CHANGE *)buf;
	g_session.m_expDivType = p->expOption;
	Trace("PACKET_ZC_GROUPINFO_CHANGE %d", g_session.m_expDivType);
	UIPartySettingWnd *partySettingWnd = g_windowMgr.GetPartySettingWnd();
	if (partySettingWnd)
		partySettingWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);

	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

	char text[128];
	switch (g_session.m_expDivType) {
	case 0:
		sprintf(text, "%s - %s : %s", MsgStr(MSI_PARTYSETTING), MsgStr(MSI_HOWEXPDIV), MsgStr(MSI_EXPDIV1));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)text, 0xffff, 0xb, 0);
		break;
	case 1:
		sprintf(text, "%s - %s : %s", MsgStr(MSI_PARTYSETTING), MsgStr(MSI_HOWEXPDIV), MsgStr(MSI_EXPDIV2));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)text, 0xffff, 0xb, 0);
		break;
	case 2:
		sprintf(text, "%s", MsgStr(MSI_PARTYSETTING_CHANGE_IMPOSSIBLE), MsgStr(MSI_HOWEXPDIV), MsgStr(MSI_EXPDIV2));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)text, 0xffff, 0xb, 0);
		break;
	}
}


void CGameMode::Zc_couplestatus(char const *buf)
{
	PACKET_ZC_COUPLESTATUS *p = (PACKET_ZC_COUPLESTATUS *)buf;
	switch (p->statusType)
	{
	case VAR_STR:
		g_session.SetStr(p->defaultStatus);
		g_session.SetPlusStr(p->plusStatus);
		break;
	case VAR_AGI:
		g_session.SetAgi(p->defaultStatus);
		g_session.SetPlusAgi(p->plusStatus);
		break;
	case VAR_VIT:
		g_session.SetVit(p->defaultStatus);
		g_session.SetPlusVit(p->plusStatus);
		break;
	case VAR_INT:
		g_session.SetInt(p->defaultStatus);
		g_session.SetPlusInt(p->plusStatus);
		break;
	case VAR_DEX:
		g_session.SetDex(p->defaultStatus);
		g_session.SetPlusDex(p->plusStatus);
		break;
	case VAR_LUK:
		g_session.SetLuk(p->defaultStatus);
		g_session.SetPlusLuk(p->plusStatus);
		break;
	default:
		break;
	}
}

void	CGameMode :: Zc_open_editdlg(const char *buf)
{
	PACKET_ZC_OPEN_EDITDLG *	p = (PACKET_ZC_OPEN_EDITDLG *)buf;

	CGameActor *	theNpc = m_world->GetGameActorByAID(p->NAID);
	if (theNpc)
	{
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NPCEDITDIALOGWND);
		wnd->SendMsg(NULL, UM_SETNAID, p->NAID, 0, 0, 0);
	}
}


void CGameMode::Zc_compass(char const *buf)
{
	Trace("PACKET_ZC_COMPASS");
	PACKET_ZC_COMPASS *p = (PACKET_ZC_COMPASS *)buf;
	Trace("id(%d) x(%d) y(%d) color(%x) type(%d)", p->id, p->xPos, p->yPos, p->color, p->type);

	if (p->type != 2)
	{
		m_compassPosList[p->id].x = p->xPos;
		m_compassPosList[p->id].y = p->yPos;
		m_compassPosList[p->id].color = p->color | 0xff000000;
		m_compassPosList[p->id].type = p->type;
		m_compassPosList[p->id].startTime = timeGetTime();
	}
	else
	{
		DeleteCompassPos(p->id);
	}

	Trace("id(%d) x(%d) y(%d) color(%x) type(%d)", p->id, p->xPos, p->yPos, p->color, p->type);
}

void	CGameMode :: Zc_show_image(const char *buf)
{
	PACKET_ZC_SHOW_IMAGE *		p = (PACKET_ZC_SHOW_IMAGE *)buf;

	char	imageName[IMAGENAME_LENGTH+1];
	strncpy(imageName, p->imageName, IMAGENAME_LENGTH);
	imageName[IMAGENAME_LENGTH] = 0;
	int		type = p->type;
	if (type == 255)
		type = -1;
	SendMsg(MM_SHOW_IMAGE, int(imageName), type);
}

void	CGameMode :: Zc_show_image2(const char *buf)
{
	PACKET_ZC_SHOW_IMAGE2 *		p = (PACKET_ZC_SHOW_IMAGE2 *)buf;

	char	imageName[IMAGENAME_LENGTH2+1];
	strncpy(imageName, p->imageName, IMAGENAME_LENGTH2);
	imageName[IMAGENAME_LENGTH2] = 0;
	int		type = p->type;
	if (type == 255)
		type = -1;
	SendMsg(MM_SHOW_IMAGE, int(imageName), type);
}

void	CGameMode :: Zc_autorun_skill(const char *buf)
{
	PACKET_ZC_AUTORUN_SKILL *	p = (PACKET_ZC_AUTORUN_SKILL *)buf;

	SKILL_INFO	skillInfo;
	skillInfo.level			= p->data.level;
	skillInfo.type			= p->data.type;
	skillInfo.SKID			= p->data.SKID;
	skillInfo.upgradable	= p->data.upgradable;
	skillInfo.skillName		= skillInfo.GetSkillIdName();
	skillInfo.spcost		= p->data.spcost;
	skillInfo.attackRange	= p->data.attackRange;

	g_session.AddTempSkillItem(skillInfo);

	SendMsg(MM_USING_SKILL, int(&skillInfo), skillInfo.level);
}

void	CGameMode :: Zc_resurrection(const char *buf)
{
	Trace("PACKET_ZC_RESURRECTION");
	PACKET_ZC_RESURRECTION *	p = (PACKET_ZC_RESURRECTION *)buf;

	Trace("p->AID(%d) playerAid(%d)", p->AID, g_session.GetAid());

	CGameActor *	theActor = m_world->GetGameActorByAID(p->AID);
	if (theActor)
	{
		Trace("PACKET_ZC_RESURRECTION (theActor!=NULL)");
		theActor->SendMsg(NULL, AM_RESURRECTION, 0, 0, 0);
	}
	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_RESURRECTION, 0, 0, 0);
		m_isOnQuest = 0;
		m_isPlayerDead = 0;
	}
}


void	CGameMode :: Zc_guild_emblem_img(const char *buf)
{
	PACKET_ZC_GUILD_EMBLEM_IMG *	p = (PACKET_ZC_GUILD_EMBLEM_IMG *)buf;
	g_session.SetCompressedEmblemImage(p->image, p->PacketLength - 12, p->GDID, p->emblemVersion);
	if (g_guildInfo.m_gdid == p->GDID)
	{
		m_world->m_player->SetGuildInfo(p->GDID, p->emblemVersion);
		UIGuildInfoManageWnd *guildInfoManageWnd = g_windowMgr.GetGuildInfoManageWnd();
		if (guildInfoManageWnd)
			guildInfoManageWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	}
}


void CGameMode::Zc_guild_info(char const *buf)
{
	PACKET_ZC_GUILD_INFO *p = (PACKET_ZC_GUILD_INFO *)buf;

	char guildname[NAME_LENGTH + 1];
	char masterName[NAME_LENGTH + 1];
	char manageLand[NAME_LENGTH + 1];

	memcpy(guildname, p->guildname, NAME_LENGTH);
	guildname[NAME_LENGTH] = 0;
	memcpy(masterName, p->masterName, NAME_LENGTH);
	masterName[NAME_LENGTH] = 0;
	memcpy(manageLand, p->manageLand, NAME_LENGTH);
	manageLand[NAME_LENGTH] = 0;

	g_guildInfo.m_guildName = guildname;
	g_guildInfo.m_masterName = masterName;
	g_guildInfo.m_amIMaster = g_guildInfo.m_masterName == g_session.GetCharName();
	g_guildInfo.m_manageLand = manageLand;

	g_guildInfo.m_gdid = p->GDID;
	g_guildInfo.m_guildLevel = p->level;
	g_guildInfo.m_numMember = p->userNum;
	g_guildInfo.m_numMaxMember = p->maxUserNum;
	g_guildInfo.m_memberAvgLevel = p->userAverageLevel;
	g_guildInfo.m_exp = p->exp;
	g_guildInfo.m_nextExp = p->maxExp;
	g_guildInfo.m_point = p->point;
	g_guildInfo.m_honor = p->honor;
	g_guildInfo.m_virtue = p->virtue;
	g_guildInfo.m_emblemVersion = p->emblemVersion;
	g_windowMgr.InvalidateGuildWnd();
}


void CGameMode::Zc_myguild_basic_info(char const *buf)
{
	PACKET_ZC_MYGUILD_BASIC_INFO *p = (PACKET_ZC_MYGUILD_BASIC_INFO *)buf;
	int packetStrLen = sizeof(PACKET_ZC_MYGUILD_BASIC_INFO);
	int size = (p->PacketLength - packetStrLen) / sizeof(RELATED_GUILD_INFO);

	g_guildInfo.m_relatedGuildInfoList.clear();

	for (int i = 0; i < size; ++i)
	{
		RELATED_GUILD_INFO *rg_info = (RELATED_GUILD_INFO *)(buf + packetStrLen + i * sizeof(RELATED_GUILD_INFO));
		char guildname[25];
		memcpy(guildname, rg_info->guildname, 24);
		guildname[24] = 0;

		RELATED_GUILD_INFORMATION info;
		info.relation = rg_info->relation;
		info.GDID = rg_info->GDID;
		info.GuildName = guildname;
		g_guildInfo.m_relatedGuildInfoList.push_back(info);
	}
	g_windowMgr.InvalidateGuildWnd();
}

void	CGameMode :: Zc_ack_guild_menuinterface(const char *buf)
{
	PACKET_ZC_ACK_GUILD_MENUINTERFACE *	p = (PACKET_ZC_ACK_GUILD_MENUINTERFACE *)buf;
	g_guildInfo.m_guildTabBitmask = p->guildMemuFlag;
}

void CGameMode::Zc_position_id_name_info(char const *buf)
{


	PACKET_ZC_POSITION_ID_NAME_INFO *p = (PACKET_ZC_POSITION_ID_NAME_INFO *)buf;
	int packetStrLen = sizeof(PACKET_ZC_POSITION_ID_NAME_INFO);
	int size = (p->PacketLength - packetStrLen) / sizeof(MEMBER_POSITION_ID_NAME_INFO);
	sAssert(size > 0);
	for (int i = 0; i < size; ++i)
	{
		MEMBER_POSITION_ID_NAME_INFO *info = (MEMBER_POSITION_ID_NAME_INFO *)(buf + packetStrLen + i * sizeof(MEMBER_POSITION_ID_NAME_INFO));
		char posName[25];
		memcpy(posName, info->posName, 24);
		posName[24] = 0;
		std::string &name = g_guildInfo.m_positionIdNameMap[info->positionID];
		name.assign(posName, strlen(posName));
	}
	g_windowMgr.InvalidateGuildWnd();
}

void	CGameMode :: Zc_ack_req_change_members(const char *buf)
{
	PACKET_ZC_ACK_REQ_CHANGE_MEMBERS *	p = (PACKET_ZC_ACK_REQ_CHANGE_MEMBERS *)buf;
	int		packetStrLen = sizeof(PACKET_ZC_ACK_REQ_CHANGE_MEMBERS);
	int		size = (p->PacketLength - packetStrLen) / sizeof(MEMBER_POSITION_INFORMATION);

	for (int i = 0; i < size; ++i)
	{
		MEMBER_POSITION_INFORMATION *	info = (MEMBER_POSITION_INFORMATION *)(buf + packetStrLen + i * sizeof(MEMBER_POSITION_INFORMATION));
		g_guildInfo.ChangeMemberPosition(info->AID, info->GID, info->positionID);
	}
}


void CGameMode::Zc_membermgr_info(char const *buf)
{
	PACKET_ZC_MEMBERMGR_INFO *p = (PACKET_ZC_MEMBERMGR_INFO *)buf;
	int packetStrLen = sizeof(PACKET_ZC_MEMBERMGR_INFO);
	int size = (p->PacketLength - packetStrLen) / sizeof(GUILD_MEMBER_INFO);

	g_guildInfo.m_numMember = size;
	g_guildInfo.ClearMemberInfo();
	for (int i = 0; i < size; ++i) {
		GUILD_MEMBER_INFO *gm_Info = (GUILD_MEMBER_INFO *)(buf + packetStrLen + i * sizeof(GUILD_MEMBER_INFO));

		GUILD_MEMBER_INFORMATION gm_Information;
		gm_Information.AID = gm_Info->AID;
		gm_Information.GID = gm_Info->GID;
		gm_Information.HeadType = gm_Info->head;
		gm_Information.HeadPalette = gm_Info->headPalette;
		gm_Information.Job = gm_Info->job;
		gm_Information.Sex = gm_Info->sex;
		gm_Information.GPositionID = gm_Info->positionID;
		gm_Information.Level = gm_Info->level;
		gm_Information.Service = 0;
		gm_Information.MemberExp = gm_Info->contributionExp;
		gm_Information.CurrentState = gm_Info->currentState;

		char intro[51];
		memcpy(intro, gm_Info->intro, 50);
		intro[50] = 0;

		char charname[NAME_LENGTH + 1];
		memcpy(charname, gm_Info->charname, NAME_LENGTH);
		charname[NAME_LENGTH] = 0;

		gm_Information.CharName = charname;
		gm_Information.Memo = intro;
		gm_Information.GPositionName = "";
		g_guildInfo.AddMemberInfo(&gm_Information);
	}
	g_guildInfo.RecalculateGuildExp();
	g_windowMgr.InvalidateGuildWnd();
}

void	CGameMode :: Zc_ack_open_member_info(const char *buf)
{
}


void CGameMode::Zc_ack_leave_guild(char const *buf)
{
	PACKET_ZC_ACK_LEAVE_GUILD *p = (PACKET_ZC_ACK_LEAVE_GUILD *)buf;

	char reasonDesc[41];
	memcpy(reasonDesc, p->reasonDesc, 40);
	reasonDesc[40] = 0;

	char charName[NAME_LENGTH + 1];
	memcpy(charName, p->charName, NAME_LENGTH);
	charName[NAME_LENGTH] = 0;

	if (std::string(charName) == g_session.GetCharName()) {
		g_guildInfo.Init();
		g_windowMgr.DeleteWindow(WID_GUILDWND);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_LEFT_GUILD)), 0xff, 4, 0);
		m_world->m_player->SetGuildInfo(0, 0);
		SendMsg(MM_DELETEGUILDPOSALL);
		return;
	}

	if (g_guildInfo.m_isRequestInfo[1]) {
		g_guildInfo.RemoveMemberInfo(charName);
	}

	char message[1024];
	sprintf(message, MsgStr(MSI_USER_LEFT_GUILD), charName);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(message), 0xffff, 4, 0);

	sprintf(message, MsgStr(MSI_REASON_LEAVE_GUILD), reasonDesc);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(message), 0xffff, 4, 0);
}


void CGameMode::Zc_ack_ban_guild(char const *buf)
{
	PACKET_ZC_ACK_BAN_GUILD *p = (PACKET_ZC_ACK_BAN_GUILD *)buf;

	char reasonDesc[41];
	memcpy(reasonDesc, p->reasonDesc, 40);
	reasonDesc[40] = 0;

	char charName[NAME_LENGTH + 1];
	memcpy(charName, p->charName, NAME_LENGTH);
	charName[NAME_LENGTH] = 0;

	char account[NAME_LENGTH + 1];
	memcpy(account, p->account, NAME_LENGTH);
	account[NAME_LENGTH] = 0;

	if (std::string(charName) == g_session.GetCharName()) {
		g_guildInfo.m_gdid = 0;
		g_windowMgr.DeleteWindow(WID_GUILDWND);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BAN_FROM_GUILD)), 0xff, 4, 0);
		SendMsg(MM_DELETEGUILDPOSALL);
		return;
	}

	if (g_guildInfo.m_isRequestInfo[1]) {
		g_guildInfo.RemoveMemberInfo(charName);
	}

	char message[1024];
	sprintf(message, MsgStr(MSI_BAN_GUILD), charName);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(message), 0xffff, 4, 0);

	sprintf(message, MsgStr(MSI_REASON_BAN_GUILD), reasonDesc);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(message), 0xffff, 4, 0);

	SGUILD_BANISH_INFO banInfo;
	banInfo.MemberName = charName;
	banInfo.MemberAccount = account;
	banInfo.Reason = reasonDesc;
	g_guildInfo.AddToBanishList(&banInfo);
}

void	CGameMode :: Zc_ack_disorganize_guild_result(const char *buf)
{
	PACKET_ZC_ACK_DISORGANIZE_GUILD_RESULT *	p = (PACKET_ZC_ACK_DISORGANIZE_GUILD_RESULT *)buf;
	switch (p->reason)
	{
	case DISRESULT_MEMBEREXIST:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DISORGANIZE_GUILD_MEMBEREXIST)), 0xffff, 4, 0);
		break;
	case DISRESULT_INVALIDKEY:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DISORGANIZE_GUILD_INVALIDKEY)), 0xffff, 0, 0);
		break;
	case DISRESULT_SUCCESS:
		g_guildInfo.Init();
		g_windowMgr.DeleteWindow(WID_GUILDWND);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DISORGANIZE_GUILD_SUCCESS)), 0xffff, 4, 0);
		break;
	}
}

void	CGameMode :: Zc_ack_disorganize_guild(const char *buf)
{
	PACKET_ZC_ACK_DISORGANIZE_GUILD *	p = (PACKET_ZC_ACK_DISORGANIZE_GUILD *)buf;

	const int	REASON_LEN = 40;
	char	reasonDesc[REASON_LEN+1];
	memcpy(reasonDesc, p->reasonDesc, REASON_LEN);
	reasonDesc[REASON_LEN] = 0;

	char	message[1024];
	sprintf(message, MsgStr(MSI_DISORGANIZATION_REASON), reasonDesc);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(message), 0xffff, 4, 0);
}


void CGameMode::Zc_position_info(char const *buf)
{
	PACKET_ZC_POSITION_INFO *p = (PACKET_ZC_POSITION_INFO *)buf;
	int packetStrLen = sizeof(PACKET_ZC_POSITION_INFO);
	int size = (p->PacketLength - packetStrLen) / sizeof(GUILD_POSITION_INFO);

	g_guildInfo.GetPosInfoList().clear();
	for (int i = 0; i < size; ++i)
	{
		GUILD_POSITION_INFO *gp_info = (GUILD_POSITION_INFO *)(buf + packetStrLen + i * sizeof(GUILD_POSITION_INFO));
		GUILD_POSITION_INFORMATION info;
		info.positionID = gp_info->positionID;
		info.posName = g_guildInfo.m_positionIdNameMap[info.positionID];
		info.ranking = gp_info->ranking;
		info.payRate = gp_info->payRate;
		info.rightOfJoin = (gp_info->right & GUILD_RIGHT_JOIN) ? TRUE : FALSE;
		info.rightOfPunish = (gp_info->right & GUILD_RIGHT_PUNISHMENT) ? TRUE : FALSE;
		g_guildInfo.GetPosInfoList().push_back(info);
	}
	g_windowMgr.InvalidateGuildWnd();
}


void CGameMode::Zc_ban_list(char const *buf)
{
	PACKET_ZC_BAN_LIST *p = (PACKET_ZC_BAN_LIST *)buf;
	int packetStrLen = sizeof(PACKET_ZC_BAN_LIST);
	int size = (p->PacketLength - packetStrLen) / sizeof(GUILD_BAN_INFO);

	g_guildInfo.m_banishList.clear();
	for (int i = 0; i < size; ++i) {
		GUILD_BAN_INFO *bInfo = (GUILD_BAN_INFO *)(buf + packetStrLen + i * sizeof(GUILD_BAN_INFO));

		char charname[NAME_LENGTH + 1];
		memcpy(charname, bInfo->charname, NAME_LENGTH);
		charname[NAME_LENGTH] = 0;

		char account[NAME_LENGTH + 1];
		memcpy(account, bInfo->account, NAME_LENGTH);
		account[NAME_LENGTH] = 0;

		char reason[41];
		memcpy(reason, bInfo->reason, 40);
		reason[40] = 0;

		SGUILD_BANISH_INFO sbInfo;
		sbInfo.MemberAccount = account;
		sbInfo.MemberName = charname;
		sbInfo.Reason = reason;

		g_guildInfo.m_banishList.push_back(sbInfo);
	}
	g_windowMgr.InvalidateGuildWnd();
}

void	CGameMode :: Zc_other_guild_list(const char * buf)
{
}

void	CGameMode :: Zc_result_make_guild(const char *buf)
{
	PACKET_ZC_RESULT_MAKE_GUILD *	p = (PACKET_ZC_RESULT_MAKE_GUILD *)buf;
	switch (p->result)
	{
	case 0:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GUILD_MAKE_SUCCESS)), 0xffff00, 4, 0);
		break;
	case 1:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GUILD_MAKE_ALREADY_MEMBER)), 0xffff, 4, 0);
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GUILD_MAKE_GUILD_EXIST)), 0xffff, 4, 0);
		break;
	case 3:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GUILD_MAKE_GUILD_NONE_ITEM)), 0xffff, 4, 0);
		break;
	}
}



void	CGameMode :: Zc_ack_req_join_guild(const char *buf)
{
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	PACKET_ZC_ACK_REQ_JOIN_GUILD *		p = (PACKET_ZC_ACK_REQ_JOIN_GUILD *)buf;
	switch (p->answer)
	{
	case	REQJOINGUILD_ALREADY_OTHERGUILDM:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_IS_ALREADY_IN_GUILD)), 0xff, 4, 0);
		break;
	case	REQJOINGUILD_REJECT:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_REJECT_JOIN_GUILD)), 0xff, 4, 0);
		break;
	case	REQJOINGUILD_ACCEPT:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CHARACTER_ACCEPT_JOIN_GUILD)), 0xffff00, 4, 0);
		break;
	case	REQJOINGUILD_OVERSIZE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_MANY_PEOPLE_IN_GUILD)), 0xff, 4, 0);
		break;
	}
}

void	CGameMode :: Zc_req_join_guild(const char *buf)
{
	PACKET_ZC_REQ_JOIN_GUILD *p = (PACKET_ZC_REQ_JOIN_GUILD *)buf;
	char guildName[NAME_LENGTH+1];
	memcpy(guildName, p->guildName, NAME_LENGTH);
	guildName[NAME_LENGTH] = 0;
	UIFrameWnd* w = g_windowMgr.MakeWindow((WINDOWID)WID_JOINGUILDACCEPTWND);
	w->SendMsg(0, UM_SET_JOIN_GUILD_MSG, p->GDID, (int)guildName, 0, 0);
}

void	CGameMode :: Zc_update_gdid(const char *buf)
{
	PACKET_ZC_UPDATE_GDID *		p = (PACKET_ZC_UPDATE_GDID *)buf;
	g_guildInfo.m_gdid			= p->GDID;
	g_guildInfo.m_rightJoin		= p->right & 1;
	g_guildInfo.m_rightPunish	= ((unsigned int)p->right >> 4) & 1;
	g_guildInfo.m_amIMaster		= p->isMaster;
	m_world->m_player->SetGuildInfo(p->GDID, p->emblemVersion);
}

void	CGameMode :: Zc_update_charstat(const char *buf)
{
	PACKET_ZC_UPDATE_CHARSTAT * p = (PACKET_ZC_UPDATE_CHARSTAT *)buf;
	g_guildInfo.SetMemberInfoState(p->AID, p->status);

	const NamePair& actor = GetActorNameByGID(p->GID);
	if (!actor.cName.empty())
	{
		SendMsg(MM_GUILD_MEMBER_STATUS_CHANGE, (int)actor.cName.c_str(), p->status, 0);
	}
	if (p->status == 0)
	{
		SendMsg(MM_DELETEGUILDPOS, p->AID, 0, 0);
	}
}

void CGameMode::Zc_update_charstat2(const char *buf)
{
	PACKET_ZC_UPDATE_CHARSTAT2 * p = (PACKET_ZC_UPDATE_CHARSTAT2 *)buf;
	g_guildInfo.SetMemberInfoState2(p->AID, p->status, p->GID, p->sex, p->head, p->headPalette);

	const NamePair& actorName = GetActorNameByGID(p->GID);
	if (!actorName.IsEmpty())
	{
		SendMsg(MM_GUILD_MEMBER_STATUS_CHANGE, (int)actorName.cName.c_str(), p->status, 0);
	}
	if (p->status == 0)
	{
		SendMsg(MM_DELETEGUILDPOS, p->AID, 0, 0);
	}
}

void	CGameMode :: Zc_ack_give_manner_point(const char *buf)
{
	PACKET_ZC_ACK_GIVE_MANNER_POINT *	p = (PACKET_ZC_ACK_GIVE_MANNER_POINT *) buf ;

	switch ( p->result )
	{
	case MP_SUCCESS:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SEND_MANNER_POINT)), int(RGB(0, 255, 255)), 0, 0);
		break;

	case MP_RELEASE_BY_ADMIN:
		break;

	case MP_SUCCESS_BY_ADMIN :
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GM_BAD_MANNER_START)), 0xffff, 0, 0);
		break ;

	case MP_BYSELF :
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AUTO_BAD_MANNER_START)), 0xffff, 0, 0);
		break ;
	}
}

void	CGameMode :: Zc_notify_manner_point_given (const char *buf)
{
	PACKET_ZC_NOTIFY_MANNER_POINT_GIVEN *	p = (PACKET_ZC_NOTIFY_MANNER_POINT_GIVEN *)buf;

	char	msg[128];
	char	charName[NAME_LENGTH+1];
	memcpy(charName, p->otherCharName, NAME_LENGTH);
	charName[NAME_LENGTH] = 0;

	if (p->type == MP_PLUS)
	{
		sprintf(msg, MsgStr(MSI_RECEIVE_PLUS_MANNER_POINT), charName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 0, 0);
	}
}


void CGameMode::Zc_guild_notice(char const *buf)
{
	PACKET_ZC_GUILD_NOTICE *p = (PACKET_ZC_GUILD_NOTICE *)buf;

	p->notice[sizeof(p->notice) - 1] = 0;
	p->subject[sizeof(p->subject) - 1] = 0;

	g_guildInfo.m_subjectName = p->subject;
	g_guildInfo.m_noticeName = p->notice;

	if (g_session.m_isLogInOut == TRUE) {
		std::string subject = "[ ";
		subject += p->subject;
		subject += " ]";
		if (strlen(p->subject) > 0) {
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(subject.c_str()), 0x80ffff, 0, 0);
		}

		std::string notice = "[ ";
		notice += p->notice;
		notice += " ]";
		if (strlen(p->notice) > 0) {
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(notice.c_str()), 0x80ffff, 0, 0);
		}
	}
}

void	CGameMode :: Zc_guild_chat(const char *buf)
{
	PACKET_ZC_GUILD_CHAT *	p = (PACKET_ZC_GUILD_CHAT *)buf;
	const char *	gChat = buf + sizeof(PACKET_ZC_GUILD_CHAT);

	int		chatLen = p->PacketLength - sizeof(PACKET_ZC_GUILD_CHAT);
	char	guildChat[2048];
	memcpy(guildChat, gChat, chatLen);
	guildChat[chatLen-1] = 0;
	guildChat[2048-1] = 0;
	if (g_session.m_isNoTalkMsg2 == FALSE)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(guildChat), 0xb4ffb4, 4, 0);
		if (g_session.m_isSaveChat)
		{
			WriteChat(guildChat);
		}
	}
}

void	CGameMode :: Zc_req_ally_guild(const char *buf)
{
	PACKET_ZC_REQ_ALLY_GUILD *p = (PACKET_ZC_REQ_ALLY_GUILD *)buf;
	char guildName[GUILD_NAME_LENGTH+1];
	memcpy(guildName, p->guildName, GUILD_NAME_LENGTH);
	guildName[GUILD_NAME_LENGTH] = 0;
	UIFrameWnd* w = g_windowMgr.MakeWindow((WINDOWID)WID_ALLYGUILDACCEPTWND);
	w->SendMsg(0, UM_SET_ALLY_GUILD_MSG, p->otherAID, (int)guildName, 0, 0);
}



void	CGameMode :: Zc_ack_req_ally_guild(const char *buf)
{
	PACKET_ZC_ACK_REQ_ALLY_GUILD *	p = (PACKET_ZC_ACK_REQ_ALLY_GUILD *)buf;
	switch (p->answer)
	{
	case	REQALLYGUILD_ALREADY_ALLIED:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REQALLYGUILD_ALREADY_ALLIED)), 0xffff, 4, 0);
		break;
	case	REQALLYGUILD_REJECT:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REQALLYGUILD_REJECT)), 0xffff, 4, 0);
		break;
	case	REQALLYGUILD_ACCEPT:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REQALLYGUILD_ACCEPT)), 0xffff00, 4, 0);
		break;
	case	REQALLYGUILD_OVERSIZE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REQALLYGUILD_OVERSIZE)), 0xffff, 4, 0);
		break;
	case	REQMYGUILD_OVERSIZE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_REQALLYMYGUILD_OVERSIZE)), 0xffff, 4, 0);
		break;
	}
}


void CGameMode::Zc_ack_change_guild_positioninfo(char const *buf)
{
	if (!g_guildInfo.m_isRequestInfo[GM_POSITION])
		return;

	PACKET_ZC_ACK_CHANGE_GUILD_POSITIONINFO *p = (PACKET_ZC_ACK_CHANGE_GUILD_POSITIONINFO *)buf;
	int packetStrLen = sizeof(PACKET_ZC_ACK_CHANGE_GUILD_POSITIONINFO);
	int size = (p->PacketLength - packetStrLen) / sizeof(GUILD_REG_POSITION_INFO);
	for (int i = 0; i < size; ++i)
	{
		GUILD_REG_POSITION_INFO info;
		memcpy(&info, buf + packetStrLen + i * sizeof(GUILD_REG_POSITION_INFO), sizeof(GUILD_REG_POSITION_INFO));

		char posName[sizeof(info.posName) + 1];
		memcpy(posName, info.posName, sizeof(info.posName));
		posName[sizeof(info.posName)] = 0;

		g_guildInfo.m_positionIdNameMap[info.positionID] = posName;
		GUILD_POSITION_INFORMATION guildPosInfo;
		guildPosInfo.positionID = info.positionID;
		guildPosInfo.posName = g_guildInfo.m_positionIdNameMap[info.positionID];
		guildPosInfo.payRate = info.payRate;
		guildPosInfo.ranking = info.ranking;
		guildPosInfo.rightOfJoin = (info.right & GUILD_RIGHT_JOIN) ? TRUE : FALSE;
		guildPosInfo.rightOfPunish = (info.right & GUILD_RIGHT_PUNISHMENT) ? TRUE : FALSE;
		g_guildInfo.UpdatePosInfo(guildPosInfo);
	}
	g_windowMgr.InvalidateGuildWnd();
}


void CGameMode::Zc_itemidentify_list(char const *buf)
{
	PACKET_ZC_ITEMIDENTIFY_LIST *p = (PACKET_ZC_ITEMIDENTIFY_LIST *)buf;
	int packetStrLen = sizeof(PACKET_ZC_ITEMIDENTIFY_LIST);
	int size = (p->PacketLength - packetStrLen) / sizeof(short);

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_ITEMIDENTIFYWND);
	wnd->SendMsg(NULL, UM_CLEAR_ITEM, 0, 0, 0, 0);

	for (int i = 0; i < size; ++i)
	{
		int index = *((short *)(buf + packetStrLen + i * sizeof(short)));
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(index);
		if (itemInfo.IsValid())
			wnd->SendMsg(NULL, UM_ADDITEM, (int)itemInfo.m_itemName.c_str(), itemInfo.m_itemIndex, 0, 0);
	}

	wnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}


void CGameMode::Zc_ack_itemidentify(char const *buf)
{
	PACKET_ZC_ACK_ITEMIDENTIFY *p = (PACKET_ZC_ACK_ITEMIDENTIFY *)buf;
	switch (p->result) {
	case ITEMIDENTIFY_SUCCESS:
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_IDENTIFY_SUCCEESS)), int(RGB(0, 255, 255)), 0, 0);
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(p->index);
		itemInfo.m_isIdentified = TRUE;
		g_session.SetItem(&itemInfo);
		g_windowMgr.RefreshItem();
		break;
	}
	case ITEMIDENTIFY_FAIL:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_IDENTIFY_FAIL)), int(RGB(255, 0, 0)), 0, 0);
		break;
	}
}


void CGameMode::Zc_itemcomposition_list(char const *buf)
{
	PACKET_ZC_ITEMCOMPOSITION_LIST *p = (PACKET_ZC_ITEMCOMPOSITION_LIST *)buf;
	int packetStrLen = sizeof(PACKET_ZC_ITEMCOMPOSITION_LIST);
	int size = (p->PacketLength - packetStrLen) / sizeof(short);

	UIItemCompositionWnd *wnd = (UIItemCompositionWnd *)g_windowMgr.MakeWindow(WID_ITEMCOMPOSITIONWND);
	wnd->SendMsg(NULL, UM_CLEAR_ITEM, 0, 0, 0, 0);
	wnd->SendMsg(NULL, UM_SET_CARD_ITEM_INDEX, this->m_lastCardItemIndex, 0, 0, 0);

	for (int i = 0; i < size; ++i)
	{
		int index = *(short *)(buf + packetStrLen + i * sizeof(short));
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(index);
		if (itemInfo.IsValid())
			wnd->SendMsg(NULL, UM_ADDITEM, (int)itemInfo.m_itemName.c_str(), itemInfo.m_itemIndex, 0, 0);
	}

	wnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}


void CGameMode::Zc_ack_itemcomposition(char const *buf)
{
	PACKET_ZC_ACK_ITEMCOMPOSITION *p = (PACKET_ZC_ACK_ITEMCOMPOSITION *)buf;

	switch (p->result) {
	case ITEMCOMPOSITON_SUCCESS: {
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_COMPOUNDING_SUCCEESS)), int(RGB(0, 255, 255)), 0, 0);

		ITEM_INFO cardItemInfo = g_session.GetItemInfoByIndex(p->cardIndex);
		ITEM_INFO equtItemInfo = g_session.GetItemInfoByIndex(p->equipIndex);
		if (!equtItemInfo.m_itemIndex || !cardItemInfo.m_itemIndex) {
			return;
		}

		for (int i = 0; i < 4; ++i) {
			if (equtItemInfo.m_slot[i] == 0) {
				equtItemInfo.m_slot[i] = atoi(cardItemInfo.m_itemName.c_str());
				break;
			}
		}

		g_session.DecItem(p->cardIndex, 1);
		g_session.SetItem(&equtItemInfo);
		break;
	}

	case ITEMCOMPOSITION_FAILURE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_COMPOUNDING_FAIL)), int(RGB(255, 0, 0)), 0, 0);
		return;
	}
}

void	CGameMode :: Zc_ack_req_hostile_guild(const char *buf)
{
	PACKET_ZC_ACK_REQ_HOSTILE_GUILD *	p =(PACKET_ZC_ACK_REQ_HOSTILE_GUILD *)buf;
	switch (p->result)
	{
	case	RESULT_HOSTILE_SUCCESS:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_HOSTILE_GUILD_SUCCEESS)), int(RGB(0, 255, 255)), 4, 0);
		break;
	case	RESULT_HOSTILE_OVERSIZE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TOO_MANY_HOSTILE_GUILD)), 0xff, 4, 0);
		break;
	case	RESULT_HOSTILE_ALREADY_HOSTILE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ALREADY_REGISTERED_HOSTILE_GUILD)), 0xff, 4, 0);
		break;
	}
}


void CGameMode::Zc_member_add(char const *buf)
{
	if (!g_guildInfo.m_isRequestInfo[1]) {
		return;
	}

	PACKET_ZC_MEMBER_ADD *	p = (PACKET_ZC_MEMBER_ADD *)buf;

	char intro[51];
	char charname[NAME_LENGTH + 1];

	memcpy(intro, p->Info.intro, 50);
	memcpy(charname, p->Info.charname, NAME_LENGTH);
	intro[50] = 0;
	charname[NAME_LENGTH] = 0;

	GUILD_MEMBER_INFORMATION guildMemberInformation;
	guildMemberInformation.AID = p->Info.AID;
	guildMemberInformation.GID = p->Info.GID;
	guildMemberInformation.CharName = charname;
	guildMemberInformation.HeadType = p->Info.head;
	guildMemberInformation.HeadPalette = p->Info.headPalette;
	guildMemberInformation.Job = p->Info.job;
	guildMemberInformation.Sex = p->Info.sex;
	guildMemberInformation.GPositionID = p->Info.positionID;
	guildMemberInformation.Level = p->Info.level;
	guildMemberInformation.Memo = intro;
	guildMemberInformation.MemberExp = p->Info.contributionExp;
	guildMemberInformation.CurrentState = p->Info.currentState;
	g_guildInfo.AddMemberInfo(&guildMemberInformation);

	g_guildInfo.RecalculateGuildExp();
	g_windowMgr.InvalidateGuildWnd();
}

void	CGameMode :: Zc_delete_related_guild(const char *buf)
{
	if (g_guildInfo.m_isRequestInfo[0]) {
		PACKET_ZC_DELETE_RELATED_GUILD * p = (PACKET_ZC_DELETE_RELATED_GUILD *)buf;

		g_guildInfo.DeleteRelatedGuild(p->OpponentGDID, p->Relation);
	}
}


void CGameMode::Zc_add_related_guild(char const *buf)
{
	if (g_guildInfo.m_isRequestInfo[0])
	{
		PACKET_ZC_ADD_RELATED_GUILD *p = (PACKET_ZC_ADD_RELATED_GUILD *)buf;

		char guildname[25];
		memcpy(guildname, p->Info.guildname, 24);
		guildname[24] = 0;

		RELATED_GUILD_INFORMATION info;
		info.relation = p->Info.relation;
		info.GDID = p->Info.GDID;
		info.GuildName = guildname;
		g_guildInfo.m_relatedGuildInfoList.push_back(info);
		g_windowMgr.InvalidateGuildWnd();
	}
}

void	CGameMode :: Zc_ack_itemrefining(const char *buf)
{
	PACKET_ZC_ACK_ITEMREFINING *	p = (PACKET_ZC_ACK_ITEMREFINING *)buf;

	switch (p->result)
	{
	case	ITEMREFINING_SUCCESS:
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(EF_REFINEOK);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_REFINING_SUCCEESS)), 0xffff00, 0, 0);
		break;
	case	ITEMREFINING_FAIL:
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(EF_REFINEFAIL);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_REFINING_FAIL)), 0xffff00, 0, 0);
		break;
	}

	ITEM_INFO	refinedItemInfo = g_session.GetItemInfoByIndex(p->itemIndex);
	refinedItemInfo.m_refiningLevel = p->refiningLevel;
	g_session.SetItem(&refinedItemInfo);
}

void	CGameMode :: Zc_notify_mapinfo (const char *buf)
{
	PACKET_ZC_NOTIFY_MAPINFO *		p = (PACKET_ZC_NOTIFY_MAPINFO *)buf;
	switch (p->type)
	{
	case TELEPORT_IMPOSSIBLE_AREA:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,  int(MsgStr(MSI_IMPOSSIBLE_TELEPORT_AREA)), 0xff,     0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_IMPOSSIBLE_TELEPORT_AREA)), 0xff,     0, 0);
		break;
	case REGISTER_IMPOSSIBLE_AREA:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,  int(MsgStr(MSI_POSSIBLE_TELEPORT_AREA)),   0xffff00, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_POSSIBLE_TELEPORT_AREA)),   0xffff00, 0, 0);
		break;
	case IMPOSSIBLE_AREA_SKILL:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,  int(MsgStr(MSI_IMPOSSIBLE_SKILL_AREA)),    0xffff00, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_IMPOSSIBLE_SKILL_AREA)),    0xffff00, 0, 0);
		break;
	case IMPOSSIBLE_AREA_USEITEM:
		m_waitingUseItemAck = 0;
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,  int(MsgStr(MSI_IMPOSSIBLE_USEITEM_AREA)),  0xffff00, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_IMPOSSIBLE_USEITEM_AREA)),  0xffff00, 0, 0);
		break;
	}
}

void	CGameMode :: Zc_ack_req_disconnect(const char *buf)
{
	PACKET_ZC_ACK_REQ_DISCONNECT *		p = (PACKET_ZC_ACK_REQ_DISCONNECT *)buf;
	switch (p->result)
	{
	case	DISCONNECTABLE_STATE:
		{
			g_connection.Disconnect();
			SendMsg(MM_QUIT);
		}
		break;
	case	NOTDISCONNECTABLE_STATE:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_EXIT_NOW)), int(RGB(255, 0, 0)), 0, 0);
		break;
	}
}

void	CGameMode :: Zc_monster_info(const char *buf)
{
	PACKET_ZC_MONSTER_INFO *	p = (PACKET_ZC_MONSTER_INFO *)buf;
	if (p->job == JT_ARCHER_GUARDIAN || p->job == JT_KNIGHT_GUARDIAN || p->job == JT_SOLDIER_GUARDIAN
		|| p->job == JT_SWORD_GUARDIAN || p->job == JT_BOW_GUARDIAN)
		return;

	UIFrameWnd * wnd = g_windowMgr.MakeWindow(WID_MONSTERINFOWND);
	wnd->SendMsg(NULL, UM_SET_MONSTER_INFO, (int)buf, 0, 0, 0);
}

void	CGameMode :: Zc_makable_item_list(const char *buf)
{
	PACKET_ZC_MAKABLEITEMLIST *	p = (PACKET_ZC_MAKABLEITEMLIST *)buf;
	MAKABLEITEM_INFO *	items;
	int		count = (p->PacketLength - sizeof(PACKET_ZC_MAKABLEITEMLIST)) / sizeof(MAKABLEITEM_INFO);
	if (count == 0) {
		g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_MAKE_ITEM), ME_OK, 1, 0, 0);
		return;
	}
	g_windowMgr.DeleteWindow(WID_MAKETARGETLISTWND);
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_MAKETARGETLISTWND);
	items = (MAKABLEITEM_INFO *)(buf + sizeof(PACKET_ZC_MAKABLEITEMLIST));
	for (int i = 0; i < count; ++i) {
		MAKABLEITEM_INFO *	info = &items[i];
		wnd->SendMsg(NULL, UM_ADDITEM, info->ITID, 0, 0, 0);
	}
}

void	CGameMode :: Zc_repair_item_list(const char *buf)
{
	PACKET_ZC_REPAIRITEMLIST *	p = (PACKET_ZC_REPAIRITEMLIST *)buf;
	REPAIRITEM_INFO *	items;
	int		count = (p->PacketLength - sizeof(PACKET_ZC_REPAIRITEMLIST)) / sizeof(REPAIRITEM_INFO);
	if (count == 0) {
		g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_MAKE_ITEM), ME_OK, 1, 0, 0);
		return;
	}
	g_windowMgr.DeleteWindow(WID_ITEMREPAIRWND);
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_ITEMREPAIRWND);
	items = (REPAIRITEM_INFO *)(buf + sizeof(PACKET_ZC_REPAIRITEMLIST));
	for (int i = 0; i < count; ++i) {
		wnd->SendMsg(NULL, UM_ADDITEM, (int)&items[i], 0, 0, 0);
	}
}

void	CGameMode :: Zc_notify_weapon_item_list(const char *buf)
{
	PACKET_ZC_NOTIFY_WEAPONITEMLIST *	p = (PACKET_ZC_NOTIFY_WEAPONITEMLIST *)buf;
	int		count = (p->PacketLength - sizeof(PACKET_ZC_NOTIFY_WEAPONITEMLIST)) / sizeof(REPAIRITEM_INFO);
	if (count == 0) {
		g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_MAKE_ITEM), ME_OK, 1, 0, 0);
		return;
	}
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_WEAPONREFINEWND);
	REPAIRITEM_INFO *items = (REPAIRITEM_INFO *)(buf + sizeof(PACKET_ZC_NOTIFY_WEAPONITEMLIST));
	for (int i = 0; i < count; ++i) {
		wnd->SendMsg(NULL, UM_ADDITEM, items[i].ITID, items[i].index, 0, 0);
	}
}

void	CGameMode :: Zc_ack_weaponrefine(const char *buf)
{
	PACKET_ZC_ACK_WEAPONREFINE *	p = (PACKET_ZC_ACK_WEAPONREFINE *)buf;
	char	msg2[256];
	char	itemName[64];
	ITEM_INFO	dummyItemInfo;
	dummyItemInfo.SetItemId(p->ITID);
	dummyItemInfo.m_refiningLevel = 0;
	dummyItemInfo.m_isIdentified = true;
	dummyItemInfo.GetItemName(itemName);

	switch (p->msg)
	{
	case	0:
		sprintf(msg2, MsgStr(MSI_ITEM_REFINE_SUCCEESS), itemName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xffff00, 0, 0);
		break;
	case	1:
		sprintf(msg2, MsgStr(MSI_ITEM_REFINE_FAIL), itemName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xcdcd00, 0, 0);
		break;
	case	2:
		sprintf(msg2, MsgStr(MSI_ITEM_REFINE_FAIL_LEVEL), itemName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xc8c8ff, 0, 0);
		break;
	case	3:
		sprintf(msg2, MsgStr(MSI_ITEM_REFINE_FAIL_MATERIAL), itemName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg2), 0xc8c8ff, 0, 0);
		break;
	}
}

void	CGameMode :: Zc_ack_itemrepair(const char *buf)
{
	PACKET_ZC_ACK_ITEMREPAIR *	p = (PACKET_ZC_ACK_ITEMREPAIR *)buf;

	switch (p->result)
	{
	case	0:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_REPAIR_SUCCEESS)), 0xffff00, 0, 0);
		break;
	case	1:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_REPAIR_FAIL)), 0xff, 0, 0);
		break;
	}
}

void	CGameMode :: Zc_ack_req_itemmaking(const char *buf)
{
	PACKET_ZC_ACK_REQMAKINGITEM *	p = (PACKET_ZC_ACK_REQMAKINGITEM *)buf;
	ITEM_INFO	item;
	char	itemDisName[256];
	char	windowText[256];

	item.SetItemId(p->ITID);
	item.m_isIdentified = true;
	item.GetItemName(itemDisName);

	if (p->result == MIRESULT_SUCCESS)
	{
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(EF_REFINEOK);
		sprintf(windowText, MsgStr(MSI_MAKE_TARGET_SUCCEESS_MSG), itemDisName);
	}
	else if (p->result == MIRESULT_FAILURE)
	{
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(EF_REFINEFAIL);
		sprintf(windowText, MsgStr(MSI_MAKE_TARGET_FAIL_MSG), itemDisName);
	}
	else if (p->result == MIRESULT_AM_SUCCESS)
	{
		if (g_modeMgr.GetGameMode()->GetWorld()->m_player->IsEffectExist(EF_PHARMACY_OK))
			g_modeMgr.GetGameMode()->GetWorld()->m_player->SendMsg(NULL, AM_DELETE_EFFECT, EF_PHARMACY_OK, 0, 0);
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(EF_PHARMACY_OK);
		sprintf(windowText, MsgStr(MSI_MAKE_TARGET_SUCCEESS_MSG), itemDisName);
	}
	else
	{
		if (g_modeMgr.GetGameMode()->GetWorld()->m_player->IsEffectExist(EF_PHARMACY_FAIL))
			g_modeMgr.GetGameMode()->GetWorld()->m_player->SendMsg(NULL, AM_DELETE_EFFECT, EF_PHARMACY_FAIL, 0, 0);
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(EF_PHARMACY_FAIL);
		sprintf(windowText, MsgStr(MSI_MAKE_TARGET_FAIL_MSG), itemDisName);
	}
}

void	CGameMode :: Zc_talkbox_chat_contents(const char *buf)
{
	PACKET_ZC_TALKBOX_CHATCONTENTS *	p = (PACKET_ZC_TALKBOX_CHATCONTENTS *)buf;

	CSkill *	theSkill = GetWorld()->GetSkillByAID(p->AID);
	if (theSkill)
		theSkill->SendMsg(NULL, AM_CHAT, (int)p->contents, 0, 0);
}

void	CGameMode :: Zc_update_mapinfo(const char *buf)
{
	PACKET_ZC_UPDATE_MAPINFO *	p = (PACKET_ZC_UPDATE_MAPINFO *)buf;
	g_modeMgr.GetGameMode()->GetWorld()->m_attr->SetCellInfo(p->type, p->xPos, p->yPos);
}
BOOL	CGameMode :: IsExceptionState(int stateId)
{
	if (stateId == 0x28 || stateId == 0x2a)
		return TRUE;
	return FALSE;
}

void	CGameMode :: Zc_Npc_Showefst_Update(const char *buf)
{
	PACKET_ZC_NPC_SHOWEFST_UPDATE *	p = (PACKET_ZC_NPC_SHOWEFST_UPDATE *)buf;
	GameModeShowEFST(p->AID, p->effectState, p->clevel, p->showEFST);
}


void CGameMode::Zc_Msg_State_Change(char const *buf)
{
	PACKET_ZC_MSG_STATE_CHANGE *p = (PACKET_ZC_MSG_STATE_CHANGE *)buf;
	if (p->AID != g_session.GetAid()) {
		CGameActor *theActor = m_world->GetGameActorByAID(p->AID);
		if (theActor)
			theActor->SendMsg(NULL, AM_STATUS_CHANGE, p->index, p->state, 0);
	}
	FRIEND_INFO friendInfo = g_session.GetPartyInfoByAid(p->AID);
	if (p->AID == g_session.GetAid() || (friendInfo.isValid && friendInfo.state == GROUPMEMBER_STATE_CONNECTED)) {
		char preMsg[256];
		sprintf(preMsg, "");
		if (p->AID != g_session.GetAid())
			sprintf(preMsg, MsgStr(MSI_CHARACTER_NAME), friendInfo.characterName.c_str());
		COLORREF textColor;
		char msg[256];
		if (p->state) {
			if (p->AID == g_session.GetAid()) {
				if (!IsExceptionState(p->index))
					stateId.erase(std::remove(stateId.begin(), stateId.end(), p->index), stateId.end());
				stateId.push_back(p->index);
			}
			textColor = RGB(0, 255, 255);
			switch (p->index) {
			case EFST_TWOHANDQUICKEN:
			case EFST_ONEHANDQUICKEN:
			case EFST_SPEARQUICKEN:
				sprintf(msg, MsgStr(MSI_INCATTACKSPEED));
				break;
			case EFST_IMPOSITIO:
				sprintf(msg, MsgStr(MSI_INCWEAPONATTACK));
				break;
			case EFST_SUFFRAGIUM:
				sprintf(msg, MsgStr(MSI_FASTPREDELAY));
				break;
			case EFST_ENCHANTPOISON:
				sprintf(msg, MsgStr(MSI_WEAPONPROPERTYPOISON));
				break;
			case EFST_ASPERSIO:
			case EFST_WEAPONPROPERTY:
				sprintf(msg, MsgStr(MSI_WEAPONPROPERTYSAINT));
				break;
			case EFST_BENEDICTIO:
				sprintf(msg, MsgStr(MSI_ARMORPROPERTYSAINT));
				break;
			case EFST_KYRIE:
				sprintf(msg, MsgStr(MSI_BARRIOR));
				break;
			case EFST_WEAPONPERFECT:
				sprintf(msg, MsgStr(MSI_PERFECTDAMAGE));
				break;
			case EFST_OVERTHRUST:
			case EFST_OVERTHRUSTMAX:
				sprintf(msg, MsgStr(MSI_OVERTHRUSTING));
				break;
			case EFST_MAXIMIZE:
				sprintf(msg, MsgStr(MSI_MAXIMIZE));
				break;
			case EFST_INC_AGI:
				sprintf(msg, MsgStr(MSI_SPEEDUP));
				break;
			case EFST_DEC_AGI:
				sprintf(msg, MsgStr(MSI_SPEEDDOWN));
				break;
			case EFST_PROVOKE:
			case EFST_ENDURE:
			case EFST_CONCENTRATION:
			case EFST_HIDING:
			case EFST_CLOAKING:
				sprintf(msg, "");
				break;
			case EFST_POISONREACT:
				sprintf(msg, MsgStr(MSI_ENST_POISONREACT));
				break;
			case EFST_QUAGMIRE:
				sprintf(msg, MsgStr(MSI_ENST_QUAGMIRE));
				break;
			case EFST_ANGELUS:
				sprintf(msg, MsgStr(MSI_ENST_ANGELUS));
				break;
			case EFST_BLESSING:
				sprintf(msg, MsgStr(MSI_ENST_BLESSING));
				break;
			case EFST_CRUCIS:
				sprintf(msg, "");
				break;
			case EFST_SLOWPOISON:
				sprintf(msg, MsgStr(MSI_ENST_SLOWPOISON));
				break;
			case EFST_MAGNIFICAT:
				sprintf(msg, MsgStr(MSI_ENST_MAGNIFICAT));
				break;
			case EFST_GLORIA:
				sprintf(msg, MsgStr(MSI_ENST_GLORIA));
				break;
			case EFST_LEXAETERNA:
				sprintf(msg, MsgStr(MSI_ENST_LEXAETERNA));
				break;
			case EFST_ADRENALINE:
			case EFST_ADRENALINE2:
				sprintf(msg, MsgStr(MSI_ENST_ADRENALINE));
				break;
			case EFST_RIDING:
			case EFST_FALCON:
				sprintf(msg, "");
				break;
			case EFST_TRICKDEAD:
				sprintf(msg, "");
				if (p->AID == g_session.GetAid()) {
					m_world->m_player->SetAction(64, 4, MT_ONETIME);
					m_world->m_player->m_isMotionFreezed = true;
				}
				break;
			case EFST_SHOUT:
				sprintf(msg, MsgStr(MSI_ENST_SHOUT));
				break;
			case EFST_ENERGYCOAT:
				sprintf(msg, MsgStr(MSI_ENST_ENERGYCOAT));
				break;
			case EFST_BROKENARMOR:
				sprintf(msg, MsgStr(MSI_ENST_BROKENAMOR));
				break;
			case EFST_BROKENWEAPON:
				sprintf(msg, MsgStr(MSI_ENST_BROKENWEAPON));
				break;
			case EFST_ILLUSION:
				sprintf(msg, "");
				break;
			case EFST_ATTHASTE_INFINITY:
			case EFST_ATTHASTE_POTION1:
			case EFST_ATTHASTE_POTION2:
			case EFST_ATTHASTE_POTION3:
				sprintf(msg, MsgStr(MSI_INCATTACKSPEED));
				break;
			case EFST_MOVHASTE_POTION:
			case EFST_MOVHASTE_INFINITY:
				sprintf(msg, MsgStr(MSI_SPEEDUP));
				break;
			case EFST_EXPLOSIONSPIRITS:
			case EFST_PROVIDENCE:
				sprintf(msg, "");
				break;
			case EFST_CHASEWALK2:
				if (p->AID == g_session.GetAid())
					PlayWave("lava_golem_move.wav");
				sprintf(msg, "");
				break;
			case EFST_GROUNDMAGIC:
			case EFST_CLAIRVOYANCE:
			case EFST_STEELBODY:
			case EFST_SG_SUN_WARM:
			case EFST_SG_MOON_WARM:
			case EFST_SG_STAR_WARM:
			case EFST_SUN_COMFORT:
			case EFST_MOON_COMFORT:
			case EFST_STAR_COMFORT:
			case EFST_AURABLADE:
			case EFST_LKCONCENTRATION:
			case EFST_BERSERK:
			case EFST_PARRYING:
			case EFST_ASSUMPTIO:
			case EFST_MAGICPOWER:
			case EFST_TRUESIGHT:
			case EFST_EDP:
			case EFST_WINDWALK:
			case EFST_CARTBOOST:
			case EFST_MELTDOWN:
			case EFST_SWORDREJECT:
			case EFST_MOON:
			case EFST_MARIONETTE_MASTER:
			case EFST_MARIONETTE:
			case EFST_BLOODING:
			case EFST_JOINTBEAT:
			case EFST_PROTECTEXP:
			case EFST_AUTOBERSERK:
			case EFST_RUN:
			case EFST_TING:
			case EFST_STORMKICK_ON:
			case EFST_DOWNKICK_ON:
			case EFST_TURNKICK_ON:
			case EFST_COUNTER_ON:
			case EFST_DODGE_ON:
			case EFST_STRUP:
			case EFST_PROPERTYUNDEAD:
			case EFST_POWERUP:
			case EFST_STOP:
			case EFST_DEVIL1:
			case EFST_PRESERVE:
			case EFST_PROTECTWEAPON:
			case EFST_PROTECTSHIELD:
			case EFST_PROTECTARMOR:
			case EFST_PROTECTHELM:
			case EFST_DOUBLECASTING:
			case EFST_TAROTCARD:
			case EFST_SOULLINK:
			case EFST_SKE:
			case EFST_KAITE:
			case EFST_KAIZEL:
			case EFST_KAAHI:
			case EFST_KAUPE:
			case EFST_SMA_READY:
			case EFST_CR_SHRINK:
			case EFST_WZ_SIGHTBLASTER:
			case EFST_DC_WINKCHARM:
			case EFST_RG_CCONFINE_M:
			case EFST_RG_CCONFINE_S:
			case EFST_EARTHSCROLL:
			case EFST_NJ_BUNSINJYUTSU:
			case EFST_NJ_UTSUSEMI:
			case EFST_NJ_NEN:
			case EFST_NJ_SUITON:
			case EFST_GS_ADJUSTMENT:
			case EFST_GS_ACCURACY:
			case EFST_GS_MADNESSCANCEL:
			case EFST_GS_GATLINGFEVER:

			case EFST_PET:
			case EFST_MENTAL:
			case EFST_EXPMEMORY:
			case EFST_PERFORMANCE:
			case EFST_GAIN:
			case EFST_GRIFFON:
			case EFST_DRIFT:
			case EFST_WALLSHIFT:
			case EFST_REINCARNATION:

			case EFST_PATTACK:
			case EFST_PSPEED:
			case EFST_PDEFENSE:
			case EFST_PCRITICAL:
			case EFST_RANKING:
			case EFST_PTRIPLE:
			case EFST_DENERGY:
			case EFST_WAVE1:
			case EFST_WAVE2:
			case EFST_WAVE3:
			case EFST_WAVE4:
			case EFST_DAURA:
			case EFST_DFREEZER:
			case EFST_DPUNISH:
			case EFST_DBARRIER:
			case EFST_DWARNING:
			case EFST_DGAUGE:
			case EFST_DACCEL:
			case EFST_DBLOCK:

			case EFST_DA_ENERGY:
			case EFST_DA_FIRSTSLOT:
			case EFST_DA_HEADDEF:
			case EFST_DA_SPACE:
			case EFST_DA_TRANSFORM:
			case EFST_DA_ITEMREBUILD:
			case EFST_DA_DARKPOWER:
			case EFST_DA_EARPLUG:
			case EFST_DA_CONTRACT:
			case EFST_DA_BLACK:
			case EFST_DA_MAGICCART:
			case EFST_CRYSTAL:
			case EFST_DA_REBUILD:
			case EFST_DA_EDARKNESS:
			case EFST_DA_EGUARDIAN:
			case EFST_DA_TIMEOUT:
			case EFST_SLOWCAST:
			case EFST_CRITICALWOUND:
			case EFST_MOVHASTE_HORSE:

			case EFST_CRITICALPERCENT:
			case EFST_HEALPLUS:
			case EFST_S_LIFEPOTION:
			case EFST_L_LIFEPOTION:
			case EFST_PLUSAVOIDVALUE:
			case EFST_PROTECT_DEF:
			case EFST_PROTECT_MDEF:
			case EFST_ARMOR_PROPERTY:

			case EFST_ATKER_BLOOD:
			case EFST_TARGET_BLOOD:

			case EFST_PARTYFLEE:
			case EFST_ANGEL_PROTECT:
				sprintf(msg, "");
				break;
			case EFST_DA_ILLUSION:
				if (p->AID == g_session.GetAid())
					g_session.m_isDA_illusionON = TRUE;
				sprintf(msg, "");
				break;
			case EFST_FOOD_STR:
			case EFST_FOOD_STR_CASH:
				sprintf(msg, MsgStr(MSI_ENST_STR));
				break;
			case EFST_FOOD_AGI:
			case EFST_FOOD_AGI_CASH:
				sprintf(msg, MsgStr(MSI_ENST_AGI));
				break;
			case EFST_FOOD_VIT:
			case EFST_FOOD_VIT_CASH:
				sprintf(msg, MsgStr(MSI_ENST_VIT));
				break;
			case EFST_FOOD_DEX:
			case EFST_FOOD_DEX_CASH:
				sprintf(msg, MsgStr(MSI_ENST_DEX));
				break;
			case EFST_FOOD_INT:
			case EFST_FOOD_INT_CASH:
				sprintf(msg, MsgStr(MSI_ENST_INT));
				break;
			case EFST_FOOD_LUK:
			case EFST_FOOD_LUK_CASH:
				sprintf(msg, MsgStr(MSI_ENST_LUK));
				break;
			case EFST_FOOD_BASICAVOIDANCE:
				sprintf(msg, MsgStr(MSI_ENST_AVOIDANCE));
				break;
			case EFST_FOOD_BASICHIT:
				sprintf(msg, MsgStr(MSI_ENST_HIT));
				break;
			case EFST_FOOD_CRITICALSUCCESSVALUE:
				sprintf(msg, MsgStr(MSI_ENST_CRITICALSUCCESSVALUE));
				break;
			case EFST_CASH_PLUSEXP:
				sprintf(msg, "");
				break;
			case EFST_CASH_DEATHPENALTY:
				sprintf(msg, MsgStr(MSI_DDEATHPENALTY));
				break;
			case EFST_CASH_RECEIVEITEM:
				sprintf(msg, MsgStr(MSI_RECEIVEITEM));
				break;
			case EFST_CASH_BOSS_ALARM:
				if (p->AID == g_session.GetAid())
					m_isBossAlarm = true;
				sprintf(msg, MsgStr(MSI_BOSS_ALARM));
				break;
			case EFST_CASH_PLUSONLYJOBEXP:
				sprintf(msg, MsgStr(MSI_PLUSONLYJOBEXP));
				break;
			default:
				if (p->AID == g_session.GetAid())
					m_world->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
				return;
			}
		} else {
			if (p->AID == g_session.GetAid()) {
				if (!IsExceptionState(p->index))
					stateId.erase(std::remove(stateId.begin(), stateId.end(), p->index), stateId.end());
			}
			textColor = RGB(255, 0, 0);
			switch (p->index) {
			case EFST_TWOHANDQUICKEN:
			case EFST_ONEHANDQUICKEN:
			case EFST_SPEARQUICKEN:
				sprintf(msg, MsgStr(MSI_DECATTACKSPEED));
				break;
			case EFST_IMPOSITIO:
				sprintf(msg, MsgStr(MSI_DECWEAPONATTACK));
				break;
			case EFST_SUFFRAGIUM:
				sprintf(msg, MsgStr(MSI_NORMALPREDELAY));
				break;
			case EFST_ENCHANTPOISON:
			case EFST_ASPERSIO:
			case EFST_WEAPONPROPERTY:
				sprintf(msg, MsgStr(MSI_WEAPONPROPERTYORIGINAL));
				break;
			case EFST_BENEDICTIO:
				sprintf(msg, MsgStr(MSI_ARMORPROPERTYORIGINAL));
				break;
			case EFST_KYRIE:
				sprintf(msg, MsgStr(MSI_DISAPPEARBARRIOR));
				break;
			case EFST_WEAPONPERFECT:
				sprintf(msg, MsgStr(MSI_DISAPPEARPERFECTDAMAGE));
				break;
			case EFST_OVERTHRUST:
			case EFST_OVERTHRUSTMAX:
				sprintf(msg, MsgStr(MSI_DISAPPEAROVERTHRUSTING));
				break;
			case EFST_MAXIMIZE:
				sprintf(msg, MsgStr(MSI_DISAPPEARMAXIMIZE));
				break;
			case EFST_DEC_AGI:
				sprintf(msg, MsgStr(MSI_SPEEDUP));
				break;
			case EFST_PROVOKE:
			case EFST_ENDURE:
			case EFST_CONCENTRATION:
			case EFST_HIDING:
			case EFST_CLOAKING:
				sprintf(msg, "");
				break;
			case EFST_POISONREACT:
				sprintf(msg, MsgStr(MSI_DSST_POISONREACT));
				break;
			case EFST_QUAGMIRE:
				sprintf(msg, MsgStr(MSI_DSST_QUAGMIRE));
				break;
			case EFST_ANGELUS:
				sprintf(msg, MsgStr(MSI_DSST_ANGELUS));
				break;
			case EFST_BLESSING:
				sprintf(msg, MsgStr(MSI_DSST_BLESSING));
				break;
			case EFST_CRUCIS:
				sprintf(msg, "");
				break;
			case EFST_SLOWPOISON:
				sprintf(msg, MsgStr(MSI_DSST_SLOWPOISON));
				break;
			case EFST_MAGNIFICAT:
				sprintf(msg, MsgStr(MSI_DSST_MAGNIFICAT));
				break;
			case EFST_GLORIA:
				sprintf(msg, MsgStr(MSI_DSST_GLORIA));
				break;
			case EFST_LEXAETERNA:
				sprintf(msg, MsgStr(MSI_DSST_LEXAETERNA));
				break;
			case EFST_ADRENALINE:
			case EFST_ADRENALINE2:
				sprintf(msg, MsgStr(MSI_DSST_ADRENALINE));
				break;
			case EFST_RIDING:
			case EFST_FALCON:
				sprintf(msg, "");
				break;
			case EFST_TRICKDEAD:
				sprintf(msg, "");
				if (p->AID == g_session.GetAid()) {
					m_world->m_player->SetState(ST_STAND);
					m_world->m_player->m_isMotionFreezed = false;
				}
				break;
			case EFST_SHOUT:
				sprintf(msg, MsgStr(MSI_DSST_SHOUT));
				break;
			case EFST_ENERGYCOAT:
				sprintf(msg, MsgStr(MSI_DSST_ENERGYCOAT));
				break;
			case EFST_BROKENARMOR:
			case EFST_BROKENWEAPON:
			case EFST_ILLUSION:
				sprintf(msg, "");
				break;
			case EFST_ATTHASTE_INFINITY:
				if (p->AID == g_session.GetAid()) {
					std::vector<int>::iterator iter = std::find(stateId.begin(), stateId.end(), EFST_ATTHASTE_INFINITY);
					if (iter != stateId.end())
						stateId.erase(iter);
				}
				sprintf(msg, MsgStr(MSI_SPEEDDOWN));
				break;
			case EFST_ATTHASTE_POTION1:
			case EFST_ATTHASTE_POTION2:
			case EFST_ATTHASTE_POTION3:
				sprintf(msg, MsgStr(MSI_DECATTACKSPEED));
				break;
			case EFST_MOVHASTE_POTION:
				sprintf(msg, MsgStr(MSI_SPEEDDOWN));
				break;
			case EFST_INC_AGI:
			case EFST_MOVHASTE_INFINITY:
				if (p->AID == g_session.GetAid()) {
					std::vector<int>::iterator iter = std::find(stateId.begin(), stateId.end(), p->index);
					if (iter != stateId.end())
						stateId.erase(iter);
				}
				sprintf(msg, MsgStr(MSI_SPEEDDOWN));
				break;
			case EFST_EXPLOSIONSPIRITS:
				sprintf(msg, "");
				break;
			case EFST_CHASEWALK2:
			case EFST_GROUNDMAGIC:
			case EFST_CLAIRVOYANCE:
			case EFST_STEELBODY:
			case EFST_SG_SUN_WARM:
			case EFST_SG_MOON_WARM:
			case EFST_SG_STAR_WARM:
			case EFST_SUN_COMFORT:
			case EFST_MOON_COMFORT:
			case EFST_STAR_COMFORT:
			case EFST_AURABLADE:
			case EFST_LKCONCENTRATION:
			case EFST_BERSERK:
			case EFST_PARRYING:
			case EFST_ASSUMPTIO:
			case EFST_MAGICPOWER:
			case EFST_TRUESIGHT:
			case EFST_EDP:
			case EFST_WINDWALK:
			case EFST_CARTBOOST:
			case EFST_MELTDOWN:
			case EFST_SWORDREJECT:
			case EFST_MOON:
			case EFST_MARIONETTE_MASTER:
			case EFST_MARIONETTE:
			case EFST_BLOODING:
			case EFST_JOINTBEAT:
			case EFST_PROTECTEXP:
			case EFST_AUTOBERSERK:
			case EFST_RUN:
			case EFST_TING:
			case EFST_STORMKICK_ON:
			case EFST_DOWNKICK_ON:
			case EFST_TURNKICK_ON:
			case EFST_COUNTER_ON:
			case EFST_DODGE_ON:
			case EFST_STRUP:
			case EFST_PROPERTYUNDEAD:
			case EFST_POWERUP:
			case EFST_STOP:
			case EFST_DEVIL1:
			case EFST_PRESERVE:
			case EFST_PROTECTWEAPON:
			case EFST_PROTECTSHIELD:
			case EFST_PROTECTARMOR:
			case EFST_PROTECTHELM:
			case EFST_DOUBLECASTING:
			case EFST_TAROTCARD:
			case EFST_SOULLINK:
			case EFST_SKE:
			case EFST_KAITE:
			case EFST_KAIZEL:
			case EFST_KAAHI:
			case EFST_KAUPE:
			case EFST_SMA_READY:
			case EFST_CR_SHRINK:
			case EFST_WZ_SIGHTBLASTER:
			case EFST_DC_WINKCHARM:
			case EFST_RG_CCONFINE_M:
			case EFST_RG_CCONFINE_S:
			case EFST_EARTHSCROLL:
			case EFST_NJ_BUNSINJYUTSU:
			case EFST_NJ_UTSUSEMI:
			case EFST_NJ_NEN:
			case EFST_NJ_SUITON:
			case EFST_GS_ADJUSTMENT:
			case EFST_GS_ACCURACY:
			case EFST_GS_MADNESSCANCEL:
			case EFST_GS_GATLINGFEVER:

			case EFST_PET:
			case EFST_MENTAL:
			case EFST_EXPMEMORY:
			case EFST_PERFORMANCE:
			case EFST_GAIN:
			case EFST_GRIFFON:
			case EFST_DRIFT:
			case EFST_WALLSHIFT:
			case EFST_REINCARNATION:

			case EFST_PATTACK:
			case EFST_PSPEED:
			case EFST_PDEFENSE:
			case EFST_PCRITICAL:
			case EFST_RANKING:
			case EFST_PTRIPLE:
			case EFST_DENERGY:
			case EFST_WAVE1:
			case EFST_WAVE2:
			case EFST_WAVE3:
			case EFST_WAVE4:
			case EFST_DAURA:
			case EFST_DFREEZER:
			case EFST_DPUNISH:
			case EFST_DBARRIER:
			case EFST_DWARNING:
			case EFST_DGAUGE:
			case EFST_DACCEL:
			case EFST_DBLOCK:

			case EFST_DA_ENERGY:
			case EFST_DA_FIRSTSLOT:
			case EFST_DA_HEADDEF:
			case EFST_DA_SPACE:
			case EFST_DA_TRANSFORM:
			case EFST_DA_ITEMREBUILD:
			case EFST_DA_DARKPOWER:
			case EFST_DA_EARPLUG:
			case EFST_DA_CONTRACT:
			case EFST_DA_BLACK:
			case EFST_DA_MAGICCART:
			case EFST_CRYSTAL:
			case EFST_DA_REBUILD:
			case EFST_DA_EDARKNESS:
			case EFST_DA_EGUARDIAN:
			case EFST_DA_TIMEOUT:
			case EFST_SLOWCAST:
			case EFST_CRITICALWOUND:
			case EFST_MOVHASTE_HORSE:

			case EFST_CRITICALPERCENT:
			case EFST_HEALPLUS:
			case EFST_S_LIFEPOTION:
			case EFST_L_LIFEPOTION:
			case EFST_PLUSAVOIDVALUE:
			case EFST_PROTECT_DEF:
			case EFST_PROTECT_MDEF:
			case EFST_ARMOR_PROPERTY:

			case EFST_ATKER_BLOOD:
			case EFST_TARGET_BLOOD:

			case EFST_PARTYFLEE:
			case EFST_ANGEL_PROTECT:
				sprintf(msg, "");
				break;
			case EFST_DA_ILLUSION:
				if (p->AID == g_session.GetAid())
					g_session.m_isDA_illusionON = FALSE;
				sprintf(msg, "");
				break;
			case EFST_FOOD_STR:
			case EFST_FOOD_STR_CASH:
				sprintf(msg, MsgStr(MSI_DSST_STR));
				break;
			case EFST_FOOD_AGI:
			case EFST_FOOD_AGI_CASH:
				sprintf(msg, MsgStr(MSI_DSST_AGI));
				break;
			case EFST_FOOD_VIT:
			case EFST_FOOD_VIT_CASH:
				sprintf(msg, MsgStr(MSI_DSST_VIT));
				break;
			case EFST_FOOD_DEX:
			case EFST_FOOD_DEX_CASH:
				sprintf(msg, MsgStr(MSI_DSST_DEX));
				break;
			case EFST_FOOD_INT:
			case EFST_FOOD_INT_CASH:
				sprintf(msg, MsgStr(MSI_DSST_INT));
				break;
			case EFST_FOOD_LUK:
			case EFST_FOOD_LUK_CASH:
				sprintf(msg, MsgStr(MSI_DSST_LUK));
				break;
			case EFST_FOOD_BASICAVOIDANCE:
				sprintf(msg, MsgStr(MSI_DSST_AVOIDANCE));
				break;
			case EFST_FOOD_BASICHIT:
				sprintf(msg, MsgStr(MSI_DSST_HIT));
				break;
			case EFST_FOOD_CRITICALSUCCESSVALUE:
				sprintf(msg, MsgStr(MSI_DSST_CRITICALSUCCESSVALUE));
				break;
			case EFST_CASH_PLUSEXP:
			case EFST_CASH_DEATHPENALTY:
			case EFST_CASH_RECEIVEITEM:
			case EFST_CASH_PLUSONLYJOBEXP:
				sprintf(msg, "");
				break;
			case EFST_CASH_BOSS_ALARM:
				if (p->AID == g_session.GetAid())
					m_isBossAlarm = false;

				break;
			default:
				if (p->AID == g_session.GetAid())
					m_world->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
				return;
			}
		}
		if (p->AID == g_session.GetAid())
			m_world->m_player->m_statusEffect->SendMsg(NULL, AM_SETCOUNT, 1, 0, 0);
		if (strlen(msg) > 1) {
			char efstMsg[510];
			sprintf(efstMsg, "%s%s", preMsg, msg);
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			if (strlen(preMsg) > 1)
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(efstMsg), int(textColor), 9, 0);
			else
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(efstMsg), int(textColor), 7, 0);
		}
		if (p->AID == g_session.GetAid())
			m_world->m_player->SendMsg(NULL, AM_STATUS_CHANGE, p->index, p->state, 0);
	}
}

void	CGameMode :: Zc_Notify_Mapproperty(const char *buf)
{
	PACKET_ZC_NOTIFY_MAPPROPERTY *p = (PACKET_ZC_NOTIFY_MAPPROPERTY *)buf;
	g_session.m_isMinEffect = g_session.m_isMinEffectOld;
	if (p->type == MAPPROPERTY_FREEPVPZONE)
		GetWorld()->SetPKZone(TRUE);
	else if (p->type == MAPPROPERTY_EVENTPVPZONE)
		GetWorld()->SetEventPVPMode(TRUE);
	else if (p->type == MAPPROPERTY_PKSERVERZONE)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_PKZONE)), 0x9b9bff, 0, 0);
	else if (p->type == MAPPROPERTY_PVPSERVERZONE)
		GetWorld()->SetPKZone(TRUE);
	else if (p->type == MAPPROPERTY_AGITZONE)
	{
		GetWorld()->SetSiegeMode(TRUE);
		g_session.m_isMinEffect = TRUE;
	}

	if (ServiceKorea != g_serviceType)
		memset(g_session.m_mapInfoTable, 0, sizeof(g_session.m_mapInfoTable));
}

void	CGameMode :: Zc_Notify_Mapproperty2(const char *buf)
{
	PACKET_ZC_NOTIFY_MAPPROPERTY2 *p = (PACKET_ZC_NOTIFY_MAPPROPERTY2 *)buf;
	g_session.m_isMinEffect = g_session.m_isMinEffectOld;
	g_session.m_curMapType = p->type;

	if (p->type == MAPTYPE_PENALTY_FREEPKZONE || p->type == MAPTYPE_NOPENALTY_FREEPKZONE)
	{
		GetWorld()->SetPKZone(TRUE);
	}
	else if (p->type == MAPTYPE_PKSERVER || p->type == MAPTYPE_PVPSERVER)
	{
	}
	else if (g_session.IsEnableSiegeMode() || (p->type == MAPTYPE_AGIT_SIEGEV15) || (p->type == MAPTYPE_AGIT))
	{
		GetWorld()->SetSiegeMode(TRUE);
		g_session.m_isMinEffect = TRUE;
	}
	else if (p->type == MAPTYPE_BATTLEFIELD)
	{
		GetWorld()->SetBattleFieldMode(TRUE);
		GetWorld()->SetPKZone(TRUE);
	}
}


void CGameMode::Zc_Notify_Ranking(char const *buf)
{
	PACKET_ZC_NOTIFY_RANKING* p = (PACKET_ZC_NOTIFY_RANKING*)buf;

	if (!m_world->IsPKZone())
		return;

	CGameActor* tActor;
	if (p->AID == g_session.GetAid() && m_world->m_player) {
		tActor = m_world->m_player;
		if (p->ranking == 1 && m_world->m_player->GetPKRank() != 1) {
			g_session.m_killTimeStartTick = timeGetTime();
		} else if (p->ranking != 1) {
			g_session.m_killTimeStartTick = 0;
		}

		if (m_world->m_player->GetPKRank() != p->ranking) {
			if (m_world->m_player->GetPKRank() > 10 && p->ranking <= 10) {
				char msgBuf[256];
				sprintf(msgBuf, MsgStr(MSI_RANK_IN_TEN), g_session.GetCharName(), p->ranking);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msgBuf), 0xffff, 0, 0);
			} else if (m_world->m_player->GetPKRank() <= 10 && p->ranking > 10) {
				char msgBuf[256];
				sprintf(msgBuf, MsgStr(MSI_RANK_OUT_TEN), g_session.GetCharName(), p->ranking);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msgBuf), 0xffff, 0, 0);
			}

			vector3d& pPos = m_world->m_player->m_pos;
			PlayWave("effect\\number_change.wav", pPos.x, 0, pPos.z);
		}
		m_world->m_player->SetPKTotal(p->total);
		m_world->m_player->SetPKRank(p->ranking);
	} else {
		tActor = m_world->GetGameActorByAID(p->AID);

		int effectState = tActor->GetEffectState();
		if (!tActor || IsEffectStateHiding(effectState) || IsEffectStateBurrow(effectState) ||
			IsEffectStateSpecialHiding(effectState)) {
			return;
		}
	}

	if (!IsGravityAid(tActor->GetGID())) {
		tActor->SendMsg(NULL, AM_CHANGE_RANK, p->total, p->ranking);
		tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_TOPRANK);
		if (p->ranking <= 10) {
			CRagEffect* TopRank = tActor->LaunchEffect(EF_TOPRANK);
			if (TopRank)
				TopRank->SendMsg(NULL, AM_SETCOUNT, p->ranking, 0, 0);
		}
	} else if (g_session.IsMasterAid(7)) {
		tActor->SendMsg(NULL, AM_CHANGE_RANK, p->total, p->ranking);
		tActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_TOPRANK);
		if (p->ranking <= 10) {
			CRagEffect* TopRank = tActor->LaunchEffect(EF_TOPRANK);
			if (TopRank)
				TopRank->SendMsg(NULL, AM_SETCOUNT, p->ranking, 0, 0);
		}
	}
}

void	CGameMode :: Zc_Notify_Effect(const char *buf)
{
	PACKET_ZC_NOTIFY_EFFECT *p = (PACKET_ZC_NOTIFY_EFFECT *)buf;
	CGameActor *pActor = GetWorld()->GetGameActorByAID(p->AID);

	if (pActor)
	{
		switch (p->effectID)
		{
		case	LEVELUP_EFFECT:
			pActor->LaunchEffect(EF_ANGEL);
			break;
		case	JOBLEVELUP_EFFECT:
			pActor->LaunchEffect(EF_JOBLVUP);
			break;
		case	LEVELUP_EFFECT2:
			pActor->LaunchEffect(EF_ANGEL2);
			break;
		case	LEVELUP_EFFECT3:
			pActor->LaunchEffect(EF_ANGEL3);
			break;
		case	JOBLEVELUP_EFFECT2:
			pActor->LaunchEffect(EF_JOBLVUP50);
			break;
		case	REFINING_FAIL_EFFECT:
			pActor->LaunchEffect(EF_REFINEFAIL);
			break;
		case	REFINING_SUCCESS_EFFECT:
			pActor->LaunchEffect(EF_REFINEOK);
			break;
		case	MAKEITEM_AM_SUCCESS_EFFECT:
			pActor->LaunchEffect(EF_PHARMACY_OK);
			break;
		case	MAKEITEM_AM_FAIL_EFFECT:
			pActor->LaunchEffect(EF_PHARMACY_FAIL);
			break;
		case	GAME_OVER_EFFECT:
			g_session.m_isShowGameOver = TRUE;
			break;
		}
	}
}

void	CGameMode :: Zc_Notify_Effect2(const char *buf)
{
	PACKET_ZC_NOTIFY_EFFECT2 *	p = (PACKET_ZC_NOTIFY_EFFECT2 *)buf;
	CGameActor *	pActor;

	if (p->AID == g_session.GetAid())
		pActor = m_world->m_player;
	else
		pActor = m_world->GetGameActorByAID(p->AID);

	if (pActor)
		pActor->LaunchEffect(p->effectID);
}

void	CGameMode :: Zc_Notify_Effect3(const char *buf)
{
	PACKET_ZC_NOTIFY_EFFECT3 *p = (PACKET_ZC_NOTIFY_EFFECT3 *)buf;
	CGameActor *pActor;

	if (p->AID == g_session.GetAid())
		pActor = m_world->m_player;
	else
		pActor = m_world->GetGameActorByAID(p->AID);

	if (pActor)
	{
		if (p->effectID == EF_HPTIME)
		{
			if (p->AID == g_session.GetAid())
			{
				g_session.ChangeXorValue();
				int prevHp = g_session.GetHp();
				int hp = p->numdata;
				hp += prevHp;
				g_session.SetHp(hp);
			}
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xff00ff00, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_SPTIME)
		{
			if (p->AID == g_session.GetAid())
			{
				g_session.ChangeXorValue();
				int prevSp = g_session.GetSp();
				int sp = p->numdata;
				sp += prevSp;
				g_session.SetSp(sp);
			}
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xff0000ff, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_GREEN_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xff00ff00, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_BLUE_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xff0000ff, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_RED_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xffff0000, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_PURPLE_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xffff00ff, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_BLACK_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xff000000, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_WHITE_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xffffffff, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_YELLOW_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xffffff00, MET_NUMBER + 100);
		}
		else if (p->effectID == EF_PINK_NUMBER)
		{
			pActor->SendMsg(NULL, AM_MAKE_NUMBER, p->numdata, 0xffff55b1, MET_NUMBER + 100);
		}

		pActor->LaunchEffect(p->effectID);
	}
}

void	CGameMode :: Zc_Start_Capture(const char *buf)
{
	m_skillUseInfo.m_skillId		= 10000;
	m_skillUseInfo.m_attackRange	= 5;
	m_skillUseInfo.m_useLevel = 0;
	m_skillUseInfo.m_skillUseType = SUT_TO_CHARACTER;
}

void	CGameMode :: Zc_Start_Couple(const char *buf)
{
	m_skillUseInfo.m_skillId		= 20000;
	m_skillUseInfo.m_attackRange	= 5;
	m_skillUseInfo.m_useLevel = 0;
	m_skillUseInfo.m_skillUseType = SUT_TO_CHARACTER;
}

void	CGameMode :: Zc_Start_Baby(const char *buf)
{
	m_skillUseInfo.m_skillId		= 20001;
	m_skillUseInfo.m_attackRange	= 5;
	m_skillUseInfo.m_useLevel = 0;
	m_skillUseInfo.m_skillUseType = SUT_TO_CHARACTER;
}

void	CGameMode :: Zc_CoupleName(const char *buf)
{
	PACKET_ZC_COUPLENAME *	p = (PACKET_ZC_COUPLENAME *)buf;
	sprintf(m_CoupleName, p->CoupleName);
}

void	CGameMode :: Zc_Congratulation(const char *buf)
{
	if (g_session.m_isEffectOn)
	{
		PACKET_ZC_CONGRATULATION *p = (PACKET_ZC_CONGRATULATION*)buf;
		CGameActor *	sActor;
		if (p->AID == g_session.GetAid())
			sActor = m_world->m_player;
		else
			sActor = m_world->GetGameActorByAID(p->AID);

		if (sActor)
		{
			if (p->AID != 0)
				sActor->LaunchEffect(EF_COLORPAPER);
		}
	}
}

void	CGameMode :: Zc_TryCapture_Monster(const char *buf)
{
	PACKET_ZC_TRYCAPTURE_MONSTER *p = (PACKET_ZC_TRYCAPTURE_MONSTER*)buf;
	if (p->result)
	{
		UIPetTamingDeceiveWnd *petTamingDeceiveWnd = g_windowMgr.GetPetTamingDeceiveWnd();
		if (petTamingDeceiveWnd)
			petTamingDeceiveWnd->SendMsg(0, UM_SET_TARGET, 1, 7, 0, 0);
	}
	else
	{
		UIPetTamingDeceiveWnd *petTamingDeceiveWnd = g_windowMgr.GetPetTamingDeceiveWnd();
		if (petTamingDeceiveWnd)
			petTamingDeceiveWnd->SendMsg(0, UM_SET_TARGET, 1, 3, 0, 0);
	}
}


void CGameMode::Zc_Change_Pet_Status(char const *buf)
{
	PACKET_ZC_CHANGESTATE_PET* p = (PACKET_ZC_CHANGESTATE_PET*)buf;
	CGameActor* actor = m_world->GetGameActorByAID(p->GID);

	switch (p->type) {
	case PET_PRE_INIT:
		g_session.m_petGID = p->GID;
		if (actor)
			g_session.m_petJob = actor->GetJob();
		break;
	case PET_RELATIONSHIP_CHANGED:
		g_session.m_petFriendly = p->data;
		if (actor)
			g_session.m_petJob = actor->GetJob();
		if (g_windowMgr.GetPetInfoWnd())
			g_windowMgr.GetPetInfoWnd()->Invalidate();
		if (g_session.GetJob() >= JT_GANGSI && g_session.GetJob() <= JT_COLLECTOR) {
			if (p->data < 100) {
				if (g_session.m_petHungry < 30) {
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_PET_STARVING)), RGB(255, 50, 50), 0, 0);
				}
			}
		}
		break;
	case PET_FULLNESS_CHANGED:
		g_session.m_petOldHungry = g_session.m_petHungry;
		g_session.m_petHungry = p->data;
		if (actor)
			g_session.m_petJob = actor->GetJob();
		if (g_windowMgr.GetPetInfoWnd())
			g_windowMgr.GetPetInfoWnd()->Invalidate();
		break;
	case PET_ACCESSARY_CHANGED:
		if (!actor)
			return;
		if (p->data) {
			actor->SetActName(g_session.GetPetAccActName(p->data));
			g_session.m_petAcc = p->data;
		} else {
			g_session.m_petAcc = 0;
			actor->SetSprJob(actor->GetJob());
		}
		if (p->GID == g_session.m_petGID) {
			if (g_windowMgr.GetPetInfoWnd())
				g_windowMgr.GetPetInfoWnd()->Invalidate();
		}
		break;
	case PET_PERFORMANCE_CHANGED:
		if (!actor)
			return;
		actor->SetPKState(p->data);
		actor->SetState(ST_STAND);
		break;
	case PET_HEADTYPE_CHANGED:
		if (!actor)
			return;
		actor->SetHeadType(HEADTYPE_LAST);
		break;
	default:
		break;
	}
}


void CGameMode::Zc_PetEgg_List(char const *buf)
{
	PACKET_ZC_PETEGG_LIST *p = (PACKET_ZC_PETEGG_LIST *)buf;
	int packetStrLen = sizeof(PACKET_ZC_PETEGG_LIST);
	int size = (p->PacketLength - packetStrLen) / sizeof(PETEGGITEM_INFO);

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_SELECTPETEGGWND);
	wnd->SendMsg(NULL, UM_CLEAR_ITEM, 0, 0, 0, 0);

	for (int i = 0; i < size; ++i)
	{
		PETEGGITEM_INFO *petEgg = (PETEGGITEM_INFO *)(buf + packetStrLen + (i * sizeof(PETEGGITEM_INFO)));
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(petEgg->index);
		if (itemInfo.IsValid())
			wnd->SendMsg(NULL, UM_ADDITEM, (int)itemInfo.m_itemName.c_str(), itemInfo.m_itemIndex, 0, 0);
	}

	wnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}


void CGameMode::Zc_Property_Pet(char const *buf)
{
	PACKET_ZC_PROPERTY_PET *p = (PACKET_ZC_PROPERTY_PET *)buf;

	if (p->nLevel < 10000)
	{
		sprintf(g_session.m_petName, p->szName);
		g_session.m_petOldHungry = g_session.m_petHungry;
		g_session.m_petHungry = p->nFullness;
		g_session.m_petFriendly = p->nRelationship;
		g_session.m_petAcc = p->ITID;
		if (g_windowMgr.GetPetInfoWnd())
		{
			g_windowMgr.GetPetInfoWnd()->m_newName->SetText(p->szName);
			g_windowMgr.GetPetInfoWnd()->Invalidate();
		}
		g_session.m_isPetNameModified = p->bModified;
		g_session.m_petLevel = p->nLevel;
	}
	else
	{
		p->nLevel -= 10000;
		UIItemCollectionWnd *wnd = (UIItemCollectionWnd *)g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		if (wnd)
		{
			wnd->m_petLevel = p->nLevel;
			wnd->m_petFullness = p->nFullness;
			wnd->m_petRelationship = p->nRelationship;
			wnd->m_petITID = p->ITID;
			wnd->m_isPetNameModified = p->bModified;
			sprintf(wnd->m_petName, p->szName);
			wnd->SendMsg(NULL, UM_SETTEXT, 0, 0, 0, 0);
		}
	}
}


void CGameMode::Zc_Feed_Pet(char const *buf)
{
	enum { ST_HOMUN_FEED = 29 };
	PACKET_ZC_FEED_PET *p = (PACKET_ZC_FEED_PET *)buf;

	if (p->PacketType == HEADER_ZC_FEED_PET)
	{
		if (p->cRet)
		{
			int friendly = g_session.GetPetFriendlyState(g_session.m_petFriendly);
			int hunger = g_session.GetPetHungryState(g_session.m_petOldHungry);
			int talk = g_session.GetPetTalkNumber(g_session.m_petJob, PM_FEEDING, hunger);
			int emotion = g_session.m_petEmotionTable[hunger][friendly][PM_FEEDING];
			SendMsg(MM_PET_ACT, talk);
			SendMsg(MM_PET_ACT, emotion);
		}
		else
		{
			char msg[256];
			ITEM_INFO item;
			item.SetItemId(p->ITID);
			sprintf(msg, MsgStr(MSI_NOT_EXIST_PET_FOOD), item.GetItemName((char *)item.m_itemName.c_str()));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), RGB(245, 30, 30), 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), RGB(245, 30, 30), 0, 0);
		}
	}
	else if (p->PacketType == HEADER_ZC_FEED_MER)
	{
		if (p->cRet)
		{
			g_session.m_homunStateId = ST_HOMUN_FEED;
		}
		else
		{
			char msg[256];
			ITEM_INFO item;
			item.SetItemId(p->ITID);
			sprintf(msg, MsgStr(MSI_NOT_EXIST_PET_FOOD), item.GetItemName((char *)item.m_itemName.c_str()));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), RGB(245, 30, 30), 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), RGB(245, 30, 30), 0, 0);

			CGameActor *actor = m_world->GetGameActorByAID(g_session.m_homunGID);
			if (actor)
				actor->SendMsg(NULL, AM_EMOTION, ET_SWEAT, 0, 0);
		}
	}
}


void CGameMode::Zc_Pet_Act(char const *buf)
{
	PACKET_ZC_PET_ACT *p = (PACKET_ZC_PET_ACT *)buf;
	CGameActor *pet = m_world->GetGameActorByAID(p->GID);
	if (pet)
	{
		if (p->data < 50000)
		{
			pet->SendMsg(NULL, AM_EMOTION, p->data, 0, 0);
		}
		else if (p->data < 90000)
		{
			p->data -= 50000;
			int hungry, act, job = 0;
			char srcStr[256], bufStr[256];
			hungry = p->data % 10;
			p->data /= 10;
			act = p->data % 10;
			p->data /= 10;
			job = p->data % 10;
			p->data /= 10;
			job += (p->data % 10) * 10;
			p->data /= 10;
			job += (p->data % 10) * 100;
			job += 1000;
			memset(srcStr, 0, 256);

			g_session.GetPetSpeak(job, hungry, act, srcStr);
			if (srcStr[0])
			{
				g_session.GetPetTalkString(srcStr, bufStr, p->GID, FALSE);
				if (strcmp(bufStr, "") != 0)
				{
					pet->SendMsg(NULL, AM_CHAT, int(bufStr), 0, 0);
					if (g_session.m_isNoTalkMsg == TRUE)
					{
					}
					else if (g_session.m_isNoTalkMsg2 == TRUE)
					{
					}
					else
					{
						g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(bufStr), RGB(250, 250, 250), 0, 0);
					}
				}
			}
		}
		else
		{
			p->data -= 90000;
			int hungry, act, job = 0;
			char bufStr[256], srcStr[256];
			hungry = p->data % 10;
			p->data /= 10;
			act = p->data % 10;
			p->data /= 10;
			job = p->data % 10;
			p->data /= 10;
			job += (p->data % 10) * 10;
			p->data /= 10;
			job += (p->data % 10) * 100;
			job += 1000;
			memset(srcStr, 0, 256);

			g_session.GetPetSpeak(job, hungry, act, srcStr);
			if (srcStr[0])
			{
				g_session.GetPetTalkString(srcStr, bufStr, p->GID, TRUE);
				if (strcmp(bufStr, "") != 0)
				{
					pet->SendMsg(NULL, AM_CHAT, int(bufStr), 0, 0);
					if (g_session.m_isNoTalkMsg == TRUE)
					{
					}
					else if (g_session.m_isNoTalkMsg2 == TRUE)
					{
					}
					else
					{
						g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(bufStr), RGB(250, 250, 250), 0, 0);
					}
				}
			}
		}
	}
}

void	CGameMode :: Zc_Par_Change_User(const char *buf)
{
	PACKET_ZC_PAR_CHANGE_USER* p = (PACKET_ZC_PAR_CHANGE_USER*)buf;

	CGameActor* actor = m_world->GetGameActorByAID(p->AID);

	sAssert(actor);

	if(!actor) return;

	switch (p->varID)
	{
	case	VAR_HONOR:
		actor->SetHonor(p->count);
		break;
	}
}

void	CGameMode :: Zc_Skill_Update(const char *buf)
{
#if defined(BALANCE_PATCH)
	PACKET_ZC_SKILL_UPDATE* p = (PACKET_ZC_SKILL_UPDATE *)buf;

	CSkill *actor = (CSkill *)m_world->GetSkillByAID(p->AID);
	if (actor)
	{
		if (actor->GetJob() > JT_TRAP && actor->GetJob() <= JT_TRAP_TALKIEBOX)
		{
			actor->SendMsg(NULL, AM_SKILL_UPDATE);
		}
	}
#endif












}

void	CGameMode :: Zc_MakingArrowList(const char *buf)
{
	PACKET_ZC_MAKINGARROW_LIST *	p = (PACKET_ZC_MAKINGARROW_LIST *)buf;
	int		packetStrLen = sizeof(PACKET_ZC_MAKINGARROW_LIST);
	int		size = (p->PacketLength - packetStrLen) / sizeof(WORD);
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_MAKINGARROWLISTWND);
	wnd->SendMsg(NULL, UM_CLEAR_ITEM, 0, 0, 0, 0);
	for (int i = 0; i < size; ++i)
	{
		WORD *	itemId = (WORD *)(buf + packetStrLen + i * sizeof(WORD));
		wnd->SendMsg(NULL, UM_ADDITEM, int(itemId), 0, 0, 0);
	}
	wnd->SendMsg(NULL, UM_SETINFO, 2, 0, 0, 0);
}

void	CGameMode :: Zc_MakingItemList(const char *buf)
{
	int		size = (*(WORD *)(buf + 2) - 4) / sizeof(WORD);
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_MAKINGARROWLISTWND);
	wnd->SendMsg(NULL, UM_CLEAR_ITEM, 0, 0, 0, 0);
	for (int i = 0; i < size; ++i)
	{
		WORD *	itemId = (WORD *)(buf + 4 + i * sizeof(WORD));
		if (i == 0)
			wnd->SendMsg(NULL, UM_SETINFO, *itemId, 0, 0, 0);
		else
			wnd->SendMsg(NULL, UM_ADDITEM, int(itemId), 0, 0, 0);
	}
}

void	CGameMode :: Zc_ChangeCartList(const char *buf)
{
	g_windowMgr.MakeWindow(WID_SELECTCARTWND);
}

void	CGameMode :: Zc_NpcSpriteChange(const char *buf)
{
	PACKET_ZC_NPCSPRITE_CHANGE *p = (PACKET_ZC_NPCSPRITE_CHANGE *)buf;
	CGameActor *actor = (CGameActor *)m_world->GetGameActorByAID(p->GID);
	if (actor)
		actor->SetSprAct(p->value, 0);
#if (_PACKET_VER >= 8-1)
	else {
	}
#endif
}

void	CGameMode :: Zc_ShowDigit(const char *buf)
{
	PACKET_ZC_SHOWDIGIT *p = (PACKET_ZC_SHOWDIGIT *)buf;
	switch(p->type)
	{
	case	0:
		g_session.m_showDigitTick = (p->value * 1000);
		g_session.m_showType = 0;
		break;
	case	1:
		g_session.m_showDigitTick = timeGetTime() + (p->value * 1000);
		g_session.m_showType = 1;
		break;
	case	2:
		g_session.m_showDigitTick = timeGetTime() - (p->value * 1000);
		g_session.m_showType = 2;
		break;
	case	3:
		g_session.m_showDigitTick = (p->value * 30) + 29;
		g_session.m_showType = 3;
		break;
	}

	g_session.m_isShowTime = TRUE;
}

void	CGameMode :: Zc_ChangeGuild(const char *buf)
{
	PACKET_ZC_CHANGE_GUILD *p = (PACKET_ZC_CHANGE_GUILD *)buf;
	CGameActor *actor = (CGameActor *)m_world->GetGameActorByAID(p->AID);
	if (actor)
	{
		actor->SetGuildInfo(p->GDID, p->emblemVersion);
	}
}


void CGameMode::Zc_GuildInfo2(char const *buf)
{
	PACKET_ZC_GUILD_INFO2 *p = (PACKET_ZC_GUILD_INFO2 *)buf;
	char guildname[NAME_LENGTH + 1];
	char masterName[NAME_LENGTH + 1];
	char manageLand[NAME_LENGTH + 1];

	memcpy(guildname, p->guildname, NAME_LENGTH);
	guildname[NAME_LENGTH] = 0;
	memcpy(masterName, p->masterName, NAME_LENGTH);
	masterName[NAME_LENGTH] = 0;
	memcpy(manageLand, p->manageLand, NAME_LENGTH);
	manageLand[NAME_LENGTH] = 0;

	g_guildInfo.m_guildName = guildname;
	g_guildInfo.m_masterName = masterName;
	g_guildInfo.m_amIMaster = g_guildInfo.m_masterName == g_session.GetCharName();
	g_guildInfo.m_manageLand = manageLand;

	g_guildInfo.m_gdid = p->GDID;
	g_guildInfo.m_guildLevel = p->level;
	g_guildInfo.m_numMember = p->userNum;
	g_guildInfo.m_numOnMember = p->userNum;
	g_guildInfo.m_numMaxMember = p->maxUserNum;
	g_guildInfo.m_memberAvgLevel = p->userAverageLevel;
	g_guildInfo.m_exp = p->exp;
	g_guildInfo.m_nextExp = p->maxExp;
	g_guildInfo.m_point = p->point;
	g_guildInfo.m_honor = p->honor;
	g_guildInfo.m_virtue = p->virtue;
	g_guildInfo.m_emblemVersion = p->emblemVersion;
	g_guildInfo.m_zeny = p->zeny;

	g_windowMgr.InvalidateGuildWnd();
}

void	CGameMode :: Zc_GuildZenyAck(const char *buf)
{
	PACKET_ZC_GUILD_ZENY_ACK *p = (PACKET_ZC_GUILD_ZENY_ACK *)buf;
	switch(p->ret)
	{
	case	ZENY_UPDATE_SUCCESS:
		{
			g_guildInfo.m_zeny += g_session.m_guildDealZeny;
			g_session.m_guildDealZeny = 0;
			UIGuildInfoManageWnd *guildInfoManageWnd = g_windowMgr.GetGuildInfoManageWnd();
			if (guildInfoManageWnd)
				guildInfoManageWnd->Invalidate();
		}
		break;
	case	ZENY_UPDATE_FAIL_MINIMUM:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GUILDZENY_NOT_ENOUGH)), int(RGB(250,35,35)), 0, 0);
		break;
	case	ZENY_UPDATE_FAIL_MAXIMUM:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_GUILDZENY_OVER)), int(RGB(250,35,35)), 0, 0);
		break;
	case	ZENY_UPDATE_FAIL_PLAYER:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OVER_OR_LOW_MONEY)), int(RGB(250,35,35)), 0, 0);
		break;
	}
}

void	CGameMode :: Zc_Dispel(const char *buf)
{
	PACKET_ZC_DISPEL *p = (PACKET_ZC_DISPEL *)buf;
	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->SendMsg(NULL, AM_DELETE_BEGIN_SPELL, 0, 0, 0);
		m_world->m_player->SendMsg(NULL, AM_DELETE_MAGIC_TARGET, 0, 0, 0);
		m_world->m_player->SendMsg(NULL, AM_DELETE_SKILL_RECHARGEGAGE, 0, 0, 0);
	}
	else
	{
		CGameActor *actor = (CGameActor *)m_world->GetGameActorByAID(p->AID);
		if (actor)
		{
			actor->SendMsg(NULL, AM_DELETE_BEGIN_SPELL, 0, 0, 0);
			actor->SendMsg(NULL, AM_DELETE_MAGIC_TARGET, 0, 0, 0);
			actor->SendMsg(NULL, AM_DELETE_SKILL_RECHARGEGAGE, 0, 0, 0);
		}
	}
}


void CGameMode::Zc_Reply_RemainTime(char const *buf)
{
	PACKET_ZC_REPLY_REMAINTIME *p = (PACKET_ZC_REPLY_REMAINTIME *)buf;
	char buffer[256];
	long currentTime;

	if (g_serviceType == ServiceChina) {
		switch (p->Result) {
		case 1:
			sprintf(buffer, MsgStr(MSI_BILLING_1), p->ExpirationDate / (60 * 60 * 24) + 1);
			break;
		case 2:
			sprintf(buffer, MsgStr(MSI_BILLING_2),
				p->RemainTime / (60 * 60),
				(p->RemainTime % (60 * 60)) / 60,
				p->RemainTime % 60);
			break;
		}
	} else {
		time(&currentTime);
		localtime(&currentTime);
		tm *expirationTime = localtime((long *)&p->ExpirationDate);

		switch (p->Result) {
		case 100:
			if (p->ExpirationDate > currentTime && p->RemainTime > 0) {
				sprintf(buffer, "You are on a combined plan. (Ends: %04d/%02d/%02d , Time left: %d h %d min %d sec)",
					expirationTime->tm_year + 1900,
					expirationTime->tm_mon + 1,
					expirationTime->tm_mday,
					p->RemainTime / (60 * 60),
					(p->RemainTime / 60) % 60,
					p->RemainTime % 60);
			} else if (p->ExpirationDate > currentTime && p->RemainTime == 0) {
				sprintf(buffer, "You are on a flat-rate plan. (Ends: %04d/%02d/%02d )",
					expirationTime->tm_year + 1900,
					expirationTime->tm_mon + 1,
					expirationTime->tm_mday);
			} else if (p->ExpirationDate <= currentTime && p->RemainTime > 0) {
				sprintf(buffer, "You are on a metered plan. (Time left: %d h %d min %d sec)",
					p->RemainTime / (60 * 60),
					(p->RemainTime / 60) % 60,
					p->RemainTime % 60);
			} else if (p->ExpirationDate <= currentTime && p->RemainTime == 0)
				sprintf(buffer, "You are on a free plan.");
			break;
		case 1000:
			sprintf(buffer, "Internet cafe plan info cannot be requested.");
			break;
		case 10000:
			sprintf(buffer, "You are on the Sakray 2-hour free plan.");
			break;
		default:
			sprintf(buffer, "Failed to request billing info.");
			break;
		}
	}

	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(buffer), 0xffffff, 0, 0);
}


void CGameMode::Zc_Info_RemainTime(char const *buf)
{
	PACKET_ZC_INFO_REMAINTIME *p = (PACKET_ZC_INFO_REMAINTIME *)buf;
	char buffer[256];

	if (g_serviceType != ServiceKorea)
	{
		if (g_serviceType != ServiceIndonesia)
			return;

		sprintf(buffer, "Sisa waktu voucher kamu adalah %d menit.", p->RemainTime / 60);
	}
	else
	{
		switch (p->Type)
		{
		case 100:
			sprintf(buffer, "Personal flat-rate time left: %d min %d sec.", p->RemainTime / 60, p->RemainTime % 60);
			break;
		case 200:
			sprintf(buffer, "Personal metered time left: %d min %d sec.", p->RemainTime / 60, p->RemainTime % 60);
			break;
		}
	}

	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(buffer), 0xffffff, 0, 0);
}


void CGameMode::Zc_Broadcast2(char const *buf)
{
	PACKET_ZC_BROADCAST2 *p = (PACKET_ZC_BROADCAST2 *)buf;
	char chatMsg[256];
	char temp[256];

	memcpy(chatMsg, buf + sizeof(PACKET_ZC_BROADCAST2), p->PacketLength - sizeof(PACKET_ZC_BROADCAST2));
	int len = strlen(chatMsg);
	std::string wMsg = chatMsg;

	int subStrSize1 = wMsg.find('#');
	int subStrSize2 = wMsg.find(':');

	if (subStrSize1 > 0 && subStrSize1 < len)
	{
		memcpy(temp, chatMsg, subStrSize1);
		temp[subStrSize1] = 0;
		wMsg = temp;

		if (subStrSize2 > 0 && subStrSize2 < len)
		{
			char temp2[256];
			memcpy(temp2, chatMsg + subStrSize2, len - subStrSize2 + 1);
			wMsg += temp2;
		}
	}

	m_broadCastTick = timeGetTime();
	if (m_broadcastBalloon == NULL)
	{
		m_broadcastBalloon = new UITransBalloonText;
		m_broadcastBalloon->Create(8, 8);
		g_windowMgr.AddWindowFront(m_broadcastBalloon);
	}
	m_broadcastBalloon->SetText(wMsg.c_str(), 72);
	m_broadcastBalloon->SetFntSize(p->fontSize);
	int r = (p->fontColor & 0xff0000) >> 16;
	int g = (p->fontColor & 0xff00) >> 8;
	int b = p->fontColor & 0xff;
	int x = (g_renderer->m_width - 640) / 2 - m_broadcastBalloon->GetWidth() / 2 + 320;
	m_broadcastBalloon->Move(x, 51);

	m_broadcastBalloon->SetFntColor(RGB(r, g, b), 0);

	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(wMsg.c_str()), int(RGB(r, g, b)), 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(wMsg.c_str()), int(RGB(r, g, b)), 0, 0);
}


void CGameMode::Zc_GuildSkillInfo(char const *buf)
{
	PACKET_ZC_GUILD_SKILLINFO *p = (PACKET_ZC_GUILD_SKILLINFO *)buf;
	m_isReqUpgradeSkillLevel = FALSE;
	g_session.ClearGuildSkillItem();
	int len = p->PacketLength - sizeof(PACKET_ZC_GUILD_SKILLINFO);
	int cnt = len / sizeof(SKILLINFO);
	const int SKID_GD_START = 10000;
	for (int a = 0; a < cnt; a++)
	{
		SKILLINFO *info = (SKILLINFO *)(buf + sizeof(PACKET_ZC_GUILD_SKILLINFO) + a * sizeof(SKILLINFO));
		SKILL_INFO skillInfo;
		skillInfo.level = info->level;
		skillInfo.SKID = info->SKID;
		skillInfo.spcost = info->spcost;
		skillInfo.type = info->type;
		skillInfo.upgradable = info->upgradable;
		skillInfo.skillName = skillInfo.GetSkillIdName();
		if (skillInfo.SKID >= SKID_GD_START && skillInfo.SKID < SKID_GD_LAST)
			g_session.AddGuildSkillItem(skillInfo, true);
	}
	g_session.SetGuildSkillPoint(p->skillPoint);
	UIGuildSkillWnd *guildSkillWnd = g_windowMgr.GetGuildSkillWnd();
	if (guildSkillWnd)
		((UIWindow *)guildSkillWnd)->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}


void CGameMode::Zc_AddItemToStore2(char const *buf)
{
	PACKET_ZC_ADD_ITEM_TO_STORE2 *	p = (PACKET_ZC_ADD_ITEM_TO_STORE2 *)buf;

	ITEM_INFO	incItem;
	incItem.m_itemIndex		= p->index;
	incItem.m_num			= p->count;
	incItem.SetItemId(p->ITID);
	incItem.m_isIdentified	= p->IsIdentified;
	incItem.m_isDamaged		= p->IsDamaged;
	incItem.m_refiningLevel = p->refiningLevel;
	incItem.m_itemType	= p->type;
	incItem.m_slot[0] = p->slot.info[0];
	incItem.m_slot[1] = p->slot.info[1];
	incItem.m_slot[2] = p->slot.info[2];
	incItem.m_slot[3] = p->slot.info[3];

	g_session.AddStoreItem(incItem);
}


void CGameMode::Zc_Add_Item_To_Cart2(char const *buf)
{
	PACKET_ZC_ADD_ITEM_TO_CART2 *	p = (PACKET_ZC_ADD_ITEM_TO_CART2 *)buf;

	ITEM_INFO	itemInfo;
	itemInfo.m_itemIndex		= p->index;
	itemInfo.m_num			= p->count;
	itemInfo.SetItemId(p->ITID);
	itemInfo.m_isIdentified	= p->IsIdentified;
	itemInfo.m_isDamaged		= p->IsDamaged;
	itemInfo.m_refiningLevel = p->refiningLevel;
	itemInfo.m_itemType	= p->type;
	itemInfo.m_slot[0] = p->slot.info[0];
	itemInfo.m_slot[1] = p->slot.info[1];
	itemInfo.m_slot[2] = p->slot.info[2];
	itemInfo.m_slot[3] = p->slot.info[3];

	g_session.AddMerchantItem(itemInfo);
}

void	CGameMode :: Sc_Ack_Encryption(const char *buf)
{
	ErrorMsg("No Packet Encryption !!!");
}


void CGameMode::Zc_Use_Item_Ack2(char const *buf)
{
	PACKET_ZC_USE_ITEM_ACK2 *p = (PACKET_ZC_USE_ITEM_ACK2 *)buf;
	if (p->AID == g_session.GetAid())
	{
		m_waitingUseItemAck = false;
		Trace("Can UseItemAck");
		if (p->result)
		{
			ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(p->index);
			if (itemInfo.IsValid())
			{
				g_session.DecItem(p->index, itemInfo.m_num - p->count);
				if (itemInfo.GetItemId() == 607)
					m_isUseYgdrasil = FALSE;
			}
		}
	}

	if (p->result)
		MakePotionEffect(p->id, p->AID);
}


void CGameMode::Zc_Skill_Entry2(char const *buf)
{
	PACKET_ZC_SKILL_ENTRY2 *p = (PACKET_ZC_SKILL_ENTRY2 *)buf;
	SKILLSTANDENTRYSTRUCT ss;
	ss.AID = p->AID;
	ss.job = p->job;
	ss.xPos = p->xPos;
	ss.yPos = p->yPos;
	ss.isVisible = p->isVisible = 1;
	ss.creatorAID = p->creatorAID;

	if (p->isContens == true)
	{
		CGraffiSkill *skill = new CGraffiSkill;
		GetWorld()->m_gameObjectList.push_back(skill);
		GetWorld()->m_skillList.push_back((CSkill *)skill);
		skill->SendMsg(NULL, AM_SETMASTERGID, p->AID, 0, 0);
		skill->SendMsg(NULL, AM_SET_SKILL, (int)p->msg, p->xPos, p->yPos);
		return;
	}

	CSkill *skill = m_world->GetSkillByAID(p->AID);
	if (ss.creatorAID != g_session.GetAid())
	{
		std::list<DWORD>::iterator i = m_visibleTrapList.begin();
		while (i != m_visibleTrapList.end())
		{
			DWORD trapAid = *i;
			if (p->AID == trapAid)
			{
				i = m_visibleTrapList.erase(i);
				ss.isVisible = p->isVisible = 0;
				break;
			}
			i++;
		}
	}
	if (ss.isVisible)
		m_visibleTrapList.push_back(p->AID);

	if (skill == NULL)
	{
		if (p->job >= JT_WARPNPC && p->job <= JT_2004_JOB_BEGIN)
		{
			skill = new CSkill;
		}
		else
		{
			sAssert(false);
			return;
		}
		GetWorld()->m_gameObjectList.push_back(skill);
		GetWorld()->m_skillList.push_back(skill);
	}
	skill->SendMsg(NULL, AM_SKILLSTANDENTRY, (int)(&ss), 0, 0);
}

void	CGameMode :: MonsterTalk(CGameActor *theActor, int job, int stateId, int skillId, DWORD gid)
{
	char bufStr[256];

	g_session.GetMonsterTalkInfo(job, stateId, skillId, bufStr, gid);
	if (strcmp(bufStr, ""))
	{
		theActor->SendMsg(NULL, AM_CHAT, int(bufStr), 0, 0);
		if (g_session.m_isNoTalkMsg == TRUE)
		{
		}
		else if (g_session.m_isNoTalkMsg2 == TRUE)
		{
		}
		else
		{
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(bufStr), 0xfafafa, 0, 0);
		}
	}
}

void	CGameMode :: Zc_AutoSpellList(const char *buf)
{
	PACKET_ZC_AUTOSPELLLIST *p = (PACKET_ZC_AUTOSPELLLIST*)buf;

	char pass = 0;
	for (int a = 0; a < 7; a++)
	{
		if (p->SKID[a])
			pass++;
	}

	if (pass > 0)
	{
		UIFrameWnd * wnd = g_windowMgr.MakeWindow(WID_SPELLLISTWND);
		if (wnd)
		{
			for (int b = 0; b < 7; b++)
			{
				if (p->SKID[b])
					wnd->SendMsg(NULL, UM_ADDITEM, p->SKID[b], 0, 0, 0);
			}
		}
	}
}


void CGameMode::Zc_DevotionList(char const *buf)
{
	PACKET_ZC_DEVOTIONLIST *p = (PACKET_ZC_DEVOTIONLIST *)buf;

	if (g_session.m_isEffectOn) {
		CGameActor *sActor = NULL;

		if (p->myAID == g_session.GetAid())
			sActor = m_world->m_player;
		else
			sActor = m_world->GetGameActorByAID(p->myAID);

		if (sActor) {
			for (int i = 0; i < 5; ++i)
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_LINELINK);
		}

		for (int i = 0; i < 5; ++i) {
			if (p->AID[i] != 0) {
				vector3d pos;
				pos.x = p->myAID;
				pos.y = p->AID[i];

				if (sActor) {
					CRagEffect *effect = sActor->LaunchEffect(EF_LINELINK);
					if (effect) {
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
		}
	}
}


void CGameMode::Zc_Spirits(char const *buf)
{
	PACKET_ZC_SPIRITS *p = (PACKET_ZC_SPIRITS *)buf;

	if (g_session.m_isEffectOn)
	{
		CGameActor *sActor = NULL;

		if (p->AID == g_session.GetAid())
			sActor = m_world->m_player;
		else
			sActor = m_world->GetGameActorByAID(p->AID);

		if (sActor)
		{
			if (sActor->GetJob() == JT_MONK_B)
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI2);
			else if (sActor->GetJob() == JT_GUNSLINGER)
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI3);
			else
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI);

			if (p->AID != 0)
			{
				if (IsEffectStateBurrow(sActor->GetEffectState()) || IsEffectStateHiding(sActor->GetEffectState()))
				{
					if (sActor->GetJob() == JT_MONK_B)
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI2);
					else if (sActor->GetJob() == JT_GUNSLINGER)
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI3);
					else
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI);
				}
				else
				{
					CRagEffect *effect;
					if (sActor->GetJob() == JT_MONK_B)
						effect = sActor->LaunchEffect(EF_CHOOKGI2);
					else if (sActor->GetJob() == JT_GUNSLINGER)
						effect = sActor->LaunchEffect(EF_CHOOKGI3);
					else
						effect = sActor->LaunchEffect(EF_CHOOKGI);

					vector3d pos;
					pos.x = p->num;

					if (effect)
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
		}
	}
}


void CGameMode::Zc_Spirits2(char const *buf)
{
	PACKET_ZC_SPIRITS2 *p = (PACKET_ZC_SPIRITS2 *)buf;

	if (g_session.m_isEffectOn)
	{
		CGameActor *sActor = NULL;

		if (p->AID == g_session.GetAid())
			sActor = m_world->m_player;
		else
			sActor = m_world->GetGameActorByAID(p->AID);

		if (sActor)
		{
			if (IsEffectStateBurrow(sActor->GetEffectState()) || IsEffectStateHiding(sActor->GetEffectState()))
			{
				if (sActor->GetJob() == JT_MONK_B)
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI2);
				else if (sActor->GetJob() == JT_GUNSLINGER)
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI3);
				else
					sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_CHOOKGI);
			}
			else if (sActor->GetJob() == JT_MONK_B)
			{
				if (!sActor->IsEffectExist(EF_CHOOKGI2))
				{
					if (p->AID != 0)
					{
						CRagEffect *effect = sActor->LaunchEffect(EF_CHOOKGI2);
						vector3d pos;
						pos.x = p->num;
						if (effect)
							effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			else if (sActor->GetJob() == JT_GUNSLINGER)
			{
				if (!sActor->IsEffectExist(EF_CHOOKGI3))
				{
					if (p->AID != 0)
					{
						CRagEffect *effect = sActor->LaunchEffect(EF_CHOOKGI3);
						vector3d pos;
						pos.x = p->num;
						if (effect)
							effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			else
			{
				if (!sActor->IsEffectExist(EF_CHOOKGI))
				{
					if (p->AID != 0)
					{
						CRagEffect *effect = sActor->LaunchEffect(EF_CHOOKGI);
						vector3d pos;
						pos.x = p->num;
						if (effect)
							effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
		}
	}
}


void CGameMode::Zc_BladeStop(char const *buf)
{
	PACKET_ZC_BLADESTOP *p = (PACKET_ZC_BLADESTOP *)buf;
	if (p->flag)
	{
		CGameActor *sActor;
		CGameActor *tActor;
		if (p->srcAID == g_session.GetAid())
			sActor = m_world->m_player;
		else sActor = m_world->GetGameActorByAID(p->srcAID);
		if (p->destAID == g_session.GetAid())
			tActor = m_world->m_player;
		else tActor = m_world->GetGameActorByAID(p->destAID);
		if (!sActor || !tActor)
			return;
		sActor->SetForceAnimation(96, 4, 4, 96, 4, TRUE, 99999999, FALSE);
		sActor->m_isSprArgbFixed = true;
		sActor->SetArgb(-1, 64, 64, 64);
		tActor->m_isMotionFreezed = TRUE;
		tActor->m_isSprArgbFixed = true;
		tActor->SetArgb(-1, 64, 64, 64);
	}
	else
	{
		CGameActor *sActor;
		CGameActor *tActor;
		if (p->srcAID == g_session.GetAid())
			sActor = m_world->m_player;
		else sActor = m_world->GetGameActorByAID(p->srcAID);
		if (p->destAID == g_session.GetAid())
			tActor = m_world->m_player;
		else tActor = m_world->GetGameActorByAID(p->destAID);
		if (sActor)
		{
			sActor->SetForceAnimation(96, 4, 4, 96, 4, TRUE, 1, FALSE);
			sActor->m_isSprArgbFixed = false;
			sActor->SetArgb(-1, 255, 255, 255);
		}
		if (tActor)
		{
			tActor->m_isMotionFreezed = FALSE;
			tActor->m_isSprArgbFixed = false;
			tActor->SetArgb(-1, 255, 255, 255);
		}
	}
}

void	CGameMode :: Zc_Open_EditDlgStr(const char *buf)
{
	PACKET_ZC_OPEN_EDITDLGSTR *p = (PACKET_ZC_OPEN_EDITDLGSTR *)buf;
	CGameActor *theNpc = m_world->GetGameActorByAID(p->NAID);
	if (theNpc || p->NAID == 0)
	{
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NPCTEXTEDITDIALOGWND);
		wnd->SendMsg(NULL, UM_SETNAID, p->NAID, 0, 0, 0);
	}
}


void CGameMode::Zc_Sound(char const *buf)
{
	PACKET_ZC_SOUND *p = (PACKET_ZC_SOUND *)buf;
	switch (p->act)
	{
	case SOUND_ACT_PLAY:
	{
		CGameActor *theActor = GetWorld()->GetGameActorByAID(p->NAID);
		if (theActor)
		{
			vector3d &pPos = theActor->m_pos;
			PlayWave(p->fileName, GetWorld()->m_player->m_pos.x - pPos.x, 0, GetWorld()->m_player->m_pos.z - pPos.z);
		}
		else
		{
			PlayWave(p->fileName);
		}
		break;
	}
	case SOUND_ACT_REPEAT:
	{
		PLAY_WAVE_INFO info;
		info.wavName = p->fileName;
		info.term = p->term;
		info.endTick = timeGetTime() + p->term;
		info.nAID = p->NAID;
		m_playWaveList.push_back(info);
		PlayWave(p->fileName);
		break;
	}
	case SOUNT_ACT_STOP:
	{
		std::vector<PLAY_WAVE_INFO>::iterator i = m_playWaveList.begin();
		while (i != m_playWaveList.end())
		{
			PLAY_WAVE_INFO info = *i;
			if (!strcmp(info.wavName.c_str(), p->fileName))
			{
				m_playWaveList.erase(i);
				break;
			}
			++i;
		}
		break;
	}
	}
}

void	CGameMode :: Zc_Monster_Talk(const char * buf)
{
}

void	CGameMode :: Zc_ComboDelay(const char *buf)
{
	PACKET_ZC_COMBODELAY *p = (PACKET_ZC_COMBODELAY *)buf;

	g_session.SetForceAnimEndTick(p->AID, p->delayTime);
	if (p->AID == g_session.GetAid())
	{
		m_world->m_player->SetForceAnimEndTick(p->delayTime);
		return;
	}

	CGameActor *actor = m_world->GetGameActorByAID(p->AID);
	if (actor)
	{
		actor->SetForceAnimEndTick(p->delayTime);
	}
}


void CGameMode::Zc_Sprite_Change2(char const *buf)
{
	PACKET_ZC_SPRITE_CHANGE2 *p = (PACKET_ZC_SPRITE_CHANGE2 *)buf;

	CGameActor *tActor = NULL;
	int right = 0, left = 0, type = 0;
	if (p->GID == g_session.GetAid()) {
		tActor = m_world->m_player;
		switch (p->type) {
		case SPRITETYPE_JOB:
			if (p->value > JT_2004_JOB_BEGIN && p->value < JT_NOVICE_B) {
				g_session.m_bodyPalette = 1;
#if defined(_UPDATE2004B)
				if (p->value == JT_HUNTER_H || p->value == JT_PRIEST_H)
					g_session.m_bodyPalette = 0;
				if (p->value == JT_BLACKSMITH_H || p->value == JT_ASSASSIN_H)
					g_session.m_bodyPalette = 0;
				if (p->value == JT_KNIGHT_H)
					g_session.m_bodyPalette = 0;
				if (p->value == JT_WIZARD_H)
					g_session.m_bodyPalette = 0;
#endif
#if defined(_UPDATE2005A)
				g_session.m_bodyPalette = 0;
				if (p->value > JT_2004_JOB_BEGIN && p->value <= JT_THIEF_H)
					g_session.m_bodyPalette = 1;
#endif
			} else {
				g_session.m_bodyPalette = 0;
			}
			g_session.SetJob(p->value);
			break;
		case SPRITETYPE_HEAD:
			g_session.m_head = p->value;
			break;
		case SPRITETYPE_WEAPON:
		{
			right = (p->value >> 16) & 0xffff;
			left = p->value & 0xffff;
			type = g_session.GetWeaponType(right);
			if (left && type == -1) {
				g_session.m_shield = right;
				g_session.m_weapon = (right << 16) | left;
				p->value = g_session.m_weapon;
			} else {
				g_session.m_weapon = p->value;
			}
			break;
		}
		case SPRITETYPE_ACCESSORY:
			g_session.m_accessory = p->value;
			break;
		case SPRITETYPE_HEADPALETTE:
			g_session.m_headPalette = p->value;
			break;
		case SPRITETYPE_BODYPALATTE:
			g_session.m_bodyPalette = p->value;
			break;
		case SPRITETYPE_ACCESSORY2:
			g_session.m_accessory2 = p->value;
			break;
		case SPRITETYPE_ACCESSORY3:
			g_session.m_accessory3 = p->value;
			break;
		}
	} else if (tActor = m_world->GetGameActorByAID(p->GID)) {
	} else if (tActor = m_world->GetSkillByAID(p->GID)) {
		if (p->type == SPRITETYPE_JOB)
			tActor->SendMsg(NULL, AM_SETJOB, p->value);
		return;
	} else {
		return;
	}

	if (tActor) {
		switch (p->type) {
		case SPRITETYPE_JOB:
			tActor->SetJob(p->value);
			if (p->value > JT_2004_JOB_BEGIN && p->value < JT_NOVICE_B) {
				tActor->SetBodyPalette(1);
#if defined(_UPDATE2004B)
				if (p->value == JT_HUNTER_H || p->value == JT_PRIEST_H)
					tActor->SetBodyPalette(0);
				if (p->value == JT_BLACKSMITH_H || p->value == JT_ASSASSIN_H)
					tActor->SetBodyPalette(0);
				if (p->value == JT_KNIGHT_H)
					tActor->SetBodyPalette(0);
				if (p->value == JT_WIZARD_H)
					tActor->SetBodyPalette(0);
#endif
#if defined(_UPDATE2005A)
				tActor->SetBodyPalette(0);
				if (p->value > JT_2004_JOB_BEGIN && p->value <= JT_THIEF_H)
					tActor->SetBodyPalette(1);
#endif
			} else {
				tActor->SetBodyPalette(0);
			}
			tActor->SetImfFileName();
			tActor->SetHeadPaletteName();
			tActor->SetBodyPaletteName();
			break;
		case SPRITETYPE_HEAD:
			tActor->SetSprHead(p->value);
			tActor->SetImfFileName();
			tActor->SetHeadPaletteName();
			break;
		case SPRITETYPE_WEAPON:
		{
			right = (p->value >> 16) & 0xffff;
			left = p->value & 0xffff;
			type = g_session.GetWeaponType(right);
			if (tActor->GetJob() == JT_ASSASSIN || tActor->GetJob() == JT_ASSASSIN_H || tActor->GetJob() == JT_ASSASSIN_B)
				tActor->SetSprWeapon2(p->value);
			else
				tActor->SetSprWeapon2(left);
			if (type == -1 || !right)
				tActor->SetSprShield(right);
			break;
		}
		case SPRITETYPE_ACCESSORY:
			tActor->SetSprAccessory(p->value);
			break;
		case SPRITETYPE_HEADPALETTE:
			tActor->SetHeadPalette(p->value);
			break;
		case SPRITETYPE_BODYPALATTE:
			tActor->SetBodyPalette(p->value);
			break;
		case SPRITETYPE_ACCESSORY2:
			tActor->SetSprAccessory2(p->value);
			break;
		case SPRITETYPE_ACCESSORY3:
			tActor->SetSprAccessory3(p->value);
			break;
		case SPRITETYPE_SHOE:
			tActor->SetSprShoe(p->value);
			break;
		}
	}
}


static inline void DecodePosDirNewentry(BYTE *packet, int &rsx, int &rsy, int &rdir)
{
	rsx = packet[0] & 0xff;
	rsx = (rsx << 2) | ((packet[1] & 0xc0) >> 6);
	rsy = packet[1] & 0x3f;
	rsy = (rsy << 4) | ((packet[2] & 0xf0) >> 4);
	rdir = packet[2] & 0xf;
}

void CGameMode::Zc_Notify_Newentry(char const *buf)
{
	PACKET_ZC_NOTIFY_NEWENTRY *p = (PACKET_ZC_NOTIFY_NEWENTRY *)buf;
	STANDENTRYSTRUCT s;

	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;

	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.state = PCSTATE_STAND;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;

	s.shield = p->shield;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		if (theActor->GetJob() > JT_WARPNPC && theActor->GetJob() < JT_2004_JOB_BEGIN) {
			DWORD gdid = (DWORD)(p->accessory2 << 16);
			gdid |= p->accessory3;
			DWORD emblemVer = p->shield;
			if (gdid > 0 && theActor->GetGdid() == 0)
				theActor->SetGuildInfo(gdid, emblemVer);
		}

		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY, (int)(&s));

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		int isPc = false;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			isPc = true;
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITSTANDENTRY, (int)(&s), 2);

		if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
			DWORD gdid = (DWORD)(p->accessory2 << 16);
			gdid |= p->accessory3;
			if (gdid > 0)
				newGameActor->SetGuildInfo(gdid, p->shield);
		}

		if (isPc) {
			if (!IsGravityAid(p->GID) && !IsEffectStateSpecialHiding(p->effectState))
				newGameActor->LaunchEffect(EF_ENTRY2);
		}

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}

void CGameMode::Zc_Notify_Newentry2(char const *buf)
{
	PACKET_ZC_NOTIFY_NEWENTRY2 *p = (PACKET_ZC_NOTIFY_NEWENTRY2 *)buf;
	STANDENTRYSTRUCT s;

	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;

	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.shield = 0;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;

	s.state = PCSTATE_STAND;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY2, (int)(&s));

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		int isPc = false;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			isPc = true;
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITSTANDENTRY2, (int)(&s), 2);

		if (isPc) {
			if (!IsGravityAid(p->GID) && !IsEffectStateSpecialHiding(p->effectState))
				newGameActor->LaunchEffect(EF_ENTRY2);
		}

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}

		if (newGameActor && (m_partyPosList.find(p->GID) != m_partyPosList.end() ||
			m_guildPosList.find(p->GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}


void CGameMode::Zc_Notify_Newentry3(char const *buf)
{
	PACKET_ZC_NOTIFY_NEWENTRY3 *p = (PACKET_ZC_NOTIFY_NEWENTRY3 *)buf;
	STANDENTRYSTRUCT s;

	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;

	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.shield = 0;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;

	s.state = PCSTATE_STAND;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY2, (int)(&s));
		theActor->m_clevel = p->clevel;

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		int isPc = false;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			isPc = true;
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITSTANDENTRY2, (int)(&s), 2);
		newGameActor->m_clevel = p->clevel;

		if (isPc) {
			if (!IsGravityAid(p->GID) && !IsEffectStateSpecialHiding(p->effectState))
				newGameActor->LaunchEffect(EF_ENTRY2);
		}

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}

		if (newGameActor && (m_partyPosList.find(p->GID) != m_partyPosList.end() ||
			m_guildPosList.find(p->GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}

void CGameMode::Zc_Notify_Newentry4(char const *buf)
{
	PACKET_ZC_NOTIFY_NEWENTRY4 *p = (PACKET_ZC_NOTIFY_NEWENTRY4 *)buf;
	STANDENTRYSTRUCT s;

	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;

	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.shield = 0;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;

	s.state = PCSTATE_STAND;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		theActor->m_charfont = p->charfont;
		theActor->m_clevel = p->clevel;

		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY2, (int)(&s), 0, 0);

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		int isPc = false;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			isPc = true;
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITSTANDENTRY2, (int)(&s), 2, 0);
		newGameActor->m_clevel = p->clevel;

		if (isPc) {
			newGameActor->m_charfont = p->charfont;
			if (!IsGravityAid(p->GID) && !IsEffectStateSpecialHiding(p->effectState))
				newGameActor->LaunchEffect(EF_ENTRY2);
		}

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}

		if (newGameActor && (m_partyPosList.find(p->GID) != m_partyPosList.end() ||
			m_guildPosList.find(p->GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}

void CGameMode::Zc_Notify_Standentry2(char const *buf)
{
	PACKET_ZC_NOTIFY_STANDENTRY2 *p = (PACKET_ZC_NOTIFY_STANDENTRY2 *)buf;
	STANDENTRYSTRUCT s;

	s.shield = 0;
	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;

	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.state = p->state;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY2, (int)(&s), 0, 0);

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		if (newGameActor) {
			newGameActor->SendMsg(NULL, AM_INITSTANDENTRY2, (int)(&s), 0, 0);

			if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
				if (!g_session.IsPc(newGameActor->GetJob())) {
					if (g_session.IsGuardian(newGameActor->GetJob()))
						MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				} else {
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				}
			}
		}

		if (newGameActor && (m_partyPosList.find(p->GID) != m_partyPosList.end() ||
			m_guildPosList.find(p->GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}


void CGameMode::Zc_Notify_Standentry3(char const *buf)
{
	PACKET_ZC_NOTIFY_STANDENTRY3 *p = (PACKET_ZC_NOTIFY_STANDENTRY3 *)buf;
	STANDENTRYSTRUCT s;

	s.shield = 0;
	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;

	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.state = p->state;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;
	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY2, (int)(&s), 0, 0);

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		if (newGameActor) {
			newGameActor->SendMsg(NULL, AM_INITSTANDENTRY2, (int)(&s), 0, 0);

			if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
				if (!g_session.IsPc(newGameActor->GetJob())) {
					if (g_session.IsGuardian(newGameActor->GetJob()))
						MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				} else {
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				}
			}
		}

		if (newGameActor && (m_partyPosList.find(p->GID) != m_partyPosList.end() ||
			m_guildPosList.find(p->GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}

void CGameMode::Zc_Notify_Standentry4(char const *buf)
{
	PACKET_ZC_NOTIFY_STANDENTRY4 *p = (PACKET_ZC_NOTIFY_STANDENTRY4 *)buf;
	STANDENTRYSTRUCT s;

	s.shield = 0;
	s.job = p->job;
	s.sex = p->sex;
	s.GID = p->GID;
	s.speed = p->speed;
	s.xSize = p->xSize;
	s.ySize = p->ySize;

	s.head = p->head;
	s.weapon = p->weapon;
	s.accessory = p->accessory;
	s.state = p->state;
	s.healthState = p->healthState;
	s.bodyState = p->bodyState;
	s.effectState = p->effectState;
	s.isPKModeON = p->isPKModeON;

	s.accessory2 = p->accessory2;
	s.accessory3 = p->accessory3;
	s.headpalette = p->headpalette;
	s.bodypalette = p->bodypalette;
	s.headDir = p->headDir;

	s.guildId = p->GUID;
	s.guildEmblemVersion = p->GEmblemVer;
	s.honor = p->honor;
	s.virtue = p->virtue;

	DecodePosDirNewentry(p->PosDir, s.x, s.y, s.dir);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		theActor->m_charfont = p->charfont;
		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				theActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		theActor->SendMsg(NULL, AM_STANDENTRY2, (int)(&s), 0, 0);

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}
	} else {
		CGameActor *newGameActor;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;

			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			s.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		if (newGameActor) {
			newGameActor->SendMsg(NULL, AM_INITSTANDENTRY2, (int)(&s), 0, 0);

			if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
				if (!g_session.IsPc(newGameActor->GetJob())) {
					if (g_session.IsGuardian(newGameActor->GetJob()))
						MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				} else {
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
				}
			}

			newGameActor->m_charfont = p->charfont;
		}

		if (newGameActor && (m_partyPosList.find(p->GID) != m_partyPosList.end() ||
			m_guildPosList.find(p->GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}


void CGameMode::Zc_Notify_Entry_HP(int JT_ID, short CX, short CY)
{
	STANDENTRYSTRUCT s;

	s.GID = JT_ID;
	s.speed = 150;
	s.xSize = 5;
	s.ySize = 5;

	if (JT_ID == 1) {
		s.job = g_Aco_Job;
		s.sex = 0;
		s.head = g_Aco_Hair;
		s.weapon = 0;
		s.accessory = 0;
		s.state = 2;
		s.accessory2 = 0;
		s.accessory3 = 0;
		s.headpalette = g_Aco_Hair_Color;
		s.bodypalette = 0;
		s.headDir = 0;
		s.x = 14;
		s.y = -5;
		s.dir = 4;
	} else if (JT_ID == 2) {
		s.job = g_BS_Job;
		s.sex = 1;
		s.head = g_BS_Hair;
		s.weapon = 0;
		s.accessory = 0x6b;
		s.state = 0;
		s.accessory2 = 0x36;
		s.accessory3 = 0x49;
		s.headpalette = g_BS_Hair_Color;
		s.bodypalette = 0;
		s.headDir = 0;
		s.x = 13;
		s.y = -5;
		s.dir = 4;
	} else if (JT_ID == 10) {
		s.job = rand() % 7;
		if (s.job == 4)
			s.job = 0xfce;
		s.sex = 0;
		s.head = rand() % 24 + 1;
		s.weapon = 0;
		s.accessory = 0x137;
		s.state = 0;
		s.accessory2 = 0xbc;
		s.accessory3 = 0;
		s.headpalette = 0;
		s.bodypalette = 0;
		s.headDir = 0;
		s.x = 10;
		s.y = 3;
		s.dir = 3;
	} else if (JT_ID == 11) {
		s.job = rand() % 2 ? 10 : 18;
		s.sex = 0;
		s.head = rand() % 24 + 1;
		s.weapon = 0;
		s.accessory = 0x10b;
		s.state = 0;
		s.accessory2 = 0x103;
		s.accessory3 = 0x7d;
		s.headpalette = 0;
		s.bodypalette = 0;
		s.headDir = 0;
		s.x = 10;
		s.y = 3;
		s.dir = 3;
	} else if (JT_ID == 3) {
		s.job = Boy_Job;
		s.sex = 1;
		s.head = g_BS_Hair;
		s.weapon = 0x477;
		s.accessory = 0x2b;
		s.state = 0;
		s.accessory2 = 0x103;
		s.accessory3 = 0x49;
		s.headpalette = g_BS_Hair_Color;
		s.bodypalette = 0;
		s.headDir = 0;
		s.dir = 4;
	} else if (JT_ID == 4) {
		s.job = 0xfd1;
		s.sex = 1;
		s.head = 4;
		s.weapon = 0;
		s.accessory = 0x11d;
		s.state = 0;
		s.accessory2 = 0;
		s.accessory3 = 0;
		s.headpalette = 0;
		s.bodypalette = 0;
		s.headDir = 0;
		s.dir = 4;
	} else if (JT_ID == 5) {
		s.job = 0xfd1;
		s.sex = 0;
		s.head = 5;
		s.weapon = 0;
		s.accessory = 0x10b;
		s.state = 0;
		s.accessory2 = 0x39;
		s.accessory3 = 0;
		s.headpalette = 0;
		s.bodypalette = 0;
		s.headDir = 0;
		s.dir = 4;
	} else if (JT_ID == 6) {
		s.job = 0xf;
		s.sex = 1;
		s.head = 3;
		s.weapon = 0;
		s.accessory = 0x27;
		s.state = 0;
		s.accessory2 = 0x38;
		s.accessory3 = 0xbb;
		s.headpalette = 0;
		s.bodypalette = 0;
		s.headDir = 0;
		s.dir = 4;
	}

	if (CX != 100 && CY != 100) {
		s.x = CX;
		s.y = CY;
	}

	CGameActor *newGameActor = GetWorld()->GetGameActorByAID(s.GID);
	if (newGameActor) {
		newGameActor->SendMsg(NULL, AM_STANDENTRY2, int(&s), 0, 0);
		if (JT_ID == 10)
			newGameActor->LaunchEffect(EF_ENTRY2);
		return;
	}

	newGameActor = new CPc;
	GetWorld()->m_gameObjectList.push_back(newGameActor);
	GetWorld()->m_actorList.push_back(newGameActor);
	newGameActor->SendMsg(NULL, AM_INITSTANDENTRY2, int(&s), 2, 0);

	newGameActor->m_clevel = 1;
	newGameActor->m_charfont = 0;
	if (JT_ID == 1) {
		newGameActor->m_Exp = 0;
		newGameActor->m_MaxHp = 1000;
		newGameActor->m_Hp = 1000;
		newGameActor->m_Str = 1;
		newGameActor->m_MaxSp = 100;
		newGameActor->m_Sp = 100;
		newGameActor->m_Int = 1;
		newGameActor->m_Dex = 1;
		newGameActor->m_Vit = 1;
		newGameActor->m_Luk = 1;
		newGameActor->m_Agi = 1;
		newGameActor->m_clevel = 1;
		g_session.SetGold(20000);
		g_session.HP_EquipItemById(0x5f0);
		g_session.HP_EquipItemById(0x1505);
		g_session.HP_EquipItemById(0x8dd);
		g_session.HP_EquipItemById(0x8ee);
		g_session.m_DayCount = 1;
		g_session.SetNextexp(GetNextExpValue(newGameActor->m_clevel) - GetNextExpValue(newGameActor->m_clevel - 1));
		g_session.SetStatusData(newGameActor);
	} else if (JT_ID == 11) {
		newGameActor->SetEffectState(0x400);
		newGameActor->SendMsg(NULL, AM_MAKE_CART, 0, 0, 0);
	} else if (JT_ID == 3) {
		newGameActor->m_MaxHp = 9999;
		newGameActor->m_Hp = 9999;
	}

	newGameActor->LaunchEffect(EF_ENTRY2);
}

void CGameMode::Zc_Notify_Entry_Mob(int JT_Num, int F2, short CX, short CY)
{
	STANDENTRYSTRUCT s;
	char msg[256];

	s.GID = F2;
	s.xSize = 5;
	s.ySize = 5;
	s.x = 0;
	s.y = 0;
	s.dir = 0;
	s.shield = 0;
	s.job = JT_Num;
	s.sex = 0;
	s.speed = 150;
	s.head = 1;
	s.weapon = 0;
	s.accessory = 0;
	s.state = 0;
	s.healthState = 0;
	s.bodyState = 0;
	s.effectState = 0;
	s.isPKModeON = 0;
	s.accessory2 = 0;
	s.accessory3 = 0;
	s.headpalette = 0;
	s.bodypalette = 0;
	s.headDir = 0;
	s.guildId = 0;
	s.guildEmblemVersion = 0;
	s.honor = 0;
	s.virtue = 0;

	if (CX == 100 && CY == 100) {
		s.x = rand() % 12 + 14;
		s.y = rand() % 13 - 6;
	} else {
		s.x = CX;
		s.y = CY;
	}

	CWorld *world = m_world;
	s.dir = 4;
	if (world->GetGameActorByAID(s.GID)) {
		sprintf(msg, "[ A mob with this GID exists ]");
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xc8c8c8, 0, 0);
		g_modeMgr.GetGameMode()->GetWorld()->RemoveGameActor(s.GID);
	}

	CGameActor *newGameActor = NewNpc(JT_Num);
	m_world->m_gameObjectList.push_back(newGameActor);
	m_world->m_actorList.push_back(newGameActor);

	if (JT_Num == 0x72c) {
		newGameActor->m_MaxHp = 0x12c;
		if (g_curStory < 10)
			newGameActor->m_Exp = 5;
		else
			newGameActor->m_Exp = 3;
		newGameActor->m_Agi = 0;
		newGameActor->m_Vit = 1;
		newGameActor->m_Str = 4;
		newGameActor->m_clevel = 1;
	} else if (JT_Num == 0x3ea) {
		newGameActor->m_MaxHp = 0x7530;
		newGameActor->m_Exp = 0;
		newGameActor->m_Agi = 0;
		newGameActor->m_Vit = 0x3e7;
		newGameActor->m_Str = 1;
		newGameActor->m_clevel = 1;
	} else if (JT_Num == 0x48f) {
		newGameActor->m_MaxHp = 0x3c;
		if (g_curStory < 10)
			newGameActor->m_Exp = 2;
		else
			newGameActor->m_Exp = 1;
		newGameActor->m_Agi = 5;
		newGameActor->m_Vit = 0;
		newGameActor->m_Str = 6;
		newGameActor->m_clevel = 1;
	} else if (JT_Num == 0x48a) {
		newGameActor->m_MaxHp = 0xdc;
		if (g_curStory < 10)
			newGameActor->m_Exp = 4;
		else
			newGameActor->m_Exp = 2;
		newGameActor->m_Agi = 10;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 3;
		newGameActor->m_Int = 1;
		newGameActor->m_clevel = 1;
	} else if (JT_Num == 0x6f9) {
		newGameActor->m_MaxHp = 0xbb8;
		if (g_curStory < 10)
			newGameActor->m_Exp = 0x32;
		else
			newGameActor->m_Exp = 0x19;
		newGameActor->m_Agi = 5;
		newGameActor->m_Vit = 10;
		newGameActor->m_Str = 10;
		newGameActor->m_clevel = 0x5a;
	} else if (JT_Num == 0x402) {
		newGameActor->m_MaxHp = 0x258;
		if (g_curStory < 20)
			newGameActor->m_Exp = 5;
		else
			newGameActor->m_Exp = 2;
		newGameActor->m_Agi = 10;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 9;
		newGameActor->m_clevel = 2;
	} else if (JT_Num == 0x5ef) {
		newGameActor->m_MaxHp = 0x1c2;
		if (g_curStory < 20)
			newGameActor->m_Exp = 3;
		else
			newGameActor->m_Exp = 1;
		newGameActor->m_Vit = 0;
		newGameActor->m_Agi = 10;
		newGameActor->m_Str = 8;
		newGameActor->m_Int = 10;
		newGameActor->m_clevel = 2;
	} else if (JT_Num == 0x5d5) {
		newGameActor->m_MaxHp = 0x2bc;
		if (g_curStory < 20)
			newGameActor->m_Exp = 7;
		else
			newGameActor->m_Exp = 4;
		newGameActor->m_Agi = 8;
		newGameActor->m_Vit = 3;
		newGameActor->m_Str = 0xb;
		newGameActor->m_clevel = 2;
	} else if (JT_Num == 0x6c6) {
		newGameActor->m_MaxHp = 0x1770;
		newGameActor->m_Exp = 1;
		newGameActor->m_Agi = 5;
		newGameActor->m_Vit = 10;
		newGameActor->m_Str = 10;
		newGameActor->m_clevel = 0x5b;
	} else if (JT_Num == 0x6c5) {
		newGameActor->m_MaxHp = 0x1770;
		if (g_curStory < 20)
			newGameActor->m_Exp = 0x4b;
		else
			newGameActor->m_Exp = 0x26;
		newGameActor->m_Agi = 0xc;
		newGameActor->m_Vit = 0xf;
		newGameActor->m_Str = 0x14;
		newGameActor->m_clevel = 0x5b;
	} else if (JT_Num == 0x698) {
		newGameActor->m_MaxHp = 0x4b0;
		if (g_curStory < 30)
			newGameActor->m_Exp = 8;
		else
			newGameActor->m_Exp = 4;
		newGameActor->m_Agi = 0xe;
		newGameActor->m_Vit = 5;
		newGameActor->m_Str = 0xf;
		newGameActor->m_clevel = 3;
	} else if (JT_Num == 0x501) {
		newGameActor->m_MaxHp = 0x1f4;
		if (g_curStory < 30)
			newGameActor->m_Exp = 6;
		else
			newGameActor->m_Exp = 3;
		newGameActor->m_Agi = 0x14;
		newGameActor->m_Vit = 0;
		newGameActor->m_Str = 0x10;
		newGameActor->m_clevel = 3;
	} else if (JT_Num == 0x510) {
		newGameActor->m_MaxHp = 0x3e8;
		if (g_curStory < 30)
			newGameActor->m_Exp = 9;
		else
			newGameActor->m_Exp = 4;
		newGameActor->m_Agi = 0x12;
		newGameActor->m_Vit = 6;
		newGameActor->m_Str = 0x14;
		newGameActor->m_clevel = 3;
	} else if (JT_Num == 0x75d) {
		newGameActor->m_MaxHp = 0x2af8;
		if (g_curStory < 30)
			newGameActor->m_Exp = 0x64;
		else
			newGameActor->m_Exp = 0x32;
		newGameActor->m_Agi = 0x12;
		newGameActor->m_Vit = 0x14;
		newGameActor->m_Str = 0x1e;
		newGameActor->m_clevel = 0x5c;
	} else if (JT_Num >= 0x52c && JT_Num <= 0x553) {
		newGameActor->m_Exp = 0;
		newGameActor->m_MaxHp = 1;
		newGameActor->m_Agi = 1;
		newGameActor->m_Vit = 1;
		newGameActor->m_Str = 1;
		newGameActor->m_clevel = 1;
		s.x = 0x17;
		s.y = 0;
	} else if (JT_Num == 0x492) {
		newGameActor->m_MaxHp = 0x708;
		if (g_curStory < 40)
			newGameActor->m_Exp = 0xb;
		else
			newGameActor->m_Exp = 5;
		newGameActor->m_Agi = 0x18;
		newGameActor->m_Vit = 6;
		newGameActor->m_Str = 0x17;
		newGameActor->m_clevel = 4;
	} else if (JT_Num == 0x5e8) {
		newGameActor->m_MaxHp = 0x898;
		if (g_curStory < 40)
			newGameActor->m_Exp = 0xe;
		else
			newGameActor->m_Exp = 7;
		newGameActor->m_Agi = 0x1e;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x1b;
		newGameActor->m_clevel = 4;
	} else if (JT_Num == 0x5e6) {
		newGameActor->m_MaxHp = 0x5dc;
		if (g_curStory < 40)
			newGameActor->m_Exp = 10;
		else
			newGameActor->m_Exp = 5;
		newGameActor->m_Agi = 0x17;
		newGameActor->m_Vit = 3;
		newGameActor->m_Str = 0x14;
		newGameActor->m_clevel = 4;
	} else if (JT_Num == 0x55d) {
		newGameActor->m_MaxHp = 0x4e20;
		if (g_curStory < 40)
			newGameActor->m_Exp = 0x96;
		else
			newGameActor->m_Exp = 0x4b;
		newGameActor->m_Agi = 0x1a;
		newGameActor->m_Vit = 0x19;
		newGameActor->m_Str = 0x28;
		newGameActor->m_clevel = 0x5d;
	} else if (JT_Num == 0x55e) {
		newGameActor->m_MaxHp = 0x1f40;
		newGameActor->m_Exp = 1;
		newGameActor->m_Agi = 0x63;
		newGameActor->m_Vit = 10;
		newGameActor->m_Str = 0x28;
		newGameActor->m_clevel = 0x5e;
	} else if (JT_Num == 0x781) {
		newGameActor->m_MaxHp = 0x960;
		if (g_curStory < 50)
			newGameActor->m_Exp = 0x14;
		else
			newGameActor->m_Exp = 10;
		newGameActor->m_Agi = 0x1e;
		newGameActor->m_Vit = 1;
		newGameActor->m_Str = 0x1f;
		newGameActor->m_clevel = 5;
	} else if (JT_Num == 0x691) {
		newGameActor->m_MaxHp = 0xbb8;
		if (g_curStory < 50)
			newGameActor->m_Exp = 0x11;
		else
			newGameActor->m_Exp = 9;
		newGameActor->m_Agi = 0x21;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x1b;
		newGameActor->m_Int = 0x28;
		newGameActor->m_clevel = 5;
	} else if (JT_Num == 0x5e1) {
		newGameActor->m_MaxHp = 0xb54;
		if (g_curStory < 50)
			newGameActor->m_Exp = 0x10;
		else
			newGameActor->m_Exp = 8;
		newGameActor->m_Agi = 0x23;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x20;
		newGameActor->m_clevel = 5;
	} else if (JT_Num == 0x6d7) {
		newGameActor->m_MaxHp = 0x61a8;
		if (g_curStory < 50)
			newGameActor->m_Exp = 0xc8;
		else
			newGameActor->m_Exp = 0x64;
		newGameActor->m_Agi = 0x27;
		newGameActor->m_Vit = 0x3c;
		newGameActor->m_Str = 0x32;
		newGameActor->m_Int = 0xbe;
		newGameActor->m_clevel = 0x5e;
	} else if (JT_Num == 0x5d9) {
		newGameActor->m_MaxHp = 0x2328;
		newGameActor->m_Exp = 1;
		newGameActor->m_Agi = 0;
		newGameActor->m_Vit = 0x3b6;
		newGameActor->m_Str = 0x32;
		newGameActor->m_clevel = 0x5f;
	} else if (JT_Num == 0x55a) {
		newGameActor->m_MaxHp = 0x1388;
		if (g_curStory < 60)
			newGameActor->m_Exp = 0x1e;
		else
			newGameActor->m_Exp = 0xf;
		newGameActor->m_Agi = 0x2d;
		newGameActor->m_Vit = 3;
		newGameActor->m_Str = 0x30;
		newGameActor->m_clevel = 6;
	} else if (JT_Num == 0x561) {
		newGameActor->m_MaxHp = 0x1194;
		if (g_curStory < 60)
			newGameActor->m_Exp = 0x19;
		else
			newGameActor->m_Exp = 0xd;
		newGameActor->m_Agi = 0x32;
		newGameActor->m_Vit = 5;
		newGameActor->m_Str = 0x34;
		newGameActor->m_clevel = 6;
	} else if (JT_Num == 0x6b2) {
		newGameActor->m_MaxHp = 0x157c;
		if (g_curStory < 60)
			newGameActor->m_Exp = 0x18;
		else
			newGameActor->m_Exp = 0xc;
		newGameActor->m_Agi = 0x37;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x32;
		newGameActor->m_clevel = 6;
	} else if (JT_Num == 0x4da) {
		newGameActor->m_MaxHp = 0x4e20;
		if (g_curStory < 60)
			newGameActor->m_Exp = 0x12c;
		else
			newGameActor->m_Exp = 0x96;
		newGameActor->m_Agi = 0x27;
		newGameActor->m_Vit = 0x3c;
		newGameActor->m_Str = 0x32;
		newGameActor->m_Int = 0x46;
		newGameActor->m_clevel = 0x63;
	} else if (JT_Num == 0x6c7) {
		newGameActor->m_MaxHp = 0x1f40;
		if (g_curStory < 80)
			newGameActor->m_Exp = 0x28;
		else
			newGameActor->m_Exp = 0x14;
		newGameActor->m_Vit = 9;
		newGameActor->m_Agi = 0x46;
		newGameActor->m_Str = 0x3c;
		newGameActor->m_Int = 0x46;
		newGameActor->m_clevel = 7;
	} else if (JT_Num == 0x6c9) {
		newGameActor->m_MaxHp = 0x1b58;
		if (g_curStory < 80)
			newGameActor->m_Exp = 0x28;
		else
			newGameActor->m_Exp = 0x14;
		newGameActor->m_Agi = 0x41;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x37;
		newGameActor->m_Int = 0x8c;
		newGameActor->m_clevel = 7;
	} else if (JT_Num == 0x74b) {
		newGameActor->m_MaxHp = 0x1d4c;
		if (g_curStory < 80)
			newGameActor->m_Exp = 0x28;
		else
			newGameActor->m_Exp = 0x14;
		newGameActor->m_Agi = 0x3c;
		newGameActor->m_Vit = 7;
		newGameActor->m_Str = 0x33;
		newGameActor->m_Int = 0x50;
		newGameActor->m_clevel = 7;
	} else if (JT_Num == 0x520) {
		newGameActor->m_MaxHp = 0x4e20;
		newGameActor->m_Exp = 0x1f4;
		newGameActor->m_Agi = 0x37;
		newGameActor->m_Vit = 0x63;
		newGameActor->m_Str = 0x21;
		newGameActor->m_clevel = 0x58;
	} else if (JT_Num == 0x503) {
		newGameActor->m_MaxHp = 0x7530;
		if (g_curStory < 70)
			newGameActor->m_Exp = 0x1c2;
		else
			newGameActor->m_Exp = 0xe1;
		newGameActor->m_Agi = 0x46;
		newGameActor->m_Vit = 0x5a;
		newGameActor->m_Str = 0x32;
		newGameActor->m_clevel = 0x60;
	} else if (JT_Num == 0x4e4) {
		newGameActor->m_MaxHp = 0x2ee0;
		if (g_curStory < 90)
			newGameActor->m_Exp = 0x41;
		else
			newGameActor->m_Exp = 0x23;
		newGameActor->m_Agi = 0x50;
		newGameActor->m_Vit = 2;
		newGameActor->m_Str = 0x44;
		newGameActor->m_clevel = 7;
	} else if (JT_Num == 0x5eb) {
		newGameActor->m_MaxHp = 0x2328;
		if (g_curStory < 80)
			newGameActor->m_Exp = 0x32;
		else
			newGameActor->m_Exp = 0x19;
		newGameActor->m_Agi = 0x4d;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x31;
		newGameActor->m_Int = 0x1e;
		newGameActor->m_clevel = 7;
	} else if (JT_Num == 0x77c) {
		newGameActor->m_MaxHp = 0x7530;
		if (g_curStory < 80)
			newGameActor->m_Exp = 0x1f4;
		else
			newGameActor->m_Exp = 0xfa;
		newGameActor->m_Agi = 0x50;
		newGameActor->m_Vit = 0x87;
		newGameActor->m_Str = 0x46;
		newGameActor->m_clevel = 0x61;
	} else if (JT_Num == 0x6f1) {
		newGameActor->m_MaxHp = 0x3a98;
		if (g_curStory < 90)
			newGameActor->m_Exp = 0x4b;
		else
			newGameActor->m_Exp = 0x28;
		newGameActor->m_Agi = 0x5a;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x41;
		newGameActor->m_clevel = 8;
	} else if (JT_Num == 0x47e) {
		newGameActor->m_MaxHp = 0x36b0;
		if (g_curStory < 90)
			newGameActor->m_Exp = 0x4b;
		else
			newGameActor->m_Exp = 0x28;
		newGameActor->m_Agi = 0x63;
		newGameActor->m_Vit = 7;
		newGameActor->m_Str = 0x46;
		newGameActor->m_clevel = 8;
	} else if (JT_Num == 0x509) {
		newGameActor->m_MaxHp = 0x7530;
		if (g_curStory < 90)
			newGameActor->m_Exp = 0x258;
		else
			newGameActor->m_Exp = 0x12c;
		newGameActor->m_Agi = 0x63;
		newGameActor->m_Vit = 0x5a;
		newGameActor->m_Str = 0x5a;
		newGameActor->m_Int = 0x64;
		newGameActor->m_clevel = 0x62;
	} else if (JT_Num == 0x487) {
		newGameActor->m_MaxHp = 0x4e20;
		if (g_curStory < 100)
			newGameActor->m_Exp = 0x6e;
		else
			newGameActor->m_Exp = 0x37;
		newGameActor->m_Agi = 0x73;
		newGameActor->m_Vit = 4;
		newGameActor->m_Str = 0x50;
		newGameActor->m_clevel = 9;
	} else if (JT_Num == 0x43f) {
		newGameActor->m_MaxHp = 0x55f0;
		if (g_curStory < 100)
			newGameActor->m_Exp = 0x78;
		else
			newGameActor->m_Exp = 0x3c;
		newGameActor->m_Agi = 0x78;
		newGameActor->m_Vit = 7;
		newGameActor->m_Str = 0x50;
		newGameActor->m_clevel = 9;
	} else if (JT_Num == 0x50f) {
		newGameActor->m_MaxHp = 0x4650;
		if (g_curStory < 100)
			newGameActor->m_Exp = 0x64;
		else
			newGameActor->m_Exp = 0x32;
		newGameActor->m_Agi = 0x6e;
		newGameActor->m_Vit = 9;
		newGameActor->m_Str = 0x50;
		newGameActor->m_clevel = 9;
	} else if (JT_Num == 0x6ac) {
		newGameActor->m_MaxHp = 0x2710;
		if (g_curStory < 90)
			newGameActor->m_Exp = 0x320;
		else
			newGameActor->m_Exp = 0x190;
		newGameActor->m_Agi = 0x82;
		newGameActor->m_Vit = 9;
		newGameActor->m_Str = 0x5a;
		newGameActor->m_Int = 0x29;
		newGameActor->m_clevel = 0x62;
	}

	newGameActor->m_Hp = newGameActor->m_MaxHp;
	newGameActor->SendMsg(NULL, AM_INITSTANDENTRY, int(&s), 1, 0);
	newGameActor->LaunchEffect(EF_ENTRY2);
	bat_mobskill_turn = 0;
	GameModeShowEFST(100, 0, newGameActor->m_clevel, 0);
}

void CGameMode::Zc_Notify_Moveentry2(char const *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY2 *p = (PACKET_ZC_NOTIFY_MOVEENTRY2 *)buf;

	MOVEENTRYSTRUCT ms;
	ms.GID = p->GID;
	ms.job = p->job;
	ms.sex = p->sex;
	ms.speed = p->speed;
	ms.head = p->head;
	ms.weapon = p->weapon;
	ms.accessory = p->accessory;
	ms.healthState = p->healthState;
	ms.bodyState = p->bodyState;
	ms.effectState = p->effectState;
	ms.isPKModeON = p->isPKModeON;
	ms.accessory2 = p->accessory2;
	ms.accessory3 = p->accessory3;
	ms.headpalette = p->headpalette;
	ms.bodypalette = p->bodypalette;
	ms.headDir = p->headDir;
	ms.guildId = p->GUID;
	ms.guildEmblemVersion = p->GEmblemVer;
	ms.honor = p->honor;
	ms.virtue = p->virtue;
	ms.moveStartTime = p->moveStartTime;

	DecodeSrcDst(p->MoveData, ms.sx, ms.sy, ms.dx, ms.dy, ms.cellX, ms.cellY);
	g_session.UpdateServerTime(ms.moveStartTime);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		theActor->SendMsg(NULL, AM_MOVEENTRY2, (int)(&ms), 0, 0);

		if (ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0 && theActor->GetGdid() == 0)
				theActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}

		if (theActor->GetGroupMemberAndVisible()) {
			std::map<unsigned long, ColorCellPos>::iterator it = m_partyPosList.find(theActor->GetGID());
			if (it != m_partyPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
			it = m_guildPosList.find(theActor->GetGID());
			if (it != m_guildPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
		}
	} else {
		CGameActor *newGameActor;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(p->job)) {
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;
			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			ms.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITMOVEENTRY2, (int)(&ms), 0, 0);

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}

		if (newGameActor && (m_partyPosList.find(ms.GID) != m_partyPosList.end() ||
			m_guildPosList.find(ms.GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}

#include "rag_budget_fill.h"
void CGameMode::Zc_Notify_Moveentry3(char const *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY3 *p = (PACKET_ZC_NOTIFY_MOVEENTRY3 *)buf;

	MOVEENTRYSTRUCT ms;
	ms.GID = p->GID;
	ms.job = p->job;
	ms.sex = p->sex;
	ms.speed = p->speed;
	ms.head = p->head;
	ms.weapon = p->weapon;
	ms.accessory = p->accessory;
	ms.healthState = p->healthState;
	ms.bodyState = p->bodyState;
	ms.effectState = p->effectState;
	ms.isPKModeON = p->isPKModeON;
	ms.accessory2 = p->accessory2;
	ms.accessory3 = p->accessory3;
	ms.headpalette = p->headpalette;
	ms.bodypalette = p->bodypalette;
	ms.headDir = p->headDir;
	ms.guildId = p->GUID;
	ms.guildEmblemVersion = p->GEmblemVer;
	ms.honor = p->honor;
	ms.virtue = p->virtue;
	ms.moveStartTime = p->moveStartTime;

	DecodeSrcDst(p->MoveData, ms.sx, ms.sy, ms.dx, ms.dy, ms.cellX, ms.cellY);
	g_session.UpdateServerTime(ms.moveStartTime);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		theActor->m_clevel = p->clevel;
		theActor->SendMsg(NULL, AM_MOVEENTRY2, (int)(&ms), 0, 0);

		if (ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0 && theActor->GetGdid() == 0)
				theActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}

		if (theActor->GetGroupMemberAndVisible()) {
			std::map<unsigned long, ColorCellPos>::iterator it = m_partyPosList.find(theActor->GetGID());
			if (it != m_partyPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
			it = m_guildPosList.find(theActor->GetGID());
			if (it != m_guildPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
		}
	} else { RAG_BUDGET_FILL(4)
		CGameActor *newGameActor;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) ||
			p->objecttype == NPC_MERSOL_TYPE || g_session.IsMerOrHomun(p->job))
		{
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;
			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			ms.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		newGameActor->m_clevel = p->clevel;

		if (p->objecttype == NPC_MERSOL_TYPE)
			newGameActor->m_objectType = p->objecttype;

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITMOVEENTRY2, (int)(&ms), 0, 0);

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}

		if (newGameActor && (m_partyPosList.find(ms.GID) != m_partyPosList.end() ||
			m_guildPosList.find(ms.GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}
void CGameMode::Zc_Notify_Moveentry4(char const *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY4 *p = (PACKET_ZC_NOTIFY_MOVEENTRY4 *)buf;

	MOVEENTRYSTRUCT ms;
	ms.GID = p->GID;
	ms.job = p->job;
	ms.sex = p->sex;
	ms.speed = p->speed;
	ms.head = p->head;
	ms.weapon = p->weapon;
	ms.accessory = p->accessory;
	ms.healthState = p->healthState;
	ms.bodyState = p->bodyState;
	ms.effectState = p->effectState;
	ms.isPKModeON = p->isPKModeON;
	ms.accessory2 = p->accessory2;
	ms.accessory3 = p->accessory3;
	ms.headpalette = p->headpalette;
	ms.bodypalette = p->bodypalette;
	ms.headDir = p->headDir;
	ms.guildId = p->GUID;
	ms.guildEmblemVersion = p->GEmblemVer;
	ms.honor = p->honor;
	ms.virtue = p->virtue;
	ms.moveStartTime = p->moveStartTime;

	DecodeSrcDst(p->MoveData, ms.sx, ms.sy, ms.dx, ms.dy, ms.cellX, ms.cellY);
	g_session.UpdateServerTime(ms.moveStartTime);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(p->GID);
	if (theActor) {
		theActor->m_clevel = p->clevel;
		theActor->m_charfont = p->charfont;
		theActor->SendMsg(NULL, AM_MOVEENTRY2, (int)(&ms), 0, 0);

		if (ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0 && theActor->GetGdid() == 0)
				theActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, p->GUID, p->GEmblemVer);
			}
		}

		if (theActor->GetGroupMemberAndVisible()) {
			std::map<unsigned long, ColorCellPos>::iterator it = m_partyPosList.find(theActor->GetGID());
			if (it != m_partyPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
			it = m_guildPosList.find(theActor->GetGID());
			if (it != m_guildPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
		}
	} else {
		CGameActor *newGameActor;
		if ((p->job >= JT_WARPNPC && p->job < JT_2004_JOB_BEGIN) ||
			p->objecttype == NPC_MERSOL_TYPE || g_session.IsMerOrHomun(p->job))
		{
			if (g_session.m_homunGID == p->GID)
				g_session.m_homunAlive = true;
			if (g_session.m_merGID == p->GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(p->job);
			ms.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == p->GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		if (p->objecttype == NPC_MERSOL_TYPE)
			newGameActor->m_objectType = p->objecttype;

		GetWorld()->m_gameObjectList.push_back(newGameActor);
		GetWorld()->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITMOVEENTRY2, (int)(&ms), 0, 0);

		newGameActor->m_charfont = p->charfont;
		newGameActor->m_clevel = p->clevel;

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, p->GUID, p->GEmblemVer);
			}
		}

		if (newGameActor && (m_partyPosList.find(ms.GID) != m_partyPosList.end() ||
			m_guildPosList.find(ms.GID) != m_guildPosList.end()))
		{
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(p->GID, p->effectState, p->clevel, p->virtue);
}


void CGameMode::GameModeShowEFST(int GID, int effectState, int clevel, int showEFST)
{
	CGameActor *sActor = m_world->GetGameActorByAID(GID);

	if (sActor) {
		if (clevel == 99) {
			if (!IsEffectStateSpecialHiding(effectState))
				sActor->SendMsg(NULL, AM_99);
		}

		if (sActor->IsThisPC(sActor->GetJob())) {
			if (IsEffectStateBurrow(sActor->GetEffectState()) || IsEffectStateHiding(sActor->GetEffectState())) {
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_GUMGANG);
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_STEELBODY);
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_DOUBLEGUMGANG);
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_M04);
			} else {
				if ((showEFST & SHOW_EFST_QUICKEN) != 0) {
					if (!sActor->IsEffectExist(EF_MAKEBLUR)) {
						CRagEffect *effect = sActor->LaunchEffect(EF_MAKEBLUR);
						if (effect)
							effect->SendMsg(NULL, AM_SETCOUNT, 65535, EF_TWOHANDQUICKEN);
					}
				} else if ((showEFST & SHOW_EFST_OVERTHRUST) != 0) {
					if (!sActor->IsEffectExist(EF_MAKEBLUR)) {
						CRagEffect *effect = sActor->LaunchEffect(EF_MAKEBLUR);
						if (effect)
							effect->SendMsg(NULL, AM_SETCOUNT, 65535, EF_OVERTHRUST);
					}
				} else if ((showEFST & SHOW_EFST_ENERGYCOAT) != 0) {
					if (!sActor->IsEffectExist(EF_MAKEBLUR)) {
						CRagEffect *effect = sActor->LaunchEffect(EF_MAKEBLUR);
						if (effect)
							effect->SendMsg(NULL, AM_SETCOUNT, 65535, EF_ENERGYCOAT);
					}
				} else if ((showEFST & SHOW_EFST_BUNSIN) != 0) {
					if (!sActor->IsEffectExist(EF_BUNSINJYUTSU)) {
						sActor->LaunchEffect(EF_BUNSINJYUTSU);
					}
				}

				if ((showEFST & SHOW_EFST_EXPLOSIONSPIRITS) != 0) {
					if (!sActor->IsEffectExist(EF_GUMGANG))
						sActor->LaunchEffect(EF_GUMGANG);
				}

				if ((showEFST & SHOW_EFST_STEELBODY) != 0) {
					if (!sActor->IsEffectExist(EF_STEELBODY))
						sActor->LaunchEffect(EF_STEELBODY);
				}

				if ((showEFST & SHOW_EFST_SUN_WARM) != 0) {
					if (!sActor->IsEffectExist(EF_DOUBLEGUMGANG)) {
						sActor->LaunchEffect(EF_DOUBLEGUMGANG);
						sActor->LaunchEffect(EF_REDLIGHTBODY);
					}
				}

				if ((showEFST & SHOW_EFST_REFLECT) != 0) {
					if (!sActor->IsEffectExist(EF_REFLECTBODY))
						sActor->LaunchEffect(EF_REFLECTBODY);
				}

				if ((showEFST & SHOW_EFST_SOULLINK) != 0) {
					if (!sActor->IsEffectExist(EF_ASURABODY))
						sActor->LaunchEffect(EF_ASURABODY);
				}

				if ((showEFST & SHOW_EFST_CONTRACT) != 0) {
					if (!sActor->IsEffectExist(EF_M04))
						sActor->LaunchEffect(EF_M04);
				}

				if ((showEFST & SHOW_EFST_UNDEAD) != 0) {
					if (!sActor->IsEffectExist(EF_UNDEADBODY))
						sActor->LaunchEffect(EF_UNDEADBODY);
				}

				if ((showEFST & SHOW_EFST_AURABLADE) != 0) {
					if (!sActor->IsEffectExist(EF_AURABLADE2))
						sActor->LaunchEffect(EF_AURABLADE2);
				}

				if ((showEFST & SHOW_EFST_LIGHTBLADE) != 0) {
					if (!sActor->IsEffectExist(EF_LIGHTBLADE))
						sActor->LaunchEffect(EF_LIGHTBLADE);
				}

				if ((showEFST & SHOW_EFST_REDBODY) != 0) {
					if (!sActor->IsEffectExist(EF_REDBODY))
						sActor->LaunchEffect(EF_REDBODY);
				}

				if ((showEFST & SHOW_EFST_MOON) != 0) {
					if (!sActor->IsEffectExist(EF_SPHEREWIND2))
						sActor->LaunchEffect(EF_SPHEREWIND2);
				}

				if ((showEFST & SHOW_EFST_PINKBODY) != 0) {
					if (!sActor->IsEffectExist(EF_PINKBODY))
						sActor->LaunchEffect(EF_PINKBODY);
				}

				if ((showEFST & SHOW_EFST_BLADESTOP) != 0) {
					if (sActor->GetJob() == JT_MONK || sActor->GetJob() == JT_MONK_H || sActor->GetJob() == JT_MONK_B) {
						sActor->SetForceAnimation(96, 4, 4, 96, 4, TRUE, 99999999, FALSE);
					} else {
						sActor->m_isMotionFreezed = TRUE;
					}
					sActor->m_isSprArgbFixed = true;
					sActor->SetArgb(-1, 64, 64, 64);
				}

				if ((showEFST & SHOW_EFST_ASSUMPTIO) != 0) {
					if (!sActor->IsEffectExist(EF_ASSUMPTIO))
						sActor->LaunchEffect(EF_ASSUMPTIO);
				}
			}
		} else {
			if (IsEffectStateBurrow(sActor->GetEffectState()) || IsEffectStateHiding(sActor->GetEffectState())) {
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_GUMGANG);
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_STEELBODY);
				sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_DOUBLEGUMGANG);
			} else {
				if ((showEFST & SHOW_EFST_BLADESTOP) != 0) {
					sActor->m_isMotionFreezed = TRUE;
					sActor->m_isSprArgbFixed = true;
					sActor->SetArgb(-1, 64, 64, 64);
				}

				if ((showEFST & SHOW_EFST_OVERTHRUST) != 0) {
					if (!sActor->IsEffectExist(EF_BABYBODY))
						sActor->LaunchEffect(EF_BABYBODY);
				} else {
					while (sActor->IsEffectExist(EF_BABYBODY)) {
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_BABYBODY);
						sActor->LaunchEffect(EF_BABYBODY_BACK);
					}
				}

				if ((showEFST & SHOW_EFST_ENERGYCOAT) != 0) {
					if (!sActor->IsEffectExist(EF_ASURABODY_MONSTER))
						sActor->LaunchEffect(EF_ASURABODY_MONSTER);
				} else {
					while (sActor->IsEffectExist(EF_ASURABODY_MONSTER))
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_ASURABODY_MONSTER);
				}

				if ((showEFST & SHOW_EFST_STEELBODY) != 0) {
					if (!sActor->IsEffectExist(EF_STEELBODY))
						sActor->LaunchEffect(EF_STEELBODY);
				} else {
					while (sActor->IsEffectExist(EF_STEELBODY))
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_STEELBODY);
				}

				if ((showEFST & SHOW_EFST_UNDEAD) != 0) {
					if (!sActor->IsEffectExist(EF_UNDEADBODY))
						sActor->LaunchEffect(EF_UNDEADBODY);
				} else {
					while (sActor->IsEffectExist(EF_UNDEADBODY)) {
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_UNDEADBODY);
						sActor->LaunchEffect(EF_UNDEADBODY_DEL);
					}
				}

				if ((showEFST & SHOW_EFST_SOULLINK) != 0) {
					if (!sActor->IsEffectExist(EF_DA_SPACE))
						sActor->LaunchEffect(EF_DA_SPACE);
				} else {
					while (sActor->IsEffectExist(EF_DA_SPACE))
						sActor->SendMsg(NULL, AM_DELETE_EFFECT, EF_DA_SPACE);
				}
			}
		}
	}
}


void CGameMode::Zc_Notify_Skill2(char const *buf)
{
	PACKET_ZC_NOTIFY_SKILL2 *p = (PACKET_ZC_NOTIFY_SKILL2 *)buf;

	SKILLSTRUCT ss;
	ss.skillId = p->SKID;
	ss.action = p->action;
	ss.damage = p->damage;
	ss.startTime = p->startTime;
	ss.targetGID = p->targetID;
	ss.level = p->level;
	ss.count = p->count;
	ss.tCellX = -1;
	ss.tCellY = -1;
	ss.attackMT = p->attackMT;
	ss.attackedMT = p->attackedMT;

	CGameActor *sActor = NULL;
	CGameActor *theActor = NULL;
	CGameActor *tActor;
	int job = -1;

	if (p->targetID == g_session.GetAid())
		tActor = m_world->m_player;
	else
		tActor = m_world->GetGameActorByAID(p->targetID);

	if (p->AID == g_session.GetAid()) {
		m_world->m_player->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
		sActor = m_world->m_player;
		job = sActor->GetJob();
	} else {
		theActor = m_world->GetGameActorByAID(p->AID);

		if (theActor) {
			theActor->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
			sActor = theActor;
			job = theActor->GetJob();
		} else {
			CSkill *theSkill = m_world->GetSkillByAID(p->AID);
			if (theSkill)
				theSkill->SendMsg(NULL, AM_SKILL, int(&ss), 0, 0);
		}
	}

	if (tActor && sActor) {
		switch (p->SKID) {
		case SKID_AS_SPLASHER:
			if (p->level == -1)
				tActor->LaunchEffect(EF_SPLASHER);
			break;
		case SKID_NPC_HELLJUDGEMENT:
			if (sActor != tActor)
				tActor->LaunchEffect(EF_DEVIL);
			break;
		}
	}

	if (!g_session.IsMonster(sActor)) {
		SendMsg(MM_MAKE_SKILL_USED_MSG, p->SKID, int(sActor), p->level);
	} else {
		int act;
		int loopType;
		g_session.GetMonsterSkillInfo((char *)g_session.m_jobNameTable[job], ss.skillId, act, loopType);

		if (act != -1) {
			if (theActor) {
				if (loopType == 0)
					loopType = MT_ONETIME;
				else
					loopType = 0;
				theActor->SetAction((act - 1) * 8, 1, loopType);
			}
		}
	}
}

void	CGameMode :: Zc_Ack_AccountName(const char *buf)
{
	PACKET_ZC_ACK_ACCOUNTNAME *p = (PACKET_ZC_ACK_ACCOUNTNAME*)buf;
	REQ_ACCOUNT_AID accAID;
	accAID.accountName = (char*)p->name;
	accAID.AID = p->AID;
	g_session.m_recievedAccountName.push_back(accAID);
	g_session.DeleteAssignAID(p->AID);
}


void CGameMode::Zc_Friends_List(char const *buf)
{
#pragma pack(push, 1)
	struct STRUCT_FRIEND {
		DWORD AID;
		DWORD GID;
		char Name[NAME_LENGTH];
	};
#pragma pack(pop)
	g_session.ClearFriend();

	PACKET_ZC_FRIENDS_LIST *p = (PACKET_ZC_FRIENDS_LIST *)buf;
	int packetStrLen = sizeof(PACKET_ZC_FRIENDS_LIST);
	if (p->PacketLength == packetStrLen)
		return;

	int friendInfoLen = sizeof(STRUCT_FRIEND);
	int loopCnt = (p->PacketLength - packetStrLen) / friendInfoLen;
	for (int i = 0; i < loopCnt; ++i)
	{
		STRUCT_FRIEND info;
		memcpy(&info, buf + packetStrLen + friendInfoLen * i, sizeof(STRUCT_FRIEND));

		char characterName[NAME_LENGTH + 1];
		memcpy(characterName, info.Name, NAME_LENGTH);
		characterName[NAME_LENGTH] = 0;

		FRIEND_INFO fInfo;
		fInfo.characterName = characterName;
		fInfo.AID = info.AID;
		fInfo.GID = info.GID;
		fInfo.state = true;

		g_session.AddFriendToList(fInfo);
	}

	g_session.RefreshFriendUI();
}


void CGameMode::Zc_Add_Friends_List(char const *buf)
{

	PACKET_ZC_ADD_FRIENDS_LIST *p = (PACKET_ZC_ADD_FRIENDS_LIST *)buf;

	char characterName[NAME_LENGTH + 1];
	memcpy(characterName, p->Name, NAME_LENGTH);
	characterName[NAME_LENGTH] = 0;

	char msg[128];
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	switch (p->Result) {
	case 1:
		sprintf(msg, MsgStr(MSI_FRIEND_ADD_FALSE), characterName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xff, 0, 0);
		break;
	case 0:
		sprintf(msg, MsgStr(MSI_FRIEND_ADD_SUCCEED), characterName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 0, 0);
		{
			FRIEND_INFO fInfo;
			fInfo.characterName = characterName;
			fInfo.AID = p->AID;
			fInfo.GID = p->GID;
			fInfo.state = false;
			g_session.AddFriendToList(fInfo);
			g_session.RefreshFriendUI();
		}
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FRIEND_ADD_ME_OVERSIZE)), 0xff, 0, 0);
		break;
	case 3:
		sprintf(msg, MsgStr(MSI_FRIEND_ADD_OTHER_OVERSIZE), characterName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xff, 0, 0);
		break;
	}
}

extern bool IsNameYellow(DWORD aid);

void	CGameMode :: Zc_Req_Join_Friend(const char *buf)
{
	PACKET_ZC_REQ_ADD_FRIENDS *	p = (PACKET_ZC_REQ_ADD_FRIENDS *)buf;
	char		friendName[NAME_LENGTH+1];
	strncpy(friendName, p->Name, NAME_LENGTH);
	friendName[NAME_LENGTH] = 0;

	if (IsNameYellow(g_session.GetAid()) || g_session.m_isNoTrade)
	{
		g_modeMgr.GetCurMode()->SendMsg(MM_JOIN_FRIEND, p->ReqAID, p->ReqGID, 0);
	}
	else
	{
		UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_JOINFRIENDACCEPTWND);
		wnd->SendMsg(NULL, UM_SET_JOIN_FRIEND_MSG, p->ReqAID, p->ReqGID, (int)friendName, 0);
	}
}

void	CGameMode :: Zc_Req_Delete_Friend(const char *buf)
{
	PACKET_ZC_DELETE_FRIENDS *		p = (PACKET_ZC_DELETE_FRIENDS *)buf;

	if  ( g_session.DeleteFriendFromList ( p->GID ) )
		g_session.RefreshFriendUI ( ) ;
}

void	CGameMode :: Zc_Friends_State(const char *buf)
{
	PACKET_ZC_FRIENDS_STATE *		p = (PACKET_ZC_FRIENDS_STATE *)buf;

	if ( g_session.SetFriendState ( p->AID, p->GID, p->State ) )
		g_session.RefreshFriendUI ( ) ;
}

void	CGameMode :: Zc_Divorce (const char *buf)
{
	PACKET_ZC_DIVORCE *		p = (PACKET_ZC_DIVORCE *)buf;

	char result[NAME_LENGTH*2+20];
	sprintf(result, MsgStr(MSI_DIVORCE), g_session.GetCharName(), p->name);
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(result), 0xffff00, 0, 0);
}

void	CGameMode :: Zc_Ack_PvpPoint (const char *buf)
{
	PACKET_ZC_ACK_PVPPOINT * p = (PACKET_ZC_ACK_PVPPOINT *)buf;

	PVPINFO	info = p->PVP;

	char	msg[128];
	sprintf(msg, MsgStr(MSI_PVP_INFO), info.WinPoint, info.LosePoint, info.Point);

	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 0, 0);
}


void CGameMode::Zc_Notify_PKinfo(char const *buf)
{
	PACKET_ZC_NOTIFY_PKINFO *p = (PACKET_ZC_NOTIFY_PKINFO *)buf;

	g_session.m_winPoint = p->winPoint;
	g_session.m_losePoint = p->losePoint;
	g_session.killName = p->killName;
	g_session.killedName = p->killedName;

	if (p->expireTime.dwLowDateTime > 0)
		FileTimeToSystemTime(&p->expireTime, &g_session.expireTime);
	else
		g_session.expireTime.wMonth = 0;

	UIStatusWnd *window = g_windowMgr.GetStatusWnd();
	if (window)
		window->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
}

void	CGameMode :: Zc_Notify_CrazyKiller (const char *buf)
{
	PACKET_ZC_NOTIFY_CRAZYKILLER * p = (PACKET_ZC_NOTIFY_CRAZYKILLER *)buf;

	if ( p->isCrazyKiller )
	{
		if ( !IsKillerAid ( p->AID ) )
			m_KillerList.push_back ( p->AID ) ;
	}
	else
	{
		std::vector<DWORD>::iterator it = std::find(m_KillerList.begin(), m_KillerList.end(), p->AID);

		if (it != m_KillerList.end())
			m_KillerList.erase(it);
	}
}

extern void Auth2(unsigned long* p);
void	CGameMode :: Zc_Game_Guard(const char *buf)
{
	PACKET_ZC_GAME_GUARD *	p = (PACKET_ZC_GAME_GUARD *)buf;
	Auth2(p->AuthData);
}


void CGameMode::Zc_Change_Mer_Status(char const *buf)
{
	PACKET_ZC_CHANGESTATE_MER *p = (PACKET_ZC_CHANGESTATE_MER *)buf;
	CGameActor *actor = (CGameActor *)m_world->GetGameActorByAID(p->GID);

	switch (p->state) {
	case HO_PRE_INIT:
		g_session.m_homunGID = p->GID;
		if (actor) {
			g_session.m_homunAlive = TRUE;
			g_session.m_homunJob = actor->GetJob();
			return;
		}
		break;

	case HO_RELATIONSHIP_CHANGED:
		g_session.m_homunFriendly = p->data;
		if (actor) {
			g_session.m_homunJob = actor->GetJob();
		}
		if (g_windowMgr.GetHomunInfoWnd()) {
			g_windowMgr.GetHomunInfoWnd()->Invalidate();
		}
		if (g_session.IsMasterAid(3)) {
			char msg[256];
			sprintf(msg, "%d", p->data);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(245, 245, 245)), 0, 0);
			return;
		}
		break;

	case HO_FULLNESS_CHANGED:
		g_session.SetHomunHungry(p->data);
		if (actor) {
			g_session.m_homunJob = actor->GetJob();
		}
		if (g_windowMgr.GetHomunInfoWnd()) {
			g_windowMgr.GetHomunInfoWnd()->Invalidate();
		}
		if (g_session.IsMasterAid(3)) {
			char msg[256];
			sprintf(msg, "Full : %d", p->data);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(245, 245, 30)), 0, 0);
			return;
		}
		break;

	case HO_ACCESSARY_CHANGED:
		if (!actor) {
			break;
		}
		if (p->data) {
			g_session.m_homunAcc = p->data;
		} else {
			g_session.m_homunAcc = 0;
			actor->SetSprJob(actor->GetJob());
		}
		if (p->GID == g_session.m_homunGID) {
			if (g_windowMgr.GetHomunInfoWnd()) {
				g_windowMgr.GetHomunInfoWnd()->Invalidate();
			}
		}
		break;
	}
}


void CGameMode::Zc_Property_Homun(char const *buf)
{
	PACKET_ZC_PROPERTY_HOMUN *p = (PACKET_ZC_PROPERTY_HOMUN *)buf;

	if (ServiceGermany == g_serviceType || ServiceBrazil == g_serviceType || ServiceVietnam == g_serviceType)
		sprintf(g_session.m_homunName, "%s", p->szName);
	else
		sprintf(g_session.m_homunName, p->szName);

	g_session.m_homunFriendly = p->nRelationship;
	g_session.m_homunAcc = p->ITID;
	g_session.m_isHomunNameModified = p->bModified;

	g_session.m_homunAtk = p->atk;
	g_session.m_homunMatk = p->Matk;
	g_session.m_homunHit = p->hit;
	g_session.m_homunCritical = p->critical;
	g_session.m_homunDef = p->def;
	g_session.m_homunMdef = p->Mdef;
	g_session.m_homunFlee = p->flee;
	g_session.m_homunAspd = p->aspd;

	g_session.m_homunLevel = p->nLevel;

	g_session.SetHomunHP(p->hp);
	g_session.SetHomunSP(p->sp);
	g_session.SetHomunMaxHP(p->maxHP);
	g_session.SetHomunMaxSP(p->maxSP);
	g_session.SetHomunExp(p->exp);
	g_session.SetHomunNextExp(p->maxEXP);
	g_session.SetHomunHungry(p->nFullness);

	g_session.SetHomunSkillPoint(p->SKPoint);

	g_session.m_homunATKRange = p->ATKRange;

	UIWindow *skillWnd = (UIWindow *)g_windowMgr.GetHomunSkillListWnd();
	if (skillWnd)
		skillWnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);

	if (p->nFullness == 1 || p->nFullness == 5 || p->nFullness == 10)
	{
		if (g_session.m_oldHomunHungry != p->nFullness)
		{
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_HOMUN_HUNGRY)), int(RGB(255, 0, 0)), 0, 0);
			g_session.m_oldHomunHungry = p->nFullness;
		}
	}
}


void CGameMode::Zc_Mer_Init(char const *buf)
{
	PACKET_ZC_MER_INIT *p = (PACKET_ZC_MER_INIT *)buf;

	g_session.ClearMerSkillItem();

	CGameActor *actor = (CGameActor *)m_world->GetGameActorByAID(p->AID);

	g_session.m_merGID = p->AID;
	g_session.m_merAlive = TRUE;

	if (actor)
		g_session.m_merJob = actor->GetJob();

	sprintf(g_session.m_merName, p->name);

	g_session.m_merAtk = p->atk;
	g_session.m_merMatk = p->Matk;
	g_session.m_merHit = p->hit;
	g_session.m_merCritical = p->critical;
	g_session.m_merDef = p->def;
	g_session.m_merMdef = p->Mdef;
	g_session.m_merFlee = p->flee;
	g_session.m_merAspd = p->aspd;

	g_session.m_merLevel = p->level;

	g_session.SetMerHP(p->hp);
	g_session.SetMerSP(p->sp);
	g_session.SetMerMaxHP(p->maxHP);
	g_session.SetMerMaxSP(p->maxSP);

	g_session.m_merFaith = p->faith;
	g_session.m_merCallNum = p->toal_call_num;
	g_session.m_merKillCounter = p->approval_monster_kill_counter;
	g_session.m_merATKRange = p->ATKRange;

	g_session.m_merExpireDate = p->ExpireDate;
}


void CGameMode::Zc_Mer_Par_Change(char const *buf)
{
	PACKET_ZC_MER_PAR_CHANGE *p = (PACKET_ZC_MER_PAR_CHANGE *)buf;

	if (!g_session.m_merAlive)
		return;

	switch (p->var)
	{
	case VAR_HP:
		g_session.SetMerHP(p->value);
		break;
	case VAR_SP:
		g_session.SetMerSP(p->value);
		break;
	case VAR_MAXHP:
		g_session.SetMerMaxHP(p->value);
		break;
	case VAR_MAXSP:
		g_session.SetMerMaxSP(p->value);
		break;
	case VAR_ATTPOWER:
		g_session.m_merAtk = p->value;
		break;
	case VAR_HITSUCCESSVALUE:
		g_session.m_merHit = p->value;
		break;
	case VAR_BASICAVOIDANCE:
		g_session.m_merFlee = p->value;
		break;
	case VAR_ASPD:
		g_session.m_merAspd = p->value;
		break;
	case VAR_MER_KILLCOUNT:
		g_session.m_merKillCounter = p->value;
		break;
	case VAR_MER_FAITH:
		g_session.m_merFaith = p->value;
		break;
	}

	g_session.InvalidateMerInfoWnd();
}

void	CGameMode :: ZC_KILLER_POINT(const char *buf)
{
	PACKET_ZC_BLACKSMITH_POINT *	p = (PACKET_ZC_BLACKSMITH_POINT *)buf;

	char msg[256];
	sprintf(msg, MsgStr(MSI_KILLER_POINT),p->Point,p->TotalPoint);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, (int)RGB(205, 205, 205), 0, 0);
}

void	CGameMode :: ZC_KILLER_RANK(const char *buf)
{
	int i;
	PACKET_ZC_BLACKSMITH_RANK *	p = (PACKET_ZC_BLACKSMITH_RANK *)buf;

	char msg[256];
	sprintf(msg,"=========== KILLER RANK ==============");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9b9b9b, 0, 0);
	for (i=0;i<10;i++)
	{
		if (i==9)
			sprintf(msg,"[ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		else
			sprintf(msg," [ %d ] %16s  :  %4d POINT",i+1,p->Name[i],p->Point[i]);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xcdcdcd, 0, 0);
	}

	sprintf(msg,"======================================");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9b9b9b, 0, 0);
}


void CGameMode::Zc_Mail_Req_Get_List(char const *buf)
{
	PACKET_ZC_MAIL_REQ_GET_LIST *p = (PACKET_ZC_MAIL_REQ_GET_LIST *)buf;
	g_session.m_mailList.clear();
	if (p->MailNumber != 0)
	{
		UIMailListWnd *mailWnd = g_windowMgr.GetMailListWnd();
		if (mailWnd == NULL)
			return;

		if (p->PacketLength == sizeof(PACKET_ZC_MAIL_REQ_GET_LIST))
			return;

		for (int i = 0; i < p->MailNumber; i++)
		{
			MAIL_LIST maillist;
			memcpy(&maillist, buf + sizeof(PACKET_ZC_MAIL_REQ_GET_LIST) + sizeof(MAIL_LIST) * i, sizeof(MAIL_LIST));
			g_session.m_mailList.push_back(maillist);
		}
	}

	if (g_windowMgr.GetMailListWnd())
	{
		if (g_windowMgr.GetMailListWnd()->GetType2() == 0)
		{
			g_windowMgr.GetMailListWnd()->SetType(0);
			g_windowMgr.GetMailListWnd()->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		}
	}
}


void CGameMode::Zc_Mail_Req_Open(char const *buf)
{
	PACKET_ZC_MAIL_REQ_OPEN *p = (PACKET_ZC_MAIL_REQ_OPEN *)buf;

	MAIL_OPEN mailOpen;
	mailOpen.MailID = p->MailID;
	strcpy(mailOpen.HEADER, p->Header);
	if (strlen(mailOpen.HEADER) == 0)
		strcpy(mailOpen.HEADER, "     ");
	strcpy(mailOpen.FromName, p->FromName);
	mailOpen.DeleteTime = p->DeleteTime;
	mailOpen.Money = p->Money;
	mailOpen.count = p->count;
	mailOpen.ITID = p->ITID;
	mailOpen.Type = p->Type;
	mailOpen.IsIdentified = p->IsIdentified;
	mailOpen.IsDamaged = p->IsDamaged;
	mailOpen.refiningLevel = p->refiningLevel;
	mailOpen.slot = p->slot;

	g_session.m_mailOpen = mailOpen;

	int len = p->PacketLength - sizeof(PACKET_ZC_MAIL_REQ_OPEN);
	char content[600];
	memcpy(&content, buf + sizeof(PACKET_ZC_MAIL_REQ_OPEN), len);
	content[len] = 0;
	g_windowMgr.MakeWindow(WID_MAILVIEWWND);
	g_windowMgr.GetMailViewWnd()->SendMsg(NULL, UM_REFRESH, strlen(content), int(content), 0, 0);

	std::vector<MAIL_LIST>::iterator it = g_session.m_mailList.begin();
	while (it != g_session.m_mailList.end())
	{
		if ((*it).MailID == p->MailID)
		{
			(*it).isOpen = true;
			if (g_windowMgr.GetMailListWnd())
			{
				if (g_windowMgr.GetMailListWnd()->GetType2() == 0)
				{
					g_windowMgr.GetMailListWnd()->SetType(0);
					g_windowMgr.GetMailListWnd()->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
				}
			}
			return;
		}
		++it;
	}
}

void	CGameMode :: Zc_Mail_Req_Get_Item(const char *buf)
{
	char result = buf[2];

	if (result == 0)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_ADD_ITEM_SUCCESS)), 0xffffff, 0, 0);
		if (g_windowMgr.GetMailViewWnd())
			g_windowMgr.GetMailViewWnd()->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
	}
	else if (result == 1)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_ADD_ITEM_FAILURE)), 0xff, 0, 0);
	}
	else if (result == 2)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_ADD_ITEM_OVER_FAILURE)), 0xff, 0, 0);
		if (g_windowMgr.GetMailViewWnd())
			g_windowMgr.GetMailViewWnd()->SendMsg(NULL, UM_COMMAND, 0x77, 0, 0, 0);
	}
}

void	CGameMode :: Zc_Mail_Req_Send(const char *buf)
{
	g_session.m_waitForSendMail = 0;
	g_session.m_waitForAddAuction = 0;

	char result = buf[2];
	if (result == 0)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_SEND_SUCCESS)), 0xffffff, 0, 0);
		if (g_windowMgr.GetMailListWnd())
			g_windowMgr.GetMailListWnd()->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		g_windowMgr.DeleteWindow(WID_MAILVIEWWND);
	}
	else if (result == 1)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_USER_NOT_FOUND)), 0xff, 0, 0);
		if (g_windowMgr.GetMailListWnd())
			g_windowMgr.GetMailListWnd()->ResetMailWrite(0);
	}
}


void	CGameMode :: Zc_Mail_Receive(const char *buf)
{
	PACKET_ZC_MAIL_RECEIVE *	p = (PACKET_ZC_MAIL_RECEIVE *)buf;
	if (p->MailID)
	{
		g_session.m_mailNew.MailID = p->MailID;
		strcpy(g_session.m_mailNew.FromName, p->FromName);
		strcpy(g_session.m_mailNew.HEADER, p->Header);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NEW_MAIL)), 0x4fc6fe, 0, 0);
		CGameMode *lpMode = g_modeMgr.GetGameMode();
		lpMode->SendMsg(MM_GET_MAIL_LIST, 0, 0, 0);
	}
}


void CGameMode::Zc_Ack_Mail_Add_Item(char const *buf)
{
	PACKET_ZC_ACK_MAIL_ADD_ITEM *p = (PACKET_ZC_ACK_MAIL_ADD_ITEM *)buf;

	enum {
		ADD_SUCCEED,
		ADD_FAILED,
	};

	switch (p->result)
	{
	case ADD_SUCCEED:
		if (p->Index)
		{
			ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(p->Index);
			itemInfo.m_num = g_session.m_mailItemCnt;
			g_session.DecItem(itemInfo.m_itemIndex, itemInfo.m_num);
		}
		else
		{
			g_session.SetGold(g_session.GetGold() - g_session.m_mailItemCnt);
		}
		break;

	case ADD_FAILED:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_ITEM_ADD_FAILED)), 0xff, 0, 0);
		if (g_windowMgr.GetMailListWnd())
			g_windowMgr.GetMailListWnd()->SendMsg(NULL, UM_DECITEM, 0, 0, 0, 0);
		break;
	}
}


void CGameMode::Zc_Ack_Mail_Delete(char const *buf)
{
	PACKET_ZC_ACK_MAIL_DELETE *p = (PACKET_ZC_ACK_MAIL_DELETE *)buf;

	enum {
		RESULT_TRUE,
		RESULT_FALSE,
	};

	if (p->Result == RESULT_TRUE)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_DELETE_SUCCESS)), 0xffffff, 0, 0);
		g_windowMgr.DeleteWindow(WID_MAILVIEWWND);

		std::vector<MAIL_LIST>::iterator it = g_session.m_mailList.begin();
		while (it != g_session.m_mailList.end())
		{
			if (it->MailID == p->MailID)
			{
				g_session.m_mailList.erase(it);
				if (g_windowMgr.GetMailListWnd())
				{
					if (g_windowMgr.GetMailListWnd()->GetType2() == 0)
					{
						g_windowMgr.GetMailListWnd()->SetType(0);
						g_windowMgr.GetMailListWnd()->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
					}
				}
				return;
			}

			it++;
		}
	}
	else
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_DELETE_FAILURE)), 0xff, 0, 0);
	}
}

void	CGameMode :: Zc_Mail_Windows(const char *buf)
{
	PACKET_ZC_MAIL_WINDOWS *p = (PACKET_ZC_MAIL_WINDOWS *)buf;

	enum {
		TYPE_OPEN,
		TYPE_CLOSE,
	};

	if (p->Type == TYPE_OPEN)
	{
		CGameMode *lpMode = g_modeMgr.GetGameMode();
		lpMode->SendMsg(MM_GET_MAIL_LIST, 0, 0, 0);
		g_windowMgr.MakeWindow(WID_MAILLISTWND);
	}
	else if (p->Type == TYPE_CLOSE)
	{
		g_windowMgr.DeleteWindow(WID_MAILLISTWND);
	}
}

void	CGameMode :: Zc_Ack_Mail_Return(const char *buf)
{
	PACKET_ZC_ACK_MAIL_RETURN *	p = (PACKET_ZC_ACK_MAIL_RETURN *)buf;

	if (p->Result == 0)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_RETURN_SUCCESS)), 0xffffff, 0, 0);
		g_windowMgr.DeleteWindow(WID_MAILVIEWWND);
		CGameMode *lpMode = g_modeMgr.GetGameMode();
		lpMode->SendMsg(MM_GET_MAIL_LIST, 0, 0, 0);
	}
	else
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MAIL_RETURN_FAILURE)), 0xff, 0, 0);
	}
}


void CGameMode::Zc_Ack_Auction_Add_Item(char const *buf)
{
	PACKET_ZC_ACK_AUCTION_ADD_ITEM *p = (PACKET_ZC_ACK_AUCTION_ADD_ITEM *)buf;

	switch (p->result) {
	case 0:
		if (p->Index) {
			ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(p->Index);
			g_session.DecItem(itemInfo.m_itemIndex, 1);
		}
		break;
	case 1:
		UIAuctionWnd *auctionWnd = g_windowMgr.GetAuctionWnd();
		if (auctionWnd) {
			auctionWnd->InitItem();
			auctionWnd->SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
		}
		break;
	}
}


void CGameMode::Zc_Auction_Result(char const *buf)
{
	PACKET_ZC_AUCTION_RESULT *p = (PACKET_ZC_AUCTION_RESULT *)buf;

	enum {
		RESULT_ADD_FALSE,
		RESULT_ADD_TRUE,
		RESULT_ADD_CANCEL,
		RESULT_ADD_CANCEL_FALSE,
		RESULT_ADD_OVER,
		RESULT_ADD_MONEY_FALSE,
		RESULT_BUY_TRUE,
		RESULT_BUY_FALSE,
		RESULT_BUY_FALSE_MONEY,
		RESULT_BUY_OVER,
	};

	g_session.m_waitForAddAuction = false;
	if (p->Result == RESULT_ADD_FALSE)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AUCTION_ADD_FALSE)), int(RGB(255, 0, 0)), 0, 0);
	else if (p->Result == RESULT_ADD_TRUE)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AUCTION_ADD_TRUE)), int(RGB(255, 255, 255)), 0, 0);
	else if (p->Result == RESULT_ADD_CANCEL)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AUCTION_ADD_CANCEL)), int(RGB(255, 0, 0)), 0, 0);
	else if (p->Result == RESULT_ADD_CANCEL_FALSE)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AUCTION_ADD_CANCEL_FALSE)), int(RGB(255, 0, 0)), 0, 0);
	else if (p->Result == RESULT_ADD_OVER)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AUCTION_ADD_OVER)), int(RGB(255, 0, 0)), 0, 0);
	else if (p->Result == RESULT_ADD_MONEY_FALSE)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ADD_MONEY_FALSE)), int(RGB(255, 0, 0)), 0, 0);
	else if (p->Result == RESULT_BUY_TRUE)
	{
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_RESULT_BUY_TRUE)), int(RGB(255, 255, 255)), 0, 0);
		CGameMode *lpMode = g_modeMgr.GetGameMode();
		lpMode->SendMsg(MM_SEARCH_AUCTION_ITEM, g_session.m_auctionSeachOption, g_session.m_auctionSeachOption2, 1);
	}
	else if (p->Result == RESULT_BUY_FALSE)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_RESULT_BUY_FALSE)), int(RGB(255, 0, 0)), 0, 0);
	else if (p->Result == RESULT_BUY_FALSE_MONEY)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_RESULT_BUY_FALSE_MONEY)), int(RGB(255, 0, 0)), 0, 0);
	else if (p->Result == RESULT_BUY_OVER)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_RESULT_BUY_OVER)), int(RGB(255, 0, 0)), 0, 0);

	UIAuctionWnd *auctionWnd = g_windowMgr.GetAuctionWnd();
	if (auctionWnd)
		auctionWnd->SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
}


void CGameMode::Zc_Auction_Item_Req_Search(char const *buf)
{

	PACKET_ZC_AUCTION_ITEM_REQ_SEARCH *p = (PACKET_ZC_AUCTION_ITEM_REQ_SEARCH *)buf;

	g_session.m_auctionList.clear();

	if (p->Number != 0) {
		int packetStrLen = sizeof(PACKET_ZC_AUCTION_ITEM_REQ_SEARCH);

		if (p->PacketLength == packetStrLen)
			return;

		int auctionListLen = sizeof(AUCTION_ITEM_SEARCH_INFO);

		for (int i = 0; i < p->Number; i++) {
			AUCTION_ITEM_SEARCH_INFO auctionList;
			memcpy(&auctionList, buf + packetStrLen + auctionListLen * i, sizeof(AUCTION_ITEM_SEARCH_INFO));
			g_session.m_auctionList.push_back(auctionList);
		}
	}

	if (g_windowMgr.GetAuctionWnd())
		g_windowMgr.GetAuctionWnd()->SendMsg(0, UM_REFRESH, 1, p->MaxPage, 0, 0);
}

void	CGameMode :: Zc_Auction_My_Sell_Stop(const char *buf)
{
	PACKET_ZC_AUCTION_ACK_MY_SELL_STOP *p = (PACKET_ZC_AUCTION_ACK_MY_SELL_STOP *)buf;

	if (p->Result == 0)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_RESULT_MY_SELL_STOP_TRUE)), 0xffffff, 0, 0);
	else if (p->Result == 1)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_RESULT_MY_SELL_STOP_FALSE)), 0xff, 0, 0);
	else if (p->Result == 2)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_RESULT_AUCTION_ID_FALSE)), 0xff, 0, 0);
}

void	CGameMode :: Zc_Auction_Windows(const char *buf)
{
	PACKET_ZC_AUCTION_WINDOWS *p = (PACKET_ZC_AUCTION_WINDOWS *)buf;

	enum {
		TYPE_OPEN,
		TYPE_CLOSE,
	};

	if (p->Type == TYPE_OPEN)
	{
		CGameMode *lpMode = g_modeMgr.GetGameMode();
		lpMode->SendMsg(MM_SEARCH_AUCTION_ITEM, 0, 0, 1);
		g_windowMgr.MakeWindow(WID_AUCTIONWND);
	}
	else if (p->Type == TYPE_CLOSE)
	{
		g_windowMgr.DeleteWindow(WID_AUCTIONWND);
	}
}


void CGameMode::Zc_Pcbang_Effect(char const *buf)
{
	char msg[200];

	m_broadCastTick = timeGetTime();
	if (m_broadcastBalloon == NULL) {
		m_broadcastBalloon = new UITransBalloonText;
		m_broadcastBalloon->Create(8, 8);
		g_windowMgr.AddWindowFront(m_broadcastBalloon);
	}

	PACKET_ZC_PCBANG_EFFECT *p = (PACKET_ZC_PCBANG_EFFECT *)buf;
	sprintf(msg, MsgStr(MSI_PCBANG_EVENT), p->ExpFactor, p->ExpFactor2, p->DropFactor);
	if (g_serviceType == ServiceVietnam) {
		if (p->ExpFactor == -50) {
			sprintf(msg, MsgStr(MSI_BILLING_1));
		} else if (p->ExpFactor == -100) {
			sprintf(msg, MsgStr(MSI_BILLING_2));
		}
	} else if (g_serviceType == ServiceBrazil) {
		sprintf(msg, MsgStr(MSI_BILLING_2));
	}

	if (strlen(msg)) {
		m_broadcastBalloon->SetText(msg, 72);
		m_broadcastBalloon->Move((g_renderer->m_width - 640) / 2 - m_broadcastBalloon->GetWidth() / 2 + 320, 51);
		m_broadcastBalloon->SetFntColor(RGB(255, 176, 98), RGB(0, 0, 0));
	}
}

void	CGameMode :: Zc_HuntingList(const char *buf)
{
	char *temp = (char *)buf + 4;
	int msgLen = *(short *)(buf + 2) - 4;
	if (msgLen > 5000)
		msgLen = 5000;

	char *body = new char[msgLen + 1];
	memcpy(body, temp, msgLen);
	body[msgLen] = 0;

	char msg[256];
	sprintf(msg, "=========== HUNTING LIST =============");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9bcd9b, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(body), 0xcdffcd, 0, 0);
	sprintf(msg, "========================================");
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x9bcd9b, 0, 0);
}

void	CGameMode :: Zc_Aid(const char * buf)
{
}


void CGameMode::Zc_Pc_Cash_Point_ItemList(char const *buf)
{
	g_session.ClearDealItems();

	PACKET_ZC_PC_CASH_POINT_ITEMLIST *p = (PACKET_ZC_PC_CASH_POINT_ITEMLIST *)buf;
	int packetStrLen = sizeof(PACKET_ZC_PC_CASH_POINT_ITEMLIST);
	int numStr = (p->PacketLength - packetStrLen) / sizeof(PURCHASE_ITEM);

	for (int i = 0; i < numStr; i++)
	{
		PURCHASE_ITEM p_item;
		memcpy(&p_item, buf + packetStrLen + i * sizeof(PURCHASE_ITEM), sizeof(PURCHASE_ITEM));

		SHOP_ITEM shop_item;
		memset(&shop_item, 0, sizeof(SHOP_ITEM));
		itoa(p_item.ITID, shop_item.itemName, 10);

		shop_item.index = -1;
		shop_item.price = p_item.price;
		shop_item.realPrice = p_item.discountprice;
		shop_item.type = p_item.type;
		shop_item.isIdentified = 1;
		g_session.AddShopItem(shop_item, 0);
	}

	g_session.m_cashPoint = p->CashPoint;
	g_session.m_freePoint = p->FreePoint;
	m_isOnQuest = true;

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_ITEMSHOPWND);
	wnd->SendMsg(NULL, UM_SETTITLE, 0, 1, 0, 0);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);

	wnd = g_windowMgr.MakeWindow(WID_ITEMPURCHASEWND);
	wnd->SendMsg(NULL, UM_SETTITLE, 1, 0, 0, 0);
	wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}


void CGameMode::Zc_Pc_Cash_Point_Update(char const *buf)
{
	PACKET_ZC_PC_CASH_POINT_UPDATE *p = (PACKET_ZC_PC_CASH_POINT_UPDATE *)buf;

	switch (p->Error)
	{
	case 0:
		g_session.m_cashPoint = p->CashPoint;
		g_session.m_freePoint = p->FreePoint;
		if (g_windowMgr.GetItemShopWnd())
			g_windowMgr.GetItemShopWnd()->Invalidate();
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_DEAL_SECCESS)), int(RGB(0, 255, 255)), 0, 0);
		break;
	case 1:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BUY_CASH_FAIL_NPC)), int(RGB(255, 0, 0)), 0, 0);
		break;
	case 2:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BUY_CASH_FAIL_SYSTEM)), int(RGB(255, 0, 0)), 0, 0);
		break;
	case 3:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OVER_WEIGHT)), int(RGB(255, 0, 0)), 0, 0);
		break;
	case 4:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BUY_CASH_FAIL_EXCHANGE)), int(RGB(255, 0, 0)), 0, 0);
		break;
	case 5:
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BUY_CASH_FAIL_ITEM_ID)), int(RGB(255, 0, 0)), 0, 0);
		break;
	case 6:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BUY_CASH_FAIL_MONEY)), int(RGB(255, 0, 0)), 0, 0);
		break;
	}
}


void CGameMode::Zc_Boss_Info(char const *buf)
{
	PACKET_ZC_BOSS_INFO *p = (PACKET_ZC_BOSS_INFO *)buf;



	enum BOSS_INFO_TYPE
	{
		BOSS_INFO_NOT = 0,
		BOSS_INFO_ALIVE,
		BOSS_INFO_ALIVE_WITHMSG,
		BOSS_INFO_DEAD,
	};
	char bossMonName[51];

	sprintf(bossMonName, "%s", p->name);
	m_posOfBossMon.x = -1;
	m_posOfBossMon.y = -1;

	switch (p->infoType) {
	case BOSS_INFO_NOT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTFIND_BOSSMON)), 0xffffff, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSI_NOTFIND_BOSSMON)), 0xffffff, 0, 0);
		return;
	case BOSS_INFO_ALIVE:
	case BOSS_INFO_ALIVE_WITHMSG:
		m_posOfBossMon.x = p->xPos;
		m_posOfBossMon.y = p->yPos;
		if (p->infoType == BOSS_INFO_ALIVE_WITHMSG) {
			char msg[256];
			sprintf(msg, MsgStr(MSI_SHOW_POSITION_OF_BOSSMON), bossMonName);
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffffff, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), 0xffffff, 0, 0);
			return;
		}
		break;
	case BOSS_INFO_DEAD: {
		char msg[256];
		if (p->minHour == 0 && p->minMinute == 1) {
			sprintf(msg, MsgStr(MSI_APPEARANCE_TIME_OF_BOSSMON2), bossMonName);
		} else {
			sprintf(msg, MsgStr(MSI_APPEARANCE_TIME_OF_BOSSMON), bossMonName, p->minHour, p->minMinute);
		}
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffffff, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), 0xffffff, 0, 0);
		break;
	}
	}
}

void	CGameMode :: Zc_Msg(const char *buf)
{
	PACKET_ZC_MSG *	p = (PACKET_ZC_MSG *)buf;

	COLORREF msgColor = RGB(255,255,255);
	switch (p->msg)
	{
	case MSI_CANNOT_PARTYCALL:
	case MSI_NO_PARTYMEM_ON_THISMAP:
		msgColor = RGB(255,0,0);
		break;
	}

	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSGSTRINGID(p->msg))), int(msgColor), 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(MsgStr(MSGSTRINGID(p->msg))), int(msgColor), 0, 0);
}

void	CGameMode :: Zc_Read_Book(const char *buf)
{
}

void CGameMode::Zc_Equipment_Itemlist2(char const *buf)
{
	g_session.ClearEquipItemInInventory();
	g_session.ClearEquipItem();

	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO2);
	EQUIPMENTITEM_EXTRAINFO2 *dumpStr = (EQUIPMENTITEM_EXTRAINFO2 *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;

		if (g_session.IsPetEgg(atoi(itemInfo.m_itemName.c_str())))
			if (itemInfo.m_isDamaged)
				g_session.m_petEggIndex = itemInfo.m_itemIndex;

		if (itemInfo.m_wearLocation) {
			g_session.AddEquipItem(&itemInfo);
		} else {
			g_session.AddItem(&itemInfo);
			g_session.SetItem(&itemInfo);
		}
	}
}

void CGameMode::Zc_Store_Equipment_Itemlist2(char const *buf)
{
	g_windowMgr.MakeWindow(WID_ITEMSTOREWND);

	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO2);
	EQUIPMENTITEM_EXTRAINFO2 *dumpStr = (EQUIPMENTITEM_EXTRAINFO2 *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		g_session.AddStoreItem(itemInfo);
	}

	g_session.RefreshStoreItemWnd();
}

void CGameMode::Zc_Cart_Equipment_Itemlist2(char const *buf)
{
	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO2);
	EQUIPMENTITEM_EXTRAINFO2 *dumpStr = (EQUIPMENTITEM_EXTRAINFO2 *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		g_session.AddMerchantItem(itemInfo);
	}

	g_session.RefreshMerchantItemWnd();
}

void	CGameMode :: Zc_Cash_Time_Counter(const char *buf)
{
	PACKET_ZC_CASH_TIME_COUNTER *		p = (PACKET_ZC_CASH_TIME_COUNTER *)buf;

	char itemID[10] ;
	_itoa ( p->ITID, itemID, 10 ) ;

	const char * itemName = ITEM_INFO::GetItemDesc(atoi(itemID)).identifiedDisplayName;

	int remainTime = ( p->RemainSecond / 60 ) ;

	char msg[MAX_PATH] ;

	if ( remainTime > 0 )
		sprintf ( msg, MsgStr(MSI_NOTICE_TO_REMAIN_ITEMUSINGTIME), itemName, remainTime ) ;
	else
		sprintf ( msg, MsgStr(MSI_NOTICE_TO_REMAIN_ITEMUSINGTIME2), itemName, p->RemainSecond ) ;

	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), 0xffff, 0, 0);

	if ( remainTime == 1 )
	{
		sprintf ( msg, MsgStr(MSI_LAST_NOTICE_TO_REMAIN_ITEMUSINGTIME), itemName ) ;
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), 0xffff, 0, 0);
	}
}

void	CGameMode :: Zc_Cash_Item_Delete(const char *buf)
{
	PACKET_ZC_CASH_ITEM_DELETE *		p = (PACKET_ZC_CASH_ITEM_DELETE *)buf;
	char itemID[10];
	char msg[MAX_PATH];

	g_session.DecItem(p->index, 1);
	_itoa(p->ITID, itemID, 10);
	const char *itemName = ITEM_INFO::GetItemDesc(atoi(itemID)).identifiedDisplayName;
	sprintf(msg, MsgStr(MSI_NOTICE_TO_DELETE_ITEM), itemName);
	g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff, 0, 0);
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), 0xffff, 0, 0);
}


void CGameMode::Zc_Item_Pickup_Ack2(char const *buf)
{
	PACKET_ZC_ITEM_PICKUP_ACK2 *p = (PACKET_ZC_ITEM_PICKUP_ACK2 *)buf;
	m_pickupReqItemNaidList.erase(m_pickupReqItemNaidList.begin(), m_pickupReqItemNaidList.end());

	switch (p->result)
	{
	case PICKUP_SUCCEED:
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = p->type;
		itemInfo.m_location = p->location;
		itemInfo.m_itemIndex = p->Index;
		itemInfo.m_num = p->count;
		itemInfo.m_wearLocation = 0;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_refiningLevel = p->refiningLevel;
		itemInfo.m_deleteTime = p->HireExpireDate;

		if (atoi(itemInfo.m_itemName.c_str()) == 607)
			m_isMakeYgdrasil = FALSE;

		if (p->type == TYPE_ARROW || p->type == TYPE_AMMO || p->type == TYPE_THROWWEAPON)
			itemInfo.m_wearLocation = LOCATION_ARROW;

		g_session.AddItem(&itemInfo);

		char itemName[64];
		itemInfo.GetItemName(itemName);
		char pickMsg[128];
		sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 5, 0);

		int pickupCount = p->count;
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NOTIFYITEMOBTAINWND);
		wnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, itemInfo.m_isIdentified, 0, 0, 0);
		wnd->SendMsg(NULL, UM_SETTITLE, 0, pickupCount, (int)itemInfo.m_itemName.c_str(), 0);
		wnd->SendMsg(NULL, UM_SETINFO, (int)&itemInfo, 0, 0, 0);
		break;
	}
	case RECEIVE_ITEM:
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = p->type;
		itemInfo.m_location = p->location;
		itemInfo.m_itemIndex = p->Index;
		itemInfo.m_wearLocation = 0;
		itemInfo.m_num = p->count;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_refiningLevel = p->refiningLevel;
		itemInfo.m_deleteTime = p->HireExpireDate;

		if (atoi(itemInfo.m_itemName.c_str()) == 607)
			m_isMakeYgdrasil = FALSE;

		if (p->type == TYPE_ARROW || p->type == TYPE_AMMO || p->type == TYPE_THROWWEAPON)
			itemInfo.m_wearLocation = LOCATION_ARROW;

		switch (p->type)
		{
		case TYPE_ARMOR:
		case TYPE_ARMORTM:
		case TYPE_ARMORTB:
		case TYPE_ARMORMB:
		case TYPE_ARMORTMB:
		case TYPE_WEAPON:
		case TYPE_BOW:
		case TYPE_BOTHHAND:
		case TYPE_GUN:
			break;
		default:
			itemInfo.m_location = 0;
			break;
		}

		ITEM_INFO curItemInfo = g_session.GetItemInfoByIndex(p->Index);
		char itemName[64];
		char pickMsg[128];
		int gainCount = p->count;
		if (curItemInfo.m_itemIndex)
		{
			itemInfo.GetItemName(itemName);
			sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count - curItemInfo.m_num);
			gainCount = p->count - curItemInfo.m_num;
		}
		else
		{
			itemInfo.GetItemName(itemName);
			sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count);
			gainCount = p->count;
		}

		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 5, 0);

		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NOTIFYITEMOBTAINWND);
		wnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, itemInfo.m_isIdentified, 0, 0, 0);
		wnd->SendMsg(NULL, UM_SETTITLE, 0, gainCount, (int)itemInfo.m_itemName.c_str(), 0);
		wnd->SendMsg(NULL, UM_SETINFO, (int)&itemInfo, 0, 0, 0);

		if (m_isWaitingCancelExchangeItem)
		{
			m_isWaitingCancelExchangeItem = FALSE;
			g_session.CancelExchangeNoRefresh();
		}

		g_session.SetItem(&itemInfo);
		break;
	}
	case PICKUP_OVERCOUNT_ONEITEM:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_OVERCOUNT_ONEITEM)), 0xff, 0, 0);
		break;
	case PICKUP_OVERCOUNT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_BECAUSE_COUNT)), 0xff, 0, 0);
		break;
	case PICKUP_OVERWEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_BECAUSE_WEIGHT)), 0xff, 0, 0);
		break;
	case PICKUP_FAILED:
	case PICKUP_REFUSED_TIME:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM)), 0xff, 0, 0);
		break;
	}
}

void CGameMode::Zc_Item_Pickup_Ack3(char const *buf)
{
	PACKET_ZC_ITEM_PICKUP_ACK3 *p = (PACKET_ZC_ITEM_PICKUP_ACK3 *)buf;
	m_pickupReqItemNaidList.erase(m_pickupReqItemNaidList.begin(), m_pickupReqItemNaidList.end());

	switch (p->result)
	{
	case PICKUP_SUCCEED:
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = p->type;
		itemInfo.m_location = p->location;
		itemInfo.m_itemIndex = p->Index;
		itemInfo.m_num = p->count;
		itemInfo.m_wearLocation = 0;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_refiningLevel = p->refiningLevel;
		itemInfo.m_deleteTime = p->HireExpireDate;
		itemInfo.m_isYours = p->bindOnEquipType;

		if (atoi(itemInfo.m_itemName.c_str()) == 607)
			m_isMakeYgdrasil = FALSE;

		if (p->type == TYPE_ARROW || p->type == TYPE_AMMO || p->type == TYPE_THROWWEAPON)
			itemInfo.m_wearLocation = LOCATION_ARROW;

		g_session.AddItem(&itemInfo);

		char itemName[64];
		itemInfo.GetItemName(itemName);
		char pickMsg[128];
		sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 5, 0);

		int pickupCount = p->count;
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NOTIFYITEMOBTAINWND);
		wnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, itemInfo.m_isIdentified, 0, 0, 0);
		wnd->SendMsg(NULL, UM_SETTITLE, 0, pickupCount, (int)itemInfo.m_itemName.c_str(), 0);
		wnd->SendMsg(NULL, UM_SETINFO, (int)&itemInfo, 0, 0, 0);
		break;
	}
	case RECEIVE_ITEM:
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = p->type;
		itemInfo.m_location = p->location;
		itemInfo.m_itemIndex = p->Index;
		itemInfo.m_wearLocation = 0;
		itemInfo.m_num = p->count;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_refiningLevel = p->refiningLevel;
		itemInfo.m_deleteTime = p->HireExpireDate;
		itemInfo.m_isYours = p->bindOnEquipType;

		if (atoi(itemInfo.m_itemName.c_str()) == 607)
			m_isMakeYgdrasil = FALSE;

		if (p->type == TYPE_ARROW || p->type == TYPE_AMMO || p->type == TYPE_THROWWEAPON)
			itemInfo.m_wearLocation = LOCATION_ARROW;

		switch (p->type)
		{
		case TYPE_ARMOR:
		case TYPE_ARMORTM:
		case TYPE_ARMORTB:
		case TYPE_ARMORMB:
		case TYPE_ARMORTMB:
		case TYPE_WEAPON:
		case TYPE_BOW:
		case TYPE_BOTHHAND:
		case TYPE_GUN:
			break;
		default:
			itemInfo.m_location = 0;
			break;
		}

		ITEM_INFO curItemInfo = g_session.GetItemInfoByIndex(p->Index);
		char itemName[64];
		char pickMsg[128];
		int gainCount = p->count;
		if (curItemInfo.m_itemIndex)
		{
			itemInfo.GetItemName(itemName);
			sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count - curItemInfo.m_num);
			gainCount = p->count - curItemInfo.m_num;
		}
		else
		{
			itemInfo.GetItemName(itemName);
			sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, p->count);
			gainCount = p->count;
		}

		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 5, 0);

		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_NOTIFYITEMOBTAINWND);
		wnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, itemInfo.m_isIdentified, 0, 0, 0);
		wnd->SendMsg(NULL, UM_SETTITLE, 0, gainCount, (int)itemInfo.m_itemName.c_str(), 0);
		wnd->SendMsg(NULL, UM_SETINFO, (int)&itemInfo, 0, 0, 0);

		if (m_isWaitingCancelExchangeItem)
		{
			m_isWaitingCancelExchangeItem = FALSE;
			g_session.CancelExchangeNoRefresh();
		}

		g_session.SetItem(&itemInfo);
		break;
	}
	case PICKUP_OVERCOUNT_ONEITEM:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_OVERCOUNT_ONEITEM)), 0xff, 0, 0);
		break;
	case PICKUP_OVERCOUNT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_BECAUSE_COUNT)), 0xff, 0, 0);
		break;
	case PICKUP_OVERWEIGHT:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM_BECAUSE_WEIGHT)), 0xff, 0, 0);
		break;
	case PICKUP_FAILED:
	case PICKUP_REFUSED_TIME:
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_CANT_GET_ITEM)), 0xff, 0, 0);
		break;
	}
}


void CGameMode::Zc_Equipment_Itemlist3(char const *buf)
{
	g_session.ClearEquipItemInInventory();
	g_session.ClearEquipItem();

	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO3);
	EQUIPMENTITEM_EXTRAINFO3 *dumpStr = (EQUIPMENTITEM_EXTRAINFO3 *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		itemInfo.m_isYours = dumpStr[i].bindOnEquipType;

		if (g_session.IsPetEgg(atoi(itemInfo.m_itemName.c_str())))
			if (itemInfo.m_isDamaged)
				g_session.m_petEggIndex = itemInfo.m_itemIndex;

		if (itemInfo.m_wearLocation) {
			g_session.AddEquipItem(&itemInfo);
		} else {
			g_session.AddItem(&itemInfo);
			g_session.SetItem(&itemInfo);
		}
	}
}

void CGameMode::Zc_Store_Equipment_Itemlist3(char const *buf)
{
	g_windowMgr.MakeWindow(WID_ITEMSTOREWND);

	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO3);
	EQUIPMENTITEM_EXTRAINFO3 *dumpStr = (EQUIPMENTITEM_EXTRAINFO3 *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		itemInfo.m_isYours = dumpStr[i].bindOnEquipType;
		g_session.AddStoreItem(itemInfo);
	}

	g_session.RefreshStoreItemWnd();
}


void CGameMode::Zc_Cart_Equipment_Itemlist3(char const *buf)
{
	PACKET_ZC_EQUIPMENT_ITEMLIST *p = (PACKET_ZC_EQUIPMENT_ITEMLIST *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST)) / sizeof(EQUIPMENTITEM_EXTRAINFO3);
	EQUIPMENTITEM_EXTRAINFO3 *dumpStr = (EQUIPMENTITEM_EXTRAINFO3 *)(buf + sizeof(PACKET_ZC_EQUIPMENT_ITEMLIST));

	for (int i = 0; i < size; ++i) {
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		itemInfo.m_isYours = dumpStr[i].bindOnEquipType;
		g_session.AddMerchantItem(itemInfo);
	}

	g_session.RefreshMerchantItemWnd();
}

void	CGameMode :: Zc_Notify_Actentry(const char * buf)
{
}


void CGameMode::Zc_Item_Pickup_Party(char const *buf)
{
	PACKET_ZC_ITEM_PICKUP_PARTY *p = (PACKET_ZC_ITEM_PICKUP_PARTY *)buf;

	FRIEND_INFO friendInfo = g_session.GetPartyInfoByAid(p->accountID);
	if (p->accountID != g_session.GetAid() && friendInfo.isValid)
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = p->type;
		itemInfo.m_location = p->location;
		itemInfo.m_itemIndex = p->ITID;
		itemInfo.m_wearLocation = 0;
		itemInfo.SetItemId(p->ITID);
		itemInfo.m_slot[0] = p->slot.info[0];
		itemInfo.m_slot[1] = p->slot.info[1];
		itemInfo.m_slot[2] = p->slot.info[2];
		itemInfo.m_slot[3] = p->slot.info[3];
		itemInfo.m_isIdentified = p->IsIdentified;
		itemInfo.m_isDamaged = p->IsDamaged;
		itemInfo.m_refiningLevel = p->refiningLevel;

		char itemName[64];
		itemInfo.GetItemName(itemName);
		char pickMsg[500];
		sprintf(pickMsg, MsgStr(MSI_ITEM_PICKUP_PARTY), friendInfo.characterName.c_str(), itemName);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 8, 0);
	}
}


void CGameMode::Zc_EquipItem_Damaged(char const *buf)
{
	PACKET_ZC_EQUIPITEM_DAMAGED *p = (PACKET_ZC_EQUIPITEM_DAMAGED *)buf;

	if (p->wearLocation == 0)
		return;

	char msg[500] = {0};
	char nItemName[128];

	if (p->accountID != g_session.GetAid()) {
		if (p->wearLocation == LOCATION_LARM || p->wearLocation == LOCATION_RARM)
			strcpy(nItemName, MsgStr(MSI_DAMAGED_WEAPON));
		else if (p->wearLocation == LOCATION_BODY)
			strcpy(nItemName, MsgStr(MSI_DAMAGED_BODY));

		FRIEND_INFO friendInfo = g_session.GetPartyInfoByAid(p->accountID);

		if (friendInfo.isValid)
			sprintf(msg, MsgStr(MSI_DAMAGED_EQUIP_PARTY), friendInfo.characterName.c_str(), nItemName);
	} else {
		int slotNum[3];
		g_session.BitMaskToSlotNum(p->wearLocation, slotNum);

		ITEM_INFO equipedItem = g_session.GetEquipedItems(slotNum[0]);

		if (equipedItem.m_itemIndex) {
			equipedItem.GetItemName(nItemName);
			sprintf(msg, MsgStr(MSI_DAMAGED_EQUIP), nItemName);
		}
	}

	if (strlen(msg)) {
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xff, 12, 0);
	}
}

void	CGameMode :: Zc_ShortCut_Key_List(const char *buf)
{
	PACKET_ZC_SHORTCUT_KEY_LIST *	p = (PACKET_ZC_SHORTCUT_KEY_LIST *)buf;

	g_session.ClearShortCutItemList();
	memcpy(g_session.m_shortCutList, p->ShortCutKey, sizeof(p->ShortCutKey));
}

void	CGameMode :: Zc_Npc_Chat(const char *buf)
{
	PACKET_ZC_NPC_CHAT *p = (PACKET_ZC_NPC_CHAT *)buf;

	if (g_session.IsExAid(p->accountID))
		return;

	char chatMsg[256];
	int  msgLen = p->packetLength - sizeof(*p);
	memcpy(chatMsg, buf + sizeof(*p), msgLen);

	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), int(p->color), 0, 0);

	CGameActor *theActor = m_world->GetGameActorByAID(p->accountID);
	if (theActor)
		theActor->SendMsg(NULL, AM_CHAT, int(chatMsg), int(p->color), 0);

	if (g_session.m_isSaveChat)
		WriteChat(chatMsg);
}


void CGameMode::Zc_Formatstring_Msg(char const *buf)
{
	PACKET_ZC_FORMATSTRING_MSG *p = (PACKET_ZC_FORMATSTRING_MSG *)buf;
	const char *pFormatStr = MsgStr((MSGSTRINGID)p->msg);

	int arg_num = 0;
	{
		char *pTemp = const_cast<char *>(pFormatStr);
		while (NULL != *pTemp)
		{
			if (IsDBCSLeadByte(*pTemp))
			{
				pTemp += 2;
				continue;
			}
			if ((*pTemp == '%') && (*(pTemp + 1) == 's'))
			{
				arg_num += 1;
				pTemp += 2;
				continue;
			}
			pTemp++;
		}
	}

	char msg[_MAX_PATH + 1] = "";
	switch (arg_num)
	{
	case 0:
		_snprintf(msg, _MAX_PATH, pFormatStr);
		break;
	case 1:
	{
		const char *pArg1 = (char *)p + sizeof(PACKET_ZC_FORMATSTRING_MSG);
		_snprintf(msg, _MAX_PATH, pFormatStr, pArg1);
		break;
	}
	case 2:
	{
		const char *pArg1 = (char *)p + sizeof(PACKET_ZC_FORMATSTRING_MSG);
		const char *pArg2 = pArg1 + strlen(pArg1) + 1;
		_snprintf(msg, _MAX_PATH, pFormatStr, pArg1, pArg2);
		break;
	}
	case 3:
	{
		const char *pArg1 = (char *)p + sizeof(PACKET_ZC_FORMATSTRING_MSG);
		const char *pArg2 = pArg1 + strlen(pArg1) + 1;
		const char *pArg3 = pArg2 + strlen(pArg2) + 1;
		_snprintf(msg, _MAX_PATH, pFormatStr, pArg1, pArg2, pArg3);
		break;
	}
	default:
		sprintf(msg, "unknown argment list");
		return;
	}

	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffffff, 0, 0);
}

void	CGameMode :: Zc_Party_Config(const char *buf)
{
	if (g_session.m_bRefuseInviteParty = buf[2])
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,
			int(MsgStr(MSI_INVITE_PARTY_REFUSE)),
			0x64ffff, 0, 0);
	else
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY,
			int(MsgStr(MSI_INVITE_PARTY_ACCEPT)),
			0x64ffff, 0, 0);
}


void CGameMode::Zc_MemorialDungeon_Subscription_Info(char const *buf)
{
	PACKET_ZC_MEMORIALDUNGEON_SUBSCRIPTION_INFO *p =
		(PACKET_ZC_MEMORIALDUNGEON_SUBSCRIPTION_INFO *)buf;

	g_session.m_dunName = (char *)p->MemorialDungeonName;
	g_session.m_priority = p->PriorityOrderNum;
	g_session.m_dunState = 0;

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_MEMORIAL_DUN_WND);
	if (wnd)
		wnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}

void	CGameMode :: Zc_MemorialDungeon_Subscription_Notify(const char *buf)
{
	PACKET_ZC_MEMORIALDUNGEON_SUBSCRIPTION_NOTIFY *	p = (PACKET_ZC_MEMORIALDUNGEON_SUBSCRIPTION_NOTIFY *)buf;
	g_session.m_priority = p->PriorityOrderNum;
	UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_MEMORIAL_DUN_WND);
	if (wnd)
	{
		if (p->PriorityOrderNum > -1)
		{
			g_session.m_dunState = 0;
			wnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		}
		else
		{
			g_windowMgr.DeleteWindow(WID_MEMORIAL_DUN_WND);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MEMORIAL_DUN_CANCEL)), 0x64ffff, 0, 0);
		}
	}
}


void CGameMode::Zc_MemorialDungeon_Info(char const *buf)
{
	PACKET_ZC_MEMORIALDUNGEON_INFO *p = (PACKET_ZC_MEMORIALDUNGEON_INFO *)buf;

	g_session.m_dunName = (char *)p->MemorialDungeonName;
	g_session.m_destroyTime = p->DestroyDate;
	g_session.m_enterTimeOut = p->EnterTimeOutDate;

	int tmpDunState = g_session.m_dunState;
	if (p->EnterTimeOutDate != 0)
		g_session.m_dunState = 1;
	else
		g_session.m_dunState = 2;

	if (tmpDunState == g_session.m_dunState && g_session.m_dunState == 2 && !g_session.m_onDunInfoWnd)
		return;

	UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_MEMORIAL_DUN_WND);
	if (wnd)
		wnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}

void	CGameMode :: Zc_MemorialDungeon_Notify(const char *buf)
{
	int	prevDunState = g_session.m_dunState;
	g_session.m_enterTimeOut = *(int *)(buf + 6);
	int	enterLimit = *(int *)(buf + 6);
	g_session.m_dunState = enterLimit ? 1 : 2;
	if (*(int *)(buf + 2) == 0) {
		if (prevDunState == g_session.m_dunState && g_session.m_dunState == 2 && !g_session.m_onDunInfoWnd)
			return;
		UIFrameWnd *	wnd = g_windowMgr.MakeWindow(WID_MEMORIAL_DUN_WND);
		if (wnd)
			wnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		return;
	}
	g_windowMgr.DeleteWindow(WID_MEMORIAL_DUN_WND);
	int	type = *(int *)(buf + 2);
	if (type == 1)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MEMORIAL_DUN_LIVE_TIME_OUT)), 0x64ffff, 0, 0);
	else if (type == 2)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MEMORIAL_DUN_ENTER_TIME_OUT)), 0x64ffff, 0, 0);
	else if (type == 3)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MEMORIAL_DUN_DESTROY_REQUEST)), 0x64ffff, 0, 0);
}


void	CGameMode :: Zc_Isvr_Disconnect(const char *buf)
{
	if (g_windowMgr.GetMemorialDunWnd())
	{
		g_windowMgr.DeleteWindow(WID_MEMORIAL_DUN_WND);
		CRagConnection::instanceR().Disconnect();
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_MEMORIAL_DUN_ERROR), ME_OK, TRUE, TRUE, 0) != ID_MODAL_PASSED)
			SendMsg(MM_QUIT_TO_IDENTRY, 0, 0, 0);
	}
}


void CGameMode::Zc_EquipwinMicroscope(char const *buf)
{
	g_session.ClearOtherUserEquipItem();

	PACKET_ZC_EQUIPWIN_MICROSCOPE *p = (PACKET_ZC_EQUIPWIN_MICROSCOPE *)buf;
	int size = (p->Length - sizeof(PACKET_ZC_EQUIPWIN_MICROSCOPE)) / sizeof(EQUIPMENTITEM_EXTRAINFO3);
	EQUIPMENTITEM_EXTRAINFO3 *dumpStr = (EQUIPMENTITEM_EXTRAINFO3 *)(buf + sizeof(PACKET_ZC_EQUIPWIN_MICROSCOPE));

	for (int i = 0; i < size; ++i)
	{
		ITEM_INFO itemInfo;
		itemInfo.m_itemType = dumpStr[i].type;
		itemInfo.m_location = dumpStr[i].location;
		itemInfo.m_itemIndex = dumpStr[i].index;
		itemInfo.m_wearLocation = dumpStr[i].WearState;
		itemInfo.m_num = 1;
		itemInfo.m_slot[0] = dumpStr[i].slot.info[0];
		itemInfo.m_slot[1] = dumpStr[i].slot.info[1];
		itemInfo.m_slot[2] = dumpStr[i].slot.info[2];
		itemInfo.m_slot[3] = dumpStr[i].slot.info[3];
		itemInfo.SetItemId(dumpStr[i].ITID);
		itemInfo.m_isIdentified = dumpStr[i].IsIdentified;
		itemInfo.m_isDamaged = dumpStr[i].IsDamaged;
		itemInfo.m_refiningLevel = dumpStr[i].RefiningLevel;
		itemInfo.m_deleteTime = dumpStr[i].HireExpireDate;
		itemInfo.m_isYours = dumpStr[i].bindOnEquipType;

		if (itemInfo.m_wearLocation)
			g_session.AddOtherUserEquipItem(&itemInfo);
	}

	g_session.m_otherUserJob = p->job;
	g_session.m_otherUserSex = p->sex;
	g_session.m_otherUserHead = p->head;
	g_session.m_otherUserAccessory = p->accessory;
	g_session.m_otherUserAccessory2 = p->accessory2;
	g_session.m_otherUserAccessory3 = p->accessory3;
	g_session.m_otherUserBodyPalette = p->bodypalette;
	g_session.m_otherUserHeadPalette = p->headpalette;
	memcpy(g_session.m_otherUserName, p->characterName, NAME_LENGTH);

	if (!g_windowMgr.DeleteWindow(WID_OTHERUSER_EQUIPWND))
		g_windowMgr.MakeWindow(WID_OTHERUSER_EQUIPWND);
}

extern int g_mustPumpOutReceiveQueue;

inline void ReassemblyCertify(PACKET_CZ_REASSEMBLY_IDENTITY& p)
{
	p.PacketLength = sizeof(PACKET_CZ_REASSEMBLY_IDENTITY);
	p.bImportAddressTableModify = CRagConnection::instanceR().CheckModulePt() ? false : true;
	CRagConnection::instanceR().SendPacket(p.PacketLength, (char *)&p);
}

inline void CGameMode::Zc_Notify_Moveentry5(const char *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY5 *	p = (PACKET_ZC_NOTIFY_MOVEENTRY5 *)buf;
	*(unsigned long *)p->moveData ^= p->GID;
	Inner_Zc_Notify_Moveentry5(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->moveStartTime,
		p->accessory2, p->accessory3, p->headpalette, p->bodypalette, p->headDir,
		p->GUID, p->GEmblemVer, p->honor, p->virtue, p->isPKModeON, p->sex,
		p->moveData, p->xSize, p->ySize, p->clevel);
}

inline void CGameMode::Zc_Notify_Moveentry5__m1(const char *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY5 *	p = (PACKET_ZC_NOTIFY_MOVEENTRY5 *)buf;
	*(unsigned long *)p->moveData ^= p->GID;
	Inner_Zc_Notify_Moveentry5(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->moveStartTime,
		p->accessory2, p->accessory3, p->headpalette, p->bodypalette, p->headDir,
		p->GUID, p->GEmblemVer, p->honor, p->virtue, p->isPKModeON, p->sex,
		p->moveData, p->xSize, p->ySize, p->clevel);
}

inline void CGameMode::Zc_Notify_Moveentry5__m2(const char *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY5 *	p = (PACKET_ZC_NOTIFY_MOVEENTRY5 *)buf;
	*(unsigned long *)p->moveData ^= p->GID;
	Inner_Zc_Notify_Moveentry5(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->moveStartTime,
		p->accessory2, p->accessory3, p->headpalette, p->bodypalette, p->headDir,
		p->GUID, p->GEmblemVer, p->honor, p->virtue, p->isPKModeON, p->sex,
		p->moveData, p->xSize, p->ySize, p->clevel);
}

inline void CGameMode::Zc_Notify_Npc_Standentry__m1(const char *buf)
{
	PACKET_ZC_NOTIFY_NPC_STANDENTRY *	p = (PACKET_ZC_NOTIFY_NPC_STANDENTRY *)buf;
	*(unsigned short *)p->PosDir ^= (unsigned short)p->GID;
	Inner_Zc_Notify_Npc_Standentry(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->shield, p->accessory2,
		p->accessory3, p->headpalette, p->bodypalette, p->headDir, p->GUID, p->GEmblemVer,
		p->honor, p->virtue, p->isPKModeON, p->sex, p->PosDir, p->xSize, p->ySize,
		p->state, p->clevel);
}

inline void CGameMode::Zc_Notify_Npc_Standentry__m2(const char *buf)
{
	PACKET_ZC_NOTIFY_NPC_STANDENTRY *	p = (PACKET_ZC_NOTIFY_NPC_STANDENTRY *)buf;
	*(unsigned short *)p->PosDir ^= (unsigned short)p->GID;
	Inner_Zc_Notify_Npc_Standentry(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->shield, p->accessory2,
		p->accessory3, p->headpalette, p->bodypalette, p->headDir, p->GUID, p->GEmblemVer,
		p->honor, p->virtue, p->isPKModeON, p->sex, p->PosDir, p->xSize, p->ySize,
		p->state, p->clevel);
}

inline void CGameMode::Zc_Notify_Npc_Standentry__m5(const char *buf)
{
	PACKET_ZC_NOTIFY_NPC_STANDENTRY *	p = (PACKET_ZC_NOTIFY_NPC_STANDENTRY *)buf;
	*(unsigned short *)p->PosDir ^= (unsigned short)p->GID;
	Inner_Zc_Notify_Npc_Standentry(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->shield, p->accessory2,
		p->accessory3, p->headpalette, p->bodypalette, p->headDir, p->GUID, p->GEmblemVer,
		p->honor, p->virtue, p->isPKModeON, p->sex, p->PosDir, p->xSize, p->ySize,
		p->state, p->clevel);
}

void	CGameMode :: Zc_Config(const char *buf)
{
	int		val = *(int *)(buf + 6);
	g_session.m_bOpenEquipedItem = val;
	if (val)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OPEN_EQUIPEDITEM_ACCEPT)), 0x64ffff, 0, 0);
	else
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OPEN_EQUIPEDITEM_REFUSE)), 0x64ffff, 0, 0);
	UIEquipWnd *equipWnd = g_windowMgr.GetEquipWnd();
	if (equipWnd)
		equipWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}

void	CGameMode :: Zc_Config_Notify(const char *buf)
{
	int		val = (unsigned char)buf[2];
	g_session.m_bOpenEquipedItem = val;
	if (val)
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OPEN_EQUIPEDITEM_ACCEPT)), 0x64ffff, 0, 0);
	else
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_OPEN_EQUIPEDITEM_REFUSE)), 0x64ffff, 0, 0);
	UIEquipWnd *equipWnd = g_windowMgr.GetEquipWnd();
	if (equipWnd)
		equipWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}


void	CGameMode :: Zc_Mapproperty(const char *buf)
{
	PACKET_ZC_MAPPROPERTY *	p = (PACKET_ZC_MAPPROPERTY *) buf ;

	int size = p->PacketLength - sizeof(PACKET_ZC_MAPPROPERTY) ;

	for ( int i = 0 ; i < size ; i++ )
		g_session.m_mapInfoTable[p->type][i] = atoi(buf + sizeof(PACKET_ZC_MAPPROPERTY)+i) ;
}



#include "rag_budget_sites.h"

void CGameMode::PollNetworkStatus()
{
	RAG_BUDGET_FILL(7)
	if (!CRagConnection::instanceR().Poll())
		g_modeMgr.GetCurMode()->SendMsg(MM_SOCKETERROR, 0, 0, 0);

	if (g_mustPumpOutReceiveQueue == 1) {
		DWORD aid;
		if (CRagConnection::instanceR().Recv((char *)&aid, 4, true))
			g_mustPumpOutReceiveQueue = 0;
		return;
	}

	char buf[20480];
	while (CRagConnection::instanceR().RecvPacket(buf)) {
		int packetType = CRagConnection::instanceR().GetPacketType(buf);
		switch (packetType) {
		case 117:
			break;
		case 118:
			break;
		case 119:
			break;
		case 777:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 819; ReassemblyCertify(p); }
			break;
		case 120:
			Zc_Notify_Standentry(buf);
			break;
		case 756:
			Zc_Item_Fall_Entry2(buf);
			break;
		case 124:
			Zc_Notify_Standentry_Npc(buf);
			break;
		case 122:
			Zc_Notify_Actentry(buf);
			break;
		case 123:
			Zc_Notify_Moveentry(buf);
			break;
		case 778:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 820; ReassemblyCertify(p); }
			break;
		case 128:
			Zc_Notify_Vanish(buf);
			break;
		case 771:
			Zc_Notify_Npc_Standentry__m2(buf);
			break;
		case 131:
			Zc_Accept_Quit(buf);
			break;
		case 129:
			Sc_Notify_Ban(buf);
			break;
		case 132:
			Zc_Refuse_Quit(buf);
			break;
		case 779:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 821; ReassemblyCertify(p); }
			break;
		case 134:
			Zc_Notify_Move(buf);
			break;
		case 135:
			Zc_Notify_Playermove(buf);
			break;
		case 138:
			{
				PACKET_ZC_NOTIFY_ACT *p = (PACKET_ZC_NOTIFY_ACT *)buf;
				Zc_Notify_Act(p->GID, p->targetGID, p->startTime, p->attackMT, p->attackedMT, p->damage, p->count, p->action, p->leftDamage);
			}
			break;
		case 780:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 822; ReassemblyCertify(p); }
			break;
		case 737:
			{
				PACKET_ZC_NOTIFY_ACT2 *p = (PACKET_ZC_NOTIFY_ACT2 *)buf;
				Zc_Notify_Act(p->GID, p->targetGID, p->startTime, p->attackMT, p->attackedMT, p->damage, p->count, p->action, p->leftDamage);
			}
			break;
		case 142:
			Zc_Notify_Playerchat(buf);
			break;
		case 141:
			Zc_Notify_Chat(buf);
			break;
		case 781:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 823; ReassemblyCertify(p); }
			break;
		case 127:
			Zc_Notify_Time(buf);
			break;
		case 774:
			Zc_Notify_Npc_Standentry__m5(buf);
			break;
		case 145:
			Zc_Npcack_Mapmove(buf);
		case 147:
			Zc_Npcack_Enable(buf);
			break;
		case 782:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 824; ReassemblyCertify(p); }
			break;
		case 146:
			Zc_Npcack_Servermove(buf);
			break;
		case 115:
			Zc_Accept_Enter(buf);
			break;
		case 764:
			Zc_Notify_Moveentry5__m2(buf);
			break;
		case 747:
			Zc_Accept_Enter2(buf);
			break;
		case 783:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 825; ReassemblyCertify(p); }
			break;
		case 149:
			Zc_Ack_Reqname(buf); RAG_BUDGET_SITES(14)
			break;
		case 765:
			Zc_Notify_Moveentry5__m1(buf);
			break;
		case 154:
			Zc_Broadcast(buf);
			break;
		case 151:
			Zc_Whisper(buf);
			break;
		case 784:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 826; ReassemblyCertify(p); }
			break;
		case 152:
			Zc_Ack_Whisper(buf);
			break;
		case 156:
			Zc_Change_Direction(buf);
			break;
		case 205:
			Zc_Ack_Disconnect_Character(buf);
			break;
		case 772:
			Zc_Notify_Npc_Standentry__m2(buf);
			break;
		case 157:
			Zc_Item_Entry(buf);
			break;
		case 160:
			Zc_Item_Pickup_Ack(buf);
			break;
		case 785:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 827; ReassemblyCertify(p); }
			break;
		case 161:
			Zc_Item_Disappear(buf);
			break;
		case 163:
			Zc_Normal_Itemlist(buf);
			break;
		case 494:
			Zc_Normal_Itemlist2(buf);
			break;
		case 744:
			Zc_Normal_Itemlist3(buf);
			break;
		case 786:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 828; ReassemblyCertify(p); }
			break;
		case 164:
			Zc_Equipment_Itemlist(buf);
			break;
		case 158:
			Zc_Item_Fall_Entry(buf);
			break;
		case 136:
			Zc_Stopmove(buf);
			break;
		case 787:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 829; ReassemblyCertify(p); }
			break;
		case 511:
			Zc_HighJump(buf);
			break;
		case 176:
			Zc_Par_Change(buf);
			break;
		case 177:
			Zc_Longpar_Change(buf);
			break;
		case 788:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 830; ReassemblyCertify(p); }
			break;
		case 168:
			Zc_Use_Item_Ack(buf);
			break;
		case 170:
			Zc_Req_Wear_Equip_Ack(buf);
			break;
		case 172:
			Zc_Req_Takeoff_Equip_Ack(buf);
			break;
		case 175:
			Zc_Item_Throw_Ack(buf);
			break;
		case 789:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 831; ReassemblyCertify(p); }
			break;
		case 180:
			Zc_Say_Dialog(buf);
			break;
		case 181:
			Zc_Wait_Dialog(buf);
			break;
		case 182:
			Zc_Close_Dialog(buf);
			break;
		case 183:
			Zc_Menu_List(buf);
			break;
		case 790:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 832; ReassemblyCertify(p); }
			break;
		case 179:
			Zc_Restart_Ack(buf);
			break;
		case 188:
			Zc_Status_Change_Ack(buf);
			break;
		case 189:
			Zc_Status(buf);
			break;
		case 190:
			Zc_Status_Change(buf);
			break;
		case 192:
			Zc_Emotion(buf);
			break;
		case 194:
			Zc_User_Count(buf);
			break;
		case 537:
			ZC_BLACKSMITH_RANK(buf);
			break;
		case 791:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 833; ReassemblyCertify(p); }
			break;
		case 538:
			ZC_ALCHEMIST_RANK(buf);
			break;
		case 550:
			ZC_TAEKWON_RANK(buf);
			break;
		case 642:
			ZC_GANGSI_RANK(buf);
			break;
		case 539:
			ZC_BLACKSMITH_POINT(buf);
			break;
		case 540:
			ZC_ALCHEMIST_POINT(buf);
			break;
		case 792:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 834; ReassemblyCertify(p); }
			break;
		case 548:
			ZC_TAEKWON_POINT(buf);
			break;
		case 640:
			ZC_GANGSI_POINT(buf);
			break;
		case 645:
			ZC_DEATH_QUESTION(buf);
			break;
		case 770:
			Zc_Notify_Npc_Standentry__m1(buf);
			break;
		case 121:
			Zc_Notify_Newentry(buf);
			break;
		case 793:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 835; ReassemblyCertify(p); }
			break;
		case 195:
			Zc_Sprite_Change(buf);
			break;
		case 196:
			Zc_Select_Dealtype(buf);
			break;
		case 198:
			Zc_Pc_Purchase_Itemlist(buf);
			break;
		case 199:
			Zc_Pc_Sell_Itemlist(buf);
			break;
		case 794:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 836; ReassemblyCertify(p); }
			break;
		case 202:
			Zc_Pc_Purchase_Result(buf);
			break;
		case 203:
			Zc_Pc_Sell_Result(buf);
			break;
		case 758:
			Zc_Item_Fall_Entry2(buf);
			break;
		case 795:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 837; ReassemblyCertify(p); }
			break;
		case 209:
			Zc_Setting_Whisper_Pc(buf);
			break;
		case 210:
			Zc_Setting_Whisper_State(buf);
			break;
		case 212:
			Zc_Whisper_List(buf);
			break;
		case 796:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 838; ReassemblyCertify(p); }
			break;
		case 214:
			Zc_Ack_Create_Chatroom(buf);
			break;
		case 215:
			Zc_Room_Newentry(buf);
			break;
		case 797:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 839; ReassemblyCertify(p); }
			break;
		case 216:
			Zc_Destroy_Room(buf);
			break;
		case 219:
			Zc_Enter_Room(buf);
			break;
		case 220:
			Zc_Member_Newentry(buf);
			break;
		case 798:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 840; ReassemblyCertify(p); }
			break;
		case 221:
			Zc_Member_Exit(buf);
			break;
		case 225:
			Zc_Role_Change(buf);
			break;
		case 799:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 841; ReassemblyCertify(p); }
			break;
		case 218:
			Zc_Refuse_Enter_Room(buf);
			break;
		case 223:
			Zc_Change_Chatroom(buf);
			break;
		case 800:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 842; ReassemblyCertify(p); }
			break;
		case 229:
			Zc_Req_Exchange_Item(buf);
			break;
		case 500:
			Zc_Req_Exchange_Item2(buf);
			break;
		case 801:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 843; ReassemblyCertify(p); }
			break;
		case 231:
			Zc_Ack_Exchange_Item(buf);
			break;
		case 501:
			Zc_Ack_Exchange_Item2(buf);
			break;
		case 233:
			Zc_Add_Exchange_Item(buf);
			break;
		case 802:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 844; ReassemblyCertify(p); }
			break;
		case 234:
			Zc_Ack_Add_Exchange_Item(buf);
			break;
		case 236:
			Zc_Conclude_Exchange_Item(buf);
			break;
		case 238:
			Zc_Cancel_Exchange_Item(buf);
			break;
		case 240:
			Zc_Exec_Exchange_Item(buf);
			break;
		case 803:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 845; ReassemblyCertify(p); }
			break;
		case 241:
			Zc_Exchangeitem_Undo(buf);
			break;
		case 165:
			Zc_Store_Normal_Itemlist(buf);
			break;
		case 496:
			Zc_Store_Normal_Itemlist2(buf);
			break;
		case 804:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 846; ReassemblyCertify(p); }
			break;
		case 746:
			Zc_Store_Normal_Itemlist3(buf);
			break;
		case 166:
			Zc_Store_Equipment_Itemlist(buf);
			break;
		case 244:
			Zc_Add_Item_To_Store(buf);
			break;
		case 805:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 847; ReassemblyCertify(p); }
			break;
		case 246:
			Zc_Delete_Item_From_Store(buf);
			break;
		case 248:
			Zc_Close_Store(buf);
			break;
		case 242:
			Zc_Notify_Storeitem_Countinfo(buf);
			break;
		case 250:
			Zc_Ack_Make_Group(buf);
			break;
		case 806:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 848; ReassemblyCertify(p); }
			break;
		case 251:
			Zc_Group_List(buf);
			break;
		case 253:
		case 709:
			Zc_Ack_Req_Join_Group(buf);
			break;
		case 254:
		case 710:
			Zc_Req_Join_Group(buf);
			break;
		case 482:
			Zc_Req_Couple(buf);
			break;
		case 807:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 849; ReassemblyCertify(p); }
			break;
		case 502:
			Zc_Req_Baby(buf);
			break;
		case 595:
			Zc_StarPlace(buf);
			break;
		case 723:
			Zc_NotifyBindOnEquip(buf);
			break;
		case 733:
			Zc_Battlefield_Notify_Campinfo(buf);
			break;
		case 751:
			Zc_Notify_Font(buf);
			break;
		case 752:
			Zc_Progress(buf);
			break;
		case 775:
			Zc_Notify_Npc_Standentry__m5(buf);
			break;
		case 754:
			Zc_Progress_Cancel(buf);
			break;
		case 808:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 850; ReassemblyCertify(p); }
			break;
		case 732:
			Zc_Battlefield_Chat(buf);
			break;
		case 734:
			Zc_Battlefield_Notify_Point(buf);
			break;
		case 735:
			Zc_Battlefield_Notify_Position(buf);
			break;
		case 736:
			Zc_Battlefield_Notify_Hp(buf);
			break;
		case 809:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 851; ReassemblyCertify(p); }
			break;
		case 526:
			Zc_StarSkill(buf);
			break;
		case 532:
			Zc_Ack_Status_GM(buf);
			break;
		case 260:
			Zc_Add_Member_To_Group(buf);
			break;
		case 810:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 852; ReassemblyCertify(p); }
			break;
		case 489:
			Zc_Add_Member_To_Group2(buf);
			break;
		case 261:
			Zc_Delete_Member_From_Group(buf);
			break;
		case 811:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 853; ReassemblyCertify(p); }
			break;
		case 262:
			Zc_Notify_Hp_To_Groupm(buf);
			break;
		case 263:
			Zc_Notify_Position_To_Groupm(buf);
			break;
		case 491:
			Zc_Notify_Position_To_Guildm(buf);
			break;
		case 492:
			Zc_Guild_Member_Map_Change(buf);
			break;
		case 265:
			Zc_Notify_Chat_Party(buf);
			break;
		case 812:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 854; ReassemblyCertify(p); }
			break;
		case 266:
			Zc_Mvp_Getting_Item(buf);
			break;
		case 267:
			Zc_Mvp_Getting_Special_Exp(buf);
			break;
		case 268:
			Zc_Mvp(buf);
			break;
		case 269:
			Zc_Throw_Mvpitem(buf);
			break;
		case 270:
		case 569:
		case 670:
			Zc_Skillinfo_Update(buf);
			break;
		case 271:
		case 565:
		case 669:
			Zc_Skillinfo_List(buf);
			break;
		case 689:
			Zc_All_Quest_List(buf);
			break;
		case 690:
			Zc_All_Quest_Mission(buf);
			break;
		case 693:
			Zc_Update_Mission_Hunt(buf);
			break;
		case 813:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 855; ReassemblyCertify(p); }
			break;
		case 695:
			Zc_Active_Quest(buf);
			break;
		case 691:
			Zc_Add_Quest(buf);
			break;
		case 761:
			Zc_Item_Fall_Entry2(buf);
			break;
		case 814:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 856; ReassemblyCertify(p); }
			break;
		case 692:
			Zc_Del_Quest(buf);
			break;
		case 272:
			Zc_Ack_Touseskill(buf);
			break;
		case 533:
			Zc_SKILLMSG(buf);
			break;
		case 534:
			Zc_BABYMSG(buf);
			break;
		case 273:
			Zc_Add_Skill(buf);
			break;
		case 276:
			Zc_Notify_Skill(buf);
			break;
		case 815:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 857; ReassemblyCertify(p); }
			break;
		case 279:
			Zc_Notify_Groundskill(buf);
			break;
		case 281:
			Zc_State_Change(buf);
			break;
		case 553:
			Zc_State_Change3(buf);
			break;
		case 282:
			Zc_Use_Skill(buf);
			break;
		case 287:
			Zc_Skill_Entry(buf);
			break;
		case 288:
			Zc_Skill_Disappear(buf);
			break;
		case 139:
			Zc_Notify_Act_Position(buf);
			break;
		case 277:
			Zc_Notify_Skill_Position(buf);
			break;
		case 284:
			Zc_Warplist(buf);
			break;
		case 286:
			Zc_Ack_Remember_Warppoint(buf);
			break;
		case 816:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 858; ReassemblyCertify(p); }
			break;
		case 289:
			Zc_Notify_Cartitem_Countinfo(buf);
			break;
		case 291:
			Zc_Cart_Normal_Itemlist(buf);
			break;
		case 495:
			Zc_Cart_Normal_Itemlist2(buf);
			break;
		case 745:
			Zc_Cart_Normal_Itemlist3(buf);
			break;
		case 290:
			Zc_Cart_Equipment_Itemlist(buf);
			break;
		case 299:
			Zc_Cartoff(buf);
			break;
		case 817:
			{ PACKET_CZ_REASSEMBLY_IDENTITY p = {0}; p.PacketType = 859; ReassemblyCertify(p); }
			break;
		case 300:
			Zc_Ack_Additem_To_Cart(buf);
			break;
		case 292:
			Zc_Add_Item_To_Cart(buf);
			break;
		case 293:
			Zc_Delete_Item_From_Cart(buf);
			break;
		case 301:
			Zc_openstore(buf);
			break;
		case 305:
			Zc_store_entry(buf);
			break;
		case 306:
			Zc_disappear_entry(buf);
			break;
		case 307:
			Zc_pc_purchase_itemlist_frommc(buf);
			break;
		case 309:
			Zc_pc_purchase_result_frommc(buf);
			break;
		case 310:
			Zc_pc_purchase_myitemlist(buf);
			break;
		case 311:
			Zc_deleteitem_from_mcstore(buf);
			break;
		case 313:
			Zc_attack_failure_for_distance(buf);
			break;
		case 314:
			Zc_attack_range(buf);
			break;
		case 315:
			Zc_action_failure(buf);
			break;
		case 316:
			Zc_equip_arrow(buf);
			break;
		case 317:
			Zc_recovery(buf);
			break;
		case 318:
			Zc_useskill_ack(buf);
			break;
		case 257:
			Zc_groupinfo_change(buf);
			break;
		case 321:
			Zc_couplestatus(buf);
			break;
		case 322:
			Zc_open_editdlg(buf);
			break;
		case 324:
			Zc_compass(buf);
			break;
		case 325:
			Zc_show_image(buf);
			break;
		case 435:
			Zc_show_image2(buf);
			break;
		case 327:
			Zc_autorun_skill(buf);
			break;
		case 328:
			Zc_resurrection(buf);
			break;
		case 338:
			Zc_guild_emblem_img(buf);
			break;
		case 336:
			Zc_guild_info(buf);
			break;
		case 356:
			Zc_other_guild_list(buf);
			break;
		case 359:
			Zc_result_make_guild(buf);
			break;
		case 361:
			Zc_ack_req_join_guild(buf);
			break;
		case 362:
			Zc_req_join_guild(buf);
			break;
		case 332:
			Zc_myguild_basic_info(buf);
			break;
		case 334:
			Zc_ack_guild_menuinterface(buf);
			break;
		case 340:
			Zc_membermgr_info(buf);
			break;
		case 342:
			Zc_ack_req_change_members(buf);
			break;
		case 344:
			Zc_ack_open_member_info(buf);
			break;
		case 346:
			Zc_ack_leave_guild(buf);
			break;
		case 348:
			Zc_ack_ban_guild(buf);
			break;
		case 350:
			Zc_ack_disorganize_guild_result(buf);
			break;
		case 351:
			Zc_ack_disorganize_guild(buf);
			break;
		case 352:
			Zc_position_info(buf);
			break;
		case 355:
			Zc_ban_list(buf);
			break;
		case 358:
			Zc_position_id_name_info(buf);
			break;
		case 364:
			Zc_update_gdid(buf);
			break;
		case 365:
			Zc_update_charstat(buf);
			break;
		case 498:
			Zc_update_charstat2(buf);
			break;
		case 330:
			Zc_ack_give_manner_point(buf);
			break;
		case 331:
			Zc_notify_manner_point_given(buf);
			break;
		case 367:
			Zc_guild_notice(buf);
			break;
		case 383:
			Zc_guild_chat(buf);
			break;
		case 369:
			Zc_req_ally_guild(buf);
			break;
		case 371:
			Zc_ack_req_ally_guild(buf);
			break;
		case 372:
			Zc_ack_change_guild_positioninfo(buf);
			break;
		case 375:
			Zc_itemidentify_list(buf);
			break;
		case 377:
			Zc_ack_itemidentify(buf);
			break;
		case 379:
			Zc_itemcomposition_list(buf);
			break;
		case 381:
			Zc_ack_itemcomposition(buf);
			break;
		case 385:
			Zc_ack_req_hostile_guild(buf);
			break;
		case 386:
			Zc_member_add(buf);
			break;
		case 388:
			Zc_delete_related_guild(buf);
			break;
		case 389:
			Zc_add_related_guild(buf);
			break;
		case 392:
			Zc_ack_itemrefining(buf);
			break;
		case 391:
			break;
		case 393:
			Zc_notify_mapinfo(buf);
			break;
		case 395:
			Zc_ack_req_disconnect(buf);
			break;
		case 396:
			Zc_monster_info(buf);
			break;
		case 397:
			Zc_makable_item_list(buf);
			break;
		case 508:
			Zc_repair_item_list(buf);
			break;
		case 545:
			Zc_notify_weapon_item_list(buf);
			break;
		case 547:
			Zc_ack_weaponrefine(buf);
			break;
		case 510:
			Zc_ack_itemrepair(buf);
			break;
		case 399:
			Zc_ack_req_itemmaking(buf);
			break;
		case 401:
			Zc_talkbox_chat_contents(buf);
			break;
		case 763:
			Zc_Notify_Moveentry5__m1(buf);
			break;
		case 402:
			Zc_update_mapinfo(buf);
			break;
		case 773:
			Zc_Notify_Npc_Standentry__m2(buf);
			break;
		case 404:
			Zc_Ack_Reqname_bygid(buf);
			break;
		case 405:
			Zc_Ack_Reqnameall(buf);
			break;
		case 406:
			Zc_Msg_State_Change(buf);
			break;
		case 650:
			Zc_Npc_Showefst_Update(buf);
			break;
		case 409:
			Zc_Notify_Mapproperty(buf);
			break;
		case 410:
			Zc_Notify_Ranking(buf);
			break;
		case 762:
			Zc_Notify_Moveentry5(buf);
			break;
		case 411:
			Zc_Notify_Effect(buf);
			break;
		case 499:
			Zc_Notify_Effect2(buf);
			break;
		case 767:
			Zc_Notify_Moveentry5__m1(buf);
			break;
		case 644:
			Zc_Notify_Effect3(buf);
			break;
		case 414:
			Zc_Start_Capture(buf);
			break;
		case 484:
			Zc_Start_Couple(buf);
			break;
		case 504:
			Zc_Start_Baby(buf);
			break;
		case 486:
			Zc_CoupleName(buf);
			break;
		case 490:
			Zc_Congratulation(buf);
			break;
		case 416:
			Zc_TryCapture_Monster(buf);
			break;
		case 422:
			Zc_PetEgg_List(buf);
			break;
		case 420:
			Zc_Change_Pet_Status(buf);
			break;
		case 418:
			Zc_Property_Pet(buf);
			break;
		case 760:
			Zc_Item_Fall_Entry2__m5(buf);
			break;
		case 419:
		case 559:
			Zc_Feed_Pet(buf);
			break;
		case 426:
			Zc_Pet_Act(buf);
			break;
		case 427:
			Zc_Par_Change_User(buf);
			break;
		case 428:
			Zc_Skill_Update(buf);
			break;
		case 429:
			Zc_MakingArrowList(buf);
			break;
		case 602:
			Zc_MakingItemList(buf);
			break;
		case 432:
			Zc_NpcSpriteChange(buf);
			break;
		case 433:
			Zc_ShowDigit(buf);
			break;
		case 436:
			Zc_ChangeGuild(buf);
			break;
		case 438:
			Zc_GuildInfo2(buf);
			break;
		case 440:
			Zc_GuildZenyAck(buf);
			break;
		case 441:
			Zc_Dispel(buf);
			break;
		case 449:
			Zc_Reply_RemainTime(buf);
			break;
		case 450:
			Zc_Info_RemainTime(buf);
			break;
		case 451:
			Zc_Broadcast2(buf);
			break;
		case 354:
			Zc_GuildSkillInfo(buf);
			break;
		case 452:
			Zc_AddItemToStore2(buf);
			break;
		case 453:
			Zc_Add_Item_To_Cart2(buf);
			break;
		case 455:
			Sc_Ack_Encryption(buf);
			break;
		case 456:
			Zc_Use_Item_Ack2(buf);
			break;
		case 457:
			Zc_Skill_Entry2(buf);
			break;
		case 460:
			Zc_Monster_Talk(buf);
			break;
		case 766:
			Zc_Notify_Moveentry5__m4(buf);
			break;
		case 461:
			Zc_AutoSpellList(buf);
			break;
		case 463:
			Zc_DevotionList(buf);
			break;
		case 464:
			Zc_Spirits(buf);
			break;
		case 481:
			Zc_Spirits2(buf);
			break;
		case 465:
			Zc_BladeStop(buf);
			break;
		case 466:
			Zc_ComboDelay(buf);
			break;
		case 467:
			Zc_Sound(buf);
			break;
		case 468:
			Zc_Open_EditDlgStr(buf);
			break;
		case 470:
			Zc_Notify_Mapproperty2(buf);
			break;
		case 471:
			Zc_Sprite_Change2(buf);
			break;
		case 472:
			Zc_Notify_Standentry2(buf);
			break;
		case 473:
			Zc_Notify_Newentry2(buf);
			break;
		case 474:
			Zc_Notify_Moveentry2(buf);
			break;
		case 554:
			Zc_Notify_Standentry3(buf);
			break;
		case 750:
			Zc_Notify_Standentry4(buf);
			break;
		case 555:
			Zc_Notify_Newentry3(buf);
			break;
		case 749:
			Zc_Notify_Newentry4(buf);
			break;
		case 556:
			Zc_Notify_Moveentry3(buf);
			break;
		case 748:
			Zc_Notify_Moveentry4(buf);
			break;
		case 478:
			Zc_Notify_Skill2(buf);
			break;
		case 480:
			Zc_Ack_AccountName(buf);
			break;
		case 513:
			Zc_Friends_List(buf);
			break;
		case 519:
			Zc_Req_Join_Friend(buf);
			break;
		case 521:
			Zc_Add_Friends_List(buf);
			break;
		case 522:
			Zc_Req_Delete_Friend(buf);
			break;
		case 518:
			Zc_Friends_State(buf);
			break;
		case 517:
			Zc_Divorce(buf);
			break;
		case 528:
			Zc_Ack_PvpPoint(buf);
			break;
		case 543:
			Zc_Notify_PKinfo(buf);
			break;
		case 544:
			Zc_Notify_CrazyKiller(buf);
			break;
		case 551:
			Zc_Game_Guard(buf);
			break;
		case 560:
			Zc_Change_Mer_Status(buf);
			break;
		case 558:
			Zc_Property_Homun(buf);
			break;
		case 667:
			Zc_Mer_Init(buf);
			break;
		case 674:
			Zc_Mer_Par_Change(buf);
			break;
		case 759:
			Zc_Item_Fall_Entry2__m4(buf);
			break;
		case 566:
			ZC_KILLER_POINT(buf);
			break;
		case 568:
			ZC_KILLER_RANK(buf);
			break;
		case 634:
			Zc_HuntingList(buf);
			break;
		case 635:
			Zc_Pcbang_Effect(buf);
			break;
		case 643:
			Zc_Aid(buf);
			break;
		case 576:
			Zc_Mail_Req_Get_List(buf);
			break;
		case 578:
			Zc_Mail_Req_Open(buf);
			break;
		case 581:
			Zc_Mail_Req_Get_Item(buf);
			break;
		case 585:
			Zc_Mail_Req_Send(buf);
			break;
		case 586:
			Zc_Mail_Receive(buf);
			break;
		case 597:
			Zc_Ack_Mail_Add_Item(buf);
			break;
		case 599:
			Zc_Ack_Mail_Delete(buf);
			break;
		case 608:
			Zc_Mail_Windows(buf);
			break;
		case 628:
			Zc_Ack_Mail_Return(buf);
			break;
		case 598:
			Zc_Ack_Auction_Add_Item(buf);
			break;
		case 592:
			Zc_Auction_Result(buf);
			break;
		case 594:
			Zc_Auction_Item_Req_Search(buf);
			break;
		case 605:
			Zc_Auction_My_Sell_Stop(buf);
			break;
		case 607:
			Zc_Auction_Windows(buf);
			break;
		case 647:
			Zc_Pc_Cash_Point_ItemList(buf);
			break;
		case 649:
			Zc_Pc_Cash_Point_Update(buf);
			break;
		case 659:
			Zc_Boss_Info(buf);
			break;
		case 657:
			Zc_Msg(buf);
			break;
		case 660:
			Zc_Read_Book(buf);
			break;
		case 661:
			Zc_Equipment_Itemlist2(buf);
			break;
		case 662:
			Zc_Store_Equipment_Itemlist2(buf);
			break;
		case 663:
			Zc_Cart_Equipment_Itemlist2(buf);
			break;
		case 664:
			Zc_Cash_Time_Counter(buf);
			break;
		case 665:
			Zc_Cash_Item_Delete(buf);
			break;
		case 666:
			Zc_Item_Pickup_Ack2(buf);
			break;
		case 720:
			Zc_Equipment_Itemlist3(buf);
			break;
		case 721:
			Zc_Store_Equipment_Itemlist3(buf);
			break;
		case 722:
			Zc_Cart_Equipment_Itemlist3(buf);
			break;
		case 724:
			Zc_Item_Pickup_Ack3(buf);
			break;
		case 757:
			Zc_Item_Fall_Entry2__m2(buf);
			break;
		case 696:
			Zc_Item_Pickup_Party(buf);
			break;
		case 699:
			Zc_EquipItem_Damaged(buf);
			break;
		case 697:
			Zc_ShortCut_Key_List(buf);
			break;
		case 705:
			Zc_Npc_Chat(buf);
			break;
		case 706:
			Zc_Formatstring_Msg(buf);
			break;
		case 713:
			Zc_Party_Config(buf);
			break;
		case 715:
			Zc_MemorialDungeon_Subscription_Info(buf);
			break;
		case 716:
			Zc_MemorialDungeon_Subscription_Notify(buf);
			break;
		case 717:
			Zc_MemorialDungeon_Info(buf);
			break;
		case 718:
			Zc_MemorialDungeon_Notify(buf);
			break;
		case 725:
			Zc_Isvr_Disconnect(buf);
			break;
		case 727:
			Zc_EquipwinMicroscope(buf);
			break;
		case 729:
			Zc_Config(buf);
			break;
		case 730:
			Zc_Config_Notify(buf);
			break;
		case 743:
			Zc_Mapproperty(buf);
			break;
		case 755:
			Zc_Item_Fall_Entry2(buf);
			break;
		case 768:
			Zc_Notify_Moveentry5__m6(buf);
			break;
		case 769:
			Zc_Notify_Npc_Standentry(buf);
			break;
		case 776:
			{
				PACKET_CZ_REASSEMBLY_IDENTITY p = {0};
				p.PacketType = 818;
				p.PacketLength = sizeof(PACKET_CZ_REASSEMBLY_IDENTITY);
				p.bImportAddressTableModify = !CRagConnection::instanceR().CheckModulePt();
				CRagConnection::instanceR().SendPacket(p.PacketLength, (char *)&p);
			}
			break;
		default:
			CRagConnection::instanceR().ResetQueue(); RAG_BUDGET_SITES(14)
			break;
		}
	}
}



void	C3dAttr :: SetCellInfo(int Type, int Cellx, int Celly)
{
	CAttrCell * cell = GetCell(Cellx, Celly);
	cell->flag = Type;
}

void CGameMode::Inner_Zc_Notify_Npc_Standentry(const unsigned char in_objecttype, const unsigned long in_GID, const short in_speed,
	const short in_bodyState, const short in_healthState, const short in_effectState, const short in_job, const short in_head,
	const short in_weapon, const short in_accessory, const short in_shield, const short in_accessory2, const short in_accessory3,
	const short in_headpalette, const short in_bodypalette, const short in_headDir, const unsigned long in_GUID,
	const short in_GEmblemVer, const short in_honor, const short in_virtue, const bool in_isPKModeON, const unsigned char in_sex,
	const unsigned char *in_PosDir, const unsigned char in_xSize, const unsigned char in_ySize, const unsigned char in_state,
	const short in_clevel)
{
	STANDENTRYSTRUCT s;

	s.job = in_job;
	s.sex = in_sex;
	s.GID = in_GID;
	s.speed = in_speed;
	s.xSize = in_xSize;
	s.ySize = in_ySize;
	s.head = in_head;
	s.weapon = in_weapon;
	s.accessory = in_accessory;
	s.state = in_state;
	s.healthState = in_healthState;
	s.bodyState = in_bodyState;
	s.effectState = in_effectState;
	s.isPKModeON = in_isPKModeON;
	s.shield = in_shield;
	s.accessory2 = in_accessory2;
	s.accessory3 = in_accessory3;
	s.headpalette = in_headpalette;
	s.bodypalette = in_bodypalette;
	s.headDir = in_headDir;
	s.guildId = in_GUID;
	s.guildEmblemVersion = in_GEmblemVer;
	s.honor = in_honor;
	s.virtue = in_virtue;

	s.x = in_PosDir[0] & 0xff;
	s.x = (s.x << 2) | ((in_PosDir[1] & 0xc0) >> 6);
	s.y = in_PosDir[1] & 0x3f;
	s.y = (s.y << 4) | ((in_PosDir[2] & 0xf0) >> 4);
	s.dir = in_PosDir[2] & 0xf;

	CGameActor *newGameActor = GetWorld()->GetGameActorByAID(in_GID);
	if (newGameActor) {
		if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
			DWORD gdid = (DWORD)(in_accessory2 << 16);
			gdid |= in_accessory3;
			if (gdid > 0 && newGameActor->GetGdid() == 0)
				newGameActor->SetGuildInfo(gdid, in_shield);
		}

		if (s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == in_GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		newGameActor->SendMsg(NULL, AM_STANDENTRY, (int)(&s), 0, 0);

		if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, in_GUID, in_GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, in_GUID, in_GEmblemVer);
			}
		}
	} else {
		if ((in_job >= JT_WARPNPC && in_job < JT_2004_JOB_BEGIN) || in_objecttype == NPC_MERSOL_TYPE || g_session.IsMerOrHomun(in_job)) {
			if (g_session.m_homunGID == in_GID)
				g_session.m_homunAlive = true;
			if (g_session.m_merGID == in_GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(in_job);
			s.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && s.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == in_GID)
				g_session.m_petAcc = s.accessory;

			if (s.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(s.accessory));
		}

		if (in_objecttype == NPC_MERSOL_TYPE)
			newGameActor->m_objectType = in_objecttype;

		m_world->m_gameObjectList.push_back(newGameActor);
		m_world->m_actorList.push_back(newGameActor);

		if (newGameActor) {
			newGameActor->SendMsg(NULL, AM_INITSTANDENTRY, (int)(&s), 0, 0);

			if (g_session.IsSiegeMode() && s.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
				if (!g_session.IsPc(newGameActor->GetJob())) {
					if (g_session.IsGuardian(newGameActor->GetJob()))
						MakeEmblemWnd(newGameActor, in_GUID, in_GEmblemVer);
				} else {
					MakeEmblemWnd(newGameActor, in_GUID, in_GEmblemVer);
				}
			}

			if (newGameActor->GetJob() > JT_WARPNPC && newGameActor->GetJob() < JT_2004_JOB_BEGIN) {
				DWORD gdid = (DWORD)(in_accessory2 << 16);
				gdid |= in_accessory3;
				if (gdid > 0)
					newGameActor->SetGuildInfo(gdid, in_shield);
			}
		}
	}

	GameModeShowEFST(in_GID, in_effectState, in_clevel, in_virtue);
}

void CGameMode::Zc_Notify_Npc_Standentry(const char *buf)
{
	PACKET_ZC_NOTIFY_NPC_STANDENTRY *	p = (PACKET_ZC_NOTIFY_NPC_STANDENTRY *)buf;
	*(unsigned short *)p->PosDir ^= (unsigned short)p->GID;
	Inner_Zc_Notify_Npc_Standentry(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->shield, p->accessory2,
		p->accessory3, p->headpalette, p->bodypalette, p->headDir, p->GUID, p->GEmblemVer,
		p->honor, p->virtue, p->isPKModeON, p->sex, p->PosDir, p->xSize, p->ySize,
		p->state, p->clevel);
}

void CGameMode::Inner_Zc_Item_Fall_Entry2(const unsigned long in_ITAID, const unsigned short in_ITID, const bool in_IsIdentified, const short in_count, const short in_xPos, const short in_yPos, const unsigned char in_subX, const unsigned char in_subY)
{
	ITEMSTANDENTRYSTRUCT s;
	memset(&s, 0, sizeof(s));
	s.count = in_count;
	s.ITAID = in_ITAID;
	s.x = in_xPos;
	s.y = in_yPos;
	s.subX = in_subX;
	s.subY = in_subY;
	itoa(in_ITID, s.name, 10);
	s.isJumpEntry = true;
	s.isIdentified = in_IsIdentified;

	CItem *newItem = m_world->GetItemByAID(in_ITAID);
	if (newItem == NULL)
	{
		newItem = new CItem;
		GetWorld()->m_gameObjectList.push_back(newItem);
		GetWorld()->m_itemList.push_back(newItem);
	}
	newItem->SendMsg(NULL, AM_INITSTANDENTRY, (int)(&s), 0, 0);
}


void	CGameMode :: Zc_Item_Fall_Entry2(const char *buf)
{
	PACKET_ZC_ITEM_FALL_ENTRY2 *p = (PACKET_ZC_ITEM_FALL_ENTRY2 *)buf;
	((unsigned char*)p)[5] ^= ((unsigned char*)p)[13];
	((unsigned char*)p)[7] ^= ((unsigned char*)p)[8];
	Inner_Zc_Item_Fall_Entry2(p->ITAID, p->ITID, p->IsIdentified, p->count, p->xPos, p->yPos, p->subX, p->subY);
}
void	CGameMode :: Zc_Item_Fall_Entry2__m2(const char *buf)
{
	PACKET_ZC_ITEM_FALL_ENTRY2 *p = (PACKET_ZC_ITEM_FALL_ENTRY2 *)buf;
	((unsigned char*)p)[5] ^= ((unsigned char*)p)[13];
	((unsigned char*)p)[7] ^= ((unsigned char*)p)[8];
	Inner_Zc_Item_Fall_Entry2(p->ITAID, p->ITID, p->IsIdentified, p->count, p->xPos, p->yPos, p->subX, p->subY);
}
void	CGameMode :: Zc_Item_Fall_Entry2__m4(const char *buf)
{
	PACKET_ZC_ITEM_FALL_ENTRY2 *p = (PACKET_ZC_ITEM_FALL_ENTRY2 *)buf;
	((unsigned char*)p)[5] ^= ((unsigned char*)p)[13];
	((unsigned char*)p)[7] ^= ((unsigned char*)p)[8];
	Inner_Zc_Item_Fall_Entry2(p->ITAID, p->ITID, p->IsIdentified, p->count, p->xPos, p->yPos, p->subX, p->subY);
}
void	CGameMode :: Zc_Item_Fall_Entry2__m5(const char *buf)
{
	PACKET_ZC_ITEM_FALL_ENTRY2 *p = (PACKET_ZC_ITEM_FALL_ENTRY2 *)buf;
	((unsigned char*)p)[5] ^= ((unsigned char*)p)[13];
	((unsigned char*)p)[7] ^= ((unsigned char*)p)[8];
	Inner_Zc_Item_Fall_Entry2(p->ITAID, p->ITID, p->IsIdentified, p->count, p->xPos, p->yPos, p->subX, p->subY);
}

void CGameMode::Inner_Zc_Notify_Moveentry5(const unsigned char in_objecttype, const unsigned long in_GID, const short in_speed,
	const short in_bodyState, const short in_healthState, const int in_effectState, const short in_job, const short in_head,
	const int in_weapon, const short in_accessory, const unsigned long in_moveStartTime, const short in_accessory2,
	const short in_accessory3, const short in_headpalette, const short in_bodypalette, const short in_headDir,
	const unsigned long in_GUID, const short in_GEmblemVer, const short in_honor, const int in_virtue,
	const bool in_isPKModeON, const unsigned char in_sex, const unsigned char *in_pMoveData, const unsigned char in_xSize,
	const unsigned char in_ySize, const short in_clevel)
{
	MOVEENTRYSTRUCT ms;
	ms.GID = in_GID;
	ms.job = in_job;
	ms.moveStartTime = in_moveStartTime;
	ms.sex = in_sex;
	ms.speed = in_speed;
	ms.head = in_head;
	ms.weapon = in_weapon;
	ms.accessory = in_accessory;
	ms.healthState = in_healthState;
	ms.bodyState = in_bodyState;
	ms.effectState = in_effectState;
	ms.isPKModeON = in_isPKModeON;
	ms.accessory2 = in_accessory2;
	ms.accessory3 = in_accessory3;
	ms.headpalette = in_headpalette;
	ms.bodypalette = in_bodypalette;
	ms.headDir = in_headDir;
	ms.guildId = in_GUID;
	ms.guildEmblemVersion = in_GEmblemVer;
	ms.honor = in_honor;
	ms.virtue = in_virtue;

	DecodeSrcDst((BYTE *)in_pMoveData, ms.sx, ms.sy, ms.dx, ms.dy, ms.cellX, ms.cellY);
	g_session.UpdateServerTime(ms.moveStartTime);

	CGameActor *theActor = GetWorld()->GetGameActorByAID(in_GID);
	if (theActor) {
		theActor->SendMsg(NULL, AM_MOVEENTRY2, (int)(&ms), 0, 0);
		theActor->m_clevel = in_clevel;

		if (ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == in_GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0 && theActor->GetGdid() == 0)
				theActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !theActor->m_emblemWnd) {
			if (!g_session.IsPc(theActor->GetJob())) {
				if (g_session.IsGuardian(theActor->GetJob()))
					MakeEmblemWnd(theActor, in_GUID, in_GEmblemVer);
			} else {
				MakeEmblemWnd(theActor, in_GUID, in_GEmblemVer);
			}
		}

		if (theActor->GetGroupMemberAndVisible()) {
			std::map<unsigned long, ColorCellPos>::iterator it = m_partyPosList.find(theActor->GetGID());
			if (it != m_partyPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
			it = m_guildPosList.find(theActor->GetGID());
			if (it != m_guildPosList.end()) {
				it->second.x = ms.dx;
				it->second.y = ms.dy;
			}
		}
	} else {
		CGameActor *newGameActor;
		if ((in_job >= JT_WARPNPC && in_job < JT_2004_JOB_BEGIN) || in_objecttype == NPC_MERSOL_TYPE || g_session.IsMerOrHomun(in_job)) {
			if (g_session.m_homunGID == in_GID)
				g_session.m_homunAlive = true;
			if (g_session.m_merGID == in_GID)
				g_session.m_merAlive = true;

			newGameActor = NewNpc(in_job);
			ms.sex = SEX_FEMALE;
		} else {
			newGameActor = new CPc;
		}

		if (newGameActor && ms.head == HEADTYPE_LAST) {
			if (g_session.m_petGID == in_GID)
				g_session.m_petAcc = ms.accessory;

			if (ms.accessory != 0)
				newGameActor->SetActName(g_session.GetPetAccActName(ms.accessory));
		}

		if (in_objecttype == NPC_MERSOL_TYPE)
			newGameActor->m_objectType = in_objecttype;

		m_world->m_gameObjectList.push_back(newGameActor);
		m_world->m_actorList.push_back(newGameActor);

		newGameActor->SendMsg(NULL, AM_INITMOVEENTRY2, (int)(&ms), 0, 0);
		newGameActor->m_clevel = in_clevel;

		if (g_session.IsSiegeMode() && ms.head != HEADTYPE_LAST && !newGameActor->m_emblemWnd) {
			if (!g_session.IsPc(newGameActor->GetJob())) {
				if (g_session.IsGuardian(newGameActor->GetJob()))
					MakeEmblemWnd(newGameActor, in_GUID, in_GEmblemVer);
			} else {
				MakeEmblemWnd(newGameActor, in_GUID, in_GEmblemVer);
			}
		}

		if (newGameActor && (m_partyPosList.find(ms.GID) != m_partyPosList.end() ||
			m_guildPosList.find(ms.GID) != m_guildPosList.end())) {
			newGameActor->SetGroupMemberAndVisible(TRUE);
		}
	}

	GameModeShowEFST(in_GID, in_effectState, in_clevel, in_virtue);
}

void CGameMode::Zc_Notify_Moveentry5__m4(const char *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY5 *	p = (PACKET_ZC_NOTIFY_MOVEENTRY5 *)buf;
	*(unsigned long *)p->moveData ^= p->GID;
	Inner_Zc_Notify_Moveentry5(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->moveStartTime,
		p->accessory2, p->accessory3, p->headpalette, p->bodypalette, p->headDir,
		p->GUID, p->GEmblemVer, p->honor, p->virtue, p->isPKModeON, p->sex,
		p->moveData, p->xSize, p->ySize, p->clevel);
}

void	CGameMode :: Zc_Notify_Moveentry5__m6(const char *buf)
{
	PACKET_ZC_NOTIFY_MOVEENTRY5 *	p = (PACKET_ZC_NOTIFY_MOVEENTRY5 *)buf;
	*(unsigned long *)p->moveData ^= p->GID;
	Inner_Zc_Notify_Moveentry5(p->objectType, p->GID, p->speed, p->bodyState, p->healthState,
		p->effectState, p->job, p->head, p->weapon, p->accessory, p->moveStartTime,
		p->accessory2, p->accessory3, p->headpalette, p->bodypalette, p->headDir,
		p->GUID, p->GEmblemVer, p->honor, p->virtue, p->isPKModeON, p->sex,
		p->moveData, p->xSize, p->ySize, p->clevel);
}




void CGameMode::Zc_BroadCast_HP(int message)
{
	m_broadCastTick = timeGetTime() - 10000;
	if (m_broadcastBalloon == NULL)
	{
		m_broadcastBalloon = new UITransBalloonText;
		m_broadcastBalloon->Create(8, 8);
		g_windowMgr.AddWindowFront(m_broadcastBalloon);
	}

	m_broadcastBalloon->SetText((const char*)message, 72);
	m_broadcastBalloon->SetFntSize(16);
	m_broadcastBalloon->Move((g_renderer->m_width - 640) / 2 - m_broadcastBalloon->GetWidth() / 2 + 320, 51);
	m_broadcastBalloon->SetFntColor(RGB(255, 200, 200), RGB(0, 0, 0));
}
