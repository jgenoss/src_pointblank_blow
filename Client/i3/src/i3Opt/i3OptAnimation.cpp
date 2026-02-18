#include "i3OptPCH.h"
#include "i3OptAnimation.h"
#include <stdio.h>

#include "i3Base/string/ext/contain_string.h"

I3_CLASS_INSTANCE(i3OptAnimation);

i3OptAnimation::i3OptAnimation(void)
{
	m_Class = CLASS_ANIMATION;
}

i3TransformSource * i3OptAnimation::_OptTransSource(i3TransformSource * pSrc, bool bDeleteTranslation /*= true*/)
{
	REAL32	ApproximateValue = 0.000001f;
	i3TransformSource * pResult1, *pResult2;

	OptScaleInterPolate(pSrc, ApproximateValue);
	pResult1 = OptEraseSameKeyFrame(pSrc, ApproximateValue, bDeleteTranslation);
	pResult2 = OptEraseSameKeyFrame_Lerp(pResult1, ApproximateValue, bDeleteTranslation);
	pResult1->Release();

	OptTimeStep(pResult2, ApproximateValue);

	return pResult2;
}

void	i3OptAnimation::Trace(i3Node * pRoot)
{
	i3::vector<i3Node*> TrackList;
	UINT32	nAnimationIndex = 0;
	char	szFileName[256];
	char	szDirectory[256];

	INT32 i, j;

	i3Scene::FindNodeByType(pRoot, i3Transform::static_meta(), TrackList);
	::GetCurrentDirectory(256, szDirectory);
	strncat(szDirectory, "/Log", sizeof(szDirectory) - 1);
	::CreateDirectory(szDirectory, NULL);

	for (size_t nCount = 0; nCount < TrackList.size(); ++nCount)
	{
		i3Transform * pTrans = (i3Transform *)TrackList[nCount];

		if (pTrans->IsAnimated() && (pTrans->getAnimationCount() > 0))
		{
			for (i = 0; i < pTrans->getAnimationCount(); i++)
			{
				i3Animation * pAnim = pTrans->getAnimation(i);

				for (j = 0; j < pAnim->GetTrackCount(); j++)
				{
					i3TransformSource * pSeqTrack = pAnim->GetTrack(j);

					if (i3::same_of<i3TransformSequence*>(pSeqTrack))
					{
						i3TransformSource * pTempSource;

						sprintf(szFileName, "./Log/Animation%dOrig.txt", nAnimationIndex);
						LogAnimation(pSeqTrack, szFileName);

						pTempSource = _OptTransSource(pSeqTrack);

						sprintf(szFileName, "./Log/Animaton%dOpt.txt", nAnimationIndex);
						LogAnimation(pTempSource, szFileName);

						if (pTempSource != pSeqTrack)//최적화에의해 새로이 만들어진 시퀀스라면 TRANSFORMSOURCE를 변
							pAnim->SetTrack(j, (i3TransformSequence *)pTempSource, pAnim->GetTrackBoneName(j));

						if (pTempSource)
							pTempSource->Release();
						
						++nAnimationIndex;
					}
				}
			}
		}
	}
}

void i3OptAnimation::OptimizeAnim(i3Animation * pAnim, bool bDeleteTranslation /*= true*/, bool b16Bits /*= false*/)
{
	INT32 i;
	bool bMode = false;

	if (bDeleteTranslation)
		I3TRACE("%s Translation key를 삭제합니다...\n", pAnim->GetName());
	if (b16Bits)
		I3TRACE("%s 16Bits로 압축합니다.\n", pAnim->GetName());

	for (i = 0; i < pAnim->GetTrackCount(); i++)
	{
		i3TransformSequence * pSeq = pAnim->GetTrack(i);
		i3TransformSequence * pNew;

		if (i3::generic_is_iequal(pAnim->GetTrackBoneName(i), "Root") ||
			i3::contain_string(pAnim->GetTrackBoneName(i), "Clavicle") >= 0)
			bMode = false;
		else
			bMode = bDeleteTranslation;

		if (b16Bits)
		{
			UINT32 keyChannel = i3TransformSequence::REAL16_FORMAT | pSeq->GetKeyframeChannels();
			pSeq->SetKeyframeChannels((i3TransformSequence::I3_KEY_CHANNEL) keyChannel);
		}

		pNew = (i3TransformSequence *)_OptTransSource(pSeq, bMode);

		pAnim->SetTrack(i, pNew, pAnim->GetTrackBoneName(i));

		if (pNew)
			pNew->Release();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////

//모든 키프레임의 Scale의 값이 모두 1인경우 스케일에대한 보간을 할필요가 없음을 명시하여줍니다.
i3TransformSource * 	i3OptAnimation::OptScaleInterPolate(i3TransformSource * pAnim, REAL32	ApproximateValue)
{
	i3TransformSequence * pTransSeq = (i3TransformSequence *)pAnim;

	if (!pTransSeq->HasScale()) return pAnim;

	INT32	nFrameCount = pTransSeq->GetKeyframeCount();
	INT32	nIdentityCount = 0;

	for (INT32 nCount = 0; nCount < nFrameCount; ++nCount)
	{
		VEC3D pVec3;
		pTransSeq->GetScale(nCount, &pVec3);

		if ((i3Math::abs(i3Vector::GetX(&pVec3) - 1.0f) < ApproximateValue) &&
			(i3Math::abs(i3Vector::GetY(&pVec3) - 1.0f) < ApproximateValue) &&
			(i3Math::abs(i3Vector::GetZ(&pVec3) - 1.0f) < ApproximateValue))
		{
			++nIdentityCount;
		}
	}

	if (nIdentityCount == nFrameCount)
	{
		pTransSeq->SetKeyframeChannels((i3TransformSequence::I3_KEY_CHANNEL)((UINT32)pTransSeq->GetKeyframeChannels() & ~i3TransformSequence::SCALE));
	}

	return pAnim;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//키프레임중에 연속된시간에 동일한 값을 가지고있는 데이터들을 삭제하여줍니다.
//삭제대상은 1 - 키프레임의 시간을 제외한 모든요소가 동일한경우 삭제
//           2 - 각요소별로 처음값에서 I3_EPS이하의 변동률을 가지고있는 요소를 FIXED 로 설정
//삭제되지 않은 요소들을 바탕으로 새로운 크기의 i3TransformSource를 생성하여 삭제여부반영하여
// return 
////////////////////////////////////////////////////////////////////////////////////////////////////
i3TransformSource *	i3OptAnimation::OptEraseSameKeyFrame(i3TransformSource * pAnim, REAL32	ApproximateValue, bool bDeleteTranslation /*= true*/)
{
	i3TransformSequence * pTransSeq = (i3TransformSequence *)pAnim;
	INT32	nFrameCount = pTransSeq->GetKeyframeCount();

	VEC3D		VecTrans;
	QUATERNION	QuatRot;
	VEC3D		VecScale;

	VEC3D		VecOldTrans;
	QUATERNION	QuatOldRot;
	VEC3D		VecOldScale;

	UINT32		CurIndex = 0;

	i3::vector<INT32> UniqueIndex;

	UINT32		nCountTranslate = 0;
	UINT32		nCountRotate = 0;
	UINT32		nCountScale = 0;

	INT32		nSameKeyCount = 0;//동일한 키값을 가진 값이 몇번이었는지 테스트

								  //같은 키프레임값을 가졌는지 선별하는 작업. 리스트에 연속되지않는 다른값을 키프레임들의 인덱스를 저장
	for (INT32 nCount = 0; nCount < nFrameCount; ++nCount)
	{
		bool	bSameKey = true;
		if (nCount == 0) bSameKey = false;//제일 처음의 값은 비교대상이없으므로 리스트에 집어넣기위해 false 로 설정

		if (pTransSeq->HasTranslation() && !(pTransSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE))
		{
			pTransSeq->GetTranslation(nCount, &VecTrans);
			if (nCount == 0)	i3Vector::Copy(&VecOldTrans, &VecTrans);

			if (nCount > 0)
			{
				if (!Vec3dApproximatelySame(&VecTrans, &VecOldTrans, ApproximateValue))
				{
					bSameKey = false;
					i3Vector::Copy(&VecOldTrans, &VecTrans);
					++nCountTranslate;
				}
			}
		}

		if (pTransSeq->HasRotation() && !(pTransSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE))
		{
			pTransSeq->GetRotation(nCount, &QuatRot);
			if (nCount == 0)	i3Quat::Copy(&QuatOldRot, &QuatRot);

			if (nCount > 0)
				if (!QuaternionApproximatelySame(&QuatRot, &QuatOldRot, ApproximateValue))
				{
					bSameKey = false;
					i3Quat::Copy(&QuatOldRot, &QuatRot);
					++nCountRotate;
				}
		}

		if (pTransSeq->HasScale() && !(pTransSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE))
		{
			pTransSeq->GetScale(nCount, &VecScale);
			if (nCount == 0)	i3Vector::Copy(&VecOldScale, &VecScale);

			if (nCount > 0)
				if (!Vec3dApproximatelySame(&VecScale, &VecOldScale, ApproximateValue))
				{
					bSameKey = false;
					i3Vector::Copy(&VecOldScale, &VecScale);
					++nCountScale;
				}
		}

		if (!bSameKey)
		{
			if (nSameKeyCount >= 2)
			{
				UniqueIndex.push_back(nCount - 1);
				CurIndex++;
			}

			UniqueIndex.push_back(nCount);
			CurIndex++;

			nSameKeyCount = 0;
		}

		nSameKeyCount++;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//처음값과 마지막값이 모두 같은경우 fixed 로 세팅
	//(0이란 HAS 조건에대한 보장은 없으며 , 0번인덱스의 값과 마지막사이값의 변화량이 0.000001f 이하
	if ((nCountTranslate == 0) && pTransSeq->HasTranslation())
		pTransSeq->SetKeyframeChannels((i3TransformSequence::I3_KEY_CHANNEL)((UINT32)pTransSeq->GetKeyframeChannels() | i3TransformSequence::FIXED_TRANSLATE));

	if ((nCountRotate == 0) && pTransSeq->HasRotation())
		pTransSeq->SetKeyframeChannels((i3TransformSequence::I3_KEY_CHANNEL)((UINT32)pTransSeq->GetKeyframeChannels() | i3TransformSequence::FIXED_ROTATE));

	if ((nCountScale == 0) && pTransSeq->HasScale())
		pTransSeq->SetKeyframeChannels((i3TransformSequence::I3_KEY_CHANNEL)((UINT32)pTransSeq->GetKeyframeChannels() | i3TransformSequence::FIXED_SCALE));
	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//최적화될 대상키프레임이 없는경우 오리지널값을 리턴
	if ((CurIndex == pTransSeq->GetKeyframeCount()) && !bDeleteTranslation)//변동사항이 없으면
	{
		pTransSeq->AddRef();			// 레퍼런스 카운트를 반드시 심어주도록 해야함...(날로 먹지 말것..)
		return pTransSeq;
	}

	//새로운 크기의 시퀀스를 생성하여 연결
	i3TransformSource * pTransSource = i3TransformSequence::new_object();
	i3TransformSequence *pTrans = (i3TransformSequence *)pTransSource;

	UINT32 keyChannel = (UINT32)pTransSeq->GetKeyframeChannels();

	if ((bDeleteTranslation == true))// && (nCountTranslate == 0) )
		keyChannel &= ~(UINT32)(i3TransformSequence::TRANSLATE);

	pTrans->Create(CurIndex, pTransSeq->GetDuration(), (i3TransformSequence::I3_KEY_CHANNEL) keyChannel);

	pTrans->SetKeyframeInterval(pTransSeq->GetKeyframeInterval());


	{
		if (keyChannel & (UINT32)i3TransformSequence::TRANSLATE)
		{
			if (pTrans->HasTranslation())
				pTrans->SetInterpolateMode(i3TransformSequence::TRANSLATE, pTransSeq->GetTranslateInterpolateMode());
		}
		if (pTrans->HasRotation())
			pTrans->SetInterpolateMode(i3TransformSequence::ROTATE, pTransSeq->GetRotateInterpolateMode());
		if (pTrans->HasScale())
			pTrans->SetInterpolateMode(i3TransformSequence::SCALE, pTransSeq->GetScaleInterpolateMode());
	}

	for (UINT32 i = 0; i < CurIndex; ++i)
	{
		INT32  nIndex = UniqueIndex[i];

		if (pTrans->HasTime())
		{
			pTrans->SetTime(i, pTransSeq->GetTime(nIndex));
		}

		if (keyChannel & (UINT32)i3TransformSequence::TRANSLATE)
		{
			if (pTrans->HasTranslation())
			{
				pTransSeq->GetTranslation(nIndex, &VecTrans);

				if ((i < 1) || !(pTrans->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE))
					pTrans->SetTranslation(i, &VecTrans);
			}
		}

		if (pTrans->HasRotation())
		{
			pTransSeq->GetRotation(nIndex, &QuatRot);

			if ((i < 1) || !(pTrans->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE))
				pTrans->SetRotation(i, &QuatRot);
		}

		if (pTrans->HasScale())
		{
			pTransSeq->GetScale(nIndex, &VecScale);

			if ((i < 1) || !(pTrans->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE))
				pTrans->SetScale(i, &VecScale);
		}
	}

	return pTransSource;
}

namespace
{
	bool	isSame_sample_Lerp(REAL32 sampleTime, const VEC3D& vSample, REAL32 startTime, const VEC3D& vStart, REAL32 endTime, const VEC3D& vEnd, REAL32 approximateVal)
	{
		REAL32 LerpRatio = (sampleTime - startTime) / (endTime - startTime);

		VEC3D vLerpResult0;
		i3Vector::Lerp(&vLerpResult0, &vStart, &vEnd, LerpRatio);

		bool isSameX = i3Math::isAlike_WithAbsRelEPS(vLerpResult0.x, vSample.x, approximateVal);
		bool isSameY = i3Math::isAlike_WithAbsRelEPS(vLerpResult0.y, vSample.y, approximateVal);
		bool isSameZ = i3Math::isAlike_WithAbsRelEPS(vLerpResult0.z, vSample.z, approximateVal);

		return (isSameX && isSameY && isSameZ);
	}
	
	bool	isSame_sample_Slerp(REAL32 sampleTime, const QUATERNION& qSample, REAL32 startTime, const QUATERNION& qStart, 
		REAL32 endTime, const QUATERNION& qEnd, REAL32 approximateVal)
	{
		QUATERNION qSample_Res = qSample;

		REAL32 LerpRatio = (sampleTime - startTime) / (endTime - startTime);

		QUATERNION qLerpResult0;
		i3Quat::Slerp(&qLerpResult0, (QUATERNION*)&qStart, (QUATERNION*)&qEnd, LerpRatio);

		// 쿼터니온의 경우 단위벡터로 바꿔야한다..
		qSample_Res.Normalize();
		qLerpResult0.Normalize();
		
//		bool isSameX = i3Math::isAlike_WithAbsRelEPS(qLerpResult0.x, qSample_Res.x, approximateVal);
	//	bool isSameY = i3Math::isAlike_WithAbsRelEPS(qLerpResult0.y, qSample_Res.y, approximateVal);
	//	bool isSameZ = i3Math::isAlike_WithAbsRelEPS(qLerpResult0.z, qSample_Res.z, approximateVal);
	//	bool isSameW = i3Math::isAlike_WithAbsRelEPS(qLerpResult0.w, qSample_Res.w, approximateVal);

	//	return (isSameX && isSameY && isSameZ && isSameW);
		REAL32 fDot = VEC4D::Dot(qSample_Res, qLerpResult0);

		if (fDot < 0.f)		// 으응...부호 일치가 필요하다..
		{
			qLerpResult0.x = -qLerpResult0.x;
			qLerpResult0.y = -qLerpResult0.y;
			qLerpResult0.z = -qLerpResult0.z;
			qLerpResult0.w = -qLerpResult0.w;
		}

//		bool isSame = (fDot >= (1.f - approximateVal));

		
		REAL32 absLen = fabs(qSample_Res.x - qLerpResult0.x) +
						fabs(qSample_Res.y - qLerpResult0.y) +
						fabs(qSample_Res.z - qLerpResult0.z) +
						fabs(qSample_Res.w - qLerpResult0.w);

		bool isSame = (absLen < approximateVal);
		
//		bool isSame = i3Math::isAlike_WithAbsRelEPS(fDot, 1.f, approximateVal);

		return isSame;
	}

	void		CalcRelativeQuaternion(QUATERNION& out, const QUATERNION& a, const QUATERNION& b)
	{
		// res = a.inverse() * b 라고 합니다...
		QUATERNION inv_a;
		i3Quat::Inverse(&inv_a, const_cast<QUATERNION*>(&a));
		i3Quat::Mul(&out, &inv_a, const_cast<QUATERNION*>(&b) );			// 내부에 노멀라이즈가 있는 모양임..
	}

	// 2개의 쿼터니온이 반대방향스러운 경우 true로 ...
	bool		IsQuaternionOpposite(const QUATERNION& a, const QUATERNION& b)
	{
		REAL32 rDot = i3Vector::Dot(const_cast<QUATERNION*>(&a), const_cast<QUATERNION*>(&b));
		return rDot < 0.f;
	}

	REAL32		QuaternionDot(const QUATERNION& a, const QUATERNION& b)
	{
		REAL32 rDot = i3Vector::Dot(const_cast<QUATERNION*>(&a), const_cast<QUATERNION*>(&b));
		return rDot;
	}
}


i3TransformSource * i3OptAnimation::OptEraseSameKeyFrame_Lerp(i3TransformSource * pAnim, REAL32	ApproximateValue, bool bDeleteTranslation)
{
	//
	// TIME셋팅이 반드시 필요하기 때문에 이 정보가 없고 최적화로 인한 변경이 가능하다면 반드시 새로운 i3TransformSource에 TIME반영하여 적용해야한다..
	// 최소한 첫값과 끝값은 보관되기 때문에, FIXED 옵션이 새로 추가되는 일은 없다..
	// 
	
	i3TransformSequence * pTransSeq = (i3TransformSequence *)pAnim;

	INT32	nFrameCount = pTransSeq->GetKeyframeCount();

	
	VEC3D		VecTrans;	QUATERNION	QuatRot;	VEC3D		VecScale;
	VEC3D		VecOldTrans;	QUATERNION	QuatOldRot;	VEC3D		VecOldScale;

	bool		HasTranslation_From_InputSeq = pTransSeq->HasTranslation();
	bool		HasRotation_From_InputSeq = pTransSeq->HasRotation();
	bool		HasScale_From_InputSeq = pTransSeq->HasScale();
	bool		HasTime_From_InputSeq = pTransSeq->HasTime();

	bool		HasFixedTranslateOption_From_InputSeq = (pTransSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE) != 0;
	bool		HasFixedRotateOption_From_InputSeq = (pTransSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE) != 0;
	bool		HasFixedScaleOption_From_InputSeq = (pTransSeq->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE) != 0;
	
	I3ASSERT(HasTranslation_From_InputSeq || HasRotation_From_InputSeq || HasScale_From_InputSeq);		// 셋중의 하나는 있어야됨..
	
	// 모든 보간은 선형 보간 혹은 구형선형 보간일 것이며, 이것을 벗어난 구현이 엔진에 없기 때문에 보간 최적화를 행한다..
	
	VEC3D		vSampleTrans;					// 보간값 일치성 검사..
	QUATERNION  qSampleRot;
	VEC3D		vSampleScale;

	INT32		startCompareIdx = 0;
//	INT32		sampleIdx0 = 1;
//	INT32		sampleIdx1 = -1;
	
	i3::vector<INT32>	vecIndexToErase;				// 기존 최적화와 달리 지워야 될 인덱스를 모으도록 한다...

	for (INT32 nCount = 2; nCount < nFrameCount; ++nCount)			// 2부터 시작한다... 중간인덱스를 지우므로 인덱스 1의 값은 유효해야한다.
	{
				
		REAL32 currTime = (HasTime_From_InputSeq) ? pTransSeq->GetTime(nCount) : pTransSeq->GetKeyframeInterval() * nCount;
		REAL32 StartTime = (HasTime_From_InputSeq) ? pTransSeq->GetTime(startCompareIdx) : pTransSeq->GetKeyframeInterval() * startCompareIdx;
			
//		REAL32	sampleTime0 = (HasTime_From_InputSeq) ? pTransSeq->GetTime(sampleIdx0) : pTransSeq->GetKeyframeInterval() * sampleIdx0;
		
//		REAL32 sampleTime1 = 0.f;
			
//		if (sampleIdx1 != -1)
//			sampleTime1 = (HasTime_From_InputSeq) ? pTransSeq->GetTime(sampleIdx1) : pTransSeq->GetKeyframeInterval() * sampleIdx1;
					
		bool canDeleteSampleIndex = true;
		
		if ( HasTranslation_From_InputSeq && !(HasFixedTranslateOption_From_InputSeq) )
		{
			pTransSeq->GetTranslation(nCount, &VecTrans);					// 보간식 최신값..
			pTransSeq->GetTranslation(startCompareIdx, &VecOldTrans);		// 보간식 시작값..

			const INT32 endCompareIdx = nCount; // -1;
			
			for (INT32 j = startCompareIdx + 1; j < endCompareIdx; ++j)
			{
				REAL32	sampleTime = (HasTime_From_InputSeq) ? pTransSeq->GetTime(j) : pTransSeq->GetKeyframeInterval() * j;

				pTransSeq->GetTranslation(j, &vSampleTrans);			// 비교할 이전 값..

				bool sameResult = isSame_sample_Lerp(sampleTime, vSampleTrans, StartTime, VecOldTrans, currTime, VecTrans, ApproximateValue);

				if (sameResult == false)
				{
					canDeleteSampleIndex = false;
					break;
				}
			}

			/*
			pTransSeq->GetTranslation(sampleIdx0, &vSampleTrans);			// 비교할 이전 값..
							
			bool sameResult0 = isSame_sample_Lerp(sampleTime0, vSampleTrans, StartTime, VecOldTrans, currTime, VecTrans, ApproximateValue);

			if (sameResult0 == false)
			{
				canDeleteSampleIndex = false;
			}
			else
			if (sampleIdx1 != -1)
			{
				pTransSeq->GetTranslation(sampleIdx1, &vSampleTrans);
				
				bool sameResult1 = isSame_sample_Lerp(sampleTime1, vSampleTrans, StartTime, VecOldTrans, currTime, VecTrans, ApproximateValue);
						
				if (sameResult1 == false)
				{
					canDeleteSampleIndex = false;
				}
			}
			*/
		}

		if ( canDeleteSampleIndex && HasRotation_From_InputSeq && !(HasFixedRotateOption_From_InputSeq))
		{
			pTransSeq->GetRotation(nCount, &QuatRot);
			pTransSeq->GetRotation(startCompareIdx, &QuatOldRot);		// 보간식 시작값..

			const INT32 endCompareIdx = nCount; // -1;

			for (INT32 j = startCompareIdx + 1; j < endCompareIdx; ++j)
			{
				REAL32	sampleTime = (HasTime_From_InputSeq) ? pTransSeq->GetTime(j) : pTransSeq->GetKeyframeInterval() * j;

				pTransSeq->GetRotation(j, &qSampleRot);			// 비교할 이전 값..

				bool sameResult = isSame_sample_Slerp(sampleTime, qSampleRot, StartTime, QuatOldRot, currTime, QuatRot, 0.0001f ); // 0.00025f); // ApproximateValue);

				if (sameResult == false)
				{
					canDeleteSampleIndex = false;
					break;
				}
			}

			/*
			pTransSeq->GetRotation(sampleIdx0, &qSampleRot);			// 비교할 이전 값..

			bool sameResult0 = isSame_sample_Slerp(sampleTime0, qSampleRot, StartTime, QuatOldRot, currTime, QuatRot, ApproximateValue); // 0.00001f); //  ApproximateValue * 100.f); // 0.00001f);

			if (sameResult0 == false)
			{
				canDeleteSampleIndex = false;
			}
			else
			if (sampleIdx1 != -1)
			{
				pTransSeq->GetRotation(sampleIdx1, &qSampleRot);

				bool sameResult1 = isSame_sample_Slerp(sampleTime1, qSampleRot, StartTime, QuatOldRot, currTime, QuatRot, ApproximateValue); // 0.00001f //ApproximateValue * 100.f); // 0.00001f);

				if (sameResult1 == false)
				{
					canDeleteSampleIndex = false;
				}
			}
			*/
		}
		
		if (canDeleteSampleIndex && HasScale_From_InputSeq && !(HasFixedScaleOption_From_InputSeq) )
		{
			pTransSeq->GetScale(nCount, &VecScale);
			pTransSeq->GetScale(startCompareIdx, &VecOldScale);		// 보간식 시작값..

			const INT32 endCompareIdx = nCount; // -1;

			for (INT32 j = startCompareIdx + 1; j < endCompareIdx; ++j)
			{
				REAL32	sampleTime = (HasTime_From_InputSeq) ? pTransSeq->GetTime(j) : pTransSeq->GetKeyframeInterval() * j;

				pTransSeq->GetScale(j, &vSampleScale);			// 비교할 이전 값..

				bool sameResult = isSame_sample_Lerp(sampleTime, vSampleScale, StartTime, VecOldScale, currTime, VecScale, ApproximateValue);

				if (sameResult == false)
				{
					canDeleteSampleIndex = false;
					break;
				}
			}
			
			/*
			pTransSeq->GetScale(sampleIdx0, &vSampleScale);			// 비교할 이전 값..

			bool sameResult0 = isSame_sample_Lerp(sampleTime0, vSampleScale, StartTime, VecOldScale, currTime, VecScale, ApproximateValue);

			if (sameResult0 == false)
			{
				canDeleteSampleIndex = false;
			}
			else
			if (sampleIdx1 != -1)
			{
				pTransSeq->GetScale(sampleIdx1, &vSampleScale);

				bool sameResult1 = isSame_sample_Lerp(sampleTime1, vSampleScale, StartTime, VecOldScale, currTime, VecScale, ApproximateValue);

				if (sameResult1 == false)
				{
					canDeleteSampleIndex = false;
				}
			}
			*/
		}

		if (canDeleteSampleIndex) // 일단 이 상태에서 회전값이 존재할 시...삭제지점 앞뒤 포지션이 존재할때, 반대방향을 
		{  // 가르킨다면 점제거를 포기하도록 처리해야한다..
			INT32 eraseIdx = nCount - 1;		// (sampleIdx1 == -1) ? sampleIdx0 : sampleIdx1;

			if (HasRotation_From_InputSeq && !(HasFixedRotateOption_From_InputSeq))
			{

				QUATERNION relA;	QUATERNION relB;

				pTransSeq->GetRotation(eraseIdx, &qSampleRot);			// 비교할 이전 값..
				pTransSeq->GetRotation(eraseIdx + 1, &QuatRot);			// 최신값..
				CalcRelativeQuaternion(relB, qSampleRot, QuatRot);

				for (INT32 j = startCompareIdx; j < eraseIdx; ++j)
				{
//					pTransSeq->GetRotation(eraseIdx - 1, &QuatOldRot);		// 보간식 시작값..
					pTransSeq->GetRotation(j, &QuatOldRot);
					CalcRelativeQuaternion(relA, QuatOldRot, qSampleRot);
					
					REAL32 rDot = QuaternionDot(relA, relB);

					if (rDot < 1.f - 0.0001f )
					{
						canDeleteSampleIndex = false;
						break;
					}

				}
							
			
			//	bool bOpposite = IsQuaternionOpposite(relA, relB);
			//	if (bOpposite)
			//	{
			//		canDeleteSampleIndex = false;
			//	}

			//	REAL32 rDot = QuaternionDot(relA, relB);

			//	if (rDot < 1.f - 0.001f)
			//	{
			//		canDeleteSampleIndex = false;
			//	}
			}

		}
		
		if (canDeleteSampleIndex == false)			// 삭제 불능 인덱스가 나오면, 그전까지의 기록을 적재함..
		{
			INT32 eraseStartIndex = startCompareIdx + 1;
			INT32 eraseEndIndex = nCount - 1;						// 이것은 포함되면 안되고 , 이전인덱스까지..

			if (eraseEndIndex - eraseStartIndex > 0)
			{
				for (INT32 j = eraseStartIndex; j < eraseEndIndex; ++j)
				{
					vecIndexToErase.push_back(j);
				}
			}

			startCompareIdx = eraseEndIndex;		// 여기서부터 재계산...
		}
		

		/*
		if (canDeleteSampleIndex)			// 지울수 있다면 지울수 있는것만 기록한다..  (기존 최적화와는 최적화를 위해 담는 자료구조처리가 다르다..)
		{
			if (sampleIdx1 == -1)			// 두번째 인덱스가 없다면 샘플은 하나 뿐이므로 그것을 지우는 인덱스로 등록..
			{
				vecIndexToErase.push_back(sampleIdx0);					// sampleIdx0은 계속 비교를 위해 재활용되므로 여기서 리셋하지 않는다..
			}
			else							// 두번째 인덱스가 있다면 이것을 지워나간다..  
			{
				vecIndexToErase.push_back(sampleIdx1);					
			}

			sampleIdx1 = nCount;			// sampleIdx1은 이제 nCount로 유지하고 다음번 비교에서 사용된다..
		}
		else                                // 중간인덱스를 소거할수 없을정도로 차이가 난 경우라면, StartIdx와 SampleIdx들을 리셋해주어야 한다..
		{
			if (sampleIdx1 != -1)
			{
				startCompareIdx = sampleIdx1;
			}
			else
			{
				startCompareIdx = sampleIdx0;
			}
			sampleIdx0 = nCount;
			sampleIdx1 = -1;
		}
		*/	
	}

	if ( startCompareIdx + 2 < nFrameCount)
	{
		INT32 eraseStartIndex = startCompareIdx + 1;
		INT32 eraseEndIndex = nFrameCount - 1;			// 이것은 포함되면 안되고 , 이전인덱스까지..

		if (eraseEndIndex - eraseStartIndex > 0)
		{
			for (INT32 j = eraseStartIndex; j < eraseEndIndex; ++j)
			{
				vecIndexToErase.push_back(j);
			}
		}

	//	startCompareIdx = eraseEndIndex;		// 여기서부터 재계산...
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

	//최적화될 대상키프레임이 없는경우 오리지널값을 리턴
	if ( vecIndexToErase.empty() && !bDeleteTranslation)//변동사항이 없으면
	{
		pTransSeq->AddRef();			// 레퍼런스 카운트를 반드시 심어주도록 해야함...(날로 먹지 말것..)
		return pTransSeq;
	}

	INT32 numErase = vecIndexToErase.size();
	
	// 시간값이 삭제된 경우 용량 대비 효율을 좀더 따져야 할것 같다..
	if (HasTime_From_InputSeq == false && numErase > 0 )
	{
		// nFrameCount
		INT32 oldSaveSize = nFrameCount * sizeof(REAL32);	

		INT32 newSaveSize = numErase * sizeof(VEC4D);			// 일단 가장 큰 타입 놈 하나를 집어보았다.
		
		if (oldSaveSize >= newSaveSize)
		{
			vecIndexToErase.clear();						// 최적화 취소의 효과임..
			I3TRACE("AnimOPT : Lerp optimization cancelled..... : oldSaveSize = %d bytes, newSaveSize = %d bytes . Tot_NumFrame = %d \n", oldSaveSize, newSaveSize, nFrameCount);
		}
	}
	else
	if (numErase > 0)
	{
		I3TRACE("AnimOPT : Lerp optimization Applied!!!!! : newSaveSize = %d bytes. Removed_NumFrame : %d , Tot_NumFrame : %d \n", numErase * sizeof(VEC4D), numErase, nFrameCount);
	}

	i3::vector<INT32> UniqueIndex;
	size_t erase_counter = 0;
	for (INT32 i = 0; i < nFrameCount; ++i)
	{
		if (erase_counter >= vecIndexToErase.size())
		{
			UniqueIndex.push_back(i);
		}
		else
		{
			INT32 idxToErase = vecIndexToErase[erase_counter];

			if (idxToErase == i)
			{
				++erase_counter;
			}
			else
			{
				UniqueIndex.push_back(i);
			}
		}
	}


	//새로운 크기의 시퀀스를 생성하여 연결
	i3TransformSource * pTransSource = i3TransformSequence::new_object();
	i3TransformSequence *pTrans = (i3TransformSequence *)pTransSource;

	UINT32 keyChannel = (UINT32)pTransSeq->GetKeyframeChannels();

	if ((bDeleteTranslation == true))// && (nCountTranslate == 0) )
		keyChannel &= ~(UINT32)(i3TransformSequence::TRANSLATE);

	if (HasTime_From_InputSeq == false && vecIndexToErase.empty() == false)
		keyChannel |= (UINT32)(i3TransformSequence::TIME);			// 시간값이 없던 경우는 이제 포함되어야 함...

	INT32 newKeyCount = INT32(UniqueIndex.size());

	pTrans->Create(newKeyCount, pTransSeq->GetDuration(), (i3TransformSequence::I3_KEY_CHANNEL) keyChannel);

	if (keyChannel & i3TransformSequence::TIME)
	{
		pTrans->SetKeyframeInterval(0.f);
		pTrans->SetTimeOffset(0.f);
	}
	else // 갱신될것도 타임값 없고..
	{
		if (HasTime_From_InputSeq == false)		// 기존의 경우도 타임값이 없다면...
		{
			pTrans->SetKeyframeInterval(pTransSeq->GetKeyframeInterval());
			pTrans->SetTimeOffset(pTransSeq->GetTimeOffset());
		}
		else
		{
			I3ASSERT_0;
		}
	}
		
	{
		if (keyChannel & (UINT32)i3TransformSequence::TRANSLATE)
		{
			if (pTrans->HasTranslation())
				pTrans->SetInterpolateMode(i3TransformSequence::TRANSLATE, pTransSeq->GetTranslateInterpolateMode());
		}
		if (pTrans->HasRotation())
			pTrans->SetInterpolateMode(i3TransformSequence::ROTATE, pTransSeq->GetRotateInterpolateMode());
		if (pTrans->HasScale())
			pTrans->SetInterpolateMode(i3TransformSequence::SCALE, pTransSeq->GetScaleInterpolateMode());
	}

	for (INT32 i = 0; i < newKeyCount; ++i)
	{
		INT32  nIndex = UniqueIndex[i];

		if (HasTime_From_InputSeq)
		{
			pTrans->SetTime(i, pTransSeq->GetTime(nIndex));
		}
		else
		if (keyChannel & (UINT32)i3TransformSequence::TIME)
		{
			pTrans->SetTime(i, pTransSeq->GetTimeOffset() + nIndex * pTransSeq->GetKeyframeInterval());
		}
		
		if (keyChannel & (UINT32)i3TransformSequence::TRANSLATE)
		{
			if (pTrans->HasTranslation())
			{
				pTransSeq->GetTranslation(nIndex, &VecTrans);

				if ((i < 1) || !(pTrans->GetKeyframeChannels() & i3TransformSequence::FIXED_TRANSLATE))
					pTrans->SetTranslation(i, &VecTrans);
			}
		}

		if (pTrans->HasRotation())
		{
			pTransSeq->GetRotation(nIndex, &QuatRot);

			if ((i < 1) || !(pTrans->GetKeyframeChannels() & i3TransformSequence::FIXED_ROTATE))
				pTrans->SetRotation(i, &QuatRot);
		}

		if (pTrans->HasScale())
		{
			pTransSeq->GetScale(nIndex, &VecScale);

			if ((i < 1) || !(pTrans->GetKeyframeChannels() & i3TransformSequence::FIXED_SCALE))
				pTrans->SetScale(i, &VecScale);
		}
	}

	return pTransSource;

}




//모든 키프레임들의 Time Step 이 균등한 경우에 Time Key의 값을 계산할 필요가 없음을 지정
i3TransformSource *	i3OptAnimation::OptTimeStep(i3TransformSource * pAnim, REAL32 ApproximateValue)
{
	i3TransformSequence * pTrans = (i3TransformSequence *)pAnim;

	UINT32	nKeyFrameCount = pTrans->GetKeyframeCount();
	if (pTrans->HasTime())
	{
		if (nKeyFrameCount > 2)
		{
			REAL32 step, gap;
			UINT32	SameCount = 2;

			step = pTrans->GetTime(1) - pTrans->GetTime(0);

			if (nKeyFrameCount > 2)
			{
				for (UINT32 i = 2; i < nKeyFrameCount; i++)
				{
					gap = pTrans->GetTime(i) - pTrans->GetTime(i - 1);

					if (i3Math::abs(gap - step) < ApproximateValue)
					{
						SameCount++;
					}
				}

				if (SameCount == nKeyFrameCount)
				{
					//					if( i3Math::abs( pTrans->GetDuration() - (step * (SameCount-1))) < ApproximateValue)
					//					{	//타임간격의 
					// Time Key 는 일정한 스텝을 가지기때문에 따로계산할 필요없다고 설정
					pTrans->SetKeyframeInterval(step);
					pTrans->SetTimeOffset(pTrans->GetTime(0));
					pTrans->SetKeyframeChannels((i3TransformSequence::I3_KEY_CHANNEL)(pTrans->GetKeyframeChannels() & ~i3TransformSequence::TIME));
					//					}
				}
			}
		}
	}
	return pAnim;
}

//ApproximateValue 보다 작은정도의 차이일경우 같음을..큰정도의 차이면 다름을 리턴
bool	i3OptAnimation::Vec3dApproximatelySame(VEC3D * pVec1, VEC3D * pVec2, REAL32	ApproximateValue)
{
	VEC3D TempVec3d;
	i3Vector::Sub(&TempVec3d, pVec1, pVec2);

	if ((i3Math::abs(i3Vector::GetX(&TempVec3d)) < ApproximateValue) &&
		(i3Math::abs(i3Vector::GetY(&TempVec3d)) < ApproximateValue) &&
		(i3Math::abs(i3Vector::GetZ(&TempVec3d)) < ApproximateValue))
	{
		return true;
	}

	return false;
}

bool	i3OptAnimation::QuaternionApproximatelySame(QUATERNION * pQuat1, QUATERNION * pQuat2, REAL32	ApproximateValue)
{
	if ((i3Math::abs(i3Quat::GetA(pQuat1) - i3Quat::GetA(pQuat2)) < ApproximateValue) &&
		(i3Math::abs(i3Quat::GetB(pQuat1) - i3Quat::GetB(pQuat2)) < ApproximateValue) &&
		(i3Math::abs(i3Quat::GetC(pQuat1) - i3Quat::GetC(pQuat2)) < ApproximateValue) &&
		(i3Math::abs(i3Quat::GetD(pQuat1) - i3Quat::GetD(pQuat2)) < ApproximateValue))
	{
		return true;
	}

	else	return false;
}

void	i3OptAnimation::LogAnimation(i3TransformSource * pAnim, char * szFileName)
{
	VEC3D	VecTrans;
	VEC3D	VecScale;
	QUATERNION	QuatRot;
	char	buf[256];

	FILE * pFile = fopen(szFileName, "w+");

	if (pFile == nullptr)
		return;

	i3TransformSequence * pTransSeq = (i3TransformSequence *)pAnim;

	UINT32 nKeyFrameCount = pTransSeq->GetKeyframeCount();

	sprintf(buf, "KEYCOUNT                   : %d\n", nKeyFrameCount);
	fwrite(buf, sizeof(char), strlen(buf), pFile);
	sprintf(buf, "TRANSLATE INTERPOLATE MODE : %d\n", pTransSeq->GetTranslateInterpolateMode());
	fwrite(buf, sizeof(char), strlen(buf), pFile);
	sprintf(buf, "ROTATE    INTERPOLATE MODE : %d\n", pTransSeq->GetRotateInterpolateMode());
	fwrite(buf, sizeof(char), strlen(buf), pFile);
	sprintf(buf, "SCALE     INTERPOLATE MODE : %d\n", pTransSeq->GetScaleInterpolateMode());
	fwrite(buf, sizeof(char), strlen(buf), pFile);

	if (!pTransSeq->HasTime())
	{
		sprintf(buf, "TimeStep %.7f \t ", pTransSeq->GetKeyframeInterval());
		fwrite(buf, sizeof(char), strlen(buf), pFile);

		sprintf(buf, "TimeDuration %.7f \t ", pTransSeq->GetDuration());
		fwrite(buf, sizeof(char), strlen(buf), pFile);
	}

	sprintf(buf, "KEY_CHANNEL : %X\n", pTransSeq->GetKeyframeChannels());
	fwrite(buf, sizeof(char), strlen(buf), pFile);

	sprintf(buf, "TimeOffSet %.7f\n", pTransSeq->GetTimeOffset());
	fwrite(buf, sizeof(char), strlen(buf), pFile);

	for (UINT32 i = 0; i < nKeyFrameCount; ++i)
	{
		if (pTransSeq->HasTime())
		{
			sprintf(buf, "Time : %.7f \t", pTransSeq->GetTime(i));
			fwrite(buf, sizeof(char), strlen(buf), pFile);
		}

		if (pTransSeq->HasTranslation())
		{
			pTransSeq->GetTranslation(i, &VecTrans);
			sprintf(buf, "TRANSLATION : %.7f \t %.7f \t %.7f\t", i3Vector::GetX(&VecTrans), i3Vector::GetY(&VecTrans), i3Vector::GetZ(&VecTrans));
			fwrite(buf, sizeof(char), strlen(buf), pFile);
		}

		if (pTransSeq->HasRotation())
		{
			pTransSeq->GetRotation(i, &QuatRot);
			sprintf(buf, "ROTATE     : %.7f \t %.7f \t %.7f \t %.7f\t", i3Quat::GetA(&QuatRot), i3Quat::GetB(&QuatRot), i3Quat::GetC(&QuatRot), i3Quat::GetD(&QuatRot));
			fwrite(buf, sizeof(char), strlen(buf), pFile);
		}

		if (pTransSeq->HasScale())
		{
			pTransSeq->GetScale(i, &VecScale);
			sprintf(buf, "SCALE       : %.7f \t %.7f \t %.7f \t", i3Vector::GetX(&VecScale), i3Vector::GetY(&VecScale), i3Vector::GetZ(&VecScale));
			fwrite(buf, sizeof(char), strlen(buf), pFile);
		}

		sprintf(buf, "\n");
		fwrite(buf, sizeof(char), strlen(buf), pFile);
	}

	fclose(pFile);
}