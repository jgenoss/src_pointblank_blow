#include "i3SceneDef.h"
#include "i3Body.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"
#include "i3BoneRef.h"
#include "i3SceneUtil.h"

#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3Body);

I3_TRACE_RESULT	I3SG_NODE_PROC( i3Body)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3Body * pBody = (i3Body *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	pBody->Traverse( pTracer);

	return Result;
}

void i3Body::Traverse( i3SceneTracer * pTracer)
{
	i3LOD * pLOD;

	if( ( IsFlag( I3_NODEFLAG_MANUAL_UPDATE)) == 0)
	{
		// Manual Update를 하는 경우, 별도로 호출되어야 한다>
		if( getStyle() & I3BS_LOD_AUTO)
		{
			CalcLOD( pTracer);
		}
	}

	pTracer->PushAnimationContext( m_pAnimCtx);

	pLOD = getCurrentLOD();
	i3Skeleton * pSkel = pLOD->getSkeleton();

	// Bone Matrix List
	if( pSkel != nullptr)
	{
		if( pSkel->getLastUpdateFrameID() != pTracer->getFrameID())
		{
			pSkel->setLastUpdateFrameID( pTracer->getFrameID());

			// Skinning Geometry는 Rendering하는 환경에 따라 매우 다양한 형태로
			// 재구성 될 수 있으며, 그 구성 및 방법에 따라 설정하는 Bone Matrix가
			// 달라질 수 있기 때문에, 직접 Bone Matrix List를 적용하지 않고
			// Rendering하는 주체인 i3GeometryAttr에게 적용 실행을 맡긴다.
			if( IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == false)
			{
				if( (getStyle() & (I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM)) == 0)
				{
					pSkel->Traverse( pTracer, m_pAnimCtx, pLOD->getBoneMap());
				}
				else
				{
					if( (getStyle() & I3BS_MANUAL_ANIM_UPDATE) == 0)
						pSkel->UpdateAnim( pTracer->getFrameID(), pTracer->GetTime(), m_pAnimCtx, pLOD->getBoneMap());

					if( (getStyle() & I3BS_MANUAL_TRANSFORM) == 0)
						pSkel->TransformWorldSpace( pTracer, m_pAnimCtx, pLOD->getBoneMap());
				}

				if( m_pAnimCtx->getLastUpdateFrameID() != pTracer->getFrameID())
				{
					m_pAnimCtx->setLastUpdateFrameID( pTracer->getFrameID());
				}
			}

			pSkel->BuildTransformedMatrix( pTracer, m_pAnimCtx, pLOD->getBoneMap());
		}

		pTracer->PushBoneMatrixList( pSkel->getBoneMatrixListAttr());
	}

	// Geometry
	if( pLOD->getShapeNode() != nullptr)
	{
		i3Node * pNode = pLOD->getShapeNode();

		if( (pNode->GetFlag() & pTracer->getDisableNodeMask()) == 0)
		{
			pLOD->getShapeNode()->Trace( pTracer);
		}
	}

	if( GetChildCount() > 0)
	{
		i3Node::CallTraceProc( this, pTracer);
	}

	if( pSkel != nullptr)
	{
		pTracer->PopBoneMatrixList();
	}

	pTracer->PopAnimationContext();
}

bool i3Body::OnQueryInFrustum( i3SceneTracer * pTracer)
{
	/*
	bool bResult, bPop = false;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	if((m_pBoneFrameLOD != nullptr) && (m_pBoneFrameLOD->getSkeleton()->getBoneCount() > 0))
	{
		i3Skeleton * pSkel = m_pBoneFrameLOD->getSkeleton();

		if( pSkel->isDynamic() == false)
		{
			// Dynamic State인 경우에는 상위의 i3SceneObject에서 이미 Matrix를 Update하였기 때문에
			// 자신은 여기서 반영하지 말아야 한다.
			i3Bone * pBone = pSkel->getBone( 0);

			MATRIX mtx;

			pStack->PushPre( &mtx, pBone->getMatrix());

			bPop = true;
		}
	}

	bResult = pTracer->IsInsideFrustum( GetBound());


	if( bPop)
	{
		pStack->Pop();
	}

	return bResult;
	*/

	return i3Node::OnQueryInFrustum( pTracer);
}

void	i3Body::PreTrace(i3SceneTracer * pTracer)
{
	i3LOD * pLOD;

	if ((IsFlag(I3_NODEFLAG_MANUAL_UPDATE)) == 0)
	{
		if (getStyle() & I3BS_LOD_AUTO)
		{
			CalcLOD(pTracer);
		}
	}

	pLOD = getCurrentLOD();

	pTracer->PushAnimationContext(m_pAnimCtx);

	i3Skeleton * pSkel = pLOD->getSkeleton();

	// Bone Matrix List
	if (pSkel != nullptr)
	{
		bool bNeedUpdate = (pSkel->getLastUpdateFrameID() != pTracer->getFrameID());

		if (bNeedUpdate)	// 아마도 모든게 이미 기처리되었을 것으로 보이므로 그냥 통으로 스킵..
		{
			pSkel->setLastUpdateFrameID(pTracer->getFrameID());

			// Skinning Geometry는 Rendering하는 환경에 따라 매우 다양한 형태로
			// 재구성 될 수 있으며, 그 구성 및 방법에 따라 설정하는 Bone Matrix가
			// 달라질 수 있기 때문에, 직접 Bone Matrix List를 적용하지 않고
			// Rendering하는 주체인 i3GeometryAttr에게 적용 실행을 맡긴다.
			if (IsFlag(I3_NODEFLAG_MANUAL_UPDATE) == false)
			{
				if ((getStyle() & (I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM)) == 0)
				{
					pSkel->Traverse(pTracer, m_pAnimCtx, pLOD->getBoneMap());
				}
				else
				{
					if ((getStyle() & I3BS_MANUAL_ANIM_UPDATE) == 0)
						pSkel->UpdateAnim(pTracer->getFrameID(), pTracer->GetTime(), m_pAnimCtx, pLOD->getBoneMap());

					if ((getStyle() & I3BS_MANUAL_TRANSFORM) == 0)
						pSkel->TransformWorldSpace(pTracer, m_pAnimCtx, pLOD->getBoneMap());
				}
				m_pAnimCtx->setLastUpdateFrameID(pTracer->getFrameID());
			}


			pSkel->BuildTransformedMatrix(pTracer, m_pAnimCtx, pLOD->getBoneMap());
		}

		pTracer->PushBoneMatrixList(pSkel->getBoneMatrixListAttr());

	}

	i3Node * pShape = pLOD->getShapeNode();

	if (pShape && (pShape->GetFlag() & pTracer->getDisableNodeMask()) == 0)
	{
		pShape->PreTrace(pTracer);
	}

	if (GetChildCount() > 0)
		i3Node::PreTrace(pTracer);

	if (pSkel != nullptr)
	{
		pTracer->PopBoneMatrixList();
	}

	pTracer->PopAnimationContext();
}


void i3Body::OnUpdate( i3SceneTracer * pTracer)
{
	i3LOD * pLOD;

	if( getStyle() & I3BS_LOD_AUTO)
	{
		CalcLOD( pTracer);
	}

	pLOD = getCurrentLOD();

	pTracer->PushAnimationContext( m_pAnimCtx);

	i3Skeleton * pSkel = pLOD->getSkeleton();

	// Bone Matrix List
	if( pSkel != nullptr)
	{
		// Skinning Geometry는 Rendering하는 환경에 따라 매우 다양한 형태로
		// 재구성 될 수 있으며, 그 구성 및 방법에 따라 설정하는 Bone Matrix가
		// 달라질 수 있기 때문에, 직접 Bone Matrix List를 적용하지 않고
		// Rendering하는 주체인 i3GeometryAttr에게 적용 실행을 맡긴다.

		if( (getStyle() & (I3BS_MANUAL_ANIM_UPDATE | I3BS_MANUAL_TRANSFORM)) == 0)
		{
			pSkel->Traverse( pTracer, m_pAnimCtx, pLOD->getBoneMap());
		}
		else
		{
			if( (getStyle() & I3BS_MANUAL_ANIM_UPDATE) == 0)
				pSkel->UpdateAnim( pTracer->getFrameID(), pTracer->GetTime(), m_pAnimCtx, pLOD->getBoneMap());

			pSkel->TransformWorldSpace( pTracer, m_pAnimCtx, pLOD->getBoneMap());
		}

		pSkel->setLastUpdateFrameID(pTracer->getFrameID());
		m_pAnimCtx->setLastUpdateFrameID(pTracer->getFrameID());
		pSkel->BuildTransformedMatrix(pTracer, m_pAnimCtx, pLOD->getBoneMap());
	}

	i3Node * pShape = pLOD->getShapeNode();
	pShape->OnUpdate(pTracer);

	i3Node::OnUpdate( pTracer);

	pTracer->PopAnimationContext();
}

void i3Body::UpdateAnim( i3SceneTracer * pTracer)
{
	i3LOD * pLOD;

	if( getStyle() & I3BS_LOD_AUTO)
	{
		CalcLOD( pTracer);
	}

	pLOD = getCurrentLOD();

	i3Skeleton * pSkel = pLOD->getSkeleton();
	I3ASSERT( pSkel != nullptr);
	pSkel->UpdateAnim( pTracer->getFrameID(), pTracer->GetTime(), m_pAnimCtx, pLOD->getBoneMap());
}

///////////////////////////////////////////////////////////////////////////////////////
//
i3Body::i3Body(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Body));
	
	m_LODList.reserve(4);
}

i3Body::~i3Body(void)
{
	I3_SAFE_RELEASE( m_pAnimCtx);
	I3_SAFE_RELEASE( m_pCurAnim);
	RemoveAllLOD();
}

void i3Body::AddLOD( i3LOD * pLOD)
{
	I3_MUST_ADDREF( pLOD);
	m_LODList.push_back( pLOD);

#if defined( I3_DEBUG)
	pLOD->setBody(this);
#endif
}

void i3Body::RemoveLOD( i3LOD * pLOD)
{
	I3ASSERT( pLOD != nullptr);

	size_t idx = i3::vu::index_of(m_LODList, pLOD);
	I3ASSERT( idx != m_LODList.size() );

#if defined( I3_DEBUG)
	pLOD->setBody(nullptr);
#endif

	I3_MUST_RELEASE( pLOD);

	m_LODList.erase(m_LODList.begin() + idx);
}

void i3Body::RemoveAllLOD(void)
{
	UINT32 i;
	i3LOD * pLOD;

	for( i = 0; i < getLODCount(); i++)
	{
		pLOD = getLOD( i);
#if defined( I3_DEBUG)
		pLOD->setBody(nullptr);
#endif

		I3_MUST_RELEASE( pLOD);
	}

	m_LODList.clear();
}

void i3Body::CalcLOD( i3SceneTracer * pTracer)
{
	if( pTracer->isState( I3SG_TRACER_STATE_OFFSCREEN))
	{
		//  그림자 등을 위해, Off-Screen으로 Rendering하는 경우...
		m_CurLevel = getLODCount() - 1;
		return;
	}

	CalcLOD( i3Matrix::GetPos( pTracer->GetModelViewMatrixStack()->GetTop()),
							i3Matrix::GetPos( pTracer->GetInverseViewMatrix()));
}

void i3Body::CalcLOD( VEC3D * pPos, VEC3D * pCamPos, REAL32 fFOVRatio)
{
	REAL32 span;
	INT32 newLv;
	VEC3D diff;

	i3Vector::Sub( &diff, pPos, pCamPos);

	span = i3Vector::Length( &diff) / fFOVRatio;

	newLv = (INT32)((m_Slope * span * (1.0f / 20.0f)) + m_Const);

	if( newLv < 0)
			newLv = 0;
		else if( newLv > (((INT32) getLODCount() - 1)))
			newLv = getLODCount() - 1;

	m_CurLevel = newLv;
}

void i3Body::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	INT32 i;
	i3LOD * pLOD;

	if( m_pBoneFrameLOD != nullptr)
	{
		pInfo->m_pSkel = m_pBoneFrameLOD->getSkeleton();
		I3ASSERT( pInfo->m_pSkel != nullptr);
	}

	for( i = 0; i < (INT32) getLODCount(); i++)
	{
		pLOD = getLOD( i);

		if( pLOD->getShapeNode() != nullptr)
		{
			pLOD->getShapeNode()->_GetWrappingBound( pInfo);

		}
	}

	pInfo->m_pSkel = nullptr;
}

void i3Body::PlayAnim( i3Animation * pAnim, 
						REAL32 rStartTime,
						REAL32 interval, 
						REAL32 timeScale,
						REAL32 rManualClampTime,
						I3_ANIM_BLEND_MODE blendMode, 
						I3_ANIM_PLAY_MODE destPlayMode, 
						I3_ANIM_PLAY_MODE srcPlayMode)
{
	INT32 i;

	I3ASSERT( pAnim != nullptr);

	I3_REF_CHANGE( m_pCurAnim, pAnim);

	if( m_pAnimCtx == nullptr)
	{
		_PrepareAnimContext();
		I3ASSERT_RETURN(m_pAnimCtx);
	}

	if( m_pBoneFrameLOD != nullptr)
	{
		i3Skeleton * pSkel = m_pBoneFrameLOD->getSkeleton();
		I3ASSERT( pSkel != nullptr);

		for( i = 0; i < pAnim->GetTrackCount(); i++)
		{
			INT32 idx;

			idx = pSkel->FindBoneByName( pAnim->GetTrackBoneName( i));
			if( idx != -1)
			{
				i3TransformSourceCombiner * pCombiner = m_pAnimCtx->getCombiner( idx);
				pCombiner->SetTimeScale( timeScale);
				
				pCombiner->AddAnimation( pAnim, i, rStartTime, 
								interval, rManualClampTime, blendMode, destPlayMode, srcPlayMode);
			}
		}
	}
}

void i3Body::StopAnim(void)
{
	INT32 i;

	if( m_pAnimCtx == nullptr)
		return;

	for( i = 0; i < m_pAnimCtx->getCombinerCount(); i++)
	{
		i3TransformSourceCombiner * pCombiner = m_pAnimCtx->getCombiner( i);

		pCombiner->RemoveAllAnimation();
	}
	
	I3_SAFE_RELEASE( m_pCurAnim);
}

void i3Body::SetPlayAnim( REAL32 tm)
{
	INT32 i;

	if( m_pAnimCtx == nullptr)
		return;

	for( i = 0; i < m_pAnimCtx->getCombinerCount(); i++)
	{
		i3TransformSourceCombiner * pCombiner = m_pAnimCtx->getCombiner( i);

		pCombiner->SetPlayTime( tm);
	}
}

REAL32 i3Body::GetDuration( void)
{
	INT32 i;
	REAL32 maxdur = 0.0f, dur;

	if( m_pAnimCtx == nullptr)
		return 0.0f;

	for( i = 0; i < m_pAnimCtx->getCombinerCount(); i++)
	{
		i3TransformSourceCombiner * pCombiner = m_pAnimCtx->getCombiner( i);

		dur = pCombiner->GetDuration();
		if( dur > maxdur)
			maxdur = dur;
	}

	return maxdur;
}

REAL32 i3Body::GetActualDuration( void)
{
	INT32 i;
	REAL32 maxdur = 0.0f, dur;

	if( m_pAnimCtx == nullptr)
		return 0.0f;

	for( i = 0; i < m_pAnimCtx->getCombinerCount(); i++)
	{
		i3TransformSourceCombiner * pCombiner = m_pAnimCtx->getCombiner( i);

		dur = pCombiner->GetActualDuration();
		if( dur > maxdur)
			maxdur = dur;
	}

	return maxdur;
}

/*REAL32 i3Body::GetPlayTimeByName( const char * pszName)
{
	INT32 idx = FindBoneByName( pszName);
	return m_pAnimCtx->getCombiner( idx)->getPlayTime();
}*/

void i3Body::CalcWorldMatrix( INT32 nBoneIndex, MATRIX * pMtx)
{
	i3Bone * pBone;
	i3Skeleton * pSkel = m_pBoneFrameLOD->getSkeleton();
	I3ASSERT( pSkel != nullptr);

	i3Matrix::Identity( pMtx);

	while( nBoneIndex != -1)
	{
		pBone = pSkel->getBone( nBoneIndex);

		i3Matrix::Mul( pMtx, pMtx, pBone->getMatrix());

		nBoneIndex = pBone->getParentIndex();
	}
}

void i3Body::OnChangeTime( REAL32 tm)
{
	SetPlayAnim( tm);

	i3Node::OnChangeTime( tm);
}

void	i3Body::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	UINT32 i;

	i3Node::CopyTo( pDest, method);

	I3ASSERT( i3::same_of<i3Body*>(pDest));
	
	i3Body * pBody = (i3Body *) pDest;

	pBody->setSlope( getSlope());
	pBody->setConst( getConst());
	pBody->setStyle( getStyle());

	if( method == I3_COPY_REF)
	{
		for( i = 0; i < getLODCount(); i++)
		{
			pBody->AddLOD( getLOD( i));
		}
	}
	else
	{
		if( m_pBoneFrameLOD != nullptr)
		{
			// 우선 BoneFrameLOD를 복사한다.
			i3LOD * pNewBoneFrameLOD = i3LOD::new_object_ref();

			m_pBoneFrameLOD->CopyTo( pNewBoneFrameLOD, method);

			for( i = 0; i < getLODCount(); i++)
			{
				if( getLOD(i) == m_pBoneFrameLOD)
				{
					// Bone Frame이니까...직접 설정
					pBody->AddLOD( pNewBoneFrameLOD);
				}
				else
				{
					i3LOD * pNewLOD = i3LOD::new_object_ref();

					if( getLOD(i)->getSkeleton() == m_pBoneFrameLOD->getSkeleton())
					{
						pNewLOD->setSharedSkeleton( pNewBoneFrameLOD->getSkeleton());
					}

					getLOD(i)->CopyTo( pNewLOD, method);

					pBody->AddLOD( pNewLOD);
				}
			}
		}
		else
		{
			for( i = 0; i < getLODCount(); i++)
			{
				i3LOD * pNewLOD = i3LOD::new_object_ref();

				getLOD( i)->CopyTo( pNewLOD, method);

				pBody->AddLOD( pNewLOD);
			}
		}
	}

	pBody->_PrepareAnimContext();

	if( m_pCurAnim != nullptr)
	{
		pBody->PlayAnim( m_pCurAnim);
	}
}

void	i3Body::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	UINT32 i;
	i3LOD * pLOD;

	for( i = 0; i < getLODCount(); i++)
	{
		pLOD = getLOD( i);

		pLOD->OnBuildObjectList( List);
	}

	if( m_pCurAnim != nullptr)
	{
		m_pCurAnim->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 SKIN2
	{
		UINT8		m_ID[4] = { 'S', 'K', 'N', '2' };
		REAL32		m_Slope = 0.0f;
		REAL32		m_Const = 0.0f;
		OBJREF		m_pCurAnim = 0;
		UINT32		m_Style = 0;
		UINT8		pad[28] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3Body::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SKIN2 data;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Data
	{
		data.m_Slope = getSlope();
		data.m_Const = getConst();
		data.m_pCurAnim = (OBJREF) pResFile->GetObjectPersistID( m_pCurAnim);
		data.m_Style = getStyle();

		Rc = pStream->Write( &data, sizeof(data));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write skin data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Skeletons
	{
		Rc = SaveFromListToResourceFile(m_LODList, pResFile); //	m_LODList.SaveTo( pResFile);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write skeleton list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32	i3Body::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SKIN2 data;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Data
	{
		Rc = pStream->Read( &data, sizeof(data));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not read skin data.");
			return STREAM_ERR;
		}
		Result += Rc;

		setSlope( data.m_Slope);
		setConst( data.m_Const);
		setStyle( data.m_Style);
	}

	// Skeletons
	{
		Rc = LoadFromResourceFileToList(m_LODList, pResFile, true);				// m_LODList.LoadFrom( pResFile, true);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not read skeleton list data.");
			return STREAM_ERR;
		}
		Result += Rc;

#if defined( I3_DEBUG)
		for( UINT32 i = 0; i < m_LODList.size(); i++)
		{
			i3LOD * pLOD = m_LODList.at(i);
			pLOD->setBody( this);
		}
#endif
	}
	
	_PrepareAnimContext();

	// Animation
	if( data.m_pCurAnim != 0)
	{
		i3Animation * pAnim = (i3Animation *) pResFile->FindObjectByID( data.m_pCurAnim);

		if( pAnim != nullptr)
		{
			PlayAnim( pAnim);
		}
	}

	return Result;
}

void i3Body::_PrepareAnimContext( bool bRecreateAnimCtx)
{
	UINT32 i, maxbone = 0;
	i3LOD * pLOD;

	if( bRecreateAnimCtx)
	{
		m_pBoneFrameLOD = nullptr;

		for( i = 0; i < getLODCount(); i++)
		{
			pLOD = getLOD( i);

			UINT32 boneCount = pLOD->getBoneCount();

			i3Skeleton * pSkel = pLOD->getSkeleton();
			if( pSkel != nullptr)
			{
				// Bone의 개수가 가장 많은 LOD,
				// 만약 Bone의 개수가 같다면, ShapeSet을 가지고 있는 Bone쪽을
				// Frame Bone Hierarchy라고 간주한다.
				// 
				if( boneCount > maxbone)
				{
					m_pBoneFrameLOD = pLOD;
					maxbone = boneCount;
				}
				else if( boneCount == maxbone)
				{
					INT32 shapeSetCnt = pLOD->hasShapeSet();			

					if( shapeSetCnt > 0)
					{
						m_pBoneFrameLOD = pLOD;
						maxbone = boneCount;
					}
				}
			}
		}
	}

	//I3TRACE( "ShapeSetCnt : %d\n", m_pBoneFrameLOD->hasShapeSet());

	// Animation Context
	if( m_pBoneFrameLOD != nullptr)
	{
		I3_SAFE_RELEASE( m_pAnimCtx);

		m_pAnimCtx = i3AnimationContext::new_object();
		
		m_pAnimCtx->Create( m_pBoneFrameLOD->getSkeleton());
	}
}

void i3Body::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	UINT32 i;
	MATRIX mtx;
	bool bPushed = false;

	I3ASSERT( pInfo != nullptr);
	I3ASSERT( pInfo->_pMatrixStack != nullptr);

	if( m_pBoneFrameLOD != nullptr)
	{
		i3Skeleton * pSkel = m_pBoneFrameLOD->getSkeleton();
		if( pSkel != nullptr && pSkel->getBoneCount() > 0)
		{
			pInfo->_pMatrixStack->PushPre( &mtx, pSkel->getBone( 0)->getMatrix());

			bPushed = true;
		}
	}

	for( i = 0; i < getLODCount(); i++)
	{
		i3LOD * pLOD = getLOD( i);

		pLOD->OnInstancing( pInfo);
	}

	if( m_pAnimCtx == nullptr)
	{
		_PrepareAnimContext();
	}

	i3Node::OnInstancing( pInfo);

	if( bPushed )
	{
		pInfo->_pMatrixStack->Pop();
	}
}

void i3Body::_CreateRenderableTransform( i3Transform * pAttachNode )
{
	char szName[ MAX_NAME];
	i3AttrSet * pAttrSet = i3AttrSet::new_object_ref();
	I3ASSERT( pAttrSet != nullptr);

	pAttrSet->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL | I3_NODEFLAG_TRANSPARENT | I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
	i3::snprintf( szName, sizeof( szName), "Debug_RT_%d", m_RenderableTransformList.size());
	pAttrSet->SetName( szName);

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set(false);
		pAttrSet->AddAttr( pAttr);
	}

	{
		COLOR color;
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

		i3Color::Set( &color, (UINT8)0, 255, 0, 255);
		pAttr->Set( &color);
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set(false);
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set(false);
		pAttrSet->AddAttr( pAttr);
	}

	{
		i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
		pAttr->setBias( 0.f);
		pAttrSet->AddAttr( pAttr);
	}

	i3Node * pMesh = i3Scene::BuildBoxMesh( 1.0f, false);
	pAttrSet->AddChild( pMesh);
	I3_SAFE_RELEASE( pMesh);

	MATRIX mtx;
	i3Transform * pTrans = i3Transform::new_object_ref();
	
	i3Matrix::SetScale( &mtx, 0.01f, 0.01f, 0.01f);
	pTrans->SetMatrix( &mtx);
	pTrans->AddChild( pAttrSet);
	pTrans->AddFlag( I3_NODEFLAG_VOLATILE);

	pAttachNode->AddChild( pTrans);

	m_RenderableTransformList.push_back( pTrans);
}

void i3Body::_FindAndCreateRenderableBone( i3Node * pRoot )
{
	INT32 i;

	// Transform을 찾는다.
	if( i3::kind_of<i3Transform*>(pRoot))
	{
		// 마지막 Child인가?
		if( pRoot->GetChildCount() == 0)
		{
			// 부모는 BoneRef인가?
			i3Node * pParent = pRoot->GetParent();
			while( pParent != nullptr)
			{
				if( i3::same_of<i3BoneRef* >(pParent))
				{
					// Create Renderable bone for transform
					_CreateRenderableTransform( (i3Transform*)pRoot);
					break;
				}

				pParent = pParent->GetParent();
			}
		}
	}


	for( i = 0; i < pRoot->GetChildCount(); i++)
	{
		_FindAndCreateRenderableBone( pRoot->GetChild( i));
	}
}

void i3Body::CreateRenderableBoneRef( void )
{
	m_RenderableTransformList.clear();

	_FindAndCreateRenderableBone( this);
}

void i3Body::CreateRenderableBone( void )
{
	INT32 i, j;

	if( getFrameLOD() == nullptr)
		return;

	i3Skeleton * pFrameSkel = getFrameLOD()->getSkeleton();
	I3ASSERT( pFrameSkel != nullptr);

	m_pRenderableBone = i3AttrSet::new_object_ref();
	I3ASSERT( m_pRenderableBone);

	m_pRenderableBone->AddFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL | I3_NODEFLAG_TRANSPARENT | I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
	m_pRenderableBone->SetName( "Debug_RenderableBone");

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pRenderableBone->AddAttr( pAttr);
	}

	{
		COLOR color;
		i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();

		i3Color::Set( &color, (UINT8)255, 255, 255, 255);
		pAttr->Set( &color);
		m_pRenderableBone->AddAttr( pAttr);
	}

	{
		i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pRenderableBone->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
		pAttr->Set(false);
		m_pRenderableBone->AddAttr( pAttr);
	}

	{
		i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
		pAttr->setBias( 0.f);
		m_pRenderableBone->AddAttr( pAttr);
	}

	m_iRenderableBoneCount = pFrameSkel->getBoneCount();

	for( i = 0; i < m_iRenderableBoneCount; i++)
	{
		REAL32 rMaxLen = 0.f;
		VEC3D vDir;

		for( j = 0; j < m_iRenderableBoneCount; j++)
		{
			if( i == j)
				continue;

			if( pFrameSkel->getBone( j)->getParentIndex() == i)
			{
				i3Bone * pChildBone = pFrameSkel->getBone( j);

				REAL32 rLen = i3Vector::Length( i3Matrix::GetPos( pChildBone->getMatrix()) );

				if( rMaxLen < rLen)
				{
					i3Vector::Copy( &vDir, i3Matrix::GetPos( pChildBone->getMatrix()));
					rMaxLen = rLen;
				}
			}
		}

		i3BoneRef * pBoneTrans = i3BoneRef::new_object_ref();
		I3ASSERT( pBoneTrans != nullptr);

		pBoneTrans->SetName( pFrameSkel->getBone( i)->getNameString());
		pBoneTrans->AddFlag( I3_NODEFLAG_VOLATILE);
		pBoneTrans->setBoneIndex( i);

		// 다이아몬드 추가
		CreateRenderDiamond( pBoneTrans, &vDir, (rMaxLen == 0.f));
		// Axis 화살표 추가
		CreateRenderAxis( pBoneTrans);

		m_pRenderableBone->AddChild( pBoneTrans);
	}

	AddChild( m_pRenderableBone);
}

void i3Body::CreateRenderDiamond( i3Node * pAttachNode, VEC3D * pDir, bool bLeaf )
{
	char szNodeName[ MAX_NAME];
	VEC3D vPos;
	COLORREAL color;
	i3VertexFormat format;

	format.SetHasPosition( true);
	format.SetHasColor( true);

	{
		// Geometry
		i3GeometryAttr * pGeoAttr = i3GeometryAttr::new_object_ref();

		i3VertexArray * pVA = i3VertexArray::new_object_ref();
		pVA->Create( &format, 2, 0);

		{
			pVA->Lock();

			i3Vector::Set( &vPos, 0.f, 0.f, 0.f);
			pVA->SetPosition( 0, &vPos);

			if( bLeaf )
			{
				i3Vector::Set( &vPos, 0.05f, 0.f, 0.f);
				i3Color::Set( &color, 0.5f, 0.5f, 0.8f, 0.8f);
			}
			else
			{
				i3Vector::Copy( &vPos, pDir);
				i3Color::Set( &color, 0.5f, 0.5f, 0.5f, 0.8f);
			}

			pVA->SetPosition( 1, &vPos);
			pVA->SetColor( 0, &color);
			pVA->SetColor( 1, &color);

			pVA->Unlock();
		}

		pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0, false );

		i3Geometry * pGeo = i3Geometry::new_object_ref();

		i3::string_ncopy_nullpad( szNodeName, "renderable_Bone\0", MAX_NAME);

		pGeo->SetName( szNodeName);

		pGeo->AppendGeometryAttr( pGeoAttr);

		pAttachNode->AddChild( pGeo);

		{
			i3Transform * pTransScale = i3Transform::new_object_ref();
			i3Matrix::Identity( pTransScale->GetMatrix());
			i3Matrix::SetScale( pTransScale->GetMatrix(), 0.015f, 0.015f, 0.015f);
			i3Color::Set( &color, 0.9f, 0.3f, 0.3f, 0.8f);

			i3Node * pSphereMesh = i3Scene::BuildSphereMesh( 1.0f, true, true, 6, 3, &color, &color, &color);

			pTransScale->AddChild( pSphereMesh);
			pTransScale->AddFlag( I3_NODEFLAG_VOLATILE);

			I3_SAFE_RELEASE( pSphereMesh);

			pAttachNode->AddChild( pTransScale);
		}
	}
}

void i3Body::CreateRenderAxis( i3Node * pAttachNode, INT32 AxisDiv /*= 20*/, REAL32 Length /*= 1.0f*/, REAL32 rad /*= 0.05f*/ )
{
	i3Node * pNode;
	i3AttrSet * pAxisRoot;
	i3BoundBox * pBoundBox;
	INT32 i;

	i3Transform *	m_pAxis[3];
	i3AttrSet *		m_pAxisAttrSet[3];
	i3ColorAttr *	m_pAxis_ColorAttr[3];

	pAxisRoot = i3AttrSet::new_object_ref();		// pAxisRoot변수는 곧 없어질것이니까...new_object_ref로 해야함..(2012.09.11.수빈)
	pAxisRoot->SetName( "Axis" );
	pAxisRoot->SetPriority( 0);
	
	pAttachNode->AddChild( pAxisRoot );

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set(false);
		pAxisRoot->AddAttr( pAttr);
	}

	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();

		pAttr->Set(false);
		pAxisRoot->AddAttr( pAttr);
	}

	COLOR	s_AxisColor[3] =
	{
		COLOR(	255, 0, 0, 255 ),
		COLOR(	0, 255, 0, 255 ),
		COLOR(	0, 0, 255, 255 )
	};

	for( i = 0; i < 3; i++)
	{
		i3Transform * pTransScale = i3Transform::new_object_ref();
		i3Matrix::Identity( pTransScale->GetMatrix());
		i3Matrix::SetScale( pTransScale->GetMatrix(), 0.1f, 0.1f, 0.1f);

		pAxisRoot->AddChild(pTransScale);

		m_pAxis[i] = i3Transform::new_object_ref();
		pTransScale->AddChild( m_pAxis[i]);

		m_pAxisAttrSet[i] = i3AttrSet::new_object_ref();
		m_pAxis[i]->AddChild( m_pAxisAttrSet[i]);

		{
			m_pAxis_ColorAttr[i] = i3ColorAttr::new_object_ref();
			m_pAxis_ColorAttr[i]->Set( &s_AxisColor[i]);
			m_pAxisAttrSet[i]->AddAttr( m_pAxis_ColorAttr[i]);
		}

		{
			pNode = i3Scene::BuildAxisMesh( AxisDiv, Length, rad, true, false, nullptr, nullptr); //&s_InitAxisInfo[i], &s_InitAxisInfo[i]);

			m_pAxisAttrSet[i]->AddChild( pNode);
			I3_MUST_RELEASE(pNode);

			pBoundBox = i3BoundBox::new_object_ref();
			pBoundBox->Reset();
			m_pAxisAttrSet[i]->GetWrappingBound( pBoundBox);
			m_pAxisAttrSet[i]->SetBound( pBoundBox);
		}
	}

	

	i3Matrix::SetRotateZDeg( m_pAxis[1]->GetMatrix(), 90.0f);
	i3Matrix::SetRotateYDeg( m_pAxis[2]->GetMatrix(), 90.0f);
}

void i3Body::EnableRenderBone( bool bEnable )
{
	if( m_pRenderableBone == nullptr)
	{
		CreateRenderableBoneRef();
		CreateRenderableBone();
	}

	if( bEnable)
	{
		m_pRenderableBone->RemoveFlag( I3_NODEFLAG_DISABLE);
		for(size_t i = 0; i < m_RenderableTransformList.size(); i++)
		{
			i3Node * pNode = m_RenderableTransformList[ i];
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
		}
	}
	else
	{
		m_pRenderableBone->AddFlag( I3_NODEFLAG_DISABLE);
		for(size_t i = 0; i < m_RenderableTransformList.size(); i++)
		{
			i3Node * pNode = m_RenderableTransformList[ i];
			pNode->AddFlag( I3_NODEFLAG_DISABLE);
		}
	}

	m_bEnableRenderBone = bEnable;
}