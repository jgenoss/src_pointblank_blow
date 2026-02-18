// DockAttributeWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DockAttributeWnd.h"


// cDockAttributeWnd
IMPLEMENT_DYNAMIC(cDockAttributeWnd, cDockBaseWnd)

cDockAttributeWnd::cDockAttributeWnd()
{

}

cDockAttributeWnd::~cDockAttributeWnd()
{
}


BEGIN_MESSAGE_MAP(cDockAttributeWnd, cDockBaseWnd)
	ON_WM_SETFOCUS()
	ON_WM_NCRBUTTONDOWN()
END_MESSAGE_MAP()





void cDockAttributeWnd::OnSetFocus(CWnd* pOldWnd)
{
	cDockBaseWnd::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void cDockAttributeWnd::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	cDockBaseWnd::OnNcRButtonDown(nHitTest, point);
}
