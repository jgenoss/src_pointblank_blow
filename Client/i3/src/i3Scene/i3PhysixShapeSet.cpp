#include "i3SceneDef.h"
#include "i3PhysixShapeSet.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"
#include "i3PhysixContext.h"

#include "NXU_helper.h"
#include "NXU_schema.h"
#include "NXU_Streaming.h"
#include "NXU_cooking.h"

#include "i3Transform2.h"

//#define I3_DUMP_SHAPESET
//#define		_DBG_NOCREATE_

//#define USE_MEMBUFF_COLLECTION

//#define OWN_PAIR_FILTER

I3_CLASS_INSTANCE( i3PhysixShapeSet);

i3PhysixShapeSet::~i3PhysixShapeSet(void)
{
	if( m_pNxJoint != nullptr && g_pPhysixCtx != nullptr )
	{
		m_pNxJoint->userData = nullptr;
		g_pPhysixCtx->CMD_releaseJoint( m_pNxJoint );
		m_pNxJoint = nullptr;
	}

	/*
	if( m_pNxActor != nullptr && g_pPhysixCtx != nullptr)
	{
		m_pNxActor->userData = nullptr;
		g_pPhysixCtx->CMD_releaseActor( m_pNxActor );

		m_pNxActor = nullptr;
	}
	*/
	DestroyActor();

	I3_SAFE_RELEASE( m_pReleaseReceiver);

	I3_SAFE_RELEASE( m_pTerrainArray);
	I3_SAFE_RELEASE( m_pFaceArray);
	I3_SAFE_RELEASE( m_pPosArray);
	I3_SAFE_RELEASE( m_pJointInfo);

	m_pReceiver	= nullptr;
	
	I3_SAFE_RELEASE( m_pNxData);

	if( m_pCollectionData != nullptr)
	{
		NXU::NxuPhysicsCollection * pColl = (NXU::NxuPhysicsCollection *) m_pCollectionData->getBuffer();

		INT32 nInstanceCount = NXU::getSceneInstanceCount((NXU::NxuPhysicsCollection&)(*pColl));
		INT32 nRefCount = m_pCollectionData->GetRefCount();

		I3TRACE( "Physix InstanceCount : %d - RefCount : %d\n", nInstanceCount, nRefCount);

		if( nRefCount == 1)
		{
			NXU::releaseCollection( pColl);
			//NXU::releasePersistentMemory();
		}
		//else
		{
			//if( nInstanceCount == nRefCount -1)
			{
				//NXU::closeCurrentSceneInstance( (NXU::NxuPhysicsCollection&)(*pColl) );
			}
		}

		I3_SAFE_RELEASE( m_pCollectionData);
	}

//	I3MEM_SAFE_FREE( m_pReports);
}

void i3PhysixShapeSet::PrepareContactReport(void)
{
	m_bEnableContactReport = true;
}

void i3PhysixShapeSet::DestroyActor( void )
{
	if( m_pNxActor != nullptr && g_pPhysixCtx != nullptr)
	{
		UINT32 i;
		NxShape * const * ppShapes = m_pNxActor->getShapes();

		for( i = 0; i < m_pNxActor->getNbShapes(); i++)
		{
			if( m_callbackRelease != nullptr)
				m_callbackRelease( m_pReleaseReceiver, ppShapes[i]);

			ppShapes[i]->userData = nullptr;
		}

		m_pNxActor->userData = nullptr;
		g_pPhysixCtx->CMD_releaseActor( m_pNxActor );

		m_pNxActor = nullptr;
	}
}

void i3PhysixShapeSet::DestroyJoint( void )
{
	if( m_pNxJoint != nullptr && g_pPhysixCtx != nullptr )
	{
		m_pNxJoint->userData = nullptr;
		g_pPhysixCtx->CMD_releaseJoint( m_pNxJoint );
		m_pNxJoint = nullptr;
	}
}

void i3PhysixShapeSet::Create( NxActorDesc * pDesc, bool bUseTool )
{
#if !defined( _DBG_NOCREATE_)
	UINT32 i;

	NxBodyDesc bodyDesc;
	NxActorDesc desc;

	if( pDesc == nullptr)
	{
		desc.setToDefault();
		desc.body = &bodyDesc;
		pDesc = &desc;
	}
	else if( !bUseTool )
	{
		NxShapeDesc * pShapeDesc = pDesc->shapes[0];
		NxMaterial * pMtl = g_pPhysixCtx->getMaterial( pShapeDesc->materialIndex );
		if( pMtl != nullptr)
		{
			m_Restitution = pMtl->getRestitution();
			m_StaticFriction = pMtl->getStaticFriction();
			m_DynamicFriction = pMtl->getDynamicFriction();
		}
	}

	//g_pPhysixCtx->PollForWork();
	NxMaterial * pMtl = g_pPhysixCtx->validateMaterial( m_Restitution, m_StaticFriction, m_DynamicFriction);
	I3ASSERT( pMtl != nullptr);

	//I3ASSERT( pDesc->isValid());
	I3ASSERT( m_pNxActor == nullptr );

	//g_pPhysixCtx->PollForWork();

	pDesc->flags = NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE | NX_AF_LOCK_COM;

	#if defined( OWN_PAIR_FILTER)
		pDesc->flags |= NX_AF_USER_ACTOR_PAIR_FILTERING;
	#endif

	if( pDesc->body != nullptr )
	{
		((NxBodyDesc *)pDesc->body)->flags &= ~NX_BF_VISUALIZATION;
		((NxBodyDesc *)pDesc->body)->flags |= NX_BF_DISABLE_GRAVITY;
	}
	
	for( i = 0; i < pDesc->shapes.size(); i++)
	{
		NxShapeDesc * pShapeDesc = pDesc->shapes[i];

		pShapeDesc->shapeFlags &= ~NX_SF_VISUALIZATION;
		pShapeDesc->shapeFlags |= NX_SF_DISABLE_RAYCASTING;
	}

	m_pNxActor = g_pPhysixCtx->createActor( pDesc);
	I3ASSERT( m_pNxActor != nullptr);
	
	m_pNxActor->userData = this;

	//modifyDynamicState( I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_FROZEN | I3_PHYSIX_STATE_NO_GRAVITY, 0);

	
	bool bTriMesh = false;
	NxShape * const * ppShape = m_pNxActor->getShapes();

	for( i = 0; i < m_pNxActor->getNbShapes(); i++)
	{	
		ppShape[i]->setMaterial( pMtl->getMaterialIndex());
		ppShape[i]->userData = this;

		if( ppShape[i]->isTriangleMesh())
			bTriMesh = true;
	}

	// Tri-Mesh가 아니라면 m_TerrainCount 및 m_pTerrainArrays는 개별 Convex에 대한
	// Terrain 정보로 의미가 변경된다.
	// 그렇기 때문에 반드시 Shape 개수와 동일한 개수의 Terrain Count가 존재해야 한다.
	if( bTriMesh == false && getTerrainArray() == nullptr )
	{
		PrepareTerrainArrayForConvex();
	}

	if( m_pNxActor->isDynamic())
	{
		m_pNxActor->setSolverIterationCount( 1 );
		getInertiaTensor( &m_vInertia );
	}
	else
	{
		i3Vector::Zero( &m_vInertia);
	}
#endif
}

void i3PhysixShapeSet::PrepareTerrainArrayForConvex(void)
{
	I3_PHYSIX_TERRAIN * pTerrain;

	if( m_pNxActor == nullptr)
		return;

	NxShape * const * ppShape = m_pNxActor->getShapes();
	INT32 i;

	I3_SAFE_RELEASE( m_pTerrainArray);

	m_TerrainCount = m_pNxActor->getNbShapes();

	m_pTerrainArray = i3MemoryBuffer::new_object();
	m_pTerrainArray->Create( sizeof(I3_PHYSIX_TERRAIN) * m_TerrainCount);

	pTerrain = (I3_PHYSIX_TERRAIN *) m_pTerrainArray->getBuffer();

	i3mem::FillZero( pTerrain, sizeof(I3_PHYSIX_TERRAIN) * m_TerrainCount);

	// shape의 이름을 보고 Terrain을 판단한다.
	for( i = 0; i < (INT32) m_pNxActor->getNbShapes(); i++)
	{
		if( ppShape[i]->getName() != nullptr)
		{
			pTerrain[i].m_Terrain = (UINT16) NXU::GetTerrainTypeByName( ppShape[i]->getName());
			pTerrain[i].m_Thickness = 100;
		}
		else
		{
			pTerrain[i].m_Terrain = I3_TERRAIN_NA;
			pTerrain[i].m_Thickness = 0;
		}
	}
}

void i3PhysixShapeSet::CreateJoint( i3PhysixShapeSet * pParent)
{
#if !defined( _DBG_NOCREATE_)
	// 이미 Desc Data가 존재하는 상태에서의 생성
	//
	// 여러 LOD에서 하나의  ShapeSet들을 공유함으로 인해,
	// 여러 번 호출 될 수 있다.
	if( m_pNxJoint == nullptr)
	{
		NxD6JointDesc desc;

		I3ASSERT( m_pJointInfo != nullptr);

		m_pJointInfo->SetTo( &desc);

		if( pParent == nullptr)
			desc.actor[0] = nullptr;
		else
			desc.actor[0]	=	pParent->getNxActor();

		desc.actor[1]	=	getNxActor();

		desc.jointFlags &= ~NX_JF_VISUALIZATION;
		
		m_pNxJoint = g_pPhysixCtx->createJoint( &desc);
		I3ASSERT( m_pNxJoint != nullptr);

		//g_pPhysixCtx->setActorPairFlags( desc.actor[0], desc.actor[1], NX_IGNORE_PAIR);
	}
#endif
}

void i3PhysixShapeSet::CreateSphereShape( VEC3D * pCenterOff, REAL32 radius, INT32 group )
{
#if !defined( _DBG_NOCREATE_)
	NxSphereShapeDesc shapeDesc;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	
	shapeDesc.group = (NxCollisionGroup) group;
	shapeDesc.localPose.t.x = pCenterOff->x;
	shapeDesc.localPose.t.y = pCenterOff->y;
	shapeDesc.localPose.t.z = pCenterOff->z;
	shapeDesc.radius		= radius;

	actorDesc.group = (NxActorGroup) group;
	actorDesc.body = &bodyDesc;
	actorDesc.shapes.pushBack( &shapeDesc);
	actorDesc.density = 1.0f;
	actorDesc.userData = this;

	I3ASSERT( actorDesc.isValid());

	Create( &actorDesc );
#endif
}

void i3PhysixShapeSet::CreateBoxShape( VEC3D * pCenter, VEC3D * pSize, INT32 group, bool bBody, MATRIX * pGlobalPose, I3_TERRAIN_TYPE terrain)
{
#if !defined( _DBG_NOCREATE_)
	NxBoxShapeDesc boxDesc;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	boxDesc.setToDefault();

	boxDesc.dimensions.x = pSize->x;
	boxDesc.dimensions.y = pSize->y;
	boxDesc.dimensions.z = pSize->z;
	boxDesc.group = (NxCollisionGroup) group;
	boxDesc.localPose.t.x = pCenter->x;
	boxDesc.localPose.t.y = pCenter->y;
	boxDesc.localPose.t.z = pCenter->z;
	boxDesc.mass = 0.0f;
	boxDesc.density = 1.0f;

	boxDesc.materialIndex = (NxMaterialIndex) terrain;
	
	actorDesc.setToDefault();

	actorDesc.shapes.pushBack( &boxDesc);
	actorDesc.density = 1.0f;
	actorDesc.userData = this;
	actorDesc.group = (NxActorGroup) group;

	if( pGlobalPose != nullptr)
	{
		NXU::Copy( &actorDesc.globalPose, pGlobalPose);
	}

	if( bBody)
	{
		bodyDesc.setToDefault();
		bodyDesc.mass = 0.0f;

		actorDesc.body = &bodyDesc;
	}

	I3ASSERT( actorDesc.isValid());

	Create( &actorDesc);

	I3_PHYSIX_TERRAIN * pTerrain  = getTerrain( 0);
	pTerrain->m_Terrain = (UINT16) terrain;
#endif
}

void i3PhysixShapeSet::CreatePlaneShape( VEC3D * pNormal, REAL32 rDistance, INT32 group, bool bBody )
{
#if !defined( _DBG_NOCREATE_)
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	//NxBodyDesc bodyDesc;

	planeDesc.normal.x = getX( pNormal );
	planeDesc.normal.y = getY( pNormal );
	planeDesc.normal.z = getZ( pNormal );
	planeDesc.d		= rDistance;
	planeDesc.group = (NxCollisionGroup) group;
	
	actorDesc.shapes.pushBack( &planeDesc);
	actorDesc.density = 1.0f;
	actorDesc.userData = this;
	actorDesc.group = (NxActorGroup) group;

	//if( bBody)
	{
		//actorDesc.body = &bodyDesc;
	}

	I3ASSERT( actorDesc.isValid());

	Create( &actorDesc);
#endif
}

void i3PhysixShapeSet::CreateCapsuleShape(VEC3D * pCenter, REAL32 radius, REAL32 height, INT32 group)
{
#if !defined( _DBG_NOCREATE_)
	NxCapsuleShapeDesc capsuleDesc;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	capsuleDesc.localPose.t.x = pCenter->x;
	capsuleDesc.localPose.t.y = pCenter->y;
	capsuleDesc.localPose.t.z = pCenter->z;
	capsuleDesc.radius = radius;
	capsuleDesc.height = height;
	capsuleDesc.group = (NxCollisionGroup) group;

	actorDesc.shapes.push_back(&capsuleDesc);
	actorDesc.density = 1.0f;
	actorDesc.userData = this;
	actorDesc.group = (NxActorGroup) group;

	bodyDesc.setToDefault();
	bodyDesc.mass = 0.0f;

	actorDesc.body = &bodyDesc;

	I3ASSERT( actorDesc.isValid());

	Create(&actorDesc);
#endif
}

void i3PhysixShapeSet::setEventReceiver( i3EventReceiver * pObj)
{
	m_pReceiver = pObj;
}

void i3PhysixShapeSet::setShapeGroup( UINT16 grp)
{
	UINT32 i;
	NxShape * const * ppShape;

	if( m_pNxActor != nullptr)
	{
		ppShape = m_pNxActor->getShapes();
		m_pNxActor->setGroup( grp);

		for( i = 0; i < m_pNxActor->getNbShapes(); i++)
		{
			ppShape[i]->setGroup( grp);
		}
	}

	m_ShapeGroup = grp;
}

void i3PhysixShapeSet::setShapeGroupMask( UINT32 mask)
{
	UINT32 i;
	NxShape * const * ppShape;
	NxGroupsMask grpMask;

	grpMask.bits0 = mask;
	grpMask.bits1 = 0;
	grpMask.bits2 = 0;
	grpMask.bits3 = 0;

#if defined( I3_DEBUG ) && defined( I3_DUMP_SHAPESET )
	I3TRACE( "ShapeGroupMask Name:%s, %x.  addr:%x\n", this->GetName(), mask, this );
#endif

	if( m_pNxActor != nullptr)
	{
		ppShape = m_pNxActor->getShapes();

		for( i = 0; i < m_pNxActor->getNbShapes(); i++)
		{
			ppShape[i]->setGroupsMask( grpMask);
		}
	}
}

void i3PhysixShapeSet::getShapeGlobalPose( MATRIX * pMtx)
{
	if( m_pNxActor != nullptr)
	{
		NxMat34 tmp(m_pNxActor->getGlobalPose());
		NXU::Copy( (MATRIX*) pMtx, (NxMat34*) &tmp);
	}
	else
	{
		i3Matrix::Identity( pMtx);
	}
}

void i3PhysixShapeSet::setShapeGlobalPose( MATRIX * pMtx)
{
	if( m_pNxActor == nullptr)		return;
	if( i3Matrix::IsSame( &m_mtxUpdate, pMtx))
	{
		return;
	}

	i3Matrix::Copy( &m_mtxUpdate, pMtx);

	g_pPhysixCtx->CMD_updateGlobalPose( this, pMtx);
}

void i3PhysixShapeSet::setLinearVelocity( VEC3D * pVec )
{
	if( m_pNxActor == nullptr )	return;
	g_pPhysixCtx->CMD_setLinearVelocity( this, pVec);
}

void i3PhysixShapeSet::setAngularVelocity( VEC3D * pVec)
{
	if( m_pNxActor == nullptr )	return;
	g_pPhysixCtx->CMD_setAngularVelocity( this, pVec);
}

void i3PhysixShapeSet::CopyFaceArray( I3_PHYSIX_TRI * pTriArray, UINT32 count)
{
	//UINT32 i;
	I3_PHYSIX_TRI * pDest;

	I3_SAFE_RELEASE( m_pFaceArray);

	m_FaceCount = count;

	m_pFaceArray = i3MemoryBuffer::new_object();
	m_pFaceArray->Create( count * sizeof(I3_PHYSIX_TRI));

	pDest = (I3_PHYSIX_TRI *) m_pFaceArray->getBuffer();

	i3mem::Copy( pDest, pTriArray, sizeof(I3_PHYSIX_TRI) * count);
}


void * i3PhysixShapeSet::_BuildTriMeshActor(void)
{
	NxTriangleMeshDesc desc;
	NxTriangleMesh * pNxMesh;

	if( g_pPhysixCtx->IsHardwarePresent() )
	{
		desc.flags |= NX_MF_HARDWARE_MESH;
	}
	desc.numVertices = m_pPosArray->GetVectorCount();
	desc.points = m_pPosArray->GetVectorArray();
	desc.pointStrideBytes = sizeof(VEC3D);

	desc.numTriangles = getFaceCount();
	desc.triangles = m_pFaceArray->getBuffer();
	desc.triangleStrideBytes = sizeof(I3_PHYSIX_TRI);

	{
		I3_PHYSIX_TRI * pTri = (I3_PHYSIX_TRI *) m_pFaceArray->getBuffer();

		desc.materialIndexStride = sizeof(I3_PHYSIX_TRI);
		desc.materialIndices = &pTri->m_MaterialIndex;
	}
	
	//desc.flags = NX_MF_16_BIT_INDICES;

	I3ASSERT( desc.isValid());

	{
		NXU::MemoryWriteBuffer buf;

		NXU::InitCooking();

		if( NXU::CookTriangleMesh( desc, buf) == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not cook physix triangle mesh");
			return nullptr;
		}

		pNxMesh = g_pPhysixCtx->getSDK()->createTriangleMesh( NXU::MemoryReadBuffer(buf.data));
		I3ASSERT( pNxMesh != nullptr);

		//AddMeshData( pNxMesh, false);

		NXU::CloseCooking();
	}

	return pNxMesh;
}

void * i3PhysixShapeSet::_BuildConvexMeshActor(void)
{
	return nullptr;
}

void i3PhysixShapeSet::_UpdateDynamicState(void)
{
	if( m_pNxActor == nullptr)
		return;

	UINT32 addFlag = 0, removeFlag = 0;

	// actor flag set
	if( m_dynamicState & I3_PHYSIX_STATE_NO_COLLISION)
	{
		if( m_pNxActor->readActorFlag( NX_AF_DISABLE_COLLISION) == false)
			addFlag |= NX_AF_DISABLE_COLLISION;
	}
	else
	{
		if( m_pNxActor->readActorFlag( NX_AF_DISABLE_COLLISION ))
			removeFlag |= NX_AF_DISABLE_COLLISION;
	}

	if( m_dynamicState & I3_PHYSIX_STATE_NO_RESPONSE)
	{
		if( m_pNxActor->readActorFlag( NX_AF_DISABLE_RESPONSE) == false)
			addFlag |= NX_AF_DISABLE_RESPONSE;
	}
	else
	{
		if( m_pNxActor->readActorFlag( NX_AF_DISABLE_RESPONSE))
			removeFlag |= NX_AF_DISABLE_RESPONSE;
	}

	if( m_dynamicState & I3_PHYSIX_STATE_LOCK_COM )
	{
		if( m_pNxActor->readActorFlag( NX_AF_LOCK_COM) == false)
			addFlag |= NX_AF_LOCK_COM;
	}
	else
	{
		if( m_pNxActor->readActorFlag( NX_AF_LOCK_COM ))
			removeFlag |= NX_AF_LOCK_COM;
	}

	if( removeFlag)
		m_pNxActor->clearActorFlag( (NxActorFlag) removeFlag);

	if( addFlag)
		m_pNxActor->raiseActorFlag( (NxActorFlag) addFlag);

	// body flag set
	VEC3D vZero;
	
	if( m_pNxActor->isDynamic())
	{
		addFlag = 0;
		removeFlag = 0;
		bool bMove = false;

		if( m_dynamicState & I3_PHYSIX_STATE_KINEMATIC )
		{
			if( m_pNxActor->readBodyFlag( NX_BF_KINEMATIC) == false)
			{
				addFlag |= NX_BF_KINEMATIC;
				bMove = true;
			}
		}
		else
		{
			if( m_pNxActor->readBodyFlag( NX_BF_KINEMATIC ))
			{
				removeFlag |= NX_BF_KINEMATIC;
			}

			setSleeping(false);
		}

		if( m_dynamicState & I3_PHYSIX_STATE_NO_GRAVITY)
		{
			if( m_pNxActor->readBodyFlag( NX_BF_DISABLE_GRAVITY) == false)
				addFlag |= NX_BF_DISABLE_GRAVITY;
		}
		else
		{
			if( m_pNxActor->readBodyFlag( NX_BF_DISABLE_GRAVITY))
				removeFlag |= NX_BF_DISABLE_GRAVITY;
		}

		if( m_dynamicState & I3_PHYSIX_STATE_FROZEN )
		{
			if( m_pNxActor->readBodyFlag( NX_BF_FROZEN) == false)
				addFlag |= NX_BF_FROZEN;
		}
		else
		{
			if( m_pNxActor->readBodyFlag( NX_BF_FROZEN))
				removeFlag |= NX_BF_FROZEN;
		}

		if( removeFlag)
			m_pNxActor->clearBodyFlag( (NxBodyFlag) removeFlag);

		if( addFlag)
			m_pNxActor->raiseBodyFlag( (NxBodyFlag) addFlag);

		if( bMove)
			m_pNxActor->moveGlobalPosition(m_pNxActor->getGlobalPosition());
	}


	NxShape * const * ppShape = m_pNxActor->getShapes();

	if (ppShape == nullptr)
	{
		I3TRACE("i3PhysixShapeSet::_UpdateDynamicState ppShape = nullptr\n");
		return;
	}
	
	NxGroupsMask mask;

	// GroupsMask에 Kinematic 상태를 추가하여 Actor Pair Filtering한다.
	if( (m_dynamicState & I3_PHYSIX_STATE_KINEMATIC))
		mask.bits0 = PHF_FILTER;
	else
		mask.bits0 = 0;

	mask.bits1 = mask.bits2 = mask.bits3 = 0;

	for( UINT32 i = 0; i < m_pNxActor->getNbShapes(); i++ )
	{
		ppShape[i]->setGroupsMask( mask);

		if( m_dynamicState & I3_PHYSIX_STATE_NO_RAYCAST)
		{
			if( !ppShape[i]->getFlag( NX_SF_DISABLE_RAYCASTING ) )
				ppShape[i]->setFlag( NX_SF_DISABLE_RAYCASTING, true);
		}
		else
		{
			if( ppShape[i]->getFlag( NX_SF_DISABLE_RAYCASTING ) )
				ppShape[i]->setFlag( NX_SF_DISABLE_RAYCASTING, false);
		}

		if( m_dynamicState & I3_PHYSIX_STATE_TRIGGER)
		{
			if( !ppShape[i]->getFlag( NX_TRIGGER_ENABLE))
				ppShape[i]->setFlag( NX_TRIGGER_ENABLE, true);
		}
		else
		{
			if( ppShape[i]->getFlag( NX_TRIGGER_ENABLE ) )
				ppShape[i]->setFlag( NX_TRIGGER_ENABLE, false);
		}
	}
}

void i3PhysixShapeSet::setDynamicState( UINT32 state)
{
	m_dynamicState = state;

	g_pPhysixCtx->CMD_updateDynamicState( this);
}

void i3PhysixShapeSet::modifyDynamicState( UINT32 addState, UINT32 removeState)
{
	m_dynamicState |= addState;
	m_dynamicState &= ~removeState;

	g_pPhysixCtx->CMD_updateDynamicState( this);
}

void i3PhysixShapeSet::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	UINT32 i;
	i3BoundBox * pBox = pInfo->m_pBound;

	if( m_pNxActor == nullptr)
	{
		return;
	}

	NxShape * const * pShape = m_pNxActor->getShapes();

	for( i = 0; i < m_pNxActor->getNbShapes(); i++)
	{
		NxBounds3 bound;
		bound.setEmpty();
		pShape[i]->getWorldBounds( bound);

		if( pBox->IsEmpty())
		{
			pBox->SetMinMax( (VEC3D*) &bound.min, (VEC3D*) &bound.max);
		}
		else
		{
			pBox->ExtendByVec( (VEC3D*) &bound.min);
			pBox->ExtendByVec( (VEC3D*) &bound.max);
		}
	}
}


void i3PhysixShapeSet::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3EventReceiver::CopyTo( pDest, method);

	i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pDest;

	//g_pPhysixCtx->Dump();

	pShapeSet->setShapeType( getShapeType());
	pShapeSet->setShapeGroup( getShapeGroup());
	pShapeSet->setEventReceiver( getEventReceiver());

	pShapeSet->setTerrainCount( getTerrainCount());
	pShapeSet->setTerrainArray( getTerrainArray());

	pShapeSet->setFaceCount( m_FaceCount);
	pShapeSet->setFaceArray( m_pFaceArray);

	pShapeSet->setPositionArray( getPositionArray());
	pShapeSet->setJointInfo( getJointInfo());

	pShapeSet->m_nVersion = m_nVersion;
	pShapeSet->setFormat( getFormat());

	if( m_pNxActor != nullptr)
	{
		NxActorDesc				actorDesc;
		NxBodyDesc				bodyDesc;
		NxBoxShapeDesc			boxShapeDesc;
		NxCapsuleShapeDesc		capsuleShapeDesc;
		NxConvexShapeDesc		convexShapeDesc;
		NxTriangleMeshShapeDesc	triMeshShapeDesc;

		m_pNxActor->saveToDesc( actorDesc);
		actorDesc.name = m_pNxActor->getName();

		if( m_pNxActor->isDynamic())
		{
			actorDesc.body = &bodyDesc;

			m_pNxActor->saveBodyToDesc( bodyDesc);
		}
		else
		{
			actorDesc.body = nullptr;
		}

		UINT32 i;
		NxShapeDesc * pShapeDesc = nullptr;
		NxShape * const * ppShape = m_pNxActor->getShapes();

		for( i = 0; i < m_pNxActor->getNbShapes(); i++)
		{
			switch( ppShape[i]->getType())
			{
				case NX_SHAPE_PLANE :
					{
						NxPlaneShapeDesc * pDesc = new NxPlaneShapeDesc;
						NetworkDump_Malloc( pDesc, sizeof( NxPlaneShapeDesc), __FILE__, __LINE__);
						((NxPlaneShape *) ppShape[i])->saveToDesc( *pDesc);
						pShapeDesc = pDesc;
					}
					break;

				case NX_SHAPE_SPHERE :	
					{
						NxSphereShapeDesc * pDesc = new NxSphereShapeDesc;
						NetworkDump_Malloc( pDesc, sizeof( NxSphereShapeDesc), __FILE__, __LINE__);
						((NxSphereShape *) ppShape[i])->saveToDesc( *pDesc);
						pShapeDesc = pDesc;
					}
					break;

				case NX_SHAPE_BOX :			
					{
						NxBoxShapeDesc * pDesc = new NxBoxShapeDesc;
						NetworkDump_Malloc( pDesc, sizeof( NxBoxShapeDesc), __FILE__, __LINE__);
						((NxBoxShape *) ppShape[i])->saveToDesc( *pDesc);
						pShapeDesc = pDesc;
					}
					break;

				case NX_SHAPE_CAPSULE :	
					{
						NxCapsuleShapeDesc * pDesc = new NxCapsuleShapeDesc;
						NetworkDump_Malloc( pDesc, sizeof( NxCapsuleShapeDesc), __FILE__, __LINE__);
						((NxCapsuleShape *) ppShape[i])->saveToDesc( *pDesc);
						pShapeDesc = pDesc;
					}
					break;

				case NX_SHAPE_CONVEX :
					{
						NxConvexShapeDesc * pDesc = new NxConvexShapeDesc;
						NetworkDump_Malloc( pDesc, sizeof( NxConvexShapeDesc), __FILE__, __LINE__);
						((NxConvexShape *) ppShape[i])->saveToDesc( *pDesc);
						pShapeDesc = pDesc;
					}
					break;

				case NX_SHAPE_MESH :
					{
						NxTriangleMeshShapeDesc * pDesc = new NxTriangleMeshShapeDesc;
						NetworkDump_Malloc( pDesc, sizeof( NxTriangleMeshShapeDesc), __FILE__, __LINE__);
						((NxTriangleMeshShape *) ppShape[i])->saveToDesc( *pDesc);
						pShapeDesc = pDesc;
					}
					break;

				default :
					I3ASSERT_0;
					break;
			}

			// xgkick
			// NXU 내부에서 Desc를 복사할 때, name Field의 의미없는 Pointer 값을 그냥
			// 단순 복사해, Invalid Pointer인 경우라도 그대로 사용해 문제가 발생
			// 이것을 피하기 위해, Shape 단계에는 이름을 아예 가지지 않도록 강제하도록 함.
			pShapeDesc->name = nullptr;

			actorDesc.shapes.pushBack( pShapeDesc);
		}
		
		I3ASSERT( actorDesc.shapes.size() > 0);

		actorDesc.density = 0.0f;			// 중요!   아무 계산을 하지 않도록 하며, 정상적인 생성을 위해서 필요.
		pShapeSet->Create( &actorDesc);

		for( i = 0; i < actorDesc.shapes.size(); i++)
		{
			I3_MUST_DELETE( actorDesc.shapes[i]);
		}

		pShapeSet->m_dynamicState = m_dynamicState;
		pShapeSet->_UpdateDynamicState();
	}
	else
	{
		I3_REF_CHANGE( pShapeSet->m_pNxData, m_pNxData);
		I3_REF_CHANGE( pShapeSet->m_pCollectionData, m_pCollectionData);
	}

	//g_pPhysixCtx->Dump();
}

void i3PhysixShapeSet::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pTerrainArray != nullptr)
	{
		m_pTerrainArray->OnBuildObjectList( List);
	}

	if( m_pFaceArray != nullptr)
	{
		m_pFaceArray->OnBuildObjectList( List);
	}

	if( m_pPosArray != nullptr)
	{
		m_pPosArray->OnBuildObjectList( List);
	}

	if( m_pJointInfo != nullptr)
	{
		m_pJointInfo->OnBuildObjectList( List);
	}

	i3EventReceiver::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 PHYSIX_SHAPE_SET
	{
		INT8		m_ID[4] = { 'P', 'X', 'A', '3' };
		INT32		m_ShapeType = 0;
		OBJREF		m_pReceiver = 0;
		INT32		m_NxDataSize = 0;

		OBJREF		m_pTerrainArray = 0;
		UINT32		m_TerrainCount = 0;

		OBJREF		m_pFaceArray = 0;
		UINT32		m_FaceCount = 0;

		OBJREF		m_pPosArray = 0;

		REAL32		m_Restitution = 0.0f;
		REAL32		m_StaticFriction = 0.0f;
		REAL32		m_DynamicFriction = 0.0f;

		OBJREF		m_pJointInfo = 0;
		INT32		m_ShapeGroup = 0;

		REAL32		m_Mass = 0.0f;
		UINT8		m_bBody = 0;
		UINT8		m_TerrainType = 0;		// Tri-mesh가 아닌 경우에 사용된다.
		UINT8		m_Format = 0;
		UINT8		pad = 0;

		INT32		m_Reserved[1] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3PhysixShapeSet::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::PHYSIX_SHAPE_SET Data;
	void * pSaveData = nullptr;

	// Node
	Result = i3EventReceiver::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Data.m_ShapeType = (INT32) getShapeType();

	if( m_pNxActor == nullptr )
	{
		I3TRACE( "nxActor nullptr. instancing...\n" );

		i3MatrixStack mtxStack;
		I3_ONINSTANCING_INFO info;
		info._pBase			= getEventReceiver();
		info._pMatrixStack	= &mtxStack;
		OnInstancing( &info);
	}

	// PhysX Data를 저장.
	if( m_pNxActor != nullptr)
	{
		NXU::NxuPhysicsCollection * pColl = NXU::createCollection();
		NXU::NXU_FileType	fileType;

		I3ASSERT( pColl != nullptr);

		// Actor를 추가한다.
		NXU::addActor( *pColl, *m_pNxActor);
		
		char szID[64];

		sprintf( szID, "ShapeSet%p", this);

		switch( getFormat())
		{
			case FORMAT_BINARY :		fileType = NXU::FT_BINARY;	break;
			default :					fileType = NXU::FT_XML;		break;
		}

		pSaveData = NXU::saveCollectionToMemory( pColl, szID, fileType, true, true, nullptr, 0, Data.m_NxDataSize);
		if( pSaveData == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not save PhysX Actor data.");
			I3_MUST_DELETE( pColl);
			return STREAM_ERR;
		}

		NXU::releaseCollection( pColl);
	}

	// Header
	Data.m_pReceiver		= pResFile->GetObjectPersistID( getEventReceiver());

	Data.m_pTerrainArray	= pResFile->GetObjectPersistID( getTerrainArray());
	Data.m_TerrainCount		= m_TerrainCount;

	Data.m_pFaceArray		= pResFile->GetObjectPersistID( m_pFaceArray);
	Data.m_FaceCount		= getFaceCount();

	Data.m_pPosArray		= pResFile->GetObjectPersistID( m_pPosArray);

	Data.m_pJointInfo		= pResFile->GetObjectPersistID( m_pJointInfo);

	Data.m_Restitution		= m_Restitution;
	Data.m_StaticFriction	= m_StaticFriction;
	Data.m_DynamicFriction	= m_DynamicFriction;

	Data.m_ShapeGroup		= m_ShapeGroup;
	Data.m_Format			= (UINT8) getFormat();

	if( m_pNxActor != nullptr)
	{
		Data.m_bBody			= m_pNxActor->isDynamic();
		Data.m_Mass				= m_pNxActor->getMass();
	}

	Rc = pResFile->Write( &Data, sizeof(Data));
	I3_CHKIO( Rc);

	// Shape(Mesh) Data
	if( pSaveData != nullptr)
	{
		Rc = pResFile->Write( pSaveData, Data.m_NxDataSize);
		I3_CHKIO( Rc);
	
		NXU::releaseCollectionMemory( pSaveData);
	}

	return Result;
}

//static List s_MeshList;				// 쓰이지 않는듯 하여 주석

// NxInstantiator에 의해 Loading 중에 각 Object들이 생성되는 시점을 CALLBACK해주는 Class이다.
class NXU_ActorNotify : public NXU_userNotify
{
public:
	i3PhysixShapeSet *		m_pShapeSet;
//	List *				m_pMaterialList;					// 쓰이지 않는듯 하여 주석처리..
	INT32					m_MaterialIndex;
	void  *					m_pCookedMesh;
	INT32					m_ShapeGroup;

	virtual void NXU_notifyActor( NxActor * actor, const char * userProperties)
	{	
		UINT32 i;
		NxShape * const * ppShape = actor->getShapes();

		m_pShapeSet->setNxActor( actor);
		actor->userData = m_pShapeSet;

		for( i = 0; i < actor->getNbShapes(); i++)
		{
			ppShape[i]->setMaterial( (NxMaterialIndex) m_MaterialIndex);
			ppShape[i]->userData = m_pShapeSet;
		}
	}

	virtual bool	NXU_preNotifyActor(NxActorDesc &actor, const char	*userProperties)
	{
		UINT32 i;

		actor.flags = NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE | NX_AF_LOCK_COM;

		#if defined( OWN_PAIR_FILTER)
			actor.flags |= NX_AF_USER_ACTOR_PAIR_FILTERING;
		#endif

		if( actor.body != nullptr )
		{
			((NxBodyDesc *)actor.body)->flags &= ~NX_BF_VISUALIZATION;
			((NxBodyDesc *)actor.body)->flags |= NX_BF_DISABLE_GRAVITY;

			//if( m_pShapeSet->m_pvInitialPos != nullptr )
			{
				//actor.globalPose.t.set( getX(m_pShapeSet->m_pvInitialPos), getY(m_pShapeSet->m_pvInitialPos), getZ(m_pShapeSet->m_pvInitialPos));
			}

			{
				VEC3D tmp;
				i3Vector::Normalize( &tmp, i3Matrix::GetRight( &m_pShapeSet->m_mtxInitiate));
				actor.globalPose.M.setColumn( 0, *((NxVec3 *) &tmp));

				i3Vector::Normalize( &tmp, i3Matrix::GetUp( &m_pShapeSet->m_mtxInitiate));
				actor.globalPose.M.setColumn( 1, *((NxVec3 *) &tmp));

				i3Vector::Normalize( &tmp, i3Matrix::GetAt( &m_pShapeSet->m_mtxInitiate));
				actor.globalPose.M.setColumn( 2, *((NxVec3 *) &tmp));

				actor.globalPose.t.set( getX( i3Matrix::GetPos( &m_pShapeSet->m_mtxInitiate)), getY( i3Matrix::GetPos( &m_pShapeSet->m_mtxInitiate)), getZ( i3Matrix::GetPos( &m_pShapeSet->m_mtxInitiate)));
			}
		}
		
		for( i = 0; i < actor.shapes.size(); i++)
		{
			NxShapeDesc * pShapeDesc = actor.shapes[i];

			pShapeDesc->shapeFlags &= ~NX_SF_VISUALIZATION;
			pShapeDesc->group = (NxCollisionGroup) m_ShapeGroup;
			pShapeDesc->name = nullptr;

			switch( pShapeDesc->getType())
			{
				case NX_SHAPE_MESH :
					{
						NxTriangleMeshShapeDesc * pTriShape = (NxTriangleMeshShapeDesc *) pShapeDesc;

						if( pTriShape->meshData != nullptr)
						{
							g_pPhysixCtx->getSDK()->releaseTriangleMesh( *pTriShape->meshData);
						}

						pTriShape->meshData = (NxTriangleMesh *) m_pCookedMesh;
						pTriShape->shapeFlags	|= NX_SF_FEATURE_INDICES;			// Contact indices 사용(Trimesh는 월드 히트용으로만 사용한다는 전제)
						pTriShape->shapeFlags	|= NX_SF_POINT_CONTACT_FORCE;

						//I3TRACE( "MESH : %f %f %f\n", pTriShape->localPose.t.x, pTriShape->localPose.t.y, pTriShape->localPose.t.z);
					}
					break;
				case NX_SHAPE_CONVEX :
					// 히트용 컨벡스라면 인덱스를 사용하여 히트 정보를 가져와야 합니다.
					if( m_pShapeSet != nullptr )	// shapecontainer::usage USAGE_RAYCAST
					{
						NxConvexShapeDesc * pConvexShape = (NxConvexShapeDesc*) pShapeDesc;
						pConvexShape->shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
					}
					break;
			}
		}

		return	true; 
	}
};

UINT32 i3PhysixShapeSet::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::PHYSIX_SHAPE_SET Data;

	// node
	Result = i3EventReceiver::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pResFile->Read( &Data, sizeof(Data));
	I3_CHKIO( Rc);

	if( Data.m_ID[3] == '1' )
		m_nVersion = 1;
	else if( Data.m_ID[3] == '2' )
		m_nVersion = 2;
	else if( Data.m_ID[3] == '3')
		m_nVersion = 3;

	if( Data.m_pReceiver != 0)
	{
		i3EventReceiver * pObj;

		pObj = (i3EventReceiver *) pResFile->FindObjectByID( Data.m_pReceiver);

		if( pObj != nullptr)
		{
			setEventReceiver( pObj);
		}
	}

	setShapeType( (I3_PHYSIX_SHAPE_TYPE) Data.m_ShapeType);
	setShapeGroup( (UINT16) Data.m_ShapeGroup);
	setFormat( (FORMAT) Data.m_Format);

	// Terrain Array
	if( Data.m_pTerrainArray != 0)
	{
		i3MemoryBuffer * pBuff = (i3MemoryBuffer *) pResFile->FindObjectByID( Data.m_pTerrainArray);
		I3ASSERT( pBuff != nullptr);

		setTerrainArray( pBuff);
	}
	setTerrainCount( Data.m_TerrainCount);

	if( Data.m_pFaceArray != 0)
	{
		i3MemoryBuffer * pBuff = (i3MemoryBuffer *) pResFile->FindObjectByID( Data.m_pFaceArray);
		I3ASSERT( pBuff != nullptr);

		setFaceArray( pBuff);
	}

	setFaceCount( Data.m_FaceCount);

	// Position Array
	if( Data.m_pPosArray != 0)
	{
		i3Vector3Array * pArray = (i3Vector3Array *) pResFile->FindObjectByID( Data.m_pPosArray);

		setPositionArray( pArray);
	}

	if( Data.m_Restitution > 0.01f )
		Data.m_Restitution *= 0.01f;
	setRestitution( Data.m_Restitution);
	setFriction( Data.m_StaticFriction, Data.m_DynamicFriction);

#if !defined( _DBG_NOCREATE_)	
	// Shape Data
	//if( pMesh == nullptr)
	{
		NXU::NXU_FileType typeFile;

		switch( getFormat())
		{
			case FORMAT_BINARY :		typeFile = NXU::FT_BINARY;	break;
			default :					typeFile = NXU::FT_XML;		break;
		}

		m_NxDataSize = Data.m_NxDataSize;

		if( Data.m_NxDataSize > 0)
		{	
			// Desc. Data가 있다는 것은 Mesh가 아니라, Capsule, Sphere 등과 같은 
			// Primitive Shape의 경우이다.
			m_pNxData = i3MemoryBuffer::new_object();
			m_pNxData->Create( Data.m_NxDataSize);
			char * pData = (char *) m_pNxData->getBuffer();

			I3ASSERT( pData != nullptr);

			Rc = pResFile->Read( pData, Data.m_NxDataSize);
			I3_CHKIO( Rc);
			Result +=Rc;

#if defined( USE_MEMBUFF_COLLECTION )
			// Collection data를 여기에서 뽑아냅니다.
			m_pCollectionData = i3MemoryBuffer::new_object();
			
			NXU::NxuPhysicsCollection * pColl;
			pColl = NXU::loadCollection( "PX", typeFile, pData, m_pNxData->GetSize());
			m_pCollectionData->Bind( pColl, m_pNxData->GetSize());
#endif
		}
	}
	/*
	else
	{
		NxActorDesc actorDesc;
		NxBodyDesc bodyDesc;
		NxTriangleMeshShapeDesc shapeDesc;

		actorDesc.setToDefault();
		bodyDesc.setToDefault();

		bodyDesc.mass = 0.0f;

		actorDesc.density = 1.0f;

		if( Data.m_bBody)
			actorDesc.body =  &bodyDesc;
		else
			actorDesc.body = nullptr;

		shapeDesc.group = Data.m_ShapeGroup;
		shapeDesc.meshData = (NxTriangleMesh *) pMesh;
		shapeDesc.materialIndex = pMtl->getMaterialIndex();
		shapeDesc.meshFlags = NX_MESH_SMOOTH_SPHERE_COLLISIONS;
		shapeDesc.userData = this;

		actorDesc.shapes.pushBack( &shapeDesc);

		actorDesc.flags = NX_AF_DISABLE_COLLISION | NX_AF_DISABLE_RESPONSE;
		bodyDesc.flags = NX_BF_DISABLE_GRAVITY;

		Create( &actorDesc);
	}
	*/
#endif

	// Joint Info
	if( Data.m_pJointInfo != 0)
	{
		i3PhysixJointInfo * pInfo = (i3PhysixJointInfo *) pResFile->FindObjectByID( Data.m_pJointInfo);
		I3ASSERT( pInfo != nullptr);

		setJointInfo( pInfo);
	}

	if( getFormat() == FORMAT_XML)
	{
		I3PRINTLOG(I3LOG_WARN,  "PhysX Data가 XML Format임. (%s)", pResFile->getFileName());
	}

	return Result;
}

void i3PhysixShapeSet::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	NXU::NxuPhysicsCollection * pColl = nullptr;
	char * pData;
	void * pMesh = nullptr;

	if( m_pNxActor != nullptr)
		return;
	
	//m_pvInitialPos = pInitPos;

	I3ASSERT( pInfo != nullptr);
	I3ASSERT( pInfo->_pMatrixStack != nullptr);
			
	i3Matrix::Copy( &m_mtxInitiate, pInfo->_pMatrixStack->GetTop());

	if( m_pNxData != nullptr)
	{
		pData = (char *) m_pNxData->getBuffer();
		
		switch( getShapeType())
		{
			case I3_PHYSIX_SHAPE_TRIMESH :
				pMesh = _BuildTriMeshActor();
				break;
		}

		// 로드에서 바이너리를 들도록 수정합니다. 20080310 정순구
		if( m_pCollectionData != nullptr)
		{	// 아직 확인 안된 기능.. 사용하면 안됩니다.
			// 우선적으로 Collection data에 대한 Pointer를 가져와서 인스턴스를 복사하도록 합니다.
			pColl = (NXU::NxuPhysicsCollection *) m_pCollectionData->getBuffer();
		}
		else
		{	// 메모리에 collection하기 위한 data가 있으면 이것을 씁니다. ( 퍼포먼스상 너무 느립니다.)
			NXU::NXU_FileType type;

			switch( getFormat())
			{
				case FORMAT_BINARY :		type = NXU::FT_BINARY;		break;
				default :					type = NXU::FT_XML;			break;
			}

			pColl = NXU::loadCollection( "PX", type, pData, m_pNxData->GetSize());

			I3ASSERT( pColl != nullptr);
		}

		NxMaterial * pMtl = g_pPhysixCtx->validateMaterial( m_Restitution, m_StaticFriction, m_DynamicFriction);

		NXU_ActorNotify notify;

		notify.m_MaterialIndex = pMtl->getMaterialIndex();
		notify.m_pShapeSet = this;
		notify.m_pCookedMesh = pMesh;
		notify.m_ShapeGroup = getShapeGroup();
		//notify.m_pMaterialList = &m_MaterialList;

		//g_pPhysixCtx->PollForWork();

		if( pColl != nullptr )
		{
			if( m_pCollectionData == nullptr)
			{
				if( NXU::instantiateCollection( pColl, (NxPhysicsSDK&)(*g_pPhysixCtx->getSDK()), g_pPhysixCtx->getScene(), nullptr, &notify) == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "Could not instantiated PhysX data.");
					NXU::releaseCollection( pColl);
					return;
				}

				NXU::releaseCollection( pColl);
			}
			else
			{	
				if( NXU::instantiateCollection( pColl, (NxPhysicsSDK&)(*g_pPhysixCtx->getSDK()), g_pPhysixCtx->getScene(), nullptr, &notify) == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "Could not instantiated PhysX data.");
					NXU::releaseCollection( pColl);
					return;
				}
			}
		}

		if( m_pNxActor != nullptr)
		{
			if( hasName())
				m_pNxActor->setName( GetName());

			// Terrain Array의 확인 및 생성
			//
			// Convex들로 ShapeSet이 구성된 경우, Convex의 개수만큼 Terrain Array가 준비되어야 한다.
			// 정상적인 생성 과정을 거친다면, OnLoad에서 추가적인 이하의 처리를 할 필요없이
			// 올바르게 설정된 Terrain Array가 Loading될 것이지만,
			// 중간에 추가된 기능이기 때문에 이전에 해당 조건이 갖추어지지 않은 것을 위해 처리함.
			{
				INT32 i;
				NxShape * const * ppShape = m_pNxActor->getShapes();
				bool bTriMesh = false;

				for( i = 0; i < (INT32) m_pNxActor->getNbShapes(); i++)
				{
					if( ppShape[i]->isTriangleMesh())
					{
						bTriMesh = true;
						break;
					}
				}

				if( (bTriMesh == false) && (m_TerrainCount == 0) )//&& (m_TerrainCount != m_pNxActor->getNbShapes()))
				{
					// 버그 회피..
					//
					// 실제로 m_TerrainCount == 0일 때, m_pTerrainArray != nullptr인 상황이 발생하면 안된다!!!!!
					if( getTerrainArray() == nullptr)
					{
						PrepareTerrainArrayForConvex();
					}
					else
					{
						m_TerrainCount = getTerrainArray()->GetSize() / sizeof(I3_PHYSIX_TERRAIN);
					}
				}
			}

			setShapeGroup( getShapeGroup());

			this->modifyDynamicState( I3_PHYSIX_STATE_DISABLE, 0);
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN,  "Physix Actor를 생성할 수 없습니다. 파일이 예전 버전인 경우 발생할 수 있습니다 (%s).", GetName());
		}

		// Joint에 대한 생성은 책임지지 않는다.
		// 이에 대한 것은 i3PhysixShapeSet을 소유하는 i3Transform2, i3Skeleton, i3BoneRef2 class 등이
		// 책임을 진다.

		if( m_pCollectionData == nullptr)
			NXU::releasePersistentMemory();
	}

#if defined( I3_DEBUG)
	if( (isDynamicActor() == true) && (m_ShapeType == I3_PHYSIX_SHAPE_TRIMESH) )
	{
		bool bHasName = false;

		if( pInfo->_pBase != nullptr)
		{
			if( i3::kind_of<i3NamedElement*>(pInfo->_pBase) )
			{
				i3NamedElement * pInstObj = (i3NamedElement *) pInfo->_pBase;

				I3PRINTLOG(I3LOG_WARN,  "%s 에 Dynamic Triangle Mesh로 PhysX 설정이 되어있습니다.", pInstObj->GetName());
				bHasName = true;
			}
		}
		
		if( bHasName == false)
		{
			I3PRINTLOG(I3LOG_WARN,  "특정 Object에 Dynamic Triangle Mesh로 PhysX 설정이 되어있습니다.");
		}
	}
#endif

	{
		VEC3D scale;

		i3Matrix::GetScale( &m_mtxInitiate, &scale);

		if( i3Vector::isAlike( &scale, getOriginalSize()) == false)
			AdjustScale( scale, &m_mtxInitiate);
	}
}

void i3PhysixShapeSet::PhysixCopyToShape( i3PhysixShapeSet * pDestShape )
{
	VEC3D Vec;
	MATRIX mat;

	I3ASSERT( pDestShape != nullptr );

	pDestShape->setFriction( getStaticFriction(), getDynamicFriction() );
	
	pDestShape->setAngularDamping( getAngularDamping() );
	getAngularMomentum( &Vec );
	pDestShape->setAngularMomentum( &Vec );
	getAngularVelocity( &Vec );
	pDestShape->setAngularVelocity( &Vec );

	pDestShape->setLinearDamping( getLinearDamping() );
	getLinearMomentum( &Vec );
	pDestShape->setLinearMomentum( &Vec );
	getLinearVelocity( &Vec );
	pDestShape->setLinearVelocity( &Vec );
	
	getInertiaTensor( &Vec );
	pDestShape->setInertiaTensor( &Vec );
	pDestShape->setMass( getMass() );

	getShapeGlobalPose( &mat );
	pDestShape->setShapeGlobalPose( &mat );

	getMassLocalPose( &Vec );
	pDestShape->setMassLocalPose( &Vec );

	pDestShape->setFormat( getFormat());
}

inline void _dump_vec3( const char * pszPrefix, NxVec3 * pVec)
{
	if( pszPrefix != nullptr)
	{
		I3TRACE( "%s [%f %f %f]\n", pszPrefix, pVec->x, pVec->y, pVec->z);
	}
	else
	{
		I3TRACE( "[%f %f %f]\n", pVec->x, pVec->y, pVec->z);
	}
}

void i3PhysixShapeSet::Dump( NxActorDesc * pActorDesc)
{
	NxBodyDesc * pBodyDesc;

	I3TRACE( "| Actor Dump\n");
	I3TRACE( "| name       : %s\n", pActorDesc->name);
	I3TRACE( "| group      : %d\n", pActorDesc->group);
	I3TRACE( "| flags      : [");

	if( pActorDesc->flags & NX_AF_DISABLE_COLLISION)
		I3TRACE( "No-Coll ");

	if( pActorDesc->flags & NX_AF_DISABLE_RESPONSE)
		I3TRACE( "No-Rsps ");

	if( pActorDesc->flags & NX_AF_LOCK_COM)
		I3TRACE( "Lock-COM ");

	if( pActorDesc->flags & NX_AF_FLUID_DISABLE_COLLISION)
		I3TRACE( "No-Fluid ");

	if( pActorDesc->flags & NX_AF_CONTACT_MODIFICATION)
		I3TRACE( "Cntt-Mod ");

	if( pActorDesc->flags & NX_AF_FORCE_CONE_FRICTION)
		I3TRACE( "Cone-Friction ");

	I3TRACE( "]\n");

	I3TRACE( "| density    : %f\n", pActorDesc->density);
	I3TRACE( "| isValid    : %d\n", pActorDesc->isValid());

	I3TRACE( "| Body -------------------");
	pBodyDesc = (NxBodyDesc *) pActorDesc->body;
	if( pBodyDesc == nullptr)
	{
		I3TRACE( " No\n");
	}
	else
	{
		I3TRACE( "\n");

		I3TRACE(	"| mass                 : %f\n", pBodyDesc->mass);
		_dump_vec3( "| massSpaceInertia     : ", & pBodyDesc->massSpaceInertia);
		I3TRACE(	"| linear Damping       : %f\n", pBodyDesc->linearDamping);
		I3TRACE(	"| angularDamping       : %f\n", pBodyDesc->angularDamping);
		I3TRACE(	"| maxAngularVelocity   : %f\n", pBodyDesc->maxAngularVelocity);
		I3TRACE(	"| solverIterationCount : %d\n", pBodyDesc->solverIterationCount);
		I3TRACE(	"| sleepLinearVelocity  : %f\n", pBodyDesc->sleepLinearVelocity);
		I3TRACE(	"| sleepAngularVelocity : %f\n", pBodyDesc->sleepAngularVelocity);
		I3TRACE(	"| sleepEnergyThreshold : %f\n", pBodyDesc->sleepEnergyThreshold);
		I3TRACE(	"| sleepDamping         : %f\n", pBodyDesc->sleepDamping);
		I3TRACE(	"| flags                : [");

		if( pBodyDesc->flags & NX_BF_DISABLE_GRAVITY )
			I3TRACE( "No-G ");

		if( pBodyDesc->flags & NX_BF_FROZEN)
			I3TRACE( "Frozen ");

		if( pBodyDesc->flags & NX_BF_KINEMATIC)
			I3TRACE( "Kinematic ");

		if( pBodyDesc->flags & NX_BF_VISUALIZATION)
			I3TRACE( "Visualize ");

		if( pBodyDesc->flags & NX_BF_ENERGY_SLEEP_TEST)
			I3TRACE( "Energy-sleep-test ");

		I3TRACE( "]\n");
	}

	// Shapes
	UINT32 i;
	NxShapeDesc * pShape;

	for( i = 0; i < pActorDesc->shapes.size(); i++)
	{
		pShape = pActorDesc->shapes[i];

		I3TRACE( "---------- SHAPE[%d] ------------\n", i);
		I3TRACE( "| name        : %s\n", pShape->name);
		I3TRACE( "| type        : ");

		switch( pShape->getType())
		{
			case NX_SHAPE_PLANE :		I3TRACE( "Plane\n");	break;
			case NX_SHAPE_SPHERE  :		I3TRACE( "Sphere\n");	break;
			case NX_SHAPE_BOX  :		I3TRACE( "Box\n");		break;
			case NX_SHAPE_CAPSULE  :	I3TRACE( "Capsule\n");	break;
			case NX_SHAPE_WHEEL  :		I3TRACE( "Wheel\n");	break;
			case NX_SHAPE_CONVEX  :
				I3TRACE( "Convex\n");	

				{
					NxConvexShapeDesc * pConvex = (NxConvexShapeDesc *) pShape;
					NxConvexMesh * pMesh = (NxConvexMesh *) pConvex->meshData;

					I3TRACE( "| Convex flag  : [");

					if( pConvex->meshFlags & NX_MESH_SMOOTH_SPHERE_COLLISIONS)
						I3TRACE( "Smooth-Sphere-Coll ");

					if( pConvex->meshFlags & NX_MESH_DOUBLE_SIDED)
						I3TRACE( "Double-Side ");

					I3TRACE( "]\n");

					I3TRACE( "| Face count   : %d\n", pMesh->getCount( 0, NX_ARRAY_HULL_POLYGONS));
				}
				break;
			case NX_SHAPE_MESH  :		I3TRACE( "Mesh\n");		break;
			case NX_SHAPE_HEIGHTFIELD  :I3TRACE( "Heightfield\n");	break;
			default :					I3TRACE( "Unknown\n");	break;
		}

		I3TRACE( "| group       : %d\n", pShape->group);
		I3TRACE( "| material    : %d\n", pShape->materialIndex);
		I3TRACE( "| density     : %f\n", pShape->density);
		I3TRACE( "| mass        : %f\n", pShape->mass);
		I3TRACE( "| skinWidth   : %f\n", pShape->skinWidth);
		I3TRACE( "| flags       : [");

		if( pShape->shapeFlags & NX_TRIGGER_ON_ENTER)
			I3TRACE( "Trigger-Enter ");

		if( pShape->shapeFlags & NX_TRIGGER_ON_LEAVE)
			I3TRACE( "Trigger-Leave ");

		if( pShape->shapeFlags & NX_TRIGGER_ON_STAY)
			I3TRACE( "Trigger-Stay ");

		if( pShape->shapeFlags & NX_SF_VISUALIZATION)
			I3TRACE( "Visualize ");

		if( pShape->shapeFlags & NX_SF_DISABLE_COLLISION)
			I3TRACE( "No-Coll ");

		if( pShape->shapeFlags & NX_SF_FEATURE_INDICES)
			I3TRACE( "Indices ");

		if( pShape->shapeFlags & NX_SF_DISABLE_RAYCASTING)
			I3TRACE( "No-Raycast ");

		if( pShape->shapeFlags & NX_SF_POINT_CONTACT_FORCE)
			I3TRACE( "Point-Contact ");

		if( pShape->shapeFlags & NX_SF_FLUID_DRAIN)
			I3TRACE( "Fluid-drain ");

		if( pShape->shapeFlags & NX_SF_FLUID_DISABLE_COLLISION)
			I3TRACE( "No-Coll-with-Fluids ");

		if( pShape->shapeFlags & NX_SF_DISABLE_RESPONSE)
			I3TRACE( "No-Rsps ");

		if( pShape->shapeFlags & NX_SF_DYNAMIC_DYNAMIC_CCD)
			I3TRACE( "Dynamic-CCD ");

		if( pShape->shapeFlags & NX_SF_DISABLE_SCENE_QUERIES)
			I3TRACE( "No-Scene-Query ");

		I3TRACE( "]\n");
	}
}

void i3PhysixShapeSet::Reset(void)
{
	NxVec3 zero( 0.0f, 0.0f, 0.0f);

	if( m_pNxActor == nullptr)
		return;

	if( m_pNxActor->isDynamic() && !m_pNxActor->readBodyFlag( NX_BF_KINEMATIC ) )
	{
		m_pNxActor->setLinearVelocity( zero);
		m_pNxActor->setAngularVelocity( zero);

		m_pNxActor->setLinearMomentum( zero);
		m_pNxActor->setAngularMomentum( zero);
	}
}

bool	i3PhysixShapeSet::addContactReport( const I3_PHYSIX_CONTACT_REPORT_PTR& spReport)
{
	if (m_bEnableContactReport == false) 
		return false;
	if ( m_vecReports.size() >= I3PHYSIX_CONTACT_REPORT_COUNT )
		return false;

	m_vecReports.push_back(spReport);
	return true;
}

void	i3PhysixShapeSet::clearContactReport( void )
{
	m_vecReports.clear();		// 스마트포인터이고, I3_PHYSIX_CONTACT_REPORT에 소멸자처리했으므로 이걸로 종료시킨다...
}

static REAL32 NO_ZERO( REAL32 val)
{
	if( i3Math::abs( val) < I3_EPS)
		val = I3_EPS;

	return val;
}

//static void	NO_ZERO( NxVec3 & vec)
//{
//	vec.x = NO_ZERO( vec.x);
//	vec.y = NO_ZERO( vec.y);
//	vec.z = NO_ZERO( vec.z);
//}


void i3PhysixShapeSet::_ScaleBox( NxBoxShape * pShape, const VEC3D & scale)
{
	NxVec3 oldDim = pShape->getDimensions();
	NxVec3 newDim, center;
	NxMat34 localPose = pShape->getLocalPose();
	VEC3D orgSize = * getOriginalSize();
	VEC3D rate;

	localPose.M.multiply( oldDim, newDim);
	localPose.multiply( NxVec3( 0.0f, 0.0f, 0.0f), center);

	// Dimension
	newDim.x = (newDim.x / orgSize.x) * scale.x;
	newDim.y = (newDim.y / orgSize.y) * scale.y;
	newDim.z = (newDim.z / orgSize.z) * scale.z;

	center.x = (center.x / orgSize.x) * scale.x;
	center.y = (center.y / orgSize.y) * scale.y;
	center.z = (center.z / orgSize.z) * scale.z;

	// 다시 LocalPose-Space로 변환.
	{
		NxMat34 invLocal;

		localPose.getInverse( invLocal);

		invLocal.M.multiply( newDim, newDim);
		//invLocal.multiply( center, center);
	}

	pShape->setDimensions( newDim);
	pShape->setLocalPosition( center);
}

void i3PhysixShapeSet::_ScaleConvex( NxConvexShape * pShape, const VEC3D & scale)
{
	NxConvexMesh & pConvex = pShape->getConvexMesh();

	NxU32 numVerts = pConvex.getCount( 0, NX_ARRAY_VERTICES);
	NxPoint * verts = (NxPoint*) pConvex.getBase( 0, NX_ARRAY_VERTICES);

	VEC3D * pNewPoints = (VEC3D*) i3MemAlloc( sizeof(VEC3D) * numVerts);
	VEC3D * pOrgSize = getOriginalSize();

	for( UINT32 i = 0; i < numVerts; i++)
	{
		REAL32 x = verts[ i ].x / pOrgSize->x;
		REAL32 y = verts[ i ].y / pOrgSize->y;
		REAL32 z = verts[ i ].z / pOrgSize->z;

		setX( &pNewPoints[ i], x * scale.x);
		setY( &pNewPoints[ i], y * scale.y);
		setZ( &pNewPoints[ i], z * scale.z);
	}

	NxConvexShapeDesc shapeDesc;

	pShape->saveToDesc( shapeDesc);

	shapeDesc.localPose = pShape->getLocalPose();

	NxVec3 localPos = pShape->getLocalPosition();
	NxVec3 originalPos( localPos.x / pOrgSize->x, localPos.y / pOrgSize->y, localPos.z / pOrgSize->z);
	localPos.x = originalPos.x * scale.x;
	localPos.y = originalPos.y * scale.y;
	localPos.z = originalPos.z * scale.z;
	shapeDesc.localPose.t = localPos;

	shapeDesc.userData = pShape->userData;
	{
		NxConvexMeshDesc meshDesc;
		meshDesc.setToDefault();
		
		meshDesc.points = pNewPoints;
		meshDesc.pointStrideBytes = sizeof(VEC3D);
		meshDesc.numVertices = numVerts;
		meshDesc.flags = NX_CF_COMPUTE_CONVEX;

		NXU::InitCooking();

		{
			NXU::MemoryWriteBuffer buf;

			if( NXU::CookConvexMesh( meshDesc, buf) == false)
			{
				// Convex의 Cooking이 불가능한 Mesh...
				// Error로 간주하지 않는다.
				I3ASSERT_0;
			}

			NxConvexMesh * pNxMesh = g_pPhysixCtx->getSDK()->createConvexMesh( NXU::MemoryReadBuffer(buf.data));
			if( pNxMesh == nullptr)
			{
				// Convex의 Cooking이 불가능한 Mesh...
				I3ASSERT_0;
			}
			else
			{
				shapeDesc.meshData = pNxMesh;
			}
			NXU::CloseCooking();
			NXU::releasePersistentMemory();
		}

		I3MEM_SAFE_FREE( pNewPoints);
	}

	NxShape * pNewShape = getNxActor()->createShape( shapeDesc);

	pNewShape->setGlobalPose( pShape->getGlobalPose());
}

void i3PhysixShapeSet::_ScaleCapsule( NxCapsuleShape * pShape, const VEC3D & scale)
{
	REAL32 height = NO_ZERO(scale.y);

	pShape->setHeight( height);
	
	REAL32 radius = NO_ZERO(scale.x * 0.5f);

	pShape->setRadius( radius);
}

void i3PhysixShapeSet::_ScaleMesh( NxTriangleMeshShape * pShape, const VEC3D & scale)
{
	NxTriangleMesh & triMesh = pShape->getTriangleMesh();
	NxU32 numVerts = triMesh.getCount(0, NX_ARRAY_VERTICES);
	NxPoint * pVerts = (NxPoint*) triMesh.getBase( 0, NX_ARRAY_VERTICES);

	REAL32 newScaleX = NO_ZERO( scale.x);
	REAL32 newScaleY = NO_ZERO( scale.y);
	REAL32 newScaleZ = NO_ZERO( scale.z);
	
	for( UINT32 i = 0; i < numVerts; i++)
	{
		pVerts[i].x *= newScaleX;
		pVerts[i].y *= newScaleY;
		pVerts[i].z *= newScaleZ;
	}
}

void i3PhysixShapeSet::_ScaleSphere( NxSphereShape * pShape, const VEC3D & scale)
{
	NxReal radius = pShape->getRadius();
	radius = NO_ZERO( radius * scale.x);
	pShape->setRadius( radius);

	VEC3D * pOrgSize = getOriginalSize();

	NxVec3 localPos = pShape->getLocalPosition();
	NxVec3 originalPos( localPos.x / pOrgSize->x, localPos.y / pOrgSize->y, localPos.z / pOrgSize->z);

	localPos.x = originalPos.x * scale.x;
	localPos.y = originalPos.y * scale.y;
	localPos.z = originalPos.z * scale.z;
	pShape->setLocalPosition( localPos);
}

void i3PhysixShapeSet::AdjustScale( const VEC3D & scale, const MATRIX * pWSTransform)
{
	i3::vector<NxShape*> releaseShapeList;
	MATRIX inv;
	VEC3D localScale;

	localScale = scale;

	INT32 nShapeCount = getNxShapeCount();
	for(INT32 i = 0; i < nShapeCount; i++)
	{
		NxShape * pShape = getNxShape( i);

		NxBoxShape * pBox = pShape->isBox();
		if( pBox != nullptr)
		{
			_ScaleBox( pBox, localScale);
			continue;
		}

		NxConvexShape * pConvexShape = pShape->isConvexMesh();
		if( pConvexShape != nullptr)
		{
			_ScaleConvex( pConvexShape, localScale);
			
			releaseShapeList.push_back( pShape);
			continue;
		}

		NxCapsuleShape * pCapsuleShape = pShape->isCapsule();
		if( pCapsuleShape != nullptr)
		{
			_ScaleCapsule( pCapsuleShape, localScale);
			continue;
		}

		NxTriangleMeshShape * pTriMeshShape = pShape->isTriangleMesh();
		if( pTriMeshShape != nullptr)
		{
			_ScaleMesh( pTriMeshShape, localScale);
			continue;
		}

		NxSphereShape * pSphereShape = pShape->isSphere();
		if( pSphereShape != nullptr)
		{
			_ScaleSphere( pSphereShape, localScale);
			continue;
		}
	}

	// 
	setOriginalSize( &localScale);

	if (getNxActor())
	{
		for (size_t i = 0; i < releaseShapeList.size(); i++)
		{
			NxShape * pShape = releaseShapeList[i];

			getNxActor()->releaseShape((NxShape&)(*pShape));
		}
	}
}

void i3PhysixShapeSet::GetWrappingBound( I3_GETBOUND_INFO * pInfo )
{
	_GetWrappingBound(pInfo);
}