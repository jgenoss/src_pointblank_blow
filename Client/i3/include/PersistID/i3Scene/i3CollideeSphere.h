#if !defined( __I3_COLLIDEE_SPHERE_H)
#define __I3_COLLIDEE_SPHERE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Collidee.h"
#include "i3Node.h"

class I3_EXPORT_SCENE i3CollideeSphere : public i3Collidee
{
	I3_CLASS_DEFINE( i3CollideeSphere);

protected:
	VEC3D		m_Center;
	REAL32		m_Radius;
	REAL32		m_RadiusSqr;

public:
	i3CollideeSphere(void);

	VEC3D *		GetCenter(void)					{ return & m_Center; }
	REAL32		GetRadius(void)					{ return m_Radius; }

	REAL32		GetSquareRadius(void)			{ return m_RadiusSqr; }

	void		SetCenter( VEC3D * pVec)		{ i3Vector::Copy( &m_Center, pVec); }
	void		SetCenter( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_Center, x, y, z);
	}

	void		SetRadius( REAL32 val)
	{ 
		m_RadiusSqr = val * val;
		m_Radius = val; 
	}

	virtual void	Transform( MATRIX * pMatrix, i3Collidee * pDest);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	//INT32	TestWithNode( i3Node * pRoot, I3_COLLISION_RESULT * pResultBuf, INT32 MaxResult);
};

#endif
