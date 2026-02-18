#if !defined( __VIEWPORT_CONTROL_OBJECT_ROTATE_H)
#define __VIEWPORT_CONTROL_OBJECT_ROTATE_H

#include "ViewportControl.h"

class CViewportControlObjectRotate : public CViewportControl
{
	I3_CLASS_DEFINE( CViewportControlObjectRotate);
protected:

public:
	CViewportControlObjectRotate(void);
	virtual ~CViewportControlObjectRotate(void);

	virtual void		Create(void);

	virtual void OnLButtonDown( UINT nFlags, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnMouseMove( UINT nFlags, POINT point);

};

#endif
