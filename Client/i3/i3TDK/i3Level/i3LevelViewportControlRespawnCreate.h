#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_RESPAWN_CREATE_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_RESPAWN_CREATE_H

#include "i3LevelViewportControl.h"
//#include "i3LevelRespawn.h"

class I3_EXPORT_TDK i3LevelViewportControlRespawnCreate : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelViewportControlRespawnCreate, i3LevelViewportControl);
protected:
	BOOL					m_bCreated = FALSE;

protected:
	void	GetPlacePos( I3_LEVEL_EDIT_CONTEXT * pCtx, CPoint point, VEC3D * pDest);

public:
	i3LevelViewportControlRespawnCreate(void) {}
	virtual ~i3LevelViewportControlRespawnCreate(void) {}

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
};

#endif