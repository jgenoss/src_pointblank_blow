// i3TDKViewerCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKViewerCtrl.h"
#include "i3TDKGlobalVariable.h"
#include ".\i3tdkviewerctrl.h"

// i3TDKViewerCtrl

IMPLEMENT_DYNAMIC(i3TDKViewerCtrl, CWnd)
i3TDKViewerCtrl::i3TDKViewerCtrl()
{
	g_pViewerCtrl = this;
}


BEGIN_MESSAGE_MAP(i3TDKViewerCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BOOL i3TDKViewerCtrl::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR lpszClass;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor( nullptr, IDC_ARROW);

	hbrBack = (HBRUSH) GetSysColorBrush( COLOR_3DFACE);

	lpszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, hCursor, hbrBack, nullptr);
	ASSERT( lpszClass);

	return CWnd::Create( lpszClass, nullptr, dwStyle, rect, pParentWnd, nID, nullptr);
}

// i3TDKViewerCtrl message handlers


BOOL i3TDKViewerCtrl::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

int i3TDKViewerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rt;

		GetClientRect( &rt);

		i3VideoInfo video;

		video.m_Width = max( rt.Width(), 1);
		video.m_Height = max( rt.Height(), 1);
		video.m_bFullScreen = FALSE;
		video.m_bMultiThread = TRUE;

		m_pViewer = i3Viewer::new_object();
		m_pViewer->Create( m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER);
	}

	return 0;
}

void i3TDKViewerCtrl::OnDestroy()
{
	I3_SAFE_RELEASE( m_pViewer);

	CWnd::OnDestroy();
}

void i3TDKViewerCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	SetFocus();
}

void i3TDKViewerCtrl::SuspendRender(void)
{
	if( m_pViewer != nullptr)
		m_pViewer->SetPendingUpdateEnable( true);
}

void i3TDKViewerCtrl::ResumeRender(void)
{
	if( m_pViewer != nullptr)
	{
		m_pViewer->SetPendingUpdateEnable( false);
		
		Invalidate();
	}
}
