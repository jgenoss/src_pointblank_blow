#pragma once

#define MAX_WEAPON_COMMAND_COUNT			8

#define MAX_EMPTYBULLETSOUND_COUNT			32					// 탄피 사운드용 버퍼 카운트
#define MAX_TERRAINSOUND_COUNT				32					// 월드 타격 사운드용 버퍼 카운트
#define BLOODEFFECT_HIT_LENGTH				5.f					// 혈흔이 벽에 붙는 거리
#define DELAY_EMPTYBULLETSOUND				0.7f				// 탄피가 튀는 소리의 딜레이 타임

#define		WEAPON_BOMBSTATE_THROWING		0x00000001			// 던져진 상태
#define		WEAPON_BOMBSTATE_SETTLED		0x00000002			// 착지한 상태
#define		WEAPON_BOMBSTATE_ATTACHOBJECT	0x00000004			// 오브젝트에 붙은 상태
#define		WEAPON_BOMBSTATE_TIMESTART		0x00000008			// 격발(일부 무기는 일정 시간 후에 격발이 된다. 예.C5)
#define		WEAPON_BOMBSTATE_ATTACHWALL		0x00000010			// 벽에 붙었다.
#define		WEAPON_BOMBSTATE_FREEZE			0x00000020			// in world
#define		WEAPON_BOMBSTATE_EXPLOSION		0x00000040			// explosion
#define		WEAPON_BOMBSTATE_HITBOUND		0x00000080			// 무엇인가에 충돌 후 바운드 된 상태 ( 헤어드라이기, 프라이팬과 같은 무기들 다중 공격 처리 방지를 위해 ) // 2014.06.12 ENG_김대영

typedef UINT32 WEAPON_FLAG;
#define WEAPON_FLAG_MULTIPLE_EXTENSION		0x00000001			// 다중 extension 유무
#define WEAPON_FLAG_ENABLE_FIRE_SOUND		0x00000002			// 발사 사운드 사용 가능 여부
#define WEAPON_FLAG_ATTACHED				0x00000004			// 붙어 있는지 체크?
#define WEAPON_FLAG_TAKE_CHARA				0x00000008			// 캐릭터가 소유 여부

enum WEAPON_ATTACH_TYPE
{
	WEAPON_ATTACH_NONE = 0,
	WEAPON_ATTACH_GRIP,						//	총 손잡이
	WEAPON_ATTACH_BARREL,					//	총열 덥개잡이
};

enum WEAPON_DUAL_MAGAZINE_TYPE
{
	WEAPON_DUAL_MAGAZINE_NONE = 0,	// 일반 총기
	WEAPON_DUAL_MAGAZINE_IDLE,		// 듀얼탄창
	WEAPON_DUAL_MAGAZINE_RIGHT,		// 듀얼탄창 우측
};

enum WEAPON_SNIPER_DUAL_MAGAZINE_TYPE
{
	WEAPON_SNIPER_DUAL_MAGAZINE_NONE = 0,
	WEAPON_SNIPER_DUAL_MAGAZINE_SWAP,
	WEAPON_SNIPER_DUAL_MAGAZINE_RELOAD,
};

enum WEAPON_MOVE_SPEED_TYPE
{
	WEAPON_MOVE_SPEED_NORMAL = 0,
	WEAPON_MOVE_SPEED_FIRE,
	WEAPON_MOVE_SPEED_SIT,
	WEAPON_MOVE_SPEED_ZOOM,
};

enum WEAPON_BULLET_HIT_STATE
{
	WEAPON_BULLET_HIT_STATE_NONE = 0,
	WEAPON_BULLET_HIT_STATE_NO_HIT,				///< Collision이 안되었다.
	WEAPON_BULLET_HIT_STATE_LESSRANGE,			///< Collision은 되었지만 무기 사거리가 안된다.
	WEAPON_BULLET_HIT_STATE_HIT,				///< 일반 Hit
	WEAPON_BULLET_HIT_STATE_PENETRATE,			///< 관통 Hit
};

namespace WEAPON {

	enum FIRE_SOUND_TYPE
	{
		FIRE_SOUND_NORMAL		= 0,	//기본 발사 소리
		FIRE_SOUND_NORMAL_2,				//기본 발사 소리 2
		FIRE_SOUND_SCOPE,				//줌상태에서의 발사소리
		FIRE_SOUND_MUFFLER,				//소음기 장착상태의 발사소리
		FIRE_SOUND_EXT,					// 유탄 발사 소리
		FIRE_SOUND_NORMAL_3PV,		//발사 소리 3인칭

		FIRE_SOUND_COUNT
	};

	enum eSTATE
	{
		STATE_READY,						// 대기 상태
		STATE_WAIT,						// 발사가 되고 다음 발사를 기다리는 상태
		STATE_FIRE,						// 발사 애니메이션중
		STATE_FIRING,					// 발사가 가능한 상태 하지만 아직 발사 애니메이션중이다.
		STATE_LOADBULLET,				// 노리쇠를 뒤로 당기는 동작
		STATE_LOADMAG,					// Magazine을 갈아 끼우는 동작
		STATE_NEEDTO_FIRE,				// 발사를 요구하는 상태(연사시에 사용)
		STATE_NEEDTO_LOADBULLET,			// Chara가 Reload를 해야하는 상태
		STATE_NEEDTO_LOADMAG,			// Chara가 Magazine을 갈아 끼워야 하는 상태
		STATE_EMPTY_MAGAZINE,			// 탄약이 빈 상태
		STATE_NEEDTO_QUICKLOAD,			// 듀얼 탄창의 교환을 요구하는 상태
		STATE_NEEDTO_LOADMAGREADY,		// 매거진을 교환하기 전 동작이 필요(샷건에 탄환 넣기 전 동작) - 사용안함
		STATE_NEEDTO_REMOVESHELL,		// 매거진을 비우는 동작
		STATE_NEEDTO_DROPWEAPON,			// 무기를 버리는 동작
		STATE_NEEDTO_LOAD_B_MAG,			// 샷건 마스터일 때 B 매거진이 필요할 때
		STATE_LOADMAG_READY,				// Reload 준비 동작 (완료되면 LOADMAG을 실행한다)
	};


	enum AI
	{
		AI_CURRENT = 0,

		AI_FIRE,							//	Action Fire
		AI_FIRE1PV,
		AI_FIRE3PV,

		AI_FIREMALE,
		AI_FIREFEMALE,

		AI_SECFIRE,						//	Action Secondary Fire
		AI_SECFIRE3PV,
		AI_IDLE,							// Action Idle
		AI_IDLE1PV,
		AI_IDLE3PV,

		AI_LOADBULLET,
		AI_LOADBULLET1PV,
		AI_LOADBULLET3PV,

		AI_LOADMAG_S,

		AI_LOADMAGAZINE,

		AI_LOADMAGAZINEDUAL,				//	Action DualHandGun loadmagazine
		AI_LOADMAGAZINEDUAL1PV_LEFT,
		AI_LOADMAGAZINEDUAL1PV_RIGHT,
		AI_LOADMAGAZINEDUAL3PV_LEFT,
		AI_LOADMAGAZINEDUAL3PV_RIGHT,

		AI_LOADMAGAZINEDUAL1PV_SWAP,		// DSR-1 듀얼 탄창
		AI_LOADMAGAZINEDUAL3PV_SWAP,
		AI_LOADMAGAZINEDUAL1PV_RELOAD,
		AI_LOADMAGAZINEDUAL3PV_RELOAD,

		AI_LOADMAGAZINE_1PV,
		AI_LOADMAGAZINE_3PV,

		// Dual 무기
		AI_LOADMAGAZINE_DUALMAG_ALL,			// 듀얼 탄창 우측에서 좌측으로 전체 교체

		AI_LOADMAGAZINE_DUALMAG_ALL_1PV_MALE,
		AI_LOADMAGAZINE_DUALMAG_ALL_1PV_FEMALE,
		AI_LOADMAGAZINE_DUALMAG_ALL_3PV_MALE,
		AI_LOADMAGAZINE_DUALMAG_ALL_3PV_FEMALE,	

		AI_LOADMAGAZINE_EXTENSION,
		AI_LOADMAGAZINE1PV_EXTENSION,
		AI_LOADMAGAZINE3PV_EXTENSION,

		//캐릭터남녀구분 
		//리로드
		AI_LOADMAGAZINE_1PV_MALE,
		AI_LOADMAGAZINE_3PV_MALE,
		AI_LOADMAGAZINE_1PV_FEMALE,
		AI_LOADMAGAZINE_3PV_FEMALE,

		//익스텐션 리로드
		AI_LOADMAGAZINE1PV_EXTENSION_MALE,
		AI_LOADMAGAZINE3PV_EXTENSION_MALE,
		AI_LOADMAGAZINE1PV_EXTENSION_FEMALE,
		AI_LOADMAGAZINE3PV_EXTENSION_FEMALE,
		
		//익스텐션 마스터 리로드
		AI_LOADMAGAZINE_MASTER1PV_EXT,
		AI_LOADMAGAZINE_MASTER3PV_EXT,

		AI_LOADMAG_E,

		AI_LOADMAG_MASTER_S,

		// 마스터 관련.
		AI_LOADMAGAZINE_MASTER1PV,
		AI_LOADMAGAZINE_MASTER3PV,
		AI_LOADMAGAZINE_RIGHT_MASTER1PV,		// Dual Magazine 무기에서 Ext로 탄창을 바뀐 상태에서 리로드
		AI_LOADMAGAZINE_RIGHT_MASTER3PV,		// Dual Magazine 무기에서 Ext로 탄창을 바뀐 상태에서 리로드

		AI_LOADMAG_MASTER_E,

		AI_CHANGEDUAL,					// Daul Weapon Change
		AI_CHANGEDUAL_LEFT,
		AI_CHANGEDUAL_RIGHT,
		AI_CHANGEDUAL1PV_LEFT,
		AI_CHANGEDUAL1PV_RIGHT,
		AI_CHANGEDUAL3PV_LEFT, 
		AI_CHANGEDUAL3PV_RIGHT,

		AI_MOVE,
		AI_CHANGE,
		AI_CHANGE1PV,
		AI_CHANGE3PV,
		AI_DAMAGE,
		AI_JUMPEND,

		// Extension animation
		AI_EXTENSION_ACTIVATE,
		AI_EXTENSION_DEACTIVATE,
		AI_IDLE_EXTENSION,
		AI_FIRE_EXTENSION,
		AI_LOADBULLET_EXTENSION,
		
		AI_EXTENSION_ACTIVATE3PV,
		AI_EXTENSION_DEACTIVATE3PV,

		// Folded weapon
		AI_IDLE_FOLDED,
		AI_FIRE_FOLDED,
		AI_IDLE_FOLDED_CHANGE,

		//로드뷸렛
		AI_LOADBULLET1PV_MALE,
		AI_LOADBULLET3PV_MALE,
		AI_LOADBULLET1PV_FEMALE,
		AI_LOADBULLET3PV_FEMALE,

		AI_FIREBULLET1PV_MALE,
		AI_FIREBULLET3PV_MALE,
		AI_FIREBULLET1PV_FEMALE,
		AI_FIREBULLET3PV_FEMALE,

		AI_ATTACKREADY_1PV,
		AI_ATTACKREADY_3PV,

		AI_GRENADEATTACKREADY_1PV,
		AI_GRENADEATTACKREADY_3PV,
		
		AI_UI_IDLE_A,
		AI_UI_IDLE_B,

		AI_EXT_IDLE_1PV,
		AI_EXT_IDLE_3PV,

		//UTS용. 좌우 번갈아 넣기(이런..)
		AI_LOADBULLET_LEFT_MALE,
		AI_LOADBULLET_LEFT_FEMALE,
		AI_LOADBULLET_RIGHT_MALE,
		AI_LOADBULLET_RIGHT_FEMALE,

		AI_MAX,
	};

	enum ROTATE_ANGLE // KEYWORD : ROTATE_GRENADE
	{
		WALL_X = 0,
		WALL_Y,
		FLOOR_X,
		FLOOR_Y,
	};
};

struct WEAPON_TERRAIN_SOUND : i3::class_storage_pool<WEAPON_TERRAIN_SOUND>
{
	WEAPON_TERRAIN_SOUND(WEAPON::FIRE_ORDER FireOrder, I3_TERRAIN_TYPE TerrainType, 
							REAL32 Delay, const VEC3D& Pos) :
		_nFireOrder(FireOrder), _nTerrainType(TerrainType), 
		_rDelay(Delay), _vPos(Pos) {}

	WEAPON::FIRE_ORDER	_nFireOrder;
	I3_TERRAIN_TYPE		_nTerrainType;
	REAL32				_rDelay;
	VEC3D				_vPos;
};

typedef WEAPON_TERRAIN_SOUND WEAPON_EMPTYBULLET_SOUND;


struct WEAPON_DROPINFO
{
	INT32		_nUserTeam;				// 드랍된 유저의 팀
	bool		_bDropping;				// 드랍중(네트워크 처리등을 합니다)
	bool		_bDroped;				// 드랍된 상태(무기를 주울수 있는 상태)
	bool		_bContact;				// 최초 드랍후 컬리젼된 상태
	bool		_bSleep;				// 드랍되고 슬립된 상태
	REAL32		_rSleepTimer;			// 드랍된 후 physix를 슬립할지에 대한 타이머
	VEC3D		_vDropDir;				// 드랍된 방향

	void		Reset( void)
	{
		_nUserTeam	= 0;
		_bDropping	= false;
		_bDroped	= false;
		_bContact	= false;
		_bSleep		= false;
		_rSleepTimer = 0.f;
		i3Vector::Zero( &_vDropDir);
	}

	void  ReturnToReady( void)
	{
		_bDropping = false;
		_bDroped = false;
		_bContact = false;
		_bSleep  = false;
		_rSleepTimer = 0.f;
		i3Vector::Zero( &_vDropDir);
	}
};

struct WEAPON_TERRAIN_EFFECT_INFO
{
	I3_TERRAIN_TYPE	_terrainType;
	VEC3D *			_pPos;
	VEC3D *			_pNormal;
	VEC3D *			_pDir;
	REAL32			_rRotate;
	i3Object *		_pObj;
};