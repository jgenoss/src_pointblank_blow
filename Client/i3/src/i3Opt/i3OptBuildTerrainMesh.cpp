#include "i3OptPCH.h"
#include "i3OptBuildTerrainMesh.h"
#include "i3OptCollapseTransform.h"
#include "i3OptBuildCollisionMesh.h"

#include "NXU_helper.h"
#include "NXU_schema.h"
#include "NXU_cooking.h"
#include "NXU_streaming.h"


I3_CLASS_INSTANCE( i3OptBuildTerrainMesh);

i3OptBuildTerrainMesh::i3OptBuildTerrainMesh(void)
{
	m_Class = CLASS_PHYSICS;
	m_Style &= ~OPT_STYLE_INTERNAL_ONLY;
}

i3OptBuildTerrainMesh::~i3OptBuildTerrainMesh(void)
{
	I3MEM_SAFE_FREE( m_pTriArray);
}

NxTriangleMeshShapeDesc * i3OptBuildTerrainMesh::_ConvertToNxTerrain( BTM_OPTINFO * pInfo)
{
	NxTriangleMeshDesc desc;
	NxTriangleMesh * pNxMesh;
	NxTriangleMeshShapeDesc * pShapeDesc;
	i3CollideeMesh * pMesh = pInfo->_pMesh;

	desc.numVertices = pInfo->_posCount;
	desc.points = pMesh->GetPositionArray()->GetVectorArray();
	desc.pointStrideBytes = sizeof(VEC3D);

	desc.numTriangles = pInfo->_triCount;
	desc.triangles = pMesh->GetTriangleArray();
	desc.triangleStrideBytes = sizeof(I3_COLLIDEE_TRI32);

	desc.materialIndexStride = sizeof(I3_PHYSIX_TRI);
	desc.materialIndices = &(m_pTriArray->m_MaterialIndex);
	
	//desc.flags = NX_MF_16_BIT_INDICES;

	NXU::MemoryWriteBuffer buf;

	NXU::InitCooking();
	if( NXU::CookTriangleMesh( desc, buf) == false)
		return nullptr;

	pNxMesh = g_pPhysixCtx->getSDK()->createTriangleMesh( NXU::MemoryReadBuffer(buf.data));
	I3ASSERT( pNxMesh != nullptr);

	// Shape Creation
	{
		pShapeDesc = new NxTriangleMeshShapeDesc;

		pShapeDesc->meshData = pNxMesh;
		pShapeDesc->meshFlags = 0;
		pShapeDesc->mass = m_Mass;
		pShapeDesc->skinWidth = 0.02f;

	}

	NXU::CloseCooking();
	NXU::releasePersistentMemory();

	return pShapeDesc;
}

static I3SG_TRAVERSAL_RESULT _GetTotalTriCountProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 * pCount = (INT32 *) pUserData;
	INT32 i;

	if( i3::kind_of<i3ColliderSet*>(pNode))
	{
		i3ColliderSet * pCollSet = (i3ColliderSet *) pNode;
		i3Collider * pCollider;

		for( i = 0; i < pCollSet->GetColliderCount(); i++)
		{
			pCollider = pCollSet->GetCollider( i);

			if( i3::kind_of<i3CollideeMesh*>(pCollider->GetCollidee()))
			{
				i3CollideeMesh * pMesh = (i3CollideeMesh *) pCollider->GetCollidee();

				*pCount += pMesh->GetTriangleCount();
			}
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

static void _MergeMesh( BTM_OPTINFO * pInfo, i3CollideeMesh * pSrcMesh)
{
	INT32 i, j;
	I3_COLLIDEE_TRI32 * pTri;
	I3_COLLIDEE_TRI32 destTri;
	i3Vector3Array * pDestPos, * pSrcPos;
	i3Vector3Array * pDestNormal, * pSrcNormal;
	i3CollideeMesh * pDestMesh = pInfo->_pMesh;
	VEC3D * pSrcVec;

	pDestPos = pDestMesh->GetPositionArray();
	pDestNormal = pDestMesh->GetNormalArray();

	pSrcPos = pSrcMesh->GetPositionArray();
	pSrcNormal = pSrcMesh->GetNormalArray();

	for( i = 0; i < (INT32) pSrcMesh->GetTriangleCount(); i++)
	{
		pTri = pSrcMesh->GetTriangle( i);

		memcpy( &destTri, pTri, sizeof(destTri));

		// Position을 Dest Vector Array에 추가한다.
		for( j = 0; j < 3; j++)
		{
			pSrcVec = pSrcPos->GetVector( pTri->m_Index[j]);

			destTri.m_Index[j] = pDestPos->Find( pSrcVec, pInfo->_posCount);
			if( destTri.m_Index[j] == 0xFFFFFFFF)
			{
				pDestPos->SetVector( pInfo->_posCount, pSrcVec);
				destTri.m_Index[j] = pInfo->_posCount;
				pInfo->_posCount++;

				I3ASSERT( pDestPos->GetVectorCount() >= (UINT32) pInfo->_posCount);
			}
		}

		// Normal 추가
		{
			pSrcVec = pSrcNormal->GetVector( pTri->m_NormalIndex);

			destTri.m_NormalIndex = pDestNormal->Find( pSrcVec, pInfo->_normCount);
			if( destTri.m_NormalIndex == 0xFFFFFFFF)
			{
				pDestNormal->SetVector( pInfo->_normCount, pSrcVec);
				destTri.m_NormalIndex = pInfo->_normCount;
				pInfo->_normCount++;

				I3ASSERT( pDestNormal->GetVectorCount() >= (UINT32) pInfo->_normCount);
			}
		}
		

		pDestMesh->SetTriangle( pInfo->_triCount, &destTri);
		pInfo->_triCount++;

		I3ASSERT( pDestMesh->GetTriangleCount() >= (UINT32) pInfo->_triCount);
	}
}

static I3SG_TRAVERSAL_RESULT _MergeTriProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	BTM_OPTINFO * pInfo = (BTM_OPTINFO *) pUserData;
	INT32 i;

	if( i3::kind_of<i3ColliderSet*>(pNode))
	{
		i3ColliderSet * pCollSet = (i3ColliderSet *) pNode;
		i3Collider * pCollider;

		for( i = 0; i < pCollSet->GetColliderCount(); i++)
		{
			pCollider = pCollSet->GetCollider( i);

			if( i3::kind_of<i3CollideeMesh*>(pCollider->GetCollidee()))
			{
				i3CollideeMesh * pSrcMesh = (i3CollideeMesh *) pCollider->GetCollidee();

				_MergeMesh( pInfo, pSrcMesh);
			}
		}
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void	i3OptBuildTerrainMesh::_RemoveAllMaterials(void)
{
	g_pPhysixCtx->RemoveAllMaterials();
	m_MaterialList.clear();

	// Default Material은 제거되지 않는다.
}

INT32	i3OptBuildTerrainMesh::_FindMaterial( INT32 terrain, INT32 reverb, COLOR * pColor, INT32 thick, INT32 PortalID)
{
	
	I3_PHYSIX_TERRAIN * pMaterial;
	//I3_COLLIDEE_TRI * pTri;

	// Default Material은 제외시켜야 한다.
	for(size_t i = 0; i < m_MaterialList.size(); i++)
	{
		pMaterial = m_MaterialList[i];

		if( (pMaterial->m_Terrain == terrain) && (pMaterial->m_Reverb == reverb) && 
			i3Color::IsSame( (COLOR *) pMaterial->m_Color, pColor) &&
			(pMaterial->m_Thickness == thick) && 
			(pMaterial->m_PortalID == PortalID))
		{
			return i;
		}
	}

	return -1;
}

INT32	i3OptBuildTerrainMesh::_AddMaterial( INT32 terrain, INT32 reverb, COLOR * pColor, INT32 thick, INT32 PortalID)
{
	INT32 idx;

	// 우선 동일한 Material이 이미 존재하는지 검사한다.
	idx = _FindMaterial( terrain, reverb, pColor, thick, PortalID);
	if( idx != -1)
		return idx;

	// 없기 때문에 새로운 Material로 등록한다.
	{
		// 사용자 정의 Data
		I3_PHYSIX_TERRAIN * pInfo;

		pInfo = (I3_PHYSIX_TERRAIN *) i3MemAlloc( sizeof(I3_PHYSIX_TERRAIN));
		I3ASSERT( pInfo != nullptr);

		pInfo->m_Terrain = (UINT16) terrain;
		pInfo->m_Reverb = (UINT16) reverb;
		pInfo->m_Color[0] = i3Color::GetR( pColor);
		pInfo->m_Color[1] = i3Color::GetG( pColor);
		pInfo->m_Color[2] = i3Color::GetB( pColor);
		pInfo->m_Color[3] = i3Color::GetA( pColor);
		pInfo->m_Thickness = thick;
		pInfo->m_PortalID = PortalID;
		
		idx = INT32(m_MaterialList.size());
		m_MaterialList.push_back( pInfo);
	}

	return idx;
}

void	i3OptBuildTerrainMesh::_MakeMaterials( i3CollideeMesh * pMesh)
{
	UINT32 i;

	_RemoveAllMaterials();

	m_pTriArray = (I3_PHYSIX_TRI *) i3MemAlloc( sizeof(I3_PHYSIX_TRI) * pMesh->GetTriangleCount());
	I3ASSERT( m_pTriArray != nullptr);

	m_MaterialList.clear();

	VEC3D mid;

	for( i = 0; i < pMesh->GetTriangleCount(); i++)
	{
		I3_COLLIDEE_TRI32 * pTri = pMesh->GetTriangle( i);

		m_pTriArray[i].m_Index[0]	= pTri->m_Index[0];
		m_pTriArray[i].m_Index[1]	= pTri->m_Index[1];
		m_pTriArray[i].m_Index[2]	= pTri->m_Index[2];
		m_pTriArray[i].m_MaterialIndex = (UINT16) _AddMaterial( pTri->m_Terrain, pTri->m_Reverb, (COLOR *) pTri->m_Color, 0, 0);


		VEC3D * pPos = pMesh->GetPosition( pTri->m_Index[0]);
		i3Vector::Add( &mid, &mid, pPos);

		pPos = pMesh->GetPosition( pTri->m_Index[1]);
		i3Vector::Add( &mid, &mid, pPos);

		pPos = pMesh->GetPosition( pTri->m_Index[2]);
		i3Vector::Add( &mid, &mid, pPos);

		i3Vector::Scale( &mid, &mid, 1.0f / 3.0f);

		I3ASSERT( m_pTriArray[i].m_MaterialIndex < (m_MaterialList.size()) );
	}	
}

void i3OptBuildTerrainMesh::Trace( i3Node * pRoot)
{
	INT32 triCount;
	i3CollideeMesh * pCollMesh;
	BTM_OPTINFO info;

	if( m_Phase == PHASE_COMMON_SG)
		return;

	_RemoveAllMaterials();

	// Transform Node를 제거한다.
	{
		CallOptimizer( i3OptCollapseTransform::static_meta());
	}

	// i3CollideeMesh로 변환한다.
	{
		CallOptimizer( i3OptBuildCollisionMesh::static_meta());

		#if defined( I3_DEBUG)
		I3_SG_DUMP( m_pSg->getInstanceSg());
		#endif
	}

	// 존재하는 모든 Geometry 및 CollideeMesh Node를 검색하여, 하나의 Mesh로 합친다.
	{
		triCount = 0;

		// 전체 Sg에 있는 Triangle 개수를 구한다.
		i3Scene::Traverse( m_pSg->getInstanceSg(), _GetTotalTriCountProc, &triCount);

		if( triCount <= 0)
			return;

		pCollMesh = i3CollideeMesh::new_object();

		pCollMesh->SetPositionCount( triCount * 3);
		pCollMesh->SetNormalCount( triCount);
		pCollMesh->SetTriangleCount( triCount);

		info._posCount = 0;
		info._normCount = 0;
		info._triCount = 0;
		info._pMesh = pCollMesh;

		// Triangle Merge
		i3Scene::Traverse( m_pSg->getInstanceSg(), _MergeTriProc, &info);
		I3TRACE( "Total %d Position, %d Normal, %d Triangles\n", info._posCount, info._normCount, info._triCount);
	}

	// Material을 생성한다.
	{
		_MakeMaterials( info._pMesh);
	}

	// i3PhysixShapeSet의 생성
	{
		i3PhysixShapeSet * pShapeSet = i3PhysixShapeSet::new_object_ref();

		pShapeSet->setShapeType( I3_PHYSIX_SHAPE_TRIMESH);

		// Terrains
		{
			i3MemoryBuffer * pBuff;
			I3_PHYSIX_TERRAIN * pDest;

			pBuff = i3MemoryBuffer::new_object_ref();

			pBuff->Create( sizeof(I3_PHYSIX_TERRAIN) * m_MaterialList.size());
			pDest = (I3_PHYSIX_TERRAIN *) pBuff->getBuffer();

			for(size_t i = 0; i < m_MaterialList.size(); i++)
			{
				memcpy( & pDest[i], m_MaterialList[i], sizeof(I3_PHYSIX_TERRAIN));
			}

			pShapeSet->setTerrainArray( pBuff);
			pShapeSet->setTerrainCount( m_MaterialList.size());
		}

		// Face Index Array
		{
			i3MemoryBuffer * pBuff;

			pBuff = i3MemoryBuffer::new_object_ref();

			pBuff->Create( sizeof(I3_PHYSIX_TRI) * info._triCount);

			memcpy( pBuff->getBuffer(), m_pTriArray, pBuff->GetSize());

			pShapeSet->setFaceArray( pBuff);
			pShapeSet->setFaceCount( info._triCount);
		}

		// Position Array
		{
			i3Vector3Array * pNewVA = i3Vector3Array::new_object_ref();
			i3Vector3Array * pOldVA = info._pMesh->GetPositionArray();

			pNewVA->SetVectorCount( pOldVA->GetVectorCount());

			for(size_t i = 0; i < pOldVA->GetVectorCount(); i++)
			{
				pNewVA->SetVector( i, pOldVA->GetVector( i));
			}

			pShapeSet->setPositionArray( pNewVA);
		}

		{
			NxTriangleMeshShapeDesc * pDesc;
			NxActorDesc actorDesc;
			NxBodyDesc body;

			pDesc = _ConvertToNxTerrain( &info);

			actorDesc.setToDefault();
			body.setToDefault();
			body.mass = m_Mass;

			if( m_bDynamic)
				actorDesc.body = &body;
			else
				actorDesc.body = nullptr;

			actorDesc.density = m_Density;
			actorDesc.shapes.pushBack( pDesc);

			pShapeSet->Create( &actorDesc );
		}

		// 모든 Node를 제거하고 i3PhysixShapeSet Node만 추가한다.

		{
			i3Transform2 * pTrans = i3Transform2::new_object_ref();

			pTrans->setShapeSet( pShapeSet);

			//getTraceRoot()->AddChild( pTrans);
			m_pSg->setInstanceSg( pTrans);
		}
	}

	I3_MUST_RELEASE( info._pMesh);
	I3MEM_SAFE_FREE( m_pTriArray);
}
