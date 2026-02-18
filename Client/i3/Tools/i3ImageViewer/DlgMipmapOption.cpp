// DlgMipmapOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3ImageViewer.h"
#include "DlgMipmapOption.h"
#include "Defines.h"
#include ".\dlgmipmapoption.h"

// CDlgMipmapOption dialog

IMPLEMENT_DYNAMIC(CDlgMipmapOption, CDialog)
CDlgMipmapOption::CDlgMipmapOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMipmapOption::IDD, pParent)
{
	m_Type = ODT_GENERATE_MIPMAP;
}

CDlgMipmapOption::~CDlgMipmapOption()
{
}

void CDlgMipmapOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_EXPORTPATH, m_ED_ExportPath);
	DDX_Control(pDX, IDC_ED_MINCX, m_ED_MinCX);
	DDX_Control(pDX, IDC_ED_MINCY, m_ED_MinCY);
	DDX_Control(pDX, IDC_CB_FILTER, m_CB_Filter);
	DDX_Control(pDX, IDC_CHK_DITHER, m_CHK_Dither);
	DDX_Control(pDX, IDC_CHK_SRGB_IN, m_CHK_SRGB_In);
	DDX_Control(pDX, IDC_CHK_SRGB_OUT, m_CHK_SRGB_Out);
	DDX_Control(pDX, IDC_CHK_EXPORT_I3I, m_CHK_ExportI3I);
}

bool CDlgMipmapOption::Execute( ODT_TYPE type)
{
	m_Type = type;

	if( DoModal() == IDCANCEL)
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CDlgMipmapOption, CDialog)
	ON_BN_CLICKED(IDC_BTN_EXPORTPATH, OnBnClickedBtnExportpath)
END_MESSAGE_MAP()


// CDlgMipmapOption message handlers

BOOL CDlgMipmapOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		char conv[512];

		// Export Path
		m_ED_ExportPath.SetWindowText( g_pOption->m_szExportPath);

		// Min CX
		sprintf( conv, "%d", g_pOption->m_MinCX);
		m_ED_MinCX.SetWindowText( conv);

		// Min CY
		sprintf( conv, "%d", g_pOption->m_MinCY);
		m_ED_MinCY.SetWindowText( conv);

		// Filter
		switch( g_pOption->m_Filter)
		{
			case tex::FILTER_NONE :			m_CB_Filter.SetCurSel( 0);	break;
			case tex::FILTER_POINT :		m_CB_Filter.SetCurSel( 1);	break;
			case tex::FILTER_LINEAR :		m_CB_Filter.SetCurSel( 2);	break;
			case tex::FILTER_TRIANGLE :		m_CB_Filter.SetCurSel( 3);	break;
			case tex::FILTER_BOX :			m_CB_Filter.SetCurSel( 4);	break;
		}

		// Dither
		if( g_pOption->m_bDither)
			m_CHK_Dither.SetCheck( BST_CHECKED);
		else
			m_CHK_Dither.SetCheck( BST_UNCHECKED);

		// SRGB In
		if( g_pOption->m_bSRGB_In)
			m_CHK_SRGB_In.SetCheck( BST_CHECKED);
		else
			m_CHK_SRGB_In.SetCheck( BST_UNCHECKED);

		// SRGB Out
		if( g_pOption->m_bSRGB_Out)
			m_CHK_SRGB_Out.SetCheck( BST_CHECKED);
		else
			m_CHK_SRGB_Out.SetCheck( BST_UNCHECKED);

		// Export I3I
		if( g_pOption->m_bExportI3I)
			m_CHK_ExportI3I.SetCheck( BST_CHECKED);
		else
			m_CHK_ExportI3I.SetCheck( BST_UNCHECKED);
	}

	switch( m_Type)
	{
		case ODT_EXTRACT_MIPMAP :
		case ODY_RELOAD_MIPMAP :
			m_ED_MinCX.EnableWindow( FALSE);
			m_ED_MinCY.EnableWindow( FALSE);
			m_CB_Filter.EnableWindow( FALSE);
			m_CHK_Dither.EnableWindow( FALSE);
			m_CHK_SRGB_In.EnableWindow( FALSE);
			m_CHK_SRGB_Out.EnableWindow( FALSE);
			break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMipmapOption::OnOK()
{
	char conv[512];

	{
		// Export Path
		m_ED_ExportPath.GetWindowText( g_pOption->m_szExportPath, sizeof( g_pOption->m_szExportPath) - 1);

		// Min CX
		m_ED_MinCX.GetWindowText( conv, sizeof(conv) - 1);
		g_pOption->m_MinCX = atoi( conv);

		// Min CY
		m_ED_MinCY.GetWindowText( conv, sizeof(conv) - 1);
		g_pOption->m_MinCY = atoi( conv);

		// Filter
		switch( m_CB_Filter.GetCurSel())
		{
			case 0 :	g_pOption->m_Filter = tex::FILTER_NONE;		break;
			case 1 :	g_pOption->m_Filter = tex::FILTER_POINT;	break;
			case 2 :	g_pOption->m_Filter = tex::FILTER_LINEAR;	break;
			case 3 :	g_pOption->m_Filter = tex::FILTER_TRIANGLE;	break;
			case 4 :	g_pOption->m_Filter = tex::FILTER_BOX;	break;
		}

		// Dither
		g_pOption->m_bDither = ( m_CHK_Dither.GetCheck() == BST_CHECKED);

		// SRGB In
		g_pOption->m_bSRGB_In = ( m_CHK_SRGB_In.GetCheck() == BST_CHECKED);

		// SRGB Out
		g_pOption->m_bSRGB_Out = ( m_CHK_SRGB_Out.GetCheck() == BST_CHECKED);

		// Export I3I
		g_pOption->m_bExportI3I = ( m_CHK_ExportI3I.GetCheck() == BST_CHECKED);
	}

	CDialog::OnOK();
}

void CDlgMipmapOption::OnBnClickedBtnExportpath()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "Export Path", BIF_RETURNONLYFSDIRS, NULL, 0, g_pOption->m_szExportPath) == FALSE)
		return;

	m_ED_ExportPath.SetWindowText( dlg.GetSelectedFolderPath());

	strcpy( g_pOption->m_szExportPath, dlg.GetSelectedFolderPath());
}
