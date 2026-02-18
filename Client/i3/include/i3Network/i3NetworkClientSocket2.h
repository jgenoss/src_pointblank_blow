#ifndef _i3NetworkClientSocket2_h
#define _i3NetworkClientSocket2_h

#define CLIENT_SOCKET_BUFFER_SIZE	(PACKETBUFFERSIZE * 3)

class i3NetworkPacket;

class I3_EXPORT_NETWORK i3NetworkClientSocket2: public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE(i3NetworkClientSocket2, i3ElementBase);
public:
	virtual ~i3NetworkClientSocket2();

	bool					OnCreate(char* serverIp,UINT16 serverPort);
	bool					OnCreate(UINT32 serverIp,UINT16 serverPort);
	bool					SetSocket(SOCKET Socket); 

	virtual void			OnReceive(void);
	virtual void			OnDestroy(void);
	virtual INT32			PacketParsing(char* pBuffer, INT32 nSize);
	virtual INT32			SendPacketMessage(i3NetworkPacket* packet);
	virtual INT32			SendPacketMessage(const char* pBuffer, INT32 nSize);
	
	void					SelectEvent(void);	
	bool					IsConnected(void) const;
	virtual void			DisConnect();

protected:
	SOCKET					GetSocket(void);
	char *					GetRecvBuffer();
	void					ReceiveProcess(INT32 receivedByte);

private:
	bool _Create(UINT32 serverIp,UINT16 serverPort);	

private:
	SOCKET m_socket = INVALID_SOCKET;
	WSAEVENT m_recvEvent = INVALID_HANDLE_VALUE;
	
	INT32 m_receivedPacketSize = 0;
	char m_receiveBuffer[CLIENT_SOCKET_BUFFER_SIZE] = { 0 };
};

#endif
