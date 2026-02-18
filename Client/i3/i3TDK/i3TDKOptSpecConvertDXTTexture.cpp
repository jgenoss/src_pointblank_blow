// SelectNodeTypeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKOptSpecConvertDXTTexture.h"
#include "resource.h"
#include "i3Base/string/ext/atoi.h"

// i3TDKOptSpecConvertDXTTexture dialog

IMPLEMENT_DYNAMIC(i3TDKOptSpecConvertDXTTexture, i3TDKOptSpecBase)

void i3TDKOptSpecConvertDXTTexture::DoDataExchange(CDataExchange* pDX)
{
	i3TDKOptSpecBase::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_CB_TARGETFORMAT, m_TargetFormatCtrl);
	
	DDX_Control( pDX, IDC_ST_QUALITY, m_StQualityCtrl);
	DDX_Control( pDX, IDC_CB_QUALITY, m_QualityCtrl);

	DDX_Control( pDX, IDC_CHK_BINARYALPHA, m_BinaryAlphaCtrl);
	
	DDX_Control( pDX, IDC_ST_ALPHAREF,	m_StAlphaRefCtrl);
	DDX_Control( pDX, IDC_ED_ALPHAREF,	m_AlphaRefCtrl);

	DDX_Control( pDX, IDC_TDK_CHK_DITHER, m_DitherCtrl);
}

struct _I3_IMAGEFORMAT_INFO
{
	TCHAR				m_szName[64];
	I3G_IMAGE_FORMAT	m_Format;
} ;

static _I3_IMAGEFORMAT_INFO s_FormatTable[] =
{
	{	_T("I3G_IMAGE_FORMAT_DXT1_X"),	I3G_IMAGE_FORMAT_DXT1_X},
	{	_T("I3G_IMAGE_FORMAT_DXT1"),	I3G_IMAGE_FORMAT_DXT1},
	{	_T("I3G_IMAGE_FORMAT_DXT3"),	I3G_IMAGE_FORMAT_DXT3},
	{	_T("I3G_IMAGE_FORMAT_DXT5"),	I3G_IMAGE_FORMAT_DXT5},
	{	_T(""),							(I3G_IMAGE_FORMAT) 0},
};

static INT32	_FindByFormat( I3G_IMAGE_FORMAT fmt)
{
	INT32 i;

	for( i = 0; s_FormatTable[i].m_szName[0] != 0; i++)
	{
		if( s_FormatTable[i].m_Format == fmt)
			return i;
	}

	return -1;
}

void	i3TDKOptSpecConvertDXTTexture::SetTargetFormat( I3G_IMAGE_FORMAT fmt)
{
	BOOL bFlag = FALSE;
	INT32 idx;

	idx = _FindByFormat( fmt);
	m_TargetFormatCtrl.SetCurSel( idx);

	if( fmt & I3G_IMAGE_FORMAT_MASK_COMPRESSED)
	{
		bFlag = TRUE;
	}

	m_StQualityCtrl.EnableWindow( bFlag);
	m_QualityCtrl.EnableWindow( bFlag);

	m_BinaryAlphaCtrl.EnableWindow( bFlag);

	m_StAlphaRefCtrl.EnableWindow( bFlag);
	m_AlphaRefCtrl.EnableWindow( bFlag);

	m_DitherCtrl.EnableWindow( bFlag);
}

BEGIN_MESSAGE_MAP(i3TDKOptSpecConvertDXTTexture, i3TDKOptSpecBase)
END_MESSAGE_MAP()


// i3TDKOptSpecConvertDXTTexture message handlers

BOOL i3TDKOptSpecConvertDXTTexture::OnInitDialog()
{
	i3TDKOptSpecBase::OnInitDialog();

	{
		INT32 i;

		for( i = 0; s_FormatTable[i].m_szName[0] != 0; i++)
		{
			m_TargetFormatCtrl.AddString( s_FormatTable[i].m_szName);
		}
	}

	if( m_pOpt != nullptr)
	{
		i3OptConvertDXTTexture * pOpt = (i3OptConvertDXTTexture *) m_pOpt;
	
		// Target Format
		SetTargetFormat( pOpt->getTargetImageFormat());

		// Quality
		switch( pOpt->getQuality())
		{
			case i3OptConvertDXTTexture::QUALITY_FASTEST :		m_QualityCtrl.SetCurSel( 0);	break;
			case i3OptConvertDXTTexture::QUALITY_NORMAL :		m_QualityCtrl.SetCurSel( 1);	break;
			default:											m_QualityCtrl.SetCurSel( 2);	break;
		}

		// Binary Alpha
		if( pOpt->getBinaryAlphaEnable())
			m_BinaryAlphaCtrl.SetCheck( BST_CHECKED);
		else
			m_BinaryAlphaCtrl.SetCheck( BST_UNCHECKED);

		// Alpha Ref.
		TCHAR conv[256];
		i3::snprintf( conv, 256, _T("%d"), pOpt->getAlphaRef());
		m_AlphaRefCtrl.SetWindowText( conv);

		// Dither
		if( pOpt->getDitherEnable())
			m_DitherCtrl.SetCheck( BST_CHECKED);
		else
			m_DitherCtrl.SetCheck( BST_UNCHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKOptSpecConvertDXTTexture::OnOK()
{
	{
		i3OptConvertDXTTexture * pOpt = (i3OptConvertDXTTexture *) m_pOpt;
	
		// Target Format
		if( m_TargetFormatCtrl.GetCurSel() == -1)
			return;

		pOpt->setTargetImageFormat( s_FormatTable[ m_TargetFormatCtrl.GetCurSel()].m_Format);

		// Quality
		switch( m_QualityCtrl.GetCurSel())
		{
			case 0 :	pOpt->setQuality( i3OptConvertDXTTexture::QUALITY_FASTEST);	break;
			case 1 :	pOpt->setQuality( i3OptConvertDXTTexture::QUALITY_NORMAL);	break;
			default :	pOpt->setQuality( i3OptConvertDXTTexture::QUALITY_HIGHEST);	break;
		}

		// Binary Alpha
		if( m_BinaryAlphaCtrl.GetCheck() == BST_CHECKED)
			pOpt->setBinaryAlphaEnable( true);
		else
			pOpt->setBinaryAlphaEnable( false);

		// Alpha Ref
		TCHAR conv[256];
		m_AlphaRefCtrl.GetWindowText( conv, sizeof(conv) - 1);
		pOpt->setAlphaRef( (UINT8)i3::atoi( conv));

		// Dither
		pOpt->setDitherEnable( m_DitherCtrl.GetCheck() == BST_CHECKED);
	}

	i3TDKOptSpecBase::OnOK();
}
