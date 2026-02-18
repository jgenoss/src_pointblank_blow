#ifndef __MODULE_CONTROL_H__
#define __MODULE_CONTROL_H__

struct CONTROL_NET_BUFFER
{
	i3NetworkPacket	**	_ppNetMessage;
	INT32				_i32WritePos;
	INT32				_i32ReadPos;
}; 

class CModuleControlSocket; 

class CModuleControl : public i3Thread
{
	I3_CLASS_DEFINE( CModuleControl );

private:
	BOOL					m_bRunning;
	UINT32					m_ui32ThreadCount;

	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Control
	CModuleControlSocket*	m_pControlSocket;
	UINT32					m_ui32ControlConnectTime;
	UINT32					m_ui32ControlSystemInfoSendTime;

	UINT32					m_ui32UpdateCheck;
	UINT32					m_ui32UpdateCheckStartIdx;
	
	// Control 은 Main, UserSession 에서만 접근이 가능합니다.
	CONTROL_NET_BUFFER		m_sControlMainMessage;
	CONTROL_NET_BUFFER		m_sControlMessage;
	CONTROL_NET_BUFFER*		m_pControlUserMessage;

	i3PDH					m_pdhPerfmon;
	UINT32					m_ui32WriteTime[WARNING_TYPE_COUNT][WARNING_LEVEL_COUNT];
	UINT32					m_ui32ReadTime[WARNING_TYPE_COUNT][WARNING_LEVEL_COUNT];

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
	BOOL					OnCreate( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ThreadCount );
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData ); 

	INT32					InitSystemLog();

	// -----------------------------------------------------------------------------
	// Game Server
	// -----------------------------------------------------------------------------
	void					SendPacketMessage( i3NetworkPacket *pSendPacket );

	BOOL					ServerInit( INT32 i32WorkIdx, INT32 i32Result );

	UINT32					GetWarningInfo( INT32 i32WarningType );
	BOOL					SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel );
	UINT32					GetFlagWarningLevel( INT32 i32WarningLevel );

};

extern CModuleControl * g_pModuleControl; 

#endif
