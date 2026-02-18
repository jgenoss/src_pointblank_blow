// PanelLog.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "PanelLog.h"


// CPanelLog dialog

IMPLEMENT_DYNAMIC(CPanelLog, CDialog)
CPanelLog::CPanelLog(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelLog::IDD, pParent)
{
}

CPanelLog::~CPanelLog()
{
}

void CPanelLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelLog, CDialog)
END_MESSAGE_MAP()


// CPanelLog message handlers
