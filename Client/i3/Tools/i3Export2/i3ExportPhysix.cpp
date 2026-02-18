#include "stdafx.h"
#include "i3Export.h"
#include "simpobj.h"
#include "i3EXPTriObject.h"
#include "Util.h"

#include "NXU_helper.h"
#include "NXU_schema.h"
#include "NXU_cooking.h"
#include "NXU_streaming.h"

#include "i3Base/pool/create_by_common_pool.h"
#include "i3Base/string/ext/atoi.h"

static INT32	_FindTerrainIndex( i3tool::exporter::TTerrainList * pList, I3_PHYSIX_TERRAIN * pTerrain)
{
	INT32 i;
	I3_PHYSIX_TERRAIN * pTemp;

	for( i = 0; i < (INT32) pList->GetCount(); i++)
	{
		pTemp = (I3_PHYSIX_TERRAIN *) pList->GetBuffer() + i;

		if(	(pTemp->m_Terrain == pTerrain->m_Terrain) &&
			(pTemp->m_Color[0] == pTerrain->m_Color[0]) &&
			(pTemp->m_Color[1] == pTerrain->m_Color[1]) &&
			(pTemp->m_Color[2] == pTerrain->m_Color[2]) &&
			(pTemp->m_Color[3] == pTerrain->m_Color[3]) &&
			(pTemp->m_Thickness == pTerrain->m_Thickness) )
		{
			return i;
		}
	}

	// 새로 추가한다.
	pList->SetCount( i + 1);
	pTemp = pList->GetBuffer() + i;

	i3mem::Copy( pTemp, pTerrain, sizeof(I3_PHYSIX_TERRAIN));

	return i;
}

static i3MemoryBuffer *  _BuildTerrainArray( i3tool::exporter::TTerrainList * pList)
{
	UINT32 sz;
	i3MemoryBuffer * pBuffer;

	if( pList->GetCount() <= 0)
		return NULL;

	pBuffer = i3MemoryBuffer::new_object_ref();

	sz = sizeof(I3_PHYSIX_TERRAIN) * pList->GetCount();
	pBuffer->Create( sz);

	i3mem::Copy( pBuffer->getBuffer(), pList->GetBuffer(), sz);

	return pBuffer;
}

static i3MemoryBuffer * _BuildFaceArray( i3tool::exporter::TFaceList * pList)
{
	UINT32 sz;
	i3MemoryBuffer * pBuffer;

	if( pList->GetCount() <= 0)
		return NULL;

	pBuffer = i3MemoryBuffer::new_object_ref();

	sz = sizeof(I3_PHYSIX_TRI) * pList->GetCount();
	pBuffer->Create( sz);

	i3mem::Copy( pBuffer->getBuffer(), pList->GetBuffer(), sz);

	return pBuffer;
}

bool i3Export::_ExtractGeom( INode * pNode, i3Vector3Array * pVA, i3::vector<INT32>& IA, i3tool::exporter::TTerrainList * pTerrainList, i3tool::exporter::TFaceList * pFaceList, Matrix3 * pLocalMtx)
{
	INT32 j, vx[3], k;
	TimeValue time = m_timeStart;
	bool bNegScale;
	INT32 vidx[3];
	Face * pFace;
	Matrix3 PivotTrans;
	VEC3D vec;
	Mesh * pMesh;
	bool bVertexColor = false;
	I3_PHYSIX_TERRAIN terrain;
	I3_PHYSIX_TRI	tri;

	i3EXPTriObject triobj( pNode, m_timeStart);

	if( triobj.GetTriObject() == NULL)
	{
		return false;
	}

	pMesh = triobj.GetMesh();

	// Pivot Transform
	GetPivotMatrix( pNode, &PivotTrans, time);

	PivotTrans = PivotTrans * *pLocalMtx;

	{
		// we take the lowest level object of the 3ds max pipeline
		Object* object = pNode->GetObjOrWSMRef();

		// we check for the winding orders of the triangle
		Matrix3 tm = pNode->GetObjTMAfterWSM( time);
		bNegScale = TMNegParity( tm);
	}

	if( bNegScale)
	{
		// Minus Scale에 의해 뒤집혀 있는 경우...
		vx[0] = 2;	vx[1] = 1;	vx[2] = 0;
	}
	else
	{
		vx[0] = 0;	vx[1] = 1;	vx[2] = 2;
	}

	if( pMesh->mapSupport( 0) && (pMesh->getNumVertCol() > 0))
		bVertexColor = true;

	i3mem::FillZero( &terrain, sizeof(I3_PHYSIX_TERRAIN));

	// Node의 이름으로 지형을 판별한다.
	// Terrain
	if( pNode->GetName() != NULL)
	{
		terrain.m_Terrain = i3OptBuildCollisionMesh::GetTerrainTypeByName( pNode->GetName());
		//terrain.m_Thickness = i3OptBuildCollisionMesh::GetTerrainThickness( (I3_TERRAIN_TYPE*)&terrain.m_Terrain, pNode->GetName());

		TSTR temp;

		pNode->GetUserPropString( "Thickness", temp );
		terrain.m_Thickness = i3::atoi( (const char*) temp.data());
	}
	else
	{
		terrain.m_Terrain = I3_TERRAIN_NA;
		terrain.m_Thickness = -1;
	}

	// Portal ID
	{
		TSTR temp;

		pNode->GetUserPropString( "Portal", temp);
		terrain.m_PortalID = atoi( (const char *) temp.data());
	}

	// Reverb

	pVA->SetVectorCount( pMesh->getNumFaces() * 3);		// 메모리 할당용
	pVA->_forceSetVectorCount( 0);					// 누적 Count용...Buggy

	IA.resize( pMesh->getNumFaces() * 3);

	for( j = 0; j < pMesh->getNumFaces(); j++)
	{
		pFace = &(pMesh->faces[ j]);

		vidx[0] = pFace->v[ vx[0]];
		vidx[1] = pFace->v[ vx[1]];
		vidx[2] = pFace->v[ vx[2]];

		// Position
		for( k = 0; k < 3; k++)
		{
			Point3 pos;
			INT32 idx;

			pos = pMesh->getVert( vidx[k]);
			pos = PivotTrans.PointTransform( pos);
			Convert( &vec, pos);

			idx = pVA->Find( &vec, pVA->GetVectorCount());

			if( idx == -1)
			{
				// 추가
				idx = pVA->GetVectorCount();
				pVA->SetVector( idx, &vec);
				pVA->_forceSetVectorCount( idx + 1);
			}

			tri.m_Index[k] = idx;
//			pIA->SetItem( (j * 3) + k, (void *) idx);
			i3::vu::set_value_force(IA, (j * 3) + k,  idx);
		}

		// Vertex Color
		if( bVertexColor)
		{
			VertColor col;
			TVFace * pTFace;

			pTFace = pMesh->vcFace + j;

			// 실제로는 각 Vertex마다 다른 Color를 가질 수 있으나,
			// 여기서는 첫번째 Vertex Color만 취한다.
			col = pMesh->vertCol[ pTFace->t[ vx[0]]];

			terrain.m_Color[0]	= (UINT8)(col.x * 255.0f);				// r
			terrain.m_Color[1]	= (UINT8)(col.y * 255.0f);				// g
			terrain.m_Color[2]	= (UINT8)(col.z * 255.0f);				// b
			terrain.m_Color[3]	= (UINT8)(255);							// a
		}
		else
		{
			terrain.m_Color[0] = 0;
			terrain.m_Color[1] = 0;
			terrain.m_Color[2] = 0;
			terrain.m_Color[3] = 0;
		}

		tri.m_MaterialIndex = _FindTerrainIndex( pTerrainList, &terrain);

		pFaceList->Add( tri);
	}

	return true;
}

static void calcCenter( i3Vector3Array * pVA, VEC3D * pCenter)
{
	UINT32 i;

	i3Vector::Set( pCenter, 0.0f, 0.0f, 0.0f);

	for( i = 0; i < pVA->GetVectorCount(); i++)
	{
		i3Vector::Add( pCenter, pCenter, pVA->GetVector( i));
	}

	i3Vector::Scale( pCenter, pCenter, 1.0f / pVA->GetVectorCount());
}

NxBoxShapeDesc *	i3Export::_BakeBoxShape( INode * pNode, i3Vector3Array * pVA, const i3::vector<INT32>& IA, Matrix3 * pLocalMtx)
{
	i3Vector3Array temp;
	size_t i;
	INT32	cnt, idxUp;
	VEC3D d1, d2, norm, * p1, * p2, * p3;
	i3BoundBox org_box;

	temp.SetVectorCount( 6);

	// 모든 Face를 돌면서 Normal을 구한다.
	for( i = 0, cnt = 0; i < IA.size() / 3; i++)
	{
		INT32 idx = (i * 3);

		p1 = pVA->GetVector( IA[idx + 0]);
		p2 = pVA->GetVector( IA[idx + 1]);
		p3 = pVA->GetVector( IA[idx + 2]);

		org_box.ExtendByVec( p1);
		org_box.ExtendByVec( p2);
		org_box.ExtendByVec( p3);

		i3Vector::Sub( &d1, p2, p1);
		i3Vector::Sub( &d2, p3, p2);
		i3Vector::Cross( &norm, &d1, &d2);
		i3Vector::Normalize( &norm, &norm);

		if( temp.Find( &norm, cnt, 0.00005f) == -1)
		{
			// 서로 다른 Normal이 6개 이상이 나온다면, 이미 Cube가 아니라고 판단.
			if( cnt >= 6)
				return NULL;

			temp.SetVector( cnt, &norm);
			cnt++;
		}
	}

	// 한 Normal을 선택한 후, 다른 것들과 Dot를 취해, 항상 0.0f이어야 한다.
	for( i = 1; i < 6; i++)
	{
		REAL32 dot = i3Vector::Dot( temp.GetVector(0), temp.GetVector(i));

		if( (i3Math::abs( dot) < I3_EPS))
		{
			idxUp = i;
		}
		else if( i3Math::abs( dot + 1.0f) < I3_EPS)
		{
			// 반대면.
		}
		else
		{
			// 직각을 유지하지 못하고 있음.
			return NULL;
		}
	}

	// 정확한 확인을 위해, 직교로 찾은 Up Vector와 다른 것들의 Dot도 확인한다.
	for( i = 1; i < 6; i++)
	{
		if( i == idxUp)
			continue;

		REAL32 dot = i3Vector::Dot( temp.GetVector(idxUp), temp.GetVector(i));

		if( (i3Math::abs( dot) < I3_EPS))
		{
		}
		else if( i3Math::abs( dot + 1.0f) < I3_EPS)
		{
		}
		else
		{
			// 직각을 유지하지 못하고 있음.
			return NULL;
		}
	}

	MATRIX mtxLocal, inv;
	i3BoundBox box;

	i3Matrix::MakeWithAt( &mtxLocal, temp.GetVector(0), temp.GetVector( idxUp));
	i3Matrix::SetPos( &mtxLocal, org_box.getCenter());

	i3Matrix::Inverse( &inv, NULL, &mtxLocal);

	// Local Space로 되돌린 후, Min, Max Bound를 구한다.
	for( i = 0; i < (INT32) pVA->GetVectorCount(); i++)
	{
		VEC3D pos;

		i3Vector::TransformCoord( &pos, pVA->GetVector(i), &inv);

		box.ExtendByVec( &pos);
	}

	NxBoxShapeDesc * pDesc = new NxBoxShapeDesc;

	pDesc->dimensions.x = i3Math::abs( box.GetMax()->x - box.GetMin()->x) * 0.5f;
	pDesc->dimensions.y = i3Math::abs( box.GetMax()->y - box.GetMin()->y) * 0.5f;
	pDesc->dimensions.z = i3Math::abs( box.GetMax()->z - box.GetMin()->z) * 0.5f;

	::Convert( pLocalMtx, &mtxLocal);
	NXU::Copy( &pDesc->localPose, &mtxLocal);

	if( pDesc->isValid() == false)
	{
		Log( pNode, "Box Shape를 생성하려 시도했으나 값이 올바르지 않습니다.");
		delete pDesc;

		return NULL;
	}

	I3TRACE( "BOX Shape\n");

	return pDesc;
}

NxConvexShapeDesc * i3Export::_BakeConvexMesh( INode * pNode, i3Vector3Array * pVA, const i3::vector<INT32>& IA, Matrix3 * pLocalMtx)
{
	NxConvexMeshDesc desc;
	NxConvexMesh * pNxMesh;
	NxConvexShapeDesc * pShapeDesc = NULL;

	desc.numVertices = pVA->GetVectorCount();
	desc.points = pVA->GetVectorArray();
	desc.pointStrideBytes = sizeof(VEC3D);
	desc.flags	= 0;
	if( m_Option.m_bCreateRappingConvex )
		desc.flags = NX_CF_COMPUTE_CONVEX;

	desc.numTriangles = IA.size() / 3;
	desc.triangles = &IA[0];	//	pIA->Items;
	desc.triangleStrideBytes = sizeof(UINT32) * 3;

	pLocalMtx->IdentityMatrix();

	NXU::InitCooking();

	{
		NXU::MemoryWriteBuffer buf;

		if( NXU::CookConvexMesh( desc, buf) == false)
		{
			// Convex의 Cooking이 불가능한 Mesh...
			// Error로 간주하지 않는다.
			goto ExitPart;
		}

		pNxMesh = m_pPhysixCtx->getSDK()->createConvexMesh( NXU::MemoryReadBuffer(buf.data));
		if( pNxMesh == NULL)
		{
			// Convex의 Cooking이 불가능한 Mesh...
			goto ExitPart;
		}

		// Shape Creation
		{
			pShapeDesc = new NxConvexShapeDesc;

			pShapeDesc->meshData = pNxMesh;
			pShapeDesc->meshFlags = 0;
			pShapeDesc->shapeFlags = 0;
		}
	}


ExitPart:

	NXU::CloseCooking();

	NXU::releasePersistentMemory();

	if( pShapeDesc != NULL)
	{
		if( pShapeDesc->isValid() == false)
		{
			Log( pNode, "Convex Shape를 생성하려 했으나 실패했습니다.");
			delete pShapeDesc;

			return NULL;
		}
	}

	return pShapeDesc;
}

NxTriangleMeshShapeDesc * i3Export::_BakeTriMesh( INode * pNode, i3Vector3Array * pVA, const i3::vector<INT32>& IA, i3tool::exporter::TFaceList * pFaceList, i3PhysixShapeSet * pShapeSet, Matrix3 * pLocalMtx)
{
	NxTriangleMeshDesc desc;
	NxTriangleMesh * pNxMesh;
	NxTriangleMeshShapeDesc * pShapeDesc = NULL;

	pShapeSet->setShapeType( I3_PHYSIX_SHAPE_TRIMESH);

	{
		i3MemoryBuffer * pBuf;

		// Face Array
		pBuf = _BuildFaceArray( pFaceList);
		pShapeSet->setFaceArray( pBuf);
		pShapeSet->setFaceCount( pFaceList->GetCount());

		// Position Array
		pShapeSet->setPositionArray( pVA);
	}
	
	desc.numVertices = pVA->GetVectorCount();
	desc.points = pVA->GetVectorArray();
	desc.pointStrideBytes = sizeof(VEC3D);

	desc.numTriangles = IA.size() / 3;
	desc.triangles = &IA[0];
	desc.triangleStrideBytes = sizeof(UINT32) * 3;

	desc.materialIndexStride = 0;
	desc.materialIndices = NULL;

	desc.convexEdgeThreshold = 0.0001f;
	
	if( g_pPhysixCtx->IsHardwarePresent() )
		desc.flags = NX_MF_HARDWARE_MESH;
	//desc.flags = NX_MF_16_BIT_INDICES;

	pLocalMtx->IdentityMatrix();

	NXU::InitCooking();

	{
		NXU::MemoryWriteBuffer buf;

		if( NXU::CookTriangleMesh( desc, buf) == false)
		{
			pShapeSet->setTerrainArray( NULL);
			pShapeSet->setTerrainCount( 0);
			pShapeSet->setFaceArray( NULL);
			pShapeSet->setFaceCount( 0);
			pShapeSet->setPositionArray( NULL);

			goto ExitPart;
		}

		pNxMesh = m_pPhysixCtx->getSDK()->createTriangleMesh( NXU::MemoryReadBuffer(buf.data));
		if( pNxMesh == NULL)
		{
			pShapeSet->setTerrainArray( NULL);
			pShapeSet->setTerrainCount( 0);
			pShapeSet->setFaceArray( NULL);
			pShapeSet->setFaceCount( 0);
			pShapeSet->setPositionArray( NULL);

			goto ExitPart;
		}

		// Shape Creation
		{
			pShapeDesc = new NxTriangleMeshShapeDesc;

			pShapeDesc->meshData = pNxMesh;
			pShapeDesc->meshFlags = 0;
			//pShapeDesc->materialIndex
		}
	}

ExitPart:

	NXU::CloseCooking();

	NXU::releasePersistentMemory();

//	pIA->Release();

	pVA->Release();

	if( pShapeDesc != NULL)
	{
		if( pShapeDesc->isValid() == false)
		{
			Log( pNode, "Triangle Mesh Shape를 생성하려 했으나 실패했습니다.");
			delete pShapeDesc;

			return NULL;
		}

		I3TRACE( "TRI Shape\n");
	}

	return pShapeDesc;
}

i3tool::exporter::SHAPEDESC_INFO *	i3Export::_BuildShapeDesc( INode * pNode, Matrix3 * pLocalMtx, i3PhysixShapeSet * pShapeSet, bool bTriMeshEnable)
{
	i3Vector3Array * pVA;
	
	i3tool::exporter::TFaceList faceList;
	NxShapeDesc * pShapeDesc = NULL;
	i3tool::exporter::TTerrainList *	pTerrainList;
	i3tool::exporter::SHAPEDESC_INFO * pInfo;

	pVA = i3Vector3Array::new_object();
	i3::vector<INT32>	IA;

	pTerrainList =	i3::create_by_common_pool(i3::identity<i3tool::exporter::TTerrainList>());	// (i3tool::exporter::TTerrainList *) NEW i3tool::exporter::TTerrainList;

	_ExtractGeom( pNode, pVA, IA, pTerrainList, &faceList, pLocalMtx);

	pInfo = new i3tool::exporter::SHAPEDESC_INFO;

	pInfo->m_pTerrainList = pTerrainList;

	if( !bTriMeshEnable )
	{	
		if( m_Option.m_bCreateForSimulate )
		{
			// 우선 항상 Box를 생성하도록 시도...
			if( m_Option.m_bExportShapeAsConvex == FALSE)
			{
				pInfo->m_pShapeDesc = _BakeBoxShape( pNode, pVA, IA, pLocalMtx);

				if( pInfo->m_pShapeDesc != NULL)
				{
					// Box 생성 성공
					I3_SAFE_RELEASE( pVA);
//					I3_SAFE_RELEASE( pIA);
					return pInfo;
				}
			}

			// Convex 생성 시도.
			{
				pInfo->m_pShapeDesc = _BakeConvexMesh( pNode, pVA, IA, pLocalMtx);
				if( pInfo->m_pShapeDesc != NULL)
				{
					// Convex 성공
					I3_SAFE_RELEASE( pVA);
//					I3_SAFE_RELEASE( pIA);
					return pInfo;
				}
			}
		}

		I3_SAFE_RELEASE( pVA);
//		I3_SAFE_RELEASE( pIA);

		//	delete pTerrainList;
		i3::delete_by_common_pool(pTerrainList);
		delete pInfo;
	
		return NULL;
	}

	pInfo->m_pShapeDesc = _BakeTriMesh( pNode, pVA, IA, &faceList, pShapeSet, pLocalMtx);

	return pInfo;
}

bool i3Export::_IsPhysixNode( INode * pNode)
{
	int PhysicsType;

	// User-Defined Properties에서 "PhysicsType" 항목을 확인한다.
	if( pNode->GetUserPropInt( TSTR( "PhysicsType"), PhysicsType) == FALSE)
		return false;

	switch( PhysicsType)
	{
		case 4 :	// Non-Physical
		case 5 :	// undefined
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
namespace
{
	struct FIND_ATTACH_BONE_INFO
	{
		char * m_pszName = nullptr;
		i3Node * m_pResult = nullptr;
	};
}

static I3SG_TRAVERSAL_RESULT _FindAttachBoneByNameProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	FIND_ATTACH_BONE_INFO * pData = (FIND_ATTACH_BONE_INFO *) pUserData;

	if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::static_meta()))
	{
		if( pNode->hasName())
		{
			if( i3::generic_is_iequal( pNode->GetName(), pData->m_pszName) )
			{
				pData->m_pResult = pNode;
				return I3SG_TRAVERSAL_STOP;
			}
		}
	}
	else if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		i3Body * pBody = (i3Body *) pNode;

		if( pBody->FindBoneByName( pData->m_pszName) != -1)
		{
			pData->m_pResult = pNode;
			return I3SG_TRAVERSAL_STOP;
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

i3Node * i3Export::_FindAttachBoneByName( char * pszName)
{
	FIND_ATTACH_BONE_INFO data;

	data.m_pszName = pszName;
	data.m_pResult = NULL;

	i3Scene::Traverse( m_pRoot, _FindAttachBoneByNameProc, &data);

	return data.m_pResult;
}

void	NxCopy( NxMat34 * pDest, MATRIX * pSrc)
{
	pDest->M.setColumn( 0, *((NxVec3 *) i3Matrix::GetRight( pSrc)));
	pDest->M.setColumn( 1, *((NxVec3 *) i3Matrix::GetUp( pSrc)));
	pDest->M.setColumn( 2, *((NxVec3 *) i3Matrix::GetAt( pSrc)));
	pDest->t.x = i3Vector::GetX( i3Matrix::GetPos( pSrc));
	pDest->t.y = i3Vector::GetY( i3Matrix::GetPos( pSrc));
	pDest->t.z = i3Vector::GetZ( i3Matrix::GetPos( pSrc));
}

#define	__PX_DUMP

#if defined( __PX_DUMP)
	#define	PXLOG				I3TRACE
#else
	#define	PXLOG				__noop
#endif

i3tool::exporter::SHAPEDESC_INFO * i3Export::_CreateShapeDesc( INode * pNode, i3PhysixShapeSet * pShapeSet, Matrix3 * pBaseMtx, INT32 geometryType, REAL32 mass, REAL32 density )
{
	TimeValue time = m_timeStart;
	ObjectState os = pNode->EvalWorldState( time);
	ulong partA = os.obj->ClassID().PartA();
	ulong partB = os.obj->ClassID().PartB();
	NxShapeDesc * pShapeDesc = NULL;
	i3tool::exporter::SHAPEDESC_INFO * pInfo = NULL;
	INT32 idx;;
	Matrix3 adjustMtx;

	PXLOG( "[PX] %s : ", pNode->GetName());

	adjustMtx.IdentityMatrix();

	Matrix3 worldMtx = __NODE_TM( pNode, time);
	Matrix3 localMtx;

	if( pBaseMtx != NULL)
	{
		localMtx = worldMtx * *pBaseMtx;
	}
	else
	{
		localMtx = worldMtx;
	}

	switch( partA)
	{
		// Sphere -> NxSphereShapeDesc
		case SPHERE_CLASS_ID :		
		// Mesh -> Convex, Box
		case CYLINDER_CLASS_ID :
		case CONE_CLASS_ID :
		case BOXOBJ_CLASS_ID :		
			{
				pInfo = _BuildShapeDesc( pNode, &localMtx, pShapeSet);
			}
			break;
		
		default :
			if( geometryType == 1 || geometryType == 0)
			{	// Automatic
				if( os.obj->ClassID() == CAPSULE_CLASS_ID)
				{
					geometryType = 2;	// auto change Capsule shape
					PXLOG( "[PX-CAPSULE]");
				}
				else if( os.obj->ClassID() == PX_HULL_CLASS_ID )
				{
					geometryType = 5;	// auto change Convex shape
					PXLOG( "[PX-HULL]");
				}
				else if( os.obj->ClassID() == PX_AGGREGATE_HULL_CLASS_ID)
				{
					geometryType = 5;	// auto change Convex shape
					PXLOG( "[PX-AGGREGATE-HULL]");
				}
				else if( os.obj->ClassID() == PX_D6_JOINT_CLASS_ID)
				{
					geometryType = 0;	// no shape
					PXLOG( "[PX-D6JOINT]");
				}
				else
				{
					//geometryType = 6;	// auto change TriMesh shape
					pInfo = _BuildShapeDesc( pNode, &localMtx, pShapeSet);
					if( pInfo == NULL )
					{
						pInfo = _BuildShapeDesc( pNode, &localMtx, pShapeSet, true);
					}
				}
			}
			

			{
				if( geometryType == 2 )
				{
					// Capsule -> NxCapsuleShapeDesc
					if( m_Option.m_bExportShapeAsConvex == FALSE)
					{
						NxCapsuleShapeDesc * pDesc = new NxCapsuleShapeDesc;

						pShapeDesc = pDesc;
						pDesc->setToDefault();

						IParamArray * pParams = os.obj->GetParamBlock();
						Interval intval;

						idx = os.obj->GetParamBlockIndex( CYLINDER_RADIUS);
						pParams->GetValue( idx, 0, pDesc->radius, intval);

						idx = os.obj->GetParamBlockIndex( CYLINDER_HEIGHT);
						pParams->GetValue( idx, 0, pDesc->height, intval);

						pDesc->height -= (pDesc->radius * 2.0f);

						{
							adjustMtx.SetRow( 0, Point3( 0.0f, 0.0f, 1.0f));
							adjustMtx.SetRow( 1, Point3( 1.0f, 0.0f, 0.0f));
							adjustMtx.SetRow( 2, Point3( 0.0f, 1.0f, 0.0f));
							adjustMtx.SetRow( 3, Point3( pDesc->height * 0.5f, 0.0f, 0.0f));
						}

						PXLOG( "CAPSULE r(%f) l(%f) ", pDesc->radius, pDesc->height);

					}
					else
					{
						pInfo = _BuildShapeDesc( pNode, &localMtx, pShapeSet);
					}
				}
				else if( geometryType == 3 || geometryType == 4 || geometryType == 5 )
				{
					pInfo = _BuildShapeDesc( pNode, &localMtx, pShapeSet);
				}
				else if( geometryType == 6 )
				{	
					// NxTriangleMeshDesc를 사용하여 추가.
					pInfo = _BuildShapeDesc( pNode, &localMtx, pShapeSet, true);
				}
			}
			break;
	}

	if( pInfo != NULL)
	{
		pShapeDesc = pInfo->m_pShapeDesc;
	}
	else
	{
		pInfo = new i3tool::exporter::SHAPEDESC_INFO;

		pInfo->m_pShapeDesc = pShapeDesc;

		pInfo->m_pTerrainList =	i3::create_by_common_pool(i3::identity<i3tool::exporter::TTerrainList>()); // (i3tool::exporter::TTerrainList *) NEW i3tool::exporter::TTerrainList;

		I3_PHYSIX_TERRAIN t;

		i3mem::FillZero( &t, sizeof(t));
		t.m_Terrain = 1;
		t.m_Color[0] = 255;
		t.m_Color[1] = 255;
		t.m_Color[2] = 255;
		t.m_Color[3] = 255;
		t.m_PortalID = 0;
		t.m_Reverb = 0;
		t.m_Thickness = 1;

		pInfo->m_pTerrainList->Add( t);
	}

	if( pShapeDesc != NULL)
	{
		// localPose
		{
			Point3 t;

			localMtx = adjustMtx * localMtx;

			{
				MATRIX tmp;

				Convert( &tmp, &localMtx);

				NXU::Copy( &pShapeDesc->localPose, &tmp);
			}
		}

		pShapeDesc->mass = mass;
		pShapeDesc->density = density;
		pShapeDesc->skinWidth = 0.01f;
		pShapeDesc->name = pNode->GetName();
	}

	PXLOG( "\n");

	return pInfo;
}

void i3Export::_OnExportPhysics( i3tool::exporter::POSTPROCESS_INFO * pInfo)
{
	int PhysicsType, GeometryType;
	i3PhysixShapeSet * pShapeSet;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxShapeDesc * pShapeDesc = NULL;
	REAL32 mass = 0.0f, density = 0.0f, restitution = 0.0f, dfriction = 0.f, sfriction = 0.f;
	REAL32 initialVelocityX = 0.f, initialVelocityY = 0.f, initialVelocityZ = 0.f;
	REAL32 initialSpinX = 0.f, initialSpinY = 0.f, initialSpinZ = 0.f;
	BOOL	bInactive = FALSE;
	char proxyName[256];
	INode * pNode = pInfo->m_pINode;
	i3Node * pi3Node = pInfo->m_pNode;
	Matrix3 matBase;

	actorDesc.setToDefault();
	bodyDesc.setToDefault();
	bodyDesc.flags &= ~NX_BF_VISUALIZATION;

	// User-Defined Properties에서 "PhysicsType" 항목을 확인한다.
	if( pNode->GetUserPropInt( TSTR( "PhysicsType"), PhysicsType) == FALSE)
		return;

	// Geometry type	Ageia 2.7.2 버전입니다.
	GeometryType = 1;
	pNode->GetUserPropInt( TSTR( "GeometryType"), GeometryType);

	//if( GeometryType == 0 )
	{
	//	Log( pNode, "Physics 설정에 필요한 Geometry Type이 설정되지 않았습니다. Shape을 만들 수 없을 수도 있습니다.");
	}

	// Mass
	pNode->GetUserPropFloat( TSTR( "Mass"), mass);

	// Density
	pNode->GetUserPropFloat( TSTR( "Density"), density);

	//Restitution	Ageia 2.7.2 버전입니다.
	if( pNode->GetUserPropFloat( TSTR( "Restitution"), restitution) == FALSE )
	{
		// Elasticity	Ageia 2.6.2 버전입니다.
		REAL32 elasticity = 0.f;
		pNode->GetUserPropFloat( TSTR( "Ellasticity"), elasticity);

		restitution = elasticity;
	}

	// Dynamic Friction
	pNode->GetUserPropFloat( TSTR( "Friction"), dfriction);

	// Static Friction		Ageia 2.7.2 버전입니다
	if( pNode->GetUserPropFloat( TSTR( "StaticFriction"), sfriction) == FALSE )
	{
		//Ageia 2.6.2 버전입니다.
		sfriction = dfriction;
	}

	// Inactive		Ageia 2.6.2 버전입니다. 2.7.2버전에는 없습니다.
	pNode->GetUserPropBool( TSTR( "Inactive"), bInactive);

	// Proxy Geometry
	{
		TSTR temp;

		if( pNode->GetUserPropString( TSTR( "Proxy_Geometry"), temp))
		{
			strcpy( proxyName, temp.data());
		}
		else
		{
			proxyName[0] = 0;
		}
	}

	pShapeSet = i3PhysixShapeSet::new_object();

	if( pNode->GetName() != NULL)
	{
		pShapeSet->SetName( pNode->GetName());
	}

	if( proxyName[0] != 0)
	{
		i3::vector<i3tool::exporter::POSTPROCESS_INFO *> list;
		i3::vector<i3tool::exporter::SHAPEDESC_INFO*> shapeList;
		i3tool::exporter::POSTPROCESS_INFO * pInfo;
		bool bAdd, bBreak = false;

		// Proxy가 있는 경우, 동일 Proxy를 사용하는 모든 Node들을 모아
		// 하나의 Actor로 생성해준다.
		_FindPostProcessByProxy( proxyName, list);

		{
			INode * pProxyNode = FindNodeByName( proxyName);
			
			if( pProxyNode != NULL)
			{
				matBase = __NODE_TM( pProxyNode, m_timeStart);

				// 각 Convex들의 Local Post를 적절히 구하기 위해, Proxy Node의 Inverse World Transform이 필요
				matBase.Invert();
			}
			else
			{
				char conv[256];

				sprintf( conv, "Proxy 이름이 이상합니다.(%s)", proxyName);
				Log( pNode, conv);
				return;
			}
		}

		for(size_t i = 0; (i < list.size()) && ( bBreak == false); i++)
		{
			bAdd = true;

			pInfo = list[i];

			i3tool::exporter::SHAPEDESC_INFO * pShapeInfo = _CreateShapeDesc( pInfo->m_pINode, pShapeSet, &matBase, GeometryType, mass, density);

			pShapeDesc = pShapeInfo->m_pShapeDesc;

			if( pShapeDesc != NULL )
			{
				if( pShapeDesc->getType() == NX_SHAPE_MESH)
				{
					// Triangle-Mesh의 경우에는 i3PhyshxShapeSet에 하나의 Shape 밖에 수용이
					// 되지 않기 때문에...
					if( i == 0)
					{
						bBreak = true;
						// 현재 처리 중인 Shape만을 추가하고 중단.
						// 나머지 Shape들은 다른 i3PhysixShapeSet에 추가되도록 의도..
					}
					else
					{
						// 이전에 Mesh가 아닌 Convex 등이 있었기 때문에, 이 Shape는 다음에
						// 처리되도록 의도...
						bAdd = false;
					}
				}

				if( bAdd)
				{
					shapeList.push_back( pShapeInfo);

					actorDesc.shapes.pushBack( pShapeDesc);
					pInfo->m_bProcessed = true;
				}
				else
				{
				//	delete pShapeInfo->m_pTerrainList;
					i3::delete_by_common_pool(pShapeInfo->m_pTerrainList);
				//
					delete pShapeInfo->m_pShapeDesc;
					delete pShapeInfo;
					pInfo->m_bProcessed = false;
				}
			}
			else
			{
				pInfo->m_bProcessed = false;
				pShapeSet->Release();
				return;
			}
		}

		// 정리된 Shape들을 위한 Terrain Array를 설정해준다.
		if( bBreak == false)
		{
			i3MemoryBuffer * pBuf;
			i3tool::exporter::TTerrainList tList;

			for(size_t i = 0; i < shapeList.size(); i++)
			{
				i3tool::exporter::SHAPEDESC_INFO * pShapeInfo = shapeList[i];

				for( INT32 j = 0; j < (INT32) pShapeInfo->m_pTerrainList->GetCount(); j++)
				{
					tList.Add( pShapeInfo->m_pTerrainList->Get( j));
				}
			}

			pBuf = _BuildTerrainArray( &tList);

			pShapeSet->setTerrainArray( pBuf);
			pShapeSet->setTerrainCount( shapeList.size());
		}
	}
	else
	{
		{
			matBase = __NODE_TM( pNode, m_timeStart);

			// 각 Convex들의 Local Post를 적절히 구하기 위해, Proxy Node의 Inverse World Transform이 필요
			matBase.Invert();
		}

		i3tool::exporter::SHAPEDESC_INFO * pShapeInfo = _CreateShapeDesc( pNode, pShapeSet, &matBase, GeometryType, mass, density);

		pShapeDesc = pShapeInfo->m_pShapeDesc;

		if( pShapeDesc == NULL )
		{
			pInfo->m_bProcessed = false;
			pShapeSet->Release();
			return;
		}

		// Terrain Array의 설정
		{
			i3MemoryBuffer * pBuf;

			pBuf = _BuildTerrainArray( pShapeInfo->m_pTerrainList);

			pShapeSet->setTerrainArray( pBuf);
			pShapeSet->setTerrainCount( pShapeInfo->m_pTerrainList->GetCount());
		}

		actorDesc.shapes.pushBack( pShapeDesc);
	}

	/*
	Mass or density:
	To simulate a dynamic actor, the SDK needs a mass and an inertia tensor. (The inertia tensor is the combination of bodyDesc.massLocalPose and bodyDesc.massSpaceInertia)

	These can be specified in several different ways:

	1) actorDesc.density == 0, bodyDesc.mass > 0, bodyDesc.massSpaceInertia.magnitude() > 0

		Here the mass properties are specified explicitly, there is nothing to compute.

	2) actorDesc.density > 0, actorDesc.shapes.size() > 0, bodyDesc.mass == 0, bodyDesc.massSpaceInertia.magnitude() == 0

		Here a density and the shapes are given. From this both the mass and the inertia tensor is computed.

	3) actorDesc.density == 0, actorDesc.shapes.size() > 0, bodyDesc.mass > 0, bodyDesc.massSpaceInertia.magnitude() == 0

		Here a mass and shapes are given. From this the inertia tensor is computed.

	Other combinations of settings are illegal.
	*/

	if( mass == 0.0f)
	{
		// Mass가 없는 경우...
		//	
		//	Density, 와 Shape Array가 반드시 Valid해야 함.
		if( density == 0.0f)
			density = 1.0f;
	}
	else
	{
		if( density != 0.0f)
			density = 0.0f;

		bodyDesc.mass = mass;
	}

	switch( PhysicsType)
	{
		case 1:	//	dynamic
			actorDesc.body = &bodyDesc;
			break;

		case 2:	//	kinematic
			actorDesc.body = &bodyDesc;
			bodyDesc.flags |= NX_BF_KINEMATIC;
			break;

		case 3:	//	static
			actorDesc.body = NULL;
			break;

		case 4:	//	non-physical
		case 5:	//	undefined
			// 정의되지 않은 것과 동일한 것으로 간주하여 Phycix 설정을 하지 않음.
			pShapeSet->Release();
			return;

		case 6:	//	cloth
			break;
	}

	actorDesc.userData = pShapeSet;
	actorDesc.density = density;
	actorDesc.name = pNode->GetName();

	// Global Pose
	{
		Matrix3 mtx = pNode->GetNodeTM( m_timeStart);
		MATRIX i3Mtx;

		if( m_Option.m_bRotateAxis)
			_i3Space( &mtx);

		::Convert( &i3Mtx, &mtx);
		
		NXU::Copy( &actorDesc.globalPose, &i3Mtx);
	}

	pShapeSet->setRestitution( restitution);
	pShapeSet->setFriction( sfriction, dfriction);

	pShapeSet->Create( &actorDesc, true );

	// Convex의 경우, Node의 이름으로 Terrain을 설정해 주어야한다.

	// 생성되었던 ShapeDesc들을 제거
	for(size_t i = 0; i < actorDesc.shapes.size(); i++)
	{
		NxShapeDesc * pShapeDesc = actorDesc.shapes[i];

		if( pShapeDesc->getType() == NX_SHAPE_MESH)
		{
			// Triangle-Mesh의 경우에는 이미 완전히 설정된 Terrain Array를 Baking하는 단계에서
			// 
		}
		else
		{
		}

		delete actorDesc.shapes[i];
	}

	// 최종적으로 i3PhysixShapeSet을 Attach 시킬 위치를 찾는다.
	{
		if( proxyName[0] != 0)
		{
			// Proxy Geometry가 존재한다.
			i3Node * pAttachNode;

			// 실제로 Physix가 설정되어야 할 Bone을 검사한다.
			pAttachNode = _FindAttachBoneByName( proxyName);
			if( pAttachNode == NULL)
			{
				Log( pNode, "Physics 설정에 필요한 Node가 Hide 상태에 있거나, 존재하지 않습니다.");
				return;
			}
			else
			{
				if( i3::kind_of<i3Body*>(pAttachNode)) //->IsTypeOf( i3Body::static_meta()))
				{
					// Skeleton에 Attach되는 경우.
					i3Body * pBody = (i3Body *) pAttachNode;
					i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();

					// 해당 Bone 및 Parent Bone을 찾는다.
					INT32 boneIdx = pBody->FindBoneByName( proxyName);
					if( boneIdx != -1)
					{
						// 현재의 i3PhysixShapeSet을 등록해준다.
						pSkel->setShapeSet( boneIdx, pShapeSet);
					}
					else
					{
						Log( pNode, "Physics 설정에 필요한 Node가 Hide 상태에 있거나, 존재하지 않습니다.");
						I3_SAFE_RELEASE( pShapeSet);
						return;
					}

					pInfo->m_Index = boneIdx;
					pInfo->m_pNode = pAttachNode;
				}
				else
				{
					// 일반적인 Node에 Attach되는 경우.
					// 이 경우, 반드시 i3Transform2 Node이어야 한다.
					i3Node * pNode;

					i3::string sNodeName;
					pNode = _ReplaceToPhysixNode( pAttachNode, pShapeSet, PhysicsType, sNodeName);
					I3ASSERT( pNode != NULL);

					pInfo->m_pNode = pNode;
				}
			}
		}
		else
		{
			if( pi3Node != NULL)
			{
				i3::string sNodeName;
				i3Node * pNode = _ReplaceToPhysixNode( pi3Node, pShapeSet, PhysicsType, sNodeName);

				pInfo->m_pNode = pNode;
			}
			else
			{
				I3TRACE( "sdfsdfsf\n");
			}
		}

		// 이 후, Joint Process를 위해 넣어둔다.
		pInfo->m_pShapeSet = pShapeSet;
	}

	PXLOG( "\n");

	I3_SAFE_RELEASE( pShapeSet);

	return;
}

i3Node * i3Export::_ReplaceToPhysixNode( i3Node * pAttachNode, i3PhysixShapeSet * pShapeSet, int PhysixType, i3::string& sNodeName)
{
	char conv[256];
	i3Transform2 * pTrans2;

	if( i3::kind_of<i3Transform2*>(pAttachNode)) //->IsTypeOf( i3Transform2::static_meta()))
	{
		if( PhysixType == 2)		// kinematic
		{
			i3ShapeSetContainer * pContainer = i3ShapeSetContainer::new_object_ref();

			pContainer->SetName( "Kinematic");
			pContainer->setShapeSet( pShapeSet);

			pAttachNode->AddChild( pContainer);

			return pContainer;
		}
		else
		{
			pTrans2 = (i3Transform2 *) pAttachNode;

			if( pTrans2->getShapeSet() != NULL)
			{
				// 기존의 것이 있다면 기존의 것을 i3ShapeSetContainer를 이용해 보관
				i3ShapeSetContainer * pContainer = i3ShapeSetContainer::new_object_ref();
				
				pContainer->SetName( "Backup");

				pContainer->setShapeSet( pTrans2->getShapeSet());

				pTrans2->AddChild( pContainer);
			}

			pTrans2->setShapeSet( pShapeSet);

			return (i3Transform2 *) pAttachNode;
		}
	}
	else 
	{
		i3Node * pParent = pAttachNode->GetParent();

		pAttachNode->AddRef();

		pTrans2 = i3Transform2::new_object_ref();

		i3Scene::MoveChildren( pTrans2, pAttachNode);
		pTrans2->SetName( pAttachNode->GetName());

		// 이후의 검색에서 새로 생성된 i3Transform2 Node가 대신 역할을 하도록
		// 하기 위해 이름을 교체한다.
		{
			sprintf( conv, "%s_RefNode", pTrans2->GetName());
			pAttachNode->SetName( conv);
		}

		if( i3::kind_of<i3Transform*>(pAttachNode)) //->IsTypeOf( i3Transform::static_meta()))
		{
			// i3Transform Node는 i3Transform2 Node가 완전히 기능적으로 대신할 수 있으므로
			// 교체한다.
			i3Transform * pTrans = (i3Transform *) pAttachNode;
			i3Node * pParent = pTrans->GetParent();

			pTrans2->SetMatrix( pTrans->GetMatrix());

			if( pTrans->IsAnimated())
			{
				pTrans2->SetAnimation( pTrans->getAnimation(0), pTrans->getAnimationBoneIndex(0));
			}

			if( pParent == NULL)
			{
				// Root
				I3_REF_CHANGE( m_pRoot, pTrans2);
			}
			else
			{
				pParent->RemoveChild( pAttachNode);
				pParent->AddChild( pTrans2);
			}
		}
		else
		{
			pAttachNode->AddChild( pTrans2);

		}

		pAttachNode->Release();

		if( PhysixType == 2)
		{
			i3ShapeSetContainer * pContainer = i3ShapeSetContainer::new_object_ref();

			sprintf( conv, "Kinematic_%s", pShapeSet->GetName());
			pContainer->SetName( conv);
			pContainer->setShapeSet( pShapeSet);

			pTrans2->AddChild( pContainer);

			sNodeName = conv;
		}
		else
		{
			pTrans2->setShapeSet( pShapeSet);
		}

	}

	return pTrans2;
}

void i3Export::_OnExportJoint( i3tool::exporter::POSTPROCESS_INFO * pInfo)
{
	// Joint 관련 설정
	TimeValue time = m_timeStart;
	INT32 jointType = 1;
	TSTR conv;
	VEC3D axis, norm;
	REAL32	twistLow, twistHigh, swing1, swing2;
	BOOL	bLimits;
	i3PhysixJointInfoD6 * pJointInfo;
	NxD6JointMotion		motion;
	INode * pNode = pInfo->m_pINode;
	MATRIX * pLocalMtx = &pInfo->m_LocalTransform;
	i3PhysixShapeSet * pShapeSet = pInfo->m_pShapeSet;
	i3PhysixShapeSet * pParentShapeSet = NULL;

	if( pInfo->m_pNode == NULL)
		return;

	if( pInfo->m_pShapeSet == NULL)
		return;

	// 우선 Joint가 필요한가?
	pNode->GetUserPropInt( TSTR( "pmljointtype"), jointType);

	// Axis
	pNode->GetUserPropString( TSTR( "pmljointaxis"), conv);
	_str2vec( conv.data(), &axis);
	
	// Normal
	pNode->GetUserPropString( TSTR( "pmljointnrml"), conv);
	_str2vec( conv.data(), &norm);

	// Limits (bool)
	pNode->GetUserPropInt( TSTR( "pmljointlimits"), bLimits);

	// Twist Low
	pNode->GetUserPropFloat( TSTR( "pmljointtwistlow"), twistLow);
	twistLow = I3_DEG2RAD( twistLow);

	// Twist High
	pNode->GetUserPropFloat( TSTR( "pmljointtwisthigh"), twistHigh);
	twistHigh = I3_DEG2RAD( twistHigh);

	// Swing 1
	pNode->GetUserPropFloat( TSTR( "pmljointswing1"), swing1);
	swing1 = I3_DEG2RAD( swing1);

	// Swing 2
	pNode->GetUserPropFloat( TSTR( "pmljointswing2"), swing2);
	swing2 = I3_DEG2RAD( swing2);

	pJointInfo = i3PhysixJointInfoD6::new_object();

	{
		NxD6JointDesc desc;

		pJointInfo->SetFrom( &desc);

		pJointInfo->projectionDistance = 0.0f;
		pJointInfo->projectionMode = NX_JPM_LINEAR_MINDIST;
	}

	pJointInfo->projectionMode = NX_JPM_NONE;

	if( bLimits)
		motion = NX_D6JOINT_MOTION_LIMITED;
	else
		motion = NX_D6JOINT_MOTION_FREE;

	switch( jointType)
	{
		case 2 :		// Locked Joint (fixed to parent)
			{
				pJointInfo->twistMotion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->swing1Motion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->swing2Motion = NX_D6JOINT_MOTION_LOCKED;

				pJointInfo->xMotion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->yMotion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->zMotion = NX_D6JOINT_MOTION_LOCKED;

				PXLOG( "Locked-Joint ");
			}
			break;

		case 3 :		// Revolute Joint (Hinge Joint)
			{
				pJointInfo->twistMotion = motion;
				pJointInfo->swing1Motion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->swing2Motion = NX_D6JOINT_MOTION_LOCKED;

				pJointInfo->xMotion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->yMotion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->zMotion = NX_D6JOINT_MOTION_LOCKED;

				pJointInfo->twistLimit.low.value = twistLow;
				pJointInfo->twistLimit.high.value = twistHigh;

				PXLOG( "Hinge-Joint ");
			}
			break;

		case 4 :		// Spherical Joint (3-DOF)
			{
				pJointInfo->twistMotion = motion;
				pJointInfo->swing1Motion = motion;
				pJointInfo->swing2Motion = motion;

				pJointInfo->xMotion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->yMotion = NX_D6JOINT_MOTION_LOCKED;
				pJointInfo->zMotion = NX_D6JOINT_MOTION_LOCKED;

				pJointInfo->twistLimit.low.value = twistLow;
				pJointInfo->twistLimit.high.value = twistHigh;
				pJointInfo->swing1Limit.value = swing1;
				pJointInfo->swing2Limit.value = swing2;

				PXLOG( "Spherical-Joint ");
			}
			break;

		default :
			pJointInfo->Release();
			PXLOG( "\n");
			return;		// Joint가 필요하지 않다.
	}

	// Parent Actor
	if( i3::kind_of<i3Body*>(pInfo->m_pNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		i3Body * pBody = (i3Body *) pInfo->m_pNode;
		i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
		INT32 idxParent;

		idxParent = pSkel->getParentBoneIndex( pInfo->m_Index);

		if( idxParent != -1)
		{
			pParentShapeSet = pSkel->getShapeSet( idxParent);
		}
	}
	else
	{
		i3Node * pParent = pInfo->m_pNode->GetParent();

		if( i3::kind_of<i3Transform2*>(pParent)) //->IsTypeOf( i3Transform2::static_meta()))
		{
			i3Transform2 * pParentTrans = (i3Transform2 *) pParent;

			pParentShapeSet = pParentTrans->getShapeSet();
		}
		else if( i3::kind_of<i3ShapeSetContainer*>(pParent)) //->IsTypeOf( i3ShapeSetContainer::static_meta()))
		{
			i3ShapeSetContainer * pContainer = (i3ShapeSetContainer *) pParent;

			pParentShapeSet = pContainer->getShapeSet();
		}
	}

	// Joint Descriptor 공통 설정
	{
		{
			Matrix3 tmpMtx, inv;
			Point3 tmp, worldAxis, worldNorm, localAxis, localNorm;

			// Anchors
			VEC3D * pPos = i3Matrix::GetPos( pLocalMtx);
			pJointInfo->setLocalAnchor( 0, pPos);
			pJointInfo->setLocalAnchor( 1, &I3_ZEROVECTOR);

			// Frame 0
			pJointInfo->setLocalAxis( 0, &axis);;
			pJointInfo->setLocalNormal( 0, &norm);;

			// Frame 1
			localAxis.Set( axis.x, axis.y, axis.z);
			localNorm.Set( norm.x, norm.y, norm.z);

			{
				Matrix3 parentTM = pNode->GetParentTM( time);
				Matrix3 childTM = pNode->GetNodeTM( time);

				// World Axis -> Child-Space Axis
				parentTM.TransformVectors( &localAxis, &worldAxis, 1);
				parentTM.TransformVectors( &localNorm, &worldNorm, 1);

				{
					childTM.Invert();
					
					childTM.TransformVectors( &worldAxis, &tmp, 1);
					tmp.Normalize();
					pJointInfo->setLocalAxis( 1, tmp.x, tmp.y, tmp.z);

					childTM.TransformVectors( &worldNorm, &tmp, 1);
					tmp.Normalize();
					pJointInfo->setLocalNormal( 1, tmp.x, tmp.y, tmp.z);
				}
			}
		}

		//pJointInfo->addJointFlags( NX_JF_COLLISION_ENABLED);
	}

	pShapeSet->setJointInfo( pJointInfo);
	pShapeSet->CreateJoint( pParentShapeSet);

	pJointInfo->Release();
}

bool i3Export::CanBeConvex( INode * pNode, INT32* errVal)
{
	i3Vector3Array * pVA;

	i3tool::exporter::TFaceList faceList;
	NxShapeDesc * pShapeDesc = NULL;
	i3tool::exporter::TTerrainList *	pTerrainList;
	NxConvexShapeDesc * pDesc;
	Matrix3 mtx;

	pVA = i3Vector3Array::new_object();
//	pIA = List::new_object();
	i3::vector<INT32> IA;

	pTerrainList =	i3::create_by_common_pool(i3::identity<i3tool::exporter::TTerrainList>());	// (i3tool::exporter::TTerrainList *) NEW i3tool::exporter::TTerrainList;

	mtx.IdentityMatrix();

	if(!_ExtractGeom( pNode, pVA, IA, pTerrainList, &faceList, &mtx))
	{
		if(m_physExportLogFlag & EPHYSEXPLOG_CANBECONVEX_SKIP)
		{
			if(!stricmp(pNode->GetName(), "Scene Root"))
			{
				if(errVal)
					*errVal = 1; // Scene Root 한정
			}
			else
			{
				LogEx(pNode, "[CanbeConvex] ERR! - [%s] is not a triangl mesh..", pNode->GetName());

				if(errVal)
					*errVal = 101;
			}

			return false;
		}
	}

	// Convex 생성 시도.
	pDesc = _BakeConvexMesh( pNode, pVA, IA, &mtx);

	I3_SAFE_RELEASE( pVA);
//	I3_SAFE_RELEASE( pIA);
	IA.clear();
	
	i3::delete_by_common_pool(pTerrainList);
	// delete pTerrainList;

	if( pDesc != NULL)
	{
		NxConvexMeshDesc meshDesc;
		NXU::MemoryWriteBuffer wb;
		NxConvexMesh * pMesh = pDesc->meshData;

		pMesh->saveToDesc( meshDesc);

		NXU::InitCooking();
		bool ok = NXU::CookConvexMesh( meshDesc, wb);
		NXU::CloseCooking();

		m_pPhysixCtx->getSDK()->releaseConvexMesh( * pDesc->meshData);

		delete pDesc;

		bool rst = true && ok;

		if(m_physExportLogFlag & EPHYSEXPLOG_CANBECONVEX_OK)
		{
			if(rst)
				LogEx(pNode, "[CanbeConvex] OK! - [%s] is succeed to baking convex mesh..", pNode->GetName());
		}

		if(rst)
			return true;
	}

	if(m_physExportLogFlag & EPHYSEXPLOG_CANBECONVEX_FAIL)
	{
		LogEx(pNode, "[CanbeConvex] ERR! - [%s] is failed to bake convex mesh..", pNode->GetName());

		if(errVal)
			*errVal = 110;
	}

	return false;
}

INT32 i3Export::_Rec_CheckCollsionMesh( INode * pNode)
{
	INT32 i, PhysicsType;
	INT32 errCnt = 0;

	if( pNode->GetUserPropInt( TSTR( "PhysicsType"), PhysicsType))
	{
		if( (PhysicsType == 1) || ((PhysicsType == 2) || (PhysicsType == 3)))
		{
			INT32 errVal = 0;
			if( CanBeConvex( pNode, &errVal) == false)
			{
				if(m_physExportLogFlag & EPHYSEXPLOG_CANBECONVEX_FAIL)
					Log( pNode, "Convex 생성이 불가합니다.");

				if(errVal > 100) // 100이하의 Err는 허용가능
					errCnt++;
			}
		}
	}
	
	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		errCnt += _Rec_CheckCollsionMesh( pNode->GetChildNode(i));
	}

	return errCnt;
}

bool i3Export::CheckCollisionMeshes(bool doClearLog)
{
	Interface * ip = GetCOREInterface();

	if(doClearLog)
		ClearLog();

	return _Rec_CheckCollsionMesh( ip->GetRootNode()) == 0 ? true : false;
}
