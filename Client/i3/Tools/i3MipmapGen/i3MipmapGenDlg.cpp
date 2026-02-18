// i3MipmapGenDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3MipmapGen.h"
#include "i3MipmapGenDlg.h"
#include ".\i3mipmapgendlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void NormalizeCmdArg( char *szOut, const char *szIn )
{
	while( *szIn )
	{
		if( *szIn != '"' )
		{
			*szOut = *szIn;
			szOut++;
		}
		szIn++;
	}
	*szOut = 0;
}

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


// Ci3MipmapGenDlg 대화 상자



Ci3MipmapGenDlg::Ci3MipmapGenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3MipmapGenDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_szCmdLineArg = NULL;
}

void Ci3MipmapGenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CTRL, m_ctrlMipmapList);
}

BEGIN_MESSAGE_MAP(Ci3MipmapGenDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_VKEYTOITEM()
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CTRL, OnLvnKeydownListCtrl)
END_MESSAGE_MAP()

// Ci3MipmapGenDlg 메시지 처리기

BOOL Ci3MipmapGenDlg::OnInitDialog()
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
	{
		LVCOLUMN col;

		//에러 종류
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		col.pszText = "Texture Name";
		col.cx = 415;
		m_ctrlMipmapList.InsertColumn( 0, &col);

		//에러 내용
		col.pszText = "Width";
		col.cx = 70;
		m_ctrlMipmapList.InsertColumn( 1, &col);

		col.pszText = "Height";
		col.cx = 70;
		m_ctrlMipmapList.InsertColumn( 2, &col);

		col.pszText = "Depth";
		col.cx = 70;
		m_ctrlMipmapList.InsertColumn( 3, &col);

		col.pszText = "MipMap Level ";
		col.cx = 90;
		m_ctrlMipmapList.InsertColumn( 4, &col);

		col.pszText = "Size ";
		col.cx = 70;
		m_ctrlMipmapList.InsertColumn( 5, &col);
	}

	if( m_szCmdLineArg != NULL )
	{
		if( m_szCmdLineArg[0] != 0 )
		{
			char szFileName[MAX_PATH];
			NormalizeCmdArg( szFileName, m_szCmdLineArg );
			//MessageBox( szFileName );
			if( AddImage( szFileName ) )
			{
				char szWindowText[ MAX_PATH + 32 ];
				sprintf( szWindowText, "i3ImageViewer - \"%s\"", szFileName );
			}
		}
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3MipmapGenDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3MipmapGenDlg::OnPaint() 
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
HCURSOR Ci3MipmapGenDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//void Ci3MipmapGenDlg::OnLbnSelchangeList1()
//{
//	// TODO: Add your control notification handler code here
//}
void Ci3MipmapGenDlg::OnDropFiles(HDROP hDropInfo)
{
	char szTemp[256];

	int count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	for(int i=0; i<count; i++)
	{
		DragQueryFile( hDropInfo, i, szTemp, sizeof(szTemp) - 1);
		AddImage( szTemp );
	}

	CDialog::OnDropFiles(hDropInfo);
}

int TexCompareFunc( const void *arg1, const void *arg2 )
{
	TEXINFO *p1 = (TEXINFO *)arg1;
	TEXINFO *p2 = (TEXINFO *)arg2;
	i3Texture *pTex1 = p1->pTex;
	i3Texture *pTex2 = p2->pTex;

	return int( pTex1->GetDataSize() ) - int( pTex2->GetDataSize() );
}

BOOL Ci3MipmapGenDlg::AddImage( const char *szPath )
{
	// TODO: Add your message handler code here and/or call default
	int nListCount;
	i3ImageFile file;
	char szTemp[256];

	nListCount = m_ctrlMipmapList.GetItemCount();
	if( nListCount >= 8 )
	{
		MessageBox( "Overflow!!" );
		return FALSE;
	}

	// 리스트
	i3Texture *pTempTex = file.Load( szPath );
	if( pTempTex == NULL )
	{
		MessageBox( "Could not open file." );
		return FALSE;
	}
	m_TexInfo[nListCount].pTex = pTempTex;
	strcpy( m_TexInfo[nListCount].szPath, szPath );
	nListCount++;

	// 데이터 크기 순으로 정렬
	qsort( m_TexInfo, nListCount, sizeof(TEXINFO), TexCompareFunc );

	// List에 올라간 리스트를 모두 삭제합니다.
	m_ctrlMipmapList.DeleteAllItems();

	int i;
	// List에 모두 추가
	for(i = 0; i < nListCount; i++)
	{
		m_ctrlMipmapList.InsertItem( i, m_TexInfo[i].szPath );
		i3Texture *pTex = m_TexInfo[i].pTex;

		sprintf(szTemp,"%d", pTex->GetWidth());
		m_ctrlMipmapList.SetItemText( i, 1, szTemp );

		sprintf(szTemp,"%d", pTex->GetHeight());
		m_ctrlMipmapList.SetItemText( i, 2, szTemp );		

		sprintf(szTemp,"%d", i3Gfx::GetImageFormatBitCount( pTex->GetPersistFormat()));
		m_ctrlMipmapList.SetItemText( i, 3, szTemp );

		sprintf(szTemp,"%d", pTex->GetLevelCount());
		m_ctrlMipmapList.SetItemText( i, 4, szTemp );

		sprintf(szTemp,"%d", pTex->GetDataSize());
		m_ctrlMipmapList.SetItemText( i, 5, szTemp );
	}

	strcpy( m_szListFileName, m_TexInfo[ nListCount-1 ].szPath );

	return TRUE;
}

void Ci3MipmapGenDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
	int i, nListCount = 0;
	UINT nDataSize = 0;
	i3Texture * pTexList[8];
	i3Texture * pDestTex;
	i3ImageFile file;	
	char* szOutFileName;
	i3OptI3Image i3Image;

	nListCount = m_ctrlMipmapList.GetItemCount();
	if(nListCount==0) return;

	strcpy(m_szInputFileName, m_szListFileName);

	for( i = 0; i < nListCount; i++ )
	{
		pTexList[i] = m_TexInfo[i].pTex;
	}

	pDestTex = i3Image.MakeI3IMipmap( pTexList, nListCount);

	// 리스트로 부터 받은 파일명을 확장자만 바꾸어 파일로 생성합니다.
 	i3String::RemoveExt(m_szInputFileName);
	szOutFileName = i3String::InsertStr( m_szInputFileName, ".i3i", strlen( m_szInputFileName)  , 4);
	file.SaveI3I(szOutFileName, pDestTex);

	// 생성되어진 밉맵 텍스쳐를 해제합니다.
	pDestTex->Release();

	// List에 올라간 리스트를 모두 삭제합니다.
	m_ctrlMipmapList.DeleteAllItems();
}

int Ci3MipmapGenDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnVKeyToItem(nKey, pListBox, nIndex);
}

void Ci3MipmapGenDlg::OnLvnKeydownListCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	
	if(pLVKeyDow->wVKey == 46 || pLVKeyDow->wVKey == 8)
	{
		UINT nListCount;
		nListCount = m_ctrlMipmapList.GetItemCount();
		for(UINT i = 0; i < nListCount; i++)
		{
			m_ctrlMipmapList.DeleteItem(0);
		}
	}
}

BOOL Ci3MipmapGenDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN)
	{
		TranslateMessage( pMsg);

		DispatchMessage( pMsg);

		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

