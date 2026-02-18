#include "pch.h"
#include "UdpHolePunching.h"
#include "RoomManager.h"
#include "UserSession.h"
#include "LogFile.h"

CUdpHolePunching * g_pUDPHolePuncher = NULL;

I3_CLASS_INSTANCE( CUdpHolePunching, i3Thread ); 

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
CUdpHolePunching::CUdpHolePunching()
{

}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
CUdpHolePunching::~CUdpHolePunching()
{
	OnDestroy();
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CUdpHolePunching::OnCreate( INT32 Port , i3List * pUserList)
{	
	m_pUserList = pUserList; 

	//Create Recv Thread 
	m_pSocket = i3Udp::NewObject();
	m_pSocket->BindAny( Port );		
	m_bIsRun	= TRUE;
	
	return Create("UdpHolePunch", 0, 4096, 0, PRIORITY_LOW);
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
BOOL CUdpHolePunching::OnDestroy(void) 
{	
	m_bIsRun = FALSE;
	i3Thread::Suspend();
	i3Thread::Terminate();
	I3_SAFE_RELEASE( m_pSocket ); 
	return TRUE; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CUdpHolePunching::BeforeRunning( void * pUserData)
{
	i3mem::FillZero( m_ReceiveBuffer, sizeof( char ) * PACKETBUFFERSIZE );
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
UINT32 CUdpHolePunching::OnRunning( void * pUserData)
{
	/*
	UINT8	nCommand;
	UINT32	nSession;
	INT32	nDbIdx; 
	INT32	nReadPos;
	INT32	FromSize;
	I3SOCKETADDR	RecvAddr;
	SOCKADDR_IN	*	pAddr;
	CUserSession *	pSession;
	char			LogString[SERVER_STRING_COUNT]; 
	
	while( m_bIsRun )
	{
		nReadPos = 0;

		FromSize = sizeof( I3SOCKETADDR );

		m_ReceivedPacketSize = m_pSocket->Recv( m_ReceiveBuffer, PACKETBUFFERSIZE, 0, &RecvAddr, &FromSize );
		pAddr	= (SOCKADDR_IN *) &RecvAddr;

		//1. Packet Size 
		if( m_ReceivedPacketSize != 9 )
		{
			sprintf(LogString, "[HP]Wrong Size IP: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port) ); 
			WRITE_LOG(NULL, LogString); 			
			continue; 
		}

		//2. Packet Header
		i3mem::Copy( &nCommand, &m_ReceiveBuffer[ nReadPos ], sizeof( UINT8 ) );
		nReadPos += sizeof( UINT8 );		

		switch( nCommand )
		{
		case UDPPROTOCOL_COMMAND_START : 
			//3. SessionId
			i3mem::Copy( &nSession, &m_ReceiveBuffer[ nReadPos ], sizeof( UINT32 ) );
			nReadPos += sizeof( INT32 );
			if( nSession >= g_pConfig->m_SessionCount )
			{
				sprintf(LogString, "[HP]Wrong SessionId IP: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port)); 
				WRITE_LOG(NULL, LogString); 				
				continue; 
			}

			//4. DataBase Value 
			i3mem::Copy( &nDbIdx, &m_ReceiveBuffer[ nReadPos ], sizeof( INT32 ) );
			nReadPos += sizeof( INT32 );			

			pSession = (CUserSession *) m_pUserList->GetItem(nSession); 	//SendPacket 

			if(pSession->m_DBIdx != nDbIdx)
			{
				sprintf(LogString, "[HP]Wrong DbIdx IP: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port)); 
				WRITE_LOG(NULL, LogString); 				
				continue; 
			}

			if(pSession->SetUdpNetInfo( pAddr->sin_addr.s_addr, pAddr->sin_port))
			{
				SendToStart(pAddr->sin_addr.s_addr, pAddr->sin_port); 
			}
			else
			{
				sprintf(LogString, "[HP]Before LOGIN: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port)); 
				WRITE_LOG(NULL, LogString); 				
			}

			break; 
		case UDPPROTOCOL_COMMAND_ROOM  : 
			//3. SessionId
			i3mem::Copy( &nSession, &m_ReceiveBuffer[ nReadPos ], sizeof( UINT32 ) );
			nReadPos += sizeof( INT32 );
			if( nSession >= g_pConfig->m_SessionCount )
			{
				sprintf(LogString, "[HP]Wrong SessionId 2 IP: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port)); 
				WRITE_LOG(NULL, LogString); 				
				continue; 
			}

			//4. DataBase Value 
			i3mem::Copy( &nDbIdx, &m_ReceiveBuffer[ nReadPos ], sizeof( INT32 ) );
			nReadPos += sizeof( INT32 );	
			pSession = (CUserSession *) m_pUserList->GetItem(nSession); 	//SendPacket 

			if(pSession->m_DBIdx != nDbIdx)
			{
				sprintf(LogString, "[HP]Wrong DbIdx 2 IP: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port)); 
				WRITE_LOG(NULL, LogString); 				
				continue; 
			}

			if(pSession->SetUdpNetInfo( pAddr->sin_addr.s_addr, pAddr->sin_port, TRUE))
			{				
				SendToRoom(pAddr->sin_addr.s_addr, pAddr->sin_port); 				
			}
			else
			{
				sprintf(LogString, "[HP]Before LOGIN 2 IP: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port)); 
				WRITE_LOG(NULL, LogString); 				
			}

			break; 
		default:
			{
				sprintf(LogString, "[HP]Wrong Packet Header 2 IP: %p %d", pAddr->sin_addr.s_addr, htons(pAddr->sin_port)); 
				WRITE_LOG(NULL, LogString);
			}
			break; 
		}
	}

	*/
	return SERVER_VALUE_UNKNOWN;
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CUdpHolePunching::AfterRunning( void * pUserData)
{
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CUdpHolePunching::SendToStart( UINT32 Ip, UINT16 Port )
{
	/*
	UINT8	nCommand = UDPPROTOCOL_COMMAND_START;

	UINT16 nPort = htons(Port);
	m_pSocket->Send( (char*)&nCommand, 1, Ip, nPort);
	*/
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CUdpHolePunching::SendToRoom( UINT32 Ip, UINT16 Port )
{
	/*
	UINT8	nCommand = UDPPROTOCOL_COMMAND_ROOM;

	UINT16 nPort = htons(Port);
	m_pSocket->Send( (char*)&nCommand, 1, Ip, nPort);
	*/
}
