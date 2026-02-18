#include "stdafx.h"
#include "UIPaneBase.h"

IMPLEMENT_DYNAMIC( CUIPaneBase, CDockablePane)

BEGIN_MESSAGE_MAP( CUIPaneBase, CDockablePane)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CUIPaneBase::OnContextMenu( CWnd * pWnd, CPoint point)
{
	if (CMFCPopupMenu::GetActiveMenu() != NULL)
	{
		return;
	}
}
