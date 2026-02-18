#if !defined( __I3_VECTOR3_LIST_H)
#define __I3_VECTOR3_LIST_H

#include "i3Base.h"
#include "i3MathType.h"
#include "i3Matrix.h"

class I3_EXPORT_MATH i3Vector3List : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Vector3List, i3ElementBase);

protected :

	INT32	m_MaxCount = 0;
	INT32	m_Count = 0;
	INT32	m_AllocUnit;
	VEC3D	*	m_pList = nullptr;
	
protected:

	bool _ResizeBuffer( INT32 Count);
	void	_QuickSort( INT32 sidx, INT32 eidx, REAL32 eps);

public:

	i3Vector3List(INT32 AllocUnit = 16) : m_AllocUnit(AllocUnit) {}
	~i3Vector3List(void);

	INT32	Add( VEC3D * p, REAL32 eps = I3_EPS);
	INT32	Insert( INT32 Idx, VEC3D * p);
	INT32	Delete( INT32 Idx);
	INT32	Find( VEC3D * p, bool bBilinear, REAL32 eps = I3_EPS);

	void	Reset( bool bFreeBuffer = false);

	void	setAllocationUnit( INT32 AllocUnit);

	INT32	getCount(void)					{ return m_Count; }
	void	setCount( INT32 Count);
	VEC3D *	GetVector( INT32 idx)			{ return &m_pList[ idx]; }
	void	SetVector( INT32 idx, VEC3D * p);

	void	Sort( REAL32 eps = I3_EPS);
	void	RemoveRedundant( REAL32 eps = I3_EPS);

	void	Reverse( INT32 sidx, INT32 eidx);
	void	TransformPoints( MATRIX * pMtx)	{ i3Matrix::TransformPoints( m_pList, m_pList, m_Count, pMtx); }

#if defined( I3_DEBUG)
	void	vlog( bool bPlot, UINT8 r, UINT8 g, UINT8 b);
#endif

	virtual void CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
};

#endif
