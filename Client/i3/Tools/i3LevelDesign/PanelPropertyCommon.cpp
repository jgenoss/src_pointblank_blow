// PanelPropertyCommon.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "PanelPropertyCommon.h"

// CPanelPropertyCommon dialog

IMPLEMENT_DYNAMIC(CPanelPropertyCommon, i3TDKDialogContainer)
CPanelPropertyCommon::CPanelPropertyCommon(CWnd* pParent /*=NULL*/)
	: i3TDKDialogContainer(CPanelPropertyCommon::IDD, pParent)
{
}

CPanelPropertyCommon::~CPanelPropertyCommon()
{
}

void CPanelPropertyCommon::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogContainer::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelPropertyCommon, i3TDKDialogContainer)
END_MESSAGE_MAP()


// CPanelPropertyCommon message handlers

BOOL CPanelPropertyCommon::OnInitDialog()
{
	i3TDKDialogContainer::OnInitDialog();

	{
		m_Prop_Elm_Common.Create( CProp_LvElm_Common::IDD, this);
		AddSpecDialog( i3LevelElement3D::static_meta(), &m_Prop_Elm_Common);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelPropertyCommon::OnCancel()
{
}

void CPanelPropertyCommon::OnOK()
{
}
