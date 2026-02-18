#pragma once

// 그냥 만들면 1개짜리 free move한 control point를 생성. 맘껏 움직일수 있는 스타일로 만들어지고,
// 기본적으루다가 마우스의 이동, 모드 설정에 대한 부분 넘기고
// 모양을 관리한다.( 2개 이상의 control point를 가지면 그 사이에 line을 그려 넣어 1개의 컨트롤을 완성시킨다.

#include "UIGripCtrlPoint.h"
#include "UIGripObjHandler.h"

class CUIGripCtrl : public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( CUIGripCtrl, i3ElementBase);

protected:
	i3::vector<CUIGripCtrlPoint*> m_CtrlPointList;

	bool					m_bSelected;

	POINT					m_Pos;
	
	POINT					m_ScrollPos;
	POINT					m_OffsetPos;
	REAL32					m_Zoom;

	CUIGripObjHandler	*	m_pHandler;

public:
	CUIGripCtrl(void);
	virtual ~CUIGripCtrl(void);

	virtual	void		RecalcLayout(void) { }

	virtual bool		Create( CUIGripObjHandler * pHandler) = 0;

	void				setHandler( CUIGripObjHandler * pHandler)	{	I3_REF_CHANGE( m_pHandler, pHandler);}
	CUIGripObjHandler	*getHandler(void)							{	return m_pHandler;}

	void				setScrollPos( POINT pos);
	void				setZoomRate( REAL32 rate);
	void				setOffsetPos( POINT pos);
	void				setPos( POINT pos);
	void				setPosX( INT32 x);
	void				setPosY( INT32 y);
	void				setWidth( INT32 w);
	void				setHeight( INT32 h);

	INT32				getPosX(void);
	INT32				getPosY(void);
	INT32				getWidth(void);
	INT32				getHeight(void);

	void				setOffsetValues( POINT pos, POINT offset, REAL32 rate);

	void			*	getObject(void)								{	return m_pHandler->getObject();}

	virtual void		OnMouseMove( POINT ptDelta);					// 마우스 이동시 동작..
	virtual void		OnNotifyMove( POINT ptDelta, CUIGripCtrlPoint * pPoint){};

	virtual void		OnDraw( HDC hdc);								// 각 컨트롤 포인트들에게 draw하도록 지시한다.

	virtual void		OnSelect(void)								{	m_pHandler->OnSelect();}
	virtual void		OnUnSelect(void)							{	m_pHandler->OnUnSelect();}					

	const i3::vector<CUIGripCtrlPoint*>&	getPointList( void)	const	{	return m_CtrlPointList;	}

	INT32				getAccPosX( void)	{	return m_Pos.x;	}
	INT32				getAccPosY( void)	{	return m_Pos.y;	}
};
