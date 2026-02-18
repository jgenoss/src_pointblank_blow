#pragma once

#define POINT_SIZE 3						///

class CUIGripCtrl;

//□ 
//한 변의 길이가 POINT_SIZE * 2 + 1 인 정사각형
class CUIGripCtrlPoint : public i3ElementBase
{
	I3_CLASS_DEFINE( CUIGripCtrlPoint, i3ElementBase);

protected:
	CUIGripCtrl*	m_pOwner;
	UINT32			m_GripStyle;					// control action type..
	POINT			m_Pos;							// grip의 위치.(중심의 좌표)

public:
	void			OnDraw( HDC hdc, POINT pt);		//기준 위치를 주면 알아서 그린다. 내맘대로..내모냥대로...
	void			OnMoving( POINT ptDelta);		// action type에 따라 실제 action에 의한 결과값 반환.

	void			setPosition( POINT pt);
	POINT			getPosition(void)			{	return m_Pos;}

	UINT32			getStyle(void)				{	return m_GripStyle;}
	void			setStyle( UINT32 style)		{	m_GripStyle = style;}


	bool			ptInRect( CPoint pt);		// pt 가 CUIGripCtrlPoint 안에 있는지 

	CUIGripCtrl*	getOwner( void)		{	return m_pOwner;	}
public:
	CUIGripCtrlPoint(void);
	virtual		~CUIGripCtrlPoint(void);

	void	Create( CUIGripCtrl * pOwner, UINT32 style);
};
