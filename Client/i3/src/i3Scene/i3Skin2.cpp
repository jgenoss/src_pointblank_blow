#include "i3SceneDef.h"
#include "i3Skin2.h"
#include "i3AttrStackManager.h"
#include "i3SceneTracer.h"
#include "../i3Math/i3Animation2.h"
#include "i3BoneRef.h"

I3_CLASS_INSTANCE( i3Skin2, i3Node);

I3_TRACE_RESULT	I3SG_NODE_PROC( i3Skin2)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3Skin2 * pSkin = (i3Skin2 *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	pSkin->Traverse( pTracer);

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

void i3Skin2::Traverse( i3SceneTracer * pTracer)
{
	i3SkinLOD * pLOD;

	CalcLOD( pTracer);

	pLOD = getCurrentLOD();

	// Bone Matrix List
	if( pLOD->getSkeleton() != NULL)
	{
		pLOD->getSkeleton()->Traverse( pTracer, &m_CombinerList, pLOD->getBoneMap());

		// Skinning Geometry는 Rendering하는 환경에 따라 매우 다양한 형태로
		// 재구성 될 수 있으며, 그 구성 및 방법에 따라 설정하는 Bone Matrix가
		// 달라질 수 있기 때문에, 직접 Bone Matrix List를 적용하지 않고
		// Rendering하는 주체인 i3GeometryAttr에게 적용 실행을 맡긴다.
		//pAttrMnger->PushAttr( pLOD->getBoneMatrixListAttr());
		pTracer->setCurrentBoneMatrixList( pLOD->getSkeleton()->getBoneMatrixListAttr());
	}

	// Geometry
	if( pLOD->getShapeNode() != NULL)
	{
		i3Node::CallTraceProc( pLOD->getShapeNode(), pTracer);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//
i3Skin2::i3Skin2(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3Skin2));

	m_Slope = 1.0f;
	m_Const = 0.0f;
	m_CurLevel = 0;

	m_pCurAnim = NULL;
}

i3Skin2::~i3Skin2(void)
{
	I3_SAFE_RELEASE( m_pCurAnim);
	RemoveAllLOD();
	_RemoveAllCombiner();
}

void i3Skin2::AddLOD( i3SkinLOD * pLOD)
{
	I3ASSERT( pLOD != NULL);

	pLOD->AddRef();
	m_LODList.Add( pLOD);

	if( m_LODList.GetCount() == 1)
	{
		// Level 0에 대하여...
		_PrepareCombiner();
	}
}

void i3Skin2::RemoveLOD( i3SkinLOD * pLOD)
{
	INT32	idx;

	I3ASSERT( pLOD != NULL);

	idx = m_LODList.IndexOf( pLOD);
	I3ASSERT( idx != -1);

	pLOD->Release();

	m_LODList.Delete( idx);
}

void i3Skin2::RemoveAllLOD(void)
{
	UINT32 i;
	i3SkinLOD * pLOD;

	for( i = 0; i < getLODCount(); i++)
	{
		pLOD = getLOD( i);

		pLOD->Release();
	}

	m_LODList.Clear();
}

void i3Skin2::CalcLOD( i3SceneTracer * pTracer)
{
	VEC3D vpos[3], vmax, vmin;
	i3BoundBox * pBox;
	REAL32 span;
	INT32 newLv;
	MATRIX mtxFull;

	if( pTracer->isState( I3SG_TRACER_STATE_OFFSCREEN))
	{
		m_CurLevel = getLODCount() - 1;
		return;
	}

	// Bound Box가 없다면 계산하지 않는다.
	if( GetBound() == NULL)
		return;

	pBox = GetBound();

	vpos[0].x = pBox->GetMin()->x;
	vpos[0].y = pBox->GetMin()->y;
	vpos[0].z = pBox->GetMax()->z;

	vpos[1].x = pBox->GetMax()->x;
	vpos[1].y = pBox->GetMin()->y;
	vpos[1].z = pBox->GetMin()->z;

	vpos[2].x = pBox->GetMax()->x;
	vpos[2].y = pBox->GetMax()->y;
	vpos[2].z = pBox->GetMax()->z;

	// 화면에 Rendering했을 때 차지랑 화면의 점유율을 구한다.
	// 전체 화면이 X, Y 방향 모두 2의 크기를 가지고 있다...
	i3Matrix::Mul( &mtxFull, pTracer->GetModelViewMatrixStack()->GetTop(), pTracer->getViewProjectionMatrix());
	i3Matrix::TransformPoints( vpos, vpos, 3, &mtxFull);

	i3Vector::Minimize( &vmin, &vpos[0], &vpos[1]);
	i3Vector::Minimize( &vmin, &vmin, &vpos[2]);

	i3Vector::Maximize( &vmax, &vpos[0], &vpos[1]);
	i3Vector::Maximize( &vmax, &vmin, &vpos[2]);

	span = MAX( i3Math::abs(vmax.x - vmin.x), i3Math::abs(vmax.y - vmin.y));		// -1 ~ 1
	
	// Level 계산
	if( span < I3_EPS)
	{
		newLv = getLODCount() - 1;
	}
	else
	{
		newLv = (INT32)((m_Slope * (2.0f / span)) + m_Const);

		if( newLv < 0)
			newLv = 0;
		else if( newLv > (((INT32) getLODCount() - 1)))
			newLv = getLODCount() - 1;
	}

	/*
	if( !pTracer->isState( I3SG_TRACER_STATE_OFFSCREEN))
		I3TRACE( "LOD : %d\n", newLv);
		*/

	m_CurLevel = newLv;
}

void i3Skin2::_GetWrappingBound( i3MatrixStack * pStack, i3BoundBox * pBox)
{
	INT32 i;
	i3SkinLOD * pLOD;

	for( i = 0; i < (INT32) getLODCount(); i++)
	{
		pLOD = getLOD( i);

		if( pLOD->getShapeNode() != NULL)
		{
			pLOD->getShapeNode()->GetWrappingBound( pBox);
		}
	}
}

void i3Skin2::_RemoveAllCombiner(void)
{
	INT32 i;
	i3TransformSourceCombiner * pCombiner;

	for( i = 0; i < m_CombinerList.GetCount(); i++)
	{
		pCombiner = (i3TransformSourceCombiner *) m_CombinerList.Items[i];

		pCombiner->Release();
	}

	m_CombinerList.Clear();
}

void i3Skin2::_PrepareCombiner(void)
{
	INT32 i, maxbone = 0, diff;
	i3SkinLOD * pLOD;

	I3ASSERT( getLODCount() > 0);

	pLOD = getLOD( 0);

	maxbone = pLOD->getBoneCount();

	if( maxbone > m_CombinerList.GetCount())
	{
		// i3TransformSourceCombiber가 모자라는 경우에만 추가로 생성한다.

		diff = maxbone - m_CombinerList.GetCount();

		for( i = 0; i < diff; i++)
		{
			i3TransformSourceCombiner * pCombiner;

			pCombiner = i3TransformSourceCombiner::NewObject();

			m_CombinerList.Add( pCombiner);
		}
	}
}

void i3Skin2::PlayAnim( i3Animation * pAnim, 
						RT_REAL32 rStartTime,
						RT_REAL32 interval, 
						I3_ANIM_BLEND_MODE blendMode, 
						I3_ANIM_PLAY_MODE destPlayMode, 
						I3_ANIM_PLAY_MODE srcPlayMode)
{
	INT32 i;
	i3SkinLOD * pLOD;

	I3ASSERT( pAnim != NULL);

	pAnim->AddRef();
	I3_SAFE_RELEASE( m_pCurAnim);

	m_pCurAnim = pAnim;

	pLOD = getCurrentLOD();

	if( m_CombinerList.GetCount() < (INT32) pLOD->getBoneCount())
	{
		_PrepareCombiner();
	}

	if( pAnim->IsExactTypeOf( i3Animation2::GetClassMeta()))
	{
		i3Animation2 * pAnim2 = (i3Animation2 *) pAnim;

		for( i = 0; i < pAnim->GetTrackCount(); i++)
		{
			INT32 idx = pAnim2->GetBoneIndex( i);

			if( idx != -1)
			{
				i3TransformSourceCombiner * pCombiner = (i3TransformSourceCombiner *) m_CombinerList.Items[idx];
				
				pCombiner->AddTransformSource( pAnim->GetTrack( i), rStartTime, 
								interval, blendMode, destPlayMode, srcPlayMode);
			}
		}
	}
	else
	{
		for( i = 0; i < pAnim->GetTrackCount(); i++)
		{
			INT32 idx;

			idx = pLOD->getSkeleton()->FindBoneByName( pAnim->GetTrackBoneName( i));
			if( idx != -1)
			{
				i3TransformSourceCombiner * pCombiner = (i3TransformSourceCombiner *) m_CombinerList.Items[idx];
				
				pCombiner->AddTransformSource( pAnim->GetTrack( i), rStartTime, 
								interval, blendMode, destPlayMode, srcPlayMode);
			}
			#if defined( I3_DEBUG)
			else
			{
				I3WARN( "Invalid Bone : %s\n", pAnim->GetTrackBoneName( i));
			}
			#endif
		}
	}
}

void i3Skin2::StopAnim(void)
{
	INT32 i;

	for( i = 0; i < m_CombinerList.GetCount(); i++)
	{
		i3TransformSourceCombiner * pCombiner = (i3TransformSourceCombiner *) m_CombinerList.Items[i];

		pCombiner->RemoveAllTransformSource();
	}
}

void i3Skin2::SetPlayAnim( RT_REAL32 tm)
{
	INT32 i;

	for( i = 0; i < m_CombinerList.GetCount(); i++)
	{
		i3TransformSourceCombiner * pCombiner = (i3TransformSourceCombiner *) m_CombinerList.Items[i];

		pCombiner->SetPlayTime( tm);
	}
}

RT_REAL32 i3Skin2::GetDuration( void)
{
	INT32 i;
	RT_REAL32 maxdur = __RT_ZERO, dur;

	for( i = 0; i < m_CombinerList.GetCount(); i++)
	{
		i3TransformSourceCombiner * pCombiner = (i3TransformSourceCombiner *) m_CombinerList.Items[i];

		dur = pCombiner->GetDuration();
		if( dur > maxdur)
			maxdur = dur;
	}

	return maxdur;
}

void i3Skin2::OnChangeTime( REAL32 tm)
{
	SetPlayAnim( tm);

	i3Node::OnChangeTime( tm);
}

void	i3Skin2::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	UINT32 i;

	i3Node::CopyTo( pDest, method);

	I3ASSERT( pDest->IsTypeOf( i3Skin2::GetClassMeta()));
	
	i3Skin2 * pSkin = (i3Skin2 *) pDest;

	pSkin->setSlope( getSlope());
	pSkin->setConst( getConst());

	if( method == I3_COPY_REF)
	{
		for( i = 0; i < getLODCount(); i++)
		{
			pSkin->AddLOD( getLOD( i));
		}
	}
	else
	{
		for( i = 0; i < getLODCount(); i++)
		{
			i3SkinLOD * pNewLOD = i3SkinLOD::NewObjectRef();

			getLOD( i)->CopyTo( pNewLOD, method);

			pSkin->AddLOD( pNewLOD);
		}
	}

	if( m_pCurAnim != NULL)
	{
		pSkin->PlayAnim( m_pCurAnim);
	}
}

void	i3Skin2::OnBuildObjectList( i3List * pList)
{
	UINT32 i;
	i3SkinLOD * pLOD;

	for( i = 0; i < getLODCount(); i++)
	{
		pLOD = getLOD( i);

		pLOD->OnBuildObjectList( pList);
	}

	if( m_pCurAnim != NULL)
	{
		m_pCurAnim->OnBuildObjectList( pList);
	}

	i3Node::OnBuildObjectList( pList);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

typedef struct ALIGN8 _tagi3PersistSkin2
{
	UINT8		m_ID[4];
	REAL32		m_Slope;
	REAL32		m_Const;
	OBJREF		m_pCurAnim;
	UINT8		pad[32];
} I3_PERSIST_SKIN2;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3Skin2::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_SKIN2 data;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	memset( &data, 0, sizeof(data));

	// Data
	{
		memcpy( data.m_ID, "SKN2", 4);
		data.m_Slope = getSlope();
		data.m_Const = getConst();
		data.m_pCurAnim = (OBJREF) m_pCurAnim;

		Rc = pStream->Write( &data, sizeof(data));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write skin data.\n");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Skeletons
	{
		Rc = m_LODList.SaveTo( pResFile);
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write skeleton list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32	i3Skin2::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_SKIN2 data;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Data
	{
		Rc = pStream->Read( &data, sizeof(data));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not read skin data.\n");
			return STREAM_ERR;
		}
		Result += Rc;

		setSlope( data.m_Slope);
		setConst( data.m_Const);
	}

	// Skeletons
	{
		Rc = m_LODList.LoadFrom( pResFile, true);
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not read skeleton list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Animation
	if( data.m_pCurAnim != NULL)
	{
		i3Animation * pAnim = (i3Animation *) pResFile->FindObjectByID( data.m_pCurAnim);

		_PrepareCombiner();

		PlayAnim( pAnim);
	}

	return Result;
}

void	i3Skin2::_Rec_AdjustBoneRef( i3Node * pNode, i3BoneMatrixListAttr * pAttr)
{
	INT32 i;
	i3Node * pChild;

	if( pNode->IsTypeOf( i3BoneRef::GetClassMeta()))
	{
		i3BoneRef * pBoneRef = (i3BoneRef *) pNode;
		pBoneRef->setBoneMatrixList( pAttr);
	}

	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		pChild = pNode->GetChild( i);

		_Rec_AdjustBoneRef( pChild, pAttr);
	}
}

void	i3Skin2::OnInitAfterLoad(void)
{
	INT32 i;
	i3Node * pChild;
	i3BoneMatrixListAttr * pAttr;

	if( getLODCount() < 0)
		return;

	i3SkinLOD * pLOD = getLOD( 0);

	pAttr = pLOD->getSkeleton()->getBoneMatrixListAttr();

	// Child로 있는 i3BoneRef Node들의 i3BoneMatrixListAttr의 Pointer를 재조정해준다.
	for( i = 0; i < GetChildCount(); i++)
	{
		pChild = GetChild( i);

		_Rec_AdjustBoneRef( pChild, pAttr);
	}
}
