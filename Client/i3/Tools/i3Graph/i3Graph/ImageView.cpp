// ImageView.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "ImageView.h"
#include ".\imageview.h"

// CImageDlg dialog
#define	IMGDLG_LEFTMARGIN	0
#define	IMGDLG_TOPMARGIN	0
#define IMGDLG_RIGHTMARGIN	15
#define IMGDLG_BOTTOMMARGIN 25
#define IMGDLG_CONTROLHEGHT 20

CSize	WndSize(0,0);
// CImageView

IMPLEMENT_DYNAMIC(CImageView, CWnd)
CImageView::CImageView()
{
	m_pTexture		= NULL;
	m_bAlphaMode	= FALSE;
	m_AlphaValue	= RGB(255,255,255);
	m_posScroll.x	= 0;
	m_posScroll.y	= 0;

	m_RButtonDown = FALSE;
	m_DragStart.x = 0 ; 
	m_DragStart.y = 0 ; 

	m_Zoom = 1.0f;
}

CImageView::CImageView(i3Texture * pTexture)
{
	m_bAlphaMode	= FALSE;
	m_AlphaValue	= RGB(255,255,255);
	m_posScroll.x	= 0;
	m_posScroll.y	= 0;

	m_RButtonDown = FALSE;
	m_DragStart.x = 0 ; 
	m_DragStart.y = 0 ; 

	m_Zoom = 1.0f;

	SetImage(pTexture);
}

CImageView::~CImageView()
{
	if(m_pTexture)	m_pTexture->Release();

	DeleteImage();
}


BEGIN_MESSAGE_MAP(CImageView, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_DROPFILES()
	ON_WM_MBUTTONDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CImageView message handlers
BOOL CImageView::Create( DWORD	dwStyle, CWnd* pParentWnd,UINT	nID	)
{
	// TODO: Add your specialized code here and/or call the base class
	LPCTSTR lpszClassName;

	lpszClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
										::LoadCursor(NULL, IDC_ARROW),
							   (HBRUSH) ::GetStockObject(WHITE_BRUSH),
										::LoadIcon(NULL, IDI_APPLICATION));

	dwStyle |= WS_VISIBLE | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_VSCROLL | WS_HSCROLL;

	DWORD	dwExStyle		= WS_EX_ACCEPTFILES;
	LPCTSTR lpszWindowName	= "Image";

	RECT rect;
	rect.left = 0;	rect.top = 0 ; 	rect.right = 1;	rect.bottom = 1;


	return CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, NULL);
}

void	CImageView::LoadImageFile(char * lpstrFile)
{
	i3Texture * pTex;
	
	pTex = m_pImageFile.Load(lpstrFile);

	if( pTex == NULL)		return;

	SetImage(pTex);
}

void CImageView::OpenImageFile()
{
	char lpstrFile[1024] = "";//얻어올 파일명
	OPENFILENAME	ofn;
	{
		memset(&ofn,0,sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner	= this->m_hWnd;
		ofn.lpstrFilter = "TGA파일(*.tga)\0*.tga\0비트맵파일(*.bmp)\0*.bmp\0";
		ofn.lpstrFile   = lpstrFile;
		ofn.nMaxFile	= 1024;
	}
	

	if(GetOpenFileName(&ofn) != 0)
	{
		LoadImageFile(lpstrFile);
	}
}


void CImageView::DeleteImage()
{
	if( m_MemDC != NULL)
	{
		if( m_hBitmap != NULL)
		{
			SelectObject( m_MemDC, m_hOldBitmap);

			DeleteObject( m_hBitmap);

			m_hBitmap = NULL;
		}
		DeleteDC( m_MemDC);

		m_MemDC = NULL;
	}

	if(	m_PaintDC != NULL)
	{
		if(m_hPaintBitmap != NULL)
		{
			SelectObject(m_PaintDC,m_hOldPaintBitmap);

			DeleteObject(m_hPaintBitmap);

			m_hPaintBitmap = NULL;
		}
		DeleteDC(m_PaintDC);

		m_PaintDC = NULL;
	}

}


void	CImageView::SetImage(i3Texture * pTexture)
{
	if(pTexture == NULL)	return ;

	if(m_pTexture)	m_pTexture->Release();
	m_pTexture = pTexture;

	DeleteImage();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Bitmap
	BITMAPINFOHEADER fi;
	PVOID pRaster;
	BITMAPINFO ih;

	//
	INT32		Height = - (INT32)pTexture->GetHeight();					//이미지가 거꾸로 들어와있는상태라height 는 - 
	I3G_IMAGE_FORMAT	DestFormat	 = I3G_IMAGE_FORMAT_BGRX_8888;  
	UINT16				DestBitCount = 32;

	::ZeroMemory(&fi,sizeof(BITMAPINFOHEADER));
	fi.biSize		= sizeof(BITMAPINFOHEADER);
	fi.biBitCount	= DestBitCount;

	fi.biHeight		= Height;
	fi.biWidth		= pTexture->GetWidth();
	fi.biPlanes		= 1;
	fi.biCompression= BI_RGB;

	memcpy(&ih.bmiHeader,&fi,sizeof(BITMAPINFOHEADER));

	m_hBitmap = CreateDIBSection(NULL,&ih,DIB_RGB_COLORS,&pRaster,NULL,0);
	
	char * src		= pTexture->Lock(0);
	char * pDest	= (char*)pRaster;

	i3Image_Convert( pTexture->GetFormat(), src, pTexture->GetWidth(), pTexture->GetHeight(), 1, pTexture->GetClut(0),
					 DestFormat, pDest, 4, NULL);


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create DC
	HDC hdc = ::GetDC(this->GetSafeHwnd());
	
	//For Image
	m_MemDC = CreateCompatibleDC(hdc);
	m_hOldBitmap = (HBITMAP) ::SelectObject( m_MemDC, m_hBitmap);

	//For Paint
	m_hPaintBitmap = CreateCompatibleBitmap(hdc , pTexture->GetWidth() , pTexture->GetHeight());
	m_PaintDC = CreateCompatibleDC(hdc);
	m_hOldPaintBitmap = (HBITMAP)	SelectObject(m_PaintDC , m_hPaintBitmap);

	MakeDCforDraw();
	::ReleaseDC(m_hWnd,hdc);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	pTexture->Unlock();

	
	m_Zoom = 1.0f;
	SetScroll();
}



void CImageView::MakeDCforDraw()
{
	if(m_bAlphaMode)//alpha view이면 아래에 그리드 깔고 위에 투명처리된 이미지를 덮어씌운다.
	{
		SetGridRect(m_PaintDC ,0, 0,  m_pTexture->GetWidth() ,m_pTexture->GetHeight());


		::TransparentBlt(m_PaintDC,IMGDLG_LEFTMARGIN , IMGDLG_TOPMARGIN ,m_pTexture->GetWidth(),m_pTexture->GetHeight(),
			             m_MemDC  ,                 0,                 0,m_pTexture->GetWidth(),m_pTexture->GetHeight(),
						 m_AlphaValue);
	}
	else
		::BitBlt(m_PaintDC,IMGDLG_LEFTMARGIN , IMGDLG_TOPMARGIN,m_pTexture->GetWidth(),	m_pTexture->GetHeight(),
				 m_MemDC  ,					0,				  0,
				 SRCCOPY);
}

//스크롤의 한계값을 결정하여 스크롤
void CImageView::Scroll(int nBar , int ScrollAmount , LONG& pos)
{
	int Min , Max;
	GetScrollRange(nBar , &Min , &Max);

	if(Max == 100)		return ;//range가 max == min이되어 스크롤 바가 사라지면 min = 0 , max = 100의 기본값을가진 
								//원래상태의 스크롤 값을 가지게되는것같다.
	
	if	   (pos + ScrollAmount >= Max)	{	pos = Max;	}
	else if(pos + ScrollAmount <= Min)	{	pos = Min;	}
	else								{	pos += ScrollAmount;	}

	SetScrollPos(nBar , pos, TRUE);
	Invalidate();
}

void CImageView::SetGridRect(HDC  hdc ,int x ,int y ,  INT32 Width , INT32 Height)
{
	BOOL OnWidth = FALSE;
	BOOL OnHeight = TRUE;

	for(INT32 i = 0 ; i  < Height ; ++i)
	{
		OnWidth = !OnHeight;
		for(INT32 j = 0 ; j < Width ; ++j)
		{
			if(OnWidth)
				SetPixel(hdc, i+x , j+y , RGB(255,255,255));
			else
				SetPixel(hdc, i+x , j+y , RGB(150,150,150));

			if(j%32 == 31) OnWidth = !OnWidth;
		}

		if(i%32 == 31) OnHeight  =  !OnHeight;
	}
}

void	CImageView::ResetScroll(int nBar , LONG& pos)
{
	pos = 0;
	SetScrollRange	(nBar , 0,0,TRUE);
	SetScrollPos	(nBar , 0,TRUE);
}

void	CImageView::SetScroll()
{
	int Min , Max , Factor;
	REAL32	magnification;
	

	//현재 출력될 이미지의 크리보다 윈도의 크기가 작으면 범위와 위치 초기화(max == min , pos = 0)
	//현재 출력될 이미지의 크기보다 윈도의 크기가 작으면 범위와 위치 계산
	//SB_HORZ////////////////////////////////////////////////////////////////////////////////////
	if(WndSize.cx > (int)(m_pTexture->GetWidth() * m_Zoom))	ResetScroll(SB_HORZ , m_posScroll.x);
	else
	{
		//SetScrollRange
		GetScrollRange(SB_HORZ , &Min , &Max );
		Factor = ((int)(m_pTexture->GetWidth() * m_Zoom)  -  WndSize.cx);
		SetScrollRange(SB_HORZ , 0 , Factor, TRUE);

		//SetScrollpos
		if(Max <= 0)	m_posScroll.x = 0;
		else
		{
			magnification = (REAL32)Factor / Max ; 
			m_posScroll.x = (int)((REAL32)m_posScroll.x * magnification) ; 
		}

		SetScrollPos(SB_HORZ , m_posScroll.x , TRUE);
	}

	//SB_VERT////////////////////////////////////////////////////////////////////////////////////
	if(WndSize.cy > (int)(m_pTexture->GetHeight() * m_Zoom))	ResetScroll(SB_VERT , m_posScroll.y);
	else
	{
		GetScrollRange(SB_VERT , &Min , &Max );
		Factor = ((int)((REAL32)m_pTexture->GetHeight() * m_Zoom)  -  WndSize.cy);
		SetScrollRange(SB_VERT , 0 , Factor , TRUE);

		if(Max <= 0)	m_posScroll.y = 0;
		else
		{
			magnification = (REAL32)Factor / Max ; 
			m_posScroll.y = (int)((REAL32)m_posScroll.y * magnification ); 
		}

		SetScrollPos(SB_VERT , m_posScroll.y , TRUE);
	}

	Invalidate();
}

void CImageView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
	if(m_pTexture == NULL)	return ;

	//이미지의 크기가 윈도의 크기보다 작은경우 남는영역을 브러쉬로 채워준다.
	//ERASEBKGND사용시 깜빡거림을 해소하기위한 방편 ;;
	dc.SelectStockObject(NULL_PEN);
	dc.SelectStockObject(LTGRAY_BRUSH);
	CRect rt ; 
	if(m_Zoom * m_pTexture->GetWidth() < (REAL32)WndSize.cx)
	{
		rt.left = (int)( m_Zoom * m_pTexture->GetWidth());
		rt.right = WndSize.cx + 1;
		rt.top = 0 ; 
		rt.bottom = WndSize.cy + 1;
	}
	dc.Rectangle(&rt);

	if(m_Zoom * m_pTexture->GetHeight() < (REAL32)WndSize.cy)
	{
		rt.left = 0 ; 
		rt.right = WndSize.cy + 1;
		rt.top = (int)( m_Zoom * m_pTexture->GetHeight());
		rt.bottom = WndSize.cy + 1;
	}
	dc.Rectangle(&rt);


	//이미지 출력, 스크롤된 위치에 따라서 소스이미지의 스타트 포지션을 변경하여 이미지를 스크롤 시킴
	StretchBlt(dc.m_hDC,0 , 0,
					WndSize.cx,
					WndSize.cy,
					m_PaintDC ,
					(int)(m_posScroll.x / m_Zoom),
					(int)(m_posScroll.y / m_Zoom),
					(int)(WndSize.cx	/ m_Zoom),
					(int)(WndSize.cy	/ m_Zoom), 
					 SRCCOPY);
					 //*/
}

void CImageView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	WndSize.cx = cx ; 
	WndSize.cy = cy ;

	if(m_pTexture)	SetScroll();
}

void CImageView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = TRUE;

	::SetCapture(m_hWnd);

	m_DragStart.x = point .x;
	m_DragStart.y = point .y;

	CWnd::OnRButtonDown(nFlags, point);
}

void CImageView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = FALSE;
	::ReleaseCapture();

	CWnd::OnRButtonUp(nFlags, point);
}

void CImageView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_RButtonDown)
	{
		//이동한양 계산
		int dX = point.x - m_DragStart.x;
		int dY = point.y - m_DragStart.y;

		//이동한양만큼 스크롤
		Scroll(SB_HORZ ,  - dX , m_posScroll.x);
		Scroll(SB_VERT ,  - dY , m_posScroll.y);

		//현재위치 저장
		m_DragStart.x = point .x;
		m_DragStart.y = point .y;
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CImageView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
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

	//ScrollAmount만큼 스크롤
	Scroll(SB_HORZ , ScrollAmount , m_posScroll.x);

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImageView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
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

	//ScrollAmount만큼 스크롤
	Scroll(SB_VERT , ScrollAmount , m_posScroll.y);

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CImageView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	{//zoom의 크기 결정
		if(zDelta >= 0)	m_Zoom += 0.25f;
		if(zDelta < 0)	m_Zoom -= 0.25f;

		if(m_Zoom <= 0.25)	m_Zoom = 0.25f;
		if(m_Zoom >= 3.0f)	m_Zoom = 3.0f;
	}

	SetScroll();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CImageView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	if(m_pTexture == NULL)	return CWnd::OnEraseBkgnd(pDC);
	else	return TRUE;
}
void CImageView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	int count; 
	char FileName[256];

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	if(count > 1)
	{
		//Error 한번에 하나만 로딩할수 있다. 
		CWnd::OnDropFiles(hDropInfo);
		return; 
	}

	DragQueryFile( hDropInfo, 0, FileName, sizeof(FileName) - 1);
	if ( CheckFileName(FileName) == FALSE ) 
	{
		//Message 
		CWnd::OnDropFiles(hDropInfo);
		return;
	}

	//여기부터 실제 적인 씬 파일 로드 입니다. 
	LoadImageFile(FileName);

	CWnd::OnDropFiles(hDropInfo);
}

BOOL CImageView::CheckFileName(char * FileName)
{
	char Ext[32];

	i3String::SplitFileExt(FileName, Ext, sizeof(Ext) - 1);
	if( i3String::Compare( (char *)Ext, (char *)"BMP") == 0)		return TRUE;
	if(i3String::Compare((char*)Ext,(char *)"TGA") ==0)				return TRUE;

	//Error ImageFile이 아니다.
	return FALSE; 
}

void CImageView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bAlphaMode = !m_bAlphaMode;	//alpha모드와 normal모드 토글

	MakeDCforDraw();				//모드변화에따른 백버퍼 준비

	Invalidate();					//onpaint()
	CWnd::OnMButtonDown(nFlags, point);
}

int CImageView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  Add your specialized creation code here
	return 0;
}

BOOL CImageView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.cx = 512;
	cs.cy = 512;

	return CWnd::PreCreateWindow(cs);
}
