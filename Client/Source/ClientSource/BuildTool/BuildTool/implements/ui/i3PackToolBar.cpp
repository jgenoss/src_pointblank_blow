#include "stdafx.h"
#include "i3PackToolBar.h"
#include "BuildTool.h"
#include "UIDefines.h"
#include "i3PackToolTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	i3PackToolBar::i3PackToolBar()
	{}

	i3PackToolBar::~i3PackToolBar()
	{}

	BEGIN_MESSAGE_MAP(i3PackToolBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_CTLCOLOR_REFLECT()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	int i3PackToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_dlg = std::make_unique<i3PackToolDlg>(this);
		m_dlg->Create(IDD_DIALOG_PACKTOOL, this);

		CenterWindow();

		return 0;
	}

	void i3PackToolBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		if (m_dlg.get())
			m_dlg->MoveWindow(0, 0, cx, cy);
	}

	HBRUSH i3PackToolBar::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		UNREFERENCED_PARAMETER(nCtlColor);

		pDC->SetBkColor(COLOR_IVORY_BLACK);

		return m_bkGndBrush;
	}

	BOOL i3PackToolBar::OnEraseBkgnd(CDC* pDC)
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillRect(&rect, &m_bkGndBrush);

		return 1;
	}
}