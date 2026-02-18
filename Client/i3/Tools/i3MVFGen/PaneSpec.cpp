// PaneSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3MVFGen.h"
#include "PaneSpec.h"


// CPaneSpec dialog

IMPLEMENT_DYNAMIC(CPaneSpec, CDialog)
CPaneSpec::CPaneSpec(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneSpec::IDD, pParent)
{
}

CPaneSpec::~CPaneSpec()
{
}

void CPaneSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneSpec, CDialog)
END_MESSAGE_MAP()


// CPaneSpec message handlers
