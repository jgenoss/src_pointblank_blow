#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_SELECT_TARGET_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_SELECT_TARGET_H

#include "i3LevelViewportControl.h"

class I3_EXPORT_TDK i3LevelViewportControlSelectTarget : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelViewportControlSelectTarget, i3LevelViewportControl);
protected:

protected:
	void	DrawSelectRect( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt);
	void	BeginMultiSelect( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt);
	void	MultiSelecting( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt);
	void	EndMultiSelect( I3_LEVEL_EDIT_CONTEXT * pCtx, POINT pt);

public:
	i3LevelViewportControlSelectTarget(void) {}
	virtual ~i3LevelViewportControlSelectTarget(void) {}

	virtual void Create(void);

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
};

#endif
