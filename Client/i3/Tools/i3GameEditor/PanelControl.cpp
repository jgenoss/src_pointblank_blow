// PanelControl.cpp : implementation file
//

#include "stdafx.h"
#include "i3GameEditor.h"
#include "PanelControl.h"


// PanelControl dialog

IMPLEMENT_DYNAMIC(PanelControl, i3TDKDialogBase)
PanelControl::PanelControl(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(PanelControl::IDD, pParent)
{
}

PanelControl::~PanelControl()
{
}

void PanelControl::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PanelControl, i3TDKDialogBase)
END_MESSAGE_MAP()


// PanelControl message handlers
