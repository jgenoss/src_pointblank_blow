#if !defined( __GIZMO_SCALE_H_)
#define __GIZMO_SCALE_H_

#include "Gizmo.h"

class CGizmoScale: public CGizmo
{
	I3_CLASS_DEFINE( CGizmoScale);

protected:
	i3Transform *		m_pTrans[3];
	i3Node *			m_pCap[3];
	i3Node *			m_pLine[3];

public:
	CGizmoScale(void);
	virtual ~CGizmoScale(void);

	BOOL	Create( void );
	void	Update( void );

	virtual bool	OnCollision( VEC3D * pVStart, VEC3D * pVEnd, MATRIX * pViewProjMtx);
};

#endif