#if !defined( __GIZMO_MOVE_H_)
#define __GIZMO_MOVE_H_

#include "Gizmo.h"

class CGizmoMove : public CGizmo
{
	I3_CLASS_DEFINE( CGizmoMove);

protected:
	i3Node *		m_pRect[3];
	i3Transform *	m_pTrans[3];
	i3Node *		m_pLine[3];

public:
	CGizmoMove(void);
	virtual ~CGizmoMove(void);

	BOOL	Create();

	void	ChangeLineColorX( COLORREAL * pColor );
	void	ChangeLineColorY( COLORREAL * pColor );
	void	ChangeLineColorZ( COLORREAL * pColor );

	void	ChangeRectColorAll( COLORREAL * pColor );
	void	ChangeRectColorX( COLORREAL * pColor );
	void	ChangeRectColorY( COLORREAL * pColor );
	void	ChangeRectColorZ( COLORREAL * pColor );

	virtual bool	OnCollision(  VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx);
};

#endif