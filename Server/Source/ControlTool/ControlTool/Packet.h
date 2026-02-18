#pragma once

typedef		UINT16	PROTOCOL;

#define		PACKETBUFFERSIZE			8912
#define		PACKETHEADERSIZE			4
#define		SOCKET_HEAD_DATASIZE_SIZE	2

class CPacket
{
	//Packet Header 
	typedef struct
	{
		UINT16	*		pDataSize;		//Encript 0x80000
		UINT16	*		pProtocolID;
	}HEADER;

	HEADER		m_PacketHeader;	
	char		m_pPacketBuffer[PACKETBUFFERSIZE];
	char	*	m_pDataField;
	char	*	m_pReadPosition;
	char	*	m_pWritePosition;
	char	*	m_pEndOfDataField;
	INT32		m_Size;
public :
	CPacket( void);
	CPacket( PROTOCOL ProtocolID );
	virtual ~CPacket(void); 

	void		Clear(void); 
	void		SetProtocol(PROTOCOL ProtocolID){ *m_PacketHeader.pProtocolID = ProtocolID; }
	//void		Encript(UINT32 c); 
	//BOOL		Decript(UINT32 c); 
	INT32		GetPacketSize(void)				{ return ( GetDataFieldSize() + PACKETHEADERSIZE ); }	
	INT32		GetReceivedSize(void)			{ return m_Size; }
	PROTOCOL	GetProtocolID(void)				{ return * m_PacketHeader.pProtocolID; }
	UINT16		GetDataFieldSize(void)			{ return (*m_PacketHeader.pDataSize) & 0x7FFF ; }
	const char	*	GetPacketBuffer(void)			{ return m_pPacketBuffer; } 
	const char	*	GetFieldBuffer(void)			{ return m_pDataField; }
	
	void		CopyToBuffer(const char * buffer, INT32 size, INT32 StartPos = 0);
	void		ReadData( void* buffer, INT32 size );
	void		WriteData( const void* buffer, UINT16 size );
}; 