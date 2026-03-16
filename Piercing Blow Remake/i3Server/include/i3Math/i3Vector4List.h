#if !defined( __I3_VECTOR4_LIST_H)
#define __I3_VECTOR4_LIST_H

#include "i3Base.h"
#include "i3MathType.h"
#include "i3Matrix.h"

class I3_EXPORT_MATH i3Vector4List : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Vector4List);

protected :

	INT32	m_MaxCount;
	INT32	m_Count;
	INT32	m_AllocUnit;
	VEC4D	*	m_pList;
	bool	m_bSort;
	
protected:

	BOOL _ResizeBuffer( INT32 Count);
	void	_QuickSort( INT32 idxLeft, INT32 idxRight, REAL32 eps);

public:

	i3Vector4List( INT32 AllocUnit = 16);
	~i3Vector4List(void);

	bool	getSortState(void)						{ return m_bSort; }
	void	setSortState( bool bState)				{ m_bSort = bState; }

	INT32	Add( VEC4D * p, REAL32 eps = I3_EPS);
	INT32	Insert( INT32 Idx, VEC4D * p);
	INT32	Delete( INT32 Idx);
	INT32	Find( VEC4D * p, bool bBilinear, REAL32 eps = I3_EPS);
	void	Sort( REAL32 eps = I3_EPS);
	void	RemoveRedundant( REAL32 eps = I3_EPS);

	void	Reset( bool bFreeBuffer = false);

	void	setAllocationUnit( INT32 AllocUnit);

	INT32	getCount(void)					{ return m_Count; }
	void	setCount( INT32 Count);
	VEC4D *	GetVector( INT32 idx)			{ return &m_pList[ idx]; }
	void	SetVector( INT32 idx, VEC4D * p);

	void	Reverse( INT32 sidx, INT32 eidx);

	virtual void CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
};

#endif
