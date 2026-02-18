#include "i3SceneDef.h"
#include "i3DecalManager.h"
#include "i3PhysixDefine.h"
#include "i3PhysixShapeSet.h"
#include "i3Decal.h"

#define	I3_DECAL_PHYSIX_HEIGHT		5000
#define DECAL_HIT_DEPTH				0.01f
#define I3SG_DECAL_DEFAULT_EPSILON		0.5f

I3_CLASS_INSTANCE( i3DecalManager);

i3DecalManager::i3DecalManager()
{
	for( INT32 i = 0; i < 36; i++ )
		m_BoxIndices[i] = i;

	m_EmitedDecalList.reserve( I3SG_DECAL_MAX_COUNT);
}

i3DecalManager::~i3DecalManager()
{
	Destroy();
	ReleasePhysXBox();
}

void i3DecalManager::Create( i3EventReceiver * pER, INT32 count)
{
#if defined( I3_PHYSX)

	if( m_iCount == count)
		return;

	// 생성전에 이전에 만든 것을 초기화
	Destroy();
	ReleasePhysXBox();
	
	m_iCount = count;

	if( m_pDecalPool == nullptr)
	{
		m_pDecalPool = new i3FixedMemoryPool;
		NetworkDump_Malloc( m_pDecalPool, sizeof( i3FixedMemoryPool), __FILE__, __LINE__);
		m_pDecalPool->Create( "Decal Pool", sizeof(i3Decal), 0xFFFF);
		m_pDecalPool->SetAutoCompactEnable(false);
	}

	if( m_pDecalDrawInfoPool == nullptr)
	{
		m_pDecalDrawInfoPool = new i3FixedMemoryPool;
		NetworkDump_Malloc( m_pDecalDrawInfoPool, sizeof( i3FixedMemoryPool), __FILE__, __LINE__);
		m_pDecalDrawInfoPool->Create( "Decal Draw Pool", sizeof(i3DecalDrawInfo), 0xFFFF);
		m_pDecalDrawInfoPool->SetAutoCompactEnable(false);
	}

	CreatePhysXBox( pER);
#endif
}

void i3DecalManager::Destroy( void)
{
	m_EmitedDecalList.clear();

	I3_SAFE_DELETE( m_pDecalPool);
	I3_SAFE_DELETE( m_pDecalDrawInfoPool);
}

void i3DecalManager::CreatePhysXBox( i3EventReceiver * pER)
{
	// Decal용 ShapeSet 
	{	
		VEC3D	vZero, vSize;

		i3Vector::Set( &vZero, 0.f, 0.f, 0.f );
		i3Vector::Set( &vSize, 0.1f, 0.1f, 0.01f );

		MATRIX mat;
		i3PhysixShapeSet * pShapeSet;

		I3ASSERT( m_pDecalHitBox == nullptr);

		m_pDecalHitBox = (I3SG_DECAL_HITBOX*) i3MemAlloc( sizeof( I3SG_DECAL_HITBOX) * m_iCount);

		i3mem::FillZero( m_pDecalHitBox, sizeof(I3SG_DECAL_HITBOX) * m_iCount);

	#if defined( I3_PHYSX)
		for( INT32 i = 0; i < m_iCount; i++ )
		{
			pShapeSet = m_pDecalHitBox[ i ].m_pHitBox = i3PhysixShapeSet::new_object();

			pShapeSet->PrepareContactReport();

			m_pDecalHitBox[ i ].m_nIndex = i;

			// 공중에 붙여줍니다.
			i3Matrix::SetTranslate( &mat, (REAL32) ((i * 20) - 2560), 5000.f, 0.f );

			pShapeSet->CreateBoxShape( &vZero, &vSize, I3SG_DECAL_ACTOR_GROUP, true, &mat);
			pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE |
										I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_NO_RAYCAST |
										I3_PHYSIX_STATE_KINEMATIC, 0 );
			// ParticleRender를 설정
			pShapeSet->setEventReceiver( pER);
		}
	#endif
	}
}

void i3DecalManager::ReleasePhysXBox( void)
{
	I3SG_DECAL_HITBOX * pBox = nullptr;

	for( size_t idx = 0 ; idx < m_EmitedDecalList.size() ; ++idx )
	{
		pBox = m_EmitedDecalList[idx];

		if( pBox->m_pParticle != nullptr )
			_CancelDecalHitBox(pBox);

		if( pBox->m_pDecal != nullptr )
		{
			FreeDecal( pBox->m_pDecal );
			pBox->m_pDecal = nullptr;
		}

		if( pBox->m_pDrawDecalInfo != nullptr )
		{
			FreeDrawDecalInfo( pBox->m_pDrawDecalInfo );
			pBox->m_pDrawDecalInfo = nullptr;
		}
	}

	m_EmitedDecalList.clear();

	if( m_pDecalHitBox != nullptr)
	{
		for( INT32 i = 0; i < m_iCount; i++)
		{
	#if defined( I3_PHYSX)
			I3_SAFE_RELEASE( m_pDecalHitBox[i].m_pHitBox );
	#endif
		}

		I3MEM_SAFE_FREE( m_pDecalHitBox);
	}
}

INT32 i3DecalManager::_FindFreeHitBox(void)
{
	INT32 i;

	for( i = 0; i < m_iCount; i++)
	{
		if( m_pDecalHitBox[i].m_pParticle == nullptr)
			return i;
	}

	return -1;
}

i3Decal * i3DecalManager::_GetDecal( void)
{
	return (i3Decal*) m_pDecalPool->Alloc( sizeof(i3Decal), __FILE__, __LINE__);
}

i3DecalDrawInfo * i3DecalManager::_GetDrawDecal( void)
{
	i3DecalDrawInfo * pInfo;
	
	pInfo = (i3DecalDrawInfo*) m_pDecalDrawInfoPool->Alloc( sizeof(i3DecalDrawInfo), __FILE__, __LINE__);
	pInfo->setEnable( true);

	return pInfo;
}

INT32 i3DecalManager::_ParseContactReport(i3PhysixShapeSet * pShapeSet)
{
	INT32	i;
	bool	bCheckCount;
	VEC3D	vBoxVert[36];				// 박스 히트용입니다.
	UINT32 nV1, nV2, nV3;
	MATRIX rotMat;
	I3_DECAL_HITED_INFO info;

	INT32 idx = (INT32) pShapeSet->getUserData();
	I3ASSERT( (idx >= 0) && (idx < I3SG_DECAL_MAX_COUNT));

	I3SG_DECAL_HITBOX * pBox = & m_pDecalHitBox[idx];
	if( pBox->m_pParticle == nullptr ) return 0;

	I3_PHYSIX_SHAPE_TYPE	type = I3_PHYSIX_SHAPE_NA;
	UINT32	nMap[ I3SG_DECAL_MAX_VERTICES ] = { 0,};
	NxInternalFormat format = NX_FORMAT_NODATA;
	// check size
	UINT32 numTrigs = 0;
	i3Vector3Array * verts = nullptr;
	VEC3D * pVerts = nullptr;
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Face 좌표 값을 버퍼에 저장합니다.
	UINT32	h = 0;

	info.m_VertexCount = 0;
	info.m_TriangleCount = 0;

	pBox->m_pDrawDecalInfo->setTriangleCount( 0);
	pBox->m_pDrawDecalInfo->setVertexCount( 0);

	NxShape * pOldDestShape = nullptr;
	void * pIndices = nullptr;

	//i3VLog::Begin();

	for( i = 0; i < pShapeSet->getContactCount(); i++ )
	{
		I3_PHYSIX_CONTACT_REPORT * pReport = pShapeSet->getContactReport( i );
		i3PhysixShapeSet * pDestShapeSet = pReport->m_pDestShapeSet;
 
		if( pDestShapeSet == nullptr ) continue;

		if( pDestShapeSet->getShapeType() == I3_PHYSIX_SHAPE_TRIMESH )
		{	// Trimesh는 바로 세팅이 가능합니다.
			type = I3_PHYSIX_SHAPE_TRIMESH;
			numTrigs = pDestShapeSet->getFaceCount();
			verts = pDestShapeSet->getPositionArray();
			UINT32 ShapeCount = pDestShapeSet->getNxShapeCount();

			if( ShapeCount > 0)
			{	
				NxShape * pShape = pDestShapeSet->getNxShape( 0);
				NxMat34 mat = pShape->getGlobalPose();
				NxMat34 localMat = pShape->getLocalPose();

				mat = localMat * mat;
				
				NXU::Copy( &rotMat, &mat);
			}
		}

		for( size_t j = 0; j < pReport->m_ContactList.size(); j++ )
		{
			I3_PHYSIX_CONTACT_INFO * pInfo = pReport->m_ContactList.at(j);

			bCheckCount = false;

			if( type != I3_PHYSIX_SHAPE_TRIMESH )
			{
				if( pOldDestShape == pInfo->m_pShape)
					continue;

				// 타입이 없다면 일단 컨벡스로 확인합니다.
				NxConvexShape * pConvexShape = pInfo->m_pShape->isConvexMesh();
				if( pConvexShape != nullptr )
				{
					format = pConvexShape->getConvexMesh().getFormat( 0, NX_ARRAY_TRIANGLES);
					if( format == NX_FORMAT_NODATA)
					{
						continue;
					}
					else
					{
						NxMat34 mat = pConvexShape->getGlobalPose();
						NXU::Copy( &rotMat, &mat );

						numTrigs = (UINT32) pConvexShape->getConvexMesh().getCount( 0, NX_ARRAY_TRIANGLES );
						pVerts = (VEC3D*) pConvexShape->getConvexMesh().getBase( 0, NX_ARRAY_VERTICES );
						pIndices = (void*) pConvexShape->getConvexMesh().getBase( 0, NX_ARRAY_TRIANGLES);

						type = I3_PHYSIX_SHAPE_CONVEXMESH;
					}
				}
				else
				{
					// 컨벡스가 아니라면 Box입니다. 그외의 Shape은 추가해줘야 합니다.
					NxBoxShape * pBoxShape = pInfo->m_pShape->isBox();
					if( pBoxShape != nullptr )
					{
						NxVec3 vDimension = pBoxShape->getDimensions();

						{
							REAL32 x1 = -vDimension.x;
							REAL32 y1 = -vDimension.y;
							REAL32 z1 = -vDimension.z;
							REAL32 x2 = vDimension.x;
							REAL32 y2 = vDimension.y;
							REAL32 z2 = vDimension.z;

							pVerts = vBoxVert;

							i3Vector::Set( &pVerts[0], x1, y2, z2);		// 0
							i3Vector::Set( &pVerts[1], x1, y1, z2);		// 1
							i3Vector::Set( &pVerts[2], x2, y2, z2);		// 2
							i3Vector::Set( &pVerts[3], x2, y2, z2);		// 3
							i3Vector::Set( &pVerts[4], x1, y1, z2);		// 4
							i3Vector::Set( &pVerts[5], x2, y1, z2);		// 5

							i3Vector::Set( &pVerts[6], x2, y2, z2);		// 6
							i3Vector::Set( &pVerts[7], x2, y1, z2);		// 7
							i3Vector::Set( &pVerts[8], x2, y2, z1);		// 8
							i3Vector::Set( &pVerts[9], x2, y2, z1);		// 9
							i3Vector::Set( &pVerts[10], x2, y1, z2);		// 10
							i3Vector::Set( &pVerts[11], x2, y1, z1);		// 11

							i3Vector::Set( &pVerts[12], x2, y2, z1);		// 12
							i3Vector::Set( &pVerts[13], x2, y1, z1);		// 13
							i3Vector::Set( &pVerts[14], x1, y2, z1);		// 14
							i3Vector::Set( &pVerts[15], x1, y2, z1);		// 15
							i3Vector::Set( &pVerts[16], x2, y1, z1);		// 16
							i3Vector::Set( &pVerts[17], x1, y1, z1);		// 17

							i3Vector::Set( &pVerts[18], x1, y2, z1);		// 18
							i3Vector::Set( &pVerts[19], x1, y1, z1);		// 19
							i3Vector::Set( &pVerts[20], x1, y2, z2);		// 20
							i3Vector::Set( &pVerts[21], x1, y2, z2);		// 21
							i3Vector::Set( &pVerts[22], x1, y1, z1);		// 22
							i3Vector::Set( &pVerts[23], x1, y1, z2);		// 23

							i3Vector::Set( &pVerts[24], x1, y2, z1);		// 24
							i3Vector::Set( &pVerts[25], x1, y2, z2);		// 25
							i3Vector::Set( &pVerts[26], x2, y2, z1);		// 26
							i3Vector::Set( &pVerts[27], x2, y2, z1);		// 27
							i3Vector::Set( &pVerts[28], x1, y2, z2);		// 28
							i3Vector::Set( &pVerts[29], x2, y2, z2);		// 29

							i3Vector::Set( &pVerts[30], x1, y1, z2);		// 30
							i3Vector::Set( &pVerts[31], x1, y1, z1);		// 31
							i3Vector::Set( &pVerts[32], x2, y1, z2);		// 32
							i3Vector::Set( &pVerts[33], x2, y1, z2);		// 33
							i3Vector::Set( &pVerts[34], x1, y1, z1);		// 34
							i3Vector::Set( &pVerts[35], x2, y1, z1);		// 35

							// Box는 WorldMatrix만 계산해주어야 합니다.
							NxMat34 mat = pBoxShape->getGlobalPose();
							
							NXU::Copy( &rotMat, &mat );

							for( INT32 k = 0; k < 36; k++)
							{
								i3Vector::TransformCoord( &pVerts[ k], &pVerts[ k], &rotMat);
							}
						}

						numTrigs = 12;
						pIndices = m_BoxIndices;

						type = I3_PHYSIX_SHAPE_BOX;
					}
					else
					{	// another shapes add line
						I3ASSERT_0;
					}
				}
			}
			else
			{
				// TriMesh에서 해당 Face가 등록되어 있는지 체크합니다. 이미 등록되어 있다면 다시 등록할 필요가 없기 때문입니다.
				if( pInfo->m_FaceIndex == 0xffffffff || pInfo->m_FaceIndex >= numTrigs )
					continue;

				bool bCheckSame = false;
				for( UINT32 k = 0; k < h; k++ )
				{	
					if( nMap[ k ] == pInfo->m_FaceIndex )
					{
						bCheckSame = true;
						break;
					}
				}

				if( bCheckSame )		continue;

				nMap[ h++ ] = pInfo->m_FaceIndex;
				if( h >= I3SG_DECAL_MAX_VERTICES )
					break;
			}

			I3_PHYSIX_TERRAIN * pTerrain = nullptr;
			I3_PHYSIX_TRI * pFace = pDestShapeSet->getFace( pInfo->m_FaceIndex );

			if( pFace == nullptr )
			{
				// TriMesh가 아닙니다. 해당 shape의 Terrain정보를 직접 가져옵니다.
				if( pDestShapeSet->getTerrainCount() > 0 )
				{
					for( INT32 k = 0; k < pDestShapeSet->getNxShapeCount(); k++)
					{
						if( pDestShapeSet->getNxShape( k) == pInfo->m_pShape )
						{
							pTerrain = pDestShapeSet->getTerrain( k);
							break;
						}
					}
				}
			}
			else
			{
				pTerrain = pDestShapeSet->getTerrain( pFace->m_MaterialIndex );
			}
			
			if( (pBox->m_pParticle->getTerrainType() != 0) && ((pTerrain != nullptr) && (pTerrain->m_Terrain != pBox->m_pParticle->getTerrainType() )) )
			{
				// Face의 정보가 다르다면(terrain type이 다르면) 해당 데칼은 다른 종류입니다. 일단 스킵합니다.
				continue;
			}

			switch( type )
			{
			case I3_PHYSIX_SHAPE_TRIMESH :
                {
					NxTriangleMeshShape * pTriMeshShape = pInfo->m_pShape->isTriangleMesh();
					if( pTriMeshShape == nullptr || pFace == nullptr)
						continue;

					nV1 = pFace->m_Index[ 0 ];
					nV2 = pFace->m_Index[ 1 ];
					nV3 = pFace->m_Index[ 2 ];

					VEC3D v1, v2, v3;
					i3Vector::TransformCoord( &v1, verts->GetVector( nV1), &rotMat);
					i3Vector::TransformCoord( &v2, verts->GetVector( nV2), &rotMat);
					i3Vector::TransformCoord( &v3, verts->GetVector( nV3), &rotMat);
					
					bCheckCount = _AddTriangleData( &info, &v1, &v2, &v3 );
				}
				break;

			case I3_PHYSIX_SHAPE_CONVEXMESH:
				{
					// 해당 Shape에 대한 모든 면들을 등록합니다.
					for( UINT32 k = 0; k < numTrigs; k++ )
					{
						if( format == NX_FORMAT_SHORT)
						{	
							UINT16 * pIndex = (UINT16*) pIndices;
							nV1 = pIndex[ k * 3];
							nV2 = pIndex[ k * 3 +1];
							nV3 = pIndex[ k * 3 +2];
						}
						else if( format == NX_FORMAT_INT )
						{
							UINT32 * pIndex = (UINT32*) pIndices;
							nV1 = (UINT32) pIndex[ k * 3];
							nV2 = (UINT32) pIndex[ k * 3 +1];
							nV3 = (UINT32) pIndex[ k * 3 +2];
						}
						else
						{
							I3PRINTLOG(I3LOG_FATAL,  "Wrong index." );
							break;
						}

						VEC3D v1, v2, v3;
						i3Vector::TransformCoord( &v1, &pVerts[ nV1], &rotMat);
						i3Vector::TransformCoord( &v2, &pVerts[ nV2], &rotMat);
						i3Vector::TransformCoord( &v3, &pVerts[ nV3], &rotMat);
						
						bCheckCount = _AddTriangleData( &info, &v1, &v2, &v3 );
						if( !bCheckCount )
							break;
					}
				}
				break;

			case I3_PHYSIX_SHAPE_BOX:
				{
					I3ASSERT( pIndices != nullptr);

					UINT32 * pIndex = (UINT32*) pIndices;

					for( UINT32 k = 0; k < numTrigs; k++ )
					{
						nV1 = (UINT32) pIndex[ k * 3];
						nV2 = (UINT32) pIndex[ k * 3 +1];
						nV3 = (UINT32) pIndex[ k * 3 +2];
						
						bCheckCount = _AddTriangleData( &info, &pVerts[ nV1 ], &pVerts[ nV2 ], &pVerts[ nV3 ] );
						if( !bCheckCount )
							break;
					}
				}
				break;
			}

			pOldDestShape = pInfo->m_pShape;

			if( !bCheckCount )
				break;
		}
	}

	// 절단면을 계산합니다.
	// Update Decal..
	if( info.m_TriangleCount > 0 )
	{
		//I3TRACE( "HitFace Count : %d\n", info.m_TriangleCount);
		if( _CalcPlaneClip( &m_pDecalHitBox[idx], &info))
		{
			//I3TRACE( "Decal Face Count : %d\n", m_pDecalHitBox[idx].m_pDrawDecalInfo->getTriangleCount());
			return 1;
		}
	}

	return 0;
}

void i3DecalManager::_CancelDecalHitBox( I3SG_DECAL_HITBOX * pInfo)
{
#if defined( I3_PHYSX)
	MATRIX mat;

	I3ASSERT( pInfo != nullptr);

	i3PhysixShapeSet * pShapeSet = pInfo->m_pHitBox;

	I3ASSERT( pShapeSet != nullptr);

	// 공중으로 돌려보냅니다.
	i3Matrix::SetPos( &mat, (REAL32) ((pInfo->m_nIndex * 20) - 2560), I3_DECAL_PHYSIX_HEIGHT, 0.f, 1.f );
	pShapeSet->setShapeGlobalPose( &mat );

	// 컬리젼을 끕니다.
	pShapeSet->modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_KINEMATIC, 0 );

	// HitBox Info Reset
	pInfo->reset();
#endif
}

void i3DecalManager::FreeDecal( i3Decal * pDecal)
{
	// Hit용으로 사용된 i3Decal을 반환한다.
	m_pDecalPool->Free( pDecal);
}

void i3DecalManager::FreeDrawDecalInfo(i3DecalDrawInfo * pInfo)
{
	I3ASSERT( pInfo != nullptr);
	pInfo->setEnable( false);
	m_pDecalDrawInfoPool->Free( pInfo);
}

bool i3DecalManager::_AddPolygon( I3SG_DECAL_HITBOX * pInfo, UINT32 vertexCount, VEC3D *pVertex, VEC3D * pNormal)
{
	UINT32 i;
	VEC3D vAddPos;

	i3DecalDrawInfo * pDrawInfo = pInfo->m_pDrawDecalInfo;

	UINT32 count = pDrawInfo->getVertexCount();

	if (count + vertexCount >= I3SG_DECAL_MAX_VERTICES )
	{	// 준비된 Vertex Array가 없습니다.
		return false;
	}

	INT32 triangleCount = pDrawInfo->getTriangleCount();
	
	// Add polygon as a triangle fan
	UINT16 * triangle = &pDrawInfo->getIndices()[ triangleCount * 3 ];

	for( i = 2; i < vertexCount; i++ )
	{
		triangle[0] = (UINT16) count;
		triangle[1] = (UINT16) (count + i - 1);
		triangle[2] = (UINT16) (count + i );
		triangle +=3;
	}

	triangleCount += vertexCount - 2;
	pDrawInfo->setTriangleCount( triangleCount);

	// Assign vertex colors
	for( i = 0; i < vertexCount; i++)
	{	
		i3Vector::Normalize( &vAddPos, &pNormal[i] );
		i3Vector::Scale( &vAddPos, &vAddPos, DECAL_HIT_DEPTH );
		i3Vector::Add( &pDrawInfo->getVertices()[ count], &pVertex[i], &vAddPos );
		count++;
	}
	
	pDrawInfo->setVertexCount( count);

	return true;
}

void i3DecalManager::_ClipMesh( I3SG_DECAL_HITBOX * pInfo, UINT32 triangleCount, UINT16 *triangle, VEC3D *vertex, VEC3D *normal)
{
	UINT32 i;
	UINT16 i1, i2, i3;

	VEC3D	newVertex[9];
	VEC3D	newNormal[9];

	VEC3D v1, v2;
	VEC3D vCross;
	
	// Clip one triangle at a time
	for( i = 0; i < triangleCount; i++)
	{
		i1 = triangle[ i*3 ];
		i2 = triangle[ i*3 + 1];
		i3 = triangle[ i*3 + 2];
		
		i3Vector::Sub( &v1, &vertex[i2], &vertex[i1] );
		i3Vector::Sub( &v2, &vertex[i3], &vertex[i1] );
		
		i3Vector::Cross( &vCross, &v1, &v2 );

		if( i3Vector::Dot( pInfo->m_pDecal->getNormal(), &vCross) >
			I3SG_DECAL_DEFAULT_EPSILON * i3Vector::Length( &vCross ) )
		{
			i3Vector::Copy( &newVertex[0], &vertex[i1] );
			i3Vector::Copy( &newVertex[1], &vertex[i2] );
			i3Vector::Copy( &newVertex[2], &vertex[i3] );

			i3Vector::Copy( &newNormal[0], &normal[i1] );
			i3Vector::Copy( &newNormal[1], &normal[i2] );
			i3Vector::Copy( &newNormal[2], &normal[i3] );
			
			UINT32 count = _ClipPolygon( pInfo->m_pDecal, 3, newVertex, newNormal, newVertex, newNormal);

			if( (count != 0) && (!_AddPolygon( pInfo, count, newVertex, newNormal)))
				break;
		}
	}
}

UINT32 i3DecalManager::_ClipPolygon( i3Decal * pDecal, UINT32 vertexCount, VEC3D *vertex, VEC3D *normal, VEC3D *newVertex, VEC3D *newNormal)
{
	VEC3D tempVertex[9];
	VEC3D tempNormal[9];
	
	// Clip against all six planes
	UINT32 count = _ClipPolygonAgainstPlane( pDecal->getPlaneLeft(), vertexCount, vertex, normal, tempVertex, tempNormal);
	if( count != 0)
	{
		count = _ClipPolygonAgainstPlane( pDecal->getPlaneRight(), count, tempVertex, tempNormal, newVertex, newNormal);
		if( count != 0)
		{
			count = _ClipPolygonAgainstPlane( pDecal->getPlaneBottom(), count, newVertex, newNormal, tempVertex, tempNormal);
			if( count != 0)
			{
				count = _ClipPolygonAgainstPlane( pDecal->getPlaneTop(), count, tempVertex, tempNormal, newVertex, newNormal);
				if( count != 0)
				{
					count = _ClipPolygonAgainstPlane( pDecal->getPlaneBack(), count, newVertex, newNormal, tempVertex, tempNormal);
					if( count != 0)
					{
						count = _ClipPolygonAgainstPlane( pDecal->getPlaneFront(), count, tempVertex, tempNormal, newVertex, newNormal);
					}
				}
			}
		}
	}
	
	return count;
}

UINT32 i3DecalManager::_ClipPolygonAgainstPlane( VEC4D * plane, UINT32 vertexCount, VEC3D *vertex, VEC3D *normal, VEC3D *newVertex, VEC3D *newNormal)
{
	UINT32	i;
	bool	bNegative[10];
	VEC3D vPlane;
	VEC3D	v1, v2;
	i3Vector::Set( &vPlane, getX( plane ), getY( plane ), getZ( plane ) );
	
	// Classify vertices
	UINT32 negativeCount = 0;
	for( i = 0; i < vertexCount; i++)
	{	
		bNegative[i] = (i3Vector::Dot( &vPlane, &vertex[i]) + i3Vector::GetW( plane ) < 0.f );
		negativeCount += (UINT32) bNegative[i];
	}
	
	// Discard this polygon if it's completely culled
	if (negativeCount == vertexCount)
		return 0;
	
	UINT32 count = 0;
	for( i = 0; i < vertexCount; i++)
	{
		// 'j' is the index of the previous vertex
		UINT32 j = (i != 0) ? i - 1 : vertexCount - 1;

		VEC3D * pV1 = &vertex[j];
		VEC3D * pV2 = &vertex[i];

		VEC3D * pN1 = &normal[j];
		VEC3D * pN2 = &normal[i];
		
		if( bNegative[i] )
		{
			if( !bNegative[j] )
			{
				// Current vertex is on negative side of plane,
				// but previous vertex is on positive side.
				REAL32 t = (i3Vector::Dot( &vPlane, pV1 ) + i3Vector::GetW( plane )) / (getX(plane) * (getX(pV1) - getX(pV2)) + getY(plane) * (getY(pV1) - getY(pV2)) + getZ(plane) * (getZ(pV1) - getZ(pV2)));
				
				i3Vector::Scale( &v1, pV1, 1.f - t );
				i3Vector::Scale( &v2, pV2, t );
				i3Vector::Add( &newVertex[count], &v1, &v2 );
				
				i3Vector::Scale( &v1, pN1, 1.f - t );
				i3Vector::Scale( &v2, pN2, t );
				i3Vector::Add( &newNormal[count], &v1, &v2 );
				count++;
			}
		}
		else
		{
			if( bNegative[j] )
			{
				// Current vertex is on positive side of plane,
				// but previous vertex is on negative side.
				REAL32 t = (i3Vector::Dot( &vPlane, pV2 ) + i3Vector::GetW( plane )) / (getX(plane) * (getX(pV2) - getX(pV1)) + getY(plane) *(getY(pV2) - getY(pV1)) + getZ(plane) * (getZ(pV2) - getZ(pV1)));

				i3Vector::Scale( &v1, pV2, 1.f - t );
				i3Vector::Scale( &v2, pV1, t );
				i3Vector::Add( &newVertex[count], &v1, &v2 );
				
				i3Vector::Scale( &v1, pN2, 1.f - t );
				i3Vector::Scale( &v2, pN1, t );
				i3Vector::Add( &newNormal[count], &v1, &v2 );
				count++;
			}
			
			// Include current vertex
			i3Vector::Copy( &newVertex[count], &vertex[i] );
			i3Vector::Copy( &newNormal[count], &normal[i] );
			count++;
		}
	}
	
	// Return number of vertices in clipped polygon
	return count;
}

bool i3DecalManager::_CalcPlaneClip( I3SG_DECAL_HITBOX * pInfo, I3_DECAL_HITED_INFO * pHitInfo)
{
	// Add this point, determine which surfaces may be affected by this decal
	// and call _ClipMesh() for each one.
	_ClipMesh( pInfo, pHitInfo->m_TriangleCount, pHitInfo->m_Indices, pHitInfo->m_VertexArray, pHitInfo->m_NormalArray );

	I3ASSERT( pInfo->m_pDrawDecalInfo != nullptr);

	// DrawDecalInfo에서 저장된 Draw를 Transform한다.
	if( pInfo->m_pDrawDecalInfo->getTriangleCount() > 0)
	{
		// UV좌표를 계산한다.
		pInfo->m_pDrawDecalInfo->AssignTextureCoord( pInfo->m_pDecal->getPosition(), pInfo->m_pDecal->getTangent(), pInfo->m_pDecal->getBinormal());

		// Local 좌표로 변경한다. Scene에 붙는 Particle에서 처리되므로..
		i3Matrix::TransformPoints( pInfo->m_pDrawDecalInfo->getVertices(), pInfo->m_pDrawDecalInfo->getVertices(), pInfo->m_pDrawDecalInfo->getVertexCount(), pInfo->m_pDecal->getInverseMatrix() );

		//I3TRACE( "Draw Face : %d\n", pDrawDecal->getTriangleCount());

		return true;
	}

	return false;
}

bool i3DecalManager::_AddTriangleData( I3_DECAL_HITED_INFO * pInfo, VEC3D * pPos1, VEC3D * pPos2, VEC3D * pPos3 )
{
	I3ASSERT( pInfo != nullptr );

	i3Vector::Set( &pInfo->m_VertexArray[ pInfo->m_VertexCount ], getX( pPos1), getY( pPos1), getZ( pPos1) );
	pInfo->m_Indices[ pInfo->m_VertexCount ] = static_cast<UINT16>(pInfo->m_VertexCount);
	pInfo->m_VertexCount++;

	if( pInfo->m_VertexCount >= I3SG_DECAL_MAX_VERTICES )
	{
		pInfo->m_VertexCount--;
		//I3TRACE( "[[ Decal over vertex1\n" );
		return false;
	}
	
	i3Vector::Set( &pInfo->m_VertexArray[ pInfo->m_VertexCount ], getX( pPos2), getY( pPos2), getZ( pPos2) );
	pInfo->m_Indices[ pInfo->m_VertexCount ] = static_cast<UINT16>(pInfo->m_VertexCount);
	pInfo->m_VertexCount++;

	if( pInfo->m_VertexCount >= I3SG_DECAL_MAX_VERTICES )
	{
		pInfo->m_VertexCount-=2;
		//I3TRACE( "[[ Decal over vertex2\n" );
		return false;
	}
	
	i3Vector::Set( &pInfo->m_VertexArray[ pInfo->m_VertexCount ], getX( pPos3), getY( pPos3), getZ( pPos3) );
	pInfo->m_Indices[ pInfo->m_VertexCount ] = static_cast<UINT16>(pInfo->m_VertexCount);
	pInfo->m_VertexCount++;

	if( pInfo->m_VertexCount >= I3SG_DECAL_MAX_VERTICES )
	{
		pInfo->m_VertexCount-=3;
		//I3TRACE( "[[ Decal over vertex3\n" );
		return false;
	}

	// normal
	VEC3D vDir1, vDir2;
	i3Vector::Sub( &vDir1, pPos2, pPos1 );
	i3Vector::Sub( &vDir2, pPos3, pPos1 );
	i3Vector::Cross( &pInfo->m_NormalArray[ pInfo->m_VertexCount - 3], &vDir1, &vDir2 );
	i3Vector::Copy( &pInfo->m_NormalArray[ pInfo->m_VertexCount - 2], &pInfo->m_NormalArray[ pInfo->m_VertexCount - 3] );
	i3Vector::Copy( &pInfo->m_NormalArray[ pInfo->m_VertexCount - 1], &pInfo->m_NormalArray[ pInfo->m_VertexCount - 2] );

	pInfo->m_TriangleCount++;

	return true;
}




I3SG_DECAL_HITBOX * i3DecalManager::EmitDecal( i3SceneTracer * pTracer, i3Particle * pParticle, I3PARTICLEOBJECT * pObj)
{
	VEC3D vSize;
	MATRIX mat;

	if( pParticle == nullptr )	return nullptr;

	INT32 idx = _FindFreeHitBox();
	if( idx == -1 )
	{
		// 남은 HitBox가 없다.
		//I3PRINTLOG(I3LOG_WARN,  "No free HitBox!!!");
		return nullptr;
	}

	i3Vector::Set( &vSize, getX( &pObj->m_Scale), getY( &pObj->m_Scale), getZ( &pObj->m_Scale) * 0.5f );
	if( getX( &vSize) <= I3_EPS &&
		getY( &vSize) <= I3_EPS &&
		getZ( &vSize) <= I3_EPS)
	{
		// 사이즈가 아주 작다..
		return nullptr;
	}

	I3SG_DECAL_HITBOX * pInfo = &m_pDecalHitBox[ idx];

	pInfo->reset();

	pInfo->m_pDecal = _GetDecal();

	if( pInfo->m_pDecal == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "no more decal...!!!");
		return nullptr;
	}

	// Draw용 정보를 따로 pool한다.
	pInfo->m_pDrawDecalInfo = _GetDrawDecal();

	if( pInfo->m_pDrawDecalInfo == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "no more draw decal...!!!");
		return nullptr;
	}

	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	// 설정되었으면 사용중인 리스트로 등록
	{	
		pInfo->setParticle( pParticle);
		pInfo->m_FrameCnt = pTracer->getFrameID();
		m_EmitedDecalList.push_back( pInfo);
	}

	i3Decal * pDecal = pInfo->m_pDecal;

	pInfo->m_pDrawDecalInfo->SetOneOverSize( getX( &vSize), getY( &vSize));
	pInfo->m_pDrawDecalInfo->SetTextureCoord( &pObj->m_UV1, &pObj->m_UV2);
	
	pDecal->setCenter( &pObj->m_Pos[0]);
	pDecal->SetSize( &vSize);
	pDecal->setRotation(	pObj->m_RotateAccm);

	i3Matrix::SetRotateZ( &mat, pObj->m_RotateAccm);
	i3Matrix::PostTranslate( &mat, &pObj->m_Pos[0]);
	
	i3Matrix::Mul( &mat, &mat, pStack->GetTop());
	i3Matrix::Inverse( pDecal->getInverseMatrix(), nullptr, pStack->GetTop());
	
	pDecal->setNormal( i3Matrix::GetAt( &mat ) );
	pDecal->setTangent( i3Matrix::GetRight( &mat ) );
	pDecal->setPosition( i3Matrix::GetPos( &mat ) );

	pDecal->CalcBoundaryPlane();
	
	pObj->m_State |= I3_PARTICLE_STATE_FIRE_DECAL;
	
	// Physics Shape을 설정한다.
	{
		i3PhysixShapeSet * pHitBox = pInfo->m_pHitBox;

		pHitBox->setUserData( (void *) idx);
		
		{	// Set Size
			NxShape * pShape = pHitBox->getNxShape( 0 );
			NxBoxShape * pBoxShape = pShape->isBox();

			i3Vector::Scale( &vSize, &vSize, 0.5f );

			pBoxShape->setSkinWidth( 0.01f);

			g_pPhysixCtx->CMD_setDimension( pBoxShape, &vSize);
		}

		// 컬리젼을 킵니다.
		pHitBox->modifyDynamicState( 0, I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_KINEMATIC);
		pHitBox->Reset();
		
		// move shapeset
		pHitBox->setShapeGlobalPose( &mat );
	}

	return pInfo;
}



void i3DecalManager::ProcessDecal( i3SceneTracer * pTracer)
{
#if defined( I3_PHYSX)
	
	I3SG_DECAL_HITBOX * pBox;

	for(size_t i = 0; i < m_EmitedDecalList.size(); i++)
	{
		pBox = m_EmitedDecalList[i];

		UINT32 diff = pTracer->getFrameID() - pBox->m_FrameCnt;

		if( diff >= 2)
		{
			// 사용이 만기된 HitBox
			if( pBox->m_pParticle != nullptr)
			{
				// Hit용으로 사용된 i3Decal을 반환한다.
				//m_pDecalPool->Free( pBox->m_pDecal);

				// HitBox를 초기화시킨다.
				_CancelDecalHitBox( pBox);
			}

			m_EmitedDecalList.erase(m_EmitedDecalList.begin() + i);
			i--;
		}
	}
#endif
}

void i3DecalManager::ProcessEventContactHitBox( i3PhysixShapeSet * pShapeSet)
{	
	INT32 idx = (INT32) pShapeSet->getUserData();

	I3ASSERT(( idx >= 0) && (idx < I3SG_DECAL_MAX_COUNT));

	I3SG_DECAL_HITBOX * pBox = &m_pDecalHitBox[ idx];

	// 생성된 Decal이 없으면 한프레임을 더 기다려봅니다.
	// Physix에서 Contact가 순차적으로 옵니다.
	// 그리려는 면과 다른 면에 대한 Contact가 먼저 발생하면 만들어지는 Decal이 없습니다.
	// 이에 따라 다음 Contact 정보를 받아서 있으면 그리게된다.
	if( pBox->m_pParticle != nullptr )
	{
		//I3TRACE( "Physix Hit..!!!\n");
		if( _ParseContactReport( pShapeSet) )
		{
			// HitBox를 초기화한다.
			_CancelDecalHitBox( pBox);
		}
	}
}


