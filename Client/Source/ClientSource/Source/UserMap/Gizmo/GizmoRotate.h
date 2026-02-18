#if !defined( __GIZMO_ROTATE_H_)
#define __GIZMO_ROTATE_H_

#include "Gizmo.h"

#define		NUM_LINE_SEG		30

class CGizmoRotate : public CGizmo
{
	I3_CLASS_DEFINE( CGizmoRotate);

protected:
	i3Transform *		m_pRotateRoot;
	i3Node *			m_pRotateLine;
	i3Transform *		m_pTrans[3];
	i3Transform *		m_pAxisTrans[3];
	i3Node *			m_pLine[3];

	VEC3D				m_LineVec[NUM_LINE_SEG];

	VEC3D				m_PickedPos;

protected:
	bool		_HitTestWithLine( VEC3D * pStart, VEC3D * pEnd);

public:
	CGizmoRotate(void);
	virtual ~CGizmoRotate(void);

	BOOL	Create( void );

	VEC3D *	getPickedPos(void)				{ return &m_PickedPos; }

//	void	RotateArc( I3_LEVEL_VIEWPORT_CTRL_CONTEXT * pCtx, VEC3D * pRotate );

	void	RotateLine( VEC3D * pRotate );
	BOOL	RotateLineX( REAL32 fDeg );
	BOOL	RotateLineY( REAL32 fDeg );
	BOOL	RotateLineZ( REAL32 fDeg );

	virtual bool	OnCollision(  VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx);
};

#endif