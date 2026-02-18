// EditDlgAction_MoveTo.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "EditDlgAction_MoveTo.h"
#include ".\editdlgaction_moveto.h"


// CEditDlgAction_MoveTo dialog

IMPLEMENT_DYNAMIC(CEditDlgAction_MoveTo, CDialog)
CEditDlgAction_MoveTo::CEditDlgAction_MoveTo(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDlgAction_MoveTo::IDD, pParent)
{
	m_pData = NULL;
}

CEditDlgAction_MoveTo::~CEditDlgAction_MoveTo()
{
	m_pData = NULL;
}

void CEditDlgAction_MoveTo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_MOVE_SELFACTOR, m_cbSelFactor);
	DDX_Control(pDX, IDC_ED_MOVE_DURATION, m_edDuration);
	DDX_Control(pDX, IDC_ED_MOVE_TARGETPOSX, m_edPosX);
	DDX_Control(pDX, IDC_ED_MOVE_TARGETPOSY, m_edPosY);
}


BEGIN_MESSAGE_MAP(CEditDlgAction_MoveTo, CDialog)
END_MESSAGE_MAP()


// CEditDlgAction_MoveTo message handlers
void CEditDlgAction_MoveTo::SetMoveEventData( I3UI_EVT_MOVE_DATA * pMove)
{
	m_pData = pMove;
}

BOOL CEditDlgAction_MoveTo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	I3ASSERT( m_pData != NULL);

	{
		char conv[MAX_PATH];

		m_cbSelFactor.AddString("F_TYPE_NORMAL");
		m_cbSelFactor.AddString("F_TYPE_ACCEL");
		m_cbSelFactor.SetCurSel( m_pData->m_nFactorType);

		sprintf( conv, "%f", m_pData->m_fCountTime);
		m_edDuration.SetWindowText( conv);

		sprintf( conv, "%f", m_pData->m_fDestPosX);
		m_edPosX.SetWindowText( conv);

		sprintf( conv, "%f", m_pData->m_fDestPosY);
		m_edPosY.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditDlgAction_MoveTo::OnOK()
{
	I3ASSERT( m_pData != NULL);

	char conv[MAX_PATH];

	UpdateData( TRUE);
	// TODO: Add your specialized code here and/or call the base class
	m_pData->m_nFactorType = (INT32)m_cbSelFactor.GetCurSel();

	if( m_edDuration.GetModify())
	{
		m_edDuration.GetWindowText( conv, MAX_PATH -1 );

		m_pData->m_fCountTime = (RT_REAL32)atof( conv);
	}

	if( m_edPosX.GetModify())
	{
		m_edPosX.GetWindowText( conv, MAX_PATH -1 );

		m_pData->m_fDestPosX = (RT_REAL32)atof( conv);
	}

	if( m_edPosY.GetModify())
	{
		m_edPosY.GetWindowText( conv, MAX_PATH -1 );

		m_pData->m_fDestPosY = (RT_REAL32)atof( conv);
	}

	CDialog::OnOK();
}
