
#include "stdafx.h"

#ifdef _USE_TOOLAM_

#include "resource.h"
#include "AIStateEditor_Ctl.h"

namespace AIStateEdit
{
//
// AIStateEditListCtrl
//

IMPLEMENT_DYNAMIC(AIStateEditListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(AIStateEditListCtrl, CListCtrl)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(LVM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(LVM_SETTEXTCOLOR, OnSetTextColor)
	ON_MESSAGE(LVM_SETTEXTBKCOLOR, OnSetTextBkColor)
	ON_MESSAGE(LVM_SETBKCOLOR, OnSetBkColor)
END_MESSAGE_MAP()


AIStateEditListCtrl::AIStateEditListCtrl()
{
	m_fullRowSel = true;

	m_doClientWidthSel = true;
	m_clientWidth = 0;
	m_stateImgOffW = 0;

	m_clrText	= ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk = ::GetSysColor(COLOR_WINDOW);
	m_clrBk		= ::GetSysColor(COLOR_WINDOW);

	GenerateHeaderColumnData();
}

AIStateEditListCtrl::~AIStateEditListCtrl()
{
	ClearView();

	for(INT32 i=0; i<(INT32)m_columns.size(); i++)
		delete m_columns[i];
}

BOOL AIStateEditListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// default is report view and full row selection
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT | LVS_OWNERDRAWFIXED;

	return CListCtrl::PreCreateWindow(cs);
}

bool AIStateEditListCtrl::SetFullRowSel(bool fullRowSel)
{
	// no painting during change
	LockWindowUpdate();

	m_fullRowSel = fullRowSel;
	bool ret;

	if(m_fullRowSel)
		ret = ModifyStyle(0L, LVS_OWNERDRAWFIXED) ? true : false;
	else
		ret = ModifyStyle(LVS_OWNERDRAWFIXED, 0L) ? true : false;

	// repaint window if we are not changing view type
	if (ret && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		Invalidate();

	// repaint changes
	UnlockWindowUpdate();

	return ret;
}

void AIStateEditListCtrl::RefreshAIState()
{
	ClearView();
	GenerateRow();
}

void AIStateEditListCtrl::ClearView()
{
	if(!GetSafeHwnd())
		return;

	DeleteAllItems();
}

void AIStateEditListCtrl::GenerateHeaderColumnData()
{
	if(!m_columns.empty())
		return;

	char* labels[] = 
	{ 
		"No",
		"ActionName", 
		"DriverMeta", 
		"AIState Name", 
		"i3a Name", 
		"Chara",
		"HeroName",
		"1pv/3pv",
		"WeaponPath",
		"i3aName"
	};

	const INT32 labelWidth[] = 
	{ 
		30,
		110, 
		110, 
		150, // aistate
		220,
		80, // chara
		100, // hero
		100, // 1pv/3pv
		220, // weaponpath
		200	 // i3aName
	};

	const static INT32 numLabel = sizeof(labels) / sizeof(labels[0]);

	for(INT32 i=0; i<numLabel; i++)
	{
		LVCOLUMN* ins = new LVCOLUMN;
		memset(ins, 0, sizeof(LVCOLUMN));

		ins->mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		ins->fmt = LVCFMT_LEFT;
		ins->cx = labelWidth[i];
		ins->pszText = (LPSTR)labels[i];
		ins->cchTextMax = strlen(labels[i]);
		ins->iSubItem = i;

		m_columns.push_back(ins);
	}	
}

void AIStateEditListCtrl::GenerateRow()
{
	DeleteAllItems();

	INT32 numColumn = (INT32)m_columns.size();

	// fill header column
	for(INT32 i=0; i<numColumn; i++)
		InsertColumn(i, m_columns[i]);

	INT32 rowIndex = 0;
	INT32 numItem = 1;

	// fill data
	for(INT32 i=0; i<numItem; i++)
	{
		LV_ITEM lvItem;

		lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
		lvItem.iItem = rowIndex;
		lvItem.iSubItem = 0;
		lvItem.pszText = "AAA";
		lvItem.iImage = 0;
		lvItem.stateMask = LVIS_STATEIMAGEMASK;
		lvItem.state = INDEXTOSTATEIMAGEMASK(1);

		InsertItem(&lvItem);
	}

	// set item text
}

#define OFFSET_FIRST    2
#define OFFSET_OTHER    6

void AIStateEditListCtrl::DrawItem(LPDRAWITEMSTRUCT drItemInfo)
{
	COLORREF clrTextSave = 0;
	COLORREF clrBkSave = 0;
	COLORREF clrImage = m_clrBk;
	
	bool doClientWidthSel = m_doClientWidthSel;
	int clientWidth = m_clientWidth;
	COLORREF clrBk = m_clrBk;
	COLORREF clrTextBk = m_clrTextBk;
	int offsetStateImageW = m_stateImgOffW;

	CListCtrl& lc = *((CListCtrl*)this);
	CDC* pDC = CDC::FromHandle(drItemInfo->hDC);
	CRect rcItem(drItemInfo->rcItem);

	UINT uiFlags = ILD_TRANSPARENT;
	CImageList* pImageList;
	int nItem = drItemInfo->itemID;

	BOOL bFocus = (GetFocus() == this);
	static _TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

	// get item data
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = 0xFFFF;     // get all state flags
	lc.GetItem(&lvi);

	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS)) && lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

	// set colors if item is selected
	CRect rcAllLabels;
	lc.GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	CRect rcLabel;
	lc.GetItemRect(nItem, rcLabel, LVIR_LABEL);

	rcAllLabels.left = rcLabel.left;
	if (doClientWidthSel && rcAllLabels.right<clientWidth)
		rcAllLabels.right = clientWidth;

	if (bSelected)
	{
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));

		CBrush cbr(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->FillRect(rcAllLabels, &cbr);
	}
	else
	{
		CBrush cbr(clrTextBk);
		pDC->FillRect(rcAllLabels, &cbr);
	}

	// set color and mask for the icon

	if (lvi.state & LVIS_CUT)
	{
		clrImage = clrBk;
		uiFlags |= ILD_BLEND50;
	}
	else if (bSelected)
	{
		clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags |= ILD_BLEND50;
	}

	// draw state icon
	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if (nStateImageMask)
	{
		int nImage = (nStateImageMask >> 12) - 1;
		pImageList = lc.GetImageList(LVSIL_STATE);
		if (pImageList)
		{
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top), ILD_TRANSPARENT);
		}
	}

	// draw normal and overlay icon
	CRect rcIcon;
	lc.GetItemRect(nItem, rcIcon, LVIR_ICON);

	pImageList = lc.GetImageList(LVSIL_SMALL);
	if(pImageList)
	{
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		if(rcItem.left<rcItem.right-1)
		{
			ImageList_DrawEx(pImageList->m_hImageList, lvi.iImage,
				pDC->m_hDC,rcIcon.left, rcIcon.top, 16, 16,
				clrBk, clrImage, uiFlags | nOvlImageMask);
		}
	}

	// draw item label
	lc.GetItemRect(nItem, rcItem, LVIR_LABEL);
	rcItem.right -= offsetStateImageW;

	pszText = MakeShortString(pDC, szBuff, rcItem.right-rcItem.left, 2*OFFSET_FIRST);
	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	pDC->DrawText(pszText,-1,rcLabel,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);

	// draw labels for extra columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	for(int nColumn = 1; lc.GetColumn(nColumn, &lvc); nColumn++)
	{
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		int nRetLen = lc.GetItemText(nItem, nColumn,
			szBuff, sizeof(szBuff));
		if (nRetLen == 0)
			continue;

		pszText = MakeShortString(pDC, szBuff,
			rcItem.right - rcItem.left, 2*OFFSET_OTHER);

		UINT nJustify = DT_LEFT;

		if(pszText == szBuff)
		{
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
			{
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default:
				break;
			}
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText, -1, rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER);
	}

	// draw focus rectangle if item has focus
	if (lvi.state & LVIS_FOCUSED && bFocus)
		pDC->DrawFocusRect(rcAllLabels);

	// set original colors if item was selected
	if (bSelected)
	{
		pDC->SetTextColor(clrTextSave);
		pDC->SetBkColor(clrBkSave);
	}
}

const char* AIStateEditListCtrl::MakeShortString(CDC* dc, const char* srcStr, int lenColumn, int off)
{
	static const char threeDots[] = "...";
	int lenStr = strlen(srcStr);

	if(lenStr == 0 || (dc->GetTextExtent(srcStr, lenStr).cx + off) <= lenColumn)
		return srcStr;

	static char shortStr[MAX_PATH];
	strcpy_s(shortStr, MAX_PATH, srcStr);
	int lenAdd = dc->GetTextExtent(threeDots, sizeof(threeDots)).cx;

	for(int i=lenStr-1; i>0; i--)
	{
		shortStr[i] = 0;

		if((dc->GetTextExtent(shortStr, i).cx + off + lenAdd) <= lenColumn)
			break;
	}

	strcat_s(shortStr, MAX_PATH, threeDots);

	return shortStr;
}

void AIStateEditListCtrl::SetImgListState(UINT bitmapID, INT bitmapW, INT growFactor, 
	COLORREF clrMask)
{
	m_stateImgLst.DeleteImageList();
	m_stateImgLst.Create(bitmapID, bitmapW, growFactor, clrMask);

	SetImageList(&m_stateImgLst, LVSIL_STATE);
}

void AIStateEditListCtrl::RepaintSelectedItems()
{
	CListCtrl& lc = *((CListCtrl*)this);
	CRect rcItem, rcLabel;

	// invalidate focused item so it can repaint properly
	int nItem = lc.GetNextItem(-1, LVNI_FOCUSED);

	if(nItem != -1)
	{
		lc.GetItemRect(nItem, rcItem, LVIR_BOUNDS);
		lc.GetItemRect(nItem, rcLabel, LVIR_LABEL);
		rcItem.left = rcLabel.left;
		InvalidateRect(rcItem, FALSE);
	}

	// if selected items should not be preserved, invalidate them
	if(!(GetStyle() & LVS_SHOWSELALWAYS))
	{
		for(nItem = lc.GetNextItem(-1, LVNI_SELECTED);
			nItem != -1; nItem = lc.GetNextItem(nItem, LVNI_SELECTED))
		{
			lc.GetItemRect(nItem, rcItem, LVIR_BOUNDS);
			lc.GetItemRect(nItem, rcLabel, LVIR_LABEL);
			rcItem.left = rcLabel.left;

			InvalidateRect(rcItem, FALSE);
		}
	}

	// update changes
	UpdateWindow();
}


//
// Handle AfxMessage
//

LRESULT AIStateEditListCtrl::OnSetImageList(WPARAM wParam, LPARAM lParam)
{
	// if we're running Windows 4, there's no need to offset the item text location
	OSVERSIONINFO info;
	info.dwOSVersionInfoSize = sizeof(info);
	VERIFY(::GetVersionEx(&info));

	if((int)wParam == LVSIL_STATE && info.dwMajorVersion < 4)
	{
		int cx, cy;
		if(::ImageList_GetIconSize((HIMAGELIST)lParam, &cx, &cy))
			m_stateImgOffW = cx;
		else
			m_stateImgOffW = 0;
	}

	return Default();
}

LRESULT AIStateEditListCtrl::OnSetTextColor(WPARAM /*wParam*/, LPARAM lParam)
{
	m_clrText = (COLORREF)lParam;
	return Default();
}

LRESULT AIStateEditListCtrl::OnSetTextBkColor(WPARAM /*wParam*/, LPARAM lParam)
{
	m_clrTextBk = (COLORREF)lParam;
	return Default();
}

LRESULT AIStateEditListCtrl::OnSetBkColor(WPARAM /*wParam*/, LPARAM lParam)
{
	m_clrBk = (COLORREF)lParam;
	return Default();
}

void AIStateEditListCtrl::OnSize(UINT nType, int cx, int cy)
{
	m_clientWidth = cx;
	CListCtrl::OnSize(nType, cx, cy);
}

void AIStateEditListCtrl::OnPaint()
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	if(m_doClientWidthSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT && m_fullRowSel)
	{
		CRect rcAllLabels;
		GetItemRect(0, rcAllLabels, LVIR_BOUNDS);

		if(rcAllLabels.right < m_clientWidth)
		{
			// need to call BeginPaint (in CPaintDC c-tor)
			// to get correct clipping rect
			CPaintDC dc(this);

			CRect rcClip;
			dc.GetClipBox(rcClip);

			rcClip.left = min(rcAllLabels.right-1, rcClip.left);
			rcClip.right = m_clientWidth;

			InvalidateRect(rcClip, FALSE);
			// EndPaint will be called in CPaintDC d-tor
		}
	}

	CListCtrl::OnPaint();
}

void AIStateEditListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);

	// check if we are getting focus from label edit box
	if(pOldWnd!=NULL && pOldWnd->GetParent()==this)
		return;

	// repaint items that should change appearance
	if(m_fullRowSel && (GetStyle() & LVS_TYPEMASK)==LVS_REPORT)
		RepaintSelectedItems();
}

void AIStateEditListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);

	// check if we are losing focus to label edit box
	if(pNewWnd != NULL && pNewWnd->GetParent() == this)
		return;

	// repaint items that should change appearance
	if(m_fullRowSel && (GetStyle() & LVS_TYPEMASK) == LVS_REPORT)
		RepaintSelectedItems();
}


//
////
//// AIStateListViewFrame
////
//
//IMPLEMENT_DYNAMIC(AIStateListViewFrame, i3TDKDockFrameWnd)
//
//BEGIN_MESSAGE_MAP(AIStateListViewFrame, i3TDKDockFrameWnd)
//	ON_WM_CREATE()
//	ON_WM_SETFOCUS()
//	ON_WM_DESTROY()
//	ON_WM_GETMINMAXINFO()
//END_MESSAGE_MAP()
//
//
//AIStateListViewFrame::AIStateListViewFrame()
//{
//	m_frameW = 100;
//	m_frameH = 100;
//}
//
//AIStateListViewFrame::~AIStateListViewFrame()
//{
//
//}
//
//int AIStateListViewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (i3TDKDockFrameWnd::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	CRect rcListView;
//	GetClientRect(&rcListView);
//
//	m_listview.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rcListView, this, IDC_AIED_LV_AISTATE, 
//		(CCreateContext*)lpCreateStruct->lpCreateParams);
//
//	m_listview.GenerateHeaderColumnData();
//
//	return 0;
//}
//
//void AIStateListViewFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
//{
//	i3TDKDockFrameWnd::OnGetMinMaxInfo(lpMMI);
//
//	lpMMI->ptMinTrackSize.x = m_frameW; // some constant width you want
//	lpMMI->ptMinTrackSize.y = m_frameH; // some constant height you want.
//
//	lpMMI->ptMaxTrackSize.x = m_frameW;
//	lpMMI->ptMaxTrackSize.y = m_frameH;
//}
//
//BOOL AIStateListViewFrame::PreCreateWindow(CREATESTRUCT& cs)
//{
//	if( !i3TDKDockFrameWnd::PreCreateWindow(cs) )
//		return FALSE;
//
//	cs.style &= ~(WS_THICKFRAME | WS_BORDER);
//	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
//	cs.lpszClass = AfxRegisterWndClass(0);
//	return TRUE;
//}
//
//void AIStateListViewFrame::OnDestroy()
//{
//	i3TDKDockFrameWnd::OnDestroy();
//}

//LRESULT AIStateListViewFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch( message)
//	{
//	case WM_EXITSIZEMOVE :
//		break;
//
//	}
//
//	return i3TDKDockFrameWnd::WindowProc(message, wParam, lParam);
//}

//LRESULT AIStateListViewFrame::_RouteWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	return i3TDKDockFrameWnd::WindowProc(message, wParam, lParam);
//}


} // end of namespace AIStateEdit

#endif // _USE_TOOLAM_