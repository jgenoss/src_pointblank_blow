// i3TDKRibbonDockBase.cpp : implementation file
//

#include "stdafx.h"
#include "i3Framework.h"
#include "i3TDKRibbonDockBase.h"


// i3TDKRibbonDockBase

IMPLEMENT_DYNCREATE(i3TDKRibbonDockBase, CDockablePane)


BEGIN_MESSAGE_MAP(i3TDKRibbonDockBase, CDockablePane)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()



// i3TDKRibbonDockBase message handlers



void i3TDKRibbonDockBase::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	CWnd * pChild = GetWindow( GW_CHILD);

	while( pChild != nullptr)
	{
		pChild->SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);

		pChild = pChild->GetNextWindow( GW_HWNDNEXT);
	}
}

void i3TDKRibbonDockBase::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (CMFCPopupMenu::GetActiveMenu() != nullptr)
	{
		return;
	}

	// default Popup 창을 사용하지 않도록 합니다.
}