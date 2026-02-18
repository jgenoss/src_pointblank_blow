#ifndef __i3NETWORKPACKET_H__
#define __i3NETWORKPACKET_H__

//패킷
#define COMPRESS_TYPE_NONE		0
#define COMPRESS_TYPE_RLE		1

/*----------------------------------------------------------
Name : Network Packet 
Desc : 네트워크 패킷을 구조화한 클래스 이다. 
메모리 카피를 하므로 속도는 빠르지 않다. 
* 패킷 파싱에서 꼬옥 사용할 필요는 없다. 차후 카피하는 부분을 빼거나 ..다른 방향으로 업데이트를 해야 한다. 
----------------------------------------------------------*/
class I3_EXPORT_NETWORK i3NetworkPacket : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NetworkPacket, i3ElementBase ); 
protected: 	
	//Packet Header 
	struct HEADER
	{
#ifdef __COMPRESS_RLE__
		UINT8	*		pCompressType = nullptr;
		UINT8	*		pEncriptType = nullptr;
#endif
		UINT16	*		pDataSize = nullptr;		
		UINT16	*		pProtocolID = nullptr;
	};

	HEADER		m_PacketHeader;	
	char		m_pPacketBuffer[PACKETBUFFERSIZE] = { 0 };
	char	*	m_pDataField = nullptr;
	char	*	m_pReadPosition = nullptr;
	char	*	m_pWritePosition = nullptr;
	char	*	m_pEndOfDataField = nullptr;
	INT32		m_Size = 0;

protected: 
#ifdef __COMPRESS_RLE__
	bool	CompressRLE(void); 
	bool	DecompressRLE(void); 
#endif

public :
	i3NetworkPacket( void);
	i3NetworkPacket( PROTOCOL ProtocolID );
	virtual ~i3NetworkPacket(void); 

	void		Clear(void); 
	void		SetProtocol(PROTOCOL ProtocolID)	{ *m_PacketHeader.pProtocolID = ProtocolID; }

	//압축
	bool		Compress(UINT8 iType);	
	bool		Decompress(void);			

	//암호화
	bool		Encript(UINT32 c); 
	bool		Decript(UINT32 c); 

	INT32		GetPacketSize(void)					{ return ( GetDataFieldSize() + PACKETHEADERSIZE ); }	
	INT32		GetReceivedSize(void)				{ return m_Size; }
	PROTOCOL	GetProtocolID(void)					{ return * m_PacketHeader.pProtocolID; }
	UINT16		GetDataFieldSize(void)				{ return (*m_PacketHeader.pDataSize) & 0x7FFF ; }
	char	*	GetPacketBuffer(void)				{ return m_pPacketBuffer; } 
	const char	*	GetFieldBuffer(void)			{ return m_pDataField; }
	HEADER	* GetHeader(void)						{return &m_PacketHeader; }
	
	void		CopyToBuffer(const char * buffer, INT32 size, INT32 StartPos = 0);
	void		ReadData( void* buffer, INT32 size );
	void		ReadEmptyData(INT32 size); 
	void		WriteData( const void* buffer, UINT16 size );
}; 
#endif
