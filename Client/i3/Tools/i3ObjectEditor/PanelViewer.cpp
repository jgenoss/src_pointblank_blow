// PanelViewer.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "PanelViewer.h"
#include "GlobalVar.h"
#include ".\panelviewer.h"


// CPanelViewer dialog

IMPLEMENT_DYNAMIC(CPanelViewer, i3TDKDialogBase)
CPanelViewer::CPanelViewer(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelViewer::IDD, pParent)
{
	m_pFramework = NULL;
}

CPanelViewer::~CPanelViewer()
{
	I3_SAFE_RELEASE( m_pFramework);
}

void CPanelViewer::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelViewer, i3TDKDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelViewer message handlers

BOOL CPanelViewer::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		rect.SetRect( 0, 0, 100, 100);

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		{
			m_pFramework = i3ObjectEditorFramework::new_object();
			//m_pFramework = (i3ObjectEditorFramework *) i3ViewerFramework::new_object();
			m_pFramework->Create( m_ViewerCtrl.getViewer());

			m_pFramework->SetGridShow( TRUE);
		}

		g_pViewer = m_ViewerCtrl.getViewer();
		g_pFramework = m_pFramework;

		m_ViewerCtrl.getViewer()->Pause();

		g_pRenderPanel = &m_ViewerCtrl;
	}

	i3TDK::RegisterUpdate( m_hWnd, CObjectTemplate::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, CObjectState::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelViewer::OnDestroy()
{
	m_pFramework->SetSceneGraph( NULL);
	I3_SAFE_RELEASE( m_pFramework);

	i3TDKDialogBase::OnDestroy();
}


void CPanelViewer::OnOK()
{
}

void CPanelViewer::OnCancel()
{
}

void CPanelViewer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( i3::kind_of<CObjectTemplate*>(pInfo->m_pMeta)) //->IsTypeOf( CObjectTemplate::static_meta()))
	{
		switch( pInfo->m_Event)
		{
			case I3_TDK_UPDATE_SCENE :
			case I3_TDK_UPDATE_EDIT :
				m_pFramework->SetSceneGraph( g_pObject->GetNode());
				m_ViewerCtrl.Invalidate();
				break;
		}
	}
	else if( i3::kind_of<CObjectState*>(pInfo->m_pMeta)) //->IsTypeOf( CObjectState::static_meta()))
	{
		switch( pInfo->m_Event)
		{			
			case I3_TDK_UPDATE_ADD :
			case I3_TDK_UPDATE_SELECT :
				m_ViewerCtrl.Invalidate();
				break;

		}
	}
}


LRESULT CPanelViewer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanelViewer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ViewerCtrl.m_hWnd))
	{
		m_ViewerCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}
