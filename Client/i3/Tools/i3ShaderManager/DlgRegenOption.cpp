// DlgRegenOption.cpp : implementation file
//

#include "stdafx.h"
#include "i3ShaderManager.h"
#include "DlgRegenOption.h"
#include ".\dlgregenoption.h"


// CDlgRegenOption dialog

IMPLEMENT_DYNAMIC(CDlgRegenOption, CDialog)
CDlgRegenOption::CDlgRegenOption(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRegenOption::IDD, pParent)
{
	m_vsVersion = I3G_SHADER_VERSION_1;
	m_psVersion = I3G_SHADER_VERSION_1;
	m_Vendor	= I3G_GPU_VENDOR_NA;
}

CDlgRegenOption::~CDlgRegenOption()
{
}

void CDlgRegenOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_HWVENDOR, m_CB_Vendor);
	DDX_Control(pDX, IDC_CB_VSVERSION, m_CB_VSVersion);
	DDX_Control(pDX, IDC_CB_PSVERSION, m_CB_PSVersion);
	DDX_Control(pDX, IDC_CHK_OVERRIDE, m_chkOverride);
}

bool CDlgRegenOption::Execute(void)
{
	if( DoModal() == IDCANCEL)
		return false;

	return true;
}

BEGIN_MESSAGE_MAP(CDlgRegenOption, CDialog)
END_MESSAGE_MAP()


// CDlgRegenOption message handlers

BOOL CDlgRegenOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		// GPU Vendor
		switch( m_Vendor)
		{
			case I3G_GPU_VENDOR_STANDARD :		m_CB_Vendor.SetCurSel( 1);  break;
			case I3G_GPU_VENDOR_NVIDIA :		m_CB_Vendor.SetCurSel( 2);	break;
			case I3G_GPU_VENDOR_ATI :			m_CB_Vendor.SetCurSel( 3);	break;
			default :							m_CB_Vendor.SetCurSel( 0);	break;
		}

		// VS Version
		switch( m_vsVersion)
		{
			case I3G_SHADER_VERSION_2 :			m_CB_VSVersion.SetCurSel( 1);	break;
			case I3G_SHADER_VERSION_3 :			m_CB_VSVersion.SetCurSel( 2);	break;
			default :							m_CB_VSVersion.SetCurSel( 0);	break;
		}

		// PS Version
		switch( m_psVersion)
		{
			case I3G_SHADER_VERSION_2 :			m_CB_PSVersion.SetCurSel( 1);	break;
			case I3G_SHADER_VERSION_3 :			m_CB_PSVersion.SetCurSel( 2);	break;
			default :							m_CB_PSVersion.SetCurSel( 0);	break;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRegenOption::OnOK()
{
	switch( m_CB_Vendor.GetCurSel())
	{
		case 1 :								m_Vendor = I3G_GPU_VENDOR_STANDARD;	break;
		case 2 :								m_Vendor = I3G_GPU_VENDOR_NVIDIA;	break;
		case 3 :								m_Vendor = I3G_GPU_VENDOR_ATI;		break;
		default :								m_Vendor = I3G_GPU_VENDOR_NA;	break;
	}

	switch( m_CB_VSVersion.GetCurSel())
	{
		case 1 :								m_vsVersion = I3G_SHADER_VERSION_2;	break;
		case 2 :								m_vsVersion = I3G_SHADER_VERSION_3;	break;
		default :								m_vsVersion = I3G_SHADER_VERSION_1;	break;
	}

	switch( m_CB_PSVersion.GetCurSel())
	{
		case 1 :								m_psVersion = I3G_SHADER_VERSION_2;	break;
		case 2 :								m_psVersion = I3G_SHADER_VERSION_3;	break;
		default :								m_psVersion = I3G_SHADER_VERSION_1;	break;
	}

	m_bOverride = ( m_chkOverride.GetCheck() == BST_CHECKED);

	CDialog::OnOK();
}
