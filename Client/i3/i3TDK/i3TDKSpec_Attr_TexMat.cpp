// i3TDKSpec_Attr_TexMat.cpp : implementation file
#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_TexMat.h"
#include ".\i3tdkspec_attr_texmat.h"

#include "i3Base/string/ext/atof.h"


struct UVANIMINFO
{
	EUVANIM_TYPE	eType;
	const TCHAR*	pszName;
	const TCHAR*	pszDesc;
} ;

static UVANIMINFO _info[] = 
{
	{	EUVANIM_NONE,		_T("None"),			_T("B= 0"),			},
	{	EUVANIM_LINEAR,		_T("Linear"),		_T("B= b1*t + b2"),		},
	{	EUVANIM_SINUSOIDAL,	_T("Sinusoidal"),	_T("B= b2*sin(b1*t)"),	},
	{	EUVANIM_COUNT,		_T(""),				_T(""),	},
};


// i3TDKSpec_Attr_TexMat dialog
IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_TexMat, i3TDKDialogBase)

i3TDKSpec_Attr_TexMat::~i3TDKSpec_Attr_TexMat()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_TexMat::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_TDK_ST_UB, m_st_UB);
	DDX_Control( pDX, IDC_TDK_ST_VB, m_st_VB);
	DDX_Control( pDX, IDC_TDK_CB_UB, m_cb_UB);
	DDX_Control( pDX, IDC_TDK_CB_VB, m_cb_VB);
	DDX_Control( pDX, IDC_TDK_ED_UB1, m_ed_UB1);
	DDX_Control( pDX, IDC_TDK_ED_UB2, m_ed_UB2);
	DDX_Control( pDX, IDC_TDK_ED_VB1, m_ed_VB1);
	DDX_Control( pDX, IDC_TDK_ED_VB2, m_ed_VB2);

	DDX_Control( pDX, IDC_TDK_ED_PROJECTIVE, m_ed_Projective);
	DDX_Control( pDX, IDC_TDK_ED_ELEMENTCOUNT, m_ed_ElementCount);
	DDX_Control( pDX, IDC_TDK_CB_BINDTYPE, m_cb_BindType);
	
}

	
void i3TDKSpec_Attr_TexMat::SetObject( i3ElementBase * pObj)
{
	TCHAR conv[64];
	REAL32 val = 0.f;

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3TextureMatrixAttr *)pObj));

	if( m_pAttr != nullptr)
	{
		val = m_pAttr->GetUB1();
		i3::snprintf( conv, _countof(conv), _T("%.3f"), val);
		m_ed_UB1.SetWindowText( conv);

		val = m_pAttr->GetUB2();
		i3::snprintf( conv, _countof(conv), _T("%.3f"), val);
		m_ed_UB2.SetWindowText( conv);

		val = m_pAttr->GetVB1();
		i3::snprintf( conv, _countof(conv), _T("%.3f"), val);
		m_ed_VB1.SetWindowText( conv);

		val = m_pAttr->GetVB2();
		i3::snprintf( conv, _countof(conv), _T("%.3f"), val);
		m_ed_VB2.SetWindowText( conv);

		

		m_cb_UB.SetCurSel( m_pAttr->GetUVAnimTypeU());
		m_cb_VB.SetCurSel( m_pAttr->GetUVAnimTypeV());

		OnCbnSelchangeTdkCbUB();
		OnCbnSelchangeTdkCbVB();

		if( m_pAttr->IsProjective())
			m_ed_Projective.SetWindowText( _T("TRUE") );
		else
			m_ed_Projective.SetWindowText( _T("FALSE") );

		INT32 n = m_pAttr->GetOutputElementCount();
		i3::snprintf( conv, _countof( conv), _T("%d"), n);
		m_ed_ElementCount.SetWindowText( conv);

		AddBindType();
		m_cb_BindType.SetCurSel( (INT32)m_pAttr->GetBindType());
	}
}

static const TCHAR* strBindType[] = 
{
	_T("I3G_TEXTURE_BIND_DIFFUSE"),		 //= 0,
	_T("I3G_TEXTURE_BIND_DIFFUSE1"),
	_T("I3G_TEXTURE_BIND_DIFFUSE2"),
	_T("I3G_TEXTURE_BIND_DIFFUSE3"),
	_T("I3G_TEXTURE_BIND_SHADOW"),
	_T("I3G_TEXTURE_BIND_SPECULAR"),
	_T("I3G_TEXTURE_BIND_REFLECT"),
	_T("I3G_TEXTURE_BIND_NORMAL"),
	_T("I3G_TEXTURE_BIND_PRT"),
	_T("I3G_TEXTURE_BIND_LUX"),
	_T("I3G_TEXTURE_BIND_LIGHT"),
	_T("I3G_TEXTURE_BIND_EMISSIVE"),
	_T("I3G_TEXTURE_BIND_REFLECT_MASK"),
};


void i3TDKSpec_Attr_TexMat::AddBindType()
{
	for( INT32 i=0; i< _countof(strBindType) ; i++)
	{
		m_cb_BindType.AddString( strBindType[i]);
	}
}


BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_TexMat, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_UB, OnCbnSelchangeTdkCbUB)
	ON_CBN_SELCHANGE(IDC_TDK_CB_VB, OnCbnSelchangeTdkCbVB)
	ON_EN_KILLFOCUS(IDC_TDK_ED_UB1, OnEnKillfocusTdkEdUB1)
	ON_EN_KILLFOCUS(IDC_TDK_ED_UB2, OnEnKillfocusTdkEdUB2)
	ON_EN_KILLFOCUS(IDC_TDK_ED_VB1, OnEnKillfocusTdkEdVB1)
	ON_EN_KILLFOCUS(IDC_TDK_ED_VB2, OnEnKillfocusTdkEdVB2)
	ON_EN_CHANGE(IDC_TDK_ED_VB2, OnEnChangeTdkEdVB2)
END_MESSAGE_MAP()

void i3TDKSpec_Attr_TexMat::OnEnChangeTdkEdVB2( void)
{
	I3TRACE("ab\n");
}

// i3TDKSpec_Attr_TexMat message handlers
LRESULT i3TDKSpec_Attr_TexMat::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

BOOL i3TDKSpec_Attr_TexMat::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	for( INT32 i=0; _info[i].eType != EUVANIM_COUNT; i++)
	{
		m_cb_UB.AddString( _info[i].pszName);
		m_cb_VB.AddString( _info[i].pszName);
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_TexMat::OnOK()
{
}

void i3TDKSpec_Attr_TexMat::OnCancel()
{
}

void i3TDKSpec_Attr_TexMat::OnCbnSelchangeTdkCbUB( void)
{
	if( m_pAttr == nullptr)
		return;

	INT32 nSel = m_cb_UB.GetCurSel();

	m_pAttr->SetUVAnimTypeU( _info[nSel].eType);
	m_st_UB.SetWindowText( _info[nSel].pszDesc);
	
	if( nSel == 0)
	{
		m_ed_UB1.EnableWindow( FALSE);
		m_ed_UB2.EnableWindow( FALSE);
	}
	else
	{
		m_ed_UB1.EnableWindow( TRUE);
		m_ed_UB2.EnableWindow( TRUE);
	}
}

void i3TDKSpec_Attr_TexMat::OnCbnSelchangeTdkCbVB( void)
{
	if( m_pAttr == nullptr)
		return;

	INT32 nSel = m_cb_VB.GetCurSel();

	m_pAttr->SetUVAnimTypeV( _info[nSel].eType);
	m_st_VB.SetWindowText( _info[nSel].pszDesc);
	
	if( nSel == 0)
	{
		m_ed_VB1.EnableWindow( FALSE);
		m_ed_VB2.EnableWindow( FALSE);
	}
	else
	{
		m_ed_VB1.EnableWindow( TRUE);
		m_ed_VB2.EnableWindow( TRUE);
	}
}

void i3TDKSpec_Attr_TexMat::OnEnKillfocusTdkEdUB1( void)
{
	TCHAR conv[64];
	if( m_pAttr != nullptr)
	{
		m_ed_UB1.GetWindowText( conv, _countof( conv) -1);
		m_pAttr->SetUB1( (REAL32)i3::atof( conv));
	}
}

void i3TDKSpec_Attr_TexMat::OnEnKillfocusTdkEdUB2( void)
{
	TCHAR conv[64];
	if( m_pAttr != nullptr)
	{
		m_ed_UB2.GetWindowText( conv, _countof( conv) -1);
		m_pAttr->SetUB2( (REAL32)i3::atof( conv));
	}
}

void i3TDKSpec_Attr_TexMat::OnEnKillfocusTdkEdVB1( void)
{
	TCHAR conv[64];
	if( m_pAttr != nullptr)
	{
		m_ed_VB1.GetWindowText( conv, _countof( conv) -1);
		m_pAttr->SetVB1( (REAL32)i3::atof( conv));
	}
}

void i3TDKSpec_Attr_TexMat::OnEnKillfocusTdkEdVB2( void)
{
	TCHAR conv[64];
	if( m_pAttr != nullptr)
	{
		m_ed_VB2.GetWindowText( conv, _countof( conv) -1);
		m_pAttr->SetVB2( (REAL32)i3::atof( conv));
	}
}