#include "i3SceneDef.h"
#include "i3Skeleton.h"
#include "i3SceneTracer.h"

#include "i3PhysixJointInfoD6.h"

#define I3_TEST_PHYSIX_FXTOFORCE

I3_CLASS_INSTANCE( i3Skeleton);


i3Skeleton::~i3Skeleton(void)
{
	INT32 i = 0;

#if defined (I3_PHYSX)
	if( m_ppShapeSet != nullptr )
	{
		for( i=0; i<getBoneCount(); i++)
		{
			I3_SAFE_RELEASE( m_ppShapeSet[i]);
		}
		I3MEM_SAFE_FREE( m_ppShapeSet);
	}

	I3MEM_SAFE_FREE( m_pInitializeForce );
#endif

	

	I3_SAFE_RELEASE( m_pBoneListAttr);
}

void i3Skeleton::setBoneMatrixListAttr( i3BoneMatrixListAttr * pAttr)
{
	I3_REF_CHANGE( m_pBoneListAttr, pAttr);
	UpdateBoneMatrixListAttrInfo();
}

void i3Skeleton::SetBoneCount( INT32 count)
{
	if( m_pBoneListAttr == nullptr)
	{
		m_pBoneListAttr = i3BoneMatrixListAttr::new_object();
	}

	m_pBoneListAttr->SetBoneCount( count);

	UpdateBoneMatrixListAttrInfo();
}

INT32 i3Skeleton::FindBoneByName( const char * pszName)
{
	if( pszName != nullptr)
	{
		i3::string str( pszName);
		i3::vector_map<i3::string, INT32>::iterator it = m_BoneMap.find(str);
		if( it != m_BoneMap.end())
			return it->second;
	}

	return -1;
}

void i3Skeleton::GetFullMatrix( INT32 idx, MATRIX * pMtx)
{
	i3Matrix::Identity( pMtx);

	while( idx != -1)
	{
		if( idx < m_pBoneListAttr->getBoneCount())
		{
			i3Bone * pBone = m_pBoneListAttr->getBone( idx);

			i3Matrix::Mul( pMtx, pMtx, pBone->getMatrix());

			idx = pBone->getParentIndex();
		}
		else
		{
			// Bone Count가 넘어간다. 다른 LOD의 경우 본의 갯수가 적을 수 있다.(i3BoneRef의 경우)
			break;
		}
	}
}

bool i3Skeleton::IsSame( i3Skeleton * pSkel)
{
	INT32 i;
	i3Bone * pBone1, * pBone2;

	// 두 개의 Skeleton이 같은 것인지를 확인한다.

	// 개수 확인
	if( getBoneCount() != pSkel->getBoneCount())
		return false;

	// 개별 Bone의 확인
	for( i = 0; i < getBoneCount(); i++)
	{
		pBone1 = getBone( i);
		pBone2 = pSkel->getBone( i);

		// 이름 확인 
		if( i3::generic_is_iequal( pBone1->getNameString(), pBone2->getNameString()) == false)			// 대소문자 비구분 이유가 없어보인다..
			return false;

		// Depth 확인
		if( pBone1->getDepth() != pBone2->getDepth())
			return false;

		// Matrix 값도 비교하는 것이 정확하나, 과다한 검사라 생각해 생략.
		// earring

	}

	return true;
}

void i3Skeleton::CreateJoints(void)
{
#if defined (I3_PHYSX)
	INT32 i;

	for( i = 0; i < getBoneCount(); i++)
	{
		if((m_ppShapeSet[i] != nullptr) && (m_ppShapeSet[i]->getJointInfo() != nullptr))
		{
			INT32 parentIdx = getParentBoneIndex( i);

			while((parentIdx != -1) && (m_ppShapeSet[ parentIdx] == nullptr))
			{
				parentIdx = getParentBoneIndex( parentIdx);
			}

			if( parentIdx != -1)
			{
				m_ppShapeSet[i]->CreateJoint( m_ppShapeSet[ parentIdx]);
				//I3TRACE( "JOINT %s- i %d parent %d-%x parent %s\n", m_ppShapeSet[i]->GetName(), i, parentIdx, m_ppShapeSet[ parentIdx], m_ppShapeSet[parentIdx]->GetName());
				
			}
			else
			{
				m_ppShapeSet[i]->CreateJoint( nullptr);
				//I3TRACE( "JOINT %s-nullptr\n", m_ppShapeSet[i]->GetName());
			}
		}
	}
#endif
}

#if defined (I3_PHYSX)
void i3Skeleton::setShapeSet( INT32 boneIdx, i3PhysixShapeSet * pShapeSet)
{
	I3ASSERT( boneIdx < getBoneCount());

	if( m_ppShapeSet == nullptr)
	{
		m_ppShapeSet = (i3PhysixShapeSet **) i3MemAlloc( sizeof(void *) * getBoneCount());
		I3ASSERT( m_ppShapeSet != nullptr);
		i3mem::FillZero( m_ppShapeSet, sizeof(void *) * getBoneCount());
	}

	if( m_pInitializeForce == nullptr )
	{
		m_pInitializeForce = (UINT8*) i3MemAlloc( sizeof(UINT8) * getBoneCount() );
		I3ASSERT( m_pInitializeForce != nullptr );
		i3mem::FillZero( m_pInitializeForce, sizeof(UINT8) * getBoneCount() );
	}

	I3_REF_CHANGE( m_ppShapeSet[ boneIdx], pShapeSet);
}

void i3Skeleton::setDynamicState( UINT32 state, i3PhysixShapeSet * pShapeSet )
{
	bool	bFlagChange = (state & I3_PHYSIX_STATE_KINEMATIC) != 0 ? false : true;

	m_dynamicState = state;

	if( m_ppShapeSet != nullptr )
	{
		INT32 i;

		for( i = 0; i < getBoneCount(); i++)
		{
			if( (m_ppShapeSet[i] != nullptr) && (m_ppShapeSet[i] == pShapeSet) )
			{	
				m_ppShapeSet[i]->setDynamicState( m_dynamicState);

				// Transform to Physix energy
				if( bFlagChange && !checkInitForceFeed( i ) )
				{
					_CalcTransformToPhysix( m_ppShapeSet[i], i );

					setInitForceFeed( i, 1 );
				}
				else
				{
					setInitForceFeed( i, 0 );
				}
			}
		}
	}

}

void i3Skeleton::setDynamicState( UINT32 state, bool bShapeSet )
{	
	bool	bFlagChange = (state & I3_PHYSIX_STATE_KINEMATIC) != 0 ? false : true;

	m_dynamicState = state;

	if( m_ppShapeSet != nullptr && bShapeSet )
	{
		INT32 i;

		for( i = 0; i < getBoneCount(); i++)
		{
			if( m_ppShapeSet[i] != nullptr)
			{	
				m_ppShapeSet[i]->setDynamicState( m_dynamicState);

				// Transform to Physix energy
				if( bFlagChange && !checkInitForceFeed( i ) )
				{
					_CalcTransformToPhysix( m_ppShapeSet[i], i );
					setInitForceFeed( i, 1 );
				}
				else
				{
					setInitForceFeed( i, 0 );
				}
			}
		}
	}
}

void	i3Skeleton::modifyDynamicState( UINT32 addState, UINT32 removeState)
{
	m_dynamicState |= addState;
	m_dynamicState &= ~removeState;

/*	if( oldState & removeState)
	{
		I3TRACE( "CHANGE DYNAMIC STATE : %p, %08X\n", this, m_dynamicState);
	}
	*/

	setDynamicState( m_dynamicState);
}

void i3Skeleton::_CalcTransformToPhysix( i3PhysixShapeSet * pShapeSet, INT32 idx )
{
	VEC3D	vLin, vAng;
	VEC3D vDef;
	
	if( pShapeSet == nullptr )
		return;

	if( pShapeSet->getNxActor() == nullptr )
		return;

	REAL32	rForce = g_pPhysixCtx->getFxToForce() ;

#if defined( I3_TEST_PHYSIX_FXTOFORCE )
	rForce *= pShapeSet->getFxForce();

	// FX Animation의 회전량 및 이동량을 계산합니다.
	{
		NxVec3 vLinVeclocity = pShapeSet->getNxActor()->getLinearVelocity();
		i3Vector::Set( &vLin, vLinVeclocity.x, vLinVeclocity.y, vLinVeclocity.z );
		
		REAL32 rMax = 0.f;
		if( i3Vector::Length( &vLin) > I3_EPS)
		{
			rMax = i3Vector::Normalize( &vLin, &vLin );
			if( rMax > g_pPhysixCtx->getFxMax() )
				rMax = g_pPhysixCtx->getFxMax();
			i3Vector::Scale( &vLin, &vLin, rForce * rMax);
		}
	}

	pShapeSet->setLinearVelocity( &vDef );
	i3Vector::Add( &vLin, &vLin, pShapeSet->getFBLinearVelocity() );
	pShapeSet->setFBLinearVelocity( &vDef );

	{
		NxVec3 vAngVelocity = pShapeSet->getNxActor()->getAngularVelocity();
		i3Vector::Set( &vAng, vAngVelocity.x, vAngVelocity.y, vAngVelocity.z );

		REAL32 rMax = 0.f;
		if( i3Vector::Length( &vAng) > I3_EPS)
		{
			rMax = i3Vector::Normalize( &vAng, &vAng);
			if( rMax > g_pPhysixCtx->getFxMax() )
				rMax = g_pPhysixCtx->getFxMax();
			i3Vector::Scale( &vAng, &vAng, rForce * rMax);
		}
	}

	pShapeSet->setAngularVelocity( &vDef );
	i3Vector::Add( &vAng, &vAng, pShapeSet->getFBAngularVelocity() );
	pShapeSet->setFBAngularVelocity( &vDef );
#else
	i3Vector::Copy( &vLin, pShapeSet->getFBLinearVelocity() );
	i3Vector::Copy( &vAng, pShapeSet->getFBLinearVelocity() );
#endif
	
	pShapeSet->setLinearVelocity( &vLin );
	pShapeSet->setAngularVelocity( &vAng );
}
#endif

void i3Skeleton::Traverse( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap)
{
#if defined (I3_PHYSX)
	INT32 idx;
	INT32 i, prevDepth = -1;
	i3Bone * pBone;

	i3TransformSourceCombiner * pCombiner;
	MATRIX * pIn, tmpOut;

	I3ASSERT( pCtx != nullptr);

	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	i3Skeleton * pFrameSkel = pCtx->getFrameSkeleton();

	bool bDynamic = pFrameSkel->isDynamic();


	pStack->SetBookmark();

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone( i);

		pStack->Pop( (prevDepth - ((INT32)pBone->getDepth() - 1)) );

		if( pBoneMap != nullptr)
			idx = pBoneMap[ i];
		else
			idx = i;

		pIn = pBone->getMatrix();

		if( idx != -1)
		{
			if( bDynamic == false)
			{
				// Animation에서 Transform을 가져온다.
				if( pCtx->getCombinerCount() > 0)
				{
					pCombiner = pCtx->getCombiner( idx);

					if( pCombiner != nullptr)
					{
						if( pCombiner->getAnimationCount() > 0)
						{
							I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

							pEnv->m_Time = pTracer->GetTime();
							pEnv->m_pIn = pIn;
							pEnv->m_pOut = &tmpOut;
							pEnv->m_pModelViewMatrix = pStack->GetTop();

							pCombiner->GetMatrix( pEnv);

#if defined( I3_DEBUG)
							if( !i3Math::isValid( i3Matrix::Get22( &tmpOut)) )
							{
								char szMsg[256] = "";
								i3::snprintf( szMsg, 256, "Invalid value of matrix in Skeleton Bone (%s)\r\nPlease check the i3Animation file", pBone->getNameString());
								I3PRINTLOG(I3LOG_FATAL,  szMsg);
							}
							else
#endif
							{
								pIn = &tmpOut;
							}							
						}
					}

					pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);
				}

				if( pFrameSkel->isShapeSet( idx))
				{	
					pFrameSkel->getShapeSet( idx )->setShapeGlobalPose( pStack->GetTop());
				}
			}
			else
			{
				//
				if( pFrameSkel->isShapeSet( idx))
				{
					// Physix에서 Transform을 가져온다.
					pFrameSkel->getShapeSet( idx)->getShapeGlobalPose( pCtx->getCacheMatrix( idx));

					pStack->Push( pCtx->getCacheMatrix( idx), nullptr);		// Stack에 단순 Push
				}
				else
				{
					// 기존 Transform을 사용한다.
					pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);
				}
			}

		}

		prevDepth = pBone->getDepth();
	}

	pStack->RestoreBookmark();
#endif
}

void i3Skeleton::UpdateAnim( UINT32 idFrame, REAL32 tm, i3AnimationContext * pCtx, INT32 * pBoneMap)
{
	INT32 idx;
	INT32 i;
	i3Bone * pBone;
	i3TransformSourceCombiner * pCombiner;
	I3MATRIXINFO  Env;

	I3ASSERT( pCtx != nullptr);

#if defined (I3_PHYSX)
	// Dynamic Physics 모드라면 아무 것도 하지 않아야 한다.
	//if( pCtx->getFrameSkeleton()->isDynamic() )
	//	return;

	Env.m_FrameID = idFrame;
	Env.m_pModelViewMatrix = &I3_IDENTITYMATRIX;
	Env.m_KeyChannel = 0;

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone( i);

		if( pBoneMap != nullptr)
			idx = pBoneMap[ i];
		else
			idx = i;

		if((idx != -1) && (pCtx->getCombinerCount() > 0))
		{
			pCombiner = pCtx->getCombiner( idx);

			if( pCombiner->getAnimationCount() > 0)
			{
				Env.m_Time	= tm;
				Env.m_pIn = pBone->getMatrix();
				Env.m_pOut = pCtx->getCacheMatrix( idx);

				pCombiner->GetMatrix( &Env);
			}
			else
			{
				i3Matrix::Copy( pCtx->getCacheMatrix( idx), pBone->getMatrix());
			}
		}
		else
		{
			i3Matrix::Copy( pCtx->getCacheMatrix( idx), pBone->getMatrix());
		}
	}
#endif
}

void i3Skeleton::TransformWorldSpace( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap)
{
	INT32 idx;
	INT32 i, prevDepth = -1;
	i3Bone * pBone;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	i3Skeleton * pFrameSkel = pCtx->getFrameSkeleton();
	bool bDynamic;

#if defined (I3_PHYSX)
	bDynamic = pFrameSkel->isDynamic();
#else 
	bDynamic = false;
#endif

	I3ASSERT( pCtx != nullptr);

	pStack->SetBookmark();

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone( i);

		pStack->Pop( (prevDepth - ((INT32)pBone->getDepth() - 1)));

		if( pBoneMap != nullptr)
			idx = pBoneMap[ i];
		else
			idx = i;

		if( bDynamic == false)
		{	
			pStack->PushPre( pCtx->getCacheMatrix( idx), pCtx->getCacheMatrix( idx));
#if defined (I3_PHYSX)
			if( pFrameSkel->isShapeSet( idx))
			{	
				pFrameSkel->getShapeSet( idx )->setShapeGlobalPose( pStack->GetTop());
			}
#endif
		}
#if defined( I3_PHYSX)
		else
		{
			if( pFrameSkel->isShapeSet( idx))
			{	
				pFrameSkel->getShapeSet( idx)->getShapeGlobalPose( pCtx->getCacheMatrix( idx));

				pStack->Push( pCtx->getCacheMatrix( idx), nullptr);
			}
			else
			{
				pStack->PushPre( pCtx->getCacheMatrix( idx), pBone->getMatrix());
			}
		}
#endif

		prevDepth = pBone->getDepth();
	}

	pStack->RestoreBookmark();
}

void i3Skeleton::BuildTransformedMatrix( i3SceneTracer * pTracer, i3AnimationContext * pAnimCtx, INT32 * pBoneMap)
{
	I3ASSERT( m_pBoneListAttr != nullptr);
	m_pBoneListAttr->BuildTransformedMatrix( pTracer, pAnimCtx, pBoneMap);
}

void i3Skeleton::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	if( !i3::same_of<i3Skeleton*>(pDest))
		return;

	i3Skeleton * pSkel = (i3Skeleton *) pDest;

	if( m_pBoneListAttr != nullptr)
	{
		if( method == I3_COPY_REF)
		{
			pSkel->setBoneMatrixListAttr( m_pBoneListAttr);
		}
		else
		{
			i3BoneMatrixListAttr * pNewBone = i3BoneMatrixListAttr::new_object_ref();

			m_pBoneListAttr->CopyTo( pNewBone, method);

			pSkel->setBoneMatrixListAttr( pNewBone);
		}

		#if defined (I3_PHYSX)
		if( m_ppShapeSet != nullptr)
		{
			INT32 i;
			i3PhysixShapeSet * pDestShapeSet;

			for( i = 0; i < getBoneCount(); i++)
			{
				if( m_ppShapeSet[i] != nullptr)
				{
					pDestShapeSet = i3PhysixShapeSet::new_object_ref();

					m_ppShapeSet[i]->CopyTo( pDestShapeSet, I3_COPY_INSTANCE);

					pSkel->setShapeSet( i, pDestShapeSet);

					//I3TRACE( "Bone Shape(%d) Name : %s\n", i, pDest->GetName() );
				}
			}
		}
		#endif
	}

	pSkel->setDynamicState( m_dynamicState);
}

void i3Skeleton::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pBoneListAttr != nullptr)
	{
		m_pBoneListAttr->OnBuildObjectList( List);

		#if defined (I3_PHYSX)
			if( m_ppShapeSet != nullptr)
			{
				INT32 i;

				for( i = 0; i < getBoneCount(); i++)
				{
					if( m_ppShapeSet[i] != nullptr)
						m_ppShapeSet[i]->OnBuildObjectList( List);
				}
			}
		#endif
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct SKELETON
	{
		UINT8			m_ID[4] = { 'S', 'K', 'T', '1' };
		UINT32			m_dynamicState = 0;
		OBJREF			m_pBoneListAttr = 0;
		UINT32			m_ShapeSetCount = 0;
		UINT32			m_SkelFlag = 0;
		UINT32			pad[7] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Skeleton::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SKELETON data;

	data.m_dynamicState		= m_dynamicState;
	data.m_pBoneListAttr	= (OBJREF) pResFile->GetObjectPersistID( m_pBoneListAttr);

#if defined (I3_PHYSX)
	if( m_ppShapeSet != nullptr)
		data.m_ShapeSetCount = m_pBoneListAttr->getBoneCount();
#endif

	data.m_SkelFlag = m_nSkelFlag;

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

#if defined (I3_PHYSX)
	if( m_ppShapeSet != nullptr)
	{
		// ShapeSet의 Ref List.
		INT32 i;

		for( i = 0; i < getBoneCount(); i++)
		{
			OBJREF ref;
			
			ref = pResFile->GetObjectPersistID( m_ppShapeSet[i]);

			Rc = pStream->Write( &ref, sizeof(OBJREF));
			I3_CHKIO( Rc);
			Result += Rc;
		}
	}
#endif

	return Result;
}

UINT32 i3Skeleton::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::SKELETON data;
	i3BoneMatrixListAttr * pAttr;	

	i3mem::FillZero( &data, sizeof( data));

	Rc = pResFile->Read( data.m_ID, 4);
	I3_CHKIO( Rc);
	Result += Rc;

	if( memcmp( data.m_ID, "SKT1", 4) == 0)
	{
		// New version
		Rc = pResFile->Read( &data.m_dynamicState, sizeof(data) - 4);
		I3_CHKIO( Rc);
		Result += Rc;

		if( data.m_pBoneListAttr != 0)
		{
			pAttr = (i3BoneMatrixListAttr *) pResFile->FindObjectByID( data.m_pBoneListAttr);
			I3ASSERT( pAttr != nullptr);

			setBoneMatrixListAttr( pAttr);
		}

		// ShapeSet Array
#if defined (I3_PHYSX)
		INT32 i;
		if( data.m_ShapeSetCount > 0)
		{
			I3MEM_SAFE_FREE( m_ppShapeSet);

			setShapeSet( 0, nullptr);			// <- Memory 할당용.

			Rc = pResFile->Read( m_ppShapeSet, sizeof(void *) * getBoneCount());
			I3_CHKIO( Rc);

			for( i = 0; i < getBoneCount(); i++)
			{
				if( m_ppShapeSet[i] != nullptr)
				{
					i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pResFile->FindObjectByID( (OBJREF) m_ppShapeSet[i]);
					m_ppShapeSet[i] = pShapeSet;
					I3_MUST_ADDREF( pShapeSet);
				}
			}
			
		}
#endif
	}
	else
	{
		OBJREF ref = *((OBJREF *) data.m_ID);

		if( ref != 0)
		{
			pAttr = (i3BoneMatrixListAttr *) pResFile->FindObjectByID( ref);
			I3ASSERT( pAttr != nullptr);

			setBoneMatrixListAttr( pAttr);
		}
	}

	return Result;
}

void i3Skeleton::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
#if defined (I3_PHYSX)
	INT32 i;

	if( m_ppShapeSet == nullptr)
		return;

	for( i = 0; i < getBoneCount(); i++)
	{
		if( m_ppShapeSet[i] != nullptr)
		{
			i3PhysixShapeSet * pShapeSet = getShapeSet( i);
			I3ASSERT( pShapeSet != nullptr);

			pShapeSet->OnInstancing( pInfo);
		}
	}

	// Joint를 생성해준다.
	CreateJoints();
	
	setDynamicState( m_dynamicState); //| I3_PHYSIX_STATE_KINEMATIC );
#endif
}

void i3Skeleton::UpdateBoneMatrixListAttrInfo()
{
	m_BoneMap.clear();

	if(m_pBoneListAttr)
	{
		for( INT32 i = 0; i < m_pBoneListAttr->getBoneCount(); i++)
		{
			i3::string str(m_pBoneListAttr->getBone(i)->getName());
			m_BoneMap.insert( i3::vector_map<i3::string, INT32>::value_type(str, i) );
		}
	}
}


#if defined( I3_DEBUG)
void i3Skeleton::Dump(void)
{
	INT32 i, j;
	i3Bone * pBone;

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone( i);

		for( j = 0; j < (INT32) pBone->getDepth(); j++)
		{
			I3TRACE( "\t");
		}

		I3TRACE( "%s\n", pBone->getName());
	}
}

#endif