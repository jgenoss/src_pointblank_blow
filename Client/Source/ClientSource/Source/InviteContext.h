#pragma once
//////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------//
//									Invite System									//
//----------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////

#include "i3Base/itl/stack.h"

class InviteContext : public i3::shared_ginst<InviteContext>
{
public:
	void		SetUserInfoInvite(const UINT32 Idx , const USER_INFO_INVITE& uInfo)
	{
		I3ASSERT_RETURN( Idx < MAX_INVITE_COUNT);		
		m_UserInfoInvite[Idx] = uInfo; 
	}
	const USER_INFO_INVITE *	GetUserInfoInvite(UINT32 Idx) const
	{
		I3ASSERT_RETURN( Idx < MAX_INVITE_COUNT, nullptr);
		return &m_UserInfoInvite[Idx]; 
	}

	bool	*	SetInviteUserIdx(UINT32 Idx)
	{
		I3_BOUNDCHK( Idx, MAX_INVITE_COUNT);		
		return &m_bInviteUserIdx[Idx]; 
	}
	const bool* GetInviteUserIdx(UINT32 Idx) const
	{
		I3ASSERT_RETURN( Idx < MAX_INVITE_COUNT, nullptr);
		return &m_bInviteUserIdx[Idx]; 
	}

	void					SetUserInfoInviteCount(UINT32 nUserInfoInviteCount)		{ m_nUserInfoInviteCount = nUserInfoInviteCount; }
	UINT32					GetUserInfoInviteCount(void) const						{ return m_nUserInfoInviteCount; }

	INT32					GetUserSessionIdxByUserNickName(const i3::wliteral_range& wNickName) const;

	void					CheckInviteUserInfo(UINT32 idx);
	void					ClearInviteCheck(void);
	UINT32					GetCheckInviteCount(void) const;

	void					ClearInviteInfo(void)									{ m_stackInviteInfo.clear();		}
	bool					PopInviteInfo(INVITE_INFO* out);
	void					PushInviteInfo(const INVITE_INFO& in); 					
	INT32					GetNumInviteInfo() const								{ return (INT32)m_stackInviteInfo.size(); }

private:
	USER_INFO_INVITE	m_UserInfoInvite[MAX_INVITE_COUNT];	// 초대하기 목록
	bool				m_bInviteUserIdx[MAX_INVITE_COUNT] = { false };	// 초대 여부
	UINT32				m_nUserInfoInviteCount = 0;				// 초대하기 목록 명수	

	//Lobby에서 초대 
	i3::vector<INVITE_INFO>	m_stackInviteInfo;						// 초대 메시지
};