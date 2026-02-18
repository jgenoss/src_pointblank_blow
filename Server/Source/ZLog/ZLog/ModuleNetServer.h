#ifndef __MODULE_SERVER_NET_
#define __MODULE_SERVER_NET_


#define MByte 1048576


enum RECEIVER_STATE
{
	RECEIVER_STATE_WAITING,
	RECEIVER_STATE_STARTING,
	RECEIVER_STATE_RUNNING,
	RECEIVER_STATE_ENDING,
};

class CModuleFileManager;


class CModuleNetServer : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleNetServer );

	HANDLE				m_hThread;
	UINT32				m_ui32ThreadID;
	bool				m_bIsRunning;

	UINT32				m_ui32Index;

	RECEIVER_STATE		m_eReceiverState;

	CModuleFileManager*	m_pFileManager;
	HANDLE				m_hFile;
	char				m_strFileName[ FILE_NAME_SIZE ];

	UINT32				m_ui32FileSize;
	UINT32				m_ui32RecvFileSize;
	
	void				_WriteFile( INT32 i32Size, char* pData );
	void				_SendFileRecvFinish( BOOL result );

public:
	CModuleNetServer();
	virtual ~CModuleNetServer();

	bool				Create( INT32 i32Idx, CModuleFileManager* pFileManager );
	void				Destroy();

	void				Working();
	bool				SetServer( SOCKET hSocket );

	virtual void		PacketParsing( i3NetworkPacket  * pPacket );

	
};

#endif