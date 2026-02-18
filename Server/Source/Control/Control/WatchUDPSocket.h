#ifndef _WATCH_UDP_SOCKET_H__
#define _WATCH_UDP_SOCKET_H__

#define UDP_RECV_PACKET_SIZE				8096

class CWatchUDPSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( CWatchUDPSocket );
protected:
	SOCKET		m_hSocket;
	sockaddr	m_RecvAddr;	
	char		m_pszReciveBuffer[UDP_RECV_PACKET_SIZE];

public:
	UINT32		m_ui32SendCount;
	UINT32		m_ui32SendBytes;
	UINT32		m_ui32RecvCount;
	UINT32		m_ui32RecvBytes;

public:
	CWatchUDPSocket();
	virtual ~CWatchUDPSocket();

	BOOL	Create( UINT16 ui16BindPort );
	BOOL	Destroy();

	SOCKET	GetSocket(void)		{	return m_hSocket; }
	void	CloseSocket(); 

	INT32	Send( char * pBuf, INT32 i32BufferSize, INT32 i32Size, UINT32 ui32IP, UINT16 ui16Port );
	INT32	NomalSend( char * pBuf, INT32 i32BufferSize, INT32 i32Size, UINT32 ui32IP, UINT16 ui16Port );
	INT32	Recv( void );

	char		*	getPacket(void)					{ return m_pszReciveBuffer; }
	sockaddr	*	getRecvAddr( void )				{ return &m_RecvAddr; }
};

#endif	// _WATCH_H__
