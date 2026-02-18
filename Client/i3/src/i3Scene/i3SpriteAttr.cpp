#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3SpriteAttr.h"

I3_CLASS_INSTANCE( i3SpriteAttr);

#if defined( I3G_PSP)
	#define			INDEXED_IMPLEMENT	0
#else
	#define			INDEXED_IMPLEMENT	1
#endif

#define			SPRITE_USAGE			(I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE)
#define			SPRITE_LOCK				(I3G_LOCK_NOOVERWRITE)

i3SpriteAttr::i3SpriteAttr(void)
{
	SetID( I3_ATTRID_SPRITE);
}

i3SpriteAttr::~i3SpriteAttr(void)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);

	I3MEM_SAFE_FREE_POINTER( m_pSprites);
}

void i3SpriteAttr::setFaceToCameraEnable( bool bFlag)
{
	m_bFaceToCamera = bFlag;

	if( bFlag == false)
	{
		i3Matrix::Identity( &m_InvViewMatrix);
	}
}

bool i3SpriteAttr::Create( UINT32 Count, bool bUV, bool bColor, bool bNormalMap, bool bAlwaysUpdate)
{
	m_Count = Count;
	m_bAlwaysUpdate = false;

	// Sprite Information
	{
		UINT32 MemSize = sizeof(I3SG_SPRITE_INFO) * m_Count;
		
		m_pSprites		= (I3SG_SPRITE_INFO *) i3MemAlloc( MemSize );
		I3ASSERT( m_pSprites );

		memset( m_pSprites, 0, MemSize );
	}

	// Vertex format
	{
		m_Format.SetHasPosition( true);
		if( bColor)
			m_Format.SetHasColor( true);			
		
		if( bUV)
		{
			m_Format.SetTextureCoordSetCount( 1);
			m_Format.SetTextureCoordCount( 2);

			m_Format.SetHasNormal( bNormalMap);
			m_Format.SetHasTangent( bNormalMap);
			m_Format.SetHasBinormal( bNormalMap);
		}
	}

	_CreateVertex();

	m_bEditted = true;

	return true;
}

void i3SpriteAttr::_CreateVertex(void)
{
	m_pVA = i3VertexArray::new_object();

	if( m_bAlwaysUpdate)
	{
		#if INDEXED_IMPLEMENT
			// Vertex Array
			// 한 Sprite 당 4개의 Vertex를 필요로 한다.
			m_pVA->Create( &m_Format, (m_Count << 2), SPRITE_USAGE );

			// Index Array
			{
				m_pIA = i3IndexArray::new_object();

				if( (m_Count << 2) > 0xFFFF)
				{
					m_pIA->Create( (m_Count * 6) , SPRITE_USAGE, I3G_IDX_32BITS);
				}
				else
				{
					m_pIA->Create( (m_Count * 6) , SPRITE_USAGE, I3G_IDX_16BITS);
				}
			}
		#else
			m_pVA->Create( &m_Format, (m_Count * 6), SPRITE_USAGE);
		#endif
	}
	else
	{
		// 변경된 경우에만 Update하는 동작 모드에서는 Vertex Buffer를 Sprite의 개수만큼
		// 미리 준비해둔다.

		#if INDEXED_IMPLEMENT		
			// Vertex Array
			{
				m_pVA->Create( &m_Format, m_Count * 4, SPRITE_USAGE);
			}

			// Index Array
			{
				m_pIA = i3IndexArray::new_object();
				m_pIA->setRestoreType( I3G_RESTORE_MEM);

				if( (m_Count * 4) > 0xFFFF)
				{
					m_pIA->Create( m_Count * 6, SPRITE_USAGE, I3G_IDX_32BITS);
				}
				else
				{
					m_pIA->Create( m_Count * 6, SPRITE_USAGE, I3G_IDX_16BITS);
				}
			}
		#else
			m_pVA->Create( &m_Format, m_Count * 6, SPRITE_USAGE);
		#endif
	}
}

void i3SpriteAttr::SetEnable( bool bFlag)
{
	I3ASSERT( m_pSprites != nullptr);

	UINT32 i;
	for( i = 0; i < m_Count; i++ )
	{
		if( m_pSprites[i].m_bEnable != bFlag )
		{
			m_pSprites[i].m_bEnable = bFlag;
			m_bEditted = true;
		}
	}
}

void i3SpriteAttr::SetEnable( UINT32 Index, bool bFlag)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	if( m_pSprites[Index].m_bEnable != bFlag )
	{
		m_pSprites[Index].m_bEnable = bFlag;
		m_bEditted = true;
	}
}

void i3SpriteAttr::SetCenter( UINT32 Index, REAL32 x, REAL32 y, REAL32 z)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Set( & m_pSprites[Index].m_Center, x, y, z);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

void i3SpriteAttr::SetCenter( UINT32 Index, VEC3D * pVec)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Copy( & m_pSprites[Index].m_Center, pVec);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

void i3SpriteAttr::GetLeftTop( UINT32 Index, VEC3D * pVec)
{
	REAL32 sx, sy;

	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	I3SG_SPRITE_INFO * pSprite = & m_pSprites[Index];

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

void i3SpriteAttr::GetRightBottom( UINT32 Index, VEC3D * pVec)
{
	REAL32 sx, sy;

	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	I3SG_SPRITE_INFO * pSprite = & m_pSprites[Index];

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

void i3SpriteAttr::SetRotation( UINT32 Index, REAL32 rotation)
{
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_Rotation = rotation;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_ROTATION;

	m_bEditted = true;
}

void i3SpriteAttr::SetRotationX(UINT32 Index, REAL32 rotation)
{
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_RotationX = rotation;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_ROTATION;

	m_bEditted = true;
}

void i3SpriteAttr::SetRotationY(UINT32 Index, REAL32 rotation)
{
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_RotationY = rotation;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_ROTATION;

	m_bEditted = true;
}

void i3SpriteAttr::SetFlip( UINT32 Index, bool FlipX, bool FlipY )
{
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_bFlipX = FlipX;
	m_pSprites[Index].m_bFlipY = FlipY;

	m_bEditted = true;
}

void i3SpriteAttr::SetSize( UINT32 Index, VEC2D* pVec)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_Width = i3Vector::GetU( pVec);
	m_pSprites[Index].m_Height = i3Vector::GetV( pVec);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

void i3SpriteAttr::SetSize( UINT32 Index, REAL32 Width, REAL32 Height)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	m_pSprites[Index].m_Width = Width;
	m_pSprites[Index].m_Height = Height;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_POSITION;

	m_bEditted = true;
}

VEC2D * i3SpriteAttr::GetSize( UINT32 Index)
{
	return (VEC2D *) &m_pSprites[Index].m_Width;
}

void i3SpriteAttr::SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Copy( & m_pSprites[Index].m_UV[0], pMin);
	i3Vector::Copy( & m_pSprites[Index].m_UV[1], pMax);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_UV;

	m_bEditted = true;
}

void i3SpriteAttr::SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Set( & m_pSprites[Index].m_UV[0], u1, v1);
	i3Vector::Set( & m_pSprites[Index].m_UV[1], u2, v2);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_UV;

	m_bEditted = true;
}

void i3SpriteAttr::SetColor( COLOR * pColor)
{
	UINT32 i;
	for( i = 0; i < m_Count; i++ )
	{
		i3Color::Set( & m_pSprites[i].m_Color, pColor);
		m_pSprites[i].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;
	}
	m_bEditted = true;
}

void i3SpriteAttr::SetColor( UINT32 Index, COLOR * pColor)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( & m_pSprites[Index].m_Color, pColor);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;

	m_bEditted = true;
}
void i3SpriteAttr::SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( & m_pSprites[Index].m_Color, r, g, b, a);
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;

	m_bEditted = true;
}

void i3SpriteAttr::SetColor( UINT32 Index, UINT32 rgba)
{
	I3ASSERT( Index< m_Count);

	((I3UNIONCOLOR *) &m_pSprites[Index].m_Color)->value = rgba;
	m_pSprites[Index].m_EditMask |= I3SG_SPRITE_EDIT_COLOR;

	m_bEditted = true;
}

inline
void _setPositions( i3VertexArray * pVA, I3SG_SPRITE_INFO * pSprite, UINT32 vIdx, MATRIX* pMat)
{
	REAL32 cx, cy;
	VEC3D *pCenter = &pSprite->m_Center;
	VEC3D norm;
	
	cx = i3Math::Mul( pSprite->m_Width, 0.5f);
	cy = i3Math::Mul( pSprite->m_Height, 0.5f);

	#if INDEXED_IMPLEMENT
		VEC3D _pos[4];
		
		// 3DMAX
		i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
		i3Vector::Set( &_pos[1], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[2], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[3], + cx, - cy, 0.0f);

		i3Matrix::TransformVectors( _pos, _pos, 4, pMat);
		
		i3Vector::Add( &_pos[0], &_pos[0], pCenter);
		i3Vector::Add( &_pos[1], &_pos[1], pCenter);
		i3Vector::Add( &_pos[2], &_pos[2], pCenter);
		i3Vector::Add( &_pos[3], &_pos[3], pCenter);

		pVA->SetPositions( vIdx, 4, _pos);

		if( pVA->GetFormat()->GetHasNormal())
		{
			i3Vector::Set( &norm, 0.0f, 0.0f, 1.0f);

			i3Vector::TransformNormal( &norm, &norm, pMat);
			i3Vector::Normalize( &norm, &norm);

			pVA->SetNormal( vIdx, &norm);
			pVA->SetNormal( vIdx+1, &norm);
			pVA->SetNormal( vIdx+2, &norm);
			pVA->SetNormal( vIdx+3, &norm);
		}
	#else
		VEC3D _pos[6];

		// 3DMAX
		i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
		i3Vector::Set( &_pos[1], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[2], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[3], + cx, + cy, 0.0f);
		i3Vector::Set( &_pos[4], - cx, - cy, 0.0f);
		i3Vector::Set( &_pos[5], + cx, - cy, 0.0f);

		i3Matrix::TransformVectors( _pos, _pos, 6, pMat);

		i3Vector::Add( &_pos[0], &_pos[0], pCenter);
		i3Vector::Add( &_pos[1], &_pos[1], pCenter);
		i3Vector::Add( &_pos[2], &_pos[2], pCenter);
		i3Vector::Add( &_pos[3], &_pos[3], pCenter);
		i3Vector::Add( &_pos[4], &_pos[4], pCenter);
		i3Vector::Add( &_pos[5], &_pos[5], pCenter);

		pVA->SetPositions( vIdx, 6, _pos);

		if( pVA->GetFormat()->GetHasNormal())
		{
			i3Vector::Set( &norm, 0.0f, 0.0f, 1.0f);

			i3Vector::TransformNormal( &norm, &norm, pMat);
			i3Vector::Normalize( &norm, &norm);

			pVA->SetNormal( vIdx, &norm);
			pVA->SetNormal( vIdx+1, &norm);
			pVA->SetNormal( vIdx+2, &norm);
			pVA->SetNormal( vIdx+3, &norm);
			pVA->SetNormal( vIdx+4, &norm);
			pVA->SetNormal( vIdx+5, &norm);
		}
	#endif
}

inline
void _setUV( i3VertexArray * pVA, I3SG_SPRITE_INFO * pInfo, UINT32 vIdx)
{
	REAL32 u1, v1, u2, v2;
	VEC3D t, b, pos1, pos2, pos3;

	//// 3D와 2D는 Y축이 뒤집혀 있으므로, 그에 대한 처리를 해줍니다.
	bool bFlipY = pInfo->m_bFlipY;

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

		if( pVA->GetFormat()->GetHasTangent())
		{
			pVA->GetPosition( vIdx, &pos1);
			pVA->GetPosition( vIdx+1, &pos2);
			pVA->GetPosition( vIdx+2, &pos3);
			i3Math::CalcTangentBasis( &pos1, &pos2, &pos3, &_uv[0], &_uv[1], &_uv[2], &t, &b);

			pVA->SetTangent( vIdx+0, &t);
			pVA->SetTangent( vIdx+1, &t);
			pVA->SetTangent( vIdx+2, &t);
			pVA->SetTangent( vIdx+3, &t);

			pVA->SetBinormal( vIdx+0, &b);
			pVA->SetBinormal( vIdx+1, &b);
			pVA->SetBinormal( vIdx+2, &b);
			pVA->SetBinormal( vIdx+3, &b);
		}
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

		if( pVA->GetFormat()->GetHasTangent())
		{
			pVA->GetPosition( vIdx, &pos1);
			pVA->GetPosition( vIdx+1, &pos2);
			pVA->GetPosition( vIdx+2, &pos3);
			i3Math::CalcTangentBasis( &pos1, &pos2, &pos3, &_uv[0], &_uv[1], &_uv[2], &t, &b);

			pVA->SetTangent( vIdx+0, &t);
			pVA->SetTangent( vIdx+1, &t);
			pVA->SetTangent( vIdx+2, &t);
			pVA->SetTangent( vIdx+3, &t);
			pVA->SetTangent( vIdx+4, &t);
			pVA->SetTangent( vIdx+5, &t);

			pVA->SetBinormal( vIdx+0, &b);
			pVA->SetBinormal( vIdx+1, &b);
			pVA->SetBinormal( vIdx+2, &b);
			pVA->SetBinormal( vIdx+3, &b);
			pVA->SetBinormal( vIdx+4, &b);
			pVA->SetBinormal( vIdx+5, &b);
		}
	}
	#endif
}

inline
void _setColor( i3VertexArray * pVA, I3SG_SPRITE_INFO * pSprite, UINT32 vIdx)
{
	#if INDEXED_IMPLEMENT
		pVA->SetColor( vIdx, 4, &pSprite->m_Color);
	#else
		pVA->SetColor( vIdx, 6, &pSprite->m_Color);
	#endif
}

void i3SpriteAttr::ApplyAlwaysUpdate( i3RenderContext * pContext)
{
	UINT32 primcount, i;
	UINT32 vLockStart, vIdx;
	I3SG_SPRITE_INFO * pSprite;
	bool bLocked = false;

	pSprite = m_pSprites;

	primcount = 0;
	vLockStart = 0;
	vIdx = 0;

	#if INDEXED_IMPLEMENT
		UINT32 iLockStart, iIdx;
		UINT16	Indices[6];

		iLockStart = 0;
		iIdx = 0;		
	#endif
 
	for( i = 0; i < m_Count; i++, pSprite++)
	{
		if( pSprite->m_bEnable == false)
			continue;

		if( bLocked == false)
		{
			UINT32 count = MIN( m_Count, m_Count - i);

			#if INDEXED_IMPLEMENT
				m_pVA->Lock( SPRITE_LOCK, vLockStart, count * 4 );
				m_pIA->Lock( iLockStart, count * 6, SPRITE_LOCK);
			#else
				m_pVA->Lock( SPRITE_LOCK, vLockStart, count * 6);
			#endif

			bLocked = true;
		}

		MATRIX Mat;

		// 3DMAX
		i3Matrix::SetRotateZ( &Mat, pSprite->m_Rotation );

		if (pSprite->m_RotationX != 0.0f)
			i3Matrix::PostRotateX( &Mat, pSprite->m_RotationX);

		if (pSprite->m_RotationY != 0.0f)
			i3Matrix::PostRotateY( &Mat, pSprite->m_RotationY);

		if( m_bFaceToCamera == true)
		{
			i3Matrix::Mul( &Mat, &Mat, &m_InvViewMatrix );
		}

		_setPositions( m_pVA, pSprite, vIdx, &Mat );

		// UV
		if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			_setUV( m_pVA, pSprite, vIdx);
		}

		// Color
		if( m_pVA->GetFormat()->GetHasColor())
			_setColor( m_pVA, pSprite, vIdx);

		// Index
		#if INDEXED_IMPLEMENT
		{
			Indices[0] = (UINT16)(vIdx);
			Indices[1] = (UINT16)(vIdx + 2);
			Indices[2] = (UINT16)(vIdx + 1);
			Indices[3] = (UINT16)(vIdx + 1);
			Indices[4] = (UINT16)(vIdx + 2);
			Indices[5] = (UINT16)(vIdx + 3);

			m_pIA->SetIndices( iIdx, 6, Indices);
		}
		#endif

		
		primcount ++;

		#if INDEXED_IMPLEMENT
			vIdx += 4;
			iIdx += 6;
		#else
			vIdx += 6;
		#endif

			/*
		if( primcount >= UPDATE_BLOCK_UNIT)
		{
			m_pVA->Unlock();
			i3System::DCacheWritebackAll();
			#if INDEXED_IMPLEMENT
				m_pIA->Unlock();

				pContext->DrawIndexedPrim( I3G_PRIM_TRILIST, iLockStart, primcount << 1);
			#else
				pContext->DrawPrim( I3G_PRIM_TRILIST, vLockStart, primcount << 1);
			#endif

			primcount = 0;
			bLocked = false;

			if( vIdx >= m_pVA->GetCount())
			{
				vLockStart = 0;
				vIdx = 0;
				#if INDEXED_IMPLEMENT
					iLockStart = 0;
					iIdx = 0;
				#endif
			}
			else
			{
				vLockStart = vIdx;
				#if INDEXED_IMPLEMENT
					iLockStart = iIdx;
				#endif
			}
		}
		*/
	}

	// Render rest of Sprites
	if( primcount > 0)
	{
		if( bLocked)
			m_pVA->Unlock();

		pContext->SetVertexArray( m_pVA);

		#if INDEXED_IMPLEMENT
			if( bLocked)
				m_pIA->Unlock();

			pContext->SetIndexArray( m_pIA);

			m_pShader = pContext->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, iLockStart, primcount << 1);
		#else
			m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, vLockStart, primcount << 1);
		#endif
	}
}

void i3SpriteAttr::ApplyEdittedUpdate( i3RenderContext * pContext)
{
	UINT32 i, vIdx;
	I3SG_SPRITE_INFO * pSprite = m_pSprites;
	bool bLocked = false;

	#if INDEXED_IMPLEMENT
		UINT16	Indices[6];
		UINT32 iIdx;
	#endif

	if(m_Count <= 0)
		return;

	if( m_bEditted)
	{
		m_PrimCount = 0;
		vIdx = 0;
		iIdx = 0;

		for( i = 0; i < m_Count; i++, pSprite++)
		{
			if( pSprite->m_bEnable == false)
				continue;

			if( !bLocked)
			{
				bLocked = true;

				m_pVA->Lock( SPRITE_LOCK);

			#if INDEXED_IMPLEMENT
				m_pIA->Lock( 0, 0, SPRITE_LOCK);
			#endif
			}

			MATRIX Mat;

			// 3DMAX
			i3Matrix::SetRotateZ( &Mat, pSprite->m_Rotation);

			if (pSprite->m_RotationX != 0.0f)
				i3Matrix::PostRotateX( &Mat, pSprite->m_RotationX);

			if (pSprite->m_RotationY != 0.0f)
				i3Matrix::PostRotateY( &Mat, pSprite->m_RotationY);

			i3Matrix::Mul( &Mat, &Mat, &m_InvViewMatrix);

			_setPositions( m_pVA, pSprite, vIdx, &Mat );
			
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
			
			// Index
			#if INDEXED_IMPLEMENT
			{
				Indices[0] = (UINT16)(vIdx);
				Indices[1] = (UINT16)(vIdx + 2);
				Indices[2] = (UINT16)(vIdx + 1);
				Indices[3] = (UINT16)(vIdx + 1);
				Indices[4] = (UINT16)(vIdx + 2);
				Indices[5] = (UINT16)(vIdx + 3);

				m_pIA->SetIndices( iIdx, 6, Indices);
			}
			#endif

			m_PrimCount++;
			
			#if INDEXED_IMPLEMENT
				vIdx += 4;
				iIdx += 6;
			#else
				vIdx += 6;
			#endif
		}

		m_bEditted = false;
	}

	if( m_PrimCount > 0)
	{
		if( bLocked)
			m_pVA->Unlock();

		pContext->SetVertexArray( m_pVA);

		#if INDEXED_IMPLEMENT
			if( bLocked)
				m_pIA->Unlock();

			pContext->SetIndexArray( m_pIA);
			m_pShader = pContext->DrawIndexedPrim( m_pShader, I3G_PRIM_TRILIST, 0, m_PrimCount << 1);
		#else
			m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, 0, m_PrimCount << 1);
		#endif
	}
}

void i3SpriteAttr::Apply( i3RenderContext * pContext)
{
	I3ASSERT( m_pSprites != nullptr);
	I3ASSERT( m_pVA != nullptr);

	#if INDEXED_IMPLEMENT
	I3ASSERT( m_pIA != nullptr);
	#endif

	if( m_bAlwaysUpdate)
	{
		ApplyAlwaysUpdate( pContext);
	}
	else
	{
		ApplyEdittedUpdate( pContext);
	}
}

void i3SpriteAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED Sprite_HEADER
	{
		UINT8			m_bFaceToCamera = 0;
		UINT8			m_bAlwaysUpdate = 0;
		UINT8			m_bUV = 0;
		UINT8			m_bColor = 0;
		UINT32			m_Count = 0;
	};

	struct PACKED Sprite_INFO
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

UINT32 i3SpriteAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::Sprite_INFO		Info;
	UINT32 i;
	I3SG_SPRITE_INFO * pSprite = m_pSprites;

	// Header
	{
		pack::Sprite_HEADER		Header;

		Header.m_bFaceToCamera	= (UINT8) !getFaceToCameraEnable();
		Header.m_bAlwaysUpdate	= (UINT8) m_bAlwaysUpdate;
		Header.m_bUV			= (UINT8) (m_pVA->GetFormat()->GetTextureCoordSetCount() > 0);
		Header.m_bColor			= (UINT8) m_pVA->GetFormat()->GetHasColor();
		Header.m_Count			= m_Count;

		Rc = pStream->Write( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SpriteAttr::OnSave()", "Could not write Sprite header.");
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
			i3Log( "i3SpriteAttr::OnSave()", "Could not write Sprite information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3SpriteAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3SpriteAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::Sprite_INFO		Info;
	UINT32 i;

	// Header
	{
		pack::Sprite_HEADER		Header;

		Rc = pStream->Read( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SpriteAttr::OnLoad()", "Could not read Sprite header.");
			return STREAM_ERR; 
		}
		Result += Rc;

		Create( Header.m_Count, Header.m_bUV != 0, 
			Header.m_bColor != 0, Header.m_bAlwaysUpdate != 0);

		setFaceToCameraEnable( (Header.m_bFaceToCamera == 0));
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Rc = pStream->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3SpriteAttr::OnLoad()", "Could not read Sprite information.");
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

void i3SpriteAttr::OnLostDevice( bool bLostDevice)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);

	m_pShader = nullptr;
}

void i3SpriteAttr::OnRevive( i3RenderContext * pCtx)
{
	_CreateVertex();
}
