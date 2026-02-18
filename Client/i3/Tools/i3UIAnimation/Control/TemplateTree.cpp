// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "TemplateTree.h"
#include "../i3UIEditor.h"
#include "../UIGlobalRes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef VK_C
#define VK_C (0x41 + 'C' - 'A')
#endif

#ifndef VK_V
#define VK_V (0x41 + 'V' - 'A')
#endif

#ifndef VK_Z
#define VK_Z (0x41 + 'Z' - 'A')
#endif

#ifndef VK_X
#define VK_X (0x41 + 'X' - 'A')
#endif

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWnd

CTemplateTree::CTemplateTree()
{
	m_pImageList = NULL;
}

CTemplateTree::~CTemplateTree()
{
}

BEGIN_MESSAGE_MAP(CTemplateTree, CTreeCtrl)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	//ON_NOTIFY_REFLECT(NM_CLICK, &CTemplateTree::OnNMClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWnd message handlers
int CTemplateTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	lpCreateStruct->style &= ~TVS_SINGLEEXPAND;

	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CFont * pFont = CFont::FromHandle( UIGlobalRes::getDefFont());

		SetFont( pFont);
	}

	return 0;
}
 
 BOOL CTemplateTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
 {
 	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);
 
 	NMHDR* pNMHDR = (NMHDR*)lParam;
 	ASSERT(pNMHDR != NULL);
 
 	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
 	{
 		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
 	}
 
 	return bRes;
 }

 BOOL CTemplateTree::PreTranslateMessage(MSG* pMsg)
 {
 	
 	// TODO: Add your specialized code here and/or call the base class
 	if (pMsg->message == WM_KEYDOWN &&
          pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
       {
          CEdit* edit = GetEditControl();
          if (edit)
          {
             edit->SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
             return TRUE;
          }
       }
 	else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F2) // Rename
 	{
 		EditLabel( GetSelectedItem());
 		return TRUE;
 	}
 	else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5) // MoveDown
 	{
 		
 		return TRUE;
 	}
 	else if( pMsg->message == WM_KEYDOWN && pMsg->wParam != VK_CONTROL )
 	{
 		CEdit* edit = GetEditControl();
 
 		if( GetAsyncKeyState( VK_CONTROL ) && edit )
 		{
 			switch( pMsg->wParam )
 			{
 			case	VK_C:
 					edit->Copy();
 					return true;
 
 			case	VK_X:
 					edit->Cut();
 					return true;
 
 			case	VK_V:
 					edit->Paste();
 					return true;
 
 			case	VK_Z:
 					edit->Undo();
 					return true;
 			}
 		}
 	}
 	
 
 	return CTreeCtrl::PreTranslateMessage(pMsg);
 }


// CTemplateTree message handlers
void	CTemplateTree::InitImageList(void)
{
	I3ASSERT( m_pImageList == NULL);

	m_pImageList = UIGlobalRes::getDefSmallImageList();

	SetImageList( m_pImageList, TVSIL_NORMAL);

	// 스크립트 여부 표시
	m_ImageList2.Create( IDB_BITMAP1, 16, 4, RGB(255, 255, 255));
	SetImageList( &m_ImageList2, TVSIL_STATE);
}


INT32 CTemplateTree::GetIconIndex( i3ElementBase * pElement)
{
	INT32 idx = -1;

	if( pElement == NULL)
	{
		idx = UIGlobalRes::getImageIndex( NULL);
	}
	else
	{
		if( i3::same_of<i3UILTreeElement*>(pElement)) //->IsExactTypeOf( i3UILTreeElement::static_meta()))
		{
			i3UILTreeElement * pTElm = (i3UILTreeElement*)pElement;
			idx = UIGlobalRes::getImageIndex( pTElm->getElementMeta());
		}
		else
		{
			idx = UIGlobalRes::getImageIndex( pElement->meta());
		}
	}

	I3ASSERT( idx != -1);

	return idx;
}

INT32 CTemplateTree::GetLockStateImage( i3UIControl * pControl)
{
	INT32 idx = -1;

	if( pControl != NULL)
	{
	}

	return idx;
}


static BOOL _setItemStateImage( CTreeCtrl * pCtrl, HTREEITEM hItem, void * pData)
{
	i3ElementBase * pItem = (i3ElementBase*)pCtrl->GetItemData( hItem);
	if( pItem == NULL)
		return FALSE;

	if( i3::same_of<i3UIScene*>(pItem)) //->IsExactTypeOf( i3UIScene::static_meta()))
	{
		i3UIScene* pScene = (i3UIScene*)pItem;
		const char* scriptFile = pScene->getScriptFile();
		if( scriptFile == NULL)
		{
			pCtrl->SetItemState( hItem, INDEXTOSTATEIMAGEMASK(UIT_IMASK_EMPTY), TVIS_STATEIMAGEMASK);
		}
		else
		{
			pCtrl->SetItemState( hItem, INDEXTOSTATEIMAGEMASK(UIT_IMASK_SCRIPT), TVIS_STATEIMAGEMASK);
		}
	}
	else if( i3::kind_of<i3UIControl*>(pItem)) //->IsTypeOf( i3UIControl::static_meta()))
	{
		i3UIControl * pControl = (i3UIControl*)pItem;
		if( pControl->isEnable())
		{
			pCtrl->SetItemState( hItem, INDEXTOSTATEIMAGEMASK(UIT_IMASK_ENABLE), TVIS_STATEIMAGEMASK);
		}
		else
		{
			pCtrl->SetItemState( hItem, INDEXTOSTATEIMAGEMASK(UIT_IMASK_DISABLE), TVIS_STATEIMAGEMASK);
		}
	}
	else
	{
		I3ASSERT(0);
	}

	return TRUE;
}

void CTemplateTree::setItemStateImage(HTREEITEM hItem, void * pData)
{
	_setItemStateImage( this, hItem, pData);
}

void CTemplateTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//I3TRACE(" --CTemplateTree::OnLButtonDown: %d\n", nFlags);

	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point, &uFlags);
	if( uFlags & TVHT_ONITEMSTATEICON)
	{ 
		i3ElementBase * pItem = (i3ElementBase*)GetItemData( hItem);

		if( i3::kind_of<i3UIControl*>(pItem)) //->IsTypeOf( i3UIControl::static_meta()))
		{
			i3UIControl * pControl = (i3UIControl*)pItem;
			
			pControl->EnableCtrl( !pControl->isEnable(), true);

			if( !pControl->isEnable())
			{	
				i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pControl);
			}

			i3TDK::TreeCtrlTraverse( this, _setItemStateImage, NULL, hItem);
		}

		return;
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

// LButtonDown을 모두 catch하고 싶을 때 
// Double-click, Triple-click 
//void CTemplateTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	//UINT uFlags = 0;
//	//CPoint point;
//	//
//	//GetCursorPos( &point);
//	//ScreenToClient( &point);
//
//	//HTREEITEM hItem = HitTest(point, &uFlags);
//	//if( uFlags & TVHT_ONITEMSTATEICON)
//	//{ 
//	//	i3ElementBase * pItem = (i3ElementBase*)GetItemData( hItem);
//
//	//	if( i3::kind_of<i3UIControl*>(pItem)) //->IsTypeOf( i3UIControl::static_meta()) )
//	//	{
//	//		i3UIControl * pControl = (i3UIControl*)pItem;
//	//		
//	//		pControl->EnableCtrl( !pControl->isEnable(), true);
//
//	//		if( !pControl->isEnable())
//	//		{
//	//			i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, pControl);
//	//		}
//
//	//		i3TDK::TreeCtrlTraverse( this, _setItemStateImage, NULL, hItem);
//	//	}
//	//}
//
//	//*pResult = 0;
//}
//