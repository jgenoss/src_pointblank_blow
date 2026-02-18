#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
// #include "ModuleLogFile.h"
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
	BOOL IsEncript = Packet.Decript( m_ui32EncriptKey ); 
	if( FALSE == IsEncript )
	{
#ifdef USE_PACKET_ENCRIPT
		_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, 0, KILL_REASON_NOT_ENCRIPT_PACKET );
		I3TRACE("Not Encript Packet - Kill User"); 
		return i32Size;	// 해킹이지 잘못된 패킷이 아닙니다.
#endif
	}
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
	LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
	QueryPerformanceFrequency(&stQP_Freq_Begin);
	QueryPerformanceCounter(&stQP_Count_Begin);

#ifdef _DEBUG
	g_PrintProtocolName( ProtocolID, "User" );
#endif
	INT32 i32ProtocolValue = ProtocolID & 0xFF00;

	switch(i32ProtocolValue)
	{
		case PROTOCOL_LOGIN				: bSendMessage = _PacketParsingLogin( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_AUTH				: bSendMessage = _PacketParsingAuth_server( &Packet, &SendPacket, bisKilled);	break;
		case PROTOCOL_SHOP				: bSendMessage = _PacketParsingShop( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_ASC				: bSendMessage = _PacketParsingAsc( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_CS				: bSendMessage = _PacketParsingClan( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_CS_MATCH			: bSendMessage = _PacketParsingClanBattle( &Packet, &SendPacket, bisKilled);	break;
		case PROTOCOL_SERVER_MESSAGE	: bSendMessage = _PacketParsingServerMessage( &Packet, &SendPacket, bisKilled);	break;
		case PROTOCOL_COMMUNITY			: bSendMessage = _PacketParsingBase( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_BASE				: bSendMessage = _PacketParsingBase( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_LOBBY				: bSendMessage = _PacketParsingLobby( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_INVENTORY			: bSendMessage = _PacketParsingInventory( &Packet, &SendPacket, bisKilled);		break;
		case PROTOCOL_RS_IGS			: bSendMessage = _PacketParsingRsIgs( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_ROOM				: bSendMessage = _PacketParsingRoom( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_BATTLE			: bSendMessage = _PacketParsingBattle( &Packet, &SendPacket, bisKilled);		break;
		case PROTOCOL_MEDAL				: bSendMessage = _PacketParsingMedal( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_SKILL				: bSendMessage = _PacketParsingSkill( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_CHAR				: bSendMessage = _PacketParsingChar( &Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_QUICKJOIN			: bSendMessage = _PacketParsingQuickJoin( &Packet, &SendPacket, bisKilled);		break;
		case PROTOCOL_MYINFO			: bSendMessage = _PacketParsingMyInfo(&Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_GACHA				: bSendMessage = _PacketParsingGacha(&Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_GMCHAT			: bSendMessage = _PacketParsingGMChat(&Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_CHEAT				: bSendMessage = _PacketParsingCheat(&Packet, &SendPacket, bisKilled);			break;
		case PROTOCOL_CLAN_WAR			: bSendMessage = _PacketParsingClanWar(&Packet, &SendPacket, bisKilled);		break;
		default							:
			{
				bisKilled 		= TRUE;
				m_ui32KillReason= KILL_REASON_UNKNOWN_PACKET;
			}
			break;
	}

	// 패킷 처리 테스트코드
	QueryPerformanceCounter(&stQP_Count_End);
	stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart;

	REAL64 r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart;

	if(m_r64ParseTimeHigh < r64ElepseTime)
	{
		m_r64ParseTimeHigh = r64ElepseTime;
		m_ui16ParseTimeHighProtocol = ProtocolID;
	}

	if(r64ElepseTime > 0.02f )	// 20ms
	{
		// 패킷 처리시간 20ms 초과
		g_pLog->WriteLogSession(this, L"PacketParsing over 20ms - Protocol:%d / UID : %I64d / Time(Sec) : %.6lf", ProtocolID, m_i64UID, r64ElepseTime);
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
	m_ui32ATLastRecv		= i3ThreadTimer::GetServerTime();

	if(m_ui32ATLastRecv != m_ui32ATPacketSecond)
	{
		m_ui32ATPacketSecond	= m_ui32ATLastRecv; 
		m_iPacketSecondCount	= 0; 
	}
	m_iPacketSecondCount++;

	g_pContextMain->AddUserPacketProcess(m_WorkThreadIdx, i32ProtocolValue / 0x0100, r64ElepseTime);
	
	return Packet.GetPacketSize();
}

BOOL	CUserSession::_PacketParsingAsc( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
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


BOOL	CUserSession::_PacketParsingServerMessage( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
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