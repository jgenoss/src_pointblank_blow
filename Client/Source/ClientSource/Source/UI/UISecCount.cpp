#include "pch.h"
#include "UISecCount.h"


UISecCountCallback_Null  UISecCount::ms_pDefaultCB;

UISecCount::UISecCount(INT32 sec) : m_base_time(sec), m_reverse(false),
	m_init_timer_value(0), m_result_time(sec), m_pCallback(&ms_pDefaultCB)
{
	
}

namespace
{
	static void _CommitResultTimeChanges(INT32 next_result_time, INT32& outResultTime, UISecCountCallback* pCallback)
	{
		if (next_result_time != outResultTime)
		{
			outResultTime = next_result_time;
			pCallback->OnSecChange(outResultTime);
		}
	}
}



void UISecCount::SetBaseSec(INT32 sec)
{
	m_base_time = sec;
	INT32 next_result_time = (m_reverse) ? 0 : sec;		// 일단 done상태로 둔다..

	if (m_init_timer_value != 0 )
	{
		m_init_timer_value = 0;
		_CommitResultTimeChanges(next_result_time, m_result_time, m_pCallback);
	}
	else
	{
		m_result_time = next_result_time;
	}
}

void UISecCount::SetReverse(bool reverse_count)
{
	if (reverse_count != m_reverse)
	{
		m_reverse = reverse_count;
		if (m_init_timer_value != 0)
			_CommitResultTimeChanges( m_base_time - m_result_time, m_result_time, m_pCallback);	
	}
}

/// 시작시에는 다음번 업데이트때 다음 것이 반영될것이므로 첫값을 위해 무조건 콜백을 반영한다...

void UISecCount::Start()
{
	if ( m_init_timer_value != 0)			// 일단 이미 시작한 경우 넘겨버립니다..
		return;
	
	UISecCount::Reset();					// 리셋후 재개한다...
	m_init_timer_value = timeGetTime();
	m_pCallback->OnStart( m_result_time );
}

void UISecCount::Stop()
{
	if (m_init_timer_value != 0)
	{
		m_init_timer_value = 0;
		m_pCallback->OnStop(m_result_time);
	}		
}

void UISecCount::Reset()
{
	if (m_init_timer_value != 0)
	{
		m_init_timer_value = 0;
		_CommitResultTimeChanges((m_reverse) ? m_base_time : 0, m_result_time, m_pCallback);
	}
	else
	{
		m_result_time = (m_reverse) ? m_base_time : 0;
	}
}

INT32 UISecCount::Update()				// rDeltaSeconds를 쓰지 않음..
{
	if (m_init_timer_value == 0)		// 더이상 계산하지 않고 이제까지 결과값 리턴한다.
		return m_result_time;

	INT32 time_interval_in_sec = static_cast<INT32>((timeGetTime() - m_init_timer_value)/1000);
	
	if (time_interval_in_sec >= m_base_time)	// 타이머종료...
	{
		time_interval_in_sec = m_base_time;	
		m_init_timer_value = 0;

		_CommitResultTimeChanges(  (m_reverse) ? 0 : m_base_time , m_result_time, m_pCallback);

		m_pCallback->OnDone(m_result_time);

		return m_result_time;
	}

	INT32 next_result_time = (m_reverse) ? m_base_time - time_interval_in_sec : time_interval_in_sec;
	
	_CommitResultTimeChanges( next_result_time, m_result_time, m_pCallback);

	return m_result_time;
}
