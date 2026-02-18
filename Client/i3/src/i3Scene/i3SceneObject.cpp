#include "i3SceneDef.h"
#include "i3SceneObject.h"
#include "i3Transform.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"
#include "i3BoneRef.h"
#include "i3Base/smart_ptr/make_shared.h"

#include "i3Base/string/compare/generic_compare.h"

I3_CLASS_INSTANCE( i3SceneObject);

static i3SceneObject * s_pDebugSceneObj = nullptr;

I3_TRACE_RESULT I3SG_NODE_PROC( i3SceneObject)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3SceneObject * pSceneObj = (i3SceneObject *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	REAL32 timeOld;
	i3PhysixShapeSet * pShapeSet = nullptr;
	bool bNeedToUpdate = true;

	// Transform process
	if( (pSceneObj->IsFlag( I3_NODEFLAG_DYNAMIC_UPDATE)) && (pSceneObj->m_pWorldSpaceSource != nullptr))
	{
		if( (pSceneObj->GetUpdateFrameID() != pTracer->getFrameID()))
		{
			#if defined (I3_PHYSX)
				// Dynamic 상태라면, Matrix Stack과는 상관없이 어딘가에 World-Space Matrix를
				// Update하는 Node가 있기 때문에...
				if( i3::same_of<i3Body*>(pSceneObj->m_pWorldSpaceSource))
				{
					i3Body * pBody = (i3Body *) pSceneObj->m_pWorldSpaceSource;
					i3LOD * pFrameLOD = pBody->getFrameLOD();
					I3ASSERT( pFrameLOD != nullptr);

					i3Skeleton * pSkel = pFrameLOD->getSkeleton();
					I3ASSERT( pSkel != nullptr);

					INT32 i;

					for( i = 0; i < pSkel->getBoneCount(); i++)
					{
						if( pSkel->isShapeSet( i))
						{
							pShapeSet = pSkel->getShapeSet(i);
							break;
						}
					}

					I3ASSERT_RETURN( pShapeSet != nullptr, Result);
					
					if( pShapeSet->isDynamicState( I3_PHYSIX_STATE_NO_COLLISION) == false)
					{
						pShapeSet->getShapeGlobalPose( pSceneObj->GetCacheMatrix());

						// I3ASSERT( i3Matrix::isValid( pSceneObj->GetName(), pSceneObj->GetCacheMatrix()));
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
						I3ASSERT( pSceneObj->isValidMatrix(pSceneObj->GetCacheMatrix()));
#endif

						bNeedToUpdate = false;
					}
				}
				else
				{
					i3Transform2 * pTrans2 = (i3Transform2 *) pSceneObj->m_pWorldSpaceSource;

					pShapeSet = pTrans2->getShapeSet();

					if( pShapeSet != nullptr)
					{
						pShapeSet->getShapeGlobalPose( pSceneObj->GetCacheMatrix());

						// I3ASSERT( i3Matrix::isValid( pSceneObj->GetName(), pSceneObj->GetCacheMatrix()));
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
						I3ASSERT( pSceneObj->isValidMatrix(pSceneObj->GetCacheMatrix()));
#endif
						bNeedToUpdate = false;
					}
				}
			#endif

			// Update SceneObject Matrix by CacheMatrix
			pSceneObj->SetMatrix( pSceneObj->GetCacheMatrix() );
		}
		else
		{
			bNeedToUpdate = false;
		}
	}

	bNeedToUpdate = bNeedToUpdate && (pSceneObj->GetUpdateFrameID() != pTracer->getFrameID());
	bNeedToUpdate = bNeedToUpdate && (pSceneObj->IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == false);
	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedToUpdate = bNeedToUpdate || (pSceneObj->GetUpdateFrameID() == 0xFFFFFFFF);

	if( bNeedToUpdate)
	{
		if( pSceneObj->IsAnimated())
		{
			I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

			pEnv->m_Time = pTracer->GetTime();
			pEnv->m_pIn = pSceneObj->GetMatrix();
			pEnv->m_pOut = pSceneObj->GetMatrix();
			pEnv->m_pModelViewMatrix = pStack->GetTop();

			pSceneObj->GetAnimatedMatrix( pEnv);
		}

		pStack->PushPre( pSceneObj->GetCacheMatrix(), pSceneObj->GetMatrix());
	}
	else
	{
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT( pSceneObj->isValidMatrix(pSceneObj->GetCacheMatrix()));
#endif

		pStack->Push( pSceneObj->GetCacheMatrix(), nullptr);
	}
	
	pSceneObj->SetUpdateFrameID( pTracer->getFrameID());

	{
		// 자신의 Model View Matrix를 tracer에 등록해 둔다.
		i3ModelViewMatrixAttr * pAttr = pTracer->AllocModelViewMatrixAttr();

		_copyTop( pAttr->GetMatrix(), pStack);

		pStackManager->PushAttr( pAttr);
	}

	timeOld = pTracer->GetTime();

	pSceneObj->traverse( pTracer->GetTime(), pTracer);

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}
	pTracer->SetTime( timeOld);

	// Transform Process
	{
		pStackManager->PopAttr( I3_ATTRID_MODEL_VIEW_MATRIX);

		pStack->Pop();
	}

	return Result;
}


i3SceneObject::i3SceneObject(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3SceneObject));
}

void i3SceneObject::Rec_BuildBoneList( i3Node * pNode)
{
	INT32 i;
	i3Node * pChild;

	if( i3::kind_of<i3Transform* >(pNode))
	{
		if( pNode->hasName())
		{
			m_pBoneList->insert( static_cast<i3Transform*>(pNode));
		}
	}
	else if( i3::same_of<i3Body* >(pNode))
	{
		m_pBody = (i3Body *) pNode;
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		Rec_BuildBoneList( pChild);
	}
}

void i3SceneObject::BuildBoneList(void)
{
	if( m_pBoneList == nullptr)
	{
		m_pBoneList = i3::make_shared< i3::vector_multiset<i3Transform*, TransformNameCompareProc> >();
	}

	m_pBoneList->clear();

	INT32 i;

	for( i = 0; i < GetChildCount(); i++)
	{
		Rec_BuildBoneList( GetChild( i));
	}
}

void i3SceneObject::GetWorldSpaceSourceMatrix( MATRIX * pMtx)
{
	if( m_pWorldSpaceSource != nullptr)
	{
		i3PhysixShapeSet * pShapeSet = nullptr;

		if( i3::same_of<i3Body* >(m_pWorldSpaceSource))
		{
			i3Body * pBody = (i3Body *) m_pWorldSpaceSource;
			i3LOD * pLOD = pBody->getFrameLOD();

			i3Skeleton * pSkel = pLOD->getSkeleton();
			I3ASSERT( pSkel != nullptr);

			INT32 i;

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				if( pSkel->isShapeSet( i))
				{
					pShapeSet = pSkel->getShapeSet( i);
					break;
				}
			}
		}
		else
		{
			i3Transform2 * pTrans2 = (i3Transform2 *) m_pWorldSpaceSource;

			pShapeSet = pTrans2->getShapeSet();
		}

		if( pShapeSet != nullptr)
		{
			pShapeSet->getShapeGlobalPose( pMtx);
		}
		else
		{
			i3Matrix::Identity( pMtx);
		}
	}
	else
	{
		i3Matrix::Identity( pMtx);
	}
}

void i3SceneObject::UpdateLastWorldTransform( MATRIX * pMatrix)
{
#if defined (I3_PHYSX)
	MATRIX	mat;

	//if( (IsFlag( I3_NODEFLAG_DYNAMIC_UPDATE) && (m_pWorldSpaceSource != nullptr)))
	if( (m_pWorldSpaceSource != nullptr))
	{
		i3PhysixShapeSet * pShapeSet = nullptr;

		if( i3::same_of<i3Body* >(m_pWorldSpaceSource))
		{
			i3Body * pBody = (i3Body *) m_pWorldSpaceSource;
			i3LOD * pLOD = pBody->getFrameLOD();
			i3Skeleton * pSkel = pLOD->getSkeleton();
			I3ASSERT( pSkel != nullptr);

			INT32 i;

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				if( pSkel->isShapeSet( i))
				{
					pShapeSet = pSkel->getShapeSet( i);
					break;
				}
			}

			i3Matrix::Copy( &mat, pBody->getCacheMatrix( 0 ) );

			i3Matrix::Copy( pBody->getCacheMatrix( 0), pMatrix);
		}
		else
		{
			i3Transform2 * pTrans2 = (i3Transform2 *) m_pWorldSpaceSource;

			i3Matrix::Copy( &mat, pMatrix );
			i3Matrix::Copy( pTrans2->GetCacheMatrix(), pMatrix);
			pShapeSet = pTrans2->getShapeSet();

			// 만약 새로 Rendering되는 노드라면 0xFFFFFFFF Frame ID로 인해, Physix의 값이 아니라
			// Matrix Stack의 값을 사용하려한다.
			// 이것을 방지하기 위해 Cache Matrix와 Frame ID를 갱신해준다.
			pTrans2->SetUpdateFrameID( GetUpdateFrameID());
		}

		if( pShapeSet != nullptr)
		{
			pShapeSet->setShapeGlobalPose( &mat );
		}
	}
#endif
}

void i3SceneObject::SetPlayTime( REAL32 tm)
{

	i3Transform * pBone;

	if( m_pBody != nullptr)
	{
		m_pBody->SetPlayAnim( tm);
	}

	if( m_pBoneList != nullptr)
	{
		for(size_t i = 0; i < m_pBoneList->size(); i++)
		{
			pBone = m_pBoneList->get_vector()[i];

			pBone->SetPlayTime( tm);
		}
	}
}

INT32 i3SceneObject::FindBoneIndexByName( const char * pszBoneName)
{
	INT32 start, end;
	INT32 idx, rv;
	i3Transform * pTrans;

	if( m_pBody != nullptr)
	{
		return m_pBody->FindBoneByName( pszBoneName);
	}

	if((m_pBoneList == nullptr) || (m_pBoneList->size() <= 0))
		return -1;
		
	start = 0;
	end = (INT32)m_pBoneList->size() - 1;

	while( start < end)
	{
		idx = (start + end) >> 1;
		
		pTrans = m_pBoneList->get_vector()[idx];

		if( pTrans->hasName())
		{
			rv = i3::generic_compare( pTrans->GetNameString(), pszBoneName);
		}
		else
		{
			rv = -1;
		}
		
		if( rv == 0)
			return idx;
		 
		if( rv > 0)
			end = idx - 1;
		else
			start = idx + 1;
	}

	if( start <= end)
	{	
		idx = (start + end) >> 1;
		
		pTrans = m_pBoneList->get_vector()[ idx];

		if( pTrans->hasName())
		{
			if( pTrans->GetNameString() ==  pszBoneName) 
				return idx;
		}
	}
	
	return -1;
}

i3Transform * i3SceneObject::FindBoneByName( const char * pszBoneName)
{
	INT32 idx;

	idx = FindBoneIndexByName( pszBoneName);
	if( idx == -1)
		return nullptr;

	if( m_pBoneList == nullptr)
		return nullptr;

	return m_pBoneList->get_vector()[idx];
}

MATRIX *	i3SceneObject::FindCacheMatrixByName( const char * pszBoneName)
{
	INT32 idx;

	idx = FindBoneIndexByName( pszBoneName);
	if( idx == -1)
		return nullptr;

	if( m_pBody != nullptr)
	{
		return m_pBody->getCacheMatrix( idx);
	}

	i3Transform * pBone = GetBone( idx);

	return pBone->GetCacheMatrix();
}


void i3SceneObject::StopAnim( i3Animation * pAnim )
{
	INT32 i;
	i3Transform * pTrans;

	if((m_pBody != nullptr) && (m_pBody->GetCurrentAnim() == pAnim))
	{
		m_pBody->StopAnim();
		return;
	}

	//pAnim->
	for( i = 0; i < pAnim->GetTrackCount(); i++)
	{
		pTrans = FindBoneByName( pAnim->GetTrackBoneName( i));		
		if( pTrans != nullptr)
		{
			pTrans->SetAnimation( nullptr, -1 );
		}
		#if defined( I3_DEBUG)
		else
		{
			i3Log( "i3SceneObject::StopAnim()", "Invalid Bone : %s\n", pAnim->GetTrackBoneName( i));
		}
		#endif
	}
}

void i3SceneObject::SetAnimMat( const char * pNodeName, MATRIX * pMat)
{
	i3Transform * pTrans; 

	pTrans = FindBoneByName( pNodeName ); 
	if( pTrans != nullptr)
	{
		pTrans->SetAnimation( nullptr, -1);
		pTrans->SetMatrix( pMat ); 
	}
	#if defined( I3_DEBUG )
	else 
	{
		i3Log( "i3SceneObject::SetAnimMat()", "Invalid Bone : %s\n", pNodeName);
	}
	#endif

	return; 
}

void i3SceneObject::PlayAnim( i3Animation * pAnim, I3_ANIM_BLEND_MODE BlendMode, REAL32 StartTime, REAL32 Interval, REAL32 timeScale, REAL32 rManualClampTime,
							 I3_ANIM_PLAY_MODE DestPlayMode, I3_ANIM_PLAY_MODE SrcPlayMode)
{
	m_pCurAnim = pAnim;
	m_AnimPlayMode		= DestPlayMode;
	m_ManualClampTime	= rManualClampTime;

	if( m_pBody != nullptr)
	{
		m_pBody->PlayAnim( pAnim, StartTime, Interval, timeScale, rManualClampTime, BlendMode, DestPlayMode, SrcPlayMode);
		return;
	}

	INT32 i;
	i3Transform * pTrans;

	for( i = 0; i < pAnim->GetTrackCount(); i++)
	{
		pTrans = FindBoneByName( pAnim->GetTrackBoneName( i));		
		if( pTrans != nullptr)
		{
			pTrans->SetAnimation( pAnim, i, StartTime, Interval, timeScale, rManualClampTime, BlendMode, DestPlayMode, SrcPlayMode);
		}
		#if defined( I3_DEBUG)
		else
		{
			i3Log( "i3SceneObject::ApplyAnimState()", "Invalid Bone : %s\n", pAnim->GetTrackBoneName( i));
		}
		#endif
	}
}

void i3SceneObject::RefreshAnim(void)
{
	if( m_pCurAnim == nullptr)
		return;

	REAL32 tm = getAnimPlayTime( 0);

	PlayAnim( m_pCurAnim, I3_ANIM_BLEND_MODE_REPLACE, tm, 0.0f, 1.0f, m_ManualClampTime, m_AnimPlayMode);
}

void i3SceneObject::traverse( REAL32 tm, i3SceneTracer * pTracer)
{
	pTracer->SetTime( tm * m_timeScale);
}

i3Node *	i3SceneObject::AttachNodeToBone( const char * pszBoneName, i3Node * pAttachNode)
{
	INT32 i;

	if( m_pBody != nullptr)
	{
		INT32 idx;

		// i3Body Sg에 대한 검색.
		idx = m_pBody->FindBoneByName( pszBoneName);
		if( idx != -1)
		{
			// 기존에 동일 Index에 대한 i3BoneRef가 있는지 검사한다.
			
			for( i = 0; i < m_pBody->GetChildCount(); i++)
			{
				i3Node * pChild = m_pBody->GetChild( i);

				if( ! i3::same_of<i3BoneRef* >(pChild))
					continue;

				i3BoneRef * pBoneRef = (i3BoneRef *) pChild;

				if( pBoneRef->getBoneIndex() == idx)
				{

					if( pAttachNode != nullptr)
					{
						pBoneRef->AddChild( pAttachNode);
					}

					return pBoneRef;
				}
			}

			i3BoneRef * pBone = i3BoneRef::new_object_ref();
			pBone->setBoneIndex( idx);
			m_pBody->AddChild( pBone);

			if( pAttachNode != nullptr)
			{
				pBone->AddChild( pAttachNode);
			}

			return ( i3Node *)pBone;
		}
	}

	i3Transform * pBone = FindBoneByName( pszBoneName);

	if((pBone != nullptr) && (pAttachNode != nullptr))
	{
		pBone->AddChild( pAttachNode);
	}

	return pBone;
}

void i3SceneObject::CalcBoneWorldMatrix( INT32 idxLODBone, MATRIX * pMtx, INT32 lod)
{
	i3Matrix::Identity( pMtx);

	if( m_pBody == nullptr)
		return;

	m_pBody->CalcWorldMatrix( idxLODBone, pMtx);

	i3Node * pParent = m_pBody->GetParent();

	while( (pParent != nullptr) && (pParent != this))
	{
		if( i3::kind_of<i3Transform* >(pParent))
		{
			i3Transform * pTrans = (i3Transform *) pParent;

			i3Matrix::Mul( pMtx, pMtx, pTrans->GetMatrix());
		}
		pParent = pParent->GetParent();
	}

	i3Matrix::Mul( pMtx, pMtx, GetMatrix());
}

void i3SceneObject::SetPathAnimation( i3Animation * pAnim, I3_ANIM_PLAY_MODE mode)
{
	if( m_pWorldSpaceSource != nullptr)
	{
		if( i3::same_of<i3Body*>(m_pWorldSpaceSource))
		{
			I3ASSERT_0;
		}
		else
		{
			i3Transform * pTarget = (i3Transform *) m_pWorldSpaceSource;

			pTarget->SetAnimation( pAnim, 0, 0.0f, 0.0f, 1.0f, 1.0f, I3_ANIM_BLEND_MODE_REPLACE, mode);
		}
	}
	else
	{
		SetAnimation( pAnim, 0, 0.0f, 0.0f, 1.0f, 1.0f, I3_ANIM_BLEND_MODE_REPLACE, mode);
	}
}

void i3SceneObject::OnUpdate( i3SceneTracer * pTracer)
{
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	REAL32 timeOld;
	bool bNeedToUpdate = true;
	i3PhysixShapeSet * pShapeSet = nullptr;

	#if NODE_PROFILE
	START_PROFILE( 26);
	#endif

	if( (IsFlag( I3_NODEFLAG_DYNAMIC_UPDATE)) && (m_pWorldSpaceSource != nullptr))
	{
		if( (GetUpdateFrameID() != pTracer->getFrameID()))
		{
			#if defined (I3_PHYSX)
				// Dynamic 상태라면, Matrix Stack과는 상관없이 어딘가에 World-Space Matrix를
				// Update하는 Node가 있기 때문에...
				if( i3::same_of<i3Body* >(m_pWorldSpaceSource))
				{
					i3Body * pBody = (i3Body *) m_pWorldSpaceSource;
					i3LOD * pFrameLOD = pBody->getFrameLOD();
					i3Skeleton * pSkel = pFrameLOD->getSkeleton();
					I3ASSERT( pSkel != nullptr);

					INT32 i;

					for( i = 0; i < pSkel->getBoneCount(); i++)
					{
						if( pSkel->isShapeSet( i))
						{
							pShapeSet = pSkel->getShapeSet(i);
							break;
						}
					}

					I3ASSERT_RETURN( pShapeSet != nullptr);
					
					if( pShapeSet->isDynamicState( I3_PHYSIX_STATE_NO_COLLISION) == false)
					{
						pShapeSet->getShapeGlobalPose( GetCacheMatrix());

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
						I3ASSERT( isValidMatrix(GetCacheMatrix()));
#endif

						bNeedToUpdate = false;
					}
				}
				else
				{
					i3Transform2 * pTrans2 = (i3Transform2 *) m_pWorldSpaceSource;

					pShapeSet = pTrans2->getShapeSet();

					if( pShapeSet != nullptr)
					{
						pShapeSet->getShapeGlobalPose( GetCacheMatrix());

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
						I3ASSERT( isValidMatrix(GetCacheMatrix()));
#endif

						bNeedToUpdate = false;
					}
				}
			#endif

			// Update SceneObject Matrix by CacheMatrix
			SetMatrix( GetCacheMatrix() );
		}
		else
		{
			bNeedToUpdate = false;
		}
	}

	bNeedToUpdate = bNeedToUpdate && (GetUpdateFrameID() != pTracer->getFrameID());

	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedToUpdate = bNeedToUpdate || (GetUpdateFrameID() == 0xFFFFFFFF);

	// Transform process
	if( bNeedToUpdate)
	{
		if( IsAnimated())
		{
			I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

			pEnv->m_Time = pTracer->GetTime();
			pEnv->m_pIn = GetMatrix();
			pEnv->m_pOut = GetMatrix();
			pEnv->m_pModelViewMatrix = pStack->GetTop();

			GetAnimatedMatrix( pEnv);
		}

		pStack->PushPre( GetCacheMatrix(), GetMatrix());
	}
	else
	{
		// I3ASSERT( i3Matrix::isValid( GetName(), GetCacheMatrix()));
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT( isValidMatrix(GetCacheMatrix()));
#endif

		pStack->Push( GetCacheMatrix(), nullptr);
	}

	SetUpdateFrameID( pTracer->getFrameID());

	timeOld = pTracer->GetTime();

	traverse( pTracer->GetTime(), pTracer);

	if( GetChildCount() > 0)
	{
		i3Node::OnUpdate( pTracer);
	}

	pTracer->SetTime( timeOld);

	// Transform Process
	{
		pStack->Pop();
	}
}

void	i3SceneObject::PreTrace(i3SceneTracer * pTracer)
{
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	REAL32 timeOld;
	bool bNeedToUpdate = true;
	i3PhysixShapeSet * pShapeSet = nullptr;

#if NODE_PROFILE
	START_PROFILE(26);
#endif

	if ((IsFlag(I3_NODEFLAG_DYNAMIC_UPDATE)) && (m_pWorldSpaceSource != nullptr))
	{
		if ((GetUpdateFrameID() != pTracer->getFrameID()))
		{
#if defined (I3_PHYSX)
			// Dynamic 상태라면, Matrix Stack과는 상관없이 어딘가에 World-Space Matrix를
			// Update하는 Node가 있기 때문에...
			if (i3::same_of<i3Body* >(m_pWorldSpaceSource))
			{
				i3Body * pBody = (i3Body *)m_pWorldSpaceSource;
				i3LOD * pFrameLOD = pBody->getFrameLOD();
				i3Skeleton * pSkel = pFrameLOD->getSkeleton();
				I3ASSERT(pSkel != nullptr);

				INT32 i;

				for (i = 0; i < pSkel->getBoneCount(); i++)
				{
					if (pSkel->isShapeSet(i))
					{
						pShapeSet = pSkel->getShapeSet(i);
						break;
					}
				}

				I3ASSERT_RETURN(pShapeSet != nullptr);

				if (pShapeSet->isDynamicState(I3_PHYSIX_STATE_NO_COLLISION) == false)
				{
					pShapeSet->getShapeGlobalPose(GetCacheMatrix());

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
					I3ASSERT(isValidMatrix(GetCacheMatrix()));
#endif

					bNeedToUpdate = false;
				}
			}
			else
			{
				i3Transform2 * pTrans2 = (i3Transform2 *)m_pWorldSpaceSource;

				pShapeSet = pTrans2->getShapeSet();

				if (pShapeSet != nullptr)
				{
					pShapeSet->getShapeGlobalPose(GetCacheMatrix());

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
					I3ASSERT(isValidMatrix(GetCacheMatrix()));
#endif

					bNeedToUpdate = false;
				}
			}
#endif

			// Update SceneObject Matrix by CacheMatrix
			SetMatrix(GetCacheMatrix());
		}
		else
		{
			bNeedToUpdate = false;
		}
	}

	bNeedToUpdate = bNeedToUpdate && (GetUpdateFrameID() != pTracer->getFrameID());
	bNeedToUpdate = bNeedToUpdate && (this->IsFlag(I3_NODEFLAG_MANUAL_UPDATE) == false);
	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedToUpdate = bNeedToUpdate || (GetUpdateFrameID() == 0xFFFFFFFF);

	// Transform process
	if (bNeedToUpdate)
	{
		if (IsAnimated())
		{
			I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

			pEnv->m_Time = pTracer->GetTime();
			pEnv->m_pIn = GetMatrix();
			pEnv->m_pOut = GetMatrix();
			pEnv->m_pModelViewMatrix = pStack->GetTop();

			GetAnimatedMatrix(pEnv);
		}

		pStack->PushPre(GetCacheMatrix(), GetMatrix());
	}
	else
	{
		// I3ASSERT( i3Matrix::isValid( GetName(), GetCacheMatrix()));
#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
		I3ASSERT(isValidMatrix(GetCacheMatrix()));
#endif

		pStack->Push(GetCacheMatrix(), nullptr);
	}

	SetUpdateFrameID(pTracer->getFrameID());

	timeOld = pTracer->GetTime();

	traverse(pTracer->GetTime(), pTracer);

	if (GetChildCount() > 0)
	{
		i3Node::PreTrace(pTracer);
	}

	pTracer->SetTime(timeOld);

	// Transform Process
	{
		pStack->Pop();
	}

	m_PreTraceCallback();				// 등록된 함수를 호출하도록 유도한다...

}

void i3SceneObject::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Transform::CopyTo( pDest, method);
}

void i3SceneObject::OnInitAfterLoad(void)
{
	BuildBoneList();
}

static i3Node * s_RecFindWorldTransformSource( i3Node * pNode)
{
#if defined (I3_PHYSX)
	if( i3::same_of<i3Transform2* >(pNode))
	{
		return pNode;
	}
#endif

	INT32 i;
	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		i3Node * pFind = s_RecFindWorldTransformSource( pChild);
		if( pFind != nullptr)
			return pFind;
	}

	return nullptr;
}

void i3SceneObject::FindWorldTransformSource(void)
{
	m_pWorldSpaceSource = nullptr;

	if( GetBody() != nullptr)
	{
		i3LOD * pLOD = GetBody()->getFrameLOD();
#if defined (I3_PHYSX)
		if( pLOD != nullptr)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();
			I3ASSERT( pSkel != nullptr);
			
			// pSkel이 nullptr이 되는 상황이 존재하여, 임시 예외처리 추가.
			if(pSkel == nullptr)
				return;

			if( pSkel->isShapeSet( 0))
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( 0);

				// 첫번째 Bone에 반드시 ShapeSet이 있어야 한다.

				if( pShapeSet != nullptr)
					m_pWorldSpaceSource = GetBody();
			}
		}
#endif
	}
	else
	{
		if( GetBoneCount() > 0)
		{
			m_pWorldSpaceSource = s_RecFindWorldTransformSource( this);
		}
	}
}

void i3SceneObject::SetDynamicUpdateEnable( bool bFlag)
{
	FindWorldTransformSource();

	if( bFlag && (m_pWorldSpaceSource != nullptr))
	{
		AddFlag( I3_NODEFLAG_DYNAMIC_UPDATE);
	}
	else
	{
		RemoveFlag( I3_NODEFLAG_DYNAMIC_UPDATE);
	}
}
