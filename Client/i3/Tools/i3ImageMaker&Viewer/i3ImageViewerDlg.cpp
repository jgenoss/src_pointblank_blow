// i3ImageViewerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3ImageViewer.h"
#include "i3ImageViewerDlg.h"
#include "DlgMipmapOption.h"
#include ".\i3imageviewerdlg.h"
#include "GridSettingDlg.h"
#include "i3ImageMakerDlg.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static HWND s_hWnd = NULL;
void LogCallBack( char *szMsg )
{
	MessageBox( s_hWnd, szMsg, "i3Log", MB_OK );
}

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


// Ci3ImageViewerDlg 대화 상자



Ci3ImageViewerDlg::Ci3ImageViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3ImageViewerDlg::IDD, pParent)
{
	m_hIcon		= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pTexture	= NULL;

	m_nMipMapLevelCount		=		0;	//텍스쳐가 가지고있는 밉맵 레벨의 카운트.
	m_CurMipMap_Level		=		0;	//현재 디스플레이 되고있는 밉맵의 레벨
	m_ViewMode_Channel		=		CHANNEL_ALL;
	m_MipMap_ViewMode		=		MIPMAP_VIEW_NONE;

	for(int nLevel = 0 ; nLevel < MAX_MIPMAP_LEVEL ; ++nLevel)
	{
		m_hBitmap[nLevel]		= NULL;
		m_hOldBitmap[nLevel]	= NULL;
		m_MemDC[nLevel]			= NULL;
	}

	m_nTotalWidth		=	 0;

	m_posScroll.x	= 0;
	m_posScroll.y	= 0;
	m_posMouse.x	= 0;
	m_posMouse.y	= 0;

	m_RButtonDown = FALSE;
	m_LButtonDown = FALSE;
	m_RDragStart.x = 0 ; 
	m_RDragStart.y = 0 ; 
	m_LDragStart.x = 0;
	m_LDragStart.y = 0;
	m_LDragRect.SetRect( 0, 0, 0, 0);

	m_Zoom = 1.0f;
	m_WndSize.cx = 0 ; 
	m_WndSize.cy = 0;

	m_pImgContext	= NULL;
	m_pClutTableDlg = NULL;
	m_bDrawProperty = TRUE;

	m_szCmdLineArg = NULL;

	m_szFileName[0] = 0;
	m_bPopMipOptionDlg = true;

	m_bDrawGrid			= false;
	m_nGridVertBlockCount	= 1;
	m_nGridHoriBlockCount	= 1;
	m_nPenIndex			= BLACK_PEN;

	m_strFileName[0] = 0;
}

void Ci3ImageViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ci3ImageViewerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
// 	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
// 	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
// 	ON_COMMAND(ID_FILE_SAVE32823, OnFileSave32823)
// 	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
 	ON_COMMAND(ID_FILE_EXIT_2, OnFileExit)
 	ON_COMMAND(ID_FILE_OPEN_2, OnFileOpen)
 	ON_COMMAND(ID_FILE_SAVE_2, OnFileSave)
 	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND_RANGE(ID_MODE_ALL,ID_MODE_RGB,OnChangeMode)
	ON_COMMAND_RANGE(ID_MIPMAP_ALL,ID_MIPMAP_NONE,OnMipMap)
	ON_COMMAND_RANGE(ID_LEVEL_0,ID_LEVEL_7,OnChangeLevel)
	ON_COMMAND(ID_VIEW_PROPERTY, OnViewProperty)
	ON_WM_INITMENUPOPUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOVE()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_EXPORTMIPMAPS, OnFileExportmipmaps)
	ON_COMMAND(ID_FILE_GENERATEMIPMAPS, OnFileGeneratemipmaps)
	ON_COMMAND(ID_FILE_LOADMIPMAPS, OnFileLoadmipmaps)
	ON_COMMAND(ID_GRID_DRAWGRID, OnGridDrawgrid)
	ON_COMMAND(ID_GRID_GRIDSETTING, OnGridGridsetting)
END_MESSAGE_MAP()


// Ci3ImageViewerDlg 메시지 처리기
void Ci3ImageViewerDlg::DeleteImage()
{
	//밉맵 레벨에따른 서로 다른크기의 이미지들을 각각 가지고있도록함
	for(UINT32 nLevel  = 0 ; nLevel < m_nMipMapLevelCount ; ++nLevel)
	{
		if(m_MemDC[nLevel]	!= NULL)
		{
			if(m_hBitmap[nLevel] != NULL)
			{
				SelectObject(m_MemDC[nLevel] , m_hOldBitmap[nLevel]);
				
				DeleteObject(m_hBitmap[nLevel]);

				m_hBitmap[nLevel] = NULL;
			}

			DeleteDC(m_MemDC[nLevel]);

			m_MemDC[nLevel] = NULL;
		}
	}
}

BOOL Ci3ImageViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	if( g_pOption->m_WndX != CW_USEDEFAULT)
	{
		SetWindowPos( NULL, g_pOption->m_WndX, g_pOption->m_WndY, g_pOption->m_WndCX, g_pOption->m_WndCY, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	 m_hAccel = LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));

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
	s_hWnd = m_hWnd;
	//i3SetLogCallBackFunc( LogCallBack );

	//i3Log( NULL, "Test" );

	{
		i3VideoInfo video;

		video.m_Width = 32;
		video.m_Height = 32;

		m_pContext = i3RenderContext::new_object();
		m_pContext->Create( &video, m_hWnd);
	}

	if( m_szCmdLineArg != NULL )
	{
		if( m_szCmdLineArg[0] != 0 )
		{
			char szFileName[MAX_PATH];
			NormalizeCmdArg( szFileName, m_szCmdLineArg );
			//MessageBox( szFileName );
			if( OpenImageFile( szFileName ) )
			{
				char szWindowText[ MAX_PATH + 32 ];
				sprintf( szWindowText, "i3ImageViewer - \"%s\"", szFileName );
			}
		}
	}

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3ImageViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3ImageViewerDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
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

	if(m_pTexture == NULL)
	{////
		dc.SelectStockObject(NULL_PEN);
		dc.SelectStockObject(DKGRAY_BRUSH);

		CRect rt ;
		GetClientRect(&rt);

		dc.Rectangle(&rt);

		return;
	}

	CSize StartPos(0,0);
	int CX = (int)(m_posScroll.x / m_Zoom);
	int CY = (int)(m_posScroll.y / m_Zoom);

	if(CX == 0)
	{
		if(m_WndSize.cx > m_nTotalWidth * m_Zoom)
			StartPos.cx = (int)((m_WndSize.cx - m_nTotalWidth * m_Zoom) * 0.5f);
	}
	else
		StartPos.cx = -CX;

	if(CY == 0)
	{
		if(m_WndSize.cy > m_nTotalHeight * m_Zoom)	
			StartPos.cy = (int)((m_WndSize.cy - m_nTotalHeight * m_Zoom) * 0.5f);
	}
	else
		StartPos.cy = -CY;

	{////
		dc.SelectStockObject(NULL_PEN);
		dc.SelectStockObject(DKGRAY_BRUSH);
		CRect rt ; 
		if(m_Zoom * m_nTotalWidth< (REAL32)m_WndSize.cx)
		{
			dc.Rectangle(0 , 0 , StartPos.cx + 1 , m_WndSize.cy + 1);	//왼쪽
			dc.Rectangle(m_WndSize.cx - StartPos.cx - 1 , 0 , m_WndSize.cx + 1,m_WndSize.cy + 1);//오른쪽
		}
		

		if(m_Zoom * m_nTotalHeight < (REAL32)m_WndSize.cy)
		{
			dc.Rectangle( StartPos.cx , 0 , m_WndSize.cx + 1  - StartPos.cx , StartPos.cy + 1);//위쪽
			dc.Rectangle( StartPos.cx , m_WndSize.cy  - StartPos.cy -1, m_WndSize.cx + 1  - StartPos.cx ,m_WndSize.cy + 1);//아래쪽
		}
		dc.Rectangle(&rt);
	}

	int posx = (int)(m_posScroll.x / m_Zoom);
	int posy = (int)(m_posScroll.y / m_Zoom);

	StartPos.cx = StartPos.cx + posx;
	StartPos.cy = StartPos.cy + posy;

	if( m_MipMap_ViewMode == MIPMAP_VIEW_ALL)
	{
		CRect rt;
		UINT32 sx = m_pTexture->GetWidth();
		UINT32 sy = m_pTexture->GetHeight();
		for( INT32 i = 0;i < (INT32)m_nMipMapLevelCount; ++i)
		{
			::StretchBlt(dc.m_hDC,
						StartPos.cx,
						StartPos.cy,
						sx,
						sy,
						m_MemDC[i],
						0,
						0,
						(int)(sx / m_Zoom),
						(int)(sy / m_Zoom), 
						SRCCOPY);

			if( i == 0)
			{
				StartPos.cx += (int)(sx / m_Zoom);

				rt.left   = StartPos.cx;
				rt.right  = rt.left + sx;
				rt.top    = StartPos.cy;
				rt.bottom = rt.top + sy;

				dc.SelectStockObject(NULL_PEN);
				dc.SelectStockObject(DKGRAY_BRUSH);

				dc.Rectangle(&rt);
			}
			else
			{
				StartPos.cy += (int)(sy / m_Zoom);
			}

			sx = sx>>1;
			sy = sy>>1;
		}
	}
	else
	{
		INT32 level = 0;
		if( m_MipMap_ViewMode == MIPMAP_VIEW_LEVEL)	level = m_CurMipMap_Level;

		::StretchBlt(dc.m_hDC,
						StartPos.cx ,
						StartPos.cy,
						m_WndSize.cx,
						m_WndSize.cy,
						m_MemDC[level],
						CX,
						CY,
						(int)(m_WndSize.cx/ m_Zoom),
						(int)(m_WndSize.cy/ m_Zoom), 
						SRCCOPY);
	}

	DrawGrid(&dc, StartPos);

	if(m_pImgContext)
		if(m_bDrawProperty)	TypeProperty(&dc , m_pImgContext);

	CDialog::OnPaint();
}

BOOL Ci3ImageViewerDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST )
	{
		TRACE("%d\n", pMsg->message);
		if(m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
			return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void	Ci3ImageViewerDlg::SetContext( const char * szFileName , i3Texture * pTexture)
{
	if(m_pImgContext)
	{
		delete m_pImgContext;
		m_pImgContext = NULL;
	}

	if(pTexture != NULL)
	{
		m_pImgContext = new Ci3ImageContext;

		if(szFileName != NULL)		m_pImgContext->SetImageName(szFileName);
		if(pTexture != NULL)		m_pImgContext->SetContext(pTexture);
	}
}

void	Ci3ImageViewerDlg::TypeProperty(CPaintDC * hdc , Ci3ImageContext * ImgContext)
{
	char szTemp[256];
	//PROPERTY CONTEXT
	//
	/*
		char				m_cFileName[256];	//파일명
		char				m_cExt[8];			//파일종류
		
		I3G_IMAGE_FORMAT	m_enImageFormat;	//이미지 포맷

		UINT32				m_nImageWidth;		//이미지 크기 WIDTH
		UINT32				m_nImageHeight;		//이미지 크기 HEIGHT

		UINT32				m_nLevelCount;		//밉맵 레벨카운트
		UINT32				m_nBitPerPixel;		//퍼픽셀 비트
		UINT32				m_nStride;			//Stride
		UINT32				m_nPitch;			//Pitch
		UINT32				m_nDataSize;		//DataSize
	*/

	//파일명
	char szFileName[256];
	sprintf(szFileName ,		"Name           : %s" , ImgContext->GetFileName());

	//파일종류
	char szExt[256];
	sprintf(szExt ,				"Ext            : %s" , ImgContext->GetFileExt());

	//이미지파일포맷
	char szPersistFormat[256];
	ImgContext->GetPersistFormatStr(szTemp);
	sprintf( szPersistFormat,	"Persist Format : %s" , szTemp);

	//이미지포맷
	char szFormat[256];
	ImgContext->GetFormatStr(szTemp);
	sprintf( szFormat,			"Format         : %s" , szTemp);

	// 알파 존재 여부
	char szHasAlpha[64];
	sprintf( szHasAlpha,		"Alpha          : %s" , ImgContext->HasAlpha() ? "Yes" : "No" );

	//이미지 크기
	char szImageSize[256];
	sprintf(szImageSize ,		"Size           : %d * %d" , ImgContext->GetWidth() , ImgContext->GetHeight());

	//밉맵레벨카운트
	char szLevelCount[256];
	sprintf(szLevelCount ,		"Level Count    : %d" , ImgContext->GetLevelCount());
	//퍼픽셀 비트
	char szBitPerPixel[256];
	sprintf(szBitPerPixel ,		"BitPerPixel    : %d" , ImgContext->GetBitPerPixel());
	//stride
	char szStride[256];
	sprintf(szStride ,			"Stride         : %d" , ImgContext->GetStride());
	//pitch
	char szPitch[256];
	sprintf(szPitch ,			"Pitch          : %d" , ImgContext->GetPitch());

	//DataSize
	char szDataSize[256];
	if(m_MipMap_ViewMode != MIPMAP_VIEW_LEVEL)
		sprintf(szDataSize ,	"DataSize       : %d" , ImgContext->GetDataSize(MAX_MIPMAP_LEVEL));
	else
		sprintf(szDataSize ,	"DataSize       : %d" , ImgContext->GetDataSize(m_CurMipMap_Level));

	//CALCULATE MOUSE POS BY IMAGE
	RECT rtClient;
	GetClientRect( &rtClient );
	CPoint POS;

	CSize StartPos(0,0);
	int CX = (int)(m_posScroll.x / m_Zoom);
	int CY = (int)(m_posScroll.y / m_Zoom);

	if(CX == 0)	if(m_WndSize.cx > m_nTotalWidth * m_Zoom)	
	{
		StartPos.cx = (int)((m_WndSize.cx - m_nTotalWidth * m_Zoom) * 0.5f);
	}

	if(CY == 0)	if(m_WndSize.cy > m_nTotalHeight * m_Zoom)	
	{
		StartPos.cy = (int)((m_WndSize.cy - m_nTotalHeight * m_Zoom) * 0.5f);
	}

	// 이미지 좌측 상단과 마우스 포인터와의 거리를 확대비율로 나눈다.
	POS.x =  (INT32)((m_posScroll.x + m_posMouse.x - StartPos.cx) / m_Zoom) ;// (INT32)(((m_posMouse.x - ((rtClient.right * 0.5f) - ( ImgContext->GetWidth() * 0.5f ) * m_Zoom)) + m_posScroll.x )/ m_Zoom );
	POS.y =  (INT32)((m_posScroll.y + m_posMouse.y - StartPos.cy) / m_Zoom);//(INT32)(((m_posMouse.y - ((rtClient.bottom * 0.5f) - ( ImgContext->GetHeight() * 0.5f ) * m_Zoom)) + m_posScroll.y ) / m_Zoom );

	char szMousePos[256];
	sprintf( szMousePos ,		"Mouse Pos      : %d , %d " , POS.x , POS.y );

	//////////////////////////////////////////////////////////////////////////
	if( m_pClutTableDlg )
	{
		if( POS.x >= 0 && POS.x <= (INT32)m_pTexture->GetWidth() )
		{
			if( POS.y >= 0 && POS.y <= (INT32)m_pTexture->GetHeight() )
			{
				i3Clut * pClut = m_pTexture->GetClut(0);

				UINT8 * pPixel = (UINT8*)m_pTexture->Lock(0);
				if( pPixel != NULL )
				{
					if( pClut->GetColorCount() > 0 && pClut->GetColorCount() <= 16 )//16이하 CLUTTABLE
					{
						pPixel += ((m_pTexture->GetWidth()>>1) * POS.y) + (POS.x>>1);
						if(POS.x%2)		m_pClutTableDlg->SetColorValue( pPixel[0]>>4 & 0x0F);
						else			m_pClutTableDlg->SetColorValue( pPixel[0] & 0x0F);
					}
					else//16이상 CLUTTABLE
					{
						pPixel += m_pTexture->GetWidth() * POS.y + POS.x;
						m_pClutTableDlg->SetColorValue( *pPixel );
					}
				}
				m_pTexture->Unlock(0);
			}
		}	
	}
	//////////////////////////////////////////////////////////////////////////

	HFONT hFont , hOldFont;
	hFont = CreateFont(12,0,0,0,0,0,0,0,HANGUL_CHARSET , 3,2,1,VARIABLE_PITCH | FF_ROMAN , "굴림체");
	hOldFont = (HFONT)SelectObject(hdc->m_hDC , hFont);


	hdc->SetTextColor(RGB(255,255,0));

	hdc->SetBkMode(TRANSPARENT);
	
	//Image Context
	int nX = 10, nY = 5, dY = 15;
	hdc->TextOut( nX, nY,  szFileName		);	nY += dY;
	hdc->TextOut( nX, nY,  szExt			);	nY += dY;
	hdc->TextOut( nX, nY,  szPersistFormat	);	nY += dY;
	hdc->TextOut( nX, nY,  szFormat			);	nY += dY;
	hdc->TextOut( nX, nY,  szHasAlpha		);	nY += dY;
	hdc->TextOut( nX, nY,  szImageSize		);	nY += dY;
	hdc->TextOut( nX, nY,  szLevelCount		);	nY += dY;
	hdc->TextOut( nX, nY,  szBitPerPixel	);	nY += dY;
	hdc->TextOut( nX, nY,  szStride			);	nY += dY;
	hdc->TextOut( nX, nY,  szPitch			);	nY += dY;
	hdc->TextOut( nX, nY,  szDataSize		);	nY += dY;
	
	//Viewer Context
	if(m_ViewMode_Channel == CHANNEL_ALL)			hdc->TextOut( nX , nY , "CHANNEL        : ALL");
	else if(m_ViewMode_Channel == CHANNEL_RED)		hdc->TextOut( nX , nY , "CHANNEL        : RED");
	else if(m_ViewMode_Channel == CHANNEL_GREEN)	hdc->TextOut( nX , nY , "CHANNEL        : GREEN");
	else if(m_ViewMode_Channel == CHANNEL_BLUE)		hdc->TextOut( nX , nY , "CHANNEL        : BLUE");
	else if(m_ViewMode_Channel == CHANNEL_ALPHA)	hdc->TextOut( nX , nY , "CHANNEL        : ALPHA");
	else if(m_ViewMode_Channel == CHANNEL_RGB)		hdc->TextOut( nX , nY , "CHANNEL        : RGB");
	nY += dY;

	//MipMap ViewMode
	if(m_MipMap_ViewMode == MIPMAP_VIEW_NONE)			hdc->TextOut( nX , nY , "MIPMAP_VIEW    : NO");
	else if(m_MipMap_ViewMode == MIPMAP_VIEW_ALL)		hdc->TextOut( nX , nY , "MIPMAP_VIEW    : ALL");
	else if(m_MipMap_ViewMode == MIPMAP_VIEW_LEVEL)
	{
		char szCurrentMipMapLevel[256];
		sprintf(szCurrentMipMapLevel , "MIPMAP_VIEW    : %d " , m_CurMipMap_Level);
		hdc->TextOut( nX , nY , szCurrentMipMapLevel);
	}
	nY += dY;


	hdc->TextOut( nX, nY,  szMousePos		);	nY += dY;
	//

	hdc->SelectStockObject( WHITE_PEN );
	hdc->SelectStockObject( NULL_BRUSH );

	if( m_LButtonDown )
	{
		m_LDragRect.SetRect( m_LDragStart.x , m_LDragStart.y , m_posMouse.x , m_posMouse.y );
		hdc->Rectangle( &m_LDragRect );

		char szMouseSize[256];
		sprintf( szMouseSize , "Select Size : %d , %d " , (INT32)(abs( (m_posMouse.x - m_LDragStart.x ) / m_Zoom )) , (INT32)(abs((m_posMouse.y - m_LDragStart.y ) / m_Zoom )));
		hdc->TextOut( nX , nY , szMouseSize );
	}
	nY += dY;

	//

	SelectObject(hdc->m_hDC , hOldFont);
	DeleteObject(hFont);
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR Ci3ImageViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3ImageViewerDlg::OnDropFiles(HDROP hDropInfo)
{
	m_hDropInfo = hDropInfo;

	// TODO: Add your message handler code here and/or call default
	int count; 
	//char FileName[1024];

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	if(count > 1)
	{
		//Error 한번에 하나만 로딩할수 있다. 
		CDialog::OnDropFiles(hDropInfo);
		return; 
	}

	DragQueryFile( hDropInfo, 0, m_strFileName, sizeof(m_strFileName) - 1);

	if( ! OpenImageFile( m_strFileName ) )
	{
		CDialog::OnDropFiles(hDropInfo);
		return;
	}

	CDialog::OnDropFiles(hDropInfo);
}

BOOL Ci3ImageViewerDlg::OpenImageFile( const char *szFileName )
{
	if ( CheckFileName(szFileName) == FALSE ) 
	{
		MessageBox( "Invalid file format." );
		return FALSE;
	}

	if( m_pClutTableDlg )
	{
		m_pClutTableDlg->DestroyWindow();
		I3_SAFE_DELETE(m_pClutTableDlg);
	}
	I3_SAFE_RELEASE(m_pTexture);

	i3Texture * pTex = i3Texture::new_object();
	//pTex->SetRuntimePlatform( I3_PLATFORM_WINDOWS );
	m_pTexture = pTex;

	if( m_pImageFile.Load( szFileName , m_pTexture ) )
	{
		SetImage(m_pTexture);
	}
	else
	{
		I3_SAFE_RELEASE(m_pTexture);
		MessageBox( "Error : 'BMP' extension only supported " );
		return FALSE;
	}

	strcpy( m_szFileName, szFileName);

	{
		INT32 screenWidth = ::GetSystemMetrics( SM_CXSCREEN);
		INT32 screenHeight = ::GetSystemMetrics( SM_CYSCREEN);

		INT32	width = m_nTotalWidth + 100;
		INT32	height = m_nTotalHeight + 100;

		CRect rt( 0,0,680, 480);
		AdjustWindowRect( &rt, WS_OVERLAPPEDWINDOW, TRUE);

		INT32 w = 680 - rt.right;
		INT32 h = 480 - rt.bottom;

		width += w;
		height += h;

		if( screenWidth < width + 100)		width = screenWidth - 100;
		if( screenHeight < height + 100)	height = screenHeight - 100;
		if( width < 640)	width = 640;
		if( height < 480)	height = 480;

		GetWindowRect(&rt);
		SetWindowPos( NULL, rt.left, rt.top, width, height, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	SetContext(szFileName , m_pTexture);
	m_Zoom = 1.0f;
	SetScroll();

	//MessageBox( "Success" );

	return TRUE;
}

BOOL Ci3ImageViewerDlg::SaveImageFile( const char * pszPath)
{
	//
	i3ImageFile file;

	if( m_pTexture == NULL)
		return FALSE;

	char szExt[8];
	
//	i3String::SplitFileExt( pszPath, szExt, 8);
	i3::extract_fileext(pszPath, szExt);
	i3::to_upper( szExt);

	if( i3::generic_is_iequal( szExt, "TGA") )
	{
		m_pTexture->SetPersistFormat( I3G_IMAGE_FORMAT_BGRA_8888);
	}

	file.Save( pszPath, m_pTexture);

	if( pszPath != m_szFileName)
		strcpy( m_szFileName, pszPath);

	return TRUE;
}

BOOL Ci3ImageViewerDlg::CheckFileName( const char	*	szFileName)
{
	char Ext[32];

//	i3String::SplitFileExt(szFileName, Ext, sizeof(Ext) - 1);
	i3::extract_fileext(szFileName, Ext);

// 	아래는 조건문 구현이 죄다 틀린것 같아서 고칩니다. (2013.05.07.수빈)

	if( i3::generic_is_iequal( (char *)Ext, (char *)"tga") )		return TRUE;
	else if( i3::generic_is_iequal( (char *)Ext, (char *)"bmp") )	return TRUE;
	else if( i3::generic_is_iequal( (char *)Ext, (char *)"i3i") )	return TRUE;
	else
	{
		//사용되어지고있는 어떤형식의 이미지 파일도 아니므로 
		return FALSE; 
	}
}

void Ci3ImageViewerDlg::OnDestroy()
{
	I3_SAFE_RELEASE( m_pContext);

	CDialog::OnDestroy();

	{
		CRect rt;

		GetWindowRect( &rt);

		g_pOption->m_WndX = rt.left;
		g_pOption->m_WndY = rt.top;
		g_pOption->m_WndCX = rt.Width();
		g_pOption->m_WndCY = rt.Height();
	}

	I3_SAFE_DELETE(m_pImgContext);
	if( m_pClutTableDlg )
	{
		m_pClutTableDlg->DestroyWindow();
		I3_SAFE_DELETE(m_pClutTableDlg);
	}
	I3_SAFE_RELEASE(m_pTexture);
	// TODO: Add your message handler code here
}

void	Ci3ImageViewerDlg::SetImage(i3Texture	*	pTexture)//백버퍼에 텍스쳐를 비트맵화하여 얹는다.
{
	if(pTexture == NULL)	return ;
	
	DeleteImage();

	//Calculate Total PaintDCSize
	m_nMipMapLevelCount = pTexture->GetLevelCount();
	m_CurMipMap_Level	= 0;
	if( m_nMipMapLevelCount > 1) m_MipMap_ViewMode = MIPMAP_VIEW_ALL;
	else	m_MipMap_ViewMode = MIPMAP_VIEW_NONE;

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create DC
	CalculateTotalSize(pTexture);		//사용될 백버퍼의 크기 계산

	SetMipMapToDC(pTexture , m_nMipMapLevelCount);

	if( pTexture )
	{
		i3Clut * pClut = pTexture->GetClut(0);
		if( pClut )
		{
			if( m_pClutTableDlg )
			{
				m_pClutTableDlg->DestroyWindow();
				I3_SAFE_DELETE(m_pClutTableDlg);
			}
			m_pClutTableDlg = new i3ClutTableDlg;

			BOOL ret = m_pClutTableDlg->Create( IDD_CLUT_TABLE, this);
			if(!ret)   //Create failed.
				AfxMessageBox("Error creating Dialog");

			//
			m_pClutTableDlg->SetClut(pClut);
			m_pClutTableDlg->ShowWindow(SW_SHOW);

			MoveClutWindow();
		}
	}
}

void	Ci3ImageViewerDlg::CalculateTotalSize(i3Texture	*	pTexture)
{
	m_nTotalWidth  = 0;
	m_nTotalHeight = 0;

	m_nTotalWidth = pTexture->GetWidth();
	m_nTotalHeight = pTexture->GetHeight();

	if(m_MipMap_ViewMode == MIPMAP_VIEW_NONE)
	{
		m_nTotalWidth = pTexture->GetWidth();
		m_nTotalHeight = pTexture->GetHeight();
	}

	else if(m_MipMap_ViewMode == MIPMAP_VIEW_LEVEL)
	{
		m_nTotalHeight = pTexture->GetHeight()>>m_CurMipMap_Level;
		m_nTotalWidth = pTexture->GetWidth()>>m_CurMipMap_Level;
	}

	else if(m_MipMap_ViewMode == MIPMAP_VIEW_ALL)
	{
		if( m_nMipMapLevelCount >= 2)
			m_nTotalWidth += pTexture->GetWidth()>>1;
	}
}

void	Ci3ImageViewerDlg::SetMipMapToDC(i3Texture * pTexture , UINT32 nLevelCount )
{
	for(UINT32 i = 0 ; i < nLevelCount ; ++i)
	{
		SetImagetoDC( i , pTexture , 0 , 0);
	}
/*
	if(m_MipMap_ViewMode == MIPMAP_VIEW_NONE)		SetImagetoDC( 0 , pTexture , 0 , 0);
	else if(m_MipMap_ViewMode == MIPMAP_VIEW_LEVEL)
	{
		if(m_CurMipMap_Level >= nLevelCount)	m_CurMipMap_Level = 0;

		SetImagetoDC( m_CurMipMap_Level , pTexture , 0 , 0);
	}
	else if(m_MipMap_ViewMode == MIPMAP_VIEW_ALL)
	{
		INT32	WidthPosInDC = 0;
		for(UINT32 i = 0 ; i < nLevelCount ; ++i)
		{
			SetImagetoDC( i , pTexture , WidthPosInDC , 0);
			WidthPosInDC += pTexture->GetWidth()>>i;
		}
	}*/
}

void	Ci3ImageViewerDlg::SetImagetoDC(INT32 nLevel , i3Texture * pTexture , INT32	WidthPosInDC , INT32 HeightPosInDC)//밉밉 레벨에따라 텍스쳐를 비트맵화
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Bitmap
	BITMAPINFOHEADER fi;
	PVOID pRaster;
	BITMAPINFO ih;

	INT32	WidthofImg  = pTexture->GetWidth()  >> nLevel;
	INT32	HeightofImg = pTexture->GetHeight() >> nLevel;

	//
	INT32		Height = - HeightofImg;					//이미지가 거꾸로 들어와있는상태라height 는 - 
	I3G_IMAGE_FORMAT	DestFormat	 = I3G_IMAGE_FORMAT_BGRA_8888;  
	UINT16				DestBitCount = 32;

	::ZeroMemory(&fi,sizeof(BITMAPINFOHEADER));
	fi.biSize		= sizeof(BITMAPINFOHEADER);
	fi.biBitCount	= DestBitCount;

	fi.biHeight		= Height;
	fi.biWidth		= WidthofImg;
	fi.biPlanes		= 1;
	fi.biCompression= BI_RGB;

	memcpy(&ih.bmiHeader,&fi,sizeof(BITMAPINFOHEADER));

	m_hBitmap[nLevel] = CreateDIBSection(NULL,&ih,DIB_RGB_COLORS,&pRaster,NULL,0);
	
	char * src		= pTexture->Lock(nLevel);
	char * pDest	= (char*)pRaster;

	i3Image_Convert( pTexture->GetFormat(), src, WidthofImg , HeightofImg , 1, pTexture->GetClut(nLevel),
					 DestFormat, pDest, 4, NULL);


	AlphaBlendFunc( pDest, WidthofImg, HeightofImg);

	HDC hdc = ::GetDC(this->GetSafeHwnd());
	
	//For Image
	m_MemDC[nLevel] = CreateCompatibleDC(hdc);
	m_hOldBitmap[nLevel] = (HBITMAP) ::SelectObject( m_MemDC[nLevel], m_hBitmap[nLevel]);

	::ReleaseDC(m_hWnd,hdc);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	pTexture->Unlock(nLevel);
}

void Ci3ImageViewerDlg::AlphaBlendFunc(char * pRaster, INT32 width, INT32 height)
{
	if( m_ViewMode_Channel != CHANNEL_RGB)
	{
		UINT8 * pPixel = (UINT8 *) pRaster;
		INT32 i, j, back;

		for( i = 0 ; i < height; i++)
		{
			for( j = 0; j < width; j++)
			{
				switch( m_ViewMode_Channel)
				{
					case CHANNEL_ALL :
						back = ((((i >> 3) ^ (j >> 3)) & 0x01) << 6) + 128;

						pPixel[0] = (UINT8)((((UINT32)pPixel[0] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						pPixel[1] = (UINT8)((((UINT32)pPixel[1] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						pPixel[2] = (UINT8)((((UINT32)pPixel[2] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						break;

					case CHANNEL_RED :	*((UINT32 *) pPixel)  &= 0xFFFF0000;	break;
					case CHANNEL_GREEN :*((UINT32 *) pPixel)  &= 0xFF00FF00;	break;
					case CHANNEL_BLUE :	*((UINT32 *) pPixel)  &= 0xFF0000FF;	break;
					case CHANNEL_ALPHA :
						pPixel[0] = pPixel[3];
						pPixel[1] = pPixel[3];
						pPixel[2] = pPixel[3];
						break;
				}

				pPixel += 4;
			}
		}
	}
}

void Ci3ImageViewerDlg::AlphaBlendFunc(HDC DestDC , BYTE * pSrc , INT32	Width , INT32 Height , INT32 DestPosX  , INT32 DestPosY)
{
	//channel_all
	COLORREF	DST;
	for(int i = 0 ; i < (signed)Height ; ++i)
	{
		for(int j = 0 ; j < (signed)Width ; ++j)
		{
			DST = GetPixel(DestDC , j + DestPosX , i + DestPosY);
			BYTE	R = GetRValue(DST);
			BYTE	G = GetGValue(DST);
			BYTE	B = GetBValue(DST);
			BYTE	A = pSrc[3];
			
			if(m_ViewMode_Channel == CHANNEL_RGB)
				DST = RGB(pSrc[2] , pSrc[1] , pSrc[0]);
			else if(m_ViewMode_Channel == CHANNEL_ALPHA)
				DST = RGB(A , A , A);
			else
			{
				GetRGBValue((pSrc[2] * A + (255 - A) * R)>>8 , 
							(pSrc[1] * A + (255 - A) * G)>>8 , 
							(pSrc[0] * A + (255 - A) * B)>>8 , 
							DST);
			}

			SetPixel(DestDC ,j + DestPosX ,
							 i + DestPosY, 
							 DST);
			pSrc += 4;
		}
	}
}

void	Ci3ImageViewerDlg::GetRGBValue(INT32 R , INT32 G , INT32 B , COLORREF& DST)
{
	switch(m_ViewMode_Channel)
	{
		case	CHANNEL_ALL:	DST = RGB(R , G , B);	break;
		case	CHANNEL_RED:	DST = RGB(R , 0 , 0);	break;
		case	CHANNEL_GREEN:	DST = RGB(0 , G , 0);	break;
		case	CHANNEL_BLUE:	DST = RGB(0 , 0 , B);	break;
		default	:				DST = RGB(R , 0 , 0);	break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void	Ci3ImageViewerDlg::SetGridtoDC(HDC	hdc , INT32	origin_x , INT32	origin_y , INT32 Width , INT32	Height )//DC에 그리드를 만들어준다.
{
	INT32 OriginGridWidth = min(Width , Height);
	INT32	nDivide = (INT32)(OriginGridWidth / 20);
	if(nDivide < 5 ) nDivide = 5;
	BOOL OnWidth = FALSE;
	BOOL OnHeight = TRUE;

	for(INT32 i = 0 ; i  < Height ; ++i)
	{
		OnWidth = !OnHeight;
		for(INT32 j = 0 ; j < Width ; ++j)
		{
			if(OnWidth)
				SetPixel(hdc, j+origin_x , i+origin_y , RGB(200,200,200));
			else
				SetPixel(hdc, j+origin_x , i+origin_y , RGB(150,150,150));

			if(j%nDivide == nDivide-1) OnWidth = !OnWidth;
		}

		if(i%nDivide == nDivide-1) OnHeight  =  !OnHeight;
	}
}

BOOL Ci3ImageViewerDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CDialog::OnEraseBkgnd(pDC);
	return FALSE;
}


void Ci3ImageViewerDlg::OnFileExit()
{
	// TODO: Add your command handler code here
	CDialog::OnCancel();
}

void Ci3ImageViewerDlg::OnFileOpen()
{
	// TODO: Add your command handler code here
	//char lpstrFile[1024] = "";//얻어올 파일명

	OPENFILENAME	ofn;
	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner	= this->m_hWnd;
	ofn.lpstrFilter = "이미지 파일(tga,bmp,i3i)\0*.tga;*.bmp;*.i3i\0TGA파일(*.tga)\0*.tga\0비트맵파일(*.bmp)\0*.bmp\0i3i 파일(*.i3i)";
	ofn.lpstrFile   = m_strFileName;
	ofn.nMaxFile	= 1024;

	if(GetOpenFileName(&ofn) != 0)
	{
		OpenImageFile( m_strFileName );
	}

}

void	Ci3ImageViewerDlg::OnMipMap(UINT nID)
{
	m_MipMap_ViewMode = nID - ID_MIPMAP_ALL;
	if(m_pTexture)
	{
		CalculateTotalSize(m_pTexture);		//사용될 백버퍼의 크기 계산
		UpdateData(TRUE);
		Invalidate();
	}
}


void	Ci3ImageViewerDlg::OnChangeLevel(UINT nID)
{
	m_CurMipMap_Level = nID - ID_LEVEL_0;
	m_MipMap_ViewMode = MIPMAP_VIEW_LEVEL;
	if(m_pTexture)
	{
		CalculateTotalSize(m_pTexture);		//사용될 백버퍼의 크기 계산
		UpdateData(TRUE);
		Invalidate();
	}
}

void Ci3ImageViewerDlg::OnChangeMode(UINT nID)
{
	m_ViewMode_Channel = nID - ID_MODE_ALL;	
	if(m_pTexture)
	{
		SetImage(m_pTexture);
		UpdateData(TRUE);
		Invalidate();
	}
}


void Ci3ImageViewerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	//Dialog Based 응용프로그램에서 메뉴를 쓸경우 ,initMenuPopup에서 업데이트 메시지가 들어오지 않으므로
	//강제로 업데이트 시 해야할일들을 지정한다.
	// TODO: Add your message handler code here
	switch(nIndex)
	{
		case 0 :	break;


		case 1 :
			//Draw Property
			OnUpdateCommand_Check(pPopupMenu , ID_VIEW_PROPERTY  , m_bDrawProperty);

			//Grid

			//MipMap
			OnUpdateCommandUI_Range_Check(pPopupMenu  , ID_MIPMAP_ALL , 3 , m_MipMap_ViewMode);
			if(m_MipMap_ViewMode == MIPMAP_VIEW_LEVEL)
				OnUpdateCommandUI_Range_Check(pPopupMenu  , ID_LEVEL_0 , m_nMipMapLevelCount , m_CurMipMap_Level);

			OnUpdateCommandUI_Range_Enable(pPopupMenu , ID_LEVEL_0 , 8 , m_nMipMapLevelCount);
			break;
		case 2 :
			OnUpdateCommandUI_Range_Check(pPopupMenu , ID_MODE_ALL , 6 , m_ViewMode_Channel);
			break;
	}
}

// bCheck값에의해 체크상태 지정
void Ci3ImageViewerDlg::OnUpdateCommand_Check(CMenu * pPopupMenu , UINT nIndex  , BOOL bCheck)
{
	if(bCheck)		pPopupMenu->CheckMenuItem(nIndex , MF_CHECKED );
	else			pPopupMenu->CheckMenuItem(nIndex , MF_UNCHECKED );
}

//그룹중에 nID 와 같은 아이디만을 체크상태로 표시
void Ci3ImageViewerDlg::OnUpdateCommandUI_Range_Check(CMenu * pPopupMenu , UINT nStartIndex , UINT Range , UINT nID)
{
	for(DWORD i = 0 ; i < Range ; ++i)
	{
		OnUpdateCommand_Check(pPopupMenu , i + nStartIndex , nID == i);
	}
}

//nCount 이상의 값을 가진 아이디들을 disable시킨다.밉맵레벨선택 제한을 위해 사용
void Ci3ImageViewerDlg::OnUpdateCommandUI_Range_Enable(CMenu * pPopupMenu , UINT nStartIndex , UINT Range , UINT nCount)
{
	for(DWORD i = 0 ; i < Range ; ++i)
	{
		if(nCount > i)
			pPopupMenu->EnableMenuItem(i + nStartIndex , MF_ENABLED );
		else
			pPopupMenu->EnableMenuItem(i + nStartIndex , MF_DISABLED | MF_GRAYED );
	}
}


void Ci3ImageViewerDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = TRUE;

	::SetCapture(this->m_hWnd);

	m_RDragStart.x = point .x;
	m_RDragStart.y = point .y;
/*
	if( m_pTexture != NULL)
	{
		CSize StartPos(0,0);
		int CX = (int)(m_posScroll.x / m_Zoom);
		int CY = (int)(m_posScroll.y / m_Zoom);

		if(CX == 0)	if(m_WndSize.cx > m_nTotalWidth * m_Zoom)	
		{
			StartPos.cx = (int)((m_WndSize.cx - m_nTotalWidth * m_Zoom) * 0.5f);
		}

		if(CY == 0)	if(m_WndSize.cy > m_nTotalHeight * m_Zoom)	
		{
			StartPos.cy = (int)((m_WndSize.cy - m_nTotalHeight * m_Zoom) * 0.5f);
		}

		if( (m_posScroll.x +  point.x < m_nTotalWidth) && (m_posScroll.y +  point.y < m_nTotalHeight))
		{
			COLORREAL col;

			m_pTexture->Lock( 0, I3G_LOCK_READONLY);

			m_pTexture->GetPixel( m_posScroll.x +  point.x, m_posScroll.y +  point.y, &col);

			i3Color::Dump( "Pick : ", &col);

			m_pTexture->Unlock();
		}
	}
*/
	CDialog::OnRButtonDown(nFlags, point);
}

void Ci3ImageViewerDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = FALSE;
	::ReleaseCapture();
	CDialog::OnRButtonUp(nFlags, point);
}


void	Ci3ImageViewerDlg::SetScroll()
{
	int Min , Max , Factor;
	REAL32	magnification;
	////////////
	//SetScrollRange
	GetScrollRange(SB_HORZ , &Min , &Max );
	Factor = ((int)(m_nTotalWidth * m_Zoom)  -  m_WndSize.cx);
	SetScrollRange(SB_HORZ , 0 , Factor, TRUE);

	//SetScrollpos
	if(Max <= 0)	m_posScroll.x = 0;
	else
	{
		magnification = (REAL32)Factor / Max ; 
		m_posScroll.x = (int)((REAL32)m_posScroll.x * magnification) ; 
	}

	SetScrollPos(SB_HORZ , m_posScroll.x , TRUE);

	/////////////
	GetScrollRange(SB_VERT , &Min , &Max );
	Factor = ((int)((REAL32)m_nTotalHeight * m_Zoom)  -  m_WndSize.cy);
	SetScrollRange(SB_VERT , 0 , Factor , TRUE);

	if(Max <= 0)	m_posScroll.y = 0;
	else
	{
		magnification = (REAL32)Factor / Max ; 
		m_posScroll.y = (int)((REAL32)m_posScroll.y * magnification ); 
	}

	SetScrollPos(SB_VERT , m_posScroll.y , TRUE);

	if(m_WndSize.cx > (int)(m_nTotalWidth * m_Zoom))	ResetScroll(SB_HORZ , m_posScroll.x);
	if(m_WndSize.cy > (int)(m_nTotalHeight * m_Zoom))ResetScroll(SB_VERT , m_posScroll.y);

	Invalidate();
}


void	Ci3ImageViewerDlg::ResetScroll(int nBar , LONG& pos)
{
	pos = 0;
	SetScrollRange(nBar , 0,0,TRUE);
	SetScrollPos(nBar , 0	,TRUE);
}

void Ci3ImageViewerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int ScrollAmount = 0;
	// TODO: Add your message handler code here and/or call default
	switch(nSBCode)
	{
		case SB_LINELEFT:		ScrollAmount -= 1;		break;
		case SB_LINERIGHT:		ScrollAmount += 1;		break;
		case SB_PAGELEFT:		ScrollAmount -= 10;		break;
		case SB_PAGERIGHT:		ScrollAmount += 10;		break;
		case SB_THUMBTRACK:		ScrollAmount  =	nPos - m_posScroll.x;	break;
	}

	Scroll(SB_HORZ , ScrollAmount , m_posScroll.x);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void Ci3ImageViewerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{	
	int ScrollAmount = 0;
	// TODO: Add your message handler code here and/or call default
	switch(nSBCode)
	{
		case SB_LINEUP:			ScrollAmount -= 1;		break;
		case SB_LINEDOWN:		ScrollAmount += 1;		break;
		case SB_PAGEUP:			ScrollAmount -= 10;		break;
		case SB_PAGEDOWN:		ScrollAmount += 10;		break;
		case SB_THUMBTRACK:		ScrollAmount  =	nPos - m_posScroll.y;	break;
	}

	Scroll(SB_VERT , ScrollAmount , m_posScroll.y);
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


//스크롤의 한계값을 결정하여 포지션 결정
void Ci3ImageViewerDlg::Scroll(int nBar , int ScrollAmount , LONG& pos)
{
	int Min , Max;
	GetScrollRange(nBar , &Min , &Max);
	if(Max == 100)		return ;

	if	   (pos + ScrollAmount >= Max)	{	ScrollAmount = Max - pos; pos = Max;	}
	else if(pos + ScrollAmount <= Min)	{	ScrollAmount = pos - Min; pos = Min;	}
	else								{	pos += ScrollAmount;	}

	SetScrollPos(nBar , pos, TRUE);
	Invalidate(false);
}

void Ci3ImageViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_WndSize.cx = cx ; 
	m_WndSize.cy = cy ;

	SetScroll();
}


void Ci3ImageViewerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_RButtonDown)
	{
		int dX = point.x - m_RDragStart.x;
		int dY = point.y - m_RDragStart.y;

		Scroll(SB_HORZ ,  - dX , m_posScroll.x);
		Scroll(SB_VERT ,  - dY , m_posScroll.y);

		m_RDragStart.x = point .x;
		m_RDragStart.y = point .y;
	}

	m_posMouse.x = point.x;
	m_posMouse.y = point.y;	

	CRect Rt( 80 , 200 , 200 , 260 );
	this->InvalidateRect(&Rt);

	if( m_LButtonDown )
	{
		m_LDragRect.left	-= 1;
		m_LDragRect.top		-= 1;
		m_LDragRect.right	+= 1;
		m_LDragRect.bottom	+= 1;
		this->InvalidateRect(&m_LDragRect);
	}

	CDialog::OnMouseMove(nFlags, point);
}

BOOL Ci3ImageViewerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if(zDelta >= 0)	m_Zoom += 0.25f;
	if(zDelta < 0)	m_Zoom -= 0.25f;

	if(m_Zoom <= 0.25)	m_Zoom = 0.25f;
	if(m_Zoom >= 3.0f)	m_Zoom = 3.0f;

	SetScroll();
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void Ci3ImageViewerDlg::OnViewProperty()
{
	// TODO: Add your command handler code here
	m_bDrawProperty = !m_bDrawProperty;
	Invalidate();
}

void Ci3ImageViewerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_posMouse.x = point.x;
	m_posMouse.y = point.y;

	m_LDragStart.x = point.x;
	m_LDragStart.y = point.y;

	m_LButtonDown	= TRUE;
	SetCapture();

	Invalidate();
	CDialog::OnLButtonDown(nFlags, point);
}

void Ci3ImageViewerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_LButtonDown	= FALSE;
	::ReleaseCapture();
	Invalidate();
	CDialog::OnLButtonUp(nFlags, point);
}

void Ci3ImageViewerDlg::MoveClutWindow()
{
	if( m_pClutTableDlg )
	{
		// Set Clut Window Pos
		CRect RtParent,RtDlg;
		this->GetWindowRect(&RtParent);
		m_pClutTableDlg->GetWindowRect(&RtDlg);
		
		RtDlg.right		= RtDlg.right - RtDlg.left;
		RtDlg.bottom	= RtDlg.bottom - RtDlg.top;

		RtDlg.left		= RtParent.right;
		RtDlg.top		= RtParent.top;
		
		m_pClutTableDlg->MoveWindow(RtDlg.left, RtDlg.top, RtDlg.right, RtDlg.bottom);
	}
}

void Ci3ImageViewerDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	// TODO: Add your message handler code here
	MoveClutWindow();
}

void Ci3ImageViewerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar)
	{
		case 'P' :
			
			break;
	}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void Ci3ImageViewerDlg::OnFileExportmipmaps()
{
	if( m_pTexture->GetLevelCount() < 2)
		return;

	if( m_bPopMipOptionDlg)
	{
		CDlgMipmapOption dlg;

		if( dlg.Execute( ODT_EXTRACT_MIPMAP) == false)
			return;
	}

	INT32 i, w, h;
	char szPath[ MAX_PATH], szName[ MAX_PATH];
	//HRESULT rv;
	I3G_IMAGE_FORMAT fmt;

	w = m_pTexture->GetWidth();
	h = m_pTexture->GetHeight();
	fmt = m_pTexture->GetFormat();

//	i3String::SplitFileName( m_pTexture->GetName(), szName, FALSE);
	i3::extract_filetitle(m_pTexture->GetName(), szName);
	
	for( i = 0; i < (INT32) m_pTexture->GetLevelCount(); i++)
	{
		i3Texture * pNewTex;

		pNewTex = i3Texture::new_object();
		pNewTex->Create( w, h, 1, fmt, 0);

		{
			i3Surface * pSrcSurface = m_pTexture->getSurface( i);
			i3Surface * pDestSurface = pNewTex->getSurface( 0);

			pSrcSurface->Lock();
			pDestSurface->Lock();

			INT32 sz = i3Gfx::CalcRasterBufferSize( w, h, 1, fmt);
			memcpy( pDestSurface->getLockedBuffer(), pSrcSurface->getLockedBuffer(), sz);

			pSrcSurface->Unlock();
			pDestSurface->Unlock();
		}

		{
			i3ImageFile file;

			if( g_pOption->m_bExportI3I)
			{
				sprintf( szPath, "%s\\%s_MIP%d.i3i", g_pOption->m_szExportPath, szName, i);
				file.SaveI3I( szPath, pNewTex);
			}
			else
			{
				sprintf( szPath, "%s\\%s_MIP%d.dds", g_pOption->m_szExportPath, szName, i);
				//file.SaveDDS( szPath, pNewTex);

				IDirect3DTexture9 * pDXTex = ((i3TextureDX *) pNewTex)->GetDXImage();
				CHECKRESULT( D3DXSaveTextureToFile( szPath, D3DXIFF_DDS, pDXTex, NULL));
			}
		}

		pNewTex->Release();
		w = w >> 1;
		h = h >> 1;
	}
}

void Ci3ImageViewerDlg::OnFileGeneratemipmaps()
{
	if( m_pTexture->GetLevelCount() < 2)
		return;

	{
		CDlgMipmapOption dlg;

		if( dlg.Execute( ODT_GENERATE_MIPMAP) == false)
			return;
	}

	{
		IDirect3DTexture9 * pDXTex = ((i3TextureDX *)m_pTexture)->GetDXImage();
		DWORD filter;

		switch( g_pOption->m_Filter)
		{
			case tex::FILTER_NONE :		filter = D3DX_FILTER_NONE;	break;
			case tex::FILTER_POINT :	filter = D3DX_FILTER_POINT;	break;
			case tex::FILTER_LINEAR :	filter = D3DX_FILTER_LINEAR;	break;
			case tex::FILTER_TRIANGLE :	filter = D3DX_FILTER_TRIANGLE;	break;
			default :					filter = D3DX_FILTER_BOX;	break;
		}

		if( g_pOption->m_bDither)
			filter |= D3DX_FILTER_DITHER;

		if( g_pOption->m_bSRGB_In)
			filter |= D3DX_FILTER_SRGB_IN;

		if( g_pOption->m_bSRGB_Out)
			filter |= D3DX_FILTER_SRGB_OUT;

		CHECKRESULT( D3DXFilterTexture( pDXTex, NULL, 0, filter));
	}

	m_bPopMipOptionDlg = false;

	OnFileExportmipmaps();

	m_bPopMipOptionDlg = true;

}

void Ci3ImageViewerDlg::OnFileLoadmipmaps()
{
	if( m_pTexture->GetLevelCount() < 2)
		return;

	{
		CDlgMipmapOption dlg;

		if( dlg.Execute( ODY_RELOAD_MIPMAP) == false)
			return;
	}

	UINT32 i, w, h;
	char szPath[ MAX_PATH], szName[ MAX_PATH];
	//HRESULT rv;
	I3G_IMAGE_FORMAT fmt;

	w = m_pTexture->GetWidth();
	h = m_pTexture->GetHeight();
	fmt = m_pTexture->GetFormat();

//	i3String::SplitFileName( m_pTexture->GetName(), szName, FALSE);
	i3::extract_filetitle( m_pTexture->GetName(), szName);
	
	for( i = 0; i <  m_pTexture->GetLevelCount(); i++)
	{
		i3Texture * pNewTex;

		if( g_pOption->m_bExportI3I)
		{
			i3ImageFile file;

			sprintf( szPath, "%s\\%s_MIP%d.i3i", g_pOption->m_szExportPath, szName, i);
		}
		else
		{
			sprintf( szPath, "%s\\%s_MIP%d.dds", g_pOption->m_szExportPath, szName, i);
		}

		{
			i3ImageFile file;

			pNewTex = file.Load( szPath);
			if( pNewTex == NULL)
			{
				NotifyBox( m_hWnd, "%s 파일을 읽어 들일 수 없습니다.", szPath);
				return;
			}
		}

		if( (pNewTex->GetWidth() != w) || (pNewTex->GetHeight() != h) || (pNewTex->GetFormat() != fmt))
		{
			NotifyBox( m_hWnd, "%s 파일을 읽어들였으나, 형식이 맞지 않습니다.\r\n예상 크기:%dx%d  -  실제 크기:%dx%d\r\n예상 형식:%s  -  실제 형식:%s", 
				szPath,
				w, h,
				pNewTex->GetWidth(), pNewTex->GetHeight(),
				i3Gfx::GetImageFormatName( fmt),
				i3Gfx::GetImageFormatName( pNewTex->GetFormat()));

			return;
		}

		{
			i3Surface * pDestSurface = m_pTexture->getSurface( i);
			i3Surface * pSrcSurface = pNewTex->getSurface( 0);

			pSrcSurface->Lock();
			pDestSurface->Lock();

			INT32 sz = i3Gfx::CalcRasterBufferSize( w, h, 1, fmt);
			memcpy( pDestSurface->getLockedBuffer(), pSrcSurface->getLockedBuffer(), sz);

			pSrcSurface->Unlock();
			pDestSurface->Unlock();
		}

		pNewTex->Release();
		w = w >> 1;
		h = h >> 1;
	}
}

void Ci3ImageViewerDlg::OnFileSave()
{
	if( m_szFileName[0] == 0)
	{
		OnFileSaveas();
		return;
	}
	OnFileSaveas();
	//SaveImageFile( m_szFileName);
}

void Ci3ImageViewerDlg::OnFileSaveas()
{
 	Ci3ImageMakerDlg	*pDlg = new Ci3ImageMakerDlg;
	pDlg->Create(IDD_i3ImageMaker_DIALOG);
	pDlg->ShowWindow(SW_HIDE);
	pDlg->OnBnClickedOk();
 	
	delete pDlg;
	//Ci3ImageMakerDlg dlg;
	//dlg.OnBnClickedOk();
	//dlg.DoModal();
}

void Ci3ImageViewerDlg::OnGridDrawgrid()
{
	// TODO: Add your command handler code here
	CMenu * pMenu = GetMenu();
	UINT Rv = pMenu->GetMenuState(ID_GRID_DRAWGRID, MF_BYCOMMAND);
	if ( Rv & MF_CHECKED )
	{
		m_bDrawGrid = false;
		pMenu->CheckMenuItem(ID_GRID_DRAWGRID, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else
	{
		m_bDrawGrid = true;
		pMenu->CheckMenuItem(ID_GRID_DRAWGRID, MF_BYCOMMAND | MF_CHECKED);
	}

	Invalidate();
}

void Ci3ImageViewerDlg::OnGridGridsetting()
{
	// TODO: Add your command handler code here
	CGridSettingDlg	dlg(this, m_nGridVertBlockCount, m_nGridHoriBlockCount, m_nPenIndex);
	if ( IDOK != dlg.DoModal() )
		return;

	// 처음 시작시에 Grid 
	CMenu * pMenu = GetMenu();
	pMenu->EnableMenuItem(ID_GRID_DRAWGRID, MF_BYCOMMAND | MF_ENABLED);
	pMenu->CheckMenuItem(ID_GRID_DRAWGRID, MF_BYCOMMAND | MF_CHECKED);

	m_bDrawGrid			= true;
	m_nGridHoriBlockCount	= dlg.GetHoriCount();
	m_nGridVertBlockCount	= dlg.GetVertCount();
	m_nPenIndex			= dlg.GetPenIndex();

	Invalidate();
}

void Ci3ImageViewerDlg::DrawGrid(CDC * pDC, CPoint StartPos)
{
	I3ASSERT(pDC != NULL);
	if ( ! m_bDrawGrid )
		return;

	pDC->SelectStockObject(m_nPenIndex);

	INT32 HoriBlockCount = m_nGridHoriBlockCount;	// 수평선 개수
	INT32 VertBlockCount = m_nGridVertBlockCount;

	// 수평선을 그립니다.
	{
		INT32 HeightInterval = m_nTotalHeight / HoriBlockCount;		// 블록 간격
		INT32 AccumHeightSize = 0;	// 블록 누적 사이즈
		INT32 HoriLineCount = HoriBlockCount - 1;
		CPoint	PtLeft, PtRight;
		PtLeft.x = StartPos.x - 10;
		PtRight.x = StartPos.x + m_nTotalWidth + 10;
		for ( INT32 i = 0 ; i < HoriLineCount ; ++i )
		{
			PtLeft.y = PtRight.y = StartPos.y + AccumHeightSize + HeightInterval;

			pDC->MoveTo(PtLeft);
			pDC->LineTo(PtRight);

			AccumHeightSize += HeightInterval;
		}
	}

	// 수직선을 그립니다.
	{
		INT32 WidthInterval = m_nTotalWidth / VertBlockCount;		// 블록 간격
		INT32 AccumWidthSize = 0;	// 블록 누적 사이즈
		INT32 WidthLineCount = VertBlockCount - 1;
		CPoint	PtTop, PtBottom;
		PtTop.y = StartPos.y - 10;
		PtBottom.y = StartPos.y + m_nTotalHeight + 10;
		for ( INT32 i = 0 ; i < WidthLineCount ; ++i )
		{
			PtTop.x = PtBottom.x = StartPos.x + AccumWidthSize + WidthInterval;

			pDC->MoveTo(PtTop);
			pDC->LineTo(PtBottom);

			AccumWidthSize += WidthInterval;
		}
	}

	return;
}

