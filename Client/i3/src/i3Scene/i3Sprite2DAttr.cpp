#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3Sprite2DAttr.h"
#include "i3SpriteAttr.h"

//I3_CLASS_INSTANCE( i3Sprite2DAttr, i3RenderAttr);
I3_CLASS_INSTANCE( i3Sprite2DAttr);

#if defined( I3G_PSP)
	#define			INDEXED_IMPLEMENT	0
#else
	#define			INDEXED_IMPLEMENT	0
#endif

#if defined( I3G_PSP)
	#define			SUPPORT_TRANSFORMED			1
#else
	#define			SUPPORT_TRANSFORMED			0
#endif

#define			USE_DOUBLE_BUFFER				0

#define			SPRITE_USAGE			(I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE)
#define			SPRITE_LOCK				(I3G_LOCK_NOOVERWRITE)

i3Sprite2DAttr::i3Sprite2DAttr(void)
{
	SetID( I3_ATTRID_SPRITE2D);
}

i3Sprite2DAttr::~i3Sprite2DAttr(void)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);

	I3MEM_SAFE_FREE_POINTER(m_pSprites);
}

bool i3Sprite2DAttr::Create( UINT32 Count, bool bUV, bool bColor, bool bUseVertexBank)
{
	m_bUseVertexBank = bUseVertexBank;
	m_Count = Count;

	// Sprite Information
	{
		UINT32 MemSize = sizeof(I3SG_SPRITE2D_INFO) * m_Count;
		
		m_pSprites		= (I3SG_SPRITE2D_INFO *) i3MemAlloc( MemSize );
		I3ASSERT( m_pSprites != nullptr);

		memset( m_pSprites, 0, MemSize );
	}

	// Vertex format
	{
		m_format.SetHasPosition( true);
		m_format.SetHasColor( (bColor == true));
		
		if( bUV)
		{
			m_format.SetTextureCoordSetCount( 1);
			m_format.SetTextureCoordCount( 2);
		}

		#if SUPPORT_TRANSFORMED
			m_format.SetIsTransformed( true);
		#endif
	}

	_CreateVertex();

	m_bEditted = true;

	return true;
}

void i3Sprite2DAttr::_CreateVertex(void)
{
	if( !m_bUseVertexBank)
	{
		m_pVA = i3VertexArray::new_object();	

		#if INDEXED_IMPLEMENT
			m_HalfVertexCount = m_Count * 4;

			m_pVA->Create( &m_format, m_HalfVertexCount * 2, SPRITE_USAGE);

			m_pVA->setRestoreType( I3G_RESTORE_MEM);

			m_pIA = i3IndexArray::new_object();

			if( (m_Count * 6 * 2) > 0xFFFF)
					m_pIA->SetDataType( I3G_IDX_32BITS);

			m_pIA->Create( m_Count * 6 * 2, 0);

			m_pIA->setRestoreType( I3G_RESTORE_MEM);

			{
				UINT32 i;
				UINT16 Indices[6];

				m_pIA->Lock( 0, 0, SPRITE_LOCK);

				for( i = 0; i < (m_Count << 1); i ++)
				{
					Indices[0] = (UINT16)((i * 4));
					Indices[1] = (UINT16)((i * 4) + 2);
					Indices[2] = (UINT16)((i * 4) + 1);
					Indices[3] = (UINT16)((i * 4) + 1);
					Indices[4] = (UINT16)((i * 4) + 2);
					Indices[5] = (UINT16)((i * 4) + 3);

					m_pIA->SetIndices16( (i * 6), 6, Indices);
				}

				m_pIA->Unlock();
			}
		#else
			m_HalfVertexCount = m_Count * 6;

			#if USE_DOUBLE_BUFFER
				m_pVA->Create( &m_format, m_HalfVertexCount * 2, SPRITE_USAGE);
			#else
				m_pVA->Create( &m_format, m_HalfVertexCount, SPRITE_USAGE);
			#endif
		#endif
	}
}

void i3Sprite2DAttr::SetEnable( bool bFlag)
{
	I3ASSERT( m_pSprites != nullptr);

	UINT32 i;
	for( i = 0; i < m_Count; i++ )
	{
		if( m_pSprites[i].m_bEnable != bFlag )
		{
			m_pSprites[i].m_bEnable = bFlag;
			m_pSprites[i].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;
		}
	}

	m_bEditted = true;
}

void i3Sprite2DAttr::SetEnable( UINT32 Index, bool bFlag)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	if( m_pSprites[Index].m_bEnable != bFlag )
	{
		m_pSprites[Index].m_bEnable = bFlag;
		m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;
		m_bEditted = true;
	}
}

void i3Sprite2DAttr::SetCenter( UINT32 Index, REAL32 x, REAL32 y, REAL32 z)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Set( & (m_pSprites[Index].m_Center), x, y, z);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetCenter( UINT32 Index, VEC3D * pVec)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Copy( & (m_pSprites[Index].m_Center), pVec);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

void i3Sprite2DAttr::GetLeftTop( UINT32 Index, VEC3D * pVec)
{
	REAL32 sx, sy;

	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	I3SG_SPRITE2D_INFO * pSprite = & (m_pSprites[Index]);

	#if defined( I3_FIXED_POINT)
		sx = pSprite->m_Width >> 1;
		sy = pSprite->m_Height >> 1;
	#else
		sx = pSprite->m_Width * 0.5f;
		sy = pSprite->m_Height * 0.5f;
	#endif

	i3Vector::Set( pVec,	i3Vector::GetX( & pSprite->m_Center) - sx,
							i3Vector::GetY( & pSprite->m_Center) - sy,
							i3Vector::GetZ( & pSprite->m_Center));
}

void i3Sprite2DAttr::SetLeftTop( UINT32 Index, VEC3D * pVec)
{
	REAL32 sx, sy;

	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	I3SG_SPRITE2D_INFO * pSprite = & (m_pSprites[Index]);

	#if defined( I3_FIXED_POINT)
		sx = pSprite->m_Width >> 1;
		sy = pSprite->m_Height >> 1;
	#else
		sx = pSprite->m_Width * 0.5f;
		sy = pSprite->m_Height * 0.5f;
	#endif

	i3Vector::Set( &pSprite->m_Center,	i3Vector::GetX( pVec) + sx,
										i3Vector::GetY( pVec) + sy,
										i3Vector::GetZ( pVec) );
}

void i3Sprite2DAttr::GetRightBottom( UINT32 Index, VEC3D * pVec)
{
	REAL32 sx, sy;

	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	I3SG_SPRITE2D_INFO * pSprite = &(m_pSprites[Index]);

	#if defined( I3_FIXED_POINT)
		sx = pSprite->m_Width >> 1;
		sy = pSprite->m_Height >> 1;
	#else
		sx = pSprite->m_Width * 0.5f;
		sy = pSprite->m_Height * 0.5f;
	#endif

	i3Vector::Set( pVec,	i3Vector::GetX( & pSprite->m_Center) + sx,
							i3Vector::GetY( & pSprite->m_Center) + sy,
							i3Vector::GetZ( & pSprite->m_Center));
}

void i3Sprite2DAttr::SetRotation( UINT32 Index, REAL32 rotation)
{
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_Rotation = rotation;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_ROTATION | I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetFlip( UINT32 Index, bool FlipX, bool FlipY )
{
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_bFlipX = FlipX;
	m_pSprites[Index].m_bFlipY = FlipY;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_FLIP;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetSize( UINT32 Index, VEC2D* pVec)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_Width = i3Vector::GetU( pVec);
	m_pSprites[Index].m_Height = i3Vector::GetV( pVec);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetSize( UINT32 Index, REAL32 Width, REAL32 Height)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_Width = Width;
	m_pSprites[Index].m_Height = Height;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

VEC2D * i3Sprite2DAttr::GetSize( UINT32 Index)
{
	I3ASSERT( Index < m_Count);

	return (VEC2D *) & m_pSprites[Index].m_Width;
}

void i3Sprite2DAttr::SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Copy( & (m_pSprites[Index].m_UV[0]), pMin);
	i3Vector::Copy( & (m_pSprites[Index].m_UV[1]), pMax);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_UV;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Set( & (m_pSprites[Index].m_UV[0]), u1, v1);
	i3Vector::Set( & (m_pSprites[Index].m_UV[1]), u2, v2);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_UV;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetColor( COLOR * pColor)
{
	UINT32 i;
	for( i = 0; i < m_Count; i++ )
	{
		i3Color::Set( & (m_pSprites[i].m_Color), pColor);
		m_pSprites[i].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;
	}
	m_bEditted = true;
}

void i3Sprite2DAttr::SetColor( UINT32 Index, COLOR * pColor)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( & (m_pSprites[Index].m_Color), pColor);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( &(m_pSprites[Index].m_Color), r, g, b, a);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;

	m_bEditted = true;
}

void i3Sprite2DAttr::SetColor( UINT32 Index, UINT32 rgba)
{
	I3ASSERT( Index < m_Count);

	((I3UNIONCOLOR *) &(m_pSprites[Index].m_Color))->value = rgba;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;

	m_bEditted = true;
}

BEGIN_INLINE
void _setPositions( i3VertexArray * pVA, I3SG_SPRITE2D_INFO * pSprite, UINT32 vIdx,
					REAL32	rcx, REAL32 rcy)
{
	REAL32 cx, cy;
	VEC3D vecCenter;
	VEC3D *pCenter = &vecCenter;
	VEC3D _pos[6];

	i3Vector::Copy( pCenter, &pSprite->m_Center );

	#ifdef I3G_DX
	// DirectX에서는 최종 좌표값을 정수화 한 다음 0.5를 빼주어야 픽셀과 화면이 1:1로 대응한다.
	// 원래 최종적으로 나온 결과값으로 계산하는 게 맞지만,
	// 대략 입력된 좌표값이 정수라는 가정하에 중심좌표에서 0.5를 빼서 맞춘다.
	i3Vector::AddX( pCenter, -0.5f);	
	i3Vector::AddY( pCenter, -0.5f);
	#endif

	#if ! SUPPORT_TRANSFORMED
		i3Vector::Set( pCenter,		i3Math::Mul( i3Vector::GetX( pCenter), rcx) - 1.0f,
									-(i3Math::Mul( i3Vector::GetY( pCenter), rcy) - 1.0f),
									i3Vector::GetZ( pCenter));
	#endif

	cx = i3Math::Mul( pSprite->m_Width, 0.5f);
	cy = i3Math::Mul( pSprite->m_Height, 0.5f);

	#if ! SUPPORT_TRANSFORMED
		cx = i3Math::Mul( cx, rcx);
		cy = -i3Math::Mul( cy, rcy);
	#endif

	#if INDEXED_IMPLEMENT
		i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
		i3Vector::Set( &_pos[1], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[2], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[3], + cx, - cy, 0.0f);

		if( pSprite->m_Rotation != 0.0f)
		{
			MATRIX Rot;

			i3Matrix::SetRotateZ( &Rot, pSprite->m_Rotation);
			i3Matrix::TransformVectors( _pos, _pos, 4, &Rot);
		}
		
		i3Vector::Add( &_pos[0], &_pos[0], pCenter);
		i3Vector::Add( &_pos[1], &_pos[1], pCenter);
		i3Vector::Add( &_pos[2], &_pos[2], pCenter);
		i3Vector::Add( &_pos[3], &_pos[3], pCenter);

		//i3Vector::Make2DCoord( &_pos[0] );
		//i3Vector::Make2DCoord( &_pos[1] );
		//i3Vector::Make2DCoord( &_pos[2] );
		//i3Vector::Make2DCoord( &_pos[3] );

		pVA->SetPositions( vIdx, 4, _pos);
	#else
		i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
		i3Vector::Set( &_pos[1], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[2], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[3], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[4], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[5], + cx, - cy, 0.0f);

		if( pSprite->m_Rotation != 0.0f)
		{
			MATRIX Rot;

			i3Matrix::SetRotateZ( &Rot, pSprite->m_Rotation);
			i3Matrix::TransformVectors( _pos, _pos, 6, &Rot);
		}

		i3Vector::Add( &_pos[0], &_pos[0], pCenter);
		i3Vector::Add( &_pos[1], &_pos[1], pCenter);
		i3Vector::Add( &_pos[2], &_pos[2], pCenter);
		i3Vector::Add( &_pos[3], &_pos[3], pCenter);
		i3Vector::Add( &_pos[4], &_pos[4], pCenter);
		i3Vector::Add( &_pos[5], &_pos[5], pCenter);

		//i3Vector::Make2DCoord( &_pos[0] );
		//i3Vector::Make2DCoord( &_pos[1] );
		//i3Vector::Make2DCoord( &_pos[2] );
		//i3Vector::Make2DCoord( &_pos[3] );
		//i3Vector::Make2DCoord( &_pos[4] );
		//i3Vector::Make2DCoord( &_pos[5] );

		pVA->SetPositions( vIdx, 6, _pos);
	#endif
}
END_INLINE

BEGIN_INLINE
void _setPositionsPixel( i3VertexArray * pVA, I3SG_SPRITE2D_INFO * pSprite, UINT32 vIdx)
{
	REAL32 cx, cy;
	VEC3D vecCenter;
	VEC3D *pCenter = &vecCenter;
	VEC3D _pos[6];

	i3Vector::Copy( pCenter, &pSprite->m_Center );

	#ifdef I3G_DX
	// DirectX에서는 최종 좌표값을 정수화 한 다음 0.5를 빼주어야 픽셀과 화면이 1:1로 대응한다.
	// 원래 최종적으로 나온 결과값으로 계산하는 게 맞지만,
	// 대략 입력된 좌표값이 정수라는 가정하에 중심좌표에서 0.5를 빼서 맞춘다.
	i3Vector::AddX( pCenter, -0.5f);	
	i3Vector::AddY( pCenter, -0.5f);
	#endif

	cx = i3Math::Mul( pSprite->m_Width, 0.5f);
	cy = i3Math::Mul( pSprite->m_Height, 0.5f);

	#if INDEXED_IMPLEMENT
		i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
		i3Vector::Set( &_pos[1], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[2], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[3], + cx, - cy, 0.0f);

		if( pSprite->m_Rotation != 0.0f)
		{
			MATRIX Rot;

			i3Matrix::SetRotateZ( &Rot, pSprite->m_Rotation);
			i3Matrix::TransformVectors( _pos, _pos, 4, &Rot);
		}
		
		i3Vector::Add( &_pos[0], &_pos[0], pCenter);
		i3Vector::Add( &_pos[1], &_pos[1], pCenter);
		i3Vector::Add( &_pos[2], &_pos[2], pCenter);
		i3Vector::Add( &_pos[3], &_pos[3], pCenter);

		pVA->SetPositions( vIdx, 4, _pos);
	#else
		i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
		i3Vector::Set( &_pos[1], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[2], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[3], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[4], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[5], + cx, - cy, 0.0f);

		if( pSprite->m_Rotation != 0.0f)
		{
			MATRIX Rot;

			i3Matrix::SetRotateZ( &Rot, pSprite->m_Rotation);
			i3Matrix::TransformVectors( _pos, _pos, 6, &Rot);
		}

		i3Vector::Add( &_pos[0], &_pos[0], pCenter);
		i3Vector::Add( &_pos[1], &_pos[1], pCenter);
		i3Vector::Add( &_pos[2], &_pos[2], pCenter);
		i3Vector::Add( &_pos[3], &_pos[3], pCenter);
		i3Vector::Add( &_pos[4], &_pos[4], pCenter);
		i3Vector::Add( &_pos[5], &_pos[5], pCenter);

		pVA->SetPositions( vIdx, 6, _pos);
	#endif
}
END_INLINE

BEGIN_INLINE
void _setUV( i3VertexArray * pVA, I3SG_SPRITE2D_INFO * pInfo, UINT32 vIdx)
{
	REAL32 u1, v1, u2, v2;

	//// 3D와 2D는 Y축이 뒤집혀 있으므로, 그에 대한 처리를 해줍니다.
	bool bFlipY = !pInfo->m_bFlipY;

	// Flip Flag가 설정되어 있다면, 맵핑위치를 변경합니다.
	if( pInfo->m_bFlipX )  
	{
		if( bFlipY)
		{
			u1 = i3Vector::GetU( &pInfo->m_UV[1]);	v1 = i3Vector::GetV( &pInfo->m_UV[1]);
			u2 = i3Vector::GetU( &pInfo->m_UV[0]);	v2 = i3Vector::GetV( &pInfo->m_UV[0]);
		}
		else
		{
			u1 = i3Vector::GetU( &pInfo->m_UV[1]);	v1 = i3Vector::GetV( &pInfo->m_UV[0]);
			u2 = i3Vector::GetU( &pInfo->m_UV[0]);	v2 = i3Vector::GetV( &pInfo->m_UV[1]);
		}	
	}
	else
	{
		if( bFlipY)
		{
			u1 = i3Vector::GetU( &pInfo->m_UV[0]);	v1 = i3Vector::GetV( &pInfo->m_UV[1]);
			u2 = i3Vector::GetU( &pInfo->m_UV[1]);	v2 = i3Vector::GetV( &pInfo->m_UV[0]);
		}
		else
		{
			u1 = i3Vector::GetU( &pInfo->m_UV[0]);	v1 = i3Vector::GetV( &pInfo->m_UV[0]);
			u2 = i3Vector::GetU( &pInfo->m_UV[1]);	v2 = i3Vector::GetV( &pInfo->m_UV[1]);
		}	
	}

	#if INDEXED_IMPLEMENT
	{
		VEC2D _uv[4];

		i3Vector::Set( &_uv[0], u1, v1);
		i3Vector::Set( &_uv[1], u2, v1);
		i3Vector::Set( &_uv[2], u1, v2);
		i3Vector::Set( &_uv[3], u2, v2);

		pVA->SetTextureCoords( 0, vIdx, 4, _uv);
	}
	#else
	{
		VEC2D _uv[6];

		i3Vector::Set( &_uv[0], u1, v1);
		i3Vector::Set( &_uv[1], u1, v2);
		i3Vector::Set( &_uv[2], u2, v1);
		i3Vector::Set( &_uv[3], u2, v1);
		i3Vector::Set( &_uv[4], u1, v2);
		i3Vector::Set( &_uv[5], u2, v2);

		pVA->SetTextureCoords( 0, vIdx, 6, _uv);
	}
	#endif
}
END_INLINE

BEGIN_INLINE
void _setColor( i3VertexArray * pVA, I3SG_SPRITE2D_INFO * pSprite, UINT32 vIdx)
{
	#if INDEXED_IMPLEMENT
		pVA->SetColor( vIdx, 4, &pSprite->m_Color);
	#else
		pVA->SetColor( vIdx, 6, &pSprite->m_Color);
	#endif
}
END_INLINE

void i3Sprite2DAttr::ConnectVertexBank( UINT32 nCount)
{	
	m_HalfVertexCount = nCount * 6;
	m_pVBElement = m_pContext->getVertexBank()->GetVertexArray( &m_format, m_HalfVertexCount);
}

void i3Sprite2DAttr::CheckVisibleCount( UINT32 * pCount)
{
	UINT32 i = 0;	
	I3SG_SPRITE2D_INFO * pCheckSprite = m_pSprites;
	
	for( i = 0; i < m_Count; i++, pCheckSprite++)
	{
		if( pCheckSprite->m_bEnable)
		{
			//if( i3Color::GetA( &pCheckSprite->m_Color) > 0 || !m_format.GetHasColor())
			{
				(*pCount)++;					
			}
		}
	}

	CheckHideSprite();
}

void i3Sprite2DAttr::CheckHideSprite(void)
{

}

void i3Sprite2DAttr::Apply( i3RenderContext * pContext)
{

#if USE_DOUBLE_BUFFER
	UINT32 oldOE;
#endif
	UINT32 i, vIdx = 0;
	I3SG_SPRITE2D_INFO * pSprite = m_pSprites;
	REAL32 rcx = 0, rcy = 0;
	bool bLocked = false;
	UINT32 nStartIdx = 0;
	UINT32 nDrawIdx = 0;
	UINT32 DrawCount = 0;

	m_pContext = pContext;

	if(m_Count <= 0)
		return;

	if( m_bUseVertexBank) 
	{
		CheckVisibleCount( &DrawCount);
		
		// Connect VertexBank!!!
		if( DrawCount > 0)
		{
			m_bEditted = true;
			ConnectVertexBank( DrawCount);
			I3_REF_CHANGE( m_pVA, m_pVBElement->m_pVA);
			nStartIdx = m_pVBElement->m_nCurIdx;
			vIdx = m_pVBElement->m_nCurIdx;
			nDrawIdx = m_pVBElement->m_nCurIdx;
		}
	}
	else
	{
		#if USE_DOUBLE_BUFFER
			oldOE = m_OddEven;
		#endif

		if( m_bEditted)
		{
		#if USE_DOUBLE_BUFFER
			m_OddEven ^= 1;
			oldOE = m_OddEven;
			nStartIdx = (m_HalfVertexCount * m_OddEven);
			vIdx = m_HalfVertexCount * m_OddEven;			
			nDrawIdx = oldOE * m_HalfVertexCount;
		#else
			nStartIdx = 0;
			vIdx = 0;			
			nDrawIdx = 0;
		#endif			
		}		
	}

	if( m_bEditted)
	{
		m_PrimCount = 0;
		
		for( i = 0; i < m_Count; i++, pSprite++)
		{			
			//if( pSprite->m_bEnable && (i3Color::GetA( &pSprite->m_Color) > 0 || !m_format.GetHasColor()) )
			if( pSprite->m_bEnable)
			{
				if( !bLocked)
				{
					bLocked = true;
					
					if( m_bUseVertexBank) 
					{
						if( nStartIdx == 0)
						{
							m_pVA->Lock( I3G_LOCK_OVERWRITE, nStartIdx, m_HalfVertexCount);
						}
						else
						{
							m_pVA->Lock( SPRITE_LOCK, nStartIdx, m_HalfVertexCount);
						}
					}
					else
					{
						m_pVA->Lock( SPRITE_LOCK, nStartIdx, m_HalfVertexCount);
					}

					// 현재 Scope관련 문제가 발생하여 이전으로 돌려 놓습니다.
					// 차후 Scope 따로 RenderTarget을 생성하는 방향으로 수정 해야 할 듯 싶다는 의견입니다.
					// Jinsik.kim(2015.04.07)
					i3RenderTarget * pTarget = pContext->GetRenderTarget(0);

					{
						rcx = i3Math::div( 1.0f, i3Math::Mul((REAL32)pTarget->GetWidth(), 0.5f));
						rcy = i3Math::div( 1.0f, i3Math::Mul((REAL32)pTarget->GetHeight(), 0.5f));
					}

					//REAL32 scrW = (REAL32) pContext->GetCurrentRTWidth();
					//REAL32 scrH = (REAL32) pContext->GetCurrentRTHeight();

					//{
					//	rcx = 1.0f / (scrW * 0.5f);
					//	rcy = 1.0f / (scrH * 0.5f);
					//}
				}

				if( m_CoordType == I3_SPRITE2D_COORD_UNIFORM)
				{
					_setPositions( m_pVA, pSprite, vIdx, rcx, rcy);
				}
				else
				{
					_setPositionsPixel( m_pVA, pSprite, vIdx);
				}
				
				// UV
				if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
				{
					_setUV( m_pVA, pSprite, vIdx);
				}

				// Color
				if( m_pVA->GetFormat()->GetHasColor())
				{
					_setColor( m_pVA, pSprite, vIdx);
				}

				m_PrimCount++;

				#if INDEXED_IMPLEMENT
					vIdx += 4;
				#else
					vIdx += 6;
				#endif
			}

			pSprite->m_EditMask = 0;
		}

		m_bEditted = false;
	}

	if( m_PrimCount > 0)
	{	
		if( bLocked)
			m_pVA->Unlock();

		pContext->SetVertexArray( m_pVA);

		#if INDEXED_IMPLEMENT

			pContext->SetIndexArray( m_pIA);
			#if USE_DOUBLE_BUFFER
				m_pShader = pContext->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, (oldOE * 6 * m_Count), (m_PrimCount << 1));
			#else
				m_pShader = pContext->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, 0, (m_PrimCount << 1));
			#endif
		#else
			m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, nDrawIdx, (m_PrimCount << 1));
		#endif
	}
}

void i3Sprite2DAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED SPRITE2D_HEADER
	{
		UINT8			m_bUV = 0;
		UINT8			m_bColor = 0;
		UINT32			m_Count = 0;
	};

	struct PACKED SPRITE2D_INFO
	{
		i3::pack::VEC3D		m_Center;
		REAL32				m_Width = 0.0f;
		REAL32				m_Height = 0.0f;
		REAL32				m_Rotation = 0.0f;
		i3::pack::VEC2D		m_MinUV;
		i3::pack::VEC2D		m_MaxUV;
		UINT32				m_Color = 0;
		UINT8				m_bEnable = 0;
		UINT8				m_Reserved[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Sprite2DAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SPRITE2D_INFO		Info;
	UINT32 i;
	I3SG_SPRITE2D_INFO * pSprite = m_pSprites;

	// Header
	{
		pack::SPRITE2D_HEADER		Header;

		Header.m_bUV			= (UINT8) (m_pVA->GetFormat()->GetTextureCoordSetCount() > 0);
		Header.m_bColor			= (UINT8) m_pVA->GetFormat()->GetHasColor();
		Header.m_Count			= m_Count;

		Rc = pStream->Write( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Sprite2DAttr::OnSave()", "Could not write Sprite header.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Info.m_Center.x = i3Vector::GetX( &pSprite->m_Center);
		Info.m_Center.y = i3Vector::GetY( &pSprite->m_Center);
		Info.m_Center.z = i3Vector::GetZ( &pSprite->m_Center);

		Info.m_Width = pSprite->m_Width;
		Info.m_Height = pSprite->m_Height;

		Info.m_Rotation = pSprite->m_Rotation;

		Info.m_MinUV.x = i3Vector::GetU( &pSprite->m_UV[0]);
		Info.m_MinUV.y = i3Vector::GetV( &pSprite->m_UV[0]);
		Info.m_MaxUV.x = i3Vector::GetU( &pSprite->m_UV[1]);
		Info.m_MaxUV.y = i3Vector::GetV( &pSprite->m_UV[1]);

		Info.m_bEnable = (UINT8) pSprite->m_bEnable;

		Info.m_Color = i3Color::ConvertRGBA32( & pSprite->m_Color);

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Sprite2DAttr::OnSave()", "Could not write Sprite information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3Sprite2DAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Sprite2DAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SPRITE2D_INFO		Info;
	UINT32 i;

	// Header
	{
		pack::SPRITE2D_HEADER		Header;

		Rc = pStream->Read( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Sprite2DAttr::OnLoad()", "Could not read Sprite header.");
			return STREAM_ERR;
		}
		Result += Rc;

		Create( Header.m_Count, Header.m_bUV != 0, Header.m_bColor != 0);
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Rc = pStream->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Sprite2DAttr::OnLoad()", "Could not read Sprite information.");
			return STREAM_ERR;
		}
		Result += Rc;

		SetCenter( i, Info.m_Center.x, Info.m_Center.y, Info.m_Center.z);
		SetSize( i, Info.m_Width, Info.m_Height);
		SetRotation( i, Info.m_Rotation);


		SetEnable( i, Info.m_bEnable != 0);

		if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			SetTextureCoord( i, Info.m_MinUV.x, Info.m_MinUV.y, Info.m_MaxUV.x, Info.m_MaxUV.y);
		}

		if( m_pVA->GetFormat()->GetHasColor())
		{
			COLOR col;

			i3Color::ConvertRGBA32( Info.m_Color, &col);

			SetColor( i, &col);
		}
	}

	return Result;
}

void i3Sprite2DAttr::OnLostDevice( bool bLostDevice)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);

	m_pShader = nullptr;
}

void i3Sprite2DAttr::OnRevive( i3RenderContext * pCtx)
{
	_CreateVertex();

	m_bEditted = true;
}
