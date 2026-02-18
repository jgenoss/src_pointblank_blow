#include "stdafx.h"
#include "i3TDKUtilCtrl.h"
#include <stdio.h>
#include "mmc.h"

I3_EXPORT_TDK
HTREEITEM		i3TDK::MoveTreeItem( CTreeCtrl * pCtrl, HTREEITEM hNewParent, HTREEITEM hMoveItem)
{
	HTREEITEM hNewItem;
	TVINSERTSTRUCT ins;
	TCHAR szText[256];

	ins.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT;
	ins.item.hItem = hMoveItem;
	ins.item.pszText = szText;
	ins.item.cchTextMax = sizeof(szText);

	pCtrl->GetItem( &ins.item);

	ins.hParent = hNewParent;
	ins.hInsertAfter = TVI_LAST;

	hNewItem = pCtrl->InsertItem( &ins);

	// New Parent Node의 Children count를 조정
	{
		TVITEM item;

		item.mask = TVIF_CHILDREN | TVIF_HANDLE;
		item.hItem = hNewParent;
		item.cChildren = 1;

		pCtrl->SetItem( &item);
	}

	// Child Node들에 대한...
	{
		HTREEITEM hChild = pCtrl->GetChildItem( hMoveItem);
		HTREEITEM hNext;

		while( hChild != nullptr)
		{
			hNext = pCtrl->GetNextItem( hChild, TVGN_NEXT);

			MoveTreeItem( pCtrl, hNewItem, hChild);

			hChild = hNext;
		}
	}

	pCtrl->DeleteItem( hMoveItem);

	return hNewItem;
}

static HTREEITEM _Rec_FindTreeItemByData( CTreeCtrl * pCtrl, HTREEITEM hParent, void * pData, BOOL bRecurse)
{
	HTREEITEM hItem, hRv;

	hItem = pCtrl->GetChildItem( hParent);
	while( hItem != nullptr)
	{
		if( pCtrl->GetItemData( hItem) == (DWORD_PTR) pData)
		{
			return hItem;
		}

		if( bRecurse == TRUE)
		{
			hRv = _Rec_FindTreeItemByData( pCtrl, hItem, pData, TRUE);
			if( hRv != nullptr)
				return hRv;
		}

		hItem = pCtrl->GetNextItem( hItem, TVGN_NEXT);
	}

	return nullptr;
}

I3_EXPORT_TDK
HTREEITEM i3TDK::FindTreeItemByData( CTreeCtrl * pCtrl, void * pData, HTREEITEM hParent, BOOL bRecurse)
{
	return _Rec_FindTreeItemByData( pCtrl, hParent, pData, bRecurse);
}

static I3_TREECTRL_TRAVERSE_PROC s_pTreeTraverseProc = nullptr;

static BOOL _Rec_TreeCtrlTraverse( CTreeCtrl * pCtrl, HTREEITEM hParent, void * pData)
{
	HTREEITEM hItem, hNext;
	BOOL Rv;

	hItem = pCtrl->GetChildItem( hParent);
	while( hItem != nullptr)
	{
		hNext = pCtrl->GetNextItem( hItem, TVGN_NEXT);

		if( s_pTreeTraverseProc( pCtrl, hItem, pData) == FALSE)
			return FALSE;

		Rv = _Rec_TreeCtrlTraverse( pCtrl, hItem, pData);
		if( Rv == FALSE)
			return FALSE;

		hItem = hNext;
	}

	return TRUE;
}

I3_EXPORT_TDK
void i3TDK::TreeCtrlTraverse( CTreeCtrl * pCtrl, I3_TREECTRL_TRAVERSE_PROC pProc, void * pData, HTREEITEM hRoot)
{
	s_pTreeTraverseProc = pProc;

	if( hRoot != TVI_ROOT)
	{
		if( pProc( pCtrl, hRoot, pData) == FALSE)
			return;
	}

	_Rec_TreeCtrlTraverse( pCtrl, hRoot, pData);
}

I3_EXPORT_TDK
void	i3TDK::DeleteAllChildItem( CTreeCtrl * pCtrl, HTREEITEM hParent)
{
	HTREEITEM hItem;

	while((hItem = pCtrl->GetChildItem( hParent)) != nullptr)
	{
		pCtrl->DeleteItem( hItem);
	}
}

//넘겨진 윈도우 이하의 모든 윈도우들을 bEnable 시킵니다.
static I3_ENABLE_CHILDWND_PROC s_pEnableChildProc = nullptr;

struct I3TDK_ENABLE_WINDOW
{
	HWND hWnd = nullptr;
	BOOL bEnable = FALSE;
};

static BOOL EnableChildWnd( void * pData )
{
	I3TDK_ENABLE_WINDOW * pInfo = ( I3TDK_ENABLE_WINDOW *) pData;

	return ::EnableWindow( pInfo->hWnd, pInfo->bEnable);
}

static BOOL _Rec_EnableChild( void * pData)
{
	I3TDK_ENABLE_WINDOW * pInfo = ( I3TDK_ENABLE_WINDOW *) pData;

	HWND hWnd = GetWindow( pInfo->hWnd, GW_CHILD );

	while( hWnd != nullptr )
	{
		if( s_pEnableChildProc( pData ) != pInfo->bEnable )
			return !pInfo->bEnable;

		I3TDK_ENABLE_WINDOW Data;
		Data.hWnd = hWnd;
		Data.bEnable = pInfo->bEnable;

		_Rec_EnableChild( &Data );

		hWnd = GetWindow( pInfo->hWnd, GW_HWNDNEXT );
	}

	return pInfo->bEnable;
}

static void _EnableChildCtrl( I3_ENABLE_CHILDWND_PROC pProc, void * pData)
{
	s_pEnableChildProc = pProc;

	I3TDK_ENABLE_WINDOW * pInfo = ( I3TDK_ENABLE_WINDOW *) pData;

	if( pProc( pData ) != pInfo->bEnable )	return;

	_Rec_EnableChild( pData );
}


I3_EXPORT_TDK
void i3TDK::EnableChildCtrl( HWND hWnd, BOOL bEnable )
{
	I3TDK_ENABLE_WINDOW Data;
	Data.hWnd = hWnd;
	Data.bEnable = bEnable;

	_EnableChildCtrl( EnableChildWnd, &Data );
}

I3_EXPORT_TDK
INT32 i3TDK::FindItemByData( CComboBox * pCtrl, DWORD_PTR data)
{
	INT32 i;

	for( i = 0; i < pCtrl->GetCount(); i++)
	{
		if( pCtrl->GetItemData(i) == data)
			return i;
	}

	return -1;
}

I3_EXPORT_TDK
void	i3TDK::SetCursor( HWND hwnd, HCURSOR hCursor)
{
	::SetCursor( hCursor);

	::SetClassLong( hwnd, GCL_HCURSOR, (LONG) hCursor);
}


namespace i3TDK
{


I3_EXPORT_TDK
void BatchAlignCtrls(DWORD alignFlag/*EBatchAlignCtlFlag*/, INT32 paddX, INT32 paddY, CWnd* parent, ...)
{
	if(!parent)
		return;

	RECT rcParent;
	HWND parentWnd = parent->GetSafeHwnd();

	if(!parentWnd || !IsWindow(parentWnd))
		return;

	GetWindowRect(parentWnd, &rcParent);

	INT32 parW = rcParent.right - rcParent.left;
	INT32 parH = rcParent.bottom - rcParent.top;

	va_list arg;
	va_start(arg, parent); // starting from parameter 'parent'

	for(;;)
	{
		CWnd* wnd = va_arg(arg, CWnd*);

		if(!wnd)
			break;

		HWND curWnd = wnd->GetSafeHwnd();
		
		if(!IsWindow(curWnd))
			continue;

		RECT rcChildRect;
		GetWindowRect(curWnd, &rcChildRect);

		INT childW = rcChildRect.right - rcChildRect.left;
		INT childH = rcChildRect.bottom - rcChildRect.top;

		POINT leftTop;
		leftTop.x = rcChildRect.left; leftTop.y = rcChildRect.top;
		ScreenToClient(parentWnd, &leftTop);

		if(alignFlag & i3TDK::EBATCH_ALGNCTL_RIGHT)
		{
			childW = parW - (rcChildRect.left - rcParent.left) - paddX;

			// MoveWindow is relative moving with parent
			MoveWindow(curWnd, leftTop.x, leftTop.y, childW, childH, FALSE);
		}

		if(alignFlag & i3TDK::EBATCH_ALGNCTL_BOTTOM)
		{
			childH = parH - rcChildRect.top;
			MoveWindow(curWnd, leftTop.x, leftTop.y, childW, childH, FALSE);
		}
	}

	InvalidateRect(parentWnd, nullptr, TRUE);

	va_end(arg);
}


};