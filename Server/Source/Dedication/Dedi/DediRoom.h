// DediRoom.h
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		#include "DediMember.h"
//		#include "UdpBuffer.h"
//	
#ifndef _DediRoom_h
#define _DediRoom_h


#include "DediMember.h"
#include "UdpBuffer.h"
#include "DediUdpBuilder.h"
#include "InBattleDef.h"
#include "S2CriticalSection.h"
#include "hms.h"
#include "RespawnState.h"
#include "WeaponTable.h"
#include "EquipmentTable.h"
#include "CharacterTable.h"
#include "ModuleControl.h"
#include "Character.h"
#include "COctreeNode.h"
#include "DSObjectGroup.h"
#include "DSObjectUseItemManager.h"
#include "ThrowWeaponMgr.h"
// -------------------------
// forward declaration
// -------------------------
class CStageInfoTimer;
class CDroppedWeaponMgr;
class CRespawnMgr;

// -------------------------
// Inline utility functions
// -------------------------
inline BOOL IS_VALID_GAME_OBJECT  ( UINT32 uIdx )	{ return (uIdx < DS_MAX_OBJECT);			}
inline BOOL IS_VALID_MEMBER_RANGE ( UINT32 uIdx )	{ return (uIdx < SLOT_MAX_COUNT);			}
inline BOOL IS_VALID_THROW_WEAPON ( UINT32 uIdx )	{ return (uIdx < MAX_THROWWEAPON_COUNT);	}
inline BOOL IS_VALID_STATIC_OBJECT( UINT32 uIdx )	{ return (uIdx < OBJ_STATIC_COUNT);			}
inline BOOL IS_VALID_ANIM_OBJECT  ( UINT32 uIdx )	{ return (uIdx < OBJ_ANIM_COUNT);			}
inline BOOL IS_VALID_DROPWEAPON	  ( UINT32 uIdx )	{ return (uIdx < MAX_DROPEDWEAPON_COUNT);	}
inline BOOL IS_VALID_CONTROLLED	  ( UINT32 uIdx )	{ return (uIdx < OBJ_CONTROLED_COUNT );		}
inline BOOL IS_VALID_SUBHEAD	  ( UINT32 uIdx )	{ return (uIdx < P2P_SUB_HEAD_COUNT );		}

#define DS_FIRST_ANIMATION_SYNC_TIME	9.0f


enum DS_DAMAGE_RESULT
{
	RESULT_FAIL		= 0,	// FAIL: set zero to damage.
	RESULT_SERVER	= 1,	// SUCCESS: server damage.
	RESULT_RANDOM	= 2,	// SUCCESS  but random damage applied
	RESULT_CLIENT	= 3,	// use client damage (head shot, falling, object, fast object, XML Error)
	RESULT_BUFF		= 4,	// buff damage: apply server damage(healing)
};

enum PACKET_CHECK
{
	PACKET_CHECK_SUCCESS,			// 문제를 발견 할 수 없음
	PACKET_CHECK_FAIL,				// 인자값중에 문제가 발생하여 모든 패킷을 드랍합니다.
	PACKET_CHECK_DELETE,			// 굳이 처리할 필요가 없어 해당 패킷만을 삭제 합니다.
};

enum HIT_TYPE_SHOTGUN
{                               // 4M  8M  12M 16M 20M 24M 28M
	HIT_TYPE_DEFAULT	= 0,	//  8   7   5   4   3   2   0
	HIT_TYPE_M1877		= 1,	//  9   7   6   4   3   2   0
	HIT_TYPE_KELTEC		= 2,	//  7   6   5   4   3   2   0
	HIT_TYPE_SPAS_MSC	= 3,	//  8   6   5   4   4   3   1
	HIT_TYPE_JACK		= 4,	//  10  7   5   2   0   0   0
	HIT_TYPE_MAX,
};

enum HIT_DISTANCE_SHOTGUN
{                              
	HIT_DISTANCE_4M		= 0,
	HIT_DISTANCE_8M		= 1,
	HIT_DISTANCE_12M	= 2,
	HIT_DISTANCE_16M	= 3,
	HIT_DISTANCE_20M	= 4,
	HIT_DISTANCE_24M	= 5,
	HIT_DISTANCE_28M	= 6,
	HIT_DISTANCE_MAX,
};

#define RUN_AWAY_OBJECT_COUNT		2
// 참고: user는 network client를 의미. 즉, 게임 중인 유저로 create room, enter, leave와 관련
//		 character는 game character. death, respawn, hp.. 등.

//  Mode Rate
#define HEAD_HUNTER_HIT_PARTS_HEAD_RATE 150.f
#define HEAD_HUNTER_HIT_PARTS_NOT_HEAD_RATE 0.01f

#define CHAOS_HIT_PARTS_HEAD_RATE 35.f

struct DetectRate
{
public:
	
	INT8	m_i8DetectRateForByPass[HACK_TYPE_MAX];

	INT8 IncreaseDetectRateCnt(DS_HACK_TYPE Type)
	{
		if (++m_i8DetectRateForByPass[Type] >9)
			  m_i8DetectRateForByPass[Type] =0;


		return m_i8DetectRateForByPass[Type];
	}

	void Reset(){
		for (int i =0; i< HACK_TYPE_MAX; i++) m_i8DetectRateForByPass[i] = 0;
	}

	INT8 GetDetectRateCntForByPass(DS_HACK_TYPE Type )	{return m_i8DetectRateForByPass[Type];}
private:
	
};

// 멀티쓰레드 지원 안합니다.
class CPacketUpdate
{
	INT32					m_i32RecvIdx;
	INT32					m_i32SendIdx;
	UINT32					m_ui32UpdateList[ MAX_PACKET_UPDATE_TIME ];

public:
	CPacketUpdate()			{	m_i32RecvIdx=0;	m_i32SendIdx=0;	};
	~CPacketUpdate()		{};

	void					RecvUpdateIdx( UINT8 ui8Idx )
	{
		if( m_i32SendIdx == (INT32)ui8Idx )
		{
			m_ui32UpdateList[ 0 ] = m_ui32UpdateList[ m_i32RecvIdx ];
			m_i32RecvIdx	= 0;
			m_i32SendIdx	= 0;			
		}
		else
		{
			m_i32RecvIdx	= ui8Idx;
		}
	}

	UINT32					GetUpdateIdx()									{	return m_ui32UpdateList[ m_i32RecvIdx ];					}
	UINT8					SetUpdateIdx( UINT32 ui32Idx )
	{
		INT32 i32PlusIdx = 0;
		if( m_i32SendIdx < m_i32RecvIdx )
		{
			i32PlusIdx = MAX_PACKET_UPDATE_TIME;
		}

		// 링버퍼에 여유가 있을 경우
		if( ((m_i32SendIdx + i32PlusIdx) - m_i32RecvIdx) >= MAX_PACKET_UPDATE_TIME )
		{
			INT32 i32Idx = m_i32RecvIdx+1;
			if( i32Idx > MAX_PACKET_UPDATE_TIME )
			{
				m_i32SendIdx		= 0;
			}
			else
			{
				m_i32SendIdx		= (UINT8)i32Idx;
			}
		}
		
		m_ui32UpdateList[ m_i32SendIdx ] = ui32Idx;
		return (UINT8)m_i32SendIdx;
	}
};

class CAvailableWeapon
{
	UINT8				m_ui8Count;
	T_ItemID			m_TList[ WT_COUNT_MAX ];

public:
	CAvailableWeapon()		{};
	~CAvailableWeapon()		{};

	void					Reset()
	{
		m_ui8Count = 0;
	}
	BOOL					Insert( T_ItemID TItemID )
	{
		INT32 i32Idx = 0;
		for( ; i32Idx < m_ui8Count ; i32Idx++ )
		{
			// 이미 존재한다면 그냥 돌려보냅니다.
			if( TItemID == m_TList[ i32Idx ] )	return TRUE;
		}

		// 개수초과
		if( WT_COUNT_MAX == i32Idx )			return FALSE;

		m_TList[ i32Idx ] = TItemID;
		m_ui8Count++;

		return TRUE;
	}
	BOOL					IsUse( T_ItemID TItemID )
	{
		for( INT32 i = 0 ; i < m_ui8Count ; i++ )
		{
			if( TItemID == m_TList[ i ] )	return TRUE;
		}
		return FALSE;
	}
};

class CDediRoom: public i3ElementBase
{
	I3_CLASS_DEFINE( CDediRoom );

public:
	CStageInfoTimer*		getStageInfoTimer()			{ return m_pStageInfoTimer; }

	// HMS 접근 인터페이스 : hack check thread, task에서 접근
	CHMS*	getHMS()							{	return m_pHMS;	}
	
	INT16	GetGroupIndex()								{	return m_i16GroupIdx;				}
	void	SetGroupIndex( INT32 i32GroupIdx )			{	m_i16GroupIdx = (INT16)i32GroupIdx;	}

	// i32TaskIdx 대신 사용하기 위함
	INT32	GetTaskIndex()								{	return m_i32TaskIdx;				}
	void	SetTaskIndex( INT32	i32TaskIdx )			{	m_i32TaskIdx = i32TaskIdx;			}


	// 캐릭터 히트 파트
	CHARA_HIT_PART	NetIdxToHitPart( UINT8 ui8NetHitPart )	{
																if( ui8NetHitPart < CHARA_HIT_MAXCOUNT ) return (CHARA_HIT_PART) m_aui8HitPartIndex[ ui8NetHitPart ];	
																return CHARA_HIT_UNKNOWN;
															}

	UINT8	HitPartToNetIdx( CHARA_HIT_PART hitPart )		{
																for( UINT32 i = 0; i < CHARA_HIT_MAXCOUNT; i++ )
																	if( hitPart == (CHARA_HIT_PART) m_aui8HitPartIndex[ i ] ) return (UINT8)i;

																I3ASSERT(0 && "HitPartToNetIdx() hitPart Error" );
																return 0; // 여기에 들어오면 안됩니다.
															}

	UINT16			GetLastExplosionNetIdx( UINT32 ui32SlotIdx )	 {		
																if( ui32SlotIdx < SLOT_MAX_COUNT ) return m_aui16LastGrenadeIdx[ui32SlotIdx]; 
																I3ASSERT(0 && "GetLastExplosionNetIdx() SlotIdx Error" );
																return 0;
															}
	void			SetLastExplosionNetIdx( UINT32 ui32SlotIdx, UINT16 ui16NetIdx )	{	if( ui32SlotIdx < SLOT_MAX_COUNT ) m_aui16LastGrenadeIdx[ui32SlotIdx] = ui16NetIdx;	}

	// Fly 없는 수류탄 검사
	void			SetFly					( UINT32 ui32SlotIdx )	{ 
																		m_cs.Lock();
																		if (IS_VALID_MEMBER_RANGE(ui32SlotIdx)) m_ar32FlyBeginTime[ui32SlotIdx] = GetGameTime();
																		m_cs.Unlock();
																	}
	BOOL			IsFly					( UINT32 ui32SlotIdx )	{	
																		return (IS_VALID_MEMBER_RANGE(ui32SlotIdx)) ? ( GetGameTime() - m_ar32FlyBeginTime[ui32SlotIdx] < DS_FLYING_TIME) : FALSE; 
																	} 
	
	// ----------------------------------------------------------------------------------
	// 유저 정보
	// ----------------------------------------------------------------------------------
	UINT32  GetHelmetID		 ( UINT32 ui32SlotIdx ) { return (IS_VALID_MEMBER_RANGE(ui32SlotIdx))? m_pGameCharacters[ ui32SlotIdx ].GetParts( CHAR_EQUIPMENT_PARTS_HEAD )	: 0; }
	REAL32	GetDesigDFDRate  ( UINT32 ui32SlotIdx ) { return (IS_VALID_MEMBER_RANGE(ui32SlotIdx))? m_ar32DisigDFDRate[ ui32SlotIdx ]: 0.0f;}
	REAL32	GetDesigBombRange( UINT32 ui32SlotIdx ) { return (IS_VALID_MEMBER_RANGE(ui32SlotIdx))? m_ar32BombRangePlus[ui32SlotIdx] : 0.0f; }
	REAL32	GetDesigSpeed	 ( UINT32 ui32SlotIdx ) { return (IS_VALID_MEMBER_RANGE(ui32SlotIdx))? m_ar32DesignationSpeed[ui32SlotIdx] : 0.0f; }
	REAL32	GetFrontSpeed	 ( UINT32 ui32SlotIdx ) { return (IS_VALID_MEMBER_RANGE(ui32SlotIdx))? m_aCharaInfo[ ui32SlotIdx ].m_fFrontSpeed	: 0.0f; }
	EquipmentInfo*	GetHelmet( UINT32 ui32SlotIdx ) { return (IS_VALID_MEMBER_RANGE(ui32SlotIdx))? &m_aHelmet[ ui32SlotIdx ] : NULL; }
	// ----------------------------------------------------------------------------------
	// 캐릭터
	// ----------------------------------------------------------------------------------
	
	CCharacter*	GetGameCharacter( UINT32 ui32SlotIdx )		{	return (IS_VALID_MEMBER_RANGE(ui32SlotIdx)) ?  &m_pGameCharacters[ ui32SlotIdx ]  : NULL ; }
	
	BOOL				SetUserDeath( UINT32 ui32SlotIdx );
	
	// ----------------------------------------------------------------------------------
	// helper
	// ----------------------------------------------------------------------------------
	
	BOOL	IsRaptorBase		( UINT32 ui32SlotIdx );
	BOOL	IsElite				( UINT32 ui32SlotIdx );
	BOOL	IsAcid				( UINT32 ui32SlotIdx );
	BOOL	IsTRex				( UINT32 ui32SlotIdx );
	BOOL	IsEscapeNearKill	( UINT32 ui32DeathIdx);
	BOOL	IsDino				( UINT32 ui32SlotIdx )	{	return (IS_VALID_MEMBER_RANGE( ui32SlotIdx ))? ( GET_ITEM_TYPE( m_pGameCharacters[ ui32SlotIdx ].GetParts(CHAR_EQUIPMENT_PARTS_CHARA) ) == ITEM_TYPE_DINO ) : FALSE; }
	BOOL	IsMedicalKit		( UINT32 ui32Class)		{	return (ui32Class == WEAPON_CLASS_THROWING_HEAL); }
	BOOL	IsWPSmoke			( UINT32 ui32Class)		{	return (ui32Class == WEAPON_CLASS_THROWING_ATTACK); }

	// ----------------------------------------------------------------------------------
	// 미션전 Helper
	// ----------------------------------------------------------------------------------

	// 방어 미션에서 죽은 사람이 타겟을 공격했는지의 여부. hit 되면 무조건 3.0, game 시간 delta 만큼 빼줌.
	BOOL			IsAttackingMissionObj( UINT32 ui32SlotIdx )		{	return (IS_VALID_MEMBER_RANGE(ui32SlotIdx)) ? ( GetGameTime() - m_ar32AttackBeginTime[ ui32SlotIdx ] < DS_DESTROY_TARGET_LOCKTIME) : FALSE; } 
	
	// 하울링 공격을 당하고 있는지의 여부. howl 공격을 받으면 5.0초, game 시간 delta 만큼 빼줌.
	BOOL			IsDamageByHowl(UINT32 ui32SlotIdx)				{	return (IS_VALID_MEMBER_RANGE(ui32SlotIdx)) ? ( GetGameTime() - m_ar32HowlBeginTime[ ui32SlotIdx ] < DS_HOWL_LOCKTIME) : FALSE; }
	
	// 터치다운 성공후부터 3초동안은 데미지 적용 안됨.
	BOOL			IsTouchDown(UINT32 ui32SlotIdx)					{	return (IS_VALID_MEMBER_RANGE(ui32SlotIdx)) ? ( GetGameTime() - m_ar32TouchDownBeginTime[ui32SlotIdx] < DS_TOUCHDOWN_TIME) : FALSE; }

	// ----------------------------------------------------------------------------------
	// multicast control
	// ----------------------------------------------------------------------------------
	
	void			SendLife			( UINT32 ui32SlotIdx, UINT32 uInfoFlag );		// life packet을 수회 보낸다( suicide, hit by explosion ).
	
	void			_SetSendLife		( UINT32 ui32SlotIdx, UINT32 ui32Count )	{	
																		I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT ); 
																		if( ui32SlotIdx < SLOT_MAX_COUNT ) m_aui8SendLife[ ui32SlotIdx ] = (UINT8)ui32Count; 
																	}
	UINT32			_GetSendLife		( UINT32 ui32SlotIdx )		{
																		I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
																		if( ui32SlotIdx < SLOT_MAX_COUNT ) return m_aui8SendLife[ ui32SlotIdx ];
																		return 0;
																	}
	void			_DecSendLife		( UINT32 ui32SlotIdx )		{
																		I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
																		if( ui32SlotIdx < SLOT_MAX_COUNT )
																		{
																			if( --m_aui8SendLife[ ui32SlotIdx ] >=  DS_SEND_LIFE_COUNT )
																				m_aui8SendLife[ ui32SlotIdx ] = 0; // underflow
																		}
																	}
	void			_ResetSendLife		( UINT32 ui32SlotIdx )		{
																		I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
																		if( ui32SlotIdx < SLOT_MAX_COUNT )
																		{
																			m_aui8SendLife[ ui32SlotIdx ] = 0; 
																		}
																	}

	BOOL	IsUserExist( UINT32 ui32SlotIdx ) const	{	return (IS_VALID_MEMBER_RANGE(ui32SlotIdx))? GetBit( m_ui32CurrentUsers, ui32SlotIdx ) : FALSE; }
	
	// 난입한 유저에게만 StageInfo를 전송하기 위한 함수 
	// NextType과 NextIndex를 보고 계속 진행여부를 판단해야 함.
	void			SetIntrusion	( UINT32 ui32SlotIdx )			{	if( ui32SlotIdx < SLOT_MAX_COUNT) SetBit( &m_ui32IntrusionUsers, ui32SlotIdx);	} 
	BOOL			GetIntrusion	( UINT32 ui32SlotIdx ) const	{	if( ui32SlotIdx < SLOT_MAX_COUNT) return GetBit( m_ui32IntrusionUsers, ui32SlotIdx ); 
																		return FALSE;
																	}
	void			ResetIntrusion	( UINT32 ui32SlotIdx )			{	if( ui32SlotIdx < SLOT_MAX_COUNT) ClearBit( &m_ui32IntrusionUsers, ui32SlotIdx);	}
	
	// 첫번째 포지션을 받은 시점에서 Send가 가능하다 (2012-03-22)
	void			SetSendEnable	( UINT32 ui32SlotIdx )			{	if( ui32SlotIdx < SLOT_MAX_COUNT) SetBit	( &m_ui32FlagSendEnable,	ui32SlotIdx );	}
	void			ResetSendEnable	( UINT32 ui32SlotIdx )			{	if( ui32SlotIdx < SLOT_MAX_COUNT) ClearBit	( &m_ui32FlagSendEnable,	ui32SlotIdx	);	}
	BOOL			IsSendEnable	( UINT32 ui32SlotIdx ) const	{ 
																		if( ui32SlotIdx < SLOT_MAX_COUNT) return GetBit( m_ui32FlagSendEnable,  ui32SlotIdx );	
																		return FALSE;
																	}
	
	
	BOOL			CheckPacket( N_GAME_HEADER* pHeader, UINT32 ui32IP, UINT16 ui16Port );

	UINT32			MakeStageInfoPacket( char* pOutBuf, UINT32 ui32BufLen, /*IN/Out*/ UINT32* pui32NextType, /*IN/Out*/ INT32* pi32NextIndex, REAL32 r32GameTime ); 
	
	
	
	
	

	UINT8					CalcPathKey( CDSObject* pObj, BOOL bFinish );  // 헬기 예외처리를 위함(삭제)
	
	// ------------------------------------------------
	// object find functions
	// ------------------------------------------------
	CDSObject*	GetGameObject(INT32 i32NetIdx)				{
																if( i32NetIdx >= 0 && i32NetIdx < m_i32GameObjectCount ) return &m_aGameObjects[ i32NetIdx];
																return NULL;
															}

	// ------------------------------------------------
	// mission
	// ------------------------------------------------
	REAL32				m_r32BombInstallDelayTime;
	REAL32				m_r32BombUnInstallDelayTime;
	REAL32				m_r32BombExplosionDelayTime;


	REAL32				m_r32BombInstallTime;
	NET_MISSION_STATE	m_MissionState;
	BOMB_AREA			m_MissionArea;
	GAMEINFO_MISSION_INTRUDER	m_MissionIntruder;	// 난입 유저를 위해서 미션 정보 보관 (stageinfo로 전송)
											// TCP 전송때도 사용된다
	BOOL				m_bArmDone;			// 폭탄이 설치 되었을 때 true. (폭탄해제가 된후 설치 패킷(NET_MISSION_TRIGGER_ARM_DONE)이 날아오는 경우를 예외처리하기 위함)

	CAvailableWeapon		m_AvailableWeapon;

	// 헬기 애니메이션 싱크용
	UINT8				m_ui8CalcCount;			// 횟수에 따라 애니메이션 세트 결정

	UINT32	GetMyRoomStageID()	const	{ return m_ui32StageID; }

	// ------------------------------------------------
	// stage type ( STAGE MASK )
	// ------------------------------------------------

	MISSION_STATE	m_RoundMissionState;			// 현재 방의 미션 라운드 상태를 관리
	UINT16			m_TeamKillCount[TEAM_COUNT];	// 팀 별 Kill Count

	BOOL			(CDediRoom:: * RoundEndChk)();	// 라운드 종료 조건 검사 ( 모드별로 종료 조건이 다르기에 함수 포인터로 사용. 방 Create 시 설정 )

	BOOL			RoundEndChkAnnihilation();
	BOOL			RoundEndChkBomb();
	BOOL			RoundEndChkCrossCount();
	BOOL			RoundEndChkDeathMatch();
	BOOL			RoundEndChkDefence();
	BOOL			RoundEndChkDestroy();
	BOOL			RoundEndChkEscape();	
	BOOL			RoundEndChkTraining();
	BOOL			RoundEndChkTutorial();
	BOOL			RoundEndChkConvoy();
	BOOL			RoundEndChkRunAway();


	BOOL			IsTrainingRoom(void)			{ 	return ( StageID::GetAIMode( m_ui32StageID ) != AI_USE_NO );	}
	BOOL			IsSniperRoom(void)				{	return ( m_ui8AvailableWeapon & LOCK_SNIPER );			}
	BOOL			IsEscapeRoom(void)				{ 	return ( StageID::GetStageMode( m_ui32StageID ) == STAGE_MODE_ESCAPE );	}
	BOOL			IsShotGunRoom(void)				{	return ( m_ui8AvailableWeapon & LOCK_SHOTGUN );			}
	BOOL			IsknuckleRoom(void)				{	return ( m_ui8AvailableWeapon & LOCK_KNUCKLE );			}
	
	TEAM_TYPE		GetAllDeathTeam(void);

	UINT8			GetAvailableWeapon() const		{	return m_ui8AvailableWeapon;		}

	// ------------------------------------------------
	// Mode Check (STAGE ID)
	// ------------------------------------------------
	
	//KEYWORD : STAGE_MODE_ADD
	
	BOOL					IsDeathMode()			const			{	return ( m_stageType == STAGE_MODE_DEATHMATCH );	}
	BOOL					IsBombMissionMode()		const			{	return ( m_stageType == STAGE_MODE_BOMB );			}
	BOOL					IsDestroyMissionMode()	const			{	return ( m_stageType == STAGE_MODE_DESTROY );		}
	BOOL					IsAnnihilationMode()	const			{	return ( m_stageType == STAGE_MODE_ANNIHILATION );	}
	BOOL					IsEscapeMissionMode()	const			{ 	return ( m_stageType == STAGE_MODE_ESCAPE );		}
	BOOL					IsDefenceMissionMode()	const			{	return ( m_stageType == STAGE_MODE_DEFENCE );		}
	BOOL					IsCrossCountMode()		const			{	return ( m_stageType == STAGE_MODE_CROSSCOUNT );	}	
	BOOL					IsChaosMode()			const			
	{	
		return ( StageID::IsHudOff(m_ui32StageID) == true) &&
			(StageID::IsOneShotOneKill(m_ui32StageID) == true)
			;
	}
	BOOL					IsHeadHunterMode( CHARA_DEATH_TYPE eDeathType )		const			
	{	
		if ( StageID::IsHeadShotOnly( m_ui32StageID ) != true	)	return FALSE;
		if ( eDeathType		!= CHARA_DEATH_A )					return FALSE;
		return TRUE;
	}
	BOOL	IsConvoyMode()			const			{	return ( m_stageType == STAGE_MODE_CONVOY );	}
	BOOL	IsSpaceMode()			const			{	return StageID::IsSpaceMode(m_ui32StageID);			}

	STAGE_MODE		GetStageType(void) 		{ return m_stageType; }

	// 공수교대 여부에 따른 팀체크 
	TEAM_TYPE GetTeamType( UINT32 ui32SlotIdx )
	{
		return m_i32TeamChanged == ((INT32)ui32SlotIdx) % 2 ? TEAM_RED : TEAM_BLUE;
	}
	
	// 떨어진 무기에 대한 처리 여부 
	BOOL	IsDeletableDroppedWeapon()	const		{
														return (m_stageType == STAGE_MODE_DEATHMATCH	|| 
																m_stageType == STAGE_MODE_DESTROY		|| 
																m_stageType == STAGE_MODE_ESCAPE		|| 
																m_stageType == STAGE_MODE_DEFENCE ) ;
													}

	// Respawn 가능 Mode인지 여부
	BOOL	IsRespawnEnableMode()		const		{	
														return (m_stageType == STAGE_MODE_DEATHMATCH	|| 
																m_stageType == STAGE_MODE_DESTROY		|| 
																m_stageType == STAGE_MODE_DEFENCE		|| 																
																m_stageType == STAGE_MODE_ESCAPE		||
																m_stageType == STAGE_MODE_CROSSCOUNT);
													}

	//	Mission Object를 사용하는 Mode인지 여부
	BOOL	IsUsingMissionObject( )		const		{
														return (m_stageType == STAGE_MODE_DESTROY		|| 
																m_stageType == STAGE_MODE_ESCAPE		||
																m_stageType == STAGE_MODE_DEFENCE );
													}


	// ------------------------------------------------
	// Initialize functions
	// ------------------------------------------------
	
	BOOL				Create();							// alloc memory
	void				Reset();							// initialize for allocated memory
															// initialize group with the shared resource
	void				ResetFlagAll();						// reset hacking test flag for all users
	void				ResetFlag( UINT32 ui32SlotIdx );	// reset hacking test flag
	void				ResetFlagEscape( UINT32 ui32SlotIdx );	// reset hacking test flag


	// ------------------------------------------------
	// Task mananager interface functions (xxxx_T)
	// ------------------------------------------------

	void					UpdateReferenceTable( UINT32 ui32SlotIdx, UINT32 uInfoFlag, REAL32 r32PacketTime = 0 );	// hp 감소등 처리
	void					UpdateSyncObject	( REAL32 r32GameTime );		// 오브젝트 동기화
	void					SendLifeForSync();								// 추가적인 life 패킷 머지( suicide 처리)
	void					CheckClientTime		( REAL32 r32GameTime  );	// 클라이언트와 서버 사이의 시간 비교
	BOOL					CheckRespawnTime	( UINT32 ui32SlotIdx, UINT32 ui32SlotItem, REAL32 r32GameTime );
	BOOL					IsValidRespawnPos	( UINT32 ui32SlotIdx, UINT32 ui32TotalRespawnIdx );
	//void					CheckGMPausePos		( UINT32 ui32UserIdx );
	REAL32					m_r32LastCheckTime;

	INT32					GetPlayingUserCount(void);

private:
	
	void					ProcessHits(UINT32 ui32SlotIdx, GAMEINFO_HIT_BYTYPE eHitByType);

	REAL32					GetDistance(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex, N_POS* pPosition, N_PCINFO_HIT_DEFAULT* pHitDefault);

	// Result는 테스트 로그용 인자이므로 테스트 완료 후 삭제
	INT32					CalculateDamageCharacter(UINT32 ui32SlotIdx, REAL32 r32Distance, N_PCINFO_HIT_DEFAULT* pHitDefault, N_PCINFO_HIT_WEAPON* pWeapon, CHARA_DEATH_TYPE eDeathType, REAL32* pResult);
	INT32					CalculateDamageObject(UINT32 ui32SlotIdx, REAL32 r32Distance, N_PCINFO_HIT_DEFAULT* pHitDefault, N_PCINFO_HIT_WEAPON* pWeapon, REAL32* pResult);


//	void					_Process_HitInfo( UINT32 ui32SlotIdx, UINT32 uInfoFlag );
	void					_Process_Suicide( UINT32 ui32SlotIdx );
	void					_Process_Mission( UINT32 ui32SlotIdx, REAL32 r32PacketTime );
	INT32					_FindListIdx( UINT8 ui8Type, INT32 i32NetIdx );

	void					_Process_HitInfo_ByChara(UINT32 ui32SlotIdx, UINT32 ui32HitCount);
	void					_Process_HitInfo_ByExplosion(UINT32 ui32SlotIdx, UINT32 ui32HitCount);
	void					_Process_HitInfo_ByObject(UINT32 ui32SlotIdx, UINT32 ui32HitCount);

	void					RecalculateHitCountByCharacter(UINT32 ui32SlotIdx, INT32& i32HitCount);
	void					RecalculateHitCountByExplosion(UINT32 ui32SlotIdx, INT32& i32HitCount);

	void					ApplyCharacterDamage(UINT32 ui32SlotIdx, GAMEINFO_HIT_BYTYPE eHitterType, CHARA_DEATH_TYPE eDeathType, VEC3D v3dHitPos, N_PCINFO_HIT_DEFAULT* pHitDefault,INT32 i32DamageResult,N_PCINFO_HIT_WEAPON* pHitWeapon, T_ItemID TItemID );
	void					ApplyObjectDamage(UINT32 ui32SlotIdx, N_PCINFO_HIT_DEFAULT* pHitDefault,INT32 i32DamageResult, N_PCINFO_HIT_WEAPON* pHitWeapon = NULL);

	void					CheckHackByDeathInfo(UINT32 ui32SlotIdx, UINT32 ui32TargetIdx, N_PCINFO_HIT_DEDICATED2* pHitByChar);
	
	void					SetHeadHunterHitPartRate(CHARA_DEATH_TYPE eDeathType, N_PCINFO_HIT_DEFAULT* pHitDefault, REAL32* r32HitPartRate);
	void					SetChaosHitPartRate(REAL32* r32HitPartRate);

public:

	void					PacketDataReset( UINT32 ui32SlotIdx );
	// ------------------------------------------------
	// multicast
	// ------------------------------------------------
	void					SetChara_bWeaponSync( UINT32 ui32SlotIdx, BOOL bSync )						{	m_bCastCheck_WeaponSync[ ui32SlotIdx ] = bSync;															}
	void					SetChara_State( UINT32 ui32SlotIdx, N_PCINFO_CHARA_STATE* pData )			{	memcpy( &m_pCastPacket_Chara_State[ ui32SlotIdx ],			pData,	sizeof(N_PCINFO_CHARA_STATE) );		}
	void					SetChara_PosRot(UINT32 ui32SlotIdx, N_PCINFO_POSROTATION* pData )			{	memcpy( &m_pCastPacket_Chara_PosRot[ ui32SlotIdx ],			pData,	sizeof(N_PCINFO_POSROTATION) );		}
	void					SetChara_Weapon(UINT32 ui32SlotIdx, N_PCINFO_WEAPON* pData)					{	memcpy( &m_pCastPacket_Chara_Weapon[ ui32SlotIdx ],			pData,	sizeof(N_PCINFO_WEAPON) );			}

	void					SetChara_WeaponParam(UINT32 ui32SlotIdx, N_PCINFO_WEAPON_PARAM* pData )		{	memcpy( &m_pCastPacket_Chara_WeaponParam[ ui32SlotIdx ],	pData,	sizeof(N_PCINFO_WEAPON_PARAM) );	}

	void					SetChara_Hit_ByChara(UINT32 ui32SlotIdx, UINT8 ui8Count, N_PCINFO_HIT_DEDICATED2* pData )	
	{	
		m_pCastPacket_Chara_HitCount_ByChara[ ui32SlotIdx ]		= ui8Count;
		memcpy( m_pCastPacket_Chara_Hit_ByChara[ ui32SlotIdx ],		pData,	sizeof(N_PCINFO_HIT_DEDICATED2)*ui8Count );			
	}

	void					SetChara_Hit_ByExplosion(UINT32 ui32SlotIdx, UINT8 ui8Count, N_PCINFO_HIT_BYEXPLOSION* pData )
	{ 
		m_pCastPacket_Chara_HitCount_ByExplosion[ ui32SlotIdx ]	= ui8Count;
		memcpy( m_pCastPacket_Chara_Hit_ByExplosion[ ui32SlotIdx ],	pData,	sizeof(N_PCINFO_HIT_BYEXPLOSION)*ui8Count );
	}

	void					SetChara_Hit_ByObject(UINT32 ui32SlotIdx, UINT8 ui8Count, N_PCINFO_HIT_BYOBJECT* pData )
	{
		m_pCastPacket_Chara_HitCount_ByObject[ ui32SlotIdx ]	= ui8Count;
		memcpy( m_pCastPacket_Chara_Hit_ByObject[ ui32SlotIdx ],		pData,	sizeof(N_PCINFO_HIT_BYOBJECT)*ui8Count );
	}

	void					RecvUpdateIdx( UINT32 ui32SlotIdx, UINT8 ui8UpdateIdx )						{	m_PacketUpdate[ ui32SlotIdx ].RecvUpdateIdx( ui8UpdateIdx );		}
	UINT32					GetUpdateIdx( UINT32 ui32SlotIdx )											{	return m_PacketUpdate[ ui32SlotIdx ].GetUpdateIdx();				}
	UINT8					SetUpdateIdx( UINT32 ui32SlotIdx, UINT32 ui32Idx )							{	return m_PacketUpdate[ ui32SlotIdx ].SetUpdateIdx( ui32Idx );		}

	//void					SetGrenade(UINT32 ui32Idx, N_WEAPONINFO_THROW* pData )						{	memcpy( &m_pCastPacket_Grenade[ ui32Idx ],					pData,	sizeof(N_WEAPONINFO_THROW) );		}
	//void					SetCheckGrenade(UINT32 ui32Idx, BOOL bCheck )								{	m_pbCastCheck_Grenade[ ui32Idx ] = bCheck;																}

	CASTCHECK_USER*			getChara_Check(UINT32 ui32SlotIdx)					{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx)? &m_pCastCheck_Chara				[ ui32SlotIdx ] : NULL; }
	N_PCINFO_POSROTATION*	getChara_PosRot(UINT32 ui32SlotIdx)					{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx)? &m_pCastPacket_Chara_PosRot		[ ui32SlotIdx ] : NULL; }
	N_PCINFO_WEAPON*		getChara_Weapon(UINT32 ui32SlotIdx)					{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx)? &m_pCastPacket_Chara_Weapon		[ ui32SlotIdx ] : NULL; }
	N_PCINFO_LIFE*			getChara_Life(UINT32 ui32SlotIdx)					{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx)? &m_pCastPacket_Chara_Life			[ ui32SlotIdx ] : NULL; }

	//CDSObject*				getObj_StaticObject(UINT32 ui32NetIdx )				{ return IS_VALID_GAME_OBJECT(ui32NetIdx)?	&m_aGameObjects						[ ui32NetIdx ]	: NULL; } // type 확인용으로 사용합니다(barricade 예외처리용)
	
	GAMEINFO_MISSION_INTRUDER*		getMissionIntruder()								{ return &m_MissionIntruder;	}
	

	INT32*					getNextSend_Idx(UINT32 ui32SubHeadIdx)				{ return IS_VALID_SUBHEAD(ui32SubHeadIdx)?	&m_pNextSend_Idx				[ ui32SubHeadIdx ]	: NULL; }
	INT32*					getStageObj_Count(UINT32 ui32SubHeadIdx)			{ return IS_VALID_SUBHEAD(ui32SubHeadIdx)?	&m_pStageObj_Count				[ ui32SubHeadIdx ]	: NULL; }

	UINT8					GetRoundNumber()									{	return m_ui8RoundNumber;					}
	void					SetRoundNumber( UINT8 ui8RoundNumber );
	REAL32					GetGameTime()										{	return m_r32BattleTime;						}



	void					DeleteData	( INT32 i32GroupIdx );


	// ------------------------------------------------
	// control server interface functions (xxxx_C)
	// ------------------------------------------------
	
	UINT32				Register_C		( P_BATTLE_ROOM_CREATE_ACK* pInfo, const P_BATTLE_ROOM_CREATE_REQ* pReq, char* pExtInfo, UINT16 ui16ExtSize, INT32 i32SocketIdx );
	void				Revoke_C		( INT32 i32GroupIdx );
	BOOL				Enter_C			( P_BATTLE_USER_ENTER_REQ* pReq );
	BOOL				Withdraw_C		( UINT8 ui8SlotIdx );
	BOOL				Leave_C			( UINT32 ui32SlotIdx );
	void				SetRoundInfo_C	( P_BATTLE_ROUND_REQ* pRoundInfo );			// chara info
	BOOL				SetDinoInfo_C	( P_BATTLE_M_DINO_INFO_REQ* pDinoInfo );

	void				TouchDown_C		( UINT32 ui32SlotIdx );
	void				KickUserBattle	( INT32 i32WorkIdx, UINT32 ui32SlotIdx, INT64 i64UID );

	BOOL				SetUserEquip_C	( STRUCT_BATTLE_SLOT_EQUIPMENT_REQ* pEquipInfo ); // 야전상점에서 구입시 무기변경 알림
	BOOL				Respawn_C		( P_BATTLE_RESPAWN_REQ* pRespawn );	  // 리스폰 정보 설정
	void				SetQACommand_C	( P_BATTLE_QA_AUTO_REQ* pQAData );


	BOOL				UdpJoinGroup(  N_GAME_HEADER* pHeader, N_GAME_RELAY_HOLEPUNCH* pInfo, GAME_ROOM_OPTION* pOption, UINT32 ui32IP, UINT16 ui16Port );
	BOOL				Withdraw( UINT8 ui8SlotIdx );
	
	BOOL				SendRequestToControlServer();	// 컨트롤 서버에 전송				
	UINT32				m_ui32CurrentUsers;				// 초기화: 방을 만들 때 함께 시작한 유저. (난입 판단)
	UINT32				m_ui32FlagSendEnable;			// 포지션 정보를 보낸 유저. 패킷을 받을 준비가 되어 있다.
	UINT32				m_ui32IntrusionUsers;			// 난입유저
	UINT32				m_ui32BitHackOffUser;			// 핵감지 제외유저
	
	// ------------------------------------------------
	// relay functions
	// ------------------------------------------------
	BOOL				CheckMember( UINT8 cSlotIdx, UINT32 ui32IP, UINT16 ui16Port );

	BOOL				GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer );
	BOOL				CheckEncryptKey( UINT32 wEncryptKey );

	void				SetUdpRecvTime( )								{ ::InterlockedExchange((volatile LONG*)&m_ui32UdpRecvTime, i3ThreadTimer::GetServerTime()); }
	CDediMember*		GetMember( UINT32 ui32Idx )						{ return IS_VALID_MEMBER_RANGE(ui32Idx) ? &m_pDediMember[ui32Idx] : NULL; }
	ENUM_GROUP_STATE	GetGroupState()									{ return m_eState;				}
	void				SetGroupState( ENUM_GROUP_STATE	eState )		{ m_eState = eState;			}
	UINT32				GetUdpRecvTime()								{ return m_ui32UdpRecvTime;		}
	UINT32				GetEncryptKey()									{ return m_ui32EncryptKey;		}
	UINT16				GetPort()										{ return m_sPort;					}
	INT32				GetSocketIndex()								{ return m_i32SocketIdx;			}
	void				SetSocketIndex( INT32 i32SocketIdx )			{ m_i32SocketIdx = i32SocketIdx;	}	
	STAGE_ID			GetStageID()									{ return (STAGE_ID)m_ui32StageID;	}
	STAGE_UID			GetStageUID()									{ return StageID::GetStageUID( m_ui32StageID ); }
	BATTLE_ROOM_INFO&	GetRoomInfo()									{ return m_RoomInfo; }
	UINT64				GetUID( UINT32 ui32SlotIdx )					{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx) ? m_ai64UID[ui32SlotIdx] : 0; }
	UINT8				GetPBVer( UINT32 ui32SlotIdx )					{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx) ? m_aui8PBVer[ui32SlotIdx] : 0; }
	BOOL				IsInBattle()									{ return RGS_USE == m_eState; }
	CRespawnState*		GetRespawnState()								{ return &m_respawnState; }
	UINT32				GetRespawnCount( UINT32 ui32SlotIdx )			{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx) ? m_aui32RespawnCount[ui32SlotIdx] : (UINT32)-1;	}
	UINT32				GetTouchDownCount( UINT32 ui32SlotIdx )			{ return IS_VALID_MEMBER_RANGE(ui32SlotIdx) ? m_aui32TouchDownCount[ui32SlotIdx] : (UINT32)-1;	}
	
	// wall shot
	void				WriteWallShotResult( DS_CHECK_WALLSHOT_RESULT* pResult );
	
	
	BOOL				_IsHackUserInBattle( UINT8 ui8ByteUID, UINT32 ui32SlotIdx, UINT8 ui8RoundNum, UINT8 ui8respawnCount )
																		{	return ( ui8ByteUID == (UINT8)(GetUID(ui32SlotIdx) & 0xff) && ui8RoundNum == GetRoundNumber() && ui8respawnCount == GetRespawnCount( ui32SlotIdx ) ); }

public:
	CDediRoom();
	virtual ~CDediRoom();

private:

	// ------------------------------------------------
	// objects initialize (internal functions)
	// ------------------------------------------------
	BOOL	_InitGameObjects();

	BOOL	_ResetRespawnObjects();
	BOOL	_ResetMisssionObjects();								// destroy/defence
	void	_ResetThrownWeaponInfo( const UINT32 ui32SlotIdx );
	// ------------------------------------------------
	// Death Info
	// ------------------------------------------------
	BOOL	_BuildDeathInfoByHit(GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEFAULT* pHit, CHARA_DEATH_TYPE deathType, N_PCINFO_HIT_WEAPON* pWeapon, T_ItemID TItemID, INT32 AssistIdx);	// 총기 (폭발에 의한 사망일 경우에는 pWeapon이 null이 아님)
	BOOL	_BuildDeathInfoForPilot (GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo,	UINT32 ui32SlotIdx, UINT32 ui32Pilot, N_PCINFO_HIT_WEAPON* pWeapon = NULL );	// 아웃포스트 헬기 조종사를 죽이기 위한 예외처리
	BOOL	_BuildDeathInfoByObjectExplosion(GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo,	UINT32 ui32SlotIdx, N_PCINFO_HIT_WEAPON* pWeapon = NULL);	// 총기 (폭발에 의한 사망일 경우에는 pWeapon이 null이 아님)

	void	_Write_Chara_Death		( GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo);
	void	_QueDeathChara(UINT32 ui32SlotIdx, UINT8 ui8DeathWeapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx, INT32 AssistIdx);
	
	BOOL	_EnqueueRaycastRequest	( UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pFinalHit);  // send msg to map
	// ------------------------------------------------
	// grenade 
	// ------------------------------------------------
	

public:
	UINT32					GetServerWeaponID( UINT32 ui32SlotIdx, UINT32 ui32Usage )
	{
		if( SLOT_MAX_COUNT <= ui32SlotIdx )		return 0;
		return GetGameCharacter( ui32SlotIdx )->GetEquipWeaponID( static_cast<UINT8>(ui32Usage) );
	}

	DS_THROWN_GRENADE*		GetThrownWeaponInfo( UINT32 ui32NetSlotIdx ) {	return ( ui32NetSlotIdx < MAX_THROWWEAPON_COUNT )? &m_aThrownGrenade[ ui32NetSlotIdx ] : NULL; }
	
	//////////////////////////////////////////////////////////////////////////
	// Calculate damage
	//////////////////////////////////////////////////////////////////////////	
	CHARA_STATE_FOR_CAM	_GetCharaStateForCam  ( UINT32 ui32State );

	void	ApplySkills				( UINT32 ui32SlotIdx, CHARA_SLOT_SKILL* pSkills);
	BOOL	ApplyEquipment			( UINT32 ui32SlotIdx, UINT32 ui32CharaID, UINT32 ui32HelmetID, UINT32 ui32BeretID );
	void	ApplyDesignation		( UINT32 ui32SlotIdx, UINT8* pui8DesignationInfo );
	
	BOOL	_CheckEnableHelicopterExplosion(UINT32 ui32SlotIdx , UINT16 ui16ObjectIdx);
	BOOL	_CheckObjectExplosionRange(UINT32 ui32ObjectIdx , UINT32 ui32HitSlotIdx);
	BOOL	_CheckHeadShotHack		( UINT32 ui32SlotIdx, N_PCINFO_HIT_DEFAULT* pHitDefault );
	BOOL	_CheckGravityHack		( UINT32 ui32SlotIdx, N_POS_R* pKillerPos ); // called by task thread
	
	BOOL	CheckTouchDown			( UINT32 ui32SlotIdx, UINT16 ui32ObjectIdx );
	void	DeathObjectExplosion	( UINT32 ui32KillSlotIdx, UINT32 ui32DeathSlotIdx,UINT32 ui32ObjectIdx, N_PCINFO_HIT_WEAPON* pWeapon );
	void	EnqueueKickUser			( UINT32 ui32SlotIdx, DS_HACK_TYPE HackType ); // task thread에서 해킹으로 판단한 경우 (wall shot만)

	
	void	ReportUDPVersionError	( UINT32 ui32SlotIdx  );
private:
	
	
	// ------------------------------------------------
	// room status
	// ------------------------------------------------
	DS_BROADCASTINFOEX		m_sBroadCastInfo;
	ENUM_GROUP_STATE		m_eState;
	BATTLE_ROOM_INFO		m_RoomInfo;							// roomInfo (for game server)
	CDediMember				m_pDediMember[ SLOT_MAX_COUNT ];	// user status
	UINT32					m_ui32EncryptKey;					// random #
	UINT32					m_ui32UdpRecvTime;					// last packet recv time
	UINT16					m_sPort;							// server port 
	INT32					m_i32SocketIdx;						// global socket index
	UINT32					m_ui32StageID;						// stage id
	STAGE_MODE				m_stageType;						// GET_STAGETYPE( m_ui32StageID) 1번만 계산
	UINT8					m_ui8RoomSubType;					// 방 생성 시 설정된 종료 조건 Time, Kill
	UINT8					m_ui8InfoFlag;						// 방 생성 시 설정된 방 설정 정보
	bool					m_bCanSoloPlay;						// 솔로 플레이 가능 여부 (QA 자동화)
	UINT8					m_ui8RoundWinTeam;					// 라운드 승리 팀
	UINT8					m_ui8RoundEndType;					// 라운드 종료 원인
	REAL32					m_r32LastSentTimeToControlServer;	// 파괴, 방어미션 타겟 hit를 1초에 1번이상 보내지 않기 위함 	
	UINT8					m_ui8RoundNumber;					// UDP 패킷 전송시 사용됨
	INT16					m_i16GroupIdx;
	UINT8					m_ui8AvailableWeapon;				// 배틀 중 사용가능한 무기	
	INT32					m_i32TaskIdx;						// my task thread index
	INT32					m_i32TeamChanged;					// 공수교대 여부,	0: 교대안함, 1: 교대됨
	UINT8					m_ui8MaxPerson;

	UINT8					m_aui8HitPartIndex[CHARA_HIT_MAXCOUNT];	 // mapping table for shuffed parts
public:
	REAL32					m_r32RoundStartTime;

	void					InitBroadCastInfo();
	DS_BROADCASTINFOEX *	GetBroadCastInfo()				{ return &m_sBroadCastInfo; }

	UINT8					GetRoundEndType()				{ return m_ui8RoundEndType; }
	UINT8					GetRoundWinTeam()				{ return m_ui8RoundWinTeam; }

private:
	// ----------------------------------------------------------------------------------
	// game status (initialized from CMapData )
	//	BATTLE을 감시하기 위한 참조 데이터
	// -----------------------------------------------------------------------------------

	// game character 
	//	- hp, position 관리
	//	- ResetSyncData(), ResetUserHP() 순서로 초기화 된다
	//	- hit 패킷, suicide dmg 패킷에 damage에 의해서 hp가 감소되며 0일 경우 게임서버로 death 패킷을 전송.
	
	CCharacter				m_pGameCharacters	[ SLOT_MAX_COUNT ];	
	KILLING_INFO			m_pKillInfo			[ SLOT_MAX_COUNT ];		// kill 정보를 게임서버에 전송하기 위한 버퍼
	ASSIST_INFO				m_AssistInfo[SLOT_MAX_COUNT];

	REAL32					m_ar32IntrusionTimes[ SLOT_MAX_COUNT ];		// stage info를 보내기 위한 시간 (플래그로도 사용)
	REAL32					m_ar32LastDeathTimes[ SLOT_MAX_COUNT ];		// 캐릭터가 마지막 죽은 시간
	
	UINT16					m_aui16LastGrenadeIdx[ SLOT_MAX_COUNT ];		 // hit by explosion을 1번만 적용하기 위함 : 라운드시작, 난입시에 초기화
	DS_THROWN_GRENADE		m_aThrownGrenade	[ MAX_THROWWEAPON_COUNT ];	// 스테이지에 던져진 무기 정보
	REAL32					m_ar32FlyBeginTime	[ SLOT_MAX_COUNT ];			// 수류탄을 던진 시간(투척시 5초에서 줄임, 
																			//  Hit By Explosion 발생시 시간이 셋팅되어 있지 않으면 해킹(아는 무기만 체크)

	UINT8					m_aui8SendLife		[ SLOT_MAX_COUNT ];			// suicide 처리 후 캐릭터가 살아 있는 경우 life를 수회 보냄

public: // 데디에서 사용되는 많은 변수들이 Get, Set 으로 결국 Public 처럼 전부 사용되는데, Private 로 할 필요가 있을까?? 구조를 바꾸지 않는이상 결국 함수 호출로 인한 낭비만 되는것이 아닌가??
	REAL32					m_r32LastWeaponSyncTime[ SLOT_MAX_COUNT ];		// 마지막으로 무기 Sync 패킷을 보낸 시간 ( 일정 시간동안 전송이 안되었으면 보내도록 한다. )
	
	BOOL					GetUserAlive( UINT32 ui32SlotIdx )		{ if( ui32SlotIdx >= SLOT_MAX_COUNT ) return FALSE; return m_pGameCharacters[ ui32SlotIdx ].GetAlive(); }
	BOOL					GetUserOnLoad( UINT32 ui32SlotIdx )		{ if( ui32SlotIdx >= SLOT_MAX_COUNT ) return TRUE;	return m_pGameCharacters[ ui32SlotIdx ].GetOnLoad();}
	
	// ----------------------------------------------------------------------------------
	// hack report
	// ----------------------------------------------------------------------------------
	
	void					ResetHackType( UINT32 ui32SlotIdx) 
	{
		I3ASSERT(ui32SlotIdx < SLOT_MAX_COUNT);
		if( ui32SlotIdx < SLOT_MAX_COUNT)
			m_aHackType		[ ui32SlotIdx ] = HACK_TYPE_NO;
	}

	void					SetHackType( UINT32 ui32SlotIdx, DS_HACK_TYPE type ) {
																					I3ASSERT(ui32SlotIdx < SLOT_MAX_COUNT);
																					I3ASSERT( type < HACK_TYPE_MAX );
																					
																					// 두번 셋팅되지 않음
																					if( ui32SlotIdx < SLOT_MAX_COUNT && m_aHackType [ ui32SlotIdx ] == HACK_TYPE_NO )
																					{	
																						m_aHackType		[ ui32SlotIdx ] = type;
																					}
																				 }

	DS_HACK_TYPE			GetHackType( UINT32 ui32SlotIdx ) const				{
																					I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
																					if( ui32SlotIdx < SLOT_MAX_COUNT ) return m_aHackType[ ui32SlotIdx ];
																					return HACK_TYPE_NO;
																				}
	BOOL					IsHackUser( UINT32 ui32SlotIdx ) const				{	return ( GetHackType( ui32SlotIdx ) != HACK_TYPE_NO );		}

	BOOL					CheckValidItem( UINT32 ui32SlotIdx )				{		
																					ui32SlotIdx &= (UINT32)0x0f;
																					return GetBit( m_ui32FlagValidItem, ui32SlotIdx);
																				}
	
	BOOL					GetHackingReportSent( UINT32 ui32SlotIdx ) const	{	
																					I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
																					if( ui32SlotIdx < SLOT_MAX_COUNT ) return GetBit( m_ui32FlagHackingReportSent, ui32SlotIdx ); 
																					return FALSE;
																				}
	void					SetHackingReportState( UINT32 ui32SlotIdx, BOOL bValue){	
																					I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
																					if( ui32SlotIdx < SLOT_MAX_COUNT )
																					{
																						if( bValue ) SetBit  ( &m_ui32FlagHackingReportSent, ui32SlotIdx ); 
																						else		 ClearBit( &m_ui32FlagHackingReportSent, ui32SlotIdx );
																					}
																				}

	void					CheckPosPacketTime( INT32 i32WorkIdx )					{
																							for( INT32 i = 0; i < SLOT_MAX_COUNT; ++i )
																							{
																								if( !GetUserAlive( i ) )					 continue;
																								if( m_bKickUserBattle[i] )					 continue;
																								if( GetGameTime() < 10.0f )					 continue;

																								if( 5.0f <= GetGameTime() - m_r32PosCheckTime[i] )
																								{
																									KickUserBattle( i32WorkIdx, i , GetUID( i ) );
																								}
																							}
																					}
	void					SetPosPacketTime( UINT32 ui32SlotIdx )					{
																						I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
																						if( ui32SlotIdx < SLOT_MAX_COUNT )
																						{
																							m_r32PosCheckTime[ ui32SlotIdx ] = GetGameTime();
																						}
																					}

private:
	DS_HACK_TYPE			m_aHackType			[ SLOT_MAX_COUNT];		// Last Hacking (round 시작시 초기화)
	
	
	UINT32					m_ui32FlagHackingReportSent;				// 게임서버로 두번 보내지 않기 위한 플래그
	UINT32					m_ui32FlagSendHackingResport;				// 게임서버로 

	// 리스폰 포지션을 검사하기 위한 변수들
	CRespawnState			m_respawnState;								// 리스폰 핵 검사
	UINT32					m_ui32FlagChkRespawnPos;					// 버퍼링된 데이터가 N개 이상이면 on. 리스폰 포지션 검사후 off.(respawn pos 제어)

	UINT32					m_aui32RespawnIdx	[ SLOT_MAX_COUNT ];		// respawn 패킷에서 받은 값

	UINT32					m_aui32RespawnCount	[ SLOT_MAX_COUNT ];		// 유저별 Respawn 횟수
	UINT32					m_aui32TouchDownCount[ SLOT_MAX_COUNT ];	// 유저별 TouchDown 횟수

	REAL32					m_ar32InvincibleTime[ SLOT_MAX_COUNT ];		// 유저별 무적 유지 시간( 게임시간 + 무적시간 더한 값 )	
																		
	REAL32					m_ar32AttackBeginTime	[ SLOT_MAX_COUNT ];		// 방어미션에서 공격자가 타겟을 공격한 시간. hit시 3.0(define)으로 설정, update시  delta 만큼 빼줌.
																		// IsAttackingMissionObj() 에서 사용함
	REAL32					m_ar32HowlBeginTime		[ SLOT_MAX_COUNT ];		// 공력이 Howl 공격을 하는 캐릭터. hit시 5.0으로 설정, update시  delta 만큼 빼줌.
																		// IsDamageByHowl() 에서 사용
	REAL32					m_ar32TouchDownBeginTime[ SLOT_MAX_COUNT ];		// 터치다운 성공시 3초로 셋팅. 3초동안은 데미지 적용안됨.
		
	// 폭파미션 해킹을 막기 위해 추가 - 서동권
	REAL32					m_r32TriggerTime		[ SLOT_MAX_COUNT ]; // C4 설치 or 해제 시작 시간

	// 2초이상 Pos 패킷이 안왔는지 체크 - 서동권
	REAL32					m_r32PosCheckTime		[ SLOT_MAX_COUNT ]; // POS 시간 주기 확인
	BOOL					m_bKickUserBattle		[ SLOT_MAX_COUNT ]; // 레디룸으로 강퇴 요청을 하였는지
	REAL32					m_r32OnLoadPacketTime	[ SLOT_MAX_COUNT ]; // 

	// 스피드 해킹을 검사하기 위해 추가 -  서동권
	BOOL					m_bCheckOnLoad			[ SLOT_MAX_COUNT ]; // 온로드 상태 저장
	N_POS					m_vPosOldSpeed			[ SLOT_MAX_COUNT ]; // 이전 Pos
	REAL32					m_ar32TimeOldSpeed		[ SLOT_MAX_COUNT ]; // 이전 Pos Time
	REAL32					m_ar32SpeedBonus		[ SLOT_MAX_COUNT ]; // 스피드 증가치 - Item , 호칭 등으로 인한 스피드 보너스
	UINT32					m_ui32OldState			[ SLOT_MAX_COUNT ];
	REAL32					m_ar32TimeStore			[ SLOT_MAX_COUNT ];

	// 케릭터 정보는 death시 리셋 되기 때문에 그룹 멤버로 보관한다

	// -------------------------------------------------------------
	// 유저 정보
	// -------------------------------------------------------------
	
	REAL32					m_ar32DisigDFDRate	[ SLOT_MAX_COUNT ];		// DFD Rate by Designation
	REAL32					m_ar32BombRangePlus	[ SLOT_MAX_COUNT ];		// Bome Range Plus by Designation
	REAL32					m_ar32DesignationSpeed[SLOT_MAX_COUNT];		// Bonus Speed by Designation
			
	CharacterInfo			m_aCharaInfo		[ SLOT_MAX_COUNT ];
	EquipmentInfo			m_aHelmet			[ SLOT_MAX_COUNT ];
	EquipmentInfo			m_aBeret			[ SLOT_MAX_COUNT ];
	UINT32					m_ui32FlagValidItem;	// 정보가 없으면 패킷을 전송하지 않음.
	BOOL					m_bAI				[ SLOT_MAX_COUNT ];

	UINT64					m_ai64UID			[ SLOT_MAX_COUNT ];
	UINT8					m_aui8PBVer			[ SLOT_MAX_COUNT ];
	
	// -------------------------------------------------------------------------------------------
	// Game Objects
	// -------------------------------------------------------------------------------------------

	// ------------------------------------------------------------------------------------
	// game objects 
	INT32					m_i32GameObjectCount;									// # of objects
	CDSObject				m_aGameObjects[ DS_MAX_OBJECT ];						// 예외 type, hp, net sync type, net index, pos
	
	INT32					m_i32GameObjectGroupCount;
	CDSObjectGroup			m_pGameObjectGroup[ DS_MAX_OBJECT ];

	// Battle Use Item 
	CDSObjectUseItemManager	m_pObjectUseItemMgr;

	// ------------------------------------------------------------------------------------
	// Weapon Box Sync:  m_pGameObjects를 참조한다
	INT32					m_i32ControlledObjectCount;								// # of controlled objects

	// ------------------------------------------------------------------------------------
	// 파괴 미션, 방어 미션 :  m_pGameObjects를 참조한다
	DS_MISSION_DATA_DESTRUCTION	m_DestructionData;						// mission destruction
	DS_MISSION_DATA_DEFENCE		m_DefenceData;							// mission defence
	
	CDSObject*				m_pConvoyObject;
	CDSObject*				m_pRepairTrain;

	INT32					m_i32RunAwayObjectCount;
	CDSObject*				m_pRunAwayObject[ RUN_AWAY_OBJECT_COUNT ];
	// ------------------------------------------------------------------------------------
	// ANIMATION SYNC OBJECTS : m_pGameObjects를 참조한다
	INT32					m_i32AnimationObjectCount;					// # of objects

	// -------------------------------------------------------------
	// stage monitors
	// -------------------------------------------------------------

	CDroppedWeaponMgr*		m_pDroppedWeaponMgr;						// world에 떨어진 무기 관리
	CThrowWeaponMgr*		m_pThrowWeaponMgr;

	CHMS*					m_pHMS;										// hacking monitor
	CRespawnMgr*			m_pRespawnMgr;								// 리스폰 관리자
	
public:
	CCharacterCastPacket		m_CharCastPacket[ SLOT_MAX_COUNT ];
	CCharacterCastPacket*		GetCharCastPacket( UINT32 ui32SlotIdx )			{	return &m_CharCastPacket[ ui32SlotIdx ];		}
	
private :

	friend class UdpCsBuilder;
	friend class HMSParser;
	friend class UDPParser;
	friend class UDPChecker;

	// --------------------------------------------------------------
	// SYNC Data : Multicast 용도
	// --------------------------------------------------------------



	// ------------------------------------------------------------------------------------
	// 게임 캐릭터 정보 버퍼
	BOOL						m_bCastCheck_WeaponSync					[ SLOT_MAX_COUNT ]; // 유저 무기 정보 싱크 여부 ( TRUE 일 경우 전체 유저의 무기 정보를 보낸다. )
	CASTCHECK_USER				m_pCastCheck_Chara						[ SLOT_MAX_COUNT ];
	N_PCINFO_CHARA_STATE		m_pCastPacket_Chara_State				[ SLOT_MAX_COUNT ];
	N_PCINFO_POSROTATION		m_pCastPacket_Chara_PosRot				[ SLOT_MAX_COUNT ];
	N_PCINFO_WEAPON				m_pCastPacket_Chara_Weapon				[ SLOT_MAX_COUNT ];
	N_PCINFO_WEAPON_PARAM		m_pCastPacket_Chara_WeaponParam			[ SLOT_MAX_COUNT ];
	N_PCINFO_LIFE				m_pCastPacket_Chara_Life				[ SLOT_MAX_COUNT ];
	N_PCINFO_COMMAND_MOVE		m_pCastPacket_Chara_CommandMove			[ SLOT_MAX_COUNT ];
	
	UINT8						m_pCastPacket_Chara_HitCount_ByChara	[ SLOT_MAX_COUNT ];		
	UINT8						m_pCastPacket_Chara_HitCount_ByExplosion[ SLOT_MAX_COUNT ];		
	UINT8						m_pCastPacket_Chara_HitCount_ByObject	[ SLOT_MAX_COUNT ];
	N_PCINFO_HIT_DEDICATED2		m_pCastPacket_Chara_Hit_ByChara			[ SLOT_MAX_COUNT ][MAX_FIREHIT_COUNT];			
	N_PCINFO_HIT_BYEXPLOSION	m_pCastPacket_Chara_Hit_ByExplosion		[ SLOT_MAX_COUNT ][MAX_FIREHIT_COUNT];
	N_PCINFO_HIT_BYOBJECT		m_pCastPacket_Chara_Hit_ByObject		[ SLOT_MAX_COUNT ][MAX_FIREHIT_COUNT];

	N_PCINFO_SUFFER				m_pCastPacket_Chara_Suffering			[ SLOT_MAX_COUNT ];
	N_PCINFO_DEATH				m_pCastPacket_Chara_Death				[ SLOT_MAX_COUNT ];

	CPacketUpdate				m_PacketUpdate							[ SLOT_MAX_COUNT ];

	CDSObjectManager			m_ObjectManager;
	
	// ------------------------------------------------------------------------------------
	// 수류탄	
	//BOOL					m_pbCastCheck_Grenade			[MAX_THROWWEAPON_COUNT];
	//N_WEAPONINFO_THROW		m_pCastPacket_Grenade			[MAX_THROWWEAPON_COUNT];
	// ------------------------------------------------------------------------------------
	// Static Object (HP만 변경되는 오브젝트 이동하지 않음)	
	UINT16					m_pNetIndex_Obj_Static			[OBJ_STATIC_COUNT];// stage info 전송시 검색을 피하기 위함
	INT32					m_i32CountOfStatic;
	// ------------------------------------------------------------------------------------
	// Animation Object	
	UINT16					m_pNetIndex_Obj_Anim			[OBJ_ANIM_COUNT];
	INT32					m_i32CountOfAnim;
	// ------------------------------------------------------------------------------------
	// Dropped weapon 
	//BOOL					m_pbCastCheck_DroppedWeapon		[MAX_DROPEDWEAPON_COUNT];
	//N_DROPEDWEAPONINFO		m_pCastPacket_DroppedWeapon		[MAX_DROPEDWEAPON_COUNT];
	// ------------------------------------------------------------------------------------
	// Controlled Object : Weapon Box 처리
	UINT16					m_pNetIndex_Obj_Controlled		[OBJ_CONTROLED_COUNT];
	INT32					m_i32CountOfControlled;
	// ------------------------------------------------------------------------------------

	UINT16					m_pNetIndex_Obj_Move			[OBJ_MOVE_COUNT];

	// 다음 전송 인덱스 : 한 패킷으로 보낼 수 있는 데이터 리미트 때문에 마지막 전송한 인덱스를 보관
	INT32					m_pNextSend_Idx [P2P_SUB_HEAD_COUNT]; 
	INT32					m_pStageObj_Count[P2P_SUB_HEAD_COUNT];		// (난입) 멀티 캐스트할 오브젝트 개수
	CStageInfoTimer*		m_pStageInfoTimer;							// 분할된 stage info를 효율적으로 보내기 위한 타이머
		
public:
	void					ResetSyncData_C();						// 라운드가 재시작할 때 오브젝트를 기본 값 리셋
	void					ResetRespawnHelicopter();				// 리스폰 헬리콥터 애니메이션 설정 (라운드 넘버가 셋팅된 이후에 호출해야 한다)
	
	void					ResetUserHP(UINT32 * pui32DinoInfo = NULL);	// 라운드가 재시작할 때 캐릭터 HP를 리커버
	void					ResetPlayDataOnce( UINT32 ui32SlotIdx );	// 난입, 이탈시 플레이 데이터 리셋

	// --------------------------------------------------------------
	// GM Pause 기능
	// --------------------------------------------------------------
	INT32					GMPause_C();
	void					GMResume_C();

private:

	// ----------------------------------------------------------------------------------------------
	// 마지막 전송 게임타임 : 시간에 따라 동기화가 필요한 오브젝트용
	//	최초 UDP 홀펀칭시 초기화 되고, 멀티 캐스트 보낼 때마다 갱신된다.
	// ----------------------------------------------------------------------------------------------
	REAL32					m_pr32LastPacketTime		  [SLOT_MAX_COUNT ];		// 클라이언트가 전송한 시간이기 때문에
																					// 패킷 검사용으로만 사용한다
	REAL32					m_ar32OffsetTime			  [SLOT_MAX_COUNT];			// 클라이언트와 서버의 최초 시간 차이 
																					// 홀펀칭시 초기화 되며, 가끔씩 
																					// ( 패킷 시간-서버시간-Offset > 50.0f)이면 해킹으로 판정

	// --------------------------------------------------------------
	// 시간 단위 SYNC 함수들
	// --------------------------------------------------------------
	
	void					_Sync_Obj_Intrusion( REAL32 r32GameTime );
	void					_Sync_Obj_Anim( REAL32 r32GameTime );
	//void					_Sync_Obj_Controlled( REAL32 r32GameTime );
	void					_Sync_Obj_RespawnHelicopter( REAL32 r32GameTime, CDSObject*	pObject );

	
	i3Timer					m_Timer;
	REAL32					m_r32BattleTime;
	REAL32					m_r32StartTime;											// 게임 시작 시간 (sec)
	UINT32					m_i32DropWeaponIndex;									// 드롭 웨폰 인덱스

	// ----------------------------------------------------------------------------------------------
	// 컨트롤 서버에 보고하기 위한 내부 함수들 ( SendRequestToControlServer가 호출 )
	// ----------------------------------------------------------------------------------------------
	
	BOOL					_SendControl_KillDeathInfo	();	
	void					_SendControl_MissionTrigger	(); 
	BOOL					_SendControl_Destruction	();
	BOOL					_SendControl_Defence		();
	BOOL					_SendControl_HackReport		();
	
	UINT32					m_ui32TcpSendFlag;										// 게임서버에 send 해야 하는 정보들을 setting

	BOOL					IsThrowWeapon( UINT32 ui32SlotIdx, UINT16 ui16Idx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow, N_PCINFO_HIT_RAYCHECK* pRay )
	{
		BOOL bFirstShot;
		if( FALSE == m_pThrowWeaponMgr->IsThrow( ui16Idx, pThrow, bFirstShot, pRay ) )							return FALSE;
		switch( pThrow->GetWeapon()->getWeaponClass() )
		{
		case WEAPON_CLASS_THROWING_GRENADE:
		case WEAPON_CLASS_THROWING_CONTAIN:
			if( bFirstShot )
			{
				INT32 i32ExtType = 0;
				if( FALSE == m_pGameCharacters[ ui32SlotIdx ].IsDecreaseBullet( pThrow->GetWeapon(), 1,i32ExtType) )		return FALSE;
			}
			break;
		}
		return TRUE;
	}

	BOOL					AddThrowWeapon( UINT32 ui32SlotIdx, UINT16 ui16Idx, GAMEINFO_THROW_WEAPON_NETDATA* pThrow )
	{
		BOOL bFirstShot;
		if( FALSE == m_pThrowWeaponMgr->AddThrow( ui32SlotIdx, ui16Idx, pThrow, bFirstShot ) )				return FALSE;
		switch( pThrow->GetWeapon()->getWeaponClass() )
		{
		case WEAPON_CLASS_THROWING_GRENADE:
		case WEAPON_CLASS_THROWING_CONTAIN:
			if( bFirstShot )
			{
				if( FALSE == m_pGameCharacters[ ui32SlotIdx ].DecreaseBullet( pThrow->GetWeapon(), 1 ) )		return FALSE;
			}
			break;
		}
		return TRUE;
	}
	void					SetMultiWeaponSlot();
	void					SetSingleWeaponSlot();
	// Drop Weapon
	INT32					GetDropWeaponIdx( REAL32 r32GameTime )
	{
		return m_pDroppedWeaponMgr->GetDropWeaponIdx( r32GameTime );
	}	
	BOOL					AddDropWeapon( INT32 i32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon, REAL32 r32GameTime, INT32 i32DropIndex )
	{	
		return m_pDroppedWeaponMgr->AddDropWeapon( i32SlotIdx, pWeapon, r32GameTime, i32DropIndex );
	}
	BOOL					UpdateDropWeapon( UINT32 ui32SlotIdx, UINT32 ui32Idx, GAMEINFO_DROPEDWEAPON_NETDATA* pData )
	{
		return m_pDroppedWeaponMgr->UpdateDropWeapon( ui32SlotIdx, ui32Idx, pData );
	}
	BOOL					GetDropWeapon( GAMEINFO_GETWEAPON* pWeapon, REAL32 r32GameTime, GAMEINFO_BULLET* pBullet )
	{
		return m_pDroppedWeaponMgr->GetDropWeapon( pWeapon, r32GameTime, pBullet );
	}
	BOOL					IsGetDropWeapon( UINT32 ui32Idx, T_ItemID TItemID, REAL32 r32GameTime )
	{
		return m_pDroppedWeaponMgr->IsGetWeapon( ui32Idx, TItemID, r32GameTime );
	}
	BOOL					IsGetDropWeaponPos( UINT32 ui32Idx, VEC3D* pPos )
	{
		return m_pDroppedWeaponMgr->IsGetWeaponPos( ui32Idx, pPos );
	}
	// --------------------------------------------------------------
	// critical section
	// --------------------------------------------------------------
	
	S2CriticalSection		m_cs;

	// --------------------------------------------------------------
	// 샷건 거리별 힛트횟수를 세팅하기 위한 함수 
	// --------------------------------------------------------------
	INT32				CalcShotgunHitCount(REAL32 r32Distance);
	UINT32				getShotGunDistanceLv(REAL32 r32Distance);
	// --------------------------------------------------------------
	// Hack Log
	// --------------------------------------------------------------
	BOOL   isHackUserForLog(UINT32 ui32SlotIdx,DS_HACK_TYPE Type);
	void   SetHackUserForLog(UINT32 ui32SlotIdx,DS_HACK_TYPE Type);
	void   ResetHackFlagForLog(UINT32 ui32SlotIdx);
	// --------------------------------------------------------------
	//	SpeedHack Log
	// --------------------------------------------------------------
	UINT32 m_ui32SpeedHackFlag;
	// --------------------------------------------------------------
	//	FireSpeed Log
	// --------------------------------------------------------------
	UINT32 m_ui32FireSpeedHackFlag;

	BOOL isLogLevel(UINT8 i8Level){
		if (i8Level>10)
			return TRUE;

		return FALSE;
	}

	// --------------------------------------------------------------
	// GM 정지 기능 (대회용)
	// --------------------------------------------------------------
	BOOL				m_bGMPause;
	//UINT32				m_ui32PauseMember;
	//REAL32				m_ar32PausePosX[SLOT_MAX_COUNT];
	//REAL32				m_ar32PausePosZ[SLOT_MAX_COUNT];
	//UINT32				m_aui32RecvOnPause[SLOT_MAX_COUNT];

public:

	void				UpdateBattleTime()	{	m_bGMPause ? m_Timer.GetDeltaSec() : m_r32BattleTime += m_Timer.GetDeltaSec(); }
	BOOL				IsGMPause()			{	return m_bGMPause; }
	//void				CheckGMPauseShoot( UINT32 ui32SlotIdx );

	BOOL				IsAI( UINT32 ui32Slot )		{	return m_bAI[ ui32Slot ];			}
	// --------------------------------------------------------------
	// 헬기 폭파 싱크
	// --------------------------------------------------------------
	BOOL				isHelicopter(UINT16 ui16Type)	
	{
		if(ui16Type == OBJECT_S_TYPE_RESPAWN_HELICOPTER || ui16Type== OBJECT_S_TYPE_RIDING_HELICOPTER)return TRUE;
		else return FALSE;
	}

	void				ProcessHelicopterExplosion(UINT32 ui32SlotIdx,UINT32 ui32TargetIdx,N_PCINFO_HIT_WEAPON* pHitWeapon);

	BOOL					IsAvailableWeapon( T_ItemID TItemID )	{	return m_AvailableWeapon.IsUse( TItemID );		}
	// --------------------------------------------------------------
	// QA Command 
	// --------------------------------------------------------------

	BOOL				CheckUseWeapon( UINT32 ui32SlotIdx, T_ItemID TItemID );

	void				QA_Command_Damage_Object(UINT8 ui8SlotIdx, INT32 ui32ObjIdx, INT32 i32Damage);
	void				QA_Command_KillCount(UINT8 ui8SlotIdx,INT32 i32KillCnt);
	void				QA_Command_Move_User(INT32 ui32SlotIdx,VEC3D *pPos);

	INT32					GetHitCount(GAMEINFO_HIT_BYTYPE eByHitType, UINT32 ui32SlotIdx);
	N_PCINFO_HIT_DEFAULT*	GetHitDefault(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex);
	N_PCINFO_HIT_WEAPON*	GetHitWeapon(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex);
	CHARA_DEATH_TYPE		GetDeathType(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex);
	VEC3D					GetHitDirection(GAMEINFO_HIT_BYTYPE eHitBytype, INT32 ui32SlotIdx, INT32 i32HitIndex);

	BOOL					IsInvincibleTime( UINT32 ui32SlotIdx );
	// --------------------------------------------------------------
	//	치팅탐지 확률 조정을 위한 구조체
	// --------------------------------------------------------------
	DetectRate				m_sDetectRateFoByPass[SLOT_MAX_COUNT];
	DetectRate				*getDetectRate(UINT32 ui32SlotIndex)	{return &m_sDetectRateFoByPass[ui32SlotIndex];}	
	// --------------------------------------------------------------
	//	QA 커맨드사용 유저
	// --------------------------------------------------------------
		BOOL				IsQaCommandCheatUser(INT32 ui32SlotIdx){ return GetBit(m_ui32BitHackOffUser,ui32SlotIdx);}
	// --------------------------------------------------------------
	//	중력핵 감지
	// --------------------------------------------------------------
private:
	UINT32 m_ui32GravityFlag;

public:
	void	_CheckGravityMove( UINT32 ui32SlotIdx, N_POS* pUserPos );

	bool	IsMaxDepth(COctreeNode *pNode);
	//레이가 통과한 노드 검색 후 컬리젼 체크
	bool	SearchRaycastOnNode(VEC3D *pStart, VEC3D *pEnd, VEC3D *pDir,RaycastResult *pRay);

	BOOL	DoRayCast(RaycastResult *pOut, N_POS* pPos) ;

		// UDP 패킷 체크 및 해킹 체크 루틴
public:

	PACKET_CHECK			ParseCharaActionKey( UINT32 ui32SlotIdx, GAMEINFO_ACTIONKEY* pActionKey );
	PACKET_CHECK			ParseGrenadePacket( UINT32 ui32SlotIdx, UINT16 ui16GrenadeIdx, GAMEINFO_THROW_WEAPON_NETDATA* pThorwWeapon );


};
#endif
