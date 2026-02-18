#pragma once

// 8 point짜리 control point들을 생성, 해당 corner및 edge에 따라 움직임이 다르다.
// 기본적으루다가 마우스의 이동, 모드 설정에 대한 부분 넘기고
// 모양을 관리한다.( 2개 이상의 control point를 가지면 그 사이에 line을 그려 넣어 1개의 컨트롤을 완성시킨다.

#define UIGRIP_CORNER_LEFT		0x00000001
#define UIGRIP_CORNER_TOP		0x00000002
#define UIGRIP_CORNER_RIGHT		0x00000004
#define UIGRIP_CORNER_BOTTOM	0x00000008

enum GRIP_EDGE_RECT
{
	GER_LEFT_TOP = 0,	// 좌측 상단
	GER_TOP,			// 상단 중앙
	GER_RIGHT_TOP,		// 우측 상단
	GER_RIGHT,			// 우측 중앙
	GER_RIGHT_BOTTOM,	// 우측 하단
	GER_BOTTOM,			// 하단 중앙
	GER_LEFT_BOTTOM,	// 좌측 하단
	GER_LEFT,			// 좌측 중앙

	GER_COUNT,
};

#include "UIGripCtrl.h"
#include "UIGripCtrlPoint.h"
#include "../UIGlobalRes.h"

class CUIGripCtrlRect : public CUIGripCtrl
{
	I3_CLASS_DEFINE( CUIGripCtrlRect, CUIGripCtrl);

	COLORSET		m_PenColor;
	COLORSET		m_BrushColor;

public:
	CUIGripCtrlRect(void);
	virtual ~CUIGripCtrlRect(void);

public:
	virtual	void		RecalcLayout(void) override;

	virtual bool		Create( CUIGripObjHandler * pHandler) override;
	virtual void		OnDraw( HDC hdc) override;						// 각 컨트롤 포인트들에게 draw하도록 지시한다.

	virtual void		OnMoving( CPoint ptDelta);

	CUIGripCtrlPoint *	isPoint( CPoint pt);					// 소유 하고 있는 point들 중에 pt를 포함하는 point가 있는지 찾는다.
	bool				ptInRect( CPoint pt);					// 현재 shape rect내부에 pt가 있는지의 여부를 반환한다.

	void				setPenColor( COLORSET cs)		{	m_PenColor = cs;	}
	void				setBrushColor( COLORSET cs)		{	m_BrushColor = cs;	}

	virtual void		OnNotifyMove( POINT ptDelta, CUIGripCtrlPoint * pPoint) override;
};
