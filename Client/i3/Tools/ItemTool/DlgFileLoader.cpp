// DlgFileLoader.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DlgFileLoader.h"

#define UM_UPDATE WM_USER
// cDlgFileLoader 대화 상자입니다.

IMPLEMENT_DYNAMIC(cDlgFileLoader, CDialog)

cDlgFileLoader::cDlgFileLoader(CWnd* pParent /*=NULL*/)
	: CDialog(cDlgFileLoader::IDD, pParent)
	,	m_pFileData(NULL)
	,	m_bLoaded(false)
	, m_str_res_name(_T(""))
{

}

cDlgFileLoader::~cDlgFileLoader()
{
	I3_SAFE_DELETE(m_pFileData);
}

void cDlgFileLoader::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RES_NAME, m_str_res_name);
}


BEGIN_MESSAGE_MAP(cDlgFileLoader, CDialog)
	ON_MESSAGE(UM_UPDATE, OnUpdateData)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

LRESULT cDlgFileLoader::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	UpdateData(FALSE);

	return 0;
}
void cDlgFileLoader::OnStartThread()
{
	m_mFileNameWeapon.clear();

 	m_szRootPath = LPSTR(LPCTSTR(g_strRootPath));
 	m_pFileData = new CFileData();
 	m_bLoaded = false;
 	
	CWinThread* pWinThread = AfxBeginThread((AFX_THREADPROC)ReadThreadProc, this);
	m_handle = pWinThread->m_hThread;
}

void cDlgFileLoader::OnEndThread()
{
	//AfxEndThread(0);
}

static void ReadThreadProc(LPVOID lpParameter)
{
	cDlgFileLoader* pDlg = (cDlgFileLoader*)lpParameter;
	CFileData* pFile = pDlg->GetFileData();

	char prevPath[ MAX_PATH ];
	::GetCurrentDirectory( MAX_PATH, prevPath );

	pFile->FindFile(pDlg->m_mFileNameWeapon,pDlg->GetRootPath(), pDlg->m_str_res_name,"", pDlg);
	
	::SetCurrentDirectory( prevPath );
	pDlg->SetLoaded(true);
}
// cDlgFileLoader 메시지 처리기입니다.

BOOL cDlgFileLoader::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnStartThread();
	SetTimer(0,1000,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL cDlgFileLoader::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	CDialog::Create(lpszTemplateName, pParentWnd);

	return TRUE;
}

BOOL cDlgFileLoader::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::DestroyWindow();
}

void cDlgFileLoader::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void cDlgFileLoader::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialog::OnTimer(nIDEvent);
// 
 	//if (MsgWaitForMultipleObjects(1, &m_handle, FALSE,INFINITE, QS_PAINT | QS_POSTMESSAGE | QS_SENDMESSAGE | QS_TIMER))
	if( WAIT_TIMEOUT != ::WaitForSingleObject( m_handle, 0 ) )
 	{
		OnOK();
 	}
}

void cDlgFileLoader::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CDialog::OnOK();
}

void cDlgFileLoader::SetFileDir(const std::string& strName, const std::string& strFileName)
{
	m_mFileNameWeapon[strName].strFileName = strFileName; 
}

void cDlgFileLoader::SetFileRootName(const std::string& strName, const std::string& strRootName)
{
	m_mFileNameWeapon[strName].strRootName = strRootName;; 
}

void cDlgFileLoader::SetFileFolderName(const std::string& strName, const std::string& strFolderName)
{
	m_mFileNameWeapon[strName].strFolderName = strFolderName; 
}

std::string cDlgFileLoader::GetFileDir(const std::string& strName)
{
	mFileNameType::iterator iter = m_mFileNameWeapon.find(strName);
	if (iter == m_mFileNameWeapon.end())
	{
		std::string strI3S = strName + "_I3S";
		iter = m_mFileNameWeapon.find(strI3S);
		if (iter == m_mFileNameWeapon.end())
		{
			I3TRACE("해당 리소스 정보를 찾을수 없습니다 : %s\n",strName.c_str());
			return "";
		}
		else
			return m_mFileNameWeapon[strI3S].strFileName; 
	}
	return m_mFileNameWeapon[strName].strFileName; 
}

std::string cDlgFileLoader::GetFileRootName(const std::string& strName)
{
	mFileNameType::iterator iter = m_mFileNameWeapon.find(strName);
	if (iter == m_mFileNameWeapon.end())
	{
		std::string strI3S = strName + "_I3S";
		iter = m_mFileNameWeapon.find(strI3S);
		if (iter == m_mFileNameWeapon.end())
		{
			I3TRACE("해당 리소스 정보를 찾을수 없습니다 : %s\n",strName.c_str());
			return "";
		}
		else
			return m_mFileNameWeapon[strI3S].strRootName; 
	}
	return m_mFileNameWeapon[strName].strRootName; 
}

std::string cDlgFileLoader::GetFileFolderName(const std::string& strName)
{
	mFileNameType::iterator iter = m_mFileNameWeapon.find(strName);
	if (iter == m_mFileNameWeapon.end())
	{
		std::string strI3S = strName + "_I3S";
		iter = m_mFileNameWeapon.find(strI3S);
		if (iter == m_mFileNameWeapon.end())
		{
			I3TRACE("해당 리소스 정보를 찾을수 없습니다 : %s\n",strName.c_str());
			return "";
		}
		else
			return m_mFileNameWeapon[strI3S].strFolderName; 
	}
	return m_mFileNameWeapon[strName].strFolderName; 
}