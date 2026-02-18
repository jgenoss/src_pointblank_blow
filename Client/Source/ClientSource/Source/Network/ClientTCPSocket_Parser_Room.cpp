#include "pch.h"
#include "ClientTCPSocket.h"

#include "UI/UIUtil.h"
#include "UI/UIMainFrame.h"

#include "InviteContext.h"
#include "ViewOtherUserItemContext.h"
#include "../StageBattle/AIModeContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleServerContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "ClanGameContext.h"
#include "CommunityContext.h"
#include "LobbyContext.h"

namespace
{
	const char * getProtocolName( PROTOCOL protocol)
	{
		switch( protocol)
		{
		case PROTOCOL_ROOM_CREATE_ACK : return "PROTOCOL_ROOM_CREATE_ACK";
		case PROTOCOL_ROOM_JOIN_ACK : return "PROTOCOL_ROOM_JOIN_ACK";
		case PROTOCOL_ROOM_GET_SLOTINFO_ACK : return "PROTOCOL_ROOM_GET_SLOTINFO_ACK";
		case PROTOCOL_ROOM_GET_SLOTONEINFO_ACK : return "PROTOCOL_ROOM_GET_SLOTONEINFO_ACK";
		case PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK : return "PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK";
		case PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_ACK : return "PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_ACK";
		case PROTOCOL_ROOM_INFO_ENTER_ACK : return "PROTOCOL_ROOM_INFO_ENTER_ACK";
		case PROTOCOL_ROOM_INFO_LEAVE_ACK : return "PROTOCOL_ROOM_INFO_LEAVE_ACK";
		case PROTOCOL_ROOM_CHECK_MAIN_ACK : return "PROTOCOL_ROOM_CHECK_MAIN_ACK";
		case PROTOCOL_ROOM_TEAM_BALANCE_ACK : return "PROTOCOL_ROOM_TEAM_BALANCE_ACK";
		case PROTOCOL_ROOM_UNREADY_4VS4_ACK : return "PROTOCOL_ROOM_UNREADY_4VS4_ACK";
		case PROTOCOL_ROOM_REQUEST_MAIN_ACK : return "PROTOCOL_ROOM_REQUEST_MAIN_ACK";
		case PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK : return "PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK";
		case PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK : return "PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK";
		case PROTOCOL_ROOM_GET_NICKNAME_ACK : return "PROTOCOL_ROOM_GET_NICKNAME_ACK";
		case PROTOCOL_ROOM_GET_RANK_ACK : return "PROTOCOL_ROOM_GET_RANK_ACK";
		case PROTOCOL_ROOM_GET_COLOR_NICK_ACK : return "PROTOCOL_ROOM_GET_COLOR_NICK_ACK";
		case PROTOCOL_ROOM_CHANGE_PASSWD_ACK : return "PROTOCOL_ROOM_CHANGE_PASSWD_ACK";
		case PROTOCOL_ROOM_CHANGE_SLOT_ACK : return "PROTOCOL_ROOM_CHANGE_SLOT_ACK";
		case PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK : return "PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK";
		case PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_ACK: return "PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_ACK";
		case PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_ACK : return "PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_ACK";
		case PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK : return "PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK";
		case PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK : return "PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK";
		case PROTOCOL_ROOM_CHATTING_ACK : return "PROTOCOL_ROOM_CHATTING_ACK";
		case PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK : return "PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK";
		case PROTOCOL_ROOM_CHARA_SHIFT_POS_OTHER_ACK : return "PROTOCOL_ROOM_CHARA_SHIFT_POS_OTHER_ACK";
		case PROTOCOL_ROOM_LOADING_START_ACK : return "PROTOCOL_ROOM_LOADING_START_ACK";
		case PROTOCOL_ROOM_GET_USER_EQUIPMENT_ACK : return "PROTOCOL_ROOM_GET_USER_EQUIPMENT_ACK";
		case PROTOCOL_ROOM_GET_USER_ITEM_ACK : return "PROTOCOL_ROOM_GET_USER_ITEM_ACK";
		case PROTOCOL_ROOM_NEW_DOMI_JOIN_US_ACK : return "PROTOCOL_ROOM_NEW_DOMI_JOIN_US_ACK";
		case PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_ACK: return "PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_ACK";
		case PROTOCOL_ROOM_GET_PLAYERINFO_ACK:	return "PROTOCOL_ROOM_GET_PLAYERINFO_ACK";
		default :
			I3PRINTLOG(I3LOG_FATAL,  "PROTOCOL_ROOM_XXX 프로토콜 이름 등록해주이소~");
			return "PROTOCOL_ROOM_UNKNOWN";
		}
	}
}


void ClientTCPSocket::_PacketParsingRoom(i3NetworkPacket  * pPacket)
{
	i3::unordered_map< UINT32, netFunc >::iterator it;
	it = m_NetFuncMap_Room.find( pPacket->GetProtocolID());

	if ( it != m_NetFuncMap_Room.end() )
	{
		I3PRINTLOG(I3LOG_QA, "* %s -->", getProtocolName( pPacket->GetProtocolID()) );
		netFunc & mf = it->second;
		(this->*mf)(pPacket);
		I3PRINTLOG(I3LOG_QA, "* %s <--", getProtocolName( pPacket->GetProtocolID()) );
	}
	else
		__Parse_UnknownProtocol( pPacket);
}

void ClientTCPSocket::__Register_Room_Function( void)
{
	RegisterParser( PROTOCOL_ROOM_CREATE_ACK,					&ClientTCPSocket::__Parse_Lobby_CreateRoom);
	RegisterParser( PROTOCOL_ROOM_JOIN_ACK,						&ClientTCPSocket::__Parse_Lobby_JoinRoom);
	RegisterParser( PROTOCOL_ROOM_GET_SLOTINFO_ACK,				&ClientTCPSocket::__Parse_Room_ReceiveAllUserSlotInfo);
	RegisterParser( PROTOCOL_ROOM_GET_SLOTONEINFO_ACK,			&ClientTCPSocket::__Parse_Room_ReceiveUserSlotInfo);
	RegisterParser( PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK,			&ClientTCPSocket::__Parse_Room_ChangeRoomInfo);
	RegisterParser( PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_ACK,	&ClientTCPSocket::__Parse_Room_ChangeRoomOption);
	RegisterParser( PROTOCOL_ROOM_INFO_ENTER_ACK,				&ClientTCPSocket::__Parse_Room_Enter);
	RegisterParser( PROTOCOL_ROOM_INFO_LEAVE_ACK,				&ClientTCPSocket::__Parse_Room_Leave);
	RegisterParser( PROTOCOL_ROOM_CHECK_MAIN_ACK,				&ClientTCPSocket::__Parse_Room_SearchHost);
	RegisterParser( PROTOCOL_ROOM_TEAM_BALANCE_ACK,				&ClientTCPSocket::__Parse_Room_TeamBalance);
	RegisterParser( PROTOCOL_ROOM_UNREADY_4VS4_ACK,				&ClientTCPSocket::__Parse_Room_Unready4vs4);
	RegisterParser( PROTOCOL_ROOM_REQUEST_MAIN_ACK,				&ClientTCPSocket::__Parse_Room_RequestChangeHost);
	RegisterParser( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK,		&ClientTCPSocket::__Parse_Room_ChangeHost);
	RegisterParser( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK,	&ClientTCPSocket::__Parse_Room_ChangeHost);
	RegisterParser( PROTOCOL_ROOM_GET_NICKNAME_ACK,				&ClientTCPSocket::__Parse_Room_ReceiveNickname);
	RegisterParser( PROTOCOL_ROOM_GET_RANK_ACK,					&ClientTCPSocket::__Parse_Room_ReceiveRank);
	RegisterParser( PROTOCOL_ROOM_GET_COLOR_NICK_ACK,			&ClientTCPSocket::__Parse_Room_ReceiveNicknameColor);
	RegisterParser( PROTOCOL_ROOM_CHANGE_PASSWD_ACK,			&ClientTCPSocket::__Parse_Room_ChangePassword);
	RegisterParser( PROTOCOL_ROOM_CHANGE_SLOT_ACK,				&ClientTCPSocket::__Parse_Room_ChangeSlot);
	RegisterParser( PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK,		&ClientTCPSocket::__Parse_Room_ReceiveLobbyUserList);
	RegisterParser( PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_ACK,	&ClientTCPSocket::__Parse_Room_ReceiveInviteUserList);
	RegisterParser( PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_ACK,		&ClientTCPSocket::__Parse_Room_ChangeTeam);
	RegisterParser( PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_ACK,		&ClientTCPSocket::__Parse_Room_ChangeObserverSlot);
	RegisterParser( PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_ACK,		&ClientTCPSocket::__Parse_Room_ChangeHiddenSlot);
	RegisterParser( PROTOCOL_ROOM_CHATTING_ACK,					&ClientTCPSocket::__Parse_Room_Chat);
	RegisterParser( PROTOCOL_ROOM_CHARA_SHIFT_POS_ACK,			&ClientTCPSocket::__Parse_Room_SelectMainCharacter);
	RegisterParser( PROTOCOL_ROOM_CHARA_SHIFT_POS_OTHER_ACK,	&ClientTCPSocket::__Parse_Room_SelectOtherUserChara);
	RegisterParser( PROTOCOL_ROOM_LOADING_START_ACK,			&ClientTCPSocket::__Parse_Room_LoadingStart);
	RegisterParser( PROTOCOL_ROOM_GET_USER_EQUIPMENT_ACK,		&ClientTCPSocket::__Parse_Room_GetUserEquipmentInfo);
	RegisterParser( PROTOCOL_ROOM_GET_USER_ITEM_ACK,			&ClientTCPSocket::__Parse_Room_GetUserItem);
	RegisterParser( PROTOCOL_ROOM_NEW_DOMI_JOIN_US_ACK,			&ClientTCPSocket::__Parse_Room_DomiEnter);
	RegisterParser(PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_ACK,	&ClientTCPSocket::__Parse_Room_GetColorMuzzleFlash);
	RegisterParser( PROTOCOL_ROOM_GET_PLAYERINFO_ACK,			&ClientTCPSocket::__Parse_Room_UserInfo);
}

void ClientTCPSocket::__Parse_Room_ReceiveAllUserSlotInfo( i3NetworkPacket * pPacket)
{
	//PACKET_ROOM_GET_SLOTINFO_ACK Recv;
	//S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	//BattleSlotContext::i()->setMainSlotIdx( Recv.m_ui8MainSlotIdx );

	UINT8	ui8MainSlotIdx = 0;
	pPacket->ReadData(&ui8MainSlotIdx, sizeof(UINT8));
	BattleSlotContext::i()->setMainSlotIdx(ui8MainSlotIdx);

	UINT16	ui16Size = 0;
	for (UINT8 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		pPacket->ReadData(&ui16Size, sizeof(UINT16));
		pPacket->ReadData(BattleSlotContext::i()->setSlotInfo(i), ui16Size);
		
		pPacket->ReadData(&ui16Size, sizeof(UINT16));
		pPacket->ReadData(BattleSlotContext::i()->setSlotInfo(i)->_ClanName, ui16Size);

		pPacket->ReadData(&BattleSlotContext::i()->setSlotInfo(i)->_bIsGmObserverMode, sizeof(bool));
		pPacket->ReadData(&BattleSlotContext::i()->setSlotInfo(i)->_ui8NationalCode, sizeof(UINT8));
	}

	//i3mem::Copy( BattleSlotContext::i()->setSlotInfo(0), &Recv.m_aSlotInfo, sizeof(SLOT_INFO) * USER_SLOT_COUNT);
	BattleSlotContext::i()->SetUseCashItem(0);

	INT32 oldMainSlotIdx = BattleSlotContext::i()->getMainSlotIdx();

	// 일단 false로 설정
	MyRoomInfoContext::i()->SetTeamChangeFlag(false);

	// 진영변경으로 잠시 자신이 방장슬롯에 남아 있을 수있어 확인해야한다.
	if (BattleSlotContext::i()->GetMySlotIdxEx() == ui8MainSlotIdx)
	{
		const SLOT_INFO* oldMainSlotInfo = BattleSlotContext::i()->getSlotInfo(oldMainSlotIdx);	

		if (ui8MainSlotIdx != oldMainSlotIdx && SLOT_STATE_EMPTY != oldMainSlotInfo->_State)
		{
			// 자신과 방장의 위치가 변경되었다.
			MyRoomInfoContext::i()->SetTeamChangeFlag(true);
		}
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_RECEIVE_USER_SLOT, ui8MainSlotIdx);
}


void ClientTCPSocket::__Parse_Room_ReceiveUserSlotInfo( i3NetworkPacket * pPacket)
{
	PACKET_ROOM_GET_SLOTONEINFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	S2_SLOT_TOTAL_INFO* pSlotInfo = &Recv.m_SlotTotalInfo;

	INT32	i32Slot = pSlotInfo->m_ui8SlotIdx;
	i3mem::Copy( BattleSlotContext::i()->setSlotInfo(i32Slot), &Recv.m_SlotTotalInfo, sizeof(SLOT_INFO) );
	BattleSlotContext::i()->SetUseCashItem(i32Slot);

	BattleSlotContext::i()->setNickForSlot(i32Slot, pSlotInfo->m_strNickName );		// 오버로딩 변환되므로 괜찮다고 간주하고 방치한다.(2014.08.22.수빈)
	BattleSlotContext::i()->setFireMuzzleColorForSlot(i32Slot, pSlotInfo->m_ui16MuzzleFlashColor);
	ClanGameContext::i()->SetMercRank(i32Slot, pSlotInfo->m_ui8MerClass, pSlotInfo->m_ui8MerRank); 


	BattleSlotContext::i()->setNickColorForSlot(i32Slot, pSlotInfo->m_ui8NickColor);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_RECEIVE_USER_SLOT, i32Slot);

	// keyword : Star Player
	MyRoomInfoContext::i()->PrintStarPlayerEntranceMsg(i32Slot);
}

void ClientTCPSocket::__Parse_Room_ChangeRoomInfo( i3NetworkPacket * pPacket)
{
	STAGE_MODE curGameMode;
	STAGE_MODE newGameMode;

	// 모드 변경 확인을 위해 현재 게임모드를 기억해놓는다.
	ROOM_INFO_BASIC	oldRoomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&oldRoomInfo);

	curGameMode = StageID::GetStageMode( oldRoomInfo._StageID);
	UINT8 cCurWeapon = oldRoomInfo._WeaponFlag;

	// 새 룸 정보 받기
	ROOM_INFO_BASIC	roomInfo;
	ROOM_INFO_ADD	roomAddInfo;

	pPacket->ReadData(&roomInfo,		sizeof(ROOM_INFO_BASIC));
	pPacket->ReadData(&roomAddInfo,		sizeof(ROOM_INFO_ADD));

	MyRoomInfoContext::i()->setMyRoomInfo(&roomInfo);
	MyRoomInfoContext::i()->setMyRoomInfoAdd(&roomAddInfo);

	if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
	{
		INT8 enemyAiCount = 0;
		INT8 difficultyLevel = 0;

		pPacket->ReadData(&enemyAiCount, sizeof(INT8));
		pPacket->ReadData(&difficultyLevel, sizeof(INT8));

		AIModeContext::i()->EnemyCount = enemyAiCount;
		AIModeContext::i()->StartLevel = difficultyLevel;
	}

	// 새 게임모드 확인
	newGameMode = StageID::GetStageMode( roomInfo._StageID);
	UINT8 cNewWeapon = roomInfo._WeaponFlag;

	// 게임모드가 변경되었으면 준비상태 해제 메시지를 출력한다.
	if (curGameMode != newGameMode )
	{
		if (MyRoomInfoContext::i()->IsManualReady())
		{
			// 배틀에서 돌아왔을 경우에만 팝업을 출력한다.
			if( g_pFramework->IsUIStage())
			{
				/*방의 게임모드가 변경되어\n준비완료 상태를 해제합니다.*/
				GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_NETWORK_CHANGE_MODE_TEXT"));
			}
			
			MyRoomInfoContext::i()->SetManualReadyFlag(false);
		}

	}

	if( cCurWeapon != cNewWeapon )
	{
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			if( BattleSlotContext::i()->getSlotInfo(i)->_State == SLOT_STATE_READY )
				BattleSlotContext::i()->setSlotState( i, SLOT_STATE_NORMAL);
		}

		//상태가 변경되었습니다.
		//옵션버튼 깜빡.
		//채팅창에 "방의 고급옵션이 변경되었습니다."을 출력합니다.
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CHANGE_ROOM_OPTION_N);

		if (MyRoomInfoContext::i()->IsManualReady())
		{
			MyRoomInfoContext::i()->SetManualReadyFlag(false);
		}
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CHANGE_READYROOM_INFO);
}

void ClientTCPSocket::__Parse_Room_ChangeRoomOption( i3NetworkPacket * pPacket)
{
	ROOM_INFO_ADD	roomAddInfo;
	pPacket->ReadData( &roomAddInfo, sizeof(ROOM_INFO_ADD));
	MyRoomInfoContext::i()->setMyRoomInfoAdd(&roomAddInfo);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CHANGE_READYROOM_INFO);
}

void ClientTCPSocket::__Parse_Room_Enter( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_ENTER_PROFILE);
}

void ClientTCPSocket::__Parse_Room_Leave( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_LEAVE_PROFILE);
}

void ClientTCPSocket::__Parse_Room_SearchHost( i3NetworkPacket * pPacket)
{
	INT32 slotIdx;

	pPacket->ReadData(&slotIdx, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_SEARCH_HOST, slotIdx);
}

void ClientTCPSocket::__Parse_Room_TeamBalance( i3NetworkPacket * pPacket)
{
	UINT8	ui8MoveReason;
	UINT8	ui8MainSlot;
	UINT8	ui8BalancedCount;

	pPacket->ReadData( &ui8MoveReason,		sizeof(UINT8) );
	pPacket->ReadData( &ui8MainSlot,		sizeof(UINT8) );
	pPacket->ReadData( &ui8BalancedCount,	sizeof(UINT8) );

	for(INT32 i = 0; i < ui8BalancedCount; i++)
	{
		ROOM_SLOT_CHANGE_INFO	ChangeInfo;				

		pPacket->ReadData( &ChangeInfo, sizeof(ROOM_SLOT_CHANGE_INFO) );

		// SlotInfo 이동
		SLOT_INFO TempSlotInfo;
		i3mem::Copy( &TempSlotInfo, BattleSlotContext::i()->getSlotInfo(ChangeInfo.ui8NewSlot), sizeof(SLOT_INFO));
		i3mem::Copy( BattleSlotContext::i()->setSlotInfo(ChangeInfo.ui8NewSlot), BattleSlotContext::i()->getSlotInfo(ChangeInfo.ui8OldSlot), sizeof(SLOT_INFO));
		i3mem::Copy( BattleSlotContext::i()->setSlotInfo(ChangeInfo.ui8OldSlot), &TempSlotInfo, sizeof(SLOT_INFO));
		BattleSlotContext::i()->SetUseCashItem(ChangeInfo.ui8NewSlot);
		BattleSlotContext::i()->SetUseCashItem(ChangeInfo.ui8OldSlot);

		// Nickname 이동
		i3::rc_wstring wstrTempNick = BattleSlotContext::i()->getNickForSlot(ChangeInfo.ui8NewSlot);
		BattleSlotContext::i()->setNickForSlot(ChangeInfo.ui8NewSlot, BattleSlotContext::i()->getNickForSlot(ChangeInfo.ui8OldSlot) );
		BattleSlotContext::i()->setNickForSlot(ChangeInfo.ui8OldSlot, wstrTempNick);

		// Nickname Color 이동
		UINT8 ui8TempNickColor;
		ui8TempNickColor = BattleSlotContext::i()->getNickColorForSlot( ChangeInfo.ui8NewSlot );
		BattleSlotContext::i()->setNickColorForSlot( ChangeInfo.ui8NewSlot, BattleSlotContext::i()->getNickColorForSlot(ChangeInfo.ui8OldSlot) );
		BattleSlotContext::i()->setNickColorForSlot( ChangeInfo.ui8OldSlot, ui8TempNickColor );

		ClanGameContext::i()->ChangeMercRank(ChangeInfo.ui8OldSlot, ChangeInfo.ui8NewSlot);

		// State 이동
		BattleSlotContext::i()->setSlotState( ChangeInfo.ui8OldSlot, (SLOT_STATE) ChangeInfo.ui8OldSlotState);
		BattleSlotContext::i()->setSlotState( ChangeInfo.ui8NewSlot, (SLOT_STATE) ChangeInfo.ui8NewSlotState);

		// 자신 SlotIdx 변경
		if (ChangeInfo.ui8OldSlot == BattleSlotContext::i()->getMySlotIdx())
		{
			BattleSlotContext::i()->setMySlotIdx(ChangeInfo.ui8NewSlot);
			CCharaInfoContext::i()->SetTeamSlotBySlotIdx( ChangeInfo.ui8NewSlot );
		}
		else if (ChangeInfo.ui8NewSlot == BattleSlotContext::i()->getMySlotIdx())
		{
			BattleSlotContext::i()->setMySlotIdx(ChangeInfo.ui8OldSlot);
			CCharaInfoContext::i()->SetTeamSlotBySlotIdx(ChangeInfo.ui8OldSlot);
		}
	}
	// 방장 SlotIdx 변경
	BattleSlotContext::i()->setMainSlotIdx( ui8MainSlot );

	
	switch( ui8MoveReason )
	{
	case 1:
		MyRoomInfoContext::i()->SetTeamBalanceMessageFlag(true);
		break;
	case 2:
		// 팀전체 이동
		MyRoomInfoContext::i()->SetTeamChangeByHostFlag(true);
		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_NETWORK_CHANGE_TEAM_TEXT"));/*방장 권한으로 레드팀과 블루팀이 서로 교체되었습니다.*/
		break;
	}

	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_TEAM_BALANCE, ui8MoveReason);
}


void ClientTCPSocket::__Parse_Room_Unready4vs4( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_UNREADY_4VS4_N);
}

void ClientTCPSocket::__Parse_Room_RequestChangeHost( i3NetworkPacket * pPacket)
{
	INT32 slotIdx;

	pPacket->ReadData(&slotIdx, sizeof(INT32));
	
	// 요청자 정보를 전달한다.
	//if (0 <= slotIdx && SLOT_MAX_COUNT >= slotIdx)
	if (0 <= slotIdx && slotIdx < SLOT_MAX_COUNT)
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_REQUEST_HOST_CHANGE, slotIdx);
	}
}

void ClientTCPSocket::__Parse_Room_ChangeHost( i3NetworkPacket * pPacket)
{
	INT32 slotIdx;
			
	pPacket->ReadData(&slotIdx, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_TRANSFER_HOST, slotIdx);
}

void ClientTCPSocket::__Parse_Room_ReceiveNickname( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	pPacket->ReadData( &Temp, sizeof(INT32)); 
	if( Temp > -1)
	{
		TTCHAR slotNick[NET_NICK_NAME_SIZE];
		UINT8 ui8NickColor;
		
		pPacket->ReadData(slotNick, NET_NICK_NAME_SIZE);
		pPacket->ReadData(&ui8NickColor, sizeof(UINT8));

		BattleSlotContext::i()->setNickForSlot(Temp, slotNick);
		BattleSlotContext::i()->setNickColorForSlot( Temp, ui8NickColor );
	}
}

void ClientTCPSocket::__Parse_Room_ReceiveRank( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData( &Temp, sizeof(INT32)); 
	if( Temp > -1)
	{
		UINT32 wRank;
		pPacket->ReadData( &wRank, sizeof(UINT32) );
	}
}

void ClientTCPSocket::__Parse_Room_ReceiveNicknameColor( i3NetworkPacket * pPacket)
{
	INT32 slotNum;
	pPacket->ReadData( &slotNum, sizeof(INT32)); 
	if( 0 <= slotNum && slotNum < SLOT_MAX_COUNT)
	{
		UINT8 cColor;
		pPacket->ReadData( &cColor, sizeof( UINT8 ));

		BattleSlotContext::i()->setNickColorForSlot(slotNum, cColor);
	}
}


void ClientTCPSocket::__Parse_Room_ChangePassword( i3NetworkPacket * pPacket)
{
	char pw[NET_ROOM_PW];
	pPacket->ReadData( pw, sizeof(char) * NET_ROOM_PW);
	
	MyRoomInfoContext::i()->SetMyRoomPW(pw, NET_ROOM_PW);


	char strBuf[NET_ROOM_PW + 1];
	i3::string_ncopy_nullpad(strBuf, MyRoomInfoContext::i()->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

	ROOM_INFO_BASIC	roomInfo;
	MyRoomInfoContext::i()->getMyRoomInfo(&roomInfo);
	if( i3::generic_string_size(strBuf) == 0)
	{
		roomInfo._InfoFlag = SET_ROOM_INFO_PRIVATE_MODE( roomInfo._InfoFlag, 0);
	}
	else
	{
		roomInfo._InfoFlag = SET_ROOM_INFO_PRIVATE_MODE( roomInfo._InfoFlag, 1);
	}
	MyRoomInfoContext::i()->setMyRoomInfo(&roomInfo);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_CHANGE_READYROOM_INFO);
}

void ClientTCPSocket::__Parse_Room_ChangeSlot( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	pPacket->ReadData( &Temp, sizeof(INT32));
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CHANGE_SLOT_N,Temp);
}

void ClientTCPSocket::__Parse_Room_ReceiveInviteUserList( i3NetworkPacket * pPacket)
{
	INT32 Temp;

	InviteContext::i()->ClearInviteCheck();

	pPacket->ReadData(&Temp, sizeof(INT32));
	
	InviteContext::i()->SetUserInfoInviteCount(Temp);

	// 1 이상.
	if (0 < Temp)
	{
		for(INT32 i = 0; i < Temp; i++)
		{
			USER_INFO_INVITE pInfo;

			pPacket->ReadData(&(pInfo._idxSession), sizeof(UINT32));
			pPacket->ReadData(&(pInfo._rank), sizeof(UINT32));
			
			UINT8 nNickLen;
			pPacket->ReadData(&nNickLen, sizeof(UINT8));
			I3ASSERT( nNickLen < NET_NICK_NAME_SIZE);
			
			TTCHAR szNick[NET_NICK_NAME_SIZE];	memset(szNick, 0, sizeof(szNick) );
			pPacket->ReadData(szNick, nNickLen * sizeof(TTCHAR) );

			pInfo._wstr_nick = szNick;

			// 컬러닉네임
			UINT8 ui8NickColor;
			pPacket->ReadData(&ui8NickColor,  sizeof(UINT8) );

			InviteContext::i()->SetUserInfoInvite(i,pInfo);
		}
	}
	else
	{
		Temp = EVENT_ERROR_FAIL;
	}

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_INVITE_LOBBY_USER, Temp);
}

void ClientTCPSocket::__Parse_Room_ReceiveLobbyUserList(i3NetworkPacket * pPacket)
{
	// Lobby UserList
	//예외처리를 해야한다. 
	INT32 Temp;
	INT32 PageNum;

	pPacket->ReadData(&Temp, sizeof(INT32));
	LobbyContext::i()->SetLobbyUserCount(Temp);

	pPacket->ReadData(&PageNum, sizeof(INT32));
	I3ASSERT(PageNum < MAXIMUM_USER_COUNT / 10);
	pPacket->ReadData(&Temp, sizeof(INT32));
	if (Temp > 0)
	{
		I3ASSERT(Temp < MAXIMUM_USER_COUNT / 10);
		LOBBY_USER_INFO_LIST * pUserInfoList = (LOBBY_USER_INFO_LIST*)i3MemAlloc(sizeof(LOBBY_USER_INFO_LIST)*Temp);
		pPacket->ReadData(pUserInfoList, sizeof(LOBBY_USER_INFO_LIST)*Temp);
		LobbyContext::i()->SetLobbyUserInfo(PageNum, Temp, pUserInfoList);
		i3MemFree(pUserInfoList);
	}
	
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_UPDATE_USER_LIST);
}

void ClientTCPSocket::__Parse_Room_ChangeTeam( i3NetworkPacket * pPacket)
{
	INT32	nMySlotIdx;		// 이동한 슬롯번호
	UINT8	doTeamBalance;	// 팀밸런스 옵션에 의해 이동하였는가?

	pPacket->ReadData(&nMySlotIdx, sizeof(INT32));
	pPacket->ReadData(&doTeamBalance, sizeof(UINT8));
	
	BattleSlotContext::i()->setMySlotIdx( nMySlotIdx );
	CCharaInfoContext::i()->SetTeamSlotBySlotIdx(nMySlotIdx);
	MyRoomInfoContext::i()->SetTeamChangeFlag(false);

	// 팀밸런스 옵션에 의한 이동
	if (1 == doTeamBalance)
	{
		MyRoomInfoContext::i()->SetTeamBalanceMessageFlag(true);
	}
	else if (2 == doTeamBalance)
	{
		// 팀전체 이동

		g_pFramework->getChatBox()->PutSystemChatting(GAME_RCSTRING("STR_TBL_NETWORK_CHANGE_TEAM_TEXT"));/*방장 권한으로 레드팀과 블루팀이 서로 교체되었습니다.*/
	}
	
	GameEventReceiver::i()->PushReceivedGameEvent( EVENT_TEAM_CHANGE, nMySlotIdx);
}

void ClientTCPSocket::__Parse_Room_ChangeObserverSlot( i3NetworkPacket * pPacket)
{
	INT32	nMySlotIdx;		// 이동한 슬롯번호
			
	pPacket->ReadData(&nMySlotIdx, sizeof(INT32));			

	BattleSlotContext::i()->setMySlotIdx( nMySlotIdx );
	CCharaInfoContext::i()->SetTeamSlotBySlotIdx(nMySlotIdx);

}

void ClientTCPSocket::__Parse_Room_ChangeHiddenSlot( i3NetworkPacket * pPacket)
{
	INT32	nMySlotIdx;		// 이동한 슬롯번호
			
	pPacket->ReadData(&nMySlotIdx, sizeof(INT32));
	
	BattleSlotContext::i()->setMySlotIdx( nMySlotIdx );
	CCharaInfoContext::i()->SetTeamSlotBySlotIdx(nMySlotIdx);

}

void ClientTCPSocket::__Parse_Room_Chat( i3NetworkPacket * pPacket)
{
	INT32 Temp;
	INT32 SlotIdx;
	UINT16 iType;
	TTCHAR  pString[MAX_STRING_COUNT];
	bool bGameMaster;
	::memset(pString, 0, sizeof(TTCHAR) * MAX_STRING_COUNT);

	pPacket->ReadData( &iType,		sizeof(UINT16) );
	pPacket->ReadData( &SlotIdx,	sizeof(INT32) );
	pPacket->ReadData( &bGameMaster, sizeof(bool));	// 20140924 김경주 GM FLAG
	pPacket->ReadData( &Temp,		sizeof(INT32) );
	if( Temp > 0)
	{
		I3ASSERT( Temp < MAX_STRING_COUNT);
		pPacket->ReadData( pString,		Temp * sizeof(TTCHAR) );

		i3::wstring wstrChat = pString;

		g_pFramework->getChatBox()->PutUserChatting( 
			BattleSlotContext::i()->getNickForSlot(SlotIdx), wstrChat, bGameMaster, iType,
			BattleSlotContext::i()->getNickColorForSlot(SlotIdx) ); 
	}
}

void ClientTCPSocket::__Parse_Room_SelectMainCharacter( i3NetworkPacket * pPacket)
{
	PACKET_ROOM_CHARA_SHIFT_POS_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED( Recv.m_TResult ) )
	{
		I3_BOUNDCHK( Recv.m_ui8SlotIdx[TEAM_RED], S2MO_MULTI_SLOT_COUNT); 

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_CHANGE_MAINCHARA_FROM_ROOM, (INT32) Recv.m_ui8SlotIdx[TEAM_RED] );
	}
}

void ClientTCPSocket::__Parse_Room_SelectOtherUserChara( i3NetworkPacket * ppacket)
{
}

void ClientTCPSocket::__Parse_Room_LoadingStart(i3NetworkPacket * pPacket)
{
	UINT32 StageID;
	pPacket->ReadData(&StageID, sizeof(UINT32));
}

void ClientTCPSocket::__Parse_Room_GetUserEquipmentInfo( i3NetworkPacket * pPacket)
{
	PACKET_ROOM_GET_USER_EQUIPMENT_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	if( EV_SUCCESSED( Recv.m_TResult ) )
	{
		I3ASSERT( Recv.m_ui8SlotIdx < SLOT_MAX_COUNT);
		UI_CHARA_INFO * pUIInfo = CCommunityContext::i()->SetAnotherUserInfo( Recv.m_ui8SlotIdx );
		pUIInfo->Reset();
	
		// 상대방의 파츠 정보가 없다!!!!!!
		I3ASSERT( Recv.m_TCharItemID[TEAM_RED] != 0);
		CHARA_PARTS_INFO_OTHER PartsInfoOther;
		
		for( int team = 0 ; team < TEAM_COUNT ; team++ )
		{
			int temp = g_pEnvSet->IsV2Version() ? TEAM_RED : team;

			PartsInfoOther.SetItemID( CHAR_EQUIPMENT_PARTS_CHARA, Recv.m_TCharItemID[ temp ] );

			for( INT32 idx = 1 ; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx )
			{
				PartsInfoOther.SetItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx), Recv.m_TBattleEquipParts[ idx ] );
			}
			
			pUIInfo->setCharaParts( &PartsInfoOther, team );
		}
		
		// vv3 - check
		for( INT32 idx = 0; idx < CHAR_EQUIPMENT_WEAPON_COUNT ; ++idx )
		{
			pUIInfo->setWeaponItemID( static_cast<WEAPON_SLOT_TYPE>(idx), Recv.m_TBattleEquipWeapon[ idx ], -1, __FILE__, __LINE__);
		}

		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GET_USER_EQUIPMENT_INFO, Recv.m_ui8SlotIdx );
	}
}

void ClientTCPSocket::__Parse_Room_GetUserItem( i3NetworkPacket * pPacket)
{
	PACKET_ROOM_GET_USER_ITEM_ACK Recv;
	S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

	S2MO_CHAR_EQUIP_INFO	CharEquipInfo = Recv.m_CharEquipInfo;

	i3::vector<T_ItemID>	OtherUserItemList;

	// vv3 - check
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_COUNT; ++i )
	{
		T_ItemID TEquipItemID = CharEquipInfo.m_aCharEquipment[i].m_TItemID;
		if ( TEquipItemID <= 0) continue;							// 
		if ( TEquipItemID > DEFAULT_PARTS_ITEM_ID_MIN ) continue;	// 기본 파츠라면 표시하지 않습니다.

		OtherUserItemList.push_back(TEquipItemID);		// 다른 유저 무기 정보
	}
	
	//아이템 정보
	INT32 i32ItemCount = Recv.m_aTItemID.GetCount();
	T_ItemID	aTItemID[MAX_INVEN_COUNT];
	Recv.m_aTItemID.GetValue(aTItemID, i32ItemCount);
	for( INT32 i = 0 ; i < i32ItemCount; i++ )
	{
		ShopItemInfo * pInfo = g_pShopItemInfoDataBase->getInfo(aTItemID[i]);
		if( pInfo != nullptr && pInfo->IsViewOtherUser() )
			OtherUserItemList.push_back(aTItemID[i]);
	}

	ViewOtherUserItemContext::i()->GetData()->m_OtherUserItemIdList.clear();
//	ViewOtherUserItemContext::i()->SetOtherUserItemInfoList(OtherUserItemList);
	
	ViewOtherUserItemContext::i()->GetData()->m_OtherUserItemIdList.swap(OtherUserItemList);

	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_VIEW_OTHER_USER_ITEM_INFO);
}

void ClientTCPSocket::__Parse_Room_GetColorMuzzleFlash(i3NetworkPacket *pPacket)
{
	INT32 i32SlotIdx = 0;
	UINT16 ui16MuzzleFlashColor = 0;

	pPacket->ReadData(&i32SlotIdx, sizeof(INT32));
	pPacket->ReadData(&ui16MuzzleFlashColor, sizeof(UINT16));

	if (i32SlotIdx != EVENT_ERROR_FAIL)
		BattleSlotContext::i()->setFireMuzzleColorForSlot(i32SlotIdx, ui16MuzzleFlashColor);
}

void ClientTCPSocket::__Parse_Room_DomiEnter( i3NetworkPacket * pPacket)
{
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_DOMI_ENTER);
}

void ClientTCPSocket::__Parse_Room_UserInfo(i3NetworkPacket * pPacket)
{
	PACKET_ROOM_GET_PLAYERINFO_ACK Recv;
	S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

	// 우선은 두 개 밖에 안씁니다.
	if( EV_SUCCESSED( Recv.m_TResult ) )
	{	
		CCommunityContext::i()->SetRoomFindUserBasicInfo(&Recv.m_UserBasicInfo);
		CCommunityContext::i()->SetRoomFindUserRecord(&Recv.m_UserRecordInfo);

		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_GET_USERINFO_N);
	}
}