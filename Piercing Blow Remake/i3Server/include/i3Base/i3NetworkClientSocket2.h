#ifndef _i3NetworkClientSocket2_h
#define _i3NetworkClientSocket2_h

#define CLIENT_SOCKET_BUFFER_SIZE	(PACKETBUFFERSIZE * 3)

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

#define TCP_DISCONNECT_TYPE_OTHER			0X80000001
#define TCP_DISCONNECT_TYPE_READ_ERROR		0X80000002

class i3NetworkPacket;

typedef	INT32 ( *CallbackRoutine2 )( void* pAgent, char* pPacket, INT32 i32Size );

class I3_EXPORT_BASE i3NetworkClientSocket2: public i3ElementBase
{
	I3_CLASS_DEFINE(i3NetworkClientSocket2);

private:
	SOCKET					m_socket;
	WSAEVENT				m_recvEvent;
	
	INT32					m_receivedPacketSize;
	char					m_receiveBuffer[CLIENT_SOCKET_BUFFER_SIZE];

	// for callback
	void*					m_pAgent;
	CallbackRoutine2		m_pCallbackFunc;

private:
	BOOL					_Create( UINT32 ui32IP, UINT16 ui16Port, CallbackRoutine2 CallBack, void* pAgent );

protected:
	SOCKET					GetSocket(void);
	char *					GetRecvBuffer();

public:
	i3NetworkClientSocket2();
	virtual ~i3NetworkClientSocket2();

	BOOL					OnCreate( char* strIP,UINT16 ui16Port, CallbackRoutine2 CallBack = NULL, void* pAgent = NULL );
	BOOL					OnCreate( UINT32 ui32IP,UINT16 ui16Port, CallbackRoutine2 CallBack = NULL, void* pAgent = NULL );
	BOOL					SetSocket(SOCKET Socket); 

	virtual INT32			OnReceive(void);	
	virtual void			OnDestroy(void);
	virtual INT32			PacketParsing(char* pBuffer, INT32 nSize);
	virtual INT32			SendPacketMessage(i3NetworkPacket* packet);
	virtual INT32			SendPacketMessage(const char* pBuffer, INT32 nSize);
	
	INT32					SelectEvent(void);	
	BOOL					IsConnected(void) const;
	virtual void			DisConnect();
};

#endif
