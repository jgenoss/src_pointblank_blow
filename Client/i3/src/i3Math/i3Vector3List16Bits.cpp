#include "i3MathType.h"
#include "i3Vector3List16Bits.h"
#include "i3Vector.h"

#define		NEARESTMULTIPLE( val, unit)			(((val + unit - 1) / unit) * unit)

I3_CLASS_INSTANCE( i3Vector3List16Bits);

i3Vector3List16Bits::~i3Vector3List16Bits(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pList );
}

bool i3Vector3List16Bits::_ResizeBuffer( INT32 nCount )
{
	INT32 NewCount;

	NewCount = NEARESTMULTIPLE( nCount, m_AllocUnit );

	if( m_MaxCount != NewCount)
	{
		if( m_pList == nullptr)
		{
			m_pList = (REAL16 *) i3MemAlloc( (NewCount * sizeof(REAL16) * 3));
		}
		else
		{
			m_pList = (REAL16 *) i3MemReAlloc( m_pList,  NewCount * sizeof(REAL16) * 3);
		}
	}

	m_MaxCount = NewCount;

	return true;
}

void i3Vector3List16Bits::setAllocationUnit( INT32 AllocUnit)
{
	INT32 NewCount;

	m_AllocUnit = AllocUnit;

	NewCount = NEARESTMULTIPLE( m_MaxCount, m_AllocUnit);

	_ResizeBuffer( NewCount);
}

void i3Vector3List16Bits::setCount( INT32 NewCount)
{
	if( NewCount+1 >= m_MaxCount) 
	{
		_ResizeBuffer( NewCount + 1);
	}

	m_Count = NewCount;
}

void i3Vector3List16Bits::SetVector( INT32 idx, VEC3D * p)
{
	if( idx >= m_MaxCount)
	{
		_ResizeBuffer( idx + 1);
	}

	m_pList[idx*3] = getX( p);
	m_pList[idx*3+1] = getY( p);
	m_pList[idx*3+2] = getZ( p);
}

inline INT32	_compVEC3D( VEC3D * pVec1, VEC3D * pVec2, REAL32 eps)
{
	VEC3D d;

	i3Vector::Sub( &d, pVec1, pVec2);

	if( i3Math::abs( d.x) > eps)
	{
		if( d.x > 0.0f)
			return 1;
		else
			return -1;
	}

	if( i3Math::abs( d.y) > eps)
	{
		if( d.y > 0.0f)
			return 1;
		else
			return -1;
	}

	if( i3Math::abs( d.z) > eps)
	{
		if( d.z > 0.0f)
			return 1;
		else
			return -1;
	}

	return 0;
}

inline INT32	isSameVEC3D( VEC3D * pVec1, VEC3D * pVec2)
{
	if( pVec1->x > pVec2->x)
		return 1;
	else if( pVec1->x < pVec2->x)
		return -1;

	if( pVec1->y > pVec2->y)
		return 1;
	else if( pVec1->y < pVec2->y)
		return -1;

	if( pVec1->z > pVec2->z)
		return 1;
	else if( pVec1->z < pVec2->z)
		return -1;

	return 0;
}

INT32 i3Vector3List16Bits::Add( VEC3D * p, REAL32 eps)
{
	bool Rv;
	INT32 idx;

	if( m_Count+1 >= m_MaxCount)
	{
		Rv = _ResizeBuffer( m_Count + 1);
		if( Rv == false)
		{
			I3TRACE( "i3Vector3List16Bits::Add() _ResizeBuffer() failed\n");
			return -1;
		}
	}

	idx = m_Count;

	SetVector( m_Count, p);

	m_Count++;

	return idx;
}

INT32 i3Vector3List16Bits::Delete( INT32 Idx)
{
	INT32 MoveLength;

	if( m_Count == 0)
	{
		return -1;
	}

	MoveLength = m_Count - Idx - 1;
	if( MoveLength > 0)
	{
		memcpy( &m_pList[ Idx*3 ], &m_pList[ (Idx+1)*3 ], MoveLength * sizeof(REAL16) * 3);
	}
	m_Count--;

	return m_Count;
}

INT32 i3Vector3List16Bits::Insert( INT32 idx, VEC3D * p)
{
	bool Rv;
	size_t MoveSize;

	if(m_Count+1 >= m_MaxCount)
	{
		Rv = _ResizeBuffer( m_Count + 1);
		if( Rv == false)
			return -1;
	}

	MoveSize = ( m_Count - idx) * sizeof(REAL16) * 3;

	if( MoveSize > 0)
		memmove( &(m_pList[ (idx+1)*3 ]), &(m_pList[ idx * 3 ]), MoveSize);

	SetVector( idx, p);
	
	m_Count++;

	return idx;
}

INT32	i3Vector3List16Bits::Find( VEC3D * p, bool bBilinear, REAL32 eps)
{
	INT32 start, end;
	INT32 idx, rv, i;

	VEC3D vTemp;

	if( bBilinear)
	{
		if( m_Count <= 0)
			return -1;
			
		start = 0;
		end = m_Count - 1;

		while( start < end)
		{
			idx = (start + end) >> 1;

			GetVector( idx, &vTemp);
			
			rv = _compVEC3D( &vTemp, p, eps);

			if( rv == 0)
				return idx;
			 
			if( rv > 0)
				end = idx - 1;
			else
				start = idx + 1;
		}

		if( start <= end)
		{	
			idx = (start + end) >> 1;

			GetVector( idx, &vTemp);
			
			if( _compVEC3D( &vTemp, p, eps) == 0)
				return idx;
		}
	}
	else
	{
		for( i = 0; i < m_Count; i++)
		{
			GetVector( i, &vTemp);

			if( _compVEC3D( &vTemp, p, eps) == 0)
				return i;
		}
	}
	
	return -1;
}

void i3Vector3List16Bits::_QuickSort( INT32 idxMin, INT32 idxMax, REAL32 eps)
{
	INT32 i = idxMin;
	INT32 j = idxMax;
	VEC3D  id;
	VEC3D vTemp;
	
	GetVector( (i + j) >> 1, &id);

	do
	{
		#if 0
			while( isSameVEC3D( &m_pList[i], &id) < 0)	i++;
			while( isSameVEC3D( &m_pList[j], &id) > 0)	j--;
		#else
			GetVector( i, &vTemp);

			while( _compVEC3D( &vTemp, &id, eps) < 0)
			{
				i++;
				GetVector( i, &vTemp);
			}

			GetVector( j, &vTemp);
			while( _compVEC3D( &vTemp, &id, eps) > 0)
			{
				j--;
				GetVector( j, &vTemp);
			}
		#endif

		if( i <= j)
		{
			VEC3D temp, temp2;

			GetVector( i, &temp);
			GetVector( j, &temp2);
			SetVector( i, &temp2);
			SetVector( j, &temp);

			i++;
			j--;
		}

	} while( i <= j);

	if( idxMin < j)		_QuickSort( idxMin, j, eps);
	if( i < idxMax)		_QuickSort( i, idxMax, eps);
}

void i3Vector3List16Bits::Sort( REAL32 eps)
{
	_QuickSort( 0, m_Count - 1, eps);

	#if 0
		INT32 i;

		for( i = 1; i < m_Count; i++)
		{
			I3ASSERT( isSameVEC3D( &m_pList[i-1], &m_pList[i]) <= 0);
		}
	#endif
}

void i3Vector3List16Bits::RemoveRedundant( REAL32 eps)
{
#if 1
	INT32 i, j;
	VEC3D vTemp1, vTemp2;

	for( i = 0; i < m_Count - 1; i++)
	{
		for( j = i + 1; j < m_Count; j++)
		{
			if( j == i)
				continue;

			GetVector( i, &vTemp1);
			GetVector( j, &vTemp2);

			if( _compVEC3D( &vTemp1, &vTemp2, eps) == 0)
			{
				if( j < i)	i--;

				Delete( j);
				j--;
			}
		}
	}
#else
	INT32 i;

	for( i = 1; i < m_Count - 1; i++)
	{
		if( _compVEC3D( &m_pList[ i -1], &m_pList[i], eps) == 0)
		{
			Delete( i);
			i--;
		}
	}
#endif
}

void i3Vector3List16Bits::Reset( bool bFreeBuffer)
{
	if( bFreeBuffer)
	{
		_ResizeBuffer( m_AllocUnit);
	}

	m_Count = 0;
}

void i3Vector3List16Bits::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT(i3::same_of<i3Vector3List16Bits*>(pDest));
	
	i3Vector3List16Bits * pList = (i3Vector3List16Bits *) pDest;

	pList->setCount( getCount());

	if( getCount() > 0)
	{
		memcpy( pList->m_pList, m_pList, getDataSize() );
	}
}

void	i3Vector3List16Bits::Reverse( INT32 sidx, INT32 eidx)
{
	while( sidx < eidx)
	{
		VEC3D temp, temp2;

		GetVector( sidx, &temp);
		GetVector( eidx, &temp2);
		SetVector( sidx, &temp2);
		SetVector( eidx, &temp);

		sidx++;
		eidx--;
	}
}

void	i3Vector3List16Bits::TransformPoints( MATRIX * pMtx)
{
	INT32 i;
	VEC3D vTemp1, vTemp2;

	for( i = 0; i < m_Count; i++)
	{
		GetVector( i, &vTemp1);

		i3Vector::TransformCoord( &vTemp2, &vTemp1, pMtx);

		SetVector( i, &vTemp2);
	}
}