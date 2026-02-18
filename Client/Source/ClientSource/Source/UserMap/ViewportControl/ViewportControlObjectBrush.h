#if !defined( __VIEWPORT_CONTROL_OBJECT_BRUSH_H)
#define __VIEWPORT_CONTROL_OBJECT_BRUSH_H

#include "ViewportControl.h"
#include "../UserMap_Object.h"

class CViewportControlObjectBrush : public CViewportControl
{
	I3_CLASS_DEFINE( CViewportControlObjectBrush);
protected:
	CUserMap_Object		*		m_pPlaceObject;

public:
	CViewportControlObjectBrush(void);
	virtual ~CViewportControlObjectBrush(void);

	void		Start( USMAP::BLOCK::ID id);
	void		Cancel(void);
	void		End(void);

	virtual void OnLButtonDown( UINT nFlags, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnMouseMove( UINT nFlags, POINT point);
};

#endif
