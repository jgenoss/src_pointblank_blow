// DockSceneGraphWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DockSceneGraphWnd.h"

// cDockSceneGraphWnd
IMPLEMENT_DYNAMIC(cDockSceneGraphWnd, cDockBaseWnd)

cDockSceneGraphWnd::cDockSceneGraphWnd()
{

}

cDockSceneGraphWnd::~cDockSceneGraphWnd()
{
}


BEGIN_MESSAGE_MAP(cDockSceneGraphWnd, cDockBaseWnd)
	ON_WM_CREATE()
	 	ON_WM_SIZE()
	 	ON_WM_CONTEXTMENU()
	 	ON_WM_PAINT()
	 	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// cDockSceneGraphWnd 메시지 처리기입니다.
int cDockSceneGraphWnd::OnCreate(LPCREATESTRUCT lpCreateStrucmait)
{
	if (cDockBaseWnd::OnCreate(lpCreateStrucmait) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	m_SceneViewTree.Create(dwViewStyle,rectDummy, this, 2);
	// 	
	HTREEITEM hRoot = m_SceneViewTree.InsertItem(_T("FakeApp 클래스"), 0, 0);
	m_SceneViewTree.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	// 
	// 	HTREEITEM hClass = m_SceneViewTree.InsertItem(_T("CFakeAboutDlg"), 1, 1, hRoot);
	// 	m_SceneViewTree.InsertItem(_T("CFakeAboutDlg()"), 3, 3, hClass);
	// 
	m_SceneViewTree.Expand(hRoot, TVE_EXPAND);

	//g_pItemSCFramework->SetResetCamera(false);

// 	RECT rtTmp;
// 	GetClientRect(&rtTmp);
// 	g_pViewer->SetWeaponRect(rtTmp, E_SWAPCHAIN_ITEM);

// 	CRect rc;
// 	rc.SetRectEmpty();
// 	m_static.Create( "", WS_VISIBLE  | WS_CHILD, rc, this, 0 );

	return 0;
}

void cDockSceneGraphWnd::OnPaint()
{
	cDockBaseWnd::OnPaint();
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_SceneViewTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}
void cDockSceneGraphWnd::OnSize(UINT nType, int cx, int cy)
{
	cDockBaseWnd::OnSize(nType, cx, cy);

	//if( m_static.GetSafeHwnd() )
//		m_static.SetWindowPos( NULL, 0, 0, cx, cy, 0 );
 
//  	RECT rtTmp;
//  	GetClientRect(&rtTmp);
//  	g_pViewer->SetWeaponRect(rtTmp, E_SWAPCHAIN_ITEM);
//  	
	if (GetSafeHwnd() == NULL)
 	{
 		return;
 	}
 
 	CRect rectClient;
 	GetClientRect(rectClient);
 
	m_SceneViewTree.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

 	if (m_BaseToolBar)
 	{
 		//int cyTlb = m_BaseToolBar.CalcFixedLayout(FALSE, TRUE).cy;
 
 		//m_BaseToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
 		
 	}
}

void cDockSceneGraphWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{

}

void cDockSceneGraphWnd::OnSetFocus(CWnd* pOldWnd)
{
	//CWnd::OnSetFocus(pOldWnd);
	 
 	
}