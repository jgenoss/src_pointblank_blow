#include "pch.h"
#include "ClanStructDef.h"

namespace 
{
	const UINT16	INIT_VALUE_UINT16 = 0xFFFF;
	const INT8		INIT_VALUE_INT8	= -1;
}

// 클랜 팀 정보
void CLAN_MATCH_TEAM_INFO::ParseData(const MATCH_TEAM_INFO & Info)
{
	_TMatchTeamID		= Info.m_TTeamID;
	_i8ClanTeamIdx		= Info.m_i8ClanTeamIdx;
	_TeamState			= static_cast<TEAM_STATE>(Info.m_ui8TeamState);
	_ui8MaxPerson		= Info.m_ui8MaxPer;
	_ui8CurPerson		= Info.m_ui8CurPer;	
}

void CLAN_MATCH_TEAM_INFO::Reset(void)
{
	_TMatchTeamID		= INIT_VALUE_UINT16;
	_i8ClanTeamIdx		= INIT_VALUE_INT8;
	_TeamState			= TEAM_STATE_NONE;
	_ui8MaxPerson		= 0;
	_ui8CurPerson		= 0;
}

// 클랜 매칭 정보
void CLAN_ALL_MATCHING_INFO::ParseData(const CLAN_ALL_MATCHING_DATA & Data)
{
	CLAN_ALL_MATCHING_DATA & data = const_cast<CLAN_ALL_MATCHING_DATA &>(Data);

	_i32MatchCompleteTeamCount	= data.m_i32MatchedTeamCount;
	_i32MatchWaitngTeamCount	= data.m_i32MatchingTeamCount;
}

void CLAN_ALL_MATCHING_INFO::Reset(void)
{
	_i32MatchCompleteTeamCount	= 0;
	_i32MatchWaitngTeamCount	= 0;
}


// 클랜 매치 방 정보
void CLAN_MATCH_ROOM_INFO::ParseData(const CLAN_MATCH_TEAM_INFO & Info)
{
	const INT8	i8TeamIdx = Info.getClanTeamIdx();
	const UINT8	ui8CurPerson = Info.getCurPerson();
	const UINT8	ui8MaxPerson = Info.getMaxPerson();

	i3::sprintf(_wstrRoomName, GAME_RCSTRING("STR_CLANCHANNEL_SQUADLIST"), i8TeamIdx + 1, ui8CurPerson, ui8MaxPerson);

	_ui8RoomIdx		= Info.getClanTeamIdx();
	_ActivateSlot	= false;
}

void CLAN_MATCH_ROOM_INFO::Reset(void)
{
	_wstrRoomName.clear();
	_ui8RoomIdx = 0xFF;
	_ActivateSlot = false;
}

// 클랜전 분대 멤버 정보
void CLAN_MATCH_SQUAD_MEMBER::ParseData(const MATCH_MEMBER_INFO & Info)
{
	_ui16UID		=	Info.m_TUID;
	_MemberType		=	static_cast<MEMBER_TYPE>(Info.m_ui8MemberType);
	_i8SlotIdx		=	Info.m_i8Slot;
	_ui8Rank		=	Info.m_ui8Rank;
	_ui8NickColor	=	Info.m_ui8NickColor;
	_ui8MerClass	=	Info.m_ui8MerClass; 

	_wstrNickName = Info.m_strNick;
}

void CLAN_MATCH_SQUAD_MEMBER::Reset(void)
{
	_ui16UID		= 0;
	_MemberType		= MEMBER_CLAN;
	_i8SlotIdx		= INIT_VALUE_INT8;
	_ui8Rank		= 0xFF;
	_ui8NickColor	= 0;
	_wstrNickName	= L"";
	_ui8MerClass	= 0;
}


// 클랜전 분대 정보.
void CLAN_MATCH_SQUAD_INFO::ParseData(const CLAN_WAR_CREATE_TEAM_DATA & Data, const MATCH_MEMBER_INFO & Member)
{
	const MATCH_TEAM_INFO TeamInfo = const_cast<CLAN_WAR_CREATE_TEAM_DATA &>(Data).m_TeamlInfo;

	CLAN_MATCH_TEAM_INFO::ParseData(TeamInfo);

	_i8LeaderSlot	= 0;
	_i8MySlot		= 0;
	
	//	분대 이름 (%d 팀 정보)
	i3::sprintf(_wstrSquadName, GAME_RCSTRING("STR_CLANCHANNEL_SQUADINFO"), TeamInfo.m_i8ClanTeamIdx + 1 );

	_Member[0].ParseData(Member);
}

void CLAN_MATCH_SQUAD_INFO::ParseData(const CLAN_WAR_JOIN_TEAM_DATA & Data)
{
	CLAN_WAR_JOIN_TEAM_DATA & data = const_cast<CLAN_WAR_JOIN_TEAM_DATA &>(Data);

	const MATCH_TEAM_DETAIL_INFO TeamInfo = data.m_TeamDetailInfo;

	CLAN_MATCH_TEAM_INFO::ParseData(TeamInfo);

	// 리더 슬롯
	_i8LeaderSlot = TeamInfo.m_i8LeaderSlot;
	
	// 내 슬롯
	if( data.m_bIsMyEnter )
		_i8MySlot = data.m_i8JoinSlot;

	//	분대 이름 (%d 팀 정보)
	i3::sprintf(_wstrSquadName, GAME_RCSTRING("STR_CLANCHANNEL_SQUADINFO"), TeamInfo.m_i8ClanTeamIdx + 1 );

	// 작전명
	_wstrOperation = TeamInfo.m_strOperation;

	const MATCH_MEMBER_INFO * pMember = &data.m_aMemberInfo;
	for( UINT8 idx = 0 ; idx < TeamInfo.m_ui8CurPer; ++idx )
	{
		INT8 slot_idx = pMember[idx].m_i8Slot;
		_Member[slot_idx].ParseData(pMember[idx]);
	}
}

void CLAN_MATCH_SQUAD_INFO::Reset(void)
{
	CLAN_MATCH_TEAM_INFO::Reset();

	_i8LeaderSlot	= INIT_VALUE_INT8;
	_i8MySlot		= INIT_VALUE_INT8;
	_wstrSquadName	= L"";
	_wstrOperation	= L"";

	for(INT32 idx = 0 ; idx < MAX_CLAN_MATCH_MEMBER ; ++idx )
		_Member[idx].Reset();
}

void CLAN_MATCH_BATTLE_TEAM::ParseData(const CLAN_MATCHING_TEAM_INFO & Info)
{
	_wstrClanName	= Info._name;
	_ui32ClanMark	= Info._mark;
	_TMatchTeamID	= Info.m_TTeamID;
}

void CLAN_MATCH_BATTLE_TEAM::Reset(void)
{
	_wstrClanName	= L"";
	_ui32ClanMark	= 0;
	_TMatchTeamID	= INIT_VALUE_UINT16;
}

/************************************************************************/
/*									용병 정보							*/
/************************************************************************/

void CLAN_MERCENARY_INFO::Reset(void)
{
	_TMercenaryID	= INIT_VALUE_UINT16;
	_State			= MER_STATE_NONE;
	_ClassType		= WEAPON_CLASS_UNKNOWN;
	_ui8MerRank		= 0;
	_ui8NickColor	= 0;
	_wstrNickName	= L"";
}

void CLAN_MERCENARY_INFO::ParseData(const MERCENARY_INFO & Info)
{
	_TMercenaryID	= Info.m_TMercenaryID;
	_State			= static_cast<MERCENARY_STATE>(Info.m_ui8State);
	_ClassType		= static_cast<WEAPON_CLASS_TYPE>(Info.m_ui8Class);
	_ui8MerRank		= Info.m_ui8MerRank;
	_ui8NickColor	= Info.m_ui8NickColor;

	_wstrNickName = Info.m_strNick;
}

void CLAN_MERCENARY_RECORD::Reset(void)
{
	_i32MatchCount		= 0;
	_i32WinMatchCnt		= 0;
	_i32LoseMatchCnt	= 0;
	_i32DisconMatchCnt	= 0;
	_i32KillCount		= 0;
	_i32DeathCount		= 0;
	_i32Headshot		= 0;
	_i32AssistCount		= 0;
}

void CLAN_MERCENARY_RECORD::ParseData(const MERCENARY_RECORD & Info)
{
	_i32MatchCount		= Info.m_i32MatchCount;
	_i32WinMatchCnt		= Info.m_i32WinMatchCnt;
	_i32LoseMatchCnt	= Info.m_i32LoseMatchCnt;
	_i32DisconMatchCnt	= Info.m_i32DisconMatchCnt;
	_i32KillCount		= Info.m_i32KillCount;
	_i32DeathCount		= Info.m_i32DeathCount;
	_i32Headshot		= Info.m_i32Headshot;
	_i32AssistCount		= Info.m_i32AssistCount;
}

void CLAN_PERSONAL_MATCH_RECORD::ParseData(const MATCHMAKING_RECORD & info)
{
	_i8RoomSlotIdx		= info.m_i8RoomSlotIdx;
	_ui32Killcount		= info.m_ui32Killcount;
	_ui32Death			= info.m_ui32Death;
	_ui32Killcount		= info.m_ui32Win;
	_ui32Death			= info.m_ui32Lose;
}

void CLAN_PERSONAL_MATCH_RECORD::Reset(void)
{
	_i8RoomSlotIdx		= -1;
	_ui32Killcount		= 0;
	_ui32Death			= 0;
	_ui32Killcount		= 0;
	_ui32Death			= 0;
}
