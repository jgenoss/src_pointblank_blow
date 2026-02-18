#ifndef __PROTOCOL_SERVER_CLAN_BATTLE_STRUCT_H__
#define __PROTOCOL_SERVER_CLAN_BATTLE_STRUCT_H__

#pragma pack( push, 1)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 클랜 매치에 사용되는 프로토콜
// PROTOCOL_CS_MATCH_NEW
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_REQ
//////////////////////////////////////////////////////////////////////////
// struct PACKET_GC_H_CS_MATCH_NEW_REQ				[재정의되어 생략]
struct PACKET_GC_D_CS_MATCH_GET_CLAN_ROOM_INFO
{
	UINT8	m_ui8ChannelNum;
};
// struct PACKET_D_CS_MATCH_NEW_GET_CLAN_ROOM_INFO	[재정의되어 생략]

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_MATCH_NEW_CLAN_INFO_REQ
//////////////////////////////////////////////////////////////////////////
// struct PACKET_GC_H_CS_MATCH_NEW_REQ				[재정의되어 생략]
struct PACKET_GC_D_CS_MATCH_CLAN_INFO_REQ
{
	UINT32	m_ui32CUID[TEAM_COUNT];					// Team Clan No(Red(0),Blue(1))	
	UINT32	m_ui32ChannelNum;
	UINT32	m_ui32RoomIdx;
};

// struct PACKET_GC_H_CLAN_ACK						[재정의되어 생략]
struct PACKET_GC_D_CS_MATCH_CLAN_INFO
{
	UINT32				m_ui32ChannelNum;
	UINT32				m_ui32RoomIdx;

	CLAN_MATCH_INFO		m_ClanMatchInfo[TEAM_COUNT];
};

//////////////////////////////////////////////////////////////////////////
// PROTOCOL_CS_MATCH_NEW_OPERATION_REQ
//////////////////////////////////////////////////////////////////////////
// struct PACKET_GC_H_CS_MATCH_NEW_REQ				[재정의되어 생략]
struct PACKET_GC_D_CS_MATCH_OPERATION
{
	char	m_strOperation[NET_CLAN_OPERATION_SIZE];
};

struct PACKET_GC_CS_MATCH_OPERATION_ACK
{
	UINT8	m_ui8UserCount;
	INT32	m_i32SessionIdx[CLAN_TEAM_MEMBER_COUNT];
	T_UID	m_i64UID[CLAN_TEAM_MEMBER_COUNT];
	
	TTCHAR	m_strOperation[NET_CLAN_OPERATION_SIZE];
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)

#endif