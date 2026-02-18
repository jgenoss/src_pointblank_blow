#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_BRUSH_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_BRUSH_H

#include "i3LevelViewportControl.h"

class I3_EXPORT_TDK i3LevelViewportControlObjectBrush : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelViewportControlObjectBrush, i3LevelViewportControl);
protected:
	void	PlaceNewObject( I3_LEVEL_EDIT_CONTEXT * pCtx, BOOL bClone, CPoint point);

public:
	i3LevelViewportControlObjectBrush(void) {}
	virtual ~i3LevelViewportControlObjectBrush(void) {}

	virtual void	Create(void);

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
};

#endif
