#ifndef _UdpBuffer_h
#define _UdpBuffer_h

class CUdpBuffer: public i3ElementBase
{
	I3_CLASS_DEFINE(CUdpBuffer);

	char			m_pBuffer[ UDP_SEND_PACKET_SIZE ];

	sockaddr_in		m_SocketAddress;
	INT32			m_nSocketAddressSize;

	UINT16			m_sReadPosition;
	UINT16			m_sWritePosition;

	N_GAME_HEADER*	m_pHead;
	N_GAME_RELAY*	m_pRelay;

public:
	CUdpBuffer();
	~CUdpBuffer();				// LPOVERLAPPED로 케스팅하기위해 non virtual로 선언

	void 			InitReceive(void);	
	void 			InitSend(void);

	void			SetAddress(UINT32 wIP, UINT16 sPort);

	void			ReadData( void* pBuffer, UINT16 sSize );
	void			WriteData( void* pBuffer, UINT16 snSize );

	void			ReadAppendData();	
	void			CopyBufferFrom( CUdpBuffer* pSource );

	void			SetUdpProtocolId(UINT8 cUdpProtocolId )			{	m_pBuffer[0] = cUdpProtocolId;			};
	UINT8			GetUdpProtocolId(void)							{	return m_pBuffer[0];					};	

	UINT16			GetBufferSize(void)								{	return ((N_GAME_HEADER*)m_pBuffer)->_Size;		};
	char*			GetBuffer()										{	return m_pBuffer;						};
	sockaddr_in*	GetAddress()									{	return &m_SocketAddress;				};
	INT32*			GetAddressSize()								{	return &m_nSocketAddressSize;			};

	UINT32			GetIp(void)										{	return m_SocketAddress.sin_addr.s_addr;	};
	UINT16			GetPort(void)									{	return m_SocketAddress.sin_port;		};

	UINT16			GetReadPosition()								{	return m_sReadPosition;					};
	UINT16			GetWritePosition()								{	return m_sWritePosition;				};

	// ReadAppendData 를 호출해야 사용 가능합니다.
	UINT32			GetGroupIdx()									{	return m_pRelay->wGroupIdx;				};
	UINT32			GetEncryptKey()									{	return m_pRelay->wEncryptKey;			};
	UINT8			GetTargetIdx()									{	return m_pRelay->cTargetIdx;			};
	UINT8			GetSlotIdx()									{	return m_pHead->_Index;					};
};

#endif