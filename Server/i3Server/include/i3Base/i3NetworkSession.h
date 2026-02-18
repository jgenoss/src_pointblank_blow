#ifndef __i3NetworkSession_H
#define __i3NetworkSession_H

#define  SESSION_ASYNCFLAG_SEND		0x01
#define  SESSION_ASYNCFLAG_RECEIVE	0x02
#define	 STRING_SIZE_IPADDRESS		32

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

class i3NetworkPacket; 

struct I3_EXPORT_BASE OVERLAPPED2 : OVERLAPPED
{
	DWORD flags;
};

class I3_EXPORT_BASE i3NetworkSession : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkSession ); 
private:	
	
	OVERLAPPED2				m_OverlappedSend;
	OVERLAPPED2				m_OverlappedRecv;
	SOCKET					m_Socket; 
	BOOL					m_IsActive; 

protected: 
	char					m_ReceiveBuffer[PACKETBUFFERSIZE];	
	INT32					m_ReceiveSize;	

protected: 
	
	void					DispatchReceive(DWORD Transferbytes); 

public:
	//Session Manager에서 사용할 변수 입니다.  차후 방법을 다시 생각해 봅시다. 다른 방법으로 할까../????
	INT32					m_SessionIdx;	

	//Network Info 
	char					m_strIPString[STRING_SIZE_IPADDRESS];
	wchar_t					m_wstrIPString[STRING_SIZE_IPADDRESS];
	UINT32					m_ConIp;
	UINT16					m_ConPort;

	//For Work Thread 
	INT32					m_WorkThreadIdx;	//
	
protected: 	

public:
	i3NetworkSession();
	virtual ~i3NetworkSession();

	virtual BOOL			Create();
	virtual void			Destroy();

	virtual BOOL			OnConnect(SOCKET Socket, struct sockaddr_in * pAddr);
	virtual BOOL			OnConnectInit();
	virtual BOOL			OnDisconnect(BOOL bForceMainThread = FALSE);

	void					Dispatch( DWORD Transferbytes); 
	BOOL					SendMessage( i3NetworkPacket * pPacket );		//차후 삭제 	
	
	BOOL					GetIsActive(void)			{ return m_IsActive;  }
	char *					GetIPStringA(void)			{ return m_strIPString; }
	wchar_t *				GetIPStringW(void)			{ return m_wstrIPString; }
	
	virtual INT32			PacketParsing(char * pPacket, INT32 iSize)		{ return iSize; }
	virtual	BOOL			WaitPacketReceive(INT32 i32Idx); 


	virtual BOOL			SendPacketMessage( i3NetworkPacket * pPacket ); 

	void					DataEncript( UINT32 ui32Key1, UINT32 ui32Key2, UINT64* pui64Value );
	void					DataDecript( UINT64 ui64Value, UINT32* pui32Key1, UINT32* pui32Key2 );
};

#endif
