// i3PackFileEditor.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "i3PackFileEditor.h"
#include "MainFrm.h"

#include "i3PackFileEditorDoc.h"
#include "LeftView.h"
#include ".\i3packfileeditor.h"
#include "GlobalVariable.h"
#include "PackNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMyCommandLineInfo g_MyCmdInfo;

CMyCommandLineInfo::CMyCommandLineInfo()
{
	m_bForceOverwrite = FALSE;
	m_bCompile = FALSE;
	m_bCompileAndExit = FALSE;
	m_bLoadPSPFile = FALSE;
	m_bNoSound = FALSE;
	m_bNoBGM = FALSE;
}

void CMyCommandLineInfo::MyParseParamFlag(const char* pszParam)
{
	if( lstrcmpA(pszParam, "f") == 0 )
	{
		m_bForceOverwrite = TRUE;
	}
	else if (lstrcmpA(pszParam, "ce") == 0)
	{
		m_bCompile = TRUE;
		m_bCompileAndExit = TRUE;
	}
	else if (lstrcmpA(pszParam, "c") == 0)
	{
		m_bCompile = TRUE;
	}
	else if( lstrcmpA(pszParam, "psp") == 0)
	{
		m_bLoadPSPFile = TRUE;
	}
	else if( lstrcmpA(pszParam, "nosound") == 0 )
	{
		m_bNoSound = TRUE;
	}
	else if( lstrcmpA(pszParam, "nobgm") == 0 )
	{
		m_bNoBGM = TRUE;
	}
}

void CMyCommandLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if (bFlag)
	{
		USES_CONVERSION;
		MyParseParamFlag(T2CA(pszParam));
	}
	else
	{
		ParseParamNotFlag(pszParam);
	}

	ParseLast(bLast);
}

// Ci3PackFileEditorApp

BEGIN_MESSAGE_MAP(Ci3PackFileEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// Ci3PackFileEditorApp 생성

Ci3PackFileEditorApp::Ci3PackFileEditorApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 Ci3PackFileEditorApp 개체입니다.

Ci3PackFileEditorApp theApp;

// Ci3PackFileEditorApp 초기화

char g_szExeDir[MAX_PATH] = "";

BOOL Ci3PackFileEditorApp::InitInstance()
{
	GetCurrentDirectory( MAX_PATH, g_szExeDir );

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	{
		i3MemoryInit(70);

		i3SceneRegisterMetas();

		PackNode::RegisterMeta();

		i3Texture::SetConcreteClass( i3TextureMem::GetClassMeta());
		i3VertexArray::SetConcreteClass( i3VertexArrayMem::GetClassMeta());
	}

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("i3PackFileEditor"));

	{
		g_pProfile = new i3PackFileEditorProfile;
		g_pProfile->Load();
	}

	LoadStdProfileSettings(8);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	// 문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Ci3PackFileEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CLeftView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);
	ParseCommandLine( g_MyCmdInfo );
	// 명령줄에 지정된 명령을 디스패치합니다. 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(g_MyCmdInfo))
		return FALSE;
	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	// SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

int Ci3PackFileEditorApp::ExitInstance()
{
	g_pProfile->Save();
	delete g_pProfile;

	return CWinApp::ExitInstance();
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
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
void Ci3PackFileEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ci3PackFileEditorApp 메시지 처리기


