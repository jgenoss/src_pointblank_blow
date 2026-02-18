#ifndef _UdpServer_h
#define _UdpServer_h

class CUdpWorker;

class CUdpServer: public i3ElementBase
{
	I3_CLASS_DEFINE(CUdpServer);

	CUdpWorker**	m_ppSocket;
	INT32			m_nSocketCount;
	
	INT32			m_i32OldCountIdx;
	INT32			m_i32NewCountIdx;
	UINT32			m_ui32Time;

public:
	CUdpServer();
	virtual ~CUdpServer();

	
	BOOL 			Create( INT32 nSocketCount, UINT32 wIP, UINT16* psPort );	

	INT32			GetSocketCount()		{	return m_nSocketCount;	};
	CUdpWorker*		GetSocket(INT32 idx )	{	return m_ppSocket[idx];	};

	INT32			GetCountIdx()			{	return m_i32NewCountIdx;	};

	// Control 서버에서 호출
	void			SetPacketCount_C();										// 시간당 패킷 카운트를 리셋해준다.
	UINT32			GetPacketCount_C();										// 시간당 총 패킷을 얻어온다.
	UINT16			GetRestPort_C();										// 패킷중 쉬고있는 포트 구하기.

};

extern CUdpServer* g_pUdpServer;

#endif
