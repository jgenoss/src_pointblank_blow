// PaneTree.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackFileEditor.h"
#include "PaneTree.h"
#include ".\panetree.h"
#include "i3PackFileEditorDoc.h"
#include "i3PackFileEditorView.h"
#include "GlobalVariable.h"

// CPaneTree dialog

IMPLEMENT_DYNAMIC(CPaneTree, CDialog)
CPaneTree::CPaneTree(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneTree::IDD, pParent)
{
}

CPaneTree::~CPaneTree()
{
}

void CPaneTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
}

void CPaneTree::OnAddNode( PackNode * pNode)
{
	HTREEITEM hParent;

	if( pNode->GetParent() != NULL)
	{
		hParent = i3Win32::FindTreeItemByData( &m_TreeCtrl, pNode->GetParent());

		if( hParent == NULL)
			hParent = TVI_ROOT;
	}
	else
	{
		hParent = TVI_ROOT;
	}

	TVINSERTSTRUCT data;

	memset( &data, 0, sizeof(data));
	data.hParent = hParent;
	data.hInsertAfter = TVI_SORT;
	data.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT;
	data.item.cChildren = 0;
	data.item.lParam = (LPARAM) pNode;
	data.item.pszText = (LPSTR) pNode->GetName();

	m_TreeCtrl.InsertItem( &data);

	// Parent TreeItem의 Child Count 변경
	{
		TVITEM item;

		item.mask = TVIF_CHILDREN | TVIF_HANDLE;
		item.hItem = hParent;
		item.cChildren = 1;

		m_TreeCtrl.SetItem( &item);
	}
}

void CPaneTree::OnRemoveNode( PackNode * pNode)
{
	HTREEITEM hItem;

	hItem = i3Win32::FindTreeItemByData( &m_TreeCtrl, pNode);

	if( hItem != NULL)
	{
		m_TreeCtrl.DeleteItem( hItem);
	}
}

void CPaneTree::OnRemoveAllNode(void)
{
	m_TreeCtrl.DeleteAllItems();
}

void CPaneTree::Rec_UpdatePackFile( PackNode * pNode, HTREEITEM hParent)
{
	HTREEITEM hItem;

	while( pNode != NULL)
	{
		TVINSERTSTRUCT data;

		memset( &data, 0, sizeof(data));
		data.hParent = hParent;
		data.hInsertAfter = TVI_SORT;
		data.item.mask = TVIF_CHILDREN | TVIF_PARAM | TVIF_TEXT;
		data.item.cChildren = 0;
		data.item.lParam = (LPARAM) pNode;
		data.item.pszText = (LPSTR) pNode->GetName();

		hItem = m_TreeCtrl.InsertItem( &data);

		// Parent TreeItem의 Child Count 변경
		{
			TVITEM item;

			item.mask = TVIF_CHILDREN | TVIF_HANDLE;
			item.hItem = hParent;
			item.cChildren = 1;

			m_TreeCtrl.SetItem( &item);
		}
		
		// Process Child Nodes
		Rec_UpdatePackFile( (PackNode *) pNode->GetFirstChild(), hItem);

		pNode = (PackNode *) pNode->GetNext();
	}
}

void CPaneTree::OnUpdatePackFile(void)
{
	Ci3PackFileEditorDoc * pDoc = m_pView->GetDocument();
	i3PackNode * pNode;

	m_TreeCtrl.DeleteAllItems();

	pNode = pDoc->m_pPack->GetRoot();

	pNode = pNode->GetFirstChild();

	if( pNode != NULL)
	{
		Rec_UpdatePackFile( (PackNode *) pNode, TVI_ROOT);
	}
}

void CPaneTree::Update( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
	{
		if( pInfo->m_pMeta->IsExactTypeOf( PackNode::GetClassMeta()))
		{
			OnAddNode( (PackNode *) pInfo->m_pObject);
		}
		else if( pInfo->m_pMeta->IsExactTypeOf( i3PackFile::GetClassMeta()))
		{
			OnUpdatePackFile();
		}
	}
	
	if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		if( pInfo->m_pMeta->IsExactTypeOf( PackNode::GetClassMeta()))
		{
			if( pInfo->m_pObject == NULL)
				OnRemoveAllNode();
			else
				OnRemoveNode( (PackNode *) pInfo->m_pObject);
		}
	}
}

LRESULT CPaneTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		this->Update( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


BEGIN_MESSAGE_MAP(CPaneTree, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_TREE, OnNMRclickTree)
END_MESSAGE_MAP()


// CPaneTree message handlers

BOOL CPaneTree::OnInitDialog()
{
	CDialog::OnInitDialog();

	RegisterUpdate( m_hWnd, PackNode::GetClassMeta(), I3_TDK_UPDATE_ALL);
	RegisterUpdate( m_hWnd, i3PackFile::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneTree::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPaneTree::OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT cmd;

	GetCursorPos( &pt);

	Menu.LoadMenu( MAKEINTRESOURCE( IDR_POPUP_NODE));

	pPopup = Menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) g_pView, NULL);

	switch( cmd)
	{
		case ID_PACKNODE_DELETE :
			{
				HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

				if( hItem != NULL)
				{
					PackNode * pNode = (PackNode *) m_TreeCtrl.GetItemData( hItem);

					Ci3PackFileEditorDoc * pDoc = m_pView->GetDocument();
					pDoc->RemoveFile( pNode);
				}
			}
			break;
	}

	*pResult = 0;
}
