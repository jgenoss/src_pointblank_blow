#include "StdAfx.h"
#include "UIGripCtrlPoint.h"
#include "UIGripCtrl.h"

#include "../GlobalVariables.h"

I3_CLASS_INSTANCE( CUIGripCtrlPoint); //, i3ElementBase);

CUIGripCtrlPoint::CUIGripCtrlPoint(void)
{
	m_pOwner = NULL;
	m_GripStyle = 0;
	m_Pos.x = m_Pos.y = 0;
}

CUIGripCtrlPoint::~CUIGripCtrlPoint(void)
{
}

//기준 위치를 주면 알아서 그린다. 내맘대로..내모냥대로...
void CUIGripCtrlPoint::OnDraw( HDC dc, POINT pt)
{
	REAL32 ratio = 1.f / g_fZoom;

	REAL32 size = (POINT_SIZE * ratio) + 0.5f;

	INT32 X = m_Pos.x + pt.x - (INT32)size;
	INT32 Y = m_Pos.y + pt.y - (INT32)size;

	INT32 width = X + (INT32)(size * 2) + 1;
	INT32 height = Y + (INT32)(size * 2) + 1;
	
	::Rectangle( dc, X, Y, width, height);
}
	
void CUIGripCtrlPoint::OnMoving( POINT ptDelta)		// action type에 따라 실제 action에 의한 결과값 반환.
{
	if( m_pOwner != NULL)
		m_pOwner->OnNotifyMove( ptDelta, this);
}

void CUIGripCtrlPoint::setPosition( POINT pt)
{
	m_Pos.x = pt.x;
	m_Pos.y = pt.y;
}

void CUIGripCtrlPoint::Create( CUIGripCtrl * pOwner, UINT32 style)
{
	m_pOwner = pOwner;

	m_GripStyle = style;
}

bool CUIGripCtrlPoint::ptInRect( CPoint pt)
{
	REAL32 ratio = 1.f / g_fZoom;

	REAL32 size = (POINT_SIZE * ratio) + 0.5f;

	INT32 X = m_Pos.x - (INT32) size;
	INT32 Y = m_Pos.y - (INT32) size;

	if( pt.x < X || pt.y < Y)
		return false;

	X = X + (INT32)(size * 2) + 1;
	Y = Y + (INT32)(size * 2) + 1;

	if( pt.x > X || pt.y > Y)
		return false;

	return true;
}