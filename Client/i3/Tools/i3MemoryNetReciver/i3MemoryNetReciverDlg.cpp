// i3MemoryNetReciverDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "i3MemoryNetReciver.h"
#include "i3MemoryNetReciverDlg.h"
#include ".\i3memorynetreciverdlg.h"
#include "UdpRecv.h"

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


// Ci3MemoryNetReciverDlg 대화 상자



Ci3MemoryNetReciverDlg::Ci3MemoryNetReciverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3MemoryNetReciverDlg::IDD, pParent)
	, m_ReadCount(0), m_pUdpRecv(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ci3MemoryNetReciverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MEMORY, m_ListControl);
	DDX_Control(pDX, IDC_BUTTON_RECV_ONOFF, m_OnOffButton);
	DDX_Control(pDX, IDC_PROGRESS_READ, m_ProgressBar);
	DDX_Text(pDX, IDC_EDIT_READ_COUNT, m_ReadCount);
}

BEGIN_MESSAGE_MAP(Ci3MemoryNetReciverDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_RECV_ONOFF, OnBnClickedButtonRecvOnoff)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnBnClickedButtonPrint)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LIST, OnBnClickedButtonClearList)
	ON_BN_CLICKED(IDC_BUTTON_FILE_PRINT, OnBnClickedButtonFilePrint)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

#define UPDATE_MAIN			0x00001

// Ci3MemoryNetReciverDlg 메시지 처리기

BOOL Ci3MemoryNetReciverDlg::OnInitDialog()
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
	//Set List Control 
	{
		LVCOLUMN col; 
		col.mask	= LVCF_TEXT |LVCF_WIDTH; 
		col.pszText	= "Address"; 
		col.cx		= 100; 
		m_ListControl.InsertColumn(0, &col); 

		col.mask	= LVCF_TEXT | LVCF_WIDTH; 
		col.pszText = "Size"; 
		col.cx		= 100; 
		m_ListControl.InsertColumn(1, &col); 

		col.mask	= LVCF_TEXT | LVCF_WIDTH; 
		col.pszText = "Alloc Position";
		col.cx		= 400; 
		m_ListControl.InsertColumn(2, &col);
	}
	m_bRecv = TRUE; 
	m_OnOffButton.SetWindowText("Recv Sleep");

	i3MemoryInit(); 
	//i3Net::Create(); 
	m_pUdpRecv = CUdpRecv::new_object();
	m_pUdpRecv->OnCreate(5557, &m_bRecv, &m_ListControl);  

	SetTimer( UPDATE_MAIN,	100,	NULL);
	m_ProgressBar.SetRange( 0, 100); 

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3MemoryNetReciverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3MemoryNetReciverDlg::OnPaint() 
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
HCURSOR Ci3MemoryNetReciverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3MemoryNetReciverDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void Ci3MemoryNetReciverDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void Ci3MemoryNetReciverDlg::OnBnClickedButtonRecvOnoff()
{
	// TODO: Add your control notification handler code here
	if( m_bRecv )
	{		
		m_bRecv = FALSE; 
		m_OnOffButton.SetWindowText("Recv Active"); 		
	}else 
	{
		m_bRecv = TRUE; 
		m_OnOffButton.SetWindowText("Recv Sleep"); 		
	}
}

void Ci3MemoryNetReciverDlg::OnBnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	m_ListControl.DeleteAllItems(); 
}

void Ci3MemoryNetReciverDlg::OnBnClickedButtonPrint()
{
	// TODO: Add your control notification handler code here
	m_pUdpRecv->dump();
}

void Ci3MemoryNetReciverDlg::OnBnClickedButtonClearList()
{
	// TODO: Add your control notification handler code here
	m_pUdpRecv->ClearList(); 
}

void Ci3MemoryNetReciverDlg::OnBnClickedButtonFilePrint()
{
	// TODO: Add your control notification handler code here
	m_pUdpRecv->DumpFile(); 
}

void Ci3MemoryNetReciverDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if( (nIDEvent & UPDATE_MAIN) == UPDATE_MAIN )
	{
		if( m_pUdpRecv->m_pHeader != NULL)
		{			
			m_ReadCount = m_pUdpRecv->m_pHeader->_idxWrite - m_pUdpRecv->m_pHeader->_idxRead; 
			UpdateData(false); 
			INT32 Value = (INT32) (((float)m_ReadCount / (float)I3_MEM_BUFFER_MAX_COUNT) * 100); 
			m_ProgressBar.SetPos( Value  );	
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void Ci3MemoryNetReciverDlg::OnDestroy()
{
	CDialog::OnDestroy();

	I3_SAFE_RELEASE( m_pUdpRecv);

	// TODO: Add your message handler code here
	KillTimer( UPDATE_MAIN ); 
}
