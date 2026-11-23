#include "common.h"
#include "packet.h"
#include "PaddingPacket.h"
#include "GameMode.h"
#include "GameActor.h"
#include "GameWorld.h"
#include "View.h"
#include "3dlib/Device.h"
#include "3dlib/Renderer.h"
#include "device/Control.h"
#include "device/Connection.h"
#include "resource/Attr.h"
#include "PathFinder.h"
#include "Session.h"
#include "window/UIFrameWnd.h"
#include "base/Util.h"
#include "Picker.h"
#include "3dlib/DrawUtil.h"
#include "device/Sound.h"
#include "Insult.h"
#include "resource/Wave.h"
#include "MsgStrings.h"
#include "TipOfTheDay.h"
#include "3dActor.h"
#include "GuildInfo.h"
#include "RagEffect.h"
#include "framework/Locale.h"
#include "framework/Language.h"
#include <string>
#include "3dGround.h"
#include "RagEffectPrim.h"
#include "GameActor3d.h"
#include "resource/Ground.h"
#include "MercenaryAI.h"
#include "Mode.h"
#include "SnapMgr.h"
#include "UIMessengerWnd.h"
#include "window/UISys.h"
#include "window/UIIllustWnd.h"
#include "base/File.h"
#include "base/Res.h"
#include "resource/Action.h"
#include "resource/Sprite.h"
#include "pdb_enums.h"
#include "pdb_shared.h"

using std::string;
using std::vector;
using std::map;

static const int HEADER_ANIM_FRAME_NUM = 9;
extern CMouse g_mouse;
extern CSession g_session;
extern CModeMgr g_modeMgr;
extern ServiceType g_serviceType;
extern ServerType g_serverType;
extern int g_frameskip;
extern int g_isFixedCamera;
extern int g_isLightMap;
extern void ResetTimer();
extern bool IsEffectStateHiding(DWORD effectState);
extern bool IsEffectStateBurrow(DWORD effectState);
extern bool IsGravityAid(DWORD aid);


#ifndef JT_GATHER_BEGIN
#define JT_GATHER_BEGIN 2800
#define JT_GATHER_END 2951
#endif
enum CURSOR_ACTION {
	CURSOR_ARROW = 0,
	CURSOR_TALK = 1,
	CURSOR_SELECT = 2,
	CURSOR_EMPTY = 3,
	CURSOR_ROTATE = 4,
	CURSOR_ATTACK = 5,
	CURSOR_ATTACK2 = 6,
	CURSOR_ENTER = 7,
	CURSOR_STOP = 8,
	CURSOR_GRIP = 9,
	CURSOR_TARGET = 10,
	CURSOR_TARGET2 = 11,
	CURSOR_ENTER2 = 12,
	CURSOR_GETHER = 13,
	CURSOR_REPAIR = 14,
	CURSOR_MOVE = 15,
	CURSOR_TOUCH = 16,
};
enum {
	ATTM_REPEAT = 0,
	ATTM_REPEAT_TILL_BUTTONUP = 1,
	ATTM_ONCE = 2,
	CTRL_LOCK_FALSE = 0,
	CTRL_LOCK_TRUE = 1,
	EFFECTSTATE_RED_NAME = 0x100000,
	NAME_BALLOON_BOTTOM = 0,
	NAME_BALLOON_TOP = 1,
	MM_CANCEL_ATTACK = 273,
	MM_ITEM_BROADCAST = 320,
	MM_WORLD_CHAT = 321,
	MM_TRADE_CHAT = 322,
	MM_CHAT_IN_MAP = 323,
};
typedef _MSG2AI MSG2AI;
extern CRenderer* g_renderer;
extern UIWindowMgr g_windowMgr;
extern BOOL g_isSkipFrame;
extern BOOL SkipSceneRendering();
extern int GetSkipFrameCount();
extern int g_isStopByLogin;
extern int g_isBlackWorld;
extern int g_showTipsAtStartup;
extern int g_BackTexture;
extern int g_isGoodSkillHanging;
extern short game_TetrisPlayed;
extern HP_Damage_Moment g_DamageMoment[];
extern CSnapMgr g_snapMgr;
extern tagRECT g_drawRect;
extern CFile saveChatFp;



enum {
	GMM_QUIT = 200, GSM_MAIN,
};

enum {
	GSM_WAITRESP_FROM_ZOSVR,
};

extern const char* MsgStr(MSGSTRINGID id);
extern int g_NextSkill;
extern short g_SpellSeek;
extern DWORD g_SpellSeekTime;
extern BOOL g_shiftKeyPressed;
extern char g_checkBOT;
extern int g_used_Skill;
extern unsigned long g_used_Skill_Time;
extern unsigned long g_BoyAttackedTime;
extern int g_curScreen;
extern int g_curStance;
extern short map_Number;
extern short g_curStory;
extern char g_curEXPmode;
extern char g_curEnding;
extern short g_Aco_Job;
extern short g_FieldConv_Alpha;
extern short g_AcoPosition;
extern unsigned long g_MainNextActTime;
extern unsigned long g_MobNextOutTime;
extern float AVG_DIST;
extern float FAR_DIST;
extern float CLOSE_DIST;
extern GroundAttrPosInfo g_groundAttrPosInfo;
void SetListenerOrient(float roty);


float CLOSE_DIST = 230.0f;
float FAR_DIST = 400.0f;
float AVG_DIST = 300.0f;
float g_viewLatitude = -45.0f;
float g_viewDistance = 500.0f;
float g_indoorViewLatitude = -45.0f;
float g_indoorViewDistance = 500.0f;
float g_outdoorViewLatitude = -45.0f;
float g_outdoorViewDistance = 500.0f;
static int lastSkillLevel = -1;
static int skillLevelDigitCount = 1;
static int skillLevelDigits[4] = { 9, 9, 9, 9 };
int oldshowImageType = -1;
char endchar[2] = { '\r', '\n' };


std::list<HP_Damage_Moment> g_DM_List;
CPathFinder g_pathFinder;
GroundAttrPosInfo g_groundAttrPosInfo;
std::list<DWORD> g_pingLogList;
unsigned long g_pingLogStart;
HP_Damage_Moment g_DamageMoment[10];
int g_BackTexture = 0;
int g_isFixedCamera = FALSE;
unsigned long CGameMode::m_lastLockOnPcGid = 0L;
unsigned long CGameMode::m_dwOldAutoFollowTime = 0L;
unsigned long CGameMode::m_dwOldDisappearTime = 0L;
bool g_pingLog = false;

inline int CheckKeyPressed(int vkcode)
{
	return HIBYTE(GetAsyncKeyState(vkcode));
}

inline bool IsGatherJob(int job)
{
	return job > JT_GATHER_BEGIN && job < JT_GATHER_END;
}

inline bool IsDBClickByJob(int)
{
	return false;
}


extern float g_viewLatitude;
extern float g_viewDistance;
extern float g_outdoorViewLatitude;
extern float g_outdoorViewDistance;
extern float g_indoorViewLatitude;
extern float g_indoorViewDistance;

extern const char* g_regPath;

extern int g_texLenDivFactor;


extern bool g_pingLog;
extern DWORD g_pingLogStart;


extern bool IsNameYellow(DWORD aid);
extern int  IsNameSpecific(DWORD aid);

#include "rag_budget_fill.h"
#include "rag_budget_sites.h"



extern bool IsEffectStateHiding(unsigned long state);
extern bool IsEffectStateBurrow(unsigned long state);
extern bool IsEffectStateSpecialHiding(unsigned long state);

extern char autosaveFileName[];
extern "C" int __cdecl access(const char *, int);
extern "C" int __cdecl _mkdir(const char *);

extern short bat_STUN_turn;
extern short g_PrevMob;
extern int g_BattleEvent;

CGameMode::CGameMode()
{
	m_world = NULL;
	m_view = NULL;
	m_mousePointer = NULL;
}


void	CGameMode :: OnInit(const char *modeName)
{
	m_posOfBossMon.x = -1;
	m_posOfBossMon.y = -1;
	m_isBossAlarm = false;
	m_onCopyName = false;

	m_world = new CWorld;
	GetWorld()->m_curMode = this;
	m_view = new CView;
	m_view->m_world = m_world;

	strcpy(m_rswName, modeName);

	string minimapBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\map\\";
	minimapBmpName += modeName;
	minimapBmpName.resize(minimapBmpName.size()-4);
	minimapBmpName += ".bmp";
	sAssert(sizeof(m_minimapBmpName) > minimapBmpName.size());
	strcpy(m_minimapBmpName, minimapBmpName.c_str());

	g_renderer->Clear(TRUE);
	g_session.SetWeight(0);
	g_session.SetMaxWeight(0);

	g_windowMgr.SetWallpaper(NULL);
	g_windowMgr.RemoveAllWindowsExceptChatWnd();
	g_windowMgr.HideChatWnd();
	g_windowMgr.MakeWindow(WID_LOADINGWND);
	g_windowMgr.SendMsg(UIM_LOADINGPERCENT, 0, 0, 0, 0);
	g_windowMgr.RenderWallPaper();
	g_windowMgr.Render();

	if(g_renderer->DrawScene())
		g_renderer->Flip(TRUE);

	m_view->OnEnterFrame();
	GetWorld()->OnEnterFrame();
	SetCheckGndAlpha();
	SetCamera();
	MakeGroundMousePointer();
	PlayBGM();
	if (g_session.IsEnableObjLightMap(m_rswName) == false)
	{
		g_Weather.SetObjLight(true);
	}
	else
	{
		g_Weather.SetObjLight(true);
	}

	if (!strcmp("yuno.rsw", m_rswName) ||
		!strcmp("valkyrie.rsw", m_rswName) ||
		!strcmp("rwc01.rsw", m_rswName) ||
		!strcmp("himinn.rsw", m_rswName) ||
		!strcmp("airplane.rsw", m_rswName) ||
		!strcmp("airplane_01.rsw", m_rswName) ||
		!strcmp("sch_gld.rsw", m_rswName) ||
		!strcmp("bat_fild02.rsw", m_rswName) ||
		!strcmp("que_qsch01.rsw", m_rswName) ||
		!strcmp("que_qsch02.rsw", m_rswName) ||
		!strcmp("que_qsch03.rsw", m_rswName) ||
		!strcmp("que_qsch04.rsw", m_rswName) ||
		!strcmp("que_qsch05.rsw", m_rswName) ||
		!strcmp("que_qaru01.rsw", m_rswName) ||
		!strcmp("que_qaru02.rsw", m_rswName) ||
		!strcmp("que_qaru03.rsw", m_rswName) ||
		!strcmp("que_qaru04.rsw", m_rswName) ||
		!strcmp("que_qaru05.rsw", m_rswName) ||
		!strcmp("bat_b01.rsw", m_rswName) ||
		!strcmp("bat_b02.rsw", m_rswName))
		g_renderer->SetBackgroundColor(0xff99ccff);
	else if (!strcmp("gonryun.rsw", m_rswName) ||
		!strcmp("gon_dun02.rsw", m_rswName) ||
		!strcmp("ra_temsky.rsw", m_rswName) ||
		!strcmp("que_temsky.rsw", m_rswName))
		g_renderer->SetBackgroundColor(0xff6699cc);
	else if (!strcmp("thana_boss.rsw", m_rswName))
		g_renderer->SetBackgroundColor(0xffe0d5c2);
	else
		g_renderer->m_nClearColor = 0;

	if (m_rswName[3] == '5' && m_rswName[4] == '@' && m_rswName[5] == 't' &&
		m_rswName[6] == 'o' && m_rswName[7] == 'w' && m_rswName[8] == 'e' &&
		m_rswName[9] == 'r') {
		g_renderer->SetBackgroundColor(0xff330033);
	} else if (!strcmp("5@tower.rsw", m_rswName)) {
		g_renderer->SetBackgroundColor(0xff330033);
	}

	MakeFog(g_session.m_fogOn);
	Initialize();
	{
		std::list<string> &exNameList = g_session.GetNumExNameList();
		std::list<string>::iterator it = exNameList.begin();
		while (it != exNameList.end())
		{
			ProcessTalkType(TT_REQ_WHISPER_PC_EX, (int)&(*it));
			++it;
		}
	}

	ResetTimer();

	m_isCtrlLock = CTRL_LOCK_FALSE;
	m_showTimeStartTick = 0;
	m_autoSaveChatCnt = 0;
	m_recordChatNum = 0;
	m_strikeNum = 0;
	for (int i=0;i<6;i++)
		m_doritime[i]=0;

	if (g_session.m_isSaveChat && g_session.IsMasterAid(7))
	{
		char buf[256];
		sprintf(buf, "Now Auto chat Save System is ENABLED.");
		g_windowMgr.SendMsg(WID_CHATWND, int(buf), 0xffff, 0, 0);
	}

	g_windowMgr.MakeWindow(WID_GUILDWND);
	g_windowMgr.DeleteWindow(WID_GUILDWND);
	if (g_session.m_initMapPos == 0)
		g_session.m_initMapPos = g_session.InitDivisionMapPosTable();
}

void CGameMode::Initialize()
{
	CMode::Initialize();

	m_actorNameList.clear();
	m_actorNameListByGID.clear();
	m_actorPosList.clear();
	m_pickupReqItemNaidList.clear();
	m_actorNameReqTimer.clear();
	m_menuIdList.clear();
	m_partyPosList.clear();
	m_compassPosList.clear();
	m_visibleTrapList.clear();

	g_snapMgr.RemoveAll();
	g_windowMgr.RemoveAllWindowsExceptChatWnd();
	g_windowMgr.SetWallpaper(NULL);
	g_windowMgr.MakeWindow(WID_BASICINFOWND);

	m_loopCond = 1;
	g_drawRect.left = 0;
	g_drawRect.right = 0;
	g_drawRect.top = 0;
	g_drawRect.bottom = 0;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = g_renderer->m_width;
	rect.bottom = g_renderer->m_height;

	g_actorPickNode.MakeQuardTree(1, rect);
	g_pathFinder.SetMap(m_world->m_attr);
	g_isBlackWorld = FALSE;
	m_fadeInCount = TRUE;

	m_mouseSnapDiff.x = 0;
	m_mouseSnapDiff.y = 0;
	m_leftBtnClickTick = 0;
	m_oldMouseX = g_mouse.GetXPos();
	m_oldMouseY = g_mouse.GetYPos();
	m_rBtnClickX = g_mouse.GetXPos();
	m_rBtnClickY = g_mouse.GetYPos();
	m_noMove = false;
	m_isOnQuest = false;
	m_isPlayerDead = false;
	m_nameBalloon = NULL;
	m_targetNameBalloon = NULL;
	m_broadcastBalloon = NULL;
	m_skillNameBalloon = NULL;
	m_skillMsgBalloon = NULL;
	m_skillUsedMsgBalloon = NULL;
	m_playerGage = NULL;
	m_nameActorAid = 0;
	m_nameBalloonWidth = 0;
	m_nameBalloonHeight = 0;
	m_nameDisplayed = TRUE;
	m_nameDisplayed2 = TRUE;
	m_waitingUseItemAck = false;
	m_waitingItemThrowAck = false;
	m_waitingReqStatusAck = false;
	m_dragInfo.m_dragType = DT_NODRAG;
	m_dragInfo.m_dragItemIndex = 0;
	m_dragInfo.m_numDragItem = 0;
	m_dragInfo.m_slotNum = -1;
	m_dragInfo.m_isIdentified = 0;
	m_lastNaid = 0;
	m_menuTargetAID = 0;
	m_lastPcGid = 0;
	m_lastMonGid = 0;
	m_lastLockOnMonGid = 0;
	m_isAutoMoveClickOn = 1;
	m_isWaitingWhisperSetting = FALSE;
	m_isWaitingEnterRoom = FALSE;
	m_isWaitingAddExchangeItem = FALSE;
	m_waitingWearEquipAck = timeGetTime() - 2000;
	m_waitingTakeoffEquipAck = timeGetTime() - 2000;
	m_isReqUpgradeSkillLevel = FALSE;
	m_exchangeItemCnt = 0;
	m_isWaitingCancelExchangeItem = FALSE;
	m_sameChatRepeatCnt = 0;
	m_numNotifyTime = 0;
	m_lastChat = "";
	m_skillUseInfo.m_skillUseType = SUT_NOSKILL;
	m_skillUseInfo.m_skillId = 0;
	g_isGoodSkillHanging = false;
	m_lastCardItemIndex = 0;
	m_showImageInfo.type = -1;
	m_SkillBallonSkillId = -1;
	m_nameBalloonType = NAME_BALLOON_BOTTOM;
	InitNameBalloonPosFromReg();

	g_mouse.ResetButtonState();

	m_receiveSyneRequestTime = 1;
	m_isReqEmotion = 0;
	m_syncRequestTime = timeGetTime();
	m_usedCachesUnloadTick = timeGetTime();
	m_reqEmotionTick = timeGetTime();
	m_noMoveStartTick = timeGetTime();
	m_lastEmblemReqTick = timeGetTime();
	m_lastNameWaitingListTick = timeGetTime();
	m_broadCastTick = timeGetTime();
	m_skillUsedTick = timeGetTime();
	m_reqTickChatRoom = timeGetTime();
	m_reqTickMerchantShop = timeGetTime();

	for (int i = 0; i < 6; i++)
		m_SiyeonStatus[i] = 0;

	m_MouseRollCount = 0;
	m_MouseRollMax = 0;

	SetListenerOrient(m_view->GetDestLongitude());

	if (g_isStopByLogin) {
		g_isStopByLogin = 0;
		g_guildInfo.Init();
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(WID_CHATWND, (int)MsgStr(MSI_LIST_COMMAND), int(RGB(230,230,255)), 0, 0);

		char ip[16];
		in_addr addr;
		addr.S_un.S_addr = g_session.m_lastLoginIP;
		strcpy(ip, inet_ntoa(addr));
	}

	g_session.ClearPlusValue();
	g_session.ChangeXorValue();
	g_session.ClearItem();
	g_session.ClearEquipItem();
	g_session.ClearStoreItem();
	g_session.ClearDealItems();
	g_session.ClearMerchantItem();
	g_session.ClearMerchantHopeToSellItem();
	g_session.ClearMerchantMyShopItem();
	g_session.ClearMerchantMirrorItem();

	static int oneTime = 0;
	if (!oneTime) {
		oneTime = 1;
		if (g_showTipsAtStartup && g_tipOfTheDay.Size())
			g_windowMgr.MakeWindow(WID_TIPOFTHEDAYWND);
	}

	PACKET_CZ_NOTIFY_ACTORINIT p;
	p.PacketType = HEADER_CZ_NOTIFY_ACTORINIT;
	CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(p.PacketType), (const char *)&p);

	SynchronizeTimeWithServer(true);

	g_Weather.Reset();
	g_Weather.ScriptProcess(m_rswName);

	if (!strcmp("xmas_fild01.rsw", m_rswName) || !strcmp("xmas.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchSnow();
		else
			g_Weather.StopSnow();
	}

	if (!strcmp("gef_fild07.rsw", m_rswName) ||
		!strcmp("mjolnir_01.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(0);
	} else if (!strcmp("yuno.rsw", m_rswName) ||
		!strcmp("gonryun.rsw", m_rswName) ||
		!strcmp("gon_dun02.rsw", m_rswName) ||
		!strcmp("ra_temsky.rsw", m_rswName) ||
		!strcmp("que_temsky.rsw", m_rswName) ||
		!strcmp("sch_gld.rsw", m_rswName) ||
		!strcmp("bat_fild02.rsw", m_rswName) ||
		!strcmp("bat_b01.rsw", m_rswName) ||
		!strcmp("bat_b02.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(1);
	} else if (!strcmp("valkyrie.rsw", m_rswName) ||
		!strcmp("rwc01.rsw", m_rswName) ||
		!strcmp("himinn.rsw", m_rswName) ||
		!strcmp("que_qsch01.rsw", m_rswName) ||
		!strcmp("que_qsch02.rsw", m_rswName) ||
		!strcmp("que_qsch03.rsw", m_rswName) ||
		!strcmp("que_qsch04.rsw", m_rswName) ||
		!strcmp("que_qsch05.rsw", m_rswName) ||
		!strcmp("que_qaru01.rsw", m_rswName) ||
		!strcmp("que_qaru02.rsw", m_rswName) ||
		!strcmp("que_qaru03.rsw", m_rswName) ||
		!strcmp("que_qaru04.rsw", m_rswName) ||
		!strcmp("que_qaru05.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(2);
	} else if (!strcmp("einbroch.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(3);
	} else if (!strcmp("thana_boss.rsw", m_rswName) ||
		!strcmp("moc_fild22.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(5);
	} else if (!strcmp("airplane.rsw", m_rswName) ||
		!strcmp("airplane_01.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(4);
	}

	if (m_rswName[3] == '6' && m_rswName[4] == '@' && m_rswName[5] == 't' &&
		m_rswName[6] == 'o' && m_rswName[7] == 'w' && m_rswName[8] == 'e' &&
		m_rswName[9] == 'r') {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(7);
	} else if (m_rswName[3] == '5' && m_rswName[4] == '@' && m_rswName[5] == 't' &&
		m_rswName[6] == 'o' && m_rswName[7] == 'w' && m_rswName[8] == 'e' &&
		m_rswName[9] == 'r') {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(8);
	} else if (!strcmp("5@tower.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(8);
	} else if (!strcmp("6@tower.rsw", m_rswName)) {
		if (g_session.m_isEffectOn)
			g_Weather.LaunchCloud(7);
	}

	if (g_serviceType == ServiceKorea ||
		g_serviceType == ServiceMalaysia ||
		g_serviceType == ServiceSingapore) {
		if (!strcmp("prontera.rsw", m_rswName) ||
			!strcmp("pay_arche.rsw", m_rswName)) {
			if (g_session.m_isEffectOn)
				g_Weather.LaunchSakura();
			else
				g_Weather.StopSakura();
		}
		if (!strcmp("comodo.rsw", m_rswName) && g_session.m_isEffectOn)
			g_Weather.LaunchPokJuk();
	}

	g_modeMgr.GetGameMode()->GetClientCoor(13, 0, m_world->m_player->m_pos.x, m_world->m_player->m_pos.z);

	if (!g_BackTexture)
		g_BackTexture = 1;

	g_session.HP_AddItem(0x5f0, false);
	g_session.HP_AddItem(0x1505, false);
	g_session.HP_AddItem(0x8ee, false);
	g_session.HP_AddItem(0x8dd, false);
	Zc_Notify_Entry_HP(1, 100, 100);
	Zc_Notify_Entry_HP(2, 100, 100);

	game_TetrisPlayed = 0;
	g_curStory = 0;
	g_session.GainSkill(0x107, 1, false);
	g_windowMgr.SetcurScreen(100);

	int j;
	for (j = 0; j < 10; j++)
		g_DamageMoment[j].DamageEnd = 0;

	char msg[256];
	sprintf(msg, "Controls use the mouse and keyboard");
	g_windowMgr.SendMsg(WID_CHATWND, int(msg), int(RGB(250,250,250)), 0, 0);
	sprintf(msg, "Choose menu items with the Up/Down keys and press A");
	g_windowMgr.SendMsg(WID_CHATWND, int(msg), int(RGB(250,250,250)), 0, 0);
}

BOOL	CGameMode :: OnRun()
{
	while (m_loopCond && !g_sysQuit) {
		if (!g_windowMgr.IsModalStatus()) {
			if (m_nextSubMode != -1) {
				int next = m_nextSubMode;
				m_subMode = next;
				m_subModeCnt = 0;
				m_nextSubMode = -1;
				OnChangeState(next);
			}
		}
		OnUpdate();
	}
	RunFadeOut(FALSE);
	return TRUE;
}

unsigned long nextCacheUnloadTime = 0;

void CGameMode::OnUpdate()
{
	static int timeGap = 0;
	static unsigned long time_update_start = ::timeGetTime();

	CheckSystemMessage();
	m_scheduler.OnRun();

	if (m_loopCond) {
		ProcessInput();
		ProcessPlayWave();
		if (g_isSkipFrame > 0) {
			ProcessAssignAID();
			UnloadRarelyUsedCaches();
		}
		int skipCount = GetSkipFrameCount();
		g_isSkipFrame = skipCount;
		if (skipCount > 0) {
			for (int i = skipCount; i > 0; --i) {
				m_world->ProcessActors();
				m_view->OnCalcViewInfo();
			}
		}
		if (skipCount == 0) {
			m_world->ProcessActors();
			m_view->OnCalcViewInfo();
		}
	}

	if (g_activatedNow) {
		g_activatedNow = FALSE;
		g_3dDevice.RestoreSurfaces();
		g_windowMgr.InvalidateAll();
	}

	g_Weather.Process();
	ProcessDamageSituation();

	if (!g_isAppActive && g_3dDevice.IsFullscreen()) return;

	if (SkipSceneRendering()) {
		g_renderer->ClearBackground();
		g_actorPickNode.DeleteSubNode();
		g_windowMgr.RenderBackGround2D();
		m_view->OnRender();
		g_windowMgr.RenderMenu();
		g_windowMgr.Render();
		DrawDragImage();
		DrawMouseCursor();
		if (g_renderer->DrawScene()) {
			g_renderer->ScreenEffect();
			g_renderer->Flip(true);
		}
	}

	ScreenShot();
}


void CGameMode::UnloadRarelyUsedCaches()
{
	DWORD now = timeGetTime();
	if (now > nextCacheUnloadTime) {
		nextCacheUnloadTime = now + 15000;
		g_renderer->UnloadRarelyUsedCaches();
	}
}

void CGameMode::OnExit()
{
	EndDigitalSound();

	if (m_world) {
		m_world->OnExitFrame();
		delete m_world;
	}
	if (m_view) {
		m_view->OnExitFrame();
		delete m_view;
	}

	g_actorPickNode.DeleteSubNode();
	g_resMgr().UnloadResByExt("rsm");
	g_resMgr().UnloadResByExt("rsw");
	g_resMgr().UnloadResByExt("gat");
	g_resMgr().UnloadRarelyUsedRes();
	g_texMgr.UnloadRarelyUsedTexture();
	g_renderer->DestroyAllRPList();
	g_renderer->ReleaseCacheSurfaces();
	g_windowMgr.WriteShorcutItemInfoToReg();
	g_windowMgr.WriteSkillUseLevelInfoToReg();
	g_session.WriteExListToRegistry();
	WriteNameBalloonPosToReg();
	if (g_serviceType == ServiceVietnam)
		WriteRagPathToReg();
	SaveCamera();
	if (g_session.m_isSaveChat) {
		CloseChatFile();
	}
}

void CGameMode::InitNameBalloonPosFromReg()
{
	char NameBalloonPosPath[512];
	HKEY hKey;
	HKEY hResultKey;
	DWORD REG_TYPE;
	DWORD DATALENGTH;

	strcpy(NameBalloonPosPath, g_regPath);
	strcat(NameBalloonPosPath, "\\NameBaloon");
	strcat(NameBalloonPosPath, "\\");
	strcat(NameBalloonPosPath, g_session.m_lastServerName.c_str());

	LRESULT lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, NameBalloonPosPath, 0, KEY_QUERY_VALUE, &hResultKey);
	if (lResult == ERROR_SUCCESS)
	{
		RegCloseKey(hResultKey);
		LRESULT lResult1 = RegOpenKeyExA(HKEY_LOCAL_MACHINE, NameBalloonPosPath, 0, KEY_ALL_ACCESS, &hKey);
		if (lResult1 == ERROR_SUCCESS)
		{
			REG_TYPE = REG_DWORD;
			DATALENGTH = sizeof(DWORD);
			RegQueryValueExA(hKey, "POS", NULL, &REG_TYPE, (LPBYTE)&m_nameBalloonType, &DATALENGTH);
			RegCloseKey(hKey);
		}
	}

	if (m_nameBalloonType != 0 && m_nameBalloonType != 1)
		m_nameBalloonType = 0;
}

void CGameMode::SetCamera()
{
	int isExist;

	m_view->SetCurAt(m_world->m_player->m_pos.x, m_world->m_player->m_pos.y, m_world->m_player->m_pos.z);
	m_view->SetDestAt(m_world->m_player->m_pos.x, m_world->m_player->m_pos.y, m_world->m_player->m_pos.z);

	m_hasViewPoint = false;
	vector<string>& ViewPointParamList = g_session.GetViewPointParameter(m_rswName, isExist);
	if (isExist) {
		m_hasViewPoint = true;
		for (int i = 0; i < 9; i++)
			ViewPointData[i] = atof(ViewPointParamList[i].c_str());
	}

	m_canRotateView = !g_session.IsIndoorRswName(m_rswName);

	float deltaLatitude = 10.0f;
	float farDist = AVG_DIST;
	if (m_canRotateView) {
		deltaLatitude = 20.0f;
		farDist = FAR_DIST;
		g_viewLatitude = g_outdoorViewLatitude;
		g_viewDistance = g_outdoorViewDistance;
	} else {
		m_fixedLongitude = 45;
		m_view->SetCurLongitude(m_fixedLongitude);
		m_view->SetDestLongitude(m_fixedLongitude);
		g_viewLatitude = g_indoorViewLatitude;
		g_viewDistance = g_indoorViewDistance;
	}

	if (g_viewLatitude < -45 - deltaLatitude)
		g_viewLatitude = -45 - deltaLatitude;
	if (g_viewLatitude > -45 + deltaLatitude)
		g_viewLatitude = -45 + deltaLatitude;

	if (g_viewDistance < CLOSE_DIST)
		g_viewDistance = CLOSE_DIST;
	if (g_viewDistance > farDist)
		g_viewDistance = farDist;

	if (m_hasViewPoint) {
		g_viewDistance = ViewPointData[2];
		m_view->SetCurLongitude(ViewPointData[5]);
		m_view->SetDestLongitude(ViewPointData[5]);
		g_viewLatitude = ViewPointData[8];
	}

	m_view->SetCurLatitude(g_viewLatitude);
	m_view->SetDestLatitude(g_viewLatitude);
	m_view->SetCurDistance(g_viewDistance);
	m_view->SetDestDistance(g_viewDistance);
}

void	CGameMode :: SaveCamera()
{
	if (m_canRotateView) {
		g_outdoorViewLatitude = g_viewLatitude;
		g_outdoorViewDistance = g_viewDistance;
	} else {
		g_indoorViewLatitude = g_viewLatitude;
		g_indoorViewDistance = g_viewDistance;
	}
}

void	CGameMode :: PlayBGM()
{
	const char *temp = g_session.GetMp3Name(m_rswName);
	m_streamFileName = temp;
	if (!g_session.m_bgmIsPaused)
		PlayStream("bgm\\08.mp3");
	SetRoomEffect(ENVIRONMENT_GENERIC);
}
void	CGameMode :: WriteRagPathToReg()
{
	char	RagPath[512];
	strcpy(RagPath, g_regPath);
	strcat(RagPath, "\\");

	DWORD	dwDisposition = 0;
	HKEY	hKey;

	LRESULT lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, RagPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult == ERROR_SUCCESS) {
		RegSetValueEx(hKey, "RAGPATH", NULL, REG_SZ, (LPBYTE)g_baseDir3, sizeof(char)*strlen(g_baseDir3));
		RegCloseKey(hKey);
	}
}

void	CGameMode :: WriteNameBalloonPosToReg()
{
	char	NameBalloonPosPath[512];
	strcpy(NameBalloonPosPath, g_regPath);
	strcat(NameBalloonPosPath, "\\NameBaloon");
	strcat(NameBalloonPosPath, "\\");
	strcat(NameBalloonPosPath, g_session.m_lastServerName.c_str());

	DWORD	dwDisposition = 0;
	HKEY	hKey;

	LRESULT lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, NameBalloonPosPath, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult == ERROR_SUCCESS) {
		RegSetValueEx(hKey, "POS", NULL, REG_DWORD, (LPBYTE)(&m_nameBalloonType), sizeof(DWORD));
		RegCloseKey(hKey);
	}
}

void	CGameMode :: SetCheckGndAlpha()
{

	m_isCheckGndAlpha = FALSE;
}

void	CGameMode :: RemoveGameActorAllAndMake()
{
	m_world->RemoveGameActorAll();
	m_world->InitPlayer();
	m_world->MakeFixedActor();
	g_Weather.Reset();

	MakeGroundMousePointer();
	m_view->SetCurAt(m_world->m_player->m_pos.x, m_world->m_player->m_pos.y, m_world->m_player->m_pos.z);
}

void CGameMode::PrintDebugString() {}

void CGameMode :: ResetDrag()
{
	m_dragInfo.m_isIdentified = 0;
	m_dragInfo.m_dragItemIndex = 0;
	m_dragInfo.m_dragItemName = "";
	m_dragInfo.m_dragItemPrice = 0;
	m_dragInfo.m_dragSprName = "";
	m_dragInfo.m_dragType = DT_NODRAG;
	m_dragInfo.m_numDragItem = 0;
	m_dragInfo.m_slotNum = -1;
	m_dragInfo.m_skillUseLevel = 0;
	m_dragInfo.m_refiningLevel = 0;
	for (int a = 0; a < 4; a++) {
		m_dragInfo.m_slot[a] = 0;
	}
}

void	CGameMode :: DrawUseSkillLevelNumber()
{
	if (m_skillUseInfo.m_skillUseType == SUT_NOSKILL || m_skillUseInfo.m_useLevel == 0)
		return;

	if (m_skillUseInfo.m_useLevel != lastSkillLevel) {
		int number = m_skillUseInfo.m_useLevel;
		lastSkillLevel = number;
		if (number > 9999)
			number = 9999;
		else if (number < 0)
			number = 0;

		skillLevelDigitCount = 1;
		if (number > 999)
			skillLevelDigitCount = 4;
		else if (number > 99)
			skillLevelDigitCount = 3;
		else if (number > 9)
			skillLevelDigitCount = 2;

		skillLevelDigits[0] = number % 10;
		number /= 10;
		skillLevelDigits[1] = number % 10;
		number /= 10;
		skillLevelDigits[2] = number % 10;
		number /= 10;
		skillLevelDigits[3] = number;
	}

	CActRes *cursorAct = (CActRes *)g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xfd\xc0\xda.act", false);
	CSprRes *cursorSpr = (CSprRes *)g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xfd\xc0\xda.spr", false);

	for (int i = 0; i < skillLevelDigitCount; i++) {
		int x = int(g_mouse.GetXPos() + m_mouseSnapDiff.x + 24.0f - i * 16 + (skillLevelDigitCount * 16 - 16));
		int y = int(g_mouse.GetYPos() + m_mouseSnapDiff.y);
		DrawSprite(x, y, cursorAct, cursorSpr, 0, skillLevelDigits[i], 2.0f, 0.0f, 0xfaffffff);
	}
}

char oldshowImageName[256] = "";

void	CGameMode :: DrawIllustImage()
{
	if (m_showImageInfo.type == -1) {
		if (g_windowMgr.GetIllustWnd())
			g_windowMgr.DeleteWindow(WID_ILLUSTWND);
		return;
	}

	if (m_showImageInfo.imageName.size() == 0)
		return;

	UIIllustWnd *illustWnd = g_windowMgr.GetIllustWnd();
	if (!illustWnd) {
		illustWnd = (UIIllustWnd *)g_windowMgr.MakeWindow(WID_ILLUSTWND);
	} else {
		if (m_showImageInfo.type == oldshowImageType &&
			strcmp(oldshowImageName, m_showImageInfo.imageName.c_str()) == 0)
			return;

		oldshowImageType = m_showImageInfo.type;
		sprintf(oldshowImageName, m_showImageInfo.imageName.c_str());
	}

	int y = 0;
	int x = 160;

	illustWnd->SendMsg(NULL, UM_SETINFO, -1, 0, 0, 0);

	switch (m_showImageInfo.type) {
	case 0:
		illustWnd->SendMsg(NULL, UM_SET_BITMAP, (int)m_showImageInfo.imageName.c_str(), 0, 0, 0);
		x = 200;
		y = g_renderer->m_height - illustWnd->GetHeight();
		break;
	case 1:
		illustWnd->SendMsg(NULL, UM_SET_BITMAP, (int)m_showImageInfo.imageName.c_str(), 0, 0, 0);
		x = g_renderer->m_halfWidth;
		y = g_renderer->m_height - illustWnd->GetHeight();
		break;
	case 2:
		illustWnd->SendMsg(NULL, UM_SET_BITMAP, (int)m_showImageInfo.imageName.c_str(), 0, 0, 0);
		x = g_renderer->m_width - 200;
		y = g_renderer->m_height - illustWnd->GetHeight();
		break;
	case 3:
		illustWnd->SendMsg(NULL, UM_SETINFO, -2, 0, 0, 0);
		illustWnd->SendMsg(NULL, UM_SET_BITMAP, (int)m_showImageInfo.imageName.c_str(), 0, 0, 0);
		illustWnd->SendMsg(NULL, UM_SETINFO, 3, 0, 0, 0);
		x = g_renderer->m_halfWidth;
		y = g_renderer->m_halfHeight - illustWnd->GetHeight() / 2;
		break;
	case 4:
		illustWnd->SendMsg(NULL, UM_SET_BITMAP, (int)m_showImageInfo.imageName.c_str(), 0, 0, 0);
		illustWnd->SendMsg(NULL, UM_SETINFO, 4, 0, 0, 0);
		x = g_renderer->m_halfWidth;
		y = g_renderer->m_halfHeight - illustWnd->GetHeight() / 2;
		break;
	}

	illustWnd->Move(x - illustWnd->GetWidth() / 2, y);
}

void	CGameMode :: DrawPKRanking()
{
	int rank = m_world->m_player->GetPKRank();
	int total = m_world->m_player->GetPKTotal();

	if (total <= 0 || rank <= 0)
		return;

	int i;
	int x, y;
	CActRes* rank_act = (CActRes*)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\RankFont.act", false);
	CSprRes* rank_spr = (CSprRes*)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\RankFont.spr", false);

	int		number = total;

	if (number > 9999)		number = 9999;

	int		numberCnt = 1;
	if (number>999)			numberCnt = 4;
	else if (number>99)		numberCnt = 3;
	else if (number>9)		numberCnt = 2;

	int		eachNum[4] = { 9, 9, 9, 9 }, numPosx = 0;
	eachNum[0] = number % 10;
	number /= 10;
	eachNum[1] = number % 10;
	number /= 10;
	eachNum[2] = number % 10;
	number /= 10;
	eachNum[3] = number;

	x = g_renderer->m_width - 80;
	y = g_renderer->m_height - 30;

	for (i=0; i<numberCnt; i++) {
		DrawSprite(x + (25 * numberCnt) + (-i * 25), y, rank_act, rank_spr, eachNum[i], 0, 1.0f, 0.0f, 0xffffffff);
	}

	x = g_renderer->m_width - 80;
	y = g_renderer->m_height - 40;

	DrawSprite(x, y, rank_act, rank_spr, 10, 0, 1.0f, 0.0f, 0xffffffff);

	number = rank;

	numberCnt = 1;
	if (number>999)			numberCnt = 4;
	else if (number>99)		numberCnt = 3;
	else if (number>9)		numberCnt = 2;

	eachNum[0] = 9;
	eachNum[1] = 9;

	eachNum[2] = 9;
	eachNum[3] = 9;

	eachNum[0] = number % 10;
	number /= 10;
	eachNum[1] = number % 10;
	number /= 10;
	eachNum[2] = number % 10;
	number /= 10;
	eachNum[3] = number;

	x = g_renderer->m_width - 80;
	y = g_renderer->m_height - 50;

	for (i=0; i<numberCnt; i++) {
		DrawSprite(x + (-i * 25) - 25, y, rank_act, rank_spr, eachNum[i], 0, 1.0f, 0.0f, 0xffffffff);
	}
}

void	CGameMode :: DrawTime(DWORD timeTick, int showType)
{
	DWORD curTime;
	if (showType == 0) {
		curTime = timeTick;
		if (!m_showTimeStartTick)
			m_showTimeStartTick = timeGetTime();
		if (timeGetTime() - m_showTimeStartTick > 5000) {
			m_showTimeStartTick = 0;
			g_session.m_isShowTime = FALSE;
		}
	} else if (showType == 1) {
		curTime = timeGetTime() - timeTick;
		if (timeTick == 0) {
			g_session.m_isShowTime = FALSE;
		}
	} else if (showType == 2) {
		curTime = timeTick - timeGetTime();
		if (curTime <= 0) {
			g_session.m_isShowTime = FALSE;
		}
	} else if (showType == 3) 	{
		curTime = g_session.m_showDigitTick;
		curTime /= 30;
		g_session.m_showDigitTick--;
		if (curTime<=0)
			g_session.m_isShowTime = FALSE;
	}

	const int offset = 40;

	int Sec = curTime / 1000, Min = 0, Hour = 0, Day = 0, Start = 0;
	int x, y, i, j, Seperator = 0, number, numberCnt, eachNum[16] = { 0, };
	if (Sec >= 60) {
		Min = Sec / 60;
		Sec = Sec - (Min * 60);
		if (Min >= 60) {
			Hour = Min / 60;
			Min = Min - (Hour * 60);
		}
		if (Hour >= 24) {
			Day = Hour / 24;
			Hour = Hour - (Day * 24);
		}
	}

	CActRes* rank_act = (CActRes*)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\TimeFont.act", false);
	CSprRes* rank_spr = (CSprRes*)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\TimeFont.spr", false);

	if (showType == 3)
	{
		number = curTime;
		Min = 0;
		Hour = 0;
		Day = 0;
	}
	else
		number = Sec;

	if (number > 9999)		number = 9999;
	if (number<0)			number=0;

	numberCnt = 2;

	eachNum[0] = number % 10;
	number /= 10;
	eachNum[1] = number % 10;
	if (Min > 0) {
		number = Min;
		eachNum[3] = number % 10;
		number /= 10;
		eachNum[4] = number % 10;
		numberCnt = 4;
	} if (Hour > 0) {
		number = Hour;
		eachNum[6] = number % 10;
		number /= 10;
		eachNum[7] = number % 10;
		numberCnt = 6;
	} if (Day > 0) {
		number = Day;
		eachNum[9] = number % 10;
		number /= 10;
		eachNum[10] = number % 10;
		numberCnt = 8;
	}

	x = g_renderer->m_width / 2;
	y = 40;
	Seperator = (numberCnt - 2) / 2;
	numberCnt = numberCnt + Seperator;
	Start = (numberCnt * 40) / 2 * -1;
	j = numberCnt - 1;
	for (i = 0; i<numberCnt; i++, j--) {
		if ((i + 1) % 3 != 0) {
			DrawSprite(x + Start, y, rank_act, rank_spr, eachNum[j], 0, 1.0f, 0.0f, 0xFFFFFFFF);
		} else {
			DrawSprite(x + Start, y, rank_act, rank_spr, 10, 0, 1.0f, 0.0f, 0xFFFFFFFF);
		}
		Start += 40;
	}
}

void	CGameMode :: DrawPKZoneTime()
{
	CPlayer* player = m_world->m_player;
	int rank = player->GetPKRank();
	int total = player->GetPKTotal();
	if (total <= 0 || rank <= 0)
		return;
	if (total <= 1 || rank != 1) {
		return;
	}

	DrawTime(g_session.m_killTimeStartTick, 1);

	if (g_session.m_killTimeStartTick && (timeGetTime() - g_session.m_killTimeStartTick) < 3000) {
		DrawTimeAttack();
	}
}

void	CGameMode :: ShowTime()
{
	if (g_session.m_isShowTime)
		DrawTime(g_session.m_showDigitTick, g_session.m_showType);
}

void CGameMode::CheckFrame() {}

void CGameMode::DrawTimeAttack()
{
	static int motionNum = 0;
	CActRes* rank_act = (CActRes*)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\TimeAttack.act", false);
	CSprRes* rank_spr = (CSprRes*)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\TimeAttack.spr", false);

	if (motionNum / 3 >= rank_act->GetMotionCount(0)) {
		return;
	}

	int x, y;
	x = g_renderer->m_width / 2 - rank_spr->m_sprites[0][0]->height / 2;
	y = 120;
	DrawSprite(x, y, rank_act, rank_spr, 0, motionNum / 3, 1.0f, 0.0f, 0xFFFFFFFF);
	motionNum++;
}

void CGameMode::DrawDragImage()
{
	if (m_dragInfo.m_dragType == DT_NODRAG)
		return;

	if (g_windowMgr.GetItemDropCntWnd() && g_windowMgr.GetItemDropCntWnd()->IsShow())
		return;

	if (m_dragInfo.m_dragSprName.size() == 0) {
		ResetDrag();
		return;
	}

	DrawSprite(g_mouse.GetXPos(), g_mouse.GetYPos(),
		(CActRes *)g_resMgr().Get((m_dragInfo.m_dragSprName + ".act").c_str(), false),
		(CSprRes *)g_resMgr().Get((m_dragInfo.m_dragSprName + ".spr").c_str(), false),
		0, 0, 1.0f, 0.0f, 0xfaffffff);
}

void	CGameMode :: AutoStatusUp()
{
	for (int i = 0; i < 6; i++)
	{
		if (m_SiyeonStatus[i] > 0)
		{
			PACKET_CZ_STATUS_CHANGE p;
			p.PacketType = HEADER_CZ_STATUS_CHANGE;
			if (i == 0)
				p.statusID = VAR_STR;
			else if (i == 1)
				p.statusID = VAR_AGI;
			else if (i == 2)
				p.statusID = VAR_LUK;
			else if (i == 3)
				p.statusID = VAR_INT;
			else if (i == 4)
				p.statusID = VAR_VIT;
			else if (i == 5)
				p.statusID = VAR_DEX;
			p.changeAmount = 1;
			if (!m_isPlayerDead)
			{
				CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(p.PacketType), (const char *)(&p));
				m_SiyeonStatus[i]--;
			}
		}
	}
}

void	CGameMode :: DrawMiniMap()
{
	if (g_windowMgr.m_minimapZoomWnd == NULL)
		return;

	const int screenMapSize = 128;
	int delta = screenMapSize+16;
	float screenMapWidth = screenMapSize, screenMapHeight = screenMapSize;
	float screenMapCenterX = g_renderer->m_width-delta+screenMapWidth/2;
	float screenMapCenterY = 16+screenMapHeight/2;

	int attrX, attrY;
	int attrW = m_world->m_attr->m_width;
	int attrH = m_world->m_attr->m_height;
	GetServerCoor(m_world->m_player->m_pos.x, m_world->m_player->m_pos.z, attrX, attrY);

	int maxAttrLen = (attrW > attrH) ? attrW : attrH;
	screenMapWidth = float(screenMapWidth) * float(attrW) / float(maxAttrLen);
	screenMapHeight = float(screenMapHeight) * float(attrH) / float(maxAttrLen);

	float miniMapZoom = g_windowMgr.m_miniMapZoomFactor;

	sAssert(miniMapZoom >= 1.0f);
	float	uvHalfDelta = 1.0f/(miniMapZoom*2.0f);


	float pU = float(attrX) / float(attrW);
	float pV = float(attrH-attrY) / float(attrH);
	UVRECT uvRect(pU-uvHalfDelta, pV-uvHalfDelta, pU+uvHalfDelta, pV+uvHalfDelta);

	if (uvRect.u1 < 0) {
		uvRect.u1 = 0;
		uvRect.u2 = uvHalfDelta*2;
		if (uvRect.u2 > 1)
			uvRect.u2 = 1;
	}
	if (uvRect.v1 < 0) {
		uvRect.v1 = 0;
		uvRect.v2 = uvHalfDelta*2;
		if (uvRect.v2 > 1)
			uvRect.v2 = 1;
	}
	if (uvRect.u2 > 1) {
		uvRect.u2 = 1;
		uvRect.u1 = uvRect.u2 - uvHalfDelta*2;
		if (uvRect.u1 < 0)
			uvRect.u1 = 0;
	}
	if (uvRect.v2 > 1) {
		uvRect.v2 = 1;
		uvRect.v1 = uvRect.v2 - uvHalfDelta*2;
		if (uvRect.v1 < 0)
			uvRect.v1 = 0;
	}

	int tmpTexLenDivFactor = g_texLenDivFactor;

	g_texLenDivFactor = 1;

	{
		CTexture * mapTexture = g_texMgr.GetTexture(m_minimapBmpName, false);
		sAssert(mapTexture);

		RECT rect = { screenMapCenterX-screenMapSize/2, screenMapCenterY-screenMapSize/2,
			screenMapCenterX+screenMapSize/2, screenMapCenterY+screenMapSize/2 };
		float oow = 1.0f/NEARPLANE-0.0002f;

		DWORD miniMapArgb = g_windowMgr.m_miniMapArgb;

		DrawTexture(&rect, 0, oow, miniMapArgb, mapTexture, uvRect);
	}

	unsigned char l_dir = 0 ;
	{

		float marginX  = ( float(screenMapSize) - float(screenMapWidth) ) / 2 ;
		float marginY = ( float(screenMapSize) - float(screenMapHeight) ) / 2 ;

		float pMapDeltaX = ( (float(attrX) * float(screenMapWidth) / float(attrW)) + marginX ) / (uvRect.u2 - uvRect.u1);
		float pMapDeltaY = (screenMapHeight - (float(attrY) * float(screenMapHeight) / float(attrH)) + marginY ) / (uvRect.v2 - uvRect.v1);
		float mpx = screenMapCenterX - screenMapSize/2 - float(screenMapSize)*uvRect.u1/(uvRect.u2 - uvRect.u1) + pMapDeltaX;
		float mpy = screenMapCenterY - screenMapSize/2 - float(screenMapSize)*uvRect.v1/(uvRect.v2 - uvRect.v1) + pMapDeltaY;

		int halfRect = 5;
		int angle = 180-(m_world->m_player->m_roty);
		RECT rect = { mpx-halfRect, mpy-halfRect, mpx+halfRect, mpy+halfRect };
		float oow = 1.0f/NEARPLANE-0.0001f;
		DWORD	argb = 0xffffffff;

		l_dir = (m_world->m_player->m_roty + 1) / 45 ;
		if ( l_dir == 8 ) l_dir = 0 ;


		DWORD	argb2 = 0x77333333 ;
		RECT rect2= { rect.left - 2, rect.top - 2, rect.right + 2, rect.bottom + 4 };
		DrawTexture(&rect2, angle, oow, argb2, g_texMgr.GetTexture("\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\map\\map_arrow.bmp", false));
		DrawTexture(&rect, angle, oow, argb, g_texMgr.GetTexture("\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\map\\map_arrow.bmp", false));
	}

	if ( m_isBossAlarm )
		DrawBossMonsterOnMiniMap(uvRect);

	DrawActorsOnMiniMap(uvRect);
	DrawCompassOnMiniMap(uvRect);

	g_texLenDivFactor = tmpTexLenDivFactor;

}

void	CGameMode :: DrawCompassOnMiniMap(const UVRECT &uvRect)
{
	const int screenMapSize = 128;

	if (m_compassPosList.size() == 0)
		return;
	{
		std::map<DWORD, ColorCellPos2>::iterator it = m_compassPosList.begin();
		while (it != m_compassPosList.end()) {
			int		type = (*it).second.type;
			DWORD	startTime = (*it).second.startTime;
			if ((COMPASS_TEMPORARY == type) && (timeGetTime()>startTime+1000*15)) {
				it = m_compassPosList.erase(it);
				continue;
			}
			++it;
		}
	}

	const int maxParty = 32;
	int delta = 128+16;
	float screenMapWidth = 128, screenMapHeight = 128;
	float screenMapCenterX = g_renderer->m_width-delta+screenMapWidth/2;
	float screenMapCenterY = 16+screenMapHeight/2;

	int attrW = m_world->m_attr->m_width;
	int attrH = m_world->m_attr->m_height;

	int maxAttrLen = attrW;
	if (maxAttrLen < attrH)
		maxAttrLen = attrH;
	screenMapWidth = float(screenMapWidth) * float(attrW) / float(maxAttrLen);
	screenMapHeight = float(screenMapHeight) * float(attrH) / float(maxAttrLen);

	sAssert(m_compassPosList.size()<=maxParty);

	int halfRect = 1;

	for (std::map<DWORD, ColorCellPos2>::iterator it = m_compassPosList.begin(); it != m_compassPosList.end(); ++it) {

		DWORD aid = (*it).first;
		float attrX = (*it).second.x;
		float attrY = (*it).second.y;
		DWORD	argb = (*it).second.color;
		int		type = (*it).second.type;
		DWORD	startTime = (*it).second.startTime;

		int deltaTime = timeGetTime()-startTime;
		if ((deltaTime%1000) >= 500)
			continue;


		float marginX  = ( float(screenMapSize) - float(screenMapWidth) ) / 2 ;
		float marginY = ( float(screenMapSize) - float(screenMapHeight) ) / 2 ;

		float pMapDeltaX = ( (float(attrX) * float(screenMapWidth) / float(attrW)) + marginX ) / (uvRect.u2 - uvRect.u1);
		float pMapDeltaY = (screenMapHeight - (float(attrY) * float(screenMapHeight) / float(attrH)) + marginY ) / (uvRect.v2 - uvRect.v1);
		float mpx = screenMapCenterX - screenMapSize/2 - float(screenMapSize)*uvRect.u1/(uvRect.u2 - uvRect.u1) + pMapDeltaX;
		float mpy = screenMapCenterY - screenMapSize/2 - float(screenMapSize)*uvRect.v1/(uvRect.v2 - uvRect.v1) + pMapDeltaY;

		if ((mpx-halfRect) < screenMapCenterX - screenMapWidth/2)
			continue;
		if ((mpx+halfRect) > screenMapCenterX + screenMapWidth/2)
			continue;
		if ((mpy-halfRect) < screenMapCenterY - screenMapHeight/2)
			continue;
		if ((mpy+halfRect) > screenMapCenterY + screenMapHeight/2)
			continue;

		RECT rect = { mpx-halfRect*4, mpy-halfRect, mpx+halfRect*4, mpy+halfRect };
		RECT rect2 = { mpx-halfRect, mpy-halfRect*4, mpx+halfRect, mpy+halfRect*4 };
		float oow = 1.0f/NEARPLANE-0.0001f;

		DrawTexture(&rect, 0, oow, argb, g_texMgr.GetDummyTexture());
		DrawTexture(&rect2, 0, oow, argb, g_texMgr.GetDummyTexture());
	}
}

void	CGameMode :: DrawActorsOnMiniMap(const UVRECT &uvRect)
{
	const int screenMapSize = 128;


	if (0 == m_partyPosList.size() && 0 == m_guildPosList.size())
		return;

	const int maxParty = 32;
	int delta = 128+16;
	float screenMapWidth = 128, screenMapHeight = 128;
	float screenMapCenterX = g_renderer->m_width-delta+screenMapWidth/2;
	float screenMapCenterY = 16+screenMapHeight/2;

	int attrW = m_world->m_attr->m_width;
	int attrH = m_world->m_attr->m_height;

	int maxAttrLen = attrW;
	if (maxAttrLen < attrH)
		maxAttrLen = attrH;
	screenMapWidth = float(screenMapWidth) * float(attrW) / float(maxAttrLen);
	screenMapHeight = float(screenMapHeight) * float(attrH) / float(maxAttrLen);

	sAssert(m_partyPosList.size()<=maxParty);

	int halfRect = 2;
	std::map<DWORD, ColorCellPos>::iterator it;
	for ( it = m_partyPosList.begin(); it != m_partyPosList.end(); it++) {

		DWORD aid = (*it).first;
		float attrX = (*it).second.x;
		float attrY = (*it).second.y;
		DWORD	argb = (*it).second.color;


		float marginX  = ( float(screenMapSize) - float(screenMapWidth) ) / 2 ;
		float marginY = ( float(screenMapSize) - float(screenMapHeight) ) / 2 ;

		float pMapDeltaX = ( (float(attrX) * float(screenMapWidth) / float(attrW)) + marginX ) / (uvRect.u2 - uvRect.u1);
		float pMapDeltaY = (screenMapHeight - (float(attrY) * float(screenMapHeight) / float(attrH)) + marginY ) / (uvRect.v2 - uvRect.v1);
		float mpx = screenMapCenterX - screenMapSize/2 - float(screenMapSize)*uvRect.u1/(uvRect.u2 - uvRect.u1) + pMapDeltaX;
		float mpy = screenMapCenterY - screenMapSize/2 - float(screenMapSize)*uvRect.v1/(uvRect.v2 - uvRect.v1) + pMapDeltaY;

		if ((mpx-halfRect) < screenMapCenterX - screenMapWidth/2)
			continue;
		if ((mpx+halfRect) > screenMapCenterX + screenMapWidth/2)
			continue;
		if ((mpy-halfRect) < screenMapCenterY - screenMapHeight/2)
			continue;
		if ((mpy+halfRect) > screenMapCenterY + screenMapHeight/2)
			continue;

		RECT rect = { mpx-halfRect, mpy-halfRect, mpx+halfRect, mpy+halfRect };
		float oow = 1.0f/NEARPLANE-0.0001f;


		RECT rect2= { rect.left - 1, rect.top - 1, rect.right + 1, rect.bottom + 1 };
		DrawTexture(&rect2, 0, oow, 0x99ffffff, g_texMgr.GetDummyTexture());
		DrawTexture(&rect, 0, oow, argb, g_texMgr.GetDummyTexture());
	}

	for (it = m_guildPosList.begin(); it != m_guildPosList.end(); it++) {

		DWORD aid = (*it).first;
		float attrX = (*it).second.x;
		float attrY = (*it).second.y;
		DWORD	argb = (*it).second.color;


		float marginX  = ( float(screenMapSize) - float(screenMapWidth) ) / 2 ;
		float marginY = ( float(screenMapSize) - float(screenMapHeight) ) / 2 ;

		float pMapDeltaX = ( (float(attrX) * float(screenMapWidth) / float(attrW)) + marginX ) / (uvRect.u2 - uvRect.u1);
		float pMapDeltaY = (screenMapHeight - (float(attrY) * float(screenMapHeight) / float(attrH)) + marginY ) / (uvRect.v2 - uvRect.v1);
		float mpx = screenMapCenterX - screenMapSize/2 - float(screenMapSize)*uvRect.u1/(uvRect.u2 - uvRect.u1) + pMapDeltaX;
		float mpy = screenMapCenterY - screenMapSize/2 - float(screenMapSize)*uvRect.v1/(uvRect.v2 - uvRect.v1) + pMapDeltaY;

		if ((mpx-halfRect) < screenMapCenterX - screenMapWidth/2)
			continue;
		if ((mpx+halfRect) > screenMapCenterX + screenMapWidth/2)
			continue;
		if ((mpy-halfRect) < screenMapCenterY - screenMapHeight/2)
			continue;
		if ((mpy+halfRect) > screenMapCenterY + screenMapHeight/2)
			continue;

		RECT rect = { mpx-halfRect, mpy-halfRect, mpx+halfRect, mpy+halfRect };
		float oow = 1.0f/NEARPLANE-0.0001f;



		if (m_partyPosList.find(it->first) == m_partyPosList.end())
		{

			DrawMinimapTriangle(mpx, mpy, ( halfRect + 1 ), 0xaaffffff);
			DrawMinimapTriangle(mpx, mpy, halfRect, argb);
		}
		else
			continue;
	}
}

void	CGameMode :: DrawMinimapTriangle(float fX, float fY, float trisize, DWORD argb)
{
	RPQuadFace& rpFace = g_renderer->BorrowQuadRP();
	const float oow = 1.0f/NEARPLANE-0.0001f;

	rpFace.tex = g_texMgr.GetDummyTexture();

	tlvertex3d*	verts = rpFace.verts;

	float		z = GetZFromOow(oow);
	float		z1 = GetZFromOow(oow);











	verts[0].x = fX;
	verts[0].y = fY - 2 * trisize;
	verts[1].x = fX - _sqrt(3) * trisize;
	verts[1].y = fY + trisize;
	verts[2].x = fX + _sqrt(3) * trisize;
	verts[2].y = fY + trisize;
	verts[3].x = fX - _sqrt(3) * trisize;
	verts[3].y = fY + trisize;

	verts[0].z = z1;
	verts[0].oow = oow;
	verts[0].tu = 0;
	verts[0].tv = 0;
	verts[0].color = argb;
	verts[1].z = z1;
	verts[1].oow = oow;
	verts[1].tu = 1;
	verts[1].tv = 0;
	verts[1].color = argb;
	verts[2].z = z;
	verts[2].oow = oow;
	verts[2].tu = 0;
	verts[2].tv = 1;
	verts[2].color = argb;
	verts[3].z = z;
	verts[3].oow = oow;
	verts[3].tu = 1;
	verts[3].tv = 1;
	verts[3].color = argb;

	g_renderer->AddRP(&rpFace, RF_UI);
}

void	CGameMode :: DrawBossMonsterOnMiniMap(const UVRECT &uvRect)
{
	const int screenMapSize = 128;

	float attrX = m_posOfBossMon.x ;
	float attrY = m_posOfBossMon.y ;

	if ( attrX < 0 || attrY < 0 )
		return ;

	int delta = 128+16;
	float screenMapWidth = 128, screenMapHeight = 128;
	float screenMapCenterX = g_renderer->m_width-delta+screenMapWidth/2;
	float screenMapCenterY = 16+screenMapHeight/2;

	int attrW = m_world->m_attr->m_width;
	int attrH = m_world->m_attr->m_height;

	int maxAttrLen = attrW;
	if (maxAttrLen < attrH)
		maxAttrLen = attrH;
	screenMapWidth = float(screenMapWidth) * float(attrW) / float(maxAttrLen);
	screenMapHeight = float(screenMapHeight) * float(attrH) / float(maxAttrLen);

	int halfRect = 6;

	{
		DWORD	argb = 0xffffffff;


		float marginX  = ( float(screenMapSize) - float(screenMapWidth) ) / 2 ;
		float marginY = ( float(screenMapSize) - float(screenMapHeight) ) / 2 ;

		float pMapDeltaX = ( (float(attrX) * float(screenMapWidth) / float(attrW)) + marginX ) / (uvRect.u2 - uvRect.u1);
		float pMapDeltaY = (screenMapHeight - (float(attrY) * float(screenMapHeight) / float(attrH)) + marginY ) / (uvRect.v2 - uvRect.v1);
		float mpx = screenMapCenterX - screenMapSize/2 - float(screenMapSize)*uvRect.u1/(uvRect.u2 - uvRect.u1) + pMapDeltaX;
		float mpy = screenMapCenterY - screenMapSize/2 - float(screenMapSize)*uvRect.v1/(uvRect.v2 - uvRect.v1) + pMapDeltaY;


		if ((mpx-halfRect) < screenMapCenterX - screenMapWidth/2)
			mpx = screenMapCenterX - screenMapWidth/2 + halfRect ;
		if ((mpx+halfRect) > screenMapCenterX + screenMapWidth/2)
			mpx = screenMapCenterX + screenMapWidth/2 - halfRect ;
		if ((mpy-halfRect) < screenMapCenterY - screenMapHeight/2)
			mpy = screenMapCenterY - screenMapHeight/2 + halfRect ;
		if ((mpy+halfRect) > screenMapCenterY + screenMapHeight/2)
			mpy = screenMapCenterY + screenMapHeight/2 - halfRect ;

		RECT rect = { mpx-halfRect, mpy-halfRect, mpx+halfRect, mpy+halfRect };
		float oow = 1.0f/NEARPLANE-0.0001f;

		DrawTexture(&rect, 0, oow, argb, g_texMgr.GetTexture("\xC0\xAF\xC0\xFA\xC0\xCE\xC5\xCD\xC6\xE4\xC0\xCC\xBD\xBA\\map\\bossmonster.bmp", false));
	}
}

void	CGameMode :: RotatePoint(float cx, float cy, float & x, float & y, int angle)
{
	float	cs = GetCos(angle);
	float	sn = GetSin(angle);
	float tmp_x = cx + ((x-cx) * cs - (y-cy) * sn);
	float tmp_y = cy + ((x-cx) * sn + (y-cy) * cs);
	x = tmp_x;
	y = tmp_y;
}

void	CGameMode :: MakeGroundMousePointer()
{
	m_mousePointer = new CMousePointer;
	m_world->m_gameObjectList.push_back(m_mousePointer);
}

void CGameMode::ShowMousePointer(float cx, float cy, int color)
{
	vector2d cPos(cx, cy);
	m_mousePointer->SendMsg(NULL, AM_SETPOSMOUSE, (int)&cPos, color, 0);
}

void	CGameMode :: PlaceEffect(vector3d * pos, int effectId, int lastTime, int F1)
{
	CDummyEffectLauncher * dummyEffectLauncher = new CDummyEffectLauncher;
	if (dummyEffectLauncher)
	{
		dummyEffectLauncher->SendMsg(NULL, 14, int(pos), effectId, lastTime , F1);
		m_world->m_gameObjectList.push_back(dummyEffectLauncher);
	}
}

void CGameMode::HideMousePointer(int time)
{
	if (m_mousePointer) m_mousePointer->SendMsg(NULL, AM_HIDEMOUSE, time, 0, 0);
}

void CGameMode::MakeCorpse3d(float cx, float cy, float cz, const char* rmaName, int curFrame, float roty, vector3d pivot, float modelZoom) {}

void CGameMode::MakeCorpse(float cx, float cy, float cz, const char* actName, const char* sprName, int action, int motion, float roty)
{
	CCorpse* corpse = new CCorpse;
	if (corpse)
	{
		m_world->m_gameObjectList.push_back(corpse);

		corpse->m_pos = vector3d(cx, cy, cz);
		corpse->SetActName(actName);
		corpse->SetSprName(sprName);
		corpse->m_baseAction = corpse->m_curAction = action;
		corpse->m_curMotion = motion;
		corpse->m_roty = roty;
	}
}

void CGameMode::MakeGrannyCorpse3d(float cx, float cy, float cz, float fRot, const char* Gr2Name)
{
	CGrannyCorpse3d* corpse = new CGrannyCorpse3d((char*)Gr2Name);
	corpse->SetPos(vector3d(cx, cy, cz));
	corpse->SetRot(fRot);
	m_world->m_gameObjectList.push_back(corpse);
}

CMsgEffect * CGameMode :: MakeMsgEffect(MAKE_MSG_EFFECT_TYPE EffectMakeType, vector3d &sPos, DWORD masterGid)
{
	SprActName sprActName;
	int actionNum = 0;
	int msgEffectType = MET_JUST_ANIMATE;
	switch (EffectMakeType)
	{
	case	MAKETYPE_MVP_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\mvp.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\mvp.act";
		actionNum = 0;
		msgEffectType = MET_MVP;
		break;

	case	MAKETYPE_CART_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT2:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "1.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "1.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT3:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "2.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "2.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT4:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "3.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "3.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT5:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "4.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "4.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT6:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbd\xb4\xb3\xeb\xbc\xd5\xbc\xf6\xb7\xb9.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbd\xb4\xb3\xeb\xbc\xd5\xbc\xf6\xb7\xb9.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT11:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xba\xa3\xc0\xcc\xba\xf1\xbc\xd5\xbc\xf6\xb7\xb9.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT12:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "1.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xba\xa3\xc0\xcc\xba\xf1\xbc\xd5\xbc\xf6\xb7\xb9" "1.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT13:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "2.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xba\xa3\xc0\xcc\xba\xf1\xbc\xd5\xbc\xf6\xb7\xb9" "2.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT14:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "3.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xba\xa3\xc0\xcc\xba\xf1\xbc\xd5\xbc\xf6\xb7\xb9" "3.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT15:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9" "4.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xba\xa3\xc0\xcc\xba\xf1\xbc\xd5\xbc\xf6\xb7\xb9" "4.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_CART_EFFECT16:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbd\xb4\xb3\xeb\xbc\xd5\xbc\xf6\xb7\xb9.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xba\xa3\xc0\xcc\xba\xf1\xbd\xb4\xb3\xeb\xbc\xd5\xbc\xf6\xb7\xb9.act";
		actionNum = 0;
		msgEffectType = MET_CART;
		break;
	case	MAKETYPE_BIRD_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xb8\xc5.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xb8\xc5.act";
		actionNum = 0;
		msgEffectType = MET_BIRD;
		break;
	case	MAKETYPE_BIRD_EFFECT2:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xb8\xc5" "2.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xb8\xc5" "2.act";
		actionNum = 0;
		msgEffectType = MET_BIRD;
		break;
	case	MAKETYPE_BIRD_EFFECT3:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xb8\xc5.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xba\xa3\xc0\xcc\xba\xf1\xb8\xc5.act";
		actionNum = 0;
		msgEffectType = MET_BIRD;
		break;
	case	MAKETYPE_BIRD_EFFECT4:
		sprActName.sprName = "\xb8\xf3\xbd\xba\xc5\xcd\\demon_pungus.spr";
		sprActName.actName = "\xb8\xf3\xbd\xba\xc5\xcd\\demon_pungus.act";
		actionNum = 0;
		msgEffectType = MET_BIRD;
		break;
	case	MAKETYPE_BIRD_EFFECT5:
		sprActName.sprName = "\xb8\xf3\xbd\xba\xc5\xcd\\explosion.spr";
		sprActName.actName = "\xb8\xf3\xbd\xba\xc5\xcd\\explosion.act";
		actionNum = 0;
		msgEffectType = MET_BIRD;
		break;
	case	MAKETYPE_BIRD_EFFECT6:
		sprActName.sprName = "\xb8\xf3\xbd\xba\xc5\xcd\\lude.spr";
		sprActName.actName = "\xb8\xf3\xbd\xba\xc5\xcd\\lude.act";
		actionNum = 0;
		msgEffectType = MET_BIRD;
		break;
	case	MAKETYPE_PK_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\emotion.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\emotion.act";
		actionNum = 8;
		msgEffectType = MET_PKEFFECT;
		break;
	case	MAKETYPE_ICE_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbe\xf3\xc0\xbd\xb6\xaf.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbe\xf3\xc0\xbd\xb6\xaf.act";
		actionNum = 0;
		msgEffectType = MET_ICEEFFECT;
		break;
	case	MAKETYPE_ICEBREAK_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbe\xf3\xc0\xbd\xb6\xaf.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\\xbe\xf3\xc0\xbd\xb6\xaf.act";
		actionNum = 1;
		msgEffectType = MET_ICEBREAKEFFECT;
		break;
	case	MAKETYPE_SMOKE_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\smoke.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\smoke.act";
		actionNum = 0;
		msgEffectType = MET_ICEBREAKEFFECT;
		break;
	case	MAKETYPE_CURSE_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\Status-curse.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\Status-curse.act";
		actionNum = 0;
		msgEffectType = MET_CURSE_EFFECT;
		break;
	case	MAKETYPE_SLEEP_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\Status-sleep.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\Status-sleep.act";
		actionNum = 0;
		msgEffectType = MET_SLEEP_EFFECT;
		break;
	case	MAKETYPE_STUN_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\Status-stun.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\Status-stun.act";
		actionNum = 0;
		msgEffectType = MET_STUN_EFFECT;
		break;
	case	MAKETYPE_SILENCE_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\emotion.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\emotion.act";
		actionNum = 10;
		msgEffectType = MET_SILENCE_EFFECT;
		break;
	case	MAKETYPE_ANGELUS_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
		actionNum = 1;
		msgEffectType = MET_ANGELUS_EFFECT;
		break;
	case	MAKETYPE_CRUSIS_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
		actionNum = 2;
		msgEffectType = MET_CRUSIS_EFFECT;
		break;
	case	MAKETYPE_CRITICAL_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
		actionNum = 2;
		msgEffectType = MET_CRITICAL_EFFECT;
		break;
	case	MAKETYPE_BLUE_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\bluemsg.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
		actionNum = 2;
		msgEffectType = MET_BLUE_EFFECT;
		break;
	case	MAKETYPE_LUCKY_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
		actionNum = 3;
		msgEffectType = MET_LUCKY_EFFECT;
		break;
	case	MAKETYPE_GUARD_EFFECT:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
		actionNum = 1;
		msgEffectType = MET_GUARD;
		break;
#if defined (_HOMUN)
	case	MAKETYPE_CURSOR_EFFECT:
		sprActName.sprName = "cursors.spr";
		sprActName.actName = "cursors.act";
		actionNum = 10;
		msgEffectType = MET_CURSOR_EFFECT;
		break;
#endif
	default:
		sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.spr";
		sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
		break;
	}

	CMsgEffect * msgEffect = new CMsgEffect;
	if (msgEffect)
	{
		m_world->m_gameObjectList.push_back(msgEffect);
		msgEffect->SendMsg(NULL, AM_SETMASTERGID, masterGid, msgEffectType);
		msgEffect->SendMsg(NULL, AM_SETPOS, (int)(&sPos), int(&sprActName), actionNum);
	}
	return msgEffect;
}



CMsgEffect* CGameMode::MakeMissEffect(vector3d &sPos, DWORD sprArgb, DWORD masterGid)
{
	SprActName sprActName;
	sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.spr";
	sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\msg.act";
	int actionNum = 0;
	int msgEffectType = MET_MISS;

	CMsgEffect* missEffect = new CMsgEffect;
	if (missEffect) {
		m_world->m_gameObjectList.push_back(missEffect);
		missEffect->SendMsg(NULL, AM_SETMASTERGID, masterGid, msgEffectType);
		missEffect->SendMsg(NULL, AM_SETPOS, (int)(&sPos), (int)(&sprActName), actionNum);
		missEffect->m_sprArgb = sprArgb;
	}
	return missEffect;
}

void	CGameMode :: MakeArrowEffectPos(vector3d &sPos, vector3d &tPos, float hideTime)
{
	CArrowEffect * arrowEffect = new CArrowEffect;
	if (arrowEffect)
	{
		m_world->m_gameObjectList.push_back(arrowEffect);
		arrowEffect->SendMsg(NULL, 64, (int)(&sPos), (int)(&tPos), (int)(&hideTime), 0);
	}
}

void	CGameMode :: MakeArrowEffect(vector3d &sPos, DWORD targetGid, float hideTime, int job)
{
	if (targetGid == 0)
		return;
	CArrowEffect * arrowEffect = new CArrowEffect;
	if (arrowEffect)
	{
		m_world->m_gameObjectList.push_back(arrowEffect);
		arrowEffect->SendMsg(NULL, 14, (int)(&sPos), targetGid, (int)(&hideTime), job);
	}
}

void	CGameMode :: MakeMandraAttackEffect(DWORD targetGid, int job, float hideTime)
{
	CMandraAttackEffect * mandraAttackEffect = new CMandraAttackEffect;
	if (mandraAttackEffect)
	{
		m_world->m_gameObjectList.push_back(mandraAttackEffect);
		mandraAttackEffect->SendMsg(NULL, 14, targetGid, (int)(&hideTime), job);
	}
}

void	CGameMode :: PushGameObject(CGameObject * gameObject)
{
	m_world->m_gameObjectList.push_back(gameObject);
}

CMsgEffect* CGameMode::MakeEmotion(vector3d &sPos, int emotionType, DWORD masterGid)
{
	SprActName sprActName;
	sprActName.sprName = "\xc0\xcc\xc6\xd1\xc6\xae\\emotion.spr";
	sprActName.actName = "\xc0\xcc\xc6\xd1\xc6\xae\\emotion.act";

	int actionNum = 0;
	switch (emotionType) {
		case	ET_SURPRISE:
			actionNum = 0;
			break;
		case	ET_QUESTION:
			actionNum = 1;
			break;
		case	ET_DELIGHT:
			actionNum = 2;
			break;
		case	ET_THROB:
			actionNum = 3;
			break;
		case	ET_SWEAT:
			actionNum = 5;
			break;
		case	ET_AHA:
			actionNum = 6;
			break;
		case	ET_FRET:
			actionNum = 7;
			break;
		case	ET_ANGER:
			actionNum = 8;
			break;
		case	ET_MONEY:
			actionNum = 9;
			break;
		case	ET_THINK:
			actionNum = 10;
			break;
		case	ET_SCISSOR:
			actionNum = 12;
			break;
		case	ET_ROCK:
			actionNum = 11;
			break;
		case	ET_WRAP:
			actionNum = 13;
			break;
		case	ET_FLAG:
			actionNum = 14;
			break;
		case	ET_BIGTHROB:
			actionNum = 4;
			break;
		case	ET_THANKS:
			actionNum = 15;
			break;
		case	ET_KEK:
			actionNum = 16;
			break;
		case	ET_SORRY:
			actionNum = 17;
			break;
		case	ET_SMILE:
			actionNum = 18;
			break;
		case	ET_PROFUSELY_SWEAT:
			actionNum = 19;
			break;
		case	ET_SCRATCH:
			actionNum = 20;
			break;
		case	ET_BEST:
			actionNum = 21;
			break;
		case	ET_STARE_ABOUT:
			actionNum = 22;
			break;
		case	ET_HUK:
			actionNum = 23;
			break;
		case	ET_O:
			actionNum = 24;
			break;
		case	ET_X:
			actionNum = 25;
			break;
		case	ET_HELP:
			actionNum = 26;
			break;
		case	ET_GO:
			actionNum = 27;
			break;
		case	ET_CRY:
			actionNum = 28;
			break;
		case	ET_KIK:
			actionNum = 29;
			break;
		case	ET_CHUP:
			actionNum = 30;
			break;
		case	ET_CHUPCHUP:
			actionNum = 31;
			break;
		case	ET_HNG:
			actionNum = 32;
			break;
		case	ET_OK:
			actionNum = 33;
			break;
		case	ET_CHAT_PROHIBIT:
			{
				actionNum = 1000;
				CMsgEffect * emotion = new CMsgEffect;
				m_world->m_gameObjectList.push_back(emotion);
				emotion->SendMsg(NULL, AM_SETMASTERGID, masterGid, MET_EMOTION);
				emotion->SendMsg(NULL, AM_SETPOS, (int)(&sPos), (int)(&sprActName), actionNum);
				return emotion;
			}
			break;
		case	ET_INDONESIA_FLAG:
			actionNum = 34;
			break;
		case	ET_STARE:
			actionNum = 35;
			break;
		case	ET_HUNGRY:
			actionNum = 36;
			break;
		case	ET_COOL:
			actionNum = 37;
			break;
		case	ET_MERONG:
			actionNum = 38;
			break;
		case	ET_SHY:
			actionNum = 39;
			break;
		case	ET_GOODBOY:
			actionNum = 40;
			break;
		case	ET_SPTIME:
			actionNum = 41;
			break;
		case	ET_SEXY:
			actionNum = 42;
			break;
		case	ET_COMEON:
			actionNum = 43;
			break;
		case	ET_SLEEPY:
			actionNum = 44;
			break;
		case	ET_CONGRATULATION:
			actionNum = 45;
			break;
		case	ET_HPTIME:
			actionNum = 46;
			break;
		case	ET_PH_FLAG:
			actionNum = 47;
			break;
		case	ET_MY_FLAG:
			actionNum = 48;
			break;
		case	ET_SI_FLAG:
			actionNum = 49;
			break;
		case	ET_BR_FLAG:
			actionNum = 50;
			break;
		case	ET_SPARK:
			actionNum = 51;
			break;
		case	ET_CONFUSE:
			actionNum = 52;
			break;
		case	ET_OHNO:
			actionNum = 53;
			break;
		case	ET_HUM:
			actionNum = 54;
			break;
		case	ET_BLABLA:
			actionNum = 55;
			break;
		case	ET_OTL:
			actionNum = 56;
			break;
		case	ET_DICE1:
			actionNum = 57;
			break;
		case	ET_DICE2:
			actionNum = 58;
			break;
		case	ET_DICE3:
			actionNum = 59;
			break;
		case	ET_DICE4:
			actionNum = 60;
			break;
		case	ET_DICE5:
			actionNum = 61;
			break;
		case	ET_DICE6:
			actionNum = 62;
			break;
		case	ET_INDIA_FLAG:
			actionNum = 63;
			break;
		case	ET_LUV:
			actionNum = 64;
			break;
		case	ET_FLAG8:
			actionNum = 65;
			break;
		case	ET_FLAG9:
			actionNum = 66;
			break;
		case	ET_MOBILE:
			actionNum = 67;
			break;
		case	ET_MAIL:
			actionNum = 68;
			break;
		case	ET_ANTENNA0:
			actionNum = 69;
			break;
		case	ET_ANTENNA1:
			actionNum = 70;
			break;
		case	ET_ANTENNA2:
			actionNum = 71;
			break;
		case	ET_ANTENNA3:
			actionNum = 72;
			break;

		case	ET_HUM2:
			actionNum = 73;
			break;
		case	ET_ABS:
			actionNum = 74;
			break;
		case	ET_OOPS:
			actionNum = 75;
			break;
		case	ET_SPIT:
			actionNum = 76;
			break;
		case	ET_ENE:
			actionNum = 77;
			break;
		case	ET_PANIC:
			actionNum = 78;
			break;
		case	ET_WHISP:
			actionNum = 79;
			break;

		default:

			actionNum = 0;
			break;
	}

	CMsgEffect* emotion = new CMsgEffect;
	m_world->m_gameObjectList.push_back(emotion);
	emotion->SendMsg(NULL, AM_SETMASTERGID, masterGid, MET_EMOTION);
	emotion->SendMsg(NULL, AM_SETPOS, (int)(&sPos), (int)(&sprActName), actionNum);
	return emotion;
}

void CGameMode::MakeNumEffect(vector3d &sPos, int number, unsigned long sprArgb, unsigned long masterGid)
{
	if (number > 9999)
		number = 9999;
	if (number < 0)
		number = 0;

	int numberCnt = 1;
	if (number > 999)
		numberCnt = 4;
	else if (number > 99)
		numberCnt = 3;
	else if (number > 9)
		numberCnt = 2;

	int eachNum[4] = { 9, 9, 9, 9 };
	eachNum[0] = number % 10;
	number /= 10;
	eachNum[1] = number % 10;
	number /= 10;
	eachNum[2] = number % 10;
	number /= 10;
	eachNum[3] = number;

	for (int i = 0; i < numberCnt; i++) {
		CNumEffect *numEffect = new CNumEffect;
		if (numEffect) {
			m_world->m_gameObjectList.push_back(numEffect);
			numEffect->SendMsg(NULL, AM_SETPOS, int(&sPos), eachNum[i], -i * 8 + (4 * (numberCnt - 1)));
			numEffect->m_sprArgb = sprArgb;
			numEffect->SendMsg(NULL, AM_SETMASTERGID, masterGid, 0, 0);
		}
	}
}


void	CGameMode :: MakeBlowEffect(float cx, float cy, float cz)
{
	CBlowEffect * blowEffect = new CBlowEffect;
	if (blowEffect)
	{
		m_world->m_gameObjectList.push_back(blowEffect);
		vector3d cPos(cx, cy, cz);
		blowEffect->SendMsg(NULL, 14, (int)(&cPos), 0, 0);
	}
}

bool	CGameMode :: IsInsideCell(int &sx, int &sy, int &dx, int &dy, int numCell)
{
	if (abs(sx - dx)>numCell)
		return false;
	if (abs(sy - dy)>numCell)
		return false;
	return true;
}

bool	CGameMode :: IsReqPickupItemNaid(DWORD itemNaid)
{
	std::list<DWORD>::iterator it = m_pickupReqItemNaidList.begin();
	while (it != m_pickupReqItemNaidList.end()) {
		if ((*it) == itemNaid)
			return true;
		++it;
	}
	return false;
}

void	CGameMode :: RemovePickupItemNaid(DWORD itemNaid)
{
	std::list<DWORD>::iterator it = m_pickupReqItemNaidList.begin();
	while (it != m_pickupReqItemNaidList.end()) {
		if ((*it) == itemNaid) {
			m_pickupReqItemNaidList.erase(it);
			return;
		}
		++it;
	}
}

void	CGameMode :: AddPickupItemNaid(DWORD itemNaid)
{
	m_pickupReqItemNaidList.push_back(itemNaid);
}

void	CGameMode :: ConvertToNumCellClose(int &sx, int &sy, int &dx, int &dy, int numCell)
{
	if (sx>dx)
		dx += numCell;
	else if (sx<dx)
		dx -= numCell;

	if (sy>dy)
		dy += numCell;
	else if (sy<dy)
		dy -= numCell;
}

void	CGameMode :: ConvertToNumCellCloseAndFindNoRedCell(float x1, float y1, float x2, float y2, int &dx, int &dy, int numCell)
{
	vector2d dPos(x1-x2, y1-y2);
	dPos.Normalize();
	dPos.x *= numCell*5;
	dPos.y *= numCell*5;

	GetServerCoor(x2+dPos.x, y2+dPos.y, dx, dy);

	if (!IsRedCell(dx, dy))
		return;

	int dx2 = dx;
	int dy2 = dy;

	int offset[8][2] = { {1,0}, {1,1}, {1,-1}, {-1,0}, {-1,1}, {-1,-1}, {0,1}, {0,-1}, };
	for (int i=0; i<8; ++i) {
		dx = dx2+offset[i][0];
		dy = dy2+offset[i][1];
		if (!IsRedCell(dx, dy))
			return;
	}
}

void	CGameMode :: ConvertToNoRedCell(int &sx, int &sy, int &dx, int &dy)
{
	int dx2 = dx;
	int dy2 = dy;

	if (sx>dx)
		dx += 1;
	else if (sx<dx)
		dx -= 1;

	if (sy>dy)
		dy += 1;
	else if (sy<dy)
		dy -= 1;

	if (!IsRedCell(dx, dy))
		return;

	int offset[8][2] = { {1,0}, {1,1}, {1,-1}, {-1,0}, {-1,1}, {-1,-1}, {0,1}, {0,-1}, };
	for (int i=0; i<8; ++i) {
		dx = dx2+offset[i][0];
		dy = dy2+offset[i][1];
		if (!IsRedCell(dx, dy))
			return;
	}
}

void	CGameMode :: GetServerCoor(float cx, float cz, int &sx, int &sy, int &cellX, int &cellY)
{
	float zoom = m_world->m_attr->m_zoom;

	sx = floor(cx/zoom + m_world->m_attr->m_width/2);
	sy = floor(cz/zoom + m_world->m_attr->m_height/2);

	cellX = int(floor((cx/zoom + m_world->m_attr->m_width/2 - sx) *16))%16;
	cellY = int(floor((cz/zoom + m_world->m_attr->m_height/2 - sy)*16))%16;
}

void	CGameMode :: GetServerCoor(float cx, float cz, int &sx, int &sy)
{
	float zoom = m_world->m_attr->m_zoom;

	sx = floor(cx/zoom + m_world->m_attr->m_width/2);
	sy = floor(cz/zoom + m_world->m_attr->m_height/2);
}

void	CGameMode :: GetClientCoor(int sx, int sy, float &cx, float &cz)
{
	float halfCell = float(m_world->m_attr->m_zoom)*8/16;
	cx = (sx-float(m_world->m_attr->m_width)/2.0f)*float(m_world->m_attr->m_zoom) + halfCell;
	cz = (sy-float(m_world->m_attr->m_height)/2.0f)*float(m_world->m_attr->m_zoom) + halfCell;
}

void	CGameMode :: GetClientCoor(int sx, int sy, int subX, int subY, float &cx, float &cz)
{
	cx = (sx-float(m_world->m_attr->m_width)/2.0f)*float(m_world->m_attr->m_zoom) + float(subX)*float(m_world->m_attr->m_zoom)/16.0f;
	cz = (sy-float(m_world->m_attr->m_height)/2.0f)*float(m_world->m_attr->m_zoom) + float(subY)*float(m_world->m_attr->m_zoom)/16.0f;
}

bool	CGameMode :: CanFindPath(int sx, int sy, int cellX, int cellY, int dx, int dy)
{
	CPathInfo	pathInfo;
	return g_pathFinder.FindPath(timeGetTime(), sx, sy, cellX, cellY, dx, dy, 150, &pathInfo);
}

bool	CGameMode :: GetClosestValidCell(int &dx, int &dy)
{
	int sx, sy, cellX, cellY;
	GetServerCoor(m_world->m_player->m_pos.x, m_world->m_player->m_pos.z, sx, sy, cellX, cellY);

	int deltaX = sx-dx, deltaY = sy-dy;
	int offset[9][2] = { {0,0}, {1,0}, {1,1}, {1,-1}, {-1,0}, {-1,1}, {-1,-1}, {0,1}, {0,-1}, };

	if (abs(deltaX) >= abs(deltaY))
	{
		if (deltaX == 0)
			return false;
		int inc = (deltaX>0)?1:-1;
		for (int x=dx; x!=sx; x+=inc)
		{
			int y = dy + (deltaY*(x-dx)/deltaX);

			for (int offIndex=0; offIndex<9; ++offIndex)
			{
				if (!IsRedCell(x+offset[offIndex][0], y+offset[offIndex][1]))
				{
					if (!IsRegisterCell(x+offset[offIndex][0], y+offset[offIndex][1]))
					{
						if (CanFindPath(sx, sy, cellX, cellY, x+offset[offIndex][0], y+offset[offIndex][1]))
						{
							dx = x+offset[offIndex][0];
							dy = y+offset[offIndex][1];
							return true;
						}
						else
						{
							return false;
						}
					}
				}
			}
		}
	} else {
		if (deltaY == 0)
			return false;
		int inc = (deltaY>0)?1:-1;
		for (int y=dy; y!=sy; y+=inc) {
			int x = dx + (deltaX*(y-dy)/deltaY);

			for (int offIndex=0; offIndex<9; ++offIndex) {
				if (!IsRedCell(x+offset[offIndex][0], y+offset[offIndex][1])) {
					if (!IsRegisterCell(x+offset[offIndex][0], y+offset[offIndex][1])) {
						if (CanFindPath(sx, sy, cellX, cellY, x+offset[offIndex][0], y+offset[offIndex][1])) {
							dx = x+offset[offIndex][0];
							dy = y+offset[offIndex][1];
							return true;
						} else {
							return false;
						}
					}
				}
			}
		}
	}
	return false;
}

int		CGameMode :: GetCellFlag(int x, int y)
{
	CAttrCell * cell = m_world->m_attr->GetCell(x, y);
	if (cell == NULL)
		return 1;
	return cell->flag;
}

bool	CGameMode :: IsRedCell(int x, int y)
{
	CAttrCell * cell = m_world->m_attr->GetCell(x, y);
	if (cell == NULL)
		return true;
	if (cell->flag==1 || cell->flag==5)
		return true;
	return false;
}

bool	CGameMode :: IsRegisterCell(int x, int y)
{
	std::map <DWORD, CellPos> :: iterator it = m_actorPosList.begin();
	while (it != m_actorPosList.end()) {
		if ((*it).second.x==x && (*it).second.y==y)
			return true;
		it++;
	}
	return false;
}

bool	CGameMode :: IsValidCell(int x, int y)
{
	if (IsRedCell(x, y))
		return false;

	int px = m_world->m_player->m_destCellX;
	int py = m_world->m_player->m_destCellZ;
	if (px==x && py==y) {

		return false;
	}

	int sx, sy, cellX, cellY;
	GetServerCoor(m_world->m_player->m_pos.x, m_world->m_player->m_pos.z, sx, sy, cellX, cellY);
	if (!CanFindPath(sx, sy, cellX, cellY, x, y))
		return false;

	if (IsRegisterCell(x, y))
		return false;

	return true;
}

const	char * CGameMode :: SetPaddingValue ( char * pad, int len )
{
	if(len==0) {
		len = sizeof ( pad );
	}

	char buf[20]  ={0,};
	DWORD value = timeGetTime ( ) >> len ;
	value ^= timeGetTime ( ) ;
	_ltoa ( value, buf, 16 ) ;

	int len2 = strlen ( buf ) - 1 ;

	for ( int i = 0 ; i < len ; i++ )
	{
		if ( len2 - i >= 0 )
			pad[i] = buf[len2 - i] ;
		else
			pad[i] = buf[len2] ;
	}


	pad[len-1] = 0 ;
	return pad ;
}
void	CGameMode :: SynchronizeTimeWithServer(bool syncNow)
{
	if (!syncNow && timeGetTime() <= m_syncRequestTime + 12000)
		return;

	m_receiveSyneRequestTime = 0;
	m_syncRequestTime = timeGetTime();

	PACKET_CZ_REQUEST_TIME p;
	p.PacketType = 0x89;
	SetPaddingValue(p.pad, sizeof(p.pad));
	p.clientTime = timeGetTime();
	CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(p.PacketType), (const char *)&p);

	if (g_pingLog)
	{
		g_pingLogList.push_back(timeGetTime());
		if (!g_pingLogStart)
			g_pingLogStart = g_pingLogList.front();
	}
}

DWORD	CGameMode :: GetActorNameRequestTimer(DWORD aid)
{
	std::map<DWORD,DWORD> :: iterator it = m_actorNameReqTimer.find(aid);

	if (it != m_actorNameReqTimer.end()) return timeGetTime() - it->second;

	return 2500;
}

void	CGameMode :: RemoveActorNameRequestTimer(DWORD aid)
{
	m_actorNameReqTimer.erase(aid);
}

void	CGameMode :: RequestActorName(DWORD aid)
{
	if (!aid) return;
	if (GetActorNameRequestTimer(aid) < 2500) return;

	m_actorNameReqTimer[aid] = timeGetTime();

	PACKET_CZ_REQNAME p;
	p.PacketType = 0x8c;
	SetPaddingValue(p.pad, sizeof(p.pad));
	p.AID = aid;
	CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(p.PacketType), (const char *)&p);

	m_actorNameList[aid].Clear();

	std::vector<DWORD>::iterator it = std::find(m_KillerList.begin(), m_KillerList.end(), aid);
	if (it != m_KillerList.end())
		m_KillerList.erase(it);
}

const NamePair&	CGameMode :: GetActorName(DWORD aid)
{
	std::map<DWORD, NamePair>::iterator iter = m_actorNameList.find(aid);

	if (iter != m_actorNameList.end())
	{
		return m_actorNameList[aid];
	}
	else
	{
		RequestActorName(aid);

		return GetNullNamePair();
	}
}

DWORD	CGameMode :: GetActorNameByGIDRequestTimer(DWORD gid)
{
	std::map<DWORD,DWORD> :: iterator it = m_actorNameByGIDReqTimer.find(gid);

	if (it != m_actorNameByGIDReqTimer.end()) return timeGetTime() - it->second;

	return 2500;
}

void	CGameMode :: RemoveActorNameByGIDRequestTimer(DWORD gid)
{
	m_actorNameByGIDReqTimer.erase(gid);
}

void	CGameMode :: RequestActorNameByGID(DWORD gid)
{
	if (!gid) return;
	if (GetActorNameByGIDRequestTimer(gid) < 2500) return;

	m_actorNameByGIDReqTimer[gid] = timeGetTime();

	PACKET_CZ_REQNAME_BYGID p;
	p.PacketType = 0xa2;
	SetPaddingValue(p.pad, sizeof(p.pad));
	p.GID = gid;
	CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(p.PacketType), (const char *)&p);
}

void	CGameMode :: CheckGuildMemberStatusCache(DWORD gid, const NamePair& actorName)
{
	std::map<DWORD, int>::iterator iter = m_guildMemberStatusCache.find(gid);
	if (iter != m_guildMemberStatusCache.end()) {
		SendMsg(MM_GUILD_MEMBER_STATUS_CHANGE, (int)actorName.cName.c_str(), iter->second, 0);
		m_guildMemberStatusCache.erase(iter);
	}
}



NamePair const & CGameMode::GetActorNameByGID(unsigned long gid)
{
	std::map<DWORD, NamePair>::iterator iter = m_actorNameListByGID.find(gid);

	if (iter != m_actorNameListByGID.end())
		return m_actorNameListByGID[gid];

	RequestActorNameByGID(gid);
	return GetNullNamePair();
}

COLORREF	CGameMode :: GetActorNameColor(int job, DWORD aid)
{
	if (IsNameYellow(aid))
		return RGB(255, 255, 0);
	else if (IsNameSpecific(aid))
		return IsNameSpecific(aid);
	else if (IsKillerAid(aid))
		return RGB(238, 109, 106);
	else {
		if (job > JT_WARPNPC && job < JT_MON_BEGIN)
			return RGB(149, 184, 247);
		else if (job > JT_MON_BEGIN && job < JT_2004_JOB_BEGIN)
			return RGB(255, 195, 195);
		else
			return RGB(255, 255, 255);
	}
}

void CGameMode::MakeNameBalloon(unsigned long aid)
{
	if (aid == 0)
		return;

	m_nameActorAid = aid;

	CGameActor *theActor = GetWorld()->GetGameActorByAID(aid);
	if (theActor) {
		if (theActor->GetJob() == JT_WARPNPC)
			return;

		m_nameDisplayed = FALSE;
		MakeActorNameBalloon(theActor);

		if (m_onCopyName && g_session.IsPc(theActor->GetJob()) && IsGravityAid(g_session.GetAid())) {
			g_windowMgr.AddWhisperCharName2(GetActorName(aid).cName.c_str());
		}
		return;
	}

	if (aid == g_session.GetAid()) {
		m_nameDisplayed = FALSE;
		MakeActorNameBalloon(GetWorld()->m_player);
		return;
	}

	CItem *item = GetWorld()->GetItemByAID(aid);
	if (item) {
		std::string nameMsg = item->m_itemName.c_str();
		m_nameDisplayed = FALSE;
		MakeNameBalloonTextWindow((UIWindow **)&m_nameBalloon);
		m_nameBalloon->SetNameText(nameMsg.c_str());
		m_nameBalloon->SetEmblemResName("");
		m_nameBalloon->SetFntColor(RGB(255, 239, 149), 0);
		m_nameBalloonWidth = m_nameBalloon->GetWidth();
		m_nameBalloonHeight = m_nameBalloon->GetHeight();
		MoveBalloonTextWindow(m_nameBalloon, item);
	}
}

void CGameMode::MakeActorNameBalloon(CGameActor *theActor)
{
	MakeNameBalloonTextWindow((UIWindow **)&m_nameBalloon);

	m_nameBalloon->SetNameText(GetActorName(theActor->GetGID()), theActor->IsThisPC(theActor->GetJob()) == FALSE);

	if (theActor->GetGdid() && g_session.GetEmblemState()) {
		char emblemResName[256];
		g_session.GetEmblemFileName(emblemResName, theActor->GetGdid(), theActor->GetEmblemVersion());
		m_nameBalloon->SetEmblemResName(emblemResName);
	} else {
		m_nameBalloon->SetEmblemResName("");
	}

	m_nameBalloonWidth = m_nameBalloon->GetWidth();
	m_nameBalloonHeight = m_nameBalloon->GetHeight();

	if (g_serviceType == ServiceBrazil) {
		switch (theActor->GetGID()) {
		case 1610001:
		case 1610002:
		case 1610003:
		case 1610004:
		case 1610006:
		case 1610007:
		case 1610009:
		case 1610010:
			m_nameBalloon->SetFntColor(RGB(255, 0, 0), RGB(100, 100, 100));
			break;
		default:
			m_nameBalloon->SetFntColor(GetActorNameColor(theActor->GetJob(), theActor->GetGID()), 0);
			break;
		}
	} else {
		m_nameBalloon->SetFntColor(GetActorNameColor(theActor->GetJob(), theActor->GetGID()), 0);
	}

	MoveBalloonTextWindow(m_nameBalloon, theActor);
}

void CGameMode::MakeAttackedActorNameBalloon(CGameActor *theActor)
{
	MakeNameBalloonTextWindow((UIWindow **)&m_targetNameBalloon);
	m_targetNameBalloon->SetNameText(GetActorName(theActor->GetGID()), theActor->IsThisPC(theActor->GetJob()) == FALSE);

	if (theActor->GetGdid() && g_session.GetEmblemState()) {
		char emblemResName[256];
		g_session.GetEmblemFileName(emblemResName, theActor->GetGdid(), theActor->GetEmblemVersion());
		m_targetNameBalloon->SetEmblemResName(emblemResName);
	} else {
		m_targetNameBalloon->SetEmblemResName("");
	}

	m_targetNameBalloon->SetFntColor(GetActorNameColor(theActor->GetJob(), theActor->GetGID()), 0);

	MoveBalloonTextWindow(m_targetNameBalloon, theActor);
}

void CGameMode::ProcessNameBalloon()
{
	if (m_nameBalloon) {
		if (m_nameDisplayed == FALSE) {
			m_nameDisplayed = 1;
			CGameActor *actor;
			CItem *item;
			if (m_nameActorAid == g_session.GetAid()) {
				MoveBalloonTextWindow(m_nameBalloon, m_world->m_player);
			} else if ((actor = GetWorld()->GetGameActorByAID(m_nameActorAid)) != NULL) {
				MoveBalloonTextWindow(m_nameBalloon, actor);
			} else if ((item = GetWorld()->GetItemByAID(m_nameActorAid)) != NULL) {
				MoveBalloonTextWindow(m_nameBalloon, item);
			}
		} else {
			g_windowMgr.PostQuit(m_nameBalloon);
			m_nameBalloon = NULL;
		}
	}

	if (m_targetNameBalloon) {
		if (m_nameDisplayed2 == FALSE) {
			m_nameDisplayed2 = 1;
			CGameActor *actor;
			CItem *item;
			if (m_nameActorAid == g_session.GetAid()) {
				MoveBalloonTextWindow(m_targetNameBalloon, m_world->m_player);
			} else if ((actor = GetWorld()->GetGameActorByAID(m_nameActorAid)) != NULL) {
				MoveBalloonTextWindow(m_targetNameBalloon, actor);
			} else if ((item = GetWorld()->GetItemByAID(m_nameActorAid)) != NULL) {
				MoveBalloonTextWindow(m_targetNameBalloon, item);
			}
		} else {
			g_windowMgr.PostQuit(m_targetNameBalloon);
			m_targetNameBalloon = NULL;
		}
	}

	if (m_broadcastBalloon) {
		if (timeGetTime() > m_broadCastTick + 20000) {
			g_windowMgr.PostQuit(m_broadcastBalloon);
			m_broadcastBalloon = NULL;
		}
	}

	if (m_skillUseInfo.m_skillUseType == SUT_NOSKILL) {
		if (m_skillNameBalloon) {
			g_windowMgr.PostQuit(m_skillNameBalloon);
			m_skillNameBalloon = NULL;
		}
		if (m_skillMsgBalloon) {
			g_windowMgr.PostQuit(m_skillMsgBalloon);
			m_skillMsgBalloon = NULL;
		}
	} else {
		char madeBalloon = false;
		if (m_skillNameBalloon == NULL) {
			m_skillNameBalloon = new UITransBalloonText;
			m_skillNameBalloon->Create(8, 8);
			g_windowMgr.AddWindow(m_skillNameBalloon);
			madeBalloon = true;
		}

		if (m_SkillBallonSkillId != m_skillUseInfo.m_skillId || madeBalloon == true) {
			m_SkillBallonSkillId = m_skillUseInfo.m_skillId;
			std::string skillName2;
			if (m_SkillBallonSkillId == 10000)
				skillName2 = "Capture Monster";
			else if (m_SkillBallonSkillId == 20000)
				skillName2 = "Please Marry Me";
			else if (m_SkillBallonSkillId == 20001)
				skillName2 = "Be my Baby";
			else if (g_session.IsAbracatabra(m_SkillBallonSkillId))
				skillName2 = "????";
			else
				skillName2 = SKILL_INFO::GetSkillNameBySkillId(m_SkillBallonSkillId);
			int width;

			m_skillNameBalloon->SetText(skillName2.c_str(), 35);

			width = m_skillNameBalloon->GetWidth();
			m_skillNameBalloon->Move((g_renderer->m_width - 640) / 2 + 320 - width / 2,
				g_renderer->m_height * 46 / 480);
			m_skillNameBalloon->SetFntColor(0x32ff32, 0);
		}

		if (m_skillMsgBalloon == NULL) {
			m_skillMsgBalloon = new UITransBalloonText;
			m_skillMsgBalloon->Create(8, 8);
			g_windowMgr.AddWindow(m_skillMsgBalloon);
			m_skillMsgBalloon->SetText(MsgStr(MSI_SELECTTARGET), 35);
			m_skillMsgBalloon->Move((g_renderer->m_width - 640) / 2 + 320 - m_skillMsgBalloon->GetWidth() / 2,
				g_renderer->m_height * 400 / 480);
			m_skillMsgBalloon->SetFntColor(0x32ff32, 0);
		}
	}

	if (m_playerGage) {
		m_playerGage->Move(GetWorld()->m_player->GetLastTlvertX() - 30,
			int(float(g_renderer->m_height * 12 / 480) * GetWorld()->m_player->GetLastPixelRatio() + GetWorld()->m_player->GetLastTlvertY()));
	}
}


bool	CGameMode :: AttackSkillOverWeightMessage()
{
	int weightPercent = g_session.GetWeight() * 100 / max(g_session.GetMaxWeight(), 1);
	if (weightPercent >= 90) {
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_NO_ATTACK_OVERWEIGHT), 0xff, 0, 0);
		return true;
	}
	return false;
}

void	CGameMode :: ProceedAttack(DWORD actorGid, int attackOnce)
{
	if (m_skillUseInfo.m_skillId != 10000 && AttackSkillOverWeightMessage())
		return;
	switch (m_skillUseInfo.m_skillUseType)
	{
	case	SUT_NOSKILL:
		m_world->m_player->SendMsg(NULL, AM_PROCEEDATTACK, actorGid, attackOnce, 0);
		break;
	case	SUT_TO_CHARACTER:
	case	SUT_TO_ALL:
		m_world->m_player->SendMsg(NULL, AM_PROCEEDSKILL, actorGid, m_skillUseInfo.m_skillId, m_skillUseInfo.m_attackRange);
		m_world->m_player->SendMsg(NULL, AM_SET_SKILL_USE_LEVEL, m_skillUseInfo.m_useLevel, 0, 0);
		break;
	}
}

void CGameMode::ProceedSkillToFriendList(DWORD actorGid)
{
	if (m_isOnQuest || m_isPlayerDead || g_windowMgr.GetExchangeWnd() || g_windowMgr.GetItemStoreWnd() ||
		g_windowMgr.GetMailListWnd() || g_windowMgr.GetMailViewWnd() || g_windowMgr.GetAuctionWnd())
		return;

	if (m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL) {
		bool isPlayer = actorGid == g_session.GetAid();
		bool isPicking = (isPlayer && m_skillUseInfo.m_skillUseType == SUT_TO_ALL) || !isPlayer;

		if (isPicking)
			ProceedAttack(actorGid, ATTM_REPEAT_TILL_BUTTONUP);
	}
}


void	CGameMode :: ProcessPcPick(int &processType, CActorPickInfo *actorInfo, CGameActor *pc)
{
	BOOL skillForCharacterIsHaiging;
	if (m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL)
		skillForCharacterIsHaiging = TRUE;
	else
		skillForCharacterIsHaiging = FALSE;
	BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
	BOOL attackCanBeApplied = shiftIsPressed;
	BOOL isGoodSkill = FALSE;

	if (skillForCharacterIsHaiging)
	{
		isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
	}

	if (isGoodSkill)
	{
		attackCanBeApplied = !shiftIsPressed;
	}

	if (actorInfo)
	{
		if (g_session.IsEnableAllyParty())
		{
			if (!g_session.IsMyParty(pc->GetGID()))
				attackCanBeApplied = true;
		}
		else if (g_session.IsEnableAllyGuild())
		{
			if (!g_session.IsMyGuild(pc->GetGdid()))
				attackCanBeApplied = true;
		}
		if (actorInfo->m_isPkState || m_world->IsPKZone() || m_world->IsEventPVPMode())
		{
			if (!g_session.IsMyParty(pc->GetGID()))
			{
				attackCanBeApplied = true;
			}

		}
		else if (m_world->IsSiegeMode())
		{
			if (!g_session.IsMyGuild(pc->GetGdid()))
				attackCanBeApplied = true;
		}
	}

	if (g_session.m_isNoShift == TRUE)
		attackCanBeApplied = true;

	if (attackCanBeApplied && pc->m_stateId != ST_DEAD)
	{
		if (skillForCharacterIsHaiging != FALSE)
			SetCursorAction(CURSOR_TARGET2);
		else
		{
			SetCursorAction(CURSOR_ATTACK);
		}
		processType = 1;
	}
	else
	{
		if (skillForCharacterIsHaiging != FALSE && pc->m_stateId != ST_DEAD)
			SetCursorAction(CURSOR_TARGET);
		else
			SetCursorAction(CURSOR_ARROW);
	}

#if defined (_HOMUN)
	if ( ( attackCanBeApplied || m_world->IsSiegeMode() ) && pc->m_stateId != ST_DEAD && skillForCharacterIsHaiging == FALSE )
	{
		if ( g_session.m_homunAlive && g_mouse.GetRBtn() == BTN_DOWN && CheckKeyPressed(VK_MENU) )
		{
			if (CheckKeyPressed(VK_SHIFT))
			{
				CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;
				if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() ) {
					MSG2AI	msg;
					msg.command = ATTACK_OBJECT_CMD;
					msg.targetID = pc->GetGID();
					actor->SetResMsg2AI (msg, MT_HOMUN);
				}
			}
			else if ( g_session.m_attackHomunEnemy && ( g_session.m_homunEnemyGID == pc->GetGID() ) )
			{
				CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;

				if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
				{
					MSG2AI msg;
					msg.command = ATTACK_OBJECT_CMD ;
					msg.targetID = g_session.m_homunEnemyGID ;
					actor->SetMsg2AI (msg, MT_HOMUN) ;
				}

				g_session.m_attackHomunEnemy = false ;
			}
			else
			{
				SetCursorAction(CURSOR_TARGET2);
				g_session.m_homunEnemyGID = pc->GetGID();
				g_session.m_homunEnemyX = 0 ;
				g_session.m_homunEnemyY = 0 ;
				g_session.m_moveHomunEnemy = false ;
				g_session.m_attackHomunEnemy = true ;
			}

			g_session.m_selectHomunEnemy = true ;
		}
	}


	if ( g_session.m_homunAlive && g_mouse.GetLBtn()==BTN_DOWN )
	{
		if ( m_skillUseInfo.m_skillId >= SKID_HOMUN_BEGIN && m_skillUseInfo.m_skillId < SKID_HOMUN_LAST )
		{
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;

			if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
			{
				MSG2AI	msg;
				msg.command = SKILL_OBJECT_CMD ;
				msg.skillLevel	= m_skillUseInfo.m_useLevel ;
				msg.skillID		  = m_skillUseInfo.m_skillId ;
				msg.targetID   = pc->GetGID();
				actor->SetMsg2AI ( msg, MT_HOMUN ) ;
			}

			return ;
		}
	}
#endif

#if defined (_MER) || defined (_MER_R2)
	if ( ( attackCanBeApplied || m_world->IsSiegeMode() ) && pc->m_stateId != ST_DEAD && skillForCharacterIsHaiging == FALSE )
	{
		if ( g_session.m_merAlive && g_mouse.GetLBtn() == BTN_DOWN && CheckKeyPressed(VK_MENU) )
		{
			if (CheckKeyPressed(VK_SHIFT))
			{
				CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;
				if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
				{
					MSG2AI	msg;
					msg.command = ATTACK_OBJECT_CMD;
					msg.targetID = pc->GetGID();
					actor->SetResMsg2AI (msg, MT_MER);
				}
			}
			else if ( g_session.m_attackMerEnemy && ( g_session.m_merEnemyGID == pc->GetGID() ) )
			{
				CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

				if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
				{
					MSG2AI msg;
					msg.command = ATTACK_OBJECT_CMD ;
					msg.targetID = g_session.m_merEnemyGID ;
					actor->SetMsg2AI (msg, MT_MER) ;
				}

				g_session.m_attackMerEnemy = false ;
			}
			else
			{
				SetCursorAction(CURSOR_TARGET2);
				g_session.m_merEnemyGID = pc->GetGID();
				g_session.m_merEnemyX = 0 ;
				g_session.m_merEnemyY = 0 ;
				g_session.m_moveMerEnemy = false ;
				g_session.m_attackMerEnemy = true ;
			}

			g_session.m_selectMerEnemy = true ;
		}
	}


	if ( attackCanBeApplied )
	{
		if ( g_session.m_merAlive && g_mouse.GetLBtn()==BTN_DOWN && m_skillUseInfo.m_skillId >= SKID_MERCENARY_BEGIN && m_skillUseInfo.m_skillId < SKID_MERCENARY_LAST )
		{
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

			if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
			{
				MSG2AI	msg;
				msg.command = SKILL_OBJECT_CMD ;
				msg.skillLevel	= m_skillUseInfo.m_useLevel ;
				msg.skillID		  = m_skillUseInfo.m_skillId ;
				msg.targetID   = pc->GetGID();
				actor->SetMsg2AI ( msg, MT_MER ) ;
			}

			return ;
		}
	}
#endif

	if (g_mouse.GetLBtn()==BTN_DOWN && attackCanBeApplied && !CheckKeyPressed(VK_MENU))
	{
		SendMsg(MM_SETLOCKONMOUSE, 1);
		m_lastLockOnMonGid = pc->GetGID();
		m_isAutoMoveClickOn = 0;

		{
			if ( !g_session.IsSiegeMode() && (CheckKeyPressed(VK_CONTROL) || g_session.m_isNoCtrl == TRUE ) )
			{
				ProceedAttack(pc->GetGID(), ATTM_REPEAT);
				m_isCtrlLock = CTRL_LOCK_TRUE;
			}
			else
			{
				ProceedAttack(pc->GetGID(), ATTM_REPEAT_TILL_BUTTONUP);
				m_isCtrlLock = CTRL_LOCK_FALSE;
			}
		}
	}

	if ( g_mouse.GetRBtn()==BTN_DOWN  && shiftIsPressed )
	{
		if ( AttackSkillOverWeightMessage() )
			return;

		SendMsg( MM_SETLOCKONMOUSE, 1 );
		m_lastLockOnPcGid = pc->GetGID();
		m_isAutoMoveClickOn = FALSE;
	}

	if (m_oldMouseX != g_mouse.GetXPos() || m_oldMouseY != g_mouse.GetYPos())
		m_lastPcGid = 0;

	MakeNameBalloon(m_lastPcGid);
}

void	CGameMode :: ProcessMonsterPick(int &processType, CActorPickInfo *actorInfo, CGameActor *monster)
{
	BOOL skillForCharacterIsHaiging
		= m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL;
	BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
	BOOL attackCanBeApplied = !shiftIsPressed;
	BOOL isGoodSkill = FALSE;
	if (skillForCharacterIsHaiging)
	{
		isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
	}
	if (isGoodSkill)
	{
		attackCanBeApplied = shiftIsPressed;
	}

	if (skillForCharacterIsHaiging)
		SetCursorAction(CURSOR_TARGET2);
	else if (monster->GetHeadType() == HEADTYPE_LAST)
		SetCursorAction(CURSOR_ARROW);
	else if (g_session.IsEnableAllyGuild())
	{
		if (g_session.IsMyGuild(monster->GetGdid()))
		{
			attackCanBeApplied = false;
			SetCursorAction(CURSOR_ARROW);
		}
	}
	else if (m_world->IsSiegeMode())
	{
		if (g_session.IsMyGuild(monster->GetGdid()))
		{
			attackCanBeApplied = false;
			SetCursorAction(CURSOR_ARROW);
		}
	}
	else
	{
		SetCursorAction(CURSOR_ATTACK);
	}
	processType = 1;

	if (g_session.m_isNoShift == TRUE)
		attackCanBeApplied = true;

#if defined (_HOMUN)
	if ( g_session.m_homunAlive && g_mouse.GetRBtn() == BTN_DOWN && CheckKeyPressed(VK_MENU) )
	{
		if (CheckKeyPressed(VK_SHIFT)) {
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;
			if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() ) {
				MSG2AI	msg;
				msg.command = ATTACK_OBJECT_CMD;
				msg.targetID = monster->GetGID();
				actor->SetResMsg2AI (msg, MT_HOMUN);
			}
		}
		else if ( g_session.m_attackHomunEnemy && ( g_session.m_homunEnemyGID == monster->GetGID() ) )
		{
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;

			if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
			{
				MSG2AI msg;
				msg.command = ATTACK_OBJECT_CMD ;
				msg.targetID = g_session.m_homunEnemyGID ;
				actor->SetMsg2AI (msg, MT_HOMUN) ;
			}

			g_session.m_attackHomunEnemy = false ;
		}
		else
		{
			SetCursorAction(CURSOR_TARGET2);
			g_session.m_homunEnemyGID = monster->GetGID();
			g_session.m_homunEnemyX = 0 ;
			g_session.m_homunEnemyY = 0 ;
			g_session.m_moveHomunEnemy = false ;
			g_session.m_attackHomunEnemy = true ;
		}

		g_session.m_selectHomunEnemy = true ;
	}


	if ( g_session.m_homunAlive && g_mouse.GetLBtn()==BTN_DOWN )
	{
		if ( m_skillUseInfo.m_skillId >= SKID_HOMUN_BEGIN && m_skillUseInfo.m_skillId < SKID_HOMUN_LAST )
		{
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;

			if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
			{
				MSG2AI	msg;
				msg.command = SKILL_OBJECT_CMD ;
				msg.skillLevel	= m_skillUseInfo.m_useLevel ;
				msg.skillID		  = m_skillUseInfo.m_skillId ;
				msg.targetID   = monster->GetGID();
				actor->SetMsg2AI ( msg, MT_HOMUN ) ;
			}

			return ;
		}
	}
#endif

#if defined (_MER) || defined (_MER_R2)
	if ( g_session.m_merAlive && g_mouse.GetLBtn() == BTN_DOWN && CheckKeyPressed(VK_MENU) )
	{
		if (CheckKeyPressed(VK_SHIFT)) {
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;
			if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() ) {
				MSG2AI	msg;
				msg.command = ATTACK_OBJECT_CMD;
				msg.targetID = monster->GetGID();
				actor->SetResMsg2AI (msg, MT_MER);
			}
		}
		else if ( g_session.m_attackMerEnemy && ( g_session.m_merEnemyGID == monster->GetGID() ) )
		{
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

			if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
			{
				MSG2AI msg;
				msg.command = ATTACK_OBJECT_CMD ;
				msg.targetID = g_session.m_merEnemyGID ;
				actor->SetMsg2AI (msg, MT_MER) ;
			}

			g_session.m_attackMerEnemy = false ;
		}
		else
		{
			SetCursorAction(CURSOR_TARGET2);
			g_session.m_merEnemyGID = monster->GetGID();
			g_session.m_merEnemyX = 0 ;
			g_session.m_merEnemyY = 0 ;
			g_session.m_moveMerEnemy = false ;
			g_session.m_attackMerEnemy = true ;
		}

		g_session.m_selectMerEnemy = true ;
	}

	if ( attackCanBeApplied && g_session.m_merAlive && g_mouse.GetLBtn()==BTN_DOWN )
	{
		if ( m_skillUseInfo.m_skillId >= SKID_MERCENARY_BEGIN && m_skillUseInfo.m_skillId < SKID_MERCENARY_LAST )
		{
			CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

			if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
			{
				MSG2AI	msg;
				msg.command = SKILL_OBJECT_CMD ;
				msg.skillLevel	= m_skillUseInfo.m_useLevel ;
				msg.skillID		  = m_skillUseInfo.m_skillId ;
				msg.targetID   = monster->GetGID();
				actor->SetMsg2AI ( msg, MT_MER ) ;
			}

			return ;
		}
	}
#endif

	if ( g_mouse.GetLBtn()==BTN_DOWN && attackCanBeApplied && !CheckKeyPressed(VK_MENU))
	{
		SendMsg(MM_SETLOCKONMOUSE, 1);
		m_lastLockOnMonGid = m_lastMonGid;
		m_isAutoMoveClickOn = 0;

		if ( !g_session.IsSiegeMode() && (CheckKeyPressed(VK_CONTROL) || g_session.m_isNoCtrl == TRUE ) )
		{
			ProceedAttack(m_lastMonGid, ATTM_REPEAT);
			m_isCtrlLock = CTRL_LOCK_TRUE;
		}
		else
		{
			ProceedAttack(m_lastMonGid, ATTM_REPEAT_TILL_BUTTONUP);
			m_isCtrlLock = CTRL_LOCK_FALSE;
		}
	}
	else if (!m_isCtrlLock && g_mouse.GetLBtn() == BTN_UP)
	{
		m_lastMonGid = 0;
	}

	if (m_oldMouseX != g_mouse.GetXPos() || m_oldMouseY != g_mouse.GetYPos()) {
		if (!m_isCtrlLock && g_mouse.GetLBtn() == BTN_NONE)
			m_lastMonGid = 0;
	}
	MakeNameBalloon(m_lastMonGid);
	if (monster->m_stateId == ST_DEAD) {
		m_lastMonGid = 0;
	}
}

int		CGameMode :: ProcessActorPick(int processType, CActorPickInfo * actorInfo)
{
	if (m_skillUseInfo.m_skillUseType == SUT_TO_GROUND)
	{




		return processType;
	}

	if (m_isMouseLockOn)
	{
		SetCursorAction(CURSOR_ATTACK);

		if (m_isCtrlLock && g_mouse.GetLBtn() == BTN_DOWN)
		{
			SendMsg(MM_SETLOCKONMOUSE, 0);
		}
		else if (m_isCtrlLock && g_mouse.GetLBtn() != BTN_DOWN)
		{
			SetCursorAction(CURSOR_ARROW);
			if (actorInfo == NULL)
			{





				return processType;
			}

			else processType = 0;
		}
		else if (g_mouse.GetLBtn() == BTN_PRESSED)
		{
			processType = 1;






			return processType;
		}
		else if (!actorInfo && !m_lastMonGid && !m_lastPcGid)
		{
			m_isMouseLockOn = FALSE;
		}
	}
	else if (!actorInfo)
	{
		if (!m_isCtrlLock && g_mouse.GetLBtn() != BTN_DOWN)
		{
			if (m_oldMouseX != g_mouse.GetXPos() || m_oldMouseY != g_mouse.GetYPos())
			{
				m_lastPcGid = 0;
				m_lastMonGid = 0;
			}
		}

		return processType;
	}

	if (m_lastMonGid)
	{
		if (actorInfo)
		{
			if (actorInfo->m_classType == CT_PET)
			{
				SetCursorAction(CURSOR_ARROW);
				return 0;
			}
#if defined (_HOMUN)
			else if (actorInfo->m_classType == CT_MER)
			{
				SetCursorAction(CURSOR_ARROW);

				MakeNameBalloon(actorInfo->m_gid);
				processType = ProcessObjClick(actorInfo, processType);






				return processType ;
			}
#endif
		}

		CGameActor * monster = m_world->GetGameActorByAID(m_lastMonGid);
		if ( monster )
		{
			ProcessMonsterPick(processType, actorInfo, monster);




		}
		else if(monster == 0)
		{
			m_lastMonGid = 0;
		}
	}
	else if (m_lastPcGid)
	{
		CGameActor * pc = m_world->GetGameActorByAID(m_lastPcGid);

		if (pc)
		{
			if (m_lastPcGid == 1)
			{
				if (g_mouse.GetLBtn() == BTN_DBLCLK)
				{
					if (g_session.HaveThisSkill(0x1c) >= 1)
						g_NextSkill = 0x1c;
				}
				else if (g_mouse.GetLBtn() == BTN_DOWN)
				{
					g_SpellSeek = 1;
					g_SpellSeekTime = timeGetTime();
				}

				if (g_mouse.GetRBtn() == BTN_DBLCLK)
				{
					if (g_session.HaveThisSkill(0x35) >= 1)
						g_NextSkill = 0x35;
				}
				else if (g_mouse.GetRBtn() == BTN_DOWN)
				{
					g_SpellSeek = 2;
					g_SpellSeekTime = timeGetTime();
				}
			}

			if (m_isCtrlLock && pc->m_stateId == ST_DEAD)
				m_isCtrlLock = FALSE;

			ProcessPcPick(processType, actorInfo, pc);






		} else
			m_lastPcGid = 0;
	}
	else
	{
		if (actorInfo && processType==0)
		{
			MakeNameBalloon(actorInfo->m_gid);
			processType = ProcessObjClick(actorInfo, processType);
		}
	}

	return processType;
}

int		CGameMode :: ProcessObjClick(CActorPickInfo * actorInfo, int processType)
{
	if (actorInfo->m_gid != g_session.GetAid())
	{
		switch (actorInfo->m_classType)
		{
		case	CT_CGAMEACTOR:
			switch (actorInfo->GetJob())
			{
			case	JT_WARPNPC:
				SetCursorAction(CURSOR_ENTER);
				processType = 2;
				break;
			default:
				processType = 1;

				if (g_session.IsThisPC(actorInfo->GetJob()))
				{
					BOOL skillForCharacterIsHaiging
						= m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL;
					BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
					BOOL attackCanBeApplied = shiftIsPressed;
					BOOL isGoodSkill = FALSE;
					if (skillForCharacterIsHaiging) {
						isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
					}
					if (isGoodSkill) {
						attackCanBeApplied = !shiftIsPressed;
					}

					CPc * pc = (CPc*)GetWorld()->GetGameActorByAID(actorInfo->m_gid);

					if (actorInfo) {
						if (g_session.IsEnableAllyParty()) {
							if (!g_session.IsMyParty(actorInfo->m_gid))
								attackCanBeApplied = true;
						} else if (g_session.IsEnableAllyGuild()) {
							if (pc) {
								if (!g_session.IsMyGuild(pc->GetGdid()))
									attackCanBeApplied = true;
							}
						}
						if (actorInfo->m_isPkState || m_world->IsPKZone() || m_world->IsEventPVPMode()) {
							if (!g_session.IsMyParty(actorInfo->m_gid))
							{
								attackCanBeApplied = true;
							}
						} else if (m_world->IsSiegeMode()) {
							if (pc) {
								if (!g_session.IsMyGuild(pc->GetGdid()))
									attackCanBeApplied = true;
							}
						}
					}

					if (g_session.m_isNoShift == TRUE)
						attackCanBeApplied = true;

					int	stateId = ST_DEAD;
					if (pc) {
						stateId = pc->m_stateId;
					}
					if (attackCanBeApplied && stateId != ST_DEAD) {
						if (skillForCharacterIsHaiging)
							SetCursorAction(CURSOR_TARGET2);
						else
						{
							CURSOR_ACTION curAction = CURSOR_ATTACK ;
							SetCursorAction(curAction);
						}
						processType = 1;
						m_lastPcGid = actorInfo->m_gid;
					} else {
						if (skillForCharacterIsHaiging && stateId != ST_DEAD)
							SetCursorAction(CURSOR_TARGET);
						else
							SetCursorAction(CURSOR_ARROW);
						processType = 0;
						if (g_mouse.GetLBtn() == BTN_NONE && !m_isMouseLockOn && !m_lastMonGid)
							m_lastPcGid = actorInfo->m_gid;
					}
				} else if (actorInfo->GetJob() > JT_WARPNPC && actorInfo->GetJob() < JT_MON_BEGIN) {
					SetCursorAction(CURSOR_TALK);
					if (g_mouse.GetLBtn() == BTN_PRESSED)
						processType = 2;
				} else if (actorInfo->GetJob() > JT_MON_BEGIN && actorInfo->GetJob() < JT_2004_JOB_BEGIN) {
					BOOL skillForCharacterIsHaiging
						= m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL;
					BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
					BOOL attackCanBeApplied = !shiftIsPressed;
					BOOL isGoodSkill = FALSE;
					if (skillForCharacterIsHaiging) {
						isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
					}
					if (isGoodSkill) {
						attackCanBeApplied = shiftIsPressed;
					}

					if (g_session.m_isNoShift == TRUE)
						attackCanBeApplied = true;

					CGameActor * npc = GetWorld()->GetGameActorByAID(actorInfo->GetGID());
					if (g_session.IsEnableAllyGuild()) {
						if (npc) {
							if (g_session.IsMyGuild(npc->GetGdid()))
								attackCanBeApplied = false;
						}
					}
					if (m_world->IsSiegeMode()) {
						if (npc) {
							if (g_session.IsMyGuild(npc->GetGdid()))
								attackCanBeApplied = false;
						}
					}

					if (m_isAutoMoveClickOn) {
						if (attackCanBeApplied) {
							if (skillForCharacterIsHaiging)
								SetCursorAction(CURSOR_TARGET2);
							else if (npc && npc->GetHeadType() == HEADTYPE_LAST)
								SetCursorAction(CURSOR_ARROW);
							else
								SetCursorAction(CURSOR_ATTACK);
						} else {
							if (skillForCharacterIsHaiging)
								SetCursorAction(CURSOR_TARGET);
							else
								SetCursorAction(CURSOR_ARROW);
						}
					}
					if (g_mouse.GetLBtn() == BTN_NONE && !m_isMouseLockOn)
						m_lastMonGid = actorInfo->m_gid;
					if (g_mouse.GetLBtn() == BTN_PRESSED || g_mouse.GetRBtn() == BTN_PRESSED)
						processType = 2;
				}
				break;
			}
			break;
		case	CT_CSKILL:
			{
				processType = 1;
				if (g_session.IsSiegeMode()) {
					switch (actorInfo->GetJob()) {
		case	JT_ICEWALL:
		case	JT_TRAP_BLASTMINE:
		case	JT_TRAP_SKIDTRAP:
		case	JT_TRAP_ANKLESNARE:
		case	JT_TRAP_LANDMINE:
		case	JT_TRAP_TALKIEBOX:
		case	JT_TRAP_SANDMAN:
		case	JT_TRAP_FLASHER:
		case	JT_TRAP_FREEZINGTRAP:
		case	JT_TRAP_SHOCKWAVE:
		case	JT_TRAP_CLAYMORETRAP:

		case	JT_LIGHTNING_S:
		case	JT_BLIND_S:
		case	JT_POISON_S:
		case	JT_FREEZING_S:
		case	JT_FLARE_S:
			if (m_skillUseInfo.m_skillUseType != SUT_TO_SKILL)
				SetCursorAction(CURSOR_ATTACK);
			else
				SetCursorAction(CURSOR_TARGET);
			break;
		default:
			if (m_skillUseInfo.m_skillUseType != SUT_TO_SKILL)
				SetCursorAction(CURSOR_ARROW);
			else
				SetCursorAction(CURSOR_TARGET);

			break;
					}
				} else {
					switch (actorInfo->GetJob()) {
		case	JT_ICEWALL:
		case	JT_TRAP_BLASTMINE:
		case	JT_TRAP_CLAYMORETRAP:
			if (m_skillUseInfo.m_skillUseType != SUT_TO_SKILL)
				SetCursorAction(CURSOR_ATTACK);
			else
				SetCursorAction(CURSOR_TARGET);
			break;
		default:
			if (m_skillUseInfo.m_skillUseType != SUT_TO_SKILL)
				SetCursorAction(CURSOR_ARROW);
			else
				SetCursorAction(CURSOR_TARGET);

			break;
					}
				}
				if (g_mouse.GetLBtn() == BTN_PRESSED)
					processType = 2;
			}
			break;
		case	CT_CITEM:
			{
				processType = 1;
				SetCursorAction(CURSOR_GRIP);
				if (g_mouse.GetLBtn() == BTN_PRESSED)
					processType = 2;
			}
			break;
		case	CT_PET:
			{
				processType = 0;
				SetCursorAction(CURSOR_ARROW);
			}
			break;

		case	CT_MER:
#if defined (_HOMUN)
			if ( g_session.IsHomun( actorInfo->GetJob() ) )
			{
				processType = 0 ;
				SetCursorAction(CURSOR_ARROW);


				if ( ( m_skillUseInfo.m_skillId == SKID_AM_POTIONPITCHER ) || ( ( m_world->IsPKZone() || m_world->IsSiegeMode() ) && g_session.m_homunGID != actorInfo->m_gid ) )
				{
					processType = 1;

					CGameActor*	theHomun = (CGameActor*) GetWorld()->GetGameActorByAID (actorInfo->m_gid);

					int	stateId = ST_DEAD ;

					if (theHomun)
						stateId = theHomun->m_stateId ;

					if ( stateId != ST_DEAD )
					{
						if ( (m_world->IsPKZone() || m_world->IsSiegeMode() ) && g_session.m_homunGID != actorInfo->m_gid )
							SetCursorAction(CURSOR_ATTACK) ;
						else
							SetCursorAction(CURSOR_TARGET2) ;

						m_lastMonGid = actorInfo->m_gid;
					}
				}
			}
#endif
#if defined (_MER) || defined (_MER_R2)
			else
			{
				processType = 1 ;

				BOOL skillForCharacterIsHaiging
					= m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL;
				BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
				BOOL attackCanBeApplied = shiftIsPressed;
				BOOL isGoodSkill = FALSE;
				if (skillForCharacterIsHaiging)
				{
					isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
				}
				if (isGoodSkill)
				{
					attackCanBeApplied = !shiftIsPressed;
				}

				CGameActor*	theMer = (CGameActor*) GetWorld()->GetGameActorByAID (actorInfo->m_gid);

				if ( (m_world->IsPKZone() || m_world->IsSiegeMode() ) && g_session.m_merGID != actorInfo->m_gid )
					attackCanBeApplied = true ;

				int	stateId = ST_DEAD ;

				if ( theMer )
					stateId = theMer->m_stateId;

				if (attackCanBeApplied && stateId != ST_DEAD)
				{
					if (skillForCharacterIsHaiging)
						SetCursorAction(CURSOR_TARGET2);
					else
						SetCursorAction(CURSOR_ATTACK);
					processType = 1;
					m_lastPcGid = actorInfo->m_gid;
				}
				else
				{
					if (skillForCharacterIsHaiging && stateId != ST_DEAD)
						SetCursorAction(CURSOR_TARGET);
					else
						SetCursorAction(CURSOR_ARROW);
					processType = 0;
				}
			}
#endif
			break ;
		}
	}

	if (g_mouse.GetLBtn()!=BTN_DOWN && g_mouse.GetRBtn()!=BTN_DOWN)
		return processType;

	switch (actorInfo->m_classType)
	{
	case	CT_CGAMEACTOR:
		switch (actorInfo->GetJob())
		{
		case	JT_WARPNPC:
			return processType;
			break;
		default:
			{



















































				if (actorInfo->GetJob() > JT_WARPNPC && actorInfo->GetJob() < JT_MON_BEGIN) {


				if (m_world->m_player->m_stateId == ST_STAND || m_world->m_player->m_stateId == 6)
					{

						int dx, dy;
						CGameActor * theActor = GetWorld()->GetGameActorByAID(actorInfo->GetGID());
						GetServerCoor(theActor->m_pos.x, theActor->m_pos.z, dx, dy);
						m_world->m_player->SendMsg(NULL, AM_REQ_CHANGE_DIR, dx, dy, 0);

						PACKET_CZ_CONTACTNPC packet;
						packet.PacketType = HEADER_CZ_CONTACTNPC;
						packet.NAID = actorInfo->m_gid;
						packet.type = NPCCONTACTTYPE_TOUCH;
						CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(packet.PacketType), (const char *)(&packet));














































					}
					return processType;
				}
				else if (actorInfo->GetJob() > JT_MON_BEGIN && actorInfo->GetJob() < JT_2004_JOB_BEGIN)
				{
					BOOL skillForCharacterIsHaiging
						= m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL;
					BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
					BOOL attackCanBeApplied = !shiftIsPressed;
					BOOL isGoodSkill = FALSE;
					if (skillForCharacterIsHaiging)
					{
						isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
					}
					if (isGoodSkill)
					{
						attackCanBeApplied = shiftIsPressed;
					}

					if (g_session.m_isNoShift == TRUE)
						attackCanBeApplied = true;

#if defined (_HOMUN)
					if ( g_session.m_homunAlive && g_mouse.GetRBtn() == BTN_DOWN && CheckKeyPressed(VK_MENU) )
					{
							if (CheckKeyPressed(VK_SHIFT)) {
								CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;
								if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() ) {
									MSG2AI	msg;
									msg.command = ATTACK_OBJECT_CMD;
									msg.targetID = actorInfo->m_gid;
									actor->SetResMsg2AI (msg, MT_HOMUN);
								}
							}
							else if ( g_session.m_attackHomunEnemy && ( g_session.m_homunEnemyGID == actorInfo->m_gid ) )
							{
								CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;

								if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
								{
									MSG2AI msg;
									msg.command = ATTACK_OBJECT_CMD ;
									msg.targetID = g_session.m_homunEnemyGID ;
									actor->SetMsg2AI (msg, MT_HOMUN) ;
								}

								g_session.m_attackHomunEnemy = false ;
							}
							else
							{
								SetCursorAction(CURSOR_TARGET2);
								g_session.m_homunEnemyGID = actorInfo->m_gid ;
								g_session.m_homunEnemyX = 0 ;
								g_session.m_homunEnemyY = 0 ;
								g_session.m_moveHomunEnemy = false ;
								g_session.m_attackHomunEnemy = true ;
							}

							g_session.m_selectHomunEnemy = true ;
					}

					if ( g_session.m_homunAlive && g_mouse.GetLBtn()==BTN_DOWN )
					{
						if ( m_skillUseInfo.m_skillId >= SKID_HOMUN_BEGIN && m_skillUseInfo.m_skillId < SKID_HOMUN_LAST )
						{
							CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;

							if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
							{
								MSG2AI	msg;
								msg.command = SKILL_OBJECT_CMD ;
								msg.skillLevel	= m_skillUseInfo.m_useLevel ;
								msg.skillID		  = m_skillUseInfo.m_skillId ;
								msg.targetID   = actorInfo->m_gid ;
								actor->SetMsg2AI ( msg, MT_HOMUN ) ;
							}

							return processType ;
						}
					}
#endif

#if defined (_MER) || defined (_MER_R2)
					if ( g_session.m_merAlive && g_mouse.GetLBtn() == BTN_DOWN && CheckKeyPressed(VK_MENU) )
					{
						if (CheckKeyPressed(VK_SHIFT))
						{
							CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;
							if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
							{
								MSG2AI	msg;
								msg.command = ATTACK_OBJECT_CMD;
								msg.targetID = actorInfo->m_gid;
								actor->SetResMsg2AI (msg, MT_MER);
							}
						}
						else if ( g_session.m_attackMerEnemy && ( g_session.m_merEnemyGID == actorInfo->m_gid ) )
						{
							CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

							if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
							{
								MSG2AI msg;
								msg.command = ATTACK_OBJECT_CMD ;
								msg.targetID = g_session.m_merEnemyGID ;
								actor->SetMsg2AI (msg, MT_MER) ;
							}

							g_session.m_attackMerEnemy = false ;
						}
						else
						{
							SetCursorAction(CURSOR_TARGET2);
							g_session.m_merEnemyGID = actorInfo->m_gid ;
							g_session.m_merEnemyX = 0 ;
							g_session.m_merEnemyY = 0 ;
							g_session.m_moveMerEnemy = false ;
							g_session.m_attackMerEnemy = true ;
						}

						g_session.m_selectMerEnemy = true ;
					}

					if ( attackCanBeApplied && g_session.m_merAlive && g_mouse.GetLBtn()==BTN_DOWN )
					{
						if ( m_skillUseInfo.m_skillId >= SKID_MERCENARY_BEGIN && m_skillUseInfo.m_skillId < SKID_MERCENARY_LAST )
						{
							CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

							if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
							{
								MSG2AI	msg;
								msg.command = SKILL_OBJECT_CMD ;
								msg.skillLevel	= m_skillUseInfo.m_useLevel ;
								msg.skillID		  = m_skillUseInfo.m_skillId ;
								msg.targetID   = actorInfo->m_gid ;
								actor->SetMsg2AI ( msg, MT_MER ) ;
							}

							return processType ;
						}
					}
#endif

					if (g_mouse.GetLBtn()==BTN_DOWN && attackCanBeApplied && !CheckKeyPressed(VK_MENU) )
					{
						SendMsg(MM_SETLOCKONMOUSE, 1);
						m_lastLockOnMonGid = actorInfo->m_gid;
						m_lastMonGid = actorInfo->m_gid;
						m_isAutoMoveClickOn = 0;
						bool isAtt = true ;
						if (attackCanBeApplied && isAtt) {
							if ( !g_session.IsSiegeMode() && ( CheckKeyPressed(VK_CONTROL) || g_session.m_isNoCtrl == TRUE ) )
							{
									ProceedAttack(actorInfo->m_gid, ATTM_REPEAT);
								m_isCtrlLock = CTRL_LOCK_TRUE;

								} else {
									ProceedAttack(actorInfo->m_gid, ATTM_REPEAT_TILL_BUTTONUP);
									m_isCtrlLock = CTRL_LOCK_FALSE;
								}
						}
						}
						else if (!m_isCtrlLock && g_mouse.GetLBtn()==BTN_UP)
							m_lastMonGid = 0;

					return processType;


				}
				else if (g_session.IsThisPC(actorInfo->GetJob()))
				{
					BOOL skillForCharacterIsHaiging
						= m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL;
					BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
					BOOL attackCanBeApplied = shiftIsPressed;
					BOOL isGoodSkill = FALSE;
					if (skillForCharacterIsHaiging) {
						isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
					}
					if (isGoodSkill) {
						attackCanBeApplied = !shiftIsPressed;
					}

					if (actorInfo)
					{
						if (actorInfo->m_isPkState || m_world->IsPKZone())
							attackCanBeApplied = true;
					}


#if defined (_MER) || defined (_MER_R2)
					if ( attackCanBeApplied && g_session.m_merAlive && g_mouse.GetLBtn()==BTN_DOWN )
					{
						if ( m_skillUseInfo.m_skillId >= SKID_MERCENARY_BEGIN && m_skillUseInfo.m_skillId < SKID_MERCENARY_LAST )
						{
							CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

							if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
							{
								MSG2AI	msg;
								msg.command = SKILL_OBJECT_CMD ;
								msg.skillLevel	= m_skillUseInfo.m_useLevel ;
								msg.skillID		  = m_skillUseInfo.m_skillId ;
								msg.targetID   = g_session.GetAid();
								actor->SetMsg2AI ( msg, MT_MER ) ;
							}

							return processType ;
						}
					}
#endif

					bool isPlayer = actorInfo->m_gid == g_session.GetAid();
					bool isPicking = (isPlayer && m_skillUseInfo.m_skillUseType == SUT_TO_ALL) || !isPlayer;
					if (isPicking && attackCanBeApplied) {
						Trace("people kill");
						SendMsg(MM_SETLOCKONMOUSE, 1);
						m_lastLockOnMonGid = actorInfo->m_gid;
						m_lastMonGid = actorInfo->m_gid;
						m_isAutoMoveClickOn = 0;
						bool isAtt = true ;
						if ( isAtt )
						{
							if ( !g_session.IsSiegeMode() && ( CheckKeyPressed(VK_CONTROL) || g_session.m_isNoCtrl == TRUE ) )
							{




									ProceedAttack(actorInfo->m_gid, ATTM_REPEAT);
								m_isCtrlLock = CTRL_LOCK_TRUE;
							} else {
								ProceedAttack(actorInfo->m_gid, ATTM_REPEAT_TILL_BUTTONUP);
								m_isCtrlLock = CTRL_LOCK_FALSE;
							}
						}
					}

					return processType;
				}

			}
			break;
		}
		break;

	case	CT_CSKILL:
		{
			switch(actorInfo->GetJob())
			{
			case	JT_ICEWALL:
			case	JT_TRAP_BLASTMINE:
			case	JT_TRAP_CLAYMORETRAP:
				{
#ifdef SECONDJOB_SKILL2
					if ( m_skillUseInfo.m_skillId == SKID_RG_STRIPWEAPON )
						break;
					if ( m_skillUseInfo.m_skillId == SKID_RG_STRIPSHIELD )
						break;
					if ( m_skillUseInfo.m_skillId == SKID_RG_STRIPARMOR )
						break;
					if ( m_skillUseInfo.m_skillId == SKID_RG_STRIPHELM )
						break;
#endif

					m_world->m_player->SendMsg(NULL, AM_PROCEEDTOUCHSKILL, actorInfo->m_gid, int(&m_skillUseInfo), 0);
				}
				break;
			default:
				if (m_skillUseInfo.m_skillUseType == SUT_TO_SKILL)
					m_world->m_player->SendMsg(NULL, AM_PROCEEDTOUCHSKILL, actorInfo->m_gid, int(&m_skillUseInfo), 0);
				break;
			}

			return processType;
		}
		break;
		case	CT_PET:
			{
				processType = 0;
				SetCursorAction(CURSOR_ARROW);
			}
			break;

		case	CT_CITEM:
		{
			if (g_mouse.GetLBtn()!=BTN_DOWN)
				return processType;

			if (m_skillUseInfo.m_skillUseType == SUT_NOSKILL)
				m_world->m_player->SendMsg(NULL, AM_PROCEEDPICKUPITEM, actorInfo->m_gid, 0, 0);
			return processType;
		}
		break;

	case	CT_MER:
		{
			SetCursorAction(CURSOR_ARROW);

#if defined (_HOMUN)
			if ( g_session.IsHomun( actorInfo->GetJob() ) )
			{

				if ( ( m_skillUseInfo.m_skillId == SKID_AM_POTIONPITCHER ) || ( (m_world->IsPKZone() || m_world->IsSiegeMode()) && g_session.m_homunGID != actorInfo->m_gid ) )
				{
					processType = 0;

					if ( (m_world->IsPKZone() || m_world->IsSiegeMode()) && g_session.m_homunGID != actorInfo->m_gid )
						SetCursorAction(CURSOR_ATTACK) ;

					ProceedAttack(actorInfo->m_gid, ATTM_REPEAT_TILL_BUTTONUP);
					m_isCtrlLock = CTRL_LOCK_FALSE;
				}
			}
#endif
#if defined (_MER) || defined (_MER_R2)
			else
			{
				processType = 0 ;

				BOOL skillForCharacterIsHaiging
					= m_skillUseInfo.m_skillUseType == SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType == SUT_TO_ALL;
				BOOL shiftIsPressed = CheckKeyPressed(VK_SHIFT);
				BOOL attackCanBeApplied = shiftIsPressed;
				BOOL isGoodSkill = FALSE;
				if (skillForCharacterIsHaiging) {
					isGoodSkill = SKILL_INFO::IsGoodSkillForActor(m_skillUseInfo.m_skillId);
				}
				if (isGoodSkill) {
					attackCanBeApplied = !shiftIsPressed;
				}

				if (actorInfo)
				{
						if (actorInfo->m_isPkState || m_world->IsPKZone() || m_world->IsSiegeMode())
							attackCanBeApplied = true;
				}

				bool isMyMer= actorInfo->m_gid == g_session.m_merGID ;
				bool isPicking = (isMyMer && m_skillUseInfo.m_skillUseType == SUT_TO_ALL) || !isMyMer;

				bool isAtt = true ;

				if (isPicking && attackCanBeApplied && isAtt)
					ProceedAttack(actorInfo->m_gid, ATTM_REPEAT_TILL_BUTTONUP);
			}
#endif
		}
		break;
	}
	return processType;
}

void	CGameMode :: ProcessLockOnMouse()
{
	CGameActor * pActor = NULL;
	if (m_lastLockOnPcGid) {
		pActor = GetWorld()->GetGameActorByAID(m_lastLockOnPcGid);
		if (NULL == pActor) return;
		m_nameDisplayed2 = FALSE;
		MakeAttackedActorNameBalloon(pActor);
	} else if (m_isMouseLockOn) {
		pActor = GetWorld()->GetGameActorByAID(m_lastLockOnMonGid);
		if (NULL == pActor) return;
		m_nameDisplayed2 = FALSE;
		MakeAttackedActorNameBalloon(pActor);
	} else {
		SendMsg(MM_HIDEMOUSEPOINTER);
		return;
	}

	if (pActor && (JT_WARPNPC != pActor->GetJob())) {
		SendMsg(MM_SHOWMOUSEPOINTER, pActor->m_pos.x, pActor->m_pos.z);
	} else {
		SendMsg(MM_HIDEMOUSEPOINTER);
	}
}

void	CGameMode :: InvalidatePlayerGage()
{
	if (m_playerGage) m_playerGage->Invalidate();
	UIMessengerGroupWnd *messengerGroupWnd = g_windowMgr.GetMessengerGroupWnd();
	if (messengerGroupWnd) {
		messengerGroupWnd->SendMsg(NULL, UM_COMMAND, 0x22, g_session.GetAid(), 0, 0);
	}
}

void CGameMode::HideCheck()
{
	if (!m_world || !m_world->m_player || !g_renderer)
		return;

	int rayScrX = m_world->m_player->GetLastTlvertX();
	int rayScrY = m_world->m_player->GetLastTlvertY() - 23.0f * g_renderer->m_screenYFactor;
	CRayPicker picker;
	ray3d ray;

	g_renderer->BuildRay(rayScrX, rayScrY, m_view->m_from, m_view->m_invViewMatrix, &ray);

	lineSegment3d lineSegRay;
	lineSegRay.dir = ray.dir;
	lineSegRay.org = ray.org;
	lineSegRay.limit = m_view->GetCurDistance() + m_world->m_player->m_pos.y - 16;
	picker.Build(ray, m_world->GetRootNode());

	std::list<SceneGraphNode *>::iterator ni = picker.m_cubeletList.begin();
	while (ni != picker.m_cubeletList.end()) {
		std::vector<C3dActor *> &modelList = (*ni)->m_actorList;
		std::vector<C3dActor *>::iterator mIt = modelList.begin();
		while (mIt != modelList.end()) {
			(*mIt)->CullByOBB(lineSegRay);
			++mIt;
		}
		++ni;
	}

	if (m_isCheckGndAlpha) {
		float nearest = infinity;
		ni = picker.m_cubeletList.begin();
		while (ni != picker.m_cubeletList.end()) {
			SceneGraphNode *node = *ni;
			C3dGround *ground = node->m_ground;
			for (int i = node->m_groundArea.top; i <= node->m_groundArea.bottom; ++i) {
				for (int j = node->m_groundArea.left; j <= node->m_groundArea.right; ++j) {
					ground->CheckAlpha(ray, j, i, lineSegRay.limit);
				}
			}
			++ni;
		}
		Trace("nearest==%f", nearest);
	}
}





bool	CGameMode :: GetGroundAttrPos(int &dx, int &dy)
{
	CRayPicker	picker;
	ray3d	ray;

	g_renderer->BuildRay(g_mouse.GetXPos(), g_mouse.GetYPos(), m_view->m_from, m_view->m_invViewMatrix, &ray);

	picker.Build(ray, m_world->GetRootNode());


	std::list <SceneGraphNode *>	:: iterator	ni = picker.m_cubeletList.begin();
	float	nearest = infinity;

	while (ni != picker.m_cubeletList.end()) {
		SceneGraphNode *	node = *ni;
		C3dAttr * attr = node->m_attr;
		float	dist;

		for (int i=node->m_attrArea.top; i<=node->m_attrArea.bottom; i++) {
			for (int j=node->m_attrArea.left; j<=node->m_attrArea.right; j++) {
				dist = attr->RayTest(ray, j, i);
				if (dist < nearest) {
					dx = j;
					dy = i;
					nearest = dist;
				}
			}
		}
		++ni;
	}
	return (nearest < infinity);
}

void	CGameMode :: ProcessMouseSnap(CActorPickInfo *	actorInfo, int processType)
{
	m_mouseSnapDiff.x = 0;
	m_mouseSnapDiff.y = 0;

	if (processType)
		return;


	if (m_skillUseInfo.m_skillUseType==SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType==SUT_TO_ALL)
		g_session.RefreshMouseSnapState(TRUE);
	else
		g_session.RefreshMouseSnapState(FALSE);

	if (actorInfo) {
		BOOL isSnap = FALSE;

		if (m_skillUseInfo.m_skillId == SKID_AM_POTIONPITCHER)
		{
			if ( g_session.IsMerOrHomun(actorInfo->GetJob()) && actorInfo->m_classType == CT_MER  )
				isSnap = TRUE;
		}
		else
		{
			if (g_session.IsMonsterSnap() && actorInfo->GetJob() > JT_MON_BEGIN && actorInfo->GetJob() < JT_MONSTER_LAST) {
				isSnap = TRUE;
			} else if (g_session.IsItemSnap() && actorInfo->m_classType == CT_CITEM) {
				isSnap = TRUE;
			}
		}

		if (isSnap) {
			vector2d pickActorCerterPos = actorInfo->GetCenterPos();
			m_mouseSnapDiff.x = pickActorCerterPos.x - g_mouse.GetXPos();
			m_mouseSnapDiff.y = pickActorCerterPos.y - g_mouse.GetYPos();
		}
	}

}void	CGameMode :: MakeMenu(CActorPickInfo *	actorInfo, int processType)
{
	if (g_mouse.GetLBtn() == BTN_PRESSED)  return;
	if (CheckKeyPressed(VK_SHIFT))	return;

	char msg[128];
	unsigned char isGravityAid = IsGravityAid(g_session.GetAid());

	if (!(actorInfo && processType==0)) return;
	if (!(g_mouse.GetRBtn() == BTN_UP) || g_windowMgr.GetItemStoreWnd()) return;

	if ((actorInfo->GetJob() >= JT_WARPNPC && actorInfo->GetJob() < JT_2004_JOB_BEGIN) || g_session.IsMerOrHomun(actorInfo->GetJob())
		|| actorInfo->m_classType == CT_MER )
	{
			CGameActor *actor = m_world->GetGameActorByAID(actorInfo->GetGID());

#if defined (_HOMUN)
		if (actor && actorInfo->GetGID() == g_session.m_homunGID)
		{
			char msg[256];
			if (g_windowMgr.GetMenu())
				g_windowMgr.DeleteWindow(WID_MENUWND);
			m_menuIdList.erase(m_menuIdList.begin(), m_menuIdList.end());

			UIFrameWnd* menu = g_windowMgr.MakeWindow(WID_MENUWND);
			m_menuTargetAID = actorInfo->GetGID();



			m_menuIdList.push_back(MENUID_HOMUN_SHOWINFO);

			sprintf(msg, MsgStr(MSI_HOMUN_SHOWINFO));
			menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


			m_menuIdList.push_back(MENUID_HOMUN_FEEDING);
			sprintf(msg,MsgStr(MSI_HOMUN_FEEDING));
			menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


			m_menuIdList.push_back(MENUID_HOMUN_FOLLOW);
			sprintf(msg, MsgStr(MSI_HOMUN_WAITING));
			menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);

			menu->SendMsg(NULL, UM_ADJUSTSIZE, 0, 0, 0, 0);
			menu->Move(g_mouse.GetXPos()+1, g_mouse.GetYPos()+1);
			return;
		}
#endif

#if defined (_MER) || defined (_MER_R2)
		if (actor && actorInfo->GetGID() == g_session.m_merGID)
		{
			char msg[256];
			if (g_windowMgr.GetMenu())
				g_windowMgr.DeleteWindow(WID_MENUWND);
			m_menuIdList.erase(m_menuIdList.begin(), m_menuIdList.end());
			UIFrameWnd* menu = g_windowMgr.MakeWindow(WID_MENUWND);
			m_menuTargetAID = actorInfo->GetGID();


			m_menuIdList.push_back(MENUID_MER_SHOWINFO);
			sprintf(msg, MsgStr(MSI_HOMUN_SHOWINFO));
			menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


			m_menuIdList.push_back(MENUID_MER_FOLLOW);
			sprintf(msg, MsgStr(MSI_HOMUN_WAITING));
			menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);
			menu->SendMsg(NULL, UM_ADJUSTSIZE, 0, 0, 0, 0);
			menu->Move(g_mouse.GetXPos()+1, g_mouse.GetYPos()+1);
			return;
		}
#endif

			if (actor && actorInfo->GetGID() == g_session.m_petGID)
		{
			if (actor->GetHeadType() == HEADTYPE_LAST)
			{
				char msg[256];
				if (g_windowMgr.GetMenu())
					g_windowMgr.DeleteWindow(WID_MENUWND);
				m_menuIdList.erase(m_menuIdList.begin(), m_menuIdList.end());
				UIFrameWnd* menu = g_windowMgr.MakeWindow(WID_MENUWND);
				m_menuTargetAID = actorInfo->GetGID();


				m_menuIdList.push_back(MENUID_PET_SHOWINFO);
				sprintf(msg, MsgStr(MSI_PET_SHOWINFO));
				menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


				m_menuIdList.push_back(MENUID_PET_FEEDING);
				sprintf(msg, MsgStr(MSI_PET_FEEDING));
				menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


				m_menuIdList.push_back(MENUID_PET_PERFORMANCE);
				sprintf(msg, MsgStr(MSI_PET_PERFORMANCE));
				menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


				m_menuIdList.push_back(MENUID_PET_ACC_OFF);
				sprintf(msg, MsgStr(MSI_PET_ACC_OFF));
				menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


				m_menuIdList.push_back(MENUID_PET_RETURN_EGG);
				sprintf(msg, MsgStr(MSI_PET_RETURN_EGG));
				menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);
				menu->SendMsg(NULL, UM_ADJUSTSIZE, 0, 0, 0, 0);
				menu->Move(g_mouse.GetXPos()+1, g_mouse.GetYPos()+1);
				return;
			}
		}
	}
	if (actorInfo->GetGID() == g_session.GetAid()) return;
	if (!( g_session.IsThisPC(actorInfo->GetJob()) || isGravityAid )) return;
	if (!(m_actorNameList.find(actorInfo->GetGID()) != m_actorNameList.end()) && !isGravityAid) return;
	if (!(m_actorNameList[actorInfo->GetGID()].cName.size()) && !isGravityAid) return;
	if (!(m_rBtnClickX == g_mouse.GetXPos() &&m_rBtnClickY == g_mouse.GetYPos())) return;

	if (g_windowMgr.GetMenu())
		g_windowMgr.DeleteWindow(WID_MENUWND);
	m_menuIdList.erase(m_menuIdList.begin(), m_menuIdList.end());
	UIFrameWnd* menu = g_windowMgr.MakeWindow(WID_MENUWND);
	RAG_BUDGET_FILL(60)
	m_menuTargetAID = actorInfo->GetGID();

	string cName = m_actorNameList[actorInfo->GetGID()].cName;



	if (isGravityAid)
	{
		m_menuIdList.push_back(MENUID_DISCONNECT_CHARACTER);
		sprintf(msg, MsgStr(MSI_BAN_USER), cName.c_str());
		menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);
	}
	CGameMode *		gameMode = g_modeMgr.GetGameMode();
	CGameActor *	Actor = NULL;
	Actor = gameMode->GetWorld()->m_player;

#ifdef	BIND_ON_EQUIP

	if (!IsEffectStateBurrow(Actor->GetEffectState()) && !IsEffectStateHiding(Actor->GetEffectState()))
	{
		m_menuIdList.push_back(MENUID_VIEW_OTHERUSER_EQUIPWND);
		sprintf(msg, MsgStr(MSI_REQ_VIEW_OTHERUSER), cName.c_str());
		menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);
	}
#endif



	if (!IsEffectStateBurrow(Actor->GetEffectState()) && !IsEffectStateHiding(Actor->GetEffectState()))
	{


		m_menuIdList.push_back(MENUID_DEAL);
		sprintf(msg, MsgStr(MSI_REQ_DEAL_WITH2), cName.c_str());

		menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);

	}









	if (g_session.m_amIPartyMaster && !(g_session.GetPartyInfoByName(cName.c_str()).isValid))
	{
		m_menuIdList.push_back(MENUID_INVITE_IN_PARTY);
		sprintf(msg, MsgStr(MSI_REQ_JOIN_PARTY2), cName.c_str());
		menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


	}

#if defined (_UPDATE2004B)


	if ( g_session.GetLevel() >= 70 )
	{
		if ( g_session.GetJob() < JT_NOVICE_B || g_session.GetJob() > JT_SUPERNOVICE_B )
		{
			if (g_session.m_amIParent && (g_session.GetPartyInfoByName(cName.c_str()).isValid)
				&& (actorInfo->GetJob() <= JT_THIEF || actorInfo->GetJob() == JT_SUPERNOVICE) )
			{
				m_menuIdList.push_back(MENUID_INVITE_IN_BABY);
				sprintf(msg, MsgStr(MSI_REQ_JOIN_BABY), cName.c_str());
				menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);
			}
		}
	}
#endif


	if (g_guildInfo.m_gdid) {
		int actorGdid;
			CGameActor * theActor = GetWorld()->GetGameActorByAID(actorInfo->GetGID());
		if (theActor) {
			actorGdid = theActor->GetGdid();
		} else {
			actorGdid = 0;
		}


		if (g_guildInfo.m_rightJoin && !actorGdid)
		{
			m_menuIdList.push_back(MENUID_INVITE_IN_GUILD);
			sprintf(msg, MsgStr(MSI_REQ_JOIN_GUILD), cName.c_str());
			menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);


		}



		if (g_guildInfo.m_amIMaster && (actorGdid != g_guildInfo.m_gdid))
		{
			menu->SendMsg(NULL, UM_ADDBOUNDARY, 0, 0, 0, 0);

			m_menuIdList.push_back(MENUID_ALLY_GUILD);
			menu->SendMsg(NULL, UM_ADDITEM, int(MsgStr(MSI_REQ_ALLY_GUILD)), 0, 0, 0);
			m_menuIdList.push_back(MENUID_HOSTILE_GUILD);
			menu->SendMsg(NULL, UM_ADDITEM, int(MsgStr(MSI_REQ_HOSTILE_GUILD)), 0, 0, 0);




		}
	}






	menu->SendMsg(NULL, UM_ADDBOUNDARY, 0, 0, 0, 0);


	m_menuIdList.push_back(MENUID_MAKE_WHISPER_WINDOW);
	menu->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_OPEN_1ON1_WINDOW), 0, 0, 0);

	if (!IsGravityAid(actorInfo->GetGID()))
	{
		if (!g_windowMgr.IsFriendName(cName.c_str()))
		{
			if (g_serverType != ServerInstantEvent)
			{
				m_menuIdList.push_back(MENUID_ADD_TO_FRIEND_CHAR_LIST);
				menu->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_ADD_TO_FRIEND_CHAR_LIST), 0, 0, 0);
			}
		}
	}


	menu->SendMsg(NULL, UM_ADDBOUNDARY, 0, 0, 0, 0);


	if (g_session.IsExName(cName.c_str()))
	{
		m_menuIdList.push_back(MENUID_REQ_IN);
		menu->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_REQ_IN), 0, 0, 0);

	} else {
		m_menuIdList.push_back(MENUID_REQ_EX_LIST);
		menu->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_REQ_EX_LIST), 0, 0, 0);
	}

#if (_PACKET_VER < 7)
	if(false)
#endif
	{
		if (IsGravityAid(g_session.GetAid()))
		{

			menu->SendMsg(NULL, UM_ADDBOUNDARY, 0, 0, 0, 0);

#ifndef	INSTANT_EVENT_SERVER
			if ( g_serverType != ServerInstantEvent )
			{

				m_menuIdList.push_back(MENUID_REQ_MANNER_POINT_PLUS);
				menu->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_GIVE_PLUS_MANNER_POINT), 0, 0, 0);


				m_menuIdList.push_back(MENUID_REQ_MANNER_POINT_MINUS);
				menu->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_GIVE_MINUS_MANNER_POINT), 0, 0, 0);


				sprintf(msg, MsgStr(MSI_PROHIBIT_LOG), g_session.GetChatProhibittedCnt(m_menuTargetAID));
				m_menuIdList.push_back(MENUID_SHOW_CHAT_PROHIBIT_LOG);
				menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);

			}
#endif
			sprintf(msg, "%s : %d (GID) : %s", g_session.m_lastServerName.c_str(), m_menuTargetAID, cName.c_str());
			m_menuIdList.push_back(MENUID_COPY_GID); RAG_BUDGET_SITES(1)
			menu->SendMsg(NULL, UM_ADDITEM, int(msg), 0, 0, 0);
		}
	}
	menu->SendMsg(NULL, UM_ADJUSTSIZE, 0, 0, 0, 0);
	menu->Move(g_mouse.GetXPos()+1, g_mouse.GetYPos()+1);
}

void CGameMode::ProcessWheel(int processType)
{
	CGameActor *actor = g_session.GetGameActorByAid(1);
	CGameActor *boy = g_session.GetGameActorByAid(100);

	if (g_mouse.GetWheel() > 0) {
		if (g_session.HaveThisSkill(0x111) >= 1) {
			if (g_used_Skill == 0x107) {
				if (g_used_Skill_Time + 1000 >= timeGetTime()) {
					if (actor) {
						g_NextSkill = actor->IsEffectExist(EF_ASURABODY) ? 0x10f : 0x111;
					}
				}
			}
		}
	} else if (g_mouse.GetWheel() < 0) {
		if (g_session.HaveThisSkill(0x19d) >= 1) {
			if (boy) {
				if (boy->m_isSprArgbFixed == 1) {
					g_NextSkill = 0x19d;
				}
			}
			if (g_session.HaveThisSkill(0x19d) >= 2) {
				if (g_BoyAttackedTime > timeGetTime()) {
					g_NextSkill = 0x19d;
				}
			}
		}
	}

	if (g_mouse.GetWBtn() == BTN_DOWN) {
		if (g_session.HaveThisSkill(0x10e) >= 1) {
			g_NextSkill = 0x10e;
		}
	}

	if (g_curScreen == 12 && g_curStance == 5) {
		char moveType = 0;

		if (g_mouse.GetWheel() > 0) {
			moveType = 1;
			actor->SendMsg(NULL, AM_CHAT,
				(int)"OK~ I'll keep going ^_^*",
				0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
			g_curStance = 6;
		}

		if (g_mouse.GetWheel() < 0) {
			if ((map_Number == 10 && g_curStory < 20)
				|| (map_Number == 11 && g_curStory < 30)) {
				actor->SendMsg(NULL, AM_CHAT,
					(int)"That way looks dangerous right now. I'll go next time T_T",
					0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 0x1c, 0, 0);
				g_AcoPosition += 2;
				g_curStance = 6;
			} else {
				actor->SendMsg(NULL, AM_CHAT,
					(int)"OK~ Side path it is~ ^^;",
					0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 0, 0, 0);
				g_curStance = 7;
				g_FieldConv_Alpha = 1;
				g_AcoPosition += 1000;
			}
		} else if (g_curStance >= 6) {
			if (moveType == 1) {
				g_AcoPosition += 2;
			} else if (moveType == 2) {
				g_AcoPosition += 1000;
			}
		}

		if (g_curStance >= 6) {
			g_MainNextActTime = timeGetTime() + 500;
			int nextOutTime = rand() % 7000;
			g_MobNextOutTime = timeGetTime() + nextOutTime + 7000;
		}
	}
}

void CGameMode::ProcessRBtn()
{
	EBtnState rBtn;
	rBtn = g_mouse.GetRBtn();

	switch (rBtn) {
	case BTN_PRESSED:
		if (!CheckKeyPressed(VK_MENU) && !CheckKeyPressed(VK_SHIFT)) {
		}

		if (!CheckKeyPressed(VK_MENU) && CheckKeyPressed(VK_SHIFT)) {
			float deltaLatitude = 10.0f;
			int deltaMouseY = (g_mouse.GetYPos() - m_oldMouseY) / 2;
			if (m_canRotateView)
				deltaLatitude = 20.0f;

			g_viewLatitude = m_view->GetDestLatitude() - deltaMouseY;
			if (m_hasViewPoint) {
				if (g_viewLatitude < ViewPointData[7])
					g_viewLatitude = ViewPointData[7];
				if (g_viewLatitude > ViewPointData[6])
					g_viewLatitude = ViewPointData[6];
			} else {
				if (g_viewLatitude < -45.0f - deltaLatitude)
					g_viewLatitude = -45.0f - deltaLatitude;
				if (CheckKeyPressed(VK_CONTROL) && m_view->GetDestDistance() <= FAR_DIST) {
					if (g_viewLatitude > -45.0f + deltaLatitude)
						g_viewLatitude = -45.0f + deltaLatitude;
				} else {
					if (g_viewLatitude > -70.0f + deltaLatitude)
						g_viewLatitude = -70.0f + deltaLatitude;
				}
			}
			m_view->SetDestLatitude(g_viewLatitude);
		}

		if (CheckKeyPressed(VK_CONTROL)) {
			g_viewDistance = m_view->GetDestDistance() - (g_mouse.GetYPos() - m_oldMouseY) / 2;
			float farDist = AVG_DIST;
			if (m_canRotateView)
				farDist = FAR_DIST;

			if (m_hasViewPoint) {
				if (g_viewDistance < ViewPointData[0])
					g_viewDistance = ViewPointData[0];
				if (g_viewDistance > ViewPointData[0] + ViewPointData[1])
					g_viewDistance = ViewPointData[0] + ViewPointData[1];
			} else {
				if (!g_session.IsMasterAid(5)) {
					if (g_viewDistance < CLOSE_DIST)
						g_viewDistance = CLOSE_DIST;
					if (g_viewDistance > farDist)
						g_viewDistance = farDist;
				}
			}
			m_view->SetDestDistance(g_viewDistance);
		}
		break;
	case BTN_DOWN:
		if (g_windowMgr.GetMenu())
			g_windowMgr.DeleteWindow(WID_MENUWND);
		SendMsg(MM_CANCEL_DRAG);
		SendMsg(MM_CANCEL_USE_SKILL);
		m_rBtnClickX = g_mouse.GetXPos();
		m_rBtnClickY = g_mouse.GetYPos();

		if (g_session.m_isQuickSpell == TRUE) {
			UIShortCutWnd *shortCutWnd = g_windowMgr.GetShortCutWnd();
			if (shortCutWnd)
				shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 0, 0, 0);
		}
		break;
	case BTN_DBLCLK:
		if (!m_hasViewPoint) {
			if (CheckKeyPressed(VK_CONTROL)) {
				m_view->SetDestDistance(AVG_DIST);
			}
			if (CheckKeyPressed(VK_SHIFT)) {
				m_view->SetDestLatitude(-50.0f);
			}

			if (g_session.m_isQuickSpell != TRUE) {
				if (m_canRotateView && !CheckKeyPressed(VK_SHIFT) && !CheckKeyPressed(VK_CONTROL)) {
					m_view->SetDestLongitude(0.0f);
				}
			}
		}
		break;
	}
}


int		CGameMode :: ProcessGroundPick(int processType)
{
	if (m_skillUseInfo.m_skillUseType==SUT_TO_CHARACTER || m_skillUseInfo.m_skillUseType==SUT_TO_ITEM || m_skillUseInfo.m_skillUseType==SUT_TO_ALL)
	{
		return processType;
	}























	int	dx = -1, dy = -1;
	bool groundAttrIsPicked = GetGroundAttrPos(dx, dy);

	if (groundAttrIsPicked)
	{

#if defined (_HOMUN)
		if ( g_session.m_homunAlive && CheckKeyPressed(VK_MENU) && g_mouse.GetRBtn()==BTN_DOWN )
		{
			if ( !g_session.m_selectHomunEnemy )
			{
				CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_homunGID ) ;
				if (CheckKeyPressed(VK_SHIFT))
				{
					if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
					{
						MSG2AI	msg;
						msg.command = MOVE_CMD ;
						msg.x		= dx ;
						msg.y		= dy ;
						actor->SetResMsg2AI (msg, MT_HOMUN);
					}
				}
				else
				{
					g_session.m_homunEnemyX = dx ;
					g_session.m_homunEnemyY = dy ;
					g_session.m_homunEnemyGID = 0 ;
					g_session.m_attackHomunEnemy = false ;

					if ( actor && actor->m_homunAI && actor->m_homunAI->IsOpened() )
					{
						MSG2AI	msg;
						msg.command = MOVE_CMD ;
						msg.x		= g_session.m_homunEnemyX ;
						msg.y		= g_session.m_homunEnemyY ;
						actor->SetMsg2AI ( msg, MT_HOMUN ) ;
					}

					g_session.m_moveHomunEnemy = true ;
				}
			}
			else
				g_session.m_selectHomunEnemy = false ;
		}
		else
#endif
		{
#if defined (_HOMUN)
			if ( g_session.m_homunAlive && g_mouse.GetRBtn()==BTN_DOWN )
				g_session.InitHomunTargetInfo ( ) ;
#endif

#if defined (_MER) || defined (_MER_R2)
			if ( g_session.m_merAlive && CheckKeyPressed(VK_MENU) && g_mouse.GetLBtn()==BTN_DOWN )
			{
				if ( !g_session.m_selectMerEnemy )
				{
					CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;
					if (CheckKeyPressed(VK_SHIFT))
					{
						if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
						{
							MSG2AI	msg;
							msg.command = MOVE_CMD ;
							msg.x		= dx ;
							msg.y		= dy ;
							actor->SetResMsg2AI (msg, MT_MER);
						}
					}
					else
					{
						g_session.m_merEnemyX = dx ;
						g_session.m_merEnemyY = dy ;
						g_session.m_merEnemyGID = 0 ;
						g_session.m_attackMerEnemy = false ;

						if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
						{
							MSG2AI	msg;
							msg.command = MOVE_CMD ;
							msg.x		= g_session.m_merEnemyX ;
							msg.y		= g_session.m_merEnemyY ;
							actor->SetMsg2AI ( msg, MT_MER ) ;
						}

						g_session.m_moveMerEnemy = true ;
					}
				}
				else
					g_session.m_selectMerEnemy = false ;
			}
			else
#endif
			{
#if defined (_MER) || defined (_MER_R2)
				if ( g_session.m_merAlive && g_mouse.GetLBtn()==BTN_DOWN )
				{
					g_session.InitMerTargetInfo ( ) ;


					if ( m_skillUseInfo.m_skillUseType==SUT_TO_GROUND &&
						m_skillUseInfo.m_skillId >= SKID_MERCENARY_BEGIN && m_skillUseInfo.m_skillId < SKID_MERCENARY_LAST )
					{
						CGameActor * actor = GetWorld()->GetGameActorByAID ( g_session.m_merGID ) ;

							if ( actor && actor->m_merAI && actor->m_merAI->IsOpened() )
							{
								if (actor->m_stateId==ST_MOVE || actor->m_stateId==ST_STAND || actor->m_stateId==4)
								{
								processType = 1;

								MSG2AI	msg;
								msg.command = SKILL_AREA_CMD ;
								msg.skillLevel	= m_skillUseInfo.m_useLevel ;
								msg.skillID		  = m_skillUseInfo.m_skillId ;
								msg.x				= dx ;
								msg.y				= dy ;

								actor->SetMsg2AI ( msg, MT_MER ) ;

								return processType ;
							}
						}
					}
				}
#endif

				if (m_skillUseInfo.m_skillUseType==SUT_TO_GROUND && g_mouse.GetLBtn()==BTN_DOWN && (processType==0 || processType==2))
				{
					processType = 1;
					if (m_world->m_player != NULL)
					{
						m_world->m_player->SendMsg(NULL, AM_PROCEEDGROUNDSKILL, m_skillUseInfo.m_skillId, dx, dy);
						m_world->m_player->SendMsg(NULL, AM_SET_SKILL_USE_LEVEL, m_skillUseInfo.m_useLevel, 0, 0);
					}
				}

					if (!CheckKeyPressed(VK_CONTROL) && !m_noMove && (processType==0 || processType==2) && m_dragInfo.m_dragType == DT_NODRAG)
					{
					if (g_session.m_isAttrIndicator)
					{
						g_groundAttrPosInfo.x = dx;
						g_groundAttrPosInfo.y = dy;
						g_groundAttrPosInfo.isRendered = false;

						int cellFlag = GetCellFlag(dx, dy);
						DWORD color;
						if (cellFlag == 1)
							color = 0xfeff0000;
						else if (cellFlag == 5)
							color = 0xfeffff00;
						else
							color = 0x6600ffaa;

						if (cellFlag==1 || cellFlag==5)
							g_groundAttrPosInfo.isRendered = true;
						g_groundAttrPosInfo.cellColor = color;
					}

						bool autoClickEnoughDelayed = timeGetTime()>m_leftBtnClickTick + 600;
						bool autoClick = (!m_world->m_player->m_proceedType && g_mouse.GetLBtn()==BTN_PRESSED && autoClickEnoughDelayed && m_isAutoMoveClickOn);
						bool clickEnoughDelayed = ( timeGetTime()>( m_leftBtnClickTick+225 ) );

						if (( ( g_mouse.GetLBtn()==BTN_DOWN )&&
							( g_mouse.GetRBtn() != BTN_PRESSED ) &&
							clickEnoughDelayed
							)
							|| autoClick)
						{


						m_leftBtnClickTick = timeGetTime();

							if (CheckKeyPressed(VK_SHIFT) || g_windowMgr.GetChatRoomWnd() ||
								m_world->m_player->m_stateId == 6)
							{
								m_world->m_player->SendMsg(NULL, AM_REQ_CHANGE_DIR, dx, dy, 0);
							}
						else
						{
							if (!IsValidCell(dx, dy))
							{
								if (!GetClosestValidCell(dx, dy))
								{
									goto SKIP_MOVE;
								}
							}

								if (g_windowMgr.GetMenu())
									g_windowMgr.DeleteWindow(WID_MENUWND);

								if (m_world->m_player != NULL)
								{
									m_world->m_player->SendMsg(NULL, AM_REQ_MOVE, dx, dy, 0);
							}
							else
							{
							}
SKIP_MOVE:
							NULL;
						}


					}
					else
					{

					}
				}
				else
				{



				}
			}
		}
	}
	return processType;
}

void	CGameMode :: ProcessReqEmblem()
{
	if (m_emblemReqGdidQueue.size() && timeGetTime() > m_lastEmblemReqTick+5000) {
		SendMsg(MM_REQ_GUILD_EMBLEM_IMG, *m_emblemReqGdidQueue.begin(), 0, 0);
		m_lastEmblemReqTick = timeGetTime();
	}
}

void	CGameMode :: ProcessNameWaitingTimer()
{
	if (timeGetTime() > m_lastNameWaitingListTick+1000)
	{
		g_windowMgr.RefreshNameWaitingWindowList();
		m_lastNameWaitingListTick = timeGetTime();
	}
}

void	CGameMode :: ProcessPeriodicalJobs()
{
	ProcessReqEmblem();
	ProcessNameWaitingTimer();
}

int g_shiftKeyPressed = FALSE;

void	CGameMode :: ProcessInput()
{
	g_mouse.ReadState();

	int processType = g_windowMgr.ProcessInput();
	g_windowMgr.OnProcess();

	ProcessNameBalloon();
	ProcessHelpBalloon();
	ProcessRBtn();
	ProcessWheel(processType);
	ProcessKeyBoard();
	ProcessPeriodicalJobs();

	if (processType==2)
		processType = 0;

	if (m_skillUseInfo.m_skillUseType != SUT_NOSKILL)
	{
		if (g_session.m_isQ2begin == 0)
		{
			if (g_mouse.GetWheel() > 0)
			{
				SendMsg(MM_USESKILL_LEVEL_PLUS, 1);
			}
			else if (g_mouse.GetWheel() < 0)
			{
				SendMsg(MM_USESKILL_LEVEL_MINUS, 1);
			}
		}
		g_session.m_isQ2begin = 0;
	}

		if (g_mouse.GetLBtn() == BTN_UP)
		{
			if (m_world->m_player != NULL)
				m_world->m_player->SendMsg(NULL, AM_ATTACKCLICKUP, 0, 0, 0);

		SendMsg(MM_DROP, g_mouse.GetXPos(), g_mouse.GetYPos());
		if (g_windowMgr.GetMenu())
			g_windowMgr.DeleteWindow(WID_MENUWND);
		m_isAutoMoveClickOn = 1;
	}

	g_shiftKeyPressed = CheckKeyPressed(VK_SHIFT);

	CActorPickInfo *actorInfo = g_actorPickNode.FindPickInfo(vector2d(g_mouse.GetXPos(), g_mouse.GetYPos()));

	UIExchangeWnd *exchangeWnd = g_windowMgr.GetExchangeWnd();
	if ( !exchangeWnd )
	{
		MakeMenu(actorInfo, processType);
	}

	ProcessMouseSnap(actorInfo, processType);
	HideCheck();

	if ( !m_isOnQuest && !m_isPlayerDead && !exchangeWnd && !g_windowMgr.GetItemStoreWnd() &&
		!g_windowMgr.GetMailListWnd() && !g_windowMgr.GetMailViewWnd() && !g_windowMgr.GetAuctionWnd() )
	{
		processType = ProcessActorPick(processType, actorInfo);
		processType = ProcessGroundPick(processType);
		ProcessLockOnMouse();
	}
	else
	{
		if (actorInfo && processType==0)
			MakeNameBalloon(actorInfo->m_gid);
	}

	if (g_mouse.GetLBtn()==BTN_DOWN)
	{
		g_checkBOT = 0;

		SendMsg(MM_CANCEL_USE_SKILL);
		g_windowMgr.DeleteWindow(WID_PASSWORDWND);

		if (timeGetTime() > m_noMoveStartTick+5000)
			m_noMove = false;
		m_leftBtnClickTick = timeGetTime();
	}

	bool hangUseSkill = m_skillUseInfo.m_skillUseType != SUT_NOSKILL;
	if (hangUseSkill && processType==0)
	{
		SetCursorAction(CURSOR_TARGET);
	}
	else if (processType == 0 && (g_mouse.GetRBtn() != BTN_PRESSED) && !hangUseSkill)
	{
		SetCursorAction(CURSOR_ARROW);
	}

	m_oldMouseX = g_mouse.GetXPos();
	m_oldMouseY = g_mouse.GetYPos();
}


void CGameMode::OnChangeState(int state) {}

void CGameMode::OnProcessState(int state)
{
	if (state == GSM_WAITRESP_FROM_ZOSVR && !m_isConnected && !g_windowMgr.IsModalStatus())
	{
		g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED), ME_OK, 1, 0, 0);
		SendMsg(MM_QUIT, 0, 0, 0);
	}
}

CGameActor* CGameMode::NewNpc(int job)
{
	char* strJobFn = (char*)g_session.GetJobName(job);
	if (strstr(strJobFn, ".gr2"))
		return new C3dGrannyGameActor(strJobFn);

	return new CNpc;
}

CGameActor* CGameMode::MakeGameActor(int job)
{
	CGameActor* theActor = NULL;
	if (g_session.IsThisPC(job))
	{
		theActor = new CPc;
	}
	else if (job >= JT_WARPNPC && job < JT_MON_BEGIN)
	{
		theActor = new CNpc;
	}
	else if (job > JT_MON_BEGIN)
	{
		theActor = new CNpc;
	}
	return theActor;
}

const char* CGameMode::GetDragItemName()
{
	switch (m_dragInfo.m_dragType) {
	case DT_FROM_ITEMWND:
		{
			ITEM_INFO& itemInfo = g_session.GetItemInfoByIndex(m_dragInfo.m_dragItemIndex);
			if (itemInfo.IsValid()) return itemInfo.m_itemName.c_str();
			return NULL;
		}
		break;
	case DT_FROM_EQUIPWND:
		{
			ITEM_INFO& itemInfo = g_session.GetEquipedItems(m_dragInfo.m_slotNum);
			if (itemInfo.IsValid()) return itemInfo.m_itemName.c_str();
			return NULL;
		}
	default:
		if (m_dragInfo.m_dragItemName.size() == 0) return NULL;
		return m_dragInfo.m_dragItemName.c_str();
	}
}

DWORD	CGameMode :: GetAid(const char * cName)
{
	std::map <DWORD, NamePair> :: iterator it = m_actorNameList.begin();
	while (it != m_actorNameList.end()) {
		if (it->second.cName == cName)
			return it->first;
		++it;
	}
	return 0;
}

void	CGameMode :: DeleteCompassPos(int id)
{
	std::map <DWORD, ColorCellPos2> :: iterator it = m_compassPosList.find(id);
	if (it != m_compassPosList.end()) {
		m_compassPosList.erase(it);
	}
}

void	CGameMode :: MakeFog(BOOL fogOn)
{
	if (fogOn) {
		std::vector<std::string> &paramList = g_session.GetFogParameter(m_rswName, fogOn);
		if (fogOn) {
			g_fogPara.start   = (float)atof(paramList[0].c_str());
			g_fogPara.end     = (float)atof(paramList[1].c_str());
			g_fogPara.color   = aToDW_HEX(paramList[2].c_str());
			g_fogPara.density = (float)atof(paramList[3].c_str());
			g_renderer->FogSwitch(TRUE);
			return;
		}
	}
	g_renderer->FogSwitch(FALSE);
}





inline void rag_symbol_shift(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20, int a21, int a22, int a23, int a24, int a25, int a26, int a27) { a0 = 0; a1 = 0; a2 = 0; a3 = 0; a4 = 0; a5 = 0; a6 = 0; a7 = 0; a8 = 0; a9 = 0; a10 = 0; a11 = 0; a12 = 0; a13 = 0; a14 = 0; a15 = 0; a16 = 0; a17 = 0; a18 = 0; a19 = 0; a20 = 0; a21 = 0; a22 = 0; a23 = 0; a24 = 0; a25 = 0; a26 = 0; a27 = 0; }

void CGameMode::MakeNameBalloonTextWindow(UIWindow **window)
{
	if (*window)
		return;

	switch (m_nameBalloonType) {
	case 0:
		*window = new UINameBalloonText;
		break;
	case 1:
		*window = new UINameBalloonText;
		break;
	case 2:
		*window = new UIVerticalNameBalloonText;
		break;
	case 3:
		*window = new UIVerticalNameBalloonText;
		break;
	default:
		*window = new UINameBalloonText;
		break;
	}

	(*window)->Create(8, 8);
	g_windowMgr.AddWindow(*window);
}

void CGameMode::MoveBalloonTextWindow(UIWindow *window, CRenderObject *renderObject)
{
	int oldType = m_nameBalloonType;
	int width = window->GetWidth();
	int height = window->GetHeight();
	int offsetX;
	int offsetY;

	if (!renderObject->m_isPc)
		m_nameBalloonType = 0;

	switch (m_nameBalloonType) {
	case 0:
		offsetX = -width / 2;
		offsetY = int((20 * g_renderer->m_height / 480) * renderObject->m_lastPixelRatio);
		break;
	case 2:
		offsetX = int((-30 * g_renderer->m_width / 640) * renderObject->m_lastPixelRatio - width / 2);
		offsetY = int((-25 * g_renderer->m_height / 480) * renderObject->m_lastPixelRatio - height / 2);
		break;
	case 3:
		offsetX = int((30 * g_renderer->m_width / 640) * renderObject->m_lastPixelRatio - width / 2);
		offsetY = int((-25 * g_renderer->m_height / 480) * renderObject->m_lastPixelRatio - height / 2);
		break;
	case 1:
		offsetX = -width / 2;
		offsetY = int((-100 * g_renderer->m_height / 480) * renderObject->m_lastPixelRatio);
		break;
	}

	m_nameBalloonType = oldType;

	int x = renderObject->m_lastTlvertX + offsetX;
	int y = renderObject->m_lastTlvertY + offsetY;
	x = __max(x, -3);
	x = __min(x, g_renderer->m_width - window->GetWidth() + 3);
	y = __max(y, -3);
	y = __min(y, g_renderer->m_height - window->GetHeight() + 3);

	window->Move(x, y);
}
void	CGameMode :: ProcessTalkType(int val1, int val2)
{
	rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TALKTYPE talkType = (TALKTYPE)val1;
	string* talkContent = (string*)val2;

#if defined (_UPDATE2005A)

	TTPair2 *it = g_session.m_cmdOnOffSet.begin() ;

	while ( it != g_session.m_cmdOnOffSet.end ( ) )
	{
		if ( it->second == talkType )
		{
			it->first = !it->first ;
			break;
		}
		++it;
	}
#endif
	if (g_windowMgr.GetUICommandListWnd())
		g_windowMgr.GetUICommandListWnd()->Invalidate();


	if ((ServiceVietnam == g_serviceType) && (TT_REQ_NUM_USER==talkType))
		talkType = TT_UNKNOWN;

	switch (talkType) {
		case TT_IS_REDCELL:
			{
			}
			break;
		case TT_REQ_WHISPER_LIST:
			{
				PACKET_CZ_REQ_WHISPER_LIST p;
				p.PacketType = HEADER_CZ_REQ_WHISPER_LIST;
				g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)&p);
			}
			break;
		case TT_REQ_WHISPER_STATE_EX:
			{
				m_isWaitingWhisperSetting = TRUE;
				PACKET_CZ_SETTING_WHISPER_STATE p;
				p.PacketType = HEADER_CZ_SETTING_WHISPER_STATE;
				p.type = WT_EXCLUDE_WHISPER;
				g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)&p);
			}
			break;

		case TT_GUILD_CHAT:
			{
				const char * param = talkContent->c_str();
				g_windowMgr.SetChatMsg(param);
				SendMsg(MM_CHAT_GUILD);
			}
			break;
		case TT_REQ_WHISPER_PC_EX:
			{

				m_isWaitingWhisperSetting = TRUE;
				const char * param = talkContent->c_str();
				m_refuseWhisperName = param;
				if (m_refuseWhisperName.size()>NAME_LENGTH)
					break;
				char name[NAME_LENGTH+1];
				strcpy(name, m_refuseWhisperName.c_str());

				PACKET_CZ_SETTING_WHISPER_PC p;
				p.PacketType = HEADER_CZ_SETTING_WHISPER_PC;
				memcpy(p.name, name, NAME_LENGTH);
				p.type = WT_EXCLUDE_WHISPER;
				g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)&p);
			}
			break;
		case TT_REQ_WHISPER_PC_IN:
			{
				m_isWaitingWhisperSetting = TRUE;
				const char * param = talkContent->c_str();
				m_refuseWhisperName = param;
				if (m_refuseWhisperName.size()>NAME_LENGTH)
					break;
				char name[NAME_LENGTH+1];
				strcpy(name, m_refuseWhisperName.c_str());

				PACKET_CZ_SETTING_WHISPER_PC p;
				p.PacketType = HEADER_CZ_SETTING_WHISPER_PC;
				memcpy(p.name, name, NAME_LENGTH);
				p.type = WT_INCLUDE_WHISPER;
				g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)&p);
			}
			break;
		case TT_REQ_WHISPER_STATE_IN:
			{
				m_isWaitingWhisperSetting = TRUE;
				PACKET_CZ_SETTING_WHISPER_STATE p;
				p.PacketType = HEADER_CZ_SETTING_WHISPER_STATE;
				p.type = WT_INCLUDE_WHISPER;
				g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)&p);
				g_session.DeleteAllExNameList();
				g_session.DeleteAllExAidList();
			}
			break;
		case TT_REQ_SAVE_CHAT_FROM_CHATROOM:
			{
				g_windowMgr.SendMsg(UIM_SAVE_CHAT_TO_FILE_FROM_CHATROOM, 0, 0, 0, 0);
			}
			break;
		case TT_REQ_SAVE_CHAT:
			{
				g_windowMgr.SendMsg(UIM_SAVE_CHAT_TO_FILE, 0, 0, 0, 0);
			}
			break;
#if (_PACKET_VER >= 8-2)
		case TT_REQ_REPORT_CHAT_FROM_CHATROOM:
			{
				g_windowMgr.SendMsg(UIM_REPORT_CHAT_TO_FILE_FROM_CHATROOM, 0, 0, 0, 0);
			}
			break;
		case TT_REQ_REPORT_CHAT:
			{
				g_windowMgr.SendMsg(UIM_REPORT_CHAT_TO_FILE, 0, 0, 0, 0);
			}
			break;
#endif
		case	TT_REQ_AUTO_SAVE_CHAT:
			{
				if (!g_session.IsMasterAid(7))
				{
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_INVALID_COMMAND)), 0xffff, 0, 0);
					break;
				}
				g_session.m_isSaveChat = 2 - g_session.m_isSaveChat - 1;
				char chatMsg[128];
				if (g_session.m_isSaveChat) {
					g_modeMgr.GetGameMode()->MakeChatFile();
					sprintf(chatMsg, "Chatting Auto Save Start..");
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xffff, 0, 0);
				} else {
					sprintf(chatMsg, "Chatting Auto Save END");
					g_modeMgr.GetGameMode()->CloseChatFile();
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(chatMsg), 0xffff, 0, 0);
				}
			}
			break;
		case TT_NORMAL_FROM_CHATROOM:
		case TT_NORMAL:
			{
				BOOL isToggle = FALSE;
				BOOL isGuildChat = FALSE;
				char chat2[maxChatSize+1];
				strcpy(chat2, g_windowMgr.m_chatMsg.c_str());
				if (chat2[0] == '%') {
					if (strlen(chat2)<=1)
						break;
					g_windowMgr.SetChatMsg(&(chat2[1]));
					isToggle = TRUE;
				} else if (chat2[0] == '$') {
					if (strlen(chat2)<=1)
						break;
					g_windowMgr.SetChatMsg(&(chat2[1]));
					isGuildChat = TRUE;
				}
				if (CheckKeyPressed(VK_CONTROL)) {
					isToggle = TRUE;
				}
				if (CheckKeyPressed(VK_MENU)) {
					isGuildChat = TRUE;
				}

				switch (talkType)
				{
				case TT_NORMAL_FROM_CHATROOM:
					if (g_session.m_talkType == 2 && g_guildInfo.m_gdid) {
						SendMsg(MM_CHAT_GUILD);
					} else if (isToggle && g_session.GetNumParty()) {
						SendMsg(MM_CHAT_PARTY);
					} else {
						SendMsg(MM_CHATMSG);
					}
					break;
				default:
					{
						if (g_session.m_isBattleChat)
						{
							SendMsg(MM_CHAT_BATTLEFIELD);
						}
						else
						{
							if (g_session.m_talkType == 2 && g_guildInfo.m_gdid)
							{
								if (!isGuildChat)
									SendMsg(MM_CHAT_GUILD);
								else
									SendMsg(MM_CHATMSG);
							}
							else if (g_session.m_talkType == 1)
							{
								if (!isToggle && g_session.GetNumParty())
									SendMsg(MM_CHAT_PARTY);
								else
									SendMsg(MM_CHATMSG);
							}
							else
							{
								if (isToggle && g_session.GetNumParty())
									SendMsg(MM_CHAT_PARTY);
								else if(isGuildChat)
									SendMsg(MM_CHAT_GUILD);
								else
									SendMsg(MM_CHATMSG);
							}
						}
					}
					break;
				}
			}
			break;
		case TT_BROADCAST:
			SendMsg(MM_BROADCASTMSG);
			break;
		case	TT_LOCALBROADCAST:
			SendMsg(MM_BROADCASTMSG, 1);
			break;
		case	TT_NAMELESS_BROADCAST:
			SendMsg(MM_BROADCASTMSG, 2);
			break;
		case	TT_NAMELESS_LOCALBROADCAST:
			SendMsg(MM_BROADCASTMSG, 3);
			break;
		case	TT_NAMELESS_BLUEBROADCAST:
			SendMsg(MM_BROADCASTMSG, 4);
			break;
		case	TT_NAMELESS_BLUELOCALBROADCAST:
			SendMsg(MM_BROADCASTMSG, 5);
			break;
		case TT_REQ_NUM_USER:
			SendMsg(MM_REQ_NUM_USER);
			break;
		case TT_HOMUN_WAIT:
#if defined (_HOMUN)
			SendMsg(MM_HOMUN_WAIT);
#endif
			break;
		case TT_REQ_BLACKSMITH:
#if defined (_UPDATE2005A)
			SendMsg(MM_REQ_BLACKSMITH);
#endif
			break;
		case TT_REQ_ALCHEMIST:
#if defined (_UPDATE2005A)
			SendMsg(MM_REQ_ALCHEMIST);
#endif
			break;
#if defined(KILLER_SYSTEM_V2)
		case TT_REQ_KILLER_RANK:
			SendMsg(MM_REQ_KILLER_RANK);
			break;
#endif
		case TT_REQ_TAEKWON:
#if defined (_UPDATE2005C)
			SendMsg(MM_REQ_TAEKWON);
#endif
			break;
		case TT_HI:
			{
				char msg[256];
				const char * param = talkContent->c_str(); if (strcmp(param, "") == 0)
				{
					g_session.HiFriends(0);
					sprintf(msg, "[ To Friends ] : (Hi) *^_^*");
				}
				else
				{
					string temp;
					const char * sentence = g_windowMgr.m_chatMsg.c_str();
					temp = sentence;
					g_session.HiFriends((int)sentence);
					if (g_serviceType == ServiceKorea)
						sprintf(msg, "[ To Friends ] : (Hi) %s", temp.c_str());
					else
						sprintf(msg, "[ To Friends ] : (Hi) %s%s", g_language->GetLanguageCharset(false), temp.c_str());
				}
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0x1effff, 0, 0);
			}
			break;
		case TT_REQ_MUNAK:
			SendMsg(MM_REQ_MUNAK);
			break;
		case TT_REQ_DEATHKNIGHT:
			SendMsg(MM_REQ_DEATHKNIGHT);
			break;
		case TT_REQ_COLLECTOR:
			SendMsg(MM_REQ_COLLECTOR);
			break;
		case TT_LEVEL:
			{
				for (int i=0;i<32;i++)
					SendMsg(MM_ITEM_CREATE, (int)"LEVEL");
			}
			break;

		case TT_SP:
			{
				for (int i=0;i<5;i++)
				{
					SendMsg(MM_ITEM_CREATE, (int)"SPPOINT");
					SendMsg(MM_ITEM_CREATE, (int)"JOBLEVEL");
				}
			}
			break;
		case TT_MAP_POS:
			{
				char msg[120];
				vector3d &pPos = m_world->m_player->m_pos;
				int dx, dy;
				GetServerCoor(pPos.x, pPos.z, dx, dy);
				string mapName = m_rswName;
				if (mapName.size()>4)
					mapName.resize(mapName.size()-4);
				sprintf(msg, "%s(%s) : %d, %d", g_session.GetMapName(m_rswName), mapName.c_str(), dx, dy);
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), int(RGB(255,255,100)), 0, 0);
				if (g_serviceType != ServiceGermany)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY2, int(msg), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_EFFECT_ON_OFF:
			{
				g_session.m_isEffectOn = 1 - g_session.m_isEffectOn;

				if( !strcmp("xmas_fild01.rsw", m_rswName) || !strcmp("xmas.rsw", m_rswName) ) {
					if(g_session.m_isEffectOn)
						g_Weather.LaunchSnow ();
					else
						g_Weather.StopSnow ();
				}
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isEffectOn)
				{
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_EFFECT_ON)), int(RGB(255,255,100)), 0, 0);
#if defined (_UPDATE2004B)
#if (_PACKET_VER >= 6)
					PACKET_CZ_LESSEFFECT p;
					p.PacketType = HEADER_CZ_LESSEFFECT;
					p.isLess = false;
					g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)(&p));
#endif
#endif
				}
				else
				{
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_EFFECT_OFF)), int(RGB(255,255,100)), 0, 0);
#if defined (_UPDATE2004B)
#if (_PACKET_VER >= 6)
					PACKET_CZ_LESSEFFECT p;
					p.PacketType = HEADER_CZ_LESSEFFECT;
					p.isLess = true;
					g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)(&p));
#endif
#endif
				}
			}
			break;
		case TT_DORIDORI_ON_OFF:
			{
				CGameActor *	mActor = NULL;
				mActor = m_world->m_player;

				if (mActor->m_curMotion == HEADERDIR_LEFT)
					mActor->m_curMotion = HEADERDIR_RIGHT;
				else
					mActor->m_curMotion = HEADERDIR_LEFT;

				SendMsg(MM_CHANGE_DIRECTION, DegToDir(mActor->m_roty), mActor->m_curMotion);


				if ( mActor->m_stateId == ST_SIT )
				{
					for (int i=0;i<5;i++)
						m_doritime[i] = m_doritime[i+1];

					m_doritime[5] = timeGetTime();

					if ( (m_doritime[5] - m_doritime[0] < 3000) && (m_doritime[5] - m_doritime[0] > 1500) )
					{
						SendMsg(MM_DORIDORI);
						for (int i=0;i<6;i++)
							m_doritime[i]=0;
					}
				}
			}
			break;
		case TT_MISS_EFFECT_ON_OFF:
			{
				g_session.m_bMakeMissEffect = 1 - g_session.m_bMakeMissEffect;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_bMakeMissEffect)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MISS_EFFECT_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MISS_EFFECT_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_NOSHIFT_ON_OFF:
			{
				g_session.m_isNoShift = 1 - g_session.m_isNoShift;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isNoShift)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOSHIFT_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOSHIFT_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_NOCTRL_ON_OFF:
			{
				g_session.m_isNoCtrl = 1 - g_session.m_isNoCtrl;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isNoCtrl)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOCTRL_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOCTRL_OFF)), int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;
		case TT_WINDOW_ON_OFF:
			{
				g_session.m_isWindow = 1 - g_session.m_isWindow;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isWindow)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_WINDOW_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_WINDOW_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_AURA_ON_OFF:
			{
				g_session.m_isAura = 1 - g_session.m_isAura;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isAura)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AURA_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AURA_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;


		case  TT_RANDOM_LOADING_SCREEN_ON_OFF :
			if(g_session.RandomScreenFlag)
			{
				g_session.RandomScreenFlag=false;
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"RANDOM LOADING SCREEN ON", 0x6e96ff, 0, 0);
			}
			else
			{
				g_session.RandomScreenFlag=true;
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"RANDOM LOADING SCREEN OFF", 0x6e96ff, 0, 0);
			}
			break;
		case TT_SET1_ON_OFF:
			{
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_SKILLFAIL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_SHOW_NAME_TYPE, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_NOCTRL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_WINDOW_ON_OFF, 0, 0);
			}
			break;
		case TT_SET2_ON_OFF:
			{
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_SKILLFAIL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_SHOW_NAME_TYPE, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_NOCTRL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_QUICKSPELL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_QUICKSPELL2_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_WINDOW_ON_OFF, 0, 0);
			}
			break;
		case TT_SET3_ON_OFF:
			{
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_SKILLFAIL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_SHOW_NAME_TYPE, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_NOCTRL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_WINDOW_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_LOGINOUT_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_BATTLE_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_SHOPPING_ON_OFF, 0, 0);

			}
			break;
		case TT_QUICKSPELL3_ON_OFF:
			{
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_QUICKSPELL_ON_OFF, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, TT_QUICKSPELL2_ON_OFF, 0, 0);
			}
			break;
		case TT_SIYEON_STATUS:
			{
			}
			break;

		case TT_SKILLFAIL_ON_OFF:
			{
				g_session.m_isSkillFail = 1 - g_session.m_isSkillFail;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isSkillFail)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SKILLFAIL_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SKILLFAIL_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_LOGINOUT_ON_OFF:
			{
				g_session.m_isLogInOut = 1 - g_session.m_isLogInOut;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isLogInOut)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_LOGINOUT_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_LOGINOUT_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_SHOPPING_ON_OFF:
			{
#if defined (_UPDATE2005C)
				g_session.m_isShopping = 1 - g_session.m_isShopping;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isShopping)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SHOPPING_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SHOPPING_OFF)), int(RGB(255,255,100)), 0, 0);
#endif
			}
			break;
		case TT_QUICKSPELL_ON_OFF:
			{
				g_session.m_isQuickSpell = 1 - g_session.m_isQuickSpell;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isQuickSpell)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_QUICKSPELL_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_QUICKSPELL_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_QUICKSPELL2_ON_OFF:
			{
				g_session.m_isQuickSpell2 = 1 - g_session.m_isQuickSpell2;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isQuickSpell2)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_QUICKSPELL2_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_QUICKSPELL2_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_MINIGAME_ON_OFF:
			{
			}
			break;
		case TT_BATTLE_ON_OFF:
			{
				g_session.m_isBattle = 1 - g_session.m_isBattle;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isBattle)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BATTLE_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BATTLE_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_BATTLECHAT_ON_OFF:
			{
				g_session.m_isBattleChat = 1 - g_session.m_isBattleChat;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isBattleChat)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BATTLECHAT_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BATTLECHAT_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_NOTALKMSG_ON_OFF:
			{
				g_session.m_isNoTalkMsg = 1 - g_session.m_isNoTalkMsg;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isNoTalkMsg)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTALKMSG_OFF)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTALKMSG_ON)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_NOTALKMSG2_ON_OFF:
			{
				g_session.m_isNoTalkMsg2 = 1 - g_session.m_isNoTalkMsg2;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isNoTalkMsg2)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTALKMSG2_OFF)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTALKMSG2_ON)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_NOTRADE_ON_OFF:
			{
				g_session.m_isNoTrade = 1 - g_session.m_isNoTrade;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isNoTrade)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTRADE_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_NOTRADE_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_SHOW_PVP_INFO:
			{
				if (m_world->IsPKZone())
				{
					PACKET_CZ_REQ_PVPPOINT p;
					p.PacketType = HEADER_CZ_REQ_PVPPOINT;
					p.AID = g_session.GetAid();
					p.GID = g_session.GetGid();
					g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)&p);
				}
			}
			break;
		case TT_AUTO_MSG_ON_OFF:
			{
				g_session.m_onAutoMSG = !g_session.m_onAutoMSG;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_onAutoMSG)
					g_windowMgr.MakeWindow(WID_AUTOMESSAGEWND);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_AUTO_MSG_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
#if defined(EPISODE_VER) || defined(_DEBUG)
		case TT_FRAME_ON_OFF:
			{




			}
			break;
#endif

#if defined (_HOMUN)
		case TT_TRACEAI_ON_OFF:
			{
				g_session.m_onTraceAI = !g_session.m_onTraceAI ;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

				if (g_session.m_onTraceAI)
				{
					char curPath[256];
					GetCurrentDirectory(256, curPath);

					int Pos = (int)strstr(curPath, "\\data"), logCnt = 0, size = 0;

					if (Pos)
					{
						Pos = Pos - (int)curPath;
						memcpy(curPath, curPath, Pos);
					}
					strcat(curPath, "\\TraceAI.txt");

					CFile fp;
					fp.Open(curPath, CFile::modeWrite|CFile::modeCreate);
					fp.Write("", strlen ( "" ));
					fp.Close();
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TRACE_AI_ON)), int(RGB(255,255,100)), 0, 0);
				}
				else
				{
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_TRACE_AI_OFF)), int(RGB(255,255,100)), 0, 0);
				}
			}
			break;

		case TT_HOMUN_USERAI_ON_OFF:
			{
				g_session.m_onHoUserAI = !g_session.m_onHoUserAI ;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

				if (g_session.m_onHoUserAI)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_HOMUN_USERAI_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_HOMUN_USERAI_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
#endif
#if defined (_MER)
		case TT_MER_USERAI_ON_OFF:
			{
				g_session.m_onMerUserAI = !g_session.m_onMerUserAI ;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

				if (g_session.m_onMerUserAI)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MER_USERAI_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_MER_USERAI_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
#endif

		case TT_BINGBING_ON_OFF:
			{
				CGameActor *	mActor = NULL;
				mActor = m_world->m_player;

				mActor->m_roty+=45;
				if (mActor->m_roty>=360)
					mActor->m_roty-=360;


				SendMsg(MM_CHANGE_DIRECTION, DegToDir(mActor->m_roty),HEADERDIR_DEFAULT);
			}
			break;
		case TT_BANGBANG_ON_OFF:
			{
				CGameActor *	mActor = NULL;
				mActor = m_world->m_player;

				mActor->m_roty-=45;
				if (mActor->m_roty<0)
					mActor->m_roty+=360;


				SendMsg(MM_CHANGE_DIRECTION, DegToDir(mActor->m_roty),HEADERDIR_DEFAULT);
			}
			break;
		case TT_FIXED_CAMERA_ON_OFF:
			{
				g_isFixedCamera = 1 - g_isFixedCamera;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_isFixedCamera)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FIXED_CAMERA_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FIXED_CAMERA_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_VOLUME:
			{
				const char * param = talkContent->c_str();
				int volume = atoi(param);
				if (volume > 127)
					volume = 127;
				if (volume < 0)
					volume = 0;
				Set2DVolume(volume);
				Set3DVolume(volume);
				char msg[120];
				sprintf(msg, "%s %d", MsgStr(MSI_VOLUME_OF_SOUND), volume);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;
		case TT_BVOLUME:
			{
				const char * param = talkContent->c_str();
				int volume = atoi(param);
				if (volume > 127)
					volume = 127;
				if (volume < 0)
					volume = 0;
				g_session.m_BgmVolume = volume;
				SetStreamVolume(volume);
				char msg[120];
				sprintf(msg, "%s %d", MsgStr(MSI_VOLUME_OF_BGM), volume);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;
		case TT_EFFECT_SOUND_ON_OFF:
			{
				g_isSoundOn = 1 - g_isSoundOn;
				if (g_isSoundOn) g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SOUND_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SOUND_OFF)), int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;
		case TT_FRAMESKIP_ON_OFF:
			{
				g_frameskip = 1 - g_frameskip;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_frameskip)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FRAME_SKIP_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_FRAME_SKIP_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;
		case TT_MUSIC_ON_OFF:
			{
				g_session.m_bgmIsPaused = 1 - g_session.m_bgmIsPaused;
				if (!g_session.m_bgmIsPaused)
				{
					PlayStream("bgm\\08.mp3");
				}
				else
					CloseStream();
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_bgmIsPaused)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BGM_OFF)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_BGM_ON)), int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;

		case TT_ATTACK_SNAP_ON_OFF:
			{
				g_session.m_monsterSnapOn_NoSkill = 1 - g_session.m_monsterSnapOn_NoSkill;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_monsterSnapOn_NoSkill)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ATTACK_SNAP_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ATTACK_SNAP_OFF)), int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;

		case TT_ITEM_SNAP_ON_OFF:
			{
				g_session.m_isItemSnap = 1-g_session.m_isItemSnap;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.IsItemSnap())
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_SNAP_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_ITEM_SNAP_OFF)), int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;

		case TT_SKILL_SNAP_ON_OFF:
			{
				g_session.m_monsterSnapOn_Skill = 1-g_session.m_monsterSnapOn_Skill;
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_monsterSnapOn_Skill)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SKILL_SNAP_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SKILL_SNAP_OFF)), int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;

		case TT_STAT:
			{



			}
			break;
		case TT_DISCONNECT_CHARACTER:
			{
				SendMsg(MM_DISCONNECT_CHARACTER);
			}
			break;
		case TT_DISCONNECT_ALL_CHARACTER:
			{
				SendMsg(MM_DISCONNECT_ALL_CHARACTER);
			}
			break;
		case TT_REQ_STAND:
			{
				if (m_world->m_player != NULL)
					m_world->m_player->SendMsg(NULL, AM_REQ_STAND, 0, 0, 0);
			}
			break;
		case TT_REQ_SIT:
			{
				if (m_world->m_player != NULL)
					m_world->m_player->SendMsg(NULL, AM_REQ_SIT, 0, 0, 0);
			}
			break;
		case TT_HELP:
			{
#define PUSH_HELP_MSG(id) g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr((MSGSTRINGID)(id))), int(RGB(100,255,255)), 0, 0)
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);

				PUSH_HELP_MSG(0xb1);
				PUSH_HELP_MSG(0x21);
				PUSH_HELP_MSG(0x22);
				PUSH_HELP_MSG(0x23);
				PUSH_HELP_MSG(0x24);
				PUSH_HELP_MSG(0x25);
				PUSH_HELP_MSG(0x26);
				PUSH_HELP_MSG(0x27);
				PUSH_HELP_MSG(0x28);
				PUSH_HELP_MSG(0x29);
				PUSH_HELP_MSG(0x2a);
				PUSH_HELP_MSG(0x2b);
				PUSH_HELP_MSG(0x2c);
				PUSH_HELP_MSG(0x2d);
				PUSH_HELP_MSG(0x2e);
				PUSH_HELP_MSG(0x6b);
				PUSH_HELP_MSG(0x84);
				PUSH_HELP_MSG(0x85);
				PUSH_HELP_MSG(0x86);
				PUSH_HELP_MSG(0x87);
				PUSH_HELP_MSG(0x89);
				PUSH_HELP_MSG(0x8a);
				PUSH_HELP_MSG(0x8b);
				PUSH_HELP_MSG(0x30);
				PUSH_HELP_MSG(0x6c);
				PUSH_HELP_MSG(0x8c);
				PUSH_HELP_MSG(0x8d);
				PUSH_HELP_MSG(0x8e);
				PUSH_HELP_MSG(0x9a);
				PUSH_HELP_MSG(0x9b);
				PUSH_HELP_MSG(0x9c);
				PUSH_HELP_MSG(0x9d);
				PUSH_HELP_MSG(0x9e);
				PUSH_HELP_MSG(0xad);
				PUSH_HELP_MSG(0xd3);
				PUSH_HELP_MSG(0x113);
				PUSH_HELP_MSG(0x139);
				PUSH_HELP_MSG(0x2f5);
				PUSH_HELP_MSG(0x141);
				PUSH_HELP_MSG(0x15d);
				PUSH_HELP_MSG(0x2e0);
				PUSH_HELP_MSG(0x354);
				PUSH_HELP_MSG(0x38b);
				PUSH_HELP_MSG(0x2d7);
				PUSH_HELP_MSG(0x2d8);
				PUSH_HELP_MSG(0x2c6);
				PUSH_HELP_MSG(0x2cb);
				PUSH_HELP_MSG(0x303);
				PUSH_HELP_MSG(0x2df);
				PUSH_HELP_MSG(0x2d4);
				PUSH_HELP_MSG(0x2ff);
				PUSH_HELP_MSG(0x310);
				PUSH_HELP_MSG(0x396);
				PUSH_HELP_MSG(0x324);
				PUSH_HELP_MSG(0x313);
				PUSH_HELP_MSG(0x314);
				PUSH_HELP_MSG(0x31b);
				PUSH_HELP_MSG(0x31c);
				PUSH_HELP_MSG(0x39c);
				PUSH_HELP_MSG(0x39b);
				PUSH_HELP_MSG(0x351);
				PUSH_HELP_MSG(0x2d9);
				PUSH_HELP_MSG(0x2dc);
				PUSH_HELP_MSG(0x2bc);
				PUSH_HELP_MSG(0x2cc);
				PUSH_HELP_MSG(0x302);
				PUSH_HELP_MSG(0x413);
				PUSH_HELP_MSG(0x453);
				PUSH_HELP_MSG(0x39d);
				PUSH_HELP_MSG(0x4a7);

				if (random(10) < 4) {
					PUSH_HELP_MSG(0x399);
					PUSH_HELP_MSG(0x39a);
				} else if (random(5) < 2) {
					PUSH_HELP_MSG(0x2f8);
				} else if (random(3) < 2) {
					PUSH_HELP_MSG(0x315);
				}
#undef PUSH_HELP_MSG
			}
			break;
		case TT_MAKE_CHATROOMMAKEWND:
			{
				if (!g_windowMgr.GetMerchantShopMakeWnd() && !g_windowMgr.GetMerchantItemMyShopWnd()) {
					g_windowMgr.MakeWindow(WID_CHATROOMMAKEWND);
				} else if (g_windowMgr.GetMerchantItemMyShopWnd()) {
					g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_OPEN_STORE_WHILE_CHAT), ME_OK, TRUE, 0, 0);
				} else {
					SendMsg(MM_REQ_OPENSTORE);
				}
			}
			break;
		case TT_REQ_EXIT_ROOM:
			{
				SendMsg(MM_REQ_EXIT_ROOM);
			}
			break;
		case TT_REQ_BREAK_GUILD:
			{
				SendMsg(MM_REQ_DISORGANIZE_GUILD, (int)talkContent->c_str());
			}
			break;
		case TT_REQ_MAKE_GUILD:
			{
				SendMsg(MM_REQ_MAKE_GUILD, (int)talkContent->c_str());
			}
			break;
		case TT_REQ_MAKE_GROUP:
			{
				SendMsg(MM_MAKE_GROUP, (int)talkContent->c_str());
			}
			break;

		case TT_REQ_ITEM_CREATE:
			SendMsg(MM_ITEM_CREATE, (int)talkContent[0].c_str());
			break;
		case TT_REQ_MOVETO_MAP:
			SendMsg(MM_MOVETO_MAP, (int)talkContent[0].c_str(), atoi(talkContent[1].c_str()), atoi(talkContent[2].c_str()));
			break;
		case	TT_REMOVE:
			SendMsg(MM_REMOVE_TO_AID, (int)talkContent[0].c_str());
			break;
		case	TT_SHIFT:
			SendMsg(MM_SHIFT_TO_GID, (int)talkContent[0].c_str());
			break;
		case	TT_RECALL_AID:
			SendMsg(MM_RECALL_AID, (int)talkContent[0].c_str());
			break;
		case	TT_RECALL_GID:
			SendMsg(MM_RECALL_GID, (int)talkContent[0].c_str());
			break;
		case	TT_CHECK_ID:
			SendMsg(MM_CHECK_ID, (int)talkContent[0].c_str());
			break;
		case	TT_RC_ID:
			SendMsg(MM_RC_ID, (int)talkContent[0].c_str());
			break;
		case	TT_BILLING_INFO:
			SendMsg(MM_BILLING_INFO, (int)talkContent[0].c_str());
			break;
		case	TT_SAVEMONEY:
			SendMsg(MM_GUILD_MONEY, atoi(talkContent[0].c_str()));
			break;
		case	TT_LOADMONEY:
			{
				int money = atoi(talkContent[0].c_str());
				money *= -1;
				SendMsg(MM_GUILD_MONEY, money);
			}
			break;
		case TT_REQ_LEAVE_GROUP:
			{
				SendMsg(MM_REQ_LEAVE_GROUP);




			}
			break;
		case	TT_REQ_PK:
			{
				string param = talkContent->c_str();
				MakeStringLower(param);
				if (!(param == "off" || param == "on")) {
					break;
				}
				int on = (param == "off")?0:1;
				SendMsg(MM_PKMODE_CHANGE, on);
			}
			break;
		case	TT_REQ_EXPEL_GROUP_MEMBER:
			{
				SendMsg(MM_REQ_EXPEL_GROUP_MEMBER, (int)talkContent->c_str());
			}
			break;
		case TT_REQ_EXCHANGE_ITEM:
			{
				const char * param = talkContent->c_str();
				const char * cName = param;
				DWORD aid = GetAid(cName);
				if (aid)
					SendMsg(MM_REQ_EXCHANGE_ITEM, aid);
			}
			break;
		case	TT_FONT:
			{
				if (m_nameBalloonType == NAME_BALLOON_BOTTOM) {
					m_nameBalloonType = NAME_BALLOON_TOP;
				} else {
					m_nameBalloonType = NAME_BALLOON_BOTTOM;
				}
			}
			break;
		case TT_REQ_EMOTION_SURPRISE:
			{
				SendMsg(MM_REQ_EMOTION, ET_SURPRISE);
			}
			break;
		case TT_REQ_EMOTION_QUESTION:
			{
				SendMsg(MM_REQ_EMOTION, ET_QUESTION);
			}
			break;
		case TT_REQ_EMOTION_DELIGHT:
			{
				SendMsg(MM_REQ_EMOTION, ET_DELIGHT);
			}
			break;
		case TT_REQ_EMOTION_THROB:
			{
				SendMsg(MM_REQ_EMOTION, ET_THROB);
			}
			break;
		case TT_REQ_EMOTION_SWEAT:
			{
				SendMsg(MM_REQ_EMOTION, ET_SWEAT);
			}
			break;
		case TT_REMEMBER_WARPPOINT:
			{
				SendMsg(MM_REMEMBER_WARPPOINT);
			}
			break;
		case TT_OPEN_TIP:
			{
				if (!g_windowMgr.DeleteWindow(WID_TIPOFTHEDAYWND))
					g_windowMgr.MakeWindow(WID_TIPOFTHEDAYWND);
			}
			break;
		case TT_REQ_EMOTION_AHA:
			{
				SendMsg(MM_REQ_EMOTION, ET_AHA);
			}
			break;
		case TT_REQ_EMOTION_FRET:
			{
				SendMsg(MM_REQ_EMOTION, ET_FRET);
			}
			break;
		case TT_REQ_EMOTION_ANGER:
			{
				SendMsg(MM_REQ_EMOTION, ET_ANGER);
			}
			break;
		case TT_REQ_EMOTION_MONEY:
			{
				SendMsg(MM_REQ_EMOTION, ET_MONEY);
			}
			break;
		case TT_REQ_EMOTION_THINK:
			{
				SendMsg(MM_REQ_EMOTION, ET_THINK);
			}
			break;
		case TT_REQ_EMOTION_SCISSOR:
			{
				SendMsg(MM_REQ_EMOTION, ET_SCISSOR);
			}
			break;
		case TT_REQ_EMOTION_ROCK:
			{
				SendMsg(MM_REQ_EMOTION, ET_ROCK);
			}
			break;
		case TT_REQ_EMOTION_WRAP:
			{
				SendMsg(MM_REQ_EMOTION, ET_WRAP);
			}
			break;
#if (_PACKET_VER >= 4)
		case TT_REQ_EMOTION_BIGTHROB:
			{
				SendMsg(MM_REQ_EMOTION, ET_BIGTHROB);
			}
			break;
		case TT_REQ_EMOTION_THANKS:
			{
				SendMsg(MM_REQ_EMOTION, ET_THANKS);
			}
			break;
		case TT_REQ_EMOTION_KEK:
			{
				SendMsg(MM_REQ_EMOTION, ET_KEK);
			}
			break;
		case TT_REQ_EMOTION_SORRY:
			{
				SendMsg(MM_REQ_EMOTION, ET_SORRY);
			}
			break;
		case TT_REQ_EMOTION_SMILE:
			{
				SendMsg(MM_REQ_EMOTION, ET_SMILE);
			}
			break;
		case TT_REQ_EMOTION_PROFUSELY_SWEAT:
			{
				SendMsg(MM_REQ_EMOTION, ET_PROFUSELY_SWEAT);
			}
			break;
		case TT_REQ_EMOTION_SCRATCH:
			{
				SendMsg(MM_REQ_EMOTION, ET_SCRATCH);
			}
			break;
		case TT_REQ_EMOTION_BEST:
			{
				SendMsg(MM_REQ_EMOTION, ET_BEST);
			}
			break;
		case TT_REQ_EMOTION_STARE_ABOUT:
			{
				SendMsg(MM_REQ_EMOTION, ET_STARE_ABOUT);
			}
			break;
		case TT_REQ_EMOTION_HUK:
			{
				SendMsg(MM_REQ_EMOTION, ET_HUK);
			}
			break;
		case TT_REQ_EMOTION_O:
			{
				SendMsg(MM_REQ_EMOTION, ET_O);
			}
			break;
		case TT_REQ_EMOTION_X:
			{
				SendMsg(MM_REQ_EMOTION, ET_X);
			}
			break;
		case TT_REQ_EMOTION_HELP:
			{
				SendMsg(MM_REQ_EMOTION, ET_HELP);
			}
			break;
		case TT_REQ_EMOTION_GO:
			{
				SendMsg(MM_REQ_EMOTION, ET_GO);
			}
			break;
		case	TT_REQ_EMOTION_CRY:
			{
				SendMsg(MM_REQ_EMOTION, ET_CRY);
			}
			break;
		case	TT_REQ_EMOTION_KIK:
			{
				SendMsg(MM_REQ_EMOTION, ET_KIK);
			}
			break;
		case	TT_REQ_EMOTION_CHUP:
			{
				SendMsg(MM_REQ_EMOTION, ET_CHUP);
			}
			break;
		case	TT_REQ_EMOTION_CHUPCHUP:
			{
				SendMsg(MM_REQ_EMOTION, ET_CHUPCHUP);
			}
			break;
		case	TT_REQ_EMOTION_HNG:
			{
				SendMsg(MM_REQ_EMOTION, ET_HNG);
			}
			break;
		case	TT_REQ_EMOTION_OK:
			{
				SendMsg(MM_REQ_EMOTION, ET_OK);
			}
			break;

		case	TT_REQ_EMOTION_STARE:
			{
				SendMsg(MM_REQ_EMOTION, ET_STARE);
			}
			break;
		case	TT_REQ_EMOTION_HUNGRY:
			{
				SendMsg(MM_REQ_EMOTION, ET_HUNGRY);
			}
			break;
		case	TT_REQ_EMOTION_COOL:
			{
				SendMsg(MM_REQ_EMOTION, ET_COOL);
			}
			break;
		case	TT_REQ_EMOTION_MERONG:
			{
				SendMsg(MM_REQ_EMOTION, ET_MERONG);
			}
			break;
		case	TT_REQ_EMOTION_SHY:
			{
				SendMsg(MM_REQ_EMOTION, ET_SHY);
			}
			break;
		case	TT_REQ_EMOTION_GOODBOY:
			{
				SendMsg(MM_REQ_EMOTION, ET_GOODBOY);
			}
			break;
		case	TT_REQ_EMOTION_SPTIME:
			{
				SendMsg(MM_REQ_EMOTION, ET_SPTIME);
			}
			break;
		case	TT_REQ_EMOTION_SEXY:
			{
				SendMsg(MM_REQ_EMOTION, ET_SEXY);
			}
			break;
		case	TT_REQ_EMOTION_COMEON:
			{
				SendMsg(MM_REQ_EMOTION, ET_COMEON);
			}
			break;
		case	TT_REQ_EMOTION_SLEEPY:
			{
				SendMsg(MM_REQ_EMOTION, ET_SLEEPY);
			}
			break;
		case	TT_REQ_EMOTION_CONGRATULATION:
			{
				SendMsg(MM_REQ_EMOTION, ET_CONGRATULATION);
			}
			break;
		case	TT_REQ_EMOTION_HPTIME:
			{
				SendMsg(MM_REQ_EMOTION, ET_HPTIME);
			}
			break;

		case	TT_REQ_EMOTION_SPARK:
			{
				SendMsg(MM_REQ_EMOTION, ET_SPARK);
			}
			break;
		case	TT_REQ_EMOTION_CONFUSE:
			{
				SendMsg(MM_REQ_EMOTION, ET_CONFUSE);
			}
			break;
		case	TT_REQ_EMOTION_OHNO:
			{
				SendMsg(MM_REQ_EMOTION, ET_OHNO);
			}
			break;
		case	TT_REQ_EMOTION_HUM:
			{
				SendMsg(MM_REQ_EMOTION, ET_HUM);
			}
			break;
		case	TT_REQ_EMOTION_BLABLA:
			{
				SendMsg(MM_REQ_EMOTION, ET_BLABLA);
			}
			break;
		case	TT_REQ_EMOTION_OTL:
			{
				SendMsg(MM_REQ_EMOTION, ET_OTL);
			}
			break;
#if !defined(_THAI)
		case	TT_REQ_EMOTION_DICE:
			{
				SendMsg(MM_REQ_EMOTION, ET_DICE1+random(6) );
			}
			break;
#endif
		case	TT_REQ_EMOTION_LUV:
			{
				SendMsg(MM_REQ_EMOTION, ET_LUV);
			}
			break;
		case	TT_REQ_EMOTION_MOBILE:
			{
				SendMsg(MM_REQ_EMOTION, ET_MOBILE);
			}
			break;
		case	TT_REQ_EMOTION_MAIL:
			{
				SendMsg(MM_REQ_EMOTION, ET_MAIL);
			}
			break;
		case	TT_REQ_EMOTION_ANTENNA0:
			{
				SendMsg(MM_REQ_EMOTION, ET_ANTENNA0);
			}
			break;
		case	TT_REQ_EMOTION_ANTENNA1:
			{
				SendMsg(MM_REQ_EMOTION, ET_ANTENNA1);
			}
			break;
		case	TT_REQ_EMOTION_ANTENNA2:
			{
				SendMsg(MM_REQ_EMOTION, ET_ANTENNA2);
			}
			break;
		case	TT_REQ_EMOTION_ANTENNA3:
			{
				SendMsg(MM_REQ_EMOTION, ET_ANTENNA3);
			}
			break;

		case	TT_REQ_EMOTION_HUM2:
			{
				SendMsg(MM_REQ_EMOTION, ET_HUM2);
			}
			break;
		case	TT_REQ_EMOTION_ABS:
			{
				SendMsg(MM_REQ_EMOTION, ET_ABS);
			}
			break;
		case	TT_REQ_EMOTION_OOPS:
			{
				SendMsg(MM_REQ_EMOTION, ET_OOPS);
			}
			break;
		case	TT_REQ_EMOTION_SPIT:
			{
				SendMsg(MM_REQ_EMOTION, ET_SPIT);
			}
			break;
		case	TT_REQ_EMOTION_ENE:
			{
				SendMsg(MM_REQ_EMOTION, ET_ENE);
			}
			break;
		case	TT_REQ_EMOTION_PANIC:
			{
				SendMsg(MM_REQ_EMOTION, ET_PANIC);
			}
			break;
		case	TT_REQ_EMOTION_WHISP:
			{
				SendMsg(MM_REQ_EMOTION, ET_WHISP);
			}
			break;

		case	TT_SHOW_EMOTIONLIST:
			{
#define PUSH_EMOTION_HELP_MSG(id) g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr((MSGSTRINGID)(id))), int(RGB(100,255,255)), 0, 0)
				PUSH_EMOTION_HELP_MSG(0x25d);
				PUSH_EMOTION_HELP_MSG(0x224);
				PUSH_EMOTION_HELP_MSG(0x22b);
				PUSH_EMOTION_HELP_MSG(0x22c);
				PUSH_EMOTION_HELP_MSG(0x22d);
				PUSH_EMOTION_HELP_MSG(0x22e);
				PUSH_EMOTION_HELP_MSG(0x22f);
				PUSH_EMOTION_HELP_MSG(0x230);
				PUSH_EMOTION_HELP_MSG(0x231);
				PUSH_EMOTION_HELP_MSG(0x232);
				PUSH_EMOTION_HELP_MSG(0x233);
				PUSH_EMOTION_HELP_MSG(0x234);
				PUSH_EMOTION_HELP_MSG(0x235);
				PUSH_EMOTION_HELP_MSG(0x236);
				PUSH_EMOTION_HELP_MSG(0x237);
				PUSH_EMOTION_HELP_MSG(0x238);
				PUSH_EMOTION_HELP_MSG(0x239);
				PUSH_EMOTION_HELP_MSG(0x23a);
				PUSH_EMOTION_HELP_MSG(0x23b);
				PUSH_EMOTION_HELP_MSG(0x23c);
				PUSH_EMOTION_HELP_MSG(0x23d);
				PUSH_EMOTION_HELP_MSG(0x304);
				PUSH_EMOTION_HELP_MSG(0x305);
				PUSH_EMOTION_HELP_MSG(0x306);
				PUSH_EMOTION_HELP_MSG(0x307);
				PUSH_EMOTION_HELP_MSG(0x308);
				PUSH_EMOTION_HELP_MSG(0x309);
				PUSH_EMOTION_HELP_MSG(0x30a);
				PUSH_EMOTION_HELP_MSG(0x30b);
				PUSH_EMOTION_HELP_MSG(0x30c);
				PUSH_EMOTION_HELP_MSG(0x30d);
				PUSH_EMOTION_HELP_MSG(0x30e);
				PUSH_EMOTION_HELP_MSG(0x30f);
#undef PUSH_EMOTION_HELP_MSG
			}
			break;
#endif
#if defined(_DEBUG) || defined(LOCAL_SERVER)
		case TT_TEST_SHOW_IMAGE:
			{
				string param = talkContent->c_str();
				SendMsg(MM_SHOW_IMAGE, int(param.c_str()), random(3));
			}
			break;
		case TT_TOGGLE_OBJLIGHT:
			break;
		case	TT_TESTPACKET:
			{

				SendMsg(MM_QUAKE);
				int sx, sy;
				GetServerCoor(m_world->m_player->m_pos.x, m_world->m_player->m_pos.z, sx, sy);
				m_world->m_ground->SetGndRiseAmount(-25, sx/2, sy/2);

				CGndRiser * gndRiser = new CGndRiser;
				gndRiser->SendMsg(NULL, AM_SETPOS, int(&(m_world->m_player->m_pos)));
				GetWorld()->m_gameObjectList.push_back(gndRiser);
				break;
			}
#endif
		case TT_FOG:
			{
				g_session.m_fogOn = 1 - g_session.m_fogOn;
				MakeFog(g_session.m_fogOn);
				if (g_session.m_fogOn)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"Fog On", int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"Fog Off", int(RGB(255,255,100)), 0, 0);
				UIOptionWnd * optionWnd = g_windowMgr.GetOptionWnd();
				if (optionWnd)
					optionWnd->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			}
			break;
		case TT_REQ_RESETSTATE:
			SendMsg(MM_RESET_PARAMETER, 0);
			break;
		case TT_REQ_RESETSKILL:
			SendMsg(MM_RESET_PARAMETER, 1);
			break;
		case TT_CHANGE_MAPTYPE:
			SendMsg(MM_REQ_CHANGE_MAPTYPE, atoi(talkContent[0].c_str()), atoi(talkContent[1].c_str()), atoi(talkContent[2].c_str()));
			break;
		case TT_TOGGLE_EMBLEM:
			if(g_session.m_emblemState)
			{
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"Guild Emblem Off", 0xffff, 0, 0);
				g_session.m_emblemState = false;
			}
			else
			{
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"Guild Emblem On", 0xffff, 0, 0);
				g_session.m_emblemState = true;
			}
			break;
		case TT_HIDE:
			{
				DWORD effectState = g_session.m_effectState;

				if(IsEffectStateHiding(effectState))
					ClearEffectStateSpecialHiding(&effectState);
				else
					SetEffectStateSpecialHiding(&effectState);

				SendMsg(MM_CHANGE_EFFECTSTATE, EFFECTSTATE_SPECIALHIDING);
			}
			break;
		case	TT_NAME_BALLOON:
			if (++m_nameBalloonType == 4)
				m_nameBalloonType = 0;
			break;
		case	TT_SHOW_NAME_TYPE:
			{
				g_session.m_isShowNameTypeOld = !g_session.m_isShowNameTypeOld;
				g_session.SetTextType(g_session.m_isShowNameTypeOld, g_session.m_isShowNameTypeOld);
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (g_session.m_isShowNameTypeOld)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SHOWNAME_ON)), int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_SHOWNAME_OFF)), int(RGB(255,255,100)), 0, 0);
			}
			break;





		case	TT_LIGHTMAP:
			g_isLightMap = !g_isLightMap;
			g_Weather.SetObjLight(g_isLightMap == 1);
			break;

		case TT_REQ_HUNTING:
#if defined (_HUNTING_TIME_LIST)
			SendMsg(MM_REQ_HUNTING);
#endif
			break;


		case TT_COPY_NAME:
			if (IsGravityAid(g_session.GetAid()))
			{
				m_onCopyName = !m_onCopyName;
				if (m_onCopyName)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"Copy_Name_On!!", int(RGB(255,255,100)), 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)"Copy_Name_Off!!", int(RGB(255,255,100)), 0, 0);
			}
			break;

#ifdef	__PARTY_INVITE
		case TT_REQ_REFUSE_INVITE_PARTY:
		case TT_REQ_ACCEPT_INVITE_PARTY:
			{
				PACKET_CZ_PARTY_CONFIG p ;
				p.PacketType = HEADER_CZ_PARTY_CONFIG ;
				p.bRefuseJoinMsg = (TT_REQ_ACCEPT_INVITE_PARTY - talkType) ? true : false ;
				g_connection.SendPacket(g_connection.GetPacketSize(p.PacketType), (const char *)&p);
			}
			break;

		case TT_REQ_INVITE_PARTY:
			{
				SendMsg(MM_REQ_JOIN_GROUP, (int)talkContent->c_str());
			}
			break;
#endif

#ifdef	BIND_ON_EQUIP
		case TT_REQ_OPEN_EQUIPEDITEM_ONOFF:
			{
				SendMsg(MM_OPEN_EQUIPEDITEM_CHANGE, !g_session.m_bOpenEquipedItem);
			}
			break;
#endif

		case TT_UNKNOWN:
			{
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(MsgStr(MSI_INVALID_COMMAND)), 0xffff, 0, 0);
			}
			break;
	}
}

void CGameMode::ProcessAutoFollow()
{
	if (m_lastLockOnPcGid == 0)
		return;

	EBtnState lBtn;
	lBtn = g_mouse.GetLBtn();

	if (lBtn != BTN_NONE) {
		m_lastLockOnPcGid = 0;
		m_dwOldDisappearTime = 0;
		m_dwOldAutoFollowTime = 0;
		return;
	}

	CGameActor *actor = GetWorld()->GetGameActorByAID(m_lastLockOnPcGid);
	if (actor == NULL) {
		if (m_dwOldDisappearTime == 0)
			m_dwOldDisappearTime = timeGetTime();

		if (timeGetTime() - m_dwOldDisappearTime > 10000) {
			m_lastLockOnPcGid = 0;
			m_dwOldAutoFollowTime = 0;
			m_dwOldDisappearTime = 0;
		}
		return;
	}

	m_dwOldDisappearTime = 0;

	if (timeGetTime() - m_dwOldAutoFollowTime > 1000) {
		m_dwOldAutoFollowTime = timeGetTime();

		int dx = actor->m_moveDestX;
		int dy = actor->m_moveDestY;
		if (g_windowMgr.GetChatRoomWnd() || m_world->m_player->m_stateId == 6) {
			m_world->m_player->SendMsg(NULL, AM_REQ_CHANGE_DIR, dx, dy, 0);
		} else {
			if (!IsValidCell(dx, dy)) {
				if (!GetClosestValidCell(dx, dy))
					return;
			}
			if (g_windowMgr.GetMenu())
				g_windowMgr.DeleteWindow(WID_MENUWND);
			m_world->m_player->SendMsg(NULL, AM_REQ_MOVE, dx, dy, 0);
		}
	}
}

void	CGameMode :: MakeEmblemWnd(CGameActor *theActor, int gdid, int emblemVer)
{
	if (gdid == 0)
		return;
	if (theActor->m_emblemWnd != NULL)
		return;
	if (IsGravityAid(theActor->GetGID()))
		return;
	if (!IsGravityAid(g_session.GetAid()))
	{
		if (IsEffectStateHiding(theActor->GetEffectState()))
			return;
		if (IsEffectStateBurrow(theActor->GetEffectState()))
			return;
		if (IsEffectStateSpecialHiding(theActor->GetEffectState()))
			return;
	}

	UIEmblemWnd * emblemWnd = new UIEmblemWnd;
	emblemWnd->SendMsg(NULL, UM_SETINFO, theActor->GetGID(), gdid, emblemVer, 0);
	g_windowMgr.AddWindow(emblemWnd);
	theActor->m_emblemWnd = (char *)emblemWnd;
}

void	CGameMode :: MakeWordDisplayWnd(CGameActor *theActor)
{
	if (theActor->m_WordDisplayWnd != NULL)
		return;
	if (IsGravityAid(theActor->GetGID()))
		return;
	if (!IsGravityAid(g_session.GetAid()))
	{
		if (IsEffectStateHiding(theActor->GetEffectState()))
			return;
		if (IsEffectStateBurrow(theActor->GetEffectState()))
			return;
		if (IsEffectStateSpecialHiding(theActor->GetEffectState()))
			return;
	}

	UIWordDisplayWnd * wordWnd = new UIWordDisplayWnd;
	wordWnd->SendMsg(NULL, UM_SETINFO, theActor->GetGID(), 0, 0, 0);
	g_windowMgr.AddWindow(wordWnd);
	theActor->m_WordDisplayWnd = (char *)wordWnd;
}

void CGameMode::MakePotionEffect(int itemId, DWORD aid)
{
	char msg[256];
	int efId;
	CGameActor *actor;

	if (aid == g_session.GetAid()) {
		actor = m_world->m_player;
		if (!actor)
			return;

		switch (itemId) {
		case 12208:
			sprintf(msg, MsgStr(MSI_PLUSEXP));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 7, 0);
			break;
		case 12312:
			sprintf(msg, MsgStr(MSI_PLUSEXP12312));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 7, 0);
			break;
		case 14532:
			sprintf(msg, MsgStr(MSI_PLUSEXP14532));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 7, 0);
			break;
		case 14533:
			sprintf(msg, MsgStr(MSI_PLUSEXP14533));
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffff00, 7, 0);
			break;
		}
	} else {
		actor = m_world->GetGameActorByAID(aid);
	}

	if (!actor)
		return;

	efId = -1;
	switch (itemId) {
	case 645:
	case 14509:
		efId = EF_POTION_CON;
		break;
	case 656:
	case 14510:
		efId = EF_POTION_;
		break;
	case 657:
	case 14511:
		efId = EF_POTION_BERSERK;
		break;
	case 501:
	case 598:
	case 507:
	case 545:
	case 569:
	case 578:
		efId = EF_POTION1;
		break;
	case 502:
	case 599:
	case 579:
		efId = EF_POTION2;
		break;
	case 503:
	case 508:
	case 546:
	case 11500:
		efId = EF_POTION3;
		break;
	case 504:
	case 509:
	case 547:
	case 555:
	case 556:
	case 557:
	case 11501:
	case 11503:
		efId = EF_POTION4;
		break;
	case 505:
	case 510:
	case 11502:
	case 11504:
		efId = EF_POTION5;
		break;
	case 506:
	case 511:
		efId = EF_POTION6;
		break;
	case 512:
	case 513:
	case 515:
	case 516:
	case 531:
	case 532:
	case 534:
	case 535:
	case 548:
	case 549:
	case 550:
	case 682:
	case 582:
		efId = EF_POTION7;
		break;
	case 514:
	case 533:
	case 568:
	case 607:
	case 680:
	case 683:
		efId = EF_POTION8;
		break;
	case 517:
	case 518:
	case 519:
	case 520:
	case 521:
	case 526:
	case 528:
	case 529:
	case 530:
	case 536:
	case 537:
	case 538:
	case 539:
	case 540:
	case 541:
	case 542:
	case 543:
	case 544:
	case 551:
	case 552:
	case 553:
	case 562:
	case 563:
	case 564:
	case 565:
	case 566:
	case 567:
	case 570:
	case 571:
	case 572:
	case 575:
	case 576:
	case 577:
	case 580:
	case 581:
	case 583:
	case 584:
	case 585:
	case 586:
	case 663:
		efId = EF_EXIT;
		break;
	case 558:
	case 561:
	case 12237:
		efId = EF_VALLENTINE2;
		break;
	case 559:
	case 560:
	case 11701:
		efId = EF_VALLENTINE;
		break;
	case 684:
		efId = EF_POTION_BERSERK;
		actor->LaunchEffect(EF_GUMGANG2);
		break;
	case 554:
	case 12122:
	case 12123:
	case 12124:
	case 12195:
	case 12196:
	case 12197:
	case 12198:
	case 12238:
	case 12239:
		PlayWave("_heal_effect.wav");
		efId = EF_MOCHI;
		break;
	case 685:
		efId = EF_LAMADAN;
		break;
	case 522:
		PlayWave("_heal_effect.wav");
		break;
	case 523:
		PlayWave("effect\\EF_Blessing.wav");
		break;
	case 662:
	case 12262:
		PlayWave("effect\\mapae.wav");
		efId = EF_MAPAE;
		break;
	case 12018:
		PlayWave("effect\\itempokjuk.wav");
		efId = EF_ITEMPOKJUK;
		break;
	case 12016:
		PlayWave("effect\\fast.wav");
		efId = EF_ITEMFAST;
		break;
	case 12017:
		efId = EF_DECAGILITY;
		break;
	case 573:
	case 596:
	case 597:
	case 12234:
		efId = EF_05VAL;
		break;
	case 574:
		efId = EF_POTION7;
		break;
	case 12023:
		PlayWave("effect\\ro2effect.wav");
		efId = EF_RO2YEAR;
		break;
	case 12027:
		PlayWave("wraith_die.wav");
		return;
	case 12028:
		efId = EF_ITEM_THUNDER;
		break;
	case 12029:
		efId = EF_ITEM_CLOUD;
		break;
	case 12030:
		efId = EF_ITEM_CURSE;
		break;
	case 12031:
		efId = EF_ITEM_ZZZ;
		break;
	case 12032:
		efId = EF_ITEM_RAIN;
		break;
	case 12033:
		efId = EF_ITEM_LIGHT;
		break;
	case 12034:
		PlayWave("ork_warrior_breath.wav");
		return;
	case 12118:
		efId = EF_FIREHIT2;
		PlayWave("evil_cloud_hermit_attack.wav");
		break;
	case 12119:
		efId = EF_COLDHIT;
		PlayWave("evil_cloud_hermit_attack.wav");
		break;
	case 12120:
		efId = EF_EARTHHIT;
		PlayWave("evil_cloud_hermit_attack.wav");
		break;
	case 12121:
		efId = EF_WINDHIT;
		PlayWave("evil_cloud_hermit_attack.wav");
		break;
	case 12131:
		efId = EF_HAPGYEOK;
		break;
	case 12928:
		efId = EF_ASPERSIO;
		break;
	case 12310:
	case 12311:
		efId = EF_FLOWERLEAF;
		break;
	case 12250:
		efId = EF_FOOD01;
		break;
	case 12251:
		efId = EF_FOOD04;
		break;
	case 12252:
		efId = EF_FOOD02;
		break;
	case 12253:
		efId = EF_FOOD05;
		break;
	case 12254:
		efId = EF_FOOD06;
		break;
	case 12255:
		efId = EF_FOOD03;
		break;
	case 14546:
		efId = EF_POK_LOVE;
		break;
	case 14547:
		efId = EF_POK_WHITE;
		break;
	case 14548:
		efId = EF_POK_VALEN;
		break;
	case 14549:
		efId = EF_POK_BIRTH;
		break;
	case 14550:
		efId = EF_POK_CHRISTMAS;
		break;
	}

	if ((itemId >= 12041 && itemId <= 12045) || (itemId >= 12071 && itemId <= 12075) || itemId == 12202)
		efId = EF_FOOD01;
	else if ((itemId >= 12046 && itemId <= 12050) || (itemId >= 12076 && itemId <= 12080) || itemId == 12204)
		efId = EF_FOOD02;
	else if ((itemId >= 12051 && itemId <= 12055) || (itemId >= 12081 && itemId <= 12085) || itemId == 12207)
		efId = EF_FOOD03;
	else if ((itemId >= 12056 && itemId <= 12060) || (itemId >= 12086 && itemId <= 12090) || itemId == 12203)
		efId = EF_FOOD04;
	else if ((itemId >= 12061 && itemId <= 12065) || (itemId >= 12091 && itemId <= 12095) || itemId == 12205)
		efId = EF_FOOD05;
	else if ((itemId >= 12066 && itemId <= 12070) || (itemId >= 12096 && itemId <= 12100) || itemId == 12206)
		efId = EF_FOOD06;

	if (efId > 0) {
		while (actor->IsEffectExist(efId))
			actor->SendMsg(NULL, AM_DELETE_EFFECT, efId);
		actor->LaunchEffect(efId);
	}
}


void	CGameMode :: ProcessPlayWave()
{
	std::vector<PLAY_WAVE_INFO>::iterator i = m_playWaveList.begin();
	while (i != m_playWaveList.end()) {
		PLAY_WAVE_INFO *info = &(*i);
		DWORD debugTime = timeGetTime();
		if (timeGetTime() >= info->endTick) {
			if (info->nAID) {
				CGameActor *theActor = GetWorld()->GetGameActorByAID(info->nAID);
				if (theActor) {
					vector3d & pPos = theActor->m_pos;
					PlayWave(info->wavName.c_str(), GetWorld()->m_player->m_pos.x - pPos.x, 0, GetWorld()->m_player->m_pos.z - pPos.z);
				}
			} else {
				PlayWave(info->wavName.c_str());
			}
			info->endTick = info->term + timeGetTime();
		}
		i++;
	}
}

CFile saveChatFp;
char autosaveFileName[256];
void	CGameMode :: WriteChat(char *txt)
{
	if (g_session.IsMasterAid(7)) {
		m_autoSaveChatCnt++;
		CFile fp;
		int	size;
		char *buf;

		fp.Open(autosaveFileName, CFile::modeRead);
		size = fp.GetLength();
		buf = new char[size];
		fp.Read(buf, size);
		fp.Close();

		saveChatFp.Open(autosaveFileName, CFile::modeCreate|CFile::modeWrite);
		saveChatFp.Write(buf, size);

		saveChatFp.Write(txt, strlen(txt) + 1);
		saveChatFp.Write(endchar, 2);
		saveChatFp.Close();
		delete [] buf;
	}
}
void	CGameMode :: MakeChatFile()
{
	if (g_session.IsMasterAid(7)) {
		m_autoSaveChatCnt = 0;
		char	tmp[2048];
		int	captureCnt = 0;
		sprintf(tmp, "%s\\Chat", g_baseDir3);
		if (access(tmp, 0)) {
			if (_mkdir(tmp))
				return;
		}
		sprintf(autosaveFileName, "%s\\Chat\\Chat.txt", g_baseDir3);
		while (access(autosaveFileName, 0)==0)
			sprintf(autosaveFileName, "%s\\Chat\\Chat%03d.txt", g_baseDir3, ++captureCnt);
	}
}

void	CGameMode :: CloseChatFile()
{
	if (g_session.m_isSaveChat && g_session.IsMasterAid(7)) {
		char tmp[256], tmp2[256];
		sprintf(tmp, "%s", saveChatFp.GetFileName());
		sprintf(tmp2, "%s.bak", tmp);
		saveChatFp.Close();
	}
}

void	CGameMode :: SetAutoFollowTime(DWORD oldFollowTime, DWORD oldDisappearTime)
{
	if (oldFollowTime != -1) {
		m_dwOldAutoFollowTime = oldFollowTime;
	}

	if (oldDisappearTime != -1) {
		m_dwOldDisappearTime = oldDisappearTime;
	}
}

void	CGameMode :: ProcessAssignAID()
{
	int a, b, size = g_session.m_reqAccountInfo.size();
	if (!size)
		return;

	for (a = 0; a < size; a++)
	{
		if (timeGetTime() - g_session.m_reqAccountInfo[a].reqTime > 1000)
		{
			SendMsg(MM_REQ_ACCOUNT_NAME, g_session.m_reqAccountInfo[a].AID);
			g_session.m_reqAccountInfo[a].reqTime = timeGetTime();
		}
	}

	size = g_session.m_chatProhibitLog.size();
	for (a = 0; a < size; a++) {
		for (b = 0; b < g_session.m_recievedAccountName.size(); b++) {
			if (g_session.m_recievedAccountName[b].AID == g_session.m_chatProhibitLog[a].AID) {
				strcpy(g_session.m_chatProhibitLog[a].accountName, g_session.m_recievedAccountName[b].accountName.c_str());
			}
		}
	}
}

bool	CGameMode::IsKillerAid(DWORD aid)
{
	return std::find(m_KillerList.begin(), m_KillerList.end(), aid) != m_KillerList.end();
}

bool CGameMode::HaveSiegfriedItem()
{
	ITEM_INFO itemInfo = g_session.GetItemInfoById(7621);

	if (m_world->IsPKZone() || m_world->IsSiegeMode() || m_world->IsEventPVPMode())
		return false;
	else if (itemInfo.m_itemIndex && itemInfo.m_num > 0)
		return true;
	else
		return false;
}

bool CGameMode::HaveBookItem(int itemId)
{
	ITEM_INFO itemInfo = g_session.GetItemInfoById(itemId);

	if (itemInfo.m_itemIndex && itemInfo.m_num > 0)
		return true;
	else
		return false;
}

void CGameMode::InsertScheduler() {}
void CGameMode::ProcessDamageSituation()
{
	char msg[100];
	std::list<HP_Damage_Moment>::iterator it = g_DM_List.begin();
	while (it != g_DM_List.end())
	{
		if (!(it->DamageTime > timeGetTime())) {
			CGameActor* actor = g_session.GetGameActorByAid(it->DamageChar);

			if (it->DamageChar == 0) {
				CGameActor* mainActor = g_session.GetGameActorByAid(1);
				if (mainActor) {
					if (it->DamageNum == -10) {
						bat_STUN_turn = 0;
						if (mainActor->GetBodyState() != 3) {
							mainActor->SetAttrState(3, mainActor->GetEffectState(), mainActor->GetHealthState(), true);
						}
					} else {
						vector3d v(0.0f, 0.0f, 0.0f);
						mainActor->LaunchEffect(it->DamageNum, v, 0.0f);
					}
				}
			} else if (it->DamageChar == 0x65) {
				CGameActor* a = g_session.GetGameActorByAid(0x64);
				if (a) {
					vector3d v(0.0f, 0.0f, 0.0f);
					a->LaunchEffect(it->DamageNum, v, 0.0f);
				}
			} else if (actor) {
				if (it->DamageNum < 0) {
					if (it->DamageNum == (int)0xffffdd48) {
						vector3d v(0.0f, 0.0f, 0.0f);
						actor->LaunchEffect(0xd0, v, 0.0f);
						actor->SendMsg(NULL, AM_MAKE_NUMBER, 0x190, 0xff0000ff, 'r');
						actor->m_Sp += 0x190;
						if (actor->m_Sp > actor->m_MaxSp) {
							actor->m_Sp = actor->m_MaxSp;
						}
					} else {
						if (-it->DamageNum >= 0xfa0) {
							vector3d v(0.0f, 0.0f, 0.0f);
							actor->LaunchEffect(0x145, v, 0.0f);
						} else if (-it->DamageNum >= 0x7d0) {
							vector3d v(0.0f, 0.0f, 0.0f);
							actor->LaunchEffect(0x139, v, 0.0f);
						} else if (-it->DamageNum >= 0xc8) {
							vector3d v(0.0f, 0.0f, 0.0f);
							actor->LaunchEffect(0x138, v, 0.0f);
						} else {
							vector3d v(0.0f, 0.0f, 0.0f);
							actor->LaunchEffect(7, v, 0.0f);
						}
						actor->SendMsg(NULL, AM_MAKE_NUMBER, -it->DamageNum, 0xff00ff00, 'r');
						if ((int)(short)actor->m_Hp - it->DamageNum > (int)(short)actor->m_MaxHp) {
							actor->m_Hp = actor->m_MaxHp;
						} else {
							actor->m_Hp = (short)((short)actor->m_Hp - (short)it->DamageNum);
						}
					}
					if (actor->m_Hp > actor->m_MaxHp)
						actor->m_Hp = actor->m_MaxHp;
					if (actor->m_Hp < 0)
						actor->m_Hp = actor->m_MaxHp;
					if (actor->GetGID() == 1) {
						g_session.SetStatusData(actor);
					}
				} else {
					int healAmt = (int)(short)actor->m_Hp - it->DamageNum;
					if (healAmt > 0) {
						if (actor->GetGID() != 3) {
							actor->m_Hp -= (short)it->DamageNum;
						}
						actor->SetState(4);
						if (actor->IsThisPC(actor->GetJob()) == 0 && g_used_Skill == 0) {
							const char* wn = g_session.GetWeaponHitWaveName(-1);
							PlayWave(wn, 0, 0, 0, 250, 40, 1.0f);
						}
						if ((int)(short)actor->m_Hp < ((int)(short)actor->m_MaxHp / 2)) {
							if (actor->GetJob() == 0x4da) {
								g_BattleEvent = 0;
								actor->SetJob(0x64d);
								actor->m_Hp = actor->m_MaxHp;
								vector3d v1(0.0f, 0.0f, 0.0f);
								actor->LaunchEffect(0x9e, v1, 0.0f);
								actor->SendMsg(NULL, AM_REMOVEGAGE);
							} else if (actor->GetJob() == 0x64d) {
								g_BattleEvent = 0;
								actor->SetJob(0x62e);
								g_PrevMob = (short)actor->GetJob();
								actor->m_Hp = actor->m_MaxHp;
								vector3d v2(0.0f, 0.0f, 0.0f);
								actor->LaunchEffect(0x9e, v2, 0.0f);
								actor->SendMsg(NULL, AM_REMOVEGAGE);
							} else if (actor->GetJob() == 0x62e) {
								g_BattleEvent = 0;
								actor->SetJob(0x56c);
								actor->m_MaxHp = 0x3a98;
								actor->m_Hp = 0x3a98;
								vector3d v3(0.0f, 0.0f, 0.0f);
								actor->LaunchEffect(0x9e, v3, 0.0f);
								actor->SendMsg(NULL, AM_REMOVEGAGE);
							}
						}
					} else {
						if (actor->GetGID() == 3) {
							actor->SetState(4);
						} else {
							actor->m_Hp = 0;
							actor->SetState(3);
							if (actor->GetGID() == 1) {
								g_curStance = 4;
								g_MainNextActTime = timeGetTime() + 0xfa0;
								if (actor->m_clevel <= 6) {
									sprintf(msg, "The day ends when father and daughter have both acted!");
									g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xc8c8c8, 0, 0);
								}
								g_windowMgr.BS_StartFasterTetris();
							} else if (actor->GetGID() >= 0x64) {
								GainExp((short)actor->m_Exp, actor->GetJob());
							}
							g_windowMgr.EndFight_Status();
						}
					}
				}

				short hp = (short)actor->m_Hp;
				short maxHp = (short)actor->m_MaxHp;
				actor->SendMsg(NULL, AM_UPDATEGAGE_HP, hp, maxHp, 0);

				if (it->DamageEnd == 1) {
					actor->SendMsg(NULL, AM_MAKE_NUMBER, it->DamageNum, 0xffffffff, 0x0e);
				}

				if (actor->GetGID() == 3) {
					CGameMode* gameMode = g_modeMgr.GetGameMode();
					float halfZoom = (float)gameMode->m_world->m_attr->m_zoom * 0.5f;
					actor->m_pos.x = (19.0f - (float)gameMode->m_world->m_attr->m_width * 0.5f) * (float)gameMode->m_world->m_attr->m_zoom + halfZoom;
					actor->m_pos.z = ((float)gameMode->m_world->m_attr->m_height * -0.5f) * (float)gameMode->m_world->m_attr->m_zoom + halfZoom;
				}

				if (actor->GetGID() == 1) {
					g_session.SetStatusData(actor);
				}
			} else {
				char msg[256];
				sprintf(msg, "[ DEAD ]");
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0xc8c8c8, 0, 0);
			}

			g_DM_List.erase(it);
			it = g_DM_List.begin();
			continue;
		}
		++it;
	}
}


void CGameMode::GainExp(short ExpValue, int JT_Opp)
{
	CGameActor *actor = g_session.GetGameActorByAid(1);
	if (!actor)
		return;

	short level = actor->m_clevel;
	if (level < 120)
		actor->m_Exp += g_curEXPmode * ExpValue;
	else
		actor->m_Exp += ExpValue;

	if (actor->m_Exp >= GetNextExpValue(level)) {
		vector3d v(0.0f, 0.0f, 0.0f);
		actor->LaunchEffect(0x152, v, 0.0f);

		actor->m_clevel++;
		level = actor->m_clevel;
		actor->m_Str++;
		int levelMod = level % 5;
		actor->m_Agi++;
		actor->m_Int++;
		if (levelMod == 0) {
			actor->m_Vit++;
			actor->m_Luk++;
			actor->m_Dex++;
		}

		if (g_Aco_Job == 0xfa9) {
			actor->m_MaxHp += 200;
			actor->m_MaxSp += 20;
		} else if (g_Aco_Job == 8) {
			actor->m_MaxHp += 130;
			actor->m_MaxSp += 13;
		} else {
			actor->m_MaxHp += 100;
			actor->m_MaxSp += 10;
		}

		if (actor->m_MaxHp > 0x7d00)
			actor->m_MaxHp = 0x7d00;
		if (actor->m_MaxSp > 0x7d00)
			actor->m_MaxSp = 0x7d00;

		g_session.SetNextexp(GetNextExpValue(actor->m_clevel) - GetNextExpValue(actor->m_clevel - 1));

		level = actor->m_clevel;
		if (level == 2) {
			g_session.GainSkill(0x1c, 1, true);
			actor->SendMsg(NULL, AM_CHAT, (int)"I learned Heal~ Pretty? *^_^*  Double-click me to use it~", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		} else if (level == 3) {
			g_session.GainSkill(0x111, 1, true);
			actor->SendMsg(NULL, AM_CHAT, (int)"From now on, roll the wheel up to use Aco Smash~", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		} else if (level == 4) {
			actor->SendMsg(NULL, AM_CHAT, (int)"Open the skill window and right-click to read the info T_T", 0, 0);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		} else {
			if (map_Number == 11 && !g_session.HaveThisSkill(0x1b) && g_curStory >= 10) {
				g_session.GainSkill(0x1b, 1, true);
				actor->SendMsg(NULL, AM_CHAT, (int)"Now I can Warp Portal home before I get hurt ^_^", 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
			} else if (map_Number == 10 && !g_session.HaveThisSkill(0x35) && g_curStory >= 20) {
				g_session.GainSkill(0x35, 1, true);
				actor->SendMsg(NULL, AM_CHAT, (int)"No more worrying about poison now ^^*", 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
			} else if (map_Number == 11 && !g_session.HaveThisSkill(0x21) && g_curStory >= 30) {
				g_session.GainSkill(0x21, 1, true);
				actor->SendMsg(NULL, AM_CHAT, (int)"Learned Angelus~ Pretty, right? ^^*", 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
			} else if (map_Number == 12 && !g_session.HaveThisSkill(0x49) && g_curStory >= 40) {
				g_session.GainSkill(0x49, 1, true);
				actor->SendMsg(NULL, AM_CHAT, (int)"I learned Kyrie Eleison, hehe~", 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
			} else if (map_Number == 13 && !g_session.HaveThisSkill(0x19d) && g_curStory >= 50) {
				g_session.GainSkill(0x19d, 1, true);
				actor->SendMsg(NULL, AM_CHAT, (int)"I learned Aco Tornado~ ^^;", 0, 0);
				actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
			}
		}
	}

	if (JT_Opp == 0x6f9) {
		if (g_session.HaveThisSkill(0x111) == 1) {
			g_session.GainSkill(0x111, 2, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 10) {
			g_curStory = 10;
			g_session.SetStatusData(actor);
			return;
		}
	} else if (JT_Opp == 0x6c5) {
		if (!g_session.HaveThisSkill(0x104)) {
			g_session.GainSkill(0x104, 1, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 20) {
			char msg[100];
			g_curStory = 20;
			sprintf(msg, "You're now brave enough for other areas of Geffen Field~!");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffffff, 0, 0);
			g_session.SetStatusData(actor);
			return;
		}
	} else if (JT_Opp == 0x75d) {
		if (g_session.HaveThisSkill(0x107) <= 1) {
			g_session.GainSkill(0x107, 2, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 30) {
			char msg[100];
			g_curStory = 30;
			sprintf(msg, "You're now brave enough for other areas of Payon Cave~!");
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(msg), 0xffffff, 0, 0);
			g_session.SetStatusData(actor);
			return;
		}
	} else if (JT_Opp == 0x55d) {
		if (g_session.HaveThisSkill(0x111) <= 2) {
			g_session.GainSkill(0x111, 3, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 40) {
			g_curStory = 40;
		}
	} else if (JT_Opp == 0x55e) {
		if (g_curStory <= 44) {
			g_curStory = 45;
			g_session.SetStatusData(actor);
			return;
		}
	} else if (JT_Opp == 0x6d7) {
		if (g_session.HaveThisSkill(0x1c) <= 1) {
			g_session.GainSkill(0x1c, 2, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 50) {
			g_curStory = 50;
		}
	} else if (JT_Opp == 0x5d9) {
		if (g_curStory <= 54) {
			g_curStory = 55;
			g_session.SetStatusData(actor);
			return;
		}
	} else if (JT_Opp == 0x56c) {
		if (g_session.HaveThisSkill(0x19d) <= 1) {
			g_session.GainSkill(0x19d, 2, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 60) {
			g_curStory = 60;
		}
	} else if (JT_Opp == 0x503) {
		if (g_session.HaveThisSkill(0x22) <= 0) {
			g_session.GainSkill(0x22, 1, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 70) {
			g_curStory = 70;
		}
	} else if (JT_Opp == 0x77c) {
		if (g_session.HaveThisSkill(0x22) <= 1) {
			g_session.GainSkill(0x22, 2, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 80) {
			g_curStory = 80;
		}
	} else if (JT_Opp == 0x509) {
		if (g_session.HaveThisSkill(0x22) <= 2) {
			g_session.GainSkill(0x22, 3, true);
			actor->SendMsg(NULL, AM_EMOTION, 5, 0, 0);
		}
		if (g_curStory < 90) {
			g_curStory = 90;
			g_session.SetStatusData(actor);
			return;
		}
	} else if (JT_Opp == 0x6ac) {
		if (g_curStory < 100) {
			g_curEnding = 1;
			g_session.SetStatusData(actor);
			return;
		}
	} else if (JT_Opp == 0x52c) {
		if (g_session.DoPCHaveItem(0x13f6))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x13f6, true);
	} else if (JT_Opp == 0x52d) {
		if (g_session.DoPCHaveItem(0x13f9))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x13f9, true);
	} else if (JT_Opp == 0x52e) {
		if (g_session.DoPCHaveItem(0x8de))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x8de, true);
	} else if (JT_Opp == 0x52f) {
		if (g_session.DoPCHaveItem(0x8db))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x8db, true);
	} else if (JT_Opp == 0x530) {
		if (g_session.DoPCHaveItem(0x13f3))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x13f3, true);
	} else if (JT_Opp == 0x531) {
		if (g_session.DoPCHaveItem(0x8da))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x8da, true);
	} else if (JT_Opp == 0x532) {
		if (g_session.DoPCHaveItem(0x1456))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x1456, true);
	} else if (JT_Opp == 0x533) {
		if (g_session.DoPCHaveItem(0x8dd))
			g_session.HP_AddItem(0x5f0, true);
		else
			g_session.HP_AddItem(0x8dd, true);
	}

	g_session.SetStatusData(actor);
}

int CGameMode::GetNextExpValue(short Level)
{
	if ( Level == 1 )
		return 9;
	if ( Level == 2 )
		return 25;
	if ( Level >= 105 )
		return 2 * (Level + 55) * Level - 10400;
	return 2 * (Level + 5) * Level;
}

short CGameMode::GetWeaponPower(int ItemID)
{
	if ( ItemID == 1520 )
		return 50;
	if ( ItemID == 1610 )
		return 60;
	if ( ItemID == 1504 )
		return 70;
	if ( ItemID == 1614 )
		return 80;
	if ( ItemID == 1516 )
		return 90;
	if ( ItemID == 1615 )
		return 100;
	if ( ItemID == 1519 )
		return 120;
	if ( ItemID == 1524 )
		return 140;
	if ( ItemID == 1529 )
		return 160;
	if ( ItemID == 1613 )
		return 200;
	return 10;
}

int CGameMode::GetWeaponCost(int ItemID)
{
	if ( ItemID == 1520 )
		return 1000;
	if ( ItemID == 1610 )
		return 2000;
	if ( ItemID == 1504 )
		return 4000;
	if ( ItemID == 1614 )
		return 8000;
	if ( ItemID == 1516 )
		return 16000;
	if ( ItemID == 1615 )
		return 32000;
	if ( ItemID == 1519 )
		return 64000;
	if ( ItemID == 1524 )
		return 100000;
	if ( ItemID == 1529 )
		return 150000;
	if ( ItemID == 1613 )
		return 200000;
	if ( ItemID == 5173 || ItemID == 5300 || ItemID == 5212 || ItemID == 5263 )
		return 5000;
	if ( ItemID == 5214 || ItemID == 5137 || ItemID == 5128 )
		return 25000;
	if ( ItemID == 5210 || ItemID == 5380 || ItemID == 5381 )
		return 98000;
	if ( ItemID == 2242 || ItemID == 2276 || ItemID == 5175 || ItemID == 5135 )
		return 5000;
	if ( ItemID == 5223 || ItemID == 5357 || ItemID == 2286 || ItemID == 5315 )
		return 25000;
	if ( ItemID == 5325 || ItemID == 5102 )
		return 99800;
	return 1000;
}

void CGameMode::DeleteBroadcastBalloon()
{
	if ( m_broadcastBalloon )
	{
		g_windowMgr.PostQuit( m_broadcastBalloon );
		m_broadcastBalloon = NULL;
	}
}
