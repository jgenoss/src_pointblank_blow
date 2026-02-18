#ifndef __MODULE_KNOCK_H__
#define __MODULE_KNOCK_H__

class CModuleKnockSoket; 

class CModuleKnock : public i3Thread
{
	I3_CLASS_DEFINE( CModuleKnock );
private:
	INT32					m_i32LogIdx;
	BOOL					m_bRunning;
	CModuleKnockSoket*		m_pKnockSocket;
	UINT32					m_ui32LastSendTime;
	UINT32					m_ui32ConnectTime;
	UINT32					m_ui32SystemInfoSendTime;

	i3PDH					m_pdhPerfmon;

public: 
	CModuleKnock(void);
	virtual ~CModuleKnock(void);
	
	BOOL					OnCreate();
	void					OnDestroy(void);
	
	virtual UINT32			OnRunning( void * pUserData ); 

	INT32					InitSystemLog();

	void 					SendPacketMessage( i3NetworkPacket *SendPacket );


	void 					SendUserInfoLocationWebAck( INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8ServerIdx, INT8 i8ChannelNum, INT32 i32RoomIdx );

	BOOL 					GetRunning();

};

extern CModuleKnock * g_pModuleKnock;

#endif