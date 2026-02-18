#include "stdafx.h"
#include "i3TDKRibbonColor.h"
#include "i3TDKColorPicker.h"

IMPLEMENT_DYNCREATE(i3TDKRibbonColor, CMFCRibbonButton)

i3TDKRibbonColor::~i3TDKRibbonColor(void)
{
	if( m_hBrush != nullptr)
	{
		::DeleteObject( m_hBrush);
		m_hBrush = nullptr;
	}
}

void i3TDKRibbonColor::reset(void)
{
	m_ImageSz.cx = 24;
	m_ImageSz.cy = 24;

	setColor( 0.0f, 0.0f, 0.0f, 0.0f);
}

CSize i3TDKRibbonColor::GetImageSize(RibbonImageType type) const
{
	return m_ImageSz;
}

void i3TDKRibbonColor::setColor( REAL32 r, REAL32 g, REAL32 b, REAL32 a)
{
	i3Color::Set( &m_Color, r, g, b, a);

	UINT8 ur, ug, ub;

	ur = (UINT8)(m_Color.r * 255.0f);
	ug = (UINT8)(m_Color.g * 255.0f);
	ub = (UINT8)(m_Color.b * 255.0f);

	if( m_hBrush != nullptr)
	{
		::DeleteObject( m_hBrush);
	}

	m_hBrush = ::CreateSolidBrush( RGB( ur, ug, ub));
}

void i3TDKRibbonColor::setColor( COLORREAL * pCol)
{
	setColor( pCol->r, pCol->g, pCol->b, pCol->a);
}

void i3TDKRibbonColor::DrawImage( CDC* pDC, RibbonImageType type, CRect rectImage)
{
	::FillRect( pDC->m_hDC, &rectImage, m_hBrush);
}

void i3TDKRibbonColor::OnClick(CPoint point)
{
	i3TDKColorPicker dlg;

	if( dlg.Execute( &m_Color) == false)
		return;

	setColor( dlg.getSelectedColor());

	if( this->GetOriginal() != nullptr)
	{
		i3TDKRibbonColor * pSrc = (i3TDKRibbonColor *) GetOriginal();

		pSrc->setColor( getColor());
	}

	CMFCRibbonButton::OnClick( point);
}

void i3TDKRibbonColor::CopyFrom(const CMFCRibbonBaseElement& s)
{
	ASSERT_VALID(this);

	CMFCRibbonBaseElement::CopyFrom(s);

	i3TDKRibbonColor & src = (i3TDKRibbonColor&) s;

	setColor( src.getColor());
}