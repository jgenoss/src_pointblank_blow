#if !defined( __IN_BATTLE_DEF_H__)
#define __IN_BATTLE_DEF_H__

struct KILLING_INFO
{
	// 죽임 종류
	UINT16			m_KillType;
	// 무기 정보
	UINT32			m_KillWeaponInfo;	//0x00 FF(Slotidx) FF(ClassType) FF(Number)
	// 연속 킬 카운트
	INT32			m_KillerAgainCount;
		// 죽은사람이 들고있던 무기
	UINT8			m_ui8Weapon[ MAX_KILLUSER_COUNT ];
	// 메시지 data
	UINT32			m_KillBuffer[ MAX_KILLUSER_COUNT ];	// 0x000 F(DefenceSystem) FF(Message) F(DeathType) F(Index)
	// 죽인 위치
	REAL32			m_KillPositionBuf[ MAX_KILLUSER_COUNT ][3];
	// 오브젝트 사망에 의한 오브젝트 인덱스
	INT32			m_KillObjectIdx;
	// 죽은 사람에 대한 어시스트 유저 인덱스
	UINT8			m_ui8AssistIdx[MAX_KILLUSER_COUNT];

	void	Reset( void);
	void	QueKillingInfo(UINT8 ui8DeathWeapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx, INT32 AssistIdx);
};

struct DEATH_ADD_INFO
{
	UINT32				_nDeathCount;	
	DEATH_INFO_CHARA	_DeathCharaInfo[MAX_KILLUSER_COUNT];
	UINT32				_KillWeaponFlag;
	INT32				_nByObjectIdx;
} ;

struct PING_INFO
{
	// 핑 센드 검사 시간
	REAL32			m_fPingSendTimer;
	// 핑 검사를 보낸 시각
	REAL32			m_rPingSendTime;
	// 핑 계산을 한 값(실제 화면에 뿌리기 위한 용도)
	UINT8			m_Pings[ SLOT_MAX_COUNT ];

	// Ping 검사 시간
	REAL32			m_rPingCheckTime;
	// 계산용 Packet을 받으면 이전에 보낸 시간으로부터 타임을 계산한다.
	REAL32			m_rPingList[ SLOT_MAX_COUNT ];		// m_PingList

	/** \brief 처음 초기화 함수 */
	void	Init( void);

	/** \brief Round 시작시 초기화 함수 */
	void	Reset( void);

	/** \brief Ping Packet을 받으면 처리한다. */
	void	RecvPing( INT32 userIdx, REAL32 sendTime, REAL32 elapsedTime);
};

struct BATTLE_RESULT_INFO
{
	UINT16		m_pBattleEventExp;
	UINT16		m_pBattleEventPoint;
	UINT8		m_pBattleResultIcon;
	UINT16		m_BattleExp;
	UINT16		m_BattlePoint;
	UINT16		m_BattleScore;

	UINT8		m_AccMissionCount;

	void	Init( void);
};

struct ASSIST_DETAIL_INFO
{
	INT32	m_i32TotalDamage;				// 총 누적 데미지
	UINT16	m_ui16AttackTurn;

	void	Reset(void);					// 유저가 죽었을 시 어시스트 정보 초기화
	void	SetAssistInfo(INT32 i32Damage, UINT16 ui16AttackTurn);
};

struct ASSIST_INFO
{
	ASSIST_DETAIL_INFO m_DetailInfo[SLOT_MAX_COUNT];
	UINT16	m_ui16AssistOrder;				//타격순서.


	void	Reset(void);					// 어시스트 정보 전체 초기화
	void	SlotReset(UINT32 ui32Slotidx);	// 특정 slotidx 초기화
	void	OrderReset(void);				// AssistOrder 리셋
	void	SetAssist(UINT8 ui8ShooterIdx, INT32 i32Damage); // 어시스트 정보 set
	INT8	GetAssistidx(UINT32 ui32KillIdx);		 // 어시스트 유저 Idx 얻기
};


#endif

