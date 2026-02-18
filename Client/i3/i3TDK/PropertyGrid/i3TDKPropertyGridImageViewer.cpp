#include "stdafx.h"
#include "i3TDKPropertyGridImageViewer.h"

IMPLEMENT_DYNAMIC( i3TDKPropertyGridImageViewer, i3TDKImageViewCtrl)

BEGIN_MESSAGE_MAP(i3TDKPropertyGridImageViewer, i3TDKImageViewCtrl)
	ON_WM_DROPFILES()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


int i3TDKPropertyGridImageViewer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( i3TDKImageViewCtrl::OnCreate( lpCreateStruct) == -1)
		return -1;

	m_bCreated = true;
	return 0;
}

void i3TDKPropertyGridImageViewer::OnDestroy()
{
	i3TDKImageViewCtrl::OnDestroy();

	// TODO: Add your message handler code here
	m_bCreated = false;
}

void i3TDKPropertyGridImageViewer::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	
	//아무동작도 안함.

	CWnd::OnDropFiles(hDropInfo);
}

UINT32 i3TDKPropertyGridImageViewer::GetImageWidth( void)
{
	I3ASSERT( m_pTexture != nullptr);

	return m_pTexture->GetWidth();
}

UINT32 i3TDKPropertyGridImageViewer::GetImageHeight( void)
{
	I3ASSERT( m_pTexture != nullptr);

	return m_pTexture->GetHeight();
}

const char* i3TDKPropertyGridImageViewer::GetImageFormatText( void)
{
	I3ASSERT( m_pTexture != nullptr);

	return m_pTexture->GetFormatText();
}

void i3TDKPropertyGridImageViewer::SetShowWindow( bool bShow)
{
	//m_ImageViewer.SetWindowPos(  nullptr, 0, 0, 500, 500, SWP_SHOWWINDOW);

	if( bShow)
		ShowWindow( SW_SHOW);
	else
		ShowWindow( SW_HIDE);
}

//void i3TDKPropertyGridImageViewer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	//// TODO: Add your message handler code here and/or call default
//	//int ScrollAmount = 0;
//	//// TODO: Add your message handler code here and/or call default
//	//switch(nSBCode)
//	//{
//	//	case SB_LINEUP:			ScrollAmount -= 1;		break;
//	//	case SB_LINEDOWN:		ScrollAmount += 1;		break;
//	//	case SB_PAGEUP:			ScrollAmount -= 10;		break;
//	//	case SB_PAGEDOWN:		ScrollAmount += 10;		break;
//	//	case SB_THUMBTRACK:		ScrollAmount  =	nPos - m_posScroll.y;	break;
//	//}
//
//	//SCROLLINFO si;
//	//pScrollBar->GetScrollInfo( &si);
//	//
//	////ScrollAmount만큼 스크롤
//	//Scroll(SB_VERT , ScrollAmount , m_posScroll.y);
//
//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
//}
