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
	I3_EXPORT_CLASS_DEFINE( i3NetworkTCPSocket, i3ElementBase );
protected:
	SOCKET				m_Socket = INVALID_SOCKET;
	WSAEVENT			m_hEvent = INVALID_HANDLE_VALUE;

	void*				m_pAgent = nullptr;
	CallbackRoutine		m_pCallbackFunc = nullptr;

	char 				m_ReceiveBuffer[PACKETBUFFERSIZE] = { 0 };
	INT32				m_ReceivedPacketSize = 0;

	char				m_SendBuffer[PACKETBUFFERSIZE] = { 0 };
	INT32				m_nSendDataLen = 0;    // length of data to send
	INT32				m_nBytesSent = 0;      // bytes sent so far

public:
	I3SOCKETADDR		m_LocalAddr;

public:
	i3NetworkTCPSocket();
	virtual ~i3NetworkTCPSocket();

	virtual bool OnCreate( char * pszAddress, UINT16 nPort, CallbackRoutine CallBack = nullptr, void* pAgent = nullptr );
	virtual bool OnDestroy(void);

	virtual void OnReceive(void); 
	virtual bool OnSend(void);
	
	virtual void PacketParsing(i3NetworkPacket  * pPacket);
	
	void		SelectEvent(void); 
	virtual INT32 SendMessage(i3NetworkPacket * pPacket); 
	void		GetName( SOCKADDR_IN * plocaladdr = nullptr );
};

#endif
