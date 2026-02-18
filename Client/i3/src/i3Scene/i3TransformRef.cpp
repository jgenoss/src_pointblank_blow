#include "i3SceneDef.h"
#include "i3TransformRef.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"
#include "i3ModelViewMatrixAttr.h"

I3_CLASS_INSTANCE( i3TransformRef);

I3_TRACE_RESULT I3SG_NODE_PROC( i3TransformRef)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	if( pNode->GetChildCount() > 0)
	{
		i3TransformRef * pTransRef = (i3TransformRef *) pNode;
		i3Transform * pTrans = pTransRef->GetTransform();
		i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
		i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();

		if( pTrans != nullptr)
		{
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
		}
		
		{
			Result = i3Node::CallTraceProc( pNode, pTracer);
		}

		if( pTrans != nullptr)
		{
			pStackManager->PopAttr( I3_ATTRID_MODEL_VIEW_MATRIX);
			pStack->Pop();
		}
	}

	return Result;
}

i3TransformRef::i3TransformRef()
{
	_setTraceProc( I3SG_NODE_PROC( i3TransformRef));
}

i3TransformRef::~i3TransformRef()
{
	I3_SAFE_RELEASE(m_pRefTrans);
}


void		i3TransformRef::SetTransform( i3Transform * pTrans)
{
	I3_REF_CHANGE( m_pRefTrans, pTrans);
}

