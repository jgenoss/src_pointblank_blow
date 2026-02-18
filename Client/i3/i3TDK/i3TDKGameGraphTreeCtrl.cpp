// i3TDKGameGraphTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKGameGraphTreeCtrl.h"
#include "i3TDKUtilGDI.h"
#include "i3TDKUtilCtrl.h"
#include "i3TDKUtilCommon.h"
#include "resource.h"
#include "i3TDKSelectNodeTypeDialog.h"
#include "i3TDKSingleSelectNodeTypeDialog.h"
#include "i3TDKSelectAttrTypeDialog.h"
#include "i3TDKMeta.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKOptSpecs.h"
#include "i3TDKUpdateSystem.h"
#include "i3TDKDlgOptProgress.h"

// i3TDKGameGraphTreeCtrl

enum
{
	I3ICON_CAMERA = 0,
	I3ICON_CHARA = 2,
	I3ICON_LIGHT = 4,
	I3ICON_OBJECT = 6,
	I3ICON_PATH = 8,
	I3ICON_RESPAWN = 10,
	I3ICON_TRIGGER = 12,
	I3ICON_WORLD = 14,
	I3ICON_NODE = 16,
};

IMPLEMENT_DYNAMIC(i3TDKGameGraphTreeCtrl, CWnd)

i3TDKGameGraphTreeCtrl::~i3TDKGameGraphTreeCtrl()
{
	if( m_pImageList != nullptr)
	{
		delete m_pImageList;
		m_pImageList = nullptr;
	}

	I3_SAFE_RELEASE( m_pRoot);
}

BOOL i3TDKGameGraphTreeCtrl::Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hBack;

	m_dwStyle = dwStyle;

	hCursor = LoadCursor( nullptr, IDC_ARROW);
	I3ASSERT( hCursor != nullptr);

	hBack = (HBRUSH) GetStockObject( NULL_BRUSH);
	I3ASSERT( hBack != nullptr);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW, hCursor, hBack, nullptr);
	I3ASSERT( pszClass != nullptr);

	dwStyle |= WS_CLIPCHILDREN;

	return CWnd::Create( pszClass, _T(""), dwStyle, rect, pParentWnd, nID, nullptr);
}

static void	MakeItemName( TCHAR* pszStr, INT32 outStrSize, i3GameNode * pNode)
{
	i3ClassMeta * pMeta = pNode->meta();
	i3::snprintf( pszStr, outStrSize, _T("%s"), pMeta->class_name() );
}

INT32 i3TDKGameGraphTreeCtrl::getChildItemCount( i3ElementBase * pObj)
{
	INT32 c = 0;

	if( i3::kind_of<i3GameNode*>(pObj))
	{
		i3GameNode * pNode = (i3GameNode *) pObj;

		c += pNode->GetChildCount();
	}

	return c;
}

void i3TDKGameGraphTreeCtrl::_SetTreeItem( TVITEM * pInfo, i3ElementBase * pObj, const TCHAR* szMetaName, HTREEITEM hItem)
{
//	i3ClassMeta * pMeta = pObj->meta();
	INT32 ImageIdx;

	pInfo->mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_TEXT;
	pInfo->hItem = hItem;
	pInfo->lParam = (LPARAM) pObj;

//#ifdef _UNICODE
//i3::stack_wstring wstrMeta;		i3::mb_to_utf16(pMeta->class_name(), wstrMeta);
//	const wchar_t* meta_name = wstrMeta.c_str();
//#else
//	const char*	meta_name = pMeta->class_name().c_str();
//#endif
	pInfo->pszText = (TCHAR*)szMetaName; 		//	const_cast<TCHAR*>(meta_name); 


	if( i3::kind_of<i3GameNode* >(pObj))
	{
		i3GameNode * pNode = (i3GameNode *) pObj;

		if( i3::same_of<i3CameraObject* >(pNode))
			ImageIdx = I3ICON_CAMERA;
		else if( i3::kind_of< i3Chara* >(pNode))
			ImageIdx = I3ICON_CHARA;
		else if( i3::same_of< i3LightObject* >(pNode))
			ImageIdx = I3ICON_LIGHT;
		else if( i3::kind_of< i3Object* >(pNode))
			ImageIdx = I3ICON_OBJECT;
		else if( i3::same_of<i3PathLine* >(pNode))
			ImageIdx = I3ICON_PATH;
		else if( i3::same_of<i3RespawnObj* >(pNode))
			ImageIdx = I3ICON_RESPAWN;
		else if( i3::kind_of< i3Trigger* >(pNode))
			ImageIdx = I3ICON_TRIGGER;
		else if( i3::kind_of< i3World* >(pNode))
			ImageIdx = I3ICON_WORLD;
		else
			ImageIdx = I3ICON_NODE;
		
		pInfo->cChildren = getChildItemCount( pNode);
	}
	else
	{
		ImageIdx = I3ICON_NODE;
		pInfo->cChildren = 0;
	}

	pInfo->iImage = ImageIdx + 1;
	pInfo->iSelectedImage = ImageIdx;
}

HTREEITEM	i3TDKGameGraphTreeCtrl::AddTreeItem( i3ElementBase * pObj, HTREEITEM hParent)
{
	TVINSERTSTRUCT info;
	HTREEITEM hItem;

	info.hInsertAfter = TVI_LAST;
	info.hParent = hParent;
#ifdef _UNICODE	
	i3::stack_wstring strMeta;	i3::mb_to_utf16(pObj->meta()->class_name(), strMeta);
#else
	const i3::fixed_string& strMeta = pObj->meta()->class_name();
#endif

	_SetTreeItem( &info.item, pObj, strMeta.c_str());

	hItem = m_TreeCtrl.InsertItem( &info);

	if((hItem != nullptr) && (hParent != nullptr))
	{
		TVITEM item;

		item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_PARAM;
		item.hItem = hParent;

		m_TreeCtrl.GetItem( &item);

		if( item.lParam != 0)
		{
			item.cChildren = getChildItemCount( (i3ElementBase *) item.lParam);
		}
		else
		{
			item.cChildren = 1;
		}

		m_TreeCtrl.SetItem( &item);
	}

	return hItem;
}

void i3TDKGameGraphTreeCtrl::Rec_SetTree( HTREEITEM hParentItem, i3GameNode * pNode)
{
	HTREEITEM hItem;
	i3GameNode * pChild;

	hItem = AddTreeItem( pNode, hParentItem);

	pChild = pNode->getFirstChild();
	while( pChild != nullptr)
	{
		Rec_SetTree( hItem, pChild);

		pChild = pChild->getNext();
	}
}

void i3TDKGameGraphTreeCtrl::setGameGraph( i3GameNode * pRoot)
{
	I3_REF_CHANGE( m_pRoot, pRoot);

	m_TreeCtrl.DeleteAllItems();

	if( m_pRoot != nullptr)
	{
		Rec_SetTree( nullptr, m_pRoot);
	}

	OnSetGameGraph();
}

void i3TDKGameGraphTreeCtrl::SetShowStyle( UINT32 style)
{
	m_ShowStyle = style;
}

void i3TDKGameGraphTreeCtrl::AddShowStyle( UINT32 style)
{
	SetShowStyle( m_ShowStyle | style);
}

void i3TDKGameGraphTreeCtrl::RemoveShowStyle( UINT32 style)
{
	SetShowStyle( m_ShowStyle & ~style);
}

BEGIN_MESSAGE_MAP(i3TDKGameGraphTreeCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( NM_RCLICK, ID_TDK_SGTREECTRL, OnNMRclickBoneTree)
	ON_NOTIFY(NM_CUSTOMDRAW, ID_TDK_SGTREECTRL, OnNMCustomdrawBoneTree)
	ON_NOTIFY( TVN_SELCHANGED, ID_TDK_SGTREECTRL, OnTVNSelChanged)
	ON_NOTIFY( TVN_KEYDOWN,		ID_TDK_SGTREECTRL, OnTVNKeyDown)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// i3TDKGameGraphTreeCtrl message handlers

void i3TDKGameGraphTreeCtrl::AddIcon( INT32 id)
{
	HICON hIcon;

	hIcon = ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));
	m_pImageList->Add( hIcon);
	::DestroyIcon( hIcon);
}

int i3TDKGameGraphTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Tree Control
	{
		CRect rect;

		GetClientRect( &rect);

		if( m_TreeCtrl.Create( m_dwStyle | WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES |
								TVS_INFOTIP | TVS_LINESATROOT,
								rect, this, ID_TDK_SGTREECTRL) == FALSE)
		{
			I3ASSERT_0;
		}
	}

	{
		m_pImageList = new CImageList;
		m_pImageList->Create( 16, 16, ILC_COLOR32 | ILC_MASK, 32, 0);

		AddIcon( IDI_CAMERA);
		AddIcon( IDI_CAMERA);
		AddIcon( IDI_TDK_CHARACTER);
		AddIcon( IDI_TDK_CHARACTER);
		AddIcon( IDI_TDK_LIGHT_SPOT);
		AddIcon( IDI_TDK_LIGHT_SPOT);
		AddIcon( IDI_TDK_OBJECT);
		AddIcon( IDI_TDK_OBJECT);
		AddIcon( IDI_NODE);
		AddIcon( IDI_NODE);
		AddIcon( IDI_TDK_RESPAWN);
		AddIcon( IDI_TDK_RESPAWN);
		AddIcon( IDI_GEOMETRY);
		AddIcon( IDI_GEOMETRY);
		AddIcon( IDI_TDK_WORLD);
		AddIcon( IDI_TDK_WORLD);
		AddIcon( IDI_NODE);
		AddIcon( IDI_NODE);

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);
		//m_TreeCtrl.SetItemHeight( 16);
	}

	{
		m_hDropCursor = (HCURSOR) ::LoadCursor( g_hInstTDK, MAKEINTRESOURCE( IDC_TDK_DRAG));
		m_hNoDropCursor = (HCURSOR) ::LoadCursor( nullptr, IDC_NO);
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3GameNode::static_meta(), I3_TDK_UPDATE_ALL);
	}

	return 0;
}

void i3TDKGameGraphTreeCtrl::OnDestroy()
{
	if( m_hDropCursor != nullptr)
	{
		DeleteObject( m_hDropCursor);
	}

	CWnd::OnDestroy();
}


void i3TDKGameGraphTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

//static INT32 _MetaCompareProc( i3ClassMeta * p1, i3ClassMeta * p2)
//{
//	return strcmp( p1->GetClassName(), p2->GetClassName());
//}

void i3TDKGameGraphTreeCtrl::OnNMRclickBoneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = nullptr;
//	List MetaList;			// 쓰이지 않아 주석..
	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if ((hSel != nullptr) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	{
		HINSTANCE hOld = AfxGetResourceHandle();

		AfxSetResourceHandle( g_hInstTDK);

		Menu.LoadMenu( IDR_POPUP_NODE);

		AfxSetResourceHandle( hOld);
	}

	pPopup = Menu.GetSubMenu( 0);

	if( hSel == nullptr)
	{
		pPopup->EnableMenuItem( ID_GG_EXPAND, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_GG_COLLAPSE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SELECT_SELECTALLDESCENDANTS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SELECT_UNSELECTALLDESCENDANTS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_GG_DELETE, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
	}

	OnPopupMenu( pPopup, hSel);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, nullptr);


	switch( cmd)
	{
		case ID_GG_EXPAND :						Expand( hSel);	break;
		case ID_GG_COLLAPSE :					Collapse( hSel);	break;
		case ID_SELECT_ALL :					SelectAll();		break;
		case ID_SELECT_UNSELECTALL :			UnselectAll();	break;
		case ID_SELECT_SELECTALLDESCENDANTS :	SelectDescendant( hSel);	break;
		case ID_SELECT_UNSELECTALLDESCENDANTS :	UnselectDescendant( hSel);	break;
		case ID_SELECT_SELECTBYTYPE :			SelectByType();	break;
		case ID_SELECT_REVERSESELECTION :		ReverseSelection();	break;

		case ID_NODE_DELETE :					Delete( hSel);		break;

		default :
			break;
	}

	OnSelectMenu( cmd);
	
	*pResult = 1;
}

/////////
static BOOL _ExpandProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	if( pData == nullptr)
		pCtrl->Expand( hItem, TVE_EXPAND);
	else
		pCtrl->Expand( hItem, TVE_COLLAPSE);

	return TRUE;
}

void i3TDKGameGraphTreeCtrl::Expand( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _ExpandProc, nullptr);
}

void i3TDKGameGraphTreeCtrl::Collapse( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _ExpandProc, (void *) 1);
}

/////////
static BOOL _SelectAllProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	switch( (INT32) pData)
	{
		case 0 :
			if( pCtrl->GetCheck( hItem) == FALSE)
			{
				pCtrl->SetCheck( hItem, TRUE);
			}
			break;

		case 1 :
			if( pCtrl->GetCheck( hItem) == TRUE)
			{
				pCtrl->SetCheck( hItem, FALSE);
			}
			break;

		case 2 :
			pCtrl->SetCheck( hItem, ! pCtrl->GetCheck( hItem));
			break;
	}

	return TRUE;
}

void i3TDKGameGraphTreeCtrl::SelectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, nullptr);
}

void i3TDKGameGraphTreeCtrl::UnselectAll(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 1);
}

void i3TDKGameGraphTreeCtrl::ReverseSelection(void)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 2);
}

void i3TDKGameGraphTreeCtrl::SelectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, nullptr, hItem);
}

void i3TDKGameGraphTreeCtrl::UnselectDescendant( HTREEITEM hItem)
{
	i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _SelectAllProc, (void *) 1, hItem);
}

void i3TDKGameGraphTreeCtrl::SelectByType(void)
{
}


BOOL i3TDKGameGraphTreeCtrl::IsValidItem( HTREEITEM hItem)
{
	TVITEM data;

	data.mask = TVIF_HANDLE;
	data.hItem = hItem;

	return m_TreeCtrl.GetItem( &data);
}

void i3TDKGameGraphTreeCtrl::Delete( HTREEITEM hItem)
{
	/*
	List SelList;
	INT32 i;
	i3GameNode * pNode;
	i3ElementBase * pObj;

	GetCheckedItem( &SelList);

	for( i = 0; i < SelList.GetCount(); i++)
	{
		hItem = (HTREEITEM) SelList.Items[i];

		if( IsValidItem( hItem))
		{

			pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hItem);
			I3ASSERT( pObj != nullptr);

			if( pObj->IsTypeOf( i3GameNode::GetClassMeta()))
			{
				pNode = (i3GameNode *) pObj;

				pNode->RemoveFromParent();
			}
			else if( pObj->IsTypeOf( i3RenderAttr::GetClassMeta()))
			{
				i3RenderAttr * pAttr = (i3RenderAttr *) pObj;
				i3AttrSet * pParent;
				HTREEITEM hParent = m_TreeCtrl.GetParentItem( hItem);
				I3ASSERT( (hParent != TVI_ROOT) && (hParent != nullptr));

				pParent = (i3AttrSet *) m_TreeCtrl.GetItemData( hParent);
				I3ASSERT( pParent->IsTypeOf( i3AttrSet::GetClassMeta()));

				pParent->RemoveAttr( pAttr);
			}
			else if( pObj->IsTypeOf( i3Collider::GetClassMeta()))
			{
			}

			m_TreeCtrl.DeleteItem( hItem);
		}
	}
	*/
}


/////////
/* 쓰이지 않으므로 주석.
static BOOL _GetCheckedItemProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	List * pList = (List *) pData;

	if( pCtrl->GetCheck( hItem))
	{
		pList->Add( (void *) hItem);
	}

	return TRUE;
}
*/
/////////
/*  현재 쓰이지 않으므로 주석.
static BOOL _GetSelectedItemProc( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	List * pList = (List *) pData;

	if( pCtrl->GetItemState( hItem, 0xFFFFFFFF) & TVIS_SELECTED)
	{
		pList->Add( (void *) hItem);
	}

	return TRUE;
}
*/
/*  현재 쓰이지 않으므로 주석.
void i3TDKGameGraphTreeCtrl::GetCheckedItem( List * pList)
{
	if( m_TreeCtrl.GetStyle() & TVS_CHECKBOXES)
	{
		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _GetCheckedItemProc, pList);
	}

	if( pList->GetCount() <= 0)
	{
		i3TDK::TreeCtrlTraverse( &m_TreeCtrl, _GetSelectedItemProc, pList);
	}
}
*/

void i3TDKGameGraphTreeCtrl::OnPopupMenu( CMenu * pMenu, HTREEITEM)
{
}

void i3TDKGameGraphTreeCtrl::OnSelectMenu( UINT)
{
}


void i3TDKGameGraphTreeCtrl::OnNMCustomdrawBoneTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	HDC dc = pNMCD->nmcd.hdc;
	RECT * pRect = & pNMCD->nmcd.rc;

	switch( pNMCD->nmcd.dwDrawStage)
	{
		//case CDDS_POSTERASE :
		//case CDDS_POSTPAINT :
		//case CDDS_PREERASE :
		case CDDS_PREPAINT :		*pResult = CDRF_NOTIFYITEMDRAW;	return;

		//case CDDS_ITEM :
		//case CDDS_ITEMPOSTERASE :
		//case CDDS_ITEMPOSTPAINT :
		//case CDDS_ITEMPREERASE :
		case CDDS_ITEMPREPAINT :
			{
				HTREEITEM hItem = (HTREEITEM) pNMCD->nmcd.dwItemSpec;
				i3ElementBase * pElm = (i3ElementBase *) pNMCD->nmcd.lItemlParam;

				if( pElm != nullptr )
				{
					if( i3::kind_of<i3GameNode*>(pElm) )
					{
						CRect rc;
						i3GameNode * pNode = (i3GameNode *) pElm;
					
						m_TreeCtrl.GetItemRect( hItem, &rc, TRUE);

						rc.left = rc.right;
						rc.right = pRect->right;

						/*
						if( m_ShowStyle & I3TDK_SGTREE_SHOW_FLAGS)
						{
							// Flags
							SetTextColor( dc, RGB( 192, 92, 92));

							i3TDK::GetNodeFlagString( pNode, conv);

							len = strlen( conv);
							DrawText( dc, conv, len, &rc, 0);

							GetTextExtentPoint32( dc, conv, len, &sz);

							sz.cx += 8;
							rc.left += sz.cx;
							rc.right += sz.cx;
						}
						*/

						///if( m_ShowStyle & I3TDK_SGTREE_SHOW_NAME)
						{
							const TCHAR* pszName;
#ifdef _UNICODE 
							i3::stack_wstring wstrName;
#endif

							SetTextColor( dc, RGB( 0, 0, 255));
							
							if( pNode->hasName())
							{
#ifdef _UNICODE
								i3::mb_to_utf16(pNode->GetName(), wstrName);
								pszName = wstrName.c_str();
#else
								pszName = pNode->GetName();
#endif
							}
							else
							{
								pszName = _T("\"\"");
							}

							DrawText( dc, pszName, i3::generic_string_size( pszName), &rc, 0);
						}

						SetTextColor( dc, pNMCD->clrText);
					}
				}

				*pResult = CDRF_DODEFAULT;
			}
			return;
		//case CDDS_SUBITEM :
	}
	
	*pResult = 0;
}

void i3TDKGameGraphTreeCtrl::OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew)
{
}

void i3TDKGameGraphTreeCtrl::OnSetGameGraph(void)
{
}

void i3TDKGameGraphTreeCtrl::OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTREEVIEW * pNMTV = (NMTREEVIEW *) pNMHDR;

	OnChangeSelectItem( pNMTV->itemOld.hItem, pNMTV->itemNew.hItem);

	i3ElementBase * pOldObj = (i3ElementBase *) pNMTV->itemOld.lParam;
	i3ElementBase * pNewObj = (i3ElementBase *) pNMTV->itemNew.lParam;

	if( pOldObj != nullptr)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pOldObj);
	}

	if( pNewObj != nullptr)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pNewObj);
	}

	*pResult = 0;
}

void i3TDKGameGraphTreeCtrl::OnTVNKeyDown( NMHDR * pNMHDR, LRESULT * pResult)
{
	NMTVKEYDOWN * pNMTV = (NMTVKEYDOWN *) pNMHDR;
	
	switch( pNMTV->wVKey)
	{
		case 'D' :
			{
				HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

				if( hItem != nullptr)
				{
					i3ElementBase * pObj = (i3ElementBase *) m_TreeCtrl.GetItemData( hItem);

					if( i3::kind_of<i3GameNode*>(pObj))
					{

						i3GameNode * pNode = (i3GameNode *) pObj;

						if( pNode->getGNodeState() & I3_GAMENODE_STATE_DISABLE)
							pNode->removeGNodeState( I3_GAMENODE_STATE_DISABLE);
						else
							pNode->addGNodeState( I3_GAMENODE_STATE_DISABLE);

						Invalidate();
					}
				}
			}
			break;
	}

	*pResult = 0;
}

HTREEITEM	i3TDKGameGraphTreeCtrl::_FindTreeItemByObj( i3ElementBase * pObj)
{
	HTREEITEM hItem = i3TDK::FindTreeItemByData( &m_TreeCtrl, pObj);

	return hItem;
}

void i3TDKGameGraphTreeCtrl::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		if( pInfo->m_pObject != nullptr)
		{
			HTREEITEM hItem = _FindTreeItemByObj( pInfo->m_pObject);

			if( hItem != nullptr)
			{
				TVITEM info;

#ifdef _UNICODE	
				i3::stack_wstring strMeta;	i3::mb_to_utf16(pInfo->m_pObject->meta()->class_name(), strMeta);
#else
				const i3::fixed_string& strMeta = pInfo->m_pObject->meta()->class_name();
#endif
				_SetTreeItem( &info, pInfo->m_pObject, strMeta.c_str(), hItem);

				m_TreeCtrl.SetItem( &info);
			}
		}
	}
}

LRESULT i3TDKGameGraphTreeCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CWnd::WindowProc(message, wParam, lParam);
}
