#if !defined( __GAME_CHARA_ANIM_CONTROL_DEFINE_H__)
#define __GAME_CHARA_ANIM_CONTROL_DEFINE_H__


//	함수 디버깅용. 동작을 호출한 함수와 라인이 표시된다.
#ifdef ANIM_DETAIL
#define TRACE_FUNC				I3TRACE( "(%d) [%s] ", __LINE__, __FUNCTION__);
#else
#define TRACE_FUNC				__noop
#endif

// 네트워크 보정을 위해 값의 오차를 넣었습니다 by 정순구
#define NET_MOVE_DIR_RANGE_VAL			0.3826834323650897717284599840304f

//	ANIM Call
#define  AIPROC_CHARA_IDLE_RUN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
													I3ASSERT( pChara != nullptr);		\
													pChara->getAnimContext()->CallbackAnim_IdleRun( aicontext, tm); }

#define  AIPROC_CHARA_MOVE_RUN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
													I3ASSERT( pChara != nullptr);	\
													pChara->getAnimContext()->CallbackAnim_MoveRun( aicontext, tm); }

#define  AIPROC_CHARA_ATTACK_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
													I3ASSERT( pChara != nullptr);		\
													pChara->getAnimContext()->CallbackAnim_AttackRun( aicontext, tm); }

#define  AIPROC_CHARA_ATTACK_FIN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
													I3ASSERT( pChara != nullptr);		\
													pChara->getAnimContext()->CallbackAnim_AttackFinish( aicontext, tm); }

#define  AIPROC_CHARA_SWAP_WEAPON_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_WeaponSwapRun( aicontext, tm); }

#define  AIPROC_CHARA_LOAD_BULLET_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_LoadBulletRun( aicontext, tm); }

#define	AIPROC_CHARA_LOAD_BULLET_FIN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);	\
														pChara->getAnimContext()->CallbackAnim_LoadBulletFinish( aicontext, tm); }

#define  AIPROC_CHARA_FIRE_BULLET_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_FireBulletRun( aicontext, tm); }

#define	AIPROC_CHARA_FIRE_BULLET_FIN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);	\
														pChara->getAnimContext()->CallbackAnim_FireBulletFinish( aicontext, tm); }

#define  AIPROC_CHARA_LOAD_MAGAZINE_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_LoadMagazineRun( aicontext, tm); }

#define  AIPROC_CHARA_JUMP_START_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr); \
														pChara->getAnimContext()->CallbackAnim_JumpStartRun( aicontext, tm); }

#define AIPROC_CHARA_JUMP_END_RUN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr); \
														pChara->getAnimContext()->CallbackAnim_JumpEndRun( aicontext, tm); }

#define  AIPROC_CHARA_JUMP_END_FIN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_JumpEndFinish( aicontext, tm); }

#define  AIPROC_CHARA_DAMAGE_RUN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_DamageRun( aicontext, tm); }

#define  AIPROC_CHARA_DEATH_FIN( aicontext, tm)			{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);	\
														pChara->getAnimContext()->CallbackAnim_DeathFinish( aicontext, tm); }

#define  AIPROC_CHARA_EXTENSION_ACTIVATE_RUN( aicontext, tm)		\
														{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_ExtensionActivateRun( aicontext, tm); }

#define  AIPROC_CHARA_EXTENSION_DEACTIVATE_RUN( aicontext, tm)		\
														{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_ExtensionDeactivateRun( aicontext, tm); }

#define  AIPROC_CHARA_ATTACK_READY_FIN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_AttackReadyFinish( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLEA_RUN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleARun( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLEA_FIN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleAFinish( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLEB_RUN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleBRun( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLEB_FIN( aicontext, tm)		{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleBFinish( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLE_CHANGE_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleChangeRun( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLE_CHANGE_FIN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleChangeFinish( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLE_STAND_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleStandRun( aicontext, tm); }

#define	AIPROC_CHARA_UI_IDLE_STAND_FIN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_IdleStandFinish( aicontext, tm); }

#define AIPROC_CHARA_UI_CEREMONY_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) aicontext->getOwner(); \
														I3ASSERT( pChara != nullptr);		\
														pChara->getAnimContext()->CallbackAnim_Profile_CeremonyRun( aicontext, tm); }

#define AIPROC_CHARA_LOAD_MAGAZINE_READY_RUN( aicontext, tm)	{ CGameCharaBase *pChara = (CGameCharaBase *) pCtx->getOwner(); \
																I3ASSERT( pChara != nullptr);		\
																pChara->getAnimContext()->CallbackAnim_LoadMagazine_ReadyRun( pCtx, tm); }

//	Lower AI State ID
enum ID_LOWER_AI
{
	ID_LOWER_UNKNOWN = -1,
	ID_LOWER_IDLE = 0,

	ID_LOWER_MOVE_FRONT,
	ID_LOWER_MOVE_FRONT_LEFT,
	ID_LOWER_MOVE_FRONT_RIGHT,
	ID_LOWER_MOVE_LEFT,
	ID_LOWER_MOVE_RIGHT,
	ID_LOWER_MOVE_BACK,
	ID_LOWER_MOVE_BACK_LEFT,
	ID_LOWER_MOVE_BACK_RIGHT,

	ID_LOWER_WALK_FRONT,
	ID_LOWER_WALK_FRONT_LEFT,
	ID_LOWER_WALK_FRONT_RIGHT,
	ID_LOWER_WALK_LEFT,
	ID_LOWER_WALK_RIGHT,
	ID_LOWER_WALK_BACK,
	ID_LOWER_WALK_BACK_LEFT,
	ID_LOWER_WALK_BACK_RIGHT,

	ID_LOWER_JUMP_START,
	ID_LOWER_JUMP_END,


	ID_LOWER_DEATH_FRONT_A1,
	ID_LOWER_DEATH_FRONT_A2,
	ID_LOWER_DEATH_FRONT_A3,

	ID_LOWER_DEATH_LEFT_A,
	ID_LOWER_DEATH_RIGHT_A,
	ID_LOWER_DEATH_BACK_A,

	ID_LOWER_DEATH_FRONT_B,
	ID_LOWER_DEATH_LEFT_B,
	ID_LOWER_DEATH_RIGHT_B,
	ID_LOWER_DEATH_BACK_B,

	ID_LOWER_DEATH_FRONT_H1,
	ID_LOWER_DEATH_FRONT_H2,

	ID_LOWER_DEATH_LEFT_H,
	ID_LOWER_DEATH_RIGHT_H,
	ID_LOWER_DEATH_BACK_H,

	ID_LOWER_JUMP_START_MOVE,
	ID_LOWER_JUMP_END_MOVE,

	ID_LOWER_AI_MAX
};

//	Lower AI State ID
enum ID_LOWER_DINO_AI
{
	ID_LOWER_DINO_UNKNOWN = -1,
	ID_LOWER_DINO_IDLE					= ID_LOWER_IDLE,
	
	ID_LOWER_DINO_MOVE_FRONT			= ID_LOWER_MOVE_FRONT,
	ID_LOWER_DINO_MOVE_FRONT_LEFT		= ID_LOWER_MOVE_FRONT_LEFT,
	ID_LOWER_DINO_MOVE_FRONT_RIGHT		= ID_LOWER_MOVE_FRONT_RIGHT,
	ID_LOWER_DINO_MOVE_LEFT				= ID_LOWER_MOVE_LEFT,
	ID_LOWER_DINO_MOVE_RIGHT			= ID_LOWER_MOVE_RIGHT,
	ID_LOWER_DINO_MOVE_BACK				= ID_LOWER_MOVE_BACK,
	ID_LOWER_DINO_MOVE_BACK_LEFT		= ID_LOWER_MOVE_BACK_LEFT,
	ID_LOWER_DINO_MOVE_BACK_RIGHT		= ID_LOWER_MOVE_BACK_RIGHT,

	ID_LOWER_DINO_WALK_FRONT			= ID_LOWER_WALK_FRONT,
	ID_LOWER_DINO_WALK_FRONT_LEFT		= ID_LOWER_WALK_FRONT_LEFT,
	ID_LOWER_DINO_WALK_FRONT_RIGHT		= ID_LOWER_WALK_FRONT_RIGHT,
	ID_LOWER_DINO_WALK_LEFT				= ID_LOWER_WALK_LEFT,
	ID_LOWER_DINO_WALK_RIGHT			= ID_LOWER_WALK_RIGHT,
	ID_LOWER_DINO_WALK_BACK				= ID_LOWER_WALK_BACK,
	ID_LOWER_DINO_WALK_BACK_LEFT		= ID_LOWER_WALK_BACK_LEFT,
	ID_LOWER_DINO_WALK_BACK_RIGHT		= ID_LOWER_WALK_BACK_RIGHT,

	ID_LOWER_DINO_JUMP_START			= ID_LOWER_JUMP_START,
	ID_LOWER_DINO_JUMP_END				= ID_LOWER_JUMP_END,


	ID_LOWER_DINO_DEATH_FRONT_A1 		= ID_LOWER_DEATH_FRONT_A1,
	ID_LOWER_DINO_DEATH_FRONT_A2 		= ID_LOWER_DEATH_FRONT_A2,
	ID_LOWER_DINO_DEATH_FRONT_A3 		= ID_LOWER_DEATH_FRONT_A3,

	ID_LOWER_DINO_DEATH_LEFT_A			= ID_LOWER_DEATH_LEFT_A,
	ID_LOWER_DINO_DEATH_RIGHT_A			= ID_LOWER_DEATH_RIGHT_A,
	ID_LOWER_DINO_DEATH_BACK_A			= ID_LOWER_DEATH_BACK_A,

	ID_LOWER_DINO_DEATH_FRONT_B			= ID_LOWER_DEATH_FRONT_B,
	ID_LOWER_DINO_DEATH_LEFT_B			= ID_LOWER_DEATH_LEFT_B,
	ID_LOWER_DINO_DEATH_RIGHT_B			= ID_LOWER_DEATH_RIGHT_B,
	ID_LOWER_DINO_DEATH_BACK_B			= ID_LOWER_DEATH_BACK_B,

	ID_LOWER_DINO_DEATH_FRONT_H1 		= ID_LOWER_DEATH_FRONT_H1,
	ID_LOWER_DINO_DEATH_FRONT_H2 		= ID_LOWER_DEATH_FRONT_H2,

	ID_LOWER_DINO_DEATH_LEFT_H			= ID_LOWER_DEATH_LEFT_H,
	ID_LOWER_DINO_DEATH_RIGHT_H			= ID_LOWER_DEATH_RIGHT_H,
	ID_LOWER_DINO_DEATH_BACK_H			= ID_LOWER_DEATH_BACK_H,

	ID_LOWER_DINO_JUMP_START_MOVE		= ID_LOWER_JUMP_START_MOVE,
	ID_LOWER_DINO_JUMP_END_MOVE			= ID_LOWER_JUMP_END_MOVE,

	ID_LOWER_DINO_STATUS_IDLE,
	ID_LOWER_DINO_STATUS_ROAR,
	ID_LOWER_DINO_STATUS_STUN,

	ID_LOWER_DINO_AI_MAX
};

enum
{
	MAX_DEATH_FRONT_A_MOTION	= 3,
	MAX_DEATH_FRONT_H_MOTION	= 2,
};

//	Upper AI State ID
// g_szNameUpperAI_Dino[ ID_UPPER_DINO_AI_MAX]도 같이 고쳐 줘야함
enum ID_UPPER_DINO_AI
{
	ID_UPPER_DINO_UNKNOWN = -1,
	ID_UPPER_DINO_IDLE = 0,
	ID_UPPER_DINO_MOVE,

	ID_UPPER_DINO_ATTACK_A,
	ID_UPPER_DINO_ATTACK_B,

	ID_UPPER_DINO_MOVE_ATTACK_A,
	ID_UPPER_DINO_MOVE_ATTACK_B,
	
	ID_UPPER_DINO_DAMAGE_LEFT,
	ID_UPPER_DINO_DAMAGE_RIGHT,

	ID_UPPER_DINO_JUMP_START,
	ID_UPPER_DINO_JUMP_END,

	ID_UPPER_DINO_DAMAGE_MOVE_LEFT,
	ID_UPPER_DINO_DAMAGE_MOVE_RIGHT,

	ID_UPPER_DINO_WALK,
	ID_UPPER_DINO_SIT_IDLE,


	ID_UPPER_DINO_AI_MAX
};

//	Upper AI State ID
enum ID_UPPER_AI
{
	ID_UPPER_UNKNOWN = -1,
	ID_UPPER_IDLE = 0,
	ID_UPPER_MOVE,

	ID_UPPER_ATTACK,
	ID_UPPER_ATTACK_READY,			//	현재 Throwing만 사용
	ID_UPPER_ATTACK2,				//	현재 Knife만 사용
	ID_UPPER_SECONDARY_ATTACK,		//	현재 Knife만 사용

	ID_UPPER_LOAD_MAGAZINE,
	ID_UPPER_LOAD_MAGAZINE2,
	ID_UPPER_LOAD_BULLET,
	ID_UPPER_SWAP_WEAPON,

	ID_UPPER_JUMP_START,
	ID_UPPER_JUMP_END,

	ID_UPPER_EXTENSION_ACTIVATE,	//	일부 무기만 사용 (현재 M4A1 Scope)
	ID_UPPER_EXTENSION_DEACTIVATE,	//	일부 무기만 사용 (현재 M4A1 Scope)

	ID_UPPER_DAMAGE_FRONT_LEFT_A,
	ID_UPPER_DAMAGE_FRONT_RIGHT_A,
	ID_UPPER_DAMAGE_BACK_A,
	ID_UPPER_DAMAGE_FRONT_LEFT_B,
	ID_UPPER_DAMAGE_FRONT_RIGHT_B,
	ID_UPPER_DAMAGE_BACK_B,

	ID_UPPER_ATTACKBURST,
	ID_UPPER_THROW,
	ID_UPPER_SECONDARY_ATTACKBURST,

	ID_UPPER_LOAD_MAGAZINE_MASTER,
	ID_UPPER_LOAD_MAGAZINE_SHOTGUN_MASTER_C,
	ID_UPPER_LOAD_MAGAZINE_SHOTGUN_MASTER_B,
	ID_UPPER_LOAD_MAGAZINE_SHOTGUN_MASTER_D,

	ID_UPPER_LOAD_MAGAZINE_READY,
	ID_UPPER_FIRE_BULLET,

	// UI Animation (Lobby, ReadyRoom, Inventory, Shop)
	ID_UPPER_NONEBATTLE_IDLE,
	ID_UPPER_NONEBATTLE_SHOWWEAPON,
	ID_UPPER_NONEBATTLE_IDLE_CHANGE,
	ID_UPPER_NONEBATTLE_IDLE_STAND,

	ID_UPPER_AI_MAX
};

enum ID_UPPER_DUAL_AI
{
	ID_UPPER_DUAL_UNKNOWN = -1,
	ID_UPPER_DUAL_IDLE_LEFT = 0,
	ID_UPPER_DUAL_MOVE_LEFT,
	ID_UPPER_DUAL_ATTACK_LEFT,
	ID_UPPER_DUAL_JUMP_START_LEFT,
	ID_UPPER_DUAL_JUMP_END_LEFT,

	ID_UPPER_DUAL_IDLE_RIGHT,
	ID_UPPER_DUAL_MOVE_RIGHT,
	ID_UPPER_DUAL_ATTACK_RIGHT,
	ID_UPPER_DUAL_JUMP_START_RIGHT,
	ID_UPPER_DUAL_JUMP_END_RIGHT,

	ID_UPPER_DUAL_AI_MAX
};

enum ID_COMMON_AI
{
	ID_COMMON_UNKNOWN = -1,
	ID_COMMON_CEREMONY,
	ID_COMMON_DEATH,

	ID_COMMON_AI_MAX
};

enum ID_DEFAULT_AI
{
	ID_DEFAULT_UNKNOWN = -1,
	ID_DEFAULT_IDLE,
	ID_DEFAULT_ATTACKIDLE,
	ID_DEFAULT_ATTACK,
	ID_DEFAULT_DAMAGE,
	ID_DEFAULT_DEATH,

	ID_DEFAULT_AI_MAX
};

//--------------------------------------
//	CHARA_VIEW_TYPE
//--------------------------------------
enum CHARA_VIEW_TYPE
{
	CHARA_VIEW_1PV = 0,			//	1인칭 애니메이션
	CHARA_VIEW_3PV,				//	3인칭 애니메이션

	CHARA_VIEW_MAX
};

#define CHARA_UPPER_BONE_COUNT				51
#define CHARA_LOWER_BONE_COUNT				10

#define CHARA_UPPER_RAPTOR_BONE_COUNT		47
#define CHARA_UPPER_ELITE_BONE_COUNT		41
#define CHARA_UPPER_DINO_BONE_COUNT			46
#define CHARA_LOWER_DINO_BONE_COUNT			32

#define CHARA_UPPER_TREX_BONE_COUNT			38
#define CHARA_LOWER_TREX_BONE_COUNT			32

#define CHARA_UPPER_MUTANT_TREX_BONE_COUNT	36

//	ID_LOWER_AI와 배열순서가 매칭되야 하며, 문자열 이름들은 I3CHR에 등록된 AIState의 이름과 매칭되어야 합니다.
extern const char * g_szNameLowerAI [ ID_LOWER_AI_MAX];

//	ID_LOWER_AI와 배열순서가 매칭되야 하며, 문자열 이름들은 I3CHR에 등록된 AIState의 이름과 매칭되어야 합니다.
extern const char * g_szNameLowerDinoAI [ ID_LOWER_DINO_AI_MAX];

//	ID_UPPER_AI와 배열순서가 매칭되야 하며, 문자열 이름들은 I3CHR에 등록된 AIState의 이름과 매칭되어야 합니다.
extern const char * g_szNameUpperAI[ ID_UPPER_AI_MAX];

extern const char * g_szNameUpperDualAI[ ID_UPPER_DUAL_AI_MAX];

extern const char * g_szNameUpperAI_Dino[ ID_UPPER_DINO_AI_MAX];

extern const char * g_szNameCommonAI[ ID_COMMON_AI_MAX];

extern const char * g_szNameDefaultAI[ ID_DEFAULT_AI_MAX];

extern const char * g_szLowerBoneName[ CHARA_LOWER_BONE_COUNT ];
extern const char * g_szUpperBoneName[ CHARA_UPPER_BONE_COUNT];

extern const char * g_szLowerDinoBoneName[ CHARA_LOWER_DINO_BONE_COUNT ];
extern const char * g_szUpperDinoBoneName[ CHARA_UPPER_RAPTOR_BONE_COUNT];

extern const char * g_szLowerTRexBoneName[ CHARA_LOWER_TREX_BONE_COUNT ];
extern const char * g_szUpperTRexBoneName[ CHARA_UPPER_TREX_BONE_COUNT];

extern const char * g_szUpperMutantTRexBoneName[ CHARA_UPPER_MUTANT_TREX_BONE_COUNT];
//
extern const char * g_szLowerStingBoneName[ CHARA_LOWER_DINO_BONE_COUNT ];
extern const char * g_szUpperStingBoneName[ CHARA_UPPER_DINO_BONE_COUNT];

extern const char * g_szUpperTankBoneName[ CHARA_UPPER_DINO_BONE_COUNT];
extern const char * g_szLowerTankBoneName[ CHARA_LOWER_DINO_BONE_COUNT ];

extern const char * g_szLowerAcidBoneName[ CHARA_LOWER_DINO_BONE_COUNT ];
extern const char * g_szUpperAcidBoneName[ CHARA_UPPER_DINO_BONE_COUNT];

extern const char * g_szLowerEliteBoneName[ CHARA_LOWER_DINO_BONE_COUNT ];
extern const char * g_szUpperEliteBoneName[ CHARA_UPPER_ELITE_BONE_COUNT];

#endif


