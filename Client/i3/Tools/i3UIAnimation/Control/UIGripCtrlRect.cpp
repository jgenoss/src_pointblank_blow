#include "StdAfx.h"
#include "UIGripCtrlRect.h"
#include "UIGripCtrlPoint.h"
#include "UIGlobalRes.h"
#include "i3Base/itl/container_util.h"

struct GRIP_INFO
{
	UINT32	style;
	VEC2D	pt;
};

GRIP_INFO s_gripInfo[GER_COUNT] = 
{
	{	UIGRIP_CORNER_LEFT | UIGRIP_CORNER_TOP,		VEC2D(0.0f, 0.0f) },	//GER_LEFT_TOP = 0,	// 좌측 상단
	{	UIGRIP_CORNER_TOP,							VEC2D(0.5f, 0.0f) },	//GER_TOP,			// 상단 중앙
	{	UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_TOP,	VEC2D(1.0f, 0.0f) },	//GER_RIGHT_TOP,	// 우측 상단
	{	UIGRIP_CORNER_RIGHT,						VEC2D(1.0f, 0.5f) },	//GER_RIGHT,		// 우측 중앙
	{	UIGRIP_CORNER_RIGHT | UIGRIP_CORNER_BOTTOM,	VEC2D(1.0f, 1.0f) },	//GER_RIGHT_BOTTOM,	// 우측 하단
	{	UIGRIP_CORNER_BOTTOM,						VEC2D(0.5f, 1.0f) },	//GER_BOTTOM,		// 하단 중앙
	{	UIGRIP_CORNER_LEFT | UIGRIP_CORNER_BOTTOM,	VEC2D(0.0f, 1.0f) },	//BER_LEFT_BOTTOM,	// 좌측 하단
	{	UIGRIP_CORNER_LEFT,							VEC2D(0.0f, 0.5f) }		//GER_LEFT,			// 좌측 중앙
};

I3_CLASS_INSTANCE( CUIGripCtrlRect); //, CUIGripCtrl);

CUIGripCtrlRect::CUIGripCtrlRect(void)
{
	m_PenColor = COLORSET_GRIP_NORMAL_BORDER;
	m_BrushColor = COLORSET_GRIP_NORMAL_BACK;
}

CUIGripCtrlRect::~CUIGripCtrlRect(void)
{
//	m_CtrlPointList.SafeReleaseAll();
	i3::cu::for_each_safe_release_clear(m_CtrlPointList, &CUIGripCtrlPoint::Release);
}

void CUIGripCtrlRect::RecalcLayout(void)
{
	INT32 i;
	for( i = 0;i < GER_COUNT; ++i)
	{
		CUIGripCtrlPoint * pPoint = m_CtrlPointList[i];

		POINT pt;
		pt.x = (INT32)(s_gripInfo[i].pt.x * m_pHandler->getWidth() * m_Zoom);
		pt.y = (INT32)(s_gripInfo[i].pt.y * m_pHandler->getHeight() * m_Zoom);

		pPoint->setPosition( pt);
	}

	m_Pos.x = (INT32)(m_pHandler->getRelativeLeft() * m_Zoom) + m_OffsetPos.x;
	m_Pos.y = (INT32)(m_pHandler->getRelativeTop() * m_Zoom) + m_OffsetPos.y;
}

bool CUIGripCtrlRect::Create( CUIGripObjHandler * pHandler)
{
	I3ASSERT( pHandler != NULL);

	INT32 i;
	for( i = 0;i < GER_COUNT; ++i)
	{
		CUIGripCtrlPoint * pPoint = CUIGripCtrlPoint::new_object();

		pPoint->Create( this, s_gripInfo[i].style);

		m_CtrlPointList.push_back( pPoint);
	}

	setHandler( pHandler);

	RecalcLayout();

	return true;
}

void CUIGripCtrlRect::OnDraw( HDC hdc)						// 각 컨트롤 포인트들에게 draw하도록 지시한다.
{
	HGDIOBJ hOldBrush = (HGDIOBJ)::SelectObject( hdc, (HBRUSH) ::GetStockObject( NULL_BRUSH));
	HGDIOBJ hOldPen = (HGDIOBJ)::SelectObject( hdc, UIGlobalRes::getPen( m_PenColor));

	INT32 i;

	// Draw Border Lines
	{
		POINT pt[4];
		static INT32	s_Index[4] = 
		{
			GER_LEFT_TOP,
			GER_RIGHT_TOP,
			GER_RIGHT_BOTTOM,
			GER_LEFT_BOTTOM
		};

		for( i = 0;i < 4; ++i)
		{
			CUIGripCtrlPoint * pPoint = m_CtrlPointList[s_Index[i]];

			pt[i].x = m_Pos.x + pPoint->getPosition().x;
			pt[i].y = m_Pos.y + pPoint->getPosition().y;

		}

		::Polygon( hdc, pt, 4);

		hOldBrush = (HGDIOBJ)::SelectObject( hdc, (HBRUSH) ::GetStockObject( NULL_BRUSH));
		hOldPen = (HGDIOBJ)::SelectObject( hdc, UIGlobalRes::getPen( COLORSET_GRIP_SELECT_BORDER));
		
		if( i3::same_of<CUIGripObjHandler_UIControl*>(m_pHandler))
			((i3UIControl*)m_pHandler->getObject())->GripDraw(hdc);
	}

	// Draw Points
	{
		::SelectObject( hdc, UIGlobalRes::getBrush( m_BrushColor));

		for( i = 0;i < GER_COUNT; ++i)
		{
			CUIGripCtrlPoint * pPoint = m_CtrlPointList[i];

			pPoint->OnDraw( hdc, m_Pos);
		}
	}
	
	::SelectObject( hdc, hOldPen);
	::SelectObject( hdc, hOldBrush);
}

CUIGripCtrlPoint * CUIGripCtrlRect::isPoint( CPoint pt)		// 소유 하고 있는 point들 중에 pt를 포함하는 point가 있는지 찾는다.
{
	CPoint localPT = pt - m_Pos + m_OffsetPos + m_ScrollPos;
	INT32 i;
	for( i = 0;i < GER_COUNT; ++i)
	{
		CUIGripCtrlPoint * pPoint = m_CtrlPointList[i];

		if( pPoint->ptInRect( localPT))
			return pPoint;
	}

	return NULL;
}

bool CUIGripCtrlRect::ptInRect( CPoint pt)					// 현재 shape rect내부에 pt가 있는지의 여부를 반환한다.
{
	CRect rt;
	rt.left = (INT32)(m_pHandler->getRelativeLeft() * m_Zoom) - m_ScrollPos.x;// + m_OffsetPos.x;
	rt.top = (INT32)(m_pHandler->getRelativeTop() * m_Zoom) - m_ScrollPos.y;// + m_OffsetPos.y;
	rt.right = (INT32)(m_pHandler->getRelativeRight() * m_Zoom) - m_ScrollPos.x;// + m_OffsetPos.x;
	rt.bottom = (INT32)(m_pHandler->getRelativeBottom() * m_Zoom) - m_ScrollPos.y;// + m_OffsetPos.y;

	rt.NormalizeRect();

	if(rt.left > pt.x || rt.right < pt.x)
		return false;

	if(rt.top > pt.y || rt.bottom < pt.y)
		return false;

	return true;
}

void CUIGripCtrlRect::OnNotifyMove( POINT ptDelta, CUIGripCtrlPoint * pPoint)
{
	if( ptDelta.x == 0 && ptDelta.y == 0)
		return;

	UINT32 style = pPoint->getStyle();

	I3ASSERT( !((style & UIGRIP_CORNER_LEFT) && (style & UIGRIP_CORNER_RIGHT)));
	I3ASSERT( !((style & UIGRIP_CORNER_TOP) && (style & UIGRIP_CORNER_BOTTOM)));

	REAL32 rate = 1.0f;
	if( i3Math::abs( m_Zoom) > I3_EPS)
	{
		rate = 1.0f / m_Zoom;
	}

	if( style & UIGRIP_CORNER_LEFT)		m_pHandler->moveLeft((REAL32)ptDelta.x * rate);
	if( style & UIGRIP_CORNER_TOP)		m_pHandler->moveTop((REAL32)ptDelta.y * rate);
	if( style & UIGRIP_CORNER_RIGHT)	m_pHandler->moveRight((REAL32)ptDelta.x * rate);
	if( style & UIGRIP_CORNER_BOTTOM)	m_pHandler->moveBottom((REAL32)ptDelta.y * rate);
}

void CUIGripCtrlRect::OnMoving( CPoint ptDelta)
{
	if( ptDelta.x == 0 && ptDelta.y == 0)
		return;

	//if( i3::same_of<CUIGripObjHandler_UIControl*>(m_pHandler)) //->IsExactTypeOf( CUIGripObjHandler_UIControl::static_meta()))
	//{
	//	i3UIControl* pControl = (i3UIControl*)m_pHandler->getObject();
	//	if( i3::same_of<i3UIListView_Item*>(pControl)) //->IsExactTypeOf( i3UIListView_Item::static_meta()))
	//	{
	//		i3UIListView_Item* pItem = (i3UIListView_Item*)pControl;
	//		
	//		if( pItem->getSelectedSep() != -1)	
	//			return;	// Resizing cell
	//	}
	//}

	REAL32 X = m_pHandler->getLeft() + ptDelta.x / m_Zoom;
	REAL32 Y = m_pHandler->getTop() + ptDelta.y / m_Zoom;

	m_pHandler->setPos( X, Y);

	 //m_wndPropList. Update();
	//i3UIControl* pControl = (i3UIControl*)m_pHandler->getObject();
	//i3TDK::Update( NULL, I3_TDK_UPDATE_MOVE, pControl);
}