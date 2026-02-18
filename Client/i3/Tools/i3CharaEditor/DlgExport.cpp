// DlgExport.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "DlgExport.h"
#include "OptionInfo.h"
#include "GlobalVar.h"
#include ".\dlgexport.h"
#include "i3Base/string/ext/extract_filename.h"

// CDlgExport dialog

IMPLEMENT_DYNAMIC(CDlgExport, CDialog)
CDlgExport::CDlgExport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExport::IDD, pParent)
{
	m_szI3SName[0] = 0;
}

CDlgExport::~CDlgExport()
{
}

void CDlgExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_I3S, m_ExportI3SCtrl);
	DDX_Control(pDX, IDC_CHECK_REMOVE_SAMEIMAGE, m_checkRemoveSameImage);
	DDX_Control(pDX, IDC_CHK_I3CHR, m_ExportI3CHRCtrl);
	DDX_Control(pDX, IDC_CHK_GENC, m_GenCCtrl);
	DDX_Control(pDX, IDC_CHK_ROTATEAXIS, m_RotateAxisCtrl);
	DDX_Control(pDX, IDC_CHK_OPTIMIZE_GENMIPMAP, m_GenMipmapCtrl);
	DDX_Control(pDX, IDC_CHK_OPTIMIZE_DXT, m_ConvertDXTCtrl);
	DDX_Control(pDX, IDC_CHK_OPTIMIZE_DXT_ALPHA, m_OptionDXTAlphaCtrl );
	DDX_Control(pDX, IDC_CHK_OPTIMIZE_ANIM, m_OptAnim);
	DDX_Control(pDX, IDC_CHK_GEN_ANIMPACK, m_GenAnimPack);
	DDX_Control(pDX, IDC_CHK_EXTERN_I3I, m_ExternTexture);
	DDX_Control(pDX, IDC_ED_I3SNAME, m_ED_I3SName);
}

bool CDlgExport::Execute( const char * pszFile)
{
//	i3String::SplitFileName( pszFile, m_szI3SName, FALSE);
	i3::extract_filetitle(pszFile, m_szI3SName);

	if( DoModal() != IDOK)
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CDlgExport, CDialog)
END_MESSAGE_MAP()

// CDlgExport message handlers

BOOL CDlgExport::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		if( g_pOption->m_bExportI3S)
			m_ExportI3SCtrl.SetCheck( BST_CHECKED);
		else
			m_ExportI3SCtrl.SetCheck( BST_UNCHECKED);
	
		if (g_pOption->m_bRemoveSameImage)
			m_checkRemoveSameImage.SetCheck(BST_CHECKED);
		else
			m_checkRemoveSameImage.SetCheck(BST_UNCHECKED);

		if( g_pOption->m_bExportI3CHR)
			m_ExportI3CHRCtrl.SetCheck( BST_CHECKED);
		else
			m_ExportI3CHRCtrl.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bGenC)
			m_GenCCtrl.SetCheck( BST_CHECKED);
		else
			m_GenCCtrl.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bRotateAxis)
			m_RotateAxisCtrl.SetCheck( BST_CHECKED);
		else
			m_RotateAxisCtrl.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bGenMipmap)
			m_GenMipmapCtrl.SetCheck( BST_CHECKED);
		else
			m_GenMipmapCtrl.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bConvertDXT)
			m_ConvertDXTCtrl.SetCheck( BST_CHECKED);
		else
			m_ConvertDXTCtrl.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bDXTAlpha )
			m_OptionDXTAlphaCtrl.SetCheck( BST_CHECKED);
		else
			m_OptionDXTAlphaCtrl.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bExternTexture )
			m_ExternTexture.SetCheck( BST_CHECKED);
		else
			m_ExternTexture.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bOptAnim )
			m_OptAnim.SetCheck( BST_CHECKED);
		else
			m_OptAnim.SetCheck( BST_UNCHECKED);

		if( g_pOption->m_bGetAnimPack )
			m_GenAnimPack.SetCheck( BST_CHECKED);
		else
			m_GenAnimPack.SetCheck( BST_UNCHECKED);

		m_ED_I3SName.SetWindowText( m_szI3SName);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExport::OnOK()
{
	g_pOption->m_bExportI3S		= ( m_ExportI3SCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bRemoveSameImage = ( m_checkRemoveSameImage.GetCheck() == BST_CHECKED);
	g_pOption->m_bExportI3CHR	= ( m_ExportI3CHRCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bGenC			= ( m_GenCCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bRotateAxis	= ( m_RotateAxisCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bGenMipmap		= ( m_GenMipmapCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bConvertDXT	= ( m_ConvertDXTCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bDXTAlpha		= ( m_OptionDXTAlphaCtrl.GetCheck() == BST_CHECKED);
	g_pOption->m_bExternTexture	= ( m_ExternTexture.GetCheck() == BST_CHECKED);
	g_pOption->m_bOptAnim		= ( m_OptAnim.GetCheck() == BST_CHECKED);
	g_pOption->m_bGetAnimPack	= ( m_GenAnimPack.GetCheck() == BST_CHECKED);

	m_ED_I3SName.GetWindowText( m_szI3SName, sizeof(m_szI3SName));

	CDialog::OnOK();
}
