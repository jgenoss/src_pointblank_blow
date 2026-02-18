// MVFGenDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3MVFGen.h"
#include "MVFGenDialog.h"


// MVFGenDialog dialog

IMPLEMENT_DYNAMIC(MVFGenDialog, CDialog)
MVFGenDialog::MVFGenDialog(CWnd* pParent /*=NULL*/)
	: CDialog(MVFGenDialog::IDD, pParent)
	, m_AlignUnit(0)
	, m_bCompress(FALSE)
{
}

MVFGenDialog::~MVFGenDialog()
{
}

void MVFGenDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_ALIGNUNIT, m_AlignUnit);
	DDX_Check(pDX, IDC_CHK_COMPRESS, m_bCompress);
}


BEGIN_MESSAGE_MAP(MVFGenDialog, CDialog)
END_MESSAGE_MAP()


// MVFGenDialog message handlers
