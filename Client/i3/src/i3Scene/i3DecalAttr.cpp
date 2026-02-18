#include "i3SceneDef.h"
#include "i3Gfx.h"
#include "i3DecalAttr.h"

I3_CLASS_INSTANCE( i3DecalAttr, i3RenderAttr);

#define			DECAL_VERTEX_COUNT		32
#define			DECAL_INDEX_COUNT		(DECAL_VERTEX_COUNT + 2)

#define			DECAL_USAGE			(I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE)


i3DecalAttr::i3DecalAttr(void)
{
	m_Count		= 0;
	m_pVA		= NULL;
	m_pIA		= NULL;
	m_PrimCount	= 0;
	m_bEditted	= TRUE;
	m_bAlwaysUpdate	= FALSE;
	m_pDecals	= NULL;

	i3Matrix::Identity( &m_InvViewMatrix);
}

i3DecalAttr::~i3DecalAttr(void)
{
	if( m_pVA != NULL)
	{
		m_pVA->Release();
		m_pVA = NULL;
	}

	if( m_pIA != NULL)
	{
		m_pIA->Release();
		m_pIA = NULL;
	}

	I3MEM_SAFE_FREE_POINTER( m_pDecals);
}

void i3DecalAttr::setFaceToCameraEnable( bool bFlag)
{
	m_bFaceToCamera = bFlag;

	if( bFlag == false)
	{
		i3Matrix::Identity( &m_InvViewMatrix);
	}
}

BOOL i3DecalAttr::Create( UINT32 Count, BOOL bUV, BOOL bColor, BOOL bNormalMap, BOOL bAlwaysUpdate)
{
	i3VertexFormat format;

	m_Count = Count;
	m_bAlwaysUpdate = bAlwaysUpdate;
	m_bAlwaysUpdate = FALSE;

	// Vertex format
	{
		format.SetHasPosition( true);
		if( bColor)
			format.SetHasColor( true);			
		
		if( bUV)
		{
			format.SetTextureCoordSetCount( 1);
			format.SetTextureCoordCount( 2);

			format.SetHasNormal( (bNormalMap == TRUE));
			format.SetHasTangent( (bNormalMap == TRUE));
			format.SetHasBinormal( (bNormalMap == TRUE));
		}
	}

	// DECAL Information
	{
		UINT32 MemSize = sizeof(I3SG_DECAL_INFO) * m_Count;
		
		m_pDecals		= (I3SG_DECAL_INFO *) i3MemAlloc( MemSize );
		I3ASSERT( m_pDecals );

		memset( m_pDecals, 0, MemSize );

		// Create HitBox Shape
		VEC3D	vZero, vSize;
		i3Vector::Set( &vZero, 0.f, 0.f, 0.f );
		i3Vector::Set( &vSize, 0.5f, 0.5f, 0.003f );

		for( INT32 i = 0; i < m_Count; i++ )
		{
			m_pDecals[i].m_pHitBoxShapeSet = i3PhysixShapeSet::NewObject();

			m_pDecals[i].m_pHitBoxShapeSet->CreateBoxShape( &vZero, &vSize, 0, true);
			m_pDecals[i].m_pHitBoxShapeSet->modifyDynamicState( I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_RAYCAST, 0);
		}
	}

	m_pVA = i3VertexArray::NewObject();
	m_pVA->setRestoreType( I3G_RESTORE_MEM);

	if( m_bAlwaysUpdate)
	{
		// Vertex Array
		{
			// 한 DECAL 당 4개의 Vertex를 필요로 한다.
			m_pVA->Create( &format, (m_Count * DECAL_VERTEX_COUNT), DECAL_USAGE );
		}

		// Index Array
		{
			m_pIA = i3IndexArray::NewObject();
			m_pIA->setRestoreType( I3G_RESTORE_MEM);

			m_pIA->Create( (m_Count * DECAL_INDEX_COUNT) , DECAL_USAGE);
		}
	}
	else
	{
		// 변경된 경우에만 Update하는 동작 모드에서는 Vertex Buffer를 DECAL의 개수만큼
		// 미리 준비해둔다.
		// Vertex Array
		{
			m_pVA->Create( &format, m_Count * DECAL_VERTEX_COUNT, DECAL_USAGE);
		}

		// Index Array
		{
			m_pIA = i3IndexArray::NewObject();
			m_pIA->setRestoreType( I3G_RESTORE_MEM);

			m_pIA->Create( m_Count * DECAL_INDEX_COUNT, DECAL_USAGE);
		}
	}

	m_bEditted = TRUE;

	return TRUE;
}

void i3DecalAttr::SetEnable( BOOL bFlag)
{
	I3ASSERT( m_pDecals != NULL);

	UINT32 i;
	for( i = 0; i < m_Count; i++ )
	{
		if( m_pDecals[i].m_bEnable != bFlag )
		{
			m_pDecals[i].m_bEnable = bFlag;
			m_bEditted = TRUE;
		}
	}
}

void i3DecalAttr::SetEnable( UINT32 Index, BOOL bFlag)
{
	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	if( m_pDecals[Index].m_bEnable != bFlag )
	{
		m_pDecals[Index].m_bEnable = bFlag;
		m_bEditted = TRUE;
	}
}

void i3DecalAttr::SetCenter( UINT32 Index, RT_REAL32 x, RT_REAL32 y, RT_REAL32 z)
{
	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	i3Vector::Set( & m_pDecals[Index].m_Center, x, y, z);
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_POSITION;

	m_bEditted = TRUE;
}

void i3DecalAttr::SetCenter( UINT32 Index, RT_VEC3D * pVec)
{
	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	i3Vector::Copy( & m_pDecals[Index].m_Center, pVec);
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_POSITION;

	m_bEditted = TRUE;
}

void i3DecalAttr::GetLeftTop( UINT32 Index, RT_VEC3D * pVec)
{
	RT_REAL32 sx, sy;

	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	I3SG_DECAL_INFO * pDecal = & m_pDecals[Index];

	#if defined( I3_FIXED_POINT)
		sx = pDecal->m_Width >> 1;
		sy = pDecal->m_Height >> 1;
	#else
		sx = pDecal->m_Width * 0.5f;
		sy = pDecal->m_Height * 0.5f;
	#endif

	i3Vector::Set( pVec,	i3Vector::GetX( & pDecal->m_Center) - sx,
							i3Vector::GetY( & pDecal->m_Center) - sy,
							i3Vector::GetZ( & pDecal->m_Center));
}

void i3DecalAttr::GetRightBottom( UINT32 Index, RT_VEC3D * pVec)
{
	RT_REAL32 sx, sy;

	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	I3SG_DECAL_INFO * pDecal = & m_pDecals[Index];

	#if defined( I3_FIXED_POINT)
		sx = pDecal->m_Width >> 1;
		sy = pDecal->m_Height >> 1;
	#else
		sx = pDecal->m_Width * 0.5f;
		sy = pDecal->m_Height * 0.5f;
	#endif

	i3Vector::Set( pVec,	i3Vector::GetX( & pDecal->m_Center) + sx,
							i3Vector::GetY( & pDecal->m_Center) + sy,
							i3Vector::GetZ( & pDecal->m_Center));
}

void i3DecalAttr::SetRotation( UINT32 Index, RT_REAL32 rotation)
{
	I3ASSERT( Index < m_Count);
	m_pDecals[Index].m_Rotation = rotation;
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_ROTATION;

	m_bEditted = TRUE;
}

void i3DecalAttr::SetFlip( UINT32 Index, BOOL FlipX, BOOL FlipY )
{
	I3ASSERT( Index < m_Count);

	m_pDecals[Index].m_bFlipX = FlipX;
	m_pDecals[Index].m_bFlipY = FlipY;

	m_bEditted = TRUE;
}

void i3DecalAttr::SetSize( UINT32 Index, RT_VEC2D* pVec)
{
	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	m_pDecals[Index].m_Width = i3Vector::GetU( pVec);
	m_pDecals[Index].m_Height = i3Vector::GetV( pVec);
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_POSITION;

	m_bEditted = TRUE;
}

void i3DecalAttr::SetSize( UINT32 Index, RT_REAL32 Width, RT_REAL32 Height)
{
	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	m_pDecals[Index].m_Width = Width;
	m_pDecals[Index].m_Height = Height;
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_POSITION;

	m_bEditted = TRUE;
}

RT_VEC2D * i3DecalAttr::GetSize( UINT32 Index)
{
	return (RT_VEC2D *) &m_pDecals[Index].m_Width;
}

void i3DecalAttr::SetTextureCoord( UINT32 Index, RT_VEC2D * pMin, RT_VEC2D * pMax)
{
	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	i3Vector::Copy( & m_pDecals[Index].m_UV[0], pMin);
	i3Vector::Copy( & m_pDecals[Index].m_UV[1], pMax);
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_UV;

	m_bEditted = TRUE;
}

void i3DecalAttr::SetTextureCoord( UINT32 Index, RT_REAL32 u1, RT_REAL32 v1, RT_REAL32 u2, RT_REAL32 v2)
{
	I3ASSERT( Index < m_Count);
	I3ASSERT( m_pDecals != NULL);

	i3Vector::Set( & m_pDecals[Index].m_UV[0], u1, v1);
	i3Vector::Set( & m_pDecals[Index].m_UV[1], u2, v2);
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_UV;

	m_bEditted = TRUE;
}

void i3DecalAttr::SetColor( COLOR * pColor)
{
	UINT32 i;
	for( i = 0; i < m_Count; i++ )
	{
		i3Color::Set( & m_pDecals[i].m_Color, pColor);
		m_pDecals[i].m_EditMask |= I3SG_DECAL_EDIT_COLOR;
	}
	m_bEditted = TRUE;
}

void i3DecalAttr::SetColor( UINT32 Index, COLOR * pColor)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( & m_pDecals[Index].m_Color, pColor);
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_COLOR;

	m_bEditted = TRUE;
}
void i3DecalAttr::SetColor( UINT32 Index, UINT8 r,UINT8 g,UINT8 b,UINT8 a)
{
	I3ASSERT( Index < m_Count);

	i3Color::Set( & m_pDecals[Index].m_Color, r, g, b, a);
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_COLOR;

	m_bEditted = TRUE;
}

void i3DecalAttr::SetColor( UINT32 Index, UINT32 rgba)
{
	I3ASSERT( Index < m_Count);

	((I3UNIONCOLOR *) &m_pDecals[Index].m_Color)->value = rgba;
	m_pDecals[Index].m_EditMask |= I3SG_DECAL_EDIT_COLOR;

	m_bEditted = TRUE;
}

inline
void _setPositions( i3VertexArray * pVA, I3SG_DECAL_INFO * pDecal, UINT32 vIdx, RT_MATRIX* pMat)
{
	INT32	i;
	RT_REAL32 cx, cy;
	RT_VEC3D vecCenter;
	RT_VEC3D *pCenter = &vecCenter;
	VEC3D norm;

	i3Vector::Copy( pCenter, &pDecal->m_Center );
	
	cx = pDecal->m_Width;
	cy = pDecal->m_Height;

	pVA->SetPositions( vIdx, pDecal->m_nHitCount, pDecal->m_pPoints );
	if( pVA->GetFormat()->GetHasNormal())
	{
		i3Vector::Set( &norm, 0.0f, 0.0f, 1.0f);

		i3Vector::TransformNormal( &norm, &norm, pMat);
		i3Vector::Normalize( &norm, &norm);

		for( INT32 i = 0; i < pDecal->m_nHitCount; i++ )
		{
			pVA->SetNormal( vIdx + i, &norm);
		}
	}
}

inline
void _setUV( i3VertexArray * pVA, I3SG_DECAL_INFO * pInfo, UINT32 vIdx)
{
	INT32 i;
	RT_REAL32 u1, v1, u2, v2;
	VEC3D t, b, pos1, pos2, pos3;

	RT_VEC2D _uv[ 32 ];

	//// 3D와 2D는 Y축이 뒤집혀 있으므로, 그에 대한 처리를 해줍니다.
	BOOL bFlipY = pInfo->m_bFlipY;

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

	VEC2D vMin, vMax, vScale;
	i3Vector::Set( &vMin, 1.f, 1.f );
	i3Vector::Set( &vMax, -1.f, -1.f );
	if( i3Vector::GetU( &vMin ) > i3Vector::GetU( &pInfo->m_UV[0] ) )
		i3Vector::SetU( &vMin, i3Vector::GetU( &pInfo->m_UV[ 0 ] ) );
	if( i3Vector::GetV( &vMin ) > i3Vector::GetV( &pInfo->m_UV[0] ) )
		i3Vector::SetV( &vMin, i3Vector::GetV( &pInfo->m_UV[ 0 ] ) );
	if( i3Vector::GetU( &vMax ) < i3Vector::GetU( &pInfo->m_UV[0] ) )
		i3Vector::SetU( &vMax, i3Vector::GetU( &pInfo->m_UV[ 0 ] ) );
	if( i3Vector::GetV( &vMax ) < i3Vector::GetV( &pInfo->m_UV[0] ) )
		i3Vector::SetV( &vMax, i3Vector::GetV( &pInfo->m_UV[ 0 ] ) );

	if( i3Vector::GetU( &vMin ) > i3Vector::GetU( &pInfo->m_UV[ 1 ] ) )
		i3Vector::SetU( &vMin, i3Vector::GetU( &pInfo->m_UV[ 1 ] ) );
	if( i3Vector::GetV( &vMin ) > i3Vector::GetV( &pInfo->m_UV[1] ) )
		i3Vector::SetV( &vMin, i3Vector::GetV( &pInfo->m_UV[ 1 ] ) );
	if( i3Vector::GetU( &vMax ) < i3Vector::GetU( &pInfo->m_UV[ 1 ] ) )
		i3Vector::SetU( &vMax, i3Vector::GetU( &pInfo->m_UV[ 1 ] ) );
	if( i3Vector::GetV( &vMax ) < i3Vector::GetV( &pInfo->m_UV[1] ) )
		i3Vector::SetV( &vMax, i3Vector::GetV( &pInfo->m_UV[ 1 ] ) );

	i3Vector::Sub( &vScale, &vMax, &vMin );
	
	for( i = 0; i < pInfo->m_nHitCount; i++ )
	{	
		i3Vector::SetU( &_uv[ i ], i3Vector::GetU( &pInfo->m_pUVs[ i ] ) * i3Vector::GetU( &vScale ) );
		i3Vector::SetV( &_uv[ i ], i3Vector::GetV( &pInfo->m_pUVs[ i ] ) * i3Vector::GetV( &vScale ) );
	}
	
	pVA->SetTextureCoords( 0, vIdx, pInfo->m_nHitCount, _uv);

	if( pVA->GetFormat()->GetHasTangent())
	{
		pVA->GetPosition( vIdx, &pos1);
		pVA->GetPosition( vIdx+1, &pos2);
		pVA->GetPosition( vIdx+2, &pos3);
		i3Math::CalcTangentBasis( &pos1, &pos2, &pos3, &_uv[0], &_uv[1], &_uv[2], &t, &b);

		for( i = 0; i < pInfo->m_nHitCount; i++ )
		{
			pVA->SetTangent( vIdx+i, &t);
			pVA->SetBinormal( vIdx+i, &b);
		}
	}
}

inline
void _setColor( i3VertexArray * pVA, I3SG_DECAL_INFO * pDecal, UINT32 vIdx)
{
	pVA->SetColor( vIdx, pDecal->m_nHitCount, &pDecal->m_Color);
}

void i3DecalAttr::ApplyAlwaysUpdate( i3RenderContext * pContext)
{
	UINT32 primcount, i;
	UINT32 vLockStart, vIdx;
	I3SG_DECAL_INFO * pDecal;
	BOOL bLocked = FALSE;

	pDecal = m_pDecals;

	primcount = 0;
	vLockStart = 0;
	vIdx = 0;

	pContext->SetVertexArray( m_pVA);

	UINT32 iLockStart, iIdx;
	UINT16	Indices[6];

	iLockStart = 0;
	iIdx = 0;

	pContext->SetIndexArray( m_pIA);
 
	for( i = 0; i < m_Count; i++, pDecal++)
	{
		if( pDecal->m_bEnable == FALSE)
			continue;

		if( bLocked == FALSE)
		{
			UINT32 count = MIN( m_Count, m_Count - i);

			m_pVA->Lock( I3G_LOCK_NOOVERWRITE | I3G_LOCK_OVERWRITE, vLockStart, count * 4 );
			m_pIA->Lock( iLockStart, count * 6, I3G_LOCK_NOOVERWRITE | I3G_LOCK_OVERWRITE);

			bLocked = TRUE;
		}

		RT_MATRIX Mat;

		// 3DMAX
		i3Matrix::SetRotateZ( &Mat, pDecal->m_Rotation );

		if( m_bFaceToCamera == true)
		{
			i3Matrix::Mul( &Mat, &Mat, &m_InvViewMatrix );
		}

		_setPositions( m_pVA, pDecal, vIdx, &Mat );

		// UV
		if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			_setUV( m_pVA, pDecal, vIdx);
		}

		// Color
		if( m_pVA->GetFormat()->GetHasColor())
			_setColor( m_pVA, pDecal, vIdx);

		// Index
		{
			Indices[0] = (UINT16)(vIdx);
			Indices[1] = (UINT16)(vIdx + 2);
			Indices[2] = (UINT16)(vIdx + 1);
			Indices[3] = (UINT16)(vIdx + 1);
			Indices[4] = (UINT16)(vIdx + 2);
			Indices[5] = (UINT16)(vIdx + 3);

			m_pIA->SetIndices16( iIdx, 6, Indices);
		}
		
		primcount ++;

		vIdx += 4;
		iIdx += 6;
	}

	// Render rest of DECALs
	if( primcount > 0)
	{
		m_pVA->Unlock();
		m_pIA->Unlock();
		pContext->DrawIndexedPrim( I3G_PRIM_TRILIST, iLockStart, primcount << 1);
	}
}

void i3DecalAttr::ApplyEdittedUpdate( i3RenderContext * pContext)
{
	UINT32 i, vIdx;
	I3SG_DECAL_INFO * pDecal = m_pDecals;

	if(m_Count <= 0)
		return;

	if( m_bEditted)
	{	
		for( i = 0; i < m_Count; i++, pDecal++)
		{
			if( pDecal->m_bEnable == FALSE)
				continue;

			RT_MATRIX Mat;

			// 3DMAX
			i3Matrix::SetRotateZ( &Mat, pDecal->m_Rotation);
			i3Matrix::Mul( &Mat, &Mat, &m_InvViewMatrix);

			{	// Set Size
				NxVec3 vec( pDecal->m_Width * 0.5f, pDecal->m_Height * 0.5f, 0.003f );
				NxShape * pShape = pDecal->m_pHitBoxShapeSet->getNxShape( 0 );
				NxBoxShape * pBoxShape = pShape->isBox();
				pBoxShape->setDimensions( vec );
			}

			// transform world space
			pDecal->m_pHitBoxShapeSet->setShapeGlobalPose( &Mat );

			// collision enable Hitbox
			SetHitOn( i );
		}

		m_bEditted = FALSE;
	}
}

void i3DecalAttr::ApplyDrawUpdate( i3RenderContext * pContext )
{
	UINT32 i, vIdx;
	I3SG_DECAL_INFO * pDecal = m_pDecals;
	
	UINT16	Indices[6];
	UINT32 iIdx;

	if(m_Count <= 0)
		return;

	if( m_bUpdatted )
	{
		m_PrimCount = 0;
		vIdx = 0;

		m_pVA->Lock( I3G_LOCK_NOOVERWRITE | I3G_LOCK_OVERWRITE);

		m_pIA->Lock( 0, 0, I3G_LOCK_NOOVERWRITE | I3G_LOCK_OVERWRITE);
		iIdx = 0;

		for( i = 0; i < m_Count; i++, pDecal++)
		{
			if( pDecal->m_bEnable == FALSE)
				continue;

			RT_MATRIX Mat;

			// 3DMAX
			i3Matrix::SetRotateZ( &Mat, pDecal->m_Rotation);
			i3Matrix::Mul( &Mat, &Mat, &m_InvViewMatrix);

			_setPositions( m_pVA, pDecal, vIdx, &Mat );
			
			// UV
			if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
			{
				_setUV( m_pVA, pDecal, vIdx);
			}

			// Color
			if( m_pVA->GetFormat()->GetHasColor())
			{
				_setColor( m_pVA, pDecal, vIdx);
			}
			
			// Index
			{
				Indices[0] = (UINT16)(vIdx);
				Indices[1] = (UINT16)(vIdx + 2);
				Indices[2] = (UINT16)(vIdx + 1);
				Indices[3] = (UINT16)(vIdx + 1);
				Indices[4] = (UINT16)(vIdx + 2);
				Indices[5] = (UINT16)(vIdx + 3);

				m_pIA->SetIndices16( iIdx, 6, Indices);
			}
			
			m_PrimCount++;
			
			vIdx += 4;
			iIdx += 6;
		}

		m_bEditted = FALSE;

		m_pVA->Unlock();
		m_pIA->Unlock();
	}

	if( m_PrimCount > 0)
	{
		//pContext->BackupRS(0);
		//pContext->BackupTS(0);
		//pContext->PrintRS(0);
		//pContext->PrintTS(0);

		pContext->SetVertexArray( m_pVA);

		pContext->SetIndexArray( m_pIA);
		pContext->DrawIndexedPrim( I3G_PRIM_TRILIST, 0, m_PrimCount << 1);
	}
}

void i3DecalAttr::SetEventOwner( i3EventReceiver * pOwner )
{
	for( INT32 i = 0; i < m_Count; i++ )
	{
		if( m_pDecals[ i ].m_pHitBoxShapeSet != NULL )
		{
			m_pDecals[ i ].m_pHitBoxShapeSet->setEventReceiver( pOwner );
		}
	}
}

void i3DecalAttr::Apply( i3RenderContext * pContext)
{
	I3ASSERT( m_pDecals != NULL);
	I3ASSERT( m_pVA != NULL);
	
	I3ASSERT( m_pIA != NULL);

	if( m_bAlwaysUpdate)
	{
		ApplyAlwaysUpdate( pContext);
	}
	else
	{
		ApplyEdittedUpdate( pContext);
	}
}

void i3DecalAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);
}

BOOL i3DecalAttr::IsSame( i3RenderAttr *)
{
	return FALSE;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

typedef struct PACKED _tagi3DECALPersistHead
{
	UINT8			m_bFaceToCamera;
	UINT8			m_bAlwaysUpdate;
	UINT8			m_bUV;
	UINT8			m_bColor;
	UINT32			m_Count;
} I3SG_PERSIST_DECAL_HEADER;

typedef struct PACKED _tagI3DECALPersistInfo
{
	PERSIST_VEC3D		m_Center;
	REAL32				m_Width;
	REAL32				m_Height;
	REAL32				m_Rotation;
	PERSIST_VEC2D		m_MinUV;
	PERSIST_VEC2D		m_MaxUV;
	UINT32				m_Color;
	UINT8				m_bEnable;
	UINT8				m_Reserved[3];
} I3SG_PERSIST_DECAL_INFO;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3DecalAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3SG_PERSIST_DECAL_INFO		Info;
	UINT32 i;
	I3SG_DECAL_INFO * pDecal = m_pDecals;

	// Header
	{
		I3SG_PERSIST_DECAL_HEADER		Header;

		Header.m_bFaceToCamera	= (UINT8) !getFaceToCameraEnable();
		Header.m_bAlwaysUpdate	= (UINT8) m_bAlwaysUpdate;
		Header.m_bUV			= (UINT8) (m_pVA->GetFormat()->GetTextureCoordSetCount() > 0);
		Header.m_bColor			= (UINT8) m_pVA->GetFormat()->GetHasColor();
		Header.m_Count			= m_Count;

		Rc = pStream->Write( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3DecalAttr::OnSave()", "Could not write DECAL header.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Info.m_Center.x = __PT( i3Vector::GetX( &pDecal->m_Center));
		Info.m_Center.y = __PT( i3Vector::GetY( &pDecal->m_Center));
		Info.m_Center.z = __PT( i3Vector::GetZ( &pDecal->m_Center));

		Info.m_Width = __PT( pDecal->m_Width);
		Info.m_Height = __PT( pDecal->m_Height);

		Info.m_Rotation = __PT( pDecal->m_Rotation);

		Info.m_MinUV.x = __PT( i3Vector::GetU( &pDecal->m_UV[0]));
		Info.m_MinUV.y = __PT( i3Vector::GetV( &pDecal->m_UV[0]));
		Info.m_MaxUV.x = __PT( i3Vector::GetU( &pDecal->m_UV[1]));
		Info.m_MaxUV.y = __PT( i3Vector::GetV( &pDecal->m_UV[1]));

		Info.m_bEnable = (UINT8) pDecal->m_bEnable;

		Info.m_Color = i3Color::ConvertRGBA32( & pDecal->m_Color);

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3DecalAttr::OnSave()", "Could not write DECAL information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3DecalAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3DecalAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3SG_PERSIST_DECAL_INFO		Info;
	UINT32 i;

	// Header
	{
		I3SG_PERSIST_DECAL_HEADER		Header;

		Rc = pStream->Read( &Header, sizeof(Header));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3DecalAttr::OnLoad()", "Could not read DECAL header.");
			return STREAM_ERR; 
		}
		Result += Rc;

		Create( Header.m_Count, (BOOL) Header.m_bUV, 
			(BOOL) Header.m_bColor, (BOOL) Header.m_bAlwaysUpdate);

		setFaceToCameraEnable( (bool)(Header.m_bFaceToCamera == 0));
	}

	//
	for( i = 0; i < m_Count; i++)
	{
		Rc = pStream->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3DecalAttr::OnLoad()", "Could not read DECAL information.");
			return STREAM_ERR;
		}
		Result += Rc;

		SetCenter( i, __RT(Info.m_Center.x), __RT(Info.m_Center.y), __RT(Info.m_Center.z));
		SetSize( i, __RT(Info.m_Width), __RT(Info.m_Height));
		SetRotation( i, __RT(Info.m_Rotation));


		SetEnable( i, Info.m_bEnable);

		if( m_pVA->GetFormat()->GetTextureCoordSetCount() > 0)
		{
			SetTextureCoord( i, __RT(Info.m_MinUV.x), __RT(Info.m_MinUV.y), __RT(Info.m_MaxUV.x), __RT(Info.m_MaxUV.y));
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
