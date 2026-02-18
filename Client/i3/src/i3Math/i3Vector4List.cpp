#include "i3MathType.h"
#include "i3Vector4List.h"
#include "i3Vector.h"

#define		NEARESTMULTIPLE( val, unit)			(((val + unit - 1) / unit) * unit)

I3_CLASS_INSTANCE( i3Vector4List);

i3Vector4List::~i3Vector4List(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pList ); 
}

bool i3Vector4List::_ResizeBuffer( INT32 nCount )
{
	INT32 NewCount;

	NewCount = NEARESTMULTIPLE( nCount, m_AllocUnit );

	if( m_MaxCount != NewCount)
	{
		if( m_pList == nullptr)
		{
			m_pList = (VEC4D *) i3MemAlloc( (NewCount * sizeof(VEC4D))); 
		}
		else
		{
			m_pList = (VEC4D *) i3MemReAlloc( m_pList,  NewCount * sizeof(VEC4D) ); 
		}
	}

	m_MaxCount = NewCount;

	return true;
}

void i3Vector4List::setAllocationUnit( INT32 AllocUnit)
{
	INT32 NewCount;

	m_AllocUnit = AllocUnit;

	NewCount = NEARESTMULTIPLE( m_MaxCount, m_AllocUnit);

	_ResizeBuffer( NewCount);
}

void i3Vector4List::setCount( INT32 NewCount)
{
	if( NewCount+1 >= m_MaxCount) 
	{
		_ResizeBuffer( NewCount + 1);
	}

	m_Count = NewCount;
}

void i3Vector4List::SetVector( INT32 idx, VEC4D * p)
{
	if( idx >= m_MaxCount)
	{
		_ResizeBuffer( idx + 1);
	}

	i3Vector::Copy( &m_pList[idx], p);
}

//#pragma optimize( "t", on)

inline INT32	_compVEC4D( VEC4D * pVec1, VEC4D * pVec2, REAL32 eps)
{
	VEC4D d;

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

	if( i3Math::abs( d.w) > eps)
	{
		if( d.w > 0.0f)
			return 1;
		else
			return -1;
	}

	return 0;
}

inline INT32	isSameVEC4D( VEC4D * pVec1, VEC4D * pVec2)
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

	if( pVec1->w > pVec2->w)
		return 1;
	else if( pVec1->w < pVec2->w)
		return -1;

	return 0;
}

INT32 i3Vector4List::Add( VEC4D * p, REAL32 eps)
{
	bool Rv;
	INT32 idx;

	if( m_Count+1 >= m_MaxCount)
	{
		Rv = _ResizeBuffer( m_Count + 1);
		if( Rv == false)
		{
			I3TRACE( "i3Vector3List::Add() _ResizeBuffer() failed\n");
			return -1;
		}
	}

	idx = m_Count;

	i3Vector::Copy( &m_pList[m_Count], p);
	m_Count++;

	return idx;
}

INT32 i3Vector4List::Delete( INT32 Idx)
{
	INT32 MoveLength;

	if( m_Count == 0)
	{
		return -1;
	}

	MoveLength = m_Count - Idx - 1;
	if( MoveLength > 0)
		memcpy( &m_pList[Idx], &m_pList[Idx+1], MoveLength * sizeof(VEC4D));
	m_Count--;

	return m_Count;
}

INT32 i3Vector4List::Insert( INT32 idx, VEC4D * p)
{
	size_t MoveSize;

	if(m_Count+1 >= m_MaxCount)
	{
		if(_ResizeBuffer(m_Count + 1) == false)
			return -1;
	}

	MoveSize = ( m_Count - idx) * sizeof(VEC4D);

	if( MoveSize > 0)
	{
		memmove( &(m_pList[idx+1]), &(m_pList[idx]), MoveSize);
	}

	i3Vector::Copy( & m_pList[idx], p);
	m_Count++;

	return idx;
}

INT32	i3Vector4List::Find( VEC4D * p, bool bBilinear, REAL32 eps)
{
	INT32 start, end;
	INT32 idx, rv;

	if( bBilinear)
	{
		if( m_Count <= 0)
			return -1;
			
		start = 0;
		end = m_Count - 1;

		while( start < end)
		{
			idx = (start + end) >> 1;
			
			rv = _compVEC4D( &m_pList[idx], p, eps);
			
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
			
			if( _compVEC4D( &m_pList[idx], p, eps) == 0)
				return idx;
		}
	}
	else
	{
		for( idx = 0; idx < m_Count; idx++)
		{
			if( _compVEC4D( &m_pList[idx], p, eps) == 0)
				return idx;
		}
	}
	
	return -1;
}

void i3Vector4List::_QuickSort( INT32 idxMin, INT32 idxMax, REAL32 eps)
{
	INT32 i = idxMin;
	INT32 j = idxMax;
	VEC4D  id;
	
	i3Vector::Copy( &id, &m_pList[ (i + j) >> 1]);

	do
	{
		#if 0
			while( isSameVEC4D( &m_pList[i], &id) < 0)	i++;
			while( isSameVEC4D( &m_pList[j], &id) > 0)	j--;
		#else
			while( _compVEC4D( &m_pList[i], &id, eps) < 0)	i++;
			while( _compVEC4D( &m_pList[j], &id, eps) > 0)	j--;
		#endif

		if( i <= j)
		{
			VEC4D temp;

			i3Vector::Copy( &temp, &m_pList[i]);
			i3Vector::Copy( &m_pList[i], &m_pList[j]);
			i3Vector::Copy( &m_pList[j], &temp);

			i++;
			j--;
		}

	} while( i <= j);

	if( idxMin < j)		_QuickSort( idxMin, j, eps);
	if( i < idxMax)		_QuickSort( i, idxMax, eps);
}

void i3Vector4List::Sort( REAL32 eps)
{
	_QuickSort( 0, m_Count - 1, eps);
}

void i3Vector4List::Reset( bool bFreeBuffer)
{
	if( bFreeBuffer)
	{
		_ResizeBuffer( m_AllocUnit);
	}

	m_Count = 0;
}

void i3Vector4List::RemoveRedundant( REAL32 eps)
{
#if 1
	#if 1
		INT32 i, j, cnt;
		bool *	pDeleteTable = new bool[ m_Count];

		i3mem::FillZero( pDeleteTable, sizeof(bool) * m_Count);

		// 중복되는 벡터들을 표시한다.
		for( i = 0; i < m_Count - 1; i++)
		{
			if( pDeleteTable[i] == true)
				continue;

			for( j = i + 1; j < m_Count; j++)
			{
				if( pDeleteTable[j] == true)
					continue;

				if( _compVEC4D( &m_pList[i], &m_pList[j], eps) == 0)
				{
					pDeleteTable[j] = true;
				}
			}
		}

		// 실질적인 벡터들의 개수를 구한다.
		for( i = 0, cnt = 0; i < m_Count; i++)
		{
			if( pDeleteTable[i] == false)
				cnt++;
		}

		if( cnt < m_Count)
		{
			// 새로운 Array에 유효한 Vector들만 복사.
			for( i = 0, j = 0; i < m_Count; i++)
			{
				if( pDeleteTable[i] == false)
				{
					if( j != i)
						i3Vector::Copy( & m_pList[j], &m_pList[i]);
					j++;
				}
			}

			I3ASSERT( j == cnt);

			m_Count = cnt;
		}

		I3_SAFE_DELETE_ARRAY( pDeleteTable);

	#else
		INT32 i, j;

		for( i = 0; i < m_Count - 1; i++)
		{
			for( j = i + 1; j < m_Count; j++)
			{
				if( j == i)
					continue;

				if( _compVEC4D( &m_pList[i], &m_pList[j], eps) == 0)
				{
					if( j < i)	i--;

					Delete( j);
					j--;
				}
			}
		}
	#endif
#else
	INT32 i;

	for( i = 1; i < m_Count - 1; i++)
	{
		if( _compVEC4D( &m_pList[ i -1], &m_pList[i], eps) == 0)
		{
			Delete( i);
			i--;
		}
	}
#endif
}

//#pragma optimize( "", on)

void i3Vector4List::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( i3::same_of<i3Vector4List*>(pDest));
	
	i3Vector4List * pList = (i3Vector4List *) pDest;

	pList->setCount( getCount());

	if( getCount() > 0)
	{
		memcpy( pList->m_pList, m_pList, sizeof(VEC4D) * m_Count);
	}
}

void	i3Vector4List::Reverse( INT32 sidx, INT32 eidx)
{
	while( sidx < eidx)
	{
		VEC4D temp;

		i3Vector::Copy( &temp, &m_pList[sidx]);
		i3Vector::Copy( &m_pList[sidx], &m_pList[eidx]);
		i3Vector::Copy( &m_pList[eidx], &temp);

		sidx++;
		eidx--;
	}
}
