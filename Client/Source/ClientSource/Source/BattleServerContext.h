#pragma once


struct PING
{
	// 핑 센드 검사 시간
	REAL32			m_fPingRecvTime = 0.0f;
	// 핑 검사를 보낸 시각
	REAL32			m_rPingSendTime = 0.0f;
	// 핑 계산을 한 값(실제 화면에 뿌리기 위한 용도)
	UINT8			m_Pings[SLOT_MAX_COUNT] = { 0 };
	// Ping 검사 시간
	REAL32			m_rPingCheckTime = 0.0f;
	// 계산용 Packet을 받으면 이전에 보낸 시간으로부터 타임을 계산한다.
	REAL32			m_rPingList[SLOT_MAX_COUNT] = { 0.0f };		// m_PingList
	// 서버 전달용 보정 핑 타임 ( 1초 = 1000 )
	UINT16			m_PingTimes[SLOT_MAX_COUNT] = { 0 };
	/** \brief 처음 초기화 함수 */
	void	Init( void);

	/** \brief Round 시작시 초기화 함수 */
	void	Reset( void);

	/** \brief Ping Packet을 받으면 처리한다. */
	void	RecvPing( INT32 userIdx, REAL32 sendTime, REAL32 elapsedTime);

	UINT8		GetPing(INT32 iUserIdx ) const
	{ 
		I3_BOUNDCHK( iUserIdx, SLOT_MAX_COUNT); return m_Pings[ iUserIdx ]; 
	}

	void		SetPings( UINT8 * pPings, UINT32 size)
	{
		I3ASSERT( sizeof( m_Pings) == size);
		i3mem::Copy( m_Pings, pPings, size);
	}
};


class BattleServerContext : public i3::shared_ginst<BattleServerContext>
{
public:

	BattleServerContext();
	~BattleServerContext();

	BATTLE_SERVER_TYPE	GetServerType( void) const				{ return m_ServerType; }
	void				SetServerType( BATTLE_SERVER_TYPE type)	{ m_ServerType = type; }

	
	void				SetP2PActivate()	{ m_bP2PActivate = true; }
	void				SetP2PDeactivate()	{ m_bP2PHost = false; m_bP2PActivate = false; }

	void				SetP2PHost()		{ m_bP2PHost = true; }
	void				SetP2PNonHost()		{ m_bP2PHost = false; }


	bool		IsP2PActivate() const	{	ISNETWORKRETURNV(false,true);	return m_bP2PActivate;	}
	bool		IsP2PHost() const		{	ISNETWORKRETURNV(false,true);	return IsUseDedicationMode()?false:m_bP2PHost;	}

	bool		IsUseRelayServer() const		{	return m_ServerType==BATTLE_SERVER_TYPE_RELAY; }
	bool		IsUseDedicatedServer() const	{	return m_ServerType==BATTLE_SERVER_TYPE_DEDICATION; }
	bool		IsUseBattleServer()const		{	return (IsUseDedicatedServer() | IsUseRelayServer()) ; 	}
	bool		IsUseDedicationMode()const ;
	
	const PING*	GetPingInfo( void) const				{ return &m_PingInfo; }
	PING*		SetPingInfo( void)						{ return &m_PingInfo; }

	// P2P진행상황 함수를 모두 여기에 옮긴다..
		
	void	InitRoundData( void );				
	void	InitMulticastInfo( INT32 idx );

	void	InitP2PData(void);							// P2P초기화(전투에 입장하기전에 호출되어야함)

	// Battle Load (Init P2P data, Create UDPSocket, create receiver)
	void	CreateBattle( bool bLoaded = false );

	// Intdrude Battle (Select Host, Client mode)
	void	P2PIntrudeBattle( void);

	// Battle 종료
	void	P2PEndBattle( void );

	/** \brief 방장이 나간 경우
		\note 방장을 새로 세팅합니다. */
	void	P2PHostChange( INT32 nNewHostIdx );

	/** \brief 자신의 캐릭터를 등록하고 로딩을 시작한다. */
	void	P2PMyStartBattle( void);

	/** \brief 팀변경 */	
	void	ChangeTeam( void);  
	// chunjong.song 2014-09-03 : 공수교대가되어 팀아바타를 변경하는것은 해당게임이끝날떄까지 한번만 가능합니다.
	// hansoft://115.91.29.169;zepetto;77b90e39/Task/1095487  (예: 라운드마다 공수교대 불가능)

	/** \brief 자신의 캐릭터 로딩을 기다린다. */
	bool	WaitLoadingMyAvatar( void);

	/** \brief 유저들에 대한 정보를 받았다.
			다른 유저의 캐릭터를 생성하자.
		\param[in] bVisible 생성하는 캐릭터의 visible 상태.*/
	void	Battle_EntranceAnotherUser( bool bVisible = false, bool bForceCreateAvatar = true);

	// UDP NetInfo 세팅 및 홀 펀칭 세팅
	void	P2PPreStartBattleUser( INT32 iSlotidx );

	// 유저가 게임에 입장합니다. 
	bool	P2PStartBattleUser( INT32 iSlotidx, bool bStart = false);

	// 게임에서 나감
	void	P2PGiveupBattleUser( INT32 iSlotidx );

	/** \brief 캐릭터 부활 처리 */
	void	P2PRespawnChara( INT32 iSlotIdx);
	
	bool		IsMovingStage(void) const;	
	UINT32		GetPreStageID(void) const			{ return m_nPreStageID;		}
	void		MoveStageStart( UINT32 nStageID )	{ m_nPreStageID	= nStageID;	m_bMoveStage	= true;	}
	void		MoveStageEnd(void)					{ m_bMoveStage	= false;	}

	static void			sAfterLoadChara( void * pAvatar, INT32 idx = -1, bool bChangeParts = false, AI_BIND_DATA * pAIBind = nullptr);

private:
	

	void	_InitUserData( INT32 idx );					// 유저 데이터 초기화(네트워크 정보도 초기화함) in Battle

#if defined( DEF_OBSERVER_MODE)
	void	_InitUserDataForObserver( INT32 idx);		// Observer slot의 data 초기화
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Debug
	void	ProfileNetwork( void );


	BATTLE_SERVER_TYPE	m_ServerType;
	bool				m_bP2PHost;
	bool				m_bP2PActivate;

	PING				m_PingInfo;

	// 스테이지 점프 - 인게임내에서 다른 스테이지로
	bool		m_bMoveStage;
	UINT32		m_nPreStageID;

	bool		m_bWaitLoadForTeamChange;	// 팀 변경 시스템에 의해 자신의 캐릭터 로딩 확인용

	// revision 31898 리스폰 요청을 보낸후 10초후까지 리스폰 ACK를 받지 못할경우 레디룸으로 보냅니다.
private:
	REAL32					m_fCheckRespawnaAck;
	bool					m_bCheckRespawnaReq;
public:
	REAL32	getRespawnAckTime(){return m_fCheckRespawnaAck;}
	bool	IsRespawnReq(){return m_bCheckRespawnaReq;}
	void	setRespawnReq(bool bTrue){m_bCheckRespawnaReq = bTrue;}
	void	setRespawnAckTime(REAL32 rTime){ m_fCheckRespawnaAck = rTime;}

	void	OnUpdate( REAL32 rDeltaTime);

	bool	LoadingAnotherCharacter(void);

	// Skin이 로드 되기 전에 아바타 생성이 되면 머리가 잘린 형상으로 나와서 타이머를 두어
	// 조금 대기하다가 Skin이 로드 되도록 설정합니다.
private:

	// 10초간 대기 하면서 모든 파츠가 로드 되는지 확인이 필요
	UINT32		m_TimeCheck;
	UINT32		m_BaseTime;

	bool	_AnotherCharaTimer();
};



