// i3TDKSpec_Attr_AlphaFunc.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_AlphaFunc.h"
#include ".\i3tdkspec_attr_alphafunc.h"
#include "i3Base/string/ext/atoi.h"

// i3TDKSpec_Attr_AlphaFunc dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_AlphaFunc, i3TDKDialogBase)

i3TDKSpec_Attr_AlphaFunc::~i3TDKSpec_Attr_AlphaFunc()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_AlphaFunc::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_REF, m_ED_Ref);
	DDX_Control(pDX, IDC_TDK_CB_FUNC, m_CB_Func);
}

void i3TDKSpec_Attr_AlphaFunc::SetObject( i3ElementBase * pObj)
{
	TCHAR conv[256];

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3AlphaFuncAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		INT32 idx = -1;

		i3::snprintf( conv, _countof(conv), _T("%d"), m_pAttr->GetRefValue());
		m_ED_Ref.SetWindowText( conv);

		switch( m_pAttr->GetFunction())
		{
			case I3G_COMP_NEVER		:	idx = 0;	break;
			case I3G_COMP_LESS		:	idx = 1;	break;
			case I3G_COMP_EQUAL		:	idx = 2;	break;
			case I3G_COMP_LEQUAL	:	idx = 3;	break;
			case I3G_COMP_GREATER	:	idx = 4;	break;
			case I3G_COMP_NOTEQUAL	:	idx = 5;	break;
			case I3G_COMP_GEQUAL	:	idx = 6;	break;
			case I3G_COMP_ALWAYS	:	idx = 7;	break;
		}

		m_CB_Func.SetCurSel( idx);
	}
	else
	{
		m_ED_Ref.SetWindowText( _T("") );
		m_CB_Func.SetCurSel( -1);
	}

	m_ED_Ref.SetModify( FALSE);
	m_ED_Ref.EnableWindow( m_pAttr != nullptr);
	m_CB_Func.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_AlphaFunc, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_FUNC, OnCbnSelchangeTdkCbFunc)
	ON_EN_KILLFOCUS(IDC_TDK_ED_REF, OnEnKillfocusTdkEdRef)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_AlphaFunc message handlers

BOOL i3TDKSpec_Attr_AlphaFunc::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_AlphaFunc::OnOK()
{
}

void i3TDKSpec_Attr_AlphaFunc::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_AlphaFunc::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_AlphaFunc::OnCbnSelchangeTdkCbFunc()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_Func.GetCurSel())
	{
		case 0 :	m_pAttr->SetFunction( I3G_COMP_NEVER);	break;
		case 1 :	m_pAttr->SetFunction( I3G_COMP_LESS);	break;
		case 2 :	m_pAttr->SetFunction( I3G_COMP_EQUAL);	break;
		case 3 :	m_pAttr->SetFunction( I3G_COMP_LEQUAL);	break;
		case 4 :	m_pAttr->SetFunction( I3G_COMP_GREATER);	break;
		case 5 :	m_pAttr->SetFunction( I3G_COMP_NOTEQUAL);	break;
		case 6 :	m_pAttr->SetFunction( I3G_COMP_GEQUAL);	break;
		case 7 :	m_pAttr->SetFunction( I3G_COMP_ALWAYS);	break;
	}
}

void i3TDKSpec_Attr_AlphaFunc::OnEnKillfocusTdkEdRef()
{
	if( m_pAttr == nullptr)
		return;

	TCHAR conv[256];

	if( m_ED_Ref.GetModify())
	{
		m_ED_Ref.GetWindowText( conv, _countof(conv) - 1);

		m_pAttr->SetRefValue( (UINT8) i3::atoi( conv));

		m_ED_Ref.SetModify( FALSE);
	}
}
