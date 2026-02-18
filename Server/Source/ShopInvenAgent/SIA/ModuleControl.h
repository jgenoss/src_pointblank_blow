#ifndef __MODULE_CONTROL__
#define __MODULE_CONTROL__

#include <pdh.h>
#include <pdhmsg.h>

class CModuleControl : public i3NetworkClientSocket2
{
	I3_CLASS_DEFINE( CModuleControl );

private		:
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bIsRunning;

	UINT32					m_ui32ServerIP; 
	UINT16					m_ui16ServerPort;
	BOOL					m_bConnect;

	UINT32					m_ui32LastConnectTime;		// 접속 타임
	UINT32					m_ui32LastSendTime;			// HartBit 타임
	UINT32					m_ui32LastInfoTime;			// Info 타임

	PDH_STATUS_INFO			m_stCPU;
	PDH_STATUS_INFO			m_stHandle;
	UINT32					m_i32PerformRevisionTime;

public		: 
	CModuleControl(); 
	virtual ~CModuleControl(); 

	BOOL					Create( UINT32 ui32ServerIp, UINT16 ui16Port );
	void					Destroy(void); 
	void					OnWorking();
	BOOL					InitSystemLog();
	

	virtual INT32			PacketParsing(char * pPacket, INT32 i32PacketSize);

	
	BOOL					ServerInit( INT32 i32Result );
};

extern CModuleControl * g_pModuleControl; 

#endif

