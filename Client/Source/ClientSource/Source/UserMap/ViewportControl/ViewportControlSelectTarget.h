#if !defined( __VIEWPORT_CONTROL_SELECT_TARGET_H)
#define __VIEWPORT_CONTROL_SELECT_TARGET_H

#include "ViewportControl.h"

class CViewportControlSelectTarget : public CViewportControl
{
	I3_CLASS_DEFINE( CViewportControlSelectTarget);
protected:

protected:
	void	DrawSelectRect( POINT pt);
	void	BeginMultiSelect( POINT pt);
	void	MultiSelecting( POINT pt);
	void	EndMultiSelect( POINT pt);

public:
	CViewportControlSelectTarget(void);
	virtual ~CViewportControlSelectTarget(void);

	virtual void Create(void);

	virtual void OnLButtonDown( UINT nFlags, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnRButtonDown( UINT nFlags, POINT point);
	virtual void OnMouseMove( UINT nFlags, POINT point);
};

#endif
