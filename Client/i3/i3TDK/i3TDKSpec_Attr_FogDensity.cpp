// i3TDKSpec_Attr_FogDensity.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_FogDensity.h"
#include ".\i3tdkspec_attr_fogdensity.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_Attr_FogDensity dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_FogDensity, i3TDKDialogBase)

i3TDKSpec_Attr_FogDensity::~i3TDKSpec_Attr_FogDensity()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_FogDensity::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_DENSITY, m_ED_Density);
}

void i3TDKSpec_Attr_FogDensity::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3FogDensityAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		TCHAR conv[256];

		i3::ftoa( m_pAttr->Get(), conv);

		m_ED_Density.SetWindowText( conv);
	}
	else
	{
		m_ED_Density.SetWindowText( _T("") );
	}

	m_ED_Density.SetModify( FALSE);
	m_ED_Density.EnableWindow( m_pAttr != nullptr);
}


BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_FogDensity, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_TDK_ED_DENSITY, OnEnKillfocusTdkEdDensity)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_FogDensity message handlers

BOOL i3TDKSpec_Attr_FogDensity::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_FogDensity::OnOK()
{
}

void i3TDKSpec_Attr_FogDensity::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_FogDensity::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_FogDensity::OnEnKillfocusTdkEdDensity()
{
	if( m_pAttr == nullptr)
		return;

	if( m_ED_Density.GetModify())
	{
		TCHAR conv[256];

		m_ED_Density.GetWindowText( conv, _countof(conv) - 1);

		m_pAttr->Set( (REAL32) i3::atof( conv));

		m_ED_Density.SetModify( TRUE);
	}
}
