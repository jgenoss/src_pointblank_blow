#include "i3GfxType.h"
#include "i3GfxUtil.h"
#include "i3VertexArray.h"
#include "i3GfxGlobalVariable.h"

#if defined( I3G_DX)
#include "i3VertexArrayDX.h"
#endif

I3_CLASS_INSTANCE( i3VertexArray);

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

INT32 i3VertexArray::m_ComponentSize[] =
{
	0,
	4,	/// I3VF_TYPE_REAL32					1		
	4,	/// I3VF_TYPE_FIXED32_16				2
	2,	/// I3VF_TYPE_FIXED16_8					3
	2,	/// I3VF_TYPE_FIXED16_12				4
	2,	/// I3VF_TYPE_FIXED16_15				5	
	1,	/// I3VF_TYPE_FIXED8_4					6
	1,	/// I3VF_TYPE_FIXED8_7					7
	4,	/// I3VF_TYPE_COLOR32_RGBA_8888			8
	2,	/// I3VF_TYPE_COLOR16_RGBA_1555			9
	2,	/// I3VF_TYPE_COLOR16_RGBX_565			10
	2,	/// I3VF_TYPE_COLOR16_RGBA_4444			11
	2,	/// I3VF_TYPE_FIXED16_0					12
	1,	/// I3VF_TYPE_FIXED8_0					13
	2,	/// I3VF_TYPE_FIXED16_15_U				14
	1,	/// I3VF_TYPE_FIXED8_7_U				15
};

namespace pack
{
	struct PACKED i3VertexArrayInfo1_0
	{
		UINT32		m_Format = 0;
		UINT32		m_Count = 0;
		UINT32		m_PersistantUsage = 0;
		UINT32		m_Types = 0;
	};

	struct PACKED i3VertexArrayInfo2_0
	{
		UINT8		m_ID[4] = { 'V', 'A', '2', '0' };		// VA20
		UINT32		m_Format = 0;
		UINT32		m_Count = 0;
		UINT32		m_PersistantUsage = 0;
		UINT32		m_Types = 0;
	};

	struct PACKED i3VertexArrayInfo3_0
	{
		UINT8			m_ID[4] = { 'V', 'A', '3', '0' };
		UINT32			m_Format = 0;
		UINT32			m_Count = 0;
		UINT32			m_PersistantUsage = 0;
		UINT32			m_Types = 0;

		UINT32			m_State = 0;
		i3::pack::VEC3D	m_Center;
		REAL32			m_Radius = 0.0f;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

bool i3VertexArray::Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage)
{
	m_Format = *pFormat;
	m_PersistFormat = *pFormat;
	m_Count = count;
	m_Usage = usage;

	m_Stride = 0;

	m_OffsetPos = 0;
	m_OffsetRHW = 0;
	m_OffsetWeight = 0;
	m_OffsetNormal = 0;
	m_OffsetColor = 0;
	m_OffsetTexCoord[0] = 0;
	m_OffsetTangent = 0;
	m_OffsetBinormal = 0;

	m_State = 0;


	return true;
};

/////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Array (for all platform)

#if defined( I3_DEBUG)
#define I3CS_ERR( rv, a, b)	\
	if( rv == STREAM_ERR)\
	{\
		i3Log( a, b);\
		Unlock();\
		return STREAM_ERR;\
	}\
	Result += rv;
#else
#define I3CS_ERR( rv, a, b)					(Result += rv)
#endif

i3VertexArray::i3VertexArray(void)
{
	m_ResType = I3G_RESOURCE_TYPE_VERTEXARRAY;

#if defined( I3G_DX ) || defined( I3G_XBOX )
	m_ColorOrder = I3VF_COLORORDER_ARGB;
#elif defined( I3G_PSP )
	m_ColorOrder = I3VF_COLORORDER_RGBA;
#elif defined( I3G_OGLES )
	m_ColorOrder = I3VF_COLORORDER_RGBA;
#endif
}

i3VertexArray::~i3VertexArray(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pPersistData );
}

// Position
void i3VertexArray::SetPosition( UINT32	Idx, VEC3D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetPositionType();
	Sz = m_ComponentSize[ type];
	pDest = (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetPos);

	{
		_REAL_R2F( type, pDest, i3Vector::GetX( pVec));
		_REAL_R2F( type, pDest + Sz, i3Vector::GetY( pVec));
		_REAL_R2F( type, pDest + (Sz << 1), i3Vector::GetZ( pVec));
	}

	m_bModified = true;
}

void i3VertexArray::GetPosition( UINT32 Idx, VEC3D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetPositionType();
	Sz = m_ComponentSize[ type];
	pSrc = (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetPos);

	i3Vector::SetX( pVec, _REAL_F2R( type, pSrc));
	i3Vector::SetY( pVec, _REAL_F2R( type, pSrc + Sz));
	i3Vector::SetZ( pVec, _REAL_F2R( type, pSrc + (Sz << 1)));
}

void i3VertexArray::GetPositions( UINT32 Idx, INT32 cnt, VEC3D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;
	INT32 i;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (Idx + cnt) <= (m_LockStartIdx + m_LockCount));

	type = m_Format.GetPositionType();
	Sz = m_ComponentSize[ type];
	pSrc = (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetPos);

	for( i = 0; i < cnt; i++)
	{
		i3Vector::SetX( pVec, _REAL_F2R( type, pSrc));
		i3Vector::SetY( pVec, _REAL_F2R( type, pSrc + Sz));
		i3Vector::SetZ( pVec, _REAL_F2R( type, pSrc + (Sz << 1)));

		pSrc += m_Stride;
		pVec++;
	}
}

void i3VertexArray::SetPositions( UINT32 StartIdx, UINT32 Count, VEC3D * pPos, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));

	type = m_Format.GetPositionType();
	Sz = m_ComponentSize[ type];

	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride + m_OffsetPos);

	if( type == I3VF_TYPE_REAL32)
	{
		for( i = 0; i < Count; i++)
		{
			*((REAL32*)pTemp) = i3Vector::GetX( pPos);
			*((REAL32*)(pTemp + Sz)) = i3Vector::GetY( pPos);
			*((REAL32*)(pTemp + (Sz << 1))) = i3Vector::GetZ( pPos);

			pPos = (VEC3D*)(((INT8 *)pPos) + pitch);
			pTemp += m_Stride;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_REAL_R2F( type, pTemp, i3Vector::GetX( pPos));
			_REAL_R2F( type, pTemp + (Sz), i3Vector::GetY( pPos));
			_REAL_R2F( type, pTemp + (Sz << 1), i3Vector::GetZ( pPos));

			pPos = (VEC3D *)(((INT8 *)pPos) + pitch);
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

bool i3VertexArray::IsSamePosition( UINT32 Idx, VEC3D * pVec)
{
	VEC3D vSrcPos;
	GetPosition( Idx, &vSrcPos);

	return i3Vector::isAlike( &vSrcPos, pVec);
}

// Vertex Color
void i3VertexArray::SetColor( UINT32 Idx, COLORREAL * pColor)
{
	INT8 * pDest;
	I3COLOR	Col;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetColorType();
	pDest = (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetColor);

	i3Color::Set( &Col, pColor);

	//i3Color::Set( &Col, 1, 2, 3, 4);

	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
	{
		_COLOR_R2F( type, pDest, Col.GetRGBA());
	}
	else
	{
		_COLOR_R2F( type, pDest, Col.GetARGB());
	}

	m_bModified = true;
}

void i3VertexArray::GetColor( UINT32 Idx, COLORREAL * pColor)
{
	INT8 * pSrc;
	I3COLOR	Col;
	UINT32	val;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	pSrc = (INT8 *) (m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetColor);

	val = _COLOR_F2R( m_Format.GetColorType(), pSrc);

	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
		Col.SetRGBA( val);
	else
		Col.SetARGB( val);

	i3Color::Set( pColor, &Col);
}

void i3VertexArray::SetColors( UINT32 StartIdx, UINT32 Count, COLOR * pColor, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));

	type = m_Format.GetColorType();
	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetColor;

	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
	{
		for( i = 0; i < Count; i++)
		{
			_COLOR_R2F( type, pTemp, pColor->GetRGBA());

			pColor = (COLOR *)((INT8 *)pColor + pitch);
			pTemp += m_Stride;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_COLOR_R2F( type, pTemp, pColor->GetARGB());

			pColor = (COLOR *)((INT8 *)pColor + pitch);
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

void i3VertexArray::SetColor( UINT32 StartIdx, UINT32 Count, COLOR * pColor)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 col;
	I3VF_TYPES type;
	
	if( m_ColorOrder == I3VF_COLORORDER_RGBA)
		col = pColor->GetRGBA();
	else
		col = pColor->GetARGB();

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));

	type = m_Format.GetColorType();
	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetColor;

	for( i = 0; i < Count; i++)
	{
		_COLOR_R2F( type, pTemp, col);

		pTemp += m_Stride;
	}

	m_bModified = true;
}

void	i3VertexArray::SetColor( UINT32 idx, UINT32 cnt, UINT32 col)
{
	UINT32 i;
	INT8 * pTemp;
	I3VF_TYPES type;
	
	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( idx >= m_LockStartIdx);
	I3ASSERT( idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (idx + cnt) <= (m_LockStartIdx + m_LockCount));

	type = m_Format.GetColorType();
	pTemp = m_pLockedBuf + ((idx - m_LockStartIdx) * m_Stride) + m_OffsetColor;

	if( m_ColorOrder == I3VF_COLORORDER_ARGB)
	{
		UINT32 r, b;

		r = (col & 0x000000FF) << 16;
		b = (col & 0x00FF0000) >> 16;
		col = (col & 0xFF00FF00) | r | b;
	}

	for( i = 0; i < cnt; i++)
	{
		_COLOR_R2F( type, pTemp, col);

		pTemp += m_Stride;
	}

	m_bModified = true;
}

bool i3VertexArray::IsSameColor( UINT32 Idx, COLORREAL * pColor)
{
	COLORREAL SrcColor;
	GetColor( Idx, &SrcColor);

	return i3Color::IsSame( &SrcColor, pColor);
}

// Normal
void i3VertexArray::SetNormal( UINT32 Idx, VEC3D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetNormalType();
	Sz = m_ComponentSize[ type];
	pDest = (INT8 *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetNormal);

	_REAL_R2F( type, pDest, i3Vector::GetX( pVec));
	_REAL_R2F( type, pDest + Sz, i3Vector::GetY( pVec));
	_REAL_R2F( type, pDest + (Sz << 1), i3Vector::GetZ( pVec));

	m_bModified = true;
}

void i3VertexArray::GetNormal( UINT32 Idx, VEC3D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetNormalType();
	Sz = m_ComponentSize[ type];
	pSrc = (INT8 *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetNormal);

	i3Vector::SetX( pVec, _REAL_F2R( type, pSrc));
	i3Vector::SetY( pVec, _REAL_F2R( type, pSrc + Sz));
	i3Vector::SetZ( pVec, _REAL_F2R( type, pSrc + (Sz << 1)));
}

void i3VertexArray::SetNormals( UINT32 StartIdx, UINT32 Count, VEC3D * pNormal, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( IsLocked());
	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));

	type = m_Format.GetNormalType();
	Sz = m_ComponentSize[ type];
	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetNormal;

	if( type == I3VF_TYPE_REAL32)
	{
		for( i = 0; i < Count; i++)
		{
			*((REAL32*)pTemp) = i3Vector::GetX( pNormal);
			*((REAL32*)(pTemp + Sz)) = i3Vector::GetY( pNormal);
			*((REAL32*)(pTemp + (Sz << 1))) = i3Vector::GetZ( pNormal);

			pNormal = (VEC3D *)((INT8 *) pNormal + pitch);
			pTemp += m_Stride;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_REAL_R2F( type, pTemp, i3Vector::GetX( pNormal));
			_REAL_R2F( type, pTemp + Sz, i3Vector::GetY( pNormal));
			_REAL_R2F( type, pTemp + (Sz << 1), i3Vector::GetZ( pNormal));

			pNormal = (VEC3D *)((INT8 *) pNormal + pitch);
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

bool i3VertexArray::IsSameNormal( UINT32 Idx, VEC3D * pVec)
{
	VEC3D vSrcNormal;
	GetNormal( Idx, &vSrcNormal);

	return i3Vector::isAlike( &vSrcNormal, pVec);
}

// Texture Coords.
void i3VertexArray::SetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetTextureCoordSetCount()));

	type = m_Format.GetTextureCoordType();
	Sz = m_ComponentSize[ type];

	pDest = (INT8 *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetTexCoord[which]);

	{
		_REAL_R2F( type, pDest, i3Vector::GetU( pVec));
		_REAL_R2F( type, pDest + Sz, i3Vector::GetV( pVec));
	}

	m_bModified = true;
}

void i3VertexArray::GetTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetTextureCoordSetCount()));

	type = m_Format.GetTextureCoordType();
	Sz = m_ComponentSize[ type];
	pSrc = (INT8 *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetTexCoord[which]);

	i3Vector::SetU( pVec, _REAL_F2R( type, pSrc));
	i3Vector::SetV( pVec, _REAL_F2R( type, pSrc + Sz));
}

void *	i3VertexArray::GetTextureCoordPointer( UINT32 which, UINT32 idx)
{
	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( idx >= m_LockStartIdx);
	I3ASSERT( idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetTextureCoordSetCount()));

	return (void *)(m_pLockedBuf + ((idx - m_LockStartIdx) * m_Stride) + m_OffsetTexCoord[which]);
}

void i3VertexArray::SetTextureCoords( UINT32 which, UINT32 StartIdx, UINT32 Count, VEC2D * pUV, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetTextureCoordSetCount()));

	type = m_Format.GetTextureCoordType();
	Sz = m_ComponentSize[ type];
	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetTexCoord[which];

	if( type == I3VF_TYPE_REAL32)
	{
		for( i = 0; i < Count; i++)
		{
			*((REAL32*)pTemp) = i3Vector::GetU( pUV);
			*((REAL32*)(pTemp + Sz)) = i3Vector::GetV( pUV);

			pUV = (VEC2D *)((INT8 *) pUV + pitch);
			pTemp += m_Stride;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_REAL_R2F( type, pTemp, i3Vector::GetU( pUV));
			_REAL_R2F( type, pTemp + Sz, i3Vector::GetV( pUV));

			pUV = (VEC2D *)((INT8 *) pUV + pitch);
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

bool i3VertexArray::IsSameTextureCoord( UINT32 which, UINT32 Idx, VEC2D * pVec)
{
	VEC2D vSrcTexCoord;
	GetTextureCoord( which, Idx, &vSrcTexCoord);

	return i3Vector::isAlike( &vSrcTexCoord, pVec);
}

// Blend Index
void i3VertexArray::SetBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex)
{
	UINT8 * pVal;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetBlendIndexCount()));

	pVal = (UINT8 *) (m_pLockedBuf +  ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetBlendIndex);

	if( (pCaps == nullptr) || (pCaps->isSupported_UBYTE4()))
	{
		I3ASSERT( BlendIndex <= 0xFF);
		pVal[which] = (UINT8) (BlendIndex & 0xFF);
	}
	else
	{
		REAL32 * pIndex = (REAL32 *) pVal;

		pIndex[ which] = (REAL32) BlendIndex;
	}

	m_bModified = true;
}

UINT32 i3VertexArray::GetBlendIndex( UINT32 which, UINT32 Idx)
{
	UINT8 * pVal;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetBlendIndexCount()));

	if( (pCaps == nullptr) || (pCaps->isSupported_UBYTE4()))
	{
		pVal = (UINT8 *) (m_pLockedBuf +  ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetBlendIndex);

		return (UINT32) pVal[ which];
	}
	else
	{
		REAL32 * pIndex = (REAL32 *) (m_pLockedBuf +  ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetBlendIndex);

		return (UINT32) pIndex[ which];
	}
}

void i3VertexArray::SetBlendIndices( UINT32 which, UINT32 StartIdx, UINT32 Count, UINT8 * pIndices, UINT32 pitch)
{
	UINT32 i;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetBlendIndexCount()));

	if( (pCaps == nullptr) || (pCaps->isSupported_UBYTE4()))
	{
		INT8 * pTemp;

		pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetBlendIndex + which;

		for( i = 0; i < Count; i++)
		{
			*pTemp = *pIndices;

			pIndices += pitch;
			pTemp += m_Stride;
		}
	}
	else
	{
		REAL32 * pTemp;

		pTemp = (REAL32 *) (m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetBlendIndex) + which;

		for( i = 0; i < Count; i++)
		{
			*pTemp = (REAL32) *pIndices;

			pIndices += pitch;
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

bool i3VertexArray::IsSameBlendIndex( UINT32 which, UINT32 Idx, UINT32 BlendIndex)
{
	return ( GetBlendIndex( which, Idx) == BlendIndex);
}

// Blend Weight
void i3VertexArray::SetBlendWeight( UINT32 which, UINT32 Idx, REAL32 weight)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetBlendIndexCount()));

	type = m_Format.GetBlendWeightType();
	Sz = m_ComponentSize[ type];

	I3ASSERT( which < (UINT32) GetFormat()->GetBlendWeightCount());

	{
		// 실제 사용되는 Weight 개수가 n개인 경우, DirectX에서는 n - 1개의 Weight만을 요구한다.
		// 마지막 Weight 값은 1 - sum(w1~w(n-1))로 구할 수 있기 때문이다.
		pDest = (INT8 *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + (m_OffsetWeight + (Sz * which)));

		_REAL_R2F( type, pDest, weight);
	}

	m_bModified = true;
}

REAL32 i3VertexArray::GetBlendWeight( UINT32 which, UINT32 Idx)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetBlendIndexCount()));

	type = m_Format.GetBlendWeightType();
	Sz = m_ComponentSize[ type];

	pSrc = (INT8 *)(m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + (m_OffsetWeight + (Sz * which)));

	return _REAL_F2R( type, pSrc);
}

void i3VertexArray::SetBlendWeights( UINT32 which, UINT32 StartIdx, UINT32 Count, REAL32 * pWeights, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));
	I3ASSERT( (which < (UINT32) GetFormat()->GetBlendIndexCount()));

	type = m_Format.GetBlendWeightType();
	Sz = m_ComponentSize[ type];
	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + (m_OffsetWeight + (Sz * which));

	if( type == I3VF_TYPE_REAL32)
	{
		for( i = 0; i < Count; i++)
		{
			*((REAL32*)pTemp) = *pWeights;
			pWeights = (REAL32 *)((INT8 *) pWeights + pitch);
			pTemp += m_Stride;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_REAL_R2F( type, pTemp, *pWeights);
			pWeights = (REAL32 *)((INT8 *) pWeights + pitch);
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

bool i3VertexArray::IsSameBlendWeight( UINT32 which, UINT32 Idx, REAL32 weight)
{	
	REAL32 thisweight = GetBlendWeight( which, Idx);

	return i3Math::isAlike( thisweight, weight);
}

// Tangent
void i3VertexArray::SetTangent( UINT32 Idx, VEC3D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetTangentType();
	Sz = m_ComponentSize[ type];
	pDest = (INT8 *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetTangent);

	_REAL_R2F( type, pDest, i3Vector::GetX( pVec));
	_REAL_R2F( type, pDest + Sz, i3Vector::GetY( pVec));
	_REAL_R2F( type, pDest + (Sz << 1), i3Vector::GetZ( pVec));

	m_bModified = true;
}

void i3VertexArray::GetTangent( UINT32 Idx, VEC3D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetTangentType();
	Sz = m_ComponentSize[ type];
	pSrc = (INT8 *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetTangent);

	i3Vector::SetX( pVec, _REAL_F2R( type, pSrc));
	i3Vector::SetY( pVec, _REAL_F2R( type, pSrc + Sz));
	i3Vector::SetZ( pVec, _REAL_F2R( type, pSrc + (Sz << 1)));
}

void i3VertexArray::SetTangents( UINT32 StartIdx, UINT32 Count, VEC3D * pTangent, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount));

	type = m_Format.GetTangentType();
	Sz = m_ComponentSize[ type];
	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetTangent;

	if( type == I3VF_TYPE_REAL32)
	{
		for( i = 0; i < Count; i++)
		{
			*((REAL32*)pTemp) = i3Vector::GetX( pTangent);
			*((REAL32*)(pTemp + Sz)) = i3Vector::GetY( pTangent);
			*((REAL32*)(pTemp + (Sz << 1))) = i3Vector::GetZ( pTangent);

			pTangent = (VEC3D *)((INT8 *) pTangent + pitch);
			pTemp += m_Stride;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_REAL_R2F( type, pTemp, i3Vector::GetX( pTangent));
			_REAL_R2F( type, pTemp + Sz, i3Vector::GetY( pTangent));
			_REAL_R2F( type, pTemp + (Sz << 1), i3Vector::GetZ( pTangent));

			pTangent = (VEC3D *)((INT8 *) pTangent + pitch);
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

bool i3VertexArray::IsSameTangent( UINT32 Idx, VEC3D * pVec)
{
	VEC3D vSrcTangent;
	GetTangent( Idx, &vSrcTangent);

	return i3Vector::isAlike( &vSrcTangent, pVec);
}

// Binormal
void i3VertexArray::SetBinormal( UINT32 Idx, VEC3D * pVec)
{
	INT8 * pDest;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetBinormalType();
	Sz = m_ComponentSize[ type];
	pDest = (INT8 *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetBinormal);

	_REAL_R2F( type, pDest, i3Vector::GetX( pVec));
	_REAL_R2F( type, pDest + Sz, i3Vector::GetY( pVec));
	_REAL_R2F( type, pDest + (Sz << 1), i3Vector::GetZ( pVec));

	m_bModified = true;
}

void i3VertexArray::GetBinormal( UINT32 Idx, VEC3D * pVec)
{
	INT8 * pSrc;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( m_pBuffer);
	I3ASSERT( IsLocked());
	I3ASSERT( Idx >= m_LockStartIdx);
	I3ASSERT( Idx < (m_LockStartIdx + m_LockCount));

	type = m_Format.GetBinormalType();
	Sz = m_ComponentSize[ type];
	pSrc = (INT8 *)( m_pLockedBuf + ((Idx - m_LockStartIdx) * m_Stride) + m_OffsetBinormal);

	i3Vector::SetX( pVec, _REAL_F2R( type, pSrc));
	i3Vector::SetY( pVec, _REAL_F2R( type, pSrc + Sz));
	i3Vector::SetZ( pVec, _REAL_F2R( type, pSrc + (Sz << 1)));
}

void i3VertexArray::SetBinormals( UINT32 StartIdx, UINT32 Count, VEC3D * pBinormal, UINT32 pitch)
{
	UINT32 i;
	INT8 * pTemp;
	UINT32 Sz;
	I3VF_TYPES type;

	I3ASSERT( StartIdx >= m_LockStartIdx);
	I3ASSERT( StartIdx < (m_LockStartIdx + m_LockCount));
	I3ASSERT( (StartIdx + Count) <= (m_LockStartIdx + m_LockCount))

	type = m_Format.GetBinormalType();
	Sz = m_ComponentSize[ type];
	pTemp = m_pLockedBuf + ((StartIdx - m_LockStartIdx) * m_Stride) + m_OffsetBinormal;

	if( type == I3VF_TYPE_REAL32)
	{
		for( i = 0; i < Count; i++)
		{
			*((REAL32*)pTemp) = i3Vector::GetX( pBinormal);
			*((REAL32*)(pTemp + Sz)) = i3Vector::GetY( pBinormal);
			*((REAL32*)(pTemp + (Sz << 1))) = i3Vector::GetZ( pBinormal);

			pBinormal = (VEC3D *)((INT8 *) pBinormal + pitch);
			pTemp += m_Stride;
		}
	}
	else
	{
		for( i = 0; i < Count; i++)
		{
			_REAL_R2F( type, pTemp, i3Vector::GetX( pBinormal));
			_REAL_R2F( type, pTemp + Sz, i3Vector::GetY( pBinormal));
			_REAL_R2F( type, pTemp + (Sz << 1), i3Vector::GetZ( pBinormal));

			pBinormal = (VEC3D *)((INT8 *) pBinormal + pitch);
			pTemp += m_Stride;
		}
	}

	m_bModified = true;
}

bool i3VertexArray::IsSameBinormal( UINT32 Idx, VEC3D * pVec)
{
	VEC3D vSrcBinormal;
	GetBinormal( Idx, &vSrcBinormal);

	return i3Vector::isAlike( &vSrcBinormal, pVec);
}

#define		ALIGN_UP( a, b)			(((a) + ((b)- 1)) / (b)) * (b);

UINT32 i3VertexArray::CalcPersistantBufferSizePSP( i3::pack::VERTEX_OFFSET * pInfo, UINT32 count)
{
	UINT32 Sz, AlignUnit = 1;
	I3VF_TYPES	type;
	i3VertexFormat * pFormat = & pInfo->m_Format;

	pInfo->m_Stride = 0;

	// 가장 큰 크기의 Vertex Component를 찾는다.
	// 그 Component의 크기에 맞추어 각 Component들의 align을 맞추어야 한다.
	{
		// Position
		if( pFormat->GetHasPosition())
		{
			type = pFormat->GetPositionType();
			AlignUnit = m_ComponentSize[ type];
		}

		// Normal
		if( pFormat->GetHasNormal())
		{
			type = pFormat->GetNormalType();
			Sz = m_ComponentSize[ type];
			AlignUnit = MAX( Sz, AlignUnit);
		}

		// Texture Coord
		if( pFormat->GetTextureCoordSetCount() > 0)
		{
			type = pFormat->GetTextureCoordType();
			Sz = m_ComponentSize[ type];
			AlignUnit = MAX( Sz, AlignUnit);
		}

		// Weight
		if( pFormat->GetBlendWeightCount() > 0 )
		{
			type = pFormat->GetBlendWeightType();
			Sz = m_ComponentSize[ type];
			AlignUnit = MAX( Sz, AlignUnit);
		}

		// Color
		if( pFormat->GetHasColor())
		{
			type = pFormat->GetColorType();
			Sz = m_ComponentSize[ type];
			AlignUnit = MAX( Sz, AlignUnit);
		}
	}

	// Blend Weight Component
	{
		pInfo->m_OffsetWeight = 0;

		if( pFormat->GetBlendWeightCount() > 0)
		{
			type = pFormat->GetBlendWeightType();
			Sz = m_ComponentSize[ type];
			pInfo->m_Stride += Sz * pFormat->GetBlendWeightCount();
		}
	}

	// Texture Coord. Set Components
	if( pFormat->GetTextureCoordSetCount() > 0)
	{
		UINT32 i;

		#if defined( I3_DEBUG)
		if( pFormat->GetTextureCoordSetCount() > 0)
		{
			I3ASSERT( pFormat->GetTextureCoordSetCount() == 1);
			I3ASSERT( pFormat->GetTextureCoordCount() == 2);
		}
		#endif

		type = pFormat->GetTextureCoordType();
		Sz = m_ComponentSize[ type];

		pInfo->m_Stride = ALIGN_UP( pInfo->m_Stride, Sz);

		for( i = 0; i < (UINT32)pFormat->GetTextureCoordSetCount(); i++)
		{
			pInfo->m_OffsetTexCoord[i] = pInfo->m_Stride;

			pInfo->m_Stride += Sz * pFormat->GetTextureCoordCount();
		}
	}

	// Vertex Diffuse Color Component
	{
		if( pFormat->GetHasColor())
		{
			type = pFormat->GetColorType();
			Sz = m_ComponentSize[ type];

			pInfo->m_Stride = ALIGN_UP( pInfo->m_Stride, Sz);
			pInfo->m_OffsetColor = pInfo->m_Stride;

			pInfo->m_Stride += Sz;
		}
	}

	// Normal Component
	{
		if( pFormat->GetHasNormal())
		{
			type = pFormat->GetNormalType();
			Sz = m_ComponentSize[ type];

			pInfo->m_Stride = ALIGN_UP( pInfo->m_Stride, Sz);
			pInfo->m_OffsetNormal = pInfo->m_Stride;

			pInfo->m_Stride += Sz * 3;
		}
	}

	// Position (X, Y, Z) Component
	{
		if( pFormat->GetHasPosition())
		{
			type = pFormat->GetPositionType();
			Sz = m_ComponentSize[ type];

			pInfo->m_Stride = ALIGN_UP( pInfo->m_Stride, Sz);
			pInfo->m_OffsetPos = pInfo->m_Stride;

			pInfo->m_Stride += Sz * 3;
		}
	}

	pInfo->m_Stride = ALIGN_UP( pInfo->m_Stride, AlignUnit);

	return pInfo->m_Stride * count;
}

UINT32 i3VertexArray::CalcPersistantBufferSize20( i3::pack::VERTEX_OFFSET * pInfo, UINT32 count)
{
	UINT32 Size = 0, i;
	i3VertexFormat * pFormat = & pInfo->m_Format;

	if( pFormat->GetPlatform() == I3VF_PLATFORM_PSP)
	{
		return CalcPersistantBufferSizePSP( pInfo, count);
	}

	// Position
	pInfo->m_OffsetPos = Size;
	if( pFormat->GetHasPosition())
	{
		pInfo->m_SizePos = m_ComponentSize[ (INT32) pFormat->GetPositionType()];
		Size += pInfo->m_SizePos * 3;
	}
	
	// Normal
	pInfo->m_OffsetNormal = Size;
	if( pFormat->GetHasNormal())
	{
		pInfo->m_SizeNormal = m_ComponentSize[ (INT32) pFormat->GetNormalType()];
		Size += pInfo->m_SizeNormal * 3;
	}

	// Color
	pInfo->m_OffsetColor = Size;
	if( pFormat->GetHasColor())
	{
		pInfo->m_SizeColor = m_ComponentSize[ (INT32) pFormat->GetColorType()];
		Size += pInfo->m_SizeColor;
	}

	// Texture Coords.
	for( i = 0; i < (UINT32) pFormat->GetTextureCoordSetCount(); i++)
	{
		pInfo->m_OffsetTexCoord[i] = Size;
		pInfo->m_SizeTexCoord = m_ComponentSize[ (INT32) pFormat->GetTextureCoordType()];

		Size += pInfo->m_SizeTexCoord * pFormat->GetTextureCoordCount();
	}

	// Blend Index
	if( pFormat->GetBlendIndexCount() > 0)
	{
		pInfo->m_OffsetBlendIndex = Size;
		pInfo->m_SizeBlendIndex = 1;
		Size += pFormat->GetBlendIndexCount();
	}

	// Blend Weight
	if( pFormat->GetBlendWeightCount() > 0)
	{
		pInfo->m_OffsetWeight = Size;
		pInfo->m_SizeWeight = m_ComponentSize[ (INT32) pFormat->GetBlendWeightType()];
		Size += pInfo->m_SizeWeight * pFormat->GetBlendWeightCount();				// 마지막 Weight는 저장하지 않는다.
	}

	// Tangent
	pInfo->m_OffsetTangent = Size;
	if( pFormat->GetHasTangent())
	{
		pInfo->m_SizeTangent = m_ComponentSize[ (INT32) pFormat->GetTangentType()];
		Size += pInfo->m_SizeTangent * 3;
	}

	// Binormal
	pInfo->m_OffsetBinormal = Size;
	if( pFormat->GetHasBinormal())
	{
		pInfo->m_SizeBinormal = m_ComponentSize[ (INT32) pFormat->GetBinormalType()];
		Size += pInfo->m_SizeBinormal * 3;
	}

	pInfo->m_Stride = Size;

	return Size * count;
}

UINT32 i3VertexArray::CalcPersistantBufferSize25( i3::pack::VERTEX_OFFSET * pInfo, UINT32 count)
{
	INT32  nPopSize = 4;
	UINT32 Size = 0, i;
	i3VertexFormat * pFormat = & pInfo->m_Format;

	if( pFormat->GetPlatform() == I3VF_PLATFORM_PSP)
	{
		return CalcPersistantBufferSizePSP( pInfo, count);
	}

	// Position
	pInfo->m_OffsetPos = Size;
	if( pFormat->GetHasPosition())
	{
		pInfo->m_SizePos = m_ComponentSize[ (INT32) pFormat->GetPositionType()];
		Size += pInfo->m_SizePos * 3;
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}
	
	
	// Normal
	pInfo->m_OffsetNormal = Size;
	if( pFormat->GetHasNormal())
	{
		pInfo->m_SizeNormal = m_ComponentSize[ (INT32) pFormat->GetNormalType()];
		Size += pInfo->m_SizeNormal * 3;
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}

	// Color
	pInfo->m_OffsetColor = Size;
	if( pFormat->GetHasColor())
	{
		pInfo->m_SizeColor = m_ComponentSize[ (INT32) pFormat->GetColorType()];
		Size += pInfo->m_SizeColor;
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}

	// Texture Coords.
	for( i = 0; i < (UINT32) pFormat->GetTextureCoordSetCount(); i++)
	{
		pInfo->m_OffsetTexCoord[i] = Size;
		pInfo->m_SizeTexCoord = m_ComponentSize[ (INT32) pFormat->GetTextureCoordType()];

		Size += pInfo->m_SizeTexCoord * pFormat->GetTextureCoordCount();
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}

	// Blend Index
	if( pFormat->GetBlendIndexCount() > 0)
	{
		pInfo->m_OffsetBlendIndex = Size;
		pInfo->m_SizeBlendIndex = 1;
		Size += pFormat->GetBlendIndexCount();
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}

	// Blend Weight
	if( pFormat->GetBlendWeightCount() > 0)
	{
		pInfo->m_OffsetWeight = Size;
		pInfo->m_SizeWeight = m_ComponentSize[ (INT32) pFormat->GetBlendWeightType()];
		Size += pInfo->m_SizeWeight * pFormat->GetBlendWeightCount();				// 마지막 Weight는 저장하지 않는다.
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}

	// Tangent
	pInfo->m_OffsetTangent = Size;
	if( pFormat->GetHasTangent())
	{
		pInfo->m_SizeTangent = m_ComponentSize[ (INT32) pFormat->GetTangentType()];
		Size += pInfo->m_SizeTangent * 3;
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}

	// Binormal
	pInfo->m_OffsetBinormal = Size;
	if( pFormat->GetHasBinormal())
	{
		pInfo->m_SizeBinormal = m_ComponentSize[ (INT32) pFormat->GetBinormalType()];
		Size += pInfo->m_SizeBinormal * 3;
		(Size%nPopSize) > 0 ? Size += nPopSize - (Size%nPopSize) : 0;
	}

	pInfo->m_Stride = Size;

	return Size * count;
}

UINT32 i3VertexArray::OnSave( i3ResourceFile * pResFile)
{
	pack::i3VertexArrayInfo3_0 header;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 Rc, Result = 0;
	UINT32 i, j, ssz, dsz;
	INT8 * pSrc;
	i3::pack::VERTEX_OFFSET Info;
	REAL32 x;
	VEC3D vec;
	INT8	tempBuf[16];

	header.m_Format = m_PersistFormat.GetFormat();
	header.m_Count = m_Count;
	header.m_PersistantUsage = i3Gfx::Convert( m_Usage);
	header.m_Types = m_PersistFormat.GetTypes();
	header.m_Center.x = m_Center.x;
	header.m_Center.y = m_Center.y;
	header.m_Center.z = m_Center.z;
	header.m_Radius = m_Rad;
	header.m_State = m_State;

	Rc = pStream->Write( &header, sizeof(pack::i3VertexArrayInfo3_0));
	I3CS_ERR( Rc, "i3VertexArray::OnSave()", "Could not write vertex array information.");

	if( m_bModified == true)
	{
		// Runtime에서 변경되었기 때문에 Persist Data가 있다고 하더라도 무시한다.
		//I3MEM_SAFE_FREE( m_pPersistData, m_PersistDataIdx );
		I3MEM_SAFE_FREE_POINTER( m_pPersistData ); 
	}
	
	if( (m_pPersistData != nullptr) && ((m_PersistFormat != m_Format) || (m_PersistFormat.GetPlatform() != I3VF_PLATFORM_NA)))
	{
		Info.m_Format = m_PersistFormat;

		ssz = CalcPersistantBufferSize25( &Info, 1);

		Rc = pStream->Write( m_pPersistData, ssz * GetCount());
		I3CS_ERR( Rc, "i3VertexArray::OnSave", "Could not write persistent data.");
	}
	else
	{
		INT32 nPopSize = 4;

		Lock();		

		pSrc = GetLockedBuffer();

		OnPreSave( pSrc);

		for( i = 0; i < m_Count; i++)
		{
			// Position
			if( m_PersistFormat.GetHasPosition())
			{
				//ssz = m_ComponentSize[ (INT32) m_Format.GetPositionType()];
				dsz = m_ComponentSize[ (INT32) m_PersistFormat.GetPositionType()];

				GetPosition( i, &vec);

				_REAL_R2F( m_PersistFormat.GetPositionType(), tempBuf, vec.x);
				_REAL_R2F( m_PersistFormat.GetPositionType(), tempBuf + dsz, vec.y);
				_REAL_R2F( m_PersistFormat.GetPositionType(), tempBuf + (dsz << 1), vec.z);

				UINT32 dszTemp = dsz * 3;
				(dszTemp%nPopSize) > 0 ? dszTemp += nPopSize - ( dszTemp % nPopSize ) : 0;
				Rc = pStream->Write( tempBuf, dszTemp );
				I3CS_ERR( Rc, "i3VertexArray::OnSave", "Could not write position data.");
			}

			// Normal
			if( m_PersistFormat.GetHasNormal())
			{
				//ssz = m_ComponentSize[ (INT32) m_Format.GetNormalType()];
				dsz = m_ComponentSize[ (INT32) m_PersistFormat.GetNormalType()];

				GetNormal( i, &vec);

				_REAL_R2F( m_PersistFormat.GetNormalType(), tempBuf, vec.x);
				_REAL_R2F( m_PersistFormat.GetNormalType(), tempBuf + dsz, vec.y);
				_REAL_R2F( m_PersistFormat.GetNormalType(), tempBuf + (dsz << 1), vec.z);

				UINT32 dszTemp = dsz * 3;
				(dszTemp%nPopSize) > 0 ? dszTemp += nPopSize - ( dszTemp % nPopSize ) : 0;
				Rc = pStream->Write( tempBuf, dszTemp );

				I3CS_ERR( Rc, "i3VertexArray::OnSave", "Could not write normal data.");
			}

			// Vertex Color
			if( m_PersistFormat.GetHasColor())
			{
				UINT32 col;
				COLORREAL rcol;
				I3COLOR c;

				dsz = m_ComponentSize[ (INT32) m_PersistFormat.GetColorType()];

				GetColor( i, &rcol);

				i3Color::Set( &c, &rcol);

				col = c.GetRGBA();

				_COLOR_R2F( m_PersistFormat.GetColorType(), tempBuf, col);

				(dsz%nPopSize) > 0 ? dsz += nPopSize - ( dsz % nPopSize ) : 0;

				Rc = pStream->Write( tempBuf, dsz);
				I3CS_ERR( Rc, "i3VertexArray::OnSave()", "Could not vertex color data.");
			}

			// Texture Coords
			for( j = 0; j < (UINT32) m_PersistFormat.GetTextureCoordSetCount(); j++)
			{
				//ssz = m_ComponentSize[ (INT32) m_Format.GetTextureCoordType()];
				dsz = m_ComponentSize[ (INT32) m_PersistFormat.GetTextureCoordType()];

				GetTextureCoord( j, i, (VEC2D *) &vec);

				REAL32 *pUV = (REAL32 *) &vec;

				for( INT32 k = 0; k < m_PersistFormat.GetTextureCoordCount(); k++, pUV++)
				{
					_REAL_R2F( m_PersistFormat.GetTextureCoordType(), tempBuf, *pUV);

					(dsz%nPopSize) > 0 ? dsz += nPopSize - ( dsz % nPopSize ) : 0;

					Rc = pStream->Write( tempBuf, dsz);
					I3CS_ERR( Rc, "i3VertexArray::OnSave()", "Could not write %dth texture cooordinates data.");
				}
			}

			// Blend Index
			if( m_PersistFormat.GetBlendIndexCount() > 0)
			{
				ssz = m_Format.GetBlendIndexCount();

				(ssz%nPopSize) > 0 ? ssz += nPopSize - ( ssz % nPopSize ) : 0;

				Rc = pStream->Write( pSrc + m_OffsetBlendIndex, ssz);
				I3CS_ERR( Rc, "i3VertexArray::OnSave()", "Could not write blend index data.");
			}

			// Blend Weight
			if( m_PersistFormat.GetBlendWeightCount() > 0)
			{
				ssz = m_ComponentSize[ (INT32) m_Format.GetBlendWeightType()];
				dsz = m_ComponentSize[ (INT32) m_PersistFormat.GetBlendWeightType()];

				for( j = 0; j < (UINT32) m_PersistFormat.GetBlendWeightCount(); j++)
				{
					x = _REAL_F2R( m_Format.GetBlendWeightType(), pSrc + m_OffsetWeight + (j * ssz));

					_REAL_R2F( m_PersistFormat.GetBlendWeightType(), tempBuf, x);

					(dsz%nPopSize) > 0 ? dsz += nPopSize - ( dsz % nPopSize ) : 0;

					Rc = pStream->Write( tempBuf, dsz);
					I3CS_ERR( Rc, "i3VertexArray::OnSave()", "Could not write blend weight data.");
				}
			}

			// Tangent
			if( m_PersistFormat.GetHasTangent())
			{
				//ssz = m_ComponentSize[ (INT32) m_Format.GetTangentType()];
				dsz = m_ComponentSize[ (INT32) m_PersistFormat.GetTangentType()];

				GetTangent( i, &vec);

				_REAL_R2F( m_PersistFormat.GetTangentType(), tempBuf, vec.x);
				_REAL_R2F( m_PersistFormat.GetTangentType(), tempBuf + dsz, vec.y);
				_REAL_R2F( m_PersistFormat.GetTangentType(), tempBuf + (dsz << 1), vec.z);


				UINT32 dszTemp = dsz * 3;
				(dszTemp%nPopSize) > 0 ? dszTemp += nPopSize - ( dszTemp % nPopSize ) : 0;

				Rc = pStream->Write( tempBuf, dszTemp);
				I3CS_ERR( Rc, "i3VertexArray::OnSave", "Could not write tangent data.");
			}

			// Binormal
			if( m_PersistFormat.GetHasBinormal())
			{
				//ssz = m_ComponentSize[ (INT32) m_Format.GetBinormalType()];
				dsz = m_ComponentSize[ (INT32) m_PersistFormat.GetBinormalType()];

				GetBinormal( i, &vec);

				_REAL_R2F( m_PersistFormat.GetBinormalType(), tempBuf, vec.x);
				_REAL_R2F( m_PersistFormat.GetBinormalType(), tempBuf + dsz, vec.y);
				_REAL_R2F( m_PersistFormat.GetBinormalType(), tempBuf + (dsz << 1), vec.z);

				UINT32 dszTemp = dsz * 3;
				(dszTemp%nPopSize) > 0 ? dszTemp += nPopSize - ( dszTemp % nPopSize ) : 0;

				Rc = pStream->Write( tempBuf, dszTemp );
				I3CS_ERR( Rc, "i3VertexArray::OnSave", "Could not write binormal data.");
			}

			pSrc += m_Stride;
		}

		OnPostSave( GetLockedBuffer());

		Unlock();
	}

	return Result;
}

UINT32 i3VertexArray::OnLoad10( i3ResourceFile * pResFile)
{
	i3VertexFormat format;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i, j;
	UINT32 Rc, Result = 0;
	pack::i3VertexArrayInfo1_0 header;
	i3VertexFormat vf;

	setRestorePoint( pResFile->GetStream());

	Rc = pStream->Read( &header, sizeof( pack::i3VertexArrayInfo1_0));
	I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read vertex information header.");

	format.SetFormat( header.m_Format);
	format.SetTypes( vf.GetTypes());

	if( Create( &format, header.m_Count, i3Gfx::Convert( header.m_PersistantUsage)) == false)
	{
		i3Log( "i3VertexArray::OnLoad()", "Could not create vertex array.");
		return STREAM_ERR;
	}

	Lock();

	// Position
	if( m_Format.GetHasPosition())
	{
		for( i = 0; i < m_Count; i++)
		{
			i3::pack::VEC3D vec;
			VEC3D pos;

			Rc = pStream->Read( &vec, sizeof(i3::pack::VEC3D));
			I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read position data.");

			i3Vector::Copy( &pos, &vec);

			SetPosition( i, &pos);
		}
	}

	// Normal
	if( m_Format.GetHasNormal())
	{
		for( i = 0; i < m_Count; i++)
		{
			i3::pack::VEC3D vec;
			VEC3D norm;

			Rc = pStream->Read( &vec, sizeof(i3::pack::VEC3D));
			I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read normal data.");

			i3Vector::Copy( &norm, &vec);

			SetNormal( i, &norm);
		}
	}

	// Color
	if( m_Format.GetHasColor())
	{
		for( i = 0; i < m_Count; i++)
		{
			UINT32 col;
			I3COLOR color;
			COLORREAL	colorreal;

			Rc = pStream->Read( &col, sizeof(UINT32));
			I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read vertex color data.");

			color.SetRGBA( col);
			i3Color::Set( &colorreal, &color);

			SetColor( i, &colorreal);
		}
	}

	// Texture Coords.
	for( i = 0; i < (UINT32) m_Format.GetTextureCoordSetCount(); i++)
	{
		for( j = 0; j < m_Count; j++)
		{
			switch( m_Format.GetTextureCoordCount())
			{
				case 2 :
					{
						i3::pack::VEC2D vec;
						VEC2D uv;

						Rc = pStream->Read( &vec, sizeof(i3::pack::VEC2D));
						I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read texture coordinates data.");

						i3Vector::Copy( &uv, &vec);

						SetTextureCoord( i, j, &uv);
					}
					break;
			}
		}
	}

	// Blend Index
	if( m_Format.GetBlendIndexCount() > 0)
	{
		for( j = 0; j < m_Count; j++)
		{
			//I3TRACE( "Index[%d] : ", j);

			for( i = 0; i < (UINT32) m_Format.GetBlendIndexCount(); i++)
			{
				UINT8 idx;

				Rc = pStream->Read( &idx, sizeof(UINT8));
				I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read blend index data.");
				
				SetBlendIndex( i, j, idx);

				//I3TRACE( "%d ", idx);
			}

			//I3TRACE( "\n");
		}
	}

	// Blend Weight
	for( i = 0; i < (UINT32) m_Format.GetBlendWeightCount(); i++)
	{
		for( j = 0; j < m_Count; j++)
		{
			REAL32 weight;

			Rc = pStream->Read( &weight, sizeof(REAL32));
			I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read blend weight data.");

			SetBlendWeight( i, j, weight);
		}
	}

	// Tangent
	if( m_Format.GetHasTangent())
	{
		for( i = 0; i < m_Count; i++)
		{
			i3::pack::VEC3D vec;
			VEC3D norm;

			Rc = pStream->Read( &vec, sizeof(i3::pack::VEC3D));
			I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read tangent data.");

			i3Vector::Copy( &norm, &vec);

			SetTangent( i, &norm);
		}
	}

	// Binormal
	if( m_Format.GetHasBinormal())
	{
		for( i = 0; i < m_Count; i++)
		{
			i3::pack::VEC3D vec;
			VEC3D norm;

			Rc = pStream->Read( &vec, sizeof(i3::pack::VEC3D));
			I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read tangent data.");

			i3Vector::Copy( &norm, &vec);

			SetBinormal( i, &norm);
		}
	}

	Unlock();

	return Result;
}

// Vertex Data Loading 시에 사용되는 임시 버퍼
// 가장 큰 경우, 아래와 같은 크기를 가질 수 있다.
//
// Position		: REAL32 * 3		: 12
// Normal		: REAL32 * 3		: 12
// Color		: RGBA_8888			: 4
// Tex. Coords	: REAL32 * 3 * 8	: 96
// Blend Index	: INT8 * 4			: 4
// Blend Weight : REAL32 * 4		: 16
// Point Size	: REAL32 * 2		: 8
// Total							: 152

UINT32 i3VertexArray::OnLoad20( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	//INT32 i, j, k;
	UINT32 Rc, Result = 0;
	pack::i3VertexArrayInfo2_0 header;
	INT8 * pBuf;
	UINT32 Sz;
	i3::pack::VERTEX_OFFSET Info;

	setRestorePoint( pResFile->GetStream());

	Rc = pStream->Read( &header, sizeof(header));
	I3CS_ERR( Rc, "i3VertexArray::OnLoad20()", "Could not read vertex array information.");
	
	m_PersistFormat.SetFormat( header.m_Format);
	m_PersistFormat.SetTypes( header.m_Types);

	if( Create( &m_PersistFormat, header.m_Count, i3Gfx::Convert( header.m_PersistantUsage)) == false)
	{
		i3Log( "i3VertexArray::OnLoad20()", "Could not create vertex array.");
		return STREAM_ERR;
	}

	setRestoreType( I3G_RESTORE_FILE);

	Info.m_Format = m_PersistFormat;

	Sz = CalcPersistantBufferSize20( &Info, 1);

	// Vertex Array Data는 Persist Format과 Runtime Format이 다를 수 있다.
	//if( m_PersistFormat != m_Format)
	{
		pBuf = (INT8 *) i3MemAlloc( Sz * header.m_Count);
		SetPersistData( (char *) pBuf );
	}
	/*
	else
	{
		Lock();

		pBuf = GetLockedBuffer();

		OnPreLoad( GetLockedBuffer(), &Info);
	}
	*/

	// 빠른 Loading을 위해, Persistent Buffer Size를 계산하여 일괄적으로 Loading한다.
	// 그 후, 각 Platform에 따라 재배치한다.
	{
		Rc = pStream->Read( pBuf, Sz * header.m_Count);
		I3CS_ERR( Rc, "i3VertexArray::OnLoad20()", "Could not read vertex array data.");
	}

	/*
	if( m_PersistFormat == m_Format)
	{
		OnPostLoad( GetLockedBuffer(), &Info);

		Unlock();
	}
	*/

	MakeRuntimeFormat( &Info);

	I3MEM_SAFE_FREE_POINTER( m_pPersistData );

	return Result;
}

UINT32 i3VertexArray::OnLoad25( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	//INT32 i, j, k;
	UINT32 Rc, Result = 0;
	pack::i3VertexArrayInfo2_0 header;
	INT8 * pBuf;
	UINT32 Sz;
	i3::pack::VERTEX_OFFSET Info;

	setRestorePoint( pResFile->GetStream());

	Rc = pStream->Read( &header, sizeof(header));
	I3CS_ERR( Rc, "i3VertexArray::OnLoad25()", "Could not read vertex array information.");
	
	m_PersistFormat.SetFormat( header.m_Format);
	m_PersistFormat.SetTypes( header.m_Types);

	if( Create( &m_PersistFormat, header.m_Count, i3Gfx::Convert( header.m_PersistantUsage)) == false)
	{
		i3Log( "i3VertexArray::OnLoad25()", "Could not create vertex array.");
		return STREAM_ERR;
	}

	setRestoreType( I3G_RESTORE_FILE);

	Info.m_Format = m_PersistFormat;

	Sz = CalcPersistantBufferSize25( &Info, 1);

	// Vertex Array Data는 Persist Format과 Runtime Format이 다를 수 있다.
	//if( m_PersistFormat != m_Format)
	{
		pBuf = (INT8 *) i3MemAlloc( Sz * header.m_Count);
		SetPersistData( (char *) pBuf );
	}	

	// 빠른 Loading을 위해, Persistent Buffer Size를 계산하여 일괄적으로 Loading한다.
	// 그 후, 각 Platform에 따라 재배치한다.
	{
		Rc = pStream->Read( pBuf, Sz * header.m_Count);
		I3CS_ERR( Rc, "i3VertexArray::OnLoad25()", "Could not read vertex array data.");
	}

	MakeRuntimeFormat( &Info);

	return Result;
}

UINT32 i3VertexArray::OnLoad30( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32 Rc, Result = 0;
	pack::i3VertexArrayInfo3_0 header;
	INT8 * pBuf;
	UINT32 Sz;
	i3::pack::VERTEX_OFFSET Info;

	setRestorePoint( pResFile->GetStream());

	Rc = pStream->Read( &header, sizeof(header));
	I3CS_ERR( Rc, "i3VertexArray::OnLoad25()", "Could not read vertex array information.");
	
	m_PersistFormat.SetFormat( header.m_Format);
	m_PersistFormat.SetTypes( header.m_Types);

	m_Center.x = header.m_Center.x;
	m_Center.y = header.m_Center.y;
	m_Center.z = header.m_Center.z;

	m_Rad = header.m_Radius;

	m_State = header.m_State;

	if( Create( &m_PersistFormat, header.m_Count, i3Gfx::Convert( header.m_PersistantUsage)) == false)
	{
		i3Log( "i3VertexArray::OnLoad30()", "Could not create vertex array.");
		return STREAM_ERR;
	}

	setRestoreType( I3G_RESTORE_FILE);

	Info.m_Format = m_PersistFormat;

	Sz = CalcPersistantBufferSize25( &Info, 1);

	// Vertex Array Data는 Persist Format과 Runtime Format이 다를 수 있다.
	//if( m_PersistFormat != m_Format)
	{
		pBuf = (INT8 *) i3MemAlloc( Sz * header.m_Count);
		SetPersistData( (char *) pBuf );
	}	

	// 빠른 Loading을 위해, Persistent Buffer Size를 계산하여 일괄적으로 Loading한다.
	// 그 후, 각 Platform에 따라 재배치한다.
	{
		Rc = pStream->Read( pBuf, Sz * header.m_Count);
		I3CS_ERR( Rc, "i3VertexArray::OnLoad25()", "Could not read vertex array data.");
	}

	MakeRuntimeFormat( &Info);

	return Result;
}


void i3VertexArray::SetPersistData( char * pBuf )	
{ 
	if( m_pPersistData != pBuf)
	{
		I3MEM_SAFE_FREE( m_pPersistData);
	}

	m_pPersistData		= pBuf;
}

UINT32 i3VertexArray::OnLoad( i3ResourceFile * pResFile)
{
	INT8 id[4];
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	setThreadMode( pResFile->InThread());

	if( pResFile->InThread())
		this->setRestoreType( I3G_RESTORE_MEM);

	Rc = pStream->Read( id, sizeof(id));
	I3CS_ERR( Rc, "i3VertexArray::OnLoad()", "Could not read vertex id field.");

	pStream->SetPosition( -(INT32)sizeof(id), STREAM_CURRENT);		// Rolling back

	// OnLoad10인지 20인지 체크
	switch( id[2])
	{
		case '3' :
			return OnLoad30( pResFile);

		case '2' :
			switch( id[3])
			{
				case '0' :
					#if defined( I3_WINCE )
						// WinCe 버전에선 2.5 이상밖에 읽을수가 없습니다.
						I3ASSERT_0;
					#endif

					return OnLoad20( pResFile);

				case '5' :
					return OnLoad25( pResFile);
			}
			break;
	}

	#if defined( I3_WINCE )
		// WinCe 버전에선 2.5 이상밖에 읽을수가 없습니다.
		I3ASSERT_0;
	#endif

	UINT32 rv = OnLoad10( pResFile);

	setThreadMode(false);

	return rv;
}

void i3VertexArray::OnPreSave( INT8 * /* pBuffer */)
{
}

void i3VertexArray::OnPostSave( INT8 * /* pBuffer */)
{
}

void i3VertexArray::OnPreLoad( INT8 * /* pBuffer */, i3::pack::VERTEX_OFFSET * /* pInfo */)
{
}

void i3VertexArray::OnPostLoad( INT8 * /* pBuffer */, i3::pack::VERTEX_OFFSET * /* pInfo */)
{
}

void i3VertexArray::MakeRuntimeFormat( i3::pack::VERTEX_OFFSET * pInfo)
{
	INT32 i, j;

	/////////////////////////////////
	// Persist Format을 Runtime Format으로 변환한다. 
	//
	//if( pInfo->m_Format.GetPlatform() == I3VF_PLATFORM_NA)
	//if( m_PersistFormat != m_Format)
	{
		INT8 * pSrc, * pDest;
	
		pSrc = (INT8 *) m_pPersistData;			// 원본 vertex data pointer

		Lock();
		pDest = GetLockedBuffer();			// 대상 vertex data pointer

		for( i = 0; i < (INT32) GetCount(); i++)
		{
			// Position
			if( m_Format.GetHasPosition())
			{
				REAL32 x, y, z;
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetPositionType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetPositionType()];

				x = _REAL_F2R( m_PersistFormat.GetPositionType(), pSrc + pInfo->m_OffsetPos);
				y = _REAL_F2R( m_PersistFormat.GetPositionType(), pSrc + pInfo->m_OffsetPos + ssz);
				z = _REAL_F2R( m_PersistFormat.GetPositionType(), pSrc + pInfo->m_OffsetPos + (ssz << 1));

				_REAL_R2F( m_Format.GetPositionType(), pDest + m_OffsetPos, x);
				_REAL_R2F( m_Format.GetPositionType(), pDest + m_OffsetPos + dsz, y);
				_REAL_R2F( m_Format.GetPositionType(), pDest + m_OffsetPos + (dsz << 1), z);
			}

			// Normal
			if( m_Format.GetHasNormal())
			{
				REAL32 x, y, z;
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetNormalType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetNormalType()];

				x = _REAL_F2R( m_PersistFormat.GetNormalType(), pSrc + pInfo->m_OffsetNormal);
				y = _REAL_F2R( m_PersistFormat.GetNormalType(), pSrc + pInfo->m_OffsetNormal + ssz);
				z = _REAL_F2R( m_PersistFormat.GetNormalType(), pSrc + pInfo->m_OffsetNormal + (ssz << 1));

				_REAL_R2F( m_Format.GetNormalType(), pDest + m_OffsetNormal, x);
				_REAL_R2F( m_Format.GetNormalType(), pDest + m_OffsetNormal + dsz, y);
				_REAL_R2F( m_Format.GetNormalType(), pDest + m_OffsetNormal + (dsz << 1), z);
			}

			// Color
			if( m_Format.GetHasColor())
			{
				UINT32 col;

				col = _COLOR_F2R( m_PersistFormat.GetColorType(), pSrc + pInfo->m_OffsetColor);

				if( m_ColorOrder == I3VF_COLORORDER_ARGB)
				{
					I3COLOR c;

					c.SetRGBA( col);
					col = c.GetARGB();
				}
				
				_COLOR_R2F( m_Format.GetColorType(), pDest + m_OffsetColor, col);
			}

			// Tangent
			if( m_Format.GetHasTangent())
			{
				REAL32 x, y, z;
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetTangentType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetTangentType()];

				x = _REAL_F2R( m_PersistFormat.GetTangentType(), pSrc + pInfo->m_OffsetTangent);
				y = _REAL_F2R( m_PersistFormat.GetTangentType(), pSrc + pInfo->m_OffsetTangent + ssz);
				z = _REAL_F2R( m_PersistFormat.GetTangentType(), pSrc + pInfo->m_OffsetTangent + (ssz << 1));

				_REAL_R2F( m_Format.GetTangentType(), pDest + m_OffsetTangent, x);
				_REAL_R2F( m_Format.GetTangentType(), pDest + m_OffsetTangent + dsz, y);
				_REAL_R2F( m_Format.GetTangentType(), pDest + m_OffsetTangent + (dsz << 1), z);
			}

			// Binormal
			if( m_Format.GetHasBinormal())
			{
				REAL32 x, y, z;
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetBinormalType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetBinormalType()];

				x = _REAL_F2R( m_PersistFormat.GetBinormalType(), pSrc + pInfo->m_OffsetBinormal);
				y = _REAL_F2R( m_PersistFormat.GetBinormalType(), pSrc + pInfo->m_OffsetBinormal + ssz);
				z = _REAL_F2R( m_PersistFormat.GetBinormalType(), pSrc + pInfo->m_OffsetBinormal + (ssz << 1));

				_REAL_R2F( m_Format.GetBinormalType(), pDest + m_OffsetBinormal, x);
				_REAL_R2F( m_Format.GetBinormalType(), pDest + m_OffsetBinormal + dsz, y);
				_REAL_R2F( m_Format.GetBinormalType(), pDest + m_OffsetBinormal + (dsz << 1), z);
			}

			// Texture Coords.
			{
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetTextureCoordType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetTextureCoordType()];

				for( j = 0; j < m_Format.GetTextureCoordSetCount(); j++)
				{
					
					for( INT32 k = 0; k < m_Format.GetTextureCoordCount(); k++)
					{
						REAL32 v;
						v = _REAL_F2R( m_PersistFormat.GetTextureCoordType(), pSrc + pInfo->m_OffsetTexCoord[j] + (ssz * k));
						
						_REAL_R2F( m_Format.GetTextureCoordType(), pDest + m_OffsetTexCoord[j] + (dsz * k), v);
					}
				}
			}

			// Blend Index
			{
				i3GfxCaps * pCaps = i3RenderContext::GetCaps();

				if( (pCaps == nullptr) || (pCaps->isSupported_UBYTE4()))
				{
					for( j = 0; j < m_Format.GetBlendIndexCount(); j++)
					{
						*(pDest + m_OffsetBlendIndex + j) = *(pSrc + pInfo->m_OffsetBlendIndex + j);
					}
				}
				else
				{
					for( j = 0; j < m_Format.GetBlendIndexCount(); j++)
					{
						((REAL32 *)(pDest + m_OffsetBlendIndex ))[j] = (REAL32) *(pSrc + pInfo->m_OffsetBlendIndex + j);
					}
				}
			}
			
			// Blend Weight
			{
				UINT32 ssz = m_ComponentSize[ m_PersistFormat.GetBlendWeightType()];
				UINT32 dsz = m_ComponentSize[ m_Format.GetBlendWeightType()];

				for( j = 0; j < m_Format.GetBlendWeightCount(); j++)
				{
					REAL32 v;

					v = _REAL_F2R( m_PersistFormat.GetBlendWeightType(), pSrc + pInfo->m_OffsetWeight + (ssz * j));

					_REAL_R2F( m_Format.GetBlendWeightType(), pDest + m_OffsetWeight + (dsz * j), v);
				}
			}

			pSrc += pInfo->m_Stride;
			pDest += m_Stride;
		}

		/*
		for( i = 0; i < GetCount(); i++)
		{
			for( j = 0; j < m_Format.GetBlendIndexCount(); j++)
			{
				I3TRACE( "Index[%d, %d] : %d\n", i, j, GetBlendIndex( j, i));
			}
		}

		for( i = 0; i < GetCount(); i++)
		{
			for( j = 0; j < m_Format.GetBlendWeightCount(); j++)
			{
				I3TRACE( "Weight[%d, %d] : %f\n", i, j, GetBlendWeight( j, i));
			}
		}
		*/

		Unlock();
	}

	if( g_pRenderContext->getGfxOption()->getKeepPersistDataEnable() == false)
	{
		SetPersistData( nullptr);
	}
}

void i3VertexArray::Transform( MATRIX * pMatrix, i3VertexArray * pDest)
{
	UINT32 i;

	CopyTo( pDest, I3_COPY_INSTANCE);

	pDest->Lock();

	for( i = 0; i < GetCount(); i++)
	{
		if( m_Format.GetHasPosition())
		{
			VEC3D	vec;

			pDest->GetPosition( i, &vec);

			i3Vector::TransformCoord( &vec, &vec, pMatrix);

			pDest->SetPosition( i, &vec);
		}

		if( m_Format.GetHasNormal())
		{
			VEC3D	vec;

			pDest->GetNormal( i, &vec);

			i3Vector::TransformNormal( &vec, &vec, pMatrix);
			i3Vector::Normalize( &vec, &vec);

			pDest->SetNormal( i, &vec);
		}

		if( m_Format.GetHasTangent())
		{
			VEC3D	vec;

			pDest->GetTangent( i, &vec);

			i3Vector::TransformNormal( &vec, &vec, pMatrix);
			i3Vector::Normalize( &vec, &vec);

			pDest->SetTangent( i, &vec);
		}

		if( m_Format.GetHasBinormal())
		{
			VEC3D	vec;

			pDest->GetBinormal( i, &vec);

			i3Vector::TransformNormal( &vec, &vec, pMatrix);
			i3Vector::Normalize( &vec, &vec);

			pDest->SetBinormal( i, &vec);
		}
	}

	pDest->Unlock();
}

void	i3VertexArray::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	INT32 i, cnt, j;

	i3GfxResource::CopyTo( pDest, method);

	i3VertexArray * pVA = (i3VertexArray *) pDest;

	Lock( 0);
	pVA->Lock( 0);

	cnt = MIN( GetCount(), pVA->GetCount());

	for( i = 0; i < cnt; i++)
	{
		// Position
		if( (GetFormat()->GetHasPosition()) && (pVA->GetFormat()->GetHasPosition()))
		{
			VEC3D pos;

			GetPosition( i, &pos);
			pVA->SetPosition( i, &pos);
		}

		// Normal
		if((GetFormat()->GetHasNormal()) && (pVA->GetFormat()->GetHasNormal()))
		{
			VEC3D norm;

			GetNormal( i, &norm);
			pVA->SetNormal( i, &norm);
		}

		// Color
		if((GetFormat()->GetHasColor()) && (pVA->GetFormat()->GetHasColor()))
		{
			COLORREAL col;

			GetColor( i, &col);
			pVA->SetColor( i, &col);
		}

		// Texture Coords
		{
			INT32 num = MIN( GetFormat()->GetTextureCoordSetCount(), GetFormat()->GetTextureCoordSetCount());
			VEC3D uv;

			for( j = 0; j < num; j++)
			{
				switch( GetFormat()->GetTextureCoordCount())
				{
					case 2 :	GetTextureCoord( j, i, (VEC2D *) &uv);		break;
				}

				switch( pVA->GetFormat()->GetTextureCoordCount())
				{
					case 2 :	pVA->SetTextureCoord( j, i, (VEC2D *) &uv);		break;
				}
			}
		}

		// Tangent
		if((GetFormat()->GetHasTangent()) && (pVA->GetFormat()->GetHasTangent()))
		{
			VEC3D vec;

			GetTangent( i, &vec);
			pVA->SetTangent( i, &vec);
		}

		// Binormal
		if((GetFormat()->GetHasBinormal()) && (pVA->GetFormat()->GetHasBinormal()))
		{
			VEC3D vec;

			GetBinormal( i, &vec);
			pVA->SetBinormal( i, &vec);
		}

		// Blend Weight
		{
			INT32 num = MIN( GetFormat()->GetBlendWeightCount(), pVA->GetFormat()->GetBlendWeightCount());
			REAL32 w;

			for( j = 0; j < num; j++)
			{
				w = GetBlendWeight( j, i);
				pVA->SetBlendWeight( j, i, w);
			}
		}

		// Blend Index
		{
			INT32 num = MIN( GetFormat()->GetBlendIndexCount(), pVA->GetFormat()->GetBlendIndexCount());
			UINT32 idx;

			for( j = 0; j < num; j++)
			{
				idx = GetBlendIndex( j, i);
				pVA->SetBlendIndex( j, i, idx);
			}
		}
	}

	Unlock();
	pVA->Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////

i3VertexArray * i3VertexArray::CreateGrid( VEC3D * pCenter, REAL32 xSize, REAL32 zSize, REAL32 xGrid, REAL32 zGrid)
{
	i3VertexFormat format;
	INT32 cx, cy, i, idx = 0;
	REAL32 sp, startX, startZ, endX, endZ;
	COLORREAL CenterColor, NormalColor;
	VEC3D vec;
	i3VertexArray * pVA;

	cx = (INT32)(xSize / xGrid * 0.5f);
	cy = (INT32)(zSize / zGrid * 0.5f);

	startX = -(xGrid * cx);
	startZ = -(zGrid * cy);
	endX = (xGrid * cx);
	endZ = (zGrid * cy);

	i3Color::Set( &NormalColor, (UINT8) 75, 75, 75, 255);
	i3Color::Set( &CenterColor, (UINT8) 255, 255, 104, 255);

	format.SetHasPosition( true);
	format.SetHasNormal( false);
	format.SetHasColor( true);
	
	{
		INT32 VertexCount;

		VertexCount = 4 + (cx * 4) + (cy * 4);

		pVA = i3VertexArray::new_object_ref();

		pVA->Create( &format, VertexCount, 0);
	}

	sp = 0.0f;

	pVA->Lock();

	// Vertical Lines
	for( i = 0; i < cx ; i++)
	{
		sp += xGrid;

		// Left Lines
		{
			setXYZ( &vec, -sp, 0.0f, startZ);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx, &vec);
			pVA->SetColor( idx, &NormalColor);

			setXYZ( &vec, -sp, 0.0f, endZ);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx + 1, &vec);
			pVA->SetColor( idx + 1, &NormalColor);
		}

		idx += 2;

		// Right Lines
		{
			setXYZ( &vec, sp, 0.0f, startZ);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx, &vec);
			pVA->SetColor( idx, &NormalColor);

			setXYZ( &vec, sp, 0.0f, endZ);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx + 1, &vec);
			pVA->SetColor( idx + 1, &NormalColor);
		}

		idx += 2;
	}

	// Horizontal Lines
	sp = 0.0f;
	for( i = 0; i < cy; i++)
	{
		sp += zGrid;

		// Up Lines
		{
			setXYZ( &vec, startX, 0.0f, -sp);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx, &vec);
			pVA->SetColor( idx, &NormalColor);

			setXYZ( &vec, endX, 0.0f, -sp);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx + 1, &vec);
			pVA->SetColor( idx + 1, &NormalColor);
		}

		idx += 2;

		// Down Lines
		{
			setXYZ( &vec, startX, 0.0f, sp);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx, &vec);
			pVA->SetColor( idx, &NormalColor);

			setXYZ( &vec, endX, 0.0f, sp);
			i3Vector::Add( &vec, &vec, pCenter);
			pVA->SetPosition( idx + 1, &vec);
			pVA->SetColor( idx + 1, &NormalColor);
		}

		idx += 2;
	}

	// Center Line (Horizontal)
	{
		i3Vector::Set( &vec, startX, 0.0f, 0.0f);
		pVA->SetPosition( idx, &vec); 
		pVA->SetColor( idx, &CenterColor); idx++;

		setXYZ( &vec, endX, 0.0f, 0.0f);
		pVA->SetPosition( idx, &vec); 
		pVA->SetColor( idx, &CenterColor); idx++;
	}

	// Center Line (Vertical)
	{
		setXYZ( &vec, 0.0f, 0.0f, startZ);
		pVA->SetPosition( idx, &vec); 
		pVA->SetColor( idx, &CenterColor); idx++;

		setXYZ( &vec, 0.0f, 0.0f, endZ);
		pVA->SetPosition( idx, &vec); 
		pVA->SetColor( idx, &CenterColor);	idx++;
	}

	pVA->Unlock();

	return pVA;
}

i3VertexArray * i3VertexArray::CreatePointSprite( UINT32 pointCount, UINT32 SizeCount, bool bColor, bool bUseUV)
{
	i3VertexArray * pNew;
	i3VertexFormat format;

	format.SetHasPosition( true);

	if( bColor)
		format.SetHasColor( true);
	
	if( bUseUV)
	{
		format.SetTextureCoordSetCount( 1);
		format.SetTextureCoordCount( 2);
	}

	//format.SetPointSpriteSizeCount( (UINT8) SizeCount);

	pNew = i3VertexArray::new_object_ref();
	I3ASSERT( pNew != nullptr);

	pNew->Create( &format, pointCount, I3G_USAGE_WRITEONLY | I3G_USAGE_POINT_SPRITES | I3G_USAGE_SOFTWAREPROCESSING);

	return pNew;
}

i3VertexArray * i3VertexArray::CreateRect( REAL32 w, REAL32 h, bool bUV, bool bColor,
										  COLORREAL * pColor1,
										  COLORREAL * pColor2,
										  COLORREAL * pColor3,
										  COLORREAL * pColor4)
{
	i3VertexFormat format;
	i3VertexArray * pVA;

	format.SetHasPosition( true);
	if( bUV)
	{
		format.SetTextureCoordSetCount( 1);
		format.SetTextureCoordCount( 2);
	}

	if( bColor)
	{
		format.SetHasColor( true);
	}

	pVA = i3VertexArray::new_object_ref();
	I3ASSERT( pVA != nullptr);

	pVA->Create( &format, 6, 0);

	pVA->Lock();

	// Position
	{
		VEC3D vec;
		REAL32 hw, hh;

		hw = w * 0.5f;
		hh = h * 0.5f;

		i3Vector::Set( &vec, -hw, hh, 0.0f);
		pVA->SetPosition( 0, &vec);

		i3Vector::Set( &vec, -hw, -hh, 0.0f);
		pVA->SetPosition( 1, &vec);
		pVA->SetPosition( 4, &vec);

		i3Vector::Set( &vec, hw, hh, 0.0f);
		pVA->SetPosition( 2, &vec);

		pVA->SetPosition( 3, &vec);

		i3Vector::Set( &vec, hw, -hh, 0.0f);
		pVA->SetPosition( 5, &vec);
	}

	if( bUV)
	{
		VEC2D vec;

		i3Vector::Set( &vec, 0.0f, 0.0f);
		pVA->SetTextureCoord( 0, 0, &vec);

		i3Vector::Set( &vec, 0.0f, 1.0f);
		pVA->SetTextureCoord( 0, 1, &vec);
		pVA->SetTextureCoord( 0, 4, &vec);

		i3Vector::Set( &vec, 1.0f, 0.0f);
		pVA->SetTextureCoord( 0, 2, &vec);
		pVA->SetTextureCoord( 0, 3, &vec);

		i3Vector::Set( &vec, 1.0f, 1.0f);
		pVA->SetTextureCoord( 0, 5, &vec);
	}

	if( bColor)
	{
		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);

		if( pColor1 != nullptr)
			pVA->SetColor( 0, pColor1);
		else
			pVA->SetColor( 0, &col);

		if( pColor2 != nullptr)
		{
			pVA->SetColor( 2, pColor2);
			pVA->SetColor( 3, pColor2);
		}
		else
		{
			pVA->SetColor( 2, &col);
			pVA->SetColor( 3, &col);
		}

		if( pColor3 != nullptr)
		{
			pVA->SetColor( 1, pColor3);
			pVA->SetColor( 4, pColor3);
		}
		else
		{
			pVA->SetColor( 1, &col);
			pVA->SetColor( 4, &col);
		}

		if( pColor4 != nullptr)
			pVA->SetColor( 5, pColor4);
		else
			pVA->SetColor( 5, &col);
	}

	pVA->Unlock();

	return pVA;
}

i3VertexArray *	i3VertexArray::CreateBox( VEC3D * pMin, VEC3D * pMax, bool bColor, bool bUV, COLORREAL * pColor)
{
	i3VertexFormat format;
	i3VertexArray * pVA;
	REAL32 x1, y1, z1, x2, y2, z2;

	format.SetHasPosition( true);

	if( bColor)
		format.SetHasColor( true);
	else
		format.SetHasNormal( true);

	if( bUV)
	{
		format.SetTextureCoordSetCount( 1);
		format.SetTextureCoordCount( 2);
	}

	pVA = i3VertexArray::new_object_ref();

	pVA->Create( &format, 36, 0);

	pVA->Lock();

	{
		VEC3D pos[36], *pPos;

		x1 = i3Vector::GetX( pMin);
		y1 = i3Vector::GetY( pMin);
		z1 = i3Vector::GetZ( pMin);
		x2 = i3Vector::GetX( pMax);
		y2 = i3Vector::GetY( pMax);
		z2 = i3Vector::GetZ( pMax);

		pPos = pos;

		i3Vector::Set( pPos, x1, y2, z2); pPos++;		// 0
		i3Vector::Set( pPos, x1, y1, z2); pPos++;		// 1
		i3Vector::Set( pPos, x2, y2, z2); pPos++;		// 2
		i3Vector::Set( pPos, x2, y2, z2); pPos++;		// 3
		i3Vector::Set( pPos, x1, y1, z2); pPos++;		// 4
		i3Vector::Set( pPos, x2, y1, z2); pPos++;		// 5

		i3Vector::Set( pPos, x2, y2, z2); pPos++;		// 6
		i3Vector::Set( pPos, x2, y1, z2); pPos++;		// 7
		i3Vector::Set( pPos, x2, y2, z1); pPos++;		// 8
		i3Vector::Set( pPos, x2, y2, z1); pPos++;		// 9
		i3Vector::Set( pPos, x2, y1, z2); pPos++;		// 10
		i3Vector::Set( pPos, x2, y1, z1); pPos++;		// 11

		i3Vector::Set( pPos, x2, y2, z1); pPos++;		// 12
		i3Vector::Set( pPos, x2, y1, z1); pPos++;		// 13
		i3Vector::Set( pPos, x1, y2, z1); pPos++;		// 14
		i3Vector::Set( pPos, x1, y2, z1); pPos++;		// 15
		i3Vector::Set( pPos, x2, y1, z1); pPos++;		// 16
		i3Vector::Set( pPos, x1, y1, z1); pPos++;		// 17

		i3Vector::Set( pPos, x1, y2, z1); pPos++;		// 18
		i3Vector::Set( pPos, x1, y1, z1); pPos++;		// 19
		i3Vector::Set( pPos, x1, y2, z2); pPos++;		// 20
		i3Vector::Set( pPos, x1, y2, z2); pPos++;		// 21
		i3Vector::Set( pPos, x1, y1, z1); pPos++;		// 22
		i3Vector::Set( pPos, x1, y1, z2); pPos++;		// 23

		i3Vector::Set( pPos, x1, y2, z1); pPos++;		// 24
		i3Vector::Set( pPos, x1, y2, z2); pPos++;		// 25
		i3Vector::Set( pPos, x2, y2, z1); pPos++;		// 26
		i3Vector::Set( pPos, x2, y2, z1); pPos++;		// 27
		i3Vector::Set( pPos, x1, y2, z2); pPos++;		// 28
		i3Vector::Set( pPos, x2, y2, z2); pPos++;		// 29

		i3Vector::Set( pPos, x1, y1, z2); pPos++;		// 30
		i3Vector::Set( pPos, x1, y1, z1); pPos++;		// 31
		i3Vector::Set( pPos, x2, y1, z2); pPos++;		// 32
		i3Vector::Set( pPos, x2, y1, z2); pPos++;		// 33
		i3Vector::Set( pPos, x1, y1, z1); pPos++;		// 34
		i3Vector::Set( pPos, x2, y1, z1); pPos++;		// 35

		pVA->SetPositions( 0, 36, pos);
	}

	if( format.GetHasNormal())
	{
		VEC3D norm[36], *pNorm;

		pNorm = norm;

		i3Vector::Set( pNorm, 0.0f, 0.0f, 1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, 1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, 1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, 1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, 1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, 1.0f); pNorm++;

		i3Vector::Set( pNorm, 1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 1.0f, 0.0f, 0.0f); pNorm++;

		i3Vector::Set( pNorm, 0.0f, 0.0f, -1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, -1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, -1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, -1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, -1.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 0.0f, -1.0f); pNorm++;

		i3Vector::Set( pNorm, -1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, -1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, -1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, -1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, -1.0f, 0.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, -1.0f, 0.0f, 0.0f); pNorm++;

		i3Vector::Set( pNorm, 0.0f, 1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, 1.0f, 0.0f); pNorm++;

		i3Vector::Set( pNorm, 0.0f, -1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, -1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, -1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, -1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, -1.0f, 0.0f); pNorm++;
		i3Vector::Set( pNorm, 0.0f, -1.0f, 0.0f); pNorm++;

		pVA->SetNormals( 0, 36, norm);
	}

	if( bUV)
	{
		VEC2D uv[36], *pUV;

		x1 = 0.0f;
		y1 = 0.0f;
		x2 = 1.0f;
		y2 = 1.0f;

		pUV = uv;

		i3Vector::Set( pUV, x1, y1); pUV++;		// 0
		i3Vector::Set( pUV, x1, y2); pUV++;		// 1
		i3Vector::Set( pUV, x2, y1); pUV++;		// 2
		i3Vector::Set( pUV, x2, y1); pUV++;		// 3
		i3Vector::Set( pUV, x1, y2); pUV++;		// 4
		i3Vector::Set( pUV, x2, y2); pUV++;		// 5

		i3Vector::Set( pUV, x1, y1); pUV++;		// 0
		i3Vector::Set( pUV, x1, y2); pUV++;		// 1
		i3Vector::Set( pUV, x2, y1); pUV++;		// 2
		i3Vector::Set( pUV, x2, y1); pUV++;		// 3
		i3Vector::Set( pUV, x1, y2); pUV++;		// 4
		i3Vector::Set( pUV, x2, y2); pUV++;		// 5

		i3Vector::Set( pUV, x1, y1); pUV++;		// 0
		i3Vector::Set( pUV, x1, y2); pUV++;		// 1
		i3Vector::Set( pUV, x2, y1); pUV++;		// 2
		i3Vector::Set( pUV, x2, y1); pUV++;		// 3
		i3Vector::Set( pUV, x1, y2); pUV++;		// 4
		i3Vector::Set( pUV, x2, y2); pUV++;		// 5

		i3Vector::Set( pUV, x1, y1); pUV++;		// 0
		i3Vector::Set( pUV, x1, y2); pUV++;		// 1
		i3Vector::Set( pUV, x2, y1); pUV++;		// 2
		i3Vector::Set( pUV, x2, y1); pUV++;		// 3
		i3Vector::Set( pUV, x1, y2); pUV++;		// 4
		i3Vector::Set( pUV, x2, y2); pUV++;		// 5

		i3Vector::Set( pUV, x1, y1); pUV++;		// 0
		i3Vector::Set( pUV, x1, y2); pUV++;		// 1
		i3Vector::Set( pUV, x2, y1); pUV++;		// 2
		i3Vector::Set( pUV, x2, y1); pUV++;		// 3
		i3Vector::Set( pUV, x1, y2); pUV++;		// 4
		i3Vector::Set( pUV, x2, y2); pUV++;		// 5

		i3Vector::Set( pUV, x1, y1); pUV++;		// 0
		i3Vector::Set( pUV, x1, y2); pUV++;		// 1
		i3Vector::Set( pUV, x2, y1); pUV++;		// 2
		i3Vector::Set( pUV, x2, y1); pUV++;		// 3
		i3Vector::Set( pUV, x1, y2); pUV++;		// 4
		i3Vector::Set( pUV, x2, y2); pUV++;		// 5

		pVA->SetTextureCoords( 0, 0, 36, uv);
	}

	if( bColor)
	{
		INT32 i;

		for( i = 0; i < 36; i++)
		{
			pVA->SetColor( i, pColor);
		}
	}

	pVA->Unlock();

	return pVA;
}

void i3VertexArray::CopyVertexFrom( INT32 idxDest, i3VertexArray * pSrc, INT32 idxSrc)
{
	INT32 i;

	I3ASSERT( (idxDest >= 0) && (idxDest < (INT32) GetCount()));
	I3ASSERT( pSrc != nullptr);

	if( HasPosition())
	{
		VEC3D	vPos;

		pSrc->GetPosition( idxSrc, &vPos);
		SetPosition( idxDest, &vPos);
	}

	if( HasNormal())
	{
		VEC3D	vNormal;

		pSrc->GetNormal( idxSrc, &vNormal);
		SetNormal( idxDest, &vNormal);
	}

	if( HasColor())
	{
		COLORREAL	Col;

		pSrc->GetColor( idxSrc, &Col);
		SetColor( idxDest, &Col);
	}

	if( HasTangent())
	{
		VEC3D	vTangent;

		pSrc->GetTangent( idxSrc, &vTangent);
		SetTangent( idxDest, &vTangent);
	}

	if( HasBinormal())
	{
		VEC3D	vBiNormal;

		pSrc->GetBinormal( idxSrc, &vBiNormal);
		SetBinormal( idxDest, &vBiNormal);
	}

	for( i = 0; i < GetFormat()->GetTextureCoordSetCount(); i++)
	{
		VEC2D pCoord;

		pSrc->GetTextureCoord( i, idxSrc, &pCoord);
		SetTextureCoord( i, idxDest, &pCoord);
	}

	for( i = 0; i < GetFormat()->GetBlendIndexCount(); i++)
	{
		UINT32 BlendIndex = pSrc->GetBlendIndex( i, idxSrc);

		SetBlendIndex( i, idxDest, BlendIndex);
	}

	for( i = 0; i < GetFormat()->GetBlendWeightCount(); i++)
	{
		REAL32 BlendWeight = pSrc->GetBlendWeight( i, idxSrc);

		SetBlendWeight( i, idxDest, BlendWeight);
	}
}

bool i3VertexArray::IsSameVertex( INT32 idxThis, i3VertexArray * pSrcVA, INT32 idxSrc)
{
	INT32 i;
	VEC3D	vec3;

	I3ASSERT( (idxThis >= 0) && (idxThis < (INT32) GetCount()));
	I3ASSERT( pSrcVA != nullptr);

	if( HasPosition())
	{
		pSrcVA->GetPosition( idxSrc, &vec3);
		
		if( IsSamePosition( idxThis, &vec3) == false)
			return false;
	}

	if( HasNormal())
	{
		pSrcVA->GetNormal( idxSrc, &vec3);
		
		if( IsSameNormal( idxThis, &vec3) == false)
			return false;
	}

	if( HasColor())
	{
		COLORREAL	Col;

		pSrcVA->GetColor( idxSrc, &Col);
		
		if( IsSameColor( idxThis, &Col) == false)
			return false;
	}

	if( HasTangent())
	{
		pSrcVA->GetTangent( idxSrc, &vec3);
		
		if( IsSameTangent( idxThis, &vec3) == false)
			return false;
	}

	if( HasBinormal())
	{
		pSrcVA->GetBinormal( idxSrc, &vec3);
		
		if( IsSameBinormal( idxThis, &vec3) == false)
			return false;
	}

	for( i = 0; i < GetFormat()->GetTextureCoordSetCount(); i++)
	{
		VEC2D uv;

		pSrcVA->GetTextureCoord( i, idxSrc, &uv);

		if( IsSameTextureCoord( i, idxThis, &uv) == false)
			return false;
	}

	for( i = 0; i < GetFormat()->GetBlendIndexCount(); i++)
	{
		UINT32 BlendIndex = pSrcVA->GetBlendIndex( i, idxSrc);

		if( IsSameBlendIndex( i, idxThis, BlendIndex) == false)
			return false;
	}

	for( i = 0; i < GetFormat()->GetBlendWeightCount(); i++)
	{
		REAL32 BlendWeight = pSrcVA->GetBlendWeight( i, idxSrc);

		if( IsSameBlendWeight( i, idxThis, BlendWeight) == false)
			return false;
	}

	return true;
}

void i3VertexArray::InterpolateVertex( INT32 idxDest, i3VertexArray * pSrc, INT32 idxStart, INT32 idxEnd, REAL32 t)
{
	INT32 i;
	VEC3D	p1, p2, p;

	I3ASSERT( (idxDest >= 0) && (idxDest < (INT32) GetCount()));
	I3ASSERT( pSrc != nullptr);

	if( HasPosition())
	{
		pSrc->GetPosition( idxStart, &p1);
		pSrc->GetPosition( idxEnd, &p2);

		i3Vector::Lerp( &p, &p1, &p2, t);
		SetPosition( idxDest, &p);
	}

	if( HasNormal())
	{
		pSrc->GetNormal( idxStart, &p1);
		pSrc->GetNormal( idxEnd, &p2);

		i3Vector::Lerp( &p, &p1, &p2, t);
		i3Vector::Normalize( &p, &p);
		SetNormal( idxDest, &p);
	}

	if( HasColor())
	{
		COLORREAL	c, c1, c2;

		pSrc->GetColor( idxStart, &c1);
		pSrc->GetColor( idxEnd, &c2);

		i3Color::Lerp( &c, &c1, &c2, t);
		SetColor( idxDest, &c);
	}

	if( HasTangent())
	{
		pSrc->GetTangent( idxStart, &p1);
		pSrc->GetTangent( idxEnd, &p2);
		
		i3Vector::Lerp( &p, &p1, &p2, t);
		i3Vector::Normalize( &p, &p);
		SetTangent( idxDest, &p);
	}

	if( HasBinormal())
	{
		pSrc->GetBinormal( idxStart, &p1);
		pSrc->GetBinormal( idxEnd, &p2);

		i3Vector::Lerp( &p, &p1, &p2, t);
		i3Vector::Normalize( &p, &p);
		SetBinormal( idxDest, &p);
	}

	for( i = 0; i < GetFormat()->GetTextureCoordSetCount(); i++)
	{
		VEC2D uv, uv1, uv2;

		pSrc->GetTextureCoord( i, idxStart, &uv1);
		pSrc->GetTextureCoord( i, idxEnd,	&uv2);

		i3Vector::Lerp( (VEC2D *) &uv, (const VEC2D *) &uv1, (const VEC2D *) &uv2, t);
		SetTextureCoord( i, idxDest, &uv);
	}

	// Blend Index 및 Weight에 대한 Interpolate는 제대로 동작하지 않음.
	// 실제 Vertex Blending에 대해 Interpolate를 하려면
	// Blend Bone의 개수가 추가되어야 한다.
	for( i = 0; i < GetFormat()->GetBlendIndexCount(); i++)
	{
		UINT32 BlendIndex = pSrc->GetBlendIndex( i, idxStart);

		SetBlendIndex( i, idxDest, BlendIndex);
	}

	for( i = 0; i < GetFormat()->GetBlendWeightCount(); i++)
	{
		REAL32 BlendWeight = pSrc->GetBlendWeight( i, idxStart);

		SetBlendWeight( i, idxDest, BlendWeight);
	}
}

void i3VertexArray::ExchangeVertex( INT32 idx0, INT32 idx1)
{
	I3_BOUNDCHK( idx0, (INT32) GetCount());
	I3_BOUNDCHK( idx1, (INT32) GetCount());

	#if 1
	{
		INT8 *	p0, * p1;
		char	temp[1024];

		p0 = m_pLockedBuf + ((idx0 - m_LockStartIdx) * m_Stride);
		p1 = m_pLockedBuf + ((idx1 - m_LockStartIdx) * m_Stride);

		I3ASSERT( m_Stride < sizeof(temp));

		i3mem::Copy( temp, p0, m_Stride);
		i3mem::Copy( p0, p1, m_Stride);
		i3mem::Copy( p1, temp, m_Stride);
	}
	#else
	{
		VEC3D vec0, vec1;

		if( HasPosition())
		{
			GetPosition( idx0, &vec0);		GetPosition( idx1, &vec1);
			SetPosition( idx0, &vec1);		SetPosition( idx1, &vec0);
		}

		if( HasNormal())
		{
			GetNormal( idx0, &vec0);		GetNormal( idx1, &vec1);
			SetNormal( idx0, &vec1);		SetNormal( idx1, &vec0);
		}

		if( HasTangent())
		{
			GetTangent( idx0, &vec0);		GetTangent( idx1, &vec1);
			SetTangent( idx0, &vec1);		SetTangent( idx1, &vec0);
		}

		if( HasBinormal())
		{
			GetBinormal( idx0, &vec0);		GetBinormal( idx1, &vec1);
			SetBinormal( idx0, &vec1);		SetBinormal( idx1, &vec0);
		}

		if( HasColor())
		{
			COLORREAL c0, c1;

			GetColor( idx0, &c0);		GetColor( idx1, &c1);
			SetColor( idx0, &c1);		SetColor( idx1, &c0);
		}

		for( i = 0; i < GetFormat()->GetTextureCoordSetCount(); i++)
		{
			VEC2D uv0, uv1;

			GetTextureCoord( i, idx0, &uv0);		GetTextureCoord( i, idx1, &uv1);
			SetTextureCoord( i, idx0, &uv1);		SetTextureCoord( i, idx1, &uv0);
		}

		for( i = 0; i < GetFormat()->GetBlendIndexCount(); i++)
		{
			UINT32 i0 = GetBlendIndex( i, idx0);
			UINT32 i1 = GetBlendIndex( i, idx1);

			SetBlendIndex( i, idx0, i1);
			SetBlendIndex( i, idx1, i0);
		}

		for( i = 0; i < GetFormat()->GetBlendWeightCount(); i++)
		{
			REAL32 w0 = GetBlendWeight( i, idx0);
			REAL32 w1 = GetBlendWeight( i, idx1);

			SetBlendWeight( i, idx0, w1);
			SetBlendWeight( i, idx1, w0);
		}
	}
	#endif
}

#ifdef I3_DEBUG

void i3VertexArray::Dump( INT32 nCount )
{
	UINT32 i, j, ssz;
	INT8 * pSrc;
	i3::pack::VERTEX_OFFSET Info;
	REAL32 x, y, z;

	if( m_bModified == true)
	{
		// Runtime에서 변경되었기 때문에 Persist Data가 있다고 하더라도 무시한다.		
		/*
		if(m_pPersistData != nullptr)
		{
			i3mem::Free( m_PersistDataIdx ); 
			m_pPersistData = nullptr; 
		}
		*/
		I3MEM_SAFE_FREE_POINTER( m_pPersistData ); 
	}

	Lock();

	pSrc = GetLockedBuffer();

	INT32 nDumpCount = (nCount < 0) ? m_Count : nCount;
	I3ASSERT( nDumpCount <= (INT32) m_Count );

	for( i = 0; i < (UINT32) nDumpCount; i++)
	{
		I3TRACE( "%08X :", i );
		// Position
		if( m_PersistFormat.GetHasPosition())
		{
			ssz = m_ComponentSize[ (INT32) m_Format.GetPositionType()];

			x = _REAL_F2R( m_Format.GetPositionType(), pSrc + m_OffsetPos);
			y = _REAL_F2R( m_Format.GetPositionType(), pSrc + m_OffsetPos + ssz);
			z = _REAL_F2R( m_Format.GetPositionType(), pSrc + m_OffsetPos + (ssz << 1));

			I3TRACE( "  x,y,z = %6.2f, %6.2f, %6.2f", x, y, z );
		}

		// Normal
		if( m_PersistFormat.GetHasNormal())
		{
			ssz = m_ComponentSize[ (INT32) m_Format.GetNormalType()];

			x = _REAL_F2R( m_Format.GetNormalType(), pSrc + m_OffsetNormal);
			y = _REAL_F2R( m_Format.GetNormalType(), pSrc + m_OffsetNormal + ssz);
			z = _REAL_F2R( m_Format.GetNormalType(), pSrc + m_OffsetNormal + (ssz << 1));

			I3TRACE( "  Normal = %6f, %6f, %6f", x, y, z );
		}

		// Vertex Color
		if( m_PersistFormat.GetHasColor())
		{
			UINT32 col;
			col = _COLOR_F2R( m_Format.GetColorType(), pSrc + m_OffsetColor);

			I3TRACE( "  Color = %0.8X, col" );
		}

		// Texture Coords
		for( j = 0; j < (UINT32) m_PersistFormat.GetTextureCoordSetCount(); j++)
		{
			I3TRACE( "  UV = " );
			ssz = m_ComponentSize[ (INT32) m_Format.GetTextureCoordType()];

			for( INT32 k = 0; k < m_PersistFormat.GetTextureCoordCount(); k++)
			{
				x = _REAL_F2R( m_Format.GetTextureCoordType(), pSrc + m_OffsetTexCoord[j] + (k * ssz));
				I3TRACE( " %6f", x );
			}
		}

		//// Blend Index
		//if( m_PersistFormat.GetBlendIndexCount() > 0)
		//{
		//	ssz = m_Format.GetBlendIndexCount();
		//}

		//// Blend Weight
		//if( m_PersistFormat.GetBlendWeightCount() > 0)
		//{
		//	ssz = m_ComponentSize[ (INT32) m_Format.GetBlendWeightType()];

		//	for( j = 0; j < (UINT32) m_PersistFormat.GetBlendWeightCount() - 1; j++)
		//	{
		//		x = _REAL_F2R( m_Format.GetBlendWeightType(), pSrc + m_OffsetWeight + (j * ssz));
		//	}
		//}

		I3TRACE( "\n" );

		pSrc += m_Stride;
	}

	Unlock();
}


#endif
