#include "pch.h"
#include "Watch.h"
CWatch * g_pWatch = NULL;

I3_CLASS_INSTANCE( CWatch, i3ElementBase );

CWatch::CWatch(void)
{

}

CWatch::~CWatch(void)
{
	Destroy();
}

BOOL	CWatch::Create(void)
{
	m_bUseInterNet	= TRUE;

	return TRUE;
}

BOOL	CWatch::Destroy(void)
{
	DestroyUDPSocket();

	return TRUE;
}

void	CWatch::DestroyUDPSocket( void)
{
	if(m_pUDPClientSocket != NULL)
	{
		I3NOTICE( "m_pUDPClientSocket->CloseSocket()\n" );
		m_pUDPClientSocket->CloseSocket();
	}

	I3NOTICE( "m_pUDPClientSocket Release\n" );
	I3_SAFE_RELEASE( m_pUDPClientSocket );
}

void	CWatch::CheckServerIPUDPSocket( UINT32 ui32IP )
{
	char	sipaddr[128]				= WATCH_DOMAIN;
	UINT32	ui32UdpIp					= 0;
	UINT32	ui32RecvHeartBit2TotalCount	= 0;				// HeartBit2 

	if( _IsDottedAddressIP( sipaddr ) )
	{
		ui32UdpIp = inet_addr(sipaddr);
	}
	else
	{
		ui32UdpIp = inet_addr( _ConvertDomainName2IP(sipaddr));
	}

	m_pUDPClientSocket = CWatchUDPSocket::NewObject();
	if( NULL == m_pUDPClientSocket )	return;

	m_pUDPClientSocket->Create( WATCH_CONTROL_PORT );

	PACKET_UDP_CONTROL_INFO		PacketReq;

	char				SendBuffer[ MAX_UDP_WATCH_PACKET_SIZE ] = {0,};
	CWatchUDPSocket *	pUDPSocket = m_pUDPClientSocket;

	UINT32	ui32LocalIP = 0;	// Local IP
	INT32	i32Count;
	char * pIPList = i3Net::GetMyAddressList( &i32Count);

	if( pIPList != NULL )
	{
		ui32LocalIP = *(UINT32*)(&pIPList[0]);
		I3MEM_SAFE_FREE( pIPList);
	}

	SYSTEMTIME		SystemTime;
	::GetLocalTime( &SystemTime );
	REAL32	r32ValTime		= (REAL32)(
		(SystemTime.wMonth * 1000000)+ 
		(SystemTime.wDay * 10000)+ 
		(SystemTime.wHour * 100)+ 
		(SystemTime.wMinute) );	

	// HeartBit2 
	for( INT32 Loop = 0; Loop < MAX_SERVER_COUNT; Loop++ )
	{
		if( 0 < g_pControl->m_aui32RecvHeartBit2Count[Loop] )
		{ 
			ui32RecvHeartBit2TotalCount += g_pControl->m_aui32RecvHeartBit2Count[Loop];
		}
	}

	// Header
	PacketReq.m_Header.Reset();
	PacketReq.m_Header.m_ui8ProtocolID			= (UINT8)UDP_WATCH_CONTROL_INFO;
	PacketReq.m_Header.m_ui16Size				= sizeof(PACKET_UDP_CONTROL_INFO);
	PacketReq.m_Header.m_ui8Type				= (UINT8)UDP_CONTROL_TYPE;
	PacketReq.m_Header.m_fTimeStampe			= r32ValTime;
	PacketReq.m_Header.CopyToPacket( SendBuffer );

	// Server Version
	PacketReq.m_WatchVersion.m_ui8Version		= (UINT8)VER_WATCH_PACKET_S;
	PacketReq.m_WatchVersion.m_ui32GameCode		= (UINT32)GAME_CORD_PB; 
	PacketReq.m_WatchVersion.m_ui32LocaleCode	= (UINT32)LOCALE_NUMBER;
	PacketReq.m_WatchVersion.m_ui32MyIP			= ui32LocalIP;
	PacketReq.m_WatchVersion.m_ui32ServerIP		= ui32IP;
	PacketReq.m_WatchVersion.CopyToPacket( &(SendBuffer[ sizeof(UDP_WATCH_HEADER) ]) );

	// Server Info
	PacketReq.m_WatchControlInfo.m_ui32MaxCCU_HourMinSec		= g_pControl->m_ui32CCUMaxTime_Day;
	PacketReq.m_WatchControlInfo.m_ui32MaxCCU					= g_pControl->m_i32CCUMaxCount_Day;
	PacketReq.m_WatchControlInfo.m_ui32CCU						= g_pControl->m_i32CCUNowCount;
	PacketReq.m_WatchControlInfo.m_ui32RecvHeartBit2TotalCount	= ui32RecvHeartBit2TotalCount;
	PacketReq.m_WatchControlInfo.CopyToPacket( &(SendBuffer[ sizeof(UDP_WATCH_HEADER) + sizeof(WATCH_VERSION) ]) );

	pUDPSocket->NomalSend( SendBuffer, MAX_UDP_WATCH_PACKET_SIZE, sizeof( PACKET_UDP_CONTROL_INFO ), ui32UdpIp, WATCH_CONTROL_PORT );

	I3NOTICE( "m_pUDPClientSocket->CloseSocket()\n" );
	m_pUDPClientSocket->CloseSocket();
	I3NOTICE( "m_pUDPClientSocket Release\n" );
	I3_SAFE_RELEASE( m_pUDPClientSocket );
}

bool	CWatch::_IsDottedAddressIP( const char* pAddr ) const
{
	char buffer[64] = {0};
	const char* pAddress = pAddr;

	while( *pAddress != '\0' )
	{
		if( *pAddress != '.' ) ::strncat( buffer, pAddress, 1 );

		if( *(pAddress+1) == '.' || *(pAddress+1) == '\0' )
		{
			if( ::_ttoi( buffer ) >= 0xff ) return false;
			else i3String::Copy( buffer, "", 64 );
		}

		pAddress++;
	}

	if( i3String::Length(pAddr) < 7 ) return false;

	if( INADDR_NONE == ::inet_addr(pAddr) ) return false;

	return true;
}

const char*	CWatch::_ConvertDomainName2IP( const char* pAddr )
{
	if( FALSE == m_bUseInterNet ) { return WATCH_IP; }

	struct hostent* pRemoteHost = NULL;

	pRemoteHost = gethostbyname( pAddr );
	if( pRemoteHost == NULL )
	{		
		I3WARN(" ## Fail to Convert DomainName to Dotted Address IP!!  \n ");
		I3WARN(" ## Invalid Domain or IP : %s\n", pAddr );

		I3WARN("Invalid DomainName or IP!!");

		//////////////////////////////////////////////////////////////////////////
		// Host 등록 절차가 필요합니다.( 현재 강제로 WATCH_IP 전송됨 )
		//////////////////////////////////////////////////////////////////////////
		m_bUseInterNet = FALSE;
		//////////////////////////////////////////////////////////////////////////
		return NULL;
	}
	else
	{
		const char* pIP = ::inet_ntoa( *(in_addr*)*pRemoteHost->h_addr_list );

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)

		I3WARN(" ## official name: %s\n", pRemoteHost->h_name);
		I3WARN(" ## Alternate Names: %s\n", pRemoteHost->h_aliases);
		switch( pRemoteHost->h_addrtype )
		{
		case AF_INET:
			I3WARN(" ## address type: AF_INET \n");
			break;
		case AF_INET6:
			I3WARN(" ## address type: AF_INET6 \n");
			break;
		case AF_NETBIOS:
			I3WARN(" ## address type: AF_NETBIOS \n");
			break;
		}

		I3WARN(" ## IP Address : %s\n", pIP);

#endif

		return pIP;
	}
}