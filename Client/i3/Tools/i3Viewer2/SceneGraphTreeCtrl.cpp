// SceneGraphTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SceneGraphTreeCtrl.h"
#include "GlobalVar.h"

// CSceneGraphTreeCtrl

IMPLEMENT_DYNAMIC(CSceneGraphTreeCtrl, i3TDKSceneGraphTreeCtrl)
CSceneGraphTreeCtrl::CSceneGraphTreeCtrl()
{
}

CSceneGraphTreeCtrl::~CSceneGraphTreeCtrl()
{
}


void CSceneGraphTreeCtrl::OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew)
{
}

void CSceneGraphTreeCtrl::OnSetSceneGraph(void)
{
	g_pFramework->RefineSg( false);
}

BEGIN_MESSAGE_MAP(CSceneGraphTreeCtrl, i3TDKSceneGraphTreeCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CSceneGraphTreeCtrl message handlers

