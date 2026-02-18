#include "StdAfx.h"
#include "ItemToolViewerCtrl.h"
#include "ItemToolViewer.h"

IMPLEMENT_DYNAMIC(cItemToolViewerCtrl, i3TDKViewerCtrl)

cItemToolViewerCtrl::cItemToolViewerCtrl(void)
{
}

cItemToolViewerCtrl::~cItemToolViewerCtrl(void)
{
}

BEGIN_MESSAGE_MAP(cItemToolViewerCtrl, i3TDKViewerCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

int cItemToolViewerCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
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


		m_pViewer = cItemToolViewer::new_object();
		((cItemToolViewer*)m_pViewer)->Create( m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER);
	}

	return 0;
}

void cItemToolViewerCtrl::OnDestroy()
{
	//I3_SAFE_RELEASE( m_pViewer);

	CWnd::OnDestroy();
}

void cItemToolViewerCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	if (g_pMainSCFramework)
	{
		g_pViewer->SetFramework(g_pMainSCFramework);
		g_pViewer->SetWndProc(GetSafeHwnd());
	}
}

void cItemToolViewerCtrl::OnSize(UINT nType, int cx, int cy)
{
	i3TDKViewerCtrl::OnSize(nType, cx, cy);


	RECT rtTmp;
	GetClientRect(&rtTmp);
	//rtTmp.right = g_pViewer->GetMaxW();
	//rtTmp.bottom = g_pViewer->GetMaxH();

	g_pViewer->SetWeaponRect(rtTmp, E_SWAPCHAIN_MAIN);

	POINT2D sz;

	sz.x = cx;
	sz.y = cy;

	//g_pMainSCFramework->OnSize(&sz);

 	//g_pViewer->SetFramework(g_pMainSCFramework);
 	//g_pViewer->GetRenderContext()->SetSkipRender( FALSE);
 	//g_pViewer->Resize( (UINT32)sz.x, (UINT32)sz.y);



	//g_pViewer->GetRenderContext()->_ValidateGfxResource();

	//g_pMainSCFramework->OnSize(&sz);
	//g_pMainSCFramework->OnUpdateCameraProjection();
}