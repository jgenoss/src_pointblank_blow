#include "StdAfx.h"
#include "Resource.h"
#include "i3RSGToolbar.h"

IMPLEMENT_DYNAMIC(i3RSGToolbar, CToolBar)

BEGIN_MESSAGE_MAP(i3RSGToolbar, CToolBar)
	ON_WM_SIZE()

END_MESSAGE_MAP()


i3RSGToolbar::i3RSGToolbar()
{
}


i3RSGToolbar::~i3RSGToolbar()
{
}
BOOL i3RSGToolbar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	BOOL rst = CToolBar::Create(pParentWnd, dwStyle, nID);
	return rst;
}

BOOL i3RSGToolbar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle,
	DWORD dwStyle, CRect rcBorders, UINT nID)
{
	BOOL rst = CToolBar::CreateEx(pParentWnd, dwCtrlStyle, dwStyle, rcBorders, nID);
	return rst;
}

BOOL i3RSGToolbar::CreateAdditional(INT nID, INT comboWidth)
{
	m_RSCBtnWidth = comboWidth;
	m_RSCBtnRect = CRect(0, 0, m_RSCBtnWidth, 20);
	m_RSCBtn.Create("RSC Save", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_TEXT,
		m_RSCBtnRect, this, nID);

	return TRUE;
}

void i3RSGToolbar::OnSize(UINT nType, int cx, int cy)
{
	CToolBar::OnSize(nType, cx, cy);

	if (::IsWindow(m_RSCBtn.GetSafeHwnd()))
	{
		GetItemRect(0, &m_RSCBtnRect);

		SetButtonInfo(0, ID_RSCSAVE, TBBS_SEPARATOR, m_RSCBtnWidth);

		m_RSCBtn.SetWindowPos(NULL, m_RSCBtnRect.left+4,
			m_RSCBtnRect.top, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS);
	}
} 

bool i3RSGToolbar::IsRSCSave()
{
	return m_RSCBtn.GetCheck() ? true : false;
}