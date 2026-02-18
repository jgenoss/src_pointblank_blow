#if !defined( __I3_COLLIDEE_CAPSULE_H )
#define __I3_COLLIDEE_CAPSULE_H

// 캡슐 형태의 컬리더
// 2006. 2. 20 by dfly79
// 2개의 점을 반구로하고 그 사이는 원통이다
// 반구는 비대칭일 수 있다... 따라서 각 반구의 반지름이 다르게 설정할 수 있도록 해야된다.

#include "i3Base.h"
#include "i3Math.h"
#include "i3Collidee.h"

class I3_EXPORT_SCENE i3CollideeCapsule : public i3Collidee
{
	I3_EXPORT_CLASS_DEFINE( i3CollideeCapsule, i3Collidee);

protected:
	// Persistent Members
	VEC3D		m_Start;
	VEC3D		m_Dir;
	REAL32		m_Length = 0.0f;
	REAL32		m_Radius = 0.0f;

	// Volatile Members
	REAL32		m_RadiusSq = 0.0f;

public:
	VEC3D *		getStart(void)					{ return & m_Start; }
	void		setStart( VEC3D * pVec)			{ i3Vector::Copy( &m_Start, pVec); }
	void		setStart( REAL32 x, REAL32 y, REAL32 z) { i3Vector::Set( &m_Start, x, y, z); }

	VEC3D *		getDirection(void)				{ return & m_Dir; }
	void		setDirection( VEC3D * pVec)		{ i3Vector::Copy( &m_Dir, pVec); }
	void		setDirection( REAL32 x, REAL32 y, REAL32 z) { i3Vector::Set( &m_Dir, x, y, z); }

	REAL32		getRadius(void)			{ return m_Radius; }
	void		setRadius( REAL32 r)
	{
		m_Radius = r;
		m_RadiusSq = r * r;
	}

	REAL32		getLength(void)					{ return m_Length; }
	void		setLength( REAL32 len)			{ m_Length = len; }

	REAL32		getSquareRadius(void)		{ return m_RadiusSq; }


public:
	virtual void	Transform( MATRIX * pMatrix, i3Collidee * pDest) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};



#endif // __I3_COLLIDEE_CAPSULE_H
