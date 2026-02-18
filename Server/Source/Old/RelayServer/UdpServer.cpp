#include "pch.h"
#include "LogFile.h"
#include "UdpServer.h"
#include "UdpWorker.h"

CUdpServer* g_pUdpServer = NULL;

I3_CLASS_INSTANCE(CUdpServer, i3ElementBase);


CUdpServer::CUdpServer()
{
	m_nSocketCount		= 0;
	m_ppSocket			= NULL;

	m_i32OldCountIdx	= 0;
	m_i32NewCountIdx	= 0;
	m_ui32Time			= 0;
}

CUdpServer::~CUdpServer()
{
	// Work Destory
	if(m_ppSocket )
	{
		for( INT32 i = 0 ; i < m_nSocketCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppSocket[i] );
		}

		I3MEM_SAFE_FREE( m_ppSocket );
	}
}

BOOL	CUdpServer::Create( INT32 nSocketCount, UINT32 wIP, UINT16* psPort )
{
	// Socket Create
	m_nSocketCount	= nSocketCount;

	m_ppSocket		= (CUdpWorker**)i3MemAlloc( sizeof(CUdpWorker*) * m_nSocketCount );
	if( NULL == m_ppSocket )										return FALSE;

	for( INT32 i = 0 ; i < m_nSocketCount ; i++ )
	{
		m_ppSocket[ i ] = CUdpWorker::NewObject();
		if( FALSE == m_ppSocket[ i ]->Create( i, wIP, psPort[i] ) ) return FALSE;
	}

	return TRUE;
}

void	CUdpServer::SetPacketCount_C()
{
	if( m_ui32Time + PACKETCOUNT_TIME > g_pConfig->GetServerTime() )	return;

	INT32 i32CountIdx = m_i32OldCountIdx = m_i32NewCountIdx;
	i32CountIdx++;
	i32CountIdx %= PACKETCOUNT_IDX;

	for( INT32 i = 0 ; i < m_nSocketCount ; i++ )
	{
		m_ppSocket[ i ]->SetPacketCount( i32CountIdx );
	}

	m_i32NewCountIdx = i32CountIdx;

	m_ui32Time = g_pConfig->GetServerTime();
}

UINT32	CUdpServer::GetPacketCount_C()
{
	UINT32 ui32PacketCount = 0;
	for( INT32 i = 0 ; i < m_nSocketCount ; i++ )
	{
		ui32PacketCount += m_ppSocket[ i ]->GetPacketCount( m_i32OldCountIdx );
	}
	return ui32PacketCount;
}

UINT16	CUdpServer::GetRestPort_C()
{
	UINT16 ui16Port = 0;
	UINT32 ui32Count = 0xFFFFFFFF;
	for( INT32 i = 0 ; i < m_nSocketCount ; i++ )
	{
		if( ui32Count > m_ppSocket[ i ]->GetPacketCount( m_i32OldCountIdx ) )
		{
			ui32Count	= m_ppSocket[ i ]->GetPacketCount( m_i32OldCountIdx );
			ui16Port	= m_ppSocket[ i ]->GetPort();
		}
	}

	return ui16Port;
}
