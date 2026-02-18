#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_SCALE_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_SCALE_H

#include "i3LevelViewportControl.h"

class I3_EXPORT_TDK i3LevelViewportControlObjectScale : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelViewportControlObjectScale, i3LevelViewportControl);
protected:
	
public:
	i3LevelViewportControlObjectScale(void) {}
	virtual ~i3LevelViewportControlObjectScale(void) {}

	virtual void	Create(void);

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);

};

#endif
