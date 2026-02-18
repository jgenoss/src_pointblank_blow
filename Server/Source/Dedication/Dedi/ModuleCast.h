#ifndef __MODULE_CAST_H__
#define __MODULE_CAST_H__

struct CAST_NET_BUFFER
{
	i3NetworkPacket	**	_ppNetMessage;
	INT32				_i32WritePos;
	INT32				_i32ReadPos;
}; 

class CModuleCastSocket; 

class CModuleCast : public i3Thread
{
	I3_CLASS_DEFINE( CModuleCast );

private:
	BOOL					m_bRunning;
	UINT32					m_ui32ThreadCount;

	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Cast
	CModuleCastSocket*		m_pCastSocket;
	UINT32					m_ui32CastConnectTime;

	UINT32					m_ui32UpdateCheck;
	UINT32					m_ui32UpdateCheckStartIdx;
	
	// Coast 은 Main, UserSession 에서만 접근이 가능합니다.
	CAST_NET_BUFFER			m_sCastMainMessage;
	CAST_NET_BUFFER			m_sCastMessage;
	CAST_NET_BUFFER*		m_pCastUserMessage;

	i3PDH					m_pdhPerfmon;
	UINT32					m_ui32WriteTime[WARNING_TYPE_COUNT][WARNING_LEVEL_COUNT];
	UINT32					m_ui32ReadTime[WARNING_TYPE_COUNT][WARNING_LEVEL_COUNT];

protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Control
	INT32					_CastConnect();
	INT32					_CastUpdate(void);

public: 
	CModuleCast(void); 
	virtual ~CModuleCast(void); 	
    	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	BOOL					OnCreate( UINT32 ui32IP, UINT16 ui16Port, UINT32 ui32ThreadCount );
	void					OnDestroy(void);
	virtual UINT32			OnRunning( void * pUserData ); 

	INT32					InitSystemLog();

	// -----------------------------------------------------------------------------
	// Game Server
	// -----------------------------------------------------------------------------

	// PROTOCOL_DSERVER_REGISTER_ROOM_ACK : 방 등록 응답
	BOOL	Send_PROTOCOL_DSERVER_REGISTER_ROOM_ACK		( INT32 i32WorkIdx, P_BATTLE_ROOM_CREATE_ACK * pRoomInfo);
	// Cast server에 의해서 passive로 동작.
	BOOL	Send_PROTOCOL_BATTLE_KILL_ACK				( INT32 i32WorkIdx, P_BATTLE_ROOM_KILL_ACK* pHeader, DEATH_INFO_CHARA* pCharacters, UINT8 ui8RoundNum );
	BOOL	Send_PROTOCOL_BATTLE_M_BOMB_INSTALL_ACK		( INT32 i32WorkIdx, P_BATTLE_M_BOMB_INSTALL_ACK*	pBombInstall	);
	BOOL	Send_PROTOCOL_BATTLE_M_BOMB_UNINSTALL_ACK	( INT32 i32WorkIdx, P_BATTLE_M_BOMB_UNINSTALL_ACK*	pBombUninstall  );
	BOOL	Send_PROTOCOL_BATTLE_M_DESTRUCTION_HP_ACK	( INT32 i32WorkIdx, P_BATTLE_M_DESTRUCTION_HP_ACK*	pDestructionHP	);
	BOOL	Send_PROTOCOL_BATTLE_M_DEFENCE_ACK			( INT32 i32WorkIdx, P_BATTLE_M_DEFENCE_ACK*		pDefence		);
	BOOL	Send_PROTOCOL_BATTLE_HACK_USER_ACK			( INT32 i32WorkIdx, P_BATTLE_HACK_USER_ACK* pHeader, P_BATTLE_HACK_INFO* pUsers ); 
	BOOL	Send_PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK		( INT32 i32WorkIdx, STRUCT_BATTLE_SLOT_EQUIPMENT_ACK* pHeader );
	BOOL	Send_PROTOCOL_BATTLE_KICK_USER_BATTLE_ACK	( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32SlotIdx, INT64 i64UID );
	BOOL	Send_PROTOCOL_BATTLE_GM_PAUSE_ACK			( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, INT32 i32Result );

	void	SendPacketMessage( i3NetworkPacket *pSendPacket );

	BOOL	ServerInit( INT32 i32WorkIdx, INT32 i32Result );

	BOOL	Send_PROTOCOL_BATTLE_RoundStart				( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo );
	BOOL	Send_PROTOCOL_BATTLE_RoundEnd				( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT8 ui8RoundEndType, UINT8 ui8RoundWindTeam );

	BOOL	Send_PROTOCOL_BATTLE_TouchDown				( INT32 i32WorkIdx, BATTLE_ROOM_INFO* pRoomInfo, UINT32 ui32SlotIdx );

	UINT32	GetWarningInfo( INT32 i32WarningType );
	BOOL	SetWarningInfo( INT32 i32WarningType, INT32 i32WarningLevel );
	UINT32	GetFlagWarningLevel( INT32 i32WarningLevel );

};

extern CModuleCast * g_pModuleCast; 

#endif
