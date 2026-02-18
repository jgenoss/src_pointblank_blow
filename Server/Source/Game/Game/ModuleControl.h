#ifndef __MODULE_CONTROL_H__
#define __MODULE_CONTROL_H__

#include "UserSession.h"

class CModuleControlSocket;

class CModuleControl : public i3Thread
{
	I3_CLASS_DEFINE( CModuleControl );

private:
	BOOL					m_bRunning;
	UINT32					m_ui32ThreadCount;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Control
	CModuleControlSocket*	m_pControlSocket;
	UINT32					m_ui32ControlConnectTime; 
	UINT32					m_ui32ControlSystemInfoSendTime;

	// Control 은 Main, UserSession 에서만 접근이 가능합니다.
	CONNECT_SERVER_NET_BUFFER		m_sControlMainMessage;
	CONNECT_SERVER_NET_BUFFER*		m_pControlUserMessage;

	i3PDH					m_pdhPerfmon;

protected:   

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Control
	INT32					_ControlConnect();
	INT32					_ControlUpdate(void); 

public: 
	CModuleControl(void); 
	virtual ~CModuleControl(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	INT32					OnCreate( UINT32 ui32ThreadCount );
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData);

	BOOL					InitSystemLog();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Battle Server 
	BOOL	ServerInit(			INT32 WorkIdx, INT32 i32Result );
	void	SendPacketMessage( i3NetworkPacket *SendPacket );

};

extern CModuleControl * g_pModuleControl; 

#endif
