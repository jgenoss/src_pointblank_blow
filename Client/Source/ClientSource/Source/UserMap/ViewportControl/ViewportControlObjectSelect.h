#if !defined( __VIEWPORT_CONTROL_OBJECT_SELECT_H)
#define __VIEWPORT_CONTROL_OBJECT_SELECT_H

#include "ViewportControl.h"

class CViewportControlObjectSelect : public CViewportControl
{
	I3_CLASS_DEFINE( CViewportControlObjectSelect);
protected:

protected:
	void	DrawSelectRect( POINT pt);
	void	BeginMultiSelect( POINT pt);
	void	MultiSelecting( POINT pt);
	void	EndMultiSelect( POINT pt);

public:
	CViewportControlObjectSelect(void);
	virtual ~CViewportControlObjectSelect(void);

	virtual void Create(void);

	virtual void OnLButtonDown( UINT nFlags, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnMouseMove( UINT nFlags, POINT point);
};

#endif
