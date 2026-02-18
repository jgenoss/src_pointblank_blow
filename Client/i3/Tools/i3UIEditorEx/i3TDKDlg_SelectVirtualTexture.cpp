// i3TDKDlg_SelectVirtualTexture.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "i3TDKDlg_SelectVirtualTexture.h"

#include "i3Base/string/ext/extract_filename.h"

// i3TDKDlg_SelectVirtualTexture

IMPLEMENT_DYNAMIC(i3TDKDlg_SelectVirtualTexture, CWnd)

#define INTERSPACE		16
#define	TEXTBOX_CY		32
/*
static INT32	_ThumbComp( i3tool::pack::VTEX_THUMB * p1, i3tool::pack::VTEX_THUMB * p2)
{
	if( p1->m_Code[0] > p2->m_Code[0])
		return 1;
	else if( p1->m_Code[0] < p2->m_Code[0])
		return -1;

	if( p1->m_Code[1] > p2->m_Code[1])
		return 1;
	else if( p1->m_Code[1] < p2->m_Code[1])
		return -1;

	if( p1->m_Code[2] > p2->m_Code[2])
		return 1;
	else if( p1->m_Code[2] < p2->m_Code[2])
		return -1;

	if( p1->m_Code[3] > p2->m_Code[3])
		return 1;
	else if( p1->m_Code[3] < p2->m_Code[3])
		return -1;

	return 0;
}

static INT32	_ThumbFind( i3tool::pack::VTEX_THUMB * p1, INT32 * pCode)
{
	if( p1->m_Code[0] > pCode[0])
		return 1;
	else if( p1->m_Code[0] < pCode[0])
		return -1;

	if( p1->m_Code[1] > pCode[1])
		return 1;
	else if( p1->m_Code[1] < pCode[1])
		return -1;

	if( p1->m_Code[2] > pCode[2])
		return 1;
	else if( p1->m_Code[2] < pCode[2])
		return -1;

	if( p1->m_Code[3] > pCode[3])
		return 1;
	else if( p1->m_Code[3] < pCode[3])
		return -1;

	return 0;
}
*/

i3TDKDlg_SelectVirtualTexture::i3TDKDlg_SelectVirtualTexture()
{
	m_pVTex = NULL;

	m_ShowY = 0;
	m_ViewUnit = 256;


//	m_pThumbFile = NULL;

	m_idxSelected = -1;

	m_hDC = NULL;
	m_hCopyDC = NULL;
	m_hFont = NULL;
	m_hBitmap = NULL;
	m_hbrBack = NULL;
	m_hbrSelect = NULL;

	m_cntVisible = 0;
}

i3TDKDlg_SelectVirtualTexture::~i3TDKDlg_SelectVirtualTexture()
{
//	I3_SAFE_RELEASE( m_pThumbFile);

	RemoveAllThumbs();
}

bool i3TDKDlg_SelectVirtualTexture::Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID)
{
	LPCTSTR pszClass;
	HCURSOR hCursor;
	HBRUSH hbrBack;

	hCursor = ::LoadCursor( NULL, IDC_ARROW);

	hbrBack = (HBRUSH) ::GetStockObject( NULL_BRUSH);

	pszClass = AfxRegisterWndClass( CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC, hCursor, hbrBack, NULL);

	return CWnd::Create( pszClass, "", dwStyle, rect, pParentWnd, nID) == TRUE;
}
/*
bool i3TDKDlg_SelectVirtualTexture::LoadThumbInfo(void)
{
	i3FileStream file;
	char szPath[ MAX_PATH];

	RemoveAllThumbs();

	//
	{
		
		m_pThumbFile = i3FileStream::new_object();

		if(i3UI::getManager()->GetUIResourceFolder()->size() == 0)
		{
			i3::snprintf( szPath, sizeof(szPath), "%s/ThumbData.thumb", i3ResourceFile::GetWorkingFolder());
		}
		else
		{
			i3::snprintf( szPath, sizeof(szPath), "%s/%s/%s/ThumbData.thumb", i3ResourceFile::GetWorkingFolder(), 
				i3UI::getManager()->GetUIResourceFolder()->c_str(), i3UI::getManager()->GetUIVersionName()->c_str());
		}
		

		if( i3System::IsFile( szPath))
		{
			if( m_pThumbFile->Open( szPath, STREAM_READ | STREAM_WRITE) == FALSE)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not open thumb image file.");
				I3_SAFE_RELEASE( m_pThumbFile);
			}
		}
		else
		{
			if( m_pThumbFile->Create( szPath, STREAM_READ | STREAM_WRITE) == FALSE)
			{
				I3PRINTLOG(I3LOG_WARN, "Could not create thumb image file.");
				I3_SAFE_RELEASE( m_pThumbFile);
			}
		}
		
	}

	// Thumb Info filee

	if(i3UI::getManager()->GetUIResourceFolder()->size() == 0)
	{
		i3::snprintf( szPath, sizeof(szPath), "%s/TexInfo.thumb", i3ResourceFile::GetWorkingFolder());
	}
	else
	{
		i3::snprintf( szPath, sizeof(szPath), "%s/%s/%s/TexInfo.thumb", i3ResourceFile::GetWorkingFolder(),
			i3UI::getManager()->GetUIResourceFolder()->c_str(), i3UI::getManager()->GetUIVersionName()->c_str());
	}

	
	if (i3System::IsFile(szPath))
	{
		if (file.Open(szPath, STREAM_READ) == FALSE)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	INT32 i, cnt;

	cnt = file.GetSize() / sizeof( i3tool::pack::VTEX_THUMB);

	for( i = 0 ; i < cnt; i++)
	{
		i3tool::pack::VTEX_THUMB * pNew;

		pNew = new i3tool::pack::VTEX_THUMB;

		if( file.Read( pNew, sizeof( i3tool::pack::VTEX_THUMB)) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not read thumb information file.");
			return false;
		}

		pNew->m_hBitmap = NULL;

		m_TexList.insert( pNew);
	}

	file.Close();

	return true;
}
*/

/*
bool i3TDKDlg_SelectVirtualTexture::SaveThumbInfo(void)
{
	i3FileStream file;
	char szPath[ MAX_PATH];

	if(i3UI::getManager()->GetUIResourceFolder()->size() == 0)
	{
		i3::snprintf( szPath, sizeof(szPath), "%s/TexInfo.thumb", i3ResourceFile::GetWorkingFolder());
	}
	else
	{
		i3::snprintf( szPath, sizeof(szPath), "%s/%s/%s/TexInfo.thumb", i3ResourceFile::GetWorkingFolder(),
			i3UI::getManager()->GetUIResourceFolder()->c_str(), i3UI::getManager()->GetUIVersionName()->c_str());
	}

	
	if( file.Create( szPath, STREAM_WRITE) == FALSE)
	{
		return false;
	}

	

	for(size_t i = 0 ; i < m_TexList.size(); i++)
	{
		i3tool::pack::VTEX_THUMB * pInfo = m_TexList.get_vector()[i];

// 		if( strcmp(pInfo->m_szName, "res/selectimage.i3i") == 0)
// 			continue;

		if( file.Write( pInfo, sizeof( i3tool::pack::VTEX_THUMB)) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "Could not write thumb information file.");
			return false;
		}
	}

	file.Close();
	
	return true;
}
*/

INT32 i3TDKDlg_SelectVirtualTexture::FindThumb( const char * pszName)
{
	UINT32 code[4];

	i3MD5::Gen( (char *) pszName, i3::generic_string_size( pszName), (char *) code);

//	m_TexList.SetCompareProc( (COMPAREPROC) _ThumbFind);
//	INT32 idx = m_TexList.FindItem( (void *) code);
//	m_TexList.SetCompareProc( (COMPAREPROC) _ThumbComp);

	i3::vector_set< i3tool::pack::VTEX_THUMB* >::iterator it = m_TexList.find_other((INT32*)code);
	return ( it != m_TexList.end() ) ? i3::distance(m_TexList.begin(), it) : -1;
}

INT32	i3TDKDlg_SelectVirtualTexture::PrepareThumb( I3VT_TEXINFO * pTexInfo)
{
	INT32 idx;
	i3tool::pack::VTEX_THUMB * pInfo;

	idx = FindThumb( pTexInfo->m_strName.c_str());

	if( idx != -1)
	{
		pInfo = m_TexList.get_vector()[idx];

		// Thumb Image를 읽어 들임.
		I3ASSERT(pInfo->m_hBitmap != NULL);

//		if( pInfo->m_hBitmap == NULL)
//		{
//			_LoadThumbImage( pInfo);
//		}
	}
	else
	{
//		if( m_pThumbFile == NULL)
//			return NULL;

		pInfo = new i3tool::pack::VTEX_THUMB;
		
		i3MD5::Gen( (char *) pTexInfo->m_strName.c_str(), pTexInfo->m_strName.size(), (char *) pInfo->m_Code);
		i3::string_ncopy_nullpad( pInfo->m_szName, pTexInfo->m_strName, sizeof(pInfo->m_szName));		// 이건 안전해보인다..(2013.05.02.수빈)

		i3::pair< i3::vector_set< i3tool::pack::VTEX_THUMB*>::iterator, bool> pr =	m_TexList.insert( pInfo);
		idx = i3::distance( m_TexList.begin(), pr.first);
		_GenThumbImage( pInfo, pTexInfo);
	}

	return idx;
}

/*
void i3TDKDlg_SelectVirtualTexture::_LoadThumbImage( i3tool::pack::VTEX_THUMB * pInfo)
{
	I3ASSERT( pInfo->m_hBitmap == NULL);

	char * pBuf;

	if( m_pThumbFile == NULL)
		return;

	pBuf = (char *) i3MemAlloc( pInfo->m_Size);

	m_pThumbFile->SetPosition( (INT32) pInfo->m_Offset);

	if( m_pThumbFile->Read( pBuf, pInfo->m_Size) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "Could not read thumb pixel data : %s", pInfo->m_szName);
		i3MemFree( pBuf);
		return;
	}

	pInfo->m_hBitmap = ::CreateBitmap( pInfo->m_Width, pInfo->m_Height, GetDeviceCaps( m_hDC, PLANES), GetDeviceCaps( m_hDC, BITSPIXEL), pBuf);
	I3ASSERT( pInfo->m_hBitmap != NULL);

	i3MemFree( pBuf);
}
*/

void i3TDKDlg_SelectVirtualTexture::_GenThumbImage( i3tool::pack::VTEX_THUMB * pInfo, I3VT_TEXINFO * pTexInfo)
{
	// 크기 결정
	{
		REAL32 scaleFactor = 1.0f;
		INT32 refSz;

		refSz = MAX( pTexInfo->m_Width, pTexInfo->m_Height);

		if( refSz > m_ViewUnit)
			scaleFactor = (REAL32) m_ViewUnit / refSz;

		pInfo->m_Width = (INT32)( pTexInfo->m_Width * scaleFactor);
		pInfo->m_Height = (INT32) (pTexInfo->m_Height * scaleFactor);
	}
	
	{
		i3Texture * pTex = i3Texture::new_object();

		pTex->Create( pInfo->m_Width, pInfo->m_Height, 1, I3G_IMAGE_FORMAT_BGRX_8888, I3G_USAGE_FREQUENT_UPDATE);

		m_pVTex->ExtractTexture( pTex, pTexInfo->m_X, pTexInfo->m_Y, pTexInfo->m_Width, pTexInfo->m_Height);

		pTex->Lock( 0);

		// 이미지 저장
//		pInfo->m_Size	= i3Gfx::CalcRasterBufferSize( pInfo->m_Width, pInfo->m_Height, 1, pTex->GetFormat());
//		pInfo->m_Offset = m_pThumbFile->SetPosition( 0, STREAM_END);

//		m_pThumbFile->Write( pTex->GetLockedBuffer(0), pInfo->m_Size);

		pInfo->m_hBitmap = ::CreateBitmap( pInfo->m_Width, pInfo->m_Height, GetDeviceCaps( m_hDC, PLANES), GetDeviceCaps( m_hDC, BITSPIXEL), pTex->GetLockedBuffer(0));
		I3ASSERT( pInfo->m_hBitmap != NULL);

		pTex->Unlock(0);

		pTex->Release();
	}

//	SaveThumbInfo();
}

void i3TDKDlg_SelectVirtualTexture::RemoveAllThumbs(void)
{
	
	i3tool::pack::VTEX_THUMB * pInfo;

	for(size_t i = 0; i < m_TexList.size(); i++)
	{
		pInfo = m_TexList.get_vector()[i];

		if (pInfo->m_hBitmap)
			::DeleteObject(pInfo->m_hBitmap);

		delete pInfo;
	}
	m_TexList.clear();
}

BEGIN_MESSAGE_MAP(i3TDKDlg_SelectVirtualTexture, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// i3TDKDlg_SelectVirtualTexture message handlers



int i3TDKDlg_SelectVirtualTexture::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		HDC dc;

		dc = ::GetDC(m_hWnd);

		m_hDC = CreateCompatibleDC( dc);
		m_hCopyDC = CreateCompatibleDC( dc);

		::ReleaseDC( m_hWnd, dc);
	}

	{
		HBITMAP hBitmap = ::LoadBitmap( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDB_PANEL_BACKGROUND));

		m_hbrBack	= i3TDK::CreateBitmapBrush( hBitmap);
	}

	{
		m_hbrSelect = ::CreateSolidBrush( RGB( 255, 201, 14));
	}

	{
		m_hFont = i3TDK::CreateDefFont( -11, "Tahoma", 400, CLEARTYPE_QUALITY);

		m_hOldFont = (HFONT) ::SelectObject( m_hDC, m_hFont);
	}

	SetupScroll();

	{
	//	LoadThumbInfo();
		RemoveAllThumbs();
	}

	return 0;
}

void i3TDKDlg_SelectVirtualTexture::OnDestroy()
{
	if( m_hFont != NULL)
	{
		::SelectObject( m_hDC, m_hOldFont);
		::DeleteObject( m_hFont);
		m_hFont = NULL;
	}

	if( m_hBitmap != NULL)
	{
		::SelectObject( m_hDC, m_hOldBitmap);
		::DeleteObject( m_hBitmap);
	}

	if( m_hbrBack != NULL)
	{
		::DeleteObject( m_hbrBack);
		m_hbrBack = NULL;

		::DeleteObject( m_hbrSelect);
		m_hbrSelect = NULL;
	}

	if( m_hCopyDC != NULL)
	{
		DeleteDC( m_hCopyDC);
		m_hCopyDC = NULL;
	}

	if( m_hDC != NULL)
	{
		DeleteDC( m_hDC);
		m_hDC = NULL;
	}


	CWnd::OnDestroy();

//	{
//		SaveThumbInfo();
//	}
}

void i3TDKDlg_SelectVirtualTexture::GetImageRect( INT32 idx, CRect * pRect)
{
	INT32 ImgPerLine, blockW, blockH;

	blockW = m_ViewUnit + INTERSPACE;
	blockH = m_ViewUnit + INTERSPACE + TEXTBOX_CY;

	ImgPerLine = MAX( 1, m_WndSz.cx / (m_ViewUnit + INTERSPACE));

	pRect->left = (idx % ImgPerLine) * blockW + (INTERSPACE >> 1);
	pRect->right = pRect->left + blockW - (INTERSPACE >> 1);

	pRect->top = ((idx / ImgPerLine) * blockH) - m_ShowY + (INTERSPACE >> 1);
	pRect->bottom = pRect->top + blockH - (INTERSPACE >> 1);
}

void i3TDKDlg_SelectVirtualTexture::PrepareVisibleTexture(void)
{

}

void i3TDKDlg_SelectVirtualTexture::SetVirtualTexture( i3VirtualTexture * pVTex)
{
	m_pVTex = pVTex;

	Invalidate();
}

// 이것도 다 할 필요가 없어요..(리팩토링 해야지)

void i3TDKDlg_SelectVirtualTexture::ModifyThumb( CProgressCtrl* pProgress)
{
	RemoveAllThumbs();		// 그냥 지우기만 해..

	/*
	INT32 i;
	INT32 idx;
	i3tool::pack::VTEX_THUMB * pInfo;
	pProgress->SetRange(0, short int(getRefTexCount()) );

	for( i = 0; i < getRefTexCount(); i++)
	{
		pProgress->SetPos(i);
		I3VT_TEXINFO * pTexInfo = getRefTex( i);

		idx = FindThumb( pTexInfo->m_strName.c_str());

		if( idx != -1)
		{
			// 다시 작성
			pInfo = m_TexList.get_vector()[idx];
		}
		else
		{	// 추가된 경우??
		//	if( m_pThumbFile == NULL)
		//		return;

			pInfo = new i3tool::pack::VTEX_THUMB;

			i3mem::FillZero( pInfo, sizeof( i3tool::pack::VTEX_THUMB));

			i3MD5::Gen( (char *) pTexInfo->m_strName.c_str(), pTexInfo->m_strName.size(), (char *) pInfo->m_Code);
			i3::string_ncopy_nullpad( pInfo->m_szName, pTexInfo->m_strName, sizeof(pInfo->m_szName));
			

			i3::pair<i3::vector_set<i3tool::pack::VTEX_THUMB*>::iterator, bool> pr = m_TexList.insert( pInfo);
			idx = i3::distance(m_TexList.begin(), pr.first);
		}

		_GenThumbImage( pInfo, pTexInfo);
	}
	*/


}

void i3TDKDlg_SelectVirtualTexture::SetupScroll(void)
{
	SCROLLINFO info;

	m_ExtentY = 0;

	if( m_pVTex != NULL)
	{
		CRect rtMin, rtMax;

		if( getRefTexCount() > 0)
		{
			GetImageRect( 0, &rtMin);
			GetImageRect( getRefTexCount() - 1, &rtMax);

			m_ExtentY = (rtMax.bottom - rtMin.top) + 4;
		}
	}

	info.cbSize = sizeof(info);
	info.fMask	= SIF_PAGE | SIF_RANGE;
	info.nMin	= 0;
	info.nMax	= m_ExtentY;
	info.nPage	= m_WndSz.cy;

	SetScrollInfo( SB_VERT, &info);
}

void i3TDKDlg_SelectVirtualTexture::UpdateScroll(void)
{
	SCROLLINFO info;

	info.cbSize = sizeof(info);
	info.fMask	= SIF_POS | SIF_TRACKPOS;
	info.nPos = info.nTrackPos = m_ShowY;

	SetScrollInfo( SB_VERT, &info);
}

void i3TDKDlg_SelectVirtualTexture::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	if( bShow)
	{
	}
}

void i3TDKDlg_SelectVirtualTexture::DrawImage( HDC dc, CRect * pRect, I3VT_TEXINFO * pTexInfo, i3tool::pack::VTEX_THUMB * pThumb, bool bSelected)
{
	HBRUSH hOldBrush;

	if( bSelected)
		hOldBrush = (HBRUSH) ::SelectObject( dc, m_hbrSelect);
	else
		hOldBrush = (HBRUSH) ::SelectObject( dc, GetStockObject( WHITE_BRUSH));
		
	// Border
	::Rectangle( dc, pRect->left, pRect->top, pRect->right, pRect->bottom);

	// Thumb
	{
		HBITMAP hOldBitmap;
		INT32 x, y, cx, cy;

		if( pThumb != NULL)
		{
			hOldBitmap = (HBITMAP) ::SelectObject( m_hCopyDC, pThumb->m_hBitmap);

			x = pRect->left + 3 + ((m_ViewUnit >> 1) - (pThumb->m_Width >> 1));
			y = pRect->top  + 3 + ((m_ViewUnit >> 1) - (pThumb->m_Height >> 1));

			cx = pThumb->m_Width;
			cy = pThumb->m_Height;

			::StretchBlt( m_hDC, x, y, cx, cy, 
					m_hCopyDC, 0, 0, pThumb->m_Width, pThumb->m_Height, SRCCOPY);
		}
	}

	// Text
	{
		CRect rcText;
		char szText[ 300], szName[MAX_PATH];

		rcText = *pRect;
		rcText.top = rcText.bottom - TEXTBOX_CY;

//		i3String::SplitFileName( pTexInfo->m_szName, szName, TRUE);
		i3::extract_filename( pTexInfo->m_strName, szName);

		i3::snprintf( szText, sizeof(szText), "%s\r\n%dx%d", szName, pTexInfo->m_Width, pTexInfo->m_Height);

		::SetBkMode( dc, TRANSPARENT);
		::DrawText( dc, szText, -1, rcText, DT_VCENTER | DT_CENTER);
	}

	::SelectObject( dc, hOldBrush);
}

static bool isUsed( I3TDK_VTEX_VISIBLE_TEX * pTable, INT32 cnt, INT32 idxThumb)
{
	INT32 i;

	for( i = 0; i < cnt; i++)
	{
		if( pTable[i].m_idxThumb == idxThumb)
			return true;
	}

	return false;
}

void i3TDKDlg_SelectVirtualTexture::Draw( HDC dc)
{
	INT32 i, x, y;
	CRect rt, rtWnd;
	i3tool::pack::VTEX_THUMB * pThumb;

	rtWnd.SetRect( 0, 0, m_WndSz.cx, m_WndSz.cy);

	// Background
	{
		FillRect( dc, &rtWnd, m_hbrBack);
	}

	m_cntVisible = 0;

	if( m_pVTex == NULL)
		return;

	y = -m_ShowY;
	x = 0;

	for( i = 0; i < getRefTexCount(); i++)
	{
		I3VT_TEXINFO * pInfo = getRefTex( i);

		GetImageRect( i, &rt);

		CRect rcInter;

		if( rcInter.IntersectRect( &rt, &rtWnd) == FALSE)
			continue;

		// 화면에 보여야 하는 놈이다.
		INT32 idxThumb = PrepareThumb( pInfo);
		I3ASSERT( idxThumb != -1);

		pThumb = getThumb( idxThumb);

		DrawImage( dc, &rt, pInfo, pThumb, (i == m_idxSelected));

		m_VisibleTex[ m_cntVisible].m_idxTexInfo = i;
		m_VisibleTex[ m_cntVisible].m_idxThumb = idxThumb;
		m_cntVisible ++;
	}

	// 이전에 사용되었던 Bitmap을 제거한다.
	/*
	for( i = 0; i < oldVisibleCount; i++)
	{
		INT32 idxThumb = oldData[ i].m_idxThumb;

		if( isUsed( m_VisibleTex, m_cntVisible, idxThumb) == false)
		{
			i3tool::pack::VTEX_THUMB * pThumb2 = (i3tool::pack::VTEX_THUMB *) getThumb( idxThumb);

			if( pThumb2->m_hBitmap != NULL)
			{
				::DeleteObject( pThumb2->m_hBitmap);
				pThumb2->m_hBitmap = NULL;
			}
		}
	}
	*/
}

void i3TDKDlg_SelectVirtualTexture::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	Draw( m_hDC);

	::BitBlt( dc.m_hDC, 0, 0, m_WndSz.cx, m_WndSz.cy, m_hDC, 0, 0, SRCCOPY);
}

void i3TDKDlg_SelectVirtualTexture::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch( nSBCode)
	{
		case SB_TOP :			m_ShowY = 0;	break;
		case SB_BOTTOM :		m_ShowY = m_ExtentY - m_WndSz.cy;	break;
		case SB_LINEDOWN :		m_ShowY ++;	break;
		case SB_LINEUP :		m_ShowY --; break;
		case SB_PAGEDOWN :		m_ShowY += m_WndSz.cy;	break;
		case SB_PAGEUP :		m_ShowY -= m_WndSz.cy;	break;
		case SB_THUMBPOSITION :
		case SB_THUMBTRACK :	m_ShowY = nPos;	break;
	}

	if( m_ShowY > (m_ExtentY - m_WndSz.cy))
		m_ShowY = (m_ExtentY - m_WndSz.cy);

	if( m_ShowY < 0)
		m_ShowY = 0;

	UpdateScroll();

	Invalidate();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

INT32 i3TDKDlg_SelectVirtualTexture::FindTexByPos( POINT pt)
{
	INT32 i;
	CRect rt;

	for( i = 0; i < m_cntVisible; i++)
	{
		INT32 idxTex = m_VisibleTex[i].m_idxTexInfo;

		GetImageRect( idxTex, &rt);

		if( rt.PtInRect( pt))
			return idxTex;
	}

	return -1;
}

void i3TDKDlg_SelectVirtualTexture::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_idxSelected = FindTexByPos( point);

	Invalidate();

	CWnd::OnLButtonDown(nFlags, point);
}

void i3TDKDlg_SelectVirtualTexture::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if( m_idxSelected != -1)
	{
		::PostMessage( GetParent()->m_hWnd, WM_COMMAND, MAKELONG( IDOK, BN_CLICKED), (LPARAM) m_hWnd);
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void i3TDKDlg_SelectVirtualTexture::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
}

void i3TDKDlg_SelectVirtualTexture::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
}

void i3TDKDlg_SelectVirtualTexture::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
}

void i3TDKDlg_SelectVirtualTexture::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_WndSz.cx = cx;
	m_WndSz.cy = cy;

	SetupScroll();

	if( m_hDC == NULL)
		return;

	if( m_hBitmap != NULL)
	{
		::SelectObject( m_hDC, m_hOldBitmap);
		::DeleteObject( m_hBitmap);
	}

	{
		m_hBitmap = ::CreateBitmap( cx, cy, GetDeviceCaps( m_hDC, PLANES), GetDeviceCaps( m_hDC, BITSPIXEL), NULL);

		m_hOldBitmap = (HBITMAP) ::SelectObject( m_hDC, m_hBitmap);
	}
}


BOOL i3TDKDlg_SelectVirtualTexture::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{	
	if( zDelta > 0)		//while a value greater than zero indicates rotating forward (away from the user)
		m_ShowY -= 100;
	else
		m_ShowY += 100;

	m_ShowY = MINMAX( 0, m_ShowY, (m_ExtentY - m_WndSz.cy));
	
	UpdateScroll();

	Invalidate();

	return TRUE;
}

void i3TDKDlg_SelectVirtualTexture::SetRefTextureList( const i3::vector_multiset<I3VT_TEXINFO*, I3VT_TEXINFO_Comp>& List)
{
//	m_RefList.clear();
//	m_RefList.AddFromList( pList);

	m_RefList = List.get_vector();
	m_ShowY = 0;
	m_idxSelected = -1;

	SetupScroll();
	
	Invalidate();
}

I3VT_TEXINFO *	 i3TDKDlg_SelectVirtualTexture::getSelectedRefTexture(void)
{
	if( m_idxSelected == -1)
		return NULL;

	return m_RefList[m_idxSelected];
}
