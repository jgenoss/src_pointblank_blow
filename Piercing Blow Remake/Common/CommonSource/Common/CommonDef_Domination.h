#ifndef __COMMONDEF_DOMINATION_H
#define	__COMMONDEF_DOMINATION_H

enum DINO_KIND
{
	DINO_KIND_NONE				= -1,
	DINO_KIND_RAPTOR_RED		= 0,	// 붉은색 랩터
	DINO_KIND_RAPTOR_GREEN,				// 초록색 랩터
	DINO_KIND_RAPTOR_BLUE,				// 파란색 랩터
	DINO_KIND_STING,					// 스팅
	DINO_KIND_ACID,						// 애시드
	DINO_KIND_ACID_BOMB,				// 폭발 애시드
	DINO_KIND_MUTANTTREX,				// 뮤턴트티렉스
	DINO_KIND_TANK,						// 탱크
	DINO_KIND_ACID_ITEM,				// 아이템 떨구는 애시드
	DINO_KIND_RAPTOR_REINFORCE,			// 강화 랩터
	DINO_KIND_STING_REINFORCE,			// 강화 스팅
	DINO_KIND_ACID_REINFORCE,			// 강화 애시드

	DINO_KIND_COUNT
};

enum DELAY_TYPE
{
	DELAY_TYPE_UNKNOWN		= 0,
	DELAY_TYPE_TIME,						// TIME: 이전그룹이 활성화 된 후 DelayTime 경과 후 이 그룹이 활성화 됨
	DELAY_TYPE_ANNIHILATE,					// ANNIHILATE: 이전 그룹 전멸되면 이 그룹 활성화(DelayTime을 사용하지 않음)
	DELAY_TYPE_HP							// HP: 지정된 서브그룹의 1번째 공룡의 체력이 지정된 HP의 %이하로 내려가면 이 그룹 활성화
};

enum DS_EVENT_ID
{
	EVENT_UNKNOWN		= 0,
	// network event
	EVENT_CREATE		= 1,
	EVENT_DESTROY		= 2,
	EVENT_RESPAWN		= 3,
	EVENT_ROUND			= 4,
	EVENT_DINOINFO		= 5,
	EVENT_TOUCHDOWN		= 6,
	EVENT_EQIPMENT		= 7,
	EVENT_USERENTER		= 8,
	EVENT_USERLEAVE 	= 9,
	// game event
	EVENT_HIT			= 20,	
	EVENT_SUICIDE		= 21,	// 추가
	EVENT_INSTALL		= 22,	// reserved
	EVENT_UNINSTALL		= 23,	// reserved
	EVENT_DEAD			= 24,
	EVENT_USE			= 25,	// reserved
	EVENT_DINORESPAWN	= 26,	// reserved
	EVENT_BLIND			= 27,	// reserved
	EVENT_USERRESPAWN	= 28,	// reserved
	EVENT_EXPLOSION		= 29,	
	EVENT_ID_MAX,
};

enum WAVE_STATE {
	WAVE_STATE_START,
	WAVE_STATE_INTERMISSION,	// 인터미션(25초), 사용자가 모두 e키를 누르면 바로 인터미션 패스. 1웨이브일 경우엔 인터미션 없음
	WAVE_STATE_COUNTDOWN,		// 카운트다운(5초)
	WAVE_STATE_UPDATE,			// 게임
	WAVE_STATE_RESULT,			// 중간결과창(마지막웨이브아닐경우), 최종결과창(마지막웨이브일 경우)
	WAVE_STATE_END
};

enum DOMI_TYPE_DINO_TARGET
{
	DOMI_TYPE_DINO_TARGET_USER			= 0,
	DOMI_TYPE_DINO_TARGET_SENTRYGUN		= 1,
	DOMI_TYPE_DINO_TARGET_DUMMY			= 2,

	DOMI_TYPE_DINO_TARGET_COUNT
};

// 공룡 상태 
// 순서 중요함. (클라이언트도 쓰기 때문에. 순서 변경시 클라이언트도 재빌드해야 정상플레이 가능)
enum DINO_STATE
{
	DINO_STATE_NONE			= 0,
	DINO_STATE_DEAD,
	DINO_STATE_FREEZE,			// 부등호를 사용하고 있음. 움직이지 않는 상태를 체크 하기 위한 용도로 사용함. 
	DINO_STATE_STOP,			// 정지
	DINO_STATE_ATTACK,			// 공격
	DINO_STATE_MOVE,			// 이동
	DINO_STATE_PAIN,			// 고통
	DINO_STATE_S_STIFFEN,		// 결직 ( 무기특수효과 )
	DINO_STATE_S_BLIND,			// 블라인드 ( 무기특수효과 )
	DINO_STATE_NOTUSE,
	DINO_STATE_RAPTORLEAP,		// 랩터: 도약스킬
	DINO_STATE_S_SLOW,			// 둔화 ( 무기특수효과 )
	DINO_STATE_S_KNOCKDOWN,		// 넉다운 ( 무기특수효과 )
	DINO_STATE_TANKDASH,		// 탱크: 돌진스킬
	DINO_STATE_ROAR,			// 유저를 발견했을때 괴성 지르기
	DINO_STATE_SUCCESSKILL,		// 유저를 죽였을때 기뻐하기
	DINO_STATE_RAPTOR_FEED,		// 랩터: 시체먹기
	DINO_STATE_ACID_SEEK,		// 애시드: 탐색
	DINO_STATE_STING_SHAKE,		// 스팅: 흔들기
	DINO_STATE_TANK_HOWL,		// 탱크: 외치기
	DINO_STATE_TREX_HOWL,		// 뮤턴트티렉스: 하울링스킬
	DINO_STATE_TREX_READY,		// 뮤턴트티렉스: 대쉬스킬 준비
	DINO_STATE_TREX_DASH,		// 뮤턴트티렉스: 대쉬스킬 진행
	DINO_STATE_TREX_HOWL_FAIL,	// 뮤턴트티렉스: 하울링스킬 실패

	DINO_STATE_COUNT
};

enum DINO_MOVE_STATE
{
	DINO_MOVE_STATE_JUMP_START	= 0,
	DINO_MOVE_STATE_JUMP_UP,
	DINO_MOVE_STATE_JUMP_DOWN,
	DINO_MOVE_STATE_JUMP_END,
	DINO_MOVE_STATE_RUN,
	DINO_MOVE_STATE_WALK,
	DINO_MOVE_STATE_SOONBOOM,

	DINO_MOVE_STATE_COUNT
};

enum DOMI_TYPE
{
	DOMI_TYPE_NO			= -1,
	DOMI_TYPE_USER			= 0,
	DOMI_TYPE_DINO			= 1,
	DOMI_TYPE_SENTRYGUN		= 2,
	DOMI_TYPE_DUMMY			= 3,
	DOMI_TYPE_HPSUPPLY		= 4,
	DOMI_TYPE_AMMOSUPPLY	= 5,
	DOMI_TYPE_SKILLSHOP		= 6,
	DOMI_TYPE_MEATBOMB		= 7,

	DOMI_TYPE_COUNT
};

enum DINO_ATTACK_WEAPON_MUTANTTREX
{
	DINO_ATTACK_WEAPON_MUTANTTREX_BITE_LOW,		// 뮤턴트티렉스 하단물기
	DINO_ATTACK_WEAPON_MUTANTTREX_BITE_MID,		// 뮤턴트티렉스 중단물기
	DINO_ATTACK_WEAPON_MUTANTTREX_CHOP,			// 뮤턴트티렉스 밟기

	DINO_ATTACK_WEAPON_MUTANTTREX_COUNT
};

enum DINO_TANKDASH_STATE
{
	DINO_TANKDASH_STATE_READY = 0,		// 돌진 준비
	DINO_TANKDASH_STATE_RUNNING,		// 돌진중
	DINO_TANKDASH_STATE_FINISH,			// 돌진 완료

	DINO_TANKDASH_STATE_COUNT
};

enum DINO_RAPTORLEAP_STATE
{
	DINO_RAPTORLEAP_STATE_READY	= 0,	// 도약 준비
	DINO_RAPTORLEAP_STATE_LEAP_UP,		// 도약 상승
	DINO_RAPTORLEAP_STATE_LEAP_DOWN,	// 도약 하강
	DINO_RAPTORLEAP_STATE_FINISH,		// 도약 완료
	DINO_RAPTORLEAP_STATE_FAIL,			// 도약 공격 실패

	DINO_RAPTORLEAP_COUNT
};

enum DINO_DEAD_STATE
{
	DINO_DEAD_STATE_NORMAL		= 0,
	DINO_DEAD_STATE_HEADSHOT,
	DINO_DEAD_STATE_GRENADE,

	DINO_DEAD_STATE_COUNT
};

enum DINO_ATTACK_STATE
{
	DINO_ATTACK_STATE_ING	= 0,
	DINO_ATTACK_STATE_WAIT_COOLTIME,

	DINO_ATTACK_STATE_COUNT
};

enum DINO_ATTACK_WEAPON_RAPTOR
{
	DINO_ATTACK_WEAPON_RAPTOR_BITE,				// 랩터 물기
	DINO_ATTACK_WEAPON_RAPTOR_CLAW,				// 랩터 할퀴기

	DINO_ATTACK_WEAPON_RAPTOR_COUNT
};

enum DINO_ATTACK_WEAPON_STING
{
	DINO_ATTACK_WEAPON_STING_SPIT,				// 스팅 침뱉기
	DINO_ATTACK_WEAPON_STING_GASBAG,			// 스팅 가스주머니발사

	DINO_ATTACK_WEAPON_STING_COUNT
};

enum DINO_ATTACK_WEAPON_ACID
{
	DINO_ATTACK_WEAPON_ACID_BITE,				// 애시드 물기

	DINO_ATTACK_WEAPON_ACID_COUNT
};

enum DINO_ATTACK_WEAPON_TANK
{
	DINO_ATTACK_WEAPON_TANK_BITE,				// 탱크 물기

	DINO_ATTACK_WEAPON_TANK_COUNT
};

enum DINO_PAIN_STATE
{
	DINO_PAIN_STATE_LOW,
	DINO_PAIN_STATE_MIDDLE,
	DINO_PAIN_STATE_HIGH,

	DINO_PAIN_STATE_COUNT
};

// 1. 유저, 2. 센트리건, 3. 더미
enum OBJ_STATE
{
	OBJ_STATE_NONE	= 0, 
	OBJ_STATE_IDLE,	
	OBJ_STATE_DEAD,	
};

enum eDINO_GRENADE
{
	DINO_GRENADE_NONE = 0,				// 슬롯 미사용중
	DINO_GRENADE_STING_GASBAG,			// 스팅 가스주머니
	DINO_GRENADE_RESERVED1,				// 차후의 공룡의 수류탄1
	DINO_GRENADE_RESERVED2,				// 차후의 공룡의 수류탄2

	DINO_GRENADE_COUNT
};

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

// 공룡 드랍 아이템
enum DINOKILLITEM
{
	DINOKILLITEM_HP,
	DINOKILLITEM_AMMO,
	DINOKILLITEM_GRENADE,

	DINOKILLITEM_COUNT
};

enum USER_SKILL
{
	USER_SKILL_HP_UP,					// 최대체력 증가
	USER_SKILL_DAMAGE_UP,				// 주는 데미지 증가
	USER_SKILL_SPEED_UP,				// 이동속도 증가
	USER_SKILL_MAGAZINE_UP,				// 장탄수 증가

	// 3차개발 유저스킬 8종+1종(미트봄) 추가 2013.04.03
	USER_SKILL_RELOAD_SPEED_FAST,		// 재장전 속도 감소
	USER_SKILL_MEATBOMB,				// 미트봄 사용

	// 4차개발 유저스킬 2종(센트리건, 더미) 추가 2014.01.15
	USER_SKILL_INSTALL_SENTRYGUN,		// 센트리건 설치
	USER_SKILL_INSTALL_DUMMY,			// 더미 설치

	USER_SKILL_COUNT
};

// 제압 보상 지급 조건. 
enum DOMI_REWARD_CONDITION
{
	DOMI_REWARD_CONDITION_ROUND    = 0,	// Round 클리어.
	DOMI_REWARD_CONDITION_BOSS,			// Boss 공룡 Kill
	DOMI_REWARD_CONDITION_ACID_ITEM,	// Acid Item 공룡 Kill


	DOMI_REWARD_CONDITION_MAX,	// **주의** : 4이상이면 반드시 PROTOCOL_ASC_DOMI_REWARD_INFO_ACK 패킷 사이즈 체크해야합니다.
};

// 제압 보상 지급 라운드.
enum DOMI_REWARD_ROUND 
{
	DOMI_REWARD_ROUND_1_TO_10  = 0,	//  1~10 라운드
	DOMI_REWARD_ROUND_11_TO_20,		// 11~20 라운드
	DOMI_REWARD_ROUND_21_TO_30,		// 21~30 라운드
	DOMI_REWARD_ROUND_31_TO_40,		// 31~40 라운드
	DOMI_REWARD_ROUND_41_TO_50,		// 41~50 라운드
	DOMI_REWARD_ROUND_51_TO,		// 51 라운드 이상.


	DOMI_REWARD_ROUND_MAX,		 // **주의** : 7이상이면 반드시 PROTOCOL_ASC_DOMI_REWARD_INFO_ACK 패킷 사이즈 체크해야합니다.
};


// 산정된 점수에 따른 보상 등급
enum RANK_GRADE
{
	S_GRADE,	// S등급 (제일 높은 확률 값의 90%를 분배)
	A_GRADE,	// A등급 (제일 높은 확률 값의 70%를 분배)
	B_GRADE,	// B등급 (제일 높은 확률 값의 50%를 분배)
	C_GRADE,	// C등급 (제일 높은 확률 값의 30%를 분배)
	D_GRADE,	// D등급 (제일 높은 확률 값의 10%를 분배)
	E_GRADE,	// E등급 (분배하지 않음)

	GRADE_END
};

enum UNLOCK_ROUND_COUNT
{
	UNLOCK_11_ROUND,	// 11라운드 시작제한 해제권
	UNLOCK_21_ROUND,	// 21라운드 시작제한 해제권
	UNLOCK_31_ROUND,	// 31라운드 시작제한 해제권
	UNLOCK_41_ROUND,	// 41라운드 시작제한 해제권
	UNLOCK_51_ROUND,	// 51라운드 시작제한 해제권

	ROUND_UNLOCK_END,	// 라운드권이 추가되면 위로 순서대로 추가해야 합니다.

	// 이 아래 있는 애들은 건드리지 마세요!! 위로 올리면 배열 꼬입니다.
	UNLOCK_01_ROUND = 127,
	UNLOCK_00_ROUND = -1
};

#define MAKE_ITEM_FLAG(event,type,class,number)			(((event)*10000000)+((type)*100000)+((class)*1000)+(number))
#define Meat_Bomb							407006

#define DINO_MOVE_CHECK_TIME				1.5f	// 패스를 다시 찾는 시간.
#define DINO_DEAD_DELAY_TIME				4.0f

#define AGGRO_VALUE_DEFENCE_OBJECT			80
#define AGGRO_VALUE_SENTRYGUN				40
#define AGGRO_UPDATE_TIME					1.0f
#define AGGRO_TARGET_COUNT					(MAX_COUNT_DOMI_USER + MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY + MAX_COUNT_DOMI_USER)

// 고통 애니메이션 재생 시간
#define DINO_TIME_RAPTOR_PAIN_LOW			0.5f
#define DINO_TIME_RAPTOR_PAIN_MIDDLE		1.0f
#define DINO_TIME_RAPTOR_PAIN_HIGH			1.1f

#define DINO_TIME_STING_PAIN_LOW			0.5f
#define DINO_TIME_STING_PAIN_MIDDLE			1.0f
#define DINO_TIME_STING_PAIN_HIGH			1.1f

#define DINO_TIME_ACID_PAIN_LOW				0.5f
#define DINO_TIME_ACID_PAIN_MIDDLE			0.5f
#define DINO_TIME_ACID_PAIN_HIGH			0.5f

#define DINO_TIME_MUTANTTREX_PAIN_LOW		1.0f	
#define DINO_TIME_MUTANTTREX_PAIN_MIDDLE	1.0f
#define DINO_TIME_MUTANTTREX_PAIN_HIGH		1.0f
//#define DINO_TIME_MUTANTTREX_PAIN_DEATHBLOW	5.3f
//#define DINO_TIME_MUTANTTREX_PAIN_HOWLING		2.0f

#define DINO_TIME_TANK_PAIN_LOW				0.5f
#define DINO_TIME_TANK_PAIN_MIDDLE			1.0f
#define DINO_TIME_TANK_PAIN_HIGH			1.2f

// 유저발견시 소리지름
#define DINO_TIME_RAPTOR_ROAR				1.5f
#define DINO_TIME_STING_ROAR				1.5f
#define DINO_TIME_ACID_ROAR					2.5f
#define DINO_TIME_MUTANTTREX_ROAR			2.6f
#define DINO_TIME_TANK_ROAR					3.3f

// 유저킬시 기뻐함
#define DINO_TIME_RAPTOR_SUCCESSKILL		3.0f
#define DINO_TIME_STING_SUCCESSKILL			4.2f
#define DINO_TIME_ACID_SUCCESSKILL			2.5f
#define DINO_TIME_MUTANTTREX_SUCCESSKILL	2.6f
#define DINO_TIME_TANK_SUCCESSKILL			3.3f

// 공룡 개개인 특수 행동
#define DINO_TIME_RAPTOR_FEED				6.6f
#define DINO_TIME_STING_SHAKE				7.6f
#define DINO_TIME_ACID_SEEK					8.6f
#define DINO_TIME_MUTANTTREX_HOWL			2.6f
#define DINO_TIME_TANK_HOWL					2.5f

// 공격 시간
#define DINO_TIME_RAPTOR_CLAW_HIT1			0.33f	// 할퀴기 1번째 데미지 들어가는 시간
#define DINO_TIME_RAPTOR_CLAW_HIT2			0.6f	// 할퀴기 2번째 데미지 들어가는 시간
#define DINO_TIME_RAPTOR_CLAW_END			1.1f	// 할퀴기 총 공격시간
#define DINO_TIME_RAPTOR_BITE_HIT			0.5f	// 물기 데미지 들어가는 시간
#define DINO_TIME_RAPTOR_BITE_END			1.0f	// 물기 총 공격시간
#define DINO_TIME_RAPTOR_LEAP_READY			2.5f	// 도약 준비시간
#define DINO_TIME_RAPTOR_LEAP_FINISH		1.5f	// 도약 성공시간
#define DINO_TIME_RAPTOR_LEAP_FAIL			0.5f	// 도약 실패시간

#define DINO_TIME_STING_SPIT_HIT			0.3f	// 침뱉기 데미지 들어가는 시간
#define DINO_TIME_STING_SPIT_END			1.0f	// 침뱉기 총 공격시간
#define DINO_TIME_STING_GASBAG_THROW		2.1f	// 가스주머니발사 수류탄 던지는 시간
#define DINO_TIME_STING_GASBAG_END			3.0f	// 가스주머니발사 총 공격시간

#define DINO_TIME_ACID_BITE_HIT				0.2f	// 물기 데미지 들어가는 시간
#define DINO_TIME_ACID_BITE_END				0.5f	// 물기 총 공격시간

#define DINO_TIME_MUTANTTREX_BITE_HIT		0.7f	// 물기 데미지 들어가는 시간
#define DINO_TIME_MUTANTTREX_BITE_END		1.5f	// 물기 총 공격시간
#define DINO_TIME_MUTANTTREX_CHOP_HIT		0.73f	// 밟기 데미지 들어가는 시간
#define DINO_TIME_MUTANTTREX_CHOP_END		1.8f	// 밟기 총 공격시간
#define DINO_TIME_MUTANTTREX_DASH_READY		1.4f	// 돌진 대기시간
#define DINO_TIME_MUTATNTREX_HOWL_FAIL		5.3f	// 하울링 실패 모션시간

#define DINO_TIME_TANK_BITE_HIT				0.5f	// 물기 데미지 들어가는 시간
#define DINO_TIME_TANK_BITE_END				1.16f	// 물기 총 공격시간
#define DINO_TIME_TANK_DASH_READY			2.5f	// 돌진 준비시간
#define DINO_TIME_TANK_DASH_FINISH			1.0f	// 돌진 성공시간

#define MAX_TCP_EVENT_COUNT					128

#define INTERMISSION_WAIT_SECOND			25.0f	// 인터미션 대기시간

#define DINO_KNOCKDOWN_TIME					4.0f
#define DINO_DEAD_DELAY_TIME				4.0f
#define SMOKE_GAS_TIME						11.0f	// 스모크 연기로 인한 특수효과 적용시간

#define DOMI_RESULT_WINDOW_SECOND			5.0f	// 중간결과창 or 최종결과창이 떠있는 시간
#define DOMI_FINAL_WAVE_REWARD_TIME_SECOND	3.0f	// 라운드 보상 아이템 지급 알림창이 떠 있는 시간 ( = 최종결과창이 닫힌후 스테이지 점프하기까지의 사이시간 )

#define	DEATHDINO_NONE						0
#define DEATHDINO_ASSIST					1
#define	DEATHDINO_KILL						2

#define	MASK_TREX_DASH_USER					0x00	// 뮤턴트티렉스가 유저를 타겟으로 잡음
#define	MASK_TREX_DASH_SENTRY				0x10	// 뮤턴트티렉스가 센트리건을 타겟으로 잡음
#define	MASK_TREX_DASH_DUMMY				0x20	// 뮤턴트티렉스가 더미를 타겟으로 잡음

// 유저스킬 업그레이드 실패사유
#define FAIL_REASON_USER_SKILL_UPGRADE_SP_NOT_ENOUGH	-1	// 보유 SP가 부족.
#define FAIL_REASON_USER_SKILL_UPGRADE_MAX_LEVEL		-2	// 이미 최고 레벨임.
#define FAIL_REASON_USER_SKILL_UPGRADE_NOT_SKILLSHOP	-3	// 스킬샵 근처에서만 가능한데, 스킬샵에서 떨어져 있음.

#define FLASHBANG_COUNT						32
#define SMOKE_COUNT							32

#define MAX_DOMI_GROUP_COUNT				20
#define MAX_DOMI_SUBGROUP_COUNT				5

#define MAX_COUNT_RESPAWNLIST_DATA			64

#define MAX_COUNT_DOMI_USER					7		// 7을 선언하지만, 0,2,4,6만 사용하고 1,3,5는 사용하지 않는다.
#define MAX_COUNT_DOMI_USER_REAL			4
#define MAX_COUNT_DOMI_DINO					32
#define MAX_COUNT_DOMI_SENTRYGUN			10
#define MAX_COUNT_DOMI_DUMMY				10
#define MAX_COUNT_DOMI_HPSUPPLY				5
#define MAX_COUNT_DOMI_AMMOSUPPLY			5
#define MAX_COUNT_DOMI_SKILLSHOP			1

#define DOMI_USER_INCREASE					2

#define BEGIN_IDX_DOMI_SENTRYGUN			0
#define BEGIN_IDX_DOMI_DUMMY				(MAX_COUNT_DOMI_SENTRYGUN)
#define BEGIN_IDX_DOMI_HPSUPPLY				(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY)
#define BEGIN_IDX_DOMI_AMMOSUPPLY			(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY + MAX_COUNT_DOMI_HPSUPPLY)
#define BEGIN_IDX_DOMI_SKILLSHOP			(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY + MAX_COUNT_DOMI_HPSUPPLY + MAX_COUNT_DOMI_AMMOSUPPLY)

#define MAX_COUNT_CONDI						64
#define MAX_COUNT_APPEAR_DINO_IN_WAVE		512

#define AI_SHOOT_HEIGHT_FROM_GROUND			0.7f
#define AI_HEAD_HEIGHT						1.7f

#define BRAIN_STATUS_WAIT_FOR_APPEAR		0
#define BRAIN_STATUS_APPEARED				1
#define BRAIN_STATUS_MURDERED				2

// 이 값이 수정되면 pszCondText도 같이 수정할 것
#define DOMI_CONDITION_APPEARED				1
#define DOMI_CONDITION_MURDERED				2
#define DOMI_CONDITION_WEAKENED				3

#define NECESSARY_CONDITION_NOT_ACHIEVE		0		// 조건 미달성
#define NECESSARY_CONDITION_ACHIEVE			1		// 조건 달성

#define	SPECIAL_SKILL_UPDATE_TIME			1.8f

#define DOMI_ROUND_CIRCLE_COUNT				5

#define MAX_COUNT_DOMI_USER_SENTRYGUN_DUMMY	10

#define USER_OBJECT_DISTANCE				3.0f

#define GET_ITEM_FLAG_EVENT(Itemflag)		(((Itemflag) % 100000000) / 10000000)
#define GET_ITEM_FLAG_TYPE(Itemflag)		(((Itemflag) % 10000000) / 100000)
#define GET_ITEM_FLAG_CLASS(Itemflag)		(((Itemflag) % 100000) / 1000)
#define GET_ITEM_FLAG_NUMBER(Itemflag)		((Itemflag) % 1000)

#define JUMP_NAVIMESH_ANGLE_VALUE			70
#define JUMP_NAVIMESH_HEIGHT				0.3f
#define MAX_COUNT_DINOKILLITEM				32		// 드랍아이템 최대 갯수
#define DINOKILLITEM_LIVETIME				20.0f	// 아이템이 드랍된후 아무유저가 획득하지 않았을 경우 없어지기까지의 시간(초)
#define DISTANCE_ITEM_CATCH_CHECK			15.0f	// 유저가 아이템을 획득했다고 알려왔을때의 아이템과 유저의 유효거리(m). 이 거리이상으로 나온다면 아이템 획득 요청을 거절합니다.
#define	USER_SP_CHARGE_TIME					5.0f	// 몇초마다 유저의 SP와 Score 싱크 정보를 보내줄 것인가?
#define MAX_WAVE_COUNT						32		// 한 라운드가 가질수 있는 최대 웨이브의 갯수
#define COMBO_CHAIN_TIME					5.0f	// 콤보가 연결되기 위한 시간(초)
#define MAX_LEVEL_SENTRYGUN					4		// 센트리건 최고레벨
#define MAX_LEVEL_DUMMY						4		// 더미 최고레벨
#define MAX_LEVEL_USERSKILL					5		// 유저스킬(일반) 최고레벨
#define MAX_LEVEL_MEATBOMB					1		// 유저스킬(미트봄) 최고레벨
#define COUNTDOWN_WAIT_SECOND				5.0f	// 카운트다운 대기시간(초)
#define MAX_COUNT_DINO_GRADE				6		// 공룡 최대 랭크 수
#define DOMI_USER_FIRST_RESPAWN_TIME		10.0f	// 첫 사망시 리스폰 시간
#define DOMI_USER_ADD_RESPAWN_TIME			5.0f	// 사망할때마다 증가되는 리스폰 시간
#define DOMI_USER_MAX_RESPAWN_TIME			60.0f	// 리스폰 시간 증가가 되지 않는 최대치 리스폰 시간
#define DOMI_USER_DIE_ANIMATION_TIME		3.5f	// 유저 사망 애니메이션 재생 시간
#define ACID_ITEM_LIVE_TIME_SECOND			30		// 애시드 아이템 등장해서 돌아다니는 시간.
#define SENTRYGUN_INSTALL_ANIMATION_TIME	3.0f	// 센트리건 설치,업그레이드시 준비 애니메이션 재생 시간(초)
#define DOMI_USE_COOL_TIME					0.9f	// HP서플라이나 Ammo서플라이 사용후, 재사용 가능하기까지의 시간(초)

#define MAX_COUNT_SDSS						(MAX_COUNT_DOMI_SENTRYGUN + MAX_COUNT_DOMI_DUMMY + MAX_COUNT_DOMI_HPSUPPLY + MAX_COUNT_DOMI_AMMOSUPPLY + MAX_COUNT_DOMI_SKILLSHOP)

#define MAX_COUNT_POLYGON_SEND_ONETIME		100
#define INVALID_IDX							-1
#define SAFE_DELETE_ARRAY(p)				{ if (p) { delete[] p; p = nullptr; } }

// Domi Reward

// **주의** : 36이상이되면 반드시 PROTOCOL_ASC_DOMI_REWARD_INFO_ACK 패킷 사이즈 체크해야합니다.
#define	MAX_DOMI_REWARD_COUNT_PER_INSERT_TYPE	35		// 제압 보상. 하나의 보상 지급 타입당 보상 아이템 최대 갯수.

//#define	MAX_DOMI_REWARD_ROUND_CHECK_NUM			6		// 제압 보상 지급 타입별 라운드 구분을 위한 숫자. DOMI_REWARD_INSERT_TYPE이 0,6,12는 0~10라운드 , 1,7,13은 11~20라운드
#define	MAX_DOMI_REWARD_PROBABILITY				10000	// 제압 보상 최대 확률. 100%.

#define	ROUND_TERM_DOMI_REWARD					10		// DOMI_REWARD_ROUND의 각 항목간 라운드 차이 

#define	MAX_ROUND_COUNT_DOMI_REWARD_INSERT_TYPE		51		// DOMI_REWARD_INSERT_TYPE의 최대 라운드 값.

#define	MAX_DOMI_REWARD_INSERT_TYPE			DOMI_REWARD_CONDITION_MAX * DOMI_REWARD_ROUND_MAX			// 제압 보상 최대 지급 타입 갯수(조건 * 라운드).

// 제압 보상 최대 아이템 갯수 = 지급 타입 갯수 * 하나의 지급 타입당 보상 아이템 최대 갯수 ( PB_BT_DomiRewardItem 테이블기준 로우수 ) 
#define	MAX_DOMI_REWARD_COUNT				MAX_DOMI_REWARD_INSERT_TYPE * MAX_DOMI_REWARD_COUNT_PER_INSERT_TYPE	

// 제압 보상아이템 관련 코드
#define		DOMI_NONE_ADJUST_REWARD	-1

// 제압모드 방을 생성할 때 보낼 수 있는 최대 라운드
#define	UNLOCK_ROUND_MAX	51

#define	ITEM_FLAG_WAEPON	0x01
#define	ITEM_FLAG_CHARA		0x02
#define	ITEM_FLAG_ITEM		0x04
#define ITEM_FLAG_ALL		ITEM_FLAG_WAEPON | ITEM_FLAG_CHARA | ITEM_FLAG_ITEM

#define MISSION_DOMI_PREBATTLE_TIME_OUT		60 * 3

#define CMP_WORD_1(string)						(i3String::Compare( szWord[0], string ) == 0)
#define CMP_WORD_2(string)						(i3String::Compare( szWord[1], string ) == 0)
#define CMP_WORD_3(string)						(i3String::Compare( szWord[2], string ) == 0)
#define CMPP_WORD_1(string1,string2)			((i3String::Compare( szWord[0], string1 ) == 0) || (i3String::Compare( szWord[0], string2 ) == 0))
#define CMPP_WORD_2(string1,string2)			((i3String::Compare( szWord[1], string1 ) == 0) || (i3String::Compare( szWord[1], string2 ) == 0))
#define CMPPP_WORD_1(string1,string2,string3)	((i3String::Compare( szWord[0], string1 ) == 0) || (i3String::Compare( szWord[0], string2 ) == 0) || (i3String::Compare( szWord[0], string3 ) == 0))

class CDomiStage;
class CDominationRoom;
struct DS_LINE;
struct ST_PATH_INPUT;

#pragma pack( push, 1 )

// 제압 인게임 보상. 
struct PACKET_DOMINATION_INSERT_ITEM_USERINVEN_ACK
{
	UINT8		m_ui8Condition = 0;	// 지급 조건. DOMI_REWARD_CONDITION	
};

// 결과창 구조체
struct ST_RESULT_WINDOW_INFO
{
	INT16		_i16KillCount = 0;	// 죽인 공룡 마리수
	INT32		_i32Score = 0;		// 획득한 점수

	void Clear()
	{
		_i16KillCount	= 0;
		_i32Score		= 0;
	}
};

struct	PACKET_H_BASE_REWARD_POPUP_ACK
{
	INT32			m_i32Rv = 0;
	UINT8			m_i8RequestType = 0;	// ITEM_INSERT_REQUEST_TYPE
};

struct	PACKET_H_BATTLE_NEW_READYBATTLE_ACK
{
	INT32				m_i32Rv = 0;
	UINT32				m_ui32StageID = 0;
};

struct PACKET_DOMINATION_PRINT_MESSAGE_ACK
{
	INT16		_i16Time = 0;						// 메세지 출력하고 있는 시간
	char		_strMessage[MAX_STRING_COUNT] = { 0 };	// 메세지 내용
};

// struct P_BATTLE_SERVER_USER_LEAVE_ACK
// {
// 	INT8		_i8SlotIdx;
// 	ROOM_ENDTYPE	eEndType;
// };

struct PACKET_DOMINATION_WAVE_PRE_START_ACK
{
	INT16		_i16Round = 0;			// 라운드값 0부터 시작
	INT8		_i8Wave = 0;			// 웨이브값 0부터 시작
	INT8		_i8MaxWave = 0;			// 최대 웨이브 갯수
	INT16		_i16TotalDinoCount = 0;	// 총 등장할 공룡 마리수
	UINT16		_ui16PlaySlot = 0;
};

struct PACKET_DOMINATION_WAVE_READY_INFO_ACK
{
	bool		_abPress[MAX_COUNT_DOMI_USER_REAL] = { false };		// 인터미션 넘김 명령 상태
};

struct PACKET_DOMINATION_WAVE_END_ACK
{
	bool					_bFinal = false;	// true:최종결과창 false:중간결과창
	ST_RESULT_WINDOW_INFO	_aUser[MAX_COUNT_DOMI_USER_REAL];	// 유저별 점수 구조체
};

// m_ui8RequestType가 ITEM_INSERT_REQUEST_TS_BONUS이면
struct	PACKET_D_BASE_REWARD_DAILY_GIFT
{
	UINT32			m_TItemID = 0;
	UINT32			m_ui32ItemArg = 0;
};

// m_ui8RequestType가 ITEM_INSERT_REQUEST_DOMI_REWARD이면
struct	PACKET_D_BASE_REWARD_DOMI
{
	UINT16			m_ui16Point = 0;
	UINT32			m_TItemID = 0;
	UINT32			m_ui32ItemArg = 0;
};

struct TARGET_INFO
{
	INT16		m_i16TypeIdx = 0;
	INT16		m_i16SlotIdx = 0;

	void copy( TARGET_INFO * pInfo )
	{
		m_i16TypeIdx = pInfo->m_i16TypeIdx ; 
		m_i16SlotIdx = pInfo->m_i16SlotIdx ;
		return; 
	}

	BOOL isEqual( TARGET_INFO * pInfo)
	{
		if( m_i16TypeIdx != pInfo->m_i16TypeIdx ) return FALSE; 
		if( m_i16SlotIdx != pInfo->m_i16SlotIdx ) return FALSE; 

		return TRUE; 
	}
};

struct ST_DINO_INFOMATION
{
	UINT8		_aui8Num[DINO_KIND_COUNT] = { 0 };
};

struct PACKET_DOMINATION_USER_SKILL_UPGRADE_ACK
{
	INT8		_i8UserIdx = 0;		// 유저슬롯번호
	INT8		_i8SkillIdx = 0;	// 어떤 스킬인지	USER_SKILL_XXXX
	INT8		_i8Level = 0;		// 레벨업해서 몇 레벨이 되었는지, 만약 레벨업이 실패했다면 사유(FAIL_REASON_USER_SKILL_UPGRADE_XXX)가 옵니다.
};

struct PACKET_DOMINATION_MUTANTTREX_HOWLING_SLOW_ACK
{
	INT8		_i8SlowTime = 0;		// 느려지는 지속시간(초)
};

struct PACKET_DOMINATION_APPEAR_ACIDITEM_ACK
{
	INT8		_i8Count = 0;		// 애시드아이템 마리수
	INT16		_i16Time = 0;		// 애시드아이템 등장시간(초)
};

struct PACKET_DOMINATION_STAGE_JUMP_ACK
{
	UINT32		_ui32NewStageID = 0;	// 새롭게 점프할 스테이지ID
	UINT16		_ui16UserMask = 0;		// 점프하기 직전의 플레이 유저들 마스크
};

struct ST_POLYGON_TRANS_INFO
{
	INT16		_i16BeginIdx = 0;		// 담겨온 데이터의 시작인덱스번호
	INT16		_i16EndIdx = 0;			// 담겨온 데이터의 끝인덱스번호
};

// PROTOCOL_BATTLE_DOMINATION_NAVIMESH_ACK과 PROTOCOL_BATTLE_DOMINATION_AI_COLLISION_ACK에서 공동으로 사용
struct PACKET_DOMINATION_NAVIMESH_AI_COLLISION_ACK : public ST_POLYGON_TRANS_INFO
{
	INT16		_i16Count = 0;			// 총 갯수
	REAL32		_aav3Polygon[MAX_COUNT_POLYGON_SEND_ONETIME][3][3] = { 0.0f };	// 폴리곤 정보
};

struct PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK
{
	INT8		_i8UserSlot = 0;	// 쏜 유저 슬롯번호
	INT8		_i8DinoSlot = 0;	// 맞은 공룡 슬롯번호
	INT8		_i8EffectID = 0;	// 어떤 특수 효과인지 ( enum WEAPON_SPECIAL_EFFECT )
	INT16		_i16Score = 0;		// 특수효과로 인한 획득 점수
};

struct PACKET_SERVER_USER_LEAVE_ACK
{
	INT32		_i32Score = 0;		// 점수
	INT8		_i8UserIdx = 0;		// 슬롯번호
};

struct ST_DINO_BRAIN_STATIC
{
	INT8		_i8CondCount = 0;
	INT16		_ai16CondID[MAX_COUNT_CONDI] = { 0 };
	INT8		_ai8CondText[MAX_COUNT_CONDI] = { 0 };	//	1-등장	2-사망
	INT32		_i32Arg0 = 0;
	REAL32		_r32AppearTime = 0.0f;					// 
	INT8		_i8Grade = 0;
	INT8		_i8DinoType = 0;
	INT8		_i8RespawnCount = 0;
	INT8		_ai8RespawnIdx[5] = { 0 };
};

struct ST_DINO_BRAIN_DYNAMIC
{
	// 1. i8Active 0->1이 되면
	// 2. r32AppearTime가 줄기 시작하고, 0미만의 값이 되면
	// 3. i8Status가 0->1이 되며 등장한다. 이후 죽으면 2가 된다.
	INT8		_i8Active = 0;					// 0-상기의 조건들 미충족	1-상기의 조건들 충족
	REAL32		_r32AppearRemainTime = 0.0f;		// 등장시간
	INT8		_i8Status = 0;					// 0-등장대기 1-등장 2-사망
	INT8		_i8AllocatedDinoIdx = 0;		// 몇번 슬롯을 배정받았는지. (로그 용도)

	void	Clear()
	{
		_i8Active			= NECESSARY_CONDITION_NOT_ACHIEVE;
		_i8Status			= BRAIN_STATUS_WAIT_FOR_APPEAR;
		_i8AllocatedDinoIdx	= INVALID_IDX;
	}
};

struct ST_UDP_EVENT_MGR
{
	INT8		_i8EventType = 0;
	INT8		_i8RepeatCount = 0;
	INT8		_i8Size = 0;
	char		_cData[64] = { 0 };
};

struct ST_DNODE
{
	INT16		m_i16MyIdx = 0;
	INT16		m_i16ParentIdx = 0;

	REAL32		m_r32H = 0.0f;
	REAL32		m_r32G = 0.0f;
	REAL32		m_r32F = 0.0f;
};

#if (defined CLIENT_SIDE) || (defined DEDI_BUILD)

struct ST_POSROT
{
	VEC3D		_v3Pos;
	INT16		_i16Angle = 0;
};

struct ST_NM_NODE_SCO
{
	VEC3D		m_v3Pos[3];
};

struct ST_NM_NODE_SAM
{
	VEC3D		m_v3Pos[3];
	INT16		m_i16NearIdx[3] = { 0 };
	bool		m_bJump = false;
};

struct ST_PATH_INPUT
{
	CDomiStage*	_pStage = nullptr;
	UINT16		_ui16IdxStart = 0;
	UINT16		_ui16IdxEnd = 0;
	VEC3D*		_pv3Begin = nullptr;
	VEC3D*		_pv3End = nullptr;
	CDominationRoom*	_pDomi = nullptr;
};

class CDomiStageCli
{
public:
	CDomiStageCli() : m_i32NaviMeshCount(0), m_pNaviMeshTri(nullptr) {}
	INT32			LoadSAM( char* pstrFileName );
	void			Destroy();
	INT32			GetNeviMeshCount(void) const	{ return m_i32NaviMeshCount;	}
	ST_NM_NODE_SAM*	GetNeviMeshNode(INT32 i32Index)	{ if(i32Index >= m_i32NaviMeshCount ) return nullptr; return &m_pNaviMeshTri[i32Index]; }

protected:
	ST_NM_NODE_SAM*	m_pNaviMeshTri;
	INT32			m_i32NaviMeshCount;
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// PACKET_DOMINATION_XXXXXXX_REQ 구조체는 클라이언트가 정보를 담아서 보내면 데디케이션 서버가 받아서 처리하기 위한 구조체이고,
// PACKET_DOMINATION_XXXXXXX_ACK 구조체는 데디케이션 서버가 정보를 담아서 보내면 클라이언트가 받아서 처리하기 위한 구조체입니다.
//

// 유저스킬
struct A_TEMP
{
	INT16		_i16SP = 0;					// 업그레이드시 소모되는 SP

	union
	{
		REAL32	_r32IncreaseRatio;		// 업그레이드 스킬 항목 증가량
		INT32	_i32Value;	
	};
};

// 센트리건
struct B_TEMP
{
	INT16		_i16MaxHP = 0;				// 최대 HP
};

// 더미
struct C_TEMP
{
	INT16		_i16DamageExplosion = 0;	// 폭발 데미지
	INT16		_i16MaxHP = 0;				// HP
};

// HP서플라이
struct D_TEMP
{
	UINT16		_ui16SpendSP_Use = 0;		// 이용시 소모되는 SP
};

// Ammo서플라이
struct E_TEMP
{
	UINT16		_ui16SpendSP_Use = 0;		// 이용시 소모되는 SP
};

struct F_TEMP
{
	UINT16		_ui16MaxSP = 0;				// 최대SP양
	UINT16		_ui16BasicSP = 0;			// 기본지급 SP양
};

struct ST_DOMI_GAME_VALUE
{	
	A_TEMP		_aaUserSkill[USER_SKILL_COUNT][MAX_LEVEL_USERSKILL];
	INT8		_ai8UserSkillMaxLevel[USER_SKILL_COUNT] = { 0 };
	B_TEMP		_aSentry[MAX_LEVEL_SENTRYGUN];
	C_TEMP		_aDummy[MAX_LEVEL_DUMMY];
	D_TEMP		_HPSupply;
	E_TEMP		_AmmoSupply;
	INT32		_i32MaxSPNormal = 0;
	INT32		_i32MaxSPHard = 0;
	UINT16		_ui16SentryGunSpendSPInstall = 0;
	UINT16		_ui16SentryGunSpendSPFix = 0;
	UINT16		_ui16DummySpendSPInstall = 0;
	UINT16		_ui16DummySpendSPFix = 0;

	void Copy( ST_DOMI_GAME_VALUE* pDest )
	{
		i3mem::Copy( pDest, this, sizeof( ST_DOMI_GAME_VALUE ) );
	}
};

// 더미
struct G_TEMP
{
	INT16		_i16DamagePrickle = 0;		// 가시 데미지
};

// 센트리건
struct H_TEMP
{
	INT16		_i16Damage = 0;				// 데미지
	REAL32		_r32RotateSpeed = 0.0f;		// 회전속도(1초에 회전할수 있는 각도)
	REAL32		_r32FireSpeed = 0.0f;			// 발사간격(초)
	REAL32		_r32Eye = 0.0f;				// 공격범위
};

struct ST_CONDITION_SERVER
{
	G_TEMP		_aDummy[MAX_LEVEL_DUMMY];
	H_TEMP		_aSentry[MAX_LEVEL_SENTRYGUN];
};

// 공룡
struct ST_DINO_UDP
{
	REAL16		_r16x = 0.0f;
	REAL16		_r16y = 0.0f;
	REAL16		_r16z = 0.0f;
	INT8		_i8DinoKind = 0;
	INT8		_i8State = 0;
	INT8		_i8SubState = 0;
	INT16		_i16HP = 0;
	INT8		_i8AddInfoMask = 0;
};

// 드랍아이템
struct ST_DROPITEM_UDP
{
	REAL16		_r16x = 0.0f;
	REAL16		_r16y = 0.0f;
	REAL16		_r16z = 0.0f;
	INT8		_i8Kind = 0;
};

struct ST_EVENT_MUTANTTREX_HOWL
{
	INT8		_i8DinoIdx = 0;
	INT16		_i16CancelHP = 0;
};

struct ST_EVENT_SCORE
{
	INT8		_i8UserIdx = 0;
	UINT16		_ui16AccSp = 0;
	INT32		_i32AccScore = 0;
	INT16		_i16AccKillDino = 0;
};

struct ST_EVENT_DEATH_DINO
{
	UINT8		_ui8UserMask = 0;	// 2비트씩 각각 1개의 유저. (비트) 00-기여없음 01-기여했음 10-죽였음 11-미사용
	INT8		_i8Combo = 0;
	bool		_bHeadShot = false;		// 헤드샷으로 죽었는지.

	INT16		_i16Sp0 = 0;
	INT16		_i16Score0 = 0;
	INT16		_i16Sp1 = 0;
	INT16		_i16Score1 = 0;
	INT16		_i16Sp2 = 0;
	INT16		_i16Score2 = 0;
	INT16		_i16Sp3 = 0;
	INT16		_i16Score3 = 0;
};

// 제압 보상 아이템. PB_Option - PB_BT_DomiRewardItem 테이블의 한로우와 매칭.
struct DOMI_REWARD_ITEM
{
	UINT16	m_ui16Probability = 0;	// 확률. 1이면 0.01%, 10000이면 100%
	UINT32	m_ui32GoodsID = 0;		// 지급 아이템 종류. 
	UINT16	m_ui16Point = 0;		// 지급 Point. 0이면 지급 안함.
};

// 하나의 지급 라운드당 보상 리스트
struct DOMI_REWARD
{
	UINT8					m_ui8ItemCount = 0;		// 실제로 지급할 아이템(DOMI_REWARD_ITEM)  갯수.
	DOMI_REWARD_ITEM		m_DomiRewardItem[MAX_DOMI_REWARD_COUNT_PER_INSERT_TYPE];	// 아이템 정보. m_ui8ItemCount 갯수까지만 유효.
};


// 하나의 지급 조건당 보상 리스트
struct DOMI_REWARD_LIST
{
	DOMI_REWARD				m_DomiReward[DOMI_REWARD_ROUND_MAX];	// 지급 라운드 별로.
};

struct PACKET_DOMINATION_USE_ITEM_REQ
{
	T_ItemDBIdx	_TItemWareIdx = 0;	// 사용할 아이템
};

// DB에서 가져온 확률값
struct DOMI_REWARD_PROBABILITY
{
	UINT8	m_ui8Grade[GRADE_END] = { 0 };
};

struct PACKET_DOMINATION_CHEAT_MESSAGE_REQ
{
	char	_szMessage[MAX_STRING_COUNT] = { 0 };	// 치트 메세지
};

struct PACKET_DOMINATION_USE_DOMIITEM_RESULT_REQ
{
	INT16	_i16CashGroupType = 0;	// enum CASHITEM_GROUP_TYPE
};

struct PACKET_DOMINATION_USER_SKILL_UPGRADE_REQ
{
	INT8	_i8SkillIdx = 0;		// 어떤 스킬인지	USER_SKILL_XXXX
};

struct PACKET_KICKUSER_ACK
{
	INT8	_i8UserIdx = 0;			// 짜를 유저 슬롯번호
	INT8	_i8Option = 0;			// 0-클라이언트 종료 1-로비로 보냄.
};

struct PACKET_DOMINATION_WAVE_INFO_ACK
{
	INT16	_i16Round = 0;			// 라운드값 0부터 시작
	INT8	_i8Wave = 0;			// 웨이브값 0부터 시작
	INT8	_i8MaxWave = 0;			// 최대 웨이브 갯수
	INT16	_i16TotalDinoCount = 0;	// 총 등장할 공룡 마리수
	bool	_bInter = false;			// true:인터미션 false:웨이브배틀
	INT8	_i8RemainInterSec = 0;	// 인터미션일경우 잔여시간(초)
};

struct ST_RESULT_WINDOW_INFO_FINAL
{
	INT16	_i16Exp = 0;
	INT16	_i16Point = 0;
	UINT8	_ui8ResultIcon = 0;

	void Clear()
	{
		_i16Exp			= 0;
		_i16Point		= 0;
		_ui8ResultIcon	= 0x00;
	}
};

struct PACKET_DOMINATION_WAVE_END_ADD_ACK
{
	ST_RESULT_WINDOW_INFO_FINAL	_UserAdd[MAX_COUNT_DOMI_USER_REAL];
};

struct DINO_DAMAGE
{
	UINT8	ui8WeaponClass = 0;
	UINT8	ui8WeaponUsage = 0;
	UINT8	ui8WeaponNumber = 0;

	UINT32	ui32Damage = 0;
	UINT8	ui8HitPart = 0;
	UINT8	ui8AttackerType = 0;
	UINT32	ui32AttackSlotIdx = 0;
};

struct	DOMINATION_HIGHSCORE
{
	INT32	m_i32DomiScore = 0;
	INT32	m_i32DomiScoreBest = 0;

	BOOL IsEmpty()
	{
		return ( m_i32DomiScore == 0 && m_i32DomiScoreBest == 0 );
	}
};

struct PACKET_DOMINATION_USE_ITEM_ACK
{
	INT8	_i8UserIdx = 0;			// 사용한 유저슬롯번호
	INT16	_i16CashGroupType = 0;	// enum CASHITEM_GROUP_TYPE
	UINT16	_ui16PlaySlot = 0;
};

#pragma pack (pop)

extern char*	g_apstrDinoName[DINO_KIND_COUNT];
extern INT8		g_ai8MaxSkillLevel[USER_SKILL_COUNT];
extern const T_ItemID	g_UnlockRoundItemID[ROUND_UNLOCK_END];
extern char* aszCheatErrMsg[];

////////////////////////////////////////////////////////////////////////////////
// End of Domination

#endif