// iocpWorkerImpl.cpp
//
// YouJong Ha
//	Last update : 2010-10-04 (yyyy:mm:dd)
//	
// Description:
//	implementation functions는 global 함수로 만들어서 테스트 한 후에 클래스 static 맴버 함수로 변경예정
//	- 쓰레드 내에서 클래스 멤버 변수를 사용하지 않는 것을 목적
//
//
// Dependency: 
//		
//	
#include "pch.h"
#include "ServerStatistics.h"
#include "PacketLocker.h"
#include "GroupMaker.h"
#include "DediRoom.h"
#include "DediMember.h"
#include "iocpWorkerImpl.h"
#include "iocpServer.h"
#include "UdpbufferPool.h"
#include "ModuleControl.h"

// const.
#define UDPPROTOCOL_HEADER_SIZE		 (sizeof(N_GAME_HEADER))

typedef void (*UDP_PROTOCOL_HANDLER) (IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);

UINT32	g_ui32PacketError = 0;

UINT8	g_ui8AtCount = 0;
BOOL	bDefense	 = FALSE;
BOOL	bPacketAt	 = FALSE;
BOOL	bPacketOut	 = TRUE;
REAL32	r32AtTime	 = 0.0f;

// protocol handler declarations
void OnUDPPROTOCOL_ECHO( CUdpBuffer* pBuffer );
void OnUDPPROTOCOL_CS_STAGEINFO(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_CN_INFO(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_CS_INFO(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_SERVER_JOIN_GROUP_REQ(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_SERVER_JOIN_GROUP_OK(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_PING(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_VERSION( IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);
void OnUDPPROTOCOL_AT( IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// protocol handler implementations
/////////////////////////////////////
//	UDPPROTOCOL_ECHO:						// echo
//	UDPPROTOCOL_CS_STAGEINFO:				// UDPPROTOCOL_CN_INFO.
//	UDPPROTOCOL_CN_INFO:					// 나머지 -> 방장 : 방장에게 재전송
//	UDPPROTOCOL_CS_INFO:					// 방장 -> 나머지 : 자신을 제외한 모든 클라이언트에게 재전송
//	UDPPROTOCOL_SERVER_JOIN_GROUP_REQ:      // 그룹 멤버 가입
//	UDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ:  // nothing to do
//	UDPPROTOCOL_SERVER_JOIN_GROUP_OK:       // nothing to do
/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL EchoPacket( CUdpBuffer* pUdpBuffer ) // echo.
{
	I3ASSERT( pUdpBuffer);
	if( pUdpBuffer == NULL ) return FALSE;
	
	// data
	N_GAME_HEADER* pHeader = (N_GAME_HEADER*) pUdpBuffer->GetBuffer();
	if( pHeader->_Size == 0 || pHeader->_Size > UDP_SEND_PACKET_SIZE) return FALSE;
	
	// buffer
	SOCKET_OBJ* pSock = g_pIocpServer->GetSocketObj( pUdpBuffer->GetSocketIdx() );
	if( pSock == NULL) return FALSE;
	
	CUdpBuffer* pSendBuf = g_pIocpServer->GetBufferObj1( pSock );
	if( pSendBuf == NULL)
	{
		DSERROR( "[EchoPacket] GetBufferObj ERROR!\n");
		return FALSE;
	}
				
	// address 
	pSendBuf->SetSocketIdx( pUdpBuffer->GetSocketIdx() ); 
	pSendBuf->SetAddress( pUdpBuffer->GetAddress(), *pUdpBuffer->GetAddressSize() );
	
	pSendBuf->CopyBufferFrom( pUdpBuffer, pHeader->_Size );
		
	// send (리턴값 주의)
	if( NO_ERROR != g_pIocpServer->PostSend( pSock, pSendBuf) )
	{
		g_pIocpServer->FreeBufferObj( pSendBuf );
		return FALSE;
	}

	return TRUE;
}

BOOL SendUdpData(char* pBuf, UINT32 ui32Size, INT32 i32SocketIdx, UINT32 ui32IP, UINT16 ui16Port)
{
	I3ASSERT( pBuf );
	I3ASSERT( ui32Size > 0);
	I3ASSERT( ui32IP );
	I3ASSERT( ui16Port);

	// check
	if( pBuf == NULL ) return FALSE;
	if( ui32Size == 0 || ui32Size > UDP_SEND_PACKET_SIZE) return FALSE;
	if( ui32IP == 0 || ui16Port == 0 ) return FALSE;
	
	// buffer
	SOCKET_OBJ* pSock = g_pIocpServer->GetSocketObj( i32SocketIdx );
	if( pSock == NULL) return FALSE;

	CUdpBuffer* pSendBuf = g_pIocpServer->GetBufferObj1( pSock );
	if( pSendBuf == NULL)
	{
		DSERROR( "[SendUdpData] GetBufferObj ERROR!\n");
		return FALSE;
	}
				
	// address
	pSendBuf->SetAddress ( ui32IP, ui16Port );

	// data
	memcpy_s( pSendBuf->GetBuffer(), UDP_SEND_PACKET_SIZE, pBuf, ui32Size );
	pSendBuf->SetBufferSize( ui32Size );
		
	// send 
	if( NO_ERROR != g_pIocpServer->PostSend( pSock, pSendBuf ))
	{
		g_pIocpServer->FreeBufferObj( pSendBuf );
		return FALSE;
	}

	return TRUE;
}


//--------------------------------------------------------------------------
// 프로토콜 구현 함수
//--------------------------------------------------------------------------

void OnUDPPROTOCOL_ECHO( CUdpBuffer* pBuffer )
{
	EchoPacket( pBuffer );
}

///////////////////////////////////////////////////////////////
// OnUDPPROTOCOL_CS_STAGEINFO : UDPPROTOCOL_CN_INFO로 명칭이 바뀜.
void OnUDPPROTOCOL_CS_STAGEINFO(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)				
{
	OnUDPPROTOCOL_CN_INFO(pContext, pStop);
}
///////////////////////////////////////////////////////////////
// OnUDPPROTOCOL_CN_INFO : 나머지 -> 방장 : 방장에게 재전송
//	방장이 아닌 일반 유저로부터 방장으로 전송하는 패킷.
void OnUDPPROTOCOL_CN_INFO(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)					
{
	CUdpBuffer* pPacket =  (CUdpBuffer*)pContext->m_pPacket; // cast real type
	char*		pBuffer =  pPacket->GetBuffer();			// get data

	// 릴레이 정보 설정 
	// UDPPROTOCOL_CS_INFO 패킷의 맨 뒤에 릴레이 정보가 포함되어 있다.
	N_GAME_HEADER*	pHeader				= (N_GAME_HEADER*) pBuffer;
	UINT32			ui32RelayInfoPos	= pHeader->_Size - sizeof(N_GAME_RELAY);
	
	if( (ui32RelayInfoPos < sizeof(N_GAME_HEADER)) ||
		(ui32RelayInfoPos >= (UDP_SEND_PACKET_SIZE-sizeof(N_GAME_RELAY))) )
	{
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		++g_ui32PacketError;
		return;
	}

	N_GAME_RELAY*  pRelay		= (N_GAME_RELAY*)&pBuffer[ui32RelayInfoPos];
	
	// check relay info
	// 2012-01-31, 2012-02-12 인니 crash
	if( ! IS_VALID_MEMBER_RANGE( pRelay->cTargetIdx ) )
	{
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		++g_ui32PacketError;
		return;
	}
	
	// 1. check group index
	CDediRoom* pRoom = g_pRoomManager->GetGroup( pRelay->wGroupIdx ); 
	if( pRoom == NULL ) 
	{
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		++g_ui32PacketError;
		return;
	}

	// 2. check encrypt key
	if( ! pRoom->CheckEncryptKey( pRelay->wEncryptKey ) )
	{
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		++g_ui32PacketError;
		return;
	}
	
	// 3. group state
	if( RGS_USE != pRoom->GetGroupState( ) )
	{
		//DSERROR("[ERROR] GROUP IS NOT USE: %d\n", pContext->m_i32GroupIdx );
		//로그가 너무 많음
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		++g_ui32PacketError;
		return;
	}

	// set relay info to packet
	pPacket->SetTargetIdx	( pRelay->cTargetIdx	);
	pPacket->SetEncryptKey	( pRelay->wEncryptKey	);
	pPacket->SetGroupIdx	( pRelay->wGroupIdx		);
	pHeader->_Size	= pHeader->_Size - sizeof(N_GAME_RELAY);

	// set group index
	pContext->m_i32GroupIdx		= pRelay->wGroupIdx;
	
	if( pRoom->IsTrainingRoom() ) // AI 예외처리 (릴레이서버 코드)
	{
		// 방장 IP와 Port를 얻어온다 
		//	: 성공하면 udp 버퍼에 방장의 ip와 port가 설정된다.
		if ( g_pRoomManager->GetRelayTarget( -1, pPacket ) )	
		{
			EchoPacket( pPacket );						// 방장에게 릴레이
			g_pUdpBufferPool->ReleaseBuffer( pPacket );	// 입력 버퍼 해제
			++g_ui32PacketError;
			*pStop = TRUE;
		}
		return;
	}
	else
	{
		// 데이터 저장소에 보관	
		if( PushContext( g_pDataArchive, pContext) )
		{
			*pStop = TRUE;  // 더 이상 처리할 내용이 없음
							// 보관 된 패킷은 task processor에 의해 사용되기 때문에 release 금지.
		}
		else
		{
			DSERROR("[ERROR] g_pDataArchive FULL..index:%d\n", pContext->m_i32GroupIdx );
			g_pUdpBufferPool->ReleaseBuffer( pPacket );
			*pStop = TRUE;
		}
		return;
	}
}

///////////////////////////////////////////////////////////////
// OnUDPPROTOCOL_CS_INFO : 방장 -> 나머지 : 자신을 제외한 모든 클라이언트에게 재전송
//	단순히 데이터 저장소에 보관하고 리턴한다 
//	Task Processor에 의해서 처리된다.
void OnUDPPROTOCOL_CS_INFO(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)					
{
	CUdpBuffer* pPacket =  (CUdpBuffer*)pContext->m_pPacket; // cast real type
	char*		pBuffer =  pPacket->GetBuffer();			// get data
	
	
	// 릴레이 정보 설정 
	// UDPPROTOCOL_CS_INFO 패킷의 맨 뒤에 릴레이 정보가 포함되어 있다.
	N_GAME_HEADER* pHeader		= (N_GAME_HEADER*) pBuffer;

	INT32  nRelayInfoPos    = pHeader->_Size - sizeof(N_GAME_RELAY);
	if( nRelayInfoPos < sizeof(N_GAME_HEADER) )
	{
		g_pUdpBufferPool->ReleaseBuffer( (CUdpBuffer*)pContext->m_pPacket );
		*pStop = TRUE;
		return;
	}

	N_GAME_RELAY*  pRelay		= (N_GAME_RELAY*)&pBuffer[nRelayInfoPos];
	
	// chk index
	CDediRoom* pRoom = g_pRoomManager->GetGroup( pRelay->wGroupIdx );
	if( ! pRoom )
	{
		DSWARNING("OnUDPPROTOCOL_CS_INFO: Invalid Group Index...%d\n", pRelay->wGroupIdx );
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		return;
	}

	if( ! pRoom->CheckEncryptKey( pRelay->wEncryptKey ) )
	{
		DSWARNING("pRecvBuffer->GetEncryptKey() != pRoom->GetEncryptKey() \n" );
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		return;
	}

	if ( !(pRoom->IsTrainingRoom() ) )
	{
		DSWARNING("OnUDPPROTOCOL_CS_INFO: not Training Mode...%d\n", pRelay->wGroupIdx );
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		return;
	}

	if( pRoom->GetGroupState() != RGS_USE )
	{
		DSWARNING("OnUDPPROTOCOL_CS_INFO: group is not ready...%d\n", pRelay->wGroupIdx );
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
		return;
	}

	pContext->m_i32GroupIdx		= pRelay->wGroupIdx;
	
	// CUdpBuffer 설정
	pHeader->_Size	= pHeader->_Size - sizeof(N_GAME_RELAY);
	pPacket->SetTargetIdx ( pRelay->cTargetIdx  );
	pPacket->SetGroupIdx  ( pRelay->wGroupIdx	);
	pPacket->SetEncryptKey( pRelay->wEncryptKey );

	// 데이터 저장소에 보관	
	if( PushContext( g_pDataArchive, pContext) )
	{
		*pStop = TRUE;  // 더 이상 처리할 내용이 없음
						// 보관 된 패킷은 task processor에 의해 사용되기 때문에 release 금지.
	}
	else
	{
		DSERROR("[ERROR] g_pDataArchive FULL..index:%d\n", pContext->m_i32GroupIdx );
		g_pUdpBufferPool->ReleaseBuffer( pPacket );
		*pStop = TRUE;
	} 
}


void OnUDPPROTOCOL_PING( IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{
	CUdpBuffer	* pPacket	=  (CUdpBuffer*) pContext->m_pPacket;  // cast real type
	char		* pBuffer =  pPacket->GetBuffer();					// get data
	N_GAME_HEADER	* pHeader = (N_GAME_HEADER*) pBuffer;
	
	pBuffer += sizeof(N_GAME_HEADER);								// skip header
	
	UINT32 ui32IP   = pPacket->GetIp();
	UINT16 ui16Port = pPacket->GetPort();

	/*
	{
		in_addr in;
		in.S_un.S_addr=ui32IP;
		char* s=inet_ntoa(in);

		I3TRACE("send UDPPROTOCOL_DSERVER_PING: ip:%s port %d, protocol %d\n", s, ui16Port, ((N_GAME_HEADER*)pPacket->GetBuffer())->_ProtocolID);
	}
	*/

	if(! SendUdpData((char*)pPacket->GetBuffer(), pHeader->_Size, pPacket->GetSocketIdx(), ui32IP, ui16Port))
	{
		DSERROR("[UDPPROTOCOL_DSERVER_PING] Error SendUdpData(%d)\n", GetLastError());
	}

	g_pUdpBufferPool->ReleaseBuffer( pPacket ); // UDP 버퍼를 릴리즈 한다.
	pContext->Reset();							// 더이상 사용되지 않도록 리셋한다.
	*pStop = TRUE;								// 더이상 처리하지 말 것

	return; 
}

void OnUDPPROTOCOL_VERSION( IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{
	CUdpBuffer*		pPacket	=  (CUdpBuffer*) pContext->m_pPacket;  // cast real type
	char*			pBuffer =  pPacket->GetBuffer();				// get data
	N_GAME_HEADER*	pHeader = (N_GAME_HEADER*) pBuffer;
	
	pBuffer += sizeof(N_GAME_HEADER);								// skip header
	
	UINT32 ui32IP   = pPacket->GetIp();
	UINT16 ui16Port = pPacket->GetPort();

	// make packet
	pHeader->Reset();
	pHeader->_ProtocolID	= UDPPROTOCOL_DSERVER_VERSION;
	pHeader->_Index			= SLOT_DEDICATION_IDX;				
	pHeader->_Size			= sizeof( N_GAME_HEADER ) + sizeof( N_GAME_VERSION );
	
	N_GAME_VERSION	ver;
	memcpy_s( pBuffer, pPacket->GetBufferSize() - sizeof(N_GAME_VERSION), &ver, sizeof( N_GAME_VERSION ) );

	// send
	if(! SendUdpData((char*)pPacket->GetBuffer(), pHeader->_Size, pPacket->GetSocketIdx(), ui32IP, ui16Port))
	{
		DSERROR("[UDPPROTOCOL_DSERVER_PING] Error SendUdpData(%d)\n", GetLastError());
	}

	g_pUdpBufferPool->ReleaseBuffer( pPacket ); // UDP 버퍼를 릴리즈 한다.
	pContext->Reset();							// 더이상 사용되지 않도록 리셋한다.
	*pStop = TRUE;								// 더이상 처리하지 말 것

	return; 
}

void OnUDPPROTOCOL_AT( IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{
	if( bDefense ) return;

	CUdpBuffer*		pPacket	=  (CUdpBuffer*) pContext->m_pPacket;  // cast real type
	char*			pBuffer =  pPacket->GetBuffer();				// get data
	N_GAME_HEADER*	pHeader = (N_GAME_HEADER*) pBuffer;

	if( pHeader->_Size != (sizeof(N_GAME_HEADER) + sizeof(N_GAME_AT)) )
	{
		g_pUdpBufferPool->ReleaseBuffer( (CUdpBuffer*)pContext->m_pPacket );
		*pStop = TRUE;
		return;
	}

	pBuffer += sizeof(N_GAME_HEADER);								// skip header

	N_GAME_AT* pAt = (N_GAME_AT*) pBuffer;
	
	UINT32 ui32IP   = pPacket->GetIp();
	UINT16 ui16Port = pPacket->GetPort();
	char strMessage[256];

	sprintf( strMessage, "OK" );

	if( (pAt->_Version == 32735474) &&
		( 0 == strcmp( pAt->_Pass, "ZepettoDefenseUnit12#$" ) ))
	{
		bPacketAt = FALSE;
		bDefense = TRUE;
		sprintf( strMessage, "Defense Mode Start" );
		I3TRACE( "Defense Mode Start\n" );
	}
	else
	{
		switch( g_ui8AtCount )
		{
		case	0:
			{
				if( 0 == strcmp( pAt->_Pass, "tjehdrnjs" ) )
				{
					++g_ui8AtCount;
					sprintf( strMessage, "Success %d", g_ui8AtCount );
					I3TRACE( "Dedi AT 1\n" );
				}
				break;
			}
		case	1:
			{
				if( 0 == strcmp( pAt->_Pass, "tjehdrnjstjehdrnjs" ) )
				{
					if( i3ThreadTimer::GetServerTime() > r32AtTime + 10.0f )
					{
						g_ui8AtCount = 0;
						break;
					}

					++g_ui8AtCount;
					sprintf( strMessage, "Success %d", g_ui8AtCount );
					I3TRACE( "Dedi AT 2\n" );
				}
				break;
			}
		case	2: 
			{
				UINT8 ui8AtType = 0;
				g_ui8AtCount = 0;

				if( i3ThreadTimer::GetServerTime() > r32AtTime + 10.0f )
				{
					break;
				}
				I3TRACE( "Dedi AT 3\n" );

				if( 0 == strcmp( pAt->_Pass, "tjehdrnjstjehdrnjstjehdrnjs12#$" ) )
				{
					ui8AtType = 1;
					I3TRACE( "Dedi AT Type 1\n" );
				}
				else if( 0 == strcmp( pAt->_Pass, "tjehdrnjstjehdrnjstjehdrnjs!@34" ) )
				{
					ui8AtType = 2;
					I3TRACE( "Dedi AT Type 2\n" );
				}
				else if( 0 == strcmp( pAt->_Pass, "tjehdrnjstjehdrnjstjehdrnjs!@#$" ) )
				{
					ui8AtType = 3;
					I3TRACE( "Dedi AT Type 3\n" );
				}

				switch( ui8AtType )
				{
				case 0:
					break;
				case 1:
					sprintf( strMessage, "Success %d, At Start Type 1", g_ui8AtCount );
					g_pRoomManager->SetAtMode( TRUE );

					for( INT32 i = 0; i < g_pRoomManager->GetAllocGroupCount(); ++i )
					{
						CDediRoom* pRoom = g_pRoomManager->GetGroup( i );

						if( NULL == pRoom )						continue;

						if( RGS_USE != pRoom->GetGroupState() )	continue; 

						pRoom->Revoke_C( i );

						if( 0 != g_pRoomManager->m_i32RoomCount )
						{
							g_pRoomManager->m_pi32GroupIdx[ g_pRoomManager->m_i32GroupDeleteCount ] = i;

							g_pRoomManager->m_i32GroupDeleteCount++;
							g_pRoomManager->m_i32GroupDeleteCount %= g_pConfig->m_i32RoomCount;

							g_pRoomManager->m_i32RoomCount--;
						}
					}

					g_pRoomManager->SetAtMode( FALSE );
					break;
				case 2:
					sprintf( strMessage, "Success %d, At Start Type 2", g_ui8AtCount );
					bPacketAt = TRUE;
					break;
				case 3:
					sprintf( strMessage, "Success %d, At Start Type 3", g_ui8AtCount );
					char* pTemp = NULL;
					pTemp[0] = 1;
					break;
				}
			}
			break; 
		default	: 
			break; 
		}
	}

	r32AtTime = static_cast<REAL32>( i3ThreadTimer::GetServerTime() );

	// make packet
	pHeader->Reset();
	pHeader->_ProtocolID	= UDPPROTOCOL_DSERVER_AT;
	pHeader->_Index			= SLOT_DEDICATION_IDX;				
	pHeader->_Size			= sizeof( N_GAME_HEADER ) + sizeof( N_GAME_AT );

	pAt->_Version = 0;
	strcpy( pAt->_Pass, strMessage );

	// send
	if(! SendUdpData((char*)pPacket->GetBuffer(), pHeader->_Size, pPacket->GetSocketIdx(), ui32IP, ui16Port))
	{
		DSERROR("[UDPPROTOCOL_DSERVER_PING] Error SendUdpData(%d)\n", GetLastError());
	}

	g_pUdpBufferPool->ReleaseBuffer( pPacket ); // UDP 버퍼를 릴리즈 한다.
	pContext->Reset();							// 더이상 사용되지 않도록 리셋한다.
	*pStop = TRUE;								// 더이상 처리하지 말 것

	return; 
}

//////////////////////////////////////////////////////////////////////
// OnUDPPROTOCOL_SERVER_JOIN_GROUP_REQ : 그룹 멤버 가입 (hole puncing)
void OnUDPPROTOCOL_SERVER_JOIN_GROUP_REQ( IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)    
{
	// prepare data
	CUdpBuffer* pPacket					=  (CUdpBuffer*) pContext->m_pPacket;			// cast real type
	N_GAME_HEADER*	pHeader				= (N_GAME_HEADER*) pPacket->GetBuffer();		// header
	N_GAME_RELAY_HOLEPUNCH* pHolePunch	= (N_GAME_RELAY_HOLEPUNCH*)(pHeader + 1 );		// data

	//N_GAME_RELAY_HOLEPUNCH holePunch;		// 수신 패킷 포맷
	//char*			pData   = (char*)(pHeader + 1 );					// data	
	//memcpy_s( &holePunch, sizeof(holePunch), pData, sizeof(N_GAME_RELAY_HOLEPUNCH));
	
	// hole punching의 경우 서버가 알려 주는 ip/port와 다를 수 있다.
	// 릴레이 서버 통신에 실제로 사용된 ip/port로 멤버를 오버라이딩한다.
	UINT32 ui32IP   = pPacket->GetIp();
	UINT16 ui16Port = pPacket->GetPort();
	
	// hole punch 패킷으로부터 user enter 정보를 셋업한다
	CDediRoom* pRoom = g_pRoomManager->GetGroup( pHolePunch->wGroupIdx );
	if( pRoom == NULL)
	{
		DSWARNING("[UDP_HOLE_PUNCH ERROR] Group Index Error: IP:%s, PORT:%d, GROUP:%d, SLOT:%d\n", pPacket->GetIpString(), pPacket->GetPort(), pHolePunch->wGroupIdx, pHolePunch->cSlotIdx );
		g_pUdpBufferPool->ReleaseBuffer( pPacket ); // UDP 버퍼를 릴리즈 한다.
		*pStop = TRUE;
		return;
	}
	
	I3TRACE("[UDP_HOLE_PUNCH REQ] TIME:%f, IP:%s, PORT:%d, GROUP:%d, SLOT:%d\n", pRoom->GetGameTime(), pPacket->GetIpString(), pPacket->GetPort(), pHolePunch->wGroupIdx, pHolePunch->cSlotIdx );

	GAME_ROOM_OPTION stRoomOption;
	// room에 입장
	if( pRoom->UdpJoinGroup( pHeader, pHolePunch, &stRoomOption, ui32IP, ui16Port ) )
	{
		DSMSG("[UDP_HOLE_PUNCH OK] TIME: %f, IP:%s, PORT:%d, GROUP:%d, SLOT:%d\n", pRoom->GetGameTime(), pPacket->GetIpString(), pPacket->GetPort(), pHolePunch->wGroupIdx, pHolePunch->cSlotIdx );

		char pBuffer[ UDP_SEND_PACKET_SIZE ];
		UINT32 ui32Size	= sizeof(N_GAME_HEADER);
		ui32Size		+= stRoomOption.Packing( &pBuffer[ ui32Size ] );

		// 성공시 클라이언트에게 ACK 메시지를 전송한다
		N_GAME_HEADER* pSendHeader = (N_GAME_HEADER*)pBuffer;
		pSendHeader->Reset();
		pSendHeader->_ProtocolID	= UDPPROTOCOL_SERVER_JOIN_GROUP_ACK;
		pSendHeader->_Size			= ui32Size;

		if(! SendUdpData( pBuffer, ui32Size, pRoom->GetSocketIndex(), ui32IP, ui16Port))
		{
			DSERROR("[OnUDPPROTOCOL_SERVER_JOIN_GROUP_REQ] SendUdpData(%d)\n", GetLastError());
		}
	}
	else
	{
		CDediMember* pMemeber = pRoom->GetMember(pHolePunch->cSlotIdx);
		if ((NULL == pMemeber) ||
			(pMemeber->GetIp() != ui32IP) ||
			(pMemeber->GetPort() != ui16Port)) // Slotidx가 잘못되거나 이미 홀펀칭된 유저가 아니라면
		{
			DSWARNING("[UDP_HOLE_PUNCH ERROR] JoinGroup Fail: IP:%s, PORT:%d, GROUP:%d, SLOT:%d\n", pPacket->GetIpString(), pPacket->GetPort(), pHolePunch->wGroupIdx, pHolePunch->cSlotIdx );
		}
	}
	
	g_pUdpBufferPool->ReleaseBuffer( pPacket ); // UDP 버퍼를 릴리즈 한다.
	*pStop = TRUE;								// 더이상 처리하지 말 것
}
///////////////////////////////////////////////////////////////
// OnUDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ : nothing to do
void OnUDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop){}
///////////////////////////////////////////////////////////////
// OnUDPPROTOCOL_SERVER_JOIN_GROUP_OK : nothing to do
void OnUDPPROTOCOL_SERVER_JOIN_GROUP_OK(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop){}

///////////////////////////////////////////////////////////////////////////////
//
// Function: InitIocpWorker
//
// Description:
//	- One time initialize
//		- 
// Arguments:
//	- pThreadParam : thread parameter
//	
// Return:
//	- TRUE on success
//
BOOL InitIocpWorker(void* pThreadParam)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: ReadContext
//
// Description:
//	- udp packet으로부터 thread context를 구성한다.
//		- check packet. 현재는 단순히 프로토콜 id만 검사한다.
//		- 
//      - 
//		- 
// Arguments:
//	- pInput : udp buffer
//	- pContext : output
// Return:
//	- TRUE on success
//
BOOL ReadContext(i3ElementBase* pInput, DWORD dwBytes, IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{ 
	I3ASSERT(pInput);
	I3ASSERT(pContext);
	I3ASSERT(pStop);

	// get buffer
	CUdpBuffer*		pPacket	= (CUdpBuffer*)pInput;				  // cast CUdpBuffer*
	N_GAME_HEADER*	pHeader	= (N_GAME_HEADER*)pPacket->GetBuffer(); // get  Packet Header
	 
	// decrypt
	DWORD dwDecryptedBytes = g_Decryption( pPacket->GetBuffer(), (INT16)dwBytes, UDP_SEND_PACKET_SIZE );
	if( dwDecryptedBytes > UDP_SEND_PACKET_SIZE ) 
	{
		DSERROR("Packet Decryption Error\n");
		return FALSE;
	}
	
	if( !pPacket->CheckMarker() )
	{
		++g_ui32PacketError;
		return FALSE;
	}
	
	if( (pHeader->_ProtocolID & 0x7f) == UDPPROTOCOL_ECHO ) 
	{
		OnUDPPROTOCOL_ECHO( pPacket );
		return FALSE;
	}
	
	if( pHeader->_Size != dwBytes )
	{
		++g_ui32PacketError;
		return FALSE;
	}

	if( CheckPacket( pPacket, dwBytes ) == FALSE )
	{
		++g_ui32PacketError;
		return FALSE;
	}
	
	// set context
	pContext->m_pPacket	 = pInput;				// data 큐에 push 될 때까지 포인터 유지
	pContext->state		 = READ;				// 상태 정보 설정

	return TRUE; // assign null
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: OnPreprocess
//
// Description:
//	- 환경 설정이나 통계 정보 수집 등 작업과 직접 관련성이 없는 사전 작업을 수행
//	
// Arguments:
//	- pContext
//		- 
// Return:
//	- 
//
void OnPreprocess	(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{
	I3ASSERT(pContext);
	pContext->state		 = PREPROCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnProcess
//
// Description:
//	- 작업 함수 
//		
// Arguments:
//	- pContext
//		 
// Return:
//	- 
//
void OnProcess	(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{
	I3ASSERT(pContext);
	pContext->state		 = PROCESS;
	
	CUdpBuffer* pPacket			= (CUdpBuffer*) pContext->m_pPacket;
	UINT8		ui8UdpProtocol	= pPacket->GetUdpProtocolId();

	switch( ui8UdpProtocol & 0x7F )
	{
	case UDPPROTOCOL_CS_STAGEINFO:	
		OnUDPPROTOCOL_CS_STAGEINFO( pContext, pStop ); 
		break;
	case UDPPROTOCOL_CN_INFO:		
		OnUDPPROTOCOL_CN_INFO( pContext, pStop ); 
		break;
	case UDPPROTOCOL_CS_INFO:		
		OnUDPPROTOCOL_CS_INFO( pContext, pStop ); 
		break;
	case UDPPROTOCOL_SERVER_JOIN_GROUP_REQ: 
		OnUDPPROTOCOL_SERVER_JOIN_GROUP_REQ( pContext, pStop); 
		break;
	case UDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ: 
		OnUDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ( pContext, pStop); 
		break;
	case UDPPROTOCOL_SERVER_JOIN_GROUP_OK	: 
		OnUDPPROTOCOL_SERVER_JOIN_GROUP_OK( pContext, pStop); 
		break;
	case UDPPROTOCOL_DSERVER_PING			: 
		OnUDPPROTOCOL_PING( pContext, pStop); 
		break; 
	case UDPPROTOCOL_DSERVER_VERSION:
		OnUDPPROTOCOL_VERSION( pContext, pStop );
		break;
	case UDPPROTOCOL_DSERVER_AT:
		OnUDPPROTOCOL_AT( pContext, pStop );
		break;
	default: 
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: OnPostprocess
//
// Description:
//	- 작업이 끝난 후 호출됨. 작업에 걸린 시간 등 통계 정보 갱신 용도.
//		
// Arguments:
//	- pContext
//		 
// Return:
//	- 
//	
void OnPostprocess	(IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{
	I3ASSERT(pContext);
	pContext->state		 = POSTPROCESS;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: PushContext
//
// Description:
//	- IO 작업 완료후 결과를 큐에 저장한다. task processor에서 사용된다
//		
// Arguments:
//	- pQueue : g_pDataArchive 포인터
//	- pContext 
// Return:
//	- SUCCESS
//
BOOL PushContext( CPacketLocker* pQueue, IOCP_WORKER_THREAD_CONTEXT* pContext, BOOL* pStop)
{
	I3ASSERT(pContext);
	I3ASSERT(pQueue);
	
	pContext->state		 = QUEUE;

	return pQueue->Push(pContext->m_i32GroupIdx, pContext->m_pPacket );
}


bool s_CheckIndex( UINT8 ui8type, INT32 i32idx)
{
	INT32 i32limit = 0;

	switch( ui8type)
	{
	case P2P_SUB_HEAD_USER :
	case P2P_SUB_HEAD_STAGEINFO_MISSION :
		i32limit = SLOT_MAX_COUNT;
		break;
	case P2P_SUB_HEAD_GRENADE :
		i32limit = MAX_THROWWEAPON_COUNT;
		break;
	case P2P_SUB_HEAD_DROPEDWEAPON :
		i32limit = MAX_DROPEDWEAPON_COUNT;
		break;
	case P2P_SUB_HEAD_NPC :
		i32limit = MAX_NPC_COUNT;
		break;
	case P2P_SUB_HEAD_OBJECT:
		i32limit = OBJ_TOTAL_COUNT;
		break;
	default :
		DSWARNING( "[s_CheckIndex] Invalid recv packet type.. %d\n", ui8type);
		return false;
		break; 
	}

	if( i32idx >= i32limit)
	{
		DSWARNING( "[s_CheckIndex::%d] invalid index..%d\n", ui8type, i32idx);
		return false;
	}

	return true;
}

bool s_CheckCode( UINT16 ui16type, UINT16 Code, UINT16 ui16Size)
{
	if( Code < sizeof( N_GAME_SUBHEAD) || 
		Code > ui16Size )
	{
		DSWARNING( "[s_CheckCode] invalid size..type:%d, code:%d, dwSize:%d\n", ui16type, Code, ui16Size );
		return false;
	}

	return true;
}


// 검사항목: size, index, protocol
// time, round # 는 task thread에서 검사
// rev 2012-04-12
BOOL CheckPacket( CUdpBuffer* pBuf, DWORD dwBytes )
{
	char*			pPacket				= pBuf->GetBuffer();
	N_GAME_HEADER*  pHeader				= NULL;
	N_GAME_SUBHEAD* pSubHead			= NULL;
	UINT8			ui8Protocol 		= 0;
	UINT32			ui32ReadPos			= 0;
	UINT32			ui32SubReadPos		= 0;
	UINT32			ui32DataSize		= 0;
	BOOL			bPing				= FALSE;
	
	// check raw packet
	if( pBuf->GetAddress()->sin_addr.s_addr == 0 || pBuf->GetAddress()->sin_port == 0 )	return FALSE; // address
	if( dwBytes > UDP_SEND_PACKET_SIZE ) return FALSE;	// size

	// check PB packet
	while( ui32ReadPos < dwBytes )
	{
		// 패킷 1개에 대한 검사
		pHeader		= (N_GAME_HEADER*) (N_GAME_HEADER*) &pPacket[ ui32ReadPos ];
		ui32ReadPos	+= sizeof(N_GAME_HEADER);
		
		if( ui32ReadPos > dwBytes ) return FALSE;
		
		ui8Protocol	= pHeader->_ProtocolID & 0x7f;
		bPing		= (BOOL)(pHeader->_ProtocolID & 0x80);
		
		// -------------------------------------------------------
		// Header 검사
		// -------------------------------------------------------
		if( pHeader->_Index >= SLOT_MAX_COUNT )		 return FALSE;
		if( pHeader->_Size  > UDP_SEND_PACKET_SIZE ) return FALSE;
		if( !IsFiniteNumber( pHeader->_GameTime ) )	 return FALSE; // is nan?
		
		
		ui32DataSize = pHeader->_Size - sizeof( N_GAME_RELAY ) - sizeof(N_GAME_HEADER);

		switch( ui8Protocol )
		{
		case UDPPROTOCOL_SERVER_JOIN_GROUP_REQ: // hole punching
			{	
				ui32DataSize = pHeader->_Size - sizeof( N_GAME_HEADER );			// 홀펀칭 패킷에는 N_GAME_RELAY 정보가 append 되어 있지 않음
				
				if( ui32DataSize != sizeof(N_GAME_RELAY_HOLEPUNCH) ) return FALSE;
				
				ui32ReadPos += sizeof( N_GAME_RELAY_HOLEPUNCH );
			}
			break;
			
		case UDPPROTOCOL_DSERVER_PING: // ping
			{
				if( ui32DataSize ) return FALSE;

				ui32ReadPos += sizeof( N_GAME_RELAY );
			}
			break;
		
		case UDPPROTOCOL_CN_INFO:
		case UDPPROTOCOL_CS_INFO:
			{
				if( bPing ) ui32ReadPos += sizeof(REAL32);
				if( ui32ReadPos >= dwBytes ) return FALSE;
				
				ui32SubReadPos	= 0;
				while ( ui32SubReadPos < ui32DataSize ) // all sub head
				{
					pSubHead	= (N_GAME_SUBHEAD*)&pPacket[ ui32ReadPos + ui32SubReadPos ];
					
					// check sub packet index
					if( ! s_CheckIndex( pSubHead->_Type, pSubHead->_Index)) return FALSE;
					
					// check sub packet size check 
					if( pSubHead->_Code < ui32DataSize ) return TRUE; // 구멍: 훈련전에서 _Code를 size가 아닌 다른 용도로 사용하고 있음.

					if( ! s_CheckCode ( pSubHead->_Type, pSubHead->_Code,  (UINT16)(ui32DataSize - ui32SubReadPos) )) return FALSE;
					
					ui32SubReadPos += pSubHead->_Code;
				}

				ui32ReadPos += ui32SubReadPos;			// 누적 sub size
				ui32ReadPos += sizeof( N_GAME_RELAY );	// relay 정보
			}
			break;
		case UDPPROTOCOL_DSERVER_VERSION:
			{
				ui32DataSize = pHeader->_Size - sizeof( N_GAME_HEADER );
				
				if( ui32DataSize != sizeof( N_GAME_VERSION ) ) return FALSE;
				ui32ReadPos += sizeof( N_GAME_VERSION );
			}
			break;
		case UDPPROTOCOL_DSERVER_AT:
			{
				ui32DataSize = pHeader->_Size - sizeof( N_GAME_HEADER );
				
				if( ui32DataSize != sizeof( N_GAME_AT ) ) return FALSE;
				ui32ReadPos += sizeof( N_GAME_AT );
			}
			break;
		case UDPPROTOCOL_SERVER_WITHDRAW_GROUP_REQ:
			{	// 사용되지 않는 프로토콜인데 클라이언트가 보내고 있음
				// 로그 없이 false 리턴
				return FALSE; 
			}
		
		case UDPPROTOCOL_CS_STAGEINFO:				// unexpected
		default:
			DSWARNING("[ReadContext] protocol %x is not supported!\n", ui8Protocol);
			return FALSE;
			break;
		}
	}
	
	return TRUE;
}

BOOL GetPacketAt()
{
	if( bPacketAt )
	{
		if( bPacketOut ) bPacketOut = FALSE;
		else			 bPacketOut = TRUE;

		return bPacketOut;
	}
	else return FALSE;
}