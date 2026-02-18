#if !defined( __VIEWPORT_CONTROL_OBJECT_SCALE_H)
#define __VIEWPORT_CONTROL_OBJECT_SCALE_H

#include "ViewportControl.h"

class CViewportControlObjectScale : public CViewportControl
{
	I3_CLASS_DEFINE( CViewportControlObjectScale);
protected:

public:
	CViewportControlObjectScale(void);
	virtual ~CViewportControlObjectScale(void);

	virtual void	Create(void);

	virtual void OnLButtonDown(  UINT nFlags, POINT point);
	virtual void OnLButtonUp(  UINT nFlags, POINT point);
	virtual void OnMouseMove(  UINT nFlags, POINT point);

};

#endif
