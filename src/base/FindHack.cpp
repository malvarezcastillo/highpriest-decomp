#include "common.h"
#include "../MsgStrings.h"
#include "../framework/Locale.h"
#include "../packet.h"
#include "FindHack.h"
#include "../device/Connection.h"
#include "../device/md5.h"
#include "../framework/Language.h"

#include "../resource/Sprite.h"
#include "../resource/Action.h"
#include "../LoginMode.h"
#include "../Mode.h"
#include <direct.h>
#include <io.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <tlhelp32.h>
#include <windows.h>
#include <shellapi.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#include "../WinCrypt.h"
#include "../rdefk.h"
#include "../NPGameLib.h"

extern ServiceType g_serviceType;
extern ServerType g_serverType;
extern CModeMgr g_modeMgr;
extern const char* PATCH_CLIENT;
extern const char* MsgStr(MSGSTRINGID id);

CNPGameLib* pGameGuard = NULL;

typedef int (__cdecl *Initialize)();
typedef int (__cdecl *StartEncryption)();
typedef int (__cdecl *StopEncryption)();
typedef int (__cdecl *mCloseDriver)();
typedef int (__cdecl *GetCryptData)(unsigned char*);
typedef void (__cdecl *RegisterHWNDForEdit)(unsigned long, void*);
typedef int (__cdecl *GetInitError)();
typedef int (__cdecl *GetAthKbdObject)(char*);
typedef int (__cdecl *GetIMEHangulMode)(void*);
typedef int (__cdecl *InitReg)();
typedef void (__cdecl *InstallReg)();
typedef void (__cdecl *UninstallReg)();
typedef int (__cdecl *CheckReg)();
typedef int (__cdecl *LoadKindDriver)();

Initialize MyInitialize;
StartEncryption MyStartEncryption;
StopEncryption MyStopEncryption;
mCloseDriver MyCloseDriver;
GetCryptData MyGetCryptData;
RegisterHWNDForEdit MyRegisterHWNDForEdit;
GetInitError MyGetInitError;
GetAthKbdObject MyGetAthKbdObject;
GetIMEHangulMode MyGetIMEHangulMode;
InitReg MyInitReg;
InstallReg MyInstallReg;
UninstallReg MyUninstallReg;
CheckReg MyCheckReg;
LoadKindDriver MyLoadKindDriver;

typedef HKCRYPT (WINAPI *fnNPKOpenDriver)();
typedef HKCRYPT (WINAPI *fnNPKOpenDriverEx)(UINT);
typedef BOOL (WINAPI *fnNPKUnregCrypt)(HKCRYPT, HWND);
typedef BOOL (WINAPI *fnNPKSetKeyMode)(UINT);
typedef BOOL (WINAPI *fnNPKSetKbdLock)(BOOL);
typedef BOOL (WINAPI *fnNPKCloseDriver)(HKCRYPT);
typedef BOOL (WINAPI *fnNPKRegCryptMsg)(HKCRYPT, HWND, UINT);
typedef BOOL (WINAPI *fnNPKRegCryptFunc)(HKCRYPT, HWND, FCryptAction, PVOID);
typedef	void (WINAPI *fnNPKSetDrvPath)(PSTR szPath);
typedef _APPCOMPATFLAG (WINAPI *fnNPKGetAppCompatFlag)(LPCTSTR szFileName);
typedef BOOL (WINAPI *fnNPKSetAppCompatFlag)(LPCTSTR szFileName, _APPCOMPATFLAG apcfNewValue);

typedef struct tagNPKFuncTbl {
	fnNPKOpenDriver FNPKOpenDriver;
	fnNPKOpenDriverEx FNPKOpenDriverEx;
	fnNPKCloseDriver FNPKCloseDriver;
	fnNPKRegCryptMsg FNPKRegCryptMsg;
	fnNPKRegCryptFunc FNPKRegCryptFunc;
	fnNPKUnregCrypt FNPKUnregCrypt;
	fnNPKSetKeyMode FNPKSetKeyMode;
	fnNPKSetKbdLock FNPKSetKbdLock;
	fnNPKSetDrvPath FNPKSetDrvPath;
	fnNPKGetAppCompatFlag FNPKGetAppCompatFlag;
	fnNPKSetAppCompatFlag FNPKSetAppCompatFlag;
} NPKFuncTbl, *PNPKFuncTbl;

const char* fnNameTbl[] = {
	"NPKOpenDriver",
	"NPKOpenDriverEx",
	"NPKCloseDriver",
	"NPKRegCryptMsg",
	"NPKRegCryptFunc",
	"NPKUnregCrypt",
	"NPKSetKeyMode",
	"NPKSetKbdLock",
	"NPKSetDrvPath",
	"NPKGetAppCompatFlag",
	"NPKSetAppCompatFlag",
};

namespace FH {
	HINSTANCE hDll = NULL;
	NPKFuncTbl npkfuncs;
	HKCRYPT hKCrypt = NULL;
	DWORD dwError = ERROR_SUCCESS;
}

HINSTANCE g_hDll;


int NKCStartKeycrypt();
void CloseKeyCrypt();
DWORD Decrypt(unsigned char* pbKey, DWORD dwKeySize, unsigned char* pbCipherText, unsigned char* pbPlaintext, DWORD dwHowMuch);
void CALLBACK KeyData(void* pParam, unsigned char* pbBuff, DWORD dwBuffSize);
void SetKCErrorInfo(DWORD dwErrorCode);
std::string CreateVersionMsg();
std::string CreateDependsDllMsg();
std::string CreateDllHash();

bool IsEnableOldKeycrypt() { return false; }

bool IsEnableNewKeycrypt()
{
	sAssert(0 <= g_serviceType && g_serviceType < ServiceLast);
	sAssert(0 <= g_serverType && g_serverType < ServerLast);

	bool bUseNewKeycrypt[ServiceLast][ServerLast] = {
		{true, true, true, 0},
		{false, false, false, 0},
		{false, false, false, 0, 0, false},
		{false, false, false, 0},
		{false, false, false, 0},
		{false, false, false, 0},
		{true, true, true, 0},
		{true, true, true, 0, 0, true},
		{false, false, false, 0},
		{false, false, false, 0},
		{false, false, false, 0},
		{true, true, true, 0, 0, true},
		{true, true, false, 0},
		{false, false, false, 0},
		{false, false, false, 0},
		{false, false, false, 0},
		{false, false, false, 0},
		{false, false, false, 0},
	};

	bool bIsDebug;
#ifdef _DEBUG
	bIsDebug = true;
#else
	bIsDebug = false;
#endif

	return bUseNewKeycrypt[g_serviceType][g_serverType];
}

int FindHack2()
{
	switch (g_serviceType) {
	case ServiceKorea:
		pGameGuard = new CNPGameLib("Ragnarok");
		break;
	case ServiceIndonesia:
		pGameGuard = new CNPGameLib("RagnarokLyto");
		break;
	case ServicePhilippine:
		pGameGuard = new CNPGameLib("RagnarokPH");
		break;
	case ServiceBrazil:
		pGameGuard = new CNPGameLib("RagnarokBR");
		break;
	case ServiceIndia:
		pGameGuard = new CNPGameLib("RagnarokIN");
		break;
	case ServiceJapan:
		pGameGuard = new CNPGameLib("RagnarokJPTest");
		break;
	default:
		Trace("GameGuard does not run in this country.\n");
		return NPGAMEMON_SUCCESS;
	}
	return pGameGuard->Init();
}

int FindHack1()
{
	FILE* file = fopen("findhack.exe", "r");
	if (file == NULL)
		return -1;

	if (fseek(file, 0, SEEK_END)) {
		fclose(file);
		return -2;
	}

	DWORD fileSize = ftell(file);
	if (fseek(file, -4, SEEK_END)) {
		fclose(file);
		return -3;
	}

	DWORD writtenFileSize;
	fread(&writtenFileSize, 4, 1, file);
	fclose(file);

	if (writtenFileSize != fileSize)
		return -4;

	int ret = spawnl(P_WAIT, "findhack.exe", "findhack.exe", NULL);
	return ret;
}

int ProcessFindHack()
{
	char lpszMsg[1024];
	char msg[256];

	int dwResult = FindHack2();
	if (dwResult != NPGAMEMON_SUCCESS && 0 != dwResult) {
		switch (dwResult) {
		case 120:
		case 124:
		case 150:
		case 153:
			sprintf(lpszMsg, MsgStr(MSI_NPGAMEMON_ERROR_AUTH_GAMEGUARD));
			break;
		case 110:
			sprintf(lpszMsg, MsgStr(MSI_ERROR_EXIST));
			break;
		case 115:
			sprintf(lpszMsg, MsgStr(MSI_GAME_EXIST));
			break;
		case 114:
			sprintf(lpszMsg, MsgStr(MSI_ERROR_INIT));
			break;
		case 155:
			sprintf(lpszMsg, MsgStr(MSI_NPGAMEMON_ERROR_CRYPTOAPI));
			break;
		case 170:
			sprintf(lpszMsg, MsgStr(MSI_NPGAMEMON_ERROR_EXECUTE));
			break;
		case 360:
			sprintf(lpszMsg, MsgStr(MSI_NPGMUP_ERROR_AUTH));
			break;
		case 112:
			sprintf(lpszMsg, MsgStr(MSI_ERROR_NPSCAN));
			break;
		case 200:
			sprintf(lpszMsg, MsgStr(MSI_NPGAMEMON_ERROR_ILLEGAL_PRG));
			break;
		case 350:
			sprintf(lpszMsg, MsgStr(MSI_NPGMUP_ERROR_ABORT));
			break;
		case 340:
		case 380:
			sprintf(lpszMsg, MsgStr(MSI_NPGMUP_ERROR_CONNECT));
			break;
		case 100:
			if (ServerPrimary == g_serverType)
				return TRUE;
			sprintf(lpszMsg, MsgStr(MSI_NPGAMEMON_ERROR_100ERROR));
			break;
		default:
			wsprintf(lpszMsg, "GameGuard Error: %lu", dwResult);
			break;
		}
		wsprintf(msg, "GameGuard Error : %lu", dwResult);
		MessageBox(g_hMainWnd, lpszMsg, msg, MB_OK);
	} else {
		return TRUE;
	}

	return FALSE;
}

void SetHwnd()
{
	pGameGuard->SetHwnd(g_hMainWnd);
}

void SendID(char* szUserID)
{
	pGameGuard->Send(szUserID);
}

void ExitFindHack()
{
	if (pGameGuard)
		delete pGameGuard;
}

void Auth2(unsigned long* pAuth)
{
	pGameGuard->Auth2((PGG_AUTH_DATA)pAuth);
}

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	char lpszMsg[1024];
	bool bAppExit;

	switch (dwMsg) {
	case 1011:
		bAppExit = true;
		sprintf(lpszMsg, MsgStr(MSI_SPEEDHACK_DETECT));
		break;
	case 1001:
	case 1002:
		bAppExit = true;
		return false;
	case 1014:
		bAppExit = true;
		wsprintf(lpszMsg, MsgStr(MSI_INIT_ERROR), dwArg);
		break;
	case 1012:
	case 1013:
		bAppExit = true;
		char gameHack[256];
		strcpy(gameHack, pGameGuard->GetInfo());
		if (gameHack)
			wsprintf(lpszMsg, MsgStr(MSI_GAMEHACK_DETECT), gameHack);
		break;
	case 1015:
		bAppExit = true;
		sprintf(lpszMsg, MsgStr(MSI_GAMEHACK_DOUBT));
		break;
	case 1017:
		PACKET_CZ_ACK_GAME_GUARD p;
		p.PacketType = 0x228;
		memcpy(p.AuthData, (void*)dwArg, sizeof(_GG_AUTH_DATA));
		CRagConnection::instanceR().SendPacket(CRagConnection::instanceR().GetPacketSize(p.PacketType), (const char*)(&p));
		return true;
	}

	if (lpszMsg)
		MessageBox(g_hMainWnd, lpszMsg, "nProtect GameGuard", MB_OK);

	if (bAppExit) {
		CRagConnection::instanceR().Disconnect();
		g_modeMgr.Quit();
		return false;
	}

	return true;
}

void DeleteHack()
{
	if (access("nconfig.ini", 6) == 0)
		_unlink("nconfig.ini");
	if (access("ffindhack.exe", 6) == 0)
		_unlink("ffindhack.exe");
	if (access("gfindhack.exe", 6) == 0)
		_unlink("gfindhack.exe");
	if (access("removenp.exe", 6) == 0)
		_unlink("removenp.exe");
	if (access("cfindhack.exe", 6) == 0)
		_unlink("cfindhack.exe");
}

char* NP_ShortPathToLongPath(const char* lpFileName, char* lpBuf)
{
	TCHAR szFindName[MAX_PATH];
	char* lpS;
	char* lpB;
	char* lpW;
	WIN32_FIND_DATA FindFileDat;
	HANDLE hFindFile;

	lpS = (char*)lpFileName;
	lpB = szFindName;
	lpBuf[0] = '\0';
	lpW = lpBuf;
	if (':' == lpS[1]) {
		*lpW++ = *lpB++ = *lpS++;
		*lpW++ = *lpB++ = *lpS++;
		*lpW = '\0';
	}
	if ('\\' == lpS[0] && '\\' == lpS[1]) {
		*lpW++ = *lpB++ = *lpS++;
		*lpW++ = *lpB++ = *lpS++;
		for (; '\\' != *lpS && '\0' != *lpS;) {
			*lpW++ = *lpB++ = *lpS++;
		}
		*lpW++ = *lpB++ = *lpS++;
		for (; '\\' != *lpS && '\0' != *lpS;) {
			*lpW++ = *lpB++ = *lpS++;
		}
		*lpW = '\0';
	}
	if ('\\' == *lpS) {
		*lpB++ = *lpS++;
	}

	while (TRUE) {
		for (; '\\' != *lpS && '\0' != *lpS;) {
			*lpB++ = *lpS++;
		}
		*lpB = '\0';
		hFindFile = FindFirstFile(szFindName, &FindFileDat);
		if (INVALID_HANDLE_VALUE == hFindFile) {
			lpBuf[0] = '\0';
			return NULL;
		}
		FindClose(hFindFile);
		lstrcat(lpBuf, "\\");
		lstrcat(lpBuf, FindFileDat.cFileName);
		if ('\0' == *lpS)
			break;
		*lpB++ = *lpS++;
	}

	return lpBuf;
}

int NKCTestCompatibility()
{
	if (NULL == FH::npkfuncs.FNPKGetAppCompatFlag)
		return TRUE;

	char szShortModulePath[MAX_PATH];
	char szLongModulePath[MAX_PATH];
	GetModuleFileName(g_hInstance, szShortModulePath, MAX_PATH);
	NP_ShortPathToLongPath(szShortModulePath, szLongModulePath);
	char msg[256];
	sprintf(msg, "ShortModulePath:%s\nLongModulePath:%s\nComFlag result:%d",
			szShortModulePath, szLongModulePath, FH::npkfuncs.FNPKGetAppCompatFlag(szLongModulePath));

	char szAbsoluteClient[MAX_PATH];
	strcpy(szAbsoluteClient, szLongModulePath);
	int len = strlen(szAbsoluteClient);
	for (int i = len - 1; i >= 0; i--) {
		if (szAbsoluteClient[i] == '\\')
			break;
		else
			szAbsoluteClient[i] = NULL;
	}
	strcat(szAbsoluteClient, PATCH_CLIENT);
	if (FH::npkfuncs.FNPKGetAppCompatFlag(szLongModulePath) != apcfNone ||
		FH::npkfuncs.FNPKGetAppCompatFlag(szAbsoluteClient) != apcfNone) {
		FH::npkfuncs.FNPKSetAppCompatFlag(szLongModulePath, apcfNone);
		FH::npkfuncs.FNPKSetAppCompatFlag(szAbsoluteClient, apcfNone);
		return FALSE;
	}
	return TRUE;
}

int NpKeyMyLoadLibrary()
{
	g_hDll = NULL;

	if (!g_hDll) {
		if ((g_hDll = LoadLibrary("npkeysdk.dll")) == NULL) {
			MessageBox(NULL, "npkeysdk.dll Load Failed!", "npkeysdk", MB_OK);
			return FALSE;
		}

		if (((MyInitialize = (Initialize)GetProcAddress(g_hDll, "Initialize")) == NULL) ||
			((MyStartEncryption = (StartEncryption)GetProcAddress(g_hDll, "StartEncryption")) == NULL) ||
			((MyStopEncryption = (StopEncryption)GetProcAddress(g_hDll, "StopEncryption")) == NULL) ||
			((MyCloseDriver = (mCloseDriver)GetProcAddress(g_hDll, "CloseDriver")) == NULL) ||
			((MyGetCryptData = (GetCryptData)GetProcAddress(g_hDll, "GetCryptData")) == NULL) ||
			((MyRegisterHWNDForEdit = (RegisterHWNDForEdit)GetProcAddress(g_hDll, "RegisterHWNDForEdit")) == NULL) ||
			((MyGetAthKbdObject = (GetAthKbdObject)GetProcAddress(g_hDll, "GetAthKbdObject")) == NULL) ||
			((MyInitReg = (InitReg)GetProcAddress(g_hDll, "InitReg")) == NULL) ||
			((MyInstallReg = (InstallReg)GetProcAddress(g_hDll, "InstallReg")) == NULL) ||
			((MyUninstallReg = (UninstallReg)GetProcAddress(g_hDll, "UninstallReg")) == NULL) ||
			((MyCheckReg = (CheckReg)GetProcAddress(g_hDll, "CheckReg")) == NULL) ||
			((MyLoadKindDriver = (LoadKindDriver)GetProcAddress(g_hDll, "LoadKindDriver")) == NULL) ||
			((MyGetInitError = (GetInitError)GetProcAddress(g_hDll, "GetInitError")) == NULL) ||
			((MyGetIMEHangulMode = (GetIMEHangulMode)GetProcAddress(g_hDll, "GetIMEHangulMode")) == NULL)) {
			MessageBox(NULL, "npkeysdk.dll Load Failed!", "npkeysdkc", MB_OK);
			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL WINAPI InitNPKCryptFuncTbl()
{
	if (FALSE == IsEnableNewKeycrypt())
		return FALSE;

	PNPKFuncTbl pTbl = &FH::npkfuncs;
	ZeroMemory(pTbl, sizeof *pTbl);
	FH::hDll = LoadLibrary("npkcrypt.dll");
	if (FH::hDll != NULL) {
		pTbl->FNPKOpenDriver = (fnNPKOpenDriver)GetProcAddress(FH::hDll, fnNameTbl[0]);
		pTbl->FNPKOpenDriverEx = (fnNPKOpenDriverEx)GetProcAddress(FH::hDll, fnNameTbl[1]);
		pTbl->FNPKCloseDriver = (fnNPKCloseDriver)GetProcAddress(FH::hDll, fnNameTbl[2]);
		pTbl->FNPKRegCryptMsg = (fnNPKRegCryptMsg)GetProcAddress(FH::hDll, fnNameTbl[3]);
		pTbl->FNPKRegCryptFunc = (fnNPKRegCryptFunc)GetProcAddress(FH::hDll, fnNameTbl[4]);
		pTbl->FNPKUnregCrypt = (fnNPKUnregCrypt)GetProcAddress(FH::hDll, fnNameTbl[5]);
		pTbl->FNPKSetKeyMode = (fnNPKSetKeyMode)GetProcAddress(FH::hDll, fnNameTbl[6]);
		pTbl->FNPKSetKbdLock = (fnNPKSetKbdLock)GetProcAddress(FH::hDll, fnNameTbl[7]);
		pTbl->FNPKSetDrvPath = (fnNPKSetDrvPath)GetProcAddress(FH::hDll, fnNameTbl[8]);
		pTbl->FNPKGetAppCompatFlag = (fnNPKGetAppCompatFlag)GetProcAddress(FH::hDll, fnNameTbl[9]);
		pTbl->FNPKSetAppCompatFlag = (fnNPKSetAppCompatFlag)GetProcAddress(FH::hDll, fnNameTbl[10]);

		if (pTbl->FNPKOpenDriver == NULL ||
			pTbl->FNPKOpenDriverEx == NULL ||
			pTbl->FNPKCloseDriver == NULL ||
			pTbl->FNPKRegCryptMsg == NULL ||
			pTbl->FNPKRegCryptFunc == NULL ||
			pTbl->FNPKUnregCrypt == NULL ||
			pTbl->FNPKSetKeyMode == NULL ||
			pTbl->FNPKSetKbdLock == NULL ||
			pTbl->FNPKSetDrvPath == NULL ||
			pTbl->FNPKGetAppCompatFlag == NULL ||
			pTbl->FNPKSetAppCompatFlag == NULL) {
			FreeLibrary(FH::hDll);
			FH::hDll = NULL;
			return FALSE;
		}
	} else {
		FH::dwError = GetLastError();
		SetKCErrorInfo(FH::dwError);
		return FALSE;
	}

	return TRUE;
}

void WinMainNpKeyStartEncryption()
{
	if (IsEnableNewKeycrypt())
		NKCStartKeycrypt();
	else if (IsEnableOldKeycrypt())
		MyStartEncryption();
}

void WinMainNpKeyStopEncryption()
{
	if (IsEnableNewKeycrypt()) {
		CloseKeyCrypt();
	} else if (IsEnableOldKeycrypt()) {
		MyStopEncryption();
	}
}

DWORD Decrypt(unsigned char* pbKey, DWORD dwKeySize, unsigned char* pbCipherText, unsigned char* pbPlaintext, DWORD dwHowMuch)
{
	HCRYPTPROV hProv = NULL;
	HCRYPTKEY hKey = NULL;
	HCRYPTHASH hHash = NULL;

	DWORD dwBuff = dwHowMuch;
	DWORD dwRet = 0;

	memcpy(pbPlaintext, pbCipherText, dwHowMuch);

	try {
		if (CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE)
			throw GetLastError();

		if (CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash) == FALSE)
			throw GetLastError();

		if (CryptHashData(hHash, pbKey, dwKeySize, 0) == FALSE)
			throw GetLastError();

		if (CryptDeriveKey(hProv, CALG_RC4, hHash, CRYPT_EXPORTABLE, &hKey) == FALSE)
			throw GetLastError();

		if (CryptDecrypt(hKey, 0, TRUE, 0, pbPlaintext, &dwBuff) == FALSE)
			throw GetLastError();
	} catch (const DWORD dwLastError) {
		dwRet = dwLastError;
	}

	if (hKey)
		CryptDestroyKey(hKey);

	if (hHash)
		CryptDestroyHash(hHash);

	if (hProv)
		CryptReleaseContext(hProv, 0);

	return dwRet;
}

void CloseKeyCrypt()
{
	if (FH::hDll) {
		if (FH::hKCrypt > 0) {
			BOOL bRet;
			bRet = FH::npkfuncs.FNPKCloseDriver(FH::hKCrypt);
			FH::hKCrypt = NULL;
		}
		FreeLibrary(FH::hDll);
		FH::hDll = NULL;
	}
}

void CALLBACK KeyData(void* pParam, unsigned char* pbBuff, DWORD dwBuffSize)
{
	HWND hWnd = (HWND)pParam;
	TCHAR buff[1024];
	TCHAR passwd[256];

	sprintf(passwd, "%x", hWnd);
	Decrypt((unsigned char*)passwd, strlen(passwd), pbBuff, (unsigned char*)buff, dwBuffSize);

	sAssert(NULL == pbBuff[dwBuffSize] && dwBuffSize > 1);
	for (int i = 0; i < dwBuffSize - 1; ++i) {
		g_language->OnChar((WPARAM)buff[i], 0);
	}
}

int NKCStartKeycrypt()
{
	if (NULL == FH::hDll) {
		InitNPKCryptFuncTbl();
	}

	TCHAR szFileName[MAX_PATH] = {0};
	_getcwd(szFileName, MAX_PATH);
	char* ptr = strrchr(szFileName, '\\');

	FH::npkfuncs.FNPKSetDrvPath(szFileName);
	FH::hKCrypt = FH::npkfuncs.FNPKOpenDriver();
	if (FH::hKCrypt > 0) {
		FH::npkfuncs.FNPKRegCryptFunc(FH::hKCrypt, g_hMainWnd, KeyData, g_hMainWnd);
	} else {
		DWORD dwError = GetLastError();
		char szErrMsg[1024];

		if ((dwError & NPK_ERROR_BASE) == NPK_ERROR_BASE) {
			strcpy(szErrMsg, "Runs as ADMIN only.");
		} else {
			HLOCAL hlocal = NULL;

			BOOL fOk = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
									   NULL,
									   dwError,
									   MAKELANGID(LANG_KOREAN, SUBLANG_ENGLISH_US),
									   (PTSTR)&hlocal,
									   0,
									   NULL);

			if (fOk) {
				strcpy(szErrMsg, (LPCTSTR)LocalLock(hlocal));
				::LocalFree(hlocal);
			}
		}
		return FALSE;
	}

	return TRUE;
}

std::string CreateVersionMsg()
{
	char szBuffer[1024];
	OSVERSIONINFOEX osv;

	osv.dwOSVersionInfoSize = sizeof osv;
	GetVersionEx((OSVERSIONINFO*)&osv);

	_snprintf(szBuffer,
			  sizeof(szBuffer),
			  "Version=%u,%u\r\nBuild=%u\r\nPlatform=%s\r\nCSDVersion=%s\r\nServicePack=%d,%d\r\n",
			  osv.dwMajorVersion,
			  osv.dwMinorVersion,
			  osv.dwBuildNumber,
			  osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ? "9x" : "NT",
			  osv.szCSDVersion,
			  osv.wServicePackMajor,
			  osv.wServicePackMinor);
	std::string strVersion(szBuffer);
	return strVersion;
}

#include "rag_entropy_pad.h"
RAG_ENTROPY_SHIFT(dependsdll, 42)
std::string CreateDependsDllMsg()
{
	std::string strDllInfo;
	TCHAR szSysDir[MAX_PATH] = {0};
	TCHAR szModuleDir[MAX_PATH] = {0};
	TCHAR szDllName[MAX_PATH];
	TCHAR* pSlash;

	GetSystemDirectory(szSysDir, sizeof szSysDir);
	GetModuleFileName(NULL, szModuleDir, sizeof szModuleDir);
	pSlash = strchr(szModuleDir, '\\');
	if (pSlash)
		*pSlash = '\0';

	char* SysDllName[] = {"kernel32.dll", "user32.dll", "gdi32.dll", "comdlg32.dll", "winspool.drv",
						  "advapi32.dll", "shell32.dll", "comctl32.dll", "imm32.dll", "setupapi.dll"};
	for (int i = 0; i < sizeof(SysDllName) / sizeof(char*); ++i) {
		strcpy(szDllName, szSysDir);
		strcat(szDllName, "\\");
		strcat(szDllName, SysDllName[i]);
		if (0 == access(szDllName, 0))
			strDllInfo += "Y\n";
		else
			strDllInfo += "N\n";
	}
	return strDllInfo;
}
RAG_ENTROPY_SHIFT(dependsdll_restore, 21)

int GetMD5CodeFromFile(const char* szPath, char* szMD5Hash)
{
	FILE* f;
	int i, j;
	MD5 hash;
	unsigned char buf[1000];
	unsigned char md5sum[16];
	const char szHexMap[] = "0123456789ABCDEF";
	BOOL bRet = FALSE;

	if ((f = fopen(szPath, "rb")) != NULL) {
		hash.init();

		while ((i = fread(buf, 1, sizeof(buf), f)) > 0)
			hash.append(buf, i);

		hash.finish(md5sum);

		for (j = 0; j < 16; ++j) {
			szMD5Hash[j * 2] = szHexMap[md5sum[j] >> 4];
			szMD5Hash[j * 2 + 1] = szHexMap[md5sum[j] & 0xf];
		}
		szMD5Hash[j * 2] = '\0';

		fclose(f);
	}

	return bRet;
}

std::string CreateDllHash()
{
	std::string strHash;
	char szDllName[MAX_PATH];
	TCHAR szWorkingDir[MAX_PATH] = {0};
	char szMD5[33] = {0};
	MD5 hash;
	hash.init();
	char* ModuleDllName[] = {"npkpdb.dll", "npkcrypt.dll"};
	_getcwd(szWorkingDir, sizeof(szWorkingDir));
	if ('\\' != szWorkingDir[strlen(szWorkingDir) - 1])
		strcat(szWorkingDir, "\\");
	for (int i = 0; i < sizeof(ModuleDllName) / sizeof(char*); ++i) {
		strcpy(szDllName, szWorkingDir);
		strcat(szDllName, ModuleDllName[i]);
		if (0 == _access(szDllName, 0)) {
			GetMD5CodeFromFile(szDllName, szMD5);
			strHash += szMD5;
			strHash += "\n";
		} else {
			strHash += ModuleDllName[i];
			strHash += " File not found\n";
		}
	}
	return strHash;
}

void CALLBACK ModuleEnum(HINSTANCE hModule, void* pUserData)
{
	TCHAR szPath[MAX_PATH];
	TCHAR szMsg[4096];
	char* szBuffer = (char*)pUserData;

	GetModuleFileName(hModule, szPath, sizeof(szPath));
	char* pszFile = strrchr(szPath, '\\');
	if (pszFile == NULL)
		pszFile = szPath;
	else
		pszFile += 1;

	_snprintf(szMsg, sizeof(szMsg), "%08X %s\r\n", hModule, pszFile);
	strcat(szBuffer, szMsg);
}

void SetKCErrorInfo(DWORD dwErrorCode)
{
	std::string strInfo;
	char szErrorNum[16];
	itoa(dwErrorCode, szErrorNum, 10);
	strInfo += szErrorNum;
	strInfo += "\n";
	strInfo += CreateDependsDllMsg();
	strInfo += CreateDllHash();
	strInfo += CreateVersionMsg();

	char szModule[4096];
	ZeroMemory(szModule, sizeof(szModule));
	IProcess::Inst()->EnumModule(GetCurrentProcessId(), ModuleEnum, szModule);
	strInfo += szModule;

	g_modeMgr.GetLoginMode()->SetKeycryptErrorInfo(strInfo.c_str());
}

IProcess::IProcess()
{
	Load();

	if (IsWin9X() == FALSE)
		EnableDebugPrivilege(TRUE);
}

IProcess::~IProcess()
{
	Unload();

	if (IsWin9X() == FALSE)
		EnableDebugPrivilege(FALSE);
}

IProcess* IProcess::Inst()
{
	static IProcess _inst;

	if (!_inst.Bad())
		return &_inst;
	else
		return NULL;
}


int IProcess::Bad() { return m_bErr; }

void IProcess::Load()
{
	if (IsWin9X() == TRUE) {
		m_bNT = FALSE;
		Load9X();
	} else {
		m_bNT = TRUE;
		LoadNT();
	}
}

int IProcess::EnableDebugPrivilege(int bEnable)
{
	HANDLE hToken;
	BOOL bRet = FALSE;
	HINSTANCE hDll = NULL;
	FOpenProcessToken fnOpenProcessToken;
	FLookupPrivilegeValue fnLookupPrivilegeValue;
	FAdjustTokenPrivileges fnAdjustTokenPrivileges;

	if (GetProcAddresses(&hDll, "advapi32.dll", 3,
						 &fnOpenProcessToken, "OpenProcessToken",
						 &fnLookupPrivilegeValue, "LookupPrivilegeValueA",
						 &fnAdjustTokenPrivileges, "AdjustTokenPrivileges") == TRUE) {
		if (fnOpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount = 1;
			fnLookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
			tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
			fnAdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
			bRet = (GetLastError() == ERROR_SUCCESS);
			CloseHandle(hToken);
		}

		FreeLibrary(hDll);
	}

	return bRet;
}

void IProcess::Load9X()
{
	m_bErr = TRUE;

	m_hDll = LoadLibrary("kernel32.dll");

	if (m_hDll != NULL) {
		fnEnumProcessModules = NULL;
		fnGetModuleFileNameEx = NULL;

		fnCreateToolhelp32Snapshot = (FCreateToolhelp32Snapshot)GetProcAddress(m_hDll, "CreateToolhelp32Snapshot");
		fnModule32First = (FModule32First)GetProcAddress(m_hDll, "Module32First");
		fnModule32Next = (FModule32Next)GetProcAddress(m_hDll, "Module32Next");
		fnProcess32First = (FProcess32First)GetProcAddress(m_hDll, "Process32First");
		fnProcess32Next = (FProcess32Next)GetProcAddress(m_hDll, "Process32Next");

		if (fnCreateToolhelp32Snapshot != NULL &&
			fnModule32First != NULL &&
			fnModule32Next != NULL &&
			fnProcess32First != NULL &&
			fnProcess32Next != NULL) {
			m_bErr = FALSE;
		}
	}
}

void IProcess::LoadNT()
{
	m_bErr = TRUE;

	m_hDll = LoadLibrary("psapi.dll");
	if (m_hDll != NULL) {
		fnEnumProcessModules = (FEnumProcessModules)GetProcAddress(m_hDll, "EnumProcessModules");
		fnGetModuleFileNameEx = (FGetModuleFileNameEx)GetProcAddress(m_hDll, "GetModuleFileNameExA");

		fnCreateToolhelp32Snapshot = NULL;
		fnModule32First = NULL;
		fnModule32Next = NULL;
		fnProcess32First = NULL;
		fnProcess32Next = NULL;

		if (fnEnumProcessModules != NULL && fnGetModuleFileNameEx != NULL) {
			m_bErr = FALSE;
		}
	}
}

void IProcess::Unload()
{
	if (m_hDll)
		FreeLibrary(m_hDll);
}

int IProcess::GetProcessModule(unsigned long dwPID, unsigned long dwModuleID, tagMODULEENTRY32* lpMe32, unsigned long cbMe32)
{
	BOOL bRet = FALSE;
	BOOL bFound = FALSE;
	HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32 = {0};

	hModuleSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == (HANDLE)-1) {
		DWORD dw = GetLastError();
		return FALSE;
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (fnModule32First(hModuleSnap, &me32)) {
		do {
			if (me32.th32ModuleID == dwModuleID) {
				CopyMemory(lpMe32, &me32, cbMe32);
				bFound = TRUE;
				break;
			}
		} while (!bFound && fnModule32Next(hModuleSnap, &me32));
		bRet = bFound;
	} else {
		bRet = FALSE;
	}

	CloseHandle(hModuleSnap);
	return bRet;
}

int IProcess::EnumModule9X(unsigned long dwPID, FEnumModuleCallback FEnum, void* pUserData)
{
	BOOL bRet = FALSE;
	BOOL bFound = FALSE;
	HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32 = {0};

	hModuleSnap = fnCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == (HANDLE)-1) {
		DWORD dw = GetLastError();
		return FALSE;
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (fnModule32First(hModuleSnap, &me32)) {
		do {
			FEnum(me32.hModule, pUserData);
		} while (fnModule32Next(hModuleSnap, &me32));
		bRet = TRUE;
	}

	CloseHandle(hModuleSnap);
	return bRet;
}

int IProcess::IsWin9X()
{
	OSVERSIONINFOEX osi;
	DWORDLONG dwlConditionMask = 0;
	BOOL bRet = FALSE;
	BOOL bVerified = FALSE;

	ZeroMemory(&osi, sizeof(OSVERSIONINFOEX));
	osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((OSVERSIONINFO*)&osi) == 0) {
		osi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (GetVersionEx((OSVERSIONINFO*)&osi) == 0)
			return 0;
	}

	return osi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
}

int IProcess::EnumModuleNT(unsigned long dwPid, FEnumModuleCallback FEnum, void* pUserData)
{
	HMODULE* pList = NULL;
	DWORD cbNeeded = 0;
	DWORD nMax = 256;
	HANDLE hProcess;
	INT cnProcesses, i;
	BOOL bRet = FALSE;

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (hProcess == NULL)
		return bRet;

	for (;;) {
		pList = new HMODULE[nMax];
		if (fnEnumProcessModules(hProcess, pList, sizeof(DWORD) * nMax, &cbNeeded) == FALSE) {
			nMax = -1;
			break;
		}

		if (nMax >= cbNeeded)
			break;
		else {
			delete[] pList;
			nMax += 6;
		}
	}

	if (nMax > 0) {
		cnProcesses = cbNeeded / sizeof(DWORD);

		for (i = 0; i < cnProcesses; ++i) {
			FEnum(pList[i], pUserData);
		}

		bRet = TRUE;
	}

	CloseHandle(hProcess);
	delete[] pList;
	return bRet;
}

int IProcess::EnumModule(unsigned long dwPid, FEnumModuleCallback FEnum, void* pUserData)
{
	BOOL bRet;

	if (IsWin9X() == TRUE)
		bRet = EnumModule9X(dwPid, FEnum, pUserData);
	else
		bRet = EnumModuleNT(dwPid, FEnum, pUserData);

	return bRet;
}

int IProcess::GetProcAddresses(HINSTANCE__** hLibrary, const char* lpszLibrary, int nCount, ...)
{
	BOOL bRet = TRUE;
	va_list va;
	va_start(va, nCount);

	if ((*hLibrary = LoadLibrary(lpszLibrary)) != NULL) {
		FARPROC* lpfProcFunction = NULL;
		char* lpszFuncName = NULL;
		INT nIdxCount = 0;
		while (nIdxCount < nCount) {
			lpfProcFunction = va_arg(va, FARPROC*);
			lpszFuncName = va_arg(va, char*);
			if ((*lpfProcFunction = GetProcAddress(*hLibrary, lpszFuncName)) == NULL) {
				lpfProcFunction = NULL;
				bRet = FALSE;
				break;
			}
			nIdxCount++;
		}
	} else {
		bRet = FALSE;
	}
	va_end(va);

	if (bRet == FALSE && *hLibrary != NULL)
		FreeLibrary(*hLibrary);

	return bRet;
}
