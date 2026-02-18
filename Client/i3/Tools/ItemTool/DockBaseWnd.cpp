// cDockBaseWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DockBaseWnd.h"


// cDockBaseWnd
//IMPLEMENT_DYNAMIC(cDockBaseWnd, CDockablePane)

cDockBaseWnd::cDockBaseWnd()
{

}

cDockBaseWnd::~cDockBaseWnd()
{
}


BEGIN_MESSAGE_MAP(cDockBaseWnd, CDockablePane)
	ON_WM_CREATE()
END_MESSAGE_MAP()
 
void cDockBaseWnd::LoadToolBar(UINT resID)
{
	m_BaseToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, resID);
//	m_BaseToolBar.LoadToolBar(resID, 0, 0, TRUE /* 잠금 */);

	//OnChangeVisualStyle();
	//m_BaseToolBar.SetPaneStyle(m_BaseToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	//m_BaseToolBar.SetPaneStyle(m_BaseToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_BaseToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_BaseToolBar.SetRouteCommandsViaFrame(FALSE);
}

// cDockBaseWnd 메시지 처리기입니다.
int cDockBaseWnd::OnCreate(LPCREATESTRUCT lpCreateStrucmait)
{
	if (CDockablePane::OnCreate(lpCreateStrucmait) == -1)
		return -1;

	return 0;
}
