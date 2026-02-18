// i3TDKSpec_Attr_Fresnel.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_Fresnel.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_Attr_Fresnel dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_Fresnel, i3TDKDialogBase)

i3TDKSpec_Attr_Fresnel::~i3TDKSpec_Attr_Fresnel()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_Fresnel::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CHK_ENABLE, m_CHK_Enable);
	DDX_Control(pDX, IDC_TDK_ED_IOR, m_ED_IOR);
	DDX_Control(pDX, IDC_ST_COLORPH, m_ST_ColorPH);
}

void i3TDKSpec_Attr_Fresnel::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3FresnelAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		TCHAR conv[256];

		// Enable/Disable
		if( m_pAttr->isEnable())
			m_CHK_Enable.SetCheck( BST_CHECKED);
		else
			m_CHK_Enable.SetCheck( BST_UNCHECKED);

		// IOR
		i3::ftoa( m_pAttr->getIOR(), conv);
		m_ED_IOR.SetWindowText( conv);

		// Color
		m_ColorCtrl.setColor( m_pAttr->getColor());
	}
	else
	{
		m_CHK_Enable.SetCheck( BST_UNCHECKED);
		m_ED_IOR.SetWindowText( false);
	}

	m_ED_IOR.SetModify( FALSE);

	m_CHK_Enable.EnableWindow( m_pAttr != nullptr);
	m_ED_IOR.EnableWindow( m_pAttr != nullptr);
	m_ST_ColorPH.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_Fresnel, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_TDK_CHK_ENABLE, OnBnClickedTdkChkEnable)
	ON_EN_KILLFOCUS(IDC_TDK_ED_IOR, OnEnKillfocusTdkEdIor)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_Fresnel message handlers

BOOL i3TDKSpec_Attr_Fresnel::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		m_ST_ColorPH.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_ColorCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_Fresnel::OnOK()
{
}

void i3TDKSpec_Attr_Fresnel::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_Fresnel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_TDK_SETCOLOR :
			if( m_pAttr != nullptr)
			{
				m_pAttr->setColor( m_ColorCtrl.getColor());
			}
			break;
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_Fresnel::OnBnClickedTdkChkEnable()
{
	if( m_pAttr == nullptr)
		return;

	if( m_CHK_Enable.GetCheck() == BST_CHECKED)
		m_pAttr->setEnable( true);
	else
		m_pAttr->setEnable( false);
}

void i3TDKSpec_Attr_Fresnel::OnEnKillfocusTdkEdIor()
{
	if( m_pAttr == nullptr)
		return;

	TCHAR conv[256];

	m_ED_IOR.GetWindowText( conv, _countof(conv) - 1);

	m_pAttr->setIOR( (REAL32) i3::atof( conv));
}
