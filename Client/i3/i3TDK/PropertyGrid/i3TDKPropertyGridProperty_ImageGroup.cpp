#include "stdafx.h"
#include "i3TDKPropertyGridProperty_ImageGroup.h"
#include "i3TDKpropertyGridProperty_Image.h"
#include "i3TDKPropertyGridProperty_Empty.h"
#include "i3TDKPropertyGridCtrl.h"

#if defined( I3_WINDOWS)

IMPLEMENT_DYNAMIC( i3TDKPropertyGridProperty_ImageGroup, i3TDKPropertyGridProperty_Group)

// Constructor
i3TDKPropertyGridProperty_ImageGroup::i3TDKPropertyGridProperty_ImageGroup( PROPERTYINFO* pInfo, i3TDKPropertyGridCtrl* pCtrl, INT32 nRowCount)
:	i3TDKPropertyGridProperty_Group( pInfo), 
	m_RowCount( nRowCount), m_pCtrl( pCtrl)
{
	// Set Parent Window 
	I3ASSERT( m_pCtrl != nullptr);

	// Add 1 Image Property
	m_pImageProp = new i3TDKPropertyGridProperty_Image( pInfo);
	AddSubItem( m_pImageProp);

	// Add n Empty Properties
	I3ASSERT( m_RowCount >= 5);		// Minimum
	for( INT32 i=0; i< m_RowCount; i++)
	{
		i3TDKPropertyGridProperty_Empty* pEmptyProp = new i3TDKPropertyGridProperty_Empty();
		AddSubItem( pEmptyProp);
	}

	// 반드시 호출해주어야 함.
	m_pCtrl->AddDrawProp(this);	// 이 객체를 추가하여 GridCtrl의 OnPaint에서 이 객체의 DrawImage가 호출될 수 있도록 한다.

	SetInfoText();
	_CalcRect();

	Expand( FALSE);
}

// Destructor
i3TDKPropertyGridProperty_ImageGroup::~i3TDKPropertyGridProperty_ImageGroup()
{
	m_pCtrl->RemoveDrawProp( this);
}

void i3TDKPropertyGridProperty_ImageGroup::Init( void)
{
	m_nImgh = 0;
	m_nImgw = 0;

	m_WhiteRect.SetRect( 0, 0, 0, 0);
	m_Rect.SetRect( 0, 0, 0, 0);
	m_DestRect.SetRect( 0, 0, 0, 0);
	m_SrcRect.SetRect( 0, 0, 0, 0);

	m_pCtrl->AddDrawProp( this);	// 이 객체를 추가하여 GridCtrl의 OnPaint에서 이 객체의 DrawImage가 호출될 수 있도록 한다.
}


void i3TDKPropertyGridProperty_ImageGroup::SetInfoText( void)
{
	// Print image information
	TCHAR conv[MAX_PATH];

	i3::snprintf( conv, _countof( conv), _T("Size: %d x %d"), m_pImageProp->GetImageWidth(), m_pImageProp->GetImageHeight());
	i3TDKPropertyGridProperty* pProp = (i3TDKPropertyGridProperty*)GetSubItem( 1);
	pProp->SetName( conv);

	i3::snprintf( conv, _countof( conv), _T("Format: %s"), m_pImageProp->GetImageViewer()->GetImageFormatText()); 
	pProp = (i3TDKPropertyGridProperty*)GetSubItem( 2);
	pProp->SetName( conv);
}


void i3TDKPropertyGridProperty_ImageGroup::_CalcRect( void)
{
	I3ASSERT( m_pImageProp != nullptr);
	
	//m_nImgw : m_nImgh = w : h
	m_nImgw = m_pImageProp->GetImageWidth();
	m_nImgh = m_pImageProp->GetImageHeight();
	
	INT32 h = m_RowCount * m_pCtrl->GetRowHeight();
	INT32 w = h * m_nImgw / m_nImgh;	

	m_fScale = (REAL32)h / (REAL32)m_nImgh;
	m_Rect.SetRect( 0, 0, w, h);
}


// Resize m_DestRect
void i3TDKPropertyGridProperty_ImageGroup::OnPosSizeChanged( CRect Rect)
{
	//I3TRACE( "OnPosSizeChanged\n");
	
	// Get Rects 
	INT32 leftwidth = m_pCtrl->GetLeftColumnWidth();
	INT32 rowheight = m_pCtrl->GetRowHeight();
//	INT32 descheight = m_pCtrl->GetDescriptionHeight();
	INT32 headerheight = m_pCtrl->GetHeaderHeight();
	CRect listrt = m_pCtrl->GetListRect();
	
	i3TDKPropertyGridProperty* pProp = (i3TDKPropertyGridProperty*)GetSubItem( 1);		// Get first empty property
	CRect rect = pProp->GetRect();

	// imaginary rect
	INT32 l = listrt.left + leftwidth;
	INT32 t = rect.top;
	INT32 r = rect.right;
	INT32 b = rect.top + rowheight * m_RowCount;
	CRect rt( l, t, r, b);	

	// white rect
	INT32 wl = listrt.left + leftwidth;	
	INT32 wt = max( t, headerheight);	
	INT32 wr = rect.right;
	INT32 wb = min( b, (INT32)listrt.bottom);
	m_WhiteRect.SetRect( wl, wt, wr, wb);

	// dest rect
	m_DestRect.CopyRect( &m_WhiteRect);
	m_DestRect.right = min( wl + m_Rect.Width(), wr);

	INT32 srctop = 0;
	INT32 srcbottom = m_nImgh;
	INT32 srcw = m_nImgw;
	
	if( m_DestRect.bottom < rt.bottom)		// cut bottom
	{
		// destH : desth = srcH : srch
		INT32 destH = m_Rect.Height();
		INT32 desth = rt.bottom - m_DestRect.bottom;
		INT32 srcH = m_nImgh;
		INT32 srch = srcH * desth / destH;

		srcbottom = srcH - srch;
	}
	
	if( m_DestRect.top > rt.top)		// cut top
	{
		// destH : desth = srcH : srch
		INT32 destH = m_Rect.Height();
		INT32 desth = m_DestRect.top - rt.top;
		INT32 srcH = m_nImgh;
		INT32 srch = srcH * desth / destH;

		srctop = srch;
	}

	if( m_DestRect.Width() < m_Rect.Width() )	// cut right
	{
		// m_Rect.Width() : m_DestRect.Width() = origW : srcw
		srcw = m_nImgw * m_DestRect.Width() / m_Rect.Width();
	}

	// src rect
	m_SrcRect.SetRect( 0, srctop, srcw, srcbottom);
	
	i3TDKPropertyGridProperty::OnPosSizeChanged( Rect);
}

// Draw an image using StretchBlt
void i3TDKPropertyGridProperty_ImageGroup::DrawImage( void)
{
	//I3TRACE( "DrawImage\n");

	if( !IsExpanded())
	{
		//I3TRACE("Not Expanded\n");
		return;
	}

	CMFCPropertyGridProperty* pParent = GetParent();
	while( pParent != nullptr)
	{
		if( !pParent->IsExpanded())
		{
			//I3TRACE("Parent Not Expanded\n");
			return;
		}

		pParent = pParent->GetParent();
	}

	//Set Rects
	INT32 ndl = m_DestRect.left;
	INT32 ndt = m_DestRect.top;
	INT32 ndw = m_DestRect.Width();
	INT32 ndh = m_DestRect.Height();

	INT32 nsl = m_SrcRect.left;
	INT32 nst = m_SrcRect.top;
	INT32 nsw = m_SrcRect.Width();
	INT32 nsh = m_SrcRect.Height();

	INT32 nww = m_WhiteRect.Width();
	INT32 nwh = m_WhiteRect.Height();

	if( ndw <= 0 || ndh <= 0)
	{
		//I3TRACE("Invisible\n");
		return;
	}

	// StretchBlt
	if( !m_pImageProp->IsViewerCreated())
		m_pImageProp->CreateViewer();
	
	CClientDC destDc( m_pCtrl);
	HDC srcDC = m_pImageProp->GetViewerDC();
	
			
	// http://www.experts-exchange.com/Programming/Languages/.NET/Visual_CPP/Q_21292287.html
	int old = destDc.SetStretchBltMode( HALFTONE);	

	// Fill White
	i3TDKPropertyGridProperty* pProp = (i3TDKPropertyGridProperty*)GetSubItem( 1);		// Get first empty property
	CRect rect = pProp->GetRect();
	//INT32 r = rect.right;
	INT32 b = rect.top + m_pCtrl->GetRowHeight() * m_RowCount;
	INT32 lb = m_pCtrl->GetListRect().bottom;
	b = min( b, lb);

	StretchBlt( destDc.m_hDC, ndl, ndt, nww, nwh, srcDC, 0, 0, 0, 0, WHITENESS);
	
	StretchBlt( destDc.m_hDC, ndl, ndt, ndw, ndh, srcDC, nsl, nst, nsw, nsh, SRCCOPY);

	destDc.SetStretchBltMode( old);

}

void i3TDKPropertyGridProperty_ImageGroup::UpdateFromProperty( void* pData)
{
	SetInfoText();			// Print image information
	_CalcRect();
		
	CRect rt;	
	OnPosSizeChanged( rt);	// OnPosSizeChanged 호출 해줘야함....

	DrawImage();
}

//BOOL i3TDKPropertyGridProperty_ImageGroup::OnSetCursor() const
//{
//	return i3TDKPropertyGridProperty_Group::OnSetCursor();
//}
//void i3TDKPropertyGridProperty_ImageGroup::OnDrawName( CDC* pDC, CRect rect)
//{
//	i3TDKPropertyGridProperty::OnDrawName( pDC, rect);
//}
//
//void i3TDKPropertyGridProperty_ImageGroup::OnDrawExpandBox( CDC* pDC, CRect rectExpand)
//{
//	i3TDKPropertyGridProperty::OnDrawExpandBox( pDC, rectExpand);
//}
//
//void i3TDKPropertyGridProperty_ImageGroup::OnClickName( CPoint C)
//{
//	i3TDKPropertyGridProperty::OnClickName( C);
//}

#endif