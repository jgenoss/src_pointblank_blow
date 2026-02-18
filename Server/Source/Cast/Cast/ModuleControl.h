#ifndef __MODULE_CONTROL_H__
#define __MODULE_CONTROL_H__

class CUserSession; 
class CModuleControlSocket; 

class CModuleControl : public i3Thread
{
	I3_CLASS_DEFINE( CModuleControl );
private:	
	BOOL					m_bRunning;
	CModuleControlSocket*	m_pControlSocket;
	UINT32					m_ui32ControlConnectTime;
	UINT32					m_ui32SystemInfoSendTime;
	
	i3PDH					m_pdhPerfmon;

public: 
	CModuleControl(void); 
	virtual ~CModuleControl(void); 	

	INT32					_ControlConnect();

	BOOL					OnCreate(void); 
	void					OnDestroy(void);
    	
	virtual UINT32			OnRunning( void * pUserData ); 

	INT32					InitSystemLog(); 

	void					SendPacketMessage( i3NetworkPacket *SendPacket );

	BOOL					ServerInit( INT32 i32Result );
};

extern CModuleControl * g_pModuleControl; 

#endif
