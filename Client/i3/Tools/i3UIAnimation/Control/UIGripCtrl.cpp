#include "StdAfx.h"
#include "UIGripCtrl.h"

//I3_ABSTRACT_CLASS_INSTANCE( CUIGripCtrl, i3ElementBase);
I3_CLASS_INSTANCE( CUIGripCtrl);

CUIGripCtrl::CUIGripCtrl(void)
{
	m_pHandler = NULL;

	POINT pt;
	pt.x = pt.y = 0;

	setScrollPos( pt);
	setZoomRate( 1.0f);
}

CUIGripCtrl::~CUIGripCtrl(void)
{
	
	for(size_t i=0; i<m_CtrlPointList.size(); ++i)
	{
		CUIGripCtrlPoint * pPoint = m_CtrlPointList[i];

		pPoint->Release();
	}
	m_CtrlPointList.clear();

	I3_SAFE_RELEASE( m_pHandler);
}

// 마우스 이동시 동작..
void CUIGripCtrl::OnMouseMove( POINT ptDelta)
{

}

//각 컨트롤 포인트들에게 draw하도록 지시한다.
void CUIGripCtrl::OnDraw(HDC hdc, REAL32 fZoom)
{

}

void CUIGripCtrl::setPos(REAL32 x, REAL32 y)
{
	I3ASSERT( m_pHandler != NULL);

	m_pHandler->setPos( (REAL32) x, (REAL32) y);

	RecalcLayout();
}

void CUIGripCtrl::setPosX( INT32 x)
{
	I3ASSERT( m_pHandler != NULL);

	m_pHandler->setPositionX( (REAL32) x);
}

void CUIGripCtrl::setPosY( INT32 y)
{
	I3ASSERT( m_pHandler != NULL);

	m_pHandler->setPositionY( (REAL32) y);
}

void CUIGripCtrl::setWidth( INT32 w)
{
	I3ASSERT( m_pHandler != NULL);

	m_pHandler->setWidth( (REAL32) w);
}

void CUIGripCtrl::setHeight( INT32 h)
{
	I3ASSERT( m_pHandler != NULL);

	m_pHandler->setHeight( (REAL32) h);
}

REAL32 CUIGripCtrl::getPosX(void)
{
	I3ASSERT( m_pHandler != NULL);

	return  m_pHandler->getLeft();
}

REAL32 CUIGripCtrl::getPosY(void)
{
	I3ASSERT( m_pHandler != NULL);

	return  m_pHandler->getTop();
}

REAL32 CUIGripCtrl::getWidth(void)
{
	I3ASSERT( m_pHandler != NULL);

	return (m_pHandler->getRight() - m_pHandler->getLeft());
}

REAL32 CUIGripCtrl::getHeight(void)
{
	I3ASSERT( m_pHandler != NULL);

	return (m_pHandler->getBottom() - m_pHandler->getTop());
}

void CUIGripCtrl::setScrollPos( POINT pos)
{	
	m_ScrollPos.x = pos.x;
	m_ScrollPos.y = pos.y;

	RecalcLayout();
}

void CUIGripCtrl::setZoomRate( REAL32 rate)					
{	
	m_Zoom = rate;	

	RecalcLayout();
}

void CUIGripCtrl::setOffsetPos( POINT pos)					
{	
	m_OffsetPos.x = pos.x;
	m_OffsetPos.y = pos.y;	

	RecalcLayout();
}

void CUIGripCtrl::setOffsetValues( POINT pos, POINT offset, REAL32 rate)
{
	m_ScrollPos.x = pos.x;
	m_ScrollPos.y = pos.y;

	m_Zoom = rate;	

	m_OffsetPos.x = pos.x;
	m_OffsetPos.y = pos.y;	

	RecalcLayout();
}
