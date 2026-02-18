#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleClan.h"
#include "ModuleContextNc.h"

/*----------------------------------------------------------
Name : 
Desc : Work Thread에서만 호출 가능 
----------------------------------------------------------*/
INT32 CUserSession::PacketParsing(char * pPacketBuffer, INT32 i32Size)
{
	i3NetworkPacket Packet; 
	Packet.CopyToBuffer( pPacketBuffer, i32Size, 0);

	PROTOCOL	ProtocolID;
	BOOL		bSendMessage = FALSE;
	BOOL		bisKilled = FALSE;

	i3NetworkPacket SendPacket;
	SendPacket.Clear(); 

	//암호화
	//2. 암호화 되지 않은 패킷은 받지 않음
	BOOL bIsEncript = Packet.Decript( m_ui32EncriptKey ); 
#ifdef USE_PACKET_ENCRIPT
	if( FALSE == bIsEncript )
	{
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, 0, KILL_REASON_NOT_ENCRIPT_PACKET );
		I3TRACE("Not Encript Packet - Kill User"); 
		return i32Size;	// 해킹이지 잘못된 패킷이 아닙니다.
	}
#endif
	// 아직 패킷을 다 받지 못했습니다. 다 받지 못했을 경우는 0을 돌려줍니다.
	if( Packet.GetPacketSize() > i32Size )				return 0;
	// 서버가 강제로 삭제한 유저입니다. 
	if( (m_ui32ATIsKill > 0) || (GetIsActive() == FALSE) )	return i32Size;

	ProtocolID			= Packet.GetProtocolID();

	if( false == _PacketDecript( &Packet ) )
	{
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT_PACKET_REPEATING, ProtocolID, KILL_REASON_HACK_PACKET );
		return i32Size;
	}

	// 패킷 처리 테스트코드
	DWORD dwParseStartTime = GetTickCount();

	switch( ProtocolID & 0x0F00)
	{
		case PROTOCOL_OPER				: bSendMessage = _PacketParsingOper( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_AUTH_SERVER		: bSendMessage = _PacketParsingAuth_server( &Packet, &SendPacket, bisKilled);	break;
		case PROTOCOL_AUTH_SERVER_NC	: bSendMessage = _PacketParsingAuthShop( &Packet, &SendPacket, bisKilled);		break;
		case PROTOCOL_TEST				: bSendMessage = _PacketParsingTest( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_ASC				: bSendMessage = _PacketParsingAsc( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_CS				: bSendMessage = _PacketParsingClan( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_CS_MATCH			: bSendMessage = _PacketParsingClanBattle( &Packet, &SendPacket, bisKilled);	break;
		case PROTOCOL_SERVER_MESSAGE	: bSendMessage = _PacketParsingServerMessage( &Packet, &SendPacket, bisKilled);	break;
		case PROTOCOL_BASE				: bSendMessage = _PacketParsingBase( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_LOBBY				: bSendMessage = _PacketParsingLobby( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_INVENTORY			: bSendMessage = _PacketParsingInventory( &Packet, &SendPacket, bisKilled);		break;
		case PROTOCOL_SHOP				: bSendMessage = _PacketParsingShop( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_ROOM				: bSendMessage = _PacketParsingRoom( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_BATTLE			: bSendMessage = _PacketParsingBattle( &Packet, &SendPacket, bisKilled);		break;
		default							:
			{
				bisKilled 		= TRUE;
				m_ui32KillReason 	= KILL_REASON_UNKNOWN_PACKET;
			}
			break;
	}

	// 패킷 처리 테스트코드
	DWORD dwParseEndTime = GetTickCount();
	DWORD dwElepseTime = dwParseEndTime - dwParseStartTime;

	if( m_dwParseTimeHigh < dwElepseTime ) 
	{
		m_dwParseTimeHigh = dwElepseTime;
		m_ui16ParseTimeHighProtocol = ProtocolID;
	}

	if( dwElepseTime > 20 )
	{
		// 패킷 처리시간 20ms 초과
		char strLog[256];
		i3String::Format( strLog, 256, "PacketParsing over 20ms - Protocol:%d / Time : %d / UID : %I64d / CheckCount : %d ", ProtocolID, dwElepseTime,  m_i64UID, m_ui32GameGuardCheckCount );
		g_pModuleLogFile->Write_M_Log(m_WorkThreadIdx, this, strLog);
	}
	
	//마지막 패킷은 보내고
	if(bSendMessage)
	{
		SendPacketMessage( &SendPacket ); 
	}
	
	if(bisKilled)
	{
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, ProtocolID, m_ui32KillReason );
	}

	m_ui16BackProtocolID	= ProtocolID;
	m_ui32ATLastRecv		= g_pContextMain->GetServerTime();

	if(m_ui32ATLastRecv != m_ui32ATPacketSecond)
	{
		m_ui32ATPacketSecond	= m_ui32ATLastRecv; 
		m_ui32PacketSecondCount	= 0; 
	}
	m_ui32PacketSecondCount++; 
	
	return Packet.GetPacketSize();
}

BOOL	CUserSession::_PacketParsingTest( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

#ifdef USE_TEST_PACKET
	INT32 Temp;
	UINT32 UTemp;
	switch( ProtocolID )
	{
	case PROTOCOL_TEST_NC_QA					: //QA팀 요청 패킷
		{
			CHECKVALID_DBINDEX; 
			pPacket->ReadData(&Temp, sizeof(INT32)); 
			//1. 계급 조정 커맨드 
			//2. 피노 조정 
			//3. 경험치 조정 
			//4. 캐쉬 조정 커맨드 
			//5. 도전과제 변경

			//10. 라운드 조정 커맨드 
			//11. 킬수 조정 커맨드 
			//12. 끝없는 배틀 

			switch(Temp)
			{
			case 1:// 계급 		
				pPacket->ReadData(&UTemp, sizeof(UINT32)); 
				if(UTemp > 50)UTemp = 50; 
				m_UserInfoBasic._rank = UTemp; 
				break;		
			case 2:	// Pino
				pPacket->ReadData(&UTemp, sizeof(UINT32)); 
				m_UserInfoBasic._point = UTemp;
				break; 
			case 3: //경험치 
				pPacket->ReadData(&UTemp, sizeof(UINT32)); 
				m_UserInfoBasic._exp = UTemp;
				break; 
			case 4: 
				pPacket->ReadData(&UTemp, sizeof(UINT32)); 
				m_UserInfoBasic._cash = UTemp;
				break; 
			case 5:	//도전과제입니다. 이 패킷은 리턴값이 있습니다. 
				//도전과제는 클라이언트가 알아서 패킷을 보냅니다. 구조변경(20080229)
				break; 
			case 10:// 윈 라운드 조정 
				{
					if( m_i32RoomIdx == SERVER_VALUE_UNKNOWN)return FALSE;
					UINT8 Red; 
					UINT8 Blue; 
					pPacket->ReadData(&Red,		sizeof(UINT8)); 
					pPacket->ReadData(&Blue,	sizeof(UINT8)); 
					m_pRoom->ForceSetKillCount(Red, Blue); 
				}
				break; 
			case 11:// 방에서 킬수 조정 
				{
					if( m_i32RoomIdx == SERVER_VALUE_UNKNOWN)return FALSE;
					UINT16 Red; 
					UINT16 Blue; 
					pPacket->ReadData(&Red,		sizeof(UINT16)); 
					pPacket->ReadData(&Blue,	sizeof(UINT16)); 
					m_pRoom->ForceSetKillCount(Red, Blue); 
				}
				break;
			case 12 : 
				if( m_i32RoomIdx == SERVER_VALUE_UNKNOWN)return FALSE; 
				m_pRoom->ForceBattleEndlessSet(); 
				break; 
				// 클랜 메시지
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
				{
					UINT32 Value, Value1, Value2;
					pPacket->ReadData(&Value,	sizeof(UINT32));
					pPacket->ReadData(&Value1,	sizeof(UINT32));
					pPacket->ReadData(&Value2,	sizeof(UINT32));

					g_pModuleClan->ClanSendTestNCQA( m_WorkThreadIdx, this, m_UserInfoBasic._clanidx, (UINT8)Temp, Value, Value1, Value2);
				}
				break;
				//훈장 메달 
			case 18: 
				{
					UINT32 Value1, Value2;
					pPacket->ReadData(&Value1,	sizeof(UINT32));		//
					pPacket->ReadData(&Value2,	sizeof(UINT32));
					switch( Value1 )
					{
					case 1: m_ui32Miniature	= 	Value2;	break; //약장
					case 2: m_ui32Insignia		= 	Value2;	break; //휘장 
					case 3: m_ui32Order		= 	Value2;	break; //훈장
					case 4: m_ui32Master		= 	Value2;	break; //마스터훈장
					case 5: //모두		
						m_ui32Miniature		= Value2;
						m_ui32Insignia			= Value2;
						m_ui32Order			= Value2;
						m_ui32Master			= Value2;
						break;
					}
				}
				break;
			}
		}
		break; 
	}
	ChangeDataBaseInfo( SAVE_DATABASE_ALL );
#endif

	return bSendMessage;
}

BOOL	CUserSession::_PacketParsingOper( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{
	BOOL		bSendMessage = FALSE;
	/*
	INT32		Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 
	
	switch( ProtocolID )
	{
	default:
		break;
	}
	*/
	return bSendMessage;
}

BOOL	CUserSession::_PacketParsingAsc( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{
	BOOL		bSendMessage = FALSE; 
	/*
	INT32		Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 
	
	switch( ProtocolID )
	{
	default:
		break;
	}
	*/

	return bSendMessage;
}


BOOL	CUserSession::_PacketParsingServerMessage( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{
	BOOL		bSendMessage = FALSE; 
	/*
	INT32		Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 
	
	switch( ProtocolID )
	{
		
	}
	*/

	return bSendMessage;
}