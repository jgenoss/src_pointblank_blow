#include "pch.h"
#include "VoteRecorder.h"

CVoteRecorder::CVoteRecorder()
{
	m_i32BeginPtr	= 0;
	m_i32EndPtr		= 0;
	m_i32Count		= 0;
}

// 일정시간 이내에 동일한 신고인과 동일한 피신고인의 완료된 투표가 있었는지 확인합니다.
// 있다면 FALSE를 리턴하여서 투표를 할수 없도록 처리합니다.
BOOL CVoteRecorder::Check( TUID i64CallUID, TUID i64TargetUID )
{
	UINT32 ui32CurrentTime	= g_pContextMain->GetServerTime();

	for( INT32 i = 0; i < m_i32Count; i++ )
	{
		if ( i64CallUID == m_aRecord[i].m_i64CallUID && i64TargetUID == m_aRecord[i].m_i64TargetUID )
		{
			if ( ui32CurrentTime - m_aRecord[i].m_ui32RTComplete < VOTE_RE_SUGGEST_TIME )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

// 투표가 끝나면 투표기록(신고자, 피신고자, 신고시간)을 남깁니다.
// 링버퍼가 가득차 있더라도 제일 마지막에 기록한 것의 위에 덮어씌웁니다.
void CVoteRecorder::Record( TUID i64CallUID, TUID i64TargetUID )
{
	m_aRecord[m_i32EndPtr].m_i64CallUID		= i64CallUID;
	m_aRecord[m_i32EndPtr].m_i64TargetUID	= i64TargetUID;
	m_aRecord[m_i32EndPtr].m_ui32RTComplete	= g_pContextMain->GetServerTime();
	m_i32EndPtr++;
	if ( m_i32EndPtr == MAX_SIZE_VOTE_RECORD )
	{
		m_i32EndPtr = 0;
	}
	// 링버퍼가 가득차 있지 않다면 갯수 증가
	if ( m_i32Count < MAX_SIZE_VOTE_RECORD )
	{
		m_i32Count++;
	}
	// 링버퍼가 가득차 있다면 시작포인터도 +1 증가(+1증가한 값이 끝포인터와 동일하므로 +1증가연산대신 끝포인터를 대입하도록 하였습니다.)
	else
	{
		m_i32BeginPtr = m_i32EndPtr;
	}
}

// 링버퍼 내용을 검사해서 
void CVoteRecorder::Clean()
{
	UINT32 ui32Time	= g_pContextMain->GetServerTime();

	INT32 i32Ptr	= m_i32BeginPtr;
	INT32 i32Count	= m_i32Count;
	for( INT32 i = 0; i < i32Count; i++ )
	{
		// 완료된 시간 + 재투표가능한 시간을 해도 현재의 시간에 못 미친다면
		if ( m_aRecord[i32Ptr].m_ui32RTComplete + VOTE_RE_SUGGEST_TIME < ui32Time )
		{
			// 완료되고 나서 1분 이상 지난 것이니 링버퍼 내에서 삭제 처리
			m_i32BeginPtr++;
			if ( m_i32BeginPtr == MAX_SIZE_VOTE_RECORD )
			{
				m_i32BeginPtr = 0;
			}
			m_i32Count--;
		}

		i32Ptr++;
		if ( i32Ptr == MAX_SIZE_VOTE_RECORD )
		{
			i32Ptr = 0;
		}
	}
}