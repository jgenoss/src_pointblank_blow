#ifndef _i3NetworkClientSocket2_h
#define _i3NetworkClientSocket2_h

#define CLIENT_SOCKET_BUFFER_SIZE	(PACKETBUFFERSIZE * 3)

class i3NetworkPacket;

class I3_EXPORT_NETWORK i3NetworkClientSocket2: public i3ElementBase
{
	I3_CLASS_DEFINE(i3NetworkClientSocket2);
public:
	i3NetworkClientSocket2();
	virtual ~i3NetworkClientSocket2();

	BOOL OnCreate(char* serverIp,UINT16 serverPort);
	BOOL OnCreate(UINT32 serverIp,UINT16 serverPort);

	virtual void OnReceive(void);	
	virtual void OnDestroy(void);
	virtual void PacketParsing(i3NetworkPacket* packet);
	virtual INT32 SendPacketMessage(i3NetworkPacket* packet);
	
	void SelectEvent(void);	
	BOOL IsConnected(void) const;

protected:
	SOCKET GetSocket(void);

private:
	BOOL _Create(UINT32 serverIp,UINT16 serverPort);	

private:
	SOCKET m_socket;
	WSAEVENT m_recvEvent;
	
	INT32 m_receivedPacketSize;
	char m_receiveBuffer[CLIENT_SOCKET_BUFFER_SIZE];
};

#endif
