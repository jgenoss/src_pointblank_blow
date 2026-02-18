#include "i3SceneDef.h"
#include "i3Skin.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3Skin);

I3_TRACE_RESULT	I3SG_NODE_PROC( i3Skin)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3Skin * pSkin = (i3Skin *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	pSkin->Traverse( pTracer);

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

i3Skin::i3Skin(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Skin));
}

i3Skin::~i3Skin(void)
{
	I3_SAFE_RELEASE( m_pCurAnim);
	RemoveAllSkeleton();
	RemoveAllShape();
	_RemoveAllCombiner();
}

void i3Skin::AddSkeleton( i3Skeleton * pSkel)
{
	I3_MUST_ADDREF( pSkel);
	m_SkelList.push_back( pSkel);

	_PrepareCombiner();
}

void i3Skin::RemoveSkeleton( i3Skeleton * pSkel)
{
	
	I3ASSERT( pSkel != nullptr);

	size_t idx = i3::vu::index_of(m_SkelList, pSkel);
	I3ASSERT( idx != m_SkelList.size());

	I3_MUST_RELEASE( pSkel);

	m_SkelList.erase(m_SkelList.begin() + idx);
}

void i3Skin::RemoveAllSkeleton(void)
{
	UINT32 i;
	i3Skeleton * pSkel;

	for( i = 0; i < getSkeletonCount(); i++)
	{
		pSkel = getSkeleton( i);

		I3_MUST_RELEASE( pSkel);
	}

	m_SkelList.clear();
}

void i3Skin::AddShape( i3Shape * pShape)
{
	I3_MUST_ADDREF( pShape);
	m_ShapeList.push_back( pShape);
}

void i3Skin::RemoveShape( i3Shape * pShape)
{
	
	I3ASSERT( pShape != nullptr);

	size_t idx = i3::vu::index_of(m_ShapeList, pShape);
	I3ASSERT( idx != m_ShapeList.size());

	I3_MUST_RELEASE( pShape);

	m_ShapeList.erase( m_ShapeList.begin() + idx);
}

void i3Skin::RemoveAllShape(void)
{
	UINT32 i;
	i3Shape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		I3_MUST_RELEASE( pShape);
	}

	m_ShapeList.clear();
}

void i3Skin::CalcLOD(void)
{
}

void i3Skin::_GetWrappingBound( I3_GETBOUND_INFO * pInfo)
{
	INT32 i, j;
	i3Shape * pShape;
	VEC3D vmin, vmax, temp;
	i3MatrixStack * pStack = pInfo->m_pStack;
	i3BoundBox * pBox = pInfo->m_pBound;

	for( i = 0; i < (INT32) getShapeCount(); i++)
	{
		pShape = getShape( i);

		for( j = 0; j < pShape->getGeometryAttrCount(); j++)
		{
			i3GeometryAttr * pGeoAttr = pShape->getGeometryAttr( j);

			pGeoAttr->CalcBoundBox( &vmin, &vmax);

			i3Vector::TransformCoord( &vmin, &vmin, pStack->GetTop());
			i3Vector::TransformCoord( &vmax, &vmax, pStack->GetTop());

			i3Vector::Minimize( &temp, &vmin, &vmax);
			i3Vector::Maximize( &vmax, &vmin, &vmax);
			i3Vector::Copy( &vmin, &temp);

			if( pBox->IsEmpty())
			{
				pBox->SetMinMax( &vmin, &vmax);
			}
			else
			{
				pBox->ExtendByVec( &vmin);
				pBox->ExtendByVec( &vmax);
			}
		}
	}
}

void i3Skin::_RemoveAllCombiner(void)
{
	
	i3TransformSourceCombiner * pCombiner;

	for( size_t i = 0; i < m_CombinerList.size(); i++)
	{
		pCombiner = m_CombinerList[i];

		I3_MUST_RELEASE( pCombiner);
	}

	m_CombinerList.clear();
}

void i3Skin::_PrepareCombiner(void)
{
	INT32 i, maxbone = 0, diff;
	i3Skeleton * pSkel;

	for( i = 0; i < (INT32) getSkeletonCount(); i++)
	{
		pSkel = getSkeleton( i);

		if( maxbone < pSkel->getBoneCount())
		{
			maxbone = pSkel->getBoneCount();
		}
	}

	if( maxbone > (INT32)m_CombinerList.size())
	{
		// i3TransformSourceCombiber가 모자라는 경우에만 추가로 생성한다.

		diff = maxbone - (INT32)m_CombinerList.size();

		for( i = 0; i < diff; i++)
		{
			i3TransformSourceCombiner * pCombiner;

			pCombiner = i3TransformSourceCombiner::new_object();

			m_CombinerList.push_back( pCombiner);
		}
	}
}

void i3Skin::PlayAnim( i3Animation * pAnim, 
						REAL32 rStartTime,
						REAL32 interval, 
						REAL32 timeScale,
						REAL32 rManualClampTime,
						I3_ANIM_BLEND_MODE blendMode, 
						I3_ANIM_PLAY_MODE destPlayMode, 
						I3_ANIM_PLAY_MODE srcPlayMode)
{
	INT32 i;
	i3Skeleton * pSkel;

	I3ASSERT( pAnim != nullptr);
	I3_REF_CHANGE( m_pCurAnim, pAnim);

	pSkel = getCurrentSkeleton();

	if( (INT32) m_CombinerList.size() < pSkel->getBoneCount())
	{
		_PrepareCombiner();
	}

	for( i = 0; i < pAnim->GetTrackCount(); i++)
	{
		INT32 idx;

		idx = pSkel->FindBoneByName( pAnim->GetTrackBoneName( i));
		if( idx != -1)
		{
			i3TransformSourceCombiner * pCombiner = m_CombinerList[idx];
			pCombiner->SetTimeScale( timeScale);

			pCombiner->AddAnimation( pAnim, i, rStartTime, 
							interval, rManualClampTime, blendMode, destPlayMode, srcPlayMode);
		}
		#if defined( I3_DEBUG)
		else
		{
			I3PRINTLOG(I3LOG_WARN,  "Invalid Bone : %s", pAnim->GetTrackBoneName( i));
		}
		#endif
	}
}

void i3Skin::StopAnim(void)
{
	
	for( size_t i = 0; i < m_CombinerList.size(); i++)
	{
		i3TransformSourceCombiner * pCombiner = m_CombinerList[i];

		pCombiner->RemoveAllAnimation();
	}
}

void i3Skin::SetPlayAnim( REAL32 tm)
{
	
	for( size_t i = 0; i < m_CombinerList.size(); i++)
	{
		i3TransformSourceCombiner * pCombiner = m_CombinerList[i];

		pCombiner->SetPlayTime( tm);
	}
}

REAL32 i3Skin::GetDuration( void)
{
	
	REAL32 maxdur = 0.0f, dur;

	for( size_t i = 0; i < m_CombinerList.size(); i++)
	{
		i3TransformSourceCombiner * pCombiner = m_CombinerList[i];

		dur = pCombiner->GetDuration();
		if( dur > maxdur)
			maxdur = dur;
	}

	return maxdur;
}

void i3Skin::Traverse( i3SceneTracer * pTracer)
{
	//i3Skeleton * pSkel;
	i3Shape * pShape;
	INT32 i;
	i3AttrStackManager * pAttrMnger = pTracer->GetAttrStackManager();

	//pSkel = getCurrentSkeleton();
	pShape = getCurrentShape();

	// Bone Matrix List
	{
		//pSkel->Traverse( pTracer, &m_CombinerList, nullptr);

		// Skinning Geometry는 Rendering하는 환경에 따라 매우 다양한 형태로
		// 재구성 될 수 있으며, 그 구성 및 방법에 따라 설정하는 Bone Matrix가
		// 달라질 수 있기 때문에, 직접 Bone Matrix List를 적용하지 않고
		// Rendering하는 주체인 i3GeometryAttr에게 적용 실행을 맡긴다.
		//pAttrMnger->PushAttr( pSkel->getBoneMatrixListAttr());
	}

	pAttrMnger->FlushPushedAttrs( pTracer->GetOutput());

	// Geometry
	{
		for( i = 0; i < pShape->getGeometryAttrCount(); i++)
		{
			i3GeometryAttr * pGeoAttr = pShape->getGeometryAttr( i);

			// Geometry Attr에 적용된 Bone Matrix List를 설정.
			//pGeoAttr->setBoneMatrixListAttr( pSkel->getBoneMatrixListAttr());

			pTracer->AddAttributeToOutput( pGeoAttr);
		}
	}

}

void	i3Skin::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	UINT32 i;

	i3Node::CopyTo( pDest, method);

	if( !i3::kind_of<i3Skin*>(pDest) )
		return;

	i3Skin * pSkin = (i3Skin *) pDest;

	pSkin->setSlope( getSlope());
	pSkin->setConst( getConst());

	if( method == I3_COPY_REF)
	{
		for( i = 0; i < getSkeletonCount(); i++)
		{
			pSkin->AddSkeleton( getSkeleton( i));
		}
	}
	else
	{
		for( i = 0; i < getSkeletonCount(); i++)
		{
			i3Skeleton * pNewSkel = i3Skeleton::new_object_ref();

			getSkeleton( i)->CopyTo( pNewSkel, method);

			pSkin->AddSkeleton( pNewSkel);
		}
	}

	// Shape는 단순히 Reference만 시킨다.
	for( i = 0; i < getShapeCount(); i++)
	{
		pSkin->AddShape( getShape( i));
	}
}

void	i3Skin::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	UINT32 i;
	i3Skeleton * pSkel;
	i3Shape * pShape;

	for( i = 0; i < getSkeletonCount(); i++)
	{
		pSkel = getSkeleton( i);

		pSkel->OnBuildObjectList( List);
	}

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		pShape->OnBuildObjectList( List);
	}

	if( m_pCurAnim != nullptr)
	{
		m_pCurAnim->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 SKIN
	{
		REAL32		m_Slope = 0.0f;
		REAL32		m_Const = 0.0f;
		OBJREF		m_pCurAnim = 0;
		UINT8		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3Skin::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SKIN data;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;
	
	// Data
	{
		data.m_Slope = getSlope();
		data.m_Const = getConst();
		data.m_pCurAnim = (OBJREF) pResFile->GetObjectPersistID( m_pCurAnim);

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
		Rc =	SaveFromListToResourceFile(m_SkelList, pResFile);		// m_SkelList.SaveTo( pResFile);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write skeleton list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Shape
	{
		Rc =	SaveFromListToResourceFile(m_ShapeList, pResFile);		//	m_ShapeList.SaveTo( pResFile);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write shape list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32	i3Skin::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::SKIN data;

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
	}

	// Skeletons
	{
		Rc = LoadFromResourceFileToList(m_SkelList, pResFile, true);		//	m_SkelList.LoadFrom( pResFile, true);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not read skeleton list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Shape
	{
		Rc = LoadFromResourceFileToList(m_ShapeList, pResFile, true);		//	m_ShapeList.LoadFrom( pResFile, true);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not read shape list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Animation
	if( data.m_pCurAnim != 0)
	{
		i3Animation * pAnim = (i3Animation *) pResFile->FindObjectByID( data.m_pCurAnim);

		_PrepareCombiner();

		PlayAnim( pAnim);
	}

	return Result;
}

