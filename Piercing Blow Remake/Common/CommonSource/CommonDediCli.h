#pragma once

using namespace std;
#include <vector>

#pragma pack (push, 1) 

#define JUMP_NAVIMESH_ANGLE_VALUE	70
#define JUMP_NAVIMESH_HEIGHT		0.3f
#define MAX_COUNT_DINOKILLITEM		32			// 드랍아이템 최대 갯수
#define DINOKILLITEM_LIVETIME		20.0f		// 아이템이 드랍된후 아무유저가 획득하지 않았을 경우 없어지기까지의 시간(초)
#define DISTANCE_ITEM_CATCH_CHECK	15.0f		// 유저가 아이템을 획득했다고 알려왔을때의 아이템과 유저의 유효거리(m). 이 거리이상으로 나온다면 아이템 획득 요청을 거절합니다.
#define	USER_SP_CHARGE_TIME			5.0f		// 몇초마다 유저의 SP와 Score 싱크 정보를 보내줄 것인가?
#define MAX_WAVE_COUNT				32			// 한 라운드가 가질수 있는 최대 웨이브의 갯수
#define COMBO_CHAIN_TIME			5.0f		// 콤보가 연결되기 위한 시간(초)
#define MAX_LEVEL_SENTRYGUN			4			// 센트리건 최고레벨
#define MAX_LEVEL_DUMMY				4			// 더미 최고레벨
#define MAX_LEVEL_USERSKILL			5			// 유저스킬 최고레벨
#define MAX_LEVEL_DOMI_RANGE		6			// 10라운드씩 끊어지는 단위 갯수( 1-10, 11-20, 21-30, 31-40, 41-50, 51-32000 )
#define COUNTDOWN_WAIT_SECOND		5.0f		// 카운트다운 대기시간(초)
#define MAX_COUNT_RANK_SELECT_RULE	8			// 랭크선정규칙의 최대 갯수
#define MAX_COUNT_DINO_GRADE		6			// 공룡 최대 랭크 수
#define MAX_COUNT_SCENARIO_GRADE	6			// 시나리오 최대 랭크 수
#define DOMI_USER_FIRST_RESPAWN_TIME	10.0f	// 첫 사망시 리스폰 시간
#define DOMI_USER_ADD_RESPAWN_TIME	5.0f		// 사망할때마다 증가되는 리스폰 시간
#define DOMI_USER_MAX_RESPAWN_TIME	60.0f		// 리스폰 시간 증가가 되지 않는 최대치 리스폰 시간
#define DOMI_USER_DIE_ANIMATION_TIME	3.5f	// 유저 사망 애니메이션 재생 시간
#define ACID_ITEM_LIVE_TIME_SECOND	30			// 애시드 아이템 등장해서 돌아다니는 시간.
#define SENTRYGUN_INSTALL_ANIMATION_TIME 3.0f	// 센트리건 설치,업그레이드시 준비 애니메이션 재생 시간(초)
#define	DOMI_UPGRADE_COOL_TIME		2.0f		// 센트리건이나 더미 업그레이드후, 재업그레이드 가능하기까지의 시간(초)
#define DOMI_USE_COOL_TIME			0.9f		// HP서플라이나 Ammo서플라이 사용후, 재사용 가능하기까지의 시간(초)

struct ST_POSROT
{
	VEC3D	_v3Pos;
	INT16	_i16Angle = 0;
};

struct ST_NM_NODE_SCO
{
	VEC3D	m_v3Pos[3];
};

struct ST_NM_NODE_SAM
{
	VEC3D	m_v3Pos[3];
	INT16	m_i16NearIdx[3] = { 0, 0, 0 };
	bool	m_bJump = false;
};

struct DS_LINE;

struct ST_PATH_INPUT;

class CDomiStageCli
{
public:
	CDomiStageCli() {}
	INT32			LoadSAM( char* szFileName );
	void			Destroy();
	INT32			GetNeviMeshCount(void) const	{ return m_i32NaviMeshCount;	}
	ST_NM_NODE_SAM*	GetNeviMeshNode(INT32 Index)	{ if(Index >= m_i32NaviMeshCount ) return nullptr; return &m_NaviMeshTri[Index]; }

protected:
	ST_NM_NODE_SAM*	m_NaviMeshTri = nullptr;
	INT32			m_i32NaviMeshCount = 0;
};

#define MAX_STAGE_MODE_DOMINATION_STAGE_COUNT	12

class CDomiStageSetCli
{
public:
	INT8			m_i8SentryGunMax = 0;
	INT8			m_i8DummyMax = 0;
	INT8			m_i8HPMax = 0;
	INT8			m_i8AmmoMax = 0;
	INT8			m_i8SkillShopMax = 0;
	ST_POSROT		m_Dummy[MAX_COUNT_DOMI_DUMMY];
	ST_POSROT		m_SentryGun[MAX_COUNT_DOMI_SENTRYGUN];
	ST_POSROT		m_HPSupply[MAX_COUNT_DOMI_HPSUPPLY];
	ST_POSROT		m_SkillShop[MAX_COUNT_DOMI_SKILLSHOP];
	ST_POSROT		m_AmmoSupply[MAX_COUNT_DOMI_AMMOSUPPLY];

	void			Init();
	INT32			LoadINI( char* szName );
};

// 유저스킬 업그레이드 실패사유
#define FAIL_REASON_USER_SKILL_UPGRADE_SP_NOT_ENOUGH	-1	// 보유 SP가 부족.
#define FAIL_REASON_USER_SKILL_UPGRADE_MAX_LEVEL		-2	// 이미 최고 레벨임.
#define FAIL_REASON_USER_SKILL_UPGRADE_ROUND_NOT_ENOUGH	-3	// 최소 요구 라운드보다도 낮은 라운드임.
#define FAIL_REASON_USER_SKILL_UPGRADE_NOT_SKILLSHOP	-5	// 스킬샵 근처에서만 가능한데, 스킬샵에서 떨어져 있음.

// 공룡 드랍 아이템
enum DINOKILLITEM
{
	DINOKILLITEM_HP,
	DINOKILLITEM_AMMO,
	DINOKILLITEM_GRENADE,

	DINOKILLITEM_COUNT
};

struct ST_SP_ROUND
{
	INT16	_i16SP = 0;						// 업그레이드시 소모되는 SP
	INT16	_i16Round = 0;					// 업그레이드 가능한 최소 요구 라운드
};

enum USER_SKILL
{
	USER_SKILL_HP_UP,					// 최대체력 증가
	USER_SKILL_DAMAGE_UP,				// 주는 데미지 증가
	USER_SKILL_SPEED_UP,				// 이동속도 증가
	USER_SKILL_MAGAZINE_UP,				// 장탄수 증가
	
	// 3차개발 유저스킬 8종+1종(미트봄) 추가 2013.04.03
	USER_SKILL_WEAPON_EFFECT_RATIO_UP,	// 무기 특수효과 확률 증가
	USER_SKILL_RELOAD_SPEED_FAST,		// 재장전 속도 감소
	USER_SKILL_GRENADE_DAMAGE_UP,		// 수류탄 데미지 증가
	USER_SKILL_AGGRO_DOWN,				// 어그로 감소
	USER_SKILL_GET_SP_UP,				// SP 획득량 증가
	USER_SKILL_MAX_SP_UP,				// SP 최고치 증가
	USER_SKILL_SPEND_SP_DOWN,			// SP 소모비용 감소
	USER_SKILL_GET_DAMAGE_DOWN,			// 받는 데미지 감소
	USER_SKILL_MEATBOMB,				// 미트봄 사용

	USER_SKILL_COUNT
};

extern char* g_pDinoName[DINO_KIND_COUNT];
extern INT8 g_i8MaxSkillLevel[USER_SKILL_COUNT];

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PACKET_DOMINATION_XXXXXXX_REQ 구조체는 클라이언트가 정보를 담아서 보내면 데디케이션 서버가 받아서 처리하기 위한 구조체이고,
// PACKET_DOMINATION_XXXXXXX_ACK 구조체는 데디케이션 서버가 정보를 담아서 보내면 클라이언트가 받아서 처리하기 위한 구조체입니다.
//

struct ST_DOMI_GAME_VALUE
{
	// 유저스킬
	struct
	{
		ST_SP_ROUND	_ReqSPRound;			// 업그레이드하는데 소모되는 SP, 요구 최소 Round

		union
		{
			REAL32	_r32IncreaseRatio;		// 업그레이드 스킬 항목 증가량
			INT32	_i32Value;	
		};
	} _UserSkill[USER_SKILL_COUNT][MAX_LEVEL_USERSKILL];

	INT8 _i8UserSkillMaxLevel[USER_SKILL_COUNT] = { 0 };
	
	// 센트리건
	struct
	{
		UINT16		_ui16SpendSP_Install = 0;	// 설치, 업그레이드하는데 소모되는 SP
		UINT16		_ui16SpendSP_Fix = 0;		// 수리하는데 소모되는 SP
		INT16		_i16MaxHP = 0;				// 최대 HP
	} _Sentry[MAX_LEVEL_SENTRYGUN];
	
	// 더미
	struct
	{
		UINT16		_ui16SpendSP_Install = 0;	// 설치, 업그레이드하는데 소모되는 SP
		UINT16		_ui16SpendSP_Fix = 0;		// 수리하는데 소모되는 SP
		INT16		_i16DamageExplosion = 0;	// 폭발 데미지
		INT16		_i16MaxHP = 0;				// HP
	} _Dummy[MAX_LEVEL_DUMMY];

	// HP서플라이
	struct
	{
		UINT16		_ui16SpendSP_Use = 0;		// 이용시 소모되는 SP
	} _HPSupply;

	// Ammo서플라이
	struct
	{
		UINT16		_ui16SpendSP_Use = 0;		// 이용시 소모되는 SP
	} _AmmoSupply;

	struct
	{
		UINT16		_ui16MaxSP = 0;				// 최대SP양
		UINT16		_ui16BasicSP = 0;			// 기본지급 SP양
	} _SPInfo[MAX_LEVEL_DOMI_RANGE];
	
	void Copy( ST_DOMI_GAME_VALUE* pDest )
	{
		i3mem::Copy( pDest, this, sizeof( ST_DOMI_GAME_VALUE ) );
	}
};

struct ST_CONDITION_SERVER
{
	// 더미
	struct
	{
		INT16		_i16DamagePrickle = 0;		// 가시 데미지
	} _Dummy[MAX_LEVEL_DUMMY];

	// 센트리건
	struct
	{
		INT16		_i16Damage = 0;				// 데미지
		REAL32		_r32RotateSpeed = 0.0f;		// 회전속도(1초에 회전할수 있는 각도)
		REAL32		_r32FireSpeed = 0.0f;			// 발사간격(초)
		REAL32		_r32Eye = 0.0f;				// 공격범위
	} _Sentry[MAX_LEVEL_SENTRYGUN];
};

// 공룡
struct ST_DINO_UDP
{
	REAL16	_r16x = 0.0f;
	REAL16	_r16y = 0.0f;
	REAL16	_r16z = 0.0f;
	INT8	_i8DinoKind = 0;
	INT8	_i8State = 0;
	INT8	_i8SubState = 0;
	INT16	_i16HP = 0;
	INT8	_i8AddInfoMask = 0;
};

// 드랍아이템
struct ST_DROPITEM_UDP
{
	REAL16	_r16x = 0.0f;
	REAL16	_r16y = 0.0f;
	REAL16	_r16z = 0.0f;
	INT8	_i8Kind = 0;
};

#define MAX_COUNT_SDSS		(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY + MAX_COUNT_DOMI_HPSUPPLY + MAX_COUNT_DOMI_AMMOSUPPLY + MAX_COUNT_DOMI_SKILLSHOP)

enum UDP_EVENT
{
	UDP_EVENT_REPAIR	= 0,
	UDP_EVENT_USE,
	UDP_EVENT_FIRE,
	UDP_EVENT_DROP,
	UDP_EVENT_CATCH,
	UDP_EVENT_SCORE,
	UDP_EVENT_DEATH_DINO,
	UDP_EVENT_MUTANTTREX_DASH,
	UDP_EVENT_MUTANTTREX_HOWL,
	UDP_EVENT_SPECIAL_EFFECT,
	UDP_EVENT_DINO_INFO,

	UDP_EVENT_COUNT
};

struct ST_EVENT_MUTANTTREX_HOWL
{
	INT8	_i8DinoIdx = 0;
	INT16	_i16CancelHP = 0;
};

struct ST_EVENT_SCORE
{
	INT8	_UserIdx = 0;
	UINT16	_AccSp = 0;
	INT32	_AccScore = 0;
	INT16	_AccKillDino = 0;
};

#define		DEATHDINO_NONE		0
#define		DEATHDINO_ASSIST	1
#define		DEATHDINO_KILL		2

#define		MASK_TREX_DASH_USER		0x00	// 뮤턴트티렉스가 유저를 타겟으로 잡음
#define		MASK_TREX_DASH_SENTRY	0x10	// 뮤턴트티렉스가 센트리건을 타겟으로 잡음
#define		MASK_TREX_DASH_DUMMY	0x20	// 뮤턴트티렉스가 더미를 타겟으로 잡음

struct ST_EVENT_DEATH_DINO
{
	UINT8	_ui8UserMask = 0;	// 2비트씩 각각 1개의 유저. (비트) 00-기여없음 01-기여했음 10-죽였음 11-미사용
	INT8	_Combo = 0;
	bool	_bHeadShot = false;		// 헤드샷으로 죽었는지.

	INT16	_Sp0 = 0;
	INT16	_Score0 = 0;
	INT16	_Sp1 = 0;
	INT16	_Score1 = 0;
	INT16	_Sp2 = 0;
	INT16	_Score2 = 0;
	INT16	_Sp3 = 0;
	INT16	_Score3 = 0;
};

#pragma pack (pop)

class CDomiGlobalCli
{
public:
	INT32				LoadCondition( char* szPath );

	ST_DOMI_GAME_VALUE	m_ConditionSvrCli;
};