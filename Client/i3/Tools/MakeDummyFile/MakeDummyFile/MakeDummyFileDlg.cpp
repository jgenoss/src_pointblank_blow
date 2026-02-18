// MakeDummyFileDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MakeDummyFile.h"
#include "MakeDummyFileDlg.h"
#include ".\makedummyfiledlg.h"

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


// CMakeDummyFileDlg 대화 상자



CMakeDummyFileDlg::CMakeDummyFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeDummyFileDlg::IDD, pParent)
	, m_Size(0)
	, m_bFillRandom(FALSE)
	, m_bChecksum(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	srand( GetTickCount());
}

void CMakeDummyFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ED_SIZE, m_Size);
	DDX_Control(pDX, IDC_EDIT2, m_PathCtrl);
	DDX_Check(pDX, IDC_CHECK1, m_bFillRandom);
	DDX_Check(pDX, IDC_CHECK2, m_bChecksum);
}

BEGIN_MESSAGE_MAP(CMakeDummyFileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_CREATE, OnBnClickedBtnCreate)
END_MESSAGE_MAP()


// CMakeDummyFileDlg 메시지 처리기

BOOL CMakeDummyFileDlg::OnInitDialog()
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

	{
		m_PathCtrl.SetWindowText( "D:\\DUMMY.DAT");
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CMakeDummyFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMakeDummyFileDlg::OnPaint() 
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
HCURSOR CMakeDummyFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMakeDummyFileDlg::OnBnClickedBtnCreate()
{
	HANDLE hFile;
	LONG Sz;
	char szPath[MAX_PATH];

	UpdateData( TRUE);

	m_PathCtrl.GetWindowText( szPath, sizeof(szPath) - 1);

	hFile = CreateFile( szPath, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox( "Could not create dummy file.", 0);
		return;
	}

	Sz = m_Size * 1024 * 1024;

	if( m_bFillRandom)
	{
		UINT32 i, j;
		UINT32 * pBuf = (UINT32 *) new char[ 1024 * 1024];
		DWORD rb;

		for( j = 0; j < (UINT32) m_Size; j++)
		{
			for( i = 0; i < (1024 * 1024 / 4); i++)
			{
				pBuf[i] = (UINT32) (rand() | (rand() << 16));
			}

			WriteFile( hFile, pBuf, 1024 * 1024, &rb, NULL);
		}

		delete pBuf;
	}
	else
	{
		SetFilePointer( hFile, Sz, NULL, FILE_BEGIN);

		SetEndOfFile( hFile);
	}

	{
		PutEncriptCode( hFile);
	}

	CloseHandle( hFile);
}

void MakeRandomCode( char * pBuf, INT32 len)
{
	INT32 i;

	for( i = 0; i < len; i++)
	{
		pBuf[i] = (char)(rand() & 0xFF);
	}
}

void CMakeDummyFileDlg::PutEncriptCode( HANDLE hFile)
{
	// 파일의 시작, 중간, 끝 부분의 (임의의) 100 바이트씩을 읽어 CRC를 구한 후,
	// Bit Rotating을 이용한 코드로 대체하고, 파일의 2048 Offset에 저장해 둔다.
	char ReadBuf[128];
	UINT32 crc;
	UINT32 Sz;
	DWORD ReadBytes;

	Sz = SetFilePointer( hFile, 0, NULL, FILE_END);

	// 시작 100 bytes
	{
		MakeRandomCode( ReadBuf, 100);

		SetFilePointer( hFile, 0, NULL, FILE_BEGIN);
		WriteFile( hFile, ReadBuf, 100, &ReadBytes, NULL);

		crc = CRC32( Sz, (UINT8 *)ReadBuf, 100);
	}

	// 중간 100 Bytes
	{
		MakeRandomCode( ReadBuf, 100);

		SetFilePointer( hFile, (Sz >> 1), NULL, FILE_BEGIN);
		WriteFile( hFile, ReadBuf, 100, &ReadBytes, NULL);

		crc = CRC32( crc, (UINT8 *)ReadBuf, 100);
	}

	// 끝 100 Bytes
	{
		MakeRandomCode( ReadBuf, 100);

		SetFilePointer( hFile, -100, NULL, FILE_END);
		WriteFile( hFile, ReadBuf, 100, &ReadBytes, NULL);

		crc = CRC32( crc, (UINT8 *)ReadBuf, 100);
	}

	// Bit Rotating
	BitRotateEncript( (UINT8 *)&crc, 4, 3);

	// CRC의 저장
	{
		SetFilePointer( hFile, 2048, NULL, FILE_BEGIN);
		WriteFile( hFile, &crc, 4, &ReadBytes, NULL);
	}
}
