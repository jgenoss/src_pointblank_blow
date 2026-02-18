#include "stdafx.h"
#include "DlgProject.h"
#include "GlobalVar.h"

IMPLEMENT_DYNAMIC(CDlgProject, CDialog)

BEGIN_MESSAGE_MAP(CDlgProject, CDialog)

	ON_BN_CLICKED(IDC_BUTTON_QA, &CDlgProject::OnBnClickedButtonQa)
	ON_BN_CLICKED(IDC_BUTTON_LIVE, &CDlgProject::OnBnClickedButtonLive)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

CDlgProject::CDlgProject(const char* chName, const char* chQAPath, const char* chLivePath, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProject::IDD, pParent)
{
	i3::generic_string_copy(m_chProjectName, chName);
	i3::generic_string_copy(m_chQA, chQAPath);
	i3::generic_string_copy(m_chLive, chLivePath);
}

CDlgProject::~CDlgProject()
{
}

BOOL CDlgProject::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_EditName.SetWindowText(m_chProjectName);
	m_EditQA.SetWindowText(m_chQA);
	m_EditLive.SetWindowText(m_chLive);
	return TRUE;
}

void CDlgProject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_QADIR, m_EditQA);
	DDX_Control(pDX, IDC_EDIT_LIVEDIR, m_EditLive);
	DDX_Control(pDX, IDC_EDIT_PROJECTNAME, m_EditName);
	DDX_Control(pDX, IDC_BUTTON_QA, m_ButtonQA);
	DDX_Control(pDX, IDC_BUTTON_LIVE, m_ButtonLive);
}

void CDlgProject::OnBnClickedButtonQa()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if (dlg.Execute(m_hWnd, "Working Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szWorkPath) == TRUE)
	{
		m_EditQA.SetWindowText(dlg.GetSelectedFolderPath());
	}
}


void CDlgProject::OnBnClickedButtonLive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	i3TDKFolderSelectDialog dlg;

	if (dlg.Execute(m_hWnd, "Working Directory", BIF_RETURNONLYFSDIRS, NULL, NULL, g_pOption->m_szWorkPath) == TRUE)
	{
		m_EditLive.SetWindowText(dlg.GetSelectedFolderPath());
	}
}

void CDlgProject::OnBnClickedOk()
{
	m_EditName.GetWindowText((LPSTR)m_chProjectName, MAX_PATH - 1);
	m_EditQA.GetWindowText((LPSTR)m_chQA, MAX_PATH - 1);
	m_EditLive.GetWindowText((LPSTR)m_chLive, MAX_PATH - 1);

	OnOK();
}

const char* CDlgProject::GetQAPath() 
{
	return m_chQA;
}

const char* CDlgProject::GetLivePath()
{
	return m_chLive;
}

const char* CDlgProject::GetName()
{
	return m_chProjectName;
}