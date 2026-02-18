#ifndef		__CLIENT_DEF_H__
#define		__CLIENT_DEF_H__

#include "../../../Common/CommonSource/CommonDediCli.h"
#include "../../../Common/CommonSource/Arxan/ArxanAIPS.h"
#include "SkillSystemInfo.h"
#include "ClanStructDef.h"


#if defined( _XIGNCODE_)

#if defined( LOCALE_PHILIPPINES)
#include "../../XignCode3_Philippines/Client/xtypes.h"
#include "../../XignCode3_Philippines/Client/zcwave_errors.h"
#include "../../XignCode3_Philippines/Client/zwave_sdk_client.h"
#include "../../XignCode3_Philippines/Client/zwave_sdk_security.h"
#elif defined( LOCALE_SINGMAL)
#include "../../XignCode3_SingMal/Client/xtypes.h"
#include "../../XignCode3_SingMal/Client/zcwave_errors.h"
#include "../../XignCode3_SingMal/Client/zwave_sdk_client.h"
#else
#include "../../XignCode3/Client/xtypes.h"
#include "../../XignCode3/Client/zcwave_errors.h"
#include "../../XignCode3/Client/zwave_sdk_client.h"
#include "../../XignCode3/Client/zwave_sdk_security.h"
#endif

#endif


#if defined(LOCALE_RUSSIA)
	#define		GUI_ADJUST_RUSSIA
#endif

#define MULTYLANGUAGE		// 다중 언어 지원


//#define ENABLE_GIRAN_NPC				// 기란 NPC 사용
//#define ENABLE_ALT_ENTER				// Alt + Enter로 윈도우모드와 전체모드 변경가능
//#define BREAK_LOAD_CHARARES			//	초기 로딩할때 캐릭터 리소스가 정상 로딩됬는지 확인하기 위해 리소스가 없으면 FATAL 뜬다.
//#define BREAK_LOAD_SOUND				//	초기 로딩할때 사운드 리소스가 정상 로딩됬는지 확인하기 위해 리소스가 없으면 FATAL 뜬다.

#define ENABLE_INVEN_ITEM_DELETE		// (인벤) 아이템 삭제 기능 활성화
#define USE_GAMMAOPTION_BY_SHADER		// 비디오유틸 감마 옵션 셰이더 모드 사용

/****************************************************************************************/
//					QA 관련 (배포가 확정되면 디파인을 상위로 이동해주세요.)
/****************************************************************************************/
//#define JUST_RENDEZVOUS_SERVER		// 꼭 랑데뷰서버를 사용하게 합니다.
//#define JUST_RELAY_SERVER				// 꼭 릴레이서버를 사용하게 합니다.

// 릴레이 서버나, 랑데뷰 서버 둘중에 하나만 켜져 있어야 합니다.
#ifdef JUST_RELAY_SERVER
#ifdef JUST_RENDEZVOUS_SERVER
break;
#endif
#endif
#ifdef JUST_RENDEZVOUS_SERVER
#ifdef JUST_RELAY_SERVER
break;
#endif
#endif

#if defined(I3_DEBUG)
#define BC_LOG "Log\\BC_Debug"
#elif defined(BUILD_RELEASE_QA_VERSION)
#define BC_LOG "Log\\BC"
#else
#define BC_LOG "BC_TEST.log"
#endif

#define LRU_FILE "V3PreCommon.pbc"

#if 0
	#define PB_VM_S		VM_START
	#define PB_VM_E		VM_END
#else
	#define PB_VM_S		__noop;
	#define PB_VM_E		__noop;
#endif

/************************//************************/

/****************************************************************************************/
//					기타 (배포나 QA가 확정되면 디파인을 상위로 이동해주세요.)
/****************************************************************************************/

	#define DOMI_ROUND_RESULT
	#define DOMI_FLOW
	#define DOMI_UTIL
    #define DOMI_CORE
	#define DOMI_OBJECT
	#define DOMI_TAG
	#define DOMI_OPTIMIZE
	#define	DOMI_DINO
	#define DOMI_STING_POISON_GRENADE
	#define DOMI_UDP_OBJECT
	#define DOMI_DINO_MINIMAP_UNIT
	#define DOMI_SKILL_OBJ_MINIMAP_UNIT
	//#define DOMI_MODESCRIPT_FLOW // 제압모드 초기 모드스크립트 작업 시 사용한 플로우(TCP), 폐기예정
	#define DOMI_NEW_FLOW // 새로운 플로우(웨이브 포함), 모드스크립트용 플로우 대체
	//#define DOMI_USER_REVIVAL
	#define DOMI_GRENADE_HACKING
	#define DOMI_STAGE_OBJ_DATA
	#define DOMI_OBSERVER

	#define  THREAD_TCP		//하트비트 스레드
	
	//#define  AUTO_LOBBY

#ifdef BUILD_RELEASE_QA_VERSION
	
	#if defined(DEV_BUILD)
		#define DINO
		//#define ENABLE_BOLD_CAPTION			// UI 캡션 텍스트에 Bold를 적용합니다. (i3GuiEditBox 컨트롤로 되어 있는 것만 적용됩니다.)
		#define	UITEST_MINIMAP_MOVE 
	//#define ENABLE_BUY_USER_TITLE_WEAPON	// (상점) 구입 또는 선물시 호칭 무기일 경우라도 가능하다.

	#elif defined (LOCALE_KOREA)

	//#define ENABLE_INVEN_DISPLAY_OBT		// (인벤) 장비에 'OBT 한정' 표시를 막으려면 주석 처리하세요.
	//#define ENABLE_BUY_USER_TITLE_WEAPON	// (상점) 구입 또는 선물시 호칭 무기일 경우라도 가능하다.

	//#define USE_EVENT_SHUTTLEX

	#elif (LOCALE_THAILAND)

		//#define ENABLE_INVEN_DISPLAY_OBT		// (인벤) 장비에 'OBT 한정' 표시를 막으려면 주석 처리하세요.
		//#define ENABLE_BUY_USER_TITLE_WEAPON	// (상점) 구입 또는 선물시 호칭 무기일 경우라도 가능하다.

	#elif defined (LOCALE_INDONESIA)
		
		
	#elif defined (LOCALE_RUSSIA)
	
	#elif defined (LOCALE_TURKEY)
	
	#elif defined (LOCALE_BRAZIL)

	#elif defined (LOCALE_LATIN_AMERICA)

	#elif defined( LOCALE_NORTHAMERICA)
	
	#elif defined( LOCALE_ITALY)

	#elif defined( LOCALE_ENGLISH)

	#elif defined( LOCALE_MIDDLE_EAST )

	#elif defined( LOCALE_PHILIPPINES  )

	#elif defined( LOCALE_TAIWAN )

	#elif defined( LOCALE_VIETNAM )

	#elif defined ( LOCALE_JAPAN )

	#endif

#endif	//BUILD_RELEASE_QA_VERSION

//	활성화할 경우 무기의 플레이되는 애니매이션 이름이 DebugOutput됩니다.
//#define ENABLE_WEAPON_ANIM_OUTPUT

//	활성화할 경우 캐릭터의 플레이되는 애니매이션 이름이 DebugOutput됩니다.
//#define ENABLE_CHARA_ANIM_OUTPUT
//#define ANIM_DETAIL		//	애니메이션 함수 디버깅용. 동작을 호출한 함수와 라인이 표시된다.
// 무기가 붙는 캐릭터 더미가 DebugOutput 됩니다.
//#define ENABLE_ATTACH_HAND_OUTPUT

//#define USE_CLIENT_SENDCOUNT	// Client의 Send횟수를 조절합니다.


#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
	#define ENABLE_UNIT_TEST 1
#else
	#define ENABLE_UNIT_TEST 0
#endif

//////////////////////////////////////////////////////////////////////////


// 매크로
#define		GET_ARRAY_COUNT(arr)	(sizeof(arr) / sizeof(arr[0]))

// 문자열
#define		UNKNOWN_TEXT			"UNKNOWN"
#define		UNKNOWN_TEXTW			L"UNKNOWN"
#define		INVALID_TEXT			"INVALID"
#define		INVALID_TEXTW			L"INVALID"
#define		COMMAND_KEYWORD			"/TEST"


#define			INVALID_INDEX				(-1)

#define	PHYSIX_CONTACT_REPORT_COUNT		32	// contact report buffer count

#define FIXED_ROOMNAME_COUNT		4

#if defined( DEF_OBSERVER_MODE)	// Observer mode에서는 Slot을 4개 더 추가합니다.
	#define	USER_SLOT_COUNT			SLOT_ALL_COUNT
#else
	#define	USER_SLOT_COUNT			SLOT_MAX_COUNT
#endif

#define INVALID_WAREDB_INDEX			0xFFFFFFFF
#define USE_RELOAD_NEWANIMATION			// 총기 장전 애니메이션 축 변경된 버전

#define BLOCK_TIME_LIMIT				4201010000
#define PERMANENT_BLOCK					9999

enum RESOURCE_LANGUAGE_TYPE
{
	RESOURCE_LANGUAGE_DEV = 0,
	RESOURCE_LANGUAGE_KOREA,
	RESOURCE_LANGUAGE_THAILAND,
	RESOURCE_LANGUAGE_INDONESIA,
	RESOURCE_LANGUAGE_RUSSIA,
	RESOURCE_LANGUAGE_TURKEY,
	RESOURCE_LANGUAGE_BRAZIL,
	RESOURCE_LANGUAGE_LATIN_AMERICA,
	RESOURCE_LANGUAGE_NORTHAMERICA,
	RESOURCE_LANGUAGE_ITALY,
	RESOURCE_LANGUAGE_ENGLISH,
	RESOURCE_LANGUAGE_MIDDLE_EAST,
	RESOURCE_LANGUAGE_PHILIPPINES,
	RESOURCE_LANGUAGE_TAIWAN,
	RESOURCE_LANGUAGE_SINGMAL,
	RESOURCE_LANGUAGE_VIETNAM,
	RESOURCE_LANGUAGE_JAPAN,

	MAX_LANGUAGE_COUNT
};

enum RoomSortType
{
	ROOM_SORT_DEFAULT = -1,		// 정렬상태 유지

	ROOM_SORT_NUMBER = 0,		// 방번호
	ROOM_SORT_ROOMNAME,			// 방이름
	ROOM_SORT_MODE,				// 게임모드
	ROOM_SORT_MAP,				// 맵정보
	ROOM_SORT_PERSON,			// 유저수
	ROOM_SORT_STATE,			// 상태(게임중, 대기중)
	ROOM_SORT_PRIVATE,			// 비공개, 공개방
	ROOM_SORT_PING,				// 핑
	
	MAX_SORT_TYPE
};

enum	COMMUNITY_PERMISSION
{
	COMMUNITY_PERMISSION_INVALID	= -1,

	COMMUNITY_PERMISSION_ALL		= 0,
	COMMUNITY_PERMISSION_NO,
	COMMUNITY_PERMISSION_FRIEND,		// 클랜원도 포함.

	COMMUNITY_PERMISSION_MAX,
};



enum AI_MODE_STATE
{
	AI_MODE_STATE_ORIGINAL = 0,
	AI_MODE_STATE_DIEHARD,
	AI_MODE_STATE_MADNESS,
};

// 치팅 유저를 서버에게 알려주기 위한 항목
enum CHEAT_CONTEXT
{
	//CHEAT_CLIENT와 1:1로 대응합니다.

	// 1000 이하는 서버에서 로그만 남기고 짜르지 않음
	CC_NON_CHEATING = 1,
	CC_MANY_GRENADE_COUNT = 410,		//수류탄, RPG7 여러번 던진다.
	// 1000 이상은 로그를 남기고, 서버에서 짜름
	CC_FILE_FALSIFY = 1000,	//파일 변조 치트
	CC_LOAD_BULLET_COUNT_MG,
	CC_LOAD_BULLET_COUNT_OTHERS,
	CC_FIRE_BULLET_COUNT,
	CC_DAUL_KRISS_SUPPER_V,
	CC_LOAD_BULLET_COUNT_ROCKET,
	CC_LOAD_BULLET_COUNT_K400,
	CC_NEED_TO_LOAD_BULLET_SNIPER,
	CC_PHYSIX_MASK,
	CC_MAX_SPEED,
	CC_INVALID_WEAPON,
	CC_INVALID_CHARA,
	CC_INVALID_DESIGN,
	CC_INVALID_EQUIP_CHARA,
	CC_RECOIL,
	CC_MAGNET,
	CC_KNIFE_SPEED,
	CC_UNKNOWN_POS,
	CC_GRAVITY_HACK,
	CC_AI_HACK,
	CC_WALLSHOT,
	CC_OBSERVER,
	CC_BULLET_COUNT,
	CC_MAX_SPEED_POS,
	CC_QUICK_CHANGE,
	CC_WEAPON_CHANGE,
	CC_AUTOAIM,
	CC_EQUIP_CHANGE,
	CC_GRENADE_POS,
	CC_UNKNOWN_POS2,
	CC_HEAD,
	CC_CHANGE_HITPART,

	CC_MAX_COUNT,
};

// 치팅 유저를 서버에게 알려주기 위한 항목
enum CHEAT_CLIENT
{
	C_NON_CHEATING 					= 0,
	C_MANY_GRENADE_COUNT 			= 1,		//수류탄, RPG7 여러번 던진다.
	C_FILE_FALSIFY 					= 2,	//파일 변조 치트
	C_LOAD_BULLET_COUNT_MG			= 3,
	C_LOAD_BULLET_COUNT_OTHERS		= 4,
	C_FIRE_BULLET_COUNT				= 5,
	C_DAUL_KRISS_SUPPER_V			= 6,
	C_LOAD_BULLET_COUNT_ROCKET		= 7,
	C_LOAD_BULLET_COUNT_K400		= 8,
	C_NEED_TO_LOAD_BULLET_SNIPER	= 9,
	C_PHYSIX_MASK					= 10,
	C_MAX_SPEED						= 11,
	C_INVALID_WEAPON				= 12,
	C_INVALID_CHARA					= 13,
	C_INVALID_DESIGN				= 14,
	C_INVALID_EQUIP_CHARA			= 15,
	C_RECOIL,
	C_MAGNET						= 17,
	C_KNIFE_SPEED					= 18,
	C_UNKNOWN_POS					= 19,
	C_GRAVITY_HACK					= 20,
	C_AI_HACK						= 21,
	C_WALLSHOT						= 22,
	C_OBSERVER,
	C_BULLET_COUNT					= 24,
	C_MAX_SPEED_POS					= 25,
	C_QUICK_CHANGE					= 26,
	C_WEAPON_CHANGE					= 27,
	C_AUTOAIM						= 28,
	C_EQUIP_CHANGE					= 29,
	C_GRENADE_POS					= 30,
	C_UNKNOWN_POS2					= 31,
	C_HEAD							= 32,
	C_CHANGE_HITPART				= 33,

	C_MAX_COUNT,
};

// AI 모드 무기 선택을 위한 사용가능 무기 테이블
extern i3::vector<INT32> g_vecAI_UsablePrimaryWeaponFlag;
extern i3::vector<INT32> g_vecAI_UsableSecondaryWeaponFlag;
extern i3::vector<INT32> g_vecAI_UsableMeleeWeaponFlag;
extern i3::vector<INT32> g_vecAI_UsableThrowingWeaponFlag;
extern i3::vector<INT32> g_vecAI_UsableItemWeaponFlag;

//extern UINT8	g_ui8PacketUpdateIdx;

INT32 GetAiUsablePrimaryWeaponCount(void);
INT32 GetAiUsableSecondaryWeaponCount(void);
INT32 GetAiUsableMeleeWeaponCount(void);
INT32 GetAiUsableThrowingWeaponCount(void);
INT32 GetAiUsableItemWeaponCount(void);

extern void __startlog(LPCTSTR processName);
extern LPCSTR __getLogName();

/****************************************************/
/* 기존에 사용되던 MAKE_ITEM_FLAG 사용법이 변경되어 */
/* 캐시 아이템을 조합하는 순서가 변경되었습니다.    */
/* 일일히 수정하자니 힘들어서 매크로로 뻅니다.		*/
/****************************************************/
#define MAKE_CASH_ITEM_FLAG(event, type, number, class)		(MAKE_ITEM_FLAG(event, type, class, number))

//	캐시 아이템 능력
#define VALUE_CASH_ITEM_MEGA_HP10							(10)		//	HP +10%
#define VALUE_CASH_ITEM_MEGA_HP5							(5)			//	HP +5%

#define VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME			(1)			//	무적시간 +1sec

#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_DAMAGE			(-5)		//	Bullet Proof Vest : 받는 데미지 -5%
#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_PLUS_DAMAGE		(-10)		//	Bullet Proof Vest Plus : 받는 데미지 -10%
#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_GM_DAMAGE			(-90)		//	Bullet Proof Vest GM : 받는 데미지 -90%
#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_METAL_DAMAGE		(-20)		//	Bullet Proof Vest Metal : 받는 데미지 -20%

#define VALUE_CASH_ITEM_FMJ_AMMO_DAMAGE						(10)		//	Full Metal Jacket Ammo : 총알 데미지 +10%		31
#define VALUE_CASH_ITEM_FMJ_AMMO_ACCURACY					(-10)		//	Full Metal Jacket Ammo : 총알 정확도 -10%
#define VALUE_CASH_ITEM_HP_AMMO_DAMAGE						(-10)		//	Hollow Point Ammo : 총알 데미지 -10%			32
#define VALUE_CASH_ITEM_HP_AMMO_ACCURACY					(10)		//	Hollow Point Ammo : 총알 정확도 +10%
#define VALUE_CASH_ITEM_HP_AMMO_PLUS_DAMAGE					(10)		//	Hollow Point Ammo Plus : 총알 데미지 +10%		78
#define VALUE_CASH_ITEM_HP_AMMO_PLUS_ACCURACY				(10)		//	Hollow Point Ammo Plus : 총알 정확도 +10%
#define VALUE_CASH_ITEM_JACKETED_HP_AMMO_DAMAGE				(5)			//	Jacketed Hollow Point Ammo : 총알 데미지 +5%		36
#define VALUE_CASH_ITEM_JACKETED_HP_AMMO_ACCURACY			(5)			//	Jacketed Hollow Point Ammo : 총알 정확도 +5%
#define VALUE_CASH_ITEM_JACKETED_HP_AMMO_MOVEMENT			(-10)		//	Jacketed Hollow Point Ammo : 이동속도 -10%

#define VALUE_CASH_ITEM_C4SPEEDKIT_INSTALL					(-2)		//	C4 Speed Kit : C4 설치 시간 감소 -2sec
#define VALUE_CASH_ITEM_C4SPPEDKIT_DISARM					(-2)		//	C4 Speed Kit : C4 해체 시간 감소 -2sec

#define VALUE_CASH_ITEM_QUICK_CHANGE_WEAPON					(-50)		//	Quick change weapon increase percentage (-50%)
#define VALUE_CASH_ITEM_QUICK_CHANGE_MAGAZINE				(-25)		//	Quick change magezine increase percentage (-25%)

#define VALUE_CASH_ITEM_INCREASE_GRENADE_SLOT				(1)			//	more grenade
#define VALUE_CASH_ITEM_INCREASE_DUAL_GRENADE_SLOT			(2)			//	more dual grenade

#define VALUE_CASH_ITEM_ANTI_FLASHBANG_MASK					(-50)		//	Anti FlashBang Mask : 플래시뱅 효과 -50%

#define VALUE_CASH_ITEM_KETUPAT_HP10						(10)		//	HP +10%
#define VALUE_CASH_ITEM_VALENTINE_CHOCOLATE					(10)		//  HP + 10 (퍼센트 아님)


//#define VALUE_CASH_ITEM_SPEEDUP_BOOTS						(10)		//	Speed Up Boots


#define VALUE_CASH_ITEM_SHORT_RESPAWN_20					(0.2f)		// 리스폰 단축	: CASHITEM_GROUP_SHORT_RESPAWN_20
#define VALUE_CASH_ITEM_SHORT_RESPAWN_30					(0.3f)		// 리스폰 단축	: CASHITEM_GROUP_SHORT_RESPAWN_30
#define VALUE_CASH_ITEM_SHORT_RESPAWN_50					(0.5f)		// 리스폰 단축	: CASHITEM_GROUP_SHORT_RESPAWN_50



/////////////////////////////////////////////////////////////////////////////////


// 글로벌 함수

bool	IsGameMasterNickName(const wchar_t * pNickName);

void		FormatLocation(UINT32 State, i3::rc_wstring& out);

bool			IsWeaponByIncreaseGrenadeSlot( WEAPON_CLASS_TYPE nClass, UINT32 nNumber);	// Increase grenade slot 캐시템에 적용되는 무기인지 판단
bool			IsWeaponByIncreaseDualGrenadeSlot( WEAPON_CLASS_TYPE nClass, UINT32 nNumber); // Increase grenade slot 캐시템에 적용되는 듀얼 무기인지 판단
UINT32			ConvertSecToUseDay(UINT32 sec);
UINT32			ConvertSecToUseHour( UINT32 sec);
UINT32			ConvertHourToUseDay(UINT32 hour);
UINT32			ConvertSecToUseMinute(UINT32 sec);
INT32			GetCashItemByGroup( T_ItemID ItemID);


void				ApplyClientStringTable(void); 

// --------------------------------------------------------------------------------------------------
// 2016-11-02 User PC Specs 추가
void GetCurrentPcSpecs(LPDIRECT3D9 pD3d, USER_INFO_PCSPECS& tInfo);
// --------------------------------------------------------------------------------------------------

// 애매해서 임시로 막아 놓은 코드.
#define			TEMP_ERASE_CODE

#define SKILL_UNDER_HP10	10.0f
#define SKILL_UNDER_HP30	30.0f

// 스킬 정보
enum SKILL_APPLY_TYPE
{
	WEAPON_SKILL_NONE = 0,					// 안쓰는 스킬(CC모드, 주특기 쓰레기값용)

	WEAPON_ATTACKRANGE_LV1,					// 사정 거리 상승		LV1
	WEAPON_CHANGESPEED_LV1,					// 무기 교체 속도 단축	LV1
	WEAPON_RELOADSPEED_LV1,					// 재장전 속도 단축		LV1
	WEAPON_FIRERATE_LV1,					// 연사력 상승			LV1
	WEAPON_ACC_LV1,							// 명중률 상승			LV1
	WEAPON_PENETRATE_LV1,					// 관통력 상승			LV1
	CHARACTER_MOVESPEED_LV1,				// 이동 속도 상승		LV1
	WEAPON_FIREPOWER_LV1,					// 공격력 상승			LV1
	WEAPON_BOMBTIMEREDUCE_LV1,				// 폭발 시간 단축		LV1
	WEAPON_THROWSOUNDREDUCE_LV1,			// "투척" 음 감소		LV1
	WEAPON_BOMBRANGEINCREASE_LV1,			// 폭파 범위 상승		LV1

	WEAPON_ATTACKRANGE_LV2,					// 사정 거리 상승		LV2
	WEAPON_CHANGESPEED_LV2,					// 무기 교체 속도 단축	LV2
	WEAPON_RELOADSPEED_LV2,					// 재장전 속도 단축		LV2
	WEAPON_FIRERATE_LV2,					// 연사력 상승			LV2
	WEAPON_ACC_LV2,							// 명중률 상승			LV2
	WEAPON_PENETRATE_LV2,					// 관통력 상승			LV2
	CHARACTER_MOVESPEED_LV2,				// 이동 속도 상승		LV2
	WEAPON_FIREPOWER_LV2,					// 공격력 상승			LV2
	WEAPON_BOMBTIMEREDUCE_LV2,				// 폭발 시간 단축		LV2
	WEAPON_THROWSOUNDREDUCE_LV2,			// "투척" 음 감소		LV2
	WEAPON_BOMBRANGEINCREASE_LV2,			// 폭파 범위 상승		LV2

	SKILL_APPLY_MAX,
};

enum SKILL_MOVE_TYPE
{
	MOVE_SKILL_FIRE = 0,
	MOVE_SKILL_SIT,
	MOVE_SKILL_STAND,
	MOVE_SKILL_ZOOM,

	MOVE_SKILL_MAX,
};


#if 0
class WeaponCore
{
	MemoryCore< UINT32 > m_WeaponList[ WEAPON_SLOT_COUNT ];	// 0x00 FF(SlotIdx) FF(ClassType) FF(Number)

public:
	WeaponCore();
	void Reset();

	//암호화 상태로 넘긴다.
	//const UINT32* GetWeaponList() const;
	//void GetWeaponList() const;

	MemoryCore< UINT32 >& GetList();
	MemoryCore< UINT32 >* GetListPtr();
	MemoryCore< UINT32 >& operator[](size_t index);
	const MemoryCore< UINT32 >& operator[](size_t index) const;
	
	//암호화된 내용을 원래되로 복원한 값을 반환한다.
	UINT32 GetWeapon(UINT32 index) const;
	//저장시에 암호화 한다.
	void SetWeapon(UINT32 index, UINT value);

	//암호화된 내용을 복사한다.
	WeaponCore& operator = (const WeaponCore & other);

	//암호화된 리스트를 복원해서 서로 비교한다.
	bool operator==(const WeaponCore & other);
	bool operator!=(const WeaponCore & other);
};
#endif



// 초대하기 목록
struct USER_INFO_INVITE
{
	UINT32	_idxSession = UINT_MAX;					// User SessionIdx
	UINT32	_rank = 0;							// 추후 UINT8로 수정
	i3::rc_wstring	_wstr_nick;				
#ifdef USE_COLOR_NICK2
	UINT8	_nickColor = 0;
#endif
	
	void Reset(void)
	{
		_idxSession = UINT_MAX;
		_rank		= 0;
		_wstr_nick.clear();
#ifdef USE_COLOR_NICK2
		_nickColor	= 0;
#endif
	}

} ;

struct INVITE_INFO
{
	i3::rc_wstring	_wstr_nick;
	INT32	_idxRoom = 0;
	T_UID	_iUID = 0;
	UINT8	_pw[NET_ROOM_PW] = { 0 };
};

void ResetInviteInfo(INVITE_INFO& out_info);

// 초대자 정보 Auth->GS, GS->Client 
struct SERVER_INVITER_INFO
{
	T_UID _uid = 0;
	INT32 _sesseionIdx = 0;
	INT32 _serverIdx = 0;
	INT32 _channelIdx = 0;
	INT32 _roomIdx = 0;
	char _nick[NET_NICK_NAME_SIZE] = { 0 };
} ;

#define LSR_AVATAR_SYSTEM

// 체크되는 무기 슬롯의 최대 카운트 
// (제압모드 머징으로 인해 WEAPON_SLOT_COUNT 값이 변경되어WEAPON_SLOT_COUNT - 1 으로 체크되는부분 대체함)
#define	MAX_WEAPON_CHECK_SLOT_COUNT			5

namespace i3
{
	namespace range_detail
	{
		inline
		const char*  range_begin(const CString& t) { return (const char*)t;	}
		inline 
		const char*  range_end(const CString& t ) { return ((const char*)t) + t.GetLength(); }
	}

	template<>
	struct range_const_iterator<CString>		{ typedef const char*	type; };			// 
	
}

namespace i3_locale
{
	template<class Integral, class Range>
	void itoa_comma(Integral val, Range & out)
	{
#if defined( LOCALE_ITALY )
		i3::itoa(val, out);
#else
		i3::itoa_comma(val, out);
#endif
	}

	template<class Integral, class Lit>
	void itoa_comma(Integral val, Lit out, size_t buff_size)
	{
#if defined( LOCALE_ITALY )
		i3::itoa(val, out, buff_size);
#else
		i3::itoa_comma(val, out, buff_size);
#endif
	}
}

struct CHAT_INFO
{
protected:
	i3::rc_wstring	wstr;

public:
	UINT16 _Type = 0;

	void	Reset() { _Type = 0; wstr.clear(); }

	const i3::rc_wstring&	getString(void)	const { return wstr; }
	void	setString( const i3::rc_wstring& wstrMsg)
	{
		I3ASSERT( wstrMsg.size() < MAX_CHATTING_COUNT);
		wstr = wstrMsg;
	}
};

//STAGE_ID 가 0이면 안되는거다.
#define STAGE_ID_NA 0

//RULE_ID 가 0이면 이벤트용 룰이다.
#define RULE_ID_EVENT 0

//여기에 추가해 놓음
enum STAGE_SLOT_MODE
{
	STAGE_SLOT_MODE_NORMAL = 0,
	STAGE_SLOT_MODE_4VS4,
	STAGE_SLOT_MODE_5VS5,

	STAGE_SLOT_MODE_MAX
};

#endif	// __CLIENT_DEF_H__