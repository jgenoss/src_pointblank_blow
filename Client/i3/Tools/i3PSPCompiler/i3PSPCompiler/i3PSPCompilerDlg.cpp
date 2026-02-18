// i3PSPCompilerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3PSPCompiler.h"
#include "i3PSPCompilerDlg.h"
#include ".\i3pspcompilerdlg.h"

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


// Ci3PSPCompilerDlg 대화 상자



Ci3PSPCompilerDlg::Ci3PSPCompilerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3PSPCompilerDlg::IDD, pParent)
	, m_ForceDither16Bits(FALSE)
	, m_bSwizzledImage(FALSE)
	, m_bNativeVertexFormat(FALSE)
	, m_bCompileStaticGeometry(FALSE)
	, m_bAddClippingBoundBox(FALSE)
	, m_bOptimizeIndexArray(FALSE)
	, m_bRotateAxis(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ci3PSPCompilerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHK_FORCEDITHER16BITS, m_ForceDither16Bits);
	DDX_Check(pDX, IDC_CHK_SWIZZLEDIMAGE, m_bSwizzledImage);
	DDX_Check(pDX, IDC_CHK_NATIVEVERTEXFORMAT, m_bNativeVertexFormat);
	DDX_Check(pDX, IDC_CHK_COMPILESTATICGEO, m_bCompileStaticGeometry);
	DDX_Check(pDX, IDC_CHK_ADDBBOX, m_bAddClippingBoundBox);
	DDX_Check(pDX, IDC_CHK_OPTINDEXARRAY, m_bOptimizeIndexArray);
	DDX_Control(pDX, IDC_LOGLIST, m_LogListCtrl);
	DDX_Control(pDX, IDC_SPLIT_HORZ, m_SplitCtrl);
	DDX_Control(pDX, IDC_CB_POSTYPE, m_PosTypeCtrl);
	DDX_Control(pDX, IDC_CB_WEIGHTTYPE, m_WeightTypeCtrl);
	DDX_Control(pDX, IDC_CB_NORMALTYPE, m_NormalTypeCtrl);
	DDX_Control(pDX, IDC_CB_TEXTYPE, m_TexTypeCtrl);
	DDX_Control(pDX, IDC_CB_COLORTYPE, m_ColorTypeCtrl);
	DDX_Control(pDX, IDC_ST_POS, m_PosStCtrl);
	DDX_Control(pDX, IDC_ST_WEIGHT, m_WeightStCtrl);
	DDX_Control(pDX, IDC_ST_NORMAL, m_NormalStCtrl);
	DDX_Control(pDX, IDC_ST_NORMAL2, m_TexStCtrl);
	DDX_Control(pDX, IDC_ST_NORMAL3, m_ColorStCtrl);
	DDX_Check(pDX, IDC_ROTATEAXIS, m_bRotateAxis);
	DDX_Control(pDX, IDC_CHK_PSPPOSTFIX, m_PSPPostfixCtrl);
	DDX_Control(pDX, IDC_CB_PLATFORM, m_PlatformCtrl);
	DDX_Control(pDX, IDC_ST_PLATFORM, m_StPlatformCtrl);
}

BEGIN_MESSAGE_MAP(Ci3PSPCompilerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHK_NATIVEVERTEXFORMAT, OnBnClickedChkNativevertexformat)
END_MESSAGE_MAP()


// Ci3PSPCompilerDlg 메시지 처리기

static void _LogProc( char * pszModule, char * pszMsg, void * pUserData)
{
	CListCtrl * pCtrl = (CListCtrl *) pUserData;
}

BOOL Ci3PSPCompilerDlg::OnInitDialog()
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
		m_LogListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LogListCtrl.InsertColumn( 0, "File", LVCFMT_LEFT, 200);
		m_LogListCtrl.InsertColumn( 1, "Message", LVCFMT_LEFT, 400);
	}

	ReplaceControls();

	{
		i3VideoInfo Info;

		Info.m_bFullScreen = FALSE;
		Info.m_FrameBufferCount = 2;
		Info.m_Width = 640;
		Info.m_Height = 480;
		Info.m_RenderTargetFormat = I3G_IMAGE_FORMAT_DEFAULT_COLOR;
		Info.m_ZBufferBits = 24;

		m_pContext = i3RenderContext::NewObject();

		m_pContext->Create( &Info, m_hWnd);
	}

	{
		m_pCompiler = i3DataCompilerPSP::NewObject();

		m_pCompiler->SetLogProc( (I3LOGPROC) _LogProc, &m_LogListCtrl);

		GetOptions();

		m_pCompiler->SetRenderContext( m_pContext);
	}

	{
		m_PSPPostfixCtrl.SetCheck( BST_CHECKED);
	}

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3PSPCompilerDlg::OnDestroy()
{
	I3_SAFE_RELEASE( m_pCompiler);
	I3_SAFE_RELEASE( m_pContext);

	i3ElementBase::DumpObjects();

	CDialog::OnDestroy();
}


void Ci3PSPCompilerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3PSPCompilerDlg::OnPaint() 
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
HCURSOR Ci3PSPCompilerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3PSPCompilerDlg::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1], Ext[32];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		i3String::SplitFileExt( TempName, Ext, sizeof(Ext) - 1);

		if( (i3String::Compare( Ext, "I3S") == 0) ||
			(i3String::Compare( Ext, "I3I") == 0))
		{
			char szTempPath[MAX_PATH], * pPath;
			char szName[MAX_PATH];

			i3String::Copy( szTempPath, TempName);

			pPath = i3String::SplitFileName( szTempPath, szName, FALSE);
			*pPath = 0;

			Compile( TempName);
		}
	}

	DragFinish( hDropInfo);

	CDialog::OnDropFiles(hDropInfo);
}

void Ci3PSPCompilerDlg::GetOptions(void)
{
	m_ForceDither16Bits			= m_pCompiler->IsOption( I3PSP_CA_FORCE_16BITS_IMAGE);
	m_bSwizzledImage			= m_pCompiler->IsOption( I3PSP_CA_SWIZZLED_IMAGE);
	m_bNativeVertexFormat		= m_pCompiler->IsOption( I3PSP_CA_NATIVE_VERTEX_FORMAT);
	m_bCompileStaticGeometry	= m_pCompiler->IsOption( I3PSP_CA_COMPILE_STATIC_GEOMETRY);
	m_bAddClippingBoundBox		= m_pCompiler->IsOption( I3PSP_CA_ADD_CLIPPING_BOUND);
	m_bOptimizeIndexArray		= m_pCompiler->IsOption( I3PSP_CA_OPTIMIZE_INDEX_ARRAY);
	m_bRotateAxis				= m_pCompiler->IsOption( I3PSP_CA_ROTATE_AXIS);

	m_PosTypeCtrl.SetCurSel( 1);		// Fixed-Point (16bits)
	m_WeightTypeCtrl.SetCurSel( 1);		// Fixed-Point (16bits)
	m_NormalTypeCtrl.SetCurSel( 1);		// Fixed-Point (16bits)
	m_TexTypeCtrl.SetCurSel( 1);		// Fixed-Point ( 16bits)
	m_ColorTypeCtrl.SetCurSel( 3);		// 16bits (4444)

	m_PlatformCtrl.SetCurSel( 1);

	SetVertexArrayOptEnable( m_bNativeVertexFormat);

	UpdateData( FALSE);
}

void Ci3PSPCompilerDlg::SetOptions(void)
{
	UpdateData( TRUE);

	if( m_ForceDither16Bits)
		m_pCompiler->AddOption( I3PSP_CA_FORCE_16BITS_IMAGE);
	else
		m_pCompiler->RemoveOption( I3PSP_CA_FORCE_16BITS_IMAGE);

	if( m_bSwizzledImage)
		m_pCompiler->AddOption( I3PSP_CA_SWIZZLED_IMAGE);
	else
		m_pCompiler->RemoveOption( I3PSP_CA_SWIZZLED_IMAGE);

	if( m_bNativeVertexFormat)
		m_pCompiler->AddOption( I3PSP_CA_NATIVE_VERTEX_FORMAT);
	else
		m_pCompiler->RemoveOption( I3PSP_CA_NATIVE_VERTEX_FORMAT);

	if( m_bCompileStaticGeometry)
		m_pCompiler->AddOption( I3PSP_CA_COMPILE_STATIC_GEOMETRY);
	else
		m_pCompiler->RemoveOption( I3PSP_CA_COMPILE_STATIC_GEOMETRY);

	if( m_bAddClippingBoundBox)
		m_pCompiler->AddOption( I3PSP_CA_ADD_CLIPPING_BOUND);
	else
		m_pCompiler->RemoveOption( I3PSP_CA_ADD_CLIPPING_BOUND);

	if( m_bOptimizeIndexArray)
		m_pCompiler->AddOption( I3PSP_CA_OPTIMIZE_INDEX_ARRAY);
	else
		m_pCompiler->RemoveOption( I3PSP_CA_OPTIMIZE_INDEX_ARRAY);

	if( m_bRotateAxis)
		m_pCompiler->AddOption( I3PSP_CA_ROTATE_AXIS);
	else
		m_pCompiler->RemoveOption( I3PSP_CA_ROTATE_AXIS);

	// Position
	{
		m_pCompiler->SetPositionConvertEnable( TRUE);

		switch( m_PosTypeCtrl.GetCurSel())
		{
			// Floating-Point (32bits)
			case 0 :	m_pCompiler->SetPositionType( I3VF_TYPE_REAL32);	break;

			// Fixed-Point (16bits )
			case 1 :	m_pCompiler->SetPositionType( I3VF_TYPE_FIXED16_15);	break;

			// Fixed-Point (8bits)
			case 2 :	m_pCompiler->SetPositionType( I3VF_TYPE_FIXED8_7);		break;
		}
	}

	// Normal
	{
		m_pCompiler->SetNormalConvertEnable( TRUE);

		switch( m_NormalTypeCtrl.GetCurSel())
		{
			// Floating-Point (32bits)
			case 0 :	m_pCompiler->SetNormalType( I3VF_TYPE_REAL32);	break;

			// Fixed-Point (16bits )
			case 1 :	m_pCompiler->SetNormalType( I3VF_TYPE_FIXED16_15);	break;

			// Fixed-Point (8bits)
			case 2 :	m_pCompiler->SetNormalType( I3VF_TYPE_FIXED8_7);		break;
		}
	}

	// Color
	{
		m_pCompiler->SetColorConvertEnable( TRUE);

		switch( m_ColorTypeCtrl.GetCurSel())
		{
			case 0 :	m_pCompiler->SetColorType( I3VF_TYPE_COLOR32_RGBA_8888);	break;
			case 1 :	m_pCompiler->SetColorType( I3VF_TYPE_COLOR16_RGBX_565);		break;
			case 2 :	m_pCompiler->SetColorType( I3VF_TYPE_COLOR16_RGBA_1555);	break;
			case 3 :	m_pCompiler->SetColorType( I3VF_TYPE_COLOR16_RGBA_4444);	break;
		}
	}

	// Texture Coords.
	{
        m_pCompiler->SetTextureCoordConvertEnable( TRUE);

		switch( m_TexTypeCtrl.GetCurSel())
		{
			// Floating-Point (32bits)
			case 0 :	m_pCompiler->SetTextureCoordType( I3VF_TYPE_REAL32);	break;

			// Fixed-Point (16bits )
			case 1 :	m_pCompiler->SetTextureCoordType( I3VF_TYPE_FIXED16_15_U);	break;

			// Fixed-Point (8bits)
			case 2 :	m_pCompiler->SetTextureCoordType( I3VF_TYPE_FIXED8_7_U);		break;
		}
	}

	// Weight
	{
		m_pCompiler->SetWeightConvertEnable( TRUE);

		switch( m_WeightTypeCtrl.GetCurSel())
		{
			// Floating-Point (32bits)
			case 0 :	m_pCompiler->SetWeightType( I3VF_TYPE_REAL32);	break;

			// Fixed-Point (16bits )
			case 1 :	m_pCompiler->SetWeightType( I3VF_TYPE_FIXED16_15_U);	break;

			// Fixed-Point (8bits)
			case 2 :	m_pCompiler->SetWeightType( I3VF_TYPE_FIXED8_7_U);		break;
		}
	}

	// Platform
	{
		switch( m_PlatformCtrl.GetCurSel())
		{
			// NA
			case  0 :	m_pCompiler->SetTargetPlatform( I3VF_PLATFORM_NA);	break;

			// PSP
			case 1 :	m_pCompiler->SetTargetPlatform( I3VF_PLATFORM_PSP);	break;
		}
	}
}


void Ci3PSPCompilerDlg::Compile( const char * pszFile)
{
	char szName[MAX_PATH], szDir[MAX_PATH], * pszName;
	char szFull[MAX_PATH], szExt[MAX_PATH];

	m_pCompiler->Load( pszFile);

	SetOptions();

	m_pCompiler->Compile();

	strcpy( szDir, pszFile);
	i3String::SplitFileExt( pszFile, szExt, sizeof( szExt) - 1);

	pszName = i3String::SplitFileName( szDir, szName, FALSE);

	*pszName = 0;

	if( m_PSPPostfixCtrl.GetCheck() == BST_CHECKED)
		sprintf( szFull, "%s\\%s_PSP.%s", szDir, szName, szExt);
	else
		sprintf( szFull, "%s\\%s.%s", szDir, szName, szExt);

	m_pCompiler->Save( szFull);
}

void Ci3PSPCompilerDlg::ReplaceControls( int cx, int cy)
{
	CRect rect;

	if( ! ::IsWindow( m_SplitCtrl.m_hWnd))
		return;

	if( cx == -1)
	{
		GetClientRect( &rect);

		cx = rect.Width();
		cy = rect.Height();
	}

	m_SplitCtrl.SetWindowPos( NULL, 0, 0, cx - 12, 2, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	m_LogListCtrl.GetWindowRect( &rect);
	ScreenToClient( &rect);
	m_LogListCtrl.SetWindowPos( NULL, 0, 0, cx - 12, cy - rect.top - 6, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

void Ci3PSPCompilerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ReplaceControls( cx, cy);
}

void Ci3PSPCompilerDlg::SetVertexArrayOptEnable( BOOL bFlag)
{
	m_bNativeVertexFormat = bFlag;

	m_PosTypeCtrl.EnableWindow( bFlag);
	m_PosStCtrl.EnableWindow( bFlag);

	m_WeightTypeCtrl.EnableWindow( bFlag);
	m_WeightStCtrl.EnableWindow( bFlag);

	m_NormalTypeCtrl.EnableWindow( bFlag);
	m_NormalStCtrl.EnableWindow( bFlag);

	m_TexTypeCtrl.EnableWindow( bFlag);
	m_TexStCtrl.EnableWindow( bFlag);

	m_ColorTypeCtrl.EnableWindow( bFlag);
	m_ColorStCtrl.EnableWindow( bFlag);

	m_PlatformCtrl.EnableWindow( bFlag);
	m_StPlatformCtrl.EnableWindow( bFlag);

	UpdateData( FALSE);
}

void Ci3PSPCompilerDlg::OnBnClickedChkNativevertexformat()
{
	UpdateData( TRUE);

	SetVertexArrayOptEnable( m_bNativeVertexFormat);
}
