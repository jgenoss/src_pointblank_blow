// ViewTree.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "ViewTree.h"


// cViewTree

IMPLEMENT_DYNAMIC(cViewTree, CWnd)

cViewTree::cViewTree()
{

}

cViewTree::~cViewTree()
{
}


BEGIN_MESSAGE_MAP(cViewTree, CWnd)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void cViewTree::OnSetFocus(CWnd* pOldWnd)
{
	CTreeCtrl::OnSetFocus(pOldWnd);

	if (g_pItemSCFramework)
	{
		g_pViewer->SetFramework(g_pItemSCFramework);
		g_pViewer->SetWndProc(GetSafeHwnd(), false);
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void cViewTree::OnSize(UINT nType, int cx, int cy)
{
	CTreeCtrl::OnSize(nType, cx, cy);

	RECT rtTmp;
	GetClientRect(&rtTmp);
	g_pViewer->SetWeaponRect(rtTmp, E_SWAPCHAIN_ITEM);

}


// cViewTree 메시지 처리기입니다.


