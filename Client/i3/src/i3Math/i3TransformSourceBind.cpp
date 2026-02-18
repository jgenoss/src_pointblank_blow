#include "i3MathType.h"
#include "i3Matrix.h"
#include "i3Quat.h"
#include "i3TransformSourceBind.h"

#if defined( I3_FIXED_POINT)
#include "i3FixedPointMatrix.h"
#endif

#ifdef I3_DEBUG
bool i3TransformSourceBind::m_bCheckRuntimeRelease = false;
void i3TransformSourceBind::CheckRuntimeRelease( bool bTrue )
{
	m_bCheckRuntimeRelease = bTrue;
}
#endif

i3TransformSourceBind::~i3TransformSourceBind(void)
{
	Reset();
}

void i3TransformSourceBind::setAnimation( i3Animation * pAnim, INT32 idxBone)
{
	I3_REF_CHANGE( m_pAnim, pAnim);
#if defined( I3_DEBUG)
	if( m_pAnim != nullptr)
	{
		I3ASSERT( m_pAnim->GetRefCount() > 0);
	}
#endif

	m_idxBone = idxBone;
	m_PlayCount = 0;
	m_bFirst = true;
}

void i3TransformSourceBind::Init( REAL32 tm)
{
	m_pAnim = nullptr;

	m_PlayMode  = I3_ANIM_PLAY_MODE_REPEAT;
	m_BlendMode = I3_ANIM_BLEND_MODE_REPLACE;

	m_bFirst = true;
	m_timeLocal			= tm;
	m_PlayCount			= 0;
	m_ManualClampTime	= 1.0f;
}

void i3TransformSourceBind::Reset(void)
{
	if( m_pAnim != nullptr)
	{
	#ifdef I3_DEBUG
		if( m_bCheckRuntimeRelease && m_pAnim->GetRefCount() <= 1)
		{
			// 애니메이션 객체 자체가 실제로 릴리즈가 될려고 함.
			// 리소스 해제 과정에서는 해제 순서에 따라서 여기로 들어오는 경우가 발생할 수 있음.
			// 하지만 게임 플레이 도중에 여기로 들어 온다면 뭔가 문제가 있는 상황임.
			I3TRACE( "WARNING : Animation의 정지 처리 중 Animation의 할당 해제가 발생!\n");
		}
	#endif

		I3_SAFE_RELEASE( m_pAnim);
	}

	Init();
}

void i3TransformSourceBind::SetPlayMode( I3_ANIM_PLAY_MODE mode)
{
	m_PlayMode = mode;
}

void i3TransformSourceBind::SetManualClampTime(REAL32 time)
{
	m_ManualClampTime = time;
}

void i3TransformSourceBind::GetInterpolatedKeyframe( I3MATRIXINFO * pInfo)
{
	REAL32 duration;
	REAL32 _oldTime = pInfo->m_Time;
	MATRIX * pOut = pInfo->m_pOut;

	I3ASSERT( m_pAnim != nullptr);
	I3ASSERT( m_idxBone != -1);

	pInfo->m_PlayMode = m_PlayMode;
	pInfo->m_pOut = &m_CacheMatrix;

	m_bFirst = false;

	switch( m_PlayMode)
	{
		case I3_ANIM_PLAY_MODE_STOP :
			{
				pInfo->m_Time = m_timeLocal;

				m_pAnim->GetInterpolatedKeyframe( m_idxBone, pInfo, false);
			}
			break;

		case I3_ANIM_PLAY_MODE_REPEAT :
			duration = m_pAnim->GetDuration();

			m_timeLocal += pInfo->m_Time;

			if( i3Math::abs( m_timeLocal) > duration)
			{
				m_PlayCount ++;

				if( duration > 0.0f )
					m_timeLocal = i3Math::fmod( m_timeLocal, duration );
			}

			// Animation은 이동 시간(dt) 값이 아닌,
			// Duration 범위의 시간 길이를 필요로 하기 때문에
			// 임시로 time 값을 변경한다.
			pInfo->m_Time = m_timeLocal;

			m_pAnim->GetInterpolatedKeyframe( m_idxBone, pInfo, false);
			break;

		case I3_ANIM_PLAY_MODE_CLAMP :
			duration = m_pAnim->GetDuration();

			m_timeLocal += pInfo->m_Time;

			if( m_PlayCount == 0 )
			{
				if( i3Math::abs( m_timeLocal) >= duration)
				{
					m_PlayCount = 1;
					m_timeLocal = duration;
				}
				pInfo->m_Time = m_timeLocal;
			}
			else
			{
				pInfo->m_Time = duration;
			}

			m_pAnim->GetInterpolatedKeyframe( m_idxBone, pInfo, false);
			break;

		case I3_ANIM_PLAY_MODE_MANUAL_CLAMP :
		{
			m_timeLocal += pInfo->m_Time;

			if (m_PlayCount == 0)
			{
				if (i3Math::abs(m_timeLocal) >= m_ManualClampTime)
				{
					m_PlayCount = 1;
					m_timeLocal = m_ManualClampTime;
				}
				pInfo->m_Time = m_timeLocal;
			}
			else
			{
				pInfo->m_Time = m_ManualClampTime;
			}

			m_pAnim->GetInterpolatedKeyframe(m_idxBone, pInfo, FALSE);
		}
			break;
	}

	pInfo->m_pOut = pOut;
	pInfo->m_Time = _oldTime;
}
