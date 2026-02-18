// SelectBoxFaceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "SelectBoxFaceDialog.h"
#include ".\selectboxfacedialog.h"


// CSelectBoxFaceDialog dialog

IMPLEMENT_DYNAMIC(CSelectBoxFaceDialog, CDialog)
CSelectBoxFaceDialog::CSelectBoxFaceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectBoxFaceDialog::IDD, pParent)
	, m_bTop(FALSE)
	, m_bLeft(FALSE)
	, m_bFront(FALSE)
	, m_bBack(FALSE)
	, m_bBottom(FALSE)
	, m_bRight(FALSE)
{
}

CSelectBoxFaceDialog::~CSelectBoxFaceDialog()
{
}

void CSelectBoxFaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_TOP, m_bTop);
	DDX_Check(pDX, IDC_CHK_LEFT, m_bLeft);
	DDX_Check(pDX, IDC_CHK_FRONT, m_bFront);
	DDX_Check(pDX, IDC_CHK_BACK, m_bBack);
	DDX_Check(pDX, IDC_CHK_BOTTOM, m_bBottom);
	DDX_Check(pDX, IDC_CHK_RIGHT, m_bRight);
}


BEGIN_MESSAGE_MAP(CSelectBoxFaceDialog, CDialog)
END_MESSAGE_MAP()


// CSelectBoxFaceDialog message handlers

BOOL CSelectBoxFaceDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bLeft = TRUE;
	m_bRight = TRUE;
	m_bFront = TRUE;
	m_bBack = TRUE;
	m_bTop = TRUE;
	m_bBottom = TRUE;

	UpdateData( FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectBoxFaceDialog::OnOK()
{
	UpdateData( TRUE);

	CDialog::OnOK();
}
