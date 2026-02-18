// PanelViewer.cpp : implementation file
//

#include "stdafx.h"
#include "i3LevelDesign.h"
#include "PanelViewer.h"
#include ".\panelviewer.h"


// CPanelViewer dialog

IMPLEMENT_DYNAMIC(CPanelViewer, i3TDKDialogBase)
CPanelViewer::CPanelViewer(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelViewer::IDD, pParent)
{
}

CPanelViewer::~CPanelViewer()
{
}

void CPanelViewer::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

void CPanelViewer::DropObject( i3LevelElement * pObj, POINT pt)
{
	i3LevelElement3D * pInstance;

	if( pObj->IsTypeOf( i3LevelRes::GetClassMeta()))
	{
		i3LevelRes * pRes = (i3LevelRes *) pObj;

		// 새로운 Instance를 생성
		// 생성된 Instance는 이미 Scene에 추가되어 있다.
		pInstance = pRes->CreateInstance( g_pScene);

		//{
		//	i3LevelUndoInfoCreate * pInfo = i3LevelUndoInfoCreate::NewObject();

		//	pInfo->SetCreatedObject( pInstance);
		//	pInfo->Execute();
		//}

		if( pInstance != NULL)
		{
			VEC3D pos;
			i3LevelFramework * pFramework;

			pFramework = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

			pFramework->GetPickedPos( pt, &pos);

			pInstance->setPos( &pos);

			g_pScene->SelectElement( pInstance);
		}
	}
}

BEGIN_MESSAGE_MAP(CPanelViewer, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelViewer message handlers

BOOL CPanelViewer::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		GetClientRect( &rt);

		m_Viewport.Create( WS_CHILD | WS_VISIBLE, rt, this, IDC_VIEWERDUMMYWND, g_pScene);

		i3Level::RegisterUpdateNotify( m_hWnd, i3LevelScene::GetClassMeta(), I3_TDK_UPDATE_ALL);

		ReplaceControl( -1, -1);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelViewer::ReplaceControl( int cx, int cy)
{
	if( cx == -1)
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	if( ::IsWindow( m_Viewport.m_hWnd))
	{
		m_Viewport.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelViewer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	ReplaceControl( cx, cy);
}

void CPanelViewer::OnOK()
{
}

void CPanelViewer::OnCancel()
{
}

void CPanelViewer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		m_Viewport.Invalidate();
	}
}

LRESULT CPanelViewer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

BOOL CPanelViewer::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST))
	{
		::TranslateMessage( pMsg);
		::DispatchMessage( pMsg);
	}

	return i3TDKDialogBase::PreTranslateMessage(pMsg);
}
