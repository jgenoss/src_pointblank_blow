#if !defined( __VIEWPORT_CONTROL_OBJECT_MOVE_H)
#define __VIEWPORT_CONTROL_OBJECT_MOVE_H

#include "ViewportControl.h"

class CViewportControlObjectMove : public CViewportControl
{
	I3_CLASS_DEFINE( CViewportControlObjectMove);
protected:

public:
	CViewportControlObjectMove(void);
	virtual ~CViewportControlObjectMove(void);

	virtual void	Create(void);

	virtual void OnLButtonDown( UINT nFlags, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnMouseMove( UINT nFlags, POINT point);
};

#endif
