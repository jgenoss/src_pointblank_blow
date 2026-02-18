// CEditDlgAction_ScaleTo.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "EditDlgAction_ScaleTo.h"

// CEditDlgAction_ScaleTo dialog

IMPLEMENT_DYNAMIC(CEditDlgAction_ScaleTo, CDialog)
CEditDlgAction_ScaleTo::CEditDlgAction_ScaleTo(CWnd* pParent /*=NULL*/)
	: CDialog(CEditDlgAction_ScaleTo::IDD, pParent)
{
	m_pData = NULL;
}

CEditDlgAction_ScaleTo::~CEditDlgAction_ScaleTo()
{
	m_pData = NULL;
}

void CEditDlgAction_ScaleTo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_SCALE_SELFACTOR, m_cbSelFactor);
	DDX_Control(pDX, IDC_ED_SCALE_DURATION, m_edDuration);
	DDX_Control(pDX, IDC_ED_SCALE_X, m_edScaleX);
	DDX_Control(pDX, IDC_ED_SCALE_Y, m_edScaleY);
	DDX_Control(pDX, IDC_CHECK_LOOPSCALE, m_chkLoop);
}


BEGIN_MESSAGE_MAP(CEditDlgAction_ScaleTo, CDialog)
END_MESSAGE_MAP()


// CEditDlgAction_ScaleTo message handlers
void CEditDlgAction_ScaleTo::SetScaleEventData( I3GUI_EVENT_SCALE_DATA * pData)
{
	m_pData = pData;
}


BOOL CEditDlgAction_ScaleTo::OnInitDialog()
{
	CDialog::OnInitDialog();

	I3ASSERT( m_pData != NULL);

	char conv[MAX_PATH];
	// TODO:  Add extra initialization here

	{
		m_cbSelFactor.AddString( "TYPE_NORMAL");
		m_cbSelFactor.AddString( "TYPE_ACCEL");
		m_cbSelFactor.SetCurSel( m_pData->m_nFactorType);

		sprintf( conv, "%f", m_pData->m_fCountTime);
		m_edDuration.SetWindowText( conv);

		sprintf( conv, "%f", m_pData->m_fDestScaleX);
		m_edScaleX.SetWindowText( conv);

		sprintf( conv, "%f", m_pData->m_fDestScaleY);
		m_edScaleY.SetWindowText( conv);

		if( m_pData->m_bLoopScale )
			m_chkLoop.SetCheck( BST_CHECKED);
		else
			m_chkLoop.SetCheck( BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditDlgAction_ScaleTo::OnOK()
{
	I3ASSERT( m_pData != NULL);

	char conv[MAX_PATH];

	// TODO: Add your specialized code here and/or call the base class
	UpdateData( TRUE);
	{
		m_pData->m_nFactorType = m_cbSelFactor.GetCurSel();

		if( m_edDuration.GetModify())
		{
			m_edDuration.GetWindowText( conv, MAX_PATH -1);

			m_pData->m_fCountTime = (REAL32)atof( conv);
		}

		if( m_edDuration.GetModify())
		{
			m_edScaleX.GetWindowText( conv, MAX_PATH -1);

			m_pData->m_fDestScaleX = (REAL32)atof( conv);
		}

		if( m_edDuration.GetModify())
		{
			m_edScaleY.GetWindowText( conv, MAX_PATH -1);

			m_pData->m_fDestScaleY = (REAL32)atof( conv);
		}

		if( m_chkLoop.GetCheck() == BST_CHECKED)
		{
			m_pData->m_bLoopScale = TRUE;
		}
		else
			m_pData->m_bLoopScale = FALSE;
	}

	CDialog::OnOK();
}
