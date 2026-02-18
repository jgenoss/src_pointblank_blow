// PanelViewer.cpp : implementation file
//

#include "stdafx.h"
#include "i3CollisionEditor.h"
#include "PanelViewer.h"
#include "GlobalVar.h"
#include "CollisionEditorFramework.h"


// CPanelViewer dialog

IMPLEMENT_DYNAMIC(CPanelViewer, CDialog)
CPanelViewer::CPanelViewer(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelViewer::IDD, pParent)
{
	m_pFramework = NULL;
}

CPanelViewer::~CPanelViewer()
{
	I3_SAFE_RELEASE( m_pFramework);
}

void CPanelViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelViewer, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPanelViewer message handlers

BOOL CPanelViewer::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rect;

		rect.SetRect( 0, 0, 100, 100);

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		{
			m_pFramework = CollisionEditorFramework::NewObject();
			m_pFramework->Create( m_ViewerCtrl.getViewer());
		}

		g_pViewer = m_ViewerCtrl.getViewer();
		g_pFramework = m_pFramework;

		m_pFramework->SetGridShow( false);
		m_pFramework->SetPanelVisible( false);

		m_ViewerCtrl.getViewer()->Pause();

		///g_pRenderPanel = &m_ViewerCtrl;
	}

	i3TDK::RegisterUpdate( m_hWnd, i3SceneGraphInfo::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelViewer::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ViewerCtrl.m_hWnd))
	{
		m_ViewerCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelViewer::OnDestroy()
{
	m_pFramework->SetSceneGraph( NULL);

	CDialog::OnDestroy();
}

void CPanelViewer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		if( pInfo->m_pObject != NULL)
		{
			m_pFramework->SetSceneGraph( g_pProject->getRoot());
		}
		else
		{
			m_pFramework->SetSceneGraph( NULL);
		}
	}
}

LRESULT CPanelViewer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
