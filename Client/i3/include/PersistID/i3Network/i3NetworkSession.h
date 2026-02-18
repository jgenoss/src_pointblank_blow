#ifndef __i3NetworkSession_H
#define __i3NetworkSession_H

#define  SESSION_ASYNCFLAG_SEND		0x01
#define  SESSION_ASYNCFLAG_RECEIVE	0x02

class i3NetworkPacket; 

class I3_EXPORT_NETWORK i3NetworkSession : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkSession ); 
private:
	typedef struct I3_EXPORT_NETWORK OVERLAPPED2 : OVERLAPPED
	{
		DWORD		flags;
	} OVERLAPPED2;
	
	OVERLAPPED2		m_OverlappedSend;
	OVERLAPPED2		m_OverlappedRecv;
	
protected: 
	SOCKET				m_Socket; 
	BOOL				m_IsActive; 
	char				m_ReceiveBuffer[PACKETBUFFERSIZE];	

protected: 
	BOOL WaitPacketReceive(void); 
	void DispatchReceive(DWORD Transferbytes); 

public:
	//Session Manager에서 사용할 변수 입니다.  차후 방법을 다시 생각해 봅시다. 다른 방법으로 할까../????
	INT32				m_SessionIdx;	

	//Network Info 
	char				m_pIpString[32];
	UINT32				m_ConIp;
	UINT16				m_ConPort;

	//For Work Thread 
	INT32				m_WorkThreadIdx;	//
	
protected: 	

public:
	i3NetworkSession();
	virtual ~i3NetworkSession();

	void	Dispatch( DWORD Transferbytes, OVERLAPPED* ov); 
	BOOL	SendMessage( i3NetworkPacket * pPacket );		//차후 삭제 	

	void	SetActive(BOOL Active)		{ m_IsActive = Active;	} 
	BOOL	GetIsActive(void)			{ return m_IsActive;	}	

	char *	GetIpString(void)			{ return m_pIpString; }
	virtual BOOL OnCreate(SOCKET Socket, struct sockaddr_in * pAddr);
	virtual BOOL OnDestroy(BOOL bForceMainThread = FALSE);
	virtual void PacketParsing(i3NetworkPacket * pPacket)			{ ; }
	virtual BOOL SendPacketMessage( i3NetworkPacket * pPacket ); 

};

#endif
