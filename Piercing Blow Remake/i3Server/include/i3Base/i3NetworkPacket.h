#ifndef __i3NETWORKPACKET_H__
#define __i3NETWORKPACKET_H__

#define	PACKETENCRIPTSIZE			2

// 패킷 헤더 사이즈
#define SOCKET_HEAD_SIZE			4
// 패킷의 사이즈의 위치
#define SOCKET_HEAD_DATASIZE_SIZE	2

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

/*----------------------------------------------------------
Name : Network Packet 
Desc : 네트워크 패킷을 구조화한 클래스 이다. 
메모리 카피를 하므로 속도는 빠르지 않다. 
* 패킷 파싱에서 꼬옥 사용할 필요는 없다. 차후 카피하는 부분을 빼거나 ..다른 방향으로 업데이트를 해야 한다. 
----------------------------------------------------------*/
class I3_EXPORT_BASE i3NetworkPacket : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkPacket ); 
protected: 	
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
	i3NetworkPacket( void);
	i3NetworkPacket( PROTOCOL ProtocolID );
	virtual ~i3NetworkPacket(void); 

	void		Clear(void); 
	void		SetProtocol(PROTOCOL ProtocolID){ *m_PacketHeader.pProtocolID = ProtocolID; }
	BOOL		Encript(UINT32 c); 
	BOOL		Decript(UINT32 c); 
	INT32		GetPacketSize(void)				{ return ( GetDataFieldSize() + PACKETHEADERSIZE ); }	
	INT32		GetReceivedSize(void)			{ return m_Size; }
	PROTOCOL	GetProtocolID(void)				{ return * m_PacketHeader.pProtocolID; }
	UINT16		GetDataFieldSize(void)			{ return (*m_PacketHeader.pDataSize) & 0x7FFF ; }
	char	*	GetPacketBuffer(void)			{ return m_pPacketBuffer; } 
	char	*	GetFieldBuffer(void)			{ return m_pDataField; }
	
	void		CopyToBuffer(const char * buffer, INT32 size, INT32 StartPos = 0);
	void		ReadData(void* buffer, UINT32 size);
	BOOL		WriteData(const void* buffer, UINT32 size);
	void		WriteData(UINT32 ui32Pos, const void* buffer, UINT32 size);	
}; 
#endif
