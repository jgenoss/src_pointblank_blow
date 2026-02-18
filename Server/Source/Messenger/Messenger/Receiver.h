#ifndef _Receiver_h
#define _Receiver_h

#include "PacketCombiner.h"

enum RECEIVER_TYPE
{
	UNKOWN_SERVER = -1,
	GAME_SERVER,
	GSM_SERVER,
	IBREQUEST_SERVER,
	IBNOTIFY_SERVER,

	MAX_RECEIVER_TYPE
};

class CPacketLocker;

class CReceiver: public i3Thread
{
	I3_CLASS_DEFINE(CReceiver);
public:
	enum STATE
	{
		WAITING = 0,
		DISCONNECTING,
		PACKET_PROCESSING,
		READING,
	};

public:
	CReceiver();
	virtual ~CReceiver();

	BOOL	Create(UINT32 ui32idx,CPacketLocker* pPacketLocker);
	BOOL	SetSocket(SOCKET socket,BOOL bI3Packet = TRUE);

	void	OnWaiting(void);
	void	OnPacketProcessing(void);
	void	OnReading(void);
	void	OnDisconnecting(void);
		
	virtual UINT32 OnRunning(void* pUserData);

	BOOL	SendPacket(i3NetworkPacket* pPacket);
	void	_RecvErrorHandling(void);

	BOOL	IsWork(void);
	BOOL	IsDisconnecting(void);
	BOOL	Disconnect(void);
	void	RequestDisconnect(void);

private:
    CPacketCombiner m_Combiner;
	SOCKET	m_socket;
	BOOL	m_bIsRunning;
	INT32	m_i32idx;
	CPacketLocker* m_pPacketLocker;
	STATE	m_state;
	BOOL	m_bI3Packet;
	BOOL	m_bRequestDisconnect;
	INT32	m_i32LogIdx;
};

#endif //_Receiver_h
