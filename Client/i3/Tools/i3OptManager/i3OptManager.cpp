// i3OptManager.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "i3OptManager.h"
#include "MainFrm.h"
#include ".\i3optmanager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3OptManagerApp

BEGIN_MESSAGE_MAP(Ci3OptManagerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()


// Ci3OptManagerApp 생성

Ci3OptManagerApp::Ci3OptManagerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 Ci3OptManagerApp 개체입니다.

Ci3OptManagerApp theApp;

// Ci3OptManagerApp 초기화

I3ERROR_HANDLE CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData)
{
	CMainFrame * pMain = (CMainFrame *) theApp.m_pMainWnd;

	if( pMain)
	{
		/*
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
		*/
	}

	if( type == I3LOG_FATAL)
		return I3ERROR_HANDLE_BREAK;
	else
		return I3ERROR_HANDLE_CONTINUE;	
}

BOOL Ci3OptManagerApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	{
		//	ErrorHandler 설정
		i3Error::Init();
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_CALLBACK | I3ERR_CHN_DEBUG);
		i3Error::SetChannelName( I3LOG_FATAL, "i3OptManager.log");
		i3Error::SetChannelName( I3LOG_WARN, "i3OptManager.log");
		i3Error::SetCallback( I3LOG_FATAL, CallbackLog, NULL);
		i3Error::SetCallback( I3LOG_WARN, CallbackLog, NULL);
		i3Error::SetCallback( I3LOG_NOTICE, CallbackLog, NULL);

	}

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("Zepetto"));

	// 주 창을 만들기 위해 이 코드에서는 새 프레임 창 개체를
	// 만든 다음 이를 응용 프로그램의 주 창 개체로 설정합니다.
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 프레임을 만들어 리소스와 함께 로드합니다.
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	// SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.
	return TRUE;
}



int Ci3OptManagerApp::ExitInstance()
{
	i3TDKGlobalRes::Close();

	i3ElementBase::DumpObjects();

	return CWinApp::ExitInstance();
}


// Ci3OptManagerApp 메시지 처리기



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
void Ci3OptManagerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// Ci3OptManagerApp 메시지 처리기
