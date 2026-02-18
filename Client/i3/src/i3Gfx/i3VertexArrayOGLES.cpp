#include "i3GfxType.h"
#include "../i3Math/i3Color.h"

#ifdef I3G_OGLES
#include "i3GfxUtilOGLES.h"
#include "i3VertexArrayOGLES.h"

#if !defined( I3_TOOL)
I3_CONCRETE_CLASS_INSTANCE( i3VertexArrayOGLES, i3VertexArray );
#else
I3_CLASS_INSTANCE( i3VertexArrayOGLES, i3VertexArray );
#endif

#define	_W_			320.0f

i3VertexArrayOGLES::i3VertexArrayOGLES(void)
{
	m_pPos = NULL;
	m_pNormal = NULL;
	m_pColor = NULL;
	m_pTexCoord = NULL;

	m_PosStride = 0;
	m_NormalStride = 0;
	m_ColorStride = 0;
	m_TexCoordStride = 0;
}

i3VertexArrayOGLES::~i3VertexArrayOGLES(void)
{
	if( m_pBuffer != NULL)
	{
		I3MEM_SAFE_FREE( m_pBuffer );		
	}
}

UINT32 i3VertexArrayOGLES::CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count)
{
	UINT32 Sz;

	m_Stride = 0;
	m_PosStride = 0;
	m_NormalStride = 0;
	m_ColorStride = 0;
	m_TexCoordStride = 0;

	// Position (X, Y, Z) Component
	if( pFormat->GetHasPosition())
	{
		m_PosStride = m_ComponentSize[ pFormat->GetPositionType()];

		if( pFormat->IsTransformed())
			m_PosStride *= 2;
		else
			m_PosStride *= 3;
	}

	// Normal Component
	if( pFormat->GetHasNormal())
	{
		m_NormalStride = m_ComponentSize[ pFormat->GetNormalType()];

		m_NormalStride *= 3;
	}

	// Vertex Diffuse Color Component
	if( pFormat->GetHasColor())
	{
		m_ColorStride = m_ComponentSize[ pFormat->GetColorType()];
	}

	// Texture Coord. Set Components
	if( pFormat->GetTextureCoordSetCount() > 0)
	{
		I3ASSERT( pFormat->GetTextureCoordSetCount() == 1);
		I3ASSERT( pFormat->GetTextureCoordCount() == 2);

		m_TexCoordStride = m_ComponentSize[ pFormat->GetTextureCoordType()];
		m_TexCoordStride *= 2;
	}

	m_Stride = m_PosStride + m_NormalStride + m_ColorStride + m_TexCoordStride;

	#if defined( SINGLE_ARRAY)
		m_OffsetPos = 0;		Sz = m_PosStride;
		m_OffsetNormal = Sz;	Sz += m_NormalStride;
		m_OffsetColor = Sz;		Sz += m_ColorStride;
		m_OffsetTexCoord[0] = Sz;

		m_PosStride = m_NormalStride = m_ColorStride = m_TexCoordStride = m_Stride;
	#else
		m_OffsetPos = 0;		Sz = (m_PosStride * Count);
		m_OffsetNormal = Sz;	Sz += (m_NormalStride * Count);
		m_OffsetColor = Sz;		Sz += (m_ColorStride * Count);
		m_OffsetTexCoord[0] = Sz;
	#endif

	return m_Stride * Count;
}


BOOL i3VertexArrayOGLES::Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage)
{
	UINT32 BuffSz;

	m_Format = * pFormat;

	m_Format.SetPositionType( I3VF_TYPE_FIXED32_16);
	m_Format.SetNormalType( I3VF_TYPE_FIXED32_16);
	m_Format.SetTextureCoordType( I3VF_TYPE_FIXED32_16);
	m_Format.SetColorType( I3VF_TYPE_COLOR32_RGBA_8888);

	m_PersistFormat = * pFormat;
    m_Count = count;
	m_Usage = usage;
 
	BuffSz = CalcBufferSize( &m_Format, count);

	m_pBuffer = (char *)i3MemAlign( BuffSz , 4 );	

	I3ASSERT( m_pBuffer != NULL);

	m_pPos = ((INT8 *)m_pBuffer + m_OffsetPos);
	m_pNormal = ((INT8 *) m_pBuffer + m_OffsetNormal);
	m_pColor = ((INT8 *) m_pBuffer + m_OffsetColor);
	m_pTexCoord = ((INT8 *) m_pBuffer + m_OffsetTexCoord[0]);

	return TRUE;
}; 


BOOL i3VertexArrayOGLES::Lock( I3G_LOCKMETHOD lock, UINT32 StartIdx, UINT32 EndIdx)
{
	if( m_bLocked)
		return FALSE;

	I3ASSERT( m_pBuffer);

	m_LockStartIdx = StartIdx;

	m_pLockedBuf = (INT8 *) m_pBuffer;

	m_bLocked = TRUE;

	return TRUE;
}

void i3VertexArrayOGLES::Unlock(void)
{
	if( ! IsLocked())
	{
		return;
	}

	I3ASSERT( m_pBuffer);
	I3ASSERT( m_pLockedBuf);

	m_bLocked = FALSE;
}

#if !defined( SINGLE_ARRAY)
void	i3VertexArrayOGLES::SetPosition( UINT32	Idx, RT_VEC3D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( (Idx) < m_Count);
	
	type = m_Format.GetPositionType();
	Sz = m_ComponentSize[ type];

	pDest = m_pPos + ((Idx) * m_PosStride);

	{
		_REAL_R2F( type, pDest, i3Vector::GetX( pVec));		pDest += Sz;
		_REAL_R2F( type, pDest, i3Vector::GetY( pVec));		pDest += Sz;
		//_REAL_R2F( type, pDest, i3Vector::GetZ( pVec));

		if( GetFormat()->IsTransformed() == FALSE)
		{
			_REAL_R2F( type, pDest, i3Vector::GetZ( pVec));

		}
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::GetPosition( UINT32 Idx, RT_VEC3D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( (Idx) < m_Count);
	
	type = m_Format.GetPositionType();
	Sz = m_ComponentSize[ type];

	pSrc = m_pPos + (m_PosStride * (Idx));

	i3Vector::Set( pVec,	_REAL_F2R( type, pSrc),
							_REAL_F2R( type, pSrc + Sz),
							_REAL_F2R( type, pSrc + (Sz << 1)));
}

void	i3VertexArrayOGLES::SetPositions( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pPos, UINT32 pitch)
{
	INT8 * pDest;
	UINT32 Sz, i;
	I3VF_TYPES type;
	RT_VEC3D * pVec;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( (StartIdx + Count - 1) < m_Count);
	
	type = m_Format.GetPositionType();
	Sz = m_ComponentSize[ type];

	pDest = m_pPos + (StartIdx * m_PosStride);

	pVec = pPos;

	for( i = 0; i < Count; i++)
	{
		_REAL_R2F( type, pDest, i3Vector::GetX( pVec));
		_REAL_R2F( type, pDest + Sz, i3Vector::GetY( pVec));

		if( m_Format.IsTransformed() == FALSE)
		{
			_REAL_R2F( type, pDest + (Sz << 1), i3Vector::GetZ( pVec));
		}
		
		pDest += m_PosStride;
		pVec = (RT_VEC3D *)(((char *)pVec) + pitch);
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::SetColor( UINT32 Idx, COLORREAL * pColor)
{
	INT8 * pDest;
	I3COLOR	Col;
	I3VF_TYPES type;
	UINT32 Sz;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT(  Idx < m_Count);

	type = m_Format.GetColorType();
	Sz = m_ComponentSize[type];

	pDest = m_pColor + (Sz * Idx);

	i3Color::Set( &Col, pColor);

	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
	{
		_COLOR_R2F( type, pDest, Col.GetRGBA());
	}
	else
	{
		_COLOR_R2F( type, pDest, Col.GetARGB());
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::GetColor( UINT32 Idx, COLORREAL * pColor)
{
	INT8 * pSrc;
	I3COLOR	Col;
	UINT32	val, Sz;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx < m_Count);

	Sz = m_ComponentSize[ m_Format.GetColorType()];
	pSrc = m_pColor + (Sz * Idx);

	val = _COLOR_F2R( m_Format.GetColorType(), pSrc);

	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
		Col.SetRGBA( val);
	else
		Col.SetARGB( val);

	i3Color::Set( pColor, &Col);
}

void	i3VertexArrayOGLES::SetColors( UINT32 StartIdx, UINT32 Count, COLOR * pColor, UINT32 pitch)
{
	UINT32 i, Sz;
	INT8 * pTemp;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( (StartIdx + Count -1) < m_Count);

	type = m_Format.GetColorType();
	Sz = m_ComponentSize[type];

	pTemp = m_pColor + (Sz *  StartIdx);

	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
	{
		for( i = 0; i < Count; i++)
		{
			_COLOR_R2F( type, pTemp, pColor->GetRGBA());

			pColor = (COLOR *)((INT8 *)pColor + pitch);
			pTemp += Sz;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_COLOR_R2F( type, pTemp, pColor->GetARGB());

			pColor = (COLOR *)((INT8 *)pColor + pitch);
			pTemp += Sz;
		}
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::SetColor( UINT32 Idx, UINT32 Count, COLOR * pColor)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 col, Sz;
	I3VF_TYPES type;

	I3ASSERT( (Idx + Count - 1) < m_Count);
	
	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
		col = pColor->GetRGBA();
	else
		col = pColor->GetARGB();

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());

	type = m_Format.GetColorType();
	Sz = m_ComponentSize[type];

	pTemp = m_pColor + (Sz * Idx);

	for( i = 0; i < Count; i++)
	{
		_COLOR_R2F( type, pTemp, col);

		pTemp += Sz;
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::SetNormal( UINT32 Idx, RT_VEC3D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( (Idx) < m_Count);
	
	type = m_Format.GetNormalType();
	Sz = m_ComponentSize[ type];

	pDest = m_pNormal + (Sz * (Idx) * 3);

	{
		_REAL_R2F( type, pDest, i3Vector::GetX( pVec));		pDest += Sz;
		_REAL_R2F( type, pDest, i3Vector::GetY( pVec));		pDest += Sz;
		_REAL_R2F( type, pDest, i3Vector::GetZ( pVec));
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::GetNormal( UINT32 Idx, RT_VEC3D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( (Idx) < m_Count);
	
	type = m_Format.GetNormalType();
	Sz = m_ComponentSize[ type];

	pSrc = m_pNormal + (Sz * (Idx) * 3);

	i3Vector::Set( pVec,	_REAL_F2R( type, pSrc),
							_REAL_F2R( type, pSrc + Sz),
							_REAL_F2R( type, pSrc + (Sz << 1)));
}

void	i3VertexArrayOGLES::SetNormals( UINT32 StartIdx, UINT32 Count, RT_VEC3D * pNormal, UINT32 pitch)
{
	INT8 * pDest;
	UINT32 Sz, i;
	I3VF_TYPES type;
	RT_VEC3D * pVec;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( (StartIdx + Count - 1) < m_Count);
	
	type = m_Format.GetNormalType();
	Sz = m_ComponentSize[ type];

	pDest = m_pNormal + (Sz * (StartIdx) * 3);

	pVec = pNormal;

	for( i = 0; i < Count; i++)
	{
		_REAL_R2F( type, pDest, i3Vector::GetX( pVec));		pDest += Sz;
		_REAL_R2F( type, pDest, i3Vector::GetY( pVec));		pDest += Sz;
		_REAL_R2F( type, pDest, i3Vector::GetZ( pVec));		pDest += Sz;

		pVec = (RT_VEC3D *)(((char *)pVec) + pitch);
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::SetTextureCoord( UINT32 which, UINT32 Idx, RT_VEC2D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( which == 0);
	I3ASSERT( (Idx) < m_Count);

	type = m_Format.GetTextureCoordType();
	Sz = m_ComponentSize[ type];

	pDest = m_pTexCoord + (Sz * 2 * (Idx));

	{
		_REAL_R2F( type, pDest, i3Vector::GetU( pVec));		pDest += Sz;
		_REAL_R2F( type, pDest, i3Vector::GetV( pVec));
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::GetTextureCoord( UINT32 which, UINT32 Idx, RT_VEC2D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( which == 0);
	I3ASSERT( (Idx) < m_Count);

	type = m_Format.GetTextureCoordType();
	Sz = m_ComponentSize[ type];

	pSrc = m_pTexCoord + (Sz * 2 * (Idx));

	i3Vector::SetU( pVec, _REAL_F2R( type, pSrc));			pSrc += Sz;
	i3Vector::SetV( pVec, _REAL_F2R( type, pSrc));
}

void	i3VertexArrayOGLES::SetTextureCoords( UINT32 which, UINT32 StartIdx, UINT32 Count, RT_VEC2D * pUV, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( which == 0);
	I3ASSERT( (StartIdx + Count - 1) < m_Count);

	type = m_Format.GetTextureCoordType();
	Sz = m_ComponentSize[ type];

	pTemp = m_pTexCoord + (Sz * 2 * (StartIdx));

	{
		for( i = 0; i < Count; i++)
		{
			_REAL_R2F( type, pTemp, i3Vector::GetU( pUV));
			_REAL_R2F( type, pTemp + Sz, i3Vector::GetV( pUV));

			pUV = (RT_VEC2D *)(((INT8 *) pUV) + pitch);
			pTemp += (Sz << 1);
		}
	}

	m_bModified = TRUE;
}

void	i3VertexArrayOGLES::SetBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex)
{
}

UINT32	i3VertexArrayOGLES::GetBlendIndex( UINT32 which, UINT32 Idx)
{
	return 0;
}

void	i3VertexArrayOGLES::SetBlendIndices( UINT32 which, UINT32 StartIdx, UINT32 Count, UINT8 * pIndices, UINT32 pitch)
{
}

void	i3VertexArrayOGLES::SetPointSpriteSize( UINT32 Idx, RT_REAL32 size)
{
}

void	i3VertexArrayOGLES::SetPointSpriteSize( UINT32 Idx, RT_VEC2D * pVec)
{
}

RT_REAL32	i3VertexArrayOGLES::GetPointSpriteSize( UINT32 Idx)
{
	return __RT_0;
}

void	i3VertexArrayOGLES::GetPointSpriteSize( UINT32 Idx, RT_VEC3D * pVec)
{
}

void	i3VertexArrayOGLES::SetPointSpriteSizes( UINT32 StartIdx, UINT32 Count, RT_VEC2D * pVec, UINT32 pitch)
{
}
#endif

void i3VertexArrayOGLES::MakeRuntimeFormat( I3_PERSIST_VERTEX_OFFSET * pInfo)
{
#if defined( SINGLE_ARRAY)
	i3VertexArray::MakeRuntimeFormat( pInfo);
#else
	INT32 i;

	//if( m_PersistFormat == m_Format)
	//	return;
	/////////////////////////////////
	// Persist Format을 Runtime Format으로 변환한다. 
	//
	{
		INT8 * pSrc, * pDest;
	
		pSrc = (INT8 *) m_pPersistData;			// 원본 vertex data pointer

		Lock();

		for( i = 0; i < (INT32) GetCount(); i++) 
		{
			// Position
			if( m_Format.GetHasPosition())
			{
				RT_REAL32 x, y, z;
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetPositionType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetPositionType()];

				x = _REAL_F2R( m_PersistFormat.GetPositionType(), pSrc + pInfo->m_OffsetPos);
				y = _REAL_F2R( m_PersistFormat.GetPositionType(), pSrc + pInfo->m_OffsetPos + ssz);
				z = _REAL_F2R( m_PersistFormat.GetPositionType(), pSrc + pInfo->m_OffsetPos + (ssz << 1));

				pDest = m_pPos + (dsz * 3 * i);

				_REAL_R2F( m_Format.GetPositionType(), pDest, x);
				_REAL_R2F( m_Format.GetPositionType(), pDest + dsz, y);
				_REAL_R2F( m_Format.GetPositionType(), pDest + (dsz << 1), z);
			}

			// Normal
			if( m_Format.GetHasNormal())
			{
				RT_REAL32 x, y, z;
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetNormalType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetNormalType()];

				x = _REAL_F2R( m_PersistFormat.GetNormalType(), pSrc + pInfo->m_OffsetNormal);
				y = _REAL_F2R( m_PersistFormat.GetNormalType(), pSrc + pInfo->m_OffsetNormal + ssz);
				z = _REAL_F2R( m_PersistFormat.GetNormalType(), pSrc + pInfo->m_OffsetNormal + (ssz << 1));

				pDest = m_pNormal + (dsz * 3 * i);

				_REAL_R2F( m_Format.GetNormalType(), pDest, x);
				_REAL_R2F( m_Format.GetNormalType(), pDest + dsz, y);
				_REAL_R2F( m_Format.GetNormalType(), pDest + (dsz << 1), z);
			}

			// Color
			if( m_Format.GetHasColor())
			{
				UINT32 col;
				UINT32 dsz = m_ComponentSize[ m_Format.GetColorType()];

				col = _COLOR_F2R( m_PersistFormat.GetColorType(), pSrc + pInfo->m_OffsetColor);

				if( m_ColorOrder == I3VF_COLORORDER_ARGB)
				{
					I3COLOR c;

					c.SetRGBA( col);
					col = c.GetARGB();
				}
				
				pDest = m_pColor + (dsz * i);

				_COLOR_R2F( m_Format.GetColorType(), pDest, col);
			}

			// Texture Coords.
			if( m_Format.GetTextureCoordSetCount() > 0)
			{
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetTextureCoordType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetTextureCoordType()];
				RT_REAL32 u, v;

				pDest = m_pTexCoord + (dsz * 2 * i);

				u = _REAL_F2R( m_PersistFormat.GetTextureCoordType(), pSrc + pInfo->m_OffsetTexCoord[0]);
				v = _REAL_F2R( m_PersistFormat.GetTextureCoordType(), pSrc + pInfo->m_OffsetTexCoord[0] + ssz);
				
				_REAL_R2F( m_Format.GetTextureCoordType(), pDest, u);
				_REAL_R2F( m_Format.GetTextureCoordType(), pDest + dsz, v);
			}

			pSrc += pInfo->m_Stride;
		}

		Unlock();
	}
#endif

	// Persist Data를 제거한다.
	//I3MEM_SAFE_FREE( m_pPersistData);
	if(m_pPersistData != NULL)
	{
		I3MEM_SAFE_FREE( m_pPersistData ); 	
	}

}

#endif
