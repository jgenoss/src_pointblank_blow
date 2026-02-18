// PanelSceneGraph.cpp : implementation file
//

#include "stdafx.h"
#include "i3ObjectEditor.h"
#include "PanelSceneGraph.h"
#include ".\panelscenegraph.h"


// CPanelSceneGraph dialog

IMPLEMENT_DYNAMIC(CPanelSceneGraph, i3TDKDialogBase)
CPanelSceneGraph::CPanelSceneGraph(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelSceneGraph::IDD, pParent)
{
}

CPanelSceneGraph::~CPanelSceneGraph()
{
}

void CPanelSceneGraph::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SGLIST, m_SgListCtrl);
}


BEGIN_MESSAGE_MAP(CPanelSceneGraph, i3TDKDialogBase)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelSceneGraph message handlers

BOOL CPanelSceneGraph::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_SgListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 150);
		m_SgListCtrl.InsertColumn( 1, "Physics", LVCFMT_LEFT, 60);
		m_SgListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelSceneGraph::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_SgListCtrl.m_hWnd))
	{
		m_SgListCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelSceneGraph::OnCancel(void)
{
}

void CPanelSceneGraph::OnOK(void)
{
}
