#include "stdafx.h"
#include "PaneBase.h"

IMPLEMENT_DYNAMIC(CPaneBase, CDockablePane)

BEGIN_MESSAGE_MAP(CPaneBase, CDockablePane)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

CPaneBase::CPaneBase()
{
}


CPaneBase::~CPaneBase()
{
}

void CPaneBase::OnContextMenu(CWnd * pWnd, CPoint point)
{
	if (CMFCPopupMenu::GetActiveMenu() != NULL)
	{
		return;
	}
}