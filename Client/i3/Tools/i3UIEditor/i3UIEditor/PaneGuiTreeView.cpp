// PaneGuiTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "PaneGuiTreeView.h"
#include "GlobalVariable.h"


// CPaneGuiTreeView dialog

IMPLEMENT_DYNAMIC(CPaneGuiTreeView, i3TDKDialogBase)
CPaneGuiTreeView::CPaneGuiTreeView(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneGuiTreeView::IDD, pParent)
{
}

CPaneGuiTreeView::~CPaneGuiTreeView()
{
}

void CPaneGuiTreeView::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneGuiTreeView, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPaneGuiTreeView message handlers

void CPaneGuiTreeView::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_TreeViewCtrl.m_hWnd ))
	{
		m_TreeViewCtrl.SetWindowPos( NULL, 0, 5, cx , cy -5 , SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

BOOL CPaneGuiTreeView::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText( "GuiTreeView");

	RECT rc;
	GetClientRect(&rc);

	m_TreeViewCtrl.Create( WS_CHILD | WS_VISIBLE, rc, this, 0);

	i3TDK::RegisterUpdate( m_hWnd, i3UIManager::GetClassMeta(), I3_TDK_UPDATE_SET);
	i3TDK::RegisterUpdate( m_hWnd, i3GameObjBase::GetClassMeta(), I3_TDK_UPDATE_EDIT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneGuiTreeView::OnUpdate( I3_TDK_UPDATE_INFO * pInfo )
{
	if( pInfo->m_hwndFrom == this->m_hWnd )	return ;

	if( pInfo->m_Event & I3_TDK_UPDATE_SET)
	{
		if( pInfo->m_Event & I3_TDK_USRWND_MAINFRAME)
		{
			if( pInfo->m_pObject->IsTypeOf( i3UIManager::GetClassMeta()))
			{
				i3GameNode * pObj = (i3GameNode*)pInfo->m_pObject;

				m_TreeViewCtrl.SetGuiGraph(pObj);
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_EDIT)
	{
		if( pInfo->m_Event & I3_TDK_USRWND_PROPERTY)
		{
			//item의 text를 새로 설정하여 줍니다.
			if( pInfo->m_pObject->IsTypeOf( i3GameObjBase::GetClassMeta()))
			{
				CTreeCtrl * pCtrl = m_TreeViewCtrl.GetTreeCtrl();

				HTREEITEM hItem = i3TDK::FindTreeItemByData( pCtrl, (i3GameObjBase*)pInfo->m_pObject);
				if( hItem != NULL)
				{
					char szName[MAX_PATH];
					m_TreeViewCtrl.MakeItemName( szName, (i3GameNode*)pInfo->m_pObject);

					pCtrl->SetItemText( hItem, szName);
				}
			}
		}
	}
}

LRESULT CPaneGuiTreeView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

