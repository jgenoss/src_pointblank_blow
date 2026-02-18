#if !defined( __CONTROL_MODE_H_)
#define __CONTROL_MODE_H_

#include "../Gizmo/Gizmo.h"

typedef struct _tagi3ViewportControlContext
{
	MATRIX					m_DragStartMatrix;
	POINT					m_DragStartPt;
	VEC3D					m_DragStartPos;
	POINT					m_OldDragPt;
	VEC3D					m_BasePos;
	bool					m_bLBDragging;
	VEC3D					m_GizmoMask;
	bool					m_bLockedFocus;
} VIEWPORT_CONTROL_CONTEXT;

class CStageMapEditor;
class CUserMap;

class CViewportControl : public i3GameNode
{
	I3_ABSTRACT_CLASS_DEFINE( CViewportControl);
protected:
	CGizmo *						m_pGizmo;
	HCURSOR							m_hCursor;

	VIEWPORT_CONTROL_CONTEXT	*	m_pCtrlCtx;
	CStageMapEditor *				m_pStage;
	CUserMap *						m_pMap;

	bool							m_bMovingEnable;

public:
	CViewportControl(void);
	virtual ~CViewportControl(void);

	CGizmo *		getGizmo(void)					{ return m_pGizmo; }
	void			setGizmo( CGizmo * pGizmo)		{ I3_REF_CHANGE( m_pGizmo, pGizmo); }

	static CGizmo *	GetDefaultGizmo(void);
	static HCURSOR	GetDefaultCursor(void);

	HCURSOR			getCursor(void)					{ return m_hCursor; }

	virtual void	Create( CStageMapEditor * pStage);

	virtual void	OnLButtonDown( UINT nFlags, POINT point);
	virtual void	OnLButtonUp( UINT nFlags, POINT point);
	virtual void	OnMButtonDown( UINT nFlags, POINT point);
	virtual void	OnMButtonUp( UINT nFlags, POINT point);
	virtual void	OnRButtonDown( UINT nFlags, POINT point);
	virtual void	OnRButtonUp( UINT nFlags, POINT point);
	virtual void	OnMouseMove( UINT nFlags, POINT point);
	virtual void	OnMouseWheel( UINT nFlags, short zDelta, POINT pt);
	virtual void	OnKeyDown( UINT nChar);
	virtual void	OnKeyUp( UINT nChar);

	virtual bool	OnStart(void);
	virtual bool	OnEnd(void);
	virtual bool	OnPopupMenu( CMenu * pMenu);

	void			LockFocus(bool bLock = true);
	void			SetMovingEnable( bool bEnable = true)	{ m_bMovingEnable = bEnable;}

	void			AttachGizmo(void);
	void			DetachGizmo(void);

	//util func
public:
	void			ScreenToWorld( POINT pt, VEC3D *pvStart, VEC3D *pvEnd);
};

#endif
