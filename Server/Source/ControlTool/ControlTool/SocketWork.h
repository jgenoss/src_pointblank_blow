
#pragma once

#include "Packet.h"
#include "RingBuffer.h"

class CSocketWork
{
	HANDLE			m_hThread;
	BOOL			m_bThreadOper;
	DWORD			m_ThreadID;

	SOCKET			m_hSocket;

	WSAEVENT		m_hRrecvEvent;
	INT32			m_i32ReceivedPacketSize;
	char			m_pReceiveBuffer[ PACKETBUFFERSIZE ];

	DWORD			m_dwSendHeartBitTime;
	BOOL			m_bSetHeartBit;

	CRingBuffer		m_PacketBuffer;

	sockaddr_in		m_serverAddr;

private:
	void			OnRunning();

	//static UINT __stdcall SocketThread(void* pParam);
	static DWORD WINAPI	SocketThread(LPVOID lpParam);

	void			PacketSetting(const char * buffer, INT32 Size, INT32 index);
	BOOL			CreateSocket(void);
	BOOL			DoConnect(void);
	void			ThreadDestroy(void);

public:
	CSocketWork();
	~CSocketWork();

	void			Init();
	BOOL			OnCreate( UINT32 ui32IP, UINT16 ui16Port, BOOL UseHeartbit);
	void			OnDestroy();

	void			OnReceive(void);	
	INT32			SendPacketMessage( CPacket* packet);

	BOOL			IsConnected(void) const;

	SOCKET			GetSocket(void);

	CPacket*		GetPacket();

	void			SetResume(void);
	void			SocketClose(void);
};