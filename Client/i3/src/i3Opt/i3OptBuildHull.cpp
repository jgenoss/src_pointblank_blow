#include "i3OptPCH.h"
#include "i3OptBuildHull.h"
#include "i3OptGetAllVertices.h"

#include "NXU_helper.h"
#include "NXU_schema.h"
#include "NXU_cooking.h"
#include "NXU_streaming.h"

I3_CLASS_INSTANCE( i3OptBuildHull);

i3OptBuildHull::i3OptBuildHull(void)
{
	m_Class = CLASS_PHYSICS;
}

bool i3OptBuildHull::OnNode( i3Node * pNode, i3Node * pParent)
{
	if( m_bLocalHull)
	{
		_OnNodeLocal( pNode);
	}
	else
	{
		_OnNodeGlobal( pNode);
	}

	return true;
}

void i3OptBuildHull::_OnNodeGlobal( i3Node * pNode)
{
}

void i3OptBuildHull::_OnNodeLocal( i3Node * pNode)
{
}

void i3OptBuildHull::_BuildGlobalHull(void)
{
	// 전체 Geometry를 포함하는 Vertex들을 구한다.
	i3OptGetAllVertices opt;

	opt.Trace( getTraceRoot());

	i3PhysixShapeSet * pShapeSet = CreateConvex( opt.getVectorArray());
	I3ASSERT( pShapeSet != nullptr);

	// Root Node를 대체
	i3Transform2 * pNewRoot = i3Transform2::new_object_ref();
	pNewRoot->SetName( "AggHull");

	pNewRoot->setShapeSet( pShapeSet);

	pNewRoot->AddChild( getTraceRoot());

	SetTraceRoot( pNewRoot);
}

i3Transform * i3OptBuildHull::_FindReplaceTransform( i3Geometry * pGeo)
{
	i3Node * pParent = pGeo->GetParent();
	
	while( pParent != nullptr)
	{
		if( i3::kind_of<i3Transform*>(pParent))
		{
			if( pParent->IsFlag( I3_NODEFLAG_STATIC) == false)
				return (i3Transform *) pParent;
		}

		pParent = pParent->GetParent();
	}

	return nullptr;
}

i3PhysixShapeSet *	i3OptBuildHull::CreateConvex( i3Vector3Array * pVA)
{
	NxConvexMeshDesc desc;
	NxConvexMesh * pNxMesh;
	NxConvexShapeDesc shapeDesc;
	NxActorDesc			actorDesc;
	NxBodyDesc			bodyDesc;
	i3PhysixShapeSet *		pShapeSet;

	actorDesc.setToDefault();

	desc.numVertices = pVA->GetVectorCount();
	desc.points = pVA->GetVectorArray();
	desc.pointStrideBytes = sizeof(VEC3D);
	desc.flags = 0;
	desc.flags = NX_CF_COMPUTE_CONVEX;

	pShapeSet = i3PhysixShapeSet::new_object_ref();

	NXU::InitCooking();

	{
		NXU::MemoryWriteBuffer buf;

		if( NXU::CookConvexMesh( desc, buf) == false)
		{
			// Convex의 Cooking이 불가능한 Mesh...
			// Error로 간주하지 않는다.
			goto ExitPart;
		}

		pNxMesh = g_pPhysixCtx->getSDK()->createConvexMesh( NXU::MemoryReadBuffer(buf.data));
		if( pNxMesh == nullptr)
		{
			// Convex의 Cooking이 불가능한 Mesh...
			goto ExitPart;
		}

		// Shape Creation
		{
			shapeDesc.setToDefault();
			shapeDesc.group = (NxCollisionGroup) m_Group;
			shapeDesc.meshData = pNxMesh;
			shapeDesc.meshFlags = 0;

			NxMaterial * pMtl = g_pPhysixCtx->validateMaterial( m_Elasticity, m_sFriction, m_dFriction);
			shapeDesc.materialIndex = pMtl->getMaterialIndex();

			actorDesc.shapes.pushBack( &shapeDesc);
			
			if( m_bDynamic)
			{
				bodyDesc.setToDefault();
				bodyDesc.mass = m_Mass;
				bodyDesc.linearDamping = m_LinearDamping;
				bodyDesc.angularDamping = m_AngularDamping;

				if( m_Mass == 0.0f)
				{
					actorDesc.density = 1.0f;
				}
				else
				{
					actorDesc.density = 0.0f;
				}

				actorDesc.body = &bodyDesc;
			}
			else
			{
				actorDesc.density = 1.0f;
			}
			
			actorDesc.flags = NX_AF_LOCK_COM | NX_AF_FLUID_DISABLE_COLLISION;

			pShapeSet->Create( &actorDesc, true);
			pShapeSet->setRestitution( m_Elasticity);
			pShapeSet->setFriction( m_sFriction, m_dFriction);
			pShapeSet->setShapeGroup( (UINT16) m_Group);

			{
				NxActor * pActor = pShapeSet->getNxActor();

				pActor->updateMassFromShapes( 1.0f, 0.0f);

				I3TRACE( "%f \n", pActor->getMass());
			}
		}
	}

ExitPart:

	NXU::CloseCooking();
	NXU::releasePersistentMemory();

	return pShapeSet;
}

i3PhysixShapeSet *	i3OptBuildHull::CreateConvexHull( i3Node * pNode)
{
	i3OptGetAllVertices opt;

	// Root Node를 대체
	opt.Trace( pNode);
	return CreateConvex( opt.getVectorArray());
}

void i3OptBuildHull::_BuildLocalHull(void)
{
	i3::vector<i3Node*> list;
	i3Geometry * pGeo;
	i3Transform * pOldTrans;
	i3Transform2 * pNewTrans;
	i3PhysixShapeSet * pShapeSet;

	// 각 Geometry 들의 상위 Transform Node에 대체.
	i3Scene::FindNodeByType( getTraceRoot(), i3Geometry::static_meta(), list);

	for(size_t i = 0; i < list.size(); i++)
	{
		pGeo = (i3Geometry *) list[i];

		pNewTrans = i3Transform2::new_object_ref();

		pOldTrans = _FindReplaceTransform( pGeo);

		if( pOldTrans == nullptr)
		{
			i3OptGetAllVertices opt;

			// Root Node를 대체
			opt.Trace( getTraceRoot());
			pShapeSet = CreateConvex( opt.getVectorArray());

			pNewTrans->setShapeSet( pShapeSet);
			pNewTrans->AddChild( getTraceRoot());

			SetTraceRoot( pNewTrans);
		}
		else
		{
			i3Scene::MoveChildren( pNewTrans, pOldTrans);

			{
				i3OptGetAllVertices opt;

				opt.Trace( pNewTrans);
				pShapeSet = CreateConvex( opt.getVectorArray());

				pNewTrans->setShapeSet( pShapeSet);
			}

			pNewTrans->SetMatrix( pOldTrans->GetMatrix());

			if( pOldTrans->GetParent() != nullptr)
			{
				pOldTrans->GetParent()->AddChild( pNewTrans);
				pOldTrans->RemoveFromParent();
			}
			else
			{
				// Root의 대체
				SetTraceRoot( pNewTrans);
			}
		}
	}
}

void i3OptBuildHull::Trace( i3Node * pNode)
{
	SetMatrixStackEnable(true);

	if( m_bLocalHull)
	{
		_BuildLocalHull();
	}
	else
	{
		_BuildGlobalHull();
	}
}
