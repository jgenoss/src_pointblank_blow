// i3TDKSpec_Attr_FogMode.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_FogMode.h"
#include ".\i3tdkspec_attr_fogmode.h"


// i3TDKSpec_Attr_FogMode dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_FogMode, i3TDKDialogBase)

i3TDKSpec_Attr_FogMode::~i3TDKSpec_Attr_FogMode()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_FogMode::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_MODE, m_CB_Mode);
}

void i3TDKSpec_Attr_FogMode::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3FogModeAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		INT32 idx = -1;

		switch( m_pAttr->Get())
		{
			case I3G_FOG_NONE :		idx = 0;	break;
			case I3G_FOG_LINEAR :	idx = 1;	break;
			case I3G_FOG_EXP :		idx = 2;	break;
			case I3G_FOG_EXP2 :		idx = 3;	break;
		}

		m_CB_Mode.SetCurSel( idx);
	}
	else
	{
		m_CB_Mode.SetCurSel( -1);
	}
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_FogMode, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_MODE, OnCbnSelchangeTdkCbMode)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_FogMode message handlers

BOOL i3TDKSpec_Attr_FogMode::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_FogMode::OnOK()
{
}

void i3TDKSpec_Attr_FogMode::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_FogMode::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_FogMode::OnCbnSelchangeTdkCbMode()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_Mode.GetCurSel())
	{
		case 0 :		m_pAttr->Set( I3G_FOG_NONE);	break;
		case 1 :		m_pAttr->Set( I3G_FOG_LINEAR);	break;
		case 2 :		m_pAttr->Set( I3G_FOG_EXP);	break;
		case 3 :		m_pAttr->Set( I3G_FOG_EXP2);	break;
	}
}
