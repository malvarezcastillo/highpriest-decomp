#include "common.h"
#include "MsgStrings.h"
#include "framework/Locale.h"

#include "device/Sound.h"
#include "ItemInfo.h"
#include "Insult.h"
#include "Mode.h"
#include "SkillInfo.h"
#include "GameActor.h"
#include "GameMode.h"
#include "Session.h"
#include "TipOfTheDay.h"
#include "3dlib/Device.h"
#include "window/UIFrameWnd.h"
#include "window/UISys.h"
#include "device/Connection.h"
#include "device/Control.h"
#include "resource/ImfRes.h"
#include "GuildInfo.h"
#include "pdb_enums.h"

#include <time.h>
#include "pdb_shared.h"

extern UIWindowMgr g_windowMgr;
extern CSession g_session;
extern CModeMgr g_modeMgr;
extern Registry2 g_registry;
extern int g_showTipsAtStartup;
extern int g_texLenDivFactor;

void CutString(const char* msg, std::vector<std::string>& strings, int maxNumCharLine);
extern CMouse g_mouse;
extern ServerType g_serverType;




#include "base/File.h"
#include "base/Res.h"
#include "3dlib/Renderer.h"
#include "resource/Bitmap.h"
#include "resource/Action.h"
#include "resource/Sprite.h"
#include "resource/PaletteRes.h"
#include "framework/Language.h"
#include "SnapMgr.h"
#include "skin.h"

extern CSnapMgr g_snapMgr;
extern int g_isSoundOn;

extern const char* g_regPath;
extern char* g_bookmarkPath;
extern const char* UIBmp(const char* uiBmpName);
extern ServiceType g_serviceType;
extern CRenderer* g_renderer;
extern const char* boundaryText;
extern std::vector<std::string> s_loadingScreenList;
extern DWORD g_codePage;
extern unsigned char hex_table[];
extern bool IsNameYellow(unsigned long aid);
extern bool IsGravityAid(unsigned long aid);

const char* boundaryText = "-";
char* g_bookmarkPath = "\\BookMark";
int Get2DVolume();
void SetStreamVolume(int volume);
void Set2DVolume(int volume);
void Set3DVolume(int volume);

bool IsEffectStatePushCart(DWORD effectState);
bool IsEffectStateBird(DWORD effectState);
bool IsEffectStateChicken(DWORD effectState);

const char* MsgStr(MSGSTRINGID id);
BOOL IsValidItemIndicator(LPCSTR pszMsg, LPCSTR pszEnd, int* psizeChr, int* pitemId);

#include "rag_budget_fill.h"
int UIFrameWnd::MakeItemText(const ITEM_INFO& iteminfo, unsigned long& manufacturerColor,
	std::vector<const char*>& itemNameList, char*& itemNameIter, char*& itemNameColored,
	bool bIsColorList)
{
	RAG_BUDGET_FILL(5)
	itemNameList.reserve(5);

	int nameLength;
	if (iteminfo.m_isIdentified) {
		if (iteminfo.m_itemType == 4 &&
			iteminfo.m_slot[0] == 0 &&
			iteminfo.m_slot[1] == 0 &&
			iteminfo.m_slot[2] == 0 &&
			iteminfo.m_slot[3] == 1) {
			itemNameIter += iteminfo.GetNamed(itemNameIter);
			itemNameList.push_back(itemNameIter);
		}

		if (iteminfo.m_slot[0] == 0xfe && iteminfo.m_slot[1] == 0) {
			if (iteminfo.m_refiningLevel) {
				itemNameIter += iteminfo.GetRefining(itemNameIter);
				itemNameList.push_back(itemNameIter);
			}

			if (g_serviceType == ServiceBrazil) {
				itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
				itemNameList.push_back(itemNameIter);
				itemNameIter += iteminfo.GetManufacturedItemPostfix(itemNameIter);
				itemNameList.push_back(itemNameIter);
				nameLength = iteminfo.GetManufacturedItemProducer(itemNameIter);
				if (nameLength) {
					itemNameColored = itemNameIter;
					itemNameIter += nameLength;
					manufacturerColor = 0xe04040;
				} else {
					g_windowMgr.AddToNameWaitingWindowList(this);
					strcpy(itemNameIter, MsgStr(MSI_NAMELESS_USER));
					itemNameIter += strlen(itemNameIter);
					manufacturerColor = 0x4040e0;
				}
				itemNameList.push_back(itemNameIter);
			} else if (g_serviceType == ServiceRussia) {
				itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
				itemNameList.push_back(itemNameIter);
				itemNameIter += iteminfo.GetManufacturedItemPostfix(itemNameIter);
				itemNameList.push_back(itemNameIter);
				nameLength = iteminfo.GetManufacturedItemProducer(itemNameIter);
				if (nameLength) {
					itemNameColored = itemNameIter;
					itemNameIter += nameLength;
					manufacturerColor = 0xe04040;
				} else {
					g_windowMgr.AddToNameWaitingWindowList(this);
					strcpy(itemNameIter, MsgStr(MSI_NAMELESS_USER));
					itemNameIter += strlen(itemNameIter);
					manufacturerColor = 0x4040e0;
				}
				itemNameList.push_back(itemNameIter);
			} else {
				nameLength = iteminfo.GetManufacturedItemProducer(itemNameIter);
				if (nameLength) {
					itemNameColored = itemNameIter;
					itemNameIter += nameLength;
					manufacturerColor = 0xe04040;
				} else {
					g_windowMgr.AddToNameWaitingWindowList(this);
					strcpy(itemNameIter, MsgStr(MSI_NAMELESS_USER));
					itemNameIter += strlen(itemNameIter);
					manufacturerColor = 0x4040e0;
				}
				itemNameList.push_back(itemNameIter);
				itemNameIter += iteminfo.GetManufacturedItemPostfix(itemNameIter);
				itemNameList.push_back(itemNameIter);
				itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
				itemNameList.push_back(itemNameIter);
			}
		} else if (iteminfo.IsAttachableItem()) {
			if (iteminfo.m_refiningLevel) {
				itemNameIter += iteminfo.GetRefining(itemNameIter);
				itemNameList.push_back(itemNameIter);
			}

			if (ITEM_INFO::IsCardItem(iteminfo.m_slot[0])) {
				itemNameIter += iteminfo.GetPrefix(itemNameIter);
				itemNameList.push_back(itemNameIter);
				itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
				itemNameList.push_back(itemNameIter);
				itemNameIter += iteminfo.GetPostfix(itemNameIter);
				itemNameList.push_back(itemNameIter);
			} else if (iteminfo.IsManufacturedItem()) {
				if (g_serviceType == ServiceBrazil) {
					itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameIter += iteminfo.GetManufacturedItemPostfix(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameIter += iteminfo.GetManufacturedItemPrefix(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameColored = itemNameIter;
					nameLength = iteminfo.GetManufacturedItemProducer(itemNameIter);
					if (nameLength) {
						itemNameColored = itemNameIter;
						itemNameIter += nameLength;
						manufacturerColor = 0xe04040;
					} else {
						g_windowMgr.AddToNameWaitingWindowList(this);
						strcpy(itemNameIter, MsgStr(MSI_NAMELESS_USER));
						itemNameIter += strlen(itemNameIter);
						manufacturerColor = 0x4040e0;
					}
					itemNameList.push_back(itemNameIter);
					if (bIsColorList)
						itemNameList.push_back(itemNameIter);
				} else if (g_serviceType == ServiceRussia) {
					itemNameIter += iteminfo.GetManufacturedItemPrefix(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameIter += iteminfo.GetManufacturedItemPostfix(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameColored = itemNameIter;
					nameLength = iteminfo.GetManufacturedItemProducer(itemNameIter);
					if (nameLength) {
						itemNameColored = itemNameIter;
						itemNameIter += nameLength;
						manufacturerColor = 0xe04040;
					} else {
						g_windowMgr.AddToNameWaitingWindowList(this);
						strcpy(itemNameIter, MsgStr(MSI_NAMELESS_USER));
						itemNameIter += strlen(itemNameIter);
						manufacturerColor = 0x4040e0;
					}
					itemNameList.push_back(itemNameIter);
					if (bIsColorList)
						itemNameList.push_back(itemNameIter);
				} else {
					itemNameIter += iteminfo.GetManufacturedItemPrefix(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameColored = itemNameIter;
					nameLength = iteminfo.GetManufacturedItemProducer(itemNameIter);
					if (nameLength) {
						itemNameColored = itemNameIter;
						itemNameIter += nameLength;
						manufacturerColor = 0xe04040;
					} else {
						g_windowMgr.AddToNameWaitingWindowList(this);
						strcpy(itemNameIter, MsgStr(MSI_NAMELESS_USER));
						itemNameIter += strlen(itemNameIter);
						manufacturerColor = 0x4040e0;
					}
					itemNameList.push_back(itemNameIter);
					if (bIsColorList)
						itemNameList.push_back(itemNameIter);
					itemNameIter += iteminfo.GetManufacturedItemPostfix(itemNameIter);
					itemNameList.push_back(itemNameIter);
					itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
					itemNameList.push_back(itemNameIter);
				}
			} else {
				itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
				itemNameList.push_back(itemNameIter);
			}
		} else {
			itemNameIter += iteminfo.GetIdDisplayName(itemNameIter);
			itemNameList.push_back(itemNameIter);
		}
	} else {
		itemNameIter += iteminfo.GetUnidDisplayName(itemNameIter);
		itemNameList.push_back(itemNameIter);
	}

	return 1;
}

int UIFrameWnd::DrawItemText(int x, int y, ITEM_INFO const &iteminfo, unsigned long color, int fontType, int fontHeight)
{
	char itemName[256];
	unsigned long manufacturerColor;
	std::vector<const char*> itemNameList;
	char* itemNameIter = itemName;
	char* itemNameColored = NULL;

	MakeItemText(iteminfo, manufacturerColor, itemNameList, itemNameIter, itemNameColored, 0);

	int offset = 0;
	char* text = itemName;
	std::vector<const char*>::iterator nameListIter = itemNameList.begin();
	const char* textEnd;
	while (nameListIter != itemNameList.end()) {
		textEnd = *nameListIter;
		int textLength = textEnd - text;
		if (textLength) {
			if (text == itemNameColored) {
				TextOutWithShadow(x + offset, y, text, textLength, manufacturerColor, 0xe0e0e0, 0, 12);
			} else if (iteminfo.m_isDamaged) {
				TextOutWithShadow(x + offset, y, text, textLength, color, 0x5050fa, 0, 12);
			} else {
				TextOutWithShadow(x + offset, y, text, textLength, color, 0xe0e0e0, 0, 12);
			}
			int width = GetTextWidth(text, textLength, 0, 12, false);
			text = const_cast<char*>(textEnd);
			offset += width;
		}
		++nameListIter;
	}

	return offset;
}

int UIFrameWnd::DrawItemTextRect(int x, int y, int width, ITEM_INFO const &iteminfo, unsigned long color, int fontType, int fontHeight)
{
	char itemName[256];
	unsigned long manufacturerColor;
	char* itemNameIter = itemName;
	char* itemNameColored = NULL;
	std::vector<const char*> itemNameList;

	MakeItemText(iteminfo, manufacturerColor, itemNameList, itemNameIter, itemNameColored, 1);
	int ret = TextOutWithItemRect(x, y, width, itemNameList, itemName, itemNameColored,
		manufacturerColor, color, fontType, fontHeight);
	return ret;
}

int UIFrameWnd::DrawItemObtainTextRect(int x, int y, int width, ITEM_INFO const &iteminfo, unsigned long color, int fontType, int fontHeight)
{
	char itemName[256];
	unsigned long manufacturerColor;
	char* itemNameIter = itemName;
	char* itemNameColored = NULL;
	std::vector<const char*> itemNameList;

	MakeItemText(iteminfo, manufacturerColor, itemNameList, itemNameIter, itemNameColored, 1);
	itemNameIter += sprintf(itemNameIter, MsgStr(MSI_EA_OBTAIN), iteminfo.m_num);
	itemNameList.push_back(itemNameIter);
	int ret = TextOutWithItemRect(x, y, width, itemNameList, itemName, itemNameColored,
		manufacturerColor, color, fontType, fontHeight);
	return ret;
}

int UIFrameWnd::TextOutWithItemRect(int x, int y, int width,
	std::vector<const char*>& itemNameList, char* itemName, char* itemNameColored,
	unsigned long manufacturerColor, unsigned long color, int fontType, int fontHeight)
{
	int offset = 0;
	char* text = itemName;
	std::vector<const char*>::iterator nameListIter = itemNameList.begin();

	if (GetTextWidth(itemName, 0, 0, 12, false) < width) {
		while (nameListIter != itemNameList.end()) {
			const char* textEnd = *nameListIter;
			int textLength = textEnd - text;
			if (textLength) {
				if (text == itemNameColored) {
					TextOutWithShadow(x + offset, y, text, textLength,
						manufacturerColor, 0xe0e0e0, 0, 12);
				} else {
					TextOutWithShadow(x + offset, y, text, textLength,
						color, 0xe0e0e0, 0, 12);
				}
				offset += GetTextWidth(text, textLength, 0, 12, false);
				text = const_cast<char*>(textEnd);
			}
			++nameListIter;
		}
		return x;
	}

	y -= fontHeight / 2;
	int yCnt = 1;
	while (nameListIter != itemNameList.end()) {
		const char* textEnd = *nameListIter;
		unsigned char isColored = text == itemNameColored;
		while (text < textEnd) {
			char* textNext = CharNextExA(g_codePage, text, 0);
			if (g_multiLang && FindCharSet(text, strlen(text)) == 0) {
				text += 3;
			} else {
				int textLength = textNext - text;
				int remainWidth = GetTextWidth(textNext, strlen(textNext), 0, 12, false);
				if (yCnt == 2 && offset >= width - 15 && offset + remainWidth > width) {
					if (isColored) {
						TextOutWithShadow(x + offset + textLength, y, "...", 3,
							manufacturerColor, 0xe0e0e0, 0, 12);
					} else {
						TextOutWithShadow(x + offset + textLength, y - 1, "...", 3,
							color, 0xe0e0e0, 0, 12);
					}
					break;
				}

				if (isColored) {
					TextOutWithShadow(x + offset, y, text, textLength,
						manufacturerColor, 0xe0e0e0, 0, 12);
				} else {
					TextOutWithShadow(x + offset, y, text, textLength,
						color, 0xe0e0e0, 0, 12);
				}
				offset += GetTextWidth(text, textLength, 0, 12, false);
				if (offset > width) {
					offset = 0;
					y += fontHeight + 1;
					++yCnt;
				}
				text = textNext;
			}
		}
		++nameListIter;
	}
	return x;
}



UIWaitWnd::UIWaitWnd() { m_pctrCancel = NULL; }


UIWaitWnd::~UIWaitWnd() {}


void UIWaitWnd::SetMsg(const char* waitMsg, int fontHeight, int fontType) {
	m_textViewer->ClearAllItems();
	m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)waitMsg, 38, 0, 0);
}

void UIWaitWnd::OnCreate(int cx, int cy)
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	m_pctrCancel = new UIBitmapButton;
	m_pctrCancel->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_cancel.bmp", BS_NORMAL);
	m_pctrCancel->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_cancel_a.bmp", BS_MOUSEON);
	m_pctrCancel->SetBitmapName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_cancel_b.bmp", BS_PRESSED);
	m_pctrCancel->Create(m_pctrCancel->GetBitmapWidth(), m_pctrCancel->GetBitmapHeight());
	m_pctrCancel->Move((cx - m_pctrCancel->GetBitmapWidth()) / 2, 50);
	m_pctrCancel->SetId(ID_CANCEL);
	m_pctrCancel->SetToolTip("");
	AddChild(m_pctrCancel);

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 22, cx - 20, cy - 55, false);
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);
	const char* waitMsg = MsgStr(MSI_PLEASE_BE_PATIENT);
	m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)waitMsg, 38, 0, 0);
}

int UIWaitWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_ENABLE_CANCEL:
		m_pctrCancel->Move((m_w - m_pctrCancel->GetBitmapWidth()) / 2,
						   m_h - m_pctrCancel->GetBitmapHeight() - 5);
		break;
	case UM_COMMAND:
		if (val1 == 0x77)
			g_modeMgr.GetCurMode()->SendMsg(LMM_DISCONNECT_ACSVR, 0, 0, 0);
		break;
	}
	return 0;
}

void UIWaitWnd::OnDraw() {
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\win_msgbox.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
}
UILoadingWnd::UILoadingWnd() { m_barGraph = NULL; }
UILoadingWnd::~UILoadingWnd() {}

void UILoadingWnd::OnCreate(int cx, int cy)
{
	static int s_loadingScreenIndex;
	char fnStr[260];
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	m_barGraph = new UIBarGraph2;
	m_barGraph->Create(240, 15);
	m_barGraph->Move(0, 0);
	m_barGraph->SetAmount(0.0f);

	g_skinMgr.GetColorChipColor(22, 2, rr, gg, bb);
	m_barGraph->m_r = rr;
	m_barGraph->m_g = gg;
	m_barGraph->m_b = bb;
	AddChild(m_barGraph);

	int loadingScreenCount = s_loadingScreenList.size();

	if (!g_session.RandomScreenFlag) {
		if (s_loadingScreenIndex < loadingScreenCount) {
			strcpy(fnStr, s_loadingScreenList[s_loadingScreenIndex++].c_str());
		} else {
			s_loadingScreenIndex = 0;
			strcpy(fnStr, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\loading00.jpg");
		}
	} else {
		if (loadingScreenCount) {
			strcpy(fnStr, s_loadingScreenList[rand() % loadingScreenCount].c_str());
		} else {
			strcpy(fnStr, "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\loading00.jpg");
		}
	}

	g_resMgr().Get(UIBmp(fnStr), false);
	g_windowMgr.SetWallpaper((CBitmapRes*)g_resMgr().Get(UIBmp(fnStr), false));
}

void UILoadingWnd::OnDraw() {}

void UILoadingWnd::SetMsg(const char* waitMsg, int fontHeight, int fontType) {}

int UILoadingWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	if (message == 0x11) {
		char msg[100];
		sprintf(msg, "Loading %d %s", val1, "%");
		m_barGraph->SetAmount((float)val1);
	}
	return 0;
}
UIBasicInfoWnd::UIBasicInfoWnd() { m_info = NULL; }
UIBasicInfoWnd::~UIBasicInfoWnd() {}

void UIBasicInfoWnd::OnCreate(int cx, int cy)
{
	int ids[8] = { ID_STATUS, ID_OPTION, ID_ITEMS, ID_EQUIP, ID_SKILL, ID_MAP, ID_COMM, ID_FRIEND };
	char* buttonName[8] = { "status", "option", "items", "equip", "skill", "map", "comm", "friend" };
	char* toolTip[8] = { "Alt+A", "Alt+O", "Alt+E", "Alt+Q", "Alt+S", "Ctrl+Tab", "Alt+C", "Alt+Z" };
	int i;
	int dY;

	std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btn_";
	dY = (cy == 35) * 40;

	for (i = 0; i < 8; ++i) {
		m_buttons[i] = new UIBitmapButton;
		m_buttons[i]->SetBitmapName((preBmpName + buttonName[i] + "_off.bmp").c_str(), BS_NORMAL);
		m_buttons[i]->SetBitmapName((preBmpName + buttonName[i] + "_on.bmp").c_str(), BS_MOUSEON);
		m_buttons[i]->SetBitmapName((preBmpName + buttonName[i] + "_on.bmp").c_str(), BS_PRESSED);
		m_buttons[i]->Create(m_buttons[i]->GetBitmapWidth(), m_buttons[i]->GetBitmapHeight());
		m_buttons[i]->Move((i % 2) * 36 + 207, (i / 2) * 24 + dY + 22);
		m_buttons[i]->SetId(ids[i]);
		m_buttons[i]->SetToolTip(toolTip[i]);
		if (i < 5)
			AddChild(m_buttons[i]);
	}

	int posX[3] = { 3, 266, 0 };
	int ids2[3] = { ID_BASE, ID_MINI, 0 };
	char* buttonName2[8] = { "base", "mini" };
	char* toolTip2[8] = { "", "Alt+V, Ctrl+V" };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

	for (i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(posX[i], 3);
		button->SetId(ids2[i]);
		button->SetToolTip(toolTip2[i]);
		AddChild(button);
	}

	m_name = new UIStaticText;
	m_name->Create(84, 15);
	m_name->Move(9, 23);
	m_name->SetTextWithShorten(g_session.GetCharName());
	m_name->SetFont(0, 12);
	AddChild(m_name);

	m_hpGraph = new UIBarGraphPlayerHp;
	m_hpGraph->m_isPlayerType = 0;
	m_hpGraph->Create(85, 9);
	m_hpGraph->Move(110, 22);
	AddChild(m_hpGraph);

	m_spGraph = new UIBarGraphPlayerSp;
	m_spGraph->m_isPlayerType = 0;
	m_spGraph->Create(85, 9);
	m_spGraph->Move(110, 43);
	AddChild(m_spGraph);

	m_expGraph = new UIBarGraph4;
	m_expGraph->m_bType = 1;
	m_expGraph->Create(102, 6);
	m_expGraph->Move(84, 76);
	m_expGraph->SetAmount(g_session.GetExpPointer(), g_session.GetNextExpPointer());
	AddChild(m_expGraph);

	m_jobexpGraph = new UIBarGraph4;
	m_jobexpGraph->m_bType = 1;
	m_jobexpGraph->Create(102, 6);
	m_jobexpGraph->Move(84, 86);
	m_jobexpGraph->SetAmount(g_session.GetJobexpPointer(), g_session.GetJobnextexpPointer());
	AddChild(m_jobexpGraph);

	NewHeight(cy);
}

void UIBasicInfoWnd::NewHeight(int newHeight)
{
	if (newHeight == MINI_SIZE) {
		Resize(m_w, MINI_SIZE);
		m_name->Resize(59, 15);
		m_name->SetTextWithShorten(g_session.GetCharName());
		m_name->Move(17, 3);
		m_hpGraph->Move(110, 62);
		m_spGraph->Move(110, 83);

		for (int i = 0; i < 8; ++i) {
			m_buttons[i]->Move((i % 2) * 36 + 207, (i / 2) * 24 + 62);
		}
	} else if (newHeight == FULL_SIZE) {
		Resize(m_w, FULL_SIZE);
		m_name->Resize(84, 15);
		m_name->SetTextWithShorten(g_session.GetCharName());
		m_name->Move(9, 22);
		m_hpGraph->Move(110, 22);
		m_spGraph->Move(110, 43);

		if (g_session.GetLevel() == 199)
			m_expGraph->Move(-200, 78);
		else
			m_expGraph->Move(85, 78);

		if (g_session.GetJob() == JT_SUPERNOVICE_B
			|| g_session.GetJob() == JT_SUPERNOVICE) {
			if (g_session.GetJoblevel() >= 99)
				m_jobexpGraph->Move(-200, 88);
			else
				m_jobexpGraph->Move(85, 88);
		} else if ((g_session.GetJob() >= JT_KNIGHT_H && g_session.GetJob() < JT_NOVICE_B)
			|| g_session.GetJob() == JT_GUNSLINGER
			|| g_session.GetJob() == JT_NINJA) {
			if (g_session.GetJoblevel() >= 70)
				m_jobexpGraph->Move(-200, 88);
			else
				m_jobexpGraph->Move(85, 88);
		} else {
			if (g_session.GetJoblevel() >= 50)
				m_jobexpGraph->Move(-200, 88);
			else
				m_jobexpGraph->Move(85, 88);
		}

		for (int i = 0; i < 8; ++i) {
			m_buttons[i]->Move((i % 2) * 36 + 207, (i / 2) * 24 + 22);
		}
	}
}

void UIBasicInfoWnd::OnDraw()
{
	int exp = g_session.GetExp();
	int nextExp = g_session.GetNextExp();

	switch (m_h) {
	case MINI_SIZE: {
		char moneyTxt[64];
		char text1[128];
		CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\basewin_mini.bmp"), false);
		DrawBitmap(0, 0, bmp, false);

		if (exp < 0) exp = 0;
		if (nextExp <= 0) nextExp = exp + 1;

		double expRatio;
		if (g_session.GetLevel() == 99)
			expRatio = 1.0;
		else
			expRatio = (double)exp / (double)nextExp;

		int textLen = sprintf(text1, "Lv. %2d / %s / Exp. %d %%", g_session.GetLevel(), g_session.m_jobNameTable[g_session.GetJob()], (int)(expRatio * 100.0));

		if (g_serviceType == ServiceKorea && g_session.GetJob() >= JT_TAEKWON && g_session.GetJob() <= JT_LINKER) {
			textLen = sprintf(text1, "Lv%2d / %s / Lv%2d / Exp%d%%", g_session.GetLevel(), g_session.m_jobNameTable[g_session.GetJob()], g_session.GetJoblevel(), (int)(expRatio * 100.0));
			TextOutWithRightAlign(264, 4, text1, textLen, 0, 11, 0);
		} else {
			TextOutWithRightAlign(264, 2, text1, textLen, 1, 14, 0);
		}

		GetNumberText(g_session.GetGold(), moneyTxt);
		textLen = sprintf(text1, "HP  %3d / %3d  |  SP  %3d / %3d  |  %s Z", g_session.GetHp(), g_session.GetMaxhp(), g_session.GetSp(), g_session.GetMaxsp(), moneyTxt);
		TextOutWithRightAlign(275, 18, text1, textLen, 1, 14, 0);
		break;
	}
	case FULL_SIZE: {
		char moneyTxt[64];
		char text1[128];
		CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\basewin_bg.bmp"), false);
		DrawBitmap(0, 0, bmp, false);

		if (g_serviceType == ServiceKorea) {
			TextOutA(9, 36, g_session.m_jobNameTable[g_session.GetJob()], 0, 0, 12, 0);
		} else {
			TextOutA(9, 36, g_session.m_jobNameTable[g_session.GetJob()], 0, 1, 14, 0);
		}

		if (g_serviceType == ServiceAmerica) {
			TextOutA(18, 3, "Basic Info", 0, 0, 12, 0xffffff);
			TextOutA(17, 2, "Basic Info", 0, 0, 12, 0);
		} else {
			TextOutA(18, 3, MsgStr(MSI_BASICINFOWND), 0, 0, 12, 0xffffff);
			TextOutA(17, 2, MsgStr(MSI_BASICINFOWND), 0, 0, 12, 0);
		}

		sprintf(text1, "HP      %3d  /  %3d", g_session.GetHp(), g_session.GetMaxhp());
		TextOutA(95, 30, text1, 0, 1, 14, 0);

		sprintf(text1, "SP      %3d  /  %3d", g_session.GetSp(), g_session.GetMaxsp());
		TextOutA(95, 51, text1, 0, 1, 14, 0);

		sprintf(text1, "Base Lv. %d", g_session.GetLevel());
		TextOutA(17, 71, text1, 0, 1, 14, 0);

		if (g_session.GetWeight() * 100 / max(g_session.GetMaxWeight(), 1) >= 50) {
			sprintf(text1, "Weight : %3d / %3d", g_session.GetWeight(), g_session.GetMaxWeight());
			TextOutA(5, 103, text1, 0, 1, 14, 0xff);
		} else {
			sprintf(text1, "Weight : %3d / %3d", g_session.GetWeight(), g_session.GetMaxWeight());
			TextOutA(5, 103, text1, 0, 1, 14, 0);
		}

		GetNumberText(g_session.GetGold(), moneyTxt);
		sprintf(text1, "Zeny : %s", moneyTxt);
		TextOutA(107, 103, text1, 0, 1, 14, 0);
		break;
	}
	}
}

void UIBasicInfoWnd::OnLBtnDblClk(int x, int y) {
	if (y < 17) SendMsg(this, UM_COMMAND, 0x88, 0, 0, 0);
}



void UIBasicInfoWnd::StoreInfo() {
	if (m_info) {
		m_info->h = m_h;
		m_info->w = m_w;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}

void UIBasicInfoWnd::OnMouseHover(int x, int y)
{
	if (m_h == FULL_SIZE) {
		if (x >= 0 && x < 100 && y >= 100 && y < 120) {
			char helpText[64];
			if (!g_session.GetMaxWeight())
				sprintf(helpText, "Weight %d%%", 0);
			else
				sprintf(helpText, "Weight %d%%", g_session.GetWeight() * 100 / g_session.GetMaxWeight());

			int gx = 0;
			int gy = 81;
			GetGlobalCoor(gx, gy);
			g_modeMgr.GetCurMode()->MakeHelpBalloon(helpText, gx, gy, -1);
		}
	}
}

int UIBasicInfoWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message)
	{
	case UM_SETINFO:
		m_info = (BasicInfoWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();

		NewHeight(m_info->h);
		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_STATUS:
			if (!g_windowMgr.DeleteWindow(WID_STATUSWND))
				g_windowMgr.MakeWindow(WID_STATUSWND);
			break;
		case ID_DETAILLEVEL:
			if (!g_windowMgr.DeleteWindow(WID_DETAILLEVELWND))
				g_windowMgr.MakeWindow(WID_DETAILLEVELWND);
			break;
		case ID_OPTION:
			if (!g_windowMgr.DeleteWindow(WID_OPTIONWND))
				g_windowMgr.MakeWindow(WID_OPTIONWND);
			break;
		case ID_MERCHANTITEMS:
			if (!g_windowMgr.DeleteWindow(WID_MERCHANTITEMWND) && IsEffectStatePushCart(g_session.m_effectState))
				g_windowMgr.MakeWindow(WID_MERCHANTITEMWND);
			break;
		case ID_ITEMS:
			if (!g_windowMgr.DeleteWindow(WID_ITEMWND))
				g_windowMgr.MakeWindow(WID_ITEMWND);
			break;
		case ID_QUEST:
			if (!g_windowMgr.DeleteWindow(WID_QUESTWND))
				g_windowMgr.MakeWindow(WID_QUESTWND);
			break;
		case ID_SKILL:
			if (!g_windowMgr.DeleteWindow(WID_SKILLLISTWND))
				g_windowMgr.MakeWindow(WID_SKILLLISTWND);
			break;
		case ID_EQUIP:
			if (!g_windowMgr.DeleteWindow(WID_EQUIPWND))
				g_windowMgr.MakeWindow(WID_EQUIPWND);
			break;
		case ID_FRIEND:
			if (!g_windowMgr.DeleteWindow(WID_MESSENGERWND))
				g_windowMgr.MakeWindow(WID_MESSENGERWND);
			break;
		case ID_COMM:
			if (!g_windowMgr.DeleteWindow(WID_CHATROOMMAKEWND) && !g_windowMgr.GetChatRoomWnd()) {
				if (!g_windowMgr.GetMerchantShopMakeWnd() && !g_windowMgr.GetMerchantItemMyShopWnd())
					g_windowMgr.MakeWindow(WID_CHATROOMMAKEWND);
				else
					g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_OPEN_STORE_WHILE_CHAT), ME_OK, 1, 0, 0);
			}
			break;
		case ID_CLOSEALL:
			g_windowMgr.DeleteWindow(WID_STATUSWND);
			g_windowMgr.DeleteWindow(WID_OPTIONWND);
			g_windowMgr.DeleteWindow(WID_ITEMWND);
			g_windowMgr.DeleteWindow(WID_QUESTWND);
			g_windowMgr.DeleteWindow(WID_EQUIPWND);
			g_windowMgr.DeleteWindow(WID_MESSENGERWND);
			g_windowMgr.DeleteWindow(WID_CHATROOMMAKEWND);
			g_windowMgr.DeleteWindow(WID_CHATROOMCHANGEWND);
			g_windowMgr.DeleteWindow(WID_SKILLLISTWND);
			g_windowMgr.DeleteWindow(WID_TIPOFTHEDAYWND);
			g_windowMgr.DeleteWindow(WID_PETINFOWND);
			g_windowMgr.DeleteWindow(WID_EMOTIONWND);
			g_windowMgr.DeleteWindow(WID_EMOTIONLISTWND);
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMWND);
			g_windowMgr.DeleteWindow(WID_GUILDWND);
			g_windowMgr.ShowWindow(WID_SHORTCUTWND, 0);
			NewHeight(MINI_SIZE);
			g_windowMgr.UpdateSnapInfo(this);
			break;
		case ID_MINI: {
			int oldHeight = m_h;
			if (oldHeight == MINI_SIZE) NewHeight(FULL_SIZE);
			else NewHeight(MINI_SIZE);

			g_windowMgr.UpdateSnapInfo(this);
			g_windowMgr.MakeTopLayer(this);

			if (m_h - oldHeight) g_snapMgr.MoveConnectedObject(this, 0, m_h - oldHeight, 3);
			break;
		}
		}
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIToolTipWnd::UIToolTipWnd() {}
UIToolTipWnd::~UIToolTipWnd() {}

void UIToolTipWnd::OnCreate(int cx, int cy)
{
	m_toolTipList = new UIListBox;

	int w = cx - 24;
	int h = cy - 60;
	if (w < 20)
		w = 20;
	if (h < 30)
		h = 30;

	m_toolTipList->Create2(12, 22, w, h, true);
	m_toolTipList->SetId(ID_OK);
	m_toolTipList->AddItem("__________");
	m_toolTipList->AddItem("**********");
	AddChild(m_toolTipList);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ m_w - 91, m_h - 24 },
		{ m_w - 46, m_h - 24 },
	};

	for (int i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}
}


int UIToolTipWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			g_windowMgr.PostQuit(this);
			break;
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
UIEquipWnd::UIEquipWnd() { m_info = NULL; m_stateStartTick = timeGetTime(); m_barHeight = 0; }
UIEquipWnd::~UIEquipWnd() {}

void UIEquipWnd::OnCreate(int cx, int cy)
{
	if (m_typeWnd == 0)
		m_barHeight = 20;
	else
		m_barHeight = 0;

	if (cy != m_h + m_barHeight)
		Resize(m_w, m_h + m_barHeight);

	int ids2[3];
	int posX[3];
	posX[0] = 3;
	posX[1] = 252;
	posX[2] = 266;
	ids2[0] = ID_BASE;
	ids2[1] = ID_MINI;
	ids2[2] = ID_CLOSE;
	char* buttonName2[8] = { "base", "mini", "close" };
	char* toolTip[8] = { "", "Ctrl+Q", "Alt+Q" };
	int i;

	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
	for (i = 0; i < 3; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(posX[i], 3);
		button->SetId(ids2[i]);
		if (m_typeWnd == 0)
			button->SetToolTip(toolTip[i]);
		AddChild(button);
	}

	if (m_typeWnd == 0) {
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\");

		int pos[1][2];
		pos[0][0] = m_w / 2 - 18;
		pos[0][1] = m_h - m_barHeight - 40;

		m_optionOffButton = new UIBitmapButton;
		m_optionOffButton->SetBitmapName((pathName + "btn_off" + ".bmp").c_str(), BS_NORMAL);
		m_optionOffButton->SetBitmapName((pathName + "btn_off" + ".bmp").c_str(), BS_MOUSEON);
		m_optionOffButton->SetBitmapName((pathName + "btn_off" + ".bmp").c_str(), BS_PRESSED);
		m_optionOffButton->Create(m_optionOffButton->GetBitmapWidth(), m_optionOffButton->GetBitmapHeight());
		m_optionOffButton->Move(pos[0][0], pos[0][1] + HasOptionOffButton() * 230);
		m_optionOffButton->SetId(ID_EQUIP_OPTION_OFF);
		m_optionOffButton->SetToolTip(GetOptionOffToolTipText());
		AddChild(m_optionOffButton);
	}

	if (m_typeWnd == 0) {
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\");

		int pos2[1][2];
		pos2[0][0] = m_w / 2 - 14;
		pos2[0][1] = m_h - m_barHeight - 60;

		m_optionOpenButton = new UIBitmapButton;
		m_optionOpenButton->SetBitmapName((pathName + "btn_items_off" + ".bmp").c_str(), BS_NORMAL);
		m_optionOpenButton->SetBitmapName((pathName + "btn_items_on" + ".bmp").c_str(), BS_MOUSEON);
		m_optionOpenButton->SetBitmapName((pathName + "btn_items_on" + ".bmp").c_str(), BS_PRESSED);
		m_optionOpenButton->Create(m_optionOpenButton->GetBitmapWidth(), m_optionOpenButton->GetBitmapHeight());
		m_optionOpenButton->Move(pos2[0][0], pos2[0][1] + HasOptionOpenButton() * 230);
		m_optionOpenButton->SetId(ID_EQUIP_OPTION_OPEN);
		m_optionOpenButton->SetToolTip(GetOptionOpenToolTipText());
		AddChild(m_optionOpenButton);
	}

	m_orgHeight = m_h;
	m_highlightMode[0] = -1;
	m_highlightMode[1] = -1;
	m_highlightMode[2] = -1;
}

void UIEquipWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (m_typeWnd != 0)
		return;

	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	if (dragInfo->m_dragType == DT_FROM_ITEMWND) {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(dragInfo->m_dragItemIndex);
		if (itemInfo.m_itemIndex) {
			if (itemInfo.m_itemType == 10 || itemInfo.m_itemType == 16 || itemInfo.m_itemType == 17) {
				gameMode->SendMsg(MM_WEAR_EQUIP_ARROW, dragInfo->m_dragItemIndex, itemInfo.m_location, 0);
			} else if (itemInfo.m_location && itemInfo.m_wearLocation == 0 && itemInfo.m_isIdentified) {
				gameMode->SendMsg(MM_WEAR_EQUIP, dragInfo->m_dragItemIndex, itemInfo.m_location, 0);
			}
		}
	}

	g_modeMgr.GetCurMode()->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}

void UIEquipWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);

	if (m_typeWnd != 0)
		return;

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		if (itemInfo.m_num > 0) {
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_BEGIN_DRAG_FROM_EQUIPMWND, itemInfo.m_wearLocation, 0, 0);
		}
	}
}

void UIEquipWnd::OnLBtnDblClk(int x, int y)
{
	if (y < 17)
		SendMsg(this, UM_COMMAND, 0x88, 0, 0, 0);

	if (m_typeWnd != 0)
		return;

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		if (itemInfo.m_num > 0) {
			int slotNum[3];
			g_session.BitMaskToSlotNum(itemInfo.m_wearLocation, slotNum);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_TAKE_OFF_EQUIP, slotNum[0], 0, 0);
		}
	}
}

void UIEquipWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}

ITEM_INFO UIEquipWnd::GetItemInfo(int x, int y)
{
	int wearLocation = -1;

	if (x >= 6 && x < 30) {
		if (y >= 17 && y < 41)
			wearLocation = 8;
		else if (y >= 43 && y < 67)
			wearLocation = 0;
		else if (y >= 69 && y < 93)
			wearLocation = 1;
		else if (y >= 95 && y < 119)
			wearLocation = 2;
		else if (y >= 121 && y < 145)
			wearLocation = 3;
	} else if (x >= 251 && x < 275) {
		if (y >= 17 && y < 41)
			wearLocation = 9;
		else if (y >= 43 && y < 67)
			wearLocation = 4;
		else if (y >= 69 && y < 93)
			wearLocation = 5;
		else if (y >= 95 && y < 119)
			wearLocation = 6;
		else if (y >= 121 && y < 145)
			wearLocation = 7;
	}

	if (m_w / 2 - 25 < x && x < m_w / 2 - 1 && y > 44 && y < 68) wearLocation = 15;

	if (m_typeWnd)
		return g_session.GetOtherUserEquipedItems(wearLocation);
	return g_session.GetEquipedItems(wearLocation);
}


void UIEquipWnd::OnMouseMove(int x, int y) {
	UIFrameWnd::OnMouseMove(x, y);
	OnMouseHover(x, y);
}

void UIEquipWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		char itemName[128];
		char itemHelp[128];
		itemInfo.m_isIdentified = 1;
		itemInfo.GetItemName(itemName);

		int gx;
		if (x < m_w / 3)
			gx = 6;
		else if (x > m_w * 2 / 3)
			gx = 251;
		else
			gx = m_w / 2 - 20;

		int gy;
		if (y < 41)
			gy = -3;
		else if (y < 67)
			gy = 23;
		else if (y < 93)
			gy = 49;
		else if (y < 119)
			gy = 75;
		else
			gy = 101;

		GetGlobalCoor(gx, gy);
		CMode* mode = g_modeMgr.GetCurMode();
		if (itemInfo.m_num <= 1) {
			mode->MakeHelpBalloon(itemName, gx, gy, g_session.GetItemDisColor(&itemInfo));
		} else {
			sprintf(itemHelp, MsgStr(MSI_EA2), itemName, itemInfo.m_num);
			mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
		}
	}

	int oldHighlight[3];
	oldHighlight[0] = m_highlightMode[0];
	oldHighlight[1] = m_highlightMode[1];
	oldHighlight[2] = m_highlightMode[2];
	m_highlightMode[2] = m_highlightMode[1] = m_highlightMode[0] = -1;

	CGameMode* gameMode = g_modeMgr.GetGameMode();
	int dragType = gameMode->GetDragType();
	if (dragType == DT_FROM_ITEMWND) {
		int dragItemIndex = gameMode->GetDragItemIndex();
		int height = m_h - 17;
		int newY = y - 17;
		if (x >= 0 && y >= 0 && x < m_w && y < m_h) {



			while (newY < height && newY > height) {}
			ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(dragItemIndex);
			if (itemInfo.m_itemIndex && itemInfo.m_location) {
				int slotNum[3];
				g_session.BitMaskToSlotNum(itemInfo.m_location, slotNum);
				m_highlightMode[0] = slotNum[0];
				if (slotNum[1] != -1)
					m_highlightMode[1] = slotNum[1];
				if (slotNum[2] != -1)
					m_highlightMode[2] = slotNum[2];
			}
		}
	}

	if (oldHighlight[0] != m_highlightMode[0] ||
		oldHighlight[1] != m_highlightMode[1] ||
		oldHighlight[2] != m_highlightMode[2]) {
		Invalidate();
	}
}

void UIEquipWnd::StoreInfo() {
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->orgHeight = m_orgHeight;
	}
}

void UIEquipWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_fix.bmp"), false);
	if (bmp)
		DrawBitmap(0, 0, bmp, false);

	if (m_typeWnd == 1) {
		char buf[100];
		sprintf(buf, MsgStr(MSI_OTHERUSER_EQUIPED_ITEM), g_session.m_otherUserName);
		TextOutA(17, 4, buf, 0, 0, 12, 0xffffff);
		TextOutA(16, 3, buf, 0, 0, 12, 0);
	} else {
		TextOutA(17, 4, MsgStr(MSI_EQUIPED_ITEM), 0, 0, 12, 0xffffff);
		TextOutA(16, 3, MsgStr(MSI_EQUIPED_ITEM), 0, 0, 12, 0);
	}

	if (m_h == 17)
		return;

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\equipwin_bg.bmp"), false);
	if (bmp)
		DrawBitmap(0, 17, bmp, false);

	for (int hi = 0; hi < 3; ++hi) {
		int mode = m_highlightMode[hi];
		switch (mode) {
		case 0: case 1: case 2: case 3:
		case 4: case 5: case 6: case 7:
		{
			int posX[2] = { 4, 248 };
			bmp = (CBitmapRes*)g_resMgr().Get(
				UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\item_invert.bmp"), false);
			if (bmp)
				DrawBitmap(posX[mode / 4], (mode % 4) * 26 + 51, bmp, true);
			break;
		}
		case 8: case 9:
		{
			int posX[2] = { 4, 248 };
			bmp = (CBitmapRes*)g_resMgr().Get(
				UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\item_invert.bmp"), false);
			if (bmp)
				DrawBitmap(posX[mode % 4], 25, bmp, true);
			break;
		}
		}
	}

	int txtX[8] = { 28, 28, 28, 28, 167, 167, 167, 167 };
	int posX[2] = { 6, 251 };
	int txtY[8] = { 0, 26, 52, 78, 0, 26, 52, 78 };
	int i;
	for (i = 0; i < 8; ++i) {
		ITEM_INFO equipedItem;
		if (m_typeWnd)
			equipedItem = g_session.GetOtherUserEquipedItems(i);
		else
			equipedItem = g_session.GetEquipedItems(i);
		if (equipedItem.m_itemIndex) {
			if (equipedItem.m_num == 1) {
				DrawItem(posX[i / 4], (i % 4) * 26 + 44, equipedItem, false);
				DrawItemTextRect(txtX[i] + 5, txtY[i] + 51, 70, equipedItem, 0, 0, 12);
			}
		}
	}

	{
	int posX[2] = { 6, 251 };
	int txtX[2] = { 28, 167 };
	for (i = 8; i < 10; ++i) {
		ITEM_INFO equipedItem;
		if (m_typeWnd)
			equipedItem = g_session.GetOtherUserEquipedItems(i);
		else
			equipedItem = g_session.GetEquipedItems(i);
		if (equipedItem.m_itemIndex) {
			if (equipedItem.m_num == 1) {
				DrawItem(posX[i % 4], 18, equipedItem, false);
				DrawItemTextRect(txtX[i - 8] + 5, 25, 70, equipedItem, 0, 0, 12);
			}
		}
	}
	}

	int DoubleAcc;
	std::string imfName;
	std::string actName[5];
	std::string sprName[5];
	std::string bodyPaletteName;
	std::string headPaletteName;
	CacheInfo info;

	CTexture* tex;
	int job;
	int sex;
	int head = 0;
	int accessory;
	int accessory2;
	int accessory3;
	int bodyPalette;
	int headPalette;
	if (m_typeWnd) {
		job = g_session.m_otherUserJob;
		sex = g_session.m_otherUserSex;
		head = g_session.m_otherUserHead;
		accessory = g_session.m_otherUserAccessory;
		accessory2 = g_session.m_otherUserAccessory2;
		accessory3 = g_session.m_otherUserAccessory3;
		bodyPalette = g_session.m_otherUserBodyPalette;
		headPalette = g_session.m_otherUserHeadPalette;
	} else {
		job = g_session.GetJob();
		sex = g_session.GetSex();
		head = g_session.m_head;
		accessory = g_session.m_accessory;
		accessory2 = g_session.m_accessory2;
		accessory3 = g_session.m_accessory3;
		bodyPalette = g_session.m_bodyPalette;
		headPalette = g_session.m_headPalette;
	}

	static int headX;
	static int headY;
	char tmpName[256];
	actName[0] = g_session.GetJobActName(job, sex, tmpName);
	sprName[0] = g_session.GetJobSprName(job, sex, tmpName);
	actName[1] = g_session.GetHeadActName(job, head, sex, tmpName);
	sprName[1] = g_session.GetHeadSprName(job, head, sex, tmpName);
	actName[2] = g_session.GetAccessoryActName(job, head, sex, accessory, tmpName);
	sprName[2] = g_session.GetAccessorySprName(job, head, sex, accessory, tmpName);
	actName[3] = g_session.GetAccessoryActName(job, head, sex, accessory2, tmpName);
	sprName[3] = g_session.GetAccessorySprName(job, head, sex, accessory2, tmpName);
	actName[4] = g_session.GetAccessoryActName(job, head, sex, accessory3, tmpName);
	sprName[4] = g_session.GetAccessorySprName(job, head, sex, accessory3, tmpName);
	if (bodyPalette) {
		char palName[64];
		bodyPaletteName = g_session.GetBodyPaletteName(job, sex, bodyPalette, palName);
	}
	if (headPalette) {
		char palName[64];
		headPaletteName = g_session.GetHeadPaletteName(head, job, sex, headPalette, palName);
	}
	imfName = g_session.GetImfName(job, head, sex, tmpName);

	DoubleAcc = 0;
	for (i = 0; i < 5; i++) {
		g_resMgr().Get(imfName.c_str(), false);
		if (actName[i].size() && sprName[i].size()) {
			CActRes* actRes = (CActRes*)g_resMgr().Get(actName[i].c_str(), false);
			CSprRes* sprRes = (CSprRes*)g_resMgr().Get(sprName[i].c_str(), false);
			{
				char accname[256];
				char buf[1024];

				if (DoubleAcc == 1)
					sprintf(accname, "_\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd");
				else if (DoubleAcc == 2)
					sprintf(accname, "_\xb4\xab\xb0\xa1\xb8\xae\xb0\xb3");
				else if (DoubleAcc == 3)
					sprintf(accname, "_\xc7\xef\xb8\xa7");

				if (DoubleAcc > 0) {
					sprintf(buf, "sprite\\\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.act",
						g_session.m_newPcSexNameTable[sex], g_session.m_newPcSexNameTable[sex], accname);
					actRes = (CActRes*)g_resMgr().Get(buf, false);
					sprintf(buf, "sprite\\\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.spr",
						g_session.m_newPcSexNameTable[sex], g_session.m_newPcSexNameTable[sex], accname);
					sprRes = (CSprRes*)g_resMgr().Get(buf, false);
					DoubleAcc = 100;
				}
			}

			CMotion* curMotion = actRes->GetMotion(0, 0);
			int offX = 0;
			int offY = 0;

			if (i == 1) {
				CMotion* headMotion = curMotion;
				CMotion* bodyMotion = ((CActRes*)g_resMgr().Get(actName[0].c_str(), false))->GetMotion(0, 0);
				if (headMotion->attachCnt && bodyMotion->attachCnt) {
					if (headMotion->attachInfo[0].m_attr == bodyMotion->attachInfo[0].m_attr) {
						offX = bodyMotion->attachInfo[0].x - headMotion->attachInfo[0].x;
						offY = bodyMotion->attachInfo[0].y - headMotion->attachInfo[0].y;
						headX = offX;
						headY = offY;
					}
				}
			} else if (i == 2 || i == 3 || i == 4) {
				CMotion* accMotion = curMotion;
				CMotion* headMotion = ((CActRes*)g_resMgr().Get(actName[1].c_str(), false))->GetMotion(0, 0);
				int acc = 0;
				if (i == 2)
					acc = accessory;
				else if (i == 3)
					acc = accessory2;
				else if (i == 4)
					acc = accessory3;

				if (g_windowMgr.IsAniAccessory(acc)) {
					int headCount = ((CActRes*)g_resMgr().Get(actName[1].c_str(), false))->GetMotionCount(0);
					int accCount = ((CActRes*)g_resMgr().Get(actName[i].c_str(), false))->GetMotionCount(0);
					if (headCount > 0 && accCount > 0 && accCount != headCount &&
						accCount > headCount && accCount % headCount == 0) {
						int step = accCount / headCount;
						int motionSpeed = (int)((CActRes*)g_resMgr().Get(actName[i].c_str(), false))->GetDelay(0);
						float frame = (timeGetTime() - m_stateStartTick) / 24.0f;
						if (motionSpeed == 0)
							motionSpeed = 4;
						int motion = (int)(frame / motionSpeed) % step;
						int tmpMotion = 0 * step + motion;
						curMotion = accMotion = ((CActRes*)g_resMgr().Get(actName[i].c_str(), false))->GetMotion(0, tmpMotion);
					}
				}

				if (accMotion->attachCnt && headMotion->attachCnt) {
					if (accMotion->attachInfo[0].m_attr == headMotion->attachInfo[0].m_attr) {
						offX = headMotion->attachInfo[0].x - accMotion->attachInfo[0].x;
						offY = headMotion->attachInfo[0].y - accMotion->attachInfo[0].y;
					}
				}

				offX += headX;
				offY += headY;
				if (i == 3) {
					switch (accessory2) {
					case 0xb5:
					case 0xc5:
						offY -= 7;
						break;
					}
				}
			}

			unsigned long* pal = sprRes->m_pal;
			switch (i) {
			case 0:
				if (bodyPalette) {
					CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(bodyPaletteName.c_str(), false);
					pal = palRes->m_pal;
				}
				break;
			case 1:
				if (headPalette) {
					CPaletteRes* palRes = (CPaletteRes*)g_resMgr().Get(headPaletteName.c_str(), false);
					pal = palRes->m_pal;
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
				tex = g_renderer->GetSpriteIndex(*img, (unsigned long)pal, &info);
				if (!tex)
					tex = g_renderer->AddSpriteIndex(*img, pal, &info);
			} else {
				tex = img->tex;
			}

			int clipPosy2 = 0;
			int clipPosy = clip->y - m_barHeight + offY + m_h - 18;
			if (clipPosy < 0) {
				clipPosy2 = clipPosy;
				clipPosy = 0;
			}

			m_dc->BltTexture2(m_w / 2 + clip->x + offX + 1, clipPosy, tex,
				(int)(info.tu * 256.0f), (int)(info.tv * 256.0f - clipPosy2),
				img->width, img->height + clipPosy2,
				clip->flags, img->isHalfW + 1, img->isHalfH + 1);

			if (DoubleAcc == 100) {
				DoubleAcc = 0;
			} else if (i == 4) {
				--i;
				if (accessory3 == 0x120 || accessory3 == 0x121)
					DoubleAcc = 1;
				else if (accessory3 == 0x124 || accessory3 == 0x125)
					DoubleAcc = 2;
				else
					++i;
			} else if (i == 3) {
				--i;
				if (accessory2 == 0x122 || accessory2 == 0x123)
					DoubleAcc = 3;
				else
					++i;
			}
		}
	}

	if (m_typeWnd == 0) {
		ITEM_INFO arrowItem = g_session.GetItemInfoByIndex(g_session.GetEquipArrowIndex());
		if (arrowItem.m_itemIndex) {
			DrawItem(m_w / 2 - 25, 44, arrowItem, false);
			DrawItemText(m_w / 2 - 25, 68, arrowItem, 0, 0, 12);
		}
	}
}

int UIEquipWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (EquipWndInfo*)val1;
		if (!m_info->IsValidInfo()) m_info->MakeDefault();

		if (m_info->h != m_info->orgHeight) SendMsg(NULL, UM_COMMAND, 0x88, 0, 0, 0);

		Move(m_info->x, m_info->y);
		break;

	case UM_REFRESH: {
		if (m_typeWnd == 0)
			m_openEquipWndCheckBox->SetCheck2(g_session.m_bOpenEquipedItem);

		int pos[2] = { m_w / 2 - 18, m_h - 40 };
		m_optionOffButton->Move(pos[0], pos[1] + HasOptionOffButton() * 230);
		pos[0] = m_w / 2 - 14;
		pos[1] = m_h - 60;
		m_optionOpenButton->Move(pos[0], pos[1] + HasOptionOpenButton() * 230);
		Invalidate();
		break;
	}

	case UM_COMMAND:
		switch (val1) {
		case ID_CHECK:
			if (m_typeWnd == 0)
				g_modeMgr.GetCurMode()->SendMsg(MM_OPEN_EQUIPEDITEM_CHANGE, val2, 0, 0);
			break;

		case ID_EQUIP_OPTION_OPEN:
			if (!g_windowMgr.DeleteWindow(WID_MERCHANTITEMWND)) {
				if (IsEffectStatePushCart(g_session.m_effectState))
					g_windowMgr.MakeWindow(WID_MERCHANTITEMWND);
			}
			break;

		case ID_EQUIP_OPTION_OFF: {
			if (g_windowMgr.GetMerchantItemMyShopWnd()) {
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_PLEASE_CLOSE_STORE), 0xff, 0, 0);
				break;
			}

			SendOptionOffToMode();

			int pos[2] = { m_w / 2 - 18, m_h };
			int pos2[2] = { m_w / 2 - 14, m_h };
			m_optionOffButton->Move(pos[0], pos[1] - 40 + HasOptionOffButton() * 230);
			m_optionOpenButton->Move(pos2[0], pos2[1] - 60 + HasOptionOpenButton() * 230);
			Invalidate();
			break;
		}

		case ID_MINI: {
			int oldHeight = m_h;
			if (oldHeight == 17) {
				Resize(m_w, m_orgHeight);
			} else {
				m_orgHeight = oldHeight;
				Resize(m_w, 17);
			}

			g_windowMgr.UpdateSnapInfo(this);
			g_windowMgr.MakeTopLayer(this);

			if (m_h - oldHeight) g_snapMgr.MoveConnectedObject(this, 0, m_h - oldHeight, 3);
			break;
		}

		case ID_CLOSE:
			if (m_typeWnd == 1)
				g_windowMgr.DeleteWindow((WINDOWID)0x92);
			else
				g_windowMgr.DeleteWindow(WID_EQUIPWND);
			break;
		}
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

bool UIEquipWnd::HasOptionOffButton() {
	if (IsEffectStatePushCart(g_session.m_effectState) ||
		IsEffectStateBird(g_session.m_effectState) ||
		IsEffectStateChicken(g_session.m_effectState))
		return false;
	return true;
}

const char* UIEquipWnd::GetOptionOffToolTipText() {
	if (IsEffectStatePushCart(g_session.m_effectState))
		return MsgStr(MSI_CARTOFF);
	if (IsEffectStateBird(g_session.m_effectState))
		return MsgStr(MSI_BIRDOFF);
	if (IsEffectStateChicken(g_session.m_effectState))
		return MsgStr(MSI_CHIKENOFF);
	return "";
}



bool UIEquipWnd::HasOptionOpenButton() {
	return IsEffectStatePushCart(g_session.m_effectState) ? false : true;
}

const char* UIEquipWnd::GetOptionOpenToolTipText() {
	if (IsEffectStatePushCart(g_session.m_effectState))
		return MsgStr(MSI_OPENCARTWINDOW);
	return "";
}

void UIEquipWnd::SendOptionOffToMode() {
	CMode* mode = g_modeMgr.GetCurMode();
	if (IsEffectStatePushCart(g_session.m_effectState)) {
		mode->SendMsg(MM_REQ_CARTOFF, 0, 0, 0);
	} else if (IsEffectStateBird(g_session.m_effectState)) {
		mode->SendMsg(MM_REQ_BIRDOFF, 0, 0, 0);
	} else if (IsEffectStateChicken(g_session.m_effectState)) {
		mode->SendMsg(MM_REQ_CHIKENOFF, 0, 0, 0);
	}
}
UIStatusWnd::UIStatusWnd() { m_state = 0; m_info = NULL; }
UIStatusWnd::~UIStatusWnd() {}

void UIStatusWnd::OnCreate(int cx, int cy)
{
	int posX[3] = { 3, 252, 266 };
	int ids2[3] = { ID_BASE, ID_MINI, ID_CLOSE };
	char* buttonName2[8] = { "base", "mini", "close" };
	char* tooltip[8] = { "", "Ctrl+A", "Alt+A" };

	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

	int i;
	for (i = 0; i < 3; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(posX[i], 3);
		button->SetId(ids2[i]);
		button->SetToolTip(tooltip[i]);
		AddChild(button);
	}

	int posY[6] = { 6, 22, 38, 54, 70, 86 };
	int ids[6] = { ID_STR, ID_AGI, ID_VIT, ID_INT, ID_DEX, ID_LUK };

	std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\arw_right.bmp";

	for (i = 0; i < 6; ++i) {
		m_incArrowButton[i] = new UIBitmapButton;
		m_incArrowButton[i]->SetBitmapName(preBmpName.c_str(), BS_NORMAL);
		m_incArrowButton[i]->SetBitmapName(preBmpName.c_str(), BS_MOUSEON);
		m_incArrowButton[i]->SetBitmapName(preBmpName.c_str(), BS_PRESSED);
		m_incArrowButton[i]->Create(m_incArrowButton[i]->GetBitmapWidth(), m_incArrowButton[i]->GetBitmapHeight());
		m_incArrowButton[i]->Move(88, posY[i] + 17);
		m_incArrowButton[i]->SetId(ids[i]);
		AddChild(m_incArrowButton[i]);
	}

	m_orgHeight = m_h;
}

void UIStatusWnd::ChangeState(int state)
{
	int posY[6];
	int count = 6;

	Invalidate();
	m_state = state;
	int x = 0;
	if (state == 1)
		x = -340;
	x += 88;
	posY[0] = count;
	posY[1] = 22;
	posY[2] = 38;
	posY[3] = 54;
	posY[4] = 70;
	posY[5] = 86;
	state = x;

	for (int i = 0; count; --count, ++i)
		m_incArrowButton[i]->Move(state, posY[i] + 17);

	SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
}
void UIStatusWnd::OnLBtnDown(int x, int y)
{
	if (x < 20) {
		if (y >= 17 && y < 43) {
			if (m_state) {
				ChangeState(0);
			}
		} else if (y >= 43 && y < 67) {
			if (m_state != 1) {
				ChangeState(1);
			}
		}
	}

	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}

void UIStatusWnd::OnLBtnDblClk(int x, int y) {
	if (y < 17)
		SendMsg(this, UM_COMMAND, 0x88, 0, 0, 0);
}
void UIStatusWnd::OnMouseHover(int x, int y)
{
	if (m_state) return;
	int pos[16][2] = {
		{ 25, 22 },
		{ 25, 38 },
		{ 25, 54 },
		{ 25, 70 },
		{ 25, 86 },
		{ 25, 102 },
		{ 118, 22 },
		{ 118, 38 },
		{ 118, 54 },
		{ 118, 70 },
		{ 118, 86 },
		{ 118, 102 },
		{ 199, 22 },
		{ 199, 38 },
		{ 199, 54 },
		{ 199, 70 },
	};

	const char* descript[16] = {
		MsgStr(MSI_DESC_STR),
		MsgStr(MSI_DESC_AGI),
		MsgStr(MSI_DESC_VIT),
		MsgStr(MSI_DESC_INT),
		MsgStr(MSI_DESC_DEX),
		MsgStr(MSI_DESC_LUK),
		MsgStr(MSI_DESC_ATK),
		MsgStr(MSI_DESC_MATK),
		MsgStr(MSI_DESC_HIT),
		MsgStr(MSI_DESC_CRI),
		MsgStr(MSI_DESC_POINT),
		MsgStr(MSI_DESC_GUILD),
		MsgStr(MSI_DESC_DEF),
		MsgStr(MSI_DESC_MDEF),
		MsgStr(MSI_DESC_FLEE),
		MsgStr(MSI_DESC_ASPD),
	};
	for (int i = 0; i < 16; ++i) {
		if (x >= pos[i][0] && x < pos[i][0] + 28 && y >= pos[i][1] && y < pos[i][1] + 13) {
			int gx = pos[i][0];
			int gy = pos[i][1] - 19;
			GetGlobalCoor(gx, gy);

			g_modeMgr.GetCurMode()->MakeHelpBalloon(descript[i], gx, gy, -1);
			return;
		}
	}

}

void UIStatusWnd::OnDraw()
{
	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_fix.bmp"), false);
	DrawBitmap(0, 0, bitmap, false);
	TextOutA(17, 4, MsgStr(MSI_STATUS), 0, 0, 12, 0xffffff);
	TextOutA(16, 3, MsgStr(MSI_STATUS), 0, 0, 12, 0);

	if (m_h == 17)
		return;
	if (m_state)
		return;

	bitmap = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\statwin0_bg.bmp"), false);
	DrawBitmap(0, 17, bitmap, false);

	int posY[6] = { 6, 22, 38, 54, 70, 86 };
	const int basicParams[6] = { g_session.GetStr(), g_session.GetAgi(), g_session.GetVit(),
		g_session.GetInt(), g_session.GetDex(), g_session.GetLuk() };
	const int basicPlusParams[6] = { g_session.GetPlusStr(), g_session.GetPlusAgi(), g_session.GetPlusVit(),
		g_session.GetPlusInt(), g_session.GetPlusDex(), g_session.GetPlusLuk() };
	char text[12];

	for (int i = 0; i < 6; i++) {
		sprintf(text, "%d", basicParams[i]);
		TextOutA(54, posY[i] + 16, text, 0, 1, 14, 0);
		if (basicPlusParams[i]) {
			TextOutA(67, posY[i] + 16, "+", 0, 1, 14, 0);
			sprintf(text, "%d", basicPlusParams[i]);
			TextOutA(74, posY[i] + 16, text, 0, 1, 14, 0);
		}
	}

	CGameActor* actor = g_session.GetGameActorByAid(1);
	if (actor) {
		int textLen = sprintf(text, "%d + %d", actor->m_Str * 10,
			g_modeMgr.GetGameMode()->GetWeaponPower(actor->GetWeapon()));
		TextOutWithRightAlign(192, 21, text, textLen, 1, 14, 0);

		textLen = sprintf(text, "%d + %d", actor->m_Vit * 5, 0);
		TextOutWithRightAlign(273, 21, text, textLen, 1, 14, 0);
	}
}

int UIStatusWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	CMode* mode = g_modeMgr.GetCurMode();

	switch (message) {
	case UM_REFRESHBUTTON: {
		if (m_state)
			break;

		int point = g_session.GetPoint();
		const int basicParams[6] = { g_session.GetStr(), g_session.GetAgi(), g_session.GetVit(),
			g_session.GetInt(), g_session.GetDex(), g_session.GetLuk() };
		int levelupAmount[6] = {
			*g_session.GetStandardStrPointer(),
			*g_session.GetStandardAgiPointer(),
			*g_session.GetStandardVitPointer(),
			*g_session.GetStandardIntPointer(),
			*g_session.GetStandardDexPointer(),
			*g_session.GetStandardLukPointer()
		};
		int posY[6] = { 6, 22, 38, 54, 70, 86 };

		for (int i = 0; i < 6; ++i) {
			int x = -340;
			if (levelupAmount[i] <= point && basicParams[i] < 99)
				x = 0;
			if (!levelupAmount[i])
				x = -340;
			m_incArrowButton[i]->Move(x + 88, posY[i] + 17);
		}

		Invalidate();
		break;
	}

	case UM_SETINFO:
		m_info = (StatusWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();

		SendMsg(NULL, UM_RESIZE, 7, m_info->w, m_info->orgHeight, 0);
		if (m_info->h != m_info->orgHeight)
			SendMsg(NULL, UM_COMMAND, ID_MINI, 0, 0, 0);
		SendMsg(NULL, UM_REFRESHBUTTON, 0, 0, 0, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_MINI: {
			int oldHeight = m_h;
			if (oldHeight == 17) {
				Resize(m_w, m_orgHeight);
			} else {
				m_orgHeight = oldHeight;
				Resize(m_w, 17);
			}

			g_windowMgr.UpdateSnapInfo(this);
			g_windowMgr.MakeTopLayer(this);

			int dy = m_h - oldHeight;
			if (dy)
				g_snapMgr.MoveConnectedObject(this, 0, dy, 3);
			break;
		}

		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_STATUSWND);
			break;

		case ID_STR:
			if (*g_session.GetStandardStrPointer() <= g_session.GetPoint()) {
				mode->SendMsg(MM_STATUS_CHANGE, ID_STR - ID_STATUS, 1, 0);
				SendMsg(this, UM_REFRESHBUTTON, 0, 0, 0, 0);
			}
			break;

		case ID_AGI:
			if (*g_session.GetStandardAgiPointer() <= g_session.GetPoint()) {
				mode->SendMsg(MM_STATUS_CHANGE, ID_AGI - ID_STATUS, 1, 0);
				SendMsg(this, UM_REFRESHBUTTON, 0, 0, 0, 0);
			}
			break;

		case ID_VIT:
			if (*g_session.GetStandardVitPointer() <= g_session.GetPoint()) {
				mode->SendMsg(MM_STATUS_CHANGE, ID_VIT - ID_STATUS, 1, 0);
				SendMsg(this, UM_REFRESHBUTTON, 0, 0, 0, 0);
			}
			break;

		case ID_INT:
			if (*g_session.GetStandardIntPointer() <= g_session.GetPoint()) {
				mode->SendMsg(MM_STATUS_CHANGE, ID_INT - ID_STATUS, 1, 0);
				SendMsg(this, UM_REFRESHBUTTON, 0, 0, 0, 0);
			}
			break;

		case ID_LUK:
			if (*g_session.GetStandardLukPointer() <= g_session.GetPoint()) {
				mode->SendMsg(MM_STATUS_CHANGE, ID_LUK - ID_STATUS, 1, 0);
				SendMsg(this, UM_REFRESHBUTTON, 0, 0, 0, 0);
			}
			break;

		case ID_DEX:
			if (*g_session.GetStandardDexPointer() <= g_session.GetPoint()) {
				mode->SendMsg(MM_STATUS_CHANGE, ID_DEX - ID_STATUS, 1, 0);
				SendMsg(this, UM_REFRESHBUTTON, 0, 0, 0, 0);
			}
			break;
		}
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIStatusWnd::StoreInfo() {
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->orgHeight = m_orgHeight;
	}
}
UIDetailLevelWnd::UIDetailLevelWnd() { m_info = NULL; }
UIDetailLevelWnd::~UIDetailLevelWnd() {}

void UIDetailLevelWnd::OnCreate(int cx, int cy)
{
	{
		int posX[3] = { 3, 252, 266 };
		int ids2[3] = { ID_BASE, ID_MINI, ID_CLOSE };
		char* buttonName2[8] = { "base", "mini", "close" };
		std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
		char* toolTip[8] = { "", "", "Alt+D" };

		int i;
		for (i = 0; i < 3; ++i) {
			if (i == 1)
				continue;

			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(posX[i], 3);
			button->SetId(ids2[i]);
			button->SetToolTip(toolTip[i]);
			AddChild(button);
		}
	}

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

		char* buttonName[2][3] = { { "btn_ok", "btn_ok_a", "btn_ok_b" }, { "btn_cancel", "btn_cancel_a", "btn_cancel_b" } };
		int ids[2] = { ID_OK, ID_CANCEL };
		int pos[2][2] = { { 189, 96 },
						  { 234, 96 } };

		int i;
		for (i = 0; i < 2; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			AddChild(button);
		}

		m_defPushId = ID_OK;
	}

	m_textureDetailLevel = 0;
	m_spriteDetailLevel = 0;
	m_trilinearFilter = g_registry.m_trilinearFilter;

	m_spriteDetailLevel = g_registry.m_spriteMode;
	if (m_spriteDetailLevel < 0)
		m_spriteDetailLevel = 0;
	if (m_spriteDetailLevel > 2)
		m_spriteDetailLevel = 2;

	m_textureDetailLevel = g_registry.m_textureMode;
	if (m_textureDetailLevel < 0)
		m_textureDetailLevel = 0;
	if (m_textureDetailLevel > 2)
		m_textureDetailLevel = 2;

	m_spriteDetailLevelScrbar = new UIScrollBar;
	m_spriteDetailLevelScrbar->Create(34, 38, 0, m_w - 78);
	m_spriteDetailLevelScrbar->SetRange(2, 1, 3);
	m_spriteDetailLevelScrbar->SetPos(m_spriteDetailLevel);
	AddChild(m_spriteDetailLevelScrbar);

	m_textureDetailLevelScrbar = new UIScrollBar;
	m_textureDetailLevelScrbar->Create(34, 70, 0, m_w - 78);
	m_textureDetailLevelScrbar->SetRange(2, 1, 3);
	m_textureDetailLevelScrbar->SetPos(m_textureDetailLevel);
	AddChild(m_textureDetailLevelScrbar);

	RAG_BUDGET_FILL(16)
	m_trilinearCheckBox = new UICheckBox;
	m_trilinearCheckBox->Create(m_w / 2, 12);
	m_trilinearCheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		"Trilinear");
	m_trilinearCheckBox->Move(6, m_h - 16);
	m_trilinearCheckBox->SetCheck(m_trilinearFilter);
	AddChild(m_trilinearCheckBox);
}

void UIDetailLevelWnd::OnDraw()
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	ClearDC(RGB(rr, gg, bb));

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_fix.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnBarMidCount = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (int i = 0; i < btnBarMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	TextOutA(34, 22, MsgStr(MSI_SPRITE_RESOLUTION), 0, 0, 12, 0);
	TextOutA(34, 54, MsgStr(MSI_TEXTURE_RESOLUTION), 0, 0, 12, 0);
	TextOutA(17, 4, MsgStr(MSI_DETAILLEVELWND), 0, 0, 12, 0xffffff);
	TextOutA(16, 3, MsgStr(MSI_DETAILLEVELWND), 0, 0, 12, 0);
}

void UIDetailLevelWnd::StoreInfo() {
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}

int UIDetailLevelWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_CHECK:
			if (sender == m_openDetailWindowCheckBox) {
				g_session.m_bAutoOpenDetailWindowIfLowMemory = 1 - val2;
			} else if (sender == m_trilinearCheckBox) {
				m_trilinearFilter = val2;
			}
			break;
		case ID_CANCEL:
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_DETAILLEVELWND);
			break;
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_DETAILLEVELWND);

			g_registry.m_spriteMode = m_spriteDetailLevel;
			switch (m_spriteDetailLevel) {
			case 0:
				g_isXHalfSpr = 1;
				g_isYHalfSpr = 1;
				break;
			case 1:
				g_isXHalfSpr = 0;
				g_isYHalfSpr = 1;
				break;
			default:
				g_isXHalfSpr = 0;
				g_isYHalfSpr = 0;
				break;
			}

			g_registry.m_textureMode = m_textureDetailLevel;
			int texLenDivFactor;
			switch (m_textureDetailLevel) {
			case 0:
				texLenDivFactor = 4;
				break;
			case 1:
				texLenDivFactor = 2;
				break;
			default:
				texLenDivFactor = 1;
				break;
			}

			bool reloadTexture = false;
			if (texLenDivFactor != g_texLenDivFactor) {
				g_texLenDivFactor = texLenDivFactor;
				reloadTexture = true;
			}

			if (g_registry.m_trilinearFilter != (unsigned long)m_trilinearFilter) {
				g_registry.m_trilinearFilter = m_trilinearFilter;
				g_3dDevice.EnableMipmap();
				reloadTexture = true;
			}

			if (reloadTexture) g_texMgr.ReloadAllTexture();
			break;
		}
		default:
			break;
		}
		break;

	case UM_SETINFO:
		m_info = (DetailLevelWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		break;

	case UM_SCROLLHORZ:
	case UM_SCROLLHORZPAGEUP:
	case UM_SCROLLHORZPAGEDOWN:
		if (m_spriteDetailLevelScrbar == sender) {
			int oldLevel = m_spriteDetailLevel;
			if (message == UM_SCROLLHORZPAGEUP)
				m_spriteDetailLevel = oldLevel - 1;
			else if (message == UM_SCROLLHORZPAGEDOWN)
				m_spriteDetailLevel = oldLevel + 1;
			else
				m_spriteDetailLevel = oldLevel + val1;

			if (m_spriteDetailLevel < 0)
				m_spriteDetailLevel = 0;
			if (m_spriteDetailLevel > 2)
				m_spriteDetailLevel = 2;
			m_spriteDetailLevelScrbar->SetPos(m_spriteDetailLevel);
			m_spriteDetailLevelScrbar->Invalidate();
			Invalidate();
			return m_spriteDetailLevel - oldLevel;
		}

		if (m_textureDetailLevelScrbar == sender) {
			int oldLevel = m_textureDetailLevel;
			if (message == UM_SCROLLHORZPAGEUP)
				m_textureDetailLevel = oldLevel - 1;
			else if (message == UM_SCROLLHORZPAGEDOWN)
				m_textureDetailLevel = oldLevel + 1;
			else
				m_textureDetailLevel = oldLevel + val1;

			if (m_textureDetailLevel < 0)
				m_textureDetailLevel = 0;
			if (m_textureDetailLevel > 2)
				m_textureDetailLevel = 2;
			m_textureDetailLevelScrbar->SetPos(m_textureDetailLevel);
			m_textureDetailLevelScrbar->Invalidate();
			Invalidate();
			return m_textureDetailLevel - oldLevel;
		}
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}


UIOptionWnd::UIOptionWnd()
{
	m_info = NULL;
}
UIOptionWnd::~UIOptionWnd() {}

void UIOptionWnd::OnCreate(int cx, int cy)
{
	int posX[3] = { 3, 252, 266 };
	int ids2[3] = { ID_BASE, ID_MINI, ID_CLOSE };
	char* buttonName2[8] = { "base", "mini", "close" };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
	char* toolTip[8] = { "", "Ctrl+O", "Alt+O" };
	for (int i = 0; i < 3; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(posX[i], 3);
		button->SetId(ids2[i]);
		button->SetToolTip(toolTip[i]);
		AddChild(button);
	}

	m_orgHeight = m_h;

	m_skinSelectButton = new UINoneVisibleButton;
	m_skinSelectButton->Create(181, 16);
	m_skinSelectButton->Move(75, 64);
	m_skinSelectButton->SetId(ID_SKINSELECTCOMBO);
	m_skinSelectButton->SetToolTip("Select Skin");
	AddChild(m_skinSelectButton);

	m_bgmVolume = g_session.m_BgmVolume;
	m_soundVolume = Get2DVolume();

	m_bgmVolumescrBar = new UIScrollBar;
	m_bgmVolumescrBar->Create(71, 21, 0, m_w - 115);
	m_bgmVolumescrBar->SetRange(0x7f, 1, 0x80);
	m_bgmVolumescrBar->SetPos(m_bgmVolume);
	AddChild(m_bgmVolumescrBar);

	m_soundVolumescrBar = new UIScrollBar;
	m_soundVolumescrBar->Create(71, 40, 0, m_w - 115);
	m_soundVolumescrBar->SetRange(0x7f, 1, 0x80);
	m_soundVolumescrBar->SetPos(m_soundVolume);
	AddChild(m_soundVolumescrBar);

	m_bgmOncheckBox = new UICheckBox;
	m_bgmOncheckBox->Create(26, 12);
	m_bgmOncheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		"on");
	m_bgmOncheckBox->Move(m_w - 43, 22);
	m_bgmOncheckBox->SetCheck(1 - g_session.m_bgmIsPaused);
	AddChild(m_bgmOncheckBox);

	m_soundOncheckBox = new UICheckBox;
	m_soundOncheckBox->Create(26, 12);
	m_soundOncheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		"on");
	m_soundOncheckBox->Move(m_w - 43, 41);
	m_soundOncheckBox->SetCheck(g_isSoundOn);
	AddChild(m_soundOncheckBox);

	m_attackSnapCheckBox = new UICheckBox;
	m_attackSnapCheckBox->Create(48, 12);
	m_attackSnapCheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		"attack");
	m_attackSnapCheckBox->Move(112, 100);
	m_attackSnapCheckBox->SetCheck(g_session.m_monsterSnapOn_NoSkill);
	AddChild(m_attackSnapCheckBox);

	m_skillSnapCheckBox = new UICheckBox;
	m_skillSnapCheckBox->Create(48, 12);
	m_skillSnapCheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		"skill");
	m_skillSnapCheckBox->Move(162, 100);
	m_skillSnapCheckBox->SetCheck(g_session.m_monsterSnapOn_Skill);
	AddChild(m_skillSnapCheckBox);

	m_itemSnapCheckBox = new UICheckBox;
	m_itemSnapCheckBox->Create(48, 12);
	m_itemSnapCheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		"item");
	m_itemSnapCheckBox->Move(204, 100);
	m_itemSnapCheckBox->SetCheck(g_session.m_isItemSnap);
	AddChild(m_itemSnapCheckBox);

	m_noCtrlCheckBox = new UICheckBox;
	m_noCtrlCheckBox->Create(52, 12);
	m_noCtrlCheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		"NoCtrl");
	m_noCtrlCheckBox->Move(11, 100);
	m_noCtrlCheckBox->SetCheck(g_session.m_isNoCtrl);
	AddChild(m_noCtrlCheckBox);
}

void UIOptionWnd::OnLBtnDblClk(int x, int y) {
	if (y < 17)
		SendMsg(this, UM_COMMAND, 0x88, 0, 0, 0);
}

void UIOptionWnd::OnLBtnDown(int x, int y)
{
	if (m_state == 0) {
		m_skinSelectButton->Move(75, 64);
	} else {
		m_skinSelectButton->Move(75, -200);
	}
	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
	}
}

void UIOptionWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_fix.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
	TextOutA(17, 4, MsgStr(MSI_OPTIONWND), 0, 0, 12, 0xffffff);
	TextOutA(16, 3, MsgStr(MSI_OPTIONWND), 0, 0, 12, 0);

	if (m_h != 17) {
		if (!m_state) {
			bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\optwin0_bg.bmp"), false);
			DrawBitmap(0, 17, bmp, false);
			bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\txtbox_btn_a.bmp"), false);
			DrawBitmap(240, 64, bmp, false);
		} else {
			bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\optwin1_bg.bmp"), false);
			DrawBitmap(0, 17, bmp, false);
		}

		TextOutA(78, 68, g_skinMgr.GetCurrentSkinName(), 0, 0, 12, 0);
		TextOutA(76, 100, MsgStr(MSI_SNAP), 0, 0, 12, 0);
	}
}

void UIOptionWnd::StoreInfo() {
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->orgHeight = m_orgHeight;
	}
}

int UIOptionWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (OptionWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();

		SendMsg(NULL, UM_RESIZE, 7, m_info->w, m_info->orgHeight, 0);
		if (m_info->h != m_info->orgHeight)
			SendMsg(NULL, UM_COMMAND, ID_MINI, 0, 0, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_MINI: {
			int oldHeight = m_h;
			if (oldHeight == 17) {
				Resize(m_w, m_orgHeight);
			} else {
				m_orgHeight = oldHeight;
				Resize(m_w, 17);
			}

			g_windowMgr.UpdateSnapInfo(this);
			g_windowMgr.MakeTopLayer(this);

			int dy = m_h - oldHeight;
			if (dy) {
				g_snapMgr.MoveConnectedObject(this, 0, dy, 3);
			}
			break;
		}

		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_OPTIONWND);
			break;

		case ID_CHECK:
			if (sender == m_soundOncheckBox) {
				g_isSoundOn = val2;
			} else if (sender == m_bgmOncheckBox) {
				g_session.m_bgmIsPaused = 1 - val2;
				g_modeMgr.GetCurMode()->SendMsg(MM_REFRESH_MUSIC, 0, 0, 0);
			} else if (sender == m_attackSnapCheckBox) {
				g_session.m_monsterSnapOn_NoSkill = val2;
			} else if (sender == m_skillSnapCheckBox) {
				g_session.m_monsterSnapOn_Skill = val2;
			} else if (sender == m_itemSnapCheckBox) {
				g_session.m_isItemSnap = val2;
			} else if (sender == m_noCtrlCheckBox) {
				g_session.m_isNoCtrl = val2;
			}
			break;

		case ID_SKINSELECTCOMBO: {
			if (g_windowMgr.GetComboBoxWnd())
				g_windowMgr.DeleteWindow(WID_COMBOBOXWND);

			int skinSize = g_skinMgr.GetNumSkin();
			UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)"<Basic Skin>", 0, 0, 0);

			for (int i = 0; i < skinSize; ++i)
				comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)g_skinMgr.GetSkinName(i), 0, 0, 0);

			int height = min((skinSize + 1) * 16, 80);

			comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, g_skinMgr.GetCurrentSkinNum() + 1, 0, 0, 0);
			comboBoxWnd->Resize(181, height);
			comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_OPTIONWND, 0, 0, 0);

			int gx = 74;
			int gy = 81;
			GetGlobalCoor(gx, gy);
			comboBoxWnd->Move(gx + 1, gy + 1);
			break;
		}
		}
		break;

	case UM_REFRESH:
		m_bgmVolume = g_session.m_BgmVolume;
		m_soundVolume = Get2DVolume();
		m_bgmVolumescrBar->SetPos(m_bgmVolume);
		m_bgmVolumescrBar->Invalidate();
		m_soundVolumescrBar->SetPos(m_soundVolume);
		m_soundVolumescrBar->Invalidate();
		m_bgmOncheckBox->SetCheck(1 - g_session.m_bgmIsPaused);
		m_soundOncheckBox->SetCheck(g_isSoundOn);
		m_attackSnapCheckBox->SetCheck(g_session.m_monsterSnapOn_NoSkill);
		m_skillSnapCheckBox->SetCheck(g_session.m_monsterSnapOn_Skill);
		m_itemSnapCheckBox->SetCheck(g_session.m_isItemSnap);
		m_noCtrlCheckBox->SetCheck(g_session.m_isNoCtrl);
		Invalidate();
		break;

	case UM_COMBOBOX_SELECTED: {
		int skinNum = val1 - 1;
		if (g_skinMgr.GetCurrentSkinNum() != skinNum) {
			g_windowMgr.InvalidateAll();
			g_skinMgr.SetCurrentSkinNum(skinNum);
		}
		break;
	}

	case UM_SCROLLHORZ:
	case UM_SCROLLHORZPAGEUP:
	case UM_SCROLLHORZPAGEDOWN:
		if (m_bgmVolumescrBar == sender) {
			int oldVolume = m_bgmVolume;
			if (message == UM_SCROLLHORZPAGEUP)
				m_bgmVolume = oldVolume - 10;
			else if (message == UM_SCROLLHORZPAGEDOWN)
				m_bgmVolume = oldVolume + 10;
			else
				m_bgmVolume = oldVolume + val1;

			if (m_bgmVolume < 0)
				m_bgmVolume = 0;
			if (m_bgmVolume > 127)
				m_bgmVolume = 127;

			m_bgmVolumescrBar->SetPos(m_bgmVolume);
			m_bgmVolumescrBar->Invalidate();
			Invalidate();
			SetStreamVolume(m_bgmVolume);
			g_session.m_BgmVolume = m_bgmVolume;
			return m_bgmVolume - oldVolume;
		}
		if (m_soundVolumescrBar == sender) {
			int oldVolume = m_soundVolume;
			if (message == UM_SCROLLHORZPAGEUP)
				m_soundVolume = oldVolume - 10;
			else if (message == UM_SCROLLHORZPAGEDOWN)
				m_soundVolume = oldVolume + 10;
			else
				m_soundVolume = oldVolume + val1;

			if (m_soundVolume < 0)
				m_soundVolume = 0;
			if (m_soundVolume > 127)
				m_soundVolume = 127;

			m_soundVolumescrBar->SetPos(m_soundVolume);
			m_soundVolumescrBar->Invalidate();
			Invalidate();
			Set2DVolume(m_soundVolume);
			Set3DVolume(m_soundVolume);
			return m_soundVolume - oldVolume;
		}
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIItemParamChangeDisplayWnd::UIItemParamChangeDisplayWnd()
{
	m_selectedAtk = 0;
	m_selectedDef = 0;
	m_sameSlotAtk = 0;
	m_sameSlotDef = 0;
	m_newAtk = 0;
	m_newDef = 0;
	m_atk = 0;
	m_def = 0;
	m_info = NULL;
	m_isIdentified = 0;
	m_itemName = "512";
}
UIItemParamChangeDisplayWnd::~UIItemParamChangeDisplayWnd() {}

void UIItemParamChangeDisplayWnd::OnCreate(int cx, int cy) {
	SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
}

void UIItemParamChangeDisplayWnd::OnDraw()
{
	char text[128];

	DrawLatticeFrame(0, 0, 0, 0);

	sprintf(text, MsgStr(MSI_ATK));
	TextOutA(20, 10, text, 0, 0, 12, 0);

	sprintf(text, "%d", m_atk);
	TextOutA(70, 10, text, 0, 0, 12, 0);
	if (m_atk < m_newAtk) {
		DrawTriangleDC(94, 11, 90, 19, 98, 19, 0xff);
	} else if (m_atk == m_newAtk) {
		DrawTriangleDC(90, 13, 98, 13, 90, 17, 0x505050);
		DrawTriangleDC(98, 13, 90, 17, 98, 17, 0x505050);
	} else {
		DrawTriangleDC(90, 11, 98, 11, 94, 19, 0xff0000);
	}

	sprintf(text, "%d", m_newAtk);
	TextOutA(110, 10, text, 0, 0, 12, 0);

	sprintf(text, MsgStr(MSI_DEF));
	TextOutA(152, 10, text, 0, 0, 12, 0);

	sprintf(text, "%d", m_def);
	TextOutA(202, 10, text, 0, 0, 12, 0);
	if (m_def < m_newDef) {
		DrawTriangleDC(226, 11, 222, 19, 230, 19, 0xff);
	} else if (m_def == m_newDef) {
		DrawTriangleDC(222, 13, 230, 13, 222, 17, 0x505050);
		DrawTriangleDC(230, 13, 222, 17, 230, 17, 0x505050);
	} else {
		DrawTriangleDC(222, 11, 230, 11, 226, 19, 0xff0000);
	}

	sprintf(text, "%d", m_newDef);
	TextOutA(242, 10, text, 0, 0, 12, 0);
}

void UIItemParamChangeDisplayWnd::RecalcEquipedParam()
{
	m_atk = 0;
	m_def = 0;

	int totalSlotBitmask = 0;
	for (int slot = 0; slot < 8; ++slot) {
		ITEM_INFO itemInfo = g_session.GetEquipedItems(slot);
		if (itemInfo.m_itemIndex) {
			if (itemInfo.m_num == 1) {
				std::string itemResName(g_session.GetItemResName_WithId(itemInfo.m_itemName.c_str(), itemInfo.m_isIdentified));

				int itemSlot = ITEM_INFO::GetItemDesc(atoi(itemResName.c_str())).slot;
				if (itemSlot & totalSlotBitmask) {
					totalSlotBitmask |= itemSlot;
					continue;
				}

				totalSlotBitmask |= itemSlot;
				const ITEM_DESC& itemDesc = ITEM_INFO::GetItemDesc(atoi(itemResName.c_str()));
				if (itemDesc.parameter.size() == 2) {
					m_atk += itemDesc.parameter[0];
					m_def += itemDesc.parameter[1];
				}
			}
		}
	}
}

void UIItemParamChangeDisplayWnd::StoreInfo() {
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

ITEM_INFO UIItemParamChangeDisplayWnd::GetSlotItem(int bitmaskSlot)
{
	if (bitmaskSlot == -1)
		return ITEM_INFO();

	for (int i = 0; i < 8; ++i) {
		ITEM_INFO itemInfo = g_session.GetEquipedItems(i);
		if (itemInfo.m_itemIndex && itemInfo.m_num == 1) {
			if (bitmaskSlot & itemInfo.m_location)
				return itemInfo;
		}
	}

	return ITEM_INFO();
}

#include "rag_budget_sites.h"
int UIItemParamChangeDisplayWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (ItemParamChangeDisplayWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		break;

	case UM_PARAM: {
		RecalcEquipedParam();
		m_itemName = (const char*)val1;
		m_isIdentified = val2;

		std::string itemResName = g_session.GetItemResName_WithId(m_itemName.c_str(), m_isIdentified);
		const ITEM_DESC& itemDesc = ITEM_INFO::GetItemDesc(atoi(itemResName.c_str()));
		int bitMask = ITEM_INFO::GetItemDesc(atoi(itemResName.c_str())).slot;
		if (itemDesc.parameter.size() == 2) {
			m_selectedAtk = itemDesc.parameter[0];
			m_selectedDef = itemDesc.parameter[1];
		}

		m_sameSlotAtk = 0;
		m_sameSlotDef = 0;
		Trace("itemResName(%s) bitMask == %d", itemResName.c_str(), bitMask);

		{
			ITEM_INFO eIntemInfo = GetSlotItem(bitMask);
			if (eIntemInfo.m_itemIndex) {
				std::string eItemResName(g_session.GetItemResName_WithId(eIntemInfo.m_itemName.c_str(), eIntemInfo.m_isIdentified));
				const ITEM_DESC& eItemDesc = ITEM_INFO::GetItemDesc(atoi(eItemResName.c_str()));
				if (eItemDesc.parameter.size() == 2) {
					Trace("eIntemInfo(%s)", eIntemInfo.m_itemName.c_str());
					m_sameSlotAtk = eItemDesc.parameter[0];
					m_sameSlotDef = eItemDesc.parameter[1];
				}
			}
		}

		m_newAtk = m_atk + m_selectedAtk - m_sameSlotAtk;
		m_newDef = m_def + m_selectedDef - m_sameSlotDef;
		Invalidate();
		break;
	}

	case UM_REFRESH: {
		RecalcEquipedParam();

		{
			m_sameSlotAtk = 0;
			m_sameSlotDef = 0;
			std::string itemResName(g_session.GetItemResName_WithId(m_itemName.c_str(), m_isIdentified)); RAG_BUDGET_SITES(3)
			int bitMask = ITEM_INFO::GetItemDesc(atoi(itemResName.c_str())).slot;
			Trace("itemResName(%s) bitMask == %d", itemResName.c_str(), bitMask);

			ITEM_INFO eIntemInfo = GetSlotItem(bitMask);
			if (eIntemInfo.m_itemIndex) {
				std::string eItemResName = g_session.GetItemResName_WithId(eIntemInfo.m_itemName.c_str(), eIntemInfo.m_isIdentified);
				const ITEM_DESC& eItemDesc = ITEM_INFO::GetItemDesc(atoi(eItemResName.c_str()));
				if (eItemDesc.parameter.size() == 2) {
					Trace("eIntemInfo(%s)", eIntemInfo.m_itemName.c_str());
					m_sameSlotAtk = eItemDesc.parameter[0];
					m_sameSlotDef = eItemDesc.parameter[1];
				}
			}
		}

		m_newAtk = m_atk + m_selectedAtk - m_sameSlotAtk;
		Trace("m_atk(%d) m_selectedAtk(%d) m_sameSlotAtk(%d)", m_atk, m_selectedAtk, m_sameSlotAtk);
		m_newDef = m_def + m_selectedDef - m_sameSlotDef;
		Trace("m_def(%d) m_selectedDef(%d) m_sameSlotDef(%d)", m_def, m_selectedDef, m_sameSlotDef);
		Invalidate();
		break;
	}

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UINotifyItemObtainWnd::UINotifyItemObtainWnd()
{
	m_isIdentified = 0;
}
UINotifyItemObtainWnd::~UINotifyItemObtainWnd() {}


void UINotifyItemObtainWnd::OnCreate(int cx, int cy) {}

void UINotifyItemObtainWnd::OnDraw() {
	DrawLatticeFrame(0, 0, 0, 0);
	if (m_itemName.size()) DrawItem(13, 5, m_itemName.c_str(), m_isIdentified);
	int x = 0;
	if (m_isMoney) x = -28;
	x += 41;
	DrawItemObtainTextRect(x, 10, m_w - 50, m_itemInfo, 0, 0, 12);
}

void UINotifyItemObtainWnd::OnProcess()
{
	if (timeGetTime() > m_madeTick + 5000)
		g_windowMgr.DeleteWindow((WINDOWID)0x3a);
}

int UINotifyItemObtainWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_itemInfo = *(ITEM_INFO*)val1;
		return 0;

	case UM_SETTITLE: {
		m_isMoney = val1;
		if (m_isMoney) {
			char mText[24];
			char pickMsg[128];

			sprintf(pickMsg, MsgStr(MSI_GET_ZENY), GetNumberText(val2, mText));
			m_message = pickMsg;
			m_itemName = "";
		} else {
			char nName[128];
			char pickMsg[128];

			m_itemName = (const char*)val3;
			ITEM_INFO dummyItemInfo;
			dummyItemInfo.m_itemName = (const char*)val3;
			dummyItemInfo.m_isIdentified = m_isIdentified;
			dummyItemInfo.m_refiningLevel = 0;
			dummyItemInfo.GetItemName(nName);
			sprintf(pickMsg, MsgStr(MSI_GET_ITEM), nName, val2);
			m_message = pickMsg;
		}

		int width = GetTextWidth(m_message.c_str(), 0, 0, 12, false) + 54;
		width = width / 28 * 28 + (width % 28 ? 28 : 0);
		if (width < 34)
			width = 34;
		if (width != m_w)
			Resize(width, m_h);
		Invalidate();

		m_madeTick = timeGetTime();
		return 0;
	}

	case UM_SET_IDENTIFIY_INFO:
		m_isIdentified = val1 != 0;
		return 0;
	}

	return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
}
UINpcEditDialogWnd::UINpcEditDialogWnd() {}
UINpcEditDialogWnd::~UINpcEditDialogWnd() {}

void UINpcEditDialogWnd::OnCreate(int cx, int cy)
{
	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(m_w - 77, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(12);
	m_editCtrl->SetFrameColor(232, 232, 232);
	m_editCtrl->SetText("0");
	AddChild(m_editCtrl);
	g_windowMgr.SetFocusEdit(m_editCtrl);
	m_editCtrl->MakeSelect();

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		char* buttonName[][3] = {
			{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		};
		int ids[] = { ID_OK };
		int pos[][2] = {
			{ m_w - 55, m_h - 32 },
		};

		for (int index = 0; index < 1; index++) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ids[index]);
			AddChild(button);
		}
	}
	m_naid = -1;
	m_defPushId = ID_OK;
}

int UINpcEditDialogWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		if (val1 == 0x76) {
			if (m_editCtrl->GetTextSize()) {
				CMode* mode = g_modeMgr.GetCurMode();
				int amount = atoi(m_editCtrl->GetText());
				mode->SendMsg(MM_SEND_NUMBER_TO_NPC, m_naid, amount, 0);
				g_windowMgr.DeleteWindow((WINDOWID)0x38);
			}
		}
		break;
	case UM_SETNAID:
		m_naid = val1;
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

void UINpcEditDialogWnd::OnDraw() {
	DrawLatticeFrame(0, 0, 0, 0);
	if (g_serviceType == ServiceBrazil) {
		TextOutA(15, 6, "Insira o Numero", 0, 0, 12, 0);
	} else if (g_serviceType == ServiceVietnam) {
		TextOutA(15, 6, MsgStr(MSI_INIT_ERROR), 0, 0, 12, 0);
	} else {
		TextOutA(15, 6, "Input number ", 0, 0, 12, 0);
	}
}
UINpcTextEditDialogWnd::UINpcTextEditDialogWnd() {}
UINpcTextEditDialogWnd::~UINpcTextEditDialogWnd() {}

void UINpcTextEditDialogWnd::OnCreate(int cx, int cy)
{
	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(m_w - 77, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(70);
	m_editCtrl->SetFrameColor(232, 232, 232);
	m_editCtrl->SetText("");
	AddChild(m_editCtrl);
	g_windowMgr.SetFocusEdit(m_editCtrl);
	m_editCtrl->MakeSelect();

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		char* buttonName[][3] = {
			{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		};
		int ids[] = { ID_OK };
		int pos[][2] = {
			{ m_w - 55, m_h - 32 },
		};

		for (int index = 0; index < 1; index++) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ids[index]);
			AddChild(button);
		}
	}
	m_naid = -1;
	m_defPushId = ID_OK;
}

void UINpcTextEditDialogWnd::OnDraw(){ DrawLatticeFrame(0, 0, 0, 0); }

int UINpcTextEditDialogWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		if (val1 == ID_OK) {
			if (m_editCtrl->GetTextSize()) {
				if (!g_insultFilter.IsGoodSentence(m_editCtrl->GetText())) {
					m_editCtrl->SetTextColor(0xff, 0, 0);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_BAD_SENTANGE), 0xff, 0, 0);
					return 0;
				}
				g_modeMgr.GetCurMode()->SendMsg(MM_SEND_TEXT_TO_NPC, m_naid, (int)m_editCtrl->GetText(), 0);
				g_windowMgr.DeleteWindow(WID_NPCTEXTEDITDIALOGWND);
			}
		}
		break;
	default:
		return UINpcEditDialogWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UISkillNameChangeWnd::UISkillNameChangeWnd() {}
UISkillNameChangeWnd::~UISkillNameChangeWnd() {}

void UISkillNameChangeWnd::OnCreate(int cx, int cy)
{
	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(m_w - 77, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(29);
	m_editCtrl->SetFrameColor(232, 232, 232);
	m_editCtrl->SetText("");
	AddChild(m_editCtrl);
	g_windowMgr.SetFocusEdit(m_editCtrl);
	m_editCtrl->MakeSelect();

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		char* buttonName[][3] = {
			{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		};
		int ids[] = { ID_OK };
		int pos[][2] = {
			{ m_w - 55, m_h - 32 },
		};

		for (int index = 0; index < 1; index++) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ids[index]);
			AddChild(button);
		}
	}
	m_skillId = -1;
	m_defPushId = ID_OK;
}

void UISkillNameChangeWnd::OnDraw() {
	DrawLatticeFrame(0, 0, 0, 0);
	TextOutA(15, 6, m_skillName.c_str(), 0, 0, 12, 0);
}

int UISkillNameChangeWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_SKILL_INFO:
		m_skillId = val1;
		m_skillName.assign(SKILL_INFO::GetSkillNameBySkillId(val1));
		m_editCtrl->SetText(m_skillName.c_str());
		g_windowMgr.SetFocusEdit(m_editCtrl);
		m_editCtrl->MakeSelect();
		break;
	case UM_COMMAND:
		if (val1 == ID_OK) {
			if (m_skillId != -1 && m_editCtrl->GetTextSize()) {
				CMode* mode = g_modeMgr.GetCurMode();
				mode->SendMsg(MM_CHANGE_SKILL_NAME, m_skillId, (int)m_editCtrl->GetText(), 0);
			}
			g_windowMgr.DeleteWindow(WID_SKILLNAMECHANGEWND);
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UIEmailAddressWnd::UIEmailAddressWnd() {}
UIEmailAddressWnd::~UIEmailAddressWnd() {}

void UIEmailAddressWnd::OnCreate(int cx, int cy)
{
	CMode* mode = g_modeMgr.GetCurMode();
	const char* emailAddress = (const char*)mode->SendMsg(LMM_GET_EMAIL_ADDRESS, 0, 0, 0);

	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(m_w - 77, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(49);
	m_editCtrl->SetFrameColor(232, 232, 232);
	m_editCtrl->SetText(emailAddress);
	m_editCtrl->SetMaskChar('*');
	AddChild(m_editCtrl);
	g_windowMgr.SetFocusEdit(m_editCtrl);
	m_editCtrl->MakeSelect();

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		int pos[][2] = { { m_w - 55, m_h - 28 } };
		for (int i = 0; i < 1; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + "btn_ok" + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + "btn_ok_a" + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + "btn_ok_b" + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ID_OK);
			AddChild(button);
		}
	}
	m_defPushId = ID_OK;
}

void UIEmailAddressWnd::OnDraw() {
	DrawLatticeFrame(0, 0, 0, 0);
	if (g_serviceType == 0)
		TextOutA(15, 6, MsgStr(MSI_ENTER_PEOPLE_REG_NUMBER), 0, 0, 12, 0);
	else
		TextOutA(15, 6, MsgStr(MSI_ENTER_EMAIL), 0, 0, 12, 0);
}

int UIEmailAddressWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		g_windowMgr.SetModalResult(val1);
		if (val1 == ID_OK) {
			g_modeMgr.GetCurMode()->SendMsg(LMM_SET_EMAIL_ADDRESS, (int)m_editCtrl->GetText(), 0, 0);
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIPasswordWnd::UIPasswordWnd() {}
UIPasswordWnd::~UIPasswordWnd() {}

void UIPasswordWnd::OnCreate(int cx, int cy)
{
	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(90, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(8);
	m_editCtrl->SetFrameColor(232, 232, 232);
	m_editCtrl->SetMaskChar('*');
	m_editCtrl->SetText("");
	AddChild(m_editCtrl);
	g_windowMgr.SetFocusEdit(m_editCtrl);

	m_newText = new UIStaticText;
	m_newText->Create(m_w - 30, 15);
	m_newText->Move(15, 6);
	m_newText->SetText(MsgStr(MSI_ENTER_PASSWORD), 0);
	AddChild(m_newText);

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		char* buttonName[][3] = {
			{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		};
		int ids[] = { ID_OK };
		int pos[][2] = {
			{ m_w - 55, m_h - 32 },
		};

		for (int index = 0; index < 1; index++) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ids[index]);
			AddChild(button);
		}
	}

	m_state = 0;
	m_roomId = 0;
	m_defPushId = ID_OK;
}
void UIPasswordWnd::OnDraw()        { DrawLatticeFrame(0, 0, 0, 0); }

void UIPasswordWnd::SetShow(int visible) {
	m_show = visible;
}

int UIPasswordWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		if (val1 == 0x76) {
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_REQ_ENTER_ROOM, m_roomId, (int)m_editCtrl->GetText(), 0);
			g_windowMgr.DeleteWindow((WINDOWID)0x1d);
		}
		break;
	case UM_SETROOMID:
		m_roomId = val1;
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UISayDialogWnd::UISayDialogWnd() {
	m_textViewer = NULL;
	m_info = NULL;
}
UISayDialogWnd::~UISayDialogWnd() {}

void UISayDialogWnd::OnCreate(int cx, int cy)
{
	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 10, cx - 20, cy - 40, false);
	AddChild(m_textViewer);
	m_naid = 0;
	m_defPushId = 117;
	m_bClearTextViewer = 0;
}

void UISayDialogWnd::Resize(int cx, int cy)
{
	UIWindow::Resize(cx, cy);
	m_textViewer->Resize(cx - 20, cy - 40);
}

void UISayDialogWnd::StoreInfo() {
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}

int UISayDialogWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_TARGET:
		m_target = val1;
		return 0;

	case UM_SETINFO:
		m_info = (SayDialogWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		return 0;

	case UM_UPPUSH:
		m_textViewer->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return 0;

	case UM_DOWNPUSH:
		m_textViewer->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return 0;

	case UM_COMMAND:
		switch (val1) {
		case ID_NOTHING:
			break;
		case ID_NEXT: {
			DWORD tick = GetTickCount();
			if (tick - g_session.m_dwOldTime < 300)
				break;
			g_session.m_dwOldTime = tick;

			g_modeMgr.GetCurMode()->SendMsg(m_target, m_naid, ID_NEXT, 0);
			m_bClearTextViewer = 1;
			SendMsg(NULL, UM_DISABLENEXTBUTTON, 0, 0, 0, 0);
			return 0;
		}
		default:
		g_modeMgr.GetCurMode()->SendMsg(m_target, m_naid, val1, 0);
		return 0;
		}
		break;

	case UM_SETSAYDIALOG:
		if (val1) {
			if (m_bClearTextViewer) {
				m_textViewer->ClearAllItems();
				m_bClearTextViewer = 0;
			}
			m_textViewer->SendMsg(NULL, UM_SETTEXT, val1, val3, 1, 0);
		}
		m_naid = val2;
		return 0;

	case UM_ENABLENEXTBUTTON:
		m_defPushId = ID_NEXT;
		m_target = MM_REQ_NEXT_SCRIPT;
		SendMsg(NULL, UM_ADD_BUTTON, ID_NEXT, (int)"btn_next", 0, 0);
		Invalidate();
		return 0;

	case UM_ENABLECLOSEBUTTON:
		m_defPushId = ID_CLOSE;
		m_target = MM_CLOSE_DIALOG;
		SendMsg(NULL, UM_ADD_BUTTON, ID_NEXT, (int)"btn_close", 0, 0);
		Invalidate();
		return 0;

	case UM_ADD_BUTTON: {
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";

		UIBitmapButton* button = new UIBitmapButton;
		char* buttonName = (char*)val2;
		button->SetBitmapName((pathName + buttonName + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName + "_a.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName + "_b.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(GetButtonOffset(button->GetBitmapWidth()), m_h - 24);
		button->SetId(val1);
		AddChild(button);
		m_buttonList.push_back(button);
		break;
	}

	case UM_DISABLENEXTBUTTON:
		m_defPushId = ID_NOTHING;
		SendMsg(NULL, UM_DELETE_BUTTON, ID_NEXT, 0, 0, 0);
		Invalidate();
		return 0;

	case UM_DELETE_BUTTON: {
		for (std::list<UIBitmapButton*>::iterator it = m_buttonList.begin(); it != m_buttonList.end(); ++it) {
			if ((*it)->GetId() == val1) {
				(*it)->Move(m_w, m_h);
				m_buttonList.erase(it);
				break;
			}
		}
		break;
	}

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

int UISayDialogWnd::GetButtonOffset(int buttonSize) {
	int x = m_w - 4;
	std::list<UIBitmapButton*>::iterator it;
	for (it = m_buttonList.begin(); it != m_buttonList.end(); ++it) {
		x += -4 - (*it)->GetBitmapWidth();
	}
	return x - buttonSize;
}

UIItemCollectionWnd::UIItemCollectionWnd()
{
	m_info = NULL;
	m_itemInfo.Reset();
	m_petLevel = -1;
	m_petFullness = -1;
	m_petRelationship = -1;
	m_petITID = -1;
	m_isPetNameModified = 0;
}
UIItemCollectionWnd::~UIItemCollectionWnd() {}

void UIItemCollectionWnd::OnCreate(int cx, int cy)
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	m_windowId = WID_ITEMCOLLECTIONWND;

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(91, 31, cx - 98, cy - 40, false);
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);

	int posX[3] = { m_w - 14, };
	int ids2[3] = { ID_CLOSE, };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

	for (int i = 0; i < 1; ++i) {
		UIBitmapButton* sysButton = new UIBitmapButton;
		sysButton->SetBitmapName((preBmpName2 + "close" + "_off.bmp").c_str(), BS_NORMAL);
		sysButton->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_MOUSEON);
		sysButton->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_PRESSED);
		sysButton->Create(sysButton->GetBitmapWidth(), sysButton->GetBitmapHeight());
		sysButton->Move(posX[i], 3);
		sysButton->SetId(ids2[i]);
		sysButton->SetToolTip("ESC");
		AddChild(sysButton);
	}

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";

	char* buttonName[][3] = { { "btn_view", "btn_view_a", "btn_view_b", }, };
	int ids[] = { ID_CARDITEM };
	int pos[][2] = { { 6, 6 }, };

	for (int index = 0; index < 1; index++) {
		m_cardItemButton = new UIBitmapButton;
		m_cardItemButton->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
		m_cardItemButton->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
		m_cardItemButton->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
		m_cardItemButton->Create(m_cardItemButton->GetBitmapWidth(), m_cardItemButton->GetBitmapHeight());
		m_cardItemButton->Move(pos[index][0], pos[index][1]);
		m_cardItemButton->SetId(ids[index]);
		AddChild(m_cardItemButton);
	}
}

void UIItemCollectionWnd::OnDraw()
{
	ClearDC(0xffff00ff);
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\collection_bg.bmp"), false);
	DrawBitmap(0, 0, bmp, true);

	if (m_itemBmp.length()) {
		bmp = (CBitmapRes*)g_resMgr().Get(m_itemBmp.c_str(), false);
		DrawBitmap(10, 11, bmp, true);
	}

	DrawItemText(90, 10, m_itemInfo, m_itemInfo.m_isIdentified ? 0 : 0x808080, 0, 12);

	if (m_h <= 120)
		return;

	DrawLatticeFrame(0, 120, m_w, 29);

	if (m_itemInfo.m_slot[0] && !ITEM_INFO::IsCardItem(m_itemInfo.m_slot[0]))
		return;

	int i = 0;
	if (m_itemInfo.GetSlotCount() <= 0)
		return;

	char itemName[16];
	int x = 4;
	do {
		if (m_itemInfo.m_slot[i]) {
			DrawItem(x, m_h - 26, itoa(m_itemInfo.m_slot[i], itemName, 10), false);
		} else {
			DrawBitmap(x, m_h - 26,
				(CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\empty_card_slot.bmp"), false),
				true);
		}
		++i;
		x += 25;
	} while (i < m_itemInfo.GetSlotCount());
}

int UIItemCollectionWnd::GetCardSlotNum(int x, int y, int &gx, int &gy)
{
	x -= 3;
	int slot = x / 25;
	gx = slot * 25 + 4;
	gy = m_h - 50;
	GetGlobalCoor(gx, gy);

	int cardSlotNum = -1;
	if (y > m_h - 26) {
		int slotX = 0;
		for (int i = 0; i < m_itemInfo.GetSlotCount(); i++) {
			if (m_itemInfo.m_slot[i] == 0)
				return -1;
			if (slotX < x && x < slotX + 25) return i;
			slotX += 25;
		}
	}
	return cardSlotNum;
}

void UIItemCollectionWnd::OnProcess() {}

void UIItemCollectionWnd::OnLBtnDown(int x, int y)
{
	if (x > 60 && y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}

void UIItemCollectionWnd::OnRBtnDown(int x, int y)
{
	int gx, gy;
	int slotNum = GetCardSlotNum(x, y, gx, gy);
	if (slotNum != -1) {
		char itemIdName[64];
		_itoa(m_itemInfo.m_slot[slotNum], itemIdName, 10);

		ITEM_INFO dummyItemInfo;
		int itemId = m_itemInfo.m_slot[slotNum];
		dummyItemInfo.m_isIdentified = 1;
		dummyItemInfo.m_itemType = TYPE_CARD;
		dummyItemInfo.m_refiningLevel = 0;
		dummyItemInfo.SetItemId(itemId);

		UIFrameWnd* itemCollectionWnd = g_windowMgr.MakeWindow(WID_COMBINEDCARDITEMCOLLECTIONWND);
		itemCollectionWnd->SendMsg(NULL, UM_SET_IDENTIFIY_INFO, dummyItemInfo.m_isIdentified, 0, 0, 0);
		itemCollectionWnd->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&dummyItemInfo, 0, 0, 0);
	}
}

void UIItemCollectionWnd::OnMouseHover(int x, int y)
{
	int gx, gy;
	int slotNum = GetCardSlotNum(x, y, gx, gy);
	if (slotNum != -1) {
		char itemName[64];
		ITEM_INFO dummyItemInfo;
		int itemId = m_itemInfo.m_slot[slotNum];
		dummyItemInfo.m_isIdentified = 1;
		dummyItemInfo.m_itemType = TYPE_CARD;
		dummyItemInfo.m_refiningLevel = 0;
		dummyItemInfo.SetItemId(itemId);
		dummyItemInfo.GetItemName(itemName);
		g_modeMgr.GetCurMode()->MakeHelpBalloon(itemName, gx, gy, -1);
	}
}

void UIItemCollectionWnd::StoreInfo() {
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

int UIItemCollectionWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (ItemCollectionWndInfo*)val1;
		if (!m_info->IsValidInfo()) {
			m_info->MakeDefault();
			Move(m_info->x + (g_renderer->m_width - 640) / 2, m_info->y * g_renderer->m_width / 640);
		} else {
			Move(m_info->x, m_info->y);
		}
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(m_windowId);
			break;
		case ID_CARDITEM: {
			UIWindow* wnd = g_windowMgr.MakeWindow(WID_CARDITEMILLUSTWND);
			wnd->SendMsg(NULL, UM_CARDITEMILLUSTRATE, (int)m_itemInfo.m_itemName.c_str(), m_itemInfo.m_isIdentified, 0, 0);
			break;
		}
		}
		break;

	case UM_ITEMEXPLANATION: {
		ITEM_INFO* itemInfo = (ITEM_INFO*)val1;
		if (itemInfo->m_itemName == m_itemInfo.m_itemName &&
			itemInfo->m_refiningLevel == m_itemInfo.m_refiningLevel &&
			itemInfo->m_slot[0] == m_itemInfo.m_slot[0] &&
			itemInfo->m_slot[1] == m_itemInfo.m_slot[1] &&
			itemInfo->m_slot[2] == m_itemInfo.m_slot[2] &&
			itemInfo->m_slot[3] == m_itemInfo.m_slot[3] &&
			!val2) {
			g_windowMgr.DeleteWindow(m_windowId);
			break;
		}

		m_petLevel = -1;
		g_session.IsPetEgg(atoi(itemInfo->m_itemName.c_str()));
		m_itemInfo = *itemInfo;
		m_itemInfo.m_deleteTime = itemInfo->m_deleteTime;

		if (m_itemInfo.m_isIdentified && m_itemInfo.GetSlotCount() && !m_itemInfo.IsManufacturedItem()) {
			if (m_h != 144) {
				Resize(m_w, 149);
				RefreshSnap();
			}
		} else {
			if (m_h != 120) {
				Resize(m_w, 120);
				RefreshSnap();
			}
		}

		m_itemBmp = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\collection\\" +
					std::string(m_itemInfo.GetResName()) + ".bmp";

		const std::vector<char const *>& itemDescribe = m_itemInfo.GetDescription();

		int cardY = ITEM_INFO::IsCardItem(atoi(m_itemInfo.m_itemName.c_str())) * 100 - 94;
		m_cardItemButton->Move(6, cardY);
		RAG_BUDGET_FILL(16)
		m_textViewer->ClearAllItems();

		if (m_itemInfo.m_deleteTime) {
			char buff[260];
			char buf[128];
			tm* deleteTime = localtime((const time_t*)&m_itemInfo.m_deleteTime);
			strftime(buf, 128, MsgStr(MSI_TIME_TYPE2), deleteTime);

			int fontHeight = 32;
			if (g_multiLang) {
				sprintf(buff, "%s%s", MsgStr(MSI_NOTICE_TO_DELETE_TIME), g_language->GetLanguageCharset(true), buf);
			} else {
				sprintf(buff, MsgStr(MSI_NOTICE_TO_DELETE_TIME), buf);
				if (g_serviceType == ServiceIndonesia)
					fontHeight = 24;
			}
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)buff, fontHeight, 0, 0);
		}

		for (int i = 0; i < itemDescribe.size(); ++i) {
			if (g_multiLang) {
				char buff[260];
				sprintf(buff, "%s%s", g_language->GetLanguageCharset(true), itemDescribe[i]);
				m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)buff, 0x1a, 0, 0);
			} else {
				if (g_serviceType == ServiceIndonesia)
					m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)itemDescribe[i], 0x18, 0, 0);
				else
					m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)itemDescribe[i], 0x1a, 0, 0);
			}
		}

		Invalidate();
		g_windowMgr.MakeTopLayer(this);

		if (val2) {
			UIItemParamChangeDisplayWnd* wnd = g_windowMgr.GetItemParamChangeDisplayWnd();
			if (wnd) wnd->SendMsg(NULL, UM_PARAM, (int)m_itemInfo.m_itemName.c_str(), m_itemInfo.m_isIdentified, 0, 0);
		}
		break;
	}

	case UM_SETTEXT: {
		const std::vector<char const *>& itemDescribe = m_itemInfo.GetDescription();
		std::vector<std::string> petString;
		std::string pet;
		char str[64];

		if (m_petLevel != -1) {
			pet = m_petName;
			petString.insert(petString.end(), pet);

			if (m_isPetNameModified)
				pet = "Renamed";
			else
				pet = "Not Renamed";
			petString.insert(petString.end(), pet);

			_itoa(m_petLevel, str, 10);
			pet = "Level : ";
			pet += str;
			petString.insert(petString.end(), pet);

			g_session.GetPetHungryStateText(m_petFullness, str);
			pet = "Hunger : ";
			pet += str;
			petString.insert(petString.end(), pet);

			g_session.GetPetFriendlyStateText(m_petRelationship, str);
			pet = "Intimacy : ";
			pet += str;
			petString.insert(petString.end(), pet);

			if (m_petITID)
				pet = "Accessory : Equipped";
			else
				pet = "Accessory : Unequipped";
			petString.insert(petString.end(), pet);
		}

		for (int i = 0; i < itemDescribe.size(); ++i) {
			petString.insert(petString.end(), std::string(itemDescribe[i]));
		}

		int cardY = ITEM_INFO::IsCardItem(atoi(m_itemInfo.m_itemName.c_str())) * 100 - 94;
		m_cardItemButton->Move(6, cardY);
		m_textViewer->ClearAllItems();

		for (int j = 0; j < petString.size(); ++j) {
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)petString[j].c_str(), 0x1a, 0, 0);
		}

		Invalidate();
		g_windowMgr.MakeTopLayer(this);
		break;
	}

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

bool UIItemCollectionWnd::IsSameCollectionDisplayParameters(ITEM_INFO const &item_info)
{
	return item_info.m_itemName == m_itemInfo.m_itemName &&
		   item_info.m_refiningLevel == m_itemInfo.m_refiningLevel &&
		   item_info.m_slot[0] == m_itemInfo.m_slot[0] &&
		   item_info.m_slot[1] == m_itemInfo.m_slot[1] &&
		   item_info.m_slot[2] == m_itemInfo.m_slot[2] &&
		   item_info.m_slot[3] == m_itemInfo.m_slot[3];
}
UICombinedCardItemCollectionWnd::UICombinedCardItemCollectionWnd() { m_combinedItemInfo = NULL; }
UICombinedCardItemCollectionWnd::~UICombinedCardItemCollectionWnd() {}

void UICombinedCardItemCollectionWnd::OnCreate(int cx, int cy)
{
	UIItemCollectionWnd::OnCreate(cx, cy);
	m_windowId = (WINDOWID)0x52;
}

int UICombinedCardItemCollectionWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		{
			m_combinedItemInfo = (CombinedCardItemCollectionWndInfo*)val1;
			if (!m_combinedItemInfo->IsValidInfo()) {
				m_combinedItemInfo->MakeDefault();
				Move(m_combinedItemInfo->x + (g_renderer->m_width - 640) / 2, m_combinedItemInfo->y * g_renderer->m_width / 640);
			} else {
				Move(m_combinedItemInfo->x, m_combinedItemInfo->y);
			}
		}
		break;

	default:
		return UIItemCollectionWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UICombinedCardItemCollectionWnd::StoreInfo() {
	if (m_combinedItemInfo) {
		m_combinedItemInfo->x = m_x;
		m_combinedItemInfo->y = m_y;
		m_combinedItemInfo->w = m_w;
		m_combinedItemInfo->h = m_h;
	}
}

UISkillDescribeWnd::UISkillDescribeWnd() {}
UISkillDescribeWnd::~UISkillDescribeWnd() {}

void UISkillDescribeWnd::OnCreate(int cx, int cy)
{
	m_skillName = MsgStr(MSI_SKILL_NAME);

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(48, 31, cx - 55, cy - 40, false);

	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);

	int posX[3] = { m_w - 14, };
	int ids2[3] = { ID_CLOSE, };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

	for (int i = 0; i < 1; ++i) {
		sysButton = new UIBitmapButton;
		sysButton->SetBitmapName((preBmpName2 + "close" + "_off.bmp").c_str(), BS_NORMAL);
		sysButton->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_MOUSEON);
		sysButton->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_PRESSED);
		sysButton->Create(sysButton->GetBitmapWidth(), sysButton->GetBitmapHeight());
		sysButton->Move(posX[i], 3);
		sysButton->SetId(ids2[i]);
		sysButton->SetToolTip("ESC");
		AddChild(sysButton);
	}
}

void UISkillDescribeWnd::OnDraw() {
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\skillcollection.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
	if (!m_skillBmp.empty()) {
		CBitmapRes* skillBmp = (CBitmapRes*)g_resMgr().Get(m_skillBmp.c_str(), false);
		DrawBitmap(10, 11, skillBmp, true);
	}
	TextOutA(50, 10, m_title.c_str(), 0, 0, 12, 0);
}

int UISkillDescribeWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		if (val1 == ID_CLOSE)
			g_windowMgr.DeleteWindow(WID_SKILLDESCRIBEWND);
		break;

	case UM_SKILLDESCRIBE: {
		SKILL_INFO skillInfo;
		skillInfo.SKID = val1;
		if (m_skillName == skillInfo.GetName()) {
			g_windowMgr.DeleteWindow(WID_SKILLDESCRIBEWND);
		} else {
			m_title = m_skillName = skillInfo.GetName();

			{
				SKILL_INFO tskillInfo;
				if (val1 >= 0x1f40 && val1 < 0x1f51)
					tskillInfo = g_session.GetHomunSkillItemInfoBySkillId(val1);
				else if (val1 >= 0x2008 && val1 < 0x202e)
					tskillInfo = g_session.GetMerSkillItemInfoBySkillId(val1);
				else
					tskillInfo = g_session.GetSkillItemInfoBySkillId(val1);

				if (tskillInfo.m_isValid && tskillInfo.spcost && tskillInfo.level) {
					char spText[64];
					sprintf(spText, " (Sp : %d)", tskillInfo.spcost);
					m_title += spText;
				}
			}

			std::vector<char const *>& skillDescribe = skillInfo.GetDescription();
			m_skillBmp = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\item\\" +
						 std::string(skillInfo.GetSkillIdName()) + ".bmp";
			m_textViewer->ClearAllItems();

			if (skillDescribe.size() != 0) {
				for (std::vector<char const *>::iterator it = skillDescribe.begin(); it != skillDescribe.end(); ++it) {
					char buff[260];
					sprintf(buff, "%s%s", g_language->GetLanguageCharset(true), *it);
					m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)buff, 0x21, 0, 0);
				}
			} else {
				m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)"Unknown Skill", 0, 0, 1);
			}

			Invalidate();
			g_windowMgr.MakeTopLayer(this);
		}
		break;
	}

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIQuestDetailWnd::UIQuestDetailWnd() {}
UIQuestDetailWnd::~UIQuestDetailWnd() {}

void UIQuestDetailWnd::OnCreate(int cx, int cy)
{
	m_skillName = MsgStr(MSI_SKILL_NAME);

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(21, 131, 307, 122, false);
	m_textViewer->m_bgR = 0xfa;
	m_textViewer->m_bgG = 0xfa;
	m_textViewer->m_bgB = 0xfa;
	AddChild(m_textViewer);

	m_textViewer2 = new UITextViewer;
	m_textViewer2->Create2(21, 281, 307, 42, false);
	m_textViewer2->m_bgR = 0xfa;
	m_textViewer2->m_bgG = 0xfa;
	m_textViewer2->m_bgB = 0xfa;
	AddChild(m_textViewer2);

	int posX[3] = { m_w - 16, };
	int ids2[3] = { ID_CLOSE, };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\que_";

	for (int i = 0; i < 1; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName2 + "close" + "_off.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(posX[i], 3);
		button->SetId(ids2[i]);
		button->SetToolTip("ESC");
		AddChild(button);
	}

	{
		UIBitmapButton* button;
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\";
		button = new UIBitmapButton;
		button->SetBitmapName((pathName + "que_btn_a" + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + "que_btn_b" + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + "que_btn_c" + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(164, 340);
		button->SetId(ID_CLASSCOMBO);
		AddChild(button);

		m_classStringList.push_back(std::string(" "));
		m_mobCurrent = 0;

		m_classText = new UIStaticText;
		m_classText->Create(100, 14);
		m_classText->SetText(m_classStringList[0].c_str(), 0);
		m_classText->Move(40, 343);
		m_classText->m_drawBackGround = 1;
		m_classText->m_backR = 0xf2;
		m_classText->m_backG = 0xf2;
		m_classText->m_backB = 0xf2;
		AddChild(m_classText);
	}
}

void UIQuestDetailWnd::OnDraw()
{
	char bmpName[1024];
	char text[256];

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\quest_window.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	g_session.GetSkillBitmapFileName(ITEM_INFO::GetQuestDesc(m_questID).detailBmp, bmpName);
	bmp = (CBitmapRes*)g_resMgr().Get(bmpName, false);
	if (bmp)
		DrawBitmap(20, 20, bmp, true);

	sprintf(text, "%s", ITEM_INFO::GetQuestDesc(m_questID).questName);
	TextOutA(121, 61, text, 0, 0, 12, 0x969696);
	TextOutA(120, 60, text, 0, 0, 12, 0);

	if (m_mobCount > 0) {
		sprintf(text, "%d", m_mobHunt[m_mobCurrent]);
		TextOutA(231, 345, text, 0, 0, 12, 0x969696);
		TextOutA(230, 344, text, 0, 0, 12, 0);
	}

	if (m_mobTime_Hour > 0 || m_mobTime_Minute > 0) {
		sprintf(text, "%d/%d %2d:%2d", m_mobTime_Month, m_mobTime_Day, m_mobTime_Hour, m_mobTime_Minute);
		TextOutA(265, 345, text, 0, 0, 12, 0x969696);
		TextOutA(264, 344, text, 0, 0, 12, 0);
	}
}

int UIQuestDetailWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_QUEST_DETAIL_WND);
			break;
		case ID_CLASSCOMBO: {
			if (g_windowMgr.GetComboBoxWnd())
				g_windowMgr.DeleteWindow(WID_COMBOBOXWND);
			UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);

			int comboCnt = m_classStringList.size();
			for (int i = 0; i < m_classStringList.size(); ++i)
				comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)m_classStringList[i].c_str(), 0, 0, 0);

			int height = min(comboCnt * 16, 48);
			comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			comboBoxWnd->Resize(140, height);
			comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_QUEST_DETAIL_WND, 0, 0, 0);

			int gx = 20;
			int gy = 340;
			GetGlobalCoor(gx, gy);
			comboBoxWnd->Move(gx + 1, gy);
			break;
		}
		}
		break;

	case UM_SKILLDESCRIBE: {
		m_questID = val1;

		char buff[260];
		sprintf(buff, "%s", ITEM_INFO::GetQuestDesc(val1).summaryTXT);
		m_textViewer->ClearAllItems();
		m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)buff, 50, 0, 0);

		sprintf(buff, "%s", ITEM_INFO::GetQuestDesc(m_questID).objectTXT);
		m_textViewer2->ClearAllItems();
		m_textViewer2->SendMsg(NULL, UM_SETTEXT, (int)buff, 50, 0, 0);

		m_classStringList.clear();

		std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
		while (it != g_session.m_QuestList.end()) {
			if (it->questID == m_questID) {
				m_mobTime_Year = it->m_mobTime_Year;
				m_mobTime_Month = it->m_mobTime_Month;
				m_mobTime_Day = it->m_mobTime_Day;
				m_mobTime_Hour = it->m_mobTime_Hour;
				m_mobTime_Minute = it->m_mobTime_Minute;
				m_mobCount = it->mobCount;
				for (int j = 0; j < m_mobCount; ++j) {
					m_mobID[j] = it->mobID[j];
					m_mobHunt[j] = it->mobHunt[j];
					m_classStringList.push_back(it->mobName[j]);
					m_classText->SetText(m_classStringList[0].c_str(), 0);
				}
				break;
			}
			++it;
		}

		Invalidate();
		break;
	}

	case UM_REFRESHITEM: {
		std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
		while (it != g_session.m_QuestList.end()) {
			if (it->questID == m_questID) {
				m_mobTime_Year = it->m_mobTime_Year;
				m_mobTime_Month = it->m_mobTime_Month;
				m_mobTime_Day = it->m_mobTime_Day;
				m_mobTime_Hour = it->m_mobTime_Hour;
				m_mobTime_Minute = it->m_mobTime_Minute;
				m_Server_Hour = it->m_Server_Hour;
				m_Server_Minute = it->m_Server_Minute;
				m_mobCount = it->mobCount;
				for (int j = 0; j < m_mobCount; ++j) {
					m_mobID[j] = it->mobID[j];
					m_mobHunt[j] = it->mobHunt[j];
				}
				break;
			}
			++it;
		}
		Invalidate();
		break;
	}

	case UM_COMBOBOX_SELECTED:
		m_classText->SetText(m_classStringList[val1].c_str(), 0);
		m_mobCurrent = val1;
		Invalidate();
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UICardItemIllustWnd::UICardItemIllustWnd() {}
UICardItemIllustWnd::~UICardItemIllustWnd() {}

void UICardItemIllustWnd::OnCreate(int cx, int cy)
{
	m_isIdentified = 0;
	m_itemName = MsgStr(MSI_ITEM_NAME);

	int posX[3] = { m_w - 14 };
	int ids2[3] = { ID_CLOSE };

	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

	UIBitmapButton* button = new UIBitmapButton;
	button->SetBitmapName((preBmpName2 + "close" + "_off.bmp").c_str(), BS_NORMAL);
	button->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_MOUSEON);
	button->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_PRESSED);
	button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
	button->Move(posX[0], 3);
	button->SetId(ids2[0]);
	AddChild(button);
}

void UICardItemIllustWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleBarMidCount = (m_w - 24) / 12 + (((m_w - 24) % 12) ? 1 : 0);
	for (int i = 0; i < titleBarMidCount; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	if (m_itemBmp.size()) {
		bmp = (CBitmapRes*)g_resMgr().Get(m_itemBmp.c_str(), false);
		DrawBitmap(0, 17, bmp, false);
	}

	ITEM_INFO dummyItemInfo;
	dummyItemInfo.m_itemName = m_itemName;
	dummyItemInfo.m_isIdentified = m_isIdentified;
	dummyItemInfo.m_refiningLevel = 0;
	DrawItemText(10, 3, dummyItemInfo, 0, 0, 12);
}

int UICardItemIllustWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		if (val1 == ID_CLOSE)
			g_windowMgr.DeleteWindow(WID_CARDITEMILLUSTWND);
		break;
	case UM_CARDITEMILLUSTRATE: {
		const char* itemName = (const char*)val1;
		m_isIdentified = val2 != 0;

		const char* itemResName = ITEM_INFO::GetCardDesc(atoi(itemName)).illustName;
		if (!ITEM_INFO::IsCardItem(atoi(itemName))) {
			g_windowMgr.DeleteWindow(WID_CARDITEMILLUSTWND);
			return 0;
		}

		if (m_itemName == itemName) {
			g_windowMgr.DeleteWindow(WID_CARDITEMILLUSTWND);
			return 0;
		}

		m_itemName = itemName;
		m_itemBmp = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\cardBmp\\";
		m_itemBmp += itemResName;
		m_itemBmp += ".bmp";
		Invalidate();
		g_windowMgr.MakeTopLayer(this);
		break;
	}
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIChooseWarpWnd::UIChooseWarpWnd()
{
	m_mapNameList.reserve(3);
}
UIChooseWarpWnd::~UIChooseWarpWnd() {}

void UIChooseWarpWnd::OnCreate(int cx, int cy)
{
	UIStaticText* title = new UIStaticText;
	title->Create(m_w - 30, 15);
	title->Move(12, 9);
	title->SetText(MsgStr(MSI_SELECTWHERETOWARP), 0);
	title->SetFont(0, 12);
	AddChild(title);

	m_choostList = new UIListBox;

	int w = cx - 24;
	int h = cy - 55;
	if (w < 20)
		w = 20;
	if (h < 30)
		h = 30;

	m_choostList->Create2(12, 26, w, h, false);
	m_choostList->m_bR = 0xf0;
	m_choostList->m_bG = 0xf0;
	m_choostList->m_bB = 0xf0;
	m_choostList->SetId(ID_OK);
	AddChild(m_choostList);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ m_w - 92, m_h - 24 },
		{ m_w - 46, m_h - 24 },
	};

	for (int i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = ID_OK;
}

int UIChooseWarpWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_DOWNPUSH:
		m_choostList->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		break;

	case UM_UPPUSH:
		m_choostList->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			int curItem = m_choostList->GetSelected();
			if (curItem == -1)
				break;
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			gameMode->SendMsg(MM_SELECT_WARPPOINT, (int)m_mapNameList[curItem].c_str(), m_skid, 0);
			g_windowMgr.DeleteWindow(WID_CHOOSEWARPWND);
			break;
		}
		case ID_CANCEL:
			RAG_BUDGET_FILL(8)
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			gameMode->SendMsg(MM_SELECT_WARPPOINT, (int)"cancel", m_skid, 0);
			g_windowMgr.DeleteWindow(WID_CHOOSEWARPWND);
			break;
		}
		break;

	case UM_SETSKID:
		m_skid = val1;
		break;

	case UM_SETCHOOSE:
		if (!val1)
			break;
		if (std::string((char*)val1) == "Random")
			m_choostList->AddItem(MsgStr(MSI_RANDOM_POS));
		else if (std::string((char*)val1) == "cancel")
			m_choostList->AddItem(MsgStr(MSI_CANCEL));
		else
			m_choostList->AddItem(g_session.GetMapName(g_session.GatName2RswName((char*)val1)));
		m_mapNameList.push_back(std::string((char*)val1));
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIChooseWnd::UIChooseWnd() : m_info(NULL) { m_index = -1; }
UIChooseWnd::~UIChooseWnd() { m_itemIndex.clear(); }

void UIChooseWnd::OnCreate(int cx, int cy)
{
	m_choostList = new UIListBox;

	int toolW = cx - 24;
	int toolH = cy - 55;
	if (toolW < 20)
		toolW = 20;
	if (toolH < 30)
		toolH = 30;

	m_choostList->Create2(12, 26, toolW, toolH, false);
	m_choostList->m_bR = 0xf0;
	m_choostList->m_bG = 0xf0;
	m_choostList->m_bB = 0xf0;
	m_choostList->SetId(ID_OK);
	AddChild(m_choostList);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b", },
	};
	int ids[] = { ID_OK };
	int pos[][2] = {
		{ m_w - 92, m_h - 24 },
	};

	for (int index = 0; index < 1; index++) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[index][0], pos[index][1]);
		button->SetId(ids[index]);
		AddChild(button);
	}
	m_defPushId = ID_OK;
	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
		char* buttonName[][3] = {
			{ "btn_cancel", "btn_cancel_a", "btn_cancel_b", },
		};
		int ids[] = { ID_CANCEL };
		int pos[][2] = {
			{ m_w - 46, m_h - 24 },
		};

		for (int index = 0; index < 1; index++) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ids[index]);
			AddChild(button);
		}
	}
}

void UIChooseWnd::StoreInfo() {
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}

int UIChooseWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_DOWNPUSH:
		m_choostList->SendMsg(NULL, UM_DOWNPUSH, 0, 0, 0, 0);
		return 0;
	case UM_SETINFO:
		m_info = (ChooseWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		return 0;
	case UM_UPPUSH:
		m_choostList->SendMsg(NULL, UM_UPPUSH, 0, 0, 0, 0);
		return 0;
	case UM_COMMAND: {
		switch (val1) {
		case ID_OK: {
		DWORD tick = GetTickCount();
		DWORD lastTick = g_session.m_dwOldTime;
		if (tick - lastTick < 300)
			break;
		g_session.m_dwOldTime = tick;
		if (m_itemIndex.empty()) {
			SendMsg(NULL, UM_COMMAND, ID_CANCEL, 0, 0, 0);
			return 0;
		}
		if (m_choostList->GetSelected() == -1)
			break;
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		gameMode->SendMsg(MM_CHOOSE_MENU, m_itemIndex[m_choostList->GetSelected()] + 1, m_naid, 0);
		g_windowMgr.DeleteWindow(WID_CHOOSEWND);
		return 0;
		}
		case ID_CANCEL: {
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			gameMode->SendMsg(MM_CHOOSE_MENU, -1, m_naid, 0);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			return 0;
		}
		}
		break;
	}
	case UM_SETCHOOSE: {
		char* msg = (char*)val1;
		if (!msg)
			break;
		++m_index;
		if (strstr(msg, MsgStr(MSI_REF_UNEQUIP)))
			break;

		int sizeChr = 0;
		int itemId = 0;
		if (IsValidItemIndicator(msg, msg + strlen(msg), &sizeChr, &itemId)) {
			std::string str;
			if (itemId) {
				char buf[260];
				ITEM_INFO item;
				item.SetItemId(itemId);
				item.GetIdDisplayName(buf);
				str = buf;
			}
			m_choostList->AddItem(str.c_str());
		}
		else {
			m_choostList->AddItem(msg);
		}
		m_itemIndex.push_back(m_index);
		break;
	}
	case UM_SETNAID: {
		static int count;
		Trace("UM_SETNAID %d\n", count++);
		m_naid = val1;
		break;
	}
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIItemIdentifyWnd::UIItemIdentifyWnd() {
	m_itemIdentifyWndInfo = NULL;
}
UIItemIdentifyWnd::~UIItemIdentifyWnd() {}

void UIItemIdentifyWnd::OnCreate(int cx, int cy)
{
	m_title.assign(MsgStr(MSI_ITEM_IDENTIFY));

	m_viewOffset = 0;
	m_curItem = -1;
	m_itemHorzNum = 1;
	m_itemVertNum = 4;
	m_vertScrEnabled = 0;

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 34, 1, m_h - 55);
	AddChild(m_scrollBar);
	RecalcScrbarPos();

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b", },
	};
	int ids[] = { ID_OK };
	int pos[][2] = {
		{ m_w - 92, m_h - 24 },
	};

	for (int index = 0; index < 1; index++) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[index][0], pos[index][1]);
		button->SetId(ids[index]);
		AddChild(button);
	}
	m_defPushId = ID_OK;
	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
		char* buttonName[][3] = {
			{ "btn_cancel", "btn_cancel_a", "btn_cancel_b", },
		};
		int ids[] = { ID_CANCEL };
		int pos[][2] = {
			{ m_w - 46, m_h - 24 },
		};

		for (int index = 0; index < 1; index++) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ids[index]);
			AddChild(button);
		}
	}
}

void UIItemIdentifyWnd::OnDraw()
{
	int drawCount;
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	int itemCount;
	std::list<ITEM_INFO>::iterator it;

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + (((m_w - 24) % 12) ? 1 : 0);
	int i;
	for (i = 0; i < titleMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	TextOutA(17, 3, m_title.c_str(), 0, 0, 12, 0xffffff);
	TextOutA(16, 2, m_title.c_str(), 0, 0, 12, 0);

	g_skinMgr.GetColorChipColor(14, 6, rr, gg, bb);
	DrawBox(0, 17, m_w, m_h - 17, RGB(rr, gg, bb));
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	DrawBox(1, 17, m_w - 2, m_h - 17, RGB(rr, gg, bb));

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (i = 0; i < btnMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	drawCount = 0;
	itemCount = 0;
	it = m_itemInfoList.begin();
	while (it != m_itemInfoList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			it++;
			itemCount++;
		} else {
			int y = (drawCount / m_itemHorzNum) * 32 + 34;
			if (m_curItem == itemCount) {
				unsigned long rr;
				unsigned long gg;
				unsigned long bb;
				g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
				DrawBox(5, y - 3, m_w - 24, 30, RGB(rr, gg, bb));
			}

			DrawItem(8, y, *it, false);
			DrawItemText(40, y, *it, 0, 0, 12);

			if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
				break;
			++it;
			drawCount++;
			itemCount++;
		}
	}
}

void UIItemIdentifyWnd::OnLBtnDblClk(int x, int y) {
	OnLBtnDown(x, y);
	SendMsg(0, UM_COMMAND, 0x76, 0, 0, 0);
}

void UIItemIdentifyWnd::OnLBtnDown(int x, int y)
{
	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
		return;
	}
	int prev = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (prev != m_curItem) {
		Invalidate();
	}
}

void UIItemIdentifyWnd::StoreInfo() {
	if (m_itemIdentifyWndInfo) {
		m_itemIdentifyWndInfo->w = m_w;
		m_itemIdentifyWndInfo->h = m_h;
		m_itemIdentifyWndInfo->x = m_x;
		m_itemIdentifyWndInfo->y = m_y;
	}
}

void UIItemIdentifyWnd::OnRBtnUp(int x, int y) {}

int UIItemIdentifyWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_itemIdentifyWndInfo = (ItemIdentifyWndInfo*)val1;
		if (!m_itemIdentifyWndInfo->IsValidInfo())
			m_itemIdentifyWndInfo->MakeDefault();
		Move(m_itemIdentifyWndInfo->x, m_itemIdentifyWndInfo->y);
		break;
	case UM_CLEAR_ITEM: {
		m_itemInfoList.clear();
		break;
	}
	case UM_REFRESH:
		RecalcScrbarPos();
		Invalidate();
		break;
	case UM_ADDITEM: {
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(val2);
		m_itemInfoList.push_back(itemInfo);
		SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		break;
	}

	case UM_COMMAND:
		switch (val1) {
		case ID_CANCEL:
			g_modeMgr.GetCurMode()->SendMsg(MM_REQ_ITEMIDENTIFY, -1, 0, 0);
			g_windowMgr.DeleteWindow(WID_ITEMIDENTIFYWND);
			break;
		case ID_OK: {
			if (m_curItem != -1) {
				int isValid;
				ITEM_INFO info = GetItemInfo(m_curItem, isValid);
				if (isValid)
					g_modeMgr.GetCurMode()->SendMsg(MM_REQ_ITEMIDENTIFY, info.m_itemIndex, 0, 0);
				g_windowMgr.DeleteWindow(WID_ITEMIDENTIFYWND);
			}
			break;
		}
		}
		break;

	case UM_SCROLLVERT:
	case UM_SCROLLVERTPAGEUP:
	case UM_SCROLLVERTPAGEDOWN: {
		int oldViewOffset = m_viewOffset;
		if (message == UM_SCROLLVERTPAGEUP)
			m_viewOffset = oldViewOffset - m_itemVertNum + 1;
		else if (message == UM_SCROLLVERTPAGEDOWN)
			m_viewOffset = m_itemVertNum + oldViewOffset - 1;
		else
			m_viewOffset = oldViewOffset + val1;

		if (oldViewOffset != m_viewOffset) {
			RecalcScrbarPos();
			Invalidate();
		}
		return m_viewOffset - oldViewOffset;
	}
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

void UIItemIdentifyWnd::OnWheel(int wheel) {
	if (m_vertScrEnabled) {
		int n = m_itemVertNum - 1;
		if (n >= 3) n = 3;
		if (wheel < 0)
			SendMsg(m_scrollBar, UM_SCROLLVERT, n, 0, 0, 0);
		else
			SendMsg(m_scrollBar, UM_SCROLLVERT, -n, 0, 0, 0);
	}
}

void UIItemIdentifyWnd::RecalcScrbarPos()
{
	if (m_scrollBar) {
		int numCanDisplayItem = m_itemVertNum;

		int numLine = m_itemInfoList.size() / m_itemHorzNum + ((m_itemInfoList.size() % m_itemHorzNum) != 0);

		int range = numLine - numCanDisplayItem;
		if (range < 0)
			range = 0;
		if (range != 0)
			m_vertScrEnabled = 1;
		else
			m_vertScrEnabled = 0;

		if (m_vertScrEnabled) {
			m_scrollBar->SetRange(range, numCanDisplayItem, numLine);
			if (m_viewOffset < 0)
				m_viewOffset = 0;
			if (m_viewOffset > numLine - m_itemVertNum)
				m_viewOffset = numLine - m_itemVertNum;

			m_scrollBar->SetPos(m_viewOffset);
		} else {
			m_viewOffset = 0;
			m_scrollBar->SetRange(0, numCanDisplayItem, numLine);
		}
		m_scrollBar->Invalidate();
	}
}


ITEM_INFO UIItemIdentifyWnd::GetItemInfo(int curItem, int &isValid)
{
	int count = 0;
	isValid = 1;

	std::list<ITEM_INFO>::iterator it = m_itemInfoList.begin();
	while (it != m_itemInfoList.end()) {
		if (curItem == count)
			return *it;

		++it;
		count++;
	}

	isValid = 0;
	ITEM_INFO itemInfo;
	return itemInfo;
}

int UIItemIdentifyWnd::GetCurItem(int x, int y)
{

	if (x > m_w - 14 || x < 4 || y < 34 || y >= m_itemVertNum * 32 + 34)
		return -1;

	int yCell = (y - 34) / 32;



	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemInfoList.begin();
	while (it != m_itemInfoList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}
		if (drawCount == m_itemHorzNum * yCell)
			return itemCount;

		++it;
		++drawCount;
		++itemCount;
	}

	return -1;
}

UIItemCompositionWnd::UIItemCompositionWnd() {
	m_itemCompositionWndInfo = NULL;
}
UIItemCompositionWnd::~UIItemCompositionWnd() {}

void UIItemCompositionWnd::OnCreate(int cx, int cy)
{
	UIItemIdentifyWnd::OnCreate(cx, cy);
	m_title = MsgStr(MSI_ITEM_COMPOUNDING);
	m_cardItemIndex = 0;
}

void UIItemCompositionWnd::OnDraw() {
	UIItemIdentifyWnd::OnDraw();
}

void UIItemCompositionWnd::StoreInfo() {
	if (m_itemCompositionWndInfo) {
		m_itemCompositionWndInfo->w = m_w;
		m_itemCompositionWndInfo->h = m_h;
		m_itemCompositionWndInfo->x = m_x;
		m_itemCompositionWndInfo->y = m_y;
	}
}

int UIItemCompositionWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {

	case UM_SETINFO:
		m_itemCompositionWndInfo = (ItemCompositionWndInfo*)val1;
		if (!m_itemCompositionWndInfo->IsValidInfo())
			m_itemCompositionWndInfo->MakeDefault();

		Move(m_itemCompositionWndInfo->x, m_itemCompositionWndInfo->y);
		break;

	case UM_SET_CARD_ITEM_INDEX:
		m_cardItemIndex = val1;
		if (val1) {
			ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(val1);
			if (itemInfo.m_itemIndex) {
				char title[128];
				sprintf(title, "%s (", MsgStr(MSI_ITEM_COMPOUNDING));
				itemInfo.GetItemName(title + strlen(title));
				sprintf(title + strlen(title), ")");
				m_title.assign(title);
			}
		}
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			if (m_curItem != -1) {
				int isValid;

				ITEM_INFO info = GetItemInfo(m_curItem, isValid);
				if (isValid)
					g_modeMgr.GetCurMode()->SendMsg(MM_REQ_ITEMCOMPOSITION, info.m_itemIndex, m_cardItemIndex, 0);
				else
					g_modeMgr.GetCurMode()->SendMsg(MM_REQ_ITEMCOMPOSITION, -1, -1, 0);

				g_windowMgr.DeleteWindow(WID_ITEMCOMPOSITIONWND);
			}
			break;

		case ID_CANCEL:
			g_modeMgr.GetCurMode()->SendMsg(MM_REQ_ITEMCOMPOSITION, -1, -1, 0);

			g_windowMgr.DeleteWindow(WID_ITEMCOMPOSITIONWND);
			break;
		}
		break;

	default:
		return UIItemIdentifyWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UIQuitWnd::UIQuitWnd() {}
UIQuitWnd::~UIQuitWnd() {}

void UIQuitWnd::OnCreate(int cx, int cy)
{
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[3][3] = {
		{ "esc_01a", "esc_01b", "esc_01c" },
		{ "esc_03a", "esc_03b", "esc_03c" },
		{ "esc_02a", "esc_02b", "esc_02c" },
	};
	int ids[3] = { ID_RESTART, ID_EXIT, ID_CANCEL };
	int pos[3][3] = {
		{ 29, 30 },
		{ 29, 61 },
		{ 29, 96 },
	};

	for (int i = 0; i < 3; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = ID_EXIT;
}

void UIQuitWnd::OnDraw() {
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\win_msgbox.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
}

int UIQuitWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	CGameMode* gm = g_modeMgr.GetGameMode();
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_EXIT:
			gm->SendMsg(MM_QUIT, 0, 0, 0);
			g_windowMgr.DeleteWindow((WINDOWID)0x30);
			break;
		case ID_RESTART:
			gm->SendMsg(MM_QUIT, 0, 0, 0);

		case ID_CANCEL:
			g_windowMgr.DeleteWindow((WINDOWID)0x30);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UIRestartWnd::UIRestartWnd() {}
UIRestartWnd::~UIRestartWnd() {}

void UIRestartWnd::OnCreate(int cx, int cy)
{
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[4][4] = {
		{ "esc_04a", "esc_04b", "esc_04c" },
		{ "esc_01a", "esc_01b", "esc_01c" },
		{ "esc_03a", "esc_03b", "esc_03c" },
		{ "esc_02a", "esc_02b", "esc_02c" },
	};
	int ids[4] = { ID_RESTART, ID_TO_CHARACTER_SELECT, ID_QUIT, ID_CANCEL };
	int pos[4][4] = {
		{ 29, 22 },
		{ 29, 45 },
		{ 29, 68 },
		{ 29, 96 },
	};

	for (int i = 0; i < 4; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = ID_TO_CHARACTER_SELECT;
}

void UIRestartWnd::OnDraw() {
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\win_msgbox.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
}

int UIRestartWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();

	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_TO_CHARACTER_SELECT:
			gameMode->SendMsg(MM_RESTART, QUIT_EXIT, 0, 0);
			g_windowMgr.DeleteWindow(WID_RESTARTWND);
			break;
		case ID_RESTART:
			gameMode->SendMsg(MM_RESTART, QUIT_RESTART, 0, 0);
			g_windowMgr.DeleteWindow(WID_RESTARTWND);
			break;
		case ID_QUIT:
			g_windowMgr.DeleteWindow(WID_RESTARTWND);
			gameMode->SendMsg(MM_QUIT_GAME, 0, 0, 0);
			break;
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_RESTARTWND);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UINoticeConfirmWnd::UINoticeConfirmWnd() : m_target(MM_GOTOIDENTRY) {}
UINoticeConfirmWnd::~UINoticeConfirmWnd() {}

void UINoticeConfirmWnd::OnCreate(int cx, int cy)
{
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ 189, 96 },
		{ 234, 96 },
	};

	for (int i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = ID_OK;

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 22, cx - 20, cy - 55, false);

	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);

	const char* msg = MsgStr(MSI_DO_YOU_AGREE);
	m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg, 38, 0, 0);
}

int UINoticeConfirmWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_SET_TARGET:
		m_target = val1;
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_CANCEL:
			g_modeMgr.GetCurMode()->SendMsg(MM_QUIT, 0, 0, 0);
			break;
		case ID_OK:
			g_modeMgr.GetCurMode()->SendMsg(m_target, 0, 0, 0);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

void UINoticeConfirmWnd::OnDraw() {
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\win_msgbox.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
}
UIChooseSellBuyWnd::UIChooseSellBuyWnd() { m_naid = 0; }
UIChooseSellBuyWnd::~UIChooseSellBuyWnd() {}

void UIChooseSellBuyWnd::OnCreate(int cx, int cy)
{
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[3][3] = {
		{ "btn_buy", "btn_buy_a", "btn_buy_b" },
		{ "btn_sell", "btn_sell_a", "btn_sell_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[3] = { ID_BUY, ID_SELL, ID_CANCEL };
	int pos[3][2] = {
		{ 144, 96 },
		{ 189, 96 },
		{ 234, 96 },
	};

	for (int i = 0; i < 3; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = ID_BUY;

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 22, cx - 20, cy - 55, false);

	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);

	const char* msg = MsgStr(MSI_SELECT_DEAL_TYPE);
	m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg, 38, 0, 0);
}

void UIChooseSellBuyWnd::OnDraw() {
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\win_msgbox.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
}

int UIChooseSellBuyWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_CANCEL: {
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			break;
		}
		case ID_SELL: {
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_ACK_SELL, m_naid, 0, 0);
			break;
		}
		case ID_BUY:
			g_modeMgr.GetCurMode()->SendMsg(MM_ACK_BUY, m_naid, 0, 0);
			break;
		default:
			return 0;
		}
		g_windowMgr.DeleteWindow((WINDOWID)0x19);
		g_session.ClearDealItems();
		return 0;
	case UM_SETNAID:
		m_naid = val1;
		return 0;
	}
	return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
}
UIExchangeAcceptWnd::UIExchangeAcceptWnd() { m_info = NULL; }
UIExchangeAcceptWnd::~UIExchangeAcceptWnd() {}

void UIExchangeAcceptWnd::OnCreate(int cx, int cy)
{
	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_ok", "btn_ok_a", "btn_ok_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ 189, 96 },
		{ 234, 96 },
	};

	for (int i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = ID_OK;

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 22, cx - 20, cy - 55, false);

	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);

	m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)"deal", 38, 0, 0);
}

int UIExchangeAcceptWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (ExchangeAcceptWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		break;

	case UM_SET_EXCHANGE_CHAR_NAME: {
		std::string msg("(");
		msg += (const char*)val1;
		msg += ")";
		msg += MsgStr(MSI_SUGGEST_DEAL);

		m_textViewer->ClearAllItems();
		m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), 0x8a, 0, 0);

		if (val2 > 0 && val3 > 0 && val3 < 100) {
			int total = val2;
			int digit = 100000000;
			std::string msg("PN : ");
			int zero = 0;
			while (digit > 0) {
				int num = 0;
				while (total >= digit) {
					total -= digit;
					++num;
				}
				digit /= 10;

				if (num > 0)
					zero = 1;

				if (num == 0) {
					if (zero > 0)
						msg += "R";
				} else if (num == 1) {
					msg += "O";
				} else if (num == 2) {
					msg += "H";
				} else if (num == 3) {
					msg += "U";
				} else if (num == 4) {
					msg += "T";
				} else if (num == 5) {
					msg += "N";
				} else if (num == 6) {
					msg += "A";
				} else if (num == 7) {
					msg += "S";
				} else if (num == 8) {
					msg += "E";
				} else if (num == 9) {
					msg += "W";
				}
			}

			if (g_serviceType == ServiceIndonesia)
				msg = "";

			if (g_serviceType == ServiceBrazil)
				msg += "     Nv.";
			else
				msg += "     Lv.";

			char msg2[10];
			memset(msg2, 0, sizeof(msg2));
			sprintf(msg2, "%d", val3);
			RAG_BUDGET_SITES(1)
			msg += msg2;
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), 0x8a, 0, 0);
		}
		break;
	}

	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_EXCHANGEACCEPTWND);
			CGameMode* gameMode = (CGameMode*)g_modeMgr.GetCurMode();
			gameMode->SendMsg(MM_ACK_EXCHANGE_ITEM, 3, 0, 0);
			break;
		}
		case ID_CANCEL: {
			g_windowMgr.DeleteWindow(WID_EXCHANGEACCEPTWND);
			CGameMode* gameMode = (CGameMode*)g_modeMgr.GetCurMode();
			gameMode->SendMsg(MM_ACK_EXCHANGE_ITEM, 4, 0, 0);
			break;
		}
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIExchangeAcceptWnd::StoreInfo() {
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}
UITipOfTheDayWnd::UITipOfTheDayWnd() {}
UITipOfTheDayWnd::~UITipOfTheDayWnd() {}

void UITipOfTheDayWnd::OnCreate(int cx, int cy)
{
	int pos[3][2];
	int ids[3];
	char* buttonName[3][3];
	const char* tip[3];

	std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

	buttonName[0][0] = "btn_back";
	buttonName[0][1] = "btn_back_a";
	buttonName[0][2] = "btn_back_b";
	buttonName[1][0] = "btn_next";
	buttonName[1][1] = "btn_next_a";
	buttonName[1][2] = "btn_next_b";
	buttonName[2][0] = "btn_close";
	buttonName[2][1] = "btn_close_a";
	buttonName[2][2] = "btn_close_b";

	ids[0] = 0xb4;
	ids[1] = 0x98;
	ids[2] = ID_CLOSE;

	pos[0][0] = 144;
	pos[0][1] = m_h - 24;
	pos[1][0] = 189;
	pos[1][1] = m_h - 24;
	pos[2][0] = 234;
	pos[2][1] = m_h - 24;

	tip[0] = "";
	tip[1] = "";
	tip[2] = "/tip";

	int i;
	for (i = 0; i < 3; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		button->SetToolTip(tip[i]);
		AddChild(button);
	}

	m_defPushId = 0xb4;

	m_textViewer = new UITextViewer;
	m_textViewer->Create2(10, 22, cx - 20, cy - 55, false);

	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	m_textViewer->m_bgR = rr;
	m_textViewer->m_bgG = gg;
	m_textViewer->m_bgB = bb;
	AddChild(m_textViewer);

	m_showTipsAtStartup = g_showTipsAtStartup;

	UICheckBox* checkBox = new UICheckBox;
	checkBox->Create(GetTextWidth(MsgStr(MSI_SHOWTIPSATSTARTUP), 0, 0, 12, false) + 12, 12);
	checkBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		MsgStr(MSI_SHOWTIPSATSTARTUP));
	checkBox->Move(10, cy - 20);
	checkBox->SetCheck(m_showTipsAtStartup);
	AddChild(checkBox);

	if (g_tipOfTheDay.Size()) {
		int randValue = rand();
		m_indexOfTip = randValue % g_tipOfTheDay.Size();
		StringList& stringList = g_tipOfTheDay.GetTip(m_indexOfTip);

		m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)MsgStr(MSI_DIDYOUKNOW), 38, 0, 0);
		for (i = 0; i < stringList.size(); ++i)
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)stringList[i].c_str(), 38, 0, 0);
	}
}

void UITipOfTheDayWnd::OnDraw()
{



	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);


	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int loopCnt = (m_w - 24) / 12 + (((m_w - 24) % 12) ? 1 : 0);
	for (int i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}
	if (m_h == 17)
		return;


	loopCnt = (m_h - 38) / 8 + (((m_h - 38) % 8) ? 1 : 0);
	for (i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
		DrawBitmap(0, i * 8 + 17, bmp, false);
	}

	loopCnt = (m_h - 38) / 8 + (((m_h - 38) % 8) ? 1 : 0);
	for (i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, i * 8 + 17, bmp, false);
	}

	loopCnt = (m_w - 60) % 32;


	{
		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17, loopCnt, m_h - 38, RGB(rr, gg, bb));



		DrawBox(20, ((m_h - 38) / 32) * 32 + 17, m_w - 24, (m_h - 38) % 32, RGB(rr, gg, bb));
	}















	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);


	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	loopCnt = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	{
		int itemCols = (m_w - 40) / 32;
		int itemRows = (m_h - 38) / 32;
		for (int hLoop = 0; hLoop < itemRows; ++hLoop)
		{
			DrawItemFrame(40, hLoop * 32 + 17);
		}

		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(72, 17, itemCols * 32 - 32, itemRows * 32, RGB(rr, gg, bb));
	}

	unsigned long rr, gg, bb;
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	TextOutA(9, 4, MsgStr(MSI_TIPOFTHEDAY), 0, 0, 12, RGB(rr, gg, bb));
	TextOutA(8, 3, MsgStr(MSI_TIPOFTHEDAY), 0, 0, 12, 0);
}

void UITipOfTheDayWnd::WriteToReg() {
	HKEY hKey;
	DWORD dwDisposition = 0;
	LRESULT lResult = RegCreateKeyExA(HKEY_LOCAL_MACHINE, g_regPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (lResult == ERROR_SUCCESS) {
		RegSetValueExA(hKey, "SHOWTIPSATSTARTUP", 0, REG_DWORD, (BYTE*)&m_showTipsAtStartup, 4);
		RegCloseKey(hKey);
	}
}

int UITipOfTheDayWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	int i;

	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_CHECK:
			m_showTipsAtStartup = val2;
			g_showTipsAtStartup = val2;
			WriteToReg();
			break;
		case ID_PREV:
			if (g_tipOfTheDay.Size()) {
				--m_indexOfTip;
				if (m_indexOfTip < 0)
					m_indexOfTip = g_tipOfTheDay.Size() - 1;
				StringList& stringList = g_tipOfTheDay.GetTip(m_indexOfTip);
				SendMsg(NULL, UM_SETTEXT, (int)&stringList, 0, 0, 0);
			}
			break;
		case ID_NEXT:
			if (g_tipOfTheDay.Size()) {
				++m_indexOfTip;
				if (m_indexOfTip >= g_tipOfTheDay.Size())
					m_indexOfTip = 0;
				StringList& stringList = g_tipOfTheDay.GetTip(m_indexOfTip);
				SendMsg(NULL, UM_SETTEXT, (int)&stringList, 0, 0, 0);
			}
			break;
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_TIPOFTHEDAYWND);
			break;
		}

		break;
	case UM_SETTEXT: {
		m_textViewer->ClearAllItems();
		m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)MsgStr(MSI_DIDYOUKNOW), UM_DROP, 0, 0);

		StringList* stringList = (StringList*)val1;
		for (i = 0; i < stringList->size(); ++i)
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)(*stringList)[i].c_str(), UM_DROP, 0, 0);

		break;
	}
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UIJoinPartyAcceptWnd::UIJoinPartyAcceptWnd() { m_info = NULL; }
UIJoinPartyAcceptWnd::~UIJoinPartyAcceptWnd() {}

int UIJoinPartyAcceptWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_JOINPARTYACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_GROUP, m_naid, 1, 0);
			break;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_JOINPARTYACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_GROUP, m_naid, 0, 0);
			break;
		}
		break;
	case UM_SET_JOIN_PARTY_MSG: {
		m_naid = val1;
		std::string msg("(");
		msg += (const char*)val2;
		msg += ")";
		msg += MsgStr(MSI_SUGGEST_JOIN_PARTY);

		m_textViewer->ClearAllItems();
		m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), UM_DROP, 0, 0);
		break;
	}
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UICoupleAcceptWnd::UICoupleAcceptWnd() { m_info = NULL; }
UICoupleAcceptWnd::~UICoupleAcceptWnd() {}

int UICoupleAcceptWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_COUPLE_MSG:
		{
			m_naid = val1;
			std::string msg("(");
			msg += (const char*)val2;
			msg += ")";
			msg += MsgStr(MSI_SUGGEST_COUPLE);

			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), UM_DROP, 0, 0);
		}
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_COUPLEACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_COUPLE, m_naid, 1, 0);
			break;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_COUPLEACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_COUPLE, m_naid, 0, 0);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIStarPlaceAcceptWnd::UIStarPlaceAcceptWnd() { m_info = NULL; }
UIStarPlaceAcceptWnd::~UIStarPlaceAcceptWnd() {}

int UIStarPlaceAcceptWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_BABY_MSG:
		{
			which = val1;
			std::string msg;
			if (which == 10)
				msg = MsgStr(MSI_DA_EXP_ACCEPT);
			else
				msg = MsgStr(MSI_STARPLACE_ACCEPT);

			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), UM_DROP, 0, 0);
		}
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			{
				CGameMode* gameMode = g_modeMgr.GetGameMode();
				g_windowMgr.DeleteWindow(WID_STARPLACE_ACCEPTWND);
				gameMode->SendMsg(MM_STARPLACE_ACCEPT, which, 0, 0);
			}
			break;
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_STARPLACE_ACCEPTWND);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIYourItemWnd::UIYourItemWnd() { m_info = NULL; }
UIYourItemWnd::~UIYourItemWnd() {}

int UIYourItemWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_BABY_MSG:
		{
			std::string msg;
			msg = MsgStr(MSI_YOURITEM_EQUIP);
			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), UM_DROP, 0, 0);
			val_index = val1;
			val_wear = val2;
		}
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			g_modeMgr.GetGameMode();
			{
				PACKET_CZ_REQ_WEAR_EQUIP packet;
				packet.PacketType = HEADER_CZ_REQ_WEAR_EQUIP;
				packet.index = val_index;
				packet.wearLocation = val_wear;
				CRagConnection::instanceR().SendPacket(
					CRagConnection::instanceR().GetPacketSize(HEADER_CZ_REQ_WEAR_EQUIP),
					(const char *)&packet);
			}
			g_windowMgr.DeleteWindow(WID_YOURITEMWND);
			break;
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_YOURITEMWND);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UIBabyAcceptWnd::UIBabyAcceptWnd() { m_info = NULL; }
UIBabyAcceptWnd::~UIBabyAcceptWnd() {}

int UIBabyAcceptWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_BABY_MSG:
		{
			m_naid = val1;
			std::string msg("(");
			msg += (const char*)val2;
			msg += ")";
			msg += MsgStr(MSI_SUGGEST_BABY);

			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), UM_DROP, 0, 0);
		}
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			{
				g_windowMgr.DeleteWindow(WID_BABYACCEPTWND);
				UIWindow* wnd = g_windowMgr.MakeWindow(WID_BABYACCEPTWND2);
				wnd->SendMsg(NULL, UM_SET_BABY_MSG, ID_OK, val2, 0, 0);
			}
			break;
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_BABYACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_BABY, m_naid, 0, 0);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIBabyAcceptWnd2::UIBabyAcceptWnd2() { m_info = NULL; }
UIBabyAcceptWnd2::~UIBabyAcceptWnd2() {}

int UIBabyAcceptWnd2::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_BABY_MSG:
		{
			m_naid = val1;
			std::string msg = MsgStr(MSI_SUGGEST_BABY2);
			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), UM_DROP, 0, 0);
		}
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_BABYACCEPTWND2);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_BABY, m_naid, 1, 0);
			break;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_BABYACCEPTWND2);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_BABY, m_naid, 0, 0);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIJoinGuildAcceptWnd::UIJoinGuildAcceptWnd() { m_info = NULL; }
UIJoinGuildAcceptWnd::~UIJoinGuildAcceptWnd() {}

int UIJoinGuildAcceptWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_JOIN_GUILD_MSG:
		{
			m_naid = val1;
			std::string msg("(");
			msg += (const char*)val2;
			msg += ")";
			msg += MsgStr(MSI_SUGGEST_JOIN_GUILD);

			m_textViewer->ClearAllItems();
			m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg.c_str(), UM_DROP, 0, 0);
		}
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_JOINGUILDACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_GUILD, m_naid, 1, 0);
			break;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_JOINGUILDACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_GUILD, m_naid, 0, 0);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIChatRoomMakeWnd::UIChatRoomMakeWnd()
{
	m_title = MsgStr(MSI_MAKING_ROOM);
}
UIChatRoomMakeWnd::~UIChatRoomMakeWnd() {}

void UIChatRoomMakeWnd::OnCreate(int cx, int cy)
{
	if (g_session.IsEFST_TrickDead()) {
		RAG_BUDGET_FILL(9)
		ErrorMsg(MsgStr(MSI_CANT_MAKE_CHAT_ROOM));
		g_windowMgr.DeleteWindow(WID_CHATROOMMAKEWND);
	}

	m_numPeople = 20;
	m_classMode = 0;

	m_classStringList.push_back(std::string(MsgStr(MSI_CHAT_ROOM)));
	m_classModeList.push_back(0);

	m_numStringList.push_back("2 " + std::string(MsgStr(MSI_COUNT_UNIT_OF_PEOPLE)));
	m_numList.push_back(2);
	m_numStringList.push_back("3 " + std::string(MsgStr(MSI_COUNT_UNIT_OF_PEOPLE)));
	m_numList.push_back(3);
	m_numStringList.push_back("5 " + std::string(MsgStr(MSI_COUNT_UNIT_OF_PEOPLE)));
	m_numList.push_back(5);
	m_numStringList.push_back("10 " + std::string(MsgStr(MSI_COUNT_UNIT_OF_PEOPLE)));
	m_numList.push_back(10);
	m_numStringList.push_back("15 " + std::string(MsgStr(MSI_COUNT_UNIT_OF_PEOPLE)));
	m_numList.push_back(15);
	m_numStringList.push_back("20 " + std::string(MsgStr(MSI_COUNT_UNIT_OF_PEOPLE)));
	m_numList.push_back(20);
	{
		int posX[3] = { m_w - 14 };
		int ids2[3] = { ID_CLOSE };

		std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
		for (int i = 0; i < 1; ++i) {
			m_sysButton = new UIBitmapButton;
			m_sysButton->SetBitmapName((preBmpName2 + "close" + "_off.bmp").c_str(), BS_NORMAL);
			m_sysButton->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_MOUSEON);
			m_sysButton->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_PRESSED);
			m_sysButton->Create(m_sysButton->GetBitmapWidth(), m_sysButton->GetBitmapHeight());
			m_sysButton->Move(posX[i], 3);
			m_sysButton->SetId(ids2[i]);
			m_sysButton->SetToolTip("Alt+C");
			AddChild(m_sysButton);
		}
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
		char* buttonName[2][3] = {
			{ "btn_ok", "btn_ok_a", "btn_ok_b" },
			{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
		};
		int ids[2] = { ID_OK, ID_CANCEL };
		int pos[2][2] = {
			{ 189, 96 },
			{ 234, 96 },
		};

		for (int i = 0; i < 2; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			AddChild(button);
		}

		m_defPushId = ID_OK;
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\";
		char* buttonName[2][3] = {
			{ "txtbox_btn_a", "txtbox_btn_b", "txtbox_btn_c" },
			{ "txtbox_btn_a", "txtbox_btn_b", "txtbox_btn_c" },
		};
		int ids[2] = { ID_NUMCOMBO, ID_CLASSCOMBO };
		int pos[2][2] = {
			{ 119, 46 },
			{ 256, 46 },
		};

		for (int i = 0; i < 2; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			AddChild(button);
		}
	}

	m_roomTitle = new UIEditCtrl;
	m_roomTitle->Create(228, 16);
	m_roomTitle->Move(45, 24);
	m_roomTitle->SetTextLimit(36);
	m_roomTitle->SetFrameColor(0xf2, 0xf2, 0xf2);
	m_roomTitle->SetText("");
	AddChild(m_roomTitle);
	g_windowMgr.SetFocusEdit(m_roomTitle);

	m_passWord = new UIEditCtrl;
	m_passWord->Create(90, 16);
	m_passWord->Move(182, 70);
	m_passWord->SetTextLimit(8);
	m_passWord->SetMaskChar('*');
	m_passWord->SetFrameColor(0xf2, 0xf2, 0xf2);
	m_passWord->SetText("");
	AddChild(m_passWord);

	m_curRadioBtn = 0;
	{
		const char* texts[2] = { MsgStr(MSI_ROOM_PUBLIC2), MsgStr(MSI_ROOM_PRIVATE2) };
		int states[2] = { 1, 0 };
		for (int i = 0; i < 2; ++i) {
			m_radioBtns[i] = new UIRadioBtn;
			m_radioBtns[i]->Create(40 + i * 12, 12);
			m_radioBtns[i]->Move(44 + i * 40, 70);
			m_radioBtns[i]->SetText(texts[i]);
			m_radioBtns[i]->SendMsg(this, UM_SETSTATE, states[i], 0, 0, 0);
			AddChild(m_radioBtns[i]);
		}
	}

	m_numText = new UIStaticText;
	m_numText->Create(68, 14);
	m_numText->SetText(m_numStringList[m_numStringList.size() - 1].c_str(), 0);
	m_numText->Move(48, 49);
	m_numText->m_drawBackGround = 1;
	m_numText->m_backR = 0xf2;
	m_numText->m_backG = 0xf2;
	m_numText->m_backB = 0xf2;
	AddChild(m_numText);

	m_classText = new UIStaticText;
	m_classText->Create(68, 14);
	m_classText->SetText(m_classStringList[0].c_str(), 0);
	m_classText->Move(187, 49);
	m_classText->m_drawBackGround = 1;
	m_classText->m_backR = 0xf2;
	m_classText->m_backG = 0xf2;
	m_classText->m_backB = 0xf2;
	AddChild(m_classText);
}

void UIChatRoomMakeWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_fix.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\chatwin0_bg.bmp"), false);
	DrawBitmap(0, 17, bmp, false);

	TextOutA(9, 4, m_title.c_str(), 0, 0, 12, 0xffffff);
	TextOutA(8, 3, m_title.c_str(), 0, 0, 12, 0);
}

int UIChatRoomMakeWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			if (!m_roomTitle->GetTextSize()) {
				g_windowMgr.ErrorMsg(MsgStr(MSI_ENTER_ROOM_TITLE), ME_OK, 1, 0, 0);
				break;
			}
			if (g_insultFilter.IsBadSentence(m_roomTitle->GetText())) {
				g_windowMgr.ErrorMsg(MsgStr(MSI_BAD_SENTANGE), ME_OK, 1, 0, 0);
				break;
			}
			if (g_serviceType == ServiceAmerica) {
				if (!g_session.IsOnlyEnglish(m_roomTitle->GetText())) {
					g_windowMgr.ErrorMsg(MsgStr(MSI_ENGLISHONLY), ME_OK, 1, 0, 0);
					break;
				}
			}
			if (m_passWord->GetTextSize() < 4 && m_curRadioBtn == 1) {
				if (!m_passWord->GetTextSize())
					g_windowMgr.ErrorMsg(MsgStr(MSI_ENTER_PASSWORD), ME_OK, 1, 0, 0);
				else
					g_windowMgr.ErrorMsg(MsgStr(MSI_Enter_Password_more_than_4_char), ME_OK, 1, 0, 0);
				break;
			}

			ChatRoomInfo info;
			info.maxNumPeople = m_numPeople;
			info.numPeople = 0;
			info.roomId = -1;
			info.roomType = m_curRadioBtn != 1;
			info.title = m_roomTitle->GetText();
			info.pass = m_passWord->GetText();
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_CREATE_CHATROOM, (int)&info, 0, 0);
			g_windowMgr.DeleteWindow(WID_CHATROOMMAKEWND);
		}
			break;
		case ID_CANCEL:
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_CHATROOMMAKEWND);
			break;
		case ID_RADIOBTN: {
			for (int i = 0; i < 2; ++i) {
				m_radioBtns[i]->Invalidate();
				if (sender == m_radioBtns[i]) {
					m_curRadioBtn = i;
					m_radioBtns[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
				} else {
					m_radioBtns[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
				}
			}
			break;
		}
		case ID_NUMCOMBO:
		case ID_CLASSCOMBO: {
			if (g_windowMgr.GetComboBoxWnd())
				g_windowMgr.DeleteWindow(WID_COMBOBOXWND);

			UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
			int comboCnt = 1;
			switch (val1) {
			case ID_NUMCOMBO: {
				m_comboMode = 0;
				comboCnt = m_numStringList.size();
				for (int i = 0; i < m_numStringList.size(); ++i)
					comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)m_numStringList[i].c_str(), 0, 0, 0);
				break;
			}
			case ID_CLASSCOMBO: {
				m_comboMode = 1;
				comboCnt = m_classStringList.size();
				for (int i = 0; i < m_classStringList.size(); ++i)
					comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)m_classStringList[i].c_str(), 0, 0, 0);
				break;
			}
			}

			int height = comboCnt * 16 < 48 ? comboCnt * 16 : 48;
			comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			comboBoxWnd->Resize(74, height);
			comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_CHATROOMMAKEWND, 0, 0, 0);

			int gx = m_comboMode + (m_comboMode + m_comboMode * 16) * 8 + 44;
			int gy = 64;
			GetGlobalCoor(gx, gy);
			comboBoxWnd->Move(gx + 1, gy);
			break;
		}
		}
		break;
	case UM_COMBOBOX_SELECTED:
		switch (m_comboMode) {
		case 0:
			m_numText->SetText(m_numStringList[val1].c_str(), 0);
			m_numPeople = m_numList[val1];
			break;
		case 1:
			m_classText->SetText(m_classStringList[val1].c_str(), 0);
			m_classMode = m_classModeList[val1];
			break;
		}
		Trace("UM_COMBOBOX_SELECTED %d", val1);
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIChatRoomChangeWnd::UIChatRoomChangeWnd()
{
	m_title = MsgStr(MSI_CHANGE_ROOM_SETTING);
}
UIChatRoomChangeWnd::~UIChatRoomChangeWnd() {}

int UIChatRoomChangeWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO: {
		ChatRoomInfo* roomInfo = (ChatRoomInfo*)val1;
		m_numPeople = roomInfo->maxNumPeople;
		m_numText->SetText(m_numStringList[m_numStringList.size() - 1].c_str(), 0);
		m_numText->Invalidate();

		m_passWord->SetText(roomInfo->pass.c_str());
		Trace("%s", roomInfo->title.c_str());
		m_roomTitle->SetText(roomInfo->title.c_str());
		g_windowMgr.SetFocusEdit(m_roomTitle);
		m_roomTitle->MakeSelect();

		m_curRadioBtn = roomInfo->roomType == 0;
		for (int i = 0; i < 2; ++i) {
			m_radioBtns[i]->Invalidate();
			if (i == m_curRadioBtn)
				m_radioBtns[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
			else
				m_radioBtns[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
		}

		m_sysButton->SetToolTip("");
		Invalidate();
		break;
	}
	case UM_COMMAND:
	switch (val1) {
	case ID_OK: {
			if (!m_roomTitle->GetTextSize()) {
				g_windowMgr.ErrorMsg(MsgStr(MSI_ENTER_ROOM_TITLE), ME_OK, 1, 0, 0);
				break;
			}
			if (g_insultFilter.IsBadSentence(m_roomTitle->GetText())) {
				g_windowMgr.ErrorMsg(MsgStr(MSI_BAD_SENTANGE), ME_OK, 1, 0, 0);
				break;
			}
			if (g_serviceType == ServiceAmerica) {
				if (!g_session.IsOnlyEnglish(m_roomTitle->GetText())) {
					g_windowMgr.ErrorMsg(MsgStr(MSI_ENGLISHONLY), ME_OK, 1, 0, 0);
					break;
				}
			}
			if (m_passWord->GetTextSize() < 4 && m_curRadioBtn == 1) {
				if (!m_passWord->GetTextSize())
					g_windowMgr.ErrorMsg(MsgStr(MSI_ENTER_PASSWORD), ME_OK, 1, 0, 0);
				else
					g_windowMgr.ErrorMsg(MsgStr(MSI_Enter_Password_more_than_4_char), ME_OK, 1, 0, 0);
				break;
			}

			ChatRoomInfo info;
			info.maxNumPeople = m_numPeople;
			info.numPeople = 0;
			info.roomId = -1;
			info.roomType = m_curRadioBtn != 1;
			info.title = m_roomTitle->GetText();
			info.pass = m_passWord->GetText();
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_CHANGE_CHATROOM, (int)&info, 0, 0);
			g_windowMgr.DeleteWindow(WID_CHATROOMCHANGEWND);
			break;
		}
	case ID_CANCEL:
	case ID_CLOSE:
			Trace("ID_CANCEL");
			g_windowMgr.DeleteWindow(WID_CHATROOMCHANGEWND);
			break;
	case ID_NUMCOMBO:
	case ID_CLASSCOMBO: {
			if (g_windowMgr.GetComboBoxWnd())
				g_windowMgr.DeleteWindow(WID_COMBOBOXWND);

			UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
			int comboCnt = 1;
			switch (val1) {
			case ID_NUMCOMBO: {
				m_comboMode = 0;
				comboCnt = m_numStringList.size();
				for (int i = 0; i < m_numStringList.size(); ++i)
					comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)m_numStringList[i].c_str(), 0, 0, 0);
				break;
			}
			case ID_CLASSCOMBO: {
				m_comboMode = 1;
				comboCnt = m_classStringList.size();
				for (int i = 0; i < m_classStringList.size(); ++i)
					comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)m_classStringList[i].c_str(), 0, 0, 0);
				break;
			}
			}

			int height = comboCnt * 16 < 48 ? comboCnt * 16 : 48;

			comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			comboBoxWnd->Resize(74, height);
			comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_CHATROOMCHANGEWND, 0, 0, 0);

			int gx = m_comboMode + (m_comboMode + m_comboMode * 16) * 8 + 44;
			int gy = 64;
			GetGlobalCoor(gx, gy);
			comboBoxWnd->Move(gx + 1, gy);
			break;
		}
	default:
		return UIChatRoomMakeWnd::SendMsg(sender, UM_COMMAND, val1, val2, val3, val4);
	}
	break;
	default:
		return UIChatRoomMakeWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIExchangeWnd::UIExchangeWnd()
{}
UIExchangeWnd::~UIExchangeWnd() {}

void UIExchangeWnd::OnCreate(int cx, int cy)
{
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);

	m_IsMyDragClosed = 0;
	m_IsOtherDragClosed = 0;
	m_okPushed = 0;
	m_exchangePushed = 0;
	m_cancelPushed = 0;

	int i;
	int ids[3] = { ID_OK, ID_EXCHANGE, ID_CANCEL };
	char* buttonName[3] = { "ok", "exchange", "cancel" };

	std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_";

	int posX[3] = { 3, 119, 235 };
	int offY[3];
	offY[0] = m_IsMyDragClosed * 120;
	offY[1] = (m_IsMyDragClosed && m_IsOtherDragClosed && !m_exchangePushed ? 0 : 1) * 120;
	offY[2] = 0;

	for (i = 0; i < 3; ++i) {
		m_buttons[i] = new UIBitmapButton;
		m_buttons[i]->SetBitmapName((preBmpName + buttonName[i] + ".bmp").c_str(), BS_NORMAL);
		m_buttons[i]->SetBitmapName((preBmpName + buttonName[i] + "_a.bmp").c_str(), BS_MOUSEON);
		m_buttons[i]->SetBitmapName((preBmpName + buttonName[i] + "_b.bmp").c_str(), BS_PRESSED);
		m_buttons[i]->Create(m_buttons[i]->GetBitmapWidth(), m_buttons[i]->GetBitmapHeight());
		m_buttons[i]->Move(posX[i], m_h - 23 + offY[i]);
		m_buttons[i]->SetId(ids[i]);
		AddChild(m_buttons[i]);
	}

	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(64, 16);
	m_editCtrl->Move(40, 74);
	m_editCtrl->SetTextLimit(9);
	m_editCtrl->SetFrameColor(0xe8, 0xe8, 0xe8);
	m_editCtrl->SetText("0");
	AddChild(m_editCtrl);
	g_windowMgr.SetFocusEdit(m_editCtrl);
	m_editCtrl->MakeSelect();

	m_defPushId = ID_NOTHING;
}

void UIExchangeWnd::OnDraw()
{
	char title[128];
	char name[128];

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_fix.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\exchange_bg.bmp"), false);
	DrawBitmap(0, 17, bmp, false);

	sprintf(title, "%s : ", MsgStr(MSI_EXCHANGE));
	TextOutA(13, 4, title, 0, 0, 12, 0xffffff);
	TextOutA(12, 3, title, 0, 0, 12, 0);

	if (m_exchangeCharacterID > 0) {
		char ID[12];
		memset(ID, 0, sizeof(ID));
		int pos = 0;
		int id = m_exchangeCharacterID;
		int div = 100000000;

		while (div > 0) {
			int digit = 0;
			while (id >= div) {
				id -= div;
				digit++;
			}
			div /= 10;

			if (pos > 0)
				pos++;
			else if (digit > 0)
				pos = 1;

			if (digit == 0) {
				if (pos > 0)
					ID[pos - 1] = 'R';
			} else if (digit == 1)
				ID[pos - 1] = 'O';
			else if (digit == 2)
				ID[pos - 1] = 'H';
			else if (digit == 3)
				ID[pos - 1] = 'U';
			else if (digit == 4)
				ID[pos - 1] = 'T';
			else if (digit == 5)
				ID[pos - 1] = 'N';
			else if (digit == 6)
				ID[pos - 1] = 'A';
			else if (digit == 7)
				ID[pos - 1] = 'S';
			else if (digit == 8)
				ID[pos - 1] = 'E';
			else if (digit == 9)
				ID[pos - 1] = 'W';
		}

		if (g_serviceType == ServiceIndonesia)
			sprintf(name, "%s  Lv%d", m_exchangeCharacterName.c_str(), m_exchangeCharacterLV);
		else
			sprintf(name, "%s  Lv%d (%s)", m_exchangeCharacterName.c_str(), m_exchangeCharacterLV, ID);

		TextOutA(63, 4, name, 0, 0, 12, 0xb4b4b4);
		TextOutA(62, 3, name, 0, 0, 12, 0);
	} else {
		sprintf(name, "%s", m_exchangeCharacterName.c_str());
		TextOutA(63, 4, name, 0, 0, 12, 0xb4b4b4);
		TextOutA(62, 3, name, 0, 0, 12, 0);
	}

	if (m_IsMyDragClosed) {
		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(10, 6, rr, gg, bb);
		DrawBox(4, 17, 134, 54, RGB(rr, gg, bb));
	}

	if (m_IsOtherDragClosed) {
		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(10, 6, rr, gg, bb);
		DrawBox(144, 17, 134, 54, RGB(rr, gg, bb));
	}

	char* buttonName[3] = { "ok", "exchange", NULL };
	std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_";
	int posX[3] = { 3, 119, 0 };

	for (int i = 0; i < 2; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp((preBmpName + buttonName[i] + "_dis.bmp").c_str()), false);
		DrawBitmap(posX[i], m_h - 23, bmp, false);
	}

	{
		int itemCount = 0;
		int drawCount = 0;
		std::list<ITEM_INFO>::iterator it = m_myItemList.begin();
		while (it != m_myItemList.end()) {
			if (itemCount < 0) {
				++it;
				++itemCount;
				continue;
			}
			DrawItemWithCount((drawCount % 5) * 27 + 4, (drawCount / 5) * 27 + 20, *it, false, false);
			if (++drawCount == 10)
				break;
			++it;
			++itemCount;
		}
	}

	{
		int itemCount = 0;
		int drawCount = 0;
		std::list<ITEM_INFO>::iterator it = m_otherItemList.begin();
		while (it != m_otherItemList.end()) {
			if (itemCount < 0) {
				++it;
				++itemCount;
				continue;
			}
			DrawItemWithCount((drawCount % 5) * 27 + 144, (drawCount / 5) * 27 + 20, *it, false, false);
			if (++drawCount == 10)
				break;
			++it;
			++itemCount;
		}
	}

	char moneyTxt[32];
	char priceText[128];
	int len = sprintf(priceText, "%s", GetNumberText(g_session.GetMyExchangeMoney(), moneyTxt));
	TextOutWithRightAlign(104, 77, priceText, len, 1, 14, 0);
	len = sprintf(priceText, "%s", GetNumberText(g_session.GetOtherExchangeMoney(), moneyTxt));
	TextOutWithRightAlign(243, 77, priceText, len, 1, 14, 0);
}

ITEM_INFO UIExchangeWnd::GetItemInfo(int x, int y)
{
	if (x < 4 || x >= m_w - 3 || y < 20 || y >= 74)
		return ITEM_INFO();

	if (x < 139) {
		int xCell = (x - 4) / 27;
		int yCell = (y - 20) / 27;
		int itemCount = 0;
		int drawCount = 0;
		std::list<ITEM_INFO>::iterator it = m_myItemList.begin();
		while (it != m_myItemList.end()) {
			if (itemCount < 0) {
				++it;
				++itemCount;
				continue;
			}
			if (drawCount == yCell * 5 + xCell)
				return *it;
			++it;
			++drawCount;
			++itemCount;
		}
	} else if (x >= 144) {
		int xCell = (x - 144) / 27;
		int yCell = (y - 20) / 27;
		int itemCount = 0;
		int drawCount = 0;
		std::list<ITEM_INFO>::iterator it = m_otherItemList.begin();
		while (it != m_otherItemList.end()) {
			if (itemCount < 0) {
				++it;
				++itemCount;
				continue;
			}
			if (drawCount == yCell * 5 + xCell)
				return *it;
			++it;
			++drawCount;
			++itemCount;
		}
	}
	return ITEM_INFO();
}

void UIExchangeWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		if (w)
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}

void UIExchangeWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx;
		int gy;
		if (x < 139) {
			int xCell = (x - 4) / 27;
			int yCell = (y - 20) / 27;
			gx = xCell * 27 + 4;
			gy = yCell * 27 + 4;
		} else {
			int xCell = (x - 144) / 27;
			int yCell = (y - 20) / 27;
			gx = xCell * 27 + 144;
			gy = yCell * 27 + 4;
		}
		GetGlobalCoor(gx, gy);

		char itemName[64];
		itemInfo.m_itemType = 0x13;
		itemInfo.GetItemName(itemName);
		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemName, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

int UIExchangeWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SET_EXCHANGE_CHAR_NAME:
		m_exchangeCharacterName.assign((const char*)val1);
		m_exchangeCharacterID = val2;
		m_exchangeCharacterLV = (short)val3;
		break;

	case UM_MYDRAGCLOSED:
		m_IsMyDragClosed = 1;
		RemoveMoneyCtrls();
		InvalidateButton();
		break;

	case UM_OTHERDRAGCLOSED:
		m_IsOtherDragClosed = 1;
		RemoveMoneyCtrls();
		InvalidateButton();
		break;

	case UM_COMMAND: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();

		switch (val1) {
		case ID_OK:
			if (!m_okPushed) {
				if (m_editCtrl) {
					int money = atoi(m_editCtrl->GetText());
					if (money < 0)
						money = 0;
					if (money > g_session.GetGold())
						money = g_session.GetGold();
					gameMode->SendMsg(MM_ADD_EXCHANGE_ITEM, 0, g_session.GetMyExchangeMoney() + money, 0);
					RemoveMoneyCtrls();
				}
				gameMode->SendMsg(MM_CONCLUDE_EXCHANGE_ITEM, 0, 0, 0);
				m_okPushed = 1;
			}
			break;

		case ID_EXCHANGE:
			if (!m_exchangePushed) {
				gameMode->SendMsg(MM_EXEC_EXCHANGE_ITEM, 0, 0, 0);
				m_exchangePushed = 1;
				InvalidateButton();
			}
			break;

		case ID_CANCEL:
			if (!m_cancelPushed) {
				gameMode->SendMsg(MM_CANCEL_EXCHANGE_ITEM, 0, 0, 0);
				m_cancelPushed = 1;
			}
			break;

		case ID_RIGHT:
			Trace("ID_RIGHT");
			if (m_editCtrl) {
				int money = atoi(m_editCtrl->GetText());
				if (money < 0)
					money = 0;
				if (money > g_session.GetGold())
					money = g_session.GetGold();
				gameMode->SendMsg(MM_ADD_EXCHANGE_ITEM, 0, g_session.GetMyExchangeMoney() + money, 0);
				RemoveMoneyCtrls();
			}
			break;
		}
		break;
	}

	case UM_REFRESHITEM: {
		m_myItemList.erase(m_myItemList.begin(), m_myItemList.end());
		m_otherItemList.erase(m_otherItemList.begin(), m_otherItemList.end());
		for (int i = 0; i < 10; ++i) {
			ITEM_INFO itemInfo = g_session.GetExchangeItemInfo(i);
			if (itemInfo.m_num)
				m_myItemList.push_back(itemInfo);

			ITEM_INFO otherItemInfo = g_session.GetOtherExchangeItemInfo(i);
			if (otherItemInfo.m_num)
				m_otherItemList.push_back(otherItemInfo);
		}
		Invalidate();
		break;
	}

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		gameMode->SendMsg(MM_ADD_EXCHANGE_ITEM, gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIExchangeWnd::RemoveMoneyCtrls()
{
	if (!m_IsMyDragClosed)
		return;
	if (m_editCtrl != NULL) {
		g_windowMgr.SetFocusEdit(NULL);
		m_editCtrl->Move(129, 500);
		m_editCtrl = NULL;
		UIWindow* chatWnd = g_windowMgr.GetChatWnd();
		if (chatWnd) {
			chatWnd->SendMsg(NULL, UM_GET_EDIT_FOCUS, 0, 0, 0, 0);
		}
	}
	Invalidate();
}

void UIExchangeWnd::InvalidateButton()
{
	int posX[3] = { 3, 119, 235 };
	int offY[3];

	offY[0] = m_IsMyDragClosed * 120;
	offY[1] = (m_IsMyDragClosed && m_IsOtherDragClosed && !m_exchangePushed ? 0 : 1) * 120;
	offY[2] = 0;

	for (int i = 0; i < 3; ++i)
		m_buttons[i]->Move(posX[i], m_h - 23 + offY[i]);

	Invalidate();
}

void UIExchangeWnd::DragAndDrop(int x, int y, const DRAG_INFO* const dragInfo) {
	CGameMode* gm = g_modeMgr.GetGameMode();
	if (m_IsMyDragClosed) {
		gm->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		return;
	}
	if (dragInfo->m_dragType == 1) {
		if (dragInfo->m_numDragItem == 1) {
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		}
		g_windowMgr.ShowWindow((WINDOWID)0xf, 1);
		UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
		wnd->SendMsg(0, UM_SETSTATE, 5, 0, 0, 0);
		return;
	}
	gm->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}
UIPartySettingWnd::UIPartySettingWnd() { m_nYOffset = 0; m_info = NULL; }
UIPartySettingWnd::~UIPartySettingWnd() {}

void UIPartySettingWnd::OnCreate(int cx, int cy)
{
	m_nYOffset = m_nYOffset;
	m_curExpDivBtn = g_session.m_expDivType;
	if (m_curExpDivBtn == 2)
		m_curExpDivBtn = 0;

	m_curItemCollectBtn = g_session.m_itemCollectType;
	m_curItemDivBtn = g_session.m_itemDivType;

	if (cy == 245)
		m_nYOffset = 70;
	else if (cy == 180)
		m_nYOffset = 5;

	int i;
	if (m_nYOffset == 70) {
		const char* texts[2];
		texts[0] = MsgStr(MSI_EXPDIV1); texts[1] = MsgStr(MSI_EXPDIV2);

		for (i = 0; i < 2; ++i) {
			m_radioExpDiv[i] = new UIRadioBtn;
			if (g_serviceType == ServiceBrazil)
				m_radioExpDiv[i]->Create(160, 17);
			else
				m_radioExpDiv[i]->Create(160, 12);
			m_radioExpDiv[i]->Move(20, 42 + i * 20);
			m_radioExpDiv[i]->SetText(texts[i]);
			m_radioExpDiv[i]->SendMsg(this, UM_SETSTATE, m_curExpDivBtn == i, 0, 0, 0);
			AddChild(m_radioExpDiv[i]);

			if (!g_session.m_amIPartyMaster)
				m_radioExpDiv[i]->m_isEnabled = 0;
		}
	}

	m_x = m_x; m_h = m_h;
	{
		int i;
		const char* texts[2];
		texts[0] = MsgStr(MSI_ITEMCOLLECT1); texts[1] = MsgStr(MSI_ITEMCOLLECT2);

		for (i = 0; i < 2; ++i) {
			m_radioItemCollect[i] = new UIRadioBtn;
			if (g_serviceType == ServiceBrazil)
				m_radioItemCollect[i]->Create(160, 17);
			else
				m_radioItemCollect[i]->Create(160, 12);
			m_radioItemCollect[i]->SetText(texts[i]);
			m_radioItemCollect[i]->Move(20, m_nYOffset + 37 + i * 20);
			m_radioItemDiv[i]->m_isEnabled = m_radioItemDiv[i]->m_isEnabled;
			m_radioItemCollect[i]->SendMsg(this, UM_SETSTATE, m_curItemCollectBtn == i, 0, 0, 0);
			m_radioItemCollect[i]->m_isEnabled = !g_session.GetNumParty();
			AddChild(m_radioItemCollect[i]);
		}
	}

	{
		int i;
		const char* texts[2];
		texts[0] = MsgStr(MSI_ITEMDIV1); texts[1] = MsgStr(MSI_ITEMDIV2);

		for (i = 0; i < 2; ++i) {
			m_radioItemDiv[i] = new UIRadioBtn;
			if (g_serviceType == ServiceBrazil)
				m_radioItemDiv[i]->Create(160, 17);
			else
				m_radioItemDiv[i]->Create(160, 12);
			m_radioItemDiv[i]->Move(20, m_nYOffset + 102 + i * 20);
			m_radioItemDiv[i]->SetText(texts[i]);
			m_radioItemDiv[i]->SendMsg(this, UM_SETSTATE, m_curItemDivBtn == i, 0, 0, 0);
			m_radioItemDiv[i]->m_isEnabled = !g_session.GetNumParty();
			AddChild(m_radioItemDiv[i]);
		}
	}




	if (0) {
		AddChild(new UIRadioBtn);
		AddChild(new UIRadioBtn);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";

		int pos[2][2];
		int ids[2];
		char* buttonName[2][3];

		buttonName[0][0] = "btn_ok";
		buttonName[0][1] = "btn_ok_a";
		buttonName[0][2] = "btn_ok_b";
		buttonName[1][0] = "btn_cancel";
		buttonName[1][1] = "btn_cancel_a";
		buttonName[1][2] = "btn_cancel_b";

		RAG_BUDGET_FILL(150)
		ids[0] = ID_OK;
		ids[1] = ID_CANCEL;

		pos[0][0] = m_w - 92;
		pos[0][1] = m_h - 24;
		pos[1][0] = m_w - 46;
		pos[1][1] = m_h - 24;

		for (i = 0; i < 2; ++i) {
			UIBitmapButton* button = new UIBitmapButton;
			button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			AddChild(button);
		}
	}

	{
		int posX = m_w - 14;
		char* toolTip[8] = { "Alt+P" };

		std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName2 + "close" + "_off.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName2 + "close" + "_on.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(posX, 3);
		button->SetId(ID_CLOSE);
		button->SetToolTip(toolTip[0]);
		AddChild(button);
	}
}

void UIPartySettingWnd::OnDraw() {}

void UIPartySettingWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}

void UIPartySettingWnd::StoreInfo() {
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

int UIPartySettingWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	int i;

	switch (message) {
	case UM_REFRESH:
		if (m_nYOffset == 5) {
			g_windowMgr.DeleteWindow(WID_PARTYSETTINGWND);
			break;
		}

		Invalidate();
		m_curExpDivBtn = g_session.m_expDivType;
		for (i = 0; i < 2; ++i) {
			m_radioExpDiv[i]->SendMsg(this, UM_SETSTATE, m_curExpDivBtn == i, 0, 0, 0);
			if (m_radioItemCollect[i] && m_radioItemDiv[i]) {
				m_radioItemCollect[i]->SendMsg(this, UM_SETSTATE, m_curItemCollectBtn == i, 0, 0, 0);
				m_radioItemDiv[i]->SendMsg(this, UM_SETSTATE, m_curItemDivBtn == i, 0, 0, 0);
			}
		}
		break;

	case UM_SETINFO:
		m_info = (PartySettingWndInfo *)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_CANCEL:
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_PARTYSETTINGWND);
			break;

		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_PARTYSETTINGWND);
			CMode *mode = g_modeMgr.GetGameMode();
			if (!g_session.GetNumParty()) {
				mode->SendMsg(MM_CONFORM_MAKEGROUP, 0, m_curItemCollectBtn, m_curItemDivBtn);
				break;
			}

			if (m_curExpDivBtn != g_session.m_expDivType && g_session.m_amIPartyMaster)
				mode->SendMsg(MM_CHANGE_GROUPEXPOPTION, m_curExpDivBtn, 0, 0);
			break;
		}

		case ID_RADIOBTN:
			if (g_session.GetNumParty() && !g_session.m_amIPartyMaster) {
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ONLY_PARTYMASTER), 0xffff64, 9, 0);
				break;
			}

			if (sender == m_radioExpDiv[0] || sender == m_radioExpDiv[1]) {
				for (i = 0; i < 2; ++i) {
					if (sender == m_radioExpDiv[i]) {
						m_curExpDivBtn = i;
						m_radioExpDiv[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
					} else {
						m_radioExpDiv[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
					}
				}
			}

			if (sender == m_radioItemCollect[0] || sender == m_radioItemCollect[1]) {
				for (i = 0; i < 2; ++i) {
					if (sender == m_radioItemCollect[i]) {
						m_curItemCollectBtn = i;
						m_radioItemCollect[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
					} else {
						m_radioItemCollect[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
					}
				}
			}

			if (sender == m_radioItemDiv[0] || sender == m_radioItemDiv[1]) {
				for (i = 0; i < 2; ++i) {
					if (sender == m_radioItemDiv[i]) {
						m_curItemDivBtn = i;
						m_radioItemDiv[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
					} else {
						m_radioItemDiv[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
					}
				}
			}
			break;
		}
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIWhisperWnd::UIWhisperWnd() { m_info = NULL; }
UIWhisperWnd::~UIWhisperWnd() {}

void UIWhisperWnd::OnCreate(int cx, int cy)
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
	m_listBox->Create2(0, 0, m_w - 2, m_h - 38, false);
	m_listBox->Move(1, 17);
	m_listBox->SetId(ID_OK);
	AddChild(m_listBox);

	{
		int posX[3] = { m_w - 14 };
		int ids2[3] = { ID_CLOSE };
		char* buttonName2[8] = { "close" };
		std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
		for (int i = 0; i < 1; i++) {
			m_sysButton = new UIBitmapButton;
			m_sysButton->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
			m_sysButton->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
			m_sysButton->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
			m_sysButton->Create(m_sysButton->GetBitmapWidth(), m_sysButton->GetBitmapHeight());
			m_sysButton->Move(posX[i], 3);
			m_sysButton->SetId(ids2[i]);
			m_sysButton->SetToolTip("/q");
			AddChild(m_sysButton);
		}
	}
}

void UIWhisperWnd::OnDraw()
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	char title[128];

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + (((m_w - 24) % 12) ? 1 : 0);
	int i;
	for (i = 0; i < titleMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(12 + i * 12, 0, bmp, false);
	}

	g_skinMgr.GetColorChipColor(20, 6, rr, gg, bb);
	DrawBox(0, 17, 1, m_h - 38, RGB(rr, gg, bb));
	DrawBox(m_w - 1, 17, 1, m_h - 38, RGB(rr, gg, bb));
	DrawBox(m_w - (m_w * 56 / 280) - 2, 17, 1, m_h - 38, RGB(rr, gg, bb));

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_width, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - 21, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (i = 0; i < btnMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp"), false);
		DrawBitmap(21 + i * 21, m_h - bmp->m_height, bmp, false);
	}

	sprintf(title, "With  %s", m_whisperTitle.c_str());
	TextOutA(10, 3, "With", 0, 0, 12, 0xffffff);
	TextOutA(9, 2, title, 0, 0, 12, 0);
}

void UIWhisperWnd::Move(int x, int y)
{
	UIFrameWnd::Move(x, y);
}


void UIWhisperWnd::StoreInfo()
{
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}

	g_windowMgr.DeleteWhisperWindow(m_whisperName);
}

void UIWhisperWnd::OnRBtnUp(int x, int y) {
	SendMsg(0, UM_MAKEMENU, 0, 0, 0, 0);
}

extern "C" int __cdecl access(const char *, int);
extern "C" int __cdecl _mkdir(const char *);

void UIWhisperWnd::WriteChatToFile()
{
	char tmp[2048];
	int captureCnt = 0;

	sprintf(tmp, "%s\\Whisper", g_baseDir3);
	if (access(tmp, 0)) {
		if (_mkdir(tmp))
			return;
	}

	std::string whisperName = m_whisperName;
	FixInvalidNameForFile(whisperName);

	sprintf(tmp, "%s\\Whisper\\%s.txt", g_baseDir3, whisperName.c_str());
	while (access(tmp, 0) == 0)
		sprintf(tmp, "%s\\Whisper\\%s%03d.txt", g_baseDir3, whisperName.c_str(), ++captureCnt);

	if (m_listBox->WriteChatToFile(tmp)) {
		std::string chatStoreMsg = tmp;
		chatStoreMsg += " is Saved.";
		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)chatStoreMsg.c_str(), 0xff00, 0, 0);
	}
}

void UIWhisperWnd::WriteChatToReportFile()
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

int UIWhisperWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (WhisperWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, 7, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			char chatBuf[256];
			strcpy(chatBuf, m_chatEdit->GetText());
			m_chatEdit->StoreInHistory();
			g_windowMgr.SetFocusEdit(m_chatEdit);
			if (!strlen(chatBuf))
				break;
			if (g_serviceType == ServiceAmerica) {
				if (!g_session.IsOnlyEnglish(chatBuf)) {
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ENGLISHONLY), 0xff, 0, 0);
					break;
				}
			}
			if (m_whisperName.size() && chatBuf[0] != '/') {
				g_windowMgr.SetChatMsg(chatBuf);
				g_modeMgr.GetCurMode()->SendMsg(MM_WHISPERMSG, (int)m_whisperName.c_str(), 0, 0);
			} else {
				std::string param[3];
				TALKTYPE talkType;
				int len = g_session.GetTalkType(chatBuf, &talkType, param);
				if (len != -1)
					g_windowMgr.SetChatMsg(chatBuf + len);
				if (talkType == TT_REQ_EXIT_ROOM)
					g_windowMgr.PostQuit(this);
				else if (talkType == TT_REQ_SAVE_CHAT)
					WriteChatToFile();
				else if (talkType == TT_REQ_REPORT_CHAT)
					WriteChatToReportFile();
				else
					g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, talkType, (int)param, 0);
			}
			break;
		}
		case ID_CLOSE:
			g_windowMgr.PostQuit(this);
			break;
		}
		break;
	case UM_MENU_SELECTED:
		if (m_menuItems[val1].id == 0xb)
			WriteChatToFile();
		break;
	case UM_MAKEMENU: {
		RAG_BUDGET_FILL(24)
		m_menuItems.erase(m_menuItems.begin(), m_menuItems.end());
		if (g_windowMgr.GetMenu())
			g_windowMgr.DeleteWindow(WID_MENUWND);
		m_menuItems.push_back(StringId(std::string(MsgStr(MSI_WRITE_CHAT_TO_FILE)).c_str(), 0xb));
		if (!g_windowMgr.IsFriendName(m_whisperName.c_str())) {
			if (g_serverType != 3)
				m_menuItems.push_back(StringId(std::string(MsgStr(MSI_ADD_TO_FRIEND_CHAR_LIST)).c_str(), 0x15));
		}
		g_modeMgr.GetCurMode()->SendMsg(MM_INITWHISPERMENU, (int)m_whisperName.c_str(), 0, 0);
		if (m_menuItems.size()) {
			UIFrameWnd* menu = g_windowMgr.MakeWindow(WID_MENUWND);
			for (int i = 0; i < m_menuItems.size(); ++i)
				menu->SendMsg(NULL, UM_ADDITEM, (int)m_menuItems[i].itemName.c_str(), 0, 0, 0);
			menu->SendMsg(NULL, UM_ADJUSTSIZE, 0, 0, 0, 0);
			int x = g_mouse.GetXPos() + 1;
			int y = g_mouse.GetYPos() + 1;
			if (x < 1)
				x = 1;
			if (x > g_renderer->m_width - menu->GetWidth())
				x = g_renderer->m_width - menu->GetWidth();
			if (y < 1)
				y = 1;
			if (y > g_renderer->m_height - menu->GetHeight())
				y = g_renderer->m_height - menu->GetHeight();
			menu->Move(x, y);
		}
		break;
	}
	case UM_ADDCHAT: {
		std::vector<std::string> strings;
		CutString((const char*)val1, strings, m_w * 80 / 640);
		for (int i = 0; i < strings.size(); ++i)
			m_listBox->AddItem(strings[i].c_str(), val2, NULL);
		break;
	}
	case UM_PAGEUPPUSH:
		if (m_listBox->IsVertScrEnabled())
			m_listBox->SendMsg(NULL, UM_SCROLLVERTPAGEUP, 0, 0, 0, 0);
		break;
	case UM_PAGEDOWNPUSH:
		if (m_listBox->IsVertScrEnabled())
			m_listBox->SendMsg(NULL, UM_SCROLLVERTPAGEDOWN, 0, 0, 0, 0);
		break;
	case UM_RESIZE:
		if (val1 == 7) {
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
			w = (w - 280) / 32 * 32 + 280;
			h = (h - 120) / 32 * 32 + 120;
			if (w != m_w || h != m_h) {
				Resize(w, h);
				m_resizer->Move(m_w - 15, m_h - 15);
				m_chatEdit->Resize(m_w - 30, 16);
				m_chatEdit->Move(3, m_h - 18);
				m_listBox->Resize(m_w - 2, m_h - 38);
				m_listBox->Move(1, 17);
				m_sysButton->Move(m_w - 14, 3);
			}
			g_windowMgr.UpdateSnapInfo(this);
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}


void UIMinimapZoomWnd::OnCreate(int cx, int cy)
{
	char* buttonName2[8] = { "plus", "minus" };
	int posY[3] = { 0, 12 };
	int ids2[3] = { ID_PLUS, ID_MINUS };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\map\\map_";

	for (int i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "0.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "1.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName2 + buttonName2[i] + "1.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(0, posY[i]);
		button->SetId(ids2[i]);
		AddChild(button);
	}
}

void UIMinimapZoomWnd::OnDraw() {}

int UIMinimapZoomWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_PLUS:
			{
				float t = g_windowMgr.m_miniMapZoomFactor / 0.75f;
				g_windowMgr.m_miniMapZoomFactor += t * 0.25f;
			}
			if (g_windowMgr.m_miniMapZoomFactor > 4.0f)
				g_windowMgr.m_miniMapZoomFactor = 4.0f;
			break;
		case ID_MINUS:
			{
				float t = g_windowMgr.m_miniMapZoomFactor * 0.25f;
				g_windowMgr.m_miniMapZoomFactor -= t;
			}
			if (g_windowMgr.m_miniMapZoomFactor < 1.0f)
				g_windowMgr.m_miniMapZoomFactor = 1.0f;
			break;
		}
		break;
	default:
		return UIWindow::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

void UINotifyLevelUpWnd::OnCreate(int cx, int cy)
{
	int ids2[3] = { ID_STATUS };
	int posY[3] = { 0 };

	char* buttonName2[8] = { "lv_up" };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\";

	UIBitmapButton* button = new UIBitmapButton;
	button->SetBitmapName((preBmpName2 + buttonName2[0] + "_off.bmp").c_str(), BS_NORMAL);
	button->SetBitmapName((preBmpName2 + buttonName2[0] + "_off.bmp").c_str(), BS_MOUSEON);
	button->SetBitmapName((preBmpName2 + buttonName2[0] + "_on.bmp").c_str(), BS_PRESSED);
	button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
	button->Move(0, posY[0]);
	button->SetId(ids2[0]);
	AddChild(button);
}

void UINotifyLevelUpWnd::OnDraw() {}

int UINotifyLevelUpWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		if (val1 == 0x7e) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_STATUSWND);
			if (w->GetHeight() == 17) {
				w->SendMsg(0, UM_COMMAND, 0x88, 0, 0, 0);
			}
			g_windowMgr.MakeTopLayer(w);
		}
		break;
	default:
		return UIWindow::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

int UINotifyJobLevelUpWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		if (val1 == 0x7e) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_SKILLLISTWND);
			g_windowMgr.MakeTopLayer(w);
		}
		break;
	default:
		return UIWindow::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

void UINotifyQuestWnd::OnCreate(int cx, int cy)
{
	int ids2[3] = { ID_STATUS };
	int posY[3] = { 0 };

	char* buttonName2[8] = { "quest" };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\";

	UIBitmapButton* button = new UIBitmapButton;
	button->SetBitmapName((preBmpName2 + buttonName2[0] + "_off.bmp").c_str(), BS_NORMAL);
	button->SetBitmapName((preBmpName2 + buttonName2[0] + "_off.bmp").c_str(), BS_MOUSEON);
	button->SetBitmapName((preBmpName2 + buttonName2[0] + "_on.bmp").c_str(), BS_PRESSED);
	button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
	button->Move(0, posY[0]);
	button->SetId(ids2[0]);
	AddChild(button);
}

void UINotifyQuestWnd::OnDraw() {}

int UINotifyQuestWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	switch (message) {
	case UM_COMMAND:
		if (val1 == 0x7e) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_QUESTWND);
			g_windowMgr.MakeTopLayer(w);
		}
		break;
	default:
		return UIWindow::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIMenuWnd::UIMenuWnd()
{
	m_callerWid = (WINDOWID)0x97;
	m_menuItemSpacing = 14;
	m_curMenuIndex = -1;
}
UIMenuWnd::~UIMenuWnd() {}

void UIMenuWnd::OnCreate(int cx, int cy) {}

void UIMenuWnd::OnLBtnUp(int x, int y)
{
	m_curMenuIndex = GetMenuIndex(x, y);
	if (m_curMenuIndex == -1 || IsBoundary(m_curMenuIndex))
		return;

	int numBoundary = GetNumBoundary(m_curMenuIndex);
	int selectIndex = (m_curMenuIndex - numBoundary) / 2;
	UIWindow* caller = g_windowMgr.QueryWindow(m_callerWid);
	if (caller) {
		caller->SendMsg(NULL, UM_MENU_SELECTED, selectIndex, 0, 0, 0);
	} else {
		if (m_callerWid == WID_LAST) {
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_MENUSELECTED, selectIndex, 0, 0);
		}
	}

	g_windowMgr.DeleteWindow(WID_MENUWND);
}

int UIMenuWnd::GetMenuIndex(int x, int y) {
	if (x < 0 || x >= m_w || y < 0 || y >= m_h)
		return -1;
	x = y / (m_menuItemSpacing / 2);
	if (x < 0) x = 0;
	if ((unsigned)x > m_menuItems.size() - 1)
		x = (int)(m_menuItems.size() - 1);
	return x;
}

int UIMenuWnd::GetNumBoundary(int seq)
{
	int numItems = (int)m_menuItems.size();
	if (numItems == 0)
		return 0;

	if (seq >= numItems)
		seq = numItems - 1;

	int numBoundary = 0;
	for (int i = 0; i < seq; ++i) {
		if (m_menuItems[i].compare(boundaryText) == 0)
			++numBoundary;
	}

	return numBoundary;
}

int UIMenuWnd::IsBoundary(int curItem) {
	if (m_menuItems[curItem].compare(boundaryText) == 0) {
		return 1;
	}
	return 0;
}

void UIMenuWnd::OnDraw()
{
	int i;
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	g_skinMgr.GetColorChipColor(6, 6, rr, gg, bb);
	ClearDC(RGB(rr, gg, bb));

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(1, 1, m_w - 2, m_h - 2, RGB(rr, gg, bb));
	}

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sysboxs_lu.bmp"), false);
	DrawBitmap(0, 0, bmp, false);
	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sysboxs_ld.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);
	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sysboxs_ru.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);
	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sysboxs_rd.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
	int menuSize = (int)m_menuItems.size();
	for (i = 0; i < menuSize; ++i) {
		int isBoundary;
		if (m_menuItems[i].compare(boundaryText) == 0) {
			isBoundary = 1;
		} else {
			isBoundary = 0;
		}

		int isFirst = 1;
		if (i > 0) {
			if (m_menuItems[i - 1].compare(m_menuItems[i]) == 0)
				isFirst = 0;
		}

		if (i == m_curMenuIndex) {
			if (!isBoundary) {
				if (isFirst)
					DrawBox(3, m_menuItemSpacing / 2 * i + 1, m_w - 6, m_menuItemSpacing - 2, RGB(rr, gg, bb));
				else
					DrawBox(3, m_menuItemSpacing / 2 * (i - 1) + 1, m_w - 6, m_menuItemSpacing - 2, RGB(rr, gg, bb));
			}
		}

		if (!isBoundary) {
			if (isFirst)
				TextOutA(3, (m_menuItemSpacing * i) / 2 + 1, m_menuItems[i].c_str(), 0, 0, 12, 0);
			else
				TextOutA(3, (m_menuItemSpacing * (i - 1)) / 2 + 1, m_menuItems[i].c_str(), 0, 0, 12, 0);
		} else {
			unsigned long rr;
			unsigned long gg;
			unsigned long bb;
			g_skinMgr.GetColorChipColor(14, 6, rr, gg, bb);
			DrawBox(0, (m_menuItemSpacing * i) / 2 + 2, m_w, 1, RGB(rr, gg, bb));
		}
	}
}

void UIMenuWnd::OnMouseMove(int x, int y) {
	OnMouseHover(x, y);
}

void UIMenuWnd::OnMouseHover(int x, int y) {
	int prev = m_curMenuIndex;
	m_curMenuIndex = GetMenuIndex(x, y);
	if (prev != m_curMenuIndex)
		Invalidate();
}

void UIMenuWnd::Move(int x, int y)
{
	if (y > g_renderer->m_height - m_h)
		y -= m_h + 2;
	if (x > g_renderer->m_width - m_w)
		x -= m_w + 2;
	UIWindow::Move(x, y);
}

void UIMenuWnd::AddMenu(const char* menuString)
{
	m_menuItems.push_back(menuString);
}

int UIMenuWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_ADDBOUNDARY:
		AddMenu(boundaryText);
		Invalidate();
		break;
	case UM_ADDITEM:
		AddMenu((const char*)val1);
		AddMenu((const char*)val1);
		Invalidate();
		break;
	case UM_SET_CALLER_WID:
		m_callerWid = (WINDOWID)val1;
		break;
	case UM_ADJUSTSIZE: {
		int numMenuItem = (int)m_menuItems.size();
		int width = 20;
		for (int i = 0; i < numMenuItem; ++i)
			width = max(width, GetTextWidth(m_menuItems[i].c_str(), 0, 0, 12, false));
		Resize(width + 6, m_menuItemSpacing * numMenuItem / 2);
		Invalidate();
		break;
	}
	default:
		return UIWindow::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UIComboBoxWnd::UIComboBoxWnd() { m_comboBox = NULL; }
UIComboBoxWnd::~UIComboBoxWnd() {}

void UIComboBoxWnd::OnCreate(int cx, int cy) {
	m_comboBox = new UIComboBox;
	m_comboBox->Create2(0, 0, cx, cy, true);
	AddChild(m_comboBox);
}



void UIComboBoxWnd::Resize(int cx, int cy) {
	UIFrameWnd::Resize(cx, cy);
	m_comboBox->Resize(cx, cy);
	m_comboBox->RecalcScrbarPos();
	m_comboBox->Invalidate();
}

void UIComboBoxWnd::OnDraw() {
	ClearDC(0xffff00ff);
}

int UIComboBoxWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	return m_comboBox->SendMsg(sender, message, val1, val2, val3, val4);
}
UIPetEggListWnd::UIPetEggListWnd() {}
UIPetEggListWnd::~UIPetEggListWnd() {}

void UIPetEggListWnd::OnDraw()
{
	m_title = MsgStr(MSI_PET_EGG_LIST);
	UIItemIdentifyWnd::OnDraw();
}

int UIPetEggListWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_ADDITEM: {
		ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(val2);
		m_itemInfoList.push_back(itemInfo);
		SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		return 0;
	}
	case UM_REFRESH:
		RecalcScrbarPos();
		Invalidate();
		return 0;
	case UM_SCROLLVERT:
	case UM_SCROLLVERTPAGEUP:
	case UM_SCROLLVERTPAGEDOWN: {
		int oldViewOffset = m_viewOffset;
		if (message == UM_SCROLLVERTPAGEUP)
			m_viewOffset = oldViewOffset - m_itemVertNum + 1;
		else if (message == UM_SCROLLVERTPAGEDOWN)
			m_viewOffset = oldViewOffset + m_itemVertNum - 1;
		else
			m_viewOffset = oldViewOffset + val1;

		if (oldViewOffset != m_viewOffset) {
			RecalcScrbarPos();
			Invalidate();
		}
		return m_viewOffset - oldViewOffset;
	}
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			int isValid;
			ITEM_INFO info = GetItemInfo(m_curItem, isValid);
			if (isValid) {
				info.m_isDamaged = 1;
				g_session.SetItem(&info);
				g_session.m_petEggIndex = info.m_itemIndex;
				UIWindow* itemWnd = g_windowMgr.GetItemWnd();
				if (itemWnd)
					itemWnd->Invalidate();
				g_modeMgr.GetCurMode()->SendMsg(MM_PETEGG_SELECT_FROM_LIST, info.m_itemIndex, 0, 0);
				g_windowMgr.DeleteWindow(WID_SELECTPETEGGWND);
			}
			return 0;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_SELECTPETEGGWND);
			break;
		}
		break;
	}

	return 0;
}

UIMakingArrowListWnd::UIMakingArrowListWnd() {}
UIMakingArrowListWnd::~UIMakingArrowListWnd() {}

void UIMakingArrowListWnd::OnDraw()
{
	m_title = "LIST";
	UIItemIdentifyWnd::OnDraw();
}

int UIMakingArrowListWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_ADDITEM: {
		ITEM_INFO itemInfo;
		itemInfo.SetItemId(*(unsigned short*)val1);
		itemInfo.m_isIdentified = 1;
		m_itemInfoList.push_back(itemInfo);
		SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		return 0;
	}
	case UM_REFRESH:
		RecalcScrbarPos();
		Invalidate();
		return 0;

	case UM_SETINFO:
		m_mkType = (short)val1;
		return 0;

	case UM_SCROLLVERT:
	case UM_SCROLLVERTPAGEUP:
	case UM_SCROLLVERTPAGEDOWN: {
		int oldViewOffset = m_viewOffset;
		if (message == UM_SCROLLVERTPAGEUP)
			m_viewOffset = oldViewOffset - m_itemVertNum + 1;
		else if (message == UM_SCROLLVERTPAGEDOWN)
			m_viewOffset = m_itemVertNum + oldViewOffset - 1;
		else
			m_viewOffset = oldViewOffset + val1;

		if (oldViewOffset != m_viewOffset) {
			RecalcScrbarPos();
			Invalidate();
		}
		return m_viewOffset - oldViewOffset;
	}

	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			int isValid;
			ITEM_INFO info = GetItemInfo(m_curItem, isValid);
			if (isValid) {
				if (m_mkType == MKTYPE_ARROW)
					g_modeMgr.GetCurMode()->SendMsg(MM_ARROW_FROM_LIST, atoi(info.m_itemName.c_str()), 0, 0);
				else {
					int itemId = atoi(info.m_itemName.c_str());
					g_modeMgr.GetCurMode()->SendMsg(MM_ITEM_FROM_LIST, itemId, m_mkType, 0);
				}
				g_windowMgr.DeleteWindow(WID_MAKINGARROWLISTWND);
			}
			break;
		}
		case ID_CANCEL:
			if (m_mkType == MKTYPE_ARROW)
				g_modeMgr.GetCurMode()->SendMsg(MM_ARROW_FROM_LIST, -1, 0, 0);
			else
				g_modeMgr.GetCurMode()->SendMsg(MM_ITEM_FROM_LIST, -1, m_mkType, 0);
			g_windowMgr.DeleteWindow(WID_MAKINGARROWLISTWND);
			break;
		}
		break;
	}

	return 0;
}

UIPetTamingDeceiveWnd::UIPetTamingDeceiveWnd()
{
	m_delay = 1;
	m_curAct = 0;
	m_curMot = 0;
	m_destPos = -1;
	m_maxDelay = 10000;
	m_isSelected = 0;
	m_startTick = -1;
}
UIPetTamingDeceiveWnd::~UIPetTamingDeceiveWnd() {}

void UIPetTamingDeceiveWnd::OnCreate(int cx, int cy) {}
#include "rag_entropy_pad.h"
RAG_ENTROPY_SHIFT(pettaming, 42)
void UIPetTamingDeceiveWnd::OnDraw()
{
	CActRes* actRes = (CActRes*)g_resMgr().Get("SlotMachine.act", false);
	CSprRes* sprRes = (CSprRes*)g_resMgr().Get("SlotMachine.spr", false);
	int curMot = m_curMot / m_delay;
	CMotion* curMotion = actRes->GetMotion(m_curAct, curMot);
	CacheInfo info;
	int a = 0;
	unsigned long* palette = sprRes->m_pal;

	for (; a < curMotion->numClips; ++a) {
		CSprClip* clip = curMotion->GetClip(a);
		SprImg* img = sprRes->m_sprites[0][clip->sprIndex];
		CTexture* tex = g_renderer->GetSpriteIndex(*img, (unsigned long)palette, &info);
		if (tex == NULL)
			tex = g_renderer->AddSpriteIndex(*img, palette, &info);

		int drawY = m_h / 2 + clip->y;
		int drawX = m_w / 2 + clip->x;
		m_dc->BltTexture2(drawX, drawY, tex,
			(int)(info.tu * 256.0f), (int)(info.tv * 256.0f),
			img->width, img->height, clip->flags, img->isHalfW + 1, img->isHalfH + 1);
	}

	if (m_destPos != curMot || m_delay > m_maxDelay)
		++m_curMot;

	if (curMot >= actRes->GetMotionCount(m_curAct)) {
		m_curMot = 0;
		if (m_destPos != -1)
			++m_delay;
	}
}
RAG_ENTROPY_SHIFT(pettaming_restore, 21)


void UIPetTamingDeceiveWnd::OnLBtnDown(int x, int y)
{
	if (m_isSelected)
		return;
	m_curAct = 1;
	m_destPos = 7;
	m_isSelected = 1;
	g_modeMgr.GetGameMode()->SendMsg(MM_PETDECEIVE_SHOOT, m_monGID, 0, 0);
}

void UIPetTamingDeceiveWnd::OnProcess()
{
	Invalidate();
	int q = m_curMot / m_delay;
	if (m_startTick == (unsigned long)-1 && m_destPos == q && m_delay <= m_maxDelay) {
		m_startTick = timeGetTime();
		return;
	}
	if (m_startTick != (unsigned long)-1 && m_destPos == q && m_delay <= m_maxDelay) {
		if (timeGetTime() - m_startTick >= 0x7d0)
			g_windowMgr.DeleteWindow(WID_PETTAMINGDECEIVEWND);
	}
}

int UIPetTamingDeceiveWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	if (message == 0x50) {
		if (val1 == 0) {
			m_monGID = val2;
		} else {
			m_destPos = val2;
			m_delay = 7;
			m_maxDelay = 7;
		}
	}
	return 0;
}
UISelectCartWnd::UISelectCartWnd() {}
UISelectCartWnd::~UISelectCartWnd() {}

void UISelectCartWnd::OnCreate(int cx, int cy)
{
	int posX = m_w - 14;
	char buttonName[] = "close";
	std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
	char toolTip[] = "";

	UIBitmapButton* closeButton = new UIBitmapButton;
	closeButton->SetBitmapName((preBmpName + buttonName + "_off.bmp").c_str(), 0);
	closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 1);
	closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), 2);
	closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
	closeButton->Move(posX, 3);
	closeButton->SetId(ID_CLOSE);
	closeButton->SetToolTip(toolTip);
	AddChild(closeButton);

	int cartCount;
	int level = g_session.GetLevel();
	if (level > 90)
		cartCount = 5;
	else if (level > 80)
		cartCount = 4;
	else if (level > 65)
		cartCount = 3;
	else if (level > 40)
		cartCount = 2;
	else
		cartCount = 1;

	switch (cartCount) {
	case 5:
		m_sprButton[0] = new UISpriteButton;
		m_sprButton[0]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.spr");
		m_sprButton[0]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.act");
		m_sprButton[0]->Create(70, 70);
		m_sprButton[0]->Move(11, 28);
		m_sprButton[0]->SetAction(0);
		m_sprButton[0]->SetMotion(0);
		m_sprButton[0]->SetId(0x2328);
		AddChild(m_sprButton[0]);
		RAG_BUDGET_FILL(200)
		m_sprButton[1] = new UISpriteButton;
		m_sprButton[1]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.spr");
		m_sprButton[1]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.act");
		m_sprButton[1]->Create(70, 70);
		m_sprButton[1]->Move(81, 28);
		m_sprButton[1]->SetAction(0);
		m_sprButton[1]->SetMotion(0);
		m_sprButton[1]->SetId(0x2329);
		AddChild(m_sprButton[1]);
		m_sprButton[2] = new UISpriteButton;
		m_sprButton[2]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""2.spr");
		m_sprButton[2]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""2.act");
		m_sprButton[2]->Create(70, 70);
		m_sprButton[2]->Move(151, 28);
		m_sprButton[2]->SetAction(0);
		m_sprButton[2]->SetMotion(0);
		m_sprButton[2]->SetId(0x232a);
		AddChild(m_sprButton[2]);
		m_sprButton[3] = new UISpriteButton;
		m_sprButton[3]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""3.spr");
		m_sprButton[3]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""3.act");
		m_sprButton[3]->Create(70, 70);
		m_sprButton[3]->Move(221, 28);
		m_sprButton[3]->SetAction(0);
		m_sprButton[3]->SetMotion(0);
		m_sprButton[3]->SetId(0x232b);
		AddChild(m_sprButton[3]);
		m_sprButton[4] = new UISpriteButton;
		m_sprButton[4]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""4.spr");
		m_sprButton[4]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""4.act");
		m_sprButton[4]->Create(70, 70);
		m_sprButton[4]->Move(291, 28);
		m_sprButton[4]->SetAction(0);
		m_sprButton[4]->SetMotion(0);
		m_sprButton[4]->SetId(0x232c);
		AddChild(m_sprButton[4]);
		break;
	case 4:
		m_sprButton[0] = new UISpriteButton;
		m_sprButton[0]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.spr");
		m_sprButton[0]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.act");
		m_sprButton[0]->Create(70, 70);
		m_sprButton[0]->Move(11, 28);
		m_sprButton[0]->SetAction(0);
		m_sprButton[0]->SetMotion(0);
		m_sprButton[0]->SetId(0x2328);
		AddChild(m_sprButton[0]);
		m_sprButton[1] = new UISpriteButton;
		m_sprButton[1]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.spr");
		m_sprButton[1]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.act");
		m_sprButton[1]->Create(70, 70);
		m_sprButton[1]->Move(81, 28);
		m_sprButton[1]->SetAction(0);
		m_sprButton[1]->SetMotion(0);
		m_sprButton[1]->SetId(0x2329);
		AddChild(m_sprButton[1]);
		m_sprButton[2] = new UISpriteButton;
		m_sprButton[2]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""2.spr");
		m_sprButton[2]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""2.act");
		m_sprButton[2]->Create(70, 70);
		m_sprButton[2]->Move(151, 28);
		m_sprButton[2]->SetAction(0);
		m_sprButton[2]->SetMotion(0);
		m_sprButton[2]->SetId(0x232a);
		AddChild(m_sprButton[2]);
		m_sprButton[3] = new UISpriteButton;
		m_sprButton[3]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""3.spr");
		m_sprButton[3]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""3.act");
		m_sprButton[3]->Create(70, 70);
		m_sprButton[3]->Move(221, 28);
		m_sprButton[3]->SetAction(0);
		m_sprButton[3]->SetMotion(0);
		m_sprButton[3]->SetId(0x232b);
		AddChild(m_sprButton[3]);
		break;
	case 3:
		m_sprButton[0] = new UISpriteButton;
		m_sprButton[0]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.spr");
		m_sprButton[0]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.act");
		m_sprButton[0]->Create(70, 70);
		m_sprButton[0]->Move(11, 28);
		m_sprButton[0]->SetAction(0);
		m_sprButton[0]->SetMotion(0);
		m_sprButton[0]->SetId(0x2328);
		AddChild(m_sprButton[0]);
		m_sprButton[1] = new UISpriteButton;
		m_sprButton[1]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.spr");
		m_sprButton[1]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.act");
		m_sprButton[1]->Create(70, 70);
		m_sprButton[1]->Move(81, 28);
		m_sprButton[1]->SetAction(0);
		m_sprButton[1]->SetMotion(0);
		m_sprButton[1]->SetId(0x2329);
		AddChild(m_sprButton[1]);
		m_sprButton[2] = new UISpriteButton;
		m_sprButton[2]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""2.spr");
		m_sprButton[2]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""2.act");
		m_sprButton[2]->Create(70, 70);
		m_sprButton[2]->Move(151, 28);
		m_sprButton[2]->SetAction(0);
		m_sprButton[2]->SetMotion(0);
		m_sprButton[2]->SetId(0x232a);
		AddChild(m_sprButton[2]);
		break;
	case 2:
		m_sprButton[0] = new UISpriteButton;
		m_sprButton[0]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.spr");
		m_sprButton[0]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.act");
		m_sprButton[0]->Create(70, 70);
		m_sprButton[0]->Move(11, 28);
		m_sprButton[0]->SetAction(0);
		m_sprButton[0]->SetMotion(0);
		m_sprButton[0]->SetId(0x2328);
		AddChild(m_sprButton[0]);
		m_sprButton[1] = new UISpriteButton;
		m_sprButton[1]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.spr");
		m_sprButton[1]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9""1.act");
		m_sprButton[1]->Create(70, 70);
		m_sprButton[1]->Move(81, 28);
		m_sprButton[1]->SetAction(0);
		m_sprButton[1]->SetMotion(0);
		m_sprButton[1]->SetId(0x2329);
		AddChild(m_sprButton[1]);
		break;
	case 1:
		m_sprButton[0] = new UISpriteButton;
		m_sprButton[0]->SetSpriteName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.spr");
		m_sprButton[0]->SetActFileName("\xc0\xcc\xc6\xd1\xc6\xae\\\xbc\xd5\xbc\xf6\xb7\xb9.act");
		m_sprButton[0]->Create(70, 70);
		m_sprButton[0]->Move(11, 28);
		m_sprButton[0]->SetAction(0);
		m_sprButton[0]->SetMotion(0);
		m_sprButton[0]->SetId(0x2328);
		AddChild(m_sprButton[0]);
		break;
	}

}
void UISelectCartWnd::OnDraw()
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	char windowName[32];

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleBarMidCount = (m_w - 24) / 12 + (((m_w - 24) % 12) ? 1 : 0);
	int i;
	for (i = 0; i < titleBarMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	sprintf(windowName, MsgStr(MSI_CART_LIST));
	TextOutA(5, 2, windowName, 0, 0, 12, 0);

	g_skinMgr.GetColorChipColor(14, 6, rr, gg, bb);
	DrawBox(0, 17, m_w, m_h - 17, RGB(rr, gg, bb));
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	DrawBox(1, 17, m_w - 2, m_h - 17, RGB(rr, gg, bb));

	char* bmpName[3] = {
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysboxs_ld.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysboxs_rd.bmp"
	};
	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);
	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);
}




void UISelectCartWnd::OnProcess() {}

void UISelectCartWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}

int UISelectCartWnd::SendMsg(UIWindow * sender, int message, int val1, int val2, int val3, int val4)
{
	if (message == UM_COMMAND) {
		switch (val1) {
		case 0x2328:
			g_modeMgr.GetGameMode()->SendMsg(MM_SELECT_CART, 1, 0, 0);
			g_windowMgr.DeleteWindow(WID_SELECTCARTWND);
			break;
		case 0x2329:
			g_modeMgr.GetGameMode()->SendMsg(MM_SELECT_CART, 2, 0, 0);
			g_windowMgr.DeleteWindow(WID_SELECTCARTWND);
			break;
		case 0x232a:
			g_modeMgr.GetGameMode()->SendMsg(MM_SELECT_CART, 3, 0, 0);
			g_windowMgr.DeleteWindow(WID_SELECTCARTWND);
			break;
		case 0x232b:
			g_modeMgr.GetGameMode()->SendMsg(MM_SELECT_CART, 4, 0, 0);
			g_windowMgr.DeleteWindow(WID_SELECTCARTWND);
			break;
		case 0x232c:
			g_modeMgr.GetGameMode()->SendMsg(MM_SELECT_CART, 5, 0, 0);
			g_windowMgr.DeleteWindow(WID_SELECTCARTWND);
			break;
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_SELECTCARTWND);
			break;
		}
	}
	return 0;
}

UISpellListWnd::UISpellListWnd()
{}
UISpellListWnd::~UISpellListWnd()
{
	m_spellList.clear();
}

void UISpellListWnd::OnDraw()
{
	unsigned long gg;
	unsigned long rr;
	unsigned long bb;
	int i;
	int drawCount;
	char text[256];
	int itemCount;
	char skillName[64];

	m_title = MsgStr(MSI_SPELL_LIST);

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + (((m_w - 24) % 12) ? 1 : 0);
	for (i = 0; i < titleMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	TextOutA(17, 3, m_title.c_str(), 0, 0, 12, 0xffffff);
	TextOutA(16, 2, m_title.c_str(), 0, 0, 12, 0);

	g_skinMgr.GetColorChipColor(14, 6, rr, gg, bb);
	DrawBox(0, 17, m_w, m_h - 17, RGB(rr, gg, bb));
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	DrawBox(1, 17, m_w - 2, m_h - 17, RGB(rr, gg, bb));

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (i = 0; i < btnMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	drawCount = 0;
	itemCount = 0;
	i = 0;
	SKILL_INFO skillInfo;
	while (i < m_spellList.size()) {
		strcpy(skillName, SKILL_INFO::GetSkillNameBySkillId(m_spellList[i]));
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			i++;
			itemCount++;
		} else {
			int y = (drawCount / m_itemHorzNum) * 32 + 34;
			if (m_curItem == itemCount) {
				unsigned long gg;
				unsigned long rr;
				unsigned long bb;
				g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
				DrawBox(5, y - 3, m_w - 24, 30, RGB(rr, gg, bb));
			}

			skillInfo.SKID = m_spellList[i];
			g_session.GetSkillBitmapFileName(skillInfo.GetSkillIdName(), text);
			bmp = (CBitmapRes*)g_resMgr().Get(text, false);
			DrawBitmap(8, y, bmp, true);
			TextOutA(40, y + 4, skillName, 0, 0, 12, 0);

			if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
				break;
			drawCount++;
			i++;
			itemCount++;
		}
	}
}

int UISpellListWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_ADDITEM: {
		m_spellList.push_back(val1);

		ITEM_INFO info;
		m_itemInfoList.push_back(info);
		SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		return 0;
	}

	case UM_REFRESH:
		RecalcScrbarPos();
		Invalidate();
		return 0;

	case UM_SCROLLVERT:
	case UM_SCROLLVERTPAGEUP:
	case UM_SCROLLVERTPAGEDOWN: {
		int oldViewOffset = m_viewOffset;
		if (message == UM_SCROLLVERTPAGEUP)
			m_viewOffset = oldViewOffset - m_itemVertNum + 1;
		else if (message == UM_SCROLLVERTPAGEDOWN)
			m_viewOffset = m_itemVertNum + oldViewOffset - 1;
		else
			m_viewOffset = oldViewOffset + val1;

		if (oldViewOffset != m_viewOffset) {
			RecalcScrbarPos();
			Invalidate();
		}
		return m_viewOffset - oldViewOffset;
	}

	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_modeMgr.GetCurMode()->SendMsg(MM_SELECT_SPELL, m_spellList[m_curItem], 0, 0);
			g_windowMgr.DeleteWindow(WID_SPELLLISTWND);
			break;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_SPELLLISTWND);
			break;
		}
		break;
	}

	return 0;
}
UIGraffiStrboxWnd::UIGraffiStrboxWnd() {}
UIGraffiStrboxWnd::~UIGraffiStrboxWnd() {}

void UIGraffiStrboxWnd::OnCreate(int cx, int cy)
{
	m_trapText = new UIEditCtrl;
	m_trapText->Create(240, 16);
	m_trapText->Move(22, 50);
	m_trapText->SetTextLimit(78);
	m_trapText->SetFrameColor(0xf2, 0xf2, 0xf2);
	AddChild(m_trapText);
	g_windowMgr.SetFocusEdit(m_trapText);

	std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");

	int pos[2][2];
	int ids[2];
	char* buttonName[2][3];
	buttonName[0][0] = "btn_ok";
	buttonName[0][1] = "btn_ok_a";
	buttonName[0][2] = "btn_ok_b";
	buttonName[1][0] = "btn_cancel";
	buttonName[1][1] = "btn_cancel_a";
	buttonName[1][2] = "btn_cancel_b";
	ids[0] = ID_OK;
	ids[1] = ID_CANCEL;
	pos[0][0] = 189;
	pos[0][1] = 96;
	pos[1][0] = 234;
	pos[1][1] = 96;

	int i;
	for (i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[i][0], pos[i][1]);
		button->SetId(ids[i]);
		AddChild(button);
	}

	m_defPushId = ID_OK;
}

void UIGraffiStrboxWnd::OnDraw()
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

	TextOutA(17, 23, MsgStr(MSI_TALKBOX_WINDOW_MSG), 0, 0, 12, 0xffffff);
	TextOutA(16, 22, MsgStr(MSI_TALKBOX_WINDOW_MSG), 0, 0, 12, 0);
}

int UIGraffiStrboxWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	int values[5];

	switch (message) {
	case UM_SET_SKILL_INFO: {
		int* recvValues = (int*)val1;
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
UIProhibitListWnd::UIProhibitListWnd() { m_oldLogCnt = 0; }
UIProhibitListWnd::~UIProhibitListWnd() {}

void UIProhibitListWnd::OnCreate(int cx, int cy)
{
	int x = m_w - 14;
	char buttonName[] = "close";
	std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
	char toolTip[] = "Close";

	UIBitmapButton* closeButton = new UIBitmapButton;
	closeButton->SetBitmapName((preBmpName + buttonName + "_off.bmp").c_str(), BS_NORMAL);
	closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), BS_MOUSEON);
	closeButton->SetBitmapName((preBmpName + buttonName + "_on.bmp").c_str(), BS_PRESSED);
	closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
	closeButton->Move(x, 3);
	closeButton->SetId(ID_CLOSE);
	closeButton->SetToolTip(toolTip);
	AddChild(closeButton);
	{
		char toolTip[] = "Command";

		UINoneVisibleButton* commandButton = new UINoneVisibleButton;
		commandButton->Create(120, 18);
		commandButton->Move(350, 59);
		commandButton->SetId(ID_SKINSELECTCOMBO);
		AddChild(commandButton);

		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\";

		UIBitmapButton* dropButton = new UIBitmapButton;
		dropButton->SetBitmapName((pathName + "txtbox_btn_a" + ".bmp").c_str(), BS_NORMAL);
		dropButton->SetBitmapName((pathName + "txtbox_btn_b" + ".bmp").c_str(), BS_MOUSEON);
		dropButton->SetBitmapName((pathName + "txtbox_btn_c" + ".bmp").c_str(), BS_PRESSED);
		dropButton->Create(dropButton->GetBitmapWidth(), dropButton->GetBitmapHeight());
		dropButton->Move(270 - dropButton->GetBitmapWidth(), 44);
		dropButton->SetToolTip(toolTip);
		dropButton->SetId(ID_SKINSELECTCOMBO);
		AddChild(dropButton);
	}
}

void UIProhibitListWnd::OnProcess()
{
	if (m_oldLogCnt != (int)g_session.m_chatProhibitLog.size()) {
		m_oldLogCnt = (int)g_session.m_chatProhibitLog.size();
		Invalidate();
	}
}

void UIProhibitListWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleBarMidCount = ((m_w - 24) % 12 != 0) + (m_w - 24) / 12;
	for (int c = 0; c < titleBarMidCount; c++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((c + 1) * 12, 0, bmp, false);
	}

	char windowName[32];
	sprintf(windowName, MsgStr(MSI_PROHIBIT_LIST));
	TextOutA(5, 2, windowName, 0, 0, 12, 0);

	unsigned long rr, gg, bb;
	g_skinMgr.GetColorChipColor(14, 6, rr, gg, bb);
	DrawBox(0, 17, m_w, m_h - 17, RGB(rr, gg, bb));
	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	DrawBox(1, 17, m_w - 2, m_h - 17, RGB(rr, gg, bb));

	char* bmpName[3] = {
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysboxs_ld.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\sysboxs_rd.bmp"
	};

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	char msg[256];
	int y = 0;
	int logCnt = g_session.m_chatProhibitLog.size();
	for (c = 0; c < logCnt; c++) {
		if (strcmp(g_session.m_chatProhibitLog[c].serverName, g_session.m_lastServerName.c_str()) == 0
			&& g_session.m_chatProhibitLog[c].AID == m_AID) {
			if (!y) {
				sprintf(msg, "AccountName : %s        CharacterName : %s", g_session.m_chatProhibitLog[c].accountName, g_session.m_chatProhibitLog[c].charName);
				TextOutA(10, 25, msg, 0, 0, 12, 0);

				sprintf(msg, "ServerName : %s", g_session.m_chatProhibitLog[c].serverName);
				TextOutA(10, 41, msg, 0, 0, 12, 0);
			}

			int logDate = g_session.m_chatProhibitLog[c].logDate;
			sprintf(msg, "ProhibitTime : %d min   LogTime : %d/%d/%d   %d:%d:%d",
				g_session.m_chatProhibitLog[c].prohibitTime,
				(logDate >> 16) & 0xffff, (logDate >> 8) & 0xff, logDate & 0xff,
				(g_session.m_chatProhibitLog[c].logTime >> 16) & 0xff, (g_session.m_chatProhibitLog[c].logTime >> 8) & 0xff, g_session.m_chatProhibitLog[c].logTime & 0xff);
			TextOutA(10, y + 65, msg, 0, 0, 12, 0);
			y += 18;
		}
	}
}

int UIProhibitListWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_AID = val1;
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_PROHIBITLISTWND);
			break;
		case ID_SKINSELECTCOMBO: {
			UIFrameWnd* comboBoxWnd = g_windowMgr.MakeWindow(WID_COMBOBOXWND);
			std::string feeding, perfomance, turnegg, accOff;
			feeding = MsgStr(MSI_PET_FEEDING);
			perfomance = MsgStr(MSI_PET_PERFORMANCE);
			turnegg = MsgStr(MSI_PET_RETURN_EGG);
			accOff = MsgStr(MSI_PET_ACC_OFF);
			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_APPEND_PROHIBIT), 0, 0, 0);
			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_ERASE_PROHIBIT), 0, 0, 0);
			comboBoxWnd->SendMsg(NULL, UM_ADDITEM, (int)MsgStr(MSI_REDUCE_PROHIBIT), 0, 0, 0);

			comboBoxWnd->SendMsg(NULL, UM_SETCURITEM, 0, 0, 0, 0);
			comboBoxWnd->Resize(130, 48);
			comboBoxWnd->SendMsg(NULL, UM_SET_CALLER_WID, WID_PROHIBITLISTWND, 0, 0, 0);

			int gx = 160;
			int gy = 44;
			GetGlobalCoor(gx, gy);
			comboBoxWnd->Move(gx, gy);
			break;
		}
		}
		break;

	case UM_COMBOBOX_SELECTED: {
		CGameMode* gameMode = g_modeMgr.GetGameMode(); RAG_BUDGET_SITES(5)
		switch (val1) {
		case 0:
			if (IsNameYellow(g_session.GetAid())) {
				UICntWnd* cntWnd = new UICntWnd;
				cntWnd->Create(182, 46);
				cntWnd->Move((g_renderer->m_width - 640) / 2 + 200, g_renderer->m_height * 200 / 480);
				g_windowMgr.AddWindow(cntWnd);
				cntWnd->SendMsg(NULL, UM_SET_TARGET, MM_REQ_GIVE_MANNER_POINT_MINUS, 0, 0, 0);
				cntWnd->SendMsg(NULL, UM_SETTEXT, (int)MsgStr(MSI_GIVE_MINUS_MANNER_POINT), 0, 0, 0);
				cntWnd->SendMsg(NULL, UM_SET_COUNT, 1, 0, 0, 0);
			}
			break;
		case 1:
			if (IsGravityAid(g_session.GetAid()))
				gameMode->SendMsg(MM_REQ_GIVE_MANNER_POINT_PLUS, 0x7530, 0, 0);
			break;
		case 2:
			if (IsGravityAid(g_session.GetAid())) {
				UICntWnd* cntWnd = new UICntWnd;
				cntWnd->Create(182, 46);
				cntWnd->Move((g_renderer->m_width - 640) / 2 + 200, g_renderer->m_height * 200 / 480);
				g_windowMgr.AddWindow(cntWnd);
				cntWnd->SendMsg(NULL, UM_SET_TARGET, MM_REQ_GIVE_MANNER_POINT_PLUS, 0, 0, 0);
				cntWnd->SendMsg(NULL, UM_SETTEXT, (int)MsgStr(MSI_GIVE_PLUS_MANNER_POINT), 0, 0, 0);
				cntWnd->SendMsg(NULL, UM_SET_COUNT, 1, 0, 0, 0);
			}
			break;
		}
		break;
	}
	}

	return 0;
}

UIBookWnd::UIBookWnd() {
	m_BookTotalLine = 0;
	m_BookTotalPage = 0;
	m_PresentPage = 1;
	m_BookCastTick = 0;
	m_PresentLine = -1;
	m_bAutoReadBook = 0;
	m_bookcastBalloon = NULL;
	strcpy(m_BookTitle, "");
}
UIBookWnd::~UIBookWnd() {}

void UIBookWnd::OnCreate(int cx, int cy)
{
	m_textViewer = new UITextViewer;
	m_textViewer->Create2(30, 45, cx - 60, cy - 60, false);
	AddChild(m_textViewer);

	std::string pathName = "book\\";

	const char* buttonName[4] = {
		"\xc3\xa5\xb0\xa5\xc7\xc7",
		"\xc3\xa5\xbf\xde\xc2\xca",
		"\xc3\xa5\xbf\xc0\xb8\xa5\xc2\xca",
		"\xc3\xa5\xb4\xdd\xb1\xe2",
	};
	int pos[4][2] = { { 10, 25 }, { cx - 57, 25 }, { cx - 35, 25 }, { cx - 25, cy - 25 } };
	int id[4] = {
		0x122,
		0x120,
		0x121,
		0x87,
	};
	const char* toolTip[4] = { MsgStr(MSI_BOOK_KEEP), MsgStr(MSI_BOOK_PREV), MsgStr(MSI_BOOK_NEXT), MsgStr(MSI_BOOK_CLOSE) };

	for (int i = 0; i < 4; ++i) {
		m_BookButton[i] = new UISpriteButton;
		m_BookButton[i]->SetSpriteName((pathName + buttonName[i] + ".spr").c_str());
		m_BookButton[i]->SetActFileName((pathName + buttonName[i] + ".act").c_str());
		m_BookButton[i]->Create(m_BookButton[i]->GetSprWidth(0, 0), m_BookButton[i]->GetSprHeight(0, 0));
		m_BookButton[i]->Move(pos[i][0], pos[i][1]);
		m_BookButton[i]->SetId(id[i]);
		m_BookButton[i]->SetToolTip(toolTip[i]);
		AddChild(m_BookButton[i]);
	}
}

void UIBookWnd::OnDraw()
{
	unsigned long rr;
	unsigned long gg;
	unsigned long bb;

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleBarMidCount = (m_w - 24) / 12 + (((m_w - 24) % 12) ? 1 : 0);
	for (int i = 0; i < titleBarMidCount; i++) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	char* titleText = new char[110];
	sprintf(titleText, "%s ( %d / %d )", m_BookTitle, m_PresentPage, m_BookTotalPage);
	TextOutA(m_w / 2 - 72, 2, titleText, 0, 0, 12, 0);

	g_skinMgr.GetColorChipColor(14, 6, rr, gg, bb);
	DrawBox(0, 17, m_w, m_h - 17, RGB(rr, gg, bb));
	DrawBox(1, 17, m_w - 2, m_h - 18, RGB(m_bgR, m_bgG, m_bgB));

	m_textViewer->m_bgR = m_bgR;
	m_textViewer->m_bgG = m_bgG;
	m_textViewer->m_bgB = m_bgB;
}

void UIBookWnd::OnProcess()
{
	if (!m_bAutoReadBook || !m_BookCastTick)
		return;
	if (timeGetTime() <= m_BookCastTick + 4000)
		return;
	if (m_PresentLine < m_BookTotalLine) {
		if ((strlen(m_bookContents[m_PresentLine].c_str()) == 0 ||
			strcmp(m_bookContents[m_PresentLine].c_str(), " ") == 0) &&
			m_PresentLine < m_BookTotalLine - 1)
			++m_PresentLine;

		g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
		g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)GetContents(m_bookContents[m_PresentLine].c_str()), 0xe1e4ff, 0, 0);

		if (!m_bookcastBalloon) {
			m_bookcastBalloon = new UITransBalloonText;
			m_bookcastBalloon->Create(8, 8);
			g_windowMgr.AddWindowFront(m_bookcastBalloon);
		}

		m_bookcastBalloon->SetText(GetContents(m_bookContents[m_PresentLine].c_str()), 0x64);
		m_bookcastBalloon->Move((g_renderer->m_width - 640) / 2 + 320 - m_bookcastBalloon->GetWidth() / 2, 81);
		m_bookcastBalloon->SetFntColor(0xe1e4ff, 0);

		++m_PresentLine;
		m_BookCastTick = timeGetTime();
		return;
	}

	m_BookCastTick = 0;
	if (m_bookcastBalloon) {
		g_windowMgr.PostQuit(m_bookcastBalloon);
		m_bookcastBalloon = NULL;
	}
	g_windowMgr.DeleteWindow(WID_BOOKWND);
}

int UIBookWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_LEFTPUSH:
		SendMsg(this, UM_COMMAND, ID_PREVIEW, 0, 0, 0);
		break;

	case UM_RIGHTPUSH:
		SendMsg(this, UM_COMMAND, ID_NEXTVIEW, 0, 0, 0);
		break;

	case UM_ENDPUSH:
		if (m_bookcastBalloon) {
			g_windowMgr.PostQuit(m_bookcastBalloon);
			m_bookcastBalloon = NULL;
		}
		g_windowMgr.DeleteWindow(WID_BOOKWND);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			SendMsg(this, UM_COMMAND, ID_BOOKMARK, 0, 0, 0);
			g_windowMgr.DeleteWindow(WID_BOOKWND);
			break;

		case ID_PREVIEW:
			if (!m_bAutoReadBook) {
				int page = m_PresentPage;
				if (page != 1)
					--page;
				SendMsg(this, UM_READBOOK, page, 0, 0, 0);
			}
			break;

		case ID_NEXTVIEW:
			if (!m_bAutoReadBook) {
				int page = m_PresentPage;
				if (page != m_BookTotalPage)
					++page;
				SendMsg(this, UM_READBOOK, page, 0, 0, 0);
			}
			break;

		case ID_BOOKMARK:
			WriteBookMarkToRegistry(m_BookTitle, m_PresentPage);
			break;
		}
		break;

	case UM_GETBOOKCONTENTS: {
		const char* bookTitle = ITEM_INFO::GetItemDesc((const char*)val1).identifiedDisplayName;

		if (strcmp(m_BookTitle, bookTitle) == 0) {
			if (!m_bAutoReadBook && val2 == 0xdd)
				break;
			val3 = m_PresentPage;
		}

		strcpy(m_BookTitle, bookTitle);

		char fName[260];
		if (g_serviceType == ServiceGermany)
			sprintf(fName, "%sbook\\%s.txt", g_language->m_languageFolder.c_str(), (const char*)val1);
		else
			sprintf(fName, "book\\%s.txt", (const char*)val1);

		{
			CFile fp;
			if (!fp.Open(g_language ? g_language->LanguageFile(fName) : fName, CFile::modeRead))
				break;

			unsigned long fileSize = fp.GetLength();
			char* contents = new char[fileSize + 1];
			fp.Read(contents, fileSize);
			contents[fileSize] = 0;
			fp.Close();

			m_bookContents.clear();
			PolishText4(contents, m_bookContents, m_textViewer->GetWidth() - 10);
			m_BookTotalLine = m_bookContents.size();

			if (contents[0] == '%') {
				m_bgR = hex_table[contents[1]] * 0x10 + hex_table[contents[2]];
				m_bgG = hex_table[contents[3]] * 0x10 + hex_table[contents[4]];
				m_bgB = hex_table[contents[5]] * 0x10 + hex_table[contents[6]];

				if (m_BookTotalLine > 0) {
					--m_BookTotalLine;
					m_bookContents.erase(m_bookContents.begin());
				}
			} else {
				g_skinMgr.GetColorChipColor(2, 2, m_bgR, m_bgG, m_bgB);
			}

			m_BookTotalPage = m_BookTotalLine / 19;
			if (m_BookTotalLine % 19 > 0)
				++m_BookTotalPage;
		}

		if (val2 == 0xdd)
			SendMsg(this, UM_READBOOK, val3, 0, 0, 0);
		else
			SendMsg(this, UM_AUTOREAD, val3, 0, 0, 0);
		break;
	}

	case UM_READBOOK: {
		m_bAutoReadBook = 0;
		SetShow(1);

		if (m_bookcastBalloon) {
			g_windowMgr.PostQuit(m_bookcastBalloon);
			m_bookcastBalloon = NULL;
		}

		if (val1 == 0)
			m_PresentPage = ReadBookMarkFromRegistry(m_BookTitle);
		else
			m_PresentPage = val1;

		if (m_PresentPage > m_BookTotalPage)
			m_PresentPage = 0;

		m_textViewer->ClearAllItems();
		for (int i = 0; i < 19; ++i) {
			int line = i + m_PresentPage * 19 - 19;
			if (line >= m_BookTotalLine)
				break;

			if (g_multiLang) {
				char* text = new char[m_bookContents[line].size() + 17];
				sprintf(text, "%s%s", g_language->GetLanguageCharset(true), m_bookContents[line].c_str());
				m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)text, 100, 0, 0);
			} else {
				m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)m_bookContents[line].c_str(), 100, 0, 0);
			}
		}

		Invalidate();
		g_windowMgr.MakeTopLayer(this);
		break;
	}

	case UM_AUTOREAD:
		m_bAutoReadBook = !m_bAutoReadBook;
		SetShow(0);

		if (m_bAutoReadBook) {
			m_BookCastTick = timeGetTime() - 4000;

			if (val1 == 0)
				m_PresentPage = ReadBookMarkFromRegistry(m_BookTitle);
			else
				m_PresentPage = val1;

			if (m_PresentPage > m_BookTotalPage)
				m_PresentPage = 0;

			m_PresentLine = m_PresentPage * 19 - 19;
			if (m_PresentLine > m_BookTotalLine)
				m_PresentLine = 0;
			break;
		}

		if (m_bookcastBalloon) {
			g_windowMgr.PostQuit(m_bookcastBalloon);
			m_bookcastBalloon = NULL;
		}
		g_windowMgr.DeleteWindow(WID_BOOKWND);
		break;

	default:
		break;
	}

	return 0;
}

char * UIBookWnd::GetContents(char const * text)
{
	const char* text_iter;
	int textLen = strlen(text);
	const char* text_end = text + textLen;
	const char* msg_charset = text + FindCharSet(text, text_end - text);
	unsigned long colorRef = 0;
	char* temp = new char[textLen + 1];
	char* contents = new char[textLen + 1];

	strcpy(contents, "");

	while (text < text_end) {
		text = InterpretColor(text, &colorRef);
		text_iter = FindColorMark(text, text_end);
		std::string str = std::string(text, text_iter);

		if (msg_charset != NULL && *msg_charset != NULL)
			str.insert(str.begin(), msg_charset, msg_charset + 3);

		if (text_iter != text) {
			strcpy(temp, str.c_str());
			temp[str.size()] = 0;
			strcat(contents, temp);
		}

		text = text_iter;
	}

	return contents;
}

void UIBookWnd::WriteBookMarkToRegistry(char const * bookName, int page)
{
	HKEY hKey;
	DWORD dwDisposition;
	char BookMarkPath[512];

	strcpy(BookMarkPath, g_regPath);
	strcat(BookMarkPath, g_bookmarkPath);
	strcat(BookMarkPath, "\\");
	strcat(BookMarkPath, g_session.m_lastServerName.c_str());
	strcat(BookMarkPath, "\\");
	strcat(BookMarkPath, g_session.GetCharName());

	dwDisposition = 0;
	RegDeleteKeyA(HKEY_LOCAL_MACHINE, BookMarkPath);
	LONG result = RegCreateKeyExA(HKEY_LOCAL_MACHINE, BookMarkPath, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (result == ERROR_SUCCESS) {
		RegSetValueExA(hKey, bookName, 0, REG_DWORD, (BYTE*)&page, 4);
		RegCloseKey(hKey);
	}
}

int UIBookWnd::ReadBookMarkFromRegistry(char const * bookName)
{
	int page;
	HKEY hKey;
	HKEY hResultKey;
	DWORD REG_TYPE;
	DWORD DATALENGTH;
	char BookMarkPath[512];

	strcpy(BookMarkPath, g_regPath);
	strcat(BookMarkPath, g_bookmarkPath);
	strcat(BookMarkPath, "\\");
	strcat(BookMarkPath, g_session.m_lastServerName.c_str());
	strcat(BookMarkPath, "\\");
	strcat(BookMarkPath, g_session.GetCharName());

	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, BookMarkPath, 0, KEY_QUERY_VALUE, &hResultKey);
	page = 1;
	if (result == ERROR_SUCCESS) {
		RegCloseKey(hResultKey);
		LONG result1 = RegOpenKeyExA(HKEY_LOCAL_MACHINE, BookMarkPath, 0, KEY_QUERY_VALUE, &hKey);
		if (result1 == ERROR_SUCCESS) {
			REG_TYPE = REG_DWORD;
			DATALENGTH = 4;
			if (RegQueryValueExA(hKey, bookName, NULL, &REG_TYPE, (BYTE*)&page, &DATALENGTH) != ERROR_SUCCESS)
				page = 1;
			RegCloseKey(hKey);
		}
	}
	return page;
}

void UIBookWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}
UIJoinFriendAcceptWnd::UIJoinFriendAcceptWnd() { m_info = NULL; }
UIJoinFriendAcceptWnd::~UIJoinFriendAcceptWnd() {}

int UIJoinFriendAcceptWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	char msg[128];

	switch (message) {
	case UM_SET_JOIN_FRIEND_MSG:
		m_nReqAID = val1;
		m_nReqGID = val2;
		sprintf(msg, MsgStr(MSI_SUGGEST_JOIN_FRIEND), (const char*)val3);
		m_textViewer->ClearAllItems();
		m_textViewer->SendMsg(NULL, UM_SETTEXT, (int)msg, 38, 0, 0);
		break;
	case UM_COMMAND:
		switch (val1) {
		case ID_OK: {
			g_windowMgr.DeleteWindow(WID_JOINFRIENDACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_FRIEND, m_nReqAID, m_nReqGID, 1);
			break;
		}
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_JOINFRIENDACCEPTWND);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_JOIN_FRIEND, m_nReqAID, m_nReqGID, 0);
			break;
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}
UICommandListWnd::UICommandListWnd() { m_curItem = -1; m_itemHorzNum = 2; }
UICommandListWnd::~UICommandListWnd() {}

void UICommandListWnd::OnCreate(int cx, int cy)
{
	m_tab = new UITab;
	m_tab->Create(20, 132);
	int numTab = 3;
	int typeTab = 1;
	m_tab->m_numTab = numTab;
	m_tab->m_typeTab = typeTab;
	m_tab->Move(0, 17);
	m_tab->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
	AddChild(m_tab);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, typeTab, m_h - 38);
	AddChild(m_scrollBar);

	int posX[2] = { 3, m_w - 14 };
	int ids[2] = { ID_BASE, ID_CLOSE };
	char* buttonName[2] = { "base", "close" };
	char* tooltip[2] = { "", "Alt+Y" };

	std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";

	int i;
	for (i = 0; i < 2; ++i) {
		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((preBmpName + buttonName[i] + "_off.bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((preBmpName + buttonName[i] + "_on.bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((preBmpName + buttonName[i] + "_on.bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(posX[i], 3);
		button->SetId(ids[i]);
		button->SetToolTip(tooltip[i]);
		AddChild(button);
	}

	m_itemVertNum = (cy - 38) / 20;
	RecalcScrbarPos();
}

void UICommandListWnd::OnDraw()
{
	m_w = m_w;

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int count = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int i = 0; i < count; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap((i + 1) * 12, 0, bmp, false);
	}

	unsigned long rr;
	unsigned long gg;
	unsigned long bb;
	g_skinMgr.GetColorChipColor(0xe, 6, rr, gg, bb);
	DrawBox(0, 17, m_w, m_h - 17, RGB(rr, gg, bb));

	g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
	DrawBox(1, 17, m_w - 2, m_h - 17, RGB(rr, gg, bb));

	count = (m_h - 42) / 8 + ((m_h - 42) % 8 != 0);
	for (i = 0; i < count; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
		DrawBitmap(0, i * 8 + 19, bmp, false);
	}

	count = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (i = 0; i < count; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, i * 8 + 17, bmp, false);
	}

	g_skinMgr.GetColorChipColor(0xe, 6, rr, gg, bb);
	DrawBox(18, 17, 1, m_h - 21, RGB(rr, gg, bb));

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - 21, bmp, false);

	count = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (i = 0; i < count; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp"), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	if (g_serviceType == ServiceBrazil)
		TextOutWithShadow(17, 2, "Lista de Comandos", 0, 0, 0xffffff, 0, 12);
	else
		TextOutWithShadow(17, 2, "Command List", 0, 0, 0xffffff, 0, 12);

	int drawCount = 0;
	int itemCount = 0;

	if (m_tab->GetState() == 2) {
		WinPair* it = g_session.m_cmdWinTable.begin();
		while (it != g_session.m_cmdWinTable.end()) {
			if (itemCount < m_viewOffset * m_itemHorzNum) {
				++it;
				++itemCount;
			} else {
				int itemX = (drawCount % m_itemHorzNum) * ((m_w - 40) / 2) + 25;
				int itemY = drawCount / m_itemHorzNum;
				itemY = itemY * 20 + 17;

				if (m_curItem == itemCount) {
					unsigned long rr;
					unsigned long gg;
					unsigned long bb;
					g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
					DrawBox(itemX, (drawCount / m_itemHorzNum) * 20 + 19,
							(m_w - 40) / 2, 20, RGB(rr, gg, bb));
					DrawBox(itemX + 1, (drawCount / m_itemHorzNum + 1) * 20,
							(m_w - 40) / 2 - 2, 18, 0xffffff);
				}

				TextOutA(itemX + 15, itemY + 5, it->first, 0, 0, 12, 0);

				++drawCount;
				if (drawCount == m_itemVertNum * m_itemHorzNum)
					break;
				++it;
				++itemCount;
			}
		}
	} else {
		std::vector<TTPair> m_cmdTempTable;

		if (m_tab->GetState() == 1)
			m_cmdTempTable = g_session.m_cmdExeTable;
		else
			m_cmdTempTable = g_session.m_cmdOnOffTable;

		TTPair* it = m_cmdTempTable.begin();
		while (it != m_cmdTempTable.end()) {
			if (itemCount < m_viewOffset * m_itemHorzNum) {
				++it;
				++itemCount;
			} else {
				int itemX = (drawCount % m_itemHorzNum) * ((m_w - 40) / 2) + 25;
				int itemY = drawCount / m_itemHorzNum;
				itemY = itemY * 20 + 17;

				if (m_curItem == itemCount) {
					unsigned long rr;
					unsigned long gg;
					unsigned long bb;
					g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
					DrawBox(itemX, (drawCount / m_itemHorzNum) * 20 + 19,
							(m_w - 40) / 2, 20, RGB(rr, gg, bb));
					DrawBox(itemX + 1, (drawCount / m_itemHorzNum + 1) * 20,
							(m_w - 40) / 2 - 2, 18, 0xffffff);
				}

				int textX = 25;
				if (m_tab->GetState() == 1)
					textX = 15;
				TextOutA(itemX + textX, itemY + 5, it->first, 0, 0, 12, 0);

				std::vector<TTPair2>::iterator stateIt = g_session.m_cmdOnOffSet.begin();
				while (stateIt != g_session.m_cmdOnOffSet.end()) {
					if (it->second == stateIt->second) {
						if (stateIt->first != 0)
							bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\grp_online.bmp"), false);
						else
							bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\grp_offline.bmp"), false);
						DrawBitmap(itemX + 2, itemY + 5, bmp, true);
						break;
					}
					++stateIt;
				}

				++drawCount;
				if (drawCount == m_itemVertNum * m_itemHorzNum)
					break;
				++it;
				++itemCount;
			}
		}
	}
}

void UICommandListWnd::OnLBtnDblClk(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}

void UICommandListWnd::OnLBtnDown(int x, int y)
{

	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
		return;
	}

	int oldCurItem = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (oldCurItem != m_curItem)
		Invalidate();

	if (m_curItem == -1)
		return;

	if (m_tab->GetState() == 2) {
		std::vector<WinPair>& commandList = g_session.m_cmdWinTable;
		g_windowMgr.ProcessPushButton(commandList[m_curItem].second, 0x2710, 0);
	} else {
		std::vector<TTPair> m_cmdTempTable;
		if (m_tab->GetState() != 0)
			m_cmdTempTable = g_session.m_cmdExeTable;
		else
			m_cmdTempTable = g_session.m_cmdOnOffTable;

		TTPair* it = m_cmdTempTable.begin() + m_curItem;
		if (it->second == TT_HI) {
			std::string param[3];
			g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, it->second, (int)param, 0);
		}
		else {
			g_modeMgr.GetCurMode()->SendMsg(MM_PROCESS_TALK_TYPE, it->second, 0, 0);
		}
	}

	Invalidate();
}

void UICommandListWnd::OnMouseHover(int x, int y)
{

	int oldCurItem = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (oldCurItem != m_curItem)
		Invalidate();

	char cmdHelp[128];
	strcpy(cmdHelp, "");

	if (m_tab->GetState() == 2) {
		const char** keyIt = g_session.m_keyWinTable.begin();
		if (m_curItem == -1)
			return;

		if (g_session.m_keyWinTable.size() <= m_curItem)
			return;

		g_modeMgr.GetCurMode()->SetCursorAction(2);
		strcpy(cmdHelp, keyIt[m_curItem]);
	} else {
		std::vector<TTPair> m_cmdTempTable;

		if (m_tab->GetState() != 0)
			m_cmdTempTable = g_session.m_cmdExeTable;
		else
			m_cmdTempTable = g_session.m_cmdOnOffTable;

		TTPair* it = m_cmdTempTable.begin();
		if (m_curItem == -1 || m_cmdTempTable.size() <= m_curItem)
			return;

		g_modeMgr.GetCurMode()->SetCursorAction(2);

		it += m_curItem;
		switch (it->second) {
		case TT_MUSIC_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_MUSIC));
			break;
		case TT_EFFECT_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_EFFECT));
			break;
		case TT_EFFECT_SOUND_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_SOUND));
			break;
		case TT_BATTLE_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_BATTLE));
			break;
		case TT_OPEN_TIP:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_OPENTIP));
			break;
		case TT_NOTRADE_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_NOTRADE));
			break;
		case TT_NOSHIFT_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_NOSHIFT));
			break;
		case TT_NOCTRL_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_NOCTRL));
			break;
		case TT_SKILLFAIL_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_SKILLFAIL));
			break;
		case TT_LOGINOUT_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_LOGINOUT));
			break;
		case TT_SHOPPING_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_SHOPPING));
			break;
		case TT_NOTALKMSG_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_NOTALKMSG));
			break;
		case TT_NOTALKMSG2_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_NOTALKMSG2));
			break;
		case TT_AURA_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_AURA));
			break;
		case TT_WINDOW_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_WINDOW));
			break;
		case TT_MISS_EFFECT_ON_OFF:
			if (g_session.m_bMakeMissEffect == 0)
				strcpy(cmdHelp, MsgStr(MSI_MISS_EFFECT_ON));
			else
				strcpy(cmdHelp, MsgStr(MSI_MISS_EFFECT_OFF));
			break;
		case TT_SET2_ON_OFF:
			if (g_serviceType == ServiceAmerica) {
				strcpy(cmdHelp, "/set2 : /set1 + /q3");
				break;
			}
		case TT_SET1_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_SET1));
			break;
		case TT_QUICKSPELL_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_QUICKSPELL));
			break;
		case TT_QUICKSPELL2_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_QUICKSPELL2));
			break;
		case TT_QUICKSPELL3_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_QUICKSPELL3));
			break;
		case TT_SHOW_NAME_TYPE:
			if (g_session.m_isShowNameTypeOld)
				strcpy(cmdHelp, MsgStr(MSI_SHOWNAME_ON));
			else
				strcpy(cmdHelp, MsgStr(MSI_SHOWNAME_OFF));
			break;
		case TT_FOG:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_FOG));
			break;
		case TT_HELP:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_HELP));
			break;
		case TT_REQ_NUM_USER:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_WHO));
			break;
		case TT_REQ_SIT:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_SIT));
			break;
		case TT_REQ_STAND:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_STAND));
			break;
		case TT_REMEMBER_WARPPOINT:
			strcpy(cmdHelp, MsgStr(MSI_WARPPOINTSTORED));
			break;
		case TT_REQ_EXIT_ROOM:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_EXIT_CHAT_ROOM));
			break;
		case TT_REQ_LEAVE_GROUP:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_LEAVE_PARTY));
			break;
		case TT_MAP_POS:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_POS));
			break;
		case TT_MAKE_CHATROOMMAKEWND:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_MAKE_CHAT));
			break;
		case TT_REQ_SAVE_CHAT:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_SAVE_CHAT));
			break;
		case TT_DORIDORI_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_DORIDORI));
			break;
		case TT_BINGBING_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_BINGBING));
			break;
		case TT_BANGBANG_ON_OFF:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_BANGBANG));
			break;
		case TT_REQ_WHISPER_LIST:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_WHISPER_LIST_WND));
			break;
		case TT_REQ_WHISPER_STATE_IN:
			strcpy(cmdHelp, MsgStr(MSI_HEAR_ALL));
			break;
		case TT_REQ_WHISPER_STATE_EX:
			strcpy(cmdHelp, MsgStr(MSI_REFUSE_HEAR_ALL));
			break;
		case TT_SHOW_PVP_INFO:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_PVP_INFO));
			break;
		case TT_HI:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_HI));
			break;
		case TT_REQ_BLACKSMITH:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_BLACKSMITH));
			break;
		case TT_REQ_ALCHEMIST:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_ALCHEMIST));
			break;
		case TT_REQ_KILLER_RANK:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_KILLER_RANK));
			break;
		case TT_REQ_HUNTING:
			strcpy(cmdHelp, MsgStr(MSI_EXPLAIN_HUNTING));
			break;
		}
	}

	if (strcmp(cmdHelp, "") != 0) {
		int gx = ((x - 40) / 20 + 2) * 20;
		int gy = ((y - 17) / 20) * 20 + 1;
		GetGlobalCoor(gx, gy);
		g_modeMgr.GetCurMode()->MakeHelpBalloon(cmdHelp, gx, gy, 0xffffff);
	}
}

void UICommandListWnd::OnWheel(int wheel) {
	if (m_vertScrEnabled) {
		int n = m_itemVertNum - 1;
		if (n >= 3) n = 3;
		if (wheel < 0)
			SendMsg(m_scrollBar, UM_SCROLLVERT, n, 0, 0, 0);
		else
			SendMsg(m_scrollBar, UM_SCROLLVERT, -n, 0, 0, 0);
	}
}


int UICommandListWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		if (val1 == ID_CLOSE)
			g_windowMgr.DeleteWindow(WID_COMMANDLISTWND);
		break;
	case UM_TAB:
		Invalidate();
		break;
	case UM_SCROLLVERT:
	case UM_SCROLLVERTPAGEUP:
	case UM_SCROLLVERTPAGEDOWN: {
		int oldViewOffset = m_viewOffset;
		if (message == UM_SCROLLVERTPAGEUP)
			m_viewOffset = oldViewOffset - m_itemVertNum + 1;
		else if (message == UM_SCROLLVERTPAGEDOWN)
			m_viewOffset = oldViewOffset + m_itemVertNum - 1;
		else
			m_viewOffset = oldViewOffset + val1;

		if (oldViewOffset != m_viewOffset) {
			RecalcScrbarPos();
			Invalidate();
		}
		return m_viewOffset - oldViewOffset;
	}
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

int UICommandListWnd::GetCurItem(int x, int y)
{

	if (x < 0 || y < 17 || y >= m_itemVertNum * 20 + 17)
		return -1;
	int xCell = x / (m_w / m_itemHorzNum);
	int yCell = (y - 17) / 20;


	int i = 0;
	int j = 0;
	std::vector<std::pair<const char*, TALKTYPE> >::iterator it = g_session.m_cmdOnOffTable.begin();
	while (it != g_session.m_cmdOnOffTable.end()) {
		if (i < m_viewOffset * m_itemHorzNum) {
			++it;
			++i;
			continue;
		}
		if (j == m_itemHorzNum * yCell + xCell)
			return i;

		++it;
		++j;
		++i;
	}

	return -1;
}

void UICommandListWnd::RecalcScrbarPos()
{
	if (m_scrollBar) {
		int numCanDisplayItem = m_itemVertNum;
		std::vector<TTPair>& commandList = g_session.m_cmdOnOffTable;
		int numLine = commandList.size() / m_itemHorzNum + ((commandList.size() % m_itemHorzNum) != 0);
		int range = numLine - numCanDisplayItem;

		if (range < 0)
			range = 0;
		if (range != 0)
			m_vertScrEnabled = 1;
		else
			m_vertScrEnabled = 0;

		if (m_vertScrEnabled) {
			m_scrollBar->SetRange(range, numCanDisplayItem, numLine);
			if (m_viewOffset < 0)
				m_viewOffset = 0;
			if (m_viewOffset > numLine - m_itemVertNum)
				m_viewOffset = numLine - m_itemVertNum;
			m_scrollBar->SetPos(m_viewOffset);
		} else {
			m_viewOffset = 0;
			m_scrollBar->SetRange(0, numCanDisplayItem, numLine);
		}
		m_scrollBar->Invalidate();
	}
}
UIAutoMessageWnd::UIAutoMessageWnd() {}
UIAutoMessageWnd::~UIAutoMessageWnd() {}

void UIAutoMessageWnd::OnCreate(int cx, int cy)
{
	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(m_w - 77, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(100);
	m_editCtrl->SetFrameColor(232, 232, 232);
	m_editCtrl->SetText(g_session.m_strAutoMSG.c_str());
	m_editCtrl->MakeSelect();
	AddChild(m_editCtrl);

	{
		std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
		int pos[1][2] = {
			{ m_w - 55, m_h - 32 },
		};

		UIBitmapButton* button = new UIBitmapButton;
		button->SetBitmapName((pathName + "btn_ok" + ".bmp").c_str(), BS_NORMAL);
		button->SetBitmapName((pathName + "btn_ok_a" + ".bmp").c_str(), BS_MOUSEON);
		button->SetBitmapName((pathName + "btn_ok_b" + ".bmp").c_str(), BS_PRESSED);
		button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
		button->Move(pos[0][0], pos[0][1]);
		button->SetId(ID_OK);
		AddChild(button);
	}
	m_defPushId = ID_OK;
}

void UIAutoMessageWnd::OnDraw() {
	DrawLatticeFrame(0, 0, 0, 0);
	TextOutA(15, 6, MsgStr(MSI_ENTER_AUTO_MSG), 0, 0, 12, 0);
	g_windowMgr.SetFocusEdit(m_editCtrl);
}

int UIAutoMessageWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		if (val1 == ID_OK) {
			g_session.m_strAutoMSG.assign(m_editCtrl->GetText());
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_AUTO_MSG_ON), 0x64ffff, 0, 0);
			g_windowMgr.FindFocusEditAndSetFocus();
			g_windowMgr.DeleteWindow(WID_AUTOMESSAGEWND);
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}







inline void rag_symbol_shift(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17, int a18, int a19, int a20, int a21, int a22, int a23, int a24, int a25, int a26, int a27) { a0 = 0; a1 = 0; a2 = 0; a3 = 0; a4 = 0; a5 = 0; a6 = 0; a7 = 0; a8 = 0; a9 = 0; a10 = 0; a11 = 0; a12 = 0; a13 = 0; a14 = 0; a15 = 0; a16 = 0; a17 = 0; a18 = 0; a19 = 0; a20 = 0; a21 = 0; a22 = 0; a23 = 0; a24 = 0; a25 = 0; a26 = 0; a27 = 0; }



UIMailListWnd::UIMailListWnd()
{
	m_type = 0;
	m_type2 = 0;
	m_page = 1;
	m_maxPage = 0;
	m_inputZeny = 0;
}
UIMailListWnd::~UIMailListWnd() {}

void UIMailListWnd::OnCreate(int cx, int cy)
{
	{
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\close2";
		char toolTip[] = "Close";
		UIBitmapButton* closeButton = new UIBitmapButton;
		closeButton->SetBitmapName((preBmpName + ".bmp").c_str(), 0);
		closeButton->SetBitmapName((preBmpName + "_a.bmp").c_str(), 1);
		closeButton->SetBitmapName((preBmpName + "_a.bmp").c_str(), 2);
		closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
		closeButton->Move(288, 4);
		closeButton->SetId(ID_CLOSE);
		closeButton->SetToolTip(toolTip);
		AddChild(closeButton);
	}

	for (int i = 0; i < 7; ++i) {
		m_btnFromNameList[i] = new UITextButton;
		m_btnFromNameList[i]->Create("", 0x7687af, 0, 12, false);
		m_btnFromNameList[i]->SetSelectColor(0x9a9aff);
		m_btnFromNameList[i]->Move(-100, -100);
		m_btnFromNameList[i]->SetId(ID_FROM_NAME);
		AddChild(m_btnFromNameList[i]);

		m_btnMailHeaderList[i] = new UITextButton;
		m_btnMailHeaderList[i]->Create("", 0x545d81, 0, 12, false);
		m_btnMailHeaderList[i]->SetSelectColor(0x9a9aff);
		m_btnMailHeaderList[i]->Move(-100, -100);
		m_btnMailHeaderList[i]->SetId(ID_HEADER);
		AddChild(m_btnMailHeaderList[i]);
	}

	m_prevBtn = new UITextButton;
	m_prevBtn->Create(MsgStr(MSI_PREV_PAGE), 0x9fb3e1, 0, 12, false);
	m_prevBtn->SetSelectColor(0x9a9aff);
	m_prevBtn->Move(-100, -100);
	m_prevBtn->SetToolTip(MsgStr(MSI_PREV_PAGE));
	m_prevBtn->SetId(ID_PREV_PAGE);
	AddChild(m_prevBtn);

	m_nextBtn = new UITextButton;
	m_nextBtn->Create(MsgStr(MSI_NEXT_PAGE), 0x9fb3e1, 0, 12, false);
	m_nextBtn->SetSelectColor(0x9a9aff);
	m_nextBtn->Move(-100, -100);
	m_nextBtn->SetToolTip(MsgStr(MSI_NEXT_PAGE));
	m_nextBtn->SetId(ID_NEXT_PAGE);
	AddChild(m_nextBtn);

	RAG_BUDGET_FILL(16) rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	m_receiveName = new UIEditCtrl2;
	m_receiveName->Create(110, 16);
	m_receiveName->Move(-100, -100);
	m_receiveName->SetTextLimit(50);
	m_receiveName->SetFrameColor(0xff, 0xfd, 0xf6);
	m_receiveName->SetTextColor(0x81, 0x5d, 0x54);
	AddChild(m_receiveName);

	m_MailHeader = new UIEditCtrl2;
	m_MailHeader->Create(170, 16);
	m_MailHeader->Move(-100, -100);
	m_MailHeader->SetTextLimit(50);
	m_MailHeader->SetFrameColor(0xff, 0xfd, 0xf6);
	m_MailHeader->SetTextColor(0x81, 0x5d, 0x54);
	AddChild(m_MailHeader);

	m_MailContent = new UIEditCtrl3;
	m_MailContent->Create(238, 216);
	m_MailContent->SetType(1);
	m_MailContent->Move(-300, -300);
	m_MailContent->SetTextLimit(198);
	m_MailContent->SetFrameColor(0xfd, 0xed, 0xe1);
	m_MailContent->SetTextColor(0x81, 0x5d, 0x54);
	AddChild(m_MailContent);
	g_windowMgr.SetFocusEdit(m_MailContent);

	m_Zeny = new UIEditCtrl2;
	m_Zeny->Create(65, 16);
	m_Zeny->SetType(1);
	m_Zeny->Move(-100, -100);
	m_Zeny->SetTextLimit(9);
	m_Zeny->SetFrameColor(0xff, 0xfd, 0xf6);
	m_Zeny->SetTextColor(0x81, 0x5d, 0x54);
	AddChild(m_Zeny);

	m_Zeny2 = new UIStaticText;
	m_Zeny2->Create(65, 16);
	m_Zeny2->Move(-100, -100);
	m_Zeny2->SetTextColor(0x81, 0x5d, 0x54);
	AddChild(m_Zeny2);

		{
			std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\inputzeny";
			m_inputZenyBtn = new UIBitmapButton;
			m_inputZenyBtn->SetBitmapName((pathName + ".bmp").c_str(), 0);
			m_inputZenyBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
			m_inputZenyBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 2);
		m_inputZenyBtn->Create(m_inputZenyBtn->GetBitmapWidth(), m_inputZenyBtn->GetBitmapHeight());
		m_inputZenyBtn->Move(-100, -100);
		m_inputZenyBtn->SetId(ID_INPUT_ZENY);
		AddChild(m_inputZenyBtn);
	}

		{
			std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\setzeny";
			m_setZenyBtn = new UIBitmapButton;
			m_setZenyBtn->SetBitmapName((pathName + ".bmp").c_str(), 0);
			m_setZenyBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
			m_setZenyBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 2);
		m_setZenyBtn->Create(m_setZenyBtn->GetBitmapWidth(), m_setZenyBtn->GetBitmapHeight());
		m_setZenyBtn->Move(-100, -100);
		m_setZenyBtn->SetId(ID_SET_ZENY);
		AddChild(m_setZenyBtn);
	}

		{
			std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\send";
			m_sendBtn = new UIBitmapButton;
			m_sendBtn->SetBitmapName((pathName + ".bmp").c_str(), 0);
			m_sendBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
			m_sendBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 2);
		m_sendBtn->Create(m_sendBtn->GetBitmapWidth(), m_sendBtn->GetBitmapHeight());
		m_sendBtn->Move(-100, -100);
		m_sendBtn->SetId(ID_SEND_MAIL);
		AddChild(m_sendBtn);
	}

		{
			std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\cancel2";
			m_cancelBtn = new UIBitmapButton;
			m_cancelBtn->SetBitmapName((pathName + ".bmp").c_str(), 0);
			m_cancelBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 1);
			m_cancelBtn->SetBitmapName((pathName + "_a.bmp").c_str(), 2);
		m_cancelBtn->Create(m_cancelBtn->GetBitmapWidth(), m_cancelBtn->GetBitmapHeight());
		m_cancelBtn->Move(-100, -100);
		m_cancelBtn->SetId(ID_CANCEL_MAIL);
		AddChild(m_cancelBtn);
	}

	SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}

void UIMailListWnd::OnDraw()
{
	char windowName[32];

	CBitmapRes* bitmap;
	if (m_type == 0) {
		bitmap = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\maillist1_bg.bmp"), false);
		sprintf(windowName, MsgStr(MSI_MAIL_LIST));
	} else {
		bitmap = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\maillist2_bg.bmp"), false);
		sprintf(windowName, MsgStr(MSI_MAIL_WRITE));
	}

	DrawBitmap(0, 0, bitmap, false);
	TextOutA(26, 4, windowName, 0, 0, 12, 0xffffff);
	TextOutA(25, 3, windowName, 0, 0, 12, 0);

	std::vector<MAIL_LIST>::iterator it;
	if (m_type == 0 && !g_session.m_mailList.empty()) {
		char buf[10];
				it = g_session.m_mailList.begin() + (m_page - 1) * 7;
				m_maxPage = g_session.m_mailList.size() / 7;
				if (g_session.m_mailList.size() % 7 > 0)
					++m_maxPage;
				if (m_page > m_maxPage)
					m_page = m_maxPage;

				for (int i = 0; i < 7 && it != g_session.m_mailList.end(); ++i, ++it) {
					char tmpbuf[128];
					int y = i * 50 + 32;
					if (!it->isOpen) {
						CBitmapRes* envelop = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\envelop.bmp"), false);
						DrawBitmap(36, y + 7, envelop, true);
					}

					m_btnFromNameList[i]->SetText(std::string(it->FromName));
					m_btnFromNameList[i]->Resize2(120, m_btnFromNameList[i]->GetHeight());
					m_btnFromNameList[i]->SetToolTip(it->FromName);
					m_btnFromNameList[i]->Move(70, y);
					m_btnFromNameList[i]->SetId2((int)it->FromName);

					m_btnMailHeaderList[i]->SetText(std::string(it->HEADER));
					m_btnMailHeaderList[i]->Resize2(190, m_btnMailHeaderList[i]->GetHeight());
					m_btnMailHeaderList[i]->SetToolTip(it->HEADER);
					m_btnMailHeaderList[i]->Move(70, y + 17);
					m_btnMailHeaderList[i]->SetId2(it->MailID);

					tm* deleteTime = localtime(&it->DeleteTime);
					strftime(tmpbuf, 128, MsgStr(MSI_TIME_TYPE), deleteTime);
					TextOutA(200, y + 3, tmpbuf, 0, 0, 11, 0x7777ff);
				}

				sprintf(buf, "%d / %d", m_page, m_maxPage);
				TextOutA(148, 383, buf, 0, 0, 12, 0x9fb3e1);
	} else if (m_type == 1) {
		char buf[10];
		sprintf(buf, "%s:", MsgStr(MSI_SENDER));
		TextOutA(47, 37, buf, 0, 0, 12, 0x9fb3e1);
		sprintf(buf, "%s:", MsgStr(MSI_TITLE2));
		TextOutA(55, 68, buf, 0, 0, 12, 0x9fb3e1);
		if (m_item.m_itemIndex)
			DrawItemWithCount(249, 333, m_item, false, false);
	}
}


void UIMailListWnd::OnLBtnDown(int x, int y)
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();

	if (x < 20) {
		if (y >= 17 && y < 77) {
			if (m_type && !m_type2) {
				m_type = 0;
				ResetMailWrite(true);
				gameMode->SendMsg(MM_GET_MAIL_LIST, 0, 0, 0);
			}
		} else if (y >= 77 && y < 137) {
			if (m_type != 1) {
				gameMode->SendMsg(MM_RESET_MAIL_ITEM, 0, 0, 0);
				m_type = 1;
				ResetMailWrite(true);
			}
		}
	}

	if (m_type) {
		if (x > 249 && x < 273 && y > 333 && y < 357) {
			if (m_item.m_itemIndex) {
				CMode* curMode = g_modeMgr.GetCurMode();
				g_session.m_mailItem = m_item;
				curMode->SendMsg(MM_BEGIN_DRAG_FROM_MAILWND, 0, 0, 0);
			}
		}
	}

	SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}

void UIMailListWnd::OnRBtnDown(int x, int y)
{
	if (m_type && m_item.m_itemIndex) {
		if (x > 249 && x < 273 && y > 333 && y < 357) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&m_item, 0, 0, 0);
		}
	}
}

void UIMailListWnd::OnMouseHover(int x, int y)
{
	if (m_type && m_item.m_itemIndex) {
		if (x > 249 && x < 273 && y > 333 && y < 357) {
			char itemName[256];
			char itemHelp[128];
			m_item.GetItemName(itemName);
			sprintf(itemHelp, MsgStr(MSI_EA2), itemName, m_item.m_num);

			int gx = 249;
			int gy = 333;
			GetGlobalCoor(gx, gy);
			g_modeMgr.GetCurMode()->MakeHelpBalloon(itemHelp, gx, gy, -1);
		}
	}

	UIFrameWnd::OnMouseHover(x, y);
}

void UIMailListWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	CGameMode* gm = g_modeMgr.GetGameMode();
	if (dragInfo->m_dragType == DT_FROM_ITEMWND) {
		if (dragInfo->m_numDragItem == 1) {
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		}

		g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
		UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
		wnd->SendMsg(0, UM_SETSTATE, 0xd, 0, 0, 0);
		return;
	}

	gm->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}

int UIMailListWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();

	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			gameMode->SendMsg(MM_RESET_MAIL_ITEM, 0, 0, 0);
			g_windowMgr.DeleteWindow(WID_MAILLISTWND);
			break;

		case ID_INPUT_ZENY:
			if (m_Zeny->GetText()) {
				CGameMode* lpMode = g_modeMgr.GetGameMode();
				lpMode->SendMsg(MM_RESET_MAIL_ITEM, 2, 0, 0);
			}
			g_windowMgr.SetFocusEdit(m_Zeny);
			m_Zeny->SetText(m_Zeny2->GetFullText());
			m_inputZeny = 1;
			SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			break;

		case ID_SET_ZENY: {
			int zenyValue = atoi(m_Zeny->GetText());
			if (g_session.GetGold() < zenyValue)
				zenyValue = g_session.GetGold();

			char zeny[64];
			GetNumberText(zenyValue, zeny);
			m_Zeny2->SetText(zeny, 0);
			m_Zeny->SetText(zeny);
			m_inputZeny = 0;
			if (zenyValue > 0)
				g_modeMgr.GetGameMode()->SendMsg(MM_ADD_MAIL_ITEM, 0, zenyValue, 0);
			SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			break;
		}

		case ID_SEND_MAIL: {
			if (strcmp(m_MailHeader->GetText(), "") != 0) {
				int numBadSentenceLine = 0;
				m_MailHeader->SetTextColor(0x81, 0x5d, 0x54);
				if (!g_insultFilter.IsGoodSentence(m_MailHeader->GetText())) {
					m_MailHeader->SetTextColor(0xff, 0, 0);
					numBadSentenceLine = 1;
				}

				if (m_Zeny->GetText() && m_inputZeny) {
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_MAIL_SEND_ERROR2), 0xff, 0, 0);
					break;
				}

				int txtLen = strlen(m_MailContent->GetText()) + 2;
				if (!g_insultFilter.IsGoodSentence(m_MailContent->GetText())) {
					m_MailContent->SetTextColor(0xff, 0, 0);
					++numBadSentenceLine;
				}

				char* Buf = new char[txtLen];
				char tmp[2];
				tmp[0] = (char)strlen(m_MailContent->GetText());
				tmp[1] = 0;
				strcpy(Buf, tmp);
				strcat(Buf, m_MailContent->GetText());
				Buf[txtLen - 1] = 0;

				if (numBadSentenceLine > 0) {
					g_windowMgr.SendMsg(
						UIM_PUSHINTOCHATHISTORY,
						(int)"Can't send mail: inappropriate word detected.",
						0xff, 0, 0);
					break;
				}

				m_sendBtn->SetDiabled(1);
				if (g_session.m_waitForSendMail)
					break;

				gameMode = g_modeMgr.GetGameMode();
				gameMode->SendMsg(MM_SEND_MAIL, (int)m_receiveName->GetText(), (int)m_MailHeader->GetText(), (int)Buf);
				g_session.m_waitForSendMail = 1;
			} else {
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_MAIL_SEND_ERROR), 0xff, 0, 0);
				g_windowMgr.SetFocusEdit(m_MailHeader);
			}
			break;
		}

		case ID_CANCEL_MAIL:
			if (!m_type2) {
				gameMode->SendMsg(MM_RESET_MAIL_ITEM, 0, 0, 0);
				m_type = 0;
				SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			} else {
				gameMode->SendMsg(MM_RESET_MAIL_ITEM, 0, 0, 0);
				g_windowMgr.DeleteWindow(WID_MAILLISTWND);
			}
			break;
		}
		break;

	case UM_DROP: {
		if (m_type != 1)
			break;

		DRAG_INFO *dragInfo = gameMode->GetDragInfo();

		rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); rag_symbol_shift(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		RAG_BUDGET_FILL(102)
		if (m_item.m_itemIndex)
			gameMode->SendMsg(MM_RESET_MAIL_ITEM, 1, 0, 0);

		m_item.m_itemType = dragInfo->m_dragItemType;
		m_item.m_itemIndex = gameMode->GetDragItemIndex();
		m_item.m_isIdentified = dragInfo->m_isIdentified;
		m_item.m_num = gameMode->GetNumDragItem();
		m_item.m_itemName.assign(dragInfo->m_dragItemName);
		memcpy(m_item.m_slot, dragInfo->m_slot, sizeof(m_item.m_slot));
		m_item.m_refiningLevel = dragInfo->m_refiningLevel;

		SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		gameMode->SendMsg(MM_ADD_MAIL_ITEM, gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	case UM_TEXT_LBTN_UP:
		switch (val1) {
		case ID_FROM_NAME:
			SendMsg(NULL, UM_NEWMAIL, val2, 0, 0, 0);
			break;

		case ID_HEADER:
			gameMode->SendMsg(MM_OPEN_MAIL, val2, 0, 0);
			break;

		case ID_PREV_PAGE:
			if (m_page > 1)
				--m_page;
			SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			break;

		case ID_NEXT_PAGE:
			if (m_page < m_maxPage)
				++m_page;
			SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
			break;
		}
		break;

	case UM_BMP_RBTN_UP:
		if (m_type == 1 && m_item.m_itemIndex) {
			UIWindow* wnd = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			wnd->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&m_item, 0, 0, 0);
		}
		break;

	case UM_NEWMAIL:
		m_type = 1;
		ResetMailWrite(true);
		if (val1)
			m_receiveName->SetText((char*)val1);
		gameMode->SendMsg(MM_RESET_MAIL_ITEM, 0, 0, 0);
		SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		break;

	case UM_DECITEM:
		if (m_item.m_itemIndex) {
			gameMode->SendMsg(MM_RESET_MAIL_ITEM, 1, 0, 0);
			m_item = ITEM_INFO();
			SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
		}
		break;

	case UM_REFRESH:
		InitAllBtnPos();
		Invalidate();
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIMailListWnd::InitAllBtnPos()
{
	int i;
	for (i = 0; i < 7; ++i)
	{
		m_btnFromNameList[i]->Move(-100, -100);
		m_btnMailHeaderList[i]->Move(-100, -100);
	}
	m_MailContent->Move(-300, -300);
	m_prevBtn->Move(-100, -100);
	m_nextBtn->Move(-100, -100);
	m_receiveName->Move(-100, -100);
	m_MailHeader->Move(-100, -100);
	m_Zeny->Move(-100, -100);
	m_Zeny2->Move(-100, -100);
	m_inputZenyBtn->Move(-100, -100);
	m_setZenyBtn->Move(-100, -100);
	m_cancelBtn->Move(-100, -100);
	m_sendBtn->Move(-100, -100);

	if (m_type == 0)
	{
		m_prevBtn->Move(110, 382);
		m_nextBtn->Move(189, 382);
		m_prevBtn->SetDiabled(0);
		m_nextBtn->SetDiabled(0);
		if (m_page == 1)
			m_prevBtn->SetDiabled(1);
		if (m_page == m_maxPage || m_maxPage == 0)
			m_nextBtn->SetDiabled(1);
		return;
	}

	m_MailContent->Move(41, 100);
	m_receiveName->Move(90, 34);
	m_MailHeader->Move(90, 65);
	m_sendBtn->Move(183, 374);
	m_cancelBtn->Move(231, 374);
	if (m_inputZeny != 0)
	{
		m_Zeny->Move(135, 341);
		m_setZenyBtn->Move(88, 338);
		return;
	}
	m_Zeny2->Move(135, 343);
	m_inputZenyBtn->Move(80, 338);
}

void UIMailListWnd::ResetMailWrite(bool allInit)
{
	m_sendBtn->SetDiabled(0);

	m_item = ITEM_INFO();
	m_inputZeny = 0;

	m_receiveName->SetText("");
	m_receiveName->Invalidate();
	m_Zeny->SetText("");
	m_Zeny->Invalidate();
	m_Zeny2->SetText("", 0);
	m_Zeny2->Invalidate();

	if (allInit) {
		m_MailHeader->SetText("");
		m_MailHeader->Invalidate();
		m_MailContent->SetText("");
		m_MailContent->Invalidate();
		m_page = 1;
	} else {
		UIEditCtrl* receiveName = m_receiveName;
		g_windowMgr.SetFocusEdit(receiveName);
	}

	SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}

UIMailViewWnd::UIMailViewWnd()
{
	m_zeny = 0;
	r = 0xff; g = 0xfd; b = 0xf6;
}
UIMailViewWnd::~UIMailViewWnd() {}

void UIMailViewWnd::OnCreate(int cx, int cy)
{
	{
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\close2";
		char toolTip[] = "Close";
		UIBitmapButton* closeButton = new UIBitmapButton;
		closeButton->SetBitmapName((preBmpName + ".bmp").c_str(), BS_NORMAL);
		closeButton->SetBitmapName((preBmpName + "_a.bmp").c_str(), BS_MOUSEON);
		closeButton->SetBitmapName((preBmpName + "_a.bmp").c_str(), BS_PRESSED);
		closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
		closeButton->Move(288, 4);
		closeButton->SetId(ID_CLOSE);
		closeButton->SetToolTip(toolTip);
		AddChild(closeButton);
	}

	m_MailContent = new UIChatHisBox3;
	m_MailContent->Create2(21, 100, 258, 216, false);
	m_MailContent->SetItemSpacing(18);
	m_MailContent->SetBgColor(0xfd, 0xed, 0xe1);
	AddChild(m_MailContent);

	{
		RAG_BUDGET_FILL(10)
		std::string pathName ="\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\return";
		m_returnMailBtn = new UIBitmapButton;
		m_returnMailBtn->SetBitmapName((pathName + ".bmp").c_str(), BS_NORMAL);
		m_returnMailBtn->SetBitmapName((pathName + "_a.bmp").c_str(), BS_MOUSEON);
		m_returnMailBtn->SetBitmapName((pathName + "_a.bmp").c_str(), BS_PRESSED);
		m_returnMailBtn->Create(m_returnMailBtn->GetBitmapWidth(), m_returnMailBtn->GetBitmapHeight());
		m_returnMailBtn->Move(135, 374);
		m_returnMailBtn->SetId(ID_RETURN_MAIL);
		AddChild(m_returnMailBtn);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\remail";
		m_reMailBtn = new UIBitmapButton;
		m_reMailBtn->SetBitmapName((pathName + ".bmp").c_str(), BS_NORMAL);
		m_reMailBtn->SetBitmapName((pathName + "_a.bmp").c_str(), BS_MOUSEON);
		m_reMailBtn->SetBitmapName((pathName + "_a.bmp").c_str(), BS_PRESSED);
		m_reMailBtn->Create(m_reMailBtn->GetBitmapWidth(), m_reMailBtn->GetBitmapHeight());
		m_reMailBtn->Move(183, 374);
		m_reMailBtn->SetId(ID_SEND_MAIL);
		AddChild(m_reMailBtn);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\del";
		m_delBtn = new UIBitmapButton;
		m_delBtn->SetBitmapName((pathName + ".bmp").c_str(), BS_NORMAL);
		m_delBtn->SetBitmapName((pathName + "_a.bmp").c_str(), BS_MOUSEON);
		m_delBtn->SetBitmapName((pathName + "_a.bmp").c_str(), BS_PRESSED);
		m_delBtn->Create(m_delBtn->GetBitmapWidth(), m_delBtn->GetBitmapHeight());
		m_delBtn->Move(231, 374);
		m_delBtn->SetId(ID_DELETE_MAIL);
		AddChild(m_delBtn);
	}
}



void UIMailViewWnd::OnMouseShape(int x, int y)
{
	if (x > 144 && x < 279 && y > 327 && y < 363) {
		g_modeMgr.GetCurMode()->SetCursorAction(2);
	} else {
		g_modeMgr.GetCurMode()->SetCursorAction(0);
	}
}

void UIMailViewWnd::OnDraw()
{
	char windowName[32];
	char zeny[64];

	CBitmapRes* bitmap = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\maillist3_bg.bmp"), false);
	DrawBitmap(0, 0, bitmap, false);

	sprintf(windowName, MsgStr(MSI_MAIL_READ));
	TextOutA(26, 4, windowName, 0, 0, 12, 0xffffff);
	TextOutA(25, 3, windowName, 0, 0, 12, 0);

	int titleX = (m_w - GetTextWidth(g_session.m_mailOpen.HEADER, 0, 0, 12, false)) / 2;
	TextOutA(titleX, 40, g_session.m_mailOpen.HEADER, 0, 0, 12, 0x7687af);
	TextOutA(titleX + 1, 40, g_session.m_mailOpen.HEADER, 0, 0, 12, 0x7687af);

	TextOutWithRightAlign(280, 72, g_session.m_mailOpen.FromName,
		strlen(g_session.m_mailOpen.FromName), 0, 12, 0x9fb3e1);

	DrawBox(129, 327, 150, 2, RGB(r, g, b));
	DrawBox(129, 361, 150, 2, RGB(r, g, b));
	DrawBox(129, 327, 2, 36, RGB(r, g, b));
	DrawBox(277, 327, 2, 36, RGB(r, g, b));

	if (g_session.m_mailOpen.Money > 0) {
		GetNumberText(m_zeny, zeny);
		TextOutWithRightAlign(195, 345, zeny, strlen(zeny), 0, 12, 0x545d81);
	}

	if (m_item.m_itemIndex) {
		DrawItemWithCount(249, 333, m_item, false, false);
	}
}

void UIMailViewWnd::OnMouseHover(int x, int y)
{
	if (x > 144 && x < 279 && y > 327 && y < 363 &&
		(m_item.m_itemIndex || g_session.m_mailOpen.Money > 0)) {
		r = 0xff; g = 0xde; b = 0xb3;
	} else {
		r = 0xff; g = 0xfd; b = 0xf6;
	}
	Invalidate();

	if (m_item.m_itemIndex) {
		char itemName[256];
		char itemHelp[128];
		if (x > 249 && x < 273 && y > 333 && y < 357) {
			m_item.GetItemName(itemName);
			sprintf(itemHelp, MsgStr(MSI_EA2), itemName, m_item.m_num);

			int gx = 249;
			int gy = 333;
			GetGlobalCoor(gx, gy);
			g_modeMgr.GetCurMode()->MakeHelpBalloon(itemHelp, gx, gy, -1);
		}
	}

	UIFrameWnd::OnMouseHover(x, y);
}

void UIMailViewWnd::OnLBtnDown(int x, int y) {
	if (x > 144 && x < 279 && y > 327 && y < 363) {
		if (m_item.m_itemIndex != 0 || g_session.m_mailOpen.Money > 0) {
			CGameMode* gm = g_modeMgr.GetGameMode();
			gm->SendMsg(MM_GET_MAIL_ITEM, g_session.m_mailOpen.MailID, 0, 0);
		}
	}
	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
	}
}

void UIMailViewWnd::OnRBtnDown(int x, int y)
{
	if (m_item.m_itemIndex) {
		if (x > 249 && x < 273 && y > 333 && y < 357) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&m_item, 0, 0, 0);
		}
	}
}

int UIMailViewWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_MAILVIEWWND);
			return 0;

		case ID_RETURN_MAIL: {
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			gameMode->SendMsg(MM_RETURN_MAIL, g_session.m_mailOpen.MailID, (int)g_session.m_mailOpen.FromName, 0);
			return 0;
		}

		case ID_SEND_MAIL: {
			g_windowMgr.MakeWindow(WID_MAILLISTWND);
			UIWindow* mailListWnd = g_windowMgr.GetMailListWnd();
			if (mailListWnd)
				mailListWnd->SendMsg(NULL, UM_NEWMAIL, (int)g_session.m_mailOpen.FromName, 0, 0, 0);
			return 0;
		}

		case ID_DELETE_MAIL:
			if (!m_item.m_itemIndex && g_session.m_mailOpen.Money <= 0)
				g_modeMgr.GetGameMode()->SendMsg(MM_DELETE_MAIL, g_session.m_mailOpen.MailID, 0, 0);
			else
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_MAIL_DELETE_ERROR), 0xff, 0, 0);
			return 0;

		case ID_CANCEL:
			m_item = ITEM_INFO();
			Invalidate();
			return 0;
		}
		return 0;

	case UM_REFRESHITEM:
		m_item = ITEM_INFO();
		m_zeny = 0;
		g_session.m_mailOpen.Money = 0;
		Invalidate();
		return 0;

	case UM_REFRESH: {
		m_item = ITEM_INFO();

		if (g_session.m_mailOpen.ITID) {
			char itemName[25];
			_itoa(g_session.m_mailOpen.ITID, itemName, 10);
			itemName[24] = '\0';
			m_item.m_itemType = g_session.m_mailOpen.Type;
			m_item.m_itemIndex = g_session.m_mailOpen.ITID;
			m_item.m_isIdentified = g_session.m_mailOpen.IsIdentified;
			m_item.m_itemName = itemName;
			m_item.m_num = g_session.m_mailOpen.count;
			m_item.m_slot[0] = g_session.m_mailOpen.slot.info[0];
			m_item.m_slot[1] = g_session.m_mailOpen.slot.info[1];
			m_item.m_slot[2] = g_session.m_mailOpen.slot.info[2];
			m_item.m_slot[3] = g_session.m_mailOpen.slot.info[3];
			m_item.m_refiningLevel = g_session.m_mailOpen.refiningLevel;
			m_item.m_isDamaged = g_session.m_mailOpen.IsDamaged;
		}
		m_zeny = g_session.m_mailOpen.Money;

		int m = 1;
		char m_text[230];
		strcpy(m_text, "");
		if (val1 < m)
			return 0;

		char* p = (char*)val2 + 1;
		int len = p[-1];
		if (len < 0)
			len += 256;

		if (len == val1 - 1) {
			memcpy(m_text, p, len);
			m_text[len] = '\0';
		} else if (len > 0) {
			for (int i = 0; i < 14; ++i) {
				if (m > val1)
					break;

				len = ((char*)val2)[m - 1];
				if (len < 0)
					len += 256;

				if (len > 0x23) {
					len = 0;
				} else if (len > 0) {
					char* buf;
					if (len == 0x23) {
						buf = new char[3];
						buf[0] = '\r';
						buf[1] = '\n';
						buf[2] = '\0';
						len = 0;
					} else {
						buf = new char[len + 1];
						memcpy(buf, (char*)val2 + m, len);
						buf[len] = '\0';
					}
					strcat(m_text, buf);
				}
				m += len + 1;
			}
			m_text[strlen(m_text)] = '\0';
		}

		{
			std::vector<std::string> strings;
			std::vector<bool> enters;
			PolishText3(m_text, strings, enters, m_MailContent->GetWidth());
			m_MailContent->ClearAllItems();
			for (int i = 0; i < strings.size(); ++i)
				m_MailContent->AddItem2(strings[i].c_str(), 0x545d81, "");
			int numItem = m_MailContent->GetNumItem();
			m_MailContent->SetScrollPos(numItem);
		}

		Invalidate();
		return 0;
	}
	}

	return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
}

UIAuctionWnd::UIAuctionWnd()
{
	m_type = 0;
	m_page = 1;
	m_maxPage = 0;
	m_searchAuction = 0;
	m_curItem = 0;
	m_curAuctionItem = NULL;
	m_curOrderBtn = 0;
	m_curSearchBtn = 0;
}

UIAuctionWnd::~UIAuctionWnd()
{}

void UIAuctionWnd::OnCreate(int cx, int cy) {}

void UIAuctionWnd::OnDraw() {}

void UIAuctionWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
}

void UIAuctionWnd::OnLBtnUp(int x, int y)
{
	if (g_windowMgr.GetImgMenuWnd()) {
		g_windowMgr.DeleteWindow((WINDOWID)0x7a);
	}
	m_curAuctionItem = NULL;
	SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
	UIFrameWnd::OnLBtnUp(x, y);
}

void UIAuctionWnd::OnRBtnUp(int x, int y)
{
	if (g_windowMgr.GetImgMenuWnd()) {
		g_windowMgr.DeleteWindow((WINDOWID)0x7a);
	}
	m_curAuctionItem = NULL;
	SendMsg(NULL, UM_REFRESH, 0, 0, 0, 0);
}

void UIAuctionWnd::DragAndDrop(int x, int y, const DRAG_INFO* const dragInfo) {
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}
	CGameMode* gm = g_modeMgr.GetGameMode();
	if (dragInfo->m_dragType == 1) {
		SendMsg(0, UM_DROP, 0, 0, 0, 0);
		return;
	}
	gm->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}

int UIAuctionWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();

	switch (message) {
	case UM_COMMAND: {
		switch (val1) {
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_AUCTIONWND);
			break;

		case ID_AUCTION_VIEW:
			m_type = 0;
			m_btnOrder[m_curOrderBtn]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
			m_btnSearch[m_curSearchBtn]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
			m_curOrderBtn = 0;
			m_curSearchBtn = 0;
			m_btnOrder[0]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
			m_btnSearch[0]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
			g_session.m_auctionSeachOption = 0;
			g_session.m_auctionSeachOption2 = 0;
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			gameMode->SendMsg(MM_SEARCH_AUCTION_ITEM, 0, 0, 1);
			break;

		case ID_AUCTION_ADD:
			m_addBtn->SetDiabled(0);
			m_type = 1;
			m_item = ITEM_INFO();
			m_nowMoney->SetText("10,000,000");
			m_maxMoney->SetText("10,000,001");
			m_nowMoney->Invalidate();
			m_maxMoney->Invalidate();
			m_deleteHour->SetText("");
			gameMode->SendMsg(MM_RESET_AUCTION_ITEM, 0, 0, 0);
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			break;

		case ID_AUCTION_SELL:
			m_type = 2;
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			gameMode->SendMsg(MM_AUCTION_REQ_MY_INFO, 0, 0, 0);
			break;

		case ID_AUCTION_BUY:
			m_type = 3;
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			gameMode->SendMsg(MM_AUCTION_REQ_MY_INFO, 1, 0, 0);
			break;

		case ID_ADD_AUCTION:
			if (!g_session.m_waitForAddAuction && m_item.m_itemIndex) {
				int nowMoney = GetNumberInText((char*)m_nowMoney->GetText());
				int maxMoney = GetNumberInText((char*)m_maxMoney->GetText());
				int deleteHour = GetNumberInText((char*)m_deleteHour->GetText());

				if (nowMoney <= 0 || maxMoney <= 0 || deleteHour <= 0) {
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ERROR_AUCTION_ITEM_INFO), 0xffffff, 0, 0);
					if (nowMoney <= 0)
						g_windowMgr.SetFocusEdit(m_nowMoney);
					else if (maxMoney <= 0)
						g_windowMgr.SetFocusEdit(m_maxMoney);
					else
						g_windowMgr.SetFocusEdit(m_deleteHour);
					return 0;
				}
				m_addBtn->SetDiabled(1);
				gameMode->SendMsg(MM_ADD_AUCTION, nowMoney, maxMoney, deleteHour);
				g_session.m_waitForAddAuction = 1;
				break;
			} else if (!m_item.m_itemIndex) {
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_NOTHING_AUCTION_ITEM_INFO), 0xffffff, 0, 0);
				return 0;
			}
			if (g_session.m_waitForAddAuction) {
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_ERROR_ADD_AUCTION), 0xffffff, 0, 0);
				return 0;
			}
			break;

		case ID_CANCEL:
			if (m_type == 1) {
				g_session.m_waitForAddAuction = 0;
				gameMode->SendMsg(MM_RESET_AUCTION_ITEM, 1, 0, 0);
				g_windowMgr.DeleteWindow(WID_AUCTIONWND);
			} else if (m_type == 2) {
				if (m_curAuctionItem) {
					gameMode->SendMsg(MM_CANCEL_ADD_AUCTION, m_curAuctionItem->AuctionID, 0, 0);
					gameMode->SendMsg(MM_SEARCH_AUCTION_ITEM,
						g_session.m_auctionSeachOption,
						g_session.m_auctionSeachOption2,
						1);
				}
			}
			break;

		case ID_FINISH_AUCTION:
		case ID_BUY_AUCTION: {
			if (g_windowMgr.GetAuctionMsgWnd())
				g_windowMgr.DeleteWindow(WID_AUCTIONMSGWND);

			UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_AUCTIONMSGWND);
			if (val1 == ID_FINISH_AUCTION)
				wnd->SendMsg(0, UM_SETSTATE, 2, 0, 0, 0);
			else
				wnd->SendMsg(0, UM_SETSTATE, 3, 0, 0, 0);

			int price = m_curAuctionItem->NowPrice;
			ITEM_INFO* item = GetCurAuctionItem2(m_curAuctionItem);
			if (item && item->m_itemIndex) {
				char itemName[25];
				item->GetItemName(itemName);
				wnd->SendMsg(0, UM_SETINFO, m_curAuctionItem->AuctionID, (int)itemName, price, 0);
			}

			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			break;
		}

		case ID_RADIOBTN: {
			m_page = 1;
			m_maxPage = 0;
			int i;
			if (sender == m_btnOrder[0] || sender == m_btnOrder[1] || sender == m_btnOrder[2] || sender == m_btnOrder[3]) {
				g_session.m_auctionList.clear();
				m_searchAuction = 1;
				SendMsg(0, UM_REFRESH, 0, 0, 0, 0);

				for (i = 0; i < 4; ++i) {
					if (sender == m_btnOrder[i]) {
						g_session.m_auctionSeachOption = i;
						g_session.m_auctionSeachOption2 = 0;
						gameMode->SendMsg(MM_SEARCH_AUCTION_ITEM, i, 0, m_page);
						m_curOrderBtn = i;
						m_btnOrder[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
					} else {
						m_btnOrder[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
					}
				}
			}

			if (sender == m_btnSearch[0] || sender == m_btnSearch[1]) {
				for (i = 0; i < 2; ++i) {
					if (sender == m_btnSearch[i]) {
						m_curSearchBtn = i;
						m_btnSearch[i]->SendMsg(this, UM_SETSTATE, 1, 0, 0, 0);
					} else {
						m_btnSearch[i]->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
					}
				}
			}
			break;
		}

		case ID_SEARCH: {
			g_session.m_auctionList.clear();
			m_searchAuction = 1;
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);

			if (m_curSearchBtn) {
				g_session.m_auctionSeachOption = 5;
				g_session.m_auctionSeachOption2 = atoi(m_search->GetText());
				gameMode->SendMsg(MM_SEARCH_AUCTION_ITEM, 5, atoi(m_search->GetText()), 1);
				break;
			}

			int len = strlen(m_search->GetText()) + 1;
			if (len <= 1)
				break;
			if (len > 5)
				len = 5;

			char* msg = (char*)operator new(len);
			strcpy(msg, PolishText2(m_search->GetText(), len - 1));
			g_session.m_auctionSeachOption = 4;
			g_session.m_auctionSeachOption2 = (int)msg;
			gameMode->SendMsg(MM_SEARCH_AUCTION_ITEM, 4, (int)m_search->GetText(), 1);
			break;
		}
		}
		break;
	}

	case UM_DROP:
		if (m_type == 1) {
			DRAG_INFO *dragInfo = gameMode->GetDragInfo();
			if (m_item.m_itemIndex)
				gameMode->SendMsg(MM_RESET_AUCTION_ITEM, 1, 0, 0);

			if (!dragInfo->m_isIdentified) {
				m_item = ITEM_INFO();
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_AUCTION_ADD_ITEM_FAILURE), 0xff, 0, 0);
			} else if (dragInfo->m_dragItemType == 0 ||
				dragInfo->m_dragItemType == 1 ||
				dragInfo->m_dragItemType == 2) {
				m_item = ITEM_INFO();
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_AUCTION_ADD_ITEM_FAILURE2), 0xff, 0, 0);
			} else {
				char itemName[256];
				char text[256];
				char itemHelp[256];

				m_item.m_itemType = dragInfo->m_dragItemType;
				m_item.m_itemIndex = gameMode->GetDragItemIndex();
				m_item.m_isIdentified = dragInfo->m_isIdentified;
				m_item.m_num = 1;
				m_item.m_itemName = dragInfo->m_dragItemName;
				memcpy(m_item.m_slot, dragInfo->m_slot, sizeof(m_item.m_slot));
				m_item.m_refiningLevel = dragInfo->m_refiningLevel;

				m_item.GetItemName(itemName);
				int x = 63 + (94 - GetTextWidth(PolishText(itemName, 94, 0, 12, false), 0, 0, 12, false)) / 2;
				m_item.m_num = m_item.m_num; m_item.m_itemType = m_item.m_itemType;
				m_itemNameBtn->SetText(std::string(itemName));
				m_itemNameBtn->Resize2(94, m_itemNameBtn->GetHeight());
				m_itemNameBtn->Move(x, 80);
				m_itemNameBtn->SetToolTip(itemName);
				g_session.GetItemBitmapFileName(m_item.m_itemName.c_str(), text, m_item.m_isIdentified);
				sprintf(itemHelp, MsgStr(MSI_EA2), itemName, m_item.m_num);
				m_itemBmpBtn->SetBitmapName(text, BS_NORMAL);
				m_itemBmpBtn->SetBitmapName(text, BS_MOUSEON);
				m_itemBmpBtn->SetBitmapName(text, BS_PRESSED);
				m_itemBmpBtn->SetToolTip(itemHelp);
				m_itemBmpBtn->Invalidate();
				RAG_BUDGET_SITES(3)
				gameMode->SendMsg(MM_ADD_AUCTION_ITEM, gameMode->GetDragItemIndex(), 1, 0);
			}
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		}
		break;

	case UM_BMP_RBTN_UP:
		if (m_type == 0 || m_type == 2 || m_type == 3) {
			ITEM_INFO* item = GetCurAuctionItem(val1);
			if (item && item->m_itemIndex) {
				UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
				wnd->SendMsg(0, UM_ITEMEXPLANATION, (int)item, 0, 0, 0);
			}
		} else if (m_type == 1 && m_item.m_itemIndex) {
			UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			wnd->SendMsg(0, UM_ITEMEXPLANATION, (int)&m_item, 0, 0, 0);
		}
		break;

	case UM_TEXT_LBTN_UP:
		switch (val1) {
		case ID_ITEM_NAME:
			if (m_type == 2 || m_type == 3) {
				m_curAuctionItem = GetCurAuction(val2);
				SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			}
			break;

		case ID_PREV_PAGE:
			if (m_page > 1)
				--m_page;
			gameMode->SendMsg(MM_SEARCH_AUCTION_ITEM, g_session.m_auctionSeachOption, g_session.m_auctionSeachOption2, m_page);
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			break;

		case ID_NEXT_PAGE:
			if (m_page < m_maxPage)
				++m_page;
			gameMode->SendMsg(MM_SEARCH_AUCTION_ITEM, g_session.m_auctionSeachOption, g_session.m_auctionSeachOption2, m_page);
			SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			break;
		}
		break;

	case UM_TEXT_RBTN_UP:
		if (val1 == ID_ITEM_NAME) {
			if (m_type == 0) {
				m_curAuctionItem = GetCurAuction(val2);
				if (m_curAuctionItem) {
					if (g_windowMgr.GetImgMenuWnd())
						g_windowMgr.DeleteWindow(WID_IMGMENUWND);
					UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_IMGMENUWND);
					wnd->SendMsg(0, UM_SETINFO, 0, 2,
						(int)"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\auction_menu", 0);
					wnd->SendMsg(0, UM_SETID, val2, 0, 0, 0);
					wnd->Resize(55, 40);
					wnd->SendMsg(0, UM_SET_CALLER_WID, WID_AUCTIONWND, 0, 0, 0);
					int gx = 140;
					int gy = m_curItem * 40 + 72;
					GetGlobalCoor(gx, gy);
					wnd->Move(gx, gy);
				}
				SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
			}
		}
		break;

	case UM_MENU_SELECTED: {
		if (g_windowMgr.GetAuctionMsgWnd())
			g_windowMgr.DeleteWindow(WID_AUCTIONMSGWND);
		UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_AUCTIONMSGWND);
		wnd->SendMsg(0, UM_SETSTATE, val1, 0, 0, 0);

		int price = m_curAuctionItem->NowPrice;
		if (val1 == 1)
			price = m_curAuctionItem->MaxPrice;
		ITEM_INFO* item = GetCurAuctionItem2(m_curAuctionItem);
		if (item && item->m_itemIndex) {
			char itemName[25];
			item->GetItemName(itemName);
			wnd->SendMsg(0, UM_SETINFO, m_curAuctionItem->AuctionID, (int)itemName, price, 0);
		}
		SendMsg(0, UM_REFRESH, 0, 0, 0, 0);
		break;
	}

	case UM_REFRESH:
		if (val1)
			m_searchAuction = 0;
		if (val2)
			m_maxPage = val2;
		Invalidate();
		InvalidateChildren();
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}


AUCTION_ITEM_SEARCH_INFO* UIAuctionWnd::GetCurAuction(int index)
{
	if (!g_session.m_auctionList.empty()) {
		int row = 0;
		std::vector<AUCTION_ITEM_SEARCH_INFO>::iterator it = g_session.m_auctionList.begin();
		while (it < g_session.m_auctionList.end()) {
			if (index == it->AuctionID) {
				m_curItem = row % 5;
				return &*it;
			}
			++it;
			++row;
		}
	}

	return NULL;
}

ITEM_INFO * UIAuctionWnd::GetCurAuctionItem(int index)
{
	ITEM_INFO* item = new ITEM_INFO;

	if (!g_session.m_auctionList.empty()) {
		for (std::vector<AUCTION_ITEM_SEARCH_INFO>::iterator it = g_session.m_auctionList.begin(); it < g_session.m_auctionList.end(); ++it) {
			if (index == it->AuctionID && it->ITID) {
				char itemName[25];
				itoa(it->ITID, itemName, 10);
				item->m_itemIndex = it->ITID;
				item->m_itemType = it->Type;
				item->m_isIdentified = it->IsIdentified;
				itemName[24] = 0;
				item->m_itemName = itemName;
				item->m_num = it->count;
				item->m_slot[0] = it->slot.info[0];
				item->m_slot[1] = it->slot.info[1];
				item->m_slot[2] = it->slot.info[2];
				item->m_slot[3] = it->slot.info[3];
				item->m_refiningLevel = it->refiningLevel;
				item->m_isDamaged = it->IsDamaged;
				break;
			}
		}
	}

	return item;
}

ITEM_INFO * UIAuctionWnd::GetCurAuctionItem2(AUCTION_ITEM_SEARCH_INFO *auction)
{
	char itemName[25];
	ITEM_INFO* item = new ITEM_INFO;

	if (auction->ITID) {
		itoa(auction->ITID, itemName, 10);
		item->m_itemIndex = auction->ITID;
		item->m_itemType = auction->Type;
		item->m_isIdentified = auction->IsIdentified;
		itemName[24] = 0;
		item->m_itemName = itemName;
		item->m_num = auction->count;
		item->m_slot[0] = auction->slot.info[0];
		item->m_slot[1] = auction->slot.info[1];
		item->m_slot[2] = auction->slot.info[2];
		item->m_slot[3] = auction->slot.info[3];
		item->m_refiningLevel = auction->refiningLevel;
		item->m_isDamaged = auction->IsDamaged;
	}

	return item;
}

void UIAuctionWnd::InitAllBtnPos() {}
UIAuctionMsgWnd::UIAuctionMsgWnd() { m_type = 0; m_price = 0; m_auctionID = 0; }
UIAuctionMsgWnd::~UIAuctionMsgWnd() {}

void UIAuctionMsgWnd::OnCreate(int cx, int cy)
{
	int closeX = m_w - 15;

	{
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\close";
		char toolTip[6] = "Close";

		UIBitmapButton* closeButton = new UIBitmapButton;
		closeButton->SetBitmapName((preBmpName + ".bmp").c_str(), BS_NORMAL);
		closeButton->SetBitmapName((preBmpName + "_a.bmp").c_str(), BS_MOUSEON);
		closeButton->SetBitmapName((preBmpName + "_a.bmp").c_str(), BS_PRESSED);
		closeButton->Create(closeButton->GetBitmapWidth(), closeButton->GetBitmapHeight());
		closeButton->Move(closeX, 4);
		closeButton->SetId(ID_CLOSE);
		closeButton->SetToolTip(toolTip);
		AddChild(closeButton);
	}

	m_Money = new UIEditCtrl2;
	m_Money->Create(65, 16);
	m_Money->SetType(1);
	m_Money->Move(-100, -100);
	m_Money->SetTextLimit(11);
	m_Money->SetFrameColor(0xff, 0xff, 0xff);
	AddChild(m_Money);

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\ok";
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), BS_NORMAL);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), BS_MOUSEON);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), BS_PRESSED);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		m_button->Move(122, 129);
		m_button->SetId(ID_OK);
		AddChild(m_button);
	}

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\cancel";
		m_button = new UIBitmapButton;
		m_button->SetBitmapName((pathName + ".bmp").c_str(), BS_NORMAL);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), BS_MOUSEON);
		m_button->SetBitmapName((pathName + "_a.bmp").c_str(), BS_PRESSED);
		m_button->Create(m_button->GetBitmapWidth(), m_button->GetBitmapHeight());
		m_button->Move(180, 129);
		m_button->SetId(ID_CANCEL);
		AddChild(m_button);
	}
}

void UIAuctionMsgWnd::OnDraw()
{
	char buf[300];
	char windowName[64];
	char price[64];

	CBitmapRes *bmp = (CBitmapRes *)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\auction_msg.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	sprintf(buf, "%s : %s", MsgStr(MSI_ITEM_NAME2), itemName);
	TextOutA(16, 31, buf, 0, 0, 12, 0);

	GetNumberText(m_price, price);
	m_Money->Move(-100, -100);

	if (m_type == 0) {
		strcpy(windowName, MsgStr(MSI_BUY_AUCTION));
		sprintf(buf, "%s : %s", MsgStr(MSI_PRICE), price);
		TextOutA(16, 62, buf, 0, 0, 12, 0);
		sprintf(buf, "%s : ", MsgStr(MSI_PRICE2));
		TextOutA(16, 91, buf, 0, 0, 12, 0);
		m_Money->Move(100, 88);
	} else if (m_type == 1) {
		strcpy(windowName, MsgStr(MSI_BUY_AUCTION2));
		sprintf(buf, "%s : %s", MsgStr(MSI_PRICE3), price);
		TextOutA(16, 62, buf, 0, 0, 12, 0);

		GetNumberText(g_session.GetGold(), price);
		sprintf(buf, "%s : %s", MsgStr(MSI_PRICE4), price);
		TextOutA(16, 91, buf, 0, 0, 12, 0);
		TextOutA(16, 130, MsgStr(MSI_BUY_ACTION_ITEM), 0, 0, 12, 0);
	} else if (m_type == 2) {
		strcpy(windowName, MsgStr(MSI_FINISH_AUCTION));
		sprintf(buf, "%s : %s", MsgStr(MSI_PRICE5), price);
		TextOutA(16, 62, buf, 0, 0, 12, 0);
		TextOutA(16, 91, MsgStr(MSI_SELL_ACTION_ITEM), 0, 0, 12, 0);
	} else if (m_type == 3) {
		strcpy(windowName, MsgStr(MSI_BUY_AUCTION3));
		sprintf(buf, "%s : %s", MsgStr(MSI_PRICE6), price);
		TextOutA(16, 62, buf, 0, 0, 12, 0);
		sprintf(buf, "%s : ", MsgStr(MSI_PRICE7));
		TextOutA(16, 91, buf, 0, 0, 12, 0);
		m_Money->Move(100, 88);
	}

	TextOutA(11, 4, windowName, 0, 0, 12, 0xffffff);
	TextOutA(10, 3, windowName, 0, 0, 12, 0);
}

int UIAuctionMsgWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_COMMAND:
		switch (val1) {
		case ID_CANCEL:
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_AUCTIONMSGWND);
			break;
		case ID_OK: {
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			if (m_type == 0) {
				int price = GetNumberInText((char*)m_Money->GetText());
				char t_Price[64];
				GetNumberText(price, t_Price);
				m_Money->SetText(t_Price);
				gameMode->SendMsg(MM_BUY_AUCTION, m_auctionID, price, 0);
			} else if (m_type == 1) {
				gameMode->SendMsg(MM_BUY_AUCTION, m_auctionID, m_price, 0);
			} else if (m_type == 2) {
				gameMode->SendMsg(MM_AUCTION_MY_SELL_STOP, m_auctionID, 0, 0);
			} else if (m_type == 3) {
				int price = GetNumberInText((char*)m_Money->GetText());
				char t_Price[64];
				GetNumberText(price, t_Price);
				m_Money->SetText(t_Price);
				gameMode->SendMsg(MM_BUY_AUCTION, m_auctionID, price, 0);
			}
			g_windowMgr.DeleteWindow(WID_AUCTIONMSGWND);
			break;
		}
		}
		break;
	case UM_SETSTATE:
		m_type = val1;
		Invalidate();
		break;
	case UM_SETINFO:
		m_auctionID = val1;
		m_price = val3;
		strcpy(itemName, (char*)val2);
		Invalidate();
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}
UIImgMenuWnd::UIImgMenuWnd() { m_imgMenu = NULL; }
UIImgMenuWnd::~UIImgMenuWnd() {}

void UIImgMenuWnd::OnCreate(int cx, int cy)
{
	m_imgMenu = new UIImgMenu;
	m_imgMenu->Create(cx, cy, 0, 0,
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\auction_menu");
	AddChild(m_imgMenu);
}

void UIImgMenuWnd::Resize(int cx, int cy) {
	UIFrameWnd::Resize(cx + 3, cy + 3);
	m_imgMenu->Resize(cx + 3, cy + 3);
	m_imgMenu->Invalidate();
}

void UIImgMenuWnd::OnDraw() {
	ClearDC(0xffff00ff);
}

void UIImgMenuWnd::Move(int x, int y)
{
	if (x > g_renderer->m_width - m_w)
		x = g_renderer->m_width - m_w - 10;

	if (y > g_renderer->m_height - m_h)
		y = g_renderer->m_height - m_h - 10;

	UIFrameWnd::Move(x, y);
}

int UIImgMenuWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4) {
	return m_imgMenu->SendMsg(sender, message, val1, val2, val3, val4);
}


const bool UIPartySettingWnd::s_bUseNewPacket = true;
