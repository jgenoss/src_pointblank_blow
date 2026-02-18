// PaneView.cpp : implementation file
//

#include "stdafx.h"
#include "i3AnimOptimizer.h"
#include "PaneView.h"
#include ".\paneview.h"
#include "GlobVar.h"

// CPaneView dialog

IMPLEMENT_DYNAMIC(CPaneView, CDialog)
CPaneView::CPaneView(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneView::IDD, pParent)
{
	m_pViewer = NULL;
	m_pFramework = NULL;
}

CPaneView::~CPaneView()
{
	I3_SAFE_RELEASE( m_pViewer);
	I3_SAFE_RELEASE( m_pFramework);
}

void CPaneView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneView, CDialog)
END_MESSAGE_MAP()


// CPaneView message handlers

BOOL CPaneView::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		i3VideoInfo video;

		video.m_Width = 100;
		video.m_Height = 100;

		m_pViewer = i3Viewer::NewObject();

		m_pViewer->Create( (void *) m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER | I3I_DEVICE_MASK_KEYBOARD, TRUE);
	}

	{
		m_pFramework = i3ViewerFramework::NewObject();
		m_pFramework->Create( m_pViewer);
	}

	RegisterUpdate( m_hWnd, i3Node::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	CPaneView::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		i3Node * pNode = (i3Node *) pInfo->m_pObject;

		//m_pViewer->SetSceneGraph( pNode);
		m_pFramework->SetSceneGraph( pNode);
	
		Invalidate();
	}
}


LRESULT CPaneView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
