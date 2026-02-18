// ExpectionCheckDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ExpectionCheck.h"
#include "ExpectionCheckDlg.h"
#include ".\expectioncheckdlg.h"

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


// CExpectionCheckDlg 대화 상자



CExpectionCheckDlg::CExpectionCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExpectionCheckDlg::IDD, pParent)
	, m_szEMapFile(_T(""))
	, m_szEAddress(_T(""))
	, m_szECodeResult(_T(""))
	, m_szECode(_T(""))
	, m_szEInput(_T(""))
	, m_szEOS(_T(""))
	, m_szECPU(_T(""))
	, m_szEGPUCard(_T(""))
	, m_szEGPUDate(_T(""))
	, m_szEGPUVer(_T(""))
	, m_szEGPUProv(_T(""))
	, m_szMEMTotalPhys(_T(""))
	, m_szMEMAvailPhys(_T(""))
	, m_szMEMTotalVir(_T(""))
	, m_szMEMAvailVir(_T(""))
	, m_szEMultMedia(_T(""))
	, m_szEDirectX(_T(""))	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExpectionCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_E_MAPFILE, m_szEMapFile);
	DDX_Text(pDX, IDC_E_ADDRESS, m_szEAddress);
	DDX_Control(pDX, IDC_L_CALLSTACK, m_ctLCallStack);
	DDX_Control(pDX, IDC_L_PROCESS, m_ctLProcess);
	DDX_Text(pDX, IDC_E_CODERESULT, m_szECodeResult);
	DDX_Text(pDX, IDC_E_CODE, m_szECode);
	DDX_Text(pDX, IDC_E_INPUT, m_szEInput);
	DDX_Text(pDX, IDC_E_OS, m_szEOS);
	DDX_Text(pDX, IDC_E_CPU, m_szECPU);
	DDX_Text(pDX, IDC_E_GPU_CARD, m_szEGPUCard);
	DDX_Text(pDX, IDC_E_GPU_DATE, m_szEGPUDate);
	DDX_Text(pDX, IDC_E_GPU_VER, m_szEGPUVer);
	DDX_Text(pDX, IDC_E_GPU_PROV, m_szEGPUProv);
	DDX_Text(pDX, IDC_E_MEM_TOTALPHYS, m_szMEMTotalPhys);
	DDX_Text(pDX, IDC_E_MEM_AVAILPHYS, m_szMEMAvailPhys);
	DDX_Text(pDX, IDC_E_MEM_TOTALVIR, m_szMEMTotalVir);
	DDX_Text(pDX, IDC_E_MEM_AVAILVIR, m_szMEMAvailVir);
	DDX_Text(pDX, IDC_E_MULTMEDIA, m_szEMultMedia);
	DDX_Text(pDX, IDC_E_DIRECTX, m_szEDirectX);
}

BEGIN_MESSAGE_MAP(CExpectionCheckDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_B_MAPOPEN, OnBnClickedBMapopen)
	ON_BN_CLICKED(IDC_B_CALLCATCH, OnBnClickedBCallcatch)
	ON_BN_CLICKED(IDC_B_CLEAN, OnBnClickedBClean)
	ON_BN_CLICKED(IDC_B_CODE, OnBnClickedBCode)
	ON_BN_CLICKED(IDC_B_INFO, OnBnClickedBInfo)
END_MESSAGE_MAP()


// CExpectionCheckDlg 메시지 처리기

BOOL CExpectionCheckDlg::OnInitDialog()
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
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CExpectionCheckDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CExpectionCheckDlg::OnPaint() 
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
HCURSOR CExpectionCheckDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CExpectionCheckDlg::OnBnClickedBMapopen()
{
	// TODO: Add your control notification handler code here
	char szFilters[260];
 	strcpy(szFilters, "Map파일(*.map)|*.map|모든파일 (*.*)|*.*||");
	CFileDialog FileDlg(TRUE, "", "", OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	CString lpszPathName;

	if( FileDlg.DoModal ()==IDOK )
	{
		m_szEMapFile = FileDlg.GetPathName();		
	}
	UpdateData(FALSE);
}

void CExpectionCheckDlg::OnBnClickedBCallcatch()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);	

	SetCallStack( m_szEAddress );
}

void CExpectionCheckDlg::OnBnClickedBClean()
{
	// TODO: Add your control notification handler code here
	m_szECode.Empty();
	m_szEAddress.Empty();
	m_szECodeResult.Empty();
	m_szEInput.Empty();

	m_szEOS.Empty();
	m_szECPU.Empty();
	m_szEGPUCard.Empty();
	m_szEGPUDate.Empty();
	m_szEGPUVer.Empty();
	m_szEGPUProv.Empty();
	m_szMEMTotalPhys.Empty();
	m_szMEMAvailPhys.Empty();
	m_szMEMTotalVir.Empty();
	m_szMEMAvailVir.Empty();
	m_szEMultMedia.Empty();
	m_szEDirectX.Empty();
	m_ctLCallStack.ResetContent();
	m_ctLProcess.ResetContent();

	UpdateData(FALSE);
}

void CExpectionCheckDlg::OnBnClickedBCode()
{
	// TODO: Add your control notification handler code here	
	UpdateData( TRUE );

	SetErroeCode( m_szECode );
	UpdateData( FALSE );	
}


void CExpectionCheckDlg::OnBnClickedBInfo()
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	
	GetOS();
	GetCPU();
	GetGPU();
	GetMemory();
	GetMultimedia();
	GetDirectX();
	GetProcess();
	GetErrorCode();
	GetCallStack();
	
	UpdateData( FALSE );
}

void	CExpectionCheckDlg::GetOS()
{
	m_szEOS = GetPrivateString( m_szEInput, "[O/S info]", "OS" );
}

void	CExpectionCheckDlg::GetCPU()
{
	m_szECPU = GetPrivateString( m_szEInput, "[CPU info]", "CPU" );
}

void	CExpectionCheckDlg::GetGPU()
{
	m_szEGPUCard =	GetPrivateString( m_szEInput, "[GPU info]", "Vedio Card" );
	m_szEGPUDate =	GetPrivateString( m_szEInput, "[GPU info]", "Vedio Card Driver Date" );
	m_szEGPUVer =	GetPrivateString( m_szEInput, "[GPU info]", "Vedio Card Driver Version" );
	m_szEGPUProv =	GetPrivateString( m_szEInput, "[GPU info]", "Vedio Card Driver Provider" );
}

void	CExpectionCheckDlg::GetMemory()
{
	m_szMEMTotalPhys =	GetPrivateString( m_szEInput, "[Memory Info]", "메인 메모리 총 크기" );
	m_szMEMAvailPhys =	GetPrivateString( m_szEInput, "[Memory Info]", "메인 메모리 가용 크기" );
	m_szMEMTotalVir =	GetPrivateString( m_szEInput, "[Memory Info]", "가상 메모리 총 사이즈" );;
	m_szMEMAvailVir =	GetPrivateString( m_szEInput, "[Memory Info]", "가용 가상 메모리 총 사이즈" );;

}

void	CExpectionCheckDlg::GetMultimedia()
{
	m_szEMultMedia = GetPrivateString( m_szEInput, "[Multimedia info]", "Multimedia" );
}

void	CExpectionCheckDlg::GetDirectX()
{
	m_szEDirectX = GetPrivateString( m_szEInput, "[DirectX info]", "DirectX" );
}

void	CExpectionCheckDlg::GetProcess()
{
	char szCount[64];
	int i = 1;
	CString szTemp;
	while(true) {
		sprintf( szCount, "%02d", i);
		szTemp = GetPrivateString( m_szEInput, "[System Process]", szCount );

		if( szTemp.IsEmpty())	break;

		m_ctLProcess.AddString( szTemp );
		i++;
	}
}

void	CExpectionCheckDlg::GetErrorCode()
{
	CString szTemp;
	szTemp = GetPrivateString( m_szEInput, "[Error Code]", "Code" );
	SetErroeCode( szTemp );
}

void	CExpectionCheckDlg::GetCallStack()
{
	char szCount[64];
	int i = 1;
	CString szTemp;
	while(true) {
		sprintf( szCount, "%02d", i);
		szTemp = GetPrivateString( m_szEInput, "[Call Stack]", szCount );

		if( szTemp.IsEmpty())	break;

		SetCallStack( szTemp );
		i++;
	}
}

CString	CExpectionCheckDlg::GetPrivateString( CString szEInput, char* pszSection, char* pszKey )
{
	CString szSection, szKey, szTemp, szValue, szKeyTemp;
	int		i = 0;
	bool	bFindSection = false;

	szSection.Format("%s", pszSection);
	szKey.Format("%s", pszKey);

	bool	bEnd = true;
	while (bEnd) {
		bEnd = AfxExtractSubString(szTemp, szEInput, i, '\r\n');
		if( !bFindSection ) {
			if( szTemp.Find( szSection ) != -1 ) {
				bFindSection = true;
			}
		}
		else {
			if( szTemp.Find( '[' ) != -1 && szTemp.Find( ']' ) != -1) return NULL;

			szKeyTemp = szTemp.Mid( 0, szTemp.Find('='));
			szKeyTemp.Replace(" ", "");
			szKey.Replace(" ", "");

			if( szKeyTemp.Find( szKey ) != -1 ) {
				szValue = szTemp.Mid( szTemp.Find('=') + 1);
				szValue.Replace(" ", "");
				return szValue;
			}
		}
		i++;
	}

	return NULL;
}

void	CExpectionCheckDlg::SetCallStack( CString szAddress )
{
	if(m_szEMapFile.IsEmpty() || szAddress.IsEmpty()) return;

	//i3MemoryInit();

	i3MapFileParser cMapFile;
	if(cMapFile.CheckExpection( m_szEMapFile.GetBuffer(), szAddress.GetBuffer() )) {

		char szFunction[260];
		sprintf( szFunction, "%s() :: %s, Line : %d -- %s",
			cMapFile.GetFunctionName(), cMapFile.GetClassName(), cMapFile.GetSourceLine(), szAddress.GetBuffer());

		m_ctLCallStack.AddString( szFunction );
	}
}


void	CExpectionCheckDlg::SetErroeCode( CString szCode )
{
	char s_szCode[20][260] = {
		"EXCEPTION_ACCESS_VIOLATION\r\n - 허가되지 않은 메모리 영역을 읽거나 쓰려고 시도했습니다.",
		"EXCEPTION_ARRAY_BOUNDS_EXCEEDED\r\n - 배열의 범위를 초과하였습니다.",
		"EXCEPTION_BREAKPOINT\r\n - 중단점을 만났습니다.",
		"EXCEPTION_DATATYPE_MISALIGNMENT\r\n - 정렬을 지원하지 않는 하드웨어에 잘못 정렬된 값을 읽거나 쓰려고 하였습니다.",
		"EXCEPTION_FLT_DENORMAL_OPERAND\r\n - 부동 소수점 연산의 피 연산자중 하나가 잘못되었습니다.",
		"EXCEPTION_FLT_DIVIDE_BY_ZERO\r\n - 실수를 0으로 나우었습니다.",
		"EXCEPTION_FLT_INEXACT_RESULT\r\n - 부동 소수점 연산의 결과 정수부를 정확하게 표현할 수 없습니다.",
		"EXCEPTION_FLT_INVALID_OPERATION\r\n - 알 수 없는 부동 소수점 예외입니다.",
		"EXCEPTION_FLT_OVERFLOW\r\n - 연산의 결과가 허용된 범위보다 더 큽니다.",
		"EXCEPTION_FLT_STACK_CHECK\r\n - 연산의 결과 스택이 오버 플로우 되었거나 언더 플로우 되었습니다.",
		"EXCEPTION_FLT_UNDERFLOW\r\n - 연산의 결과가 허용된 범위보다 더 작습니다.",
		"EXCEPTION_ILLEGAL_INSTRUCTION\r\n - The thread tried to execute an invalid instruction. ",
		"EXCEPTION_IN_PAGE_ERROR\r\n - The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network. ",
		"EXCEPTION_INT_DIVIDE_BY_ZERO\r\n - 0으로 나누기 정수 연산을 하였습니다.",
		"EXCEPTION_INT_OVERFLOW\r\n - 정수 연산의 결과가 허용된 범위보다 더 큽니다.",
		"EXCEPTION_INVALID_DISPOSITION\r\n - An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception. ",
		"EXCEPTION_NONCONTINUABLE_EXCEPTION\r\n - 실행을 계속할 수 없는 예외를 다시 실행할려고 하였습니다.",
		"EXCEPTION_PRIV_INSTRUCTION\r\n - 기계모드에서 사용할 수 없는 명령을 실행하려고 하였습니다.",
		"EXCEPTION_SINGLE_STEP\r\n - 단계 실행을 위한 신호입니다.",
		"EXCEPTION_STACK_OVERFLOW\r\n - The thread used up its stack. "
	};

	DWORD dwCode = 0;

	sscanf(szCode.GetBuffer(), "0x%08X", &dwCode);

	switch(dwCode) {
		case EXCEPTION_ACCESS_VIOLATION:
			m_szECodeResult.Format( s_szCode[0] );
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			m_szECodeResult.Format( s_szCode[1] );
			break;
		case EXCEPTION_BREAKPOINT:
			m_szECodeResult.Format( s_szCode[2] );
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			m_szECodeResult.Format( s_szCode[3] );
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			m_szECodeResult.Format( s_szCode[4] );
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			m_szECodeResult.Format( s_szCode[5] );
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			m_szECodeResult.Format( s_szCode[6] );
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			m_szECodeResult.Format( s_szCode[7] );
			break;
		case EXCEPTION_FLT_OVERFLOW:
			m_szECodeResult.Format( s_szCode[8] );
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			m_szECodeResult.Format( s_szCode[9] );
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			m_szECodeResult.Format( s_szCode[10] );
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			m_szECodeResult.Format( s_szCode[11] );
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			m_szECodeResult.Format( s_szCode[12] );
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			m_szECodeResult.Format( s_szCode[13] );
			break;
		case EXCEPTION_INT_OVERFLOW:
			m_szECodeResult.Format( s_szCode[14] );
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			m_szECodeResult.Format( s_szCode[15] );
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			m_szECodeResult.Format( s_szCode[16] );
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			m_szECodeResult.Format( s_szCode[17] );
			break;
		case EXCEPTION_SINGLE_STEP:
			m_szECodeResult.Format( s_szCode[18] );
			break;
		case EXCEPTION_STACK_OVERFLOW:
			m_szECodeResult.Format( s_szCode[19] );
			break;
	}
}