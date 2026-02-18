#include "i3MathType.h"
#include "i3Vector.h"
#include "i3Vector2Array.h"

I3_CLASS_INSTANCE( i3Vector2Array);

i3Vector2Array::~i3Vector2Array(void)
{
	//I3MEM_SAFE_FREE( m_pVector, m_hVectorMemID );
	I3MEM_SAFE_FREE_POINTER( m_pVector );

	m_VectorCount = 0;
}

void i3Vector2Array::SetVectorCount( UINT32 count)
{
	I3MEM_SAFE_FREE_POINTER( m_pVector );

	m_VectorCount = count;	
	if( count > 0)
	{
		m_pVector		= (VEC2D *) i3MemAlloc( sizeof(VEC2D) * count);
		I3ASSERT( m_pVector);
	}
}

UINT32 i3Vector2Array::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	i3::pack::VEC2D vec;
	VEC2D * pSrc;
	UINT32 i;

	// Vector Count
	Rc = pStream->Write( &m_VectorCount, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Vector2Array::OnSave()", "Coild not write vector count data.");
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

			Rc = pStream->Write( &vec, sizeof(i3::pack::VEC2D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Vector2Array::OnSave()", "Could not write vector array data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3Vector2Array::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i;
	i3::pack::VEC2D vec;
	VEC2D * pDest;

	// Vector Count
	Rc = pStream->Read( &m_VectorCount, sizeof(UINT32));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Vector2Array::OnLoad()", "Could not read vector array data.");
		return STREAM_ERR;
	}
	Result += Rc;

	SetVectorCount( m_VectorCount);

	// Vectors
	{
		pDest = m_pVector;

		for( i = 0; i < m_VectorCount; i++, pDest++)
		{
			Rc = pStream->Read( &vec, sizeof(i3::pack::VEC2D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3Vector2Array::OnLoad()", "Could not read vector data.");
				return STREAM_ERR;
			}
			Result += Rc;

			i3Vector::Set( pDest, vec.x, vec.y );
		}
	}

	return Result;
}

