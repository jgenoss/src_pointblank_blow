#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_MOVE_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_OBJECT_MOVE_H

#include "i3LevelViewportControl.h"

class I3_EXPORT_TDK i3LevelViewportControlObjectMove : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelViewportControlObjectMove, i3LevelViewportControl);
protected:

public:
	i3LevelViewportControlObjectMove(void) {}
	virtual ~i3LevelViewportControlObjectMove(void) {}

	virtual void	Create(void);

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
};

#endif
