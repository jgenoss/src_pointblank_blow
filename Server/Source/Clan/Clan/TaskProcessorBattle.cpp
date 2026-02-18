#include "pch.h"
#include "TaskProcessor.h"
#include "PacketLocker.h"
#include "Gateway.h"
//#include "LogFile.h"
#include "DataBase.h"
#include "Receiver.h"

BOOL CTaskProcessor::_DoTaskForGameBattle( UINT32 ui32idxBuffer, i3NetworkPacket* pPacket )
{
	CLAN_USER_INFO UserInfo;
	UserInfo.ui32ServerIdx = ui32idxBuffer;
	pPacket->ReadData( &UserInfo.i32SessionIdx,	sizeof(INT32) );
	pPacket->ReadData( &UserInfo.i64UID,		sizeof(T_UID) );
	pPacket->ReadData( &UserInfo.ui32ClanDBIdx,	sizeof(UINT32) );

	switch(pPacket->GetProtocolID()) 
	{
	case PROTOCOL_CS_MATCH_NEW_GET_CLAN_ROOM_INFO_REQ:
		{
			PACKET_GC_D_CS_MATCH_GET_CLAN_ROOM_INFO		PacketReqGCD;
			pPacket->ReadData( &PacketReqGCD,			sizeof(PACKET_GC_D_CS_MATCH_GET_CLAN_ROOM_INFO) );
			BattleMatchGetClanRoomInfo( &UserInfo, (UINT32)((ui32idxBuffer*VIEW_CHANNEL_COUNT)+PacketReqGCD.m_ui8ChannelNum) );
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_CONTEXT_REQ:
		{
			BattleMatchTeamContext( &UserInfo );
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_CONTEXT_REQ:
		{
			UINT8 ui8Channel;

			pPacket->ReadData(&ui8Channel,	sizeof(UINT8));

			BattleMatchAllTeamContext( &UserInfo, (UINT32)((ui32idxBuffer*VIEW_CHANNEL_COUNT)+ui8Channel) );
		}
		break;
	case PROTOCOL_CS_MATCH_ALL_TEAM_LIST_REQ:
		{
			UINT8	ui8Channel;
			UINT16	ui16Section;

			pPacket->ReadData(&ui8Channel,	sizeof(UINT8));
			pPacket->ReadData(&ui16Section,	sizeof(UINT16));

			BattleMatchAllTeamList( &UserInfo, (UINT32)((ui32idxBuffer*VIEW_CHANNEL_COUNT)+ui8Channel), ui16Section );
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_DETAIL_INFO_REQ:
		{
			UINT32 ui32TeamIdx;

			pPacket->ReadData(&ui32TeamIdx,	sizeof(UINT32));

			BattleTeamDetailInfo( &UserInfo, ui32TeamIdx );
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_JOIN_REQ:
		{
			UINT32 ui32TeamIdx;
			UINT8  ui8Type;

			pPacket->ReadData(&ui32TeamIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8Type,		sizeof(UINT8));

			switch( ui8Type )
			{
				case 0:
					BattleTeamJoin( &UserInfo, ui32TeamIdx );
					break;
				case 1:
					BattleTeamJoinNaming( &UserInfo, (UINT8)ui32TeamIdx );
					break;
			}
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_LEAVE_REQ:
		{
			BattleTeamLeave( &UserInfo );
		}
		break;
	case PROTOCOL_CS_MATCH_TEAM_CHANGE_PER_REQ:
		{
			UINT8	ui8PerMax;

			pPacket->ReadData(&ui8PerMax,		sizeof(UINT8));

			BattleTeamChangePer( &UserInfo, ui8PerMax );
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_REQUEST_REQ:
		{
			UINT32 ui32RequsetTeamIdx;

			pPacket->ReadData(&ui32RequsetTeamIdx,	sizeof(UINT32));

			BattleClanRequset( &UserInfo, ui32RequsetTeamIdx );
		}
		break;
	case PROTOCOL_CS_MATCH_FIGHT_ACCECT_REQ:
		{
			UINT32 ui32SpIdx;
			UINT32 ui32RequsetTeamIdx;
			UINT8  ui8Accept;

			pPacket->ReadData(&ui32SpIdx,			sizeof(UINT32));
			pPacket->ReadData(&ui32RequsetTeamIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui8Accept,			sizeof(UINT8));

			BattleClanAccept( &UserInfo, ui32SpIdx, ui32RequsetTeamIdx, ui8Accept );
		}
		break;
	case PROTOCOL_CS_MATCH_CHATING_REQ:
		{
			UINT8	ui8ChatSize;
			UINT8	ui8NickColor;
			TTCHAR	strChating[NET_CLAN_CHATING_SIZE];
			bool	bIsGMUser;

			pPacket->ReadData(&bIsGMUser,		sizeof(bool));
			pPacket->ReadData(&ui8NickColor,	sizeof(UINT8));
			pPacket->ReadData(&ui8ChatSize,		sizeof(UINT8));
			pPacket->ReadData(strChating,		sizeof(TTCHAR) * ui8ChatSize);

			strChating[ NET_CLAN_CHATING_SIZE - 1 ] = '\0';

			MatchUserChatting( &UserInfo, ui8ChatSize, strChating, bIsGMUser, ui8NickColor );
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_CREATE_GS_ACK:
		{
			UINT32 ui32TeamIdx;
			UINT32 ui32RequsetTeamIdx;
			UINT32 ui32RoomIdx;

			pPacket->ReadData(&ui32TeamIdx,			sizeof(UINT32));
			pPacket->ReadData(&ui32RequsetTeamIdx,	sizeof(UINT32));
			pPacket->ReadData(&ui32RoomIdx,			sizeof(UINT32));

			BattleClanRoomJoin( &UserInfo, ui32TeamIdx, ui32RequsetTeamIdx, ui32RoomIdx );
		}
		break;
	case PROTOCOL_CS_MATCH_ROOM_LEAVE_REQ:
		{
			BattleClanRoomLeave( &UserInfo );
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}