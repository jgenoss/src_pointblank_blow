#ifndef __MODULE_CONTROL_H__
#define __MODULE_CONTROL_H__

class CModuleControlSocket; 

class CModuleControl : public i3Thread
{
	I3_CLASS_DEFINE( CModuleControl );
private:	
	BOOL					m_bRunning;
	CModuleControlSocket*	m_pControlSocket;
	UINT32					m_ui32LastSendTime;
	UINT32					m_ui32ConnectTime;
	UINT32					m_ui32SystemInfoSendTime;

	i3PDH					m_pdhPerfmon;

	INT32					m_i32LogIdx;

public: 
	CModuleControl(void);
	virtual ~CModuleControl(void); 	
	
	BOOL					OnCreate(); 
	void					OnDestroy(void);

	virtual UINT32			OnRunning( void * pUserData );

	INT32					InitSystemLog();

	void					SendPacketMessage( i3NetworkPacket *pSendPacket );

	BOOL					ServerInit( INT32 i32Result );
};

extern CModuleControl * g_pModuleControl; 

#endif
