#include "pch.h"
#include "Packet.h"

CPacket::CPacket()	
{
	Clear(); 
	return; 
}

CPacket::CPacket( PROTOCOL ProtocolID )
{
	Clear(); 
	SetProtocol( ProtocolID ); 	
	return;
}

CPacket::~CPacket()
{
	return; 
}
	
void CPacket::ReadData( void* buffer, INT32 size)
{
	if( 0 == size )
	{
		return;
	}

	if( m_pReadPosition + size > m_pEndOfDataField )
	{
		return;
	}

	memcpy( buffer, m_pReadPosition, size );
	m_pReadPosition	+= size;

	return; 
}


void CPacket::WriteData( const void* buffer, UINT16 size)
{
	if( 0 == size )
	{
		return;
	}

	if( m_pWritePosition + size > m_pEndOfDataField )
	{
		return;
	}

	memcpy( m_pWritePosition, buffer, size );
	m_pWritePosition	+= size;
	m_Size				+= size;

	*m_PacketHeader.pDataSize = (UINT16) m_Size; 

	return; 
}  

void CPacket::CopyToBuffer(const char * buffer, INT32 Size, INT32 StartPos)
{
	if(Size > (PACKETBUFFERSIZE - 1) )
	{
		return;
	}

	Clear();
	memcpy( m_pPacketBuffer, &buffer[StartPos], Size );
	m_Size = Size;

	return; 
}

//void CPacket::Encript(UINT32 c)
//{
//	UINT16 Size = *m_PacketHeader.pDataSize; 
//	if( Size & 0x8000 )
//	{
//		return; 
//	}
//
//	BitRotateEncript( (UINT8*)&m_pPacketBuffer[2], (Size + 2), c);
//
//	*m_PacketHeader.pDataSize = Size | 0x8000 ; 
//
//	return; 
//}
//
//BOOL CPacket::Decript(UINT32 c)
//{	
//	UINT16 Size = *m_PacketHeader.pDataSize; 
//
//	if( Size & 0x8000 )
//	{
//		Size  = Size & 0x7FFF;
//
//		if( Size > PACKETBUFFERSIZE)
//		{
//			I3TRACE("[CPacket::Encript]Error Decript \n"); 
//			return FALSE;
//		}
//
//		BitRotateDecript( (UINT8*)&m_pPacketBuffer[2], (Size + 2) ,  c);
//		*m_PacketHeader.pDataSize = Size; 
//	}
//	else
//	{
//		return FALSE; 
//	}
//
//	return TRUE; 
//}

void CPacket::Clear(void)
{
	::ZeroMemory( m_pPacketBuffer, PACKETBUFFERSIZE );

	m_PacketHeader.pDataSize	= ( UINT16 * )&m_pPacketBuffer[0];	//  packetSize size = 2
	m_PacketHeader.pProtocolID	= ( UINT16 * )&m_pPacketBuffer[2];	//  protocolID size	= 2
	m_pDataField				= &m_pPacketBuffer[PACKETHEADERSIZE];
	m_pReadPosition				= m_pWritePosition = m_pDataField;
	m_pEndOfDataField			= &m_pPacketBuffer[PACKETBUFFERSIZE - 1];
	m_Size						= 0;
	return; 
}
