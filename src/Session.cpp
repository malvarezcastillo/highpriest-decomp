#include "common.h"
#include "MsgStrings.h"
#include "framework/Locale.h"
#include "device/Sound.h"
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include "Session.h"
#include "base/Util.h"
#include "pdb_enums.h"
#include "window/UISys.h"
#include "GuildInfo.h"
#include "UIMessengerWnd.h"
#include "Insult.h"
#include "Mode.h"
#include "GameActor.h"
#include "GameMode.h"
#include "GameWorld.h"
#include "base/File.h"
#include "resource/Action.h"
#include "resource/Sprite.h"
#include "skin.h"

#include "resource/Skybox.h"
#include "resource/Ground.h"
#include "pdb_shared.h"

#ifndef __MEMORIAL_DUNGEON
#define __MEMORIAL_DUNGEON
#endif

using namespace std;

typedef std::vector<const char*> PCharList;
typedef std::vector<const char*>::iterator PCharListIter;
typedef std::pair<const char*, const char*> StringPair;
typedef std::pair<const char*, bool> IntegerPair;
typedef std::vector<std::string> StringList;
typedef std::pair<std::string, StringList> StringListPair;
typedef std::vector<int> IntegerList;
typedef std::pair<std::string, IntegerList> IntegerListPair;
enum { MAX_FRIENDS_LIST = 40 };
enum { ONE_SLOTCOUNT = 9, NUM_SHORTCUT_SLOT = 27 };
enum { JOBMINUS = 3950 };
char *g_optionPath = "\\Option";



typedef struct _tagWebFont
{
	HANDLE hWebFont;
	char WebFontName[256];
} WebFont;


extern UIWindowMgr g_windowMgr;
extern int g_bOldServer;
extern CModeMgr g_modeMgr;
extern CRenderer* g_renderer;
extern CSession g_session;
extern char g_MessageHi;
extern const char* g_regPath;
extern Registry2 g_registry;
extern CMssVariables g_mssVar;
extern int g_isSoundOn;
extern int g_isFixedCamera;
extern int g_isLightMap;
extern char g_isGravityID;
extern int g_used_Skill;
extern int g_Mob_used_Skill;
extern int g_BattleEvent;

CSession g_session;
char g_isGravityID = 0;
extern float g_outdoorViewLatitude;
extern float g_outdoorViewDistance;
extern float g_indoorViewLatitude;
extern float g_indoorViewDistance;
extern std::vector<int> stateId;
extern ServiceType g_serviceType;
extern const char* MsgStr(MSGSTRINGID id);
extern bool IsGravityAid(DWORD aid);
bool	IsEffectStateChicken(DWORD effectState);
bool	IsEffectStateStar2(DWORD effectState);
void InitMsgStrings();
void SetStreamVolume(int volume);
void Set2DVolume(int volume);
void Set3DVolume(int volume);
int GetStreamVolume();
int Get2DVolume();

#ifndef MAX_EQUIP_SLOT
#define MAX_EQUIP_SLOT 10
#endif





CSession :: CSession()
{
}

CSession :: ~CSession()
{
	RerecordProhibitLog();
	m_recievedAccountName.clear();
	m_reqAccountInfo.clear();
	m_chatProhibitLog.clear();
}

#if 1
void	CSession :: Init()
{
	m_averagePingTime = 0;
	m_talkType = 0;
	m_isAttrIndicator = 1;
	m_aid = 0;
	m_isEffectOn = 1;
	m_bgmIsPaused = 0;

	int sysTime = GetCurSystemTime();
	int tGetTime = timeGetTime();
	m_systemDiffTime = sysTime - tGetTime;

	m_emblemState = true;
	m_LoadingTime = 0;
	m_isShowGameOver = FALSE;
	m_killTimeStartTick = 0;
	m_showDigitTick = 0;
	m_isShowNameTypeOld = true;
	m_isNeverDie = FALSE;
	m_isShowTime = FALSE;
	m_isMinEffect = FALSE;
	m_isMinEffectOld = FALSE;
	m_isPropertyDisableLockOn = FALSE;
	m_isPropertyCountPK = FALSE;
	m_isPropertyNoParty = FALSE;
	m_showType = 0;
	m_shortcutSlotCnt = 0;
	m_isSaveChat = FALSE;
	m_curMapType = 0;
	stateId.clear();
	m_isShowTeamGravityPlanetLogo = FALSE;

	m_isPetNameModified = FALSE;
	m_petJob = -1;
	m_petLevel = 1;
	m_petHungry = 100;
	m_petFriendly = 100;
	m_petEggIndex = -1;
	m_petOldHungry = 100;
	sprintf(m_petName, "");

	InitPetTables();
	InitMapTypeInfoTable();
	InitChattingProhibitLog();

	sprintf(m_homunName, "");
	m_homunAtk = 0;
	m_homunMatk = 0;
	m_homunHit = 0;
	m_homunCritical = 0;
	m_homunDef = 0;
	m_homunMdef = 0;
	m_homunFlee = 0;
	m_homunAspd = 0;
	m_homunJob = -1;
	m_homunLevel = 1;
	m_homunHP = 0;
	m_homunMaxHP = 999999;
	m_homunSP = 0;
	m_homunMaxSP = 999999;
	m_homunExp = 0;
	m_homunNextExp = 999999;
	m_homunHungry = 100;
	m_homunMaxHungry = 100;
	m_homunFriendly = 100;
	m_isHomunNameModified = false;
	m_homunAlive = false;
	m_onTraceAI = false;
	m_onHoUserAI = false;
	m_homunStateId = 0;
	m_oldHomunHungry = 0;
	m_homunATKRange = 0;
	InitHomunTargetInfo();

	m_merJob = -1;
	sprintf(m_merName, "Mercenary");
	m_merAtk = 0;
	m_merMatk = 0;
	m_merHit = 0;
	m_merCritical = 0;
	m_merDef = 0;
	m_merMdef = 0;
	m_merFlee = 0;
	m_merAspd = 0;
	m_merLevel = 1;
	m_merHP = 0;
	m_merSP = 0;
	m_merMaxHP = 999999;
	m_merMaxSP = 999999;
	m_merFaith = 0;
	m_merCallNum = 0;
	m_merKillCounter = 0;
	m_merATKRange = 0;
	m_merExpireDate = 0;
	m_merStateId = 0;
	m_onMerUserAI = false;
	m_onAutoMSG = false;

	m_waitForSendMail = false;
	m_waitForAddAuction = false;
	m_auctionSeachOption = 0;
	m_auctionSeachOption2 = 0;
	m_dwOldTime = GetTickCount();
	m_changeNameGID = 0;
	sprintf(m_changeCharName, "");
	m_bRefuseInviteParty = false;
	for (int i = 0; i < sizeof(m_battleMsgOnOffSet); ++i)
		m_battleMsgOnOffSet[i] = true;
	m_onDunInfoWnd = FALSE;
	dunInfoWndX = -1;
	dunInfoWndY = -1;
	m_dunName = "";
	m_priority = 0;
	m_destroyTime = 0;
	m_enterTimeOut = 0;
	m_dunState = 0;
	m_bOpenEquipedItem = 0;
}
#endif


void	CSession :: InitExListWithRegistry()
{
	return;
}

void	CSession :: WriteExListToRegistry()
{
	return;
}

#if 1
void	CSession :: InitOptionWithRegistry()
{
	char OptionPath[512];
	strcpy(OptionPath, g_regPath);
	strcat(OptionPath, g_optionPath);

	HKEY hResultKey;
	LRESULT lResult;
	DWORD REG_TYPE;
	DWORD DATALENGTH;

	lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, OptionPath, 0, KEY_QUERY_VALUE, &hResultKey);




	if (lResult == ERROR_SUCCESS) {
		RegCloseKey(hResultKey);
		HKEY hKey;
		LRESULT lResult1;

		lResult1 = RegOpenKeyExA(HKEY_LOCAL_MACHINE, OptionPath, 0, KEY_QUERY_VALUE, &hKey);
		if (lResult1 == ERROR_SUCCESS) {
			int streamVolume;
			int soundVolume;
			char skinName[129];
			REG_TYPE = REG_DWORD;
			DATALENGTH = 4;
			if (RegQueryValueExA(hKey, "isEffectOn", 0, &REG_TYPE, (LPBYTE)&m_isEffectOn, &DATALENGTH) != ERROR_SUCCESS)
				m_isEffectOn = 1;
			if (RegQueryValueExA(hKey, "bgmIsPaused", 0, &REG_TYPE, (LPBYTE)&m_bgmIsPaused, &DATALENGTH) != ERROR_SUCCESS)
				m_bgmIsPaused = 0;
			if (RegQueryValueExA(hKey, "isSoundOn", 0, &REG_TYPE, (LPBYTE)&g_isSoundOn, &DATALENGTH) != ERROR_SUCCESS)
				g_isSoundOn = 1;
			if (RegQueryValueExA(hKey, "streamVolume", 0, &REG_TYPE, (LPBYTE)&streamVolume, &DATALENGTH) != ERROR_SUCCESS)
				streamVolume = 100;
			if (RegQueryValueExA(hKey, "soundVolume", 0, &REG_TYPE, (LPBYTE)&soundVolume, &DATALENGTH) != ERROR_SUCCESS)
				soundVolume = 100;
			if (RegQueryValueExA(hKey, "m_bAutoOpenDetailWindowIfLowMemory", 0, &REG_TYPE, (LPBYTE)&m_bAutoOpenDetailWindowIfLowMemory, &DATALENGTH) != ERROR_SUCCESS)
				m_bAutoOpenDetailWindowIfLowMemory = 1;

			m_isEffectOn = m_isEffectOn != 0;
			g_isSoundOn = g_isSoundOn != 0;
			m_bgmIsPaused = m_bgmIsPaused != 0;
			g_session.m_BgmVolume = streamVolume;
			SetStreamVolume(streamVolume);
			Set2DVolume(soundVolume);
			Set3DVolume(soundVolume);

			if (RegQueryValueExA(hKey, "m_monsterSnapOn_Skill", 0, &REG_TYPE, (LPBYTE)&m_monsterSnapOn_Skill, &DATALENGTH) != ERROR_SUCCESS)
				m_monsterSnapOn_Skill = 1;
			if (m_monsterSnapOn_Skill)
				m_monsterSnapOn_Skill = 1;
			if (RegQueryValueExA(hKey, "m_monsterSnapOn_NoSkill", 0, &REG_TYPE, (LPBYTE)&m_monsterSnapOn_NoSkill, &DATALENGTH) != ERROR_SUCCESS)
				m_monsterSnapOn_NoSkill = 0;
			if (m_monsterSnapOn_NoSkill)
				m_monsterSnapOn_NoSkill = 1;
			if (RegQueryValueExA(hKey, "m_isItemSnap", 0, &REG_TYPE, (LPBYTE)&m_isItemSnap, &DATALENGTH) != ERROR_SUCCESS)
				m_isItemSnap = 0;
			if (m_isItemSnap)
				m_isItemSnap = 1;
			if (RegQueryValueExA(hKey, "m_isShowWhisperWnd", 0, &REG_TYPE, (LPBYTE)&m_isShowWhisperWnd, &DATALENGTH) != ERROR_SUCCESS)
				m_isShowWhisperWnd = 1;
			if (m_isShowWhisperWnd)
				m_isShowWhisperWnd = 1;
			if (RegQueryValueExA(hKey, "m_isShowWhisperWnd_Friend", 0, &REG_TYPE, (LPBYTE)&m_isShowWhisperWnd_Friend, &DATALENGTH) != ERROR_SUCCESS)
				m_isShowWhisperWnd_Friend = 1;
			if (m_isShowWhisperWnd_Friend)
				m_isShowWhisperWnd = 1;
			if (RegQueryValueExA(hKey, "m_isPlayWhisperOpenSound", 0, &REG_TYPE, (LPBYTE)&m_isPlayWhisperOpenSound, &DATALENGTH) != ERROR_SUCCESS)
				m_isPlayWhisperOpenSound = 1;
			if (m_isPlayWhisperOpenSound)
				m_isPlayWhisperOpenSound = 1;

			m_isMonsterSnap = 0;
			if (m_monsterSnapOn_NoSkill) m_isMonsterSnap = 1;

			if (RegQueryValueExA(hKey, "m_bMakeMissEffect", 0, &REG_TYPE, (LPBYTE)&m_bMakeMissEffect, &DATALENGTH) != ERROR_SUCCESS)
				m_bMakeMissEffect = 1;
			else
				m_bMakeMissEffect = m_bMakeMissEffect != 0;

			if (RegQueryValueExA(hKey, "g_isFixedCamera", 0, &REG_TYPE, (LPBYTE)&g_isFixedCamera, &DATALENGTH) != ERROR_SUCCESS)
				g_isFixedCamera = 0;
			else
				g_isFixedCamera = g_isFixedCamera != 0;


			if (false) {
				RegQueryValueExA(hKey, "g_outdoorViewLatitude", 0, &REG_TYPE, (LPBYTE)&g_outdoorViewLatitude, &DATALENGTH);
				RegQueryValueExA(hKey, "g_outdoorViewDistance", 0, &REG_TYPE, (LPBYTE)&g_outdoorViewDistance, &DATALENGTH);
				RegQueryValueExA(hKey, "g_indoorViewLatitude", 0, &REG_TYPE, (LPBYTE)&g_indoorViewLatitude, &DATALENGTH);
				RegQueryValueExA(hKey, "g_indoorViewDistance", 0, &REG_TYPE, (LPBYTE)&g_indoorViewDistance, &DATALENGTH);
			} else {
				g_outdoorViewLatitude = -45.0f;
				g_outdoorViewDistance = 500.0f;
				g_indoorViewLatitude = -45.0f;
				g_indoorViewDistance = 500.0f;
			}

			REG_TYPE = REG_SZ;
			DATALENGTH = 128;
			if (RegQueryValueExA(hKey, "SKINNAME", 0, &REG_TYPE, (LPBYTE)skinName, &DATALENGTH) != ERROR_SUCCESS)
				strcpy(skinName, "<Basic Skin>");
			g_skinMgr.m_savedSkinName = skinName;

			if (RegQueryValueExA(hKey, "m_isNoCtrl", 0, &REG_TYPE, (LPBYTE)&m_isNoCtrl, &DATALENGTH) != ERROR_SUCCESS)
				m_isNoCtrl = 0;
			if (m_isNoCtrl)
				m_isNoCtrl = 1;
			if (RegQueryValueExA(hKey, "g_isGravityID", 0, &REG_TYPE, (LPBYTE)&g_isGravityID, &DATALENGTH) != ERROR_SUCCESS)
				g_isGravityID = 0;
			RegCloseKey(hKey);
		}
	} else {
		m_fogOn = 1;
		m_isEffectOn = 1;
		m_bgmIsPaused = 0;
		Set2DVolume(100);
		Set3DVolume(100);
		g_isSoundOn = 1;
		SetStreamVolume(100);
		m_onMerUserAI = 0;
		m_onHoUserAI = 0;
	}




	bool g_isTipOn = false;
	if (g_windowMgr.GetTipOfDayWnd())
		g_isTipOn = true;
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_bgmIsPaused == 0, TT_MUSIC_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isEffectOn, TT_EFFECT_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_isSoundOn, TT_EFFECT_SOUND_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isBattle, TT_BATTLE_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_isTipOn, TT_OPEN_TIP));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isNoTrade, TT_NOTRADE_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isNoShift, TT_NOSHIFT_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isNoCtrl, TT_NOCTRL_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isSkillFail, TT_SKILLFAIL_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isNoTalkMsg == 0, TT_NOTALKMSG_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isNoTalkMsg2 == 0, TT_NOTALKMSG2_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isAura, TT_AURA_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isWindow, TT_WINDOW_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_bMakeMissEffect, TT_MISS_EFFECT_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(0, TT_SET1_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(0, TT_SET2_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isQuickSpell, TT_QUICKSPELL_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isQuickSpell2, TT_QUICKSPELL2_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(0, TT_QUICKSPELL3_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isShowNameTypeOld, TT_SHOW_NAME_TYPE));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_fogOn, TT_FOG));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isLogInOut, TT_LOGINOUT_ON_OFF));
	m_cmdOnOffSet.push_back(std::pair<int, TALKTYPE>(g_session.m_isShopping, TT_SHOPPING_ON_OFF));
}
#endif


void	CSession :: Create()
{
	srand(timeGetTime());
	ChangeXorValue();
	InitTable();
	InitOptionWithRegistry();
}

#if 1
void	CSession :: WriteOptionToRegistry()
{
	HKEY hKey;
	DWORD dwDisposition = 0;
	int numExName;
	char currentSkinName[129];
	int soundVolume;
	char OptionPath[512];

	if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, g_regPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {
		RegSetValueExA(hKey, "SPRITEMODE", 0, REG_DWORD, (LPBYTE)&g_registry.m_spriteMode, 4);
		RegSetValueExA(hKey, "TEXTUREMODE", 0, REG_DWORD, (LPBYTE)&g_registry.m_textureMode, 4);
		RegSetValueExA(hKey, "TRILINEARFILTER", 0, REG_DWORD, (LPBYTE)&g_registry.m_trilinearFilter, 4);
		RegSetValueExA(hKey, "FOG", 0, REG_DWORD, (LPBYTE)&m_fogOn, 4);
		RegCloseKey(hKey);
	}

	strcpy(OptionPath, g_regPath);
	strcat(OptionPath, g_optionPath);
	if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, OptionPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS) {
		soundVolume = g_mssVar.digital2DVolume;
		RegSetValueExA(hKey, "isEffectOn", 0, REG_DWORD, (LPBYTE)&m_isEffectOn, 4);
		RegSetValueExA(hKey, "bgmIsPaused", 0, REG_DWORD, (LPBYTE)&m_bgmIsPaused, 4);
		RegSetValueExA(hKey, "isSoundOn", 0, REG_DWORD, (LPBYTE)&g_isSoundOn, 4);
		RegSetValueExA(hKey, "streamVolume", 0, REG_DWORD, (LPBYTE)&g_session.m_BgmVolume, 4);
		RegSetValueExA(hKey, "soundVolume", 0, REG_DWORD, (LPBYTE)&soundVolume, 4);
		RegSetValueExA(hKey, "m_monsterSnapOn_Skill", 0, REG_DWORD, (LPBYTE)&m_monsterSnapOn_Skill, 4);
		RegSetValueExA(hKey, "m_monsterSnapOn_NoSkill", 0, REG_DWORD, (LPBYTE)&m_monsterSnapOn_NoSkill, 4);
		RegSetValueExA(hKey, "m_isItemSnap", 0, REG_DWORD, (LPBYTE)&m_isItemSnap, 4);
		RegSetValueExA(hKey, "m_isShowWhisperWnd", 0, REG_DWORD, (LPBYTE)&m_isShowWhisperWnd, 4);
		RegSetValueExA(hKey, "m_isShowWhisperWnd_Friend", 0, REG_DWORD, (LPBYTE)&m_isShowWhisperWnd_Friend, 4);
		RegSetValueExA(hKey, "m_isPlayWhisperOpenSound", 0, REG_DWORD, (LPBYTE)&m_isPlayWhisperOpenSound, 4);
		RegSetValueExA(hKey, "g_outdoorViewLatitude", 0, REG_DWORD, (LPBYTE)&g_outdoorViewLatitude, 4);
		RegSetValueExA(hKey, "g_outdoorViewDistance", 0, REG_DWORD, (LPBYTE)&g_outdoorViewDistance, 4);
		RegSetValueExA(hKey, "g_indoorViewLatitude", 0, REG_DWORD, (LPBYTE)&g_indoorViewLatitude, 4);
		RegSetValueExA(hKey, "g_indoorViewDistance", 0, REG_DWORD, (LPBYTE)&g_indoorViewDistance, 4);
		RegSetValueExA(hKey, "m_bMakeMissEffect", 0, REG_DWORD, (LPBYTE)&m_bMakeMissEffect, 4);
		RegSetValueExA(hKey, "g_isFixedCamera", 0, REG_DWORD, (LPBYTE)&g_isFixedCamera, 4);
		RegSetValueExA(hKey, "m_bAutoOpenDetailWindowIfLowMemory", 0, REG_DWORD, (LPBYTE)&m_bAutoOpenDetailWindowIfLowMemory, 4);

		sprintf(currentSkinName, "%s", g_skinMgr.GetCurrentSkinName());
		RegSetValueExA(hKey, "SKINNAME", 0, REG_SZ, (LPBYTE)currentSkinName, 128);

		numExName = g_session.m_exNameList.size();
		if (numExName > 20)
			numExName = 20;
		RegSetValueExA(hKey, "NUMEXNAME", 0, REG_DWORD, (LPBYTE)&numExName, 4);
		{
			int i = 0;
			std::list<std::string>::iterator it = m_exNameList.begin();
			while (it != m_exNameList.end()) {
				char keyName[40];
				char name[65];
				sprintf(keyName, "EXNAME%d", i);
				sprintf(name, "%s", it->c_str());
				RegSetValueExA(hKey, keyName, 0, REG_SZ, (LPBYTE)name, 64);
				++i;
				++it;
			}
		}

		int numExAid = g_session.m_exAidList.size();
		if (numExAid > 20) numExAid = 20;
		RegSetValueExA(hKey, "NUMEXAID", 0, REG_DWORD, (LPBYTE)&numExAid, 4);
		{
			int i = 0;
			std::list<DWORD>::iterator aidIt = m_exAidList.begin();
			while (aidIt != m_exAidList.end()) {
				char keyName[40];
				DWORD aid;
				sprintf(keyName, "EXAID%d", i);
				aid = *aidIt;
				RegSetValueExA(hKey, keyName, 0, REG_DWORD, (LPBYTE)&aid, 4);
				++i;
				++aidIt;
			}
		}

		RegSetValueExA(hKey, "onHoUserAI", 0, REG_DWORD, (LPBYTE)&m_onHoUserAI, 4);
		RegSetValueExA(hKey, "onMerUserAI", 0, REG_DWORD, (LPBYTE)&m_onMerUserAI, 4);
		RegSetValueExA(hKey, "m_isNoCtrl", 0, REG_DWORD, (LPBYTE)&m_isNoCtrl, 4);
		RegSetValueExA(hKey, "g_isGravityID", 0, REG_DWORD, (LPBYTE)&g_isGravityID, 4);

		RECT windowRect;
		GetWindowRect(g_hMainWnd, &windowRect);
		RegSetValueExA(hKey, "window_left", 0, REG_DWORD, (LPBYTE)&windowRect.left, 4);
		RegSetValueExA(hKey, "window_top", 0, REG_DWORD, (LPBYTE)&windowRect.top, 4);
		RegCloseKey(hKey);
	}
}
#endif


void CSession :: InitLanguageTable()
{

	InitMsgStrings();
	g_insultFilter.Load("manner.txt");
	InitPetTalkTable();
	InitMapNameTable();

	ITEM_INFO::InitItemInfoTables();
	SKILL_INFO::InitSkillInfoTables();
}

#if 1
void	CSession :: InitTable()
{
	static int initCnt = 0;
	++initCnt;

	InitPcNameTable();
	InitWeaponSwingWaveName();
	InitWeaponHitWaveName();
	InitJobHitWaveName();
	InitEnableObjLightMapTable();
	InitMp3NameTable();

	InitIndoorRswTable();
	InitViewPointTable();

	InitSkillEffectIdTable();

	InitLanguageTable();

	InitFogParameterTable();
	InitShadowFactorWithJob();
	InitJobNameTable();
	InitTalkTypeTable();
	InitVariables();

	InitWithClear();
	InitWhenServerMove();

	InitShortCutTextFromReg();
	Trace("Session table init finished");

	m_initMapPos = 0;
	m_maxNum = -1;
	InitMapPosTable();
}
#endif


void	CSession :: InitWithClear()
{
	ClearDealItems();
	ClearExchangeItemList();
	ClearOtherExchangeItemList();
	ClearStoreItem();
	ClearSkillItem();
	ClearFriend();
	ClearParty();
}


void	CSession :: ClearPlusValue()
{
	m_plusStr = 0;
	m_plusDex = 0;
	m_plusLuk = 0;
	m_plusAgi = 0;
	m_plusVit = 0;
	m_plusInt = 0;
}

void	CSession :: ClearDealItems()
{
	ClearPurchaseItem();
	ClearSellItem();
	ClearShopItem();
}

void	CSession :: ClearMerchantDealItems()
{
	m_merchantPurchaseItemList.clear();
	m_merchantShopItemList.clear();
}

void	CSession :: InitPcNameTable()
{
	m_newPcJobNameTable.resize(JT_LAST-JOBMINUS, "\xc3\xca\xba\xb8\xc0\xda");
	m_newPcJobNameTable[0] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobNameTable[26] = "\xbb\xea\xc5\xb8";
	m_newPcJobNameTable[27] = "\xbf\xa9\xb8\xa7";
	m_newPcJobNameTable[1] = "\xb0\xcb\xbb\xe7";
	m_newPcJobNameTable[2] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_newPcJobNameTable[3] = "\xb1\xc3\xbc\xf6";
	m_newPcJobNameTable[4] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobNameTable[5] = "\xbb\xf3\xc0\xce";
	m_newPcJobNameTable[6] = "\xb5\xb5\xb5\xcf";
	m_newPcJobNameTable[7] = "\xb1\xe2\xbb\xe7";
	m_newPcJobNameTable[8] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_newPcJobNameTable[9] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_newPcJobNameTable[10] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	m_newPcJobNameTable[11] = "\xc7\xe5\xc5\xcd";
	m_newPcJobNameTable[12] = "\xbe\xee\xbc\xbc\xbd\xc5";
	m_newPcJobNameTable[13] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_newPcJobNameTable[14] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobNameTable[15] = "\xb8\xf9\xc5\xa9";
	m_newPcJobNameTable[16] = "\xbc\xbc\xc0\xcc\xc1\xf6";
	m_newPcJobNameTable[17] = "\xb7\xce\xb1\xd7";
	m_newPcJobNameTable[18] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_newPcJobNameTable[19] = "\xb9\xd9\xb5\xe5";
	m_newPcJobNameTable[20] = "\xb9\xab\xc8\xf1";
	m_newPcJobNameTable[21] = "\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobNameTable[23] = "\xbd\xb4\xc6\xdb\xb3\xeb\xba\xf1\xbd\xba";
	m_newPcJobNameTable[22] = "\xb0\xe1\xc8\xa5";
	m_newPcJobNameTable[24] = "\xb0\xc7\xb3\xca";
	m_newPcJobNameTable[25] = "\xb4\xd1\xc0\xda";
	m_newPcJobNameTable[51] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobNameTable[52] = "\xb0\xcb\xbb\xe7";
	m_newPcJobNameTable[53] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_newPcJobNameTable[54] = "\xb1\xc3\xbc\xf6";
	m_newPcJobNameTable[55] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobNameTable[56] = "\xbb\xf3\xc0\xce";
	m_newPcJobNameTable[57] = "\xb5\xb5\xb5\xcf";
	m_newPcJobNameTable[59] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae_H";
	m_newPcJobNameTable[62] = "\xc7\xe5\xc5\xcd_H";
	m_newPcJobNameTable[61] = "\xc1\xa6\xc3\xb6\xb0\xf8_H";
	m_newPcJobNameTable[63] = "\xbe\xee\xbc\xbc\xbd\xc5_H";
	m_newPcJobNameTable[58] = "\xb1\xe2\xbb\xe7_H";
	m_newPcJobNameTable[64] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7_H";
	m_newPcJobNameTable[60] = "\xc0\xa7\xc0\xfa\xb5\xe5_H";
	m_newPcJobNameTable[65] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5_H";
	m_newPcJobNameTable[72] = "\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5_H";
	m_newPcJobNameTable[66] = "\xb8\xf9\xc5\xa9_H";
	m_newPcJobNameTable[67] = "\xbc\xbc\xc0\xcc\xc1\xf6_H";
	m_newPcJobNameTable[68] = "\xb7\xce\xb1\xd7_H";
	m_newPcJobNameTable[69] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7_H";
	m_newPcJobNameTable[70] = "\xb9\xd9\xb5\xe5_H";
	m_newPcJobNameTable[71] = "\xb9\xab\xc8\xf1_H";
	m_newPcJobNameTable[58] = "\xb7\xce\xb5\xe5\xb3\xaa\xc0\xcc\xc6\xae";
	m_newPcJobNameTable[64] = "\xb7\xce\xb5\xe5\xc6\xe4\xc4\xda";
	m_newPcJobNameTable[60] = "\xc7\xcf\xc0\xcc\xc0\xa7\xc0\xfa\xb5\xe5";
	m_newPcJobNameTable[61] = "\xc8\xad\xc0\xcc\xc6\xae\xbd\xba\xb9\xcc\xbd\xba";
	m_newPcJobNameTable[63] = "\xbe\xee\xbd\xd8\xbd\xc5\xc5\xa9\xb7\xce\xbd\xba";
	m_newPcJobNameTable[59] = "\xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae";
	m_newPcJobNameTable[62] = "\xbd\xba\xb3\xaa\xc0\xcc\xc6\xdb";
	m_newPcJobNameTable[68] = "\xbd\xba\xc5\xe4\xc4\xbf";
	m_newPcJobNameTable[67] = "\xc7\xc1\xb7\xce\xc6\xe4\xbc\xad";
	m_newPcJobNameTable[69] = "\xc5\xa9\xb8\xae\xbf\xa1\xc0\xcc\xc5\xcd";
	m_newPcJobNameTable[66] = "\xc3\xa8\xc7\xc7\xbf\xc2";
	m_newPcJobNameTable[70] = "\xc5\xac\xb6\xf3\xbf\xee";
	m_newPcJobNameTable[71] = "\xc1\xfd\xbd\xc3";
	m_newPcJobNameTable[65] = "\xc6\xc8\xb6\xf3\xb5\xf2";
	m_newPcJobNameTable[72] = "\xc6\xe4\xc4\xda\xc6\xc8\xb6\xf3\xb5\xf2";
	m_newPcJobNameTable[73] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobNameTable[100] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobNameTable[101] = "\xb1\xe2\xbb\xe7";
	m_newPcJobNameTable[102] = "\xbc\xbc\xc0\xcc\xc1\xf6";
	m_newPcJobNameTable[74] = "\xb0\xcb\xbb\xe7";
	m_newPcJobNameTable[75] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_newPcJobNameTable[76] = "\xb1\xc3\xbc\xf6";
	m_newPcJobNameTable[77] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobNameTable[78] = "\xbb\xf3\xc0\xce";
	m_newPcJobNameTable[79] = "\xb5\xb5\xb5\xcf";
	m_newPcJobNameTable[80] = "\xb1\xe2\xbb\xe7";
	m_newPcJobNameTable[81] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_newPcJobNameTable[82] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_newPcJobNameTable[83] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	m_newPcJobNameTable[84] = "\xc7\xe5\xc5\xcd";
	m_newPcJobNameTable[85] = "\xbe\xee\xbc\xbc\xbd\xc5";
	m_newPcJobNameTable[86] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_newPcJobNameTable[87] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobNameTable[88] = "\xb8\xf9\xc5\xa9";
	m_newPcJobNameTable[89] = "\xbc\xbc\xc0\xcc\xc1\xf6";
	m_newPcJobNameTable[90] = "\xb7\xce\xb1\xd7";
	m_newPcJobNameTable[91] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_newPcJobNameTable[92] = "\xb9\xd9\xb5\xe5";
	m_newPcJobNameTable[93] = "\xb9\xab\xc8\xf1\xb9\xd9\xc1\xf6";
	m_newPcJobNameTable[94] = "\xb1\xb8\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobNameTable[95] = "\xbd\xb4\xc6\xdb\xb3\xeb\xba\xf1\xbd\xba";
	m_newPcJobNameTable[96] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_newPcJobNameTable[97] = "\xb1\xc7\xbc\xba";
	m_newPcJobNameTable[98] = "\xb1\xc7\xbc\xba\xc0\xb6\xc7\xd5";
	m_newPcJobNameTable[99] = "\xbc\xd2\xbf\xef\xb8\xb5\xc4\xbf";

	m_newPcJobImfNameTable.resize(JT_LAST-JOBMINUS, "\xc3\xca\xba\xb8\xc0\xda");
	m_newPcJobImfNameTable[0] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobImfNameTable[1] = "\xb0\xcb\xbb\xe7";
	m_newPcJobImfNameTable[2] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_newPcJobImfNameTable[3] = "\xb1\xc3\xbc\xf6";
	m_newPcJobImfNameTable[4] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobImfNameTable[5] = "\xbb\xf3\xc0\xce";
	m_newPcJobImfNameTable[6] = "\xb5\xb5\xb5\xcf";
	m_newPcJobImfNameTable[7] = "\xb1\xe2\xbb\xe7";
	m_newPcJobImfNameTable[8] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_newPcJobImfNameTable[9] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_newPcJobImfNameTable[10] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	m_newPcJobImfNameTable[11] = "\xc7\xe5\xc5\xcd";
	m_newPcJobImfNameTable[12] = "\xbe\xee\xbc\xbc\xbd\xc5";
	m_newPcJobImfNameTable[13] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_newPcJobImfNameTable[14] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobImfNameTable[15] = "\xb8\xf9\xc5\xa9";
	m_newPcJobImfNameTable[16] = "\xbc\xbc\xc0\xcc\xc1\xf6";
	m_newPcJobImfNameTable[17] = "\xb7\xce\xb1\xd7";
	m_newPcJobImfNameTable[18] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_newPcJobImfNameTable[19] = "\xb9\xd9\xb5\xe5";
	m_newPcJobImfNameTable[20] = "\xb9\xab\xc8\xf1";
	m_newPcJobImfNameTable[21] = "\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobImfNameTable[23] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobImfNameTable[22] = "\xb0\xe1\xc8\xa5";
	m_newPcJobImfNameTable[26] = "\xb0\xe1\xc8\xa5";
	m_newPcJobImfNameTable[27] = "\xb0\xe1\xc8\xa5";
	m_newPcJobImfNameTable[24] = "\xb0\xc7\xb3\xca";
	m_newPcJobImfNameTable[25] = "\xb4\xd1\xc0\xda";
	m_newPcJobImfNameTable[51] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobImfNameTable[52] = "\xb0\xcb\xbb\xe7";
	m_newPcJobImfNameTable[53] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_newPcJobImfNameTable[54] = "\xb1\xc3\xbc\xf6";
	m_newPcJobImfNameTable[55] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobImfNameTable[56] = "\xbb\xf3\xc0\xce";
	m_newPcJobImfNameTable[57] = "\xb5\xb5\xb5\xcf";
	m_newPcJobImfNameTable[58] = "\xb1\xe2\xbb\xe7";
	m_newPcJobImfNameTable[59] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_newPcJobImfNameTable[60] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_newPcJobImfNameTable[61] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	m_newPcJobImfNameTable[62] = "\xc7\xe5\xc5\xcd";
	m_newPcJobImfNameTable[63] = "\xbe\xee\xbc\xbc\xbd\xc5";
	m_newPcJobImfNameTable[64] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_newPcJobImfNameTable[65] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobImfNameTable[66] = "\xb8\xf9\xc5\xa9";
	m_newPcJobImfNameTable[67] = "\xbc\xbc\xc0\xcc\xc1\xf6";
	m_newPcJobImfNameTable[68] = "\xb7\xce\xb1\xd7";
	m_newPcJobImfNameTable[69] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_newPcJobImfNameTable[70] = "\xb9\xd9\xb5\xe5";
	m_newPcJobImfNameTable[71] = "\xb9\xab\xc8\xf1";
	m_newPcJobImfNameTable[72] = "\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobImfNameTable[73] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobImfNameTable[74] = "\xb0\xcb\xbb\xe7";
	m_newPcJobImfNameTable[75] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_newPcJobImfNameTable[76] = "\xb1\xc3\xbc\xf6";
	m_newPcJobImfNameTable[77] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobImfNameTable[78] = "\xbb\xf3\xc0\xce";
	m_newPcJobImfNameTable[79] = "\xb5\xb5\xb5\xcf";
	m_newPcJobImfNameTable[80] = "\xb1\xe2\xbb\xe7";
	m_newPcJobImfNameTable[81] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_newPcJobImfNameTable[82] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_newPcJobImfNameTable[83] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	m_newPcJobImfNameTable[84] = "\xc7\xe5\xc5\xcd";
	m_newPcJobImfNameTable[85] = "\xbe\xee\xbc\xbc\xbd\xc5";
	m_newPcJobImfNameTable[86] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_newPcJobImfNameTable[87] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobImfNameTable[88] = "\xb8\xf9\xc5\xa9";
	m_newPcJobImfNameTable[89] = "\xbc\xbc\xc0\xcc\xc1\xf6";
	m_newPcJobImfNameTable[90] = "\xb7\xce\xb1\xd7";
	m_newPcJobImfNameTable[91] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_newPcJobImfNameTable[92] = "\xb9\xd9\xb5\xe5";
	m_newPcJobImfNameTable[93] = "\xb9\xab\xc8\xf1";
	m_newPcJobImfNameTable[94] = "\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_newPcJobImfNameTable[95] = "\xc3\xca\xba\xb8\xc0\xda";
	m_newPcJobImfNameTable[96] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_newPcJobImfNameTable[97] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_newPcJobImfNameTable[98] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_newPcJobImfNameTable[99] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_newPcJobImfNameTable[100] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_newPcJobImfNameTable[101] = "\xb1\xe2\xbb\xe7";
	m_newPcJobImfNameTable[102] = "\xbc\xbc\xc0\xcc\xc1\xf6";

	m_newPcSexNameTable.resize(2, "\xbf\xa9");
	m_newPcSexNameTable[0] = "\xbf\xa9";
	m_newPcSexNameTable[1] = "\xb3\xb2";

	m_newPcHeadNameTable_M.resize(HEADTYPE_LAST, "2");
	m_newPcHeadNameTable_M[0] = "2";
	m_newPcHeadNameTable_M[1] = "2";
	m_newPcHeadNameTable_M[2] = "1";
	m_newPcHeadNameTable_M[3] = "7";
	m_newPcHeadNameTable_M[4] = "5";
	m_newPcHeadNameTable_M[5] = "4";
	m_newPcHeadNameTable_M[6] = "3";
	m_newPcHeadNameTable_M[7] = "6";
	m_newPcHeadNameTable_M[8] = "8";
	m_newPcHeadNameTable_M[9] = "9";
	m_newPcHeadNameTable_M[10] = "10";
	m_newPcHeadNameTable_M[11] = "12";
	m_newPcHeadNameTable_M[12] = "11";
	m_newPcHeadNameTable_M[13] = "13";
	m_newPcHeadNameTable_M[14] = "14";
	m_newPcHeadNameTable_M[15] = "15";
	m_newPcHeadNameTable_M[16] = "16";
	m_newPcHeadNameTable_M[17] = "17";
	m_newPcHeadNameTable_M[18] = "18";
	m_newPcHeadNameTable_M[19] = "19";
	m_newPcHeadNameTable_M[20] = "20";
	m_newPcHeadNameTable_M[21] = "21";
	m_newPcHeadNameTable_M[22] = "22";
	m_newPcHeadNameTable_M[23] = "23";
	m_newPcHeadNameTable_M[24] = "24";
	m_newPcHeadNameTable_M[25] = "25";

	m_newPcHeadNameTable_F.resize(HEADTYPE_LAST, "2");
	m_newPcHeadNameTable_F[0] = "2";
	m_newPcHeadNameTable_F[1] = "2";
	m_newPcHeadNameTable_F[2] = "4";
	m_newPcHeadNameTable_F[3] = "7";
	m_newPcHeadNameTable_F[4] = "1";
	m_newPcHeadNameTable_F[5] = "5";
	m_newPcHeadNameTable_F[6] = "3";
	m_newPcHeadNameTable_F[7] = "6";
	m_newPcHeadNameTable_F[8] = "12";
	m_newPcHeadNameTable_F[9] = "10";
	m_newPcHeadNameTable_F[10] = "9";
	m_newPcHeadNameTable_F[11] = "11";
	m_newPcHeadNameTable_F[12] = "8";
	m_newPcHeadNameTable_F[13] = "13";
	m_newPcHeadNameTable_F[14] = "14";
	m_newPcHeadNameTable_F[15] = "15";
	m_newPcHeadNameTable_F[16] = "16";
	m_newPcHeadNameTable_F[17] = "17";
	m_newPcHeadNameTable_F[18] = "18";
	m_newPcHeadNameTable_F[19] = "19";
	m_newPcHeadNameTable_F[20] = "20";
	m_newPcHeadNameTable_F[21] = "21";
	m_newPcHeadNameTable_F[22] = "22";
	m_newPcHeadNameTable_F[23] = "23";
	m_newPcHeadNameTable_F[24] = "24";
	m_newPcHeadNameTable_F[25] = "25";

	m_newAccessoryNameTable.resize(ACCESSORY_LAST, "");
	m_newAccessoryNameTable[0] = "";
	m_newAccessoryNameTable[1] = "_\xb0\xed\xb1\xdb";
	m_newAccessoryNameTable[2] = "_\xb0\xed\xbe\xe7\xc0\xcc\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[3] = "_\xb1\xdb\xb7\xa1\xbd\xba";
	m_newAccessoryNameTable[4] = "_\xb2\xc9";
	m_newAccessoryNameTable[5] = "_\xb2\xc9\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[6] = "_\xb5\xce\xb0\xc7";
	m_newAccessoryNameTable[7] = "_\xb5\xd5\xb1\xd9\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[8] = "_\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[9] = "_\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[10] = "_\xb9\xb0\xbe\xc8\xb0\xe6";
	m_newAccessoryNameTable[11] = "_\xba\xf1\xb7\xb9\xc5\xb8";
	m_newAccessoryNameTable[12] = "_\xbc\xb1\xb1\xdb\xb7\xa1\xbd\xba";
	m_newAccessoryNameTable[13] = "_\xbe\xc8\xb4\xeb";
	m_newAccessoryNameTable[14] = "_\xc4\xb8";
	m_newAccessoryNameTable[15] = "_\xc5\xe4\xb3\xa2\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[16] = "_\xc7\xde";
	m_newAccessoryNameTable[17] = "_\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[18] = "_\xbc\xad\xc5\xac\xb8\xb4";
	m_newAccessoryNameTable[19] = "_\xc6\xbc\xbe\xc6\xb6\xf3";
	m_newAccessoryNameTable[20] = "_\xbb\xea\xc5\xb8\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[21] = "_\xc5\xce\xbc\xf6\xbf\xb0";
	m_newAccessoryNameTable[22] = "_\xc4\xe0\xbc\xf6\xbf\xb0";
	m_newAccessoryNameTable[23] = "_\xbf\xdc\xb4\xab\xbe\xc8\xb0\xe6";
	m_newAccessoryNameTable[24] = "_\xbc\xf6\xbf\xb0";
	m_newAccessoryNameTable[25] = "_\xc8\xf2\xbc\xf6\xbf\xb0";
	m_newAccessoryNameTable[26] = "_\xb0\xed\xb1\xde\xbc\xb1\xb1\xdb\xb7\xa1\xbd\xba";
	m_newAccessoryNameTable[27] = "_\xbd\xba\xc7\xc9\xb1\xdb\xb7\xa1\xbd\xba";
	m_newAccessoryNameTable[28] = "_\xbf\xd5\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[29] = "_\xbd\xba\xc0\xa7\xc6\xae\xc1\xa8\xc6\xb2";
	m_newAccessoryNameTable[30] = "_\xb0\xf1\xb5\xe7\xc7\xec\xb5\xe5\xb1\xe2\xbe\xee";
	m_newAccessoryNameTable[31] = "_\xbf\xc3\xb5\xe5\xbd\xba\xc5\xcd\xb7\xce\xb8\xc7\xbd\xba";
	m_newAccessoryNameTable[32] = "_\xbf\xfe\xbd\xba\xc5\xcf\xb1\xd7\xb7\xb9\xc0\xcc\xbd\xba";
	m_newAccessoryNameTable[33] = "_\xc4\xda\xb7\xce\xb3\xdd";
	m_newAccessoryNameTable[34] = "_\xb8\xd3\xb8\xae\xb2\xf6";
	m_newAccessoryNameTable[35] = "_\xbc\xba\xc1\xf7\xc0\xda\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[36] = "_\xc0\xa7\xc0\xfa\xb5\xe5\xc7\xde";
	m_newAccessoryNameTable[37] = "_\xc0\xe5\xbd\xc4\xbf\xeb\xc7\xd8\xb9\xd9\xb6\xf3\xb1\xe2";
	m_newAccessoryNameTable[38] = "_\xc3\xb5\xbb\xe7\xc0\xc7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[39] = "_\xbe\xc7\xb8\xb6\xc0\xc7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[40] = "_\xc7\xef\xb8\xa7";
	m_newAccessoryNameTable[41] = "_\xb8\xb6\xc1\xa6\xbd\xba\xc6\xbd\xb0\xed\xbf\xec\xc6\xae";
	m_newAccessoryNameTable[42] = "_\xbb\xf5\xc7\xcf\xbe\xe1\xbb\xd4";
	m_newAccessoryNameTable[43] = "_\xbb\xfe\xc7\xc1\xc7\xec\xb5\xe5\xb1\xe2\xbe\xee";
	m_newAccessoryNameTable[44] = "_\xb8\xe9\xbb\xe7\xc6\xf7";
	m_newAccessoryNameTable[45] = "_\xc5\xa9\xb6\xf3\xbf\xee";
	m_newAccessoryNameTable[46] = "_\xc7\xc1\xb7\xce\xc6\xe7\xb6\xf3";
	m_newAccessoryNameTable[47] = "_\xb9\xcc\xb4\xcf\xb1\xdb\xb7\xa1\xbd\xba";
	m_newAccessoryNameTable[48] = "_\xc7\xc1\xb7\xd0\xc5\xd7\xb6\xf3\xb1\xba\xb8\xf0";
	m_newAccessoryNameTable[49] = "_\xb1\xa4\xb4\xeb\xc4\xda";
	m_newAccessoryNameTable[50] = "_\xc0\xc7\xc0\xfb\xbe\xc8\xb4\xeb";
	m_newAccessoryNameTable[51] = "_\xb9\xab\xb3\xab\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[52] = "_\xbd\xc3\xc0\xa7\xbf\xeb\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[53] = "_\xbe\xc6\xc0\xcc\xbe\xf0\xc4\xc9\xc0\xce";
	m_newAccessoryNameTable[54] = "_\xb4\xe3\xb9\xe8";
	m_newAccessoryNameTable[55] = "_\xc6\xc4\xc0\xcc\xc7\xc1\xb4\xe3\xb9\xe8";
	m_newAccessoryNameTable[56] = "_\xb2\xc9\xc0\xd9";
	m_newAccessoryNameTable[57] = "_\xc7\xae\xc0\xd9";
	m_newAccessoryNameTable[58] = "_\xb8\xd3\xb8\xae\xbe\xc8\xb0\xe6";
	m_newAccessoryNameTable[59] = "_\xc1\xa4\xc1\xf6\xc7\xa5\xc1\xf6\xc6\xc7";
	m_newAccessoryNameTable[60] = "_\xc0\xc7\xbb\xe7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[61] = "_\xb8\xc1\xc0\xda\xc0\xc7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[62] = "_\xb9\xdd\xb4\xd9\xb3\xaa";
	m_newAccessoryNameTable[63] = "_\xb5\xb5\xb3\xa2\xb4\xab";
	m_newAccessoryNameTable[64] = "_\xb0\xa3\xc8\xa3\xb8\xf0";
	m_newAccessoryNameTable[65] = "_\xbd\xba\xb8\xb6\xc0\xcf";
	m_newAccessoryNameTable[66] = "_\xbd\xc9\xc1\xf6";
	m_newAccessoryNameTable[67] = "_\xbb\xf1\xb0\xab";
	m_newAccessoryNameTable[68] = "_\xbf\xc0\xc6\xe4\xb6\xf3\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[69] = "_\xbf\xb5\xc8\xa5\xb0\xed\xb8\xae";
	m_newAccessoryNameTable[70] = "_\xb1\xcd\xb8\xb6\xb0\xb3";
	m_newAccessoryNameTable[71] = "_\xbb\xe7\xbd\xbf\xbb\xd4";
	m_newAccessoryNameTable[72] = "_\xb8\xed\xbb\xe7\xbc\xf6\xc0\xc7\xbb\xe7\xb0\xfa";
	m_newAccessoryNameTable[73] = "_\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd";
	m_newAccessoryNameTable[74] = "_\xc7\xd8\xc0\xfb\xb5\xce\xb0\xc7";
	m_newAccessoryNameTable[75] = "_\xbd\xba\xc5\xa9\xb7\xa1\xc4\xa1\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[76] = "_\xb0\xc5\xbd\xc3\xb1\xe2";
	m_newAccessoryNameTable[77] = "_\xc0\xe5\xb7\xca\xb0\xc7";
	m_newAccessoryNameTable[78] = "_\xb3\xaa\xba\xf1\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[79] = "_\xbf\xeb\xc1\xa2\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[80] = "_\xbf\xac\xb1\xd8\xbc\xd2\xb5\xb5\xb1\xb8";
	m_newAccessoryNameTable[81] = "_\xbd\xba\xc5\xb8\xb4\xf5\xbd\xba\xc6\xae";
	m_newAccessoryNameTable[82] = "_\xb4\xab\xb0\xa1\xb8\xae\xb0\xb3";
	m_newAccessoryNameTable[83] = "_\xb8\xc1\xbf\xf8\xb0\xe6";
	m_newAccessoryNameTable[84] = "_\xb0\xed\xba\xed\xb8\xb0\xc1\xb7\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[85] = "_\xc3\xca\xb7\xcf\xb4\xf5\xb5\xeb\xc0\xcc";
	m_newAccessoryNameTable[86] = "_\xbf\xc0\xc5\xa9\xc1\xb7\xc7\xef\xb8\xa7";
	m_newAccessoryNameTable[94] = "_\xc7\xb2\xbb\xe7\xb6\xfb";
	m_newAccessoryNameTable[95] = "_\xbe\xc8\xc0\xfc\xb8\xf0";
	m_newAccessoryNameTable[96] = "_\xbf\xf8\xc1\xd6\xb9\xce\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[87] = "_\xc7\xec\xb5\xe5\xc6\xf9";
	m_newAccessoryNameTable[88] = "_\xc1\xea\xbe\xf3\xc5\xa9\xb6\xf3\xbf\xee";
	m_newAccessoryNameTable[89] = "_\xb1\xa4\xb4\xeb\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[90] = "_\xbb\xea\xbc\xd2\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[91] = "_\xb0\xa1\xbd\xba\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[92] = "_\xc7\xcf\xc6\xae\xc6\xc4\xbf\xee\xb5\xa5\xc0\xcc\xbc\xc7";
	m_newAccessoryNameTable[93] = "_\xb7\xce\xb5\xe5\xbc\xad\xc5\xac\xb8\xb4";
	m_newAccessoryNameTable[97] = "_\xbe\xc8\xc5\xd7\xb3\xaa";
	m_newAccessoryNameTable[98] = "_\xc7\xd0\xbb\xe7\xb8\xf0";
	m_newAccessoryNameTable[99] = "_\xb7\xce\xb5\xe5\xc4\xab\xc8\xa3\xc0\xc7\xbb\xd4";
	m_newAccessoryNameTable[100] = "_\xc7\xc9\xc7\xef\xb8\xa7";
	m_newAccessoryNameTable[101] = "_\xc0\xe5\xbd\xc4\xbf\xeb\xbe\xcb\xb2\xae\xc1\xfa";
	m_newAccessoryNameTable[102] = "_\xc7\xd0\xbb\xfd\xb8\xf0";
	m_newAccessoryNameTable[103] = "_\xba\xbb\xc7\xef\xb8\xa7";
	m_newAccessoryNameTable[104] = "_\xb1\xea\xc5\xd0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[105] = "_\xc4\xbf\xbc\xbc\xbe\xee";
	m_newAccessoryNameTable[106] = "_\xc4\xab\xc7\xc1\xb6\xf3\xc0\xc7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[107] = "_\xb5\xb7\xc0\xd2\xc0\xba\xc0\xda\xc0\xc7\xb8\xb6\xc0\xbd";
	m_newAccessoryNameTable[108] = "_\xba\xb8\xb5\xfb\xb8\xae\xb8\xf0";
	m_newAccessoryNameTable[109] = "_\xc4\xc9\xc0\xcc\xc5\xa9\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[110] = "_\xc3\xb5\xbb\xe7\xc0\xc7\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[111] = "_\xbf\xe4\xb8\xae\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[112] = "_\xb8\xb6\xb9\xfd\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[113] = "_\xc3\xd0\xba\xd2";
	m_newAccessoryNameTable[114] = "_\xbd\xba\xc6\xf7\xbe\xc6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[115] = "_\xc6\xd2\xb4\xf5\xb0\xf5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[116] = "_\xb1\xa4\xbb\xea\xc7\xef\xb8\xe4";
	m_newAccessoryNameTable[117] = "_\xb3\xaa\xb5\xe9\xc0\xcc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[118] = "_\xb3\xca\xb1\xb8\xb8\xae\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[119] = "_\xc0\xfc\xb1\xb8\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[120] = "_\xc6\xf7\xb8\xb5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[121] = "_\xbd\xca\xc0\xda\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[122] = "_\xbf\xad\xb8\xc5\xb2\xae\xc1\xfa";
	m_newAccessoryNameTable[123] = "_\xbb\xf5\xb3\xa2\xbe\xc7\xb8\xb6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[124] = "_\xbe\xcb\xb7\xcf\xb4\xde\xb7\xcf\xbe\xcb\xb2\xae\xc1\xfa";
	m_newAccessoryNameTable[125] = "_\xb9\xdf\xb1\xd7\xb7\xb9";
	m_newAccessoryNameTable[126] = "_\xc7\xcf\xc6\xae\xb8\xd3\xb8\xae\xc7\xc9";
	m_newAccessoryNameTable[127] = "_\xb8\xd3\xb8\xae\xc6\xf7\xc0\xda\xb1\xe2";
	m_newAccessoryNameTable[128] = "_\xbf\xc0\xc6\xe4\xb6\xf3\xc0\xaf\xb7\xc9\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[129] = "_\xbe\xc7\xb8\xb6\xc0\xc7\xb3\xaf\xb0\xb3";
	m_newAccessoryNameTable[130] = "_\xb8\xb6\xbc\xfa\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[131] = "_\xc6\xd0\xbc\xc7\xbc\xb1\xb1\xdb\xb7\xa1\xbd\xba";
	m_newAccessoryNameTable[132] = "_\xc3\xca\xbd\xc2\xb4\xde\xb8\xd3\xb8\xae\xc7\xc9";
	m_newAccessoryNameTable[133] = "_\xc1\xd9\xb9\xab\xb4\xcc\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[134] = "_\xbd\xc5\xba\xf1\xc7\xd1\xbf\xad\xb8\xc5\xb2\xae\xc1\xfa";
	m_newAccessoryNameTable[135] = "_\xb0\xed\xbe\xe7\xc0\xcc\xb9\xe6\xbf\xef";
	m_newAccessoryNameTable[136] = "_\xc3\xbb\xbb\xf6\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[137] = "_\xbd\xba\xc7\xce\xc5\xa9\xbd\xba\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[138] = "_\xc5\xc2\xbe\xe7\xbd\xc5\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[139] = "_\xba\xbb\xb0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[140] = "_\xbb\xe7\xb6\xfb\xc0\xc7\xbf\xad\xb8\xc5";
	m_newAccessoryNameTable[141] = "_\xb0\xcb\xc0\xba\xb0\xed\xbe\xe7\xc0\xcc\xb1\xcd";
	m_newAccessoryNameTable[142] = "_\xb4\xc3\xbe\xee\xc1\xf8\xb0\xed\xbe\xe7\xc0\xcc";
	m_newAccessoryNameTable[143] = "_\xb0\xf5\xb5\xb9\xc0\xcc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[144] = "_\xb2\xbf\xb1\xf2\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[145] = "_\xb2\xc9\xb8\xd3\xb8\xae\xc7\xc9";
	m_newAccessoryNameTable[146] = "_\xb9\xd0\xc2\xa4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[147] = "_\xb9\xdd\xc3\xa2\xb0\xed";
	m_newAccessoryNameTable[148] = "_\xb3\xaa\xb9\xb5\xc0\xd9\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[149] = "_\xbb\xfd\xbc\xb1\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[150] = "_\xbc\xad\xc5\xa5\xb9\xf6\xbd\xba\xbb\xd4";
	m_newAccessoryNameTable[151] = "_\xbc\xd8\xba\xea\xb7\xb9\xb7\xce";
	m_newAccessoryNameTable[152] = "_\xbe\xc7\xb8\xb6\xb3\xaf\xb0\xb3\xb1\xcd";
	m_newAccessoryNameTable[153] = "_\xbf\xa9\xbf\xec\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[154] = "_\xbf\xad\xc7\xf7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[155] = "_\xc0\xce\xb5\xf0\xbe\xc8\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[156] = "_\xc0\xce\xc5\xa5\xb9\xf6\xbd\xba\xbb\xd4";
	m_newAccessoryNameTable[157] = "_\xc0\xda\xbc\xbc\xb1\xb3\xc1\xa4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[158] = "_\xc3\xb5\xbb\xe7\xb3\xaf\xb0\xb3\xb1\xcd";
	m_newAccessoryNameTable[159] = "_\xc4\xab\xbf\xec\xba\xb8\xc0\xcc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[160] = "_\xc5\xd0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[161] = "_\xc6\xab\xb8\xb3\xb8\xd3\xb8\xae\xc7\xc9";
	m_newAccessoryNameTable[162] = "_\xc7\xd8\xb4\xde\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[163] = "_X\xc0\xda\xb8\xd3\xb8\xae\xc7\xc9";
	m_newAccessoryNameTable[164] = "_\xbf\xa9\xbf\xd5\xc0\xc7\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[166] = "_\xb9\xf6\xbc\xb8\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[167] = "_\xbb\xa1\xb0\xa3\xc5\xd7\xc0\xcf\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[168] = "_\xb1\xcd\xc2\xfa\xc0\xba\xb3\xca\xb1\xb8\xb8\xae";
	m_newAccessoryNameTable[169] = "_\xbe\xe7\xc2\xca\xb1\xcd\xb9\xd8\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[165] = "_\xb9\xcc\xbd\xba\xc6\xae\xb7\xb9\xbd\xba\xbf\xd5\xb0\xfc";
	m_newAccessoryNameTable[170] = "_\xbe\xcb\xb6\xf7\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[171] = "_\xb0\xed\xba\xed\xb8\xb0\xb0\xa1\xb8\xe9""1\xc8\xa3";
	m_newAccessoryNameTable[172] = "_\xb0\xed\xba\xed\xb8\xb0\xb0\xa1\xb8\xe9""2\xc8\xa3";
	m_newAccessoryNameTable[173] = "_\xb0\xed\xba\xed\xb8\xb0\xb0\xa1\xb8\xe9""3\xc8\xa3";
	m_newAccessoryNameTable[174] = "_\xb0\xed\xba\xed\xb8\xb0\xb0\xa1\xb8\xe9""4\xc8\xa3";
	m_newAccessoryNameTable[175] = "_\xb4\xeb\xc7\xfc\xb1\xdd\xb9\xe6\xbf\xef";
	m_newAccessoryNameTable[176] = "_\xbc\xf6\xb3\xe0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[177] = "_\xbc\xf6\xb3\xe0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[178] = "_\xbf\xc0\xc5\xa9\xc8\xf7\xbe\xee\xb7\xce\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[179] = "_\xbf\xc0\xc5\xa9\xc8\xf7\xbe\xee\xb7\xce\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[180] = "_\xbe\xee\xbb\xf5\xbd\xc5\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[181] = "_\xc5\xb8\xc0\xcc\xb0\xc5\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[182] = "_\xb0\xed\xbe\xe7\xc0\xcc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[183] = "_\xbc\xbc\xc0\xcf\xc7\xa5\xc1\xf6\xc6\xc7";
	m_newAccessoryNameTable[184] = "_\xc5\xb8\xc5\xb0\xbf\xec\xbd\xba\xb4\xab\xb0\xa1\xb8\xae\xb0\xb3";
	m_newAccessoryNameTable[185] = "_\xb9\xe8\xc6\xb2\xbf\xc2\xb6\xf3\xc0\xce";
	m_newAccessoryNameTable[186] = "_\xc7\xd8\xb9\xd9\xb6\xf3\xb1\xe2\xc7\xc9";
	m_newAccessoryNameTable[187] = "_\xb0\xcb\xc0\xba\xb4\xab\xb0\xa1\xb8\xae\xb0\xb3";
	m_newAccessoryNameTable[188] = "_\xc5\xe4\xbd\xba\xc6\xae";
	m_newAccessoryNameTable[189] = "_\xc5\xbd\xc1\xa4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[190] = "_\xc2\xf7\xbe\xe7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[191] = "_\xc1\xa5\xb2\xc0\xc1\xf6";
	m_newAccessoryNameTable[192] = "_\xbf\xc0\xb8\xae\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[193] = "_\xbd\xb4\xb3\xeb\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[194] = "_\xba\xd0\xb3\xeb\xc0\xd4";
	m_newAccessoryNameTable[195] = "_\xba\xa1\xb0\xc5\xc1\xf6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[196] = "_\xb9\xe6\xc7\xd1\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[197] = "_\xb9\xd9\xb3\xaa\xb3\xaa\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[198] = "_\xb9\xcc\xbd\xba\xc6\xbd\xb7\xce\xc1\xee";
	m_newAccessoryNameTable[199] = "_\xb0\xad\xbe\xc6\xc1\xf6\xb1\xcd";
	m_newAccessoryNameTable[200] = "_\xc1\xf6\xb8\xa3\xc5\xb8\xbd\xba\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[201] = "_\xbb\xf5\xb5\xd5\xc1\xf6";
	m_newAccessoryNameTable[202] = "_\xb6\xf3\xc0\xcc\xbf\xc2\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[203] = "_\xc5\xac\xb7\xce\xbd\xba\xc7\xef\xb8\xe4";
	m_newAccessoryNameTable[204] = "_\xbf\xa3\xc1\xa9\xb8\xb5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[205] = "_\xbe\xe7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[206] = "_\xc8\xa3\xb9\xda\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[207] = "_\xbb\xe7\xc0\xcc\xc5\xac\xb7\xd3\xbd\xba\xbe\xc6\xc0\xcc";
	m_newAccessoryNameTable[208] = "_\xbf\xa4\xb8\xae\xbd\xba\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[209] = "_\xb8\xc5\xc1\xf7\xbe\xc6\xc0\xcc\xc1\xee";
	m_newAccessoryNameTable[210] = "_\xba\xce\xbf\xeb";
	m_newAccessoryNameTable[211] = "_\xbf\xb9\xbb\xdb\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[212] = "_\xb8\xb6\xb8\xae\xbf\xc0\xb3\xdd\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[213] = "_\xc3\xca\xbd\xc2\xb4\xde\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[214] = "_\xb0\xa1\xba\xce\xc5\xb0\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[215] = "_\xc4\xda\xb3\xa2\xb8\xae\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[216] = "_\xbe\xdf\xb1\xb8\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[217] = "_\xb1\xb9\xb0\xe6\xc0\xcf\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[218] = "_\xbd\xc7\xb9\xf6\xc6\xbc\xbe\xc6\xb6\xf3";
	m_newAccessoryNameTable[219] = "_\xb1\xa4\xb4\xeb\xb8\xf0\xc0\xda_";
	m_newAccessoryNameTable[220] = "_\xc0\xbd\xc7\xa5\xc7\xec\xb5\xe5\xc6\xf9";
	m_newAccessoryNameTable[221] = "_\xba\xc0\xb0\xfc";
	m_newAccessoryNameTable[222] = "_\xbf\xa3\xc1\xa9\xb8\xb5\xc7\xc9";
	m_newAccessoryNameTable[223] = "_\xb0\xa2\xbd\xc3\xc5\xbb";
	m_newAccessoryNameTable[224] = "_\xb1\xea\xc5\xd0\xba\xa3\xb7\xb9\xb8\xf0";
	m_newAccessoryNameTable[225] = "_\xb9\xdf\xc5\xb0\xb8\xae\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[226] = "_\xba\xa3\xb7\xb9\xb8\xf0";
	m_newAccessoryNameTable[227] = "_\xbb\xe7\xb6\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[228] = "_\xbe\xc6\xbe\xe4";
	m_newAccessoryNameTable[229] = "_\xc0\xcd\xb8\xed\xbf\xe4\xc3\xbb";
	m_newAccessoryNameTable[230] = "_\xc7\xcf\xc8\xb8\xc5\xbb";
	m_newAccessoryNameTable[231] = "_\xbb\xe7\xc0\xda\xc5\xbb";
	m_newAccessoryNameTable[232] = "_\xb0\xf1\xb5\xe5\xc6\xbc\xbe\xc6\xb6\xf3";
	m_newAccessoryNameTable[233] = "_\xbe\xcf\xc8\xe6\xc0\xc7\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[234] = "_\xb0\xad\xbe\xc6\xc1\xf6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[235] = "_\xbb\xf5\xb5\xd5\xc1\xf6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[236] = "_\xbc\xb1\xc0\xe5\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[237] = "_\xbf\xf9\xb0\xe8\xbc\xf6\xb0\xfc";
	m_newAccessoryNameTable[238] = "_\xc0\xe5\xbd\xc4\xbf\xeb\xc1\xf6\xbf\xc0\xb1\xd7\xb7\xa1\xc6\xdb";
	m_newAccessoryNameTable[239] = "_\xc6\xae\xc0\xa9\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[240] = "_\xc0\xbd\xc0\xaf\xbd\xc3\xc0\xce\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[241] = "_\xb3\xab\xbf\xb1";
	m_newAccessoryNameTable[242] = "_\xb8\xae\xba\xbb_\xb0\xcb\xc1\xa4";
	m_newAccessoryNameTable[243] = "_\xb8\xae\xba\xbb_\xb3\xeb\xb6\xfb";
	m_newAccessoryNameTable[244] = "_\xb8\xae\xba\xbb_\xc3\xca\xb7\xcf";
	m_newAccessoryNameTable[245] = "_\xb8\xae\xba\xbb_\xba\xd0\xc8\xab";
	m_newAccessoryNameTable[246] = "_\xb8\xae\xba\xbb_\xbb\xa1\xb0\xad";
	m_newAccessoryNameTable[247] = "_\xb8\xae\xba\xbb_\xc1\xd6\xc8\xab";
	m_newAccessoryNameTable[248] = "_\xb8\xae\xba\xbb_\xc7\xcf\xbe\xe7";
	m_newAccessoryNameTable[249] = "_\xb4\xc3\xbe\xee\xc1\xf8\xc5\xe4\xb3\xa2";
	m_newAccessoryNameTable[250] = "_\xb8\xde\xb1\xe4\xc4\xb8";
	m_newAccessoryNameTable[251] = "_\xc7\xc1\xb8\xae\xc4\xab\xbc\xad\xc5\xac\xb8\xb4";
	m_newAccessoryNameTable[252] = "_\xc4\xda\xc6\xfa\xb6\xf3";
	m_newAccessoryNameTable[253] = "_\xb8\xde\xb7\xd5\xb0\xa1\xb8\xe9";
	m_newAccessoryNameTable[254] = "_\xbf\xef\xc4\xb8";
	m_newAccessoryNameTable[255] = "_\xc3\xb5\xbb\xe7\xc0\xc7\xc0\xd4\xb8\xc2\xc3\xe3";
	m_newAccessoryNameTable[256] = "_\xb8\xf0\xb8\xa3\xc7\xc7\xbc\xc5\xbd\xba\xb5\xce\xb0\xc7";
	m_newAccessoryNameTable[257] = "_\xb8\xf0\xb8\xae\xbe\xc6\xb3\xd7\xc7\xef\xb8\xa7";
	m_newAccessoryNameTable[258] = "_\xb0\xd4\xba\xea\xb3\xd7\xc0\xcc\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[259] = "_\xc7\xcf\xbe\xe1\xb2\xc9\xc0\xd9";
	m_newAccessoryNameTable[260] = "_\xb1\xdd\xbb\xf6\xb0\xed\xb8\xae";
	m_newAccessoryNameTable[261] = "_\xbf\xf9\xb0\xe8\xb0\xfc";
	m_newAccessoryNameTable[262] = "_\xb6\xf3\xc0\xcc\xb5\xe5\xbf\xf6\xb5\xe5";
	m_newAccessoryNameTable[263] = "_\xb3\xeb\xb6\xf5\xbe\xdf\xb1\xb8\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[264] = "_\xba\xf1\xbb\xf3\xc7\xcf\xb4\xc2\xc3\xb5\xbb\xe7";
	m_newAccessoryNameTable[265] = "_\xb5\xe5\xb7\xb9\xbd\xba\xc7\xde";
	m_newAccessoryNameTable[266] = "_\xbb\xf5\xc6\xb2\xb6\xf3\xc0\xcc\xc6\xae\xc7\xec\xbe\xee\xb9\xea\xb5\xe5";
	m_newAccessoryNameTable[267] = "_\xb0\xcb\xc0\xba\xc5\xe4\xb3\xa2\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[268] = "_\xbf\xf9\xbe\xdf\xc8\xad\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[269] = "_\xb8\xb6\xc4\xaa\xc7\xde";
	m_newAccessoryNameTable[270] = "_\xc7\xc1\xb7\xce\xc6\xe7\xb6\xf3_";
	m_newAccessoryNameTable[271] = "_\xbb\xa1\xb0\xad\xbb\xf5\xb3\xa2\xbe\xc7\xb8\xb6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[272] = "_\xc8\xf2\xbb\xf6\xbb\xf5\xb3\xa2\xbe\xc7\xb8\xb6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[273] = "_\xc8\xb8\xbb\xf6\xbb\xf5\xb3\xa2\xbe\xc7\xb8\xb6\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[274] = "_\xc8\xf2\xbb\xf6\xb4\xc3\xbe\xee\xc1\xf8\xb0\xed\xbe\xe7\xc0\xcc";
	m_newAccessoryNameTable[275] = "_\xc8\xb8\xbb\xf6\xb4\xc3\xbe\xee\xc1\xf8\xb0\xed\xbe\xe7\xc0\xcc";
	m_newAccessoryNameTable[276] = "_\xba\xd0\xc8\xab\xb4\xc3\xbe\xee\xc1\xf8\xb0\xed\xbe\xe7\xc0\xcc";
	m_newAccessoryNameTable[277] = "_\xc6\xc4\xb6\xfb\xb4\xc3\xbe\xee\xc1\xf8\xb0\xed\xbe\xe7\xc0\xcc";
	m_newAccessoryNameTable[278] = "_\xb3\xeb\xb6\xfb\xb4\xc3\xbe\xee\xc1\xf8\xb0\xed\xbe\xe7\xc0\xcc";
	m_newAccessoryNameTable[279] = "_\xc8\xb8\xbb\xf6\xc5\xd0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[280] = "_\xc6\xc4\xb6\xfb\xc5\xd0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[281] = "_\xba\xd0\xc8\xab\xc5\xd0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[282] = "_\xbb\xa1\xb0\xad\xb8\xb6\xb9\xfd\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[283] = "_\xc8\xf2\xbb\xf6\xb8\xb6\xb9\xfd\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[284] = "_\xc8\xb8\xbb\xf6\xb8\xb6\xb9\xfd\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[285] = "_\xc6\xc4\xb6\xfb\xb8\xb6\xb9\xfd\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[286] = "_\xb3\xeb\xb6\xfb\xb8\xb6\xb9\xfd\xbb\xe7\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[287] = "_\xb1\xcd\xb8\xb6\xb0\xb3\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[288] = "_\xb4\xab\xb0\xa1\xb8\xae\xb0\xb3";
	m_newAccessoryNameTable[289] = "_\xbc\xb1\xb1\xdb\xb7\xa1\xbd\xba";
	m_newAccessoryNameTable[290] = "_\xc3\xb5\xbb\xe7\xc0\xc7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[291] = "_\xbe\xc7\xb8\xb6\xc0\xc7\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[292] = "_\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd";
	m_newAccessoryNameTable[293] = "_\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd";
	m_newAccessoryNameTable[294] = "_\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd";
	m_newAccessoryNameTable[295] = "_\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd";
	m_newAccessoryNameTable[296] = "_\xb4\xc3\xbe\xee\xc1\xf8\xb1\xb8\xb9\xcc\xc8\xa3";
	m_newAccessoryNameTable[297] = "_\xb8\xae\xc7\xc1\xc0\xce\xc7\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[298] = "_\xb5\xa5\xba\xf4\xb8\xb5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[299] = "_\xc6\xf7\xb8\xb5\xb0\xe3\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[300] = "_\xb9\xdf\xc5\xb0\xb8\xae\xb1\xea\xc5\xd0\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[301] = "_\xbc\xd2\xbf\xef\xb8\xae\xbd\xba\xc0\xae";
	m_newAccessoryNameTable[302] = "_\xbe\xc6\xc7\xc1\xb7\xce\xb0\xa1\xb9\xdf";
	m_newAccessoryNameTable[303] = "_\xbe\xc6\xc6\xbc\xbe\xc6\xc6\xbc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[304] = "_\xc8\xa3\xc1\xd6\xb1\xb9\xb1\xe2\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[305] = "_\xc7\xe0\xba\xb9\xc7\xd1\xb0\xa1\xb9\xdf";
	m_newAccessoryNameTable[306] = "_\xb4\xab\xba\xce\xbd\xc5\xb0\xa1\xb9\xdf";
	m_newAccessoryNameTable[307] = "_\xb3\xee\xb6\xf3\xbf\xee\xb0\xa1\xb9\xdf";
	m_newAccessoryNameTable[308] = "_\xc8\xaf\xbb\xf3\xc0\xfb\xc0\xce\xb0\xa1\xb9\xdf";
	m_newAccessoryNameTable[309] = "_\xb3\xeb\xb6\xf5\xbb\xf6\xb5\xce\xb0\xc7";
	m_newAccessoryNameTable[310] = "_\xb3\xeb\xb6\xf5\xbb\xf6\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[311] = "_\xba\xb4\xbe\xc6\xb8\xae\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[312] = "_\xbc\xf6\xb7\xc3\xbf\xd5\xb0\xfc";
	m_newAccessoryNameTable[313] = "_\xc7\xb3\xc2\xf7\xba\xf1\xb3\xe0";
	m_newAccessoryNameTable[314] = "_\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[315] = "_\xb9\xd9\xc4\xb2\xbd\xba\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[316] = "_\xbb\xa1\xb0\xa3\xbe\xc8\xb0\xe6";
	m_newAccessoryNameTable[317] = "_\xb9\xd9\xb4\xd2\xb9\xcc\xb8\xa3\xbd\xba\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[318] = "_\xc1\xd6\xc0\xfc\xc0\xda\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[319] = "_\xbf\xeb\xc0\xc7\xc7\xd8\xb0\xf1";
	m_newAccessoryNameTable[320] = "_\xb6\xf3\xb8\xe9\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[321] = "_\xc0\xa7\xbd\xba\xc6\xdb\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[322] = "_\xbb\xd4\xc5\xf5\xb1\xb8\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[323] = "_\xb7\xaf\xbd\xc3\xbe\xc6\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[324] = "_\xbf\xac\xb2\xc9\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[325] = "_\xc8\xad\xb0\xfc";
	m_newAccessoryNameTable[326] = "_\xb8\xcd\xb8\xf1\xb8\xf0";
	m_newAccessoryNameTable[327] = "_\xc7\xd8\xc0\xfb\xc0\xc7\xb4\xeb\xb0\xc5";
	m_newAccessoryNameTable[328] = "_\xc7\xc1\xb7\xb9\xc0\xcc\xbe\xdf\xc0\xc7\xc5\xa9\xb6\xf3\xbf\xee";
	m_newAccessoryNameTable[329] = "_\xbf\xad\xb4\xeb\xb0\xfa\xc0\xcf\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[330] = "_\xba\xea\xb6\xf3\xc1\xfa\xb1\xb9\xb1\xe2\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[331] = "_\xb8\xf0\xc8\xf7\xc4\xad";
	m_newAccessoryNameTable[332] = "_\xb9\xdd\xc2\xa6\xc0\xfc\xb1\xb8\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[333] = "_\xc5\xab\xc7\xcf\xc0\xcc\xba\xf1\xbd\xba\xc4\xbf\xbd\xba";
	m_newAccessoryNameTable[334] = "_\xbe\xc6\xbf\xe4\xc5\xb8\xbe\xdf\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[335] = "_\xb4\xd9\xc0\xcc\xbe\xc6\xb5\xa9";
	m_newAccessoryNameTable[336] = "_\xc7\xcf\xc5\xb0\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[337] = "_\xbf\xc9\xc0\xfa\xb9\xf6";
	m_newAccessoryNameTable[338] = "_\xbf\xec\xbb\xea\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[339] = "_\xbe\xee\xba\xce\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[340] = "_\xc6\xf7\xb8\xb5\xc6\xc4\xc6\xbc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[341] = "_\xc6\xc4\xb6\xf5\xbf\xa3\xc1\xa9\xb8\xb5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[342] = "_\xc0\xce\xb5\xb5\xb3\xd7\xbd\xc3\xbe\xc6\xc7\xec\xbe\xee\xb9\xea\xb5\xe5";
	m_newAccessoryNameTable[343] = "_\xbd\xba\xc4\xab\xc7\xc1";
	m_newAccessoryNameTable[344] = "_\xb8\xae\xc6\xb2\xbf\xa3\xc1\xa9\xb5\xb9";
	m_newAccessoryNameTable[345] = "_\xb7\xce\xba\xb8\xbe\xc6\xc0\xcc";
	m_newAccessoryNameTable[346] = "_\xb7\xce\xc5\xb0\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[347] = "_\xc0\xfc\xc6\xc4\xbe\xc8\xc5\xd7\xb3\xaa";
	m_newAccessoryNameTable[348] = "_\xb3\xaf\xb0\xed\xbd\xcd\xc0\xba\xbf\xa3\xc1\xa9\xb8\xb5";
	m_newAccessoryNameTable[349] = "_\xc6\xfa\xc2\xa6\xc0\xcc\xb4\xc2\xc6\xf7\xb8\xb5";
	m_newAccessoryNameTable[350] = "_\xb1\xe6\xb5\xe5\xbf\xf8\xb8\xf0\xc1\xfd\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[351] = "_\xc6\xc4\xc6\xbc\xb1\xb8\xc7\xd4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[353] = "_\xc4\xa3\xb1\xb8\xb1\xb8\xc7\xd4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[352] = "_\xb3\xb2\xc4\xa3\xb1\xb8\xc7\xd4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[354] = "_\xb5\xf0\xc6\xf7\xb8\xa3\xc5\xd7\xc0\xce\xc7\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[355] = "_\xb1\xdb\xb6\xf3\xb8\xae\xbd\xba\xc0\xce\xc7\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[356] = "_\xbc\xd2\xb8\xb0\xc0\xce\xc7\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[357] = "_\xc5\xd7\xc0\xcf\xb8\xb5\xc0\xce\xc7\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[358] = "_\xba\xf1\xb4\xd6\xc0\xce\xc7\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[359] = "_\xb4\xf5\xba\xed\xb7\xf9\xc0\xce\xc7\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[360] = "_\xbf\xa9\xc4\xa3\xb1\xb8\xc7\xd4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[361] = "_\xc6\xf7\xc6\xf7\xb8\xb5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[362] = "_\xb9\xab\xbd\xbd\xb8\xb2\xb3\xb2\xc0\xda\xb8\xf0";
	m_newAccessoryNameTable[363] = "_\xb9\xab\xbd\xbd\xb8\xb2\xbf\xa9\xc0\xda\xb8\xf0";
	m_newAccessoryNameTable[364] = "_\xb4\xd9\xc5\xa9\xb9\xd9\xbd\xc7\xb8\xae\xbf\xf2";
	m_newAccessoryNameTable[365] = "_\xbd\xc2\xb8\xae\xc0\xc7\xb3\xaf\xb0\xb3";
	m_newAccessoryNameTable[366] = "_\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda\xb3\xaf\xb0\xb3\xb1\xcd";
	m_newAccessoryNameTable[367] = "_\xbc\xb1\xc0\xe5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[368] = "_\xc8\xd6\xc4\xc9\xb9\xd9\xc0\xce\xc0\xc7\xb1\xcd";
	m_newAccessoryNameTable[369] = "_\xb0\xbb\xbd\xba\xc4\xab\xc7\xc1";
	m_newAccessoryNameTable[370] = "_\xb4\xd1\xc0\xda\xc0\xc7\xb5\xce\xb7\xe7\xb8\xb6\xb8\xae";
	m_newAccessoryNameTable[371] = "_\xbd\xc9\xbf\xac\xc0\xc7\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[372] = "_\xc8\xe6\xbb\xe7\xbf\xd5\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[373] = "_\xb0\xe8\xb6\xf5\xc8\xc4\xb6\xf3\xc0\xcc";
	m_newAccessoryNameTable[374] = "_\xbf\xd5\xc0\xc7\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[375] = "_\xc7\xfd\xb1\xba\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[376] = "_\xb3\xad\xc3\xca\xb8\xd3\xb8\xae\xb6\xec";
	m_newAccessoryNameTable[377] = "_\xc6\xc7\xb0\xfc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[378] = "_\xb4\xc3\xbe\xee\xc1\xf8\xc7\xcf\xbe\xe1\xb0\xed\xbe\xe7\xc0\xcc";
	m_newAccessoryNameTable[379] = "_\xb4\xd9\xc5\xa9\xb3\xd7\xbd\xba\xc7\xef\xb8\xa7";
	m_newAccessoryNameTable[380] = "_\xb4\xeb\xc7\xfc\xb8\xb6\xc1\xa6\xbd\xba\xc6\xbd\xb0\xed\xbf\xec\xc6\xae";
	m_newAccessoryNameTable[381] = "_\xb4\xeb\xc7\xfc\xbf\xc0\xc5\xa9\xc8\xf7\xbe\xee\xb7\xce\xc5\xf5\xb1\xb8";
	m_newAccessoryNameTable[382] = "_\xbb\xe7\xc5\xb8\xb4\xd0\xc3\xbc\xc0\xce";
	m_newAccessoryNameTable[383] = "_\xbe\xd8\xc6\xbc\xc5\xa9\xb4\xe3\xb9\xee\xb4\xeb";
	m_newAccessoryNameTable[384] = "_\xc5\xe4\xb3\xa2\xb1\xcd\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[385] = "_\xb9\xfa\xb7\xe9\xc7\xde";
	m_newAccessoryNameTable[386] = "_\xb9\xb0\xb0\xed\xb1\xe2\xb8\xd3\xb8\xae\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[387] = "_\xbb\xea\xc5\xb8\xc6\xf7\xb8\xb5\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[388] = "_\xb9\xe6\xbf\xef\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[389] = "_\xc7\xe5\xc6\xc3\xc4\xb8";
	m_newAccessoryNameTable[390] = "_\xbe\xe7\xb0\xa5\xb7\xa1\xbb\xea\xc5\xb8\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[391] = "_\xbf\xe4\xbf\xe4\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[392] = "_\xb3\xd7\xc4\xda\xb9\xcc\xb9\xcc\xc4\xab\xc7\xc1\xb6\xf3";
	m_newAccessoryNameTable[393] = "_\xb9\xec\xb8\xd3\xb8\xae\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[394] = "_\xc3\xb5\xbb\xe7\xc0\xc7\xc0\xaf\xb7\xc9";
	m_newAccessoryNameTable[395] = "_\xc6\xc4\xb6\xf5\xbe\xe7\xb0\xa5\xb7\xa1\xbb\xea\xc5\xb8\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[396] = "_\xb7\xef\xbe\xe7\xc0\xc7\xbc\xb3\xb3\xaf\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[397] = "_\xb9\xdf\xb7\xbb\xc5\xb8\xc0\xce\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[398] = "_\xc6\xbc\xb6\xf3\xbe\xdf\xba\xb8\xb3\xdd";
	m_newAccessoryNameTable[399] = "_\xc0\xe7\xbd\xba\xc6\xdb\xc5\xa9\xb7\xb9\xbd\xba\xc6\xae";
	m_newAccessoryNameTable[400] = "_\xbd\xba\xc4\xed\xb9\xf6\xb8\xb6\xbd\xba\xc5\xa9";
	m_newAccessoryNameTable[403] = "_\xbf\xa9\xbf\xec\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[404] = "_\xb0\xcb\xc0\xba\xbb\xd4\xc5\xd7\xbe\xc8\xb0\xe6";
	m_newAccessoryNameTable[405] = "_\xc0\xe5\xb3\xad\xb2\xd9\xb7\xaf\xb1\xe2\xbf\xe4\xc1\xa4";
	m_newAccessoryNameTable[406] = "_\xc0\xd4\xbf\xa1\xb9\xae\xb9\xb0\xb0\xed\xb1\xe2";
	m_newAccessoryNameTable[407] = "_\xba\xed\xb7\xe7\xb8\xae\xba\xbb";
	m_newAccessoryNameTable[408] = "_\xc7\xca\xb8\xae\xb8\xa3\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[409] = "_\xbe\xc6\xc4\xab\xb5\xa5\xb9\xcc\xbd\xc5\xc0\xd4\xbb\xfd\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[410] = "_\xbe\xc6\xc4\xab\xb5\xa5\xb9\xcc""1\xbc\xf6\xb7\xe1\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[401] = "_\xba\xbb\xc7\xec\xb5\xe5";
	m_newAccessoryNameTable[402] = "_\xb8\xb8\xb5\xe5\xb6\xf3\xb0\xed\xb6\xf3\xc4\xb8";
	m_newAccessoryNameTable[411] = "_\xbd\xc5\xc4\xab\xbf\xec\xba\xb8\xc0\xcc\xb8\xf0\xc0\xda";
	m_newAccessoryNameTable[412] = "_\xbb\xa7\xba\xc0\xc5\xf5""2";

	m_pcJobNameTable.resize(JT_LAST-JOBMINUS, "\xc3\xca\xba\xb8\xc0\xda\x5c\xc3\xca\xba\xb8\xc0\xda");
	m_pcJobNameTable[0] = "\xc3\xca\xba\xb8\xc0\xda\x5c\xc3\xca\xba\xb8\xc0\xda";
	m_pcJobNameTable[1] = "\xb0\xcb\xbb\xe7\x5c\xb0\xcb\xbb\xe7";
	m_pcJobNameTable[2] = "\xb8\xb6\xb9\xfd\xbb\xe7\x5c\xb8\xb6\xb9\xfd\xbb\xe7";
	m_pcJobNameTable[3] = "\xb1\xc3\xbc\xf6\x5c\xb1\xc3\xbc\xf6";
	m_pcJobNameTable[4] = "\xbc\xba\xc1\xf7\xc0\xda\x5c\xbc\xba\xc1\xf7\xc0\xda";
	m_pcJobNameTable[5] = "\xbb\xf3\xc0\xce\x5c\xbb\xf3\xc0\xce";
	m_pcJobNameTable[6] = "\xb5\xb5\xb5\xcf\x5c\xb5\xb5\xb5\xcf";
	m_pcJobNameTable[7] = "\xb1\xe2\xbb\xe7\x5c\xb1\xe2\xbb\xe7";
	m_pcJobNameTable[8] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\x5c\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_pcJobNameTable[9] = "\xc0\xa7\xc0\xfa\xb5\xe5\x5c\xc0\xa7\xc0\xfa\xb5\xe5";
	m_pcJobNameTable[10] = "\xc1\xa6\xc3\xb6\xb0\xf8\x5c\xc1\xa6\xc3\xb6\xb0\xf8";
	m_pcJobNameTable[11] = "\xc7\xe5\xc5\xcd\x5c\xc7\xe5\xc5\xcd";
	m_pcJobNameTable[12] = "\xbe\xee\xbc\xbc\xbd\xc5\x5c\xbe\xee\xbc\xbc\xbd\xc5";
	m_pcJobNameTable[13] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7\x5c\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_pcJobNameTable[14] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5\x5c\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_pcJobNameTable[15] = "\xb8\xf9\xc5\xa9\x5c\xb8\xf9\xc5\xa9";
	m_pcJobNameTable[16] = "\xbc\xbc\xc0\xcc\xc1\xf6\x5c\xbc\xbc\xc0\xcc\xc1\xf6";
	m_pcJobNameTable[17] = "\xb7\xce\xb1\xd7\x5c\xb7\xce\xb1\xd7";
	m_pcJobNameTable[18] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7\x5c\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_pcJobNameTable[19] = "\xb9\xd9\xb5\xe5\x5c\xb9\xd9\xb5\xe5";
	m_pcJobNameTable[20] = "\xb9\xab\xc8\xf1\x5c\xb9\xab\xc8\xf1";
	m_pcJobNameTable[21] = "\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5\x5c\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_pcJobNameTable[23] = "\xc3\xca\xba\xb8\xc0\xda\x5c\xc3\xca\xba\xb8\xc0\xda";
	m_pcJobNameTable[22] = "\xb0\xe1\xc8\xa5\x5c\xb0\xe1\xc8\xa5";
	m_pcJobNameTable[26] = "\xb0\xe1\xc8\xa5\x5c\xb0\xe1\xc8\xa5";
	m_pcJobNameTable[27] = "\xb0\xe1\xc8\xa5\x5c\xb0\xe1\xc8\xa5";
	m_pcJobNameTable[24] = "\xb0\xc7\xb3\xca\x5c\xb0\xc7\xb3\xca";
	m_pcJobNameTable[25] = "\xb4\xd1\xc0\xda\x5c\xb4\xd1\xc0\xda";
	m_pcJobNameTable[51] = "\xc3\xca\xba\xb8\xc0\xda\x5c\xc3\xca\xba\xb8\xc0\xda";
	m_pcJobNameTable[52] = "\xb0\xcb\xbb\xe7\x5c\xb0\xcb\xbb\xe7";
	m_pcJobNameTable[53] = "\xb8\xb6\xb9\xfd\xbb\xe7\x5c\xb8\xb6\xb9\xfd\xbb\xe7";
	m_pcJobNameTable[54] = "\xb1\xc3\xbc\xf6\x5c\xb1\xc3\xbc\xf6";
	m_pcJobNameTable[55] = "\xbc\xba\xc1\xf7\xc0\xda\x5c\xbc\xba\xc1\xf7\xc0\xda";
	m_pcJobNameTable[56] = "\xbb\xf3\xc0\xce\x5c\xbb\xf3\xc0\xce";
	m_pcJobNameTable[57] = "\xb5\xb5\xb5\xcf\x5c\xb5\xb5\xb5\xcf";
	m_pcJobNameTable[58] = "\xb1\xe2\xbb\xe7\x5c\xb1\xe2\xbb\xe7";
	m_pcJobNameTable[59] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\x5c\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_pcJobNameTable[60] = "\xc0\xa7\xc0\xfa\xb5\xe5\x5c\xc0\xa7\xc0\xfa\xb5\xe5";
	m_pcJobNameTable[61] = "\xc1\xa6\xc3\xb6\xb0\xf8\x5c\xc1\xa6\xc3\xb6\xb0\xf8";
	m_pcJobNameTable[62] = "\xc7\xe5\xc5\xcd\x5c\xc7\xe5\xc5\xcd";
	m_pcJobNameTable[63] = "\xbe\xee\xbc\xbc\xbd\xc5\x5c\xbe\xee\xbc\xbc\xbd\xc5";
	m_pcJobNameTable[64] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7\x5c\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_pcJobNameTable[65] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5\x5c\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_pcJobNameTable[66] = "\xb8\xf9\xc5\xa9\x5c\xb8\xf9\xc5\xa9";
	m_pcJobNameTable[67] = "\xbc\xbc\xc0\xcc\xc1\xf6\x5c\xbc\xbc\xc0\xcc\xc1\xf6";
	m_pcJobNameTable[68] = "\xb7\xce\xb1\xd7\x5c\xb7\xce\xb1\xd7";
	m_pcJobNameTable[69] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7\x5c\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_pcJobNameTable[70] = "\xb9\xd9\xb5\xe5\x5c\xb9\xd9\xb5\xe5";
	m_pcJobNameTable[71] = "\xb9\xab\xc8\xf1\x5c\xb9\xab\xc8\xf1";
	m_pcJobNameTable[72] = "\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5\x5c\xbd\xc5\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_pcJobNameTable[73] = "\xc3\xca\xba\xb8\xc0\xda\x5c\xc3\xca\xba\xb8\xc0\xda";
	m_pcJobNameTable[74] = "\xb0\xcb\xbb\xe7\x5c\xb0\xcb\xbb\xe7";
	m_pcJobNameTable[75] = "\xb8\xb6\xb9\xfd\xbb\xe7\x5c\xb8\xb6\xb9\xfd\xbb\xe7";
	m_pcJobNameTable[76] = "\xb1\xc3\xbc\xf6\x5c\xb1\xc3\xbc\xf6";
	m_pcJobNameTable[77] = "\xbc\xba\xc1\xf7\xc0\xda\x5c\xbc\xba\xc1\xf7\xc0\xda";
	m_pcJobNameTable[78] = "\xbb\xf3\xc0\xce\x5c\xbb\xf3\xc0\xce";
	m_pcJobNameTable[79] = "\xb5\xb5\xb5\xcf\x5c\xb5\xb5\xb5\xcf";
	m_pcJobNameTable[80] = "\xb1\xe2\xbb\xe7\x5c\xb1\xe2\xbb\xe7";
	m_pcJobNameTable[81] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae\x5c\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_pcJobNameTable[82] = "\xc0\xa7\xc0\xfa\xb5\xe5\x5c\xc0\xa7\xc0\xfa\xb5\xe5";
	m_pcJobNameTable[83] = "\xc1\xa6\xc3\xb6\xb0\xf8\x5c\xc1\xa6\xc3\xb6\xb0\xf8";
	m_pcJobNameTable[84] = "\xc7\xe5\xc5\xcd\x5c\xc7\xe5\xc5\xcd";
	m_pcJobNameTable[85] = "\xbe\xee\xbc\xbc\xbd\xc5\x5c\xbe\xee\xbc\xbc\xbd\xc5";
	m_pcJobNameTable[86] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7\x5c\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_pcJobNameTable[87] = "\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5\x5c\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_pcJobNameTable[88] = "\xb8\xf9\xc5\xa9\x5c\xb8\xf9\xc5\xa9";
	m_pcJobNameTable[89] = "\xbc\xbc\xc0\xcc\xc1\xf6\x5c\xbc\xbc\xc0\xcc\xc1\xf6";
	m_pcJobNameTable[90] = "\xb7\xce\xb1\xd7\x5c\xb7\xce\xb1\xd7";
	m_pcJobNameTable[91] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7\x5c\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
	m_pcJobNameTable[92] = "\xb9\xd9\xb5\xe5\x5c\xb9\xd9\xb5\xe5";
	m_pcJobNameTable[93] = "\xb9\xab\xc8\xf1\x5c\xb9\xab\xc8\xf1";
	m_pcJobNameTable[94] = "\xb1\xb8\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5\x5c\xb1\xb8\xc6\xe4\xc4\xda\xc5\xa9\xb7\xe7\xbc\xbc\xc0\xcc\xb4\xf5";
	m_pcJobNameTable[95] = "\xc3\xca\xba\xb8\xc0\xda\x5c\xc3\xca\xba\xb8\xc0\xda";
	m_pcJobNameTable[96] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2\x5c\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_pcJobNameTable[97] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2\x5c\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_pcJobNameTable[98] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2\x5c\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_pcJobNameTable[99] = "\xc0\xa7\xc0\xfa\xb5\xe5\x5c\xc0\xa7\xc0\xfa\xb5\xe5";
	m_pcJobNameTable[100] = "\xbc\xba\xc1\xf7\xc0\xda\x5c\xbc\xba\xc1\xf7\xc0\xda";
	m_pcJobNameTable[101] = "\xb1\xe2\xbb\xe7\x5c\xb1\xe2\xbb\xe7";
	m_pcJobNameTable[102] = "\xbc\xbc\xc0\xcc\xc1\xf6\x5c\xbc\xbc\xc0\xcc\xc1\xf6";

	m_pcSexNameTable.resize(2, "\xbf\xa9");
	m_pcSexNameTable[0] = "_\xbf\xa9";
	m_pcSexNameTable[1] = "_\xb3\xb2";

	m_pcSexImfNameTable.resize(2, "f");
	m_pcSexImfNameTable[0] = "f";
	m_pcSexImfNameTable[1] = "";

	m_pcWeaponNameTable.resize(WEAPONTYPE_LAST, "");
	m_pcWeaponNameTable[0] = "";
	m_pcWeaponNameTable[1] = "_\xb4\xdc\xb0\xcb";
	m_pcWeaponNameTable[2] = "_\xb0\xcb";
	m_pcWeaponNameTable[3] = "_\xb0\xcb";
	m_pcWeaponNameTable[4] = "_\xc3\xa2";
	m_pcWeaponNameTable[5] = "_\xc3\xa2";
	m_pcWeaponNameTable[6] = "_\xb5\xb5\xb3\xa2";
	m_pcWeaponNameTable[7] = "_\xb5\xb5\xb3\xa2";
	m_pcWeaponNameTable[8] = "_\xc5\xac\xb7\xb4";
	m_pcWeaponNameTable[9] = "_\xc5\xac\xb7\xb4";
	m_pcWeaponNameTable[10] = "_\xb7\xd4\xb5\xe5";
	m_pcWeaponNameTable[23] = "_\xb7\xd4\xb5\xe5";
	m_pcWeaponNameTable[11] = "_\xc8\xb0";
	m_pcWeaponNameTable[12] = "_\xb3\xca\xc5\xac";
	m_pcWeaponNameTable[13] = "_\xbe\xc7\xb1\xe2";
	m_pcWeaponNameTable[14] = "_\xc3\xa4\xc2\xef";
	m_pcWeaponNameTable[15] = "_\xc3\xa5";
	m_pcWeaponNameTable[16] = "_\xc4\xab\xc5\xb8\xb8\xa3_\xc4\xab\xc5\xb8\xb8\xa3";
	m_pcWeaponNameTable[22] = "_\xbc\xf6\xb8\xae\xb0\xcb";
	m_pcWeaponNameTable[17] = "_\xb1\xc7\xc3\xd1";
	m_pcWeaponNameTable[18] = "_\xb1\xe2\xb0\xfc\xc3\xd1";
	m_pcWeaponNameTable[19] = "_\xb1\xe2\xb0\xfc\xc3\xd1";
	m_pcWeaponNameTable[20] = "_\xb1\xe2\xb0\xfc\xc3\xd1";
	m_pcWeaponNameTable[21] = "_\xb1\xe2\xb0\xfc\xc3\xd1";
	m_pcWeaponNameTable[25] = "_\xb4\xdc\xb0\xcb_\xb4\xdc\xb0\xcb";
	m_pcWeaponNameTable[26] = "_\xb0\xcb_\xb0\xcb";
	m_pcWeaponNameTable[27] = "_\xb5\xb5\xb3\xa2_\xb5\xb5\xb3\xa2";
	m_pcWeaponNameTable[28] = "_\xb4\xdc\xb0\xcb_\xb0\xcb";
	m_pcWeaponNameTable[29] = "_\xb4\xdc\xb0\xcb_\xb5\xb5\xb3\xa2";
	m_pcWeaponNameTable[30] = "_\xb0\xcb_\xb5\xb5\xb3\xa2";

	m_palJobNameTable.resize(JT_LAST-JOBMINUS, "\xc3\xca\xba\xb8\xc0\xda");
	m_palJobNameTable[0] = "\xc3\xca\xba\xb8\xc0\xda";
	m_palJobNameTable[1] = "\xb0\xcb\xbb\xe7";
	m_palJobNameTable[2] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_palJobNameTable[3] = "\xb1\xc3\xbc\xf6";
	m_palJobNameTable[4] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_palJobNameTable[5] = "\xbb\xf3\xc0\xce";
	m_palJobNameTable[6] = "\xb5\xb5\xb5\xcf";
	m_palJobNameTable[7] = "\xb1\xe2\xbb\xe7";
	m_palJobNameTable[8] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_palJobNameTable[9] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_palJobNameTable[10] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	m_palJobNameTable[11] = "\xc7\xe5\xc5\xcd";
	m_palJobNameTable[12] = "\xbe\xee\xbc\xbc\xbd\xc5";
	m_palJobNameTable[13] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	if (g_serviceType == ServiceKorea)
	{
		m_palJobNameTable[14] = "\xc5\xa9\xb7\xe7";
		m_palJobNameTable[15] = "\xb8\xf9\xc5\xa9";
		m_palJobNameTable[16] = "\xbc\xbc\xc0\xcc\xc1\xf6";
		m_palJobNameTable[17] = "\xb7\xce\xb1\xd7";
		m_palJobNameTable[18] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
		m_palJobNameTable[19] = "\xb9\xd9\xb5\xe5";
		m_palJobNameTable[20] = "\xb4\xed\xbc\xad";
		m_palJobNameTable[21] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xc5\xa9\xb7\xe7";
		m_palJobNameTable[87] = "\xc5\xa9\xb7\xe7";
		m_palJobNameTable[88] = "\xb8\xf9\xc5\xa9";
		m_palJobNameTable[89] = "\xbc\xbc\xc0\xcc\xc1\xf6";
		m_palJobNameTable[90] = "\xb7\xce\xb1\xd7";
		m_palJobNameTable[91] = "\xbf\xac\xb1\xdd\xbc\xfa\xbb\xe7";
		m_palJobNameTable[92] = "\xb9\xd9\xb5\xe5";
		m_palJobNameTable[93] = "\xb4\xed\xbc\xad";
		m_palJobNameTable[94] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xc5\xa9\xb7\xe7";
		m_palJobNameTable[24] = "\xb0\xc7\xb3\xca";
		m_palJobNameTable[97] = "\xb1\xc7\xbc\xba";
		m_palJobNameTable[98] = "\xb1\xc7\xbc\xba";
		m_palJobNameTable[25] = "\xb4\xd1\xc0\xda";
		m_palJobNameTable[99] = "\xbc\xd2\xbf\xef\xb8\xb5\xc4\xbf";
		m_palJobNameTable[58] = "\xb7\xce\xb5\xe5\xb3\xaa\xc0\xcc\xc6\xae";
		m_palJobNameTable[64] = "\xc6\xe4\xc4\xda\xb7\xce\xb3\xaa";
		m_palJobNameTable[72] = "\xc6\xe4\xc4\xda\xc6\xc8\xb6\xf3";
		m_palJobNameTable[62] = "\xbd\xba\xb3\xaa\xc0\xcc\xc6\xdb";
		m_palJobNameTable[68] = "\xbd\xba\xc5\xe4\xc4\xbf";
		m_palJobNameTable[63] = "\xbe\xee\xbc\xbc\xbd\xc5\xc5\xa9\xb7\xce\xbd\xba";
		m_palJobNameTable[71] = "\xc1\xfd\xbd\xc3";
		m_palJobNameTable[66] = "\xc3\xa8\xc7\xc7\xbf\xc2";
		m_palJobNameTable[70] = "\xc5\xa9\xb6\xf3\xbf\xee";
		m_palJobNameTable[69] = "\xc5\xa9\xb8\xae\xbf\xa1\xc0\xcc\xc5\xcd";
		m_palJobNameTable[65] = "\xc6\xc8\xb6\xf3\xb5\xf2";
		m_palJobNameTable[67] = "\xc7\xc1\xb7\xce\xc6\xe4\xbc\xad";
		m_palJobNameTable[60] = "\xc7\xcf\xc0\xcc\xc0\xa7\xc0\xfa\xb5\xe5";
		m_palJobNameTable[59] = "\xc7\xcf\xc0\xcc\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
		m_palJobNameTable[61] = "\xc8\xad\xc0\xcc\xc6\xae\xbd\xba\xb9\xcc\xbd\xba";
	}
	else
	{
		m_palJobNameTable[14] = "\xb1\xe2\xbb\xe7";
		m_palJobNameTable[15] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
		m_palJobNameTable[16] = "\xc0\xa7\xc0\xfa\xb5\xe5";
		m_palJobNameTable[17] = "\xbe\xee\xbc\xbc\xbd\xc5";
		m_palJobNameTable[18] = "\xc1\xa6\xc3\xb6\xb0\xf8";
		m_palJobNameTable[19] = "\xc7\xe5\xc5\xcd";
		m_palJobNameTable[20] = "\xc7\xe5\xc5\xcd";
		m_palJobNameTable[21] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
		m_palJobNameTable[87] = "\xb1\xe2\xbb\xe7";
		m_palJobNameTable[88] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
		m_palJobNameTable[89] = "\xc0\xa7\xc0\xfa\xb5\xe5";
		m_palJobNameTable[90] = "\xbe\xee\xbc\xbc\xbd\xc5";
		m_palJobNameTable[91] = "\xc1\xa6\xc3\xb6\xb0\xf8";
		m_palJobNameTable[92] = "\xc7\xe5\xc5\xcd";
		m_palJobNameTable[93] = "\xc7\xe5\xc5\xcd";
		m_palJobNameTable[94] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
		m_palJobNameTable[24] = "\xc3\xca\xba\xb8\xc0\xda";
		m_palJobNameTable[97] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
		m_palJobNameTable[98] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
		m_palJobNameTable[25] = "\xc3\xca\xba\xb8\xc0\xda";
		m_palJobNameTable[99] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
		m_palJobNameTable[58] = "\xb1\xe2\xbb\xe7";
		m_palJobNameTable[64] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
		m_palJobNameTable[72] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
		m_palJobNameTable[62] = "\xc7\xe5\xc5\xcd";
		m_palJobNameTable[68] = "\xb1\xe2\xbb\xe7";
		m_palJobNameTable[63] = "\xbe\xee\xbc\xbc\xbd\xc5";
		m_palJobNameTable[71] = "\xc7\xe5\xc5\xcd";
		m_palJobNameTable[66] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
		m_palJobNameTable[70] = "\xc7\xe5\xc5\xcd";
		m_palJobNameTable[69] = "\xc1\xa6\xc3\xb6\xb0\xf8";
		m_palJobNameTable[65] = "\xb1\xe2\xbb\xe7";
		m_palJobNameTable[67] = "\xc0\xa7\xc0\xfa\xb5\xe5";
		m_palJobNameTable[60] = "\xc0\xa7\xc0\xfa\xb5\xe5";
		m_palJobNameTable[59] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
		m_palJobNameTable[61] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	}
	m_palJobNameTable[23] = "\xbd\xb4\xc6\xdb\xb3\xeb\xba\xf1\xbd\xba";
	m_palJobNameTable[22] = "\xb0\xe1\xc8\xa5";
	m_palJobNameTable[26] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_palJobNameTable[27] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_palJobNameTable[51] = "\xc3\xca\xba\xb8\xc0\xda";
	m_palJobNameTable[52] = "\xb0\xcb\xbb\xe7";
	m_palJobNameTable[53] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_palJobNameTable[54] = "\xb1\xc3\xbc\xf6";
	m_palJobNameTable[55] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_palJobNameTable[56] = "\xbb\xf3\xc0\xce";
	m_palJobNameTable[57] = "\xb5\xb5\xb5\xcf";
	m_palJobNameTable[73] = "\xc3\xca\xba\xb8\xc0\xda";
	m_palJobNameTable[74] = "\xb0\xcb\xbb\xe7";
	m_palJobNameTable[75] = "\xb8\xb6\xb9\xfd\xbb\xe7";
	m_palJobNameTable[76] = "\xb1\xc3\xbc\xf6";
	m_palJobNameTable[77] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_palJobNameTable[78] = "\xbb\xf3\xc0\xce";
	m_palJobNameTable[79] = "\xb5\xb5\xb5\xcf";
	m_palJobNameTable[80] = "\xb1\xe2\xbb\xe7";
	m_palJobNameTable[81] = "\xc7\xc1\xb8\xae\xbd\xba\xc6\xae";
	m_palJobNameTable[82] = "\xc0\xa7\xc0\xfa\xb5\xe5";
	m_palJobNameTable[83] = "\xc1\xa6\xc3\xb6\xb0\xf8";
	m_palJobNameTable[84] = "\xc7\xe5\xc5\xcd";
	m_palJobNameTable[85] = "\xbe\xee\xbc\xbc\xbd\xc5";
	m_palJobNameTable[86] = "\xc6\xe4\xc4\xda\xc6\xe4\xc4\xda_\xb1\xe2\xbb\xe7";
	m_palJobNameTable[95] = "\xbd\xb4\xc6\xdb\xb3\xeb\xba\xf1\xbd\xba";
	m_palJobNameTable[96] = "\xc5\xc2\xb1\xc7\xbc\xd2\xb3\xe2";
	m_palJobNameTable[100] = "\xbc\xba\xc1\xf7\xc0\xda";
	m_palJobNameTable[101] = "\xb1\xe2\xbb\xe7";
	m_palJobNameTable[102] = "\xc0\xa7\xc0\xfa\xb5\xe5";

	m_shieldNameTable.resize(SHIELD_LAST, "");
	m_shieldNameTable[0] = "";
	m_shieldNameTable[1] = "_\xb0\xa1\xb5\xe5";
	m_shieldNameTable[3] = "_\xbd\xaf\xb5\xe5";
	m_shieldNameTable[4] = "_\xb9\xcc\xb7\xaf\xbd\xaf\xb5\xe5";
	m_shieldNameTable[2] = "_\xb9\xf6\xc5\xac\xb7\xaf";

}

void	CSession :: InitJobNameTable()
{
	m_jobNameTable.resize(JT_LAST, "1_ETC_01");

	m_jobNameTable[JT_NOVICE] = "Novice";
	m_jobNameTable[JT_SWORDMAN] = "Swordman";

	if (ServiceAmerica == g_serviceType)
	{
		m_jobNameTable[JT_MAGICIAN] = "Mage";
	}
	else
	{
		m_jobNameTable[JT_MAGICIAN] = "Magician";
	}

	m_jobNameTable[JT_ARCHER] = "Archer";
	m_jobNameTable[JT_ACOLYTE] = "Acolyte";
	m_jobNameTable[JT_MERCHANT] = "Merchant";
	m_jobNameTable[JT_THIEF] = "Thief";
	m_jobNameTable[JT_KNIGHT] = "Knight";
	m_jobNameTable[JT_PRIEST] = "Priest";
	m_jobNameTable[JT_WIZARD] = "Wizard";
	m_jobNameTable[JT_BLACKSMITH] = "Blacksmith";
	m_jobNameTable[JT_HUNTER] = "Hunter";
	m_jobNameTable[JT_ASSASSIN] = "Assassin";
	m_jobNameTable[JT_CHICKEN] = "Knight";
	m_jobNameTable[JT_CRUSADER] = "Crusader";
	m_jobNameTable[JT_MONK] = "Monk";
	m_jobNameTable[JT_SAGE] = "Sage";
	m_jobNameTable[JT_ROGUE] = "Rogue";
	m_jobNameTable[JT_ALCHEMIST] = "Alchemist";
	m_jobNameTable[JT_BARD] = "Bard";
	m_jobNameTable[JT_DANCER] = "Dancer";
	m_jobNameTable[JT_CHICKEN2] = "Crusader";
	m_jobNameTable[JT_SUPERNOVICE] = "Super Novice";
	m_jobNameTable[JT_GUNSLINGER] = "Gunslinger";
	m_jobNameTable[JT_NINJA] = "Ninja";
	m_jobNameTable[JT_NOVICE_H] = "Novice High";
	m_jobNameTable[JT_SWORDMAN_H] = "Swordman High";
	m_jobNameTable[JT_MAGICIAN_H] = "Magician High";
	m_jobNameTable[JT_ARCHER_H] = "Archer High";
	m_jobNameTable[JT_ACOLYTE_H] = "Acolyte High";
	m_jobNameTable[JT_MERCHANT_H] = "Merchant High";
	m_jobNameTable[JT_THIEF_H] = "Thief High";

	if (ServiceJapan == g_serviceType)
	{
		m_jobNameTable[JT_ROGUE_H] = "Chaser";
	}
	else
	{
		m_jobNameTable[JT_ROGUE_H] = "Stalker";
	}

	m_jobNameTable[JT_KNIGHT_H] = "Lord Knight";
	m_jobNameTable[JT_PRIEST_H] = "High Priest";
	m_jobNameTable[JT_WIZARD_H] = "High Wizard";
	m_jobNameTable[JT_BLACKSMITH_H] = "Whitesmith";
	m_jobNameTable[JT_HUNTER_H] = "Sniper";
	m_jobNameTable[JT_ASSASSIN_H] = "Assassin Cross";
	m_jobNameTable[JT_CHICKEN_H] = "Lord Knight";
	m_jobNameTable[JT_CRUSADER_H] = "Paladin";
	m_jobNameTable[JT_MONK_H] = "Champion";
	m_jobNameTable[JT_SAGE_H] = "Professor";
	m_jobNameTable[JT_ALCHEMIST_H] = "Creator";
	m_jobNameTable[JT_BARD_H] = "Clown";
	m_jobNameTable[JT_DANCER_H] = "Gypsy";
	m_jobNameTable[JT_CHICKEN2_H] = "Paladin";
	m_jobNameTable[JT_NOVICE_B] = "Baby";
	m_jobNameTable[JT_SWORDMAN_B] = "Baby Swordman";
	m_jobNameTable[JT_MAGICIAN_B] = "Baby Magician";
	m_jobNameTable[JT_ARCHER_B] = "Baby Archer";
	m_jobNameTable[JT_ACOLYTE_B] = "Baby Acolyte";
	m_jobNameTable[JT_MERCHANT_B] = "Baby Merchant";
	m_jobNameTable[JT_THIEF_B] = "Baby Thief";
	m_jobNameTable[JT_KNIGHT_B] = "Baby Knight";
	m_jobNameTable[JT_PRIEST_B] = "Baby Priest";
	m_jobNameTable[JT_WIZARD_B] = "Baby Wizard";
	m_jobNameTable[JT_BLACKSMITH_B] = "Baby Blacksmith";
	m_jobNameTable[JT_HUNTER_B] = "Baby Hunter";
	m_jobNameTable[JT_ASSASSIN_B] = "Baby Assassin";
	m_jobNameTable[JT_CHICKEN_B] = "Baby Knight";
	m_jobNameTable[JT_CRUSADER_B] = "Baby Crusader";
	m_jobNameTable[JT_MONK_B] = "Baby Monk";
	m_jobNameTable[JT_SAGE_B] = "Baby Sage";
	m_jobNameTable[JT_ROGUE_B] = "Baby Rogue";
	m_jobNameTable[JT_ALCHEMIST_B] = "Baby Alchemist";
	m_jobNameTable[JT_BARD_B] = "Baby Bard";
	m_jobNameTable[JT_DANCER_B] = "Baby Dancer";
	m_jobNameTable[JT_CHICKEN2_B] = "Baby Crusader";
	m_jobNameTable[JT_SUPERNOVICE_B] = "Super Baby";
	m_jobNameTable[JT_TAEKWON] = "TaeKwon Boy";
	m_jobNameTable[JT_STAR] = "Star Gladiator";
	m_jobNameTable[JT_STAR2] = "Star Gladiator";
	m_jobNameTable[JT_LINKER] = "Soul Linker";
	m_jobNameTable[JT_GANGSI] = "Munak";
	m_jobNameTable[JT_DEATHKNIGHT] = "Death Knight";
	m_jobNameTable[JT_COLLECTOR] = "Dark Collector";

	if (ServiceIndonesia == g_serviceType)
	{
		m_jobNameTable[JT_CRUSADER] = "Defender";
		m_jobNameTable[JT_CHICKEN2] = "Defender";
		m_jobNameTable[JT_CRUSADER_B] = "Baby Defender";
		m_jobNameTable[JT_CHICKEN2_B] = "Baby Defender";
	}

	if (ServicePhilippine == g_serviceType)
	{
		m_jobNameTable[JT_BLACKSMITH_H] = "Mastersmith";
		m_jobNameTable[JT_SAGE_H] = "Scholar";
		m_jobNameTable[JT_ALCHEMIST_H] = "Biochemist";
		m_jobNameTable[JT_BARD_H] = "Minstrel";
	}

	if (ServiceAmerica == g_serviceType)
	{
		m_jobNameTable[JT_NOVICE_H] = "High Novice";
		m_jobNameTable[JT_SWORDMAN_H] = "High Swordman";
		m_jobNameTable[JT_MAGICIAN_H] = "High Mage";
		m_jobNameTable[JT_ARCHER_H] = "High Archer";
		m_jobNameTable[JT_ACOLYTE_H] = "High Acolyte";
		m_jobNameTable[JT_MERCHANT_H] = "High Merchant";
		m_jobNameTable[JT_THIEF_H] = "High Thief";
		m_jobNameTable[JT_BLACKSMITH_H] = "Mastersmith";
		m_jobNameTable[JT_SAGE_H] = "Scholar";
		m_jobNameTable[JT_ALCHEMIST_H] = "Biochemist";
		m_jobNameTable[JT_BARD_H] = "Minstrel";
		m_jobNameTable[JT_STAR] = "Taekwon Master";
		m_jobNameTable[JT_STAR2] = "Taekwon Master";
	}

	m_jobNameTable[JT_WARPNPC] = "1_ETC_01";
	m_jobNameTable[JT_1_ETC_01] = "1_ETC_01";
	m_jobNameTable[JT_1_M_01] = "1_M_01";
	m_jobNameTable[JT_1_M_02] = "1_M_02";
	m_jobNameTable[JT_1_M_03] = "1_M_03";
	m_jobNameTable[JT_1_M_04] = "1_M_04";
	m_jobNameTable[JT_1_M_BARD] = "1_M_BARD";
	m_jobNameTable[JT_1_M_HOF] = "1_M_HOF";
	m_jobNameTable[JT_1_M_INNKEEPER] = "1_M_INNKEEPER";
	m_jobNameTable[JT_1_M_JOBGUIDER] = "1_M_JOBGUIDER";
	m_jobNameTable[JT_1_M_JOBTESTER] = "1_M_JOBTESTER";
	m_jobNameTable[JT_1_M_KNIGHTMASTER] = "1_M_KNIGHTMASTER";
	m_jobNameTable[JT_1_M_LIBRARYMASTER] = "1_M_LIBRARYMASTER";
	m_jobNameTable[JT_1_M_MERCHANT] = "1_M_MERCHANT";
	m_jobNameTable[JT_1_M_ORIENT01] = "1_M_ORIENT01";
	m_jobNameTable[JT_1_M_PASTOR] = "1_M_PASTOR";
	m_jobNameTable[JT_1_M_PUBMASTER] = "1_M_PUBMASTER";
	m_jobNameTable[JT_1_M_SIZ] = "1_M_SIZ";
	m_jobNameTable[JT_1_M_SMITH] = "1_M_SMITH";
	m_jobNameTable[JT_1_M_WIZARD] = "1_M_WIZARD";
	m_jobNameTable[JT_1_M_YOUNGKNIGHT] = "1_M_YOUNGKNIGHT";
	m_jobNameTable[JT_1_F_01] = "1_F_01";
	m_jobNameTable[JT_1_F_02] = "1_F_02";
	m_jobNameTable[JT_1_F_03] = "1_F_03";
	m_jobNameTable[JT_1_F_04] = "1_F_04";
	m_jobNameTable[JT_1_F_GYPSY] = "1_F_GYPSY";
	m_jobNameTable[JT_1_F_LIBRARYGIRL] = "1_F_LIBRARYGIRL";
	m_jobNameTable[JT_1_F_MARIA] = "1_F_MARIA";
	m_jobNameTable[JT_1_F_MERCHANT_01] = "1_F_MERCHANT_01";
	m_jobNameTable[JT_1_F_MERCHANT_02] = "1_F_MERCHANT_02";
	m_jobNameTable[JT_1_F_ORIENT_01] = "1_F_ORIENT_01";
	m_jobNameTable[JT_1_F_ORIENT_02] = "1_F_ORIENT_02";
	m_jobNameTable[JT_1_F_ORIENT_03] = "1_F_ORIENT_03";
	m_jobNameTable[JT_1_F_ORIENT_04] = "1_F_ORIENT_04";
	m_jobNameTable[JT_1_F_PRIEST] = "1_F_PRIEST";
	m_jobNameTable[JT_1_F_PUBGIRL] = "1_F_PUBGIRL";
	m_jobNameTable[JT_4_M_BARBER] = "4_M_BARBER";
	m_jobNameTable[JT_4_M_ORIENT01] = "4_M_ORIENT01";
	m_jobNameTable[JT_4_M_ORIENT02] = "4_M_ORIENT02";
	m_jobNameTable[JT_4_DOG01] = "4_DOG01";
	m_jobNameTable[JT_4_KID01] = "4_KID01";
	m_jobNameTable[JT_4_M_01] = "4_M_01";
	m_jobNameTable[JT_4_M_02] = "4_M_02";
	m_jobNameTable[JT_4_M_03] = "4_M_03";
	m_jobNameTable[JT_4_M_04] = "4_M_04";
	m_jobNameTable[JT_4_F_01] = "4_F_01";
	m_jobNameTable[JT_4_F_02] = "4_F_02";
	m_jobNameTable[JT_4_F_03] = "4_F_03";
	m_jobNameTable[JT_4_F_04] = "4_F_04";
	m_jobNameTable[JT_4_F_MAID] = "4_F_MAID";
	m_jobNameTable[JT_4_F_SISTER] = "4_F_SISTER";
	m_jobNameTable[JT_4W_KID] = "4W_KID";
	m_jobNameTable[JT_4W_M_01] = "4W_M_01";
	m_jobNameTable[JT_4W_M_02] = "4W_M_02";
	m_jobNameTable[JT_4W_M_03] = "4W_M_03";
	m_jobNameTable[JT_4W_SAILOR] = "4W_SAILOR";
	m_jobNameTable[JT_4W_F_01] = "4W_F_01";
	m_jobNameTable[JT_8_F] = "8_F";
	m_jobNameTable[JT_8_F_GRANDMOTHER] = "8_F_GRANDMOTHER";
	m_jobNameTable[JT_EFFECTLAUNCHER] = "1_ETC_01";
	m_jobNameTable[JT_8W_SOLDIER] = "8W_SOLDIER";
	m_jobNameTable[JT_1_M_MOC_LORD] = "1_M_MOC_LORD";
	m_jobNameTable[JT_1_M_PAY_ELDER] = "1_M_PAY_ELDER";
	m_jobNameTable[JT_1_M_PRON_KING] = "1_M_PRON_KING";
	m_jobNameTable[JT_4_M_MANAGER] = "4_M_MANAGER";
	m_jobNameTable[JT_4_M_MINISTER] = "4_M_MINISTER";
	m_jobNameTable[JT_HIDDEN_NPC] = "1_ETC_01";
	m_jobNameTable[JT_4_F_KAFRA7] = "4_F_KAFRA7";
	m_jobNameTable[JT_4_F_KAFRA6] = "4_F_KAFRA6";
	m_jobNameTable[JT_4_F_KAFRA5] = "4_F_KAFRA5";
	m_jobNameTable[JT_4_F_KAFRA4] = "4_F_KAFRA4";
	m_jobNameTable[JT_4_F_KAFRA3] = "4_F_KAFRA3";
	m_jobNameTable[JT_4_F_KAFRA2] = "4_F_KAFRA2";
	m_jobNameTable[JT_4_F_KAFRA1] = "4_F_KAFRA1";
	m_jobNameTable[JT_2_M_THIEFMASTER] = "2_M_THIEFMASTER";
	m_jobNameTable[JT_2_M_SWORDMASTER] = "2_M_SWORDMASTER";
	m_jobNameTable[JT_2_M_PHARMACIST] = "2_M_PHARMACIST";
	m_jobNameTable[JT_2_M_MOLGENSTEIN] = "2_M_MOLGENSTEIN";
	m_jobNameTable[JT_2_M_DYEINGER] = "2_M_DYEINGER";
	m_jobNameTable[JT_2_F_MAGICMASTER] = "2_F_MAGICMASTER";
	m_jobNameTable[JT_4_F_TELEPORTER] = "4_F_TELEPORTER";
	m_jobNameTable[JT_4_M_TELEPORTER] = "4_M_TELEPORTER";
	m_jobNameTable[JT_8_F_GIRL] = "8_F_GIRL";
	m_jobNameTable[JT_4_M_SNOWMAN] = "4_M_SNOWMAN";
	m_jobNameTable[JT_4_F_05] = "4_F_05";
	m_jobNameTable[JT_4_M_05] = "4_M_05";
	m_jobNameTable[JT_4_M_06] = "4_M_06";
	m_jobNameTable[JT_4_F_06] = "4_F_06";
	m_jobNameTable[JT_4_M_PIERROT] = "4_M_PIERROT";
	m_jobNameTable[JT_4_M_KID2] = "4_M_KID2";
	m_jobNameTable[JT_4_F_KID3] = "4_F_KID3";
	m_jobNameTable[JT_4_M_SANTA] = "4_M_SANTA";
	m_jobNameTable[JT_4_F_GODEMOM] = "4_F_GODEMOM";
	m_jobNameTable[JT_4_F_GON] = "4_F_GON";
	m_jobNameTable[JT_4_F_KID2] = "4_F_KID2";
	m_jobNameTable[JT_4_M_BIBI] = "4_M_BIBI";
	m_jobNameTable[JT_4_M_GEF_SOLDIER] = "4_M_GEF_SOLDIER";
	m_jobNameTable[JT_4_M_KID1] = "4_M_KID1";
	m_jobNameTable[JT_4_M_MOC_SOLDIER] = "4_M_MOC_SOLDIER";
	m_jobNameTable[JT_4_M_PAY_SOLDIER] = "4_M_PAY_SOLDIER";
	m_jobNameTable[JT_4_M_SEAMAN] = "4_M_SEAMAN";
	m_jobNameTable[JT_4_F_NACORURI] = "4_F_NACORURI";
	m_jobNameTable[JT_4_F_SHAMAN] = "4_F_SHAMAN";
	m_jobNameTable[JT_GUILD_FLAG] = "Guildflag90_1.gr2";
	m_jobNameTable[JT_1_SHADOW_NPC] = "1_SHADOW_NPC";
	m_jobNameTable[JT_4_F_07] = "4_F_07";
	m_jobNameTable[JT_4_F_JOB_ASSASSIN] = "4_F_JOB_ASSASSIN";
	m_jobNameTable[JT_4_F_JOB_BLACKSMITH] = "4_F_JOB_BLACKSMITH";
	m_jobNameTable[JT_4_F_JOB_HUNTER] = "4_F_JOB_HUNTER";
	m_jobNameTable[JT_4_F_JOB_KNIGHT] = "4_F_JOB_KNIGHT";
	m_jobNameTable[JT_4_F_NOVICE] = "4_F_NOVICE";
	m_jobNameTable[JT_4_M_JOB_ASSASSIN] = "4_M_JOB_ASSASSIN";
	m_jobNameTable[JT_4_M_JOB_BLACKSMITH] = "4_M_JOB_BLACKSMITH";
	m_jobNameTable[JT_4_M_JOB_HUNTER] = "4_M_JOB_HUNTER";
	m_jobNameTable[JT_4_M_JOB_KNIGHT1] = "4_M_JOB_KNIGHT1";
	m_jobNameTable[JT_4_M_JOB_KNIGHT2] = "4_M_JOB_KNIGHT2";
	m_jobNameTable[JT_4_M_JOB_WIZARD] = "4_M_JOB_WIZARD";
	m_jobNameTable[JT_4_BAPHOMET] = "4_BAPHOMET";
	m_jobNameTable[JT_4_DARKLORD] = "4_DARKLORD";
	m_jobNameTable[JT_4_DEVIRUCHI] = "4_DEVIRUCHI";
	m_jobNameTable[JT_8_DOPPEL] = "8_DOPPEL";
	m_jobNameTable[JT_4_F_TWGIRL] = "4_F_TWGIRL";
	m_jobNameTable[JT_4_F_TWGRANDMOM] = "4_F_TWGRANDMOM";
	m_jobNameTable[JT_4_F_TWMASKGIRL] = "4_F_TWMASKGIRL";
	m_jobNameTable[JT_4_F_TWMIDWOMAN] = "4_F_TWMIDWOMAN";
	m_jobNameTable[JT_4_M_TWBOY] = "4_M_TWBOY";
	m_jobNameTable[JT_4_M_TWMASKMAN] = "4_M_TWMASKMAN";
	m_jobNameTable[JT_4_M_TWMIDMAN] = "4_M_TWMIDMAN";
	m_jobNameTable[JT_4_M_TWOLDMAN] = "4_M_TWOLDMAN";
	m_jobNameTable[JT_4_M_TWTEAMAN] = "4_M_TWTEAMAN";
	m_jobNameTable[JT_4_M_YOYOROGUE] = "4_M_YOYOROGUE";
	m_jobNameTable[JT_8_M_TWSOLDIER] = "8_M_TWSOLDIER";
	m_jobNameTable[JT_2_M_ALCHE] = "2_M_ALCHE";
	m_jobNameTable[JT_2_M_BARD_ORIENT] = "2_M_BARD_ORIENT";
	m_jobNameTable[JT_2_M_SAGE_B] = "2_M_SAGE_B";
	m_jobNameTable[JT_2_M_SAGE_OLD] = "2_M_SAGE_OLD";
	m_jobNameTable[JT_4_F_ALCHE] = "4_F_ALCHE";
	m_jobNameTable[JT_4_F_CRU] = "4_F_CRU";
	m_jobNameTable[JT_4_F_MONK] = "4_F_MONK";
	m_jobNameTable[JT_4_F_ROGUE] = "4_F_ROGUE";
	m_jobNameTable[JT_4_M_ALCHE_A] = "4_M_ALCHE_A";
	m_jobNameTable[JT_4_M_ALCHE_B] = "4_M_ALCHE_B";
	m_jobNameTable[JT_4_M_ALCHE_C] = "4_M_ALCHE_C";
	m_jobNameTable[JT_4_M_CRU] = "4_M_CRU";
	m_jobNameTable[JT_4_M_CRU_OLD] = "4_M_CRU_OLD";
	m_jobNameTable[JT_4_M_MONK] = "4_M_MONK";
	m_jobNameTable[JT_4_M_SAGE_A] = "4_M_SAGE_A";
	m_jobNameTable[JT_4_M_SAGE_C] = "4_M_SAGE_C";
	m_jobNameTable[JT_4_F_SON] = "4_F_SON";
	m_jobNameTable[JT_4_F_JPN2] = "4_F_JPN2";
	m_jobNameTable[JT_4_F_JPN] = "4_F_JPN";
	m_jobNameTable[JT_4_F_JPNCHIBI] = "4_F_JPNCHIBI";
	m_jobNameTable[JT_4_F_JPNOBA2] = "4_F_JPNOBA2";
	m_jobNameTable[JT_4_F_JPNOBA] = "4_F_JPNOBA";
	m_jobNameTable[JT_4_M_JPN2] = "4_M_JPN2";
	m_jobNameTable[JT_4_M_JPN] = "4_M_JPN";
	m_jobNameTable[JT_4_M_JPNCHIBI] = "4_M_JPNCHIBI";
	m_jobNameTable[JT_4_M_JPNOJI2] = "4_M_JPNOJI2";
	m_jobNameTable[JT_4_M_JPNOJI] = "4_M_JPNOJI";
	m_jobNameTable[JT_8_M_JPNSOLDIER] = "8_M_JPNSOLDIER";
	m_jobNameTable[JT_8_M_JPNMASTER] = "8_M_JPNMASTER";
	m_jobNameTable[JT_4_F_JPNMU] = "4_F_JPNMU";
	m_jobNameTable[JT_4_F_UMGIRL] = "4_F_UMGIRL";
	m_jobNameTable[JT_4_F_UMOLDWOMAN] = "4_F_UMOLDWOMAN";
	m_jobNameTable[JT_4_F_UMWOMAN] = "4_F_UMWOMAN";
	m_jobNameTable[JT_4_M_UMCHIEF] = "4_M_UMCHIEF";
	m_jobNameTable[JT_4_M_UMDANCEKID2] = "4_M_UMDANCEKID2";
	m_jobNameTable[JT_4_M_UMDANCEKID] = "4_M_UMDANCEKID";
	m_jobNameTable[JT_4_M_UMKID] = "4_M_UMKID";
	m_jobNameTable[JT_4_M_UMOLDMAN] = "4_M_UMOLDMAN";
	m_jobNameTable[JT_4_M_UMSOLDIER] = "4_M_UMSOLDIER";
	m_jobNameTable[JT_4_M_SALVATION] = "4_M_SALVATION";
	m_jobNameTable[JT_4_F_NFDEADKAFRA] = "4_F_NFDEADKAFRA";
	m_jobNameTable[JT_4_F_NFDEADMGCIAN] = "4_F_NFDEADMGCIAN";
	m_jobNameTable[JT_4_F_NFLOSTGIRL] = "4_F_NFLOSTGIRL";
	m_jobNameTable[JT_4_M_NFDEADMAN2] = "4_M_NFDEADMAN2";
	m_jobNameTable[JT_4_M_NFDEADMAN] = "4_M_NFDEADMAN";
	m_jobNameTable[JT_4_M_NFDEADSWDMAN] = "4_M_NFDEADSWDMAN";
	m_jobNameTable[JT_4_M_NFLOSTMAN] = "4_M_NFLOSTMAN";
	m_jobNameTable[JT_4_M_NFMAN] = "4_M_NFMAN";
	m_jobNameTable[JT_4_NFBAT] = "4_NFBAT";
	m_jobNameTable[JT_4_NFCOCK] = "4_NFCOCK";
	m_jobNameTable[JT_4_NFCOFFIN] = "4_NFCOFFIN";
	m_jobNameTable[JT_4_NFWISP] = "4_NFWISP";
	m_jobNameTable[JT_1_F_SIGNZISK] = "1_F_SIGNZISK";
	m_jobNameTable[JT_1_M_SIGN1] = "1_M_SIGN1";
	m_jobNameTable[JT_1_M_SIGNALCHE] = "1_M_SIGNALCHE";
	m_jobNameTable[JT_1_M_SIGNART] = "1_M_SIGNART";
	m_jobNameTable[JT_1_M_SIGNMCNT] = "1_M_SIGNMCNT";
	m_jobNameTable[JT_1_M_SIGNMONK2] = "1_M_SIGNMONK2";
	m_jobNameTable[JT_1_M_SIGNMONK] = "1_M_SIGNMONK";
	m_jobNameTable[JT_1_M_SIGNROGUE] = "1_M_SIGNROGUE";
	m_jobNameTable[JT_4_F_VALKYRIE] = "4_F_VALKYRIE";
	m_jobNameTable[JT_TW_TOWER] = "TW_TOWER";
	m_jobNameTable[JT_2_M_OLDBLSMITH] = "2_M_OLDBLSMITH";
	m_jobNameTable[JT_4_F_CHNDOCTOR] = "4_F_CHNDOCTOR";
	m_jobNameTable[JT_4_F_CHNDRESS1] = "4_F_CHNDRESS1";
	m_jobNameTable[JT_4_F_CHNDRESS2] = "4_F_CHNDRESS2";
	m_jobNameTable[JT_4_F_CHNDRESS3] = "4_F_CHNDRESS3";
	m_jobNameTable[JT_4_F_CHNWOMAN] = "4_F_CHNWOMAN";
	m_jobNameTable[JT_4_M_CHN8GUEK] = "4_M_CHN8GUEK";
	m_jobNameTable[JT_4_M_CHNCOOK] = "4_M_CHNCOOK";
	m_jobNameTable[JT_4_M_CHNGENERL] = "4_M_CHNGENERL";
	m_jobNameTable[JT_4_M_CHNMAN] = "4_M_CHNMAN";
	m_jobNameTable[JT_4_M_CHNMONK] = "4_M_CHNMONK";
	m_jobNameTable[JT_4_M_CHNOLD] = "4_M_CHNOLD";
	m_jobNameTable[JT_4_M_CHNSOLDIER] = "4_M_CHNSOLDIER";
	m_jobNameTable[JT_4_M_DWARF] = "4_M_DWARF";
	m_jobNameTable[JT_4_M_GRANDMONK] = "4_M_GRANDMONK";
	m_jobNameTable[JT_4_M_ROGUE] = "4_M_ROGUE";
	m_jobNameTable[JT_4_M_DOMINO] = "4_M_DOMINO";
	m_jobNameTable[JT_4_F_DOMINO] = "4_F_DOMINO";
	m_jobNameTable[JT_4_F_ZONDAGIRL] = "4_F_ZONDAGIRL";
	m_jobNameTable[JT_4_M_REIDIN_KURS] = "4_M_REIDIN_KURS";
	m_jobNameTable[JT_4_M_ZONDAOYAJI] = "4_M_ZONDAOYAJI";
	m_jobNameTable[JT_4_M_BUDDHIST] = "4_M_BUDDHIST";
	m_jobNameTable[JT_2_BOARD1] = "2_BOARD1";
	m_jobNameTable[JT_2_BOARD2] = "2_BOARD2";
	m_jobNameTable[JT_2_BULLETIN_BOARD] = "2_BULLETIN_BOARD";
	m_jobNameTable[JT_4_F_THAIAYO] = "4_F_THAIAYO";
	m_jobNameTable[JT_4_F_THAIGIRL] = "4_F_THAIGIRL";
	m_jobNameTable[JT_4_F_THAISHAMAN] = "4_F_THAISHAMAN";
	m_jobNameTable[JT_4_M_THAIAYO] = "4_M_THAIAYO";
	m_jobNameTable[JT_4_M_THAIOLD] = "4_M_THAIOLD";
	m_jobNameTable[JT_4_M_THAIONGBAK] = "4_M_THAIONGBAK";
	m_jobNameTable[JT_CLEAR_NPC] = "CLEAR_NPC";
	m_jobNameTable[JT_4_F_RACING] = "4_F_RACING";
	m_jobNameTable[JT_4_F_EINOLD] = "4_F_EINOLD";
	m_jobNameTable[JT_4_M_EINOLD] = "4_M_EINOLD";
	m_jobNameTable[JT_4_M_EINMINER] = "4_M_EINMINER";
	m_jobNameTable[JT_4_M_DIEMAN] = "4_M_DIEMAN";
	m_jobNameTable[JT_4_F_EINWOMAN] = "4_F_EINWOMAN";
	m_jobNameTable[JT_4_M_REPAIR] = "4_M_REPAIR";
	m_jobNameTable[JT_4_M_EIN_SOLDIER] = "4_M_EIN_SOLDIER";
	m_jobNameTable[JT_4_M_YURI] = "4_M_YURI";
	m_jobNameTable[JT_4_M_EINMAN2] = "4_M_EINMAN2";
	m_jobNameTable[JT_4_M_EINMAN] = "4_M_EINMAN";
	m_jobNameTable[JT_2_F_SIGN1] = "2_F_SIGN1";
	m_jobNameTable[JT_4_BOARD3] = "4_BOARD3";
	m_jobNameTable[JT_4_BULLETIN_BOARD2] = "4_BULLETIN_BOARD2";
	m_jobNameTable[JT_4_F_AGENTKAFRA] = "4_F_AGENTKAFRA";
	m_jobNameTable[JT_4_F_KAFRA8] = "4_F_KAFRA8";
	m_jobNameTable[JT_4_F_KAFRA9] = "4_F_KAFRA9";
	m_jobNameTable[JT_4_F_LGTGIRL] = "4_F_LGTGIRL";
	m_jobNameTable[JT_4_F_LGTGRAND] = "4_F_LGTGRAND";
	m_jobNameTable[JT_4_F_OPERATION] = "4_F_OPERATION";
	m_jobNameTable[JT_4_LGTSCIENCE] = "4_LGTSCIENCE";
	m_jobNameTable[JT_4_M_LGTGRAND] = "4_M_LGTGRAND";
	m_jobNameTable[JT_4_M_LGTGUARD2] = "4_M_LGTGUARD2";
	m_jobNameTable[JT_4_M_LGTGUARD] = "4_M_LGTGUARD";
	m_jobNameTable[JT_4_M_LGTMAN] = "4_M_LGTMAN";
	m_jobNameTable[JT_4_M_LGTPOOR] = "4_M_LGTPOOR";
	m_jobNameTable[JT_4_M_OPERATION] = "4_M_OPERATION";
	m_jobNameTable[JT_4_M_PRESIDENT] = "4_M_PRESIDENT";
	m_jobNameTable[JT_4_M_REINDEER] = "4_M_REINDEER";
	m_jobNameTable[JT_4_M_ZONDAMAN] = "4_M_ZONDAMAN";
	m_jobNameTable[JT_4_M_PECOKNIGHT] = "4_M_PECOKNIGHT";
	m_jobNameTable[JT_4_CAT] = "4_CAT";
	m_jobNameTable[JT_4_F_YUNYANG] = "4_F_YUNYANG";
	m_jobNameTable[JT_4_M_OILMAN] = "4_M_OILMAN";
	m_jobNameTable[JT_4_F_CAPEGIRL] = "4_F_CAPEGIRL";
	m_jobNameTable[JT_4_M_MASKMAN] = "4_M_MASKMAN";
	m_jobNameTable[JT_4_M_SITDOWN] = "4_M_SITDOWN";
	m_jobNameTable[JT_4_F_SITDOWN] = "4_F_SITDOWN";
	m_jobNameTable[JT_4_M_ALCHE_D] = "4_M_ALCHE_D";
	m_jobNameTable[JT_4_M_ACROSS] = "4_M_ACROSS";
	m_jobNameTable[JT_4_F_ACROSS] = "4_F_ACROSS";
	m_jobNameTable[JT_4_COOK] = "4_COOK";
	m_jobNameTable[JT_4_M_LIEMAN] = "4_M_LIEMAN";
	m_jobNameTable[JT_2_POSTBOX] = "2_POSTBOX";
	m_jobNameTable[JT_4_BULL] = "4_BULL";
	m_jobNameTable[JT_4_LAM] = "4_LAM";
	m_jobNameTable[JT_4_F_HUGIRL] = "4_F_HUGIRL";
	m_jobNameTable[JT_4_F_HUGRANMA] = "4_F_HUGRANMA";
	m_jobNameTable[JT_4_F_HUWOMAN] = "4_F_HUWOMAN";
	m_jobNameTable[JT_4_F_KHELLISIA] = "4_F_KHELLISIA";
	m_jobNameTable[JT_4_F_KHELLY] = "4_F_KHELLY";
	m_jobNameTable[JT_4_M_HUBOY] = "4_M_HUBOY";
	m_jobNameTable[JT_4_M_HUGRANFA] = "4_M_HUGRANFA";
	m_jobNameTable[JT_4_M_HUMAN_01] = "4_M_HUMAN_01";
	m_jobNameTable[JT_4_M_HUMAN_02] = "4_M_HUMAN_02";
	m_jobNameTable[JT_4_M_HUMERCHANT] = "4_M_HUMERCHANT";
	m_jobNameTable[JT_4_M_HUOLDARMY] = "4_M_HUOLDARMY";
	m_jobNameTable[JT_4_M_KHKIEL] = "4_M_KHKIEL";
	m_jobNameTable[JT_4_M_KHKYEL] = "4_M_KHKYEL";
	m_jobNameTable[JT_4_M_KHMAN] = "4_M_KHMAN";
	m_jobNameTable[JT_4_F_KHWOMAN] = "4_F_KHWOMAN";
	m_jobNameTable[JT_4_F_KHGIRL] = "4_F_KHGIRL";
	m_jobNameTable[JT_4_M_KHBOY] = "4_M_KHBOY";
	m_jobNameTable[JT_4_M_PHILMAN] = "4_M_PHILMAN";
	m_jobNameTable[JT_4_PORING] = "4_PORING";
	m_jobNameTable[JT_2_COLAVEND] = "2_COLAVEND";
	m_jobNameTable[JT_4_F_SOCCER] = "4_F_SOCCER";
	m_jobNameTable[JT_4_M_SOCCER7] = "4_M_SOCCER7";
	m_jobNameTable[JT_4_M_SOCCER9] = "4_M_SOCCER9";
	m_jobNameTable[JT_4_DST_CAMEL] = "4_DST_CAMEL";
	m_jobNameTable[JT_4_DST_SOLDIER] = "4_DST_SOLDIER";
	m_jobNameTable[JT_4_F_DESERT] = "4_F_DESERT";
	m_jobNameTable[JT_4_F_DST_CHILD] = "4_F_DST_CHILD";
	m_jobNameTable[JT_4_F_DST_GRAND] = "4_F_DST_GRAND";
	m_jobNameTable[JT_4_F_CHILD] = "4_F_CHILD";
	m_jobNameTable[JT_4_F_MADAME] = "4_F_MADAME";
	m_jobNameTable[JT_4_F_MASK1] = "4_F_MASK1";
	m_jobNameTable[JT_4_F_MASK] = "4_F_MASK";
	m_jobNameTable[JT_4_F_RACHOLD] = "4_F_RACHOLD";
	m_jobNameTable[JT_4_F_SHABBY] = "4_F_SHABBY";
	m_jobNameTable[JT_4_F_TRAINEE] = "4_F_TRAINEE";
	m_jobNameTable[JT_4_M_CHILD1] = "4_M_CHILD1";
	m_jobNameTable[JT_4_M_CHILD] = "4_M_CHILD";
	m_jobNameTable[JT_4_M_DOCTOR] = "4_M_DOCTOR";
	m_jobNameTable[JT_4_M_FROZEN1] = "4_M_FROZEN1";
	m_jobNameTable[JT_4_M_FROZEN] = "4_M_FROZEN";
	m_jobNameTable[JT_4_M_MASK1] = "4_M_MASK1";
	m_jobNameTable[JT_4_M_MASK] = "4_M_MASK";
	m_jobNameTable[JT_4_M_MIDDLE1] = "4_M_MIDDLE1";
	m_jobNameTable[JT_4_M_MIDDLE] = "4_M_MIDDLE";
	m_jobNameTable[JT_4_M_RACHMAN2] = "4_M_RACHMAN2";
	m_jobNameTable[JT_4_M_RACHMAN1] = "4_M_RACHMAN1";
	m_jobNameTable[JT_4_M_RACHOLD1] = "4_M_RACHOLD1";
	m_jobNameTable[JT_4_M_RACHOLD] = "4_M_RACHOLD";
	m_jobNameTable[JT_4_M_RASWORD] = "4_M_RASWORD";
	m_jobNameTable[JT_4_M_TRAINEE] = "4_M_TRAINEE";
	m_jobNameTable[JT_4_F_ARUNA_POP] = "4_F_ARUNA_POP";
	m_jobNameTable[JT_4_M_ARUNA_NFM1] = "4_M_ARUNA_NFM1";
	m_jobNameTable[JT_4_M_DESERT] = "4_M_DESERT";
	m_jobNameTable[JT_4_M_DST_CHILD] = "4_M_DST_CHILD";
	m_jobNameTable[JT_4_M_DST_GRAND] = "4_M_DST_GRAND";
	m_jobNameTable[JT_4_M_DST_MASTER] = "4_M_DST_MASTER";
	m_jobNameTable[JT_4_M_DST_TOUGH] = "4_M_DST_TOUGH";
	m_jobNameTable[JT_4_ANGELING] = "4_ANGELING";
	m_jobNameTable[JT_4_ARCHANGELING] = "4_ARCHANGELING";
	m_jobNameTable[JT_4_GHOSTRING] = "4_GHOSTRING";
	m_jobNameTable[JT_4_F_EDEN_MASTER] = "4_F_EDEN_MASTER";
	m_jobNameTable[JT_4_F_EDEN_OFFICER] = "4_F_EDEN_OFFICER";
	m_jobNameTable[JT_4_M_EDEN_GUARDER] = "4_M_EDEN_GUARDER";
	m_jobNameTable[JT_4_M_PATRICK] = "4_M_PATRICK";
	m_jobNameTable[JT_4_DONKEY] = "4_DONKEY";
	m_jobNameTable[JT_4_M_TRISTAN] = "4_M_TRISTAN";
	m_jobNameTable[JT_4_WHITE_COW] = "4_WHITE_COW";
	m_jobNameTable[JT_4_F_RUSCHILD] = "4_F_RUSCHILD";
	m_jobNameTable[JT_4_F_RUSWOMAN1] = "4_F_RUSWOMAN1";
	m_jobNameTable[JT_4_F_RUSWOMAN2] = "4_F_RUSWOMAN2";
	m_jobNameTable[JT_4_F_RUSWOMAN3] = "4_F_RUSWOMAN3";
	m_jobNameTable[JT_4_M_RUSCHILD] = "4_M_RUSCHILD";
	m_jobNameTable[JT_4_M_GUSLIMAN] = "4_M_GUSLIMAN";
	m_jobNameTable[JT_4_M_RUSBALD] = "4_M_RUSBALD";
	m_jobNameTable[JT_4_M_RUSKING] = "4_M_RUSKING";
	m_jobNameTable[JT_4_M_RUSKNIGHT] = "4_M_RUSKNIGHT";
	m_jobNameTable[JT_4_M_RUSMAN1] = "4_M_RUSMAN1";
	m_jobNameTable[JT_4_M_RUSMAN2] = "4_M_RUSMAN2";
	m_jobNameTable[JT_4_M_DRAKE] = "4_M_DRAKE";
	m_jobNameTable[JT_4_F_BABAYAGA] = "4_F_BABAYAGA";
	m_jobNameTable[JT_4_F_RUSGREEN] = "4_F_RUSGREEN";
	m_jobNameTable[JT_4_RUS_DWOLF] = "4_RUS_DWOLF";
	m_jobNameTable[JT_1_FLAG_LION] = "1_FLAG_LION";
	m_jobNameTable[JT_1_FLAG_EAGLE] = "1_FLAG_EAGLE";
	m_jobNameTable[JT_4_M_MIKID] = "4_M_MIKID";
	m_jobNameTable[JT_4_BLUE_FLOWER] = "4_BLUE_FLOWER";
	m_jobNameTable[JT_4_RED_FLOWER] = "4_RED_FLOWER";
	m_jobNameTable[JT_4_YELL_FLOWER] = "4_YELL_FLOWER";
	m_jobNameTable[JT_4_F_CAVE1] = "4_F_CAVE1";
	m_jobNameTable[JT_4_F_MUT1] = "4_F_MUT1";
	m_jobNameTable[JT_4_F_MUT2] = "4_F_MUT2";
	m_jobNameTable[JT_4_F_SCIENCE] = "4_F_SCIENCE";
	m_jobNameTable[JT_4_M_1STPRIN1] = "4_M_1STPRIN1";
	m_jobNameTable[JT_4_M_1STPRIN2] = "4_M_1STPRIN2";
	m_jobNameTable[JT_4_M_2NDPRIN1] = "4_M_2NDPRIN1";
	m_jobNameTable[JT_4_M_2NDPRIN2] = "4_M_2NDPRIN2";
	m_jobNameTable[JT_4_M_3RDPRIN1] = "4_M_3RDPRIN1";
	m_jobNameTable[JT_4_M_3RDPRIN2] = "4_M_3RDPRIN2";
	m_jobNameTable[JT_4_M_4THPRIN1] = "4_M_4THPRIN1";
	m_jobNameTable[JT_4_M_4THPRIN2] = "4_M_4THPRIN2";
	m_jobNameTable[JT_4_M_5THPRIN1] = "4_M_5THPRIN1";
	m_jobNameTable[JT_4_M_5THPRIN2] = "4_M_5THPRIN2";
	m_jobNameTable[JT_4_M_6THPRIN1] = "4_M_6THPRIN1";
	m_jobNameTable[JT_4_M_6THPRIN2] = "4_M_6THPRIN2";
	m_jobNameTable[JT_4_M_CASMAN1] = "4_M_CASMAN1";
	m_jobNameTable[JT_4_M_CAVE1] = "4_M_CAVE1";
	m_jobNameTable[JT_4_M_MOCASS1] = "4_M_MOCASS1";
	m_jobNameTable[JT_4_M_MOCASS2] = "4_M_MOCASS2";
	m_jobNameTable[JT_4_M_MUT1] = "4_M_MUT1";
	m_jobNameTable[JT_4_M_MUT2] = "4_M_MUT2";
	m_jobNameTable[JT_4_M_SCIENCE] = "4_M_SCIENCE";
	m_jobNameTable[JT_4_F_VALKYRIE2] = "4_F_VALKYRIE2";
	m_jobNameTable[JT_4_M_UNCLEKNIGHT] = "4_M_UNCLEKNIGHT";
	m_jobNameTable[JT_4_M_YOUNGKNIGHT] = "4_M_YOUNGKNIGHT";
	m_jobNameTable[JT_2_MONEMUS] = "2_MONEMUS";
	m_jobNameTable[JT_4_M_ATEIL] = "4_M_ATEIL";
	m_jobNameTable[JT_4_F_ANNIVERSARY] = "4_F_ANNIVERSARY";
	m_jobNameTable[JT_4_M_GREATPO] = "4_M_GREATPO";
	m_jobNameTable[JT_4_M_NOVELIST] = "4_M_NOVELIST";
	m_jobNameTable[JT_4_M_CHAMPSOUL] = "4_M_CHAMPSOUL";
	m_jobNameTable[JT_4_M_OLDFRIAR] = "4_M_OLDFRIAR";
	m_jobNameTable[JT_4_M_CRU_SOLD] = "4_M_CRU_SOLD";
	m_jobNameTable[JT_4_M_CRU_KNT] = "4_M_CRU_KNT";
	m_jobNameTable[JT_4_M_CRU_HEAD] = "4_M_CRU_HEAD";
	m_jobNameTable[JT_4_M_CRU_CRUA] = "4_M_CRU_CRUA";
	m_jobNameTable[JT_4_M_KY_SOLD] = "4_M_KY_SOLD";
	m_jobNameTable[JT_4_M_KY_KNT] = "4_M_KY_KNT";
	m_jobNameTable[JT_4_M_KY_HEAD] = "4_M_KY_HEAD";
	m_jobNameTable[JT_4_M_KY_KIYOM] = "4_M_KY_KIYOM";
	m_jobNameTable[JT_4_M_BOSSCAT] = "4_M_BOSSCAT";
	m_jobNameTable[JT_4_M_BABYCAT] = "4_M_BABYCAT";
	m_jobNameTable[JT_4W_F_KAFRA2] = "4W_F_KAFRA2";
	m_jobNameTable[JT_4_F_MUNAK] = "4_F_MUNAK";
	m_jobNameTable[JT_4_M_BONGUN] = "4_M_BONGUN";
	m_jobNameTable[JT_4_BEAR] = "4_BEAR";
	m_jobNameTable[JT_4_BLUEWOLF] = "4_BLUEWOLF";
	m_jobNameTable[JT_4_PECOPECO] = "4_PECOPECO";
	m_jobNameTable[JT_4_M_JP_MID] = "4_M_JP_MID";
	m_jobNameTable[JT_4_M_JP_RUN] = "4_M_JP_RUN";
	m_jobNameTable[JT_MON_BEGIN] = "Scorpion";
	m_jobNameTable[JT_SCORPION] = "Scorpion";
	m_jobNameTable[JT_PORING] = "Poring";
	m_jobNameTable[JT_THIEF_BUG_AGG] = "Thief_bug_egg";
	m_jobNameTable[JT_HORNET] = "Hornet";
	m_jobNameTable[JT_FARMILIAR] = "Farmiliar";
	m_jobNameTable[JT_THIEF_BUG_LARVA] = "Thief_bug_larva";
	m_jobNameTable[JT_FABRE] = "Fabre";
	m_jobNameTable[JT_PUPA] = "Pupa";
	m_jobNameTable[JT_CONDOR] = "Condor";
	m_jobNameTable[JT_WILOW] = "Wilow";
	m_jobNameTable[JT_CHONCHON] = "Chocho";
	m_jobNameTable[JT_RODA_FROG] = "Roda_frog";
	m_jobNameTable[JT_WOLF] = "Wolf";
	m_jobNameTable[JT_SPORE] = "Spore";
	m_jobNameTable[JT_ZOMBIE] = "Zombie";
	m_jobNameTable[JT_ARCHER_SKELETON] = "Skel_archer";
	m_jobNameTable[JT_THIEF_BUG_FEMALE] = "Thief_bug_female";
	m_jobNameTable[JT_CREAMY] = "Creamy";
	m_jobNameTable[JT_PECOPECO] = "Pecopeco";
	m_jobNameTable[JT_MANDRAGORA] = "Mandragora";
	m_jobNameTable[JT_THIEF_BUG_MALE] = "Thief_bug_male";
	m_jobNameTable[JT_WERE_WOLF] = "Were_wolf";
	m_jobNameTable[JT_ORK_WARRIOR] = "Ork_warrior";
	m_jobNameTable[JT_WORM_TAIL] = "Worm_tail";
	m_jobNameTable[JT_SNAKE] = "Snake";
	m_jobNameTable[JT_MUNAK] = "Munak";
	m_jobNameTable[JT_RAPTICE] = "Raptice";
	m_jobNameTable[JT_SOLDIER_SKELETON] = "Skel_soldier";
	m_jobNameTable[JT_ISIS] = "Isis";
	m_jobNameTable[JT_ANACONDAQ] = "Anacondaq";
	m_jobNameTable[JT_POPORING] = "Poporing";
	m_jobNameTable[JT_VERIT] = "Vorit";
	m_jobNameTable[JT_ELDER_WILOW] = "Elder_wilow";
	m_jobNameTable[JT_THARA_FROG] = "Thara_frog";
	m_jobNameTable[JT_HUNTER_FLY] = "Hunter_fly";
	m_jobNameTable[JT_GHOUL] = "Ghoul";
	m_jobNameTable[JT_SIDE_WINDER] = "Side_winder";
	m_jobNameTable[JT_OSIRIS] = "Osiris";
	m_jobNameTable[JT_BAPHOMET] = "Baphomet";
	m_jobNameTable[JT_GOLEM] = "Golem";
	m_jobNameTable[JT_MUMMY] = "Mummy";
	m_jobNameTable[JT_STEEL_CHONCHON] = "Steel_chocho";
	m_jobNameTable[JT_SEAHORES] = "Seahores";
	m_jobNameTable[JT_OBEAUNE] = "Obeaune";
	m_jobNameTable[JT_MARC] = "Marc";
	m_jobNameTable[JT_DOPPELGANGER] = "Doppelganger";
	m_jobNameTable[JT_PECOPECO_EGG] = "Peco_egg";
	m_jobNameTable[JT_THIEF_BUG_EGG] = "Thief_bug_egg";
	m_jobNameTable[JT_PICKY] = "Picky";
	m_jobNameTable[JT_PICKY_] = "Picky_";
	m_jobNameTable[JT_THIEF_BUG] = "Thief_bug_larva";
	m_jobNameTable[JT_ROCKER] = "Rocker";
	m_jobNameTable[JT_THIEF_BUG_] = "Thief_bug_female";
	m_jobNameTable[JT_THIEF_BUG__] = "Thief_bug_male";
	m_jobNameTable[JT_MUKA] = "Muka";
	m_jobNameTable[JT_SMOKIE] = "Smokie";
	m_jobNameTable[JT_YOYO] = "Yoyo";
	m_jobNameTable[JT_METALLER] = "Metaller";
	m_jobNameTable[JT_MISTRESS] = "Mistress";
	m_jobNameTable[JT_BIGFOOT] = "Bigfoot";
	m_jobNameTable[JT_NIGHTMARE] = "Nightmare";
	m_jobNameTable[JT_PORING_] = "Poring_";
	m_jobNameTable[JT_LUNATIC] = "Lunatic";
	m_jobNameTable[JT_MEGALODON] = "MEGALODON";
	m_jobNameTable[JT_STROUF] = "STROUF";
	m_jobNameTable[JT_VADON] = "VADON";
	m_jobNameTable[JT_CORNUTUS] = "CORNUTUS";
	m_jobNameTable[JT_HYDRA] = "HYDRA";
	m_jobNameTable[JT_SWORD_FISH] = "SWORD_FISH";
	m_jobNameTable[JT_KUKRE] = "KUKRE";
	m_jobNameTable[JT_PIRATE_SKEL] = "PIRATE_SKEL";
	m_jobNameTable[JT_KAHO] = "KAHO";
	m_jobNameTable[JT_CRAB] = "CRAB";
	m_jobNameTable[JT_SHELLFISH] = "SHELLFISH";
	m_jobNameTable[JT_TURTLE] = "TURTLE";
	m_jobNameTable[JT_SKELETON] = "SKELETON";
	m_jobNameTable[JT_POISON_SPORE] = "POISON_SPORE";
	m_jobNameTable[JT_RED_PLANT] = "RED_PLANT";
	m_jobNameTable[JT_BLUE_PLANT] = "BLUE_PLANT";
	m_jobNameTable[JT_GREEN_PLANT] = "GREEN_PLANT";
	m_jobNameTable[JT_YELLOW_PLANT] = "YELLOW_PLANT";
	m_jobNameTable[JT_WHITE_PLANT] = "WHITE_PLANT";
	m_jobNameTable[JT_SHINING_PLANT] = "SHINING_PLANT";
	m_jobNameTable[JT_BLACK_MUSHROOM] = "BLACK_MUSHROOM";
	m_jobNameTable[JT_RED_MUSHROOM] = "RED_MUSHROOM";
	m_jobNameTable[JT_GOLDEN_BUG] = "GOLDEN_BUG";
	m_jobNameTable[JT_ORK_HERO] = "ORK_HERO";
	m_jobNameTable[JT_VOCAL] = "VOCAL";
	m_jobNameTable[JT_TOAD] = "TOAD";
	m_jobNameTable[JT_MASTERING] = "MASTERING";
	m_jobNameTable[JT_DRAGON_FLY] = "DRAGON_FLY";
	m_jobNameTable[JT_VAGABOND_WOLF] = "VAGABOND_WOLF";
	m_jobNameTable[JT_ECLIPSE] = "ECLIPSE";
	m_jobNameTable[JT_PREFIREPILLAR] = "1_ETC_01";
	m_jobNameTable[JT_FIREPILLAR] = "1_ETC_01";
	m_jobNameTable[JT_AMBERNITE] = "AMBERNITE";
	m_jobNameTable[JT_ANDRE] = "ANDRE";
	m_jobNameTable[JT_ANGELING] = "ANGELING";
	m_jobNameTable[JT_ANT_EGG] = "ANT_EGG";
	m_jobNameTable[JT_ANUBIS] = "ANUBIS";
	m_jobNameTable[JT_ARGIOPE] = "ARGIOPE";
	m_jobNameTable[JT_ARGOS] = "ARGOS";
	m_jobNameTable[JT_BAPHOMET_] = "BAPHOMET_";
	m_jobNameTable[JT_BATHORY] = "BATHORY";
	m_jobNameTable[JT_CARAMEL] = "CARAMEL";
	m_jobNameTable[JT_COCO] = "COCO";
	m_jobNameTable[JT_DENIRO] = "DENIRO";
	m_jobNameTable[JT_DESERT_WOLF] = "DESERT_WOLF";
	m_jobNameTable[JT_DESERT_WOLF_B] = "DESERT_WOLF_B";
	m_jobNameTable[JT_DEVIACE] = "DEVIACE";
	m_jobNameTable[JT_DEVIRUCHI] = "DEVIRUCHI";
	m_jobNameTable[JT_DOKEBI] = "DOKEBI";
	m_jobNameTable[JT_DRAINLIAR] = "DRAINLIAR";
	m_jobNameTable[JT_DRAKE] = "DRAKE";
	m_jobNameTable[JT_DROPS] = "DROPS";
	m_jobNameTable[JT_DUSTINESS] = "DUSTINESS";
	m_jobNameTable[JT_EDDGA] = "EDDGA";
	m_jobNameTable[JT_EGGYRA] = "EGGYRA";
	m_jobNameTable[JT_KOBOLD_1] = "KOBOLD_1";
	m_jobNameTable[JT_KOBOLD_2] = "KOBOLD_2";
	m_jobNameTable[JT_KOBOLD_3] = "KOBOLD_3";
	m_jobNameTable[JT_KOBOLD_4] = "KOBOLD_4";
	m_jobNameTable[JT_KOBOLD_5] = "KOBOLD_5";
	m_jobNameTable[JT_EVIL_DRUID] = "EVIL_DRUID";
	m_jobNameTable[JT_FLORA] = "FLORA";
	m_jobNameTable[JT_FRILLDORA] = "FRILLDORA";
	m_jobNameTable[JT_GHOSTRING] = "GHOSTRING";
	m_jobNameTable[JT_GIEARTH] = "GIEARTH";
	m_jobNameTable[JT_GOBLIN_1] = "GOBLIN_1";
	m_jobNameTable[JT_GOBLIN_2] = "GOBLIN_2";
	m_jobNameTable[JT_GOBLIN_3] = "GOBLIN_3";
	m_jobNameTable[JT_GOBLIN_4] = "GOBLIN_4";
	m_jobNameTable[JT_GOBLIN_5] = "GOBLIN_5";
	m_jobNameTable[JT_HODE] = "HODE";
	m_jobNameTable[JT_HORN] = "HORN";
	m_jobNameTable[JT_HORONG] = "HORONG";
	m_jobNameTable[JT_JAKK] = "JAKK";
	m_jobNameTable[JT_JOKER] = "JOKER";
	m_jobNameTable[JT_KHALITZBURG] = "KHALITZBURG";
	m_jobNameTable[JT_MAGNOLIA] = "MAGNOLIA";
	m_jobNameTable[JT_MANTIS] = "MANTIS";
	m_jobNameTable[JT_MARDUK] = "MARDUK";
	m_jobNameTable[JT_MARINA] = "MARINA";
	m_jobNameTable[JT_MARINE_SPHERE] = "MARINE_SPHERE";
	m_jobNameTable[JT_MARIONETTE] = "MARIONETTE";
	m_jobNameTable[JT_MARSE] = "MARSE";
	m_jobNameTable[JT_MARTIN] = "MARTIN";
	m_jobNameTable[JT_MATYR] = "MATYR";
	m_jobNameTable[JT_MAYA] = "MAYA";
	m_jobNameTable[JT_MEDUSA] = "MEDUSA";
	m_jobNameTable[JT_MINOROUS] = "MINOROUS";
	m_jobNameTable[JT_MOONLIGHT] = "MOONLIGHT";
	m_jobNameTable[JT_MYST] = "MYST";
	m_jobNameTable[JT_ORC_SKELETON] = "ORC_SKELETON";
	m_jobNameTable[JT_ORC_ZOMBIE] = "ORC_ZOMBIE";
	m_jobNameTable[JT_PASANA] = "PASANA";
	m_jobNameTable[JT_PETIT] = "PETIT";
	m_jobNameTable[JT_PETIT_] = "PETIT_";
	m_jobNameTable[JT_PHARAOH] = "PHARAOH";
	m_jobNameTable[JT_PHEN] = "PHEN";
	m_jobNameTable[JT_PHREEONI] = "PHREEONI";
	m_jobNameTable[JT_PIERE] = "PIERE";
	m_jobNameTable[JT_PLANKTON] = "PLANKTON";
	m_jobNameTable[JT_RAFFLESIA] = "RAFFLESIA";
	m_jobNameTable[JT_RAYDRIC] = "RAYDRIC";
	m_jobNameTable[JT_REQUIEM] = "REQUIEM";
	m_jobNameTable[JT_SAND_MAN] = "SAND_MAN";
	m_jobNameTable[JT_SAVAGE] = "SAVAGE";
	m_jobNameTable[JT_SAVAGE_BABE] = "SAVAGE_BABE";
	m_jobNameTable[JT_SCORPION_KING] = "Pupa";
	m_jobNameTable[JT_SKEL_WORKER] = "SKEL_WORKER";
	m_jobNameTable[JT_SOHEE] = "SOHEE";
	m_jobNameTable[JT_SOLDIER_ANDRE] = "SOLDIER_ANDRE";
	m_jobNameTable[JT_SOLDIER_DENIRO] = "SOLDIER_DENIRO";
	m_jobNameTable[JT_SOLDIER_PIERE] = "SOLDIER_PIERE";
	m_jobNameTable[JT_STAINER] = "STAINER";
	m_jobNameTable[JT_TAROU] = "TAROU";
	m_jobNameTable[JT_VITATA] = "VITATA";
	m_jobNameTable[JT_ZENORC] = "ZENORC";
	m_jobNameTable[JT_ZEROM] = "ZEROM";
	m_jobNameTable[JT_WHISPER] = "WHISPER";
	m_jobNameTable[JT_NINE_TAIL] = "NINE_TAIL";
	m_jobNameTable[JT_ZOMBIE_DRAGON] = "dragon_5.gr2";
	m_jobNameTable[JT_THIEF_MUSHROOM] = "RED_MUSHROOM";
	m_jobNameTable[JT_CHONCHON_] = "CHOCHO";
	m_jobNameTable[JT_FABRE_] = "FABRE";
	m_jobNameTable[JT_WHISPER_] = "WHISPER";
	m_jobNameTable[JT_WHISPER_BOSS] = "WHISPER_BOSS";
	m_jobNameTable[JT_SWITCH] = "SWITCH";
	m_jobNameTable[JT_KARAKASA] = "KARAKASA";
	m_jobNameTable[JT_SHINOBI] = "SHINOBI";
	m_jobNameTable[JT_POISON_TOAD] = "POISON_TOAD";
	m_jobNameTable[JT_ANTIQUE_FIRELOCK] = "ANTIQUE_FIRELOCK";
	m_jobNameTable[JT_MIYABI_NINGYO] = "MIYABI_NINGYO";
	m_jobNameTable[JT_TENGU] = "TENGU";
	m_jobNameTable[JT_KAPHA] = "KAPHA";
	m_jobNameTable[JT_BON_GUN] = "BON_GUN";
	m_jobNameTable[JT_ORC_ARCHER] = "ORC_ARCHER";
	m_jobNameTable[JT_ORC_LORD] = "ORC_LORD";
	m_jobNameTable[JT_MIMIC] = "MIMIC";
	m_jobNameTable[JT_WRAITH] = "WRAITH";
	m_jobNameTable[JT_ALARM] = "ALARM";
	m_jobNameTable[JT_ARCLOUSE] = "ARCLOUSE";
	m_jobNameTable[JT_RIDEWORD] = "RIDEWORD";
	m_jobNameTable[JT_SKEL_PRISONER] = "SKEL_PRISONER";
	m_jobNameTable[JT_ZOMBIE_PRISONER] = "ZOMBIE_PRISONER";
	m_jobNameTable[JT_DARK_PRIEST] = "DARK_PRIEST";
	m_jobNameTable[JT_PUNK] = "PUNK";
	m_jobNameTable[JT_ZHERLTHSH] = "ZHERLTHSH";
	m_jobNameTable[JT_RYBIO] = "RYBIO";
	m_jobNameTable[JT_PHENDARK] = "PHENDARK";
	m_jobNameTable[JT_MYSTELTAINN] = "MYSTELTAINN";
	m_jobNameTable[JT_TIRFING] = "TIRFING";
	m_jobNameTable[JT_EXECUTIONER] = "EXECUTIONER";
	m_jobNameTable[JT_ANOLIAN] = "ANOLIAN";
	m_jobNameTable[JT_STING] = "STING";
	m_jobNameTable[JT_WANDER_MAN] = "WANDER_MAN";
	m_jobNameTable[JT_CRAMP] = "CRAMP";
	m_jobNameTable[JT_BRILIGHT] = "BRILIGHT";
	m_jobNameTable[JT_IRON_FIST] = "IRON_FIST";
	m_jobNameTable[JT_HIGH_ORC] = "HIGH_ORC";
	m_jobNameTable[JT_CHOCO] = "CHOCO";
	m_jobNameTable[JT_STEM_WORM] = "STEM_WORM";
	m_jobNameTable[JT_PENOMENA] = "PENOMENA";
	m_jobNameTable[JT_DESERT_WOLF_2] = "desert_wolf";
	m_jobNameTable[JT_SAVAGE_2] = "savage";
	m_jobNameTable[JT_HIGH_ORC_2] = "high_orc";
	m_jobNameTable[JT_ORC_WARRIOR_2] = "orc_warrior";
	m_jobNameTable[JT_POISON_SPORE_2] = "poison_spore";
	m_jobNameTable[JT_CHOCO_2] = "choco";
	m_jobNameTable[JT_KOBOLD_1_2] = "kobold_1";
	m_jobNameTable[JT_GOBLIN_1_2] = "goblin_1";
	m_jobNameTable[JT_PHEN_2] = "phen";
	m_jobNameTable[JT_FABRE_2] = "fabre";
	m_jobNameTable[JT_PUPA_2] = "pupa";
	m_jobNameTable[JT_CREAMY_2] = "creamy";
	m_jobNameTable[JT_PECO_EGG_2] = "peco_egg";
	m_jobNameTable[JT_PECOPECO_2] = "pecopeco";
	m_jobNameTable[JT_YOYO_2] = "yoyo";
	m_jobNameTable[JT_ORK_WARRIOR_2] = "ork_warrior";
	m_jobNameTable[JT_ANT_EGG_2] = "ant_egg";
	m_jobNameTable[JT_ANDRE_2] = "andre";
	m_jobNameTable[JT_PIERE_2] = "picky";
	m_jobNameTable[JT_DENIRO_2] = "deniro";
	m_jobNameTable[JT_PICKY_2] = "picky";
	m_jobNameTable[JT_PICKY__2] = "picky_";
	m_jobNameTable[JT_KNIGHT_OF_ABYSS] = "knight_of_abyss";
	m_jobNameTable[JT_MARIN] = "MARIN";
	m_jobNameTable[JT_SASQUATCH] = "SASQUATCH";
	m_jobNameTable[JT_JAKK_XMAS] = "JAKK_XMAS";
	m_jobNameTable[JT_GOBLINE_XMAS] = "GOBLINE_XMAS";
	m_jobNameTable[JT_COOKIE_XMAS] = "COOKIE_XMAS";
	m_jobNameTable[JT_ANTONIO] = "ANTONIO";
	m_jobNameTable[JT_CRUISER] = "CRUISER";
	m_jobNameTable[JT_MYSTCASE] = "MYSTCASE";
	m_jobNameTable[JT_CHEPET] = "CHEPET";
	m_jobNameTable[JT_KNIGHT_OF_WINDSTORM] = "KNIGHT_OF_WINDSTORM";
	m_jobNameTable[JT_GARM] = "GARM";
	m_jobNameTable[JT_GARGOYLE] = "Gargoyle";
	m_jobNameTable[JT_RAGGLER] = "Raggler";
	m_jobNameTable[JT_NERAID] = "Neraid";
	m_jobNameTable[JT_PEST] = "Pest";
	m_jobNameTable[JT_INJUSTICE] = "Injustice";
	m_jobNameTable[JT_GOBLIN_ARCHER] = "Goblin_Archer";
	m_jobNameTable[JT_GRYPHON] = "Gryphon";
	m_jobNameTable[JT_DARK_FRAME] = "Dark_Frame";
	m_jobNameTable[JT_WILD_ROSE] = "Wild_Rose";
	m_jobNameTable[JT_MUTANT_DRAGON] = "Mutant_Dragon";
	m_jobNameTable[JT_WIND_GHOST] = "Wind_Ghost";
	m_jobNameTable[JT_MERMAN] = "Merman";
	m_jobNameTable[JT_COOKIE] = "Cookie";
	m_jobNameTable[JT_ASTER] = "Aster";
	m_jobNameTable[JT_CARAT] = "Carat";
	m_jobNameTable[JT_BLOODY_KNIGHT] = "Bloody_Knight";
	m_jobNameTable[JT_CLOCK] = "Clock";
	m_jobNameTable[JT_C_TOWER_MANAGER] = "C_Tower_Manager";
	m_jobNameTable[JT_ALLIGATOR] = "Alligator";
	m_jobNameTable[JT_DARK_LORD] = "Dark_Lord";
	m_jobNameTable[JT_ORC_LADY] = "Orc_Lady";
	m_jobNameTable[JT_MEGALITH] = "Megalith";
	m_jobNameTable[JT_ALICE] = "Alice";
	m_jobNameTable[JT_RAYDRIC_ARCHER] = "Raydric_Archer";
	m_jobNameTable[JT_GREATEST_GENERAL] = "Greatest_General";
	m_jobNameTable[JT_STALACTIC_GOLEM] = "Stalactic_Golem";
	m_jobNameTable[JT_TRI_JOINT] = "Tri_Joint";
	m_jobNameTable[JT_STEAM_GOBLIN] = "Steam_Goblin";
	m_jobNameTable[JT_SAGEWORM] = "SageWorm";
	m_jobNameTable[JT_KOBOLD_ARCHER] = "Kobold_Archer";
	m_jobNameTable[JT_CHIMERA] = "Chimera";
	m_jobNameTable[JT_HUGELING] = "Hugeling90_6.gr2";
	m_jobNameTable[JT_ARCHER_GUARDIAN] = "Aguardian90_8.gr2";
	m_jobNameTable[JT_KNIGHT_GUARDIAN] = "Kguardian90_7.gr2";
	m_jobNameTable[JT_SOLDIER_GUARDIAN] = "Sguardian90_9.gr2";
	m_jobNameTable[JT_EMPELIUM] = "Empelium90_0.gr2";
	m_jobNameTable[JT_MAYA_PUPLE] = "maya_puple";
	m_jobNameTable[JT_SKELETON_GENERAL] = "SKELETON_GENERAL";
	m_jobNameTable[JT_WRAITH_DEAD] = "WRAITH_DEAD";
	m_jobNameTable[JT_MINI_DEMON] = "MINI_DEMON";
	m_jobNameTable[JT_CREMY_FEAR] = "CREMY_FEAR";
	m_jobNameTable[JT_KILLER_MANTIS] = "KILLER_MANTIS";
	m_jobNameTable[JT_OWL_BARON] = "OWL_BARON";
	m_jobNameTable[JT_KOBOLD_LEADER] = "KOBOLT_LEADER";
	m_jobNameTable[JT_ANCIENT_MUMMY] = "ANCIENT_MUMMY";
	m_jobNameTable[JT_ZOMBIE_MASTER] = "ZOMBIE_MASTER";
	m_jobNameTable[JT_GOBLIN_LEADER] = "GOBLIN_LEADER";
	m_jobNameTable[JT_CATERPILLAR] = "CATERPILLAR";
	m_jobNameTable[JT_AM_MUT] = "AM_MUT";
	m_jobNameTable[JT_DARK_ILLUSION] = "DARK_ILLUSION";
	m_jobNameTable[JT_GIANT_HONET] = "GIANT_HONET";
	m_jobNameTable[JT_GIANT_SPIDER] = "GIANT_SPIDER";
	m_jobNameTable[JT_ANCIENT_WORM] = "ANCIENT_WORM";
	m_jobNameTable[JT_LEIB_OLMAI] = "LEIB_OLMAI";
	m_jobNameTable[JT_CAT_O_NINE_TAIL] = "CAT_O_NINE_TAIL";
	m_jobNameTable[JT_PANZER_GOBLIN] = "PANZER_GOBLIN";
	m_jobNameTable[JT_GAJOMART] = "GAJOMART";
	m_jobNameTable[JT_MAJORUROS] = "MAJORUROS";
	m_jobNameTable[JT_GULLINBURSTI] = "GULLINBURSTI";
	m_jobNameTable[JT_TURTLE_GENERAL] = "TURTLE_GENERAL";
	m_jobNameTable[JT_MOBSTER] = "MOBSTER";
	m_jobNameTable[JT_PERMETER] = "PERMETER";
	m_jobNameTable[JT_ASSULTER] = "ASSULTER";
	m_jobNameTable[JT_SOLIDER] = "SOLIDER";
	m_jobNameTable[JT_FUR_SEAL] = "FUR_SEAL";
	m_jobNameTable[JT_HEATER] = "HEATER";
	m_jobNameTable[JT_FREEZER] = "FREEZER";
	m_jobNameTable[JT_OWL_DUKE] = "OWL_DUKE";
	m_jobNameTable[JT_DRAGON_TAIL] = "DRAGON_TAIL";
	m_jobNameTable[JT_SPRING_RABBIT] = "SPRING_RABBIT";
	m_jobNameTable[JT_SEE_OTTER] = "SEE_OTTER";
	m_jobNameTable[JT_TREASURE_BOX1] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX2] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX3] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX4] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX5] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX6] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX7] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX8] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX9] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX10] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX11] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX12] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX13] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX14] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX15] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX16] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX17] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX18] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX19] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX20] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX21] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX22] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX23] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX24] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX25] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX26] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX27] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX28] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX29] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX30] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX31] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX32] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX33] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX34] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX35] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX36] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX37] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX38] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX39] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX40] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_G_ASSULTER] = "G_ASSULTER";
	m_jobNameTable[JT_APOCALIPS] = "APOCALIPS";
	m_jobNameTable[JT_MEDUSA] = "MEDUSA";
	m_jobNameTable[JT_LAVA_GOLEM] = "LAVA_GOLEM";
	m_jobNameTable[JT_BLAZZER] = "BLAZZER";
	m_jobNameTable[JT_GEOGRAPHER] = "GEOGRAPHER";
	m_jobNameTable[JT_GRAND_PECO] = "GRAND_PECO";
	m_jobNameTable[JT_SUCCUBUS] = "SUCCUBUS";
	m_jobNameTable[JT_FAKE_ANGEL] = "FAKE_ANGEL";
	m_jobNameTable[JT_GOAT] = "GOAT";
	m_jobNameTable[JT_LORD_OF_DEATH] = "LORD_OF_DEATH";
	m_jobNameTable[JT_PASANA] = "PASANA";
	m_jobNameTable[JT_INCUBUS] = "INCUBUS";
	m_jobNameTable[JT_THE_PAPER] = "THE_PAPER";
	m_jobNameTable[JT_PHARAOH] = "PHARAOH";
	m_jobNameTable[JT_HARPY] = "HARPY";
	m_jobNameTable[JT_ELDER] = "ELDER";
	m_jobNameTable[JT_CRYSTAL_1] = "CRYSTAL_1";
	m_jobNameTable[JT_CRYSTAL_2] = "CRYSTAL_2";
	m_jobNameTable[JT_CRYSTAL_3] = "CRYSTAL_3";
	m_jobNameTable[JT_CRYSTAL_4] = "CRYSTAL_4";
	m_jobNameTable[JT_EVENT_BAPHO] = "EVENT_BAPHO";
	m_jobNameTable[JT_DOKEBI_] = "DOKEBI_";
	m_jobNameTable[JT_DEMON_PUNGUS] = "DEMON_PUNGUS";
	m_jobNameTable[JT_NIGHTMARE_TERROR] = "NIGHTMARE_TERROR";
	m_jobNameTable[JT_DRILLER] = "DRILLER";
	m_jobNameTable[JT_GRIZZLY] = "GRIZZLY";
	m_jobNameTable[JT_DIABOLIC] = "DIABOLIC";
	m_jobNameTable[JT_EXPLOSION] = "EXPLOSION";
	m_jobNameTable[JT_DELETER] = "DELETER";
	m_jobNameTable[JT_DELETER_] = "DELETER_";
	m_jobNameTable[JT_SLEEPER] = "SLEEPER";
	m_jobNameTable[JT_GIG] = "GIG";
	m_jobNameTable[JT_ARCHANGELING] = "ARCHANGELING";
	m_jobNameTable[JT_DRACULA] = "DRACULA";
	m_jobNameTable[JT_VIOLY] = "VIOLY";
	m_jobNameTable[JT_GALAPAGO] = "GALAPAGO";
	m_jobNameTable[JT_ROTAR_ZAIRO] = "ROTAR_ZAIRO";
	m_jobNameTable[JT_BLOOD_BUTTERFLY] = "BLOOD_BUTTERFLY";
	m_jobNameTable[JT_RICE_CAKE_BOY] = "RICE_CAKE_BOY";
	m_jobNameTable[JT_LIVE_PEACH_TREE] = "LIVE_PEACH_TREE";
	m_jobNameTable[JT_PEACH_TREE_BULLET] = "PEACH_TREE_BULLET";
	m_jobNameTable[JT_EVIL_CLOUD_HERMIT] = "EVIL_CLOUD_HERMIT";
	m_jobNameTable[JT_WILD_GINSENG] = "WILD_GINSENG";
	m_jobNameTable[JT_GINSENG_BULLET] = "GINSENG_BULLET";
	m_jobNameTable[JT_BABY_LEOPARD] = "BABY_LEOPARD";
	m_jobNameTable[JT_WICKED_NYMPH] = "WICKED_NYMPH";
	m_jobNameTable[JT_ZIPPER_BEAR] = "ZIPPER_BEAR";
	m_jobNameTable[JT_DARK_SNAKE_LORD] = "DARK_SNAKE_LORD";
	m_jobNameTable[JT_G_FARMILIAR] = "FARMILIAR";
	m_jobNameTable[JT_G_ARCHER_SKELETON] = "Skel_archer";
	m_jobNameTable[JT_G_ISIS] = "ISIS";
	m_jobNameTable[JT_G_HUNTER_FLY] = "HUNTER_FLY";
	m_jobNameTable[JT_G_GHOUL] = "GHOUL";
	m_jobNameTable[JT_G_SIDE_WINDER] = "SIDE_WINDER";
	m_jobNameTable[JT_G_OBEAUNE] = "OBEAUNE";
	m_jobNameTable[JT_G_MARC] = "MARC";
	m_jobNameTable[JT_G_NIGHTMARE] = "NIGHTMARE";
	m_jobNameTable[JT_G_POISON_SPORE] = "POISON_SPORE";
	m_jobNameTable[JT_G_ARGIOPE] = "ARGIOPE";
	m_jobNameTable[JT_G_ARGOS] = "ARGOS";
	m_jobNameTable[JT_G_BAPHOMET_] = "BAPHOMET_";
	m_jobNameTable[JT_G_DESERT_WOLF] = "DESERT_WOLF";
	m_jobNameTable[JT_G_DEVIRUCHI] = "DEVIRUCHI";
	m_jobNameTable[JT_G_DRAINLIAR] = "DRAINLIAR";
	m_jobNameTable[JT_G_EVIL_DRUID] = "EVIL_DRUID";
	m_jobNameTable[JT_G_JAKK] = "JAKK";
	m_jobNameTable[JT_G_JOKER] = "JOKER";
	m_jobNameTable[JT_G_KHALITZBURG] = "KHALITZBURG";
	m_jobNameTable[JT_G_HIGH_ORC] = "HIGH_ORC";
	m_jobNameTable[JT_G_STEM_WORM] = "STEM_WORM";
	m_jobNameTable[JT_G_PENOMENA] = "PENOMENA";
	m_jobNameTable[JT_G_SASQUATCH] = "SASQUATCH";
	m_jobNameTable[JT_G_CRUISER] = "CRUISER";
	m_jobNameTable[JT_G_CHEPET] = "CHEPET";
	m_jobNameTable[JT_G_RAGGLER] = "RAGGLER";
	m_jobNameTable[JT_G_INJUSTICE] = "INJUSTICE";
	m_jobNameTable[JT_G_GRYPHON] = "GRYPHON";
	m_jobNameTable[JT_G_DARK_FRAME] = "DARK_FRAME";
	m_jobNameTable[JT_G_MUTANT_DRAGON] = "MUTANT_DRAGON";
	m_jobNameTable[JT_G_WIND_GHOST] = "WIND_GHOST";
	m_jobNameTable[JT_G_MERMAN] = "MERMAN";
	m_jobNameTable[JT_G_ORC_LADY] = "ORC_LADY";
	m_jobNameTable[JT_G_RAYDRIC_ARCHER] = "RAYDRIC_ARCHER";
	m_jobNameTable[JT_G_TRI_JOINT] = "TRI_JOINT";
	m_jobNameTable[JT_G_KOBOLD_ARCHER] = "KOBOLD_ARCHER";
	m_jobNameTable[JT_G_CHIMERA] = "CHIMERA";
	m_jobNameTable[JT_G_MANTIS] = "MANTIS";
	m_jobNameTable[JT_G_MARDUK] = "MARDUK";
	m_jobNameTable[JT_G_MARIONETTE] = "MARIONETTE";
	m_jobNameTable[JT_G_MATYR] = "MATYR";
	m_jobNameTable[JT_G_MINOROUS] = "MINOROUS";
	m_jobNameTable[JT_G_ORC_SKELETON] = "ORC_SKELETON";
	m_jobNameTable[JT_G_ORC_ZOMBIE] = "ORC_ZOMBIE";
	m_jobNameTable[JT_G_PASANA] = "PASANA";
	m_jobNameTable[JT_G_PETIT] = "PETIT";
	m_jobNameTable[JT_G_PETIT_] = "PETIT_";
	m_jobNameTable[JT_G_RAYDRIC] = "RAYDRIC";
	m_jobNameTable[JT_G_REQUIEM] = "REQUIEM";
	m_jobNameTable[JT_G_SKEL_WORKER] = "SKEL_WORKER";
	m_jobNameTable[JT_G_ZEROM] = "ZEROM";
	m_jobNameTable[JT_G_NINE_TAIL] = "NINE_TAIL";
	m_jobNameTable[JT_G_BON_GUN] = "BON_GUN";
	m_jobNameTable[JT_G_ORC_ARCHER] = "ORC_ARCHER";
	m_jobNameTable[JT_G_MIMIC] = "MIMIC";
	m_jobNameTable[JT_G_WRAITH] = "WRAITH";
	m_jobNameTable[JT_G_ALARM] = "ALARM";
	m_jobNameTable[JT_G_ARCLOUSE] = "ARCLOUSE";
	m_jobNameTable[JT_G_RIDEWORD] = "RIDEWORD";
	m_jobNameTable[JT_G_SKEL_PRISONER] = "SKEL_PRISONER";
	m_jobNameTable[JT_G_ZOMBIE_PRISONER] = "ZOMBIE_PRISONER";
	m_jobNameTable[JT_G_PUNK] = "PUNK";
	m_jobNameTable[JT_G_ZHERLTHSH] = "ZHERLTHSH";
	m_jobNameTable[JT_G_RYBIO] = "RYBIO";
	m_jobNameTable[JT_G_PHENDARK] = "PHENDARK";
	m_jobNameTable[JT_G_MYSTELTAINN] = "MYSTELTAINN";
	m_jobNameTable[JT_G_TIRFING] = "TIRFING";
	m_jobNameTable[JT_G_EXECUTIONER] = "EXECUTIONER";
	m_jobNameTable[JT_G_ANOLIAN] = "ANOLIAN";
	m_jobNameTable[JT_G_STING] = "STING";
	m_jobNameTable[JT_G_WANDER_MAN] = "WANDER_MAN";
	m_jobNameTable[JT_G_DOKEBI] = "DOKEBI";
	m_jobNameTable[JT_G_MUMMY] = "G_MUMMY";
	m_jobNameTable[JT_G_ZOMBIE] = "G_ZOMBIE";
	m_jobNameTable[JT_INCANTATION_SAMURAI] = "INCANTATION_SAMURAI";
	m_jobNameTable[JT_DRYAD] = "DRYAD";
	m_jobNameTable[JT_KIND_OF_BEETLE] = "KIND_OF_BEETLE";
	m_jobNameTable[JT_STONE_SHOOTER] = "STONE_SHOOTER";
	m_jobNameTable[JT_STONE_SHOOTER_BULLET] = "STONE_SHOOTER_BULLET";
	m_jobNameTable[JT_WOODEN_GOLEM] = "WOODEN_GOLEM";
	m_jobNameTable[JT_WOOTAN_SHOOTER] = "WOOTAN_SHOOTER";
	m_jobNameTable[JT_WOOTAN_FIGHTER] = "WOOTAN_FIGHTER";
	m_jobNameTable[JT_PARASITE] = "PARASITE";
	m_jobNameTable[JT_PARASITE_BULLET] = "PARASITE_BULLET";
	m_jobNameTable[JT_PORING_V] = "Poring";
	m_jobNameTable[JT_GIBBET] = "GIBBET";
	m_jobNameTable[JT_DULLAHAN] = "DULLAHAN";
	m_jobNameTable[JT_LOLI_RURI] = "LOLI_RURI";
	m_jobNameTable[JT_DISGUISE] = "DISGUISE";
	m_jobNameTable[JT_BLOODY_MURDERER] = "BLOODY_MURDERER";
	m_jobNameTable[JT_QUVE] = "QUVE";
	m_jobNameTable[JT_LUDE] = "LUDE";
	m_jobNameTable[JT_HYLOZOIST] = "HYLOZOIST";
	m_jobNameTable[JT_AMON_RA] = "AMON_RA";
	m_jobNameTable[JT_HYEGUN] = "HYEGUN";
	m_jobNameTable[JT_CIVIL_SERVANT] = "CIVIL_SERVANT";
	m_jobNameTable[JT_DANCING_DRAGON] = "DANCING_DRAGON";
	m_jobNameTable[JT_GARM_BABY] = "GARM_BABY";
	m_jobNameTable[JT_INCREASE_SOIL] = "INCREASE_SOIL";
	m_jobNameTable[JT_LI_ME_MANG_RYANG] = "LI_ME_MANG_RYANG";
	m_jobNameTable[JT_BACSOJIN] = "BACSOJIN";
	m_jobNameTable[JT_CHUNG_E] = "CHUNG_E";
	m_jobNameTable[JT_BOILED_RICE] = "BOILED_RICE";
	m_jobNameTable[JT_G_ALICE] = "Alice";
	m_jobNameTable[JT_G_ANCIENT_MUMMY] = "ANCIENT_MUMMY";
	m_jobNameTable[JT_G_ANTIQUE_FIRELOCK] = "ANTIQUE_FIRELOCK";
	m_jobNameTable[JT_G_BABY_LEOPARD] = "BABY_LEOPARD";
	m_jobNameTable[JT_G_BATHORY] = "BATHORY";
	m_jobNameTable[JT_G_BLOOD_BUTTERFLY] = "BLOOD_BUTTERFLY";
	m_jobNameTable[JT_G_C_TOWER_MANAGER] = "C_TOWER_MANAGER";
	m_jobNameTable[JT_G_CLOCK] = "CLOCK";
	m_jobNameTable[JT_G_DARK_SNAKE_LORD] = "DARK_SNAKE_LORD";
	m_jobNameTable[JT_G_DRACULA] = "DRACULA";
	m_jobNameTable[JT_G_EVIL_CLOUD_HERMIT] = "EVIL_CLOUD_HERMIT";
	m_jobNameTable[JT_G_EXPLOSION] = "EXPLOSION";
	m_jobNameTable[JT_G_FUR_SEAL] = "FUR_SEAL";
	m_jobNameTable[JT_G_GOBLIN_1] = "GOBLIN_1";
	m_jobNameTable[JT_G_GOBLIN_2] = "GOBLIN_2";
	m_jobNameTable[JT_G_GOBLIN_3] = "GOBLIN_3";
	m_jobNameTable[JT_G_GOBLIN_4] = "GOBLIN_4";
	m_jobNameTable[JT_G_GOBLIN_5] = "GOBLIN_5";
	m_jobNameTable[JT_G_GOBLIN_LEADER] = "GOBLIN_LEADER";
	m_jobNameTable[JT_G_GOLEM] = "GOLEM";
	m_jobNameTable[JT_G_GREATEST_GENERAL] = "GREATEST_GENERAL";
	m_jobNameTable[JT_G_INCANTATION_SAMURAI] = "INCANTATION_SAMURAI";
	m_jobNameTable[JT_G_KAPHA] = "KAPHA";
	m_jobNameTable[JT_G_KARAKASA] = "KARAKASA";
	m_jobNameTable[JT_G_KOBOLD_1] = "KOBOLD_1";
	m_jobNameTable[JT_G_KOBOLD_2] = "KOBOLD_2";
	m_jobNameTable[JT_G_KOBOLD_3] = "KOBOLD_3";
	m_jobNameTable[JT_G_KOBOLD_LEADER] = "KOBOLD_LEADER";
	m_jobNameTable[JT_G_LAVA_GOLEM] = "LAVA_GOLEM";
	m_jobNameTable[JT_G_LIVE_PEACH_TREE] = "LIVE_PEACH_TREE";
	m_jobNameTable[JT_G_MARSE] = "MARSE";
	m_jobNameTable[JT_G_MIYABI_NINGYO] = "MIYABI_NINGYO";
	m_jobNameTable[JT_G_MYST] = "MYST";
	m_jobNameTable[JT_G_NIGHTMARE_TERROR] = "NIGHTMARE_TERROR";
	m_jobNameTable[JT_G_PARASITE] = "PARASITE";
	m_jobNameTable[JT_G_POISON_TOAD] = "POISON_TOAD";
	m_jobNameTable[JT_G_ROTAR_ZAIRO] = "ROTAR_ZAIRO";
	m_jobNameTable[JT_G_SAND_MAN] = "SAND_MAN";
	m_jobNameTable[JT_G_SCORPION] = "SCORPION";
	m_jobNameTable[JT_G_SHINOBI] = "SHINOBI";
	m_jobNameTable[JT_G_SMOKIE] = "SMOKIE";
	m_jobNameTable[JT_G_SOLDIER_SKELETON] = "Skel_soldier";
	m_jobNameTable[JT_G_TENGU] = "TENGU";
	m_jobNameTable[JT_G_WICKED_NYMPH] = "WICKED_NYMPH";
	m_jobNameTable[JT_G_WILD_GINSENG] = "WILD_GINSENG";
	m_jobNameTable[JT_G_WRAITH_DEAD] = "WRAITH_DEAD";
	m_jobNameTable[JT_G_ANCIENT_WORM] = "ANCIENT_WORM";
	m_jobNameTable[JT_G_ANGELING] = "ANGELING";
	m_jobNameTable[JT_G_BLOODY_KNIGHT] = "BLOODY_KNIGHT";
	m_jobNameTable[JT_G_CRAMP] = "CRAMP";
	m_jobNameTable[JT_G_DEVIACE] = "DEVIACE";
	m_jobNameTable[JT_G_DROPS] = "DROPS";
	m_jobNameTable[JT_G_ELDER] = "ELDER";
	m_jobNameTable[JT_G_ELDER_WILOW] = "ELDER_WILOW";
	m_jobNameTable[JT_G_FLORA] = "FLORA";
	m_jobNameTable[JT_G_GHOSTRING] = "GHOSTRING";
	m_jobNameTable[JT_G_GOBLIN_ARCHER] = "GOBLIN_ARCHER";
	m_jobNameTable[JT_G_HORONG] = "HORONG";
	m_jobNameTable[JT_G_HYDRA] = "HYDRA";
	m_jobNameTable[JT_G_INCUBUS] = "INCUBUS";
	m_jobNameTable[JT_G_VOCAL] = "VOCAL";
	m_jobNameTable[JT_DEVILING] = "DEVILING";
	m_jobNameTable[JT_TAO_GUNKA] = "TAO_GUNKA";
	m_jobNameTable[JT_TAMRUAN] = "TAMRUAN";
	m_jobNameTable[JT_MIME_MONKEY] = "MIME_MONKEY";
	m_jobNameTable[JT_LEAF_CAT] = "LEAF_CAT";
	m_jobNameTable[JT_KRABEN] = "KRABEN";
	m_jobNameTable[JT_ORC_XMAS] = "ORC_XMAS";
	m_jobNameTable[JT_G_MANDRAGORA] = "Mandragora";
	m_jobNameTable[JT_G_GEOGRAPHER] = "GEOGRAPHER";
	m_jobNameTable[JT_A_LUNATIC] = "Lunatic";
	m_jobNameTable[JT_A_MOBSTER] = "MOBSTER";
	m_jobNameTable[JT_A_ANCIENT_MUMMY] = "ANCIENT_MUMMY";
	m_jobNameTable[JT_G_FREEZER] = "FREEZER";
	m_jobNameTable[JT_G_MARIN] = "MARIN";
	m_jobNameTable[JT_G_TAMRUAN] = "TAMRUAN";
	m_jobNameTable[JT_G_GARGOYLE] = "Gargoyle";
	m_jobNameTable[JT_G_BLAZZER] = "BLAZZER";
	m_jobNameTable[JT_G_WHISPER_BOSS] = "WHISPER_BOSS";
	m_jobNameTable[JT_G_HEATER] = "HEATER";
	m_jobNameTable[JT_G_PERMETER] = "PERMETER";
	m_jobNameTable[JT_G_SOLIDER] = "SOLIDER";
	m_jobNameTable[JT_G_BIGFOOT] = "BIGFOOT";
	m_jobNameTable[JT_G_GIANT_HONET] = "GIANT_HONET";
	m_jobNameTable[JT_G_DARK_ILLUSION] = "DARK_ILLUSION";
	m_jobNameTable[JT_G_GARM_BABY] = "GARM_BABY";
	m_jobNameTable[JT_G_GOBLINE_XMAS] = "GOBLINE_XMAS";
	m_jobNameTable[JT_G_THIEF_BUG__] = "Thief_bug_male";
	m_jobNameTable[JT_G_DANCING_DRAGON] = "DANCING_DRAGON";
	m_jobNameTable[JT_A_MUNAK] = "Munak";
	m_jobNameTable[JT_A_BON_GUN] = "BON_GUN";
	m_jobNameTable[JT_A_HYEGUN] = "HYEGUN";
	m_jobNameTable[JT_METALING] = "METALING";
	m_jobNameTable[JT_MINERAL] = "MINERAL";
	m_jobNameTable[JT_OBSIDIAN] = "OBSIDIAN";
	m_jobNameTable[JT_PITMAN] = "PITMAN";
	m_jobNameTable[JT_WASTE_STOVE] = "WASTE_STOVE";
	m_jobNameTable[JT_UNGOLIANT] = "UNGOLIANT";
	m_jobNameTable[JT_PORCELLIO] = "PORCELLIO";
	m_jobNameTable[JT_NOXIOUS] = "NOXIOUS";
	m_jobNameTable[JT_VENOMOUS] = "VENOMOUS";
	m_jobNameTable[JT_TEDDY_BEAR] = "TEDDY_BEAR";
	m_jobNameTable[JT_RSX_0806] = "RSX_0806";
	m_jobNameTable[JT_G_WASTE_STOVE] = "WASTE_STOVE";
	m_jobNameTable[JT_G_PORCELLIO] = "PORCELLIO";
	m_jobNameTable[JT_G_DARK_PRIEST] = "DARK_PRIEST";
	m_jobNameTable[JT_ANOPHELES] = "ANOPHELES";
	m_jobNameTable[JT_MOLE] = "MOLE";
	m_jobNameTable[JT_HILL_WIND] = "HILL_WIND";
	m_jobNameTable[JT_BACSOJIN_] = "BACSOJIN";
	m_jobNameTable[JT_CHUNG_E_] = "CHUNG_E";
	m_jobNameTable[JT_GREMLIN] = "GREMLIN";
	m_jobNameTable[JT_BEHOLDER] = "BEHOLDER";
	m_jobNameTable[JT_SEYREN] = "SEYREN";
	m_jobNameTable[JT_EREMES] = "EREMES";
	m_jobNameTable[JT_HARWORD] = "HARWORD";
	m_jobNameTable[JT_MAGALETA] = "MAGALETA";
	m_jobNameTable[JT_SHECIL] = "SHECIL";
	m_jobNameTable[JT_KATRINN] = "KATRINN";
	m_jobNameTable[JT_G_SEYREN] = "SEYREN";
	m_jobNameTable[JT_G_EREMES] = "EREMES";
	m_jobNameTable[JT_G_HARWORD] = "HARWORD";
	m_jobNameTable[JT_G_MAGALETA] = "MAGALETA";
	m_jobNameTable[JT_G_SHECIL] = "SHECIL";
	m_jobNameTable[JT_G_KATRINN] = "KATRINN";
	m_jobNameTable[JT_B_SEYREN] = "SEYREN";
	m_jobNameTable[JT_B_EREMES] = "EREMES";
	m_jobNameTable[JT_B_HARWORD] = "HARWORD";
	m_jobNameTable[JT_B_MAGALETA] = "MAGALETA";
	m_jobNameTable[JT_B_SHECIL] = "SHECIL";
	m_jobNameTable[JT_B_KATRINN] = "KATRINN";
	m_jobNameTable[JT_YGNIZEM] = "YGNIZEM";
	m_jobNameTable[JT_WHIKEBAIN] = "WHIKEBAIN";
	m_jobNameTable[JT_ARMAIA] = "ARMAIA";
	m_jobNameTable[JT_EREND] = "EREND";
	m_jobNameTable[JT_KAVAC] = "KAVAC";
	m_jobNameTable[JT_RAWREL] = "RAWREL";
	m_jobNameTable[JT_B_YGNIZEM] = "YGNIZEM";
	m_jobNameTable[JT_G_WHIKEBAIN] = "WHIKEBAIN";
	m_jobNameTable[JT_G_ARMAIA] = "ARMAIA";
	m_jobNameTable[JT_G_EREND] = "EREND";
	m_jobNameTable[JT_G_KAVAC] = "KAVAC";
	m_jobNameTable[JT_G_RAWREL] = "RAWREL";
	m_jobNameTable[JT_POTON_CANON] = "POTON_CANON";
	m_jobNameTable[JT_POTON_CANON_1] = "POTON_CANON_1";
	m_jobNameTable[JT_POTON_CANON_2] = "POTON_CANON_2";
	m_jobNameTable[JT_POTON_CANON_3] = "POTON_CANON_3";
	m_jobNameTable[JT_ARCHDAM] = "ARCHDAM";
	m_jobNameTable[JT_DIMIK] = "DIMIK";
	m_jobNameTable[JT_DIMIK_1] = "DIMIK_1";
	m_jobNameTable[JT_DIMIK_2] = "DIMIK_2";
	m_jobNameTable[JT_DIMIK_3] = "DIMIK_3";
	m_jobNameTable[JT_DIMIK_4] = "DIMIK_4";
	m_jobNameTable[JT_MONEMUS] = "MONEMUS";
	m_jobNameTable[JT_VENATU] = "VENATU";
	m_jobNameTable[JT_VENATU_1] = "VENATU_1";
	m_jobNameTable[JT_VENATU_2] = "VENATU_2";
	m_jobNameTable[JT_VENATU_3] = "VENATU_3";
	m_jobNameTable[JT_VENATU_4] = "VENATU_4";
	m_jobNameTable[JT_HILL_WIND_1] = "HILL_WIND_1";
	m_jobNameTable[JT_GEMINI] = "GEMINI";
	m_jobNameTable[JT_REMOVAL] = "REMOVAL";
	m_jobNameTable[JT_G_POTON_CANON] = "POTON_CANON";
	m_jobNameTable[JT_G_ARCHDAM] = "ARCHDAM";
	m_jobNameTable[JT_APOCALIPS_H] = "APOCALIPS_H";
	m_jobNameTable[JT_ORC_BABY] = "ORC_BABY";
	m_jobNameTable[JT_GREEN_IGUANA] = "GREEN_IGUANA";
	m_jobNameTable[JT_LADY_TANEE] = "LADY_TANEE";
	m_jobNameTable[JT_G_BACSOJIN] = "G_BACSOJIN";
	m_jobNameTable[JT_G_SPRING_RABBIT] = "G_SPRING_RABBIT";
	m_jobNameTable[JT_G_KRABEN] = "KRABEN";
	m_jobNameTable[JT_BREEZE] = "BREEZE";
	m_jobNameTable[JT_PLASMA_Y] = "PLASMA_Y";
	m_jobNameTable[JT_PLASMA_R] = "PLASMA_R";
	m_jobNameTable[JT_PLASMA_G] = "PLASMA_G";
	m_jobNameTable[JT_PLASMA_P] = "PLASMA_P";
	m_jobNameTable[JT_PLASMA_B] = "PLASMA_B";
	m_jobNameTable[JT_DEATHWORD] = "DEATHWORD";
	m_jobNameTable[JT_ANCIENT_MIMIC] = "ANCIENT_MIMIC";
	m_jobNameTable[JT_OBSERVATION] = "OBSERVATION";
	m_jobNameTable[JT_SHELTER] = "SHELTER";
	m_jobNameTable[JT_RETRIBUTION] = "RETRIBUTION";
	m_jobNameTable[JT_SOLACE] = "SOLACE";
	m_jobNameTable[JT_THA_ODIUM] = "THA_ODIUM";
	m_jobNameTable[JT_THA_DESPERO] = "THA_DESPERO";
	m_jobNameTable[JT_THA_MAERO] = "THA_MAERO";
	m_jobNameTable[JT_THA_DOLOR] = "THA_DOLOR";
	m_jobNameTable[JT_THANATOS] = "THANATOS";
	m_jobNameTable[JT_G_THA_ODIUM] = "THA_ODIUM";
	m_jobNameTable[JT_G_THA_DESPERO] = "THA_DESPERO";
	m_jobNameTable[JT_G_THA_MAERO] = "THA_MAERO";
	m_jobNameTable[JT_G_THA_DOLOR] = "THA_DOLOR";
	m_jobNameTable[JT_ACIDUS] = "ACIDUS";
	m_jobNameTable[JT_FERUS] = "FERUS";
	m_jobNameTable[JT_NOVUS] = "NOVUS";
	m_jobNameTable[JT_ACIDUS_] = "ACIDUS_";
	m_jobNameTable[JT_FERUS_] = "FERUS_";
	m_jobNameTable[JT_NOVUS_] = "NOVUS_";
	m_jobNameTable[JT_DETALE] = "DETALE";
	m_jobNameTable[JT_HYDRO] = "HYDRO";
	m_jobNameTable[JT_DRAGON_EGG] = "DRAGON_EGG";
	m_jobNameTable[JT_EVENT_JAKK] = "JAKK";
	m_jobNameTable[JT_A_SHECIL] = "SHECIL";
	m_jobNameTable[JT_A_POTON_CANON] = "POTON_CANON";
	m_jobNameTable[JT_R_PORING] = "Poring";
	m_jobNameTable[JT_R_LUNATIC] = "Lunatic";
	m_jobNameTable[JT_R_SAVAGE_BABE] = "SAVAGE_BABE";
	m_jobNameTable[JT_R_DESERT_WOLF_B] = "DESERT_WOLF_B";
	m_jobNameTable[JT_R_BAPHOMET_] = "BAPHOMET_";
	m_jobNameTable[JT_R_DEVIRUCHI] = "DEVIRUCHI";
	m_jobNameTable[JT_G_DOPPELGANGER] = "DOPPELGANGER";
	m_jobNameTable[JT_G_TREASURE_BOX] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_KIEL] = "KIEL";
	m_jobNameTable[JT_KIEL_] = "KIEL";
	m_jobNameTable[JT_ALICEL] = "ALICEL";
	m_jobNameTable[JT_ALIOT] = "ALIOT";
	m_jobNameTable[JT_ALIZA] = "ALIZA";
	m_jobNameTable[JT_CONSTANT] = "CONSTANT";
	m_jobNameTable[JT_G_ALICEL] = "ALICEL";
	m_jobNameTable[JT_G_ALIOT] = "ALIOT";
	m_jobNameTable[JT_G_COOKIE_XMAS] = "COOKIE_XMAS";
	m_jobNameTable[JT_G_CARAT] = "Carat";
	m_jobNameTable[JT_G_MYSTCASE] = "MYSTCASE";
	m_jobNameTable[JT_G_WILD_ROSE] = "Wild_Rose";
	m_jobNameTable[JT_G_CONSTANT] = "CONSTANT";
	m_jobNameTable[JT_G_ALIZA] = "ALIZA";
	m_jobNameTable[JT_G_SNAKE] = "Snake";
	m_jobNameTable[JT_G_ANACONDAQ] = "Anacondaq";
	m_jobNameTable[JT_G_MEDUSA] = "Medusa";
	m_jobNameTable[JT_G_RED_PLANT] = "RED_PLANT";
	m_jobNameTable[JT_RANDGRIS] = "RANDGRIS";
	m_jobNameTable[JT_SKOGUL] = "SKOGUL";
	m_jobNameTable[JT_FRUS] = "FRUS";
	m_jobNameTable[JT_SKEGGIOLD] = "SKEGGIOLD";
	m_jobNameTable[JT_SKEGGIOLD_] = "SKEGGIOLD_";
	m_jobNameTable[JT_G_HYDRO] = "HYDRO";
	m_jobNameTable[JT_G_ACIDUS] = "ACIDUS";
	m_jobNameTable[JT_G_FERUS] = "FERUS";
	m_jobNameTable[JT_G_ACIDUS_] = "ACIDUS_";
	m_jobNameTable[JT_G_FERUS_] = "FERUS_";
	m_jobNameTable[JT_G_SKOGUL] = "SKOGUL";
	m_jobNameTable[JT_G_FRUS] = "FRUS";
	m_jobNameTable[JT_G_SKEGGIOLD] = "SKEGGIOLD";
	m_jobNameTable[JT_G_SKEGGIOLD_] = "SKEGGIOLD_";
	m_jobNameTable[JT_G_RANDGRIS] = "G_RANDGRIS";
	m_jobNameTable[JT_EM_ANGELING] = "EM_ANGELING";
	m_jobNameTable[JT_EM_DEVILING] = "EM_DEVILING";
	m_jobNameTable[JT_GLOOMUNDERNIGHT] = "GLOOMUNDERNIGHT";
	m_jobNameTable[JT_AGAV] = "AGAV";
	m_jobNameTable[JT_ECHIO] = "ECHIO";
	m_jobNameTable[JT_VANBERK] = "VANBERK";
	m_jobNameTable[JT_ISILLA] = "ISILLA";
	m_jobNameTable[JT_HODREMLIN] = "HODREMLIN";
	m_jobNameTable[JT_SEEKER] = "SEEKER";
	m_jobNameTable[JT_SNOWIER] = "SNOWIER";
	m_jobNameTable[JT_SIROMA] = "SIROMA";
	m_jobNameTable[JT_ICE_TITAN] = "ICE_TITAN";
	m_jobNameTable[JT_GAZETI] = "GAZETI";
	m_jobNameTable[JT_KTULLANUX] = "KTULLANUX";
	m_jobNameTable[JT_MUSCIPULAR] = "MUSCIPULAR";
	m_jobNameTable[JT_DROSERA] = "DROSERA";
	m_jobNameTable[JT_ROWEEN] = "ROWEEN";
	m_jobNameTable[JT_GALION] = "GALION";
	m_jobNameTable[JT_STAPO] = "STAPO";
	m_jobNameTable[JT_ATROCE] = "ATROS";
	m_jobNameTable[JT_G_AGAV] = "AGAV";
	m_jobNameTable[JT_G_ECHIO] = "ECHIO";
	m_jobNameTable[JT_G_ICE_TITAN] = "ICE_TITAN";
	m_jobNameTable[JT_ICEICLE] = "ICEICLE";
	m_jobNameTable[JT_G_RAFFLESIA] = "RAFFLESIA";
	m_jobNameTable[JT_G_GALION] = "GALION";
	m_jobNameTable[JT_SOCCER_BALL] = "SOCCER_BALL";
	m_jobNameTable[JT_G_MEGALITH] = "MEGALITH";
	m_jobNameTable[JT_G_ROWEEN] = "ROWEEN";
	m_jobNameTable[JT_BLOODY_KNIGHT_] = "BLOODY_KNIGHT";
	m_jobNameTable[JT_AUNOE] = "AUNOE";
	m_jobNameTable[JT_FANAT] = "FANAT";
	m_jobNameTable[JT_TREASURE_BOX_] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_B_SEYREN_] = "SEYREN";
	m_jobNameTable[JT_B_EREMES_] = "EREMES";
	m_jobNameTable[JT_B_HARWORD_] = "HARWORD";
	m_jobNameTable[JT_B_MAGALETA_] = "MAGALETA";
	m_jobNameTable[JT_B_SHECIL_] = "SHECIL";
	m_jobNameTable[JT_B_KATRINN_] = "KATRINN";
	m_jobNameTable[JT_G_SEYREN_] = "SEYREN";
	m_jobNameTable[JT_G_EREMES_] = "EREMES";
	m_jobNameTable[JT_G_HARWORD_] = "HARWORD";
	m_jobNameTable[JT_G_MAGALETA_] = "MAGALETA";
	m_jobNameTable[JT_G_SHECIL_] = "SHECIL";
	m_jobNameTable[JT_G_KATRINN_] = "KATRINN";
	m_jobNameTable[JT_G_SMOKIE_] = "SMOKIE";
	m_jobNameTable[JT_EVENT_LUDE] = "LUDE";
	m_jobNameTable[JT_EVENT_HYDRO] = "HYDRO";
	m_jobNameTable[JT_EVENT_MOON] = "MOONLIGHT";
	m_jobNameTable[JT_EVENT_RICECAKE] = "EVENT_RICECAKE";
	m_jobNameTable[JT_EVENT_GOURD] = "EVENT_GOURD";
	m_jobNameTable[JT_EVENT_DETALE] = "DETALE";
	m_jobNameTable[JT_EVENT_ALARM] = "ALARM";
	m_jobNameTable[JT_EVENT_BATHORY] = "BATHORY";
	m_jobNameTable[JT_EVENT_BIGFOOT] = "BIGFOOT";
	m_jobNameTable[JT_EVENT_DESERT_WOLF] = "DESERT_WOLF";
	m_jobNameTable[JT_EVENT_DEVIRUCHI] = "DEVIRUCHI";
	m_jobNameTable[JT_EVENT_FREEZER] = "FREEZER";
	m_jobNameTable[JT_EVENT_GARM_BABY] = "GARM_BABY";
	m_jobNameTable[JT_EVENT_GOBLINE_XMAS] = "GOBLINE_XMAS";
	m_jobNameTable[JT_EVENT_MYST] = "MYST";
	m_jobNameTable[JT_EVENT_SASQUATCH] = "SASQUATCH";
	m_jobNameTable[JT_EVENT_GULLINBURSTI] = "GULLINBURSTI";
	m_jobNameTable[JT_SWORD_GUARDIAN] = "Kguardian90_7.gr2";
	m_jobNameTable[JT_BOW_GUARDIAN] = "Aguardian90_8.gr2";
	m_jobNameTable[JT_SALAMANDER] = "SALAMANDER";
	m_jobNameTable[JT_IFRIT] = "IFRIT";
	m_jobNameTable[JT_KASA] = "KASA";
	m_jobNameTable[JT_G_SALAMANDER] = "SALAMANDER";
	m_jobNameTable[JT_G_KASA] = "KASA";
	m_jobNameTable[JT_MAGMARING] = "MAGMARING";
	m_jobNameTable[JT_IMP] = "IMP";
	m_jobNameTable[JT_KNOCKER] = "KNOCKER";
	m_jobNameTable[JT_BYORGUE] = "BYORGUE";
	m_jobNameTable[JT_GOLDEN_SAVAGE] = "GOLDEN_SAVAGE";
	m_jobNameTable[JT_G_SNAKE_] = "Snake";
	m_jobNameTable[JT_G_ANACONDAQ_] = "Anacondaq";
	m_jobNameTable[JT_G_SIDE_WINDER_] = "SIDE_WINDER";
	m_jobNameTable[JT_G_ISIS_] = "ISIS";
	m_jobNameTable[JT_G_TREASURE_BOX_] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_DREAMMETAL] = "Empelium90_0.gr2";
	m_jobNameTable[JT_EVENT_PORING] = "Poring";
	m_jobNameTable[JT_EVENT_BAPHOMET] = "Baphomet";
	m_jobNameTable[JT_EVENT_OSIRIS] = "Osiris";
	m_jobNameTable[JT_EVENT_ORCHERO] = "ORK_HERO";
	m_jobNameTable[JT_EVENT_MOBSTER] = "MOBSTER";
	m_jobNameTable[JT_G_EM_ANGELING] = "EM_ANGELING";
	m_jobNameTable[JT_G_EM_DEVILING] = "EM_DEVILING";
	m_jobNameTable[JT_E_MUKA] = "Muka";
	m_jobNameTable[JT_E_POISONSPORE] = "POISON_SPORE";
	m_jobNameTable[JT_E_MAGNOLIA] = "MAGNOLIA";
	m_jobNameTable[JT_E_MARIN] = "MARIN";
	m_jobNameTable[JT_E_PLANKTON] = "PLANKTON";
	m_jobNameTable[JT_E_MANDRAGORA] = "Mandragora";
	m_jobNameTable[JT_E_COCO] = "COCO";
	m_jobNameTable[JT_E_CHOCO] = "CHOCO";
	m_jobNameTable[JT_E_MARTIN] = "MARTIN";
	m_jobNameTable[JT_E_SPRING_RABBIT] = "G_SPRING_RABBIT";
	m_jobNameTable[JT_ZOMBIE_SLAUGHTER] = "ZOMBIE_SLAUGHTER";
	m_jobNameTable[JT_RAGGED_ZOMBIE] = "RAGGED_ZOMBIE";
	m_jobNameTable[JT_HELL_POODLE] = "HELL_POODLE";
	m_jobNameTable[JT_BANSHEE] = "BANSHEE";
	m_jobNameTable[JT_G_BANSHEE] = "BANSHEE";
	m_jobNameTable[JT_FLAME_SKULL] = "FLAME_SKULL";
	m_jobNameTable[JT_NECROMANCER] = "NECROMANCER";
	m_jobNameTable[JT_FALLINGBISHOP] = "FALLINGBISHOP";
	m_jobNameTable[JT_BEELZEBUB_FLY] = "HUNTER_FLY";
	m_jobNameTable[JT_BEELZEBUB] = "DRAGON_FLY";
	m_jobNameTable[JT_BEELZEBUB_] = "BEELZEBUB_";
	m_jobNameTable[JT_TRISTAN_3RD] = "TRISTAN_3RD";
	m_jobNameTable[JT_E_LORD_OF_DEATH] = "LORD_OF_DEATH";
	m_jobNameTable[JT_CRYSTAL_5] = "CRYSTAL_4";
	m_jobNameTable[JT_E_SHINING_PLANT] = "SHINING_PLANT";
	m_jobNameTable[JT_ECLIPSE_P] = "ECLIPSE_P";
	m_jobNameTable[JT_WOOD_GOBLIN] = "WOOD_GOBLIN";
	m_jobNameTable[JT_LES] = "LES";
	m_jobNameTable[JT_VAVAYAGA] = "VAVAYAGA";
	m_jobNameTable[JT_UZHAS] = "UZHAS";
	m_jobNameTable[JT_MAVKA] = "MAVKA";
	m_jobNameTable[JT_GOPINICH] = "GOPINICH";
	m_jobNameTable[JT_G_MAVKA] = "MAVKA";
	m_jobNameTable[JT_FREEZER_R] = "FREEZER";
	m_jobNameTable[JT_GARM_BABY_R] = "GARM_BABY";
	m_jobNameTable[JT_GARM_R] = "GARM";
	m_jobNameTable[JT_GOPINICH_R] = "GOPINICH";
	m_jobNameTable[JT_G_RANDGRIS_] = "G_RANDGRIS";
	m_jobNameTable[JT_G_LOLI_RURI] = "LOLI_RURI";
	m_jobNameTable[JT_G_KNIGHT_OF_ABYSS] = "knight_of_abyss";
	m_jobNameTable[JT_POURING] = "POURING";
	m_jobNameTable[JT_EVENT_SEYREN] = "SEYREN";
	m_jobNameTable[JT_EVENT_KATRINN] = "KATRINN";
	m_jobNameTable[JT_EVENT_BAPHOMET_] = "Baphomet";
	m_jobNameTable[JT_EVENT_ZOMBIE] = "Zombie";
	m_jobNameTable[JT_SWORD_GUARDIAN_] = "Kguardian90_7.gr2";
	m_jobNameTable[JT_BOW_GUARDIAN_] = "Aguardian90_8.gr2";
	m_jobNameTable[JT_E_CONDOR] = "Condor";
	m_jobNameTable[JT_E_TREASURE1] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_E_TREASURE2] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_BOMBPORING] = "BOMBPORING";
	m_jobNameTable[JT_BARRICADE] = "BARRICADE";
	m_jobNameTable[JT_BARRICADE_] = "BARRICADE";
	m_jobNameTable[JT_S_EMPEL_1] = "s_empel_1";
	m_jobNameTable[JT_S_EMPEL_2] = "s_empel_2";
	m_jobNameTable[JT_B_S_GUARDIAN] = "Kguardian90_7.gr2";
	m_jobNameTable[JT_B_B_GUARDIAN] = "Aguardian90_8.gr2";
	m_jobNameTable[JT_CRYSTAL_6] = "CRYSTAL_1";
	m_jobNameTable[JT_CRYSTAL_7] = "CRYSTAL_2";
	m_jobNameTable[JT_CRYSTAL_8] = "CRYSTAL_3";
	m_jobNameTable[JT_CRYSTAL_9] = "CRYSTAL_4";
	m_jobNameTable[JT_OBJ_A] = "OBJ_A";
	m_jobNameTable[JT_OBJ_B] = "OBJ_B";
	m_jobNameTable[JT_OBJ_NEUTRAL] = "OBJ_NEUTRAL";
	m_jobNameTable[JT_OBJ_FLAG_A] = "OBJ_FLAG_A";
	m_jobNameTable[JT_OBJ_FLAG_B] = "OBJ_FLAG_B";
	m_jobNameTable[JT_OBJ_A2] = "OBJ_A2";
	m_jobNameTable[JT_OBJ_B2] = "OBJ_B2";
	m_jobNameTable[JT_MOROCC] = "MOROCC";
	m_jobNameTable[JT_MOROCC_] = "MOROCC";
	m_jobNameTable[JT_MOROCC_1] = "MOROCC_1";
	m_jobNameTable[JT_MOROCC_2] = "MOROCC_2";
	m_jobNameTable[JT_MOROCC_3] = "MOROCC_3";
	m_jobNameTable[JT_MOROCC_4] = "MOROCC_4";
	m_jobNameTable[JT_G_MOROCC_1] = "MOROCC_1";
	m_jobNameTable[JT_G_MOROCC_2] = "MOROCC_2";
	m_jobNameTable[JT_G_MOROCC_3] = "MOROCC_3";
	m_jobNameTable[JT_G_MOROCC_4] = "MOROCC_4";
	m_jobNameTable[JT_JAKK_H] = "JAKK";
	m_jobNameTable[JT_WHISPER_H] = "WHISPER";
	m_jobNameTable[JT_DEVIRUCHI_H] = "DEVIRUCHI";
	m_jobNameTable[JT_BAPHOMET_I] = "BAPHOMET_I";
	m_jobNameTable[JT_PIAMETTE] = "PIAMETTE";
	m_jobNameTable[JT_WISH_MAIDEN] = "WISH_MAIDEN";
	m_jobNameTable[JT_GARDEN_KEEPER] = "GARDEN_KEEPER";
	m_jobNameTable[JT_GARDEN_WATCHER] = "GARDEN_WATCHER";
	m_jobNameTable[JT_BLUE_FLOWER] = "BLUE_FLOWER";
	m_jobNameTable[JT_RED_FLOWER] = "RED_FLOWER";
	m_jobNameTable[JT_YELL_FLOWER] = "YELL_FLOWER";
	m_jobNameTable[JT_CONSTANT_] = "CONSTANT";
	m_jobNameTable[JT_TREASURE_BOX41] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX42] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX43] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX44] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX45] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX46] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX47] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX48] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_TREASURE_BOX49] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_PIAMETTE_] = "PIAMETTE";
	m_jobNameTable[JT_TREASURE_BOX_I] = "TREASUREBOX_2.gr2";
	m_jobNameTable[JT_NAGHT_SIEGER] = "NAGHT_SIEGER";
	m_jobNameTable[JT_ENTWEIHEN] = "ENTWEIHEN";
	m_jobNameTable[JT_G_ENTWEIHEN_R] = "G_ENTWEIHEN";
	m_jobNameTable[JT_G_ENTWEIHEN_H] = "G_ENTWEIHEN";
	m_jobNameTable[JT_G_ENTWEIHEN_M] = "G_ENTWEIHEN";
	m_jobNameTable[JT_G_ENTWEIHEN_S] = "G_ENTWEIHEN";
	m_jobNameTable[JT_ANTONIO_] = "ANTONIO";
	m_jobNameTable[JT_P_CHUNG_E] = "\xbb\xf5\xc7\xd8\xc0\xce\xc7\xfc";
	m_jobNameTable[JT_NIGHTMARE_T] = "Nightmare";
	m_jobNameTable[JT_M_WILD_ROSE] = "Wild_Rose";
	m_jobNameTable[JT_M_DOPPELGANGER] = "DOPPELGANGER";
	m_jobNameTable[JT_M_YGNIZEM] = "YGNIZEM";
	m_jobNameTable[JT_E_STROUF] = "STROUF";
	m_jobNameTable[JT_E_MARC] = "MARC";
	m_jobNameTable[JT_E_OBEAUNE] = "OBEAUNE";
	m_jobNameTable[JT_E_VADON] = "VADON";
	m_jobNameTable[JT_E_MARINA] = "MARINA";
	m_jobNameTable[JT_E_PORING] = "Poring";
	m_jobNameTable[JT_BANSHEE_MASTER] = "BANSHEE_MASTER";
	m_jobNameTable[JT_BEHOLDER_MASTER] = "BEHOLDER_MASTER";
	m_jobNameTable[JT_COBALT_MINERAL] = "COBALT_MINERAL";
	m_jobNameTable[JT_HEAVY_METALING] = "HEAVY_METALING";
	m_jobNameTable[JT_HELL_APOCALIPS] = "HELL_APOCALIPS";
	m_jobNameTable[JT_ZAKUDAM] = "ZAKUDAM";
	m_jobNameTable[JT_KUBLIN] = "KUBLIN";
	m_jobNameTable[JT_I_HIGH_ORC] = "HIGH_ORC";
	m_jobNameTable[JT_I_ORC_ARCHER] = "ORC_ARCHER";
	m_jobNameTable[JT_I_ORC_SKELETON] = "ORC_SKELETON";
	m_jobNameTable[JT_I_ORC_LADY] = "ORC_LADY";
	m_jobNameTable[JT_DANDELION] = "DANDELION";
	m_jobNameTable[JT_MER_LIF] = "LIF";
	m_jobNameTable[JT_MER_AMISTR] = "AMISTR";
	m_jobNameTable[JT_MER_FILIR] = "FILIR";
	m_jobNameTable[JT_MER_VANILMIRTH] = "VANILMIRTH";
	m_jobNameTable[JT_MER_LIF2] = "LIF2";
	m_jobNameTable[JT_MER_AMISTR2] = "AMISTR2";
	m_jobNameTable[JT_MER_FILIR2] = "FILIR2";
	m_jobNameTable[JT_MER_VANILMIRTH2] = "VANILMIRTH2";
	m_jobNameTable[JT_MER_LIF_H] = "LIF_H";
	m_jobNameTable[JT_MER_AMISTR_H] = "AMISTR_H";
	m_jobNameTable[JT_MER_FILIR_H] = "FILIR_H";
	m_jobNameTable[JT_MER_VANILMIRTH_H] = "VANILMIRTH_H";
	m_jobNameTable[JT_MER_LIF_H2] = "LIF_H2";
	m_jobNameTable[JT_MER_AMISTR_H2] = "AMISTR_H2";
	m_jobNameTable[JT_MER_FILIR_H2] = "FILIR_H2";
	m_jobNameTable[JT_MER_VANILMIRTH_H2] = "VANILMIRTH_H2";
	m_jobNameTable[JT_MER_ARCHER01] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER02] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER03] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER04] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER05] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER06] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER07] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER08] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER09] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_ARCHER10] = "\xbf\xa9\x5c\xc8\xb0\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER01] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER02] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER03] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER04] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER05] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER06] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER07] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER08] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER09] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_LANCER10] = "\xb3\xb2\x5c\xc3\xa2\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN01] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN02] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN03] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN04] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN05] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN06] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN07] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN08] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN09] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";
	m_jobNameTable[JT_MER_SWORDMAN10] = "\xb3\xb2\x5c\xb0\xcb\xbf\xeb\xba\xb4";

	m_sexNameTable.resize(2, "F");
	m_sexNameTable[SEX_FEMALE] = "F";
	m_sexNameTable[SEX_MALE]   = "M";

	m_guildSkillUseLevelList.resize(SKID_GD_LAST - SKID_GD_APPROVAL, 0);


	m_skillUseLevelList.resize(1039, 0);
	m_merSkillUseLevelList.resize(SKID_MERCENARY_LAST - SKID_MERCENARY_BEGIN, 0);
	m_homunSkillUseLevelList.resize(SKID_HOMUN_LAST - SKID_HOMUN_BEGIN, 0);
}

#if 1
void	CSession :: InitVariables()
{
	float	attrZoom = 5.0f;
	m_oneCellDist = _sqrt(attrZoom*attrZoom + attrZoom*attrZoom);

	int		randNum = random(1024)+1;
	m_gold		= 0;
	m_exp = 0;
	m_jobexp	= 0;
	m_maxWeight = 0;
	m_weight	= 0;

	SetMaxhp(randNum);
	SetHp(randNum);

	m_gold			= randNum;
	m_level			= randNum;
	m_joblevel		= randNum;
	strcpy(m_cName, "Sleepyhead");

	m_str;
	m_agi;
	m_vit;
	m_int;
	m_dex;
	m_luk;
	m_plusInt = 0;
	m_plusDex = 0;
	m_plusLuk = 0;
	m_plusStr = 0;
	m_plusAgi = 0;
	m_plusVit = 0;

	m_standardStr	= 2;
	m_standardAgi = 2;
	m_standardLuk = 2;
	m_standardVit = 2;
	m_standardInt = 2;
	m_standardDex = 2;

	m_point = 0;

	m_attPower = 0;
	m_refiningPower = 0;
	m_max_mattPower = 0;
	m_min_mattPower = 0;
	m_itemDefPower = 0;
	m_plusdefPower = 0;
	m_mdefPower = 0;
	m_plusmdefPower = 0;
	m_hitSuccessValue = 0;
	m_avoidSuccessValue = 0;
	m_plusAvoidSuccessValue = 0;
	m_criticalSuccessValue = 0;
	m_ASPD = 0;
	m_plusASPD = 0;

	m_exp = 0;
	m_jobexp	= 0;

	m_nextexp = 999999;
	m_jobnextexp = 999999;


	m_cartCurCount = 0;
	m_cartMaxCount = 0;
	m_cartCurWeight = 0;
	m_cartMaxWeight = 0;


	m_maxNumOfSellItemOfMerchant = 13;

	m_camp = 0;
	m_camp_A = -1;
	m_camp_B = -1;

	m_charfont = 0;


	m_attackRange = 20;

	m_charSlot = 0;
	m_equipArrowIndex = 0;
	m_expDivType = 0;
	m_itemCollectType = 0;
	m_itemDivType = 0;

	m_isItemSnap = FALSE;

	m_isMonsterSnap = TRUE;
	m_monsterSnapOn_Skill = TRUE;
	m_monsterSnapOn_NoSkill = FALSE;
	m_bMakeMissEffect = TRUE;
	m_isNoShift = FALSE;

	m_isQuickSpell = FALSE;
	m_isQuickSpell2 = FALSE;
	m_isBattle = FALSE;
	m_isBattleChat = FALSE;

	m_isAura = TRUE;
	m_isNoCtrl = FALSE;
	m_isSkillFail = TRUE;
	m_isLogInOut = TRUE;
	m_isShopping = FALSE;
	m_isNoTalkMsg = FALSE;
	m_isNoTalkMsg2 = FALSE;
	m_isNoTrade = FALSE;
	m_isBmChat = FALSE ;
	m_isStartBmChat = FALSE ;
	m_isWindow = FALSE;
	g_session.m_isQ2begin = 0;
	m_isShowNameTypeOld = true;
	m_amIParent = false;
	RandomScreenFlag = true;
	m_isDA_illusionON = FALSE;
	m_isMiniGame = FALSE;

	m_MightyGauge = 0;

	{
		MEMORYSTATUS ms;
		memset(&ms, 0, sizeof(MEMORYSTATUS));
		ms.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&ms);
		m_isLowSystemMemory = (ms.dwTotalPhys < 163000000) ? TRUE : FALSE;
	}

	memset(&m_charBlockInfo2[0],0,sizeof(TAG_CHARACTER_BLOCK_INFO)*9);

}
#endif

void	CSession :: ClearShortenItemName()
{
	for (int i=0; i<9; i++)
		m_shortenItemName[i] = "nothing";
}

void	CSession :: SetShortenItemName(const char *shortenItemName, int slot)
{

	sAssert(slot>=0 && slot<=NUM_SHORTCUT_SLOT-1);
	for (int i = m_shortcutSlotCnt * ONE_SLOTCOUNT ; i< (m_shortcutSlotCnt + 1) * ONE_SLOTCOUNT ; ++i) {
		if (m_shortenItemName[i] == shortenItemName)
			m_shortenItemName[i] = "nothing";
	}

	if (strlen(shortenItemName)==0)
		m_shortenItemName[slot] = "nothing";
	else
		m_shortenItemName[slot] = shortenItemName;

	sAssert(m_shortenItemName[slot].size());
}

const char *	CSession :: GetShortenItemName(int slot)
{
	return	m_shortenItemName[slot].c_str();
}

void	CSession :: RecalcAveragePingTime(unsigned long pingTime)
{
	m_pingTimeList.push_back(pingTime);
	while (m_pingTimeList.size() > 3)
		m_pingTimeList.erase(m_pingTimeList.begin());

	unsigned long sum = 0;
	std::list<unsigned long>::iterator it = m_pingTimeList.begin();
	while (it != m_pingTimeList.end()) {
		sum += *it;
		it++;
	}

	m_averagePingTime = sum / m_pingTimeList.size();
}

void	CSession :: InitWhenServerMove()
{
	Trace("InitWhenServerMove()");
	m_itemList.erase(m_itemList.begin(), m_itemList.end());
	m_pingTimeList.erase(m_pingTimeList.begin(), m_pingTimeList.end());

	for (int i = 0; i < 10; ++i) m_equipedItems[i].Reset();
}

int		CSession :: GetNumItem()
{
	return	m_itemList.size();
}

int		CSession :: GetNumShopItem()
{
	return	m_shopItemList.size();
}

int		CSession :: GetNumMerchantShopItem()
{
	return	m_merchantShopItemList.size();
}

int		CSession :: GetNumMerchantHopeToSellItem()
{
	return	m_merchantHopeToSellItemList.size();
}

int		CSession :: GetNumPurchaseItem()
{
	return	m_purchaseItemList.size();
}

int		CSession :: GetNumMerchantPurchaseItem()
{
	return	m_merchantPurchaseItemList.size();
}

int		CSession :: GetNumSellItem()
{
	return	m_sellItemList.size();
}

int		CSession :: GetNumStoreItem()
{
	return	m_storeItemList.size();
}

int		CSession :: GetNumMerchantItem()
{
	return	m_merchantItemList.size();
}

int		CSession :: GetNumMerchantMirrorItem()
{
	return	m_merchantMirrorItemList.size();
}

void	CSession :: ClearExchangeItemList()
{
	m_myExchangeMoney = 0;
	for (int i=0; i<10; i++)
		m_exchangeItems[i].m_num = 0;
}

void	CSession :: CancelExchangeNoRefresh()
{
	if (g_bOldServer) {
		for (int i=0; i<10; i++) {
			if (m_exchangeItems[i].m_num)
				AddItemNoRefresh(&m_exchangeItems[i]);
		}
		SetGoldNoRefresh(GetGold() + GetMyExchangeMoney());
	}

	ClearExchangeItemList();
}

void	CSession :: CancelExchange()
{
	if (g_bOldServer) {
		for (int i=0; i<10; i++) {
			if (m_exchangeItems[i].m_num)
				AddItem(&m_exchangeItems[i]);
		}
		SetGold(GetGold() + GetMyExchangeMoney());
	}

	ClearExchangeItemList();
}

bool	CSession :: MoveItemToExchangeList(ITEM_INFO itemInfo)
{
	if (IsExchangeItemListFull())
		return	false;
	DecItem(itemInfo.m_itemIndex, itemInfo.m_num);
	AddToExchangeList(itemInfo);

	return	true;
}

ITEM_INFO	CSession :: GetExchangeItemInfo(int sequence)
{
	sAssert(sequence>=0 && sequence<10);
	return	m_exchangeItems[sequence];
}

ITEM_INFO	CSession :: GetOtherExchangeItemInfo(int sequence)
{
	sAssert(sequence>=0 && sequence<10);
	return	m_otherExchangeItems[sequence];
}

void	CSession :: SetToExchangeList(ITEM_INFO itemInfo)
{
	itemInfo.m_itemIndex;
	itemInfo.m_itemName;
	itemInfo.m_num;
	bool	isAdded = false;
	for (int i=0; i<10; i++) {
		if (m_exchangeItems[i].m_num) {
			if (m_exchangeItems[i].m_itemIndex == itemInfo.m_itemIndex) {
				m_exchangeItems[i].m_num = itemInfo.m_num;
				isAdded = true;
				break;
			}
		}
	}
	if (!isAdded) {
		for (int i=0; i<10; i++) {
			if (m_exchangeItems[i].m_num == 0) {
				m_exchangeItems[i] = itemInfo;
				break;
			}
		}
	}
	UIExchangeWnd *exchangeWnd = g_windowMgr.GetExchangeWnd();
	if (exchangeWnd)
		exchangeWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

void	CSession :: AddToExchangeList(ITEM_INFO itemInfo)
{
	itemInfo.m_itemIndex;
	itemInfo.m_itemName;
	itemInfo.m_num;
	bool	isAdded = false;
	for (int i=0; i<10; i++) {
		if (m_exchangeItems[i].m_num) {
			if (m_exchangeItems[i].m_itemIndex == itemInfo.m_itemIndex) {
				m_exchangeItems[i].m_num += itemInfo.m_num;
				isAdded = true;
				break;
			}
		}
	}
	if (!isAdded) {
		for (int i=0; i<10; i++) {
			if (m_exchangeItems[i].m_num == 0) {
				m_exchangeItems[i] = itemInfo;
				break;
			}
		}
	}
	UIExchangeWnd *exchangeWnd = g_windowMgr.GetExchangeWnd();
	if (exchangeWnd)
		exchangeWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

BOOL	CSession :: IsExchangeItemListFull()
{
	return	(GetFilledExchangeItemList() == 10);
}

int		CSession :: GetFilledExchangeItemList()
{
	int		count = 0;
	for (int i=0; i<10; i++) {
		if (m_exchangeItems[i].m_num)
			++count;
	}
	return	count;
}

void	CSession :: ClearOtherExchangeItemList()
{
	m_otherExchangeMoney = 0;
	for (int i=0; i<10; i++)
		m_otherExchangeItems[i].m_num = 0;
}

bool	CSession :: IsExistIndexInMyChangeList(int index)
{
	for (int i=0; i<10; i++) {
		if (m_exchangeItems[i].m_num) {
			if (m_exchangeItems[i].m_itemIndex == index)
				return	true;
		}
	}
	return	false;
}

void	CSession :: SetMyExchangeMoney(int money)
{
	m_myExchangeMoney = money;
	UIExchangeWnd *exchangeWnd = g_windowMgr.GetExchangeWnd();
	if (exchangeWnd)
		exchangeWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

void	CSession :: SetOtherExchangeMoney(int money)
{
	m_otherExchangeMoney = money;
	UIExchangeWnd *exchangeWnd = g_windowMgr.GetExchangeWnd();
	if (exchangeWnd)
		exchangeWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

void	CSession :: SetToOtherExchangeList(ITEM_INFO itemInfo)
{
	for (int i = 0; i < 10; i++) {
		if (m_otherExchangeItems[i].m_num == 0) {
			m_otherExchangeItems[i] = itemInfo;
			break;
		}
	}
	UIExchangeWnd *exchangeWnd = g_windowMgr.GetExchangeWnd();
	if (exchangeWnd)
		exchangeWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

void	CSession :: AddToOtherExchangeList(ITEM_INFO itemInfo)
{
	itemInfo.m_itemIndex;
	itemInfo.m_itemName;
	itemInfo.m_num;
	bool	isAdded = false;
	for (int i=0; i<10; i++) {
		if (m_otherExchangeItems[i].m_num) {
			if (m_otherExchangeItems[i].m_itemIndex == itemInfo.m_itemIndex) {
				m_otherExchangeItems[i].m_num += itemInfo.m_num;
				isAdded = true;
				break;
			}
		}
	}
	if (!isAdded) {
		for (int i=0; i<10; i++) {
			if (m_otherExchangeItems[i].m_num == 0) {
				m_otherExchangeItems[i] = itemInfo;
				break;
			}
		}
	}
	UIExchangeWnd *exchangeWnd = g_windowMgr.GetExchangeWnd();
	if (exchangeWnd)
		exchangeWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);

}

void	CSession :: ClearItem()
{
	m_itemList.erase(m_itemList.begin(), m_itemList.end());

	g_windowMgr.RefreshItem();
}

BOOL	CSession :: IsOtherExchangeItemListFull()
{
	return	(GetNumOtherExchangeItems() == 10);
}

int		CSession :: GetNumOtherExchangeItems()
{
	int		count = 0;
	for (int i=0; i<10; i++) {
		if (m_otherExchangeItems[i].m_num)
			++count;
	}
	return	count;
}

void	CSession :: ClearAntiEquipItemInInventory()
{
	std::list<ITEM_INFO>::iterator	it = m_itemList.begin();
	while (it != m_itemList.end()) {
		switch (it->m_itemType) {
			case	TYPE_ARMOR:
			case	TYPE_ARMORTM:
			case	TYPE_ARMORTB:
			case	TYPE_ARMORMB:
			case	TYPE_ARMORTMB:
			case	TYPE_WEAPON:
			case	TYPE_BOW:
			case	TYPE_BOTHHAND:
			case	TYPE_GUN:
				++it;
				break;
			default:
				it = m_itemList.erase(it);
				break;
		}
	}
	g_windowMgr.RefreshItem();
}

void	CSession :: BitMaskToSlotNum(int wearLocation, int *slotNum)
{

	slotNum[0] = -1;
	slotNum[1] = -1;
	slotNum[2] = -1;

	if (wearLocation == LOCATION_ARROW) {
		slotNum[0] = 15;
		return;
	}

	int		slotMask = wearLocation;
	int		slotIndex = 0;
	for (int i=0; i<10; i++, slotMask>>=1) {
		if (slotMask & 0x01) {
			slotNum[slotIndex] = i;
			++slotIndex;
		}
	}


}

void	CSession :: ClearEquipItemInInventory()
{
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		switch (it->m_itemType) {
			case	TYPE_ARMOR:
			case	TYPE_BOW:
			case	TYPE_BOTHHAND:
			case	TYPE_ARMORTM:
			case	TYPE_ARMORTB:
			case	TYPE_ARMORMB:
			case	TYPE_ARMORTMB:
			case	TYPE_WEAPON:
			case	TYPE_GUN:
				it = m_itemList.erase(it);
				break;
			default:
				++it;
				break;
		}
	}
	g_windowMgr.RefreshItem();
}

void	CSession :: DecItem(DWORD index, int numItem)
{
	Trace("DecItem numItem == %d", numItem);
	std::list<ITEM_INFO>::iterator	it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (it->m_itemIndex == index) {
			if (it->m_num <= numItem) {
				m_itemList.erase(it);
				Trace("DecItem - delete");
			} else {
				Trace("beforeit->m_num=%d", it->m_num);
				it->m_num -= numItem;
				Trace("after it->m_num = %d", it->m_num);
			}
			break;
		}
		it++;
	}
	g_windowMgr.RefreshItem();
}

bool	CSession :: MoveItemFromItemWndToEquipWnd(ITEM_INFO *itemInfo)
{
	char	nItemName[128];
	char	msg[256];
	itemInfo->GetItemName(nItemName);
	sprintf(msg, "%s%s", nItemName, MsgStr(MSI_ITEM_IS_EQUIPED));
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x00c8ff00, 6, 0);

	if (itemInfo->m_isYours == 1)
		itemInfo->m_isYours = 2;

	CGameMode *gameMode = g_modeMgr.GetGameMode();
	CGameActor *theActor = gameMode->GetWorld()->GetGameActorByAID(1);
	if (theActor) {
		if (itemInfo->m_location == LOCATION_RARM) {
			theActor->SetSprWeapon2(atoi(itemInfo->m_itemName.c_str()));
			SetStatusData(theActor);
		} else if (itemInfo->m_location == LOCATION_HEAD) {
			theActor->SetSprAccessory3(atoi(itemInfo->m_itemName.c_str()));
		} else if (itemInfo->m_location == LOCATION_HEAD2) {
			theActor->SetSprAccessory(atoi(itemInfo->m_itemName.c_str()));
		} else if (itemInfo->m_location == LOCATION_HEAD3) {
			theActor->SetSprAccessory2(atoi(itemInfo->m_itemName.c_str()));
		}
	}

	AddEquipItem(itemInfo);
	DecItem(itemInfo->m_itemIndex, 1);
	UIWindow *itemParamChangeDisplayWnd = (UIWindow *)g_windowMgr.GetItemParamChangeDisplayWnd();
	if (itemParamChangeDisplayWnd)
		itemParamChangeDisplayWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	return	true;
}

bool	CSession :: MoveItemFromEquipWndToItemWnd(int wearLocation)
{
	if (wearLocation == LOCATION_ARROW) {
		SetEquipArrowIndwx(0);
	} else {
		int		slotNum[3];

		BitMaskToSlotNum(wearLocation, slotNum);
		if (m_equipedItems[slotNum[0]].m_wearLocation == 0)
			return	false;

		char	msg[256];
		char	nItemName[128];
		m_equipedItems[slotNum[0]].GetItemName(nItemName);
		sprintf(msg, "%s%s", nItemName, MsgStr(MSI_ITEM_IS_REMOVED));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x000000ff, 6, 0);

		CGameMode *gameMode = g_modeMgr.GetGameMode();
		CGameActor *theActor = gameMode->GetWorld()->GetGameActorByAID(1);
		if (theActor) {
			if (m_equipedItems[slotNum[0]].m_wearLocation == LOCATION_RARM) {
				theActor->SetSprWeapon2(0);
				SetStatusData(theActor);
			} else if (m_equipedItems[slotNum[0]].m_wearLocation == LOCATION_HEAD) {
				theActor->SetSprAccessory3(0);
			} else if (m_equipedItems[slotNum[0]].m_wearLocation == LOCATION_HEAD2) {
				theActor->SetSprAccessory(0);
			} else if (m_equipedItems[slotNum[0]].m_wearLocation == LOCATION_HEAD3) {
				theActor->SetSprAccessory2(0);
			}
		}

		m_equipedItems[slotNum[0]].m_wearLocation = 0;
		AddItem(&m_equipedItems[slotNum[0]]);
		m_equipedItems[slotNum[0]].Reset();
		if (slotNum[1] != -1) m_equipedItems[slotNum[1]].Reset();
		if (slotNum[2] != -1) m_equipedItems[slotNum[2]].Reset();
	}

	g_windowMgr.InvalidateUpdateNeededUI();
	if (g_windowMgr.GetItemParamChangeDisplayWnd())
		((UIWindow *)g_windowMgr.GetItemParamChangeDisplayWnd())->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	return	true;
}

void	CSession :: ClearEquipItem()
{
	int *	p = (int *)m_equipedItems;
	for (int i=0; i<190; i++)
		p[i] = 0;
	g_windowMgr.InvalidateUpdateNeededUI();
}

bool	CSession :: AddEquipItem(ITEM_INFO *itemInfo)
{
	sAssert(itemInfo->m_location);
	sAssert(itemInfo->m_wearLocation);

	int		slotNum[3];

	BitMaskToSlotNum(itemInfo->m_wearLocation, slotNum);
	sAssert(slotNum[0] != -1);
	sAssert(m_equipedItems[slotNum[0]].m_num==0);

	m_equipedItems[slotNum[0]] = *itemInfo;
	m_equipedItems[slotNum[0]].m_num = 1;
	m_equipedItems[slotNum[0]].m_deleteTime = itemInfo->m_deleteTime ;

	m_equipedItems[slotNum[0]].m_isYours = itemInfo->m_isYours;

	if (slotNum[1] != -1) {
		sAssert(m_equipedItems[slotNum[1]].m_num==0);
		m_equipedItems[slotNum[1]] = *itemInfo;
		m_equipedItems[slotNum[1]].m_num = 1;
		m_equipedItems[slotNum[1]].m_deleteTime = itemInfo->m_deleteTime ;
		m_equipedItems[slotNum[1]].m_isYours = itemInfo->m_isYours;
	}

	if (slotNum[2] != -1) {
		sAssert(m_equipedItems[slotNum[2]].m_num==0);
		m_equipedItems[slotNum[2]] = *itemInfo;
		m_equipedItems[slotNum[2]].m_num = 1;
		m_equipedItems[slotNum[2]].m_deleteTime = itemInfo->m_deleteTime ;
		m_equipedItems[slotNum[2]].m_isYours = itemInfo->m_isYours;
	}

	InvalidateUI();
	return	true;
}

void	CSession :: ClearOtherUserEquipItem()
{
	int *	p = (int *)m_otherUserEquipedItems;
	for (int i=0; i<190; i++)
		p[i] = 0;
}

bool	CSession :: AddOtherUserEquipItem(ITEM_INFO *itemInfo)
{
	int		slotNum[3];

	BitMaskToSlotNum(itemInfo->m_wearLocation, slotNum);
	sAssert(slotNum[0] != -1);
	sAssert(m_otherUserEquipedItems[slotNum[0]].m_num==0);

	m_otherUserEquipedItems[slotNum[0]] = *itemInfo;
	m_otherUserEquipedItems[slotNum[0]].m_num = 1;
	m_otherUserEquipedItems[slotNum[0]].m_deleteTime = itemInfo->m_deleteTime ;

	m_otherUserEquipedItems[slotNum[0]].m_isYours = itemInfo->m_isYours;

	if (slotNum[1] != -1) {
		sAssert(m_otherUserEquipedItems[slotNum[1]].m_num==0);
		m_otherUserEquipedItems[slotNum[1]] = *itemInfo;
		m_otherUserEquipedItems[slotNum[1]].m_num = 1;
		m_otherUserEquipedItems[slotNum[1]].m_deleteTime = itemInfo->m_deleteTime ;
		m_otherUserEquipedItems[slotNum[1]].m_isYours = itemInfo->m_isYours;
	}

	if (slotNum[2] != -1) {
		sAssert(m_otherUserEquipedItems[slotNum[2]].m_num==0);
		m_otherUserEquipedItems[slotNum[2]] = *itemInfo;
		m_otherUserEquipedItems[slotNum[2]].m_num = 1;
		m_otherUserEquipedItems[slotNum[2]].m_deleteTime = itemInfo->m_deleteTime ;
		m_otherUserEquipedItems[slotNum[2]].m_isYours = itemInfo->m_isYours;
	}

	return	true;
}

ITEM_INFO	CSession :: GetOtherUserEquipedItems(int wearLocation)
{

	if (wearLocation == 15) return	ITEM_INFO(0);

	if (wearLocation<0 || wearLocation>MAX_EQUIP_SLOT-1) return	ITEM_INFO(0);


	return	m_otherUserEquipedItems[wearLocation];
}

void	CSession :: SetItem(ITEM_INFO * itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (it->m_itemIndex == itemInfo->m_itemIndex) {
			if (itemInfo->m_num == 0)
				m_itemList.erase(it);
			else
				*it = *itemInfo;
			g_windowMgr.RefreshItem();
			return;
		}
		it++;
	}
	m_itemList.push_back(*itemInfo);
	g_windowMgr.RefreshItem();
}


ITEM_INFO	CSession :: GetItemInfoBySequence(int sequence)
{
	if (m_itemList.size() == 0)
		return	ITEM_INFO(0);

	list<ITEM_INFO>::iterator	it = m_itemList.begin();
	int		count = 0;
	while (it != m_itemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

bool	CSession :: AddItemNoRefresh(ITEM_INFO * itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (it->m_itemIndex == itemInfo->m_itemIndex) {
			it->m_num += itemInfo->m_num;
			g_windowMgr.RefreshItem();
			return	false;
		}
		it++;
	}
	m_itemList.push_back(*itemInfo);
	return	true;
}

bool	CSession :: AddItem(ITEM_INFO *itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (it->m_itemIndex == itemInfo->m_itemIndex) {
			it->m_num += itemInfo->m_num;
			g_windowMgr.RefreshItem();
			return	false;
		}
		it++;
	}
	m_itemList.push_back(*itemInfo);
	m_MaxItemIndex++;
	g_windowMgr.RefreshItem();
	return	true;
}

#if 1
void	CSession :: HP_AddItem(int itemID, bool F1)
{
	ITEM_INFO itemInfo;
	char pickMsg[128];
	char itemName[64];

	if (itemID >= 1000 && itemID < 2000) {
		itemInfo.m_itemType = 5;
		itemInfo.m_location = 2;
	} else {
		itemInfo.m_itemType = 4;
		itemInfo.m_location = HP_GetLocation(itemID);
	}

	itemInfo.m_wearLocation = 0;
	itemInfo.SetItemId(itemID);
	itemInfo.m_num = 1;
	itemInfo.m_itemIndex = g_session.m_MaxItemIndex;
	itemInfo.m_slot[0] = 0;
	itemInfo.m_slot[1] = 0;
	itemInfo.m_slot[2] = 0;
	itemInfo.m_slot[3] = 0;
	itemInfo.m_isIdentified = 1;
	itemInfo.m_isDamaged = 0;
	itemInfo.m_refiningLevel = 0;
	itemInfo.m_deleteTime = 0;
	itemInfo.m_isYours = 0;

	if (itemID == 0x04b1 || itemID == 0x0709) {
		itemInfo.m_price = 3000;
	} else if (itemID == 0x04b4 || itemID == 0x070b) {
		itemInfo.m_price = 4000;
	} else if (itemID == 0x04b7 || itemID == 0x070d) {
		itemInfo.m_price = 5000;
	} else if (itemID == 0x04ba || itemID == 0x0711) {
		itemInfo.m_price = 6000;
	} else if (itemID == 0x04bd || itemID == 0x0713) {
		itemInfo.m_price = 7000;
	} else if (itemID == 0x04c0 || itemID == 0x070f) {
		itemInfo.m_price = 8000;
	} else if (itemID == 0x04c3 || itemID == 0x0715) {
		itemInfo.m_price = 9000;
	} else if (itemID == 0x04d2 || itemID == 0x0716 || itemID == 0x04c7 || itemID == 0x0717) {
		itemInfo.m_price = 10000;
	} else {
		itemInfo.m_price = 1500;
	}

	if (itemInfo.m_itemType == 10 || itemInfo.m_itemType == 16 || itemInfo.m_itemType == 17)
		itemInfo.m_wearLocation = LOCATION_ARROW;

	switch (itemInfo.m_itemType) {
	case 4:
	case 5:
	case 8:
	case 9:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		break;
	default:
		itemInfo.m_location = 0;
		break;
	}

	ITEM_INFO curItemInfo = g_session.GetItemInfoByIndex(itemInfo.m_itemIndex);

	if (F1 == 1) {
		itemInfo.GetItemName(itemName);
		sprintf(pickMsg, MsgStr(MSI_GET_ITEM), itemName, itemInfo.m_num);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(pickMsg), 0xffff00, 5, 0);

		UIWindow *notifyWnd = (UIWindow *)g_windowMgr.MakeWindow(WID_NOTIFYITEMOBTAINWND);
		notifyWnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, itemInfo.m_isIdentified, 0, 0, 0);
		notifyWnd->SendMsg(NULL, UM_SETTITLE, 0, 1, int(itemInfo.m_itemName.c_str()), 0);
		notifyWnd->SendMsg(NULL, UM_SETINFO, int(&itemInfo), 0, 0, 0);
	}

	AddItem(&itemInfo);
}
#endif

void	CSession :: HP_MakingItem()
{
	short index = random(8);
	short kind = random(2);
	if (index == 0) {
		if (!kind)
			HP_AddItem(0x04b1, true);
		else
			HP_AddItem(0x0709, true);
		return;
	}
	if (index == 1) {
		if (!kind)
			HP_AddItem(0x04b4, true);
		else
			HP_AddItem(0x070b, true);
		return;
	}
	if (index == 2) {
		if (!kind)
			HP_AddItem(0x04b7, true);
		else
			HP_AddItem(0x070d, true);
		return;
	}
	if (index == 3) {
		if (!kind)
			HP_AddItem(0x04ba, true);
		else
			HP_AddItem(0x0711, true);
		return;
	}
	if (index == 4) {
		if (!kind)
			HP_AddItem(0x04bd, true);
		else
			HP_AddItem(0x0713, true);
		return;
	}
	if (index == 5) {
		if (!kind)
			HP_AddItem(0x04c0, true);
		else
			HP_AddItem(0x070f, true);
		return;
	}
	if (index == 6) {
		if (!kind)
			HP_AddItem(0x04c3, true);
		else
			HP_AddItem(0x0715, true);
		return;
	}

	kind = random(4);
	if (kind == 0) {
		HP_AddItem(0x04c7, true);
		return;
	}
	if (kind == 1) {
		HP_AddItem(0x0716, true);
		return;
	}
	if (kind == 2) {
		HP_AddItem(0x04d2, true);
		return;
	}
	HP_AddItem(0x0717, true);
}

short	CSession :: HP_GetLocation(int itemID)
{
	switch (itemID) {
	default:
		return	0;
	case 0x1408:
	case 0x1411:
	case 0x1435:
	case 0x145a:
	case 0x145c:
	case 0x145e:
	case 0x148f:
	case 0x14b4:
	case 0x1504:
	case 0x1505:
		return	0x0100;
	case 0x08c2:
	case 0x08e4:
	case 0x08ee:
	case 0x13ee:
	case 0x140f:
	case 0x1437:
	case 0x1467:
	case 0x14c3:
	case 0x14cd:
	case 0x14ed:
		return	0x0200;
	case 0x08da:
	case 0x08db:
	case 0x08dd:
	case 0x08de:
	case 0x13f3:
	case 0x13f6:
	case 0x13f9:
	case 0x1456:
		return	0x0001;
	}
}

void	CSession :: SetStatusData(CGameActor *TheActor)
{
	if (TheActor->m_Hp > TheActor->m_MaxHp)
		TheActor->m_Hp = TheActor->m_MaxHp;
	if (TheActor->m_Hp < 0)
		TheActor->m_Hp = 0;

	SetHp(TheActor->m_Hp);
	SetMaxhp(TheActor->m_MaxHp);

	if (TheActor->m_Sp > TheActor->m_MaxSp)
		TheActor->m_Sp = TheActor->m_MaxSp;

	SetSp(TheActor->m_Sp);
	SetMaxsp(TheActor->m_MaxSp);

	SetExp(TheActor->m_Exp - g_modeMgr.GetGameMode()->GetNextExpValue(TheActor->m_clevel - 1));
	SetLevel(TheActor->m_clevel);
	SetJob(TheActor->GetJob());
	SetStr(TheActor->m_Str);
	SetAgi(TheActor->m_Agi);
	SetVit(TheActor->m_Vit);
	SetInt(TheActor->m_Int);
	SetDex(TheActor->m_Dex);
	SetLuk(TheActor->m_Luk);

	UIStatusWnd *statusWnd = g_windowMgr.GetStatusWnd();
	if (statusWnd)
		statusWnd->SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);

	if (g_modeMgr.GetGameMode()) {
		if (g_windowMgr.GetBasicInfoWnd()) {
			g_windowMgr.GetBasicInfoWnd()->Invalidate();
			g_windowMgr.GetBasicInfoWnd()->InvalidateChildren();
		}
		g_modeMgr.GetGameMode()->InvalidatePlayerGage();
	}
}

int CSession::Mob_SkillReturn(CGameActor *MobActor)
{
	short randNum = rand() % 10;

	if (MobActor->GetJob() == 0x72c) {
		if (randNum < 2)
			return 0x98;
	}
	else if (MobActor->GetJob() == 0x6f9) {
		if (g_used_Skill == 0x111)
			return 0x3fd;
		else if (randNum < 3)
			return 0x13;
	}
	else if (MobActor->GetJob() == 0x48a) {
		if (randNum < 3)
			return 0x1c;
	}
	else if (MobActor->GetJob() == 0x5ef) {
		if (randNum < 3)
			return 0xe7;
	}
	else if (MobActor->GetJob() == 0x402) {
		if (randNum < 3)
			return 0x88;
	}
	else if (MobActor->GetJob() == 0x5d5) {
		if (randNum < 3)
			return 0x3fe;
	}
	else if (MobActor->GetJob() == 0x6c6) {
		if (g_BattleEvent == 0)
			return 0x3ff;
		else if (g_BattleEvent == 1)
			return 0x10f;
		else if (g_BattleEvent == 2)
			return 0x400;
		else if (g_BattleEvent == 3)
			return 0x401;
		else if (g_BattleEvent == 4)
			return 0x402;
	}
	else if (MobActor->GetJob() == 0x6c5) {
		if (g_used_Skill == 0x111) {
			if (g_Mob_used_Skill == 0x403 || g_Mob_used_Skill == 0x404)
				return 0x3fd;
		} else if (g_Mob_used_Skill == 0x404) {
			return 0x3ff;
		}
		if (g_Mob_used_Skill == 0x403)
			return 0x404;
		if (randNum < 2 && g_Mob_used_Skill != 0x10f)
			return 0x403;
	}
	else if (MobActor->GetJob() == 0x698) {
		if (randNum < 2)
			return 0x34;
	}
	else if (MobActor->GetJob() == 0x501) {
		if (randNum < 5)
			return 0xd;
	}
	else if (MobActor->GetJob() == 0x510) {
		if (randNum < 4 && !MobActor->IsEffectExist(0x1ab))
			return 0x1d7;
	}
	else if (MobActor->GetJob() == 0x75d) {
		if (randNum < 1)
			return 0x12;
		else if (randNum < 2)
			return 0x293;
	}
	else if (MobActor->GetJob() == 0x5e8) {
		if (randNum < 3) {
			if (g_Mob_used_Skill != 0x88)
				return 0x88;
		}
		else if (randNum < 4 && MobActor->m_Hp < MobActor->m_MaxHp / 2 && !MobActor->IsEffectExist(0x1ab))
			return 0x1d7;
	}
	else if (MobActor->GetJob() == 0x5e6) {
		if (randNum < 4)
			return 0xe;
		else if (randNum < 5 && !MobActor->IsEffectExist(0x1ab))
			return 0x1d7;
	}
	else if (MobActor->GetJob() == 0x492) {
		if (randNum < 2)
			return 0xf;
	}
	else if (MobActor->GetJob() == 0x55d) {
		if (g_Mob_used_Skill == 0x405) {
			if (g_used_Skill == 0 || g_used_Skill == 0x107 || g_used_Skill == 0x111)
				return 0x17b;
			return 0x3fd;
		}
		else if (randNum < 2)
			return 0x405;
		else if (randNum < 3)
			return 0x13;
	}
	else if (MobActor->GetJob() == 0x55e) {
		if (g_BattleEvent == 5)
			return 0x407;
		else if (g_BattleEvent == 6)
			return 0x408;
		else if (g_BattleEvent == 7)
			return 0x401;
		else if (g_BattleEvent == 8)
			return 0x409;
		else if (g_BattleEvent == 9)
			return 0x40a;
		else if (g_BattleEvent == 10)
			return 0x402;
	}
	else if (MobActor->GetJob() == 0x781) {
		if (randNum < 2)
			return 0xfb;
		else if (randNum < 3 && !MobActor->IsEffectExist(0x1ab))
			return 0x1d7;
	}
	else if (MobActor->GetJob() == 0x691) {
		if (randNum < 2) {
			if (g_Mob_used_Skill != 0xfd)
				return 0xfd;
		}
		else if (randNum < 3)
			return 0x1c;
	}
	else if (MobActor->GetJob() == 0x5e1) {
		if (randNum < 2) {
			if (g_Mob_used_Skill != 0x15)
				return 0x40b;
		}
	}
	else if (MobActor->GetJob() == 0x6d7) {
		if (g_used_Skill == 0x1c)
			return 0x1c;
		else if (randNum < 5 && g_Mob_used_Skill != 0xfe)
			return 0x40c;
	}
	else if (MobActor->GetJob() == 0x55a) {
		if (randNum < 3)
			return 0xc7;
		else if (randNum < 4)
			return 0x6e;
	}
	else if (MobActor->GetJob() == 0x561) {
		if (randNum < 1)
			return 0x12;
		else if (randNum < 2) {
			if (g_Mob_used_Skill != 0x59)
				return 0x59;
		}
	}
	else if (MobActor->GetJob() == 0x6b2) {
		if (randNum < 2) {
			if (MobActor->m_isSprArgbFixed != 1)
				return 8;
		}
		else if (randNum < 3 && !MobActor->IsEffectExist(0x1ab))
			return 0x1d7;
	}
	else if (MobActor->GetJob() == 0x4da) {
		if (randNum < 1) {
			if (MobActor->m_isSprArgbFixed != 1)
				return 8;
		}
		else if (randNum < 2)
			return 0xe7;
		else if (randNum < 3)
			return 0x98;
	}
	else if (MobActor->GetJob() == 0x64d) {
		if (randNum < 1) {
			if (MobActor->m_isSprArgbFixed != 1)
				return 8;
		}
		else if (randNum < 2)
			return 0x98;
		else if (randNum < 3)
			return 0xf;
	}
	else if (MobActor->GetJob() == 0x62e) {
		if (g_BattleEvent == 0)
			return 0x115;
		else if (randNum < 1) {
			if (MobActor->m_isSprArgbFixed != 1)
				return 8;
		}
		else if (randNum < 2)
			return 0x98;
		else if (randNum < 3)
			return 0xf;
		else if (randNum < 5)
			return 0xd;
	}
	else if (MobActor->GetJob() == 0x56c) {
		if (g_BattleEvent % 5 == 1)
			return 0x53;
		else if (randNum < 1)
			return 0xf;
		else if (randNum < 3)
			return 0xd;
	}
	else if (MobActor->GetJob() == 0x6c7) {
		if (g_BattleEvent % 3 == 1)
			return 0x88;
		else if (randNum < 1)
			return 0x1c;
		else if (randNum < 2) {
			if (MobActor->m_Hp < MobActor->m_MaxHp / 2) {
				if (!MobActor->IsEffectExist(0x1ab))
					return 0x1d7;
			}
		}
		else if (randNum < 3 && g_Mob_used_Skill != 0xfe)
			return 0x40c;
	}
	else if (MobActor->GetJob() == 0x6c9) {
		if (g_used_Skill == 0x1c)
			return 0x1c;
		else if (randNum < 2)
			return 0x13;
		else if (randNum < 4)
			return 0xe;
		else if (randNum < 5)
			return 0x14;
	}
	else if (MobActor->GetJob() == 0x74b) {
		if (g_used_Skill == 0x111)
			return 0x40b;
		else if (randNum < 1) {
			if (MobActor->m_Hp < MobActor->m_MaxHp / 2) {
				if (!MobActor->IsEffectExist(0x1ab))
					return 0x1d7;
			}
		}
		else if (randNum < 3)
			return 0x5b;
	}
	else if (MobActor->GetJob() == 0x520) {
		if (g_used_Skill == 0x111 || g_used_Skill == 0x107 || g_used_Skill == 0x19d)
			return 0x16f;
		else if (g_Mob_used_Skill == 0x401)
			return 0x402;
		else if (g_BattleEvent >= 3)
			return 0x401;
	}
	else if (MobActor->GetJob() == 0x503) {
		if (g_Mob_used_Skill == 0x405) {
			if (g_used_Skill == 0 || g_used_Skill == 0x107 || g_used_Skill == 0x111 || g_used_Skill == 0x19d)
				return 0x17b;
			return 0x3fd;
		}
		else if (MobActor->m_Hp < 15000) {
			if (!MobActor->IsEffectExist(0x1ab))
				return 0x1d7;
		}
		if (g_BattleEvent % 8 == 1)
			return 0xfb;
		else if (g_BattleEvent % 11 == 4)
			return 0x405;
		else if (randNum < 2)
			return 0x170;
	}
	else if (MobActor->GetJob() == 0x4e4) {
		if (randNum < 2) {
			if (g_Mob_used_Skill != 0x54)
				return 0x54;
		}
		else if (randNum < 3) {
			if (g_Mob_used_Skill != 0xf)
				return 0xf;
		}
		else if (randNum < 4) {
			if (g_Mob_used_Skill != 0x59)
				return 0x59;
		}
	}
	else if (MobActor->GetJob() == 0x5eb) {
		if (g_used_Skill == 0x35) {
			if (g_Mob_used_Skill != 0xe7)
				return 0xe7;
		}
		if (g_Mob_used_Skill == 0xf)
			return 0x40b;
		else if (randNum < 2)
			return 0xf;
		else if (randNum < 4)
			return 0xd;
	}
	else if (MobActor->GetJob() == 0x77c) {
		if (g_BattleEvent == 6)
			return 0x115;
		else if (g_BattleEvent % 12 == 4)
			return 0x1dc;
		else if (g_used_Skill == 0x111)
			return 0x173;
		else if (randNum < 1)
			return 0x40d;
		else if (randNum < 2) {
			if (g_Mob_used_Skill != 0x293)
				return 0x293;
		}
	}
	else if (MobActor->GetJob() == 0x6f1) {
		if (g_Mob_used_Skill == 0x405) {
			if (g_used_Skill == 0 || g_used_Skill == 0x107 || g_used_Skill == 0x111 || g_used_Skill == 0x19d)
				return 0x17b;
			return 0x3fd;
		}
		else if (g_used_Skill == 0x1c && !MobActor->IsEffectExist(0x1ab))
			return 0x1d7;
		else if (g_BattleEvent == 0)
			return 0x172;
		else if (g_BattleEvent % 10 == 4)
			return 0x6e;
		else if (g_Mob_used_Skill == 0x6e && MobActor->m_isSprArgbFixed != 1)
			return 8;
		else if (randNum < 1) {
			if (g_Mob_used_Skill != 0x172)
				return 0x172;
		}
		else if (randNum < 2)
			return 0x405;
	}
	else if (MobActor->GetJob() == 0x47e) {
		if (g_used_Skill == 0x111)
			return 0x53;
		else if (randNum < 1)
			return 0xc7;
		else if (randNum < 2)
			return 0x88;
		else if (randNum < 3 && !MobActor->IsEffectExist(0x1ab))
			return 0x1d7;
	}
	else if (MobActor->GetJob() == 0x509) {
		ITEM_INFO itemInfo;
		if (g_BattleEvent % 10 == 1) {
			itemInfo = g_session.GetEquipedItems(8);
			if (atoi(itemInfo.m_itemName.c_str()) > 0) {
				itemInfo = g_session.GetEquipedItems(0);
				if (atoi(itemInfo.m_itemName.c_str()) > 0) {
					itemInfo = g_session.GetEquipedItems(9);
					if (atoi(itemInfo.m_itemName.c_str()) > 0)
						return 0x40e;
				}
			}
		}
		if (g_BattleEvent % 6 == 2)
			return 0x1c;
		if (g_BattleEvent % 10 == 9)
			return 0x55;
		if (randNum < 2)
			return 0x176;
	}
	else if (MobActor->GetJob() == 0x487) {
		if (g_used_Skill == 0x10e)
			return 8;
		else if (g_used_Skill == 0x1c)
			return 0xf;
		else if (randNum < 1)
			return 0xfb;
		else if (randNum < 2) {
			if (!MobActor->IsEffectExist(0x1ab))
				return 0x1d7;
		}
		else if (randNum < 3) {
			if (g_Mob_used_Skill != 0xfe)
				return 0x40c;
		}
	}
	else if (MobActor->GetJob() == 0x43f) {
		if (g_used_Skill == 0x111)
			return 0x6e;
		else if (randNum < 1) {
			if (!MobActor->IsEffectExist(0x1ab))
				return 0x1d7;
		}
		else if (randNum < 2)
			return 0x88;
		else if (randNum < 3) {
			if (!MobActor->IsEffectExist(0x1ab))
				return 0x6e;
		}
		else if (randNum < 4)
			return 0x173;
	}
	else if (MobActor->GetJob() == 0x50f) {
		if (g_used_Skill == 0x1c)
			return 0x12;
		else if (randNum < 1)
			return 0xd;
		else if (randNum < 2)
			return 0x176;
		else if (randNum < 3)
			return 0x5b;
		else if (randNum < 4)
			return 0x59;
	}
	else if (MobActor->GetJob() == 0x6ac) {
		if (g_BattleEvent % 10 == 0 && g_BattleEvent > 8)
			return 0x3ff;
		else if (g_Mob_used_Skill == 0x3ff)
			return 0x10f;
		else if (g_BattleEvent % 10 == 7)
			return 0x1c;
		else if (randNum < 3)
			return 0x174;
	}
	return 0;
}

void CSession::GainSkill(int skillID, short LV, bool F1)
{
	char msg[100];
	SKILL_INFO skillInfo;

	if (skillID <= SKID_PR_KYRIE) {
		if (skillID != SKID_PR_KYRIE) {
			switch (skillID) {
			case SKID_AL_HEAL:
				skillInfo.type = SKILLTYPE_TOME;
				skillInfo.spcost = LV * 10;
				break;
			case SKID_AL_WARP:
			case SKID_TF_DETOXIFY:
				skillInfo.type = SKILLTYPE_TOME;
				skillInfo.spcost = 20;
				break;
			case SKID_AL_ANGELUS:
				skillInfo.type = SKILLTYPE_TOME;
				skillInfo.spcost = 50;
				break;
			case SKID_AL_BLESSING:
				skillInfo.type = SKILLTYPE_TOCHARACTER;
				skillInfo.spcost = LV * 10;
				break;
			default:
				skillInfo.type = SKILLTYPE_TOME;
				skillInfo.spcost = 999;
				break;
			}
		} else {
			skillInfo.type = SKILLTYPE_TOME;
			skillInfo.spcost = 10;
		}
	} else {
		switch (skillID) {
		case SKID_MO_TRIPLEATTACK:
			skillInfo.type = SKILLTYPE_PASSIVE;
			skillInfo.spcost = 1;
			break;
		case SKID_MO_COMBOFINISH:
			skillInfo.type = SKILLTYPE_TOCHARACTER;
			if (LV <= 2)
				skillInfo.spcost = 2;
			else
				skillInfo.spcost = LV;
			break;
		case SKID_MO_SPIRITSRECOVERY:
			skillInfo.type = SKILLTYPE_TOME;
			skillInfo.spcost = 0;
			break;
		case SKID_TK_STORMKICK:
			skillInfo.type = SKILLTYPE_TOCHARACTER;
			skillInfo.spcost = 5;
			break;
		case SKID_MO_EXPLOSIONSPIRITS:
			skillInfo.type = SKILLTYPE_TOME;
			skillInfo.spcost = 40;
			break;
		default:
			skillInfo.type = SKILLTYPE_TOME;
			skillInfo.spcost = 999;
			break;
		}
	}

	skillInfo.attackRange = 1;
	skillInfo.level = LV;
	skillInfo.SKID = skillID;
	skillInfo.upgradable = 0;
	skillInfo.skillName = skillInfo.GetSkillIdName();

	AddSkillItem(skillInfo, true);
	RefreshSkillItemWnd();

	if (F1 == 1) {
		if (LV == 1)
			sprintf(msg, "Learned the skill [%s]", skillInfo.GetName());
		else
			sprintf(msg, "[%s] skill raised to Lv %d", skillInfo.GetName(), int(LV));
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffc8c8, 0, 0);
	}
}

short	CSession :: HaveThisSkill(int skillID)
{
	int size = m_skillInfoList.size();
	for (int i=0; i<size; ++i) {
		SKILL_INFO skillInfo;
		skillInfo = GetSkillItemInfoBySequence(i);
		if (skillInfo.SKID == skillID)
			return	skillInfo.level;
	}
	return	0;
}

const char *	CSession :: GetItemName(int index)
{
	ITEM_INFO	itemInfo = GetItemInfoByIndex(index);
	if (itemInfo.IsValid()) {
		return	itemInfo.m_itemName.c_str();
	} else {
		for (int i=0; i<10; i++) {
			if (m_equipedItems[i].m_num==1 && m_equipedItems[i].m_itemIndex==index)
				return	m_equipedItems[i].m_itemName.c_str();
		}
	}

	return	"Black Potion";

}

bool	CSession :: IsValidItemIndexInEquiped(int itemIndex)
{
	if (itemIndex!=0 && itemIndex==m_equipArrowIndex)  return	true;

	for (int i=0; i<10; i++) {
		if (m_equipedItems[i].m_num==1 && m_equipedItems[i].m_itemIndex==itemIndex) return	true;
	}

	return	false;
}

bool	CSession :: IsValidItemIndexInUnequiped(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (it->m_itemIndex == itemIndex)
			return	true;
		it++;
	}
	return	false;
}

ITEM_INFO	CSession :: GetItemInfoByIndex(int index)
{
	if (m_itemList.size() == 0) return	ITEM_INFO(0);

	list<ITEM_INFO>::iterator	it = m_itemList.begin();

	for (; it != m_itemList.end(); ++it)
		if (it->m_itemIndex == index)
			return	(*it);

	return	ITEM_INFO(0);
}

ITEM_INFO	CSession :: GetItemInfoById(int itemId)
{
	if (m_itemList.size() == 0) return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_itemList.begin();

	for (; it != m_itemList.end(); ++it)
		if (it->GetItemId() == itemId)
			return	(*it);

	return	ITEM_INFO(0);
}

void	CSession :: HP_EquipItemById(int itemId)
{
	ITEM_INFO itemInfo = GetItemInfoById(itemId);
	if (itemInfo.IsValid()) {
		if (GetWeaponType(itemInfo.GetItemId()) == 1)
			return;
		if (GetWeaponType(itemInfo.GetItemId()) == 12)
			return;

		MoveItemFromEquipWndToItemWnd(itemInfo.m_location);
		itemInfo.m_wearLocation = itemInfo.m_location;
		MoveItemFromItemWndToEquipWnd(&itemInfo);
	}
}

bool	CSession :: DoPCHaveItem(int itemId)
{
	ITEM_INFO itemInfo = GetItemInfoById(itemId);
	if (itemInfo.IsValid())
		return	true;

	itemInfo = g_session.GetEquipedItems(0);
	if (itemInfo.GetItemId() == itemId)
		return	true;

	itemInfo = g_session.GetEquipedItems(1);
	if (itemInfo.GetItemId() == itemId)
		return	true;

	itemInfo = g_session.GetEquipedItems(8);
	if (itemInfo.GetItemId() == itemId)
		return	true;

	itemInfo = g_session.GetEquipedItems(9);
	if (itemInfo.GetItemId() == itemId)
		return	true;

	return	false;
}

void	CSession :: ClearPurchaseItem()
{
	m_purchaseItemList.erase(m_purchaseItemList.begin(), m_purchaseItemList.end());
}

ITEM_INFO	CSession :: GetPurchaseItemInfoBySequence(int sequence)
{
	list<ITEM_INFO>::iterator	it = m_purchaseItemList.begin();
	int		count = 0;
	while (it != m_purchaseItemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetPurchaseItemTotalPrice()
{
	int		totalPrice = 0;
	std::list<ITEM_INFO>::iterator	it = m_purchaseItemList.begin();
	while (it != m_purchaseItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		totalPrice += itemInfo.m_realPrice * itemInfo.m_num;
		it++;
	}
	return	totalPrice;
}

void	CSession :: AddPurchaseItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_purchaseItemList.begin();
	while (it != m_purchaseItemList.end()) {
		if (it->m_itemName == itemInfo.m_itemName) {
			it->m_num += itemInfo.m_num;
			return;
		}
		it++;
	}
	m_purchaseItemList.push_back(itemInfo);
}

void	CSession :: DecPurchaseItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_purchaseItemList.begin();

	while (it != m_purchaseItemList.end())
	{
		ITEM_INFO &		item = (*it);
		if (item.m_itemName == itemInfo.m_itemName)
		{
			sAssert(item.m_num >= itemInfo.m_num);
			item.m_num -= itemInfo.m_num;
			if (item.m_num <= 0)
				m_purchaseItemList.erase(it);
			return;
		}
		it++;
	}
}

ITEM_INFO	CSession :: GetPurchaseItemInfoByName(const char *itemName)
{
	list<ITEM_INFO>::iterator	it = m_purchaseItemList.begin();
	while (it != m_purchaseItemList.end()) {
		if (it->m_itemName == itemName)
			return	(*it);
		it++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetPurchaseItemNum(const char * itemName)
{
	std::list<ITEM_INFO>::iterator	it = m_purchaseItemList.begin();
	while (it!=m_purchaseItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemName == itemName)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

void	CSession :: CloneMerchantPurchaseItemToMerchantPurchaseItemSaved()
{
	m_merchantPurchaseItemSavedList.erase(m_merchantPurchaseItemSavedList.begin(), m_merchantPurchaseItemSavedList.end());
	std::list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();
	while (it != m_merchantPurchaseItemList.end()) {
		m_merchantPurchaseItemSavedList.push_back(*it);
		++it;
	}
}

ITEM_INFO CSession :: GetMerchantPurchaseItemSaved(int index)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemSavedList.begin();
	while (it != m_merchantPurchaseItemSavedList.end()) {
		if (it->m_itemIndex == index)
			return	*it;
		++it;
	}
	return	ITEM_INFO(0);
}

void	CSession :: ClearMerchantPurchaseItem()
{
	m_merchantPurchaseItemList.erase(m_merchantPurchaseItemList.begin(), m_merchantPurchaseItemList.end());
}

ITEM_INFO	CSession :: GetMerchantPurchaseItemInfoBySequence(int sequence)
{
	list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();
	int		count = 0;
	while (it != m_merchantPurchaseItemList.end())
	{
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetMerchantPurchaseItemTotalPrice()
{
	int		totalPrice = 0;
	std::list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();
	while (it != m_merchantPurchaseItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		totalPrice += itemInfo.m_realPrice * itemInfo.m_num;
		it++;
	}
	return	totalPrice;
}

void	CSession :: AddMerchantPurchaseItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();
	while (it != m_merchantPurchaseItemList.end()) {
		if (it->m_itemName == itemInfo.m_itemName) {
			it->m_num += itemInfo.m_num;
			return;
		}
		it++;
	}
	m_merchantPurchaseItemList.push_back(itemInfo);
}

int		CSession :: GetMerchantPurchaseItemNum(const char * itemName)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();
	while (it!=m_merchantPurchaseItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemName == itemName)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

void	CSession :: DecMerchantPurchaseItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();

	while (it != m_merchantPurchaseItemList.end())
	{
		ITEM_INFO &		item = (*it);
		if (item.m_itemName == itemInfo.m_itemName)
		{
			sAssert(item.m_num >= itemInfo.m_num);
			item.m_num -= itemInfo.m_num;
			if (item.m_num <= 0)
				m_merchantPurchaseItemList.erase(it);
			return;
		}
		it++;
	}
}

ITEM_INFO	CSession :: GetMerchantPurchaseItemInfoByName(const char *itemName)
{
	list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();
	while (it != m_merchantPurchaseItemList.end()) {
		if (it->m_itemName == itemName)
			return	(*it);
		it++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetMerchantPurchaseItemNumByIndex(int index)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantPurchaseItemList.begin();
	while (it != m_merchantPurchaseItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == index)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

void	CSession :: ClearSellItem()
{
	m_sellItemList.erase(m_sellItemList.begin(), m_sellItemList.end());
}

ITEM_INFO	CSession :: GetSellItemInfoBySequence(int sequence)
{
	if (m_sellItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_sellItemList.begin();
	int		count = 0;
	while (it != m_sellItemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

ITEM_INFO CSession :: GetSellItemInfoByIndex(int index)
{
	list<ITEM_INFO>::iterator	it = m_sellItemList.begin();
	while (it != m_sellItemList.end()) {
		if (it->m_itemIndex == index)
			return	(*it);
		it++;
	}
	return	ITEM_INFO(0);
}

BOOL	CSession :: IsSellItemValid()
{
	std::list<ITEM_INFO>::iterator	it = m_sellItemList.begin();
	while (it != m_sellItemList.end()) {
		ITEM_INFO &		sellItem = (*it);
		ITEM_INFO		posessionItem = GetItemInfoByIndex(sellItem.m_itemIndex);
		if (posessionItem.IsValid()) {
			if (posessionItem.m_num < sellItem.m_num)
				return	FALSE;
		} else
			return	FALSE;
		it++;
	}
	return	TRUE;
}

void	CSession :: DecSellItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_sellItemList.begin();
	while (it != m_sellItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			sAssert(item.m_num >= itemInfo.m_num);
			item.m_num -= itemInfo.m_num;
			if (item.m_num <= 0)
				m_sellItemList.erase(it);
			return;
		}
		it++;
	}
}

void	CSession :: AddSellItem(ITEM_INFO &itemInfo)
{
	ITEM_INFO	posessionItem = GetItemInfoByIndex(itemInfo.m_itemIndex);

	if (!posessionItem.IsValid()) return;

	list<ITEM_INFO>::iterator	it = m_sellItemList.begin();
	while (it != m_sellItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			if (posessionItem.m_num < item.m_num+itemInfo.m_num)
				return;
			item.m_num += itemInfo.m_num;
			return;
		}
		it++;
	}
	if (posessionItem.m_num >= itemInfo.m_num)
		m_sellItemList.push_back(itemInfo);
}

int		CSession :: GetSellItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_sellItemList.begin();
	while (it != m_sellItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

bool	CSession :: AddTempSkillItem(SKILL_INFO &skillInfo)
{
	std::list<SKILL_INFO>::iterator		it = m_tempSkillList.begin();
	while (it != m_tempSkillList.end()) {
		SKILL_INFO &	curSkillInfo = (*it);
		if (skillInfo.SKID == curSkillInfo.SKID) {
			curSkillInfo.level		= skillInfo.level;
			curSkillInfo.SKID		= skillInfo.SKID;
			curSkillInfo.spcost		= skillInfo.spcost;
			curSkillInfo.upgradable	= skillInfo.upgradable;
			return	false;
		}
		it++;
	}
	m_tempSkillList.push_back(skillInfo);
	return	true;
}

SKILL_INFO	CSession :: GetTempSkillItemInfoBySkillId(int skillId)
{
	if (m_tempSkillList.size() == 0) {
		return	SKILL_INFO(-1);
	}
	std::list<SKILL_INFO>::iterator	it = m_tempSkillList.begin();
	while (it != m_tempSkillList.end()) {
		SKILL_INFO &	skillInfo = (*it);
		if (skillInfo.SKID == skillId)
			return	(*it);
		++it;
	}
	return	SKILL_INFO(-1);
}
void	CSession :: ClearMerchantHopeToSellItem()
{
	m_merchantHopeToSellItemList.erase(m_merchantHopeToSellItemList.begin(), m_merchantHopeToSellItemList.end());
}

void	CSession :: RefreshMerchantShopMakeWnd()
{
	UIMerchantShopMakeWnd *merchantShopMakeWnd = g_windowMgr.GetMerchantShopMakeWnd();
	if (merchantShopMakeWnd)
		merchantShopMakeWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

bool	CSession ::	DeleteCommaInPrice(char* cPrice)
{
	int pricelength;
	pricelength = strlen(cPrice);

	for (int j=0;j<pricelength;j++)
	{
		if (cPrice[j] == ',')
		{
			for (int k=j;k<pricelength;k++)
			{
				if ( k == pricelength - 1 )
					cPrice[k] = ' ';
				else
					cPrice[k] = cPrice[k+1];
			}
		}

		if ( cPrice[j] >= '0' && cPrice[j] <= '9' )
		{
		}
		else if ( cPrice[j] != ' ' )
		{
			return false;
		}
	}
	return true;
}

ITEM_INFO	CSession :: GetMerchantHopeToSellItemInfoBySequence(int sequence)
{
	if (m_merchantHopeToSellItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_merchantHopeToSellItemList.begin();
	int		count = 0;
	while (it != m_merchantHopeToSellItemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

void	CSession :: SetMerchantHopeToSellItemPrice(int sequence, int price)
{
	int		count = 0;
	std::list<ITEM_INFO>::iterator	it = m_merchantHopeToSellItemList.begin();
	while (it != m_merchantHopeToSellItemList.end()) {
		if (count == sequence) {
			it->m_price = price;
			it->m_realPrice = price;
			return;
		}
		++it;
		++count;
	}

	return;
}

bool	CSession :: AddMerchantHopeToSellItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantHopeToSellItemList.begin();
	while (it != m_merchantHopeToSellItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num += itemInfo.m_num;
			RefreshMerchantShopMakeWnd();
			return false;
		}
		it++;
	}
	m_merchantHopeToSellItemList.push_back(itemInfo);
	RefreshMerchantShopMakeWnd();
	return	true;
}

bool	CSession :: DecMerchantHopeToSellItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator it = m_merchantHopeToSellItemList.begin();
	while (it != m_merchantHopeToSellItemList.end()) {
		ITEM_INFO &item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num -= itemInfo.m_num;
			if (item.m_num <= 0)
				m_merchantHopeToSellItemList.erase(it);
			RefreshMerchantShopMakeWnd();
			return	true;
		}
		it++;
	}
	RefreshMerchantShopMakeWnd();
	return	false;
}

ITEM_INFO	CSession :: GetMerchantHopeToSellItemInfoByIndex(int index)
{
	list<ITEM_INFO>::iterator	it = m_merchantHopeToSellItemList.begin();
	while (it != m_merchantHopeToSellItemList.end()) {
		if (it->m_itemIndex == index)
			return	(*it);
		it++;
	}
	return	ITEM_INFO(0);
}

int			CSession :: GetMerchantHopeToSellItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantHopeToSellItemList.begin();
	while (it != m_merchantHopeToSellItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

void	CSession :: ClearMerchantItem()
{
	m_merchantItemList.erase(m_merchantItemList.begin(), m_merchantItemList.end());
}

void	CSession :: RefreshMerchantItemWnd()
{
	UIMerchantItemWnd *merchantItemWnd = g_windowMgr.GetMerchantItemWnd();
	if (merchantItemWnd)
		merchantItemWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

ITEM_INFO	CSession :: GetMerchantItemInfoBySequence(int sequence)
{
	if (m_merchantItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_merchantItemList.begin();
	int		count = 0;
	while (it != m_merchantItemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

bool	CSession :: AddMerchantItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantItemList.begin();
	while (it != m_merchantItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num += itemInfo.m_num;
			RefreshMerchantItemWnd();
			return	false;
		}
		it++;
	}
	m_merchantItemList.push_back(itemInfo);
	RefreshMerchantItemWnd();
	return	true;
}

bool	CSession :: DecMerchantItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantItemList.begin();
	while (it != m_merchantItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num -= itemInfo.m_num;
			if (item.m_num <= 0)
				m_merchantItemList.erase(it);
			RefreshMerchantItemWnd();
			return	true;
		}
		it++;
	}
	RefreshMerchantItemWnd();
	return	false;
}

ITEM_INFO	CSession :: GetMerchantItemInfoByIndex(int index)
{
	list<ITEM_INFO>::iterator	it = m_merchantItemList.begin();
	while (it != m_merchantItemList.end()) {
		if (it->m_itemIndex == index)
			return	(*it);
		it++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetMerchantItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantItemList.begin();
	while (it != m_merchantItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

void	CSession :: CloneMerchantItemToMerchantMirrorItem()
{
	ClearMerchantMirrorItem();
	std::list<ITEM_INFO>::iterator	it = m_merchantItemList.begin();
	while (it != m_merchantItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		m_merchantMirrorItemList.push_back(itemInfo);
		it++;
	}
}

void	CSession :: ClearMerchantMirrorItem()
{
	m_merchantMirrorItemList.erase(m_merchantMirrorItemList.begin(), m_merchantMirrorItemList.end());
}

void	CSession :: RefreshMerchantMirrorItemWnd()
{
	UIMerchantMirrorItemWnd *merchantMirrorItemWnd = g_windowMgr.GetMerchantMirrorItemWnd();
	if (merchantMirrorItemWnd)
		merchantMirrorItemWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

ITEM_INFO	CSession :: GetMerchantMirrorItemInfoBySequence(int sequence)
{
	if (m_merchantMirrorItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_merchantMirrorItemList.begin();
	int		count = 0;
	while (it != m_merchantMirrorItemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

bool	CSession :: AddMerchantMirrorItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantMirrorItemList.begin();
	while (it != m_merchantMirrorItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num += itemInfo.m_num;
			RefreshMerchantMirrorItemWnd();
			return	false;
		}
		it++;
	}
	m_merchantMirrorItemList.push_back(itemInfo);
	RefreshMerchantMirrorItemWnd();
	return	true;
}

bool	CSession :: DecMerchantMirrorItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantMirrorItemList.begin();
	while (it != m_merchantMirrorItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num -= itemInfo.m_num;
			if (item.m_num <= 0)
				m_merchantMirrorItemList.erase(it);
			RefreshMerchantMirrorItemWnd();
			return	true;
		}
		it++;
	}
	RefreshMerchantMirrorItemWnd();
	return	false;
}

ITEM_INFO	CSession :: GetMerchantMirrorItemInfoByIndex(int index)
{
	list<ITEM_INFO>::iterator	it = m_merchantMirrorItemList.begin();
	while (it != m_merchantMirrorItemList.end()) {
		if (it->m_itemIndex == index)
			return	*it;
		it++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetMerchantMirrorItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantMirrorItemList.begin();
	while (it != m_merchantMirrorItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

void	CSession :: ClearStoreItem()
{
	m_storeItemList.erase(m_storeItemList.begin(), m_storeItemList.end());
}

void	CSession :: RefreshStoreItemWnd()
{
	UIItemStoreWnd *itemStoreWnd = g_windowMgr.GetItemStoreWnd();
	if (itemStoreWnd)
		itemStoreWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

ITEM_INFO	CSession :: GetStoreItemInfoBySequence(int sequence)
{
	if (m_storeItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_storeItemList.begin();
	int		count = 0;
	while (it != m_storeItemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

bool	CSession :: AddStoreItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_storeItemList.begin();
	while (it != m_storeItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num += itemInfo.m_num;
			RefreshStoreItemWnd();
			return	false;
		}
		it++;
	}
	m_storeItemList.push_back(itemInfo);
	RefreshStoreItemWnd();
	return	true;
}

bool	CSession :: DecStoreItem(ITEM_INFO &itemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_storeItemList.begin();
	while (it != m_storeItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == itemInfo.m_itemIndex) {
			item.m_num -= itemInfo.m_num;
			if (item.m_num <= 0)
				m_storeItemList.erase(it);
			RefreshStoreItemWnd();
			return	true;
		}
		it++;
	}
	RefreshStoreItemWnd();
	return	false;
}

ITEM_INFO	CSession :: GetStoreItemInfoByIndex(int index)
{
	list<ITEM_INFO>::iterator	it = m_storeItemList.begin();
	while (it != m_storeItemList.end()) {
		if (it->m_itemIndex == index)
			return	*it;
		it++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetStoreItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_storeItemList.begin();
	while (it != m_storeItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex)
			return	itemInfo.m_num;
		it++;
	}
	return	0;
}

void	CSession :: ClearShopItem()
{
	m_shopItemList.erase(m_shopItemList.begin(), m_shopItemList.end());
}

ITEM_INFO	CSession :: GetShopItemInfoBySequence(int sequence)
{
	if (m_shopItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_shopItemList.begin();
	int		count = 0;
	while (it != m_shopItemList.end()) {
		if (count == sequence)
			return	(*it);
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetShopItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_shopItemList.begin();
	while (it != m_shopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex) {
			return	itemInfo.m_num;
		}
		it++;
	}
	return	0;
}

void	CSession :: DecShopItem(ITEM_INFO &decItemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_shopItemList.begin();
	while (it != m_shopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == decItemInfo.m_itemIndex) {
			itemInfo.m_num -= decItemInfo.m_num;
			sAssert(itemInfo.m_num >= 0);
			if (itemInfo.m_num <= 0)
				m_shopItemList.erase(it);
			return;
		}
		it++;
	}
}

void	CSession :: AddShopItemFromSell(ITEM_INFO &addItemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_shopItemList.begin();
	while (it != m_shopItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == addItemInfo.m_itemIndex) {
			item.m_num += addItemInfo.m_num;
			return;
		}
		it++;
	}
	m_shopItemList.push_back(addItemInfo);
}

void	CSession :: AddShopItem(SHOP_ITEM &shop_item, BOOL isSellMode)
{
	if (isSellMode) {
		ITEM_INFO	possItem = GetItemInfoByIndex(shop_item.index);
		sAssert(possItem.IsValid());
		if (possItem.IsValid()) {
			ITEM_INFO	itemInfo = possItem;
			itemInfo = shop_item;
			itemInfo.SetItemId(atoi(itemInfo.m_itemName.c_str()));
			m_shopItemList.push_back(itemInfo);
		}
	} else {
		ITEM_INFO	itemInfo;
		itemInfo = shop_item;
		itemInfo.m_refiningLevel = 0;
		itemInfo.m_isDamaged = 0;
		itemInfo.SetItemId(atoi(itemInfo.m_itemName.c_str()));
		m_shopItemList.push_back(itemInfo);
	}
}

bool	CSession :: IsExistInShopItemList(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_shopItemList.begin();
	while (it != m_shopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex)
			return	true;
		it++;
	}
	return	false;
}

int		CSession :: GetNumMerchantMyShopItem()
{
	return	m_merchantMyShopItemList.size();
}

void	CSession :: ClearMerchantMyShopItem()
{
	m_merchantMyShopItemList.erase(m_merchantMyShopItemList.begin(), m_merchantMyShopItemList.end());
}

ITEM_INFO	CSession :: GetMerchantMyShopItemInfoBySequence(int sequence)
{
	if (m_merchantMyShopItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_merchantMyShopItemList.begin();
	int		count = 0;
	while (it != m_merchantMyShopItemList.end()) {
		if (count == sequence)
			return	*it;
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetMerchantMyShopItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantMyShopItemList.begin();
	while (it!=m_merchantMyShopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex) {
			return	itemInfo.m_num;
		}
		it++;
	}
	return	0;
}

void	CSession :: DecMerchantMyShopItem(ITEM_INFO &decItemInfo)
{
	list<ITEM_INFO>::iterator	it = m_merchantMyShopItemList.begin();
	while (it != m_merchantMyShopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == decItemInfo.m_itemIndex) {
			decItemInfo.m_itemName = itemInfo.m_itemName;
			itemInfo.m_num -= decItemInfo.m_num;
			sAssert(itemInfo.m_num >= 0);
			if (itemInfo.m_num <= 0)
				m_merchantMyShopItemList.erase(it);
			return;
		}
		it++;
	}
}

void	CSession :: AddMerchantMyShopItem(ITEM_INFO &addItemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantMyShopItemList.begin();
	while (it != m_merchantMyShopItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == addItemInfo.m_itemIndex) {
			item.m_num += addItemInfo.m_num;
			return;
		}
		it++;
	}
	m_merchantMyShopItemList.push_back(addItemInfo);
}

bool	CSession :: IsExistInMerchantMyShopItemList(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantMyShopItemList.begin();
	while (it != m_merchantMyShopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex)
			return	true;
		it++;
	}
	return	false;
}

void	CSession :: ClearMerchantShopItem()
{
	m_merchantShopItemList.erase(m_merchantShopItemList.begin(), m_merchantShopItemList.end());
}

ITEM_INFO	CSession :: GetMerchantShopItemInfoBySequence(int sequence)
{
	if (m_merchantShopItemList.size() == 0)
		return	ITEM_INFO(0);
	list<ITEM_INFO>::iterator	it = m_merchantShopItemList.begin();
	int		count = 0;
	while (it != m_merchantShopItemList.end()) {
		if (count == sequence)
			return	*it;
		it++;
		count++;
	}
	return	ITEM_INFO(0);
}

int		CSession :: GetMerchantShopItemNum(int itemIndex)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantShopItemList.begin();
	while (it != m_merchantShopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == itemIndex) {
			return	itemInfo.m_num;
		}
		it++;
	}
	return	0;
}

void	CSession :: DecMerchantShopItem(ITEM_INFO &decItemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantShopItemList.begin();
	while (it != m_merchantShopItemList.end()) {
		ITEM_INFO &		itemInfo = (*it);
		if (itemInfo.m_itemIndex == decItemInfo.m_itemIndex) {
			itemInfo.m_num -= decItemInfo.m_num;
			sAssert(itemInfo.m_num >= 0);
			if (itemInfo.m_num <= 0)
				m_merchantShopItemList.erase(it);
			return;
		}
		it++;
	}
}

void	CSession :: AddMerchantShopItem(ITEM_INFO &addItemInfo)
{
	std::list<ITEM_INFO>::iterator	it = m_merchantShopItemList.begin();
	while (it != m_merchantShopItemList.end()) {
		ITEM_INFO &		item = (*it);
		if (item.m_itemIndex == addItemInfo.m_itemIndex) {
			item.m_num += addItemInfo.m_num;
			return;
		}
		it++;
	}
	m_merchantShopItemList.push_back(addItemInfo);
}

int		CSession :: GetNumParty()
{
	return	m_partyList.size();
}

void	CSession :: ClearParty()
{
	m_partyList.erase(m_partyList.begin(), m_partyList.end());
	m_amIPartyMaster = false;
	m_partyName = "";
}

void	CSession :: AddMemberToParty(FRIEND_INFO &info)
{
	if (info.mapName.size() > 4) {
		info.mapName.resize(info.mapName.size() - 3);
		info.mapName += "rsw";
	}

	int		count = 0;
	list<FRIEND_INFO>::iterator		it = m_partyList.begin();
	while (it != m_partyList.end()) {
		FRIEND_INFO &	fInfo = (*it);
		if (fInfo.characterName == info.characterName) {
			info.color = GetPartyColorBySeq(count);
			fInfo = info;
			return;
		}
		++it;
		++count;
	}
	info.color = GetPartyColorBySeq(count);
	m_partyList.push_back(info);
}

DWORD	CSession :: GetMemberAidFromParty(const char *characterName)
{
	std::list<FRIEND_INFO>::iterator		it = m_partyList.begin();
	while (it != m_partyList.end()) {
		if (it->characterName == characterName) {
			return	it->AID;
		}
		++it;
	}
	return	0;
}

void	CSession :: DeleteMemberFromParty(const char *characterName)
{
	std::list<FRIEND_INFO>::iterator it = m_partyList.begin();
	while (it != m_partyList.end()) {
		if (it->characterName == characterName) {
			m_partyList.erase(it);
			break;
		}
		++it;
	}
}

void	CSession :: ChangeRoleFromParty(DWORD aid, int role)
{
	std::list<FRIEND_INFO>::iterator it = m_partyList.begin();
	while (it != m_partyList.end()) {
		if (it->AID == aid) {
			it->role = role;
			return;
		}
		++it;
	}
}

void	CSession :: RefreshPartyUI()
{
	UIMessengerGroupWnd *messengerGroupWnd = g_windowMgr.GetMessengerGroupWnd();
	if (messengerGroupWnd)
		messengerGroupWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

FRIEND_INFO		CSession :: GetPartyInfoBySequence(int sequence)
{
	list<FRIEND_INFO>::iterator		it = m_partyList.begin();
	int		count = 0;
	while (it != m_partyList.end()) {
		if (count == sequence) {
			return	*it;
		}
		++count;
		++it;
	}
	return	FRIEND_INFO(0);
}

FRIEND_INFO		CSession :: GetPartyInfoByName(const char * name)
{
	list<FRIEND_INFO>::iterator it = m_partyList.begin();
	int		count = 0;
	while (it != m_partyList.end()) {
		if (it->characterName == name) {
			return	*it;
		}
		++count;
		++it;
	}
	return	FRIEND_INFO(0);
}

FRIEND_INFO		CSession :: GetPartyInfoByAid(DWORD aid)
{
	list<FRIEND_INFO>::iterator		it = m_partyList.begin();
	while (it != m_partyList.end()) {
		if (it->AID == aid) {
			return	*it;
		}
		++it;
	}
	return	FRIEND_INFO(0);
}

void	CSession :: ClearFriend()
{
	m_friendList.erase(m_friendList.begin(), m_friendList.end());

}

int		CSession :: GetNumFriend()
{
	return	m_friendList.size();
}

FRIEND_INFO		CSession :: GetFriendInfoBySequence(int sequence)
{
	list<FRIEND_INFO>::iterator		it = m_friendList.begin();
	int		count = 0;
	while (it != m_friendList.end()) {
		if (count == sequence) {
			return	*it;
		}
		++count;
		++it;
	}
	return	FRIEND_INFO(0);
}

BOOL	CSession :: IsFriendName(const char * cName)
{
	std::list<FRIEND_INFO>::iterator it = m_friendList.begin();
	while (it != m_friendList.end()) {
		if (it->characterName == cName) {
			return TRUE;
			break;
		}
		++it;
	}

	return FALSE;
}

#if 1
void	CSession :: HiFriends(int val1)
{
	string temp = "(Hi) ";
	if(val1 == NULL || val1 == 0)
		temp += "*^_^*";
	else
		temp += (char *)val1;

	list<FRIEND_INFO>::iterator it = m_friendList.begin();
	while (it != m_friendList.end())
	{
		g_windowMgr.SetChatMsg(temp.c_str());
		g_modeMgr.GetCurMode()->SendMsg(MM_WHISPERMSG, (int)it->characterName.c_str(), 0, 0);
		g_MessageHi++;

		if (g_MessageHi > MAX_FRIENDS_LIST)
			g_MessageHi = MAX_FRIENDS_LIST;

		++it;
	}
}
#endif

BOOL	CSession :: IsCheatName(const char * cName)
{
	std::list<FRIEND_INFO>::iterator it = m_friendList.begin();
	while (it != m_friendList.end())
	{
		if (CompareTwoNames(cName, it->characterName.c_str()) == 0)
		{
			return TRUE;
		}
		++it;
	}

	return FALSE;
}

BOOL	CSession :: CompareTwoNames(const char * cName1,const char * cName2)
{
	char checkC[NAME_LENGTH];
	char checkC_list[NAME_LENGTH];
	short L_len,C_len,minlen,count,diff,i,ii,jj;
	char PASS;

	sprintf(checkC,"%s",cName1);
	C_len = strlen(checkC);

	sprintf(checkC_list,"%s",cName2);
	L_len = strlen(checkC_list);

	if (C_len < L_len)
	{
		minlen = C_len;
		diff = L_len - C_len;
	}
	else
	{
		minlen = L_len;
		diff = C_len - L_len;
	}

	if ( (C_len >= L_len+2) && (C_len <= L_len+4) )
	{
		count = 0;
		jj=0;
		ii=2;
		for (i=0;i<minlen;i++)
		{
			if (checkC[ii] == 'I' || checkC[ii] == 'l')
			{
				if (checkC_list[jj] == 'I' || checkC_list[jj] == 'l')
					count++;
			}
			else if (checkC[ii] == checkC_list[jj])
			{
				count++;
			}

			ii++;
			jj++;
		}

		if ( count == minlen )
			return 0;
	}

	if ( (C_len >= L_len+1) && (C_len <= L_len+4) )
	{
		count = 0;
		jj=0;
		ii=1;
		for (i=0;i<minlen;i++)
		{
			if (checkC[ii] == 'I' || checkC[ii] == 'l')
			{
				if (checkC_list[jj] == 'I' || checkC_list[jj] == 'l')
					count++;
			}
			else if (checkC[ii] == checkC_list[jj])
			{
				count++;
			}

			ii++;
			jj++;
		}

		if ( count == minlen )
			return 0;
	}

	if ( diff <=2 )
	{
		count = 0;
		for (i=0;i<minlen;i++)
		{
			if (checkC[i] == 'I' || checkC[i] == 'l')
			{
				if (checkC_list[i] == 'I' || checkC_list[i] == 'l')
					count++;
			}
			else if (checkC[i] == checkC_list[i])
				count++;
		}

		if ( (count >= minlen-2) )
			return 0;


		{
			PASS = 1;
			count = 0;
			jj=0;
			ii=0;
			for (i=0;i<minlen;i++)
			{
				if (checkC[ii] == 'I' || checkC[ii] == 'l')
				{
					if (checkC_list[jj] == 'I' || checkC_list[jj] == 'l')
						count++;
				}
				else if (checkC[ii] == checkC_list[jj])
				{
					count++;
				}

				if (count != ii+1)
				{
					if (PASS == 1)
					{
						PASS=2;
						jj--;
					}
				}

				ii++;
				jj++;
			}

			if ( count == minlen-1 )
				return 0;
		}
	}

	return 1;
}

BOOL	CSession :: IsGuildMember(const char * cName)
{
	list<GUILD_MEMBER_INFORMATION> m_memberInfoList;
	m_memberInfoList = g_guildInfo.GetMemberInfoList();
	list<GUILD_MEMBER_INFORMATION>::iterator it = m_memberInfoList.begin();
	while (it != m_memberInfoList.end())
	{
		if (it->CharName == cName)
		{
			return TRUE;
			break;
		}
		++it;
	}

	if (strcmp(g_guildInfo.m_masterName.c_str(), cName) == 0)
		return TRUE;

	return FALSE;
}

BOOL	CSession :: IsCheatGuildName(const char * cName)
{
	list<GUILD_MEMBER_INFORMATION> m_memberInfoList;
	m_memberInfoList = g_guildInfo.GetMemberInfoList();
	list<GUILD_MEMBER_INFORMATION>::iterator it = m_memberInfoList.begin();
	while (it != m_memberInfoList.end())
	{
		if (CompareTwoNames(cName,it->CharName.c_str()) == 0)
		{
			return TRUE;
			break;
		}
		++it;
	}

	return FALSE;
}

BOOL	CSession :: DeleteFriendFromList(DWORD gid)
{
	std::list<FRIEND_INFO>::iterator it = m_friendList.begin();
	while (it != m_friendList.end()) {
		if (it->GID == gid) {
			m_friendList.erase(it);
			return TRUE;
		}
		++it;
	}

	return FALSE;
}

void	CSession :: AddFriendToList(FRIEND_INFO &info)
{
	int count = 0 ;
	list<FRIEND_INFO>::iterator		it = m_friendList.begin ( ) ;

	while (it != m_friendList.end()) {
		FRIEND_INFO &	fInfo = (*it);
		if (fInfo.characterName == info.characterName) {
			info.color = GetPartyColorBySeq(count);
			fInfo = info;
			return;
		}
		++it;
		++count;
	}
	info.color = GetPartyColorBySeq(count);
	m_friendList.push_back(info);
}

BOOL	CSession :: SetFriendState(DWORD aid, DWORD gid, bool state)
{
	list<FRIEND_INFO>::iterator		it = m_friendList.begin();

	while (it != m_friendList.end())
	{
		if ( ( it->AID == aid ) && ( it->GID == gid ) )
		{
			it->state = state;

			if (g_session.m_isLogInOut == TRUE)
			{
				char msg[256];

				if (GROUPMEMBER_STATE_CONNECTED == it->state)
					sprintf(msg, MsgStr(MSI_LOGIN_FRIEND), it->characterName.c_str());
				else
					sprintf(msg, MsgStr(MSI_LOGOUT_FRIEND), it->characterName.c_str());

				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 0, 0);
			}

			return true ;
		}

		++it;
	}

	return	false ;
}

void	CSession :: RefreshFriendUI()
{
	UIMessengerGroupWnd *messengerGroupWnd = g_windowMgr.GetMessengerGroupWnd();
	if (messengerGroupWnd)
		messengerGroupWnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
}

bool	CSession :: IsVisibleNPC(JOBTYPE jobType)
{
	switch (jobType) {
		case	JT_WARPNPC:
		case	JT_EFFECTLAUNCHER:
		case	JT_HIDDEN_NPC:
			return	false;
			break;
	}
	return	true;
}

void	CSession :: SetPlayerInfo(int job, int sex, char *name, int speed)
{
	m_job = job;
	if (m_job == JT_CHICKEN)
		m_job = JT_KNIGHT;
	else if (m_job == JT_CHICKEN2)
		m_job = JT_CRUSADER;
	else if (m_job == JT_CHICKEN_H)
		m_job = JT_KNIGHT_H;
	else if (m_job == JT_CHICKEN_B)
		m_job = JT_KNIGHT_B;
	else if (m_job == JT_STAR2)
		m_job = JT_STAR;
	else if (m_job == JT_CHICKEN2_H)
		m_job = JT_CRUSADER_H;
	else if (m_job == JT_CHICKEN2_B)
		m_job = JT_CRUSADER_B;
	SetSex(sex);
	m_speed = speed;
	strcpy(m_cName, name);
}

void	CSession :: SetPlayerPosDir(int x, int y, int dir)
{
	m_dir = dir;
	m_posX = x;
	m_posY = y;
}

void	CSession :: UpdateServerTime(DWORD startTime)
{
	DWORD	serverTime = timeGetTime();
	serverTime -= m_diffTime;
	serverTime -= 72;
	DWORD	newDiff;

	if (serverTime + 72 < startTime) {
		newDiff = timeGetTime() - startTime;
	} else {
		if (serverTime + 72 <= startTime + 144) {
			m_numLatePacket = 0;
			return;
		}
		++m_numLatePacket;
		if (m_numLatePacket < 4)
			return;
		newDiff = timeGetTime() + 72 - serverTime;
	}
	m_diffTime = newDiff;
	m_numLatePacket = 0;
}

void	CSession :: SetServerTime(DWORD startTime)
{
	DWORD now = timeGetTime();
	m_numLatePacket = 0;
	m_diffTime = now - startTime;
}

DWORD	CSession :: GetServerTime()
{
	return timeGetTime() - m_diffTime - 72;
}

void	CSession :: InvalidateJob()
{
	if (g_windowMgr.GetBasicInfoWnd())
		g_windowMgr.GetBasicInfoWnd()->NewHeight(g_windowMgr.GetBasicInfoWnd()->GetHeight());
}

void	CSession :: InvalidateBasicWnd()
{
	if (!g_modeMgr.GetGameMode())
		return;
	if (g_windowMgr.GetBasicInfoWnd()) {
		g_windowMgr.GetBasicInfoWnd()->Invalidate();
		g_windowMgr.GetBasicInfoWnd()->InvalidateChildren();
	}
	g_modeMgr.GetGameMode()->InvalidatePlayerGage();
}

void CSession::InvalidateUI()
{
	g_windowMgr.InvalidateUpdateNeededUI();
}

void	CSession :: InvalidateHomunInfoWnd()
{
	if (!g_modeMgr.GetGameMode())
		return;
	if (g_windowMgr.GetHomunInfoWnd()) {
		g_windowMgr.GetHomunInfoWnd()->Invalidate();
		g_windowMgr.GetHomunInfoWnd()->InvalidateChildren();
	}
}

void	CSession :: InitHomunTargetInfo()
{
	m_moveHomunEnemy = false ;
	m_selectHomunEnemy = false ;
	m_attackHomunEnemy = false ;
	m_homunEnemyGID = 0 ;
	m_homunEnemyX = 0 ;
	m_homunEnemyY = 0 ;
}

void	CSession :: InvalidateMerInfoWnd()
{
	if (!g_modeMgr.GetGameMode())
		return;
	if (g_windowMgr.GetMerInfoWnd()) {
		g_windowMgr.GetMerInfoWnd()->Invalidate();
		g_windowMgr.GetMerInfoWnd()->InvalidateChildren();
	}
}

void	CSession :: InitMerTargetInfo()
{
	m_moveMerEnemy = false ;
	m_selectMerEnemy = false ;
	m_attackMerEnemy = false ;
	m_merEnemyGID = 0 ;
	m_merEnemyX = 0 ;
	m_merEnemyY = 0 ;
}

bool	CSession :: IsProperWeapon(int job, int weapon)
{
	switch (job)
	{
	case	JT_GANGSI:
	case	JT_DEATHKNIGHT:
	case	JT_COLLECTOR:
		return true;
		break;
	case	JT_GUNSLINGER:
		switch (weapon)
		{
			case	WPCLASS_GUN_HANDGUN:
			case	WPCLASS_GUN_RIFLE:
			case	WPCLASS_GUN_GATLING:
			case	WPCLASS_GUN_SHOTGUN:
			case	WPCLASS_GUN_GRANADE:
				return	true;
		}
		break;
	case	JT_NINJA:
		switch (weapon)
		{
			case	WPCLASS_SYURIKEN:
			return	true;
		}
		break;
	case	JT_STAR:
	case	JT_STAR2:
		switch (weapon)
		{
		case	WEAPONTYPE_BOOK:
		return	true;
		}
		break;
	case	JT_LINKER:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_ROD:
			case	WPCLASS_TWOHANDROD:
			return	true;
		}
		break;
	case	JT_NOVICE:
	case	JT_NOVICE_H:
	case	JT_SUPERNOVICE:
	case	JT_NOVICE_B:
	case	JT_SUPERNOVICE_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_SWORD:
			case	WEAPONTYPE_AXE:
			case	WEAPONTYPE_MACE:
			case	WEAPONTYPE_ROD:
			case	WPCLASS_TWOHANDROD:
			return	true;
		}
		break;
	case	JT_SWORDMAN:
	case	JT_SWORDMAN_H:
	case	JT_SWORDMAN_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_SWORD:
			case	WEAPONTYPE_TWOHANDSWORD:
			case	WEAPONTYPE_SPEAR:
			case	WEAPONTYPE_TWOHANDSPEAR:
			case	WEAPONTYPE_AXE:
			case	WEAPONTYPE_TWOHANDAXE:
			case	WEAPONTYPE_MACE:
			case	WEAPONTYPE_TWOHANDMACE:
			return	true;
		}
		break;
	case	JT_MAGICIAN:
	case	JT_MAGICIAN_H:
	case	JT_MAGICIAN_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_ROD:
			case	WPCLASS_TWOHANDROD:
			return	true;
		}
		break;
	case	JT_ARCHER:
	case	JT_ARCHER_H:
	case	JT_ARCHER_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_BOW:
			return	true;
		}
		break;
	case	JT_ACOLYTE:
	case	JT_ACOLYTE_H:
	case	JT_ACOLYTE_B:
		switch (weapon)
		{
			case	WEAPONTYPE_MACE:
			case	WEAPONTYPE_ROD:
			case	WPCLASS_TWOHANDROD:
			return	true;
		}
		break;
	case	JT_MERCHANT:
	case	JT_MERCHANT_H:
	case	JT_MERCHANT_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_SWORD:
			case	WEAPONTYPE_AXE:
			case	WEAPONTYPE_TWOHANDAXE:
			case	WEAPONTYPE_MACE:
			return	true;
		}
		break;
	case	JT_THIEF:
	case	JT_THIEF_H:
	case	JT_THIEF_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_SWORD:
			case	WEAPONTYPE_BOW:
			return	true;
		}
		break;
	case	JT_KNIGHT:
	case	JT_CHICKEN:
	case	JT_KNIGHT_H:
	case	JT_CHICKEN_H:
	case	JT_KNIGHT_B:
	case	JT_CHICKEN_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_SWORD:
			case	WEAPONTYPE_TWOHANDSWORD:
			case	WEAPONTYPE_SPEAR:
			case	WEAPONTYPE_TWOHANDSPEAR:
			case	WEAPONTYPE_AXE:
			case	WEAPONTYPE_TWOHANDAXE:
			case	WEAPONTYPE_MACE:
			case	WEAPONTYPE_TWOHANDMACE:
		return	true;
		}
		break;
	case	JT_PRIEST:
	case	JT_PRIEST_H:
	case	JT_PRIEST_B:
		switch (weapon)
		{
			case	WEAPONTYPE_MACE:
			case	WEAPONTYPE_ROD:
			case	WPCLASS_TWOHANDROD:
			case	WEAPONTYPE_BOOK:
			return	true;
		}
		break;
	case	JT_BLACKSMITH:
	case	JT_BLACKSMITH_H:
	case	JT_BLACKSMITH_B:
		switch (weapon)
		{
		case	WEAPONTYPE_SHORTSWORD:
		case	WEAPONTYPE_SWORD:
		case	WEAPONTYPE_AXE:
		case	WEAPONTYPE_TWOHANDAXE:
		case	WEAPONTYPE_MACE:
		return	true;
		}
		break;
	case	JT_HUNTER:
	case	JT_HUNTER_H:
	case	JT_HUNTER_B:
		switch (weapon)
		{
		case	WEAPONTYPE_SHORTSWORD:
		case	WEAPONTYPE_BOW:
		return	true;
		}
		break;
	case	JT_ASSASSIN:
	case	JT_ASSASSIN_H:
	case	JT_ASSASSIN_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_SWORD:
			case	WEAPONTYPE_AXE:
			case	WEAPONTYPE_CATARRH:
			case	WEAPONTYPE_SHORTSWORD_SHORTSWORD:
			case	WEAPONTYPE_SHORTSWORD_SWORD:
			case	WEAPONTYPE_SHORTSWORD_AXE:
			case	WEAPONTYPE_SWORD_SWORD:
			case	WEAPONTYPE_SWORD_AXE:
			case	WEAPONTYPE_AXE_AXE:
			return	true;
		}
		break;
	case	JT_WIZARD:
	case	JT_WIZARD_H:
	case	JT_WIZARD_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_ROD:
			case	WPCLASS_TWOHANDROD:
			return	true;
		}
		break;
	case	JT_CHICKEN2:
	case	JT_CRUSADER:
	case	JT_CHICKEN2_H:
	case	JT_CRUSADER_H:
	case	JT_CRUSADER_B:
	case	JT_CHICKEN2_B:
		switch (weapon)
		{
		case	WEAPONTYPE_SHORTSWORD:
		case	WEAPONTYPE_SWORD:
		case	WEAPONTYPE_TWOHANDSWORD:
		case	WEAPONTYPE_AXE:
		case	WEAPONTYPE_TWOHANDAXE:
		case	WEAPONTYPE_TWOHANDSPEAR:
		case	WEAPONTYPE_SPEAR:
		case	WEAPONTYPE_MACE:
		return true;
		}
		break;
	case	JT_ROGUE:
	case	JT_ROGUE_H:
	case	JT_ROGUE_B:
		switch (weapon)
		{
		case	WEAPONTYPE_SHORTSWORD:
		case	WEAPONTYPE_SWORD:
		case	WEAPONTYPE_BOW:
		return true;
		}
		break;
	case	JT_BARD:
	case	JT_BARD_H:
	case	JT_BARD_B:
		switch (weapon)
		{
		case	WEAPONTYPE_INSTRUMENT:
		case	WEAPONTYPE_SHORTSWORD:
		case	WEAPONTYPE_BOW:
		return true;
		}
		break;
	case	JT_ALCHEMIST:
	case	JT_ALCHEMIST_H:
	case	JT_ALCHEMIST_B:
		switch (weapon)
		{
			case	WEAPONTYPE_SHORTSWORD:
			case	WEAPONTYPE_SWORD:
			case	WEAPONTYPE_AXE:
			case	WEAPONTYPE_TWOHANDAXE:
			case	WEAPONTYPE_MACE:
			case	WEAPONTYPE_ROD:
			case	WPCLASS_TWOHANDROD:
		return true;
		}
		break;
	case	JT_MONK:
	case	JT_MONK_H:
	case	JT_MONK_B:
		switch (weapon)
		{
		case	WEAPONTYPE_ROD:
		case	WPCLASS_TWOHANDROD:
		case	WEAPONTYPE_MACE:
		case	WEAPONTYPE_KNUKLE:
		return true;
		}
		break;
	case	JT_SAGE:
	case	JT_SAGE_H:
	case	JT_SAGE_B:
		switch (weapon)
		{
		case	WEAPONTYPE_ROD:
		case	WEAPONTYPE_BOOK:
		case	WPCLASS_TWOHANDROD:
		case	WEAPONTYPE_SHORTSWORD:
		case	WEAPONTYPE_SWORD:
		return true;
		case	JT_DANCER:
		case	JT_DANCER_H:
		case	JT_DANCER_B:
		switch (weapon)
		{
		case	WEAPONTYPE_WHIP:
		case	WEAPONTYPE_SHORTSWORD:
		case	WEAPONTYPE_BOW:
		return true;
		}
		break;
		}
		break;
	default:
		sAssert(weapon == (int)"Unknown Job <- weapon");
		switch (weapon) {
	case	WEAPONTYPE_SHORTSWORD:
		return	true;
		break;
		}
		break;
	}
	return	false;
}

void	CSession :: CorrectHead(int &head, int job)
{
	if (HEADTYPE_SAMEASJOB == head) {
		switch (job) {
			case	JT_NOVICE:
			case	JT_SUPERNOVICE:
				head = HEADTYPE_NOVICE;
				break;
			case	JT_SWORDMAN:
				head = HEADTYPE_SWORDMAN;
				break;
			case	JT_ACOLYTE:
				head = HEADTYPE_ACOLYTE;
				break;
			case	JT_MAGICIAN:
				head = HEADTYPE_MAGICIAN;
				break;
			case	JT_ARCHER:
				head = HEADTYPE_ARCHER;
				break;
			case	JT_MERCHANT:
				head = HEADTYPE_MERCHANT;
				break;
			case	JT_THIEF:
				head = HEADTYPE_THIEF;
				break;
			default:
				head = HEADTYPE_NOVICE;
				break;
		}
	}
}

int		CSession :: GetHeadJob(int head, int job)
{
	CorrectHead(head, job);
	int		jobOfHead = job;
	switch (head) {
		case	HEADTYPE_NOVICE:
			jobOfHead = JT_NOVICE;
			break;
		case	HEADTYPE_SWORDMAN:
			jobOfHead = JT_SWORDMAN;
			break;
		case	HEADTYPE_MAGICIAN:
			jobOfHead = JT_MAGICIAN;
			break;
		case	HEADTYPE_ARCHER:
			jobOfHead = JT_ARCHER;
			break;
		case	HEADTYPE_ACOLYTE:
			jobOfHead = JT_ACOLYTE;
			break;
		case	HEADTYPE_MERCHANT:
			jobOfHead = JT_MERCHANT;
			break;
		case	HEADTYPE_THIEF:
			jobOfHead = JT_THIEF;
			break;
		default:
			jobOfHead = JT_NOVICE;
			break;
	}
	return	jobOfHead;
}

ITEM_INFO	CSession :: GetEquipedItems(int wearLocation)
{


	if (wearLocation == 15) return	GetItemInfoByIndex(m_equipArrowIndex);

	if (wearLocation<0 || wearLocation>MAX_EQUIP_SLOT-1) return	ITEM_INFO(0);

	return	m_equipedItems[wearLocation];
}

void	CSession :: SetEquipArrowIndwx(int value)
{
	m_equipArrowIndex = value;
	g_windowMgr.InvalidateUpdateNeededUI();
}


DWORD	CSession :: GetPartyColorBySeq(int seq)
{
	switch (seq) {
		case	0:
			return	0xff0a3278;
		case	1:
			return	0xff007878;
		case	2:
			return	0xff000000;
		case	3:
			return	0xff780000;
		case	4:
			return	0xff007800;
		case	5:
			return	0xff787800;
		case	6:
			return	0xff780078;
		case	7:
			return	0xff783200;
		case	8:
			return	0xff787878;
		case	9:
			return	0xff000078;
		case	10:
			return	0xff003232;
		case	11:
			return	0xff320000;
		case	12:
			return	0xff003200;
		case	13:
			return	0xff323200;
		case	14:
			return	0xff320032;
		case	15:
			return	0xff780000;
		default:
			return	0xff323232;
	}
}

char *	CSession :: GetJobActName(int job, int sex, char *buf)
{
	if (job>0xf6e)
		sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s.act", m_newPcSexNameTable[sex], m_newPcJobNameTable[job-0xf6e], m_pcSexNameTable[sex]);
	else
		sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s.act", m_newPcSexNameTable[sex], m_newPcJobNameTable[job], m_pcSexNameTable[sex]);
	return	buf;
}

char *	CSession :: GetJobSprName(int job, int sex, char *buf)
{
	if (job>0xf6e)
		sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s.spr", m_newPcSexNameTable[sex], m_newPcJobNameTable[job-0xf6e], m_pcSexNameTable[sex]);
	else
		sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s.spr", m_newPcSexNameTable[sex], m_newPcJobNameTable[job], m_pcSexNameTable[sex]);
	return	buf;
}

char *	CSession :: GetHeadActName(int job, int &head, int sex, char *buf)
{
	if (head < 0 || head > HEADTYPE_ADD18) head = HEADTYPE_ADD6;
	if (sex != 0) sex = 1;

	switch (sex) {
		case SEX_MALE:
			sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xd3\xb8\xae\xc5\xeb\\%s\\%s%s.act", m_newPcSexNameTable[sex], m_newPcHeadNameTable_M[head], m_pcSexNameTable[sex]);
			break;
		default:
			sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xd3\xb8\xae\xc5\xeb\\%s\\%s%s.act", m_newPcSexNameTable[sex], m_newPcHeadNameTable_F[head], m_pcSexNameTable[sex]);
			break;
	}
	return buf;
}

char *	CSession :: GetHeadSprName(int job, int &head, int sex, char *buf)
{
	if (head < 0 || head > HEADTYPE_ADD18) head = HEADTYPE_ADD6;
	if (sex != 0) sex = 1;

	switch (sex) {
		case SEX_MALE:
			sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xd3\xb8\xae\xc5\xeb\\%s\\%s%s.spr", m_newPcSexNameTable[sex], m_newPcHeadNameTable_M[head], m_pcSexNameTable[sex]);
			break;
		default:
			sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xd3\xb8\xae\xc5\xeb\\%s\\%s%s.spr", m_newPcSexNameTable[sex], m_newPcHeadNameTable_F[head], m_pcSexNameTable[sex]);
			break;
	}
	return buf;
}

char *	CSession :: GetJobDyeSprName(int job, int sex, char *buf)
{
	sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s", m_newPcSexNameTable[sex], "\xbc\xba\xc1\xf7\xc0\xda", "_\xbf\xa9");
	strcat(buf, "_dye");
	strcat(buf, ".spr");
	return	buf;
}

char *	CSession :: GetJobDyeActName(int job, int sex, char *buf)
{
	if (job>0xf6e)
		sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s", m_newPcSexNameTable[sex], m_newPcJobNameTable[job-0xf6e], m_pcSexNameTable[sex]);
	else
		sprintf(buf, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s", m_newPcSexNameTable[sex], m_newPcJobNameTable[job], m_pcSexNameTable[sex]);
	return	buf;
}

char *	CSession :: GetImfName(int job, int head, int sex, char *buf)
{
	if (job > 0xf6e)
		sprintf(buf, "%s%s.imf", m_newPcJobImfNameTable[job-0xf6e], m_pcSexNameTable[sex]);
	else
		sprintf(buf, "%s%s.imf", m_newPcJobImfNameTable[job], m_pcSexNameTable[sex]);

	return	buf;
}

char *	CSession :: GetAccessoryActName(int job, int &head, int sex, int accessory, char *buf)
{
	if (accessory == ACCESSORY_NONE) {
		buf[0] = 0;
		return	buf;
	}
	if (m_newAccessoryNameTable[accessory] == NULL) {
		buf[0] = 0;
		return	buf;
	}
	sprintf(buf, "\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.act", m_newPcSexNameTable[sex], m_newPcSexNameTable[sex], m_newAccessoryNameTable[accessory]);
	if (accessory == ACCESSORY_BATTLEONLINE) {
		switch (sex) {
		case	SEX_MALE:
			sprintf(buf, "\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s_\xb9\xe8\xc6\xb2\xbf\xc2\xb6\xf3\xc0\xce%s.act", m_newPcSexNameTable[sex], m_newPcSexNameTable[sex], m_newPcHeadNameTable_M[head]);
			break;
		default:
			sprintf(buf, "\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s_\xb9\xe8\xc6\xb2\xbf\xc2\xb6\xf3\xc0\xce%s.act", m_newPcSexNameTable[sex], m_newPcSexNameTable[sex], m_newPcHeadNameTable_F[head]);
			break;
		}
	}
	return	buf;
}

char *	CSession :: GetAccessorySprName(int job, int &head, int sex, int accessory, char * buf)
{
	if (accessory == ACCESSORY_NONE) {
		buf[0] = 0;
		return	buf;
	}
	if (m_newAccessoryNameTable[accessory] == NULL) {
		buf[0] = 0;
		return	buf;
	}
	sprintf(buf, "\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.spr", m_newPcSexNameTable[sex], m_newPcSexNameTable[sex], m_newAccessoryNameTable[accessory]);
	if (accessory == ACCESSORY_BATTLEONLINE) {
		switch (sex) {
		case	SEX_MALE:
			sprintf(buf, "\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s_\xb9\xe8\xc6\xb2\xbf\xc2\xb6\xf3\xc0\xce%s.spr", m_newPcSexNameTable[sex], m_newPcSexNameTable[sex], m_newPcHeadNameTable_M[head]);
			break;
		default:
			sprintf(buf, "\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s_\xb9\xe8\xc6\xb2\xbf\xc2\xb6\xf3\xc0\xce%s.spr", m_newPcSexNameTable[sex], m_newPcSexNameTable[sex], m_newPcHeadNameTable_F[head]);
			break;
		}
	}
	return	buf;
}

int		CSession :: GetTalkType(const char *chatBuf, TALKTYPE* talkType, string param[3])
{
	int		chatStartOffset = 0;



	if (chatBuf == NULL )
	{
		*talkType = TT_UNKNOWN;
		chatStartOffset = -1;
		return	chatStartOffset;
	}

	CTokenizer	t;
	t << string(chatBuf);

	if (t.NumTokens() == 0) {
		*talkType = TT_UNKNOWN;
		chatStartOffset = -1;
		return	chatStartOffset;
	}


	MakeStringLower(t[0]);

	if (chatBuf[0] != '/') {
		*talkType = TT_NORMAL;
	} else if (t[0] == "/\xa4\xd0") {
		*talkType = TT_BROADCAST;
		chatStartOffset = 3;
	} else if (t[0] == "/b") {
		*talkType = TT_BROADCAST;
		chatStartOffset = 2;
	} else if (t[0] == "/lb") {
		*talkType = TT_LOCALBROADCAST;
		chatStartOffset = 3;
	} else if (t[0] == "/\xa4\xd3\xa4\xd0") {
		*talkType = TT_LOCALBROADCAST;
		chatStartOffset = 5;
	}  else if (t[0] == "/nb") {
		*talkType = TT_NAMELESS_BROADCAST;
		chatStartOffset = 3;
	}  else if (t[0] == "/\xc6\xc4\xb6\xf5\xb8\xbb") {
		*talkType = TT_NAMELESS_BLUEBROADCAST;
		chatStartOffset = 7;
	}  else if (t[0] == "/\xa4\xcc\xa4\xd0") {
		*talkType = TT_NAMELESS_BROADCAST;
		chatStartOffset = 5;
	}  else if (t[0] == "/nlb") {
		*talkType = TT_NAMELESS_LOCALBROADCAST;
		chatStartOffset = 4;
	}  else if (t[0] == "/\xa4\xcf\xa4\xd0") {
		*talkType = TT_NAMELESS_LOCALBROADCAST;
		chatStartOffset = 5;
	} else if (t[0] == "/bb"){
		*talkType = TT_NAMELESS_BLUEBROADCAST;
		chatStartOffset = 3;
	} else if (t[0] == "/\xc1\xf6\xbf\xaa\xc6\xc4\xb6\xf5\xb8\xbb") {
		*talkType = TT_NAMELESS_BLUELOCALBROADCAST;
		chatStartOffset = 11;
	} else if (t[0] == "/mb") {
		*talkType = TT_NAMELESS_BLUELOCALBROADCAST;
		chatStartOffset = 3;
	} else if (t[0] == "/kill") {
		*talkType = TT_DISCONNECT_CHARACTER;
		chatStartOffset = 5;
	} else if (t[0] == "/v" || t[0] == "/\xa4\xbd" ) {
		*talkType = TT_VOLUME;
		if (t.NumTokens() >= 2)
			param[0] = t[1];
		else {
			int		v = Get2DVolume();
			char	buf[36];
			itoa(v, buf, 10);
			param[0] = buf;
		}
		chatStartOffset = -1;
	} else if (t[0] == "/bv" || t[0] == "/\xa4\xd0\xa4\xbd" ) {
		*talkType = TT_BVOLUME;
		if (t.NumTokens() >= 2)
			param[0] = t[1];
		else {
			int		bv = GetStreamVolume();
			char	buf[36];
			itoa(bv, buf, 10);
			param[0] = buf;
		}
		chatStartOffset = -1;
	} else if ((t[0] == "/gc" || t[0] == "/guildchat") && t.NumTokens()>=2) {
		*talkType = TT_GUILD_CHAT;
		if (t[0] == "/gc") {
			param[0] = &(chatBuf[4]);
		} else {
			param[0] = &(chatBuf[11]);
		}
		chatStartOffset = -1;
	} else if ((t[0] == "/ex" || t[0] == "/\xc2\xf7\xb4\xdc" ) && t.NumTokens()>=2) {
		*talkType = TT_REQ_WHISPER_PC_EX;
		if (t[0] == "/ex") {
			param[0] = &(chatBuf[4]);
		} else {
			param[0] = &(chatBuf[6]);
		}
		chatStartOffset = -1;
	} else if ((t[0] == "/in" || t[0] == "/\xc7\xd8\xc1\xa6" ) && t.NumTokens()>=2) {
		*talkType = TT_REQ_WHISPER_PC_IN;
		if (t[0] == "/in") {
			param[0] = &(chatBuf[4]);
		} else {
			param[0] = &(chatBuf[6]);
		}
		chatStartOffset = -1;
	} else if (t[0] == "/red") {
		*talkType = TT_IS_REDCELL;
		chatStartOffset = -1;
		for (int i=1; i<t.NumTokens(); i++) {
			param[0] += t[i];
			param[0] += " ";
		}
	} else if (( t[0] == "/\xb0\xc5\xb7\xa1" || t[0] == "/rjfo" || t[0] == "/\xb1\xb3\xc8\xaf" || t[0] == "/ryghks" || t[0] == "/deal" || t[0] == "/\xa4\xb7\xa4\xa7\xb9\xcc" ) && t.NumTokens()==2) {
		*talkType = TT_REQ_EXCHANGE_ITEM;
		param[0] = t[1];
		chatStartOffset = -1;
	} else if (( t[0] == "/\xb1\xe6\xb5\xe5" || t[0] == "/rlfem" || t[0] == "/guild") && t.NumTokens()==2) {
		*talkType = TT_REQ_MAKE_GUILD;
		param[0] = t[1];
		chatStartOffset = -1;
	} else if (( t[0] == "/\xc7\xd8\xc3\xbc" || t[0] == "/gocp" || t[0] == "/breakguild") && t.NumTokens()==2) {
		*talkType = TT_REQ_BREAK_GUILD;
		param[0] = t[1];
		chatStartOffset = -1;
	} else if (( t[0] == "/\xb0\xe1\xbc\xba" || t[0] == "/ruftjd" || t[0] == "/organize") && t.NumTokens()==2) {
		*talkType = TT_REQ_MAKE_GROUP;
		param[0] = t[1];
		chatStartOffset = -1;
	} else if ((t[0] == "/\xb8\xf3\xbd\xba\xc5\xcd" || t[0] == "/\xbe\xc6\xc0\xcc\xc5\xdb" || t[0] == "/item" || t[0] == "/monster") && t.NumTokens()==2) {
		*talkType = TT_REQ_ITEM_CREATE;
		param[0] = t[1];
		chatStartOffset = -1;
	} else if (( t[0] == "/\xb8\xca\xc0\xcc\xb5\xbf" || t[0] == "/mapmove" || t[0] == "/mm" ) && t.NumTokens() > 1) {
		*talkType = TT_REQ_MOVETO_MAP;
		param[0] = t[1];
		chatStartOffset = -1;
		if(t.NumTokens()==4) {
			param[1] = t[2];
			param[2] = t[3];
		} else {
			param[1] = param[2] = "0";
		}
	} else if (( t[0] == "/\xc1\xa6\xb8\xed" || t[0] == "/wpaud" || t[0] == "/expel") && t.NumTokens()==2) {
		*talkType = TT_REQ_EXPEL_GROUP_MEMBER;
		param[0] = t[1];
		chatStartOffset = -1;

	} else if ((t[0] == "/sm") && t.NumTokens()==2) {
		*talkType = TT_TEST_SHOW_IMAGE;
		param[0] = t[1];
		chatStartOffset = -1;
	} else if (t[0] == "/resetstate") {
		*talkType = TT_REQ_RESETSTATE;
		chatStartOffset = -1;
	} else if (t[0] == "/resetskill") {
		*talkType = TT_REQ_RESETSKILL;
		chatStartOffset = -1;
	} else if ((t[0] == "/changemaptype" || t[0] == "/cmt") && t.NumTokens()==4) {
		*talkType = TT_CHANGE_MAPTYPE;
		param[0] = t[1];
		param[1] = t[2];
		param[2] = t[3];
		chatStartOffset = -1;
	} else if(t[0] == "/emblem" || t[0] == "/\xbf\xa5\xba\xed\xb7\xbd" || t[0] == "/\xbe\xda\xba\xed\xb7\xbd" || t[0] == "/dpaqmffpa" || t[0] == "/doaqmffpa" ) {
		*talkType = TT_TOGGLE_EMBLEM;
		chatStartOffset = -1;
	} else if(t[0] == "/hide") {
		*talkType = TT_HIDE;
		chatStartOffset = -1;
	} else if(t[0] == "/\xc6\xf9\xc6\xae" || t[0] == "/font") {
		*talkType = TT_FONT;
		chatStartOffset = -1;
	} else if(t[0] == "/mineffect" || t[0] == MsgStr(MSI_MINEFFECT)) {
		*talkType = TT_IS_REDCELL;
		m_isMinEffect = 1 - m_isMinEffect;
		m_isMinEffectOld = m_isMinEffect;
		if (m_isMinEffect)
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MINEFFECT_ON)), int(RGB(255,255,100)), 0, 0);
		else
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MINEFFECT_OFF)), int(RGB(255,255,100)), 0, 0);
	}
	else if (t[0] == "/remove" && t.NumTokens() == 2)
	{
		*talkType = TT_REMOVE;
		param[0] = t[1];
		chatStartOffset = -1;
	}
	else if (t[0] == "/summon" && t.NumTokens() == 2)
	{
		*talkType = TT_RECALL_GID;
		param[0] = t[1];
		chatStartOffset = -1;
	} else if (t[0] == "/shift") {
		*talkType = TT_SHIFT;
		for (int a = 0; a < t.NumTokens()-1; a++) {
			param[a] = t[a+1];
		}
		chatStartOffset = -1;
	} else if (t[0] == "/recall" && t.NumTokens() == 2) {
		*talkType = TT_RECALL_AID;
		param[0] = t[1];
		chatStartOffset = -1;
	}
	else if (t[0] == "/check" && t.NumTokens() == 2)
	{
		*talkType = TT_CHECK_ID;
		param[0] = t[1];
		chatStartOffset = -1;
	}
	else if (t[0] == "/rc" && t.NumTokens() == 2)
	{
		*talkType = TT_RC_ID;
		param[0] = t[1];
		chatStartOffset = -1;
	}
	else if ( t[0] == "/\xbf\xe4\xb1\xdd")
	{
		*talkType = TT_BILLING_INFO;
		chatStartOffset = -1;
	}
	else if ( t[0] == "/\xc0\xcc\xb8\xa7\xc7\xa5\xbd\xc3" || t[0] == "/showname")
	{
		*talkType = TT_SHOW_NAME_TYPE;
	}
	else if ( ( t[0] == "/str+" ) && t.NumTokens() > 1 )
	{
		*talkType = TT_SIYEON_STATUS;
		g_modeMgr.GetGameMode()->m_SiyeonStatus[0] = atoi(t[1].c_str());
	}
	else if ( ( t[0] == "/agi+" ) && t.NumTokens() > 1 )
	{
		*talkType = TT_SIYEON_STATUS;
		g_modeMgr.GetGameMode()->m_SiyeonStatus[1] = atoi(t[1].c_str());
	}
	else if ( ( t[0] == "/luk+" ) && t.NumTokens() > 1 )
	{
		*talkType = TT_SIYEON_STATUS;
		g_modeMgr.GetGameMode()->m_SiyeonStatus[2] = atoi(t[1].c_str());
	}
	else if ( ( t[0] == "/int+" ) && t.NumTokens() > 1 )
	{
		*talkType = TT_SIYEON_STATUS;
		g_modeMgr.GetGameMode()->m_SiyeonStatus[3] = atoi(t[1].c_str());
	}
	else if ( ( t[0] == "/vit+" ) && t.NumTokens() > 1 )
	{
		*talkType = TT_SIYEON_STATUS;
		g_modeMgr.GetGameMode()->m_SiyeonStatus[4] = atoi(t[1].c_str());
	}
	else if ( ( t[0] == "/dex+" ) && t.NumTokens() > 1 )
	{
		*talkType = TT_SIYEON_STATUS;
		g_modeMgr.GetGameMode()->m_SiyeonStatus[5] = atoi(t[1].c_str());
	}
	else if ( ( t[0] == "/hi" ) )
	{
		string temp;
		temp = "";

		*talkType = TT_HI;

		if (t.NumTokens()>1)
		{
			param[0] = t[1];

			for (int i=1;i<t.NumTokens();i++)
			{
				temp += t[i];
				temp += " ";
			}
			g_windowMgr.SetChatMsg(temp.c_str());
		}

		chatStartOffset = -1;

	}
	else if (( t[0] == "/\xc3\xca\xb4\xeb" || t[0] == "/cheo" || t[0] == "/invite") && t.NumTokens()==2)
	{
		*talkType = TT_REQ_INVITE_PARTY;
		param[0] = t[1];
		chatStartOffset = -1;
	}

	else
	{
		{
			*talkType = GetNoParamTalkType(t[0].c_str());
			chatStartOffset = -1;
		}
	}

	if (chatStartOffset != -1 && *talkType != TT_NORMAL) {
		while (*(chatBuf+chatStartOffset) == ' ')
			chatStartOffset++;
	}
	return	chatStartOffset;
}

extern ServerType g_serverType;
void	CSession :: InitTalkTypeTable()
{
	m_talkTypeTable.clear();

	m_talkTypeTable.push_back(TTPair("/help", TT_HELP));
	m_talkTypeTable.push_back(TTPair("/h", TT_HELP));
	m_talkTypeTable.push_back(TTPair("/killall", TT_DISCONNECT_ALL_CHARACTER));
	m_talkTypeTable.push_back(TTPair("/wjqthrwk", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/snrn", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/who", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/w", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/dmadkr", TT_MUSIC_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/bgm", TT_MUSIC_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/effect", TT_EFFECT_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/dlvprxm", TT_EFFECT_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/sound", TT_EFFECT_SOUND_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/gyrhkdma", TT_EFFECT_SOUND_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/skip", TT_FRAMESKIP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/ex", TT_REQ_WHISPER_LIST));
	m_talkTypeTable.push_back(TTPair("/inall", TT_REQ_WHISPER_STATE_IN));
	m_talkTypeTable.push_back(TTPair("/exall", TT_REQ_WHISPER_STATE_EX));
	m_talkTypeTable.push_back(TTPair("/sit", TT_REQ_SIT));
	m_talkTypeTable.push_back(TTPair("/dkswrl", TT_REQ_SIT));
	m_talkTypeTable.push_back(TTPair("/stand", TT_REQ_STAND));
	m_talkTypeTable.push_back(TTPair("/tjrl", TT_REQ_STAND));
	m_talkTypeTable.push_back(TTPair("/coxldqkd", TT_MAKE_CHATROOMMAKEWND));
	m_talkTypeTable.push_back(TTPair("/chat", TT_MAKE_CHATROOMMAKEWND));
	m_talkTypeTable.push_back(TTPair("/xkfxhl", TT_REQ_LEAVE_GROUP));
	m_talkTypeTable.push_back(TTPair("/leave", TT_REQ_LEAVE_GROUP));
	m_talkTypeTable.push_back(TTPair("/stat", TT_STAT));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SURPRISE), TT_REQ_EMOTION_SURPRISE));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_QUESTION), TT_REQ_EMOTION_QUESTION));
	m_talkTypeTable.push_back(TTPair("/delight", TT_REQ_EMOTION_DELIGHT));
	m_talkTypeTable.push_back(TTPair("/ghk", TT_REQ_EMOTION_ANGER));
	m_talkTypeTable.push_back(TTPair("/fret", TT_REQ_EMOTION_FRET));
	m_talkTypeTable.push_back(TTPair("/Wkwmd", TT_REQ_EMOTION_FRET));
	m_talkTypeTable.push_back(TTPair("/q", TT_REQ_EXIT_ROOM));
	m_talkTypeTable.push_back(TTPair("/rlQma", TT_REQ_EMOTION_DELIGHT));
	m_talkTypeTable.push_back(TTPair("/heart", TT_REQ_EMOTION_THROB));
	m_talkTypeTable.push_back(TTPair("/gkxm", TT_REQ_EMOTION_THROB));
	m_talkTypeTable.push_back(TTPair("/sweat", TT_REQ_EMOTION_SWEAT));
	m_talkTypeTable.push_back(TTPair("/Eka", TT_REQ_EMOTION_SWEAT));
	m_talkTypeTable.push_back(TTPair("/wnajr", TT_REQ_EMOTION_ROCK));
	m_talkTypeTable.push_back(TTPair("/qh", TT_REQ_EMOTION_WRAP));
	m_talkTypeTable.push_back(TTPair("/bo", TT_REQ_EMOTION_WRAP));
	m_talkTypeTable.push_back(TTPair("/rkdnl", TT_REQ_EMOTION_SCISSOR));
	m_talkTypeTable.push_back(TTPair("/gawi", TT_REQ_EMOTION_SCISSOR));
	m_talkTypeTable.push_back(TTPair("/bawi", TT_REQ_EMOTION_ROCK));
	m_talkTypeTable.push_back(TTPair("/qkdnl", TT_REQ_EMOTION_ROCK));
	m_talkTypeTable.push_back(TTPair("/whkvy", TT_MAP_POS));
	m_talkTypeTable.push_back(TTPair("/wkdth", TT_MAP_POS));
	m_talkTypeTable.push_back(TTPair("/where", TT_MAP_POS));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_THINK), TT_REQ_EMOTION_THINK));
	m_talkTypeTable.push_back(TTPair("/money", TT_REQ_EMOTION_MONEY));
	m_talkTypeTable.push_back(TTPair("/ehs", TT_REQ_EMOTION_MONEY));
	m_talkTypeTable.push_back(TTPair("/anger", TT_REQ_EMOTION_ANGER));
	m_talkTypeTable.push_back(TTPair("/aha", TT_REQ_EMOTION_AHA));
	m_talkTypeTable.push_back(TTPair("/dkgk", TT_REQ_EMOTION_AHA));
	m_talkTypeTable.push_back(TTPair("/tip", TT_OPEN_TIP));
	m_talkTypeTable.push_back(TTPair("/rldjr", TT_REMEMBER_WARPPOINT));
	m_talkTypeTable.push_back(TTPair("/memo", TT_REMEMBER_WARPPOINT));
	m_talkTypeTable.push_back(TTPair("/snap", TT_ATTACK_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/tmsoq", TT_ATTACK_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/skillsnap", TT_SKILL_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/tmzlftmsoq", TT_SKILL_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/itemsnap", TT_ITEM_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/dkdlxpatmsoq", TT_ITEM_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/rkfanfl", TT_REQ_SAVE_CHAT));
	m_talkTypeTable.push_back(TTPair("/savechat", TT_REQ_SAVE_CHAT));
	m_talkTypeTable.push_back(TTPair("/miss", TT_MISS_EFFECT_ON_OFF));
	if (g_serverType != ServerInstantEvent) {
		m_talkTypeTable.push_back(TTPair("/noshift", TT_NOSHIFT_ON_OFF));
		m_talkTypeTable.push_back(TTPair("/noctrl", TT_NOCTRL_ON_OFF));
		m_talkTypeTable.push_back(TTPair("/ns", TT_NOSHIFT_ON_OFF));
		m_talkTypeTable.push_back(TTPair("/nc", TT_NOCTRL_ON_OFF));
	}
	m_talkTypeTable.push_back(TTPair("/window", TT_WINDOW_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/wi", TT_WINDOW_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/aura", TT_AURA_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/doridori", TT_DORIDORI_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/bingbing", TT_BINGBING_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/bangbang", TT_BANGBANG_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/skillfail", TT_SKILLFAIL_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/sf", TT_SKILLFAIL_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/loginout", TT_LOGINOUT_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/li", TT_LOGINOUT_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/shopping", TT_SHOPPING_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/sh", TT_SHOPPING_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/quickspell", TT_QUICKSPELL_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/q1", TT_QUICKSPELL_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/quickspell2", TT_QUICKSPELL2_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/q2", TT_QUICKSPELL2_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/q3", TT_QUICKSPELL3_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/notalkmsg", TT_NOTALKMSG_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/nm", TT_NOTALKMSG_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/notalkmsg2", TT_NOTALKMSG2_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/nm2", TT_NOTALKMSG2_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/notrade", TT_NOTRADE_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/nt", TT_NOTRADE_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/set1", TT_SET1_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/set2", TT_SET2_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/set3", TT_SET3_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/camera", TT_FIXED_CAMERA_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/tp", TT_TESTPACKET));
	m_talkTypeTable.push_back(TTPair("/fog", TT_FOG));
	m_talkTypeTable.push_back(TTPair("/\xa4\xc7", TT_HELP));
	m_talkTypeTable.push_back(TTPair("/\xc3\xa4\xc6\xc3\xb9\xe6", TT_MAKE_CHATROOMMAKEWND));
	m_talkTypeTable.push_back(TTPair("/\xc3\xce\xa4\xb5", TT_MAKE_CHATROOMMAKEWND));
	m_talkTypeTable.push_back(TTPair("/\xc1\xa2\xbc\xd3\xc0\xda", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/\xb4\xa9\xb1\xb8", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/\xa4\xb8", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/\xc1\xc8", TT_REQ_NUM_USER));
	m_talkTypeTable.push_back(TTPair("/\xc0\xbd\xbe\xc7", TT_MUSIC_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xa4\xd0\xc8\xe5", TT_MUSIC_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xc0\xcc\xc6\xe5\xc6\xae", TT_EFFECT_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xc8\xbf\xb0\xfa\xc0\xbd", TT_EFFECT_SOUND_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xb3\xaa\xa4\xc1\xa4\xc4", TT_FRAMESKIP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xc2\xf7\xb4\xdc", TT_REQ_WHISPER_LIST));
	m_talkTypeTable.push_back(TTPair("/\xc7\xd8\xc1\xa6\xc0\xfc\xba\xce", TT_REQ_WHISPER_STATE_IN));
	m_talkTypeTable.push_back(TTPair("/\xc2\xf7\xb4\xdc\xc0\xfc\xba\xce", TT_REQ_WHISPER_STATE_EX));
	m_talkTypeTable.push_back(TTPair("/\xbe\xc9\xb1\xe2", TT_REQ_SIT));
	m_talkTypeTable.push_back(TTPair("/\x86n", TT_REQ_SIT));
	m_talkTypeTable.push_back(TTPair("/\xbc\xad\xb1\xe2", TT_REQ_STAND));
	m_talkTypeTable.push_back(TTPair("/\xa4\xa4\xa4\xb5\xb9\xb6", TT_REQ_STAND));
	m_talkTypeTable.push_back(TTPair("/\xa4\xb2", TT_REQ_EXIT_ROOM));
	m_talkTypeTable.push_back(TTPair("/\xc5\xbb\xc5\xf0", TT_REQ_LEAVE_GROUP));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_DELIGHT), TT_REQ_EMOTION_DELIGHT));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_THROB), TT_REQ_EMOTION_THROB));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SWEAT), TT_REQ_EMOTION_SWEAT));
	m_talkTypeTable.push_back(TTPair("/\xc1\xd6\xb8\xd4", TT_REQ_EMOTION_ROCK));
	m_talkTypeTable.push_back(TTPair("/\xba\xb8", TT_REQ_EMOTION_WRAP));
	m_talkTypeTable.push_back(TTPair("/\xb0\xa1\xc0\xa7", TT_REQ_EMOTION_SCISSOR));
	m_talkTypeTable.push_back(TTPair("/\xb9\xd9\xc0\xa7", TT_REQ_EMOTION_ROCK));
	m_talkTypeTable.push_back(TTPair("/\xc1\xc2\xc7\xa5", TT_MAP_POS));
	m_talkTypeTable.push_back(TTPair("/\xc0\xe5\xbc\xd2", TT_MAP_POS));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_MONEY), TT_REQ_EMOTION_MONEY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_ANGER), TT_REQ_EMOTION_ANGER));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_FRET), TT_REQ_EMOTION_FRET));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_AHA), TT_REQ_EMOTION_AHA));
	m_talkTypeTable.push_back(TTPair("/\xbb\xfe\xa4\xc4", TT_OPEN_TIP));
	m_talkTypeTable.push_back(TTPair("/\xb1\xe2\xbe\xef", TT_REMEMBER_WARPPOINT));
	m_talkTypeTable.push_back(TTPair("/\xb4\xa9\xb8\xde", TT_ATTACK_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xbd\xba\xb3\xc0", TT_ATTACK_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xb3\xaa\xa4\xc1\xa4\xd3\xa4\xd3\xb4\xa9\xb8\xde", TT_SKILL_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xbd\xba\xc5\xb3\xbd\xba\xb3\xc0", TT_SKILL_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xa4\xc1\xa4\xb5\xb5\xe5\xb4\xa9\xb8\xde", TT_ITEM_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xbe\xc6\xc0\xcc\xc5\xdb\xbd\xba\xb3\xc0", TT_ITEM_SNAP_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xb0\xa5\xb9\xab\xb8\xae", TT_REQ_SAVE_CHAT));
	m_talkTypeTable.push_back(TTPair("/autosavechat", TT_REQ_AUTO_SAVE_CHAT));
	m_talkTypeTable.push_back(TTPair("/objlight", TT_TOGGLE_OBJLIGHT));
	m_talkTypeTable.push_back(TTPair("/loading", TT_RANDOM_LOADING_SCREEN_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/minigame", TT_MINIGAME_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xbd\xc5\xb0\xed", TT_REQ_REPORT_CHAT));
	m_talkTypeTable.push_back(TTPair("/tlsrh", TT_REQ_REPORT_CHAT));
	m_talkTypeTable.push_back(TTPair("/report", TT_REQ_REPORT_CHAT));
	m_talkTypeTable.push_back(TTPair("/battlemode", TT_BATTLE_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/bm", TT_BATTLE_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/battlechat", TT_BATTLECHAT_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xc0\xfc\xc0\xe5", TT_BATTLECHAT_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/pvpinfo", TT_SHOW_PVP_INFO));
	m_talkTypeTable.push_back(TTPair("/pi", TT_SHOW_PVP_INFO));
	m_talkTypeTable.push_back(TTPair("/\xc0\xda\xb5\xbf\xc0\xc0\xb4\xe4", TT_AUTO_MSG_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/automsg", TT_AUTO_MSG_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/am", TT_AUTO_MSG_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/traceai", TT_TRACEAI_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xc8\xa3\xb9\xae\xc1\xf6\xb4\xc9", TT_HOMUN_USERAI_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/hoai", TT_HOMUN_USERAI_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/\xbf\xeb\xba\xb4\xc1\xf6\xb4\xc9", TT_MER_USERAI_ON_OFF));
	m_talkTypeTable.push_back(TTPair("/merai", TT_MER_USERAI_ON_OFF));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_BIGTHROB), TT_REQ_EMOTION_BIGTHROB));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_THANKS), TT_REQ_EMOTION_THANKS));
	m_talkTypeTable.push_back(TTPair("/\xa4\xa3", TT_REQ_EMOTION_THANKS));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_KEK), TT_REQ_EMOTION_KEK));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SORRY), TT_REQ_EMOTION_SORRY));
	m_talkTypeTable.push_back(TTPair("/sorry", TT_REQ_EMOTION_SORRY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SMILE), TT_REQ_EMOTION_SMILE));
	m_talkTypeTable.push_back(TTPair("/\xc7\xcf\xc7\xcf", TT_REQ_EMOTION_SMILE));
	m_talkTypeTable.push_back(TTPair("/\xa4\xb7", TT_REQ_EMOTION_SMILE));
	m_talkTypeTable.push_back(TTPair("/smile", TT_REQ_EMOTION_SMILE));
	m_talkTypeTable.push_back(TTPair("/\xa4\xa1", TT_REQ_EMOTION_DELIGHT));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_PROFUSELY_SWEAT), TT_REQ_EMOTION_PROFUSELY_SWEAT));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SCRATCH), TT_REQ_EMOTION_SCRATCH));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_BEST), TT_REQ_EMOTION_BEST));
	m_talkTypeTable.push_back(TTPair("/\xbf\xf8\xc3\xf2", TT_REQ_EMOTION_BEST));
	m_talkTypeTable.push_back(TTPair("/\xa4\xba", TT_REQ_EMOTION_BEST));
	m_talkTypeTable.push_back(TTPair("/emotion", TT_SHOW_EMOTIONLIST));
	m_talkTypeTable.push_back(TTPair("/clipmouse", TT_CLIPMOUSE));
	m_talkTypeTable.push_back(TTPair("/lightmap", TT_LIGHTMAP));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_STARE_ABOUT), TT_REQ_EMOTION_STARE_ABOUT));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_HUK), TT_REQ_EMOTION_HUK));
	m_talkTypeTable.push_back(TTPair("/\xa4\xbe", TT_REQ_EMOTION_HUK));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_O), TT_REQ_EMOTION_O));
	m_talkTypeTable.push_back(TTPair("/o", TT_REQ_EMOTION_O));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_X), TT_REQ_EMOTION_X));
	m_talkTypeTable.push_back(TTPair("/x", TT_REQ_EMOTION_X));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_HELP), TT_REQ_EMOTION_HELP));
	m_talkTypeTable.push_back(TTPair("/\xa4\xba\xa4\xba", TT_REQ_EMOTION_HELP));
	m_talkTypeTable.push_back(TTPair("/help", TT_REQ_EMOTION_HELP));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_GO), TT_REQ_EMOTION_GO));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_CRY), TT_REQ_EMOTION_CRY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_KIK), TT_REQ_EMOTION_KIK));
	m_talkTypeTable.push_back(TTPair("/\xc5\xb1", TT_REQ_EMOTION_KIK));
	m_talkTypeTable.push_back(TTPair("/\xa4\xbb", TT_REQ_EMOTION_KIK));
	m_talkTypeTable.push_back(TTPair("/\xc4\xc9\xc4\xc9", TT_REQ_EMOTION_KIK));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_CHUP), TT_REQ_EMOTION_CHUP));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_CHUPCHUP), TT_REQ_EMOTION_CHUPCHUP));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_HNG), TT_REQ_EMOTION_HNG));
	m_talkTypeTable.push_back(TTPair("/\xb2\xf4\xb4\xf6", TT_REQ_EMOTION_OK));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_OK), TT_REQ_EMOTION_OK));
	m_talkTypeTable.push_back(TTPair("/\xc0\xcc\xb8\xf0\xbc\xc7", TT_SHOW_EMOTIONLIST));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_STARE), TT_REQ_EMOTION_STARE));
	m_talkTypeTable.push_back(TTPair("/\xa4\xb9", TT_REQ_EMOTION_STARE));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_HUNGRY), TT_REQ_EMOTION_HUNGRY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_COOL), TT_REQ_EMOTION_COOL));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_MERONG), TT_REQ_EMOTION_MERONG));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SHY), TT_REQ_EMOTION_SHY));
	m_talkTypeTable.push_back(TTPair("/\xba\xce", TT_REQ_EMOTION_SHY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_GOODBOY), TT_REQ_EMOTION_GOODBOY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SPTIME), TT_REQ_EMOTION_SPTIME));
	m_talkTypeTable.push_back(TTPair("/\xbf\xa5", TT_REQ_EMOTION_SPTIME));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SEXY), TT_REQ_EMOTION_SEXY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_COMEON), TT_REQ_EMOTION_COMEON));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SLEEPY), TT_REQ_EMOTION_SLEEPY));
	m_talkTypeTable.push_back(TTPair("/\xa4\xb8", TT_REQ_EMOTION_SLEEPY));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_CONGRATULATION), TT_REQ_EMOTION_CONGRATULATION));
	m_talkTypeTable.push_back(TTPair("/\xa4\xba\xa4\xbb", TT_REQ_EMOTION_CONGRATULATION));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_HPTIME), TT_REQ_EMOTION_HPTIME));
	m_talkTypeTable.push_back(TTPair("/\xc7\xc7", TT_REQ_EMOTION_HPTIME));
	m_talkTypeTable.push_back(TTPair("/e1", TT_REQ_EMOTION_STARE));
	m_talkTypeTable.push_back(TTPair("/e2", TT_REQ_EMOTION_HUNGRY));
	m_talkTypeTable.push_back(TTPair("/e3", TT_REQ_EMOTION_COOL));
	m_talkTypeTable.push_back(TTPair("/e4", TT_REQ_EMOTION_MERONG));
	m_talkTypeTable.push_back(TTPair("/e5", TT_REQ_EMOTION_SHY));
	m_talkTypeTable.push_back(TTPair("/e6", TT_REQ_EMOTION_GOODBOY));
	m_talkTypeTable.push_back(TTPair("/e7", TT_REQ_EMOTION_SPTIME));
	m_talkTypeTable.push_back(TTPair("/e8", TT_REQ_EMOTION_SEXY));
	m_talkTypeTable.push_back(TTPair("/e9", TT_REQ_EMOTION_COMEON));
	m_talkTypeTable.push_back(TTPair("/e10", TT_REQ_EMOTION_SLEEPY));
	m_talkTypeTable.push_back(TTPair("/e11", TT_REQ_EMOTION_CONGRATULATION));
	m_talkTypeTable.push_back(TTPair("/e12", TT_REQ_EMOTION_HPTIME));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_SPARK), TT_REQ_EMOTION_SPARK));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_CONFUSE), TT_REQ_EMOTION_CONFUSE));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_OHNO), TT_REQ_EMOTION_OHNO));
	m_talkTypeTable.push_back(TTPair("/\xc7\xd1\xbc\xfb", TT_REQ_EMOTION_OHNO));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_HUM), TT_REQ_EMOTION_HUM));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_BLABLA), TT_REQ_EMOTION_BLABLA));
	m_talkTypeTable.push_back(TTPair("/\xbd\xc3\xb2\xf8\xbd\xc3\xb2\xf8", TT_REQ_EMOTION_BLABLA));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_OTL), TT_REQ_EMOTION_OTL));
	m_talkTypeTable.push_back(TTPair(MsgStr(MSI_EMOTION_DICE), TT_REQ_EMOTION_DICE));
	m_talkTypeTable.push_back(TTPair("/dice", TT_REQ_EMOTION_DICE));
	m_talkTypeTable.push_back(TTPair("/e13", TT_REQ_EMOTION_SPARK));
	m_talkTypeTable.push_back(TTPair("/e14", TT_REQ_EMOTION_CONFUSE));
	m_talkTypeTable.push_back(TTPair("/e15", TT_REQ_EMOTION_OHNO));
	m_talkTypeTable.push_back(TTPair("/e16", TT_REQ_EMOTION_HUM));
	m_talkTypeTable.push_back(TTPair("/e17", TT_REQ_EMOTION_BLABLA));
	m_talkTypeTable.push_back(TTPair("/e18", TT_REQ_EMOTION_OTL));
	m_talkTypeTable.push_back(TTPair("/e19", TT_REQ_EMOTION_DICE));
	m_talkTypeTable.push_back(TTPair("/otl", TT_REQ_EMOTION_OTL));
	m_talkTypeTable.push_back(TTPair("/\xb7\xaf\xba\xea", TT_REQ_EMOTION_LUV));
	m_talkTypeTable.push_back(TTPair("/e20", TT_REQ_EMOTION_LUV));
	m_talkTypeTable.push_back(TTPair("/e27", TT_REQ_EMOTION_HUM2));
	m_talkTypeTable.push_back(TTPair("/e28", TT_REQ_EMOTION_ABS));
	m_talkTypeTable.push_back(TTPair("/e29", TT_REQ_EMOTION_OOPS));
	m_talkTypeTable.push_back(TTPair("/e30", TT_REQ_EMOTION_SPIT));
	m_talkTypeTable.push_back(TTPair("/e31", TT_REQ_EMOTION_ENE));
	m_talkTypeTable.push_back(TTPair("/e32", TT_REQ_EMOTION_PANIC));
	m_talkTypeTable.push_back(TTPair("/e33", TT_REQ_EMOTION_WHISP));
	m_talkTypeTable.push_back(TTPair("/\xc8\xec", TT_REQ_EMOTION_HUM2));
	m_talkTypeTable.push_back(TTPair("/\xb8\xdb", TT_REQ_EMOTION_ABS));
	m_talkTypeTable.push_back(TTPair("/\xbf\xf3", TT_REQ_EMOTION_OOPS));
	m_talkTypeTable.push_back(TTPair("/\xc0\xa1", TT_REQ_EMOTION_SPIT));
	m_talkTypeTable.push_back(TTPair("/\xc5\xbb\xb7\xc2", TT_REQ_EMOTION_ENE));
	m_talkTypeTable.push_back(TTPair("/\xb0\xf8\xc8\xb2", TT_REQ_EMOTION_PANIC));
	m_talkTypeTable.push_back(TTPair("/\xc0\xa7\xbd\xba\xc6\xdb", TT_REQ_EMOTION_WHISP));
	m_talkTypeTable.push_back(TTPair("/hum", TT_REQ_EMOTION_HUM2));
	m_talkTypeTable.push_back(TTPair("/abs", TT_REQ_EMOTION_ABS));
	m_talkTypeTable.push_back(TTPair("/oops", TT_REQ_EMOTION_OOPS));
	m_talkTypeTable.push_back(TTPair("/spit", TT_REQ_EMOTION_SPIT));
	m_talkTypeTable.push_back(TTPair("/ene", TT_REQ_EMOTION_ENE));
	m_talkTypeTable.push_back(TTPair("/panic", TT_REQ_EMOTION_PANIC));
	m_talkTypeTable.push_back(TTPair("/whisp", TT_REQ_EMOTION_WHISP));
	m_talkTypeTable.push_back(TTPair("/hw", TT_HOMUN_WAIT));
	m_talkTypeTable.push_back(TTPair("/\xa4\xc7\xa4\xb8", TT_HOMUN_WAIT));
	m_talkTypeTable.push_back(TTPair("/blacksmith", TT_REQ_BLACKSMITH));
	m_talkTypeTable.push_back(TTPair("/alchemist", TT_REQ_ALCHEMIST));
	m_talkTypeTable.push_back(TTPair("/taekwon", TT_REQ_TAEKWON));
	m_talkTypeTable.push_back(TTPair("/lv", TT_LEVEL));
	m_talkTypeTable.push_back(TTPair("/sp", TT_SP));
	m_talkTypeTable.push_back(TTPair("/pk", TT_REQ_KILLER_RANK));
	m_talkTypeTable.push_back(TTPair("/hunting", TT_REQ_HUNTING));
	m_talkTypeTable.push_back(TTPair("/cn", TT_COPY_NAME));
	m_talkTypeTable.push_back(TTPair("/\xc3\xca\xb4\xeb\xb0\xc5\xba\xce", TT_REQ_REFUSE_INVITE_PARTY));
	m_talkTypeTable.push_back(TTPair("/\xc3\xca\xb4\xeb\xbc\xf6\xb6\xf4", TT_REQ_ACCEPT_INVITE_PARTY));
	m_talkTypeTable.push_back(TTPair("/refuse", TT_REQ_REFUSE_INVITE_PARTY));
	m_talkTypeTable.push_back(TTPair("/accept", TT_REQ_ACCEPT_INVITE_PARTY));
	m_talkTypeTable.push_back(TTPair("/\xc0\xe5\xba\xf1\xb0\xf8\xb0\xb3", TT_REQ_OPEN_EQUIPEDITEM_ONOFF));
	m_talkTypeTable.push_back(TTPair("/wkdqlrhdro", TT_REQ_OPEN_EQUIPEDITEM_ONOFF));
	m_talkTypeTable.push_back(TTPair("/eqopen", TT_REQ_OPEN_EQUIPEDITEM_ONOFF));

	stable_sort(m_talkTypeTable.begin(), m_talkTypeTable.end(), DComp());

	m_cmdOnOffTable.push_back(TTPair("/battlemode", TT_BATTLE_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/notrade", TT_NOTRADE_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/noshift", TT_NOSHIFT_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/noctrl", TT_NOCTRL_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/skillfail", TT_SKILLFAIL_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/notalkmsg", TT_NOTALKMSG_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/notalkmsg2", TT_NOTALKMSG2_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/showname", TT_SHOW_NAME_TYPE));
	m_cmdOnOffTable.push_back(TTPair("/fog", TT_FOG));
	m_cmdOnOffTable.push_back(TTPair("/aura", TT_AURA_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/window", TT_WINDOW_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/miss", TT_MISS_EFFECT_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/set1", TT_SET1_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/set2", TT_SET2_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/q1", TT_QUICKSPELL_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/q2", TT_QUICKSPELL2_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/q3", TT_QUICKSPELL3_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/effect", TT_EFFECT_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/bgm", TT_MUSIC_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/sound", TT_EFFECT_SOUND_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/tip", TT_OPEN_TIP));
	m_cmdOnOffTable.push_back(TTPair("/loginout", TT_LOGINOUT_ON_OFF));
	m_cmdOnOffTable.push_back(TTPair("/shopping", TT_SHOPPING_ON_OFF));

	m_cmdExeTable.push_back(TTPair("/taekwon", TT_REQ_TAEKWON));
	m_cmdExeTable.push_back(TTPair("/hi", TT_HI));
	m_cmdExeTable.push_back(TTPair("/blacksmith", TT_REQ_BLACKSMITH));
	m_cmdExeTable.push_back(TTPair("/alchemist", TT_REQ_ALCHEMIST));
	m_cmdExeTable.push_back(TTPair("/help", TT_HELP));
	m_cmdExeTable.push_back(TTPair("/who", TT_REQ_NUM_USER));
	m_cmdExeTable.push_back(TTPair("/sit", TT_REQ_SIT));
	m_cmdExeTable.push_back(TTPair("/stand", TT_REQ_STAND));
	m_cmdExeTable.push_back(TTPair("/q", TT_REQ_EXIT_ROOM));
	m_cmdExeTable.push_back(TTPair("/leave", TT_REQ_LEAVE_GROUP));
	m_cmdExeTable.push_back(TTPair("/where", TT_MAP_POS));
	m_cmdExeTable.push_back(TTPair("/memo", TT_REMEMBER_WARPPOINT));
	m_cmdExeTable.push_back(TTPair("/chat", TT_MAKE_CHATROOMMAKEWND));
	m_cmdExeTable.push_back(TTPair("/savechat", TT_REQ_SAVE_CHAT));
	m_cmdExeTable.push_back(TTPair("/doridori", TT_DORIDORI_ON_OFF));
	m_cmdExeTable.push_back(TTPair("/bangbang", TT_BANGBANG_ON_OFF));
	m_cmdExeTable.push_back(TTPair("/bingbing", TT_BINGBING_ON_OFF));
	m_cmdExeTable.push_back(TTPair("/ex", TT_REQ_WHISPER_LIST));
	m_cmdExeTable.push_back(TTPair("/inall", TT_REQ_WHISPER_STATE_IN));
	m_cmdExeTable.push_back(TTPair("/exall", TT_REQ_WHISPER_STATE_EX));
	m_cmdExeTable.push_back(TTPair("/pvpinfo", TT_SHOW_PVP_INFO));
	m_cmdExeTable.push_back(TTPair("/hunting", TT_REQ_HUNTING));

	if (g_serviceType == ServiceBrazil) {
		m_cmdWinTable.push_back(WinPair("Sala de Chat", 'C'));
		m_cmdWinTable.push_back(WinPair("Atributos", 'A'));
		m_cmdWinTable.push_back(WinPair("Detalhes", 'D'));
		m_cmdWinTable.push_back(WinPair("Configura\xe7\xe3o", 'O'));
		m_cmdWinTable.push_back(WinPair("Invent?io", 'E'));
		m_cmdWinTable.push_back(WinPair("Carrinho", 'W'));
		m_cmdWinTable.push_back(WinPair("Habilidades", 'S'));
		m_cmdWinTable.push_back(WinPair("Equipamentos", 'Q'));
		m_cmdWinTable.push_back(WinPair("Inf. B?icas", 'V'));
		m_cmdWinTable.push_back(WinPair("Cl?", 'G'));
		m_cmdWinTable.push_back(WinPair("Conf. Grupo", 'P'));
		m_cmdWinTable.push_back(WinPair("Conf. Contatos", 'I'));
		m_cmdWinTable.push_back(WinPair("Grupo", 'Z'));
		m_cmdWinTable.push_back(WinPair("Contatos", 'H'));
		m_cmdWinTable.push_back(WinPair("Atalhos", 'M'));
		m_cmdWinTable.push_back(WinPair("Lista Emoticons", 'L'));
		m_cmdWinTable.push_back(WinPair("Bicho de Est.", 'J'));
	} else {
		m_cmdWinTable.push_back(WinPair("Chat", 'C'));
		m_cmdWinTable.push_back(WinPair("Status", 'A'));
		m_cmdWinTable.push_back(WinPair("DetailLevel", 'D'));
		m_cmdWinTable.push_back(WinPair("Option", 'O'));
		m_cmdWinTable.push_back(WinPair("Items", 'E'));
		m_cmdWinTable.push_back(WinPair("Merchant", 'W'));
		m_cmdWinTable.push_back(WinPair("Skill", 'S'));
		m_cmdWinTable.push_back(WinPair("Equipment", 'Q'));
		m_cmdWinTable.push_back(WinPair("BasicInfo", 'V'));
		m_cmdWinTable.push_back(WinPair("Guild", 'G'));
		m_cmdWinTable.push_back(WinPair("PartySetting", 'P'));
		m_cmdWinTable.push_back(WinPair("FriendOption", 'I'));
		m_cmdWinTable.push_back(WinPair("Messenger", 'Z'));
		m_cmdWinTable.push_back(WinPair("WhisperList", 'H'));
		m_cmdWinTable.push_back(WinPair("Emotion", 'M'));
		m_cmdWinTable.push_back(WinPair("EmotionList", 'L'));
		m_cmdWinTable.push_back(WinPair("PetInfo", 'J'));
	}

	m_keyWinTable.push_back("Alt + C");
	m_keyWinTable.push_back("Alt + A");
	m_keyWinTable.push_back("Alt + D");
	m_keyWinTable.push_back("Alt + O");
	m_keyWinTable.push_back("Alt + E");
	m_keyWinTable.push_back("Alt + W");
	m_keyWinTable.push_back("Alt + S");
	m_keyWinTable.push_back("Alt + Q");
	m_keyWinTable.push_back("Alt + V");
	m_keyWinTable.push_back("Alt + G");
	m_keyWinTable.push_back("Alt + P");
	m_keyWinTable.push_back("Alt + I");
	m_keyWinTable.push_back("Alt + Z");
	m_keyWinTable.push_back("Alt + H");
	m_keyWinTable.push_back("Alt + M");
	m_keyWinTable.push_back("Alt + L");
	m_keyWinTable.push_back("Alt + J");
}

const char *	CSession :: GetItemSpriteFileName_Skill(const char *itemName)
{
	static string	itemSpriteFileName;
	itemSpriteFileName = "\xbe\xc6\xc0\xcc\xc5\xdb\\";
	itemSpriteFileName += itemName;
	return	itemSpriteFileName.c_str();
}

TALKTYPE	CSession :: GetNoParamTalkType(const char *command)
{
	std::vector<std::pair<const char *, TALKTYPE> >::iterator	it =
		std::lower_bound(m_talkTypeTable.begin(), m_talkTypeTable.end(), command, DComp());
	if (it != m_talkTypeTable.end())
		if (strcmp(it->first, command) == 0)
			return	it->second;
	return	TT_UNKNOWN;
}

const char *	CSession :: GetItemSpriteFileName(const char *itemName, BOOL isIdentified)
{
	static string	itemSpriteFileName;
	itemSpriteFileName = "\xbe\xc6\xc0\xcc\xc5\xdb\\";
	itemSpriteFileName += GetItemResName_WithId(itemName, isIdentified);
	return	itemSpriteFileName.c_str();
}

void	CSession :: GetSkillBitmapFileName(const char *itemName, char *buf)
{
	sprintf(buf, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\item\\%s.bmp", itemName);
}

void	CSession :: GetItemBitmapFileName(const char *itemName, char *buf, BOOL isIdentified)
{
	sprintf(buf, "\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\item\\%s.bmp", GetItemResName_WithId(itemName, isIdentified));
}

const char *	CSession :: GetItemResName_WithId(const char *itemName, int isIdentified)
{
	if (isIdentified)
		return ITEM_INFO::GetItemDesc(atoi(itemName)).identifiedResourceName;

	return ITEM_INFO::GetItemDesc(atoi(itemName)).unidentifiedResourceName;
}

DWORD	CSession :: GetItemDisColor(ITEM_INFO *itemInfo)
{
	if (itemInfo->m_isIdentified)
	{
		if (itemInfo->m_isYours == 2)
			return	0xffffff00;
		else
			return	0xffffffff;
	}
	else
		return	0xffaaaaaa;
}

BOOL	CSession :: IsKorean()
{
	return g_serviceType == ServiceKorea ? TRUE : FALSE;
}

BOOL	CSession :: IsMasterAid(int showLevel)
{
	if (IsGravityAid(m_aid))
	{
		if (showLevel == 7)
			return TRUE;
	}
	if (g_serviceType != ServiceKorea)
		return FALSE;

	switch (m_aid) {
	case 100025:
		return (showLevel == 5);
	case 2919555:
	case 2919558:
		if (showLevel == 2)
			return TRUE;

	case 234073:
		return (showLevel == 1);
	}
	return FALSE;
}

BOOL	CSession :: InitMapNameTable()
{
	PCharList pCharList;
	PCharListIter it;
	m_mapNameTable.clear();
	m_mapNameTable.reserve(512);

	bool bResult = m_objParser.TokenFileToPCharList(pCharList,"mapNameTable.txt");
	sAssert(pCharList.size() % 2 == 0 );

	for ( it = pCharList.begin(); it != pCharList.end(); it+=2 )
		m_mapNameTable.push_back( StringPair(*it,*(it+1)) );

	sort(m_mapNameTable.begin(), m_mapNameTable.end(), SComp());
	return	bResult;
}

const char *	CSession :: GetMapName(const char *rswName)
{
	string modifyRswName = rswName;
#if defined(__MEMORIAL_DUNGEON)
	string::size_type markPos = modifyRswName.rfind("@");
	if((markPos != string::npos) && (markPos >= 4)){
		modifyRswName.erase(markPos-4,3);
	}
#endif
	vector<StringPair>::iterator it = lower_bound(m_mapNameTable.begin(), m_mapNameTable.end(), StringPair(modifyRswName.c_str(), modifyRswName.c_str()), SComp());
	if (it != m_mapNameTable.end()) {
		if ((strcmp(it->first, modifyRswName.c_str())==0)) {
			return	it->second;
		}
	}
	return	MsgStr(MSI_NOWHERE);
}

BOOL	CSession :: InitEnableObjLightMapTable()
{
	PCharList pCharList;
	PCharListIter it;
	m_enableObjLightMapList.clear();
	m_enableObjLightMapList.reserve(512);

	bool bResult = m_objParser.TokenFileToPCharList(pCharList,"mapObjLightTable.txt");
	sAssert(pCharList.size() % 2 == 0 );

	for ( it = pCharList.begin(); it != pCharList.end(); it+=2 )
	{
		bool b = ( strcmp( "off", *(it+1) ) != 0 );
		m_enableObjLightMapList.push_back( IntegerPair(*it,b) );
	}

	sort(m_enableObjLightMapList.begin(), m_enableObjLightMapList.end(), IListComp2());
	return	bResult;
}

bool	CSession :: IsEnableObjLightMap(const char *rswName)
{
	const char *key = rswName;
	std::vector<std::pair<const char*, bool> >::iterator it =
		std::lower_bound(m_enableObjLightMapList.begin(),
						 m_enableObjLightMapList.end(),
						 key, IListComp2());
	if (it != m_enableObjLightMapList.end()) {
		if (strcmp(it->first, rswName) == 0)
			return it->second;
	}
	return true;
}

const char *	CSession :: GetMp3Name(const char *rswName)
{
	string modifyRswName = rswName;
#if defined(__MEMORIAL_DUNGEON)
	string::size_type markPos = modifyRswName.rfind("@");
	if((markPos != string::npos) && (markPos >= 4)){
		modifyRswName.erase(markPos-4,3);
	}
#endif

	vector<StringPair>::iterator	it = lower_bound(m_mp3NameTable.begin(), m_mp3NameTable.end(), StringPair(modifyRswName.c_str(), modifyRswName.c_str()), SComp());
	if (it != m_mp3NameTable.end())
		if ((strcmp(it->first, modifyRswName.c_str()) == 0))
			return	it->second;

	return	"bgm\\01.mp3";
}

BOOL	CSession :: InitMp3NameTable()
{
	PCharList pCharList;
	PCharListIter it;
	m_mp3NameTable.clear();
	m_mp3NameTable.reserve(512);

	bool bResult = m_objParser.TokenFileToPCharList(pCharList,"mp3NameTable.txt");
	sAssert(pCharList.size() % 2 == 0 );

	for ( it = pCharList.begin(); it != pCharList.end(); it+=2 )
		m_mp3NameTable.push_back( StringPair(*it,*(it+1)) );

	sort(m_mp3NameTable.begin(), m_mp3NameTable.end(), SComp());
	return	bResult;
}

const char *	CSession :: GatName2RswName(const char *gatName)
{
	static string	rswName;

	rswName = gatName;
	if (rswName.size() >= 5) {
		rswName.resize(rswName.size()-3);
		rswName += "rsw";
	}

	return	rswName.c_str();
}

BOOL	CSession :: InitIndoorRswTable()
{
	m_objParser.TokenFileToPCharList(m_indoorRswTable,"indoorRswTable.txt");
	sort(m_indoorRswTable.begin(), m_indoorRswTable.end(), LPCSTRComp());
	return	TRUE;
}

BOOL	CSession :: InitViewPointTable()
{
	m_ViewPointTable.clear();

	CFile	fp;
	if (!fp.Open("ViewPointTable.txt", CFile::modeRead))
		return	false;

	char *	buf = (char *)fp.GetBuf();
	char *	buf_end = (char *)(buf + fp.GetLength());

	char *		base = buf;
	StringList	tmpList;
	string		a;

	for (;buf<buf_end; ++buf)
	{
		switch (*buf)
		{
		case	'#':
			if (a.empty())
			{
				a.assign(base, buf);
			}
			else if (tmpList.size() < 8)
			{
				tmpList.push_back(string(base, buf));
			}
			else
			{
				tmpList.push_back(string(base, buf));
				m_ViewPointTable.push_back(StringListPair(a, tmpList));
				a.erase();
				tmpList.clear();
			}
			base = buf+1;
			break;
		case	10:
		case	13:
			base = buf+1;
			break;
		}
	}

	sort(m_ViewPointTable.begin(), m_ViewPointTable.end(), SListComp());
	fp.Close();
	return	TRUE;
}




StringList &	CSession :: GetViewPointParameter(const char *rswName, BOOL &isExist)
{
	isExist = FALSE;
	static StringList	tmpFloatList(9);

	string modifyRswName = rswName;
#if defined(__MEMORIAL_DUNGEON)
	string::size_type markPos = modifyRswName.rfind("@");
	if((markPos != string::npos) && (markPos >= 4)){
		modifyRswName.erase(markPos-4,3);
	}
#endif
	vector<StringListPair>::iterator	it = lower_bound(m_ViewPointTable.begin(), m_ViewPointTable.end(), modifyRswName.c_str(), SListComp());
	if (it != m_ViewPointTable.end()) {
		if ((strcmp(it->first.c_str(), modifyRswName.c_str())==0)) {
			isExist = TRUE;
			return	it->second;
		}
	}











	return	tmpFloatList;
}

bool	CSession :: IsIndoorRswName(const char * rswName)
{
	string modifyRswName = rswName;
#if defined(__MEMORIAL_DUNGEON)
	string::size_type markPos = modifyRswName.rfind("@");
	if((markPos != string::npos) && (markPos >= 4)){
		modifyRswName.erase(markPos-4,3);
	}
#endif

	PCharListIter	it = lower_bound(m_indoorRswTable.begin(), m_indoorRswTable.end(), modifyRswName.c_str(), LPCSTRComp());
	if (it != m_indoorRswTable.end())
		if ((strcmp(*it, modifyRswName.c_str())==0))
			return	true;







	return	false;
}

BOOL	CSession :: InitFogParameterTable()
{
	m_fogParameterTable.clear();

	CFile	fp;
	if (!fp.Open("fogParameterTable.txt", CFile::modeRead))
		return	false;

	char *	buf = (char *)fp.GetBuf();
	char *	buf_end = (char *)(buf + fp.GetLength());

	char *		base = buf;
	string		a;
	StringList	tmpList;

	for (;buf<buf_end; ++buf) {
		switch (*buf) {
			case	'#':
				if (a.empty()) {
					a.assign(base, buf);
				} else if (tmpList.size() < 3) {
					tmpList.push_back(string(base, buf));
				} else {
					tmpList.push_back(string(base, buf));
					m_fogParameterTable.push_back(StringListPair(a, tmpList));
					a.erase();
					tmpList.clear();
				}
				base = buf+1;
				break;
			case	10:
			case	13:
				base = buf+1;
				break;
		}
	}

	sort(m_fogParameterTable.begin(), m_fogParameterTable.end(), SListComp());
	fp.Close();
	return	TRUE;
}

StringList &	CSession :: GetFogParameter(const char *rswName, BOOL &isExist)
{
	isExist = FALSE;
	static StringList	tmpFloatList(4);

	string modifyRswName = rswName;
#if defined(__MEMORIAL_DUNGEON)
	string::size_type markPos = modifyRswName.rfind("@");
	if((markPos != string::npos) && (markPos >= 4)){
		modifyRswName.erase(markPos-4,3);
	}
#endif

	vector<StringListPair>::iterator	it = lower_bound(m_fogParameterTable.begin(), m_fogParameterTable.end(), modifyRswName.c_str(), SListComp());
	if (it != m_fogParameterTable.end()) {
		if ((strcmp(it->first.c_str(), modifyRswName.c_str())==0)) {
			isExist = TRUE;
			return	it->second;
		}
	}









	return	tmpFloatList;
}

bool	CSession :: IsOnlyEnglish(const char *chat)
{
	int		size = strlen(chat);
	for (int i=0; i<size; ++i) {
		if (chat[i] & 0x80)
			return	false;
	}
	return	true;
}

bool	CSession :: IsBadNameWithSpaceChar(const char *chat)
{
	int		size = strlen(chat);
	if (size == 0)
		return	true;
	if (chat[0] == ' ')
		return	true;
	if (chat[size-1] == ' ')
		return	true;
	return	false;
}

extern DWORD g_codePage;

const char *	CSession :: RemoveUnderbar(const char *msg, char *buf)
{
	if ( buf != msg )
		strcpy(buf, msg);

	char* textIter=buf;

	while(*textIter) {
		if(*textIter == '_')
			*textIter = ' ';
		textIter = CharNextExA(g_codePage, textIter, 0);
	}

	return	buf;
}

int		CSession :: GetAttackEffectIdBySkillId(int skillId)
{
	if (skillId>=0 && skillId<0x40f)
		return	m_skillAttackEffectIdTable[skillId];
	return	-1;
}

int		CSession :: GetAttackedEffectIdBySkillId(int skillId)
{
	if (skillId>=0 && skillId<0x40f)
		return	m_skillAttackedEffectIdTable[skillId];
	return	-1;
}

void	CSession :: InitJobHitWaveName()
{
	m_jobHitWaveNameTable.clear();
	m_jobHitWaveNameTable.resize(JT_LAST-JOBMINUS, "player_clothes.wav");
	m_jobHitWaveNameTable[JT_NOVICE]				= "player_clothes.wav";

	m_jobHitWaveNameTable[JT_GUNSLINGER]			= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_NINJA]					= "player_wooden_male.wav";

	m_jobHitWaveNameTable[JT_MERCHANT]				= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_ACOLYTE]				= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_MAGICIAN]				= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_BLACKSMITH]			= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_PRIEST]				= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_WIZARD]				= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_ARCHER]				= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_THIEF]					= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_HUNTER]				= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_ASSASSIN]				= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_SWORDMAN]				= "player_metal.wav";
	m_jobHitWaveNameTable[JT_KNIGHT]				= "player_metal.wav";

	m_jobHitWaveNameTable[JT_SAGE]					= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_BARD]					= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_DANCER]				= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_ROGUE]					= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_CRUSADER]				= "player_metal.wav";
	m_jobHitWaveNameTable[JT_MONK]					= "player_metal.wav";
	m_jobHitWaveNameTable[JT_ALCHEMIST]				= "player_clothes.wav";

	m_jobHitWaveNameTable[JT_SUPERNOVICE]			= "player_clothes.wav";

	m_jobHitWaveNameTable[JT_NOVICE_H-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_MERCHANT_H-JOBMINUS]	= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_ACOLYTE_H-JOBMINUS]	= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_MAGICIAN_H-JOBMINUS]	= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_BLACKSMITH_H-JOBMINUS] = "player_clothes.wav";
	m_jobHitWaveNameTable[JT_PRIEST_H-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_WIZARD_H-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_ARCHER_H-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_THIEF_H-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_HUNTER_H-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_ASSASSIN_H-JOBMINUS]	= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_SWORDMAN_H-JOBMINUS]	= "player_metal.wav";
	m_jobHitWaveNameTable[JT_KNIGHT_H-JOBMINUS]		= "player_metal.wav";

	m_jobHitWaveNameTable[JT_SAGE_H-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_BARD_H-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_DANCER_H-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_ROGUE_H-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_CRUSADER_H-JOBMINUS]	= "player_metal.wav";
	m_jobHitWaveNameTable[JT_MONK_H-JOBMINUS]		= "player_metal.wav";
	m_jobHitWaveNameTable[JT_ALCHEMIST_H-JOBMINUS]	= "player_clothes.wav";

	m_jobHitWaveNameTable[JT_NOVICE_B-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_MERCHANT_B-JOBMINUS]	= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_ACOLYTE_B-JOBMINUS]	= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_MAGICIAN_B-JOBMINUS]	= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_BLACKSMITH_B-JOBMINUS] = "player_clothes.wav";
	m_jobHitWaveNameTable[JT_PRIEST_B-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_WIZARD_B-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_ARCHER_B-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_THIEF_B-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_HUNTER_B-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_ASSASSIN_B-JOBMINUS]	= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_SWORDMAN_B-JOBMINUS]	= "player_metal.wav";
	m_jobHitWaveNameTable[JT_KNIGHT_B-JOBMINUS]		= "player_metal.wav";

	m_jobHitWaveNameTable[JT_GANGSI-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_DEATHKNIGHT-JOBMINUS]	= "player_metal.wav";
	m_jobHitWaveNameTable[JT_COLLECTOR-JOBMINUS]	= "player_clothes.wav";

	m_jobHitWaveNameTable[JT_SAGE_B-JOBMINUS]		= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_BARD_B-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_DANCER_B-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_ROGUE_B-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_CRUSADER_B-JOBMINUS]	= "player_metal.wav";
	m_jobHitWaveNameTable[JT_MONK_B-JOBMINUS]		= "player_metal.wav";
	m_jobHitWaveNameTable[JT_ALCHEMIST_B-JOBMINUS]	= "player_clothes.wav";
	m_jobHitWaveNameTable[JT_SUPERNOVICE_B-JOBMINUS] = "player_clothes.wav";

	m_jobHitWaveNameTable[JT_TAEKWON-JOBMINUS]		= "player_wooden_male.wav";
	m_jobHitWaveNameTable[JT_STAR-JOBMINUS]			= "player_metal.wav";
	m_jobHitWaveNameTable[JT_STAR2-JOBMINUS]		= "player_metal.wav";
	m_jobHitWaveNameTable[JT_LINKER-JOBMINUS]		= "player_clothes.wav";
}

const	char *	CSession :: GetJobHitWaveName(int job)
{
	if (job >= JT_USER_LAST && job <= JT_2004_JOB_BEGIN || job < JT_NOVICE) {
		return GetWeaponHitWaveName(-1);
	}

	if (job > 3950)
		return m_jobHitWaveNameTable[job-3950].c_str();
	return m_jobHitWaveNameTable[job].c_str();
}

void	CSession :: InitWeaponHitWaveName()
{
	m_weaponHitWaveNameTable.clear();
	m_weaponHitWaveNameTable.resize(WEAPONTYPE_LAST, "_hit_mace.wav");
	m_weaponHitWaveNameTable[WEAPONTYPE_NONE]			= "_hit_mace.wav";

	m_weaponHitWaveNameTable[WPCLASS_GUN_HANDGUN]		= "_hit_\xb1\xc7\xc3\xd1.wav";
	m_weaponHitWaveNameTable[WPCLASS_GUN_RIFLE]			= "_hit_\xb6\xf3\xc0\xcc\xc7\xc3.wav";

	m_weaponHitWaveNameTable[WEAPONTYPE_SHORTSWORD]		= "_hit_sword.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_SWORD]			= "_hit_sword.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_TWOHANDSWORD]	= "_hit_sword.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_BOW]			= "_hit_arrow.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_SPEAR]			= "_hit_spear.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_TWOHANDSPEAR]	= "_hit_spear.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_AXE]			= "_hit_axe.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_TWOHANDAXE]		= "_hit_axe.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_MACE]			= "_hit_mace.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_TWOHANDMACE]	= "_hit_mace.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_ROD]			= "_hit_mace.wav";
	m_weaponHitWaveNameTable[WPCLASS_TWOHANDROD]		= "_hit_mace.wav";
	m_weaponHitWaveNameTable[WEAPONTYPE_BOOK]			= "_hit_mace.wav";
}

const char *	CSession :: GetWeaponHitWaveName(int weapon)
{
	if (weapon == -1) {
		int		randNum = random(4);
		switch (randNum) {
			case	0:
				return	"_enemy_hit_normal1.wav";
				break;
			case	1:
				return	"_enemy_hit_normal2.wav";
				break;
			case	2:
				return	"_enemy_hit_normal3.wav";
				break;
			case	3:
				return	"_enemy_hit_normal4.wav";
				break;
		}
	}






	{
		return	m_weaponHitWaveNameTable[weapon].c_str();
	}

	if (WEAPONTYPE_NONE == weapon)
	{
		int		randNum = random(4);
		switch (randNum) {
			case	0:
				return	"_hit_fist1.wav";
			case	1:
				return	"_hit_fist2.wav";
			case	2:
				return	"_hit_fist3.wav";
			case	3:
				return	"_hit_fist4.wav";
		}
	}
	return	m_weaponHitWaveNameTable[weapon].c_str();
}

void	CSession :: InitWeaponSwingWaveName()
{
	m_weaponSwingWaveNameTable.clear();
	m_weaponSwingWaveNameTable.resize(WEAPONTYPE_LAST, "_attack_mace.wav");
	m_weaponSwingWaveNameTable[WEAPONTYPE_NONE]			= "_attack_mace.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_ROD]			= "_attack_mace.wav";
	m_weaponSwingWaveNameTable[WPCLASS_TWOHANDROD]		= "_attack_mace.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_SHORTSWORD]	= "_attack_sword.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_SWORD]		= "_attack_sword.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_TWOHANDSWORD]	= "_attack_sword.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_BOW]			= "_attack_bow.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_SPEAR]		= "_attack_spear.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_TWOHANDSPEAR]	= "_attack_spear.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_AXE]			= "_attack_axe.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_TWOHANDAXE]	= "_attack_axe.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_MACE]			= "_attack_mace.wav";
	m_weaponSwingWaveNameTable[WEAPONTYPE_TWOHANDMACE]	= "_attack_mace.wav";
}

const char *	CSession :: GetWeaponSwingWaveName(int weapon)
{
	if (weapon == -1) {
		int		randNum = random(4);
		switch (randNum) {
			case	0:
				return	"_enemy_hit_normal1.wav";
				break;
			case	1:
				return	"_enemy_hit_normal2.wav";
				break;
			case	2:
				return	"_enemy_hit_normal3.wav";
				break;
			case	3:
				return	"_enemy_hit_normal4.wav";
				break;
		}
	}
	return	m_weaponSwingWaveNameTable[weapon].c_str();
}

const char *	CSession :: GetAttrWaveName(int attr)
{
	switch (attr) {
		case	ATTR_FIRE: {
			int		randNum = random(2);
			switch (randNum) {
				case	0:
					return	"_enemy_hit_fire1.wav";
				case	1:
					return	"_enemy_hit_fire2.wav";
			}
			break;
		}
		case	ATTR_WIND: {
			int		randNum = random(2);
			switch (randNum) {
				case	0:
					return	"_enemy_hit_wind1.wav";
				case	1:
					return	"_enemy_hit_wind2.wav";
			}
			break;
		}
	}

	int		randNum = random(4);
	switch (randNum) {
		case	0:
			return	"_enemy_hit_normal1.wav";
		case	1:
			return	"_enemy_hit_normal2.wav";
		case	2:
			return	"_enemy_hit_normal3.wav";
		case	3:
			return	"_enemy_hit_normal4.wav";
	}
	return	"_enemy_hit_normal4.wav";
}

float	CSession :: GetPCAttackMotion(int job, int sex, int weapon, BOOL isSecondAttack)
{
	switch (isSecondAttack) {
		case	0:
			switch (job) {
		case	JT_THIEF:
			return	5.75f;
			break;
		case	JT_MERCHANT:
			return	5.85f;
			break;
			}
			break;
		case	1:
			switch (job) {
		case	JT_NOVICE:
		case	JT_SUPERNOVICE:
		case	JT_SUPERNOVICE_B:
			switch (sex) {
		case	SEX_MALE:
			return	5.85f;
			break;
			}
			break;
		case	JT_ASSASSIN:
		case	JT_ASSASSIN_H:
		case	JT_ASSASSIN_B:
			switch (weapon) {
		case	WEAPONTYPE_CATARRH:
		case	WEAPONTYPE_SHORTSWORD_SHORTSWORD:
		case	WEAPONTYPE_SWORD_SWORD:
		case	WEAPONTYPE_AXE_AXE:
		case	WEAPONTYPE_SHORTSWORD_SWORD:
		case	WEAPONTYPE_SHORTSWORD_AXE:
		case	WEAPONTYPE_SWORD_AXE:
			return	3.0f;
			}
			break;
			}
			break;
	}
	return	6;
}


bool	CSession :: IsSecondAttack(int job, int sex, int weapon)
{
	bool	isSecondAttack = false;
	if (weapon >= WEAPONTYPE_LAST)
	{
		if (job == JT_ASSASSIN || job == JT_ASSASSIN_H || job == JT_ASSASSIN_B) {
			int right, left;
			left = weapon & 0xFFFF;
			right = (weapon >> 16) & 0xFFFF;
			if (g_session.GetWeaponType(right) > 0) {
				weapon = MakeWeaponType(left, right);
			} else {
				weapon = GetWeaponType((weapon & 0xffff));
			}
		} else {
			weapon = GetWeaponType((weapon & 0xffff));
		}
	}
	switch (job) {
		case	JT_GUNSLINGER:
			{
				switch (weapon)
				{
				case	WPCLASS_GUN_RIFLE:
				case	WPCLASS_GUN_GATLING:
				case	WPCLASS_GUN_SHOTGUN:
				case	WPCLASS_GUN_GRANADE:
					isSecondAttack = true;
					break;
				}
				break;
			}
		case	JT_NINJA:
			{
				switch (weapon)
				{
				case	WPCLASS_SYURIKEN:
					isSecondAttack = true;
					break;
				}
				break;
			}
		case	JT_GANGSI:
		case	JT_DEATHKNIGHT:
		case	JT_COLLECTOR:
			{

				break;
			}
		case	JT_TAEKWON:
		case	JT_STAR:
		case	JT_STAR2:
			{
				break;
			}
		case	JT_LINKER:
			{
				switch (weapon)
				{
				case	WEAPONTYPE_SHORTSWORD:
					if(sex == SEX_MALE) isSecondAttack = true;
					break;
				case	WEAPONTYPE_ROD:
				case	WPCLASS_TWOHANDROD:
					if(sex == SEX_FEMALE) isSecondAttack = true;
					break;
				}
				break;
			}
		case	JT_SWORDMAN:
		case	JT_SWORDMAN_H:
		case	JT_SWORDMAN_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SPEAR:
		case	WEAPONTYPE_TWOHANDSPEAR:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_ARCHER:
		case	JT_ARCHER_H:
		case	JT_ARCHER_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_BOW:
			break;
		default:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_THIEF:
		case	JT_THIEF_H:
		case	JT_THIEF_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_BOW:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_MAGICIAN:
		case	JT_MAGICIAN_H:
		case	JT_MAGICIAN_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SHORTSWORD:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_MERCHANT:
		case	JT_MERCHANT_H:
		case	JT_MERCHANT_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SHORTSWORD:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_ACOLYTE:
		case	JT_ACOLYTE_H:
		case	JT_ACOLYTE_B:
			{
				break;
			}
		case	JT_NOVICE:
		case	JT_NOVICE_H:
		case	JT_SUPERNOVICE:
		case	JT_NOVICE_B:
		case	JT_SUPERNOVICE_B:
			{
				switch (sex) {
		case	SEX_FEMALE:
			switch (weapon) {
		case	WEAPONTYPE_SWORD:
		case	WEAPONTYPE_TWOHANDSWORD:
		case	WEAPONTYPE_AXE:
		case	WEAPONTYPE_TWOHANDAXE:
		case	WEAPONTYPE_ROD:
		case	WPCLASS_TWOHANDROD:
		case	WEAPONTYPE_MACE:
		case	WEAPONTYPE_TWOHANDMACE:
			break;
		case	WEAPONTYPE_SHORTSWORD:
			isSecondAttack = true;
			break;
			}
			break;
		case	SEX_MALE:
			switch (weapon) {
		case	WEAPONTYPE_SWORD:
		case	WEAPONTYPE_TWOHANDSWORD:
		case	WEAPONTYPE_AXE:
		case	WEAPONTYPE_TWOHANDAXE:
		case	WEAPONTYPE_ROD:
		case	WPCLASS_TWOHANDROD:
		case	WEAPONTYPE_MACE:
		case	WEAPONTYPE_TWOHANDMACE:
			isSecondAttack = true;
			break;
		case	WEAPONTYPE_SHORTSWORD:
			break;
			}
			break;
				}
				break;
			}
		case	JT_KNIGHT:
		case	JT_CHICKEN:
		case	JT_KNIGHT_H:
		case	JT_CHICKEN_H:
		case	JT_KNIGHT_B:
		case	JT_CHICKEN_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SPEAR:
		case	WEAPONTYPE_TWOHANDSPEAR:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_PRIEST:
		case	JT_PRIEST_H:
		case	JT_PRIEST_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_BOOK:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_WIZARD:
		case	JT_WIZARD_H:
		case	JT_WIZARD_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SHORTSWORD:
			if(sex == SEX_MALE) isSecondAttack = true;
			break;
		case	WEAPONTYPE_ROD:
		case	WPCLASS_TWOHANDROD:
			if(sex == SEX_FEMALE) isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_BLACKSMITH:
		case	JT_BLACKSMITH_H:
		case	JT_BLACKSMITH_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SWORD:
		case	WEAPONTYPE_AXE:
		case	WEAPONTYPE_TWOHANDAXE:
		case	WEAPONTYPE_MACE:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_ASSASSIN:
		case	JT_ASSASSIN_H:
		case	JT_ASSASSIN_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_CATARRH:
		case	WEAPONTYPE_SHORTSWORD_SHORTSWORD:
		case	WEAPONTYPE_SHORTSWORD_SWORD:
		case	WEAPONTYPE_SHORTSWORD_AXE:
		case	WEAPONTYPE_SWORD_SWORD:
		case	WEAPONTYPE_SWORD_AXE:
		case	WEAPONTYPE_AXE_AXE:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_HUNTER:
		case	JT_HUNTER_H:
		case	JT_HUNTER_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_BOW:
			isSecondAttack = true;
			break;
				}
				break;
			}
		case	JT_SAGE:
		case	JT_SAGE_H:
		case	JT_SAGE_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_BOOK:
		case	WEAPONTYPE_ROD:
		case	WPCLASS_TWOHANDROD:
		case	WEAPONTYPE_TWOHANDSPEAR:
			isSecondAttack = true;
			break;
				}
			}
			break;
		case	JT_ALCHEMIST:
		case	JT_ALCHEMIST_H:
		case	JT_ALCHEMIST_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SWORD:
		case	WEAPONTYPE_AXE:
		case	WEAPONTYPE_TWOHANDAXE:
		case	WEAPONTYPE_MACE:
			isSecondAttack = true;
			break;
				}
			}
			break;
		case	JT_CHICKEN2:
		case	JT_CRUSADER:
		case	JT_CHICKEN2_H:
		case	JT_CRUSADER_H:
		case	JT_CRUSADER_B:
		case	JT_CHICKEN2_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_SPEAR:
		case	WEAPONTYPE_TWOHANDSPEAR:
			isSecondAttack = true;
			break;
				}
			}
			break;
		case	JT_MONK:
		case	JT_MONK_H:
		case	JT_MONK_B:
			{
				switch(weapon) {
		case	WEAPONTYPE_KNUKLE:
		case	WEAPONTYPE_NONE:
			isSecondAttack = true;
			break;
				}
			}
			break;
		case	JT_ROGUE:
		case	JT_ROGUE_H:
		case	JT_ROGUE_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_BOW:
			isSecondAttack = true;
			break;
				}
			}
			break;
		case	JT_BARD:
		case	JT_DANCER:
		case	JT_BARD_H:
		case	JT_DANCER_H:
		case	JT_BARD_B:
		case	JT_DANCER_B:
			{
				switch (weapon) {
		case	WEAPONTYPE_BOW:
			isSecondAttack = true;
			break;
				}
			}
			break;
	}
	return	isSecondAttack;
}

void	CSession :: InitShadowFactorWithJob()
{
	m_shadowFactorTable.clear();
	m_shadowFactorTable.resize(JT_LAST, 1.0f);
	m_shadowFactorTable[JT_NOVICE] = 1.0f;
	m_shadowFactorTable[JT_SWORDMAN] = 1.0f;
	m_shadowFactorTable[JT_MAGICIAN] = 1.0f;
	m_shadowFactorTable[JT_ARCHER] = 1.0f;
	m_shadowFactorTable[JT_ACOLYTE] = 1.0f;
	m_shadowFactorTable[JT_MERCHANT] = 1.0f;
	m_shadowFactorTable[JT_THIEF] = 1.0f;
	m_shadowFactorTable[JT_KNIGHT] = 1.0f;
	m_shadowFactorTable[JT_WIZARD] = 1.0f;
	m_shadowFactorTable[JT_ASSASSIN] = 1.0f;
	m_shadowFactorTable[JT_BLACKSMITH] = 1.0f;
	m_shadowFactorTable[JT_PRIEST] = 1.0f;
	m_shadowFactorTable[JT_HUNTER] = 1.0f;
	m_shadowFactorTable[JT_CHICKEN] = 1.5f;
	m_shadowFactorTable[JT_CRUSADER] = 1.0f;
	m_shadowFactorTable[JT_MONK] = 1.0f;
	m_shadowFactorTable[JT_ROGUE] = 1.0f;
	m_shadowFactorTable[JT_ALCHEMIST] = 1.0f;
	m_shadowFactorTable[JT_DANCER] = 1.0f;
	m_shadowFactorTable[JT_BARD] = 1.0f;
	m_shadowFactorTable[JT_CHICKEN2] = 1.5f;
	m_shadowFactorTable[JT_SUPERNOVICE] = 1.0f;
	m_shadowFactorTable[JT_GUNSLINGER] = 1.0f;
	m_shadowFactorTable[JT_NINJA] = 1.0f;
	m_shadowFactorTable[JT_CHICKEN_H] = 1.5f;
	m_shadowFactorTable[JT_CHICKEN2_H] = 1.5f;
	m_shadowFactorTable[JT_NOVICE_B] = 0.75f;
	m_shadowFactorTable[JT_SWORDMAN_B] = 0.75f;
	m_shadowFactorTable[JT_MAGICIAN_B] = 0.75f;
	m_shadowFactorTable[JT_ARCHER_B] = 0.75f;
	m_shadowFactorTable[JT_ACOLYTE_B] = 0.75f;
	m_shadowFactorTable[JT_MERCHANT_B] = 0.75f;
	m_shadowFactorTable[JT_THIEF_B] = 0.75f;
	m_shadowFactorTable[JT_KNIGHT_B] = 0.82f;
	m_shadowFactorTable[JT_WIZARD_B] = 0.82f;
	m_shadowFactorTable[JT_ASSASSIN_B] = 0.82f;
	m_shadowFactorTable[JT_BLACKSMITH_B] = 0.82f;
	m_shadowFactorTable[JT_PRIEST_B] = 0.82f;
	m_shadowFactorTable[JT_HUNTER_B] = 0.82f;
	m_shadowFactorTable[JT_CHICKEN_B] = 1.0f;
	m_shadowFactorTable[JT_CHICKEN2_B] = 1.0f;
	m_shadowFactorTable[JT_CRUSADER_B] = 0.82f;
	m_shadowFactorTable[JT_MONK_B] = 0.82f;
	m_shadowFactorTable[JT_ROGUE_B] = 0.82f;
	m_shadowFactorTable[JT_ALCHEMIST_B] = 0.82f;
	m_shadowFactorTable[JT_DANCER_B] = 0.82f;
	m_shadowFactorTable[JT_BARD_B] = 0.82f;
	m_shadowFactorTable[JT_SUPERNOVICE_B] = 0.75f;
	m_shadowFactorTable[JT_WARPNPC] = 1.0f;
	m_shadowFactorTable[JT_1_ETC_01] = 1.0f;
	m_shadowFactorTable[JT_1_M_01] = 1.0f;
	m_shadowFactorTable[JT_1_M_02] = 1.0f;
	m_shadowFactorTable[JT_1_M_03] = 1.0f;
	m_shadowFactorTable[JT_1_M_04] = 1.0f;
	m_shadowFactorTable[JT_1_M_BARD] = 1.0f;
	m_shadowFactorTable[JT_1_M_HOF] = 1.0f;
	m_shadowFactorTable[JT_1_M_INNKEEPER] = 1.0f;
	m_shadowFactorTable[JT_1_M_JOBGUIDER] = 1.0f;
	m_shadowFactorTable[JT_1_M_JOBTESTER] = 1.0f;
	m_shadowFactorTable[JT_1_M_KNIGHTMASTER] = 1.0f;
	m_shadowFactorTable[JT_1_M_LIBRARYMASTER] = 1.0f;
	m_shadowFactorTable[JT_1_M_MERCHANT] = 1.0f;
	m_shadowFactorTable[JT_1_M_ORIENT01] = 1.0f;
	m_shadowFactorTable[JT_1_M_PASTOR] = 1.0f;
	m_shadowFactorTable[JT_1_M_PUBMASTER] = 1.0f;
	m_shadowFactorTable[JT_1_M_SIZ] = 1.0f;
	m_shadowFactorTable[JT_1_M_SMITH] = 1.0f;
	m_shadowFactorTable[JT_1_M_WIZARD] = 1.0f;
	m_shadowFactorTable[JT_1_M_YOUNGKNIGHT] = 1.0f;
	m_shadowFactorTable[JT_1_F_01] = 1.0f;
	m_shadowFactorTable[JT_1_F_02] = 1.0f;
	m_shadowFactorTable[JT_1_F_03] = 1.0f;
	m_shadowFactorTable[JT_1_F_04] = 1.0f;
	m_shadowFactorTable[JT_1_F_GYPSY] = 1.0f;
	m_shadowFactorTable[JT_1_F_LIBRARYGIRL] = 1.0f;
	m_shadowFactorTable[JT_1_F_MARIA] = 1.0f;
	m_shadowFactorTable[JT_1_F_MERCHANT_01] = 1.0f;
	m_shadowFactorTable[JT_1_F_MERCHANT_02] = 1.0f;
	m_shadowFactorTable[JT_1_F_ORIENT_01] = 1.0f;
	m_shadowFactorTable[JT_1_F_ORIENT_02] = 1.0f;
	m_shadowFactorTable[JT_1_F_ORIENT_03] = 1.0f;
	m_shadowFactorTable[JT_1_F_ORIENT_04] = 1.0f;
	m_shadowFactorTable[JT_1_F_PRIEST] = 1.0f;
	m_shadowFactorTable[JT_1_F_PUBGIRL] = 1.0f;
	m_shadowFactorTable[JT_4_M_MANAGER] = 1.0f;
	m_shadowFactorTable[JT_4_M_MINISTER] = 1.0f;
	m_shadowFactorTable[JT_4_DOG01] = 1.0f;
	m_shadowFactorTable[JT_4_KID01] = 1.0f;
	m_shadowFactorTable[JT_4_M_01] = 1.0f;
	m_shadowFactorTable[JT_4_M_02] = 1.0f;
	m_shadowFactorTable[JT_4_M_03] = 1.0f;
	m_shadowFactorTable[JT_4_M_04] = 1.0f;
	m_shadowFactorTable[JT_4_M_BARBER] = 1.0f;
	m_shadowFactorTable[JT_4_M_ORIENT01] = 1.0f;
	m_shadowFactorTable[JT_4_M_ORIENT02] = 1.0f;
	m_shadowFactorTable[JT_4_F_01] = 1.0f;
	m_shadowFactorTable[JT_4_F_02] = 1.0f;
	m_shadowFactorTable[JT_4_F_03] = 1.0f;
	m_shadowFactorTable[JT_4_F_04] = 1.0f;
	m_shadowFactorTable[JT_4_F_MAID] = 1.0f;
	m_shadowFactorTable[JT_4_F_SISTER] = 1.0f;
	m_shadowFactorTable[JT_4W_KID] = 1.0f;
	m_shadowFactorTable[JT_4W_M_01] = 1.0f;
	m_shadowFactorTable[JT_4W_M_02] = 1.0f;
	m_shadowFactorTable[JT_4W_M_03] = 1.0f;
	m_shadowFactorTable[JT_4W_SAILOR] = 1.0f;
	m_shadowFactorTable[JT_4W_F_01] = 1.0f;
	m_shadowFactorTable[JT_8_F] = 1.0f;
	m_shadowFactorTable[JT_8_F_GRANDMOTHER] = 1.0f;
	m_shadowFactorTable[JT_EFFECTLAUNCHER] = 1.0f;
	m_shadowFactorTable[JT_8W_SOLDIER] = 1.0f;
	m_shadowFactorTable[JT_1_M_MOC_LORD] = 1.0f;
	m_shadowFactorTable[JT_1_M_PAY_ELDER] = 1.0f;
	m_shadowFactorTable[JT_1_M_PRON_KING] = 1.0f;
	m_shadowFactorTable[JT_HIDDEN_NPC] = 0.0f;
	m_shadowFactorTable[JT_SAFETYWALL] = 1.0f;
	m_shadowFactorTable[JT_FIREWALL] = 1.0f;
	m_shadowFactorTable[JT_ICEWALL] = 1.0f;
	m_shadowFactorTable[JT_WARPPORTAL] = 1.0f;
	m_shadowFactorTable[JT_PREWARPPORTAL] = 1.0f;
	m_shadowFactorTable[JT_BENEDICTIO] = 1.0f;
	m_shadowFactorTable[JT_SANCTUARY] = 1.0f;
	m_shadowFactorTable[JT_MAGNUS] = 1.0f;
	m_shadowFactorTable[JT_MON_BEGIN] = 1.0f;
	m_shadowFactorTable[JT_SCORPION] = 1.0f;
	m_shadowFactorTable[JT_PORING] = 1.0f;
	m_shadowFactorTable[JT_THIEF_BUG_AGG] = 1.0f;
	m_shadowFactorTable[JT_HORNET] = 0.5f;
	m_shadowFactorTable[JT_FARMILIAR] = 0.5f;
	m_shadowFactorTable[JT_THIEF_BUG_LARVA] = 1.0f;
	m_shadowFactorTable[JT_FABRE] = 0.5f;
	m_shadowFactorTable[JT_PUPA] = 0.3f;
	m_shadowFactorTable[JT_CONDOR] = 1.0f;
	m_shadowFactorTable[JT_WILOW] = 1.0f;
	m_shadowFactorTable[JT_CHONCHON] = 0.5f;
	m_shadowFactorTable[JT_RODA_FROG] = 1.0f;
	m_shadowFactorTable[JT_WOLF] = 1.0f;
	m_shadowFactorTable[JT_SPORE] = 1.0f;
	m_shadowFactorTable[JT_ZOMBIE] = 1.0f;
	m_shadowFactorTable[JT_ARCHER_SKELETON] = 1.0f;
	m_shadowFactorTable[JT_THIEF_BUG_FEMALE] = 1.0f;
	m_shadowFactorTable[JT_CREAMY] = 1.0f;
	m_shadowFactorTable[JT_PECOPECO] = 1.0f;
	m_shadowFactorTable[JT_MANDRAGORA] = 1.0f;
	m_shadowFactorTable[JT_THIEF_BUG_MALE] = 1.0f;
	m_shadowFactorTable[JT_WERE_WOLF] = 1.0f;
	m_shadowFactorTable[JT_ORK_WARRIOR] = 1.0f;
	m_shadowFactorTable[JT_WORM_TAIL] = 1.0f;
	m_shadowFactorTable[JT_SNAKE] = 0.0f;
	m_shadowFactorTable[JT_MUNAK] = 1.0f;
	m_shadowFactorTable[JT_RAPTICE] = 1.0f;
	m_shadowFactorTable[JT_SOLDIER_SKELETON] = 1.0f;
	m_shadowFactorTable[JT_ISIS] = 1.0f;
	m_shadowFactorTable[JT_ANACONDAQ] = 0.0f;
	m_shadowFactorTable[JT_POPORING] = 1.0f;
	m_shadowFactorTable[JT_VERIT] = 1.0f;
	m_shadowFactorTable[JT_ELDER_WILOW] = 1.0f;
	m_shadowFactorTable[JT_THARA_FROG] = 1.0f;
	m_shadowFactorTable[JT_HUNTER_FLY] = 0.5f;
	m_shadowFactorTable[JT_GHOUL] = 1.0f;
	m_shadowFactorTable[JT_SIDE_WINDER] = 0.0f;
	m_shadowFactorTable[JT_OSIRIS] = 1.0f;
	m_shadowFactorTable[JT_BAPHOMET] = 1.0f;
	m_shadowFactorTable[JT_GOLEM] = 2.0f;
	m_shadowFactorTable[JT_MUMMY] = 1.0f;
	m_shadowFactorTable[JT_STEEL_CHONCHON] = 0.5f;
	m_shadowFactorTable[JT_SEAHORES] = 1.0f;
	m_shadowFactorTable[JT_OBEAUNE] = 1.0f;
	m_shadowFactorTable[JT_MARC] = 1.0f;
	m_shadowFactorTable[JT_DOPPELGANGER] = 1.0f;
	m_shadowFactorTable[JT_PECOPECO_EGG] = 0.2f;
	m_shadowFactorTable[JT_THIEF_BUG_EGG] = 0.2f;
	m_shadowFactorTable[JT_PICKY] = 0.3f;
	m_shadowFactorTable[JT_PICKY_] = 0.3f;
	m_shadowFactorTable[JT_THIEF_BUG] = 0.3f;
	m_shadowFactorTable[JT_ROCKER] = 1.0f;
	m_shadowFactorTable[JT_THIEF_BUG_] = 1.0f;
	m_shadowFactorTable[JT_THIEF_BUG__] = 1.0f;
	m_shadowFactorTable[JT_MUKA] = 1.0f;
	m_shadowFactorTable[JT_SMOKIE] = 1.0f;
	m_shadowFactorTable[JT_YOYO] = 1.0f;
	m_shadowFactorTable[JT_METALLER] = 1.0f;
	m_shadowFactorTable[JT_MISTRESS] = 1.0f;
	m_shadowFactorTable[JT_BIGFOOT] = 1.0f;
	m_shadowFactorTable[JT_NIGHTMARE] = 1.0f;
	m_shadowFactorTable[JT_PORING_] = 1.0f;
	m_shadowFactorTable[JT_LUNATIC] = 1.0f;
	m_shadowFactorTable[JT_MEGALODON] = 1.0f;
	m_shadowFactorTable[JT_STROUF] = 1.0f;
	m_shadowFactorTable[JT_VADON] = 1.0f;
	m_shadowFactorTable[JT_CORNUTUS] = 1.0f;
	m_shadowFactorTable[JT_HYDRA] = 1.0f;
	m_shadowFactorTable[JT_SWORD_FISH] = 1.0f;
	m_shadowFactorTable[JT_KUKRE] = 0.1f;
	m_shadowFactorTable[JT_PIRATE_SKEL] = 1.0f;
	m_shadowFactorTable[JT_KAHO] = 1.0f;
	m_shadowFactorTable[JT_CRAB] = 1.0f;
	m_shadowFactorTable[JT_SHELLFISH] = 1.0f;
	m_shadowFactorTable[JT_TURTLE] = 1.0f;
	m_shadowFactorTable[JT_SKELETON] = 1.0f;
	m_shadowFactorTable[JT_POISON_SPORE] = 1.0f;
	m_shadowFactorTable[JT_AMBERNITE] = 1.0f;
	m_shadowFactorTable[JT_ANDRE] = 1.0f;
	m_shadowFactorTable[JT_ANGELING] = 1.0f;
	m_shadowFactorTable[JT_ANT_EGG] = 1.0f;
	m_shadowFactorTable[JT_ANUBIS] = 1.0f;
	m_shadowFactorTable[JT_ARGIOPE] = 1.0f;
	m_shadowFactorTable[JT_RED_PLANT] = 0.0f;
	m_shadowFactorTable[JT_BLUE_PLANT] = 0.0f;
	m_shadowFactorTable[JT_GREEN_PLANT] = 0.0f;
	m_shadowFactorTable[JT_YELLOW_PLANT] = 0.0f;
	m_shadowFactorTable[JT_WHITE_PLANT] = 0.0f;
	m_shadowFactorTable[JT_SHINING_PLANT] = 0.0f;
	m_shadowFactorTable[JT_BLACK_MUSHROOM] = 0.0f;
	m_shadowFactorTable[JT_RED_MUSHROOM] = 0.0f;
	m_shadowFactorTable[JT_GOLDEN_BUG] = 1.0f;
	m_shadowFactorTable[JT_ORK_HERO] = 1.0f;
	m_shadowFactorTable[JT_VOCAL] = 1.0f;
	m_shadowFactorTable[JT_TOAD] = 1.0f;
	m_shadowFactorTable[JT_MASTERING] = 1.0f;
	m_shadowFactorTable[JT_DRAGON_FLY] = 0.5f;
	m_shadowFactorTable[JT_VAGABOND_WOLF] = 1.0f;
	m_shadowFactorTable[JT_ECLIPSE] = 1.0f;
	m_shadowFactorTable[JT_ARGOS] = 1.0f;
	m_shadowFactorTable[JT_BAPHOMET_] = 1.0f;
	m_shadowFactorTable[JT_BATHORY] = 1.0f;
	m_shadowFactorTable[JT_CARAMEL] = 1.0f;
	m_shadowFactorTable[JT_COCO] = 1.0f;
	m_shadowFactorTable[JT_DENIRO] = 1.0f;
	m_shadowFactorTable[JT_DESERT_WOLF] = 1.0f;
	m_shadowFactorTable[JT_DESERT_WOLF_B] = 1.0f;
	m_shadowFactorTable[JT_DEVIACE] = 1.0f;
	m_shadowFactorTable[JT_DEVIRUCHI] = 1.0f;
	m_shadowFactorTable[JT_DOKEBI] = 1.0f;
	m_shadowFactorTable[JT_DRAINLIAR] = 1.0f;
	m_shadowFactorTable[JT_DRAKE] = 1.0f;
	m_shadowFactorTable[JT_DROPS] = 1.0f;
	m_shadowFactorTable[JT_DUSTINESS] = 1.0f;
	m_shadowFactorTable[JT_EDDGA] = 1.0f;
	m_shadowFactorTable[JT_EGGYRA] = 1.0f;
	m_shadowFactorTable[JT_EVIL_DRUID] = 1.0f;
	m_shadowFactorTable[JT_FLORA] = 1.0f;
	m_shadowFactorTable[JT_FRILLDORA] = 1.0f;
	m_shadowFactorTable[JT_GHOSTRING] = 1.0f;
	m_shadowFactorTable[JT_GIEARTH] = 1.0f;
	m_shadowFactorTable[JT_GOBLIN_1] = 1.0f;
	m_shadowFactorTable[JT_GOBLIN_2] = 1.0f;
	m_shadowFactorTable[JT_GOBLIN_3] = 1.0f;
	m_shadowFactorTable[JT_GOBLIN_4] = 1.0f;
	m_shadowFactorTable[JT_GOBLIN_5] = 1.0f;
	m_shadowFactorTable[JT_HODE] = 0.0f;
	m_shadowFactorTable[JT_HORN] = 1.0f;
	m_shadowFactorTable[JT_HORONG] = 1.0f;
	m_shadowFactorTable[JT_JAKK] = 1.0f;
	m_shadowFactorTable[JT_JOKER] = 1.0f;
	m_shadowFactorTable[JT_KHALITZBURG] = 1.0f;
	m_shadowFactorTable[JT_KOBOLD_1] = 1.0f;
	m_shadowFactorTable[JT_KOBOLD_2] = 1.0f;
	m_shadowFactorTable[JT_KOBOLD_3] = 1.0f;
	m_shadowFactorTable[JT_KOBOLD_4] = 1.0f;
	m_shadowFactorTable[JT_KOBOLD_5] = 1.0f;
	m_shadowFactorTable[JT_MAGNOLIA] = 1.0f;
	m_shadowFactorTable[JT_MANTIS] = 1.0f;
	m_shadowFactorTable[JT_MARDUK] = 1.0f;
	m_shadowFactorTable[JT_MARINA] = 1.0f;
	m_shadowFactorTable[JT_MARINE_SPHERE] = 1.0f;
	m_shadowFactorTable[JT_MARIONETTE] = 1.0f;
	m_shadowFactorTable[JT_MARSE] = 1.0f;
	m_shadowFactorTable[JT_MARTIN] = 1.0f;
	m_shadowFactorTable[JT_MATYR] = 1.0f;
	m_shadowFactorTable[JT_MAYA] = 1.0f;
	m_shadowFactorTable[JT_MEDUSA] = 1.0f;
	m_shadowFactorTable[JT_MINOROUS] = 1.0f;
	m_shadowFactorTable[JT_MOONLIGHT] = 1.0f;
	m_shadowFactorTable[JT_MYST] = 1.0f;
	m_shadowFactorTable[JT_ORC_SKELETON] = 1.0f;
	m_shadowFactorTable[JT_ORC_ZOMBIE] = 1.0f;
	m_shadowFactorTable[JT_PASANA] = 1.0f;
	m_shadowFactorTable[JT_PETIT] = 1.0f;
	m_shadowFactorTable[JT_PETIT_] = 1.0f;
	m_shadowFactorTable[JT_PHARAOH] = 1.0f;
	m_shadowFactorTable[JT_PHEN] = 1.0f;
	m_shadowFactorTable[JT_PHREEONI] = 1.0f;
	m_shadowFactorTable[JT_PIERE] = 1.0f;
	m_shadowFactorTable[JT_PLANKTON] = 1.0f;
	m_shadowFactorTable[JT_RAFFLESIA] = 1.0f;
	m_shadowFactorTable[JT_RAYDRIC] = 1.0f;
	m_shadowFactorTable[JT_REQUIEM] = 1.0f;
	m_shadowFactorTable[JT_SAND_MAN] = 1.0f;
	m_shadowFactorTable[JT_SAVAGE] = 1.0f;
	m_shadowFactorTable[JT_SAVAGE_BABE] = 0.5f;
	m_shadowFactorTable[JT_SCORPION_KING] = 1.0f;
	m_shadowFactorTable[JT_SKEL_WORKER] = 1.0f;
	m_shadowFactorTable[JT_SOHEE] = 1.0f;
	m_shadowFactorTable[JT_SOLDIER_ANDRE] = 1.0f;
	m_shadowFactorTable[JT_SOLDIER_DENIRO] = 1.0f;
	m_shadowFactorTable[JT_SOLDIER_PIERE] = 1.0f;
	m_shadowFactorTable[JT_STAINER] = 0.5f;
	m_shadowFactorTable[JT_TAROU] = 0.5f;
	m_shadowFactorTable[JT_VITATA] = 1.0f;
	m_shadowFactorTable[JT_ZENORC] = 1.0f;
	m_shadowFactorTable[JT_ZEROM] = 1.0f;
	m_shadowFactorTable[JT_WHISPER] = 1.0f;
	m_shadowFactorTable[JT_NINE_TAIL] = 1.0f;
	m_shadowFactorTable[JT_ZOMBIE_DRAGON] = 1.0f;
	m_shadowFactorTable[JT_THIEF_MUSHROOM] = 1.0f;
	m_shadowFactorTable[JT_CHONCHON_] = 1.0f;
	m_shadowFactorTable[JT_FABRE_] = 1.0f;
	m_shadowFactorTable[JT_WHISPER_] = 1.0f;
	m_shadowFactorTable[JT_WHISPER_BOSS] = 1.0f;
	m_shadowFactorTable[JT_SWITCH] = 1.0f;
	m_shadowFactorTable[JT_KARAKASA] = 1.0f;
	m_shadowFactorTable[JT_SHINOBI] = 1.6f;
	m_shadowFactorTable[JT_POISON_TOAD] = 1.0f;
	m_shadowFactorTable[JT_ANTIQUE_FIRELOCK] = 1.0f;
	m_shadowFactorTable[JT_MIYABI_NINGYO] = 1.0f;
	m_shadowFactorTable[JT_TENGU] = 1.0f;
	m_shadowFactorTable[JT_KAPHA] = 1.0f;
	m_shadowFactorTable[JT_DOKEBI_] = 1.0f;
	m_shadowFactorTable[JT_APOCALIPS] = 2.0f;
	m_shadowFactorTable[JT_APOCALIPS_H] = 0.0f;
	m_shadowFactorTable[JT_ORC_BABY] = 0.5f;
	m_shadowFactorTable[JT_ATROCE] = 2.0f;
	m_shadowFactorTable[JT_GOPINICH] = 2.0f;
	m_shadowFactorTable[JT_MOROCC] = 2.0f;
	m_shadowFactorTable[JT_4_F_KAFRA7] = 1.0f;
	m_shadowFactorTable[JT_4_F_KAFRA6] = 1.0f;
	m_shadowFactorTable[JT_4_F_KAFRA5] = 1.0f;
	m_shadowFactorTable[JT_4_F_KAFRA4] = 1.0f;
	m_shadowFactorTable[JT_4_F_KAFRA3] = 1.0f;
	m_shadowFactorTable[JT_4_F_KAFRA2] = 1.0f;
	m_shadowFactorTable[JT_4_F_KAFRA1] = 1.0f;
	m_shadowFactorTable[JT_2_M_THIEFMASTER] = 1.0f;
	m_shadowFactorTable[JT_2_M_SWORDMASTER] = 1.0f;
	m_shadowFactorTable[JT_2_M_PHARMACIST] = 1.0f;
	m_shadowFactorTable[JT_2_M_MOLGENSTEIN] = 1.0f;
	m_shadowFactorTable[JT_2_M_DYEINGER] = 1.0f;
	m_shadowFactorTable[JT_2_F_MAGICMASTER] = 1.0f;
	m_shadowFactorTable[JT_4_F_TELEPORTER] = 1.0f;
	m_shadowFactorTable[JT_4_M_TELEPORTER] = 1.0f;
	m_shadowFactorTable[JT_8_F_GIRL] = 1.0f;
	m_shadowFactorTable[JT_4_F_07] = 1.0f;
	m_shadowFactorTable[JT_4_F_JOB_ASSASSIN] = 1.0f;
	m_shadowFactorTable[JT_4_F_JOB_BLACKSMITH] = 1.0f;
	m_shadowFactorTable[JT_4_F_JOB_HUNTER] = 1.0f;
	m_shadowFactorTable[JT_4_F_JOB_KNIGHT] = 1.0f;
	m_shadowFactorTable[JT_4_F_NOVICE] = 1.0f;
	m_shadowFactorTable[JT_4_M_JOB_ASSASSIN] = 1.0f;
	m_shadowFactorTable[JT_4_M_JOB_BLACKSMITH] = 1.0f;
	m_shadowFactorTable[JT_4_M_JOB_HUNTER] = 1.0f;
	m_shadowFactorTable[JT_4_M_JOB_KNIGHT1] = 1.0f;
	m_shadowFactorTable[JT_4_M_JOB_KNIGHT2] = 1.0f;
	m_shadowFactorTable[JT_4_M_JOB_WIZARD] = 1.0f;
	m_shadowFactorTable[JT_4_BAPHOMET] = 1.0f;
	m_shadowFactorTable[JT_4_DARKLORD] = 1.0f;
	m_shadowFactorTable[JT_4_DEVIRUCHI] = 1.0f;
	m_shadowFactorTable[JT_8_DOPPEL] = 1.0f;
	m_shadowFactorTable[JT_4_F_GODEMOM] = 1.0f;
	m_shadowFactorTable[JT_4_F_GON] = 1.0f;
	m_shadowFactorTable[JT_4_F_KID2] = 1.0f;
	m_shadowFactorTable[JT_4_M_BIBI] = 1.0f;
	m_shadowFactorTable[JT_4_M_GEF_SOLDIER] = 1.0f;
	m_shadowFactorTable[JT_4_M_KID1] = 1.0f;
	m_shadowFactorTable[JT_4_M_MOC_SOLDIER] = 1.0f;
	m_shadowFactorTable[JT_4_M_PAY_SOLDIER] = 1.0f;
	m_shadowFactorTable[JT_4_M_SEAMAN] = 1.0f;
	m_shadowFactorTable[JT_2_M_ALCHE] = 1.0f;
	m_shadowFactorTable[JT_2_M_BARD_ORIENT] = 1.0f;
	m_shadowFactorTable[JT_2_M_SAGE_B] = 1.0f;
	m_shadowFactorTable[JT_2_M_SAGE_OLD] = 1.0f;
	m_shadowFactorTable[JT_4_F_ALCHE] = 1.0f;
	m_shadowFactorTable[JT_4_F_CRU] = 1.0f;
	m_shadowFactorTable[JT_4_F_MONK] = 1.0f;
	m_shadowFactorTable[JT_4_F_ROGUE] = 1.0f;
	m_shadowFactorTable[JT_4_M_ALCHE_A] = 1.0f;
	m_shadowFactorTable[JT_4_M_ALCHE_B] = 1.0f;
	m_shadowFactorTable[JT_4_M_ALCHE_C] = 1.0f;
	m_shadowFactorTable[JT_4_M_CRU] = 1.0f;
	m_shadowFactorTable[JT_4_M_CRU_OLD] = 1.0f;
	m_shadowFactorTable[JT_4_M_MONK] = 1.0f;
	m_shadowFactorTable[JT_4_F_JPNOBA] = 1.0f;
	m_shadowFactorTable[JT_4_M_JPN2] = 1.0f;
	m_shadowFactorTable[JT_4_M_JPN] = 1.0f;
	m_shadowFactorTable[JT_4_M_JPNCHIBI] = 1.0f;
	m_shadowFactorTable[JT_4_M_JPNOJI2] = 1.0f;
	m_shadowFactorTable[JT_4_M_JPNOJI] = 1.0f;
	m_shadowFactorTable[JT_8_M_JPNSOLDIER] = 1.0f;
	m_shadowFactorTable[JT_8_M_JPNMASTER] = 1.0f;
	m_shadowFactorTable[JT_4_M_SAGE_A] = 1.0f;
	m_shadowFactorTable[JT_4_M_SAGE_C] = 1.0f;
	m_shadowFactorTable[JT_4_F_SON] = 1.0f;
	m_shadowFactorTable[JT_4_F_JPN2] = 1.0f;
	m_shadowFactorTable[JT_4_F_JPN] = 1.0f;
	m_shadowFactorTable[JT_4_F_JPNCHIBI] = 1.0f;
	m_shadowFactorTable[JT_4_F_JPNOBA2] = 1.0f;
	m_shadowFactorTable[JT_4_F_JPNMU] = 1.0f;
}

void	CSession :: RefreshMouseSnapState(BOOL isCharacterSkillState)
{
	m_isMonsterSnap = FALSE;
	if (isCharacterSkillState) {
		if (m_monsterSnapOn_Skill)
			m_isMonsterSnap = TRUE;
	} else {
		if (m_monsterSnapOn_NoSkill)
			m_isMonsterSnap = TRUE;
	}
}

float	CSession :: GetShadowFactorWithJob(int job)
{
	return	m_shadowFactorTable[job];
}

void	CSession :: SetSkillUseLevel(int skillId, int skillUseLevel)
{
	if (skillId>=1 && skillId<0x40f)
		m_skillUseLevelList[skillId] = skillUseLevel;
}

int		CSession :: GetSkillUseLevel(int skillId)
{
	if (skillId>=1 && skillId<0x40f)
		return	m_skillUseLevelList[skillId];
	return	0;
}

void	CSession :: SetGuildSkillUseLevel(int skillId, int skillUseLevel)
{
	skillId -= 10000;
	if (skillId > -1)
		m_guildSkillUseLevelList[skillId] = skillUseLevel;
}

int		CSession :: GetGuildSkillUseLevel(int skillId)
{
	skillId -= 10000;
	if (skillId > -1)
		return	m_guildSkillUseLevelList[skillId];
	return	0;
}

void	CSession :: SetHomunSkillUseLevel(int skillId, int skillUseLevel)
{
	skillId -= 8000 ;

	if (skillId > -1)
		m_homunSkillUseLevelList[skillId] = skillUseLevel;
}

int		CSession :: GetHomunSkillUseLevel(int skillId)
{
	skillId -= 8000 ;

	if (skillId > -1)
		return	m_homunSkillUseLevelList[skillId];

	return	0 ;
}

void	CSession :: SetMerSkillUseLevel(int skillId, int skillUseLevel)
{
	skillId -= 8200 ;

	if (skillId > -1)
		m_merSkillUseLevelList[skillId] = skillUseLevel;
}

int		CSession :: GetMerSkillUseLevel(int skillId)
{
	skillId -= 8200 ;

	if (skillId > -1)
		return	m_merSkillUseLevelList[skillId];

	return	0 ;
}

const char *	CSession :: GetRmaName(int job)
{
	return	"\xC4\xB3\xB8\xAF\xC5\xCD\\\xC4\xAE\xC1\xFA\xB8\xF3\xBD\xBA\xC5\xCD.rma";
}

const char *	CSession :: GetW3DName(int job)
{
	return	"\xC4\xB3\xB8\xAF\xC5\xCD\\mdragon.w3d";
}

const char *	CSession :: GetHeadPaletteName(int head, int job, int sex, int palNum, char *buf)
{
	sAssert(palNum>=PALETTE_ZERO && palNum<=PALETTE_LAST-1);

	sAssert(head < HEADTYPE_LAST);
	CorrectHead(head, job);

	if (sex == SEX_MALE)
		sprintf(buf, "\xb8\xd3\xb8\xae\\\xb8\xd3\xb8\xae%s%s_%d.pal", m_newPcHeadNameTable_M[head], m_pcSexNameTable[sex], palNum);
	else
		sprintf(buf, "\xb8\xd3\xb8\xae\\\xb8\xd3\xb8\xae%s%s_%d.pal", m_newPcHeadNameTable_F[head], m_pcSexNameTable[sex], palNum);

	return	buf;
}

const char *	CSession :: GetBodyPaletteName(int job, int sex, int palNum, char *buf)
{
	if (job>0xf6e)
		sprintf(buf, "\xb8\xf6\\%s%s_%d.pal", m_palJobNameTable[job-0xf6e], m_pcSexNameTable[sex], palNum);
	else
		sprintf(buf, "\xb8\xf6\\%s%s_%d.pal", m_palJobNameTable[job], m_pcSexNameTable[sex], palNum);

	return	buf;
}

BOOL	CSession :: IsProperShield(int job, int shield)
{
	BOOL	isProperShield = FALSE;
	if (shield > 2000)
		shield = GetShieldType(shield);

	switch (job) {
		case	JT_GANGSI:
		case	JT_DEATHKNIGHT:
		case	JT_COLLECTOR:
			break;
		case	JT_TAEKWON:
		case	JT_STAR:
		case	JT_STAR2:
		case	JT_LINKER:
			break;
		case	JT_NOVICE:
		case	JT_NOVICE_H:
		case	JT_SUPERNOVICE:
		case	JT_NOVICE_B:
		case	JT_SUPERNOVICE_B:
			switch (shield) {
		case	SHIELD_GUARD:
			isProperShield = TRUE;
			break;
			}
			break;
		case	JT_SWORDMAN:
		case	JT_KNIGHT:
		case	JT_CRUSADER:
		case	JT_SWORDMAN_H:
		case	JT_KNIGHT_H:
		case	JT_CRUSADER_H:
		case	JT_SWORDMAN_B:
		case	JT_KNIGHT_B:
		case	JT_CRUSADER_B:
			switch (shield) {
		case	SHIELD_GUARD:
		case	SHIELD_SHIELD:
		case	SHIELD_MIRRORSHIELD:
		case	SHIELD_BUCKLER:
			isProperShield = TRUE;
			break;
			}
			break;
		case	JT_MAGICIAN:
		case	JT_WIZARD:
		case	JT_SAGE:
		case	JT_MAGICIAN_H:
		case	JT_WIZARD_H:
		case	JT_SAGE_H:
		case	JT_MAGICIAN_B:
		case	JT_WIZARD_B:
		case	JT_SAGE_B:
			switch (shield) {
		case	SHIELD_GUARD:
			isProperShield = TRUE;
			break;
			}
			break;
		case	JT_ARCHER:
		case	JT_HUNTER:
		case	JT_BARD:
		case	JT_DANCER:
		case	JT_ARCHER_H:
		case	JT_HUNTER_H:
		case	JT_BARD_H:
		case	JT_DANCER_H:
		case	JT_ARCHER_B:
		case	JT_HUNTER_B:
		case	JT_BARD_B:
		case	JT_DANCER_B:
			switch (shield) {
		case	SHIELD_GUARD:
			isProperShield = TRUE;
			break;
			}
			break;

		case	JT_PRIEST:
		case	JT_PRIEST_H:
		case	JT_PRIEST_B:
		case	JT_ACOLYTE:
		case	JT_ACOLYTE_B:
		case	JT_ACOLYTE_H:



			switch (shield) {
		case	SHIELD_GUARD:
		case	SHIELD_BUCKLER:
			isProperShield = TRUE;
			break;
			}
			break;
		case	JT_MERCHANT:
		case	JT_BLACKSMITH:
		case	JT_ALCHEMIST:
		case	JT_MERCHANT_H:
		case	JT_BLACKSMITH_H:
		case	JT_ALCHEMIST_H:
		case	JT_MERCHANT_B:
		case	JT_BLACKSMITH_B:
		case	JT_ALCHEMIST_B:
			switch (shield) {
		case	SHIELD_GUARD:
		case	SHIELD_BUCKLER:
			isProperShield = TRUE;
			break;
			}
			break;
		case	JT_THIEF:
		case	JT_ASSASSIN:
		case	JT_ROGUE:
		case	JT_THIEF_H:
		case	JT_ASSASSIN_H:
		case	JT_ROGUE_H:
		case	JT_THIEF_B:
		case	JT_ASSASSIN_B:
		case	JT_ROGUE_B:
			switch (shield) {
		case	SHIELD_GUARD:
		case	SHIELD_BUCKLER:
			isProperShield = TRUE;
			break;
			}
			break;
	}
	return	isProperShield;
}

char *	CSession :: GetShieldActName(int job, int sex, int shield, char *buf)
{
	const int JOBMINUS = 3950;

	if (shield == 0 || !IsProperShield(job, shield)) {
		buf[0] = 0;
		return buf;
	}

	if (job > JOBMINUS)
		sprintf(buf, "\xb9\xe6\xc6\xd0\\%s%s%s.act", m_pcJobNameTable[job-JOBMINUS], m_pcSexNameTable[sex], m_shieldNameTable[shield]);
	else
		sprintf(buf, "\xb9\xe6\xc6\xd0\\%s%s%s.act", m_pcJobNameTable[job], m_pcSexNameTable[sex], m_shieldNameTable[shield]);

	return buf;
}

char *	CSession :: GetShieldSprName(int job, int sex, int shield, char *buf)
{
	const int JOBMINUS = 3950;

	if (shield == 0 || !IsProperShield(job, shield)) {
		buf[0] = 0;
		return buf;
	}

	if (job > JOBMINUS)
		sprintf(buf, "\xb9\xe6\xc6\xd0\\%s%s%s.spr", m_pcJobNameTable[job-JOBMINUS], m_pcSexNameTable[sex], m_shieldNameTable[shield]);
	else
		sprintf(buf, "\xb9\xe6\xc6\xd0\\%s%s%s.spr", m_pcJobNameTable[job], m_pcSexNameTable[sex], m_shieldNameTable[shield]);

	return buf;
}

bool	CSession :: IsHaveAnotherPalette(int job, char *palName)
{
	return	false;
}

DWORD	CSession :: GetCurSystemTime()
{
	SYSTEMTIME	st;
	GetSystemTime(&st);
	return	(((st.wYear&0xff) * 32140800) + (st.wMonth * 2678400) + (st.wDay * 86400) + ((st.wHour+9) * 3600) + (st.wMinute * 60) + st.wSecond)*1000 + st.wMilliseconds;
}

void	CSession :: AddToExNameList(const char *exName)
{
	m_exNameList.push_back(exName);
}

void	CSession :: DeleteFromExNameList(const char *exName)
{
	std::list<std::string>::iterator	it = m_exNameList.begin();
	while (it != m_exNameList.end()) {
		if (*it == exName) {
			it = m_exNameList.erase(it);
			break;
		}
		++it;
	}
}

BOOL	CSession :: IsExName(const char * exName)
{
	std::list<std::string>::iterator	it = m_exNameList.begin();
	while (it != m_exNameList.end()) {
		if (*it == exName) {
			return	TRUE;
		}
		++it;
	}
	return	FALSE;
}

void	CSession :: AddToExAidList(DWORD aid)
{
	m_exAidList.push_back(aid);
}

void	CSession :: DeleteAllExNameList()
{
	m_exNameList.clear();
}

int		CSession :: NumExName()
{
	return	m_exNameList.size();
}

std::list<std::string> &	CSession :: GetNumExNameList()
{
	return	m_exNameList;
}

void	CSession :: DeleteFromExAidList(DWORD aid)
{
	std::list<DWORD>::iterator	it = m_exAidList.begin();
	while (it != m_exAidList.end()) {
		if (*it == aid) {
			it = m_exAidList.erase(it);
			break;
		}
		++it;
	}
}

BOOL	CSession :: IsExAid(DWORD aid)
{
	std::list<DWORD>::iterator	it = m_exAidList.begin();
	while (it != m_exAidList.end()) {
		if (*it == aid) {
			return	TRUE;
		}
		++it;
	}
	return	FALSE;
}

void	CSession :: DeleteAllExAidList()
{
	m_exAidList.clear();
}

int		CSession :: NumExAid()
{
	return	m_exAidList.size();
}

int	MoveBitForward(int value);
int	MoveBitForwardCnt(int value, int moveCnt);


void	CSession :: ChangeXorValue()
{
	static BOOL		s_isFirst = TRUE;
	if (s_isFirst)
	{
		s_isFirst = FALSE;
		m_xorIndex = random(12);
		m_spIndex = random(12);
		m_maxspIndex = random(12);
		m_hpIndex = random(12);
		m_maxhpIndex = random(12);
	}

	int		sp = GetSp();
	int		maxsp = GetMaxsp();
	int		hp = GetHp();
	int		maxhp = GetMaxhp();

	m_xorIndex = random(12);
	m_hpIndex = random(12);
	m_maxhpIndex = random(12);
	m_spIndex = random(12);
	m_maxspIndex = random(12);

	for (int i=0; i<12; ++i) {
		m_xorValue[i] = random(0xffffffff);
	}
	SetHp(hp);
	SetMaxhp(maxhp);
	SetSp(sp);
	SetMaxsp(maxsp);
}

inline int	MoveBitBackward(int value)
{
	return	((value&0xff000000)>>24) + ((value&0x00ff0000)<<8) + ((value&0x0000ff00)<<8) + ((value&0x000000ff)<<8);
}

int	MoveBitForward(int value)
{
	return	((value&0xff000000)>>8) + ((value&0x00ff0000)>>8) + ((value&0x0000ff00)>>8) + ((value&0x000000ff)<<24);
}

int	MoveBitForwardCnt(int value, int moveCnt)
{
	return	((value&0x0f000000)>>(moveCnt%24+1))
		+ ((value&0x000f0000)>>(moveCnt%16+1))
		+ ((value&0x00f00000)>>(moveCnt%20+1))
		+ ((value&0x00000f00)<<(moveCnt%20+1))
		+ ((value&0x000000f0)<<(moveCnt%24+1))
		+ ((value&0x0000000f)<<(moveCnt%28+1))
		+ ((value&0x0000f000)<<(moveCnt%16+1))
		+ ((value&0xf0000000)>>(moveCnt%28+1));
}

int		CSession :: Encode(int number, int encodeValue)
{

	int		rValue = number;
	int		count = encodeValue%6+1;
	count *= 2;
	for (int i=0; i<count; ++i) {
		rValue = MoveBitForward(rValue);
		rValue ^= MoveBitForwardCnt(encodeValue, i);
	}
	return	rValue;
}

int		CSession :: Decode(int number, int decodeValue)
{

	int		rValue = number;
	int		count = decodeValue%6+1;
	count *= 2;
	for (int i=count-1; i>=0; --i) {
		rValue ^= MoveBitForwardCnt(decodeValue, i);
		rValue = MoveBitBackward(rValue);
	}
	return	rValue;
}

void	CSession :: PreLoadAlwaysNeededRes()
{
	CActRes *	shadowAct = (CActRes *)g_resMgr().Get("shadow.act", false);
	CSprRes *	shadowSpr = (CSprRes *)g_resMgr().Get("shadow.spr", false);
	shadowAct->Lock();
	shadowSpr->Lock();

	int		tmpXHalfSpr = g_isXHalfSpr;
	int		tmpYHalfSpr = g_isYHalfSpr;
	g_isXHalfSpr = 0;
	g_isYHalfSpr = 0;

	CActRes *	cursorAct = (CActRes *)g_resMgr().Get("cursors.act", false);
	CSprRes *	cursorSpr = (CSprRes *)g_resMgr().Get("cursors.spr", false);
	cursorAct->Lock();
	cursorSpr->Lock();

	CActRes *	numberAct = (CActRes *)g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xfd\xc0\xda.act", false);
	CSprRes *	numberSpr = (CSprRes *)g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xfd\xc0\xda.spr", false);
	numberAct->Lock();
	numberSpr->Lock();

	g_isXHalfSpr = tmpXHalfSpr;
	g_isYHalfSpr = tmpYHalfSpr;
}

void	CSession :: ReLoadAlwaysNeededRes()
{
	CActRes *	shadowAct = (CActRes *)g_resMgr().Get("shadow.act", false);
	CSprRes *	shadowSpr = (CSprRes *)g_resMgr().Get("shadow.spr", false);
	g_resMgr().Unload(shadowAct);
	g_resMgr().Unload(shadowSpr);
















	CActRes *	cursorAct = (CActRes *)g_resMgr().Get("cursors.act", false);
	CSprRes *	cursorSpr = (CSprRes *)g_resMgr().Get("cursors.spr", false);
	g_resMgr().Unload(cursorAct);
	g_resMgr().Unload(cursorSpr);

	CActRes *	numberAct = (CActRes *)g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xfd\xc0\xda.act", false);
	CSprRes *	numberSpr = (CSprRes *)g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xfd\xc0\xda.spr", false);
	g_resMgr().Unload(numberAct);
	g_resMgr().Unload(numberSpr);
	PreLoadAlwaysNeededRes();

}

void	CSession :: SetCompressedEmblemImage(const char *compressedEmblemImage, DWORD emblemLen, int GDID, int emblemVersion)
{
	if (GDID == g_guildInfo.m_gdid) {
		g_guildInfo.m_emblemVersion = emblemVersion;
	}



	char	emblempath[256];
	sprintf(emblempath, "%s\\_tmpEmblem\\", g_baseDir3);

	if (access(emblempath, 0))
		if (_mkdir(emblempath) == -1)
			return;


	char	emblemtodeletem[256];
	sprintf(emblemtodeletem, "%s_%d_*.ebm", m_selectedServerName.c_str(), GDID);

	list<string>	emblemList;
	GetFileNames(emblemList, emblempath, emblemtodeletem);

	list<string>::iterator	it = emblemList.begin();
	while (it != emblemList.end()) {
		sprintf(emblemtodeletem, "%s%s", emblempath, it->c_str());
		_chmod(emblemtodeletem, 0000200);
		_unlink(emblemtodeletem);
		++it;
	}


	char	emblemfName[256];
	sprintf(emblemfName, "%s%s_%d_%d.ebm", emblempath, m_selectedServerName.c_str(), GDID, emblemVersion);

	FILE *	file = fopen(emblemfName, "wb");
	if (file) {
		fwrite(compressedEmblemImage, emblemLen, 1, file);
		fclose(file);
	}
}

const char *	CSession :: GetJobName(int job)
{
	return	m_jobNameTable[job];
}

void	CSession :: GetEmblemFileName(char *buffer, int gdid, int version)
{
	if (version > 0)
		sprintf(buffer, "%s_%d_%d.ebm", m_selectedServerName.c_str(), gdid, version);
	else
		sprintf(buffer, "..\\data\\texture\\effect\\flagGraffiti%d.ebm", gdid);
}


bool TokenFileToIntegerListPair(vector<IntegerListPair>& integerListPair, char* fName)
{
	integerListPair.clear();

	CFile fp;
	if (!fp.Open(fName, CFile::modeRead))
		return false;

	char *buf = (char *)fp.GetBuf();
	char *buf_end = (char *)(buf + fp.GetLength());
	char *base = buf;

	string a;
	IntegerList tmpList;

	for (; buf < buf_end; ++buf) {
		switch (*buf) {
		case '@':
			integerListPair.push_back(IntegerListPair(a, tmpList));
			a.erase();
			tmpList.clear();
			break;
		case '#':
			if (a.empty()) {
				a.assign(base, buf);
			} else {
				tmpList.push_back(atoi(base));
			}
			base = buf + 1;
			break;
		case 10:
		case 13:
			base = buf + 1;
			break;
		}
	}

	sort(integerListPair.begin(), integerListPair.end(), IListComp());
	fp.Close();
	return true;
}

BOOL	CSession :: InitSystemInfoTable()
{
	return true;
}

int		CSession :: GetJob()
{
	if (m_job == JT_KNIGHT && IsEffectStateChicken(g_session.m_effectState)) {
		return JT_CHICKEN;
	} else if (m_job == JT_CRUSADER && IsEffectStateChicken(g_session.m_effectState)) {
		return JT_CHICKEN2;
	} else if (m_job == JT_KNIGHT_H && IsEffectStateChicken(g_session.m_effectState)) {
		return JT_CHICKEN_H;
	} else if (m_job == JT_KNIGHT_B && IsEffectStateChicken(g_session.m_effectState)) {
		return JT_CHICKEN_B;
	} else if (m_job == JT_CRUSADER_H && IsEffectStateChicken(g_session.m_effectState)) {
		return JT_CHICKEN2_H;
	} else if (m_job == JT_CRUSADER_B && IsEffectStateChicken(g_session.m_effectState)) {
		return JT_CHICKEN2_B;
	} else if (m_job == JT_STAR && IsEffectStateStar2(g_session.m_effectState)) {
		return JT_STAR2;
	} else {
		return m_job;
	}
}

BOOL CSession :: IsThisPC(int job)
{
	if ( job > JT_2004_JOB_BEGIN && job < JT_MER_BEGIN)
		return TRUE;
	if ( job < JT_USER_LAST && job >= JT_NOVICE )
		return TRUE;
	return FALSE;
}

#if 1
BOOL	CSession :: InitMapPosTable()
{
	m_divisionMapPosTable = NULL ;

	m_mapPosTable.clear();
	m_divisionMapPosIndexTable.clear();

	CFile	fp;
	if (!fp.Open("mapPosTable.txt", CFile::modeRead))
		return	false;

	char *	buf = (char *)fp.GetBuf();
	char *	buf_end = (char *)(buf + fp.GetLength());
	char *		base = buf;

	int			divisionNum = 0 ;
	string		mapName;
	StringList	tmpPosList;
	string		groupNum ;

	MapPosInfo * mapPosInfo = NULL ;
	MapPosInfo * mapPosIndexInfo = NULL ;

	for (;buf<buf_end; ++buf)
	{
		switch (*buf)
		{
		case	'@':
			if ( m_maxNum == -1 )
			{
				m_maxNum = atoi(string(base, buf).c_str());
				m_divisionMapPosTable = new map<string, MapPosInfo>[m_maxNum] ;
			}
			break;
		case	'#':
			if (groupNum.empty())
			{
				groupNum.assign(base, buf);
				int num = atoi(string(base, buf).c_str());

				if ( ( divisionNum != num ) && mapPosIndexInfo )
				{
					m_divisionMapPosIndexTable[divisionNum] = *mapPosIndexInfo ;
					mapPosIndexInfo = NULL ;
					divisionNum = num ;
				}
			}
			else if (mapName.empty())
			{
				mapName.assign(base, buf);
			}
			else if (tmpPosList.size() < 3)
			{
				tmpPosList.push_back(string(base, buf));
			}
			else
			{
				tmpPosList.push_back(string(base, buf));

				mapPosInfo = new MapPosInfo ( atoi ( tmpPosList[0].c_str() ), atoi ( tmpPosList[1].c_str() ),
					atoi ( tmpPosList[2].c_str() ), atoi ( tmpPosList[3].c_str() ) ) ;

				if ( mapPosInfo )
				{
					m_divisionMapPosTable[divisionNum][mapName] = *mapPosInfo ;
					m_mapPosTable[mapName] = *mapPosInfo ;

					if ( mapPosIndexInfo )
					{
						if ( mapPosInfo->x_RB > mapPosIndexInfo->x_RB )
							mapPosIndexInfo->x_RB = mapPosInfo->x_RB ;

						if ( mapPosInfo->y_RB > mapPosIndexInfo->y_RB )
							mapPosIndexInfo->y_RB = mapPosInfo->y_RB ;
						if ( mapPosInfo->x_LT < mapPosIndexInfo->x_LT )
							mapPosIndexInfo->x_LT = mapPosInfo->x_LT ;

						if ( mapPosInfo->y_LT < mapPosIndexInfo->y_LT )
							mapPosIndexInfo->y_LT = mapPosInfo->y_LT ;
					}
					else
						mapPosIndexInfo = mapPosInfo ;
				}

				groupNum.erase();
				mapName.erase();
				tmpPosList.clear();
			}
			base = buf+1;
			break;
		case	10:
		case	13:
			base = buf+1;
			break;
		}
	}

	if ( mapPosIndexInfo )
		m_divisionMapPosIndexTable[divisionNum] = *mapPosIndexInfo ;

	fp.Close();
	return	TRUE;
}
#endif

BOOL	CSession :: InitDivisionMapPosTable()
{
	for ( int i = 0 ; i < m_maxNum ; i++ )
	{
		map <string, MapPosInfo> :: iterator it = m_divisionMapPosTable[i].begin() ;

		while (it != m_divisionMapPosTable[i].end())
		{
			CalculateMapPos ( &it->second ) ;

			it++ ;
		}
	}

	map <int, MapPosInfo> :: iterator it2 = m_divisionMapPosIndexTable.begin() ;

	while (it2 != m_divisionMapPosIndexTable.end())
	{
		CalculateMapPos ( &it2->second ) ;

		it2++ ;
	}

	return TRUE ;
}


void	CSession :: CalculateMapPos(MapPosInfo *mapPosInfo)
{
	if ( !mapPosInfo )
		return ;

	float w = ( float ( g_renderer->m_width ) / MAP_IMG_W ) ;
	float h = ( float ( g_renderer->m_height - 17 ) / MAP_IMG_H ) ;

	mapPosInfo->x_LT = mapPosInfo->x_LT * w + 0.5 ;
	mapPosInfo->y_LT = mapPosInfo->y_LT * h + 0.5 ;

	mapPosInfo->y_LT += 17 ;

	mapPosInfo->x_RB = mapPosInfo->x_RB * w + 0.5 ;
	mapPosInfo->y_RB = mapPosInfo->y_RB * h + 0.5 ;

	mapPosInfo->y_RB += 17 ;
}

bool	IsEffectStateNothing(DWORD effectState) { return effectState == EFFECTSTATE_NOTHING; }

bool	IsEffectStateHiding(DWORD effectState) { return effectState & EFFECTSTATE_HIDING ? true : false; }

bool	IsEffectStateSight(DWORD effectState) { return effectState & EFFECTSTATE_SIGHT ? true : false; }

bool	IsEffectStateRuwach(DWORD effectState) { return effectState & EFFECTSTATE_RUWACH ? true : false; }

bool	IsEffectStateBurrow(DWORD effectState) { return effectState & EFFECTSTATE_BURROW ? true : false; }


bool	IsEffectStatePushCart(DWORD effectState)
{
	bool isCart;
	isCart = effectState & EFFECTSTATE_PUSHCART ? true : false;
	if (isCart) return true;
	isCart = effectState & EFFECTSTATE_PUSHCART2 ? true : false;
	if (isCart) return true;
	isCart = effectState & EFFECTSTATE_PUSHCART3 ? true : false;
	if (isCart) return true;
	isCart = effectState & EFFECTSTATE_PUSHCART4 ? true : false;
	if (isCart) return true;
	return effectState & EFFECTSTATE_PUSHCART5 ? true : false;
}

bool	IsEffectStateBird(DWORD effectState) { return effectState & EFFECTSTATE_BIRD ? true : false; }

bool	IsEffectStateChicken(DWORD effectState) { return effectState & EFFECTSTATE_CHICKEN ? true : false; }

bool	IsEffectStateSpecialHiding(DWORD effectState) { return effectState & EFFECTSTATE_SPECIALHIDING ? true : false; }

bool	IsEffectStateOrcFace(DWORD effectState) { return effectState & EFFECTSTATE_ORCFACE ? true : false; }

bool	IsEffectStateTransform(DWORD effectState) { return effectState & EFFECTSTATE_TRANSFORM ? true : false; }

bool	IsEffectStateMarried(DWORD effectState) { return effectState & EFFECTSTATE_MARRIED ? true : false; }

bool	IsEffectStateFootPrint(DWORD effectState) { return effectState & EFFECTSTATE_FOOTPRINT ? true : false; }

bool	IsEffectStateStar2(DWORD effectState) { return effectState & EFFECTSTATE_STAR2 ? true : false; }

bool	IsEffectStateSanta(DWORD effectState) { return effectState & EFFECTSTATE_SANTA ? true : false; }

bool	IsEffectStateSummer(DWORD effectState) { return effectState & EFFECTSTATE_SUMMER ? true : false; }

bool	IsEffectStateSightChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_SIGHT ? true : false; }

bool	IsEffectStateRuwachChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_RUWACH ? true : false; }

bool	IsEffectStateBurrowChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_BURROW ? true : false; }

bool	IsEffectStateHidingChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_HIDING ? true : false; }


bool	IsEffectStatePushCartChanged(DWORD effectState, DWORD oldEffectState)
{
	if (effectState == oldEffectState)
		return false;
	bool state;
	state = (effectState ^ oldEffectState) & EFFECTSTATE_PUSHCART ? true : false;
	if (state)
		return true;
	state = (effectState ^ oldEffectState) & EFFECTSTATE_PUSHCART2 ? true : false;
	if (state)
		return true;
	state = (effectState ^ oldEffectState) & EFFECTSTATE_PUSHCART3 ? true : false;
	if (state)
		return true;
	state = (effectState ^ oldEffectState) & EFFECTSTATE_PUSHCART4 ? true : false;
	if (state)
		return true;
	state = (effectState ^ oldEffectState) & EFFECTSTATE_PUSHCART5 ? true : false;
	return state;
}

void	SetEffectStateSpecialHiding(DWORD* effectState){ *effectState |= EFFECTSTATE_SPECIALHIDING; }

void	ClearEffectStateSpecialHiding(DWORD* effectState){ *effectState &= ~EFFECTSTATE_SPECIALHIDING; }

bool	IsEffectStateBirdChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_BIRD ? true : false; }

bool	IsEffectStateChickenChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_CHICKEN ? true : false; }

bool	IsEffectStateSpecialHidingChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_SPECIALHIDING ? true : false; }

bool	IsEffectStateOrcFaceChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_ORCFACE ? true : false; }

bool	IsEffectStateTransformChanged(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_TRANSFORM ? true : false; }

bool	IsEffectStateStar2Changed(DWORD effectState, DWORD oldEffectState) { return (effectState ^ oldEffectState) & EFFECTSTATE_STAR2 ? true : false; }
