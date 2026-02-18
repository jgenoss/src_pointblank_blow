#include "i3SceneDef.h"
#include "i3Transform.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"

I3_CLASS_INSTANCE( i3Transform);

I3_TRACE_RESULT I3SG_NODE_PROC( i3Transform)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	// Dummy transform은 업데이트를 해야한다. 2015.02.11 swoongoo
	//if( pNode->GetChildCount() > 0)
	{
		i3Transform * pTrans = (i3Transform *) pNode;
		i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
		i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();

		// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
		if( ((pNode->IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == false) && (pTracer->getFrameID() != pTrans->GetUpdateFrameID()))
			|| (pTrans->GetUpdateFrameID() == 0xFFFFFFFF))
		{
			// 이미 한번 Update가 된 상황이면 Animation 등을 계산하지 않고
			// 예전 Update된 Cache Matrix를 그냥 활용한다.
			pTrans->SetUpdateFrameID( pTracer->getFrameID());

			if( pTrans->IsAnimated())
			{
				I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

				pEnv->m_Time = pTracer->GetTime();
				pEnv->m_pIn = pTrans->GetMatrix();
				pEnv->m_pOut = pTrans->GetMatrix();
				pEnv->m_pModelViewMatrix = pStack->GetTop();

				pTrans->GetAnimatedMatrix( pEnv);
			}

			pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
		}
		else
		{
			// I3ASSERT( i3Matrix::isValid( pTrans->GetName(), pTrans->GetCacheMatrix()));
	#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
			if( pTrans->IsFlag( I3_NODEFLAG_MANUAL_UPDATE))
			{
				I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
			}
			else if( pTracer->getFrameID() == pTrans->GetUpdateFrameID())
			{
				I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
			}
			else
			{
				I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
			}
	#endif
			pStack->Push( pTrans->GetCacheMatrix(), nullptr);
		}
		
		{
			// 자신의 Model View Matrix를 tracer에 등록해 둔다.
			register i3ModelViewMatrixAttr * pAttr = pTracer->AllocModelViewMatrixAttr();

			_copyTop( pAttr->GetMatrix(), pStack);

			pStackManager->PushAttr( pAttr);
		}


		if( pNode->GetChildCount() > 0)
		{
			Result = i3Node::CallTraceProc( pNode, pTracer);
		}


		pStackManager->PopAttr( I3_ATTRID_MODEL_VIEW_MATRIX);

		pStack->Pop();
	}

	return Result;
}

i3Transform::i3Transform(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Transform));

	m_pMatrixObject = i3MatrixObject::new_object();
	
	AddFlag( I3_NODEFLAG_TRANSFORM);
}

i3Transform::~i3Transform(void)
{
	I3_SAFE_RELEASE( m_pMatrixObject);
	I3_SAFE_RELEASE( m_pTransCombiner);
}

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX

bool i3Transform::isValidMatrix(MATRIX * pMat)
{
	INT32 i, j;

	bool bValid = true;

	for( i = 0; i < 4; i++)
	{
		for( j = 0; j < 4; j++)
		{
			if( i3Math::isValid( pMat->m[i][j]) == false)
			{
				i3Node* pParent = this;
				while(pParent != nullptr)
				{
					I3PRINTLOG(I3LOG_WARN, "!!!!!!!! i3Transform::isValidMatrix %s %.3f i %d j %d", pParent->GetName(), ( *((UINT32 *) &(pMat->m[i][j]) ) ), i, j );
					pParent = pParent->GetParent();

				}
				
//#ifdef FILL_ZERO_HEAP_AND_PHYSIX_DATA
//				pMat->m[i][j] = 0.f;
//#else
//				int* iii = nullptr;
//				*iii = 10;
//#endif

				bValid = false;
				break;
			}
		}

		if( !bValid)
			break;
	}

	if( !bValid)
	{
		i3Matrix::Identity( pMat);
	}

	return bValid;
}

#endif

void i3Transform::SetMatrixObject( i3MatrixObject * pObj)
{
	I3_REF_CHANGE( m_pMatrixObject, pObj);
}

void i3Transform::SetMatrix( MATRIX * pMatrix)
{
	I3ASSERT( m_pMatrixObject != nullptr);

	m_pMatrixObject->SetMatrix( pMatrix);
}

void i3Transform::SetPos( const VEC3D & pos)
{
	i3Matrix::SetPos( m_pMatrixObject->GetMatrix(), (VEC3D *) &pos);
}

void i3Transform::SetPlayCount(UINT32 count)
{
	if( m_pTransCombiner != nullptr)
	{
		m_pTransCombiner->SetPlayCount(count);		
	}
}

void i3Transform::SetPlayTime( REAL32 tm)
{
	if( m_pTransCombiner != nullptr)
	{
		m_pTransCombiner->SetPlayTime( tm);		
	}
}

void i3Transform::AddPlayTime( REAL32 tm)
{
	if( m_pTransCombiner != nullptr)
	{
		m_pTransCombiner->AddPlayTime( tm);
	}
}

void i3Transform::SetAnimation( i3Animation * pAnim,
									 INT32	idxBone,
									 REAL32 rStartTime,
									 REAL32 Interval,
									 REAL32 timeScale,
									 REAL32 rManualClampTime,
									 I3_ANIM_BLEND_MODE BlendMode,
									 I3_ANIM_PLAY_MODE destPlayMode,
									 I3_ANIM_PLAY_MODE srcPlayMode)
{
	if( m_pTransCombiner == nullptr)
	{
		m_pTransCombiner = i3TransformSourceCombiner::new_object();
	}

	m_pTransCombiner->SetTimeScale( timeScale);
	
	if(pAnim == nullptr)
		m_pTransCombiner->RemoveAllAnimation(); 
	else 
		m_pTransCombiner->AddAnimation( pAnim, idxBone, rStartTime, Interval, rManualClampTime, BlendMode, destPlayMode, srcPlayMode);
}

void i3Transform::GetAnimatedMatrix( I3MATRIXINFO * pInfo)
{
	if( m_pTransCombiner != nullptr)
	{
		m_pTransCombiner->GetMatrix( pInfo);
	}
	else
	{
		i3Matrix::Copy( pInfo->m_pOut, m_pMatrixObject->GetMatrix());
	}
}

void i3Transform::CalcUpdatedMatrix( MATRIX * pOut, i3SceneTracer * pTracer)
{
	I3ASSERT( pOut != nullptr);
	I3ASSERT( pTracer != nullptr);

	I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();
	MATRIX tmp;
	i3Transform * pTrans;
	i3Node * pNode = GetParent();

	pEnv->m_FrameID = pTracer->getFrameID();
	pEnv->m_Time = pTracer->GetTime();
	pEnv->m_pModelViewMatrix = pTracer->GetModelViewMatrixStack()->GetTop();
	pEnv->m_pIn = &tmp;

	// 현재 Transform Node...
	pEnv->m_pOut = pOut;
	GetAnimatedMatrix( pEnv);

	pEnv->m_pOut = &tmp;

	while( pNode != nullptr)
	{
		if( i3::kind_of<i3Transform*>(pNode))
		{
			pTrans = (i3Transform *) pNode;

			pTrans->GetAnimatedMatrix( pEnv);

			i3Matrix::Mul( pOut, pOut, &tmp);
		}

		pNode = pNode->GetParent();
	}
}

void i3Transform::OnChangeTime( REAL32 tm)
{
	if( m_pTransCombiner != nullptr)
	{
		m_pTransCombiner->SetPlayTime( tm);
	}

	i3Node::OnChangeTime( tm);
}

void i3Transform::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pMatrixObject != nullptr)
	{
		m_pMatrixObject->OnBuildObjectList( List);
	}

	if( m_pTransCombiner != nullptr)
	{
		m_pTransCombiner->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

UINT32 i3Transform::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Matrix Object
	{
		OBJREF val = (OBJREF) pResFile->GetObjectPersistID( m_pMatrixObject);

		Rc = pResFile->GetStream()->Write( &val, sizeof(val));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Transform::OnSave()", "Could not write matrix value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Transform Source
	{
		OBJREF val = pResFile->GetObjectPersistID( m_pTransCombiner);

		Rc = pResFile->GetStream()->Write( &val, sizeof(UINT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Transform::OnSave()", "Could not write transform source reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3Transform::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Transform::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	AddFlag( I3_NODEFLAG_TRANSFORM);

	// Matrix Object
	{
		OBJREF val;
		i3MatrixObject * pMtxObj;

		Rc = pResFile->GetStream()->Read( &val, sizeof(val));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Transform::OnLoad()", "Could not read matrix value.");
			return STREAM_ERR;
		}
		Result += Rc;

		if( val != 0)
		{
			pMtxObj = (i3MatrixObject *) pResFile->FindObjectByID( val);

			if( pMtxObj != nullptr)
				SetMatrixObject( pMtxObj);
		}
	}

	// Transform Source
	{
		OBJREF val;

		Rc = pResFile->GetStream()->Read( &val, sizeof(UINT32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Transform::OnLoad()", "Could not read transform source reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		if( val != 0)
		{
            m_pTransCombiner = (i3TransformSourceCombiner *) pResFile->FindObjectByID( val);
			I3_MUST_ADDREF( m_pTransCombiner);
		}
	}

	return Result;
}

void i3Transform::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	i3MatrixStack * pStack = pInfo->m_pStack;

	pStack->PushPre( GetCacheMatrix(), GetMatrix());

	i3Node::_GetWrappingBound( pInfo);

	pStack->Pop();
}

void	i3Transform::CalcOccMeshSet(i3::occ_mesh_set* occMeshSet, i3MatrixStack* matStack, bool twoSideEnabled)
{
	matStack->PushPre(GetCacheMatrix(), GetMatrix());

	i3Node::CalcOccMeshSet(occMeshSet, matStack, twoSideEnabled);

	matStack->Pop();
}

void	i3Transform::PreTrace(i3SceneTracer*  pTracer)
{
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	bool bNeedToUpdate = (this->IsFlag(I3_NODEFLAG_MANUAL_UPDATE) == false &&
		pTracer->getFrameID() != this->GetUpdateFrameID()) || (this->GetUpdateFrameID() == 0xFFFFFFFF);

	if (bNeedToUpdate)			// 아마도 모든게 이미 기처리되었을 것으로 보이므로 그냥 통으로 스킵..
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

		SetUpdateFrameID(pTracer->getFrameID());

		pStack->PushPre(GetCacheMatrix(), GetMatrix());
	}
	else
	{
		pStack->Push(GetCacheMatrix(), nullptr);
	}

	I3ASSERT(this->isValidMatrix(this->GetCacheMatrix()));

	if (GetChildCount() > 0)
	{
		i3Node::PreTrace(pTracer);
	}

	pStack->Pop();

}

void i3Transform::OnUpdate( i3SceneTracer * pTracer)
{
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

	if( IsAnimated())
	{
		I3MATRIXINFO * pEnv = pTracer->GetMatrixEnvironment();

		pEnv->m_Time = pTracer->GetTime();
		pEnv->m_pIn = GetMatrix();
		pEnv->m_pOut = GetMatrix();
		pEnv->m_pModelViewMatrix = pStack->GetTop();

		GetAnimatedMatrix( pEnv);
	}

	SetUpdateFrameID( pTracer->getFrameID());

	pStack->PushPre( GetCacheMatrix(), GetMatrix());
	
	//	I3ASSERT(this->isValidMatrix(this->GetCacheMatrix()));

	if( GetChildCount() > 0)
	{
		i3Node::OnUpdate( pTracer);
	}

	pStack->Pop();
}

void i3Transform::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3Transform * pTrans = (i3Transform *) pDest;

	if( method == I3_COPY_REF)
	{
		pTrans->SetMatrixObject( GetMatrixObject());

		// Reference Mode로 복사를 하는 경우에는 Transform Source에 대한 Ref까지
		// 복사할 필요가 없다.
		// 그것은 Matrix Object에 대한 Reference를 공유하기 때문에, 하나만 Animation이
		// 계산되어도 모든 Clone들이 동시에 Animation 하기 때문이다.
	}
	else
	{
		pTrans->SetMatrix( GetMatrix());

		// Transform Sources
		// 복사를 해야하는데...흠...
		// 현재 상황으로서는 100% 원본과 동일하게 복사할 방법이 없다.
		// 또한 방법을 만들어 내더라도, 과연 그것이 실제 게임 구현에서
		// 사용될지 알 수 없기 때문에...
		// 지금은 구현하지 않는다.
	}

	if( m_pTransCombiner != nullptr)
	{
		if( m_pTransCombiner->getAnimationCount() > 0)
		{
			pTrans->SetAnimation( m_pTransCombiner->getAnimation(0), m_pTransCombiner->getAnimationBoneIndex(0));
		}
	}
}

void i3Transform::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	I3ASSERT( pInfo != nullptr);
	I3ASSERT( pInfo->_pMatrixStack != nullptr);

	MATRIX mtx;
	
	pInfo->_pMatrixStack->PushPre( &mtx, GetMatrix());

	i3Node::OnInstancing( pInfo);

	pInfo->_pMatrixStack->Pop();
}



