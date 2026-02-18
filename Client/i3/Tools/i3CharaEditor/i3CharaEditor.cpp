// i3CharaEditor.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "MainFrm.h"
#include ".\i3charaeditor.h"
#include "AI.h"
#include "GICShapeAI.h"
#include "GICDiagramAI.h"

#ifdef _USE_TOOLAM_
#include "ToolAM.h"
#include "ToolAM_Def_CharTool.h"
#endif
#include "i3Base/itl/vector_util.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/ext/extract_filename.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//template<class T>
//static T* DoDailogAsModeless(CWnd* parentCWnd, void (*OnDlgInitBeforeCallBack)(void* ))
//{
//	T* modeless = new T(parentCWnd);
//
//	if(OnDlgInitBeforeCallBack)
//		(OnDlgInitBeforeCallBack)(modeless);
//
//	modeless->Create(T::IDD, CWnd::GetDesktopWindow());	
//	modeless->ShowWindow(SW_HIDE);
//
//	return modeless;
//}

// Ci3CharaEditorApp

BEGIN_MESSAGE_MAP(Ci3CharaEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// Ci3CharaEditorApp 생성

Ci3CharaEditorApp::Ci3CharaEditorApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 Ci3CharaEditorApp 개체입니다.

/*  _CRtSetBreakAlloc이 잘 안될때는, 아래와 같이 쓰면 될것이다. (2012.09.07.수빈)
 --> 이유) InitInstance()나 App클래스 생성자 이전에 메모리할당된 것들은 잡히지 않음.
 --> 참조) 다른 DLL이나 라이브러리에 의한 가짜 릭 발생은 MFC의 CrtDbg객체가 너무 늦게 생성되서 이다..
           이걸 교정하려면, MFC CrtDbg객체가 되도록 일찍 생성되게 만들면 된다...
		   현재로서는, 프로젝트옵션Debug의 의존 라이브러리항목의 최선두에 mfc90d.lib를 넣으면 일찍 생성되도록 교정이 되므로
		   유사한 문제가 있다면 mfc90d.lib를 명시적으로 삽입할것..
*/
/*
struct TestStructForDBG
{
	TestStructForDBG()
	{
		_CrtSetBreakAlloc(2923);
	}
} _TEST_DBG_;
*/

Ci3CharaEditorApp theApp;



// Ci3CharaEditorApp 초기화

BOOL Ci3CharaEditorApp::InitInstance()
{	
	//_CrtSetBreakAlloc(9236);
	
	//AfxSetAllocStop(134);		// 임시 확인 코드 (임수빈)

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

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

	AfxInitRichEdit();

	{
		i3MemoryInit();

		//	ErrorHandler 설정
		i3Error::Init();
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_DEBUG);
		i3Error::SetChannelName( I3LOG_FATAL, "i3CharaEditor.log");
		i3Error::SetChannelName( I3LOG_WARN, "i3CharaEditor.log");
		//i3Error::SetCallback( I3LOG_FATAL, CallbackLog, NULL);
		//i3Error::SetCallback( I3LOG_WARN, CallbackLog, NULL);
		//i3Error::SetCallback( I3LOG_NOTICE, CallbackLog, NULL);

		i3FrameworkRegisterMetas();
		i3OptRegisterMetas();
		i3TDKRegisterMetas();
		i3GICRegisterMetas();
/*
		CChara::RegisterMeta();
		CLOD::RegisterMeta();
		AI::RegisterMeta();
		GICShapeAI::RegisterMeta();
		GICDiagramAI::RegisterMeta();
*/
	}


	SetRegistryKey(_T("Zepetto"));
	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
	{
		return FALSE;
	}
	m_pMainWnd = pFrame;
	// 프레임을 만들어 리소스와 함께 로드합니다.
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	
	int num = 0;
	CStringW strw(GetCommandLine());

	LPWSTR* argv = CommandLineToArgvW(strw, &num);


 	if( num > 1 )
 	{
		char temp[512] = {0,};

		int nLen = ( int )wcslen( argv[0] );
		wcstombs( temp, argv[0], nLen + 1 );
		//int nLen = ( int )wcslen( argv[1] ); //테스트버전
		//wcstombs( temp, argv[1], nLen + 1 );

		if( strcmp("-b", temp) == 0 )
		{
			int nLen2 = ( int )wcslen( argv[1] );
			wcstombs( temp, argv[1], nLen2 + 1 );

			//AfxMessageBox(temp, MB_YESNO);

 			if ( i3System::IsFile(temp) )
 			{
				std::string sPath = temp;
				std::string sWeaponName = "";
				std::string sMediaPath;
				std::string sWeaponPath;

				INT32 nPos = sPath.rfind("\\");
				if( nPos != -1 )
				{
					sPath = sPath.substr(0, nPos);
					sWeaponPath = sPath;
				}

				nPos = sPath.rfind("\\");
				if( nPos != -1 )
				{
					sWeaponName = &sPath[nPos+1];
					sMediaPath = sPath.substr(0, nPos);
				}

				nPos = sMediaPath.rfind("\\");
				if( nPos != -1 )
				{
					sMediaPath = sMediaPath.substr(0, nPos);
				}
				sprintf_s(g_pOption->m_szWorkPath, "%s", (char*)sMediaPath.c_str());
				g_pOption->Save();

				sPath += "\\";
				sPath += "Resource\\";
				sPath += sWeaponName;

 				pFrame->OnOpenChara(temp);
 				pFrame->removeAllSceneGraph();

				for(int i = 0; i < 3; i++)
				{
					char chLODName[512] = {0,};
					sprintf_s(chLODName, 512, "%s_LOD%d.i3s", (char*)sPath.c_str(), i);
					
					pFrame->LoadResFile(chLODName, false);
				}

				g_pOption->m_bExportI3S			= 0;
				g_pOption->m_bRemoveSameImage   = 0;
				g_pOption->m_bExportI3CHR		= 1;
				g_pOption->m_bGenC				= 1;
				g_pOption->m_bRotateAxis		= 0;
				g_pOption->m_bGenMipmap			= 1;
				g_pOption->m_bConvertDXT		= 1;
				g_pOption->m_bDXTAlpha			= 1;
				g_pOption->m_bExternTexture		= 1;
				g_pOption->m_bOptAnim			= 0;
				g_pOption->m_bGetAnimPack		= 0;

				sPath = sWeaponPath;
 				sPath += "\\";
				sPath += sWeaponName;
				sPath += ".I3CHR";
	 
 				bool rst = g_pChara->Export((char*)sPath.c_str(), (char*)sWeaponName.c_str(), NULL);
				rst;		// 경고로 인해 무의미한 변수이름 넣음..

 				pFrame->OnSaveChara(temp);
 			}
	 
 			delete m_pMainWnd;
 			m_pMainWnd = NULL;
			return TRUE;
		}
		else if( strcmp("-GenerateMask", temp) == 0 )
		{
			//pFrame->GenerateMask(argv);
// 			delete m_pMainWnd;
// 			m_pMainWnd = NULL;
// 			return TRUE;
 	}
 	}

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	pFrame->DragAcceptFiles();
	// SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if ( i3System::IsFile(cmdInfo.m_strFileName) )
	{
		pFrame->OnOpenChara(cmdInfo.m_strFileName);
	}
	return TRUE;
}
 
int Ci3CharaEditorApp::ExitInstance()
{

	i3System::TerminateSys();

	int rv =	CWinApp::ExitInstance();

	i3ElementBase::DumpObjects();
	return rv;
}


// Ci3CharaEditorApp 메시지 처리기

I3ERROR_HANDLE Ci3CharaEditorApp::CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData)
{
	CMainFrame * pMain = (CMainFrame *) theApp.m_pMainWnd;

	if( pMain)
	{
		CPanelLog * pLog = (CPanelLog *) &pMain->m_PanelLog;

		if( pLog)
		{
			char strMessage[256];

			switch( type)
			{
			case I3LOG_FATAL:
				sprintf( strMessage, "[Fatal] %s : %s", pszWhere, pszMsg);
				break;
			case I3LOG_WARN:
				sprintf( strMessage, "[Warning] %s : %s", pszWhere, pszMsg);
				break;
			default:
				sprintf( strMessage, "[Notice] %s", pszMsg);
				break;
			}

			//	Log 창에 메시지 출력
			pLog->m_LogCtrl.AddText( strMessage);
		}
	}

	if( type == I3LOG_FATAL)
		return I3ERROR_HANDLE_BREAK;
	else
		return I3ERROR_HANDLE_CONTINUE;	
}

BOOL Ci3CharaEditorApp::OnIdle(LONG lCount)
{
#ifdef _USE_TOOLAM_
	if(TOOLAM_CMDQUEUE_DISPATCH(ToolAM::CharTool::QNAME) != TOOLAM_CMDQUEUE_NO_DISPATCH)
		return TRUE;
#endif

	return CWinApp::OnIdle(lCount);
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
void Ci3CharaEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ci3CharaEditorApp 메시지 처리기

