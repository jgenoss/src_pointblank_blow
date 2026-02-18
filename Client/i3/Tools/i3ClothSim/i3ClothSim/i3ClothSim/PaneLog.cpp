// PaneLog.cpp : implementation file
//

#include "stdafx.h"
#include "i3ClothSim.h"
#include "PaneLog.h"
#include ".\panelog.h"


// CPaneLog dialog

IMPLEMENT_DYNAMIC(CPaneLog, i3TDKDialogBase)
CPaneLog::CPaneLog(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPaneLog::IDD, pParent)
{
}

CPaneLog::~CPaneLog()
{
}

void CPaneLog::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneLog, i3TDKDialogBase)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CPaneLog message handlers

int CPaneLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDialogBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText( "Log");

	return 0;
}
