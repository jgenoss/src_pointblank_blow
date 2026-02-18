#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_ROTATE_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_ROTATE_H

#include "i3LevelViewportControl.h"

class I3_EXPORT_TDK i3LevelViewportControlObjectRotate : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelViewportControlObjectRotate, i3LevelViewportControl);
protected:
	i3TextNode *		m_pTextNode = nullptr;

public:
	i3LevelViewportControlObjectRotate(void) {}
	virtual ~i3LevelViewportControlObjectRotate(void);

	virtual void		Create(void);

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);

};

#endif
