#include "pch.h"
#include "AwayChecker.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE(CAwayChecker);//, i3GameNode);

CAwayChecker::CAwayChecker()
{
	m_pObserver = nullptr;
	m_rAwayTime = 0.0f;
	m_rCautionTime = 0.0f;
	m_rCurrentTime = 0.0f;
	m_bOneTimeNotify = true;
	m_bPause = true;
	m_bAwayNotify = false;
	m_bCautionNotify = false;
}

CAwayChecker::~CAwayChecker()
{
	// Empty
}

void CAwayChecker::OnUpdate(REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate(rDeltaSeconds);

	// 멈춰 있으면 처리하지 않는다
	if (m_bPause) return;

	// 통보할 class가 없다면 처리하지 않는다
	if (nullptr == m_pObserver) return;

	if( !UserInfoContext::i()->CheckAuthLevel() )	// revision 44121
	{
		// 0.0f 까지 감소
		m_rCurrentTime = MAX(0.0f, m_rCurrentTime - rDeltaSeconds);
	}

	// Away 검사
	if (m_rCurrentTime <= 0.0f)
	{
		if (m_bOneTimeNotify && m_bAwayNotify)
		{
			m_bPause = true;
		}
		else
		{
			m_pObserver->OnEvent(ACE_AWAY, this, 0, I3_EVT_CODE_ACTIVATE);
			m_bAwayNotify = true;
		}

		return;
	}

	
	// Caution 검사	
	if (m_rCurrentTime <= m_rCautionTime)
	{
		if (false == m_bOneTimeNotify || false == m_bCautionNotify)
		{
			// 남은 시간도 전달한다
			m_pObserver->OnEvent(ACE_CAUTION, this, (UINT32)m_rCurrentTime, I3_EVT_CODE_ACTIVATE);
			m_bCautionNotify = true;
		}
	}
}

void CAwayChecker::SetReceiver(i3EventReceiver* pObserver)
{
	m_pObserver = pObserver;
}

void CAwayChecker::SetTime(REAL32 rAwayTime,REAL32 rCautionTime,bool bOneTimeNotify)
{
	// 초기화
	m_bOneTimeNotify = bOneTimeNotify;
	m_bPause = true;
	m_bAwayNotify = false;
	m_bCautionNotify = false;	

	// 새 값 설정
	m_rAwayTime = MAX(0.0f, rAwayTime);
	m_rCautionTime = MAX(0.0f, rCautionTime);

	m_rCurrentTime = m_rAwayTime;

	m_bPause = false;
}

void CAwayChecker::SetNotify(bool bOneTimeNotify)
{
	m_bOneTimeNotify = bOneTimeNotify;
}

void CAwayChecker::Restart(void)
{
	m_rCurrentTime = m_rAwayTime;
	
	m_bAwayNotify = false;
	m_bCautionNotify = false;

	m_bPause = false;
}

void CAwayChecker::Reset(void)
{
	m_rAwayTime = 0.0f;
	m_rCautionTime = 0.0f;
	m_rCurrentTime = 0.0f;

	m_bOneTimeNotify = true;
	m_bPause = true;
	m_bAwayNotify = false;
	m_bCautionNotify = false;
}
