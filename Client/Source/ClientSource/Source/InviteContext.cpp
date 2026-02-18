#include "pch.h"
#include "InviteContext.h"

void InviteContext::CheckInviteUserInfo(UINT32 idx)
{
//	idx = MINMAX(0, idx, MAX_INVITE_COUNT);
//	I3_BOUNDCHK(idx, MAX_INVITE_COUNT);				//  위에꺼 하고 아래거는 왜 하는건데... MINMAX는 걍..별로다..

	if (idx < MAX_INVITE_COUNT)		// 설마 이코드에 문제가 있다고 생각친 않겠지??  이걸 함수로 빼는게 나쁜건 아니지만?
		m_bInviteUserIdx[idx] = true;
	else
		I3ASSERT(idx < MAX_INVITE_COUNT);
}

void InviteContext::ClearInviteCheck(void)
{
	INT32 i;
	for( i = 0; i < MAX_INVITE_COUNT; i++)
	{
		m_bInviteUserIdx[i] = false;
	}
}

UINT32 InviteContext::GetCheckInviteCount(void) const
{
	INT32 i;
	UINT32 nCount = 0;

	for( i = 0; i < MAX_INVITE_COUNT; i++)
	{
		if (m_bInviteUserIdx[i])
		{
			nCount++;
		}
	}

	return nCount;
}

bool	InviteContext::PopInviteInfo(INVITE_INFO* out)
{
	if (m_stackInviteInfo.empty())
		return false;

	*out = m_stackInviteInfo.back();
	m_stackInviteInfo.pop_back();
	return true;
}

void	InviteContext::PushInviteInfo(const INVITE_INFO& in)
{
	m_stackInviteInfo.push_back(in);
}

// 특정 유저의 닉네임으로 세션 인덱스 가져오기 
INT32 	InviteContext::GetUserSessionIdxByUserNickName(const i3::wliteral_range& wNickName ) const
{
	INT32 i32SessionIndex = -1;

	for ( UINT32 i = 0 ; i < m_nUserInfoInviteCount ; i++ )
	{
		if( i3::generic_is_iequal( wNickName, GetUserInfoInvite(i)->_wstr_nick ) ) 
		{	
			i32SessionIndex = GetUserInfoInvite(i)->_idxSession;
		}
	}

	return i32SessionIndex;
}