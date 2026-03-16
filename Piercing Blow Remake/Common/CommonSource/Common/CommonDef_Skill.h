#ifndef __COMMONDEF_SKILL_H
#define	__COMMONDEF_SKILL_H

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 스킬 시스템 공용 카테고리 스킬 리스트
enum COMMON_SKILL_TYPE
{
	COMMON_SKILL_NONE = -1,

	COMMON_RUN,
	COMMON_WALK,
	COMMON_DAMAGE_RATE,
	COMMON_HELMET_DEF,
	COMMON_JUMP,

	MAX_COMMON_SKILL
};

// 스킬 시스템 도움 카테고리 스킬 리스트
enum ASSIST_SKILL_TYPE
{
	ASSIST_SKILL_NONE = -1,

	///////////////////////////////////////////////////
	//     HANDGUN
	///////////////////////////////////////////////////
	ASSIST_HANDGUN_DAMAGE,
	ASSIST_HANDGUN_CONTROL,
	ASSIST_HANDGUN_FIREDELAY,
	ASSIST_HANDGUN_RELOAD,

	///////////////////////////////////////////////////
	//     MELEE
	///////////////////////////////////////////////////
	ASSIST_MELEE_DAMAGE,
	ASSIST_MELEE_RANGE,
	ASSIST_MELEE_CHANGE,

	///////////////////////////////////////////////////
	//     THROW
	///////////////////////////////////////////////////	
	ASSIST_THROW_DAMAGE,
	ASSIST_THROW_EXPLOSION_RANGE,
	ASSIST_THROW_THROW_RANGE,
	ASSIST_THROW_EXPLOSION_TIME,

	///////////////////////////////////////////////////
	//     ETC
	///////////////////////////////////////////////////
	ASSIST_EXP,
	ASSIST_POINT,
	//ASSIST_MAP_SIGHT,

	MAX_ASSIST_SKILL
};

// 스킬 시스템 메인 카테고리 스킬 리스트
enum MAIN_SKILL_TYPE
{
	MAIN_SKILL_NONE = -1,
///////////////////////////////////////////////////
//     ASSAULT
///////////////////////////////////////////////////
	MAIN_ASSAULT_FIREDELAY,
	MAIN_ASSAULT_DAMAGE,	
	MAIN_ASSAULT_CONTROL,	
	MAIN_ASSAULT_RANGE,
///////////////////////////////////////////////////
//     SMG
///////////////////////////////////////////////////
	MAIN_SMG_FIREDELAY,
	MAIN_SMG_PENETRATE,
	MAIN_SMG_CONTROL,
	MAIN_SMG_RELOAD,
///////////////////////////////////////////////////
//     SNIPER
///////////////////////////////////////////////////
	MAIN_SNIPER_PENETRATE,
	MAIN_SNIPER_CONTROL,
	MAIN_SNIPER_RANGE,
	MAIN_SNIPER_CHANGE,
///////////////////////////////////////////////////
//     SHOTGUN
///////////////////////////////////////////////////	
	MAIN_SHOTGUN_FIREDELAY,
	MAIN_SHOTGUN_CONTROL,
	MAIN_SHOTGUN_RELOAD,
	MAIN_SHOTGUN_CHANGE,
///////////////////////////////////////////////////
//     MG
///////////////////////////////////////////////////
	MAIN_MG_PENETRATE,
	MAIN_MG_CONTROL,
	MAIN_MG_DAMAGE,
	MAIN_MG_RELOAD,

	MAX_MAIN_SKILL
};

// 스킬 카테고리별 최대 달성 가능 레벨
#define MAIN_SKILL_MAX_LEVEL	10
#define ASSIST_SKILL_MAX_LEVEL	5 // 어시스트의 경우 3레벨이 최대인 경우도 있다.
#define COMMON_SKILL_MAX_LEVEL	10

#define SKILL_INFO_BIT			8 // 스킬 정보를 한 변수에 비트를 나눠서 보관
enum CHARA_CLASS
{
	CHARA_CLASS_NONE		= 0,

	CHARA_CLASS_ASSAULT,		// 돌격소총
	CHARA_CLASS_SMG,			// 기관단총
	CHARA_CLASS_SNIPER,			// 저격총
	CHARA_CLASS_SHOTGUN,		// 산탄총
	CHARA_CLASS_MACHINE,		// 기관총

	CHARA_CLASS_COUNT,
};

#pragma pack( push, 1)

// 1.5 스킬 시스템
// 계정의 스킬 장착 정보
struct CHARA_SLOT_SKILL
{
	// 캐릭터 병과
	CHARA_CLASS				m_PrimarySkill = CHARA_CLASS_NONE;

	// 카테고리별 장착 스킬 정보
	COMMON_SKILL_TYPE		m_CommonSkill = COMMON_SKILL_NONE;
	MAIN_SKILL_TYPE			m_MainSkill = MAIN_SKILL_NONE;
	ASSIST_SKILL_TYPE		m_AssistSkill = ASSIST_SKILL_NONE;

	// 카테고리별 장착 스킬 레벨 정보
	UINT8					m_ui8CommonSkillLv = 0;
	UINT8					m_ui8MainSkillLv = 0;
	UINT8					m_ui8AssistSkillLv = 0;

	void	Reset() 
	{
		m_CommonSkill		= COMMON_SKILL_NONE;
		m_AssistSkill		= ASSIST_SKILL_NONE;
		m_MainSkill			= MAIN_SKILL_NONE;
		m_PrimarySkill		= CHARA_CLASS_NONE;
		m_ui8MainSkillLv	= 0;
		m_ui8AssistSkillLv	= 0;
		m_ui8CommonSkillLv	= 0;
	}

	void	CopyTo( CHARA_SLOT_SKILL * pDest)
	{
		pDest->m_CommonSkill		= m_CommonSkill;
		pDest->m_AssistSkill		= m_AssistSkill;
		pDest->m_MainSkill			= m_MainSkill;
		pDest->m_PrimarySkill		= m_PrimarySkill;
		pDest->m_ui8MainSkillLv		= m_ui8MainSkillLv;
		pDest->m_ui8AssistSkillLv	= m_ui8AssistSkillLv;
		pDest->m_ui8CommonSkillLv	= m_ui8CommonSkillLv;
	}
};

// 메인 스킬의 개방 조건
struct MAIN_SKILL_INFO
{
	UINT32 ui32BattleTime = 0;
};

// 보조 스킬의 개방 조건
struct ASSIST_SKILL_INFO
{
	UINT8 ui8Rank = 0;
};

// 공용 스킬의 개방 조건
struct COMMON_SKILL_INFO
{
	UINT8 ui8Rank = 0;
};

#pragma pack( pop )

// 각 스킬들의 개방 조건이 담겨있는 배열
extern MAIN_SKILL_INFO		g_MainSkillInfo[MAX_MAIN_SKILL][ MAIN_SKILL_MAX_LEVEL ];
extern ASSIST_SKILL_INFO	g_AssistSkillInfo[MAX_ASSIST_SKILL][ ASSIST_SKILL_MAX_LEVEL ];
extern COMMON_SKILL_INFO	g_CommonSkillInfo[MAX_COMMON_SKILL][ COMMON_SKILL_MAX_LEVEL ];

BOOL	CheckGetMainSkill( CHARA_CLASS ePrimaryClass, MAIN_SKILL_TYPE eSkill, UINT8 ui8Level, UINT32 ui32BattleTime, bool bIsEnable );
BOOL	CheckGetAssistSkill( ASSIST_SKILL_TYPE eSkill, UINT8 ui8Level, UINT8 ui8Rank, bool bIsEnable );
BOOL	CheckGetCommonSkill( COMMON_SKILL_TYPE eSkill, UINT8 ui8Level, UINT8 ui8Rank, bool bIsEnable );

void	SetMainSkillInfo();
void	SetAssistSkillInfo();
void	SetCommonSkillInfo();

void	GetMainSkillBit( UINT8& ui8SkillType, UINT8& ui8SkillLv, INT32 i32SkillData );
void	GetAssistSkillBit( UINT8& ui8SkillType, UINT8& ui8SkillLv, INT32 i32SkillData );
void	GetCommonSkillBit( UINT8& ui8SkillType, UINT8& ui8SkillLv, INT32 i32SkillData );

void	SetMainSkillBit( UINT8 ui8SkillType, UINT8 ui8SkillLv, INT32& i32SkillData );
void	SetAssistSkillBit( UINT8 ui8SkillType, UINT8 ui8SkillLv, INT32& i32SkillData );
void	SetCommonSkillBit( UINT8 ui8SkillType, UINT8 ui8SkillLv, INT32& i32SkillData );

#endif // #ifndef __COMMONDEF_SKILL_H