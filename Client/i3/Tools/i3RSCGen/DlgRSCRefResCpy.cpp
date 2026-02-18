
#include "stdafx.h"
#include "DlgRSCRefResCpy.h"


IMPLEMENT_DYNAMIC(CDlgRSCRefResCpy, CDialog)

CDlgRSCRefResCpy::CDlgRSCRefResCpy(CWnd* pParent)
: CDialog(CDlgRSCRefResCpy::IDD, pParent)
{
	memset(m_ownerWorkDir, 0, sizeof(m_ownerWorkDir));
	memset(m_filePath, 0, sizeof(m_filePath));
}

CDlgRSCRefResCpy::~CDlgRSCRefResCpy()
{
}

void CDlgRSCRefResCpy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_RSCREFRESCPY_EB_WORKDIR,	m_ebWorkDir);
	DDX_Control(pDX, IDC_RSCREFRESCPY_EB_SRCRSC,	m_ebSrcRSC);
	DDX_Control(pDX, IDC_RSCREFRESCPY_EB_COPYTO,	m_ebCopyTo);

	DDX_Control(pDX, IDC_RSCREFRESCPY_CK_GENRSCREFSTATUS, m_btGenRSCRefStatFile);
	DDX_Control(pDX, IDC_RSCREFRESCPY_CK_CPY_RSCREFRES, m_btCpyRSCRefRes);
}


BEGIN_MESSAGE_MAP(CDlgRSCRefResCpy, CDialog)
	ON_BN_CLICKED(IDC_RSCREFRESCPY_BT_GO, OnBtnCpy)
END_MESSAGE_MAP()


BOOL CDlgRSCRefResCpy::OnInitDialog()
{
	CDialog::OnInitDialog();

	//if(strlen(m_ownerWorkDir) > 0)
	//	m_ebWorkDir.SetWindowText(m_ownerWorkDir);

	//if(strlen(m_filePath) > 0)
	//{
	//	INT32 lenOwnerWorkDir = strlen(m_ownerWorkDir);
	//	if(lenOwnerWorkDir > 0)
	//	{
	//		if(!strnicmp(m_ownerWorkDir, m_filePath, lenOwnerWorkDir))
	//		{
	//			char tmp[MAX_PATH] = {0};
	//			strcpy_s(tmp, MAX_PATH, m_filePath + lenOwnerWorkDir+1);
	//			strcpy_s(m_filePath, MAX_PATH, tmp);
	//		}
	//	}

	//	m_ebFile.SetWindowText(m_filePath);
	//}

	m_btGenRSCRefStatFile.SetCheck(true);
	m_btCpyRSCRefRes.SetCheck(true);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRSCRefResCpy::OnOK()
{
	CDialog::OnOK();
}

void CDlgRSCRefResCpy::OnBtnCpy()
{
	UpdateData(FALSE);

	CString workdir;		m_ebWorkDir.GetWindowText(workdir);
	CString rscfilepath;	m_ebSrcRSC.GetWindowText(rscfilepath);
	CString copytodir;		m_ebCopyTo.GetWindowText(copytodir);

	bool genResInfo = m_btGenRSCRefStatFile.GetCheck() ? true : false;
	bool copyRefRes = m_btCpyRSCRefRes.GetCheck() ? true : false;

	if(workdir.IsEmpty() || rscfilepath.IsEmpty() || copytodir.IsEmpty())
	{
		AfxMessageBox("ERR! - Empty info founded..", MB_ICONERROR);
		return;
	}

	i3TDK::SetPendingUpdateState( TRUE);

	BOOL rst = i3ResourceManager::DumpRSCInfo(rscfilepath.GetString(), workdir.GetString(), copytodir.GetString(), 
		genResInfo, copyRefRes);

	i3TDK::SetPendingUpdateState( FALSE);

	if(!rst)
	{
		AfxMessageBox("ERR! - Fail!, directory or file path is invalid..", MB_ICONERROR);
		return;
	}
}
