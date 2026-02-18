// EditDlgAction_RotateTo.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "EditDlgAction_RotateTo.h"
#include ".\editdlgaction_rotateto.h"


// CEditDlgAction_RotateTo dialog

IMPLEMENT_DYNAMIC(CEditDlgAction_RotateTo, CDialog)
CEditDlgAction_RotateTo::CEditDlgAction_RotateTo(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDlgAction_RotateTo::IDD, pParent)
{
	m_pData = NULL;
}

CEditDlgAction_RotateTo::~CEditDlgAction_RotateTo()
{
	m_pData = NULL;
}

void CEditDlgAction_RotateTo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_ROTATE_SELFACTOR, m_cbSelFactor);
	DDX_Control(pDX, IDC_CB_ROTATE_SELSPINDIR, m_cbSelSpinDir);
	DDX_Control(pDX, IDC_ED_ROTATE_DURATION, m_edDuration);
	DDX_Control(pDX, IDC_CHECK_LOOPROTATE, m_checkLoopRotate);
}

void CEditDlgAction_RotateTo::SetRotateEventData( I3GUI_EVENT_ROTATE_DATA * pData)
{
	m_pData = pData;
}

BEGIN_MESSAGE_MAP(CEditDlgAction_RotateTo, CDialog)
END_MESSAGE_MAP()


// CEditDlgAction_RotateTo message handlers

BOOL CEditDlgAction_RotateTo::OnInitDialog()
{
	CDialog::OnInitDialog();

	I3ASSERT( m_pData != NULL);

	char conv[MAX_PATH];
	// TODO:  Add extra initialization here

	{
		m_cbSelFactor.AddString( "TYPE_NORMAL");
		m_cbSelFactor.AddString( "TYPE_ACCEL");
		m_cbSelFactor.SetCurSel( m_pData->m_nFactorType);

		m_cbSelSpinDir.AddString( "CW");
		m_cbSelSpinDir.AddString( "CCW");
		m_cbSelSpinDir.SetCurSel( m_pData->m_nSpinDirection);

		sprintf( conv, "%f", m_pData->m_fCountTime);
		m_edDuration.SetWindowText( conv);

		if( m_pData->m_bLoopRotate)
		{
			m_checkLoopRotate.SetCheck( BST_CHECKED);
		}
		else
		{	
			m_checkLoopRotate.SetCheck( BST_UNCHECKED);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditDlgAction_RotateTo::OnOK()
{
	I3ASSERT( m_pData != NULL);

	// TODO: Add your specialized code here and/or call the base class
	char conv[MAX_PATH];

	UpdateData( TRUE);

	m_pData->m_nFactorType		= m_cbSelFactor.GetCurSel();
	m_pData->m_nSpinDirection	= m_cbSelSpinDir.GetCurSel();

	if( m_edDuration.GetModify())
	{
		m_edDuration.GetWindowText( conv, MAX_PATH -1);
		m_pData->m_fCountTime = (REAL32) atof( conv);
	}

	if( m_checkLoopRotate.GetCheck() == BST_CHECKED)
	{
		m_pData->m_bLoopRotate = TRUE;
	}
	else
	{
		m_pData->m_bLoopRotate = FALSE;
	}

	CDialog::OnOK();
}
