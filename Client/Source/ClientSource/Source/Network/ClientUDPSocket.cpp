#include "pch.h"
#include "ClientUDPSocket.h"
#include "GlobalVariables.h"
#include "SecurityHeader.h"

#include "../StageBattle/NetworkContext.h"

I3_CLASS_INSTANCE( ClientUDPSocket);//, i3ElementBase );

ClientUDPSocket::ClientUDPSocket()
{
	m_hSocket			= INVALID_SOCKET;
	i3mem::FillZero( &m_RecvAddr, sizeof( sockaddr ) );
	i3mem::FillZero( m_pszReciveBuffer, sizeof(m_pszReciveBuffer));
	m_nSendCount		= 0;
	m_nSendBytes		= 0;
	m_nRecvCount		= 0;
	m_nRecvBytes		= 0;
}

ClientUDPSocket::~ClientUDPSocket()
{
	Destroy();
}

bool ClientUDPSocket::Create( UINT16 nBindPort )
{
	SOCKADDR_IN	localAddr;
	
	m_hSocket	= ::socket( AF_INET, SOCK_DGRAM, 0 );

	if( m_hSocket == INVALID_SOCKET )
	{
		i3Net::GetError( "Socket create failed.\n" );
		return false;
	}
	
	// bind
	i3mem::FillZero( &localAddr, sizeof(SOCKADDR_IN) );
	localAddr.sin_family		= AF_INET;
	localAddr.sin_addr.s_addr	= htonl( INADDR_ANY );
	localAddr.sin_port			= htons( nBindPort );

	if( ::bind( m_hSocket, (struct sockaddr *) &localAddr, sizeof( localAddr ) ) == SOCKET_ERROR )
	{
		i3Net::GetError( "UDP bind failed.\n" );
		return false;
	}

	return true;
}

void ClientUDPSocket::CloseSocket()
{
	INT32	result;
	if( m_hSocket != INVALID_SOCKET )
	{
		result = ::shutdown( m_hSocket, SD_BOTH );
		if( result < 0 )
		{
			i3Net::GetError( "UDP shutdown.\n" );
			return;
		}

		result = ::closesocket( m_hSocket );
		if( result < 0 )
		{
			i3Net::GetError( "UDP close\n" );
			return;
		}

		m_hSocket = INVALID_SOCKET;
	}
}

bool ClientUDPSocket::Destroy()
{
	CloseSocket(); 	
	return true;
}

INT32 ClientUDPSocket::Send( char * pBuf, INT32 nSize, UINT32 nIP, UINT16 nPort )
{
	INT32	nRv;
	SOCKADDR_IN cast;
	cast.sin_family			= AF_INET;
	cast.sin_addr.s_addr	= nIP;
	cast.sin_port			= ::htons( nPort );

	if( g_Encryption(pBuf, (INT16)nSize, (INT16)(sizeof(char)*nSize)) < 0 )
	{
		NETTRACE( " Encryption invalid.\n " );
		return -1;
	}

	if( m_hSocket == INVALID_SOCKET )
	{
		NETTRACE( "UDP Socket invalid.\n" );
		return -1;
	}

//#if defined(_FROST_)
//	nRv = frostSendto( m_hSocket, pBuf, nSize, 0, (const sockaddr *)&cast, sizeof(sockaddr) );
//#else
	nRv = ::sendto( m_hSocket, pBuf, nSize, 0, (const sockaddr *)&cast, sizeof(sockaddr) );
//#endif
	if( nRv == SOCKET_ERROR )
	{
		i3Net::GetError( "UDPSocket Send()");
		I3PRINTLOG(I3LOG_NOTICE, "UDPSocket Send() IP %s Port :%d", inet_ntoa( cast.sin_addr ), cast.sin_port);
	}
	else if( nRv != nSize )
	{	
		char szTrace[ 256] = {0,};
		sprintf_s( szTrace, "UDP Send size not equal. %d, %d", nSize, nRv);
		I3PRINTLOG(I3LOG_NOTICE, szTrace);
	}

	m_nSendCount++;
	m_nSendBytes += nSize;

	return nRv;
}

INT32 ClientUDPSocket::SendBattleServer( char* pBuf, INT32 nSize, UINT8 cTargetIdx )
{
	// 릴레이 패킷이 아니면, 추가 정보를 덧붙인다.
	N_GAME_HEADER* pHeader = (N_GAME_HEADER*)pBuf;

	N_GAME_RELAY *pRelay = (N_GAME_RELAY*)(pBuf + pHeader->_Size);
	pRelay->wGroupIdx	= NetworkContext::i()->relayGroupId;
	pRelay->wEncryptKey	= NetworkContext::i()->RelayEncryptKey;
	pRelay->cTargetIdx	= cTargetIdx;
	

	pHeader->_Size += sizeof(N_GAME_RELAY);
	nSize = pHeader->_Size;

	INT32 Rv = Send(pBuf, nSize, NetworkContext::i()->relayServerIp, NetworkContext::i()->relayServerPort);

	return Rv;
}

INT32 ClientUDPSocket::Recv( void )
{
	INT32	len	= sizeof( sockaddr );
	INT32	nRecvSize;

//#if defined(_FROST_)
//	nRecvSize = frostRecvfrom( m_hSocket, m_pszReciveBuffer, UDP_RECV_PACKET_SIZE, 0, &m_RecvAddr, &len );
//#else
	nRecvSize = ::recvfrom( m_hSocket, m_pszReciveBuffer, UDP_RECV_PACKET_SIZE, 0, &m_RecvAddr, &len );
//#endif

	if( g_Decryption(m_pszReciveBuffer, (INT16)nRecvSize, sizeof(m_pszReciveBuffer)) < 0 )
	{
		NETTRACE( " Decryption invalid.\n " );
		return -1;
	}

	m_nRecvCount++;
	m_nRecvBytes += nRecvSize;

	return nRecvSize;
}


