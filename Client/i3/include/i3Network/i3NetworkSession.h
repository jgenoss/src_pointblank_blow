#ifndef __i3NetworkSession_H
#define __i3NetworkSession_H

#define  SESSION_ASYNCFLAG_SEND		0x01
#define  SESSION_ASYNCFLAG_RECEIVE	0x02
#define	 STRING_SIZE_IPADDRESS		32
class i3NetworkPacket; 

struct I3_EXPORT_NETWORK OVERLAPPED2 : OVERLAPPED
{
	DWORD flags = 0;
};

class I3_EXPORT_NETWORK i3NetworkSession : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NetworkSession, i3ElementBase ); 
private:	
	
	OVERLAPPED2	m_OverlappedSend;
	OVERLAPPED2	m_OverlappedRecv;
	SOCKET		m_Socket = INVALID_SOCKET;
	bool		m_IsActive = false; 

protected: 
	char	m_ReceiveBuffer[PACKETBUFFERSIZE] = { 0 };
	INT32	m_ReceiveSize = 0;	

protected: 
	
	void DispatchReceive(DWORD Transferbytes); 

public:
	//Session Manager에서 사용할 변수 입니다.  차후 방법을 다시 생각해 봅시다. 다른 방법으로 할까../????
	INT32				m_SessionIdx = 0;

	//Network Info 
	char				m_pIpString[STRING_SIZE_IPADDRESS] = { 0 };
	UINT32				m_ConIp = 0;
	UINT16				m_ConPort = 0;

	//For Work Thread 
	INT32				m_WorkThreadIdx = -1;
	
protected: 	

public:
	i3NetworkSession();
	virtual ~i3NetworkSession();

	void	Dispatch( DWORD Transferbytes); 
	bool	SendMessage( i3NetworkPacket * pPacket );		//차후 삭제 	
	
	bool	GetIsActive(void)			{ return m_IsActive;  }
	char *	GetIpString(void)			{ return m_pIpString; }
	virtual bool OnCreate(SOCKET Socket, struct sockaddr_in * pAddr);
	virtual bool OnDestroy(bool bForceMainThread = false);
	virtual INT32 PacketParsing(char * pPacket, INT32 iSize)		{ return iSize; }
	virtual	bool WaitPacketReceive(INT32 i32Idx); 


	virtual bool SendPacketMessage( i3NetworkPacket * pPacket ); 

	void	DataEncript( UINT32 ui32Key1, UINT32 ui32Key2, UINT64* pui64Value );
	void	DataDecript( UINT64 ui64Value, UINT32* pui32Key1, UINT32* pui32Key2 );
};

#endif
