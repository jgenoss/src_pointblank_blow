#include "i3MathType.h"
#include "i3MathDefine.h"
#include "i3Animation2.h"
#include "i3Vector.h"
#include "i3Quat.h"
#include "i3Base/string/ext/safe_string_copy.h"


I3_CLASS_INSTANCE(i3Animation2);

i3Animation2::~i3Animation2(void)
{
	// 이 소거방식은 사실상 약점이 없는 것은 아님..(Create나 Load함수가 중복되면 중간에 메모리 소실은 있을수 있다...)
	if (m_pBindBuf == nullptr)
	{
		const INT32 numTrack = m_TrackCount;

		for (INT32 i = 0; i < numTrack; ++i)
		{
			i3::pack::animation::TRACK* pTrack = m_pSeq + i;

			if (pTrack->m_pTime)
				i3MemFree(pTrack->m_pTime);
			if (pTrack->m_pSIndex)
				i3MemFree(pTrack->m_pSIndex);
			if (pTrack->m_pRIndex)
				i3MemFree(pTrack->m_pRIndex);
			if (pTrack->m_pTIndex)
				i3MemFree(pTrack->m_pTIndex);
		}
		
		i3MemFree(m_pSeq);
	}
	
}

bool i3Animation2::Create(INT32 TrackCount, bool b16BitsMode /*= false*/)
{
	m_TrackCount = TrackCount;

	m_pTracks = (i3::pack::animation::TRACK_INFO *)i3::static_pool_malloc((sizeof(i3::pack::animation::TRACK_INFO) + sizeof(INT32)) * TrackCount);
	I3ASSERT(m_pTracks);

	memset(m_pTracks, 0, (sizeof(i3::pack::animation::TRACK_INFO) + sizeof(INT32)) * TrackCount);

	{
		m_pSeq = (i3::pack::animation::TRACK *)i3MemAlloc(sizeof(i3::pack::animation::TRACK) * TrackCount);		// 에라이썅...뒤에 m_pSeq처리하고 하나도 안맞네..
		I3ASSERT(m_pSeq != nullptr);														// 병신같은데, 유지보수는 해야겠고...

		i3mem::FillZero(m_pSeq, sizeof(i3::pack::animation::TRACK) * TrackCount);
	}

	m_bIs16Bits = b16BitsMode;

	return true;
}

void i3Animation2::Create(i3Animation * pAnim)
{
	INT32 i;

	I3ASSERT(pAnim->GetTrackCount() > 0);

	{
		if (pAnim->GetTrack(0)->GetKeyframeChannels() & i3TransformSequence::REAL16_FORMAT)
			Create(pAnim->GetTrackCount(), true);
		else
			Create(pAnim->GetTrackCount());

		for (i = 0; i < pAnim->GetTrackCount(); i++)
		{
			i3TransformSequence * pSeq = pAnim->GetTrack(i);
			m_pSeq[i].Create(pSeq);
		}
	}

	SetName(pAnim->GetNameString());
}

void i3::pack::animation::TRACK::Create(i3TransformSequence * pSeq)
{
	INT32 cnt;

	m_Channels = pSeq->GetKeyframeChannels();
	m_KeyCount = pSeq->GetKeyframeCount();
	m_KeyInterval = pSeq->GetKeyframeInterval();
	m_TimeOffset = pSeq->GetTimeOffset();
	m_Duration = pSeq->GetDuration();
	m_TIMode = (INT8)pSeq->GetTranslateInterpolateMode();
	m_RIMode = (INT8)pSeq->GetRotateInterpolateMode();
	m_SIMode = (INT8)pSeq->GetScaleInterpolateMode();

	// Translate
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		m_pTIndex = (INT32 *)i3MemAlloc(sizeof(INT32) * cnt);
	}

	// Rotation
	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		m_pRIndex = (INT32 *)i3MemAlloc(sizeof(INT32) * cnt);
	}

	// Scale
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		m_pSIndex = (INT32 *)i3MemAlloc(sizeof(INT32) * cnt);
	}

	// Time
	if (m_Channels & i3TransformSequence::TIME)
	{
		m_pTime = (REAL32 *)i3MemAlloc(sizeof(REAL32) * m_KeyCount);
	}
	else
	{
		I3ASSERT(m_KeyInterval > 0);
	}
}

INT32 i3::pack::animation::TRACK::CalcBufferSize(void)
{
	INT32 sz = 0, cnt;

	// Translate
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		sz += sizeof(INT32) * cnt;
	}

	// Rotation
	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		sz += sizeof(INT32) * cnt;
	}

	// Scale
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		sz += sizeof(INT32) * cnt;
	}

	// Time
	if (m_Channels & i3TransformSequence::TIME)
	{
		sz += sizeof(REAL32) * m_KeyCount;
	}

	return sz;
}

UINT32 i3::pack::animation::TRACK::StoreData(i3Stream * pFile)
{
	INT32 cnt;
	UINT32 rc, result = 0;

	// Translate
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		rc = pFile->Write(m_pTIndex, sizeof(INT32) * cnt);
		I3_CHKIO(rc);
		result += rc;
	}

	// Rotation
	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		rc = pFile->Write(m_pRIndex, sizeof(INT32) * cnt);
		I3_CHKIO(rc);
		result += rc;
	}

	// Scale
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		rc = pFile->Write(m_pSIndex, sizeof(INT32) * cnt);
		I3_CHKIO(rc);
		result += rc;
	}

	// Time
	if (m_Channels & i3TransformSequence::TIME)
	{
		rc = pFile->Write(m_pTime, sizeof(REAL32) * m_KeyCount);
		I3_CHKIO(rc);
		result += rc;
	}

	return result;
}

UINT32	 i3::pack::animation::TRACK::LoadData(char * pData)
{
	INT32 cnt = 0;

	// Translate
	m_pTIndex = (INT32 *)pData;
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)	cnt += 1;
		else													cnt += m_KeyCount;
	}

	// Rotation
	m_pRIndex = m_pTIndex + cnt;							// 쌍놈의 코드다...실행만 되면 장땡인가...앞뒤 코드도 안보고 메모리 관리 안해?...

	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)		cnt += 1;
		else													cnt += m_KeyCount;
	}

	// Scale
	m_pSIndex = m_pTIndex + cnt;
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)		cnt += 1;
		else													cnt += m_KeyCount;
	}

	// Time
	m_pTime = (REAL32 *)(m_pTIndex + cnt);
	if (m_Channels & i3TransformSequence::TIME)
	{
		cnt += m_KeyCount;
	}

	return cnt * sizeof(INT32);
}

INT32 i3::pack::animation::TRACK::_getTimeIndex(REAL32 tm)
{
	register INT32 si, ei, mi;

	si = 0;
	ei = m_KeyCount - 1;

	if (tm >= m_pTime[ei])
	{
		return ei;
	}

	do
	{
		mi = (si + ei) >> 1;

		if ((ei - si) <= 1)
		{
			break;
		}
		else if (m_pTime[mi] < tm)
		{
			si = mi;
		}
		else if (m_pTime[mi] > tm)
		{
			ei = mi;
		}
		else
		{
			si = mi;
			break;
		}
	} while ((ei - si) > 1);

	return si;
}


// 16Bits Track Info function
void i3::pack::animation::TRACK2::Create(i3TransformSequence * pSeq)
{
	INT32 cnt;

	m_Channels = pSeq->GetKeyframeChannels();

	I3ASSERT((m_Channels & i3TransformSequence::REAL16_FORMAT) == i3TransformSequence::REAL16_FORMAT);

	I3ASSERT(pSeq->GetKeyframeCount() < (UINT16)0xFFFF);
	m_KeyCount = (UINT16)pSeq->GetKeyframeCount();
	m_KeyInterval = pSeq->GetKeyframeInterval();
	m_TimeOffset = pSeq->GetTimeOffset();
	m_Duration = pSeq->GetDuration();
	m_TIMode = (INT8)pSeq->GetTranslateInterpolateMode();
	m_RIMode = (INT8)pSeq->GetRotateInterpolateMode();
	m_SIMode = (INT8)pSeq->GetScaleInterpolateMode();

	// Translate
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		m_pTIndex = (UINT16 *)i3MemAlloc(sizeof(UINT16) * cnt);
	}

	// Rotation
	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		m_pRIndex = (UINT16 *)i3MemAlloc(sizeof(UINT16) * cnt);
	}

	// Scale
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		m_pSIndex = (UINT16 *)i3MemAlloc(sizeof(UINT16) * cnt);
	}

	// Time
	if (m_Channels & i3TransformSequence::TIME)
	{
		m_pTime = (REAL16 *)i3MemAlloc(sizeof(REAL16) * m_KeyCount);
	}
	else
	{
		I3ASSERT(m_KeyInterval > 0);
	}
}

INT32 i3::pack::animation::TRACK2::CalcBufferSize(void)
{
	INT32 sz = 0, cnt;

	// Translate
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		sz += sizeof(UINT16) * cnt;
	}

	// Rotation
	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		sz += sizeof(UINT16) * cnt;
	}

	// Scale
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		sz += sizeof(UINT16) * cnt;
	}

	// Time
	if (m_Channels & i3TransformSequence::TIME)
	{
		sz += sizeof(REAL16) * m_KeyCount;
	}

	return sz;
}

UINT32 i3::pack::animation::TRACK2::StoreData(i3Stream * pFile)
{
	INT32 cnt;
	UINT32 rc, result = 0;

	// Translate
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		rc = pFile->Write(m_pTIndex, sizeof(UINT16) * cnt);
		I3_CHKIO(rc);
		result += rc;
	}

	// Rotation
	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		rc = pFile->Write(m_pRIndex, sizeof(UINT16) * cnt);
		I3_CHKIO(rc);
		result += rc;
	}

	// Scale
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)
			cnt = 1;
		else
			cnt = m_KeyCount;

		rc = pFile->Write(m_pSIndex, sizeof(UINT16) * cnt);
		I3_CHKIO(rc);
		result += rc;
	}

	// Time
	if (m_Channels & i3TransformSequence::TIME)
	{
		rc = pFile->Write(m_pTime, sizeof(REAL16) * m_KeyCount);
		I3_CHKIO(rc);
		result += rc;
	}

	return result;
}

UINT32	 i3::pack::animation::TRACK2::LoadData(char * pData)
{
	INT32 cnt = 0;

	// Translate
	m_pTIndex = (UINT16 *)pData;
	if (m_Channels & i3TransformSequence::TRANSLATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_TRANSLATE)	cnt += 1;
		else													cnt += m_KeyCount;
	}

	// Rotation
	m_pRIndex = m_pTIndex + cnt;
	if (m_Channels & i3TransformSequence::ROTATE)
	{
		if (m_Channels & i3TransformSequence::FIXED_ROTATE)		cnt += 1;
		else													cnt += m_KeyCount;
	}

	// Scale
	m_pSIndex = m_pTIndex + cnt;
	if (m_Channels & i3TransformSequence::SCALE)
	{
		if (m_Channels & i3TransformSequence::FIXED_SCALE)		cnt += 1;
		else													cnt += m_KeyCount;
	}

	// Time
	m_pTime = (REAL16 *)(m_pTIndex + cnt);
	if (m_Channels & i3TransformSequence::TIME)
	{
		cnt += m_KeyCount;
	}

	return cnt * sizeof(UINT16);
}

INT32 i3::pack::animation::TRACK2::_getTimeIndex(REAL32 tm)
{
	register UINT16 si, ei, mi;

	si = 0;
	ei = m_KeyCount - 1;

	if (tm >= (REAL32)m_pTime[ei])
	{
		return ei;
	}

	do
	{
		mi = (si + ei) >> 1;

		if ((ei - si) <= 1)
		{
			break;
		}
		else if ((REAL32)m_pTime[mi] < tm)
		{
			si = mi;
		}
		else if ((REAL32)m_pTime[mi] > tm)
		{
			ei = mi;
		}
		else
		{
			si = mi;
			break;
		}
	} while ((ei - si) > 1);

	return si;
}


INT32 i3Animation2::CalcBufferSize(void)
{
	INT32 i;
	INT32 sz = 0;

	if (m_pSeq != nullptr)
	{
		sz = sizeof(i3::pack::animation::TRACK) * GetTrackCount();

		sz += sizeof(i3::pack::animation::TRACK_INFO) * GetTrackCount();

		for (i = 0; i < GetTrackCount(); i++)
		{
			sz += m_pSeq[i].CalcBufferSize();
		}
	}
	else
	{
		I3ASSERT(m_pSeq2 != nullptr);

		sz = sizeof(i3::pack::animation::TRACK2) * GetTrackCount();

		sz += sizeof(i3::pack::animation::TRACK_INFO) * GetTrackCount();

		for (i = 0; i < GetTrackCount(); i++)
		{
			sz += m_pSeq2[i].CalcBufferSize();
		}
	}

	return sz;
}



UINT32	i3Animation2::GetInterpolatedKeyframe(INT32 idxTrack, I3MATRIXINFO * pInfo, bool bFirst)
{
	INT32 Idx, nextIdx;
	VEC3D vTemp1, vTemp2;
	QUATERNION qTemp1, qTemp2;

	UINT8 TIMode = 0, RIMode = 0, SIMode = 0;
	REAL32 t = 0.f;

	REAL32 _time = pInfo->m_Time;
	UINT32 Key = 0;
	REAL32 interv = 0.f;

	i3::pack::animation::TRACK * pSeq = nullptr;
	i3::pack::animation::TRACK2 * pSeq2 = nullptr;

	UINT32	Channels = 0;
	INT32	KeyCount = 0;

	if (m_pSeq != nullptr)
	{
		pSeq = m_pSeq + idxTrack;

		Channels = pSeq->m_Channels;
		KeyCount = pSeq->m_KeyCount;

		TIMode = pSeq->m_TIMode;
		RIMode = pSeq->m_RIMode;
		SIMode = pSeq->m_SIMode;

		interv = pSeq->m_KeyInterval;
		_time -= pSeq->m_TimeOffset;

		if( pInfo->m_PlayMode == I3_ANIM_PLAY_MODE_CLAMP || 
			pInfo->m_PlayMode == I3_ANIM_PLAY_MODE_MANUAL_CLAMP )
		{
			_time = i3Math::Min(pSeq->m_Duration, _time);
		}
		else if (pInfo->m_PlayMode != I3_ANIM_PLAY_MODE_STOP)
		{
			if (_time >= pSeq->m_Duration)
				_time = i3Math::fmod(_time, m_Duration);
		}

		if (Channels & i3TransformSequence::TIME)
			Idx = pSeq->_getTimeIndex(_time);
		else
			Idx = (UINT32)(i3Math::div(pInfo->m_Time, interv));

		if (Idx >= KeyCount - 1)
		{
			Idx = KeyCount - 1;

			if (pInfo->m_PlayMode != I3_ANIM_PLAY_MODE_REPEAT)
			{
				t = 0.0f;
				nextIdx = Idx;
			}
			else
			{
				t = 1.0f;
				nextIdx = 0;
			}
		}
		else
		{
			nextIdx = Idx + 1;

			if (Channels & i3TransformSequence::TIME)
			{
				REAL32 t1, t2;

				t1 = pSeq->m_pTime[Idx];
				t2 = pSeq->m_pTime[nextIdx];

				t = i3Math::div((_time - t1), ((t2 - t1)));
				//I3ASSERT( (t >= __RT_0) && (t <= __RT_1));
			}
			else
			{
				t = i3Math::div(i3Math::fmod(_time, pSeq->m_KeyInterval), pSeq->m_KeyInterval);
				//I3ASSERT( (t >= __RT_0) && (t <= __RT_1));
			}
		}
	}
	else
	{
		I3ASSERT(m_pSeq2 != nullptr);

		pSeq2 = m_pSeq2 + idxTrack;

		Channels = pSeq2->m_Channels;
		KeyCount = pSeq2->m_KeyCount;

		TIMode = pSeq2->m_TIMode;
		RIMode = pSeq2->m_RIMode;
		SIMode = pSeq2->m_SIMode;

		interv = pSeq2->m_KeyInterval;
		_time -= pSeq2->m_TimeOffset;

		if (pInfo->m_PlayMode == I3_ANIM_PLAY_MODE_CLAMP ||
			pInfo->m_PlayMode == I3_ANIM_PLAY_MODE_MANUAL_CLAMP)
		{
			_time = i3Math::Min(pSeq2->m_Duration, _time);
		}
		else if (pInfo->m_PlayMode != I3_ANIM_PLAY_MODE_STOP)
		{
			if (_time >= pSeq2->m_Duration)
				_time = i3Math::fmod(_time, m_Duration);
		}

		if (Channels & i3TransformSequence::TIME)
			Idx = pSeq2->_getTimeIndex(_time);
		else
			Idx = (UINT32)(i3Math::div(pInfo->m_Time, interv));

		if (Idx >= (INT32)KeyCount - 1)
		{
			Idx = KeyCount - 1;

			if (pInfo->m_PlayMode != I3_ANIM_PLAY_MODE_REPEAT)
			{
				t = 0.0f;
				nextIdx = Idx;
			}
			else
			{
				t = 1.0f;
				nextIdx = 0;
			}
		}
		else
		{
			nextIdx = Idx + 1;

			if (Channels & i3TransformSequence::TIME)
			{
				REAL32 t1, t2;

				t1 = pSeq2->m_pTime[Idx];
				t2 = pSeq2->m_pTime[nextIdx];

				t = i3Math::div((_time - t1), ((t2 - t1)));
			}
			else
			{
				t = i3Math::div(i3Math::fmod(_time, pSeq2->m_KeyInterval), pSeq2->m_KeyInterval);
			}
		}
	}



	// Translation
	if (Channels & i3TransformSequence::TRANSLATE)
	{
		VEC3D * pSrc = nullptr, *pSrc2 = nullptr;

		if (Channels & i3TransformSequence::FIXED_TRANSLATE)
		{
			if (m_pTArray != nullptr)
			{
				I3ASSERT_RETURN(pSeq != nullptr, 0);

				pSrc = m_pTArray + pSeq->m_pTIndex[0];
				i3Vector::Copy(pInfo->m_pOutTrans, pSrc);
			}
			else
			{
				I3ASSERT(m_pTArray16Bits != nullptr);

				INT32 idx1, idx2, idx3;

				if (pSeq != nullptr)
				{
					idx1 = pSeq->m_pTIndex[0] * 3;
					idx2 = pSeq->m_pTIndex[0] * 3 + 1;
					idx3 = pSeq->m_pTIndex[0] * 3 + 2;
				}
				else
				{
					idx1 = pSeq2->m_pTIndex[0] * 3;
					idx2 = pSeq2->m_pTIndex[0] * 3 + 1;
					idx3 = pSeq2->m_pTIndex[0] * 3 + 2;
				}

				// 16Bits
				i3Vector::Set(pInfo->m_pOutTrans, (REAL32)m_pTArray16Bits[idx1],
					(REAL32)m_pTArray16Bits[idx2],
					(REAL32)m_pTArray16Bits[idx3]);
			}
		}
		else
		{
			if (m_pTArray != nullptr)
			{
				I3ASSERT_RETURN(pSeq != nullptr, 0);

				pSrc = m_pTArray + pSeq->m_pTIndex[Idx];
				pSrc2 = m_pTArray + pSeq->m_pTIndex[nextIdx];
			}
			else
			{	// 16Bits
				I3ASSERT(m_pTArray16Bits != nullptr);

				INT32 idx1, idx2, idx3, idx4, idx5, idx6;

				if (pSeq != nullptr)
				{
					idx1 = pSeq->m_pTIndex[Idx] * 3;
					idx2 = pSeq->m_pTIndex[Idx] * 3 + 1;
					idx3 = pSeq->m_pTIndex[Idx] * 3 + 2;

					idx4 = pSeq->m_pTIndex[nextIdx] * 3;
					idx5 = pSeq->m_pTIndex[nextIdx] * 3 + 1;
					idx6 = pSeq->m_pTIndex[nextIdx] * 3 + 2;
				}
				else
				{
					idx1 = pSeq2->m_pTIndex[Idx] * 3;
					idx2 = pSeq2->m_pTIndex[Idx] * 3 + 1;
					idx3 = pSeq2->m_pTIndex[Idx] * 3 + 2;

					idx4 = pSeq2->m_pTIndex[nextIdx] * 3;
					idx5 = pSeq2->m_pTIndex[nextIdx] * 3 + 1;
					idx6 = pSeq2->m_pTIndex[nextIdx] * 3 + 2;
				}

				i3Vector::Set(&vTemp1, (REAL32)m_pTArray16Bits[idx1],
					(REAL32)m_pTArray16Bits[idx2],
					(REAL32)m_pTArray16Bits[idx3]);

				i3Vector::Set(&vTemp2, (REAL32)m_pTArray16Bits[idx4],
					(REAL32)m_pTArray16Bits[idx5],
					(REAL32)m_pTArray16Bits[idx6]);

				pSrc = &vTemp1;
				pSrc2 = &vTemp2;
			}

			switch (TIMode)
			{
			default:							i3Vector::Copy(pInfo->m_pOutTrans, pSrc);	break;
			case i3TransformSequence::LINEAR:	i3Vector::Lerp(pInfo->m_pOutTrans, pSrc, pSrc2, t);	break;
			case i3TransformSequence::SPLINE:
			case i3TransformSequence::BEZIER:
			case i3TransformSequence::SLERP:
				i3Log("i3TransformSequence::GetMatrix()", "SPLINE and BEZIER interpolation is not implemented for translation.");
				break;
			}
		}

		Key |= I3MATH_KEY_TRANSLATE;
	}

	// Rotation
	if (Channels & i3TransformSequence::ROTATE)
	{
		QUATERNION * pQ1 = nullptr, *pQ2 = nullptr;

		if (Channels & i3TransformSequence::FIXED_ROTATE)
		{
			if (m_pRArray != nullptr)
			{
				I3ASSERT_RETURN(pSeq != nullptr, 0);

				pQ1 = m_pRArray + pSeq->m_pRIndex[0];

				i3Quat::Copy(pInfo->m_pOutRot, pQ1);
			}
			else
			{
				I3ASSERT(m_pRArray16Bits != nullptr);

				INT32 idx1, idx2, idx3, idx4;

				if (pSeq != nullptr)
				{
					idx1 = pSeq->m_pRIndex[0] * 4;
					idx2 = pSeq->m_pRIndex[0] * 4 + 1;
					idx3 = pSeq->m_pRIndex[0] * 4 + 2;
					idx4 = pSeq->m_pRIndex[0] * 4 + 3;
				}
				else
				{
					idx1 = pSeq2->m_pRIndex[0] * 4;
					idx2 = pSeq2->m_pRIndex[0] * 4 + 1;
					idx3 = pSeq2->m_pRIndex[0] * 4 + 2;
					idx4 = pSeq2->m_pRIndex[0] * 4 + 3;
				}

				i3Quat::Set(pInfo->m_pOutRot, (REAL32)m_pRArray16Bits[idx1],
					(REAL32)m_pRArray16Bits[idx2],
					(REAL32)m_pRArray16Bits[idx3],
					(REAL32)m_pRArray16Bits[idx4]);
			}
		}
		else
		{
			if (m_pRArray != nullptr)
			{
				I3ASSERT_RETURN(pSeq != nullptr, 0);

				pQ1 = m_pRArray + pSeq->m_pRIndex[Idx];
				pQ2 = m_pRArray + pSeq->m_pRIndex[nextIdx];
			}
			else
			{
				I3ASSERT(m_pRArray16Bits != nullptr);

				INT32 idx11, idx12, idx13, idx14;
				INT32 idx21, idx22, idx23, idx24;

				if (pSeq != nullptr)
				{
					idx11 = pSeq->m_pRIndex[Idx] * 4;
					idx12 = pSeq->m_pRIndex[Idx] * 4 + 1;
					idx13 = pSeq->m_pRIndex[Idx] * 4 + 2;
					idx14 = pSeq->m_pRIndex[Idx] * 4 + 3;

					idx21 = pSeq->m_pRIndex[nextIdx] * 4;
					idx22 = pSeq->m_pRIndex[nextIdx] * 4 + 1;
					idx23 = pSeq->m_pRIndex[nextIdx] * 4 + 2;
					idx24 = pSeq->m_pRIndex[nextIdx] * 4 + 3;
				}
				else
				{
					idx11 = pSeq2->m_pRIndex[Idx] * 4;
					idx12 = pSeq2->m_pRIndex[Idx] * 4 + 1;
					idx13 = pSeq2->m_pRIndex[Idx] * 4 + 2;
					idx14 = pSeq2->m_pRIndex[Idx] * 4 + 3;

					idx21 = pSeq2->m_pRIndex[nextIdx] * 4;
					idx22 = pSeq2->m_pRIndex[nextIdx] * 4 + 1;
					idx23 = pSeq2->m_pRIndex[nextIdx] * 4 + 2;
					idx24 = pSeq2->m_pRIndex[nextIdx] * 4 + 3;
				}

				i3Quat::Set(&qTemp1, (REAL32)m_pRArray16Bits[idx11],
					(REAL32)m_pRArray16Bits[idx12],
					(REAL32)m_pRArray16Bits[idx13],
					(REAL32)m_pRArray16Bits[idx14]);

				i3Quat::Set(&qTemp2, (REAL32)m_pRArray16Bits[idx21],
					(REAL32)m_pRArray16Bits[idx22],
					(REAL32)m_pRArray16Bits[idx23],
					(REAL32)m_pRArray16Bits[idx24]);

				pQ1 = &qTemp1;
				pQ2 = &qTemp2;
			}

			switch (RIMode)
			{
			default:
				i3Quat::Copy(pInfo->m_pOutRot, pQ1);
				break;

			case i3TransformSequence::LINEAR:	//_lerpRot( pInfo, pQ, &m_pRotation[Idx], &m_pRotation[nextIdx], t);	break;
			case i3TransformSequence::SLERP:
				i3Quat::Slerp(pInfo->m_pOutRot, pQ1, pQ2, t);
				break;
			}
		}

		Key |= I3MATH_KEY_ROTATE;
	}

	// Scale
	if (Channels & i3TransformSequence::SCALE)
	{
		VEC3D * pS1 = nullptr, *pS2 = nullptr;

		if (Channels & i3TransformSequence::FIXED_SCALE)
		{
			if (m_pSArray != nullptr)
			{
				I3ASSERT_RETURN(pSeq != nullptr, 0);

				pS1 = m_pSArray + pSeq->m_pSIndex[0];

				i3Vector::Copy(pInfo->m_pOutScale, pS1);
			}
			else
			{
				I3ASSERT(m_pSArray16Bits != nullptr);

				INT32 idx1, idx2, idx3;

				if (pSeq != nullptr)
				{
					idx1 = pSeq->m_pSIndex[0] * 3;
					idx2 = pSeq->m_pSIndex[0] * 3 + 1;
					idx3 = pSeq->m_pSIndex[0] * 3 + 2;
				}
				else
				{
					idx1 = pSeq2->m_pSIndex[0] * 3;
					idx2 = pSeq2->m_pSIndex[0] * 3 + 1;
					idx3 = pSeq2->m_pSIndex[0] * 3 + 2;
				}

				i3Vector::Set(pInfo->m_pOutScale, (REAL32)m_pSArray16Bits[idx1],
					(REAL32)m_pSArray16Bits[idx2],
					(REAL32)m_pSArray16Bits[idx3]);
			}
		}
		else
		{
			if (m_pSArray != nullptr)
			{
				I3ASSERT_RETURN(pSeq != nullptr, 0);

				pS1 = m_pSArray + pSeq->m_pSIndex[Idx];
				pS2 = m_pSArray + pSeq->m_pSIndex[nextIdx];
			}
			else
			{
				I3ASSERT(m_pSArray16Bits != nullptr);

				INT32 idx1, idx2, idx3, idx4, idx5, idx6;

				if (pSeq != nullptr)
				{
					idx1 = pSeq->m_pSIndex[Idx] * 3;
					idx2 = pSeq->m_pSIndex[Idx] * 3 + 1;
					idx3 = pSeq->m_pSIndex[Idx] * 3 + 2;

					idx4 = pSeq->m_pSIndex[nextIdx] * 3;
					idx5 = pSeq->m_pSIndex[nextIdx] * 3 + 1;
					idx6 = pSeq->m_pSIndex[nextIdx] * 3 + 2;
				}
				else
				{
					idx1 = pSeq2->m_pSIndex[Idx] * 3;
					idx2 = pSeq2->m_pSIndex[Idx] * 3 + 1;
					idx3 = pSeq2->m_pSIndex[Idx] * 3 + 2;

					idx4 = pSeq2->m_pSIndex[nextIdx] * 3;
					idx5 = pSeq2->m_pSIndex[nextIdx] * 3 + 1;
					idx6 = pSeq2->m_pSIndex[nextIdx] * 3 + 2;
				}

				i3Vector::Set(&vTemp1, (REAL32)m_pSArray16Bits[idx1],
					(REAL32)m_pSArray16Bits[idx2],
					(REAL32)m_pSArray16Bits[idx3]);

				i3Vector::Set(&vTemp2, (REAL32)m_pSArray16Bits[idx4],
					(REAL32)m_pSArray16Bits[idx5],
					(REAL32)m_pSArray16Bits[idx6]);

				pS1 = &vTemp1;
				pS2 = &vTemp2;
			}

			switch (SIMode)
			{
			default:							i3Vector::Copy(pInfo->m_pOutScale, pS1);	break;
			case i3TransformSequence::LINEAR:	i3Vector::Lerp(pInfo->m_pOutScale, pS1, pS2, t);	break;
			}
		}

		Key |= I3MATH_KEY_SCALE;
	}

	pInfo->m_KeyChannel = Key;

	return Key;
}

void i3Animation2::GetKeyframe(INT32 idxTrack, INT32 idxKey, MATRIX * pMtx)
{
	VEC3D vTemp;
	QUATERNION qTemp;

	i3::pack::animation::TRACK * pSeq = nullptr;
	i3::pack::animation::TRACK2 * pSeq2 = nullptr;

	UINT32 Channels = 0;

	if (m_pSeq != nullptr)
	{
		pSeq = m_pSeq + idxTrack;
		Channels = pSeq->m_Channels;
	}
	else
	{
		I3ASSERT(m_pSeq2 != nullptr);

		pSeq2 = m_pSeq2 + idxTrack;
		Channels = pSeq2->m_Channels;
	}

	// Rotation
	if (Channels & i3TransformSequence::ROTATE)
	{
		QUATERNION * pQ = nullptr;

		if (m_pRArray != nullptr)
		{
			I3ASSERT_RETURN(pSeq != nullptr);

			pQ = &m_pRArray[pSeq->m_pRIndex[idxKey]];
		}
		else
		{
			I3ASSERT(m_pRArray16Bits != nullptr);

			INT32 idx1, idx2, idx3, idx4;

			if (pSeq != nullptr)
			{
				idx1 = pSeq->m_pRIndex[idxKey] * 4;
				idx2 = pSeq->m_pRIndex[idxKey] * 4 + 1;
				idx3 = pSeq->m_pRIndex[idxKey] * 4 + 2;
				idx4 = pSeq->m_pRIndex[idxKey] * 4 + 3;
			}
			else
			{
				idx1 = pSeq2->m_pRIndex[idxKey] * 4;
				idx2 = pSeq2->m_pRIndex[idxKey] * 4 + 1;
				idx3 = pSeq2->m_pRIndex[idxKey] * 4 + 2;
				idx4 = pSeq2->m_pRIndex[idxKey] * 4 + 3;
			}

			i3Quat::Set(&qTemp, (REAL32)m_pRArray16Bits[idx1],
				(REAL32)m_pRArray16Bits[idx2],
				(REAL32)m_pRArray16Bits[idx3],
				(REAL32)m_pRArray16Bits[idx4]);

			pQ = &qTemp;
		}

		i3Matrix::SetRotateQuat(pMtx, pQ);
	}
	else
	{
		i3Matrix::Identity(pMtx);
	}

	// Translation
	if (Channels & i3TransformSequence::TRANSLATE)
	{
		VEC3D * pPos = nullptr;

		if (m_pTArray != nullptr)
		{
			I3ASSERT_RETURN(pSeq != nullptr);

			pPos = &m_pTArray[pSeq->m_pTIndex[idxKey]];
		}
		else
		{
			I3ASSERT(m_pTArray16Bits != nullptr);

			INT32 idx1, idx2, idx3;

			if (pSeq != nullptr)
			{
				idx1 = pSeq->m_pTIndex[idxKey] * 3;
				idx2 = pSeq->m_pTIndex[idxKey] * 3 + 1;
				idx3 = pSeq->m_pTIndex[idxKey] * 3 + 2;
			}
			else
			{
				idx1 = pSeq2->m_pTIndex[idxKey] * 3;
				idx2 = pSeq2->m_pTIndex[idxKey] * 3 + 1;
				idx3 = pSeq2->m_pTIndex[idxKey] * 3 + 2;
			}

			i3Vector::Set(&vTemp, (REAL32)m_pTArray16Bits[idx1],
				(REAL32)m_pTArray16Bits[idx2],
				(REAL32)m_pTArray16Bits[idx3]);

			pPos = &vTemp;
		}

		i3Matrix::SetPos(pMtx, pPos);
	}

	// Scale
	if (Channels & i3TransformSequence::SCALE)
	{
		VEC3D * pVec = nullptr;

		if (m_pSArray != nullptr)
		{
			I3ASSERT_RETURN(pSeq != nullptr);

			pVec = &m_pSArray[pSeq->m_pSIndex[idxKey]];
		}
		else
		{
			I3ASSERT(m_pSArray16Bits != nullptr);

			INT32 idx1, idx2, idx3;

			if (pSeq != nullptr)
			{
				idx1 = pSeq->m_pSIndex[idxKey] * 3;
				idx2 = pSeq->m_pSIndex[idxKey] * 3 + 1;
				idx3 = pSeq->m_pSIndex[idxKey] * 3 + 2;
			}
			else
			{
				idx1 = pSeq2->m_pSIndex[idxKey] * 3;
				idx2 = pSeq2->m_pSIndex[idxKey] * 3 + 1;
				idx3 = pSeq2->m_pSIndex[idxKey] * 3 + 2;
			}

			i3Vector::Set(&vTemp, (REAL32)m_pSArray16Bits[idx1],
				(REAL32)m_pSArray16Bits[idx2],
				(REAL32)m_pSArray16Bits[idx3]);

			pVec = &vTemp;
		}

		i3Matrix::PreScale(pMtx, i3Vector::GetX(pVec), i3Vector::GetY(pVec), i3Vector::GetZ(pVec));
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ANIMATION2
	{
		INT32		m_TrackCount = 0;
		UINT32		m_Flag = 0;
		INT8		reserved[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct ALIGN4 TRACKINFO2
	{
		OBJREF		m_pSeq = 0;
		char		m_szBone[MAX_NAME] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Animation2::StoreData(i3Stream * pFile)
{
	UINT32 rc, result = 0;
	INT32 i;

	if (GetTrackCount() <= 0)
		return 0;

	// Track Information
	rc = pFile->Write(m_pTracks, sizeof(i3::pack::animation::TRACK_INFO) * GetTrackCount());
	I3_CHKIO(rc);
	result += rc;

	// Track Sequence
	if (m_pSeq != nullptr)
	{
		rc = pFile->Write(m_pSeq, sizeof(i3::pack::animation::TRACK) * GetTrackCount());
		I3_CHKIO(rc);
		result += rc;

		for (i = 0; i < GetTrackCount(); i++)
		{
			rc = m_pSeq[i].StoreData(pFile);
			I3_CHKIO(rc);
			result += rc;
		}
	}
	else if (m_pSeq2 != nullptr)
	{
		I3ASSERT(m_pSeq2 != nullptr);

		rc = pFile->Write(m_pSeq2, sizeof(i3::pack::animation::TRACK2) * GetTrackCount());
		I3_CHKIO(rc);
		result += rc;

		for (i = 0; i < GetTrackCount(); i++)
		{
			rc = m_pSeq2[i].StoreData(pFile);
			I3_CHKIO(rc);
			result += rc;
		}
	}

	return result;
}

UINT32	i3Animation2::LoadData(i3MemoryBuffer * pBuff, INT32 offset, INT32 TrackCount, REAL32 duration)
{
	INT32 i;
	char * pStart, *pBase;

	if (TrackCount <= 0)
		return 0;

	I3_REF_CHANGE(m_pBindBuf, pBuff);
	m_offMemory = offset;

	m_TrackCount = TrackCount;

	pBase = pStart = (char *)pBuff->getBuffer();

	m_pTracks = (i3::pack::animation::TRACK_INFO *)(pStart + offset);
	m_pSeq = (i3::pack::animation::TRACK *)(m_pTracks + TrackCount);

	pStart = (char *)(m_pSeq + TrackCount);

	m_Duration = 0.0f;

	for (i = 0; i < GetTrackCount(); i++)
	{
		UINT32 sz;

		// Pointer 오류를 막기 위해 Clear
		m_pTracks[i].m_pSeq = nullptr;

		sz = m_pSeq[i].LoadData(pStart);

		m_Duration = MAX(m_Duration, m_pSeq[i].m_Duration);

		pStart += sz;
	}

	return pStart - pBase;
}

UINT32 i3Animation2::OnSave(i3ResourceFile * pResFile)
{
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::ANIMATION2 data;

	// Named Element
	Result = i3NamedElement::OnSave(pResFile);
	if (Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Track Count
	data.m_TrackCount = m_TrackCount;

	if (m_pSeq2 != nullptr)
	{
		data.m_Flag |= I3_ANIMATION2_FLAG_REAL16;
	}

	Rc = pStream->Write(&data, sizeof(data));
	if (Rc == STREAM_ERR)
	{
		i3Log("i3Animation2::OnSave()", "Could not write track count data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Track Data
	//
	// i3TransformSequence에 대한 포인터를 그대로 저장한다.
	// 이는 Loading 시에 적당한 Runtime Pointer로 Rebind되기 때문에 문제가 되지 않는다.
	if (m_TrackCount > 0)
	{
		for (INT32 i = 0; i < m_TrackCount; i++)
		{
			pack::TRACKINFO2 data2;

			data2.m_pSeq = pResFile->GetObjectPersistID(m_pTracks[i].m_pSeq);
			i3::safe_string_copy(data2.m_szBone, m_pTracks[i].m_szBone, MAX_NAME);

			Rc = pStream->Write(&data2, sizeof(data2));
			I3_CHKIO(Rc);
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3Animation2::OnLoad(i3ResourceFile * pResFile)
{
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();
	INT32 i;
	pack::ANIMATION2 data;

	// Named Element
	Result = i3NamedElement::OnLoad(pResFile);
	if (Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read(&data, sizeof(data));
	if (Rc == STREAM_ERR)
	{
		i3Log("i3Animation2::OnLoad()", "Could not read track count data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if (data.m_Flag & I3_ANIMATION2_FLAG_REAL16)
		Create(data.m_TrackCount, true);
	else
		Create(data.m_TrackCount);

	// Track References
	for (i = 0; i < m_TrackCount; i++)
	{
		pack::TRACKINFO2 data2;

		Rc = pStream->Read(&data2, sizeof(data2));
		I3_CHKIO(Rc);
		Result += Rc;

		if (data2.m_pSeq != 0)
		{
			i3TransformSequence * pSeq;

			pSeq = (i3TransformSequence *)pResFile->FindObjectByID(data2.m_pSeq);
			I3ASSERT(pSeq != nullptr);

			SetTrack(i, pSeq, data2.m_szBone);
		}
	}

	return Result;
}

UINT32	i3Animation2::LoadData16Bits(i3MemoryBuffer * pBuff, INT32 offset, INT32 TrackCount, REAL32 duration, INT32 nTrackType /*= 0*/)
{
	INT32 i;
	char * pStart, *pBase;

	if (TrackCount <= 0)
		return 0;

	I3_REF_CHANGE(m_pBindBuf, pBuff);
	m_offMemory = offset;

	m_TrackCount = TrackCount;

	pBase = pStart = (char *)pBuff->getBuffer();

	m_pTracks = (i3::pack::animation::TRACK_INFO *)(pStart + offset);

	if (nTrackType == 1)
	{
		m_pSeq = (i3::pack::animation::TRACK *)(m_pTracks + TrackCount);
		pStart = (char *)(m_pSeq + TrackCount);
	}
	else
	{
		m_pSeq2 = (i3::pack::animation::TRACK2 *)(m_pTracks + TrackCount);
		pStart = (char *)(m_pSeq2 + TrackCount);
	}

	m_Duration = 0.0f;

	for (i = 0; i < GetTrackCount(); i++)
	{
		UINT32 sz;

		// Pointer 오류를 막기 위해 Clear
		m_pTracks[i].m_pSeq = nullptr;

		if (m_pSeq != nullptr)
		{
			sz = m_pSeq[i].LoadData(pStart);
			m_Duration = MAX(m_Duration, m_pSeq[i].m_Duration);
		}
		else
		{
			sz = m_pSeq2[i].LoadData(pStart);
			m_Duration = MAX(m_Duration, m_pSeq2[i].m_Duration);
		}

		pStart += sz;
	}

	//I3TRACE( "Load Animation2 : %s (for 16Bits)\n", this->GetName());

	return pStart - pBase;
}


