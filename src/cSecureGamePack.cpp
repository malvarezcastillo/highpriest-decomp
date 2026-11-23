#include <windows.h>
#include "common.h"
#include "MsgStrings.h"








#include "cSecureGamePack.h"
#include "Mode.h"
#include "device/Connection.h"

#include <stdio.h>

extern UIWindowMgr g_windowMgr;
extern CModeMgr    g_modeMgr;

const char* MsgStr(MSGSTRINGID id);




#define SGP_DLL      "IMESCHLP"
#define SGP_ENTRY    "QueryIMEHelper"
#define SGP_VERSION  "5,01,2600,1832\0"

cSecureGamePack* cSecureGamePack::m_pInst = 0;

cSecureGamePack::cSecureGamePack()
{
	if (m_pInst == 0)
		m_pInst = this;
}





cSecureGamePack::~cSecureGamePack()
{
	HMODULE hLib = ::LoadLibraryA(SGP_DLL);
	if (hLib)
	{
		FARPROC (__stdcall *pfn)(unsigned int, const char*) =
			(FARPROC (__stdcall *)(unsigned int, const char*))::GetProcAddress(hLib, SGP_ENTRY);
		if (pfn)
		{
			void (__cdecl *cleanup)(const char*) =
				(void (__cdecl *)(const char*))pfn(0x9000F004, SGP_VERSION);
			if (cleanup)
				cleanup(SGP_VERSION);
			::FreeLibrary(::GetModuleHandleA(SGP_DLL));
			::OutputDebugStringA("Secure Game Pack was unloaded successfully");
		}
	}
}

void cSecureGamePack::Destroy()
{



	if (m_pInst)
	{
		delete m_pInst;
		m_pInst = 0;
	}
}

bool cSecureGamePack::InitSecureGamePack()
{



	char strMsg[256];
	DWORD rc;
	unsigned int (__cdecl *pError)();

	FARPROC (__stdcall *pfn)(unsigned int, const char*) =
		(FARPROC (__stdcall *)(unsigned int, const char*))::GetProcAddress(::LoadLibraryA(SGP_DLL), SGP_ENTRY);
	if (pfn == 0)
	{
		::MessageBoxA(g_hMainWnd,
					  "Failed to load IMESCHLP.dll",
					  ":::Warning:::",
					  MB_ICONINFORMATION);
		return 0;
	}

	char szBuffer[128] = {0};


	int (__cdecl *pInit)(HWND, char*, void (__cdecl*)(void*), const char*) =
		(int (__cdecl *)(HWND, char*, void (__cdecl*)(void*), const char*))pfn(0x9000F002, SGP_VERSION);
	if (pInit)
	{
		rc = pInit(g_hMainWnd, szBuffer, OnNotifyFromSGP, SGP_VERSION);
		switch (rc)
		{
		case 0x8000E001:
			{
				g_windowMgr.ErrorMsg(MsgStr(MSI_SGP_CODE_ALREADY_RUNNING), ME_OK, 1, 0, 0);

				LeaveSecureGamePack();
			}
			break;
		case 1:
			{

				int (__cdecl *pVerify)(HWND, char*) = (int (__cdecl *)(HWND, char*))pfn(0x9000F001, SGP_VERSION);
				if (pVerify)
				{
					if (pVerify(g_hMainWnd, szBuffer) == 0)
					{
						::MessageBoxA(g_hMainWnd, MsgStr(MSI_SGP_CODE_NOT_EQUALENCRYPTEDDATA), "::: Warning :::", 1);
						return 0;
					}
				}
				::OutputDebugStringA("Secure Game Pack started successfully");
			}
			break;
		}


		if (rc != 1)
		{
			pError = (unsigned int (__cdecl *)())pfn(0x9000F005, SGP_VERSION);
			if (pError)
			{
				sprintf(strMsg,
						"An internal error occurred; cannot continue.     \n\nError code: 0x%08X, 0x%08X",
						rc,
						pError());
				::MessageBoxA(g_hMainWnd, strMsg, ":::Warning:::", MB_OK);
			}
		}
	}

	return 1;
}

void cSecureGamePack::OnNotifyFromSGP(void* lpNotifyData)
{




	unsigned int event = *((unsigned int*)lpNotifyData);

	switch (event)
	{
	case 0x8000E001:
		g_windowMgr.ErrorMsg(MsgStr(MSI_SGP_CODE_ALREADY_RUNNING),     ME_OK, 1, 0, 0);
		break;
	case 0x8000E003:
		g_windowMgr.ErrorMsg(MsgStr(MSI_SGP_CODE_DETECTED_MACRO),      ME_OK, 1, 0, 0);
		break;
	case 0x8000E004:
		g_windowMgr.ErrorMsg(MsgStr(MSI_SGP_CODE_DETECTED_SPEEDHACK),  ME_OK, 1, 0, 0);
		break;
	case 0x8000E005:
		g_windowMgr.ErrorMsg(MsgStr(MSI_SGP_CODE_DETECTED_APIHOOK),    ME_OK, 1, 0, 0);
		break;
	case 0x8000E006:
		g_windowMgr.ErrorMsg(MsgStr(MSI_SGP_CODE_DETECTED_MSGHOOK),    ME_OK, 1, 0, 0);
		break;
	default:
		break;
	}

	FARPROC (__stdcall *pfn)(unsigned int, const char*) =
		(FARPROC (__stdcall *)(unsigned int, const char*))::GetProcAddress(::LoadLibraryA(SGP_DLL), SGP_ENTRY);
	if (pfn)
	{
		int (__cdecl *leave)(HWND, const char*) =
			(int (__cdecl *)(HWND, const char*))pfn(0x9000F003, SGP_VERSION);
		if (leave)
			leave(g_hMainWnd, SGP_VERSION);
	}

	CRagConnection::instanceR().Disconnect();
	g_modeMgr.Quit();
}

void cSecureGamePack::DestroySecureGamePack()
{
	HMODULE hLib = ::LoadLibraryA(SGP_DLL);
	if (hLib)
	{
		FARPROC (__stdcall *pfn)(unsigned int, const char*) =
			(FARPROC (__stdcall *)(unsigned int, const char*))::GetProcAddress(hLib, SGP_ENTRY);
		if (pfn)
		{
			void (__cdecl *cleanup)(const char*) =
				(void (__cdecl *)(const char*))pfn(0x9000F004, SGP_VERSION);
			if (cleanup)
				cleanup(SGP_VERSION);
			::FreeLibrary(::GetModuleHandleA(SGP_DLL));
			::OutputDebugStringA("Secure Game Pack was unloaded successfully");
		}
	}
}

void cSecureGamePack::LeaveSecureGamePack()
{
	FARPROC (__stdcall *pfn)(unsigned int, const char*) =
		(FARPROC (__stdcall *)(unsigned int, const char*))::GetProcAddress(::LoadLibraryA(SGP_DLL), SGP_ENTRY);
	if (pfn)
	{
		int (__cdecl *leave)(HWND, const char*) =
			(int (__cdecl *)(HWND, const char*))pfn(0x9000F003, SGP_VERSION);
		if (leave)
			leave(g_hMainWnd, SGP_VERSION);
	}
}
