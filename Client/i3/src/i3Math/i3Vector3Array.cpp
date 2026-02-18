#include "i3MathType.h"
#include "i3Vector.h"
#include "i3Vector3Array.h"

I3_CLASS_INSTANCE( i3Vector3Array);

i3Vector3Array::~i3Vector3Array(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pVector ); 

	m_VectorCount = 0;
}

void i3Vector3Array::SetVectorCount( UINT32 count)
{
	I3MEM_SAFE_FREE_POINTER( m_pVector ); 

	m_VectorCount = count;	

	if( count > 0)
	{
		m_pVector	= (VEC3D *) i3MemAlloc( sizeof(VEC3D) * count);
		I3ASSERT( m_pVector);
	}
}

INT32 i3Vector3Array::Find( VEC3D * pVec, INT32 limitCount, REAL32 err)
{
	INT32 i;

	if( limitCount < 0)
		limitCount = GetVectorCount();

	for( i = 0; i < limitCount; i++)
	{
		if( i3Math::abs( i3Vector::GetX( GetVector( i)) - i3Vector::GetX( pVec)) <= err &&
			i3Math::abs( i3Vector::GetY( GetVector( i)) - i3Vector::GetY( pVec)) <= err &&
			i3Math::abs( i3Vector::GetZ( GetVector( i)) - i3Vector::GetZ( pVec)) <= err)
		{
			return i;
		}
	}

	return -1;
}

#if defined( I3_CHECK_FLOAT_VALUE_FOR_PHYSICX)
void i3Vector3Array::CheckValid( UINT32 cnt)
{
	UINT32 i;

	if( cnt == 0xFFFFFFFF)
		cnt = m_VectorCount;

	for( i = 0; i < cnt; i++)
	{
		I3ASSERT( i3Math::isValid( m_pVector[i].x));
		I3ASSERT( i3Math::isValid( m_pVector[i].y));
		I3ASSERT( i3Math::isValid( m_pVector[i].z));
	}
}
#endif

UINT32 i3Vector3Array::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	i3::pack::VEC3D vec;
	VEC3D * pSrc;
	UINT32 i;

	I3ASSERT( m_VectorCount > 0);

	// Vector Count
	Rc = pStream->Write( &m_VectorCount, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Vector3Array::OnSave()", "Coild not write vector count data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Vector
	if( m_VectorCount > 0)
	{
		pSrc = m_pVector;

		for( i = 0; i < m_VectorCount; i++, pSrc++)
		{
			vec.x = i3Vector::GetX( pSrc);
			vec.y = i3Vector::GetY( pSrc);
			vec.z = i3Vector::GetZ( pSrc);

			Rc = pStream->Write( &vec, sizeof(i3::pack::VEC3D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Vector3Array::OnSave()", "Could not write vector array data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3Vector3Array::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i;
	i3::pack::VEC3D vec;
	VEC3D * pDest;

	// Vector Count
	Rc = pStream->Read( &m_VectorCount, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Vector3Array::OnLoad()", "Could not read vector array data.");
		return STREAM_ERR;
	}
	Result += Rc;

	I3ASSERT( m_VectorCount > 0);

	SetVectorCount( m_VectorCount);

	// Vectors
	{
		pDest = m_pVector;

		for( i = 0; i < m_VectorCount; i++, pDest++)
		{
			Rc = pStream->Read( &vec, sizeof(i3::pack::VEC3D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Vector3Array::OnLoad()", "Could not read vector data.");
				return STREAM_ERR;
			}
			Result += Rc;

			i3Vector::Set( pDest, vec.x, vec.y, vec.z);
		}
	}

	return Result;
}

