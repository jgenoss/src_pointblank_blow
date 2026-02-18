// i3LevelDesign.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "MainFrm.h"
#include "GlobalVariable.h"
#include "i3LevelDesignDoc.h"
#include "i3LevelDesignView.h"

#include "i3Base/pool/pool_dbg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void _levelEditModeChanged( LEVEL_EDIT_MODE _mode )
{
	CMainFrame* pMain = static_cast< CMainFrame* >( theApp.GetMainWnd() );

	if( pMain )
	{
		pMain->m_PanelNavMesh.LevelEditModeChanged( _mode );
		i3Level::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3LevelElement3D::static_meta() );
	}
}


// Ci3LevelDesignApp

BEGIN_MESSAGE_MAP(Ci3LevelDesignApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)

	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &Ci3LevelDesignApp::OnFileOpen)
	//ON_COMMAND(ID_FILE_SAVEAS, &Ci3LevelDesignApp::OnFileSaveas)
END_MESSAGE_MAP()


// Ci3LevelDesignApp 생성

Ci3LevelDesignApp::Ci3LevelDesignApp()
{
}

Ci3LevelDesignApp::~Ci3LevelDesignApp(void)
{
}


// 유일한 Ci3LevelDesignApp 개체입니다.

Ci3LevelDesignApp theApp;

// Ci3LevelDesignApp 초기화

BOOL Ci3LevelDesignApp::InitInstance()
{
	//_CrtSetBreakAlloc(50482);
	//i3::set_break_alloc(58677);		// 메모리 풀 전용...

	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	{
		// init custom control
		{
			WNDCLASS wndcls;
			HINSTANCE hInst = AfxGetInstanceHandle();

			if( FALSE == ::GetClassInfo( hInst, "i3NavMeshGeneratorPropCtrl", &wndcls ) )
			{
				// otherwise we need to register a new class
				wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
				wndcls.lpfnWndProc      = ::DefWindowProc;
				wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
				wndcls.hInstance        = hInst;
				wndcls.hIcon            = NULL;
				wndcls.hCursor          = AfxGetApp()->LoadStandardCursor( IDC_ARROW );
				wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
				wndcls.lpszMenuName     = NULL;
				wndcls.lpszClassName    = "i3NavMeshGeneratorPropCtrl";

				if( !AfxRegisterClass(&wndcls) )
				{
					AfxThrowResourceException();
					return FALSE;
				}
			}

		}
	}

	{
		i3MemoryInit();
		i3Error::Init();

		i3Error::SetChannelName( I3LOG_NOTICE, "i3LevelDesign.log" );
		i3Error::SetChannelName( I3LOG_FATAL, "i3LevelDesign.log");
		i3Error::SetChannelName( I3LOG_WARN, "i3LevelDesign.log");

        i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE | I3ERR_CHN_CALLBACK);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );

		//i3GuiRegisterMetas();
		i3TDKRegisterMetas();
		i3Level::Init();

		srand( GetTickCount());
	}

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("Zepetto"));
	LoadStdProfileSettings(0);  // Load standard INI file options

	g_pProfile = new CLevelDesignProfile;
	g_pProfile->Load();

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Ci3LevelDesignDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(Ci3LevelDesignView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	{
		i3LevelFramework* frame = NULL;

		if( i3LevelViewport::GetCurrentFocusViewport() )
			frame = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

		if( frame )
			frame->SetFnEditModeChanged( _levelEditModeChanged );

		SetLevelEditMode( LEM_NORMAL );
	}	

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	
	return TRUE;
}

int Ci3LevelDesignApp::ExitInstance()
{
	i3Level::Close();
	i3LevelScene::ReleaseCommonResource();

	if( g_pProfile != NULL)
	{
		delete g_pProfile;
		g_pProfile = NULL;
	}

	i3System::TerminateSys();

	if( m_pMainWnd != NULL)
	{
		delete m_pMainWnd;
		m_pMainWnd = NULL;
	}
	
	//new int[625/4];				// 일부러 낸 릭..crtdbg 씹혔는지 체크용도로 주석건채로 그냥 놔두시기 바랍니다..(12.09.18.수빈)
	//i3ElementBase::DumpObjects();
	CMFCToolBar::CleanUpImages();		// 이게 없으면, 릭검출이 안되었었음.....
										// MainFrame의 안되던 소멸자호출을 복구하니..이것없이도 정상작동은 함..(2012.09.12.수빈)
	CMFCVisualManager::DestroyInstance();	// 이것이 없어 릭발생했는데..MFC의 버그로 보고된 것 같고, VS2008에서는 개선되지 않는다고함....(2012.09.12.수빈)
	CKeyboardManager::CleanUp();			// 이것 역시 보고된 MFC버그로 VS2008에서 개선되기 어려울것 같다.(2012.09.12.수빈)

	i3ElementBase::DumpObjects();

	return CWinApp::ExitInstance();
}



// Ci3LevelDesignApp 메시지 처리기



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
void Ci3LevelDesignApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ci3LevelDesignApp 메시지 처리기


void Ci3LevelDesignApp::OnFileOpen()
{
	/*
	CFileDialog	Dlg( TRUE, "i3LevelDesign", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3LevelDesign Project File(*.XML)|*.XML||", AfxGetMainWnd());

	if( Dlg.DoModal() != IDOK)
		return;

	OpenDocumentFile( LPCTSTR( Dlg.GetPathName()));
	*/

	CWinAppEx::OnFileOpen();
}

void	Ci3LevelDesignApp::SetLevelEditMode( LEVEL_EDIT_MODE _mode )
{
	i3LevelFramework* frame = NULL;

	if( i3LevelViewport::GetCurrentFocusViewport() )
		frame = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( frame )
		frame->SetLevelEditMode( _mode );
}

LEVEL_EDIT_MODE		Ci3LevelDesignApp::GetLevelEditMode()
{
	i3LevelFramework* frame = NULL;

	if( i3LevelViewport::GetCurrentFocusViewport() )
		frame = i3LevelViewport::GetCurrentFocusViewport()->getFramework();

	if( frame )
		return frame->GetLevelEditMode();

	return LEM_NORMAL;
}

