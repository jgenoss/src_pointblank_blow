// Panel_Viewer.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiOpt.h"
#include "Panel_Viewer.h"
#include "GlobalVar.h"

// CPanel_Viewer dialog

IMPLEMENT_DYNAMIC(CPanel_Viewer, i3TDKDialogBase)

CPanel_Viewer::CPanel_Viewer(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanel_Viewer::IDD, pParent)
{

}

CPanel_Viewer::~CPanel_Viewer()
{
}

void CPanel_Viewer::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

void	CPanel_Viewer::StartProcessing(void)
{
	m_ViewerCtrl.SuspendRender();
}

void	CPanel_Viewer::EndProcessing(void)
{
	m_ViewerCtrl.ResumeRender();
}


BEGIN_MESSAGE_MAP(CPanel_Viewer, i3TDKDialogBase)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// CPanel_Viewer message handlers

BOOL CPanel_Viewer::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rect;

		rect.SetRect( 0, 0, 100, 100);

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);

		{
			m_pFramework = i3ViewerFramework::new_object();
			m_pFramework->Create( m_ViewerCtrl.getViewer());
		}

		g_pViewer = m_ViewerCtrl.getViewer();
		g_pFramework = m_pFramework;

		g_pRenderPanel = &m_ViewerCtrl;
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3Node::static_meta(), I3_TDK_UPDATE_ALL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Viewer::OnOK()
{
}

void CPanel_Viewer::OnCancel()
{
}

void CPanel_Viewer::OnDestroy()
{
	m_pFramework->SetSceneGraph( NULL);
	I3_SAFE_RELEASE( m_pFramework);

	i3TDKDialogBase::OnDestroy();
}

LRESULT CPanel_Viewer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void CPanel_Viewer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ViewerCtrl.m_hWnd))
	{
		m_ViewerCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanel_Viewer::OnSetFocus(CWnd* pOldWnd)
{
	i3TDKDialogBase::OnSetFocus(pOldWnd);

	SetFocus();
}

BOOL CPanel_Viewer::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}
