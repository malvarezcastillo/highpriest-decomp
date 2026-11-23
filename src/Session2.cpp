#include "common.h"
#include "Session.h"
#include "window/UIFrameWnd.h"
#include "window/UIEmotionWnd.h"
#include "MsgStrings.h"
#include "framework/Locale.h"
#include "framework/Language.h"
#include "RagEffect.h"
#include "GameMode.h"
#include "GameWorld.h"
#include "resource/Wave.h"
#include "pdb_enums.h"
#include "Mode.h"
#include "GameActor.h"
#include "base/File.h"
#include "resource/Attr.h"
#include "resource/Ground.h"
#include "pdb_shared.h"


extern UIWindowMgr g_windowMgr;
extern CSession g_session;
extern std::list<HP_Damage_Moment> g_DM_List;
extern ServiceType g_serviceType;
extern char* DataTxtDecode(char* str);

extern CModeMgr g_modeMgr;
const char* szCharNameEcryption = "\xb0\xa1\xb3\xaa\xb4\xd9\xb6\xf3\xb8\xb6\xb9\xd9\xbb\xe7\xbe\xc6\xc0\xda\xc2\xf7\xc4\xab\xc5\xb8\xc6\xc4\xc7\xcf\xb0\xa1\xb3\xaa\xb4\xd9\xb6\xf3\xb8\xb6\xb9\xd9\xbb\xe7\xbe\xc6\xc0\xda\xc2\xf7\xc4\xab\xc5\xb8\xc6\xc4\xc7\xcf";
extern const char* g_regPath;
extern char* g_emotionPath;
extern const char* MsgStr(MSGSTRINGID id);


#ifndef NUM_SHORTCUT_SLOT
#define NUM_SHORTCUT_SLOT 27
#endif

#ifndef ONE_SLOTCOUNT
#define ONE_SLOTCOUNT 9
#endif

#ifndef _PACKET_VER
#define _PACKET_VER 5
#endif

#ifndef MAX_EMOTION_TEXT
#define MAX_EMOTION_TEXT 10
#endif

static const int SESSION_SKID_LAST = 1039;
static const int EF_BEGINSPELLWHITE = 1013;


#define SAVE_FOOD_PARAM

const char* CSession::GetCharName()
{
	static char cName[sizeof(m_cName)];
	memcpy(cName, m_cName, sizeof(m_cName));

	for(int i=0; i<sizeof(m_cName); ++i) cName[i] ^= szCharNameEcryption[i];

	return cName;
}

void CSession::SetCharName(const char* cName)
{
	memcpy(m_cName, cName, sizeof(m_cName));

	for(int i=0; i<sizeof(m_cName); ++i) m_cName[i] ^= szCharNameEcryption[i];
}


void CSession::SetShortenSkillUseLevel(int skillUseLevel, int slot)
{
	sAssert(slot>=0 && slot<=NUM_SHORTCUT_SLOT-1);
	m_shortenSkillUseLevel[slot] = skillUseLevel;
}

void CSession::SetSex(int sex)
{
	m_sex = sex & 0x01;
	m_sexCheck = m_sex;
}

int CSession::GetSex()
{
	if(m_sex != m_sexCheck) m_sex = m_sexCheck;

	return m_sex;
}

void CSession::ClearShortenSkillUseLevel()
{
	for (int i=0; i<NUM_SHORTCUT_SLOT; i++)
		m_shortenSkillUseLevel[i] = 0;
}

int CSession::GetShortenSkillUseLevel(int slot)
{
	sAssert(slot>=0 && slot<=NUM_SHORTCUT_SLOT-1);
	return	m_shortenSkillUseLevel[slot];
}

void	CSession :: SetShortenSkillName(const char *shortenSkillName, int skillUseLevel, int slot)
{
	sAssert(slot>=0 && slot<=NUM_SHORTCUT_SLOT-1);
	for (int i = m_shortcutSlotCnt * ONE_SLOTCOUNT; i< (m_shortcutSlotCnt + 1) * ONE_SLOTCOUNT; ++i) {
		if ((m_shortenItemName[i]==shortenSkillName) && (m_shortenSkillUseLevel[i]==skillUseLevel)) {
			m_shortenItemName[i] = "nothing";
			m_shortenSkillUseLevel[i] = 0;
		}
	}
	if (strlen(shortenSkillName)==0) {
		m_shortenItemName[slot] = "nothing";
		m_shortenSkillUseLevel[slot] = 0;
	} else {
		m_shortenItemName[slot] = shortenSkillName;
		m_shortenSkillUseLevel[slot] = skillUseLevel;
	}


	sAssert(m_shortenItemName[slot].size());
}

SKILL_INFO	CSession :: GetSkillItemInfoBySkillId(int skillId)
{
	if (m_skillInfoList.size() == 0)
		return	SKILL_INFO(-1);
	std::list<SKILL_INFO>::iterator	it = m_skillInfoList.begin();
	while (it != m_skillInfoList.end()) {
		SKILL_INFO &	skillInfo = (*it);
		if (skillInfo.SKID == skillId)
			return	*it;
		++it;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetSkillItemInfoByName(const char *skillName)
{
	if (m_skillInfoList.size() == 0)
		return	SKILL_INFO(-1);
	std::list<SKILL_INFO>::iterator it = m_skillInfoList.begin();
	while (it != m_skillInfoList.end()) {
		SKILL_INFO &	skillInfo = (*it);
		if (strcmp(skillInfo.skillName,skillName) == 0)
			return	(*it);
		++it;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetGuildSkillItemInfoBySequence(int sequence)
{
	if (m_guildSkillList.size() == 0)
		return	SKILL_INFO(-1);
	int		count = 0;
	std::list<SKILL_INFO>::iterator	it = m_guildSkillList.begin();
	while (it != m_guildSkillList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetSkillItemInfoBySequence(int sequence)
{
	if (m_skillInfoList.size() == 0)
		return	SKILL_INFO(-1);
	int		count = 0;
	std::list<SKILL_INFO>::iterator	it = m_skillInfoList.begin();
	while (it != m_skillInfoList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetHomunSkillItemInfoBySkillId(int skillId)
{
	if (m_homunSkillList.size() == 0)
		return	SKILL_INFO(-1);
	std::list<SKILL_INFO>::iterator	it = m_homunSkillList.begin();
	while (it != m_homunSkillList.end()) {
		SKILL_INFO &	skillInfo = (*it);
		if (skillInfo.SKID == skillId)
			return	*it;
		++it;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetHomunSkillItemInfoByName(const char *skillName)
{
	if (m_homunSkillList.size() == 0)
		return	SKILL_INFO(-1);
	std::list<SKILL_INFO>::iterator it = m_homunSkillList.begin();
	while (it != m_homunSkillList.end()) {
		SKILL_INFO &	skillInfo = (*it);
		if (strcmp(skillInfo.skillName,skillName) == 0)
			return	(*it);
		++it;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetHomunSkillItemInfoBySequence(int sequence)
{
	if (m_homunSkillList.size() == 0)
		return	SKILL_INFO(-1);
	int		count = 0;
	std::list<SKILL_INFO>::iterator	it = m_homunSkillList.begin();
	while (it != m_homunSkillList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	SKILL_INFO(-1);
}

void CSession::RefreshHomunSkillItemWnd()
{
	UISkillListWnd *skillWnd = g_windowMgr.GetHomunSkillListWnd();
	if (skillWnd)
		skillWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
	UIShortCutWnd *shortCutWnd = g_windowMgr.GetShortCutWnd();
	if (shortCutWnd)
		shortCutWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

bool	CSession :: AddHomunSkillItem(SKILL_INFO &skillInfo, bool needRefresh)
{
	std::list<SKILL_INFO>::iterator it = m_homunSkillList.begin();
	while (it != m_homunSkillList.end()) {
		SKILL_INFO &	curSkillInfo = (*it);
		if (skillInfo.SKID == curSkillInfo.SKID) {
			curSkillInfo.level			= skillInfo.level;
			curSkillInfo.spcost			= skillInfo.spcost;
			curSkillInfo.attackRange	= skillInfo.attackRange;
			curSkillInfo.upgradable		= skillInfo.upgradable;

			if (needRefresh)
			{
				RefreshHomunSkillItemWnd();
			}
			return	false;
		}
		it++;
	}

#ifdef _DEBUG
	switch (skillInfo.type) {
			case	SKILLTYPE_PASSIVE:
			case	SKILLTYPE_TOSKILL:
			case	SKILLTYPE_TOCHARACTER:
			case	SKILLTYPE_TOGROUND:
			case	SKILLTYPE_TOME:
			case	SKILLTYPE_TOITEM:
			case	SKILLTYPE_TOALL:
				break;
			default:
				sAssert(false);
				break;
	}
#endif

	m_homunSkillList.push_back(skillInfo);
	if (needRefresh)
	{
		RefreshHomunSkillItemWnd();
	}
	return	true;
}

SKILL_INFO	CSession :: GetMerSkillItemInfoBySkillId(int skillId)
{
	if (m_merSkillList.size() == 0)
		return	SKILL_INFO(-1);
	std::list<SKILL_INFO>::iterator	it = m_merSkillList.begin();
	while (it != m_merSkillList.end()) {
		SKILL_INFO &	skillInfo = (*it);
		if (skillInfo.SKID == skillId)
			return	*it;
		++it;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetMerSkillItemInfoByName(const char *skillName)
{
	if (m_merSkillList.size() == 0)
		return	SKILL_INFO(-1);
	std::list<SKILL_INFO>::iterator it = m_merSkillList.begin();
	while (it != m_merSkillList.end()) {
		SKILL_INFO &	skillInfo = (*it);
		if (strcmp(skillInfo.skillName,skillName) == 0)
			return	(*it);
		++it;
	}
	return	SKILL_INFO(-1);
}

SKILL_INFO	CSession :: GetMerSkillItemInfoBySequence(int sequence)
{
	if (m_merSkillList.size() == 0)
		return	SKILL_INFO(-1);
	int		count = 0;
	std::list<SKILL_INFO>::iterator	it = m_merSkillList.begin();
	while (it != m_merSkillList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	SKILL_INFO(-1);
}

void CSession::RefreshMerSkillItemWnd()
{
	UISkillListWnd *skillWnd = g_windowMgr.GetMerSkillListWnd();
	if (skillWnd)
		skillWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
	UIShortCutWnd *shortCutWnd = g_windowMgr.GetShortCutWnd();
	if (shortCutWnd)
		shortCutWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

bool	CSession :: AddMerSkillItem(SKILL_INFO &skillInfo, bool needRefresh)
{
	std::list<SKILL_INFO>::iterator it = m_merSkillList.begin();
	while (it != m_merSkillList.end()) {
		SKILL_INFO &	curSkillInfo = (*it);
		if (skillInfo.SKID == curSkillInfo.SKID) {
			curSkillInfo.level			= skillInfo.level;
			curSkillInfo.spcost			= skillInfo.spcost;
			curSkillInfo.attackRange	= skillInfo.attackRange;
			curSkillInfo.upgradable		= skillInfo.upgradable;

			if (needRefresh)
			{
				RefreshMerSkillItemWnd();
			}
			return	false;
		}
		it++;
	}

#ifdef _DEBUG
	switch (skillInfo.type) {
			case	SKILLTYPE_PASSIVE:
			case	SKILLTYPE_TOSKILL:
			case	SKILLTYPE_TOCHARACTER:
			case	SKILLTYPE_TOGROUND:
			case	SKILLTYPE_TOME:
			case	SKILLTYPE_TOITEM:
			case	SKILLTYPE_TOALL:
				break;
			default:
				sAssert(false);
				break;
	}
#endif

	m_merSkillList.push_back(skillInfo);
	if (needRefresh)
	{
		RefreshMerSkillItemWnd();
	}
	return	true;
}

void CSession::RefreshSkillItemWnd()
{
	UISkillListWnd *skillWnd = g_windowMgr.GetSkillListWnd();
	if (skillWnd)
		skillWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
	UIShortCutWnd *shortCutWnd = g_windowMgr.GetShortCutWnd();
	if (shortCutWnd)
		shortCutWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}


void CSession::RefreshQuestWnd()
{
	UIQuestWnd *questWnd = g_windowMgr.GetQuestWnd();
	if (questWnd)
		questWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

void CSession::RefreshQuestDetailWnd()
{
	UIQuestDetailWnd *questDetailWnd = g_windowMgr.GetQuestDetailWnd();
	if (questDetailWnd)
		questDetailWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

bool	CSession :: AddGuildSkillItem(SKILL_INFO &skillInfo, bool needRefresh)
{
	Trace("%s %d", skillInfo.skillName, skillInfo.upgradable);


	std::list<SKILL_INFO>::iterator it = m_guildSkillList.begin();
	while (it != m_guildSkillList.end()) {
		SKILL_INFO &	curSkillInfo = (*it);
		if (skillInfo.SKID == curSkillInfo.SKID) {
			curSkillInfo.level			= skillInfo.level;
			curSkillInfo.spcost			= skillInfo.spcost;
			curSkillInfo.attackRange	= skillInfo.attackRange;
			curSkillInfo.upgradable		= skillInfo.upgradable;


			return	false;
		}
		it++;
	}

#ifdef _DEBUG
	switch (skillInfo.type) {
			case	SKILLTYPE_PASSIVE:
			case	SKILLTYPE_TOSKILL:
			case	SKILLTYPE_TOCHARACTER:
			case	SKILLTYPE_TOGROUND:
			case	SKILLTYPE_TOME:
			case	SKILLTYPE_TOITEM:
			case	SKILLTYPE_TOALL:
				break;
			default:
				sAssert(false);
				break;
	}
#endif

	m_guildSkillList.push_back(skillInfo);


	return	true;
}

bool	CSession :: AddSkillItem(SKILL_INFO &skillInfo, bool needRefresh)
{
	Trace("%s %d", skillInfo.skillName, skillInfo.upgradable);

#if defined(NEW_SKILLWND)
	AddSkillToList ( skillInfo ) ;
#endif

	std::list<SKILL_INFO>::iterator it = m_skillInfoList.begin();
	while (it != m_skillInfoList.end()) {
		SKILL_INFO &	curSkillInfo = (*it);
		if (skillInfo.SKID == curSkillInfo.SKID) {
			curSkillInfo.level			= skillInfo.level;
			curSkillInfo.spcost			= skillInfo.spcost;
			curSkillInfo.attackRange	= skillInfo.attackRange;
			curSkillInfo.upgradable		= skillInfo.upgradable;

			if (needRefresh)
			{
				RefreshSkillItemWnd();
			}
			return	false;
		}
		it++;
	}

#ifdef _DEBUG
	switch (skillInfo.type) {
			case	SKILLTYPE_PASSIVE:
			case	SKILLTYPE_TOSKILL:
			case	SKILLTYPE_TOCHARACTER:
			case	SKILLTYPE_TOGROUND:
			case	SKILLTYPE_TOME:
			case	SKILLTYPE_TOITEM:
			case	SKILLTYPE_TOALL:
				break;
			default:
				sAssert(false);
				break;
	}
#endif

	m_skillInfoList.push_back(skillInfo);
	if (needRefresh)
	{
		RefreshSkillItemWnd();
	}

	return	true;
}

bool	CSession :: AddQuestList(QUEST_INFO &QuestInfo, bool needRefresh)
{
	std::list<QUEST_INFO>::iterator it = m_QuestList.begin();
	while (it != m_QuestList.end()) {
		QUEST_INFO &curQuestInfo = (*it);
		if (QuestInfo.questID == curQuestInfo.questID) {
			curQuestInfo.show = QuestInfo.show;
			return false;
		}
		it++;
	}
	m_QuestList.push_back(QuestInfo);
	return true;
}

void	CSession :: InitSkillEffectIdTable()
{
	m_skillAttackEffectIdTable.clear();
	m_skillAttackEffectIdTable.resize(SESSION_SKID_LAST, -1);

	m_skillAttackEffectIdTable[0]						= -1;
	m_skillAttackEffectIdTable[SKID_NV_BASIC]			= -1;
	m_skillAttackEffectIdTable[SKID_SM_SWORD]			= -1;
	m_skillAttackEffectIdTable[SKID_SM_TWOHAND]			= -1;
	m_skillAttackEffectIdTable[SKID_SM_RECOVERY]		= -1;
	m_skillAttackEffectIdTable[SKID_SM_BASH]			= -1;
	m_skillAttackEffectIdTable[SKID_SM_PROVOKE]			= -1;

	m_skillAttackEffectIdTable[SKID_SM_MAGNUM]			= EF_BASH;

	m_skillAttackEffectIdTable[SKID_SM_ENDURE]			= -1;
	m_skillAttackEffectIdTable[SKID_MG_SRECOVERY]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_SIGHT]			= -1;

	m_skillAttackEffectIdTable[SKID_MG_SAFETYWALL]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_SOULSTRIKE]		= EF_SOULSTRIKE;
#if defined(MOB_SKILL_PATCH)
	m_skillAttackEffectIdTable[SKID_NPC_DARKSTRIKE]		= EF_SOULSTRIKE2;
#endif

	m_skillAttackEffectIdTable[SKID_MG_COLDBOLT]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_FROSTDIVER]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_STONECURSE]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_FIREBALL]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_FIREWALL]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_FIREBOLT]		= -1;
	m_skillAttackEffectIdTable[SKID_MG_LIGHTNINGBOLT]	= -1;
	m_skillAttackEffectIdTable[SKID_MG_THUNDERSTORM]	= -1;
	m_skillAttackEffectIdTable[SKID_AL_DP]				= -1;
	m_skillAttackEffectIdTable[SKID_AL_DEMONBANE]		= -1;
	m_skillAttackEffectIdTable[SKID_AL_RUWACH]			= -1;
	m_skillAttackEffectIdTable[SKID_AL_PNEUMA]			= -1;
	m_skillAttackEffectIdTable[SKID_AL_TELEPORT]		= -1;
	m_skillAttackEffectIdTable[SKID_AL_WARP]			= -1;
	m_skillAttackEffectIdTable[SKID_AL_HEAL]			= -1;

	m_skillAttackEffectIdTable[SKID_AL_INCAGI]			= -1;
	m_skillAttackEffectIdTable[SKID_CASH_INCAGI]		= -1;

	m_skillAttackEffectIdTable[SKID_AL_DECAGI]			= -1;
	m_skillAttackEffectIdTable[SKID_AL_HOLYWATER]		= -1;
	m_skillAttackEffectIdTable[SKID_AL_CRUCIS]			= -1;
	m_skillAttackEffectIdTable[SKID_AL_ANGELUS]			= -1;

	m_skillAttackEffectIdTable[SKID_AL_BLESSING]		= -1;
	m_skillAttackEffectIdTable[SKID_CASH_BLESSING]		= -1;

	m_skillAttackEffectIdTable[SKID_AL_CURE]			= -1;
	m_skillAttackEffectIdTable[SKID_MC_INCCARRY]		= -1;
	m_skillAttackEffectIdTable[SKID_MC_DISCOUNT]		= -1;
	m_skillAttackEffectIdTable[SKID_MC_OVERCHARGE]		= -1;
	m_skillAttackEffectIdTable[SKID_MC_PUSHCART]		= -1;
	m_skillAttackEffectIdTable[SKID_MC_IDENTIFY]		= -1;
	m_skillAttackEffectIdTable[SKID_MC_VENDING]			= -1;
	m_skillAttackEffectIdTable[SKID_MC_MAMMONITE]		= -1;
	m_skillAttackEffectIdTable[SKID_AC_OWL]				= -1;
	m_skillAttackEffectIdTable[SKID_AC_VULTURE]			= -1;
	m_skillAttackEffectIdTable[SKID_AC_CONCENTRATION]	= -1;
	m_skillAttackEffectIdTable[SKID_AC_DOUBLE]			= -1;
	m_skillAttackEffectIdTable[SKID_HT_POWER]			= -1;
	m_skillAttackEffectIdTable[SKID_HT_PHANTASMIC]		= -1;
	m_skillAttackEffectIdTable[SKID_AC_SHOWER]			= -1;
	m_skillAttackEffectIdTable[SKID_TF_DOUBLE]			= -1;
	m_skillAttackEffectIdTable[SKID_TF_MISS]			= -1;
	m_skillAttackEffectIdTable[SKID_TF_STEAL]			= EF_BASH;
	m_skillAttackEffectIdTable[SKID_TF_HIDING]			= EF_BASH;
	m_skillAttackEffectIdTable[SKID_TF_POISON]			= EF_BASH;
	m_skillAttackEffectIdTable[SKID_TF_DETOXIFY]		= EF_BASH;
	m_skillAttackEffectIdTable[SKID_ALL_RESURRECTION]	= -1;
	m_skillAttackEffectIdTable[SKID_KN_SPEARMASTERY]	= -1;
	m_skillAttackEffectIdTable[SKID_KN_PIERCE]			= -1;
	m_skillAttackEffectIdTable[SKID_KN_BRANDISHSPEAR]	= -1;
	m_skillAttackEffectIdTable[SKID_KN_SPEARSTAB]		= -1;
	m_skillAttackEffectIdTable[SKID_KN_SPEARBOOMERANG]	= -1;
	m_skillAttackEffectIdTable[SKID_KN_TWOHANDQUICKEN]	= -1;
	m_skillAttackEffectIdTable[SKID_KN_AUTOCOUNTER]		= -1;
	m_skillAttackEffectIdTable[SKID_KN_BOWLINGBASH]		= -1;
	m_skillAttackEffectIdTable[SKID_KN_RIDING]			= -1;
	m_skillAttackEffectIdTable[SKID_KN_CAVALIERMASTERY]	= -1;
	m_skillAttackEffectIdTable[SKID_PR_MACEMASTERY]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_IMPOSITIO]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_SUFFRAGIUM]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_ASPERSIO]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_BENEDICTIO]		= -1;
	m_skillAttackEffectIdTable[SKID_ALL_ANGEL_PROTECT]		= -1;

	m_skillAttackEffectIdTable[SKID_PR_SANCTUARY]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_STRECOVERY]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_KYRIE]			= -1;
	m_skillAttackEffectIdTable[SKID_PR_MAGNIFICAT]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_GLORIA]			= -1;
	m_skillAttackEffectIdTable[SKID_PR_LEXDIVINA]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_TURNUNDEAD]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_LEXAETERNA]		= -1;
	m_skillAttackEffectIdTable[SKID_PR_MAGNUS]			= -1;
	m_skillAttackEffectIdTable[SKID_WZ_FIREPILLAR]		= EF_FIREPILLAR;
	m_skillAttackEffectIdTable[SKID_WZ_SIGHTRASHER]		= EF_SIGHTRASHER;
	m_skillAttackEffectIdTable[SKID_WZ_METEOR]			= -1;
	m_skillAttackEffectIdTable[SKID_WZ_JUPITEL]			= EF_YUFITEL;
#if defined(MOB_SKILL_PATCH)
	m_skillAttackEffectIdTable[SKID_NPC_DARKTHUNDER]	= EF_YUFITEL;
#endif
	m_skillAttackEffectIdTable[SKID_WZ_VERMILION]		= -1;
	m_skillAttackEffectIdTable[SKID_WZ_FROSTNOVA]		= -1;
	m_skillAttackEffectIdTable[SKID_WZ_STORMGUST]		= -1;
	m_skillAttackEffectIdTable[SKID_NPC_WIDEFREEZE]		= EF_STORMGUST;
	m_skillAttackEffectIdTable[SKID_WZ_EARTHSPIKE]		= -1;
	m_skillAttackEffectIdTable[SKID_WZ_HEAVENDRIVE]		= -1;
	m_skillAttackEffectIdTable[SKID_WZ_QUAGMIRE]		= -1;
	m_skillAttackEffectIdTable[SKID_WZ_ESTIMATION]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_SKIDTRAP]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_LANDMINE]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_ANKLESNARE]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_SHOCKWAVE]		= -1;

	m_skillAttackEffectIdTable[SKID_HT_SANDMAN]			= -1;
	m_skillAttackEffectIdTable[SKID_GM_SANDMAN]			= -1;
	m_skillAttackEffectIdTable[SKID_ALL_CATCRY]			= -1;
	m_skillAttackEffectIdTable[SKID_ALL_PARTYFLEE]		= -1;
	m_skillAttackEffectIdTable[SKID_NPC_PICK_ITEM]		= -1;

	m_skillAttackEffectIdTable[SKID_HT_FLASHER]			= -1;
	m_skillAttackEffectIdTable[SKID_HT_FREEZINGTRAP]	= -1;
	m_skillAttackEffectIdTable[SKID_HT_BLASTMINE]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_CLAYMORETRAP]	= -1;
	m_skillAttackEffectIdTable[SKID_HT_REMOVETRAP]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_TALKIEBOX]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_BEASTBANE]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_FALCON]			= -1;
	m_skillAttackEffectIdTable[SKID_HT_STEELCROW]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_BLITZBEAT]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_DETECTING]		= -1;
	m_skillAttackEffectIdTable[SKID_HT_SPRINGTRAP]		= -1;
	m_skillAttackEffectIdTable[SKID_AS_RIGHT]			= -1;
	m_skillAttackEffectIdTable[SKID_AS_LEFT]			= -1;
	m_skillAttackEffectIdTable[SKID_AS_KATAR]			= -1;
	m_skillAttackEffectIdTable[SKID_AS_CLOAKING]		= -1;
	m_skillAttackEffectIdTable[SKID_AS_SONICBLOW]		= -1;
	m_skillAttackEffectIdTable[SKID_AS_GRIMTOOTH]		= -1;
	m_skillAttackEffectIdTable[SKID_AS_ENCHANTPOISON]	= -1;
	m_skillAttackEffectIdTable[SKID_AS_POISONREACT]		= -1;
	m_skillAttackEffectIdTable[SKID_AS_VENOMDUST]		= -1;
	m_skillAttackEffectIdTable[SKID_AS_SPLASHER]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_IRON]			= -1;
	m_skillAttackEffectIdTable[SKID_BS_STEEL]			= -1;
	m_skillAttackEffectIdTable[SKID_BS_ENCHANTEDSTONE]	= -1;
	m_skillAttackEffectIdTable[SKID_BS_ORIDEOCON]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_DAGGER]			= -1;
	m_skillAttackEffectIdTable[SKID_BS_SWORD]			= -1;
	m_skillAttackEffectIdTable[SKID_BS_TWOHANDSWORD]	= -1;
	m_skillAttackEffectIdTable[SKID_BS_AXE]				= -1;
	m_skillAttackEffectIdTable[SKID_BS_MACE]			= -1;
	m_skillAttackEffectIdTable[SKID_BS_KNUCKLE]			= -1;
	m_skillAttackEffectIdTable[SKID_BS_SPEAR]			= -1;
	m_skillAttackEffectIdTable[SKID_BS_HILTBINDING]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_FINDINGORE]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_WEAPONRESEARCH]	= -1;
	m_skillAttackEffectIdTable[SKID_BS_REPAIRWEAPON]	= EF_REPAIRWEAPON;
	m_skillAttackEffectIdTable[SKID_BS_SKINTEMPER]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_HAMMERFALL]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_ADRENALINE]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_WEAPONPERFECT]	= EF_PERFECTION;
	m_skillAttackEffectIdTable[SKID_BS_OVERTHRUST]		= -1;
	m_skillAttackEffectIdTable[SKID_BS_MAXIMIZE]		= EF_MAXPOWER;
	m_skillAttackEffectIdTable[SKID_AS_SPLASHER]		= -1;
#ifdef	MOB_SKILL
	m_skillAttackEffectIdTable[SKID_NPC_MENTALBREAKER]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_MAGICALATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_PIERCINGATT]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_RANGEATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_ATTRICHANGE]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGEWATER]	= EF_CHANGECOLD;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGEGROUND]	= EF_CHANGEEARTH;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGEFIRE]	= EF_CHANGEFIRE;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGEWIND]	= EF_CHANGEWIND;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGEPOISON]	= EF_CHANGEPOISON;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGEHOLY]	= EF_CHAINGEHOLY;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGEDARKNESS]	= EF_CHANGEDARK;
	m_skillAttackEffectIdTable[SKID_NPC_CHANGETELEKINESIS]	= EF_CHANGEFLAME;
	m_skillAttackEffectIdTable[SKID_NPC_COMBOATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_GUIDEDATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_SELFDESTRUCTION]	= EF_SUI_EXPLOSION;
	m_skillAttackEffectIdTable[SKID_NPC_SPLASHATTACK]	= -1;

	m_skillAttackEffectIdTable[SKID_NPC_SUICIDE]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_SUICIDE]	= EF_SUICIDE;

	m_skillAttackEffectIdTable[SKID_NPC_POISON]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_BLINDATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_SILENCEATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_WIDESILENCE]	= EF_DARKCASTING;
	m_skillAttackEffectIdTable[SKID_NPC_WIDESTUN]	= EF_DARKCASTING;
	m_skillAttackEffectIdTable[SKID_NPC_WIDESTONE]	= EF_DARKCASTING;
	m_skillAttackEffectIdTable[SKID_NPC_WIDESLEEP]	= EF_DARKCASTING;
	m_skillAttackEffectIdTable[SKID_NPC_WIDECONFUSE]	= EF_DARKCASTING;
	m_skillAttackEffectIdTable[SKID_NPC_STUNATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_PETRIFYATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_CURSEATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_SLEEPATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_RANDOMATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_WATERATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_GROUNDATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_FIREATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_FIREBREATH]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_ANTIMAGIC]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_HELLJUDGEMENT]	= -1;
#if defined(MOB_SKILL_PATCH)
	m_skillAttackEffectIdTable[SKID_NPC_UNDEADATTACK]	= -1;
#endif
	m_skillAttackEffectIdTable[SKID_NPC_WINDATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_POISONATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_HOLYATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_DARKNESSATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_TELEKINESISATTACK]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_BLOODDRAIN]		= -1;
	m_skillAttackEffectIdTable[SKID_NPC_ENERGYDRAIN]	= -1;
	m_skillAttackEffectIdTable[SKID_NPC_DARKBREATH]		= -1;
#endif

	m_skillAttackedEffectIdTable.clear();
	m_skillAttackedEffectIdTable.reserve(SESSION_SKID_LAST);
	m_skillAttackedEffectIdTable.resize(SESSION_SKID_LAST);
	for (int i=0; i<SESSION_SKID_LAST; ++i) {
		m_skillAttackedEffectIdTable[i] = -1;
	}

	m_skillAttackedEffectIdTable[0] = EF_HIT1;
	m_skillAttackedEffectIdTable[SKID_NV_BASIC]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_SM_SWORD]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_SM_TWOHAND]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_SM_RECOVERY]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_SM_BASH]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_SM_PROVOKE]			= EF_HIT2;

	m_skillAttackedEffectIdTable[SKID_SM_MAGNUM]			= EF_MAGNUMBREAK;

	m_skillAttackedEffectIdTable[SKID_SM_ENDURE]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MG_SRECOVERY]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MG_SIGHT]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MG_NAPALMBEAT]		= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MG_SAFETYWALL]		= -1;
	m_skillAttackedEffectIdTable[SKID_MG_SOULSTRIKE]		= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MG_COLDBOLT]			= -1;
	m_skillAttackedEffectIdTable[SKID_MG_FROSTDIVER]		= EF_FROSTDIVER2;
	m_skillAttackedEffectIdTable[SKID_MG_STONECURSE]		= EF_STONECURSE;
	m_skillAttackedEffectIdTable[SKID_MG_FIREBALL]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MG_FIREWALL]			= -1;
	m_skillAttackedEffectIdTable[SKID_MG_FIREBOLT]			= -1 ;
	m_skillAttackedEffectIdTable[SKID_MG_LIGHTNINGBOLT]		= -1;
	m_skillAttackedEffectIdTable[SKID_MG_THUNDERSTORM]		= -1;
	m_skillAttackedEffectIdTable[SKID_AL_DP]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_DEMONBANE]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_RUWACH]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_PNEUMA]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_TELEPORT]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_WARP]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_HEAL]				= EF_HIT2;

	m_skillAttackedEffectIdTable[SKID_AL_INCAGI]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_CASH_INCAGI]			= EF_HIT2;

	m_skillAttackedEffectIdTable[SKID_AL_DECAGI]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_HOLYWATER]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_CRUCIS]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_ANGELUS]			= EF_HIT2;

	m_skillAttackedEffectIdTable[SKID_AL_BLESSING]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_CASH_BLESSING]		= EF_HIT2;

	m_skillAttackedEffectIdTable[SKID_AL_CURE]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MC_INCCARRY]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MC_DISCOUNT]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MC_OVERCHARGE]		= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MC_PUSHCART]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MC_IDENTIFY]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MC_VENDING]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MC_MAMMONITE]			= EF_COIN;
	m_skillAttackedEffectIdTable[SKID_AC_OWL]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AC_VULTURE]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AC_CONCENTRATION]		= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AC_DOUBLE]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_HT_POWER]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_HT_PHANTASMIC]		= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AC_SHOWER]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_TF_DOUBLE]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_TF_MISS]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_TF_STEAL]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_TF_HIDING]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_TF_POISON]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_TF_DETOXIFY]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_ALL_RESURRECTION]		= EF_REVIVE;
	m_skillAttackedEffectIdTable[SKID_KN_SPEARMASTERY]		= -1;
	m_skillAttackedEffectIdTable[SKID_KN_PIERCE]			= EF_PIERCE;
	m_skillAttackedEffectIdTable[SKID_KN_BRANDISHSPEAR]		= -1;
	m_skillAttackedEffectIdTable[SKID_KN_SPEARSTAB]			= EF_PIERCE;
	m_skillAttackedEffectIdTable[SKID_KN_SPEARBOOMERANG]	= EF_HIT4;
	m_skillAttackedEffectIdTable[SKID_KN_TWOHANDQUICKEN]	= -1;
	m_skillAttackedEffectIdTable[SKID_KN_AUTOCOUNTER]		= -1;
	m_skillAttackedEffectIdTable[SKID_KN_BOWLINGBASH]		= EF_BOWLINGBASH;
	m_skillAttackedEffectIdTable[SKID_KN_RIDING]			= -1;
	m_skillAttackedEffectIdTable[SKID_KN_CAVALIERMASTERY]	= -1;
	m_skillAttackedEffectIdTable[SKID_PR_MACEMASTERY]		= -1;
	m_skillAttackedEffectIdTable[SKID_PR_IMPOSITIO]			= EF_IMPOSITIO;
	m_skillAttackedEffectIdTable[SKID_PR_SUFFRAGIUM]		= EF_SUFFRAGIUM;
	m_skillAttackedEffectIdTable[SKID_PR_ASPERSIO]			= EF_HOLYHIT;
	m_skillAttackedEffectIdTable[SKID_PR_BENEDICTIO]		= -1;
	m_skillAttackedEffectIdTable[SKID_ALL_ANGEL_PROTECT]	= -1;

	m_skillAttackedEffectIdTable[SKID_PR_SANCTUARY]			= -1;
	m_skillAttackedEffectIdTable[SKID_PR_STRECOVERY]		= -1;
	m_skillAttackedEffectIdTable[SKID_PR_KYRIE]				= -1;
	m_skillAttackedEffectIdTable[SKID_PR_MAGNIFICAT]		= EF_MAGNIFICAT;
	m_skillAttackedEffectIdTable[SKID_PR_GLORIA]			= EF_GLORIA;
	m_skillAttackedEffectIdTable[SKID_PR_LEXDIVINA]			= EF_LEXDIVINA;
	m_skillAttackedEffectIdTable[SKID_PR_TURNUNDEAD]		= EF_HOLYHIT;
	m_skillAttackedEffectIdTable[SKID_PR_LEXAETERNA]		= EF_LEXAETERNA;
	m_skillAttackedEffectIdTable[SKID_PR_MAGNUS]			= EF_HOLYHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_FIREPILLAR]		= EF_FIREHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_SIGHTRASHER]		= EF_FIREHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_METEOR]			= EF_FIREHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_JUPITEL]			= EF_YUFITELHIT;
#if defined(MOB_SKILL_PATCH)
	m_skillAttackedEffectIdTable[SKID_NPC_DARKTHUNDER]		= EF_YUFITEL2;
#endif
	m_skillAttackedEffectIdTable[SKID_WZ_VERMILION]			= EF_WINDHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_FROSTNOVA]			= EF_FROSTDIVER2;
	m_skillAttackedEffectIdTable[SKID_WZ_STORMGUST]			= EF_STORMGUST;
	m_skillAttackedEffectIdTable[SKID_WZ_EARTHSPIKE]		= EF_EARTHHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_HEAVENDRIVE]		= EF_EARTHHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_QUAGMIRE]			= EF_EARTHHIT;
	m_skillAttackedEffectIdTable[SKID_WZ_ESTIMATION]		= EF_LOCKON;
	m_skillAttackedEffectIdTable[SKID_HT_SKIDTRAP]			= EF_BOWLINGBASH;
	m_skillAttackedEffectIdTable[SKID_HT_LANDMINE]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_ANKLESNARE]		= -1;
	m_skillAttackedEffectIdTable[SKID_HT_SHOCKWAVE]			= -1;

	m_skillAttackedEffectIdTable[SKID_HT_SANDMAN]			= -1;
	m_skillAttackedEffectIdTable[SKID_GM_SANDMAN]			= -1;
	m_skillAttackedEffectIdTable[SKID_ALL_CATCRY]			= -1;
	m_skillAttackedEffectIdTable[SKID_ALL_PARTYFLEE]			= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_PICK_ITEM]			= -1;

	m_skillAttackedEffectIdTable[SKID_HT_FLASHER]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_FREEZINGTRAP]		= -1;
	m_skillAttackedEffectIdTable[SKID_HT_BLASTMINE]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_CLAYMORETRAP]		= -1;
	m_skillAttackedEffectIdTable[SKID_HT_REMOVETRAP]		= -1;
	m_skillAttackedEffectIdTable[SKID_HT_TALKIEBOX]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_BEASTBANE]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_FALCON]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_STEELCROW]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_BLITZBEAT]			= EF_BLITZBEAT;
	m_skillAttackedEffectIdTable[SKID_HT_DETECTING]			= -1;
	m_skillAttackedEffectIdTable[SKID_HT_SPRINGTRAP]		= -1;
	m_skillAttackedEffectIdTable[SKID_AS_RIGHT]				= -1;
	m_skillAttackedEffectIdTable[SKID_AS_LEFT]				= -1;
	m_skillAttackedEffectIdTable[SKID_AS_KATAR]				= -1;
	m_skillAttackedEffectIdTable[SKID_AS_CLOAKING]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AS_SONICBLOW]			= EF_SONICBLOWHIT;
	m_skillAttackedEffectIdTable[SKID_AS_GRIMTOOTH]			= EF_GRIMTOOTHATK;
	m_skillAttackedEffectIdTable[SKID_AS_ENCHANTPOISON	]	= -1;
	m_skillAttackedEffectIdTable[SKID_AS_POISONREACT]		= -1;
	m_skillAttackedEffectIdTable[SKID_AS_VENOMDUST]			= EF_VENOMDUST;
	m_skillAttackedEffectIdTable[SKID_AS_SPLASHER]			= -1;
	m_skillAttackedEffectIdTable[SKID_BS_IRON]				= -1;
	m_skillAttackedEffectIdTable[SKID_BS_STEEL]				= -1;
	m_skillAttackedEffectIdTable[SKID_BS_ENCHANTEDSTONE]	= -1;
	m_skillAttackedEffectIdTable[SKID_BS_ORIDEOCON]			= -1;
	m_skillAttackedEffectIdTable[SKID_BS_DAGGER]			= -1;
	m_skillAttackedEffectIdTable[SKID_BS_SWORD]				= -1;
	m_skillAttackedEffectIdTable[SKID_BS_TWOHANDSWORD]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_AXE]				= -1;
	m_skillAttackedEffectIdTable[SKID_BS_MACE]				= -1;
	m_skillAttackedEffectIdTable[SKID_BS_KNUCKLE]			= -1;
	m_skillAttackedEffectIdTable[SKID_BS_SPEAR]				= -1;
	m_skillAttackedEffectIdTable[SKID_BS_HILTBINDING]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_FINDINGORE]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_WEAPONRESEARCH]	= -1;
	m_skillAttackedEffectIdTable[SKID_BS_REPAIRWEAPON]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_SKINTEMPER]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_HAMMERFALL]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_ADRENALINE]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_WEAPONPERFECT]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_OVERTHRUST]		= -1;
	m_skillAttackedEffectIdTable[SKID_BS_MAXIMIZE]			= -1;
	m_skillAttackedEffectIdTable[SKID_AS_SPLASHER]			= -1;
#if defined(BALANCE_PATCH)
	m_skillAttackedEffectIdTable[SKID_NV_TRICKDEAD]			= -1;
	m_skillAttackedEffectIdTable[SKID_MC_CARTREVOLUTION]	= EF_CARTREVOLUTION;
#endif
#ifdef MOB_SKILL
	m_skillAttackedEffectIdTable[SKID_NPC_MENTALBREAKER]	= EF_MENTALBREAK;
	m_skillAttackedEffectIdTable[SKID_NPC_MAGICALATTACK]	= EF_MAGICALATTHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_PIERCINGATT]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_RANGEATTACK]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_ATTRICHANGE]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGEWATER]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGEGROUND]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGEFIRE]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGEWIND]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGEPOISON]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGEHOLY]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGEDARKNESS]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_CHANGETELEKINESIS]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_COMBOATTACK]	= EF_COMBOATTACK1;
	m_skillAttackedEffectIdTable[SKID_NPC_GUIDEDATTACK]	= EF_GUIDEDATTACK;
	m_skillAttackedEffectIdTable[SKID_NPC_SELFDESTRUCTION]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_SPLASHATTACK]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_SUICIDE]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_POISON]	= EF_ENCHANTPOISON;
	m_skillAttackedEffectIdTable[SKID_NPC_BLINDATTACK]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_SILENCEATTACK]	= EF_SILENCEATTACK;
	m_skillAttackedEffectIdTable[SKID_NPC_WIDESILENCE]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_STUNATTACK]	= EF_STUNATTACK;
	m_skillAttackedEffectIdTable[SKID_NPC_PETRIFYATTACK]	= EF_PETRIFYATTACK;
	m_skillAttackedEffectIdTable[SKID_NPC_CURSEATTACK]	= EF_CURSEATTACK;
	m_skillAttackedEffectIdTable[SKID_NPC_SLEEPATTACK]	= EF_SLEEPATTACK;
	m_skillAttackedEffectIdTable[SKID_NPC_RANDOMATTACK]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_WATERATTACK]	= EF_COLDHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_GROUNDATTACK]	= EF_EARTHHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_FIREATTACK]	= EF_FIREHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_FIREBREATH]	= EF_FIREHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_HELLJUDGEMENT]	= -1;
	m_skillAttackedEffectIdTable[SKID_NPC_WINDATTACK]	= EF_WINDHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_POISONATTACK]	= EF_POISONATTACK;
	m_skillAttackedEffectIdTable[SKID_NPC_HOLYATTACK]	= EF_HOLYHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_DARKNESSATTACK]	= EF_DARKATTACK;
#if defined(MOB_SKILL_PATCH)
	m_skillAttackedEffectIdTable[SKID_NPC_UNDEADATTACK]	= -1;
#endif
	m_skillAttackedEffectIdTable[SKID_NPC_TELEKINESISATTACK]	= EF_TELEKHIT;
	m_skillAttackedEffectIdTable[SKID_NPC_BLOODDRAIN]	= EF_BLOODDRAIN;
	m_skillAttackedEffectIdTable[SKID_NPC_ENERGYDRAIN]	= EF_ENERGYDRAIN;
	m_skillAttackedEffectIdTable[SKID_NPC_DARKBREATH]	= EF_DARKBREATH;
#endif
#ifdef	SECONDJOB_SKILL2

	m_skillAttackedEffectIdTable[SKID_MO_FINGEROFFENSIVE]		= EF_HIT2;

	m_skillAttackedEffectIdTable[SKID_MO_COMBOFINISH]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_AL_HOLYLIGHT]				= EF_HOLYHIT;
	m_skillAttackedEffectIdTable[SKID_MO_CHAINCOMBO]			= EF_SONICBLOWHIT;
#endif


	m_skillAttackedEffectIdTable[SKID_CH_TIGERFIST]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_CH_CHAINCRUSH]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_SN_FALCONASSAULT]			= EF_BLITZBEAT;
	m_skillAttackedEffectIdTable[SKID_SN_SHARPSHOOTING]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_CG_ARROWVULCAN]			= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_LK_SPIRALPIERCE]			= EF_PIERCE;
	m_skillAttackedEffectIdTable[SKID_HW_NAPALMVULCAN]			= -1;
	m_skillAttackedEffectIdTable[SKID_WS_CARTTERMINATION]		= -1;
	m_skillAttackedEffectIdTable[SKID_KN_CHARGEATK]				= EF_HIT2;
	m_skillAttackedEffectIdTable[SKID_MO_BALKYOUNG]				= EF_HIT3;
	m_skillAttackedEffectIdTable[SKID_NJ_KOUENKA]				= EF_FIREHIT;
	m_skillAttackedEffectIdTable[SKID_NJ_HYOUSENSOU]			= EF_COLDHIT;
}


void	CSession :: GetSkillActionInfo(int skillId, int &beginEffectId, int &motionType)
{
	beginEffectId = EF_BASH;
	motionType = ST_SKILL;
	switch (skillId) {
		case	SKID_NV_BASIC:
			break;
		case	SKID_SM_SWORD:
		case	SKID_SM_TWOHAND:
		case	SKID_SM_RECOVERY:
		case	SKID_SM_PROVOKE:
		case	SKID_SM_ENDURE:
			break;
		case	SKID_SM_BASH:
			motionType = ST_ATTACK;
			break;
		case	SKID_SM_MAGNUM:
			motionType = ST_ATTACK;
			break;
		case	SKID_MG_SRECOVERY:
		case	SKID_MG_SIGHT:
		case	SKID_NPC_WIDESIGHT:
		case	SKID_MG_NAPALMBEAT:
		case	SKID_MG_SAFETYWALL:
		case	SKID_MG_SOULSTRIKE:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKSTRIKE:
		case	SKID_NPC_STOP:
#endif
		case	SKID_MG_COLDBOLT:
		case	SKID_MG_FROSTDIVER:
		case	SKID_MG_STONECURSE:
		case	SKID_MG_FIREBALL:
		case	SKID_MG_FIREWALL:
		case	SKID_MG_FIREBOLT:
		case	SKID_MG_LIGHTNINGBOLT:
		case	SKID_MG_THUNDERSTORM:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_ALL_RESURRECTION:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_AL_HEAL:
			beginEffectId = -1;
			motionType = ST_SKILL;
			break;
		case SKID_PR_DEVOTION:
			break ;
		case	SKID_AL_DP:
		case	SKID_AL_DEMONBANE:
		case	SKID_AL_RUWACH:
		case	SKID_AL_PNEUMA:
		case	SKID_AL_TELEPORT:
		case	SKID_AL_WARP:
		case	SKID_AL_DECAGI:
		case	SKID_AL_HOLYWATER:
		case	SKID_AL_CRUCIS:
		case	SKID_AL_ANGELUS:
		case	SKID_AL_CURE:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_AL_INCAGI:
		case	SKID_CASH_INCAGI:
			beginEffectId = -1;
			motionType = ST_STAND;
			break;
		case	SKID_AL_BLESSING:
		case	SKID_CASH_BLESSING:
			beginEffectId = -1;
			motionType = ST_SKILL;
			break;
		case	SKID_MC_INCCARRY:
		case	SKID_MC_DISCOUNT:
		case	SKID_MC_OVERCHARGE:
		case	SKID_MC_PUSHCART:
		case	SKID_MC_IDENTIFY:
		case	SKID_MC_VENDING:
			break;
		case	SKID_MC_MAMMONITE:
			motionType = ST_ATTACK;
			break;
		case	SKID_AC_OWL:
		case	SKID_AC_VULTURE:
			break;
		case	SKID_AC_CONCENTRATION:
			beginEffectId = EF_INCAGIDEX;
			break;
		case	SKID_AC_DOUBLE:
		case	SKID_HT_POWER:
		case	SKID_HT_PHANTASMIC:
			motionType = ST_ATTACK;
			break;
		case	SKID_AC_SHOWER:
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_TF_DOUBLE:
		case	SKID_TF_MISS:
		case	SKID_TF_STEAL:
		case	SKID_TF_DETOXIFY:
			break;
		case	SKID_TF_POISON:
			break;

		case	SKID_KN_SPEARMASTERY:
		case	SKID_KN_TWOHANDQUICKEN:
		case	SKID_KN_ONEHAND:
#ifdef	SECONDJOB_SKILL2
		case	SKID_CR_SPEARQUICKEN:
#endif
		case	SKID_KN_RIDING:
		case	SKID_KN_CAVALIERMASTERY:
			beginEffectId = -1;
			break;
		case	SKID_KN_AUTOCOUNTER:
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;

		case	SKID_KN_PIERCE:
		case	SKID_KN_SPEARSTAB:
		case	SKID_KN_BOWLINGBASH:
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;

		case	SKID_KN_BRANDISHSPEAR:
			beginEffectId = EF_BRANDISH2;
			motionType = ST_ATTACK;
			break;

		case	SKID_KN_SPEARBOOMERANG:
			beginEffectId = -1;
			motionType = ST_THROW;
			break;

		case	SKID_MER_MAGNIFICAT:
		case	SKID_PR_MACEMASTERY:
		case	SKID_PR_IMPOSITIO:
		case	SKID_PR_SUFFRAGIUM:
		case	SKID_PR_ASPERSIO:
		case	SKID_PR_BENEDICTIO:
		case	SKID_ALL_ANGEL_PROTECT:
		case	SKID_PR_SANCTUARY:
		case	SKID_PR_SLOWPOISON:
		case	SKID_PR_STRECOVERY:
		case	SKID_PR_KYRIE:
		case	SKID_PR_MAGNIFICAT:
		case	SKID_PR_GLORIA:
		case	SKID_PR_LEXDIVINA:
		case	SKID_PR_TURNUNDEAD:
		case	SKID_PR_LEXAETERNA:
		case	SKID_PR_MAGNUS:
			beginEffectId = EF_BEGINSPELL;
			break;

		case	SKID_WZ_FIREPILLAR:
		case	SKID_WZ_SIGHTRASHER:
		case	SKID_WZ_FIREIVY:
		case	SKID_WZ_METEOR:
		case	SKID_WZ_JUPITEL:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKTHUNDER:
#endif
		case	SKID_WZ_VERMILION:
		case	SKID_WZ_ICEWALL:
		case	SKID_WZ_FROSTNOVA:
		case	SKID_WZ_STORMGUST:
		case	SKID_WZ_EARTHSPIKE:
		case	SKID_WZ_HEAVENDRIVE:
		case	SKID_WZ_QUAGMIRE:
		case	SKID_WZ_SIGHTBLASTER:
			beginEffectId = EF_BEGINSPELL;
			break;

		case	SKID_WZ_WATERBALL:
			beginEffectId = EF_BEGINSPELL;

			break;

		case	SKID_WZ_ESTIMATION:
			beginEffectId = -1;
			break;

		case	SKID_BS_IRON:
		case	SKID_BS_STEEL:
		case	SKID_BS_ENCHANTEDSTONE:
		case	SKID_BS_ORIDEOCON:
		case	SKID_BS_DAGGER:
		case	SKID_BS_SWORD:
		case	SKID_BS_TWOHANDSWORD:
		case	SKID_BS_AXE:
		case	SKID_BS_MACE:
		case	SKID_BS_KNUCKLE:
		case	SKID_BS_SPEAR:
		case	SKID_BS_HILTBINDING:
		case	SKID_BS_FINDINGORE:
		case	SKID_BS_WEAPONRESEARCH:
		case	SKID_BS_REPAIRWEAPON:
		case	SKID_BS_HAMMERFALL:
		case	SKID_BS_ADRENALINE:
		case	SKID_BS_ADRENALINE2:
		case	SKID_BS_WEAPONPERFECT:
		case	SKID_BS_OVERTHRUST:
		case	SKID_BS_MAXIMIZE:
		case	SKID_WS_OVERTHRUSTMAX:
			beginEffectId = -1;
			break;

		case	SKID_BS_SKINTEMPER:
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;

		case	SKID_HT_BEASTBANE:
		case	SKID_HT_FALCON:
		case	SKID_HT_STEELCROW:
		case	SKID_HT_BLITZBEAT:
		case	SKID_HT_DETECTING:
		case	SKID_HT_SPRINGTRAP:
			beginEffectId = -1;
			break;

		case	SKID_HT_SKIDTRAP:
		case	SKID_HT_LANDMINE:
		case	SKID_HT_ANKLESNARE:
		case	SKID_HT_SHOCKWAVE:

		case	SKID_HT_SANDMAN:
		case	SKID_GM_SANDMAN:

		case	SKID_HT_FREEZINGTRAP:
		case	SKID_HT_BLASTMINE:
		case	SKID_HT_CLAYMORETRAP:
		case	SKID_HT_REMOVETRAP:
		case	SKID_HT_TALKIEBOX:
		case	SKID_HT_FLASHER:
			motionType = ST_PICKUP;
			break;

		case	SKID_AS_RIGHT:
		case	SKID_AS_LEFT:
		case	SKID_AS_KATAR:
		case	SKID_AS_CLOAKING:
		case	SKID_AS_GRIMTOOTH:
		case	SKID_AS_ENCHANTPOISON:
		case	SKID_AS_POISONREACT:
		case	SKID_AS_VENOMDUST:
		case	SKID_AS_SPLASHER:
			beginEffectId = -1;
			break;

		case	SKID_AS_SONICBLOW:
			beginEffectId = -1;
			motionType = ST_SKILL;
			break;

		case	SKID_TF_HIDING:
			beginEffectId = -1;
			break;

#if defined(BALANCE_PATCH)
		case	SKID_MC_CARTREVOLUTION:
			beginEffectId = -1;
			break;
#endif
#ifdef	MOB_SKILL
		case	SKID_NPC_PIERCINGATT:
		case	SKID_NPC_MENTALBREAKER:
		case	SKID_NPC_RANGEATTACK:
		case	SKID_NPC_ATTRICHANGE:
		case	SKID_NPC_CHANGEWATER:
		case	SKID_NPC_CHANGEGROUND:
		case	SKID_NPC_CHANGEFIRE:
		case	SKID_NPC_CHANGEWIND:
		case	SKID_NPC_CHANGEPOISON:
		case	SKID_NPC_CHANGEHOLY:
		case	SKID_NPC_CHANGEDARKNESS:
		case	SKID_NPC_CHANGETELEKINESIS:
		case	SKID_NPC_CRITICALSLASH:
		case	SKID_NPC_COMBOATTACK:
		case	SKID_NPC_GUIDEDATTACK:
		case	SKID_NPC_SELFDESTRUCTION:
		case	SKID_NPC_SPLASHATTACK:
		case	SKID_NPC_SUICIDE:
		case	SKID_NPC_POISON:
		case	SKID_NPC_BLINDATTACK:
		case	SKID_NPC_SILENCEATTACK:
		case	SKID_NPC_WIDESILENCE:
		case	SKID_NPC_STUNATTACK:
		case	SKID_NPC_WIDESTUN:
		case	SKID_NPC_WIDESTONE:
		case	SKID_NPC_WIDESLEEP:
		case	SKID_NPC_WIDECONFUSE:
		case	SKID_NPC_PETRIFYATTACK:
		case	SKID_NPC_CURSEATTACK:
		case	SKID_NPC_SLEEPATTACK:
		case	SKID_NPC_RANDOMATTACK:
		case	SKID_NPC_WATERATTACK:
		case	SKID_NPC_GROUNDATTACK:
		case	SKID_NPC_FIREATTACK:
		case	SKID_NPC_FIREBREATH:
		case	SKID_NPC_ANTIMAGIC:
		case	SKID_NPC_STONESKIN:
		case	SKID_NPC_WINDATTACK:
		case	SKID_NPC_POISONATTACK:
		case	SKID_NPC_HOLYATTACK:
		case	SKID_NPC_DARKNESSATTACK:
		case	SKID_NPC_TELEKINESISATTACK:
		case	SKID_NPC_MAGICALATTACK:
			beginEffectId = -1;
			break;
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_UNDEADATTACK:
			beginEffectId = EF_DARKCASTING;
			break;
#endif
		case	SKID_NPC_SUMMONSLAVE:
		case	SKID_NPC_SUMMONMONSTER:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
#endif
#ifdef	SECONDJOB_SKILL2
		case	SKID_ML_AUTOGUARD:
		case	SKID_ML_DEFENDER :
		case	SKID_CR_AUTOGUARD:
		case	SKID_CR_REFLECTSHIELD:
		case	SKID_CR_DEFENDER:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_CR_HOLYCROSS:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKCROSS:
#endif
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_CR_SHIELDCHARGE:
		case	SKID_CR_SHIELDBOOMERANG:
		case	SKID_PA_SHIELDCHAIN:
			motionType = ST_THROW;
			beginEffectId = -1;
			break;
		case	SKID_ITM_TOMAHAWK:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_CR_PROVIDENCE:
		case	SKID_MO_CALLSPIRITS:
		case	SKID_MO_KITRANSLATION:
			motionType = ST_STAND;
			break;
		case	SKID_RG_INTIMIDATE:
		case	SKID_MO_INVESTIGATE :
		case	SKID_MO_FINGEROFFENSIVE :
		case	SKID_NPC_EXPULSION:
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_MO_STEELBODY :
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_POWERUP :
#endif
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_RG_BACKSTAP :
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_RG_RAID :
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_BA_MUSICALSTRIKE :
		case	SKID_DC_THROWARROW:
			motionType = ST_ATTACK2;
			beginEffectId = -1;
			break;
		case	SKID_AM_POTIONPITCHER:
		case	SKID_AM_ACIDTERROR:
		case	SKID_AM_DEMONSTRATION:
		case	SKID_AM_CANNIBALIZE:
			motionType = ST_THROW;
			beginEffectId = -1;
			break;
		case	SKID_DC_WINKCHARM:
			motionType = ST_DANCE;
			beginEffectId = -1;
			break;
		case	SKID_AM_BERSERKPITCHER:
			motionType = -1;
			beginEffectId = -1;
			break;
		case	SKID_AM_CALLHOMUN:
		case	SKID_AM_REST:
		case	SKID_AM_RESURRECTHOMUN:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_CR_ACIDDEMONSTRATION:
			motionType = -1;
			beginEffectId = -1;
			break;
		case	SKID_CR_SLIMPITCHER:
			motionType = -1;
			beginEffectId = -1;
			break;
		case	SKID_AM_CP_WEAPON:
		case	SKID_AM_CP_SHIELD:
		case	SKID_AM_CP_ARMOR:
		case	SKID_AM_CP_HELM:
		case	SKID_CR_FULLPROTECTION:
			beginEffectId = -1;
			break;
		case	SKID_MO_CHAINCOMBO :
		case	SKID_MO_COMBOFINISH :
		case	SKID_MO_EXTREMITYFIST :
			beginEffectId = -1;
			break;
		case	SKID_MO_TRIPLEATTACK :
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;
		case	SKID_MO_BLADESTOP:
			motionType = ST_STAND;
			break;
		case	SKID_MO_BODYRELOCATION :
			beginEffectId = -1;
			break;
		case	SKID_DC_FORTUNEKISS :
		case	SKID_DC_UGLYDANCE :
		case	SKID_DC_HUMMING :
		case	SKID_DC_DONTFORGETME :
		case	SKID_DC_SERVICEFORYOU :
			beginEffectId = -1;
			motionType = ST_DANCE;
			break;
		case	SKID_BA_APPLEIDUN :
		case	SKID_BA_DISSONANCE :
		case	SKID_BA_WHISTLE :
		case	SKID_BA_ASSASSINCROSS :
		case	SKID_BA_POEMBRAGI :
			beginEffectId = -1;
			motionType = ST_SING;
			break;
		case	SKID_TF_THROWSTONE :
		case	SKID_TF_SPRINKLESAND :
		case	SKID_AS_VENOMKNIFE :
			beginEffectId = -1;
			motionType = ST_THROW;
			break;
		case	SKID_BD_LULLABY :
		case	SKID_BD_RICHMANKIM :
		case	SKID_BD_ETERNALCHAOS :
		case	SKID_BD_DRUMBATTLEFIELD :
		case	SKID_BD_SIEGFRIED :
		case	SKID_CG_HERMODE :
		case	SKID_BD_RINGNIBELUNGEN :
		case	SKID_BD_ROKISWEIL :
		case	SKID_BD_INTOABYSS :
			beginEffectId = -1;
			motionType = ST_DANCE;
			break;
		case	SKID_BD_ADAPTATION :
			beginEffectId = -1;
			motionType = ST_STAND;
			break;
		case	SKID_CG_LONGINGFREEDOM :
		case	SKID_CG_TAROTCARD :
			beginEffectId = -1;
			motionType = ST_SKILL;
			break;
		case	SKID_AL_HOLYLIGHT:
		case	SKID_MG_ENERGYCOAT:
			beginEffectId = -1;
			motionType = ST_SKILL;
			break;
		case	SKID_TF_PICKSTONE :
			beginEffectId = -1;
			motionType = ST_STAND;
			break;
#ifdef WEDDING_PATCH
		case	SKID_WE_MALE :
		case	SKID_WE_FEMALE :
		case	SKID_WE_BABY :
			beginEffectId = -1;
			motionType = ST_SKILL;
			break;
#endif
		case	SKID_LK_AURABLADE:
			break;
		case	SKID_LK_PARRYING:
		case	SKID_MS_PARRYING:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_MS_BERSERK:
		case	SKID_LK_BERSERK:
		case	SKID_LK_FURY:
		case	SKID_LK_CONCENTRATION:
			motionType = ST_SKILL;
			break;
		case	SKID_PA_PRESSURE :
		case	SKID_PA_SACRIFICE :
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_PA_GOSPEL :
			motionType = ST_STAND;
			break;
		case	SKID_HP_ASSUMPTIO :
		case	SKID_CASH_ASSUMPTIO :
		case	SKID_HP_BASILICA :
			motionType = ST_SKILL;
			break;
		case	SKID_CH_PALMSTRIKE :
		case	SKID_CH_CHAINCRUSH :
		case	SKID_MO_BALKYOUNG :
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;
		case	SKID_CH_TIGERFIST :
			beginEffectId = -1;
			break;
		case	SKID_ASC_BREAKER :
		case	SKID_ASC_METEORASSAULT :
		case	SKID_NPC_PULSESTRIKE :
			beginEffectId = -1;
			break;
		case	SKID_HW_SOULDRAIN:
			motionType = ST_MOVE;
			beginEffectId = -1;
			break;
		case	SKID_HW_MAGICCRASHER:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_HW_MAGICPOWER:
			motionType = ST_ATTACK;
			break;
		case	SKID_PF_DOUBLECASTING:
			motionType = ST_SKILL;
			break;
		case	SKID_PF_HPCONVERSION:
		case	SKID_PF_SOULCHANGE:
		case	SKID_PF_SOULBURN:
		case	SKID_PF_MINDBREAKER:
		case	SKID_PF_MEMORIZE:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_SN_SIGHT:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_SN_WINDWALK:
			beginEffectId = -1;
			break;
		case	SKID_SN_FALCONASSAULT:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_SN_SHARPSHOOTING:
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_ST_CHASEWALK:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_HW_NAPALMVULCAN:
		case	SKID_HW_GANBANTEIN:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_WS_MELTDOWN :
		case	SKID_WS_CARTBOOST :
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_AGIUP :
#endif
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_CG_ARROWVULCAN:
			motionType = ST_ATTACK2;
			beginEffectId = -1;
			break;
		case	SKID_CG_MOONLIT :
		case	SKID_CG_MARIONETTE :
			beginEffectId = -1;
			motionType = ST_DANCE;
			break;
		case	SKID_LK_SPIRALPIERCE:
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;
		case	SKID_LK_HEADCRUSH :
		case	SKID_LK_JOINTBEAT :
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;
		case	SKID_HW_GRAVITATION:
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;
		case	SKID_CH_SOULCOLLECT:
			motionType = ST_STAND;
			break;
		case	SKID_PF_SPIDERWEB :
			motionType = ST_SKILL;
			break;
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_SHIELDBRAKE:
		case	SKID_NPC_ARMORBRAKE:
		case	SKID_NPC_HELMBRAKE:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
#endif
		case	SKID_TK_RUN:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_TK_HIGHJUMP:
			motionType = ST_LANDJUMP;
			beginEffectId = -1;
			break;
		case	SKID_SG_FEEL:
		case	SKID_SG_HATE:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_SG_FUSION:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_SG_SUN_WARM:
		case	SKID_SG_MOON_WARM:
		case	SKID_SG_STAR_WARM:
			motionType = ST_SG_TURN;
			beginEffectId = -1;
			break;
		case	SKID_SG_SUN_COMFORT:
		case	SKID_SG_MOON_COMFORT:
		case	SKID_SG_STAR_COMFORT:
			motionType = ST_SG_DOWN;
			beginEffectId = -1;
			break;
		case	SKID_TK_READYSTORM:
			motionType = ST_R_STORM;
			beginEffectId = -1;
			break;
		case	SKID_TK_STORMKICK:
			motionType = ST_ATTACK;
			beginEffectId = EF_STORMKICK;
			break;
		case	SKID_TK_READYDOWN:
			motionType = ST_R_DOWN;
			beginEffectId = -1;
			break;
		case	SKID_TK_DOWNKICK:
			motionType = ST_A_DOWN;
			beginEffectId = -1;
			break;
		case	SKID_TK_READYTURN:
			motionType = ST_R_TURN;
			beginEffectId = -1;
			break;
		case	SKID_TK_TURNKICK:
			motionType = ST_A_TURN;
			beginEffectId = -1;
			break;
		case	SKID_TK_READYCOUNTER:
			motionType = ST_R_COUNTER;
			beginEffectId = -1;
			break;
		case	SKID_TK_COUNTER:
			motionType = ST_A_COUNTER;
			beginEffectId = -1;
			break;
		case	SKID_TK_DODGE:
			motionType = ST_R_DODGE;
			beginEffectId = -1;
			break;
		case	SKID_TK_JUMPKICK:
			motionType = ST_A_JUMPKICK;
			beginEffectId = -1;
			break;
		case	SKID_TK_SEVENWIND:
		case	SKID_TK_MISSION:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_WS_CARTTERMINATION:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_SL_KAIZEL:
		case	SKID_SL_KAAHI :
		case	SKID_SL_KAUPE :
		case	SKID_SL_KAITE :
		case	SKID_SL_STIN :
		case	SKID_SL_STUN :
		case	SKID_SL_SMA :
		case	SKID_SL_SWOO :
		case	SKID_SL_SKE :
		case	SKID_SL_SKA :
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_SL_ALCHEMIST:
		case	SKID_SL_MONK:
		case	SKID_SL_STAR:
		case	SKID_SL_SAGE:
		case	SKID_SL_CRUSADER:
		case	SKID_SL_BLACKSMITH:
		case	SKID_SL_HUNTER:
		case	SKID_SL_SOULLINKER:
		case	SKID_SL_SUPERNOVICE:
		case	SKID_SL_KNIGHT:
		case	SKID_SL_WIZARD:
		case	SKID_SL_PRIEST:
		case	SKID_SL_BARDDANCER:
		case	SKID_SL_ROGUE:
		case	SKID_SL_ASSASIN:
		case	SKID_SL_HIGH:

		case	SKID_SL_DEATHKNIGHT:
		case	SKID_SL_COLLECTOR:
		case	SKID_SL_NINJA:
		case	SKID_SL_GUNNER:
			motionType = ST_R_TURN;
			beginEffectId = -1;
			break;
		case	SKID_HFLI_MOON:
		case	SKID_HFLI_SBR44:
			motionType = ST_SKILL;
			beginEffectId = -1;
			break;
		case	SKID_KN_CHARGEATK:
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_RG_CLOSECONFINE:
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_BS_GREED:
			motionType = ST_PICKUP;
			beginEffectId = -1;
			break;
		case	SKID_NJ_SYURIKEN:
		case	SKID_NJ_KUNAI:
		case	SKID_NJ_HUUMA:
		case	SKID_NJ_ZENYNAGE:
		case	SKID_NJ_UTSUSEMI:
		case	SKID_NJ_KOUENKA:
		case	SKID_NJ_HYOUSYOURAKU:
		case	SKID_NJ_HUUJIN:
		case	SKID_NJ_RAIGEKISAI:
		case	SKID_NJ_KAMAITACHI:
		case	SKID_NJ_NEN:
			motionType = ST_NINJASKILL2;
			beginEffectId = -1;
			break;
		case	SKID_GS_GLITTERING:
			motionType = ST_GUNSKILL2;
			beginEffectId = -1;
			break;
		case	SKID_NJ_TATAMIGAESHI:
		case	SKID_NJ_KASUMIKIRI:
		case	SKID_NJ_SHADOWJUMP:
		case	SKID_NJ_BUNSINJYUTSU:
		case	SKID_NJ_KAENSIN:
		case	SKID_NJ_BAKUENRYU:
		case	SKID_NJ_HYOUSENSOU:
		case	SKID_NJ_SUITON:
			motionType = ST_NINJASKILL1;
			beginEffectId = -1;
			break;
		case	SKID_NJ_KIRIKAGE:
			motionType = ST_ATTACK2;
			beginEffectId = -1;
			break;
		case	SKID_NJ_ISSEN:
			beginEffectId = -1;
			break;
		case	SKID_GS_TRIPLEACTION:
		case	SKID_GS_BULLSEYE:
		case	SKID_GS_MAGICALBULLET:
		case	SKID_GS_TRACKING:
		case	SKID_GS_DISARM:
		case	SKID_GS_PIERCINGSHOT:
		case	SKID_GS_RAPIDSHOWER:
		case	SKID_GS_DUST:
		case	SKID_GS_FULLBUSTER:
		case	SKID_GS_SPREADATTACK:
			motionType = ST_ATTACK;
			beginEffectId = -1;
			break;
		case	SKID_GS_FLING:
		case	SKID_GS_MADNESSCANCEL:
		case	SKID_GS_ADJUSTMENT:
		case	SKID_GS_INCREASING:
		case	SKID_GS_DESPERADO:
		case	SKID_GS_GATLINGFEVER:
			motionType = ST_GUNSKILL1;
			beginEffectId = -1;
			break;
		case	SKID_GS_CRACKER:
		case	SKID_GS_GROUNDDRIFT:
			motionType = ST_GUNSKILL3;
			beginEffectId = -1;
			break;
#endif
	}
}







void	CSession :: GetSkillActionInfo2(int skillId, int &beginEffectId, int &motionType, int property , int sjob)
{
	beginEffectId = EF_BASH;
	motionType = ST_BEGIN_SKILL;

	switch (skillId) {
		case	SKID_NV_BASIC:
			break;
		case	SKID_SM_SWORD:
		case	SKID_SM_TWOHAND:
		case	SKID_SM_RECOVERY:
		case	SKID_SM_PROVOKE:
		case	SKID_SM_ENDURE:
			break;
		case	SKID_SM_BASH:
			motionType = ST_ATTACK;
			break;
		case	SKID_SM_MAGNUM:
			motionType = ST_ATTACK;
			break;
		case	SKID_MG_SRECOVERY:
		case	SKID_MG_SIGHT:
		case	SKID_NPC_WIDESIGHT:
		case	SKID_MG_NAPALMBEAT:
		case	SKID_MG_SAFETYWALL:
		case	SKID_MG_SOULSTRIKE:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKSTRIKE:
		case	SKID_NPC_STOP:
#endif
		case	SKID_MG_COLDBOLT:
		case	SKID_MG_FROSTDIVER:
		case	SKID_MG_STONECURSE:
		case	SKID_MG_FIREBALL:
		case	SKID_MG_FIREWALL:
		case	SKID_MG_FIREBOLT:
		case	SKID_MG_LIGHTNINGBOLT:
		case	SKID_MG_THUNDERSTORM:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_WZ_FIREPILLAR:
		case	SKID_WZ_SIGHTRASHER:
		case	SKID_WZ_METEOR:
		case	SKID_WZ_JUPITEL:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKTHUNDER:
#endif
		case	SKID_WZ_VERMILION:
		case	SKID_WZ_ICEWALL:
		case	SKID_WZ_SIGHTBLASTER:
		case	SKID_WZ_FROSTNOVA:
		case	SKID_WZ_STORMGUST:
		case	SKID_WZ_EARTHSPIKE:
		case	SKID_WZ_HEAVENDRIVE:
		case	SKID_WZ_QUAGMIRE:
		case	SKID_WZ_FIREIVY:
			beginEffectId = EF_BEGINSPELL;
			break;

		case	SKID_WZ_WATERBALL:
			beginEffectId = EF_BEGINSPELL;

			break;

		case	SKID_WZ_ESTIMATION:
			beginEffectId = -1;
			break;

		case	SKID_KN_AUTOCOUNTER:
			beginEffectId = -1;
			motionType = ST_ATTACKWAIT;
			break;
		case	SKID_MER_MAGNIFICAT:
		case	SKID_PR_IMPOSITIO:
		case	SKID_PR_SUFFRAGIUM:
		case	SKID_PR_ASPERSIO:
		case	SKID_PR_BENEDICTIO:
		case	SKID_ALL_ANGEL_PROTECT:
		case	SKID_PR_SANCTUARY:
		case	SKID_PR_STRECOVERY:
		case	SKID_PR_KYRIE:
		case	SKID_PR_MAGNIFICAT:
		case	SKID_PR_GLORIA:
		case	SKID_PR_LEXDIVINA:
		case	SKID_PR_TURNUNDEAD:
		case	SKID_PR_LEXAETERNA:
		case	SKID_PR_MAGNUS:
			beginEffectId = EF_BEGINSPELL;
			break;

		case	SKID_KN_SPEARMASTERY:
		case	SKID_KN_TWOHANDQUICKEN:
		case	SKID_KN_ONEHAND:
#ifdef	SECONDJOB_SKILL2
		case	SKID_CR_SPEARQUICKEN:
#endif
		case	SKID_KN_RIDING:
		case	SKID_KN_CAVALIERMASTERY:
			beginEffectId = -1;
			break;

		case	SKID_KN_PIERCE:
		case	SKID_KN_SPEARSTAB:
		case	SKID_KN_BOWLINGBASH:
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;

		case	SKID_KN_BRANDISHSPEAR:
			beginEffectId = EF_BRANDISH2;
			motionType = ST_ATTACK;
			break;

		case	SKID_KN_SPEARBOOMERANG:
			beginEffectId = -1;
			motionType = ST_THROW;
			break;

		case	SKID_BS_IRON:
		case	SKID_BS_STEEL:
		case	SKID_BS_ENCHANTEDSTONE:
		case	SKID_BS_ORIDEOCON:
		case	SKID_BS_DAGGER:
		case	SKID_BS_SWORD:
		case	SKID_BS_TWOHANDSWORD:
		case	SKID_BS_WEAPONPERFECT:
		case	SKID_BS_OVERTHRUST:
		case	SKID_BS_MAXIMIZE:
		case	SKID_BS_AXE:
		case	SKID_BS_MACE:
		case	SKID_BS_KNUCKLE:
		case	SKID_BS_SPEAR:
		case	SKID_BS_HILTBINDING:
		case	SKID_BS_FINDINGORE:
		case	SKID_BS_WEAPONRESEARCH:
		case	SKID_BS_SKINTEMPER:
		case	SKID_BS_ADRENALINE:
		case	SKID_BS_ADRENALINE2:
		case	SKID_WS_OVERTHRUSTMAX:
			break;
		case	SKID_BS_REPAIRWEAPON:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_BS_HAMMERFALL:
			motionType = ST_ATTACK;
			break;

		case	SKID_HT_SKIDTRAP:
		case	SKID_HT_LANDMINE:
		case	SKID_HT_ANKLESNARE:
		case	SKID_HT_SHOCKWAVE:

		case	SKID_HT_SANDMAN:
		case	SKID_GM_SANDMAN:

		case	SKID_HT_FREEZINGTRAP:
		case	SKID_HT_BLASTMINE:
		case	SKID_HT_CLAYMORETRAP:
		case	SKID_HT_REMOVETRAP:
		case	SKID_HT_TALKIEBOX:
		case	SKID_HT_FLASHER:
			motionType = ST_PICKUP;
			break;

		case	SKID_HT_BEASTBANE:
		case	SKID_HT_FALCON:
		case	SKID_HT_STEELCROW:
		case	SKID_HT_BLITZBEAT:
		case	SKID_HT_DETECTING:
		case	SKID_HT_SPRINGTRAP:
			break;

		case	SKID_AS_RIGHT:
		case	SKID_AS_LEFT:
		case	SKID_AS_KATAR:
		case	SKID_AS_CLOAKING:
		case	SKID_AS_GRIMTOOTH:
		case	SKID_AS_ENCHANTPOISON:
		case	SKID_AS_POISONREACT:
		case	SKID_AS_VENOMDUST:
			break;
		case	SKID_AS_SPLASHER:
#if defined (_UPDATE2004B)
			beginEffectId = EF_BEGINSPELL7;
#endif
			break;

		case	SKID_AS_SONICBLOW:
			beginEffectId = -1;
			motionType = ST_SKILL;
			break;

		case	SKID_ALL_RESURRECTION:
		case	SKID_AL_HEAL:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_AL_BLESSING:
		case	SKID_CASH_BLESSING:
			beginEffectId = EF_BEGINSPELL;
			motionType = ST_STAND;
			break;
		case	SKID_AL_INCAGI:
		case	SKID_CASH_INCAGI:
			beginEffectId = EF_BEGINSPELL;
			motionType = ST_SKILL;
			break;
		case	SKID_AL_DEMONBANE:
		case	SKID_AL_DP:
		case	SKID_AL_RUWACH:
		case	SKID_AL_PNEUMA:
		case	SKID_AL_TELEPORT:
		case	SKID_AL_DECAGI:
		case	SKID_AL_WARP:
		case	SKID_AL_HOLYWATER:
		case	SKID_AL_CRUCIS:
		case	SKID_AL_ANGELUS:
		case	SKID_AL_CURE:
#ifdef SECONDJOB_SKILL2
		case	SKID_AL_HOLYLIGHT:
		case	SKID_MG_ENERGYCOAT:
		case	SKID_CR_GRANDCROSS:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_GRANDDARKNESS:
#endif
		case	SKID_CR_PROVIDENCE:
		case	SKID_CR_DEVOTION:

		case	SKID_SA_AUTOSPELL:
		case	SKID_SA_DISPELL:
		case	SKID_SA_SPELLBREAKER:
		case	SKID_SA_FLAMELAUNCHER:
		case	SKID_SA_FROSTWEAPON:
		case	SKID_SA_LIGHTNINGLOADER:
		case	SKID_SA_SEISMICWEAPON:
		case	SKID_SA_VOLCANO:
		case	SKID_SA_DELUGE:
		case	SKID_SA_VIOLENTGALE:
		case	SKID_SA_LANDPROTECTOR:
		case	SKID_SA_ABRACADABRA:

		case	SKID_MO_CALLSPIRITS:
		case	SKID_MO_KITRANSLATION:
		case	SKID_MO_ABSORBSPIRITS:
		case	SKID_MO_FINGEROFFENSIVE:
		case	SKID_MO_INVESTIGATE:
#endif
			beginEffectId = EF_BEGINSPELL;
			break;

		case	SKID_SA_ELEMENTWATER:
			beginEffectId = EF_BEGINSPELL2;
			break;
		case	SKID_SA_ELEMENTFIRE:
			beginEffectId = EF_BEGINSPELL3;
			break;
		case	SKID_SA_ELEMENTGROUND:
			beginEffectId = EF_BEGINSPELL4;
			break;
		case	SKID_SA_ELEMENTWIND:
			beginEffectId = EF_BEGINSPELL5;
			break;
		case	SKID_MC_INCCARRY:
		case	SKID_MC_DISCOUNT:
		case	SKID_MC_OVERCHARGE:
		case	SKID_MC_PUSHCART:
		case	SKID_MC_IDENTIFY:
		case	SKID_MC_VENDING:
			break;
		case	SKID_MC_MAMMONITE:
			motionType = ST_ATTACK;
			break;
		case	SKID_AC_OWL:
		case	SKID_AC_VULTURE:
			break;
		case	SKID_AC_CONCENTRATION:
			beginEffectId = EF_INCAGIDEX;
			break;
		case	SKID_AC_DOUBLE:
		case	SKID_HT_POWER:
		case	SKID_HT_PHANTASMIC:
			motionType = ST_ATTACK;
			break;
		case	SKID_AC_SHOWER:
			beginEffectId = -1;
			motionType = ST_ATTACK;
			break;
		case	SKID_TF_DOUBLE:
		case	SKID_TF_MISS:
		case	SKID_TF_STEAL:
		case	SKID_TF_DETOXIFY:
			break;
		case	SKID_TF_HIDING:
			beginEffectId = -1;
			break;
#ifdef	MOB_SKILL
		case	SKID_NPC_PIERCINGATT:
		case	SKID_NPC_MENTALBREAKER:
		case	SKID_NPC_RANGEATTACK:
		case	SKID_NPC_ATTRICHANGE:
		case	SKID_NPC_CHANGEWATER:
		case	SKID_NPC_CHANGEGROUND:
		case	SKID_NPC_CHANGEFIRE:
		case	SKID_NPC_ANTIMAGIC:
		case	SKID_NPC_STONESKIN:
		case	SKID_NPC_WINDATTACK:
		case	SKID_NPC_POISONATTACK:
		case	SKID_NPC_CHANGEWIND:
		case	SKID_NPC_CHANGEPOISON:
		case	SKID_NPC_CHANGEHOLY:
		case	SKID_NPC_CHANGEDARKNESS:
		case	SKID_NPC_CHANGETELEKINESIS:
		case	SKID_NPC_CRITICALSLASH:
		case	SKID_NPC_COMBOATTACK:
		case	SKID_NPC_GUIDEDATTACK:
		case	SKID_NPC_SELFDESTRUCTION:
		case	SKID_NPC_SPLASHATTACK:
		case	SKID_NPC_SUICIDE:
		case	SKID_NPC_POISON:
		case	SKID_NPC_BLINDATTACK:
		case	SKID_NPC_SILENCEATTACK:
		case	SKID_NPC_WIDESILENCE:
		case	SKID_NPC_STUNATTACK:
		case	SKID_NPC_WIDESTUN:
		case	SKID_NPC_WIDESTONE:
		case	SKID_NPC_WIDESLEEP:
		case	SKID_NPC_WIDECONFUSE:
		case	SKID_NPC_PETRIFYATTACK:
		case	SKID_NPC_CURSEATTACK:
		case	SKID_NPC_SLEEPATTACK:
		case	SKID_NPC_RANDOMATTACK:
		case	SKID_NPC_WATERATTACK:
		case	SKID_NPC_GROUNDATTACK:
		case	SKID_NPC_FIREATTACK:
		case	SKID_NPC_FIREBREATH:
		case	SKID_NPC_HOLYATTACK:
		case	SKID_NPC_DARKNESSATTACK:
		case	SKID_NPC_TELEKINESISATTACK:
		case	SKID_NPC_MAGICALATTACK:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKCROSS:
		case	SKID_NPC_UNDEADATTACK:
#endif
			beginEffectId = -1;
			break;
		case	SKID_NPC_SUMMONSLAVE:
		case	SKID_NPC_SUMMONMONSTER:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
#endif
#ifdef	SECONDJOB_SKILL2
		case	SKID_ML_AUTOGUARD:
		case	SKID_CR_AUTOGUARD:
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_MO_EXPLOSIONSPIRITS:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_POWERUP:
#endif
			motionType = ST_STAND;
			beginEffectId = -1;
			break;
		case	SKID_MO_STEELBODY:
			motionType = ST_STAND;
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_MO_CHAINCOMBO:
		case	SKID_MO_COMBOFINISH:
			beginEffectId = -1;
			break;
		case	SKID_MO_EXTREMITYFIST:
#if defined (_UPDATE2005A)
			if (sjob == JT_MONK_H)
				beginEffectId = EF_BEGINASURA11;
			else
#endif
				beginEffectId = EF_BEGINASURA;
			break;
		case	SKID_AM_POTIONPITCHER:
		case	SKID_AM_ACIDTERROR:
		case	SKID_AM_CP_WEAPON:
		case	SKID_AM_CP_SHIELD:
		case	SKID_AM_CP_ARMOR:
		case	SKID_AM_CP_HELM:
		case	SKID_AM_CANNIBALIZE:
		case	SKID_CR_FULLPROTECTION:
			motionType = ST_STAND;
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_AM_CALLHOMUN:
		case	SKID_AM_REST:
		case	SKID_AM_RESURRECTHOMUN:
			motionType = ST_STAND;
			beginEffectId = EF_COUPLECASTING;
			break;
		case	SKID_CR_SLIMPITCHER:
		case	SKID_CR_ACIDDEMONSTRATION:
			motionType = ST_THROW2;
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_ST_CHASEWALK:
			motionType = ST_STAND;
			beginEffectId = EF_CASTSPIN;
			break;
		case	SKID_ST_PRESERVE:
			motionType = ST_STAND;
			beginEffectId = EF_BEGINSPELL7;
			break;
		case	SKID_TF_PICKSTONE :
			beginEffectId = -1;
			motionType = ST_PICKUP;
			break;
		case	SKID_TF_THROWSTONE :
		case	SKID_AS_VENOMKNIFE :
			beginEffectId = -1;
		case	SKID_TF_SPRINKLESAND :
			motionType = ST_ATTACKWAIT;
			break;
#ifdef WEDDING_PATCH
		case	SKID_WE_MALE:
		case	SKID_WE_CALLPARTNER:
		case	SKID_WE_BABY:
		case	SKID_WE_CALLPARENT:
		case	SKID_WE_CALLBABY:
			beginEffectId = EF_COUPLECASTING;
			break;
		case	SKID_WE_FEMALE:
			beginEffectId = EF_HEARTCASTING;
			break;
#endif
		case	SKID_PA_PRESSURE:
		case	SKID_HP_ASSUMPTIO:
		case	SKID_CASH_ASSUMPTIO:
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_PA_SACRIFICE:
			beginEffectId = -1;
			break;
		case	SKID_HP_BASILICA :
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_CH_PALMSTRIKE:
		case	SKID_CH_CHAINCRUSH:
		case	SKID_MO_BALKYOUNG:
			beginEffectId = -1;
			break;
		case	SKID_CH_TIGERFIST:
			beginEffectId = -1;
			break;
		case	SKID_ASC_BREAKER :
		case	SKID_ASC_METEORASSAULT :
		case	SKID_NPC_PULSESTRIKE:
			beginEffectId = EF_BEGINSPELL7;
			break;
		case	SKID_HW_MAGICCRASHER:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_PF_HPCONVERSION:
		case	SKID_PF_SOULCHANGE:
		case	SKID_PF_SOULBURN:
		case	SKID_PF_MINDBREAKER:
		case	SKID_PF_MEMORIZE:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_SN_SHARPSHOOTING:
		case	SKID_CG_ARROWVULCAN:
			motionType = ST_READYSHOT;
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_SN_WINDWALK:
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_LK_SPIRALPIERCE:
#if defined (_UPDATE2004B)
			beginEffectId = EF_PIERCEBODY;
			motionType = ST_ATTACKWAIT;
#else
			beginEffectId = -1;
			motionType = ST_ATTACK;
#endif
			break;
		case	SKID_LK_HEADCRUSH:
		case	SKID_LK_JOINTBEAT:
			beginEffectId = -1;
			break;
		case	SKID_PA_SHIELDCHAIN:
		case	SKID_HW_GANBANTEIN:
		case	SKID_CG_TAROTCARD:
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_HW_NAPALMVULCAN:
		case	SKID_HW_GRAVITATION:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_CH_SOULCOLLECT:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_PF_SPIDERWEB :
		case	SKID_PF_DOUBLECASTING:
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_TK_RUN:

			beginEffectId = EF_BLUECASTING;
			break;
		case	SKID_SG_FEEL:
		case	SKID_SG_HATE:
		case	SKID_SG_FUSION:
			beginEffectId = EF_BLUECASTING;
			break;
		case	SKID_TK_HIGHJUMP:
			motionType = ST_READYJUMP;
			beginEffectId = -1;
			break;
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_AGIUP :
		case	SKID_NPC_SHIELDBRAKE:
		case	SKID_NPC_ARMORBRAKE:
		case	SKID_NPC_HELMBRAKE:
			beginEffectId = EF_BEGINSPELL7;
			break;
#endif
		case	SKID_TK_SEVENWIND:
		case	SKID_TK_MISSION:
			motionType = ST_STAND;
			beginEffectId = EF_BLUECASTING;
			break;
		case	SKID_SL_KAIZEL:
		case	SKID_SL_KAAHI :
		case	SKID_SL_KAUPE :
		case	SKID_SL_KAITE :
		case	SKID_SL_STIN :
		case	SKID_SL_STUN :
		case	SKID_SL_SMA :
		case	SKID_SL_SWOO :
		case	SKID_SL_SKE :
		case	SKID_SL_SKA :
			beginEffectId = EF_BLUECASTING;
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
			beginEffectId = EF_BLUECASTING;
			break;
		case	SKID_AM_BERSERKPITCHER:
			motionType = ST_THROW;
			beginEffectId = EF_BEGINSPELL;
			break;
		case	SKID_AM_TWILIGHT1:
			motionType = ST_SKILL;
			beginEffectId = EF_TWILIGHT1;
			break;
		case	SKID_AM_TWILIGHT2:
			motionType = ST_SKILL;
			beginEffectId = EF_TWILIGHT2;
			break;
		case	SKID_AM_TWILIGHT3:
			motionType = ST_SKILL;
			beginEffectId = EF_TWILIGHT3;
			break;
		case	SKID_BA_PANGVOICE:
			motionType = ST_STAND;
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_KN_CHARGEATK:
			motionType = ST_STAND;
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_PR_REDEMPTIO:
			motionType = ST_STAND;
			beginEffectId = EF_BEGINSPELL6;
			break;
		case	SKID_DC_WINKCHARM:
			motionType = ST_DANCE;
			beginEffectId = -1;
			break;
		case	SKID_NJ_HUUMA:
		case	SKID_NJ_BUNSINJYUTSU:
		case	SKID_NJ_NEN:
			motionType = ST_NINJAREADY;
			beginEffectId = EF_BLUECASTING;
			break;
		case	SKID_NJ_KOUENKA:
		case	SKID_NJ_KAENSIN:
		case	SKID_NJ_BAKUENRYU:
			motionType = ST_NINJAREADY;
			beginEffectId = EF_BEGINSPELL3;
			break;
		case	SKID_NJ_HYOUSENSOU:
		case	SKID_NJ_SUITON:
		case	SKID_NJ_HYOUSYOURAKU:
			motionType = ST_NINJAREADY;
			beginEffectId = EF_BEGINSPELL2;
			break;
		case	SKID_NJ_HUUJIN:
		case	SKID_NJ_RAIGEKISAI:
		case	SKID_NJ_KAMAITACHI:
			motionType = ST_NINJAREADY;
			beginEffectId = EF_BEGINSPELL5;
			break;
		case	SKID_GS_BULLSEYE:
		case	SKID_GS_GATLINGFEVER:
		case	SKID_GS_TRACKING:
		case	SKID_GS_MADNESSCANCEL:
		case	SKID_GS_ADJUSTMENT:
		case	SKID_GS_DISARM:
		case	SKID_GS_PIERCINGSHOT:
		case	SKID_GS_RAPIDSHOWER:
			motionType = ST_GUNREADY;
			beginEffectId = EF_BLUECASTING;
			break;
		case	SKID_GS_DUST:
		case	SKID_GS_FULLBUSTER:
		case	SKID_GS_SPREADATTACK:
		case	SKID_GS_GROUNDDRIFT:
			motionType = ST_GUNREADY2;
			beginEffectId = EF_BLUECASTING;
			break;
		case	SKID_MB_M_TELEPORT:
			motionType = ST_STAND;
			beginEffectId = EF_CASTSPIN;
			break;
#endif
	}

	if (beginEffectId == EF_BEGINSPELL) {
		switch (property) {
			case	PROPERTY_WATER:
				beginEffectId = EF_BEGINSPELL2;
				break;
			case	PROPERTY_FIRE:
				beginEffectId = EF_BEGINSPELL3;
				break;
			case	PROPERTY_GROUND:
				beginEffectId = EF_BEGINSPELL4;
				break;
			case	PROPERTY_WIND:
				beginEffectId = EF_BEGINSPELL5;
				break;
			case	PROPERTY_SAINT:
				beginEffectId = EF_BEGINSPELL6;
				break;
			case	PROPERTY_POISON:
				beginEffectId = EF_BEGINSPELL7;
				break;
		}
	}
}

void	CSession :: LaunchSkillSpecialEffect(int skillId, int level, CGameActor * sActor, CGameActor * tActor)
{
	CGameMode *		gameMode = g_modeMgr.GetGameMode();

	if (sActor==NULL || tActor==NULL)
		return;
















	switch (skillId)
	{
		case	SKID_AM_POTIONPITCHER:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM2);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					effect->SendMsg(NULL, AM_SETCOUNT, level);
				}
				sActor->SetState(ST_SKILL);
			}
			break;
		case	SKID_NPC_CRITICALWOUND:
			{
				if (tActor)
					tActor->LaunchEffect(EF_CRITICALWOUND);
			}
			break;
		case	SKID_NPC_SLOWCAST:
			{
				if (tActor)
					tActor->LaunchEffect(EF_NPC_SLOWCAST);
			}
			break;
		case	SKID_NPC_STONESKIN:
			{
				if (sActor)
					sActor->LaunchEffect(EF_KEEPING);
			}
			break;
		case	SKID_NPC_ANTIMAGIC:
			{
				sActor->LaunchEffect(EF_GUARD3);
			}
			break;
		case	SKID_NPC_FIREBREATH:
			{
				PlayWave("effect\\EF_FireWall.wav");
			}
			break;
		case	SKID_NPC_EXPULSION:
			{
				CGameMode *		gameMode = g_modeMgr.GetGameMode();
				CGameActor * mActor = gameMode->GetWorld()->m_player;
				CRagEffect *	effect = mActor->LaunchEffect(EF_INTIMIDATE);
				if (effect && tActor) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_NPC_WIDESTONE:
			{
				PlayWave("_stonecurse.wav");
			}
			break;
		case	SKID_NPC_WIDESLEEP:
			{
				PlayWave("effect\\widesleep.wav");
			}
			break;
		case	SKID_NPC_WIDECONFUSE:
			{
				PlayWave("amon_ra_die01.wav");
				sActor->LaunchEffect(EF_WIDECONFUSE);
			}
			break;
		case	SKID_NPC_WIDESIGHT:
			{
				sActor->LaunchEffect(EF_SIGHT);
			}
			break;
		case	SKID_NPC_DRAGONFEAR:
			{
				sActor->LaunchEffect(EF_DRAGONFEAR);
			}
			break;
		case	SKID_NPC_WIDEBLEEDING:
			{
				sActor->LaunchEffect(EF_BLEEDING);
			}
			break;
		case	SKID_NPC_PULSESTRIKE:
			{
				sActor->LaunchEffect(EF_SOULBREAKER2);
			}
			break;
		case	SKID_KN_PIERCE: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_PIERCESELF);
			if (effect) {
				effect->SendMsg(NULL, AM_SETCOUNT, level);
				vector3d	pos = sActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
								}
		case	SKID_WZ_JUPITEL: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_YUFITEL);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
								 }
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKTHUNDER: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_YUFITEL);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
									  }
#endif
		case	SKID_WZ_EARTHSPIKE: {
			CRagEffect *	effect = tActor->LaunchEffect(EF_EARTHSPIKE);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
									}
		case	SKID_WZ_WATERBALL: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_WATERBALL2);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
								   }
		case	SKID_KN_SPEARBOOMERANG: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_SPEARBMR);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}

			CRagEffect *	effect2 = sActor->LaunchEffect(EF_SPEARBMRSELF);
			if (effect2) {
				vector3d	pos = sActor->m_pos;
				effect2->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
										}
		case	SKID_WS_CARTTERMINATION:
			{
				if (tActor) {
					tActor->LaunchEffect(EF_CARTTER);
				}
				break;
			}
		case	SKID_LK_SPIRALPIERCE:
			{
#if defined (_UPDATE2004B)
				tActor->LaunchEffect(EF_MAGNUM2);
#else
				CRagEffect *	effect = tActor->LaunchEffect(EF_PIERCESELF);

				if (effect) {
					effect->SendMsg(NULL, AM_SETCOUNT, level);
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
#endif

				break;
			}
		case	SKID_KN_BOWLINGBASH: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_BOWLINGSELF);
			if (effect) {
				vector3d	pos = sActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
									 }
		case	SKID_KN_BRANDISHSPEAR: {
			CRagEffect *	effect = tActor->LaunchEffect(EF_BRANDISHSPEAR);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
									   }
		case	SKID_KN_SPEARSTAB: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_SPEARSTABSELF);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
								   }
		case	SKID_AS_GRIMTOOTH: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_GRIMTOOTH);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
								   }
		case	SKID_AS_SONICBLOW: {
			CRagEffect *	effect = sActor->LaunchEffect(EF_SONICBLOW);
			if (effect) {
				vector3d	pos = tActor->m_pos;
				effect->SendMsg(NULL, AM_SETPOS, int(&pos));
			}

			CRagEffect *	effect2 = sActor->LaunchEffect(EF_SONICBLOW2);
			if (effect2) {
				vector3d	pos = sActor->m_pos;
				effect2->SendMsg(NULL, AM_SETPOS, int(&pos));
			}
			break;
								   }
		case	SKID_PR_TURNUNDEAD: {
										{
											CRagEffect *	effect = sActor->LaunchEffect(EF_TURNUNDEAD);
											if (effect) {
												vector3d	pos = tActor->m_pos;
												effect->SendMsg(NULL, AM_SETPOS, int(&pos));
											}
										}
										break;
									}
		case	SKID_MG_FIREBALL: {
									  {
										  CRagEffect *	effect = sActor->LaunchEffect(EF_FIREBALL);
										  if (effect) {
											  vector3d	pos = tActor->m_pos;
											  effect->SendMsg(NULL, AM_SETPOS, int(&pos));
										  }
									  }
									  break;
								  }
		case	SKID_MG_FROSTDIVER: {
										{
											CRagEffect *	effect = sActor->LaunchEffect(EF_FROSTDIVER);
											if (effect) {
												vector3d	pos = tActor->m_pos;
												effect->SendMsg(NULL, AM_SETPOS, int(&pos));
											}
										}
										break;
									}
		case	SKID_AC_SHOWER: {
			CGameMode *		gameMode = g_modeMgr.GetGameMode();
			sAssert(gameMode);
			gameMode->MakeArrowEffectPos(sActor->m_pos, tActor->m_pos, 18);
			int		cellX, cellY;
			gameMode->GetServerCoor(tActor->m_pos.x, tActor->m_pos.z, cellX, cellY);
			int		offset[8][2] = { {1, 0}, {1, 1}, {1, -1}, {-1, 0}, {-1, 1}, {-1, -1}, {0, 1}, {0, -1}, };
			for (int i=0; i<8; ++i) {
				float	destX, destZ, destY;
				gameMode->GetClientCoor(cellX+offset[i][0], cellY+offset[i][1], destX, destZ);
				destY = gameMode->GetWorld()->m_attr->GetHeight(destX, destZ);
				gameMode->MakeArrowEffectPos(sActor->m_pos, vector3d(destX, destY, destZ), 18);
			}
			break;
								}
		case	SKID_MG_COLDBOLT: {
									  {
										  CRagEffect *	effect = tActor->LaunchEffect(EF_ICEARROW);
										  if (effect)
											  effect->SendMsg(NULL, AM_SETCOUNT, level);
										  break;
									  }
								  }
		case	SKID_MG_FIREBOLT: {
									  {
										  CRagEffect *	effect = tActor->LaunchEffect(EF_FIREARROW);
										  if (effect)
											  effect->SendMsg(NULL, AM_SETCOUNT, level);
										  break;
									  }
								  }
		case	SKID_MG_LIGHTNINGBOLT: {
										   {
											   CRagEffect *	effect = tActor->LaunchEffect(EF_LIGHTBOLT);
											   if (effect)
												   effect->SendMsg(NULL, AM_SETCOUNT, level);
											   break;
										   }
									   }
		case	SKID_MG_THUNDERSTORM:
			{
				tActor->LaunchEffect(EF_THUNDERSTORM);
			}
			break;
#if defined(BALANCE_PATCH)
		case	SKID_MC_CARTREVOLUTION:
			{
				sActor->LaunchEffect(EF_CARTREVOLUTION);
			}
			break;
#endif
#ifdef	MOB_SKILL
		case	SKID_NPC_RANGEATTACK:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_SPEARBMR);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
#endif
#ifdef SECONDJOB_SKILL2
		case	SKID_CR_GRANDCROSS:
			{
				sActor->LaunchEffect(EF_GRANDCROSS);
			}
			break;
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_GRANDDARKNESS:
			{
				sActor->LaunchEffect(EF_GRANDCROSS2);
			}
			break;
#endif
		case	SKID_CR_HOLYCROSS:
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_DARKCROSS:
#endif
			{
				tActor->LaunchEffect(EF_HOLYCROSS);
			}
			break;
		case	SKID_CR_SHIELDCHARGE:
			{
				tActor->LaunchEffect(EF_SHIELDCHARGE);
			}
			break;
		case	SKID_CR_SHIELDBOOMERANG:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_SHIELDBOOMERANG);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_PA_SHIELDCHAIN:
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_SHIELDBOOMERANG3);
			}
			break;
		case	SKID_ITM_TOMAHAWK:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_SHIELDBOOMERANG2);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_RG_BACKSTAP:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_BACKSTAP);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_AM_ACIDTERROR:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_CR_ACIDDEMONSTRATION:
			{
				{
					CGameMode *		gameMode = g_modeMgr.GetGameMode();
					vector3d	pos = tActor->m_pos;
					gameMode->PlaceEffect(&pos, EF_ACIDDEMON,100);
				}
			}
			break;
		case	SKID_TF_SPRINKLESAND:
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_SPRINKLESAND);
			}
			break;
		case	SKID_TF_THROWSTONE:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM3);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_AS_VENOMKNIFE:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM6);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_NJ_SYURIKEN:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM7);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					PlayWave("effect\\\xb4\xd1\xc0\xda_\xb4\xf8\xc1\xf6\xb1\xe2.wav");

				}
			}
			break;
		case	SKID_NJ_KUNAI:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM8);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					PlayWave("effect\\\xb4\xd1\xc0\xda_\xb4\xf8\xc1\xf6\xb1\xe2.wav");

				}
			}
			break;
		case	SKID_NJ_HUUMA:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM9);
				if (effect)
				{
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					PlayWave("effect\\\xb4\xd1\xc0\xda_\xb4\xf8\xc1\xf6\xb1\xe2.wav");

				}
			}
			break;
		case	SKID_NJ_ZENYNAGE:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_THROWITEM10);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					PlayWave("effect\\\xb4\xd1\xc0\xda_\xb4\xf8\xc1\xf6\xb1\xe2.wav");

				}
			}
			break;
		case	SKID_NJ_KOUENKA:
			{
				tActor->LaunchEffect(EF_KOUENKA);
				break;
			}
		case	SKID_NJ_HYOUSENSOU:
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_HYOUSENSOU);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_NJ_HUUJIN:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_STIN4);

				if (effect)
				{
					vector3d	pos;
					pos.y = tActor->GetGID();
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));

					effect->SendMsg(NULL, AM_SETCOUNT, level);
				}
			}
			break;
		case	SKID_NJ_KAMAITACHI:
			{
				{
					CRagEffect *effect = sActor->LaunchEffect(EF_STIN5);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_GS_FLING:
			{
				tActor->LaunchEffect(EF_RED_HIT);
			}
			break;
		case	SKID_NJ_KIRIKAGE:
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_KIRIKAGE);
			}
			break;
		case	SKID_NJ_KASUMIKIRI:
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_KASUMIKIRI);
			}
			break;
		case	SKID_NJ_ISSEN:
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_ISSEN);
			}
			break;
		case	SKID_GS_PIERCINGSHOT:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_CHEMICAL4);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_GS_DUST:
			{
				tActor->LaunchEffect(EF_BASH3D5);
			}
			break;
		case	SKID_GS_FULLBUSTER:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_M02);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_GS_RAPIDSHOWER:
			{
				tActor->LaunchEffect(EF_RAPIDSHOWER);
			}
			break;
		case	SKID_GS_MAGICALBULLET:
			{
				tActor->LaunchEffect(EF_MAGICALBULLET);
			}
			break;
		case	SKID_GS_TRACKING:
			{
				tActor->LaunchEffect(EF_TRACKING);
			}
			break;
		case	SKID_GS_TRIPLEACTION:
			{
				tActor->LaunchEffect(EF_TRIPLEACTION);
			}
			break;
		case	SKID_GS_BULLSEYE:
			{
				tActor->LaunchEffect(EF_BULLSEYE);
			}
			break;
		case	SKID_GS_SPREADATTACK:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_SPREADATTACK);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_RG_INTIMIDATE:
			{
				CGameMode *		gameMode = g_modeMgr.GetGameMode();
				CGameActor * mActor = gameMode->GetWorld()->m_player;
				CRagEffect *	effect = mActor->LaunchEffect(EF_INTIMIDATE);
				if (effect && sActor) {
					vector3d	pos = sActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_MO_EXTREMITYFIST:
			{
				gameMode->SendMsg(MM_QUAKE);

				PlayWave("effect\\mon_\xbe\xc6\xbc\xf6\xb6\xf3 \xc6\xd0\xc8\xb2\xb1\xc7.wav");

				tActor->LaunchEffect(EF_TEIHIT1X);
			}
			break;
		case	SKID_MO_TRIPLEATTACK:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_TRIPLEATTACK);
				if (effect) {
					vector3d	pos = tActor->m_pos;
					effect->SendMsg(NULL, AM_SETPOS, int(&pos));
				}
			}
			break;
		case	SKID_MO_FINGEROFFENSIVE:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_TANJI);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
						effect->SendMsg(NULL, AM_SETCOUNT, level);
					}
				}
			}
			break;
		case	SKID_MO_INVESTIGATE:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_TEIHIT2);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}

					CRagEffect *	effect2 = sActor->LaunchEffect(EF_CHIMTO);
					if (effect2) {
						vector3d	pos = tActor->m_pos;
						effect2->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
#endif
		case	SKID_AL_HEAL:
			{
				tActor->LaunchEffect(EF_HEAL3);
			}
			break;
		case	SKID_PA_PRESSURE:
			{
				CRagEffect *	effect = tActor->LaunchEffect(EF_PRESSURE);
			}
			break;
		case	SKID_LK_HEADCRUSH:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_BASH3D3);
			}
			break;
		case	SKID_LK_JOINTBEAT:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_BASH3D4);
			}
			break;
		case	SKID_PA_SACRIFICE:
			{
				CRagEffect *	effect = sActor->LaunchEffect(EF_BASH3D);
			}
			break;
		case	SKID_ASC_BREAKER:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_SOULBREAKER);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
					gameMode->SendMsg(MM_QUAKE);
					PlayWave("effect\\mon_\xbe\xc6\xbc\xf6\xb6\xf3 \xc6\xd0\xc8\xb2\xb1\xc7.wav");
				}
			}
			break;
		case	SKID_HW_MAGICCRASHER:
			{
				{
					tActor->LaunchEffect(EF_MAGICCRASHER);
				}
			}
			break;
		case	SKID_HW_NAPALMVULCAN:
			{
				{
					tActor->LaunchEffect(EF_NAPALMVALCAN);
				}
			}
			break;
		case	SKID_SN_FALCONASSAULT:
			{
				{
					tActor->LaunchEffect(EF_FALCONASSAULT);
				}
			}
			break;
		case	SKID_SN_SHARPSHOOTING:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_TRIPLEATTACK2);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_CG_ARROWVULCAN:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_TRIPLEATTACK3);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
#if defined(MOB_SKILL_PATCH)
		case	SKID_NPC_SHIELDBRAKE:
			{
				tActor->LaunchEffect(EF_STRIPSHIELD);
			}
			break;
		case	SKID_NPC_ARMORBRAKE:
			{
				tActor->LaunchEffect(EF_STRIPARMOR);
			}
			break;
		case	SKID_NPC_HELMBRAKE:
			{
				tActor->LaunchEffect(EF_STRIPHELM);
			}
			break;
#endif
		case	SKID_TK_DOWNKICK:
			{
				{
					tActor->LaunchEffect(EF_PRESSEDBODY);
					tActor->LaunchEffect(EF_HITLINE6);
				}
			}
			break;
		case	SKID_TK_TURNKICK:
			{
				{
					tActor->LaunchEffect(EF_SPINEDBODY2);
					tActor->LaunchEffect(EF_HITLINE4);
				}
			}
			break;
		case	SKID_TK_COUNTER:
			{
				{
					tActor->LaunchEffect(EF_KICKEDBODY);

					CRagEffect *	effect = tActor->LaunchEffect(EF_HITLINE5);
					if (effect) {
						vector3d	pos = sActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
				}
			}
			break;
		case	SKID_TK_JUMPKICK:
			{
				sActor->LaunchEffect(EF_JUMPKICK);

				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_CHEMICAL3);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
					tActor->LaunchEffect(EF_QUAKEBODY2);
				}
			}
			break;
		case	SKID_SL_STIN:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_STIN);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
					tActor->LaunchEffect(EF_QUAKEBODY3);
				}
			}
			break;
		case	SKID_SL_STUN:
			{
				{
					CRagEffect *	effect = sActor->LaunchEffect(EF_STIN3);
					if (effect) {
						vector3d	pos = tActor->m_pos;
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}
					tActor->LaunchEffect(EF_HITLINE4);
				}
			}
			break;
		case	SKID_SL_SMA:
			{
				{
					tActor->LaunchEffect(EF_4WAYBODY);

					CRagEffect *	effect = sActor->LaunchEffect(EF_STIN2);

					if (effect) {
						vector3d	pos;
						pos.y = tActor->GetGID();
						effect->SendMsg(NULL, AM_SETPOS, int(&pos));
					}

					tActor->LaunchEffect(EF_HITLINE6);
					tActor->LaunchEffect(EF_HITTEXTURE);
				}
			}
			break;
		case	SKID_HFLI_SBR44:
			{
				{
					tActor->LaunchEffect(EF_HFLIMOON3);
					tActor->LaunchEffect(EF_4WAYBODY);
				}

			}
			break;
		case	SKID_HFLI_MOON:
			{
				{
					if ( level == 1 )
						tActor->LaunchEffect(EF_HFLIMOON1);
					else if ( level == 2 )
						tActor->LaunchEffect(EF_HFLIMOON2);
					else
						tActor->LaunchEffect(EF_HFLIMOON3);
				}

			}
			break;
		case	SKID_NPC_EARTHQUAKE:
			{
				if (sActor)
					sActor->LaunchEffect(EF_NPC_EARTHQUAKE);
			}
			break;
	}
}

void	CSession :: InitShortCutTextFromReg()
{
	m_shortcutText.clear();
	m_shortcutText.resize(MAX_EMOTION_TEXT);


	m_shortcutText[0] = MsgStr(MSI_EMOTION_SURPRISE);
	m_shortcutText[1] = MsgStr(MSI_EMOTION_QUESTION);
	m_shortcutText[2] = MsgStr(MSI_EMOTION_DELIGHT);
	m_shortcutText[3] = MsgStr(MSI_EMOTION_THROB);
	m_shortcutText[4] = MsgStr(MSI_EMOTION_SWEAT);
	m_shortcutText[5] = MsgStr(MSI_EMOTION_AHA);
	m_shortcutText[6] = MsgStr(MSI_EMOTION_FRET);
	m_shortcutText[7] = MsgStr(MSI_EMOTION_ANGER);
	m_shortcutText[8] = MsgStr(MSI_EMOTION_MONEY);
	m_shortcutText[9] = MsgStr(MSI_EMOTION_THINK);

#if (_PACKET_VER >= 4)
	char text[256];
	LRESULT lResult;
	HKEY	hResultKey;

	char	EmotionPath[512];
	strcpy(EmotionPath, g_regPath);
	strcat(EmotionPath, g_emotionPath);
	strcat(EmotionPath, "\\");

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, EmotionPath, 0, KEY_QUERY_VALUE, &hResultKey);

	DWORD	REG_TYPE;
	DWORD	DATALENGTH;

	if (lResult == ERROR_SUCCESS) {
		RegCloseKey(hResultKey);
		HKEY	hKey;
		LRESULT lResult1;
		lResult1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, EmotionPath, 0, KEY_ALL_ACCESS, &hKey);
		if (lResult1 == ERROR_SUCCESS) {
			REG_TYPE = REG_SZ;

			char KeyNumber[5];
			for(int a = 0; a < MAX_EMOTION_TEXT; a++) {
				DATALENGTH = 50;
				itoa(a, KeyNumber, 10);
				memset(text, 0, 256);
				RegQueryValueEx(hKey, KeyNumber, NULL, &REG_TYPE, (LPBYTE)text, &DATALENGTH);
				if (strcmp(text, "") != 0) {
					m_shortcutText[a] =  text;
					strcpy(text, m_shortcutText[a].c_str());
				}
			}
			RegCloseKey(hKey);
		}
	}

	if(m_shortcutText[0] == "/!") m_shortcutText[0] = MsgStr(MSI_EMOTION_SURPRISE);
	if(m_shortcutText[1] == "/?") m_shortcutText[1] = MsgStr(MSI_EMOTION_QUESTION);
	if(m_shortcutText[2] == "/\xb1\xe2\xbb\xdd") m_shortcutText[2] = MsgStr(MSI_EMOTION_DELIGHT);
	if(m_shortcutText[3] == "/\xc7\xcf\xc6\xae") m_shortcutText[3] = MsgStr(MSI_EMOTION_THROB);
	if(m_shortcutText[4] == "/\xb6\xa1") m_shortcutText[4] = MsgStr(MSI_EMOTION_SWEAT);
	if(m_shortcutText[5] == "/\xbe\xc6\xc7\xcf") m_shortcutText[5] = MsgStr(MSI_EMOTION_AHA);
	if(m_shortcutText[6] == "/\xc2\xa5\xc1\xf5") m_shortcutText[6] = MsgStr(MSI_EMOTION_FRET);
	if(m_shortcutText[7] == "/\xc8\xad") m_shortcutText[7] = MsgStr(MSI_EMOTION_ANGER);
	if(m_shortcutText[8] == "/\xb5\xb7") m_shortcutText[8] = MsgStr(MSI_EMOTION_MONEY);
	if(m_shortcutText[9] == "/...") m_shortcutText[9] = MsgStr(MSI_EMOTION_THINK);
#endif
}

BOOL	CSession :: IsMyParty(int AID)
{
	CGameMode *curMode = g_modeMgr.GetGameMode();
	if (curMode->GetWorld()->IsPKZone() || curMode->GetWorld()->IsEventPVPMode()) {
		for (int a = 0; a < GetNumParty(); a++) {
			FRIEND_INFO fi = GetPartyInfoBySequence(a);
			if (fi.AID == AID) {
				return TRUE;
			}
		}
	}
	return FALSE;
}



char *CSession :: GetStateFileName(int statusId, int priority)
{
#if defined(BALANCE_PATCH)
	switch(priority) {
		case 0:
			switch(statusId) {
		case EFST_DEC_AGI:
			return "effect\\\271\316\303\270\274\272\260\250\274\322.tga";
		case EFST_BROKENARMOR:
			return "effect\\\260\251\277\312\306\304\261\253.tga";
		case EFST_ILLUSION:
			return "effect\\\310\257\260\242.tga";
		case EFST_QUAGMIRE:
			return "effect\\\304\342\261\327\270\266\300\314\276\356.tga";
		case EFST_BROKENWEAPON:
			return "effect\\\271\253\261\342\306\304\261\253.tga";
		case EFST_WEIGHTOVER50:
			return "effect\\\271\253\260\32450\300\314\273\363.tga";
		case EFST_WEIGHTOVER90:
			return "effect\\\271\253\260\32490\300\314\273\363.tga";
		case EFST_NOEQUIPWEAPON:
			return "effect\\\275\272\306\256\270\263\277\376\306\371.tga";
		case EFST_NOEQUIPSHIELD:
			return "effect\\\275\272\306\256\270\263\275\257\265\345.tga";
		case EFST_NOEQUIPARMOR:
			return "effect\\\275\272\306\256\270\263\276\306\270\323.tga";
		case EFST_NOEQUIPHELM:
			return "effect\\\275\272\306\256\270\263\307\357\270\247.tga";
		case EFST_PROPERTYFIRE:
			if ( g_session.GetJob() >= JT_TAEKWON && g_session.GetJob() <= JT_LINKER )
				return "effect\\i_p_FIRE.tga";
			return "effect\\\307\301\267\271\300\323\267\261\303\304.tga";
		case EFST_STATUSONE:
			return "effect\\\261\333\267\316\270\256\276\306.tga";
		case EFST_PARTYFLEE:
			return "effect\\gogi.tga";
#ifdef SAVE_FOOD_PARAM
		case	EFST_FOOD_INT:
		case	EFST_FOOD_INT_CASH:
			return "effect\\Int_gogi.tga";
		case	EFST_FOOD_STR:
		case	EFST_FOOD_STR_CASH:
			return "effect\\Str_gogi.tga";
		case	EFST_FOOD_AGI:
		case	EFST_FOOD_AGI_CASH:
			return "effect\\Agi_gogi.tga";
		case	EFST_FOOD_VIT:
		case	EFST_FOOD_VIT_CASH:
			return "effect\\Vit_gogi.tga";
		case	EFST_FOOD_DEX:
		case	EFST_FOOD_DEX_CASH:
			return "effect\\Dex_gogi.tga";
		case	EFST_FOOD_LUK:
		case	EFST_FOOD_LUK_CASH:
			return "effect\\Luk_gogi.tga";
		case	EFST_FOOD_BASICAVOIDANCE:
		case	EFST_FOOD_BASICHIT:
		case	EFST_FOOD_CRITICALSUCCESSVALUE:
			return "effect\\gogi.tga";
		case	EFST_CASH_PLUSEXP:
			return "effect\\exp.tga";
		case	EFST_CASH_DEATHPENALTY:
			return "effect\\death.tga";
		case	EFST_CASH_RECEIVEITEM:
			return "effect\\item.tga";
		case	EFST_CASH_PLUSONLYJOBEXP:
			return "effect\\job.tga";
#endif
			}
			break;
		case 1:
			switch(statusId) {
		case EFST_ENDURE:
			return "effect\\\300\316\265\340\276\356.tga";
		case EFST_ENCHANTPOISON:
			return "effect\\\300\316\302\371\306\256\306\367\300\314\301\360.tga";
		case EFST_CLOAKING:
			return "effect\\\305\254\267\316\305\267.tga";
		case EFST_TWOHANDQUICKEN:
			return "effect\\\305\365\307\332\265\345\304\373\305\253.tga";
		case EFST_MAXIMIZE:
			return "effect\\\270\306\275\303\270\266\300\314\301\356\306\304\277\366.tga";
		case EFST_ADRENALINE:
		case EFST_ADRENALINE2:
			return "effect\\\276\306\265\345\267\271\263\257\270\260\267\257\275\254.tga";
		case EFST_POISONREACT:
			return "effect\\\306\367\300\314\301\360\270\256\276\327\306\256.tga";
		case EFST_AUTOGUARD :
			return "effect\\\277\300\305\344\260\241\265\345.tga";
		case EFST_AUTOSPELL :
			return "effect\\\277\300\305\344\275\272\306\347.tga";
		case EFST_PROVOKE:
			return "effect\\\307\301\267\316\272\270\305\251.tga";
		case EFST_OVERTHRUST:
			return "effect\\\277\300\271\366\306\256\267\257\275\272\306\256.tga";
		case EFST_WEAPONPERFECT:
			return "effect\\\277\376\306\371\306\333\306\345\274\307.tga";
		case EFST_HIDING:
			return "effect\\\307\317\300\314\265\371.tga";
		case EFST_SPEARQUICKEN:
			return "effect\\\275\272\307\307\276\356\304\373\305\253.tga";
		case EFST_DEFENDER:
			return "effect\\\265\360\306\346\264\365.tga";
		case EFST_REFLECTSHIELD:
			return "effect\\\270\256\307\303\267\272\306\256\275\257\265\345.tga";
		case EFST_PROPERTYWATER:
			if ( g_session.GetJob() >= JT_TAEKWON && g_session.GetJob() <= JT_LINKER )
				return "effect\\i_p_WATER.tga";
			return "effect\\\307\301\267\316\275\272\306\256\277\376\306\371.tga";
		case EFST_PROPERTYWIND:
			if ( g_session.GetJob() >= JT_TAEKWON && g_session.GetJob() <= JT_LINKER )
				return "effect\\i_p_WIND.tga";
			return "effect\\\266\363\300\314\306\256\264\327\267\316\264\365.tga";
		case	EFST_BLOODING:
			return "effect\\\303\342\307\367\273\363\305\302.tga";
		case	EFST_JOINTBEAT:
			return "effect\\\260\374\300\375\260\370\260\335.tga";
		case	EFST_PROTECTEXP:
		case	EFST_ANGEL_PROTECT:
			return "effect\\\276\366\270\266\276\306\272\374\273\347\266\373\307\330\277\344.tga";
		case EFST_PARRYING :
			return "effect\\ICON10.tga";
		case EFST_TENSIONRELAX :
			return "effect\\ICON08.tga";
		case EFST_ASSUMPTIO :
			return "effect\\ICON05.tga";


		case EFST_TRUESIGHT :
			return "effect\\ICON09.tga";
		case EFST_WINDWALK :
			return "effect\\ICON06.tga";
		case EFST_CARTBOOST :
			return "effect\\ICON07.tga";
		case EFST_MELTDOWN :
			return "effect\\ICON03.tga";
		case EFST_SWORDREJECT:
			return "effect\\ICON04.tga";
		case EFST_MARIONETTE_MASTER :
			return "effect\\ICON01.tga";
		case EFST_MARIONETTE:
			return "effect\\ICON01.tga";
		case EFST_AUTOBERSERK:
			return "effect\\\261\335\260\255\272\322\261\253.tga";




		case	EFST_STORMKICK_ON:
			return "effect\\i_STORMKICK.tga";
		case	EFST_DOWNKICK_ON:
			return "effect\\i_DOWNKICK.tga";
		case	EFST_TURNKICK_ON:
			return "effect\\i_TURNKICK.tga";
		case	EFST_COUNTER_ON:
			return "effect\\i_COUNTER.tga";
		case	EFST_DODGE_ON:
			return "effect\\i_DODGE.tga";
		case EFST_STRUP :
			return "effect\\i_RUN.tga";
		case EFST_PROPERTYDARK:
			return "effect\\i_p_DARK.tga";
		case EFST_PROPERTYTELEKINESIS:
			return "effect\\i_p_TELE.tga";
		case EFST_EDP :
#if defined (_UPDATE2004B)
			return "effect\\I_\304\241\265\266.tga";
#endif
		case EFST_PRESERVE:
			return "effect\\I_\307\301\270\256\300\372\272\352.tga";
		case EFST_CHASEWALK2:
			return "effect\\I_\303\274\300\314\275\272\310\373.tga";
		case EFST_GROUNDMAGIC:
			return "effect\\I_\300\345\306\307.tga";
		case EFST_DOUBLECASTING:
			return "effect\\I_\264\365\272\355\304\263\275\272\306\303.tga";
		case EFST_OVERTHRUSTMAX:
			return "effect\\I_\277\300\271\366\270\306\275\272.tga";
		case EFST_TAROTCARD:
			return "effect\\I_\305\270\267\316\304\253\265\345.tga";
		case EFST_SUN_COMFORT:
			return "effect\\i_SUNCOMFORT.tga";
		case EFST_MOON_COMFORT:
			return "effect\\i_MOONCOMFORT.tga";
		case EFST_STAR_COMFORT:
			return "effect\\i_STARCOMFORT.tga";
		case EFST_KAIZEL:
			return "effect\\i_KAIZEL.tga";
		case	EFST_KAAHI:
			return "effect\\i_KAAHI.tga";
		case	EFST_KAUPE:
			return "effect\\i_KAUPE.tga";
		case EFST_ONEHANDQUICKEN:
			return "effect\\i_ONEHAND.tga";


		case EFST_CR_SHRINK:
			return "effect\\i_shrink.tga";
		case EFST_WZ_SIGHTBLASTER:
			return "effect\\i_sightblaster.tga";


		case EFST_RG_CCONFINE_M:
		case EFST_RG_CCONFINE_S:
			return "effect\\i_closeconfine.tga";
		case	EFST_EARTHSCROLL:
			return "effect\\\261\333\267\316\270\256\276\306.tga";
		case	EFST_NJ_BUNSINJYUTSU:
			return "effect\\i_BUNSIN.tga";
		case	EFST_NJ_UTSUSEMI:
			return "effect\\i_MAEMI.tga";
		case	EFST_NJ_NEN:
			return "effect\\i_NEN.tga";


		case	EFST_GS_ADJUSTMENT:
			return "effect\\i_ADJUSTMENT.tga";
		case	EFST_GS_ACCURACY:
			return "effect\\i_ACCURACY.tga";
		case	EFST_GS_MADNESSCANCEL:
			return "effect\\i_MADNESS.tga";
		case	EFST_GS_GATLINGFEVER:
			return "effect\\i_FEVER.tga";

		case	EFST_ATKER_BLOOD:
			return "effect\\SP\274\322\270\360\267\256\260\250\274\322\306\367\274\307.tga";
		case	EFST_TARGET_BLOOD:
			return "effect\\\277\303\273\363\305\302\300\372\307\327\306\367\274\307.tga";
		case	EFST_PET:
		case	EFST_MENTAL:
		case	EFST_EXPMEMORY:
		case	EFST_PERFORMANCE:
		case	EFST_GAIN:
		case	EFST_GRIFFON:
		case	EFST_DRIFT:
		case	EFST_PROPERTYUNDEAD:
		case	EFST_WALLSHIFT:
		case	EFST_REINCARNATION:

		case	EFST_PATTACK:
		case	EFST_PSPEED:
		case	EFST_PDEFENSE:
		case	EFST_PCRITICAL:
		case	EFST_RANKING:
		case	EFST_PTRIPLE:
		case	EFST_DENERGY:
		case	EFST_WAVE1:
		case	EFST_WAVE2:
		case	EFST_WAVE3:
		case	EFST_WAVE4:
		case	EFST_DAURA:
		case	EFST_DFREEZER:
		case	EFST_DPUNISH:
		case	EFST_DBARRIER:
		case	EFST_DWARNING:
		case	EFST_DACCEL:
		case	EFST_DBLOCK:

		case	EFST_DA_ENERGY:
		case	EFST_DA_BLACK:
		case	EFST_DA_MAGICCART:
		case	EFST_CRYSTAL:
		case	EFST_DA_FIRSTSLOT:
		case	EFST_DA_HEADDEF:
		case	EFST_DA_SPACE:
		case	EFST_DA_TRANSFORM:
		case	EFST_DA_ITEMREBUILD:
		case	EFST_DA_ILLUSION:
		case	EFST_DA_DARKPOWER:
		case	EFST_DA_EARPLUG:
		case	EFST_DA_REBUILD:
		case	EFST_DA_EDARKNESS:
		case	EFST_DA_EGUARDIAN:
		case	EFST_DA_TIMEOUT:
			return "effect\\\261\333\267\316\270\256\276\306.tga";
		case	EFST_DA_CONTRACT:
			return "effect\\\301\327\300\272\303\264\307\317\261\342.tga";
		case	EFST_SLOWCAST:
			return "effect\\slowcast.tga";
		case	EFST_CRITICALWOUND:
			return "effect\\criticalwound.tga";
		case	EFST_HELLPOWER:
			return "effect\\npc_hellpower.tga";

		case	EFST_CRITICALPERCENT:
			return "effect\\\277\254\270\266\301\246.tga";
		case	EFST_HEALPLUS:
			return "effect\\\310\270\272\271\267\302\307\342\273\363\306\367\274\307.tga";
		case	EFST_S_LIFEPOTION:
			return "effect\\\274\322\307\374\273\375\270\355\274\366.tga";
		case	EFST_L_LIFEPOTION:
			return "effect\\\301\337\307\374\273\375\270\355\274\366.tga";
		case	EFST_PLUSAVOIDVALUE:
			return "effect\\\310\257\277\265\300\307\274\372\300\334.tga";
		case	EFST_PROTECT_DEF:
			return "effect\\\271\260\270\256\271\346\276\356\306\367\274\307.tga";
		case	EFST_PROTECT_MDEF:
			return "effect\\\270\266\271\375\271\346\276\356\306\367\274\307.tga";
		case	EFST_ARMOR_PROPERTY:
			return "effect\\\304\263\275\303\260\251\277\312\274\323\274\272\272\316\277\251.tga";
			}
			break;
		case 2:
			switch(statusId) {
		case EFST_CONCENTRATION:
			return "effect\\\301\375\301\337\267\302\307\342\273\363.tga";
		case EFST_TRICKDEAD:
			return "effect\\\301\327\300\272\303\264\307\317\261\342.tga";
		case EFST_FALCON:
			return "effect\\\306\310\304\334.tga";
		case EFST_RIDING:
			return "effect\\\266\363\300\314\265\371.tga";
		case EFST_SHOUT:
			return "effect\\\260\355\274\272\301\366\270\243\261\342.tga";
		case EFST_ENERGYCOAT:
			return "effect\\\277\241\263\312\301\366\304\332\306\256.tga";
		case	EFST_ATTHASTE_INFINITY:
		case	EFST_ATTHASTE_POTION1:
		case	EFST_ATTHASTE_POTION2:
		case	EFST_ATTHASTE_POTION3:
			return "effect\\\260\370\274\323\271\260\276\340.tga";
		case EFST_PROTECTWEAPON:
			return "effect\\\304\311\271\314\304\303\307\301\267\316\305\330\274\307[\277\376\306\371].tga";
		case EFST_PROTECTSHIELD:
			return "effect\\\304\311\271\314\304\303\307\301\267\316\305\330\274\307[\275\257\265\345].tga";
		case EFST_PROTECTARMOR:
			return "effect\\\304\311\271\314\304\303\307\301\267\316\305\330\274\307[\276\306\270\323].tga";
		case EFST_PROTECTHELM:
			return "effect\\\304\311\271\314\304\303\307\301\267\316\305\330\274\307[\307\357\270\247].tga";
		case EFST_PROPERTYGROUND:
			if ( g_session.GetJob() >= JT_TAEKWON && g_session.GetJob() <= JT_LINKER )
				return "effect\\i_p_EARTH.tga";
			return "effect\\\273\347\300\314\301\356\271\315\277\376\306\371.tga";

			}
			break;
		case 3:
			switch(statusId) {
		case	EFST_GLORIA:
			return "effect\\\261\333\267\316\270\256\276\306.tga";
		case	EFST_KYRIE:
			return "effect\\\261\342\270\256\277\241\277\244\267\271\300\314\274\325.tga";
		case	EFST_LEXAETERNA:
			return "effect\\\267\272\275\272\277\241\305\327\270\243\263\252.tga";
		case	EFST_CRUCIS:
			return "effect\\\275\303\261\327\263\321\305\251\267\347\275\303\275\272.tga";
		case	EFST_MAGNIFICAT:
			return "effect\\\270\266\264\317\307\307\304\261.tga";
		case	EFST_MOVHASTE_INFINITY:
		case	EFST_MOVHASTE_POTION:
		case	EFST_INC_AGI:
		case	EFST_MOVHASTE_HORSE:
			return "effect\\\271\316\303\270\274\272\301\365\260\241.tga";
		case	EFST_BLESSING:
			return "effect\\\272\355\267\271\275\314.tga";
		case	EFST_BENEDICTIO:
			return "effect\\\274\272\303\274\260\255\272\271.tga";
		case	EFST_SUFFRAGIUM:
			return "effect\\\274\366\307\301\266\363\261\342\277\362.tga";
		case	EFST_SLOWPOISON:
			return "effect\\\275\275\267\316\277\354\306\367\300\314\301\360.tga";
		case	EFST_ASPERSIO:
		case	EFST_WEAPONPROPERTY:
			if ( g_session.GetJob() >= JT_TAEKWON && g_session.GetJob() <= JT_LINKER )
				return "effect\\i_p_SAINT.tga";
			return "effect\\\276\306\275\272\306\344\270\243\275\303\277\300.tga";
		case	EFST_ANGELUS:
			return "effect\\\276\310\301\251\267\347\275\272.tga";
		case	EFST_IMPOSITIO:
			return "effect\\\300\323\306\367\275\303\306\274\277\300\270\266\264\251\275\272.tga";
		case	EFST_PROVIDENCE:
			return "effect\\\275\305\300\307\266\346.tga";

			}
			break;
	}
#endif
	return NULL;
}

int CSession::GetEffectIgnoreCnt(int count)
{
	int ig = 0;

	if (count > 40) {
		ig = 4;
	} else if (count > 20) {
		ig = 3;
	} else if (count > 10) {
		ig = 2;
	} else if (count == 10) {
		ig = 2;
	} else
		ig = 1;

	return ig;
}

void	CSession :: InitPetTables()
{

	m_petEmotionTable[0][0][0] = ET_HNG;
	m_petEmotionTable[0][0][1] = ET_HNG;
	m_petEmotionTable[0][0][2] = ET_KIK;
	m_petEmotionTable[0][0][3] = ET_KIK;
	m_petEmotionTable[0][0][4] = ET_FRET;
	m_petEmotionTable[0][0][5] = ET_HUK;
	m_petEmotionTable[0][1][6] = ET_CRY;

	m_petEmotionTable[0][1][0] = ET_HNG;
	m_petEmotionTable[0][1][1] = ET_HNG;
	m_petEmotionTable[0][1][2] = ET_KIK;
	m_petEmotionTable[0][1][3] = ET_KIK;
	m_petEmotionTable[0][1][4] = ET_FRET;
	m_petEmotionTable[0][1][5] = ET_FRET;
	m_petEmotionTable[0][1][6] = ET_SWEAT;

	m_petEmotionTable[0][2][0] = ET_SCRATCH;
	m_petEmotionTable[0][2][1] = ET_HNG;
	m_petEmotionTable[0][2][2] = -1;
	m_petEmotionTable[0][2][3] = ET_HNG;
	m_petEmotionTable[0][2][4] = ET_SWEAT;
	m_petEmotionTable[0][2][5] = ET_SCRATCH;
	m_petEmotionTable[0][2][6] = ET_THINK;

	m_petEmotionTable[0][3][0] = ET_OK;
	m_petEmotionTable[0][3][1] = -1;
	m_petEmotionTable[0][3][2] = -1;
	m_petEmotionTable[0][3][3] = ET_THINK;
	m_petEmotionTable[0][3][4] = ET_SCRATCH;
	m_petEmotionTable[0][3][5] = ET_THINK;
	m_petEmotionTable[0][3][6] = ET_AHA;

	m_petEmotionTable[0][4][0] = ET_SMILE;
	m_petEmotionTable[0][4][1] = -1;
	m_petEmotionTable[0][4][2] = -1;
	m_petEmotionTable[0][4][3] = ET_SCRATCH;
	m_petEmotionTable[0][4][4] = ET_OK;
	m_petEmotionTable[0][4][5] = ET_DELIGHT;
	m_petEmotionTable[0][4][6] = ET_AHA;


	m_petEmotionTable[1][0][0] = ET_SCRATCH;
	m_petEmotionTable[1][0][1] = ET_HNG;
	m_petEmotionTable[1][0][2] = ET_KIK;
	m_petEmotionTable[1][0][3] = ET_KIK;
	m_petEmotionTable[1][0][4] = ET_HNG;
	m_petEmotionTable[1][0][5] = ET_HNG;
	m_petEmotionTable[1][1][6] = ET_SWEAT;

	m_petEmotionTable[1][1][0] = ET_SCRATCH;
	m_petEmotionTable[1][1][1] = ET_HNG;
	m_petEmotionTable[1][1][2] = -1;
	m_petEmotionTable[1][1][3] = ET_HNG;
	m_petEmotionTable[1][1][4] = ET_KIK;
	m_petEmotionTable[1][1][5] = ET_HNG;
	m_petEmotionTable[1][1][6] = ET_HNG;

	m_petEmotionTable[1][2][0] = ET_OK;
	m_petEmotionTable[1][2][1] = -1;
	m_petEmotionTable[1][2][2] = -1;
	m_petEmotionTable[1][2][3] = ET_THINK;
	m_petEmotionTable[1][2][4] = ET_SCRATCH;
	m_petEmotionTable[1][2][5] = ET_THINK;
	m_petEmotionTable[1][2][6] = ET_AHA;

	m_petEmotionTable[1][3][0] = ET_SMILE;
	m_petEmotionTable[1][3][1] = -1;
	m_petEmotionTable[1][3][2] = -1;
	m_petEmotionTable[1][3][3] = ET_SCRATCH;
	m_petEmotionTable[1][3][4] = ET_OK;
	m_petEmotionTable[1][3][5] = ET_DELIGHT;
	m_petEmotionTable[1][3][6] = ET_AHA;

	m_petEmotionTable[1][4][0] = ET_THANKS;
	m_petEmotionTable[1][4][1] = -1;
	m_petEmotionTable[1][4][2] = -1;
	m_petEmotionTable[1][4][3] = ET_PROFUSELY_SWEAT;
	m_petEmotionTable[1][4][4] = ET_THROB;
	m_petEmotionTable[1][4][5] = ET_DELIGHT;
	m_petEmotionTable[1][4][6] = ET_BEST;


	m_petEmotionTable[2][0][0] = ET_HNG;
	m_petEmotionTable[2][0][1] = ET_HNG;
	m_petEmotionTable[2][0][2] = -1;
	m_petEmotionTable[2][0][3] = ET_HNG;
	m_petEmotionTable[2][0][4] = ET_KIK;
	m_petEmotionTable[2][0][5] = ET_SWEAT;
	m_petEmotionTable[2][1][6] = ET_HNG;

	m_petEmotionTable[2][1][0] = ET_SCRATCH;
	m_petEmotionTable[2][1][1] = -1;
	m_petEmotionTable[2][1][2] = -1;
	m_petEmotionTable[2][1][3] = ET_THINK;
	m_petEmotionTable[2][1][4] = ET_THINK;
	m_petEmotionTable[2][1][5] = ET_THINK;
	m_petEmotionTable[2][1][6] = ET_SCRATCH;

	m_petEmotionTable[2][2][0] = ET_OK;
	m_petEmotionTable[2][2][1] = -1;
	m_petEmotionTable[2][2][2] = -1;
	m_petEmotionTable[2][2][3] = ET_SCRATCH;
	m_petEmotionTable[2][2][4] = ET_OK;
	m_petEmotionTable[2][2][5] = ET_DELIGHT;
	m_petEmotionTable[2][2][6] = ET_AHA;

	m_petEmotionTable[2][3][0] = ET_SMILE;
	m_petEmotionTable[2][3][1] = -1;
	m_petEmotionTable[2][3][2] = -1;
	m_petEmotionTable[2][3][3] = ET_PROFUSELY_SWEAT;
	m_petEmotionTable[2][3][4] = ET_SMILE;
	m_petEmotionTable[2][3][5] = ET_DELIGHT;
	m_petEmotionTable[2][3][6] = ET_BEST;

	m_petEmotionTable[2][4][0] = ET_THANKS;
	m_petEmotionTable[2][4][1] = ET_BEST;
	m_petEmotionTable[2][4][2] = -1;
	m_petEmotionTable[2][4][3] = ET_HUK;
	m_petEmotionTable[2][4][4] = ET_BIGTHROB;
	m_petEmotionTable[2][4][5] = ET_OK;
	m_petEmotionTable[2][4][6] = ET_BEST;


	m_petEmotionTable[3][0][0] = ET_HNG;
	m_petEmotionTable[3][0][1] = -1;
	m_petEmotionTable[3][0][2] = -1;
	m_petEmotionTable[3][0][3] = ET_THINK;
	m_petEmotionTable[3][0][4] = ET_THINK;
	m_petEmotionTable[3][0][5] = ET_THINK;
	m_petEmotionTable[3][1][6] = ET_SCRATCH;

	m_petEmotionTable[3][1][0] = ET_HNG;
	m_petEmotionTable[3][1][1] = -1;
	m_petEmotionTable[3][1][2] = -1;
	m_petEmotionTable[3][1][3] = ET_SCRATCH;
	m_petEmotionTable[3][1][4] = ET_SCRATCH;
	m_petEmotionTable[3][1][5] = ET_SCRATCH;
	m_petEmotionTable[3][1][6] = ET_THINK;

	m_petEmotionTable[3][2][0] = ET_SCRATCH;
	m_petEmotionTable[3][2][1] = -1;
	m_petEmotionTable[3][2][2] = -1;
	m_petEmotionTable[3][2][3] = ET_PROFUSELY_SWEAT;
	m_petEmotionTable[3][2][4] = ET_BEST;
	m_petEmotionTable[3][2][5] = ET_DELIGHT;
	m_petEmotionTable[3][2][6] = ET_AHA;

	m_petEmotionTable[3][3][0] = ET_OK;
	m_petEmotionTable[3][3][1] = ET_BEST;
	m_petEmotionTable[3][3][2] = ET_PROFUSELY_SWEAT;
	m_petEmotionTable[3][3][3] = ET_HUK;
	m_petEmotionTable[3][3][4] = ET_THROB;
	m_petEmotionTable[3][3][5] = ET_OK;
	m_petEmotionTable[3][3][6] = ET_BEST;

	m_petEmotionTable[3][4][0] = ET_SMILE;
	m_petEmotionTable[3][4][1] = ET_BEST;
	m_petEmotionTable[3][4][2] = ET_HELP;
	m_petEmotionTable[3][4][3] = ET_CRY;
	m_petEmotionTable[3][4][4] = ET_CHUP;
	m_petEmotionTable[3][4][5] = ET_STARE_ABOUT;
	m_petEmotionTable[3][4][6] = ET_SMILE;


	m_petEmotionTable[4][0][0] = ET_HUK;
	m_petEmotionTable[4][0][1] = -1;
	m_petEmotionTable[4][0][2] = -1;
	m_petEmotionTable[4][0][3] = ET_SCRATCH;
	m_petEmotionTable[4][0][4] = ET_SCRATCH;
	m_petEmotionTable[4][0][5] = ET_SCRATCH;
	m_petEmotionTable[4][1][6] = ET_MONEY;

	m_petEmotionTable[4][1][0] = ET_HNG;
	m_petEmotionTable[4][1][1] = -1;
	m_petEmotionTable[4][1][2] = -1;
	m_petEmotionTable[4][1][3] = ET_PROFUSELY_SWEAT;
	m_petEmotionTable[4][1][4] = ET_SCRATCH;
	m_petEmotionTable[4][1][5] = ET_SURPRISE;
	m_petEmotionTable[4][1][6] = ET_SURPRISE;

	m_petEmotionTable[4][2][0] = ET_HNG;
	m_petEmotionTable[4][2][1] = ET_BEST;
	m_petEmotionTable[4][2][2] = ET_PROFUSELY_SWEAT;
	m_petEmotionTable[4][2][3] = ET_HUK;
	m_petEmotionTable[4][2][4] = ET_SMILE;
	m_petEmotionTable[4][2][5] = ET_OK;
	m_petEmotionTable[4][2][6] = ET_BEST;

	m_petEmotionTable[4][3][0] = ET_SCRATCH;
	m_petEmotionTable[4][3][1] = ET_BEST;
	m_petEmotionTable[4][3][2] = ET_HELP;
	m_petEmotionTable[4][3][3] = ET_CRY;
	m_petEmotionTable[4][3][4] = ET_THROB;
	m_petEmotionTable[4][3][5] = ET_SMILE;
	m_petEmotionTable[4][3][6] = ET_SMILE;

	m_petEmotionTable[4][4][0] = ET_SCRATCH;
	m_petEmotionTable[4][4][1] = ET_BEST;
	m_petEmotionTable[4][4][2] = ET_HELP;
	m_petEmotionTable[4][4][3] = ET_CRY;
	m_petEmotionTable[4][4][4] = ET_CHUP;
	m_petEmotionTable[4][4][5] = ET_SMILE;
	m_petEmotionTable[4][4][6] = ET_CHUP;
}

void	CSession :: InitPetTalkTable()
{
	CFile fp;
	fp.Open(g_language->LanguageFile("PetTalkTable.xml"), CFile::modeRead);
	m_petTalkXmlTable.ReadDocument((const char*)fp.GetBuf(), (const char*)fp.GetBuf() + fp.GetLength());
	fp.Close();

	CFile fp2;
	if (fp2.Open("MonsterSkillInfo.xml", CFile::modeRead)) {
		m_monsterSkillInfo.ReadDocument((const char*)fp2.GetBuf(), (const char*)fp2.GetBuf() + fp2.GetLength());
		fp2.Close();
	}

	CFile fp3;
	if (fp3.Open("MonsterTalkTable.xml", CFile::modeRead)) {
		m_monsterTalkTable.ReadDocument((const char*)fp3.GetBuf(), (const char*)fp3.GetBuf() + fp3.GetLength());
		fp3.Close();
	}
}

#include "rag_budget_sites.h"
void	CSession :: GetPetSpeak(int petJob, int hunger, int act, char *speak)
{
	XMLElement *petEliment;
	XMLElement *actEliment;
	XMLElement *childEliment;
	char tmpStr[256];

	speak[0] = 0;

	strcpy( tmpStr, "monster_talk_table" );
	actEliment = m_petTalkXmlTable.FindElement(DataTxtDecode(tmpStr));
	if (!actEliment) return;

	strcpy( tmpStr, g_session.GetPetName(petJob) );
	petEliment = actEliment->FindChild(DataTxtDecode(tmpStr));
	if (!petEliment) return;

	strcpy( tmpStr, g_session.GetPetHungryText(hunger) );

	actEliment = petEliment->FindChild(DataTxtDecode(tmpStr));
	if (!actEliment) return;

	strcpy( tmpStr, g_session.GetPetActText(act) );
	childEliment = actEliment->FindChild(DataTxtDecode(tmpStr));
	if (!childEliment) return;

	std::vector<char *> speakTable;
	while (childEliment) {
		speakTable.push_back((char *)childEliment->GetContents().c_str());
		strcpy( tmpStr, g_session.GetPetActText(act) );
		childEliment = childEliment->FindNext(DataTxtDecode(tmpStr));
	}

	int size = speakTable.size(), randCnt = 0;
	if (size) {
		randCnt = rand() % size;
		strcpy(speak, speakTable[randCnt]);
		DataTxtDecode(speak);
		speakTable.clear(); RAG_BUDGET_SITES(1)
		return;
	}

	return;
}

void	CSession :: GetMonsterSkillInfo(char *Name, int skillId, int &act, int &loopType)
{
	XMLElement *monsterElement;
	XMLElement *skillElement;
	XMLElement *actElement;
	XMLElement *loopTypeElement;

	act = -1;

	char	tmpStr[256];
	strcpy( tmpStr, "Monster_Action_Table");
#if (_PACKET_VER < 10)
	skillElement = m_monsterSkillInfo.FindElement(DataTxtDecode(tmpStr));
#else
	skillElement = m_monsterSkillInfo.FindElement(tmpStr);
#endif
	if (!skillElement) {
		return;
	}

	char pSkillId[8], pAct[8], pLoopType[8], pName[256];
	itoa(skillId, pAct,10);
	sprintf(pSkillId, "_%s", pAct);
	strcpy(pName, Name);
	_strlwr(pName);
#if (_PACKET_VER < 10)
	monsterElement = skillElement->FindChild(DataTxtDecode(pName));
#else
	monsterElement = skillElement->FindChild(pName);
#endif
	if (!monsterElement)
		return;
#if (_PACKET_VER < 10)
	skillElement = monsterElement->FindChild(DataTxtDecode(pSkillId));
#else
	skillElement = monsterElement->FindChild(pSkillId);
#endif
	if (!skillElement)
		return;

	strcpy( tmpStr, "action" );
#if (_PACKET_VER < 10)
	actElement = skillElement->FindChild(DataTxtDecode(tmpStr));
#else
	actElement = skillElement->FindChild(tmpStr);
#endif
	strcpy( tmpStr, "loopType" );
#if (_PACKET_VER < 10)
	loopTypeElement = skillElement->FindChild(DataTxtDecode(tmpStr));
#else
	loopTypeElement = skillElement->FindChild(tmpStr);
#endif

	if (g_session.IsMasterAid(1)) {
		if (!actElement || !loopTypeElement) {
			act = 0;
			loopType = 0;
			g_windowMgr.ErrorMsg("An error occurred. Notify the client developer of this error. Location - CPlayer :: GetMonsterSkillInfo", ME_OK, 1, 0, 0);
			g_windowMgr.ErrorMsg("action or loopType does not exist.", ME_OK, 1, 0, 0);
			return;
		}
	}

	sprintf(pAct, DataTxtDecode((char*)actElement->GetContents().c_str()));
	sprintf(pLoopType, DataTxtDecode((char*)loopTypeElement->GetContents().c_str()));
	act = atoi(pAct);
	loopType = atoi(pLoopType);

}

char * CSession :: GetPetHungryText(int hunger)
{
	switch (hunger) {
	case PET_HUNGER:		return "hungry";
	case PET_HUNGRY:		return "bit_hungry";
	case PET_SATISFIED:		return "noting";
	case PET_ENOUGH:		return "full";
	case PET_FULL:			return "so_full";
	}
	return "hungry";
}

void	CSession :: GetMonsterTalkInfo(int job, int stateId, int arg1, char *speak, DWORD gid)
{
	XMLElement *childEliment;
	XMLElement *monsterElement;
	XMLElement *stateElement;

	sprintf(speak, "");
	char tmpStr[256];
	strcpy( tmpStr, "enemy_monster_talk_table" );
	stateElement = m_monsterTalkTable.FindElement(DataTxtDecode(tmpStr));
	if (!stateElement) {
		return;
	}

	char argText[16], pName[256];
	strcpy(pName, g_session.m_jobNameTable[job]);
	_strlwr(pName);
	monsterElement = stateElement->FindChild(DataTxtDecode(pName));
	if (!monsterElement)
		return;

	stateElement = monsterElement->FindChild(DataTxtDecode(g_session.GetStateName(stateId)));
	if (!stateElement)
		return;


	BOOL	isSkillState = FALSE;
	switch(stateId) {
		case	MAT_CASTSKILL:
		case	MAT_USESKILL:
		case	MAT_ATTACKED_SKILL:
			isSkillState = TRUE;
			break;
	}

	std::vector<char *> speakTable;
	if (isSkillState) {
		itoa(arg1, argText, 10);
		childEliment = stateElement->FindChild(DataTxtDecode(argText));
		while (childEliment) {
			speakTable.push_back(DataTxtDecode((char *)childEliment->GetContents().c_str()));
			childEliment = childEliment->FindNext(DataTxtDecode(argText));
		}
	} else {
		while (stateElement) {
			speakTable.push_back(DataTxtDecode((char *)stateElement->GetContents().c_str()));
			stateElement = stateElement->FindNext(DataTxtDecode(g_session.GetStateName(stateId)));
		}
	}

	int size = speakTable.size(), randCnt = 0;
	char buf[256];
	if (size) {
		randCnt = rand() % size;
		sprintf(buf,speakTable[randCnt]);
		speakTable.clear();
		g_session.GetPetTalkString(buf, speak, gid, FALSE);
		return;
	}
}

char * CSession :: GetPetName(int job)
{
	switch(job) {
	case	JT_PORING:
		return "poring";
		break;
	case	JT_ROCKER:
		return "rocker";
		break;
	case	JT_DROPS:
		return "drops";
		break;
	case	JT_POPORING:
		return "poporing";
		break;
	case	JT_LUNATIC:
		return "lunatic";
		break;
	case	JT_PICKY:
		return "picky";
		break;
	case	JT_CHONCHON:
		return "chonchon";
		break;
	case	JT_STEEL_CHONCHON:
		return "steel_chonchon";
		break;
	case	JT_HUNTER_FLY:
		return "hunter_fly";
		break;
	case	JT_DESERT_WOLF_B:
		return "desert_wolf_babe";
		break;
	case	JT_SPORE:
		return "spore";
		break;
	case	JT_POISON_SPORE:
		return "poison_spore";
		break;
	case	JT_PECOPECO:
		return "pecopeco";
		break;
	case	JT_SMOKIE:
		return "smokie";
		break;
	case	JT_YOYO:
		return "yoyo";
		break;
	case	JT_ORK_WARRIOR:
		return "ork_warrior";
		break;
	case	JT_MUNAK:
		return "munak";
		break;
	case	JT_DOKEBI:
		return "dokebi";
		break;
	case	JT_SOHEE:
		return "sohee";
		break;
	case	JT_ISIS:
		return "isis";
		break;
	case	JT_PETIT:
		return "petit";
		break;
	case	JT_DEVIRUCHI:
		return "deviruchi";
		break;
	case	JT_BAPHOMET_:
		return "baphomet_j";
		break;
	case	JT_SAVAGE_BABE:
		return "savage_babe";
		break;
	case	JT_BON_GUN:
		return "bon_gun";
		break;
	case	JT_ECLIPSE_P:
		return "ECLIPSE_P";
	case	JT_ZHERLTHSH:
		return "zherlthsh";
		break;
	case	JT_ALICE:
		return "alice";
		break;
	case	JT_EVENT_RICECAKE:
		return "event_ricecake";
		break;
	case	JT_GOBLINE_XMAS:
		return "gobline_xmas";
		break;
	case	JT_CHUNG_E:
		return "CHUNG_E";
		break;
	case	JT_P_CHUNG_E:
		return "P_CHUNG_E";
		break;

	case	JT_GOBLIN_1:
		return "goblin_1";
	case	JT_GOBLIN_2:
		return "goblin_2";
	case	JT_GOBLIN_4:
		return "goblin_4";
	case	JT_DELETER_:
		return "deleter_";
	case	JT_DIABOLIC:
		return "diabolic";
	case	JT_WANDER_MAN:
		return "wander_man";

		break;
	}
	return "poring";
}

char * CSession :: GetPetActText(int act)
{
	switch(act) {
	case	PM_FEEDING:
		return "feeding";
		break;
	case	PM_HUNTING:
		return "hunting";
		break;
	case	PM_DANGER:
		return "danger";
		break;
	case	PM_DEAD:
		return "dead";
		break;
	case	PM_NORMAL:
		return "stand";
		break;
	case	PM_CONNENCT:
		return "connect";
		break;
	case	PM_LEVELUP:
		return "levelup";
		break;
	case	PM_PERFORMANCE1:
		return "perfor_1";
		break;
	case	PM_PERFORMANCE2:
		return "perfor_2";
		break;
	case	PM_PERFORMANCE3:
		return "perfor_3";
		break;
	case	PM_PERFORMANCE_S:
		return "perfor_s";
		break;
	}

	return "normal";
}

char *	CSession :: GetPetAccActName(int  accId)
{

	switch(accId) {
	case	10001:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x42\x41\x50\x48\x4F\x4D\x45\x54\x5F\xBB\xC1\xB4\xD9\xB1\xB8\xB8\xF0\xC0\xDA\x2E\x61\x63\x74";
		break;
	case	10003:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x44\x45\x53\x45\x52\x54\x5F\x57\x4F\x4C\x46\x5F\x42\x5F\xBF\xEC\xC1\xD6\xBA\xB9\xB8\xD3\xB8\xAE\x2E\x61\x63\x74";
		break;
	case	10004:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x44\x45\x56\x49\x52\x55\x43\x48\x49\x5F\xC1\xA5\xB2\xC0\xC1\xF6\x2E\x61\x63\x74";
		break;
	case	10005:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x44\x4F\x4B\x45\x42\x49\x5F\xBE\xC6\xC8\xC4\xB7\xCE\xB8\xD3\xB8\xAE\x2E\x61\x63\x74";
		break;
	case	10006:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x69\x73\x69\x73\x5F\xC5\xAC\xB7\xB9\xBF\xC0\xC6\xC4\xC6\xAE\xB6\xF3\xB8\xD3\xB8\xAE\xB6\xEC\x2E\x61\x63\x74";
		break;
	case	10007:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x6C\x75\x6E\x61\x74\x69\x63\x5F\xB8\xAE\xBA\xBB\x2E\x61\x63\x74";
		break;
	case	10008:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x6D\x75\x6E\x61\x6B\x5F\xBF\xE4\xBC\xFA\xBA\xC0\x2E\x61\x63\x74";
		break;
	case	10009:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x6F\x72\x6B\x5F\x77\x61\x72\x72\x69\x6F\x72\x5F\xB2\xC9\x2E\x61\x63\x74";
		break;
	case	10010:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x70\x65\x63\x6F\x70\x65\x63\x6F\x5F\xB3\xBF\xBA\xF1\x2E\x61\x63\x74";
		break;
	case	10011:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x50\x45\x54\x49\x54\x5F\xBA\xB0\x2E\x61\x63\x74";
		break;
	case	10012:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x70\x69\x63\x6B\x79\x5F\xBE\xCB\xB2\xAE\xC1\xFA\x2E\x61\x63\x74";
		break;
	case	10013:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x70\x6F\x72\x69\x6E\x67\x5F\xC3\xA5\xB0\xA1\xB9\xE6\x2E\x61\x63\x74";
		break;
	case	10014:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x72\x6F\x63\x6B\x65\x72\x5F\xB8\xDE\xB6\xD1\xB1\xE2\xBE\xC8\xB0\xE6\x2E\x61\x63\x74";
		break;
	case	10015:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x73\x61\x76\x61\x67\x65\x5F\x62\x61\x62\x65\x5F\xB7\xB9\xC0\xCC\xBD\xBA\x2E\x61\x63\x74";
		break;
	case	10016:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x53\x4F\x48\x45\x45\x5F\xB9\xE6\xBF\xEF\x2E\x61\x63\x74";
		break;
	case	10017:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x73\x70\x6F\x72\x65\x5F\xBF\xF8\xC1\xD6\xB9\xCE\xC4\xA1\xB8\xB6\x2E\x61\x63\x74";
		break;
	case	10018:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x79\x6F\x79\x6F\x5F\xB8\xD3\xB8\xAE\xB6\xEC\x2E\x61\x63\x74";
		break;
	case	10019:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x73\x6D\x6F\x6B\x69\x65\x5F\xB8\xD3\xC7\xC3\xB7\xAF\x2E\x61\x63\x74";
		break;
	case	10020:
		return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x62\x6F\x6E\x5F\x67\x75\x6E\x5F\xBF\xB5\xC8\xAF\xB5\xB5\xBB\xE7\xB0\xCB\x2E\x61\x63\x74";
		break;
	}
	return "\xB8\xF3\xBD\xBA\xC5\xCD\x5C\x63\x68\x6F\x63\x68\x6F\x5F\xB9\xE6\xB5\xB6\xB8\xE9\x2E\x61\x63\x74";

}


void	CSession :: GetPetIllustName(int job, BOOL IsAccEquiped, char *fName)
{
	switch(job) {
	case JT_BAPHOMET_:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB9\xD9\xC6\xF7\xB8\xDE\xC6\xAE.bmp");
		return;
	case JT_PECOPECO:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC6\xE4\xC4\xDA\xC6\xE4\xC4\xDA.bmp");
		return;
	case JT_CHONCHON:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC3\xCC\xC3\xCC.bmp");
		return;
	case JT_DESERT_WOLF_B:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB5\xA5\xC1\xAE\xC6\xAE\xBF\xEF\xC7\xC1\xBB\xF5\xB3\xA2.bmp");
		return;
	case JT_DEVIRUCHI:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB5\xA5\xBA\xF1\xB7\xE7\xC4\xA1.bmp");
		return;
	case JT_ISIS:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC0\xCC\xBD\xC3\xBD\xBA.bmp");
		return;
	case JT_LUNATIC:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB7\xE7\xB3\xAA\xC6\xBD.bmp");
		return;
	case JT_MUNAK:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB9\xAB\xB3\xAB.bmp");
		return;
	case JT_ORK_WARRIOR:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBF\xC0\xC5\xA9\xBF\xF6\xB8\xAE\xBE\xEE.bmp");
		return;
	case JT_PICKY:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC7\xC8\xC5\xB0.bmp");
		return;
	case JT_PORING:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC6\xF7\xB8\xB5.bmp");
		return;
	case JT_ROCKER:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB7\xCE\xC4\xBF.bmp");
		return;
	case JT_SPORE:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBD\xBA\xC6\xF7\xBE\xC6.bmp");
		return;
	case JT_YOYO:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBF\xE4\xBF\xE4.bmp");
		return;
	case JT_SMOKIE:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBD\xBA\xB8\xF0\xC5\xB0.bmp");
		return;
	case JT_STEEL_CHONCHON:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBD\xBA\xC6\xBF\xC3\xCC\xC3\xCC.bmp");
		return;
	case JT_POPORING:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC6\xF7\xC6\xF7\xB8\xB5.bmp");
		return;
	case JT_HUNTER_FLY:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC7\xE5\xC5\xCD\xC7\xC3\xB6\xF3\xC0\xCC.bmp");
		return;
	case JT_POISON_SPORE:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC6\xF7\xC0\xCC\xC1\xF0\xBD\xBA\xC6\xF7\xBE\xC6.bmp");
		return;
	case JT_DOKEBI:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB5\xB5\xB1\xFA\xBA\xF1.bmp");
		return;
	case JT_PETIT:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBB\xDA\xB6\xEC.bmp");
		return;
	case JT_SAVAGE_BABE:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBC\xBC\xBA\xF1\xC1\xF6\xBA\xA3\xBA\xA3.bmp");
		return;
	case JT_SOHEE:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBC\xD2\xC8\xF1.bmp");
		return;
	case JT_BON_GUN:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xBA\xBB\xB0\xC7.bmp");
		return;
	case JT_DROPS:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB5\xE5\xB7\xD3\xC7\xC1\xBD\xBA.bmp");
		return;
	case JT_ZHERLTHSH:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_ZHERLTHSH.bmp");
		return;
	case JT_GOBLIN_1:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB0\xED\xBA\xED\xB8\xB0_\xB4\xDC\xB0\xCB.bmp");
		return;
	case JT_GOBLIN_2:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB0\xED\xBA\xED\xB8\xB0_\xC7\xC3\xB7\xB9\xC0\xCF.bmp");
		return;
	case JT_GOBLIN_4:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB0\xED\xBA\xED\xB8\xB0_\xC7\xD8\xB8\xD3.bmp");
		return;
	case JT_WANDER_MAN:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB9\xE8\xC8\xB8\xC7\xCF\xB4\xC2\xC0\xDA.bmp");
		return;
	case JT_ALICE:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_ALICE.bmp");
		return;
	case JT_GOBLINE_XMAS:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB0\xED\xBA\xED\xB8\xB0_\xC0\xCC\xBA\xA5\xC6\xAE.bmp");
		return;
	case JT_DELETER_:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC1\xF6\xBB\xF3\xB5\xF4\xB8\xAE\xC5\xCD.bmp");
		return;
	case JT_DIABOLIC:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB5\xF0\xBE\xC6\xBA\xBC\xB8\xAF.bmp");
		return;
	case JT_CHUNG_E:
	case JT_P_CHUNG_E:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC3\xBB\xC0\xCC.bmp");
		return;
	case JT_ECLIPSE_P:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB7\xE7\xB3\xAA\xC6\xBD.bmp");
		return;
	case JT_EVENT_RICECAKE:
		sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xB6\xB1_\xC0\xCC\xBA\xA5\xC6\xAE.bmp");
		return;
	}
	sprintf(fName, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\illust\\\xC6\xEA_\xC6\xF7\xB8\xB5.bmp");
}

int	CSession :: GetPetHungryState(int Hunger)
{
	if (Hunger > 90 && Hunger <= 100)
		return PET_FULL;
	else if (Hunger > 75 && Hunger <= 90)
		return PET_ENOUGH;
	else if (Hunger > 25 && Hunger <= 75)
		return PET_SATISFIED;
	else if (Hunger > 10 && Hunger <= 25)
		return PET_HUNGRY;
	return 0;
}

int	CSession :: GetPetFriendlyState(int Friendly)
{
	if (Friendly)
		Friendly /= 10;
	if (Friendly > 90 && Friendly <= 100)
		return PET_FAMILIAR;
	else if (Friendly > 75 && Friendly <= 90)
		return PET_FRIENDLY;
	else if (Friendly > 25 && Friendly <= 75)
		return PET_NORMAL;
	else if (Friendly > 10 && Friendly <= 25)
		return PET_AWKWARD;
	return 0;
}

BYTE	nameCount = 13;













char * sexTable[36] =
{
	"\xBE\xC6\xBA\xFC",
	"\xC7\xFC\xB4\xD4",
	"\xBF\xC0\xBA\xFC",
	"\xB5\xB5\xB7\xC3\xB4\xD4",
	"\xC7\xFC\xBE\xBE",
	"\xB5\xB5\xB7\xC3\xB4\xD4\xC0\xBA",
	"\xB5\xB5\xB7\xC3\xB4\xD4\xC0\xBB",
	"\xB5\xB5\xB7\xC3\xB4\xD4\xB0\xFA",
	"\xBE\xC6\xBA\xFC",
	"\xBE\xC6\xBA\xFC",
	"\xBE\xC6\xBA\xFC",
	"\xBE\xC6\xBA\xFC",
	"\xBE\xC6\xBA\xFC",
	"\xBE\xF6\xB8\xB6",
	"\xB4\xA9\xB4\xD4",
	"\xBE\xF0\xB4\xCF",
	"\xBE\xC6\xB0\xA1\xBE\xBE",
	"\xBE\xC6\xB0\xA1\xBE\xBE",
	"\xBE\xC6\xB0\xA1\xBE\xBE\xB4\xC2",
	"\xBE\xC6\xB0\xA1\xBE\xBE\xB8\xA6",
	"\xBE\xC6\xB0\xA1\xBE\xBE\xBF\xCD",
	"\xBE\xF6\xB8\xB6",
	"\xBE\xF6\xB8\xB6",
	"\xBE\xF6\xB8\xB6",
	"\xBE\xF6\xB8\xB6",
	"\xBE\xF6\xB8\xB6",

};

void	CSession :: GetPetTalkString(char *srcStr, char *bufStr, int petGID, BOOL isChange)
{
	if (srcStr[0] == 9)
		srcStr = srcStr + 1;
	sprintf(bufStr, srcStr);
	BOOL isFind = TRUE;
	char *bufAddress = bufStr;
	int a, pos = 0, len = 0, pos2 = 0;
	while(isFind && isChange) {
		isFind = FALSE;
		for(a = 0; a < nameCount; a++) {
			bufStr = strstr(bufStr, sexTable[a]);
			if (bufStr) {
				isFind = TRUE;
				len = strlen(sexTable[a + nameCount]);
				pos2 += len;
				strcpy(bufStr, sexTable[a + nameCount]);
				pos = (int)bufStr - (int)bufAddress;
				srcStr = srcStr + len + pos;
				strcat(bufStr, srcStr);
				srcStr = srcStr - len - pos;
				bufStr = bufStr + len;
				break;
			}
			if (!bufStr)
				bufStr = bufAddress + pos + len;
		}
	}
	bufStr = bufAddress;
	NamePair petName = g_modeMgr.GetGameMode()->GetActorName(petGID);
	if (!petName.IsEmpty()) {
		if (strcmp(petName.cName.c_str(), "") != 0)
			sprintf(srcStr, "%s : ", petName.cName.c_str());
		else {
			strcpy(bufStr, "");
			return;
		}
	} else if (petGID == g_session.m_petGID) {
		sprintf(srcStr, "%s : ", g_session.m_petName);
	} else {
		strcpy(bufStr, "");
		return;
	}

	strcat(srcStr, bufStr);
	strcpy(bufStr, srcStr);

	len = strlen(bufStr);
	if (bufStr[len-1] == 9)
		bufStr[len-1] = NULL;
}

BOOL	CSession :: IsEFST_Berserk()
{
	for(int a = 0; a < stateId.size(); a++) {
		if (stateId[a] == EFST_BERSERK)
			return TRUE;
	}
	return FALSE;
}

int	CSession :: GetPetTalkNumber(int job, int act, int hungry)
{
	int Total = 0;
	Total += hungry;
	Total += act * 10;
	Total += (job - 1000) * 100;
	if (job >= 1519)
		Total += 90000;
	else if (g_session.GetSex() == 1)
		Total += 50000;
	else
		Total += 90000;

	return Total;
}

void	CSession :: GetPetHungryStateText(int hungry, char *buf)
{
	if (hungry >= 0 && hungry <= 10)
		strcpy(buf, "Very Hungry");
	else if (hungry > 10 && hungry <= 25)
		strcpy(buf, "Hungry");
	else if (hungry > 25 && hungry <= 75)
		strcpy(buf, "Neutral");
	else if (hungry > 75 && hungry <= 90)
		strcpy(buf, "Satisfied");
	else if (hungry > 90 && hungry <= 100)
		strcpy(buf, "Stuffed");
	else
		strcpy(buf, "Unknown.");
}

void	CSession :: GetPetFriendlyStateText(int friendly, char *buf)
{
	if (friendly >= 0 && friendly <= 100)
		strcpy(buf, "Awkward");
	else if (friendly > 100 && friendly <= 250)
		strcpy(buf, "Shy");
	else if (friendly > 250 && friendly <= 750)
		strcpy(buf, "Neutral");
	else if (friendly > 750 && friendly <= 900)
		strcpy(buf, "Cordial");
	else if (friendly > 900 && friendly <= 1000)
		strcpy(buf, "Loyal");
	else
		strcpy(buf, "I don't knooow.");
}

BOOL	CSession :: IsEFST_TrickDead()
{
	for(int a = 0; a < stateId.size(); a++) {
		if (stateId[a] == EFST_TRICKDEAD)
			return TRUE;
	}
	return FALSE;
}

int CSession::IsMonster(CGameActor *pActor)
{
	if (pActor)
	{
		if (pActor->m_objectType == NPC_MERSOL_TYPE)
			return FALSE;

		if (pActor->GetJob() > JT_MON_BEGIN && pActor->GetJob() < JT_2004_JOB_BEGIN)
			return TRUE;
	}

	return FALSE;
}

int CSession::IsPc(int job)
{
	return IsThisPC(job);
}

BOOL	CSession :: IsSiegeMode()
{
	return g_modeMgr.GetGameMode()->GetWorld()->IsSiegeMode();
}

BOOL	CSession :: IsBattleFieldMode()
{
	return g_modeMgr.GetGameMode()->GetWorld()->IsBattleFieldMode();
}

CGameActor * CSession :: GetGameActorByAid(DWORD aid)
{
	CGameMode *curMode = g_modeMgr.GetGameMode();
	if (!curMode)
		return NULL;
	return curMode->GetWorld()->GetGameActorByAID(aid);
}

int CSession::IsMyGuild(int gdid)
{
	if (gdid == 0)
		return FALSE;
	CGameMode *curMode = g_modeMgr.GetGameMode();
	if (!curMode->GetWorld()->m_player)
		return FALSE;
	if (gdid == curMode->GetWorld()->m_player->GetGdid())
		return TRUE;
	return FALSE;
}

int CSession::IsGuardian(int job)
{
	if (job == JT_ARCHER_GUARDIAN || job == JT_KNIGHT_GUARDIAN || job == JT_SOLDIER_GUARDIAN)
		return TRUE;
	return FALSE;
}

int CSession::IsPetEgg(int itemId)
{
	if (itemId > 9000 && itemId <= 9049)
		return TRUE;
	return FALSE;
}

int CSession::IsAttack(int act)
{
	switch(act) {
		case	ACTION_ATTACK:
		case	ACTION_ATTACK_NOMOTION:
			return TRUE;
			break;
	}
	return FALSE;
}

int CSession::IsExistEffectState(int efstId)
{
	for(int a = 0; a < stateId.size(); a++) {
		if (stateId[a] == efstId)
			return TRUE;
	}
	return FALSE;
}

int CSession::IsMerOrHomun(int job)
{
	if (job > JT_MER_BEGIN && job < JT_MER_LAST)
		return true;

	return false;
}

int CSession::IsMer(int job)
{
	if (job > JT_MER_ARCHER01 && job < JT_MER_LAST)
		return true;

	return false;
}

int CSession::IsHomun(int job)
{
	if (job > JT_MER_BEGIN && job <= JT_MER_VANILMIRTH_H2)
		return true;

	return false;
}

char *CSession :: GetStateName(int stateId)
{
	switch (stateId) {
	case MAT_ATTACK:			return "attack";
	case MAT_CASTSKILL:			return "castskill";
	case MAT_USESKILL:			return "useskill";
	case MAT_ATTACKED_SKILL:	return "skillattacked";
	case MAT_HP50:				return "hp50";
	case MAT_HP25:				return "hp25";
	case MAT_DEAD:				return "dead";
	}
	return NULL;
}

int CSession::IsAbracatabra(int skillId)
{
	if (skillId >= 0x123 && skillId <= 0x12F)
		return 1;
	return 0;
}


void	CSession :: SetForceAnimEndTick(DWORD AID, DWORD tick)
{
	FORCE_ANIM_TICK p;
	p.AID = AID;
	p.tick = tick;
	int a = 0, size = m_forceAnimEndTick.size();
	while (a < size) {
		if (m_forceAnimEndTick[a].AID == AID) {
			m_forceAnimEndTick[a].tick = tick;
			return;
		}
		a++;
	}
	m_forceAnimEndTick.push_back(p);
}

DWORD CSession::GetForceAnimEndTick(DWORD AID)
{
	int a = 0, size = m_forceAnimEndTick.size();
	while (a < size) {
		if (m_forceAnimEndTick[a].AID == AID) {
			return m_forceAnimEndTick[a].tick;
		}
		a++;
	}
	return -1;
}

int CSession::IsSpace(char ch)
{
	if (ch == ' ' || ch == 0x09)
		return TRUE;
	return FALSE;
}

void	CSession :: InitMapTypeInfoTable()
{
	CFile fp;
	if (!fp.Open("MapInfoTable.scp", CFile::modeRead)) {
		if( g_serviceType == ServiceKorea )
		{
			sAssertM(false,"Load MapInfoTable Failed!");
			ErrorMsg("Load MapInfoTable Failed!");
			return;
		}
		else
		{
			memset(m_mapInfoTable,0,sizeof(m_mapInfoTable));
			return;
		}
	}

	char	*buf, tmpChar, buf2[16];
	int     a, b, i, j, k;

	i = fp.GetLength();
	buf = new char[i];
	fp.Read(buf, i);

	a = 0;
	b = 0;
	j = 0;
	k = 0;
	while (1) {
		while (1) {
			tmpChar = buf[k];
			k++;
			if (k >= i) {
				break;
			} else if (tmpChar == '/') {
				if (buf[k] == '/') {
					while(tmpChar != 0x0a) {
						tmpChar = buf[k];
						k++;
					}
				} else if (buf[k] == '*') {
					while (1) {
						tmpChar = buf[k];
						k++;
						if (tmpChar == '*') {
							if (buf[k] == '/')
								break;
						}
					}
				}
			} else if (IsSpace(tmpChar)) {
				;
			} else if (tmpChar == 0x0d) {
				k++;
			} else {
				memset(buf2, 0, 16);
				while (!IsSpace(tmpChar)) {
					buf2[j] = tmpChar;
					tmpChar = buf[k];
					k++;
					j++;
					if (tmpChar == 0x0d) {
						k++;
						break;
					}
				}
				m_mapInfoTable[a][b] = atoi(buf2);
				b++;
				j = 0;
				if (b >= MAPPROPERTY_LAST) {
					b = 0;
					a++;
					break;
				}
			}
		}
		if(a >= MAPTYPE_LAST)
			break;
		if (k >= i)
			break;
	}
	delete [] buf;
	fp.Close();
}

int CSession::IsEnablePK()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_PK];
}

int CSession::IsEnableGomok()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_CALLMONSTER];
}

int CSession::IsEnableTeleport()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_TELEPORT];
}

int CSession::IsEnableMemory()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_NOTREMEMBER];
}

int CSession::IsEnableDropItem()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_ITEMDROP];
}

int CSession::IsEnableDropExp()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_EXP];
}

int CSession::IsEnableNeverMemory()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_DISCONNECT_NOTREMEMBER];
}

int CSession::IsEnableAllyParty()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_PARTY];
}

int CSession::IsEnableAllyGuild()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_GUILD];
}

int CSession::IsEnableSiegeMode()
{
	return m_mapInfoTable[m_curMapType][MAPPROPERTY_SIEGE];
}

int		CSession :: GetWeaponType(int itemID)
{
	if (itemID == 0) {
		return WEAPONTYPE_NONE;
	} else if (itemID >= 1100 && itemID < 1150) {
		return WEAPONTYPE_SWORD;
	} else if (itemID >= 13400 && itemID < 13500) {
		return WEAPONTYPE_SWORD;
	} else if (itemID >= 1150 && itemID < 1200) {
		return WEAPONTYPE_TWOHANDSWORD;
	} else if (itemID >= 1200 && itemID < 1250) {
		return WEAPONTYPE_SHORTSWORD;
	} else if (itemID >= 1250 && itemID < 1300) {
		return WEAPONTYPE_CATARRH;
	} else if (itemID >= 1300 && itemID < 1350) {
		return WEAPONTYPE_AXE;
	} else if (itemID >= 1350 && itemID < 1400) {
		return WEAPONTYPE_TWOHANDAXE;
	} else if (itemID >= 1400 && itemID < 1450) {
		return WEAPONTYPE_SPEAR;
	} else if (itemID >= 1450 && itemID < 1500) {
		if (itemID == 1472 || itemID == 1473)
			return WEAPONTYPE_ROD;
		return WEAPONTYPE_TWOHANDSPEAR;
	} else if (itemID >= 1500 && itemID < 1550) {
		return WEAPONTYPE_MACE;
	} else if (itemID >= 1550 && itemID < 1600) {
		return WEAPONTYPE_BOOK;
	} else if (itemID >= 1600 && itemID < 1700) {
		return WEAPONTYPE_ROD;
	} else if (itemID >= 1700 && itemID < 1750) {
		return WEAPONTYPE_BOW;
	} else if (itemID >= 1800 && itemID < 1900) {
		return WEAPONTYPE_KNUKLE;
	} else if (itemID >= 1900 && itemID < 1950) {
		return WEAPONTYPE_INSTRUMENT;
	} else if (itemID >= 1950 && itemID < 2000) {
		return WEAPONTYPE_WHIP;
	} else if (itemID >= 2000 && itemID < 2100) {
		return WPCLASS_TWOHANDROD;
	} else if (itemID >= 13150 && itemID < 13200) {
		return WPCLASS_GUN_RIFLE;
	} else if (itemID >= 13000 && itemID < 13100) {
		return WEAPONTYPE_SHORTSWORD;
	} else if (itemID >= 13100 && itemID < 13150) {
		return WPCLASS_GUN_HANDGUN;
	} else if (itemID >= 13300 && itemID < 13400) {
		return WPCLASS_SYURIKEN;
	}

	return -1;
}

int CSession::GetShieldType(int ShieldId)
{
	switch (ShieldId) {
	case 2101:
	case 2102:
		return 1;
	case 2103:
	case 2104:
		return 2;
	case 2105:
	case 2106:
		return 3;
	case 2107:
	case 2108:
		return 4;
	case 2109:
		return 0;
	case 2110:
	case 2111:
		return 4;
	}
	return 0;
}

void	CSession :: InitChattingProhibitLog()
{



	CFile fp;
	char curPath[256];
	GetCurrentDirectory(256, curPath);

	int Pos = (int)strstr(curPath, "\\data");
	if (Pos) {
		Pos = Pos - (int)curPath;
		memcpy(curPath, curPath, Pos);
	}
	strcat(curPath, "\\ProhibitionLog.gd");

	if (!fp.Open(curPath, CFile::modeRead)) {
		Trace("Cannot open Chatting Prohibitioin Log.");
		return;
	}

	int logCnt = 0, a = 0, len;
	fp.Read(&logCnt, sizeof(int));
	for (a = 0; a < logCnt; a++) {
		CHAT_PROHIBIT_LOG log;
			fp.Read(&len, sizeof(int));
		fp.Read(log.accountName, len);
		fp.Read(&len, sizeof(int));
		fp.Read(log.charName, len);
		fp.Read(&len, sizeof(int));
		fp.Read(log.serverName, len);
		fp.Read(&log.AID, sizeof(DWORD));
		fp.Read(&log.GID, sizeof(DWORD));
		fp.Read(&log.prohibitTime, sizeof(DWORD));
		fp.Read(&log.logDate, sizeof(DWORD));
		fp.Read(&log.logTime, sizeof(DWORD));

		m_chatProhibitLog.push_back(log);
	}
	fp.Close();
}

int		CSession :: GetChatProhibittedCnt(char *charName)
{
	int prohibitCnt = 0, a = 0, size = m_chatProhibitLog.size();
	while (a < size) {
		if (!strcmp(m_chatProhibitLog[a].serverName, m_lastServerName.c_str())) {
			if(!strcmp(m_chatProhibitLog[a].charName, charName))
				prohibitCnt++;
		}
		a++;
	}
	return prohibitCnt;
}

int		CSession :: GetChatProhibittedCnt(DWORD AID)
{



	int prohibitCnt = 0, a = 0, size = m_chatProhibitLog.size(), gid;

	CGameMode *gameMode = g_modeMgr.GetGameMode();
	CGameActor *theActor = gameMode->GetWorld()->GetGameActorByAID(AID);
	if (!theActor)
		return -1;
	m_targetGID = gid = theActor->GetGID();
	BOOL isAssignedAID = FALSE;
	while (a < size) {
		int debugGID = m_chatProhibitLog[a].GID;
		if (!strcmp(m_chatProhibitLog[a].serverName, m_lastServerName.c_str())) {
			if (m_chatProhibitLog[a].GID == gid)
				prohibitCnt++;
			if (strcmp(m_chatProhibitLog[a].accountName, "not Assigned AccountName")) {
				isAssignedAID = TRUE;
			}
		}
		a++;
	}
	if (!isAssignedAID) {
		int a, size = m_reqAccountInfo.size();
		if (size) {
			for(a = 0; a < size; a++) {
				if (m_reqAccountInfo[a].AID == AID) {
					return prohibitCnt;
				}
			}
		}
		REQ_ACCOUNTNAME_INFO reqInfo;
		reqInfo.AID = AID;
		reqInfo.reqTime = timeGetTime();
		gameMode->SendMsg(MM_REQ_ACCOUNT_NAME, AID);
		m_reqAccountInfo.push_back(reqInfo);
	}
	return prohibitCnt;
}

void	CSession :: AppendProhibit(DWORD AID, DWORD prohibitTime)
{



	CHAT_PROHIBIT_LOG log;

	NamePair name;
	name.Clear();
	CGameActor *theActor = g_modeMgr.GetGameMode()->GetWorld()->GetGameActorByAID(AID);
	if (!theActor)
		return;

	DWORD GID = theActor->GetGID();
	name = g_modeMgr.GetGameMode()->GetActorName(AID);
	name.cName.c_str();
	char charName[32];
	strcpy(charName, name.cName.c_str());
	int len = strlen(charName) + 1;
	strcpy(log.charName, charName);


	strcpy(log.accountName, "not Assigned AccountName");
	strcpy(log.serverName, m_lastServerName.c_str());
	log.prohibitTime = prohibitTime;
	log.AID = AID;
	log.GID = GID;
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	log.logDate = (sTime.wYear << 16) | ((short)sTime.wMonth << 8) | (short)sTime.wDay;
	log.logTime = (sTime.wHour << 16) | ((short)sTime.wMinute << 8) | (short)sTime.wSecond;
	m_chatProhibitLog.push_back(log);

	char *buf;
	CFile fp;
	char curPath[256];
	GetCurrentDirectory(256, curPath);

	int Pos = (int)strstr(curPath, "\\data"), logCnt = 0, size = 0;
	if (Pos) {
		Pos = Pos - (int)curPath;
		memcpy(curPath, curPath, Pos);
	}
	strcat(curPath, "\\ProhibitionLog.gd");

	if (!fp.Open(curPath, CFile::modeRead)) {
		Trace("Read Chat Log Failed");
	} else {
		size = fp.GetLength() - sizeof(int);
		buf = new char[size];
		fp.Read(&logCnt, sizeof(int));
		fp.Read(buf, size);
		fp.Close();
	}

	logCnt++;
	fp.Open(curPath, CFile::modeWrite|CFile::modeCreate);
	fp.Write(&logCnt, sizeof(int));
	if (size) {
		fp.Write(buf, size);
	}
	len = strlen(log.accountName) + 1;
	fp.Write(&len, sizeof(int));
	fp.Write(log.accountName, len);
	len = strlen(log.charName) + 1;
	fp.Write(&len, sizeof(int));
	fp.Write(log.charName, len);
	len = strlen(log.serverName) + 1;
	fp.Write(&len, sizeof(int));
	fp.Write(log.serverName, len);
	fp.Write(&AID, sizeof(DWORD));
	fp.Write(&GID, sizeof(DWORD));
	fp.Write(&log.prohibitTime, sizeof(DWORD));
	fp.Write(&log.logDate, sizeof(DWORD));
	fp.Write(&log.logTime, sizeof(DWORD));
	fp.Close();

	if (size) {
		delete [] buf;
	}
}

void	CSession :: DeleteAssignAID(DWORD AID)
{
	std::vector<REQ_ACCOUNTNAME_INFO>::iterator i = m_reqAccountInfo.begin();
	while (i != m_reqAccountInfo.end()) {
		if ((*i).AID == AID) {
			i = m_reqAccountInfo.erase(i);
			continue;
		}
		i++;
	}
}

void	CSession :: RerecordProhibitLog()
{
	int a, len, logCnt = m_chatProhibitLog.size();

	if (!logCnt)
		return;

	CFile fp;
	char curPath[256];
	GetCurrentDirectory(256, curPath);

	int Pos = (int)strstr(curPath, "\\data");
	if (Pos) {
		Pos = Pos - (int)curPath;
		memcpy(curPath, curPath, Pos);
	}
	strcat(curPath, "\\ProhibitionLog.gd");

	fp.Open(curPath, CFile::modeWrite|CFile::modeCreate);
	fp.Write(&logCnt, sizeof(int));
	for (a = 0; a < logCnt; a++) {
		len = strlen(m_chatProhibitLog[a].accountName) + 1;
		fp.Write(&len, sizeof(int));
		char *debugStr = m_chatProhibitLog[a].accountName;
		fp.Write(m_chatProhibitLog[a].accountName, len);
		len = strlen(m_chatProhibitLog[a].charName) + 1;
		fp.Write(&len, sizeof(int));
		fp.Write(m_chatProhibitLog[a].charName, len);
		len = strlen(m_chatProhibitLog[a].serverName) + 1;
		fp.Write(&len, sizeof(int));
		fp.Write(m_chatProhibitLog[a].serverName, len);
		fp.Write(&m_chatProhibitLog[a].AID, sizeof(DWORD));
		fp.Write(&m_chatProhibitLog[a].GID, sizeof(DWORD));
		fp.Write(&m_chatProhibitLog[a].prohibitTime, sizeof(DWORD));
		fp.Write(&m_chatProhibitLog[a].logDate, sizeof(DWORD));
		fp.Write(&m_chatProhibitLog[a].logTime, sizeof(DWORD));
	}

	fp.Close();

	GetCurrentDirectory(256, curPath);

	Pos = (int)strstr(curPath, "\\data");
	if (Pos) {
		Pos = Pos - (int)curPath;
		memcpy(curPath, curPath, Pos);
	}
	strcat(curPath, "\\ProhibitionLog.txt");

	fp.Open(curPath, CFile::modeWrite|CFile::modeCreate);
	for (a = 0; a < logCnt; a++)
	{
		char msg[256];
		char *debugStr = m_chatProhibitLog[a].accountName;
		fp.Write(m_chatProhibitLog[a].accountName, len);
		fp.Write(" - ", 3);

		len = strlen(m_chatProhibitLog[a].charName) + 1;
		fp.Write(m_chatProhibitLog[a].charName, len);
		fp.Write(" - ", 3);

		len = strlen(m_chatProhibitLog[a].serverName) + 1;
		fp.Write(m_chatProhibitLog[a].serverName, len);
		fp.Write(" - ", 3);

		PrintNumber(&fp,m_chatProhibitLog[a].AID);

		PrintNumber(&fp,m_chatProhibitLog[a].GID);

		DWORD b,c;
		int year,month,day,hour,min,sec;

		b = m_chatProhibitLog[a].logDate;
		c = m_chatProhibitLog[a].logTime;
		year = (b >> 16) & 0xffff;
		month = (b >> 8) & 0xff;
		day = b & 0xff;
		hour = (c >> 16) & 0xff;
		min = (c >> 8) & 0xff;
		sec = c & 0xff;
		sprintf(msg, "%d min chat ban\t%d/%d/%d\t%d:%d:%d",
			m_chatProhibitLog[a].prohibitTime, year, month, day, hour, min, sec);
		fp.Write(msg,strlen(msg));

		char e;
		e=13;
		fp.Write(&e, 1);
		e=10;
		fp.Write(&e, 1);
	}

	fp.Close();
}

void	CSession :: PrintNumber(CFile *fp, DWORD number)
{
	char msg[256];
	sprintf(msg, "%d", number);
	fp->Write(msg, strlen(msg));
	fp->Write(" - ", 3);
}

void	CSession :: EraseProhibit(DWORD AID, DWORD prohibitTime)
{




	std::vector<CHAT_PROHIBIT_LOG>::iterator i = m_chatProhibitLog.begin();
	std::vector<CHAT_PROHIBIT_LOG>::iterator eraseAID = m_chatProhibitLog.end();
	while (i != m_chatProhibitLog.end()) {
		if ((*i).AID == AID) {
			eraseAID = i;
		}
		i++;
	}

	if (eraseAID != m_chatProhibitLog.end()) {
		if ((*eraseAID).prohibitTime > prohibitTime) {
			g_windowMgr.ErrorMsg("Chat log not deleted: the time subtracted is shorter than the current chat ban.", ME_OK, 1, 0, 0);
		} else {
			m_chatProhibitLog.erase(eraseAID);
			g_windowMgr.ErrorMsg("Chat log deleted", ME_OK, 1, 0, 0);
		}
	} else {
		g_windowMgr.ErrorMsg("Chat log deletion failed: no chat ban record found.", ME_OK, 1, 0, 0);
	}
}

int		CSession :: MakeWeaponType(int left, int right)
{
	int type = 0;
	if (!left && right)
	{
		left = right;
		right = 0;
	}

	if ( (left >= 1100 && left < 1150) || (left >= 13400 && left < 13500) )
	{
		type = WEAPONTYPE_SWORD;
		if (right >= 1100 && right < 1150)
			return WEAPONTYPE_SWORD_SWORD;

		if (right >= 13400 && right < 13500)
			return WEAPONTYPE_SWORD_SWORD;

		if (right >= 1300 && right < 1350)
			return WEAPONTYPE_SWORD_AXE;
	}
	else if (left >= 1200 && left < 1250)
	{
		type = WEAPONTYPE_SHORTSWORD;
		if (right >= 1200 && right < 1250)
			return WEAPONTYPE_SHORTSWORD_SHORTSWORD;

		if (right >= 13000 && right < 13100)
			return WEAPONTYPE_SHORTSWORD_SHORTSWORD;

		if (right >= 1100 && right < 1150)
			return WEAPONTYPE_SHORTSWORD_SWORD;

		if (right >= 13400 && right < 13500)
			return WEAPONTYPE_SHORTSWORD_SWORD;

		if (right >= 1300 && right < 1350)
			return WEAPONTYPE_SHORTSWORD_AXE;
	}
	else if (left >= 1300 && left < 1350)
	{
		type = WEAPONTYPE_AXE;
		if (right >= 1300 && right < 1350)
			return WEAPONTYPE_AXE_AXE;
	}
	else if (left >= 13000 && left < 13100)
	{
		type = WEAPONTYPE_SHORTSWORD;
		if (right >= 1200 && right < 1250)
			return WEAPONTYPE_SHORTSWORD_SHORTSWORD;
		else if (right >= 1100 && right < 1150)
			return WEAPONTYPE_SHORTSWORD_SWORD;
		else if (right >= 13000 && right < 13100)
			return WEAPONTYPE_SHORTSWORD_SHORTSWORD;
		else if (right >= 13400 && right < 13500)
			return WEAPONTYPE_SHORTSWORD_SWORD;
		else if (right >= 1300 && right < 1350)
			return WEAPONTYPE_SHORTSWORD_AXE;
	}

	return type;
}

void	CSession :: ClearShortCutItemList ( )
{
	for ( int i = 0 ; i < NUM_SHORTCUT_SLOT ; i++ )
	{
		m_shortCutList[i].isSkill = 0 ;
		m_shortCutList[i].ID = 0 ;
		m_shortCutList[i].count = 0 ;
	}
}

void	CSession :: SetShortCutItemInfo ( char isSkill, int ID, int count, int slot )
{
	for (int i = m_shortcutSlotCnt * ONE_SLOTCOUNT; i < (m_shortcutSlotCnt + 1) * ONE_SLOTCOUNT; ++i)
	{
		if ((m_shortCutList[i].ID == ID) && (m_shortCutList[i].count == count) && (m_shortCutList[i].isSkill == isSkill))
		{
			m_shortCutList[i].isSkill = 0;
			m_shortCutList[i].ID = 0;
			m_shortCutList[i].count = 0;

			CGameMode * gameMode = g_modeMgr.GetGameMode();
			gameMode->SendMsg(MM_SHORTCUT_KEY_CHANGE, i, 0, 0);
		}
	}

	if (ID)
	{
		m_shortCutList[slot].count = count;
		m_shortCutList[slot].isSkill = isSkill;
		m_shortCutList[slot].ID = ID;
	}
	else
	{
		m_shortCutList[slot].count = 0;
		m_shortCutList[slot].isSkill = 0;
		m_shortCutList[slot].ID = 0;
	}

	CGameMode * gameMode = g_modeMgr.GetGameMode();
	gameMode->SendMsg(MM_SHORTCUT_KEY_CHANGE, slot, 0, 0);
}

void	CSession :: SetDamageSituation(int Time, int GID, int Num, bool Disp)
{
	HP_Damage_Moment temp_HP;
	temp_HP.DamageEnd = (Disp == 1) ? 1 : 2;
	temp_HP.DamageChar = GID;
	temp_HP.DamageTime = Time;
	temp_HP.DamageNum = Num;
	g_DM_List.push_back(temp_HP);
}
