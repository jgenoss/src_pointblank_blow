#ifndef _i3NetworkClientSocket_h
#define _i3NetworkClientSocket_h

#define CLIENT_SOCKET_BUFFER_SIZE	(PACKETBUFFERSIZE * 3)

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

class i3NetworkPacket;

typedef	INT32 ( *CallbackRoutine )( void* pAgent, i3NetworkPacket* pPacket );

class I3_EXPORT_BASE i3NetworkClientSocket: public i3ElementBase
{
	I3_CLASS_DEFINE(i3NetworkClientSocket);

private:
	SOCKET					m_socket;
	WSAEVENT				m_recvEvent;
	
	INT32					m_receivedPacketSize;
	char					m_receiveBuffer[CLIENT_SOCKET_BUFFER_SIZE];

	// for callback
	void*					m_pAgent;
	CallbackRoutine			m_pCallbackFunc;

private:
	BOOL					_Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine CallBack, void* pAgent );

protected:
	SOCKET					GetSocket(void);
	char *					GetRecvBuffer();

public:
	i3NetworkClientSocket();
	virtual ~i3NetworkClientSocket();

	BOOL					OnCreate( char* strIP,UINT16 ui16Port, CallbackRoutine CallBack = NULL, void* pAgent = NULL );
	BOOL					OnCreate( UINT32 ui32IP,UINT16 ui16Port, CallbackRoutine CallBack = NULL, void* pAgent = NULL );
	BOOL					SetSocket( SOCKET Socket ); 

	virtual void			OnReceive(void);	
	virtual void			OnDestroy(void);
	virtual void			PacketParsing(i3NetworkPacket* packet);
	virtual INT32			SendPacketMessage(i3NetworkPacket* packet);
	virtual INT32			SendPacketMessage(const char* pBuffer, INT32 nSize);
	
	void					SelectEvent(void);	
	BOOL					IsConnected(void) const;
	virtual void			DisConnect();
};

#endif
