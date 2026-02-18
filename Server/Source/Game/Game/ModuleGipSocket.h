#ifndef __MODULE_SOCKET_GIP_H__
#define __MODULE_SOCKET_GIP_H__

typedef struct Gip_Request
{
	INT32 _Type;		//1, 2
	INT32 _Qid; 
}GIP_REQUEST; 

#define GIP_REQUEST_MAX_COUNT		10

class CModuleGipSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleGipSocket ); 
public : 

protected: 
	SOCKET				m_Socket; 
	WSAEVENT			m_RecvEvent;
	char 				m_ReceiveBuffer[PACKETBUFFERSIZE];	
	INT32				m_ReceivedPacketSize;
	BOOL				m_bSendOk;
	UINT32				m_iSendErrorCount;

public:
	BOOL				m_bGreeting;
	UINT16				m_iPId; 
	I3SOCKETADDR		m_LocalAddr;

	UINT32				m_RequestCount; 
	GIP_REQUEST			m_RequestInfo[GIP_REQUEST_MAX_COUNT];

public :
	CModuleGipSocket(void);
	virtual ~CModuleGipSocket(void); 
	
	virtual BOOL OnConnect(char * Address, UINT16 PortNo);

	virtual void OnReceive(void); 	
	virtual BOOL OnDestroy(void);
	virtual INT32 PacketParsing(char * pBuffer); 	
	
	BOOL	SelectEvent(void);										//리턴값이 FALSE 이면 받지 못함. TRUE면 받음 삭제 해야함.	
	INT32	SendPacketMessage(char * pPacket, INT32 PacketSize); 
	void	GetName( SOCKADDR_IN * plocaladdr = NULL );

	BOOL	IsConnected(void)	
	{
		if(m_Socket == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}

	UINT32	GetSendErrorCount(void)		{ return m_iSendErrorCount; }    
}; 
#endif

