#include "stdafx.h"
#include "i3GICShapeBezierLine.h"
#include "i3GICDiagramContext.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeBezierLine);

i3GICShapeBezierLine::i3GICShapeBezierLine()
{
	setStyle( I3GIC_STYLE_BEZIERLINE | I3GIC_STYLE_GDIPLUS | I3GIC_STYLE_NOUPDATE_SIDE_LINK);
}

void i3GICShapeBezierLine::CalculateCurve( void)
{
	m_ControlP1.m_Point.x = m_EndPoint.m_Point.x - (m_EndPoint.m_Point.x - m_StartPoint.m_Point.x) * 0.5f;
	m_ControlP1.m_Point.y = m_StartPoint.m_Point.y;

	m_ControlP2.m_Point.x = m_StartPoint.m_Point.x + (m_EndPoint.m_Point.x - m_StartPoint.m_Point.x) * 0.5f;
	m_ControlP2.m_Point.y = m_EndPoint.m_Point.y;
}

/*virtual*/	void i3GICShapeBezierLine::OnDraw( i3GICDiagramContext * pCtx)
{
	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		POINT p1, p2, p3, p4;

		CalculateCurve();

		p1.x = (INT32)m_StartPoint.m_Point.x;	p1.y = (INT32)m_StartPoint.m_Point.y;

		p2.x = (INT32)m_ControlP1.m_Point.x;	p2.y = (INT32)m_ControlP1.m_Point.y;

		p3.x = (INT32)m_ControlP2.m_Point.x;	p3.y = (INT32)m_ControlP2.m_Point.y;

		p4.x = (INT32)m_EndPoint.m_Point.x;		p4.y = (INT32)m_EndPoint.m_Point.y;

		pCtx->drawBezierLine( getLineStyle(), (REAL32) getLineWidth(), getLineColor(), p1, p2, p3, p4);
	}

	i3GICShape::OnDraw( pCtx);
}

#endif	// I3_DEBUG
