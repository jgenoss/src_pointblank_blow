#include "i3SceneDef.h"
#include "i3Skeleton2.h"
#include "i3SceneTracer.h"
#include "i3PhysixJointInfoD6.h"
#include "Animation/i3AnimationContext2.h"

//#include "i3LuaInc.h"
//#include "i3SceneUtilScript.h"

I3_CLASS_INSTANCE( i3Skeleton2, i3Skeleton);

i3Skeleton2::i3Skeleton2()
{	
	m_nSkelFlag				= 0;
	m_pIKContextList		= NULL;

	m_pTxMotionBoneArray	= NULL;
}

i3Skeleton2::~i3Skeleton2()
{
	INT32 i;

	// IK animation
	if( m_pIKContextList != NULL)
	{	
		for( i = 0; i < m_pIKContextList->GetCount(); i++)
		{
			i3IKContext * pIKContext = getIKContext( i);
			I3_SAFE_RELEASE( pIKContext);
		}
		I3_SAFE_RELEASE( m_pIKContextList);
	}

	I3_SAFE_RELEASE( m_pTxMotionBoneArray);
}



void i3Skeleton2::SetBoneCount( INT32 count)
{
	i3Skeleton::SetBoneCount( count);
	
	if( m_pTxMotionBoneArray == NULL)
	{
		m_pTxMotionBoneArray = i3MatrixArray::NewObject();
	}

	m_pTxMotionBoneArray->Create( count);
}

bool i3Skeleton2::IsSame( i3Skeleton * pSkel)
{
	INT32 i;
	i3Bone2 * pBone1, * pBone2;

	// 두 개의 Skeleton이 같은 것인지를 확인한다.

	if( pSkel->IsExactTypeOf( i3Skeleton2::GetClassMeta()) == FALSE)
		return false;

	// 개수 확인
	if( getBoneCount() != ((i3Skeleton2*)pSkel)->getBoneCount())
		return false;

	// 개별 Bone의 확인
	for( i = 0; i < getBoneCount(); i++)
	{
		pBone1 = getBone2( i);
		pBone2 = ((i3Skeleton2*)pSkel)->getBone2( i);

		// 이름 확인 
		if( i3String::Compare( pBone1->getName(), pBone2->getName()) != 0)
			return false;

		// Depth 확인
		if( pBone1->getDepth() != pBone2->getDepth())
			return false;

		// Matrix 값도 비교하는 것이 정확하나, 과다한 검사라 생각해 생략.
		// earring

	}

	return true;
}

void i3Skeleton2::Traverse( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap)
{	
	INT32 idx;
	INT32 i, prevDepth = -1;
	i3Bone2 * pBone;

	i3TransformSourceCombiner * pCombiner;

	I3ASSERT( pCtx->IsExactTypeOf( i3AnimationContext2::GetClassMeta()) );

	if( m_pTraverse != NULL)
	{	
		m_pTraverse( pTracer, (i3AnimationContext2*)pCtx, this, pBoneMap);
		return;
	}

	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	MATRIX * pIn, tmpOut;

	I3ASSERT( pCtx != NULL);

	i3Skeleton2 * pFrameSkel = (i3Skeleton2*) ((i3AnimationContext2*)pCtx)->getFrameSkeleton();

	bool bDynamic = pFrameSkel->isDynamic();

	I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

	pStack->SetBookmark();

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone2( i);

		INT32 iPopCount = (prevDepth - ((INT32)pBone->getDepth() - 1));
		pStack->Pop( iPopCount);

		if( pBoneMap != NULL)	idx = pBoneMap[ i];
		else					idx = i;

		if( idx != -1)
		{
			pIn = pBone->getMatrix();

			if( bDynamic == false)
			{	
				//if( (getSkelFlag() & I3_SKELFLAG_USE_MOTION) && !(pBone->getFlag() & I3_BONEFLAG_USE_FKANIM) )
				//{	// Motion data를 사용하는 경우
				//	if( pBone->getFlag() & I3_BONEFLAG_USE_MOTIONDATA)
				//		pIn = m_pTxMotionBoneArray->GetMatrix( idx);

				//	pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);
				//}
				//else
				{
					// Animation에서 Transform을 가져온다.
					if( pCtx->getCombinerCount() > 0)
					{
						pCombiner = pCtx->getCombiner( idx);

						if( pCombiner->getAnimationCount() > 0)
						{	
							pEnv->m_Time = pTracer->GetTime();
							pEnv->m_pIn = pIn;
							pEnv->m_pOut = &tmpOut;
							pEnv->m_pModelViewMatrix = pStack->GetTop();

							pCombiner->GetMatrix( pEnv);

							if( (m_nSkelFlag & I3_SKELFLAG_NO_TRANSLATION) && 
								(i3String::Compare( pBone->getName(), "Root") != 0) )
							{
								i3Matrix::SetPos( &tmpOut, i3Matrix::GetPos( pIn));
							}

							pIn = &tmpOut;
						}
					}

					pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);
				}
				

#if defined (I3_PHYSX)
				if( pFrameSkel->isShapeSet( idx))
				{
					if( pFrameSkel->getShapeSet( idx)->isDynamicState( I3_PHYSIX_STATE_KINEMATIC) )
						pFrameSkel->getShapeSet( idx )->setShapeGlobalPose( pStack->GetTop());
					else
					{
						pFrameSkel->getShapeSet( idx)->getShapeGlobalPose( pCtx->getCacheMatrix( idx));
						pStack->Pop();
						pStack->Push( pCtx->getCacheMatrix( idx), NULL);		// Stack에 단순 Push
					}
				}
#endif
			}
			else
			{
#if defined( I3_PHYSX)
				if( pFrameSkel->isShapeSet( idx))
				{
					// Physix에서 Transform을 가져온다.
					pFrameSkel->getShapeSet( idx)->getShapeGlobalPose( pCtx->getCacheMatrix( idx));

					pStack->Push( pCtx->getCacheMatrix( idx), NULL);		// Stack에 단순 Push
				}
				else
#endif
				{
					// 기존 Transform을 사용한다.
					pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);
				}
			}
		}

		prevDepth = pBone->getDepth();
	}

	pStack->RestoreBookmark();
}

void i3Skeleton2::UpdateAnim( UINT32 idFrame, REAL32 tm, i3AnimationContext * pCtx, INT32 * pBoneMap)
{
	INT32 idx;
	INT32 i;
	i3Bone2 * pBone;
	i3TransformSourceCombiner * pCombiner;
	I3MATRIXINFO  Env;

	I3ASSERT( pCtx != NULL);

	I3ASSERT( pCtx->IsExactTypeOf( i3AnimationContext2::GetClassMeta()) );

	i3Skeleton * pFrameSkel = ((i3AnimationContext2*)pCtx)->getFrameSkeleton();

	Env.m_FrameID = idFrame;
	Env.m_pModelViewMatrix = &I3_IDENTITYMATRIX;
	Env.m_Time	= tm;

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone2( i);

		if( pBoneMap != NULL)	idx = pBoneMap[ i];
		else					idx = i;

		if( idx == -1)
			continue;

		if( getSkelFlag() & I3_SKELFLAG_USE_MOTION )
		{
			// Motion data를 사용하는 경우
			i3Matrix::Copy( pCtx->getCacheMatrix( idx), m_pTxMotionBoneArray->GetMatrix( idx));
		}
		else
		{
			if( (pCtx->getCombinerCount() > 0))
			{
				pCombiner = pCtx->getCombiner( idx);

				if( pCombiner->getAnimationCount() > 0)
				{	
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
	}
}

void i3Skeleton2::TransformWorldSpace( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap)
{
	INT32 idx;
	INT32 i, prevDepth = -1;
	i3Bone2 * pBone;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	I3ASSERT( pCtx != NULL);
	I3ASSERT( pCtx->IsExactTypeOf( i3AnimationContext2::GetClassMeta()) );

	i3Skeleton * pFrameSkel = ((i3AnimationContext2*)pCtx)->getFrameSkeleton();
	bool bDynamic;

#if defined (I3_PHYSX)
	bDynamic = pFrameSkel->isDynamic();
#else 
	bDynamic = false;
#endif

	I3ASSERT( pCtx != NULL);

	pStack->SetBookmark();

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone2( i);

		INT32 iPopCount = (prevDepth - ((INT32)pBone->getDepth() - 1));
		pStack->Pop( iPopCount);

		if( pBoneMap != NULL)	idx = pBoneMap[ i];
		else					idx = i;

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
		else
		{
#if defined( I3_PHYSX)
			if( pFrameSkel->isShapeSet( idx))
			{	
				pFrameSkel->getShapeSet( idx)->getShapeGlobalPose( pCtx->getCacheMatrix( idx));

				pStack->Push( pCtx->getCacheMatrix( idx), NULL);
			}
			else
#endif
			{
				pStack->PushPre( pCtx->getCacheMatrix( idx), pBone->getMatrix());
			}
		}

		prevDepth = pBone->getDepth();
	}

	pStack->RestoreBookmark();
}

/*virtual*/ void i3Skeleton2::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	INT32 i;

	if( pDest->IsExactTypeOf( i3Skeleton2::GetClassMeta()) == FALSE)
		return;

	i3Skeleton::CopyTo( pDest, method);

	i3Skeleton2 * pSkel = (i3Skeleton2 *) pDest;

	// IK Context
	if( m_pIKContextList != NULL)
	{
		if( pSkel->getIKContextList() == NULL)
		{
			i3List * pList = i3List::NewObjectRef();

			pList->SetCount( getIKContextCount() );
			pSkel->setIKContextList( pList);
		}
		else
		{
			if( pSkel->getIKContextList()->GetCount() != getIKContextCount() )
				pSkel->getIKContextList()->SetCount( getIKContextCount());
		}

		if( method == I3_COPY_REF)
		{
			for( i = 0; i < getIKContextCount(); i++)
			{
				if( getIKContext( i) != NULL)
				{
					pSkel->setIKContext( i, getIKContext( i));
				}
			}
		}	
		else
		{
			for( i = 0; i < getIKContextCount(); i++)
			{
				if( getIKContext( i) != NULL)
				{
					i3IKContext * pCtx = i3IKContext::NewObjectRef();

					getIKContext( i)->CopyTo( pCtx, method);

					pSkel->setIKContext( i, pCtx);
				}
			}
			
		}
	}
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tagi3PersistSkeleton2
{
	UINT8			m_ID[4];
	UINT32			m_dynamicState;
	OBJREF			m_pBoneListAttr;
	UINT32			m_ShapeSetCount;
	UINT32			m_LuaPlayerCount;
	UINT32			m_SkeletonFlag;
	UINT32			pad[6];
} I3_PERSIST_SKELETON2;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

/*virtual*/ UINT32 i3Skeleton2::OnSave( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_SKELETON2 data;

	i3mem::FillZero( &data, sizeof(data));

	i3mem::Copy( data.m_ID, "SKT3", 4);
	data.m_dynamicState		= m_dynamicState;
	data.m_pBoneListAttr	= (OBJREF) pResFile->GetObjectPersistID( m_pBoneListAttr);

#if defined (I3_PHYSX)
	if( m_ppShapeSet != NULL)
		data.m_ShapeSetCount = m_pBoneListAttr->getBoneCount();
#endif

	data.m_LuaPlayerCount = 0;

	data.m_SkeletonFlag = m_nSkelFlag;

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

#if defined (I3_PHYSX)
	if( m_ppShapeSet != NULL)
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

/*virtual*/ UINT32 i3Skeleton2::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	UINT32 Rc, Result = 0;
	I3_PERSIST_SKELETON2 data;
	i3BoneMatrixListAttr * pAttr;	

	Rc = pResFile->Read( data.m_ID, 4);
	I3_CHKIO( Rc);
	Result += Rc;

	if( memcmp( data.m_ID, "SKT3", 4) == 0)
	{
		// New version
		Rc = pResFile->Read( &data.m_dynamicState, sizeof(data) - 4);
		I3_CHKIO( Rc);
		Result += Rc;

		m_nSkelFlag = data.m_SkeletonFlag;

		if( data.m_pBoneListAttr != NULL)
		{
			pAttr = (i3BoneMatrixListAttr *) pResFile->FindObjectByID( data.m_pBoneListAttr);
			I3ASSERT( pAttr != NULL);

			setBoneMatrixListAttr( pAttr);
		}

		// ShapeSet Array
#if defined (I3_PHYSX)
		if( data.m_ShapeSetCount > 0)
		{
			I3MEM_SAFE_FREE( m_ppShapeSet);

			setShapeSet( 0, NULL);			// <- Memory 할당용.

			Rc = pResFile->Read( m_ppShapeSet, sizeof(void *) * getBoneCount());
			I3_CHKIO( Rc);

			for( i = 0; i < getBoneCount(); i++)
			{
				if( m_ppShapeSet[i] != NULL)
				{
					i3PhysixShapeSet * pShapeSet = (i3PhysixShapeSet *) pResFile->FindObjectByID( (OBJREF) m_ppShapeSet[i]);
					I3ASSERT( pShapeSet != NULL);

					//I3TRACE( L"SHAPESET : %s\n", pShapeSet->GetName());

					m_ppShapeSet[i] = pShapeSet;

					I3_SAFE_ADDREF( pShapeSet);
				}
			}
			
		}
#endif	
	}
	else
	{
		I3FATAL( "invalid ID..%s", data.m_ID);
	}

	return Result;
}

void i3Skeleton2::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	INT32 i;
#if defined (I3_PHYSX)
	if( m_ppShapeSet != NULL)
	{
		for( i = 0; i < getBoneCount(); i++)
		{
			if( m_ppShapeSet[i] != NULL)
			{
				i3PhysixShapeSet * pShapeSet = getShapeSet( i);
				I3ASSERT( pShapeSet != NULL);

				pShapeSet->OnInstancing( pInfo);
			}
		}

		// Joint를 생성해준다.
		CreateJoints();
	
		setDynamicState( m_dynamicState); //| I3_PHYSIX_STATE_KINEMATIC );
	}
#endif

	// LuaPlayer를 Bind합니다.
	/*if( m_ppLuaPlayer != NULL)
	{
		for( i = 0; i < getBoneCount(); i++)
		{
			if( isLuaPlayer( i) )
			{
				i3SceneUtilScript::BindLuaFunction( getLuaPlayer( i));
				i3LuaMatrix::BindLuaFunction( getLuaPlayer( i));
				i3LuaMath::BindLuaFunction( getLuaPlayer( i));
				i3LuaVector::BindLuaFunction( getLuaPlayer( i));
				i3LuaQuat::BindLuaFunction( getLuaPlayer( i));
			}
		}
	}*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//		for IK animation
////////////////////////////////////////////////////////////////////////////////////////////////////

//bool i3Skeleton2::_CheckSharingBone( i3IKBoneInfo * pBoneInfo, i3IKContext * pSrcIKCtx)
//{
//	INT32 i;
//
//	i3IKBoneInfo * pInsertBoneInfo;
//	i3Bone2 * pBone;
//
//	I3ASSERT( pBoneInfo != NULL);
//
//	for( i = 0; i < pSrcIKCtx->getIKBoneCount(); i++)
//	{
//		pInsertBoneInfo = pSrcIKCtx->getIKBoneInfo( i);
//
//		pBone = getBone2( pInsertBoneInfo->getBoneIndex());
//		if( pBone->getFlag() & I3_IKBONE_FLAG_EFFECTOR)
//		{
//			while( pBone )
//			{	
//				if( pBoneInfo->getBoneIndex() == pBone->getParentIndex() )
//				{
//					// 이미 Context가 있는 본을 사용하고 있다.
//					return true;
//				}
//
//				if( pBone->getParentIndex() != -1)
//					pBone = getBone2( pBone->getParentIndex());
//				else
//					pBone = NULL;
//			}
//		}
//	}
//
//	return false;
//}

//bool i3Skeleton2::_CheckIncludeBone( i3IKBoneInfo * pBoneInfo, i3IKContext * pSrcIKCtx)
//{
//	INT32 i;
//	i3IKBoneInfo * pInsertBoneInfo;
//
//	for( i = 0; i < pSrcIKCtx->getIKBoneCount(); i++)
//	{
//		pInsertBoneInfo = pSrcIKCtx->getIKBoneInfo( i);
//
//		if( pBoneInfo->getBoneIndex() == pInsertBoneInfo->getBoneIndex() )
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

//bool i3Skeleton2::_MergeBranch( i3IKContext * pIKCtx, INT32 * pBoneMap )
//{
//	INT32 i, j;
//
//	i3IKBoneInfo * pNewInfo;
//	i3IKBoneInfo * pTempInfo;
//
//	bool bRv = false;
//
//	// 등록되어 있는 본들을 검사한다.
//	for( i = 0; i < getIKContextCount(); i++)
//	{
//		i3IKContext * pPrevCtx = getIKContext( i);
//
//		// check include bone
//		for( j = 0; j < pPrevCtx->getIKBoneCount(); j++)
//		{
//			pTempInfo = pPrevCtx->getIKBoneInfo( j);
//			
//			bRv = _CheckSharingBone( pTempInfo, pIKCtx);
//
//			if( bRv)
//				break;
//		}
//
//		// 하나의 Context로 merge한다.
//		if( bRv)
//		{	
//			// copy
//			INT32 iPrevInfoCount = pPrevCtx->getIKBoneCount();
//			INT32 iCount = 0;
//
//			// BoneInfo를 검사한다. 새로운 정보라면 새로 생성하여 복사한다.
//			for( j = 0; j < pIKCtx->getIKBoneCount(); j++)
//			{	
//				pNewInfo = pIKCtx->getIKBoneInfo( j);
//
//				if( _CheckIncludeBone( pNewInfo, pPrevCtx) == false)
//				{
//					// 새로운 bone정보입니다. 
//					pPrevCtx->setIKBoneCount( iPrevInfoCount + (iCount+1) );
//
//					i3IKBoneInfo * pInfo = pPrevCtx->getIKBoneInfo( iPrevInfoCount + iCount);
//					i3mem::Copy( pInfo, pIKCtx->getIKBoneInfo( j), sizeof( i3IKBoneInfo) );
//					pInfo->setConstraintIndex( pPrevCtx->getIKConstraintCount() );
//
//					INT32 iBoneIdx = pInfo->getBoneIndex();
//					
//					getBone2( iBoneIdx)->setIKBoneInfoIndex( iPrevInfoCount + iCount);
//					getBone2( iBoneIdx)->setIKContextIndex( i);
//
//					// Joint Count를 새로 세팅합니다.
//					_IncreaseIKJointCountProcess( iBoneIdx, pPrevCtx, pInfo);
//
//					iCount++;
//				}
//			}
//
//			// Constraint를 생성
//			for( j = 0; j < pIKCtx->getIKConstraintCount(); j++)
//			{
//				i3IKConstraint * pConstraint = i3IKConstraint::NewObject();
//
//				pIKCtx->getIKConstraint( j)->CopyTo( pConstraint, I3_COPY_INSTANCE);
//
//				pPrevCtx->addIKConstraint( pConstraint);
//			}
//
//			break;
//		}
//	}
//
//	return bRv;
//}

//void i3Skeleton2::_IncreaseIKJointCountProcess( INT32 iBoneIdx, i3IKContext * pPrevCtx, i3IKBoneInfo * pSetInfo)
//{
//	INT32 i, j;
//	INT32 iJointCount = 0;
//	UINT32 iStarterDepth = 0;
//
//	for( i = 0; i < getBoneCount(); i++)
//	{
//		i3Bone2 * pTempBone = getBone2( i);
//
//		if( (pPrevCtx->getIKContextIdx() == pTempBone->getIKContextIndex()) &&
//			(pTempBone->getFlag() & I3_BONEFLAG_IS_IK) &&
//			(iBoneIdx != i) &&
//			(getBone( iBoneIdx)->getDepth() == pTempBone->getDepth()) )
//		{
//			i3IKBoneInfo * pTempIKInfo = getIKBoneInfoByBoneIdx( pPrevCtx, i);
//			pSetInfo->setIKJointCount( pTempIKInfo->getIKJointCount() + 1);
//
//			for( j = 0; j < getBoneCount(); j++)
//			{
//				i3Bone2 * pTempBone2 = getBone2( j);
//
//				if( (pPrevCtx->getIKContextIdx() == pTempBone2->getIKContextIndex()) &&
//					(pTempBone2->getFlag() & I3_BONEFLAG_IS_IK) &&
//					(iBoneIdx != j) &&
//					(getBone( iBoneIdx)->getDepth() < pTempBone2->getDepth()) )
//				{
//					i3IKBoneInfo * pTempIKInfo2 = getIKBoneInfoByBoneIdx( pPrevCtx, j);
//					pTempIKInfo2->setIKJointCount( pTempIKInfo2->getIKJointCount() + 1);
//				}
//				
//			}
//		}
//	}
//}

void i3Skeleton2::_SetDefaultIKBoneInfo( i3IKBoneInfo * pOut, INT32 iBoneIdx)
{
	bool bDefault = false;

	I3ASSERT( pOut != NULL);

	I3_BOUNDCHK( iBoneIdx, getBoneCount());

#if defined( I3_PHYSX)
	if( isShapeSet( iBoneIdx))
	{
		i3PhysixShapeSet * pShapeSet = getShapeSet( iBoneIdx);
		I3ASSERT( pShapeSet != NULL);
	
		i3PhysixJointInfoD6 * pJointInfo = (i3PhysixJointInfoD6*) pShapeSet->getJointInfo();

		if( pJointInfo != NULL)
		{
			VEC3D * pLocalAxis = pJointInfo->getLocalAxis( 0);

			if( pJointInfo->swing1Motion == NX_D6JOINT_MOTION_LIMITED)
			{
				pOut->setLimitSwing1High( pJointInfo->swing1Limit.value * 0.5f);
				pOut->setLimitSwing1Low( -pJointInfo->swing1Limit.value * 0.5f);
				pOut->setTensionSwing1( pJointInfo->swing1Limit.restitution);
			}
			else if( pJointInfo->swing1Motion == NX_D6JOINT_MOTION_LOCKED)
			{
				pOut->setLimitSwing1High( 0.f);
				pOut->setLimitSwing1Low( 0.f);
				pOut->setTensionSwing1( 0.f);
			}

			if( pJointInfo->swing2Motion == NX_D6JOINT_MOTION_LIMITED)
			{
				pOut->setLimitSwing2High( pJointInfo->swing2Limit.value * 0.5f);
				pOut->setLimitSwing2Low( -pJointInfo->swing2Limit.value * 0.5f);
				pOut->setTensionSwing2( pJointInfo->swing1Limit.restitution);
			}
			else if( pJointInfo->swing2Motion == NX_D6JOINT_MOTION_LOCKED)
			{
				pOut->setLimitSwing2High( 0.f);
				pOut->setLimitSwing2Low( 0.f);
				pOut->setTensionSwing2( 0.f);
			}

			if( pJointInfo->twistMotion == NX_D6JOINT_MOTION_LIMITED)
			{	
				pOut->setLimitTwistHigh( pJointInfo->twistLimit.high.value);
				pOut->setLimitTwistLow( pJointInfo->twistLimit.low.value);
				pOut->setTensionTwist( pJointInfo->twistLimit.high.restitution);
			}
			else if( pJointInfo->twistMotion == NX_D6JOINT_MOTION_LOCKED)
			{
				pOut->setLimitTwistHigh( 0.f );
				pOut->setLimitTwistLow( 0.f );
				pOut->setTensionTwist( 0.f );
			}
		}
	}
	else
		bDefault = true;
#else
	bDefault = true;
#endif

	if( bDefault)
	{
		i3Bone2 * pBone = getBone2( iBoneIdx);

		const char * pszName = pBone->getName();

		if( (i3String::Compare( pszName, "L_Knee") == 0) ||
			(i3String::Compare( pszName, "R_Knee") == 0) )
		{
			pOut->setLimitSwing1High( 0.f);
				pOut->setLimitSwing1Low( 0.f);
				pOut->setTensionSwing1( 0.f);
			pOut->setLimitSwing2High( 0.f);
				pOut->setLimitSwing2Low( -1.f);
				pOut->setTensionSwing2( 0.5f);
			pOut->setLimitTwistHigh( 0.f );
				pOut->setLimitTwistLow( 0.f );
				pOut->setTensionTwist( 0.f );
		}
		else
		{
			pOut->setLimitSwing1High( 1.f);
				pOut->setLimitSwing1Low( -1.f);
				pOut->setTensionSwing1( 1.f);
			pOut->setLimitSwing2High( 1.f);
				pOut->setLimitSwing2Low( -1.f);
				pOut->setTensionSwing2( 1.f);
			pOut->setLimitTwistHigh( 1.f );
				pOut->setLimitTwistLow( -1.f );
				pOut->setTensionTwist( 1.f );
		}
	}
}

void i3Skeleton2::BuildIKBoneInfo( INT32 iStartBoneIndex, INT32 iEndEffectBoneIndex)
{
	INT32 i;
	INT32 iBoneIdx = iEndEffectBoneIndex;
	UINT32 ikType = 0;
	INT32 iCount = 0;

	INT32 iStartIdx = iStartBoneIndex;
	INT32 iEndIdx = iEndEffectBoneIndex;
	
	INT32 iIKBoneCount = 0;

	//find bone
	i3Bone2 * pEndBone = pEndBone = getBone2( iEndIdx);

	if( m_pIKContextList == NULL)
	{
		m_pIKContextList = i3List::NewObject();
	}

	INT32 IKContextIndex = m_pIKContextList->GetCount();

	UINT32 * boneFlagMap = (UINT32 *) i3MemAlloc( sizeof(UINT32) * getBoneCount());
	i3mem::FillZero( boneFlagMap, sizeof(UINT32) * getBoneCount());

	i3IKContext * pIKContext = i3IKContext::NewObject();

	// find IK Child bone
	for( i = 0; i < getBoneCount(); i++)
	{
		i3Bone2 * pBone = getBone2( i);

		INT32 iParentIdx = pBone->getParentIndex();

		while( iParentIdx != -1)
		{
			if( iParentIdx == iEndIdx ||
				iParentIdx == iStartIdx )
			{
				boneFlagMap[ i] = I3_IKBONE_FLAG_CHILD;
				break;
			}

			iParentIdx = getBone( iParentIdx)->getParentIndex();
		}
	}

	iBoneIdx = iEndIdx;

	while( pEndBone)
	{
		iIKBoneCount++;

		if( pEndBone->getParentIndex() != -1 && pEndBone->getParentIndex() >= iStartIdx)
		{
			// find bone
			pEndBone = getBone2( pEndBone->getParentIndex());
		}
		else
			break;
	}

	pIKContext->CreateIKConstraint( iIKBoneCount, iStartIdx, iEndIdx);

	pEndBone = getBone2( iEndIdx);
	iBoneIdx = iEndIdx;
	
	while( pEndBone)
	{
		i3IKBoneInfo * pInfo = pIKContext->getIKBoneInfo( iCount);
		I3ASSERT( pInfo);

		i3mem::FillZero( pInfo, sizeof( i3IKBoneInfo));

		pInfo->setBoneIndex( iBoneIdx);
		pInfo->setIKJointCount( iIKBoneCount - (iCount+1) );
		i3Matrix::Identity( pInfo->getNextFrameRotate());
		pInfo->setRotateLimit( I3_DEG2RAD( 5.f) );
		pInfo->setFirstFrameValue( true);

		if( iBoneIdx == iStartIdx)
		{
			boneFlagMap[iBoneIdx]  |= I3_IKBONE_FLAG_STARTER;
		}
		else if( iBoneIdx == iEndIdx)
		{
			boneFlagMap[iBoneIdx] |= I3_IKBONE_FLAG_EFFECTOR;
		}
		else if( iBoneIdx > iStartIdx && iBoneIdx < iEndIdx)
		{	
			boneFlagMap[iBoneIdx] &= ~I3_IKBONE_FLAG_CHILD;
		}

		pInfo->setFlag( boneFlagMap[iBoneIdx]);

		pEndBone->setIKBoneInfoIndex( iCount);
		pEndBone->setIKContextIndex( IKContextIndex );

		_SetDefaultIKBoneInfo( pInfo, iBoneIdx);

		if( pEndBone->getParentIndex() != -1 && pEndBone->getParentIndex() >= iStartIdx)
		{
			// find bone
			iBoneIdx = pEndBone->getParentIndex();
			pEndBone = getBone2( iBoneIdx);
		}
		else
			break;

		iCount++;
		I3ASSERT( iCount <= iIKBoneCount);
	}

	// Bone에 새로추가되는 IKContext의 Index를 색인합니다.
	// 검색을 없애기 위함
	for( i = 0; i < pIKContext->getIKBoneCount(); i++)
	{
		i3IKBoneInfo * pIKInfo = pIKContext->getIKBoneInfo( i);
		i3Bone2 * pBone = getBone2( pIKInfo->getBoneIndex());
		pBone->setIKContextIndex( IKContextIndex);
	}

	// find IK Child bone
	for( i = 0; i < getBoneCount(); i++)
	{
		i3Bone2 * pBone = getBone2( i);

		INT32 iParentIdx = pBone->getParentIndex();

		while( iParentIdx != -1)
		{
			if( iParentIdx == iEndIdx || iParentIdx == iStartIdx)
			{	
				pBone->setIKContextIndex( IKContextIndex);
				break;
			}

			iParentIdx = getBone( iParentIdx)->getParentIndex();
		}
	}
	
	m_pIKContextList->Add( pIKContext);

	I3_SAFE_FREE( boneFlagMap);
}

void i3Skeleton2::AfterTraverseIKUpdate( i3SceneTracer * pTracer, i3AnimationContext2 * pCtx, INT32 * pBoneMap)
{
	INT32 idx;
	INT32 i, prevDepth;
	MATRIX * pIn, tmpOut;
	i3Bone2 * pBone;
	i3TransformSourceCombiner * pCombiner;

	// IKAnimation이 아닙니다.
	if( m_pIKContextList == NULL)
		return;

	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	
	I3ASSERT( pCtx != NULL);

	bool bDynamic = false;
	
	i3Skeleton2 * pFrameSkel = (i3Skeleton2*) pCtx->getFrameSkeleton();
	I3ASSERT( pFrameSkel != NULL);

    bDynamic = pFrameSkel->isDynamic();

	pStack->SetBookmark();

	prevDepth = -1;

	for( i = 0; i < getBoneCount(); i++)
	{
		pBone = getBone2( i);

		INT32 iPopCount = (prevDepth - ((INT32)pBone->getDepth() - 1));
		pStack->Pop( iPopCount);

		if( pBoneMap != NULL)	idx = pBoneMap[ i];
		else					idx = i;

		if( idx != -1)
		{	
			i3IKContext * pIKContext = NULL;
			if( pBone->getIKContextIndex() != -1)
				pIKContext = getIKContext( pBone->getIKContextIndex());

			bool bIncludeCtx = CheckIncludeIKContext( pIKContext, idx, idx, pBoneMap);
			bool bIKChild = (bool)((pBone->getFlag() & I3_IKBONE_FLAG_CHILD) == I3_IKBONE_FLAG_CHILD);

			if( (bIncludeCtx && !bIKChild) ||	// 하위 본이지만 IK이다. 스킵
				(!bIncludeCtx && bIKChild) ||	// 하위 본이지만 다른 IK Node에 붙어 있는 경우 스킵
				(!bIncludeCtx && !bIKChild) )	// IK 가 아니다. 스킵
				
			{
				// 기존 Transform을 사용한다.
				pStack->Push( pCtx->getCacheMatrix( idx), NULL);
			}
			else
			{
				// EndEffector bone의 하위 본을 다시 업데이트 한다.

				pIn = pBone->getMatrix();

				if( bDynamic == false)
				{
					// Animation에서 Transform을 가져온다.
					if( pCtx->getCombinerCount() > 0)
					{
						pCombiner = pCtx->getCombiner( idx);

						if( pCombiner->getAnimationCount() > 0)
						{
							I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

							pEnv->m_Time = pTracer->GetTime();
							pEnv->m_pIn = pIn;
							pEnv->m_pOut = &tmpOut;
							pEnv->m_pModelViewMatrix = pStack->GetTop();

							pCombiner->GetMatrix( pEnv);

							pIn = &tmpOut;
						}
					}

					pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);

	#if defined (I3_PHYSX)
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
						pStack->Push( pCtx->getCacheMatrix( idx), NULL);		// Stack에 단순 Push
					}
					else
					{
						// 기존 Transform을 사용한다.
						pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);
					}
				}
	#else
				}
				else
				{
					// 기존 Transform을 사용한다.
					pStack->PushPre( pCtx->getCacheMatrix( idx), pIn);
				}
	#endif
			}

		}

		prevDepth = pBone->getDepth();
	}

	pStack->RestoreBookmark();

}

void i3Skeleton2::ConvertFromSkeleton( i3Skeleton * pSrc)
{
	INT32 i;

	I3ASSERT( pSrc->IsExactTypeOf( i3Skeleton::GetClassMeta()) );
	
	if( pSrc->getBoneMatrixListAttr() != NULL)
	{
		((i3PersistantElement*)pSrc)->CopyTo( this, I3_COPY_INSTANCE);

		if( pSrc->getBoneCount() > 0)
		{
			SetBoneCount( pSrc->getBoneCount());
			
			for( i = 0; i < pSrc->getBoneCount(); i++)
			{
				i3Bone * pBone = pSrc->getBone( i);

				i3Bone2 * pDestBone = getBone2(i);

				pDestBone->setDepth( pBone->getDepth());
				pDestBone->setFlag( 0);
				pDestBone->setIKBoneInfoIndex( -1);
				pDestBone->setIKContextIndex( -1);
				pDestBone->setMatrix( pBone->getMatrix());
				pDestBone->setName( pBone->getName());
				pDestBone->setParentIndex( pBone->getParentIndex());
			}
		}

		if( pSrc->getBoneMatrixListAttr()->getInverseBoneArray() != NULL)
		{
			getBoneMatrixListAttr()->setInverseBoneArray( pSrc->getBoneMatrixListAttr()->getInverseBoneArray());
		}
	}

	if( m_pTxMotionBoneArray != NULL)
	{		
		for( i = 0; i < pSrc->getBoneCount(); i++)
		{
			m_pTxMotionBoneArray->SetMatrix( i, pSrc->getBone( i)->getMatrix() );
		}
	}

#if defined (I3_PHYSX)
	{	
		i3PhysixShapeSet * pDest;

		for( i = 0; i < pSrc->getBoneCount(); i++)
		{
			if( pSrc->isShapeSet( i) )
			{
				pDest = i3PhysixShapeSet::NewObjectRef();

				pSrc->getShapeSet( i)->CopyTo( pDest, I3_COPY_INSTANCE);

				setShapeSet( i, pDest);

				//I3TRACE( L"Bone Shape(%d) Name : %s\n", i, pDest->GetName() );
			}
		}
	}

	setDynamicState( m_dynamicState);
#endif
}

void	i3Skeleton2::ConvertToSkeleton( i3Skeleton * pDest)
{
	INT32 i;

	I3ASSERT( pDest->IsExactTypeOf( i3Skeleton::GetClassMeta()) );
	
	if( getBoneMatrixListAttr() != NULL)
	{
		if( getBoneCount() > 0)
		{
			pDest->SetBoneCount( getBoneCount());

			for( i = 0; i < getBoneCount(); i++)
			{
				i3Bone2 * pBone2 = getBone2( i);
				i3Bone * pDestBone = pDest->getBone( i);

				pDestBone->setDepth( pBone2->getDepth());
				pDestBone->setMatrix( pBone2->getMatrix());
				pDestBone->setName( (char*)pBone2->getName());
				pDestBone->setParentIndex( pBone2->getParentIndex());
			}
		}

		if( getBoneMatrixListAttr()->getInverseBoneArray() != NULL)
		{
			pDest->getBoneMatrixListAttr()->setInverseBoneArray( getBoneMatrixListAttr()->getInverseBoneArray());
		}
	}

#if defined (I3_PHYSX)
	{	
		i3PhysixShapeSet * pShapeSet;

		if( m_ppShapeSet != NULL)
		{
			for( i = 0; i < getBoneCount(); i++)
			{
				if( isShapeSet( i) )
				{
					pShapeSet = i3PhysixShapeSet::NewObjectRef();

					getShapeSet( i)->CopyTo( pShapeSet, I3_COPY_INSTANCE);

					pDest->setShapeSet( i, pShapeSet);
				}
			}
		}
	}

	pDest->setDynamicState( m_dynamicState);
#endif
}

bool i3Skeleton2::CheckIncludeIKContext( i3IKContext * pIKContext, INT32 iStartBoneIdx, INT32 iBoneIdx, INT32 * pBoneMap)
{
	INT32 i;
	if( pIKContext == NULL)
		return false;
	INT32 iIdx = iBoneIdx;

	if( pBoneMap != NULL)
	{
		for( i = 0; i < getBoneCount(); i++)
		{
			if( pBoneMap[ i] == iBoneIdx)
				iIdx = i;
		}
	}

	i3Bone2 * pBone = getBone2( iIdx);

	while( pBone )
	{
		if( iStartBoneIdx > iBoneIdx )
			return false;

		if( pIKContext->FindIKBone( pBone->getParentIndex()) != -1)
			return true;

		iIdx = pBone->getParentIndex();

		if( iIdx != -1)		pBone = getBone2( iIdx);
		else				pBone = NULL;
	}

	return false;
}


void i3Skeleton2::SetEnableIKAnimation( bool bEnable)
{
	INT32 i, j;
	i3IKContext * pIKCtx;
	i3IKBoneInfo * pIKInfo;
	

	if( bEnable)
	{
		addSkelFlag( I3_SKELFLAG_IK_ENABLE);

		for( i = 0; i < getIKContextCount(); i++)
		{
			pIKCtx = getIKContext( i);
			pIKCtx->addIKStyle( I3_IKBONE_STYLE_ENABLE);

			for( j = 0; j < pIKCtx->getIKBoneCount(); j++)
			{
				pIKInfo = pIKCtx->getIKBoneInfo( j);
				pIKInfo->setFirstFrameValue( true);
				pIKInfo->addFlag( I3_IKBONE_FLAG_DISABLE);
			}
		}
	}
	else
	{
		removeSkelFlag( I3_SKELFLAG_IK_ENABLE);
		
		for( i = 0; i < getIKContextCount(); i++)
		{
			pIKCtx = getIKContext( i);
			pIKCtx->removeIKStyle( I3_IKBONE_STYLE_ENABLE);

			for( j = 0; j < pIKCtx->getIKBoneCount(); j++)
			{
				pIKInfo = pIKCtx->getIKBoneInfo( j);
				pIKInfo->setFirstFrameValue( true);
				pIKInfo->addFlag( I3_IKBONE_FLAG_DISABLE);
			}
		}
	}
}

