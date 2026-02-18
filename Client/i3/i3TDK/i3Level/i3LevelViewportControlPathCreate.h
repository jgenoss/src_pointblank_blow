#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_PATH_CREATE_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_PATH_CREATE_H

#include "i3LevelViewportControl.h"
#include "i3LevelPath.h"
#include "i3LevelHitMap.h"

class I3_EXPORT_TDK i3LevelViewportControlPathCreate : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelViewportControlPathCreate, i3LevelViewportControl);
protected:
	i3LevelPath *		m_pPath = nullptr;

	i3Billboard *		m_pDragLink = nullptr;
	bool				m_bDragging = false;

	i3LevelPathPoint *	m_pStartLink = nullptr;
	i3LevelPathPoint *	m_pLastLink = nullptr;

public:
	i3LevelViewportControlPathCreate(void) {}
	virtual ~i3LevelViewportControlPathCreate(void);

	virtual void Create(void);

	virtual bool OnStart( I3_LEVEL_EDIT_CONTEXT * pCtx);

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
};

#endif
