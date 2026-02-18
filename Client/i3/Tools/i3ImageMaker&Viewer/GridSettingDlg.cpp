// GridSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3ImageViewer.h"
#include "GridSettingDlg.h"
#include ".\gridsettingdlg.h"


// CGridSettingDlg dialog

IMPLEMENT_DYNAMIC(CGridSettingDlg, CDialog)
CGridSettingDlg::CGridSettingDlg(CWnd* pParent /*=NULL*/, UINT32 VertBlockCount, UINT32 HoriCount, INT32 PenIndex)
	: CDialog(CGridSettingDlg::IDD, pParent)
	, m_VertBlockCount(VertBlockCount)
	, m_HoriBlockCount(HoriCount)
	, m_PenIndex(PenIndex)
{
}

CGridSettingDlg::~CGridSettingDlg()
{
}

void CGridSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VERT_COUNT, m_VertBlockCount);
	DDX_Text(pDX, IDC_EDIT_HORI_COUNT, m_HoriBlockCount);
}


BEGIN_MESSAGE_MAP(CGridSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGridSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGridSettingDlg message handlers

BOOL CGridSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	INT32	RadioIndex = IDC_RADIO_PEN_BLACK;
	switch(m_PenIndex)
	{
	case WHITE_PEN:	RadioIndex = IDC_RADIO_PEN_WHITE;	break;
	case BLACK_PEN:	RadioIndex = IDC_RADIO_PEN_BLACK;	break;
	}

	CheckRadioButton(IDC_RADIO_PEN_BLACK, IDC_RADIO_PEN_WHITE, RadioIndex);

	CWnd * pWnd = GetDlgItem(IDC_EDIT_VERT_COUNT);
	if ( pWnd )
		pWnd->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGridSettingDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	INT32 CheckedID = GetCheckedRadioButton(IDC_RADIO_PEN_BLACK, IDC_RADIO_PEN_WHITE);
	switch(CheckedID)
	{
	case IDC_RADIO_PEN_WHITE:	m_PenIndex = WHITE_PEN;	break;
	case IDC_RADIO_PEN_BLACK:	m_PenIndex = BLACK_PEN;	break;
	}

	CDialog::OnOK();
}

void CGridSettingDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
