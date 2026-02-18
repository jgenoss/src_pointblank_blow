#ifndef __I3NETWORKCLIENTSOCKET_H__
#define __I3NETWORKCLIENTSOCKET_H__

class i3NetworkPacket; 
typedef	void ( *CallbackRoutine )( i3NetworkPacket * packet );

// 성백 : 
// 클라이언트 패킷은 다시 만들었으면 함...... 2006/11/13
// 불필요한 부분이 많이 있음

class I3_EXPORT_NETWORK i3NetworkClientSocket : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NetworkClientSocket ); 
public : 


protected: 
	SOCKET				m_Socket; 
	WSAEVENT			m_RecvEvent;
	CallbackRoutine		m_pCallbackFunc;

	char 				m_ReceiveBuffer[PACKETBUFFERSIZE * 3];	// *3 버퍼 여유공간 확보
	INT32				m_ReceivedPacketSize;

	BOOL				m_bSendFlush; 

public:
	I3SOCKETADDR		m_LocalAddr;

public :
	i3NetworkClientSocket(void);
	virtual ~i3NetworkClientSocket(void); 
	
	virtual BOOL OnCreate(char * Address, UINT16 PortNo, CallbackRoutine CallBack = NULL); 	
	virtual BOOL OnCreate(UINT32 iAddress, UINT16 PortNo, CallbackRoutine CallBack = NULL); 	

	virtual void OnReceive(void); 
	virtual void OnSend(void)	{;} 
	virtual BOOL OnDestroy(void);
	virtual void PacketParsing(i3NetworkPacket  * pPacket); 
	
	void SelectEvent(void); 
	virtual INT32 SendPacketMessage(i3NetworkPacket * pPacket); 
	void	GetName( SOCKADDR_IN * plocaladdr = NULL );
	BOOL	IsConnected(void)	
	{
		if(m_Socket == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}
}; 
#endif

