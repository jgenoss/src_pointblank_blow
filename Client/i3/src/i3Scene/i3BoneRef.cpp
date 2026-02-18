#include "i3SceneDef.h"
#include "i3BoneRef.h"
#include "i3SceneTracer.h"
#include "i3CommonNodeProcs.h"

I3_CLASS_INSTANCE( i3BoneRef);

I3_TRACE_RESULT I3SG_NODE_PROC( i3BoneRef)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	if( pNode->GetChildCount() > 0)
	{
		i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

		if( pTracer->getCurrentAnimationContext() != nullptr)
		{
			i3AttrStackManager * pStackManager = pTracer->GetAttrStackManager();
			i3BoneRef * pBoneRef = (i3BoneRef *) pNode;
			INT32 boneIdx = pBoneRef->getBoneIndex();
			MATRIX * pCacheMtx = pTracer->getCurrentAnimationContext()->getCacheMatrix( boneIdx);

			pStack->Push( pCacheMtx, nullptr);

			{
				// 자신의 Model View Matrix를 tracer에 등록해 둔다.
				register i3ModelViewMatrixAttr * pAttr = pTracer->AllocModelViewMatrixAttr();

				_copyTop( pAttr->GetMatrix(), pStack);

				pStackManager->PushAttr( pAttr);
			}

			Result = i3Node::CallTraceProc( pNode, pTracer);

			pStack->Pop();
			pStackManager->PopAttr( I3_ATTRID_MODEL_VIEW_MATRIX);
		}
		else
		{
			Result = i3Node::CallTraceProc( pNode, pTracer);
		}
	}

	return Result;
}

i3BoneRef::i3BoneRef(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3BoneRef));
}


void i3BoneRef::OnUpdate( i3SceneTracer * pTracer)
{
	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	i3AnimationContext * pCtx = pTracer->getCurrentAnimationContext();

	if( pCtx != nullptr)
	{
		INT32 boneIdx = getBoneIndex();
		MATRIX * pCacheMtx = pCtx->getCacheMatrix( boneIdx);

		pStack->Push( pCacheMtx, nullptr);
	}

	if( GetChildCount() > 0)
	{
		i3Node::OnUpdate( pTracer);
	}

	if( pCtx != nullptr)
	{
		pStack->Pop();
	}
}

void	i3BoneRef::PreTrace(i3SceneTracer * pTracer)
{
	if (GetChildCount() < 1)
		return;

	i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();
	i3AnimationContext * pCtx = pTracer->getCurrentAnimationContext();

	if (pCtx != nullptr)
	{
		INT32 boneIdx = getBoneIndex();
		MATRIX * pCacheMtx = pCtx->getCacheMatrix(boneIdx);

		pStack->Push(pCacheMtx, nullptr);
	}

	i3Node::PreTrace(pTracer);

	if (pCtx != nullptr)
	{
		pStack->Pop();
	}
}

void i3BoneRef::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	if( pInfo->m_pSkel == nullptr)
	{
		i3Node::_GetWrappingBound( pInfo);
		return;
	}

	i3Skeleton * pSkel = pInfo->m_pSkel;
	i3MatrixStack * pStack = pInfo->m_pStack;

	MATRIX mtx;

	pSkel->GetFullMatrix( getBoneIndex(), &mtx);

	pStack->Push( &mtx, nullptr);

	i3Node::_GetWrappingBound( pInfo);

	pStack->Pop();
}

void i3BoneRef::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	if( !i3::same_of<i3BoneRef*>(pDest))
		return;

	i3BoneRef * pBoneRef = (i3BoneRef *) pDest;

	pBoneRef->setBoneIndex( getBoneIndex());
}

void i3BoneRef::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3Node::OnBuildObjectList( List);
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)				// [test required] 패킹 사이즈 누락으로 추가합니다. 2017.02.23. soon9
#endif

namespace pack
{
	struct ALIGN8 BONE_REF
	{
		UINT32		pad0 = 0;
		INT32		m_BoneIndex = 0;
		UINT8		pad[8] = { 0 };
	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32 i3BoneRef::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32 Rc, Result = 0;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::BONE_REF data;	
	data.m_BoneIndex = getBoneIndex();

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write data");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3BoneRef::OnLoad( i3ResourceFile * pResFile)
{
	pack::BONE_REF data;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 Rc, Result;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read data");
		return STREAM_ERR;
	}
	Result += Rc;

	setBoneIndex( data.m_BoneIndex);

	return Result;
}

void i3BoneRef::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	i3Node::OnInstancing( pInfo);
}

