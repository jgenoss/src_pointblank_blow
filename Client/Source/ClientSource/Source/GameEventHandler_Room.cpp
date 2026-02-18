#include "pch.h"
#include "GameEventHandler.h"
#include "./UI/UIPopupDomiRound.h"

#include "InviteContext.h"
#include "./StageBattle/AIModeContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"

const char * CGameEventHandler::__getRoomEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_READY_BATTLE : return "EVENT_READY_BATTLE";
	case EVENT_GET_LOBBY_USER : return "EVENT_GET_LOBBY_USER";
	case EVENT_GET_INVITE_LOBBY_USER: return "EVENT_GET_INVITE_LOBBY_USER";
	case EVENT_INVITE_LOBBY_USER : return "EVENT_INVITE_LOBBY_USER";
	case EVENT_SEARCH_HOST : return "EVENT_SEARCH_HOST";
	case EVENT_TRANSFER_HOST : return "EVENT_TRANSFER_HOST";
	case EVENT_REQUEST_HOST_CHANGE : return "EVENT_REQUEST_HOST_CHANGE";
	case EVENT_TEAM_CHANGE : return "EVENT_TEAM_CHANGE";
	case EVENT_CHANGE_ROOM_OPTION_N : return "EVENT_CHANGE_ROOM_OPTION_N";
	case EVENT_CHANGE_ROOMINFO_N : return "EVENT_CHANGE_ROOMINFO_N";
	case EVENT_CHANGE_ROOM_ALLINFO_N : return "EVENT_CHANGE_ROOM_ALLINFO_N";
	case EVENT_CHANGE_ROOM_PASSWORD_N : return "EVENT_CHANGE_ROOM_PASSWORD_N";
	case EVENT_CHANGE_TEAM_N : return "EVENT_CHANGE_TEAM_N";
	case EVENT_CHANGE_OBSERVER_SLOT : return "EVENT_CHANGE_OBSERVER_SLOT";
	case EVENT_CHANGE_HIDDEN_SLOT : return "EVENT_CHANGE_HIDDEN_SLOT";
	case EVENT_CHANGE_SLOT_N : return "EVENT_CHANGE_SLOT_N";
	case EVENT_GET_SLOTINFO_N : return "EVENT_GET_SLOTINFO_N";
	case EVENT_GET_USERINFO_N : return "EVENT_GET_USERINFO_N";
	case EVENT_EQUIPMENT_TEMP_WEAPON_N : return "EVENT_EQUIPMENT_TEMP_WEAPON_N";
	case EVENT_GET_ROOM_SCORE : return "EVENT_GET_ROOM_SCORE";
	case EVENT_GET_USER_EQUIPMENT_INFO : return "EVENT_GET_USER_EQUIPMENT_INFO";
	default :					return nullptr;
	}
}

void CGameEventHandler::__register_Room( void)
{
	regSendEvent( EVENT_READY_BATTLE,				&CGameEventHandler::__evtRoom_Ready);
	regSendEvent( EVENT_GET_LOBBY_USER,				&CGameEventHandler::__evtRoom_GetLobbyUser);
	regSendEvent( EVENT_GET_INVITE_LOBBY_USER,		&CGameEventHandler::__evtRoom_GetInviteLobbyUser);
	regSendEvent( EVENT_INVITE_LOBBY_USER,			&CGameEventHandler::__evtRoom_InviteLobbyUser);
	regSendEvent( EVENT_SEARCH_HOST,				&CGameEventHandler::__evtRoom_SearchHost);
	regSendEvent( EVENT_TRANSFER_HOST,				&CGameEventHandler::__evtRoom_TransferHost);
	regSendEvent( EVENT_REQUEST_HOST_CHANGE,		&CGameEventHandler::__evtRoom_RequestChangeHost);
	regSendEvent( EVENT_TEAM_CHANGE,				&CGameEventHandler::__evtRoom_TeamChange);
	regSendEvent( EVENT_CHANGE_ROOM_OPTION_N,		&CGameEventHandler::__evtRoom_ChangeOption);
	regSendEvent( EVENT_CHANGE_ROOMINFO_N,			&CGameEventHandler::__evtRoom_ChangeInfo);
	regSendEvent( EVENT_CHANGE_ROOM_ALLINFO_N,		&CGameEventHandler::__evtRoom_ChangeAllInfo);
	regSendEvent( EVENT_CHANGE_ROOM_PASSWORD_N,		&CGameEventHandler::__evtRoom_ChangePassword);
	regSendEvent( EVENT_CHANGE_TEAM_N,				&CGameEventHandler::__evtRoom_ChangeTeam);
	regSendEvent( EVENT_CHANGE_OBSERVER_SLOT,		&CGameEventHandler::__evtRoom_ChangeObserverSlot);
	regSendEvent( EVENT_CHANGE_HIDDEN_SLOT,			&CGameEventHandler::__evtRoom_ChangeHiddenSlot);
	regSendEvent( EVENT_CHANGE_SLOT_N,				&CGameEventHandler::__evtRoom_ChagneSlot);
	regSendEvent( EVENT_GET_SLOTINFO_N,				&CGameEventHandler::__evtRoom_GetSlotInfo);
	regSendEvent( EVENT_GET_USERINFO_N,				&CGameEventHandler::__evtRoom_GetUserInfo);
	regSendEvent( EVENT_EQUIPMENT_TEMP_WEAPON_N,	&CGameEventHandler::__evtRoom_EquipmentTempWeapon);
	regSendEvent( EVENT_GET_ROOM_SCORE,				&CGameEventHandler::__evtRoom_GetRoomScore);
	regSendEvent( EVENT_GET_USER_EQUIPMENT_INFO,	&CGameEventHandler::__evtRoom_GetUserEquipment);
}

bool CGameEventHandler::__evtRoom_Ready( GAMEEVENTINFO * pInfo)
{
	PACKET_BATTLE_NEW_READYBATTLE_REQ readyBattle;			

	//1.
	// GM Observer 일경우 true 사용 한다고 함.
	// revision 29500
	readyBattle.m_bIsObserverMode = UserInfoContext::i()->IsGM_Observer();

	//2. 
	//if( CCharaInfoContext::i() )
	//	readyBattle.m_ui8CharaSlotIdx = (UINT8)CCharaInfoContext::i()->GetMainCharaSlotIdx();

	
	
	//3. 
	readyBattle.m_i64StartItemIdx = 0;
	bool is_host = BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx();
	/*if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DOMINATION) && is_host)
	{
		ROOM_INFO_BASIC InfoBasic;
		MyRoomInfoContext::i()->getMyRoomInfo(&InfoBasic);

		dui::RoundItem round = dui::FindDomiRoundItem( InfoBasic._i16BeginRound);
		if( round.item)
		{
			readyBattle.m_i64StartItemIdx = round.item->_TItemWareDBIdx;
		}
	}*/


	pInfo->_SendPacket.SetProtocol( PROTOCOL_BATTLE_READYBATTLE_REQ );
	pInfo->_SendPacket.WriteData( &readyBattle, sizeof(PACKET_BATTLE_NEW_READYBATTLE_REQ) );

	//I3TRACE( "READYBATTLE %d, %d\n", readyBattle.m_bIsObserverMode, readyBattle.m_ui8CharaSlotIdx);
	I3TRACE( "READYBATTLE %d\n", readyBattle.m_bIsObserverMode );

	if( !is_host)
	{
		I3_BOUNDCHK(BattleSlotContext::i()->getMySlotIdx(),SLOT_MAX_COUNT);

		// 서버에 의해 레디버튼을 해제할때를 구별하기위해
		// 수동으로 레디를 했다는 것을 기록해놓는다.
		if (SLOT_STATE_READY != BattleSlotContext::i()->getMySlotInfo()->_State)
		{
			MyRoomInfoContext::i()->SetManualReadyFlag(true);
		}
		else
		{
			MyRoomInfoContext::i()->SetManualReadyFlag(false);
		}
	}

	return true;
}

bool CGameEventHandler::__evtRoom_GetLobbyUser( GAMEEVENTINFO * pInfo)
{
	UINT8 gettype = *(UINT8*)pInfo->_pArg;

	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_GET_LOBBY_USER_LIST_REQ);
	pInfo->_SendPacket.WriteData(&gettype, sizeof(UINT8));
	return true;
}

bool CGameEventHandler::__evtRoom_GetInviteLobbyUser(GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_REQ);
	return true;
}

bool CGameEventHandler::__evtRoom_InviteLobbyUser( GAMEEVENTINFO * pInfo)
{
	UINT32 nCount = InviteContext::i()->GetCheckInviteCount();

	// 1명 이상
	if (0 < nCount)
	{
		pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_INVITE_LOBBY_USER_REQ);
		// 초대 인원
		pInfo->_SendPacket.WriteData(&nCount, sizeof(UINT32));
		for(UINT32 i = 0; i < MAX_INVITE_COUNT; i++)
		{
			// 초대 체크 되어 있으면 세션ID 기록
			if ( *InviteContext::i()->GetInviteUserIdx( i) == true )
			{
				UINT32 idxSession = InviteContext::i()->GetUserInfoInvite(i)->_idxSession;
				pInfo->_SendPacket.WriteData(&idxSession, sizeof(UINT32));
			}
		}

		return true;
	}

	return false;
}

bool CGameEventHandler::__evtRoom_SearchHost( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_CHECK_MAIN_REQ);
	return true;
}

bool CGameEventHandler::__evtRoom_TransferHost( GAMEEVENTINFO * pInfo)
{
	INT32 slotIdx = *(INT32*) pInfo->_pArg;

	// 방장을 넘길 대상 유저를 선택했는가?
	if (slotIdx < 0)
	{
		// 최선의 방장을 찾는다.
		pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_REQ);
	}
	else
	{
		// 대상 유저에게 방장을 넘긴다.
		pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_REQ);
		pInfo->_SendPacket.WriteData(&slotIdx, sizeof(INT32));
	}

	return true;
}

bool CGameEventHandler::__evtRoom_RequestChangeHost( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_REQUEST_MAIN_REQ);
	return true;
}
bool CGameEventHandler::__evtRoom_TeamChange( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_MASTER_TEAM_CHANGE_REQ);
	return true;
}

bool CGameEventHandler::__evtRoom_ChangeOption( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg, sizeof(ROOM_INFO_ADD));
	return true;
}

bool CGameEventHandler::__evtRoom_ChangeInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHANGE_ROOMINFO_REQ );
	pInfo->_SendPacket.WriteData( pInfo->_pArg,	sizeof(ROOM_INFO_BASIC));

	
	INT8 enemyAiCount = (INT8)AIModeContext::i()->NewEnemyCount;
	INT8 difficultyLevel = (INT8)AIModeContext::i()->StartLevel;

	
	pInfo->_SendPacket.WriteData(&enemyAiCount, sizeof(INT8));
	pInfo->_SendPacket.WriteData(&difficultyLevel, sizeof(INT8));
	return true;
}

bool CGameEventHandler::__evtRoom_ChangeAllInfo( GAMEEVENTINFO * pInfo)
{
	ROOM_INFO_ALL *pRoomInfo = (ROOM_INFO_ALL*)pInfo->_pArg;
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_REQ );
	pInfo->_SendPacket.WriteData(&pRoomInfo->_Basic,	sizeof(ROOM_INFO_BASIC));
	pInfo->_SendPacket.WriteData(&pRoomInfo->_Add,		sizeof(ROOM_INFO_ADD));

	
	INT8 enemyAiCount = (INT8)AIModeContext::i()->NewEnemyCount;
	INT8 difficultyLevel = (INT8)AIModeContext::i()->StartLevel;

	
	pInfo->_SendPacket.WriteData(&enemyAiCount, sizeof(INT8));
	pInfo->_SendPacket.WriteData(&difficultyLevel, sizeof(INT8));
	return true;
}

bool CGameEventHandler::__evtRoom_ChangePassword( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHANGE_PASSWD_REQ);
	pInfo->_SendPacket.WriteData( MyRoomInfoContext::i()->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
	return true;
}

bool CGameEventHandler::__evtRoom_ChangeTeam( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_REQ );
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtRoom_ChangeObserverSlot( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHANGE_OBSERVER_SLOT_REQ );
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtRoom_ChangeHiddenSlot( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHANGE_HIDDEN_SLOT_REQ );
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));	
	return true;
}

bool CGameEventHandler::__evtRoom_ChagneSlot( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_CHANGE_SLOT_REQ );
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));			//Open 0x1000**** Close 0x0000****
	return true;
}

bool CGameEventHandler::__evtRoom_GetSlotInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_GET_SLOTINFO_REQ);
	return true;
}

bool CGameEventHandler::__evtRoom_GetUserInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_GET_PLAYERINFO_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtRoom_EquipmentTempWeapon( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_EQUIPMENT_TEMP_WEAPON_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(UINT32));
	return true;
}

bool CGameEventHandler::__evtRoom_GetRoomScore( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_REQ);
	return true;
}

bool CGameEventHandler::__evtRoom_GetUserEquipment( GAMEEVENTINFO * pInfo)
{
	PACKET_ROOM_NEW_GET_USER_EQUIPMENT_REQ info;
	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_GET_USER_EQUIPMENT_REQ);

	INT32 idx = *(INT32*)(pInfo->_pArg);
	I3_BOUNDCHK( idx, SLOT_MAX_COUNT);
	info.m_i8SlotIdx = static_cast<INT8>(idx);
	pInfo->_SendPacket.WriteData( &info, sizeof(info));
	return true;
}