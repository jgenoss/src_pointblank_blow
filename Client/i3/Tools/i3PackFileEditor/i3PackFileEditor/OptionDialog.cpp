// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackFileEditor.h"
#include "OptionDialog.h"
#include ".\optiondialog.h"
#include "GlobalVariable.h"

// COptionDialog dialog

IMPLEMENT_DYNAMIC(COptionDialog, CDialog)
COptionDialog::COptionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDialog::IDD, pParent)
	, m_bSwizzled(FALSE)
{
}

COptionDialog::~COptionDialog()
{
}

void COptionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_MEDIAFOLDER, m_MediaFolderCtrl);
	DDX_Control(pDX, IDC_ED_TEXDIR, m_TexPathCtrl);
	DDX_Control(pDX, IDC_ED_OUTDIR, m_OutPathCtrl);
	DDX_Check(pDX, IDC_CHECK_PSP_SWIZZLED, m_bSwizzled);
}


BEGIN_MESSAGE_MAP(COptionDialog, CDialog)
	ON_BN_CLICKED(IDC_BTN_FOLDERSEL, OnBnClickedBtnFoldersel)
	ON_BN_CLICKED(IDC_BTN_TEXSEL, OnBnClickedBtnTexsel)
	ON_BN_CLICKED(IDC_BTN_OUTDIR, OnBnClickedBtnOutdir)
	ON_BN_CLICKED(IDC_CHECK_PSP_SWIZZLED, OnBnClickedCheckPspSwizzled)
END_MESSAGE_MAP()


// COptionDialog message handlers

BOOL COptionDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_MediaFolderCtrl.SetWindowText( g_pProfile->m_szWorkDir);
		m_TexPathCtrl.SetWindowText( g_pProfile->m_szTexDir);
		m_OutPathCtrl.SetWindowText( g_pProfile->m_szOutDir);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionDialog::OnBnClickedBtnFoldersel()
{
	i3Win32FolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "작업 폴더를 선택하세요") == FALSE)
		return;

	m_MediaFolderCtrl.SetWindowText( dlg.GetSelectedFolderPath());
}

void COptionDialog::OnBnClickedBtnTexsel()
{
	i3Win32FolderSelectDialog dlg;
	char szRelPath[MAX_PATH];

	if( dlg.Execute( m_hWnd, "Texture Image를 읽어 들일 폴더를 선택하세요") == FALSE)
		return;

	i3String::MakeRelativePath( g_pProfile->m_szWorkDir, dlg.GetSelectedFolderPath(), szRelPath);
	i3String::MakeUnixPath( g_pProfile->m_szTexDir);

	m_TexPathCtrl.SetWindowText( szRelPath);
}

void COptionDialog::OnBnClickedBtnOutdir()
{
	i3Win32FolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "I3P 파일을 저장할 폴더를 선택하세요") == FALSE)
		return;

	m_OutPathCtrl.SetWindowText( dlg.GetSelectedFolderPath());
}

void COptionDialog::OnOK()
{
	char conv[MAX_PATH];

	m_MediaFolderCtrl.GetWindowText( g_pProfile->m_szWorkDir, sizeof(g_pProfile->m_szWorkDir));
	m_OutPathCtrl.GetWindowText( g_pProfile->m_szOutDir, sizeof( g_pProfile->m_szOutDir) - 1);

	m_TexPathCtrl.GetWindowText( conv, sizeof(conv) - 1);

	i3String::MakeRelativePath( g_pProfile->m_szWorkDir, conv, g_pProfile->m_szTexDir);
	i3String::MakeUnixPath( g_pProfile->m_szTexDir);

	CDialog::OnOK();
}

void COptionDialog::OnBnClickedCheckPspSwizzled()
{
	// TODO: Add your control notification handler code here
	m_bSwizzled = !m_bSwizzled;
	
}
