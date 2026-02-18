// i3ImageMakerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3ImageMaker.h"
#include "i3ImageMakerDlg.h"
#include "i3Profile.h"
#include ".\i3imagemakerdlg.h"
#include "i3Base/string/ext/generic_string_ncat.h"
#include "i3Base/string/ext/remove_ext.h"


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


// Ci3ImageMakerDlg 대화 상자



Ci3ImageMakerDlg::Ci3ImageMakerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3ImageMakerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_szCmdLineArg = NULL;
	m_pCtx = NULL;
}

void Ci3ImageMakerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PLATFORM, m_ctrPlatformCB);
	DDX_Control(pDX, IDC_CB_COMPRESS, m_CompressCtrl);
}

BEGIN_MESSAGE_MAP(Ci3ImageMakerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_WM_VKEYTOITEM()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_MIPMAP, OnBnClickedCheckMipmap)
END_MESSAGE_MAP()

// Ci3ImageMakerDlg 메시지 처리기

BOOL Ci3ImageMakerDlg::OnInitDialog()
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
		i3VideoInfo fmt;

		m_pCtx = i3RenderContext::new_object();

		fmt.m_bFullScreen = FALSE;
		fmt.m_AdapterIndex = 0;
		fmt.m_bInterlaced = FALSE;
		fmt.m_Width = 100;
		fmt.m_Height = 100;

		m_pCtx->Create( &fmt, m_hWnd);
	}

	m_ctrPlatformCB.AddString( "Unknown" );
	m_ctrPlatformCB.AddString( "DirectX" );
	m_ctrPlatformCB.AddString( "PSP" );

	//	Mipmap option - komet
	{
		char szDefault[256] = "";
		
		CEdit * pEditMipmapCX = (CEdit*) GetDlgItem( IDC_EDIT_MIPMAP_CX);
		CEdit * pEditMipmapCY = (CEdit*) GetDlgItem( IDC_EDIT_MIPMAP_CY);

		CEdit * pEditMaxLevel = (CEdit*) GetDlgItem( IDC_EDIT_MAXLEVEL);

		sprintf( szDefault, "%d", 32);
		pEditMipmapCX->SetWindowText( szDefault);

		sprintf( szDefault, "%d", 32);
		pEditMipmapCY->SetWindowText( szDefault);

		pEditMaxLevel->SetWindowText( "4");
	}

	UpdateData( FALSE );

	i3Profile pf( this );
	m_TargetPlatform = (I3_PLATFORM) pf.GetInt( "TargetPlatform", I3_PLATFORM_WINDOWS );
	m_CompressMethod = pf.GetInt( "CompressMethod", 0);
	UpdateControls( FALSE );

	if( m_szCmdLineArg != NULL )
	{
		if( m_szCmdLineArg[0] != 0 )
		{
			char szFileName[MAX_PATH];
			NormalizeCmdArg( szFileName, m_szCmdLineArg );
			//MessageBox( szFileName );
			printf( "Converting... %s\n", szFileName );
			if( Convert( szFileName ) )
			{
				MessageBox( "Success." );
			}
			else
			{
				MessageBox( "Failed." );
			}
			OnOK();
		}
	}
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3ImageMakerDlg::OnDestroy()
{
	I3_SAFE_RELEASE( m_pCtx);

	UpdateControls( TRUE );

	i3Profile pf( this );
	pf.WriteInt( "TargetPlatform", m_TargetPlatform );
	pf.WriteInt( "CompressMethod", m_CompressMethod);

	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void Ci3ImageMakerDlg::UpdateControls( BOOL bTrue )
{
	if( bTrue )
	{
		UpdateData( TRUE );

		switch( m_ctrPlatformCB.GetCurSel() )
		{
		case 1:	m_TargetPlatform = I3_PLATFORM_WINDOWS; break;
		case 2:	m_TargetPlatform = I3_PLATFORM_PSP;		break;
		default: m_TargetPlatform = I3_PLATFORM_UNKNOWN; break;
		}

		m_CompressMethod = m_CompressCtrl.GetCurSel();
	}
	else
	{
		switch( m_TargetPlatform )
		{
		case I3_PLATFORM_WINDOWS:	m_ctrPlatformCB.SetCurSel( 1 ); break;
		case I3_PLATFORM_PSP:		m_ctrPlatformCB.SetCurSel( 2 ); break;
		default: m_ctrPlatformCB.SetCurSel( 0 ); break;
		}

		m_CompressCtrl.SetCurSel( m_CompressMethod);

		UpdateData( FALSE );
	}
}

void Ci3ImageMakerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3ImageMakerDlg::OnPaint() 
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
HCURSOR Ci3ImageMakerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//void Ci3ImageMakerDlg::OnLbnSelchangeList1()
//{
//	// TODO: Add your control notification handler code here
//}
void Ci3ImageMakerDlg::OnDropFiles(HDROP hDropInfo)
{
	UpdateControls();

	CString msg;
	char szTemp[256];

	int count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	int nSuccess = 0;

	for(int i=0; i<count; i++)
	{
		DragQueryFile( hDropInfo, i, szTemp, sizeof(szTemp) - 1);
		if( Convert( szTemp ) )
		{
			nSuccess++;
		}
		else
		{
			msg.Format( "Failed to converting file. (%s)", szTemp );
			MessageBox( LPCTSTR(msg), "Error" );
		}
	}

	msg.Format( "Success/Total : %d/%d",
				nSuccess, count );
	MessageBox( LPCTSTR( msg ), "Result" );

	CDialog::OnDropFiles(hDropInfo);
}

BOOL Ci3ImageMakerDlg::Convert( const char *szPath )
{
	// 원본 로딩
	i3ImageFile file;
	i3Texture *pTexture = file.Load( szPath );
	if( pTexture == NULL )
	{
		MessageBox( "Could not open file." );
		return FALSE;
	}

	i3Texture * pTexList[8];
	i3Texture * pDestTex;
//	char* szOutFileName;
	i3OptI3Image i3Image;

	pTexList[0] = pTexture;

	//pDestTex = i3Image.MakeI3I( pTexture );

	pDestTex =  pTexture;

	if( m_TargetPlatform == I3_PLATFORM_PSP )
	{
		i3OptPSPSwizzledImage opt;
		pDestTex = opt.MakeSwizzledImage( pDestTex );
	}

	//	마지막에 밉맵 생성 - komet
	CButton * pCheckBox = (CButton *) GetDlgItem( IDC_CHECK_MIPMAP);
	if( pCheckBox->GetCheck() == BST_CHECKED)
	{
		i3OptMipmapGen optMipmap;
		i3Texture * pGenTex = NULL;
		char szTemp[256] = "";
		INT32 w = 0, h = 0;

		CEdit * pEditMipmapCX = (CEdit*) GetDlgItem( IDC_EDIT_MIPMAP_CX);
		CEdit * pEditMipmapCY = (CEdit*) GetDlgItem( IDC_EDIT_MIPMAP_CY);
		
		pEditMipmapCX->GetLine( 0, szTemp, 256);
		w = atoi( szTemp);

		pEditMipmapCY->GetLine( 0, szTemp, 256);
		h = atoi( szTemp);

		optMipmap.setMinWidth( w);
		optMipmap.setMinHeight( h);

		CEdit * pEditMaxLevel = (CEdit*) GetDlgItem( IDC_EDIT_MAXLEVEL);
		pEditMaxLevel->GetLine( 0, szTemp, 256);
		INT32 max = atoi( szTemp);

		optMipmap.setLimitMaxLevel( max);

		pGenTex = optMipmap.GenMipmap( pDestTex);

		if( pGenTex != NULL)
		{
			pDestTex->Release();
			pDestTex = pGenTex;
			pGenTex->AddRef();
		}
	}	

	switch( m_CompressMethod)
	{
		case 0 :			// None
			{
				i3OptCompressImage opt;

				opt.DecompressImage( pDestTex);
			}
			break;

		case 1 :			// ZIP
			{
				i3OptCompressImage opt;

				opt.CompressImage( pDestTex);
			}
			break;
		
		case 2	:			// DXT1-NoAlpha
		case 3 :			// DXT1
		case 4 :			// DXT3
		case 5 :			// DXT5
			if( !(pDestTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_COMPRESSED))
			{
				i3Texture * pDXTTex;
				i3OptConvertDXTTexture opt;

				switch( m_CompressMethod)
				{
					case 2	:		opt.setTargetImageFormat( I3G_IMAGE_FORMAT_DXT1_X);	break;
					case 3 :		opt.setTargetImageFormat( I3G_IMAGE_FORMAT_DXT1);	break;
					case 4 :		opt.setTargetImageFormat( I3G_IMAGE_FORMAT_DXT3);	break;
					case 5 :		opt.setTargetImageFormat( I3G_IMAGE_FORMAT_DXT5);	break;
				}

				opt.setDitherEnable( true);

				pDXTTex = opt.MakeToDXT( pDestTex);

				if( pDXTTex != NULL)
				{
					pDestTex->Release();
					pDestTex = pDXTTex;
					pDXTTex->AddRef();
				}
			}
			break;
	}

	char szOutPath[MAX_PATH];
	strcpy( szOutPath, szPath );
 	i3::remove_ext(szOutPath);
//	szOutFileName = i3String::InsertStr( szOutPath, ".i3i", strlen( szOutPath)  , 4);
	
	i3::generic_string_ncat( szOutPath, ".i3i", 4); // strlen( szOutPath)  , 4);
	file.SaveI3I(szOutPath, pDestTex);

	// 생성되어진 밉맵 텍스쳐를 해제합니다.
	pDestTex->Release();

	return TRUE;
}

int Ci3ImageMakerDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnVKeyToItem(nKey, pListBox, nIndex);
}

BOOL Ci3ImageMakerDlg::PreTranslateMessage(MSG* pMsg)
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

void Ci3ImageMakerDlg::OnBnClickedCheckMipmap()
{	//	komet
	// TODO: Add your control notification handler code here
	CButton * pCheckBox = (CButton *) GetDlgItem( IDC_CHECK_MIPMAP);
	CEdit * pEditMipmapCX = (CEdit*) GetDlgItem( IDC_EDIT_MIPMAP_CX);
	CEdit * pEditMipmapCY = (CEdit*) GetDlgItem( IDC_EDIT_MIPMAP_CY);
	CEdit * pEditMaxLevel = (CEdit*) GetDlgItem( IDC_EDIT_MAXLEVEL);

	//	Mipmap option activate
	if( pCheckBox->GetCheck() == BST_CHECKED)
	{
		pEditMipmapCX->EnableWindow( TRUE);
		pEditMipmapCY->EnableWindow( TRUE);
		pEditMaxLevel->EnableWindow( TRUE);
	}
	else if( pCheckBox->GetCheck() == BST_UNCHECKED)
	{
		pEditMipmapCX->EnableWindow( FALSE);
		pEditMipmapCY->EnableWindow( FALSE);
		pEditMaxLevel->EnableWindow( FALSE);
	}
}
