// i3TDKSpec_Attr_Material.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_Material.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_Attr_Material dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_Material, i3TDKDialogBase)

i3TDKSpec_Attr_Material::~i3TDKSpec_Attr_Material()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_Material::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CHK_DIFF, m_CHK_Diff);
	DDX_Control(pDX, IDC_TDK_CHK_SPEC, m_CHK_Spec);
	DDX_Control(pDX, IDC_TDK_CHK_EMIS, m_CHK_Emis);
	DDX_Control(pDX, IDC_TDK_CHK_SHINI, m_CHK_Shini);
	DDX_Control(pDX, IDC_ST_DIFFPH, m_ST_DiffPH);
	DDX_Control(pDX, IDC_ST_SPECPH, m_ST_SpecPH);
	DDX_Control(pDX, IDC_ST_EMISPH, m_ST_EmisPH);
	DDX_Control(pDX, IDC_TDK_ED_SHINI, m_ED_Shini);
}

void i3TDKSpec_Attr_Material::SetObject( i3ElementBase * pObj)
{
	BOOL bDiff, bSpec, bEmis, bShini;
	TCHAR conv[256];

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3MaterialAttr *) pObj));

	bDiff = bSpec = bEmis = bShini = FALSE;

	if( m_pAttr != nullptr)
	{
		// Diffuse
		if( m_pAttr->isValidDiffuse())
			m_CHK_Diff.SetCheck( BST_CHECKED);
		else
			m_CHK_Diff.SetCheck( BST_UNCHECKED);

		bDiff = m_pAttr->isValidDiffuse();
		m_DiffCtrl.setColor( m_pAttr->GetDiffuse());

		// Specular
		if( m_pAttr->isValidSpecular())
			m_CHK_Spec.SetCheck( BST_CHECKED);
		else
			m_CHK_Spec.SetCheck( BST_UNCHECKED);

		bSpec = m_pAttr->isValidSpecular();
		m_SpecCtrl.setColor( m_pAttr->GetSpecular());

		// Emissive
		if( m_pAttr->isValidEmissive())
			m_CHK_Emis.SetCheck( BST_CHECKED);
		else
			m_CHK_Emis.SetCheck( BST_UNCHECKED);

		bEmis = m_pAttr->isValidEmissive();
		m_EmisCtrl.setColor( m_pAttr->GetEmissive());

		// Shininess
		if( m_pAttr->isValidShininess())
			m_CHK_Shini.SetCheck( BST_CHECKED);
		else
			m_CHK_Shini.SetCheck( BST_UNCHECKED);

		bShini = m_pAttr->isValidShininess();

		i3::ftoa( m_pAttr->GetShininess(), conv);
		m_ED_Shini.SetWindowText( conv);
	}
	else
	{
		m_CHK_Diff.SetCheck( BST_UNCHECKED);
		m_CHK_Spec.SetCheck( BST_UNCHECKED);
		m_CHK_Emis.SetCheck( BST_UNCHECKED);
		m_CHK_Shini.SetCheck( BST_UNCHECKED);
		m_ED_Shini.SetWindowText( _T("") );

		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		m_DiffCtrl.setColor( &col);
		m_SpecCtrl.setColor( &col);
		m_EmisCtrl.setColor( &col);
	}

	m_CHK_Diff.EnableWindow( m_pAttr != nullptr);
	m_CHK_Spec.EnableWindow( m_pAttr != nullptr);
	m_CHK_Emis.EnableWindow( m_pAttr != nullptr);
	m_CHK_Shini.EnableWindow( m_pAttr != nullptr);

	m_DiffCtrl.EnableWindow( bDiff);
	m_SpecCtrl.EnableWindow( bSpec);
	m_EmisCtrl.EnableWindow( bEmis);
	m_ED_Shini.EnableWindow( bShini);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_Material, i3TDKDialogBase)
ON_BN_CLICKED(IDC_TDK_CHK_DIFF, OnBnClickedTdkChkDiff)
ON_BN_CLICKED(IDC_TDK_CHK_SPEC, OnBnClickedTdkChkSpec)
ON_BN_CLICKED(IDC_TDK_CHK_EMIS, OnBnClickedTdkChkEmis)
ON_BN_CLICKED(IDC_TDK_CHK_SHINI, OnBnClickedTdkChkShini)
ON_EN_KILLFOCUS(IDC_TDK_ED_SHINI, OnEnKillfocusTdkEdShini)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_Material message handlers

BOOL i3TDKSpec_Attr_Material::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		CRect rt;

		m_ST_DiffPH.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_DiffCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 100);
		m_DiffCtrl.SetWindowPos( nullptr, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

		m_ST_SpecPH.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_SpecCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 101);
		m_SpecCtrl.SetWindowPos( nullptr, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

		m_ST_EmisPH.GetWindowRect( &rt);
		ScreenToClient( &rt);
		m_EmisCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 102);
		m_EmisCtrl.SetWindowPos( nullptr, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_Material::OnOK()
{
}

void i3TDKSpec_Attr_Material::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_Material::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_SETCOLOR)
	{
		if( m_pAttr != nullptr)
		{
			switch( wParam)
			{
				case 100 :		m_pAttr->SetDiffuse( m_DiffCtrl.getColor());	break;
				case 101 :		m_pAttr->SetSpecular( m_SpecCtrl.getColor());	break;
				case 102 :		m_pAttr->SetEmissive( m_EmisCtrl.getColor());	break;
			}
		}
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_Material::OnBnClickedTdkChkDiff()
{
	BOOL bValid;

	if( m_pAttr == nullptr)
		return;

	bValid = m_CHK_Diff.GetCheck() == BST_CHECKED;

	m_pAttr->setValidDiffuse( bValid == TRUE);
	m_DiffCtrl.EnableWindow( bValid);
}

void i3TDKSpec_Attr_Material::OnBnClickedTdkChkSpec()
{
	BOOL bValid;

	if( m_pAttr == nullptr)
		return;

	bValid = m_CHK_Spec.GetCheck() == BST_CHECKED;

	m_pAttr->setValidSpecular( bValid == TRUE);
	m_SpecCtrl.EnableWindow( bValid);
}

void i3TDKSpec_Attr_Material::OnBnClickedTdkChkEmis()
{
	BOOL bValid;

	if( m_pAttr == nullptr)
		return;

	bValid = m_CHK_Emis.GetCheck() == BST_CHECKED;

	m_pAttr->setValidEmissive( bValid == TRUE);
	m_EmisCtrl.EnableWindow( bValid);
}

void i3TDKSpec_Attr_Material::OnBnClickedTdkChkShini()
{
	BOOL bValid;

	if( m_pAttr == nullptr)
		return;

	bValid = m_CHK_Shini.GetCheck() == BST_CHECKED;

	m_pAttr->setValidShininess( bValid == TRUE);
	m_ED_Shini.EnableWindow( bValid);
}

void i3TDKSpec_Attr_Material::OnEnKillfocusTdkEdShini()
{
	TCHAR conv[256];

	if( m_pAttr == nullptr)
		return;

	if( m_ED_Shini.GetModify())
	{
		m_ED_Shini.GetWindowText( conv, _countof(conv) - 1);
		m_ED_Shini.SetModify( FALSE);

		m_pAttr->SetShininess( (REAL32) i3::atof( conv));
	}
}
