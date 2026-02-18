#ifndef _Receiver_h
#define _Receiver_h

#include "PacketCombiner.h"

enum RECEIVER_TYPE
{
	UNKOWN_SERVER = -1,
	GAME_SERVER,

	MAX_RECEIVER_TYPE
};

typedef enum ReceiverState
{
	STATE_WAITING	= 0,
	STATE_DISCONNECTING,
	STATE_READING,
}RECEIVER_STATE;

class CPacketLocker;

class CReceiver: public i3Thread
{
	I3_CLASS_DEFINE(CReceiver);

	SOCKET			m_socket;
	INT32			m_i32idx;
	BOOL			m_bIsRunning;

	UINT8			m_ui8RecvBuffer[ PACKETBUFFERSIZE ];
	INT32			m_i32RecvSize;

	RECEIVER_STATE	m_state;
	CPacketLocker*	m_pPacketLocker;

private:
	INT32			m_i32LogIdx;

private: 
	void			_OnReading(void);
	void			_OnDisconnecting(void);
	void			_RecvErrorHandling(void);

public:
	CReceiver();
	virtual ~CReceiver();

	BOOL			Create(UINT32 ui32idx,CPacketLocker* pPacketLocker);
	void			OnDestroy();
	BOOL			SetSocket(SOCKET socket,BOOL bI3Packet = TRUE);
		
	virtual UINT32	OnRunning(void* pUserData);

	BOOL			SendPacket( i3NetworkPacket* pPacket );

	BOOL			IsWork(void);
	BOOL			IsDisconnecting(void);
	BOOL			Disconnect(void);

	static RECEIVER_TYPE GetReceiverType(UINT32 ui32idx);
};

#endif //_Receiver_h
