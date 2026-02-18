// PanelToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "i3CollisionEditor.h"
#include "PanelToolbar.h"


// CPanelToolbar dialog

IMPLEMENT_DYNAMIC(CPanelToolbar, CDialog)
CPanelToolbar::CPanelToolbar(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelToolbar::IDD, pParent)
{
}

CPanelToolbar::~CPanelToolbar()
{
}

void CPanelToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelToolbar, CDialog)
END_MESSAGE_MAP()


// CPanelToolbar message handlers
