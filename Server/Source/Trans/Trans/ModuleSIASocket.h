#ifndef	__MODULE_SOCKET_SIA_H__
#define __MODULE_SOCKET_SIA_H__

#define TCP_SOCKET_BUFFER_SIZE		8912 * 3

class CModuleSIASocket : public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleSIASocket );

private:
	INT32		m_i32LogIdx;
	SOCKET		m_socket;
	WSAEVENT	m_recvEvent;
	INT32		m_i32receivedPacketSize;
	char		m_receiveBuffer[TCP_SOCKET_BUFFER_SIZE];
	BOOL		m_bConnect; 

protected:
	SOCKET GetSocket(void);

public:
	UINT32		m_ui32LastRecvTime;
	UINT32		m_ui32ControlSendTime;

	CModuleSIASocket();
	virtual ~CModuleSIASocket();
	
	BOOL		OnCreate(char	*	pServerIp, UINT16 ui16ServerPort, INT32 i32LogIdx );
	BOOL		OnCreate(UINT32 ui32serverIp, UINT16 ui16serverPort, INT32 i32LogIdx );

	virtual void OnReceive(void);
	virtual void OnDestroy(void);

	virtual INT32 PacketParsing(char * pPacket, INT32 i32Packetsize);
	virtual INT32 SendPacketMessage(char * pPacket, INT32 i32size);
	
	void		SelectEvent(void);
	BOOL		IsConnected(void) const;
	BOOL		IsActive(void)			{ return m_bConnect; }
};

#endif
