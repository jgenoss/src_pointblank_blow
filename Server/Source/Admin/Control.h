#ifndef _CONTROL_H__
#define _CONTROL_H__

#define IP_NAME_COUNT		32
class CControl :	public i3ElementBase
{
	I3_CLASS_DEFINE( CControl );
protected :	
	i3NetworkServerSocket m_AcceptSocket; 
	struct		timeval m_timeout; 
	SOCKET		m_SocketAccept;
	i3Timer		m_Timer;
	WSAEVENT	m_RecvEvent;
	REAL32		m_rConnectTime; 

	char		m_ConnectIp[32];	
public:
	CControl(void);
	~CControl(void);

public:	
	char	m_Ip[IP_NAME_COUNT]; 
	UINT16	m_Port; 	
	char	m_pServiceName[512]; 

	INT32		m_MacAddressListCount; 
	UINT64	*	m_pMacAddressList; 
public:
	BOOL	Create(const char * szFileName); 
	BOOL	Destroy(void);

	INT32	Update(void); 

	BOOL	LoadFromFile(const char * szFileName);	


	void	CloseSocket(void); 
	INT32	OnReceive(void); 
	void	OnSend(INT32 Rv); 
};

extern CControl * g_pControl;

#endif //_CONFIG_H__