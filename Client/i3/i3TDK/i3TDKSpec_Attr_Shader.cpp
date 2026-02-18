// i3TDKSpec_Attr_Shader.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_Shader.h"


// i3TDKSpec_Attr_Shader dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_Shader, i3TDKDialogBase)

i3TDKSpec_Attr_Shader::~i3TDKSpec_Attr_Shader()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_Shader::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_SRC, m_ED_Src);
	DDX_Control(pDX, IDC_TDK_CB_VS_VERSION, m_VSVersion);
	DDX_Control(pDX, IDC_TDK_CB_PS_VERSION, m_PSVersion);
	DDX_Control(pDX, IDC_TDK_CHK_ADDCACHE, m_AddToCache);
	DDX_Control(pDX, IDC_TDK_BTN_RECOMPILE, m_Recompile);
}

void i3TDKSpec_Attr_Shader::SetObject( i3ElementBase * pObj)
{
	I3_REF_CHANGE( m_pAttr, (i3ShaderAttr *) pObj);

	if( m_pAttr != nullptr)
	{
		I3ASSERT( i3::kind_of<i3ShaderAttr*>(pObj));

		// Source
#ifdef _UNICODE
		i3::stack_wstring strSource;	i3::mb_to_utf16(m_pAttr->getSource(), strSource);
#else
		const i3::rc_string& strSource = m_pAttr->getSourceString();
#endif
		m_ED_Src.SetWindowText( strSource.c_str() );

		// Vertex Shader Version
		switch( m_pAttr->getTargetVertexShaderVersion())
		{
			case I3G_SHADER_VERSION_3:		m_VSVersion.SetCurSel( 2);	break;
			case I3G_SHADER_VERSION_2:		m_VSVersion.SetCurSel( 1);	break;
			default :						m_VSVersion.SetCurSel( 0);	break;
		}

		// Pixel Shader Version
		switch( m_pAttr->getTargetPixelShaderVersion())
		{
			case I3G_SHADER_VERSION_3:		m_PSVersion.SetCurSel( 2);	break;
			case I3G_SHADER_VERSION_2:		m_PSVersion.SetCurSel( 1);	break;
			default :						m_PSVersion.SetCurSel( 0);	break;
		}

		// Add to the cache
		if( m_pAttr->getAddCacheState())
			m_AddToCache.SetCheck( BST_CHECKED);
		else
			m_AddToCache.SetCheck( BST_UNCHECKED);
	}
	else
	{
		m_ED_Src.SetWindowText( _T("") );
		m_VSVersion.SetCurSel( -1);
		m_PSVersion.SetCurSel( -1);
		m_AddToCache.SetCheck( BST_UNCHECKED);
	}
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_Shader, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_VS_VERSION, &i3TDKSpec_Attr_Shader::OnCbnSelchangeTdkCbVsVersion)
	ON_CBN_SELCHANGE(IDC_TDK_CB_PS_VERSION, &i3TDKSpec_Attr_Shader::OnCbnSelchangeTdkCbPsVersion)
	ON_BN_CLICKED(IDC_TDK_CHK_ADDCACHE, &i3TDKSpec_Attr_Shader::OnBnClickedTdkChkAddcache)
	ON_BN_CLICKED(IDC_TDK_BTN_RECOMPILE, &i3TDKSpec_Attr_Shader::OnBnClickedTdkBtnRecompile)
	ON_EN_KILLFOCUS(IDC_TDK_ED_SRC, &i3TDKSpec_Attr_Shader::OnEnKillfocusTdkEdSrc)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_Shader message handlers

void i3TDKSpec_Attr_Shader::OnCbnSelchangeTdkCbVsVersion()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_VSVersion.GetCurSel())
	{
		case 1 :	m_pAttr->setTargetVertexShaderVersion( I3G_SHADER_VERSION_2);	break;
		case 2 :	m_pAttr->setTargetVertexShaderVersion( I3G_SHADER_VERSION_3);	break;
		default :	m_pAttr->setTargetVertexShaderVersion( I3G_SHADER_VERSION_1);	break;
	}
}

void i3TDKSpec_Attr_Shader::OnCbnSelchangeTdkCbPsVersion()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_PSVersion.GetCurSel())
	{
		case 1 :	m_pAttr->setTargetPixelShaderVersion( I3G_SHADER_VERSION_2);	break;
		case 2 :	m_pAttr->setTargetPixelShaderVersion( I3G_SHADER_VERSION_3);	break;
		default :	m_pAttr->setTargetPixelShaderVersion( I3G_SHADER_VERSION_1);	break;
	}
}

void i3TDKSpec_Attr_Shader::OnBnClickedTdkChkAddcache()
{
	if( m_pAttr == nullptr)
		return;

	m_pAttr->setAddCacheState( m_AddToCache.GetCheck() == BST_CHECKED);
}

void i3TDKSpec_Attr_Shader::OnBnClickedTdkBtnRecompile()
{
	if( m_pAttr == nullptr)
		return;

	m_pAttr->LoadShader();
}

void i3TDKSpec_Attr_Shader::OnOK()
{
	if( m_pAttr == nullptr)
		return;

	OnEnKillfocusTdkEdSrc();

	m_pAttr->LoadShader();
}

void i3TDKSpec_Attr_Shader::OnCancel()
{
}

void i3TDKSpec_Attr_Shader::OnEnKillfocusTdkEdSrc()
{
	TCHAR conv[MAX_PATH];

	m_ED_Src.GetWindowText( conv, _countof( conv));

	if( m_pAttr != nullptr)
	{
#ifdef _UNICODE
		i3::stack_string strSource;	i3::utf16_to_mb(conv, strSource);
		const char* szSource = strSource.c_str();
#else
		const char* szSource = conv;
#endif
		m_pAttr->setSource( szSource);
	}
}
