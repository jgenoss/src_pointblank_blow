// DebugPanel.cpp : implementation file
//

#include "stdafx.h"
#include <psapi.h>

#if defined( I3_DEBUG)
#include "DlgGameGraph.h"
#include "GlobalVar.h"

// CDlgGameGraph dialog

IMPLEMENT_DYNAMIC(CDlgGameGraph, CDialog)

CDlgGameGraph::CDlgGameGraph(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGameGraph::IDD, pParent)
{
}

CDlgGameGraph::~CDlgGameGraph()
{
}

void CDlgGameGraph::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgGameGraph, CDialog)
END_MESSAGE_MAP()


// CDlgGameGraph message handlers

void CDlgGameGraph::UpdateAll(void)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();

	m_Ctrl.setGameGraph( pFramework);
}


BOOL CDlgGameGraph::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		GetClientRect( &rt);

		m_Ctrl.Create( WS_VISIBLE | WS_CHILD, rt, this, 1000);
	}

	UpdateAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGameGraph::PlaceCtrls( INT32 cx, INT32 cy)
{
	if( ::IsWindow( m_Ctrl.m_hWnd))
	{
		m_Ctrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


#endif
