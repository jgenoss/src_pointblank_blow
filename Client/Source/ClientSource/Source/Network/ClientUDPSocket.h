#ifndef __CLIENTUDPSOCKET_H__
#define __CLIENTUDPSOCKET_H__

#define UDP_RECV_PACKET_SIZE				8096

class ClientUDPSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( ClientUDPSocket, i3ElementBase );
protected:
	SOCKET		m_hSocket;
	sockaddr	m_RecvAddr;	
	char		m_pszReciveBuffer[UDP_RECV_PACKET_SIZE];

public:
	UINT32		m_nSendCount;
	UINT32		m_nSendBytes;
	UINT32		m_nRecvCount;
	UINT32		m_nRecvBytes;

public:
	ClientUDPSocket();
	virtual ~ClientUDPSocket();

	bool	Create( UINT16 nBindPort );
	bool	Destroy();

	SOCKET	GetSocket(void)		{	return m_hSocket; }
	void	CloseSocket(); 

	INT32	Send( char * pBuf, INT32 nSize, UINT32 nIP, UINT16 nPort );
	INT32	Recv( void );
	INT32	SendBattleServer( char* pBuf, INT32 nSize, UINT8 cTargetIdx );

	char		*	getPacket(void)					{ return m_pszReciveBuffer; }
	sockaddr	*	getRecvAddr( void )				{ return &m_RecvAddr; }
};

#endif	// __ClientUDPSocket_H__
