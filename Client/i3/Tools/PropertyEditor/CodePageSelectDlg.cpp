// ExportCodePage.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyEditor.h"
#include "GlobalVariable.h"
#include ".\CodePageSelectDlg.h"


// CCodePageSelectDlg dialog

IMPLEMENT_DYNAMIC(CCodePageSelectDlg, CDialog)
CCodePageSelectDlg::CCodePageSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCodePageSelectDlg::IDD, pParent),
	m_nCP(0)
{	
}

CCodePageSelectDlg::~CCodePageSelectDlg()
{
}

void CCodePageSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_EXPORT_CP, m_CodePage);
}


BEGIN_MESSAGE_MAP(CCodePageSelectDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_EXPORT_CP, OnCbnSelchangeComboExportCp)
END_MESSAGE_MAP()


// CCodePageSelectDlg message handlers

BOOL CCodePageSelectDlg::OnInitDialog()
{
	BOOL result = CDialog::OnInitDialog();

	if (!result)
	{
		return FALSE;
	}

	//	콤보박스에 언어 추가
	m_CodePage.Clear();
	for(INT32 i=0; i< GetCodePageCount(); i++)
	{
		char szItem[256] = "";
		UINT32 code = FindCodePageByIdx( i);
		GetCodePageName( code, szItem);

		m_CodePage.AddString( szItem );		
	}

	//	현재 선택되어 있는 언어 미리 선택
	INT32 idx = FindIdxByCodePage( GetSelCodePage());

	if( idx != -1)
	{
		m_CodePage.SetCurSel( idx);

		m_nCP = FindCodePageByIdx( idx);
	}

	UpdateData(TRUE);

	return TRUE;
}

void CCodePageSelectDlg::OnCbnSelchangeComboExportCp()
{
	// TODO: Add your control notification handler code here
	INT32 cur = m_CodePage.GetCurSel();
	INT32 codepage = FindCodePageByIdx( cur);

	m_nCP = codepage;
}
