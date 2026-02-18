#pragma once

#include "i3LevelViewportControl.h"

class I3_EXPORT_TDK i3NavMeshViewportControl_VertexAdd : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshViewportControl_VertexAdd, i3LevelViewportControl);
	
public:
	virtual void	Create() override {}

public:
	virtual bool	OnStart(I3_LEVEL_EDIT_CONTEXT * pCtx) override { return true; }

public:
	virtual bool	OnLButtonDown(I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point) override { return true; }
	virtual bool	OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override;
	virtual bool	OnRButtonDown(I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point) override { return true; }
	virtual bool	OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override;

	virtual bool	OnRButtonUp(I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override
	{
		return true;
	}
};