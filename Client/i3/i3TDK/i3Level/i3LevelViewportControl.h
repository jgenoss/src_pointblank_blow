#if !defined( __I3_LEVEL_VIEWPORT_CONTROL_H)
#define __I3_LEVEL_VIEWPORT_CONTROL_H

#include "i3LevelAxis.h"

class i3LevelViewport;
class i3LevelFramework;

class I3_EXPORT_TDK i3LevelViewportControl : public i3ElementBase
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3LevelViewportControl, i3ElementBase);
protected:
	i3LevelAxis *				m_pAxis = nullptr;
	HCURSOR						m_hCursor = nullptr;

protected:

public:
	i3LevelViewportControl(void);
	virtual ~i3LevelViewportControl(void);

	i3LevelAxis *		getAxis(void)					{ return m_pAxis; }
	void				setAxis( i3LevelAxis * pAxis);

	static i3LevelAxis *	GetDefaultAxis(void);
	static HCURSOR			GetDefaultCursor(void);

	HCURSOR				getCursor(void)					{ return m_hCursor; }

	virtual void		Create(void);

	virtual void		OnUpdate( REAL32 tm);

	virtual bool OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnRButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point);
	virtual bool OnMouseWheel( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, short zDelta, CPoint pt);
	virtual bool OnKeyDown(  I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual bool OnKeyUp(  I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual bool	OnStart( I3_LEVEL_EDIT_CONTEXT * pCtx);
	virtual bool	OnEnd(  I3_LEVEL_EDIT_CONTEXT * pCtx);
	virtual bool	OnPopupMenu( I3_LEVEL_EDIT_CONTEXT * pCtx, CMenu * pMenu);

};

#endif
