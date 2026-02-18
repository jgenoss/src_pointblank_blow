// i3TDKSpec_Attr_FogNearFar.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_FogNearFar.h"
#include ".\i3tdkspec_attr_fognearfar.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_Attr_FogNearFar dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_FogNearFar, i3TDKDialogBase)

i3TDKSpec_Attr_FogNearFar::~i3TDKSpec_Attr_FogNearFar()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_FogNearFar::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_NEAR, m_ED_Near);
	DDX_Control(pDX, IDC_TDK_ED_FAR, m_ED_Far);
}

void i3TDKSpec_Attr_FogNearFar::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3FogNearFarAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		TCHAR conv[256];

		i3::ftoa( m_pAttr->GetNear(), conv);
		m_ED_Near.SetWindowText( conv);

		i3::ftoa( m_pAttr->GetFar(), conv);
		m_ED_Far.SetWindowText( conv);
	}
	else
	{
		m_ED_Near.SetWindowText( _T(""));
		m_ED_Far.SetWindowText( _T(""));
	}

	m_ED_Near.SetModify( FALSE);
	m_ED_Far.SetModify( FALSE);

	m_ED_Near.EnableWindow( m_pAttr != nullptr);
	m_ED_Far.EnableWindow( m_pAttr != nullptr);
}


BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_FogNearFar, i3TDKDialogBase)
	ON_EN_KILLFOCUS(IDC_TDK_ED_NEAR, OnEnKillfocusTdkEdNear)
	ON_EN_KILLFOCUS(IDC_TDK_ED_FAR, OnEnKillfocusTdkEdFar)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_FogNearFar message handlers

BOOL i3TDKSpec_Attr_FogNearFar::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_FogNearFar::OnOK()
{
}

void i3TDKSpec_Attr_FogNearFar::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_FogNearFar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_FogNearFar::OnEnKillfocusTdkEdNear()
{
	if( m_pAttr == nullptr)
		return;

	if( m_ED_Near.GetModify())
	{
		TCHAR conv[256];

		m_ED_Near.GetWindowText( conv, _countof(conv) - 1);

		m_pAttr->SetNear( (REAL32) i3::atof( conv));

		m_ED_Near.SetModify( FALSE);
	}
}

void i3TDKSpec_Attr_FogNearFar::OnEnKillfocusTdkEdFar()
{
	if( m_pAttr == nullptr)
		return;

	if( m_ED_Far.GetModify())
	{
		TCHAR conv[256];

		m_ED_Far.GetWindowText( conv, _countof(conv) - 1);

		m_pAttr->SetFar( (REAL32) i3::atof( conv));

		m_ED_Far.SetModify( FALSE);
	}
}
