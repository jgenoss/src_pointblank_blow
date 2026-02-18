// PanelLog.cpp : implementation file
//

#include "stdafx.h"
#include "i3LevelDesign.h"
#include "PanelLog.h"
#include ".\panellog.h"


// CPanelLog dialog

IMPLEMENT_DYNAMIC(CPanelLog, i3TDKDialogBase)
CPanelLog::CPanelLog(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelLog::IDD, pParent)
{
}

CPanelLog::~CPanelLog()
{
}

void CPanelLog::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelLog, i3TDKDialogBase)
END_MESSAGE_MAP()


// CPanelLog message handlers

void CPanelLog::OnOK()
{
}

void CPanelLog::OnCancel()
{
}
