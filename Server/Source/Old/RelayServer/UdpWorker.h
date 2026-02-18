#ifndef _UdpWorker_h
#define _UdpWorker_h

#define PACKETCOUNT_IDX			2
#define PACKETCOUNT_TIME		1

class CUdpBuffer;

class CUdpWorker: public i3Thread
{	
	I3_CLASS_DEFINE(CUdpWorker);

	INT32			m_i32WorkerIndex;
	SOCKET			m_ServerSocket;
	CUdpBuffer*		m_pRecvBuffer;
	CUdpBuffer*		m_pSendBuffer;

	UINT32			m_pui32PacketCount[ PACKETCOUNT_IDX ];		// 분당 패킷양을 구합니다.

	virtual BOOL	Create(char* pszName,UINT32 Attr = 0,UINT32 StackSize = 4096,void* pUserData = NULL,INT32 priority = PRIORITY_NORMAL);

	UINT32			m_wIP;
	UINT16			m_sPort;

	INT32			m_i32ErrorCount;
	UINT32			m_ui32ErrorTime;

	BOOL			m_bRunning;
public:
	CUdpWorker();
	virtual ~CUdpWorker();

	BOOL			Create( INT32 nWorkerIndex, UINT32 wIP, UINT16 sPort );
	virtual UINT32	OnRunning(void* pUserData);

	BOOL			IsConnect()						{	(INVALID_SOCKET == m_ServerSocket)?FALSE:TRUE;	};
	BOOL			RecvPacket();
	BOOL			SendPacket();
	BOOL			SendPacket( CUdpBuffer* pRecvBuffer );

	void			PacketParsing();

	UINT32			GetPacketCount( INT32 i32Idx )	{	return m_pui32PacketCount[ i32Idx ];	};
	void			SetPacketCount( INT32 i32Idx )	{	m_pui32PacketCount[ i32Idx ] = 0;	};

	UINT16			GetPort()						{	return m_sPort;	};
};

#endif
