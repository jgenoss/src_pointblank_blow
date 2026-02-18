#ifndef __NETWORK_TCP_BASE_H
#define __NETWORK_TCP_BASE_H

typedef struct sockaddr		_SOCKADDR;	//

class EXPORT_BASE NSM_cTcpBase
{	
protected: 
	int		m_Socket;
	int		CreateSocket(void); 
	void	DeleteSocket(void);

public:
	NSM_cTcpBase(); 
	virtual ~NSM_cTcpBase(); 
	BOOL	Connect(const char * pAddress, UINT16 iPortNo); 
	BOOL	Connect(unsigned int iAddress, UINT16 iPortNo);
	void	Send(const char * pBuffer, int iByte, int iFlag = 0);
	int		Recv(char * pBuffer, int iByte, int iFlags = 0);
	void	GetName( _SOCKADDR * pLocal );
	int		getSocketHandle( void )	{ return m_Socket; }
}; 

#endif
