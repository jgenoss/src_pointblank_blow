// siadlgDlg.cpp : 구현 파일
//

#include "pch.h"

#ifdef BUILDMODE_DLG

#include "ZLogdlg.h"
#include "ZLogdlgDlg.h"
#include ".\ZLogdlgdlg.h"
#include "Serverdef.h"

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


// CLogdlgDlg 대화 상자



CLogdlgDlg::CLogdlgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogdlgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogdlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLogdlgDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CLogdlgDlg 메시지 처리기
#define UPDATE_TIMER		1

INT32 g_GetBuildCount()
{
	INT32 i32BuildCount = -1;
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (hRsrc == NULL) return -1;

	HGLOBAL hGlobalMemory = LoadResource( NULL, hRsrc );
	if( hGlobalMemory != NULL )
	{
		CString rVersion;
		LPVOID pVersionResouece = LockResource( hGlobalMemory );
		LPVOID pVersion = NULL;  
		DWORD uLength, langD;
		BOOL bRetVal = VerQueryValue(pVersionResouece, _T("\\VarFileInfo\\Translation"), (LPVOID*)&pVersion, (UINT*)&uLength);
		if( bRetVal && 4 == uLength )
		{
			memcpy( &langD, pVersion, 4);
			rVersion.Format( _T("\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion"),
				(langD & 0xff00)>>8, langD & 0xff, (langD & 0xff000000)>>24, (langD & 0xff0000)>>16 );
		}
		else
		{
			rVersion.Format( _T("\\StringFileInfo\\%04X04B0\\FileVersion"), GetUserDefaultLangID() );
		}
		
		if( VerQueryValue( pVersionResouece, rVersion.GetBuffer(0), (LPVOID*)&pVersion, (UINT *)&uLength) != 0 )
		{
			INT32 i32Major, i32Tag, i32SC;
			sscanf( (char*)pVersion, "%d, %d, %d, %d", &i32Major, &i32Tag, &i32SC, &i32BuildCount );
		}
	}
	FreeResource( hGlobalMemory );

	return i32BuildCount;
}

BOOL CLogdlgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(false);
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
	SetIcon(m_hIcon, true);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, false);		// 작은 아이콘을 설정합니다.

	char szTitle[512]; 
	i3String::Format( szTitle, 512, "ZLog Ver:%d.%d.%d.%d (%s %s)", VER_GAME, VER_SC_PROTOCOL, VER_DATE, g_GetBuildCount(), __DATE__, __TIME__ );
	SetWindowText(szTitle);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	{
		char Temp[SERVER_STRING_COUNT]; 
		GetCurrentDirectory(SERVER_STRING_COUNT, Temp); 
		g_SetCurrentPath(Temp); 
	}
			

	INT32 Rv = g_InitServer(); 
	if(Rv != EVENT_SUCCESS )
	{
		switch( Rv )
		{ 
		case SERVER_ERROR_CONFIGINI		:	MessageBox("EVENT_FAIL_CREATE_INI.",			"Error", MB_OK);	break; 
		case SERVER_ERROR_LOGFILE		:	MessageBox("EVENT_FAIL_CREATE_LOG.",			"Error", MB_OK);	break; 
		case SERVER_ERROR_INI_VALUE		:	MessageBox("EVENT_FAIL_CREATE_INI_VALUE.",		"Error", MB_OK);	break; 
		case SERVER_ERROR_CONTROLSERVER	:	MessageBox("EVENT_FAIL_CREATE_CONTROL.",		"Error", MB_OK);	break; 
		case SERVER_ERROR_SYSTEMLOG		:	MessageBox("EVENT_FAIL_CREATE_SYSTEMLOG.",		"Error", MB_OK);	break;
		default							:	MessageBox("Unknown Fail.",						"Error", MB_OK);	break; 	
		//g_InitServer에서 g_StartServer로 작업 위치 변경되었습니다.
		//case SERVER_ERROR_TASK			:	MessageBox("EVENT_FAIL_CREATE_TASK.",			"Error", MB_OK);	break;
		//case SERVER_ERROR_DATABASE		:	MessageBox("EVENT_FAIL_CREATE_DATABASE.",		"Error", MB_OK);	break;
		//case SERVER_ERROR_SERVICELOG	:	MessageBox("EVENT_FAIL_CREATE_SERVICE_LOG.",	"Error", MB_OK);	break; 
		//case SERVER_ERROR_NETWORK		:	MessageBox("EVENT_FAIL_CREATE_NETWORK.",		"Error", MB_OK);	break;
		//case SERVER_ERROR_DATABASE_SQL	:	MessageBox("EVENT_FAIL_DATABASE_SQL.",			"Error", MB_OK);	break;
		}
				
		SendMessage( WM_COMMAND, IDOK );
	}

	SetTimer(UPDATE_TIMER, 100, NULL); 
	
	return true;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 true를 반환합니다.
}

void CLogdlgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLogdlgDlg::OnPaint() 
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
HCURSOR CLogdlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLogdlgDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CLogdlgDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CLogdlgDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if( (nIDEvent & UPDATE_TIMER) == UPDATE_TIMER )
	{		
		g_UpdateServer();		
	}

	CDialog::OnTimer(nIDEvent);
}

void CLogdlgDlg::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer( UPDATE_TIMER ); 

	// TODO: Add your message handler code here
	g_DestroyServer(SERVER_ERROR_NO);
}

#endif