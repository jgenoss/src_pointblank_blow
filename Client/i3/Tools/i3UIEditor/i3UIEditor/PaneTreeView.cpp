// PaneTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "PaneTreeView.h"
#include "GlobalVariable.h"


// CPaneTreeView dialog

IMPLEMENT_DYNAMIC(CPaneTreeView, i3TDKDialogBase)
CPaneTreeView::CPaneTreeView(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneTreeView::IDD, pParent)
{
}

CPaneTreeView::~CPaneTreeView()
{
}

void CPaneTreeView::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneTreeView, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPaneTreeView message handlers

void CPaneTreeView::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_TreeViewCtrl.m_hWnd ))
	{
		m_TreeViewCtrl.SetWindowPos( NULL, 0, 5, cx , cy -5 , SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

BOOL CPaneTreeView::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText( "SceneTreeView");

	RECT rc;
	GetClientRect(&rc);

	m_TreeViewCtrl.Create( WS_CHILD | WS_VISIBLE, rc, this, 0);

	i3TDK::RegisterUpdate( m_hWnd, i3Node::GetClassMeta(), I3_TDK_UPDATE_ALL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneTreeView::OnUpdate( I3_TDK_UPDATE_INFO * pInfo )
{
	if( pInfo->m_hwndFrom == this->m_hWnd )	return ;

	if( pInfo->m_Event & I3_TDK_UPDATE_SET )
	{
		if( pInfo->m_Event & I3_TDK_USRWND_MAINFRAME)
		{
			if( pInfo->m_pObject->IsTypeOf( i3Node::GetClassMeta()))
			{
				m_TreeViewCtrl.SetSceneGraph((i3Node*)pInfo->m_pObject);
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SCENE )
	{
		if( pInfo->m_pObject->IsTypeOf( i3Node::GetClassMeta()))
		{
			m_TreeViewCtrl.SetSceneGraph( (i3Node*)pInfo->m_pObject );
		}
	}

	/*
	else if( pInfo->m_Event & I3_TDK_UPDATE_SCENEGRAPH )
	{
		if( pInfo->m_pObject->IsTypeOf( i3Node::GetClassMeta()))
		{
			m_TreeViewCtrl.SetSceneGraph( (i3Node*)pInfo->m_pObject );
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_pObject->IsTypeOf( i3Node::GetClassMeta()))
		{
			m_TreeViewCtrl.SetSceneGraph( (i3Node*)pInfo->m_pObject);
		}
	}
	*/
}

LRESULT CPaneTreeView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}
