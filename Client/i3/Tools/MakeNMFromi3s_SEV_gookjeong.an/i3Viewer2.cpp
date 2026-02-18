// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3Viewer2.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "i3Viewer2.h"
#include "MainFrm.h"

#include "i3Viewer2Doc.h"
#include "i3Viewer2View.h"

#include "DlgNoticeConvertState.h"

#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3Viewer2App

BEGIN_MESSAGE_MAP(Ci3Viewer2App, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &Ci3Viewer2App::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// Ci3Viewer2App 생성

Ci3Viewer2App::Ci3Viewer2App()
{

	m_bHiColorIcons = TRUE;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

// 유일한 Ci3Viewer2App 개체입니다.

Ci3Viewer2App theApp;


char* g_pMapName;
extern CDlgNoticeConvertState g_Dlg;

BOOL Ci3Viewer2App::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

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
	// 표준 초기화
	
	SetRegistryKey(_T("Zepetto"));
	LoadStdProfileSettings(0);  // Load standard INI file options

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();

	{
		i3MemoryInit();

		i3Error::Init();
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		i3Error::SetChannelName( I3LOG_FATAL, "i3Viewer2.log");
		i3Error::SetChannelName( I3LOG_WARN, "i3Viewer2.log");

		i3FrameworkRegisterMetas();
		i3TDKRegisterMetas();

		COptionInfo::RegisterMeta();
	}

	{
		//g_pOption = new COptionInfo;
		g_pOption = COptionInfo::NewObject();
	}

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(Ci3Viewer2Doc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(Ci3Viewer2View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// DDE Execute 열기를 활성화합니다.
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	// 끌어서 놓기에 대한 열기를 활성화합니다.
	m_pMainWnd->DragAcceptFiles();

	//pDocTemplate->OpenDocumentFile( "Test.i3s" );
	//g_Dlg.SetMessage( "Convert TestNM Complete. (1/1)" );
	
#ifdef CONVERT_BY_AUTO

	char *pstrWorldName[] = {
		"BlackPanther",
		"Blowcity",
		"Breakdown",
		"BreedingNest",
		"BurningHall",
		"Construction",
		"Construction2",
		"D_BreakDown",
		"D_Uptown",
		"DeadTown",
		"DesertCamp",
		"Downtown",
		"Downtown2",
		"DragonAlley",
		"DSquad",
		"EasternRoad",
		"FaceRock",
		"Giran2",
		"Helispot",
		"Hospital",
		"Kickpoint",
		"Library",
		"Luxville",
		"MachuPichu",
		"NPort",
		"NStation",
		"Outpost",
		"Safari",
		"SaintMansion",
		"SandStorm",
		"Sentrybase",
		"ShoppingCenter",
		"Stormtube",
		"SupplyBase",
		"TrainingCamp",
		"Tutorial",
		"Uptown"
	};

	int iMap = sizeof( pstrWorldName ) / sizeof( char* );
	REAL32 r32TimeTotal = 0.0f;

	char strBuf[256];
	for( int i = 0; i < iMap; i++ )
	{
		i3String::Format( strBuf, 256, "D:\\Project\\PointBlank\\Media\\World\\%s\\%s_Col_Hit.i3s", pstrWorldName[i], pstrWorldName[i] );
		g_pMapName = pstrWorldName[i];
		DWORD TimeBegin = GetTickCount();
		pDocTemplate->OpenDocumentFile( strBuf );
		DWORD TimeWork = GetTickCount() - TimeBegin;

		i3String::Format( strBuf, 256, "Convert %s Complete. (%d/%d) %.2f Second", pstrWorldName[i], i + 1, iMap, (REAL32)TimeWork / 1000.0f );
		g_Dlg.SetMessage( strBuf );
		r32TimeTotal += (REAL32)TimeWork / 1000.0f;
	}
	i3String::Format( strBuf, 256, "Convert Work Over. Total %.2f Second", r32TimeTotal );
	g_Dlg.SetMessage( strBuf );
	AfxMessageBox( "Convert Complete" );
#endif

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
void Ci3Viewer2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// Ci3Viewer2App customization load/save methods

void Ci3Viewer2App::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void Ci3Viewer2App::LoadCustomState()
{
}

void Ci3Viewer2App::SaveCustomState()
{
}

// Ci3Viewer2App 메시지 처리기




int Ci3Viewer2App::ExitInstance()
{
	if( g_pOption != NULL)
	{
		delete g_pOption;
		g_pOption = NULL;
	}

	i3System::TerminateSys();

	return CWinAppEx::ExitInstance();
}
