#include "i3SceneDef.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"
#include "i3TransformLinkShapeSet.h"

#if defined ( I3_PHYSX)

I3_CLASS_INSTANCE( i3TransformLinkShapeSet, i3Transform);

I3_TRACE_RESULT I3SG_NODE_PROC( i3TransformLinkShapeSet)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3TransformLinkShapeSet * pTrans = (i3TransformLinkShapeSet *) pNode;
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();
	bool bNeedUpdate = true;
	i3PhysixShapeSet * pShapeSet;

	{
		// Physix -> Animated Transform
		if( pTrans->GetUpdateFrameID() != pTracer->getFrameID())
		{
			pShapeSet = pTrans->getShapeSet();

			if( (pShapeSet != NULL) )
			{
				pTrans->getShapeSet()->getShapeGlobalPose( pTrans->GetCacheMatrix());
				bNeedUpdate = false;
			}
		}
		else
		{
			bNeedUpdate = false;
		}
	}

	bNeedUpdate = bNeedUpdate && (pNode->IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == FALSE);
	bNeedUpdate = bNeedUpdate && ( pTrans->GetUpdateFrameID() != pTracer->getFrameID());

	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedUpdate = bNeedUpdate || (pTrans->GetUpdateFrameID() == 0xFFFFFFFF);

	if( bNeedUpdate)
	{
		pStack->PushPre( pTrans->GetCacheMatrix(), pTrans->GetMatrix());
	}
	else
	{
		// I3ASSERT( i3Matrix::isValid( pTrans->GetName(), pTrans->GetCacheMatrix()));
		I3ASSERT( pTrans->isValidMatrix(pTrans->GetCacheMatrix()));
		pStack->Push( pTrans->GetCacheMatrix(), NULL);
	}

	pTrans->SetUpdateFrameID( pTracer->getFrameID());
	
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

	return Result;
}

i3TransformLinkShapeSet::i3TransformLinkShapeSet()
{
	_setTraceProc( I3SG_NODE_PROC( i3TransformLinkShapeSet));

	m_pShapeSet = NULL;
}

i3TransformLinkShapeSet::~i3TransformLinkShapeSet()
{
	I3_SAFE_RELEASE( m_pShapeSet);
}

/*virtual*/ void	i3TransformLinkShapeSet::OnUpdate( i3SceneTracer * pTracer)
{

	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	bool bNeedUpdate = true;

	{
		// Physix -> Animated Transform
		if( GetUpdateFrameID() != pTracer->getFrameID())
		{
			if( (getShapeSet() != NULL) )
			{
				getShapeSet()->getShapeGlobalPose( GetCacheMatrix());

				bNeedUpdate = false;
			}
		}
		else
		{
			bNeedUpdate = false;
		}
	}

	bNeedUpdate = bNeedUpdate && ( IsFlag( I3_NODEFLAG_MANUAL_UPDATE) == FALSE);
	bNeedUpdate = bNeedUpdate && ( GetUpdateFrameID() != pTracer->getFrameID());

	// 만약 UpdateFrameID가 0xFFFFFFF라면 어떤 경우라도 Cache Matrix를 Update해야 한다.
	bNeedUpdate = bNeedUpdate || ( GetUpdateFrameID() == 0xFFFFFFFF);

	if( bNeedUpdate)
	{
		pStack->PushPre( GetCacheMatrix(), GetMatrix());
	}
	else
	{
		// I3ASSERT( i3Matrix::isValid( GetName(), GetCacheMatrix()));
		I3ASSERT( isValidMatrix(GetCacheMatrix()));

		pStack->Push( GetCacheMatrix(), NULL);
	}

	SetUpdateFrameID( pTracer->getFrameID());

	if( GetChildCount() > 0)
	{
		i3Node::OnUpdate( pTracer);
	}

	pStack->Pop();
}


#endif