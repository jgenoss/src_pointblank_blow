#ifndef __COMMONDEF_STAGE_H__
#define __COMMONDEF_STAGE_H__

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Define

/* StageId bit-field

   3                   2                   1                   0
 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| StageMode |  Weapon |   AI  |C|A| MinorRules  |    StageUID   |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	StageMode	- STAGE_MODE enum 참조
	Weapon		- WEAPON_RESTRICTION enum 참조
	AI			- AI_USE enum 참조
	C			- ClanMatch 가능여부 bool 값.
	A			- 공수교대 가능여부 bool 값.
	Minor Rules - On/Off 형식의 마이너 옵션값( ex - Hud Off, 머리만피격시 데미지 적용, 피격시 바로 사망 )
	StageUID	- StageUID enum 참조

*/
typedef		UINT16		V_STAGEOPTION;	// 옵션이 더 늘어나게 되면 혹시나 몰라서..

// 무기제한 ( 이것은 이제 방 옵션으로 제한 했을 경우에 쓰도록 합니다. ROOM_ADD_INFO )
#define	LOCK_KNUCKLE				0x80
#define LOCK_SHOTGUN				0x40
#define LOCK_SNIPER					0x20
#define LOCK_RPG7					0x10
#define LOCK_PRIMARY				0x08
#define LOCK_SECONDARY				0x04
#define LOCK_MELEE					0x02
#define LOCK_THROW					0x01
#define LOCK_BASIC					0x0F

#define	STAGE_NAME_COUNT			64
#define	STAGE_CODE_NAME_COUNT		32
#define MAX_OPTION_COUNT			14	//(STAGE_OPTION_NONE 제외 수)
#define RULE_STRING_MAX				64
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enum
//KEYWORD : STAGE_ADD
//== 스테이지 추가시 필히 추가항목 ==라자== 각모드의 마지막에추가

typedef UINT32 STAGE_ID;

// STAGE_UID
// 맵 리소스 단위의 고유 식별값
// 플레이 하는 StageMode(Rule)가 달라도 같은맵에서 플레이하는경우 라면
// StageUID가 같아야한다.
enum STAGE_UID
{	// *** 주의. 신규 맵 추가시 반드시 가장 아래쪽에 추가해주세요!! ***

	STAGE_UID_NONE = 0,

	STAGE_UID_PORT_AKABA,
	STAGE_UID_RED_ROCK,
	STAGE_UID_LIBRARY,
	STAGE_UID_MSTATION,
	STAGE_UID_MIDNIGHT_ZONE,
	STAGE_UID_UPTOWN,
	STAGE_UID_BURNINGHALL,
	STAGE_UID_DSQUAD,
	STAGE_UID_CRACKDOWN,
	STAGE_UID_SAINTMANSION,
	STAGE_UID_EASTERNROAD,
	STAGE_UID_DOWNTOWN,
	STAGE_UID_LUXVILLE,
	STAGE_UID_BLOWCITY,
	STAGE_UID_STORMTUBE,
	STAGE_UID_GIRAN2,
	STAGE_UID_BREAKDOWN,
	STAGE_UID_TRAININGCAMP,
	STAGE_UID_SENTRYBASE,
	STAGE_UID_DESERTCAMP,
	STAGE_UID_KICKPOINT,
	STAGE_UID_FACEROCK,
	STAGE_UID_SUPPLYBASE,
	STAGE_UID_SANDSTORM,
	STAGE_UID_SHOPPINGCENTER,
	STAGE_UID_SAFARI,
	STAGE_UID_DRAGONALLEY,
	STAGE_UID_MACHUPICHU,
	STAGE_UID_TWOTOWERS,
	STAGE_UID_ANGKOR,
	STAGE_UID_GHOSTTOWN,
	STAGE_UID_GRANDBAZAAR,
	STAGE_UID_UNDER23,
	STAGE_UID_TAIPEICITYMALL,
	STAGE_UID_SAFARI2,
	STAGE_UID_METRO,
	STAGE_UID_RUSHHOUR,
	STAGE_UID_CARGOPORT,
	STAGE_UID_BLACKMAMBA,
	STAGE_UID_HOLIDAY,
	STAGE_UID_WESTSTATION,
	STAGE_UID_HOUSEMUSEUM,
	STAGE_UID_OUTPOST,
	STAGE_UID_HOSPITAL,
	STAGE_UID_DOWNTOWN2,
	STAGE_UID_CARGOSHIP,
	STAGE_UID_AIRPORT,
	STAGE_UID_SAFEHOUSE,
	STAGE_UID_HARDROCK,
	STAGE_UID_GIRAN,
	STAGE_UID_HELISPOT,
	STAGE_UID_BLACKPANTHER,
	STAGE_UID_BREEDING_NEST,
	STAGE_UID_DUPTOWN,
	STAGE_UID_DINOBREAKDOWN,
	STAGE_UID_DINOLAB,
	STAGE_UID_TUTORIAL,
	STAGE_UID_RANDOMSTAGE,
	STAGE_UID_FACTORYRUINS,
	//STAGE_UID_SEWERS,
	STAGE_UID_MILITARYBASE,
	STAGE_UID_DOWNTOWNSQUARE,
	STAGE_UID_STONYDESERT,
	STAGE_UID_DRAGONNEST,
	STAGE_UID_DM8UNKNOWN,
	STAGE_UID_WATERCOMPLEX,
	STAGE_UID_HOTELCAMOUFLAGE,
	STAGE_UID_PUMPKINHOLLOW,
	STAGE_UID_TESTMAP,
	STAGE_UID_BATTLESHIP,
	STAGE_UID_RAMPARTTOWN,
	STAGE_UID_BALLISTIC,
	STAGE_UID_TEST,
	STAGE_UID_HOLIDAY2,
	STAGE_UID_ROTHENVILLAGE,
	STAGE_UID_MERRYWHITEVILLAGE,
	STAGE_UID_FALLUCITY,
	STAGE_UID_HINDRANCE,
	STAGE_UID_SEWERAGE,
	STAGE_UID_SLUMAREA,
	STAGE_UID_NEWLIBRARY,
	STAGE_UID_SANDSTORM2,
	STAGE_UID_DINORUINS,
	STAGE_UID_FATALZONE,
	STAGE_UID_MARINEWAVE,
	STAGE_UID_STILLRAID,
	STAGE_UID_OLDDOCK,
	STAGE_UID_BIOLAB,
	STAGE_UID_BROKENALLEY,
	STAGE_UID_BANKHALL,
	STAGE_UID_PROVENCE = 91,
	STAGE_UID_M_BRIDGE,
	STAGE_UID_MIHAWK = 93,
	STAGE_UID_DESERTCANYON = 94,
	STAGE_UID_S_TWOTOWERS = 96,
	STAGE_UID_THAISTADIUM = 97,
	STAGE_UID_MAX
};

#define STAGE_AC		64							// STAGE_ARRAY_COUNT
#define MAX_OPEN_STAGE	(STAGE_UID_MAX / STAGE_AC) + 1

// Stage Mode (Bitpattern)
// 항목추가시 클라, 서버 모두 재빌드 필요함.
enum STAGE_MODE
{
	STAGE_MODE_NA = 0,
	STAGE_MODE_DEATHMATCH,
	STAGE_MODE_BOMB,				// 폭파
	STAGE_MODE_DESTROY,				// 파괴
	STAGE_MODE_ANNIHILATION,		// 섬멸
	STAGE_MODE_DEFENCE,				// 방어
	STAGE_MODE_RESERVED1,			// 차후 신규모드 추가시 해당enum 수정하여 사용
	STAGE_MODE_ESCAPE,				// 공룡탈출
	STAGE_MODE_RESERVED2,			// 차후 신규모드 추가시 해당enum 수정하여 사용
	STAGE_MODE_RESERVED3,			// 차후 신규모드 추가시 해당enum 수정하여 사용 
	STAGE_MODE_TUTORIAL,
	STAGE_MODE_RESERVED4,			// 차후 신규모드 추가시 해당enum 수정하여 사용
	STAGE_MODE_CROSSCOUNT,			// 공룡대스매치
	STAGE_MODE_CONVOY,				// 탈취모드
	STAGE_MODE_RUN_AWAY,			// 탈출모드
	STAGE_MODE_RESERVED5,			// 차후 신규모드 추가시 해당enum 수정하여 사용
	STAGE_MODE_CHAMPIONSHIP,		// 대회모드

	STAGE_MODE_ALL,
	STAGE_MODE_MAX = STAGE_MODE_ALL	// 패킷크기 싱크때문에 스테이지 추가시 1식 증가해야 합니다. 
};

// Weapon Restriction
enum WEAPON_RESTRICTION
{
	WEAPON_ALL,					//무기제한 없음
	WEAPON_ASSULT_ONLY,			//돌격소총전
	WEAPON_SMG_ONLY,			//기관단총전
	WEAPON_SNIPER_ONLY,			//저격전
	WEAPON_SHOTGUN_ONLY,		//샷건전
	WEAPON_MACHINEG_ONLY,		//머신건전
	WEAPON_PISTOL_ONLY,			//권총전(주무기 사용불가)
	WEAPON_KNUCKLE_ONLY,		//너클전(주무기,보조무기 사용불가)
	WEAPON_RPG_ONLY,			//RPG전
	WEAPON_CHAMPIONSHIP_RULE,	//대회룰용 무기제한

	WEAPON_RESTRICTION_MAX
};

enum AI_USE
{
	AI_USE_NO,			//AI를 사용하지 않음
	AI_USE_CHALLENGE,	//챌린지 모드 AI사용
	AI_USE_DIEHARD,		//다이하드 모드 AI사용
	AI_USE_MADNESS,		//매드니스 모드 AI사용

	AI_USE_MAX
};

#define RULE_DATA_MAX		256
#define STAGE_DATA_MAX		256
#define STAGE_MATCHING_MAX	1024

enum STAGE_EVENT
{
	STAGE_EVENT_NORMAL = 0,			// 기본 모드
	STAGE_EVENT_SEASON,				// 시즌. 전적 저장 안함.

	STAGE_EVENT_MAX,
};

// 20160311 - StageID 구조 수정
// StageID = Rule + StageUID
// Rule = StageMode + WeaponRestriction + AI + Clan 의 조합으로 생성
namespace StageID
{
	//STAGE_ID 관련 함수 집합용 namespace 
	//Rule 구성에 항목이 추가될 경우 MakeStageID 함수 수정 및 해당 항목 Getter 함수 만들어주세요.

	inline UINT32 MakeStageID(STAGE_MODE eStageMode, WEAPON_RESTRICTION eWeapon, AI_USE eAI, BOOL bClanMatch, BOOL bAtkDefChange, INT32 i32MinorRules = 0, STAGE_UID eStageUID = STAGE_UID_NONE)
	{
		UINT32 ui32StageID = ((eStageMode & 0x3F) << 26 | (eWeapon & 0x1F) << 21 | (eAI & 0x0F) << 17 | bClanMatch << 16 | bAtkDefChange << 15 | (i32MinorRules & 0x7F) << 8 | (eStageUID & 0xFF));

		return ui32StageID;
	};

	inline UINT32 GetRule(STAGE_ID ui32StageID)
	{
		return (ui32StageID & 0xFFFFFF00);
	};

	inline STAGE_MODE GetStageMode(STAGE_ID ui32StageID)
	{
		return static_cast<STAGE_MODE>((ui32StageID >> 26) & 0x3F);
	};

	inline WEAPON_RESTRICTION GetWeaponRestriction(STAGE_ID ui32StageID)
	{
		return static_cast<WEAPON_RESTRICTION>((ui32StageID >> 21) & 0x1F);
	};

	inline AI_USE GetAIMode(STAGE_ID ui32StageID)
	{
		return static_cast<AI_USE>((ui32StageID >> 17) & 0x0F);
	};

	inline bool IsClanMatchStage(STAGE_ID ui32StageID)
	{
		return static_cast<bool>((ui32StageID >> 16) & 0x01);
	};

	inline bool IsAtkDefChangeStage(STAGE_ID ui32StageID)
	{
		return static_cast<bool>((ui32StageID >> 15) & 0x01);
	};

	//Minor Rule Hud Off 적용여부 확인
	inline bool IsHudOff(STAGE_ID ui32StageID)
	{
		return static_cast<bool>((ui32StageID >> 14) & 0x01);
	};

	//헤드샷 온리 설정 적용여부 확인
	inline bool IsHeadShotOnly(STAGE_ID ui32StageID)
	{
		return static_cast<bool>((ui32StageID >> 13) & 0x01);
	};

	//원샷원킬 설정 적용여부 확인
	inline bool IsOneShotOneKill(STAGE_ID ui32StageID)
	{
		return static_cast<bool>((ui32StageID >> 12) & 0x01);
	};

	//스페이스 설정 적용여부 확인
	inline bool IsSpaceMode(STAGE_ID ui32StageID)
	{
		return static_cast<bool>((ui32StageID >> 11) & 0x01);
	};

	// 다중무기 슬롯 설정 적용여부 확인
	inline bool IsMultiWeaponSlot(STAGE_ID ui32StageID)
	{
		return static_cast<bool>((ui32StageID >> 10) & 0x01);
	};

	inline STAGE_UID GetStageUID(STAGE_ID ui32StageID)
	{
		return static_cast<STAGE_UID>(ui32StageID & 0xFF);
	};
};

#define RULE_DATA_MAX			256
#define STAGE_DATA_MAX			256
#define STAGE_MATCHING_MAX		1024
#define STAGE_MATCHING_SEND_MAX	100

#define RANDOM_MAP_MAX_PERCENTAGE 10000

enum RANDOM_MAX_PERSON_TYPE
{
	TYPE_RANDOM_MAX_PERSON16 = 0,
	TYPE_RANDOM_MAX_PERSON10,
	TYPE_RANDOM_MAX_PERSON8,
	TYPE_RANDOM_MAX_PERSON_END // TYPE 0 = 8:8, 1 = 5:5, 2 = 4:4
};

#define RANDOM_MAX_PERSON16		16
#define RANDOM_MAX_PERSON10		10
#define RANDOM_MAX_PERSON8		8

struct MAP_RULE_DATA
{
	INT32	m_i32RuleIdx = 0;
	UINT32	m_ui32Rule = 0;
	UINT8	m_ui8DefaultStage = 0;
	char	m_strRuleLabel[RULE_STRING_MAX] = { 0 };
};

struct MAP_STAGE_DATA
{
	char	m_strCodeName[STAGE_CODE_NAME_COUNT] = { 0 };
	INT32	m_i32Priority = 0;
	bool	m_bIsOpen = false;
};

struct MAP_STAGE_MATCHING
{
	INT32	m_i32RuleIdx = 0;
	UINT32	m_ui32StageID = 0;
	UINT8	m_ui8MaxPerson = 0;
	UINT8	m_ui8Mark = 0;
	UINT8	m_ui8Event = 0;
};

struct MAP_RANDOM_STAGE_DATA
{
	UINT8 m_ui8RuleIdx;
	UINT8 m_ui8StageUID;
	UINT8 m_ui8MaxPerson;
	UINT16 m_ui16Percentage;
};

struct MAP_RANDOM_IDX_DATA
{
	UINT8  m_ui8MaxPersonFlag;						// 0x0000 0111, 0x0001 -> 16, 0x0002 -> 10, 0x0004 -> 8
	UINT16 m_aStartIdx[TYPE_RANDOM_MAX_PERSON_END];
	UINT16 m_aEndIdx[TYPE_RANDOM_MAX_PERSON_END];
};

#endif