// PanelRender.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelRender.h"
#include "GlobalVar.h"
#include ".\panelrender.h"

// CPanelRender dialog

IMPLEMENT_DYNAMIC(CPanelRender, i3TDKDialogBase)
CPanelRender::CPanelRender(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelRender::IDD, pParent)
{
	m_pFramework = NULL;
}

CPanelRender::~CPanelRender()
{
	I3_SAFE_RELEASE( m_pFramework);
}

void CPanelRender::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelRender, i3TDKDialogBase)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CPanelRender message handlers

BOOL CPanelRender::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		rect.SetRect( 0, 0, 100, 100);

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		{
			m_pFramework = i3CharaEditorFramework::new_object();
			m_pFramework->Create( m_ViewerCtrl.getViewer());

			m_pFramework->SetGridShow( FALSE);
		}

		g_pViewer = m_ViewerCtrl.getViewer();
		g_pFramework = m_pFramework;

		m_ViewerCtrl.getViewer()->Pause();

		g_pRenderPanel = &m_ViewerCtrl;

		COLOR col;
		i3Color::Set( &col,  (UINT8) 210, 210, 210, 255);
		g_pViewer->GetRenderContext()->SetClearColor( &col);
	}

	i3TDK::RegisterUpdate( m_hWnd, CChara::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelRender::OnDestroy()
{
	m_pFramework->SetSceneGraph( NULL);

	i3TDKDialogBase::OnDestroy();
}


void CPanelRender::OnOK()
{
}

void CPanelRender::OnCancel()
{
}

void CPanelRender::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ViewerCtrl.m_hWnd))
	{
		m_ViewerCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelRender::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	bool bSetChara = false;

	if( i3::same_of<CChara*>(pInfo->m_pMeta)) //->IsExactTypeOf( CChara::static_meta()))
	{
		if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
		{
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
		{
			bSetChara = true;
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
		{
			bSetChara = true;
		}
		else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
		{
			m_pFramework->SetSceneGraph( NULL);
		}
	}

	if( bSetChara)
	{
		if( g_pChara != NULL)
		{
			i3SceneObject * pSceneObj = g_pChara->geti3Chara()->getSceneObject();

			m_pFramework->SetSceneGraph( pSceneObj);
			//I3_SG_DUMP( g_pChara->geti3Chara()->GetSceneObject());
		}
	}
}

LRESULT CPanelRender::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanelRender::OnSetFocus(CWnd* pOldWnd)
{
	i3TDKDialogBase::OnSetFocus(pOldWnd);

	SetFocus();
}
