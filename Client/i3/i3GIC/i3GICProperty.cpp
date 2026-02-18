#include "stdafx.h"
#include "i3GICProperty.h"
#include "i3GIC.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#if defined( I3_DEBUG)

IMPLEMENT_DYNAMIC(i3GICProperty, CMFCPropertyGridProperty)

void i3GICProperty::_UpdateName( void)
{
	COleVariant var = GetValue();

	if( var.bstrVal != _T('\0'))
	{
		i3::string str;
		
		i3::utf16_to_mb( (const wchar_t*) var.bstrVal , str);

		m_pShape->SetName( str);
	}
	
}

void i3GICProperty::_Update2DPosition( void)
{
	CMFCPropertyGridProperty * pSub1 = GetSubItem( 0);
	CMFCPropertyGridProperty * pSub2 = GetSubItem( 1);

	if( (pSub1 == nullptr) || (pSub2 == nullptr) )
		return;

	REAL32 x, y;

	{
		COleVariant var = pSub1->GetValue();
		x = var.fltVal;
	}

	{
		COleVariant var = pSub2->GetValue();
		y = var.fltVal;
	}

	m_pShape->setPos( x, y);
}

void i3GICProperty::_Update3DPosition( void)
{
}

void i3GICProperty::_Update2DSize( void)
{
	CMFCPropertyGridProperty * pSub1 = GetSubItem( 0);
	CMFCPropertyGridProperty * pSub2 = GetSubItem( 1);

	if( (pSub1 == nullptr) || (pSub2 == nullptr) )
		return;

	REAL32 x, y;

	{
		COleVariant var = pSub1->GetValue();
		x = var.fltVal;
	}

	{
		COleVariant var = pSub2->GetValue();
		y = var.fltVal;
	}

	m_pShape->setSize( x, y);
}

void i3GICProperty::_Update3DSize( void)
{
}


void i3GICProperty::_UpdateBorderSize( void)
{
	COleVariant var = GetValue();
	
	if( i3::kind_of<i3GICShapeRect*>(m_pShape))
	{
		((i3GICShapeRect*)m_pShape)->setBorderSize( var.uintVal);

		if( var.uintVal == 0)
			((i3GICShapeRect*)m_pShape)->addStyle( I3GIC_STYLE_NOBORDERLINE);
		else
			((i3GICShapeRect*)m_pShape)->removeStyle( I3GIC_STYLE_NOBORDERLINE);
	}
}

void i3GICProperty::_UpdateLineWidth( void)
{
	COleVariant var = GetValue();
	
	if( i3::kind_of<i3GICShapeLine*>(m_pShape))
	{
		((i3GICShapeLine*)m_pShape)->setLineWidth( var.uintVal);
	}
}

void i3GICProperty::_UpdateText( void)
{
	COleVariant var = GetValue();

	if( i3::kind_of<i3GICShapeTextRect*>(m_pShape))
	{
		if( var.bstrVal != _T('\0'))
		{
			i3::string str;
			i3::utf16_to_mb( (const wchar_t*)var.bstrVal, str);
			((i3GICShapeTextRect*) m_pShape)->setText( str.c_str() );
		}
	}
}

void i3GICProperty::_UpdateFill( void)
{
	COleVariant var = GetValue();

	if( i3::kind_of<i3GICShapeFigure*>(m_pShape))
	{
		if( var.boolVal)
			((i3GICShapeFigure*) m_pShape)->removeStyle( I3GIC_STYLE_NOFILL);
		else
			((i3GICShapeFigure*) m_pShape)->addStyle( I3GIC_STYLE_NOFILL);
	}
}

void i3GICProperty::_UpdateRoundValue( void)
{
	COleVariant var = GetValue();
	
	if( i3::kind_of<i3GICShapeRect* >(m_pShape))
	{
		((i3GICShapeRect*)m_pShape)->setCornerRadius( (REAL32) var.uintVal);
	}
}

void i3GICProperty::_UpdateBorderStyle( void)
{
	COleVariant var = GetValue();

	if( i3::kind_of<i3GICShapeFigure*>(m_pShape))
	{
		char szTemp[260];

		GIC_SHAPE_BORDER_STYLE style;

		WideCharToMultiByte( GetACP(), 0, var.bstrVal, -1, szTemp, sizeof( szTemp), nullptr, nullptr);

		if( i3::generic_is_iequal( szTemp, "Dash") )	style = GIC_SHAPE_BORDER_STYLE_DASH;
		else if( i3::generic_is_iequal( szTemp, "Dot") )	style = GIC_SHAPE_BORDER_STYLE_DOT;
		else if( i3::generic_is_iequal( szTemp, "DashDot") )	style = GIC_SHAPE_BORDER_STYLE_DASHDOT;
		else if( i3::generic_is_iequal( szTemp, "DashDotDot") )	style = GIC_SHAPE_BORDER_STYLE_DASHDOTDOT;
		else
			style = GIC_SHAPE_BORDER_STYLE_SOLID;

		
		((i3GICShapeFigure*)m_pShape)->setBorderStyle( style);
	}
}

void i3GICProperty::_UpdateLineStyle( void)
{
	COleVariant var = GetValue();

	if( i3::kind_of<i3GICShapeLine*>(m_pShape))
	{
		char szTemp[260];

		GIC_LINE_STYLE style;

		WideCharToMultiByte( GetACP(), 0, var.bstrVal, -1, szTemp, sizeof( szTemp), nullptr, nullptr);

		if( i3::generic_is_iequal( szTemp, "Dash") )	style = GIC_LINE_STYLE_DASH;
		else if( i3::generic_is_iequal( szTemp, "Dot") )	style = GIC_LINE_STYLE_DOT;
		else if( i3::generic_is_iequal( szTemp, "DashDot") )	style = GIC_LINE_STYLE_DASHDOT;
		else if( i3::generic_is_iequal( szTemp, "DashDotDot") )	style = GIC_LINE_STYLE_DASHDOTDOT;
		else
			style = GIC_LINE_STYLE_SOLID;

		
		((i3GICShapeLine*)m_pShape)->setLineStyle( style);
	}
}

void i3GICProperty::_UpdateFontStyle( void)
{
}

void i3GICProperty::_UpdateFontSize( void)
{
}

void i3GICProperty::_UpdateBarValue( void)
{
	COleVariant var = GetValue();

	if( i3::kind_of<i3GICShapeCircleBar* >(m_pShape))
	{
		((i3GICShapeCircleBar*)m_pShape)->setAngle( (REAL32)var.fltVal);
	}
	else if( i3::kind_of<i3GICShapeRectBar* >(m_pShape))
	{
		((i3GICShapeRectBar*)m_pShape)->setBarValue( (REAL32)var.fltVal);
	}
}

void i3GICProperty::_UpdateBarWidth( void)
{
	COleVariant var = GetValue();

	if( i3::kind_of<i3GICShapeCircleBar* >(m_pShape))
	{
		((i3GICShapeCircleBar*)m_pShape)->setBarWidth( (UINT32)var.uintVal);
	}
	else if( i3::kind_of<i3GICShapeRectBar* >(m_pShape))
	{
		((i3GICShapeRectBar*)m_pShape)->setBarWidth( (UINT32)var.uintVal);
	}
}

void i3GICProperty::_UpdateShapeStyle( void)
{
	COleVariant var = GetValue();

	i3::string str;

	i3::utf16_to_mb((const wchar_t*)var.bstrVal, str );

	I3GIC_STYLE style = m_pShape->getStyleByStr( str.c_str());

	m_pShape->removeStyle( I3GIC_STYLE_MASK_FLAG);
	m_pShape->addStyle( style);
}

void i3GICProperty::UpdateChangeValue( void)
{
	if( m_pShape != nullptr)
	{

		switch( m_UpdateType)
		{
		case GIC_PROPERTY_UPDATE_NAME :				_UpdateName();				break;
		case GIC_PROPERTY_UPDATE_POSITION_2D :		_Update2DPosition();		break;
		case GIC_PROPERTY_UPDATE_POSITION_3D :		_Update3DPosition();		break;
		case GIC_PROPERTY_UPDATE_SIZE_2D :			_Update2DSize();			break;
		case GIC_PROPERTY_UPDATE_SIZE_3D :			_Update3DSize();			break;
		case GIC_PROPERTY_UPDATE_BORDERSIZE :		_UpdateBorderSize();		break;
		case GIC_PROPERTY_UPDATE_LINEWIDTH :		_UpdateLineWidth();			break;
		case GIC_PROPERTY_UPDATE_TEXT :				_UpdateText();				break;
		case GIC_PROPERTY_UPDATE_FILL :				_UpdateFill();				break;
		case GIC_PROPERTY_UPDATE_ROUNDVALUE :		_UpdateRoundValue();		break;
		case GIC_PROPERTY_UPDATE_BORDERSTYLE :		_UpdateBorderStyle();		break;
		case GIC_PROPERTY_UPDATE_LINESTYLE :		_UpdateLineStyle();			break;
		case GIC_PROPERTY_UPDATE_BARVALUE :			_UpdateBarValue();			break;
		case GIC_PROPERTY_UPDATE_BARWIDTH :			_UpdateBarWidth();			break;
		case GIC_PROPERTY_UPDATE_SHAPESTYLE :		_UpdateShapeStyle();		break;
		}

		m_pShape->addState( I3GIC_STATE_REDRAW);
	}
}


/*virtual*/ BOOL i3GICProperty::OnUpdateValue( void)
{
	BOOL bRv = CMFCPropertyGridProperty::OnUpdateValue();

	if( bRv )
	{
		if( (GetParent() != nullptr) &&
			(GetParent()->GetRuntimeClass() == i3GICProperty::GetThisClass()) )
		{
			((i3GICProperty*)GetParent())->UpdateChangeValue();
		}
		else
			UpdateChangeValue();
	}

	return bRv;
}


#endif	// I3_DEBUG
