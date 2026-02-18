#include "stdafx.h"
#include "i3GICColorProperty.h"
#include "i3GICShape.h"
#include "i3GICShapeFigure.h"
#include "i3GICShapeLine.h"
#include "i3GICShapeTextRect.h"
#include "i3GICShapeCircleBar.h"
#include "i3GICShapeRectBar.h"

#if defined( I3_DEBUG)

IMPLEMENT_DYNAMIC( i3GICColorProperty, CMFCPropertyGridColorProperty)


void i3GICColorProperty::_UpdateColorToShape( void)
{
	I3ASSERT( m_pShape != nullptr);

	switch( m_UpdateType)
	{
	case GIC_PROPERTY_UPDATE_COLOR_BK:
		if( i3::kind_of<i3GICShapeFigure* >(m_pShape))
		{
			((i3GICShapeFigure*)m_pShape)->setFillColor( GetColor());		
		}
		break;
	case GIC_PROPERTY_UPDATE_COLOR_BORDER :
		if( i3::kind_of<i3GICShapeFigure* >(m_pShape))
		{
			((i3GICShapeFigure*)m_pShape)->setBorderColor( GetColor());
		}
		break;
	case GIC_PROPERTY_UPDATE_COLOR_LINE :
		if( i3::kind_of<i3GICShapeLine* >(m_pShape))
		{
			((i3GICShapeLine*)m_pShape)->setLineColor( GetColor());
		}
		break;
	case GIC_PROPERTY_UDPATE_COLOR_FONT :
		if( i3::kind_of<i3GICShapeTextRect* >(m_pShape))
		{
			((i3GICShapeTextRect*)m_pShape)->setFontColor( GetColor());
		}
		break;

	case GIC_PROPERTY_UDPATE_COLOR_BAR :
		if( i3::kind_of<i3GICShapeCircleBar* >(m_pShape))
		{
			((i3GICShapeCircleBar*)m_pShape)->setBarColor( GetColor());
		}
		else if( i3::kind_of<i3GICShapeRectBar* >(m_pShape))
		{
			((i3GICShapeRectBar*)m_pShape)->setBarColor( GetColor());
		}
		break;
	}
}

/*virtual*/ BOOL i3GICColorProperty::OnUpdateValue( void)
{
	BOOL bRv = CMFCPropertyGridColorProperty::OnUpdateValue();

	if( bRv && (m_pShape != nullptr))
	{
		_UpdateColorToShape();
		m_pShape->addState( I3GIC_STATE_REDRAW);
	}

	return bRv;
}

#endif	// I3_DEBUG
