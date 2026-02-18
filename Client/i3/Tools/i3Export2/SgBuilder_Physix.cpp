#include "stdafx.h"
#include "SgBuilder.h"
#include "Util.h"
#include "i3Export.h"
#include "i3Base/pool/create_by_common_pool.h"
#include "i3Base/string/compare/generic_compare.h"

i3MemoryBuffer *  _BuildTerrainArray( i3tool::exporter::TTerrainList * pList)
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

void SgBuilder::_OnJobPhysix( JOB_INFO * pJob)
{
	int PhysicsType, GeometryType;
	i3PhysixShapeSet * pShapeSet;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxShapeDesc * pShapeDesc = NULL;
	REAL32 mass = 0.0f, density = 0.0f, restitution = 0.0f, dfriction = 0.f, sfriction = 0.f;
	REAL32 initialVelocityX = 0.f, initialVelocityY = 0.f, initialVelocityZ = 0.f;
	REAL32 initialSpinX = 0.f, initialSpinY = 0.f, initialSpinZ = 0.f;
	char proxyName[256];
	INode * pNode = pJob->m_pINode;
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

	//NxMaterial * pMtl = g_pPhysixCtx->validateMaterial( restitution, sfriction, dfriction);

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

	I3TRACE( "PHYSIX : %s - proxy:%s\n", pNode->GetName(), proxyName);

	if( proxyName[0] != 0)
	{
		i3::vector<i3tool::exporter::SHAPEDESC_INFO*> shapeList;
		bool bAdd, bBreak = false;
		
		i3::vector<JOB_INFO*>	list;
		// Proxy가 있는 경우, 동일 Proxy를 사용하는 모든 Node들을 모아
		// 하나의 Actor로 생성해준다.
		SearchJobByPhysixProxy( proxyName, list);

		{
			INode * pProxyNode = FindNodeByName( proxyName);
			
			if( pProxyNode != NULL)
			{
				matBase = __NODE_TM( pProxyNode, m_pExport->getStartTime());

				// 각 Convex들의 Local Post를 적절히 구하기 위해, Proxy Node의 Inverse World Transform이 필요
				matBase.Invert();
			}
			else
			{
				char conv[256];

				sprintf( conv, "Proxy 이름이 이상합니다.(%s)", proxyName);
				m_pExport->Log( pNode, conv);
				return;
			}
		}

		for(size_t i = 0; (i < list.size()) && ( bBreak == false); i++)
		{
			bAdd = true;

			JOB_INFO * pTempJob = list[i];

			I3TRACE( "GATHER JOB : %s\n", pTempJob->m_pINode->GetName());

			i3tool::exporter::SHAPEDESC_INFO * pShapeInfo = m_pExport->_CreateShapeDesc( pTempJob->m_pINode, pShapeSet, &matBase, GeometryType, mass, density);

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
					pTempJob->m_bProcessed = true;
				}
				else
				{
				//	delete pShapeInfo->m_pTerrainList;
					i3::delete_by_common_pool( pShapeInfo->m_pTerrainList );
				//
					delete pShapeInfo->m_pShapeDesc;
					delete pShapeInfo;
					pTempJob->m_bProcessed = false;
				}
			}
			else
			{
				pTempJob->m_bProcessed = false;
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

				for( UINT32 j = 0; j < pShapeInfo->m_pTerrainList->GetCount(); j++)
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
			matBase = __NODE_TM( pNode, m_pExport->getStartTime());

			// 각 Convex들의 Local Post를 적절히 구하기 위해, Proxy Node의 Inverse World Transform이 필요
			matBase.Invert();
		}

		i3tool::exporter::SHAPEDESC_INFO * pShapeInfo = m_pExport->_CreateShapeDesc( pNode, pShapeSet, &matBase, GeometryType, mass, density);

		pShapeDesc = pShapeInfo->m_pShapeDesc;

		if( pShapeDesc == NULL )
		{
			pJob->m_bProcessed = false;
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

		// Material
		pShapeDesc->materialIndex = I3_TERRAIN_CONCRETE; //pMtl->getMaterialIndex();

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
		Matrix3 mtx = pNode->GetNodeTM( m_pExport->getStartTime());
		MATRIX i3Mtx;

		if( m_pExport->m_Option.m_bRotateAxis)
			_i3Space( &mtx);

		::Convert( &i3Mtx, &mtx);
		
		NXU::Copy( &actorDesc.globalPose, &i3Mtx);
	}

	pShapeSet->setRestitution( restitution);
	pShapeSet->setFriction( sfriction, dfriction);

	pShapeSet->Create( &actorDesc, true );

	// Convex의 경우, Node의 이름으로 Terrain을 설정해 주어야한다.

	// 최종적으로 i3PhysixShapeSet을 Attach 시킬 위치를 찾는다.
	{
		if( proxyName[0] != 0)
		{
			// Proxy Geometry가 존재한다.
			i3Node * pAttachNode;

			// 실제로 Physix가 설정되어야 할 Bone을 검사한다.
			INT32 idxBone = findBoneByName( proxyName);
			if( idxBone == -1)
			{
				m_pExport->Log( pNode, "Physics 설정에 필요한 Node가 Hide 상태에 있거나, 존재하지 않습니다.");
				return;
			}
			else
			{
				BONE_INFO * pBoneInfo = getBone( idxBone);

				pAttachNode = pBoneInfo->m_pNode;

				if( i3::kind_of<i3Body*>(pAttachNode)) //->IsTypeOf( i3Body::static_meta()))
				{
					// Skeleton에 Attach되는 경우.
					i3Body * pBody = (i3Body *) pAttachNode;
					i3LOD * pLOD = pBody->getLOD( 0);

					if( pLOD == NULL)
					{
						m_pExport->Log( pNode, "i3Body에 올바른 LOD가 설정되어 있지 않아 Physics 설정을 Export할 수 없습니다.");
						RemoveActorDesc(actorDesc);
						I3_SAFE_RELEASE( pShapeSet);
						return;
					}

					INT32 boneIdx = -1;
					i3Skeleton * pSkel = pLOD->getSkeleton();

					// 해당 Bone 및 Parent Bone을 찾는다.
					if( pSkel != NULL)
					{
						boneIdx =  pSkel->FindBoneByName( proxyName);
						if( boneIdx != -1)
						{
							// 현재의 i3PhysixShapeSet을 등록해준다.
							pSkel->setShapeSet( boneIdx, pShapeSet);

							pJob->m_idxBone = boneIdx;			// Joint 처리를 위해 저장.
							pJob->m_pPhysixNode = pAttachNode;	// Joint 처리를 위해 저장.
						}
						else
						{
							m_pExport->Log( pNode, "Physics 설정에 필요한 Node가 Hide 상태에 있거나, 존재하지 않습니다.");
							RemoveActorDesc(actorDesc);
							I3_SAFE_RELEASE( pShapeSet);
							return;
						}
					}
					else
					{
						m_pExport->Log( pNode, "i3Body Node의 Skeleton이 존재하지 않아 Physics 설정을 진행할 수 없습니다.");
						RemoveActorDesc(actorDesc);
						I3_SAFE_RELEASE( pShapeSet);
						return;
					}
				}
				else
				{
					// 일반적인 Node에 Attach되는 경우.
					// 이 경우, 반드시 i3Transform2 Node이어야 한다.
					i3Node * pNode;
					i3::string sNodeName;

					pNode = m_pExport->_ReplaceToPhysixNode( pAttachNode, pShapeSet, PhysicsType, sNodeName);
					I3ASSERT( pNode != NULL);
					
//  					if (sNodeName.size() > 0)
// 					{
// 						i3Node* pShapeContainer = NULL;
// 						i3::vector<i3Node*> vecNode = pNode->GetChildList();
// 						for (INT32 i = 0; i < vecNode.size(); i++)
// 						{
// 							if (i3::generic_compare(vecNode[i]->GetName(), sNodeName) == 0)
// 							{
// 								pShapeContainer = vecNode[i];
// 								break;
// 							}
// 						}
// 
// 						if (pShapeContainer)
// 						{
// 							for (size_t i = 0; i < actorDesc.shapes.size(); i++)
// 							{
// 								i3Transform* pTrans = i3Transform::new_object();
// 								pTrans->SetName(actorDesc.shapes[i]->name);
// 
// 								pShapeContainer->AddChild(pTrans);
// 							}
// 						}
// 					}

					// i3Transform2나 i3ShapeSetContainer와 같은 새로운 Node로 대체되었을 수 있다.
					pBoneInfo->m_pNode = pNode;
					
					pJob->m_pPhysixNode = pNode;			// Joint 처리를 위해 저장.
				}
			}
		}
		else
		{
			INT32 idx = findBoneByINode( pNode);

			if( idx == -1)
			{
				char conv[256];
				sprintf( conv, "Physics 설정이 된 Node(%s)에 대해 Bone 정보가 존재하지 않습니다. Export 버그입니다.", pNode->GetName());
				m_pExport->Log( pNode, conv);
				I3_SAFE_RELEASE( pShapeSet);
				return;
			}

			BONE_INFO * pBoneInfo = getBone( idx);

			i3::string sNodeName;
			i3Node * pNode = m_pExport->_ReplaceToPhysixNode( pBoneInfo->m_pNode, pShapeSet, PhysicsType, sNodeName);

			// i3Transform2나 i3ShapeSetContainer와 같은 새로운 Node로 대체되었을 수 있다.
			pBoneInfo->m_pNode = pNode;

			pJob->m_pPhysixNode = pNode;			// Joint 처리를 위해 저장.
		}

		// 이 후, Joint Process를 위해 넣어둔다.
		pJob->m_pShapeSet = pShapeSet;
	}

	RemoveActorDesc(actorDesc);
	I3_SAFE_RELEASE( pShapeSet);
}

void SgBuilder::RemoveActorDesc(NxActorDesc& desc)
{
	for (size_t i = 0; i < desc.shapes.size(); i++)
	{
		NxShapeDesc * pShapeDesc = desc.shapes[i];

		if (pShapeDesc->getType() == NX_SHAPE_MESH)
		{
			// Triangle-Mesh의 경우에는 이미 완전히 설정된 Terrain Array를 Baking하는 단계에서
			// 
		}
		else
		{
		}

		delete desc.shapes[i];
	}
}

void SgBuilder::_OnJobJoint( JOB_INFO * pJob)
{
	// Joint 관련 설정
	TimeValue time = m_pExport->getStartTime();
	INT32 jointType = 1;
	TSTR conv;
	VEC3D axis, norm;
	REAL32	twistLow, twistHigh, swing1, swing2;
	BOOL	bLimits;
	i3PhysixJointInfoD6 * pJointInfo;
	NxD6JointMotion		motion;
	INode * pNode = pJob->m_pINode;
	i3PhysixShapeSet * pShapeSet = pJob->m_pShapeSet;
	i3PhysixShapeSet * pParentShapeSet = NULL;

	if( pJob->m_pPhysixNode == NULL)
		return;

	if( pJob->m_pShapeSet == NULL)
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
			}
			break;

		default :
			pJointInfo->Release();
			return;		// Joint가 필요하지 않다.
	}

	// Parent Actor
	if( i3::kind_of<i3Body*>(pJob->m_pPhysixNode)) //->IsTypeOf( i3Body::static_meta()))
	{
		i3Body * pBody = (i3Body *) pJob->m_pPhysixNode;
		i3LOD * pLOD = pBody->getLOD( 0);

		if( pLOD == NULL)
		{
			m_pExport->Log( pNode, "Joint 설정을 시도했으나, i3Body에 LOD가 존재하지 않아 중지합니다.");
			pJointInfo->Release();
			return;
		}

		i3Skeleton * pSkel = pLOD->getSkeleton();
		INT32 idxParent;

		idxParent = pSkel->getParentBoneIndex( pJob->m_idxBone);

		if( idxParent != -1)
		{
			pParentShapeSet = pSkel->getShapeSet( idxParent);
		}
	}
	else
	{
		i3Node * pParent = pJob->m_pPhysixNode->GetParent();

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
		MATRIX localMtx;

		ExtractTransform( pJob->m_pINode, &localMtx, m_pExport->getStartTime());

		{
			Matrix3 tmpMtx, inv;
			Point3 tmp, worldAxis, worldNorm, localAxis, localNorm;

			// Anchors
			VEC3D * pPos = i3Matrix::GetPos( &localMtx);
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

