// PaneSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "PaneSpec.h"
#include ".\panespec.h"


// CPaneSpec dialog

IMPLEMENT_DYNAMIC(CPaneSpec, i3TDKDialogContainer)
CPaneSpec::CPaneSpec(CWnd* pParent /*=NULL*/)
	: i3TDKDialogContainer(CPaneSpec::IDD, pParent)
{
}

CPaneSpec::~CPaneSpec()
{
}

void CPaneSpec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPaneSpec, i3TDKDialogContainer)
END_MESSAGE_MAP()


// CPaneSpec message handlers

BOOL CPaneSpec::OnInitDialog()
{
	i3TDKDialogContainer::OnInitDialog();

	{
		m_Spec_EditBox.Create( CSpec_EditBox::IDD, this);
		AddSpecDialog( i3UIEditBox::GetClassMeta(), &m_Spec_EditBox);

		m_Spec_Button.Create( CSpec_Button::IDD, this);
		AddSpecDialog( i3UIButton::GetClassMeta(), &m_Spec_Button);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
