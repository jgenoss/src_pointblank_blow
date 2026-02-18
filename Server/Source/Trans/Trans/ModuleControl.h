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

public: 
	CModuleControl(void);
	virtual ~CModuleControl(void);
	
	BOOL					OnCreate();
	void					OnDestroy(void);
	
	virtual UINT32			OnRunning( void * pUserData ); 

	INT32					InitSystemLog();

	void 					SendPacketMessage( i3NetworkPacket *SendPacket );

	void 					SendUserInfoLocationWebAck( INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8ServerIdx, INT8 i8ChannelNum, INT32 i32RoomIdx );

	void 					SendRSJackPotUserNickInsertReq(char* pJackPotUserNick);

	BOOL					ServerInit( INT32 i32Result );
	BOOL 					GetRunning();
};

extern CModuleControl * g_pModuleControl; 

#endif
