#ifndef __COMMONDEF_ROOM_H__
#define __COMMONDEF_ROOM_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define
#define GET_DEATHINFO_IDX_TYPE( Type )				(Type & 0x0000000F)
#define GET_DEATHINFO_DEATH_TYPE( Type )			((Type & 0x000000F0) >> 4)
#define GET_DEATHINFO_MESSAGE_TYPE( Type )			((Type & 0x00FFFF00) >> 8)
#define GET_DEATHINFO_ENEMY_TYPE( Type )			((Type & 0x0F000000) >> 24) 
#define GET_DEATHINFO_MY_TYPE( Type )				((Type & 0xF0000000) >> 28)

#define SET_DEATHINFO_IDX_TYPE( Type, State )		( Type | (State & 0x0000000F) )
#define SET_DEATHINFO_DEATH_TYPE( Type, State )		( Type | ((State << 4)&0x000000F0) )
#define SET_DEATHINFO_MESSAGE_TYPE( Type, State )	( Type | ((State << 8)&0x00FFFF00) )
#define SET_DEATHINFO_ENEMY_TYPE( Type, State )		( Type | ((State << 24)&0x0F000000) )
#define SET_DEATHINFO_MY_TYPE( Type, State )		( Type | ((State << 28)&0xF0000000) )

//Mission 
#define MISSION_PREBATTLE_TIME		5			//5초 시작전 대기타임 ( CC모드, 탈출모드,탈취모드 사용 )
#define MISSION_TEAM_CHANGE_TIME	5			//5초 팀변경타임 ( 폭파미션 팀변경 라운드에만 사용 )
#define MISSION_FREE_TIME			5			//5초 프리타임
#define MISSION_RESULT_TIME			3			//3초 결과타임
#define COUNT_DOWN_TIME				3			// 배틀 시작전 3초 카운트다운. 폭파미션, 섬멸전만 사용.

// 게임 설정 - 주: 팀, 데스메치
#define BATTLE_KILL_TYPE_60			0x00
#define BATTLE_KILL_TYPE_80			0x01
#define BATTLE_KILL_TYPE_100		0x02
#define BATTLE_KILL_TYPE_120		0x03
#define BATTLE_KILL_TYPE_140		0x04
#define BATTLE_KILL_TYPE_160		0x05

// 게임 설정 - 부
#define BATTLE_TIME_MISSION_TYPE_3	0x00
#define BATTLE_TIME_MISSION_TYPE_5	0x10
#define BATTLE_TIME_MISSION_TYPE_7	0x20

#define BATTLE_TIME_TYPE_3			0x00
#define BATTLE_TIME_TYPE_5			0x30
#define BATTLE_TIME_TYPE_10			0x40
#define BATTLE_TIME_TYPE_15			0x50
#define BATTLE_TIME_TYPE_20			0x60
#define BATTLE_TIME_TYPE_25			0x70
#define BATTLE_TIME_TYPE_30			0x80
#define BATTLE_TIME_TYPE_20s		0xA0

#define MAX_NPC_COUNT				16

#define GET_QUEST_EXP_POINT			0x00000001
#define GET_QUEST_WEAPON			0x00000010
#define GET_QUEST_FINISH_LAST_CARD	0x00000100

#define RESULT_ICON_PC				0x0001	// 일반 피씨방
#define RESULT_ICON_PC_PLUS			0x0002	// 프리미엄 피씨방
#define RESULT_ICON_ITEM			0x0004	// 기능성 아이템
#define RESULT_ICON_EVENT			0x0008	// 이벤트(채널,맵 등)
#define RESULT_ICON_PC_ID			0x0010	// 
#define RESULT_ICON_GWARNET			0x0020	// 인도네시아 GWarnet 마크
#define RESULT_ICON_PC_HOUSE		0x0040	// 정액제 유저
#define RESULT_ICON_GARENA_GCA		0x0080	// 태국 가레나 GCA 마크

// 챌린지모드 난이도
#define CHALLENGE_DIFFICULTY_LEVEL_NONE		0
#define CHALLENGE_DIFFICULTY_LEVEL_HIGH		10
#define CHALLENGE_DIFFICULTY_LEVEL_LOW		1

#define		MODE_DATA_MAX	100
#define		MODE_TIMER_MAX	5

#define GET_KILLTYPE_HIGH(t)			(t&0xF0)
#define SET_KILLTYPE_HIGH(t,bit)		(t|(bit&0xF0))

#define GET_KILLTYPE_LOW(t)				(t&0x0F)
#define SET_KILLTYPE_LOW(t,Idx)			((t&0xF0)|(Idx&0x0F))

#define	TEAMBATTLE_TIME_COUNT			11			//클라이언트에서 이값을 사용하면 -1을해줘야 함	 
#define TEAMBATTLE_KILL_COUNT			7			//클라이언트에서 이값을 사용하면 -1을해줘야 함	
#define MISSION_ROUND_TIME_COUNT		30			//

#define		MAX_MULTI_KILL_COUNT	99	// 멀티킬 최대 표시횟수.


////////////////////////////////////////////////////////////////////////////////
// 두 종류의 #define를 같이 고쳐야 함. - 박명진 2008. 11. 14
#define ROOM_INFO_FLAG_INTER_ENTER			0x80
#define ROOM_INFO_FLAG_OBSERVER				0x40
#define ROOM_INFO_FLAG_TEAM_BALANCE			0x20
#define ROOM_INFO_FLAG_REAL_IP				0x10			// 더 이상 필요가 있을지 의문 입니다.
#define ROOM_INFO_FLAG_OBSERVER_MODE		0x08
#define ROOM_INFO_FLAG_PRIVATE_MODE			0x04
#define ROOM_INFO_FLAG_RANDOM_MODE			0x02
#define ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE	0x01

#define GET_ROOM_INFO_INTERENTER( state )			((state>>7) & 0x00000001)
#define GET_ROOM_INFO_OBSERVER( state )				((state>>6) & 0x00000001)
#define GET_ROOM_INFO_TEAMBALANCE( state )			((state>>5) & 0x00000001)
#define GET_ROOM_INFO_REAL_IP( state )				((state>>4) & 0x00000001)
#define GET_ROOM_INFO_OBSERVER_MODE( state )		((state>>3) & 0x00000001)
#define GET_ROOM_INFO_PRIVATE_MODE( state )			((state>>2) & 0x00000001)
#define GET_ROOM_INFO_RANDOM_MODE( state )			((state>>1) & 0x00000001)
#define GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE( state )	((state>>0) & 0x00000001)

#define SET_ROOM_INFO_INTERENTER( state, flag )				((state & 0x7F) | ((flag & 0x00000001) << 7))
#define SET_ROOM_INFO_OBSERVER( state, flag )				((state & 0xBF) | ((flag & 0x00000001) << 6))
#define SET_ROOM_INFO_TEAMBALANCE( state, flag )			((state & 0xDF) | ((flag & 0x00000001) << 5))
#define SET_ROOM_INFO_REAL_IP( state, flag )				((state & 0xEF) | ((flag & 0x00000001) << 4))
#define SET_ROOM_INFO_OBSERVER_MODE( state, flag )			((state & 0xF7) | ((flag & 0x00000001) << 3))
#define SET_ROOM_INFO_PRIVATE_MODE( state, flag )			((state & 0xFB) | ((flag & 0x00000001) << 2))
#define SET_ROOM_INFO_RANDOM_MODE( state, flag )			((state & 0xFD) | ((flag & 0x00000001) << 1))
#define SET_ROOM_INFO_ATK_DEF_AUTO_CHANGE( state, flag )	((state & 0xFE) | ((flag & 0x00000001) << 0))

#define GET_ROOM_INFO_WEAPON_KNUCKLE( state )	((state>>7)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_SHOTGUN( state )	((state>>6)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_SNIPER( state )	((state>>5)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_RPG7( state )		((state>>4)	& 0x00000001)
#define GET_ROOM_INFO_WEAPON_00( state )		((state>>3) & 0x00000001)
#define GET_ROOM_INFO_WEAPON_01( state )		((state>>2) & 0x00000001)
#define GET_ROOM_INFO_WEAPON_02( state )		((state>>1) & 0x00000001)
#define GET_ROOM_INFO_WEAPON_03( state )		((state   ) & 0x00000001)

#define SET_ENABLE_RANDOM_MAP( value, map, enable )			{if(enable) value[ map/STAGE_AC ] |= (1LL << (map%STAGE_AC));	else value[ map/STAGE_AC ] &= ~(1LL << (map%STAGE_AC)); }
#define GET_ENABLE_RANDOM_MAP( value, map )					((value[ map/STAGE_AC ] >> (map%STAGE_AC)) & 0x1)

#define MULTI_WEAPON_SLOT_MIN	1	// 다중무기슬롯 최소 개수
#define MULTI_WEAPON_SLOT_MAX	10	// 다중무기슬롯 최대 개수
// 여기까지 (두 종류의 #define를 같이 고쳐야 함. - 박명진 2008. 11. 14)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum

enum EWEAPON_TYPE
{
	WT_RPG7_OUTPOST = 0,
	WT_RPG7_BOSS,
	WT_K201,
	WT_CHINESE_CLEAVER,
	WT_CROWNWATERMELON,
	WT_RPG7,

	WT_COUNT_MAX,
};

// 탈취물 상태
enum USURPATION_OBJECT_STATE
{
	//USURPATION_OBJECT_STATE_GET,	
	// 유저 소지 상태는 슬롯 인덱스로 대체 합니다. 0~15
	USURPATION_OBJECT_STATE_GET_SLOT_0 = 0,	
	USURPATION_OBJECT_STATE_GET_SLOT_1 = 1,	
	USURPATION_OBJECT_STATE_GET_SLOT_2 = 2,	
	USURPATION_OBJECT_STATE_GET_SLOT_3 = 3,	
	USURPATION_OBJECT_STATE_GET_SLOT_4 = 4,	
	USURPATION_OBJECT_STATE_GET_SLOT_5 = 5,	
	USURPATION_OBJECT_STATE_GET_SLOT_6 = 6,	
	USURPATION_OBJECT_STATE_GET_SLOT_7 = 7,	
	USURPATION_OBJECT_STATE_GET_SLOT_8 = 8,	
	USURPATION_OBJECT_STATE_GET_SLOT_9 = 9,	
	USURPATION_OBJECT_STATE_GET_SLOT_10 = 10,	
	USURPATION_OBJECT_STATE_GET_SLOT_11 = 11,	
	USURPATION_OBJECT_STATE_GET_SLOT_12 = 12,	
	USURPATION_OBJECT_STATE_GET_SLOT_13 = 13,	
	USURPATION_OBJECT_STATE_GET_SLOT_14 = 14,	
	USURPATION_OBJECT_STATE_GET_SLOT_15 = 15,	

	USURPATION_OBJECT_STATE_GOAL_IN		= 253,	// 목표지점 도달 상태.
	USURPATION_OBJECT_STATE_DROP		= 254,	// 소지 유저 사망으로 인해 바닥에 떨어진 상태.
	USURPATION_OBJECT_STATE_DEFAULT		= 255,	// 초기 리스폰 상태.

};

// 게임 설정 - 주: 미션모드에서 사용합니다.
enum BATTLE_ROUND_TYPE
{
	BATTLE_ROUND_TYPE_1	= 0,
	BATTLE_ROUND_TYPE_2,
	BATTLE_ROUND_TYPE_3,
	BATTLE_ROUND_TYPE_5,
	BATTLE_ROUND_TYPE_7,
	BATTLE_ROUND_TYPE_9,

	BATTLE_ROUND_TYPE_ENDLESS,

	BATTLE_ROUND_TYPE_MAX,
};

// 죽인 방법 - 0xF(16개) 이상을 사용하면 않됨.
enum CHARA_DEATH_TYPE
{
	CHARA_DEATH_UN = 0,
	CHARA_DEATH_A,					// default
	CHARA_DEATH_B,					// Bome! = Granade
	CHARA_DEATH_H,					// Head Shot!!
	CHARA_DEATH_F,					// Falling death
	CHARA_DEATH_O,					// by Object Explosion
	CHARA_DEATH_FAST_OBJECT,		// by Fast Object, Damage Object
	CHARA_DEATH_POISON,				// 독에 의한 사망
	CHARA_DEATH_TRAMPLED,			// 짓밟힘
	CHARA_DEATH_HOWL,				// HOWL에 놀라 죽음;;;
//	CHARA_DEATH_ASSIST,				// Assistkill;;;
	CHARA_DEATH_BUFF,				// 엘리트 버프
	CHARA_DEATH_TEMP5,
	CHARA_DEATH_TEMP4,
	CHARA_DEATH_TEMP3,
	CHARA_DEATH_TEMP2,
	CHARA_DEATH_TEMP1,
	
	CHARA_DEATH_MAXCOUNT
};

// 죽였을때 적의 상태 - 0xF(16개) 이상 사용하면 안됨
enum CHARA_DEATH_ENEMY_STATE_TYPE
{
	CHARA_DEATH_ENEMY_STATE_NONE	= 0,
	CHARA_DEATH_ENEMY_STATE_DEFENCE,		// 방어미션에서 Defence
	CHARA_DEATH_ENEMY_STATE_ESCAPE,			// 탈출미션에서 탈출 직전
	CHARA_DEATH_ENEMY_STATE_SAVER,			// 제압미션에서 Saver
	CHARA_DEATH_ENEMY_STATE_TEMP12,
	CHARA_DEATH_ENEMY_STATE_TEMP11,
	CHARA_DEATH_ENEMY_STATE_TEMP10,
	CHARA_DEATH_ENEMY_STATE_TEMP9,
	CHARA_DEATH_ENEMY_STATE_TEMP8,
	CHARA_DEATH_ENEMY_STATE_TEMP7,
	CHARA_DEATH_ENEMY_STATE_TEMP6,
	CHARA_DEATH_ENEMY_STATE_TEMP5,
	CHARA_DEATH_ENEMY_STATE_TEMP4,
	CHARA_DEATH_ENEMY_STATE_TEMP3,
	CHARA_DEATH_ENEMY_STATE_TEMP2,
	CHARA_DEATH_ENEMY_STATE_TEMP1,

	CHARA_DEATH_ENEMY_STATE_MAXCOUNT,
};

// 죽였을때 나의 상태 - 0xF(16개) 이상 사용하면 안됨
enum CHARA_DEATH_MY_STATE_TYPE
{
	CHARA_DEATH_MY_STATE_NONE	= 0,
	CHARA_DEATH_MY_STATE_ASSIST,			// 탈출미션에서 Assist
	CHARA_DEATH_MY_STATE_TEMP14,			
	CHARA_DEATH_MY_STATE_TEMP13,
	CHARA_DEATH_MY_STATE_TEMP12,
	CHARA_DEATH_MY_STATE_TEMP11,
	CHARA_DEATH_MY_STATE_TEMP10,
	CHARA_DEATH_MY_STATE_TEMP9,
	CHARA_DEATH_MY_STATE_TEMP8,
	CHARA_DEATH_MY_STATE_TEMP7,
	CHARA_DEATH_MY_STATE_TEMP6,
	CHARA_DEATH_MY_STATE_TEMP5,
	CHARA_DEATH_MY_STATE_TEMP4,
	CHARA_DEATH_MY_STATE_TEMP3,
	CHARA_DEATH_MY_STATE_TEMP2,
	CHARA_DEATH_MY_STATE_TEMP1,

	CHARA_DEATH_MY_STATE_MAXCOUNT,
};
// 16가지 종류밖에 사용할수 없습니다. 20110624 김현우
enum CHARA_KILL_TYPE
{
	CHARA_KILL_DEFAULT = 0,
	CHARA_KILL_PIERCING,
	CHARA_KILL_MASS,
};
// 4가지 종류밖에 사용할수 없습니다. 20110624 김현우
enum CHARA_KILL_TYPE_HIGH
{
	CHARA_KILL_HIGH_BOMBING	= 0x10,
	CHARA_KILL_HIGH_TEMP2	= 0x20,
	CHARA_KILL_HIGH_TEMP1	= 0x40,
	CHARA_KILL_HIGH_TEMP	= 0x80,
};

//미션 라운드 엔드 타입 
enum MISSION_END_TYPE
{
	MISSION_END_TYPE_NONE			= 0,
	MISSION_END_TIMEOUT				= 1,
	MISSION_END_BOMBFIRE			= 2,
	MISSION_END_UNINSTALL			= 3,
	MISSION_END_ALLDEATH			= 4,	
	MISSION_END_GENERATOR_DESTROY	= 5,			// 파괴 미션 오브젝트 파괴
	MISSION_END_DEFENCE_DESTROY		= 6,			// 방어 미션 오브젝트 파괴
	MISSION_END_TUTORIAL_COMPLETE	= 7,			// 튜토리얼 임무 완료
	MISSION_END_MAX_KILL			= 8,			// 데스매치에서 목표 킬카운트 달성
	MISSION_END_USURPATION			= 9,			// 탈취모드 탈취성공

	MISSION_END_TYPE_MAX
};

enum KILL_TYPE
{
	KILL_TYPE_DEFAULT			= 0,
	KILL_TYPE_MASS				,
	KILL_TYPE_PIERCE			,
	KILL_TYPE_HEADSHOT			,
	KILL_TYPE_CHAIN_HEADSHOT	,
	KILL_TYPE_CHAIN_SLUGGER		,
	KILL_TYPE_CHAIN_STOPPER		,
	KILL_TYPE_SUICIDE			,

	KILL_TYPE_COUNT				,
};

enum ROOM_SCORE_TYPE
{
	ROOM_SCORE_NONE_TYPE		= 0,
	ROOM_SCORE_KILLCOUNT_TYPE,			// 1 : 게임 경과시간 n:n 현재 스코어 n : n
	ROOM_SCORE_ROUNDCOUNT_TYPE,			// 2 : 현재 스코어 n : n
	ROOM_SCORE_AI_TYPE,					// 3 : 게임 경과시간 n:n 현재 난이도 n(EASY/NORMAL/HARD)
	ROOM_SCORE_DOMI_TYPE,				// 4 : 인터미션 경과시간. 레드팀 스코어에 라운드. 블루팀 스코어에 웨이브.

	MAX_ROOM_SCORE_TYPE
};

/// RadioChat Type
// RadioChat은 Type * 10 + Number(0~9) 조합으로 UDP통신 패킷을 보냅니다.
enum RADIOCHAT_TYPE
{
	RADIOCHAT_TYPE_COMMAND	= 0,
	RADIOCHAT_TYPE_REQUEST,
	RADIOCHAT_TYPE_STATUS,
	RADIOCHAT_TYPE_AUTO,

	RADIOCHAT_TYPE_COUNT
};

// 방어 미션 오브젝트 개수
enum MISSION_OBJECT_COUNT
{
	MISSION_FIRST_DEFENCE_OBJECT = 0,	// 첫번째 오브젝트
	MISSION_SECOND_DEFENCE_OBJECT,		// 두번째 오브젝트

	MAX_OBJECT_COUNT					// 최대 오브젝트의 수
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function
//Battle Info Table 
extern const UINT32	g_TeamTimeTable[TEAMBATTLE_TIME_COUNT];
extern const UINT32	g_TeamKillCountTable[TEAMBATTLE_KILL_COUNT]; 
extern const UINT32	g_MissionRoundCountTable[BATTLE_ROUND_TYPE_MAX];
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Struct
#pragma pack( push, 1)

struct ROOM_SLOT_CHANGE_INFO
{
	UINT8	ui8OldSlot = 0;
	UINT8	ui8NewSlot = 0;
	UINT8	ui8OldSlotState = 0;
	UINT8	ui8NewSlotState = 0;
};

//슬롯 정보 
struct SLOT_INFO
{	
	UINT8		_State = 0;				//현재 상태	
	UINT8		_Rank = 0;				//랭크 51단계.	( Level ); 
	UINT32		_clanidx = 0;			//
	UINT32		_clanstate = 0;			//
	UINT8		_clanExpRank = 0;		//
	UINT32		_clanMark = CLAN_MARK_DEFAULT;	//
	UINT8		_pcCafe = 0;			//
	UINT8		_eSportID = 0;			//
	UINT32		_Item = 0;				// 1111111111110000
									// SLOT_ITEM_QUICK_CHANGE_WEAPON
									// SLOT_ITEM_QUICK_CHANGE_MAGAZINE
									// SLOT_ITEM_MEGA_HP10
									// SLOT_ITEM_INCREASE_INVINCIBLE_TIME
									// SLOT_ITEM_BULLET_PROOF_VEST
									// SLOT_ITEM_FULLMETALJACKETAMMO
									// SLOT_ITEM_HOLLOW_POINT_AMMO
									// SLOT_ITEM_C4_SPEED_KIT
									// SLOT_ITEM_INCREASE_GRENADE_SLOT
									// SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO
									// SLOT_ITEM_MEGA_HP5
									// SLOT_ITEM_BULLET_PROOF_VEST_PLUS
	UINT32		_CharaItemID = 0;		// 케릭터 아이템아이디
	TTCHAR		_ClanName[NET_CLAN_NAME_SIZE] = { 0 };	// 클랜 이름

	bool		_bIsGmObserverMode = false;
	UINT8		_ui8NationalCode = 0;

	void Reset( UINT8 ui8State = SLOT_STATE_EMPTY )
	{
		_State = ui8State;
		_Rank = _clanExpRank = _pcCafe = _eSportID = 0;
		_clanidx = _clanstate = 0;
		_clanMark = CLAN_MARK_DEFAULT;
		_Item = 0;
		_ClanName[0] = '\0';
		_bIsGmObserverMode = false;
		_ui8NationalCode   = 0;
	}
};

struct DEATH_INFO_CHARA
{
	UINT8	_ui8Weapon = 0;					// Weapon
	UINT32	_DeathInfo = 0;					// MyState(0xF0000000), EnemyState(0x0F000000), Message(0x00FFFF00), DeathType(0x000000F0), Index(0x0000000F)
	REAL32	_DeathPosition[3] = { 0.0f, 0.0f, 0.0f };			// 2Byte position( x, y, z )
	UINT8	_ui8AssistIdx = 0;				// Assist User Idx
};

//방 정보 
struct ROOM_INFO_BASIC
{
	INT32	_Idx = 0;
	TTCHAR	_Title[NET_ROOM_NAME_SIZE] = { 0 };		// 타이틀명
	UINT32	_StageID = 0;						//
	UINT8	_State = 0;							// 상태	
	UINT8	_PersonNow = 0;						// 들어와 있는 사람수
	UINT8	_PersonPlayerMax = 0;				// 플레이어인 전체 사람수 	
	UINT8	_Ping = 0;							// 핑	
	UINT8	_WeaponFlag = 0x0F;					// 0x01111111, 0, 샷건, 저격, RPG7, 주, 보조, 칼, 투척
	UINT8	_InfoFlag = 0;						// 0x11111000, 난입제한, 관전제한, 팀벨런스, RealIP,옵저버모드	
	UINT32	_OptionFlag = 0;					// 0x0000 0000 0000 0000 0000 0000 0000 1111 , 랜덤맵 인원 정보(5:5, 4:4, 8:8)
	bool	_IsClanMatch = false;					// 클랜매치로 사용중인 방인지

	void	Reset() {
		_Title[0]		= 0;
		_Idx			= 0;
		_StageID		= 0;
		_State			= _PersonNow = _PersonPlayerMax = _Ping = _InfoFlag = 0;
		_WeaponFlag		= 0x0F;
		_OptionFlag		= 0;
		_IsClanMatch	= false;
	}
};	

struct ROOM_INFO_ADD
{
//protected:
#ifdef USE_COLOR_NICK2
	UINT8	_NicklColor = 0;						// 방장 Nick Color
#endif
	TTCHAR	_Nick[NET_NICK_NAME_SIZE] = { 0 };			// 방장 Nick	

public:
	UINT8	_SubType = 0;							// Time, kill	
	UINT8	_NowRoundCount = 0;						// 라운드 카운트 
	UINT16	_NowIngTime = 0;						// 타임 카운트(진행시간)	
	UINT8	_LockInterEnter = 0;					// 1 Lock, 0 UnLock	
	UINT8	_LockObserver = 0;						// 0x00001111,	(상대진영HP,상대진영,자유관전,3인칭자유)
	UINT8	_LockTeamBalance = 0;					// 1 Lock, 0 UnLock (default unlock)
	UINT8	_AIState = 0;							// 1 DieHard 0 Original (default Original)
	UINT64	_ui64OpenStage[MAX_OPEN_STAGE] = { 0 };	// random 에서 열릴 스테이지 값

	TTCHAR *	getNick(void)				{ return _Nick; }
	void		setNick( const TTCHAR * pszName)
	{
		I3ASSERT( pszName != nullptr);
				
		I3ASSERT( wcslen( pszName) < NET_NICK_NAME_SIZE);
		wcscpy_s( _Nick, NET_NICK_NAME_SIZE, pszName );
	}

	void		Reset(void)
	{
#ifdef USE_COLOR_NICK2
		_NicklColor			= 0;
#endif
		_Nick[0]			= '\0';
		_SubType			= 0x00;
		_NowRoundCount		= 0x00;
		_NowIngTime			= 0;
		_LockInterEnter		= 0;
		_LockObserver		= 0;
		_LockTeamBalance	= 0;		
		_AIState			= 0;
	}
};	

struct ROOM_INFO_ALL
{
	ROOM_INFO_BASIC _Basic;  
	ROOM_INFO_ADD	_Add;
};	

struct ROOM_NOW_SCORE
{
	INT32	ScoreType = 0;				// ROOM_SCORE_TYPE
	INT32	PlayTime = 0;				// 00초
	INT32	Score[TEAM_COUNT] = { 0 };	// Red : Blue
};

struct PACKET_BATTLE_NEW_JOIN_ROOM_SCORE_ACK
{
	ROOM_NOW_SCORE		m_RoomNowScore;				// RoomNowScore
};

// Score
struct SCORE_INFO
{
	UINT16		m_KillCount = 0;
	UINT16		m_DeathCount = 0;
	UINT16		m_AssistCount = 0;

	void	Reset() { m_KillCount = m_DeathCount = m_AssistCount = 0; }
	SCORE_INFO&	operator=( const SCORE_INFO& _info)
	{
		m_KillCount = _info.m_KillCount;
		m_DeathCount = _info.m_DeathCount;
		m_AssistCount = _info.m_AssistCount;
		return *this;
	}
}; 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// InBattle

struct DEATH_INFO_HEADER
{										// 0xF (High-BitOR)			F(Low-KillIdx)
	UINT8	_nKillType = 0;					// 0x1(자폭) 0x2(미정)		0(Default), 1(Piercing), 2(Mass)
	UINT8	_nCount = 0;					// deafult이면 1
	UINT8	_nUserIdx = 0;					// killer user SlotIdx
	UINT32	_WeaponInfo = 0;				// aabbcccddd - aa:Class, bb:USAGE, ccc:TYPE(Weapon Class), ddd:ItemNumber
	REAL32	_KillPosition[3] = { 0.0f, 0.0f, 0.0f };			// 2Byte position( x, y, z )
	UINT8	_nByObjectIdx = 0;				// 오브젝트에 의한 사망시 오브젝트 인덱스 ( _nKillType == CHARA_KILL_BYOBJECT)
} ;

// 파괴미션 정보 오브젝트
struct DESTRUCTION_INFO
{
	UINT16		_nRedObjectHP = 0;
	UINT16		_nBlueObjectHP = 0;
	UINT16		_nUserDamage[SLOT_MAX_COUNT] = { 0 };		//변수 이름 변경해야함 퍼센트 아님

	void		Reset( void)
	{
		_nRedObjectHP		= 0;
		_nBlueObjectHP		= 0;
		for( INT32 i=0; i<SLOT_MAX_COUNT;i++)
		{
			_nUserDamage[ i] = 0;
		}
	}

	void		CopyTo( DESTRUCTION_INFO * pDest)
	{
		pDest->_nRedObjectHP = _nRedObjectHP;
		pDest->_nBlueObjectHP = _nBlueObjectHP;
		memcpy( pDest->_nUserDamage, _nUserDamage, sizeof(UINT16)*SLOT_MAX_COUNT );
	}
	void		AddUserDamage(INT32 idx, INT32 iDamage)
	{
		INT32 iMax = (INT32)ldexp( 1.f, sizeof(_nUserDamage[0])*8 ) - 1;
		INT32 iTemp = _nUserDamage[ idx] + iDamage;	

		if(iTemp > iMax) {
			iTemp = iMax;
		}

		_nUserDamage[ idx] = (UINT16)iTemp;
	}
};

// 디펜스 미션 오브젝트 정보
struct DEFENCE_INFO
{	
	UINT16		_nObjectHP[MAX_OBJECT_COUNT] = { 0 };
	UINT16		_nUserDamage[MAX_OBJECT_COUNT][SLOT_MAX_COUNT] = { 0 };		// [Red : 두개의object 각각에 대한 유저당 피해량],[Blue : 각각의 오브젝트에 대한 방어 성공시 보너스]

	void		Reset( void)
	{
		for(INT32 Loop = 0; Loop < MAX_OBJECT_COUNT; Loop++)
		{
			_nObjectHP[Loop]		= 0;

			for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				_nUserDamage[Loop][i] = 0;
			}
		}
	}

	void		CopyTo( DEFENCE_INFO * pDest)
	{
		for(INT32 Loop = 0; Loop < MAX_OBJECT_COUNT; Loop++)
		{
			pDest->_nObjectHP[Loop] = _nObjectHP[Loop];
		}

		for(INT32 Loop2 = 0; Loop2 < MAX_OBJECT_COUNT; Loop2++)
		{
			for( INT32 i=0; i<SLOT_MAX_COUNT; i++)
			{
				pDest->_nUserDamage[Loop2][i] = _nUserDamage[Loop2][i];
			}
		}
	}

	void		CopyToBlueDefenceInfo( DEFENCE_INFO * pDest, UINT8 ui8Changed)
	{
		for( INT32 Loop2 = 0; Loop2 < MAX_OBJECT_COUNT; Loop2++ )
		{
			for( INT32 i=0; i<SLOT_MAX_COUNT; i++ )
			{
				if( (i % TEAM_COUNT) != ui8Changed )
					pDest->_nUserDamage[Loop2][i] = _nUserDamage[Loop2][i];
			}
		}
	}

	UINT16		getObjectHP( INT32 idx) const
	{
		if (idx < 0 || idx >= MAX_OBJECT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_OBJECT_COUNT);	return 0;
		}
		return _nObjectHP[ idx];
	}

	void		setObjectHP( INT32 idx, UINT16 hp)
	{
		if (idx < 0 || idx >= MAX_OBJECT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_OBJECT_COUNT);	return;
		}
		_nObjectHP[ idx] = hp;
	}

	void		AddUserDamage(INT32 iObjectIdx, INT32 idx, INT32 iDamage)
	{
		INT32 iMax = (INT32)ldexp( 1.f, sizeof(_nUserDamage[iObjectIdx][0])*8 ) - 1;
		INT32 iTemp = _nUserDamage[iObjectIdx][idx] + iDamage;	

		if(iTemp > iMax) {
			iTemp = iMax;
		}

		_nUserDamage[iObjectIdx][idx] = (UINT16)iTemp;
	}

	UINT16		getUserDamage( INT32 iObjectIdx, INT32 idx) const
	{
		if (iObjectIdx < 0 || iObjectIdx >= MAX_OBJECT_COUNT )
		{
			I3_BOUNDCHK(iObjectIdx, MAX_OBJECT_COUNT);	return 0;
		}

		if (idx < 0 || idx >= SLOT_MAX_COUNT)
		{
			I3_BOUNDCHK(idx, SLOT_MAX_COUNT);	return 0;
		}

		return _nUserDamage[ iObjectIdx][ idx];
	}

	void		setUserDamage( INT32 iObjectIdx, INT32 idx, UINT16 damage)
	{
		if (iObjectIdx < 0 || iObjectIdx >= MAX_OBJECT_COUNT)
		{
			I3_BOUNDCHK(iObjectIdx, MAX_OBJECT_COUNT);
			return;
		}
		if (idx < 0 || idx >= SLOT_MAX_COUNT)
		{
			I3_BOUNDCHK(idx, SLOT_MAX_COUNT);
			return;
		}
		_nUserDamage[ iObjectIdx][ idx] = damage;
	}

};

//공룡임무카드 데스블로우를 위해 추가된 구조체
struct DEATHBLOW_INFO
{
	UINT8	_nTrexIdx = 0;					// Target user SlotIdx
	UINT32	_WeaponInfo = 0;				// 0x00 FF(SlotIdx ) FF(ClassType) FF(Number)
};

// 챌린지모드 NPC 정보
struct AI_ENTRY
{
	INT32		_level = 0;
	INT32		_number = 0;
	INT8		_hp = 0;
	REAL32		_respawnTime = 0.0f;

	void		Reset()
	{
		_level = 0;
		_number = 0;
		_hp = 0;
		_respawnTime = 0.0f;
	}
};

struct MULTI_KILL_INFO
{
	UINT8				m_ui8MaxMultiKillCount = 0;		// 배틀 한판중 리스폰과 리스폰 사이에 최대 멀티킬수.( 최소 2이상 ) 
	UINT16				m_ui16MultiKillUser = 0;		// 최대 멀티킬을 달성한 유저 슬롯인덱스 순서대로 비트플래그.(복수유저 가능). ex : 0번,2번슬롯 유저일시. 2진수 : 0000000000000101
};

struct KILLDATA
{
	DEATH_INFO_HEADER	head;
	DEATH_INFO_CHARA	info[ MAX_KILLUSER_COUNT ];
};

/////////////////////////////////////////////////////////////////////
//For P2P
//----------------------------------------------------------------------------------------------------------------
// InGame [P2P] Default UDP Packet Header
// N_GAME_HEADER
//	기본 Packet header
struct N_GAME_HEADER
{
	UINT8	_ProtocolID = 0;	// 데이타 프로토콜 타입
	UINT8	_Index = 0;			// 유저 슬롯 인덱스
	REAL32	_GameTime = 0.0f;		// Game Time
	UINT8	_RoundNum = 0;		// round number : 일치하지 않으면 패킷 버림
	UINT16	_Size = 0;			// 패킷 사이즈
	UINT8	_RespawnCount = 0;	// 리스폰 카운트( 리스폰 싱크를 맞추기 위한 값으로 데디 서버와 값이 다를경우 무시됩니다. - 일부 패킷 제외 )
	
	UINT8	_TouchDownCount = 0;// 터치다운 카운트( 터치다운 싱크를 맞추기 위한 값으로 데디 서버와 값이 다를경우 무시됩니다. - 일부 패킷 제외 )
//	UINT8	_State;			// 탈취모드 - 탈취물 상태 ( USURPATION_OBJECT_STATE )
							
	UINT8	_ui8UID = 0;		// user id(64bit)를 % 256한 값(user 확인용)
	UINT8	_ui8Extra = 0;		// DU 작업용 여분 패킷 추가

#ifdef __USE_PACKET_OPTIMIZE__
	UINT8	m_ui8PacketUpdateIdx = 0;		// 패킷 업데이트 시간
#endif

#ifndef _CONTROL_TOOL_
	//REAL16	_r16ObjectPos[3] = { 0.0f, 0.0f, 0.0f };	// 탈취모드 - 탈취물 포지션
#endif

	void	Reset()
	{
		_ProtocolID = _Index = 0;
		_GameTime = 0.f;
		_RoundNum = 0;
		_Size = 0;
		_RespawnCount = 0;
		_TouchDownCount = 0;
//		_State = USURPATION_OBJECT_STATE_DEFAULT;
		_ui8UID = 0;
		_ui8Extra  = 0;
#ifndef _CONTROL_TOOL_
		//i3mem::FillZero( _r16ObjectPos, sizeof(REAL16)*3 );
#endif

	}

	void	CopyTo( N_GAME_HEADER * pDest )
	{
		pDest->_ProtocolID		= _ProtocolID;
		pDest->_Index			= _Index;
		pDest->_GameTime		= _GameTime;
		pDest->_RoundNum		= _RoundNum;
		pDest->_Size			= _Size;
		pDest->_RespawnCount 	= _RespawnCount;
		pDest->_TouchDownCount 	= _TouchDownCount;
		//pDest->_State			= _State;
		pDest->_ui8UID		 	= _ui8UID;
		pDest->_ui8Extra		= _ui8Extra;
#ifndef _CONTROL_TOOL_
		//i3mem::Copy( _r16ObjectPos , pDest->_r16ObjectPos, sizeof(REAL16)*3 );
#endif

	}

	UINT16	CopyToPacket( char * pPacket )
	{
		CopyTo( (N_GAME_HEADER*) pPacket);
		return (UINT16) sizeof(N_GAME_HEADER);
	}
};		// 8bytes

struct GAME_ROOM_OPTION
{
	UINT8					m_ui8Mode = 0;

	// Bomb Mission 용
	REAL16					m_r16BombInstallDelayTime = 0.0f;					// C4 설치 시간
	REAL16					m_r16BombUnInstallDelayTime = 0.0f;				// C4 해체 시간
	REAL16					m_r16BombExplosionDelayTime = 0.0f;				// C4 설치 후 폭발 할때까지의 시간

	INT32 Packing( char* pBuffer )
	{
		INT32 i32Size = 0;

		memcpy( &pBuffer[ i32Size ],			&m_ui8Mode,						sizeof(UINT8) );	i32Size += sizeof(UINT8);
		switch( m_ui8Mode )
		{
		case STAGE_MODE_BOMB:
			{
				memcpy( &pBuffer[ i32Size ],	&m_r16BombInstallDelayTime,		sizeof(REAL16) );	i32Size += sizeof(REAL16);
				memcpy( &pBuffer[ i32Size ],	&m_r16BombUnInstallDelayTime,	sizeof(REAL16) );	i32Size += sizeof(REAL16);
				memcpy( &pBuffer[ i32Size ],	&m_r16BombExplosionDelayTime,	sizeof(REAL16) );	i32Size += sizeof(REAL16);
			}
			break;
		}
		return i32Size;
	}
	
	INT32 UnPacking( char* pBuffer )
	{
		INT32 i32Size = 0;

		memcpy( &m_ui8Mode,								&pBuffer[ i32Size ],			sizeof(UINT8) );	i32Size += sizeof(UINT8);
		switch( m_ui8Mode )
		{
		case STAGE_MODE_BOMB:
			{
				memcpy( &m_r16BombInstallDelayTime,		&pBuffer[ i32Size ],	sizeof(REAL16) );	i32Size += sizeof(REAL16);
				memcpy( &m_r16BombUnInstallDelayTime,	&pBuffer[ i32Size ],	sizeof(REAL16) );	i32Size += sizeof(REAL16);
				memcpy( &m_r16BombExplosionDelayTime,	&pBuffer[ i32Size ],	sizeof(REAL16) );	i32Size += sizeof(REAL16);
			}
			break;
		}
		return i32Size;
	}
};

struct USER_INFO_LAST_GAME
{
	UINT32	m_ui32StageID = 0;			// 스테이지
			
	UINT16	m_ui16Killcount = 0;		// 킬
	UINT16	m_ui16Death = 0;			// 데스
	UINT32	m_ui32Exp = 0;				// 획득 경험치
	UINT32  m_ui32Point = 0;			// 포인트
	UINT16	m_ui16Assist = 0;			// 어시스트 회수.

	//////////////////////////////////////////////////////////////////////////
	void	Reset()
	{
		m_ui32StageID = m_ui32Exp = m_ui32Point = m_ui16Killcount = m_ui16Death = m_ui16Assist= 0;
	}
};

//////////////////////////////////////////////////////////////////////
// 배틀 중 실시간 아이템 장착 패킷
struct GC_PROTOCOL_SLOT_EQUIPMENT
{
	INT32						m_i32SlotIdx = 0;										// 슬롯인덱스
	UINT16						m_ui16ChangeFlag = 0;									// 변경된 장착 정보 비트 연산으로 보내집니다. - EQUIPMENT_IN_BATTLE	참조
	USER_INFO_EQUIPMENT_CHARA	m_EquipmentChara;									// 장착 중인 케릭터
	USER_INFO_EQUIPMENT_WEAPON	m_EquipmentWeapon;									// 장착중인 무기 리스트

	UINT8						m_aui8WeaponEndurance[CHAR_EQUIPMENT_WEAPON_COUNT] = { 0 };			// 장착중인 무기의 열화 상태. 기존 것은 aui8Weaponendurance[0]
	//UINT8						m_ui8WeaponEndurance;								// 장착중인 무기의 열화 상태

	UINT8						m_aui8EquipUserTitle[MAX_EQUIP_USER_TITLE] = { 0 };		// 장착중인 타이틀
	UINT32						m_EquipItem = 0;										// 장착중인 아이템 리스트 - SLOT_ITEM_BULLET_PROOF_VEST_GM 검색
};

#pragma pack( pop )
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif