#include "common.h"
#include "MsgStrings.h"


#include "base/Util.h"
#include "framework/Locale.h"
#include "device/Control.h"
#include "ItemInfo.h"
#include "Mode.h"
#include "pdb_enums.h"
#include "Session.h"
#include "SkillInfo.h"
#include "SnapMgr.h"
#include "window/UIFrameWnd.h"
#include "UIRectInfo.h"
#include "window/UISys.h"
#include "GameMode.h"
#include "base/File.h"
#include "device/Connection.h"
#include "resource/Bitmap.h"
#include "skin.h"

#include <mmsystem.h>

extern UIWindowMgr g_windowMgr;
extern CModeMgr    g_modeMgr;
extern CSession    g_session;
extern CSnapMgr    g_snapMgr;
extern CRenderer*  g_renderer;
extern const char* MsgStr(MSGSTRINGID id);
extern const char* UIBmp(const char* uiBmpName);
extern ServiceType  g_serviceType;
extern char         g_isGravityID;



int           g_isSkillEntry  = 0;
char          g_SkillEntryNum = 0;
unsigned long g_SkillEntryTime = 0;
int           g_EntrySKID     = 0;

UIItemDropCntWnd::UIItemDropCntWnd() {}

UIItemDropCntWnd::~UIItemDropCntWnd() {}

#include "rag_budget_fill.h"
#include "rag_budget_sites.h"
void UIItemDropCntWnd::OnCreate(int cx, int cy)
{
	RAG_BUDGET_FILL(15)
	m_editCtrl = new UIEditCtrl;
	m_editCtrl->Create(80, 16);
	m_editCtrl->Move(15, 22);
	m_editCtrl->SetTextLimit(6);
	m_editCtrl->SetFrameColor(232, 232, 232);
	AddChild(m_editCtrl);

	CGameMode* gameMode = g_modeMgr.GetGameMode();

	char buf[40];
	int numDragItem = gameMode->GetNumDragItem();
	sprintf(buf, "%d", numDragItem);
	m_editCtrl->SetText(buf);
	g_windowMgr.SetFocusEdit(m_editCtrl);
	m_editCtrl->MakeSelect();

	const char* dragItemName = gameMode->GetDragItemName();
	std::string msg;
	if (dragItemName) {
		ITEM_INFO dummyItemInfo;
		dummyItemInfo.m_itemName = dragItemName;
		dummyItemInfo.m_isIdentified = gameMode->GetDragInfo()->m_isIdentified;
		dummyItemInfo.m_refiningLevel = 0;

		char itemDisName[128];
		dummyItemInfo.GetItemName(itemDisName);
		msg = itemDisName;
	}

	m_newText = new UIStaticText;
	m_newText->Create(m_w - 30, 15);
	m_newText->Move(15, 6);
	m_newText->SetText(msg.c_str(), 0);
	AddChild(m_newText);

	{
		std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
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
			button->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED); RAG_BUDGET_SITES(1)
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[index][0], pos[index][1]);
			button->SetId(ids[index]);
			AddChild(button);
		}
	}

	m_defPushId = ID_OK;
	m_state = 0;
}

void UIItemDropCntWnd::OnDraw() { DrawLatticeFrame(0, 0, 0, 0); }

void UIItemDropCntWnd::SetShow(int visible)
{
	m_show = visible;
	if (visible) {
		CGameMode* gameMode = g_modeMgr.GetGameMode();

		char buf[40];
		int numDragItem = gameMode->GetNumDragItem();
		sprintf(buf, "%d", numDragItem);
		m_editCtrl->SetText(buf);
		g_windowMgr.SetFocusEdit(m_editCtrl);
		m_editCtrl->MakeSelect();

		const char* dragItemName = gameMode->GetDragItemName();
		std::string msg;
		if (dragItemName) {
			ITEM_INFO dummyItemInfo;
			dummyItemInfo.m_itemName = dragItemName;
			dummyItemInfo.m_isIdentified = gameMode->GetDragInfo()->m_isIdentified;
			dummyItemInfo.m_refiningLevel = 0;

			char itemDisName[128];
			dummyItemInfo.GetItemName(itemDisName);
			msg = itemDisName;
		}

		m_newText->SetText(msg.c_str(), 0);
		Invalidate();
		g_windowMgr.UpdateSnapInfo(this);
	} else {
		if (m_editCtrl == g_windowMgr.GetFocusEdit()) {
			RAG_BUDGET_FILL(64)
			if (g_windowMgr.GetChatWnd())
				g_windowMgr.GetChatWnd()->SendMsg(NULL, UM_GET_EDIT_FOCUS, 0, 0, 0, 0);
		}
		g_windowMgr.DeleteSnapInfo(this);
	}
}

int UIItemDropCntWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message)
	{
	case UM_SETSTATE:
		m_state = val1;
		break;
	case UM_COMMAND:
		if (val1 == ID_OK) {
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			DRAG_INFO* dragInfo = gameMode->GetDragInfo();
			Trace("UIItemDropCntWnd ID_OK");

			switch (m_state) {
			case 1: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to purchasewnd numItemInputed == %d\n", numItemInputed);
				if (numItemInputed <= 0) {
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				} else {
					if (numItemInputed > 10000)
						numItemInputed = 10000;
					dragInfo->m_numDragItem = numItemInputed;
					if (g_windowMgr.GetItemPurchaseWnd())
						g_windowMgr.GetItemPurchaseWnd()->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				}
				break;
			}
			case 0: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("from itemwnd numItemInputed == %d", numItemInputed);
				if (numItemInputed > 0 && numItemInputed <= dragInfo->m_numDragItem)
					gameMode->SendMsg(MM_ITEMTHROW_THROUGH_DROPCNTWND, numItemInputed, 0, 0);
				else
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				break;
			}
			case 2: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to sellwnd numItemInputed == %d\n", numItemInputed);
				int itemNum = g_session.GetShopItemNum(dragInfo->m_dragItemIndex);
				if (itemNum) {
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					UIWindow* wnd = g_windowMgr.GetItemSellWnd();
					if (wnd)
						wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				} else {
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				}
				break;
			}
			case 3: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to sellwnd numItemInputed == %d\n", numItemInputed);
				int itemNum = g_session.GetPurchaseItemNum(dragInfo->m_dragItemName.c_str());
				if (itemNum) {
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					UIWindow* wnd = g_windowMgr.GetItemShopWnd();
					if (wnd)
						wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				} else {
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				}
				break;
			}
			case 4: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to sellwnd numItemInputed == %d\n", numItemInputed);
				int itemNum = g_session.GetSellItemNum(dragInfo->m_dragItemIndex);
				if (itemNum) {
					Trace("sellItemNum == %d\n", itemNum);
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					UIWindow* wnd = g_windowMgr.GetItemShopWnd();
					if (wnd)
						wnd->SendMsg(0, UM_DROP, 1, 0, 0, 0);
				} else {
					Trace("gameMode->SendMsg(MM_CANCEL_DRAG); sellItemNum == %d\n", itemNum);
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				}
				break;
			}
			case 5: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to exchangeWnd numItemInputed == %d\n", numItemInputed);
				if (g_session.IsValidItemIndexInUnequiped(dragInfo->m_dragItemIndex)) {
					int itemNum = g_session.GetItemInfoByIndex(dragInfo->m_dragItemIndex).m_num;
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					UIWindow* wnd = g_windowMgr.GetExchangeWnd();
					if (wnd)
						wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				} else {
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				}
				break;
			}
			case 6: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to exchangeWnd numItemInputed == %d\n", numItemInputed);
				switch (dragInfo->m_dragType) {
				case DT_FROM_ITEMSTOREWND: {
					if (g_session.GetStoreItemNum(dragInfo->m_dragItemIndex) == 0)
						break;
					int itemNum = g_session.GetStoreItemInfoByIndex(dragInfo->m_dragItemIndex).m_num;
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					if (g_windowMgr.GetItemWnd())
						g_windowMgr.GetItemWnd()->SendMsg(0, UM_DROP, 0, 0, 0, 0);
					break;
				}
				case DT_FROM_MERCHANTITEMWND: {
					if (g_session.GetMerchantItemNum(dragInfo->m_dragItemIndex) == 0)
						break;
					int itemNum = g_session.GetMerchantItemInfoByIndex(dragInfo->m_dragItemIndex).m_num;
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					UIWindow* wnd = g_windowMgr.GetItemWnd();
					if (wnd)
						wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
					break;
				}
				case DT_FROM_MAILWND: {
					int itemNum;
					if ((itemNum = dragInfo->m_numDragItem) == 0)
						break;
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					UIWindow* wnd = g_windowMgr.GetItemWnd();
					if (wnd)
						wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
					break;
				}
				}
				break;
			}
			case 7: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to exchangeWnd numItemInputed == %d\n", numItemInputed);
				int itemNum = 0;
				switch (dragInfo->m_dragType) {
				case DT_FROM_ITEMWND:
					if (!g_session.IsValidItemIndexInUnequiped(dragInfo->m_dragItemIndex))
						break;
					itemNum = g_session.GetItemInfoByIndex(dragInfo->m_dragItemIndex).m_num;
					break;
				case DT_FROM_MERCHANTITEMWND: {
					ITEM_INFO itemInfo = g_session.GetMerchantItemInfoByIndex(dragInfo->m_dragItemIndex);
					if (itemInfo.IsValid())
						itemNum = itemInfo.m_num;
				}
					break;
				}

				if (itemNum == 0)
					break;
				if (numItemInputed <= 0)
					numItemInputed = 1;
				if (numItemInputed > itemNum)
					numItemInputed = itemNum;
				dragInfo->m_numDragItem = numItemInputed;
				UIWindow* wnd = g_windowMgr.GetItemStoreWnd();
				if (wnd)
					wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				break;
			}
			case 8: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to exchangeWnd numItemInputed == %d\n", numItemInputed);
				int itemNum = 0;
				switch (dragInfo->m_dragType) {
				case DT_FROM_ITEMWND: {
					ITEM_INFO itemInfo = g_session.GetItemInfoByIndex(dragInfo->m_dragItemIndex);
					if (itemInfo.IsValid())
						itemNum = itemInfo.m_num;
					break;
				}
				case DT_FROM_ITEMSTOREWND: {
					ITEM_INFO itemInfo = g_session.GetStoreItemInfoByIndex(dragInfo->m_dragItemIndex);
					if (itemInfo.IsValid())
						itemNum = itemInfo.m_num;
					break;
				}
				}

				if (itemNum == 0)
					break;
				if (numItemInputed <= 0)
					numItemInputed = 1;
				if (numItemInputed > itemNum)
					numItemInputed = itemNum;
				dragInfo->m_numDragItem = numItemInputed;
				UIWindow* wnd = g_windowMgr.GetMerchantItemWnd();
				if (wnd)
					wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				break;
			}
			case 9: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to exchangeWnd numItemInputed == %d\n", numItemInputed);
				int itemNum = 0;
				if (dragInfo->m_dragType != DT_FROM_MERCHANTMIRRORITEMWND)
					break;
				{
					ITEM_INFO itemInfo = g_session.GetMerchantMirrorItemInfoByIndex(dragInfo->m_dragItemIndex);
					if (itemInfo.IsValid())
						itemNum = itemInfo.m_num;
				}
				if (itemNum == 0)
					break;
				if (numItemInputed <= 0)
					numItemInputed = 1;
				if (numItemInputed > itemNum)
					numItemInputed = itemNum;
				dragInfo->m_numDragItem = numItemInputed;
				UIWindow* wnd = g_windowMgr.GetMerchantShopMakeWnd();
				if (wnd)
					wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				break;
			}
			case 10: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to exchangeWnd numItemInputed == %d\n", numItemInputed);
				int itemNum = 0;
				if (dragInfo->m_dragType != DT_FROM_MERCHANTSHOPMAKEWND)
					break;
				{
					ITEM_INFO itemInfo = g_session.GetMerchantHopeToSellItemInfoByIndex(dragInfo->m_dragItemIndex);
					if (itemInfo.IsValid())
						itemNum = itemInfo.m_num;
				}
				if (itemNum == 0)
					break;
				if (numItemInputed <= 0)
					numItemInputed = 1;
				if (numItemInputed > itemNum)
					numItemInputed = itemNum;
				dragInfo->m_numDragItem = numItemInputed;
				UIWindow* wnd = g_windowMgr.GetMerchantMirrorItemWnd();
				if (wnd)
					wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				break;
			}
			case 11: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				if (dragInfo->m_dragType != DT_FROM_MERCHANTITEMPURCHASEWND)
					break;
				int itemNum = g_session.GetMerchantPurchaseItemNum(dragInfo->m_dragItemName.c_str());
				if (itemNum == 0)
					break;
				if (numItemInputed <= 0)
					numItemInputed = 1;
				if (numItemInputed > itemNum)
					numItemInputed = itemNum;
				dragInfo->m_numDragItem = numItemInputed;
				UIWindow* wnd = g_windowMgr.GetMerchantItemShopWnd();
				if (wnd)
					wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				break;
			}
			case 12: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				if (numItemInputed <= 0)
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				if (numItemInputed > 10000)
					numItemInputed = 10000;
				dragInfo->m_numDragItem = numItemInputed;
				UIWindow* wnd = g_windowMgr.GetMerchantItemPurchaseWnd();
				if (wnd)
					wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				break;
			}
			case 13: {
				if (m_editCtrl->GetTextSize() == 0) break;
				int numItemInputed = atoi(m_editCtrl->GetText());
				Trace("to mailWnd numItemInputed == %d\n", numItemInputed);
				if (g_session.IsValidItemIndexInUnequiped(dragInfo->m_dragItemIndex)) {
					int itemNum = g_session.GetItemInfoByIndex(dragInfo->m_dragItemIndex).m_num;
					if (numItemInputed <= 0)
						gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					if (numItemInputed > itemNum)
						numItemInputed = itemNum;
					dragInfo->m_numDragItem = numItemInputed;
					if (g_windowMgr.GetMailListWnd())
						g_windowMgr.GetMailListWnd()->SendMsg(0, UM_DROP, 0, 0, 0, 0);
				} else {
					gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				}
				break;
			}
			}

			gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
			g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 0);
		}
		break;
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIItemBaseWnd::UIItemBaseWnd()
{
	m_resizer = NULL;
	m_scrollBar = NULL;
	m_viewOffset = 0;
	m_vertScrEnabled = 0;
	m_isBasket = 0;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_drawNumItem = 1;
}

UIItemBaseWnd::~UIItemBaseWnd()
{}

void UIItemBaseWnd::OnCreate(int cx, int cy) {}

void UIItemBaseWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int titleLoop = 0; titleLoop < titleMidCount; ++titleLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
	}

	if (m_h == 17)
		return;

	int sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int leftLoop = 0; leftLoop < sideCount; ++leftLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
		DrawBitmap(0, leftLoop * 8 + 17, bmp, false);
	}

	sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int rightLoop = 0; rightLoop < sideCount; ++rightLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, rightLoop * 8 + 17, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17, (m_w - 60) % 32, m_h - 38, RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17, m_w - 24, (m_h - 38) % 32, RGB(rr, gg, bb));
	}

	char* bmpName[2][3] = {
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp",
		},
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp",
		},
	};

	bmp = (CBitmapRes*)g_resMgr().Get(bmpName[m_isBasket][0], false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(bmpName[m_isBasket][1], false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (int btnLoop = 0; btnLoop < btnMidCount; ++btnLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(bmpName[m_isBasket][2], false);
		DrawBitmap(btnLoop * 21 + 21, m_h - bmp->m_height, bmp, false);
	}

	int hLoop = (m_w - 40) / 32;
	int itemRows = (m_h - 38) / 32;
	for (int vLoop = 0; vLoop < itemRows; ++vLoop) {
		for (int wLoop = 0; wLoop < hLoop; ++wLoop)
			DrawItemFrame(wLoop * 32 + 40, vLoop * 32 + 17);
	}

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		int itemX = (drawCount % m_itemHorzNum) * 32 + 40;
		int itemY = (drawCount / m_itemHorzNum) * 32 + 17;
		if (m_drawNumItem)
			DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);
		else
			DrawItem(itemX + 4, itemY + 4, *it, false);

		if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
			return;

		++it;
		++drawCount;
		++itemCount;
	}
}

ITEM_INFO UIItemBaseWnd::GetItemInfo(int x, int y)
{
	if (x < 40 || x >= m_w - 20 || y < 17 || y >= m_itemVertNum * 32 + 17)
		return ITEM_INFO();

	int xCell = (x - 40) / 32;
	int yCell = (y - 17) / 32;
	if (xCell > m_itemHorzNum - 1)
		return ITEM_INFO();

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		if (drawCount == yCell * m_itemHorzNum + xCell)
			return *it;

		++it;
		++drawCount;
		++itemCount;
	}

	return ITEM_INFO();
}

ITEM_INFO UIItemBaseWnd::GetItemInfo(int sequence)
{
	int count = 0;
	const int sequence2 = sequence;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (count == sequence2)
			return *it;
		++it;
		++count;
	}
	return ITEM_INFO();
}


void UIItemBaseWnd::RecalcScrbarPos()
{
	if (m_scrollBar) {
		int numCanDisplayItem = m_itemVertNum;
		int numLine = (m_itemList.size() % m_itemHorzNum != 0) + m_itemList.size() / m_itemHorzNum;
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
int UIItemBaseWnd::GetCurItem(int x, int y)
{
	if (x < 40 || y < 17 || y >= m_itemVertNum * 32 + 17)
		return -1;

	int xCell = (x - 40) / 32;
	int yCell = (y - 17) / 32;
	if (xCell > m_itemHorzNum - 1)
		xCell = m_itemHorzNum - 1;

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		if (drawCount == yCell * m_itemHorzNum + xCell)
			return itemCount;

		++it;
		++drawCount;
		++itemCount;
	}

	return -1;
}
int UIItemBaseWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_RESIZE: {
		if (val1 == RT_DOWNRIGHT) {
			int width = val2;
			int height = val3;
			if (width > 320)
				width = 320;
			if (height > 240)
				height = 240;
			if (width < 232)
				width = 232;
			if (height < 102)
				height = 102;

			height = ((height - 38) / 32) * 32 + 38;
			width = ((width - 280) / 32) * 32 + 280;
			if (width != m_w || height != m_h) {
				Resize(width, height);
				m_itemHorzNum = (width - 40) / 32;
				m_itemVertNum = (height - 38) / 32;
				m_resizer->Move(m_w - 15, m_h - 15);
				m_scrollBar->Move(m_w - 15, 17);
				m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
				RecalcScrbarPos();
			}
			g_windowMgr.UpdateSnapInfo(this);
		}
		return 0;
	}
	case UM_COMMAND:
	case UM_REFRESHITEM:
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
	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
}

void UIItemBaseWnd::OnWheel(int wheel)
{
	if (m_vertScrEnabled) {
		int scrollNum = m_itemVertNum - 1;
		if (scrollNum >= 3)
			scrollNum = 3;
		if (wheel < 0)
			SendMsg(m_scrollBar, UM_SCROLLVERT, scrollNum, 0, 0, 0);
		else
			SendMsg(m_scrollBar, UM_SCROLLVERT, -scrollNum, 0, 0, 0);
	}
}

void UIItemBaseWnd::StoreInfo() {}

void UIItemBaseWnd::DragAndDrop(int x, int y, const DRAG_INFO* const dragInfo)
{
	g_modeMgr.GetCurMode()->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}

UIItemShopWnd::UIItemShopWnd()
{
	m_resizer = NULL;
	m_scrollBar = NULL;
	m_viewOffset = 0;
	m_vertScrEnabled = 0;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_drawNumItem = 0;
	m_isSellMode = 0;
	m_info = NULL;
	m_barHeight2 = 0;
	m_isPointShop = 0;
	m_curItem = -1;
	m_isBasket = 1;
	m_barHeight = 29;
}

UIItemShopWnd::~UIItemShopWnd()
{}

void UIItemShopWnd::OnCreate(int cx, int cy)
{
	m_titleText = new UIStaticText;
	m_titleText->Create(70, 15);
	m_titleText->SetText(MsgStr(MSI_ITEM_STORE), 0);
	m_titleText->Move(16, 2);
	m_titleText->m_drawTwice = 1;
	AddChild(m_titleText);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWNRIGHT);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_dragAllCheckBox = new UICheckBox;
	m_dragAllCheckBox->Create(200, 12);
	m_dragAllCheckBox->SetBitmap2(
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_1.bmp",
		"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\checkbox_0.bmp",
		MsgStr(MSI_NOQUESTION_ITEMCOUNT));
	m_dragAllCheckBox->Move(12, m_h - 20);
	m_dragAllCheckBox->SetCheck(g_windowMgr.m_isDragAll);
	AddChild(m_dragAllCheckBox);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - m_barHeight2 - m_barHeight - 17);
	AddChild(m_scrollBar);

	m_itemHorzNum = 1;
	m_itemVertNum = (cy - m_barHeight2 - m_barHeight - 17) / 32;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIItemShopWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);

		char tmp[10];
		sprintf(tmp, "Zeny");
		if (m_isPointShop) {
			if (g_serviceType == ServiceIndonesia) {
				sprintf(tmp, "Koin");
			} else if (g_serviceType == ServiceKorea) {
				if (g_isGravityID)
					sprintf(tmp, "Hancoin");
				else
					sprintf(tmp, MsgStr(MSI_CASH));
			} else {
				sprintf(tmp, "Point");
			}
		}

		char moneyTxt[64];
		char itemHelp[200];
		if (m_isSellMode) {
			if (itemInfo.m_price != itemInfo.m_realPrice) {
				sprintf(itemHelp, "%s %d %s %d -> %s %s",
					itemName, itemInfo.m_num, MsgStr(MSI_EA4),
					itemInfo.m_price, GetNumberText(itemInfo.m_realPrice, moneyTxt), tmp);
			} else {
				sprintf(itemHelp, "%s %d %s %s %s",
					itemName, itemInfo.m_num, MsgStr(MSI_EA4),
					GetNumberText(itemInfo.m_realPrice, moneyTxt), tmp);
			}
		} else {
			if (itemInfo.m_price != itemInfo.m_realPrice) {
				sprintf(itemHelp, "%s %d -> %s %s",
					itemName, itemInfo.m_price,
					GetNumberText(itemInfo.m_realPrice, moneyTxt), tmp);
			} else {
				sprintf(itemHelp, "%s %s %s",
					itemName, GetNumberText(itemInfo.m_realPrice, moneyTxt), tmp);
			}
		}

		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

void UIItemShopWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int titleLoop = 0; titleLoop < titleMidCount; ++titleLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
	}

	if (m_h == 17)
		return;

	int sideCount = (m_h - m_barHeight - 17) / 8 + ((m_h - m_barHeight - 17) % 8 != 0);
	for (int leftLoop = 0; leftLoop < sideCount; ++leftLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
		DrawBitmap(0, leftLoop * 8 + 17, bmp, false);
	}

	sideCount = (m_h - m_barHeight - 17) / 8 + ((m_h - m_barHeight - 17) % 8 != 0);
	for (int rightLoop = 0; rightLoop < sideCount; ++rightLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, rightLoop * 8 + 17, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17, (m_w - 60) % 32, m_h - m_barHeight - 17, RGB(rr, gg, bb));
		DrawBox(20, ((m_h - m_barHeight - 17) / 32) * 32 + 17, m_w - 24, (m_h - m_barHeight - 17) % 32, RGB(rr, gg, bb));
	}

	char* bmpName[2][3] = {
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp",
		},
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp",
		},
	};

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (int btnLoop = 0; btnLoop < btnMidCount; ++btnLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][2]), false);
		DrawBitmap(btnLoop * 21 + 21, m_h - bmp->m_height, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		int itemCols = (m_w - 40) / 32;
		int itemRows = (m_h - m_barHeight2 - m_barHeight - 17) / 32;
		for (int itemY = 0; itemY < itemRows; ++itemY)
			DrawItemFrame(40, itemY * 32 + 17);

		if (m_isPointShop)
			itemRows = (m_h - m_barHeight - 17) / 32;

		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(72, 17, itemCols * 32 - 32, itemRows * 32, RGB(rr, gg, bb));

		if (m_isPointShop)
			DrawBox(40, itemRows * 32 - 15, 32, 32, RGB(rr, gg, bb));
	}

	{
		char priceText[64];
		char moneyTxt[64];
		char tmp[5];
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		int drawCount = 0;
		int itemCount = 0;
		std::list<ITEM_INFO>::iterator it = m_itemList.begin();
		while (it != m_itemList.end()) {
			int skipLimit = m_itemHorzNum * m_viewOffset;
			if (itemCount < skipLimit) {
				++it;
				++itemCount;
				continue;
			}

			if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
				if (it->m_isDamaged) {
					++itemCount;
					++it;
					continue;
				}
			}

			int itemX = (drawCount % m_itemHorzNum) * 32 + 40;
			int itemY = (drawCount / m_itemHorzNum) * 32 + 17;

			if (m_curItem == itemCount) {
				g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
				DrawBox(itemX + 32, itemY + 2, 191, 28, RGB(rr, gg, bb));
			}

			DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);
			DrawItemText(itemX + 36, itemY + 12, *it, 0, 0, 12);

			sprintf(tmp, "Z");
			if (m_isPointShop) {
				if (g_serviceType == ServiceIndonesia) {
					sprintf(tmp, "K");
				} else if (g_serviceType == ServiceKorea) {
					if (g_isGravityID)
						sprintf(tmp, "H");
					else
						sprintf(tmp, "C");
				} else {
					sprintf(tmp, "P");
				}
			}

			int priceTextLen;
			if (it->m_price != it->m_realPrice) {
				priceTextLen = sprintf(priceText, " %d -> %s %s", it->m_price, GetNumberText(it->m_realPrice, moneyTxt), tmp);
			} else {
				priceTextLen = sprintf(priceText, " %s %s", GetNumberText(it->m_price, moneyTxt), tmp);
			}

			TextOutWithRightAlign(m_w - 18, itemY + 12, priceText, priceTextLen, 0, 12, 0);

			if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
				break;

			++it;
			++drawCount;
			++itemCount;
		}
	}

	if (m_isPointShop) {
		char priceText[350];

		int textLen;
		if (g_serviceType == ServiceKorea && g_isGravityID) {
			textLen = sprintf(
				priceText,
				MsgStr(MSI_POINT_SHOP_NHN),
				PolishText(g_session.GetCharName(), 100, 0, 12, false),
				g_session.m_cashPoint);
		} else {
			textLen = sprintf(
				priceText,
				MsgStr(MSI_POINT_SHOP),
				PolishText(g_session.GetCharName(), 100, 0, 12, false),
				g_session.m_cashPoint);
		}

		TextOutA(12, m_h - 21, priceText, textLen, 0, 12, 0);

		textLen = sprintf(
			priceText,
			MsgStr(MSI_POINT_SHOP2),
			PolishText(g_session.GetCharName(), 100, 0, 12, false),
			g_session.m_freePoint);
		TextOutA(12, m_h - m_barHeight - 21, priceText, textLen, 0, 12, 0);
	}
}

void UIItemShopWnd::OnLBtnDblClk(int x, int y)
{
	if (!m_isSellMode || !g_windowMgr.m_isDragAll) return;
	UIItemSellWnd* sell = g_windowMgr.GetItemSellWnd();
	if (!sell) return;

	int clickX = x;
	int clickY = y;
	OnLBtnDown(clickX, clickY);
	CGameMode* mode = g_modeMgr.GetGameMode();
	sell->DragAndDrop(clickX, clickY, mode->GetDragInfo());
}

void UIItemShopWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		g_modeMgr.GetGameMode();
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}

void UIItemShopWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
	int oldCurItem = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (oldCurItem != m_curItem)
		Invalidate();
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* mode = g_modeMgr.GetCurMode();
		mode->SendMsg(MM_BEGIN_DRAG_FROM_ITEMSHOPWND, (int)&itemInfo, 0, 0);
	} else {
		ITEM_INFO itemInfo = GetItemInfo(m_curItem);
		if (itemInfo.m_itemIndex) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 1, 0, 0);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_BEGIN_DRAG_FROM_ITEMSHOPWND, (int)&itemInfo, 0, 0);
		}
	}
}

bool UIItemShopWnd::IsExistItem(const char* itemName)
{
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (it->m_itemName == itemName) return true;
		++it;
	}
	return false;
}

void UIItemShopWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	switch (dragInfo->m_dragType) {
	case DT_FROM_ITEMPURCHASEWND: {
		if (m_isSellMode)
			return;

		if (dragInfo->m_numDragItem == 1) {
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		}

		g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
		UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
		wnd->SendMsg(0, UM_SETSTATE, 3, 0, 0, 0);
		return;
	}
	case DT_FROM_ITEMSELLWND: {
		if (!m_isSellMode)
			return;

		if (dragInfo->m_numDragItem == 1 || g_windowMgr.m_isDragAll) {
			SendMsg(0, UM_DROP, 1, 0, 0, 0);
			return;
		}

		g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
		UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
		wnd->SendMsg(0, UM_SETSTATE, 4, 0, 0, 0);
		return;
	}
	default:
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		return;
	}
}
int UIItemShopWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message)
	{
	case UM_SETINFO:
		m_info = (ItemShopWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWNRIGHT, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		if (val1 == 0x93)
		{
			g_windowMgr.m_isDragAll = val2;
		}
		break;
	case UM_DROP:
		switch (val1) {
		case 0: {
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			DRAG_INFO* dragInfo = gameMode->GetDragInfo();
			ITEM_INFO itemInfo;
			itemInfo.CopyItemInfo(dragInfo);
			g_session.DecPurchaseItem(itemInfo);
			UIWindow* wnd = g_windowMgr.GetItemPurchaseWnd();
			if (wnd)
				wnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
			SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
			gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		}
		break;
		case 1: {
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			DRAG_INFO* dragInfo = gameMode->GetDragInfo();
			ITEM_INFO itemInfo;
			itemInfo.CopyItemInfo(dragInfo);
			Trace("To player shop: m_itemName == %s itemIndex == %d itemNum = %d",
				itemInfo.m_itemName.c_str(), itemInfo.m_itemIndex, itemInfo.m_num);
			g_session.AddShopItemFromSell(itemInfo);
			g_session.DecSellItem(itemInfo);
			UIWindow* wnd = g_windowMgr.GetItemSellWnd();
			if (wnd)
				wnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
			SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
			gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		}
		break;
		}
		break;
	case UM_SETTITLE:
		m_isPointShop = val2;
		if (val1) {
			m_isSellMode = 1;
			m_drawNumItem = 1;
			m_titleText->SetTextWithNewWidth(MsgStr(MSI_ITEMSYOUCANSELL));
			m_dragAllCheckBox->m_isDisabled = 0;
		} else {
			m_isSellMode = 0;
			m_drawNumItem = 0;
			m_titleText->SetTextWithNewWidth(MsgStr(MSI_SHOPITEMS));
			m_dragAllCheckBox->m_isDisabled = 1;
		}
		if (m_isPointShop)
			m_barHeight2 = 20;
		else
			m_barHeight2 = 0;
		m_itemVertNum = (m_h - m_barHeight - m_barHeight2 - 17) / 32;
		m_scrollBar->Resize(m_w - 15, m_h - m_barHeight - m_barHeight2 - 17);
		Invalidate();
		break;

	case UM_RESIZE: {
		if (val1 != RT_DOWNRIGHT)
			break;

		int width = val2;
		int height = val3;
		if (width > 320)
			width = 320;
		if (height > 336)
			height = 336;
		if (width < 232)
			width = 232;
		if (height < 120)
			height = 120;

		width = ((width - 280) / 32) * 32 + 280;
		height = ((height - 120) / 32) * 32 + 120;
		if (width != m_w || height != m_h)
		{
			Resize(width, height);
			m_itemHorzNum = 1;
			m_itemVertNum = (height - m_barHeight - m_barHeight2 - 17) / 32;
			m_resizer->Move(m_w - 15, m_h - 15);
			m_scrollBar->Move(m_w - 15, 14);
			m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - m_barHeight - m_barHeight2 - 17);
			m_dragAllCheckBox->Move(12, m_h - 20);
			RecalcScrbarPos();
		}
		g_windowMgr.UpdateSnapInfo(this);
		break;
	}

	case UM_REFRESHITEM: {
		m_itemList.clear();
		int numItem = g_session.GetNumShopItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetShopItemInfoBySequence(i);
			if (itemInfo.m_itemIndex)
				m_itemList.push_back(itemInfo);
		}
		Invalidate();
		RecalcScrbarPos();
		break;
	}
	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIItemShopWnd::StoreInfo()
{
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}

UIMerchantItemMyShopWnd::UIMerchantItemMyShopWnd()
{
	m_drawNumItem = 1;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_isBasket = 1;
	m_info = NULL;
}

UIMerchantItemMyShopWnd::~UIMerchantItemMyShopWnd()
{}

void UIMerchantItemMyShopWnd::OnCreate(int cx, int cy)
{
	m_titleText = new UIStaticText;
	m_titleText->Create(240, 15);
	m_titleText->SetText(MsgStr(MSI_ITEM_MY_STORE), 0);
	m_titleText->Move(16, 2);
	m_titleText->m_drawTwice = 1;
	AddChild(m_titleText);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWNRIGHT);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	std::string pathName("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\");
	char* buttonName[][3] = {
		{ "btn_close", "btn_close_a", "btn_close_b", },
	};
	int ids[] = { ID_CLOSE };
	int pos[][2] = {
		{ m_w - 61, m_h - 24 },
	};

	for (int index = 0; index < 1; index++) {
		m_button[index] = new UIBitmapButton;
		m_button[index]->SetBitmapName((pathName + buttonName[index][0] + ".bmp").c_str(), BS_NORMAL);
		m_button[index]->SetBitmapName((pathName + buttonName[index][1] + ".bmp").c_str(), BS_MOUSEON);
		m_button[index]->SetBitmapName((pathName + buttonName[index][2] + ".bmp").c_str(), BS_PRESSED);
		m_button[index]->Create(m_button[index]->GetBitmapWidth(), m_button[index]->GetBitmapHeight());
		m_button[index]->Move(pos[index][0], pos[index][1]);
		m_button[index]->SetId(ids[index]);
		AddChild(m_button[index]);
	}

	m_itemHorzNum = (cx - 40) / 32;
	m_itemVertNum = (cy - 38) / 32;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIMerchantItemMyShopWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		g_modeMgr.GetGameMode();
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}

void UIMerchantItemMyShopWnd::OnLBtnDown(int x, int y)
{
	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
	}
}

void UIMerchantItemMyShopWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

void UIMerchantItemMyShopWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);
		char moneyTxt[64];
		char itemHelp[200];
		sprintf(itemHelp, "%s %s Zeny", itemName, GetNumberText(itemInfo.m_realPrice, moneyTxt));

		g_modeMgr.GetCurMode()->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

int UIMerchantItemMyShopWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message)
	{
	case UM_SETTITLE: {
		char title[120];
		sprintf(title, "%s : %s", MsgStr(MSI_ITEM_MY_STORE), g_session.m_myShopName.c_str());
		m_titleText->SetTextWithShorten(title);
		break;
	}

	case UM_SETINFO:
		m_info = (MerchantItemMyShopWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWNRIGHT, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_RESIZE: {
		if (val1 != RT_DOWNRIGHT)
			break;

		int width = val2;
		int height = val3;
		if (width > 320)
			width = 320;
		if (height > 240)
			height = 240;
		if (width < 232)
			width = 232;
		if (height < 120)
			height = 120;

		width = ((width - 280) / 32) * 32 + 280;
		height = ((height - 120) / 32) * 32 + 120;
		if (width != m_w || height != m_h)
		{
			Resize(width, height);
			m_itemHorzNum = (width - 40) / 32;
			m_itemVertNum = (height - 38) / 32;
			m_resizer->Move(m_w - 15, m_h - 15);
			m_scrollBar->Move(m_w - 15, 14);
			m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
			int pos[2] = { m_w - 61, m_h - 24 };
			m_button[0]->Move(pos[0], pos[1]);
			RecalcScrbarPos();
		}
		g_windowMgr.UpdateSnapInfo(this);
		break;
	}

	case UM_COMMAND:
		if (val1 == ID_CLOSE)
		{
			CGameMode* gameMode = (CGameMode*)g_modeMgr.GetCurMode();
			gameMode->SendMsg(MM_REQ_CLOSESTORE, 0, 0, 0);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMMYSHOPWND);
			g_session.ClearMerchantMyShopItem();
		}
		break;

	case UM_REFRESHITEM: {
		m_itemList.erase(m_itemList.begin(), m_itemList.end());
		int numItem = g_session.GetNumMerchantMyShopItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetMerchantMyShopItemInfoBySequence(i);
			if (itemInfo.m_itemIndex)
				m_itemList.push_back(itemInfo);
		}
		Invalidate();
		RecalcScrbarPos();
		break;
	}
	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIMerchantItemShopWnd::UIMerchantItemShopWnd()
{
	m_drawNumItem = 1;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_isBasket = 1;
	m_info = NULL;
	m_curItem = -1;
}

UIMerchantItemShopWnd::~UIMerchantItemShopWnd()
{}

void UIMerchantItemShopWnd::OnCreate(int cx, int cy)
{
	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWN);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	m_itemHorzNum = 1;
	m_itemVertNum = (cy - 38) / 32;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIMerchantItemShopWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int titleLoop = 0; titleLoop < titleMidCount; ++titleLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
	}

	if (m_h == 17)
		return;

	int sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int leftLoop = 0; leftLoop < sideCount; ++leftLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
		DrawBitmap(0, leftLoop * 8 + 17, bmp, false);
	}

	sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int rightLoop = 0; rightLoop < sideCount; ++rightLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, rightLoop * 8 + 17, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17, (m_w - 60) % 32, m_h - 38, RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17, m_w - 24, (m_h - 38) % 32, RGB(rr, gg, bb));
	}

	char* bmpName[2][3] = {
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp",
		},
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp",
		},
	};

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (int btnLoop = 0; btnLoop < btnMidCount; ++btnLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][2]), false);
		DrawBitmap(btnLoop * 21 + 21, m_h - bmp->m_height, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		int itemCols = (m_w - 40) / 32;
		int itemRows = (m_h - 38) / 32;
		for (int itemY = 0; itemY < itemRows; ++itemY)
			DrawItemFrame(40, itemY * 32 + 17);

		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(72, 17, itemCols * 32 - 32, itemRows * 32, RGB(rr, gg, bb));
	}

	{
		char temp[256];
		const NamePair& actorName = g_modeMgr.GetGameMode()->GetActorName(m_aid);
		if (actorName.cName.size() != 0) {
			sprintf(temp, "%s - %s", MsgStr(MSI_ITEM_MERCHANT_STORE), actorName.cName.c_str());
		} else {
			g_windowMgr.AddToNameWaitingWindowList(this);
			sprintf(temp, "%s", MsgStr(MSI_ITEM_MERCHANT_STORE));
		}
		TextOutA(17, 3, temp, 0, 0, 12, 0xffffff);
		TextOutA(16, 2, temp, 0, 0, 12, 0);
	}

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		int itemX = (drawCount % m_itemHorzNum) * 32 + 40;
		int itemY = (drawCount / m_itemHorzNum) * 32 + 17;

		if (m_curItem == itemCount) {
			unsigned long rr;
			unsigned long gg;
			unsigned long bb;
			g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
			DrawBox(itemX + 32, itemY + 2, 191, 28, RGB(rr, gg, bb));
		}

		DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);

		char priceText[128];
		char moneyTxt[128];
		int priceWidth = GetTextWidth("Z", 0, 0, 12, false);
		TextOutA(m_w - priceWidth - 18, itemY + 12, "Z", 0, 0, 12, 0);

		if (it->m_price != it->m_realPrice) {
			sprintf(priceText, " %d -> %s ", it->m_price, GetNumberText(it->m_realPrice, moneyTxt));
		} else {
			sprintf(priceText, " %s ", GetNumberText(it->m_price, moneyTxt));
		}

		priceWidth += GetTextWidth(priceText, 0, 0, 12, false);

		if (it->m_price >= 100000000) {
			TextOutWithTwoColors(m_w - priceWidth - 18, itemY + 12, priceText, 0x64c8c8, 0, 0, 0, 12);
		} else if (it->m_price >= 10000000) {
			TextOutA(m_w - priceWidth - 18, itemY + 12, priceText, 0, 0, 12, 0xff);
		} else if (it->m_price >= 1000000) {
			TextOutWithTwoColors(m_w - priceWidth - 18, itemY + 12, priceText, 0xff00, 0, 0, 0, 12);
		} else if (it->m_price >= 100000) {
			TextOutA(m_w - priceWidth - 18, itemY + 12, priceText, 0, 0, 12, 0xff0000);
		} else if (it->m_price >= 10000) {
			TextOutA(m_w - priceWidth - 18, itemY + 12, priceText, 0, 0, 12, 0xff1eff);
		} else if (it->m_price >= 1000) {
			TextOutWithTwoColors(m_w - priceWidth - 18, itemY + 12, priceText, 0xffff, 0xff, 0, 0, 12);
		} else if (it->m_price >= 100) {
			TextOutWithTwoColors(m_w - priceWidth - 18, itemY + 12, priceText, 0xffff00, 0xff0000, 0, 0, 12);
		} else if (it->m_price >= 10) {
			TextOutWithTwoColors(m_w - priceWidth - 18, itemY + 12, priceText, 0xc800c8, 0xff0000, 0, 0, 12);
		} else {
			TextOutWithTwoColors(m_w - priceWidth - 18, itemY + 12, priceText, 0xffff00, 0, 0, 0, 12);
		}

		DrawItemTextRect(itemX + 36, itemY + 12, m_w - priceWidth - itemX - 66, *it, 0, 0, 12);

		if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
			break;

		++it;
		++drawCount;
		++itemCount;
	}
}

void UIMerchantItemShopWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		g_modeMgr.GetGameMode();
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}

void UIMerchantItemShopWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
	int oldCurItem = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (oldCurItem != m_curItem)
		Invalidate();
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* mode = g_modeMgr.GetCurMode();
		mode->SendMsg(MM_BEGIN_DRAG_FROM_MERCHANTITEMSHOPWND, (int)&itemInfo, 0, 0);
	} else {
		ITEM_INFO itemInfo = GetItemInfo(m_curItem);
		if (itemInfo.m_itemIndex) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 1, 0, 0);
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_BEGIN_DRAG_FROM_MERCHANTITEMSHOPWND, (int)&itemInfo, 0, 0);
		}
	}
}

void UIMerchantItemShopWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemHelp[128];
		g_modeMgr.GetCurMode()->MakeHelpBalloon(itemInfo.GetItemName(itemHelp), gx, gy,
			g_session.GetItemDisColor(&itemInfo));
	} else {
		itemInfo = GetItemInfo(GetCurItem(x, y));
		if (itemInfo.m_itemIndex) {
			int gx = 72;
			int gy = ((y - 17) / 32) * 32 + 1;
			GetGlobalCoor(gx, gy);

			char itemHelp[128];
			g_modeMgr.GetCurMode()->MakeHelpBalloon(itemInfo.GetItemName(itemHelp), gx - 2, gy + 24,
				g_session.GetItemDisColor(&itemInfo));
		}
	}
}

void UIMerchantItemShopWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

int UIMerchantItemShopWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message)
	{
	case UM_SETINFO:
		m_info = (MerchantItemShopWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWN, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_SETNAID:
		m_aid = val1;
		break;

	case UM_SETTITLE:
		m_titleText->SetTextWithNewWidth(MsgStr(MSI_SHOPITEMS));
		break;

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		DRAG_INFO* dragInfo = gameMode->GetDragInfo();
		ITEM_INFO itemInfo;
		itemInfo.CopyItemInfo(dragInfo);
		g_session.DecMerchantPurchaseItem(itemInfo);
		UIWindow* wnd = g_windowMgr.GetMerchantItemPurchaseWnd();
		if (wnd)
			wnd->SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
		SendMsg(0, UM_REFRESHITEM, 0, 0, 0, 0);
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}
	case UM_RESIZE: {
		if (val1 != RT_DOWN)
			break;

		int width = val2;
		int height = val3;
		if (width > 320)
			width = 320;
		if (height > 336)
			height = 336;
		if (width < 232)
			width = 232;
		if (height < 120)
			height = 120;

		width = ((width - 280) / 32) * 32 + 280;
		height = ((height - 120) / 32) * 32 + 120;
		if (width != m_w || height != m_h)
		{
			Resize(width, height);
			m_itemHorzNum = 1;
			m_itemVertNum = (height - 38) / 32;
			m_resizer->Move(m_w - 15, m_h - 15);
			m_scrollBar->Move(m_w - 15, 14);
			m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
			RecalcScrbarPos();
		}
		g_windowMgr.UpdateSnapInfo(this);
		break;
	}

	case UM_REFRESHITEM: {
		m_itemList.erase(m_itemList.begin(), m_itemList.end());
		int numItem = g_session.GetNumMerchantShopItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetMerchantShopItemInfoBySequence(i);
			if (itemInfo.m_itemIndex)
				m_itemList.push_back(itemInfo);
		}
		Invalidate();
		RecalcScrbarPos();
		break;
	}
	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIMerchantItemShopWnd::DragAndDrop(int x, int y, const DRAG_INFO* const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}
	if (dragInfo->m_dragType != DT_FROM_MERCHANTITEMPURCHASEWND) {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		return;
	}
	if (dragInfo->m_numDragItem == 1) {
		SendMsg(0, UM_DROP, 0, 0, 0, 0);
		return;
	}
	g_windowMgr.ShowWindow((WINDOWID)0xf, 1);
	UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
	wnd->SendMsg(0, UM_SETSTATE, 0xb, 0, 0, 0);
}

UIItemPurchaseWnd::UIItemPurchaseWnd()
{
	m_drawNumItem = 1;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_isBasket = 1;
	m_isPointShop = 0;
	m_info = NULL;
	m_curItem = -1;
}

UIItemPurchaseWnd::~UIItemPurchaseWnd()
{}

void UIItemPurchaseWnd::OnCreate(int cx, int cy)
{
	m_freePoint = new UIEditCtrl;
	m_freePoint->Create(64, 16);
	m_freePoint->SetTextLimit(9);
	m_freePoint->Move(-100, -100);
	m_freePoint->SetFrameColor(232, 232, 232);
	m_freePoint->SetText("0");
	AddChild(m_freePoint);
	g_windowMgr.SetFocusEdit(m_freePoint);
	m_defPushId = 0xcd;

	m_priceText = new UIStaticText;
	m_priceText->Create(160, 15);
	m_priceText->Move(12, m_h - 21);
	AddChild(m_priceText);

	UIStaticText* title = new UIStaticText;
	title->Create(160, 15);
	title->SetText(MsgStr(MSI_ITEMS_FOR_BUY), 0);
	title->Move(16, 2);
	title->m_drawTwice = 1;
	AddChild(title);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWN);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_buy", "btn_buy_a", "btn_buy_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ m_w - 106, m_h - 24 },
		{ m_w - 61, m_h - 24 },
	};

	int i;
	for (i = 0; i < 2; ++i) {
		m_button[i] = new UIBitmapButton;
		m_button[i]->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		m_button[i]->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		m_button[i]->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		m_button[i]->Create(m_button[i]->GetBitmapWidth(), m_button[i]->GetBitmapHeight());
		m_button[i]->Move(pos[i][0], pos[i][1]);
		m_button[i]->SetId(ids[i]);
		AddChild(m_button[i]);
	}

	m_itemVertNum = (cy - 38) / 32;
	m_itemHorzNum = 1;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIItemPurchaseWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		g_modeMgr.GetGameMode();
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}

void UIItemPurchaseWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
	int oldCurItem = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (oldCurItem != m_curItem)
		Invalidate();
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* m = g_modeMgr.GetCurMode();
		m->SendMsg(MM_BEGIN_DRAG_FROM_ITEMPURCHASEWND,
			(int)itemInfo.m_itemName.c_str(), itemInfo.m_price, itemInfo.m_itemIndex);
	} else {
		ITEM_INFO itemInfo = GetItemInfo(m_curItem);
		if (itemInfo.m_itemIndex) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 1, 0, 0);
			CMode* m = g_modeMgr.GetCurMode();
			m->SendMsg(MM_BEGIN_DRAG_FROM_ITEMPURCHASEWND,
				(int)itemInfo.m_itemName.c_str(), itemInfo.m_price, itemInfo.m_itemIndex);
		}
	}
}

void UIItemPurchaseWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);

		char tmp[10];
		sprintf(tmp, "Zeny");
		if (m_isPointShop) {
			if (g_serviceType == ServiceIndonesia) {
				sprintf(tmp, "Koin");
			} else if (g_serviceType == ServiceKorea) {
				if (g_isGravityID)
					sprintf(tmp, "Hancoin");
				else
					sprintf(tmp, MsgStr(MSI_CASH));
			} else {
				sprintf(tmp, "Point");
			}
		}

		char moneyTxt[64];
		char itemHelp[200];
		if (itemInfo.m_price != itemInfo.m_realPrice) {
			sprintf(itemHelp, "%s %d %s %d -> %s %s",
				itemName, itemInfo.m_num, MsgStr(MSI_EA4),
				itemInfo.m_price, GetNumberText(itemInfo.m_realPrice, moneyTxt), tmp);
		} else {
			sprintf(itemHelp, "%s %d %s %s %s",
				itemName, itemInfo.m_num, MsgStr(MSI_EA4),
				GetNumberText(itemInfo.m_realPrice, moneyTxt), tmp);
		}

		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

void UIItemPurchaseWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	CGameMode* gameMode = g_modeMgr.GetGameMode();
	if (dragInfo->m_dragType == DT_FROM_ITEMSHOPWND) {
		switch (dragInfo->m_dragItemType) {
		case 4:
		case 5:
		case 8:
		case 9:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		default:
			g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
			UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
			wnd->SendMsg(0, UM_SETSTATE, 1, 0, 0, 0);
			return;
		}
	}

	gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}
void UIItemPurchaseWnd::OnDraw()
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

	{
		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);

		DrawBox(20, 17,
			(m_w - 60) % 32, m_h - 38,
			RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17,
			m_w - 24, (m_h - 38) % 32,
			RGB(rr, gg, bb));
	}

	char* bmpName[2][3] = {
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp",
		},
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp",
		},
	};

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);


	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	loopCnt = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][2]), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	{
		int itemCols = (m_w - 40) / 32;
		int itemRows = (m_h - 38) / 32;

		for (int hLoop = 0; hLoop < itemRows; ++hLoop)
		{



			if (m_isPointShop)
			{
				DrawBox(40, hLoop * 32 + 17, 32, 32, 0xffffff);
				DrawItemFrame(40, 17);
			}
			else
			{
				DrawItemFrame(40, hLoop * 32 + 17);
			}
		}

		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(72, 17, itemCols * 32 - 32, itemRows * 32, RGB(rr, gg, bb));
	}

	{
		char moneyTxt[64];
		char priceText[64];
		int drawCount = 0;
		int itemCount = 0;
		std::list<ITEM_INFO>::iterator it = m_itemList.begin();
		while (it != m_itemList.end()) {
			int skipLimit = m_itemHorzNum;
			skipLimit *= m_viewOffset;
			if (itemCount < skipLimit) {
				++it;
				++itemCount;
				continue;
			} else if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
				if (it->m_isDamaged) {
					++itemCount;
					++it;
					continue;
				}
			}

			int itemX = (drawCount % m_itemHorzNum) * 32 + 40;
			int itemY = (drawCount / m_itemHorzNum) * 32 + 17;

			if (m_curItem == itemCount) {
				unsigned long rr;
				unsigned long gg;
				unsigned long bb;
				g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
				DrawBox(itemX + 32, itemY + 2, 191, 28, RGB(rr, gg, bb));
			}

			DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);
			DrawItemText(itemX + 36, itemY + 12, *it, 0, 0, 12);

			char tmp[10];
			sprintf(tmp, "Zeny");
			if (m_isPointShop) {
				if (g_serviceType == ServiceIndonesia) {
					sprintf(tmp, "Koin");
				} else if (g_serviceType == ServiceKorea) {
					if (g_isGravityID)
						sprintf(tmp, "Hancoin");
					else
						sprintf(tmp, MsgStr(MSI_CASH));
				} else {
					sprintf(tmp, "Point");
				}
			}

			int priceTextLen;
			if (it->m_price != it->m_realPrice) {
				priceTextLen = sprintf(priceText, " %d -> %s %s", it->m_price,
					GetNumberText(it->m_realPrice, moneyTxt), tmp);
			} else {
				priceTextLen = sprintf(priceText, " %s %s",
					GetNumberText(it->m_price, moneyTxt), tmp);
			}

			TextOutWithRightAlign(m_w - 18, itemY + 12, priceText, priceTextLen, 0, 12, 0);

			if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
				break;

			it++;
			++drawCount;
			++itemCount;
		}
	}

	if (m_isPointShop) {
		TextOutA(12, m_h - 48, MsgStr(MSI_USE_FREE_POINT), 0, 0, 12, 0);
		TextOutA(179, m_h - 48, MsgStr(MSI_CASH), 0, 0, 12, 0);
		DrawBox(109, m_h - 51, 66, 18, 0x8e8e8e);
	}
}

int UIItemPurchaseWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETTITLE: {
		m_isPointShop = val1;
		if (m_isPointShop)
			m_freePoint->Move(110, m_h - 50);
		else
			m_freePoint->Move(-100, -100);

		char text[80];
		if (m_isPointShop) {
			if (g_serviceType == ServiceIndonesia) {
				sprintf(text, "Total : %d Koin", 0);
			} else if (g_serviceType == ServiceKorea) {
				if (g_isGravityID)
					sprintf(text, "Total : %d Hancoin", 0);
				else
					sprintf(text, "Total : %d Cash", 0);
			} else {
				sprintf(text, "Total : %d Point", 0);
			}
		} else {
			sprintf(text, "Total : %d Zeny", 0);
		}
		m_priceText->SetTextWithNewWidth(text);
		break;
	}

	case UM_SETINFO:
		m_info = (ItemPurchaseWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWN, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		DRAG_INFO* dragInfo = gameMode->GetDragInfo();

		switch (dragInfo->m_dragItemType) {
		case 4:
		case 5:
		case 8:
		case 9:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			if (g_session.GetPurchaseItemNum(dragInfo->m_dragItemName.c_str())) {
				gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_EQUIPITEM_OLNY_ONE), 0xff, 0, 0);
				return 0;
			}
			break;
		default:
			break;
		}

		ITEM_INFO itemInfo;
		itemInfo.CopyItemInfo(dragInfo);
		int totalPrice = g_session.GetPurchaseItemTotalPrice();
		bool enoughMoney;
		if (m_isPointShop) {
			if (totalPrice) {
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_CAN_BUY_ONLY_ONEITEM), 0xff, 0, 0);
			}
			g_session.ClearPurchaseItem();
			enoughMoney = g_session.m_cashPoint + g_session.m_freePoint >= dragInfo->m_dragItemRealPrice * dragInfo->m_numDragItem;
		} else {
			enoughMoney = g_session.GetGold() >= totalPrice + dragInfo->m_dragItemRealPrice * dragInfo->m_numDragItem;
		}

		int totalCount = 0;
		std::list<ITEM_INFO>::iterator it = m_itemList.begin();
		while (it != m_itemList.end()) {
			totalCount = 0;
			if (itemInfo.m_itemName == it->m_itemName) {
				totalCount = it->m_num;
				break;
			}
			++it;
		}

		int count = itemInfo.m_num + totalCount;
		if (count > 30000) {
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_LIMIT_BUY_ITEM), 0xff, 0, 0);
		} else {
			if (enoughMoney) {
				g_session.AddPurchaseItem(itemInfo);
				SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
			} else {
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				if (m_isPointShop)
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_BUY_CASH_FAIL_MONEY), 0xff, 0, 0);
				else
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_INSUFFICIENT_MONEY), 0xff, 0, 0);
			}
		}

		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	case UM_COMMAND: {
		CMode* gameMode = g_modeMgr.GetCurMode();
		switch (val1) {
		case ID_OK:
			if (m_isPointShop) {
				ITEM_INFO itemInfo = g_session.GetPurchaseItemInfoBySequence(0);
				if (itemInfo.IsValid()) {

					int freePoint = atoi(m_freePoint->GetText());
					if (freePoint < 0)
						freePoint = 0;
					if (freePoint > g_session.m_freePoint)
						freePoint = g_session.m_freePoint;
					if (freePoint % 100 > 0)
						freePoint = (freePoint / 100) * 100;

					if (freePoint > itemInfo.m_realPrice * itemInfo.m_num)
						freePoint = itemInfo.m_realPrice * itemInfo.m_num;

					char buf[10];
					itoa(freePoint, buf, 10);
					g_windowMgr.SetFocusEdit(m_freePoint);
					m_freePoint->SetText(buf);
					g_windowMgr.SetFocusEdit(NULL);

					char name[100];
					itemInfo.GetItemName(name);

					char msg[200];
					if (!freePoint && g_session.m_cashPoint < itemInfo.m_realPrice * itemInfo.m_num &&
						g_session.m_freePoint >= itemInfo.m_realPrice * itemInfo.m_num) {
							if (g_windowMgr.ErrorMsg(MsgStr(MSI_BUY_TO_FREE_POINT), ME_OK, TRUE, FALSE, 0))
								return 0;
					}
					if (freePoint) {
						sprintf(msg, MsgStr(MSI_BUY_RECONFIRM2), name, itemInfo.m_realPrice * itemInfo.m_num - freePoint, freePoint);
					} else {
						sprintf(msg, MsgStr(MSI_BUY_RECONFIRM), name, itemInfo.m_realPrice * itemInfo.m_num);
					}

					if (g_windowMgr.ErrorMsg(msg, ME_YESNO, TRUE, TRUE, 0) == ID_YES) {
						g_windowMgr.SetFocusEdit(m_freePoint);
						m_freePoint->SetText("0");
						gameMode->SendMsg(MM_REQ_BUY_CASH_POINT, freePoint, 0, 0);
						SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
					}
				}
				break;
			}

			gameMode->SendMsg(MM_REQ_PURCHASE, 0, 0, 0);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			g_session.ClearDealItems();
			return 0;

		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_ITEMSHOPWND);
			g_windowMgr.DeleteWindow(WID_ITEMPURCHASEWND);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			g_session.ClearDealItems();
			return 0;

		case ID_APPLY:
			if (m_isPointShop) {
				int freePoint = atoi(m_freePoint->GetText());
				if (freePoint < 0)
					freePoint = 0;
				if (freePoint > g_session.m_freePoint)
					freePoint = g_session.m_freePoint;
				if (freePoint % 100 > 0)
					freePoint = (freePoint / 100) * 100;

				char buf[10];
				itoa(freePoint, buf, 10);
				m_freePoint->SetText(buf);
				g_windowMgr.SetFocusEdit(NULL);
				return 0;
			}
			break;
		}
		break;
	}

	case UM_RESIZE: {
		if (val1 == RT_DOWN) {
			int width = val2;
			int height = val3;
			if (width > 320)
				width = 320;
			if (height > 336)
				height = 336;
			if (width < 232)
				width = 232;
			if (height < 120)
				height = 120;

			width = ((width - 280) / 32) * 32 + 280;
			height = ((height - 120) / 32) * 32 + 120;
			if (width != m_w || height != m_h) {
				Resize(width, height);
				m_itemHorzNum = 1;
				m_itemVertNum = (height - 38) / 32;
				m_resizer->Move(m_w - 15, m_h - 15);
				m_scrollBar->Move(m_w - 15, 14);
				m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
				RecalcScrbarPos();
			}

			m_priceText->Move(12, m_h - 20);
			if (m_isPointShop)
				m_freePoint->Move(110, m_h - 50);
			else
				m_freePoint->Move(-100, -100);


			int pos[2][2] = {
				{ m_w - 106, m_h - 24 },
				{ m_w - 61, m_h - 24 },
			};
			for (int i = 0; i < 2; ++i)
				m_button[i]->Move(pos[i][0], pos[i][1]);

			g_windowMgr.UpdateSnapInfo(this);
		}
		break;
	}

	case UM_REFRESHITEM: {
		int totalPrice = 0;
		m_itemList.erase(m_itemList.begin(), m_itemList.end());

		int numItem = g_session.GetNumPurchaseItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetPurchaseItemInfoBySequence(i);
			if (itemInfo.IsValid()) {
				totalPrice += itemInfo.m_realPrice * itemInfo.m_num;
				m_itemList.push_back(itemInfo);
			}
		}

		char text[80];
		char moneyTxt[64];
		if (m_isPointShop) {
			if (g_serviceType == ServiceIndonesia) {
				sprintf(text, "Total : %s Koin", GetNumberText(totalPrice, moneyTxt));
			} else if (g_serviceType == ServiceKorea) {
				if (g_isGravityID)
					sprintf(text, "Total : %s Hancoin", GetNumberText(totalPrice, moneyTxt));
				else
					sprintf(text, "Total : %s Cash", GetNumberText(totalPrice, moneyTxt));
			} else {
				sprintf(text, "Total : %s Point", GetNumberText(totalPrice, moneyTxt));
			}
		} else {
			sprintf(text, "Total : %s Zeny", GetNumberText(totalPrice, moneyTxt));
		}
		m_priceText->SetTextWithNewWidth(text);
		Invalidate();
		RecalcScrbarPos();
		break;
	}

	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}


void UIItemPurchaseWnd::StoreInfo()
{
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}
UIMerchantItemPurchaseWnd::UIMerchantItemPurchaseWnd()
{
	m_drawNumItem = 1;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_isBasket = 1;
	m_aid = 0;
	m_info = NULL;
	m_curItem = -1;
}

UIMerchantItemPurchaseWnd::~UIMerchantItemPurchaseWnd()
{}

void UIMerchantItemPurchaseWnd::OnCreate(int cx, int cy)
{
	m_priceText = new UIStaticText;
	m_priceText->Create(160, 15);
	char text[80];
	sprintf(text, "Total : %d Zeny", 0);
	m_priceText->SetTextWithNewWidth(text);
	m_priceText->Move(12, m_h - 21);
	AddChild(m_priceText);

	UIStaticText* title = new UIStaticText;
	title->Create(160, 15);
	title->SetText(MsgStr(MSI_ITEMS_FOR_BUY_FROM_MERCHANT), 0);
	title->Move(16, 2);
	title->m_drawTwice = 1;
	AddChild(title);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWN);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_buy", "btn_buy_a", "btn_buy_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ m_w - 106, m_h - 24 },
		{ m_w - 61, m_h - 24 },
	};

	int i;
	for (i = 0; i < 2; ++i) {
		m_button[i] = new UIBitmapButton;
		m_button[i]->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		m_button[i]->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		m_button[i]->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		m_button[i]->Create(m_button[i]->GetBitmapWidth(), m_button[i]->GetBitmapHeight());
		m_button[i]->Move(pos[i][0], pos[i][1]);
		m_button[i]->SetId(ids[i]);
		AddChild(m_button[i]);
	}

	m_itemVertNum = (cy - 38) / 32;
	m_itemHorzNum = 1;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIMerchantItemPurchaseWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int titleLoop = 0; titleLoop < titleMidCount; ++titleLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
	}

	if (m_h == 17)
		return;

	int sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int leftLoop = 0; leftLoop < sideCount; ++leftLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
		DrawBitmap(0, leftLoop * 8 + 17, bmp, false);
	}

	sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int rightLoop = 0; rightLoop < sideCount; ++rightLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, rightLoop * 8 + 17, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17, (m_w - 60) % 32, m_h - 38, RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17, m_w - 24, (m_h - 38) % 32, RGB(rr, gg, bb));
	}

	char* bmpName[2][3] = {
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp",
		},
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp",
		},
	};

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (int btnLoop = 0; btnLoop < btnMidCount; ++btnLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][2]), false);
		DrawBitmap(btnLoop * 21 + 21, m_h - bmp->m_height, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		int itemCols = (m_w - 40) / 32;
		int itemRows = (m_h - 38) / 32;
		for (int hLoop = 0; hLoop < itemRows; ++hLoop) {
			DrawItemFrame(40, hLoop * 32 + 17);
		}


		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(72, 17, itemCols * 32 - 32, itemRows * 32, RGB(rr, gg, bb));
	}

	char priceText[64];
	char moneyTxt[64];

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		int skipLimit = m_itemHorzNum;
		skipLimit *= m_viewOffset;
		if (itemCount < skipLimit) {
			++it;
			++itemCount;
			continue;
		} else if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		int itemX = (drawCount % m_itemHorzNum) * 32 + 40;
		int itemY = (drawCount / m_itemHorzNum) * 32 + 17;

		if (m_curItem == itemCount) {
			unsigned long rr;
			unsigned long gg;
			unsigned long bb;
			g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
			DrawBox(itemX + 32, itemY + 2, 191, 28, RGB(rr, gg, bb));
		}

		DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);
		DrawItemTextRect(itemX + 36, itemY + 12, 90, *it, 0, 0, 12);

		int priceTextLen;
		if (it->m_price != it->m_realPrice) {
			priceTextLen = sprintf(priceText, " %d -> %s Zeny",
				it->m_price, GetNumberText(it->m_realPrice, moneyTxt));
		} else {
			priceTextLen = sprintf(priceText, " %s Zeny",
				GetNumberText(it->m_price, moneyTxt));
		}
		TextOutWithRightAlign(m_w - 18, itemY + 12, priceText, priceTextLen, 0, 12, 0);

		if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
			return;
		++it;
		++drawCount;
		++itemCount;
	}
}

void UIMerchantItemPurchaseWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		g_modeMgr.GetGameMode();
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}

void UIMerchantItemPurchaseWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);
		char moneyTxt[64];
		char itemHelp[200];
		sprintf(itemHelp, "%s %d %s %s Zeny",
			itemName, itemInfo.m_num, MsgStr(MSI_EA4),
			GetNumberText(itemInfo.m_realPrice, moneyTxt));

		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

void UIMerchantItemPurchaseWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
	int oldCurItem = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (oldCurItem != m_curItem)
		Invalidate();
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* m = g_modeMgr.GetCurMode();
		m->SendMsg(MM_BEGIN_DRAG_FROM_MERCHANTITEMPURCHASEWND,
			(int)itemInfo.m_itemName.c_str(), itemInfo.m_price, itemInfo.m_itemIndex);
	} else {
		ITEM_INFO itemInfo = GetItemInfo(m_curItem);
		if (itemInfo.m_itemIndex) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 1, 0, 0);
			CMode* m = g_modeMgr.GetCurMode();
			m->SendMsg(MM_BEGIN_DRAG_FROM_MERCHANTITEMPURCHASEWND,
				(int)itemInfo.m_itemName.c_str(), itemInfo.m_price, itemInfo.m_itemIndex);
		}
	}
}

void UIMerchantItemPurchaseWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	CGameMode* gameMode = g_modeMgr.GetGameMode();
	if (dragInfo->m_dragType == DT_FROM_MERCHANTITEMSHOPWND) {
		switch (dragInfo->m_dragItemType) {
		case 4:
		case 5:
		case 8:
		case 9:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			if (g_session.GetMerchantPurchaseItemNumByIndex(dragInfo->m_dragItemIndex)) {
				gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_EQUIPITEM_OLNY_ONE), 0xff, 0, 0);
				return;
			}
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		default:
			g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
			UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
			wnd->SendMsg(0, UM_SETSTATE, 12, 0, 0, 0);
			return;
		}
	}

	gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}

void UIMerchantItemPurchaseWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}
int UIMerchantItemPurchaseWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (MerchantItemPurchaseWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWN, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_SETNAID:
		m_aid = val1;
		break;

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		DRAG_INFO* dragInfo = gameMode->GetDragInfo();
		ITEM_INFO itemInfo;
		itemInfo.CopyItemInfo(dragInfo);

		ITEM_INFO buyerInfo;
		int numItem = g_session.GetNumMerchantPurchaseItem(), itemNum = 0;
		for (int buyerIndex = 0; buyerIndex < numItem; ++buyerIndex) {
			buyerInfo = g_session.GetMerchantPurchaseItemInfoBySequence(buyerIndex);
			if (buyerInfo.m_itemIndex == itemInfo.m_itemIndex)
				itemNum = buyerInfo.m_num;
		}

		ITEM_INFO sellerInfo;
		for (int sellerIndex = 0; sellerIndex < g_session.GetNumMerchantShopItem(); ++sellerIndex) {
			sellerInfo = g_session.GetMerchantShopItemInfoBySequence(sellerIndex);
			if (sellerInfo.m_itemIndex == itemInfo.m_itemIndex)
				break;
		}

		if (sellerInfo.m_num < itemInfo.m_num + itemNum)
			itemInfo.m_num = sellerInfo.m_num - itemNum;

		int totalPrice = g_session.GetMerchantPurchaseItemTotalPrice();
		totalPrice += dragInfo->m_dragItemRealPrice * itemInfo.m_num;
		if (g_session.GetGold() >= totalPrice) {
			g_session.AddMerchantPurchaseItem(itemInfo);
			SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
		} else {
			g_windowMgr.SendMsg(UIM_OPENCHATWND, 0, 0, 0, 0);
			g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)MsgStr(MSI_INSUFFICIENT_MONEY), 0xff, 0, 0);
		}

		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	case UM_COMMAND: {
		CMode* gameMode = g_modeMgr.GetCurMode();
		switch (val1) {
		case ID_OK:
			gameMode->SendMsg(MM_PC_PURCHASE_ITEMLIST_FROMMC, m_aid, 0, 0);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			g_session.ClearMerchantDealItems();
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMSHOPWND);
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMPURCHASEWND);
			break;

		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMSHOPWND);
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMPURCHASEWND);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			g_session.ClearMerchantDealItems();
			break;
		}
		break;
	}

	case UM_RESIZE: {
		if (val1 == RT_DOWN) {
			int width = val2;
			int height = val3;
			if (width > 320)
				width = 320;
			if (height > 336)
				height = 336;
			if (width < 232)
				width = 232;
			if (height < 120)
				height = 120;

			width = ((width - 280) / 32) * 32 + 280;
			height = ((height - 120) / 32) * 32 + 120;
			if (width != m_w || height != m_h) {
				Resize(width, height);
				m_itemHorzNum = 1;
				m_itemVertNum = (height - 38) / 32;
				m_resizer->Move(m_w - 15, m_h - 15);
				m_scrollBar->Move(m_w - 15, 14);
				m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
				RecalcScrbarPos();
			}

			m_priceText->Move(12, m_h - 20);
			int pos[2][2] = {
				{ m_w - 106, m_h - 24 },
				{ m_w - 61, m_h - 24 },
			};
			for (int i = 0; i < 2; ++i)
				m_button[i]->Move(pos[i][0], pos[i][1]);

			g_windowMgr.UpdateSnapInfo(this);
		}
		break;
	}

	case UM_REFRESHITEM: {
		Trace("UIMerchantItemPurchaseWnd UM_REFRESHITEM");
		int totalPrice = 0;
		m_itemList.erase(m_itemList.begin(), m_itemList.end());

		int numItem = g_session.GetNumMerchantPurchaseItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetMerchantPurchaseItemInfoBySequence(i);
			if (itemInfo.m_itemIndex) {
				totalPrice += itemInfo.m_realPrice * itemInfo.m_num;
				m_itemList.insert(m_itemList.end(), itemInfo);
			}
		}

		char text[80];
		char moneyTxt[64];
		sprintf(text, "Total : %s Zeny", GetNumberText(totalPrice, moneyTxt));
		m_priceText->SetTextWithNewWidth(text);
		Invalidate();
		RecalcScrbarPos();
		break;
	}

	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIItemSellWnd::UIItemSellWnd()
{
	m_drawNumItem = 1;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_isBasket = 1;
	m_info = NULL;
	m_curItem = -1;
}

UIItemSellWnd::~UIItemSellWnd()
{}

void UIItemSellWnd::OnCreate(int cx, int cy)
{
	m_priceText = new UIStaticText;
	m_priceText->Create(160, 15);
	char text[80];
	sprintf(text, "Total : %d Zeny", 0);
	m_priceText->SetTextWithNewWidth(text);
	m_priceText->Move(12, m_h - 21);
	AddChild(m_priceText);

	UIStaticText* title = new UIStaticText;
	title->Create(90, 15);
	title->SetText(MsgStr(MSI_ITEMS_FOR_SELL), 0);
	title->Move(16, 2);
	title->m_drawTwice = 1;
	AddChild(title);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWN);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_sell", "btn_sell_a", "btn_sell_b" },
		{ "btn_cancel", "btn_cancel_a", "btn_cancel_b" },
	};
	int ids[2] = { ID_OK, ID_CANCEL };
	int pos[2][2] = {
		{ m_w - 106, m_h - 24 },
		{ m_w - 61, m_h - 24 },
	};

	int i;
	for (i = 0; i < 2; ++i) {
		m_button[i] = new UIBitmapButton;
		m_button[i]->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		m_button[i]->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		m_button[i]->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		m_button[i]->Create(m_button[i]->GetBitmapWidth(), m_button[i]->GetBitmapHeight());
		m_button[i]->Move(pos[i][0], pos[i][1]);
		m_button[i]->SetId(ids[i]);
		AddChild(m_button[i]);
	}

	m_itemVertNum = (cy - 38) / 32;
	m_itemHorzNum = 1;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIItemSellWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);
	int oldCurItem = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (oldCurItem != m_curItem)
		Invalidate();
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* m = g_modeMgr.GetCurMode();
		m->SendMsg(MM_BEGIN_DRAG_FROM_ITEMSELLWND,
			(int)itemInfo.m_itemName.c_str(), itemInfo.m_price, itemInfo.m_itemIndex);
	} else {
		ITEM_INFO itemInfo = GetItemInfo(m_curItem);
		if (itemInfo.m_itemIndex) {
			UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
			w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 1, 0, 0);
			CMode* m = g_modeMgr.GetCurMode();
			m->SendMsg(MM_BEGIN_DRAG_FROM_ITEMSELLWND,
				(int)itemInfo.m_itemName.c_str(), itemInfo.m_price, itemInfo.m_itemIndex);
		}
	}
}

void UIItemSellWnd::OnLBtnDblClk(int x, int y)
{
	if (!g_windowMgr.m_isDragAll) return;
	UIItemShopWnd* shop = g_windowMgr.GetItemShopWnd();
	if (!shop) return;

	int clickX = x;
	int clickY = y;
	OnLBtnDown(clickX, clickY);
	CGameMode* mode = g_modeMgr.GetGameMode();
	shop->DragAndDrop(clickX, clickY, mode->GetDragInfo());
}

void UIItemSellWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}
void UIItemSellWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);
		char moneyTxt[64];
		char itemHelp[200];
		if (itemInfo.m_price != itemInfo.m_realPrice) {
			sprintf(itemHelp, "%s %d %s %d -> %s Zeny",
				itemName, itemInfo.m_num, MsgStr(MSI_EA4),
				itemInfo.m_price, GetNumberText(itemInfo.m_realPrice, moneyTxt));
		} else {
			sprintf(itemHelp, "%s %d %s %s Zeny",
				itemName, itemInfo.m_num, MsgStr(MSI_EA4),
				GetNumberText(itemInfo.m_realPrice, moneyTxt));
		}

		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

void UIItemSellWnd::OnDraw()
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

	{
		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);

		DrawBox(20, 17,
			(m_w - 60) % 32, m_h - 38,
			RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17,
			m_w - 24, (m_h - 38) % 32,
			RGB(rr, gg, bb));
	}

	char* bmpName[2][3] = {
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp",
		},
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp",
		},
	};

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);


	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	loopCnt = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][2]), false);
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

	char priceText[64];
	char moneyTxt[64];

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		int skipCount = m_itemHorzNum;
		skipCount *= m_viewOffset;
		if (itemCount < skipCount) {
			++it;
			++itemCount;
			continue;
		} else if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		int itemX = (drawCount % m_itemHorzNum) * 32 + 40;
		int itemY = (drawCount / m_itemHorzNum) * 32 + 17;

		if (m_curItem == itemCount) {
			unsigned long rr;
			unsigned long gg;
			unsigned long bb;
			g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
			DrawBox(itemX + 32, itemY + 2, 191, 28, RGB(rr, gg, bb));
		}

		DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);
		DrawItemText(itemX + 36, itemY + 12, *it, 0, 0, 12);

		int priceTextLen;
		if (it->m_price != it->m_realPrice) {
			priceTextLen = sprintf(priceText, " %d -> %s Zeny",
				it->m_price, GetNumberText(it->m_realPrice, moneyTxt));
		} else {
			priceTextLen = sprintf(priceText, " %s Zeny",
				GetNumberText(it->m_price, moneyTxt));
		}
		TextOutWithRightAlign(m_w - 18, itemY + 12, priceText, priceTextLen, 0, 12, 0);

		if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
			return;
		++it;
		++drawCount;
		++itemCount;
	}
}

void UIItemSellWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	CGameMode* gameMode = g_modeMgr.GetGameMode();
	if (dragInfo->m_dragType == DT_FROM_ITEMSHOPWND) {
		if (dragInfo->m_numDragItem == 1 || g_windowMgr.m_isDragAll) {
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		}

		g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
		UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
		wnd->SendMsg(0, UM_SETSTATE, 2, 0, 0, 0);
		return;
	}

	gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}
int UIItemSellWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (ItemSellWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWN, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		DRAG_INFO* dragInfo = gameMode->GetDragInfo();
		ITEM_INFO itemInfo;
		itemInfo.CopyItemInfo(dragInfo);
		Trace("UIItemSellWnd :: DragAndDrop itemInfo.m_itemName == %s",
			itemInfo.m_itemName.c_str());
		Trace("UIItemSellWnd :: DragAndDrop itemInfo.m_itemIndex == %d",
			itemInfo.m_itemIndex);
		if (g_session.IsExistInShopItemList(dragInfo->m_dragItemIndex)) {
			g_session.AddSellItem(itemInfo);
			g_session.DecShopItem(itemInfo);
			UIWindow* wnd = g_windowMgr.GetItemShopWnd();
			if (wnd)
				wnd->SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
			SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
		}
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	case UM_COMMAND: {
		CMode* gameMode = g_modeMgr.GetCurMode();
		switch (val1) {
		case ID_OK:
			if (g_session.IsSellItemValid())
				gameMode->SendMsg(MM_REQ_SELL, 0, 0, 0);
			g_windowMgr.DeleteWindow(WID_ITEMSHOPWND);
			g_windowMgr.DeleteWindow(WID_ITEMSELLWND);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			g_session.ClearDealItems();
			break;
		case ID_CANCEL:
			g_windowMgr.DeleteWindow(WID_ITEMSHOPWND);
			g_windowMgr.DeleteWindow(WID_ITEMSELLWND);
			gameMode->SendMsg(MM_RELEASE_QUEST, 0, 0, 0);
			g_session.ClearDealItems();
		}
		break;
	}

	case UM_RESIZE: {
		if (val1 == RT_DOWN) {
			int width = val2;
			int height = val3;
			if (width > 320)
				width = 320;
			if (height > 336)
				height = 336;
			if (width < 232)
				width = 232;
			if (height < 120)
				height = 120;

			width = ((width - 280) / 32) * 32 + 280;
			height = ((height - 120) / 32) * 32 + 120;
			if (width != m_w || height != m_h) {
				Resize(width, height);
				m_itemHorzNum = 1;
				m_itemVertNum = (height - 38) / 32;
				m_resizer->Move(m_w - 15, m_h - 15);
				m_scrollBar->Move(m_w - 15, 14);
				m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
				RecalcScrbarPos();
			}

			m_priceText->Move(12, m_h - 20);
			int pos[2][2] = {
				{ m_w - 106, m_h - 24 },
				{ m_w - 61, m_h - 24 },
			};
			for (int i = 0; i < 2; ++i)
				m_button[i]->Move(pos[i][0], pos[i][1]);
			g_windowMgr.UpdateSnapInfo(this);
		}
		break;
	}

	case UM_REFRESHITEM: {
		Trace("UIItemSellWnd UM_REFRESHITEM");
		int totalPrice = 0;
		m_itemList.erase(m_itemList.begin(), m_itemList.end());
		int numItem = g_session.GetNumSellItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetSellItemInfoBySequence(i);
			if (itemInfo.m_itemIndex) {
				totalPrice += itemInfo.m_realPrice * itemInfo.m_num;
				m_itemList.insert(m_itemList.end(), itemInfo);
			}
		}

		char text[80];
		char moneyTxt[64];
		sprintf(text, "Total : %s Zeny", GetNumberText(totalPrice, moneyTxt));
		m_priceText->SetTextWithNewWidth(text);
		Invalidate();
		RecalcScrbarPos();
		break;
	}
	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}


void UIItemSellWnd::StoreInfo()
{
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}

UIMerchantItemWnd::UIMerchantItemWnd()
{
	m_drawNumItem = 1;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_isBasket = 1;
	m_info = NULL;
}
UIMerchantItemWnd::~UIMerchantItemWnd()
{}

void UIMerchantItemWnd::OnCreate(int cx, int cy)
{
	UIStaticText* title = new UIStaticText;
	title->Create(90, 15);
	title->SetText(MsgStr(MSI_MERCHANTITEM), 0);
	title->Move(16, 2);
	title->m_drawTwice = 1;
	AddChild(title);

	m_supplyText = new UIStaticText;
	m_supplyText->Create(180, 15);
	{
		char supply[128];
		sprintf(supply, "Num: %d/%d Weight: %d/%d",
			g_session.m_cartCurCount, g_session.m_cartMaxCount,
			g_session.m_cartCurWeight, g_session.m_cartMaxWeight);
		m_supplyText->SetText(supply, 0);
	}
	m_supplyText->Move(16, m_h - 19);
	AddChild(m_supplyText);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWNRIGHT);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	{
		int posX[3] = { m_w - 14 };
		int ids2[3] = { ID_CLOSE };
		char* buttonName2[8] = { "close" };
		char* toolTip[8] = { "Alt+W" };

		std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
		for (int i = 0; i < 1; ++i) {
			m_sysButtons[i] = new UIBitmapButton;
			m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
			m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
			m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
			m_sysButtons[i]->Create(m_sysButtons[i]->GetBitmapWidth(), m_sysButtons[i]->GetBitmapHeight());
			m_sysButtons[i]->Move(posX[i], 3);
			m_sysButtons[i]->SetId(ids2[i]);
			m_sysButtons[i]->SetToolTip(toolTip[i]);
			AddChild(m_sysButtons[i]);
		}
	}

	m_itemHorzNum = (cx - 40) / 32;
	m_itemVertNum = (cy - 38) / 32;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIMerchantItemWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}
void UIMerchantItemWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);
		char itemHelp[200];
		sprintf(itemHelp, "%s %d %s", itemName, itemInfo.m_num, MsgStr(MSI_EA4));

		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

void UIMerchantItemWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd() || g_windowMgr.GetSayDialogWnd() || g_windowMgr.GetChooseWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	CGameMode* gameMode = g_modeMgr.GetGameMode();
	switch (dragInfo->m_dragType) {
	case DT_FROM_ITEMWND:
	case DT_FROM_ITEMSTOREWND: {
		if (dragInfo->m_numDragItem == 1) {
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		}

		g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
		UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
		wnd->SendMsg(0, UM_SETSTATE, 8, 0, 0, 0);
		return;
	}
	default:
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
	}
}
void UIMerchantItemWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* mode = g_modeMgr.GetCurMode();
		mode->SendMsg(MM_BEGIN_DRAG_FROM_MERCHANTITEMWND, itemInfo.m_itemIndex, 0, 0);
	}
}

void UIMerchantItemWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

int UIMerchantItemWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (MerchantItemWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWNRIGHT, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		if (val1 == 0x87)
		{
			g_windowMgr.DeleteWindow(WID_MERCHANTITEMWND);
		}
		break;

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		switch (gameMode->GetDragType()) {
		case DT_FROM_ITEMWND:
			gameMode->SendMsg(MM_MOVE_ITEM_FROM_BODY_TO_CART,
				gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
			break;
		case DT_FROM_ITEMSTOREWND:
			gameMode->SendMsg(MM_MOVE_ITEM_FROM_STORE_TO_CART,
				gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
			break;
		}
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	case UM_RESIZE: {
		if (val1 == RT_DOWNRIGHT) {

			int width = val2;
			int height = val3;
			if (width > 320)
				width = 320;
			if (height > 240)
				height = 240;
			if (width < 232)
				width = 232;
			if (height < 120)
				height = 120;

			width = ((width - 280) / 32) * 32 + 280;
			height = ((height - 120) / 32) * 32 + 120;
			if (width != m_w || height != m_h) {
				Resize(width, height);
				m_itemHorzNum = (width - 40) / 32;
				m_itemVertNum = (height - 38) / 32;
				m_resizer->Move(m_w - 15, m_h - 15);
				m_scrollBar->Move(m_w - 15, 14);
				m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
				RecalcScrbarPos();
			}

			m_supplyText->Move(16, m_h - 19);
			int posX[3] = { m_w - 14 };

			m_sysButtons[0]->Move(posX[0], 3);
			g_windowMgr.UpdateSnapInfo(this);
		}
		break;
	}

	case UM_REFRESHITEM: {
		Trace("UIMerchantItemWnd UM_REFRESHITEM");
		m_itemList.erase(m_itemList.begin(), m_itemList.end());
		int numItem = g_session.GetNumMerchantItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetMerchantItemInfoBySequence(i);
			if (itemInfo.m_itemIndex)
				m_itemList.insert(m_itemList.end(), itemInfo);
		}
		Invalidate();
		RecalcScrbarPos();
		break;
	}

	case UM_SETCOUNTINFO: {
		char supply[128];
		sprintf(supply, "Num: %d/%d Weight: %d/%d",
			g_session.m_cartCurCount, g_session.m_cartMaxCount,
			g_session.m_cartCurWeight, g_session.m_cartMaxWeight);
		m_supplyText->SetText(supply, 0);
		Invalidate();
		break;
	}

	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}
	return 0;
}

UIMerchantMirrorItemWnd::UIMerchantMirrorItemWnd()
{
	m_drawNumItem = 1;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_isBasket = 1;
	m_info = NULL;
}

UIMerchantMirrorItemWnd::~UIMerchantMirrorItemWnd()
{}

void UIMerchantMirrorItemWnd::OnCreate(int cx, int cy)
{
	UIStaticText* title = new UIStaticText;
	title->Create(150, 15);
	title->SetText(MsgStr(MSI_MERCHANTMIRRORITEM), 0);
	title->Move(16, 2);
	title->m_drawTwice = 1;
	AddChild(title);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWNRIGHT);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	m_itemHorzNum = (cx - 40) / 32;
	m_itemVertNum = (cy - 38) / 32;
	g_session.CloneMerchantItemToMerchantMirrorItem();
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIMerchantMirrorItemWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* mode = g_modeMgr.GetCurMode();
		mode->SendMsg(MM_BEGIN_DRAG_FROM_MERCHANTMIRRORITEMWND, itemInfo.m_itemIndex, 0, 0);
	}
}

void UIMerchantMirrorItemWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}
void UIMerchantMirrorItemWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);
		char itemHelp[200];
		sprintf(itemHelp, "%s %d %s", itemName, itemInfo.m_num, MsgStr(MSI_EA4));

		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

void UIMerchantMirrorItemWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	CGameMode* gameMode = g_modeMgr.GetGameMode();
	if (dragInfo->m_dragType != DT_FROM_MERCHANTSHOPMAKEWND) {
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		return;
	}

	if (dragInfo->m_numDragItem == 1) {
		SendMsg(0, UM_DROP, 0, 0, 0, 0);
		return;
	}

	g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
	UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
	wnd->SendMsg(0, UM_SETSTATE, 10, 0, 0, 0);
}

void UIMerchantMirrorItemWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}
int UIMerchantMirrorItemWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message)
	{
	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		DRAG_INFO* dragInfo = gameMode->GetDragInfo();
		ITEM_INFO itemInfo;
		itemInfo.CopyItemInfo(dragInfo);
		if (dragInfo->m_dragType == DT_FROM_MERCHANTSHOPMAKEWND) {
			if (g_session.GetMerchantHopeToSellItemNum(dragInfo->m_dragItemIndex)) {
				g_session.AddMerchantMirrorItem(itemInfo);
				g_session.DecMerchantHopeToSellItem(itemInfo);
			}
		}
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	case UM_SETINFO:
		m_info = (MerchantMirrorItemWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWNRIGHT, m_info->w, m_info->h, 0);
		Move(m_info->x, m_info->y);
		break;

	case UM_RESIZE: {
		if (val1 != RT_DOWNRIGHT)
			break;

		int width = val2;
		int height = val3;
		if (width > 320)
			width = 320;
		if (height > 240)
			height = 240;
		if (width < 232)
			width = 232;
		if (height < 120)
			height = 120;

		width = ((width - 280) / 32) * 32 + 280;
		height = ((height - 120) / 32) * 32 + 120;
		if (width != m_w || height != m_h)
		{
			Resize(width, height);
			m_itemHorzNum = (width - 40) / 32;
			m_itemVertNum = (height - 38) / 32;
			m_resizer->Move(m_w - 15, m_h - 15);
			m_scrollBar->Move(m_w - 15, 14);
			m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
			RecalcScrbarPos();
		}
		g_windowMgr.UpdateSnapInfo(this);
		break;
	}

	case UM_REFRESHITEM: {
		m_itemList.erase(m_itemList.begin(), m_itemList.end());
		int numItem = g_session.GetNumMerchantMirrorItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetMerchantMirrorItemInfoBySequence(i);
			if (itemInfo.m_itemIndex)
				m_itemList.push_back(itemInfo);
		}
		Invalidate();
		RecalcScrbarPos();
		break;
	}
	case UM_COMMAND:
		break;
	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

UIItemWnd::UIItemWnd()
{
	m_drawNumItem = 1;
	m_isBasket = 0;
	UIItemBaseWnd::m_button[0] = NULL;
	UIItemBaseWnd::m_button[1] = NULL;
	m_resizer = NULL;
	m_scrollBar = NULL;
	m_info = NULL;
	m_curRadioBtn[0] = 0;
	m_curRadioBtn[1] = 0;
	m_curRadioBtn[2] = 0;
	m_curTab = 1;
	m_viewOffset = 0;
	m_vertScrEnabled = 0;
}

UIItemWnd::~UIItemWnd()
{}

void UIItemWnd::OnCreate(int cx, int cy)
{
	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWNRIGHT);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_tab = new UITab;
	m_tab->Create(20, 82);
	m_tab->Move(0, 17);
	m_tab->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
	AddChild(m_tab);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 17, 1, m_h - 38);
	AddChild(m_scrollBar);

	int posX[3] = { 3, m_w - 28, m_w - 14 };
	int ids2[3] = { ID_BASE, ID_MINI, ID_CLOSE };
	char* buttonName2[8] = { "base", "mini", "close" };
	char* toolTip[8] = { "", "Ctrl+E", "Alt+E" };

	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
	{
		for (int i = 0; i < 3; ++i) {
			m_sysButtons[i] = new UIBitmapButton;
			m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
			m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
			m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
			m_sysButtons[i]->Create(m_sysButtons[i]->GetBitmapWidth(), m_sysButtons[i]->GetBitmapHeight());
			m_sysButtons[i]->Move(posX[i], 3);
			m_sysButtons[i]->SetId(ids2[i]);
			m_sysButtons[i]->SetToolTip(toolTip[i]);
			AddChild(m_sysButtons[i]);
		}
	}

	m_orgHeight = m_h;
	m_itemHorzNum = (cx - 40) / 32;
	m_itemVertNum = (cy - 38) / 32;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}


void UIItemWnd::OnDestroy() {}

void UIItemWnd::OnDraw()
{
	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int titleLoop = 0; titleLoop < titleMidCount; ++titleLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
	}

	TextOutA(17, 3, MsgStr(MSI_ITEM), 0, 0, 12, 0xffffff);
	TextOutA(16, 2, MsgStr(MSI_ITEM), 0, 0, 12, 0);

	if (m_h == 17)
		return;

	if (m_h > 120) {
		int sideCount = (m_h - 120) / 8 + ((m_h - 120) % 8 != 0);
		for (int leftLoop = 0; leftLoop < sideCount; ++leftLoop) {
			bmp = (CBitmapRes*)g_resMgr().Get(
				UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
			DrawBitmap(0, leftLoop * 8 + 99, bmp, false);
		}
	}

	int sideCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int rightLoop = 0; rightLoop < sideCount; ++rightLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, rightLoop * 8 + 17, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17, (m_w - 60) % 32, m_h - 38, RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17, m_w - 24, (m_h - 38) % 32, RGB(rr, gg, bb));
	}

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (int btnLoop = 0; btnLoop < btnMidCount; ++btnLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp"), false);
		DrawBitmap(btnLoop * 21 + 21, m_h - bmp->m_height, bmp, false);
	}

	int hLoop = (m_w - 40) / 32;
	int itemRows = (m_h - 38) / 32;
	for (int vLoop = 0; vLoop < itemRows; ++vLoop) {
		for (int wLoop = 0; wLoop < hLoop; ++wLoop)
			DrawItemFrame(wLoop * 32 + 40, vLoop * 32 + 17);
	}

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		int skipLimit = m_itemHorzNum * m_viewOffset;
		if (itemCount < skipLimit) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		DrawItemWithCount((drawCount % m_itemHorzNum) * 32 + 44,
			(drawCount / m_itemHorzNum) * 32 + 21, *it, false, false);

		++drawCount;
		if (drawCount == m_itemVertNum * m_itemHorzNum)
			return;

		++it;
		++itemCount;
	}
}

void UIItemWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* mode = g_modeMgr.GetCurMode();
		mode->SendMsg(MM_BEGIN_DRAG_FROM_ITEMWND, itemInfo.m_itemIndex, itemInfo.m_num, 0);
	}
}

ITEM_INFO UIItemWnd::GetItemInfo(int x, int y)
{
	if (x < 40 || x >= m_w - 20 || y < 17 || y >= m_itemVertNum * 32 + 17)
		return ITEM_INFO();

	int xCell = (x - 40) / 32;
	int yCell = (y - 17) / 32;
	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		if (drawCount == yCell * m_itemHorzNum + xCell)
			return *it;

		++it;
		++drawCount;
		++itemCount;
	}

	return ITEM_INFO();
}

void UIItemWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		g_modeMgr.GetGameMode();
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}


void UIItemWnd::OnLBtnDblClk(int x, int y)
{
	if (y < 17) {
		SendMsg(this, UM_COMMAND, 0x88, 0, 0, 0);
		return;
	}

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.IsValid()) {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		switch (itemInfo.m_itemType) {
		case 0:
		case 1:
		case 2:
			gameMode->SendMsg(MM_USEITEM, itemInfo.m_itemIndex);
			break;
		case 18:
			if (g_session.GetHonor() >= 0) {
				char msg[500];
				char itemName[128];
				itemInfo.GetItemName(itemName);
				sprintf(msg, MsgStr(MSI_CONFIRM_TO_OPEN_CASH_ITEM), itemName);
				if (g_windowMgr.ErrorMsg(msg, ME_YESNO, FALSE, FALSE, 0) == ID_YES)
					gameMode->SendMsg(MM_USEITEM, itemInfo.m_itemIndex);
			}
			break;
		case 4:
		case 5:
		case 8:
		case 9:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			if (itemInfo.IsValid() && itemInfo.m_location && itemInfo.m_wearLocation == 0 && itemInfo.m_isIdentified)
				gameMode->SendMsg(MM_WEAR_EQUIP, itemInfo.m_itemIndex, itemInfo.m_location, 0);
			else if (itemInfo.GetItemId() >= 10001
				&& itemInfo.GetItemId() <= 10020
				&& itemInfo.m_isIdentified)
				gameMode->SendMsg(MM_WEAR_EQUIP, itemInfo.m_itemIndex, itemInfo.m_location, 0);
			break;
		case 10:
		case 16:
		case 17:
			if (itemInfo.IsValid() && itemInfo.m_wearLocation == 0)
				gameMode->SendMsg(MM_WEAR_EQUIP_ARROW, itemInfo.m_itemIndex, itemInfo.m_location, 0);
			break;
		case 6:
			gameMode->SendMsg(MM_REQ_ITEMCOMPOSITION_LIST, itemInfo.m_itemIndex, 0, 0);
			break;
		}
	}
}

void UIItemWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		itemInfo.GetItemName(itemName);
		char itemHelp[128];
		sprintf(itemHelp, MsgStr(MSI_EA2), itemName, itemInfo.m_num);

		CMode* mode = g_modeMgr.GetCurMode();
		mode->MakeHelpBalloon(itemHelp, gx, gy, g_session.GetItemDisColor(&itemInfo));
	}
}

void UIItemWnd::StoreInfo()
{
	if (m_info == NULL) return;
	m_info->w = m_w;
	m_info->h = m_h;
	m_info->x = m_x;
	m_info->y = m_y;
	m_info->orgHeight = m_orgHeight;
	m_info->curTab = m_curTab;
	m_info->curRadio1 = m_curRadioBtn[0];
	m_info->curRadio2 = m_curRadioBtn[1];
	m_info->curRadio3 = m_curRadioBtn[2];
}

void UIItemWnd::RefreshRadioBtn() { Invalidate(); }

int UIItemWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	RAG_BUDGET_FILL(255)
	RAG_BUDGET_FILL(255)
	int posX[3];

	switch (message) {
	case UM_SETINFO:
		m_info = (ItemWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();

		SendMsg(NULL, UM_RESIZE, RT_DOWNRIGHT, m_info->w, m_info->orgHeight, 0);
		m_curTab = m_info->curTab;
		m_curRadioBtn[0] = 3;
		m_curRadioBtn[1] = 3;
		m_curRadioBtn[2] = 4;
		m_tab->SendMsg(NULL, UM_SETSTATE, m_info->curTab, 0, 0, 0);
		Invalidate();
		SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);

		if (m_info->h != m_info->orgHeight)
			SendMsg(NULL, UM_COMMAND, 0x88, 0, 0, 0);

		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_ITEMWND);
			break;

		case ID_MINI: {
			int oldHeight = m_h;
			if (m_h == 17) {
				Resize(m_w, m_orgHeight);
			} else {
				m_orgHeight = m_h;
				Resize(m_w, 17);
			}

			g_windowMgr.UpdateSnapInfo(this);
			g_windowMgr.MakeTopLayer(this);

			int dy = m_h - oldHeight;
			if (dy)
				g_snapMgr.MoveConnectedObject(this, 0, dy, 3);
			break;
		}
		}
		break;

	case UM_TAB:
		m_curTab = val1;
		if (m_curTab < 0)
			m_curTab = 0;
		if (m_curTab > 2)
			m_curTab = 2;
		Invalidate();
		SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
		break;

	case UM_RESIZE: {
		if (val1 != RT_DOWNRIGHT)
			break;

		int width = val2;
		int height = val3;
		if (width > 320)
			width = 320;
		if (height > 240)
			height = 240;
		if (width < 232)
			width = 232;
		if (height < 102)
			height = 102;

		height = ((height - 38) / 32) * 32 + 38;
		width = ((width - 280) / 32) * 32 + 280;

		if (width != m_w || height != m_h) {
			Resize(width, height);
			m_itemHorzNum = (width - 40) / 32;
			m_itemVertNum = (height - 38) / 32;
			m_resizer->Move(m_w - 15, m_h - 15);
			m_scrollBar->Move(m_w - 15, 17);
			m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
			RecalcScrbarPos();
			Invalidate();

			posX[0] = 3;
			posX[1] = m_w - 28;
			posX[2] = m_w - 14;
			for (int i = 0; i < 3; ++i)
				m_sysButtons[i]->Move(posX[i], 3);
		}

		m_orgHeight = m_h;
		g_windowMgr.UpdateSnapInfo(this);
		break;
	}

	case UM_REFRESHITEM: {
		m_itemList.clear();

		int numItem = g_session.GetNumItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetItemInfoBySequence(i);
			if (itemInfo.m_itemIndex) {
				if (Sort(&itemInfo))
					m_itemList.push_back(itemInfo);
			}
		}

		Invalidate();
		RecalcScrbarPos();
		break;
	}

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		switch (gameMode->GetDragType()) {
		case DT_FROM_EQUIPWND:
			gameMode->SendMsg(MM_TAKE_OFF_EQUIP, gameMode->GetDragInfo()->m_slotNum, 0, 0);
			break;

		case DT_FROM_ITEMSTOREWND:
			gameMode->SendMsg(MM_MOVE_ITEM_FROM_STORE_TO_BODY,
				gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
			break;

		case DT_FROM_MERCHANTITEMWND:
			gameMode->SendMsg(MM_MOVE_ITEM_FROM_CART_TO_BODY,
				gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
			break;

		case DT_FROM_MAILWND:
			if (g_windowMgr.GetMailListWnd())
				g_windowMgr.GetMailListWnd()->SendMsg(
					NULL, 0x68, gameMode->GetDragItemIndex(),
					gameMode->GetNumDragItem(), 0, 0);
			break;
		}

		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

bool UIItemWnd::Sort(ITEM_INFO const *const itemInfo)
{
	UIItemWnd *wnd = this;
	int itemType = itemInfo->m_itemType;

	switch (wnd->m_curTab) {
	case 0:
		if (itemType == 0 || itemType == 1 || itemType == 2 || itemType == 18) {
			switch (wnd->m_curRadioBtn[0]) {
			case 0:
				if (itemType == 0)
					return true;
				break;
			case 2:
				if (itemType == 1 || itemType == 2)
					return true;
				break;
			case 3:
				return true;
			}
		}
		break;
	case 1:
		switch (itemType) {
		case 4:
		case 5:
		case 8:
		case 9:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			switch (wnd->m_curRadioBtn[1]) {
			case 0:
				if (itemType == 5 || itemType == 9 || itemType == 8)
					return true;
				break;
			case 1:
				if (itemType == 4 || (itemType > 10 && itemType <= 14))
					return true;
				break;
			case 3:
				return true;
			}
			break;
		}
		return false;
	case 2:
		if (itemType == 6 || itemType == 3 || itemType == 7 ||
			itemType == 10 || itemType == 16 || itemType == 17) {
			if (wnd->m_curRadioBtn[2] == 4)
				return true;
		}
		break;
	}

	return false;
}
void UIItemWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}

	switch (dragInfo->m_dragType) {
	case DT_FROM_EQUIPWND:
		SendMsg(0, UM_DROP, 0, 0, 0, 0);
		return;
	case DT_FROM_ITEMSTOREWND:
	case DT_FROM_MERCHANTITEMWND: {
		if (dragInfo->m_numDragItem == 1) {
			SendMsg(0, UM_DROP, 0, 0, 0, 0);
			return;
		}

		g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
		UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
		wnd->SendMsg(0, UM_SETSTATE, 6, 0, 0, 0);
		return;
	}
	case DT_FROM_METALPROCESSWND: {
		UIWindow* wnd = (UIWindow*)g_windowMgr.m_metalProcessWnd;
		if (wnd) {
			wnd->SendMsg(
				0, 0x4b, (int)dragInfo->m_dragItemName.c_str(), 0, 0, 0);
		}
		break;
	}
	case DT_FROM_MAILWND: {
		UIWindow* wnd = g_windowMgr.GetItemWnd();
		if (!wnd)
			return;
		wnd->SendMsg(0, UM_DROP, 0, 0, 0, 0);
		return;
	}
	default:
		break;
	}

	g_modeMgr.GetGameMode()->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
}
UIQuestWnd::UIQuestWnd()
{
	m_isBasket = 0;
	UIItemBaseWnd::m_button[0] = NULL;
	UIItemBaseWnd::m_button[1] = NULL;
	m_resizer = NULL;
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_curTab = 0;
	m_scrollBar = NULL;
	m_vertScrEnabled = 0;
	m_viewOffset = 0;
	m_info = NULL;
	m_curQuest = 0;
}

UIQuestWnd::~UIQuestWnd()
{}

void UIQuestWnd::OnCreate(int cx, int cy)
{
	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWNRIGHT);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_tab = new UITab;
	m_tab->Create(20, 94);
	m_tab->Move(0, 17);
	m_tab->m_typeTab = 2;
	m_tab->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
	AddChild(m_tab);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 17, 1, m_h - 46);
	AddChild(m_scrollBar);

	int posX[3] = { 3, m_w - 28, m_w - 14 };
	int ids2[3] = { ID_BASE, ID_MINI, ID_CLOSE };
	char* buttonName2[8] = { "base", "mini", "close" };
	char* toolTip[8] = { "", "Ctrl+U", "Alt+U" };

	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
	for (int i = 0; i < 3; ++i) {
		m_sysButtons[i] = new UIBitmapButton;
		m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_off.bmp").c_str(), BS_NORMAL);
		m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_MOUSEON);
		m_sysButtons[i]->SetBitmapName((preBmpName2 + buttonName2[i] + "_on.bmp").c_str(), BS_PRESSED);
		m_sysButtons[i]->Create(m_sysButtons[i]->GetBitmapWidth(), m_sysButtons[i]->GetBitmapHeight());
		m_sysButtons[i]->Move(posX[i], 3);
		m_sysButtons[i]->SetId(ids2[i]);
		m_sysButtons[i]->SetToolTip(toolTip[i]);
		AddChild(m_sysButtons[i]);
	}

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_view", "btn_view_a", "btn_view_b" },
		{ "btn_close", "btn_close_a", "btn_close_b" },
	};
	int ids[2] = { ID_OK, ID_CLOSE };
	int pos[2][2] = {
		{ m_w - 107, m_h - 24 },
		{ m_w - 61, m_h - 24 },
	};

	{
		for (int i = 0; i < 2; ++i) {
			m_button[i] = new UIBitmapButton;
			m_button[i]->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
			m_button[i]->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
			m_button[i]->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
			m_button[i]->Create(m_button[i]->GetBitmapWidth(), m_button[i]->GetBitmapHeight());
			m_button[i]->Move(pos[i][0], pos[i][1]);
			m_button[i]->SetId(ids[i]);
			AddChild(m_button[i]);
		}
	}

	m_orgHeight = m_h;
	m_itemVertNum = (m_h - 40) / 40;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}


void UIQuestWnd::OnDestroy() {}

void UIQuestWnd::OnDraw()
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

	TextOutA(17, 3, MsgStr(MSI_QUESTWIN), 0, 0, 12, 0xffffff);
	TextOutA(16, 2, MsgStr(MSI_QUESTWIN), 0, 0, 12, 0);

	if (m_h == 17)
		return;

	if (m_h > 120) {
		loopCnt = (m_h - 120) / 8 + (((m_h - 120) % 8) ? 1 : 0);
		for (i = 0; i < loopCnt; ++i) {
			bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"), false);
			DrawBitmap(0, i * 8 + 99, bmp, false);
		}
	}
	loopCnt = (m_h - 38) / 8 + (((m_h - 38) % 8) ? 1 : 0);
	for (i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"), false);
		DrawBitmap(m_w - bmp->m_width, i * 8 + 17, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(20, 17,
			(m_w - 60) % 32, m_h - 38,
			RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17,
			m_w - 24, (m_h - 38) % 32,
			RGB(rr, gg, bb));
	}


	int hLoop = (m_w - 40) / 32;
	int itemRows = (m_h - 38) / 32;
	for (int vLoop = 0; vLoop < itemRows; ++vLoop)
	{
		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		for (int wLoop = 0; wLoop < hLoop; ++wLoop)
			DrawBox(wLoop * 32 + 40, 17,
				vLoop * 32 + 17, m_h - 38,
				RGB(rr, gg, bb));
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




	int QuestCount = 0;
	int drawCount = 0;

	std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();

	while (it != g_session.m_QuestList.end())
	{


		if (m_curTab == 0)
		{
			if (!it->show)
			{
				++it;
				continue;
			}
		}
		else if (m_curTab == 1)
		{
			if (it->show == 1)
			{
				++it;
				continue;
			}
		}

		if (m_viewOffset > QuestCount)
		{
			++it;
			++QuestCount;
			continue;
		}


		int rowY = 20 + drawCount * 40;
		char text[256];
		char bmpName[1024];
		if (ITEM_INFO::GetQuestDesc(it->questID).questName) {
			g_session.GetSkillBitmapFileName(ITEM_INFO::GetQuestDesc(it->questID).iconName, bmpName);
			bmp = (CBitmapRes*)g_resMgr().Get(bmpName, false);
			DrawBitmap(40, rowY, bmp, true);
			sprintf(text, "%s", ITEM_INFO::GetQuestDesc(it->questID).questName);
		} else {
			sprintf(text, "ID=%d , active=%d", it->questID, it->show);
		}

		if (m_curQuest == (int)it->questID)
			DrawBox(88, rowY - 2, m_w - 110, 40, 0xc8c8c8);

		if (it->show == 1) {
			TextOutA(101, rowY + 1, text, 0, 0, 12, 0x969696);
			TextOutA(100, rowY, text, 0, 0, 12, 0);
		} else {
			TextOutA(101, rowY + 1, text, 0, 0, 12, 0xc8c8c8);
			TextOutA(100, rowY, text, 0, 0, 12, 0x464646);
		}

		++QuestCount;
		++drawCount;
		++it;

		if (drawCount >= (m_h - 40) / 40)
			break;
	}
}

void UIQuestWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);



	int row = (y - 20) / 40;
	if (row < 0)
		row = 0;

	if (row >= m_itemVertNum)
		row = m_itemVertNum - 1;

	row += m_viewOffset;


	std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();

	if (m_curTab == 0)
	{
		while (it->show != 1)
		{
			++it;

			if (it == g_session.m_QuestList.end())
				break;
		}
	}
	else if (m_curTab == 1)
	{
		while (it->show != 0)
		{
			++it;
			if (it == g_session.m_QuestList.end())
				break;
		}
	}

	for (int count = 0; count < row; ++count)
	{
		++it;

		if (m_curTab == 0)
		{
			while (it->show != 1)
			{
				++it;

				if (it == g_session.m_QuestList.end())
					break;
			}
		}
		else if (m_curTab == 1)
		{
			while (it->show != 0)
			{
				++it;

				if (it == g_session.m_QuestList.end())
					break;
			}
		}

		if (it == g_session.m_QuestList.end())
			break;
	}



	if (it != g_session.m_QuestList.end())
	{

		m_curQuest = it->questID;
		Invalidate();
	}
}

void UIQuestWnd::OnLBtnDblClk(int x, int y)
{
	if (m_curQuest > 0) {
		UIFrameWnd* wnd = g_windowMgr.MakeWindow(WID_QUEST_DETAIL_WND);
		wnd->SendMsg(NULL, UM_SKILLDESCRIBE, m_curQuest, 0, 0, 0);
	}
}
void UIQuestWnd::OnRBtnDown(int x, int y)
{


	int row = (y - 20) / 40;
	if (row < 0)
		row = 0;

	if (row >= m_itemVertNum)
		row = m_itemVertNum - 1;

	row += m_viewOffset;


	std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();

	if (m_curTab == 0)
	{
		while (it->show != 1)
		{
			++it;

			if (it == g_session.m_QuestList.end())
				break;
		}
	}
	else if (m_curTab == 1)
	{
		while (it->show != 0)
		{
			++it;
			if (it == g_session.m_QuestList.end())
				break;
		}
	}

	for (int count = 0; count < row; ++count)
	{
		++it;

		if (m_curTab == 0)
		{
			while (it->show != 1)
			{
				++it;

				if (it == g_session.m_QuestList.end())
					break;
			}
		}
		else if (m_curTab == 1)
		{
			while (it->show != 0)
			{
				++it;

				if (it == g_session.m_QuestList.end())
					break;
			}
		}

		if (it == g_session.m_QuestList.end())
			break;
	}

	if (it != g_session.m_QuestList.end()) {
		PACKET_CZ_ACTIVE_QUEST p;
		p.PacketType = HEADER_CZ_ACTIVE_QUEST;
		p.questID = it->questID;
		p.active = it->show != 1;
		CRagConnection::instanceR().SendPacket(
			CRagConnection::instanceR().GetPacketSize(HEADER_CZ_ACTIVE_QUEST),
			(const char*)&p);
		m_curQuest = it->questID;
	}
}

void UIQuestWnd::OnMouseHover(int x, int y) {}

void UIQuestWnd::RefreshRadioBtn() { Invalidate(); }

void UIQuestWnd::StoreInfo()
{
	if (m_info == NULL) return;
	m_info->w = m_w;
	m_info->h = m_h;
	m_info->x = m_x;
	m_info->y = m_y;
	m_info->orgHeight = m_orgHeight;
	m_info->curTab = m_curTab;
}
int UIQuestWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETINFO:
		m_info = (QuestWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		SendMsg(NULL, UM_RESIZE, RT_DOWNRIGHT, m_info->w, m_info->orgHeight, 0);

		m_curTab = m_info->curTab;
		m_tab->SendMsg(NULL, UM_SETSTATE, m_info->curTab, 0, 0, 0);
		Invalidate();

		SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);

		if (m_info->h != m_info->orgHeight)
			SendMsg(NULL, UM_COMMAND, ID_MINI, 0, 0, 0);

		Move(m_info->x, m_info->y);
		break;

	case UM_COMMAND:
		switch (val1) {
		case ID_OK:
			if (m_curQuest > 0) {
				UIWindow* wnd = g_windowMgr.MakeWindow(WID_QUEST_DETAIL_WND);
				wnd->SendMsg(NULL, UM_SKILLDESCRIBE, m_curQuest, 0, 0, 0);
			}
			break;

		case ID_CLOSE:
			g_windowMgr.DeleteWindow(WID_QUESTWND);
			break;

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
			int distance = m_h - oldHeight;
			if (distance)
				g_snapMgr.MoveConnectedObject(this, 0, distance, 3);
			break;
		}
		}
		break;

	case UM_SCROLLVERT:
	case UM_SCROLLVERTPAGEUP:
	case UM_SCROLLVERTPAGEDOWN: {
		int oldOffset = m_viewOffset;
		if (message == UM_SCROLLVERTPAGEUP)
			m_viewOffset = oldOffset - m_itemVertNum + 1;
		else if (message == UM_SCROLLVERTPAGEDOWN)
			m_viewOffset = oldOffset + m_itemVertNum - 1;
		else
			m_viewOffset = oldOffset + val1;

		if (oldOffset != m_viewOffset)
		{
			RecalcScrbarPos();
			Invalidate();
		}
		return m_viewOffset - oldOffset;
	}

	case UM_TAB:
		m_curTab = val1;
		if (m_curTab < 0)
			m_curTab = 0;
		if (m_curTab > 2)
			m_curTab = 2;
		Invalidate();
		SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
		break;

	case UM_RESIZE:
		if (val1 == RT_DOWNRIGHT)
		{
			int width = val2;
			int height = val3;
			if (width > 320)
				width = 320;
			if (height > 380)
				height = 380;
			if (width < 232)
				width = 232;
			if (height < 180)
				height = 180;

			height = (height / 40) * 40 + 6;
			width = ((width - 280) / 32) * 32 + 280;

			if (width != m_w || height != m_h)
			{
				Resize(width, height);
				m_itemVertNum = (m_h - 40) / 40;

				m_resizer->Move(m_w - 15, m_h - 15);
				m_scrollBar->Move(m_w - 15, 17);
				m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 46);
				RecalcScrbarPos();
				Invalidate();
				int posX[3] = { 3, m_w - 28, m_w - 14 };
				for (int i = 0; i < 3; ++i)
					m_sysButtons[i]->Move(posX[i], 3);

				m_button[0]->Move(m_w - 107, m_h - 24);
				m_button[1]->Move(m_w - 61, m_h - 24);
			}

			m_orgHeight = m_h;
			g_windowMgr.UpdateSnapInfo(this);
		}
		break;

	case UM_REFRESHITEM:
		Invalidate();
		RecalcScrbarPos();
		break;

	case UM_DROP:
		break;

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIQuestWnd::RecalcScrbarPos()
{
	int questCount = g_session.m_QuestList.size();
	int numLine = 0;
	if (m_scrollBar) {
		if (m_curTab < 2) {
			std::list<QUEST_INFO>::iterator it = g_session.m_QuestList.begin();
			std::list<QUEST_INFO>::iterator end = g_session.m_QuestList.end();
			while (it != end) {
				if (m_curTab == 0) {
					if (it->show == 1)
						++numLine;
				} else if (m_curTab == 1) {
					if (it->show == 0)
						++numLine;
				}
				++it;
			}
		} else {
			numLine = questCount;
		}

		int range = numLine - m_itemVertNum;
		if (range < 0)
			range = 0;
		if (range != 0)
			m_vertScrEnabled = 1;
		else
			m_vertScrEnabled = 0;

		if (m_vertScrEnabled) {
			m_scrollBar->SetRange(range, m_itemVertNum, numLine);
			if (m_viewOffset < 0)
				m_viewOffset = 0;
			if (m_viewOffset > numLine - m_itemVertNum)
				m_viewOffset = numLine - m_itemVertNum;
			m_scrollBar->SetPos(m_viewOffset);
		} else {
			m_viewOffset = 0;
			m_scrollBar->SetRange(0, m_itemVertNum, numLine);
		}
		m_scrollBar->Invalidate();
	}
}
UIItemStoreWnd::UIItemStoreWnd()
{
	m_curRadioBtn[0] = 0;
	m_curRadioBtn[1] = 0;
	m_curRadioBtn[2] = 0;
	m_curTab = 0;
	m_drawNumItem = 1;
	m_isBasket = 1;
	m_info = NULL;
}

UIItemStoreWnd::~UIItemStoreWnd()
{}

void UIItemStoreWnd::OnCreate(int cx, int cy)
{
	m_titleText = new UIStaticText;
	m_titleText->Create(70, 15);
	m_titleText->SetText(MsgStr(MSI_ITEMS_THAT_IS_STORED), 0);
	m_titleText->Move(16, 2);
	m_titleText->m_drawTwice = 1;
	AddChild(m_titleText);

	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWN);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_tab = new UITab;
	m_tab->Create(20, 82);
	m_tab->Move(0, 17);
	m_tab->SendMsg(this, UM_SETSTATE, 0, 0, 0, 0);
	AddChild(m_tab);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	int pos[][2] = { { m_w - 61, m_h - 24 } };
	for (int i = 0; i < 1; ++i) {
		m_button[i] = new UIBitmapButton;
		m_button[i]->SetBitmapName((pathName + "btn_close" + ".bmp").c_str(), BS_NORMAL);
		m_button[i]->SetBitmapName((pathName + "btn_close_a" + ".bmp").c_str(), BS_MOUSEON);
		m_button[i]->SetBitmapName((pathName + "btn_close_b" + ".bmp").c_str(), BS_PRESSED);
		m_button[i]->Create(m_button[i]->GetBitmapWidth(), m_button[i]->GetBitmapHeight());
		m_button[i]->Move(pos[i][0], pos[i][1]);
		m_button[i]->SetId(ID_CLOSE);
		AddChild(m_button[i]);
	}

	m_countInfoText = "";
	m_itemVertNum = (cy - 38) / 32;
	m_itemHorzNum = 1;
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIItemStoreWnd::OnDraw()
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

	{
		unsigned long rr, gg, bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);

		DrawBox(20, 17,
			(m_w - 60) % 32, m_h - 38,
			RGB(rr, gg, bb));
		DrawBox(20, ((m_h - 38) / 32) * 32 + 17,
			m_w - 24, (m_h - 38) % 32,
			RGB(rr, gg, bb));
	}

	char* bmpName[2][3] = {
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid.bmp",
		},
		{
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp",
			"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp",
		},
	};

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][0]), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][1]), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	loopCnt = (m_w - 42) / 21 + (((m_w - 42) % 21) ? 1 : 0);
	for (i = 0; i < loopCnt; ++i) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp(bmpName[m_isBasket][2]), false);
		DrawBitmap((i + 1) * 21, m_h - bmp->m_height, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		int itemCols = (m_w - 40) / 32;
		int itemRows = (m_h - 38) / 32;
		for (int hLoop = 0; hLoop < itemRows; ++hLoop)
			DrawItemFrame(40, hLoop * 32 + 17);

		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(72, 17, itemCols * 32 - 32, itemRows * 32, RGB(rr, gg, bb));
	}

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		int skipLimit = m_itemHorzNum;
		skipLimit *= m_viewOffset;
		if (itemCount < skipLimit) {
			++it;
			++itemCount;
			continue;
		} else if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		int itemX = (drawCount % m_itemHorzNum) * 32 + 40;
		int itemY = (drawCount / m_itemHorzNum) * 32 + 17;
		DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);
		DrawItemTextRect(itemX + 40, itemY + 12, 180, *it, 0, 0, 12);

		if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
			break;

		it++;
		++drawCount;
		++itemCount;
	}

	TextOutA(16, m_h - 20, m_countInfoText.c_str(), 0, 0, 12, 0);
}

void UIItemStoreWnd::OnLBtnDown(int x, int y)
{
	if (y < 17)
		UIFrameWnd::OnLBtnDown(x, y);

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* mode = g_modeMgr.GetCurMode();
		mode->SendMsg(MM_BEGIN_DRAG_FROM_ITEMSTOREWND, itemInfo.m_itemIndex, itemInfo.m_num, 0);
	}
}

void UIItemStoreWnd::OnMouseHover(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		int gx = ((x - 40) / 32) * 32 + 40;
		int gy = ((y - 17) / 32) * 32 + 1;
		GetGlobalCoor(gx, gy);

		char itemName[128];
		g_modeMgr.GetCurMode()->MakeHelpBalloon(
			itemInfo.GetItemName(itemName),
			gx,
			gy,
			g_session.GetItemDisColor(&itemInfo));
	}
}

void UIItemStoreWnd::OnRBtnDown(int x, int y)
{
	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		UIWindow* w = g_windowMgr.MakeWindow(WID_ITEMCOLLECTIONWND);
		w->SendMsg(NULL, UM_ITEMEXPLANATION, (int)&itemInfo, 0, 0, 0);
	}
}
ITEM_INFO UIItemStoreWnd::GetItemInfo(int x, int y)
{
	if (x < 40 || x >= 72 || y < 17 || y >= m_itemVertNum * 32 + 17)
		return ITEM_INFO();

	int col = (x - 40) / 32;
	int row = (y - 17) / 32;
	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		if (drawCount == row * m_itemHorzNum + col)
			return *it;

		++it;
		++drawCount;
		++itemCount;
	}

	return ITEM_INFO();
}

void UIItemStoreWnd::DragAndDrop(int x, int y, DRAG_INFO const *const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
	} else {
		switch (dragInfo->m_dragType) {
		case DT_FROM_ITEMWND:
		case DT_FROM_MERCHANTITEMWND:
			if (dragInfo->m_numDragItem == 1) {
				SendMsg(0, UM_DROP, 0, 0, 0, 0);
				return;
			}

			if (g_session.GetEquipArrowIndex() == dragInfo->m_dragItemIndex) {
				ITEM_INFO arrowItem = g_session.GetItemInfoByIndex(dragInfo->m_dragItemIndex);
				if (arrowItem.m_itemIndex) {
					g_modeMgr.GetGameMode()->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
					g_windowMgr.ErrorMsg(MsgStr(MSI_UNEQUIP_ARROW), ME_OK, TRUE, FALSE, 0);
					return;
				}
			} else {
				g_windowMgr.ShowWindow(WID_ITEMDROPCNTWND, 1);
				UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
				wnd->SendMsg(0, UM_SETSTATE, 7, 0, 0, 0);
			}
			return;
		default:
			CGameMode* gameMode = g_modeMgr.GetGameMode();
			gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
			return;
		}
	}
}

void UIItemStoreWnd::StoreInfo()
{
	if (m_info) {
		m_info->w = m_w;
		m_info->h = m_h;
		m_info->x = m_x;
		m_info->y = m_y;
	}
}
int UIItemStoreWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	switch (message) {
	case UM_SETCOUNTINFO: {
		char buf[64];
		sprintf(buf, "%d/%d", val1, val2);
		m_countInfoText = buf;
		Invalidate();
		break;
	}

	case UM_SETINFO:
		m_info = (ItemStoreWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();

		Move(m_info->x, m_info->y);
		SendMsg(NULL, UM_RESIZE, RT_DOWN, m_info->w, m_info->h, 0);
		m_curRadioBtn[0] = 3;
		m_curRadioBtn[1] = 3;
		m_curRadioBtn[2] = 4;
		Invalidate();
		SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
		break;

	case UM_COMMAND:
		if (val1 == ID_CLOSE)
		{
			CMode* mode = g_modeMgr.GetCurMode();
			mode->SendMsg(MM_CLOSE_STORE, 0, 0, 0);
		}
		break;

	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		switch (gameMode->GetDragType()) {
		case DT_FROM_ITEMWND:
			gameMode->SendMsg(MM_ADD_ITEM_TO_STORE,
				gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
			break;
		case DT_FROM_MERCHANTITEMWND:
			gameMode->SendMsg(MM_MOVE_ITEM_FROM_CART_TO_STORE,
				gameMode->GetDragItemIndex(), gameMode->GetNumDragItem(), 0);
			break;
		}

		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		break;
	}

	case UM_RESIZE: {
		if (val1 == RT_DOWN) {
			int width = val2;
			int height = val3;
			if (width > 320)
				width = 320;
			if (height > 336)
				height = 336;
			if (width < 232)
				width = 232;
			if (height < 120)
				height = 120;

			width = ((width - 280) / 32) * 32 + 280;
			height = ((height - 120) / 32) * 32 + 120;

			if (width != m_w || height != m_h) {
				Resize(width, height);
				m_itemHorzNum = 1;
				m_itemVertNum = (height - 38) / 32;
				m_resizer->Move(m_w - 15, m_h - 15);
				m_button[0]->Move(m_w - 61, m_h - 24);
				m_scrollBar->Move(m_w - 15, 14);
				m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);
				RecalcScrbarPos();
			}

			g_windowMgr.UpdateSnapInfo(this);
		}
		break;
	}

	case UM_REFRESHITEM: {
		m_itemList.clear();

		int numItem = g_session.GetNumStoreItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetStoreItemInfoBySequence(i);
			if (itemInfo.IsValid()) {
				if (Sort(&itemInfo))
					m_itemList.push_back(itemInfo);
			}
		}

		Invalidate();
		RecalcScrbarPos();
		break;
	}

	case UM_TAB:
		m_curTab = val1;
		if (m_curTab < 0)
			m_curTab = 0;
		if (m_curTab > 2)
			m_curTab = 2;
		Invalidate();
		SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
		break;

	default:
		return UIItemBaseWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UIItemStoreWnd::RefreshRadioBtn() { Invalidate(); }

bool UIItemStoreWnd::Sort(ITEM_INFO const *const itemInfo)
{
	UIItemStoreWnd *wnd = this;
	int itemType = itemInfo->m_itemType;

	switch (wnd->m_curTab) {
	case 0:
		if (itemType == 0 || itemType == 1 || itemType == 2 || itemType == 18) {
			switch (wnd->m_curRadioBtn[0]) {
			case 0:
				if (itemType == 0)
					return true;
				break;
			case 2:
				if (itemType == 1 || itemType == 2)
					return true;
				break;
			case 3:
				return true;
			}
		}
		break;
	case 1:
		switch (itemType) {
		case 4:
		case 5:
		case 8:
		case 9:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			switch (wnd->m_curRadioBtn[1]) {
			case 0:
				if (itemType == 5 || itemType == 9 || itemType == 8)
					return true;
				break;
			case 1:
				if (itemType == 4 || (itemType > 10 && itemType <= 14))
					return true;
				break;
			case 3:
				return true;
			}
			break;
		}
		return false;
	case 2:
		if (itemType == 6 || itemType == 3 || itemType == 7 ||
			itemType == 10 || itemType == 16 || itemType == 17) {
			if (wnd->m_curRadioBtn[2] == 4)
				return true;
		}
		break;
	}

	return false;
}
UISkillListWnd::UISkillListWnd()
{
	m_button[0] = NULL;
	m_button[1] = NULL;
	m_info = NULL;
	m_curItem = -1;
	m_itemHorzNum = 1;
}

UISkillListWnd::~UISkillListWnd()
{}

void UISkillListWnd::OnCreate(int cx, int cy)
{
	m_resizer = new UIResizer;
	m_resizer->Create(16, 16);
	m_resizer->SetType(RT_DOWN);
	m_resizer->SetBitmap("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\btn_resize.bmp");
	m_resizer->Move(m_w - 15, m_h - 15);
	m_resizer->SetColor(0xffff00ff, 0xffff00ff);
	AddChild(m_resizer);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 14, 1, m_h - 38);
	AddChild(m_scrollBar);

	std::string pathName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\";
	char* buttonName[2][3] = {
		{ "btn_use", "btn_use_a", "btn_use_b" },
		{ "btn_close", "btn_close_a", "btn_close_b" },
	};
	int ids[2] = { ID_OK, ID_CLOSE };
	int pos[2][2] = {
		{ m_w - 107, m_h - 24 },
		{ m_w - 61, m_h - 24 },
	};

	int i;
	for (i = 0; i < 2; ++i) {
		m_button[i] = new UIBitmapButton;
		m_button[i]->SetBitmapName((pathName + buttonName[i][0] + ".bmp").c_str(), BS_NORMAL);
		m_button[i]->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON);
		m_button[i]->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
		m_button[i]->Create(m_button[i]->GetBitmapWidth(), m_button[i]->GetBitmapHeight());
		m_button[i]->Move(pos[i][0], pos[i][1]);
		m_button[i]->SetId(ids[i]);
		AddChild(m_button[i]);
	}

	m_itemHorzNum = 1;
	m_itemVertNum = (cy - 38) / 32;

	int posX[3] = { 3, m_w - 28, m_w - 14 };
	int ids2[3] = { ID_BASE, ID_MINI, ID_CLOSE };
	char* buttonName2[8] = { "base", "mini", "close" };
	char* toolTip[8] = { "", "", "Alt+S" };
	std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_";
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

	{
		int ids[7] = { ID_LVLUP1, ID_LVLUP2, ID_LVLUP3, ID_LVLUP4, ID_LVLUP5, ID_LVLUP6, ID_LVLUP7 };
		std::string preBmpName1 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\skill_up_a.bmp";
		std::string preBmpName2 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\skill_up_b.bmp";
		std::string preBmpName3 = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\skill_up_c.bmp";

		for (i = 0; i < 7; ++i) {
			m_incArrowButton[i] = new UIBitmapButton;
			m_incArrowButton[i]->SetBitmapName(preBmpName1.c_str(), BS_NORMAL);
			m_incArrowButton[i]->SetBitmapName(preBmpName2.c_str(), BS_MOUSEON);
			m_incArrowButton[i]->SetBitmapName(preBmpName3.c_str(), BS_PRESSED);
			m_incArrowButton[i]->Create(m_incArrowButton[i]->GetBitmapWidth(), m_incArrowButton[i]->GetBitmapHeight());
			m_incArrowButton[i]->Move(78, 21 + i * 32);
			m_incArrowButton[i]->SetId(ids[i]);
			AddChild(m_incArrowButton[i]);
		}
	}

	{
		int ids[7] = { ID_LVLSELECTDN1, ID_LVLSELECTDN2, ID_LVLSELECTDN3, ID_LVLSELECTDN4, ID_LVLSELECTDN5, ID_LVLSELECTDN6, ID_LVLSELECTDN7 };
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\arw_left.bmp";

		for (i = 0; i < 7; ++i) {
			m_lvlSelectDnButton[i] = new UIBitmapButton;
			m_lvlSelectDnButton[i]->SetBitmapName(preBmpName.c_str(), BS_NORMAL);
			m_lvlSelectDnButton[i]->SetBitmapName(preBmpName.c_str(), BS_MOUSEON);
			m_lvlSelectDnButton[i]->SetBitmapName(preBmpName.c_str(), BS_PRESSED);
			m_lvlSelectDnButton[i]->Create(m_lvlSelectDnButton[i]->GetBitmapWidth(), m_lvlSelectDnButton[i]->GetBitmapHeight());
			m_lvlSelectDnButton[i]->Move(112, 34 + i * 32);
			m_lvlSelectDnButton[i]->SetId(ids[i]);
			AddChild(m_lvlSelectDnButton[i]);
		}
	}

	{
		int ids[7] = { ID_LVLSELECTUP1, ID_LVLSELECTUP2, ID_LVLSELECTUP3, ID_LVLSELECTUP4, ID_LVLSELECTUP5, ID_LVLSELECTUP6, ID_LVLSELECTUP7 };
		std::string preBmpName = "\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\arw_right.bmp";

		for (i = 0; i < 7; ++i) {
			m_lvlSelectUpButton[i] = new UIBitmapButton;
			m_lvlSelectUpButton[i]->SetBitmapName(preBmpName.c_str(), BS_NORMAL);
			m_lvlSelectUpButton[i]->SetBitmapName(preBmpName.c_str(), BS_MOUSEON);
			m_lvlSelectUpButton[i]->SetBitmapName(preBmpName.c_str(), BS_PRESSED);
			m_lvlSelectUpButton[i]->Create(m_lvlSelectUpButton[i]->GetBitmapWidth(), m_lvlSelectUpButton[i]->GetBitmapHeight());
			m_lvlSelectUpButton[i]->Move(192, 34 + i * 32);
			m_lvlSelectUpButton[i]->SetId(ids[i]);
			AddChild(m_lvlSelectUpButton[i]);
		}
	}

	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UISkillListWnd::RefreshButton()
{
	int i = 0, y = 34;
	for (; y < 258; ++i, y += 32) {
		SKILL_INFO sInfo = GetSkillInfoByViewCnt(i);

		int skillPoint = 0;
		if (m_typeWnd == 0)
			skillPoint = g_session.GetSkillPoint();
		else if (m_typeWnd == 1)
			skillPoint = g_session.GetHomunSkillPoint();
		else if (m_typeWnd == 2)
			skillPoint = g_session.GetMerSkillPoint();

		if (i < m_itemVertNum && m_viewOffset + i < m_itemList.size() &&
			skillPoint && sInfo.m_isValid && sInfo.upgradable) {
			m_incArrowButton[i]->Move(78, y - 13);
		} else {
			m_incArrowButton[i]->Move(-200, y - 17);
		}

		if (i < m_itemVertNum && m_viewOffset + i < m_itemList.size() &&
			sInfo.m_isValid && sInfo.level && SKILL_INFO::IsLevelUseSkill(sInfo.SKID)) {
			m_lvlSelectDnButton[i]->Move(112, y);
			m_lvlSelectUpButton[i]->Move(192, y);
		} else {
			m_lvlSelectDnButton[i]->Move(-200, y);
			m_lvlSelectUpButton[i]->Move(-200, y);
		}
	}
}
void UISkillListWnd::OnDraw()
{
	CFile fp;
	char text[80];
	CBitmapRes* bmp;

	if (!g_isSkillEntry) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"),
			false);
		DrawBitmap(0, 0, bmp, false);
	} else {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left2.bmp"),
			false);
		DrawBitmap(0, 0, bmp, false);
	}

	if (!g_isSkillEntry) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"),
			false);
		DrawBitmap(m_w - bmp->m_width, 0, bmp, false);
	} else {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right2.bmp"),
			false);
		DrawBitmap(m_w - bmp->m_width, 0, bmp, false);
	}

	int drawCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int titleLoop = 0; titleLoop < drawCount; ++titleLoop) {
		if (!g_isSkillEntry) {
			bmp = (CBitmapRes*)g_resMgr().Get(
				UIBmp(
					"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"),
				false);
			DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
		} else {
			bmp = (CBitmapRes*)g_resMgr().Get(
				UIBmp(
					"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid2.bmp"),
				false);
			DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
		}
	}

	if (m_h == 17)
		return;

	drawCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int leftLoop = 0; leftLoop < drawCount; ++leftLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_left.bmp"),
			false);
		DrawBitmap(0, leftLoop * 8 + 17, bmp, false);
	}

	drawCount = (m_h - 38) / 8 + ((m_h - 38) % 8 != 0);
	for (int rightLoop = 0; rightLoop < drawCount; ++rightLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\itemwin_right.bmp"),
			false);
		DrawBitmap(m_w - bmp->m_width, rightLoop * 8 + 17, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		int fillWidth = (m_w - 60) % 32;
		DrawBox(20, 17, fillWidth, m_h - 38, RGB(rr, gg, bb));
		int fillHeight = (m_h - 38) % 32;
		int fillRows = (m_h - 38) / 32;
		DrawBox(20, fillRows * 32 + 17, m_w - 24, fillHeight, RGB(rr, gg, bb));
	}

	{
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"),
			false);
		DrawBitmap(0, m_h - bmp->m_height, bmp, false);
	}

	{
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"),
			false);
		DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);
	}

	drawCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (int btnLoop = 0; btnLoop < drawCount; ++btnLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(
			UIBmp(
				"\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"),
			false);
		DrawBitmap(btnLoop * 21 + 21, m_h - bmp->m_height, bmp, false);
	}

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		int itemCols = (m_w - 40) / 32;
		int itemRows = (m_h - 38) / 32;
		for (int frameLoop = 0; frameLoop < itemRows; ++frameLoop) {
			DrawItemFrame(40, frameLoop * 32 + 17);
		}

		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(72, 17, itemCols * 32 - 32, itemRows * 32, RGB(rr, gg, bb));
	}

	if (m_typeWnd == 0) {
		TextOutA(17, 4, MsgStr(MSI_SKILLLIST), 0, 0, 12, 0xffffff);
		TextOutA(16, 3, MsgStr(MSI_SKILLLIST), 0, 0, 12, 0);
		sprintf(text, MsgStr(MSI_SKILLPOINT), g_session.GetSkillPoint());
	} else if (m_typeWnd == 1) {
		TextOutA(17, 4, MsgStr(MSI_HOMUN_SKILLLIST), 0, 0, 12, 0xffffff);
		TextOutA(16, 3, MsgStr(MSI_HOMUN_SKILLLIST), 0, 0, 12, 0);
		sprintf(text, MsgStr(MSI_SKILLPOINT), g_session.GetHomunSkillPoint());
	} else if (m_typeWnd == 2) {
		TextOutA(17, 4, MsgStr(MSI_MER_SKILLLIST), 0, 0, 12, 0xffffff);
		TextOutA(16, 3, MsgStr(MSI_MER_SKILLLIST), 0, 0, 12, 0);
		sprintf(text, MsgStr(MSI_SKILLPOINT), g_session.GetMerSkillPoint());
	}
	TextOutA(16, m_h - 20, text, 0, 0, 12, 0);

	drawCount = 0;
	int itemCount = 0;
	std::list<SKILL_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
		} else {
			int itemX = (drawCount % m_itemHorzNum) * 32 + 72;
			int itemY = (drawCount / m_itemHorzNum) * 32 + 17;

			if (m_curItem == itemCount) {
				unsigned long rr;
				unsigned long gg;
				unsigned long bb;
				g_skinMgr.GetColorChipColor(6, 2, rr, gg, bb);
				if (it->level == 0)
					g_skinMgr.GetColorChipColor(10, 2, rr, gg, bb);
				else if (it->spcost == 0)
					g_skinMgr.GetColorChipColor(14, 2, rr, gg, bb);
				DrawBox(itemX + 32, itemY + 2, 164, 28, RGB(rr, gg, bb));
			}

			g_session.GetSkillBitmapFileName(it->skillName, text);
			bmp = (CBitmapRes*)g_resMgr().Get(text, false);
			DrawBitmap(itemX - 28, itemY + 4, bmp, true);

			int r = 0;
			int g = 0;
			int b = 0;
			if (it->level != 0) {
				if (SKILL_INFO::IsLevelUseSkill(it->SKID)) {
					sprintf(text, "Lv :");
					TextOutA(itemX + 52, itemY + 17, text, 0, 0, 12, 0);

					int skillUseLevel;
					if (m_typeWnd == 0)
						skillUseLevel = g_session.GetSkillUseLevel(it->SKID);
					else if (m_typeWnd == 1)
						skillUseLevel = g_session.GetHomunSkillUseLevel(it->SKID);
					else if (m_typeWnd == 2)
						skillUseLevel = g_session.GetMerSkillUseLevel(it->SKID);

					if (skillUseLevel <= 0 || skillUseLevel > it->level)
						skillUseLevel = it->level;

					sprintf(text, "%2d", skillUseLevel);
					if (skillUseLevel == it->level)
						TextOutA(itemX + 77, itemY + 17, text, 0, 0, 12, 0);
					else
						TextOutA(itemX + 77, itemY + 17, text, 0, 0, 12, 0xff);

					sprintf(text, "/ %2d", it->level);
					TextOutA(itemX + 96, itemY + 17, text, 0, 0, 12, 0);
				} else {
					sprintf(text, "Lv : %d", it->level);
					TextOutA(itemX + 41, itemY + 17, text, 0, 0, 12, 0);
				}

				if (it->type == SKILLTYPE_PASSIVE) {
					sprintf(text, MsgStr(MSI_PASSIVE));
				} else {
					if (SKILL_INFO::IsLevelUseSkill(it->SKID)) {
						const std::vector<int>& spList = it->LevelUseSpAmountList();
						int skillUseLevel;
						if (m_typeWnd == 0)
							skillUseLevel = g_session.GetSkillUseLevel(it->SKID);
						else if (m_typeWnd == 1)
							skillUseLevel = g_session.GetHomunSkillUseLevel(it->SKID);
						else if (m_typeWnd == 2)
							skillUseLevel = g_session.GetMerSkillUseLevel(it->SKID);

						if (spList.size() >= it->level && skillUseLevel <= it->level) {
							int spcost = spList[skillUseLevel - 1];
							sprintf(text, "Sp : %d", spcost);
						} else
							sprintf(text, "Sp : %d", it->spcost);
					} else {
						sprintf(text, "Sp : %d", it->spcost);
					}
				}
				TextOutA(itemX + 140, itemY + 17, text, 0, 0, 12, 0);
			} else {
				r = 0x78;
				g = 0x78;
				b = 0x78;
			}

			TextOutA(itemX + 40, itemY + 5, it->GetName(), 0, 0, 12, RGB(r, g, b));

			if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
				break;

			++it;
			++drawCount;
			++itemCount;
		}
	}
}

SKILL_INFO UISkillListWnd::GetSkillInfoByViewCnt(int viewCnt)
{
	std::list<SKILL_INFO>::iterator it = m_itemList.begin();
	int count = 0;
	if (it != m_itemList.end()) {
		while (it != m_itemList.end()) {
			if (m_viewOffset + viewCnt == count)
				return *it;
			++it;
			++count;
		}
	}

	return SKILL_INFO(0);
}
SKILL_INFO UISkillListWnd::GetSkillInfo(int x, int y)
{
	if (x < 40 || x >= 72 || y < 17 || y >= m_itemVertNum * 32 + 17)
		return SKILL_INFO(0);

	int row = (y - 17) / 32;
	int drawCount = 0;
	int itemCount = 0;
	std::list<SKILL_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_itemHorzNum * m_viewOffset) {
			++it;
			++itemCount;
			continue;
		}
		if (drawCount == row * m_itemHorzNum)
			return *it;
		++it;
		++drawCount;
		++itemCount;
	}
	return SKILL_INFO(0);
}


int UISkillListWnd::GetCurItem(int x, int y)
{
	if (x < 40 || y < 17 || y >= m_itemVertNum * 32 + 17)
		return -1;

	int yCell = (y - 17) / 32;
	int drawCount = 0;
	int itemCount = 0;
	std::list<SKILL_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
		} else {
			if (drawCount == m_itemHorzNum * yCell)
				return itemCount;
			++it;
			++drawCount;
			++itemCount;
		}
	}
	return -1;
}

SKILL_INFO UISkillListWnd::GetSkillInfo(int curItem)
{
	int count = 0;
	SKILL_INFO skillInfo(0);
	std::list<SKILL_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (curItem == count) {
			skillInfo = *it;
			break;
		}
		++it;
		++count;
	}
	return skillInfo;
}

void UISkillListWnd::OnLBtnDblClk(int x, int y)
{
	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
		return;
	}

	if (m_curItem == -1) return;

	SKILL_INFO skillInfo = GetSkillInfo(m_curItem);
	if (skillInfo.m_isValid && skillInfo.level && skillInfo.spcost)
		SendMsg(NULL, UM_USESKILL, (int)&skillInfo, 0, 0, 0);
}

void UISkillListWnd::OnLBtnDown(int x, int y)
{
	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
		return;
	}

	int prev = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (prev != m_curItem)
		Invalidate();

	SKILL_INFO skillInfo = GetSkillInfo(x, y);
	if (skillInfo.m_isValid && skillInfo.level && skillInfo.type) {
		CMode *mode = g_modeMgr.GetCurMode();
		if (m_typeWnd == 0) {
			mode->SendMsg(MM_BEGIN_DRAG_FROM_SKILLLISTWND,
				skillInfo.SKID,
				(int)skillInfo.skillName,
				g_session.GetSkillUseLevel(skillInfo.SKID));
		} else if (m_typeWnd == 1) {
			mode->SendMsg(MM_BEGIN_DRAG_FROM_HOSKILLLISTWND,
				skillInfo.SKID,
				(int)skillInfo.skillName,
				g_session.GetHomunSkillUseLevel(skillInfo.SKID));
		} else if (m_typeWnd == 2) {
			mode->SendMsg(MM_BEGIN_DRAG_FROM_MERSKILLLISTWND,
				skillInfo.SKID,
				(int)skillInfo.skillName,
				g_session.GetMerSkillUseLevel(skillInfo.SKID));
		}
	}
}

void UISkillListWnd::OnRBtnDown(int x, int y)
{
	int prev = m_curItem;
	m_curItem = GetCurItem(x, y);
	if (prev != m_curItem) {
		Invalidate();
	}
}

void UISkillListWnd::OnRBtnUp(int x, int y)
{
	SendMsg(NULL, UM_MAKEMENU, 0, 0, 0, 0);
}

void UISkillListWnd::OnMouseHover(int x, int y)
{
	if (g_isSkillEntry == 1) {
		int diff = (int)(timeGetTime() - g_SkillEntryTime);
		if (diff > 1000 || diff < 0) {
			g_isSkillEntry = 0;
			Invalidate();
		}
	}
}

void UISkillListWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}
int UISkillListWnd::SendMsg(UIWindow *sender, int message, int val1, int val2, int val3, int val4)
{
	if (g_isSkillEntry == 1) {
		int diff = (int)(timeGetTime() - g_SkillEntryTime);
		if (diff > 1000 || diff < 0) {
			g_isSkillEntry = 0;
			Invalidate();
		}
	}

	switch (message) {
	case UM_SETINFO:
		m_info = (SkillListWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		SendMsg(NULL, UM_RESIZE, RT_DOWN, m_info->w, m_info->h, 0);
		break;

	case UM_RESIZE: {
		if (val1 != RT_DOWN)
			break;

		int width = val2;
		int height = val3;
		if (width > 320)
			width = 320;
		if (height > 288)
			height = 288;
		if (width < 232)
			width = 232;
		if (height < 120)
			height = 120;

		width = ((width - 280) / 32) * 32 + 280;
		height = ((height - 120) / 32) * 32 + 120;

		if (width != m_w || height != m_h) {
			Resize(width, height);

			m_itemHorzNum = 1;
			m_itemVertNum = (height - 38) / 32;
			m_resizer->Move(m_w - 15, m_h - 15);
			m_button[0]->Move(m_w - 107, m_h - 24);
			m_button[1]->Move(m_w - 61, m_h - 24);
			m_scrollBar->Move(m_w - 15, 14);
			m_scrollBar->Resize(m_scrollBar->GetWidth(), m_h - 38);

			int i = 0;
			int y = 17;
			for (; y < 241; ++i, y += 32) {
				if (i < m_itemVertNum && m_viewOffset + i < m_itemList.size())
					m_incArrowButton[i]->Move(53, y);
				else
					m_incArrowButton[i]->Move(-200, y);
			}

			RecalcScrbarPos();
			RefreshButton();
		}
		g_windowMgr.UpdateSnapInfo(this);
		break;
	}

	case UM_USESKILL: {
		SKILL_INFO* skillInfo = (SKILL_INFO*)val1;
		if (!skillInfo->level)
			break;

		CMode* mode = g_modeMgr.GetCurMode();
		int skillUseLevel;
		if (m_typeWnd == 0)
			skillUseLevel = g_session.GetSkillUseLevel(skillInfo->SKID);
		else if (m_typeWnd == 1)
			skillUseLevel = g_session.GetHomunSkillUseLevel(skillInfo->SKID);
		else if (m_typeWnd == 2)
			skillUseLevel = g_session.GetMerSkillUseLevel(skillInfo->SKID);

		mode->SendMsg(MM_USING_SKILL, val1, skillUseLevel, 0);
		break;
	}

	case UM_COMMAND:
		switch (val1) {
		case ID_CLOSE:
			if (m_typeWnd == 0)
				g_windowMgr.DeleteWindow(WID_SKILLLISTWND);
			else if (m_typeWnd == 1)
				g_windowMgr.DeleteWindow(WID_HOMUNSKILLLISTWND);
			else if (m_typeWnd == 2)
				g_windowMgr.DeleteWindow(WID_MERSKILLLISTWND);
			break;

		case ID_OK:
			if (m_curItem == -1)
				break;
			{
				SKILL_INFO skillInfo = GetSkillInfo(m_curItem);
				RAG_BUDGET_FILL(16)
				if (skillInfo.m_isValid && skillInfo.level && skillInfo.spcost)
					SendMsg(NULL, UM_USESKILL, (int)&skillInfo, 0, 0, 0);
				break;
			}

		case ID_LVLUP1:
		case ID_LVLUP2:
		case ID_LVLUP3:
		case ID_LVLUP4:
		case ID_LVLUP5:
		case ID_LVLUP6:
		case ID_LVLUP7:
			if (m_typeWnd == 0) {
				if (!g_session.GetSkillPoint())
					break;
			} else if (m_typeWnd == 1) {
				if (!g_session.GetHomunSkillPoint())
					break;
			} else if (m_typeWnd == 2) {
				if (!g_session.GetMerSkillPoint())
					break;
			}

			if (g_isSkillEntry == 0) {
				int viewCnt = val1 - ID_LVLUP1;
				int skillIndex = m_viewOffset + viewCnt;
				SKILL_INFO skillInfo = GetSkillInfo(skillIndex);
				g_EntrySKID = skillInfo.SKID;
				if (skillInfo.m_isValid) {
					CMode* curMode = g_modeMgr.GetCurMode();
					curMode->SendMsg(MM_UPGRADE_SKILLLEVEL, skillInfo.SKID, 0, 0);
				}
				break;
			} else {
				int viewCnt = val1 - ID_LVLUP1;
				int skillIndex = m_viewOffset + viewCnt;
				SKILL_INFO skillInfo = GetSkillInfo(skillIndex);
				if (g_EntrySKID == skillInfo.SKID) {
					if (skillInfo.m_isValid) {
						CMode* curMode = g_modeMgr.GetCurMode();
						curMode->SendMsg(MM_UPGRADE_SKILLLEVEL, skillInfo.SKID, 0, 0);
					}
				}
				break;
			}

		case ID_LVLSELECTDN1:
		case ID_LVLSELECTDN2:
		case ID_LVLSELECTDN3:
		case ID_LVLSELECTDN4:
		case ID_LVLSELECTDN5:
		case ID_LVLSELECTDN6:
		case ID_LVLSELECTDN7: {
			int viewCnt = val1 - ID_LVLSELECTDN1;
			int skillIndex = m_viewOffset + viewCnt;
			SKILL_INFO skillInfo = GetSkillInfo(skillIndex);
			int skillId = skillInfo.SKID;
			if (skillInfo.m_isValid) {
				int skillUseLevel;
				if (m_typeWnd == 0)
					skillUseLevel = g_session.GetSkillUseLevel(skillId);
				else if (m_typeWnd == 1)
					skillUseLevel = g_session.GetHomunSkillUseLevel(skillId);
				else if (m_typeWnd == 2)
					skillUseLevel = g_session.GetMerSkillUseLevel(skillId);

				--skillUseLevel;
				if (skillUseLevel <= 0)
					skillUseLevel = 1;
				if (skillUseLevel > skillInfo.level)
					skillUseLevel = skillInfo.level;

				if (m_typeWnd == 0)
					g_session.SetSkillUseLevel(skillId, skillUseLevel);
				else if (m_typeWnd == 1)
					g_session.SetHomunSkillUseLevel(skillId, skillUseLevel);
				else if (m_typeWnd == 2)
					g_session.SetMerSkillUseLevel(skillId, skillUseLevel);

				Invalidate();
			}
			break;
		}

		case ID_LVLSELECTUP1:
		case ID_LVLSELECTUP2:
		case ID_LVLSELECTUP3:
		case ID_LVLSELECTUP4:
		case ID_LVLSELECTUP5:
		case ID_LVLSELECTUP6:
		case ID_LVLSELECTUP7: {
			int viewCnt = val1 - ID_LVLSELECTUP1;
			int skillIndex = m_viewOffset + viewCnt;
			SKILL_INFO skillInfo = GetSkillInfo(skillIndex);
			int skillId = skillInfo.SKID;
			if (skillInfo.m_isValid) {
				int skillUseLevel;
				if (m_typeWnd == 0)
					skillUseLevel = g_session.GetSkillUseLevel(skillId);
				else if (m_typeWnd == 1)
					skillUseLevel = g_session.GetHomunSkillUseLevel(skillId);
				else if (m_typeWnd == 2)
					skillUseLevel = g_session.GetMerSkillUseLevel(skillId);

				++skillUseLevel;
				if (skillUseLevel <= 0)
					skillUseLevel = 1;
				if (skillUseLevel > skillInfo.level)
					skillUseLevel = skillInfo.level;

				if (m_typeWnd == 0)
					g_session.SetSkillUseLevel(skillId, skillUseLevel);
				else if (m_typeWnd == 1)
					g_session.SetHomunSkillUseLevel(skillId, skillUseLevel);
				else if (m_typeWnd == 2)
					g_session.SetMerSkillUseLevel(skillId, skillUseLevel);

				Invalidate();
			}
			break;
		}

		default:
			break;
		}
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
			RefreshButton();
			Invalidate();
		}
		return m_viewOffset - oldViewOffset;
	}

	case UM_REFRESHITEM: {
		m_itemList.erase(m_itemList.begin(), m_itemList.end());

		int numItem;
		if (m_typeWnd == 0)
			numItem = g_session.GetSkillItemCount();
		else if (m_typeWnd == 1)
			numItem = g_session.GetHomunSkillItemCount();
		else if (m_typeWnd == 2)
			numItem = g_session.GetMerSkillItemCount();

		char count = 0;
		for (int i = 0; i < numItem; ++i) {
			SKILL_INFO skillInfo;
			if (m_typeWnd == 0)
				skillInfo = g_session.GetSkillItemInfoBySequence(i);
			else if (m_typeWnd == 1)
				skillInfo = g_session.GetHomunSkillItemInfoBySequence(i);
			else if (m_typeWnd == 2)
				skillInfo = g_session.GetMerSkillItemInfoBySequence(i);

			if (skillInfo.m_isValid) {
				if (skillInfo.upgradable || skillInfo.level) {
					m_itemList.push_back(skillInfo);
					++count;
				}

				if (SKILL_INFO::IsLevelUseSkill(skillInfo.SKID) && skillInfo.level) {
					int skillUseLevel;
					if (m_typeWnd == 0)
						skillUseLevel = g_session.GetSkillUseLevel(skillInfo.SKID);
					else if (m_typeWnd == 1)
						skillUseLevel = g_session.GetHomunSkillUseLevel(skillInfo.SKID);
					else if (m_typeWnd == 2)
						skillUseLevel = g_session.GetMerSkillUseLevel(skillInfo.SKID);

					if (skillUseLevel <= 0 || skillUseLevel > skillInfo.level) {
						if (m_typeWnd == 0)
							g_session.SetSkillUseLevel(skillInfo.SKID, skillInfo.level);
						else if (m_typeWnd == 1)
							g_session.SetHomunSkillUseLevel(skillInfo.SKID, skillInfo.level);
						else if (m_typeWnd == 2)
							g_session.SetMerSkillUseLevel(skillInfo.SKID, skillInfo.level);
					}
				}
			}
		}

		if (count != g_SkillEntryNum) {
			if (g_SkillEntryTime)
				g_isSkillEntry = 1;
			g_SkillEntryNum = count;
			g_SkillEntryTime = timeGetTime();
		}

		Invalidate();
		RecalcScrbarPos();
		RefreshButton();
		break;
	}

	case UM_MAKEMENU: {
		m_menuItems.erase(m_menuItems.begin(), m_menuItems.end());

		if (g_windowMgr.GetMenu())
			g_windowMgr.DeleteWindow(WID_MENUWND);

		if (m_curItem != -1) {
			SKILL_INFO skillInfo = GetSkillInfo(m_curItem);
			if (skillInfo.m_isValid) {
				m_menuItems.push_back(StringId(std::string(MsgStr(MSI_SEE_INFORMATION_OF_SKILL)).c_str(), 3));
			}
		}

		if (m_menuItems.size() == 0)
			break;

		UIFrameWnd* menu = g_windowMgr.MakeWindow(WID_MENUWND);
		for (int i = 0; i < m_menuItems.size(); ++i)
			menu->SendMsg(NULL, UM_ADDITEM, (int)m_menuItems[i].itemName.c_str(), 0, 0, 0);

		if (m_typeWnd == 0)
			menu->SendMsg(NULL, UM_SET_CALLER_WID, WID_SKILLLISTWND, 0, 0, 0);
		else if (m_typeWnd == 1)
			menu->SendMsg(NULL, UM_SET_CALLER_WID, WID_HOMUNSKILLLISTWND, 0, 0, 0);
		else if (m_typeWnd == 2)
			menu->SendMsg(NULL, UM_SET_CALLER_WID, WID_MERSKILLLISTWND, 0, 0, 0);

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
		break;
	}

	case UM_MENU_SELECTED:
		if (m_curItem == -1)
			break;
		{
			SKILL_INFO skillInfo = GetSkillInfo(m_curItem);
			if (skillInfo.m_isValid) {
				switch (m_menuItems[val1].id) {
				case 3: {
					UIWindow* wnd = g_windowMgr.MakeWindow(WID_SKILLDESCRIBEWND);
					wnd->SendMsg(NULL, UM_SKILLDESCRIBE, skillInfo.SKID, 0, 0, 0);
					break;
				}
				case 14: {
					UIWindow* wnd = g_windowMgr.MakeWindow(WID_SKILLNAMECHANGEWND);
					wnd->SendMsg(NULL, UM_SET_SKILL_INFO, skillInfo.SKID, 0, 0, 0);
					break;
				}
				}
			}
			break;
		}

	default:
		return UIFrameWnd::SendMsg(sender, message, val1, val2, val3, val4);
	}

	return 0;
}

void UISkillListWnd::OnWheel(int wheel)
{
	if (m_vertScrEnabled) {
		int scrollNum = m_itemVertNum - 1;
		if (scrollNum >= 3)
			scrollNum = 3;
		if (wheel < 0)
			SendMsg(m_scrollBar, UM_SCROLLVERT, scrollNum, 0, 0, 0);
		else
			SendMsg(m_scrollBar, UM_SCROLLVERT, -scrollNum, 0, 0, 0);
	}
}

void UISkillListWnd::RecalcScrbarPos()
{
	if (m_scrollBar) {
		int numCanDisplayItem = m_itemVertNum;
		int numLine = (m_itemList.size() % m_itemHorzNum != 0) + m_itemList.size() / m_itemHorzNum;
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
UIMerchantShopMakeWnd::UIMerchantShopMakeWnd()
{
	m_scrollBar = NULL;
	m_viewOffset = 0;
	m_vertScrEnabled = 0;
	m_info = NULL;
	m_itemHorzNum = 1;
	m_itemVertNum = 4;
}

UIMerchantShopMakeWnd::~UIMerchantShopMakeWnd()
{}

void UIMerchantShopMakeWnd::OnCreate(int cx, int cy)
{
	int maxEditNum = g_session.m_maxNumOfSellItemOfMerchant;

	m_title = MsgStr(MSI_MERCHANTSHOPMAKER);

	m_shopTitle = new UIEditCtrl;
	m_shopTitle->Create(m_w - 66, 16);
	m_shopTitle->Move(41, 22);
	m_shopTitle->SetTextLimit(36);
	m_shopTitle->SetFrameColor(232, 232, 232);
	m_shopTitle->SetText("");
	AddChild(m_shopTitle);
	g_windowMgr.SetFocusEdit(m_shopTitle);

	m_scrollBar = new UIScrollBar;
	m_scrollBar->Create(m_w - 15, 17, 1, m_h - 58);
	AddChild(m_scrollBar);

	m_numItemCountEdit = maxEditNum;
	if (m_numItemCountEdit > 13)
		m_numItemCountEdit = 13;

	m_itemHorzNum = 1;
	m_itemVertNum = 4;

	for (int i = 0; i < m_numItemCountEdit; ++i) {
		m_itemCountEdit[i] = new UIEditCtrlWheel;
		m_itemCountEdit[i]->Create(64, 16);
		m_itemCountEdit[i]->Move(m_w - 89, 40 + 18 * i);
		m_itemCountEdit[i]->SetTextLimit(12);
		m_itemCountEdit[i]->SetFrameColor(232, 232, 232);
		m_itemCountEdit[i]->SetText("0,000");
		AddChild(m_itemCountEdit[i]);
	}

	{
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
			button->SetBitmapName((pathName + buttonName[i][1] + ".bmp").c_str(), BS_MOUSEON); RAG_BUDGET_SITES(1)
			button->SetBitmapName((pathName + buttonName[i][2] + ".bmp").c_str(), BS_PRESSED);
			button->Create(button->GetBitmapWidth(), button->GetBitmapHeight());
			button->Move(pos[i][0], pos[i][1]);
			button->SetId(ids[i]);
			AddChild(button);
		}
	}

	m_defPushId = ID_OK;
	g_session.ClearMerchantHopeToSellItem();
	SendMsg(NULL, UM_REFRESHITEM, 0, 0, 0, 0);
}

void UIMerchantShopMakeWnd::OnDraw()
{
	ClearDC(0xff);

	CBitmapRes* bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_left.bmp"), false);
	DrawBitmap(0, 0, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_right.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, 0, bmp, false);

	int titleMidCount = (m_w - 24) / 12 + ((m_w - 24) % 12 != 0);
	for (int titleLoop = 0; titleLoop < titleMidCount; ++titleLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\titlebar_mid.bmp"), false);
		DrawBitmap(titleLoop * 12 + 12, 0, bmp, false);
	}

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\sys_base_off.bmp"), false);
	DrawBitmap(3, 3, bmp, true);

	{
		unsigned long rr;
		unsigned long gg;
		unsigned long bb;
		g_skinMgr.GetColorChipColor(2, 2, rr, gg, bb);
		DrawBox(0, 17, m_w, m_h - 38, RGB(rr, gg, bb));
	}

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_left2.bmp"), false);
	DrawBitmap(0, m_h - bmp->m_height, bmp, false);

	bmp = (CBitmapRes*)g_resMgr().Get(
		UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_right2.bmp"), false);
	DrawBitmap(m_w - bmp->m_width, m_h - bmp->m_height, bmp, false);

	int btnMidCount = (m_w - 42) / 21 + ((m_w - 42) % 21 != 0);
	for (int btnLoop = 0; btnLoop < btnMidCount; ++btnLoop) {
		bmp = (CBitmapRes*)g_resMgr().Get(UIBmp("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\basic_interface\\btnbar_mid2.bmp"), false);
		DrawBitmap(btnLoop * 21 + 21, m_h - bmp->m_height, bmp, false);
	}

	for (int editCount = 0; editCount < m_numItemCountEdit; ++editCount) {
		unsigned char offScreen = true;
		if (m_viewOffset <= editCount && m_viewOffset + m_itemVertNum > editCount)
			offScreen = false;
		m_itemCountEdit[editCount]->Move(m_w - 89 + offScreen * 340,
			(editCount - m_viewOffset) * 32 + 54);
	}

	for (int y = 44; y < 172; y += 32) {
		TextOutA(m_w - 135, y + 13, MsgStr(MSI_SELLPRICE), 0, 0, 12, 0);
		DrawItemFrame(3, y);
	}

	TextOutA(17, 4, MsgStr(MSI_MERCHANTSHOPMAKER), 0, 0, 12, 0xffffff);
	TextOutA(16, 3, MsgStr(MSI_MERCHANTSHOPMAKER), 0, 0, 12, 0);
	TextOutA(2, 25, MsgStr(MSI_STALL_NAME), 0, 0, 12, 0);

	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		int itemX = (drawCount % m_itemHorzNum) * 32 + 3;
		int itemY = (drawCount / m_itemHorzNum) * 32 + 44;
		DrawItemWithCount(itemX + 4, itemY + 4, *it, false, false);
		DrawItemText(itemX + 36, itemY + 13, *it, 0, 0, 12);

		if (drawCount + 1 == m_itemVertNum * m_itemHorzNum)
			break;

		++it;
		++drawCount;
		++itemCount;
	}
}

ITEM_INFO UIMerchantShopMakeWnd::GetItemInfo(int x, int y)
{
	if (x < 1 || x >= m_w - 20 || y < 27 || y >= m_itemVertNum * 32 + 44)
		return ITEM_INFO();

	int yCell = (y - 44) / 32;
	int drawCount = 0;
	int itemCount = 0;
	std::list<ITEM_INFO>::iterator it = m_itemList.begin();
	while (it != m_itemList.end()) {
		if (itemCount < m_viewOffset * m_itemHorzNum) {
			++it;
			++itemCount;
			continue;
		}

		if (g_session.IsPetEgg(atoi(it->m_itemName.c_str()))) {
			if (it->m_isDamaged) {
				++itemCount;
				++it;
				continue;
			}
		}

		if (drawCount == yCell * m_itemHorzNum) {
			if (x < 35)
				return *it;
			return ITEM_INFO();
		}

		++it;
		++drawCount;
		++itemCount;
	}

	return ITEM_INFO();
}

void UIMerchantShopMakeWnd::OnLBtnDown(int x, int y)
{
	if (y < 17) {
		UIFrameWnd::OnLBtnDown(x, y);
		return;
	}

	ITEM_INFO itemInfo = GetItemInfo(x, y);
	if (itemInfo.m_itemIndex) {
		CMode* mode = g_modeMgr.GetCurMode();
		mode->SendMsg(MM_BEGIN_DRAG_FROM_MERCHANTSHOPMAKEWND, itemInfo.m_itemIndex, 0, 0);
	}
}


void UIMerchantShopMakeWnd::OnWheel(int wheel)
{
	if (m_vertScrEnabled) {
		int scrollNum = m_itemVertNum - 1;
		if (scrollNum >= 3)
			scrollNum = 3;
		if (wheel < 0)
			SendMsg(m_scrollBar, UM_SCROLLVERT, scrollNum, 0, 0, 0);
		else
			SendMsg(m_scrollBar, UM_SCROLLVERT, -scrollNum, 0, 0, 0);
	}
}

void UIMerchantShopMakeWnd::DragAndDrop(int x, int y, const DRAG_INFO* const dragInfo)
{
	if (g_windowMgr.GetExchangeWnd()) {
		UIWindow::DragAndDrop(x, y, dragInfo);
		return;
	}
	if (dragInfo->m_dragType != DT_FROM_MERCHANTMIRRORITEMWND) {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
		return;
	}
	if (dragInfo->m_numDragItem == 1) {
		SendMsg(0, UM_DROP, 0, 0, 0, 0);
		return;
	}
	g_windowMgr.ShowWindow((WINDOWID)0xf, 1);
	UIWindow* wnd = g_windowMgr.GetItemDropCntWnd();
	wnd->SendMsg(0, UM_SETSTATE, 9, 0, 0, 0);
}

void UIMerchantShopMakeWnd::StoreInfo()
{
	if (m_info) {
		m_info->x = m_x;
		m_info->y = m_y;
		m_info->w = m_w;
		m_info->h = m_h;
	}
}

int UIMerchantShopMakeWnd::SendMsg(UIWindow* sender, int message, int val1, int val2, int val3, int val4)
{
	char cPrice[24];

	switch (message) {
	case UM_SETINFO:
		m_info = (MerchantShopMakeWndInfo*)val1;
		if (!m_info->IsValidInfo())
			m_info->MakeDefault();
		Move(m_info->x, m_info->y);
		break;
	case UM_DROP: {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		DRAG_INFO* dragInfo = gameMode->GetDragInfo();
		ITEM_INFO itemInfo;
		itemInfo.CopyItemInfo(dragInfo);

		if (dragInfo->m_dragType == DT_FROM_MERCHANTMIRRORITEMWND) {
			int numItem = g_session.GetNumMerchantHopeToSellItem();
			unsigned char isFull = m_numItemCountEdit <= numItem;
			Trace("m_numItemCountEdit (%d)", m_numItemCountEdit);

			if (!isFull) {
				if (g_session.GetMerchantMirrorItemNum(dragInfo->m_dragItemIndex)) {
					g_session.AddMerchantHopeToSellItem(itemInfo);
					g_session.DecMerchantMirrorItem(itemInfo);
				}
			} else {
				if (g_session.GetMerchantMirrorItemNum(dragInfo->m_dragItemIndex) &&
					g_session.GetMerchantHopeToSellItemNum(dragInfo->m_dragItemIndex)) {
					g_session.AddMerchantHopeToSellItem(itemInfo);
					g_session.DecMerchantMirrorItem(itemInfo);
				}
			}

			int editIndex = g_session.GetNumMerchantHopeToSellItem() - 1;
			if (editIndex < 0)
				editIndex = 0;
			else if (editIndex >= 13)
				editIndex = 12;

			if (m_itemCountEdit[editIndex] && !m_itemCountEdit[editIndex]->GetTextSize()) {
				m_itemCountEdit[editIndex]->SetText("0");
				g_windowMgr.SetFocusEdit(m_itemCountEdit[editIndex]);
				m_itemCountEdit[editIndex]->MakeSelect();
			}
		}

		gameMode->SendMsg(MM_CANCEL_DRAG, 0, 0, 0);
	}
		break;
	case UM_REFRESHITEM: {
		m_itemList.erase(m_itemList.begin(), m_itemList.end());
		int numItem = g_session.GetNumMerchantHopeToSellItem();
		for (int i = 0; i < numItem; ++i) {
			ITEM_INFO itemInfo = g_session.GetMerchantHopeToSellItemInfoBySequence(i);
			if (itemInfo.m_itemIndex)
				m_itemList.push_back(itemInfo);
		}
		RecalcScrbarPos();
		Invalidate();
		break;
	}
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
		case ID_OK:
			{
				CGameMode* gameMode = g_modeMgr.GetGameMode();

				if (g_windowMgr.GetChatRoomWnd() || g_windowMgr.GetChatRoomMakeWnd()) {
					g_windowMgr.ErrorMsg(
						"Can't open a shop and a chat room at once.",
						0, TRUE, FALSE, 0);
					g_windowMgr.DeleteWindow(WID_MERCHANTSHOPMAKEWND);
					break;
				}

				char moneyTxt[32];
				char priceText[32];
				int isExist0Zeny = 0;
				int isExist1000Zeny = 0;
				{
				int i;
				for (i = 0; i < m_numItemCountEdit; ++i) {
					const char* text = m_itemCountEdit[i]->GetText();
					sprintf(cPrice, "%s ", text);
					if (!g_session.DeleteCommaInPrice(cPrice)) {
						g_windowMgr.ErrorMsg(MsgStr(MSI_CAN_INPUT_NUMBER_ONLY), ME_OK, TRUE, FALSE, 0);
						return 0;
					}

					ITEM_INFO iInfo = g_session.GetMerchantHopeToSellItemInfoBySequence(i);
					if (m_itemList.size() > (unsigned int)i && !iInfo.m_isIdentified) {
						g_windowMgr.ErrorMsg(MsgStr(MSI_CANT_SELL_UNIDENTIFIED_ITEM), ME_OK, TRUE, FALSE, 0);
						return 0;
					}

					if (g_serviceType == ServiceGermany ||
						g_serviceType == ServiceJapan ||
						g_serviceType == ServiceIndia) {
						if (m_itemList.size() > (unsigned int)i && (atof(cPrice) <= 0.0 || atof(cPrice) == 0.0))
							isExist0Zeny = 1;
						if (m_itemList.size() > (unsigned int)i && (atof(cPrice) >= 99990001.0 || atof(cPrice) == 99990001.0))
							isExist1000Zeny = 1;
					} else {
						int price;
						if (m_itemList.size() > (unsigned int)i && (atoi(cPrice) <= 0 || (price = atoi(cPrice)) == 0))
							isExist0Zeny = 1;
						if (m_itemList.size() > (unsigned int)i && (atoi(cPrice) >= 99990001 || atoi(cPrice) == 99990001))
							isExist1000Zeny = 1;
					}
				}
				}

				if (isExist0Zeny) {
					if (g_windowMgr.ErrorMsg(MsgStr(MSI_YOU_HAVE_FREE_ITEM_ON_SHOP), ME_YESNO, TRUE, FALSE, 0) != ID_YES)
						break;
				}
				if (isExist1000Zeny) {
					if (g_windowMgr.ErrorMsg(MsgStr(MSI_OVERPRICE9999), ME_YESNO, TRUE, FALSE, 0) != ID_YES)
						break;
				}
				if (!m_shopTitle->GetTextSize()) {
					g_windowMgr.ErrorMsg(MsgStr(MSI_ENTER_SHOP_TITLE), ME_OK, TRUE, FALSE, 0);
					break;
				}

				{
				int i;
				for (i = 0; i < m_numItemCountEdit; ++i) {
					int itemPrice = 0;
					const char* text = m_itemCountEdit[i]->GetText();
					sprintf(cPrice, "%s ", text);
					g_session.DeleteCommaInPrice(cPrice);

					if (m_itemCountEdit[i]->GetTextSize())
						itemPrice = atoi(cPrice);

					if (g_serviceType == ServiceJapan) {
						if (!m_itemCountEdit[i]->GetTextSize() &&
							m_itemCountEdit[i]->GetText()) {
							g_windowMgr.ErrorMsg(MsgStr(MSI_CAN_INPUT_NUMBER_ONLY), ME_OK, TRUE, FALSE, 0);
							return 0;
						}
					}

					g_session.SetMerchantHopeToSellItemPrice(i, itemPrice);
				}
				}

				gameMode->SendMsg(MM_REQ_OPENSTORE, (int)m_shopTitle->GetText(), 1, 0);
				g_windowMgr.DeleteWindow(WID_MERCHANTSHOPMAKEWND);

				char msg[256];
				std::list<ITEM_INFO>::iterator it = m_itemList.begin();

				sprintf(msg, "======================  SALE LIST  ======================");
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0x96ffff, 0, 0);

				for (int i = 0; i < m_itemList.size(); ++i) {
					char name[256];
					char* nameIter = name;
					if (it->m_refiningLevel)
						nameIter += it->GetRefining(nameIter);

					if (ITEM_INFO::IsCardItem(it->m_slot[0])) {
						nameIter += it->GetPrefix(nameIter);
						nameIter += it->GetIdDisplayName(nameIter);
						nameIter += it->GetPostfix(nameIter);
						it->GetSocketNumber(nameIter);
					} else if (it->IsManufacturedItem()) {
						nameIter += it->GetManufacturedItemPrefix(nameIter);
						nameIter += it->GetManufacturedItemProducer(nameIter);
						nameIter += it->GetManufacturedItemPostfix(nameIter);
						it->GetIdDisplayName(nameIter);
					} else {
						nameIter += it->GetIdDisplayName(nameIter);
						it->GetSocketNumber(nameIter);
					}

					const char* text = m_itemCountEdit[i]->GetText();
					sprintf(cPrice, "%s ", text);
					g_session.DeleteCommaInPrice(cPrice);

					if (atoi(cPrice) > 99990000)
						sprintf(priceText, "99,990,000");
					else
						sprintf(priceText, "%s", GetNumberText(atoi(cPrice), moneyTxt));

					sprintf(msg, "    %s  :  %s  Zeny", name, priceText);
					g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0x5affff, 0, 0);

					++it;
				}

				sprintf(priceText, "%s", GetNumberText(g_session.GetGold(), moneyTxt));
				sprintf(msg, "                                          Current Zeny : %s Z", priceText);
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0x78ffff, 0, 0);

				sprintf(msg, "============================ ============================");
				g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, (int)msg, 0x96ffff, 0, 0);
			}
			break;
		case ID_CANCEL:
			{
				CGameMode* gameMode = g_modeMgr.GetGameMode();
				gameMode->SendMsg(MM_REQ_OPENSTORE, (int)m_shopTitle->GetText(), 0, 0);
				g_windowMgr.DeleteWindow(WID_MERCHANTSHOPMAKEWND);
			}
			break;
		}
		break;
	default:
		break;
	}

	return 0;
}

void UIMerchantShopMakeWnd::RecalcScrbarPos()
{
	if (m_scrollBar) {
		int numCanDisplayItem = m_itemVertNum;
		int numLine = (m_numItemCountEdit % m_itemHorzNum != 0) + m_numItemCountEdit / m_itemHorzNum;
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
