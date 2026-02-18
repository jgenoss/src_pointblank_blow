#ifndef __TCP_SOCKET_H
#define __TCP_SOCKET_H

#define TCP_SOCKET_BUFFER_SIZE		8192 * 3

//Selelct Disconnect Cord
#define TCP_DISCONNECT_TYPE_OTHER			0X80000001
#define TCP_DISCONNECT_TYPE_READ_ERROR		0X80000002

typedef	INT32 ( *CallbackRoutine )( void* pAgent, char * pPacket, INT32 Packetsize );

class EXPORT_BASE NSM_cTcpsocket
{	
private:
	SOCKET				m_socket;
	WSAEVENT			m_recvEvent;
	INT32				m_receivedPacketSize;
	char				m_receiveBuffer[TCP_SOCKET_BUFFER_SIZE];

	// for callback
	void*				m_pAgent;
	CallbackRoutine		m_pCallbackFunc;

protected:
	SOCKET	GetSocket(void);
	BOOL	OnConnect(UINT32 serverIp, UINT16 serverPort, CallbackRoutine CallBack = NULL, void* pAgent = NULL );
	void	OnDisConnect(void); 

public:
	NSM_cTcpsocket();
	virtual ~NSM_cTcpsocket();
	
	virtual BOOL OnCreate(char	*	pServerIp, UINT16 iServerPort, CallbackRoutine CallBack = NULL, void* pAgent = NULL);
	virtual BOOL OnCreate(unsigned int serverIp, UINT16 serverPort, CallbackRoutine CallBack = NULL, void* pAgent = NULL);	
	//virtual BOOL OnCreate(SOCKET Socket); 
	virtual void OnDestroy(void);

	SOCKET AcceptConnection( struct timeval * ptimeout, struct sockaddr_in * pAddr, INT32 * pSize );

	BOOL SetSocket(SOCKET Socket); 

	virtual BOOL OnReceive(void);


	virtual INT32 PacketParsing(char * pPacket, INT32 Packetsize);
	virtual INT32 SendPacketMessage(char * pPacket, INT32 size);
	
	INT32 SelectEvent(void);	
	BOOL IsConnected(void) const;
};

#endif
