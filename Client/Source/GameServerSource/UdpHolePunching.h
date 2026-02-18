#ifndef __UDPHOLEPUNCHING__
#define __UDPHOLEPUNCHING__

/*
0529 : 네트워크구조 변경으로 사용하지 않음
*/
class CUdpHolePunching : public i3Thread
{
	I3_CLASS_DEFINE( CUdpHolePunching ); 
private: 

protected:
	char	m_ReceiveBuffer[ PACKETBUFFERSIZE ];
	INT32	m_ReceivedPacketSize;

	i3Udp * m_pSocket;	
	BOOL	m_bIsRun;

	
	i3List * m_pUserList; 

	void	SendToStart(UINT32 Ip, UINT16 Port);
	void	SendToRoom(UINT32 Ip, UINT16 Port);
	

public :
	CUdpHolePunching(); 
	virtual ~CUdpHolePunching(); 

	BOOL OnCreate( INT32 Port , i3List * pUserList);
	BOOL OnDestroy(void); 
	void OnUpdate(void); 

	virtual void	BeforeRunning( void * pUserData);
	virtual UINT32	OnRunning( void * pUserData);
	virtual void	AfterRunning( void * pUserData); 
}; 

extern CUdpHolePunching * g_pUDPHolePuncher;

#endif