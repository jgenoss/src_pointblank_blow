#include "stdafx.h"
#include "i3Viewer2.h"
#include "Panel_Spec.h"


// Panel_Spec dialog

IMPLEMENT_DYNAMIC(Panel_Spec, i3TDKSpecDialogContainer)

Panel_Spec::Panel_Spec(CWnd* pParent /*=NULL*/)
	: i3TDKSpecDialogContainer(Panel_Spec::IDD, pParent)
{

}

Panel_Spec::~Panel_Spec()
{
}

void Panel_Spec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKSpecDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Panel_Spec, i3TDKSpecDialogContainer)
END_MESSAGE_MAP()


// Panel_Spec message handlers

BOOL Panel_Spec::OnInitDialog()
{
	i3TDKSpecDialogContainer::OnInitDialog();

	i3TDK::SetResInstance();

	AddBuiltInSpecDialogs();

	i3TDK::RestoreResInstance();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Panel_Spec::OnOK()
{
}

void Panel_Spec::OnCancel()
{
}
