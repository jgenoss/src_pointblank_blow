// PaneTrack.cpp : implementation file
//

#include "stdafx.h"
#include "i3AnimOptimizer.h"
#include "PaneTrack.h"


// CPaneTrack dialog

IMPLEMENT_DYNAMIC(CPaneTrack, CDialog)
CPaneTrack::CPaneTrack(CWnd* pParent /*=NULL*/)
	: CDialog(CPaneTrack::IDD, pParent)
{
}

CPaneTrack::~CPaneTrack()
{
}

void CPaneTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneTrack, CDialog)
END_MESSAGE_MAP()


// CPaneTrack message handlers
