// Panel_SceneGraph.cpp : implementation file
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "Panel_SceneGraph.h"
#include ".\panel_scenegraph.h"


// CPanel_SceneGraph dialog

IMPLEMENT_DYNAMIC(CPanel_SceneGraph, i3TDKDialogBase)
CPanel_SceneGraph::CPanel_SceneGraph(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanel_SceneGraph::IDD, pParent)
{
}

CPanel_SceneGraph::~CPanel_SceneGraph()
{
}

void CPanel_SceneGraph::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanel_SceneGraph, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanel_SceneGraph message handlers

BOOL CPanel_SceneGraph::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		rt.SetRect( 0, 0, 10, 10);

		m_SgCtrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 100);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3SceneGraphInfo::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_SceneGraph::OnOK()
{
}

void CPanel_SceneGraph::OnCancel()
{
}

void CPanel_SceneGraph::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		m_SgCtrl.setSgInfo( (i3SceneGraphInfo *) pInfo->m_pObject);	
	}
}

LRESULT CPanel_SceneGraph::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanel_SceneGraph::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_SgCtrl.m_hWnd))
	{
		m_SgCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
