#pragma once

#include "i3LevelViewportControl.h"

class I3_EXPORT_TDK i3NavMeshViewportControl_VertexSelect : public i3LevelViewportControl
{
	I3_EXPORT_CLASS_DEFINE( i3NavMeshViewportControl_VertexSelect, i3LevelViewportControl );
	
public:
	void		DrawRect( HWND hWnd, const CPoint& pt1, const CPoint& pt2 );

public:
	virtual void	Create() override;

public:
	virtual bool	OnStart( I3_LEVEL_EDIT_CONTEXT * pCtx) override;

public:
	virtual bool	OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override;
	virtual bool	OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override;
	virtual bool	OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override;
	virtual bool	OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override;

public:
	virtual bool	OnRButtonUp(I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point ) override
	{
		return true;
	}
};