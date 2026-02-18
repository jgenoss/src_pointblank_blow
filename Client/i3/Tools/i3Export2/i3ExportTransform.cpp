#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"
#include "dummy.h"
#include "i3Base/string/compare/generic_is_niequal.h"

INT32 i3Export::_OnExportTransformSeq( INode * pNode)
{
	::Animatable * pCtrl;
	IKey * pKey = NULL;
	Interval intv;
	Interface * ip = GetCOREInterface();
	INT32 j, idxBone = -1;
	i3::vector<TimeValue> TimeList;
	i3TransformSequence * pTransSeq;
	TimeValue step;

//	if( m_ExcludeAnimList.IndexOf( pNode->GetName()) != -1)
	if ( i3::vu::int_index_of(m_ExcludeAnimList, pNode->GetName()) != -1 )
		return NULL;

	if( _IsValidNode( pNode) == false)
		return NULL;

	pCtrl = pNode->GetTMController();

	if( pCtrl == NULL)
		return NULL;

	// 동일 이름의 Animation Track이 이미 존재하는지 확인한다.
	if( m_pAnim != NULL)
	{
		for(INT32 i = 0; i < m_pAnim->GetTrackCount(); i++)
		{
			if( i3::generic_is_iequal( m_pAnim->GetTrackBoneName( i), pNode->GetName()) )
			{
				// 동일 이름의 Track이 이미 있기 때문에 추가하지 않는다.
				return NULL;
			}
		}
	}

	intv = ip->GetAnimRange();

	// Key Data를 추출해야 하는 Time을 구한다.
	//TimeList.SetOnceMode( TRUE);

	m_Option.m_KeySampleType = i3tool::exporter::KEYSAMPLE_SAMPLE;
	if( m_Option.m_KeySampleType == i3tool::exporter::KEYSAMPLE_PICKEDONLY)
	{
		// Designer가 입력한 Key위치에서의 값만 가져온다.
		if( pCtrl->NumKeys() < 0)
		{
			for(INT32 i = 0; i < pCtrl->NumSubs(); i++)
			{
				::Animatable * pSubAnim = pCtrl->SubAnim( i);

				for( j = 0; j < pSubAnim->NumKeys(); j++)
				{
					TimeList.push_back(  pSubAnim->GetKeyTime( j));
				}
			}
		}
		else
		{
			for(INT32 i = 0; i < pCtrl->NumKeys(); i++)
			{
				TimeList.push_back(  pCtrl->GetKeyTime( i));
			}
		}
	}
	else
	{
		// 일정한 시간 간격을 두고 Sampling한다.
		if( m_Option.m_SamplingInterval <= 1)
			m_Option.m_SamplingInterval = 1;

		step = m_Option.m_SamplingInterval * GetTicksPerFrame();
		INT32 count = intv.Duration() / step;
		TimeValue accm = intv.Start();

		for(INT32 i = 0; i < count; i++)
		{
			TimeList.push_back(  accm);
			accm += step;
		}

		TimeList.push_back( intv.End());
	}
	
	i3::vu::remove_duplicates(TimeList);

	if( TimeList.size() <= 0)
		return NULL;					// No Animation

	if( m_pAnim == NULL)
	{
		// 처음 생성되는 TransformSequence인 경우이기 때문...
		m_pAnim = i3Animation::new_object();
	}

	// Keyframe이 지정된 대로 Export
	{
		i3TransformSequence::I3_KEY_CHANNEL chn;
		VEC3D scale;
		QUATERNION quat;
		Matrix3 pmtx;
		MATRIX Matrix;
		TimeValue time, start;
		Point3 vec;
		REAL32 dur;

		chn = (i3TransformSequence::I3_KEY_CHANNEL) (	i3TransformSequence::TRANSLATE | 
														i3TransformSequence::ROTATE | 
														i3TransformSequence::SCALE |
														i3TransformSequence::TIME);

		pTransSeq = i3TransformSequence::new_object_ref();

		pTransSeq->SetName( pNode->GetName());

		{
			TimeValue s, e;

			start = s = TimeList[0];
			e = TimeList[ TimeList.size() - 1];

			dur = Time2Sec( e - s);
		}

		pTransSeq->Create((INT32) TimeList.size(), dur, chn);

		for(size_t i = 0; i < TimeList.size(); i++)
		{
			// copy the key information
			time = TimeList[i];

			ExtractTransform( pNode, &Matrix, time);

			// Time
			pTransSeq->SetTime( i, Time2Sec( time - start));

			// Scale
			{
				i3Vector::SetX( &scale, i3Vector::Length( i3Matrix::GetRight( &Matrix)));
				i3Vector::SetY( &scale, i3Vector::Length( i3Matrix::GetUp( &Matrix)));
				i3Vector::SetZ( &scale, i3Vector::Length( i3Matrix::GetAt( &Matrix)));

				pTransSeq->SetScale( i, &scale);
			}

			// Rotation
			{
				i3Matrix::GetRotateQuat( &quat, &Matrix);

				i3Quat::Normalize( &quat, &quat);

				pTransSeq->SetRotation( i, &quat);
			}

			// Position
			{
				pTransSeq->SetTranslation( i, i3Matrix::GetPos( &Matrix));
			}
		}

		idxBone = m_pAnim->AddTrack( pTransSeq, pNode->GetName());
	}

	return idxBone;
}

namespace
{
	struct IS_REF_PROXY_INFO
	{
		const char *	_pszName = nullptr;
		INode *			_pNode = nullptr;
	};
}

static bool _IsRefForProxyProc( INT32 Level, INode * pNode, void * pUserData)
{
	TSTR temp;
	IS_REF_PROXY_INFO * pInfo = (IS_REF_PROXY_INFO *) pUserData;

	bool bFlag;

	bFlag = pNode->GetUserPropString( TSTR( "Proxy_Geometry"), temp) == TRUE;
	if( bFlag)
	{
		bFlag = i3::generic_is_iequal( temp.data(), pInfo->_pszName) ;
		if( bFlag)
		{
			pInfo->_pNode = pNode;
			return false;				// Traverse를 정지시키기 위함.
		}
	}

	return true;		// traversal 계속
}

bool i3Export::_IsReferencedForProxy( INode * pNode)
{
	i3ExportTracer Tracer;
	IS_REF_PROXY_INFO info;

	info._pNode = NULL;
	info._pszName = pNode->GetName();

	Tracer.Trace( GetCOREInterface()->GetRootNode(), _IsRefForProxyProc, &info);

	return info._pNode != NULL;
}

bool i3Export::_OnExportTransform( INode * pNode, ObjectState * pState)
{
	MATRIX Matrix;
	bool bNeedTransform, bPxNode, bProxy = false;
	bool bAttachPoint = false;

	bPxNode = _IsPhysixNode( pNode);
	bNeedTransform = ExtractTransform( pNode, &Matrix, m_timeStart);

	bNeedTransform = bNeedTransform || _IsReferencedForProxy( pNode);

	if( bPxNode)
	{
		// Proxy Geometry
		TSTR temp;

		if( pNode->GetUserPropString( TSTR( "Proxy_Geometry"), temp))
		{
			if( temp.data()[0] != 0)
				bProxy = true;
		}
	}

	if( bNeedTransform == false)
	{
		INT32 forceExport;

		if( pNode->GetUserPropInt( TSTR("ForceExport"), forceExport))
		{
			bNeedTransform = (forceExport != 0);
			bAttachPoint = true;
		}
	}

	if( bNeedTransform || bPxNode || IsDummy( pNode->GetObjectRef()))
	{
		i3Transform * pTrans = NULL;
		BONETYPE boneType;

		FindBonesModifier( pNode, &boneType);

		bNeedTransform = (IsBiped( pNode) == false) || ((boneType != BONES_3DS) && (boneType != PHYSIQUE));

		if( bNeedTransform && ((bPxNode == false) || (m_Option.m_bExportPhysicsGeometry == true) || (bProxy == false)))
		{
			// 만약 해당 노드에 Physix 설정이 있다면 i3Transform2 Node를 생성한다.
			if( bPxNode)
				pTrans = i3Transform2::new_object_ref();
			else
				pTrans = i3Transform::new_object_ref();

			pTrans->SetMatrix( &Matrix);
			pTrans->SetName( pNode->GetName());

			if( bAttachPoint)
				pTrans->AddFlag( I3_NODEFLAG_ATTACH_POINT);

			if( bPxNode == false)
			{
				// Physix Node에 대해서는 Animation값을 Export 하지 않는다.
				if( m_Option.m_bForceExportAnim || ::IsAnimation( pNode))
				{
					if( IsLeafOfBiped( pNode) == false)
					{
						// Biped의 Child Node라면 Animation을 Export하지 않는다.
						_OnExportTransformSeq( pNode);
					}
					// 실제 Animation은 처리가 완전히 끝난 후 설정된다.
				}
				else
				{
					UINT32	partA	=	pState->obj->ClassID().PartA();
					bool bBone = false;

					if( pState->obj->ClassID() == BONE_OBJ_CLASSID)
						bBone = true;

					if((partA == DUMMY_CLASS_ID) || (partA == BONE_CLASS_ID))
						bBone = true;

					if( bBone == false)
					{
						if( pState->obj->SuperClassID() == SClass_ID( GEOMOBJECT_CLASS_ID))
						{
							pTrans->AddFlag( I3_NODEFLAG_STATIC);
						}
					}
				}
			}

			m_pCurProcessInfo->pushi3Node( pTrans);
		}

		if( bPxNode)
		{
			_RegisterPostProcess( i3tool::exporter::POSTPROCESS_SHAPESET, pNode, pTrans, &Matrix, true);
		}
	}

	return true;
}

void	i3Export::_Rec_ExtractTransformSeq( INode * pNode)
{
	INT32 i;
	INode * pChild;
	bool bProcess = true;

	// 이름으로 Animation Keyframe을 뽑아낼 것인지 판단
	// 선두 이름이 "Pivot...", "Bone..."로 시작하는 Bone들은 포함시킨다.
	// 이름에 Dummy가 포함된 것은 Keyframe을 뽑아내지 않는다.
	if( IsBiped( pNode))
	{
		if( strstr( pNode->GetName(), "Nub") != NULL)
			bProcess = false;
	}
	else if( IsFootPrint( pNode))
	{
		bProcess = false;
	}
	else 
	{
		if( i3::generic_is_niequal( pNode->GetName(), "PIVOT", 5)  ||
			i3::generic_is_niequal( pNode->GetName(), "BONE", 4) )
			bProcess = true;
		else
		{
			if( isValidBone( pNode, m_Option.m_bTreatControlAsBone != 0) == BONE_TYPE_SKELETON)
				bProcess = true;
			else
				bProcess = false;
		}
	}

	if( bProcess)
	{
		_OnExportTransformSeq( pNode);
	}

	for( i = 0; i < pNode->NumberOfChildren(); i++)
	{
		pChild = pNode->GetChildNode( i);

		_Rec_ExtractTransformSeq( pChild);
	}
}

i3Animation *	i3Export::_ExtractAnimationFromBone(void)
{

	_Rec_ExtractTransformSeq( m_pSkinBoneRoot);

//	if( IdxList.GetCount() <= 0)
//		return NULL;
//	return m_pAnim;

	return NULL;			// IdxList가 채워지지 않으므로 항상 널로 리턴하는 것으로 수정..
}
