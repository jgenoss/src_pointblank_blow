// i3GuiOpt.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "i3GuiOpt.h"
#include "MainFrm.h"
#include "OptConfig.h"
#include "OptionInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3GuiOptApp

BEGIN_MESSAGE_MAP(Ci3GuiOptApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// Ci3GuiOptApp 생성

Ci3GuiOptApp::Ci3GuiOptApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 Ci3GuiOptApp 개체입니다.

Ci3GuiOptApp theApp;


// Ci3GuiOptApp 초기화

BOOL Ci3GuiOptApp::InitInstance()
{

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("Zepetto"));

	{
		i3MemoryInit();

		i3Error::Init();
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		i3Error::SetChannelName( I3LOG_FATAL, "i3GuiOpt.log");
		i3Error::SetChannelName( I3LOG_WARN, "i3GuiOpt.log");

		i3FrameworkRegisterMetas();

//		COptionInfo::RegisterMeta();
//		COptConfig::RegisterMeta();

		i3TDKRegisterMetas();

		i3ResourceFile::setSearchExternRefEnable( true);
	}

	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	
	return TRUE;
}

Ci3GuiOptApp::~Ci3GuiOptApp()
{
}

int Ci3GuiOptApp::ExitInstance()
{
	i3TDKGlobalRes::Close();					// 종료시에 아예 ExitInstance()와 소멸자가 호출되지 않는 버그가 있음..
	i3System::TerminateSys();					// 아직 미해결 상태로....원인 불명...(2012.09.10.수빈)
	i3ElementBase::DumpObjects();

	return CWinApp::ExitInstance();
}
