// AuthDlgDlg.cpp : 구현 파일
//

#include "pch.h"
#include "ServerDlg.h"
#include "ServerDlgDlg.h"
#include ".\serverdlgdlg.h"

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


// CServerDlgDlg 대화 상자



CServerDlgDlg::CServerDlgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlgDlg::IDD, pParent)
	, m_sServerList(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SERVER_LIST, m_sServerList);	
}

BEGIN_MESSAGE_MAP(CServerDlgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CServerDlgDlg 메시지 처리기

BOOL CServerDlgDlg::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	i3MemoryInit(0); 

	//Set Start Path 이부분 고쳐야 한다. 
	char Temp[SERVER_STRING_COUNT]; 
	GetCurrentDirectory(SERVER_STRING_COUNT, Temp); 
	g_SetCurrentPath(Temp); 

	INT32 i32Rv = g_InitServer(); 
	if( EVENT_ERROR_SUCCESS != i32Rv )
	{
		switch( i32Rv )
		{
		case SERVER_ERROR_MEMORY		: MessageBox("메모리 생성에 실패 했습니다.",				"Error", MB_OK);	break;
		case SERVER_ERROR_NETWORK		: MessageBox("네트워크를 초기화 못했습니다.",				"Error", MB_OK);	break;
		case SERVER_ERROR_SYSTEM		: MessageBox("System을 초기화 하지 못했습니다.",			"Error", MB_OK);	break;
		case SERVER_ERROR_CONFIG		: MessageBox("config.ini 를 읽어 들이지 못하였습니다. ",	"Error", MB_OK);	break;
		case SERVER_ERROR_LOGFILE		: MessageBox("LogFile을 생성하지 못했습니다. ",				"Error", MB_OK);	break;

		case SERVER_ERROR_CONTROLSERVER	: MessageBox("ControlServer을 초기화 못했습니다.",			"Error", MB_OK);	break;
		case SERVER_ERROR_TASK			: MessageBox("TaskProcessor를 생성하지 못했습니다.",		"ERROR",MB_OK);		break;

		//case SERVER_ERROR_DATABASE		: MessageBox("DataBase를 연결하지 못했습니다. ","Error", MB_OK);			break;
		//case SERVER_ERROR_CONTEXT		: MessageBox("Server Context를 생성하지 못했습니다.","ERROR",MB_OK);		break;
		//
		//case SERVER_ERROR_NCUBS			: MessageBox("NC_UBS 초기화 못했습니다.","Error", MB_OK);					break;
		//
		//case SERVER_ERROR_LOGD			: MessageBox("엔씨 로그D를초기화 못했습니다.","Error", MB_OK);				break;
		
		default							: MessageBox("알수 없는 이유로 초기화를 못했습니다.","Error", MB_OK);		break; 			
		}
		
		::SendMessage( m_hWnd, WM_CLOSE, NULL, NULL );
	}

	SetTimer( UPDATE_MAIN,		10,	NULL);		
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CServerDlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlgDlg::OnPaint() 
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
HCURSOR CServerDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlgDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(MessageBox("정말 종료하시겠습니까.", "Wanring ", MB_OKCANCEL) != 1)	return; 
	OnOK();
}

void CServerDlgDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//if(MessageBox("정말 종료하시겠습니까.", "Wanring ", MB_OKCANCEL) != 1)	return; 
	OnCancel();
}

void CServerDlgDlg::OnDestroy()
{
	CDialog::OnDestroy();
	KillTimer( UPDATE_MAIN );

	// TODO: Add your message handler code here
	g_DestroyServer();
	i3mem::Destroy();
}

void CServerDlgDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default	

	KillTimer( UPDATE_MAIN );

	if( (nIDEvent & UPDATE_MAIN) == UPDATE_MAIN )
	{		
		if( SERVER_ERROR_SUCCESS != g_UpdateServer() )
		{
			::SendMessage( m_hWnd, WM_CLOSE, NULL, NULL ); 
		}
		//char pServerList[32]; 
		//sprintf(pServerList," 0X%p",g_ConnectServer);  
		//m_sServerList = pServerList;

		//Update Dlg
		UpdateData(false);
	}
	
	SetTimer( UPDATE_MAIN,		10,	NULL);		

	CDialog::OnTimer(nIDEvent);
}
