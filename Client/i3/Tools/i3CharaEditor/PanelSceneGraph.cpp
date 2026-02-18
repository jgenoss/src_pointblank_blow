// PanelSceneGraph.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelSceneGraph.h"
#include ".\panelscenegraph.h"


// CPanelSceneGraph dialog

IMPLEMENT_DYNAMIC(CPanelSceneGraph, i3TDKDialogBase)
CPanelSceneGraph::CPanelSceneGraph(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelSceneGraph::IDD, pParent)
{
	m_pSg = i3SceneGraphInfo::new_object();
}

CPanelSceneGraph::~CPanelSceneGraph()
{
	I3_SAFE_RELEASE( m_pSg);
}

void CPanelSceneGraph::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}

void CPanelSceneGraph::SetSceneGraph( i3Node * pNode)
{
	m_pSg->setInstanceSg( pNode);

	m_TreeCtrl.setSgInfo( m_pSg);
}

BEGIN_MESSAGE_MAP(CPanelSceneGraph, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelSceneGraph message handlers

BOOL CPanelSceneGraph::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		GetClientRect( &rt);

		rt.left += 3;
		rt.top += 3;
		rt.right -= 3;
		rt.bottom -= 3;

		m_TreeCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3Node::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, i3SceneGraphInfo::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, CLOD::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



void CPanelSceneGraph::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelSceneGraph::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_pObject != NULL)
		{
			if( i3::kind_of<i3SceneGraphInfo*>(pInfo->m_pObject)) //->IsTypeOf( i3SceneGraphInfo::static_meta()))
			{
				i3SceneGraphInfo * pSgInfo = (i3SceneGraphInfo *) pInfo->m_pObject;

				SetSceneGraph( pSgInfo->getInstanceSg());
			}
			else if( i3::kind_of<i3Node*>(pInfo->m_pObject)) //->IsTypeOf( i3Node::static_meta()))
			{
				SetSceneGraph( (i3Node *) pInfo->m_pObject);
			}
			else if( i3::kind_of<CLOD*>(pInfo->m_pObject)) //->IsTypeOf( CLOD::static_meta()))
			{
				CLOD * pLOD = (CLOD *) pInfo->m_pObject;

				SetSceneGraph( pLOD->getLOD()->getShapeNode());
			}
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_UNSELECT)
	{
		SetSceneGraph( NULL);
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		if( pInfo->m_pObject == NULL)
		{
			SetSceneGraph( NULL);
		}
		else if( pInfo->m_pObject == m_TreeCtrl.GetSceneGraph())
		{
			SetSceneGraph( NULL);
		}
	}
}

LRESULT CPanelSceneGraph::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}
