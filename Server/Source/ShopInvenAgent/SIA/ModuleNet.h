#ifndef __MODULE_NET_
#define __MODULE_NET_

#include "ModuleNet_Server.h"

class CModuleNet
{
private		: 
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bIsRunning;

	i3NetworkServerSocket	m_sServerSocket; 
 
	INT32					m_ui32ServerCount;
	UINT8					m_ui32ConnectCount;
	
	CModuleNet_Server**		m_ppNetServer;

	i3RingBuffer	**		m_ppRecvBuffer; //Push Net, POP Task
	i3RingBuffer	**		m_ppSendBuffer; //Push Task, POP Net

	INT32					m_i32RecvServerIdx; 
protected	: 
	void					_SendConnect( UINT32 ui32Idx );

public		: 
	CModuleNet(); 
	virtual ~CModuleNet(); 

	BOOL OnCreate(UINT32 * pui32ServerList, INT32 i32ServerCount);
	BOOL OnDestroy(void); 

	void OnWorking(void);

	INT32					GetRecvCount(void);
	NET_PACKET_BUFFER *		RecvPop( UINT32 * pui32Idx );
	void					RecvPopIdx(void); 

	INT32	SendPush( UINT32 ui32Idx, void* pBuffer );
	void	SendPush( void* pBuffer );
};

extern CModuleNet * g_pModuleNet; 
#endif

