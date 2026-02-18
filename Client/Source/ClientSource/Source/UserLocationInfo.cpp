#include "pch.h"
#include "UserLocationInfo.h"
#include "ServerInfo.h"



//////////////////////////////////////////////////////////////////////////

CUserLocationInfo::CUserLocationInfo()
{
	m_State = 0;
}

CUserLocationInfo::CUserLocationInfo(UINT64 FriendState)
{
	m_State = FriendState;
}

FRIEND_STATE CUserLocationInfo::GetState() const
{
	return static_cast<FRIEND_STATE>(GET_FRIEND_STATE(m_State));
}

UINT32 CUserLocationInfo::GetMatch() const
{
	return static_cast<UINT32>(GET_FRIEND_MATCH(m_State));
}

bool CUserLocationInfo::IsMatch() const
{
	return (GetMatch() != MATCH_TEAM_INVALID_IDX) ? true : false;
}

UINT32	CUserLocationInfo::GetServerNumber() const
{
	INT32	ServerIndex = static_cast<UINT32>(GET_FRIEND_SERVER(m_State));

	if ( g_pServerInfo->IsUseServerSelectPage() )
		ServerIndex -= 1; 

	// 클라이언트에서 사용하는 게임서버인덱스는 0base 지만
	// 서버단에서는 게이트웨이 서버를 사용시 게이트웨이가 0이고 게임서버는 1base로 하기 때문에
	// 게이트웨이 사용시에 -1을 한다.

	// I3ASSERT(ServerIndex >= 0);
	return ServerIndex;
}

UINT32	CUserLocationInfo::GetChannelNumber() const
{
	return static_cast<UINT32>(GET_FRIEND_CHANNEL(m_State));
}

UINT32	CUserLocationInfo::GetRoomNumber() const
{
	return static_cast<UINT32>(GET_FRIEND_ROOMIDX(m_State));
}

UINT32	CUserLocationInfo::GetDisplayServerNumber() const
{
	return GetServerNumber();
}

UINT32 CUserLocationInfo::GetDisplayChannelNumber() const
{
	return GetChannelNumber() + 1;
}

UINT32	CUserLocationInfo::GetDisplayRoomNumber() const
{
	return GetRoomNumber() + 1;
}

UINT32 CUserLocationInfo::GetDisplayServerChannelNumber() const
{
	return GetDisplayServerNumber() * 10 + GetDisplayChannelNumber();
}
