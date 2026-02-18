// ImageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "ImageDlg.h"
#include ".\imagedlg.h"
#include "resource.h"
#include "i3GraphDlg.h"

#define	IMGDLG_LEFTMARGIN	0
#define	IMGDLG_TOPMARGIN	0
#define IMGDLG_RIGHTMARGIN	7
#define IMGDLG_BOTTOMMARGIN 17
#define IMGDLG_CONTROLHEGHT 20
	
IMPLEMENT_DYNAMIC(CImageDlg, CDialog)
CImageDlg::CImageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageDlg::IDD, pParent)
{
	m_pTexture		= NULL;
	m_bChanged		= FALSE;
	m_bBltMode		= BLT_NORMAL;
	m_posScroll.x	= 0;
	m_posScroll.y	= 0;

	m_RButtonDown = FALSE;
	m_DragStart.x = 0 ; 
	m_DragStart.y = 0 ; 

	m_Zoom = 1.0f;
	WndSize.cx = 0 ; 
	WndSize.cy = 0;
	m_nLevelCount = 0;
}

CImageDlg::~CImageDlg()
{
	I3_SAFE_RELEASE( m_pTexture );
	DeleteImage();
}

void CImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CImageDlg, CDialog)

ON_WM_PAINT()
ON_COMMAND(ID_FILE_APPLYIMG, OnFileApplyimg)
ON_COMMAND(ID_FILE_SAVEIMG, OnFileSaveimg)
ON_COMMAND(ID_FILE_LOADIMG, OnFileImport)
ON_COMMAND(ID_FILE_EXITIMG, OnFileExitimg)
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_WM_SIZE()
ON_WM_ERASEBKGND()
ON_WM_MOUSEMOVE()
ON_WM_MOUSEWHEEL()
ON_COMMAND(ID_FILE_IMPORTASEXTERNAL, OnFileImportAsExternal)
END_MESSAGE_MAP()


BOOL CImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//윈도우 스타일 재지정 - 리사이즈
	DWORD removeStyle = WS_POPUPWINDOW;
	//DWORD addStyle = WS_SIZEBOX | WS_CAPTION   ;
	DWORD addStyle = WS_POPUP |WS_CAPTION | WS_THICKFRAME | WS_SYSMENU;
	ModifyStyle( removeStyle, addStyle);

	if(m_pTexture == NULL  || (m_pTexture->GetWidth() == 0 && m_pTexture->GetHeight() == 0))	return TRUE;

	SetMipMapImage(m_pTexture);	//DibSection만들고 이미지를 한가지포맷으로 변환

	::SetScrollPos(this->m_hWnd , SB_VERT , 0 , TRUE);
	::SetScrollPos(this->m_hWnd , SB_HORZ , 0 , TRUE);

	// TODO:  Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CImageDlg::DeleteImage()
{
	for(int nLevel = 0 ; nLevel < m_nLevelCount ; ++nLevel)
	{
		if( m_MemDC[nLevel] != NULL)
		{
			if( m_hBitmap[nLevel] != NULL)
			{
				SelectObject( m_MemDC[nLevel], m_hOldBitmap[nLevel]);

				DeleteObject( m_hBitmap[nLevel]);

				m_hBitmap[nLevel] = NULL;
			}
			DeleteDC( m_MemDC[nLevel]);

			m_MemDC[nLevel] = NULL;
		}
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

void	CImageDlg::SetMipMapImage(i3Texture * pTexture)
{
	if(pTexture == NULL || (pTexture->GetWidth() == 0 && pTexture->GetHeight() == 0))	return ;
	
	DeleteImage();

	//
	INT32	Width	= m_pTexture->GetWidth();
	INT32	Height	= m_pTexture->GetHeight();

	m_nTotalSize.cx = 0;
	m_nTotalSize.cy = m_pTexture->GetHeight();

	//Calculate Total PaintDCSize
	m_nLevelCount = pTexture->GetLevelCount();
	for( int Count = 0 ; Count < m_nLevelCount ; ++Count )
	{
		m_nTotalSize.cx += Width;
		Width  = Width>>1;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//Create DC
	HDC hdc = ::GetDC(this->GetSafeHwnd());

	//For Paint
	m_hPaintBitmap = CreateCompatibleBitmap(hdc , m_nTotalSize.cx , m_nTotalSize.cy);
	m_PaintDC = CreateCompatibleDC(hdc);
	m_hOldPaintBitmap = (HBITMAP)	SelectObject(m_PaintDC , m_hPaintBitmap);

	SetGridRect(m_PaintDC ,0, 0,  m_nTotalSize.cx ,m_nTotalSize.cy);

	for(int i = 0 ; i < m_nLevelCount ; ++i)
	{
		SetImage( i , pTexture);
	}
}

void	CImageDlg::SetImage(INT32	nLevel , i3Texture * pTexture )
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Bitmap
	BITMAPINFOHEADER fi;
	PVOID pRaster;
	BITMAPINFO ih;

	INT32	WidthofImg  = m_pTexture->GetWidth()  >> nLevel;
	INT32	HeightofImg = m_pTexture->GetHeight() >> nLevel;

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


	HDC hdc = ::GetDC(this->GetSafeHwnd());
	
	//For Image
	m_MemDC[nLevel] = CreateCompatibleDC(hdc);
	m_hOldBitmap[nLevel] = (HBITMAP) ::SelectObject( m_MemDC[nLevel], m_hBitmap[nLevel]);

	//if have alphachannel
	BYTE * pSrc = (BYTE*)pDest;

	INT32 WidthPosInDC = 0;
	for(int count = 0 ; count < nLevel ; ++count)	WidthPosInDC += m_pTexture->GetWidth()>>count;

	COLORREF	DST;
	for(int i = 0 ; i < (signed)HeightofImg ; ++i)
	{
		for(int j = 0 ; j < (signed)WidthofImg ; ++j)
		{
			DST = GetPixel(m_PaintDC , j + WidthPosInDC , i );
			BYTE	R = GetRValue(DST);
			BYTE	G = GetGValue(DST);
			BYTE	B = GetBValue(DST);
			BYTE	A = pSrc[3];
			

			SetPixel(m_PaintDC , j + WidthPosInDC , i , RGB(	(pSrc[2] * A + (255 - A) * R)>>8 ,
												(pSrc[1] * A + (255 - A) * G)>>8 ,
												(pSrc[0] * A + (255 - A) * B )>>8));
			++pSrc;
			++pSrc;
			++pSrc;
			++pSrc;
		}
	}

	::ReleaseDC(m_hWnd,hdc);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	pTexture->Unlock();
}
/*
void	CImageDlg::SetImage(i3Texture * pTexture)
{
	if(pTexture == NULL)	return ;
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

	//if have alphachannel
	SetGridRect(m_PaintDC ,0, 0,  m_pTexture->GetWidth() ,m_pTexture->GetHeight());
	BYTE * pSrc = (BYTE*)pDest;

	COLORREF	DST;
	for(int i = 0 ; i < (signed)m_pTexture->GetHeight() ; ++i)
	{
		for(int j = 0 ; j < (signed)m_pTexture->GetWidth() ; ++j)
		{
			DST = GetPixel(m_PaintDC , j , i );
			BYTE	R = GetRValue(DST);
			BYTE	G = GetGValue(DST);
			BYTE	B = GetBValue(DST);
			BYTE	A = pSrc[3];
			

			SetPixel(m_PaintDC , j , i , RGB(	(pSrc[2] * A + (255 - A) * R)>>8 ,
												(pSrc[1] * A + (255 - A) * G)>>8 ,
												(pSrc[0] * A + (255 - A) * B )>>8));
			++pSrc;
			++pSrc;
			++pSrc;
			++pSrc;
		}
	}

	::ReleaseDC(m_hWnd,hdc);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	pTexture->Unlock();
}
*/
void CImageDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	if(!m_pTexture)	return ;

	{
		dc.SelectStockObject(NULL_PEN);
		dc.SelectStockObject(DKGRAY_BRUSH);
		CRect rt ; 
		if(m_Zoom * m_nTotalSize.cx< (REAL32)WndSize.cx)
		{
			rt.left = (int)( m_Zoom * m_nTotalSize.cx);
			rt.right = WndSize.cx+1;
			rt.top = 0 ; 
			rt.bottom = WndSize.cy+1;
		}
		dc.Rectangle(&rt);

		if(m_Zoom * m_nTotalSize.cy < (REAL32)WndSize.cy)
		{
			rt.left = 0 ; 
			rt.right = WndSize.cx+1;
			rt.top = (int)( m_Zoom * m_nTotalSize.cy);
			rt.bottom = WndSize.cy+1;
		}
		dc.Rectangle(&rt);
	}

	int CX = (int)(m_posScroll.x / m_Zoom);
	int CY = (int)(m_posScroll.y / m_Zoom);

	::StretchBlt(dc.m_hDC,0 , 0,
					WndSize.cx,
					WndSize.cy,
					m_PaintDC ,
					CX,
					CY,
					(int)(WndSize.cx	/ m_Zoom),
					(int)(WndSize.cy	/ m_Zoom), 
					 SRCCOPY);
					 //*/
}

//ok이면  =  변경되지않았으면 널값리턴,변경되었으면 변경된이미지 리턴
void CImageDlg::OnFileApplyimg()
{
	OnOK();
}

//취소이면 널값 리턴.변경된이미지일경우에 릴리즈
void CImageDlg::OnFileExitimg()
{
	OnCancel();
}

void CImageDlg::OnFileSaveimg()
{
	char lpstrFile[1024] = "";//얻어올 파일명

	OPENFILENAME	ofn;
	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner	= this->m_hWnd;
	ofn.lpstrFile   = lpstrFile;
	ofn.nMaxFile	= 1024;

	if(GetSaveFileName(&ofn) != 0)
	{
		if(!m_pImageFile.Save( lpstrFile , m_pTexture ))
		{
			MessageBox("파일 저장 실패");
		}
		else
		{
			MessageBox("성공");
		}
	}
}

BOOL CImageDlg::ImportImage( const char *szPath )
{
	i3Texture * pTex;

	pTex = m_pImageFile.Load( szPath );
	if( pTex == NULL)
	{
		MessageBox( "Failed to load file" );
		return FALSE;
	}
	I3ASSERT( pTex->GetRefCount() == 1 );

	I3_SAFE_RELEASE(m_pTexture);
	m_pTexture = pTex;
	m_bChanged = TRUE;			// 현재 이미지는 원본과는 다른이미지이다 라고 설정

	SetMipMapImage(m_pTexture);

	m_Zoom = 1.0f;

	return TRUE;
}


//현재이미지가 원본과 다른이미지이면 릴리즈하고 새로이미지 로드
//원본이면 포인터 연결만 바꿔준다.
void CImageDlg::OnFileImport()
{
	// TODO: Add your control notification handler code here
	{
		char lpstrFile[1024] = "";//얻어올 파일명

		OPENFILENAME	ofn;
		memset(&ofn,0,sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner	= this->m_hWnd;
		ofn.lpstrFilter = "TGA파일(*.tga)\0*.tga\0비트맵파일(*.bmp)\0*.bmp\0I3I파일(*.i3i)\0*.i3i";
		ofn.lpstrFile   = lpstrFile;
		ofn.nMaxFile	= 1024;

		if(GetOpenFileName(&ofn) != 0)
		{
			if( ImportImage( lpstrFile ) )
			{
				SetScroll();
			}
		}
	}
}

void CImageDlg::OnFileImportAsExternal()
{
	// TODO: Add your control notification handler code here
	{
		char lpstrFile[1024] = "";//얻어올 파일명

		OPENFILENAME	ofn;
		memset(&ofn,0,sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner	= this->m_hWnd;
		ofn.lpstrFilter = "이미지 파일(*.i3i,*.tga,*.bmp)\0*.tga;*.i3i;*.bmp\0모든 파일(*.*)\0*.*\0";
		ofn.lpstrFile   = lpstrFile;
		ofn.nMaxFile	= 1024;

		if(GetOpenFileName(&ofn) != 0)
		{
			if( ImportImage( lpstrFile ) )
			{
				I3ASSERT( m_pTexture != NULL );
				char szPath[MAX_PATH];
				g_pMainWnd->GetRelativePath( szPath, lpstrFile );
				m_pTexture->SetName( szPath );
				m_pTexture->SetExternRes();
				SetScroll();
			}
		}
	}
}

void CImageDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = TRUE;

	::SetCapture(this->m_hWnd);

	m_DragStart.x = point .x;
	m_DragStart.y = point .y;

	CDialog::OnRButtonDown(nFlags, point);
}

void CImageDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_RButtonDown = FALSE;
	::ReleaseCapture();
	CDialog::OnRButtonUp(nFlags, point);
}

void CImageDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CImageDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
void CImageDlg::Scroll(int nBar , int ScrollAmount , LONG& pos)
{
	int Min , Max;
	GetScrollRange(nBar , &Min , &Max);
	if(Max == 100)		return ;

	if	   (pos + ScrollAmount >= Max)	{	ScrollAmount = Max - pos; pos = Max;	}
	else if(pos + ScrollAmount <= Min)	{	ScrollAmount = pos - Min; pos = Min;	}
	else								{	pos += ScrollAmount;	}

	SetScrollPos(nBar , pos, TRUE);
	Invalidate();
}

void CImageDlg::SetGridRect(HDC  hdc ,int x ,int y ,  INT32 Width , INT32 Height)
{
	INT32	nDivide = (INT32)(Width / 20);
	if(nDivide < 5 ) nDivide = 5;
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

void CImageDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	WndSize.cx = cx ; 
	WndSize.cy = cy ;

	SetScroll();
	//*/
}

void	CImageDlg::ResetScroll(int nBar , LONG& pos)
{
	pos = 0;
	SetScrollRange(nBar , 0,0,TRUE);
	SetScrollPos(nBar , 0	,TRUE);
}

void	CImageDlg::SetScroll()
{
	int Min , Max , Factor;
	REAL32	magnification;

	if( m_pTexture == NULL)
		return;

	////////////
	//SetScrollRange
	GetScrollRange(SB_HORZ , &Min , &Max );
	Factor = ((int)(m_nTotalSize.cx * m_Zoom)  -  WndSize.cx);
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
	Factor = ((int)((REAL32)m_pTexture->GetHeight() * m_Zoom)  -  WndSize.cy);
	SetScrollRange(SB_VERT , 0 , Factor , TRUE);

	if(Max <= 0)	m_posScroll.y = 0;
	else
	{
		magnification = (REAL32)Factor / Max ; 
		m_posScroll.y = (int)((REAL32)m_posScroll.y * magnification ); 
	}

	SetScrollPos(SB_VERT , m_posScroll.y , TRUE);

	if(WndSize.cx > (int)(m_nTotalSize.cx * m_Zoom))	ResetScroll(SB_HORZ , m_posScroll.x);
	if(WndSize.cy > (int)(m_pTexture->GetHeight() * m_Zoom))ResetScroll(SB_VERT , m_posScroll.y);

	Invalidate();
}

BOOL CImageDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void CImageDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_RButtonDown)
	{
		int dX = point.x - m_DragStart.x;
		int dY = point.y - m_DragStart.y;

		Scroll(SB_HORZ ,  - dX , m_posScroll.x);
		Scroll(SB_VERT ,  - dY , m_posScroll.y);

		m_DragStart.x = point .x;
		m_DragStart.y = point .y;
	}

	CDialog::OnMouseMove(nFlags, point);
}

BOOL CImageDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if(zDelta >= 0)	m_Zoom += 0.25f;
	if(zDelta < 0)	m_Zoom -= 0.25f;

	if(m_Zoom <= 0.25)	m_Zoom = 0.25f;
	if(m_Zoom >= 3.0f)	m_Zoom = 3.0f;

	SetScroll();
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CImageDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.cx = 600;
	cs.cy = 600;
	return CDialog::PreCreateWindow(cs);
}

