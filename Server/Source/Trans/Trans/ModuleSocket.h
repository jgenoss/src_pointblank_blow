#ifndef __MODULESOCKET_H__
#define __MODULESOCKET_H__

class CModuleSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleSocket ); 
public : 

protected: 
	SOCKET				m_Socket; 
	WSAEVENT			m_RecvEvent;

	char 				m_ReceiveBuffer[PACKETBUFFERSIZE];	
	INT32				m_ReceivedPacketSize;
	
public:
	I3SOCKETADDR		m_LocalAddr;

public :
	BOOL			m_bSendOk;

	CModuleSocket(void);
	virtual ~CModuleSocket(void); 
	
	virtual BOOL OnConnect(char * Address, UINT16 PortNo);

	virtual void OnReceive(void); 	
	virtual BOOL OnDestroy(void);
	virtual INT32 PacketParsing(char * pBuffer); 	
	
	BOOL	SelectEvent(void);			//리턴값이 FALSE 이면 받지 못함. TRUE면 받음 삭제 해야함.	
	INT32	SendPacketMessage(char * pPacket, INT32 PacketSize); 
	void	GetName( SOCKADDR_IN * plocaladdr = NULL );

	BOOL	IsConnected(void)	
	{
		if(!g_pConfig->m_bLogDActive)return TRUE; //사용하지 않음 	
		if(m_Socket == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
}; 

#endif
