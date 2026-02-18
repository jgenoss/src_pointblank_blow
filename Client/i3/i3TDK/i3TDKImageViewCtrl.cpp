// ImageView.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKImageViewCtrl.h"
#include "i3Base/string/ext/extract_fileext.h"

// CImageDlg dialog
#define	IMGDLG_LEFTMARGIN	0
#define	IMGDLG_TOPMARGIN	0
#define IMGDLG_RIGHTMARGIN	15
#define IMGDLG_BOTTOMMARGIN 25
#define IMGDLG_CONTROLHEGHT 20

// i3TDKImageViewCtrl

IMPLEMENT_DYNAMIC(i3TDKImageViewCtrl, CWnd)

i3TDKImageViewCtrl::i3TDKImageViewCtrl(i3Texture * pTexture)
{
	SetImage(pTexture);
}

i3TDKImageViewCtrl::~i3TDKImageViewCtrl()
{
	I3_SAFE_RELEASE(m_pTexture);

	DeleteImage();
}


BEGIN_MESSAGE_MAP(i3TDKImageViewCtrl, CWnd)
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
	ON_WM_CREATE()
END_MESSAGE_MAP()

// i3TDKImageViewCtrl message handlers
BOOL i3TDKImageViewCtrl::Create( DWORD	dwStyle, CWnd* pParentWnd,UINT	nID	)
{
	// TODO: Add your specialized code here and/or call the base class
	LPCTSTR lpszClassName;

	lpszClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
										::LoadCursor(nullptr, IDC_ARROW),
							   (HBRUSH) ::GetStockObject(WHITE_BRUSH),
										::LoadIcon(nullptr, IDI_APPLICATION));

	dwStyle |= WS_VSCROLL | WS_HSCROLL | WS_TABSTOP;

	DWORD	dwExStyle		= WS_EX_ACCEPTFILES;
	LPCTSTR lpszWindowName	= _T("Image");

	RECT rect;
	rect.left = 0;	rect.top = 0 ; 	rect.right = 1;	rect.bottom = 1;


	return CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, nullptr);
}

void	i3TDKImageViewCtrl::LoadImageFile(const TCHAR* lpstrFile)
{
	i3Texture * pTex;
	
#ifdef _UNICODE
	i3::stack_string strFilePath;	i3::utf16_to_mb(lpstrFile, strFilePath);
	const char* szFilePath = strFilePath.c_str();
#else
	const char* szFilePath = lpstrFile;
#endif

	pTex = m_pImageFile.Load(szFilePath);

	if( pTex == nullptr)		return;

	SetImage(pTex);
}

void i3TDKImageViewCtrl::OpenImageFile()
{
	TCHAR lpstrFile[1024] = _T("");//얻어올 파일명
	OPENFILENAME	ofn;
	{
		memset(&ofn,0,sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner	= this->m_hWnd;
		ofn.lpstrFilter = _T("TGA파일(*.tga)\0*.tga\0비트맵파일(*.bmp)\0*.bmp\0i3i 파일(*.i3i)");
		ofn.lpstrFile   = lpstrFile;
		ofn.nMaxFile	= 1024;
	}
	

	if(GetOpenFileName(&ofn) != 0)
	{
		LoadImageFile(lpstrFile);
	}
}


void i3TDKImageViewCtrl::DeleteImage()
{
	if( m_MemDC != nullptr)
	{
		if( m_hBitmap != nullptr)
		{
			SelectObject( m_MemDC, m_hOldBitmap);

			DeleteObject( m_hBitmap);

			m_hBitmap = nullptr;
		}
		DeleteDC( m_MemDC);

		m_MemDC = nullptr;

		ResetScroll(SB_HORZ, m_posScroll.x);
		ResetScroll(SB_VERT, m_posScroll.y);
	}
}

void i3TDKImageViewCtrl::SetViewMode( VIEW_MODE mode)
{
	if( m_ViewMode != mode)
	{
		m_ViewMode = mode;

		SetImage();
		Invalidate();
	}
}

void	i3TDKImageViewCtrl::SetImage()
{
	if(m_pTexture == nullptr)	return ;

	DeleteImage();

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Bitmap
	BITMAPINFOHEADER fi;
	PVOID pRaster;
	BITMAPINFO ih;

	//
	INT32		Height = - (INT32)m_pTexture->GetHeight();					//이미지가 거꾸로 들어와있는상태라height 는 - 
	UINT16				DestBitCount = 32;

	i3mem::FillZero( &fi,sizeof(BITMAPINFOHEADER));
	fi.biSize		= sizeof(BITMAPINFOHEADER);
	fi.biBitCount	= DestBitCount;

	fi.biHeight		= Height;
	fi.biWidth		= m_pTexture->GetWidth();
	fi.biPlanes		= 1;
	fi.biCompression= BI_RGB;

	memcpy( &ih.bmiHeader,&fi,sizeof(BITMAPINFOHEADER));

	m_hBitmap = CreateDIBSection(nullptr,&ih,DIB_RGB_COLORS,&pRaster,nullptr,0);

	if( m_MemDC == nullptr)
	{
		HDC dc;

		dc = ::GetDC( m_hWnd);

		m_MemDC = CreateCompatibleDC( dc);
		m_hOldBitmap = (HBITMAP) ::SelectObject( m_MemDC, m_hBitmap);

		::ReleaseDC( m_hWnd, dc);
	}
	
	char * src		= m_pTexture->Lock(0);
	char * pDest	= (char*)pRaster;

	i3Image_Convert( m_pTexture->GetFormat(), src, m_pTexture->GetWidth(), m_pTexture->GetHeight(), 1, m_pTexture->GetClut(0),
					 I3G_IMAGE_FORMAT_BGRA_8888, pDest, 4, nullptr);



	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create DC
	//For Image

	//if have alphachannel
	if( m_ViewMode != VIEW_RGB)
	{
		UINT8 * pPixel = (UINT8 *) pRaster;
		UINT32 i, j, back;

		for( i = 0 ; i < m_pTexture->GetHeight(); i++)
		{
			for( j = 0; j < m_pTexture->GetWidth(); j++)
			{
				switch( m_ViewMode)
				{
					case VIEW_BLEND :
						back = ((((i >> 3) ^ (j >> 3)) & 0x01) << 6) + 128;

						pPixel[0] = (UINT8)((((UINT32)pPixel[0] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						pPixel[1] = (UINT8)((((UINT32)pPixel[1] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						pPixel[2] = (UINT8)((((UINT32)pPixel[2] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
						break;

					case VIEW_R :	*((UINT32 *) pPixel)  &= 0xFFFF0000;	break;
					case VIEW_G :	*((UINT32 *) pPixel)  &= 0xFF00FF00;	break;
					case VIEW_B :	*((UINT32 *) pPixel)  &= 0xFF0000FF;	break;
					case VIEW_A :
						pPixel[0] = pPixel[3];
						pPixel[1] = pPixel[3];
						pPixel[2] = pPixel[3];
						break;
				}

				pPixel += 4;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	m_pTexture->Unlock( 0);

	
	m_Zoom = 1.0f;
	SetScroll();
}

void	i3TDKImageViewCtrl::SetImage(i3Texture * pTexture)
{

	I3_REF_CHANGE( m_pTexture, pTexture);

	if( pTexture == nullptr)
	{
		DeleteImage();
		return ;
	}

	SetImage();
}

//스크롤의 한계값을 결정하여 스크롤
void i3TDKImageViewCtrl::Scroll(int nBar , int ScrollAmount , LONG& pos)
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

void i3TDKImageViewCtrl::SetGridRect(HDC  hdc ,int x ,int y ,  INT32 Width , INT32 Height)
{
	INT32	nDivide = (INT32)(Width / 20);
	if(nDivide < 5 )	nDivide = 5;

	BOOL OnWidth = FALSE;
	BOOL OnHeight = TRUE;

	for(INT32 i = 0 ; i  < Height ; ++i)
	{
		OnWidth = !OnHeight;
		for(INT32 j = 0 ; j < Width ; ++j)
		{
			if(OnWidth)
				SetPixel(hdc, j+x , i+y , RGB(255,255,255));
			else
				SetPixel(hdc, j+x , i+y , RGB(160,160,160));

			if(j%nDivide == nDivide-1) OnWidth = !OnWidth;
		}

		if(i%nDivide == nDivide-1) OnHeight  =  !OnHeight;
	}
}

void	i3TDKImageViewCtrl::ResetScroll(int nBar , LONG& pos)
{
	pos = 0;
	SetScrollRange	(nBar , 0,0,TRUE);
	SetScrollPos	(nBar , 0,TRUE);
}

void	i3TDKImageViewCtrl::SetScroll()
{
	int Min , Max , Factor;
	REAL32	magnification;
	if (m_pTexture == nullptr) return;

	//현재 출력될 이미지의 크리보다 윈도의 크기가 작으면 범위와 위치 초기화(max == min , pos = 0)
	//현재 출력될 이미지의 크기보다 윈도의 크기가 작으면 범위와 위치 계산
	//SB_HORZ////////////////////////////////////////////////////////////////////////////////////
	if(m_WndSize.cx > (int)(m_pTexture->GetWidth() * m_Zoom))	ResetScroll(SB_HORZ , m_posScroll.x);
	else
	{
		//SetScrollRange
		GetScrollRange(SB_HORZ , &Min , &Max );
		//Factor = ((int)(m_pTexture->GetWidth() * m_Zoom)  -  m_WndSize.cx);
		Factor = (int)(m_pTexture->GetWidth() * m_Zoom);

		{
			SCROLLINFO info;

			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_PAGE | SIF_RANGE;
			info.nMax = Factor;
			info.nMin = 0;
			info.nPage = m_WndSize.cx;

			SetScrollInfo( SB_HORZ, &info);
		}

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
	if(m_WndSize.cy > (int)(m_pTexture->GetHeight() * m_Zoom))	ResetScroll(SB_VERT , m_posScroll.y);
	else
	{
		GetScrollRange(SB_VERT , &Min , &Max );
		//Factor = ((int)((REAL32)m_pTexture->GetHeight() * m_Zoom)  -  m_WndSize.cy);
		Factor = (int)(m_pTexture->GetHeight() * m_Zoom);

		{
			SCROLLINFO info;

			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_PAGE | SIF_RANGE;
			info.nMax = Factor;
			info.nMin = 0;
			info.nPage = m_WndSize.cy;

			SetScrollInfo( SB_VERT, &info);
		}

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

void i3TDKImageViewCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	OnDraw( dc.m_hDC);
}

void i3TDKImageViewCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	m_WndSize.cx = cx ; 
	m_WndSize.cy = cy ;

	if(m_pTexture)
	{
		SetScroll();
	}
	else
	{
		ResetScroll(SB_HORZ, m_posScroll.x);
		ResetScroll(SB_VERT, m_posScroll.y);
	}
}

void i3TDKImageViewCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = TRUE;

	::SetCapture(m_hWnd);

	m_DragStart.x = point .x;
	m_DragStart.y = point .y;

	CWnd::OnRButtonDown(nFlags, point);
}

void i3TDKImageViewCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = FALSE;
	::ReleaseCapture();

	CWnd::OnRButtonUp(nFlags, point);
}

void i3TDKImageViewCtrl::OnMouseMove(UINT nFlags, CPoint point)
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

void i3TDKImageViewCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void i3TDKImageViewCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

BOOL i3TDKImageViewCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
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

BOOL i3TDKImageViewCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	if(m_pTexture == nullptr)	return CWnd::OnEraseBkgnd(pDC);
	else	return TRUE;
}
void i3TDKImageViewCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	int count; 
	TCHAR FileName[256];

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, nullptr, 0);

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

BOOL i3TDKImageViewCtrl::CheckFileName(const TCHAR* FileName) const
{
	TCHAR Ext[32];

//	i3String::SplitFileExt(FileName, Ext, sizeof(Ext) - 1);
	i3::extract_fileext(FileName, Ext);

	if(i3::generic_is_iequal( Ext, _T("BMP") ) )		return TRUE;
	if(i3::generic_is_iequal(Ext, _T("TGA") ) )		return TRUE;
	if(i3::generic_is_iequal(Ext, _T("I3I") ) )		return TRUE;

	//Error ImageFile이 아니다.
	return FALSE; 
}

int i3TDKImageViewCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL i3TDKImageViewCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.cx = 512;
	cs.cy = 512;

	return CWnd::PreCreateWindow(cs);
}

void i3TDKImageViewCtrl::OnDraw( HDC dc)
{
	CRect rt ; 

	//이미지의 크기가 윈도의 크기보다 작은경우 남는영역을 브러쉬로 채워준다.
	//ERASEBKGND사용시 깜빡거림을 해소하기위한 방편 ;;
	::SelectObject( dc, GetStockObject( NULL_PEN));
	::SelectObject( dc, GetStockObject( LTGRAY_BRUSH));

	if(m_pTexture == nullptr)
	{
		Rectangle( dc, 0, 0, m_WndSize.cx + 1, m_WndSize.cy + 1);
		return;
	}
	
	
	if(m_Zoom * m_pTexture->GetWidth() < (REAL32)m_WndSize.cx)
	{
		rt.left = (int)( m_Zoom * m_pTexture->GetWidth());
		rt.right = m_WndSize.cx + 1;
		rt.top = 0 ; 
		rt.bottom = m_WndSize.cy + 1;
	}
	Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);

	if(m_Zoom * m_pTexture->GetHeight() < (REAL32)m_WndSize.cy)
	{
		rt.left = 0 ; 
		rt.right = m_WndSize.cy + 1;
		rt.top = (int)( m_Zoom * m_pTexture->GetHeight());
		rt.bottom = m_WndSize.cy + 1;
	}
	Rectangle( dc, rt.left, rt.top, rt.right, rt.bottom);

	int oldMode = SetStretchBltMode(dc, COLORONCOLOR);
	//이미지 출력, 스크롤된 위치에 따라서 소스이미지의 스타트 포지션을 변경하여 이미지를 스크롤 시킴
	StretchBlt( dc, 0 , 0,
					m_WndSize.cx,
					m_WndSize.cy,
					m_MemDC ,
					(int)(m_posScroll.x / m_Zoom),
					(int)(m_posScroll.y / m_Zoom),
					(int)(m_WndSize.cx	/ m_Zoom),
					(int)(m_WndSize.cy	/ m_Zoom), 
					 SRCCOPY);

	SetStretchBltMode(dc, oldMode);
}