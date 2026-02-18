#ifndef __MODULE_RECEIVER_H__
#define __MODULE_RECEIVER_H__

enum RECEIVER_STATE
{
	STATE_WAITING	= 0,
	STATE_DISCONNECTING,
	STATE_READING,
};

class CRingBuffer;

class CModuleReceiver: public i3Thread
{
	I3_CLASS_DEFINE(CModuleReceiver);
private:    
	SOCKET				m_hSocket;
	BOOL				m_IsRunning;
	INT32				m_i32dx;

	INT32				m_i32LogThreadIdx;

	CRingBuffer*		m_pRingReceiver;

	UINT8				m_ui8RecvBuffer[ PACKETBUFFERSIZE ];
	INT32				m_i32RecvSize;

	RECEIVER_STATE		m_eState;
	//CPacketLocker*	m_pPacketLocker;

protected: 
	void				_OnReading(void);
	void				_OnDisconnecting(void);
	void				_RecvErrorHandling(void);

public:
	CModuleReceiver();
	virtual ~CModuleReceiver();

	BOOL				Create( UINT32 i32Idx );
	void				OnDestroy();

	BOOL				SetSocket( SOCKET hSocket );
		
	virtual UINT32		OnRunning(void* pUserData);

	BOOL				SendPacket( i3NetworkPacket* pPacket );
	BOOL				SetRingBuffer( CRingBuffer* pRingReceiver );
	
	BOOL				IsWork(void);
	BOOL				IsDisconnecting(void);
	BOOL				Disconnect(void);

};

#endif //_Receiver_h
