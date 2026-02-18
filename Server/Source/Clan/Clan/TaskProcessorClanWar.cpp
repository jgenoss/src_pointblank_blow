#include "pch.h"
#include "TaskProcessor.h"

#include "Gateway.h"
#include "DataBase.h"


BOOL CTaskProcessor::_DoTaskForClanWar(UINT32 ui32idxBuffer, i3NetworkPacket* pPacket)
{
	if( !g_pConfig->IsContentsEnabled(CONTENTS_CLAN_MATCH_NEW) ) return FALSE;

	if( ui32idxBuffer == 0 ) return FALSE;

	i3NetworkPacket SendPacket;

	CLAN_USER_INFO stUserInfo;
	stUserInfo.ui32ServerIdx = ui32idxBuffer;
	pPacket->ReadData(&stUserInfo.i32SessionIdx,	sizeof(INT32) );
	pPacket->ReadData(&stUserInfo.i64UID,			sizeof(T_UID) );
	pPacket->ReadData(&stUserInfo.ui32ClanDBIdx,	sizeof(UINT32) );

	CTeamManager* pTeamManager				= m_pTeamManager[ui32idxBuffer];
	CMatchManager* pMatchManager			= m_pMatchManager[ui32idxBuffer];
	CMercenaryManager* pMercenaryManager	= m_pMercenaryMgr[ui32idxBuffer];

	switch( pPacket->GetProtocolID() ) 
	{
	case PROTOCOL_CLAN_WAR_START_GAME_SERVER_REQ:
	{	// Game 서버 크래시의 경우 초기화 해줍니다.
		pTeamManager->ReleaseAllMatchTeam();
		pMercenaryManager->ReleaseAllMercenary();
	}
	break;

	case PROTOCOL_CLAN_WAR_MATCH_TEAM_LIST_REQ:
		{
			RequestMatchTeamList(pTeamManager, &stUserInfo);
		}
		break;

	case PROTOCOL_CLAN_WAR_CREATE_TEAM_REQ:
		{
			UINT8 ui8MaxPer;
			INT32 i32KDWeight;
			pPacket->ReadData(&ui8MaxPer,	sizeof(UINT8));
			pPacket->ReadData(&i32KDWeight, sizeof(INT32));

			T_TeamID	TTeamID = MATCH_TEAM_INVALID_IDX;

			T_RESULT TResult = CreateClanMatchTeam(pTeamManager, &stUserInfo, ui8MaxPer, i32KDWeight, TTeamID);
			
			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_CREATE_TEAM_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&TResult,						sizeof(T_RESULT));
			
			if( EV_SUCCESSED( TResult ) )
			{
				pTeamManager->MakePacketTeam(TTeamID, &SendPacket);
			}
 
			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_JOIN_TEAM_REQ:
		{
			T_TeamID	TTeamID;
			INT32		i32KillDeathRate;
			INT8		i8JoinSlot;

			pPacket->ReadData(&TTeamID,				sizeof(UINT16));
			pPacket->ReadData(&i32KillDeathRate,	sizeof(INT32));

			T_RESULT TResult = JoinClanMatchTeam( pTeamManager, &stUserInfo, TTeamID, i32KillDeathRate, i8JoinSlot);

			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,		sizeof(T_UID));
			SendPacket.WriteData(&TResult,					sizeof(T_RESULT));

			if( EV_SUCCESSED( TResult) )
			{
				SendPacket.WriteData(&i8JoinSlot,			sizeof(INT8));
				pTeamManager->MakePacketTeamDetail(TTeamID, &SendPacket);
				pTeamManager->MakePacketMemberInfo(TTeamID, &SendPacket);
			}

			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_LEAVE_TEAM_REQ:
		{
			T_TeamID	TTeamID;
			INT8		i8LeaveSlot;
			pPacket->ReadData(&TTeamID, sizeof(T_TeamID));
			 
			T_RESULT TResult = LeaveClanMatchTeam(pTeamManager, &stUserInfo , TTeamID, i8LeaveSlot);

			CLAN_WAR_INFO stWarInfo;
			i3mem::Copy( &stWarInfo, &stUserInfo, sizeof(CLAN_USER_INFO) );
			stWarInfo.m_TTeamID = TTeamID;
			SendLeaveMatchTeam( pTeamManager, &stWarInfo, i8LeaveSlot, TResult );
		}
		break;

	case PROTOCOL_CLAN_WAR_CHANGE_OPERATION_REQ:
		{
			T_TeamID	TTeamID;
			UINT8		ui8Length = 0;
			TTCHAR		strOperation[NET_CLAN_OPERATION_SIZE];

			pPacket->ReadData(&TTeamID,		sizeof(T_TeamID));
			pPacket->ReadData(&ui8Length,	sizeof(UINT8));
			pPacket->ReadData(strOperation, sizeof(TTCHAR) * ui8Length);
			strOperation[ui8Length] = '\0';

			T_RESULT TResult = ChangeOperationClanMatchTeam(pTeamManager, stUserInfo.i64UID, TTeamID, strOperation);

			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_CHANGE_OPERATION_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,		sizeof(T_UID));
			SendPacket.WriteData(&TResult,					sizeof(T_RESULT));

			if( EV_SUCCESSED( TResult) )
			{
				pTeamManager->MakePacketOperation(TTeamID, &SendPacket);
			}

			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_REQ:
		{
			T_TeamID	TTeamID;
			UINT8		ui8MaxPerCount = 0;
		
			pPacket->ReadData(&TTeamID,			sizeof(T_TeamID));
			pPacket->ReadData(&ui8MaxPerCount,	sizeof(UINT8));

			T_RESULT TResult = ChangeMaxPerCountClanMatchTeam(pTeamManager, stUserInfo.i64UID, TTeamID, ui8MaxPerCount);

			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_CHANGE_MAX_PER_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,		sizeof(T_UID));
			SendPacket.WriteData(&TResult,					sizeof(T_RESULT));
			if( EV_SUCCESSED( TResult) )
			{
				pTeamManager->MakePacketPerMaxCount(TTeamID, &SendPacket);
			}

			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_MATCHMAKING_REQ:
		{
			T_TeamID	TTeamID;
			pPacket->ReadData(&TTeamID, sizeof(T_TeamID));

			T_RESULT TResult = InsertMatchmakingRequest( pTeamManager, ui32idxBuffer, TTeamID);

			i3NetworkPacket stSendPacket(PROTOCOL_CLAN_WAR_MATCHMAKING_ACK);
			stSendPacket.WriteData(&TResult,					sizeof(T_RESULT));
			if( EV_SUCCESSED( TResult)  )
			{
				pTeamManager->MakePacketTeamUIDList(TTeamID, &stSendPacket);
			}
		
			g_pGateway->SendPacket(ui32idxBuffer, &stSendPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_REQ:
		{
			T_TeamID	TTeamID;
			pPacket->ReadData(&TTeamID, sizeof(T_TeamID));

			T_RESULT TResult = CancelMatchmaking( pTeamManager, TTeamID);

			if( EV_FAILED( TResult) )
			{
				i3NetworkPacket stSendPacket(PROTOCOL_CLAN_WAR_CANCEL_MATCHMAKING_ACK);
				stSendPacket.WriteData(&TResult, sizeof(T_RESULT));
				g_pGateway->SendPacket(ui32idxBuffer, &stSendPacket);
			}
		}
		break;
	case PROTOCOL_CLAN_WAR_CREATE_ROOM_ACK:
		{
			T_MatchingID TMatchingID;
			INT32 i32ChannelIdx;
			INT32 i32MatchRoomIdx;

			pPacket->ReadData(&TMatchingID,		sizeof(T_MatchingID));
			pPacket->ReadData(&i32ChannelIdx,	sizeof(INT32));
			pPacket->ReadData(&i32MatchRoomIdx,	sizeof(INT32));

			if( EV_FAILED(i32MatchRoomIdx)  )
			{
				pMatchManager->RemoveMatchingTeam(TMatchingID);
				break;
			}			

			i3NetworkPacket stJoinRoomPacket(PROTOCOL_CLAN_WAR_JOIN_ROOM_REQ);
			stJoinRoomPacket.WriteData(&TMatchingID,	sizeof(T_MatchingID));

			pMatchManager->SetRoom(TMatchingID, i32ChannelIdx, i32MatchRoomIdx);
			pMatchManager->JoinRoomMatchMembers(TMatchingID, &stJoinRoomPacket );

			g_pGateway->SendPacket(ui32idxBuffer, &stJoinRoomPacket);
		}
		break;


	case PROTOCOL_CLAN_WAR_JOIN_ROOM_ACK:
		{
			T_RESULT		TResult;
			T_MatchingID	TMatchingID;
			T_TeamID		TTeamID;

			pPacket->ReadData(&TResult,			sizeof(T_RESULT));
			pPacket->ReadData(&TMatchingID,		sizeof(T_MatchingID));
			pPacket->ReadData(&TTeamID,			sizeof(T_TeamID));

			if( EV_SUCCESSED( TResult ) ) break;

			
			// 매치룸 입장에 실패했다면 매치팀에서 퇴장시켜줍니다.
			INT8	i8LeaveSlot;
			LeaveClanMatchTeam(pTeamManager, &stUserInfo , TTeamID, i8LeaveSlot);
			
			CLAN_WAR_INFO stWarInfo;
			i3mem::Copy( &stWarInfo, &stUserInfo, sizeof(CLAN_USER_INFO) );
			stWarInfo.m_TTeamID = TTeamID;
			SendLeaveMatchTeam( pTeamManager, &stWarInfo, i8LeaveSlot, TResult );
			
		}
		break;

	case PROTOCOL_CLAN_WAR_LEAVE_ROOM_REQ:
		{
			T_MatchingID	TMatchingID;
			bool			bRoomDestroy;
			UINT8			ui8RoomState;	// ROOM_STATE
			bool			bForceLeave;	
			T_TeamID		TTeamID;

			pPacket->ReadData(&TMatchingID,		sizeof(T_MatchingID));
			pPacket->ReadData(&bRoomDestroy,	sizeof(bool));
			pPacket->ReadData(&TTeamID,			sizeof(T_TeamID));
			pPacket->ReadData(&ui8RoomState,	sizeof(UINT8));
			pPacket->ReadData(&bForceLeave,		sizeof(bool));

			INT8 i8LeaveSlot;
			pMatchManager->LeaveMatchRoom(pTeamManager, &stUserInfo, TMatchingID, TTeamID, i8LeaveSlot, ui8RoomState, bForceLeave );

			if( bRoomDestroy  )
			{	// 방 폭이면 매칭팀 제거 합니다.
				pMatchManager->RemoveMatchingTeam(TMatchingID);
			}			
 
		}
		break;

	case PROTOCOL_CLAN_WAR_MERCENARY_LIST_REQ:
		{
			pMercenaryManager->SendMercenaryList(&stUserInfo);
		}
		break;

	case PROTOCOL_CLAN_WAR_REGIST_MERCENARY_REQ:
		{
			REGIST_MERCENARY	stRegistMercenary;
			stRegistMercenary.m_TUID = stUserInfo.i64UID;

			pPacket->ReadData(&stRegistMercenary.m_ui8Class,		sizeof(UINT8));
			pPacket->ReadData(&stRegistMercenary.m_ui8NickColor,	sizeof(UINT8));	
			
			pPacket->ReadData(&stRegistMercenary.m_strNick,			sizeof(TTCHAR)* NET_NICK_NAME_SIZE);
			//pPacket->ReadData(&stRegistMercenary.m_i32KillDeathRate,sizeof(INT32));
			
			T_RESULT TResult = pMercenaryManager->RegistMercenary(&stRegistMercenary, &stUserInfo );
			if( EV_SUCCESSED( TResult ) ) break;

			// 용병 등록에 실패했을 경우 입니다.
			i3NetworkPacket stPacket(PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK);
			stPacket.WriteData(&stUserInfo.i32SessionIdx,	sizeof(INT32));
			stPacket.WriteData(&stUserInfo.i64UID,			sizeof(T_UID)); 
			stPacket.WriteData(&TResult,					sizeof(T_RESULT));

			g_pGateway->SendPacket(ui32idxBuffer, &stPacket);			
		}
		break;

	case PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_REQ:
		{
			T_MerID	TMerID;
			pPacket->ReadData(&TMerID,		sizeof(T_MerID));

			T_RESULT TResult = EVENT_ERROR_SUCCESS;

			i3NetworkPacket stPacket(PROTOCOL_CLAN_WAR_REMOVE_MERCENARY_ACK);
			stPacket.WriteData(&stUserInfo.i32SessionIdx,	sizeof(INT32));
			stPacket.WriteData(&stUserInfo.i64UID,			sizeof(T_UID));
			
			if( !pMercenaryManager->RemoveMercenary(TMerID) )
			{
				TResult = EVENT_ERROR_CLAN_WAR_NOT_FIND_MERCENARY;
				stPacket.WriteData(&TResult, sizeof(T_RESULT));
				g_pGateway->SendPacket(ui32idxBuffer, &stPacket);
				break;
			}
			 
			stPacket.WriteData(&TResult,	sizeof(T_RESULT));
			stPacket.WriteData(&TMerID,		sizeof(T_MerID));

			g_pGateway->SendPacket(ui32idxBuffer, &stPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_MERCENARY_REQ:
		{
			T_MerID		TMerID;
			T_TeamID	TTeamID;
			pPacket->ReadData(&TMerID,		sizeof(T_MerID));
			pPacket->ReadData(&TTeamID,		sizeof(T_TeamID));

			T_RESULT TResult = InviteMercenary(pMercenaryManager, &stUserInfo, TMerID, TTeamID);
			if( EV_FAILED( TResult) )
			{
				i3NetworkPacket stPacket(PROTOCOL_CLAN_WAR_INVITE_MERCENARY_ACK);
				stPacket.WriteData(&stUserInfo.i64UID,	sizeof(T_UID));			// 초대한 팀 리더 UID				
				stPacket.WriteData(&TResult,			sizeof(T_RESULT)); 

				g_pGateway->SendPacket(ui32idxBuffer, &stPacket);
			}
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_ACCEPT_REQ:
		{
			T_MerID		TMerID;
			INT32		i32KillDeathRate;
			pPacket->ReadData(&TMerID,				sizeof(T_MerID));
			pPacket->ReadData(&i32KillDeathRate,	sizeof(INT32));

			CMercenary* pMercenary = pMercenaryManager->FindMercenary(TMerID);
			if( !pMercenary ) 
			{
				SendInviteAccept( EVENT_ERROR_CLAN_WAR_NOT_FIND_MERCENARY, &stUserInfo ); 
				break;
			}

			T_TeamID TTeamID = pMercenary->GetInviteTeamID();
			CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam( TTeamID );
			if( !pMatchTeam )  
			{
				SendInviteAccept( EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL, &stUserInfo );
				break;
			}

			INT8 i8JoinSlot;
			T_RESULT TResult = pMatchTeam->JoinMember(pMercenary,i32KillDeathRate,i8JoinSlot ); 
			if(EV_FAILED( TResult ) )
			{
				SendInviteAccept( TResult, &stUserInfo );
				break;
			}
			
			// 1. 수락한 용병에게 성공 알림.
			SendInviteAccept( TResult, &stUserInfo );

			// 2. 팀 입장 알림. ( 팀원 모두 )
			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_JOIN_TEAM_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,		sizeof(T_UID));
			SendPacket.WriteData(&TResult,					sizeof(T_RESULT));
			SendPacket.WriteData(&i8JoinSlot,				sizeof(INT8));
			pTeamManager->MakePacketTeamDetail(TTeamID, &SendPacket);
			pTeamManager->MakePacketMemberInfo(TTeamID, &SendPacket );
			
			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket); 
		}
		break;

	case PROTOCOL_CLAN_WAR_INVITE_DENIAL_REQ:
		{
			T_MerID		TMerID; 
			pPacket->ReadData(&TMerID,				sizeof(T_MerID));

			CMercenary* pMercenary = pMercenaryManager->FindMercenary(TMerID);
			if( !pMercenary ) 
			{
				SendInviteDenial( EVENT_ERROR_CLAN_WAR_NOT_FIND_MERCENARY, &stUserInfo, TMerID ); 
				break;
			}

			T_TeamID TTeamID = pMercenary->GetInviteTeamID();
			CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam( TTeamID );
			if( !pMatchTeam )  
			{
				SendInviteDenial( EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL, &stUserInfo, TMerID );
				break;
			}

			pMercenary->SetInviteTeamID(MATCH_TEAM_INVALID_IDX);

			SendInviteDenial( EVENT_ERROR_SUCCESS, &stUserInfo, TMerID, pMatchTeam->GetLeaderUID() );
		}
		break;

	case PROTOCOL_CLAN_WAR_RELEASE_INVITE_TEAM_REQ:
	{
		T_MerID		TMerID;
		pPacket->ReadData(&TMerID, sizeof(T_MerID));

		CMercenary* pMercenary = pMercenaryManager->FindMercenary(TMerID);
		if (!pMercenary)
		{
			break;
		}

		pMercenary->SetInviteTeamID(MATCH_TEAM_INVALID_IDX);
	}
	break;
	
	case PROTOCOL_CLAN_WAR_TEAM_CHATTING_REQ:
		{
			T_RESULT		TResult;
			T_TeamID		TTeamID; 
			UINT8			ui8ChatSize;
			TTCHAR			strChat[NET_CLAN_CHATING_SIZE];
			pPacket->ReadData(&TTeamID,				sizeof(T_TeamID));
			pPacket->ReadData(&ui8ChatSize,			sizeof(UINT8));
			pPacket->ReadData(strChat,				sizeof(TTCHAR)*ui8ChatSize);
			strChat[ui8ChatSize] = 0;
 
			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_TEAM_CHATTING_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,	sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,		sizeof(T_UID));
				
			CMatchTeam* pMatchTeam = pTeamManager->FindMatchTeam( TTeamID );
			if( !pMatchTeam )  
			{
				TResult = EVENT_ERROR_CLAN_BATTLE_TEAM_NOSEARCH_FAIL; 
				SendPacket.WriteData(&TResult,				sizeof(T_RESULT));
				g_pGateway->SendPacket(ui32idxBuffer, &SendPacket); 
				break;
			}

			TResult = EVENT_ERROR_SUCCESS;
			SendPacket.WriteData(&TResult,					sizeof(T_RESULT));
			SendPacket.WriteData(&ui8ChatSize,				sizeof(UINT8));
			SendPacket.WriteData(strChat,					sizeof(TTCHAR)*ui8ChatSize);

			pTeamManager->MakePacketTeamChatting(TTeamID, &SendPacket );

			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket); 
		 
		}
		break;

	case PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_REQ:
		{
			T_MerID	TMerID;
			pPacket->ReadData(&TMerID,		sizeof(T_MerID));

			T_RESULT TResult = EVENT_ERROR_SUCCESS; 
			MERCENARY_DETAIL_INFO	DetailInfo;
			CMercenary* pMercenary	= pMercenaryManager->FindMercenary(TMerID);
			T_UID		TMerUID		= -1;
			INT32		i32MerSessionIdx = -1;

			if( NULL == pMercenary )
			{
				TResult = EVENT_ERROR_CLAN_WAR_NOT_FIND_MERCENARY; 
			}
			else
			{	//용병정보를 찾은경우 상세정보를 가져온다.
				pMercenary->GetDetailInfo(DetailInfo);
				TMerUID = pMercenary->GetUserID();
				i32MerSessionIdx = pMercenary->GetSessionIdx();
			}			

			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_MERCENARY_DETAIL_INFO_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&TResult,						sizeof(T_RESULT));
			SendPacket.WriteData(&TMerID,						sizeof(T_MerID));
			SendPacket.WriteData(&TMerUID,						sizeof(T_UID));
			SendPacket.WriteData(&i32MerSessionIdx,				sizeof(INT32));
			SendPacket.WriteData(&DetailInfo,					sizeof(MERCENARY_DETAIL_INFO));
			  
			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;

	case PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_REQ:
		{
			T_TeamID	TTeamID;
			pPacket->ReadData(&TTeamID,		sizeof(T_TeamID));
 
			INT32 i32MatchingPoint = 	GetMatchingPoint(pTeamManager, TTeamID);
			
			SendPacket.SetProtocol(PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&i32MatchingPoint,				sizeof(INT32));
			  
			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;

	case PROTOCOL_CHEAT_CLAN_WAR_RESULT_REQ:
		{
			TEAM_TYPE		eWinTeam;
			T_MatchingID	TMatchingID;
			double			ResultClanPoint[TEAM_COUNT] = {0.f,};	

			pPacket->ReadData(&eWinTeam,			sizeof(TEAM_TYPE));
			pPacket->ReadData(&TMatchingID,			sizeof(T_MatchingID));
 
			pMatchManager->GetMatchResult(TMatchingID, eWinTeam, ResultClanPoint);
			
			SendPacket.SetProtocol(PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx,		sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID,			sizeof(T_UID));
			SendPacket.WriteData(&eWinTeam,						sizeof(TEAM_TYPE));
			SendPacket.WriteData(ResultClanPoint,				sizeof(double)*TEAM_COUNT);
			  
			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;

	case PROTOCOL_CLAN_WAR_RESULT_REQ:
		{
			SendPacket.SetProtocol(PROTOCOL_CLAN_WAR_RESULT_ACK);
			SendPacket.WriteData(&stUserInfo.i32SessionIdx, sizeof(INT32));
			SendPacket.WriteData(&stUserInfo.i64UID, sizeof(T_UID));
			T_RESULT TResult = EVENT_ERROR_SUCCESS;

			CLAN_DETAIL_INFO_SERVER* pstClan = FindClan2(stUserInfo.ui32ClanDBIdx);
			if (NULL == pstClan)
			{
				TResult = EVENT_ERROR_CLAN_NOSEARCH_CLANIDX;
				SendPacket.WriteData(&TResult, sizeof(T_RESULT));
			}
			else
			{
				SendPacket.WriteData(&TResult, sizeof(T_RESULT));

				MakePacketClanWarResult(pstClan, &SendPacket);
			}

			g_pGateway->SendPacket(ui32idxBuffer, &SendPacket);
		}
		break;
	case PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_LOAD_REQ:
		{
			RING_IN_DB stRingInDB;
			stRingInDB._i64UID			= stUserInfo.i64UID;
			stRingInDB._ui32ServerIdx	= stUserInfo.ui32ServerIdx;
			stRingInDB._i32SessionIdx	= stUserInfo.i32SessionIdx;
			stRingInDB._ui8Type			= RING_DB_LOAD_MERCENARY_PENALTY_TIME;
			if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorkingUID(stUserInfo.i64UID, &stRingInDB) )
			{
				g_pLog->WriteLog( L"[ERROR] Failed to Load Mercenary Peanalty Time. (UID : %I64d)", stUserInfo.i64UID);
			}
		}
		break;
	case PROTOCOL_CLAN_WAR_MERCENARY_PENALTY_SAVE_REQ:
		{
			RING_IN_DB stRingInDB;
			stRingInDB.Reset();
			DATE32 dtMerPenaltyTime;
			pPacket->ReadData( &dtMerPenaltyTime, sizeof(DATE32) );

			stRingInDB._i64UID			= stUserInfo.i64UID;
			stRingInDB._ui32ServerIdx	= stUserInfo.ui32ServerIdx;
			stRingInDB._i32SessionIdx	= stUserInfo.i32SessionIdx;
			stRingInDB._ui8Type			= RING_DB_UPDATE_MERCENARY_DISCONNECT;
			stRingInDB.Write( &dtMerPenaltyTime, sizeof(DATE32) );

			if( RINGBUFFER_SUCCESS != g_pDataBase->InsertWorkingUID(stUserInfo.i64UID, &stRingInDB) )
			{
				g_pLog->WriteLog( L"[ERROR] Failed to update Mercenary Peanalty Time. (UID : %I64d)", stUserInfo.i64UID);
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
