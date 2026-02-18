#pragma once

#include "Packet.h"

class CControlSocket
{
	void*					m_pView;
	SOCKET					m_hSocket;
	WSAEVENT				m_hRrecvEvent;
	INT32					m_i32ReceivedPacketSize;
	char					m_pReceiveBuffer[ PACKETBUFFERSIZE ];

	DWORD					m_dwSendHeartBitTime;

public:
	CControlSocket();
	~CControlSocket();

	bool					OnCreate( UINT32 ui32IP, UINT16 ui16Port, void* pView );
	void					OnDestroy();

	void					OnRunning();

	void					OnReceive(void);	
	void					PacketParsing( CPacket* packet);
	INT32					SendPacketMessage( CPacket* packet);

	BOOL					IsConnected(void) const;

	SOCKET					GetSocket(void);
};