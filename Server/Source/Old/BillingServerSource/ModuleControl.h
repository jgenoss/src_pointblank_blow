#ifndef __MODULE_CONTROL_H__
#define __MODULE_CONTROL_H__

#define CONTROL_SEND_BUFFER					100
#define CONTROL_RECONNECT_TIME				1000 *	30	//MS
#define CONTROL_SERVER_HEART_BIT			60
#define CONTROL_SERVER_HEART_BIT_REQ_TIME	70			//10초 동안 하트비트를 받지 못하면 연결을 다시한다. 

class CModuleControlSocket; 

class CModuleControl : public i3Thread
{
	I3_CLASS_DEFINE( CModuleControl );

	BOOL					m_bRunning;
	CModuleControlSocket*	m_pControlSocket;

	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	UINT32					m_ui32ConnectTime; 
	UINT32					m_ui32SystemSendTime;

	INT32					m_i32LogThreadIdx;

protected:
	BOOL					_OnCreatePCState();
	INT32					_ControlConnect();

public: 
	CModuleControl(void); 
	virtual ~CModuleControl(void); 	
	
	BOOL					OnCreate( UINT32 ui32IP, UINT16 ui16Port );
	
	void					OnDestroy(void);
    	
	virtual UINT32			OnRunning( void * pUserData ); 
};

extern CModuleControl * g_pModuleControl; 

#endif
