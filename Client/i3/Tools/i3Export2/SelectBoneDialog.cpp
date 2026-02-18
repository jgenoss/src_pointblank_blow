#include "stdafx.h"
#include "resource.h"
#include "i3Export.h"
#include "Util.h"

typedef BOOL	(*_TREECTRL_TRAVERSE_PROC)( HWND, HTREEITEM hItem, void * pData);

static i3::vector<i3::rc_string>* s_pExcludeList;
static i3Export * s_pExport;

static void _Rec_SetBone( HWND hwnd, INode * pNode, HTREEITEM hParent)
{
	TVINSERTSTRUCT data;
	HTREEITEM hItem;
	INT32 i;
	INode * pChild;

	// pNode 자체에 대한 처리
	if( pNode->GetTMController() != NULL)
	{
		// Bone이다.
		data.hParent = hParent;
		data.hInsertAfter = TVI_LAST;
		data.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT | TVIF_STATE;
		data.item.pszText = pNode->GetName();
		data.item.cChildren = pNode->NumberOfChildren();
		data.item.lParam = (LPARAM) pNode;
		data.item.state = TVIS_EXPANDED;
		data.item.stateMask = TVIS_EXPANDED;

		hItem = (HTREEITEM) SendMessage( hwnd, TVM_INSERTITEM, 0, (LPARAM) & data);

		if (i3::vu::int_index_of(*s_pExcludeList, pNode->GetName()) == -1 )
		{
			TreeView_SetCheckState( hwnd, hItem, TRUE);
		}
		else
		{
			TreeView_SetCheckState( hwnd, hItem, FALSE);
		}
	}
	else
	{
		// Bone이 아니기 때문에...
		// Parent Node를 다시 전달한다.
		hItem = hParent;
	}

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		pChild = pNode->GetChildNode( i);

		_Rec_SetBone( hwnd, pChild, hItem);
	}
}

static void _SetBones( HWND hwndCtrl)
{
	Interface * ip = GetCOREInterface();
	INode * pRoot;

	pRoot = ip->GetRootNode();

	if( pRoot != NULL)
	{
		_Rec_SetBone( hwndCtrl, pRoot, TVI_ROOT);
	}
}

static void _ReplaceControls( HWND hwnd, int cx, int cy)
{
	RECT rc, rcBtn;
	HWND hwndCtrl;
	POINT pt;

	{
		i3tool::exporter::pack::OPTION_INFO * pOpt = g_pExport->getOption();

		::GetWindowRect( hwnd, &rc);

		pOpt->m_SelectBoneWndX = rc.left;
		pOpt->m_SelectBoneWndY = rc.top;
		pOpt->m_SelectBoneWndCX = rc.right - rc.left;
		pOpt->m_SelectBoneWndCY = rc.bottom - rc.top;
	}

	// Button들을 오른쪽으로 밀어붙인다.
	{
		INT32 y = 3;

		// OK
		hwndCtrl = ::GetDlgItem( hwnd, IDOK);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// Cancel
		hwndCtrl = ::GetDlgItem( hwnd, IDCANCEL);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// Select All
		hwndCtrl = ::GetDlgItem( hwnd, ID_SELALL);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// Unselect All
		hwndCtrl = ::GetDlgItem( hwnd, ID_UNSELALL);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// Inverse
		hwndCtrl = ::GetDlgItem( hwnd, ID_REVSEL);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// Update
		hwndCtrl = ::GetDlgItem( hwnd, ID_UPDATE);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// All Biped
		hwndCtrl = ::GetDlgItem( hwnd, IDC_BTN_ALLBIPED);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// Upper
		hwndCtrl = ::GetDlgItem( hwnd, IDC_BTN_UPPER);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
		y += (rcBtn.bottom - rcBtn.top) + 4;

		// Lower
		hwndCtrl = ::GetDlgItem( hwnd, IDC_BTN_LOWER);
		::GetWindowRect( hwndCtrl, &rcBtn);
		::SetWindowPos( hwndCtrl, NULL, cx - (rcBtn.right - rcBtn.left) - 3, y, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	}

	hwndCtrl = ::GetDlgItem( hwnd, IDOK);
	::GetWindowRect( hwndCtrl, &rc);
	pt.x = rc.left;
	pt.y = rc.top;

	::ScreenToClient( hwnd, &pt);

	hwndCtrl = ::GetDlgItem( hwnd, IDC_BONETREE);
	::SetWindowPos( hwndCtrl, NULL, 3, 3, pt.x - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static _TREECTRL_TRAVERSE_PROC s_pTreeTraverseProc = NULL;

static BOOL _Rec_TreeCtrlTraverse( HWND hwnd, HTREEITEM hParent, void * pData)
{
	HTREEITEM hItem;
	BOOL Rv;

	hItem = TreeView_GetChild( hwnd, hParent);

	while( hItem != NULL)
	{
		if( s_pTreeTraverseProc( hwnd, hItem, pData) == FALSE)
			return FALSE;

		Rv = _Rec_TreeCtrlTraverse( hwnd, hItem, pData);
		if( Rv == FALSE)
			return FALSE;

		hItem = TreeView_GetNextItem( hwnd, hItem, TVGN_NEXT);
	}

	return TRUE;
}

static void _TreeCtrlTraverse( HWND hwnd, _TREECTRL_TRAVERSE_PROC pProc, void * pData, HTREEITEM hRoot)
{
	hwnd = ::GetDlgItem( hwnd, IDC_BONETREE);

	s_pTreeTraverseProc = pProc;

	if( hRoot != TVI_ROOT)
	{
		if( pProc( hwnd, hRoot, pData) == FALSE)
			return;
	}

	_Rec_TreeCtrlTraverse( hwnd, hRoot, pData);
}

static INode *	TreeView_GetItemData( HWND hwnd, HTREEITEM hItem)
{
	TVITEM item;

	memset( &item, 0, sizeof(item));
	item.mask = TVIF_HANDLE | TVIF_PARAM;
	item.hItem = hItem;

	TreeView_GetItem( hwnd, &item);

	return (INode *) item.lParam;
}

static void	TreeView_GetItemText( HWND hwnd, HTREEITEM hItem, char * pszStr, int length)
{
	TVITEM item;

	memset( &item, 0, sizeof(item));
	item.mask = TVIF_HANDLE | TVIF_TEXT;
	item.cchTextMax = length;
	item.pszText = pszStr;
	item.hItem = hItem;

	TreeView_GetItem( hwnd, &item);
}

static bool		TreeView_IsDescendentOf( HWND hwnd, HTREEITEM hItem, const char * pszParentName)
{
	char szName[ 256];

	while( hItem != NULL)
	{
		TreeView_GetItemText( hwnd, hItem, szName, _countof( szName));

		if( stricmp( szName, pszParentName) == 0)
			return true;

		hItem = TreeView_GetParent( hwnd, hItem);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static BOOL _UpdateSelectProc( HWND hwnd, HTREEITEM hItem, void * pData)
{
	TVITEM item;

	item.mask = TVIF_HANDLE | TVIF_PARAM;
	item.hItem = hItem;

	TreeView_GetItem( hwnd, &item);

	INode * pNode = (INode *) item.lParam;

	if (i3::vu::int_index_of(*s_pExcludeList, pNode->GetName() ) == -1 )
	{
		TreeView_SetCheckState( hwnd, hItem, TRUE);
	}
	else
	{
		TreeView_SetCheckState( hwnd, hItem, FALSE);
	}

	return TRUE;
}

static BOOL	_SelectAllProc( HWND hwnd, HTREEITEM hItem, void * pData)
{
	TreeView_SetCheckState( hwnd, hItem, TRUE);

	return TRUE;
}

static BOOL	_UnselectAllProc( HWND hwnd, HTREEITEM hItem, void * pData)
{
	TreeView_SetCheckState( hwnd, hItem, FALSE);

	return TRUE;
}

static BOOL	_InverseProc( HWND hwnd, HTREEITEM hItem, void * pData)
{
	if( TreeView_GetCheckState( hwnd, hItem) == 1)
	{
		TreeView_SetCheckState( hwnd, hItem, FALSE);
	}
	else
	{
		TreeView_SetCheckState( hwnd, hItem, TRUE);
	}

	return TRUE;
}


static BOOL	_SelectAllBiped( HWND hwnd, HTREEITEM hItem, void * pData)
{
	INode * pNode = TreeView_GetItemData( hwnd, hItem);

	if( (pNode != NULL) && (isValidBone( pNode, false) == BONE_TYPE_SKELETON))
	{
		TreeView_SetCheckState( hwnd, hItem, TRUE);
	}
	else
	{
		TreeView_SetCheckState( hwnd, hItem, FALSE);
	}

	return TRUE;
}

static BOOL	_SelectUpper( HWND hwnd, HTREEITEM hItem, void * pData)
{
	INode * pNode = TreeView_GetItemData( hwnd, hItem);
	bool bValidBone = isValidBone( pNode, false) == BONE_TYPE_SKELETON;

	if( bValidBone && TreeView_IsDescendentOf( hwnd, hItem, "IK"))
	{
		TreeView_SetCheckState( hwnd, hItem, TRUE);
	}
	else
	{
		TreeView_SetCheckState( hwnd, hItem, FALSE);
	}

	return TRUE;
}

static BOOL	_SelectLower( HWND hwnd, HTREEITEM hItem, void * pData)
{
	INode * pNode = TreeView_GetItemData( hwnd, hItem);
	bool bValidBone = isValidBone( pNode, false) == BONE_TYPE_SKELETON;

	if( bValidBone && !TreeView_IsDescendentOf( hwnd, hItem, "IK"))
	{
		TreeView_SetCheckState( hwnd, hItem, TRUE);
	}
	else
	{
		TreeView_SetCheckState( hwnd, hItem, FALSE);
	}

	return TRUE;
}

static BOOL _ExpandAllProc( HWND hwnd, HTREEITEM hItem, void * pData)
{
	TreeView_Expand( hwnd, hItem, TVM_EXPAND);

	return TRUE;
}

static BOOL	_UpdateExcludeProc( HWND hwnd, HTREEITEM hItem, void * pData)
{
	if( TreeView_GetCheckState( hwnd, hItem) != 1)
	{
		TVITEM item;

		item.mask = TVIF_HANDLE | TVIF_PARAM;
		item.hItem = hItem;

		TreeView_GetItem( hwnd, &item);

		INode * pNode = (INode *) item.lParam;

		s_pExcludeList->push_back( pNode->GetName());
	}

	return TRUE;
}

static void OnBoneTreeRClick( HWND hwnd, NMHDR * pHdr, UINT32 * pResult)
{
	POINT pt;
	HWND hwndCtrl = GetDlgItem( hwnd, IDC_BONETREE);
	HTREEITEM hItem;
	UINT32 id;

	::GetCursorPos( &pt);

	{
		TVHITTESTINFO info;

		info.pt = pt;

		::ScreenToClient( hwndCtrl, &info.pt);

		hItem = TreeView_HitTest( hwndCtrl, &info);
	}

	TreeView_SelectItem( hwndCtrl, hItem);

	// Popup Menu
	{
		HMENU hMenu, hPopupMenu;

		hMenu = LoadMenu( hInstance, MAKEINTRESOURCE( IDR_POPUP_SELBONE));

		hPopupMenu = ::GetSubMenu( hMenu, 0);

		id= ::TrackPopupMenuEx( hPopupMenu, TPM_RETURNCMD | TPM_HORPOSANIMATION | TPM_VERPOSANIMATION | TPM_VERTICAL,
				pt.x, pt.y, hwndCtrl, NULL);
		
		switch( id)
		{
			case ID_SELBONE_SELECTALLCHILDREN :
				if( hItem != NULL)
				{
					_TreeCtrlTraverse( hwnd, _SelectAllProc, NULL, hItem);
				}
				break;

			case ID_SELBONE_UNSELECTALLCHILDREN :
				if( hItem != NULL)
				{
					_TreeCtrlTraverse( hwnd, _UnselectAllProc, NULL, hItem);
				}
				break;
		}
	}

	*pResult = 0;
}

BOOL CALLBACK  _SelectBoneDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT id, code;
	HWND hwndCtrl;

	switch( msg) 
	{
		case WM_INITDIALOG:
			s_pExcludeList = g_pExport->getExcludeAnimList();

			{
				i3tool::exporter::pack::OPTION_INFO * pOpt = g_pExport->getOption();

				if( pOpt->m_SelectBoneWndCX > 0)
				{
					::SetWindowPos( hwnd, NULL,	pOpt->m_SelectBoneWndX, pOpt->m_SelectBoneWndY,
						pOpt->m_SelectBoneWndCX, pOpt->m_SelectBoneWndCY,
						SWP_NOACTIVATE | SWP_NOZORDER);
				}
			}

			SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG)lParam);
			CenterWindow( hwnd, GetParent( hwnd));

			{
				RECT rt;

				GetClientRect( hwnd, &rt);
				_ReplaceControls( hwnd, rt.right, rt.bottom);
			}

			{
				hwndCtrl = ::GetDlgItem( hwnd, IDC_BONETREE);

				{
					COLORREF col;

					col = GetColorManager()->GetColor( kTrackbarBg );
					SendMessage( hwndCtrl, TVM_SETBKCOLOR, 0, (LPARAM) col);
				}
				_SetBones( hwndCtrl);
			}
			break;

		case WM_SIZE :
			_ReplaceControls( hwnd, LOWORD( lParam), HIWORD( lParam));
			break;

		case WM_SHOWWINDOW :
			// Upper Bone들을 기본으로 선택.
			_TreeCtrlTraverse( hwnd, _SelectUpper, NULL, TVI_ROOT);	
			break;

		case WM_NOTIFY:
			{
				id = wParam;
				NMHDR * pHdr = (NMHDR *) lParam;
				UINT32 Result = 0;

				switch( id)
				{
					case IDC_BONETREE :
						switch( pHdr->code)
						{
							case NM_RCLICK :		OnBoneTreeRClick( hwnd, pHdr, &Result);	break;
						}
						break;
				}

				SetWindowLong( hwnd, DWL_MSGRESULT, Result);
			}
			break;

		case WM_COMMAND:
			id = LOWORD(wParam);
			code = HIWORD( wParam);

			//pExport = (i3Export *)GetWindowLongPtr( hwnd, GWLP_USERDATA);

			if( code == BN_CLICKED)
			{
				switch( id) 
				{
					case ID_SELALL :	_TreeCtrlTraverse( hwnd, _SelectAllProc, NULL, TVI_ROOT);	break;
					case ID_UNSELALL :	_TreeCtrlTraverse( hwnd, _UnselectAllProc, NULL, TVI_ROOT);	break;
					case ID_REVSEL :	_TreeCtrlTraverse( hwnd, _InverseProc, NULL, TVI_ROOT);	break;
					case ID_UPDATE :	_TreeCtrlTraverse( hwnd, _UpdateSelectProc, NULL, TVI_ROOT);	break;

					case IDC_BTN_ALLBIPED :	_TreeCtrlTraverse( hwnd, _SelectAllBiped, NULL, TVI_ROOT);	break;
					case IDC_BTN_UPPER :	_TreeCtrlTraverse( hwnd, _SelectUpper, NULL, TVI_ROOT);	break;
					case IDC_BTN_LOWER :	_TreeCtrlTraverse( hwnd, _SelectLower, NULL, TVI_ROOT);	break;

					case IDOK:
						s_pExcludeList->clear();

						_TreeCtrlTraverse( hwnd, _UpdateExcludeProc, NULL, TVI_ROOT);

						EndDialog( hwnd, id);
						break;

					case IDCANCEL :
						EndDialog( hwnd, id);
						break;
				}
			}
			break;

		default:
			return false;
	}
	return true;
}
