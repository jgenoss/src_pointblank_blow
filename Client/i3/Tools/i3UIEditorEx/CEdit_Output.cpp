#include "stdafx.h"
#include "CEdit_Output.h"
#include "resource.h"

IMPLEMENT_DYNAMIC( COutputEdit, CEdit)

BEGIN_MESSAGE_MAP( COutputEdit, CEdit)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void COutputEdit::OnContextMenu( CWnd* pWnd, CPoint pos)
{
	CMenu Menu;
	CMenu* pPopup = NULL;
	UINT32 cmd = 0;

	Menu.LoadMenu( IDR_MENU_OUTPUT);
	pPopup = Menu.GetSubMenu( 0);

	if( pPopup != NULL)
	{
		cmd = pPopup ->TrackPopupMenuEx(  TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
					pos.x, pos.y, (CWnd *) this, NULL);
	}

	switch( cmd)
	{
	case ID__CLEARALL: _ClearAll(); break;
	default: break;
	}
}

void COutputEdit::_ClearAll( void)
{
	SetSel(0, -1); 
	//Clear();	// => ES_READONLY Flag가 설정되어 있으면 안먹힘..
	ReplaceSel((LPCTSTR)"");
}

