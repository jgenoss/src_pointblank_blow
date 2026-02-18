// DlgReplaceRes.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgReplaceRes.h"
#include ".\dlgreplaceres.h"


// CDlgReplaceRes dialog

IMPLEMENT_DYNAMIC(CDlgReplaceRes, CDialog)
CDlgReplaceRes::CDlgReplaceRes(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReplaceRes::IDD, pParent)
{
	m_pSrcRes = NULL;
	m_pDestRes = NULL;
}

CDlgReplaceRes::~CDlgReplaceRes()
{
}

void CDlgReplaceRes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_SRC, m_ED_Src);
	DDX_Control(pDX, IDC_ED_DEST, m_ED_Dest);
}

bool CDlgReplaceRes::Execute( i3LevelRes * pRes)
{
	m_pSrcRes = pRes;

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CDlgReplaceRes, CDialog)
END_MESSAGE_MAP()


// CDlgReplaceRes message handlers

BOOL CDlgReplaceRes::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_ED_Src.SetWindowText( m_pSrcRes->getPath());
		m_ED_Dest.SetWindowText( m_pSrcRes->getPath());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReplaceRes::OnOK()
{
	char conv[256];

	m_ED_Dest.GetWindowText( conv, sizeof(conv) - 1);

	m_pDestRes = g_pResDB->FindResByPath( conv);

	if( m_pDestRes == NULL)
	{
		MessageBox( "해당 이름의 Resource를 찾을 수 없습니다.", MB_OK);

		return;
	}

	CDialog::OnOK();
}
