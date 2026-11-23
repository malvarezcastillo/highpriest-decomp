#include "common.h"
#include "device/Control.h"
#include "GameMode.h"
#include "3dlib/DrawUtil.h"
#include "device/Connection.h"
#include "MsgStrings.h"
#include "framework/Locale.h"
#include "framework/Language.h"
#include "SnapMgr.h"
#include "device/Sound.h"
#include "resource/Wave.h"
#include "window/UISys.h"
#include "window/UIFrameWnd.h"
#include "window/UIIllustWnd.h"
#include "window/UIImeWnd.h"
#include "window/UIEmotionWnd.h"
#include "window/UIEmotionListWnd.h"
#include "UIGuildWnd.h"
#include "UIMessengerWnd.h"
#include "GameActor.h"
#include "RagEffect.h"
#include "3dActor.h"
#include "Session.h"
#include "3dlib/Renderer.h"
#include "3dlib/Device.h"
#include "3dlib/Texture.h"
#include "resource/Bitmap.h"
#include "window/UIMetalWorkWnd.h"
#include "resource/Skybox.h"
#include "Mode.h"
#include "GameWorld.h"
#include "MercenaryAI.h"
#include "HP_Tetris.h"
#include "GuildInfo.h"
#include "base/Res.h"
#include "pdb_enums.h"
#include "resource/Ground.h"
#include "pdb_shared.h"

extern CRenderer* g_renderer;
extern CSession g_session;
extern Registry2 g_registry;
extern CModeMgr g_modeMgr;
extern CMouse g_mouse;
extern const char* g_regPath;
extern char* g_uiPath;
extern char* g_shortItemPath;
extern char* g_suPath;
extern LanguageType g_languageType;
extern ServiceType g_serviceType;
extern CSnapMgr g_snapMgr;
extern UIWindowMgr g_windowMgr;
extern short g_mouse_L;
extern bool g_mouseMoved;
extern short g_menuSelect;
extern int g_curScreen;
extern int g_curStance_B;
extern Tetris* TT;
extern const char* MsgStr(MSGSTRINGID id);
extern void CutString(const char* msg, std::vector<std::string>& strings, int maxNumCharLine);
extern const char* whisperWindowOpenSound;




static void ClearChatHistory(std::list<StringAndColor>& history)
{
	history.erase(history.begin(), history.end());
}

char* g_uiPath = "\\UIRectInfo";
short g_menuSelect = 1;
int g_curRectTop = 210;
short g_AcoPosition = 1000;
short g_PrevMob = 1002;
short map_Number = 10;
short map_firstway = 10;
short map_secondstart = -1;
short map_secondway = 4;
short map_thirdstart = -1;
short map_thirdway = 7;
short map_BNPC_ID = 3;
short Boy_Job = 1;
short g_Aco_Job = 4;
short g_Aco_Head = 1;
short g_BS_Job = 10;
char g_Aco_Name[32] = "Daughter";
char g_Aco_Hair = 1;
char g_BS_Hair = 3;
char g_curSave = 1;
char g_curHardness = 1;
char g_curEXPmode = 1;
char* g_shortItemPath = "\\ShortcutItem";
char* g_suPath = "\\SkillUseLevelInfo";
int guildWndWidth = 400;
int guildWndHeight = 317;



const char* whisperWindowOpenSound = "..\\..\\BGM\\Msg_NewMsg.wav";

UIWindowMgr g_windowMgr;
std::vector<int> stateId;


char g_savedata[3][256];
HP_TRAVEL_AVAILABLE HP_TravelMap[5] = {0};
short g_PoringCurX = 0;
short g_PoringCurY = 0;
short g_PoringTarY = 0;
short g_PoringTarX = 0;
int g_curScreen = 0;
short g_curStory = 0;
int g_curStance = 0;
int g_curStance_B = 0;
short g_BS_phase = 0;
unsigned long g_BS_NextActTime = 0;
bool g_WordDisplay = 0;
short g_mouse_L = 0;
short g_menuFreeze = 0;
bool g_mouseMoved = 0;
short g_ScreenConv_Alpha = 0;
short g_FieldConv_Alpha = 0;
unsigned long g_MainNextActTime = 0;
unsigned long g_MobNextOutTime = 0;
unsigned long g_HelpNextActTime = 0;
unsigned long g_BoyAttackedTime = 0;
int g_Frame = 0;
bool g_MobNextTimeFixed = 0;
short g_scroll_Y = 0;
short bat_STUN_turn = 0;
short bat_mouse_pos = 0;
char bat_ChookGi = 0;
short bat_mobskill_turn = 0;
bool map_BOSS_done = 0;
short map_Heal_Used = 0;
short map_ComboF_Used = 0;
short map_BNPC_Pos = 0;
bool map_BNPC_done = 0;
bool map_BNPC_event = 0;
short game_TetrisPlayed = 0;
short game_ShopOpenNumber = 0;
bool game_ItemRentEvent = 0;
short g_selected_item = 0;
short g_selected_item_ID = 0;
int g_used_Skill = 0;
unsigned long g_used_Skill_Time = 0;
int g_Mob_used_Skill = 0;
int g_BattleEvent = 0;
int g_NextSkill = 0;
short g_SpellSeek = 0;
unsigned long g_SpellSeekTime = 0;
char g_TravelMap_Count = 0;
short Boy_friendly = 0;
short Boy_talked = 0;
bool Boy_frame = 0;
short g_Aco_HeadPal = 0;
short g_Aco_BodyPal = 0;
char g_TIME_hr = 0;
char g_TIME_min = 0;
char g_Aco_Hair_Color = 0;
char g_BS_Hair_Color = 0;
char g_curEnding = 0;
char g_ClearedGame = 0;
unsigned long g_TimeStorage = 0;







void UIWindowMgr::InitUIRectInfoFromReg()
{
	DWORD REG_TYPE;
	DWORD DATALENGTH;
	HKEY hResultKey;
	HKEY hKey;
	char UIPath[512];

	strcpy(UIPath, g_regPath);
	strcat(UIPath, g_uiPath);

	LRESULT lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, KEY_QUERY_VALUE, &hResultKey);
	if (lResult == ERROR_SUCCESS)
	{
	RegCloseKey(hResultKey);
	LRESULT lResult1 = RegOpenKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, KEY_QUERY_VALUE, &hKey);
	if (lResult1 == ERROR_SUCCESS)
	{

	REG_TYPE = REG_DWORD;
	DATALENGTH = 4;

	RegQueryValueExA(hKey, "m_minimapZoomWnd", 0, &REG_TYPE, (LPBYTE)&m_miniMapZoomFactor, &DATALENGTH);
	if (RegQueryValueExA(hKey, "M_ISDRAGALL", 0, &REG_TYPE, (LPBYTE)&m_isDragAll, &DATALENGTH) != 0)
		m_isDragAll = 1;
	if (m_isDragAll != 0)
		m_isDragAll = 1;
	if (RegQueryValueExA(hKey, "M_ISDRAWCOMPASS", 0, &REG_TYPE, (LPBYTE)&m_isDrawCompass, &DATALENGTH) != 0)
		m_isDrawCompass = 0;
	if (RegQueryValueExA(hKey, "M_MINIMAPARGB", 0, &REG_TYPE, (LPBYTE)&m_miniMapArgb, &DATALENGTH) != 0)
		m_miniMapArgb = 0xaaffffff;
	if (m_miniMapArgb != 0xffffffff)
		m_miniMapArgb = 0xaaffffff;
	RegQueryValueExA(hKey, "M_CHATWNDHEIGHT", 0, &REG_TYPE, (LPBYTE)&m_chatWndHeight, &DATALENGTH);
	RegQueryValueExA(hKey, "M_CHATWNDX", 0, &REG_TYPE, (LPBYTE)&m_chatWndX, &DATALENGTH);
	RegQueryValueExA(hKey, "M_CHATWNDY", 0, &REG_TYPE, (LPBYTE)&m_chatWndY, &DATALENGTH);
	RegQueryValueExA(hKey, "M_CHATWNDSHOW", 0, &REG_TYPE, (LPBYTE)&m_chatWndShow, &DATALENGTH);

	RegQueryValueExA(hKey, "SAYDIALOGWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&sayDialogWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "SAYDIALOGWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&sayDialogWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "SAYDIALOGWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&sayDialogWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "SAYDIALOGWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&sayDialogWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "CHOOSEWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&chooseWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "CHOOSEWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&chooseWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "CHOOSEWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&chooseWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "CHOOSEWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&chooseWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "BASICINFOWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&basicInfoWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "BASICINFOWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&basicInfoWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "BASICINFOWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&basicInfoWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "BASICINFOWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&basicInfoWndInfo.h, &DATALENGTH);
	basicInfoWndInfo.x = 740;
	basicInfoWndInfo.y = 645;
	basicInfoWndInfo.h = 120;

	RegQueryValueExA(hKey, "ITEMWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.h, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.ORGHEIGHT", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.orgHeight, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.CURTAB", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.curTab, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.CURRADIO1", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.curRadio1, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.CURRADIO2", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.curRadio2, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMWNDINFO.CURRADIO3", 0, &REG_TYPE, (LPBYTE)&itemWndInfo.curRadio3, &DATALENGTH);
	itemWndInfo.x = 744;
	itemWndInfo.y = 514;
	itemWndInfo.w = 280;
	itemWndInfo.h = 134;
	itemWndInfo.orgHeight = 134;
	itemWndInfo.curTab = 1;
	itemWndInfo.show = 0;

	RegQueryValueExA(hKey, "QUESTWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&questWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "QUESTWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&questWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "QUESTWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&questWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "QUESTWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&questWndInfo.h, &DATALENGTH);
	RegQueryValueExA(hKey, "QUESTWNDINFO.ORGHEIGHT", 0, &REG_TYPE, (LPBYTE)&questWndInfo.orgHeight, &DATALENGTH);
	RegQueryValueExA(hKey, "QUESTWNDINFO.CURTAB", 0, &REG_TYPE, (LPBYTE)&questWndInfo.curTab, &DATALENGTH);
	RegQueryValueExA(hKey, "QUESTWNDINFO.SHOW", 0, &REG_TYPE, (LPBYTE)&questWndInfo.show, &DATALENGTH);

	RegQueryValueExA(hKey, "STATUSWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&statusWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "STATUSWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&statusWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "STATUSWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&statusWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "STATUSWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&statusWndInfo.h, &DATALENGTH);
	RegQueryValueExA(hKey, "STATUSWNDINFO.ORGHEIGHT", 0, &REG_TYPE, (LPBYTE)&statusWndInfo.orgHeight, &DATALENGTH);
	statusWndInfo.show = 0;

	RegQueryValueExA(hKey, "EQUIPWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&equipWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "EQUIPWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&equipWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "EQUIPWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&equipWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "EQUIPWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&equipWndInfo.h, &DATALENGTH);
	RegQueryValueExA(hKey, "EQUIPWNDINFO.ORGHEIGHT", 0, &REG_TYPE, (LPBYTE)&equipWndInfo.orgHeight, &DATALENGTH);
	equipWndInfo.show = 0;

	RegQueryValueExA(hKey, "OPTIONWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&optionWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "OPTIONWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&optionWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "OPTIONWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&optionWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "OPTIONWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&optionWndInfo.h, &DATALENGTH);
	RegQueryValueExA(hKey, "OPTIONWNDINFO.ORGHEIGHT", 0, &REG_TYPE, (LPBYTE)&optionWndInfo.orgHeight, &DATALENGTH);
	optionWndInfo.show = 0;

	RegQueryValueExA(hKey, "SHORTENITEMWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&shortenItemWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "SHORTENITEMWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&shortenItemWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "SHORTENITEMWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&shortenItemWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "SHORTENITEMWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&shortenItemWndInfo.h, &DATALENGTH);
	shortenItemWndInfo.show = 0;

	RegQueryValueExA(hKey, "SHORTCUTWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&shortCutWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "SHORTCUTWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&shortCutWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "SHORTCUTWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&shortCutWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "SHORTCUTWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&shortCutWndInfo.h, &DATALENGTH);
	shortCutWndInfo.show = 0;

	RegQueryValueExA(hKey, "CHATROOMWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&chatRoomWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "CHATROOMWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&chatRoomWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "CHATROOMWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&chatRoomWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "CHATROOMWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&chatRoomWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMSTOREWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemStoreWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSTOREWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemStoreWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSTOREWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemStoreWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSTOREWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemStoreWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMSHOPWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemShopWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSHOPWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemShopWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSHOPWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemShopWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSHOPWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemShopWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMSELLWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemSellWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSELLWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemSellWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSELLWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemSellWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMSELLWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemSellWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMPURCHASEWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemPurchaseWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMPURCHASEWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemPurchaseWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMPURCHASEWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemPurchaseWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMPURCHASEWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemPurchaseWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "MESSENGERGROUPWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&messengerGroupWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "MESSENGERGROUPWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&messengerGroupWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "MESSENGERGROUPWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&messengerGroupWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "MESSENGERGROUPWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&messengerGroupWndInfo.h, &DATALENGTH);
	if (RegQueryValueExA(hKey, "MESSENGERGROUPWNDINFO.SHOW", 0, &REG_TYPE, (LPBYTE)&messengerGroupWndInfo.show, &DATALENGTH) != 0)
		messengerGroupWndInfo.show = 0;
	if (RegQueryValueExA(hKey, "MESSENGERGROUPWNDINFO.RADIO", 0, &REG_TYPE, (LPBYTE)&messengerGroupWndInfo.radio, &DATALENGTH) != 0)
		messengerGroupWndInfo.radio = 1;

	RegQueryValueExA(hKey, "EXCHANGEACCEPTWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&exchangeAcceptWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "EXCHANGEACCEPTWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&exchangeAcceptWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "EXCHANGEACCEPTWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&exchangeAcceptWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "EXCHANGEACCEPTWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&exchangeAcceptWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "SKILLLISTWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&skillListWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "SKILLLISTWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&skillListWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "SKILLLISTWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&skillListWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "SKILLLISTWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&skillListWndInfo.h, &DATALENGTH);
	skillListWndInfo.show = 0;

	RegQueryValueExA(hKey, "MERCHANTITEMWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&merchantItemWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&merchantItemWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&merchantItemWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&merchantItemWndInfo.h, &DATALENGTH);
	merchantItemWndInfo.show = 0;

	RegQueryValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&merchantMirrorItemWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&merchantMirrorItemWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&merchantMirrorItemWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&merchantMirrorItemWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&merchantShopMakeWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&merchantShopMakeWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&merchantShopMakeWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&merchantShopMakeWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&merchantItemShopWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&merchantItemShopWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&merchantItemShopWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&merchantItemShopWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&merchantItemMyShopWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&merchantItemMyShopWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&merchantItemMyShopWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&merchantItemMyShopWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&merchantItemPurchaseWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&merchantItemPurchaseWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&merchantItemPurchaseWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&merchantItemPurchaseWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMCOLLECTIONWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemCollectionWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMCOLLECTIONWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemCollectionWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMCOLLECTIONWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemCollectionWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMCOLLECTIONWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemCollectionWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&combinedCardItemCollectionWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&combinedCardItemCollectionWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&combinedCardItemCollectionWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&combinedCardItemCollectionWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemParamChangeDisplayWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemParamChangeDisplayWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemParamChangeDisplayWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemParamChangeDisplayWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "PARTYSETTINGWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&partySettingWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "PARTYSETTINGWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&partySettingWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "PARTYSETTINGWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&partySettingWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "PARTYSETTINGWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&partySettingWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "DETAILLEVELWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&detailLevelWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "DETAILLEVELWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&detailLevelWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "DETAILLEVELWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&detailLevelWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "DETAILLEVELWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&detailLevelWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "WHISPERWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&whisperWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "WHISPERWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&whisperWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "WHISPERWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&whisperWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "WHISPERWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&whisperWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "FRIENDOPTIONWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&friendOptionWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "FRIENDOPTIONWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&friendOptionWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "FRIENDOPTIONWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&friendOptionWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "FRIENDOPTIONWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&friendOptionWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemCompositionWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemCompositionWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemCompositionWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemCompositionWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "ITEMIDENTIFYWNDINFO.X", 0, &REG_TYPE, (LPBYTE)&itemIdentifyWndInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMIDENTIFYWNDINFO.Y", 0, &REG_TYPE, (LPBYTE)&itemIdentifyWndInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMIDENTIFYWNDINFO.W", 0, &REG_TYPE, (LPBYTE)&itemIdentifyWndInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "ITEMIDENTIFYWNDINFO.H", 0, &REG_TYPE, (LPBYTE)&itemIdentifyWndInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "SUBCHATWNDST.X", 0, &REG_TYPE, (LPBYTE)&subChatWndStInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "SUBCHATWNDST.Y", 0, &REG_TYPE, (LPBYTE)&subChatWndStInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "SUBCHATWNDST.W", 0, &REG_TYPE, (LPBYTE)&subChatWndStInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "SUBCHATWNDST.H", 0, &REG_TYPE, (LPBYTE)&subChatWndStInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "SUBCHATWNDBT.X", 0, &REG_TYPE, (LPBYTE)&subChatWndBtInfo.x, &DATALENGTH);
	RegQueryValueExA(hKey, "SUBCHATWNDBT.Y", 0, &REG_TYPE, (LPBYTE)&subChatWndBtInfo.y, &DATALENGTH);
	RegQueryValueExA(hKey, "SUBCHATWNDBT.W", 0, &REG_TYPE, (LPBYTE)&subChatWndBtInfo.w, &DATALENGTH);
	RegQueryValueExA(hKey, "SUBCHATWNDBT.H", 0, &REG_TYPE, (LPBYTE)&subChatWndBtInfo.h, &DATALENGTH);

	RegQueryValueExA(hKey, "CHATWNDSTICKON", 0, &REG_TYPE, (LPBYTE)&m_chatWndStickOn, &DATALENGTH);
	RegQueryValueExA(hKey, "ONSTSUBCHAT", 0, &REG_TYPE, (LPBYTE)&m_onStSubChat, &DATALENGTH);
	RegQueryValueExA(hKey, "ONBTSUBCHAT", 0, &REG_TYPE, (LPBYTE)&m_onBtSubChat, &DATALENGTH);
	RegQueryValueExA(hKey, "MEMORIALDUNWND.X", 0, &REG_TYPE, (LPBYTE)&g_session.dunInfoWndX, &DATALENGTH);
	RegQueryValueExA(hKey, "MEMORIALDUNWND.Y", 0, &REG_TYPE, (LPBYTE)&g_session.dunInfoWndY, &DATALENGTH);


	RegCloseKey(hKey);
	}

	return;
	}

	MakeDefaultUIRectInfo();
}

void UIWindowMgr::InitShortcutItemInfoFromReg()
{
	int shortenSkillUseLevel;
	HKEY hKey;
	DWORD REG_TYPE;
	DWORD DATALENGTH;
	HKEY hResultKey;
	char itemName[33];
	char sItemRegName[33];
	char UIPath[512];

	strcpy(UIPath, g_regPath);
	strcat(UIPath, g_shortItemPath);
	strcat(UIPath, "\\");
	if (strstr(g_session.m_lastServerName.c_str(), "\xbb\xe7\xc5\xa9\xb6\xf3\xc0\xcc"))
		strcat(UIPath, "\xbb\xe7\xc5\xa9\xb6\xf3\xc0\xcc");
	else
		strcat(UIPath, g_session.m_lastServerName.c_str());

	LRESULT lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, KEY_QUERY_VALUE, &hResultKey);
	if (lResult == ERROR_SUCCESS)
	{
		RegCloseKey(hResultKey);
		LRESULT lResult1 = RegOpenKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, KEY_QUERY_VALUE, &hKey);
		if (lResult1 == ERROR_SUCCESS)
		{
			g_session.ClearShortenItemName();
			g_session.ClearShortenSkillUseLevel();

			for (int i = 0; i < 27; ++i)
			{
				REG_TYPE = 1;
				DATALENGTH = 0x20;
				sprintf(sItemRegName, "SITEM%d%d%d", i, g_serviceType, g_session.m_charSlot);
				memset(itemName, 0, sizeof(itemName));
				LONG res = RegQueryValueExA(hKey, sItemRegName, 0, &REG_TYPE, (LPBYTE)itemName, &DATALENGTH);
				int flag = 0;
				g_session.m_shortcutSlotCnt = i / 9;
				if (res == 0) {
					if (itemName[0] != '#')
						g_session.SetShortenItemName(itemName, i);
					else
						flag = 1;
				}
				else
					g_session.SetShortenItemName("nothing", i);

				REG_TYPE = 4;
				DATALENGTH = 4;
				sprintf(sItemRegName, "S_SKILL_USE_LEVEL%d%d%d", i, g_serviceType, g_session.m_charSlot);
				shortenSkillUseLevel = 0;
				res = RegQueryValueExA(hKey, sItemRegName, 0, &REG_TYPE, (LPBYTE)&shortenSkillUseLevel, &DATALENGTH);

				if (res == 0)
					g_session.SetShortenSkillUseLevel(shortenSkillUseLevel, i);
				else
				{
					g_session.SetShortenSkillUseLevel(0, i);
					shortenSkillUseLevel = 0;
				}

				if (flag)
					g_session.SetShortenSkillName(itemName, shortenSkillUseLevel, i);
			}

			RegCloseKey(hKey);
		}
	}
	g_session.m_shortcutSlotCnt = 0;
}

void UIWindowMgr::WriteShorcutItemInfoToReg()
{
	HKEY hKey;
	int shortenSkillUseLevel;
	DWORD dwDisposition;
	char sItemRegName[33];
	char itemName[33];
	char UIPath[512];
	int i;

	strcpy(UIPath, g_regPath);
	strcat(UIPath, g_shortItemPath);
	strcat(UIPath, "\\");
	if (strstr(g_session.m_lastServerName.c_str(), "\xbb\xe7\xc5\xa9\xb6\xf3\xc0\xcc"))
		strcat(UIPath, "\xbb\xe7\xc5\xa9\xb6\xf3\xc0\xcc");
	else
		strcat(UIPath, g_session.m_lastServerName.c_str());

	dwDisposition = 0;
	LRESULT lResult = RegCreateKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult != ERROR_SUCCESS)
		return;

	for (i = 0; i < 27; ++i)
	{
		memset(itemName, 0, sizeof(itemName));
		strcpy(itemName, g_session.GetShortenItemName(i));
		sprintf(sItemRegName, "SITEM%d%d%d", i, g_serviceType, g_session.m_charSlot);
		RegSetValueExA(hKey, sItemRegName, 0, REG_SZ, (LPBYTE)itemName, 0x20);

		shortenSkillUseLevel = g_session.GetShortenSkillUseLevel(i);
		sprintf(sItemRegName, "S_SKILL_USE_LEVEL%d%d%d", i, g_serviceType, g_session.m_charSlot);
		RegSetValueExA(hKey, sItemRegName, 0, REG_DWORD, (LPBYTE)&shortenSkillUseLevel, 4);
	}

	RegCloseKey(hKey);
}

void UIWindowMgr::WriteUIRectInfoToReg()
{
	HKEY hKey;
	DWORD dwDisposition;
	char UIPath[512];

	strcpy(UIPath, g_regPath);
	strcat(UIPath, g_uiPath);

	dwDisposition = 0;
	LRESULT lResult = RegCreateKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult != ERROR_SUCCESS)
		return;

	RegSetValueExA(hKey, "m_minimapZoomWnd", 0, REG_DWORD, (LPBYTE)&m_miniMapZoomFactor, 4);
	RegSetValueExA(hKey, "M_ISDRAGALL", 0, REG_DWORD, (LPBYTE)&m_isDragAll, 4);
	RegSetValueExA(hKey, "M_ISDRAWCOMPASS", 0, REG_DWORD, (LPBYTE)&m_isDrawCompass, 4);
	RegSetValueExA(hKey, "M_MINIMAPARGB", 0, REG_DWORD, (LPBYTE)&m_miniMapArgb, 4);
	RegSetValueExA(hKey, "M_CHATWNDSTATUS", 0, REG_DWORD, (LPBYTE)&m_chatWndStatus, 4);
	RegSetValueExA(hKey, "M_CHATWNDHEIGHT", 0, REG_DWORD, (LPBYTE)&m_chatWndHeight, 4);
	RegSetValueExA(hKey, "M_CHATWNDX", 0, REG_DWORD, (LPBYTE)&m_chatWndX, 4);
	RegSetValueExA(hKey, "M_CHATWNDY", 0, REG_DWORD, (LPBYTE)&m_chatWndY, 4);
	RegSetValueExA(hKey, "M_CHATWNDSHOW", 0, REG_DWORD, (LPBYTE)&m_chatWndShow, 4);

	RegSetValueExA(hKey, "SAYDIALOGWNDINFO.X", 0, REG_DWORD, (LPBYTE)&sayDialogWndInfo.x, 4);
	RegSetValueExA(hKey, "SAYDIALOGWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&sayDialogWndInfo.y, 4);
	RegSetValueExA(hKey, "SAYDIALOGWNDINFO.W", 0, REG_DWORD, (LPBYTE)&sayDialogWndInfo.w, 4);
	RegSetValueExA(hKey, "SAYDIALOGWNDINFO.H", 0, REG_DWORD, (LPBYTE)&sayDialogWndInfo.h, 4);

	RegSetValueExA(hKey, "CHOOSEWNDINFO.X", 0, REG_DWORD, (LPBYTE)&chooseWndInfo.x, 4);
	RegSetValueExA(hKey, "CHOOSEWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&chooseWndInfo.y, 4);
	RegSetValueExA(hKey, "CHOOSEWNDINFO.W", 0, REG_DWORD, (LPBYTE)&chooseWndInfo.w, 4);
	RegSetValueExA(hKey, "CHOOSEWNDINFO.H", 0, REG_DWORD, (LPBYTE)&chooseWndInfo.h, 4);

	RegSetValueExA(hKey, "BASICINFOWNDINFO.X", 0, REG_DWORD, (LPBYTE)&basicInfoWndInfo.x, 4);
	RegSetValueExA(hKey, "BASICINFOWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&basicInfoWndInfo.y, 4);
	RegSetValueExA(hKey, "BASICINFOWNDINFO.W", 0, REG_DWORD, (LPBYTE)&basicInfoWndInfo.w, 4);
	RegSetValueExA(hKey, "BASICINFOWNDINFO.H", 0, REG_DWORD, (LPBYTE)&basicInfoWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemWndInfo.h, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.ORGHEIGHT", 0, REG_DWORD, (LPBYTE)&itemWndInfo.orgHeight, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.CURTAB", 0, REG_DWORD, (LPBYTE)&itemWndInfo.curTab, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.CURRADIO1", 0, REG_DWORD, (LPBYTE)&itemWndInfo.curRadio1, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.CURRADIO2", 0, REG_DWORD, (LPBYTE)&itemWndInfo.curRadio2, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.CURRADIO3", 0, REG_DWORD, (LPBYTE)&itemWndInfo.curRadio3, 4);
	RegSetValueExA(hKey, "ITEMWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&itemWndInfo.show, 4);

	RegSetValueExA(hKey, "QUESTWNDINFO.X", 0, REG_DWORD, (LPBYTE)&questWndInfo.x, 4);
	RegSetValueExA(hKey, "QUESTWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&questWndInfo.y, 4);
	RegSetValueExA(hKey, "QUESTWNDINFO.W", 0, REG_DWORD, (LPBYTE)&questWndInfo.w, 4);
	RegSetValueExA(hKey, "QUESTWNDINFO.H", 0, REG_DWORD, (LPBYTE)&questWndInfo.h, 4);
	RegSetValueExA(hKey, "QUESTWNDINFO.ORGHEIGHT", 0, REG_DWORD, (LPBYTE)&questWndInfo.orgHeight, 4);
	RegSetValueExA(hKey, "QUESTWNDINFO.CURTAB", 0, REG_DWORD, (LPBYTE)&questWndInfo.curTab, 4);
	RegSetValueExA(hKey, "QUESTWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&questWndInfo.show, 4);

	RegSetValueExA(hKey, "STATUSWNDINFO.X", 0, REG_DWORD, (LPBYTE)&statusWndInfo.x, 4);
	RegSetValueExA(hKey, "STATUSWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&statusWndInfo.y, 4);
	RegSetValueExA(hKey, "STATUSWNDINFO.W", 0, REG_DWORD, (LPBYTE)&statusWndInfo.w, 4);
	RegSetValueExA(hKey, "STATUSWNDINFO.H", 0, REG_DWORD, (LPBYTE)&statusWndInfo.h, 4);
	RegSetValueExA(hKey, "STATUSWNDINFO.ORGHEIGHT", 0, REG_DWORD, (LPBYTE)&statusWndInfo.orgHeight, 4);
	RegSetValueExA(hKey, "STATUSWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&statusWndInfo.show, 4);

	RegSetValueExA(hKey, "EQUIPWNDINFO.X", 0, REG_DWORD, (LPBYTE)&equipWndInfo.x, 4);
	RegSetValueExA(hKey, "EQUIPWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&equipWndInfo.y, 4);
	RegSetValueExA(hKey, "EQUIPWNDINFO.W", 0, REG_DWORD, (LPBYTE)&equipWndInfo.w, 4);
	RegSetValueExA(hKey, "EQUIPWNDINFO.H", 0, REG_DWORD, (LPBYTE)&equipWndInfo.h, 4);
	RegSetValueExA(hKey, "EQUIPWNDINFO.ORGHEIGHT", 0, REG_DWORD, (LPBYTE)&equipWndInfo.orgHeight, 4);
	RegSetValueExA(hKey, "EQUIPWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&equipWndInfo.show, 4);

	RegSetValueExA(hKey, "OPTIONWNDINFO.X", 0, REG_DWORD, (LPBYTE)&optionWndInfo.x, 4);
	RegSetValueExA(hKey, "OPTIONWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&optionWndInfo.y, 4);
	RegSetValueExA(hKey, "OPTIONWNDINFO.W", 0, REG_DWORD, (LPBYTE)&optionWndInfo.w, 4);
	RegSetValueExA(hKey, "OPTIONWNDINFO.H", 0, REG_DWORD, (LPBYTE)&optionWndInfo.h, 4);
	RegSetValueExA(hKey, "OPTIONWNDINFO.ORGHEIGHT", 0, REG_DWORD, (LPBYTE)&optionWndInfo.orgHeight, 4);
	RegSetValueExA(hKey, "OPTIONWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&optionWndInfo.show, 4);

	RegSetValueExA(hKey, "SHORTENITEMWNDINFO.X", 0, REG_DWORD, (LPBYTE)&shortenItemWndInfo.x, 4);
	RegSetValueExA(hKey, "SHORTENITEMWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&shortenItemWndInfo.y, 4);
	RegSetValueExA(hKey, "SHORTENITEMWNDINFO.W", 0, REG_DWORD, (LPBYTE)&shortenItemWndInfo.w, 4);
	RegSetValueExA(hKey, "SHORTENITEMWNDINFO.H", 0, REG_DWORD, (LPBYTE)&shortenItemWndInfo.h, 4);
	RegSetValueExA(hKey, "SHORTENITEMWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&shortenItemWndInfo.show, 4);

	RegSetValueExA(hKey, "SHORTCUTWNDINFO.X", 0, REG_DWORD, (LPBYTE)&shortCutWndInfo.x, 4);
	RegSetValueExA(hKey, "SHORTCUTWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&shortCutWndInfo.y, 4);
	RegSetValueExA(hKey, "SHORTCUTWNDINFO.W", 0, REG_DWORD, (LPBYTE)&shortCutWndInfo.w, 4);
	RegSetValueExA(hKey, "SHORTCUTWNDINFO.H", 0, REG_DWORD, (LPBYTE)&shortCutWndInfo.h, 4);
	RegSetValueExA(hKey, "SHORTCUTWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&shortCutWndInfo.show, 4);

	RegSetValueExA(hKey, "CHATROOMWNDINFO.X", 0, REG_DWORD, (LPBYTE)&chatRoomWndInfo.x, 4);
	RegSetValueExA(hKey, "CHATROOMWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&chatRoomWndInfo.y, 4);
	RegSetValueExA(hKey, "CHATROOMWNDINFO.W", 0, REG_DWORD, (LPBYTE)&chatRoomWndInfo.w, 4);
	RegSetValueExA(hKey, "CHATROOMWNDINFO.H", 0, REG_DWORD, (LPBYTE)&chatRoomWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMSTOREWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemStoreWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMSTOREWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemStoreWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMSTOREWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemStoreWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMSTOREWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemStoreWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMSHOPWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemShopWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMSHOPWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemShopWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMSHOPWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemShopWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMSHOPWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemShopWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMSELLWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemSellWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMSELLWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemSellWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMSELLWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemSellWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMSELLWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemSellWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMPURCHASEWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemPurchaseWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMPURCHASEWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemPurchaseWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMPURCHASEWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemPurchaseWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMPURCHASEWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemPurchaseWndInfo.h, 4);

	RegSetValueExA(hKey, "MESSENGERGROUPWNDINFO.X", 0, REG_DWORD, (LPBYTE)&messengerGroupWndInfo.x, 4);
	RegSetValueExA(hKey, "MESSENGERGROUPWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&messengerGroupWndInfo.y, 4);
	RegSetValueExA(hKey, "MESSENGERGROUPWNDINFO.W", 0, REG_DWORD, (LPBYTE)&messengerGroupWndInfo.w, 4);
	RegSetValueExA(hKey, "MESSENGERGROUPWNDINFO.H", 0, REG_DWORD, (LPBYTE)&messengerGroupWndInfo.h, 4);
	RegSetValueExA(hKey, "MESSENGERGROUPWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&messengerGroupWndInfo.show, 4);
	RegSetValueExA(hKey, "MESSENGERGROUPWNDINFO.RADIO", 0, REG_DWORD, (LPBYTE)&messengerGroupWndInfo.radio, 4);

	RegSetValueExA(hKey, "EXCHANGEACCEPTWNDINFO.X", 0, REG_DWORD, (LPBYTE)&exchangeAcceptWndInfo.x, 4);
	RegSetValueExA(hKey, "EXCHANGEACCEPTWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&exchangeAcceptWndInfo.y, 4);
	RegSetValueExA(hKey, "EXCHANGEACCEPTWNDINFO.W", 0, REG_DWORD, (LPBYTE)&exchangeAcceptWndInfo.w, 4);
	RegSetValueExA(hKey, "EXCHANGEACCEPTWNDINFO.H", 0, REG_DWORD, (LPBYTE)&exchangeAcceptWndInfo.h, 4);

	RegSetValueExA(hKey, "SKILLLISTWNDINFO.X", 0, REG_DWORD, (LPBYTE)&skillListWndInfo.x, 4);
	RegSetValueExA(hKey, "SKILLLISTWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&skillListWndInfo.y, 4);
	RegSetValueExA(hKey, "SKILLLISTWNDINFO.W", 0, REG_DWORD, (LPBYTE)&skillListWndInfo.w, 4);
	RegSetValueExA(hKey, "SKILLLISTWNDINFO.H", 0, REG_DWORD, (LPBYTE)&skillListWndInfo.h, 4);
	RegSetValueExA(hKey, "SKILLLISTWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&skillListWndInfo.show, 4);

	RegSetValueExA(hKey, "MERCHANTITEMWNDINFO.X", 0, REG_DWORD, (LPBYTE)&merchantItemWndInfo.x, 4);
	RegSetValueExA(hKey, "MERCHANTITEMWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&merchantItemWndInfo.y, 4);
	RegSetValueExA(hKey, "MERCHANTITEMWNDINFO.W", 0, REG_DWORD, (LPBYTE)&merchantItemWndInfo.w, 4);
	RegSetValueExA(hKey, "MERCHANTITEMWNDINFO.H", 0, REG_DWORD, (LPBYTE)&merchantItemWndInfo.h, 4);
	RegSetValueExA(hKey, "MERCHANTITEMWNDINFO.SHOW", 0, REG_DWORD, (LPBYTE)&merchantItemWndInfo.show, 4);

	RegSetValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.X", 0, REG_DWORD, (LPBYTE)&merchantMirrorItemWndInfo.x, 4);
	RegSetValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&merchantMirrorItemWndInfo.y, 4);
	RegSetValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.W", 0, REG_DWORD, (LPBYTE)&merchantMirrorItemWndInfo.w, 4);
	RegSetValueExA(hKey, "MERCHANTMIRRORITEMWNDINFO.H", 0, REG_DWORD, (LPBYTE)&merchantMirrorItemWndInfo.h, 4);

	RegSetValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.X", 0, REG_DWORD, (LPBYTE)&merchantShopMakeWndInfo.x, 4);
	RegSetValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&merchantShopMakeWndInfo.y, 4);
	RegSetValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.W", 0, REG_DWORD, (LPBYTE)&merchantShopMakeWndInfo.w, 4);
	RegSetValueExA(hKey, "MERCHANTSHOPMAKEWNDINFO.H", 0, REG_DWORD, (LPBYTE)&merchantShopMakeWndInfo.h, 4);

	RegSetValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.X", 0, REG_DWORD, (LPBYTE)&merchantItemShopWndInfo.x, 4);
	RegSetValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&merchantItemShopWndInfo.y, 4);
	RegSetValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.W", 0, REG_DWORD, (LPBYTE)&merchantItemShopWndInfo.w, 4);
	RegSetValueExA(hKey, "MERCHANTITEMSHOPWNDINFO.H", 0, REG_DWORD, (LPBYTE)&merchantItemShopWndInfo.h, 4);

	RegSetValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.X", 0, REG_DWORD, (LPBYTE)&merchantItemMyShopWndInfo.x, 4);
	RegSetValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&merchantItemMyShopWndInfo.y, 4);
	RegSetValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.W", 0, REG_DWORD, (LPBYTE)&merchantItemMyShopWndInfo.w, 4);
	RegSetValueExA(hKey, "MERCHANTITEMMYSHOPWNDINFO.H", 0, REG_DWORD, (LPBYTE)&merchantItemMyShopWndInfo.h, 4);

	RegSetValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.X", 0, REG_DWORD, (LPBYTE)&merchantItemPurchaseWndInfo.x, 4);
	RegSetValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&merchantItemPurchaseWndInfo.y, 4);
	RegSetValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.W", 0, REG_DWORD, (LPBYTE)&merchantItemPurchaseWndInfo.w, 4);
	RegSetValueExA(hKey, "MERCHANTITEMPURCHASEWNDINFO.H", 0, REG_DWORD, (LPBYTE)&merchantItemPurchaseWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMCOLLECTIONWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemCollectionWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMCOLLECTIONWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemCollectionWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMCOLLECTIONWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemCollectionWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMCOLLECTIONWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemCollectionWndInfo.h, 4);

	RegSetValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.X", 0, REG_DWORD, (LPBYTE)&combinedCardItemCollectionWndInfo.x, 4);
	RegSetValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&combinedCardItemCollectionWndInfo.y, 4);
	RegSetValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.W", 0, REG_DWORD, (LPBYTE)&combinedCardItemCollectionWndInfo.w, 4);
	RegSetValueExA(hKey, "COMBINEDCARDITEMCOLLECTIONWNDINFO.H", 0, REG_DWORD, (LPBYTE)&combinedCardItemCollectionWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemParamChangeDisplayWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemParamChangeDisplayWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemParamChangeDisplayWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMPARAMCHANGEDISPLAYWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemParamChangeDisplayWndInfo.h, 4);

	RegSetValueExA(hKey, "PARTYSETTINGWNDINFO.X", 0, REG_DWORD, (LPBYTE)&partySettingWndInfo.x, 4);
	RegSetValueExA(hKey, "PARTYSETTINGWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&partySettingWndInfo.y, 4);
	RegSetValueExA(hKey, "PARTYSETTINGWNDINFO.W", 0, REG_DWORD, (LPBYTE)&partySettingWndInfo.w, 4);
	RegSetValueExA(hKey, "PARTYSETTINGWNDINFO.H", 0, REG_DWORD, (LPBYTE)&partySettingWndInfo.h, 4);

	RegSetValueExA(hKey, "DETAILLEVELWNDINFO.X", 0, REG_DWORD, (LPBYTE)&detailLevelWndInfo.x, 4);
	RegSetValueExA(hKey, "DETAILLEVELWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&detailLevelWndInfo.y, 4);
	RegSetValueExA(hKey, "DETAILLEVELWNDINFO.W", 0, REG_DWORD, (LPBYTE)&detailLevelWndInfo.w, 4);
	RegSetValueExA(hKey, "DETAILLEVELWNDINFO.H", 0, REG_DWORD, (LPBYTE)&detailLevelWndInfo.h, 4);

	RegSetValueExA(hKey, "WHISPERWNDINFO.X", 0, REG_DWORD, (LPBYTE)&whisperWndInfo.x, 4);
	RegSetValueExA(hKey, "WHISPERWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&whisperWndInfo.y, 4);
	RegSetValueExA(hKey, "WHISPERWNDINFO.W", 0, REG_DWORD, (LPBYTE)&whisperWndInfo.w, 4);
	RegSetValueExA(hKey, "WHISPERWNDINFO.H", 0, REG_DWORD, (LPBYTE)&whisperWndInfo.h, 4);

	RegSetValueExA(hKey, "FRIENDOPTIONWNDINFO.X", 0, REG_DWORD, (LPBYTE)&friendOptionWndInfo.x, 4);
	RegSetValueExA(hKey, "FRIENDOPTIONWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&friendOptionWndInfo.y, 4);
	RegSetValueExA(hKey, "FRIENDOPTIONWNDINFO.W", 0, REG_DWORD, (LPBYTE)&friendOptionWndInfo.w, 4);
	RegSetValueExA(hKey, "FRIENDOPTIONWNDINFO.H", 0, REG_DWORD, (LPBYTE)&friendOptionWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemCompositionWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemCompositionWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemCompositionWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMCOMPOSITIONWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemCompositionWndInfo.h, 4);

	RegSetValueExA(hKey, "ITEMIDENTIFYWNDINFO.X", 0, REG_DWORD, (LPBYTE)&itemIdentifyWndInfo.x, 4);
	RegSetValueExA(hKey, "ITEMIDENTIFYWNDINFO.Y", 0, REG_DWORD, (LPBYTE)&itemIdentifyWndInfo.y, 4);
	RegSetValueExA(hKey, "ITEMIDENTIFYWNDINFO.W", 0, REG_DWORD, (LPBYTE)&itemIdentifyWndInfo.w, 4);
	RegSetValueExA(hKey, "ITEMIDENTIFYWNDINFO.H", 0, REG_DWORD, (LPBYTE)&itemIdentifyWndInfo.h, 4);

	RegSetValueExA(hKey, "SUBCHATWNDST.X", 0, REG_DWORD, (LPBYTE)&subChatWndStInfo.x, 4);
	RegSetValueExA(hKey, "SUBCHATWNDST.Y", 0, REG_DWORD, (LPBYTE)&subChatWndStInfo.y, 4);
	RegSetValueExA(hKey, "SUBCHATWNDST.W", 0, REG_DWORD, (LPBYTE)&subChatWndStInfo.w, 4);
	RegSetValueExA(hKey, "SUBCHATWNDST.H", 0, REG_DWORD, (LPBYTE)&subChatWndStInfo.h, 4);

	RegSetValueExA(hKey, "SUBCHATWNDBT.X", 0, REG_DWORD, (LPBYTE)&subChatWndBtInfo.x, 4);
	RegSetValueExA(hKey, "SUBCHATWNDBT.Y", 0, REG_DWORD, (LPBYTE)&subChatWndBtInfo.y, 4);
	RegSetValueExA(hKey, "SUBCHATWNDBT.W", 0, REG_DWORD, (LPBYTE)&subChatWndBtInfo.w, 4);
	RegSetValueExA(hKey, "SUBCHATWNDBT.H", 0, REG_DWORD, (LPBYTE)&subChatWndBtInfo.h, 4);

	RegSetValueExA(hKey, "CHATWNDSTICKON", 0, REG_DWORD, (LPBYTE)&m_chatWndStickOn, 4);
	RegSetValueExA(hKey, "ONSTSUBCHAT", 0, REG_DWORD, (LPBYTE)&m_onStSubChat, 4);
	RegSetValueExA(hKey, "ONBTSUBCHAT", 0, REG_DWORD, (LPBYTE)&m_onBtSubChat, 4);
	RegSetValueExA(hKey, "MEMORIALDUNWND.X", 0, REG_DWORD, (LPBYTE)&g_session.dunInfoWndX, 4);
	RegSetValueExA(hKey, "MEMORIALDUNWND.Y", 0, REG_DWORD, (LPBYTE)&g_session.dunInfoWndY, 4);


	RegCloseKey(hKey);
}

void UIWindowMgr::InitSkillUseLevelInfoFromReg()
{
	DWORD REG_TYPE;
	DWORD DATALENGTH;
	int skillUseLevel;
	HKEY hKey;
	HKEY hResultKey;
	char sItemRegName[24];
	char UIPath[512];

	strcpy(UIPath, g_regPath);
	strcat(UIPath, g_suPath);

	LRESULT lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, KEY_QUERY_VALUE, &hResultKey);
	if (lResult != ERROR_SUCCESS)
		return;
	RegCloseKey(hResultKey);
	lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, KEY_QUERY_VALUE, &hKey);
	if (lResult != ERROR_SUCCESS)
		return;

	int i;
	for (i = 1; i < 0x40f; ++i)
	{
		REG_TYPE = 4;
		DATALENGTH = 4;
		sprintf(sItemRegName, "%s%d", SKILL_INFO::GetSkillIdNameBySkillId(i), g_session.m_charSlot);
		lResult = RegQueryValueExA(hKey, sItemRegName, 0, &REG_TYPE, (LPBYTE)&skillUseLevel, &DATALENGTH);
		if (lResult == ERROR_SUCCESS) {
			if (SKILL_INFO::IsLevelUseSkill(i))
				g_session.SetSkillUseLevel(i, skillUseLevel);
			else
				g_session.SetSkillUseLevel(i, 0);
		} else
			g_session.SetSkillUseLevel(i, 0);
	}
	for (i = 0x1f40; i < 0x1f51; ++i)
	{
		REG_TYPE = 4;
		DATALENGTH = 4;
		sprintf(sItemRegName, "%s%d", SKILL_INFO::GetSkillIdNameBySkillId(i), g_session.m_charSlot);
		lResult = RegQueryValueExA(hKey, sItemRegName, 0, &REG_TYPE, (LPBYTE)&skillUseLevel, &DATALENGTH);
		if (lResult == ERROR_SUCCESS) {
			if (SKILL_INFO::IsLevelUseSkill(i))
				g_session.SetHomunSkillUseLevel(i, skillUseLevel);
			else
				g_session.SetHomunSkillUseLevel(i, 0);
		} else
			g_session.SetHomunSkillUseLevel(i, 0);
	}
	for (i = 0x2008; i < 0x202e; ++i)
	{
		REG_TYPE = 4;
		DATALENGTH = 4;
		sprintf(sItemRegName, "%s%d", SKILL_INFO::GetSkillIdNameBySkillId(i), g_session.m_charSlot);
		lResult = RegQueryValueExA(hKey, sItemRegName, 0, &REG_TYPE, (LPBYTE)&skillUseLevel, &DATALENGTH);
		if (lResult == ERROR_SUCCESS) {
			if (SKILL_INFO::IsLevelUseSkill(i))
				g_session.SetMerSkillUseLevel(i, skillUseLevel);
			else
				g_session.SetMerSkillUseLevel(i, 0);
		} else
			g_session.SetMerSkillUseLevel(i, 0);
	}
	RegCloseKey(hKey);
}

void UIWindowMgr::WriteSkillUseLevelInfoToReg()
{
	HKEY hKey;
	DWORD dwDisposition;
	char UIPath[512];
	char sItemRegName[24];
	int skillUseLevel;
	int i;

	strcpy(UIPath, g_regPath);
	strcat(UIPath, g_suPath);
	dwDisposition = 0;

	LRESULT lResult = RegCreateKeyExA(HKEY_LOCAL_MACHINE, UIPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult != ERROR_SUCCESS)
		return;

	for (i = 1; i < 0x40f; ++i)
	{
		skillUseLevel = g_session.GetSkillUseLevel(i);
		sprintf(sItemRegName, "%s%d", SKILL_INFO::GetSkillIdNameBySkillId(i), g_session.m_charSlot);
		RegSetValueExA(hKey, sItemRegName, 0, REG_DWORD, (LPBYTE)&skillUseLevel, 4);
	}

	for (i = 0x1f40; i < 0x1f51; ++i)
	{
		skillUseLevel = g_session.GetHomunSkillUseLevel(i);
		sprintf(sItemRegName, "%s%d", SKILL_INFO::GetSkillIdNameBySkillId(i), g_session.m_charSlot);
		RegSetValueExA(hKey, sItemRegName, 0, REG_DWORD, (LPBYTE)&skillUseLevel, 4);
	}

	for (i = 0x2008; i < 0x202e; ++i)
	{
		skillUseLevel = g_session.GetMerSkillUseLevel(i);
		sprintf(sItemRegName, "%s%d", SKILL_INFO::GetSkillIdNameBySkillId(i), g_session.m_charSlot);
		RegSetValueExA(hKey, sItemRegName, 0, REG_DWORD, (LPBYTE)&skillUseLevel, 4);
	}

	RegCloseKey(hKey);
}

void UIWindowMgr::GetDeltaSnap(UIWindow *snapWindow, int &rdx, int &rdy, int mode)
{
	rdx = 0;
	rdy = 0;

	int mdx = 0;
	int mdy = 0;
	bool xNotAdded = true;
	bool yNotAdded = true;
	CSnapInfo snapInfo(snapWindow->m_x, snapWindow->m_y, snapWindow->m_w, snapWindow->m_h);
	CSnapInfo mgrSnapInfo(0, 0, m_w, m_h);
	mgrSnapInfo.Reverse();
	mgrSnapInfo.GetDeltaSnap(&snapInfo, mdx, mdy);

	if (mdx) {
		xNotAdded = false;
		rdx = mdx;
	}
	if (mdy) {
		yNotAdded = false;
		rdy = mdy;
	}

	if (!xNotAdded && !yNotAdded)
		return;

	int dx = 0;
	int dy = 0;
	bool AmIMaster = g_snapMgr.IsMasterSnap(snapWindow);

	std::map<UIWindow *, CSnapInfo>::iterator it = m_snapInfo.begin();
	while (it != m_snapInfo.end()) {
		bool isConnected = g_snapMgr.IsConnected(snapWindow, it->first);
		bool windowIsMaster = g_snapMgr.IsMasterSnap(it->first);
		bool notSameWindow = (snapWindow != it->first);
		bool windowSnapedToMaster = g_snapMgr.IsWindowSnapedToMaster(it->first);

		bool doSnap;
		if (mode)
			doSnap = notSameWindow && !windowIsMaster && !isConnected && !windowSnapedToMaster;
		else
			doSnap = notSameWindow && (!AmIMaster || (!windowSnapedToMaster && !isConnected));

		if (doSnap) {
			it->second.GetDeltaSnap(&snapInfo, dx, dy);
			if (dx && xNotAdded) {
				xNotAdded = false;
				rdx = dx;
			}
			if (dy && yNotAdded) {
				yNotAdded = false;
				rdy = dy;
			}
			if (!xNotAdded && !yNotAdded)
				return;
		}
		++it;
	}
}

void UIWindowMgr::UpdateSnapInfo(UIWindow *snapWindow)
{
	if (!g_session.m_isWindow)
		return;
	if (snapWindow == NULL)
		return;

	m_snapInfo[snapWindow].SetInfo(snapWindow->m_x, snapWindow->m_y, snapWindow->m_w, snapWindow->m_h);
}

void UIWindowMgr::DeleteSnapInfo(UIWindow *window)
{
	if (g_session.m_isWindow) {
		std::map<UIWindow *, CSnapInfo>::iterator it = m_snapInfo.find(window);
		if (it != m_snapInfo.end())
			m_snapInfo.erase(it);
	}
}

void UIWindowMgr::MakeWndPointerNull()
{
	m_lastHitWindow = NULL;
	m_captureWindow = NULL;
	m_editWindow = NULL;
	m_modalWindow = NULL;
	m_chatWnd = NULL;
	m_loadingWnd = NULL;
	m_basicInfoWnd = NULL;
	m_itemWnd = NULL;
	m_equipWnd = NULL;
	m_statusWnd = NULL;
	m_optionWnd = NULL;
	m_minimapZoomWnd = NULL;
	m_itemDropCntWnd = NULL;
	m_sayDialogWnd = NULL;
	m_chooseWnd = NULL;
	m_itemIdentifyWnd = NULL;
	m_itemCompositionWnd = NULL;
	m_chooseWarpWnd = NULL;
	m_menu = NULL;
	m_comboBoxWnd = NULL;
	m_itemCollectionWnd = NULL;
	m_combinedCardItemCollectionWnd = NULL;
	m_itemParamChangeDisplayWnd = NULL;
	m_skillDescribeWnd = NULL;
	m_questDetailWnd = NULL;
	m_quitWnd = NULL;
	m_restartWnd = NULL;
	m_cardItemIllustWnd = NULL;
	m_notifyLevelUpWnd = NULL;
	m_notifyJobLevelUpWnd = NULL;
	m_notifyQuestWnd = NULL;
	m_itemShopWnd = NULL;
	m_itemPurchaseWnd = NULL;
	m_itemSellWnd = NULL;
	m_chooseSellBuyWnd = NULL;
	m_chatRoomMakeWnd = NULL;
	m_chatRoomChangeWnd = NULL;
	m_chatRoomWnd = NULL;
	m_passwordWnd = NULL;
	m_exchangeWnd = NULL;
	m_exchangeAcceptWnd = NULL;
	m_itemStoreWnd = NULL;
	m_skillListWnd = NULL;
	m_messengerGroupWnd = NULL;
	m_joinPartyAcceptWnd = NULL;
	m_CoupleAcceptWnd = NULL;
	m_StarPlaceAcceptWnd = NULL;
	m_YourItemWnd = NULL;
	m_BabyAcceptWnd = NULL;
	m_BabyAcceptWnd2 = NULL;
	m_joinGuildAcceptWnd = NULL;
	m_allyGuildAcceptWnd = NULL;
	m_shortCutWnd = NULL;
	m_tipOfTheDayWnd = NULL;
	m_merchantItemWnd = NULL;
	m_merchantMirrorItemWnd = NULL;
	m_merchantShopMakeWnd = NULL;
	m_merchantItemShopWnd = NULL;
	m_merchantItemPurchaseWnd = NULL;
	m_merchantItemMyShopWnd = NULL;
	m_candidateWnd = NULL;
	m_compositionWnd = NULL;
	m_keyStrokeWnd = NULL;
	m_partySettingWnd = NULL;
	m_skillNameChangeWnd = NULL;
	m_npcEditDialogWnd = NULL;
	m_npcTextEditDialogWnd = NULL;
	m_detailLevelWnd = NULL;
	m_notifyItemObtainWnd = NULL;
	m_guildInfoManageWnd = NULL;
	m_guildMemberManageWnd = NULL;
	m_guildPositionManageWnd = NULL;
	m_guildSkillWnd = NULL;
	m_guildBanishedMemberWnd = NULL;
	m_guildNoticeWnd = NULL;
	m_guildTotalInfoWnd = NULL;
	m_friendOptionWnd = NULL;
	m_guildLeaveReasonDescWnd = NULL;
	m_guildBanReasonDescWnd = NULL;
	m_illustWnd = NULL;
	m_petInfoWnd = NULL;
	m_selectPetEggWnd = NULL;
	m_petTamingDeceiveWnd = NULL;
	m_itemRepairWnd = NULL;
	m_makingArrowListWnd = NULL;
	m_selectCartWnd = NULL;
	m_emotionWnd = NULL;
	m_emotionListWnd = NULL;
	m_bookWnd = NULL;
	m_joinFriendAcceptWnd = NULL;
	m_commandListWnd = NULL;
	m_homunInfoWnd = NULL;
	m_homunSkillListWnd = NULL;
	m_merInfoWnd = NULL;
	m_merSkillListWnd = NULL;
	m_autoMessageWnd = NULL;
	m_mailListWnd = NULL;
	m_mailViewWnd = NULL;
	m_auctionWnd = NULL;
	m_auctionMsgWnd = NULL;
	m_imgMenuWnd = NULL;
	m_selectCharWnd = NULL;
	m_selCharForUServerWnd = NULL;
	m_rebirthWnd = NULL;
	m_subChatWndST = NULL;
	m_subChatWndBT = NULL;
	m_subChatMiniWnd = NULL;
	m_battleMsgOpionWnd = NULL;
	m_memorialDunWnd = NULL;
	m_otherUserEquipWnd = NULL;
	m_roMapWnd = NULL;
}















UIWindowMgr::UIWindowMgr()
{
	MakeWndPointerNull();
	m_lastMouseX = -1;
	m_lastMouseY = -1;
	m_wallpaperSurface = NULL;
	m_w = 0;
	m_h = 0;
	m_chatMsg = "";
	m_chatWndStatus = 7;
	m_chatWndHeight = 98;
	m_chatWndX = 0;
	m_chatWndY = 0;
	m_chatWndShow = 1;
	RestoreChatWndStatus();
	m_miniMapZoomFactor = 4.0f;
	m_miniMapArgb = 0xaaffffff;
	m_isDrawCompass = 1;
	m_chatWndWhisperCurHis = 0;
	m_chatWndCurHis = 0;
	m_isInvalidatedByForce = 0;
	m_conversionMode = (g_languageType == LanguageJapanese);
	ChatWndStickOnOff(1);
	m_onBtSubChat = 0;
	m_onStSubChat = 0;

	if (g_serviceType == ServiceKorea) {
		T_Logo_X = 0;
		T_Logo_Y = -25;
		T_Char_X = 0;
		T_Char_Y = 0;
		T_Tape_Y = 0;
		T_12Age_X = 0;
		T_12Age_Y = 0;
		T_Grav_X = 0;
		T_Grav_Y = 0;
		T_Intel_X = 0;
		T_Intel_Y = 0;
		T_CLICK = 0;
	}
	else {
		T_Logo_X = 0;
		T_Logo_Y = 0;
		T_Char_X = 0;
		T_Char_Y = 0;
		T_Tape_Y = 0;
		T_12Age_X = 0;
		T_12Age_Y = 0;
		T_Grav_X = 0;
		T_Grav_Y = 0;
		T_Intel_X = 0;
		T_Intel_Y = 0;
		T_CLICK = 0;
	}
}

UIWindowMgr::~UIWindowMgr()
{
	RemoveAllWindows();
	DestroyDC();
}

void UIWindowMgr::SetSize(int cx, int cy)
{
	m_w = cx;
	m_h = cy;
	m_chatWndX = 0;
	m_chatWndY = cy - m_chatWndHeight;
}

void UIWindowMgr::CreateDCA(int cx, int cy) {}

void UIWindowMgr::DestroyDC()
{
	if (m_wallpaperSurface) {
		delete m_wallpaperSurface;
		m_wallpaperSurface = NULL;
	}
}

void UIWindowMgr::ClearDC(unsigned long color)
{
	if (m_wallpaperSurface) { delete m_wallpaperSurface; m_wallpaperSurface = NULL; }
}

void UIWindowMgr::AddWindow(UIWindow* window)
{
	m_children.push_back(window);
}

void UIWindowMgr::AddWindowFront(UIWindow* window)
{
	m_children.push_front(window);
}

void UIWindowMgr::RemoveWindow(UIWindow *window)
{
	if (window == NULL)
		return;

	std::list<UIWindow *>::iterator it = m_children.begin();
	while (it != m_children.end())
	{
		if (*it == window)
		{
			if (g_session.m_isWindow)
				DeleteSnapInfo(window);

			g_snapMgr.RemoveSnapObject(*it);
			(*it)->StoreInfo();
			delete *it;
			m_children.erase(it);
			return;
		}
		++it;
	}
}

void UIWindowMgr::HideChatWnd()
{
	if (m_chatWnd)
		m_chatWnd->SetShow(0);
}





void UIWindowMgr::RemoveAllWindowsExceptChatWnd()
{
	if (m_chatWnd != NULL)
	{
		std::list<UIWindow *>::iterator it = m_children.begin();
		while (it != m_children.end())
		{
			if ((UIWindow *)m_chatWnd != (*it) &&
				(UIWindow *)m_subChatWndST != (*it) &&
				(UIWindow *)m_subChatWndBT != (*it) &&
				(UIWindow *)m_subChatMiniWnd != (*it))
			{
				if ((*it) == (UIWindow *)m_memorialDunWnd && g_session.m_onDunInfoWnd)
				{
					g_session.dunInfoWndX = (*it)->m_x;
					g_session.dunInfoWndY = (*it)->m_y;
				}
				(*it)->StoreInfo();
				delete (*it);
				it = m_children.erase(it);
			}
			else
				++it;
		}

		it = m_quitWindow.begin();
		while (it != m_quitWindow.end())
		{
			if ((UIWindow *)m_chatWnd != (*it) &&
				(UIWindow *)m_subChatWndST != (*it) &&
				(UIWindow *)m_subChatWndBT != (*it) &&
				(UIWindow *)m_subChatMiniWnd != (*it))
			{
				it = m_quitWindow.erase(it);
			}
			else
				++it;
		}

		std::map<UIWindow *, CSnapInfo>::iterator snapIt = m_snapInfo.begin();
		while (snapIt != m_snapInfo.end())
		{

			if ((UIWindow *)m_chatWnd != snapIt->first &&
				(UIWindow *)m_subChatWndST != *it &&
				(UIWindow *)m_subChatWndBT != *it &&
				(UIWindow *)m_subChatMiniWnd != *it)
			{
				snapIt = m_snapInfo.erase(snapIt);
			}
			else
				++snapIt;
		}

		UISubChatHisWnd *subChatWndST = NULL;
		UISubChatHisWnd *subChatWndBT = NULL;
		UISubChatMiniWnd *subChatMiniWnd = NULL;
		UINewChatWnd *chatWnd = NULL;
		if (m_subChatWndST != NULL)
			subChatWndST = m_subChatWndST;
		if (m_subChatWndBT != NULL)
			subChatWndBT = m_subChatWndBT;
		if (m_subChatMiniWnd != NULL)
			subChatMiniWnd = m_subChatMiniWnd;
		if (m_chatWnd != NULL)
			chatWnd = m_chatWnd;

		MakeWndPointerNull();
		m_chatWnd = chatWnd;
		m_subChatWndST = subChatWndST;
		m_subChatWndBT = subChatWndBT;
		m_subChatMiniWnd = subChatMiniWnd;
	}
	else
		RemoveAllWindows();
}

void UIWindowMgr::RemoveAllWindows()
{
	std::list<UIWindow *>::iterator it = m_children.begin();
	while (it != m_children.end())
	{
		(*it)->StoreInfo();
		delete *it;
		++it;
	}

	m_children.erase(m_children.begin(), m_children.end());
	m_quitWindow.erase(m_quitWindow.begin(), m_quitWindow.end());
	m_snapInfo.erase(m_snapInfo.begin(), m_snapInfo.end());
	MakeWndPointerNull();
}

void UIWindowMgr::MakeTopLayer(UIWindow* window)
{
	if (m_roMapWnd)
		return;

	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if (*i == window) {
			m_children.erase(i);
			m_children.push_back(window);
			return;
		}
		i++;
	}
}

void UIWindowMgr::InvalidateAll()
{
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		(*i)->InvalidateChildren();
		++i;
	}
}

void UIWindowMgr::InvalidateUpdateNeededUI()
{
	if (m_isInvalidatedByForce)
		return;
	m_isInvalidatedByForce = 1;
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsUpdateNeed())
			(*i)->InvalidateChildren();
		++i;
	}
}

bool UIWindowMgr::Drop(int dropX, int dropY, const DRAG_INFO* const dragInfo)
{
	UIWindow* hit = HitTest(dropX, dropY);
	if (hit) {
		int px, py;
		GetWindowPos(hit, &px, &py);
		hit->DragAndDrop(dropX - px, dropY - py, dragInfo);
		return true;
	}
	return false;
}
void UIWindowMgr::EditFocusToChatWnd()
{
	if (m_chatWnd)
		SetFocusEdit((UIWindow*)m_chatWnd->m_commonChat);
}



int UIWindowMgr::ProcessInput()
{
	int rValue = 0;
	int x = g_mouse.GetXPos();
	int y = g_mouse.GetYPos();

	if (m_modalWindow == NULL) {
		std::list<UIWindow *>::iterator it = m_quitWindow.begin();
		while (it != m_quitWindow.end()) {
			UIWindow *window = *it;

			if (m_captureWindow)
			{
				if (m_captureWindow == window || m_captureWindow->IsChildOf(window))
					m_captureWindow = NULL;
			}
			if (m_editWindow)
			{
				if (m_editWindow == window || m_editWindow->IsChildOf(window))
					m_editWindow = NULL;
			}
			if (m_modalWindow)
			{
				if (m_modalWindow == window || m_modalWindow->IsChildOf(window))
					m_modalWindow = NULL;
			}
			if (m_lastHitWindow)
			{
				if (m_lastHitWindow == window || m_lastHitWindow->IsChildOf(window))
					m_lastHitWindow = NULL;
			}

			RemoveWindow(window);
			it++;
		}
		m_quitWindow.erase(m_quitWindow.begin(), m_quitWindow.end());
	}

	if (m_captureWindow) {
		rValue = 1;
		int gx, gy;
		GetWindowPos(m_captureWindow, &gx, &gy);

		if (g_mouse.GetLBtn() == BTN_DOWN) {
			UIWindow *ancestor = m_captureWindow->GetAncestor();
			if (ancestor->IsFrameWnd())
				MakeTopLayer(ancestor);
			m_captureWindow->OnLBtnDown(x - gx, y - gy);
		}

		if (g_mouse.GetLBtn() == BTN_DBLCLK) {
			m_captureWindow->OnLBtnDown(x - gx, y - gy);
			m_captureWindow->OnLBtnDblClk(x - gx, y - gy);
		}

		m_captureWindow->OnMouseShape(x - gx, y - gy);

		if (m_lastMouseX != x || m_lastMouseY != y)
			m_captureWindow->OnMouseMove(x - gx, y - gy);
		else
			m_captureWindow->OnMouseHover(x - gx, y - gy);

		if (g_mouse.GetWheel())
			GetCapture()->OnWheel(g_mouse.GetWheel());

		if (g_mouse.GetLBtn() == BTN_UP)
			m_captureWindow->OnLBtnUp(x - gx, y - gy);
	}
	else {
		UIWindow *hit = HitTest(x, y);

		if (m_modalWindow && hit) {
			if (hit != m_modalWindow && !hit->IsChildOf(m_modalWindow))
				hit = NULL;
		}

		if (hit) {
			rValue = hit->IsTransmitMouseInput() ? 2 : 1;

			int gx, gy;
			GetWindowPos(hit, &gx, &gy);

			if (g_mouse.GetLBtn() == BTN_DBLCLK)
				hit->OnLBtnDblClk(x - gx, y - gy);

			if (g_mouse.GetRBtn() == BTN_DBLCLK)
				hit->OnRBtnDblClk(x - gx, y - gy);

			if (g_mouse.GetLBtn() == BTN_DOWN) {
				UIWindow *ancestor = hit->GetAncestor();
				if (ancestor->IsFrameWnd())
					MakeTopLayer(ancestor);
				hit->OnLBtnDown(x - gx, y - gy);
			}

			if (g_mouse.GetRBtn() == BTN_DOWN) {
				UIWindow *ancestor = hit->GetAncestor();
				if (ancestor->IsFrameWnd())
					MakeTopLayer(ancestor);
				hit->OnRBtnDown(x - gx, y - gy);
			}

			hit->OnMouseShape(x - gx, y - gy);

			if (m_lastMouseX != x || m_lastMouseY != y)
				hit->OnMouseMove(x - gx, y - gy);
			else
				hit->OnMouseHover(x - gx, y - gy);

			if (g_mouse.GetWheel())
				hit->OnWheel(g_mouse.GetWheel());

			if (g_mouse.GetRBtn() == BTN_UP)
				hit->OnRBtnUp(x - gx, y - gy);

			if (g_mouse.GetLBtn() == BTN_UP)
				hit->OnLBtnUp(x - gx, y - gy);
		}

		g_mouse_L = g_mouse.GetLBtn();

		if (g_mouse_L == BTN_DOWN) {
			if (m_comboBoxWnd) {
				if (hit) {
					if (hit != (UIWindow *)m_comboBoxWnd && !hit->IsChildOf((UIWindow *)m_comboBoxWnd))
						DeleteWindow(WID_COMBOBOXWND);
				}
				else
					DeleteWindow(WID_COMBOBOXWND);
			}
		}

		if (m_selectCharWnd)
			m_selectCharWnd->Invalidate();

		if (m_equipWnd)
			m_equipWnd->Invalidate();

		if (hit) {
			if (m_lastHitWindow) {
				if (hit != m_lastHitWindow) {
					int gx = 0, gy = 0;
					m_lastHitWindow->GetGlobalCoor(gx, gy);
					m_lastHitWindow->OnMouseMove(x - gx, y - gy);
					m_lastHitWindow->OnMouseShape(x - gx, y - gy);
				}
			}
			m_lastHitWindow = hit;
		}
	}

	if (GetFocusEdit())
		GetFocusEdit()->Invalidate();

	if (x == m_lastMouseX && y == m_lastMouseY)
		g_mouseMoved = false;
	else
		g_mouseMoved = true;

	m_lastMouseX = x;
	m_lastMouseY = y;

	if (m_modalWindow)
		return 1;
	return rValue;
}

void UIWindowMgr::ClearAllWindowState()
{
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		(*i)->ClearAllChildrenState();
		++i;
	}
}

void UIWindowMgr::OnProcess()
{
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		(*i)->OnProcess();
		++i;
	}
}

void UIWindowMgr::Render()
{
	BOXINFO boxInfo;
	std::list<UIWindow *>::iterator i = m_children.begin();

	for (; i != m_children.end(); ++i) {
		UIWindow* window = *i;
		if (window->m_show) {
			window->DoDraw(0);
			if (window->GetTransBoxInfo(&boxInfo)) {
				DrawBoxScreen(boxInfo.x, boxInfo.y, boxInfo.cx, boxInfo.cy, boxInfo.color);
			} else if (window->GetTransBoxInfo2(&boxInfo)) {
				window->DrawBoxScreen2();
			}

			if (window == m_roMapWnd)
				m_roMapWnd->DrawRoMap();

			window->m_dc->DrawSurface(window->m_x, window->m_y, window->m_w, window->m_h, window->GetColor());
		}
	}
}

void UIWindowMgr::RenderWallPaper()
{
	m_isInvalidatedByForce = 0;

	if (m_wallpaperSurface)
		m_wallpaperSurface->DrawSurfaceStretch(((int)g_3dDevice.GetWidth() - m_w) / 2, ((int)g_3dDevice.GetHeight() - m_h) / 2, m_w, m_h);
}

void UIWindowMgr::RenderTitleGraphic() {}

void UIWindowMgr::DrawBitmapToFrame(int x, int y, CBitmapRes* bitmap)
{
	if (!bitmap)
		return;
	CDC* dc = g_3dDevice.CreateDCBuffer(bitmap->m_width, bitmap->m_height);
	dc->Update(0, 0, bitmap->m_width, bitmap->m_height, bitmap->m_data, true);
	dc->DrawSurface(x, y, bitmap->m_width, bitmap->m_height, 0xffffffff);
	delete dc;
}

void UIWindowMgr::SetWallpaper(CBitmapRes* bitmap)
{
	if (!bitmap) {
		if (m_wallpaperSurface) { delete m_wallpaperSurface; m_wallpaperSurface = NULL; }
		return;
	}
	if (!m_wallpaperSurface)
		m_wallpaperSurface = g_3dDevice.CreateWallPaper(bitmap->m_width, bitmap->m_height);
	if (m_wallpaperSurface)
		m_wallpaperSurface->Update(0, 0, bitmap->m_width, bitmap->m_height, bitmap->m_data, 0, 0);
}

void UIWindowMgr::FindFocusEditAndSetFocus()
{
	std::list<UIWindow *>::iterator it = m_children.begin();
	while (it != m_children.end()) {
		UIWindow* child = *it;
		if (child->m_show) {
			std::list<UIWindow *>::iterator j = child->m_children.begin();
			while (j != child->m_children.end()) {
				UIWindow* w = *j;
				if (w->CanGetFocus()) {
					SetFocusEdit(w);
					return;
				}
				j++;
			}
		}
		it++;
	}
}

bool UIWindowMgr::IsFirstFocusEdit(UIWindow* wnd)
{
	if (!wnd)
		return false;
	if (wnd->m_parent->GetPrevSiblingOf(wnd))
		return false;
	return true;
}

bool UIWindowMgr::IsLastFocusEdit(UIWindow* wnd)
{
	if (!wnd)
		return false;
	if (wnd->m_parent->GetNextSiblingOf(wnd))
		return false;
	return true;
}

void UIWindowMgr::SetFocusEditNext()
{
	if (!m_editWindow)
		return;
	UIWindow* next = m_editWindow->m_parent->GetNextSiblingOf(m_editWindow);
	if (next) {
		if (!next->CanGetFocus())
			return;
		SetFocusEdit(next);
	} else {
		SetFocusEdit(NULL);
	}
}

void UIWindowMgr::SetFocusEditFirst()
{
	if (m_editWindow) {
		UIWindow* parent = m_editWindow->m_parent;
		std::list<UIWindow *>::iterator i = parent->m_children.begin();
		while (i != parent->m_children.end()) {
			UIWindow* w = *i;
			if (w->CanGetFocus()) {
				SetFocusEdit(w);
				return;
			}
			++i;
		}
	}
	SetFocusEdit(NULL);
}

void UIWindowMgr::SetFocusEditPrev()
{
	if (!m_editWindow)
		return;
	UIWindow* prev = m_editWindow->m_parent->GetPrevSiblingOf(m_editWindow);
	if (prev) {
		if (!prev->CanGetFocus())
			return;
		SetFocusEdit(prev);
	} else {
		SetFocusEdit(NULL);
	}
}

void UIWindowMgr::SetFocusEditLast()
{
	if (m_editWindow) {
		UIWindow* parent = m_editWindow->m_parent;
		std::list<UIWindow *>::reverse_iterator i = parent->m_children.rbegin();
		while (i != parent->m_children.rend()) {
			UIWindow* w = *i;
			if (w->CanGetFocus()) {
				SetFocusEdit(w);
				return;
			}
			i++;
		}
	}
	SetFocusEdit(NULL);
}

void UIWindowMgr::FinishEdit()
{
	if (m_editWindow)
		m_editWindow->OnFinishEdit();
}

void UIWindowMgr::SetFocusEdit(UIWindow* wnd)
{
	if (m_editWindow)
		m_editWindow->OnFinishEdit();
	if (wnd != (UIWindow*)m_chatWnd->m_commonChat) {
		m_editWindow = wnd;
		if (wnd)
			wnd->OnBeginEdit();
	}
}

UIWindow* UIWindowMgr::GetFocusEdit() { return m_editWindow; }

void UIWindowMgr::SetCapture(UIWindow* window)
{
	m_captureWindow = window;
}

UIWindow* UIWindowMgr::GetCapture() { return m_captureWindow; }

void UIWindowMgr::ReleaseCapture()
{
	m_captureWindow = NULL;
}

void UIWindowMgr::DeleteButton()
{
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_DELETE, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::PageUpButton()
{
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_PAGEUPPUSH, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::LeftButton()
{
	if (m_changeNameWnd) {
		m_changeNameWnd->SendMsg(NULL, UM_LEFTPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_bookWnd) {
		m_bookWnd->SendMsg(NULL, UM_LEFTPUSH, 0, 0, 0, 0);
		return;
	}
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_LEFTPUSH, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::RightButton()
{
	if (m_changeNameWnd) {
		m_changeNameWnd->SendMsg(NULL, UM_RIGHTPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_bookWnd) {
		m_bookWnd->SendMsg(NULL, UM_RIGHTPUSH, 0, 0, 0, 0);
		return;
	}
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_RIGHTPUSH, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::UpButton()
{
	if (m_chooseWnd) {
		m_chooseWnd->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_sayDialogWnd) {
		m_sayDialogWnd->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_comboBoxWnd) {
		m_comboBoxWnd->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_chooseWarpWnd) {
		m_chooseWarpWnd->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_metalListWnd) {
		((UIWindow*)m_metalListWnd)->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_editWindow) {
		m_editWindow->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return;
	}
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::DownButton()
{
	if (m_chooseWnd) {
		m_chooseWnd->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_sayDialogWnd) {
		m_sayDialogWnd->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_comboBoxWnd) {
		m_comboBoxWnd->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_chooseWarpWnd) {
		m_chooseWarpWnd->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_metalListWnd) {
		((UIWindow*)m_metalListWnd)->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_editWindow) {
		m_editWindow->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return;
	}
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::PageDownButton()
{
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_PAGEDOWNPUSH, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::ToggleButton(int toggleDir)
{
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_TOGGLEPUSH, toggleDir, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::CancelPushButton()
{
	if (m_editWindow) {
		m_editWindow->SendMsg(NULL, UM_CANCELPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_modalWindow) {
		m_modalWindow->SendMsg(NULL, UM_CANCELPUSH, 0, 0, 0, 0);
		return;
	}
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_CANCELPUSH, 0, 0, 0, 0);
		++i;
	}
}

void UIWindowMgr::RefreshItem()
{
	if (m_itemWnd)
		m_itemWnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
	if (m_shortCutWnd)
		m_shortCutWnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIWindowMgr::DefPushButton()
{
	if (m_modalWindow) {
		m_modalWindow->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_quitWnd) {
		m_quitWnd->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_npcEditDialogWnd) {
		m_npcEditDialogWnd->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_npcTextEditDialogWnd) {
		m_npcTextEditDialogWnd->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_chooseWnd) {
		m_chooseWnd->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_sayDialogWnd) {
		m_sayDialogWnd->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_comboBoxWnd) {
		m_comboBoxWnd->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_chooseWarpWnd) {
		m_chooseWarpWnd->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_metalListWnd) {
		((UIWindow*)m_metalListWnd)->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_metalProcessWnd) {
		((UIWindow*)m_metalProcessWnd)->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	if (m_editWindow) {
		m_editWindow->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		return;
	}
	std::list<UIWindow *>::iterator i = m_children.begin();
	while (i != m_children.end()) {
		if ((*i)->IsFrameWnd())
			(*i)->SendMsg(NULL, UM_DEFPUSH, 0, 0, 0, 0);
		++i;
	}
}

UIWindow* UIWindowMgr::HitTest(int x, int y)
{
	std::list<UIWindow *>::reverse_iterator i = m_children.rbegin();
	while (i != m_children.rend()) {
		UIWindow* result = (*i)->HitTest(x, y);
		if (result)
			if (result->m_show)
				return result;
		i++;
	}
	return NULL;
}

void UIWindowMgr::GetWindowPos(UIWindow* window, int* x, int* y)
{
	int sx = 0;
	int sy = 0;
	while (window) {
		sx += window->m_x;
		sy += window->m_y;
		window = window->m_parent;
	}
	*x = sx;
	*y = sy;
}

void UIWindowMgr::PostQuit(UIWindow* wnd)
{
	if (wnd == (UIWindow*)m_selectCharWnd)
		m_selectCharWnd = NULL;
	if (wnd == (UIWindow*)m_selCharForUServerWnd)
		m_selCharForUServerWnd = NULL;
	if (wnd == (UIWindow*)m_changeNameWnd)
		m_changeNameWnd = NULL;
	m_quitWindow.push_back(wnd);
}



int UIWindowMgr::DoModal(UIWindow *wnd)
{
	m_modalResult = 0;
	while (!g_sysQuit) {
		if (m_modalResult) {
			PostQuit(wnd);
			return m_modalResult;
		}

		if (g_modeMgr.GetCurMode())
			g_modeMgr.GetCurMode()->OnUpdate();
	}

	return ID_OK;
}

void UIWindowMgr::AddFriendCharName(const char* friendCharName)
{
	PACKET_CZ_ADD_FRIENDS p;
	p.PacketType = HEADER_CZ_ADD_FRIENDS;
	memcpy(p.name, friendCharName, 24);
	CRagConnection::instanceR().SendPacket(
		CRagConnection::instanceR().GetPacketSize(p.PacketType),
		(const char*)&p);
}






void UIWindowMgr::AddWhisperCharName(char const *whisperCharName)
{
	if (m_chatWnd) {
		std::vector<std::string> &hisChat = m_chatWnd->m_whisperChat->m_hisChat;

		std::vector<std::string>::iterator it = hisChat.begin();
		while (it != hisChat.end()) {
			if (*it == whisperCharName)
				return;

			it++;
		}

		hisChat.push_back(whisperCharName);
		if (m_chatWnd->m_whisperChat->m_curHis >= hisChat.size() - 1)
			m_chatWnd->m_whisperChat->m_curHis = hisChat.size();
	}
}

void UIWindowMgr::AddWhisperCharName2(char const *whisperCharName)
{
	int curHis = 0;
	if (m_chatWnd) {
		std::vector<std::string> &whisperHisChat = m_chatWnd->m_whisperChat->m_hisChat;

		std::vector<std::string>::iterator it = whisperHisChat.begin();
		while (it != whisperHisChat.end()) {
			if (*it == whisperCharName)
				goto found;

			it++;
			curHis++;
		}

		whisperHisChat.push_back(whisperCharName);

found:
		m_chatWnd->m_whisperChat->m_curHis = curHis;
		m_chatWnd->SendMsg(m_chatWnd->m_whisperChat, UM_COMBOBOX_SELECTED, curHis, 0, 0, 0);
	}
}

void UIWindowMgr::StoreChatWndStatus() {}

void UIWindowMgr::RestoreChatWndStatus() {}

int UIWindowMgr::ErrorMsg(char const *msg, int type, int isDefYes, int changeMsg, unsigned long autoReturnTime)
{
	Trace("windowMgr.ErrorMsg==%s", msg);
	if (m_modalWindow) {
		if (changeMsg)
			m_modalWindow->SendMsg(NULL, UM_CHANGEMSG, (int)msg, 0, 0, 0);
		return ID_CANCEL;
	}

	UIMessageBox *messageBox = new UIMessageBox;
	messageBox->Create(msg, type, isDefYes, autoReturnTime);
	AddWindow(messageBox);
	ReleaseCapture();
	SetFocusEdit(NULL);
	m_modalWindow = messageBox;
	int result = DoModal(messageBox);

	m_modalWindow = NULL;
	FindFocusEditAndSetFocus();
	return result;
}

int UIWindowMgr::ErrorMsgAutoreturn(char const *msg, unsigned long autoReturnTime)
{
	if (m_modalWindow)
		return ID_CANCEL;

	UIMessageBoxAutoreturn *messageBox = new UIMessageBoxAutoreturn;
	messageBox->Create(msg, autoReturnTime);
	AddWindow(messageBox);
	ReleaseCapture();
	SetFocusEdit(NULL);
	m_modalWindow = messageBox;
	int result = DoModal(messageBox);

	m_modalWindow = NULL;
	FindFocusEditAndSetFocus();
	return result;
}

int UIWindowMgr::EmailAddress()
{
	if (m_modalWindow)
		return ID_CANCEL;

	UIEmailAddressWnd *emailWnd = new UIEmailAddressWnd;
	emailWnd->Create(280, 48);
	emailWnd->Move((g_renderer->m_width - 640) / 2 + 185, (g_renderer->m_height - 480) / 2 + 180);
	AddWindow(emailWnd);
	m_captureWindow = NULL;
	m_modalWindow = emailWnd;
	int result = DoModal(emailWnd);

	m_modalWindow = NULL;
	FindFocusEditAndSetFocus();
	return result;
}

UIWindow * UIWindowMgr::QueryWindow(WINDOWID windowId)
{
	switch (windowId)
	{
	case WID_LOGINWND:
		return (UIWindow *)m_loginWnd;
	case WID_CHATWND:
		return (UIWindow *)m_chatWnd;
	case WID_CHATROOMMAKEWND:
		return (UIWindow *)m_chatRoomMakeWnd;
	case WID_CHATROOMCHANGEWND:
		return (UIWindow *)m_chatRoomChangeWnd;
	case WID_CHATROOMWND:
		return (UIWindow *)m_chatRoomWnd;
	case WID_MESSENGERGROUPWND:
		return (UIWindow *)m_messengerGroupWnd;
	case WID_OPTIONWND:
		return (UIWindow *)m_optionWnd;
	case WID_SKILLLISTWND:
		return (UIWindow *)m_skillListWnd;
	case WID_QUESTWND:
		return (UIWindow *)m_questWnd;
	case WID_QUEST_DETAIL_WND:
		return (UIWindow *)m_questDetailWnd;
	case WID_GUILDMEMBERMANAGEWND:
		return (UIWindow *)m_guildMemberManageWnd;
	case WID_GUILDINFOMANAGEWND:
		return (UIWindow *)m_guildInfoManageWnd;
	case WID_PETINFOWND:
		return (UIWindow *)m_petInfoWnd;
	case WID_GUILDSKILLWND:
		return (UIWindow *)m_guildSkillWnd;
	case WID_PROHIBITLISTWND:
		return (UIWindow *)m_prohibitListWnd;
	case WID_HOMUNINFOWND:
		return (UIWindow *)m_homunInfoWnd;
	case WID_HOMUNSKILLLISTWND:
		return (UIWindow *)m_homunSkillListWnd;
	case WID_MERINFOWND:
		return (UIWindow *)m_merInfoWnd;
	case WID_MERSKILLLISTWND:
		return (UIWindow *)m_merSkillListWnd;
	case WID_AUCTIONWND:
		return (UIWindow *)m_auctionWnd;
	default:
		return NULL;
	}
}

#include "rag_budget_fill.h"
bool UIWindowMgr::DeleteWindow(WINDOWID windowId)
{
	if (m_modalWindow)
		return false;
	switch (windowId) {
	case WID_CHATWND:
		DeleteWindow(WID_COMBOBOXWND);
		if (!m_chatWnd) return false;
		PostQuit(m_chatWnd);
		m_chatWnd = NULL;
		m_chatWndShow = 0;
		return true;
	case WID_ITEMWND:
		if (!m_itemWnd) return false;
		PostQuit(m_itemWnd);
		m_itemWnd = NULL;
		itemWndInfo.show = 0;
		return true;
	case WID_QUESTWND:
		if (!m_questWnd) return false;
		PostQuit(m_questWnd);
		m_questWnd = NULL;
		questWndInfo.show = 0;
		return true;
	case WID_EQUIPWND:
		if (!m_equipWnd) return false;
		PostQuit(m_equipWnd);
		m_equipWnd = NULL;
		equipWndInfo.show = 0;
		return true;
	case WID_STATUSWND:
		if (!m_statusWnd) return false;
		PostQuit(m_statusWnd);
		m_statusWnd = NULL;
		statusWndInfo.show = 0;
		return true;
	case WID_ITEMDROPCNTWND:
		if (!m_itemDropCntWnd) return false;
		PostQuit(m_itemDropCntWnd);
		m_itemDropCntWnd = NULL;
		return true;
	case WID_SAYDIALOGWND:
		if (!m_sayDialogWnd) return false;
		PostQuit(m_sayDialogWnd);
		m_sayDialogWnd = NULL;
		return true;
	case WID_CHATROOMWND:
		if (!m_chatRoomWnd) return false;
		PostQuit(m_chatRoomWnd);
		m_chatRoomWnd = NULL;
		return true;
	case WID_CHOOSEWARPWND:
		if (!m_chooseWarpWnd) return false;
		PostQuit(m_chooseWarpWnd);
		m_chooseWarpWnd = NULL;
		return true;
	case WID_CHOOSEWND:
		if (!m_chooseWnd) return false;
		PostQuit(m_chooseWnd);
		m_chooseWnd = NULL;
		return true;
	case WID_ITEMCOMPOSITIONWND:
		if (!m_itemCompositionWnd) return false;
		PostQuit(m_itemCompositionWnd);
		m_itemCompositionWnd = NULL;
		return true;
	case WID_ITEMIDENTIFYWND:
		if (!m_itemIdentifyWnd) return false;
		PostQuit(m_itemIdentifyWnd);
		m_itemIdentifyWnd = NULL;
		return true;
	case WID_COMBOBOXWND:
		if (!m_comboBoxWnd) return false;
		PostQuit(m_comboBoxWnd);
		m_comboBoxWnd = NULL;
		return true;
	case WID_MENUWND:
		if (!m_menu) return false;
		PostQuit(m_menu);
		m_menu = NULL;
		return true;
	case WID_OPTIONWND:
		if (!m_optionWnd) return false;
		PostQuit(m_optionWnd);
		m_optionWnd = NULL;
		optionWndInfo.show = 0;
		return true;
	case WID_CHATROOMCHANGEWND:
		if (!m_chatRoomChangeWnd) return false;
		PostQuit(m_chatRoomChangeWnd);
		m_chatRoomChangeWnd = NULL;
		return true;
	case WID_CHATROOMMAKEWND:
		if (!m_chatRoomMakeWnd) return false;
		PostQuit(m_chatRoomMakeWnd);
		m_chatRoomMakeWnd = NULL;
		return true;
	case WID_CHOOSESELLBUYWND:
		if (!m_chooseSellBuyWnd) return false;
		PostQuit(m_chooseSellBuyWnd);
		m_chooseSellBuyWnd = NULL;
		return true;
	case WID_ITEMSELLWND:
		if (!m_itemSellWnd) return false;
		PostQuit(m_itemSellWnd);
		m_itemSellWnd = NULL;
		return true;
	case WID_MERCHANTITEMPURCHASEWND:
		if (!m_merchantItemPurchaseWnd) return false;
		PostQuit(m_merchantItemPurchaseWnd);
		m_merchantItemPurchaseWnd = NULL;
		return true;
	case WID_ITEMPURCHASEWND:
		if (!m_itemPurchaseWnd) return false;
		PostQuit(m_itemPurchaseWnd);
		m_itemPurchaseWnd = NULL;
		return true;
	case WID_MERCHANTITEMMYSHOPWND:
		if (!m_merchantItemMyShopWnd) return false;
		PostQuit(m_merchantItemMyShopWnd);
		m_merchantItemMyShopWnd = NULL;
		return true;
	case WID_MERCHANTITEMSHOPWND:
		if (!m_merchantItemShopWnd) return false;
		PostQuit(m_merchantItemShopWnd);
		m_merchantItemShopWnd = NULL;
		return true;
	case WID_ITEMSHOPWND:
		if (!m_itemShopWnd) return false;
		PostQuit(m_itemShopWnd);
		m_itemShopWnd = NULL;
		return true;
	case WID_NOTIFYJOBLEVELUPWND:
		if (!m_notifyJobLevelUpWnd) return false;
		PostQuit(m_notifyJobLevelUpWnd);
		m_notifyJobLevelUpWnd = NULL;
		return true;
	case WID_NOTIFYLEVELUPWND:
		if (!m_notifyLevelUpWnd) return false;
		PostQuit(m_notifyLevelUpWnd);
		m_notifyLevelUpWnd = NULL;
		return true;
	case WID_NOTIFYQUESTWND:
		if (!m_notifyQuestWnd) return false;
		PostQuit(m_notifyQuestWnd);
		m_notifyQuestWnd = NULL;
		return true;
	case WID_CARDITEMILLUSTWND:
		if (!m_cardItemIllustWnd) return false;
		PostQuit(m_cardItemIllustWnd);
		m_cardItemIllustWnd = NULL;
		return true;
	case WID_RESTARTWND:
		if (!m_restartWnd) return false;
		PostQuit(m_restartWnd);
		m_restartWnd = NULL;
		return true;
	case WID_QUITWND:
		if (!m_quitWnd) return false;
		PostQuit(m_quitWnd);
		m_quitWnd = NULL;
		return true;
	case WID_SKILLDESCRIBEWND:
		if (!m_skillDescribeWnd) return false;
		PostQuit(m_skillDescribeWnd);
		m_skillDescribeWnd = NULL;
		return true;
	case WID_QUEST_DETAIL_WND:
		if (!m_questDetailWnd) return false;
		PostQuit(m_questDetailWnd);
		m_questDetailWnd = NULL;
		return true;
	case WID_ITEMPARAMCHANGEDISPLAYWND:
		if (!m_itemParamChangeDisplayWnd) return false;
		PostQuit(m_itemParamChangeDisplayWnd);
		m_itemParamChangeDisplayWnd = NULL;
		return true;
	case WID_COMBINEDCARDITEMCOLLECTIONWND:
		if (!m_combinedCardItemCollectionWnd) return false;
		PostQuit(m_combinedCardItemCollectionWnd);
		m_combinedCardItemCollectionWnd = NULL;
		return true;
	case WID_ITEMCOLLECTIONWND:
		if (!m_itemCollectionWnd) return false;
		PostQuit(m_itemCollectionWnd);
		m_itemCollectionWnd = NULL;
		return true;
	case WID_MINIMAPZOOMWND:
		if (!m_minimapZoomWnd) return false;
		PostQuit(m_minimapZoomWnd);
		m_minimapZoomWnd = NULL;
		return true;
	case WID_SKILLLISTWND:
		if (!m_skillListWnd) return false;
		PostQuit(m_skillListWnd);
		m_skillListWnd = NULL;
		skillListWndInfo.show = 0;
		return true;
	case WID_ITEMSTOREWND:
		if (!m_itemStoreWnd) return false;
		PostQuit(m_itemStoreWnd);
		m_itemStoreWnd = NULL;
		return true;
	case WID_MESSENGERWND:
		if (m_messengerGroupWnd)
			((UIWindow*)m_messengerGroupWnd)->StoreInfo();
		if (DeleteWindow(WID_MESSENGERGROUPWND))
			return true;
		break;
	case WID_MESSENGERGROUPWND:
		if (!m_messengerGroupWnd) return false;
		PostQuit(m_messengerGroupWnd);
		m_messengerGroupWnd = NULL;
		messengerGroupWndInfo.show = 0;
		return true;
	case WID_MERCHANTSHOPMAKEWND:
		DeleteWindow(WID_MERCHANTMIRRORITEMWND);
		if (!m_merchantShopMakeWnd) return false;
		PostQuit(m_merchantShopMakeWnd);
		m_merchantShopMakeWnd = NULL;
		return true;
	case WID_MERCHANTMIRRORITEMWND:
		if (!m_merchantMirrorItemWnd) return false;
		PostQuit(m_merchantMirrorItemWnd);
		m_merchantMirrorItemWnd = NULL;
		return true;
	case WID_MERCHANTITEMWND:
		if (!m_merchantItemWnd) return false;
		PostQuit(m_merchantItemWnd);
		m_merchantItemWnd = NULL;
		merchantItemWndInfo.show = 0;
		return true;
	case WID_CANDIDATEWND:
		if (!m_candidateWnd) return false;
		PostQuit(m_candidateWnd);
		m_candidateWnd = NULL;
		return true;
	case WID_COMPOSITIONWND:
		if (!m_compositionWnd) return false;
		PostQuit(m_compositionWnd);
		m_compositionWnd = NULL;
		return true;
	case WID_DETAILLEVELWND:
		if (!m_detailLevelWnd) return false;
		PostQuit(m_detailLevelWnd);
		m_detailLevelWnd = NULL;
		return true;
	case WID_NOTIFYITEMOBTAINWND:
		if (!m_notifyItemObtainWnd) return false;
		PostQuit(m_notifyItemObtainWnd);
		m_notifyItemObtainWnd = NULL;
		return true;
	case WID_NPCEDITDIALOGWND:
		if (!m_npcEditDialogWnd) return false;
		PostQuit(m_npcEditDialogWnd);
		m_npcEditDialogWnd = NULL;
		return true;
	case WID_NPCTEXTEDITDIALOGWND:
		if (!m_npcTextEditDialogWnd) return false;
		PostQuit(m_npcTextEditDialogWnd);
		m_npcTextEditDialogWnd = NULL;
		return true;
	case WID_SKILLNAMECHANGEWND:
		if (!m_skillNameChangeWnd) return false;
		PostQuit(m_skillNameChangeWnd);
		m_skillNameChangeWnd = NULL;
		return true;
	case WID_GUILDWND:
		{
			int allFalse = 1;
			if (g_windowMgr.DeleteWindow(WID_GUILDINFOMANAGEWND)) allFalse = 0;
			if (g_windowMgr.DeleteWindow(WID_GUILDMEMBERMANAGEWND)) allFalse = 0;
			if (g_windowMgr.DeleteWindow(WID_GUILDPOSITIONMANAGEWND)) allFalse = 0;
			if (g_windowMgr.DeleteWindow(WID_GUILDSKILLWND)) allFalse = 0;
			if (g_windowMgr.DeleteWindow(WID_GUILDBANISHEDMEMBERWND)) allFalse = 0;
			if (g_windowMgr.DeleteWindow(WID_GUILDNOTICEWND)) allFalse = 0;
			if (!g_windowMgr.DeleteWindow(WID_GUILDTOTALINFOWND) && allFalse) return false;
			return true;
		}
	case WID_GUILDMEMBERMANAGEWND:
		if (!m_guildMemberManageWnd) return false;
		m_guildMemberManageWnd->StoreInfo();
		PostQuit(m_guildMemberManageWnd);
		m_guildMemberManageWnd = NULL;
		return true;
	case WID_GUILDPOSITIONMANAGEWND:
		if (!m_guildPositionManageWnd) return false;
		m_guildPositionManageWnd->StoreInfo();
		PostQuit(m_guildPositionManageWnd);
		m_guildPositionManageWnd = NULL;
		return true;
	case WID_GUILDSKILLWND:
		if (!m_guildSkillWnd) return false;
		m_guildSkillWnd->StoreInfo();
		PostQuit(m_guildSkillWnd);
		m_guildSkillWnd = NULL;
		return true;
	case WID_GUILDBANISHEDMEMBERWND:
		if (!m_guildBanishedMemberWnd) return false;
		m_guildBanishedMemberWnd->StoreInfo();
		PostQuit(m_guildBanishedMemberWnd);
		m_guildBanishedMemberWnd = NULL;
		return true;
	case WID_GUILDNOTICEWND:
		if (!m_guildNoticeWnd) return false;
		m_guildNoticeWnd->StoreInfo();
		PostQuit(m_guildNoticeWnd);
		m_guildNoticeWnd = NULL;
		return true;
	case WID_GUILDTOTALINFOWND:
		if (!m_guildTotalInfoWnd) return false;
		m_guildTotalInfoWnd->StoreInfo();
		PostQuit(m_guildTotalInfoWnd);
		m_guildTotalInfoWnd = NULL;
		return true;
	case WID_GUILDINFOMANAGEWND:
		if (!m_guildInfoManageWnd) return false;
		m_guildInfoManageWnd->StoreInfo();
		PostQuit(m_guildInfoManageWnd);
		m_guildInfoManageWnd = NULL;
		return true;
	case WID_ILLUSTWND:
		if (!m_illustWnd) return false;
		PostQuit(m_illustWnd);
		m_illustWnd = NULL;
		return true;
	case WID_GUILDBANREASONDESCWND:
		if (!m_guildBanReasonDescWnd) return false;
		PostQuit(m_guildBanReasonDescWnd);
		m_guildBanReasonDescWnd = NULL;
		return true;
	case WID_GUILDLEAVEREASONDESCWND:
		if (!m_guildLeaveReasonDescWnd) return false;
		PostQuit(m_guildLeaveReasonDescWnd);
		m_guildLeaveReasonDescWnd = NULL;
		return true;
	case WID_FRIENDOPTIONWND:
		if (!m_friendOptionWnd) return false;
		PostQuit(m_friendOptionWnd);
		m_friendOptionWnd = NULL;
		return true;
	case WID_PARTYSETTINGWND:
		if (!m_partySettingWnd) return false;
		PostQuit(m_partySettingWnd);
		m_partySettingWnd = NULL;
		return true;
	case WID_TIPOFTHEDAYWND:
		if (!m_tipOfTheDayWnd) return false;
		PostQuit(m_tipOfTheDayWnd);
		m_tipOfTheDayWnd = NULL;
		return true;
	case WID_JOINGUILDACCEPTWND:
		if (!m_joinGuildAcceptWnd) return false;
		PostQuit(m_joinGuildAcceptWnd);
		m_joinGuildAcceptWnd = NULL;
		return true;
	case WID_ALLYGUILDACCEPTWND:
		if (!m_allyGuildAcceptWnd) return false;
		PostQuit(m_allyGuildAcceptWnd);
		m_allyGuildAcceptWnd = NULL;
		return true;
	case WID_JOINPARTYACCEPTWND:
		if (!m_joinPartyAcceptWnd) return false;
		PostQuit(m_joinPartyAcceptWnd);
		m_joinPartyAcceptWnd = NULL;
		return true;
	case WID_COUPLEACCEPTWND:
		if (!m_CoupleAcceptWnd) return false;
		PostQuit(m_CoupleAcceptWnd);
		m_CoupleAcceptWnd = NULL;
		return true;
	case WID_STARPLACE_ACCEPTWND:
		if (!m_StarPlaceAcceptWnd) return false;
		PostQuit(m_StarPlaceAcceptWnd);
		m_StarPlaceAcceptWnd = NULL;
		return true;
	case WID_YOURITEMWND:
		if (!m_YourItemWnd) return false;
		PostQuit(m_YourItemWnd);
		m_YourItemWnd = NULL;
		return true;
	case WID_BABYACCEPTWND:
		if (!m_BabyAcceptWnd) return false;
		PostQuit(m_BabyAcceptWnd);
		m_BabyAcceptWnd = NULL;
		return true;
	case WID_BABYACCEPTWND2:
		if (!m_BabyAcceptWnd2) return false;
		PostQuit(m_BabyAcceptWnd2);
		m_BabyAcceptWnd2 = NULL;
		return true;
	case WID_EXCHANGEACCEPTWND:
		if (!m_exchangeAcceptWnd) return false;
		PostQuit(m_exchangeAcceptWnd);
		m_exchangeAcceptWnd = NULL;
		return true;
	case WID_EXCHANGEWND:
		if (!m_exchangeWnd) return false;
		PostQuit(m_exchangeWnd);
		m_exchangeWnd = NULL;
		return true;
	case WID_PASSWORDWND:
		if (!m_passwordWnd) return false;
		PostQuit(m_passwordWnd);
		m_passwordWnd = NULL;
		return true;
	case WID_MAKETARGETLISTWND:
		if (!m_metalListWnd) return false;
		PostQuit(m_metalListWnd);
		m_metalListWnd = NULL;
		return false;
	case WID_ITEMREPAIRWND:
		if (!m_itemRepairWnd) return false;
		PostQuit(m_itemRepairWnd);
		m_itemRepairWnd = NULL;
		return false;
	case WID_WEAPONREFINEWND:
		if (!m_weaponRefineWnd) return false;
		PostQuit(m_weaponRefineWnd);
		m_weaponRefineWnd = NULL;
		return false;
	case WID_MAKETARGETPROCESSWND:
		if (!m_metalProcessWnd) return false;
		PostQuit(m_metalProcessWnd);
		m_metalProcessWnd = NULL;
		return false;
	case WID_MAKETARGETRESULTWND:
		if (!m_metalResultWnd) return false;
		PostQuit(m_metalResultWnd);
		m_metalResultWnd = NULL;
		return false;
	case WID_KEYSTROKEWND:
		if (!m_keyStrokeWnd) return false;
		PostQuit(m_keyStrokeWnd);
		m_keyStrokeWnd = NULL;
		return false;
	case WID_EMOTIONWND:
		if (!m_emotionWnd) return false;
		PostQuit(m_emotionWnd);
		m_emotionWnd = NULL;
		return true;
	case WID_EMOTIONLISTWND:
		if (!m_emotionListWnd) return false;
		PostQuit(m_emotionListWnd);
		m_emotionListWnd = NULL;
		return true;
	case WID_PETINFOWND:
		if (!m_petInfoWnd) return false;
		PostQuit(m_petInfoWnd);
		m_petInfoWnd = NULL;
		return true;
	case WID_SELECTPETEGGWND:
		if (!m_selectPetEggWnd) return false;
		PostQuit(m_selectPetEggWnd);
		m_selectPetEggWnd = NULL;
		return true;
	case WID_PETTAMINGDECEIVEWND:
		if (!m_petTamingDeceiveWnd) return false;
		PostQuit(m_petTamingDeceiveWnd);
		m_petTamingDeceiveWnd = NULL;
		return true;
	case WID_MAKINGARROWLISTWND:
		if (!m_makingArrowListWnd) return false;
		PostQuit(m_makingArrowListWnd);
		m_makingArrowListWnd = NULL;
		return true;
	case WID_SELECTCARTWND:
		if (!m_selectCartWnd) return false;
		PostQuit(m_selectCartWnd);
		m_selectCartWnd = NULL;
		return true;
	case WID_SPELLLISTWND:
		if (!m_spellListWnd) return false;
		PostQuit(m_spellListWnd);
		m_spellListWnd = NULL;
		return true;
	case WID_PROHIBITLISTWND:
		if (!m_prohibitListWnd) return false;
		PostQuit(m_prohibitListWnd);
		m_prohibitListWnd = NULL;
		return true;
	case WID_BOOKWND:
		if (!m_bookWnd) return false;
		PostQuit(m_bookWnd);
		m_bookWnd = NULL;
		return true;
	case WID_JOINFRIENDACCEPTWND:
		if (!m_joinFriendAcceptWnd) return false;
		PostQuit(m_joinFriendAcceptWnd);
		m_joinFriendAcceptWnd = NULL;
		return true;
	case WID_COMMANDLISTWND:
		if (!m_commandListWnd) return false;
		PostQuit(m_commandListWnd);
		m_commandListWnd = NULL;
		return true;
	case WID_HOMUNINFOWND:
		if (!m_homunInfoWnd) return false;
		PostQuit(m_homunInfoWnd);
		m_homunInfoWnd = NULL;
		return true;
	case WID_HOMUNSKILLLISTWND:
		if (!m_homunSkillListWnd) return false;
		PostQuit(m_homunSkillListWnd);
		m_homunSkillListWnd = NULL;
		homunSkillListWndInfo.show = 0;
		return true;
	case WID_MERINFOWND:
		if (!m_merInfoWnd) return false;
		PostQuit(m_merInfoWnd);
		m_merInfoWnd = NULL;
		return true;
	case WID_MERSKILLLISTWND:
		if (!m_merSkillListWnd) return false;
		PostQuit(m_merSkillListWnd);
		m_merSkillListWnd = NULL;
		merSkillListWndInfo.show = 0;
		return true;
	case WID_AUTOMESSAGEWND:
		if (!m_autoMessageWnd) return false;
		PostQuit(m_autoMessageWnd);
		m_autoMessageWnd = NULL;
		return true;
	case WID_MAILLISTWND:
		if (!m_mailListWnd) return false;
		PostQuit(m_mailListWnd);
		m_mailListWnd = NULL;
		return true;
	case WID_MAILVIEWWND:
		if (!m_mailViewWnd) return false;
		PostQuit(m_mailViewWnd);
		m_mailViewWnd = NULL;
		return true;
	case WID_AUCTIONWND:
		if (!m_auctionWnd) return false;
		PostQuit(m_auctionWnd);
		m_auctionWnd = NULL;
		return true;
	case WID_AUCTIONMSGWND:
		if (!m_auctionMsgWnd) return false;
		PostQuit(m_auctionMsgWnd);
		m_auctionMsgWnd = NULL;
		return true;
	case WID_IMGMENUWND:
		if (!m_imgMenuWnd) return false;
		PostQuit(m_imgMenuWnd);
		m_imgMenuWnd = NULL;
		return true;
	case WID_CHANGENAMEWND:
		RAG_BUDGET_FILL(8)
		if (!m_changeNameWnd) return false;
		PostQuit(m_changeNameWnd);
		m_changeNameWnd = NULL;
		return true;
	case WID_REBIRTHWND:
		if (!m_rebirthWnd) return false;
		PostQuit(m_rebirthWnd);
		m_rebirthWnd = NULL;
		return true;
	case WID_SUBCHATWND_ST:
		if (!m_subChatWndST) return false;
		m_subChatWndST->StoreInfo();
		PostQuit(m_subChatWndST);
		m_subChatWndST = NULL;
		return true;
	case WID_SUBCHATWND_BT:
		if (!m_subChatWndBT) return false;
		m_subChatWndBT->StoreInfo();
		PostQuit(m_subChatWndBT);
		m_subChatWndBT = NULL;
		return true;
	case WID_BATTLEMSG_OPTIONWND:
		if (!m_battleMsgOpionWnd) return false;
		PostQuit(m_battleMsgOpionWnd);
		m_battleMsgOpionWnd = NULL;
		return true;
	case WID_SUBCHAT_MINIWND:
		if (!m_subChatMiniWnd) return false;
		PostQuit(m_subChatMiniWnd);
		m_subChatMiniWnd = NULL;
		return true;
	case WID_MEMORIAL_DUN_WND:
		if (!m_memorialDunWnd) return false;
		m_memorialDunWnd->InitInfo();
		PostQuit(m_memorialDunWnd);
		m_memorialDunWnd = NULL;
		g_session.m_onDunInfoWnd = 0;
		return true;
	case WID_OTHERUSER_EQUIPWND:
		if (!m_otherUserEquipWnd) return false;
		PostQuit(m_otherUserEquipWnd);
		m_otherUserEquipWnd = NULL;
		return true;
	case WID_ROMAPWND:
		if (!m_roMapWnd) return false;
		PostQuit(m_roMapWnd);
		m_roMapWnd = NULL;
		return true;
	default:
		break;
	}
	return false;
}


bool UIWindowMgr::ShowWindow(WINDOWID windowId, int show)
{
	switch (windowId) {
	case WID_CHATWND:
		m_chatWndShow = show;
		if (m_chatWnd == NULL)
			g_windowMgr.MakeWindow(WID_CHATWND);

		m_chatWnd->SetShow(show);
		if (show)
			g_windowMgr.MakeTopLayer((UIWindow *)m_chatWnd);
		else
			m_chatWnd->StoreInfo();
		return true;

	case WID_ITEMDROPCNTWND:
		g_windowMgr.MakeWindow(WID_ITEMDROPCNTWND);
		if (m_itemDropCntWnd->IsShow() != show)
			m_itemDropCntWnd->SetShow(show);

		if (show)
			g_windowMgr.MakeTopLayer((UIWindow *)m_itemDropCntWnd);
		break;

	case WID_SHORTCUTWND:
		shortCutWndInfo.show = show;
		g_windowMgr.MakeWindow(WID_SHORTCUTWND);
		m_shortCutWnd->SetShow(show);
		if (show)
			g_windowMgr.MakeTopLayer((UIWindow *)m_shortCutWnd);
		return true;
	}

	return false;
}


UIFrameWnd * UIWindowMgr::MakeWindow(WINDOWID windowId)
{
	UIFrameWnd *wnd = NULL;
	UIFrameWnd *window;
	int width;
	int height;

	switch (windowId) {
	case WID_BASICINFOWND:
		if (!(wnd = m_basicInfoWnd)) {
			wnd = m_basicInfoWnd = new UIBasicInfoWnd;
			wnd->Create(280, 34);
			wnd->Move(0, 0);
			AddWindow(wnd);
			m_basicInfoWnd->SendMsg(NULL, UM_SETINFO, (int)&basicInfoWndInfo, 0, 0, 0);
			g_snapMgr.AddSnapMasterObject(wnd);
		}
		wnd->SetShow(1);
		if (itemWndInfo.show)
			MakeWindow(WID_ITEMWND);
		if (equipWndInfo.show)
			MakeWindow(WID_EQUIPWND);
		if (statusWndInfo.show)
			MakeWindow(WID_STATUSWND);
		if (optionWndInfo.show)
			MakeWindow(WID_OPTIONWND);
		if (skillListWndInfo.show)
			MakeWindow(WID_SKILLLISTWND);
		if (questWndInfo.show)
			DeleteWindow(WID_QUESTWND);
		g_modeMgr.GetGameMode();
		RAG_BUDGET_FILL(255)
		RAG_BUDGET_FILL(96)
		ShowWindow(WID_CHATWND, m_chatWndShow);
		ShowWindow(WID_SHORTCUTWND, shortCutWndInfo.show);
		ShowWindow(WID_ITEMDROPCNTWND, 0);
		if (messengerGroupWndInfo.show)
			MakeWindow(WID_MESSENGERWND);
		if (g_session.m_isLowSystemMemory && g_session.m_bAutoOpenDetailWindowIfLowMemory) {
			if (g_registry.m_spriteMode == 2 && g_registry.m_textureMode == 2)
				MakeWindow(WID_DETAILLEVELWND);
		}
		break;
	case WID_CHATWND:
		if (!(wnd = m_chatWnd)) {
			wnd = m_chatWnd = new UINewChatWnd;
			wnd->Create(600, m_chatWndHeight);
			AddWindow(wnd);
		}
		m_chatWnd->SendMsg(NULL, UM_SETINFO, 0, 0, 0, 0);
		break;
	case WID_SELECTSERVERWND:
		wnd = new UISelectServerWnd;
		wnd->Create(280, 200);
		wnd->Move(UIX(185), UICY(300) - 80);
		AddWindow(wnd);
		break;
	case WID_LOGINWND:
		wnd = new UILoginWnd;
		m_loginWnd = (UILoginWnd *)wnd;
		wnd->Create(280, 120);
		wnd->Move(UIX(185), UICY(300));
		AddWindow(wnd);
		break;
	case WID_NOTICEWND:
		wnd = new UINoticeWnd;
		wnd->Create(600, 400);
		wnd->Move(UIX(33), UIY(40));
		AddWindow(wnd);
		break;
	case WID_MAKECHARWND:
		wnd = new UIMakeCharWnd;
		wnd->Create(576, 342);
		wnd->Move(UIX(33), UIY(65));
		AddWindow(wnd);
		break;
	case WID_SELECTCHARWND:
		wnd = new UISelectCharWnd;
		m_selectCharWnd = (UISelectCharWnd *)wnd;
		wnd->Create(576, 342);
		wnd->Move(UIX(33), UIY(65));
		AddWindow(wnd);
		stateId.erase(stateId.begin(), stateId.end());
		break;
	case WID_ITEMWND:
		if (!(wnd = (UIFrameWnd *)m_itemWnd)) {
			wnd = m_itemWnd = new UIItemWnd;
			wnd->Create(280, 120);
			wnd->Move(0, 120);
			AddWindow(wnd);
			m_itemWnd->SendMsg(NULL, UM_SETINFO, (int)&itemWndInfo, 0, 0, 0);
			g_snapMgr.AddSnapObject(wnd);
		}
		wnd->SetShow(1);
		itemWndInfo.show = 1;
		break;
	case WID_QUESTWND:
		if (!(wnd = m_questWnd)) {
			wnd = m_questWnd = new UIQuestWnd;
			wnd->Create(280, 220);
			wnd->Move(0, 120);
			AddWindow(wnd);
			m_questWnd->SendMsg(NULL, UM_SETINFO, (int)&questWndInfo, 0, 0, 0);
			g_snapMgr.AddSnapObject(wnd);
		}
		wnd->SetShow(1);
		DeleteWindow(WID_NOTIFYQUESTWND);
		questWndInfo.show = 1;
		break;
	case WID_WAITWND:
		wnd = new UIWaitWnd;
		wnd->Create(280, 120);
		wnd->Move(UIX(185), UICY(300));
		AddWindow(wnd);
		break;
	case WID_LOADINGWND:
		if (!(wnd = (UIFrameWnd *)m_loadingWnd)) {
			wnd = m_loadingWnd = new UILoadingWnd;
			wnd->Create(240, 15);
			wnd->Move(g_renderer->m_width / 2 - 100, g_renderer->m_height / 2 + 160);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_TOOLTIPWND:
		wnd = new UIToolTipWnd;
		wnd->Create(200, 120);
		wnd->Move(UIX(185), UICY(300));
		AddWindow(wnd);
		break;
	case WID_SHORTCUTWND:
		wnd = m_shortCutWnd;
		if (wnd) break;
		wnd = new UIShortCutWnd;
		m_shortCutWnd = (UIShortCutWnd *)wnd;
		wnd->Create(280, 34);
		wnd->Move(UIX(200), UICY(200));
		AddWindow(wnd);
		m_shortCutWnd->SendMsg(NULL, UM_SETINFO, (int)&shortCutWndInfo, 0, 0, 0);
		g_snapMgr.AddSnapObject(wnd);
		break;
	case WID_EQUIPWND:
		if (!(wnd = m_equipWnd)) {
			wnd = m_equipWnd = new UIEquipWnd;
			m_equipWnd->m_typeWnd = 0;
			wnd->Create(280, 147);
			wnd->Move(UIX(185), UICY(300));
			AddWindow(wnd);
			m_equipWnd->SendMsg(NULL, UM_SETINFO, (int)&equipWndInfo, 0, 0, 0);
			g_snapMgr.AddSnapObject(wnd);
		}
		wnd->SetShow(1);
		equipWndInfo.show = 1;
		break;
	case WID_STATUSWND:
		if (!(wnd = m_statusWnd)) {
			wnd = m_statusWnd = new UIStatusWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(185), UICY(300));
			AddWindow(wnd);
			m_statusWnd->SendMsg(NULL, UM_SETINFO, (int)&statusWndInfo, 0, 0, 0);
			g_snapMgr.AddSnapObject(wnd);
		}
		wnd->SetShow(1);
		statusWndInfo.show = 1;
		DeleteWindow(WID_NOTIFYLEVELUPWND);
		break;
	case WID_SKILLLISTWND:
		if (!(wnd = m_skillListWnd)) {
			wnd = m_skillListWnd = new UISkillListWnd;
			m_skillListWnd->m_typeWnd = 0;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_skillListWnd->SendMsg(NULL, UM_SETINFO, (int)&skillListWndInfo, 0, 0, 0);
		}
		m_skillListWnd->m_typeWnd = 0;
		wnd->SetShow(1);
		skillListWndInfo.show = 1;
		DeleteWindow(WID_NOTIFYJOBLEVELUPWND);
		break;
	case WID_ITEMSTOREWND:
		if (!(wnd = (UIFrameWnd *)m_itemStoreWnd)) {
			wnd = m_itemStoreWnd = new UIItemStoreWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_itemStoreWnd->SendMsg(NULL, UM_SETINFO, (int)&itemStoreWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_MESSENGERWND:
		if (messengerGroupWndInfo.radio)
			MakeWindow(WID_MESSENGERGROUPWND)->SendMsg(NULL, UM_COMMAND, 0x95, 2, 0, 0);
		else
			MakeWindow(WID_MESSENGERGROUPWND)->SendMsg(NULL, UM_COMMAND, 0x95, 1, 0, 0);
		break;
	case WID_MESSENGERGROUPWND:
		DeleteWindow(WID_MESSENGERWND);
		if (!(wnd = (UIFrameWnd *)m_messengerGroupWnd)) {
			wnd = m_messengerGroupWnd = new UIMessengerGroupWnd;
			wnd->Create(140, 155);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_messengerGroupWnd->SendMsg(NULL, UM_SETINFO, (int)&messengerGroupWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		messengerGroupWndInfo.show = 1;
		break;
	case WID_MERCHANTSHOPMAKEWND:
		if (!(wnd = m_merchantShopMakeWnd)) {
			wnd = m_merchantShopMakeWnd = new UIMerchantShopMakeWnd;
			wnd->Create(280, 220);
			wnd->Move(UIX(185), UICY(200));
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&merchantShopMakeWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		MakeWindow(WID_MERCHANTMIRRORITEMWND);
		break;
	case WID_MERCHANTMIRRORITEMWND:
		if (!(wnd = (UIFrameWnd *)m_merchantMirrorItemWnd)) {
			wnd = m_merchantMirrorItemWnd = new UIMerchantMirrorItemWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(0), UICY(120));
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&merchantMirrorItemWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_MERCHANTITEMWND:
		if (m_mailListWnd || m_mailViewWnd) {
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_CLOSE_MAILWND), 0xff, 0, 0);
		} else {
			if (!(wnd = (UIFrameWnd *)m_merchantItemWnd)) {
				wnd = m_merchantItemWnd = new UIMerchantItemWnd;
				wnd->Create(280, 120);
				wnd->Move(UIX(200), UICY(120));
				AddWindow(wnd);
				wnd->SendMsg(NULL, UM_SETINFO, (int)&merchantItemWndInfo, 0, 0, 0);
			}
			wnd->SetShow(1);
			merchantItemWndInfo.show = 1;
		}
		break;
	case WID_DETAILLEVELWND:
		if (!(wnd = (UIFrameWnd *)m_detailLevelWnd)) {
			wnd = m_detailLevelWnd = new UIDetailLevelWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&detailLevelWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_NOTIFYITEMOBTAINWND:
		if (!(wnd = (UIFrameWnd *)m_notifyItemObtainWnd)) {
			wnd = m_notifyItemObtainWnd = new UINotifyItemObtainWnd;
			wnd->Create(180, 32);
			wnd->Move(UIX(220), 51);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_NPCEDITDIALOGWND:
		if (!(wnd = (UIFrameWnd *)m_npcEditDialogWnd)) {
			wnd = m_npcEditDialogWnd = new UINpcEditDialogWnd;
			wnd->Create(176, 46);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_NPCTEXTEDITDIALOGWND:
		if (!(wnd = (UIFrameWnd *)m_npcTextEditDialogWnd)) {
			wnd = m_npcTextEditDialogWnd = new UINpcTextEditDialogWnd;
			wnd->Create(346, 46);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_SKILLNAMECHANGEWND:
		if (!(wnd = (UIFrameWnd *)m_skillNameChangeWnd)) {
			wnd = m_skillNameChangeWnd = new UISkillNameChangeWnd;
			wnd->Create(280, 48);
			wnd->Move(UIX(185), UIY(180));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDWND:
		if (!guildWndInfo.IsValidInfo())
			guildWndInfo.MakeDefault();
		switch (guildWndInfo.curTab) {
		case 0: wnd = g_windowMgr.MakeWindow(WID_GUILDINFOMANAGEWND); break;
		case 1: wnd = g_windowMgr.MakeWindow(WID_GUILDMEMBERMANAGEWND); break;
		case 2: wnd = g_windowMgr.MakeWindow(WID_GUILDPOSITIONMANAGEWND); break;
		case 3: wnd = g_windowMgr.MakeWindow(WID_GUILDSKILLWND); break;
		case 4: wnd = g_windowMgr.MakeWindow(WID_GUILDBANISHEDMEMBERWND); break;
		case 5: wnd = g_windowMgr.MakeWindow(WID_GUILDNOTICEWND); break;
		case 6: wnd = g_windowMgr.MakeWindow(WID_GUILDTOTALINFOWND); break;
		default: break;
		}
		break;
	case WID_GUILDMEMBERMANAGEWND:
		if (!(wnd = m_guildMemberManageWnd)) {
			wnd = m_guildMemberManageWnd = new UIGuildMemberManageWnd;
			width = guildWndWidth;
			height = guildWndHeight;
			wnd->Create(width, height);
			wnd->Move(0, 147);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&guildWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDPOSITIONMANAGEWND:
		if (!(wnd = m_guildPositionManageWnd)) {
			wnd = m_guildPositionManageWnd = new UIGuildPositionManageWnd;
			width = guildWndWidth;
			height = guildWndHeight;
			wnd->Create(width, height);
			wnd->Move(0, 147);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&guildWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDSKILLWND:
		if (!(wnd = m_guildSkillWnd)) {
			wnd = m_guildSkillWnd = new UIGuildSkillWnd;
			width = guildWndWidth;
			height = guildWndHeight;
			wnd->Create(width, height);
			wnd->Move(0, 147);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&guildWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDBANISHEDMEMBERWND:
		if (!(wnd = m_guildBanishedMemberWnd)) {
			wnd = m_guildBanishedMemberWnd = new UIGuildBanishedMemberWnd;
			width = guildWndWidth;
			height = guildWndHeight;
			wnd->Create(width, height);
			wnd->Move(0, 147);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&guildWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDNOTICEWND:
		if (!(wnd = m_guildNoticeWnd)) {
			wnd = m_guildNoticeWnd = new UIGuildNoticeWnd;
			width = guildWndWidth;
			height = guildWndHeight;
			wnd->Create(width, height);
			wnd->Move(0, 147);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&guildWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDTOTALINFOWND:
		if (!(wnd = m_guildTotalInfoWnd)) {
			wnd = m_guildTotalInfoWnd = new UIGuildTotalInfoWnd;
			width = guildWndWidth;
			height = guildWndHeight;
			wnd->Create(width, height);
			wnd->Move(0, 147);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&guildWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDINFOMANAGEWND:
		if (!(wnd = m_guildInfoManageWnd)) {
			wnd = m_guildInfoManageWnd = new UIGuildInfoManageWnd;
			width = guildWndWidth;
			height = guildWndHeight;
			wnd->Create(width, height);
			wnd->Move(0, 147);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&guildWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDLEAVEREASONDESCWND:
		if (!(wnd = (UIFrameWnd *)m_guildLeaveReasonDescWnd)) {
			wnd = m_guildLeaveReasonDescWnd = new UIGuildLeaveReasonDescWnd;
			wnd->Create(280, 80);
			wnd->Move(UIX(200), UICY(120));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_ILLUSTWND:
		if (!(wnd = m_illustWnd)) {
			wnd = m_illustWnd = new UIIllustWnd;
			wnd->Create(4, 4);
			wnd->Move(UIX(200), UICY(120));
			AddWindowFront(wnd);
		} else {
			wnd->SendMsg(NULL, UM_SETINFO, -1, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_GUILDBANREASONDESCWND:
		if (!(wnd = (UIFrameWnd *)m_guildBanReasonDescWnd)) {
			wnd = m_guildBanReasonDescWnd = new UIGuildBanReasonDescWnd;
			wnd->Create(280, 80);
			wnd->Move(UIX(200), UICY(120));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_FRIENDOPTIONWND:
		if (!(wnd = (UIFrameWnd *)m_friendOptionWnd)) {
			wnd = m_friendOptionWnd = new UIFriendOptionWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(120));
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&friendOptionWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_PARTYSETTINGWND:
		if (!(wnd = m_partySettingWnd)) {
			wnd = m_partySettingWnd = new UIPartySettingWnd;
			wnd->Create(280, g_session.GetNumParty() ? 245 : 180);
			wnd->Move(UIX(200), UICY(120));
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&partySettingWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_COMPOSITIONWND:
		if (!(wnd = (UIFrameWnd *)m_compositionWnd)) {
			wnd = m_compositionWnd = new UICompositionWnd;
			wnd->Create(34, 34);
			wnd->Move(0, 0);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_CANDIDATEWND:
		if (!(wnd = m_candidateWnd)) {
			wnd = m_candidateWnd = new UICandidateWnd;

			width = 292;
			height = 34;
			if (m_conversionMode) {
				width = 120;
				height = 212;
			}
			wnd->Create(width, height);
			wnd->Move(g_renderer->m_width - width, g_renderer->m_height - height);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_TIPOFTHEDAYWND:
		if (!(wnd = (UIFrameWnd *)m_tipOfTheDayWnd)) {
			wnd = m_tipOfTheDayWnd = new UITipOfTheDayWnd;
			wnd->Create(280, 180);
			wnd->Move(UIX(200), UICY(120));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_JOINGUILDACCEPTWND:
		if (!(wnd = (UIFrameWnd *)m_joinGuildAcceptWnd)) {
			wnd = m_joinGuildAcceptWnd = new UIJoinGuildAcceptWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_ALLYGUILDACCEPTWND:
		if (!(wnd = (UIFrameWnd *)m_allyGuildAcceptWnd)) {
			wnd = m_allyGuildAcceptWnd = new UIAllyGuildAcceptWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_JOINPARTYACCEPTWND:
		if (!(wnd = (UIFrameWnd *)m_joinPartyAcceptWnd)) {
			wnd = m_joinPartyAcceptWnd = new UIJoinPartyAcceptWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_COUPLEACCEPTWND:
		if (!(wnd = (UIFrameWnd *)m_CoupleAcceptWnd)) {
			wnd = m_CoupleAcceptWnd = new UICoupleAcceptWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_STARPLACE_ACCEPTWND:
		if (!(wnd = (UIFrameWnd *)m_StarPlaceAcceptWnd)) {
			wnd = m_StarPlaceAcceptWnd = new UIStarPlaceAcceptWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_YOURITEMWND:
		if (!(wnd = (UIFrameWnd *)m_YourItemWnd)) {
			wnd = m_YourItemWnd = new UIYourItemWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_BABYACCEPTWND:
		if (!(wnd = (UIFrameWnd *)m_BabyAcceptWnd)) {
			wnd = m_BabyAcceptWnd = new UIBabyAcceptWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_BABYACCEPTWND2:
		if (!(wnd = (UIFrameWnd *)m_BabyAcceptWnd2)) {
			wnd = m_BabyAcceptWnd2 = new UIBabyAcceptWnd2;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_EXCHANGEACCEPTWND:
		if (!(wnd = (UIFrameWnd *)m_exchangeAcceptWnd)) {
			wnd = m_exchangeAcceptWnd = new UIExchangeAcceptWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_exchangeAcceptWnd->SendMsg(NULL, UM_SETINFO, (int)&exchangeAcceptWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_EXCHANGEWND:
		if (!(wnd = (UIFrameWnd *)m_exchangeWnd)) {
			wnd = m_exchangeWnd = new UIExchangeWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_PASSWORDWND:
		if (!(wnd = (UIFrameWnd *)m_passwordWnd)) {
			wnd = m_passwordWnd = new UIPasswordWnd;
			wnd->Create(176, 46);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_ITEMDROPCNTWND:
		if (!(wnd = m_itemDropCntWnd)) {
			wnd = m_itemDropCntWnd = new UIItemDropCntWnd;
			wnd->Create(182, 46);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
		}
		break;
	case WID_SAYDIALOGWND:
		if (!(wnd = m_sayDialogWnd)) {
			wnd = m_sayDialogWnd = new UISayDialogWnd;
			wnd->Create(280, 180);
			wnd->Move(UIX(200), UICY(100));
			AddWindow(wnd);
			m_sayDialogWnd->SendMsg(NULL, UM_SETINFO, (int)&sayDialogWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_CHATROOMWND:
		if (!(wnd = m_chatRoomWnd)) {
			wnd = m_chatRoomWnd = new UIChatRoomWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(300));
			AddWindow(wnd);
			m_chatRoomWnd->SendMsg(NULL, UM_SETINFO2, (int)&chatRoomWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_CHOOSEWARPWND:
		{
			if (!(window = m_chooseWarpWnd)) {
				window = m_chooseWarpWnd = new UIChooseWarpWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(200), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_CHOOSEWND:
		if (!(wnd = (UIFrameWnd *)m_chooseWnd)) {
			wnd = m_chooseWnd = new UIChooseWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(300));
			AddWindow(wnd);
			m_chooseWnd->SendMsg(NULL, UM_SETINFO, (int)&chooseWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_ITEMCOMPOSITIONWND:
		if (!(wnd = (UIFrameWnd *)m_itemCompositionWnd)) {
			wnd = m_itemCompositionWnd = new UIItemCompositionWnd;
			wnd->Create(200, 200);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_itemCompositionWnd->SendMsg(NULL, UM_SETINFO, (int)&itemCompositionWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_ITEMIDENTIFYWND:
		if (!(wnd = (UIFrameWnd *)m_itemIdentifyWnd)) {
			wnd = m_itemIdentifyWnd = new UIItemIdentifyWnd;
			wnd->Create(200, 200);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_itemIdentifyWnd->SendMsg(NULL, UM_SETINFO, (int)&itemIdentifyWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_COMBOBOXWND:
		{
			if (!(window = m_comboBoxWnd)) {
				window = m_comboBoxWnd = new UIComboBoxWnd;
				window->Create(16, 16);
				wnd = window;
				wnd->Move(UIX(200), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_MENUWND:
		{
			if (!(window = m_menu)) {
				window = m_menu = new UIMenuWnd;
				window->Create(4, 4);
				wnd = window;
				wnd->Move(UIX(200), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_OPTIONWND:
		if (!(wnd = (UIFrameWnd *)m_optionWnd)) {
			wnd = m_optionWnd = new UIOptionWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(185), UICY(300));
			AddWindow(wnd);
			m_optionWnd->SendMsg(NULL, UM_SETINFO, (int)&optionWndInfo, 0, 0, 0);
			g_snapMgr.AddSnapObject(wnd);
		}
		wnd->SetShow(1);
		optionWndInfo.show = 1;
		break;
	case WID_MINIMAPZOOMWND:
		if (!(wnd = m_minimapZoomWnd)) {
			wnd = m_minimapZoomWnd = new UIMinimapZoomWnd;
			wnd->Create(12, 24);
			width = g_renderer->m_width - 29;
			wnd->Move(width, 17);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_CHATROOMCHANGEWND:
		{
			if (!(window = m_chatRoomChangeWnd)) {
				window = m_chatRoomChangeWnd = new UIChatRoomChangeWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(185), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_CHATROOMMAKEWND:
		{
			if (!(window = m_chatRoomMakeWnd)) {
				window = m_chatRoomMakeWnd = new UIChatRoomMakeWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(185), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_CHOOSESELLBUYWND:
		{
			if (!(window = m_chooseSellBuyWnd)) {
				window = m_chooseSellBuyWnd = new UIChooseSellBuyWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(185), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_ITEMSELLWND:
		if (!(wnd = (UIFrameWnd *)m_itemSellWnd)) {
			wnd = m_itemSellWnd = new UIItemSellWnd;
			wnd->Create(280, 120);
			wnd->Move(300, 200);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&itemSellWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_MERCHANTITEMPURCHASEWND:
		if (!(wnd = (UIFrameWnd *)m_merchantItemPurchaseWnd)) {
			wnd = m_merchantItemPurchaseWnd = new UIMerchantItemPurchaseWnd;
			wnd->Create(280, 120);
			wnd->Move(300, 200);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&merchantItemPurchaseWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_ITEMPURCHASEWND:
		if (!(wnd = m_itemPurchaseWnd)) {
			wnd = m_itemPurchaseWnd = new UIItemPurchaseWnd;
			wnd->Create(280, 120);
			wnd->Move(300, 200);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&itemPurchaseWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		MakeWindow(WID_ITEMPARAMCHANGEDISPLAYWND);
		break;
	case WID_MERCHANTITEMMYSHOPWND:
		if (!(wnd = (UIFrameWnd *)m_merchantItemMyShopWnd)) {
			wnd = m_merchantItemMyShopWnd = new UIMerchantItemMyShopWnd;
			wnd->Create(280, 120);
			wnd->Move(0, 200);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&merchantItemMyShopWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_MERCHANTITEMSHOPWND:
		if (!(wnd = (UIFrameWnd *)m_merchantItemShopWnd)) {
			wnd = m_merchantItemShopWnd = new UIMerchantItemShopWnd;
			wnd->Create(280, 120);
			wnd->Move(0, 200);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&merchantItemShopWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_ITEMSHOPWND:
		if (!(wnd = (UIFrameWnd *)m_itemShopWnd)) {
			wnd = m_itemShopWnd = new UIItemShopWnd;
			wnd->Create(280, 120);
			wnd->Move(0, 200);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&itemShopWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_NOTIFYJOBLEVELUPWND:
		{
			if (!(window = m_notifyJobLevelUpWnd)) {
				window = m_notifyJobLevelUpWnd = new UINotifyJobLevelUpWnd;
				window->Create(43, 43);
				wnd = window;
				wnd->Move(0, g_renderer->m_height - 43);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_NOTIFYLEVELUPWND:
		{
			if (!(window = m_notifyLevelUpWnd)) {
				window = m_notifyLevelUpWnd = new UINotifyLevelUpWnd;
				window->Create(43, 43);
				wnd = window;
				wnd->Move(g_renderer->m_width - 43, g_renderer->m_height - 43);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_NOTIFYQUESTWND:
		{
			if (!(window = m_notifyQuestWnd)) {
				window = m_notifyQuestWnd = new UINotifyQuestWnd;
				window->Create(43, 43);
				wnd = window;
				wnd->Move(g_renderer->m_width - 43, g_renderer->m_height - 43);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_CARDITEMILLUSTWND:
		{
			if (!(window = m_cardItemIllustWnd)) {
				window = m_cardItemIllustWnd = new UICardItemIllustWnd;
				window->Create(300, 417);
				wnd = window;
				wnd->Move(UIX(0), UICY(0));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_RESTARTWND:
		{
			if (!(window = m_restartWnd)) {
				window = m_restartWnd = new UIRestartWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(185), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_QUITWND:
		{
			if (!(window = m_quitWnd)) {
				window = m_quitWnd = new UIQuitWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(185), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_SKILLDESCRIBEWND:
		{
			if (!(window = m_skillDescribeWnd)) {
				window = m_skillDescribeWnd = new UISkillDescribeWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(185), UICY(300) - 120);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_QUEST_DETAIL_WND:
		{
			if (!(window = m_questDetailWnd)) {
				window = m_questDetailWnd = new UIQuestDetailWnd;
				window->Create(350, 375);
				wnd = window;
				wnd->Move(UIX(10), UICY(50));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_ITEMPARAMCHANGEDISPLAYWND:
		if (g_serviceType == ServiceVietnam) break;
		if (!(wnd = (UIFrameWnd *)m_itemParamChangeDisplayWnd)) {
			wnd = m_itemParamChangeDisplayWnd = new UIItemParamChangeDisplayWnd;
			wnd->Create(280, 34);
			wnd->Move(UIX(185), UICY(300) - 34);
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&itemParamChangeDisplayWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_COMBINEDCARDITEMCOLLECTIONWND:
		if (!(wnd = (UIFrameWnd *)m_combinedCardItemCollectionWnd)) {
			wnd = m_combinedCardItemCollectionWnd = new UICombinedCardItemCollectionWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(185), UICY(300));
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&combinedCardItemCollectionWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_ITEMCOLLECTIONWND:
		if (!(wnd = (UIFrameWnd *)m_itemCollectionWnd)) {
			wnd = m_itemCollectionWnd = new UIItemCollectionWnd;
			wnd->Create(280, 120);
			wnd->Move(UIX(185), UICY(300));
			AddWindow(wnd);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&itemCollectionWndInfo, 0, 0, 0);
		}
		wnd->SetShow(1);
		break;
	case WID_NOTICECONFIRMWND:
		wnd = new UINoticeConfirmWnd;
		wnd->Create(280, 120);
		wnd->Move(UIX(185), UICY(300));
		AddWindow(wnd);
		break;
	case WID_MONSTERINFOWND:
		wnd = new UIMonsterInfoWnd;
		wnd->Create(400, 200);
		wnd->Move(0, 0);
		AddWindow(wnd);
		break;
	case WID_MAKETARGETLISTWND:
		{
			if (!(window = m_metalListWnd)) {
				window = m_metalListWnd = new UIMakeTargetListWnd;
				window->Create(280, 150);
				wnd = window;
				wnd->Move(UIX(185), UICY(300) - 60);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_ITEMREPAIRWND:
		{
			if (!(window = m_itemRepairWnd)) {
				window = m_itemRepairWnd = new UIItemRepairWnd;
				window->Create(280, 150);
				wnd = window;
				wnd->Move(UIX(185), UICY(300) - 60);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_WEAPONREFINEWND:
		{
			if (!(window = m_weaponRefineWnd)) {
				window = m_weaponRefineWnd = new UIWeaponRefineWnd;
				window->Create(280, 150);
				wnd = window;
				wnd->Move(UIX(185), UICY(300) - 60);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_MAKETARGETPROCESSWND:
		{
			if (!(window = m_metalProcessWnd)) {
				window = m_metalProcessWnd = new UIMakeTargetProcessWnd;
				window->Create(280, 150);
				wnd = window;
				wnd->Move(UIX(185), UICY(300) - 60);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_MAKETARGETRESULTWND:
		{
			if (!(window = m_metalResultWnd)) {
				window = m_metalResultWnd = new UIMakeTargetResultWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(185), UICY(300) - 60);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_TALKBOXTRAPINPUTWND:
		wnd = new UITalkboxTrapInputWnd;
		wnd->Create(280, 120);
		wnd->Move(UIX(185), UICY(300) - 60);
		AddWindow(wnd);
		break;
	case WID_KEYSTROKEWND:
		{
			if (!(window = m_keyStrokeWnd)) {
				window = m_keyStrokeWnd = new UIKeyStrokeWnd;
				window->Create(60, 18);
				wnd = window;
				wnd->Move(UIX(185), UICY(300) - 60);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_EMOTIONWND:
		if (!(wnd = m_emotionWnd)) {
			wnd = m_emotionWnd = new UIEmotionWnd;
			wnd->Create(375, 280);
			wnd->Move(UIX(185), UICY(300) - 60);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&emotionWndInfo, 0, 0, 0);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_EMOTIONLISTWND:
		if (!(wnd = m_emotionListWnd)) {
			wnd = m_emotionListWnd = new UIEmotionListWnd;
			wnd->Create(445, 310);
			wnd->Move(UIX(185), UICY(400) - 60);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&emotionListWndInfo, 0, 0, 0);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_PETINFOWND:
		if (g_session.m_petJob == -1) break;
		if (!(wnd = m_petInfoWnd)) {
			wnd = m_petInfoWnd = new UIPetInfoWnd;
			wnd->Create(280, 160);
			wnd->Move(UIX(185), UICY(400) - 60);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&petInfoWndInfo, 0, 0, 0);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_SELECTPETEGGWND:
		{
			if (!(window = m_selectPetEggWnd)) {
				window = m_selectPetEggWnd = new UIPetEggListWnd;
				window->Create(200, 200);
				wnd = window;
				wnd->Move(UIX(200), UICY(200));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_PETTAMINGDECEIVEWND:
		{
			if (!(window = m_petTamingDeceiveWnd)) {
				window = m_petTamingDeceiveWnd = new UIPetTamingDeceiveWnd;
				window->Create(196, 256);
				wnd = window;
				wnd->Move(g_renderer->m_width / 2 - 98, g_renderer->m_height / 2 - 128);
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_MAKINGARROWLISTWND:
		{
			if (!(window = m_makingArrowListWnd)) {
				window = m_makingArrowListWnd = new UIMakingArrowListWnd;
				window->Create(200, 200);
				wnd = window;
				wnd->Move(UIX(200), UICY(200));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_SELECTCARTWND:
		{
			if (!(window = m_selectCartWnd)) {
				window = m_selectCartWnd = new UISelectCartWnd;
				window->Create(372, 98);
				wnd = window;
				wnd->Move(UIX(200), UICY(200));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_SPELLLISTWND:
		if (m_spellListWnd) {
			PostQuit(m_spellListWnd);
			m_spellListWnd = NULL;
		}
		wnd = new UISpellListWnd;
		m_spellListWnd = (UISpellListWnd *)wnd;
		wnd->Create(200, 200);
		wnd->Move(UIX(200), UICY(200));
		AddWindow(wnd);
		wnd->SetShow(1);
		break;
	case WID_GRAFFISTRBOXWND:
		wnd = new UIGraffiStrboxWnd;
		wnd->Create(280, 120);
		wnd->Move(UIX(185), UICY(300) - 60);
		AddWindow(wnd);
		break;
	case WID_PROHIBITLISTWND:
		{
			if (!(window = m_prohibitListWnd)) {
				window = m_prohibitListWnd = new UIProhibitListWnd;
				window->Create(500, 400);
				wnd = window;
				wnd->Move(UIX(100), UICY(100));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_BOOKWND:
		{
			if (!(window = m_bookWnd)) {
				window = m_bookWnd = new UIBookWnd;
				window->Create(600, 420);
				wnd = window;
				wnd->Move(UIX(0), UICY(0));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_JOINFRIENDACCEPTWND:
		{
			if (!(window = m_joinFriendAcceptWnd)) {
				window = m_joinFriendAcceptWnd = new UIJoinFriendAcceptWnd;
				window->Create(280, 120);
				wnd = window;
				wnd->Move(UIX(200), UICY(200));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_COMMANDLISTWND:
		{
			if (!(window = m_commandListWnd)) {
				window = m_commandListWnd = new UICommandListWnd;
				window->Create(250, 200);
				wnd = window;
				wnd->Move(UIX(400), UICY(200));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_HOMUNINFOWND:
		if (g_session.m_homunJob == -1) break;
		if (!(wnd = m_homunInfoWnd)) {
			wnd = m_homunInfoWnd = new UIHomunInfoWnd;
			wnd->Create(280, 180);
			wnd->Move(UIX(185), UICY(400) - 60);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&homunInfoWndInfo, 0, 0, 0);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_HOMUNSKILLLISTWND:
		if (!(wnd = m_homunSkillListWnd)) {
			wnd = m_homunSkillListWnd = new UISkillListWnd;
			m_homunSkillListWnd->m_typeWnd = 1;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_homunSkillListWnd->SendMsg(NULL, UM_SETINFO, (int)&homunSkillListWndInfo, 0, 0, 0);
		}
		m_homunSkillListWnd->m_typeWnd = 1;
		wnd->SetShow(1);
		homunSkillListWndInfo.show = 1;
		break;
	case WID_MERINFOWND:
		if (g_session.m_merJob == -1) break;
		if (!(wnd = m_merInfoWnd)) {
			wnd = m_merInfoWnd = new UIMerInfoWnd;
			wnd->Create(280, 180);
			wnd->Move(UIX(185), UICY(400) - 60);
			wnd->SendMsg(NULL, UM_SETINFO, (int)&merInfoWndInfo, 0, 0, 0);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_MERSKILLLISTWND:
		if (!(wnd = m_merSkillListWnd)) {
			wnd = m_merSkillListWnd = new UISkillListWnd;
			m_merSkillListWnd->m_typeWnd = 2;
			wnd->Create(280, 120);
			wnd->Move(UIX(200), UICY(200));
			AddWindow(wnd);
			m_merSkillListWnd->SendMsg(NULL, UM_SETINFO, (int)&merSkillListWndInfo, 0, 0, 0);
		}
		m_merSkillListWnd->m_typeWnd = 2;
		wnd->SetShow(1);
		merSkillListWndInfo.show = 1;
		break;
	case WID_AUTOMESSAGEWND:
		{
			if (!(window = m_autoMessageWnd)) {
				window = m_autoMessageWnd = new UIAutoMessageWnd;
				window->Create(280, 48);
				wnd = window;
				wnd->Move(UIX(185), UIY(180));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_MAILLISTWND:
		if (m_merchantItemWnd) {
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_CLOSE_MERCHANTITEMWND), 0xff, 0, 0);
			break;
		}
		{
			if (!(window = m_mailListWnd)) {
				window = m_mailListWnd = new UIMailListWnd;
				window->Create(300, 400);
				wnd = window;
				wnd->Move(UIX(100), UICY(100));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_MAILVIEWWND:
		if (m_merchantItemWnd) {
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_CLOSE_MERCHANTITEMWND), 0xff, 0, 0);
			break;
		}
		{
			if (!(window = m_mailViewWnd)) {
				window = m_mailViewWnd = new UIMailViewWnd;
				window->Create(300, 400);
				wnd = window;
				wnd->Move(UIX(400), UICY(100));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_AUCTIONWND:
		{
			if (!(window = m_auctionWnd)) {
				window = m_auctionWnd = new UIAuctionWnd;
				window->Create(550, 300);
				wnd = window;
				wnd->Move(UIX(200), UICY(100));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_AUCTIONMSGWND:
		{
			if (!(window = m_auctionMsgWnd)) {
				window = m_auctionMsgWnd = new UIAuctionMsgWnd;
				window->Create(236, 161);
				wnd = window;
				wnd->Move(UIX(300), UICY(130));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_IMGMENUWND:
		if (!m_imgMenuWnd) {
			wnd = new UIImgMenuWnd;
			m_imgMenuWnd = (UIImgMenuWnd *)wnd;
			wnd->Create(16, 16);
			wnd->Move(UIX(200), UICY(300));
			AddWindow(wnd);
		} else {

			wnd = m_comboBoxWnd;
		}
		wnd->SetShow(1);
		break;
	case WID_SELCHARFORUSERVERWND:
		wnd = new UISelCharForUServerWnd;
		m_selCharForUServerWnd = (UISelCharForUServerWnd *)wnd;
		wnd->Create(540, 400);
		wnd->Move(UIX(33), UIY(65));
		AddWindow(wnd);
		break;
	case WID_CHANGENAMEWND:
		{
			if (!(window = m_changeNameWnd)) {
				window = m_changeNameWnd = new UIChangeNameWnd;
				window->Create(184, 105);
				wnd = window;
				wnd->Move(UIX(260), UIY(180));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_REBIRTHWND:
		{
			if (!(window = m_rebirthWnd)) {
				window = m_rebirthWnd = new UIRebirthWnd;
				window->Create(280, 143);
				wnd = window;
				wnd->Move(UIX(185), UICY(300));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_SUBCHATWND_ST:
		if (!(wnd = (UIFrameWnd *)m_subChatWndST)) {
			wnd = m_subChatWndST = new UISubChatHisWnd;
			wnd->Create(300, 95);
			wnd->SetId(0);
			AddWindow(wnd);
			((UIWindow *)m_subChatWndST)->SendMsg(NULL, UM_SETINFO, (int)&subChatWndStInfo, 0, 0, 0);
		}
		wnd->SetShow(0);
		break;
	case WID_SUBCHATWND_BT:
		if (!(wnd = (UIFrameWnd *)m_subChatWndBT)) {
			wnd = m_subChatWndBT = new UISubChatHisWnd;
			wnd->Create(300, 95);
			wnd->SetId(1);
			AddWindow(wnd);
			((UIWindow *)m_subChatWndBT)->SendMsg(NULL, UM_SETINFO, (int)&subChatWndBtInfo, 0, 0, 0);
		}
		wnd->SetShow(0);
		break;
	case WID_BATTLEMSG_OPTIONWND:
		{
			if (!(window = m_battleMsgOpionWnd)) {
				window = m_battleMsgOpionWnd = new UIBattleMsgOptionWnd;
				window->Create(280, 200);
				wnd = window;
				wnd->Move(UIX(400), UICY(200));
				AddWindow(wnd);
			}
			wnd = window;
			wnd->SetShow(1);
		}
		break;
	case WID_SUBCHAT_MINIWND:
		if (!(wnd = (UIFrameWnd *)m_subChatMiniWnd)) {
			wnd = m_subChatMiniWnd = new UISubChatMiniWnd;
			wnd->Create(5, 5);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_MEMORIAL_DUN_WND:
		if (g_session.m_dunName == "") return NULL;
		if (!(wnd = m_memorialDunWnd)) {
			wnd = m_memorialDunWnd = new UIMemorialDunWnd;
			wnd->Create(200, 150);
			if (g_session.dunInfoWndX >= 0 && g_session.dunInfoWndY >= 0)
				wnd->Move(g_session.dunInfoWndX, g_session.dunInfoWndY);
			else
				wnd->Move(0, 310);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		g_session.m_onDunInfoWnd = 1;
		break;
	case WID_OTHERUSER_EQUIPWND:
		if (!(wnd = m_otherUserEquipWnd)) {
			wnd = m_otherUserEquipWnd = new UIEquipWnd;
			m_otherUserEquipWnd->m_typeWnd = 1;
			wnd->Create(280, 147);
			wnd->Move(UIX(185), UICY(300));
			AddWindow(wnd);
			g_snapMgr.AddSnapObject(wnd);
		}
		wnd->SetShow(1);
		break;
	case WID_ROMAPWND:
		if (!(wnd = m_roMapWnd)) {
			wnd = m_roMapWnd = new UIRoMapWnd;
			wnd->Create(g_renderer->m_width, g_renderer->m_height);
			wnd->Move(0, 0);
			AddWindow(wnd);
		}
		wnd->SetShow(1);
		g_windowMgr.MakeTopLayer((UIWindow *)m_roMapWnd);
		break;
	default:
		break;
	}

	return wnd;
}

unsigned long __stdcall FinalStrike(void *pvoid)
{
	g_modeMgr.GetGameMode()->GetWorld()->m_player->SendMsg(NULL, AM_DELETE_EFFECT, 0x9f, 0, 0);
	g_modeMgr.GetGameMode()->GetWorld()->m_player->SendMsg(NULL, AM_DELETE_EFFECT, 0xa6, 0, 0);

	int sleepAdd = 0;
	int slowStep = 0;
	for (int i = 0; i < 160; ++i)
	{
		switch (i)
		{
		case 19:
		case 61:
		case 113:
		case 145:
		case 156:
		case 157:
			continue;
		}

		vector3d deltaPos = vector3d(0.0f, 0.0f, 0.0f);
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(i, deltaPos, 0.0f);
		Sleep(sleepAdd + 200);
		sleepAdd += 5 - slowStep;
		if ((i % 20) == 1)
			++slowStep;
	}

	return 0;
}

unsigned long __stdcall TestStrike(void * pvoid)
{
	{
		vector3d deltaPos = vector3d(0.0f, 0.0f, 0.0f);
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(0x1fe, deltaPos, 0.0f);
	}

	Sleep(1000);

	for (int i = 0; i < 7; ++i)
	{
		vector3d deltaPos = vector3d(0.0f, 0.0f, 0.0f);
		g_modeMgr.GetGameMode()->GetWorld()->m_player->LaunchEffect(0x1d2 + i, deltaPos, 0.0f);
		Sleep(1000);
	}

	return 0;
}














void MakeEffectList() {}

bool UIWindowMgr::ExcuteMsgInBattleMode(unsigned long virtualKey, int newKeydown)
{
	if (g_session.m_isBattle && !g_session.m_isBmChat) {
		if (m_shortCutWnd) {
			switch (newKeydown) {
			case 0x100001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 0, 1, 0, 0);
				break;
			case 0x110001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 1, 1, 0, 0);
				break;
			case 0x120001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 2, 1, 0, 0);
				break;
			case 0x130001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 3, 1, 0, 0);
				break;
			case 0x140001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 4, 1, 0, 0);
				break;
			case 0x150001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 5, 1, 0, 0);
				break;
			case 0x160001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 6, 1, 0, 0);
				break;
			case 0x170001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 7, 1, 0, 0);
				break;
			case 0x180001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 1, 0, 0);
				break;
			case 0x1e0001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 0, 2, 0, 0);
				break;
			case 0x1f0001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 1, 2, 0, 0);
				break;
			case 0x200001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 2, 2, 0, 0);
				break;
			case 0x210001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 3, 2, 0, 0);
				break;
			case 0x220001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 4, 2, 0, 0);
				break;
			case 0x230001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 5, 2, 0, 0);
				break;
			case 0x240001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 6, 2, 0, 0);
				break;
			case 0x250001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 7, 2, 0, 0);
				break;
			case 0x260001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 2, 0, 0);
				break;
			case 0x2c0001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 0, 0, 0, 0);
				break;
			case 0x2d0001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 1, 0, 0, 0);
				break;
			case 0x2e0001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 2, 0, 0, 0);
				break;
			case 0x2f0001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 3, 0, 0, 0);
				break;
			case 0x300001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 4, 0, 0, 0);
				break;
			case 0x310001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 5, 0, 0, 0);
				break;
			case 0x320001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 6, 0, 0, 0);
				break;
			case 0x330001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 7, 0, 0, 0);
				break;
			case 0x340001:
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 0, 0, 0);
				break;
			}
		}
	}

	return false;
}

bool UIWindowMgr::ExcuteMsgInBattleModeVietnam(long lnKey)
{
	if (g_session.m_isBattle && !g_session.m_isBmChat) {
		if (m_shortCutWnd) {
			switch (lnKey) {
			case 'q':
			case 'Q':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 0, 1, 0, 0);
				break;
			case 'w':
			case 'W':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 1, 1, 0, 0);
				break;
			case 'e':
			case 'E':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 2, 1, 0, 0);
				break;
			case 'r':
			case 'R':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 3, 1, 0, 0);
				break;
			case 't':
			case 'T':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 4, 1, 0, 0);
				break;
			case 'y':
			case 'Y':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 5, 1, 0, 0);
				break;
			case 'u':
			case 'U':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 6, 1, 0, 0);
				break;
			case 'i':
			case 'I':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 7, 1, 0, 0);
				break;
			case 'o':
			case 'O':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 1, 0, 0);
				break;
			case 'a':
			case 'A':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 0, 2, 0, 0);
				break;
			case 's':
			case 'S':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 1, 2, 0, 0);
				break;
			case 'd':
			case 'D':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 2, 2, 0, 0);
				break;
			case 'f':
			case 'F':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 3, 2, 0, 0);
				break;
			case 'g':
			case 'G':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 4, 2, 0, 0);
				break;
			case 'h':
			case 'H':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 5, 2, 0, 0);
				break;
			case 'j':
			case 'J':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 6, 2, 0, 0);
				break;
			case 'k':
			case 'K':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 7, 2, 0, 0);
				break;
			case 'l':
			case 'L':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 2, 0, 0);
				break;
			case 'z':
			case 'Z':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 0, 0, 0, 0);
				break;
			case 'x':
			case 'X':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 1, 0, 0, 0);
				break;
			case 'c':
			case 'C':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 2, 0, 0, 0);
				break;
			case 'v':
			case 'V':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 3, 0, 0, 0);
				break;
			case 'b':
			case 'B':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 4, 0, 0, 0);
				break;
			case 'n':
			case 'N':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 5, 0, 0, 0);
				break;
			case 'm':
			case 'M':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 6, 0, 0, 0);
				break;
			case ',':
			case '<':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 7, 0, 0, 0);
				break;
			case '.':
			case '>':
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 0, 0, 0);
				break;
			}
		}
	}

	return false;
}

inline BOOL CheckKeyPressed(int vkcode)
{
	return HIBYTE(GetAsyncKeyState(vkcode));
}

void UIWindowMgr::SetTextAtBattleMode()
{
	if (!m_chatWnd)
		return;
	if (m_editWindow == (UIWindow*)m_chatWnd->m_commonChat)
		m_chatWnd->m_commonChat->SetText("");
	if (m_editWindow == (UIWindow*)m_chatWnd->m_whisperChat)
		m_chatWnd->m_whisperChat->SetText(m_chatWnd->m_whisperChat->GetText());
}

bool UIWindowMgr::isFocusChatWnd()
{
	if (m_chatWnd) {
		if (GetFocusEdit() == (UIWindow*)m_chatWnd->m_commonChat || GetFocusEdit() == (UIWindow*)m_chatWnd->m_whisperChat)
			return true;
	}
	return false;
}

bool UIWindowMgr::ProcessPushButton(unsigned long virtualKey, int newKeydown, int accurateKey)
{
	if (accurateKey == 0x1e0001)
		virtualKey = 'A';
	else if (accurateKey == 0x200001)
		virtualKey = 'D';
	else if (virtualKey == VK_RETURN)
		virtualKey = 'A';
	else if (virtualKey == 0x110001 || virtualKey == 'W')
		virtualKey = VK_UP;
	else if (virtualKey == 0x2d0001 || virtualKey == 'X')
		virtualKey = VK_DOWN;

	switch (virtualKey) {
	case VK_ESCAPE:
		if (newKeydown) {
			if (m_itemCollectionWnd)
				DeleteWindow(WID_ITEMCOLLECTIONWND);
			else if (m_skillDescribeWnd)
				DeleteWindow(WID_SKILLDESCRIBEWND);
			else if (m_questDetailWnd)
				DeleteWindow(WID_QUEST_DETAIL_WND);
			else if (m_combinedCardItemCollectionWnd)
				DeleteWindow(WID_COMBINEDCARDITEMCOLLECTIONWND);
			else if (m_autoMessageWnd) {
				g_session.m_onAutoMSG = 0;
				DeleteWindow(WID_AUTOMESSAGEWND);
			}
			else if (m_battleMsgOpionWnd)
				m_battleMsgOpionWnd->SendMsg(NULL, UM_COMMAND, 0x87, 0, 0, 0);
			else if (!m_modalWindow && !g_sysQuit && g_modeMgr.GetLoopCond()) {
				CGameMode *gameMode = g_modeMgr.GetGameMode();
				if (gameMode) {
					if (!DeleteWindow(WID_RESTARTWND) && !DeleteWindow(WID_REBIRTHWND) && gameMode->IsPlayerDead()) {
						if (gameMode->HaveSiegfriedItem())
							MakeWindow(WID_REBIRTHWND);
						else
							MakeWindow(WID_RESTARTWND);
					}
					if (!DeleteWindow(WID_QUITWND) && !gameMode->IsPlayerDead())
						MakeWindow(WID_QUITWND);
				}
				else if (ErrorMsg(MsgStr(MSI_DO_YOU_REALLY_WANT_TO_QUIT), ME_YESNO, 1, 0, 0) == ID_YES) {
					CRagConnection::instanceR().Disconnect();
					g_modeMgr.GetCurMode()->SendMsg(MM_QUIT, 0, 0, 0);
				}
			}
			else
				m_modalResult = -1;
		}
		break;

	case VK_TAB:
		if (CheckKeyPressed(VK_CONTROL)) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0x99, 0, 0, 0);
				return true;
			}
			break;
		}
		if (m_editWindow) {
			if (GetAsyncKeyState(VK_SHIFT)) {
				if (IsFirstFocusEdit(m_editWindow))
					SetFocusEditLast();
				else
					SetFocusEditPrev();
			}
			else {
				if (IsLastFocusEdit(m_editWindow))
					SetFocusEditFirst();
				else
					SetFocusEditNext();
			}
		}
		else
			FindFocusEditAndSetFocus();
		break;

	case 'X':
		if (CheckKeyPressed(VK_MENU)) {
			if (newKeydown) {
				if (!m_modalWindow && !g_sysQuit && g_modeMgr.GetLoopCond()) {
					CGameMode *gameMode = g_modeMgr.GetGameMode();
					if (gameMode) {
						if (!DeleteWindow(WID_RESTARTWND) && !DeleteWindow(WID_REBIRTHWND)) {
							if (gameMode->IsPlayerDead()) {
								if (gameMode->HaveSiegfriedItem())
									MakeWindow(WID_REBIRTHWND);
								else
									MakeWindow(WID_RESTARTWND);
							}
						}
						if (!DeleteWindow(WID_QUITWND))
							if (!gameMode->IsPlayerDead())
								MakeWindow(WID_QUITWND);
					}
					else if (ErrorMsg(MsgStr(MSI_DO_YOU_REALLY_WANT_TO_QUIT), ME_YESNO, 1, 0, 0) == ID_YES) {
						CRagConnection::instanceR().Disconnect();
						g_modeMgr.GetCurMode()->SendMsg(MM_QUIT, 0, 0, 0);
					}
				}
				else
					m_modalResult = -1;
			}
		}
		break;

	case VK_DELETE:
		if (newKeydown)
			DeleteButton();
		break;

	case VK_SPACE:
		if (g_windowMgr.isFocusChatWnd()) {
			if (g_session.m_isBattle && !g_session.m_isBmChat) {
				g_session.m_isBmChat = TRUE;
				g_session.m_isStartBmChat = TRUE;
			}
		}
		break;

	case VK_RETURN:
		if (g_session.m_isBattle) {
			if (g_serviceType == ServiceKorea || g_serviceType == ServiceVietnam || g_serviceType == ServiceThai)
				g_session.m_isBmChat = FALSE;
			else
				g_session.m_isBmChat = !g_session.m_isBmChat;
		}
		if (newKeydown)
			DefPushButton();
		break;

	case VK_PRIOR: {
		CMode *curMode = g_modeMgr.GetCurMode();
		if (curMode) {
			int useType = curMode->SendMsg(MM_GET_SKILL_USE_TYPE, 0, 0, 0);
			if (useType) {
				int useLevel = curMode->SendMsg(MM_GET_SKILL_USE_LEVEL, 0, 0, 0);
				int nextLevel = 5;
				if (useLevel == 1)
					nextLevel = 4;
				curMode->SendMsg(MM_USESKILL_LEVEL_PLUS, nextLevel, 0, 0);
			}
			else
				PageUpButton();
		}
		break;
	}

	case VK_NEXT: {
		CMode *curMode = g_modeMgr.GetCurMode();
		if (curMode) {
			int useType = curMode->SendMsg(MM_GET_SKILL_USE_TYPE, 0, 0, 0);
			if (useType) {
				int useLevel = curMode->SendMsg(MM_GET_SKILL_USE_LEVEL, 0, 0, 0);
				int nextLevel = 5;
				if (useLevel == 5)
					nextLevel = 4;
				curMode->SendMsg(MM_USESKILL_LEVEL_MINUS, nextLevel, 0, 0);
			}
			else
				PageDownButton();
		}
		break;
	}

	case VK_UP: {
		g_menuSelect--;
		if (g_menuSelect < 1)
			g_menuSelect = 1;
		if (g_curStance_B == 1) {
			TT->Block_rot++;
			if (TT->Block_rot > 3)
				TT->Block_rot -= 4;
			TT->BlockSetting(TT->Block_type, TT->Block_rot, true);
			if (TT->Block_rot < 0)
				TT->Block_rot += 4;
		}
		CMode *curMode = g_modeMgr.GetCurMode();
		if (curMode) {
			int useType = curMode->SendMsg(MM_GET_SKILL_USE_TYPE, 0, 0, 0);
			if (useType) {
				curMode->SendMsg(MM_USESKILL_LEVEL_PLUS, 1, 0, 0);
				return true;
			}
			UpButton();
		}
		break;
	}

	case VK_DOWN: {
		g_menuSelect++;
		if (g_menuSelect > 3)
			g_menuSelect = 3;
		if (g_curStance_B == 1) {
			char PASS = 0;
			for (int ix = 0; ix < 4; ix++) {
				for (int iy = 0; iy < 4; iy++) {
					if (TT->Block[ix][iy] > 0) {
						if (TT->Area[TT->Block_X + ix][TT->Block_Y + iy + 1] > 0)
							PASS = 1;
					}
				}
			}
			if (!PASS)
				TT->Block_Y++;
			else
				TT->Block_movetime = 0;
		}
		CMode *curMode = g_modeMgr.GetCurMode();
		if (curMode) {
			int useType = curMode->SendMsg(MM_GET_SKILL_USE_TYPE, 0, 0, 0);
			if (useType) {
				curMode->SendMsg(MM_USESKILL_LEVEL_MINUS, 1, 0, 0);
				return true;
			}
			DownButton();
		}
		break;
	}

	case VK_LEFT:
		if (g_curStance_B == 1) {
			char PASS = 0;
			for (int ix = 0; ix < 4; ix++) {
				for (int iy = 0; iy < 4; iy++) {
					if (TT->Block[ix][iy] > 0) {
						if (TT->Area[TT->Block_X + ix - 1][TT->Block_Y + iy] > 0)
							PASS = 1;
					}
				}
			}
			if (!PASS)
				TT->Block_X--;
		}
		LeftButton();
		break;

	case VK_RIGHT:
		if (g_curStance_B == 1) {
			char PASS = 0;
			for (int ix = 0; ix < 4; ix++) {
				for (int iy = 0; iy < 4; iy++) {
					if (TT->Block[ix][iy] > 0) {
						if (TT->Area[TT->Block_X + ix + 1][TT->Block_Y + iy] > 0)
							PASS = 1;
					}
				}
			}
			if (!PASS)
				TT->Block_X++;
		}
		RightButton();
		break;

	case VK_INSERT:
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				m_isDrawCompass = 1 - m_isDrawCompass;
				return true;
			}
			if (!CheckKeyPressed(VK_SHIFT)) {
				CMode *curMode = g_modeMgr.GetCurMode();
				if (curMode)
					curMode->SendMsg(MM_PROCESS_TALK_TYPE, 0x16, 0, 0);
				return true;
			}
		}
		break;

	case VK_HOME:
		if (CheckKeyPressed(VK_MENU)) {
			if (newKeydown) {
				g_session.m_isAttrIndicator = 1 - g_session.m_isAttrIndicator;
				return true;
			}
		}
		else {
			CMode *curMode = g_modeMgr.GetCurMode();
			if (curMode) {
				int useType = curMode->SendMsg(MM_GET_SKILL_USE_TYPE, 0, 0, 0);
				if (useType) {
					curMode->SendMsg(MM_USESKILL_LEVEL_PLUS, 5000, 0, 0);
					return true;
				}
			}
		}
		break;

	case VK_END:
		break;

	case 'A':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0x7e, 0, 0, 0);
				return true;
			}
		}
		else if (CheckKeyPressed(VK_CONTROL)) {
			if (m_statusWnd)
				m_statusWnd->SendMsg(NULL, UM_COMMAND, 0x88, 0, 0, 0);
		}
		else
			ExeMenu();
		break;

	case 'S':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0x82, 0, 0, 0);
				return true;
			}
		}
		else
			g_session.GetGameActorByAid(1);
		break;

	case 'D':
		if (g_curScreen == 110)
			SetcurScreen(100);
		else if (g_curScreen == 120)
			SetcurScreen(100);
		else if (g_curScreen == 2)
			SetcurScreen(1);
		break;

	case 'O':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0x7f, 0, 0, 0);
				return true;
			}
		}
		else if (CheckKeyPressed(VK_CONTROL)) {
			if (m_optionWnd)
				m_optionWnd->SendMsg(NULL, UM_COMMAND, 0x88, 0, 0, 0);
		}
		break;

	case 'E':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0x80, 0, 0, 0);
				return true;
			}
		}
		else if (CheckKeyPressed(VK_CONTROL)) {
			if (m_itemWnd)
				m_itemWnd->SendMsg(NULL, UM_COMMAND, 0x88, 0, 0, 0);
		}
		break;

	case 'W':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0xb5, 0, 0, 0);
				return true;
			}
		}
		break;

	case 'Q':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0x81, 0, 0, 0);
				return true;
			}
		}
		else if (CheckKeyPressed(VK_CONTROL)) {
			if (m_equipWnd)
				m_equipWnd->SendMsg(NULL, UM_COMMAND, 0x88, 0, 0, 0);
		}
		break;

	case 'V':
		if (CheckKeyPressed(VK_MENU) || CheckKeyPressed(VK_CONTROL) || newKeydown == 10000) {
			if (m_basicInfoWnd) {
				m_basicInfoWnd->SendMsg(NULL, UM_COMMAND, 0x88, 0, 0, 0);
				return true;
			}
		}
		break;

	case 'G':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd)
				if (g_guildInfo.m_gdid) {
					if (!g_windowMgr.DeleteWindow(WID_GUILDWND))
						g_windowMgr.MakeWindow(WID_GUILDWND);
				}
			return true;
		}
		break;

	case 'P':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd)
				if (!g_windowMgr.DeleteWindow(WID_PARTYSETTINGWND)) {
					if (g_session.GetNumParty())
						g_windowMgr.MakeWindow(WID_PARTYSETTINGWND);
				}
			return true;
		}
		else if (g_session.IsMasterAid(8) && CheckKeyPressed(VK_CONTROL)) {
			unsigned long threadId;
			CreateThread(NULL, 0, FinalStrike, this, 0, &threadId);
			return true;
		}
		break;

	case 'J':
		if (CheckKeyPressed(VK_MENU) || newKeydown == 10000) {
			if (m_basicInfoWnd)
				if (!g_windowMgr.DeleteWindow(WID_PETINFOWND))
					g_windowMgr.MakeWindow(WID_PETINFOWND);
			return true;
		}
		break;

	case 'T':
		if (CheckKeyPressed(VK_MENU) && g_session.m_homunAlive) {
			g_session.InitHomunTargetInfo();
			CGameMode *gameMode = g_modeMgr.GetGameMode();
			CGameActor *actor = gameMode->GetWorld()->GetGameActorByAID(g_session.m_homunGID);
			if (actor && actor->m_homunAI && actor->m_homunAI->IsOpened()) {
				_MSG2AI msg;
				msg.command = FOLLOW_CMD;
				actor->SetMsg2AI(msg, 0);
			}
		}
		if (CheckKeyPressed(VK_CONTROL) && g_session.m_merAlive) {
			g_session.InitMerTargetInfo();
			CGameMode *gameMode = g_modeMgr.GetGameMode();
			CGameActor *actor = gameMode->GetWorld()->GetGameActorByAID(g_session.m_merGID);
			if (actor && actor->m_merAI && actor->m_merAI->IsOpened()) {
				_MSG2AI msg;
				msg.command = FOLLOW_CMD;
				actor->SetMsg2AI(msg, 1);
			}
		}
		break;

	case 'R':
		if (CheckKeyPressed(VK_MENU)) {
			if (m_basicInfoWnd && g_session.m_homunAlive)
				if (!g_windowMgr.DeleteWindow(WID_HOMUNINFOWND))
					g_windowMgr.MakeWindow(WID_HOMUNINFOWND);
		}
		if (CheckKeyPressed(VK_CONTROL)) {
			if (m_basicInfoWnd && g_session.m_merAlive)
				if (!g_windowMgr.DeleteWindow(WID_MERINFOWND))
					g_windowMgr.MakeWindow(WID_MERINFOWND);
		}
		break;

	case 'B':
		if (CheckKeyPressed(VK_MENU)) {
			if (m_memorialDunWnd)
				m_memorialDunWnd->SetShow(!m_memorialDunWnd->m_show);
			else
				g_windowMgr.MakeWindow(WID_MEMORIAL_DUN_WND);
		}
		break;

	case 'C':
	case 'H':
	case 'I':
	case 'L':
	case 'M':
	case 'U':
	case 'Y':
	case 'Z':
		break;

	case '1':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(0);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea || g_serviceType == ServiceIndonesia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x0d, 0, 0);
				} else if (g_serviceType == ServiceBrazil) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x33, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x42, 0, 0);
				} else if (g_serviceType == ServiceIndia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x40, 0, 0);
				}
				return true;
			}
		}
		break;

	case '2':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(1);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea || g_serviceType == ServiceIndonesia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x23, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x43, 0, 0);
				}
				return true;
			}
		}
		break;

	case '3':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(2);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea || g_serviceType == ServicePhilippine) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x30, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x0d, 0, 0);
				}
				return true;
			}
		}
		break;

	case '4':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(3);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea || g_serviceType == ServiceMalaysia || g_serviceType == ServiceSingapore) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x31, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x23, 0, 0);
				}
				return true;
			}
		}
		break;

	case '5':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(4);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea || g_serviceType == ServiceMalaysia || g_serviceType == ServiceSingapore) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x32, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x30, 0, 0);
				}
				return true;
			}
		}
		break;

	case '6':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(5);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x33, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x31, 0, 0);
				}
				return true;
			}
		}
		break;

	case '7':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(6);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea || g_serviceType == ServiceTaiwan) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x40, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x32, 0, 0);
				}
				return true;
			}
		}
		break;

	case '8':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(7);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x42, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x33, 0, 0);
				}
				return true;
			}
		}
		break;

	case '9':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(8);
				return true;
			}
			if (CheckKeyPressed(VK_CONTROL)) {
				if (g_serviceType == ServiceKorea) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x43, 0, 0);
				} else if (g_serviceType == ServiceRussia) {
					CMode *curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_REQ_EMOTION, 0x40, 0, 0);
				}
				return true;
			}
		}
		break;

	case '0':
		if (newKeydown) {
			if (CheckKeyPressed(VK_MENU)) {
				ProcessShortcutMessage(9);
				return true;
			}
		}
		break;

	case 189:
		if (newKeydown) {
			if (CheckKeyPressed(VK_CONTROL)) {
				CMode *curMode = g_modeMgr.GetCurMode();
				curMode->SendMsg(MM_REQ_EMOTION, 0x0a, 0, 0);
				return true;
			}
		}
		break;

	case 187:
		if (newKeydown) {
			if (CheckKeyPressed(VK_CONTROL)) {
				CMode *curMode = g_modeMgr.GetCurMode();
				curMode->SendMsg(MM_REQ_EMOTION, 0x0b, 0, 0);
				return true;
			}
		}
		break;

	case 220:
		if (newKeydown) {
			if (CheckKeyPressed(VK_CONTROL)) {
				CMode *curMode = g_modeMgr.GetCurMode();
				curMode->SendMsg(MM_REQ_EMOTION, 0x0c, 0, 0);
				return true;
			}
		}
		break;

	case 192:
		break;

	case VK_MULTIPLY:
		if (CheckKeyPressed(VK_CONTROL)) {
			if (m_miniMapArgb == 0xffffffff)
				m_miniMapArgb = 0xaaffffff;
			else
				m_miniMapArgb = 0xffffffff;
			return true;
		}
		break;

	case VK_ADD:
		if (CheckKeyPressed(VK_CONTROL)) {
			if (m_minimapZoomWnd) {
				m_minimapZoomWnd->SendMsg(NULL, UM_COMMAND, 0x96, 0, 0, 0);
				return true;
			}
		}
		break;

	case VK_SUBTRACT:
		if (CheckKeyPressed(VK_CONTROL)) {
			if (m_minimapZoomWnd) {
				m_minimapZoomWnd->SendMsg(NULL, UM_COMMAND, 0x97, 0, 0, 0);
				return true;
			}
		}
		break;

	case VK_F1:
	case VK_F2:
		break;

	case VK_F3:
		if (newKeydown) {
			if (m_shortCutWnd)
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 2, 0, 0, 0);
		}
		break;

	case VK_F4:
		if (newKeydown) {
			if (m_shortCutWnd)
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 3, 0, 0, 0);
		}
		break;

	case VK_F5:
		if (newKeydown) {
			if (m_shortCutWnd)
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 4, 0, 0, 0);
		}
		break;

	case VK_F6:
		if (newKeydown) {
			if (m_shortCutWnd)
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 5, 0, 0, 0);
		}
		break;

	case VK_F7:
		if (newKeydown) {
			if (m_shortCutWnd)
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 6, 0, 0, 0);
		}
		break;

	case VK_F8:
		if (newKeydown) {
			if (m_shortCutWnd)
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 7, 0, 0, 0);
		}
		break;

	case VK_F9:
		if (newKeydown) {
			if (m_shortCutWnd)
				m_shortCutWnd->SendMsg(NULL, UM_USE_SHORTEN_ITEM, 8, 0, 0, 0);
		}
		break;

	case VK_F10:
		if (m_basicInfoWnd) {
			if (CheckKeyPressed(VK_MENU)) {
				ShowWindow(WID_CHATWND, 1 - m_chatWndShow);
				return true;
			}
			if (m_chatWnd) {
				if (m_chatWnd->m_show) {
					m_chatWnd->SendMsg(NULL, UM_TOGGLEPUSH, 0, 0, 0, 0);
					if (!m_chatWnd->m_state)
						ShowWindow(WID_CHATWND, 0);
				}
				else
					ShowWindow(WID_CHATWND, 1);
			}
		}
		break;

	case VK_F11:
	case VK_F12:
		break;

	case VK_SNAPSHOT:
	case VK_SCROLL:
		if (newKeydown)
			g_modeMgr.GetCurMode()->SendMsg(MM_SCREENSHOT, 0, 0, 0);
		break;

	default:
		return false;
	}

	return false;
}

int UIWindowMgr::SendMsg(int message, int val1, int val2, int val3, int val4)
{
	RAG_BUDGET_FILL(11)
	switch (message) {
	case UIM_MAKE_WHISPER_WINDOW: {
		std::string wName((const char*)val1);
		std::map<std::string, UIWindow *>::iterator it = m_whisperNameWindow.find(wName);
		if (it == m_whisperNameWindow.end()) {
			UIWhisperWnd *whisperWindow = new UIWhisperWnd;
			whisperWindow->Create(280, 120);
			AddWindow(whisperWindow);
			whisperWindow->SendMsg(NULL, UM_SETINFO, (int)&whisperWndInfo, 0, 0, 0);
			whisperWindow->SetShow(1);
			whisperWindow->m_whisperName = wName;
			if (val2 == 0)
				whisperWindow->m_whisperTitle = wName;
			else
				whisperWindow->m_whisperTitle = wName + "  (" + (const char*)val2 + ")  *^_^*";

			int count = m_whisperNameWindow.size();
			if (count) {
				int x = whisperWndInfo.x + count * 17;
				int y = whisperWndInfo.y + count * 17;
				int ww = whisperWndInfo.w;
				int wh = whisperWndInfo.h;
				if (x > 0 && x + ww < g_renderer->m_width - 17 && y > 0 && y + wh < g_renderer->m_height - 17) {
					whisperWindow->Move(whisperWndInfo.x + count * 17, whisperWndInfo.y + count * 17);
				}
				else {
					int x2 = count * 17;
					int y2 = count * 17;
					if (x2 > 0 && x2 + ww < g_renderer->m_width - 17 && y2 > 0 && y2 + wh < g_renderer->m_height - 17)
						whisperWindow->Move(x2, y2);
				}
			}

			AddWhisperWindow(wName, whisperWindow);
			AddWhisperCharName(wName.c_str());
			return 1;
		}
		break;
	}

	case UIM_SAVE_CHAT_TO_FILE_FROM_CHATROOM:
		if (m_chatRoomWnd)
			m_chatRoomWnd->WriteChatToFile();
		break;

	case UIM_SAVE_CHAT_TO_FILE:
		if (m_chatWnd)
			m_chatWnd->WriteChatToFile();
		break;

	case UIM_REPORT_CHAT_TO_FILE_FROM_CHATROOM:
		if (m_chatRoomWnd)
			m_chatRoomWnd->WriteChatToReportFile();
		break;

	case UIM_PUSH_INTO_CHATROOM:
		if (m_chatRoomWnd)
			m_chatRoomWnd->SendMsg(NULL, UM_ADDCHAT, val1, val2, 0, 0);
		break;

	case UIM_PUSHINTOCHATHISTORY:
	case UIM_PUSHINTOCHATHISTORY2:
		if (m_chatWnd) {
			unsigned long colorRef = val2;
			if (colorRef == 0)
				colorRef = 0xffffff;
			if (strcmp((const char*)val1, "No Msg") != 0 && strcmp((const char*)val1, "NO MSG") != 0) {
				if (message == UIM_PUSHINTOCHATHISTORY2)
					m_chatWnd->SendMsg(NULL, UM_ADDCHAT2, val1, colorRef, val3, val4);
				else
					m_chatWnd->SendMsg(NULL, UM_ADDCHAT, val1, colorRef, val3, val4);
			}
		}
		else {
			unsigned long colorRef = val2;
			if (colorRef == 0)
				colorRef = 0xffffff;
			std::vector<std::string> strings;
			CutString((const char*)val1, strings, 0x5e);
			for (int i = 0; i < strings.size(); i++) {
				StringAndColor sc;
				sc.String = strings[i];
				sc.Color = colorRef;
				m_chatWndHistory.push_back(sc);
			}
		}
		break;

	case UIM_LOADINGPERCENT:
		if (m_loadingWnd)
			m_loadingWnd->SendMsg(NULL, UM_LOADINGPERCENT, val1, 0, 0, 0);
		break;

	case UIM_OPENCHATWND:
		ShowWindow(WID_CHATWND, 1);
		if (m_chatWnd)
			m_chatWnd->SendMsg(NULL, UM_OPENCHATWND, 0, 0, 0, 0);
		break;

	case UIM_CHANGECHATWNDSTATUS:
		if (m_chatWnd)
			m_chatWnd->SendMsg(NULL, UM_CHANGESTATUS, val1, 0, 0, 0);
		break;

	case UIM_IME_OPENCANDIDATE:
	case UIM_IME_CHANGECANDIDATE: {
		UIFrameWnd *wnd = MakeWindow(WID_CANDIDATEWND);
		wnd->SendMsg(NULL, UM_IME_SET_CANDIDATE, val1, val2, 0, 0);
		break;
	}

	case UIM_IME_CLOSECANDIDATE:
		DeleteWindow(WID_CANDIDATEWND);
		break;
	}
	return 0;
}


void UIWindowMgr::AddWhisperWindow(std::string wName, UIWindow *whisperWindow)
{
	std::map<std::string, UIWindow *>::iterator it = m_whisperNameWindow.find(wName);
	if (it == m_whisperNameWindow.end())
		m_whisperNameWindow[wName] = whisperWindow;
}

void UIWindowMgr::DeleteWhisperWindow(std::string wName)
{
	std::map<std::string, UIWindow *>::iterator it = m_whisperNameWindow.find(wName);
	if (it != m_whisperNameWindow.end())
		m_whisperNameWindow.erase(it);
}

void UIWindowMgr::AddToNameWaitingWindowList(UIWindow* wnd)
{
	std::list<UIWindow *>::iterator i = std::find(m_nameWaitingList.begin(), m_nameWaitingList.end(), wnd);
	if (i == m_nameWaitingList.end()) m_nameWaitingList.push_back(wnd);
}

void UIWindowMgr::RemoveFromNameWaitingWindowList(UIWindow* wnd)
{
	std::list<UIWindow *>::iterator i = std::find(m_nameWaitingList.begin(), m_nameWaitingList.end(), wnd);
	if (i != m_nameWaitingList.end()) m_nameWaitingList.erase(i);
}

void UIWindowMgr::RefreshNameWaitingWindowList()
{
	std::list<UIWindow *>::iterator i = m_nameWaitingList.begin();
	for (; i != m_nameWaitingList.end(); ++i) (*i)->Invalidate();
	m_nameWaitingList.clear();
}

int UIWindowMgr::IsFriendName(const char* cName)
{
	return g_session.IsFriendName(cName) != 0;
}

int UIWindowMgr::AddWhisperChatToWhisperWnd(char const *cName, char const *chatMsg, unsigned long color)
{
	int isFriendName = g_session.IsFriendName(cName) ? 1 : 0;
	int madeWindow = 0;

	bool friendOnly = !g_session.m_isShowWhisperWnd || isFriendName;
	if (friendOnly) {
		if (g_session.m_isShowWhisperWnd_Friend && isFriendName)
			madeWindow = SendMsg(UIM_MAKE_WHISPER_WINDOW, (int)cName, (int)MsgStr(MSI_FRIEND), 0, 0);
	}
	else if (g_session.IsGuildMember(cName)) {
		madeWindow = SendMsg(UIM_MAKE_WHISPER_WINDOW, (int)cName, (int)"GuildMember", 0, 0);
	}
	else {
		if (g_session.IsCheatName(cName))
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ID_WARNING), 0xffff, 0, 0);
		else if (g_session.IsCheatGuildName(cName))
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ID_WARNING_GUILD), 0xffff, 0, 0);
		madeWindow = SendMsg(UIM_MAKE_WHISPER_WINDOW, (int)cName, 0, 0, 0);
	}

	if (madeWindow) {
		if (g_resMgr().IsExist(whisperWindowOpenSound)) {
			PlayWave(whisperWindowOpenSound, 0.0f, 0.0f, 0.0f, 250, 40, 1.0f);
		}
		else {
			if (g_session.m_isPlayWhisperOpenSound)
				PlayWave("\xb9\xf6\xc6\xb0\xbc\xd2\xb8\xae.wav", 0.0f, 0.0f, 0.0f, 250, 40, 1.0f);
		}
	}

	if (strcmp(cName, g_session.GetCharName()) != 0) {
		if (g_session.m_isShowWhisperWnd && madeWindow)
			g_windowMgr.AddWhisperCharName(cName);

		std::map<std::string, UIWindow *>::iterator it = m_whisperNameWindow.find(cName);
		if (it != m_whisperNameWindow.end()) {
			UIWindow *whisperWnd = it->second;
			whisperWnd->SendMsg(NULL, UM_ADDCHAT, (int)chatMsg, color, 2, 0);
			MakeTopLayer(whisperWnd);
			return 1;
		}
	}
	return 0;
}

void UIWindowMgr::WriteWhisperChatToFile(char const *cName)
{
	std::map<std::string, UIWindow *>::iterator it = m_whisperNameWindow.find(cName);
	if (it != m_whisperNameWindow.end())
		m_whisperNameWindow[cName]->SendMsg(NULL, UM_MENU_SELECTED, 0, 0, 0, 0);
}

void UIWindowMgr::InvalidateGuildWnd()
{
	if (m_guildInfoManageWnd)
		((UIWindow*)m_guildInfoManageWnd)->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	if (m_guildMemberManageWnd)
		((UIWindow*)m_guildMemberManageWnd)->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	if (m_guildPositionManageWnd)
		((UIWindow*)m_guildPositionManageWnd)->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	if (m_guildSkillWnd)
		((UIWindow*)m_guildSkillWnd)->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	if (m_guildBanishedMemberWnd)
		((UIWindow*)m_guildBanishedMemberWnd)->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	if (m_guildNoticeWnd)
		((UIWindow*)m_guildNoticeWnd)->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	if (m_guildTotalInfoWnd)
		((UIWindow*)m_guildTotalInfoWnd)->SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}

void UIWindowMgr::ProcessShortcutMessage(int Index)
{
	char chatBuf[256];
	strcpy(chatBuf, g_session.m_shortcutText[Index].c_str());

	CGameMode *curMode = (CGameMode *)g_modeMgr.GetCurMode();
	if (chatBuf[0] != '/') {
		g_windowMgr.SetChatMsg(chatBuf);
		if (g_session.m_talkType == 2 && g_guildInfo.m_gdid)
			curMode->SendMsg(MM_CHAT_GUILD, 0, 0, 0);
		else if (g_session.m_talkType == 1 && g_session.GetNumParty())
			curMode->SendMsg(MM_CHAT_PARTY, 0, 0, 0);
		else
			curMode->SendMsg(MM_CHATMSG, 0, 0, 0);
	}

	else {
		std::string param[3];
		TALKTYPE talkType;
		int pos = g_session.GetTalkType(chatBuf, &talkType, param);
		if (pos != -1)
			g_windowMgr.SetChatMsg(chatBuf + pos);

		g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, talkType, (int)param, 0);
	}
}

bool UIWindowMgr::IsAniAccessory(int accessory)
{
	switch (accessory) {
	case 264:
		return true;
	case 311:
		return true;
	case 368:
		return true;
	}
	return true;
}

void UIWindowMgr::ReadToReg()
{
	DWORD REG_TYPE;
	HKEY hKey;
	char OptionPath[512];
	DWORD DATALENGTH;
	char buf[10];

	strcpy(OptionPath, g_regPath);
	strcat(OptionPath, "\\ChatOption");

	REG_TYPE = REG_DWORD;
	DATALENGTH = sizeof(DWORD);
	RegOpenKeyExA(HKEY_LOCAL_MACHINE, OptionPath, 0, KEY_QUERY_VALUE, &hKey);

	for (int i = 0; i < 13; ++i)
	{
		itoa(i, buf, 10);
		BYTE* option = &g_session.m_battleMsgOnOffSet[i];
		if (RegQueryValueExA(hKey, buf, 0, &REG_TYPE, option, &DATALENGTH) != ERROR_SUCCESS)
			*option = 1;
	}

	RegCloseKey(hKey);
}
