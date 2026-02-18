// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// ItemTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "ItemTool.h"
#include "MainFrm.h"

#include "ItemToolDoc.h"
#include "ItemToolView.h"
//#include "vld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CItemToolApp

BEGIN_MESSAGE_MAP(CItemToolApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CItemToolApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CItemToolApp::OnFileSave)
END_MESSAGE_MAP()


// CItemToolApp 생성

CItemToolApp::CItemToolApp()
{

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 CItemToolApp 개체입니다.

CItemToolApp theApp;


// CItemToolApp 초기화

BOOL CItemToolApp::InitInstance()
{
	afxAmbientActCtx = FALSE;

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		return FALSE;
	}
	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("Zepetto"));
	LoadStdProfileSettings(0);  // 표준 INI 파일 옵션을 로드합니다.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();

	{
 		i3MemoryInit();
 
 		i3Error::Init();
 		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
 		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
 		i3Error::SetChannelName( I3LOG_FATAL, "ItemTool.log");
 		i3Error::SetChannelName( I3LOG_WARN, "ItemTool.log");
 
 		i3FrameworkRegisterMetas();
		i3OptRegisterMetas();
		i3TDKRegisterMetas();
		i3GICRegisterMetas();

//		CChara::RegisterMeta();
//		CLOD::RegisterMeta();
//		AI::RegisterMeta();
//		GICShapeAI::RegisterMeta();
//		GICDiagramAI::RegisterMeta();
 		//i3TDKRegisterMetas();

		//i3ResourceFile::RemoveResourceManager(i3SceneGraphInfo::GetClassMeta());
		//i3ResourceFile::RemoveResourceManager(i3Sound::GetClassMeta());
		//COptionInfo::RegisterMeta();

	}

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
 	CSingleDocTemplate* pDocTemplate;
 	pDocTemplate = new CSingleDocTemplate(
 		IDR_MAINFRAME_ITEMTOOL,
 		RUNTIME_CLASS(CItemToolDoc),
 		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
 		RUNTIME_CLASS(CItemToolView));
 	if (!pDocTemplate)
 		return FALSE;
 	AddDocTemplate(pDocTemplate);



	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
//  	CCommandLineInfo cmdInfo;
//  	ParseCommandLine(cmdInfo);
//  
//  
//  	// 명령줄에 지정된 명령을 디스패치합니다.
//  	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
//  	if (!ProcessShellCommand(cmdInfo))
//  		return FALSE;

 	CMainFrame* pFrame = new CMainFrame;
 	if (!pFrame)
 		return FALSE;
 	m_pMainWnd = pFrame;
 	// create and load the frame with its resources
 	pFrame->LoadFrame(IDR_MAINFRAME_ITEMTOOL,
 		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
 		NULL);
 
 	// The one and only window has been initialized, so show and update it
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
	enum { IDD = IDD_ABOUTBOX_ITEMTOOL };

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
void CItemToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CItemToolApp 사용자 지정 로드/저장 메서드

void CItemToolApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CItemToolApp::LoadCustomState()
{
}

void CItemToolApp::SaveCustomState()
{
}

// CItemToolApp 메시지 처리기




int CItemToolApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	i3System::TerminateSys();

	i3ElementBase::DumpObjects();
	
//	new int[512];	// crtdbg 죽었는지 체크해봄.(2012.09.24.수빈)

	return CWinAppEx::ExitInstance();
}

void CItemToolApp::OnFileSave()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(g_pPefList)
		g_pPefList->SaveFile();
}
