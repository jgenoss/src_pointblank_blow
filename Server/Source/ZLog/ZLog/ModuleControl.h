#ifndef __MODULE_CONTROL__
#define __MODULE_CONTROL__

#include <pdh.h>
#include <pdhmsg.h>

class cModuleControl : public i3NetworkClientSocket2
{
	I3_CLASS_DEFINE( cModuleControl );

private		:
	UINT32					m_ui32ServerIP;
	UINT16					m_ui16ServerPort;
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bIsRunning;
	BOOL					m_bConnect;

	UINT32					m_ui32LastConnectTime;		// 접속 타임
	UINT32					m_ui32LastSendTime;			// HartBit 타임
	UINT32					m_ui32LastInfoTime;			// Info 타임

	PDH_STATUS_INFO			m_stCPU;
	PDH_STATUS_INFO			m_stHandle;
	UINT32					m_i32PerformRevisionTime;	

public		: 
	cModuleControl(); 
	virtual ~cModuleControl(); 

	BOOL					Create( UINT32 ui32IP, UINT16 ui16Port );
	void					Destroy(void); 
	void					OnWorking();

	BOOL					InitSystemLog();

	virtual INT32			PacketParsing(char * pPacket, INT32 i32PacketSize);

	BOOL					ServerInit( INT32 i32Result );
};

extern cModuleControl * g_pModuleControl; 

#endif

