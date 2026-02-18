// BAServerDlg.cpp : 구현 파일
//

#include "pch.h"

#ifdef BUILDMODE_DLG

#include "GameDlg.h"
#include "GameDlgDlg.h"
#include ".\gamedlgdlg.h"
#include "ServerContext.h"
#include "UserManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define UPDATE_MAIN			0x00001

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


// CBAServerDlg 대화 상자



CBAServerDlg::CBAServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBAServerDlg::IDD, pParent)
	, m_ui32SessionCount(0)
	, m_i32RoomCount(0)
	, m_ui32ServerTime(0)
	, m_szNotice(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bNotice = FALSE;
}

void CBAServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COUNT_EDIT, m_ui32SessionCount);
	DDX_Text(pDX, IDC_EDIT_ROOMCOUNT, m_i32RoomCount);
	DDX_Text(pDX, IDC_EDIT_SERVERTIME, m_ui32ServerTime);
	if( m_bNotice )
	{
		DDX_Text(pDX, IDC_ED_NOTICE, m_szNotice);
		m_bNotice = FALSE;
	}
}

BEGIN_MESSAGE_MAP(CBAServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnBnClickedUpdateButton)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DUMP_USER_BUTTON, OnBnClickedDumpUserButton)
	ON_BN_CLICKED(IDC_BT_NOTICE, OnBnClickedBtNotice)
END_MESSAGE_MAP()


// CBAServerDlg 메시지 처리기

BOOL CBAServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.


	/////////////////////////////////////////////////////////////////////////////////////////////////
	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	char strTitle[512];
	sprintf( strTitle, "Game Ver:%d.%d.%d.%d (%s %s) %s", VER_GAME, VER_SC_PROTOCOL, VER_DATE, g_GetBuildCount(), g_preMacroDateToInt(), __TIME__, BUILD_VER );
	SetWindowText(strTitle);

	//메모리 초기화 
#if 1
	i3MemoryInit(0);	
#else 
	i3MemoryInit(0,"I3ENGINE_MEMORY");	
	i3mem::SetDumpEnable(TRUE); 
#endif

	//Set Start Path 이부분 고쳐야 한다. 
	{
		char Temp[SERVER_STRING_COUNT]; 
		GetCurrentDirectory(SERVER_STRING_COUNT, Temp); 
		g_SetCurrentPath(Temp); 
	}
	
	INT32 Rv = g_InitServer(); 
	if(Rv != EVENT_ERROR_SUCCESS )
	{
		switch( Rv )
		{
		case SERVER_ERROR_NETWORK		: 			MessageBox("네트워크를 초기화 못했습니다. ","Error", MB_OK);				break; //
		case SERVER_ERROR_CONFIGINI		: 			MessageBox("config.ini 를 읽어 들이지 못하였습니다. ","Error", MB_OK);		break; //
		case SERVER_ERROR_LOGFILE		: 			MessageBox("LogFile을 생성하지 못했습니다. ","Error", MB_OK);				break; //
		case SERVER_ERROR_CONTROL		: 			MessageBox("Control연동 실패.","Error", MB_OK);								break; //
		default							:			MessageBox("알수 없는 이유로 초기화를 못했습니다.","Error", MB_OK);			break;
		//g_InitServer에서 g_StartServer로 작업위치가 변경되었습니다.
		/*case SERVER_ERROR_DATABASE		: 			MessageBox("DataBase 초기화 실패. ","Error", MB_OK);						break; 
		case SERVER_ERROR_CONTEXT		: 			MessageBox("Server Context를 생성하지 못했습니다.","ERROR",MB_OK);			break; 
		case SERVER_ERROR_HOLE			: 			MessageBox("UDP Hole을 생성하지 못했습니다. ", "Error", MB_OK);				break; 
		case SERVER_ERROR_AUTH			: 			MessageBox("AUTH SERVER와 연결을 못했습니다.","Error", MB_OK);				break; 
		case SERVER_ERROR_LOG			: 			MessageBox("LOG SERVER와 연결을 못했습니다.","Error", MB_OK);				break; 
		case SERVER_ERROR_PATH			: 			MessageBox("Current Path를초기화 못했습니다.","Error", MB_OK);				break; 
		case SERVER_ERROR_SYSTEMLOG		: 			MessageBox("System Log를초기화 못했습니다.","Error", MB_OK);				break; 
		case SERVER_ERROR_GAMEGUARD		: 			MessageBox("게임가드 연동에 실패","Error", MB_OK);							break; 
		case SERVER_ERROR_LOAD_QUEST	: 			MessageBox("도전과제 정보 로딩 실패.","Error", MB_OK);						break; 
		case SERVER_ERROR_GIP			: 			MessageBox("GIP연동 실패.","Error", MB_OK);									break; 
		case SERVER_ERROR_CLAN			: 			MessageBox("클랜 서버연동 실패.","Error", MB_OK);							break; 
		case SERVER_ERROR_MESSENGER		: 			MessageBox("메신저 서버연동 실패.","Error", MB_OK);							break;
		case SERVER_ERROR_SHOP_CONTEXT	: 			MessageBox("샵 초기화 실패.", "Error", MB_OK);								break;
		case SERVER_ERROR_DIFFERENT_BUILD_VERSION : MessageBox("Control Server와 버전이 다릅니다.", "Error", MB_OK);			break;*/
		}
		Sleep(500);
		g_DestroyServer( Rv );
		::SendMessage( m_hWnd, WM_CLOSE, NULL, NULL );
	}
	else
	{	
		//Set Timer
		CreateTimer(); 
	}

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CBAServerDlg::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: Add your message handler code here
	
	//Kill Timer
	DeleteTimer(); 	
	
	//Destroy Server 
	g_DestroyServer(SERVER_ERROR_NO);

	//Destroy Memory 
	i3mem::Destroy();
}


void CBAServerDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	KillTimer( UPDATE_MAIN ); 

	if( (nIDEvent & UPDATE_MAIN) == UPDATE_MAIN )
	{		
		g_UpdateServer();
		OnBnClickedUpdateButton(); 
	}
	
	CDialog::OnTimer(nIDEvent);
	SetTimer( UPDATE_MAIN,		g_pContextMain->m_i32UpdateMainTime,	NULL);		
}

void CBAServerDlg::CreateTimer(void)
{
	SetTimer( UPDATE_MAIN,		g_pContextMain->m_i32UpdateMainTime,	NULL);		
	return; 
}

void CBAServerDlg::DeleteTimer(void)
{
	KillTimer( UPDATE_MAIN ); 
	return; 
}

void CBAServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CBAServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CBAServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBAServerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here	
	if(MessageBox("정말 종료하시겠습니까.", "Wanring ", MB_OKCANCEL) != 1)	return; 

	//종료 처리를 합니다. 
	OnOK();
}

void CBAServerDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//if(MessageBox("정말 종료하시겠습니까.", "Wanring ", MB_OKCANCEL) != 1)	return; 
	OnCancel();
}

void CBAServerDlg::OnBnClickedUpdateButton()
{
	// TODO: Add your control notification handler code here
	if( NULL == g_pServerContext ) return;
	m_ui32SessionCount  = g_pServerContext->GetSessionCount();
	m_i32RoomCount		= g_pServerContext->GetRoomCount();
	m_ui32ServerTime	= i3ThreadTimer::GetServerTime();

	UpdateData(false);
}

void CBAServerDlg::OnBnClickedDumpUserButton()
{
	// TODO: Add your control notification handler code here
	//g_pUserManager->Dump(); 
}

void CBAServerDlg::OnBnClickedBtNotice()
{
	// TODO: Add your control notification handler code here
	m_bNotice = TRUE;
	UpdateData( TRUE );
	if( m_szNotice.IsEmpty() ) return;
	if( m_szNotice.GetLength() > NOTICE_STRING_MAX_COUNT - 1) return;
	wchar_t wstrNotice[ NOTICE_STRING_MAX_COUNT ];
	MultiByteToWideChar( CP_ACP, 0, m_szNotice, -1, wstrNotice, NOTICE_STRING_MAX_COUNT );
	g_pUserManager->OnSendNoticeMessage( MAIN_THREAD_IDX, NOTICE_TYPE_EMERGENCY, wstrNotice, i3String::Length( wstrNotice ) );
	m_szNotice.Empty();
	m_bNotice = TRUE;
	UpdateData( FALSE );
}


#endif

