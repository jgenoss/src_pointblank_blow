#include "i3GFXType.h"
#include "../i3Math/i3Color.h"

#ifdef I3_DEBUG
#include "../i3base/i3Debug.h"
#endif

#ifdef I3G_XBOX
#include "i3GfxUtilXbox.h"
#include "i3VertexArrayXbox.h"
#include "i3GfxGlobalVariableXbox.h"

I3_CONCRETE_CLASS_INSTANCE( i3VertexArrayXbox, i3VertexArray);

i3VertexArrayXbox::i3VertexArrayXbox(void)
{
	m_pVertexBuf = NULL;
	
	m_OffsetPos = 0;
	m_OffsetRHW = 0;
	m_OffsetWeight = 0;
	m_OffsetNormal = 0;
	m_OffsetPointSize = 0;
	m_OffsetColor = 0;

	m_OffsetTexCoord[0] = 0;
}

i3VertexArrayXbox::~i3VertexArrayXbox(void)
{
	if( m_pVertexBuf != NULL)
	{
		m_pVertexBuf->Release();
		m_pVertexBuf = NULL;
	}
}

UINT32 i3VertexArrayXbox::CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count)
{
	// Position (X, Y, Z) Component
	{
		m_OffsetPos = 0;

		if( pFormat->GetHasPosition())
			m_Stride += sizeof(REAL32) * 3;
	}

	// RHW(Reciprocal Homogeneous W) Component
	{
		m_OffsetRHW = m_Stride;

		if( pFormat->IsTransformed() && !pFormat->GetHasNormal())
			m_Stride += sizeof(REAL32);
	}

	// Blend Weight Component
	{
		m_OffsetWeight = m_Stride;

		if( pFormat->GetBlendWeightCount() > 0)
			m_Stride += sizeof(REAL32) * pFormat->GetBlendWeightCount();
	}

	// Normal Component
	{
		m_OffsetNormal = m_Stride;

		if( pFormat->GetHasNormal())
			m_Stride += sizeof(REAL32) * 3;
	}

	// Point-Sprite Size Component
	{
		m_OffsetPointSize = m_Stride;

		if( pFormat->GetPointSpriteCoordCount() > 0)
		{
			#if defined( I3_DEBUG)
			if( pFormat->GetPointSpriteCoordCount() != 1)
			{
				i3Log( "i3VertexArrayXbox::CalcBufferSize()", "More than 1 point-sprite size is not supported on DirectX.");
			}
			#endif
			m_Stride += sizeof(REAL32) * pFormat->GetPointSpriteCoordCount();
		}
	}

	// Vertex Diffuse Color Component
	{
		m_OffsetColor = m_Stride;

		if( pFormat->GetHasColor())
			m_Stride += sizeof(UINT32);
	}

	// Texture Coord. Set Components
	{
		UINT32 i;

		for( i = 0; pFormat->GetTextureCoordSetCount(); i++)
		{
			m_OffsetTexCoord[i] = m_Stride;

			m_Stride += sizeof(REAL32) * pFormat->GetTextureCoordCount();
		}
	}

	return m_Stride * Count;
}

BOOL i3VertexArrayXbox::Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage)
{
	HRESULT Rv;
	UINT32 BuffSz, Usage, FVF;
	D3DPOOL Pool;

	I3ASSERT( g_pD3DDevice);

	i3VertexArray::Create( pFormat, count, usage);

	BuffSz = CalcBufferSize( pFormat, count);

	Usage = i3_GetNativeUsage( usage);

	FVF = i3_GetNativeVertexFormat( pFormat);

	Pool = D3DPOOL_MANAGED;

	Rv = g_pD3DDevice->CreateVertexBuffer( BuffSz, Usage, FVF, Pool, &m_pVertexBuf);
	I3ASSERT( !FAILED( Rv));

	return TRUE;
};

BOOL i3VertexArrayXbox::Lock( I3G_LOCKMETHOD lock, UINT32 StartIdx, UINT32 EndIdx)
{
	HRESULT Rv;
	UINT StartOff, Size;

	if( m_bLocked)
		return FALSE;

	I3ASSERT( m_pVertexBuf);

	StartOff = StartIdx * m_Stride;

	m_LockStartIdx = StartIdx;

	if( EndIdx == 0)
	{
		Size = m_Count * m_Stride;
	}
	else
	{
		Size = (EndIdx - StartIdx + 1) * m_Stride;
	}

	Rv = m_pVertexBuf->Lock( StartOff, Size, (BYTE **) & m_pLockedBuf, lock);
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DebugBox(NULL, "i3VertexArrayXbox::Lock() failed", Rv);
		return FALSE;
	}
	#endif

	m_bLocked = TRUE;

	return TRUE;
}

void i3VertexArrayXbox::Unlock(void)
{
	HRESULT Rv;

	if( ! IsLocked())
	{
		return;
	}

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( m_pLockedBuf);

	Rv = m_pVertexBuf->Unlock();
	#if defined( I3_DEBUG)
	if( FAILED( Rv))
	{
		DebugBox( NULL, "i3VertexArrayXbox::Unlock() failed.", Rv);
	}
	#endif
}

// Position
void i3VertexArrayXbox::SetPosition( UINT32	Idx, VEC3D * pVec)
{
	VEC3D * pDestVec;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());
	
	pDestVec = (VEC3D*) (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride));

	*pDestVec = *pVec;
}

void i3VertexArrayXbox::GetPosition( UINT32 Idx, VEC3D * pVec)
{
	VEC3D * pSrcVec;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pSrcVec = (VEC3D*) (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride));

	*pVec = * pSrcVec;
}

// Vertex Color
void i3VertexArrayXbox::SetColor( UINT32 Idx, COLORREAL * pColor)
{
	DWORD * pColorVal;
	I3COLOR	Col;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pColorVal = (DWORD *) (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetColor);

	i3Color::Set( &Col, pColor);

	*pColorVal = Col.GetRGBA();
}

void i3VertexArrayXbox::GetColor( UINT32 Idx, COLORREAL * pColor)
{
	DWORD * pColorVal;
	I3COLOR	Col;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pColorVal = (DWORD *) (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetColor);

	Col.SetRGBA( *pColorVal);

	i3Color::Set( pColor, &Col);
}

// Normal
void i3VertexArrayXbox::SetNormal( UINT32 Idx, VEC3D * pVec)
{
	VEC3D * pDestVec;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pDestVec = (VEC3D *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetNormal);

	*pDestVec = *pVec;
}

void i3VertexArrayXbox::GetNormal( UINT32 Idx, VEC3D * pVec)
{
	VEC3D * pSrcVec;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pSrcVec = (VEC3D *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetNormal);

	*pVec = *pSrcVec;
}

// Texture Coords.
void i3VertexArrayXbox::SetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec)
{
	VEC2D * pDestVec;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pDestVec = (VEC2D *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetTexCoord[which]);

	*pDestVec = *pVec;
}

void i3VertexArrayXbox::GetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec)
{
	VEC2D * pSrcVec;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pSrcVec = (VEC2D *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetTexCoord[which]);

	*pVec = *pSrcVec;
}

// Blend Index
void i3VertexArrayXbox::SetBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex)
{
}

UINT32 i3VertexArrayXbox::GetBlendIndex( UINT32 which, UINT32 Idx)
{
	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	return 0;
}

// Blend Weight
void i3VertexArrayXbox::SetBlendWeight( UINT32 which, UINT32 Idx, REAL32 weight)
{
	REAL32 * pVal;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pVal = (REAL32 *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + 
		(m_OffsetWeight + (sizeof(REAL32) * which)));

	*pVal = weight;
}

REAL32 i3VertexArrayXbox::GetBlendWeight( UINT32 which, UINT32 Idx)
{
	REAL32 * pVal;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	pVal = (REAL32 *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + 
		(m_OffsetWeight + (sizeof(REAL32) * which)));

	return *pVal;
}

// Point-Sprite Size
void i3VertexArrayXbox::SetPointSpriteSize( UINT32 Idx, REAL32 size)
{
	REAL32 * pVal;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	Idx -= m_LockStartIdx;

	pVal = (REAL32 *)(m_pLockedBuf + (Idx * m_Stride) + m_OffsetPointSize);

	*pVal = size;
}

void i3VertexArrayXbox::SetPointSpriteSize( UINT32 Idx, VEC2D * pVec)
{
	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());
}

REAL32 i3VertexArrayXbox::GetPointSpriteSize( UINT32 Idx)
{
	REAL32 * pVal;

	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());

	Idx -= m_LockStartIdx;

	pVal = (REAL32 * )(m_pLockedBuf + (Idx * m_Stride) + m_OffsetPointSize);

	return *pVal;
}

void i3VertexArrayXbox::GetPointSpriteSize( UINT32 Idx, VEC3D * pVec)
{
	I3ASSERT( m_pVertexBuf);
	I3ASSERT( IsLocked());
}

#endif