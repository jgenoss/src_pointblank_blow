#ifndef __MODULE_SOCKET_GIP_H__
#define __MODULE_SOCKET_GIP_H__

struct GIP_REQUEST
{
	INT32 _Type;		//1, 2
	INT32 _Qid; 
}; 

#define GIP_REQUEST_MAX_COUNT		10

class CModuleGipSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( CModuleGipSocket ); 
public : 

protected: 
	SOCKET				m_Socket; 
	WSAEVENT			m_RecvEvent;
	char 				m_ReceiveBuffer[PACKETBUFFERSIZE];	
	INT32				m_i32ReceivedPacketSize;
	BOOL				m_bSendOk;
	UINT32				m_ui32SendErrorCount;

public:
	BOOL				m_bGreeting;
	UINT16				m_ui16PId; 
	I3SOCKETADDR		m_LocalAddr;

	UINT32				m_RequestCount; 
	GIP_REQUEST			m_RequestInfo[GIP_REQUEST_MAX_COUNT];

public :
	CModuleGipSocket(void);
	virtual ~CModuleGipSocket(void); 
	
	virtual BOOL OnConnect(char * pAddress, UINT16 ui16PortNo);

	virtual void OnReceive(void); 	
	virtual BOOL OnDestroy(void);
	virtual INT32 PacketParsing(char * pBuffer); 
	
	BOOL	SelectEvent(void);										//리턴값이 FALSE 이면 받지 못함. TRUE면 받음 삭제 해야함.	
	INT32	SendPacketMessage(char * pPacket, INT32 i32PacketSize); 
	void	GetName( SOCKADDR_IN * plocaladdr = NULL );

	BOOL	IsConnected(void)	
	{
		if(m_Socket == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}

	UINT32	GetSendErrorCount(void)		{ return m_ui32SendErrorCount; }
}; 
#endif

