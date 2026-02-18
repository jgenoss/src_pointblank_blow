// PaneCommon.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackFileEditor.h"
#include "PaneCommon.h"


// CPaneCommon dialog

IMPLEMENT_DYNAMIC(CPaneCommon, CDialog)
CPaneCommon::CPaneCommon(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneCommon::IDD, pParent)
{
}

CPaneCommon::~CPaneCommon()
{
}

void CPaneCommon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_PATH, m_PathCtrl);
}


BEGIN_MESSAGE_MAP(CPaneCommon, CDialog)
END_MESSAGE_MAP()


// CPaneCommon message handlers
