#ifndef __I3NETWORKTCPSOCKET_H__
#define __I3NETWORKTCPSOCKET_H__

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// New TCPSocket
// make : 2006. 12. 20. Á¤¼ø±¸
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "i3Base.h"

class i3NetworkPacket; 
typedef	void ( *CallbackRoutine )( i3NetworkPacket * packet );

class I3_EXPORT_NETWORK i3NetworkTCPSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkTCPSocket );
protected:
	SOCKET				m_Socket; 
	WSAEVENT			m_hEvent;
	CallbackRoutine		m_pCallbackFunc;

	char 				m_ReceiveBuffer[PACKETBUFFERSIZE];
	INT32				m_ReceivedPacketSize;

	char				m_SendBuffer[PACKETBUFFERSIZE];
	INT32				m_nSendDataLen;    // length of data to send
	INT32				m_nBytesSent;      // bytes sent so far

public:
	I3SOCKETADDR		m_LocalAddr;

public:
	i3NetworkTCPSocket();
	virtual ~i3NetworkTCPSocket();

	virtual BOOL OnCreate( char * pszAddress, UINT16 nPort, CallbackRoutine CallBack = NULL );
	virtual BOOL OnDestroy(void);

	virtual void OnReceive(void); 
	virtual BOOL OnSend(void);
	
	virtual void PacketParsing(i3NetworkPacket  * pPacket);
	
	void		SelectEvent(void); 
	virtual INT32 SendMessage(i3NetworkPacket * pPacket); 
	void		GetName( SOCKADDR_IN * plocaladdr = NULL );
};

#endif
