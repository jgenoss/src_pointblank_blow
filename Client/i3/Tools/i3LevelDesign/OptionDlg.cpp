// OptionDlg.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "OptionDlg.h"
#include "GlobalVariable.h"
#include ".\optiondlg.h"
#include "i3Base/string/ext/ftoa.h"
// COptionDlg dialog

IMPLEMENT_DYNAMIC(COptionDlg, CDialog)
COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
{
}

COptionDlg::~COptionDlg()
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_OPTION_WORKINGDIR, m_edWorkDir);
	DDX_Control(pDX, IDC_BTN_WORKINGDIR, m_btnWorkDir);
	DDX_Control(pDX, IDC_ED_OPTION_RESPATH, m_ResPathCtrl);
	DDX_Control(pDX, IDC_ED_SNAP_POS, m_ED_SnapPos);
	DDX_Control(pDX, IDC_ED_SNAP_ROTATE, m_ED_SnapRotate);
	DDX_Control(pDX, IDC_ED_SNAP_SCALE, m_ED_SnapScale);
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_WORKINGDIR, OnBnClickedBtnWorkingdir)
	ON_BN_CLICKED(IDC_BTN_RESPATH, OnBnClickedBtnRespath)
END_MESSAGE_MAP()


// COptionDlg message handlers

BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( g_pProfile != NULL)
	{
		m_edWorkDir.SetWindowText( (LPCTSTR)g_pProfile->m_szWorkingDir);
		m_ResPathCtrl.SetWindowText( (LPCTSTR)g_pProfile->m_szResPath);

		char conv[256];

		i3::ftoa( g_pProfile->m_PosSnap, conv);
		m_ED_SnapPos.SetWindowText( conv);

		i3::ftoa( g_pProfile->m_RotateSnap, conv);
		m_ED_SnapRotate.SetWindowText( conv);

		i3::ftoa( g_pProfile->m_ScaleSnap, conv);
		m_ED_SnapScale.SetWindowText( conv);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if(g_pProfile != NULL)
	{
		char conv[MAX_PATH];

		m_edWorkDir.GetWindowText( conv, MAX_PATH -1 );
		strcpy( g_pProfile->m_szWorkingDir, conv);

		m_ResPathCtrl.GetWindowText( conv, MAX_PATH - 1);
		strcpy( g_pProfile->m_szResPath, conv);

		m_ED_SnapPos.GetWindowText( conv, _countof( conv) - 1);
		g_pProfile->m_PosSnap = (REAL32) atof( conv);

		m_ED_SnapRotate.GetWindowText( conv, _countof( conv) - 1);
		g_pProfile->m_RotateSnap = (REAL32) atof( conv);

		m_ED_SnapScale.GetWindowText( conv, _countof( conv) - 1);
		g_pProfile->m_ScaleSnap = (REAL32) atof( conv);
	}

	g_pProfile->Apply();
	g_pProfile->Save();

	CDialog::OnOK();
}

void COptionDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void COptionDlg::OnBnClickedBtnWorkingdir()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd) == FALSE)
		return ;

	m_edWorkDir.SetWindowText( dlg.GetSelectedFolderPath());
}

void COptionDlg::OnBnClickedBtnRespath()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd) == FALSE)
		return ;

	m_ResPathCtrl.SetWindowText( dlg.GetSelectedFolderPath());
}
