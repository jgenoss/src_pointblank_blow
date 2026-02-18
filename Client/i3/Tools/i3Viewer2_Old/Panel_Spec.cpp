// Panel_Spec.cpp : implementation file
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "Panel_Spec.h"
#include ".\panel_spec.h"


// CPanel_Spec dialog

IMPLEMENT_DYNAMIC(CPanel_Spec, i3TDKSpecDialogContainer)
CPanel_Spec::CPanel_Spec(CWnd* pParent /*=NULL*/)
	: i3TDKSpecDialogContainer(CPanel_Spec::IDD, pParent)
{
}

CPanel_Spec::~CPanel_Spec()
{
}

void CPanel_Spec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKSpecDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanel_Spec, i3TDKSpecDialogContainer)
END_MESSAGE_MAP()


// CPanel_Spec message handlers

BOOL CPanel_Spec::OnInitDialog()
{
	i3TDKSpecDialogContainer::OnInitDialog();

	AddBuiltInSpecDialogs();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Spec::OnOK()
{
}

void CPanel_Spec::OnCancel()
{
}
