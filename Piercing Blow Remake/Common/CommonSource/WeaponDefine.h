#if !defined( __WEAPON_DEFINE_H)
#define __WEAPON_DEFINE_H

#define WEAPON_SUPPLY_SETTIME		2.0f
#define WEAPON_USE_SUPPLY			2.f


#define WEAPON_BOMB_SETTIME				5.5f
#define WEAPON_BOMB_DISARMTIME			7.f

#define WEAPON_LOD_SLOPE		3.5f

#define WEAPON_BULLET_INCREASING_COUNT_40		0.4f	// 보유탄수 증가권 40%

#define GET_BULLET_EXCEPTION_TYPE(t, Type)			(t & (1 << (Type + 8)))			//
#define SET_BULLET_EXCEPTION_TYPE(t, type)			(t | (1 << (type + 8)))		//

#define GET_BULLET_DECREASE_COUNT(t)				(t & 0x000000FF)


//총알 예외처리가 필요한 타입들.
enum WEAPON_EXCEPTION_TYPE
{
	WEAPON_EXCEPTION_TYPE_SILENCE,
	WEAPON_EXCEPTION_TYPE_BAYONET,
	WEAPON_EXCEPTION_TYPE_CROSSFIRE,

	WEAPON_EXCEPTION_TYPE_COUNT
};

enum VALUETYPE {
	VALUETYPE_BOOL,
	VALUETYPE_INT,
	VALUETYPE_REAL,
	VALUETYPE_COLOR,
	VALUETYPE_VEC,
	VALUETYPE_STR,

	VALUETYPE_COUNT
};

namespace WEAPON {
	typedef UINT32 STATE_MASK;
	#define	WSM_FIRE_DISABLE			0x00000001			// 발사 불가
	#define WSM_LOADMAG_DISABLE			0x00000002			// 리로드 불가
	#define	WSM_LOADBULLET_DISABLE		0x00000004			// 노리쇠 전진 불가
	#define WSM_FIRING					0x00000008			// 애니메이션이 플레이중
	#define WSM_WAIT_AUTOFIREDELAY		0x00000010			// 자동 발사 타임을 기다리는중
	#define	WSM_ZOOM					0x00000020			// 줌 기능 사용중
	#define WSM_EXT_DISABLE				0x00000040			// 익스텐션을 사용할 수 없다
	#define WSM_ATTACHOBJECT			0x00000080			// 오브젝트에 붙은 경우
	#define WSM_NEED_SEND_FIREACTION	0x00000100			// 발사 동작을 네트워크로 보내야 하는 경우
	#define WSM_MORE_FIRING				0x00000200			// 연속 발사중
	#define WSM_PLAYED_FIRE_SOUND		0x00000400			// 발사 사운드 플레이 유무
	#define WSM_FIRST_ZOOM				0x00000800			// 첫 줌을 한지에 대한 유무
	#define WSM_READY_TO_LOADMAG		0x00001000			// 장전시에 바로 장전하지 않고 장전 준비 동작을 한다.
	#define WSM_ACTIVATE_EXTENSION		0x00002000			// extension 상태 체크
	#define WSM_WAIT_FOR_LOADMAG		0x00004000			// 장전 완료를 기다린다
	#define WSM_EMPTY_CARTRIDGE			0x00008000			// 탄피가 배출되었는지 체크
	#define WSM_LOADED_BULLET			0x00010000			// 탄약이 비어 있는지 체크
	#define WSM_ACTIVATE_SCOPE			0x00020000			// Scope의 Activate 상태
	#define	WSM_LOADMAG_TO_NEEDBULLET	0x00040000			// LoadMag 후 Bullet에서 탄을 채워야 하는 경우
	#define WSM_POINTSHOTING			0x00080000			// 신규 점사 작동방식 진행 중.


	//	타격 방식
	enum FIRE_TYPE
	{
		FIRE_UNKNOWN = 0,
		FIRE_MELEE,			//	근접
		FIRE_RANGE,			//	발사
		FIRE_THROWING,		//	투척
		FIRE_SFLASH,		//	범위

		FIRE_COUNT
	};

	//	트리거 방식
	enum TRIGGER_TYPE	
	{
		TRIGGER_UNKNOWN = 0,
		TRIGGER_SHOT,			//	즉시 발사
		TRIGGER_RELOADING_SHOT,	//	장전중 발사 가능 및 무기 교체 가능
		TRIGGER_TIMER,			//	시한
		TRIGGER_CRASH,			//	충돌시

		TRIGGER_COUNT
	};

	//	줌 방식 
	enum ZOOM_TYPE			
	{
		ZOOM_UNKNOWN = 0,
		ZOOM_NONE,			//	없음
		ZOOM_ZOOM,			//	견착
		ZOOM_SCOPE,			//	스코프
		ZOOM_STEADY_AIM,		// 애니메이션은 바뀌지 않고, fov만 확대

		ZOOM_COUNT
	};

	//	피해 방식
	enum DAMAGE_TYPE			
	{
		DAMAGE_UNKNOWN = 0,	
		DAMAGE_HP_LOSS,				//	HP 감소	
		DAMAGE_NOTHING,				//	아무 피해 없음
		DAMAGE_FLASH,				//	섬광 효과	
		DAMAGE_HOWL,				//  멍해짐
		DAMAGE_TRAMPLE,				//	밟혀죽음
		DAMAGE_BUFF,				//	밟혀죽음
		DAMAGE_POISON,				// 공룡 독

		DAMAGE_COUNT
	};

	//	무기 공격 방식
	enum FIRE_ORDER
	{
		PRIMARY_FIRE = 0,		//	주 공격	
		SECONDARY_FIRE,			//	보조 공격
		READY_FIRE,
		HITED_FIRE,

		FIRE_ORDER_COUNT
	};

	// 반동 기본 속도 (1초에 3도(Degree)를 회전한다.
	#define		RECOIL_SPEED			1.0f

	// Inven/Shop 출력용 총기 정보
	enum INFO_TYPE
	{
		INFO_DAMAGE = 0,
		INFO_RAPID,
		INFO_HIT_RATE,
		INFO_VERT_RECOIL,
		INFO_HORZ_RECOIL,
		INFO_SPEED,

		INFO_TYPE_COUNT
	};

	enum WEAPON_ROTATE_ANGLE // KEYWORD : ROTATE_GRENADE
	{
		WEAPON_WALL_X = 0,
		WEAPON_WALL_Y,
		WEAPON_FLOOR_X,
		WEAPON_FLOOR_Y,
	};

	inline
	WEAPON_CLASS_TYPE	ItemID2Class( T_ItemID itemID)
	{
		return (WEAPON_CLASS_TYPE) GET_ITEM_SUBTYPE( itemID);
	}

	typedef UINT32 RES_ID;

	inline
	RES_ID	ItemID2ResID( T_ItemID itemID)
	{
		return (RES_ID) GET_ITEM_NUMBER( itemID);
	}

	inline
	WEAPON_SLOT_TYPE	ItemID2Slot( T_ItemID itemid)
	{
		switch( GET_ITEM_TYPE( itemid ) )
		{
		case ITEM_TYPE_PRIMARY:			return WEAPON_SLOT_PRIMARY;
		case ITEM_TYPE_SECONDARY:		return WEAPON_SLOT_SECONDARY;	
		case ITEM_TYPE_MELEE:			return WEAPON_SLOT_MELEE;
		case ITEM_TYPE_THROWING1:		return WEAPON_SLOT_THROWING1;
		case ITEM_TYPE_THROWING2:		return WEAPON_SLOT_THROWING2;
		case ITEM_TYPE_MISSION:			return WEAPON_SLOT_MISSION;
		default:						return WEAPON_SLOT_UNKNOWN;
		}
	}

	#define WEAPON_EXT_MASK_ORIGINAL					0x0000FFFF		// EXT_TYPE 에서 오리지널 값을 뽑아 낼 때 사용됨
	#define	WEAPON_EXT_MASK_NEED_TO_ACTIVATE			0x20000000		// M4A1의 Scope와 같이 사용하기 위해서는 Activate를 시켜야 하는 경우
	#define	WEAPON_EXT_MASK_DIRECT_USE					0x40000000		// Silencer처럼 Activate가 되었다면 항상 사용 상태에 있는 경우
	#define WEAPON_EXT_MASK_THROW						0x80000000		// 무기를 던지는 계열 (수류탄, MiniAxe)
	#define WEAPON_EXT_MASK_FIRE						0x10000000		// 발사시 익스텐션 정보를 사용한다.
	#define WEAPON_EXT_MASK_ADDABLE_DAMAGE				0x01000000		// Damage 계산을 따로 사용하는 Extension(m1887,CrossFire,DMelee)
	#define WEAPON_EXT_MASK_DIFFERANCE					0x02000000		// 왼손 무기가 오른손 무기와 다른 경우

	enum EXT_TYPE
	{
		EXT_NONE = 0,
		EXT_DUAL_MAG		= (0x00000001 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_DIRECT_USE ),
		EXT_SILENCER		= (0x00000002 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE | WEAPON_EXT_MASK_DIRECT_USE ),
		EXT_ZOOM			= (0x00000003 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE | WEAPON_EXT_MASK_DIRECT_USE),
		EXT_DOT_SIGHT		= (0x00000004 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE | WEAPON_EXT_MASK_DIRECT_USE),
		EXT_STEADY_AIM		= (0x00000005 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE | WEAPON_EXT_MASK_DIRECT_USE),
		EXT_SCOPE			= (0x00000006 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE | WEAPON_EXT_MASK_DIRECT_USE),													// Scope
		EXT_THROW_SHELL		= (0x00000007 | WEAPON_EXT_MASK_FIRE),																				// Rocket Launcher 유탄
		EXT_GLAUNCHER		= (0x00000008 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE),											// 보조 유탄
		EXT_SHOTGUN_LAUNCHER= (0x00000009 | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE),											// 보조 샷건
		EXT_BAYONET			= (0x0000000A | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_ADDABLE_DAMAGE),									// 근접 공격형 무기
		EXT_CROSSFIRE		= (0x0000000B | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE | WEAPON_EXT_MASK_ADDABLE_DAMAGE),			// 집중 사격
		EXT_DUAL			= (0x0000000C | WEAPON_EXT_MASK_NEED_TO_ACTIVATE | WEAPON_EXT_MASK_FIRE),											// Dual 무기를 사용 (Kriss류), 미니액스용M7
		EXT_DUAL_LEFT		= (0x0000000D | WEAPON_EXT_MASK_FIRE),																				// Dual Left Weapon의 공격
		EXT_DIRECT_MELEE	= (0x0000000E | WEAPON_EXT_MASK_DIRECT_USE | WEAPON_EXT_MASK_ADDABLE_DAMAGE),										// 근접 공격
		EXT_THROW			= (0x0000000F | WEAPON_EXT_MASK_FIRE),																				// 던지기 (미니액스)
		EXT_DINO			= (0x00000010 | WEAPON_EXT_MASK_DIRECT_USE | WEAPON_EXT_MASK_ADDABLE_DAMAGE),										// 공룡 타입
		EXT_DUAL_LEFT_DIF	= (0x00000011 | WEAPON_EXT_MASK_FIRE | WEAPON_EXT_MASK_DIFFERANCE),	

		EXT_MAX	= 0x00000012
	};

	inline
	INT32 GetExtTypeOriginal( EXT_TYPE eType )
	{
		return eType&WEAPON_EXT_MASK_ORIGINAL;
	}

	enum EXTENSION_TYPE
	{
		EXTENSION_OFF,
		EXTENSION_ON,
	
		EXTENSION_MAX
	};

	enum HAND_TYPE
	{
		HAND_TYPE_LEFT,				// Attack A Damage 1 (ex. Damage, AttackDamageA1 )
		HAND_TYPE_RIGHT,			// Attack A Damage 2 (ex. AttackDamageA2)
		
		HAND_TYPE_MAX
	};

	// 총알 타입
	enum BULLET_SHOT
	{
		BULLET_SHOT_NONE,
		BULLET_SHOT_DIRECT,			// 레이저 총알
		BULLET_SHOT_DROPPING,		// 총알이 보이는 형태
	};

	// 총알 위치
	enum BULLET_POS
	{
		BULLET_POS_LOAD,			// 탄창
		BULLET_POS_STANDBY,			// 장전 대기

		BULLET_POS_MAX,
	};

	enum ATTACK_TYPE
	{
		ATTACK_TYPE_NONE	= -1,	// None		

		ATTACK_TYPE_L1,				// Attack A Damage 1 (ex. Damage, AttackDamageA1 )
		ATTACK_TYPE_L2,				// Attack A Damage 2 (ex. AttackDamageA2)
		ATTACK_TYPE_L3,				// Attack A Damage 3 (ex. AttackDamageA3)

		ATTACK_TYPE_R1,				// Attack B Damage 1 (ex. AttackDamageB1, DirectMelee, Bayonet)
		ATTACK_TYPE_R2,				// Attack B Damage 2 (ex. AttackDamageB2)
		ATTACK_TYPE_R3,				// Attack B Damage 3 (ex. AttackDamageB3)
		ATTACK_TYPE_R4,

		ATTACK_TYPE_MAX
	};

	enum TARGET_TYPE
	{
		TARGET_NONE		= -1,
	
		TARGET_HUMAN	= 0,		// Damage Target Human
		TARGET_DINO		= 1,		// Damage Target Dino
	
		TARGET_TYPE_MAX
	};

	inline
	FIRE_ORDER	AttackType2FireOrder( ATTACK_TYPE type )
	{
		switch( type )
		{
		case ATTACK_TYPE_L1:
		case ATTACK_TYPE_L2:
		case ATTACK_TYPE_L3:
			return PRIMARY_FIRE;
		default:
			return SECONDARY_FIRE;
		}
	}
};

enum BOMB_AREA
{
	BOMB_AREA_A	= 0,
	BOMB_AREA_B,
	MAX_BOMB_AREA_COUNT,
	BOMB_AREA_UNKNOWN,
};

#endif //__WEAPON_DEFINE_H

