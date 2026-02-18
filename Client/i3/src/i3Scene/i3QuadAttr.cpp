#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3QuadAttr.h"

//I3_CLASS_INSTANCE( i3QuadAttr, i3RenderAttr);
I3_CLASS_INSTANCE( i3QuadAttr);

#if defined( I3G_PSP)
	#define			INDEXED_IMPLEMENT	0
#else
	#define			INDEXED_IMPLEMENT	0
#endif

#define	QUAD_LOCK	(I3G_LOCK_NOOVERWRITE)

i3QuadAttr::i3QuadAttr(void)
{
	SetID( I3_ATTRID_QUAD);
}

i3QuadAttr::~i3QuadAttr(void)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);
	I3MEM_SAFE_FREE_POINTER( m_pQuads ); 
}

bool i3QuadAttr::Create( UINT32 Count, I3SG_QUAD_SPACE Space, bool bUV, bool bColor, bool bAlwaysUpdate)
{
	m_Count = Count;
	m_Space = Space;
	m_bAlwaysUpdate = bAlwaysUpdate;

	// Quad Information
	{
		UINT32 MemSize = sizeof(I3SG_QUAD_INFO) * m_Count;

		m_pQuads = (I3SG_QUAD_INFO *) i3MemAlloc( MemSize );
		I3ASSERT( m_pQuads );
		memset( m_pQuads, 0, MemSize );
	}

	// Vertex m_Format
	{
		m_Format.SetHasPosition( true);
		m_Format.SetHasColor(bColor);
		
		if( bUV)
		{
			m_Format.SetTextureCoordSetCount( 1);
			m_Format.SetTextureCoordCount( 2);
		}

		if( Space == I3SG_QUAD_SPACE_SCREEN)
			m_Format.SetIsTransformed( true);
	}

	_CreateVertex();

	m_bEditted = true;

	return true;
}

void i3QuadAttr::_CreateVertex(void)
{
	m_pVA = i3VertexArray::new_object();
	m_pVA->setRestoreType( I3G_RESTORE_MEM);

	if( m_bAlwaysUpdate)
	{
		#if INDEXED_IMPLEMENT
			// Vertex Array
			{
				// 한 Quad 당 4개의 Vertex를 필요로 한다.
				m_pVA->Create( &m_Format, (m_Count * 4 * 2), (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE) );
			}

			// Index Array
			{
				m_pIA = i3IndexArray::new_object();
				m_pIA->setRestoreType( I3G_RESTORE_MEM);

				m_pIA->Create( (m_Count * 6) , (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			}
		#else
			#if defined( I3G_PSP)
				m_pVA->Create( &m_Format, (m_Count*2 * 2), (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			#else
				m_pVA->Create( &m_Format, (m_Count*6 * 2), (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			#endif
		#endif
	}
	else
	{
		// 변경된 경우에만 Update하는 동작 모드에서는 Vertex Buffer를 Quad의 개수만큼
		// 미리 준비해둔다.

		#if INDEXED_IMPLEMENT		
			// Vertex Array
			{
				m_pVA->Create( &m_Format, m_Count * 4 * 2, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			}

			// Index Array
			{
				m_pIA = i3IndexArray::new_object();
				m_pIA->setRestoreType( I3G_RESTORE_MEM);

				m_pIA->Create( m_Count * 6, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			}
		#else
			#if defined( I3G_PSP)
				m_pVA->Create( &m_Format, m_Count * 2 * 2, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			#else
				m_pVA->Create( &m_Format, m_Count * 6 * 2, (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE));
			#endif			
		#endif
	}
}

void i3QuadAttr::SetEnable( UINT32 Index, bool bFlag)
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	if( m_pQuads[Index].m_bEnable != bFlag )
	{
		m_pQuads[Index].m_bEnable = bFlag;
		m_bEditted = true;
	}
}

void i3QuadAttr::SetCenter( UINT32 Index, REAL32 x, REAL32 y, REAL32 z)
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);
	
	i3Vector::Set( & m_pQuads[Index].m_Center, x, y, z);
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_POSITION;

	m_bEditted = true;
}

void i3QuadAttr::SetCenter( UINT32 Index, VEC3D * pVec)
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Copy( & m_pQuads[Index].m_Center, pVec);
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_POSITION;

	m_bEditted = true;
}

void i3QuadAttr::GetLeftTop( UINT32 Index, VEC3D * pVec)
{
	REAL32 sx, sy;

	I3ASSERT( m_pQuads != nullptr);

	I3SG_QUAD_INFO * pQuad = & m_pQuads[Index];

	sx = i3Math::Mul( pQuad->m_Width, 0.5f);
	sy = i3Math::Mul( pQuad->m_Height, 0.5f);

	i3Vector::Set( pVec,	i3Vector::GetX( & pQuad->m_Center) - sx,
							i3Vector::GetY( & pQuad->m_Center) - sy,
							i3Vector::GetZ( & pQuad->m_Center));
}

void i3QuadAttr::GetRightBottom( UINT32 Index, VEC3D * pVec)
{
	REAL32 sx, sy;

	I3ASSERT( m_pQuads != nullptr);

	I3SG_QUAD_INFO * pQuad = & m_pQuads[Index];

	sx = i3Math::Mul( pQuad->m_Width, 0.5f);
	sy = i3Math::Mul( pQuad->m_Height, 0.5f);

	i3Vector::Set( pVec,	i3Vector::GetX( & pQuad->m_Center) + sx,
							i3Vector::GetY( & pQuad->m_Center) + sy,
							i3Vector::GetZ( & pQuad->m_Center));
}

void i3QuadAttr::SetRotation( UINT32 Index, QUAD_ROTATION Rot )
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	m_pQuads[Index].m_Rotation = Rot;

	m_bEditted = true;
}

void i3QuadAttr::SetFlip( UINT32 Index, bool FlipX, bool FlipY )
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	m_pQuads[Index].m_bFlipX = FlipX;
	m_pQuads[Index].m_bFlipY = FlipY;

	m_bEditted = true;
}

void i3QuadAttr::SetSize( UINT32 Index, VEC2D* pVec)
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	m_pQuads[Index].m_Width = i3Vector::GetU( pVec);
	m_pQuads[Index].m_Height = i3Vector::GetV( pVec);
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_POSITION;

	m_bEditted = true;
}

void i3QuadAttr::SetSize( UINT32 Index, REAL32 Width, REAL32 Height)
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	m_pQuads[Index].m_Width = Width;
	m_pQuads[Index].m_Height = Height;
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_POSITION;

	m_bEditted = true;
}

void i3QuadAttr::GetSize( UINT32 Index, VEC2D * pVec)	
{
	I3ASSERT( Index < m_Count);

	i3Vector::Set( pVec, m_pQuads[Index].m_Width, m_pQuads[Index].m_Height); 
}

void i3QuadAttr::SetTextureCoord( UINT32 Index, VEC2D * pMin, VEC2D * pMax)
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Copy( & m_pQuads[Index].m_UV[0], pMin);
	i3Vector::Copy( & m_pQuads[Index].m_UV[1], pMax);
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_UV;

	m_bEditted = true;
}

void i3QuadAttr::SetTextureCoord( UINT32 Index, REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
	I3ASSERT( m_pQuads != nullptr);
	I3ASSERT( Index < m_Count);

	i3Vector::Set( & m_pQuads[Index].m_UV[0], u1, v1);
	i3Vector::Set( & m_pQuads[Index].m_UV[1], u2, v2);
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_UV;

	m_bEditted = true;
}

void i3QuadAttr::SetColor( UINT32 Index, COLOR * pColor)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( & m_pQuads[Index].m_Color, pColor);
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_COLOR;

	m_bEditted = true;
}
void i3QuadAttr::SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( & m_pQuads[Index].m_Color, r, g, b, a);
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_COLOR;

	m_bEditted = true;
}

void i3QuadAttr::SetColor( UINT32 Index, UINT32 rgba)
{
	((I3UNIONCOLOR *) &m_pQuads[Index].m_Color)->value = rgba;
	m_pQuads[Index].m_EditMask |= I3SG_QUAD_EDIT_COLOR;

	m_bEditted = true;
}

inline
void _setPositions( i3VertexArray * pVA, I3SG_QUAD_INFO * pQuad, UINT32 vIdx,MATRIX* pMat, I3SG_QUAD_SPACE Space)
{
	REAL32 cx, cy;
	VEC3D vecCenter;
	VEC3D *pCenter = &vecCenter;

	i3Vector::Copy( &vecCenter, &pQuad->m_Center );

	cx = i3Math::Mul( pQuad->m_Width, 0.5f);
	cy = i3Math::Mul( pQuad->m_Height, 0.5f);

	#if INDEXED_IMPLEMENT
		VEC3D _pos[4];
		
		switch( pQuad->m_Rotation )
		{
		case QUAD_ROT_0:
			{
				i3Vector::Set( &_pos[0], - cx, - cy, 0.0f);
				i3Vector::Set( &_pos[1], + cx, - cy, 0.0f);
				i3Vector::Set( &_pos[2], + cx, + cy, 0.0f);
				i3Vector::Set( &_pos[3], - cx, + cy, 0.0f);
			}
			break;
		case QUAD_ROT_90:
			{
				i3Vector::Set( &_pos[0], + cx, - cy, 0.0f);
				i3Vector::Set( &_pos[1], + cx, + cy, 0.0f);
				i3Vector::Set( &_pos[2], - cx, + cy, 0.0f);
				i3Vector::Set( &_pos[3], - cx, - cy, 0.0f);
			}
			break;
		case QUAD_ROT_180:
			{
				i3Vector::Set( &_pos[0], + cx, + cy, 0.0f);
				i3Vector::Set( &_pos[1], - cx, + cy, 0.0f);
				i3Vector::Set( &_pos[2], - cx, - cy, 0.0f);
				i3Vector::Set( &_pos[3], + cx, - cy, 0.0f);
			}
			break;
		case QUAD_ROT_270:
			{
				i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
				i3Vector::Set( &_pos[1], - cx, - cy, 0.0f);
				i3Vector::Set( &_pos[2], + cx, - cy, 0.0f);
				i3Vector::Set( &_pos[3], + cx, + cy, 0.0f);
			}
			break;
		}

		i3Matrix::TransformVectors( _pos, _pos, 4, pMat);
		
		i3Vector::Add( &_pos[0], &_pos[0], pCenter);
		i3Vector::Add( &_pos[1], &_pos[1], pCenter);
		i3Vector::Add( &_pos[2], &_pos[2], pCenter);
		i3Vector::Add( &_pos[3], &_pos[3], pCenter);

		if( Space == I3SG_QUAD_SPACE_SCREEN )
		{
			i3Vector::Make2DCoord( &_pos[0] );
			i3Vector::Make2DCoord( &_pos[1] );
			i3Vector::Make2DCoord( &_pos[2] );
			i3Vector::Make2DCoord( &_pos[3] );
		}

		pVA->SetPositions( vIdx, 4, _pos);
	#else
		#if defined(I3G_PSP)
			VEC3D _pos[2];
			switch( pQuad->m_Rotation )
			{
			case QUAD_ROT_0:
				{
					i3Vector::Set( &_pos[0], - cx, - cy, 0.0f);
					i3Vector::Set( &_pos[1], + cx, + cy, 0.0f);								
				}
				break;
			case QUAD_ROT_90:
				{
					i3Vector::Set( &_pos[0], + cx, - cy, 0.0f);
					i3Vector::Set( &_pos[1], - cx, + cy, 0.0f);
				}
				break;
			case QUAD_ROT_180:
				{
					i3Vector::Set( &_pos[0], + cx, + cy, 0.0f);
					i3Vector::Set( &_pos[1], - cx, - cy, 0.0f);					
				}
				break;
			case QUAD_ROT_270:
				{
					i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
					i3Vector::Set( &_pos[1], + cx, - cy, 0.0f);
				}
				break;
			}

			i3Matrix::TransformVectors( _pos, _pos, 2, pMat);

			i3Vector::Add( &_pos[0], &_pos[0], pCenter );
			i3Vector::Add( &_pos[1], &_pos[1], pCenter );

			if( Space == I3SG_QUAD_SPACE_SCREEN )
			{
				i3Vector::Make2DCoord( &_pos[0] );
				i3Vector::Make2DCoord( &_pos[1] );
			}

			pVA->SetPositions( vIdx, 2, _pos);
		#else
			VEC3D _pos[6];

			switch( pQuad->m_Rotation )
			{
			case QUAD_ROT_0:
				{
					i3Vector::Set( &_pos[0], - cx, - cy, 0.0f);
					i3Vector::Set( &_pos[1], + cx, - cy, 0.0f);
					i3Vector::Set( &_pos[2], + cx, + cy, 0.0f);
					i3Vector::Set( &_pos[3], - cx, + cy, 0.0f);
					i3Vector::Set( &_pos[4], - cx, - cy, 0.0f);
					i3Vector::Set( &_pos[5], + cx, + cy, 0.0f);				
				}
				break;
			case QUAD_ROT_90:
				{
					i3Vector::Set( &_pos[0], + cx, - cy, 0.0f);
					i3Vector::Set( &_pos[1], + cx, + cy, 0.0f);
					i3Vector::Set( &_pos[2], - cx, + cy, 0.0f);
					i3Vector::Set( &_pos[3], - cx, - cy, 0.0f);
					i3Vector::Set( &_pos[4], + cx, - cy, 0.0f);
					i3Vector::Set( &_pos[5], - cx, + cy, 0.0f);
				}
				break;
			case QUAD_ROT_180:
				{
					i3Vector::Set( &_pos[0], + cx, + cy, 0.0f);
					i3Vector::Set( &_pos[1], - cx, + cy, 0.0f);
					i3Vector::Set( &_pos[2], - cx, - cy, 0.0f);
					i3Vector::Set( &_pos[3], + cx, - cy, 0.0f);
					i3Vector::Set( &_pos[4], + cx, + cy, 0.0f);
					i3Vector::Set( &_pos[5], - cx, - cy, 0.0f);
				}
				break;
			case QUAD_ROT_270:
				{
					i3Vector::Set( &_pos[0], - cx, + cy, 0.0f);
					i3Vector::Set( &_pos[1], - cx, - cy, 0.0f);
					i3Vector::Set( &_pos[2], + cx, - cy, 0.0f);
					i3Vector::Set( &_pos[3], + cx, + cy, 0.0f);
					i3Vector::Set( &_pos[4], - cx, + cy, 0.0f);
					i3Vector::Set( &_pos[5], + cx, - cy, 0.0f);
				}
				break;
			}
			
			i3Matrix::TransformVectors( _pos, _pos, 6, pMat);

			i3Vector::Add( &_pos[0], &_pos[0], pCenter );
			i3Vector::Add( &_pos[1], &_pos[1], pCenter );
			i3Vector::Add( &_pos[2], &_pos[2], pCenter );
			i3Vector::Add( &_pos[3], &_pos[3], pCenter );
			i3Vector::Add( &_pos[4], &_pos[4], pCenter );
			i3Vector::Add( &_pos[5], &_pos[5], pCenter );
			
			if( Space == I3SG_QUAD_SPACE_SCREEN )
			{
				i3Vector::Make2DCoord( &_pos[0] );
				i3Vector::Make2DCoord( &_pos[1] );
				i3Vector::Make2DCoord( &_pos[2] );
				i3Vector::Make2DCoord( &_pos[3] );
				i3Vector::Make2DCoord( &_pos[4] );
				i3Vector::Make2DCoord( &_pos[5] );
			}

			pVA->SetPositions( vIdx, 6, _pos);
		#endif
	#endif
}

inline
void _setUV( i3VertexArray * pVA, I3SG_QUAD_INFO * pQuad, UINT32 vIdx, I3SG_QUAD_SPACE space)
{
	REAL32 u1, v1, u2, v2;

	// 3D와 2D는 Y축이 뒤집혀 있으므로, 그에 대한 처리를 해줍니다.
	bool bFlipY = (space == I3SG_QUAD_SPACE_WORLD) ? ( ! pQuad->m_bFlipY ) : ( pQuad->m_bFlipY );

	// Flip Flag가 설정되어 있다면, 맵핑위치를 변경합니다.
	if( pQuad->m_bFlipX )  
	{
		if( bFlipY)
		{
			u1 = i3Vector::GetU( &pQuad->m_UV[1]);	v1 = i3Vector::GetV( &pQuad->m_UV[1]);
			u2 = i3Vector::GetU( &pQuad->m_UV[0]);	v2 = i3Vector::GetV( &pQuad->m_UV[0]);
		}
		else
		{
			u1 = i3Vector::GetU( &pQuad->m_UV[1]);	v1 = i3Vector::GetV( &pQuad->m_UV[0]);
			u2 = i3Vector::GetU( &pQuad->m_UV[0]);	v2 = i3Vector::GetV( &pQuad->m_UV[1]);
		}	
	}
	else
	{
		if( bFlipY)
		{
			u1 = i3Vector::GetU( &pQuad->m_UV[0]);	v1 = i3Vector::GetV( &pQuad->m_UV[1]);
			u2 = i3Vector::GetU( &pQuad->m_UV[1]);	v2 = i3Vector::GetV( &pQuad->m_UV[0]);
		}
		else
		{
			u1 = i3Vector::GetU( &pQuad->m_UV[0]);	v1 = i3Vector::GetV( &pQuad->m_UV[0]);
			u2 = i3Vector::GetU( &pQuad->m_UV[1]);	v2 = i3Vector::GetV( &pQuad->m_UV[1]);
		}	
	}

#if INDEXED_IMPLEMENT
	{
		VEC2D _uv[4];

		i3Vector::Set( &_uv[0], u1, v1);
		i3Vector::Set( &_uv[1], u2, v1);
		i3Vector::Set( &_uv[2], u2, v2);
		i3Vector::Set( &_uv[3], u1, v2);

		pVA->SetTextureCoords( 0, vIdx, 4, _uv);
	}
	#else
		#if defined(I3G_PSP)
			{
				VEC2D _uv[2];

				i3Vector::Set( &_uv[0], u1, v1);
				i3Vector::Set( &_uv[1], u2, v2);

				pVA->SetTextureCoords( 0, vIdx, 2, _uv);
			}
		#else
			{
				VEC2D _uv[6];

				i3Vector::Set( &_uv[0], u1, v1);
				i3Vector::Set( &_uv[1], u2, v1);
				i3Vector::Set( &_uv[2], u2, v2);
				i3Vector::Set( &_uv[3], u1, v2);
				i3Vector::Set( &_uv[4], u1, v1);
				i3Vector::Set( &_uv[5], u2, v2);

				pVA->SetTextureCoords( 0, vIdx, 6, _uv);
			}
		#endif
	#endif
}

inline
void _setColor( i3VertexArray * pVA, I3SG_QUAD_INFO * pQuad, UINT32 vIdx)
{
	#if INDEXED_IMPLEMENT
		pVA->SetColor( vIdx, 4, &pQuad->m_Color);
	#else
		#if defined(I3G_PSP)
			pVA->SetColor( vIdx, 2, &pQuad->m_Color);
		#else
			pVA->SetColor( vIdx, 6, &pQuad->m_Color);
		#endif
	#endif
}

void i3QuadAttr::ApplyAlwaysUpdate( i3RenderContext * pContext)
{
	UINT32 primcount, i;
	UINT32 vLockStart = 0, vIdx;
	I3SG_QUAD_INFO * pQuad;
	bool bLocked = false;

	pQuad = m_pQuads;

	m_nBufferIndex ^= 1;

	primcount = 0;

	vIdx = 0;

	#if INDEXED_IMPLEMENT
		UINT32 iLockStart, iIdx;
		UINT16	Indices[6];		
	#endif

//	MATRIX BBMat;
//	i3Matrix::Transpose( &BBMat, pContext->GetViewMatrix());

//	i3Matrix::Dump(&m_InvViewMatrix);

	for( i = 0; i < m_Count; i++, pQuad++)
	{
		if( pQuad->m_bEnable == false)
			continue;

		if( !bLocked)
		{
			bLocked = true;

		#if INDEXED_IMPLEMENT
			vLockStart = m_nBufferIndex*m_Count*4;
			iLockStart = m_nBufferIndex*m_Count*6;
			m_pVA->Lock( QUAD_LOCK, vLockStart, m_Count * 4 );
		#else
			#if defined(I3G_PSP)
			//	vLockStart = m_nBufferIndex*m_Count*2;
				vLockStart = 0;
				m_pVA->Lock( QUAD_LOCK, vLockStart, m_Count * 2);
			#else
			//	vLockStart = m_nBufferIndex*m_Count*6;
				vLockStart = 0;
				m_pVA->Lock( QUAD_LOCK, vLockStart, m_Count * 6);
			#endif
		#endif
		}
		
		// Position
		_setPositions( m_pVA, pQuad, vIdx, &m_InvViewMatrix, m_Space );

		// UV
		if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			_setUV( m_pVA, pQuad, vIdx, m_Space);
		}

		// Color
		if( m_pVA->GetFormat()->GetHasColor())
			_setColor( m_pVA, pQuad, vIdx);

		// Index
		#if INDEXED_IMPLEMENT
		{
			Indices[0] = (UINT16)(vIdx + 0);
			Indices[1] = (UINT16)(vIdx + 1);
			Indices[2] = (UINT16)(vIdx + 2);
			Indices[3] = (UINT16)(vIdx + 3);
			Indices[4] = (UINT16)(vIdx + 0);
			Indices[5] = (UINT16)(vIdx + 2);

			m_pIA->SetIndices16( iIdx, 6, Indices);
		}
		#endif

		
		primcount ++;

		#if INDEXED_IMPLEMENT
			vIdx += 4;
			iIdx += 6;
		#else
			#if defined(I3G_PSP)
				vIdx += 2;
			#else
				vIdx += 6;
			#endif
		#endif

	}

	// Render rest of Quads
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
			#if defined(I3G_PSP)
				m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_POINTSPRITE, vLockStart, primcount << 1);
			#else
				m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, vLockStart, primcount << 1);
			#endif
	
		#endif
	}
}

void i3QuadAttr::ApplyEdittedUpdate( i3RenderContext * pContext)
{
	UINT32 i, vIdx;
	I3SG_QUAD_INFO * pQuad = m_pQuads;
	bool bLocked = false;

	#if INDEXED_IMPLEMENT
		UINT16	Indices[6];
		UINT32 iIdx`
		iIdx = 0;
	#endif

	if(m_Count <= 0)
		return;

	if( m_bEditted)
	{
		m_PrimCount = 0;
		vIdx = 0;		

		for( i = 0; i < m_Count; i++, pQuad++)
		{
			if( pQuad->m_bEnable == false)
				continue;

			if( !bLocked)
			{
				bLocked = true;

				m_pVA->Lock( QUAD_LOCK);
				#if INDEXED_IMPLEMENT
					m_pIA->Lock( 0, 0, QUAD_LOCK);
				#endif
			}

			_setPositions( m_pVA, pQuad, vIdx, &m_InvViewMatrix, m_Space );
			
			// UV
			if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
			{
				_setUV( m_pVA, pQuad, vIdx, m_Space);
			}

			// Color
			if( m_pVA->GetFormat()->GetHasColor())
				_setColor( m_pVA, pQuad, vIdx);
			
			// Index
			#if INDEXED_IMPLEMENT
			{
				Indices[0] = (UINT16)(vIdx);
				Indices[1] = (UINT16)(vIdx + 1);
				Indices[2] = (UINT16)(vIdx + 2);
				Indices[3] = (UINT16)(vIdx + 2);
				Indices[4] = (UINT16)(vIdx + 3);
				Indices[5] = (UINT16)(vIdx + 0);

				m_pIA->SetIndices16( iIdx, 6, Indices);
			}
			#endif

			m_PrimCount++;
			
			#if INDEXED_IMPLEMENT
				vIdx += 4;
				iIdx += 6;
			#else
				#if defined(I3G_PSP)
					vIdx += 2;
				#else
					vIdx += 6;
				#endif
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
			#if defined(I3G_PSP)
				m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_POINTSPRITE, 0, m_PrimCount << 1);	
			#else
				m_pShader = pContext->DrawPrim( m_pShader, I3G_PRIM_TRILIST, 0, m_PrimCount << 1);	
			#endif
		#endif
	}
}

void i3QuadAttr::Apply( i3RenderContext * pContext)
{
	I3ASSERT( m_pQuads != nullptr);
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

void i3QuadAttr::CopyTo( i3ElementBase *pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED QUAD_HEADER
	{
		UINT8			m_Space = 0;
		UINT8			m_bAlwaysUpdate = 0;
		UINT8			m_bUV = 0;
		UINT8			m_bColor = 0;
		UINT32			m_Count = 0;
	};

	struct PACKED QUAD_INFO
	{
		i3::pack::VEC3D		m_Center;
		REAL32				m_Width = 0.0f;
		REAL32				m_Height = 0.0f;
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

UINT32 i3QuadAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::QUAD_INFO		Info;
	UINT32 i;
	I3SG_QUAD_INFO * pQuad = m_pQuads;

	// Header
	{
		pack::QUAD_HEADER		Header;

		Header.m_Space			= (UINT8) m_Space;
		Header.m_bAlwaysUpdate	= (UINT8) m_bAlwaysUpdate;
		Header.m_bUV			= (UINT8) (m_pVA->GetFormat()->GetTextureCoordSetCount() > 0);
		Header.m_bColor			= (UINT8) m_pVA->GetFormat()->GetHasColor();
		Header.m_Count			= m_Count;

		Rc = pStream->Write( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3QuadAttr::OnSave()", "Could not write Quad header.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Info.m_Center.x = i3Vector::GetX( &pQuad->m_Center);
		Info.m_Center.y = i3Vector::GetY( &pQuad->m_Center);
		Info.m_Center.z = i3Vector::GetZ( &pQuad->m_Center);

		Info.m_Width = pQuad->m_Width;
		Info.m_Height = pQuad->m_Height;

		Info.m_MinUV.x = i3Vector::GetU( &pQuad->m_UV[0]);
		Info.m_MinUV.y = i3Vector::GetV( &pQuad->m_UV[0]);
		Info.m_MaxUV.x = i3Vector::GetU( &pQuad->m_UV[1]);
		Info.m_MaxUV.y = i3Vector::GetV( &pQuad->m_UV[1]);

		Info.m_bEnable = (UINT8) pQuad->m_bEnable;

		Info.m_Color = i3Color::ConvertRGBA32( & pQuad->m_Color);

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3QuadAttr::OnSave()", "Could not write Quad information.");
			return STREAM_ERR;
		}
		Result += Rc;
		pQuad++;
	}

	return Result;
}

UINT32 i3QuadAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3QuadAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::QUAD_INFO		Info;
	UINT32 i;

	// Header
	{
		pack::QUAD_HEADER		Header;

		Rc = pStream->Read( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3QuadAttr::OnLoad()", "Could not read Quad header.");
			return STREAM_ERR;
		}
		Result += Rc;

		Create( Header.m_Count, (I3SG_QUAD_SPACE) Header.m_Space, Header.m_bUV != 0,
			Header.m_bColor != 0, Header.m_bAlwaysUpdate != 0);
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Rc = pStream->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3QuadAttr::OnLoad()", "Could not read Quad information.");
			return STREAM_ERR;
		}
		Result += Rc;

		SetCenter( i, Info.m_Center.x, Info.m_Center.y, Info.m_Center.z);
		SetSize( i, Info.m_Width, Info.m_Height);
		
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

void i3QuadAttr::OnLostDevice( bool bLostDevice)
{
	I3_SAFE_RELEASE( m_pVA);
	I3_SAFE_RELEASE( m_pIA);
	m_pShader = nullptr;
}

void i3QuadAttr::OnRevive( i3RenderContext * pCtx)
{
	_CreateVertex();

	//ApplyAlwaysUpdate( pCtx);
}
