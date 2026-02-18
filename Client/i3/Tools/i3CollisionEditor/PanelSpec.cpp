// PanelSpec.cpp : implementation file
//

#include "stdafx.h"
#include "i3CollisionEditor.h"
#include "PanelSpec.h"
#include "Collidee.h"


// CPanelSpec dialog

IMPLEMENT_DYNAMIC(CPanelSpec, i3TDKDialogContainer)
CPanelSpec::CPanelSpec(CWnd* pParent /*=NULL*/)
	: i3TDKDialogContainer(CPanelSpec::IDD, pParent)
{
}

CPanelSpec::~CPanelSpec()
{
}

void CPanelSpec::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelSpec, i3TDKDialogContainer)
END_MESSAGE_MAP()


// CPanelSpec message handlers

BOOL CPanelSpec::OnInitDialog()
{
	i3TDKDialogContainer::OnInitDialog();

	{
		m_SpecTriangle.Create( CSpecTriangle::IDD, this);
		AddSpecDialog( Collidee::GetClassMeta(), &m_SpecTriangle);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
