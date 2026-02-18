// i3DumpDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3Dump.h"
#include "i3DumpDlg.h"
#include ".\i3dumpdlg.h"

#include "i3Base/string/ext/extract_filename.h"

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


// Ci3DumpDlg 대화 상자



Ci3DumpDlg::Ci3DumpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3DumpDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ci3DumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_ENCRIPT, m_CHK_Encript);
	DDX_Control(pDX, IDC_CHK_DUMP_SOURCE, m_CHK_DumpCPP);
}

BEGIN_MESSAGE_MAP(Ci3DumpDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// Ci3DumpDlg 메시지 처리기

BOOL Ci3DumpDlg::OnInitDialog()
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
		CRect rect;

		GetClientRect( &rect);

		rect.left += 3;
		rect.top += 23;
		rect.right -= 3;
		rect.bottom -= 3;

		m_LogCtrl.Create( WS_CHILD | WS_VISIBLE, rect, this, 100);
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3DumpDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3DumpDlg::OnPaint() 
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
HCURSOR Ci3DumpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3DumpDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LogCtrl.m_hWnd))
	{
		m_LogCtrl.SetWindowPos( NULL, 3, 23, cx - 6, cy - 23, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Ci3DumpDlg::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	m_bEncript =	(m_CHK_Encript.GetCheck() == BST_CHECKED);
	m_bDumpToCPP =	(m_CHK_DumpCPP.GetCheck() == BST_CHECKED);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		DumpFile( TempName);
	}

	DragFinish( hDropInfo);

	CDialog::OnDropFiles(hDropInfo);
}

void Ci3DumpDlg::DumpFile( char * pszPath)
{
	UINT8 * pBuff = NULL;
	UINT32 sz;

	// Read
	{
		i3FileStream strm;

		if( strm.Open( pszPath, STREAM_READ) == FALSE)
		{
			m_LogCtrl.AddText( "* Could not open %s file.\r\n", pszPath);
			return;
		}

		sz = strm.GetSize();

		if( sz == 0)
		{
			m_LogCtrl.AddText( "* File size is 0. (%s)\r\n", pszPath);
			return;
		}

		pBuff = (UINT8 *) i3MemAlloc( sz);
		if( pBuff == NULL)
		{
			m_LogCtrl.AddText( "* Could not allocate memory for %s file. (%d bytes).\r\n", pszPath, sz);
			goto ExitPart;
		}

		strm.Read( pBuff, sz);
	}

	// Encript
	if( m_bEncript)
	{
		BitRotateEncript( (UINT8 *) pBuff, sz, 3);
	}

	{
		char szPath[MAX_PATH];
		i3FileStream strm;

		sprintf( szPath, "%s.dump", pszPath);

		if( strm.Create( szPath, STREAM_WRITE) == FALSE)
		{
			m_LogCtrl.AddText( "* Could not %s file.\r\n", szPath);
			goto ExitPart;
		}

		strm.Write( pBuff, sz);

		strm.Close();

		m_LogCtrl.AddText( "%s\r\n", szPath);
	}

	// Dump to source
	if( m_bDumpToCPP)
	{
		char szPath[MAX_PATH], conv[512], szName[MAX_PATH];
		i3FileStream strm;
		UINT32  accm, line;

	//	i3String::SplitFileName( pszPath, szName, FALSE);
		i3::extract_filetitle( pszPath, szName);

		sprintf( szPath, "%s.cpp", pszPath);

		if( strm.Create( szPath, STREAM_WRITE) == FALSE)
		{
			m_LogCtrl.AddText( "* Could not create %s file\n", szPath);
			goto ExitPart;
		}

		strm.Printf( conv, "unsigned char g_%s[%d] = \n{\n", szName, sz);

		for( accm = 0, line = 0; accm < sz; accm ++)
		{
			if( line == 0)
			{
				strm.Printf( conv, "\t");
			}

			strm.Printf( conv, "0x%02X", (UINT8) pBuff[ accm]);
			
			if( accm < (sz - 1))
			{
				strm.Printf( conv, ", ");
			}

			line++;
			if( line == 16)
			{
				line = 0;
				strm.Printf( conv, "\n");
			}
		}

		strm.Printf( conv, "\n};\n\n");
		strm.Close();
	}

	m_LogCtrl.AddText( "%s file has been dumped.\r\n", pszPath);

ExitPart:
	if( pBuff != NULL)
	{
		i3MemFree( pBuff);
	}
	
}

