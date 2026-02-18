#if !defined( _GAME_SOUND_MANAGER_H__)
#define _GAME_SOUND_MANAGER_H__

#include "GameStringTable.h"

class CGameCharaBase;

///////////////////////////////////////////
//	환경 사운드
enum GSND_LOBBY_AMBIENT_TYPE
{
	GSND_LOBBY_AMBIENT_CHOPPER = 0,
	GSND_LOBBY_AMBIENT_GUNFIRE1,
	GSND_LOBBY_AMBIENT_GUNFIRE2,
	GSND_LOBBY_AMBIENT_ENGINE,
	GSND_LOBBY_AMBIENT_WIND_HOWL,
	GSND_LOBBY_AMBIENT_EMERGENCY,

	GSND_LOBBY_AMBIENT_COUNT,
};

enum GSND_TYPE_BY_TERRAINTYPE
{
	GTBT_MOVE_LEFT_1	= 0,
	GTBT_MOVE_LEFT_2,
	GTBT_MOVE_RIGHT_1,
	GTBT_MOVE_RIGHT_2,
	GTBT_JUMP_START,
	GTBT_JUMP_END,
	GTBT_WPN_HIT_1,
	GTBT_WPN_HIT_2,
	GTBT_WPN_SHOTGUN_HIT_1,
	GTBT_WPN_MELEE_HIT,
	GTBT_WPN_EMPTYCATRIDGE_1,
	GTBT_WPN_SHOTGUN_EMPTYCATRIDGE,
	GTBT_BOUND_THROW,
	GTBT_BOUND_HELMET,
	GTBT_WPN_KUKRII_HIT,
	GTBT_WPN_M1887_HIT,
	GTBT_WPN_GH5007_SEC1_HIT,	// 오른쪽 1타 공격
	GTBT_WPN_GH5007_SEC2_HIT,	// 오른쪽 2타 공격
	GTBT_WPN_MINIAXE_HIT,
	GTBT_BOUND_CANDY,
	GTBT_WPN_BONEKNIFE_HIT,
	GTBT_CHINESECLEAVER,
	GTBT_SUMMEREVENT_WEAPON_HIT,
	GTBT_WATERGUN_HIT,
	//GTBT_WPN_FANG_BLADE_HIT,

	GTBT_MAX_COUNT,
};

enum GSND_TYPE_BY_DINO_TERRAINTYPE
{
	GTTBDTR_MOVE_TREX_LEFT_1	= 0,
	GTTBDTR_MOVE_TREX_LEFT_2,
	GTTBDTR_MOVE_TREX_RIGHT_1,
	GTTBDTR_MOVE_TREX_RIGHT_2,
	GTTBDTR_MOVE_RAPTOR_LEFT_1,
	GTTBDTR_MOVE_RAPTOR_LEFT_2,
	GTTBDTR_MOVE_RAPTOR_RIGHT_1,
	GTTBDTR_MOVE_RAPTOR_RIGHT_2,
	GTTBDTR_MOVE_RAPTOR_JUMP_START,
	GTTBDTR_MOVE_RAPTOR_JUMP_END,
	GTTBDTR_MOVE_TANK_LEFT_1,
	GTTBDTR_MOVE_TANK_LEFT_2,
	GTTBDTR_MOVE_TANK_RIGHT_1,
	GTTBDTR_MOVE_TANK_RIGHT_2,
	GTTBDTR_MOVE_ACID_LEFT_1,
	GTTBDTR_MOVE_ACID_LEFT_2,
	GTTBDTR_MOVE_ACID_RIGHT_1,
	GTTBDTR_MOVE_ACID_RIGHT_2,
	GTTBDTR_MOVE_ACID_JUMP_START,
	GTTBDTR_MOVE_ACID_JUMP_END,

	GTBT_WPN_DINO_HIT,
	GTBT_ATTACK_TREX_HIT,
	GTBT_ATTACK_TANK_HIT,
	
	GTTBDTR_MAX_COUNT,
};

enum GSND_TYPE_BY_CHARACTER_ACTION
{
	GTBCA_ON_THUD = 0,
	GTBCA_ON_SITDOWN,
	GTBCA_ON_STANDUP,
	GTBCA_ON_RUN_EQUIPMENT,
	GTBCA_ON_WALK_EQUIPMENT,
	GTBCA_ON_RUN_RUSTLE,
	GTBCA_ON_WALK_RUSTLE,
	GTBCA_ON_HIT_HEAD,
	GTBCA_ON_KNIFEHIT_HEAD,
	GTBCA_ON_RADIOINTERRUPT,
	GTBCA_ON_DEATH_CHARA,
	GTBCA_ON_ZOOMBREATH,
	GTBCA_ON_DOWNFALL,
	GTBCA_ON_HEARTBEAT,

	GTBCA_ON_WARPGATE,
	GTBCA_ON_HIT_CRITICAL,
	
	GTBCA_MADNESS_ATTACK,
	GTBCA_MADNESS_DEATH,
	GTBCA_MADNESS_IDLE,

	GTBCA_MAX_COUNT,
};

enum GSND_TYPE_BY_DINO_ACTION
{
	GTBDA_ON_BREATH_1 = 0,
	GTBDA_ON_BREATH_2,
	GTBDA_ON_BREATH_3,
	GTBDA_ON_BREATH_4,

	GTBDA_ON_DOWN,
	GTBDA_ON_HIT,
	GTBDA_ON_DAMAGE_LITTLE,
	GTBDA_ON_DAMAGE_SERIOUSLY,
	GTBDA_ON_DAMAGE_DOWNFALL,
	GTBDA_ON_DEATH_HOWL,
	GTBDA_ON_ATTACK_1_A,
	GTBDA_ON_ATTACK_1_B,
	GTBDA_ON_ATTACK_2,

	GTBDA_ON_BITE_HIT,
	GTBDA_ON_SCRATCH_HIT,
	GTBDA_ON_EFFECT,


	GTBDA_MAX_COUNT,
};

enum GSND_TYPE_BY_DINO
{
	GTBD_ON_TREX = 0,
	GTBD_ON_RAPTOR,
	GTBD_ON_STING,
	GTBD_ON_TANK,
	GTBD_ON_ACID,
	GTBD_ON_ELITE,

	GTBD_MAX_COUNT,
};

///////////////////////////////////////////
// NPC Sound
enum GSND_TYPE_BY_NPC_ACTION
{
	GTBNPC_ON_ORC_IDLE = 0,
	GTBNPC_ON_ORC_ATTACK,
	GTBNPC_ON_ORC_DEATH,

	GTBNPC_ON_MAHUM_IDLE,
	GTBNPC_ON_MAHUM_ATTACK,
	GTBNPC_ON_MAHUM_DEATH,

	GTBNPC_MAX_COUNT
};

enum GSND_TYPE_BY_WEAPON_STATE
{
	GTBWS_ON_ZOOM = 0,			//ZOOM 시 효과음
	GTBWS_ON_SCOPE,				//SCOPE시 효과음
	GTBWS_ON_ASSAULTSCOPE,		//ASSAULT계열의 스코프 IN/OUT효과음
	GTBWS_ON_GRAZE_BULLET,		//피탄효과음
	GTBWS_ON_EMPTY_BULLET,		//총알이 없는경우 발사 시도시 효과음
	GTBWS_ON_HIT_HELMET_ATTACK,	//헬멧을 맞추었을경우 공격자가 듣는소리
	GTBWS_ON_HIT_HELMET_DEFENCE,//헬멧을 맞추었을경우 맞은 사람이
	GTBWS_ON_HIT_HUMAN,			//사람을 맞추었을 경우 
	GTBWS_ON_HIT_KNIFE_HUMAN,	//칼로 사람을 맞춘경우
	GTBWS_ON_HIT_THROW,			//투척무기에 맞은경우의 또다른 효과음
	GTBWS_ON_HIT_HELMET_KNIFE,	//칼로 헬멧을 맞추는 경우

	GTBWS_ON_WILDHOG_ATTACK,	// Giran Range golem attack sound
	GTBWS_ON_SIEGE_ATTACK,		// Giran Siege golem attack sound
	GTBWS_ON_SIEGE_ATTACK2,

	GTBWS_ON_GRENADE_EXT,		// GrenadeLauncher On/Off
	GTBWS_ON_GRENADE_FIRE,		// GrenadeLauncher Fire/Empty

	GTBWS_ON_C5,				// C5 격발시 비프음/바운드/

	GTBWS_M197,					

	GTBWS_ON_GRAZE_RPG7,		// RPG7
	GTBWS_ON_HIT_KUKRII,		// Kukrii 사운드 0: 사람, 1:콘크리트 히트
	GTBWS_ON_HIT_GH5007_1,
	GTBWS_ON_HIT_GH5007_2,
	GTBWS_ON_HIT_MINIAXE,
	//GTBWS_ON_HIT_CANDY,

	GTBWS_ON_HIT_KNUCKLE_HUMAN_A,	// Brass Knuckle hit human
	GTBWS_ON_HIT_KNUCKLE_HUMAN_B,	// Brass Knuckle secondary hit human

	GTBWS_ON_HIT_PINDAD_KNIFE,	// Brass Knuckle secondary hit human
	GTBWS_ON_HIT_RAPTOR_BITE,	// Brass Knuckle secondary hit human
	GTBWS_ON_HIT_BONEKNIFE,
	GTBWS_ON_HIT_FANG_BLADE_1,	//FangBlade secondary hit human
	GTBWS_ON_HIT_FANG_BLADE_2,	//FangBlade secondary hit human

	GTBWS_ON_HIT_DINO_STING,	//FangBlade secondary hit human
	
	GTBWS_ON_HIT_BALLISTIC_HUMAN_A,	//BALLISTIC hit human
	GTBWS_ON_HIT_BALLISTIC_HUMAN_B,	//BALLISTIC secondary hit human

	GTBWS_ON_HIT_KERIS_HUMAN_A,	//BALLISTIC hit human
	GTBWS_ON_HIT_KERIS_HUMAN_B,	//BALLISTIC secondary hit human

	GTBWS_ON_HIT_SHOVEL_HUMAN_A,	//SHOVEL hit human
	GTBWS_ON_HIT_SHOVEL_HUMAN_B,	//SHOVEL secondary hit human

	GTBWS_ON_READY_GRENADE,	//SHOVEL secondary hit human

	GTBWS_ON_HIT_CHINESECLEAVER,		// 중식칼 hit human
	GTBWS_ON_HIT_SUMMEREVENT_WEAPON,	// 후라이팬, 헤어드라이기 hit human
	GTBWS_ON_HIT_GOATHAMMER,			// 양망치 Hit Human
	GTBWS_ON_HIT_NUNCHAKU,				// 쌍절곤 Hit Human

	GTBWS_ON_HIT_HALLOWEEN_HAMMER,	// HalloweenHammer

	GTBWS_MAX_COUNT,
};

enum GSND_TYPE_FOR_WPN_C4
{
	GTFW_C4_BUTTON = 0,
	GTFW_C4_INSTALLATION,
	GTFW_C4_COUNT_A,
	GTFW_C4_COUNT_B,
	GTFW_C4_COUNT_C,
	GTFW_C4_COUNT_D,
	GTFW_C4_COUNT_E,
	GTFW_C4_EXPLOSION,
	GTFW_C4_DISARM,
	GTFW_C4_DISARMED,

	GTFW_C4_MAX_COUNT,
};

enum GSND_TYPE_ANNOUNCE
{
	GTA_CHAIN_KILLER = 0,
	GTA_MASSKILL,
	GTA_DOUBLE_KILL,
	GTA_CHAIN_STOPPER,
	GTA_TRIPLE_KILL,
	GTA_HEADSHOT,
	GTA_CHAIN_SLUGGER,
	GTA_PIERCING_SHOT,
	GTA_CHAIN_HEADSHOT,
	GTA_MISSION_HELMETPROTECTION,
	GTA_CHALLENGE_CLEAR,
	GTA_TARGETBONUS,
	GTA_DEATH_BLOW,
	GTA_CK_HP_RECOVERY,
	GTA_OBJECT_DEFENCE,
	GTA_HP_CANDYRECOVERY,
	GTA_TOUCHDOWN,
	GTA_ACEATTACKER,
	GTA_HATTRICK,
	GTA_GAMEMAKER,
	GTA_ASSISTKILL,
	GTA_DEFAULT_KILL,
	GTA_DEFAULT_KILL1,
	GTA_DEFAULT_KILL2,
	GTA_INTERCEPT,
	GTA_HOLDTHELINE,
	GTA_MOBILITYUP,
	GTA_START_MISSION_1,
	GTA_START_MISSION_2,
	GTA_START_MISSION_3,
	GTA_REDTEAM_WIN,
	GTA_BLUETEAM_WIN,
	GTA_BOMB_PLATED,
	GTA_BOMB_EXPLODED,
	GTA_BOMB_REMOVED,
	GTA_MISSION_SUCCESS,
	GTA_MISSION_FAIL,
	GTA_MISSION_WARN_HELICOPTER,
	GTA_MISSION_WARN_GENERATOR,
	GTA_TRAINING_LEVELUP,
	GTA_START_MISSION_RAPTOR,
	GTA_START_MISSION_TREX,
	GTA_MISSION_WARN_ATANK,
	GTA_MISSION_WARN_BTANK,
	GTA_MISSION_BREAK_ATANK,
	GTA_MISSION_BREAK_BTANK,
	GTA_PUMPKIN_KNUCKLE_MALE,
	GTA_PUMPKIN_KNUCKLE_FEMALE,
	GTA_PUMPKIN_KNUCKLE_CHAIN_KILLER,
	GTA_GARENA_KNUCKLE_HIT,
	GTA_REVENGE_EFFECT,
	GTA_GET_WATERMELON_RED,
	GTA_GET_WATERMELON_BLUE,
	GTA_HALLOWEEN_KILL,
	GTA_CAREBERUS_KILL,
	GTA_FIRSTGET_CONVOY,
	GTA_GET_CONVOY,
	GTA_MADNESS_DROPWATER,
	GTA_MADNESS_DROPWATER2,
	GTA_MADNESS_GIRLSCREAM,
	GTA_MADNESS_GIRLSCREAM2,
	GTA_DROP_CONVOY,
	GTA_POINT_CONVOY,
	GTA_MAX_COUNT,
};

enum INFO_TYPE
{
	IT_TERRAIN_TYPE = 0,
	IT_CHARACTER_ACTION,
	IT_WEAPON_STATE,
	IT_ANNOUNCE,
	IT_RADIO,
	IT_DINO_TERRAIN_TYPE,
	IT_DINO_ACTION,
};

enum GSND_OBJECT_TYPE
{
	// StormTube
	GSND_OBJECT_STORMTUBE_RT_IDLE = 0,	// Repair Train anim sound
	GSND_OBJECT_STORMTUBE_RT_MOVE,		// Repair Train anim sound

	GSND_OBJECT_UH60_ALARM,				// Alarm
	GSND_OBJECT_UH60_DAMAGE,			// Damage

	GSND_OBJECT_JUMPHOLD,				// 점프대
	GSND_OBJECT_BATTLE_USE_ITEM,		// 인게임 배틀 아이템

	GSND_OBJECT_COUNT,
};

enum GSND_ROULETTE_TYPE
{
	// StormTube
	GSND_ROULETTE_BOOM = 0,	// Repair Train anim sound
	GSND_ROULETTE_WIN,		// Repair Train anim sound
	GSND_ROULETTE_JACKPOT,				// Alarm
	GSND_ROULETTE_START,
	GSND_ROULETTE_SHUFFLE,
	GSND_ROULETTE_CLEAR,
	GSND_ROULETTE_ITMESET,
	GSND_ROULETTE_END1,
	GSND_ROULETTE_END2,
	GSND_ROULETTE_END3,
	GSND_ROULETTE_END4,
	GSND_ROULETTE_END5,
	GSND_ROULETTE_END6,
	GSND_ROULETTE_END7,
	GSND_ROULETTE_END8,
	GSND_ROULETTE_END9,
	GSND_ROULETTE_END10,
	GSND_ROULETTE_END11,
	GSND_ROULETTE_END12,
	GSND_ROULETTE_END13,
	
	GSND_ROULETTE_COUNT,
};

enum GSND_LOTTERY_TYPE
{
	GSND_LOTTERY_WEAPONLOAD = 0,
	GSND_LOTTERY_BUYYONBUY,
	GSND_LOTTERY_CAPSULEDROP_ONE,
	GSND_LOTTERY_CAPSULEDROP_FIVE,
	GSND_LOTTERY_CAPSULEDROP_TEN,
	GSND_LOTTERY_POPUPCHECK,
	GSND_LOTTERY_CONGRATULATION,
	GSND_LOTTERY_LUCKY,

	GSND_LOTTERY_COUNT,
};

enum PBSND_CHANNEL_GROUP
{
	PBSND_CHANNEL_MASTER = -1,		//채널그룹이 설정되지 않은상태에서의 모든 사운드가 플레이
	PBSND_CHANNEL_NORMAL = 0,		//일반적인 소리//DSP가 적용되는 시점에서는 추가적으로 네트워크 구성이 필요: 현재 필요에따른 볼륨조절용
	PBSND_CHANNEL_OUTSTANDING,		//두드러지게 들리는 소리, 현재 이 그룹이 활성화되면 NORMAL그룹의 볼륨이 작아지게된다.
	
	PBSND_CHANNEL_MAX,
};

struct GSND_TERRAIN_RESERVE_INFO
{
	INFO_TYPE  nInfoType = IT_TERRAIN_TYPE;
	UINT32	Param_1 = 0;
	UINT32	Param_2 = 0;
	UINT32	Param_3 = 0;
	VEC3D	vPos;
	bool	bListener = false;
};

struct RADIO_MSGSND_INFO
{
	i3SoundPlayState	*	pState = nullptr;
	CGameCharaBase	*		pPlayer = nullptr;
	REAL32					m_rElapsedTime = 0.0f;
	REAL32					m_rDuration = 0.0f;
	UINT32					m_Sex = 0;

	void Init()
	{
		pState			= nullptr;
		pPlayer			= nullptr;
		m_rElapsedTime	= 0.0f;
		m_rDuration		= 0.0f;
		m_Sex			= 0;
	}
};

class BGM_INFO
{
public:
	i3SoundPlayInfo			*	_playInfo = nullptr;
	i3SoundPlayState		*	_playState = nullptr;

	char						_szName[MAX_PATH] = { 0 };

	bool						_bSuspendStart = false;
	bool						_bFadeEnable = false;
	bool						_bFadeOut = false;
	bool						_Loop = false;
	REAL32						_rFadeElapsedTime = 0.0f;
	REAL32						_rFadeTime = 0.0f;
	REAL32						_rUpdateInterval = 0.0f;
	REAL32						_rUpdateElapse = 0.0f;
	
	FMOD_CHANNEL_CALLBACK		_Callback;

	BGM_INFO() :_playInfo(0), _playState(0) { _Callback = nullptr; }
};

struct SOUND_INFO
{
	i3SoundPlayInfo			*	_playInfo = nullptr;
	i3SoundPlayState		*	_playState = nullptr;	
};

#define SNDINFO_READY_CNT	256

class CGameSoundManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameSoundManager, i3ElementBase);

protected:
	i3SoundContext		*		m_pSndCtx;

	//Bgm
	BGM_INFO					m_pBackGroundMusic[2];//하나는 fadein 및 주 플레이 , 하나는 fadeout용
	BGM_INFO					m_pDinoBackGroundMusic[2];//하나는 fadein 및 주 플레이 , 하나는 fadeout용


	
	bool						m_bPlayBGM;

	INT32						m_nCurReserveIndex;				
	INT32						m_nInfoIndex;					//링버퍼의 현재 인덱스
	i3SoundPlayInfo		*		m_pInfo[SNDINFO_READY_CNT];	

	bool						m_bEnableOutStandingSound;		//두드러지게 나타나야하는 형태의 소리 Enable여부
	bool						m_bInnerEnableOutStanding;
	REAL32						m_rOutStandingTime;
	REAL32						m_rElapsedOutStandingTime;
	REAL32						m_rCurrentPitch;

	//snd res
	i3GameResSound		*		m_pSndByTerrainType[I3_TERRAIN_TYPE_COUNT][GTBT_MAX_COUNT];
	i3GameResSound		*		m_pSndByCharacterAction[2][GTBCA_MAX_COUNT];
	i3GameResSound		*		m_pSndByWeaponState[3][GTBWS_MAX_COUNT];
	i3GameResSound		*		m_pSndByWeaponState_C4[GTFW_C4_MAX_COUNT];
	i3GameResSound		*		m_pSndAnnounce[GTA_MAX_COUNT];
	i3GameResSound		*		m_pSndRadioMsg[STBL_IDX_RADIOMSG_AUTO_US2 - STBL_IDX_RADIOMSG_CMD_KR + 1][3];
	i3GameResSound		*		m_pSndByNPCAction[ 3][ GTBNPC_MAX_COUNT];
	
	GSND_TERRAIN_RESERVE_INFO	m_ReserveInfo[32];

	i3GameResSound		*		m_pSndByObject[ GSND_OBJECT_COUNT];
	i3GameResSound		*		m_pSndByLobbyAmbient[ GSND_LOBBY_AMBIENT_COUNT];

	SOUND_INFO					m_LobbyAmbientSndInfo[ GSND_LOBBY_AMBIENT_COUNT];

	i3GameResSound		*		m_pSndDinoActionSound[GTBD_MAX_COUNT][GTBDA_MAX_COUNT];
	i3GameResSound		*		m_pSndDinoTerrainTypeSound[I3_TERRAIN_TYPE_COUNT][GTTBDTR_MAX_COUNT];


	i3GameResSound		*		m_pSndByRoulette[ GSND_ROULETTE_COUNT];

	i3GameResSound		*		m_pSndByLottery[ GSND_LOTTERY_COUNT];

	i3::vector<i3GameResSound*> m_vSndByWeaponKill;

	//----------------------------------------------------------------------------------//
	//								For RadioMessage Sound								//
	//----------------------------------------------------------------------------------//
	UINT32						m_nCurrentRadioSndIndex;
	RADIO_MSGSND_INFO			m_RadioSndInfo[SNDINFO_READY_CNT];

protected:
	i3SoundPlayInfo	*	_PrepareStartSound( i3GameResSound * pSound, bool bListener, bool bLoop = false, bool bStopAtAllStop = true);
	i3SoundPlayState *	_StartSound( i3GameResSound * pSound, MATRIX * pMat, bool bListener, bool bLoop = false, bool bStopAtAllStop = true);
	i3SoundPlayState *	_StartSound( i3GameResSound * pSnd, VEC3D * vPos, bool bListener, bool bLoop = false, bool bStopAtAllStop = true);
	i3SoundPlayInfo	*	_GetFreeSndInfo( INT32 * npCnt);

	void				_ProcessRadioMessage( REAL32 rDeltaTime, bool bForce = false);

	void				_ProcessOutstandingSound( REAL32 rDeltaSeconds);

	void				_RegisterNextBGM( const char * pszName, bool bFadeIn = true, bool bLoop = true, REAL32 fadetime = 0.5f, FMOD_CHANNEL_CALLBACK callback = nullptr);
	void				_StartBGM();
	void				_UpdateBGM( REAL32 rDeltaSeconds);
	void				_StopBGM(BGM_INFO * pInfo);
	void				_StopLobbyAmbient(void);

public:
	void				Create( i3SoundContext * pCtx);
	void				CreateChannelGroup(void);

	bool				ReleaseSoundFromRes( i3GameResSound * pRes);
	void				ReleaseSoundFromPlayInfo( i3SoundPlayInfo * pInfo);

	void				InitTerrainTypeSnd( void);
	void				ReleaseTerrainTypeSnd( void);

	void				InitCharacterActionSnd(void);
	void				ReleaseCharacterActionSnd(void);

	void				InitWeaponStateSnd(void);
	void				ReleaseWeaponStateSnd(void);

	void				InitWeapon_C4StateSnd(void);
	void				ReleaseWeapon_C4StateSnd(void);

	void				InitAnnounceSnd();
	void				ReleaseAnnounceSnd();

	void				InitRadioMessage();
	void				ReleaseRadioMessage();

	void				InitObjectSnd( STAGE_ID stageID );
	void				ReleaseObjectSnd( void);

	void				InitLobbyAmbientSnd( void);
	void				ReleaseLobbyAmbientSnd(void);

	void				InitDinoActionSound(void);
	void				ReleaseDinoActionSound(void);

	void				InitDinoTerrainTypeSound(void);
	void				ReleaseDinoTerrainTypeSound(void);


	void				InitRouletteShopSound(void);
	void				ReleaseRouletteShopSound(void);

	void				InitLotterySound(void);
	void				ReleaseLotterySound(void);

	void				InitWeaponKillSound();
	void				ReleaseWeaponKillSound();


	i3SoundPlayState *	tellPlaySound( i3SoundPlayInfo * pInfo);	//soundplayinfo 를 넘겨주고 그정보를 토대로 대신 사운드를 플레이 해준다.

	i3SoundPlayState *	StartTerrainTypeSnd( UINT32 nTerrain, UINT32 nIdx, VEC3D * vPos, bool bListener);
	i3SoundPlayState *	StartDinoTerrainTypeSnd( UINT32 nTerrain, UINT32 nIdx, VEC3D * vPos, bool bListener);
	i3SoundPlayState *	StartCharacterActionSnd( UINT32 nAction, UINT32 nIdx, VEC3D * vPos, bool bListener);
	i3SoundPlayState *	StartDinoActionSound( UINT32 ntype, UINT32 idx, VEC3D * vPos, bool bListener);
	i3SoundPlayState *	StartWeaponStateSnd( UINT32 nState, UINT32 nIdx, VEC3D * vPos, bool bListener);
	i3SoundPlayState *	StartWeapon_C4StateSnd(UINT32 nState, VEC3D * vPos, bool bListener, bool bLoop = false);
	i3SoundPlayState *	StartAnnounceMessage( UINT32 nIndex, VEC3D * vPos = nullptr, bool bListener = true);
	i3SoundPlayState *	StartRadioMessage( UINT32 nIndex, UINT32 nSex, CGameCharaBase * pPlayer);
	i3SoundPlayState *	StartObjectSnd( GSND_OBJECT_TYPE nType, VEC3D * pPos, bool bLoop = false);
	i3SoundPlayState *	StartRouletteSnd( GSND_ROULETTE_TYPE nType, VEC3D * pPos, bool bLoop = false);
	i3SoundPlayState *	StartLotterySnd( GSND_LOTTERY_TYPE nType, VEC3D * pPos=nullptr, bool bLoop = false);

	i3SoundPlayState *	StartWeaponKillSound(UINT32 nIndex, VEC3D * vPos = nullptr, bool bListener = true);

	SOUND_INFO *		StartLobbyAmbientSnd( GSND_LOBBY_AMBIENT_TYPE nType, MATRIX * pMat, INT32 nVolume = 100,
												bool bListener = false, bool bLoop = false);
	
	void				StartBGM(const char * pBGMPath, bool bFadeIn = false, bool bLoop = true, REAL32 fadetime = 0.5f, FMOD_CHANNEL_CALLBACK callback = nullptr);
	void				StopBGM(REAL32 fadetime = 0.5f);
	void				StopDinoBGM(REAL32 fadetime = 2.0f);

	// TerrainType Sound만 동작하도록 만들어 놓은 거심미다. 다른용도로 쓰실경우엔 일단 파악부터 하시고 사용하시기 바랍니다.
	INT32				ReserveSoundPlay( INFO_TYPE nType, UINT32 nParam1, UINT32 nParam2, VEC3D * vPos, bool bListener);
	void				FlushTerrainSound(bool bForce = false);

	void				Drive( REAL32 rDeltaTime, bool bForce = false);

	void				ResetRadioMessageInfo();
	void				setOutStandingEffect( bool bEnable, REAL32 rTime);

	bool				IsPlayBGM(void)				{ return m_bPlayBGM;					}
	SOUND_INFO *		GetLobbyAmbientSound(GSND_LOBBY_AMBIENT_TYPE type )		{ return &m_LobbyAmbientSndInfo[type];		}

	i3SoundContext*		GetSndCtx() { return m_pSndCtx;}
	
public:
	CGameSoundManager();
	virtual ~CGameSoundManager();

	void		EnterStage( i3Stage * pStage);
	void		LeaveStage( void);

#ifdef DOMI_NEW_FLOW
	void		LeaveStageRoundJump( void);
#endif

};

#endif