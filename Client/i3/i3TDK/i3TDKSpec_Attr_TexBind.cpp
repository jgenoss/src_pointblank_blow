// i3TDKSpec_Attr_TexBind.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_TexBind.h"
#include ".\i3tdkspec_attr_texbind.h"


// i3TDKSpec_Attr_TexBind dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_TexBind, i3TDKDialogBase)

i3TDKSpec_Attr_TexBind::~i3TDKSpec_Attr_TexBind()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_TexBind::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_PATH, m_ED_Path);
	DDX_Control(pDX, IDC_TDK_BTN_FILE, m_BTN_Path);
	DDX_Control(pDX, IDC_TDK_ED_WIDTH, m_ED_Width);
	DDX_Control(pDX, IDC_TDK_ED_HEIGHT, m_ED_Height);
	DDX_Control(pDX, IDC_TDK_ED_FORMAT_PERSIST, m_ED_FormatPersist);
	DDX_Control(pDX, IDC_TDK_ED_FORMAT_RUNTIME, m_ED_FormatRuntime);
	DDX_Control(pDX, IDC_TDK_ED_LEVEL, m_ED_Level);
	DDX_Control(pDX, IDC_TDK_CB_MIPMAPGEN, m_CB_Gen);
	DDX_Control(pDX, IDC_TDK_CB_MIPMAP_GENFILTER, m_CB_GenFilter);
	DDX_Control(pDX, IDC_TDK_IMAGEPH, m_ST_ImagePH);
}

void i3TDKSpec_Attr_TexBind::SetObject( i3ElementBase * pObj)
{
	TCHAR conv[512];
	bool bInvalid = true;

	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3TextureBindAttr *)pObj));

	if( m_pAttr != nullptr)
	{
		i3Texture * pTex = m_pAttr->GetTexture();

		if( pTex != nullptr)
		{
			bInvalid = false;

			// Path
#ifdef _UNICODE
			i3::stack_wstring wstrTexName;	i3::mb_to_utf16(pTex->GetName(), wstrTexName);
			const wchar_t* szTexName = wstrTexName.c_str();
#else
			const char* szTexName = pTex->GetName();
#endif
			m_ED_Path.SetWindowText( szTexName);

			// Width & Height
			i3::snprintf( conv, _countof(conv), _T("%d"), pTex->GetWidth());
			m_ED_Width.SetWindowText( conv);

			i3::snprintf( conv, _countof(conv), _T("%d"), pTex->GetHeight());
			m_ED_Height.SetWindowText( conv);

			// Format
#ifdef _UNICODE
			m_ED_FormatPersist.SetWindowText( i3Gfx::GetImageFormatNameW( pTex->GetPersistFormat()));
			m_ED_FormatRuntime.SetWindowText( i3Gfx::GetImageFormatNameW( pTex->GetFormat()));
#else
			m_ED_FormatPersist.SetWindowText( i3Gfx::GetImageFormatName( pTex->GetPersistFormat()));
			m_ED_FormatRuntime.SetWindowText( i3Gfx::GetImageFormatName( pTex->GetFormat()));
#endif
			// Mipmap
			i3::snprintf( conv, _countof(conv), _T("%d"), pTex->GetLevelCount());
			m_ED_Level.SetWindowText( conv);

			switch( pTex->getMipmapGenType())
			{
				case I3G_MIPMAP_LOAD :			m_CB_Gen.SetCurSel( 0);	break;
				case I3G_MIPMAP_AUTO_GEN :		m_CB_Gen.SetCurSel( 1);	break;
				case I3G_MIPMAP_DISABLE :		m_CB_Gen.SetCurSel( 2);	break;
			}

			m_CB_GenFilter.SetCurSel( pTex->getMipmapGenFilter());

			m_ImageCtrl.SetImage( pTex);
		}
	}
	
	if( bInvalid)
	{
		m_ED_Path.SetWindowText( _T("") );
		m_ED_Width.SetWindowText( _T(""));
		m_ED_Height.SetWindowText( _T(""));
		m_ED_FormatPersist.SetWindowText( _T(""));
		m_ED_FormatRuntime.SetWindowText( _T(""));
		m_ED_Level.SetWindowText( _T(""));
		m_CB_Gen.SetCurSel( -1);
		m_ImageCtrl.SetImage( nullptr);
	}

	m_ED_Path.EnableWindow( bInvalid == false);
	m_BTN_Path.EnableWindow( true);
	m_ED_Width.EnableWindow( bInvalid == false);
	m_ED_Height.EnableWindow( bInvalid == false);
	m_ED_FormatPersist.EnableWindow( bInvalid == false);
	m_ED_FormatRuntime.EnableWindow( bInvalid == false);
	m_ED_Level.EnableWindow( bInvalid == false);
	m_CB_Gen.EnableWindow( bInvalid == false);
}


BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_TexBind, i3TDKDialogBase)
	ON_BN_CLICKED(IDC_TDK_BTN_FILE, OnBnClickedTdkBtnFile)
	ON_CBN_SELCHANGE(IDC_TDK_CB_MIPMAP_GENFILTER, OnCbnSelchangeTdkCbMipmapGenfilter)
	ON_CBN_SELCHANGE(IDC_TDK_CB_MIPMAPGEN, OnCbnSelchangeTdkCbMipmapgen)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_TexBind message handlers

BOOL i3TDKSpec_Attr_TexBind::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_ImageCtrl.Create( WS_VISIBLE | WS_CHILD, this, 100);

		CRect rt;

		m_ST_ImagePH.GetWindowRect( &rt);

		ScreenToClient( &rt);

		m_ImageCtrl.SetWindowPos( nullptr, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_TexBind::OnOK()
{
}

void i3TDKSpec_Attr_TexBind::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_TexBind::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_TexBind::OnBnClickedTdkBtnFile()
{
	if( m_pAttr == nullptr)
		return;

	CFileDialog	Dlg( TRUE, _T("i3i"), nullptr, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("i3i File(*.i3i) |*.i3i|tga파일(*.tga)|*.tga|"), this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	{
		i3ImageFile file;

#ifdef _UNICODE
		i3::stack_string strPathName;	i3::utf16_to_mb(Dlg.GetPathName(), strPathName);
#else
		i3::stack_string strPathName = (const char*)Dlg.GetPathName();
#endif
		i3Texture * pTex = file.Load(strPathName.c_str() );

		if( pTex == nullptr)
		{
			NotifyBox( m_hWnd, "%s 파일을 읽어들일 수 없습니다.", strPathName.c_str());
			return;
		}

		m_pAttr->SetTexture( pTex);

		SetObject( m_pAttr);
	}
}

void i3TDKSpec_Attr_TexBind::OnCbnSelchangeTdkCbMipmapGenfilter()
{
	if( m_pAttr == nullptr)
		return;

	i3Texture * pTex = m_pAttr->GetTexture();

	if( pTex != nullptr)
	{
		pTex->setMipmapGenFilter( (I3G_TEXTURE_FILTER) m_CB_GenFilter.GetCurSel());
	}
}

void i3TDKSpec_Attr_TexBind::OnCbnSelchangeTdkCbMipmapgen()
{
	if( m_pAttr == nullptr)
		return;

	i3Texture * pTex = m_pAttr->GetTexture();

	if( pTex == nullptr)
		return;

	switch( m_CB_Gen.GetCurSel())
	{
		case 0 :		pTex->setMipmapGenType( I3G_MIPMAP_LOAD);	break;
		case 1 :		pTex->setMipmapGenType( I3G_MIPMAP_AUTO_GEN);	break;
		case 2 :		pTex->setMipmapGenType( I3G_MIPMAP_DISABLE);	break;
	}
}
