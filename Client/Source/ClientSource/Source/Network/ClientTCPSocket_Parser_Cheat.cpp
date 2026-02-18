#include "pch.h"
#include "ClientTCPSocket.h"
#include "MedalManager.h"
#include "MedalSetBase.h"
#include "UI/UIMainFrame.h"
#include "UI/UITopMenu_V15.h"
#include "UI/UIHudManager.h"
#include "MedalContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "StageBattle/ScoreContext.h"
#include "StageBattle/AIModeContext.h"
#include "StageBattle/DinoModeContext.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_CHEAT_MEDAL_COMMAND_ACK :			return "PROTOCOL_CHEAT_MEDAL_COMMAND_ACK";
		case PROTOCOL_CHEAT_REDUCE_TS_EVENT_ACK :		return "PROTOCOL_CHEAT_REDUCE_TS_EVENT_ACK";
		case PROTOCOL_CHEAT_INCREASE_KILL_COUNT_ACK :	return "PROTOCOL_CHEAT_INCREASE_KILL_COUNT_ACK";
		case PROTOCOL_CHEAT_PLAY_SOLO_ACK:				return "PROTOCOL_CHEAT_PLAY_SOLO_ACK";
		case PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK:		return "PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK";
		case PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK:				return "PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK";
		case PROTOCOL_CHEAT_ITEM_AUTH_ACK:						return "PROTOCOL_CHEAT_ITEM_AUTH_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_CHEAT_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_CHEAT_UNKNOWN";
		}
	}
}

void ClientTCPSocket::_PacketParsingCheat( i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Cheat.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Cheat.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()));
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()));
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Cheat_Function( void)
{
	RegisterParser( PROTOCOL_CHEAT_MEDAL_COMMAND_ACK,		&ClientTCPSocket::__Parse_Cheat_Medal_Command );
	RegisterParser( PROTOCOL_CHEAT_REDUCE_TS_EVENT_ACK,		&ClientTCPSocket::__Parse_Cheat_Reduce_TSEvent );
	RegisterParser( PROTOCOL_CHEAT_INCREASE_KILL_COUNT_ACK,	&ClientTCPSocket::__Parse_Cheat_Increase_Killcount );
	RegisterParser( PROTOCOL_CHEAT_PLAY_SOLO_ACK,			&ClientTCPSocket::__Parse_Cheat_Play_Solo );
	RegisterParser( PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK,			&ClientTCPSocket::__Parse_Cheat_ClanWarMatchingPoint );
	RegisterParser( PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK,					&ClientTCPSocket::__Parse_Cheat_ClanWarMatchResult );
	RegisterParser( PROTOCOL_CHEAT_ITEM_AUTH_ACK,						&ClientTCPSocket::__Parse_Cheat_RandomBoxAuthResult );
}

void ClientTCPSocket::__Parse_Cheat_Medal_Command( i3NetworkPacket * pPacket )
{
	PACKET_MEDAL_COMMAND_ACK		MedalAck;
 
	pPacket->ReadData(&MedalAck, sizeof(PACKET_MEDAL_COMMAND_ACK));

	if( EVENT_ERROR_SUCCESS == MedalAck.m_i32Rv )
	{
		switch( MedalAck.m_MedalCommand.m_ui8CommandType ) 
		{
		case MEDAL_COMMAND_TYPE_COUNT :
			{
				INT32 i32Type = MEDAL_SET_TYPE_NORMAL;

				switch ( MedalAck.m_MedalCommand.m_ui8MedalType )
				{
				case MEDAL_TYPE_NORMAL:	// 일반 메달
					{
						i32Type = MEDAL_SET_TYPE_NORMAL;
					}
					break;
				case MEDAL_TYPE_SET:	// 
					{
						i32Type = CMedalManager::i()->getMedalSetTypeByIndex( MedalAck.m_MedalCommand.m_ui16Idx);
					}
					break;
				default:
					return;
					break;
				}

				if(i32Type == -1) return;

				MEDAL_STATE MedalComplete;
				MedalComplete.m_ui8MedalType = MedalAck.m_MedalCommand.m_ui8MedalType;
				MedalComplete.m_ui16MedalIdx = MedalAck.m_MedalCommand.m_ui16Idx;
				MedalComplete.m_ui16Count = MedalAck.m_MedalCommand.m_ui16Count;

				MedalSetBase * pBase = CMedalManager::i()->getMedalSet(i32Type);

				if( nullptr == pBase ) return;
				pBase = CMedalManager::i()->getMedalSet(i32Type);
				pBase->setBattleInfo(MedalComplete);
				CMedalManager::i()->PushMedalMessageInfo(MedalComplete);
			}
			break;
		default:
			return;
			break;
		}
	}
	else // 실패시 로그 기록
	{
		char strErrorLog[MAX_PATH];
		i3::snprintf( strErrorLog, MAX_PATH, "[MEDALCOMMAND ERROR] %x ", MedalAck.m_i32Rv );

		I3PRINTLOG(I3LOG_QA,strErrorLog);
		I3TRACE(strErrorLog);
	}
}

void ClientTCPSocket::__Parse_Cheat_Reduce_TSEvent( i3NetworkPacket * pPacket )
{
	pPacket->ReadData( UserInfoContext::i()->SetUserInfoDaily(),	sizeof(USER_INFO_DAILY) );

	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	if ( pFrame )
	{
		if (g_pEnvSet->m_nUIVersion == UFV_1_5)
			pFrame->GetTopMenu_V15()->cb_tmenu_gauge(true);
	}

	UINT32 ui32PlaySec = UserInfoContext::i()->SetUserInfoDaily()->_DGPlaytime;

	i3::rc_wstring wstrTemp;
	i3::sprintf( wstrTemp, GAME_RCSTRING("STBL_IDX_EP_QA_CHEAT_REDUCE_TS"), ui32PlaySec/60, ui32PlaySec%60);

	g_pFramework->PutSystemChatting(wstrTemp);
}

void ClientTCPSocket::__Parse_Cheat_Increase_Killcount( i3NetworkPacket * pPacket)
{
	SCORE_INFO svrTempScore[SLOT_MAX_COUNT]; 
	pPacket->ReadData( &svrTempScore, sizeof(SCORE_INFO) * TEAM_COUNT );	

	if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
	{
		TEAM_TYPE team = (TEAM_TYPE)(BattleSlotContext::i()->getMainSlotIdx() % 2);

		INT32 preKillCount = ScoreContext::i()->GetSvrTeamScore(team)->m_KillCount;
		INT32 nowKillCount = svrTempScore[team].m_KillCount;

		if (preKillCount == nowKillCount)
		{
			AIModeContext::i()->KillCount = 0;
		}
		else
		{
			INT32 totalKillCount = nowKillCount;
			
			if (20 < nowKillCount && preKillCount < 20)			totalKillCount = 20;
			else if (40 < nowKillCount && preKillCount < 40)	totalKillCount = 40;
			else if (60 < nowKillCount && preKillCount < 60)	totalKillCount = 60;
			else if (80 < nowKillCount && preKillCount < 80)	totalKillCount = 80;
			else if (100 < nowKillCount && preKillCount < 100)	totalKillCount = 100;
			else if (120 < nowKillCount && preKillCount < 120)	totalKillCount = 120;
			else if (140 < nowKillCount && preKillCount < 140)	totalKillCount = 140;
			else if (160 < nowKillCount && preKillCount < 160)	totalKillCount = 160;
			else if (180 < nowKillCount && preKillCount < 180)	totalKillCount = 180;
			else if (200 < nowKillCount && preKillCount < 200)	totalKillCount = 200;

			switch(totalKillCount)
			{
			case 20:	// fall throght
			case 40:	// fall throght
			case 60:	// fall throght
			case 80:	// fall throght
			case 100:	// fall throght
			case 120:	// fall throght
			case 140:	// fall throght
			case 160:	// fall throght
			case 180:	// fall throght
			case 200:
				AIModeContext::i()->KillCount = totalKillCount;
				
				if (BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx())
				{
					GameEventSender::i()->SetEvent(EVENT_CHANGE_DIFFICULTY_LEVEL);
				}
				break;
			default:
				AIModeContext::i()->KillCount = 0;
				break;
			}
		}
	}

	ScoreContext::i()->SetSvrTeamScore(svrTempScore[TEAM_RED], svrTempScore[TEAM_BLUE]);
	
	SCORE_INFO info[ SLOT_MAX_COUNT];
	pPacket->ReadData( info,	sizeof(info));		//스코어
	ScoreContext::i()->setIndividualScores( info, sizeof(info));

	UINT16	aui16RoundCount[ TEAM_COUNT ];
	pPacket->ReadData( aui16RoundCount,		sizeof(UINT16) * TEAM_COUNT );

	switch( MyRoomInfoContext::i()->getStage()->GetStageMode() )
	{
	case STAGE_MODE_ESCAPE:
	case STAGE_MODE_CROSSCOUNT:
		DinoModeContext::i()->RedTouchDownCount = aui16RoundCount[TEAM_RED];
		DinoModeContext::i()->BlueTouchDownCount = aui16RoundCount[TEAM_BLUE];
		break;
	}
}

void ClientTCPSocket::__Parse_Cheat_Play_Solo( i3NetworkPacket * pPacket )
{
	MyRoomInfoContext::i()->SetSoloPlayFlag(true);

	g_pFramework->PutSystemChatting(GAME_RCSTRING("STBL_IDX_EP_QA_CHEAT_SOLO_PLAY"));
}


void ClientTCPSocket::__Parse_Cheat_ClanWarMatchingPoint( i3NetworkPacket * pPacket )
{
	INT32 i32MatchingPoint = 0;
	pPacket->ReadData( &i32MatchingPoint,	sizeof(INT32) );

	i3::rc_wstring wstrTemp;

	i3::sprintf( wstrTemp, L"MatchingPoint:%d \n", i32MatchingPoint );

	g_pFramework->PutSystemChatting(wstrTemp);
}

void ClientTCPSocket::__Parse_Cheat_ClanWarMatchResult( i3NetworkPacket * pPacket )
{
	TEAM_TYPE	eWinTeam;
	double		ResultClanPoint[TEAM_COUNT] = {0.f,};	

	pPacket->ReadData( &eWinTeam,		sizeof(TEAM_TYPE) );
	pPacket->ReadData( ResultClanPoint,	sizeof(double)*TEAM_COUNT );

	 
	i3::rc_wstring wstrTemp;

	switch( eWinTeam )
	{
	case TEAM_RED:
		i3::sprintf( wstrTemp, L"RED_TEAM_WIN / RedTeamClanPoint : %f / BlueTeamClanPoint : %f", ResultClanPoint[TEAM_RED], ResultClanPoint[TEAM_BLUE] );
		break;
	case TEAM_BLUE:
		i3::sprintf( wstrTemp, L"BLUE_TEAM_WIN / RedTeamClanPoint : %f / BlueTeamClanPoint : %f", ResultClanPoint[TEAM_RED], ResultClanPoint[TEAM_BLUE] );
		break;
	default:
		i3::sprintf( wstrTemp, L"UNKNOWN_TEAM_WIN / RedTeamClanPoint : %f / BlueTeamClanPoint : %f", ResultClanPoint[TEAM_RED], ResultClanPoint[TEAM_BLUE] );
		break;
	}

	g_pFramework->PutSystemChatting(wstrTemp);
}

void ClientTCPSocket::__Parse_Cheat_RandomBoxAuthResult(i3NetworkPacket * pPacket)
{
	INT32 result;
	i3::rc_wstring wstrMsg;

	pPacket->ReadData(&result, sizeof(INT32));

	if ( EV_SUCCESSED( result ) )
	{
		UINT32 invenTime;
		pPacket->ReadData(&invenTime, sizeof(UINT32));
		UserInfoContext::i()->SetInvenServerTime(invenTime);

		INVEN_BUFFER ItemBuffer;
		pPacket->ReadData(&ItemBuffer, sizeof(INVEN_BUFFER));

		// 인증후 아이템 ID 가 없으면 아이템은 삭제 된것이다. ( 개수제 -> 기간제로 바뀌었을경우, 개수제 모두사용 0으로 변경
		if (0 == ItemBuffer._ui32ItemID)
		{
			CInvenList::i()->DeleteInvenBuffer(ItemBuffer._TItemWareDBIdx);
		}
		else
		{
			CInvenList::i()->UpdateInvenBuffer(&ItemBuffer);
		}

		CInvenList::i()->SetLatestUsedCashItemFlag(ItemBuffer._ui32ItemID);
		wstrMsg = L"[Cheat RandomBox] Succ Auth";
	}
	else
	{
		//i3::rc_wstring
		if ( EVENT_ERROR_RS_START_FAIL_NOT_ENOUGH_ITEM == result )
			wstrMsg = L"[Cheat RandomBox] Fail! Need More Box";
		else if ( EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL == result )
			wstrMsg = L"[Cheat RandomBox] Fail! Need More Inven Slot";
		else
			wstrMsg = L"[Cheat RandomBox] Fail Auth";
	}

	g_pFramework->getChatBox()->PutSystemChatting(wstrMsg);
	//GameEventReceiver::i()->PushReceivedGameEvent(EVENT_ITEM_AUTH, result);
}
