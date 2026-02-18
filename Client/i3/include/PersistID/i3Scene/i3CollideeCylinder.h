#if !defined( __I3_COLLIDEE_CYLINDER_H)
#define __I3_COLLIDEE_CYLINDER_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Collidee.h"

class I3_EXPORT_SCENE i3CollideeCylinder : public i3Collidee
{
	I3_CLASS_DEFINE( i3CollideeCylinder);

protected:
	// Persistent Members
	VEC3D		m_Start;
	VEC3D		m_Dir;
	REAL32		m_Length;
	REAL32		m_Radius;

	// Volatile Members
	REAL32		m_RadiusSq;

public:
	i3CollideeCylinder(void);

	VEC3D *		GetStart(void)					{ return & m_Start; }
	void		SetStart( VEC3D * pVec)			{ i3Vector::Copy( &m_Start, pVec); }
	void		SetStart( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_Start, x, y, z);
	}

	VEC3D *		GetDirection(void)				{ return & m_Dir; }
	void		SetDirection( VEC3D * pVec)		{ i3Vector::Copy( &m_Dir, pVec); }
	void		SetDirection( REAL32 x, REAL32 y, REAL32 z)
	{
		i3Vector::Set( &m_Dir, x, y, z);
	}

	REAL32		GetRadius(void)					{ return m_Radius; }
	void		SetRadius( REAL32 r)
	{
		m_Radius = r;
		m_RadiusSq = r * r;
	}

	REAL32		GetLength(void)					{ return m_Length; }
	void		SetLength( REAL32 len)			{ m_Length = len; }

	REAL32		GetSquareRadius(void)			{ return m_RadiusSq; }

	virtual void	Transform( MATRIX * pMatrix, i3Collidee * pDest);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
