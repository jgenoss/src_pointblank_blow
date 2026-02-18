
// RscTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "RscTool.h"
#include "MainFrm.h"

#include "RscToolDoc.h"
#include "RscToolView.h"

#include "PathMgr.h"
#include "FileCheck_PackScriptMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRscToolApp

BEGIN_MESSAGE_MAP(CRscToolApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CRscToolApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CRscToolApp 생성
#include "PackQueue.h"

//#include "i3Base/pool/pool_dbg.h"




CRscToolApp::CRscToolApp() : m_progress_cb(NULL)
{
//	i3::string test;
//	i3::sprintf(test, "%%d%%asdf%%asf%%%d%%asfasdf%%", 3);
//	i3::sprintf(test, "super power : %%soobin%% %d%%", 88);


	m_bHiColorIcons = TRUE;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
	
}

// 유일한 CRscToolApp 개체입니다.

CRscToolApp theApp;


// CRscToolApp 초기화
/*
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/itoa.h"
#include "intsafe.h"
*/

BOOL CRscToolApp::InitInstance()
{
/*
	char szTemp1[100];
	char szTemp2[100];
/*
//	i3::itoa(-1, 10, szTemp1, _countof(szTemp1));
//	i3::snprintf(szTemp1, 100, "%03d", 11);
	i3::snprintf(szTemp1, 100, "0x12");

	::_itoa(DWORD_MAX, szTemp2, 10);

	int i1 = _atoi64(szTemp1);
	int i2 = i3::atoi(szTemp1);
	int i3 = ::atoi(szTemp1);
	*/
	CWinAppEx::InitInstance();

	i3MemoryInit();
	PathMgr::i()->Initialize();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.

	// 아이콘 이미지 오버레이 때 아래함수가 없으면 출력 실패가 되었음..
	if (!AfxOleInit())
	{
		AfxMessageBox(100);
		return FALSE;
	}

	::InitCommonControls();
	
	AfxEnableControlContainer();

	SetRegistryKey(_T("제페토 리소스 관리툴 레지스트리 V1000"));

	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CRscToolDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CRscToolView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

//	CleanState();


	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	return TRUE;
}



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CRscToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CRscToolApp 사용자 지정 로드/저장 메서드

void CRscToolApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

// 현재 경로 저장은 파일로 하지 않고, 레지스트리로 하는게 좋겠다..

void CRscToolApp::LoadCustomState()
{
	//
}

void CRscToolApp::SaveCustomState()
{
	const i3::wstring& path_name = PathMgr::i()->GetPathName();
	const i3::wstring& export_root_path = PathMgr::i()->GetExportRootPath();

	if (path_name.empty() )
	{
		this->Unregister();
	}
	else
	{
		this->WriteProfileString(L"Path", L"PathName", path_name.c_str());
		this->WriteProfileString(L"Path", L"ExportRootPath", export_root_path.c_str());
	}
}

// CRscToolApp 메시지 처리기

//PackQueue_ProgressCallback g_cb;


BOOL CRscToolApp::OnIdle(LONG lCount)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	 
	PackQueue::i()->UpdateProgress_InMainThread(m_progress_cb);
	
	FileCheck_PackScriptMgr::i()->Update_Progress();

	return TRUE;
//	return CWinAppEx::OnIdle(lCount);
}

int CRscToolApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	  
	PathMgr::i()->SavePathFile();
	return CWinAppEx::ExitInstance();
}

