#ifndef __SERVERDEF_H
#define	__SERVERDEF_H

#include "../../CommonServerSource/DestroyReason.h"

#include "DSObjectManager.h"
#include "DroppedWeaponMgr.h"
#include "ThrowWeaponMgr.h"

////////////////////////////////////////////////
// DEDICATION SERVER 컴파일 스위치 
#define SAMPLES_USE_VRD			1		// physX 디버거 사용/사용안함
typedef	void*	NxPtr;					// PhysX 2.8.4에서 define. 현재 버전은 2.8.1 사용

/////////////////////////////////////////////////////////

#define SERVER_MAIN_UPDATE_TIME		1000

#ifdef I3_DEBUG 
#define MAX_TRACK_BUFFER_COUNT		5000
#else
#define MAX_TRACK_BUFFER_COUNT		10000
#endif

#ifdef I3_DEBUG 
#define MAX_UDP_BUFFER_COUNT		20000
#else
#define MAX_UDP_BUFFER_COUNT		100000
#endif


#define	SERVER_STRING_COUNT			512

#define SERVER_UPDATE_MAIN			0x00000001
#define SERVER_UPDATE_LOGFILE		0x00000002

//#define SERVER_ERROR_NETWORK					0X80000001
//#define SERVER_ERROR_CONFIG						0X80000002
//#define SERVER_ERROR_LOGFILE					0X80000003
//#define SERVER_ERROR_SYSTEMLOG					0x80000004
//#define SERVER_ERROR_CONTROLSERVER				0x80000005
//#define SERVER_ERROR_UDPMODULE					0x80000006
//#define SERVER_ERROR_GROUPMAKER					0x80000007
//#define SERVER_ERROR_LOG						0x80000008
//#define SERVER_ERROR_IOCPSERVER					0x80000009 
//#define SERVER_ERROR_TASKMANAGER				0x8000000a
//#define SERVER_ERROR_STATISTICS					0x8000000b
//#define SERVER_ERROR_RESOURCE					0x8000000c
//#define SERVER_ERROR_PHYSICS					0x8000000d
//#define SERVER_ERROR_CONTROL					0X8000000F
//#define SERVER_ERROR_HACK_CHECKER				0X80000010
//#define SERVER_ERROR_XMLFILELOADING				0X80000011
//#define SERVER_ERROR_XMLFILENATIONALCODE		0x80000012
//#define SERVER_ERROR_DIFFERENT_BUILD_VERSION	0x80000013
//#define SERVER_ERROR_INSUFFICIENT_MEMORY		0x80000014

#define MAIN_THREAD_IDX					-100	
//#define MODULE_ZP_THREAD_IDX			-200		//zepetto에서 만든 모듈 Auth, Clan, Message, Relay, Control
#define MODULE_CONTROL_THREAD_IDX		-201		//Control
#define MODULE_CAST_THREAD_IDX			-202		//Trans
#define MODULE_NC_THREAD_IDX			-300		//NC에서 만든 모듈 GIP, LOG_D
#define MODULE_DATABASE_THREAD_IDX		-400		//독립 DataBase 쓰레드 Update부분을 저장합니다.
#define MODULE_USER_THREAD_IDX			-500		// DB 
#define MODULE_GAMEGUARD_THREAD_IDX		-600		// GameGuard 
#define MODULE_HACKCHECK_THREAD_IDX		-700		// Hack Check Thread
#define MODULE_PHYSICS_THREAD_IDX		-800		// Physics
#define MODULE_CAST_THREAD_IDX			-900		// Cast

//서동권 
enum SERVER_STATE
{
	SERVER_STATE_NONE		= 0,
	SERVER_STATE_LOADING,
	SERVER_STATE_INIT,
	SERVER_STATE_INITING,
	SERVER_STATE_START,
};

inline char* GetServerStateString( INT32 i32State )
{
	switch( i32State )
	{
	case SERVER_STATE_NONE:
		return "SERVER_STATE_NONE(0)";
	case SERVER_STATE_LOADING:
		return "SERVER_STATE_LOADING(1)";
	case SERVER_STATE_INIT:
		return "SERVER_STATE_INIT(2)";
	case SERVER_STATE_INITING:
		return "SERVER_STATE_INITING(3)";
	case SERVER_STATE_START:
		return "SERVER_STATE_START(4)";
	default:
		return "SERVER_STATE_UNKNWON:check!";
	}
}


// global variable
extern HANDLE				g_hevtQuit;
extern SERVER_STATE			g_eServerState;
extern char					g_pCCurrentPath[SERVER_STRING_COUNT]; 

// service interface
INT32						g_InitServer(void); 
INT32						g_StartServer(void);
void						g_UpdateServer(void); 
void						g_DestroyServer(INT32 i32DestroyReason);
void						g_SetCurrentPath(LPCTSTR pPath);
BOOL						g_CheckSystemMemory();

#define INFINIY_HP		0xFFFF

// 스킬 시스템 카테고리 리스트
enum SKILL_CARTEGORY
{
	SKILL_NONE	= -1,

	SKILL_TYPE_COMMON,
	SKILL_TYPE_MAIN,
	SKILL_TYPE_ASSIST,

	SKILL_CARTEGORY_MAX
};

struct CCharacterCastPacket
{
	GAMEINFO_TAKING_OBJECT			m_TakingObject;
	GAMEINFO_ACTIONKEY				m_ActionKey;
	GAMEINFO_SYNC_OBJECT			m_SyncObject;
	GAMEINFO_RADIO_CHAT				m_RadioChat;

	GAMEINFO_SUICIDEDAMAGE			m_SuicideDamage;
	GAMEINFO_MISSION				m_Mission;
	GAMEINFO_DROPWEAPON				m_DropWeapon;
	GAMEINFO_GETWEAPON				m_GetWeapon;
	GAMEINFO_DIRECTPICKUP			m_DirectPickUp;
	GAMEINFO_FIRE					m_Fire;
	GAMEINFO_THROW_WEAPON_NETDATA	m_ThorwWeapon;
	
	void					Reset()
	{
		m_TakingObject.Reset();
		m_ActionKey.Reset();
		m_SyncObject.Reset();
		m_RadioChat.Reset();
		m_SuicideDamage.Reset();
		m_Mission.Reset();
		m_DropWeapon.Reset();
		m_GetWeapon.Reset();
		m_Fire.Reset();
		m_ThorwWeapon.Reset();
		m_DirectPickUp.Reset();
	}
	void					ResetData()
	{	// 매 Tick ( Send 한번 보낼때 ) 마다 리셋해줘야 하는 변수입니다.
		m_ActionKey.Reset();		
	}
};

struct DS_BROADCASTINFOEX
{
	REAL32					m_r32GameTime;					// 게임 시작부터 현재까지 시간(sec)
	UINT32					m_ui32StageID;					// For stage type
	UINT8					m_ui8RoundNum;
	
	CCharacterCastPacket*	m_pCastPacket;
	CDroppedWeaponMgr*		m_pDropPacket;
	CThrowWeaponMgr*		m_pThrowPacket;

	// 게임 캐릭터 정보 버퍼
	BOOL*					m_pCastCheck_bWeaponSync		;
	CASTCHECK_USER*			m_pCastCheck_Chara				;

	N_PCINFO_CHARA_STATE*	m_pCastPacket_Chara_State		;
	N_PCINFO_POSROTATION*	m_pCastPacket_Chara_PosRot		;
	//UINT8*					m_ui8CastPacket_Chara_ActionKeyCount;
	//N_PCINFO_ACTIONKEY		(*m_pCastPacket_Chara_ActionKey)[ MAX_ACTION_COUNT ];
	//N_PCINFO_ONLOADOBJECT*	m_pCastPacket_Chara_OnLoadObject;
	//N_PCINFO_SYNCOBJECT*	m_pCastPacket_Chara_SyncObject	;
	//N_PCINFO_RADIOCHAT*		m_pCastPacket_Chara_Radio		;
	N_PCINFO_WEAPON*		m_pCastPacket_Chara_Weapon		;
	N_PCINFO_WEAPON_PARAM*	m_pCastPacket_Chara_WeaponParam	;
	N_PCINFO_LIFE*			m_pCastPacket_Chara_Life		;
	//N_PCINFO_BULLET*		m_pCastPacket_Chara_Bullet		;
	N_PCINFO_COMMAND_MOVE*	m_pCastPacket_Chara_CommandMove	;

	INT32*					m_pCastPacket_Chara_HitCount	;		// 히트 카운트
	N_PCINFO_HIT			(*m_ppCastPacket_Chara_Hit)[MAX_FIREHIT_COUNT];

	N_PCINFO_DEF2			m_pCastPacket_Chara_Def2		;				// 새로운 Hit용으로 32bit 플레그 데이타
	UINT8*					m_pCastPacket_Chara_HitCount_ByChara;		
	UINT8*					m_pCastPacket_Chara_HitCount_ByExplosion;	
	UINT8*					m_pCastPacket_Chara_HitCount_ByObject;

	N_PCINFO_HIT_DEDICATED2 (*m_ppCastPacket_Chara_Hit_ByChara)[MAX_FIREHIT_COUNT];
	N_PCINFO_HIT_BYEXPLOSION (*m_ppCastPacket_Chara_Hit_ByExplosion)[MAX_FIREHIT_COUNT];
	N_PCINFO_HIT_BYOBJECT	(*m_ppCastPacket_Chara_Hit_ByObject)[MAX_FIREHIT_COUNT];

	N_PCINFO_SUFFER*		m_pCastPacket_Chara_Suffering	;		// 피격입니다.
	N_PCINFO_DEATH*			m_pCastPacket_Chara_Death		;
	//INT32*					m_pCastPacket_Chara_SuicideCount;
	//N_PCINFO_SUICIDEDMG		(*m_ppCastPacket_Chara_Suicide)[MAX_SUICIDE_COUNT];
	//N_PCINFO_MISSION*		m_pCastPacket_Chara_Mission		;
	//N_PCINFO_DROPWEAPON* 	m_pCastPacket_Chara_DropW		;
	//N_PCINFO_GETWEAPON*		m_pCastPacket_Chara_GetW		;
	
	// 게임 오브젝트 정보 버퍼
	// 수류탄	
	//BOOL*					m_pbCastCheck_Grenade			;
	//N_WEAPONINFO_THROW*		m_pCastPacket_Grenade			;
	// Static Object (HP만 변경되는 오브젝트 이동하지 않음)	
	UINT16*					m_pNetIndex_Obj_Static			;
	INT32*					m_pi32GameObjectCount			;
	CDSObject*				m_pGameObjects					; // barricade 타입 확인용
	// Animation Object	
	UINT16*					m_pNetIndex_Obj_Anim			;
	// Dropped weapon	
	BOOL*					m_pbCastCheck_DroppedWeapon		;
	//N_DROPEDWEAPONINFO*		m_pCastPacket_DroppedWeapon		;
	//Controlled Object
	//BOOL*					m_pbCastCheck_Obj_Controlled	;

	UINT16*					m_pNetIndex_Obj_Move			;

	UINT16*					m_pNetIndex_Obj_Controlled		;
	//MISSION_INTRUDER
	GAMEINFO_MISSION_INTRUDER*		m_pMissionIntruder;		// 폭파 모드시 스테이지 정보를 만들기 위해 사용
	
	
	// 다음 전송 인덱스 : 한 패킷으로 보낼 수 있는 데이터 리미트 때문에 마지막 전송한 인덱스를 보관
	INT32					m_i32NextSendIdx;	// next index 배열 : index = sub head type

	INT32*					m_pNextSend_Idx;	// next index 배열 : index = sub head type
	INT32*					m_pStageObj_Count;	// count 배열 : index = sub head type
	BOOL					m_bOnMulticast;		// 모든 싱크 정보를 전송한 후에 끔
	
	CDSObjectManager*		m_pObjectManager;

	// multicast 하지 않도록 수정 : 난입한 유저에게만 stage info 전송
	//INT32					m_nStageInfoSendCount;

	UINT8					m_ui8PacketUpdateIdx;
	// 전송 시간

	//WATCH_PARAM*			m_pWatchParam;
	
	void SettingBasicInfo( REAL32 r32GameTime, UINT32 ui32RoundNumber )
	{		
		m_r32GameTime		= r32GameTime;
		m_ui8RoundNum		= ui32RoundNumber;
	}
} ;

struct RECV_PACKET_SUB_INFO
{
	N_GAME_SUBHEAD*					m_pSubHead;
	
	//CCharacterCastPacket			m_CastPacket;

	N_PCINFO_DEF2*					m_pChara_Def;
	N_PCINFO_CHARA_STATE*			m_pChara_State;
	GAMEINFO_TAKING_OBJECT			m_TakingObject;					//N_PCINFO_ONLOADOBJECT*		m_pChara_OnLoadObject;
	N_PCINFO_POSROTATION*			m_pChara_PosRot;	
	GAMEINFO_ACTIONKEY				m_ActionKey;					//N_PCINFO_ACTIONKEY*			m_pChara_ActionKey;
	GAMEINFO_SYNC_OBJECT			m_SyncObject;					//N_PCINFO_SYNCOBJECT*			m_pChara_SyncObject;
	GAMEINFO_RADIO_CHAT				m_RadioChat;					//N_PCINFO_RADIOCHAT*			m_pChara_Radio;
	N_PCINFO_WEAPON_PARAM*			m_pChara_WeaponParam;
	GAMEINFO_SUICIDEDAMAGE			m_SuicideDamage;				//UINT8*						m_pui8Chara_SuicideCount;
																	//N_PCINFO_SUICIDEDMG*			m_pChara_Suicide;
	GAMEINFO_MISSION				m_Mission;						//N_PCINFO_MISSION*				m_pChara_Mission;
	GAMEINFO_DROPWEAPON				m_DropWeapon;					//N_PCINFO_DROPWEAPON* 			m_pChara_DropW;
	GAMEINFO_GETWEAPON				m_GetWeapon;					//N_PCINFO_GETWEAPON*			m_pChara_GetW;
	GAMEINFO_FIRE					m_Fire;							//N_PCINFO_BULLET*				m_pChara_Bullet;
	GAMEINFO_DIRECTPICKUP			m_DirectPickUp;					// DirectPickUp
	//GAMEINFO_HIT					m_Hit;

	UINT8*							m_pui8Chara_Hit_ByChara;
	N_PCINFO_HIT_DEDICATED2*		m_pChara_Hit_ByChara;
	UINT8*							m_pui8Chara_Hit_ByExplosion;
	N_PCINFO_HIT_BYEXPLOSION*		m_pChara_Hit_ByExplosion;
	UINT8*							m_pui8Chara_Hit_ByObject;
	N_PCINFO_HIT_BYOBJECT*			m_pChara_Hit_ByObject;
	GAMEINFO_THROW_WEAPON_NETDATA	m_ThorwWeapon;					//N_WEAPONINFO_THROW*				m_pGrenade;
	GAMEINFO_DROPEDWEAPON_NETDATA	m_DroppedWeapon;				//N_DROPEDWEAPONINFO*				m_pDroppedWeapon;
	GAMEINFO_TOUCHDOWN				m_TouchDown;					//UINT16*							m_pui16TouchDown;
};

struct RECV_PACKET_INFO
{
	INT32						m_i32EventCount;
	N_GAME_HEADER*				m_pHeader;

	RECV_PACKET_SUB_INFO		m_SubInfo[ 64 ];
};
#endif
