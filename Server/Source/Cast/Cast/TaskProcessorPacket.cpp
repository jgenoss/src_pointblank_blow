#include "pch.h"
#include "TaskProcessor.h"

BOOL CTaskProcessor::_TaskProcessorPacket()
{
	BOOL bWorking = FALSE;
	INT32 i32ServerIdx;
	i3NetworkPacket Packet;

	for( INT32 i = 0 ; i < m_pSessionManager->GetBufferCount() ; i++ )
	{
		if( FALSE == m_pSessionManager->GetBuffer( i, i32ServerIdx, &Packet ) )	continue;

		bWorking = TRUE;
		_SesseionPacketParsing( i32ServerIdx, &Packet );
	}

	return bWorking;
}

BOOL CTaskProcessor::_SesseionPacketParsing( INT32 i32ServerIdx, i3NetworkPacket* pPacket )
{
	switch( pPacket->GetProtocolID() )
	{
	case PROTOCOL_BATTLE_SERVER_CREATE_REQ:
		{
			P_BATTLE_ROOM_CREATE_REQ ReqPacket;
			pPacket->ReadData( &ReqPacket,	sizeof(P_BATTLE_ROOM_CREATE_REQ) );
			STAGE_UID eStageUID = StageID::GetStageUID( ReqPacket._iStage );
			//Set GameServer Info
			ReqPacket._RoomInfo._cGame	= (UINT8)i32ServerIdx;
			INT32 i32BattleIdx			= g_pConfig->GetEmptyBattleServer( eStageUID );

			if( -1 == i32BattleIdx )
			{
				g_pLog->WriteLog(L"[FAIL] Battle Create / Battle:%d / Game:%d / Channel:%d / Room:%d ", ReqPacket._RoomInfo._cBattle, ReqPacket._RoomInfo._cGame, ReqPacket._RoomInfo._cChannel, ReqPacket._RoomInfo._sRoom);

				INT32 i32Result			= EVENT_ERROR_FAIL;

				i3NetworkPacket SendPacket;
				SendPacket.SetProtocol( PROTOCOL_BATTLE_SERVER_CREATE_ACK );
				SendPacket.WriteData( &ReqPacket._RoomInfo,		sizeof(BATTLE_ROOM_INFO) );
				SendPacket.WriteData( &i32Result,				sizeof(INT32) );
				m_pSessionManager->SendPacketMessageGame( i32ServerIdx, &SendPacket );
				break;
			}

			ReqPacket._RoomInfo._cBattle = (UINT8)i32BattleIdx;

			pPacket->WriteData( 0, &ReqPacket, 	sizeof(P_BATTLE_ROOM_CREATE_REQ) );
			m_pSessionManager->SendPacketMessageBattle( i32BattleIdx, pPacket );
		}
		break;
	case PROTOCOL_BATTLE_SERVER_DESTROY_REQ:
		{
			P_BATTLE_ROOM_DESTROY_REQ ReqPacket;
			pPacket->ReadData( &ReqPacket,	sizeof(P_BATTLE_ROOM_DESTROY_REQ) );

			g_pConfig->FreeBattleServer( ReqPacket._RoomInfo._cBattle );

			if (FALSE == m_pSessionManager->SendPacketMessageBattle(ReqPacket._RoomInfo._cBattle, pPacket))
			{
				g_pLog->WriteLog(L"[FAIL] Battle Dest / Battle:%d / Game:%d / Channel:%d / Room:%d ", ReqPacket._RoomInfo._cBattle, ReqPacket._RoomInfo._cGame, ReqPacket._RoomInfo._cChannel, ReqPacket._RoomInfo._sRoom );
			}
		}
		break;
	case PROTOCOL_ASC_BATTLE_ROOM_COUNT_REQ:
		{
			INT32 i32RoomCount;
			pPacket->ReadData( &i32RoomCount, sizeof(INT32) );

			g_pConfig->SetBattleRoomCount( i32ServerIdx, i32RoomCount );

		}
		break;
	}

	return TRUE;
}