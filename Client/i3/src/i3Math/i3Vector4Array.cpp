#include "i3MathType.h"
#include "i3Vector.h"
#include "i3Vector4Array.h"

I3_CLASS_INSTANCE( i3Vector4Array);

i3Vector4Array::~i3Vector4Array(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pVector ); 
	m_VectorCount = 0;
}

void i3Vector4Array::SetVectorCount( UINT32 count)
{
	I3MEM_SAFE_FREE_POINTER( m_pVector ); 

	m_VectorCount = count;	

	if( count > 0)
	{
		//m_VectorIdx = i3MemAlloc( sizeof(VEC4D) * count);
		m_pVector	= (VEC4D *) i3MemAlloc( sizeof(VEC4D) * count);
		I3ASSERT( m_pVector);
	}
}

UINT32 i3Vector4Array::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	i3::pack::VEC4D vec;
	VEC4D * pSrc;
	UINT32 i;

	I3ASSERT( m_VectorCount > 0);

	// Vector Count
	Rc = pStream->Write( &m_VectorCount, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Vector4Array::OnSave()", "Coild not write vector count data.");
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

			Rc = pStream->Write( &vec, sizeof(i3::pack::VEC4D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Vector4Array::OnSave()", "Could not write vector array data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3Vector4Array::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i;
	i3::pack::VEC4D vec;
	VEC4D * pDest;

	// Vector Count
	Rc = pStream->Read( &m_VectorCount, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Vector4Array::OnLoad()", "Could not read vector array data.");
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
			Rc = pStream->Read( &vec, sizeof(i3::pack::VEC4D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Vector4Array::OnLoad()", "Could not read vector data.");
				return STREAM_ERR;
			}
			Result += Rc;

			i3Vector::Set( pDest, vec.x, vec.y, vec.z, vec.w);
		}
	}

	return Result;
}

