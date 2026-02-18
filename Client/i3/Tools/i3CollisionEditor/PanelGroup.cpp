// PanelGroup.cpp : implementation file
//

#include "stdafx.h"
#include "i3CollisionEditor.h"
#include "PanelGroup.h"
#include ".\panelgroup.h"


// CPanelGroup dialog

IMPLEMENT_DYNAMIC(CPanelGroup, CDialog)
CPanelGroup::CPanelGroup(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelGroup::IDD, pParent)
{
}

CPanelGroup::~CPanelGroup()
{
}

void CPanelGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUPLIST, m_GroupCtrl);
}

void CPanelGroup::UpdateGroup( INT32 idx, ColliderGroup * pGroup)
{
}

BEGIN_MESSAGE_MAP(CPanelGroup, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanelGroup message handlers

BOOL CPanelGroup::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_GroupCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_GroupCtrl.InsertColumn( 0, "ID", LVCFMT_LEFT, 70);
	}

	i3TDK::RegisterUpdate( m_hWnd, ColliderGroup::GetClassMeta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelGroup::OnCancel()
{
}

void CPanelGroup::OnOK()
{
}

void CPanelGroup::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_GroupCtrl.m_hWnd))
	{
		m_GroupCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelGroup::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
	{
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
	}
}

LRESULT CPanelGroup::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

