#ifndef __USER_MANAGER_H__
#define __USER_MANAGER_H__

class CSession; 


class CSessionManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CSessionManager ); 
protected: 		
	i3List					m_SessionList; 	

	//서버, 유저의 연결을 위해 필요함
	struct					timeval m_timeout; 
	i3NetworkServerSocket	m_AcceptSocket; 
	BOOL					m_bAllServerStop;
	INT32					m_i32ServerStep;
	UINT32					m_ui32ServerDelayTime;

	//All Server Start.
	BOOL					m_bAllServerStart;
	INT32					m_i32ServerStartStep;

	void					_SendGameReConfig();
	void					_SendTransReConfig();
	void					_SendDediReConfig();
	void					_SendServiceMessage( UINT32 ui32IP, UINT16 ui16Port, UINT8 ui8Type, INT8 i8State );
	

public:
	CSessionManager(); 
	virtual ~CSessionManager();

	virtual BOOL			OnCreate(void);
	virtual void			OnUpdate(void);
	virtual BOOL			OnDestroy(void);

	void	SendMD5Key(void); 
	void	SendServerLog(void); 
	void	SendAllUserDisconnect(void); 
	void	SendOneUserDisconnect(UINT32 i32Idx);
	void	SendNoticeMessage(INT32 i32type, TTCHAR * pstrMessage, INT32 i32strLen, INT32 i32RGB = 0);
	void	SendControlAppMessage( wchar_t* strMessage );
	void	SendTransUserKick(void); 

	void	TotalStopServer();

	void	AllStartServer();
	void	AllStopServer();
	BOOL	StartServer(CSession * pSession, UINT8 ui8Type, UINT32 i32Idx);
	BOOL	StopServer(CSession * pSession, UINT8 ui8Type, UINT32 i32Idx);

	void	PacketMakeGameReConfig( i3NetworkPacket* pSendPacket );
	void	PacketMakeTransReConfig( i3NetworkPacket* pSendPacket );
	void	PacketMakeDediReConfig( i3NetworkPacket* pSendPacket );

	void	PacketMakeBannerURL(i3NetworkPacket* pSendPacket);
	//Battle Server 
	//Game -> Control -> Battle 
//	BOOL	BattleCreateRoom(	P_BATTLE_ROOM_CREATE_REQ * pReqPacket, const char * pPacket, INT32 i32Size );


	//Battle -> Control ->Game
	void	SendToGameServer( UINT8 ui8GameServer, const char * pData, INT32 i32DataSize); 

	//Dedi Only
	//Dedi -> Control
	//BOOL	DedicationFillWeaponList( N_DSERVER_WEAPON_LIST* pListReq);		// 무기 리스트 요청

	//Game -> Control -> Dedi
	void	SendToDediServer(UINT8 iServer, const char * pData, INT32 iDataSize);

	void	SendCastServer( UINT8 iServer, const char * pData, INT32 iDataSize );

	// Send To all Control Tools
	void	SendToAllControlTools(const char * pData, INT32 i32DataSize);

	void	SendServerError(INT32 i32ServerIdx, INT32 i32DestroyReason);
	
	//Web tool 명령 전달
	void	SendKickUserWebReq(INT32 i32WebSessionIdx, INT8 i8Count, INT64 *pi64UID);
	void	RecvKickUserWebAck(INT32 i32WebSessionIdx, INT32 i32Rv);

	void	SendUserInfoLocationWebReq( INT32 i32WebSessionIdx, INT64 i64UID );
	void	RecvUserInfoLocationWebAck( INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8ServerIdx, INT8 i8ChannelNum, INT32 i32RoomIdx );

	void	SendClanInfoBattleWebReq( INT32 i32WebSessionIdx, UINT32 ui32CID );
	void	RecvClanInfoBattleWebAck( INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8Count );

	void	SendConfirmationMessage( CSession* pSession, wchar_t* strID, INT32 i32Result, INT8 i8Service );
}; 

extern CSessionManager * g_pSessionManager;

#endif