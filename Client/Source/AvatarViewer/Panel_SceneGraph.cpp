// Panel_SceneGraph.cpp : implementation file
//

#include "pch.h"
#include "Panel_SceneGraph.h"


// Panel_SceneGraph

IMPLEMENT_DYNAMIC(Panel_SceneGraph, CDockablePane)

Panel_SceneGraph::Panel_SceneGraph()
{

}

Panel_SceneGraph::~Panel_SceneGraph()
{
}


BEGIN_MESSAGE_MAP(Panel_SceneGraph, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// Panel_SceneGraph message handlers



int Panel_SceneGraph::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		CRect rt;

		rt.SetRect( 0, 0, 10, 10);

		i3TDK::SetResInstance();
		m_SgCtrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 100);
		i3TDK::RestoreResInstance();
	}

	i3TDK::RegisterUpdate( m_hWnd, i3SceneGraphInfo::static_meta(), I3_TDK_UPDATE_ALL);

	return 0;
}

void Panel_SceneGraph::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if( ::IsWindow( m_SgCtrl.m_hWnd))
	{
		m_SgCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Panel_SceneGraph::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		//m_SgCtrl.setSgInfo( (i3SceneGraphInfo *) pInfo->m_pObject);	
	}
}

LRESULT Panel_SceneGraph::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDockablePane::WindowProc(message, wParam, lParam);
}
