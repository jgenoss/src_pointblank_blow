#if !defined( __I3NETWORK_UDPSOCKET_H )
#define __I3NETWORK_UDPSOCKET_H

////////////////////////////////////////////////////////////////////////////////
// UDPSocket 입니다. by 정순구 2007. 1. 3

class I3_EXPORT_NETWORK i3NetworkUDPSocket : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NetworkUDPSocket, i3ElementBase );
protected:
	SOCKET		m_hSocket = INVALID_SOCKET;
	sockaddr	m_RecvAddr;

	INT32		m_nBufSize = 0;
	char	*	m_pszReciveBuffer = nullptr;

public:
#if !defined( I3_NO_PROFILE)
	INT32		m_nSendCount = 0;
	INT32		m_nSendBytes = 0;
	INT32		m_nRecvCount = 0;
	INT32		m_nRecvBytes = 0;
#endif

public:
	i3NetworkUDPSocket();
	virtual ~i3NetworkUDPSocket();

	bool	Create( INT32 nBufSize, UINT16 nBindPort );
	bool	Destroy();

	INT32	Send( char * pBuf, INT32 nSize, UINT32 nIP, UINT16 nPort );
	INT32	Recv( void );

	char *	getPacket(void)						{ return m_pszReciveBuffer; }
	char *	getPacket( INT32 nPos )				{ return &m_pszReciveBuffer[ nPos ]; }
	sockaddr * getRecvAddr( void )				{ return &m_RecvAddr; }
};

#endif	// __I3NETWORK_UDPSOCKET_H

