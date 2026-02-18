#if !defined( __I3_BOUNDBOX_H)
#define __I3_BOUNDBOX_H

#include "../i3Base/i3CommonType.h"
#include "i3Volume.h"
#include "i3FixedPointVector.h"

class I3_EXPORT_MATH i3BoundBox : public i3Volume
{
	I3_CLASS_DEFINE( i3BoundBox);

protected:
	RT_VEC3D		m_Min;
	RT_VEC3D		m_Max;
	VEC3D			m_Center;

public:
	i3BoundBox(void);

	RT_VEC3D *		GetMin(void)				{ return &m_Min; }
	RT_VEC3D *		GetMax(void)				{ return &m_Max; }

	void	Reset(void)
	{
		i3Vector::Set( &m_Min, __RT_0, __RT_0, __RT_0);
		i3Vector::Set( &m_Max, __RT_0, __RT_0, __RT_0);
		i3Vector::Set( &m_Center, 0.0f, 0.0f, 0.0f);

		m_bEmpty = TRUE;
	}

	void	SetMinMax( RT_VEC2D * pMin, RT_VEC2D * pMax)
	{
		i3Vector::Set( &m_Min, i3Vector::GetX( pMin ), i3Vector::GetY( pMin ), __RT_0 );
		i3Vector::Set( &m_Max, i3Vector::GetX( pMax ), i3Vector::GetY( pMax ), __RT_0 );

		m_bEmpty = FALSE;
	}

	void	SetMinMax( RT_VEC3D * pMin, RT_VEC3D * pMax)
	{
		i3Vector::Copy( &m_Min, pMin);
		i3Vector::Copy( &m_Max, pMax);

		m_bEmpty = FALSE;
	}

	VEC3D *	getCenter(void)						{ return &m_Center; }

	void	ExtendByVec( RT_VEC2D * pVec);
	void	ExtendByVec( RT_VEC3D * pVec);
	void	ExtendByBoundBox( i3BoundBox * pBox);

	void	GetCornerVertices( VEC3D * pVtx);
	void	GetPlanes( VEC4D * pPlanes);
	void	Transform( i3BoundBox * pOut, MATRIX * pMtx);
	void	IntersectWith( i3BoundBox * pInBox);
	bool	IntersectWithLine( VEC3D * pStart, VEC3D * pDir, VEC3D * pIntersect);

	BOOL	IsTouched( i3BoundBox * pBBox);

#if defined( I3_DEBUG)
	void	vlog( UINT8 r, UINT8 g, UINT8 b);
#endif

	virtual void GetCenter( RT_VEC2D * pVec);
	virtual void GetCenter( RT_VEC3D * pVec);

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
