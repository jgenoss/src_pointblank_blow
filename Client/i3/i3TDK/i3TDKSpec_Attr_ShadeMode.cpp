// i3TDKSpec_Attr_ShadeMode.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_ShadeMode.h"
#include ".\i3tdkspec_attr_shademode.h"


// i3TDKSpec_Attr_ShadeMode dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_ShadeMode, i3TDKDialogBase)

i3TDKSpec_Attr_ShadeMode::~i3TDKSpec_Attr_ShadeMode()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_ShadeMode::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_MODE, m_CB_Mode);
}

void i3TDKSpec_Attr_ShadeMode::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3ShadeModeAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		switch( m_pAttr->Get())
		{
			case I3G_SHADE_GOURAUD		:	m_CB_Mode.SetCurSel( 0);	break;
			case I3G_SHADE_CUSTOM		:	m_CB_Mode.SetCurSel( 2);	break;
			case I3G_SHADE_HEMISPHERE	:	m_CB_Mode.SetCurSel( 3);	break;
			case I3G_SHADE_GI1			:	m_CB_Mode.SetCurSel( 4);	break;
			default :						m_CB_Mode.SetCurSel( 1);	break;
		}
	}
	else
	{
		m_CB_Mode.SetCurSel( -1);
	}

	m_CB_Mode.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_ShadeMode, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_MODE, OnCbnSelchangeTdkCbMode)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_ShadeMode message handlers

BOOL i3TDKSpec_Attr_ShadeMode::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_ShadeMode::OnOK()
{
}

void i3TDKSpec_Attr_ShadeMode::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_ShadeMode::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_ShadeMode::OnCbnSelchangeTdkCbMode()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_Mode.GetCurSel())
	{
		case 0 :	m_pAttr->Set( I3G_SHADE_GOURAUD);	break;
		case 1 :	m_pAttr->Set( I3G_SHADE_PHONG);	break;
		case 2 :	m_pAttr->Set( I3G_SHADE_CUSTOM);	break;
		case 3 :	m_pAttr->Set( I3G_SHADE_HEMISPHERE);	break;
		case 4 :	m_pAttr->Set( I3G_SHADE_GI1);	break;
	}
}
