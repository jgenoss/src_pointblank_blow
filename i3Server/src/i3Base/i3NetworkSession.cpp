#include "i3CommonType.h"
#include "i3NetworkSession.h"
#include "i3NetworkPacket.h"
#include "i3String.h"
#include "i3Inet.h"
#include "i3CRC32.h"

I3_CLASS_INSTANCE(i3NetworkSession, i3ElementBase); 

i3NetworkSession::i3NetworkSession()
{
	m_IsActive			= FALSE; 
	m_Socket			= INVALID_SOCKET; 
	m_strIPString[0]	= 0;
	m_wstrIPString[0]	= 0;
	m_WorkThreadIdx		= -1; 
	m_ReceiveSize		= 0;

	m_ConIp				= 0;
	m_ConPort			= 0;

	m_SessionIdx		= 0;
}

i3NetworkSession::~i3NetworkSession()
{
	Destroy(); 
}

BOOL i3NetworkSession::Create()
{
	return TRUE;
}

void i3NetworkSession::Destroy()
{
	OnDisconnect();
}

BOOL i3NetworkSession::OnConnect(SOCKET Socket, struct sockaddr_in * pAddr)
{	
	m_Socket	= Socket; 

	//예외 처리 나올 가능성 없을듯 .
	i3Net::GetIPToSockA( pAddr, m_strIPString );
	i3Net::GetIPToSockW( pAddr, m_wstrIPString );

	//Set Real Adress
	m_ConIp			=	pAddr->sin_addr.s_addr; 
	m_ConPort		=	pAddr->sin_port;

	m_IsActive	= TRUE; 

	m_ReceiveSize	= 0; 

	return TRUE; 
}

BOOL i3NetworkSession::OnConnectInit()
{
	WaitPacketReceive( -1 );
	return true;
}

BOOL i3NetworkSession::OnDisconnect(BOOL bForceMainThread)
{	
	m_strIPString[0]	= '\0';
	m_wstrIPString[0]	= '\0';
	if(m_Socket != INVALID_SOCKET)
	{
		::shutdown( m_Socket, SD_BOTH);
		::closesocket( m_Socket );
		m_Socket = INVALID_SOCKET;
	}

	m_IsActive	= FALSE; 

	return TRUE;
}

void i3NetworkSession::Dispatch( DWORD Transferbytes )
{
	if( Transferbytes == 0 )return;

	DispatchReceive( Transferbytes );
}

void i3NetworkSession::DispatchReceive(DWORD Transferbytes)
{	
	m_ReceiveSize	= m_ReceiveSize + Transferbytes; 

	//1. 패킷의 사이즈가 패킷 버퍼 보다 클때 //절대로 나오면 않됨
	if( m_ReceiveSize >= PACKETBUFFERSIZE )
	{//Error ...에러 입니다. //로그를 남기는 것이 좋습니다. 
		I3ASSERT_0; 
		return;
	}
    
	INT32 iReadSize = 0;
	INT32 i32PacketRv;
	while( ( (m_ReceiveSize - iReadSize) >= PACKETHEADERSIZE ) )
	{		 
		i32PacketRv = PacketParsing( &m_ReceiveBuffer[iReadSize], (m_ReceiveSize - iReadSize) ); 
		if( 0 == i32PacketRv ) break;
		iReadSize = iReadSize + i32PacketRv; 
	}

	//만약 받은 패킷보다 더 읽으면 에러 // 이부분은 PacketParsing이 함수에서 처리를 해줘야 합니다. 
	if( iReadSize > m_ReceiveSize) 
	{
		m_ReceiveSize = 0;
		return;
		//I3ASSERT_0; 
	}
	 
	m_ReceiveSize = m_ReceiveSize - iReadSize;	
	//받은만큼 처리 못했으면. 버퍼 이동
	if( 0 != m_ReceiveSize )
	{		
		memmove( m_ReceiveBuffer, m_ReceiveBuffer + iReadSize, m_ReceiveSize );
	}    
	return; 
}
	
BOOL i3NetworkSession::WaitPacketReceive(INT32 i32Idx)
{
	if( m_Socket == INVALID_SOCKET )return FALSE;
	DWORD readBytes = 0;

	i3mem::FillZero( &m_OverlappedRecv, sizeof( OVERLAPPED2 ) );
	m_OverlappedRecv.flags = SESSION_ASYNCFLAG_RECEIVE;
	
	//이부분도 수정되어야 합니다. 
	if( !::ReadFile( ( HANDLE )m_Socket, &m_ReceiveBuffer[m_ReceiveSize], ( PACKETBUFFERSIZE - (m_ReceiveSize) ), &readBytes, ( LPOVERLAPPED )&m_OverlappedRecv ) )
	{
		DWORD lastError = ::GetLastError();
		if( lastError != ERROR_IO_PENDING && lastError != ERROR_SUCCESS )
		{
			i3Net::GetError("[i3NetworkSession::WaitPacketReceive]");
			return FALSE;
		}
	}	

	return TRUE; 
}

BOOL i3NetworkSession::SendPacketMessage( i3NetworkPacket * pPacket )
{	
	DWORD writtenBytes = 0;

	i3mem::FillZero( &m_OverlappedSend, sizeof( OVERLAPPED2 ) );
	m_OverlappedSend.flags = SESSION_ASYNCFLAG_SEND;
	
	if( !::WriteFile( ( HANDLE )m_Socket, pPacket->GetPacketBuffer(), pPacket->GetPacketSize(), &writtenBytes, ( LPOVERLAPPED )&m_OverlappedSend ) )
	{
		DWORD lastError = ::GetLastError();
		if( lastError != ERROR_IO_PENDING && lastError != ERROR_SUCCESS )
		{
			i3Net::GetError("[i3NetworkSession::SendMessage]");			
			return FALSE;
		}
	}

	return TRUE; 
}

BOOL i3NetworkSession::SendMessage(i3NetworkPacket * pPacket)
{
	if( !m_IsActive)return FALSE; 
	if( m_Socket == INVALID_SOCKET )return FALSE;
	DWORD writtenBytes = 0;

	i3mem::FillZero( &m_OverlappedSend, sizeof( OVERLAPPED2 ) );
	m_OverlappedSend.flags = SESSION_ASYNCFLAG_SEND;
	
	if( !::WriteFile( ( HANDLE )m_Socket, pPacket->GetPacketBuffer(), pPacket->GetPacketSize(), &writtenBytes, ( LPOVERLAPPED )&m_OverlappedSend ) )
	{
		DWORD lastError = ::GetLastError();
		if( lastError != ERROR_IO_PENDING && lastError != ERROR_SUCCESS )
		{
			i3Net::GetError("[i3NetworkSession::SendMessage]");			
			return FALSE;
		}
	}

	return TRUE; 
}

void i3NetworkSession::DataEncript( UINT32 ui32Key1, UINT32 ui32Key2, UINT64* pui64Value )
{
	UINT8	ui8Encrypt[8];

	ui8Encrypt[0] = (UINT8)((((ui32Key1>>24)	& 0xF0))	| ((ui32Key2>>24)	& 0xF0)>>4);
	ui8Encrypt[1] = (UINT8)((((ui32Key1>>8)		& 0x0F)<<4)	| ((ui32Key2>>16)	& 0x0F));
	ui8Encrypt[2] = (UINT8)((((ui32Key1>>0)		& 0xF0))	| ((ui32Key2>>8)	& 0x0F));
	ui8Encrypt[3] = (UINT8)((((ui32Key1>>16)	& 0x0F)<<4)	| ((ui32Key2>>0)	& 0x0F));
	ui8Encrypt[4] = (UINT8)((((ui32Key1>>8)		& 0xF0))	| ((ui32Key2>>24)	& 0x0F));
	ui8Encrypt[5] = (UINT8)((((ui32Key1>>24)	& 0x0F)<<4)	| ((ui32Key2>>16)	& 0xF0)>>4);
	ui8Encrypt[6] = (UINT8)((((ui32Key1>>0)		& 0x0F)<<4)	| ((ui32Key2>>0)	& 0xF0)>>4);
	ui8Encrypt[7] = (UINT8)((((ui32Key1>>16)	& 0xF0))	| ((ui32Key2>>8)	& 0xF0)>>4);

	BitRotateEncript( ui8Encrypt, sizeof( UINT64 ), 3 );

	i3mem::Copy( pui64Value, ui8Encrypt, sizeof(UINT64) );
}

void i3NetworkSession::DataDecript( UINT64 ui64Value, UINT32* pui32Key1, UINT32* pui32Key2 )
{
	UINT8	ui8Encrypt[8];
	UINT8	ui8Decrypt1[4];
	UINT8	ui8Decrypt2[4];

	i3mem::Copy( ui8Encrypt, &ui64Value, sizeof(UINT64) );
	BitRotateDecript( ui8Encrypt, sizeof( UINT64 ), 3 );

	ui8Decrypt1[0] = ((ui8Encrypt[6]&0xF0)>>4) | (ui8Encrypt[2]&0xF0);
	ui8Decrypt1[1] = ((ui8Encrypt[1]&0xF0)>>4) | (ui8Encrypt[4]&0xF0);
	ui8Decrypt1[2] = ((ui8Encrypt[3]&0xF0)>>4) | (ui8Encrypt[7]&0xF0);
	ui8Decrypt1[3] = ((ui8Encrypt[5]&0xF0)>>4) | (ui8Encrypt[0]&0xF0);

	ui8Decrypt2[0] = ((ui8Encrypt[6]&0x0F)<<4) | (ui8Encrypt[3]&0x0F);
	ui8Decrypt2[1] = ((ui8Encrypt[7]&0x0F)<<4) | (ui8Encrypt[2]&0x0F);
	ui8Decrypt2[2] = ((ui8Encrypt[5]&0x0F)<<4) | (ui8Encrypt[1]&0x0F);
	ui8Decrypt2[3] = ((ui8Encrypt[0]&0x0F)<<4) | (ui8Encrypt[4]&0x0F);

	i3mem::Copy( pui32Key1, ui8Decrypt1,	sizeof(UINT32) );
	i3mem::Copy( pui32Key2, ui8Decrypt2,	sizeof(UINT32) );
}