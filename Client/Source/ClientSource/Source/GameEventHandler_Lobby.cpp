#include "pch.h"
#include "GameEventHandler.h"

#include "Network/GameNetworkManager.h"
#include "./StageBattle/AIModeContext.h"
#include "./StageBattle/UserContext.h"
#include "GameLoginContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "LobbyContext.h"
#include "i3Base/string/ext/utf16_to_mb.h"

const char * CGameEventHandler::__getLobbyEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_CREATE_NICK_NAME : return "EVENT_CREATE_NICK_NAME";
	case EVENT_CREATE_ROOM : return "EVENT_CREATE_ROOM";
	case EVENT_JOIN_ROOM : return "EVENT_JOIN_ROOM";
	case EVENT_JOIN_QUICK : return "EVENT_JOIN_QUICK";
	case EVENT_GET_ROOM_ADDINFO_N : return "EVENT_GET_ROOM_ADDINFO_N";
	case EVENT_ABUSING_SET : return "EVENT_ABUSING_SET";
	case EVENT_REPAIR_EQUIPMENT : return "EVENT_REPAIR_EQUIPMENT";
	case EVENT_LOBBY_REFRESH_N : return "EVENT_LOBBY_REFRESH_N";
	default :					return nullptr;
	}
}

void CGameEventHandler::__register_Lobby( void)
{
	regSendEvent( EVENT_CREATE_NICK_NAME,		&CGameEventHandler::__evtLobby_CreateNickname,		new PACKET_BASE_CREATE_NICK_REQ );
	regSendEvent( EVENT_CREATE_ROOM,			&CGameEventHandler::__evtLobby_CreateRoom);
	regSendEvent( EVENT_JOIN_ROOM,				&CGameEventHandler::__evtLobby_JoinRoom);
	regSendEvent( EVENT_JOIN_QUICK,				&CGameEventHandler::__evtLobby_QuickJoinRoom);
	regSendEvent( EVENT_GET_ROOM_ADDINFO_N,		&CGameEventHandler::__evtLobby_DetailRoomInfo);
	regSendEvent( EVENT_ABUSING_SET,			&CGameEventHandler::__evtLobby_EndAbuse);
	regSendEvent( EVENT_REPAIR_EQUIPMENT,		&CGameEventHandler::__evtLobby_RepairEquipment);
	regSendEvent( EVENT_LOBBY_REFRESH_N,		&CGameEventHandler::__evtLobby_RefreshRoomList);
}

bool CGameEventHandler::__evtLobby_CreateNickname( GAMEEVENTINFO * pInfo)
{
	PACKET_BASE_CREATE_NICK_REQ* pSend = static_cast<PACKET_BASE_CREATE_NICK_REQ*>(pInfo->m_S2MOPacket);

	TTCHAR szNickName[NET_NICK_NAME_SIZE];
	::memset(szNickName, 0, sizeof(szNickName));

	i3::safe_string_copy(szNickName, GameLoginContext::i()->getLoginInfo()->m_wstrUserNick, NET_NICK_NAME_SIZE);

	pSend->m_strNickName = szNickName;
	
	return true;
}

bool CGameEventHandler::__evtLobby_CreateRoom( GAMEEVENTINFO * pInfo)
{
	UserContext::i()->InitRoomData();
	BattleSlotContext::i()->InitRoomData();
	
	PACKET_LOBBY_CREATE_ROOM_REQ RoomData;

	i3mem::FillZero(&RoomData, sizeof(PACKET_LOBBY_CREATE_ROOM_REQ));

	MyRoomInfoContext::i()->getMyRoomInfo( &RoomData.m_RoomInfoBasic);
	MyRoomInfoContext::i()->getMyRoomInfoAdd( &RoomData.m_RoomInfoAdd);
	i3mem::Copy(&RoomData.m_szRoomPw, MyRoomInfoContext::i()->getMyRoomPW(), NET_ROOM_PW);
	RoomData.m_ui8CharMainSlotIdx = (UINT8)CCharaInfoContext::i()->GetMainCharaSlotIdx(TEAM_RED);

	if ( MyRoomInfoContext::i()->getStage()->IsAiMode() )
	{
		RoomData.m_enemyAiCount = (INT8)AIModeContext::i()->NewEnemyCount;
		RoomData.m_difficultyLevel = (INT8)AIModeContext::i()->StartLevel;
	}

	pInfo->_SendPacket.SetProtocol(PROTOCOL_ROOM_CREATE_REQ);
	pInfo->_SendPacket.WriteData(&RoomData, sizeof(PACKET_LOBBY_CREATE_ROOM_REQ));

	return true;
}

bool CGameEventHandler::__evtLobby_JoinRoom( GAMEEVENTINFO * pInfo)
{
	UserContext::i()->InitRoomData();
	BattleSlotContext::i()->InitRoomData();

	PACKET_LOBBY_JOIN_ROOM_REQ RoomData;
	i3mem::FillZero(&RoomData, sizeof(PACKET_LOBBY_JOIN_ROOM_REQ));

	pInfo->_SendPacket.SetProtocol( PROTOCOL_ROOM_JOIN_REQ );

	RoomData.m_ui32RoomIdx = MyRoomInfoContext::i()->getMyRoomIndex();
	i3mem::Copy(&RoomData.m_szRoomPw, MyRoomInfoContext::i()->getMyRoomPW(), NET_ROOM_PW);
	RoomData.m_ui8CharMainSlotIdx = (UINT8)CCharaInfoContext::i()->GetMainCharaSlotIdx();

	// 초대하기/찾아가기 시 비밀번호를 무시한다.
	if (nullptr != pInfo->_pArg)
		RoomData.m_cForce = (UINT8) (*(bool*)pInfo->_pArg);

	pInfo->_SendPacket.WriteData(&RoomData, sizeof(PACKET_LOBBY_JOIN_ROOM_REQ));

	return true;
}

bool CGameEventHandler::__evtLobby_QuickJoinRoom( GAMEEVENTINFO * pInfo)
{
	UserContext::i()->InitRoomData();
	BattleSlotContext::i()->InitRoomData();
	
	pInfo->_SendPacket.SetProtocol(PROTOCOL_LOBBY_QUICKJOIN_ROOM_REQ);
	return true;
}

bool CGameEventHandler::__evtLobby_RefreshRoomList( GAMEEVENTINFO * pInfo)
{
	if( CGameNetworkManager::i()->IsTCPConnected() )
	{
		pInfo->_SendPacket.SetProtocol( PROTOCOL_LOBBY_GET_ROOMLIST_REQ );
		return true;
	}
	return false;
}

bool CGameEventHandler::__evtLobby_DetailRoomInfo( GAMEEVENTINFO * pInfo)
{
	INT32 roomIdx = LobbyContext::i()->getSelectedRoomIdx();
	pInfo->_SendPacket.SetProtocol( PROTOCOL_LOBBY_GET_ROOMINFOADD_REQ );
	pInfo->_SendPacket.WriteData(&roomIdx, sizeof(INT32));
	return true;
}

bool CGameEventHandler::__evtLobby_EndAbuse( GAMEEVENTINFO * pInfo)
{
	//어뷰징 팝업 대기 완료 요청
	pInfo->_SendPacket.SetProtocol(PROTOCOL_LOBBY_ABUSING_POPUP_END_REQ);
	return true;
}

bool CGameEventHandler::__evtLobby_RepairEquipment( GAMEEVENTINFO * pInfo)
{
	UINT8 ui8CharaSlotIdx	= *(UINT8*)(pInfo->_pArg);
	UINT8 ui8RepairType		= (UINT8)S2MO_REPAIR_TYPE_WEAPON;
	T_ItemDBIdx TItemDBIdx  = 0;	// 캐릭터 기준으로 수리하기 때문에 의미 없습니다.

	pInfo->_SendPacket.SetProtocol( PROTOCOL_SHOP_REPAIR_REQ );
	pInfo->_SendPacket.WriteData( &ui8RepairType,	sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( &ui8CharaSlotIdx, sizeof(UINT8) );
	pInfo->_SendPacket.WriteData( &TItemDBIdx,		sizeof(T_ItemDBIdx) );

	return true;
}