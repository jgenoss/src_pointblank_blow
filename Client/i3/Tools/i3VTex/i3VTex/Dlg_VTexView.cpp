// Dlg_VTexView.cpp : implementation file
//

#include "stdafx.h"
#include "i3VTex.h"
#include "Dlg_VTexView.h"


// CDlg_VTexView dialog

IMPLEMENT_DYNAMIC(CDlg_VTexView, CDialog)

CDlg_VTexView::CDlg_VTexView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_VTexView::IDD, pParent)
{
	m_pTex = NULL;
	m_hDC = NULL;
	m_hWndDC = NULL;
	m_hBitmap = NULL;

	m_idxShow = 0;
	m_idxOld = -1;

	m_WndCX = m_WndCY = 0;
	m_BitmapCX = m_BitmapCY = 0;
}

CDlg_VTexView::~CDlg_VTexView()
{
	I3_SAFE_RELEASE( m_pTex);

	if( m_hBitmap != NULL)
	{
		::SelectObject( m_hDC, m_hOldBitmap);
		::DeleteObject( m_hBitmap);
		m_hBitmap = NULL;
	}

	if( m_hDC != NULL)
	{
		::DeleteDC( m_hDC);
		m_hDC = NULL;
	}
}

void CDlg_VTexView::setVTex( i3VirtualTexture * pTex)
{
	I3_REF_CHANGE( m_pTex, pTex);

	if( m_pTex != NULL)
	{
	}
}

void CDlg_VTexView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_VTexView, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CDlg_VTexView message handlers

BOOL CDlg_VTexView::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_hWndDC = ::GetDC( m_hWnd);

		m_hDC = ::CreateCompatibleDC( m_hWndDC);
	}

	{
		BITMAPINFO info;
		UINT32 sz;

		m_BitmapCX = 128;
		m_BitmapCY = 128;

		sz = i3Gfx::CalcRasterBufferSize( m_BitmapCX, m_BitmapCY, 4, I3G_IMAGE_FORMAT_BGRA_8888);

		i3mem::FillZero( &info, sizeof(info));

		info.bmiHeader.biSize			= sizeof( info.bmiHeader);
		info.bmiHeader.biBitCount		= 32;
		info.bmiHeader.biWidth			= m_BitmapCX;
		info.bmiHeader.biHeight			= m_BitmapCY;
		info.bmiHeader.biPlanes			= 1;
		info.bmiHeader.biCompression	= BI_RGB;
		info.bmiHeader.biSizeImage		= sz;
		info.bmiHeader.biXPelsPerMeter	= 0;
		info.bmiHeader.biYPelsPerMeter	= 0;
		info.bmiHeader.biClrUsed		= 0;
		info.bmiHeader.biClrImportant	= 0;

		m_hBitmap = ::CreateDIBSection( m_hDC, &info, DIB_RGB_COLORS, (void **) & m_pPixelData, NULL, 0);

		m_hOldBitmap = (HBITMAP) ::SelectObject( m_hDC, m_hBitmap);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_VTexView::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	m_WndCX = cx;
	m_WndCY = cy;
}

void CDlg_VTexView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if( (m_hDC != NULL) && (m_hWndDC != NULL))
	{
		#if 0
		{
			UINT32 sz = i3Gfx::CalcRasterBufferSize( m_BitmapCX, m_BitmapCY, 4, I3G_IMAGE_FORMAT_BGRA_8888);
			UINT32 c, i, * pData;
			UINT32 r, g, b;

			r = (rand() % 128) + 100;
			g = (rand() % 128) + 100;
			b = (rand() % 128) + 100;

			c = (r << 16) | (g << 8) | (b) | 0xFF000000;

			pData = (UINT32 *) m_pPixelData;

			for( i = 0; i < (sz >> 2); i++)
			{
				pData[i] = c;
			}
		}
		#endif

		if( (m_pTex != NULL) && (m_idxShow != m_idxOld))
		{
			m_idxOld = m_idxShow;

			m_pTex->LoadATile( m_idxShow, m_pPixelData);
		}

		BitBlt( m_hWndDC, 0, 0, m_BitmapCX, m_BitmapCY, m_hDC, 0, 0, SRCCOPY);
	}
}

void CDlg_VTexView::OnDestroy()
{
	if( m_hWndDC != NULL)
	{
		::ReleaseDC( m_hWnd, m_hWndDC);
		m_hWndDC = NULL;
	}

	CDialog::OnDestroy();
}

void CDlg_VTexView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch( nChar)
	{
		case VK_LEFT :
			m_idxShow --;
			if( m_idxShow < 0)
				m_idxShow = 0;

			Invalidate();
			break;

		case VK_RIGHT :
			m_idxShow ++;
			if( m_idxShow >= m_pTex->getNumberOfTiles())
				m_idxShow = m_pTex->getNumberOfTiles() - 1;

			Invalidate();
			break;
	}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CDlg_VTexView::PreTranslateMessage(MSG* pMsg)
{
	CDialog::PreTranslateMessage(pMsg);

	return 0;
}
