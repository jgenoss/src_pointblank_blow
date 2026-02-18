// siadlgDlg.cpp : 구현 파일
//

#include "pch.h"

#ifdef BUILDMODE_DLG

#include "siadlg.h"
#include "siadlgDlg.h"
#include ".\siadlgdlg.h"
#include "../Serverdef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CsiadlgDlg 대화 상자



CsiadlgDlg::CsiadlgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CsiadlgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsiadlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CsiadlgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CsiadlgDlg 메시지 처리기
#define UPDATE_TIMER		1

BOOL CsiadlgDlg::OnInitDialog()
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

	char strTitle[MAX_PATH]; 
	i3String::Format( strTitle, MAX_PATH, "SIA Ver:%d.%d.%d.%d (%s %s)", VER_GAME, VER_SC_PROTOCOL, VER_DATE, g_GetBuildCount(), __DATE__, __TIME__ );
	SetWindowText(strTitle);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	{
		char strTemp[SERVER_STRING_COUNT];
		GetCurrentDirectory(SERVER_STRING_COUNT, strTemp);
		g_SetCurrentPath(strTemp);
	}


	INT32 i32Rv = g_InitServer();
	if(i32Rv != EVENT_SUCCESS )
	{
		switch( i32Rv )
		{
		case SERVER_ERROR_CONFIGINI					:	MessageBox("EVENT_FAIL_CREATE_INI.",				"Error", MB_OK);	break; 
		case SERVER_ERROR_LOGFILE					: 	MessageBox("EVENT_FAIL_CREATE_LOG.",				"Error", MB_OK);	break; 
		case SERVER_ERROR_INI_VALUE					:	MessageBox("EVENT_FAIL_CREATE_INI_VALUE.",			"Error", MB_OK);	break;
		case SERVER_ERROR_CONTROLSERVER				:	MessageBox("EVENT_FAIL_CREATE_CONTROL.",			"Error", MB_OK);	break; 
		case SERVER_ERROR_SYSTEMLOG					:	MessageBox("EVENT_FAIL_CREATE_SYSTEMLOG.",			"Error", MB_OK);	break;
		case SERVER_ERROR_INIT						:	MessageBox("EVENT_FAIL_LOAD_DLL.",					"Error", MB_OK);	break;
		//g_InitServer에서 -> g_StartServer가 사용하는 부분
		//case SERVER_ERROR_TASK					:	MessageBox("EVENT_FAIL_CREATE_TASK.",				"Error", MB_OK);	break; 		
		//case SERVER_ERROR_DATABASE				:	MessageBox("EVENT_FAIL_CREATE_DATABASE.",			"Error", MB_OK);	break; 
		//case SERVER_ERROR_LINK					:	MessageBox("EVENT_FAIL_CREATE_LINK.",				"Error", MB_OK);	break; 		
		//case SERVER_ERROR_NETWORK					:	MessageBox("EVENT_FAIL_CREATE_NETWORK.",			"Error", MB_OK);	break; 
		//case SERVER_ERROR_SHOP					:	MessageBox("EVENT_FAIL_CREATE_SHOP.",				"Error", MB_OK);	break; 
		//case SERVER_ERROR_RUSSA_INI				:	MessageBox("EVENT_FAIL_INITILIZE_innbill.",			"Error", MB_OK);	break;
		//case SERVER_ERROR_DIFFERENT_BUILD_VERSION	:	MessageBox("EVENT_FAIL_DIFFERENT_BUILD_VERSION.",	"Error", MB_OK);	break;
		//case SERVER_ERROR_MODULE_DATABASE			:	MessageBox("EVENT_FAIL_CREATE_DATABASE_MODULE",		"Error", MB_OK);	break;
		//case SERVER_ERROR_MODULE_NETWORK			:	MessageBox("EVENT_FAIL_CREATE_NETWORK_MODULE",		"Error", MB_OK);	break;
		default										: 	MessageBox("Unknown Fail.",							"Error", MB_OK);	break; 			
		}

		// 에러가 났다면 컨트롤 서버로 전달
		//무조건 전달로 변경.
		/*NET_I3_PACKET_BUFFER	SendErrorPacket(PROTOCOL_ASC_SERVER_LOADING_RESULT);
		SendErrorPacket.WriteData(&Rv, sizeof(INT32));
		g_pModuleControl->SendPacketMessage((char*)&SendErrorPacket, SendErrorPacket._ui16TotalSize);*/

		Sleep(500);
	//	g_DestroyServer(i32Rv); 
		SendMessage( WM_COMMAND, IDOK );
	}

	SetTimer(UPDATE_TIMER, 100, NULL); 
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CsiadlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CsiadlgDlg::OnPaint() 
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
HCURSOR CsiadlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CsiadlgDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CsiadlgDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if(MessageBox("정말 종료하시겠습니까.", "Wanring ", MB_OKCANCEL) != 1)	return; 
	OnCancel();
}

void CsiadlgDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if( (nIDEvent & UPDATE_TIMER) == UPDATE_TIMER )
	{
		g_UpdateServer();
	}

	CDialog::OnTimer(nIDEvent);
}

void CsiadlgDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	g_DestroyServer(EVENT_SUCCESS);
}

#endif