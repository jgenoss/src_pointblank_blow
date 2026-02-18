// i3VLoggerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3VLogger.h"
#include "i3VLoggerDlg.h"
#include ".\i3vloggerdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3VLoggerDlg 대화 상자



Ci3VLoggerDlg::Ci3VLoggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3VLoggerDlg::IDD, pParent)
{
	m_hIcon = (HICON) ::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDR_MAINFRAME), IMAGE_ICON, 32, 32, 0);

	m_pSocket = NULL;
	m_pFramework = NULL;
}

void Ci3VLoggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ci3VLoggerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// Ci3VLoggerDlg 메시지 처리기

BOOL Ci3VLoggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	{
		CRect rect;

		GetClientRect( &rect);

		rect.left += 3;
		rect.right -= 3;
		rect.top += 3;
		rect.bottom -= 3;

		m_ViewerCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);
	}

	{
		m_pFramework = i3VLoggerFramework::new_object();

		m_pFramework->Create( m_ViewerCtrl.getViewer());

		m_pFramework->setViewerCtrl( &m_ViewerCtrl);
		m_pFramework->SetPanelVisible( false);
	}

	{
		m_pSocket = new CUdpSocket;
		m_pSocket->setFramework( m_pFramework);

		if( !m_pSocket->Create( I3VLOG_PORT, SOCK_DGRAM))
		{
			I3PRINTLOG(I3LOG_FATAL, "Could not create socket.");
			return FALSE;
		}

		int bufsz = 4 * 1024 * 1024;

		if( m_pSocket->SetSockOpt( SO_RCVBUF, &bufsz, sizeof(bufsz), SOL_SOCKET) == 0)
		{
			i3Net::GetError( "SetSockOpt()\n");
			return FALSE;
		}
	}

	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3VLoggerDlg::OnDestroy()
{
	if( m_pSocket != NULL)
		delete m_pSocket;

	I3_SAFE_RELEASE( m_pFramework);

	CDialog::OnDestroy();
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3VLoggerDlg::OnPaint() 
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
HCURSOR Ci3VLoggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3VLoggerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(( cx > 0) && ( ::IsWindow( m_ViewerCtrl.m_hWnd)))
	{
		m_ViewerCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

BOOL Ci3VLoggerDlg::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

BOOL Ci3VLoggerDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN)
	{
		::TranslateMessage( pMsg);
		::DispatchMessage( pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);
}
