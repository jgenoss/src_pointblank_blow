// i3TDKSpec_Attr_Bool.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_Bool.h"
#include ".\i3tdkspec_attr_bool.h"


// i3TDKSpec_Attr_Bool dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_Bool, i3TDKDialogBase)

i3TDKSpec_Attr_Bool::~i3TDKSpec_Attr_Bool()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_Bool::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CHK_ENABLE, m_CHK_Enable);
}

void i3TDKSpec_Attr_Bool::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3AbstractBoolAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		if( m_pAttr->Get())
			m_CHK_Enable.SetCheck( BST_CHECKED);
		else
			m_CHK_Enable.SetCheck( BST_UNCHECKED);
	}
	else
	{
		m_CHK_Enable.SetCheck( BST_UNCHECKED);
	}

	m_CHK_Enable.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_Bool, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_TDK_CHK_ENABLE, OnBnClickedTdkChkEnable)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_Bool message handlers

BOOL i3TDKSpec_Attr_Bool::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_Bool::OnOK()
{
}

void i3TDKSpec_Attr_Bool::OnCancel()
{
}

void i3TDKSpec_Attr_Bool::OnBnClickedTdkChkEnable()
{
	if( m_pAttr == nullptr)
		return;

	if( m_CHK_Enable.GetCheck() == BST_CHECKED)
		m_pAttr->Set( TRUE);
	else
		m_pAttr->Set( FALSE);
}
