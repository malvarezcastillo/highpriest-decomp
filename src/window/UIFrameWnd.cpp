#include "common.h"
#include "../MsgStrings.h"
#include "../framework/Locale.h"
#include <string>

#include "../resource/Wave.h"
#include "../device/Sound.h"
#include "../pdb_enums.h"
#include "../ItemInfo.h"
#include "../GameActor.h"
#include "../GameMode.h"
#include "../Session.h"
#include "../Insult.h"
#include "../skin.h"
#include "UIFrameWnd.h"
#include "UISys.h"
#include "../SnapMgr.h"
#include "../base/Res.h"
#include "../base/File.h"
#include "../base/Util.h"
#include "../3dlib/Renderer.h"
#include "../device/Control.h"
#include "../resource/Bitmap.h"
#include "../resource/Action.h"
#include "../resource/ImfRes.h"
#include "../resource/Sprite.h"
#include "../resource/PaletteRes.h"
#include "../movie/CBink.h"
#include <windows.h>
#include <shellapi.h>
#include <mmsystem.h>
#include <time.h>

extern UIWindowMgr g_windowMgr;
void CutString(const char* msg, std::vector<std::string>& strings, int maxNumCharLine);
extern CSnapMgr g_snapMgr;
extern const char* UIBmp(const char* uiBmpName);
extern const char* MsgStr(MSGSTRINGID id);
extern const char* g_regstrationWeb;
extern const char* g_regPath;
extern ServiceType g_serviceType;
extern ServerType g_serverType;
extern bool g_extended_slot;
extern char g_isGravityID;
extern char g_curHardness;
extern char g_curSave;
extern char g_curEXPmode;
extern char g_TIME_hr;
extern char g_TIME_min;
extern unsigned long g_TimeStorage;
extern bool game_ItemRentEvent;
extern char g_Aco_Name[];
extern char g_Aco_Hair;
extern char g_BS_Hair;
extern char g_Aco_Hair_Color;
extern char g_BS_Hair_Color;
extern CHARACTER_INFO g_charInfo;


extern CSession g_session;
extern CModeMgr g_modeMgr;
extern CRenderer* g_renderer;
extern CBink g_Bink;


UIFrameWnd::UIFrameWnd()
{
	m_defPushId = 0x75;
	m_defCancelPushId = 0x75;
	m_startGlobalX = 0;
	m_startGlobalY = 0;
}


UIFrameWnd::~UIFrameWnd()
{
}

void UIFrameWnd::SaveOriginalPos()
{
	m_orgX = m_x;
	m_orgY = m_y;
}

void UIFrameWnd::OnLBtnDown(int x, int y)
{
	if (!this) return;
	g_windowMgr.SetCapture(this);
	int gx = x, gy = y;
	GetGlobalCoor(gx, gy);
	m_startGlobalX = gx;
	m_startGlobalY = gy;
	m_orgX = m_x;
	m_orgY = m_y;
	m_transTarget = 0x80;
	m_transTime = timeGetTime();
	g_snapMgr.BeginMove(this);
}

void UIFrameWnd::OnLBtnUp(int x, int y)
{
	if (g_windowMgr.GetCapture() == this) {
		m_transTarget = 0xff;
		m_transTime = timeGetTime();
		g_windowMgr.ReleaseCapture();
		g_snapMgr.EndMove(this);
	}
}

void UIFrameWnd::Move(int x, int y)
{
	m_x = x;
	m_y = y;
	RefreshSnap();
}

void UIFrameWnd::RefreshSnap()
{
	int dx;
	int dy;
	g_windowMgr.GetDeltaSnap(this, dx, dy, 0);
	m_x += dx;
	m_y += dy;
	g_windowMgr.UpdateSnapInfo(this);
}

void UIFrameWnd::OnMouseHover(int x, int y) {}

void UIFrameWnd::OnMouseMove(int x, int y)
{
	if (g_windowMgr.GetCapture() == this) {
		int gx = x, gy = y;
		GetGlobalCoor(gx, gy);

		int deltaX = gx - m_startGlobalX;
		int deltaY = gy - m_startGlobalY;
		if (!g_snapMgr.IsRegisteredToSnap(this)) {
			Move(m_orgX + deltaX, m_orgY + deltaY);
			return;
		}

		MoveDelta(deltaX, deltaY);

		int dx;
		int dy;
		g_windowMgr.GetDeltaSnap(this, dx, dy, 0);

		int fDy;
		int fDx;
		fDx = deltaX;
		fDy = deltaY;
		g_snapMgr.GetDeltaDrag(this, fDx, fDy);
		if (abs(fDx) < abs(dx)) {
			fDx = dx;
		}
		if (abs(fDy) < abs(dy)) {
			fDy = dy;
		}

		MoveDelta(fDx + deltaX, fDy + deltaY);
		g_windowMgr.UpdateSnapInfo(this);
		g_snapMgr.MoveDelta(this, fDx + deltaX, fDy + deltaY);
	}
}

void UIFrameWnd::MoveDelta(int deltaDragX, int deltaDragY)
{
	m_x = m_orgX + deltaDragX;
	m_y = m_orgY + deltaDragY;
}

int UIFrameWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_DEFPUSH:
		if (m_defPushId)
			SendMsg(this, UM_COMMAND, m_defPushId, 0, 0, 0);
		break;
	case UM_CANCELPUSH:
		if (m_defCancelPushId)
			SendMsg(this, UM_COMMAND, m_defCancelPushId, 0, 0, 0);
		break;
	default:
		return UIWindow::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

void UIFrameWnd::DrawItemWithCount(int x, int y, ITEM_INFO const &iteminfo, bool forceidentified, bool boldtext)
{
	char text[16];
	DrawItem(x, y, iteminfo, forceidentified);

	if (iteminfo.m_num) {
		short textX = 14;
		sprintf(text, "%d", iteminfo.m_num);
		if (boldtext) {
			TextOutA(x + 16, y + 20, text, 0, 0, 11, 0);
			return;
		}

		if (iteminfo.m_num >= 1000) {
			textX = 0;
		}
		TextOutA((x + textX) + 1, y + 15, text, 0, 0, 11, 0xffffff);
		TextOutA((x + textX) - 1, y + 15, text, 0, 0, 11, 0xffffff);
		TextOutA((x + textX), y + 16, text, 0, 0, 11, 0xffffff);
		TextOutA((x + textX), y + 14, text, 0, 0, 11, 0xffffff);
		TextOutA((x + textX), y + 15, text, 0, 0, 11, 0);
	}
}

#include "rag_budget_fill.h"
#include "rag_symbol_shift.h"



void UIFrameWnd::DrawItem(int x, int y, ITEM_INFO const &iteminfo, bool forceidentified)
{
	DrawItem(x, y, iteminfo.m_itemName.c_str(), iteminfo.m_isIdentified || forceidentified);
}

void UIFrameWnd::DrawItem(int x, int y, char const *itemName, bool identified)
{
	char text[128];
	g_session.GetItemBitmapFileName(itemName, text, identified);
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(text, false);
	if (bmp) DrawBitmap(x, y, bmp, true);
}

void UIFrameWnd::DrawItemFrame(int x, int y)
{
	DrawBitmap(x, y, (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_mid.bmp"), false), false);
}

void UIFrameWnd::DrawLatticeFrame(int x, int y, int width, int height)
{
	if (width == 0) width = m_w;
	if (height == 0) height = m_h;

	for (int i = 0; i < (width - 28) / 14 + ((width - 28) % 14 != 0); ++i) {
		CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_mu.bmp"), false);
		DrawBitmap(x + (i + 1) * 14, y, bmp, false);
	}

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_lu.bmp"), false);
	DrawBitmap(x, y, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_ru.bmp"), false);
	DrawBitmap(x + width - bmp->m_width, y, bmp, false);

	for (i = 0; i < (height - 28) / 14 + ((height - 28) % 14 != 0); ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_lm.bmp"), false);
		DrawBitmap(x, y + (i + 1) * 14, bmp, false);
	}

	for (i = 0; i < (height - 28) / 14 + ((height - 28) % 14 != 0); ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_rm.bmp"), false);
		DrawBitmap(x + width - bmp->m_width, y + (i + 1) * 14, bmp, false);
	}

	for (i = 0; i < (width - 28) / 14 + ((width - 28) % 14 != 0); ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_md.bmp"), false);
		DrawBitmap(x + (i + 1) * 14, y + height - bmp->m_height, bmp, false);
	}

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_ld.bmp"), false);
	DrawBitmap(x, y + height - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysbox_rd.bmp"), false);
	DrawBitmap(x + width - bmp->m_width, y + height - bmp->m_height, bmp, false);

	{
		unsigned long gg;
		unsigned long rr;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(x + 14, y + 14, width - 28, height - 28, RGB(rr, gg, bb));
	}
}

UIMessageBoxAutoreturn::UIMessageBoxAutoreturn()
{
	m_isAutoReturn = 0;
}


UIMessageBoxAutoreturn::~UIMessageBoxAutoreturn()
{
}

void UIMessageBoxAutoreturn::Create(char const *msg, unsigned long autoReturnTime)
{
	m_msg = msg;
	m_countMsg = msg;
	m_openTime = timeGetTime();
	m_autoReturnTime = timeGetTime() + autoReturnTime;
	m_remainSecond = (m_autoReturnTime - m_openTime) / 1000 + ((m_autoReturnTime - m_openTime) % 1000 != 0);
	m_maxSecond = m_remainSecond;
	if (autoReturnTime) {
		m_isAutoReturn = TRUE;
		char text[512];
		sprintf(text, m_countMsg.c_str(), m_maxSecond);
		m_msg = text;
	} else {
		m_isAutoReturn = FALSE;
	}

	int cx = 280;
	int cy = 120;

	UIWindow::Create(cx, cy);
	OnCreate(cx, cy);
	OnSize(cx, cy);

	Move((g_renderer->m_width - 640) / 2 + 185, (g_renderer->m_height - 480) / 2 + 180);

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 22, 260, 48, false);
	unsigned long rr, gg, bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);
	m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)m_msg.c_str(), 0x26, 0, 0);

	m_barGraph = new UIBarGraph2;
	m_barGraph->Create(240, 15);
	m_barGraph->Move(20, m_h - 50);
	m_barGraph->SetAmount(0.0f);

	g_skinMgr.GetColorChipColor(0x12, 2, rr, gg, bb);
	m_barGraph->m_backR = rr;
	m_barGraph->m_backG = gg;
	m_barGraph->m_backB = bb;
	g_skinMgr.GetColorChipColor(0x16, 2, rr, gg, bb);
	m_barGraph->m_r = rr;
	m_barGraph->m_g = gg;
	m_barGraph->m_b = bb;
	AddChild(m_barGraph);



	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char *buttonName[][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b", },
		{ "btn_ok", "btn_ok_a", "btn_ok_b", },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b", },
	};
	int ids[] = { ID_OK, ID_YES, ID_NO, };
	int pos[][2] = {
		{ 189, 96 },
		{ 189, 96 },
		{ 234, 96 },
	};

	int index = 2;
	UIBitmapButton *button = new UIBitmapButton;
	button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), 0);
	button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), 1);
	button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), 2);
	button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
	button->Move(pos[index][0], pos[index][1]);
	button->SetId(ID_CANCEL);
	AddChild(button);
	m_defPushId = 0x77;
}

void UIMessageBoxAutoreturn::OnProcess()
{
	int prevRemainSecond;
	char text[512];

	if (m_isAutoReturn) {
		if (timeGetTime() > m_autoReturnTime) {
			g_windowMgr.SetModalResult(0x79);
			m_isAutoReturn = 0;
			return;
		}

		prevRemainSecond = m_remainSecond;
		m_remainSecond = ((m_autoReturnTime - timeGetTime()) % 1000 != 0) + (m_autoReturnTime - timeGetTime()) / 1000;
		if (prevRemainSecond != m_remainSecond) {
			sprintf(text, m_countMsg.c_str(), m_remainSecond);
			m_msg = text;
			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)m_msg.c_str(), 0x26, 0, 0);
		}

		m_barGraph->SetAmount((float)(((timeGetTime() + m_maxSecond * 1000) * 100 - m_autoReturnTime * 100) / (m_maxSecond * 1000)));
	}
}

void UIMessageBoxAutoreturn::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\win_msgbox.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
}

int UIMessageBoxAutoreturn::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		g_windowMgr.SetModalResult(val1);
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UIMessageBox::UIMessageBox()
{
	m_isAutoReturn = 0;
}


UIMessageBox::~UIMessageBox()
{
}

void UIMessageBox::Create(char const *msg, int type, int isDefYes, unsigned long autoReturnTime)
{
	m_msg = msg;
	m_countMsg = msg;
	m_openTime = timeGetTime();
	m_autoReturnTime = timeGetTime() + autoReturnTime;
	m_remainSecond = (m_autoReturnTime - m_openTime) / 1000 + ((m_autoReturnTime - m_openTime) % 1000 != 0);
	if (autoReturnTime) {
		m_isAutoReturn = TRUE;
		char text[512];
		sprintf(text, m_countMsg.c_str(), m_remainSecond);
		m_msg = text;
	} else {
		m_isAutoReturn = FALSE;
	}

	int cx = 280;
	int cy = 120;

	UIWindow::Create(cx, cy);
	OnCreate(cx, cy);
	OnSize(cx, cy);

	Move((g_renderer->m_width - 640) / 2 + 185, (g_renderer->m_height - 480) / 2 + 180);

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 22, 260, 65, false);
	unsigned long rr, gg, bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);
	m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)m_msg.c_str(), 0x26, 0, 0);

	if (type == 2)
		m_defPushId = 0x7a;
	else
		m_defPushId = 0x77;

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char *buttonName[][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b", },
		{ "btn_ok", "btn_ok_a", "btn_ok_b", },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b", },
	};
	int ids[] = { ID_OK, ID_YES, ID_NO, };
	int pos[][2] = {
		{ 189, 96 },
		{ 189, 96 },
		{ 234, 96 },
	};

	switch (type) {
	case 4:
		{
			int index = 2;
			UIBitmapButton *button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), 0);
			button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), 1);
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), 2);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ID_CANCEL);
			AddChild(button);
			m_defPushId = 0x77;
		}
		break;

	case UM_DEFPUSH:
		{
			for (int index = 0; index < 1; index++) {
				UIBitmapButton *button = new UIBitmapButton;
				button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), 0);
				button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), 1);
				button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), 2);
				button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
				button->Move(pos[index][0], pos[index][1]);
				button->SetId(ids[index]);
				AddChild(button);
			}
			m_defPushId = ids[0];
		}
		break;

	case UM_TOGGLEPUSH:
		{
			for (int index = 1; index < 3; index++) {
				UIBitmapButton *button = new UIBitmapButton;
				button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), 0);
				button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), 1);
				button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), 2);
				button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
				button->Move(pos[index][0], pos[index][1]);
				button->SetId(ids[index]);
				AddChild(button);
			}
			if (isDefYes)
				m_defPushId = ids[1];
			else
				m_defPushId = ids[2];
			m_defCancelPushId = ids[2];
		}
		break;
	}
}

void UIMessageBox::OnProcess()
{
	int prevRemainSecond;
	char text[512];

	if (m_isAutoReturn) {
		if (timeGetTime() > m_autoReturnTime) {
			g_windowMgr.SetModalResult(0x79);
			m_isAutoReturn = 0;
			return;
		}

		prevRemainSecond = m_remainSecond;
		m_remainSecond = ((m_autoReturnTime - timeGetTime()) % 1000 != 0) + (m_autoReturnTime - timeGetTime()) / 1000;
		if (prevRemainSecond != m_remainSecond) {
			sprintf(text, m_countMsg.c_str(), m_remainSecond);
			m_msg = text;
			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)m_msg.c_str(), 0x26, 0, 0);
		}
	}
}

int UIMessageBox::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETCOUNTMSG:
		m_countMsg = (char *)val1;
		break;
	case UM_CHANGEMSG:
		m_textViewer->ClearAllItems();
		m_textViewer->SendMsg(NULL, UM_SETTEXT, val1, 0x26, 0, 0);
		break;
	case UM_COMMAND:
		g_windowMgr.SetModalResult(val1);
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

void UIMessageBox::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\win_msgbox.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
}

void UIMessageBox::OnDestroy() {}

UISelectServerWnd::UISelectServerWnd()
{
	m_serverList = NULL;
	m_exitButton = NULL;
}




UISelectServerWnd::~UISelectServerWnd()
{
}

void UISelectServerWnd::OnCreate(int cx, int cy)
{
	int listWidth = cx - 24;
	int listHeight = cy - 55;

	m_serverList = new UIServerListBox;
	m_serverList->Create2(12, 22, listWidth, listHeight, false);
	m_serverList->m_bR = 0xf0;
	m_serverList->m_bG = 0xf0;
	m_serverList->m_bB = 0xf0;
	m_serverList->SetId(ID_OK);
	AddChild(m_serverList);

	std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

	char* buttonName[3][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
		{ "login_interface\\btn_exit", "login_interface\\btn_exit_a", "login_interface\\btn_exit_b" },
	};
	int ids[3] = { 0x76, 0x77, 0x9b };
	int pos[3][2] = {
		{ 189, cy - 24 },
		{ 234, cy - 24 },
		{ 434, cy - 24 },
	};

	for (int i = 0; i < 3; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), 0);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), 1);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), 2);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
		if (ids[i] == 0x9b) m_exitButton = button;
	}

	m_defPushId = 0x76;
}

void UISelectServerWnd::SaveCurItemToReg(const char* regkey)
{
	HKEY hKey;
	DWORD dwDisposition;

	m_defaultCurItem = m_serverList->GetSelected();
	dwDisposition = 0;
	LRESULT lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, g_regPath, 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, &dwDisposition);
	if (lResult == ERROR_SUCCESS) {
		RegSetValueEx(hKey, regkey, 0, REG_DWORD, (LPBYTE)&m_defaultCurItem, 4);
		RegCloseKey(hKey);
	}
}

void UISelectServerWnd::InitDefaultCurItemFromReg(const char* regkey)
{
	HKEY hResultKey;
	HKEY hKey;
	DWORD REG_TYPE;
	DWORD DATALENGTH;

	m_defaultCurItem = g_serviceType;
	LRESULT lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_regPath, 0, KEY_QUERY_VALUE, &hResultKey);
	if (lResult == ERROR_SUCCESS) {
		RegCloseKey(hResultKey);
		LRESULT lResult1 = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_regPath, 0, KEY_QUERY_VALUE, &hKey);
		if (lResult1 == ERROR_SUCCESS) {
			REG_TYPE = REG_DWORD;
			DATALENGTH = 4;
			RegQueryValueEx(hKey, regkey, NULL, &REG_TYPE, (LPBYTE)&m_defaultCurItem, &DATALENGTH);
			RegCloseKey(hKey);
		}
	}
}

void UISelectServerWnd::OnDraw()
{
	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\win_service.bmp"), false);

	static char* serviceBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\win_service_ph.bmp";
	if (g_serviceType == ServicePhilippine) {
		if (g_resMgr().IsExist(UIBmp(serviceBmpName))) {
			bitmap = (CBitmapRes*)g_resMgr().Get(UIBmp(serviceBmpName), false);
		}
	}
	DrawBitmap(0, 0, bitmap, false);

	DWORD rr;
	DWORD gg;
	DWORD bb;
	int i;
	g_skinMgr.GetColorChipColor(14, 6, rr, gg, bb);
	DrawBox(0, 17, m_w, m_h - 17, RGB(rr, gg, bb));
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	DrawBox(1, 17, m_w - 2, m_h - 17, RGB(rr, gg, bb));

	bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bitmap->m_height, bitmap, false);

	bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bitmap->m_width, m_h - bitmap->m_height, bitmap, false);

	int loopCnt = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (i = 0; i < loopCnt; i++) {
		bitmap = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bitmap->m_height, bitmap, false);
	}
}

int UISelectServerWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETCURITEM:
		m_serverList->SendMsg(NULL, UM_SETCURITEM, m_defaultCurItem, 0, 0, 0);
		return 0;
	case UM_UPPUSH:
		m_serverList->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return 0;
	case UM_DOWNPUSH:
		m_serverList->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return 0;
	case UM_SET_TARGET:
	{
		m_target = val1;
		m_regKey = (const char*)val2;
		InitDefaultCurItemFromReg(m_regKey.c_str());
		return 0;
	}
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			if (m_serverList->GetSelected() != -1) {
				g_windowMgr.PostQuit(this);
				g_modeMgr.GetCurMode()->SendMsg(m_target, m_serverList->GetSelected(), 0, 0);
				PlayWave("\xb9\xf6\xc6\xb0\xbc\xd2\xb8\xae.wav", 0, 0, 0, 250, 40, 1.0f);
				SaveCurItemToReg(m_regKey.c_str());
			}
			return 0;
		case ID_CANCEL:
			g_windowMgr.PostQuit(this);
			g_modeMgr.GetCurMode()->SendMsg(LMM_CANCEL, 0, 0, 0);
			return 0;
		case ID_EXIT:
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_DO_YOU_REALLY_WANT_TO_QUIT), ME_YESNO, 1, 0, 0) == ID_YES) {
				g_modeMgr.GetCurMode()->SendMsg(MM_QUIT, 0, 0, 0);
				g_windowMgr.PostQuit(this);
			}
			return 0;
		default:
			return 0;
		}
	case UM_ENABLE_EXIT:
		m_exitButton->Move(234, m_exitButton->GetY());
		return 0;
	default:
		break;
	}

	return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
}

void UISelectServerWnd::OnDestroy() {}

UILoginWnd::UILoginWnd()
{
	m_login = NULL;
	m_password = NULL;
	m_cancelButton = NULL;
	m_isCheckOn = 0;
	m_isHangame = 0;
	m_classText = NULL;
	m_isGravity = 1;
}


UILoginWnd::~UILoginWnd()
{
}

void UILoginWnd::WriteToReg()
{
	HKEY hKey;
	DWORD dwDisposition = 0;
	DWORD bitwise;
	DWORD checkSave;
	char passwordName[26];
	char idName[26];

	LRESULT lResult = RegCreateKeyExA(HKEY_LOCAL_MACHINE, g_regPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult == ERROR_SUCCESS)
	{
		if (m_isCheckOn)
		{
			strcpy(idName, m_login->GetText());
			strcpy(passwordName, "");
		}
		else
		{
			memset(idName, 0, sizeof(idName));
		}

		idName[24] = 0;
		checkSave = m_isCheckOn;
		if (checkSave)
			checkSave = 1;

		RegSetValueExA(hKey, "ID", 0, REG_SZ, (LPBYTE)idName, 25);
		RegSetValueExA(hKey, "CHECKSAVE", 0, REG_DWORD, (LPBYTE)&checkSave, 4);
		bitwise = 1;
		RegSetValueExA(hKey, "BITWISE", 0, REG_DWORD, (LPBYTE)&bitwise, 4);
		RegCloseKey(hKey);
	}
}

void UILoginWnd::OnCreate(int cx, int cy)
{
	std::string pathUI("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\";
	char msg[256];
	{
		char* buttonName[5][3] = {
			{ "btn_connect", "btn_connect_a", "btn_connect_b" },
			{ "btn_exit", "btn_exit_a", "btn_exit_b" },
			{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
			{ "btn_request", "btn_request_a", "btn_request_b" },
			{ "btn_intro", "btn_intro_a", "btn_intro_b" }
		};
		int ids[5] = { ID_LOGIN, ID_EXIT, ID_CANCEL, ID_MAKEACCOUNT, ID_INTRO };
		const char* toolTip[5];
		int pos[5][2] = {
			{ 189, 96 },
			{ 234, 96 },
			{ 434, 96 },
			{ 4, 96 },
			{ 137, 96 }
		};
		toolTip[0] = "";
		toolTip[1] = "";
		toolTip[2] = "";
		toolTip[3] = MsgStr(MSI_MAKEACCOUNT);
		toolTip[4] = "";
		int buttonCount = 4;

		if (CFile::IsFileExist("..\\openning.bik")) buttonCount = 5;

		for (int i = 0; i < buttonCount; ++i) {
			if (i == 3 && g_serviceType != ServiceKorea && g_serviceType != (ServiceType)3 &&
				g_serviceType != (ServiceType)10 && g_serviceType != (ServiceType)11 &&
				g_serviceType != (ServiceType)13 && g_serviceType != (ServiceType)14)
				break;

			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName(((i != 2 ? pathName : pathUI) + buttonName[i][0] + ".bmp").c_str(), 0);
			button->SetBitmapName(((i != 2 ? pathName : pathUI) + buttonName[i][1] + ".bmp").c_str(), 1);
			button->SetBitmapName(((i != 2 ? pathName : pathUI) + buttonName[i][2] + ".bmp").c_str(), 2);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			button->SetToolTip(toolTip[i]);
			AddChild(button);
			if (ids[i] == 0x77) m_cancelButton = button;
		}
	}

	m_defPushId = 0x78;

	m_password = new UIEditCtrl;
	m_password->Create(100, 16);
	m_password->SetTextLimit(24);
	m_password->Move(92, 62);
	m_password->SetMaskChar('*');
	m_password->SetFrameColor(0xf2, 0xf2, 0xf2);
	AddChild(m_password);

	m_login = new UIEditCtrl;
	m_login->Create(100, 16);
	m_login->SetTextLimit(24);
	m_login->Move(92, 30);
	m_login->SetFrameColor(0xf2, 0xf2, 0xf2);
	AddChild(m_login);

	InitFromReg();
	m_password->SetText("");

	UICheckBox* checkBox = new UICheckBox;
	std::string on = pathName + "chk_saveon.bmp";
	std::string off = pathName + "chk_saveoff.bmp";
	checkBox->SetBitmap(on.c_str(), off.c_str());
	checkBox->Create(checkBox->GetWidth(), checkBox->GetHeight());
	checkBox->Move(232, 33);
	checkBox->SetCheck(m_isCheckOn);
	AddChild(checkBox);

	if (g_serviceType == ServiceKorea) {
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\";
		char* buttonName[2][3] = {
			{ "win_transbtn", "win_transbtn", "win_transbtn" },
			{ "win_transbtn", "win_transbtn", "win_transbtn" }
		};
		int ids[2] = { ID_B_GRAVITY, ID_B_HANGAME };
		int pos[2][2] = {
			{ 1, 1 },
			{ 141, 1 }
		};

		for (int i = 0; i < 2; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), 0);
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), 1);
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), 2);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			AddChild(button);
		}
	}

	if (m_login->GetTextSize())
		g_windowMgr.SetFocusEdit(m_password);
	else
		g_windowMgr.SetFocusEdit(m_login);

	sprintf(msg, "%d", 4);
}

void UILoginWnd::InitFromReg()
{
	HKEY hResultKey;
	HKEY hKey;
	DWORD DATALENGTH;
	DWORD REG_TYPE;
	DWORD CHECKSAVE;
	DWORD bitwise;
	char passwordName[26];
	char idName[26];

	LRESULT lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, g_regPath, 0, 1, &hResultKey);
	if (lResult == 0)
	{
		RegCloseKey(hResultKey);

		LRESULT lResult1 = RegOpenKeyExA(HKEY_LOCAL_MACHINE, g_regPath, 0, 1, &hKey);
		if (lResult1 != 0)
			return;

		REG_TYPE = 4;
		DATALENGTH = 4;
		RegQueryValueExA(hKey, "CHECKSAVE", 0, &REG_TYPE, (LPBYTE)&CHECKSAVE, &DATALENGTH);
		if (CHECKSAVE != 1)
			CHECKSAVE = 0;
		RegQueryValueExA(hKey, "BITWISE", 0, &REG_TYPE, (LPBYTE)&bitwise, &DATALENGTH);

		memset(idName, 0, sizeof(idName));
		memset(passwordName, 0, sizeof(passwordName));

		REG_TYPE = 1;
		DATALENGTH = 25;
		RegQueryValueExA(hKey, "PASSWORD", 0, &REG_TYPE, (LPBYTE)passwordName, &DATALENGTH);
		passwordName[24] = 0;
		REG_TYPE = 1;
		DATALENGTH = 25;
		RegQueryValueExA(hKey, "ID", 0, &REG_TYPE, (LPBYTE)idName, &DATALENGTH);
		idName[24] = 0;
		passwordName[24] = 0;

		if (m_isCheckOn = CHECKSAVE)
		{
			m_login->SetText(idName);
			m_password->SetText("");
		}
		RegCloseKey(hKey);
		return;
	}
	m_login->SetText("");
	m_password->SetText("");
}

void UILoginWnd::StoreInfo()
{
	WriteToReg();
}

void UILoginWnd::OnDestroy() {}

void UILoginWnd::OpenRegistPage()
{
	ShellExecuteA(g_hMainWnd, "open", g_regstrationWeb, NULL, NULL, SW_SHOWNORMAL);
}

int UILoginWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_MAKEACCOUNT:
			if (g_serviceType == ServiceKorea) {
				if (!g_isGravityID) {
					if (g_windowMgr.ErrorMsg(MsgStr(MSI_3DAY_FREE), ME_YESNO, 1, 0, 0) == ID_YES) {
						g_modeMgr.Quit();
						ShellExecuteA(g_hMainWnd, "open", g_regstrationWeb, NULL, NULL, SW_SHOWNORMAL);
					}
					return 0;
				}
				g_modeMgr.Quit();
				ShellExecuteA(g_hMainWnd, "open",
					"http://ro.hangame.com/login/loginstep.asp?prevURL=/NHNCommon/NHN/Memberjoin.asp",
					NULL, NULL, SW_SHOWNORMAL);
				return 0;
			}
			g_modeMgr.Quit();
			ShellExecuteA(g_hMainWnd, "open", g_regstrationWeb, NULL, NULL, SW_SHOWNORMAL);
			return 0;
		case ID_LOGIN: {
			PlayWave("\xb9\xf6\xc6\xb0\xbc\xd2\xb8\xae.wav", 0.0f, 0.0f, 0.0f, 0xfa, 0x28, 1.0f);
			g_modeMgr.GetCurMode()->SendMsg(LMM_PASSWORD, (int)m_password->GetText(), 0, 0);
			g_modeMgr.GetCurMode()->SendMsg(LMM_ID, (int)m_login->GetText(), 0, 0);
			g_modeMgr.GetCurMode()->SendMsg(LMM_CONNECT_TO_ACSVR, 0, 0, 0);
			g_windowMgr.PostQuit(this);
			return 0;
		}
		case ID_CANCEL:
			g_modeMgr.GetCurMode()->SendMsg(LMM_GOTOSELECTACCOUNT, 0, 0, 0);
			return 0;
		case ID_EXIT:
			WriteToReg();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_DO_YOU_REALLY_WANT_TO_QUIT), ME_YESNO, 1, 0, 0) == ID_YES) {
				g_modeMgr.GetCurMode()->SendMsg(MM_QUIT, 0, 0, 0);
				g_windowMgr.PostQuit(this);
			}
			return 0;
		case ID_CHECK:
			m_isCheckOn = val2;
			WriteToReg();
			return 0;
		case ID_INTRO:
			g_Bink.OpenningMain(0);
			return 0;
		case ID_CLASSCOMBO: {
			if (g_windowMgr.GetComboBoxWnd())
				g_windowMgr.DeleteWindow(WID_COMBOBOXWND);

			UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
			m_comboMode = 1;
			int comboCnt = (int)m_classStringList.size();
			for (int i = 0; i < m_classStringList.size(); ++i)
				comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)m_classStringList[i].c_str(), 0, 0, 0);

			int height = min(comboCnt * 16, 48);
			comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			comboBoxWnd->Resize(74, height);
			comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_LOGINWND, 0, 0, 0);
			int gx = 60;
			int gy = 96;
			GetGlobalCoor(gx, gy);
			comboBoxWnd->Move(gx + 1, gy);
			return 0;
		}
		case ID_B_GRAVITY:
			g_isGravityID = 0;
			Invalidate();
			return 0;
		case ID_B_HANGAME:
			g_isGravityID = 1;
			Invalidate();
			return 0;
		}
		break;
	case UM_ENABLE_CANCEL:
		m_cancelButton->Move(234, 96);
		break;
	case UM_COMBOBOX_SELECTED:
		switch (m_comboMode) {
		case 1:
			m_classText->SetText(m_classStringList[val1].c_str(), 0);
			g_isGravityID = val1;
			break;
		}
		Invalidate();
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UILoginWnd::OnDraw()
{
	CBitmapRes* bmp;
	if (g_serviceType == 0) {
		if (!g_isGravityID) {
			bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\win_login_K1.bmp"), false);
		} else {
			bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\win_login_K2.bmp"), false);
		}
	} else {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\win_login.bmp"), false);
	}
	DrawBitmap(0, 0, bmp, false);
}

UINoticeWnd::UINoticeWnd()
{
	for (int i = 0; i < 19; ++i)
		m_NoticeMessage[i] = new UIStaticText;

	m_NoticeTotalLine = 20;
	m_NoticeDisplayLine = 0;
	m_TotalPage = 0;
	m_PresentPage = 0;
	m_PageStartLine[0] = 0;
	m_PageLastLine[0] = 0;
	m_PagePresentLine = 0;
	g_windowMgr.SetFocusEdit(this);
}




UINoticeWnd::~UINoticeWnd()
{
}

void UINoticeWnd::OnCreate(int cx, int cy)
{
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[3][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_back", "btn_back_a", "btn_back_b" },
		{ "btn_next", "btn_next_a", "btn_next_b" }
	};
	int ids[3] = { ID_OK, ID_PREV, ID_NEXT };
	int pos[3][2] = { { 534, 376 }, { 244, 376 }, { 294, 376 } };
	int same_file_check = 0;
	int fileSize;
	int lineStart[21];
	int fileSize2;
	int count;
	int i;
	int j;
	FILE* file;
	FILE* file2;
	char* fileBuf1;
	char* fileBuf2;

	for (j = 0; j < 3; ++j) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[j][0] + ".bmp").c_str(), 0);
		button->SetBitmapName((pathName + buttonName[j][1] + ".bmp").c_str(), 1);
		button->SetBitmapName((pathName + buttonName[j][2] + ".bmp").c_str(), 2);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[j][0], pos[j][1]);
		button->SetId(ids[j]);
		AddChild(button);
	}

	m_defPushId = 0x76;
	m_NoticeFileName = "notice_patch.txt";

	if (!(file = fopen("notice_patch.txt", "r"))) {
		file = fopen(m_NoticeFileName, "w");
		fwrite("\xb0\xf8\xc1\xf6\xbb\xe7\xc7\xd7\xc6\xc4\xc0\xcf\xbe\xf8\xc0\xbd", 16, 1, file);
		fclose(file);
		g_windowMgr.PostQuit(this);
	}

	if (!(file = fopen("notice_check.txt", "r"))) {
		file = fopen("notice_check.txt", "w");
		fwrite("\xbe\xf8\xc0\xb8\xb4\xcf\xb8\xb8\xb5\xe9\xc0\xda", 12, 1, file);
		fclose(file);
	}

	if (!(file = fopen("notice_read.txt", "r"))) {
		file = fopen("notice_read.txt", "wb");
		unsigned char a = 0;
		fwrite(&a, 1, 1, file);
		fclose(file);
	}

	file = fopen(m_NoticeFileName, "r");
	file2 = fopen("notice_check.txt", "r");
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file2, 0, SEEK_END);
	fileSize2 = ftell(file2);
	if (!fileSize2)
		same_file_check = 1000;

	fileBuf1 = new char[fileSize + 1];
	fileBuf2 = new char[fileSize2 + 1];
	fseek(file, 0, SEEK_SET);
	fread(fileBuf1, fileSize, 1, file);
	fileBuf1[fileSize] = 0;
	fclose(file);
	fseek(file2, 0, SEEK_SET);
	fread(fileBuf2, fileSize2, 1, file2);
	fileBuf2[fileSize2] = 0;
	fclose(file2);

	lineStart[0] = 0;
	count = 1;
	m_NoticeTotalLine = 0;
	for (i = 0; i < fileSize; ++i) {
		switch (fileBuf1[i]) {
		case '\n':
			fileBuf1[i] = 0;
			lineStart[count++] = i + 1;
			if (count > 20)
				count = 20;
			m_NoticeTotalLine++;
			break;
		case '\r':
			fileBuf1[i] = 0;
			break;
		case '^':
			count--;
			if (count < 0)
				count = 0;
			m_PageLastLine[m_TotalPage] = m_NoticeTotalLine - 1;
			m_TotalPage++;
			m_PageStartLine[m_TotalPage] = m_NoticeTotalLine + 1;
			break;
		default:
			if (i < fileSize2)
				if (i < 1000)
					if (fileBuf1[i] != fileBuf2[i])
						same_file_check++;
			break;
		}
	}
	if (count > 19)
		count = 19;

	m_PageLastLine[m_TotalPage] = m_NoticeTotalLine;

	if (!same_file_check) {
		unsigned char r;
		file = fopen("notice_read.txt", "rb");
		fread(&r, 1, 1, file);
		fclose(file);
		if (r == 0xff) {
			g_windowMgr.PostQuit(this);
		}
	}
	else {
		file = fopen("notice_read.txt", "wb");
		unsigned char a = 0;
		fwrite(&a, 1, 1, file);
		fclose(file);
	}

	for (j = 0; j < 19; ++j) {
		if (j < count) {
			m_NoticeMessage[j]->Create(540, 16);
			m_NoticeMessage[j]->SetFont(0, 16);

			m_NoticeMessage[j]->SetText(fileBuf1 + lineStart[j], 1);

			m_NoticeMessage[j]->Move(30, 22 + j * 18);
			AddChild(m_NoticeMessage[j]);
		} else {
			m_NoticeMessage[j]->Create(540, 16);
			m_NoticeMessage[j]->SetFont(0, 16);

			m_NoticeMessage[j]->SetText("", 1);

			m_NoticeMessage[j]->Move(30, 22 + j * 18);
			AddChild(m_NoticeMessage[j]);
		}
	}

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(583, 18, 1, m_h - 48);
	m_scrollBar->SetRange(m_PageLastLine[m_PresentPage] - m_PageStartLine[m_PresentPage] - 19, 19, m_PageLastLine[m_PresentPage] - m_PageStartLine[m_PresentPage]);
	if (m_PageLastLine[m_PresentPage] - m_PageStartLine[m_PresentPage] < 19)
		m_scrollBar->SetRange(0, 19, 19);
	m_scrollBar->SetPos(m_PagePresentLine);
	AddChild(m_scrollBar);

	file = fopen(m_NoticeFileName, "r");
	file2 = fopen("notice_check.txt", "w");
	fseek(file, 0, SEEK_END);
	fseek(file2, 0, SEEK_END);
	fileSize = ftell(file);
	fileBuf1 = new char[fileSize + 1];
	fseek(file, 0, SEEK_SET);
	fread(fileBuf1, fileSize, 1, file);
	fileBuf1[fileSize] = 0;
	fclose(file);
	for (i = 0; i < fileSize; i++) {
		fwrite(fileBuf1, 1, 1, file2);
		fileBuf1++;
	}
	fclose(file2);

	if (!same_file_check) {
		unsigned char r;
		file = fopen("notice_read.txt", "rb");
		fread(&r, 1, 1, file);
		fclose(file);
		if (r != 0xff)
			m_PagePresentLine = r & 0xff;
		if (m_PagePresentLine > m_PageLastLine[m_PresentPage] - 19)
			m_PagePresentLine = m_PageLastLine[m_PresentPage] - 19;
		if (m_PagePresentLine < 0)
			m_PagePresentLine = 0;
		PrintNotice();
	}
}

void UINoticeWnd::OnWheel(int wheel)
{
	if (wheel < 0) {
		SendMsg(m_scrollBar, UM_SCROLLVERT, 6, 0, 0, 0);
	} else {
		SendMsg(m_scrollBar, UM_SCROLLVERT, -6, 0, 0, 0);
	}
}

void UINoticeWnd::OnDraw()
{
	DWORD rr;
	DWORD gg;
	DWORD bb;
	int i;

	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bitmap, false);

	bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bitmap->m_width, 0, bitmap, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (i = 0; i < titleMidCount; i++) {
		bitmap = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(12 + i * 12, 0, bitmap, false);
	}

	if (m_h != 17) {
		int sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
		for (i = 0; i < sideCount; i++) {
			bitmap = (CBitmapRes*)g_resMgr().Get(
				UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
			DrawBitmap(0, 17 + i * 8, bitmap, false);
		}

		sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
		for (i = 0; i < sideCount; i++) {
			bitmap = (CBitmapRes*)g_resMgr().Get(
				UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
			DrawBitmap(m_w - bitmap->m_width, 17 + i * 8, bitmap, false);
		}

		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17, m_w - 40, m_h - 38, RGB(rr, gg, bb));
		DrawBox(20, (m_h - 38) / 32 * 32 + 17, m_w - 24, (m_h - 38) % 32, RGB(rr, gg, bb));

		bitmap = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
		DrawBitmap(0, m_h - bitmap->m_height, bitmap, false);

		bitmap = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
		DrawBitmap(m_w - bitmap->m_width, m_h - bitmap->m_height, bitmap, false);

		int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
		for (i = 0; i < btnMidCount; i++) {
			bitmap = (CBitmapRes*)g_resMgr().Get(
				UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
			DrawBitmap((i + 1) * 21, m_h - bitmap->m_height, bitmap, false);
		}
	}
}

void UINoticeWnd::OnDestroy() {}

void UINoticeWnd::OnMouseHover(int x, int y) {}

int UINoticeWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_DEFPUSH:
		SendMsg(NULL, UM_COMMAND, 0x76, 0, 0, 0);
		return 0;
	case UM_COMMAND:
		switch (val1) {
		case ID_PREV:
			m_PresentPage--;
			if (m_PresentPage < 0) {
				m_PresentPage = 0;
				return 0;
			}
			m_PagePresentLine = m_PageStartLine[m_PresentPage];
			PrintNotice();
			return 0;
		case ID_NEXT: {
			int page = ++m_PresentPage;
			if (page > m_TotalPage) {
				m_PresentPage = m_TotalPage;
				return 0;
			}
			m_PagePresentLine = m_PageStartLine[page];
			PrintNotice();
			return 0;
		}
		case ID_OK: {
			PlayWave("\xb9\xf6\xc6\xb0\xbc\xd2\xb8\xae.wav", 0, 0, 0, 250, 40, 1.0f);
			FILE* file3 = fopen("notice_read.txt", "wb");
			unsigned char a;
			if (m_PresentPage <= 0 && m_PagePresentLine <= 0xff) {
				a = (unsigned char)m_PagePresentLine;
				if ((a & 0xff) == m_PageLastLine[m_PresentPage] - 19) {
					a = 0xff;
				}
				else if (a != 0xff) {
					a += 19;
					if (m_PagePresentLine + 19 > m_PageLastLine[m_PresentPage] - 19) {
						if (m_PageLastLine[m_PresentPage] - 19 < 0)
							a = 0;
						else
							a = (unsigned char)(m_PageLastLine[m_PresentPage] - 19);
					}
				}
			}
			else {
				a = 0xff;
			}
			fwrite(&a, 1, 1, file3);
			fclose(file3);
			g_windowMgr.PostQuit(this);
			return 0;
		}
		}
		break;
	case UM_SCROLLVERT:
	case UM_SCROLLVERTPAGEUP:
	case UM_SCROLLVERTPAGEDOWN: {
		int oldLine = m_PagePresentLine;
		if (message == 9) {
			m_PagePresentLine = oldLine - 1;
		}
		else if (message == 10) {
			m_PagePresentLine = oldLine + 1;
		}
		else {
			m_PagePresentLine = oldLine + val1;
		}

		if (m_PagePresentLine > m_PageLastLine[m_PresentPage] - 19)
			m_PagePresentLine = m_PageLastLine[m_PresentPage] - 19;

		if (m_PagePresentLine < m_PageStartLine[m_PresentPage])
			m_PagePresentLine = m_PageStartLine[m_PresentPage];

		PrintNotice();
		return m_PagePresentLine - oldLine;
	}
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UINoticeWnd::PrintNotice()
{
	int lineStart[9999];
	char* fileBuf;

	m_scrollBar->SetRange(m_PageLastLine[m_PresentPage] - m_PageStartLine[m_PresentPage] - 19, 19,
		m_PageLastLine[m_PresentPage] - m_PageStartLine[m_PresentPage]);
	if (m_PageLastLine[m_PresentPage] - m_PageStartLine[m_PresentPage] < 19)
		m_scrollBar->SetRange(0, 19, 19);
	m_scrollBar->SetPos(m_PagePresentLine - m_PageStartLine[m_PresentPage]);
	m_scrollBar->Invalidate();

	FILE* file = fopen(m_NoticeFileName, "r");
	fseek(file, 0, SEEK_END);
	int fileLen = ftell(file);
	fileBuf = (char*)::operator new(fileLen + 1);
	fseek(file, 0, SEEK_SET);
	fread(fileBuf, fileLen, 1, file);
	fileBuf[fileLen] = 0;
	fclose(file);

	memset(lineStart, 0, 9999);
	int mark = 0;
	int count = 1;
	lineStart[0] = 0;
	if (fileLen > 0) {
		int lineBase = count - (int)fileBuf;
		for (char* p = fileBuf; fileLen > 0; ++p, --fileLen) {
			char ch = *p;
			if (ch != '\n') {
				if (ch != '\r') {
					if (ch == '^') {
						if (!mark) {
							if (count > m_PageStartLine[m_PresentPage]) {
								mark = count - 1;
								if (count < 0) {
									count = 0;
								}
							}
						}
					}
				}
				else {
					*p = 0;
				}
			}
			else if (!mark) {
				*p = 0;
				lineStart[count] = lineBase + (int)p;
				count++;
			}
		}
	}

	if (mark > 0)
		count = mark - m_PageStartLine[m_PresentPage];

	count--;
	if (count > 19)
		count = 19;

	for (int i = 0; i < 19; ++i) {
		if (i < count)
			m_NoticeMessage[i]->SetText(fileBuf + lineStart[m_PagePresentLine + i], 1);
		else
			m_NoticeMessage[i]->SetText("", 0);
	}
	Invalidate();
}





UISelectCharWnd::UISelectCharWnd()
{
	m_stateCnt = 0;
	m_okButton = NULL;
	m_makeButton = NULL;
	m_cancelButton = NULL;
	m_deleteButton = NULL;
	m_chargeButton = NULL;
	m_noticeButton = NULL;
	m_dontmove = 0;

	if (g_extended_slot)
	{
		m_pageCount = 3;
		if (g_serviceType == ServiceBrazil)
			m_pageCount = 4;
		else if (g_serviceType == ServiceFrance)
			m_pageCount = 2;
	}
	else
	{
		m_pageCount = 1;
	}

	for (int i = 0; i < m_pageCount * 3; ++i)
	{
		m_isEmpty[i] = 1;
		m_isAvailable[i] = 0;
		if (i < 9)
			m_isAvailable[i] = 1;
	}

	m_curSlot = 0;
	m_curPage = 0;
	m_stateStartTick = timeGetTime();
	m_changeNameBtn = NULL;
}


UISelectCharWnd::~UISelectCharWnd()
{
}

void UISelectCharWnd::InitTextControls()
{
	CHARACTER_INFO* charInfo = (CHARACTER_INFO*)g_modeMgr.GetCurMode()->SendMsg(MM_QUERYCHARICTORINFO, m_curPage * 3 + m_curSlot, 0, 0);
	if (m_changeNameBtn)
		m_changeNameBtn->Move(-100, -100);
	if (charInfo)
	{
		if (m_changeNameBtn) {
			if (charInfo->bIsChangedCharName == 0)
				m_changeNameBtn->Move(360, 318);
		}

		char nameBuf[25];
		memcpy(nameBuf, charInfo->name, 24);
		nameBuf[24] = 0;
		char buf[100];
		m_text[0]->SetTextWithShorten((const char*)nameBuf);
		m_text[1]->SetTextWithShorten(g_session.m_jobNameTable[charInfo->job]);
		sprintf(buf, "%d", charInfo->level);
		m_text[2]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->exp);
		m_text[3]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->hp);
		m_text[4]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->sp);
		m_text[5]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->Str);
		m_text[6]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->Agi);
		m_text[7]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->Vit);
		m_text[8]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->Int);
		m_text[9]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->Dex);
		m_text[10]->SetTextWithShorten(buf);
		sprintf(buf, "%d", charInfo->Luk);
		m_text[11]->SetTextWithShorten(buf);
		return;
	}
	for (int i = 0; i < 12; ++i)
		m_text[i]->SetText("", 0);
}

void UISelectCharWnd::MakeButton(int id)
{
	int index;

	switch (id) {
	case ID_OK:
		if (m_okButton)
			return;
		if (m_makeButton)
			m_makeButton = NULL;
		index = 0;
		break;
	case ID_MAKE:
		if (m_makeButton)
			return;
		if (m_okButton)
			m_okButton = NULL;
		if (m_deleteButton) {
			RemoveChild(m_deleteButton);
			m_deleteButton = NULL;
		}
		index = 1;
		break;
	case ID_CANCEL:
		if (m_cancelButton)
			return;
		index = 2;
		break;
	case ID_DELETE:
		if (m_deleteButton)
			return;
		if (m_makeButton) {
			RemoveChild(m_makeButton);
			m_makeButton = NULL;
		}
		index = 3;
		break;
	case ID_CHARGE:
		if (m_chargeButton)
			return;
		index = 4;
		break;
	case ID_NOTICE:
		if (m_noticeButton)
			return;
		index = 5;
		break;
	default:
		return;
	}

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[6] = {
		"btn_ok",
		"btn_make",
		"btn_cancel",
		"btn_del",
		"btn_charge",
		"btn_charge",
	};
	int ids[6] = { ID_OK, ID_MAKE, ID_CANCEL, ID_DELETE, ID_CHARGE, ID_NOTICE };

	UIBitmapButton* button = 0;
	button = new UIBitmapButton;
	button->SetBitmapName((pathName + buttonName[index] + ".bmp").c_str(), 0);
	button->SetBitmapName((pathName + buttonName[index] + "_a.bmp").c_str(), 1);
	button->SetBitmapName((pathName + buttonName[index] + "_b.bmp").c_str(), 2);
	button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());

	int x = button->GetBitmapWidth() + 5;
	if (id == 0x91)
		button->Move(5, 318);
	else if (id == 0xda) {
		int bx = 402 - m_cancelButton->GetBitmapWidth() - x;
		button->Move(bx, 318);
	}
	else if (id == 0xdc) {
		int bx = 402 - m_cancelButton->GetBitmapWidth() - x;
		button->Move(bx, 318);
	}
	else {
		button->Move(530 + ((index == 2) - 1) * x, 318);
	}

	button->SetId(ids[index]);
	AddChild(button);

	switch (id) {
	case ID_OK:
		m_okButton = button;
		break;
	case ID_MAKE:
		m_makeButton = button;
		break;
	case ID_CANCEL:
		m_cancelButton = button;
		break;
	case ID_DELETE:
		m_deleteButton = button;
		break;
	case ID_CHARGE:
		m_chargeButton = button;
		break;
	case ID_NOTICE:
		m_noticeButton = button;
		break;
	}
}

void UISelectCharWnd::DeleteButton(int id)
{
	switch (id) {
	case ID_OK:
		if (m_okButton) {
			RemoveChild(m_okButton);
			m_okButton = NULL;
		}
		break;
	case ID_MAKE:
		if (m_makeButton) {
			RemoveChild(m_makeButton);
			m_makeButton = NULL;
		}
		break;
	case ID_DELETE:
		if (m_deleteButton) {
			RemoveChild(m_deleteButton);
			m_deleteButton = NULL;
		}
		break;
	}
}

void UISelectCharWnd::InvalidateWallPaper() {}

void UISelectCharWnd::WriteToReg()
{
	HKEY hKey;
	DWORD dwDisposition = 0;
	DWORD CURSLOT;

	LRESULT lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, g_regPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult == ERROR_SUCCESS) {
		CURSLOT = m_curPage * 3 + m_curSlot;
		RegSetValueEx(hKey, "CURSLOT", 0, REG_DWORD, (LPBYTE)&CURSLOT, 4);
		RegCloseKey(hKey);
	}
}

void UISelectCharWnd::InitFromReg()
{
	HKEY hResultKey;
	HKEY hKey;
	DWORD DATALENGTH;
	DWORD REG_TYPE;
	DWORD CURSLOT;

	LONG ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, g_regPath, 0, 1, &hResultKey);
	m_curPage = 0;
	m_curSlot = 0;
	if (ret != 0)
		return;
	RegCloseKey(hResultKey);

	LONG ret1 = RegOpenKeyExA(HKEY_LOCAL_MACHINE, g_regPath, 0, 1, &hKey);
	if (ret1 != 0)
		return;

	REG_TYPE = 4;
	DATALENGTH = 4;
	RegQueryValueExA(hKey, "CURSLOT", 0, &REG_TYPE, (LPBYTE)&CURSLOT, &DATALENGTH);
	RegCloseKey(hKey);

	m_curPage = CURSLOT / 3;
	m_curSlot = CURSLOT % 3;
	if (m_curPage >= m_pageCount)
		m_curPage = 0;
	if (m_curPage < 0)
		m_curPage = 0;
}

void UISelectCharWnd::OnProcess() {}

void UISelectCharWnd::OnCreate(int cx, int cy)
{
	RAG_SYMBOL_SHIFT(1)
	int m;
	int i;
	int t;

	InitFromReg();
	g_charInfo.CharNum = m_curPage * 3 + m_curSlot;

	int states[3];
	for (i = 0; i < 3; ++i)
		states[i] = (m_curSlot != i);

	for (i = 0; i < 3; ++i) {
		UISlotBitmap* slot = new UISlotBitmap(i);
		slot->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\box_select.bmp");
		slot->Create(slot->GetWidth(), slot->GetHeight());
		slot->Move(i * 163 + 56, 40);
		slot->SetId(ID_SLOT);
		AddChild(slot);
		m_slots[i] = slot;
		slot->SendMsg(this, UM_SETSTATE, states[i], 0, 0, 0);
	}

	if (g_extended_slot) {
		UIBitmapButton* scrollButton = new UIBitmapButton;
		scrollButton->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\scroll1left.bmp", 0);
		scrollButton->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\scroll1left.bmp", 1);
		scrollButton->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\scroll1left.bmp", 2);
		scrollButton->Create(scrollButton->GetBitmapWidth(), scrollButton->GetBitmapHeight());
		scrollButton->Move(44, 110);
		scrollButton->SetId(ID_MINUS);
		AddChild(scrollButton);

		scrollButton = new UIBitmapButton;
		scrollButton->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\scroll1right.bmp", 0);
		scrollButton->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\scroll1right.bmp", 1);
		scrollButton->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\scroll1right.bmp", 2);
		scrollButton->Create(scrollButton->GetBitmapWidth(), scrollButton->GetBitmapHeight());
		scrollButton->Move(520, 110);
		scrollButton->SetId(ID_PLUS);
		AddChild(scrollButton);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\";
		m_changeNameBtn = new UIBitmapButton;
		m_changeNameBtn->SetBitmapName((pathName + "btn_change_name" + ".bmp").c_str(), 0);
		m_changeNameBtn->SetBitmapName((pathName + "btn_change_name_a" + ".bmp").c_str(), 1);
		m_changeNameBtn->SetBitmapName((pathName + "btn_change_name_b" + ".bmp").c_str(), 2);
		m_changeNameBtn->Create(m_changeNameBtn->GetBitmapWidth(), m_changeNameBtn->GetBitmapHeight());
		m_changeNameBtn->Move(-100, -100);
		m_changeNameBtn->SetId(ID_CHANGE_NAME);
		AddChild(m_changeNameBtn);
	}

	for (t = 0; t < 12; ++t) {
		m_text[t] = new UIStaticText;
		m_text[t]->Create(90, 15);
		m_text[t]->SetText("", 0);
		m_text[t]->Move((t / 6) * 144 + 69, (t % 6) * 16 + 206);
		AddChild(m_text[t]);
	}

	InitTextControls();

	for (m = 0; m < m_pageCount * 3; ++m) {
		CHARACTER_INFO* charInfo = (CHARACTER_INFO*)g_modeMgr.GetCurMode()->SendMsg(MM_QUERYCHARICTORINFO, m, 0, 0);
		if (charInfo) {
			sAssert(charInfo->head < HEADTYPE_LAST);

			int slot = charInfo->CharNum;
			m_isEmpty[slot] = 0;

			m_viewChar[slot].x = (slot % 3) * 163 + 124;
			m_viewChar[slot].y = 157;
			m_viewChar[slot].baseAction = 0;
			m_viewChar[slot].curAction = m_viewChar[slot].baseAction;
			m_viewChar[slot].curMotion = 0;

			char tmpName[256]; m_viewChar[slot].x = m_viewChar[slot].x;
			m_viewChar[slot].actName[0] = g_session.GetJobActName(charInfo->job, g_session.GetSex(), tmpName);
			m_viewChar[slot].sprName[0] = g_session.GetJobSprName(charInfo->job, g_session.GetSex(), tmpName);

			int head = charInfo->head;
			m_viewChar[slot].actName[1] = g_session.GetHeadActName(charInfo->job, head, g_session.GetSex(), tmpName);
			m_viewChar[slot].sprName[1] = g_session.GetHeadSprName(charInfo->job, head, g_session.GetSex(), tmpName);

			m_viewChar[slot].actName[2] =
				g_session.GetAccessoryActName(charInfo->job, head, g_session.GetSex(), charInfo->accessory, tmpName);
			m_viewChar[slot].sprName[2] =
				g_session.GetAccessorySprName(charInfo->job, head, g_session.GetSex(), charInfo->accessory, tmpName);
			charInfo->head = head;

			m_viewChar[slot].actName[3] =
				g_session.GetAccessoryActName(charInfo->job, head, g_session.GetSex(), charInfo->accessory2, tmpName);
			m_viewChar[slot].sprName[3] =
				g_session.GetAccessorySprName(charInfo->job, head, g_session.GetSex(), charInfo->accessory2, tmpName);
			m_accessory_top[slot] = charInfo->accessory2;

			m_viewChar[slot].actName[4] =
				g_session.GetAccessoryActName(charInfo->job, head, g_session.GetSex(), charInfo->accessory3, tmpName);
			m_viewChar[slot].sprName[4] =
				g_session.GetAccessorySprName(charInfo->job, head, g_session.GetSex(), charInfo->accessory3, tmpName);

			if (charInfo->job > 4000 && charInfo->job <= 4007)
				m_viewChar[slot].bodyPalette = 1;
			else
				m_viewChar[slot].bodyPalette = charInfo->bodypalette;

			if (m_viewChar[slot].bodyPalette) {
				char palName[64];
				m_viewChar[slot].bodyPaletteName.assign(g_session.GetBodyPaletteName(charInfo->job, g_session.GetSex(), m_viewChar[slot].bodyPalette, palName));
			}

			m_viewChar[slot].headPalette = charInfo->headpalette;

			if (m_viewChar[slot].headPalette) {
				char palName[64];
				m_viewChar[slot].headPaletteName.assign(g_session.GetHeadPaletteName(head, charInfo->job, g_session.GetSex(), m_viewChar[slot].headPalette, palName));
			}

			m_viewChar[slot].imfName.assign(g_session.GetImfName(charInfo->job, charInfo->head, g_session.GetSex(), tmpName));
		}
	}

	MakeButton(0x77);
	if (m_isEmpty[m_curPage * 3 + m_curSlot]) {
		MakeButton(0x89);
	}
	else {
		RAG_BUDGET_FILL(2)
		MakeButton(0x76);
		MakeButton(0x91);
	}
	MakeButton(0xda);
	m_defPushId = 0x76;
}

void UISelectCharWnd::OnMouseMove(int x, int y)
{
	UIFrameWnd::OnMouseMove(x, y);
}

void UISelectCharWnd::OnLBtnDown(int x, int y)
{
	UIFrameWnd::OnLBtnDown(x, y);
}


void UISelectCharWnd::OnDraw()
{
	int j;
	int DoubleAcc;

	DrawBitmap(0, 0, (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\win_select.bmp"), false), false);

	static int head_imfOffX = 0;
	static int head_imfOffY = 0;

	for (j = 0; j < 3; ++j) {
		int curChar = m_curPage * 3 + j;
		if (!m_isAvailable[curChar]) {
			TextOutA(j * 163 + 82, 107, "Not Available", 0, 1, 18, 0);
		} else if (!m_isEmpty[curChar]) {

		CTexture* tex;
		CacheInfo info;
		DoubleAcc = 0;

		for (int i = 0; i < 5; ++i) {
			if (m_viewChar[curChar].actName[i].empty() || m_viewChar[curChar].sprName[i].empty())
				continue;

			g_resMgr().Get(m_viewChar[curChar].imfName.c_str(), false);
			CActRes* actRes = (CActRes*)g_resMgr().Get(m_viewChar[curChar].actName[i].c_str(), false);
			CSprRes* sprRes = (CSprRes*)g_resMgr().Get(m_viewChar[curChar].sprName[i].c_str(), false);

			{
				char accname[256];
				if (DoubleAcc == 1)
					sprintf(accname, "_\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd");
				else if (DoubleAcc == 2)
					sprintf(accname, "_\xb4\xab\xb0\xa1\xb8\xae\xb0\xb3");
				else if (DoubleAcc == 3)
					sprintf(accname, "_\xc7\xef\xb8\xa7");

				if (DoubleAcc > 0) {
					char buf[1024];
					sprintf(buf, "sprite\\\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.act",
						g_session.m_newPcSexNameTable[g_session.GetSex()],
						g_session.m_newPcSexNameTable[g_session.GetSex()], accname);
					actRes = (CActRes*)g_resMgr().Get(buf, false);

					sprintf(buf, "sprite\\\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.spr",
						g_session.m_newPcSexNameTable[g_session.GetSex()],
						g_session.m_newPcSexNameTable[g_session.GetSex()], accname);
					sprRes = (CSprRes*)g_resMgr().Get(buf, false);
					DoubleAcc = 100;
				}
			}

			CMotion* curMotion = actRes->GetMotion(m_viewChar[curChar].curAction, m_viewChar[curChar].curMotion);
			int offX = 0, offY = 0;

			if (i == 1) {
				CMotion* bodyMotion = ((CActRes*)g_resMgr().Get(m_viewChar[curChar].actName[0].c_str(), false))->
					GetMotion(m_viewChar[curChar].curAction, m_viewChar[curChar].curMotion);
				if (curMotion->attachCnt && bodyMotion->attachCnt) {
					if (curMotion->attachInfo[0].m_attr == bodyMotion->attachInfo[0].m_attr) {
						offX += bodyMotion->attachInfo[0].x - curMotion->attachInfo[0].x;
						offY += bodyMotion->attachInfo[0].y - curMotion->attachInfo[0].y;
						head_imfOffX = offX;
						head_imfOffY = offY;
					}
				}
			}
			else if (i == 2 || i == 3 || i == 4) {
				{
					CMotion* accMotion = curMotion;
					CMotion* headMotion = ((CActRes*)g_resMgr().Get(m_viewChar[curChar].actName[1].c_str(), false))->
						GetMotion(m_viewChar[curChar].curAction, m_viewChar[curChar].curMotion);

					CHARACTER_INFO* charInfo = (CHARACTER_INFO*)g_modeMgr.GetCurMode()->SendMsg(MM_QUERYCHARICTORINFO, curChar, 0, 0);
					int accessory = 0;
					if (i == 2)
						accessory = charInfo->accessory;
					else if (i == 3)
						accessory = charInfo->accessory2;
					else if (i == 4)
						accessory = charInfo->accessory3;

					if (g_windowMgr.IsAniAccessory(accessory)) {
						int headMotionCount = ((CActRes*)g_resMgr().Get(m_viewChar[curChar].actName[1].c_str(), false))->
							GetMotionCount(m_viewChar[curChar].curAction);
						int accMotionCount = ((CActRes*)g_resMgr().Get(m_viewChar[curChar].actName[i].c_str(), false))->
							GetMotionCount(m_viewChar[curChar].curAction);
						if (headMotionCount > 0 && accMotionCount > 0 &&
							(headMotionCount != accMotionCount) && (accMotionCount > headMotionCount) &&
							((accMotionCount % headMotionCount) == 0)) {
							int motionStep = accMotionCount / headMotionCount;
							int motionSpeed = (int)((CActRes*)g_resMgr().Get(m_viewChar[curChar].actName[i].c_str(), false))->
								GetDelay(m_viewChar[curChar].curAction);
							float stateCnt = (timeGetTime() - m_stateStartTick) / 24.0f;
							if (motionSpeed == 0)
								motionSpeed = 4;
							int motion = (int)(stateCnt / motionSpeed) % motionStep;
							int tmpMotion = m_viewChar[curChar].curMotion * motionStep + motion;
							curMotion = accMotion = ((CActRes*)g_resMgr().Get(m_viewChar[curChar].actName[i].c_str(), false))->
								GetMotion(m_viewChar[curChar].curAction, tmpMotion);
						}
					}

					if (accMotion->attachCnt && headMotion->attachCnt) {
						if (accMotion->attachInfo[0].m_attr == headMotion->attachInfo[0].m_attr) {
							offX += headMotion->attachInfo[0].x - accMotion->attachInfo[0].x;
							offY += headMotion->attachInfo[0].y - accMotion->attachInfo[0].y;
						}
					}
				}

				offX += head_imfOffX;
				offY += head_imfOffY;
				if (i == 3) {
					switch (m_accessory_top[curChar]) {
					case 0xb5:
					case 0xc5:
						offY -= 7;
						break;
					}
				}
			}

			unsigned long* palette = sprRes->m_pal;
			switch (i) {
			case 0:
				if (m_viewChar[curChar].bodyPalette) {
					CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(m_viewChar[curChar].bodyPaletteName.c_str(), false);
					palette = palRes->m_pal;
				}
				break;
			case 1:
				if (m_viewChar[curChar].headPalette) {
					CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(m_viewChar[curChar].headPaletteName.c_str(), false);
					palette = palRes->m_pal;
				}
				break;
			}

			int clipIndex = i;
			if (i > 1)
				clipIndex = 0;

			CSprClip* clip = curMotion->GetClip(clipIndex);
			if (clip->sprIndex == -1)
				continue;

			SprImg* img = sprRes->m_sprites[clip->clipType][clip->sprIndex];
			if (clip->clipType == 0) {
				tex = g_renderer->GetSpriteIndex(*img, (unsigned long)palette, &info);
				if (tex == NULL)
					tex = g_renderer->AddSpriteIndex(*img, palette, &info);
			}
			else {
				tex = img->tex;
			}

			m_dc->BltTexture2(m_viewChar[curChar].x + clip->x + offX, m_viewChar[curChar].y + clip->y + offY, tex,
				(int)(info.tu * 256.0f), (int)(info.tv * 256.0f),
				img->width, img->height, clip->flags, img->isHalfW + 1, img->isHalfH + 1);

			CHARACTER_INFO* charInfo = (CHARACTER_INFO*)g_modeMgr.GetCurMode()->SendMsg(MM_QUERYCHARICTORINFO, curChar, 0, 0);
			if (DoubleAcc == 100) {
				DoubleAcc = 0;
			}
			else if (i == 4) {
				i = 3;
				if (charInfo->accessory3 == 0x120 || charInfo->accessory3 == 0x121) {
					DoubleAcc = 1;
				}
				else if (charInfo->accessory3 == 0x124 || charInfo->accessory3 == 0x125) {
					DoubleAcc = 2;
				}
				else {
					i = 4;
				}
			}
			else if (i == 3) {
				i = 2;
				if (charInfo->accessory2 == 0x122 || charInfo->accessory2 == 0x123) {
					DoubleAcc = 3;
				}
				else {
					i = 3;
				}
			}
		}
		}
	}
}

int UISelectCharWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			if (m_okButton) {
				m_dontmove = 1;
				CHARACTER_INFO *charInfo =
					(CHARACTER_INFO *)g_modeMgr.GetCurMode()->SendMsg(MM_QUERYCHARICTORINFO, m_curPage * 3 + m_curSlot, 0, 0);
				g_session.SetCharName((const char *)charInfo->name);

				for (int i = 0; i < m_pageCount * 3; ++i) {
					if (g_session.m_charBlockInfo2[i].GID == charInfo->GID) {
						char temp[256];
						sprintf(temp, MsgStr(MSI_BLOCKED), g_session.m_charBlockInfo2[i].szExpireDate);
						g_windowMgr.ErrorMsg(temp, ME_OK, TRUE, TRUE, 0);
						m_dontmove = 0;
						return 0;
					}
				}

				PlayWave("\xb9\xf6\xc6\xb0\xbc\xd2\xb8\xae.wav", 0.0f, 0.0f, 0.0f, 250, 40, 1.0f);
				g_modeMgr.GetCurMode()->SendMsg(MM_COMMAND, LMM_TRANSFERCHARACTER, 0, 0);
				g_modeMgr.GetCurMode()->SendMsg(LMM_SELECT_CHARACTER, m_curPage * 3 + m_curSlot, 0, 0);
				g_windowMgr.PostQuit(this);
				return 0;
			}

		case ID_MAKE:
			if (m_makeButton) {
				if (m_isEmpty[m_curPage * 3 + m_curSlot] && m_isAvailable[m_curPage * 3 + m_curSlot]) {
					g_modeMgr.GetCurMode()->SendMsg(MM_COMMAND, LMM_MAKECHARACTER, 0, 0);
					g_windowMgr.PostQuit(this);
					return 0;
				}
			}
			break;

		case ID_DELETE:
			if (m_deleteButton) {
				if (!m_isEmpty[m_curPage * 3 + m_curSlot] || !m_isAvailable[m_curPage * 3 + m_curSlot]) {
					CHARACTER_INFO *charInfo =
						(CHARACTER_INFO *)g_modeMgr.GetCurMode()->SendMsg(MM_QUERYCHARICTORINFO, m_curPage * 3 + m_curSlot, 0, 0);
					for (int i = 0; i < 9; ++i) {
						if (g_session.m_charBlockInfo2[i].GID == charInfo->GID) {
							char temp[256];
							sprintf(temp, MsgStr(MSI_BLOCKED), g_session.m_charBlockInfo2[i].szExpireDate);
							g_windowMgr.ErrorMsg(temp, ME_OK, TRUE, TRUE, 0);
							return 0;
						}
					}

					if (g_windowMgr.ErrorMsg(MsgStr(MSI_DELETE_CHARACTER), ME_YESNO, FALSE, FALSE, 0) != ID_YES)
						break;
					m_dontmove = 1;
					if (g_windowMgr.EmailAddress() != 'v') {
						m_dontmove = 0;
						return 0;
					}
					if (g_windowMgr.ErrorMsgAutoreturn(MsgStr(MSI_CHARARATER_DELETE_COUNT), 0x2710) == ID_YES) {
						g_modeMgr.GetCurMode()->SendMsg(MM_COMMAND, LMM_DELETE_CHARACTER, 0, 0);
						g_windowMgr.PostQuit(this);
					}
					m_dontmove = 0;
				}
			}
			break;

		case ID_CANCEL:
			g_modeMgr.GetCurMode()->SendMsg(LMM_GOTO_LOGIN, 0, 0, 0);
			return 0;

		case ID_SLOT:
			if (!g_windowMgr.IsModalStatus()) {
				int curChar = m_curPage * 3 + m_curSlot;
				if (!m_isAvailable[curChar]) {
					if (m_okButton) {
						RemoveChild(m_okButton);
						m_okButton = NULL;
					}
					if (m_makeButton) {
						RemoveChild(m_makeButton);
						m_makeButton = NULL;
					}
					if (m_deleteButton) {
						RemoveChild(m_deleteButton);
						m_deleteButton = NULL;
					}
				}

				Invalidate();
				for (int i = 0; i < 3; ++i) {
					m_slots[i]->Invalidate();
					if (sender == m_slots[i]) {
						m_curSlot = i;
						m_slots[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
					}
					else {
						m_slots[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
					}
				}

				InitTextControls();

				if (m_isEmpty[m_curPage * 3 + m_curSlot] && m_isAvailable[m_curPage * 3 + m_curSlot])
					MakeButton(0x89);
				else if (!m_isEmpty[m_curPage * 3 + m_curSlot]) {
					MakeButton(0x76);
					MakeButton(0x91);
				}

				g_charInfo.CharNum = m_curPage * 3 + m_curSlot;

				WriteToReg();
			}
			break;

		case ID_CHARGE: {
			char msg[256];
			g_modeMgr.GetCurMode()->SendMsg(LMM_GET_BILLING_INFO, (int)msg, 0, 0);
			g_windowMgr.ErrorMsg(msg, ME_OK, TRUE, FALSE, 0);
			return 0;
		}

		case ID_NOTICE: {
			char msg[256];
			g_modeMgr.GetCurMode()->SendMsg(LMM_GET_BILLING_INFO, (int)msg, 0, 0);
			g_windowMgr.ErrorMsg(msg, ME_OK, TRUE, FALSE, 0);
			return 0;
		}

		case ID_MINUS:
			if (!m_dontmove)
				SendMsg(NULL, UM_LEFTPUSH, 0, 0, 0, 0);
			return 0;

		case ID_PLUS:
			if (!m_dontmove)
				SendMsg(NULL, UM_RIGHTPUSH, 0, 0, 0, 0);
			return 0;

		case ID_CHANGE_NAME:
		{
			if (g_windowMgr.GetChangeNameWnd())
				g_windowMgr.DeleteWindow(WID_CHANGENAMEWND);

			CHARACTER_INFO *charInfo = (CHARACTER_INFO *)g_modeMgr.GetCurMode()->SendMsg(
				8, m_curPage * 3 + m_curSlot, 0, 0);
			UIFrameWnd *wnd = g_windowMgr.MakeWindow(WID_CHANGENAMEWND);
			wnd->SendMsg(NULL, UM_SETINFO, (int)charInfo, 0, 0, 0);
			return 0;
		}

		case ID_CHANGE_NAME_SUCCESS:
		{
			CHARACTER_INFO *charInfo = (CHARACTER_INFO *)g_modeMgr.GetCurMode()->SendMsg(
				8, m_curPage * 3 + m_curSlot, 0, 0);
			if (charInfo->GID == g_session.m_changeNameGID) {
				char nameBuf[25];
				memcpy(charInfo->name, g_session.m_changeCharName, 24);
				charInfo->bIsChangedCharName = 1;
				m_changeNameBtn->Move(-100, -100);
				memcpy(nameBuf, charInfo->name, 24);
				nameBuf[24] = 0;
				m_text[0]->SetTextWithShorten(nameBuf);
			}
			return 0;
		}

		default:
			break;
		}
		return 0;

	case UM_DELETE:
		SendMsg(NULL, UM_COMMAND, 0x91, 0, 0, 0);
		return 0;

	case UM_LEFTPUSH:
		if (!g_windowMgr.IsModalStatus()) {
			if (!m_dontmove) {
				--m_curSlot;
				if (m_curSlot < 0) {
					m_curSlot = 2;
					--m_curPage;
				}
				if (m_curPage < 0)
					m_curPage = m_pageCount - 1;
				SendMsg(m_slots[m_curSlot], UM_COMMAND, 0x8a, 0, 0, 0);
				if (g_serviceType != ServiceBrazil) {
					if (m_curPage > 2)
						m_curPage = 2;
				}
				if (m_curPage < 0) {
					m_curPage = 0;
					return 0;
				}
			}
		}
		return 0;

	case UM_RIGHTPUSH:
		if (!g_windowMgr.IsModalStatus()) {
			if (!m_dontmove) {
				++m_curSlot;
				if (m_curSlot > 2) {
					m_curSlot = 0;
					++m_curPage;
				}
				if (m_curPage > m_pageCount - 1)
					m_curPage = 0;
				SendMsg(m_slots[m_curSlot], UM_COMMAND, 0x8a, 0, 0, 0);
				if (g_serviceType != ServiceBrazil) {
					if (m_curPage > 2)
						m_curPage = 2;
				}
				if (m_curPage < 0) {
					m_curPage = 0;
					return 0;
				}
			}
		}
		return 0;

	case UM_SETSTATE:
		m_curSlot = val1;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
}














UIMakeCharWnd::UIMakeCharWnd()
{
	int head;
	char tmpName[256];

	m_stateCnt = 0;
	m_nameEditCtrl = NULL;
	m_charInfo.headpalette = 0;
	m_charInfo.head = 1;
	m_charInfo.name[0] = NULL;
	m_charInfo.job = 4;
	m_charInfo.level = 1;
	m_charInfo.sppoint = 1;
	m_charInfo.jobpoint = 0;

	m_charInfo.maxhp = 100;
	m_charInfo.hp = m_charInfo.maxhp;
	m_charInfo.maxsp = 100;
	m_charInfo.sp = m_charInfo.maxsp;
	m_charInfo.exp = 0;
	m_charInfo.CharNum = g_charInfo.CharNum;

	m_charInfo.bodypalette = 0;
	m_charInfo.shield = 0;
	m_charInfo.accessory2 = 0;
	m_charInfo.accessory3 = 0;
	m_charInfo.headpalette = 0;

	sAssert(m_charInfo.head < HEADTYPE_LAST);
	m_viewChar.x = 95;
	m_viewChar.y = 213;
	m_viewChar.baseAction = 0;
	m_viewChar.curAction = m_viewChar.baseAction;
	m_viewChar.curMotion = 0;
	m_viewChar.actName[0] = g_session.GetJobActName(m_charInfo.job, g_session.GetSex(), tmpName);
	m_viewChar.sprName[0] = g_session.GetJobSprName(m_charInfo.job, g_session.GetSex(), tmpName);
	head = m_charInfo.head;
	m_viewChar.actName[1] = g_session.GetHeadActName(m_charInfo.job, head, g_session.GetSex(), tmpName);
	m_viewChar.sprName[1] = g_session.GetHeadSprName(m_charInfo.job, head, g_session.GetSex(), tmpName);
	m_charInfo.head = head;
	m_viewChar.bodyPalette = m_charInfo.bodypalette;
	if (m_viewChar.bodyPalette) {
		char palName[64];
		m_viewChar.bodyPaletteName = g_session.GetBodyPaletteName(m_charInfo.job, g_session.GetSex(), m_viewChar.bodyPalette, palName);
	}
	m_viewChar.headPalette = m_charInfo.headpalette;
	if (m_viewChar.headPalette) {
		char palName[64];
		m_viewChar.headPaletteName = g_session.GetHeadPaletteName(head, m_charInfo.job, g_session.GetSex(), m_viewChar.headPalette, palName);
	}
	m_viewChar.imfName = g_session.GetImfName(m_charInfo.job, m_charInfo.head, g_session.GetSex(), tmpName);

	m_charInfo2.headpalette = 0;
	m_charInfo2.head = 3;
	m_charInfo2.job = 10;
	m_charInfo2.CharNum = g_charInfo.CharNum;
	m_charInfo2.bodypalette = 0;
	m_charInfo2.shield = 0;
	m_charInfo2.accessory2 = 0;
	m_charInfo2.accessory3 = 0;
	m_charInfo2.headpalette = 0;

	sAssert(m_charInfo2.head < HEADTYPE_LAST);
	m_viewChar2.x = 436;
	m_viewChar2.y = 171;
	m_viewChar2.baseAction = 0;
	m_viewChar2.curAction = m_viewChar2.baseAction;
	m_viewChar2.curMotion = 0;
	RAG_BUDGET_FILL(6)
	m_viewChar2.actName[0] = g_session.GetJobActName(m_charInfo2.job, 1, tmpName);
	m_viewChar2.sprName[0] = g_session.GetJobSprName(m_charInfo2.job, 1, tmpName);
	head = m_charInfo2.head;
	m_viewChar2.actName[1] = g_session.GetHeadActName(m_charInfo2.job, head, 1, tmpName);
	m_viewChar2.sprName[1] = g_session.GetHeadSprName(m_charInfo2.job, head, 1, tmpName);
	m_charInfo2.head = head;
	m_viewChar2.bodyPalette = m_charInfo2.bodypalette;
	if (m_viewChar2.bodyPalette) {
		char palName[64];
		m_viewChar2.bodyPaletteName = g_session.GetBodyPaletteName(m_charInfo2.job, 1, m_viewChar2.bodyPalette, palName);
	}
	m_viewChar2.headPalette = m_charInfo2.headpalette;
	if (m_viewChar2.headPalette) {
		char palName[64];
		m_viewChar2.headPaletteName = g_session.GetHeadPaletteName(head, m_charInfo2.job, 1, m_viewChar2.headPalette, palName);
	}
	m_viewChar2.imfName = g_session.GetImfName(m_charInfo2.job, m_charInfo2.head, 1, tmpName);
}
UIMakeCharWnd::~UIMakeCharWnd()
{}

void UIMakeCharWnd::OnProcess()
{
	Invalidate();
	if (m_stateCnt % 0x22 == 0) {
		m_viewChar.curAction++;
		if (m_viewChar.curAction >= m_viewChar.baseAction + 8) {
			m_viewChar.curAction = m_viewChar.baseAction;
		}
		m_viewChar2.curAction--;
		if (m_viewChar2.curAction < 0) {
			m_viewChar2.curAction = m_viewChar2.baseAction + 7;
		}
	}
	m_viewChar2.curMotion = m_viewChar.curMotion = 0;
	m_stateCnt++;
}


void UIMakeCharWnd::OnCreate(int cx, int cy)
{
	CMode *mode = g_modeMgr.GetCurMode();
	mode->SendMsg(LMM_GET_MAKING_CHAR_NAME, 0, 0, 0);
	m_nameEditCtrl = new UIBitmapEditCtrl;
	m_nameEditCtrl->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\name-edit.bmp");
	m_nameEditCtrl->Create(m_nameEditCtrl->GetWidth(), m_nameEditCtrl->GetHeight());
	m_nameEditCtrl->Move(62, 244);
	m_nameEditCtrl->SetTextLimit(24);
	m_nameEditCtrl->SetId(ID_OK);
	AddChild(m_nameEditCtrl);
	MakeButton();
	g_windowMgr.SetFocusEdit(m_nameEditCtrl);
	m_defPushId = 0x76;
}

void UIMakeCharWnd::DrawHexagon()
{
	int xPos[6];
	int yPos[6];
	int maxXPos[6] = { 288, 356, 356, 288, 220, 220 };
	int maxYPos[6] = { 86, 126, 206, 244, 206, 126 };
	int params[6] = { m_charInfo.Str, m_charInfo.Vit, m_charInfo.Luk, m_charInfo.Int, m_charInfo.Dex, m_charInfo.Agi };

	for (int i = 0; i < 6; i++) {
		xPos[i] = 288 + (maxXPos[i] - 288) * params[i] / 9;
		yPos[i] = 166 + (maxYPos[i] - 166) * params[i] / 9;
	}

	for (int j = 0; j < 6; j++) {
		int next = j + 1;
		if (next == 6)
			next = 0;

		DrawTriangleDC(288, 166, xPos[j], yPos[j], xPos[next], yPos[next], 0xce977c);
	}
}



void UIMakeCharWnd::OnDraw()
{
	char text1[100];
	CTexture* tex;

	DrawBitmap(0, 0, (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\login_interface\\win_make_HP.bmp"), false), false);

	CacheInfo info;

	int i;
	for (i = 0; i < 2; ++i) {
		const char* imfName = m_viewChar.imfName.c_str();
		CImfRes* imfRes = (CImfRes*)g_resMgr().Get(imfName, false);
		CActRes* actRes = (CActRes*)g_resMgr().Get(m_viewChar.actName[i].c_str(), false);
		CSprRes* sprRes = (CSprRes*)g_resMgr().Get(m_viewChar.sprName[i].c_str(), false);

		CMotion* curMotion = actRes->GetMotion(m_viewChar.curAction, m_viewChar.curMotion);
		POINT p = imfRes->GetPoint(i, m_viewChar.curAction, m_viewChar.curMotion);
		int offX = p.x;
		int offY = p.y;

		if (i == 1) {
			CMotion* bodyMotion = ((CActRes*)g_resMgr().Get(m_viewChar.actName[0].c_str(), false))->
				GetMotion(m_viewChar.curAction, m_viewChar.curMotion);
			if (curMotion->attachCnt && bodyMotion->attachCnt) {
				if (curMotion->attachInfo[0].m_attr == bodyMotion->attachInfo[0].m_attr) {
					offX += bodyMotion->attachInfo[0].x - curMotion->attachInfo[0].x;
					offY += bodyMotion->attachInfo[0].y - curMotion->attachInfo[0].y;
				}
			}
		}

		unsigned long* palette = sprRes->m_pal;
		switch (i) {
		case 0:
			if (m_viewChar.bodyPalette) {
				CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(m_viewChar.bodyPaletteName.c_str(), false);
				palette = palRes->m_pal;
			}
			break;
		case 1:
			if (m_viewChar.headPalette) {
				CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(m_viewChar.headPaletteName.c_str(), false);
				palette = palRes->m_pal;
			}
			break;
		}

		CSprClip* clip = curMotion->GetClip(i);
		SprImg* img = sprRes->m_sprites[clip->clipType][clip->sprIndex];
		if (clip->clipType == 0) {
			tex = g_renderer->GetSpriteIndex(*img, (unsigned long)palette, &info);
			if (tex == NULL)
				tex = g_renderer->AddSpriteIndex(*img, palette, &info);
		}
		else {
			tex = img->tex;
		}

		m_dc->BltTexture2(m_viewChar.x + clip->x + offX, m_viewChar.y + clip->y + offY, tex,
			(int)(info.tu * 256.0f), (int)(info.tv * 256.0f),
			img->width, img->height, clip->flags, img->isHalfW + 1, img->isHalfH + 1);
	}

	for (i = 0; i < 2; ++i) {
		const char* imfName = m_viewChar2.imfName.c_str();
		CImfRes* imfRes = (CImfRes*)g_resMgr().Get(imfName, false);
		CActRes* actRes = (CActRes*)g_resMgr().Get(m_viewChar2.actName[i].c_str(), false);
		CSprRes* sprRes = (CSprRes*)g_resMgr().Get(m_viewChar2.sprName[i].c_str(), false);

		CMotion* curMotion = actRes->GetMotion(m_viewChar2.curAction, m_viewChar2.curMotion);
		POINT p = imfRes->GetPoint(i, m_viewChar2.curAction, m_viewChar2.curMotion);
		int offX = p.x;
		int offY = p.y;

		if (i == 1) {
			CMotion* bodyMotion = ((CActRes*)g_resMgr().Get(m_viewChar2.actName[0].c_str(), false))->
				GetMotion(m_viewChar2.curAction, m_viewChar2.curMotion);
			if (curMotion->attachCnt && bodyMotion->attachCnt) {
				if (curMotion->attachInfo[0].m_attr == bodyMotion->attachInfo[0].m_attr) {
					offX += bodyMotion->attachInfo[0].x - curMotion->attachInfo[0].x;
					offY += bodyMotion->attachInfo[0].y - curMotion->attachInfo[0].y;
				}
			}
		}

		unsigned long* palette = sprRes->m_pal;
		switch (i) {
		case 0:
			if (m_viewChar2.bodyPalette) {
				CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(m_viewChar2.bodyPaletteName.c_str(), false);
				palette = palRes->m_pal;
			}
			break;
		case 1:
			if (m_viewChar2.headPalette) {
				CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(m_viewChar2.headPaletteName.c_str(), false);
				palette = palRes->m_pal;
			}
			break;
		}

		CSprClip* clip = curMotion->GetClip(i);
		SprImg* img = sprRes->m_sprites[clip->clipType][clip->sprIndex];
		if (clip->clipType == 0) {
			tex = g_renderer->GetSpriteIndex(*img, (unsigned long)palette, &info);
			if (tex == NULL)
				tex = g_renderer->AddSpriteIndex(*img, palette, &info);
		}
		else {
			tex = img->tex;
		}

		m_dc->BltTexture2(m_viewChar2.x + clip->x + offX, m_viewChar2.y + clip->y + offY, tex,
			(int)(info.tu * 256.0f), (int)(info.tv * 256.0f),
			img->width, img->height, clip->flags, img->isHalfW + 1, img->isHalfH + 1);
	}

	sprintf(text1, "Income Diff.  =");
	TextOutA(211, 159, text1, 0, 0, 12, 0xf5cdcd);
	TextOutA(210, 158, text1, 0, 0, 12, 0xcd6969);

	short y = (short)(int)(g_sinTable[m_stateCnt % 360] * 5.0f + 158.0f);
	if (g_curHardness == 1)
		sprintf(text1, "Normal");
	else
		sprintf(text1, "Easy");

	TextOutA(310, y + 1, text1, 0, 0, 16, 0xf5cdcd);
	TextOutA(309, y, text1, 0, 0, 16, 0xcd6969);
}

void UIMakeCharWnd::OnDestroy() {}

void UIMakeCharWnd::MakeButton()
{
	std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

	const char* buttonName[8][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
		{ "login_interface\\arw-str0", "login_interface\\arw-str1", "login_interface\\arw-str0" },
		{ "login_interface\\arw-agi0", "login_interface\\arw-agi1", "login_interface\\arw-agi0" },
		{ "login_interface\\arw-vit0", "login_interface\\arw-vit1", "login_interface\\arw-vit0" },
		{ "login_interface\\arw-int0", "login_interface\\arw-int1", "login_interface\\arw-int0" },
		{ "login_interface\\arw-dex0", "login_interface\\arw-dex1", "login_interface\\arw-dex0" },
		{ "login_interface\\arw-luk0", "login_interface\\arw-luk1", "login_interface\\arw-luk0" },
	};
	const char* toolTip[8] = {
		"",
		"",
		MsgStr(MSI_DESC_STR),
		MsgStr(MSI_DESC_AGI),
		MsgStr(MSI_DESC_VIT),
		MsgStr(MSI_DESC_INT),
		MsgStr(MSI_DESC_DEX),
		MsgStr(MSI_DESC_LUK),
	};
	int ids[8] = { ID_OK, ID_CANCEL, ID_STR, ID_AGI, ID_VIT, ID_INT, ID_DEX, ID_LUK };
	int pos[8][2] = {
		{ 483, 318 },
		{ 530, 318 },
		{ 270, 50 },
		{ 190, 104 },
		{ 349, 104 },
		{ 270, 244 },
		{ 190, 190 },
		{ 349, 190 },
	};

	for (int i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), 0);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), 1);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), 2);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		button->SetToolTip(toolTip[i]);
		AddChild(button);
	}

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

		const char* buttonName[3][3] = {
			{ "scroll1left", "scroll1left", "scroll1left" },
			{ "scroll1right", "scroll1right", "scroll1right" },
			{ "scroll0up", "scroll0up", "scroll0up" },
		};
		int ids[3] = { ID_LEFT, ID_RIGHT, ID_UP };
		int pos[3][2] = {
			{ 48, 135 },
			{ 130, 135 },
			{ 89, 101 },
		};
		const char* toolTip[3] = {
			MsgStr(MSI_SELECT_HAIR_STYLE),
			MsgStr(MSI_SELECT_HAIR_STYLE),
			MsgStr(MSI_SELECT_HAIR_COLOR),
		};

		for (int i = 0; i < 3; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), 0);
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), 1);
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), 2);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			button->SetToolTip(toolTip[i]);
			AddChild(button);
		}
	}

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

		const char* buttonName[3][3] = {
			{ "scroll1left", "scroll1left", "scroll1left" },
			{ "scroll1right", "scroll1right", "scroll1right" },
			{ "scroll0up", "scroll0up", "scroll0up" },
		};
		int ids[3] = { ID_LEFT2, ID_RIGHT2, ID_UP2 };
		int pos[3][2] = {
			{ 387, 90 },
			{ 469, 90 },
			{ 428, 56 },
		};
		const char* toolTip[3] = {
			MsgStr(MSI_SELECT_HAIR_STYLE),
			MsgStr(MSI_SELECT_HAIR_STYLE),
			MsgStr(MSI_SELECT_HAIR_COLOR),
		};

		for (int i = 0; i < 3; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), 0);
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), 1);
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), 2);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			button->SetToolTip(toolTip[i]);
			AddChild(button);
		}
	}

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		const char* toolTip[1] = { MsgStr(MSI_GIVING_MANNER_POINT_SUCCESS) };

		for (int i = 0; i < 1; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + "scroll1left" + ".bmp").c_str(), 0);
			button->SetBitmapName((pathName + "scroll1left" + ".bmp").c_str(), 1);
			button->SetBitmapName((pathName + "scroll1left" + ".bmp").c_str(), 2);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(333, 157);
			button->SetId(ID_HARDNESS);
			button->SetToolTip(toolTip[i]);
			AddChild(button);
		}
	}
}

void UIMakeCharWnd::InitTextControls() {}

int UIMakeCharWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	CGameActor* AcoActor = g_session.GetGameActorByAid(1);
	CGameActor* BS_Actor = g_session.GetGameActorByAid(2);

	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			if (m_nameEditCtrl->GetTextSize() >= 4) {
				BOOL isGood = g_insultFilter.IsGoodSentence(m_nameEditCtrl->GetText());
				BOOL isCorrectLang = TRUE;

				BOOL isBadNameWithSpaceChar = g_session.IsBadNameWithSpaceChar(m_nameEditCtrl->GetText());

				if (isGood && isCorrectLang && !isBadNameWithSpaceChar) {
					if (m_nameEditCtrl->GetTextSize() >= 24) {
						g_windowMgr.ErrorMsg(MsgStr(MSI_NAMELENGTH_TOO_LONG), ME_OK, 1, 0, 0);
						m_nameEditCtrl->SetText("");
						return 0;
					}
					char tmpName[24];
					memset(tmpName, 0, sizeof(m_charInfo.name));
					strcpy(tmpName, m_nameEditCtrl->GetText());
					memcpy(m_charInfo.name, tmpName, sizeof(m_charInfo.name));
					g_charInfo = m_charInfo;
					sprintf(g_Aco_Name, "%s", tmpName);
					g_session.SetCharName(g_Aco_Name);

					g_Aco_Hair = (char)m_charInfo.head;
					g_Aco_Hair_Color = (char)m_charInfo.headpalette;
					AcoActor->SetSprHead(g_Aco_Hair);
					AcoActor->SetHeadPalette(g_Aco_Hair_Color);
					g_session.m_head = g_Aco_Hair;
					g_session.m_headPalette = g_Aco_Hair_Color;

					g_BS_Hair = (char)m_charInfo2.head;
					g_BS_Hair_Color = (char)m_charInfo2.headpalette;
					BS_Actor->SetSprHead(g_BS_Hair);
					BS_Actor->SetHeadPalette(g_BS_Hair_Color);
					g_session.SetJob(m_charInfo.job);

					g_session.InvalidateBasicWnd();
					g_windowMgr.PostQuit(this);
					g_windowMgr.SetcurScreen(1);
					g_TIME_hr = 0;
					g_TIME_min = 0;
					g_TimeStorage = timeGetTime();
					if (g_windowMgr.MakeSaveFileName_IN(g_curSave, 0x63) == 1) {
						game_ItemRentEvent = true;
						g_curEXPmode = 5;
					}
				} else {
					if (!isGood)
						g_windowMgr.ErrorMsg(MsgStr(MSI_BAD_NAME), ME_OK, 1, 0, 0);
					if (!isCorrectLang)
						g_windowMgr.ErrorMsg(MsgStr(MSI_ENGLISHONLY), ME_OK, 1, 0, 0);
					if (isBadNameWithSpaceChar)
						g_windowMgr.ErrorMsg(MsgStr(MSI_NO_SPACE_IN_NAME), ME_OK, 1, 0, 0);
				}
			}
			else
			{
				g_windowMgr.ErrorMsg(MsgStr(MSI_ENTER_NAME_MORE_THAN_4_CHAR), ME_OK, 1, 0, 0);
			}
			RAG_BUDGET_FILL(9)
			break;

		case ID_CANCEL:
			g_windowMgr.PostQuit(this);
			g_windowMgr.SetcurScreen(0x6e);
			break;

		case ID_STR:
			if (m_charInfo.Int >= 2) {
				m_charInfo.Int--;
				m_charInfo.Str++;
				if (m_charInfo.Int + m_charInfo.Str != 10) {
					m_charInfo.Int = 5;
					m_charInfo.Str = 5;
				}
			}
			break;

		case ID_AGI:
			if (m_charInfo.Luk >= 2) {
				m_charInfo.Luk--;
				m_charInfo.Agi++;
				if (m_charInfo.Luk + m_charInfo.Agi != 10) {
					m_charInfo.Luk = 5;
					m_charInfo.Agi = 5;
				}
			}
			break;

		case ID_VIT:
			m_x = m_x; m_x = m_x; m_x = m_x; m_x = m_x; m_x = m_x; m_x = m_x; m_x = m_x;
			if (m_charInfo.Dex >= 2) {
				m_charInfo.Dex--;
				m_charInfo.Vit++;
				if (m_charInfo.Vit + m_charInfo.Dex != 10) {
					m_charInfo.Vit = 5;
					m_charInfo.Dex = 5;
				}
			}
			break;

		case ID_INT:
			if (m_charInfo.Str >= 2) {
				m_charInfo.Str--;
				m_charInfo.Int++;
				if (m_charInfo.Int + m_charInfo.Str != 10) {
					m_charInfo.Int = 5;
					m_charInfo.Str = 5;
				}
			}
			break;

		case ID_DEX:
			if (m_charInfo.Vit >= 2) {
				m_charInfo.Vit--;
				m_charInfo.Dex++;
				if (m_charInfo.Vit + m_charInfo.Dex != 10) {
					m_charInfo.Vit = 5;
					m_charInfo.Dex = 5;
				}
			}
			break;

		case ID_LUK:
			if (m_charInfo.Agi >= 2) {
				m_charInfo.Agi--;
				m_charInfo.Luk++;
				if (m_charInfo.Luk + m_charInfo.Agi != 10) {
					m_charInfo.Luk = 5;
					m_charInfo.Agi = 5;
				}
			}
			break;

		case ID_UP:
			m_viewChar.headPalette++;
			if (m_viewChar.headPalette > 8)
				m_viewChar.headPalette = 0;
			if (m_viewChar.headPalette) {
				char palName[64];
				int head = m_charInfo.head;
				m_viewChar.headPaletteName = g_session.GetHeadPaletteName(head, m_charInfo.job, g_session.GetSex(), m_viewChar.headPalette, palName);
			}
			m_charInfo.headpalette = (short)m_viewChar.headPalette;
			break;

		case ID_LEFT: {
			int head;
			char tmpName[256];
			Invalidate();
			m_charInfo.head--;
			if (m_charInfo.head == 0)
				m_charInfo.head = 25;
			head = m_charInfo.head;
			m_viewChar.actName[1] = g_session.GetHeadActName(m_charInfo.job, head, g_session.GetSex(), tmpName);
			m_viewChar.sprName[1] = g_session.GetHeadSprName(m_charInfo.job, head, g_session.GetSex(), tmpName);
			m_charInfo.head = (short)head;
			m_viewChar.imfName = g_session.GetImfName(m_charInfo.job, m_charInfo.head, g_session.GetSex(), tmpName);
			if (m_viewChar.headPalette) {
				char palName[64];
				m_viewChar.headPaletteName = g_session.GetHeadPaletteName(
					head, m_charInfo.job, g_session.GetSex(), m_viewChar.headPalette, palName);
			}
			break;
		}

		case ID_RIGHT: {
			int head;
			char tmpName[256];
			Invalidate();
			m_charInfo.head++;
			if (m_charInfo.head == 26)
				m_charInfo.head = 1;
			head = m_charInfo.head;
			m_viewChar.actName[1] = g_session.GetHeadActName(m_charInfo.job, head, g_session.GetSex(), tmpName);
			m_viewChar.sprName[1] = g_session.GetHeadSprName(m_charInfo.job, head, g_session.GetSex(), tmpName);
			m_charInfo.head = (short)head;
			m_viewChar.imfName = g_session.GetImfName(m_charInfo.job, m_charInfo.head, g_session.GetSex(), tmpName);
			if (m_viewChar.headPalette) {
				char palName[64];
				m_viewChar.headPaletteName = g_session.GetHeadPaletteName(
					head, m_charInfo.job, g_session.GetSex(), m_viewChar.headPalette, palName);
				Trace("headPaletteName = %s", m_viewChar.headPaletteName.c_str());
			}
			break;
		}

		case ID_UP2:
			m_viewChar2.headPalette++;
			if (m_viewChar2.headPalette > 8)
				m_viewChar2.headPalette = 0;
			if (m_viewChar2.headPalette) {
				char palName[64];
				int head = m_charInfo2.head;
				m_viewChar2.headPaletteName = g_session.GetHeadPaletteName(head, m_charInfo2.job, 1, m_viewChar2.headPalette, palName);
			}
			m_charInfo2.headpalette = (short)m_viewChar2.headPalette;
			break;

		case ID_LEFT2: {
			int head;
			char tmpName[256];
			Invalidate();
			m_charInfo2.head--;
			if (m_charInfo2.head == 0)
				m_charInfo2.head = 25;
			head = m_charInfo2.head;
			m_viewChar2.actName[1] = g_session.GetHeadActName(m_charInfo2.job, head, 1, tmpName);
			m_viewChar2.sprName[1] = g_session.GetHeadSprName(m_charInfo2.job, head, 1, tmpName);
			m_charInfo2.head = (short)head;
			m_viewChar2.imfName = g_session.GetImfName(m_charInfo2.job, m_charInfo2.head, 1, tmpName);
			if (m_viewChar2.headPalette) {
				char palName[64];
				m_viewChar2.headPaletteName = g_session.GetHeadPaletteName(
					head, m_charInfo2.job, 1, m_viewChar2.headPalette, palName);
			}
			break;
		}

		case ID_RIGHT2: {
			int head;
			char tmpName[256];
			Invalidate();
			m_charInfo2.head++;
			if (m_charInfo2.head == 26)
				m_charInfo2.head = 1;
			head = m_charInfo2.head;
			m_viewChar2.actName[1] = g_session.GetHeadActName(m_charInfo2.job, head, 1, tmpName);
			m_viewChar2.sprName[1] = g_session.GetHeadSprName(m_charInfo2.job, head, 1, tmpName);
			m_charInfo2.head = (short)head;
			m_viewChar2.imfName = g_session.GetImfName(m_charInfo2.job, m_charInfo2.head, 1, tmpName);
			if (m_viewChar2.headPalette) {
				char palName[64];
				m_viewChar2.headPaletteName = g_session.GetHeadPaletteName(
					head, m_charInfo2.job, 1, m_viewChar2.headPalette, palName);
			}
			break;
		}

		case ID_HARDNESS:
			if (g_curHardness == 1)
				g_curHardness = 0;
			else
				g_curHardness = 1;
			break;
		}
		break;

	case 0:
		{
			char tmpName[25];
			strcpy(tmpName, m_nameEditCtrl->GetText());
			memcpy(m_charInfo.name, tmpName, sizeof(m_charInfo.name));
			SendMsg(NULL, UM_COMMAND, m_defPushId, 0, 0, 0);
		}
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIChatWnd::UIChatWnd()
{
	m_commonChat = 0;
	m_whisperChat = 0;
	m_lastFocusChat = 0;
	m_resizer = 0;
	m_listBox = 0;
	m_state = 0;
	m_comboMode = 0;
}


UIChatWnd::~UIChatWnd() {}

void UIChatWnd::OnCreate(int cx, int cy)
{
	if (cy <= 24) {
		m_state = 0;
	} else {
		m_state = (cy - 98) / 42 + 1;
		if (m_state > 4)
			m_state = 4;
	}

	m_commonChat = new UIChatEditCtrl;
	m_commonChat->Create(460, 16);
	m_commonChat->Move(110, m_h - 20);
	m_commonChat->SetTextLimit(70);
	m_commonChat->SetFrameColor(0xf0, 0xf0, 0xf0);
	AddChild(m_commonChat);
	m_lastFocusChat = m_commonChat;
	g_windowMgr.SetFocusEdit(m_commonChat);

	m_whisperChat = new UIWhisperEditCtrl;
	m_whisperChat->Create(90, 16);
	m_whisperChat->Move(6, m_h - 20);
	m_whisperChat->SetTextLimit(24);
	m_whisperChat->SetFrameColor(0xf0, 0xf0, 0xf0);
	AddChild(m_whisperChat);

	m_defPushId = 0x76;

	m_resizer = new UIResizer;
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\dialog_mid.bmp");
	m_resizer->Create(m_resizer->GetWidth(), m_resizer->GetHeight());
	m_resizer->SetType(RT_LEFT);
	m_resizer->Move(2, m_h - m_resizer->GetHeight());
	AddChild(m_resizer);

	int gx;
	int gy;
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\";
	char* buttonName[3][3] = {
		{ "dialog_btn0", "dialog_btn1", "dialog_btn2" },
		{ "sys_base_off", "sys_base_on", "sys_base_on" },
		{ "sys_base_off", "sys_base_on", "sys_base_on" }
	};
	int ids[3] = { ID_WHISPERCOMBO, ID_HIS, ID_TALKTOWHOCOMBO };
	int pos[3][2] = {
		{ 97, m_h - 21 },
		{ 586, m_h - 17 },
		{ 572, m_h - 17 }
	};

	for (int i = 0; i < 3; ++i) {
		m_buttons[i] = new UIBitmapButton;
		m_buttons[i]->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), 0);
		m_buttons[i]->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), 1);
		m_buttons[i]->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), 2);
		m_buttons[i]->Create(m_buttons[i]->GetBitmapWidth(), m_buttons[i]->GetBitmapHeight());
		m_buttons[i]->Move(pos[i][0], pos[i][1]);
		m_buttons[i]->SetId(ids[i]);
		AddChild(m_buttons[i]);
	}

	m_listBox = new UIChatHisBox;
	m_listBox->Create2(0, 0, 590, 74, false);
	if (cy - 74 >= 24)
		m_listBox->Move(5, 0);
	else
		m_listBox->Move(5, -74);
	m_listBox->SetId(ID_OK);
	AddChild(m_listBox);

	UIResizer* topResizer = new UIResizer;
	topResizer->Create(m_w - 26, 6);
	topResizer->SetType(RT_UP);
	topResizer->Move(3, 0);
	topResizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(topResizer);

	gx = 0;
	gy = 0;
	GetGlobalCoor(gx, gy);
	m_boxInfo.x = gx + 3;
	m_boxInfo.y = gy;
	m_boxInfo.cx = m_w - 6;
	m_boxInfo.cy = m_h - 24;
	m_boxInfo.color2 = 0;
	m_boxInfo.drawEdge = 0;
	m_boxInfo.color = 0x66000000;
}

void UIChatWnd::OnLBtnUp(int x, int y)
{
	UIFrameWnd::OnLBtnUp(x, y);
}

void UIChatWnd::OnMouseMove(int x, int y)
{
	if (g_windowMgr.GetCapture() == this) {
		UIFrameWnd::OnMouseMove(x, y);
	}
}

void UIChatWnd::OnLBtnDown(int x, int y)
{
	UIFrameWnd::OnLBtnDown(x, y);
}

void UIChatWnd::Move(int x, int y)
{
	UIFrameWnd::Move(x, y);
	int gx = 0;
	int gy = 0;
	GetGlobalCoor(gx, gy);
	m_boxInfo.x = gx + 3;
	m_boxInfo.y = gy;
}

void UIChatWnd::SetState(int state)
{
	int pos[3][2];

	if (g_windowMgr.GetComboBoxWnd())
		g_windowMgr.DeleteWindow(WID_COMBOBOXWND);

	if (g_windowMgr.GetCapture() == this)
		return;

	if (state > 15)
		state = 0;
	else if (state < 0)
		state = 15;

	m_state = state;
	Invalidate();

	if (state) {
		int height = (state + 4) * 14;
		int oldHeight = m_h;
		Resize(m_w, height);
		Move(m_x, m_y + oldHeight - height);
		m_listBox->Resize(590, height - 24);
		m_listBox->Move(5, 0);

		m_whisperChat->Move(6, m_h - 20);
		m_commonChat->Move(110, m_h - 20);
		m_resizer->Move(3, m_h - m_resizer->GetHeight());

		pos[0][0] = 97;
		pos[1][0] = 586;
		pos[2][0] = 572;
		pos[0][1] = m_h - 21;
		pos[1][1] = m_h - 17;
		pos[2][1] = m_h - 17;

		for (int i = 0; i < 3; ++i)
			m_buttons[i]->Move(pos[i][0], pos[i][1]);
	} else {
		int oldHeight = m_h;
		Resize(m_w, 24);
		Move(m_x, m_y + oldHeight - 24);
		m_listBox->Resize(20, 18);
		m_listBox->Move(5, -18);

		m_whisperChat->Move(6, m_h - 20);
		m_commonChat->Move(110, m_h - 20);
		m_resizer->Move(3, m_h - m_resizer->GetHeight());

		pos[0][0] = 97;
		pos[1][0] = 586;
		pos[2][0] = 572;
		pos[0][1] = m_h - 21;
		pos[1][1] = m_h - 17;
		pos[2][1] = m_h - 17;

		for (int i = 0; i < 3; ++i)
			m_buttons[i]->Move(pos[i][0], pos[i][1]);
	}
}

bool UIChatWnd::GetTransBoxInfo(BOXINFO* boxInfo)
{
	if (m_state) {
		*boxInfo = m_boxInfo;
		return true;
	}
	return false;
}

void UIChatWnd::Resize(int cx, int cy)
{
	UIWindow::Resize(cx, cy);
	m_boxInfo.cx = m_w - 6;
	m_boxInfo.cy = m_h - 24;
}

void UIChatWnd::OnDraw()
{
	if (g_session.m_isBattle == 1 && g_session.m_isBmChat != 1) {
		CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\dialog_bg2.bmp"),
			false);
		DrawBitmap(0, m_h - bmp->m_height, bmp, false);
		return;
	}

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\dialog_bg.bmp"),
		false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);
}

void UIChatWnd::OnDraw2()
{
	if (m_h > 24) {
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = m_w;
		rect.bottom = m_h - 24;
		ClearDCRect(0xffff00ff, rect);
	}
}

void UIChatWnd::StoreInfo()
{
	g_windowMgr.m_chatWndStatus = m_state;
	g_windowMgr.m_chatWndHeight = m_h;
	g_windowMgr.m_chatWndX = m_x;
	g_windowMgr.m_chatWndY = m_y;
}

void UIChatWnd::SetShow(int visible)
{
	m_show = visible;
	if (visible) {
		Invalidate();
		g_windowMgr.UpdateSnapInfo(this);
	} else {
		g_windowMgr.DeleteSnapInfo(this);
	}
	m_listBox->SetShow(m_show);
}

extern "C" int __cdecl access(const char *, int);
extern "C" int __cdecl _mkdir(const char *);

void UIChatWnd::WriteChatToReportFile()
{
	char tmp[2048];
	int captureCnt;

	captureCnt = 0;
	sprintf(tmp, "%s\\Report", g_baseDir3);
	if (access(tmp, 0)) {
		if (_mkdir(tmp))
			return;
	}

	std::string strCharacter = g_session.GetCharName();
	FixInvalidNameForFile(strCharacter);

	sprintf(tmp, "%s\\Report\\%s.txt", g_baseDir3, strCharacter.c_str());
	while (access(tmp, 0) == 0)
		sprintf(tmp, "%s\\Report\\%s%03d.txt", g_baseDir3, strCharacter.c_str(), ++captureCnt);

	if (m_listBox->WriteChatToReportFile(tmp)) {
		std::string chatStoreMsg = tmp;
		chatStoreMsg += " is Saved.";
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)chatStoreMsg.c_str(), 0xff00, 0, 0);
	}
}

void UIChatWnd::WriteChatToFile()
{
	char tmp[2048];
	int captureCnt = 0;

	sprintf(tmp, "%s\\Chat", g_baseDir3);
	if (access(tmp, 0)) {
		if (_mkdir(tmp))
			return;
	}

	sprintf(tmp, "%s\\Chat\\Chat.txt", g_baseDir3);
	while (access(tmp, 0) == 0)
		sprintf(tmp, "%s\\Chat\\Chat%03d.txt", g_baseDir3, ++captureCnt);

	if (m_listBox->WriteChatToFile(tmp)) {
		std::string chatStoreMsg = tmp;
		chatStoreMsg += " is Saved.";
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)chatStoreMsg.c_str(), 0xff00, 0, 0);
	}
}

int UIChatWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_DELETE_WHISPER_NAME:
		{
			std::string name((const char*)val1);
			int size = m_whisperChat->m_hisChat.size();
			std::vector<std::string>::iterator it = m_whisperChat->m_hisChat.begin();
			int i = 0;
			while (it != m_whisperChat->m_hisChat.end()) {
				if (*it == name) {
					m_whisperChat->m_hisChat.erase(it);
					if (m_whisperChat->m_curHis == i)
						m_whisperChat->m_curHis = size - 1;
					else if (m_whisperChat->m_curHis > i)
						--m_whisperChat->m_curHis;
					break;
				}
				++it;
				++i;
			}
		}
		break;

	case UM_GET_EDIT_FOCUS:
		g_windowMgr.SetFocusEdit(m_commonChat);
		break;

	case UM_COMBOBOX_SELECTED:
		switch (m_comboMode) {
		case 0:
			{
				int size = m_whisperChat->m_hisChat.size();
				if (val1 >= 0 && val1 <= size) {
					if (val1 == size)
						m_whisperChat->SetText("");
					else
						m_whisperChat->SetText(m_whisperChat->m_hisChat[val1].c_str());
					m_whisperChat->m_curHis = val1;
				}
				g_windowMgr.SetFocusEdit(m_whisperChat);
				g_windowMgr.SetFocusEdit(m_commonChat);
			}
			break;

		case 1:
			g_session.m_talkType = val1;
			switch (val1) {
			case 0:
				m_commonChat->SetTextColor(0, 0, 0);
				break;
			case 1:
				m_commonChat->SetTextColor(0x80, 0, 0x80);
				break;
			case 2:
				m_commonChat->SetTextColor(0, 0x80, 0x80);
				break;
			}
			Invalidate();
			break;
		}
		break;

	case UM_ADDCHAT:
		{
			std::vector<std::string> strings;
			CutString((const char*)val1, strings, 94);
			for (unsigned i = 0; i < strings.size(); ++i)
				m_listBox->AddItem(strings[i].c_str(), val2, NULL);
		}
		break;

	case UM_SETINFO:
		{
			bool invalid = false;
			if (g_windowMgr.m_chatWndX < 0)
				invalid = true;
			if (g_windowMgr.m_chatWndY < 0)
				invalid = true;
			if (g_windowMgr.m_chatWndX > g_renderer->m_width - m_w)
				invalid = true;
			if (g_windowMgr.m_chatWndY > g_renderer->m_height - m_h)
				invalid = true;
			if (invalid) {
				g_windowMgr.m_chatWndX = 0;
				g_windowMgr.m_chatWndY = g_renderer->m_height - m_h;
			}

			m_x = g_windowMgr.m_chatWndX;
			m_y = g_windowMgr.m_chatWndY;
			SetState(g_windowMgr.m_chatWndStatus);
			Move(m_x, m_y);
		}
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_TALKTOWHOCOMBO:
			{
				m_comboMode = 1;
				if (g_windowMgr.GetComboBoxWnd())
					g_windowMgr.DeleteWindow(WID_COMBOBOXWND);

				UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
				comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_SEND_TO_ALL), 0, 0, 0);
				comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_SEND_TO_PARTY), 0, 0, 0);
				comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_SEND_TO_GUILD), 0, 0, 0);
				comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, g_session.m_talkType, 0, 0, 0);
				comboBoxWnd->Resize(112, 48);
				comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_CHATWND, 0, 0, 0);

				int gx = m_w - 115;
				int gy = m_h - 24;
				GetGlobalCoor(gx, gy);
				comboBoxWnd->Move(gx + 1, gy - 47);
			}
			break;

		case ID_WHISPERCOMBO:
			{
				m_comboMode = 0;
				if (g_windowMgr.GetComboBoxWnd())
					g_windowMgr.DeleteWindow(WID_COMBOBOXWND);

				int whisperSize = m_whisperChat->m_hisChat.size();
				if (whisperSize) {
					UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
					for (int i = 0; i < whisperSize; ++i)
						comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)m_whisperChat->m_hisChat[i].c_str(), 0, 0, 0);

					comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)"", 0, 0, 0);
					int height = (whisperSize + 1) * 16 < 80 ? (whisperSize + 1) * 16 : 80;

					comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, m_whisperChat->m_curHis, 0, 0, 0);
					comboBoxWnd->Resize(112, height);
					comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_CHATWND, 0, 0, 0);

					int gx = 0;
					int gy = m_h - 24;
					GetGlobalCoor(gx, gy);
					comboBoxWnd->Move(gx + 1, gy - height + 1);
				} else {
					g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_NOWHISPERLIST), 0xff, 0, 0);
				}
			}
			break;

		case ID_OK:
			{
				char chatBuf[256];
				strcpy(chatBuf, m_commonChat->GetText());
				m_commonChat->StoreInHistory();
				m_lastFocusChat = m_commonChat;
				g_windowMgr.SetFocusEdit(m_commonChat);

				if (!strlen(chatBuf))
					break;

				if (g_serviceType == ServiceAmerica) {
					if (!g_session.IsOnlyEnglish(chatBuf)) {
						g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ENGLISHONLY), 0xff, 0, 0);
						break;
					}
				}

				if (m_whisperChat->GetTextSize() && chatBuf[0] != '/') {
					g_windowMgr.SetChatMsg(chatBuf);
					g_modeMgr.GetCurMode()->SendMsg(MM_WHISPERMSG, (int)m_whisperChat->GetText(), 0, 0);
					break;
				}

				std::string param[3];
				TALKTYPE talkType;
				int talkOffset = g_session.GetTalkType(chatBuf, &talkType, param);
				if (talkOffset != -1) {
					g_windowMgr.SetChatMsg(chatBuf + talkOffset);
				}
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, talkType, (int)param, 0);
			}
			break;

		case ID_HIS:
			SendMsg(NULL, UM_TOGGLEPUSH, 0, 0, 0, 0);
			break;
		}
		break;

	case UM_RESIZE:
		switch (val1) {
		case 2:
			RAG_BUDGET_FILL(10)
			if (val3 >= 56) {
				int state = (val3 - 56) / 14 + 1;
				if (state > 15)
					state = 15;
				if (m_state != state)
					SetState(state);
			} else {
				if (m_state)
					SetState(0);
			}
			break;
		}
		break;

	case 2:
		{
			int step = val1 ? -1 : 1;
			int state;
			if (m_state)
				state = m_state + step * 3;
			else
				state = step;

			if (state > 0)
				state = state - state % 3 + 1;
			else if (m_state)
				state = 0;
			SetState(state);
		}
		break;

	case UM_PAGEUPPUSH:
		if (m_listBox->IsVertScrEnabled())
			m_listBox->SendMsg(NULL, UM_SCROLLVERTPAGEUP, 0, 0, 0, 0);
		break;

	case UM_PAGEDOWNPUSH:
		if (m_listBox->IsVertScrEnabled())
			m_listBox->SendMsg(NULL, UM_SCROLLVERTPAGEDOWN, 0, 0, 0, 0);
		break;

	case UM_CHANGESTATUS:
		SetState(val1);
		break;

	case UM_OPENCHATWND:
		if (!m_state)
			SetState(1);
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIChatRoomWnd::UIChatRoomWnd()
{
	m_info = NULL;
}

UIChatRoomWnd::~UIChatRoomWnd()
{}

void UIChatRoomWnd::OnCreate(int cx, int cy)
{
	m_defPushId = 0x76;

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWNRIGHT);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_chatEdit = new UIChatEditCtrl;
	m_chatEdit->Create(m_w - 30, 16);
	m_chatEdit->Move(3, m_h - 18);
	m_chatEdit->SetTextLimit(70);
	m_chatEdit->SetFrameColor(0xf2, 0xf2, 0xf2);
	AddChild(m_chatEdit);
	g_windowMgr.SetFocusEdit(m_chatEdit);

	m_listBox = new UIChatHisBox2;
	m_listBox->Create2(0, 0, m_w - (m_w * 56) / 280 - 3, m_h - 38, 0);
	m_listBox->Move(1, 17);
	m_listBox->SetId(ID_OK);
	AddChild(m_listBox);

	m_memberListBox = new UIMemberListBox;
	m_memberListBox->Create2(0, 0, (m_w * 56) / 280, m_h - 38, 0);
	m_memberListBox->Move(m_w - (m_w * 56) / 280 - 1, 17);
	m_memberListBox->SetId(ID_OK);
	AddChild(m_memberListBox);

	{
		int posX[3] = { m_w - 14 };
		int ids2[3] = { ID_CLOSE };
		char* buttonName2[8] = { "close" };
		std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

		for (int i = 0; i < 1; ++i) {
			m_sysButton = new UIBitmapButton;
			m_sysButton->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), 0);
			m_sysButton->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), 1);
			m_sysButton->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), 2);
			m_sysButton->Create(m_sysButton->GetBitmapWidth(), m_sysButton->GetBitmapHeight());
			m_sysButton->Move(posX[i], 3);
			m_sysButton->SetId(ids2[i]);
			m_sysButton->SetToolTip("/q");
			AddChild(m_sysButton);
		}
	}

	m_title = new UIStaticText;
	m_title->Create(m_w - 30, 15);
	m_title->Move(9, 2);
	m_title->SetTextWithShorten("");
	m_title->SetFont(0, 12);
	AddChild(m_title);
}

void UIChatRoomWnd::Move(int x, int y)
{
	UIFrameWnd::Move(x, y);
}

void UIChatRoomWnd::StoreInfo()
{
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}

void UIChatRoomWnd::OnDraw()
{
	DWORD rr;
	DWORD gg;
	DWORD bb;
	int i;

	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bitmap, false);

	bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bitmap->m_width, 0, bitmap, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (i = 0; i < titleMidCount; i++) {
		bitmap = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(12 + i * 12, 0, bitmap, false);
	}

	g_skinMgr.GetColorChipColor(20, 6, rr, gg, bb);
	DrawBox(0, 17, 1, m_h - 38, RGB(rr, gg, bb));
	DrawBox(m_w - 1, 17, 1, m_h - 38, RGB(rr, gg, bb));
	DrawBox(m_w - (m_w * 56) / 280 - 2, 17, 1, m_h - 38, RGB(rr, gg, bb));

	bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp"), false);
	DrawBitmap(0, m_h - bitmap->m_width, bitmap, false);

	bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp"), false);
	DrawBitmap(m_w - bitmap->m_width, m_h - 21, bitmap, false);

	int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (i = 0; i < btnMidCount; i++) {
		bitmap = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp"), false);
		DrawBitmap(21 + i * 21, m_h - bitmap->m_height, bitmap, false);
	}
}

void UIChatRoomWnd::WriteChatToReportFile()
{
	char tmp[2048];
	int captureCnt;

	captureCnt = 0;
	sprintf(tmp, "%s\\Report", g_baseDir3);
	if (access(tmp, 0)) {
		if (_mkdir(tmp))
			return;
	}

	std::string strCharacter = g_session.GetCharName();
	FixInvalidNameForFile(strCharacter);

	sprintf(tmp, "%s\\Report\\%s.txt", g_baseDir3, strCharacter.c_str());
	while (access(tmp, 0) == 0)
		sprintf(tmp, "%s\\Report\\%s%03d.txt", g_baseDir3, strCharacter.c_str(), ++captureCnt);

	if (m_listBox->WriteChatToReportFile(tmp)) {
		std::string chatStoreMsg = tmp;
		chatStoreMsg += " is Saved.";
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)chatStoreMsg.c_str(), 0xff00, 0, 0);
	}
}

void UIChatRoomWnd::WriteChatToFile()
{
	char tmp[2048];
	int captureCnt = 0;

	sprintf(tmp, "%s\\Chat", g_baseDir3);
	if (access(tmp, 0)) {
		if (_mkdir(tmp))
			return;
	}

	std::string title = m_roomInfo.title;
	FixInvalidNameForFile(title);

	sprintf(tmp, "%s\\Chat\\%s.txt", g_baseDir3, title.c_str());
	while (access(tmp, 0) == 0)
		sprintf(tmp, "%s\\Chat\\%s%03d.txt", g_baseDir3, title.c_str(), ++captureCnt);

	if (m_listBox->WriteChatToFile(tmp)) {
		std::string chatStoreMsg = tmp;
		chatStoreMsg += " is Saved.";
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)chatStoreMsg.c_str(), 0xff00, 0, 0);
	}
}




int UIChatRoomWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO2:
		m_info = (ChatRoomWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, 7, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		return 0;

	case UM_COMMAND:
		switch (val1) {
		case ID_NAME:
			g_windowMgr.AddWhisperCharName2((const char*)val2);
			return 0;
		case ID_OK: {
			char chatBuf[256];
			if (!m_chatEdit->GetTextSize()) return 0;

			strcpy(chatBuf, m_chatEdit->GetText());

			if (g_serviceType == ServiceAmerica) {
				if (!g_session.IsOnlyEnglish(chatBuf)) {
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ENGLISHONLY), 0xff, 0, 0);
					return 0;
				}
			}

			unsigned char isMyName = 0;
			if (m_memberListBox->GetSelected() != -1) {
				std::string whisperMember = m_memberListBox->GetItem(m_memberListBox->GetSelected());
				if (whisperMember == g_session.GetCharName())
					isMyName = 1;
			}

			if (m_memberListBox->GetSelected() != -1 && chatBuf[0] != '/' && !isMyName) {
				g_windowMgr.SetChatMsg(chatBuf);
				std::string whisperMember = m_memberListBox->GetItem(m_memberListBox->GetSelected());
				g_modeMgr.GetCurMode()->SendMsg(MM_WHISPERMSG, (int)whisperMember.c_str(), 0, 0);
			} else {
				std::string param[3];
				TALKTYPE talkType;
				int len = g_session.GetTalkType(chatBuf, &talkType, param);
				if (len != -1)
					g_windowMgr.SetChatMsg(chatBuf + len);
				if (talkType == 0)
					talkType = TT_NORMAL_FROM_CHATROOM;
				else if (talkType == TT_REQ_SAVE_CHAT)
					talkType = TT_REQ_SAVE_CHAT_FROM_CHATROOM;
				else if (talkType == TT_REQ_REPORT_CHAT)
					talkType = TT_REQ_REPORT_CHAT_FROM_CHATROOM;
				g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, talkType, (int)param, 0);
			}

			m_chatEdit->StoreInHistory();
			m_chatEdit->SetText("");
			g_windowMgr.SetFocusEdit(m_chatEdit);
			break;
		}
		case ID_CLOSE:
			g_modeMgr.GetCurMode()->SendMsg(MM_REQ_EXIT_ROOM, 0, 0, 0);
			return 0;
		}
		return 0;

	case UM_MENU_SELECTED: {
		CGameMode* curMode = g_modeMgr.GetGameMode();

		switch (m_menuItems[val1].id) {
		case 0:
			if (m_memberListBox->GetSelected() != -1) {
				std::string name = m_memberListBox->GetItem(m_memberListBox->GetSelected());
				curMode->SendMsg(MM_REQ_EXPEL_MEMBER, (int)name.c_str(), 0, 0);
			}
			break;
		case 1:
			if (m_memberListBox->GetSelected() != -1) {
				std::string name = m_memberListBox->GetItem(m_memberListBox->GetSelected());
				curMode->SendMsg(MM_REQ_ROLE_CHANGE, (int)name.c_str(), 0, 0);
			}
			break;
		case 2: {
			UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_CHATROOMCHANGEWND);
			wnd->SendMsg(NULL, UM_SETINFO, (int)curMode->GetLastChatroomInfo(), 0, 0, 0);
			break;
		}
		case 3:
			g_windowMgr.ErrorMsg(MsgStr(MSI_NOTSUPPORTEDYET), ME_OK, 1, 0, 0);
			break;
		case 0xb:
			WriteChatToFile();
			break;
		case 0x15:
			if (m_memberListBox->GetSelected() != -1) {
				std::string name = m_memberListBox->GetItem(m_memberListBox->GetSelected());
				g_windowMgr.AddFriendCharName(name.c_str());
			}
			break;
		}
		return 0;
	}

	case UM_MAKEMENU: {
		m_menuItems.erase(m_menuItems.begin(), m_menuItems.end());
		if (g_windowMgr.GetMenu())
			g_windowMgr.DeleteWindow(WID_MENUWND);

		std::string myName = g_session.GetCharName();
		std::map<std::string, unsigned long>::iterator it = m_memberList.find(myName);
		int myRole = -1;
		if (it != m_memberList.end())
			myRole = m_memberList[myName];
		if (myRole == -1)
			return 0;

		if (m_memberListBox->GetSelected() != -1) {
			std::string memberName = m_memberListBox->GetItem(m_memberListBox->GetSelected());
			int memberRole = -1;
			it = m_memberList.find(memberName);
			if (it != m_memberList.end())
				memberRole = m_memberList[memberName];
			if (memberRole == -1)
				return 0;

			switch (myRole) {
			case 0:
					if (memberName != myName) {
						m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_BAN)).c_str(), 0));

						m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_GIVE_GIVE_ROOM_POWER)).c_str(), 1));
					}

				m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_SEE_INFORMATION_OF_CHARACTER)).c_str(), 3));

				if (g_serverType != ServerInstantEvent) {
					if (!g_windowMgr.IsFriendName(memberName.c_str()) && memberName != myName)
						m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_ADD_TO_FRIEND_CHAR_LIST)).c_str(), 0x15));
				}

				m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_CHANGE_ROOM_SETTING)).c_str(), 2));
				break;
			case 1:
				m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_SEE_INFORMATION_OF_CHARACTER)).c_str(), 3));

				if (!g_windowMgr.IsFriendName(memberName.c_str()) && memberName != myName) {
					if (g_serverType != ServerInstantEvent)
						m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_ADD_TO_FRIEND_CHAR_LIST)).c_str(), 0x15));
				}
				break;
			}
		} else {
			if (myRole == 0) {
				m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_CHANGE_ROOM_SETTING)).c_str(), 2));
			}
			RAG_BUDGET_FILL(10)
			m_menuItems.push_back(StringId(((std::string)MsgStr(MSI_WRITE_CHAT_TO_FILE)).c_str(), 0xb));
		}

		if (m_menuItems.size() != 0) {
			UIFrameWnd* menu = g_windowMgr.MakeWindow(WID_MENUWND);
			for (int i = 0; i < m_menuItems.size(); ++i)
				menu->SendMsg(NULL, UM_ADDITEM, (int)m_menuItems[i].itemName.c_str(), 0, 0, 0);
			menu->SendMsg(NULL, UM_SET_CALLER_WID, (int)WID_CHATROOMWND, 0, 0, 0);
			menu->SendMsg(NULL, UM_ADJUSTSIZE, 0, 0, 0, 0);
			val1 = g_mouse.GetXPos() + 1;
			val2 = g_mouse.GetYPos() + 1;
			if (val1 < 1)
				val1 = 1;
			if (val1 > g_renderer->m_width - menu->GetWidth())
				val1 = g_renderer->m_width - menu->GetWidth();
			if (val2 < 1)
				val2 = 1;
			if (val2 > g_renderer->m_height - menu->GetHeight())
				val2 = g_renderer->m_height - menu->GetHeight();
			menu->Move(val1, val2);
		}
	}
		return 0;

	case UM_SETINFO: {
		char title[256];
		ChatRoomInfo* info = (ChatRoomInfo*)val1;
m_roomInfo = *info;
		std::string typeString;
		if (m_roomInfo.roomType == 0)
			typeString = MsgStr(MSI_ROOM_PRIVATE);
		else
			typeString = MsgStr(MSI_ROOM_PUBLIC);

		sprintf(title, "%s%s (%d/%d)", typeString.c_str(), m_roomInfo.title.c_str(),
			m_roomInfo.numPeople, m_roomInfo.maxNumPeople);
		m_title->SetTextWithShorten(title);
		Invalidate();
	}
		return 0;

	case UM_ROLE_CHANGE: {
		std::map<std::string, unsigned long>::iterator it = m_memberList.find((const char*)val1);
		if (it != m_memberList.end())
			m_memberList[(const char*)val1] = val2;

		m_memberListBox->ClearAllItems();
		it = m_memberList.begin();
		while (it != m_memberList.end()) {
			unsigned long color = 0;
			if (it->second == 0)
				color = 0x808000;
			m_memberListBox->AddItem(it->first.c_str(), color, NULL);
			++it;
		}
		return 0;
	}

	case UM_MEMBER_EXIT: {
		std::map<std::string, unsigned long>::iterator it = m_memberList.find((const char*)val1);
		if (it != m_memberList.end()) {
			m_memberList.erase(it);

			m_memberListBox->ClearAllItems();
			it = m_memberList.begin();
			while (it != m_memberList.end()) {
				unsigned long color = 0;
				if (it->second == 0)
					color = 0x808000;
				m_memberListBox->AddItem(it->first.c_str(), color, NULL);
				++it;
			}
		}
		return 0;
	}

	case UM_CLEARMEMBER:
		m_memberListBox->ClearAllItems();
		m_memberList.erase(m_memberList.begin(), m_memberList.end());
		return 0;

	case UM_ADDMEMBER: {
		m_memberList[(const char*)val1] = val2;
		unsigned long color = 0;
		if (val2 == 0)
			color = 0x808000;
		else
			PlayWave("\xb9\xf6\xc6\xb0\xbc\xd2\xb8\xae.wav", 0.0f, 0.0f, 0.0f, 250, 40, 1.0f);
		m_memberListBox->AddItem((const char*)val1, color, NULL);
		return 0;
	}

	case UM_ADDCHAT: {
		std::vector<std::string> strings;
		const int maxNumCharLine = (m_w * 80) / 640;
		char talkerName[25] = { NULL, };
		{
		std::string wMsg = (const char*)val1;
		int len = strlen((const char*)val1);
		int enterPos = wMsg.find(") has entered.");
		int exitPos = wMsg.find(") has left.");
		int colonPos = wMsg.find(" :");
		if (enterPos > 0 && enterPos < len) {
			memcpy(talkerName, (const char*)val1 + 1, enterPos - 1);
			talkerName[enterPos - 1] = 0;
		} else if (exitPos > 0 && exitPos < len) {
			memcpy(talkerName, (const char*)val1 + 1, exitPos - 1);
			talkerName[exitPos - 1] = 0;
		} else if (colonPos > 0 && colonPos < len) {
			memcpy(talkerName, (const char*)val1, colonPos);
			talkerName[colonPos] = 0;
		}

		if (strcmp(talkerName, g_session.GetCharName()) == 0)
			talkerName[0] = 0;
		}
		CutString((const char*)val1, strings, maxNumCharLine);
		for (int i = 0; i < strings.size(); ++i) {
			m_listBox->AddItem(strings[i].c_str(), val2, talkerName);
		}
	}
		return 0;

	case UM_PAGEUPPUSH:
		if (m_listBox->IsVertScrEnabled())
			m_listBox->SendMsg(NULL, UM_SCROLLVERTPAGEUP, 0, 0, 0, 0);
		return 0;

	case UM_PAGEDOWNPUSH:
		if (m_listBox->IsVertScrEnabled())
			m_listBox->SendMsg(NULL, UM_SCROLLVERTPAGEDOWN, 0, 0, 0, 0);
		return 0;

	case UM_RESIZE: {
		if (val1 != 7) return 0;
		int w = val2;
		int h = val3;
		if (w > 512)
			w = 512;
		if (h > 384)
			h = 384;
		if (w < 280)
			w = 280;
		if (h < 120)
			h = 120;
		w = ((w - 280) / 32) * 32 + 280;
		h = ((h - 120) / 32) * 32 + 120;
		if (w != m_w || h != m_h) {
			Resize(w, h);
			m_resizer->Move(m_w - 15, m_h - 15);
			m_chatEdit->Resize(m_w - 30, 16);
			m_chatEdit->Move(3, m_h - 18);
			m_listBox->Resize(m_w - (m_w * 56) / 280 - 3, m_h - 38);
			m_listBox->Move(1, 17);
			m_memberListBox->Resize((m_w * 56) / 280, m_h - 38);
			m_memberListBox->Move(m_w - (m_w * 56) / 280 - 1, 17);
			m_sysButton->Move(m_w - 14, 3);
			std::string title = m_title->GetFullText();
			m_title->Resize(m_w - 30, 12);
			m_title->SetTextWithShorten(title.c_str());

		}
		g_windowMgr.UpdateSnapInfo(this);
		return 0;
	}
	default: return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}



UIAllyGuildAcceptWnd::UIAllyGuildAcceptWnd()
{
}


UIAllyGuildAcceptWnd::~UIAllyGuildAcceptWnd()
{
}


int UIAllyGuildAcceptWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_ALLY_GUILD_MSG:
		{
			m_naid = val1;
			std::string msg = "(";
			msg += (const char*)val2;
			msg += ")";
			msg += MsgStr(MSI_SUGGEST_ALLY_GUILD);

			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), 0x26, 0, 0);
		}
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_ALLYGUILDACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_ALLY_GUILD, m_naid, 1, 0);
			break;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_ALLYGUILDACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_ALLY_GUILD, m_naid, 0, 0);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIMonsterInfoWnd::UIMonsterInfoWnd()
{
	memset(&m_zc_monster_info, 0, sizeof(m_zc_monster_info));
}


UIMonsterInfoWnd::~UIMonsterInfoWnd() {}

void UIMonsterInfoWnd::OnCreate(int cx, int cy)
{
	std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

	UIBitmapButton* button = new UIBitmapButton;
	button->SetBitmapName((pathName + "btn_ok" + ".bmp").c_str(), 0);
	button->SetBitmapName((pathName + "btn_ok_a" + ".bmp").c_str(), 1);
	button->SetBitmapName((pathName + "btn_ok_b" + ".bmp").c_str(), 2);
	button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
	button->Move(m_w - 46, m_h - 24);
	button->SetId(ID_OK);
	AddChild(button);
	m_defPushId = 0x76;
}

void UIMonsterInfoWnd::OnDraw()
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	char actName[128];
	CTexture* tex;
	CSprClip* clip;
	int half_H;

	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	unsigned long surfaceColor = RGB(rr, gg, bb);
	ClearDC(surfaceColor);

	int titleBarMidCount = ((m_w - 24) % 12 != 0) + (m_w - 24) / 12;

	int i;
	for (i = 0; i < titleBarMidCount; i++) {
		CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	g_skinMgr.GetColorChipColor(20, 6, rr, gg, bb);
	DrawBox(0, 17, 1, m_h - 38, RGB(rr, gg, bb));
	DrawBox(m_w - 1, 17, 1, m_h - 38, RGB(rr, gg, bb));

	int btnBarMidCount = ((m_w - 42) % 21 != 0) + (m_w - 42) / 21;

	for (i = 0; i < btnBarMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	g_skinMgr.GetColorChipColor(22, 2, rr, gg, bb);
	unsigned long borderColor = RGB(rr, gg, bb);

	TextOutA(17, 3, MsgStr(MSI_MONSTER_INFO_WINDOW), 0, 0, 12, 0xffffff);
	TextOutA(16, 2, MsgStr(MSI_MONSTER_INFO_WINDOW), 0, 0, 12, 0);

	TextOutA(120, 30, MsgStr(MSI_NAME), 0, 0, 12, 0);
	TextOutA(160, 30, "Monster", 0, 0, 12, 0);

	{
	char chBuffer[256];
	sprintf(chBuffer, "%d", m_zc_monster_info.level);
	TextOutA(120, 50, MsgStr(MSI_LEVEL), 0, 0, 12, 0);
	TextOutA(160, 50, chBuffer, 0, 0, 12, 0);

	sprintf(chBuffer, "%d", m_zc_monster_info.hp);
	TextOutA(120, 70, MsgStr(MSI_HP), 0, 0, 12, 0);
	TextOutA(160, 70, chBuffer, 0, 0, 12, 0);

	sprintf(chBuffer, "%d", m_zc_monster_info.def);
	TextOutA(120, 90, MsgStr(MSI_DEF), 0, 0, 12, 0);
	TextOutA(160, 90, chBuffer, 0, 0, 12, 0);

	TextOutA(260, 30, MsgStr(MSI_SIZE), 0, 0, 12, 0);
	switch (m_zc_monster_info.size) {
	case 0:
		TextOutA(300, 30, MsgStr(MSI_SIZE_SMALL), 0, 0, 12, 0);
		break;
	case 1:
		TextOutA(300, 30, MsgStr(MSI_SIZE_MIDDLE), 0, 0, 12, 0);
		break;
	case 2:
		TextOutA(300, 30, MsgStr(MSI_SIZE_BIG), 0, 0, 12, 0);
		break;
	}

	sprintf(chBuffer, "%d", m_zc_monster_info.raceType);
	TextOutA(260, 50, MsgStr(MSI_RACETYPE), 0, 0, 12, 0);
	if (m_zc_monster_info.raceType == 0)
		TextOutA(300, 50, "Unknown", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 1)
		TextOutA(300, 50, "Undead", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 2)
		TextOutA(300, 50, "Animal", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 3)
		TextOutA(300, 50, "Plant", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 4)
		TextOutA(300, 50, "Insect", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 5)
		TextOutA(300, 50, "Marine", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 6)
		TextOutA(300, 50, "Devil", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 7)
		TextOutA(300, 50, "Human", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 8)
		TextOutA(300, 50, "Angel", 0, 0, 12, 0);
	else if (m_zc_monster_info.raceType == 9)
		TextOutA(300, 50, "Dragon", 0, 0, 12, 0);

	sprintf(chBuffer, "%d", m_zc_monster_info.mdefPower);
	TextOutA(260, 70, MsgStr(MSI_MDEFPOWER), 0, 0, 12, 0);
	TextOutA(300, 70, chBuffer, 0, 0, 12, 0);
	}

	TextOutA(260, 90, MsgStr(MSI_PROPERTY), 0, 0, 12, 0);
	switch (m_zc_monster_info.property % 20) {
	case 0:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_NEUTURAL), 0, 0, 12, 0);
		break;
	case 1:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_WATER), 0, 0, 12, 0);
		break;
	case 2:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_EARTH), 0, 0, 12, 0);
		break;
	case 3:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_FIRE), 0, 0, 12, 0);
		break;
	case 4:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_WIND), 0, 0, 12, 0);
		break;
	case 5:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_POISON), 0, 0, 12, 0);
		break;
	case 6:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_SAINT), 0, 0, 12, 0);
		break;
	case 7:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_DARK), 0, 0, 12, 0);
		break;
	case 8:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_MENTAL), 0, 0, 12, 0);
		break;
	case 9:
		TextOutA(300, 90, MsgStr(MSI_PROPERTY_UNDEAD), 0, 0, 12, 0);
		break;
	}

	DrawPropertyBox(120, 110, MsgStr(MSI_PROPERTY_WATER), m_zc_monster_info.propertyTable.water, borderColor, surfaceColor);
	DrawPropertyBox(120, 128, MsgStr(MSI_PROPERTY_EARTH), m_zc_monster_info.propertyTable.earth, borderColor, surfaceColor);
	DrawPropertyBox(120, 146, MsgStr(MSI_PROPERTY_FIRE), m_zc_monster_info.propertyTable.fire, borderColor, surfaceColor);
	DrawPropertyBox(200, 110, MsgStr(MSI_PROPERTY_WIND), m_zc_monster_info.propertyTable.wind, borderColor, surfaceColor);
	DrawPropertyBox(200, 128, MsgStr(MSI_PROPERTY_POISON), m_zc_monster_info.propertyTable.poison, borderColor, surfaceColor);
	DrawPropertyBox(200, 146, MsgStr(MSI_PROPERTY_SAINT), m_zc_monster_info.propertyTable.saint, borderColor, surfaceColor);
	DrawPropertyBox(280, 110, MsgStr(MSI_PROPERTY_DARK), m_zc_monster_info.propertyTable.dark, borderColor, surfaceColor);
	DrawPropertyBox(280, 128, MsgStr(MSI_PROPERTY_MENTAL), m_zc_monster_info.propertyTable.mental, borderColor, surfaceColor);
	DrawPropertyBox(280, 146, MsgStr(MSI_PROPERTY_UNDEAD), m_zc_monster_info.propertyTable.undead, borderColor, surfaceColor);

	{
	char sprName[128];
	CacheInfo info;
	sprintf(sprName, "\xb8\xf3\xbd\xba\xc5\xcd\\%s.spr", g_session.m_jobNameTable[m_zc_monster_info.job]);
	sprintf(actName, "\xb8\xf3\xbd\xba\xc5\xcd\\%s.act", g_session.m_jobNameTable[m_zc_monster_info.job]);

	if (g_resMgr().IsExist(actName) && g_resMgr().IsExist(sprName)) {
		CSprRes* sprRes;
		CActRes* actRes;
		actRes = (CActRes*)g_resMgr().Get(actName, false);
		sprRes = (CSprRes*)g_resMgr().Get(sprName, false);
		if (actRes && sprRes) {
			CMotion* motion = actRes->GetMotion(0, 0);
			unsigned long* palette = sprRes->m_pal;
			clip = motion->GetClip(0);
			if (clip->sprIndex != -1) {
				SprImg* img = sprRes->m_sprites[clip->clipType][clip->sprIndex];
				if (clip->clipType == 0) {
					tex = g_renderer->GetSpriteIndex(*img, (unsigned long)palette, &info);
					if (tex == NULL)
						tex = g_renderer->AddSpriteIndex(*img, palette, &info);
				}
				else {
					tex = img->tex;
				}

				if (img->width < 200 && img->height < 200) {
					m_dc->BltTexture2(60 - img->width / (2 - img->isHalfW),
						102 - img->height / (2 - img->isHalfH), tex,
						(int)(info.tu * 256.0f), (int)(info.tv * 256.0f),
						img->width, img->height, clip->flags, img->isHalfW + 1, img->isHalfH + 1);
				}
				else {
					int half_W = img->width / 2;
					half_H = img->height / 2;
					m_dc->BltTexture2(60 - half_W / (2 - img->isHalfW),
						102 - half_H / (2 - img->isHalfH), tex,
						(int)(info.tu * 256.0f), (int)(info.tv * 256.0f),
						half_W, half_H, clip->flags, img->isHalfW + 1, img->isHalfH + 1);
				}
			}
		}
	}
	}
}

void UIMonsterInfoWnd::DrawPropertyBox(int x, int y, char const *propertyString, int power, unsigned long borderColor, unsigned long surfaceColor)
{
	char propertyText[64];
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	wsprintf(propertyText, "%s:%d", propertyString, power);
	g_skinMgr.GetColorChipColor(22, 2, rr, gg, bb);

	DrawBox(x, y, 70, 16, borderColor);
	DrawBox(x + 1, y + 1, 68, 14, surfaceColor);
	TextOutA(x + (70 - GetTextWidth(propertyText, 0, 0, 12, false)) / 2, y + 2, propertyText, 0, 0, 12, RGB(rr, bb, gg));
}

int UIMonsterInfoWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		if (val1 == 0x76) {
			g_windowMgr.PostQuit(this);
		}
		break;
	case UM_SET_MONSTER_INFO:
		if (val1) memcpy(&m_zc_monster_info, (void*)val1, sizeof(m_zc_monster_info));
		Invalidate();
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIPetInfoWnd::UIPetInfoWnd()
{
}


UIPetInfoWnd::~UIPetInfoWnd() {}
void UIPetInfoWnd::OnCreate(int cx, int cy)
{
	{
		int posX = m_w - 14;
		char buttonName[] = "close";
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
		char toolTip[] = "Close";

		UIBitmapButton* closeButton = new UIBitmapButton;
		closeButton->SetBitmapName((preBmpName + buttonName + "_off.bmp").c_str(), 0);
		closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 1);
		closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 2);
		closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
		closeButton->Move(posX, 3);
		closeButton->SetId(ID_CLOSE);
		closeButton->SetToolTip(toolTip);
		AddChild(closeButton);
	}

	m_newName = new UIEditCtrl2;
	m_newName->Create(80, 16);
	if (g_session.m_petJob == -1) {
		m_newName->Move(-100, -100);
	} else {
		if (strcmp(g_session.m_petName, "") == 0)
			sprintf(g_session.m_petName, g_session.GetJobName(g_session.m_petJob));

		if (g_session.m_isPetNameModified == 0)
			m_newName->Move(135, 24);
		else
			m_newName->Move(-100, -100);

		m_newName->SetText(g_session.m_petName);
	}
	m_newName->SetTextLimit(50);
	m_newName->SetFrameColor(0xe8, 0xe8, 0xe8);
	AddChild(m_newName);

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";

		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + "btn_rewrite" + ".bmp").c_str(), 0);
		m_button->SetBitmapName((pathName + "btn_rewrite_a" + ".bmp").c_str(), 1);
		m_button->SetBitmapName((pathName + "btn_rewrite_b" + ".bmp").c_str(), 2);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		if (g_session.m_isPetNameModified == 0)
			m_button->Move(230, 21);
		else
			m_button->Move(-100, -100);
		m_button->SetId(ID_OK);
		AddChild(m_button);
	}

	{
		char toolTip[] = "Pet Command";

		m_skinSelectButton = new UINoneVisibleButton;
		m_skinSelectButton->Create(120, 18);
		m_skinSelectButton->Move(150, 44);
		m_skinSelectButton->SetId(ID_SKINSELECTCOMBO);
		AddChild(m_skinSelectButton);

		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\";

		UIBitmapButton* dropButton = new UIBitmapButton;
		dropButton->SetBitmapName((pathName + "txtbox_btn_a" + ".bmp").c_str(), 0);
		dropButton->SetBitmapName((pathName + "txtbox_btn_b" + ".bmp").c_str(), 1);
		dropButton->SetBitmapName((pathName + "txtbox_btn_c" + ".bmp").c_str(), 2);
		dropButton->Create(dropButton->GetBitmapWidth(), dropButton->GetBitmapHeight());
		dropButton->Move(270 - dropButton->GetBitmapWidth(), 44);
		dropButton->SetToolTip(toolTip);
		dropButton->SetId(ID_SKINSELECTCOMBO);
		AddChild(dropButton);
	}

	m_defPushId = 0x87;
}

void UIPetInfoWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

void UIPetInfoWnd::OnDraw()
{
	CSprRes* sprRes;
	CTexture* tex;
	DWORD gg;
	char stLevel[8];
	char stFriendly[24];
	CMotion* curMotion;
	char windowName[32];
	DWORD rr;
	char stHungry[24];
	unsigned long* palette;
	DWORD bb;
	char sprName[128];
	char actName[128];
	char petIllustName[256];

	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	ClearDC(RGB(rr, gg, bb));

	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bitmap, false);

	CBitmapRes* right = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - right->m_width, 0, right, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int i = 0; i < titleMidCount; i++) {
		CBitmapRes* mid = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(12 + i * 12, 0, mid, false);
	}

	g_skinMgr.GetColorChipColor(20, 6, rr, gg, bb);
	DrawBox(0, 17, 1, m_h - 38, RGB(rr, gg, bb));
	DrawBox(m_w - 1, 17, 1, m_h - 38, RGB(rr, gg, bb));

	char* bmpName[3] = {
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysboxs_ld.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysboxs_rd.bmp"
	};
	CBitmapRes* ld = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[0]), false);
	DrawBitmap(0, m_h - ld->m_height, ld, false);

	CBitmapRes* rd = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[1]), false);
	DrawBitmap(m_w - rd->m_width, m_h - rd->m_height, rd, false);

	if (g_session.m_isPetNameModified) {
		TextOutA(135, 24, g_session.m_petName, 0, 0, 12, 0xff00);
		TextOutA(136, 24, g_session.m_petName, 0, 0, 12, 0x1e1e1e);
	}

	sprintf(windowName, MsgStr(MSI_PET_INFO));
	TextOutA(5, 2, windowName, 0, 0, 12, 0);

	TextOutA(100, 24, MsgStr(MSI_NAME), 0, 0, 12, 0);
	TextOutA(100, 44, MsgStr(MSI_LEVEL), 0, 0, 12, 0);
	TextOutA(100, 69, MsgStr(MSI_PET_HUNGRY), 0, 0, 12, 0);
	TextOutA(100, 89, MsgStr(MSI_PET_FRIENDLY), 0, 0, 12, 0);

	if (g_session.m_petHungry >= 0 && g_session.m_petHungry <= 10)
		strcpy(stHungry, MsgStr(MSI_VERY_HUNGRY));
	else if (g_session.m_petHungry > 10 && g_session.m_petHungry <= 25)
		strcpy(stHungry, MsgStr(MSI_HUNGRY));
	else if (g_session.m_petHungry > 25 && g_session.m_petHungry <= 75)
		strcpy(stHungry, MsgStr(MSI_NORMAL));
	else if (g_session.m_petHungry > 75 && g_session.m_petHungry <= 90)
		strcpy(stHungry, MsgStr(MSI_REPLETE));
	else if (g_session.m_petHungry > 90 && g_session.m_petHungry <= 100)
		strcpy(stHungry, MsgStr(MSI_VERY_REPLETE));
	else
		strcpy(stHungry, MsgStr(MSI_UNKNOWN));

	if (g_session.m_petFriendly >= 0 && g_session.m_petFriendly <= 100) {
		if (g_session.GetJob() >= 4050 && g_session.GetJob() <= 4052) {
			if (g_session.m_petFriendly <= 20)
				strcpy(stFriendly, MsgStr(MSI_VERYHATE));
			else if (g_session.m_petFriendly <= 40)
				strcpy(stFriendly, MsgStr(MSI_HATE));
			else
				strcpy(stFriendly, MsgStr(MSI_VERY_AWKWARD));
		}
		else {
			strcpy(stFriendly, MsgStr(MSI_VERY_AWKWARD));
		}
	}
	else if (g_session.m_petFriendly > 100 && g_session.m_petFriendly <= 250)
		strcpy(stFriendly, MsgStr(MSI_AWKWARD));
	else if (g_session.m_petFriendly > 250 && g_session.m_petFriendly <= 750)
		strcpy(stFriendly, MsgStr(MSI_NORMAL));
	else if (g_session.m_petFriendly > 750 && g_session.m_petFriendly <= 900)
		strcpy(stFriendly, MsgStr(MSI_FRIENDLY));
	else if (g_session.m_petFriendly > 900 && g_session.m_petFriendly <= 1000)
		strcpy(stFriendly, MsgStr(MSI_VERY_FRIENDLY));
	else
		strcpy(stFriendly, MsgStr(MSI_UNKNOWN));

	itoa(g_session.m_petLevel, stLevel, 10);
	TextOutA(135, 44, stLevel, 0, 0, 12, 0);
	TextOutA(145, 69, stHungry, 0, 0, 12, 0);
	TextOutA(145, 89, stFriendly, 0, 0, 12, 0);

	if (g_session.m_petJob == -1)
		return;

	CacheInfo info;
	if (g_session.m_petJob >= 1000 && g_session.m_petJob < 1986) {
		sprintf(sprName, "\xb8\xf3\xbd\xba\xc5\xcd\\%s.spr", g_session.m_jobNameTable[g_session.m_petJob]);
		sprintf(actName, "\xb8\xf3\xbd\xba\xc5\xcd\\%s.act", g_session.m_jobNameTable[g_session.m_petJob]);

		CActRes* actRes = (CActRes*)g_resMgr().Get(actName, false);
		sprRes = (CSprRes*)g_resMgr().Get(sprName, false);
		curMotion = actRes->GetMotion(0, 0);

		char txt[] = "< Command >";
		DrawBox(160, 44, 110, 18, 0xfae6e6);
		TextOutA(206 - GetTextWidth(txt, 0, 0, 12, false) / 2, 48, txt, 0, 0, 12, 0);

		g_session.GetPetIllustName(g_session.m_petJob, g_session.m_petAcc, petIllustName);
		if (g_resMgr().IsExist(petIllustName)) {
			CBitmapRes* illust = (CBitmapRes*)g_resMgr().Get(petIllustName, false);
			if (illust)
				DrawBitmap(5, 21, illust, false);
		}
		else {
			int alignx = 0;
			int aligny = 0;
			int srcx = 0;
			int srcy = 0;

			for (int a = 0; a < curMotion->numClips; a++) {
				int destx = 0;
				int desty = 0;
				CSprClip* clip = curMotion->GetClip(a);
				SprImg* img = sprRes->m_sprites[clip->clipType][clip->sprIndex];
				palette = sprRes->m_pal;
				tex = g_renderer->GetSpriteIndex(*img, (unsigned long)palette, &info);
				if (tex == NULL)
					tex = g_renderer->AddSpriteIndex(*img, palette, &info);

				int w = img->width;
				int h = img->height;
				if (a == 0) {
					alignx = -clip->x;
					aligny = -clip->y;
					srcx = (90 - w) / 2;
					srcy = (134 - h) / 2;
					destx = srcx;
					desty = srcy;
				}
				else {
					destx = srcx + alignx + clip->x;
					desty = clip->y + srcy + aligny;
				}

				if (destx < 0)
					destx = 0;
				if (w + destx > 90)
					w = 90 - destx;
				if (desty < 0)
					desty = 0;
				if (h + desty > 155)
					h = 155 - desty;

				m_dc->BltTexture2(destx + 5, desty + 21, tex,
					(int)(info.tu * 256.0f), (int)(info.tv * 256.0f),
					w, h, clip->flags, img->isHalfW + 1, img->isHalfH + 1);
			}
		}
		TextOutA(100, 119, MsgStr(MSI_PET_ACCESSARY), 0, 0, 12, 0);
		if (g_session.m_petAcc != 0)
			TextOutA(100, 139, MsgStr(MSI_ITEM_EQUIPED), 0, 0, 12, 0);
		else
			TextOutA(100, 139, MsgStr(MSI_ITEM_UNEQUIPED), 0, 0, 12, 0);
	}
}

int UIPetInfoWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_DEFPUSH:
		return 0;
	case UM_COMBOBOX_SELECTED: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		switch (val1) {
		case 0:
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_SURE_TO_FEED_PET), ME_YESNO, 0, 0, 0) == ID_YES)
				gameMode->SendMsg(MM_COMMAND_PET, 1, 0, 0);
			return 0;
		case 1:
			gameMode->SendMsg(MM_COMMAND_PET, 2, 0, 0);
			return 0;
		case 2:
			gameMode->SendMsg(MM_COMMAND_PET, 4, 0, 0);
			return 0;
		case 3: {
			gameMode->SendMsg(MM_COMMAND_PET, 3, 0, 0);
			ITEM_INFO info = g_session.GetItemInfoByIndex(g_session.m_petEggIndex);
			info.m_isDamaged = false;
			g_session.SetItem(&info);
			g_session.m_petEggIndex = -1;
			g_session.m_petJob = -1;
			UIItemWnd* itemWnd = g_windowMgr.GetItemWnd();
			if (itemWnd)
				itemWnd->Invalidate();
			g_windowMgr.DeleteWindow(WID_PETINFOWND);
		}
			return 0;
		}
		break;
	}
	case UM_COMMAND:
		switch (val1) {
		case ID_SKINSELECTCOMBO: {
			UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
			std::string feeding, perfomance, turnegg, accOff;

			feeding = MsgStr(MSI_PET_FEEDING);
			perfomance = MsgStr(MSI_PET_PERFORMANCE);
			turnegg = MsgStr(MSI_PET_RETURN_EGG);
			accOff = MsgStr(MSI_PET_ACC_OFF);

			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)feeding.c_str(), 0, 0, 0);
			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)perfomance.c_str(), 0, 0, 0);
			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)accOff.c_str(), 0, 0, 0);
			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)turnegg.c_str(), 0, 0, 0);
			comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			comboBoxWnd->Resize(110, 64);
			comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_PETINFOWND, 0, 0, 0);

			int gx = 160;
			int gy = 44;
			GetGlobalCoor(gx, gy);
			comboBoxWnd->Move(gx, gy);
			return 0;
		}
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_PETINFOWND);
			return 0;
		case ID_OK: {
			char msg[512];
			if (strlen(m_newName->GetText()) >= 24) {
				g_windowMgr.ErrorMsg(MsgStr(MSI_PETNAME_IN23), ME_OK, 1, 0, 0);
				return 0;
			}
			sprintf(msg, MsgStr(MSI_PETNAME_CHANGE_ONLYONCE), m_newName->GetText());
			MsgStr(MSI_PETNAME_CHANGE_ONLYONCE);
			int result = g_windowMgr.ErrorMsg(msg, ME_YESNO, 0, 0, 0);
			if (result != ID_NO) {
				g_modeMgr.GetGameMode()->SendMsg(MM_PET_RENAME, (int)m_newName->GetText(), 0, 0);
				g_session.m_isPetNameModified = 1;
				sprintf(g_session.m_petName, m_newName->GetText());
				m_button->Move(-100, -100);
				m_newName->Move(-100, -100);
				Invalidate();
			}
			return 0;
		}
		}
		break;
	case UM_SETINFO:
		m_info = (PetInfoWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		return 0;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UITalkboxTrapInputWnd::UITalkboxTrapInputWnd()
{
}


UITalkboxTrapInputWnd::~UITalkboxTrapInputWnd() {}

void UITalkboxTrapInputWnd::OnCreate(int cx, int cy)
{
	m_trapText = new UIEditCtrl;
	m_trapText->Create(240, 16);
	m_trapText->Move(22, 50);
	m_trapText->SetTextLimit(78);
	m_trapText->SetFrameColor(0xf2, 0xf2, 0xf2);
	AddChild(m_trapText);
	g_windowMgr.SetFocusEdit(m_trapText);

	std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
	char* buttonName[2][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" }
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ 189, 96 },
		{ 234, 96 }
	};

	int i;
	for (i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), 0);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), 1);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), 2);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = 0x76;
}

int UITalkboxTrapInputWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	int values[5];

	switch (message) {
	case UM_SET_SKILL_INFO: {
		int *recvValues = (int *)val1;
		m_skillId = recvValues[0];
		m_gSkillDx = recvValues[1];
		m_gSkillDy = recvValues[2];
		m_skillUseLevel = recvValues[3];
		break;
	}
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			values[0] = m_skillId;
			values[1] = m_gSkillDx;
			values[2] = m_gSkillDy;
			values[3] = m_skillUseLevel;
			values[4] = (int)m_trapText->GetText();
			g_modeMgr.GetGameMode()->SendMsg(MM_USE_SKILL_TOGROUND_WITH_TALKBOX, (int)values, 0, 0);

		case ID_CANCEL:
			g_windowMgr.PostQuit(this);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UITalkboxTrapInputWnd::OnDraw()
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	ClearDC(RGB(rr, gg, bb));

	int titleBarMidCount = ((m_w - 24) % 12 != 0) + (m_w - 24) / 12;

	int i;
	for (i = 0; i < titleBarMidCount; i++) {
		CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	g_skinMgr.GetColorChipColor(20, 6, rr, gg, bb);
	DrawBox(0, 17, 1, m_h - 38, RGB(rr, gg, bb));
	DrawBox(m_w - 1, 17, 1, m_h - 38, RGB(rr, gg, bb));

	int btnBarMidCount = ((m_w - 42) % 21 != 0) + (m_w - 42) / 21;

	for (i = 0; i < btnBarMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	TextOutA(17, 3, MsgStr(MSI_TALKBOX_WINDOW), 0, 0, 12, 0xffffff);
	TextOutA(16, 2, MsgStr(MSI_TALKBOX_WINDOW), 0, 0, 12, 0);
	TextOutA(17, 23, MsgStr(MSI_TALKBOX_WINDOW_MSG), 0, 0, 12, 0xffffff);
	TextOutA(16, 22, MsgStr(MSI_TALKBOX_WINDOW_MSG), 0, 0, 12, 0);
}

UICntWnd::UICntWnd()
{
}


UICntWnd::~UICntWnd() {}

void UICntWnd::OnCreate(int cx, int cy)
{
	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(80, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(6);
	m_editCtrl->SetFrameColor(0xe8, 0xe8, 0xe8);
	AddChild(m_editCtrl);
	g_windowMgr.SetFocusEdit(m_editCtrl);
	m_editCtrl->MakeSelect();

	m_newText = new UIStaticText;
	m_newText->Create(m_w - 30, 15);
	m_newText->Move(15, 6);
	AddChild(m_newText);

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		int pos[1][2] = {
			{ m_w - 55, m_h - 32 }
		};

		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + "btn_ok" + ".bmp").c_str(), 0);
		button->SetBitmapName((pathName + "btn_ok_a" + ".bmp").c_str(), 1);
		button->SetBitmapName((pathName + "btn_ok_b" + ".bmp").c_str(), 2);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[0][0], pos[0][1]);
		button->SetId(ID_OK);
		AddChild(button);
	}
	m_defPushId = 0x76;
}

int UICntWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	char temp[16];

	switch (message) {
	case UM_SETTEXT:
		m_newText->SetText((const char*)val1, 0);
		return 0;
	case UM_SET_COUNT:
		m_editCtrl->SetText(itoa(val1, temp, 10));
		return 0;
	case UM_SET_TARGET:
		m_target = val1;
		return 0;
	case UM_COMMAND:
		if (val1 == 0x76)
			g_modeMgr.GetCurMode()->SendMsg(m_target, atoi(m_editCtrl->GetText()), 0, 0);
		g_windowMgr.PostQuit(this);
		return 0;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
}

void UICntWnd::OnDraw()
{
	DrawLatticeFrame(0, 0, 0, 0);
}

UIHomunInfoWnd::UIHomunInfoWnd()
{
}


UIHomunInfoWnd::~UIHomunInfoWnd() {}

#include "rag_budget_sites.h"
void UIHomunInfoWnd::OnCreate(int cx, int cy)
{
	{
		int posX = m_w - 14;
		char buttonName[] = "close";
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
		char toolTip[] = "Close";

		UIBitmapButton* closeButton = new UIBitmapButton;
		closeButton->SetBitmapName((preBmpName + buttonName + "_off.bmp").c_str(), 0);
		closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 1);
		closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 2);
		closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
		closeButton->Move(posX, 3);
		closeButton->SetId(ID_CLOSE);
		closeButton->SetToolTip(toolTip);
		AddChild(closeButton);
	}

	m_newName = new UIEditCtrl2;
	m_newName->Create(80, 16);
	if (g_session.m_homunJob == -1) {
		m_newName->Move(-100, -100);
	} else {
		if (strcmp(g_session.m_homunName, "") == 0)
			sprintf(g_session.m_homunName, g_session.GetJobName(g_session.m_homunJob));

		if (!(g_session.m_isHomunNameModified & 1))
			m_newName->Move(135, 22);
		else
			m_newName->Move(-100, -100);

		m_newName->SetText(g_session.m_homunName);
	}
	m_newName->SetTextLimit(50);
	m_newName->SetFrameColor(0xe8, 0xe8, 0xe8);
	AddChild(m_newName);

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_del";
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), 0);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
		m_button->SetBitmapName((pathName + "_b.bmp").c_str(), 2);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		m_button->Move(187, 44);
		m_button->SetId(ID_DELETE_HOMUN);
		AddChild(m_button);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_skill";
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), 0);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
		m_button->SetBitmapName((pathName + "_b.bmp").c_str(), 2);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		m_button->Move(232, 44);
		m_button->SetId(ID_OPEN_HOMUNSKILLLIST);
		AddChild(m_button);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_feed"; RAG_BUDGET_SITES(1)
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), 0);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
		m_button->SetBitmapName((pathName + "_b.bmp").c_str(), 2);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		m_button->Move(210, 155);
		m_button->SetId(ID_FEED_TO_HOMUN);
		AddChild(m_button);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_rewrite";
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), 0);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
		m_button->SetBitmapName((pathName + "_b.bmp").c_str(), 2);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		if (!(g_session.m_isHomunNameModified & 1))
			m_button->Move(232, 21);
		else
			m_button->Move(-100, -100);
		m_button->SetId(ID_OK);
		AddChild(m_button);
	}

	m_hpGraph = new UIBarGraphPlayerHp;
	m_hpGraph->m_isPlayerType = 1;
	m_hpGraph->Create(85, 9);
	m_hpGraph->Move(117, 73);
	AddChild(m_hpGraph);

	m_spGraph = new UIBarGraphPlayerSp;
	m_spGraph->m_isPlayerType = 1;
	m_spGraph->Create(85, 9);
	m_spGraph->Move(117, 88);
	AddChild(m_spGraph);

	m_expGraph = new UIBarGraph4;
	m_expGraph->m_bType = 1;
	m_expGraph->Create(102, 6);
	m_expGraph->Move(101, 119);
	m_expGraph->SetAmount(&g_session.m_homunExp, &g_session.m_homunNextExp);
	AddChild(m_expGraph);

	m_hungryGraph = new UIBarGraph4;
	m_hungryGraph->m_bType = 0;
	m_hungryGraph->Create(102, 6);
	m_hungryGraph->Move(101, 145);
	m_hungryGraph->SetAmount(&g_session.m_homunHungry, &g_session.m_homunMaxHungry);
	AddChild(m_hungryGraph);

	m_defPushId = 0x87;
}

void UIHomunInfoWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

void UIHomunInfoWnd::OnDraw()
{
	char itemName[64];
	char windowName[32];
	char stLevel[8];
	char text[12];
	int textLen;

	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\homuninfo_bg.bmp"), false);
	DrawBitmap(0, 0, bitmap, false);

	sprintf(windowName, MsgStr(MSI_HOMUN_INFO));
	TextOutA(5, 2, windowName, 0, 0, 12, 0);

	TextOutA(100, 24, MsgStr(MSI_NAME), 0, 0, 12, 0);
	TextOutA(100, 47, MsgStr(MSI_LEVEL), 0, 0, 12, 0);
	TextOutA(100, 105, MsgStr(MSI_HOMUN_EXP), 0, 0, 12, 0);
	TextOutA(216, 105, MsgStr(MSI_PET_ACCESSARY), 0, 0, 12, 0);
	TextOutA(100, 131, MsgStr(MSI_PET_HUNGRY), 0, 0, 12, 0);
	TextOutA(100, 159, MsgStr(MSI_PET_FRIENDLY), 0, 0, 12, 0);
	TextOutA(140, 159, ":", 0, 0, 12, 0);

	textLen = sprintf(text, "%d", g_session.m_homunAtk);
	TextOutWithRightAlign(79, 24, text, textLen, 1, 14, 0);
	textLen = sprintf(text, "%d", g_session.m_homunMatk);
	TextOutWithRightAlign(79, 42, text, textLen, 1, 14, 0);
	textLen = sprintf(text, "%d", g_session.m_homunHit);
	TextOutWithRightAlign(79, 60, text, textLen, 1, 14, 0);
	textLen = sprintf(text, "%d", g_session.m_homunCritical);
	TextOutWithRightAlign(79, 78, text, textLen, 1, 14, 0);
	textLen = sprintf(text, "%d", g_session.m_homunDef);
	TextOutWithRightAlign(79, 96, text, textLen, 1, 14, 0);
	textLen = sprintf(text, "%d", g_session.m_homunMdef);
	TextOutWithRightAlign(79, 114, text, textLen, 1, 14, 0);
	textLen = sprintf(text, "%d", g_session.m_homunFlee);
	TextOutWithRightAlign(79, 132, text, textLen, 1, 14, 0);
	textLen = sprintf(text, "%d", (2000 - g_session.m_homunAspd) / 10);
	TextOutWithRightAlign(79, 150, text, textLen, 1, 14, 0);

	if (g_session.m_isHomunNameModified & 1) {
		TextOutA(135, 24, g_session.m_homunName, 0, 0, 12, 0xff00);
		TextOutA(136, 24, g_session.m_homunName, 0, 0, 12, 0x1e1e1e);
	}

	itoa(g_session.m_homunLevel, stLevel, 10);
	TextOutA(135, 47, stLevel, 0, 0, 12, 0);

	if (g_session.m_homunJob != -1) {
		char text[24];
		textLen = sprintf(text, "%d / %d", g_session.m_homunHP, g_session.m_homunMaxHP);
		TextOutA(218, 70, text, textLen, 1, 14, 0);

		textLen = sprintf(text, "%d / %d", g_session.m_homunSP, g_session.m_homunMaxSP);
		TextOutA(218, 85, text, textLen, 1, 14, 0);

		textLen = sprintf(text, "%d", g_session.m_homunNextExp);
		TextOutWithRightAlign(200, 104, text, textLen, 1, 14, 0);

		textLen = sprintf(text, "%d / %d", g_session.m_homunHungry, 100);
		TextOutWithRightAlign(200, 130, text, textLen, 1, 14, 0);

		if (g_session.m_homunAcc == 0) {
			TextOutA(220, 130, MsgStr(MSI_ITEM_UNEQUIPED), 0, 0, 12, 0);
		}
		else {
			ITEM_INFO itemInfo;
			itemInfo.SetItemId(g_session.m_homunAcc);
			itemInfo.GetItemName(itemName);
			sprintf(text, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\item\\%s.bmp", itemName);
			bitmap = (CBitmapRes*)g_resMgr().Get(UIBmp(text), false);
			DrawBitmap(235, 122, bitmap, true);
		}

		if (g_session.m_homunFriendly >= 0 && g_session.m_homunFriendly <= 3)
			strcpy(text, MsgStr(MSI_VERYHATE));
		else if (g_session.m_homunFriendly > 3 && g_session.m_homunFriendly <= 10)
			strcpy(text, MsgStr(MSI_HATE));
		else if (g_session.m_homunFriendly > 10 && g_session.m_homunFriendly <= 100)
			strcpy(text, MsgStr(MSI_VERY_AWKWARD));
		else if (g_session.m_homunFriendly > 100 && g_session.m_homunFriendly <= 250)
			strcpy(text, MsgStr(MSI_AWKWARD));
		else if (g_session.m_homunFriendly > 250 && g_session.m_homunFriendly <= 750)
			strcpy(text, MsgStr(MSI_NORMAL));
		else if (g_session.m_homunFriendly > 750 && g_session.m_homunFriendly <= 910)
			strcpy(text, MsgStr(MSI_FRIENDLY));
		else if (g_session.m_homunFriendly > 910 && g_session.m_homunFriendly <= 1000)
			strcpy(text, MsgStr(MSI_VERY_FRIENDLY));
		else
			strcpy(text, MsgStr(MSI_UNKNOWN));

		TextOutA(150, 159, text, 0, 0, 12, 0);
	}
}

int UIHomunInfoWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	char msg[512];

	switch (message) {
	case UM_DEFPUSH:
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			if (strlen(m_newName->GetText()) >= 24) {
				g_windowMgr.ErrorMsg(MsgStr(MSI_HOMUN_NAME_IN23), ME_OK, 1, 0, 0);
				break;
			}
			sprintf(msg, MsgStr(MSI_HOMUN_NAME_CHANGE_ONLYONCE), m_newName->GetText());
			MsgStr(MSI_HOMUN_NAME_CHANGE_ONLYONCE);
			int result = g_windowMgr.ErrorMsg(msg, ME_YESNO, 0, 0, 0);
			if (result != ID_NO) {
				g_modeMgr.GetGameMode()->SendMsg(MM_MER_RENAME, (int)m_newName->GetText(), 0, 0);
				g_session.m_isHomunNameModified = 1;
				sprintf(g_session.m_homunName, m_newName->GetText());
				m_button->Move(-100, -100);
				m_newName->Move(-100, -100);
				Invalidate();
			}
			break;
		}
		case ID_DELETE_HOMUN: {
			CMode* gameMode = (CMode*)g_modeMgr.GetGameMode();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_DELETE_HOMUN), ME_YESNO, 0, 0, 0) == ID_YES) {
				gameMode->SendMsg(MM_COMMAND_HOMUN, 2, 0, 0);
				g_windowMgr.DeleteWindow(WID_HOMUNINFOWND);
				g_windowMgr.DeleteWindow(WID_HOMUNSKILLLISTWND);
				g_session.InitHomunTargetInfo();
				g_session.m_homunAlive = 0;
			}
			break;
		}
		case ID_OPEN_HOMUNSKILLLIST:
			if (!g_windowMgr.DeleteWindow(WID_HOMUNSKILLLISTWND))
				g_windowMgr.MakeWindow(WID_HOMUNSKILLLISTWND);
			break;
		case ID_FEED_TO_HOMUN: {
			CMode* gameMode = (CMode*)g_modeMgr.GetGameMode();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_SURE_TO_FEED_PET), ME_YESNO, 0, 0, 0) == ID_YES)
				gameMode->SendMsg(MM_COMMAND_HOMUN, 1, 0, 0);
			break;
		}
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_HOMUNINFOWND);
			break;
		}
		break;
	case UM_SETINFO:
		m_info = (HomunInfoWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}


UIMerInfoWnd::UIMerInfoWnd()
{
}


UIMerInfoWnd::~UIMerInfoWnd() {}

void UIMerInfoWnd::OnCreate(int cx, int cy)
{
	{
		int posX = m_w - 14;
		char buttonName[] = "close";
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
		char toolTip[] = "Close";

		UIBitmapButton* closeButton = new UIBitmapButton;
		closeButton->SetBitmapName((preBmpName + buttonName + "_off.bmp").c_str(), 0);
		closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 1);
		closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 2);
		closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
		closeButton->Move(posX, 3);
		closeButton->SetId(ID_CLOSE);
		closeButton->SetToolTip(toolTip);
		AddChild(closeButton);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_fired";
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), 0);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
		m_button->SetBitmapName((pathName + "_b.bmp").c_str(), 2);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		m_button->Move(187, 44);
		m_button->SetId(ID_FIRE_MER);
		AddChild(m_button);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_skill";
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), 0);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
		m_button->SetBitmapName((pathName + "_b.bmp").c_str(), 2);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		m_button->Move(232, 44);
		m_button->SetId(ID_OPEN_MERSKILLLIST);
		AddChild(m_button);
	}

	m_hpGraph = new UIBarGraphPlayerHp;
	m_hpGraph->m_isPlayerType = 2;
	m_hpGraph->Create(85, 9);
	m_hpGraph->Move(117, 73);
	AddChild(m_hpGraph);

	m_spGraph = new UIBarGraphPlayerSp;
	m_spGraph->m_isPlayerType = 2;
	m_spGraph->Create(85, 9);
	m_spGraph->Move(117, 88);
	AddChild(m_spGraph);

	m_defPushId = 0x87;
}

void UIMerInfoWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

void UIMerInfoWnd::OnDraw()
{
	char windowName[32];
	int textLen;
	long tt;
	char text[24];

	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\homuninfo_bg.bmp"), false);
	DrawBitmap(0, 0, bitmap, false);

	if (g_session.m_merJob >= JT_MER_ARCHER01 && g_session.m_merJob <= JT_MER_ARCHER10)
		sprintf(windowName, MsgStr(MSI_MER_INFO_TYPE_ARCHER));
	else if (g_session.m_merJob >= JT_MER_SWORDMAN01 && g_session.m_merJob <= JT_MER_SWORDMAN10)
		sprintf(windowName, MsgStr(MSI_MER_INFO_TYPE_SWORDMAN));
	else if (g_session.m_merJob >= JT_MER_LANCER01 && g_session.m_merJob <= JT_MER_LANCER10)
		sprintf(windowName, MsgStr(MSI_MER_INFO_TYPE_LANCER));
	else
		sprintf(windowName, MsgStr(MSI_MER_INFO_TYPE_MONSTER));

	TextOutA(5, 2, windowName, 0, 0, 12, 0);
	TextOutA(100, 24, MsgStr(MSI_NAME), 0, 0, 12, 0);
	TextOutA(100, 47, MsgStr(MSI_LEVEL), 0, 0, 12, 0);
	TextOutA(100, 111, MsgStr(MSI_LEFT_TIME), 0, 0, 12, 0);
	TextOutA(100, 135, MsgStr(MSI_MER_FRIENDLY), 0, 0, 12, 0);
	TextOutA(100, 159, MsgStr(MSI_MER_CALLCOUNT), 0, 0, 12, 0);
	TextOutA(230, 133, MsgStr(MSI_MER_KILL), 0, 0, 12, 0);

	{
		char text[12];
		textLen = sprintf(text, "%d", g_session.m_merAtk);
		TextOutWithRightAlign(79, 24, text, textLen, 1, 14, 0);
		textLen = sprintf(text, "%d", g_session.m_merMatk);
		TextOutWithRightAlign(79, 42, text, textLen, 1, 14, 0);
		textLen = sprintf(text, "%d", g_session.m_merHit);
		TextOutWithRightAlign(79, 60, text, textLen, 1, 14, 0);
		textLen = sprintf(text, "%d", g_session.m_merCritical);
		TextOutWithRightAlign(79, 78, text, textLen, 1, 14, 0);
		textLen = sprintf(text, "%d", g_session.m_merDef);
		TextOutWithRightAlign(79, 96, text, textLen, 1, 14, 0);
		textLen = sprintf(text, "%d", g_session.m_merMdef);
		TextOutWithRightAlign(79, 114, text, textLen, 1, 14, 0);
		textLen = sprintf(text, "%d", g_session.m_merFlee);
		TextOutWithRightAlign(79, 132, text, textLen, 1, 14, 0);
		textLen = sprintf(text, "%d", (2000 - g_session.m_merAspd) / 10);
		TextOutWithRightAlign(79, 150, text, textLen, 1, 14, 0);
	}

	TextOutA(136, 24, g_session.m_merName, 0, 0, 12, 0x646464);
	TextOutA(135, 24, g_session.m_merName, 0, 0, 12, 0);

	{
		char stLevel[8];
		itoa(g_session.m_merLevel, stLevel, 10);
		TextOutA(135, 47, stLevel, 0, 0, 12, 0);
	}

	if (g_session.m_merJob != -1) {
		textLen = sprintf(text, "%d / %d", g_session.m_merHP, g_session.m_merMaxHP);
		TextOutA(218, 70, text, textLen, 1, 14, 0);

		textLen = sprintf(text, "%d / %d", g_session.m_merSP, g_session.m_merMaxSP);
		TextOutA(218, 85, text, textLen, 1, 14, 0);

		tt = (long)&g_session.m_merExpireDate;
		tm* expireTime = localtime(&tt);
		strftime(text, 128, MsgStr(MSI_TIME_TYPE2), expireTime);
		textLen = strlen(text);
		TextOutA(155, 111, text, textLen, 0, 12, 0xff);

		textLen = sprintf(text, "%d", g_session.m_merFaith);
		TextOutA(170, 133, text, textLen, 1, 14, 0);

		textLen = sprintf(text, "%d", g_session.m_merCallNum);
		TextOutA(170, 157, text, textLen, 1, 14, 0);

		textLen = sprintf(text, "%d", g_session.m_merKillCounter);
		TextOutA(245, 157, text, textLen, 1, 14, 0);
	}
}

int UIMerInfoWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_DEFPUSH:
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			Invalidate();
			break;
		case ID_FIRE_MER: {
			CMode* gameMode = (CMode*)g_modeMgr.GetGameMode();
			if (g_windowMgr.ErrorMsg(MsgStr(MSI_DELETE_MER), ME_YESNO, 0, 0, 0) == ID_YES) {
				gameMode->SendMsg(MM_COMMAND_MER, 2, 0, 0);
				g_windowMgr.DeleteWindow(WID_MERINFOWND);
				g_windowMgr.DeleteWindow(WID_MERSKILLLISTWND);
				g_session.InitMerTargetInfo();
				g_session.m_merAlive = 0;
			}
			break;
		}
		case ID_OPEN_MERSKILLLIST:
			if (!g_windowMgr.DeleteWindow(WID_MERSKILLLISTWND))
				g_windowMgr.MakeWindow(WID_MERSKILLLISTWND);
			break;
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_MERINFOWND);
			break;
		}
		break;
	case UM_SETINFO:
		m_info = (MerInfoWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
