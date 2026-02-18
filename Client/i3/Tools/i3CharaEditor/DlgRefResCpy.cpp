
#include "stdafx.h"
#include "DlgRefResCpy.h"


IMPLEMENT_DYNAMIC(CDlgRefResCpy, CDialog)

CDlgRefResCpy::CDlgRefResCpy(CWnd* pParent)
	: CDialog(CDlgRefResCpy::IDD, pParent)
{
	memset(m_ownerWorkDir, 0, sizeof(m_ownerWorkDir));
	memset(m_filePath, 0, sizeof(m_filePath));
}

CDlgRefResCpy::~CDlgRefResCpy()
{
}

void CDlgRefResCpy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_REFRESCPY_EB_WORKDIR,	m_ebWorkDir);
	DDX_Control(pDX, IDC_REFRESCPY_EB_FILE,		m_ebFile);
	DDX_Control(pDX, IDC_REFRESCPY_EB_COPYTO,	m_ebCopyToDir);	
}


BEGIN_MESSAGE_MAP(CDlgRefResCpy, CDialog)
	ON_BN_CLICKED(IDC_REFRESCPY_BT_COPY, OnBtnCpy)
END_MESSAGE_MAP()


// CDlgRefResCpy message handlers

BOOL CDlgRefResCpy::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(strlen(m_ownerWorkDir) > 0)
		m_ebWorkDir.SetWindowText(m_ownerWorkDir);

	if(strlen(m_filePath) > 0)
	{
		INT32 lenOwnerWorkDir = strlen(m_ownerWorkDir);
		if(lenOwnerWorkDir > 0)
		{
			if(!strnicmp(m_ownerWorkDir, m_filePath, lenOwnerWorkDir))
			{
				char tmp[MAX_PATH] = {0};
				strcpy_s(tmp, MAX_PATH, m_filePath + lenOwnerWorkDir+1);
				strcpy_s(m_filePath, MAX_PATH, tmp);
			}
		}

		m_ebFile.SetWindowText(m_filePath);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRefResCpy::OnOK()
{
	CDialog::OnOK();
}

void CDlgRefResCpy::OnBtnCpy()
{
	if(!g_pChara)
		return;

	CString workdir; m_ebWorkDir.GetWindowText(workdir);
	CString filepath; m_ebFile.GetWindowText(filepath);
	CString copyToDir; m_ebCopyToDir.GetWindowText(copyToDir);
	
	if(workdir.IsEmpty() || filepath.IsEmpty() || copyToDir.IsEmpty())
	{
		AfxMessageBox("ERR! - Empty info founded..", MB_ICONERROR);
		return;
	}

	i3TDK::SetPendingUpdateState( TRUE);

	CChara::CopyRefFileTo(workdir.GetString(), filepath.GetString(), copyToDir.GetString());

	i3TDK::SetPendingUpdateState( FALSE);
}
