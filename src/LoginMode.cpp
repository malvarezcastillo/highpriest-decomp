#include "common.h"
#include "MsgStrings.h"
#include "framework/Locale.h"
#include "device/Sound.h"
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "LoginMode.h"
#include "Session.h"
#include "GuildInfo.h"
#include "window/UISys.h"
#include "window/UIFrameWnd.h"
#include "device/Connection.h"
#include "device/Control.h"
#include "device/md5.h"
#include "3dlib/Device.h"
#include "base/Res.h"
#include "base/FindHack.h"
#include "base/xml.h"
#include "3dlib/Renderer.h"
#include "pdb_enums.h"
#include "packet.h"
#include "PaddingPacket.h"

extern const char* MsgStr(MSGSTRINGID id);
extern const char* UIBmp(const char* uiBmpName);
extern CSession g_session;
extern CMouse g_mouse;
extern UIWindowMgr g_windowMgr;
extern CRenderer* g_renderer;
extern ServerAddress g_zoneServerAddr;
extern CModeMgr g_modeMgr;
extern ServerAddressEx g_charServerAddr;
extern bool g_pingLog;
extern std::list<DWORD> g_pingLogList;
extern DWORD g_pingLogStart;
extern int g_mustPumpOutReceiveQueue;
extern CHARACTER_INFO g_charInfo;
extern ServiceType g_serviceType;
extern ServerType g_serverType;
extern int g_version;
extern bool g_hideAccountList;
extern int g_loginStartMode;
extern int g_isStopByLogin;
extern char g_isGravityID;
extern char g_multiSTOP;
extern char g_passwordWrong;
extern bool g_passwordEncrypt2;
extern bool g_passwordEncrypt;
extern bool g_multiLang;
extern bool g_bUseCommand;
extern const char* TITLE_FILE;
extern const char* g_accountAddr;
extern const char* g_accountPort;

#include <Nb30.h>

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff [30];
}ASTAT, * PASTAT;

extern _ASTAT_ Adapter;
extern int g_bOldServer;


int g_loginStartMode = 3;
ServerAddressEx g_charServerAddr;
CHARACTER_INFO g_charInfo;
_ASTAT_ Adapter;
ServerAddress g_zoneServerAddr;



#pragma comment(lib, "Netapi32.lib")
int g_bOldServer = 0;
int g_isStopByLogin = 0;
bool g_bUseCommand = false;
extern int g_se_go;
extern int g_isSkillEntry;
extern char g_FirstEffect;
extern void WinMainNpKeyStartEncryption();
extern void WinMainNpKeyStopEncryption();
extern BOOL WINAPI InitNPKCryptFuncTbl();
extern int NKCTestCompatibility();
extern const char* GetLicenceText();
extern BOOL SkipSceneRendering();
extern void SendID(char *id);
extern XMLElement *GetClientInfo();
extern void SelectClientInfo(int number);
extern void SelectClientInfo2(int number, int number2);
int GetAccountType();



void SendUDPPacket(LPCSTR szURL, USHORT uPort, const char* p, int pLen)
{
	SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);

	if (s != INVALID_SOCKET) {
		hostent* h = gethostbyname(szURL);
		if (h) {
			sockaddr_in servAddr;
			servAddr.sin_family = AF_INET;
			memcpy(&servAddr.sin_addr.S_un.S_un_b, h->h_addr_list[0], 4);
			servAddr.sin_port = htons(uPort);
			sendto(s, p, pLen, 0, (sockaddr*)&servAddr, sizeof(servAddr));
		}
		closesocket(s);
	}
}

CLoginMode::CLoginMode() : CMode()
{
	m_fadeInCount = TRUE;
	m_syncRequestTime = timeGetTime() + 12000;
	m_multiLang = g_multiLang;
	g_multiLang = false;
	m_nSelectedAccountNo = -1;
	m_wndWait = NULL;
}

CLoginMode::~CLoginMode()
{
	g_multiLang = m_multiLang;
}


void CLoginMode::OnInit(const char * modeName)
{
	CloseStream();
	g_mouse.ResetButtonState();

	for (int p = 0; p < 6; ++p)
		m_charParam[p] = 5;
	m_charParam[6] = 0;
	m_charParam[7] = 1;

	m_makingCharName[0] = 0;
	m_emaiAddress[0] = 0;
	m_wallPaperBmpName = TITLE_FILE;
	m_loopCond = 1;
	m_nextSubMode = -1;

	InitNPKCryptFuncTbl();
	if (g_serviceType == ServiceThai || g_serviceType == ServiceKorea || g_serviceType == ServiceJapan) {
		if (!NKCTestCompatibility()) {
			g_windowMgr.ErrorMsg(MsgStr(MSI_NPGAMEMON_XP_COMPATIBILITY), ME_OK, TRUE, FALSE, 0);
			SendMsg(MM_QUIT, 0, 0, 0);
		}
	}

	SetSubState(g_loginStartMode);

	g_loginStartMode = LSM_IDENTRY;
	g_isStopByLogin = 1;
	g_session.InitWithClear();
	g_guildInfo.Init();
	PreLoadAlwaysNeededRes();
	if (!g_session.m_bgmIsPaused) PlayStream("bgm\\01.mp3");

	memset(&m_billingInfo, 0, sizeof(m_billingInfo));
}



void CLoginMode::PreLoadAlwaysNeededRes()
{
	g_session.PreLoadAlwaysNeededRes();
}

int CLoginMode::OnRun()
{
	while (m_loopCond && !g_sysQuit) {
		if (!g_windowMgr.IsModalStatus()) {
			if (m_nextSubMode != -1) {
				SetSubState(m_nextSubMode);
			}
		}
		OnUpdate();
		m_subModeCnt++;
	}
	RunFadeOut(0);
	return TRUE;
}




void CLoginMode::CheckPingTime()
{
	if (m_subMode == LSM_SELECTCHAR || m_subMode == LSM_MAKECHAR ||
		m_subMode == LSM_WAITRESP_FROM_CHSVR_DELETECHAR ||
		m_subMode == LSM_SELECTCHAR2)
	{
		if (timeGetTime() > m_syncRequestTime) {
			Trace("CharServer Ping !");
			m_syncRequestTime = timeGetTime() + 12000;
			PACKET_PING p;
			p.PacketType = HEADER_PING;
			p.AID = g_session.GetAid();
			CRagConnection::instanceR().SendPacket(
				CRagConnection::instanceR().GetPacketSize(HEADER_PING),
				(const char *)&p);
		}
	}
}




void CLoginMode::CheckExeHashFromAccServer()
{
	BOOL bCheckExe = FALSE;

	if ((ServiceIndonesia == g_serviceType) ||
		(ServiceKorea == g_serviceType && ServerSakray == g_serverType))
	{
		bCheckExe = TRUE;
	}

	PACKET_CA_EXE_HASHCHECK pHashCheck;
	pHashCheck.PacketType = HEADER_CA_EXE_HASHCHECK;
	if (!bCheckExe || !GenerateExeHash(pHashCheck.HashValue)) {
		return;
	}

	BYTE MainHash[16] = { 0x65, 0xE3, 0x64, 0x31, 0x03, 0x1C, 0x03, 0x11,
						0x1E, 0x50, 0x6D, 0x13, 0x39, 0x69, 0x2D, 0x7A };
	BYTE SakHash[16] = { 0x82, 0xD1, 0x2C, 0x91, 0x4F, 0x5A, 0xD4, 0x8F,
						0xD9, 0x6F, 0xCF, 0x7E, 0xF4, 0xCC, 0x49, 0x2D };

	if (ServiceKorea == g_serviceType) {
		if (ServerSakray == g_serverType) {
			memcpy(pHashCheck.HashValue, SakHash, sizeof(SakHash));
		} else if (ServerPrimary == g_serverType) {
			memcpy(pHashCheck.HashValue, MainHash, sizeof(SakHash));
		} else {
			ZeroMemory(pHashCheck.HashValue, sizeof(pHashCheck.HashValue));
		}
	} else if (ServiceIndonesia == g_serviceType) {
		if (ServerSakray == g_serverType) {
			BYTE SakHash[16] = { 0xC7, 0x0A, 0x94, 0xC2, 0x7A, 0xCC, 0x38, 0x9A,
								0x47, 0xF5, 0x54, 0x39, 0x7C, 0xA4, 0xD0, 0x39 };
			memcpy(pHashCheck.HashValue, SakHash, sizeof(SakHash));
		}
	}

	CRagConnection::instanceR().SendPacket(
		CRagConnection::instanceR().GetPacketSize(pHashCheck.PacketType),
		(const char *)&pHashCheck);
}




int CLoginMode::GenerateExeHash(unsigned char Hash[16])
{
	MD5 exehash;
	exehash.init();

	char szFile[MAX_PATH];
	GetModuleFileName(g_hInstance, szFile, sizeof(szFile));

	FILE *exefile = fopen(szFile, "rb");
	if (exefile) {
		unsigned char btFile[1024];
		while (0 == feof(exefile)) {
			int cbRead = fread(btFile, 1, sizeof(btFile), exefile);
			exehash.append(btFile, cbRead);
		}
		exehash.finish(Hash);
		return TRUE;
	}

	return FALSE;
}

void CLoginMode::OnUpdate()
{
	CheckSystemMessage();
	PollNetworkStatus();
	CheckPingTime();

	g_mouse.ReadState();
	g_windowMgr.ProcessInput();
	ProcessKeyBoard();

	if (m_loopCond) {
		OnProcessState(m_subMode);
		ProcessHelpBalloon();
	}

	if (g_activatedNow) {
		g_activatedNow = FALSE;
		g_3dDevice.RestoreSurfaces();
		g_windowMgr.InvalidateAll();
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
	}

	if (g_isAppActive || !g_3dDevice.IsFullscreen()) {
		if (SkipSceneRendering()) {
			g_renderer->Clear(TRUE);
			g_windowMgr.OnProcess();

			if (g_serviceType == ServiceKorea || g_serviceType == ServiceTaiwan ||
				g_serviceType == ServiceMalaysia || g_serviceType == ServiceSingapore ||
				g_serviceType == ServiceRussia || g_serviceType == ServiceChina ||
				g_serviceType == ServiceGermany || g_serviceType == ServiceAmerica ||
				g_serviceType == ServiceIndia)
			{
				g_windowMgr.RenderTitleGraphic();
				if (m_subMode == LSM_WARNING)
					g_windowMgr.RenderWallPaper();
			} else {
				g_windowMgr.RenderWallPaper();
			}

			g_windowMgr.Render();
			DrawMouseCursor();
			ProcessFadeIn();

			if (g_renderer->DrawScene())
				g_renderer->Flip(TRUE);
		}
		ScreenShot();
	}
}






void CLoginMode::OnExit()
{
	EndDigitalSound();
	g_resMgr().UnloadUnlockedRes();
	g_renderer->ReleaseCacheSurfaces();
	m_strErrorInfo.resize(0);
}

const char * GetUserCountString(WORD usercount)
{
	if (usercount <= 1000)
		return MsgStr(MSI_LITTLEUSER);
	else if (usercount <= 3000)
		return MsgStr(MSI_ALITTLEUSER);
	else if (usercount <= 6000)
		return MsgStr(MSI_NORMLEUSER);
	else if (usercount <= 10000)
		return MsgStr(MSI_MANYUSER);
	else
		return MsgStr(MSI_VERYMANYUSER);
}





void CLoginMode::OnChangeState(int state)
{
	m_subModeStartTime = timeGetTime();
	g_windowMgr.RemoveAllWindows();
	g_session.m_isBattle = false;
	m_isConnected = true;

	switch (state) {
	case LSM_WARNING:
		if (g_serviceType == ServiceKorea)
			m_wallPaperBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\warning.bmp";
		else
			m_wallPaperBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\warning.bmp";
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		g_windowMgr.MakeWindow(WID_NOTICECONFIRMWND)->SendMsg(NULL, UM_SET_TARGET, LMM_GOTOSELECTACCOUNT, 0, 0, 0);
		break;
	case LSM_LICENCE:
	{
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		m_wallPaperBmpName = TITLE_FILE;
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_SAYDIALOGWND);
		wnd->Resize(400, 300);
		wnd->Move((g_renderer->m_width - 640) / 2 + 120, (g_renderer->m_height - 480) / 2 + 90);
		wnd->SendMsg(NULL, UM_ADD_BUTTON, 0x87, (int)"btn_disagree", 0, 0);
		wnd->SendMsg(NULL, UM_ADD_BUTTON, 0x98, (int)"btn_agree", 0, 0);
		if (g_serviceType == ServiceIndonesia)
			wnd->SendMsg(NULL, UM_SETSAYDIALOG, (int)GetLicenceText(), 0, 0x35, 0);
		else
			wnd->SendMsg(NULL, UM_SETSAYDIALOG, (int)GetLicenceText(), 0, 0x3a, 0);
		wnd->SendMsg(NULL, UM_SET_TARGET, LMM_GOTOSELECTACCOUNT, 0, 0, 0);
		break;
	}
	case LSM_SELECTACCOUNT:
		if (InitAccountInfo()) {
			m_wallPaperBmpName = TITLE_FILE;
			g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
			UISelectServerWnd *selectServerWnd = (UISelectServerWnd *)g_windowMgr.MakeWindow(WID_SELECTSERVERWND);
			selectServerWnd->SendMsg(NULL, UM_SET_TARGET, LMM_SELECTACCOUNT, (int)"NUMACCOUNT", 0, 0);
			std::vector<accountInfo>::iterator iter = m_accountInfo.begin();
			while (iter != m_accountInfo.end()) {
				selectServerWnd->m_serverList->AddItem(iter->display.c_str());
				selectServerWnd->m_serverList->m_strings.push_back(iter->balloon);
				++iter;
			}
			selectServerWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			selectServerWnd->SendMsg(NULL, UM_ENABLE_EXIT, 0, 0, 0, 0);
		} else {
			SendMsg(MM_GOTOIDENTRY, 0, 0, 0);
		}
		break;
	case LSM_SELECTACCOUNT2:
		if (InitAccountInfo2(m_nSelectedAccountNo)) {
			m_wallPaperBmpName = TITLE_FILE;
			g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
			UISelectServerWnd *selectServerWnd = (UISelectServerWnd *)g_windowMgr.MakeWindow(WID_SELECTSERVERWND);
			selectServerWnd->SendMsg(NULL, UM_SET_TARGET, LMM_SELECTACCOUNT2, (int)"NUMACCOUNT", 0, 0);
			std::vector<accountInfo>::iterator iter = m_accountInfo2.begin();
			while (iter != m_accountInfo2.end()) {
				selectServerWnd->m_serverList->AddItem(iter->display.c_str());
				selectServerWnd->m_serverList->m_strings.push_back(iter->balloon);
				++iter;
			}
			selectServerWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			selectServerWnd->SendMsg(NULL, UM_ENABLE_EXIT, 0, 0, 0, 0);
		} else {
			SendMsg(MM_GOTOIDENTRY, 0, 0, 0);
		}
		break;
	case LSM_IDENTRY:
	{
		WinMainNpKeyStartEncryption();
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_LOGINWND);
		if (!g_hideAccountList) {
			if (wnd)
				wnd->SendMsg(NULL, UM_ENABLE_CANCEL, 0, 0, 0, 0);
		}
		break;
	}
	case LSM_SELECTSVR:
	{
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		UISelectServerWnd *selectServerWnd = (UISelectServerWnd *)g_windowMgr.MakeWindow(WID_SELECTSERVERWND);
		selectServerWnd->SendMsg(NULL, UM_SET_TARGET, LMM_SELECTSVR, (int)"NUMSERVER", 0, 0);
		for (int i = 0; i < m_numServer; ++i) {
			char serverName[64] = "";
			if (m_serverInfo[i].property == SERVERPROPERTY_NEW) sprintf(serverName, MsgStr(MSI_SERVER_PROPERTY_NEW));

			if (!strcmp((char *)m_serverInfo[i].name, "\xc5\xd7\xc0\xcf\xb8\xb5"))
				sprintf(serverName + strlen(serverName), "%s - %s", m_serverInfo[i].name, "Lydia+Fenrir");
			else if (!strcmp((char *)m_serverInfo[i].name, "\xb1\xdb\xb6\xf3\xb8\xae\xbd\xba"))
				sprintf(serverName + strlen(serverName), "%s - %s", m_serverInfo[i].name, "Iris+Thor");
			else if (!strcmp((char *)m_serverInfo[i].name, "\xbc\xd2\xb8\xb0"))
				sprintf(serverName + strlen(serverName), "%s - %s", m_serverInfo[i].name, "Loki+Baldur");
			else if (!strcmp((char *)m_serverInfo[i].name, "\xb5\xf0\xc6\xf7\xb8\xa3\xc5\xd7"))
				sprintf(serverName + strlen(serverName), "%s - %s", m_serverInfo[i].name, "Chaos+Odin");
			else
				sprintf(serverName + strlen(serverName), (char *)m_serverInfo[i].name);

			if (ServiceChina == g_serviceType || g_serviceType == ServiceMalaysia || g_serviceType == ServiceSingapore)
			{
			}

			else if ((g_serviceType == ServiceBrazil) || (ServiceVietnam == g_serviceType))
			{
			}
			else
			{
				switch (m_serverInfo[i].state) {
				case SERVERSTATE_NORMAL:
					sprintf(serverName + strlen(serverName), MsgStr(MSI_SERVER_USER_COUNT), m_serverInfo[i].usercount);
					break;
				case SERVERSTATE_INSPECT:
					sprintf(serverName + strlen(serverName), MsgStr(MSI_SERVER_INSPECTING));
					break;
				case SERVERSTATE_ADULT:
					sprintf(serverName + strlen(serverName), MsgStr(MSI_SERVER_PROPERTY_ADULT), m_serverInfo[i].usercount);
					break;
				case SERVERSTATE_PAY:
					sprintf(serverName + strlen(serverName), MsgStr(MSI_SERVER_PROPERTY_PAY), m_serverInfo[i].usercount);
					break;
				case SERVERSTATE_FREE:
					sprintf(serverName + strlen(serverName), MsgStr(MSI_SERVER_PROPERTY_FREE), m_serverInfo[i].usercount);
					break;
				}
			}
			selectServerWnd->m_serverList->AddItem(serverName);
		}
		selectServerWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
		break;
	}
	case LSM_SELECTCHAR:
		g_se_go = 0;
		g_FirstEffect = 0;
		g_isSkillEntry = 0;
		g_session.m_cartCurCount = 0;
		g_session.m_cartCurWeight = 0;
		g_session.m_petJob = -1;
		g_session.SetEquipArrowIndwx(0);
		g_session.m_isDA_illusionON = FALSE;
		g_session.ClearQuestList();
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		g_windowMgr.MakeWindow(WID_SELECTCHARWND);
		break;
	case LSM_MAKECHAR:
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		g_windowMgr.MakeWindow(WID_MAKECHARWND);
		break;
	case LSM_WAITRESP_FROM_ACSVR:
	{
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		ServerAddress addr;
		strcpy(addr.ip, g_accountAddr);
		addr.port = atoi(g_accountPort);
		m_isConnected = CRagConnection::instanceR().Connect(&addr);
		if (m_isConnected) {
			if (g_bUseCommand) {
				PACKET_CA_CONNECT_INFO_CHANGED pCmd;
				pCmd.PacketType = HEADER_CA_CONNECT_INFO_CHANGED;
				memcpy((char *)pCmd.ID, m_userId, sizeof(pCmd.ID));
				CRagConnection::instanceR().SendPacket(
					CRagConnection::instanceR().GetPacketSize(pCmd.PacketType),
					(char *)&pCmd);
			}

			CheckExeHashFromAccServer();
			if (!g_passwordEncrypt) {
				if (g_serviceType == ServiceKorea) {
					PACKET_CA_LOGIN_HAN p;
					p.PacketType = HEADER_CA_LOGIN_HAN;
					p.Version = g_version;
					memcpy((char *)p.ID, m_userId, sizeof(p.ID));
					memcpy((char *)p.Passwd, m_userPassword, sizeof(p.Passwd));
					p.clienttype = GetAccountType();

					NCB ncb;
					memset(&ncb, 0, sizeof(ncb));
					ncb.ncb_command = NCBRESET;
					ncb.ncb_lana_num = 0;
					Netbios(&ncb);
					memset(&ncb, 0, sizeof(ncb));
					ncb.ncb_lana_num = 0;
					ncb.ncb_command = NCBASTAT;
					strcpy((char *)ncb.ncb_callname, "*               ");
					ncb.ncb_buffer = (unsigned char *)&Adapter;
					ncb.ncb_length = sizeof(Adapter);
					UCHAR uRetCode = Netbios(&ncb);
					if (uRetCode == 0) {
						sprintf((char *)p.m_szMacAddr, "%02x%02x%02x%02x%02x%02x\n",
							Adapter.adapt.adapter_address[0],
							Adapter.adapt.adapter_address[1],
							Adapter.adapt.adapter_address[2],
							Adapter.adapt.adapter_address[3],
							Adapter.adapt.adapter_address[4],
							Adapter.adapt.adapter_address[5]);
					} else {
						strcpy((char *)p.m_szMacAddr, "111111111111");
					}

					char host[100];
					gethostname(host, sizeof(host));
					hostent *hostInfo = gethostbyname(host);
					if (!hostInfo) {
						strcpy(p.m_szIP, "111.111.111.111");
					} else {
						strcpy(p.m_szIP, inet_ntoa(*(in_addr *)hostInfo->h_addr_list[0]));
					}
					p.isHanGameUser = g_isGravityID;
					CRagConnection::instanceR().SendPacket(
						CRagConnection::instanceR().GetPacketSize(p.PacketType),
						(char *)&p);
				} else {
					PACKET_CA_LOGIN p;
					p.PacketType = HEADER_CA_LOGIN;
					p.Version = g_version;
					memcpy((char *)p.ID, m_userId, sizeof(p.ID));
					memcpy((char *)p.Passwd, m_userPassword, sizeof(p.Passwd));
					p.clienttype = GetAccountType();
					CRagConnection::instanceR().SendPacket(
						CRagConnection::instanceR().GetPacketSize(p.PacketType),
						(char *)&p);
				}
				UIWaitWnd *wnd = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
				wnd->SetMsg(MsgStr(MSI_WAITING_RESPONSE_FROM_SERVER), 16, 1);
			} else {
				PACKET_CA_REQ_HASH p;
				p.PacketType = HEADER_CA_REQ_HASH;
				CRagConnection::instanceR().SendPacket(
					CRagConnection::instanceR().GetPacketSize(p.PacketType),
					(char *)&p);
				m_wndWait = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
				m_wndWait->SetMsg(MsgStr(MSI_WAITING_RESPONSE_FROM_SERVER), 16, 1);
			}
		} else {
			CRagConnection::instanceR().Disconnect();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED), ME_OKCANCEL, TRUE, TRUE, 0) != ID_MODAL_PASSED) {
				g_mustPumpOutReceiveQueue = 0;
				SetNextSubState(LSM_IDENTRY);
			}
		}
		break;
	}
	case LSM_WAITRESP_FROM_CHSVR:
	{
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		CRagConnection::instanceR().Disconnect();
		m_isConnected = CRagConnection::instanceR().Connect(&g_charServerAddr);
		WinMainNpKeyStopEncryption();
		if (m_isConnected) {
			g_mustPumpOutReceiveQueue = 1;
			PACKET_CH_ENTER packet;
			packet.PacketType = HEADER_CH_ENTER;
			packet.clientType = 0;
			packet.AID = g_session.GetAid();
			packet.AuthCode = g_session.m_authCode;
			packet.userLevel = g_session.m_userLevel;
			packet.Sex = g_session.GetSex();
			CRagConnection::instanceR().SendPacket(
				CRagConnection::instanceR().GetPacketSize(packet.PacketType),
				(char *)&packet);
			UIWaitWnd *wnd = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
			wnd->SetMsg(MsgStr(MSI_WAITING_RESPONSE_FROM_SERVER), 16, 1);
		} else {
			CRagConnection::instanceR().Disconnect();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED), ME_OKCANCEL, TRUE, TRUE, 0) != ID_MODAL_PASSED) {
				g_mustPumpOutReceiveQueue = 0;
				SetNextSubState(LSM_IDENTRY);
			}
		}
		break;
	}
	case LSM_WAITRESP_FROM_CHSVR_MAKECHAR:
	{
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		PACKET_CH_MAKE_CHAR packet;
		packet.PacketType = HEADER_CH_MAKE_CHAR;
		sAssert(sizeof(g_charInfo.name) == NAME_LENGTH);
		memcpy((char *)packet.name, (char *)g_charInfo.name, sizeof(g_charInfo.name));
		packet.Str = g_charInfo.Str;
		packet.Agi = g_charInfo.Agi;
		packet.Vit = g_charInfo.Vit;
		packet.Int = g_charInfo.Int;
		packet.Dex = g_charInfo.Dex;
		packet.Luk = g_charInfo.Luk;
		packet.CharNum = g_charInfo.CharNum;
		packet.headPal = g_charInfo.headpalette;
		packet.head = g_charInfo.head;
		CRagConnection::instanceR().SendPacket(
			CRagConnection::instanceR().GetPacketSize(packet.PacketType),
			(char *)&packet);
		UIWaitWnd *wnd = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
		wnd->SetMsg(MsgStr(MSI_MAKING_CHARACTER), 17, 1);
		break;
	}
	case LSM_WAITRESP_FROM_CHSVR_DELETECHAR:
	{
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		CHARACTER_INFO *charInfo = (CHARACTER_INFO *)SendMsg(MM_QUERYCHARICTORINFO, g_charInfo.CharNum, 0, 0);
		sAssert(charInfo);
		PACKET_CH_DELETE_CHAR packet;
		packet.GID = charInfo->GID;
		packet.PacketType = HEADER_CH_DELETE_CHAR;
		strncpy(packet.key, m_emaiAddress, sizeof(packet.key));
		packet.key[sizeof(packet.key) - 1] = 0;
		if (g_serviceType == ServiceKorea) {
			if (m_emaiAddress[3] == 0)
				packet.key[0] = 0;
		}
		Trace("%s", m_emaiAddress);
		if (g_serviceType == ServiceGermany || g_serviceType == ServiceBrazil) {
			PACKET_CH_DELETE_CHAR2 packet2;
			packet2.GID = charInfo->GID;
			packet2.PacketType = HEADER_CH_DELETE_CHAR2;
			strncpy(packet2.key, m_emaiAddress, sizeof(packet2.key));
			packet2.key[sizeof(packet2.key) - 1] = 0;
			CRagConnection::instanceR().SendPacket(
				CRagConnection::instanceR().GetPacketSize(packet2.PacketType),
				(char *)&packet2);
		} else {
			CRagConnection::instanceR().SendPacket(
				CRagConnection::instanceR().GetPacketSize(packet.PacketType),
				(char *)&packet);
		}
		UIWaitWnd *wnd = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
		wnd->SetMsg(MsgStr(MSI_DELETING_CHARACTER), 17, 1);
		break;
	}
	case LSM_WAITRESP_FROM_CHSVR_SELECTCHAR:
	{
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		PACKET_CH_SELECT_CHAR packet;
		packet.PacketType = HEADER_CH_SELECT_CHAR;
		packet.CharNum = g_charInfo.CharNum;
		CRagConnection::instanceR().SendPacket(
			CRagConnection::instanceR().GetPacketSize(packet.PacketType),
			(char *)&packet);
		UIWaitWnd *wnd = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
		wnd->SetMsg(MsgStr(MSI_WAITING_RESPONSE_FROM_SERVER), 16, 1);
		break;
	}
	case LSM_READY_FOR_ZONESVR:
	{
		g_session.m_onDunInfoWnd = FALSE;
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		m_isConnected = CRagConnection::instanceR().Connect(&g_zoneServerAddr);
		if (m_isConnected) {
			Trace("PACKET_CZ_ENTER");
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
			CRagConnection::instanceR().SendPacket(
				CRagConnection::instanceR().GetPacketSize(p.PacketType),
				(char *)&p);
			CRagConnection::instanceR().SetBlock(true);
			UIWaitWnd *wnd = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
			wnd->SetMsg(MsgStr(MSI_WAITING_RESPONSE_FROM_SERVER), 16, 1);
		} else {
			CRagConnection::instanceR().Disconnect();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED), ME_OKCANCEL, TRUE, TRUE, 0) != ID_MODAL_PASSED) {
				g_mustPumpOutReceiveQueue = 0;
				SetNextSubState(LSM_IDENTRY);
			}
		}
		break;
	}
	case LSM_SELECTCHAR2:
		g_se_go = 0;
		g_FirstEffect = 0;
		g_isSkillEntry = 0;
		g_session.m_petJob = -1;
		g_session.SetEquipArrowIndwx(0);
		m_wallPaperBmpName = TITLE_FILE;
		g_windowMgr.SetWallpaper((CBitmapRes *)g_resMgr().Get(UIBmp(m_wallPaperBmpName.c_str()), false));
		g_windowMgr.MakeWindow(WID_SELCHARFORUSERVERWND);
		break;
	default:
		break;
	}
}


void CLoginMode::OnProcessState(int state)
{
	int waitAccountServerResponseTime;
	if (g_serviceType == ServiceThai)
		waitAccountServerResponseTime = 120 * 1000;
	else
		waitAccountServerResponseTime = 15 * 1000;

	switch (state) {
	case LSM_WAITRESP_FROM_ACSVR:
		if (timeGetTime() > m_subModeStartTime + waitAccountServerResponseTime)
			SendMsg(LMM_DISCONNECT_ACSVR, 0, 0, 0);
		if (g_serviceType == ServiceThai &&
			timeGetTime() > m_subModeStartTime + 15 * 1000)
		{
			m_wndWait->SendMsg(NULL, UM_ENABLE_CANCEL, 0, 0, 0, 0);
			m_wndWait->Invalidate();
		}
		break;
	case LSM_WAITRESP_FROM_CHSVR:
		if (timeGetTime() > m_subModeStartTime + 15 * 1000) {
			CRagConnection::instanceR().Disconnect();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED),
									  ME_OKCANCEL, TRUE, TRUE, 0) != ID_MODAL_PASSED)
			{
				g_mustPumpOutReceiveQueue = 0;
				SetNextSubState(LSM_IDENTRY);
			}
			break;
		}
	case LSM_READY_FOR_ZONESVR:
		if (timeGetTime() > m_subModeStartTime + 15 * 1000) {
			CRagConnection::instanceR().Disconnect();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED),
									  ME_OKCANCEL, TRUE, TRUE, 0) != ID_MODAL_PASSED)
			{
				g_mustPumpOutReceiveQueue = 0;
				SetNextSubState(LSM_IDENTRY);
			}
			break;
		}
	}
}



void CLoginMode::OnRenderState(int state)
{
}





void CLoginMode::OpenSettlePage()
{
	switch (g_serviceType) {
	case ServiceKorea:
		if (!g_isGravityID)
			ShellExecute(g_hMainWnd, "open", MsgStr(MSI_SETTLE_WEB_URL), NULL, NULL, SW_SHOWNORMAL);
		else
			ShellExecute(g_hMainWnd, "open", MsgStr(MSI_SETTLE_WEB_URL_HANGAME), NULL, NULL, SW_SHOWNORMAL);
		break;
	case ServiceAmerica:
		ShellExecute(g_hMainWnd, "open", "https://iro.ragnarokonline.com/member/memberjoin1.asp?mNum=1", NULL, NULL, SW_SHOWNORMAL);
		break;
	case ServiceJapan:
		break;
	case ServiceChina:
		ShellExecute(g_hMainWnd, "open", "http://ro.sdo.com/business/index.asp?a=3", NULL, NULL, SW_SHOWNORMAL);
		break;
	default:
		ShellExecute(g_hMainWnd, "open", MsgStr(MSI_SETTLE_WEB_URL), NULL, NULL, SW_SHOWNORMAL);
		break;
	}
}


void CLoginMode::PollNetworkStatus()
{
	if (!CRagConnection::instanceR().Poll())
		g_modeMgr.GetCurMode()->SendMsg(MM_SOCKETERROR, 0, 0, 0);

	if (g_mustPumpOutReceiveQueue == 1) {
		DWORD aid;
		if (CRagConnection::instanceR().Recv((char *)&aid, 4, true)) {
			g_mustPumpOutReceiveQueue = 0;
			sAssert(aid == g_session.GetAid());
		}
		return;
	}

	char buf[2048];
	while (CRagConnection::instanceR().RecvPacket(buf)) {
		switch (CRagConnection::instanceR().GetPacketType(buf)) {
		case HEADER_AC_ACCEPT_LOGIN:
			Ac_Accept_Login(buf);
			break;
		case HEADER_AC_REFUSE_LOGIN:
			Ac_Refuse_Login(buf);
			break;
		case HEADER_AC_EVENT_RESULT:
			Ac_Event_Result(buf);
			break;
		case HEADER_HC_ACCEPT_ENTER:
			Hc_Accept_Enter(buf);
			break;
		case HEADER_HC_BLOCK_CHARACTER:
			Hc_Block_Character(buf);
			break;
		case HEADER_HC_REFUSE_ENTER:
			Hc_Refuse_Enter(buf);
			break;
		case HEADER_HC_ACCEPT_MAKECHAR:
			Hc_Accept_Makechar(buf);
			break;
		case HEADER_HC_REFUSE_MAKECHAR:
			Hc_Refuse_Makechar(buf);
			break;
		case HEADER_HC_ACCEPT_DELETECHAR:
			Hc_Accept_Deletechar(buf);
			break;
		case HEADER_HC_REFUSE_DELETECHAR:
			Hc_Refuse_Deletechar(buf);
			break;
		case HEADER_HC_NOTIFY_ZONESVR:
			Hc_Notify_Zonesvr(buf);
			break;
		case HEADER_ZC_ACCEPT_ENTER:
			Zc_Accept_Enter(buf);
			return;
		case HEADER_ZC_ACCEPT_ENTER2:
			Zc_Accept_Enter2(buf);
			return;
		case HEADER_ZC_REFUSE_ENTER:
			Zc_Refuse_Enter(buf);
			break;
		case HEADER_SC_NOTIFY_BAN:
			Sc_Notify_Ban(buf);
			break;
		case HEADER_PING:
			break;
		case HEADER_SC_BILLING_INFO:
			Sc_Billing_Info(buf);
			break;
		case HEADER_AC_ASK_PNGAMEROOM:
			Ac_Ask_Pngameroom(buf);
			break;
		case HEADER_SC_ACK_ENCRYPTION:
			Sc_Ack_Encryption(buf);
			break;
		case HEADER_AC_ACK_HASH:
			Ac_Ack_Hash(buf);
			break;
		case HEADER_AC_NOTIFY_ERROR:
			Ac_Notify_Error(buf);
			break;
		case HEADER_ZC_AID:
			Zc_Aid(buf);
			break;
		case HEADER_HC_CHARNOTBEENSELECTED:
			Hc_CharNotBeenSelected(buf);
			break;
		case HEADER_HC_ACK_IS_VALID_CHARNAME:
			Hc_Ack_Is_Valid_CharName(buf);
			break;
		case HEADER_HC_ACK_CHANGE_CHARNAME:
			Hc_Ack_Change_CharName(buf);
			break;
		case HEADER_HC_REFUSE_SELECTCHAR:
			Hc_Refuse_SelectChar(buf);
			break;
		default:
			CRagConnection::instanceR().ResetQueue();
			break;
		}
	}
}


int CLoginMode::SendMsg(int messageId, int val1, int val2, int val3)
{
	switch (messageId) {
	case LMM_GET_EMAIL_ADDRESS:
		return (int)m_emaiAddress;
	case LMM_SET_EMAIL_ADDRESS:
		g_windowMgr.InvalidateAll();
		strcpy(m_emaiAddress, (char *)val1);
		break;
	case LMM_GET_MAKING_CHAR_NAME:
		return (int)m_makingCharName;
	case LMM_SET_MAKING_CHAR_NAME:
		strcpy(m_makingCharName, (char *)val1);
		break;
	case LMM_GET_CHAR_PARAM:
		return (int)m_charParam;
	case LMM_SET_CHAR_PARAM:
	{
		int *param = (int *)val1;
		for (int i = 0; i < 8; ++i)
			m_charParam[i] = param[i];
		break;
	}
	case LMM_PASSWORD:
		strcpy(m_userPassword, (char *)val1);
		break;
	case LMM_ID:
		strcpy(m_userId, (char *)val1);
		strcpy(g_session.m_aName, (char *)val1);
		break;
	case LMM_SELECT_CHARACTER:
	{
		g_session.m_charSlot = g_charInfo.CharNum;
		g_windowMgr.InitShortcutItemInfoFromReg();
		g_windowMgr.InitSkillUseLevelInfoFromReg();
		for (int i = 0; i < m_numChar; ++i) {
			if (g_charInfo.CharNum == m_charInfo[i].CharNum) {
				g_session.m_charNum = m_charInfo[i].CharNum;
				g_session.SetJob(m_charInfo[i].job);
				g_session.SetGid(m_charInfo[i].GID);
				g_session.SetSpeed(m_charInfo[i].speed);
				g_session.SetLevel(m_charInfo[i].level);
				g_session.SetSkillPoint(m_charInfo[i].sppoint);
				g_session.SetPoint(m_charInfo[i].jobpoint);
				g_session.SetStr(m_charInfo[i].Str);
				g_session.SetAgi(m_charInfo[i].Agi);
				g_session.SetVit(m_charInfo[i].Vit);
				g_session.SetInt(m_charInfo[i].Int);
				g_session.SetDex(m_charInfo[i].Dex);
				g_session.SetLuk(m_charInfo[i].Luk);
				g_session.SetExp(m_charInfo[i].exp);
				g_session.SetHp(m_charInfo[i].hp);
				g_session.SetMaxhp(m_charInfo[i].maxhp);
				g_session.SetSp(m_charInfo[i].sp);
				g_session.SetMaxsp(m_charInfo[i].maxsp);
				g_session.SetGold(m_charInfo[i].money);
				g_session.SetJobexp(m_charInfo[i].jobexp);
				g_session.SetVirtue(m_charInfo[i].virtue);
				g_session.SetHonor(m_charInfo[i].honor);
				g_session.SetJoblevel(m_charInfo[i].joblevel);
				g_session.m_haircolor = m_charInfo[i].haircolor;
				g_session.m_head = m_charInfo[i].head;
				g_session.m_weapon = m_charInfo[i].weapon;
				g_session.m_accessory = m_charInfo[i].accessory;
				g_session.m_shield = m_charInfo[i].shield;
				g_session.m_bodyPalette = m_charInfo[i].bodypalette;
				g_session.m_headPalette = m_charInfo[i].headpalette;
				g_session.m_accessory2 = m_charInfo[i].accessory2;
				g_session.m_accessory3 = m_charInfo[i].accessory3;
				g_session.m_bodyState = m_charInfo[i].bodystate;
				g_session.m_healthState = m_charInfo[i].healthstate;
				g_session.m_effectState = m_charInfo[i].effectstate;
				break;
			}
		}
		break;
	}
	case MM_GOTOIDENTRY:
		g_mustPumpOutReceiveQueue = 0;
		SetNextSubState(LSM_IDENTRY);
		break;
	case LMM_GOTOSELECTACCOUNT:
		if (val2 == 0x87)
			g_modeMgr.GetCurMode()->SendMsg(MM_QUIT, 0, 0, 0);
		else
			SetNextSubState(LSM_SELECTACCOUNT);
		break;
	case LMM_SELECTACCOUNT:
		g_session.m_SelectAccountNumber = m_nSelectedAccountNo = val1;
		SelectClientInfo(val1);
		if (g_serviceType == ServiceThai)
			SetNextSubState(LSM_SELECTACCOUNT2);
		else
			SetNextSubState(LSM_IDENTRY);
		break;
	case LMM_SELECTACCOUNT2:
		m_nSelectedAccountNo2 = val1;
		m_accountInfo2.clear();
		SelectClientInfo2(m_nSelectedAccountNo, m_nSelectedAccountNo2);
		SetNextSubState(LSM_IDENTRY);
		break;
	case MM_SOCKETERROR:
		CRagConnection::instanceR().Disconnect();
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_UNABLE_TO_CONNECT_SERVER), ME_OK, TRUE, TRUE, 0) != ID_MODAL_PASSED) {
			g_mustPumpOutReceiveQueue = 0;
			SetNextSubState(LSM_IDENTRY);
		}
		break;
	case LMM_GOTO_LOGIN:
		CRagConnection::instanceR().Disconnect();
		g_mustPumpOutReceiveQueue = 0;
		SetNextSubState(LSM_IDENTRY);
		break;
	case LMM_CONNECT_TO_ACSVR:
		SetNextSubState(LSM_WAITRESP_FROM_ACSVR);
		break;
	case LMM_DISCONNECT_ACSVR:
		CRagConnection::instanceR().Disconnect();
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_CONNECTION_FAILED), ME_OKCANCEL, TRUE, TRUE, 0) != ID_MODAL_PASSED) {
			g_mustPumpOutReceiveQueue = 0;
			SetNextSubState(LSM_IDENTRY);
		}
		break;
	case LMM_SELECTSVR:
	{
		m_serverSelected = val1;
		if (val1 != -1) {
			if (m_serverInfo[m_serverSelected].state != SERVERSTATE_PAY) {
				g_session.SetSelectedServerName((const char *)m_serverInfo[m_serverSelected].name);

				ServerAddress addr;
				BYTE ip[4];
				memcpy(ip, &m_serverInfo[m_serverSelected].ip, 4);
				wsprintf(addr.ip, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
				addr.port = m_serverInfo[m_serverSelected].port;
				memcpy(&g_charServerAddr, &addr, sizeof(addr));
				g_charServerAddr.dwIP = m_serverInfo[m_serverSelected].ip;

				g_session.m_lastServerName = (char *)m_serverInfo[m_serverSelected].name;
				g_session.InitExListWithRegistry();
				g_bOldServer = 0;
				SetNextSubState(LSM_WAITRESP_FROM_CHSVR);
			} else {
				g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_CONNECT_TO_PAY_SERVER), ME_OK, TRUE, FALSE, 0);
				SetNextSubState(LSM_SELECTSVR);
			}
		}
		break;
	}
	case LMM_SENDCHARINFO:
		SetNextSubState(LSM_WAITRESP_FROM_CHSVR_MAKECHAR);
		break;
	case LMM_CANCEL:
		if (m_subMode == LSM_SELECTSVR) {
			CRagConnection::instanceR().Disconnect();
			g_mustPumpOutReceiveQueue = 0;
			SetNextSubState(LSM_IDENTRY);
		}
		break;
	case LMM_GET_BILLING_INFO:
	{
		DWORD time;
		switch (m_billingInfo.code) {
		case 100:
			time = m_billingInfo.time1;
			sprintf((char *)val1, MsgStr(MSI_BILLING_100), time / 60);
			break;
		case 200:
			time = m_billingInfo.time1;
			sprintf((char *)val1, MsgStr(MSI_BILLING_200),
				time / 3600, (time / 60) % 60, time % 60);
			break;
		case 300:
			sprintf((char *)val1, MsgStr(MSI_BILLING_300));
			break;
		case 400:
			time = m_billingInfo.time2 - m_billingInfo.time1;
			sprintf((char *)val1, MsgStr(MSI_BILLING_400),
				time / 3600, (time / 60) % 60, time % 60);
			break;
		case 500:
			if (m_billingInfo.time1) {
				time = m_billingInfo.time1;
				sprintf((char *)val1, MsgStr(MSI_BILLING_500),
					time / 3600, (time / 60) % 60, time % 60);
			} else {
				time = m_billingInfo.time2;
				sprintf((char *)val1, MsgStr(MSI_BILLING_501),
					time / 3600, (time / 60) % 60, time % 60);
			}
			break;
		case 600:
			sprintf((char *)val1, MsgStr(MSI_BILLING_600));
			break;
		case 700:
			sprintf((char *)val1, MsgStr(MSI_BILLING_700));
			break;
		case 800:
			sprintf((char *)val1, MsgStr(MSI_BILLING_800));
			break;
		default:
			sprintf((char *)val1, MsgStr(MSI_CHARACTER_INFO));
			break;
		}
		break;
	}
	case MM_QUERYCHARICTORINFO:
	{
		for (int i = 0; i < m_numChar; ++i) {
			if (m_charInfo[i].CharNum == val1)
				return (int)&m_charInfo[i];
		}
		return 0;
	}
	case MM_COMMAND:
		switch (val1) {
		case LMM_SELECTCHARACTER:
			SetNextSubState(LSM_SELECTCHAR);
			break;
		case LMM_DELETE_CHARACTER:
			SetNextSubState(LSM_WAITRESP_FROM_CHSVR_DELETECHAR);
			break;
		case LMM_MAKECHARACTER:
			SetNextSubState(LSM_MAKECHAR);
			break;
		case LMM_TRANSFERCHARACTER:
			SetNextSubState(LSM_WAITRESP_FROM_CHSVR_SELECTCHAR);
			break;
		case LMM_CANCEL:
			if (m_subMode == LSM_SELECTCHAR) {
				g_windowMgr.RemoveAllWindows();
				CRagConnection::instanceR().Disconnect();
				SetNextSubState(LSM_SELECTSVR);
			}
			break;
		default:
			break;
		}
		break;
	default:
		return CMode::SendMsg(messageId, val1, val2, val3);
	}
	return 0;
}



void CLoginMode::Ac_Accept_Login(const char *buf)
{
	g_session.m_playingTime = timeGetTime();
	SendID(g_session.m_aName);
	g_session.m_strAutoMSG = MsgStr(MSI_AUTO_MSG);

	PACKET_AC_ACCEPT_LOGIN *p = (PACKET_AC_ACCEPT_LOGIN *)buf;

	g_session.m_authCode = p->AuthCode;
	g_session.m_userLevel = p->userLevel;
	g_session.SetAid(p->AID);

	if (p->Sex >= 10) {
		g_session.SetSex(p->Sex - 10);
	} else {
		if (g_multiSTOP == 1)
			exit(0);
		g_session.SetSex(p->Sex);
	}
	g_session.SetTextType(g_session.m_isShowNameTypeOld, g_session.m_isShowNameTypeOld);

	g_session.m_lastLoginIP = p->lastLoginIP;
	memcpy(g_session.m_lastLoginTime, p->lastLoginTime, 26);
	g_session.m_lastLoginTime[26] = 0;

	m_numServer = (p->PacketLength - sizeof(PACKET_AC_ACCEPT_LOGIN)) / sizeof(SERVER_ADDR);
	memcpy(m_serverInfo, ((char *)p) + sizeof(PACKET_AC_ACCEPT_LOGIN), sizeof(SERVER_ADDR) * m_numServer);

	CRagConnection::instanceR().Disconnect();
	g_passwordWrong = 0;
	SetNextSubState(LSM_SELECTSVR);
}


void CLoginMode::Ac_Refuse_Login(const char *buf)
{
	char refuseString[256];

	if (*(BYTE *)(buf + 2) != REFUSE_WARNING)
		CRagConnection::instanceR().Disconnect();

	switch (*(BYTE *)(buf + 2)) {
	case REFUSE_INVALID_ID:
		g_windowMgr.ErrorMsg(MsgStr(MSI_INCORRECT_USERID), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_INVALID_PASSWD:
		if (g_serviceType == ServiceKorea) {
			++g_passwordWrong;
			if (g_passwordWrong >= 3) {
				if (g_windowMgr.ErrorMsg("Wrong password entered 3 times. Please check your password.              The client will now close.",
						ME_OK, TRUE, TRUE, 0) == ID_OK) {
					if (!g_isGravityID) {
						ShellExecute(g_hMainWnd, "open", "http://ragnarok.co.kr", NULL, NULL, SW_SHOWNORMAL);
					} else {
						ShellExecute(g_hMainWnd, "open", "http://www.hangame.com", NULL, NULL, SW_SHOWNORMAL);
					}
					g_modeMgr.Quit();
				}
			} else {
				g_windowMgr.ErrorMsg(MsgStr(MSI_INCORRECT_LOGIN_PASSWORD), ME_OK, TRUE, FALSE, 0);
			}
		} else {
			g_windowMgr.ErrorMsg(MsgStr(MSI_INCORRECT_LOGIN_PASSWORD), ME_OK, TRUE, FALSE, 0);
		}
		break;
	case REFUSE_ID_EXPIRED:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ID_EXPIRED), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_NOT_CONFIRMED:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ID_EMAIL_CONFIRM_NEEDED), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_INVALID_VERSION:
		g_windowMgr.ErrorMsg(MsgStr(MSI_INVALID_VERSION), ME_OK, TRUE, FALSE, 0);
		SendMsg(MM_QUIT, 0, 0, 0);
		break;
	case REFUSE_DELETED_ACCOUNT:
		g_windowMgr.ErrorMsg(MsgStr(MSI_DELETED_ACCOUNT), ME_OK, TRUE, FALSE, 0);
		SendMsg(MM_QUIT, 0, 0, 0);
		break;
	case REFUSE_BLOCK_TEMPORARY:
		((char *)buf)[0x16] = 0;
		sprintf(refuseString, MsgStr(MSI_LOGIN_REFUSE_BLOCKED_UNTIL), buf + 3);
		g_windowMgr.ErrorMsg(refuseString, ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_TEMP_BAN_HACKING_INVESTIGATION:
		((char *)buf)[0x16] = 0;
		sprintf(refuseString, MsgStr(MSI_HACKING_INVESTIGATION), buf + 3);
		g_windowMgr.ErrorMsg(refuseString, ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_TEMP_BAN_BUG_INVESTIGATION:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BUG_INVESTIGATION), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_TEMP_BAN_DELETING_CHAR:
		g_windowMgr.ErrorMsg(MsgStr(MSI_DELETING_CHAR), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_TEMP_BAN_DELETING_SPOUSE_CHAR:
		g_windowMgr.ErrorMsg(MsgStr(MSI_DELETING_SPOUSE_CHAR), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_BILLING_NOT_READY:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ACCOUNT_BUSY), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_NONSAKRAY_ID_BLOCKED:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_NONSAKRAY_ID_BLOCKED), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_BAN_BY_DBA:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_BAN_BY_DBA), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_EMAIL_NOT_CONFIRMED:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_EMAIL_NOT_CONFIRMED), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_BAN_BY_GM:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_BAN_BY_GM), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_TEMP_BAN_FOR_DBWORK:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_TEMP_BAN_FOR_DBWORK), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_SELF_LOCK:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_SELF_LOCK), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_NOT_PERMITTED_GROUP:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_NOT_PERMITTED_GROUP), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_WAIT_FOR_SAKRAY_ACTIVE:
		g_windowMgr.ErrorMsg(MsgStr(MSI_REFUSE_WAIT_FOR_SAKRAY_ACTIVE), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_ALREADY_CONNECT:
		((char *)buf)[0x16] = 0;
		sprintf(refuseString, MsgStr(MSI_ALREADY_CONNECT), buf + 3);
		g_windowMgr.ErrorMsg(refuseString, ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_NOT_CHANGED_PASSWD:
		if (g_serviceType == ServiceKorea) {
			if (g_windowMgr.ErrorMsg("Please log in again after changing your password.",
					ME_OK, TRUE, TRUE, 0) == ID_OK) {
				ShellExecute(g_hMainWnd, "open",
					"http://www.ragnarok.co.kr/setNewPW/password_change_agreement.asp",
					NULL, NULL, SW_SHOWNORMAL);
				g_modeMgr.Quit();
			}
		}
		break;
	case ACCEPT_LOGIN_USER_PHONE_BLOCK:
		g_windowMgr.ErrorMsg(MsgStr(MSI_PHONE_CONFIRM), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_USER_PHONE_BLOCK:
		g_windowMgr.ErrorMsg(MsgStr(MSI_PHONE_BLOCK), ME_OK, TRUE, FALSE, 0);
		break;
	case REFUSE_BLOCK_INVALID:
		((char *)buf)[0x16] = 0;
		sprintf(refuseString, MsgStr(MSI_RE17), buf + 3);
		if (g_windowMgr.ErrorMsg(refuseString, ME_YESNO, TRUE, TRUE, 0) == ID_YES)
			g_modeMgr.Quit();
		break;
	case REFUSE_WARNING:
		g_windowMgr.ErrorMsg(MsgStr(MSI_RE18), ME_OK, TRUE, FALSE, 0);
		return;
	default:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ACCESS_DENIED), ME_OK, TRUE, FALSE, 0);
		break;
	}

	g_mustPumpOutReceiveQueue = 0;
	SetNextSubState(LSM_IDENTRY);
}



void CLoginMode::Ac_Event_Result(const char *buf)
{
	PACKET_AC_EVENT_RESULT *p = (PACKET_AC_EVENT_RESULT *)buf;
	switch (p->EventItemCount) {
	default:
		g_windowMgr.ErrorMsg(MsgStr(MSI_EVENT_RESULT), ME_OK, 1, 0, 0);
		break;
	}
}



struct PACKET_HC_ACCEPT_ENTER {
	short PacketType;
	WORD PacketLength;
	char m_extension[20];
};







void CLoginMode::Hc_Accept_Enter(const char *buf)
{
	PACKET_HC_ACCEPT_ENTER *p = (PACKET_HC_ACCEPT_ENTER *)buf;
	memcpy(&m_billingInfo, p->m_extension, sizeof(m_billingInfo));
	m_billingInfo.code = ntohl(m_billingInfo.code);
	m_billingInfo.time1 = ntohl(m_billingInfo.time1);
	m_billingInfo.time2 = ntohl(m_billingInfo.time2);

	m_numChar = (p->PacketLength - sizeof(PACKET_HC_ACCEPT_ENTER)) / sizeof(CHARACTER_INFO);

	sAssert(m_numChar >= 0 && m_numChar <= 12);
	int billingtempsize = (p->PacketLength - sizeof(PACKET_HC_ACCEPT_ENTER)) % sizeof(CHARACTER_INFO);
	memcpy(&m_charInfo[0], ((char *)p) + sizeof(PACKET_HC_ACCEPT_ENTER) + billingtempsize,
		sizeof(CHARACTER_INFO) * m_numChar);
	SetNextSubState(LSM_SELECTCHAR);
}



struct PACKET_HC_BLOCK_CHARACTER {
	short PacketType;
	WORD PacketLength;
};

void CLoginMode::Hc_Block_Character(const char *buf)
{
	PACKET_HC_BLOCK_CHARACTER *p = (PACKET_HC_BLOCK_CHARACTER *)buf;

	int num = (p->PacketLength - sizeof(PACKET_HC_BLOCK_CHARACTER)) / sizeof(TAG_CHARACTER_BLOCK_INFO);
	memset(&g_session.m_charBlockInfo2[0], 0,
		sizeof(TAG_CHARACTER_BLOCK_INFO) * 9);
	memcpy(&g_session.m_charBlockInfo2[0], ((char *)p) + sizeof(PACKET_HC_BLOCK_CHARACTER), sizeof(TAG_CHARACTER_BLOCK_INFO) * num);
}






void CLoginMode::Hc_Refuse_Enter(const char *buf)
{
	PACKET_HC_REFUSE_ENTER *p = (PACKET_HC_REFUSE_ENTER *)buf;
	CRagConnection::instanceR().Disconnect();
	switch (p->ErrorCode) {
	case 0:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ACCESS_DENIED), ME_OK, 1, 0, 0);
		break;
	case 1:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ID_MISMATCH), ME_OK, 1, 1, 0);
		break;
	default:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ACCESS_DENIED), ME_OK, 1, 0, 0);
		break;
	}
	g_mustPumpOutReceiveQueue = 0;
	SetNextSubState(LSM_IDENTRY);
}





void CLoginMode::Hc_Accept_Makechar(const char *buf)
{
	PACKET_HC_ACCEPT_MAKECHAR *p = (PACKET_HC_ACCEPT_MAKECHAR *)buf;
	sAssert(p->charinfo.head < HEADTYPE_LAST);
	memcpy(&g_charInfo, &(p->charinfo), sizeof(CHARACTER_INFO));
	memcpy(&(m_charInfo[m_numChar]), &(p->charinfo), sizeof(CHARACTER_INFO));
	m_numChar++;
	SetNextSubState(LSM_SELECTCHAR);
}





void CLoginMode::Hc_Refuse_Makechar(const char *buf)
{
	switch ((unsigned char)buf[2]) {
	case 0:
		g_windowMgr.ErrorMsg(MsgStr(MSI_CHARACTER_NAME_ALREADY_EXISTS), ME_OK, 1, 0, 0);
		break;
	case 1:
		g_windowMgr.ErrorMsg(MsgStr(MSI_LIMIT_AGE), ME_OK, 1, 0, 0);
		break;
	case 2:
		g_windowMgr.ErrorMsg(MsgStr(MSI_LIMIT_CHAR_DELETE), ME_OK, 1, 0, 0);
		break;
	case 11:
		g_windowMgr.ErrorMsg(MsgStr(MSI_NEED_PREMIUM_SERVICE), ME_OK, 1, 0, 0);
		break;
	case 3:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_ERR_MKCHAR_INVALID_SLOT), ME_OK, 1, 0, 0);
		break;
	default:
		g_windowMgr.ErrorMsg(MsgStr(MSI_CHARACTER_CREATION_DENIED), ME_OK, 1, 0, 0);
		break;
	}
	SetNextSubState(LSM_SELECTCHAR);
}





void CLoginMode::Hc_Accept_Deletechar(const char * buf)
{
	CHARACTER_INFO tmpInfo[9];
	int j = 0;
	for (int i = 0; i < m_numChar; i++) {
		if (m_charInfo[i].CharNum != g_charInfo.CharNum) {
			memcpy(&tmpInfo[j], &m_charInfo[i], sizeof(CHARACTER_INFO));
			j++;
		}
	}
	m_numChar--;
	memcpy(&m_charInfo[0], &tmpInfo[0], m_numChar * sizeof(CHARACTER_INFO));
	SetNextSubState(LSM_SELECTCHAR);
}




void CLoginMode::Hc_Refuse_Deletechar(const char *buf)
{
	PACKET_HC_REFUSE_DELETECHAR *p = (PACKET_HC_REFUSE_DELETECHAR *)buf;
	switch (p->ErrorCode) {
	case 0:
		if (g_serviceType == ServiceKorea)
			g_windowMgr.ErrorMsg(MsgStr(MSI_CANNOT_DELETE_CHARACTER_PEOPLE_REG_NUMBER), ME_OK, 1, 0, 0);
		else
			g_windowMgr.ErrorMsg(MsgStr(MSI_CANNOT_DELETE_CHARACTER_EMAIL), ME_OK, 1, 0, 0);
		break;
	case 1:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_ERR_DELCHAR_INVALID_SLOT), ME_OK, 1, 0, 0);
		break;
	default:
		g_windowMgr.ErrorMsg(MsgStr(MSI_CANNOT_DELETE_CHARACTER), ME_OK, 1, 0, 0);
		break;
	}
	SetNextSubState(LSM_SELECTCHAR);
}


void CLoginMode::Hc_Notify_Zonesvr(const char *buf)
{
	PACKET_HC_NOTIFY_ZONESVR *p = (PACKET_HC_NOTIFY_ZONESVR *)buf;
	char mapName[17];

	g_session.SetGid(p->GID);
	memcpy(mapName, (char *)p->mapName, 16);
	mapName[16] = 0;

	std::string curMap = mapName;
	curMap.resize(curMap.size() - 4);
	strcpy(g_session.m_curMap, curMap.c_str());

	in_addr addr;
	addr.S_un.S_addr = p->addr.ip;
	strcpy(g_zoneServerAddr.ip, inet_ntoa(addr));
	g_zoneServerAddr.port = p->addr.port;

	CRagConnection::instanceR().Disconnect();

	SetNextSubState(LSM_READY_FOR_ZONESVR);
}


void CLoginMode::Zc_Accept_Enter(const char *buf)
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

	UIWaitWnd *waitWindow = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
	waitWindow->SetMsg(MsgStr(MSI_PLEASE_BE_PATIENT), 17, 1);

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


void CLoginMode::Zc_Accept_Enter2(const char *buf)
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

	UIWaitWnd *waitWindow = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
	waitWindow->SetMsg(MsgStr(MSI_PLEASE_BE_PATIENT), 17, 1);

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




void CLoginMode::Zc_Refuse_Enter(const char *buf)
{
	PACKET_ZC_REFUSE_ENTER *p = (PACKET_ZC_REFUSE_ENTER *)buf;
	p->ErrorCode;
	g_windowMgr.ErrorMsg(MsgStr(MSI_ACCESS_DENIED), ME_OK, 1, 0, 0);
	CRagConnection::instanceR().Disconnect();
	g_mustPumpOutReceiveQueue = 0;
	SetNextSubState(LSM_IDENTRY);
}


void CLoginMode::Sc_Notify_Ban(const char *buf)
{
	Trace("Sc_Notify_Ban - login");
	int errorCode = *(BYTE *)(buf + 2);

	CRagConnection::instanceR().Disconnect();
	switch (errorCode) {
	case BAN_UNFAIR:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BANNED), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_SERVER_SHUTDOWN:
		g_windowMgr.ErrorMsg(MsgStr(MSI_SERVER_OFF), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_ANOTHER_LOGIN:
		Trace("BAN_ANOTHER_LOGIN - login");
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
	case BAN_NOT_SETTLED:
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_NOT_SETTLED), ME_YESNO, TRUE, TRUE, 0) == ID_YES) {
			g_modeMgr.Quit();
			OpenSettlePage();
		}
		break;
	case BAN_ACCOUNT_BUSY:
		g_windowMgr.ErrorMsg(MsgStr(MSI_ACCOUNT_BUSY), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_INFORMATION_REMAINED:
		g_windowMgr.ErrorMsg(MsgStr(MSI_INFORMATION_REMAINED), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_IP_OVERFLOW:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_IP_OVERFLOW), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PAY_OUT:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PAY_OUT), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PAY_SUSPEND:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PAY_SUSPEND), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PAY_CHANGE:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PAY_CHANGE), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PAY_WRONGIP:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PAY_WRONGIP), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PAY_PNGAMEROOM:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PAY_PNGAMEROOM), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_JAPAN_REFUSE1:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_JAPAN_REFUSE1), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_JAPAN_REFUSE2:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_JAPAN_REFUSE2), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_INFORMATION_REMAINED_ANOTHER_ACCOUNT:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_INFORMATION_REMAINED_ANOTHER_ACCOUNT), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PC_IP_UNFAIR:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PC_IP_UNFAIR), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PC_IP_COUNT_ALL:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PC_IP_COUNT_ALL), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_PC_IP_COUNT:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_PC_IP_COUNT), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_GRAVITY_MEM_AGREE:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_GRAVITY_MEM_AGREE), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_GAME_MEM_AGREE:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_GAME_MEM_AGREE), ME_OK, TRUE, TRUE, 0);
		break;
	case BAN_HAN_VALID:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BAN_HAN_VALID), ME_OK, TRUE, TRUE, 0);
		break;
	default:
		g_windowMgr.ErrorMsg(MsgStr(MSI_BANNED), ME_OK, TRUE, TRUE, 0);
		break;
	}

	g_mustPumpOutReceiveQueue = 0;
	SetNextSubState(LSM_IDENTRY);
}


void CLoginMode::Sc_Billing_Info(const char *buf)
{
	PACKET_SC_BILLING_INFO *p = (PACKET_SC_BILLING_INFO *)buf;
	char buffer[256];

	if (p->dwAmountRemain == -2 && p->dwQuantityRemain == -2) {
		sprintf(buffer, "%s\n", MsgStr(MSI_BILLING_FREE_USER));
	} else if (p->dwAmountRemain == -3 && p->dwQuantityRemain == -3) {
		sprintf(buffer, "%s\n", MsgStr(MSI_BILLING_PCB));
	} else {
		if (p->dwAmountRemain == -1 && p->dwQuantityRemain == -1) {
			sprintf(buffer, "%s\n", MsgStr(MSI_BILLING_INTERNET_CAFE));
		} else if (p->dwAmountRemain > 0 && p->dwQuantityRemain > 0) {
			sprintf(buffer, MsgStr(MSI_BILLING_BOTH),
				p->dwAmountRemain / (60 * 24), (p->dwAmountRemain / 60) % 24, p->dwAmountRemain % 60,
				p->dwQuantityRemain / 60, p->dwQuantityRemain % 60);
		} else if (p->dwAmountRemain > 0) {
			sprintf(buffer, "%s\n", MsgStr(MSI_BILLING_100));
			sprintf(buffer + strlen(buffer), MsgStr(MSI_BILLING_DAY_HOUR_MINUTES),
				p->dwAmountRemain / (60 * 24), (p->dwAmountRemain / 60) % 24, p->dwAmountRemain % 60);
		} else if (p->dwQuantityRemain > 0) {
			sprintf(buffer, "%s\n", MsgStr(MSI_BILLING_800));
			sprintf(buffer + strlen(buffer), MsgStr(MSI_BILLING_HOUR_MINUTES),
				p->dwQuantityRemain / 60, p->dwQuantityRemain % 60);
		} else {
			sprintf(buffer, "\n");
		}
	}

	m_subModeStartTime = timeGetTime() + 1000 * 90;

	g_windowMgr.ErrorMsg(buffer, ME_OK, TRUE, TRUE, 0);
}






void CLoginMode::Ac_Ask_Pngameroom(const char * buf)
{
	if (g_windowMgr.ErrorMsg(MsgStr(MSI_ASK_PNGAMEROOM), ME_YESNO, 1, 0, 0) == ID_YES) {
		PACKET_CA_REPLY_PNGAMEROOM p;
		p.PacketType = HEADER_CA_REPLY_PNGAMEROOM;
		p.Permission = 1;
		CRagConnection::instanceR().SendPacket(
			CRagConnection::instanceR().GetPacketSize(p.PacketType),
			(const char *)&p);
	} else {
		CRagConnection::instanceR().Disconnect();
		g_mustPumpOutReceiveQueue = 0;
		SetNextSubState(LSM_IDENTRY);
	}
}

void CLoginMode::Sc_Ack_Encryption(const char *buf)
{
	PACKET_SC_ACK_ENCRYPTION *p = (PACKET_SC_ACK_ENCRYPTION *)buf;

	ErrorMsg("No Packet Encryption !!!");
}


void CLoginMode::Ac_Ack_Hash(const char *buf)
{
	int msgLen = *(short *)(buf + 2) - 4;

	MD5 hash;
	hash.init();
	if (g_passwordEncrypt2) {
		hash.append((md5_byte_t *)m_userPassword, strlen(m_userPassword));
		hash.append((md5_byte_t *)(buf + 4), msgLen);
	} else {
		hash.append((md5_byte_t *)(buf + 4), msgLen);
		hash.append((md5_byte_t *)m_userPassword, strlen(m_userPassword));
	}

	if (ServiceThai == g_serviceType) {
		PACKET_CA_LOGIN3 p;
		p.PacketType = HEADER_CA_LOGIN3;
		p.Version = g_version;
		memcpy((char *)p.ID, m_userId, sizeof(p.ID));
		hash.finish((md5_byte_t *)p.PasswdMD5);
		p.clienttype = GetAccountType();

		if (g_session.m_isCheckToPcbang)
			p.ClientInfo = g_session.m_SelectAccountNumber + 10;
		else
			p.ClientInfo = g_session.m_SelectAccountNumber;
		CRagConnection::instanceR().SendPacket(
			CRagConnection::instanceR().GetPacketSize(p.PacketType),
			(char *)&p);
	} else if (ServiceJapan == g_serviceType) {
		PACKET_CA_LOGIN4 p;
		p.PacketType = HEADER_CA_LOGIN4;
		p.Version = g_version;
		memcpy((char *)p.ID, m_userId, sizeof(p.ID));
		hash.finish((md5_byte_t *)p.PasswdMD5);
		p.clienttype = GetAccountType();

		CRagConnection::instanceR().SendPacket(
			CRagConnection::instanceR().GetPacketSize(p.PacketType),
			(char *)&p);
	} else {
		PACKET_CA_LOGIN2 p;
		p.PacketType = HEADER_CA_LOGIN2;
		p.Version = g_version;
		memcpy((char *)p.ID, m_userId, sizeof(p.ID));
		hash.finish((md5_byte_t *)p.PasswdMD5);
		p.clienttype = GetAccountType();
		CRagConnection::instanceR().SendPacket(
			CRagConnection::instanceR().GetPacketSize(p.PacketType),
			(char *)&p);
	}

	UIWaitWnd *waitWindow = (UIWaitWnd *)g_windowMgr.MakeWindow(WID_WAITWND);
	waitWindow->SetMsg(MsgStr(MSI_WAITING_RESPONSE_FROM_SERVER), 16, 1);
}


void CLoginMode::Ac_Notify_Error(const char *buf)
{
	std::string error;
	error.assign(buf + 4, buf + *(WORD *)(buf + 2));

	g_windowMgr.ErrorMsg(error.c_str(), ME_OK, TRUE, TRUE, 0);

	g_mustPumpOutReceiveQueue = 0;
	SetNextSubState(LSM_IDENTRY);
}


bool CLoginMode::InitAccountInfo()
{
	XMLElement *clientinfo;
	XMLElement *connection;

	if (m_accountInfo.empty()) {
		if (!(clientinfo = GetClientInfo())) return false;

		if (!(connection = clientinfo->FindChild("connection"))) return false;

		while (connection) {
			accountInfo info;
			XMLElement *element;

			if ((element = connection->FindChild("display"))) info.display = element->GetContents();
			if ((element = connection->FindChild("desc"))) info.desc = element->GetContents();
			if ((element = connection->FindChild("balloon"))) info.balloon = element->GetContents();

			m_accountInfo.push_back(info);

			connection = connection->FindNext("connection");
		}
	}

	return !g_hideAccountList;
}


bool CLoginMode::InitAccountInfo2(int accNumber)
{
	XMLElement *clientinfo;
	XMLElement *connection;
	XMLElement *tempconnection;
	XMLElement *subconnection;

	if (m_accountInfo2.empty()) {
		if (!(clientinfo = GetClientInfo())) return false;

		if (connection = clientinfo->FindChild("connection")) {
			while (accNumber && connection) {
				--accNumber;
				connection = connection->FindNext("connection");
			}
			tempconnection = connection;

			if (connection) {
				if (subconnection = connection->FindChild("subconnection")) {
					while (subconnection) {
						accountInfo info;
						XMLElement *element;

						if ((element = subconnection->FindChild("display"))) info.display = element->GetContents();
						if ((element = subconnection->FindChild("desc"))) info.desc = element->GetContents();
						if ((element = subconnection->FindChild("balloon"))) info.balloon = element->GetContents();

						m_accountInfo2.push_back(info);

						subconnection = subconnection->FindNext("subconnection");
					}
				} else {
					if (tempconnection) {
						accountInfo info;
						XMLElement *element;

						if ((element = tempconnection->FindChild("display"))) info.display = element->GetContents();
						if ((element = tempconnection->FindChild("desc"))) info.desc = element->GetContents();
						if ((element = tempconnection->FindChild("balloon"))) info.balloon = element->GetContents();

						m_accountInfo2.push_back(info);
					}
				}
			}
		}
	}
	return !g_hideAccountList;
}


int GetAccountType()
{
	if (g_serverType != ServerSakray && g_serverType != ServerPK) {
		switch (g_serviceType) {
		case ServiceAmerica:
			return CLIENTTYPE_ENGLISH;
		case ServiceJapan:
			return CLIENTTYPE_JAPAN;
		case ServiceChina:
			return CLIENTTYPE_CHINA;
		case ServiceTaiwan:
			return CLIENTTYPE_TAIWAN;
		case ServiceThai:
			return CLIENTTYPE_THAI;
		case ServiceIndonesia:
			return CLIENTTYPE_INDONESIA;
		case ServicePhilippine:
			return CLIENTTYPE_PHILIPPINE;
		case ServiceMalaysia:
			return CLIENTTYPE_MALAYSIA;
		case ServiceSingapore:
			return CLIENTTYPE_SINGAPORE;
		case ServiceGermany:
			return CLIENTTYPE_GERMANY;
		case ServiceIndia:
			return CLIENTTYPE_INDIA;
		case ServiceBrazil:
			return CLIENTTYPE_BRAZIL;
		case ServiceAustralia:
			return CLIENTTYPE_AUSTRALIA;
		case ServiceVietnam:
			return CLIENTTYPE_VIETNAM;
		case ServiceRussia:
			return CLIENTTYPE_RUSSIA;
		case ServiceChile:
			return CLIENTTYPE_CHILE;
		case ServiceFrance:
			return CLIENTTYPE_FRANCE;
		default:
			return CLIENTTYPE_KOREAN;
		}
	} else if (g_serverType == ServerPK) {
		switch (g_serviceType) {
		case ServicePhilippine:
			return CLIENTTYPE_PHILIPPINE_PK;
		case ServiceJapan:
			return CLIENTTYPE_JAPAN_PK;
		case ServiceThai:
			return CLIENTTYPE_THAI_PK;
		case ServiceVietnam:
			return CLIENTTYPE_VIETNAM_PK;
		case ServiceIndonesia:
			return CLIENTTYPE_INDONESIA_PK;
		default:
			return CLIENTTYPE_KOREAN;
		}
	} else {
		switch (g_serviceType) {
		case ServiceAmerica:
			return CLIENTTYPE_ENGLISH_SAKRAY;
		case ServiceJapan:
			return CLIENTTYPE_JAPAN_SAKRAY;
		case ServiceChina:
			return CLIENTTYPE_CHINA;
		case ServiceTaiwan:
			return CLIENTTYPE_TAIWAN;
		case ServiceThai:
			return CLIENTTYPE_THAI_SAKRAY;
		case ServiceIndonesia:
			return CLIENTTYPE_INDONESIA_SAKRAY;
		case ServicePhilippine:
			return CLIENTTYPE_PHILIPPINE_SAKRAY;
		case ServiceMalaysia:
			return CLIENTTYPE_MALAYSIA;
		case ServiceSingapore:
			return CLIENTTYPE_SINGAPORE;
		case ServiceGermany:
			return CLIENTTYPE_GERMANY;
		case ServiceIndia:
			return CLIENTTYPE_INDIA;
		case ServiceBrazil:
			return CLIENTTYPE_BRAZIL;
		case ServiceAustralia:
			return CLIENTTYPE_AUSTRALIA;
		case ServiceVietnam:
			return CLIENTTYPE_VIETNAM_SAKRAY;
		default:
			return CLIENTTYPE_SAKRAY;
		}
	}
}


void CLoginMode::SetKeycryptErrorInfo(const char *pszErrorInfo)
{
	m_strErrorInfo = pszErrorInfo;
}


int CLoginMode::SendKeycryptErrorInfo()
{
	if (m_strErrorInfo.empty())
		return FALSE;

	char pBuffer[4096];
	char *pBufItr = pBuffer;
	DWORD dwAID = g_session.GetAid();
	if (dwAID == 0)
		return FALSE;

	itoa(dwAID, pBufItr, 10);
	strcat(pBufItr, "\n");
	pBufItr = pBufItr + strlen(pBufItr);
	memcpy(pBufItr, m_strErrorInfo.c_str(), m_strErrorInfo.length() + 1);
	pBufItr += m_strErrorInfo.length() + 1;
	SendUDPPacket("nplog.ragnarok.co.id", 6001, pBuffer, pBufItr - pBuffer);

	return TRUE;
}


char * CLoginMode::SetPaddingValue(char * pad, int len)
{
	if (len == 0) {
		len = sizeof(pad);
	}

	char buf[20] = { 0, };
	DWORD value = timeGetTime() >> len;
	value ^= timeGetTime();
	_ltoa(value, buf, 16);

	int len2 = strlen(buf) - 1;

	for (int i = 0; i < len; i++) {
		if (len2 - i >= 0)
			pad[i] = buf[len2 - i];
		else
			pad[i] = buf[len2];
	}

	pad[len - 1] = 0;
	return pad;
}

void CLoginMode::SendCaIpSecurity()
{
	PACKET_CA_LOGIN_PCBANG p;
	p.PacketType = HEADER_CA_LOGIN_PCBANG;
	p.Version = g_version;
	memcpy(p.ID, m_userId, 24);
	memcpy(p.Passwd, m_userPassword, 24);
	p.clienttype = GetAccountType();

	NCB ncb;
	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_command = NCBRESET;
	ncb.ncb_lana_num = 0;
	Netbios(&ncb);

	memset(&ncb, 0, sizeof(ncb));
	ncb.ncb_lana_num = 0;
	ncb.ncb_command = NCBASTAT;
	strcpy((char *)ncb.ncb_callname, "*               ");
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	UCHAR uRetCode = Netbios(&ncb);
	if (uRetCode == 0) {
		sprintf((char *)p.MacAdress, "%02x%02x%02x%02x%02x%02x\n",
			Adapter.adapt.adapter_address[0],
			Adapter.adapt.adapter_address[1],
			Adapter.adapt.adapter_address[2],
			Adapter.adapt.adapter_address[3],
			Adapter.adapt.adapter_address[4],
			Adapter.adapt.adapter_address[5]);
	} else {
		strcpy((char *)p.MacAdress, "111111111111");
	}

	char host[100];
	gethostname(host, 100);
	hostent *hostInfo = gethostbyname(host);
	if (!hostInfo) {
		strcpy(p.IP, "111.111.111.111");
	} else {
		strcpy(p.IP, inet_ntoa(*(in_addr *)hostInfo->h_addr_list[0]));
	}

	CRagConnection::instanceR().SendPacket(
		CRagConnection::instanceR().GetPacketSize(p.PacketType),
		(const char *)&p);
}

void CLoginMode::Zc_Aid(const char *buf)
{
	PACKET_ZC_AID *p = (PACKET_ZC_AID *)buf;
	sAssert(p->AID == g_session.GetAid());
}




void CLoginMode::Hc_CharNotBeenSelected(const char *buf)
{
	PACKET_HC_ACCEPT_ENTER *p = (PACKET_HC_ACCEPT_ENTER *)buf;
	memcpy(&m_billingInfo, p->m_extension, sizeof(m_billingInfo));
	m_billingInfo.code = ntohl(m_billingInfo.code);
	m_billingInfo.time1 = ntohl(m_billingInfo.time1);
	m_billingInfo.time2 = ntohl(m_billingInfo.time2);

	m_numChar = (p->PacketLength - sizeof(PACKET_HC_ACCEPT_ENTER)) / sizeof(CHARACTER_INFO);
	int billingtempsize = (p->PacketLength - sizeof(PACKET_HC_ACCEPT_ENTER)) % sizeof(CHARACTER_INFO);
	memcpy(&m_charInfo[0], ((char *)p) + sizeof(PACKET_HC_ACCEPT_ENTER) + billingtempsize,
		sizeof(CHARACTER_INFO) * m_numChar);
	SetNextSubState(LSM_SELECTCHAR2);
}

void CLoginMode::Hc_Ack_Is_Valid_CharName(const char *buf)
{
	if (*(WORD *)(buf + 2) != 0) {
		char msg[300];
		sprintf(msg, MsgStr(MSI_CONFIRM_TO_CHANGE_NAME), g_session.m_changeCharName);
		if (g_windowMgr.ErrorMsg(msg, ME_YESNO, 0, 0, 0) == ID_YES) {
			PACKET_CH_REQ_CHANGE_CHARNAME p;
			p.PacketType = HEADER_CH_REQ_CHANGE_CHARNAME;
			p.dwGID = g_session.m_changeNameGID;
			CRagConnection::instanceR().SendPacket(
				CRagConnection::instanceR().GetPacketSize(p.PacketType),
				(const char *)&p);
			return;
		}
		if (g_windowMgr.GetSelectCharWnd())
			g_windowMgr.GetSelectCharWnd()->SendMsg(NULL, UM_COMMAND, 0x11d, 0, 0, 0);
		return;
	}

	if (g_windowMgr.ErrorMsg(MsgStr(MSI_CHARACTER_NAME_ALREADY_EXISTS), ME_OK, TRUE, TRUE, 0) == ID_OK) {
		if (g_windowMgr.GetSelectCharWnd())
			g_windowMgr.GetSelectCharWnd()->SendMsg(NULL, UM_COMMAND, 0x11d, 0, 0, 0);
	}
}

void CLoginMode::Hc_Ack_Change_CharName(const char *buf)
{
	enum UPDATE_ERRORCODE {
		UE_SUCCESS,
		UE_ALREADYCHANGED,
		UE_NOTREGISTEDUSER,
		UE_QUERYFAILED,
		UE_DUPLICATEDCHARNAME,
	};

	switch (*(short *)(buf + 2)) {
	case UE_SUCCESS:
		g_windowMgr.ErrorMsg(MsgStr(MSI_CHANGE_NAME_SUCCESS), ME_OK, TRUE, 0, 0);
		if (g_windowMgr.GetSelectCharWnd())
			g_windowMgr.GetSelectCharWnd()->SendMsg(NULL, UM_COMMAND, 0x11e, 0, 0, 0);
		break;
	case UE_ALREADYCHANGED:
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_ALREADY_CHANGED_NAME), ME_OK, TRUE, TRUE, 0) == ID_OK) {
			if (g_windowMgr.GetSelectCharWnd())
				g_windowMgr.GetSelectCharWnd()->SendMsg(NULL, UM_COMMAND, 0x77, 0, 0, 0);
		}
		break;
	case UE_NOTREGISTEDUSER:
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_NOTREGISTED_USER), ME_OK, TRUE, TRUE, 0) == ID_OK) {
			if (g_windowMgr.GetSelectCharWnd())
				g_windowMgr.GetSelectCharWnd()->SendMsg(NULL, UM_COMMAND, 0x77, 0, 0, 0);
		}
		break;
	case UE_QUERYFAILED:
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_CHANGE_NAME_FAILURE), ME_OK, TRUE, TRUE, 0) == ID_OK) {
			if (g_windowMgr.GetSelectCharWnd())
				g_windowMgr.GetSelectCharWnd()->SendMsg(NULL, UM_COMMAND, 0x11d, 0, 0, 0);
		}
		break;
	case UE_DUPLICATEDCHARNAME:
		if (g_windowMgr.ErrorMsg(MsgStr(MSI_DUPLICATED_CHAR_NAME), ME_OK, TRUE, TRUE, 0) == ID_OK) {
			if (g_windowMgr.GetSelectCharWnd())
				g_windowMgr.GetSelectCharWnd()->SendMsg(NULL, UM_COMMAND, 0x11d, 0, 0, 0);
		}
		break;
	}
}





void CLoginMode::Hc_Refuse_SelectChar(const char *buf)
{
	switch ((unsigned char)buf[2]) {
	case 0:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_INVALID_SLOT), ME_OK, 1, 0, 0);
		break;
	case 1:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_BASELVL), ME_OK, 1, 0, 0);
		break;
	case 2:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_INVALID_JOBLV), ME_OK, 1, 0, 0);
		break;
	case 3:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_JOB), ME_OK, 1, 0, 0);
		break;
	case 4:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_MAP), ME_OK, 1, 0, 0);
		break;
	case 5:
		g_windowMgr.ErrorMsg(MsgStr(MSI_FR_INVALID_MONEY), ME_OK, 1, 0, 0);
		break;
	}
	if (g_connection.IsConnect())
		SetNextSubState(LSM_SELECTCHAR);
	else
		SetNextSubState(LSM_IDENTRY);
}
