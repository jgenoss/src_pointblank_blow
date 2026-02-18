#include "pch.h"
#include "UserSession.h"

#include "Room.h"

BOOL CUserSession::_PacketParsingMyInfo(i3NetworkPacket* pRecvPacket, i3NetworkPacket* pSendPacket, BOOL &bIsKilled)
{
	BOOL bSendMessage = FALSE;

	PROTOCOL ProtocolID = pRecvPacket->GetProtocolID();
	switch( ProtocolID )
	{
	case PROTOCOL_MYINFO_ENTER_REQ:
		{
			CHECKVALID_LOBBY;

			if( SERVER_VALUE_UNKNOWN < m_i32RoomIdx )
				m_pRoom->MoveInfo(this, TRUE); 

			m_i32SubTask = GAME_TASK_SUB_INFO;

			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_MYINFO_ENTER_ACK);
		}
		break;

	case PROTOCOL_MYINFO_LEAVE_REQ:
		{
			CHECKVALID_LOBBY;

			if( SERVER_VALUE_UNKNOWN < m_i32RoomIdx )
				m_pRoom->MoveInfo(this, FALSE); 

			m_i32SubTask = GAME_TASK_SUB_NONE;

			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_MYINFO_LEAVE_ACK);
		}
		break;

	default:
		{
			bIsKilled = TRUE;
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
		}
		break;
	}

	return bSendMessage;
}
