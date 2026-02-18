// i3TDKSpec_Attr_TexWrap.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_TexWrap.h"
#include ".\i3tdkspec_attr_texwrap.h"


// i3TDKSpec_Attr_TexWrap dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_TexWrap, i3TDKDialogBase)

i3TDKSpec_Attr_TexWrap::~i3TDKSpec_Attr_TexWrap()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_TexWrap::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_HWRAP, m_CB_HWrap);
	DDX_Control(pDX, IDC_TDK_CB_VWRAP, m_CB_VWrap);
}

void i3TDKSpec_Attr_TexWrap::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3TextureWrapAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		switch( m_pAttr->GetHorzWrap())
		{
			case I3G_TEXTURE_WRAP_WRAP		:	m_CB_HWrap.SetCurSel( 0);	break;
			case I3G_TEXTURE_WRAP_CLAMP		:	m_CB_HWrap.SetCurSel( 1);	break;
			case I3G_TEXTURE_WRAP_BORDER	:	m_CB_HWrap.SetCurSel( 2);	break;
		}

		switch( m_pAttr->GetVertWrap())
		{
			case I3G_TEXTURE_WRAP_WRAP		:	m_CB_VWrap.SetCurSel( 0);	break;
			case I3G_TEXTURE_WRAP_CLAMP		:	m_CB_VWrap.SetCurSel( 1);	break;
			case I3G_TEXTURE_WRAP_BORDER	:	m_CB_VWrap.SetCurSel( 2);	break;
		}
	}

	m_CB_HWrap.EnableWindow( m_pAttr != nullptr);
	m_CB_VWrap.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_TexWrap, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_HWRAP, OnCbnSelchangeTdkCbHwrap)
	ON_CBN_SELCHANGE(IDC_TDK_CB_VWRAP, OnCbnSelchangeTdkCbVwrap)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_TexWrap message handlers

BOOL i3TDKSpec_Attr_TexWrap::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_TexWrap::OnOK()
{
}

void i3TDKSpec_Attr_TexWrap::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_TexWrap::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_TexWrap::OnCbnSelchangeTdkCbHwrap()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_HWrap.GetCurSel())
	{
		case 0 :	m_pAttr->SetHorzWrap( I3G_TEXTURE_WRAP_WRAP);	break;
		case 1 :	m_pAttr->SetHorzWrap( I3G_TEXTURE_WRAP_CLAMP);	break;
		case 2 :	m_pAttr->SetHorzWrap( I3G_TEXTURE_WRAP_BORDER);	break;
	}
}

void i3TDKSpec_Attr_TexWrap::OnCbnSelchangeTdkCbVwrap()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_VWrap.GetCurSel())
	{
		case 0 :	m_pAttr->SetVertWrap( I3G_TEXTURE_WRAP_WRAP);	break;
		case 1 :	m_pAttr->SetVertWrap( I3G_TEXTURE_WRAP_CLAMP);	break;
		case 2 :	m_pAttr->SetVertWrap( I3G_TEXTURE_WRAP_BORDER);	break;
	}
}
