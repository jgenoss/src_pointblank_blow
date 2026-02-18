#ifndef __MODULE_NET_MANAGER_H__
#define __MODULE_NET_MANAGER_H__

class CModuleNetServer;
class CModuleFileManager;

class CModuleNetManager
{
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	bool					m_bIsRunning;

	SOCKET					m_hServerSocket;

	i3NetworkServerSocket	m_sServerSocket; 
 
	UINT32					m_ui32ServerIP;
	UINT16					m_ui16ServerPort;

	INT32					m_i32RecvCount;
	UINT32*					m_pui32RecvIP;

	CModuleNetServer**		m_ppNetServer;

public: 
	CModuleNetManager(); 
	virtual ~CModuleNetManager(); 

	bool					Create( UINT32 ui32ServerIP, UINT16 ui16Port, INT32 i32RecvCount, UINT32* pui32RecvIP, CModuleFileManager* pFileManager );
	void					Destroy(void);

	void					Working(void);

	INT32					InsertDB( void* pData );
};

#endif

