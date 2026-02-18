#include "stdafx.h"
#include "i3EXPLogDialog.h"
#include "resource.h"

static	HWND	s_hwndList;

static void _OnInitDialog( HWND hwnd)
{
	HWND hwndCtrl = GetDlgItem( hwnd, IDC_LIST_LOG);

	s_hwndList = hwndCtrl;

	// Header
	{
		LVCOLUMN	data;

		// Level
		data.mask = LVCF_TEXT | LVCF_WIDTH;
		data.cx = 80;
		data.pszText = "Node";
		SendMessage( hwndCtrl, LVM_INSERTCOLUMN,	0,	(LPARAM) &data);

		// Message
		data.cx = 700;
		data.pszText = "Message";
		SendMessage( hwndCtrl, LVM_INSERTCOLUMN,	1,	(LPARAM) &data);
	}

	// Extended Style
	SendMessage( hwndCtrl, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, 0xFFFFFFFF);
}

static void _OnLog( INode * pNode, char * pszMsg)
{
	LVITEM item;

	// Node Name
	item.mask = LVIF_PARAM | LVIF_TEXT;
	item.iItem = SendMessage( s_hwndList, LVM_GETITEMCOUNT, 0, 0);
	item.iSubItem = 0;

	if( pNode != NULL)
		item.pszText = pNode->GetName();
	else
		item.pszText = "(Global)";

	item.lParam = (LPARAM) pNode;

	SendMessage( s_hwndList, LVM_INSERTITEM, 0, (LPARAM) &item);

	// Message
	item.mask = LVIF_TEXT;
	item.iSubItem = 1;
	item.pszText = pszMsg;

	SendMessage( s_hwndList, LVM_SETITEMTEXT, item.iItem, (LPARAM) &item);
}

static void	_OnClearLog(void)
{
	SendMessage( s_hwndList, LVM_DELETEALLITEMS, 0, 0);
}

static void _SelectNode( INT32 idx)
{
	LVITEM item;

	item.mask = LVIF_PARAM;
	item.iItem = idx;
	item.iSubItem = 0;
	if( SendMessage( s_hwndList, LVM_GETITEM, 0, (LPARAM) &item) == FALSE)
		return;

	if( item.lParam != NULL)
	{
		Interface * ip = GetCOREInterface();

		theHold.Begin();
		ip->SelectNode( (INode *) item.lParam, 1);
		ip->ViewportZoomExtents( FALSE, FALSE);
		ip->RedrawViews( ip->GetTime());
		TSTR undostr; undostr.printf("Select");
		theHold.Accept(undostr);
	}
}

static void _OnLogListNotify( HWND hwndDialog, NMHDR * pHdr)
{
	switch( pHdr->code)
	{
		case NM_DBLCLK :
			{
				NMITEMACTIVATE * pItemNM = (NMITEMACTIVATE *) pHdr;

				if( pItemNM->iItem != -1)
				{
					_SelectNode( pItemNM->iItem);
				}
			}
			break;
	}
}

static void _ResizeLogList( HWND hwnd, int cx, int cy)
{
	HWND hwndCtrl;

	hwndCtrl = ::GetDlgItem( hwnd, IDC_LIST_LOG);
	
	::SetWindowPos( hwndCtrl, NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CALLBACK  _LogDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT id, code;
	
	switch( msg) 
	{
		case WM_INITDIALOG:
			_OnInitDialog( hwnd);
			break;

		case WM_COMMAND:
			id = LOWORD(wParam);
			code = HIWORD( wParam);

			switch( id) 
			{
				case IDCANCEL :
					//EndDialog( hwnd, id);
					::ShowWindow( hwnd, SW_HIDE);
					break;
			}
			break;

		case WM_SIZE :
			_ResizeLogList( hwnd, LOWORD( lParam), HIWORD( lParam));
			break;

		case WM_EXP_LOG :		
			_OnLog( (INode *) wParam, (char *) lParam);	
			ShowWindow( hwnd, SW_SHOW);
			break;

		case WM_EXP_CLEAR :		_OnClearLog();	break;

		case WM_NOTIFY :
			{
				switch( wParam)		// Ctrl ID
				{
					case IDC_LIST_LOG :
						_OnLogListNotify( hwnd, (NMHDR *) lParam);
						break;
				}
			}
			break;

		default:
			return false;
	}

	return true;
}
