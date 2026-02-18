#include "pch.h"
#include "ModuleNet_Server.h"

I3_CLASS_INSTANCE( CModuleNet_Server, i3NetworkClientSocket2 );

#define SEND_MAX_PACKET_COUNT		10

CModuleNet_Server::CModuleNet_Server()
{
	m_Socket		= INVALID_SOCKET; 
	m_pRecvBuffer	= NULL;
	m_pSendBuffer	= NULL;
}

CModuleNet_Server::~CModuleNet_Server()
{
	Destroy();
}

BOOL CModuleNet_Server::Create( UINT32 iServerIp, i3RingBuffer *pRecvBuffer, i3RingBuffer* pSendBuffer )
{
	m_ui32ServerIP		= iServerIp;
	m_pRecvBuffer		= pRecvBuffer;
	m_pSendBuffer		= pSendBuffer;
	m_ui32WorkingIdx	= 0; 

	m_i32LogFileHour	= i3ThreadTimer::LocalTime().GetHour();
	m_ui32SendCount		= 0;
	m_ui32RecvCount		= 0;

	return TRUE; 
}

void CModuleNet_Server::Destroy()
{
	i3NetworkClientSocket2::OnDestroy();
}

void CModuleNet_Server::Dump(void)
{
	if(m_i32LogFileHour != i3ThreadTimer::LocalTime().GetHour() )
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Network Packet Recv = %d Send = %d", m_ui32RecvCount, m_ui32SendCount);

		m_ui32SendCount		= 0;
		m_ui32RecvCount		= 0;
		m_i32LogFileHour	= i3ThreadTimer::LocalTime().GetHour();
	}
	return; 
}

INT32 CModuleNet_Server::PacketParsing(char * pPacket, INT32 i32PacketSize)
{
	//예외 처리 1: [정상]패킷 사이즈가 헤더보다 작으면 패킷을 기다린다. 
	if( i32PacketSize < SIA_PACKET_HEADER_SIZE )	return 0;

	NET_PACKET_BUFFER * pBuffer = (NET_PACKET_BUFFER *)pPacket; 

	//예외 처리 2: [비정상]패킷을 사이즈와 헤더를 검사해서 패킷이 정상인지를 확인합니다. 
	if( ( pBuffer->_ui16TotalSize - pBuffer->_ui16DataSize ) != SIA_PACKET_HEADER_SIZE)
	{
		// 이곳에서 로그를 보낸다. 		
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Packet Parsing Error TotalSize = %d TotalDatasize = %d ProtoclID = %d", pBuffer->_ui16TotalSize, pBuffer->_ui16DataSize, pBuffer->_ui16Protocol);
		OnDestroy();
		//return 0;
	}

	if( i32PacketSize < pBuffer->_ui16TotalSize )return 0;
	
	//예외 처리 3: [비정상] 패킷 사이즈가 너무 크다. 
	if( pBuffer->_ui16TotalSize > sizeof(NET_PACKET_BUFFER))
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Packet size Error TotalSize = %d TotalDatasize = %d ProtoclID = %d", pBuffer->_ui16TotalSize, pBuffer->_ui16DataSize, pBuffer->_ui16Protocol);
		OnDestroy();
		//return 0;
	}

	void * pRingBuffer = m_pRecvBuffer->PushPointer();
	if(NULL != pRingBuffer)
	{
		i3mem::Copy( pRingBuffer, pBuffer, pBuffer->_ui16TotalSize);
		m_pRecvBuffer->PushPointerIdx(); 
	}
	else  
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO, L"Packet Input Buffer Full");
	}

	m_ui32RecvCount++;

	return pBuffer->_ui16TotalSize; 
}

BOOL CModuleNet_Server::Send(void)
{
	//한번에 너무 많은 패킷을 보내지 않는다. 
	INT32 i32SendCount = m_pSendBuffer->GetBufferCount(); 
	if( i32SendCount > SEND_MAX_PACKET_COUNT) i32SendCount = SEND_MAX_PACKET_COUNT;

	for(INT32 i = 0; i < i32SendCount; i++)
	{
		NET_PACKET_BUFFER * pBuffer = (NET_PACKET_BUFFER *)m_pSendBuffer->Pop();
		if( pBuffer->_ui16TotalSize != SendPacketMessage((char*)pBuffer, pBuffer->_ui16TotalSize) )
		{
			g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_NO,  L"Send Fail" );
		}
		m_pSendBuffer->PopIdx();
	}

	m_ui32SendCount += i32SendCount;

	return TRUE; 
}
