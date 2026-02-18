// SceneGraphWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "SceneGraphWnd.h"


// cSceneGraphWnd

IMPLEMENT_DYNAMIC(cSceneGraphWnd, CDockablePane)

cSceneGraphWnd::cSceneGraphWnd()
{

}

cSceneGraphWnd::~cSceneGraphWnd()
{
}


BEGIN_MESSAGE_MAP(cSceneGraphWnd, CDockablePane)
END_MESSAGE_MAP()



// cSceneGraphWnd 메시지 처리기입니다.
int cSceneGraphWnd::OnCreate(LPCREATESTRUCT lpCreateStrucmait)
{
	if (CDockablePane::OnCreate(lpCreateStrucmait) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	

	m_SceneToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_MAINFRAME);
	m_SceneToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, TRUE /* 잠금 */);

	//OnChangeVisualStyle();

	m_SceneToolBar.SetPaneStyle(m_SceneToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_SceneToolBar.SetPaneStyle(m_SceneToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_SceneToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_SceneToolBar.SetRouteCommandsViaFrame(FALSE);

	//AdjustLayout();

	return 0;
}
