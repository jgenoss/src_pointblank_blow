// PaneState.cpp : implementation file
//

#include "stdafx.h"
#include "i3RSCGen.h"
#include "PaneState.h"
#include "GlobalVar.h"
#include ".\panestate.h"

// CPaneState dialog

IMPLEMENT_DYNAMIC(CPaneState, CDialog)
CPaneState::CPaneState(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneState::IDD, pParent)
{
	g_pState			= this;
}

CPaneState::~CPaneState()
{
	g_pState = NULL;
}

void CPaneState::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_ctrStateLog);
}

BEGIN_MESSAGE_MAP(CPaneState, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CPaneState message handlers
void CPaneState::ClearLog( void)
{
	m_ctrStateLog.ClearText();
}

static char szTemp[4096];

void CPaneState::Log( char * pszMsg)
{
	m_ctrStateLog.AddText( pszMsg);
}

void CPaneState::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ctrStateLog))
	{
		m_ctrStateLog.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

BOOL CPaneState::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPaneState::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
}

void CPaneState::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
}

int CPaneState::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CRect rt;
	GetClientRect( &rt);

	{
		rt.left		+= 3;
		rt.right	-= 6;
		rt.top		+= 3;
		rt.bottom	-= 6;
	}

	m_ctrStateLog.Create( WS_CHILD | WS_VISIBLE | WS_BORDER, rt, this, 0);

	return 0;
}
