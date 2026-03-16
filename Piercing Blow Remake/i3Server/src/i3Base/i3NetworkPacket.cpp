#include "i3CommonType.h"
#include "i3NetworkPacket.h"
#include "i3CRC32.h"

I3_CLASS_INSTANCE(i3NetworkPacket, i3ElementBase); 

i3NetworkPacket::i3NetworkPacket()	
{
	Clear(); 
	return; 
}

i3NetworkPacket::i3NetworkPacket( PROTOCOL ProtocolID )
{
	Clear(); 
	SetProtocol( ProtocolID ); 	
	return;
}

i3NetworkPacket::~i3NetworkPacket()
{
	return; 
}

void i3NetworkPacket::ReadData(void* buffer, UINT32 size)
{
	if (0 == size)
	{
		//I3TRACE("[i3NetworkPacket::ReadData] Size Zero\n");
		return;
	}

	if (m_pReadPosition + size > m_pEndOfDataField)
	{
		I3TRACE("[i3NetworkPacket::ReadData] Error\n");
		return;
	}

	i3mem::Copy(buffer, m_pReadPosition, size);
	m_pReadPosition += size;

	return;
}

BOOL i3NetworkPacket::WriteData(const void* buffer, UINT32 size)
{
	if (0 == size)
	{
		//I3TRACE("[i3NetworkPacket::WriteData] Size Zero\n");
		return FALSE;
	}

	if (m_pWritePosition + size > m_pEndOfDataField)
	{
		I3TRACE("[i3NetworkPacket::WriteData] Write Failed \n");
		return FALSE;
	}

	i3mem::Copy(m_pWritePosition, buffer, size);
	m_pWritePosition += size;
	m_Size += size;

	*m_PacketHeader.pDataSize = (UINT32)m_Size;

	return TRUE;
}

void i3NetworkPacket::WriteData(UINT32 ui32Pos, const void* buffer, UINT32 size)
{
	if (0 == size)	return;

	if (&m_pPacketBuffer[PACKETHEADERSIZE + ui32Pos] + size > m_pEndOfDataField)
	{
		I3TRACE("[i3NetworkPacket::WriteData] Write Failed \n");
		return;
	}

	UINT32 ui32NewSize = ui32Pos + size;
	if (m_Size < ui32NewSize)
	{	// 데이터 사이즈 변경이 일어났다면 사이즈 증가시켜줍니다.		
		m_pWritePosition = &m_pPacketBuffer[PACKETHEADERSIZE + ui32Pos];
		i3mem::Copy(m_pWritePosition, buffer, size);

		m_pWritePosition += size;
		m_Size = ui32NewSize;
	}
	else
	{
		i3mem::Copy(&m_pPacketBuffer[PACKETHEADERSIZE + ui32Pos], buffer, size);
	}

	return;
}

void i3NetworkPacket::CopyToBuffer(const char * buffer, INT32 Size, INT32 StartPos)
{
	if(Size > PACKETBUFFERSIZE )
	{
		I3TRACE("[i3NetworkPacket::CopyToBuffer] Packet Size Error \n"); 
		return; 
	}

	Clear();	
	i3mem::Copy( m_pPacketBuffer, &buffer[StartPos], Size); 
	m_Size = Size;

	return; 
}

BOOL i3NetworkPacket::Encript(UINT32 c)
{
	UINT16 Size = *m_PacketHeader.pDataSize; 
	if( Size & 0x8000 )
	{
		I3TRACE("[i3NetworkPacket::Encript]Error Encript \n"); 
		return FALSE; 
	}
	if( Size > PACKETDATASIZE )
	{
		I3TRACE("[i3NetworkPacket::Encript]Error Encript, Size Over \n"); 
		return FALSE;
	}

	BitRotateEncript( (UINT8*)&m_pPacketBuffer[PACKETENCRIPTSIZE], (Size + PACKETENCRIPTSIZE), c);

	*m_PacketHeader.pDataSize = Size | 0x8000 ; 

	return TRUE;
}

BOOL i3NetworkPacket::Decript(UINT32 c)
{	
	UINT16 Size = *m_PacketHeader.pDataSize; 

	if( FALSE == (Size & 0x8000) )
	{
		return FALSE;
	}

	Size  = Size & 0x7FFF;
	if( Size > PACKETDATASIZE )
	{
		I3TRACE("[i3NetworkPacket::Encript]Error Decript \n"); 
		return FALSE;
	}

	BitRotateDecript( (UINT8*)&m_pPacketBuffer[PACKETENCRIPTSIZE], (Size + PACKETENCRIPTSIZE) ,  c);
	*m_PacketHeader.pDataSize = Size; 

	return TRUE; 
}

void i3NetworkPacket::Clear(void)
{
	i3mem::FillZero( m_pPacketBuffer, PACKETBUFFERSIZE );

	m_PacketHeader.pDataSize	= ( UINT16 * )&m_pPacketBuffer[0];	//  packetSize size = 2
	m_PacketHeader.pProtocolID	= ( UINT16 * )&m_pPacketBuffer[2];	//  protocolID size	= 2
	m_pDataField				= &m_pPacketBuffer[PACKETHEADERSIZE];
	m_pReadPosition				= m_pWritePosition = m_pDataField;
	m_pEndOfDataField			= &m_pPacketBuffer[PACKETBUFFERSIZE];
	m_Size						= 0;
	return; 
}
