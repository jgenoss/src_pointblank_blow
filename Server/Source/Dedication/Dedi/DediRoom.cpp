// DediRoom.cpp
//
// YouJong Ha
//	Last update : 2010-10-01 (yyyy:mm:dd)
//	
// Description:
//	DediGroup 클래스 구현파일.
//
// Dependency: 
//		
//	
#include "pch.h"

#include "WeaponTable.h"
#include "CharacterTable.h"
#include "EquipmentTable.h"
#include "hms.h"

#include "DediMember.h"

#include "MapManager.h"
#include "MapData.h"
#include "ModuleControl.h"
#include "ModuleCast.h"

#include "DediUdpBuilder.h"
#include "DediRoom.h"

#include "iocpWorkerImpl.h"
#include "IocpServer.h"
#include "UdpBufferPool.h"

#include "PacketLocker.h"
#include "StageInfoTimer.h"
#include "DroppedWeaponMgr.h"

#include "RespawnMgr.h"
#include "HmsParser.h"

I3_CLASS_INSTANCE( CDediRoom, i3ElementBase);

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
#define  ADDITIONAL_RANGE 1.5f
#define  ADDITIONAL_RANGE_DINO 5.0f

UINT32 ShotGunHit[HIT_DISTANCE_MAX] ={8,   7,   6,   5,   4,   3,   2};

CDediRoom::CDediRoom()
{ 
	m_pStageInfoTimer	= new CStageInfoTimer;
	m_pDroppedWeaponMgr = new CDroppedWeaponMgr;
	m_pThrowWeaponMgr	= new CThrowWeaponMgr;
	m_pHMS				= new CHMS;
	m_pRespawnMgr		= new CRespawnMgr;
	InitBroadCastInfo();
} 

void CDediRoom::InitBroadCastInfo()
{
	m_sBroadCastInfo.m_pCastPacket								= m_CharCastPacket;
	m_sBroadCastInfo.m_pDropPacket								= m_pDroppedWeaponMgr;
	m_sBroadCastInfo.m_pThrowPacket								= m_pThrowWeaponMgr;

	m_sBroadCastInfo.m_ui32StageID								= m_ui32StageID;
	// 게임 캐릭터 정보 버퍼
	m_sBroadCastInfo.m_pCastCheck_bWeaponSync					= m_bCastCheck_WeaponSync;
	m_sBroadCastInfo.m_pCastCheck_Chara							= m_pCastCheck_Chara;
	m_sBroadCastInfo.m_pCastPacket_Chara_State					= m_pCastPacket_Chara_State;
	m_sBroadCastInfo.m_pCastPacket_Chara_PosRot					= m_pCastPacket_Chara_PosRot;
	m_sBroadCastInfo.m_pCastPacket_Chara_Weapon					= m_pCastPacket_Chara_Weapon;
	m_sBroadCastInfo.m_pCastPacket_Chara_WeaponParam			= m_pCastPacket_Chara_WeaponParam;
	m_sBroadCastInfo.m_pCastPacket_Chara_Life					= m_pCastPacket_Chara_Life;
	m_sBroadCastInfo.m_pCastPacket_Chara_CommandMove			= m_pCastPacket_Chara_CommandMove;

	m_sBroadCastInfo.m_pCastPacket_Chara_HitCount_ByChara		= m_pCastPacket_Chara_HitCount_ByChara;
	m_sBroadCastInfo.m_pCastPacket_Chara_HitCount_ByExplosion	= m_pCastPacket_Chara_HitCount_ByExplosion;
	m_sBroadCastInfo.m_pCastPacket_Chara_HitCount_ByObject		= m_pCastPacket_Chara_HitCount_ByObject;

	m_sBroadCastInfo.m_ppCastPacket_Chara_Hit_ByChara			= m_pCastPacket_Chara_Hit_ByChara;
	m_sBroadCastInfo.m_ppCastPacket_Chara_Hit_ByExplosion		= m_pCastPacket_Chara_Hit_ByExplosion;
	m_sBroadCastInfo.m_ppCastPacket_Chara_Hit_ByObject			= m_pCastPacket_Chara_Hit_ByObject;

	m_sBroadCastInfo.m_pCastPacket_Chara_Suffering				= m_pCastPacket_Chara_Suffering;		// 피격입니다.
	m_sBroadCastInfo.m_pCastPacket_Chara_Death					= m_pCastPacket_Chara_Death;

	// 게임 오브젝트
	m_sBroadCastInfo.m_pNetIndex_Obj_Static						= m_pNetIndex_Obj_Static;
	m_sBroadCastInfo.m_pNetIndex_Obj_Move						= m_pNetIndex_Obj_Move;
	m_sBroadCastInfo.m_pi32GameObjectCount						= &m_i32GameObjectCount;
	m_sBroadCastInfo.m_pGameObjects								= m_aGameObjects; // 참조용
	m_sBroadCastInfo.m_pNetIndex_Obj_Anim						= m_pNetIndex_Obj_Anim;
	//m_sBroadCastInfo.m_pbCastCheck_DroppedWeapon				= m_pbCastCheck_DroppedWeapon;
	
	m_sBroadCastInfo.m_pNetIndex_Obj_Controlled					= m_pNetIndex_Obj_Controlled;
	m_sBroadCastInfo.m_pMissionIntruder							= &m_MissionIntruder;
	// 인덱스
	m_sBroadCastInfo.m_pNextSend_Idx							= m_pNextSend_Idx;
	// 패킷의 남는 공간에 stageinfo를 채우기 위함
	m_sBroadCastInfo.m_pStageObj_Count							= m_pStageObj_Count;
	m_sBroadCastInfo.m_pObjectManager							= &m_ObjectManager;
}

void CDediRoom::ResetFlagAll()
{
	m_ui32FlagChkRespawnPos		= 0;
	m_ui32GravityFlag			= 0;
	m_ui32SpeedHackFlag			= 0;
	m_ui32FireSpeedHackFlag     = 0;
	m_respawnState.Reset();
}

void CDediRoom::ResetFlagEscape( UINT32 ui32SlotIdx )
{
	if( ui32SlotIdx < SLOT_MAX_COUNT )
	{
		ClearBit( &m_ui32FlagChkRespawnPos		, ui32SlotIdx );
		ClearBit( &m_ui32GravityFlag			, ui32SlotIdx );
	}
}

void CDediRoom::ResetFlag( UINT32 ui32SlotIdx )
{
	if( ui32SlotIdx < SLOT_MAX_COUNT )
	{
		ClearBit( &m_ui32FlagChkRespawnPos		, ui32SlotIdx );
		ClearBit( &m_ui32GravityFlag			, ui32SlotIdx );
		m_respawnState.ResetPos( ui32SlotIdx );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: Reset
//
// Description:
//	- 방생성시에만 호출 (참고로, reset sync data는 round 시작할 때마다 동작) 
//  - 라운드 번호라든가 게임 시간 등 한번만 초기화 되어야 하는 변수들을 초기화한다.
void CDediRoom::Reset()
{
	ResetFlagAll();	// 라운드 정보를 받기 전에 핵체킹 기능이 동작하지 않게 하기 위함.
	
	// ------------------------------------------------
	// ROOM STATUS
	// ------------------------------------------------
	m_eState			= RGS_INVALID;
	m_RoomInfo.Reset();
	
	m_ui32EncryptKey	= RELAY_ENCRYPTKEY_DEFAULT;
	m_i32SocketIdx		= -1;
	m_r32StartTime		= 0.0f;
	m_Timer.GetDeltaSec();
	m_r32BattleTime		= 0.0f;

	m_r32BombInstallDelayTime	= BOME_INTSALL_MIN_TIME;
	m_r32BombUnInstallDelayTime	= BOME_UINSTALL_MIN_TIME;
	m_r32BombExplosionDelayTime	= C4_BOMB_TIME;

	// ------------------------------------------------
	// CONTROL SERVER
	// ------------------------------------------------
	m_ui32TcpSendFlag	= 0;
	m_ui32UdpRecvTime	= 0;
	
	// ------------------------------------------------
	// STAGE
	// ------------------------------------------------
	m_ui32StageID		= StageID::MakeStageID( STAGE_MODE_NA, WEAPON_ALL, AI_USE_NO, false, false );					// stage ord는 1부터 시작 (STAGE_UID_NONE == 0)
	m_stageType			= STAGE_MODE_NA;	
	m_ui8MaxPerson		= 16;
	m_ui8RoundNumber	= INVALID_ROUND_NUMBER;
	m_RoundMissionState	= MISSION_STATE_UNKNOWN;
	I3TRACE( "Reset Round\n" );

	// 리스폰 헬기
	m_ui8CalcCount		= 0;
	
	// 드롭 웨폰 슬롯
	m_i32DropWeaponIndex = 0;

	// 미션 데이터
	// : null로 설정되기 때문에 방 생성시에만 호출되어야 한다
	m_DestructionData.m_pObjectBlue = NULL;
	m_DestructionData.m_pObjectRed  = NULL;
	m_DefenceData.setDefenceObj(0, NULL);
	m_DefenceData.setDefenceObj(1, NULL);

	// ------------------------------------------------
	// TIME
	// ------------------------------------------------
	m_r32LastSentTimeToControlServer	= 0.0f;
	m_r32BombInstallTime				= 0.0f;

	// ------------------------------------------------
	// COUNTER
	// ------------------------------------------------
	
	// reference object counters
	m_i32GameObjectCount			= 0;
	m_i32AnimationObjectCount		= 0;
	m_i32ControlledObjectCount		= 0;

	m_i32GameObjectGroupCount		= 0;

	// sync object counters
	m_i32CountOfStatic				= 0;
	m_i32CountOfAnim				= 0;
	m_i32CountOfControlled			= 0;
	
	// stage object counters
	i3mem::FillZero( m_pStageObj_Count, sizeof(m_pStageObj_Count) );
	i3mem::FillZero( m_bAI, sizeof(m_bAI) );
	
	m_ui8AvailableWeapon = 0;
	m_ui32BitHackOffUser = 0;

	m_ui32GravityFlag	 = 0;
	m_ui32SpeedHackFlag			= 0;
	m_ui32FireSpeedHackFlag     = 0;

	for (INT32 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		m_sDetectRateFoByPass[i].Reset();
		m_AssistInfo[i].Reset();
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
CDediRoom::~CDediRoom()
{
	SAFE_DELETE( m_pStageInfoTimer );
	SAFE_DELETE( m_pDroppedWeaponMgr );
	SAFE_DELETE( m_pThrowWeaponMgr );
	SAFE_DELETE( m_pHMS );
	SAFE_DELETE( m_pRespawnMgr );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::Create()
{
	m_eState = RGS_READY;	// 그룹 상태 변경

	if( FALSE == m_ObjectManager.Create( DS_MAX_OBJECT ) )	return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//

//원거리 데미지 향상 Lv1 - 호칭 슬롯 2획득	: 6
//원거리 데미지 향상 Lv1 - DragunovG 		: 14
//원거리 데미지 향상 Lv2					: 15
//원거리 데미지 향상 Lv2 - PSG1 S			: 16
//원거리 데미지 향상 Lv3 - 호칭 슬롯 3 획득	: 17
//원거리 데미지 향상 Lv3 - L115A1			: 18
//원거리 데미지 향상 Lv4 					: 19

UINT32 CDediRoom::Register_C( P_BATTLE_ROOM_CREATE_ACK* pInfo, const P_BATTLE_ROOM_CREATE_REQ* pReq, char* pExtInfo, UINT16 ui16ExtSize, INT32 i32SocketIdx )
{
	if( RGS_READY != m_eState )
		return RELAY_ENCRYPTKEY_DEFAULT;
	
	// 스테이지 오브젝트 확인
	if( g_pMapManager )
	{
		CMapData* pMap = g_pMapManager->GetMap( pReq->_iStage );
		if( !pMap )
		{
			DSERROR("critical error: stage object not found: STAGE_UID:%d", StageID::GetStageUID(pReq->_iStage));
			return RELAY_ENCRYPTKEY_DEFAULT;
		}
	}
	
	// 메모리 초기화
	Reset();
	DeleteData( pInfo->_GroupIdx );
	m_TeamKillCount[ TEAM_RED  ] = 0;
	m_TeamKillCount[ TEAM_BLUE ] = 0;

	// 유저 상태 리셋
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++) 
	{
		m_pDediMember[i].Init();
	}
	
	// 게임서버의 룸 정보 저장
	m_RoomInfo			 = pReq->_RoomInfo;
	m_ui32CurrentUsers	 = pReq->_iStartUser;
	m_ui32BitHackOffUser = pReq->_ui32BitHackOffUser;
	m_ui32StageID		 = pReq->_iStage;
	m_stageType			 = StageID::GetStageMode( m_ui32StageID );	
	m_ui8AvailableWeapon = pReq->_ui8AvailableWeapon;
	m_ui8RoomSubType	 = pReq->_SubType;
	m_ui8InfoFlag		 = pReq->_ui8InfoFlag;
	m_bCanSoloPlay		 = pReq->_bCanSoloPlay;
	m_ui8MaxPerson		 = pReq->m_ui8MaxPerson;
	
	UINT16	nPos = 0;

	INT32 i32FirstSlot = -1;
	// 유저 아이템, 유저 타입
	for (UINT32 ui32SlotIdx = 0; ui32SlotIdx < SLOT_MAX_COUNT; ui32SlotIdx++)
	{
		CCharacter* pChara = &m_pGameCharacters[ui32SlotIdx];
		if (!GetBit(m_ui32CurrentUsers, ui32SlotIdx))
		{
			pChara->SetSlotItem(0);
			m_ar32DisigDFDRate[ui32SlotIdx] = 0.0f;
			m_ar32BombRangePlus[ui32SlotIdx] = 0.0f;
			m_ai64UID[ui32SlotIdx] = 0;
			m_ar32DesignationSpeed[ui32SlotIdx] = 0.0f;

			continue;
		}

		if (-1 == i32FirstSlot) i32FirstSlot = ui32SlotIdx;

		if (ui16ExtSize < nPos + sizeof(UINT32))
			return RELAY_ENCRYPTKEY_DEFAULT;

		pChara->SetSlotItem(*(UINT32*)&pExtInfo[nPos]);
		nPos += sizeof(UINT32);

		UINT8 ui8PBVer = *(UINT8*)&pExtInfo[nPos];
		nPos += sizeof(UINT8);

		m_aui8PBVer[ui32SlotIdx] = g_pConfig->RegistPBVersion(ui8PBVer);
		

		// vv3 - 체크
		for( INT32 j = CHAR_EQUIPMENT_PARTS_CHARA ; j < CHAR_EQUIPMENT_PARTS_COUNT ; j++ )
		{
			if( nPos + sizeof(T_ItemID) > ui16ExtSize )
				return RELAY_ENCRYPTKEY_DEFAULT;

			pChara->SetParts( j, *(T_ItemID*)&pExtInfo[nPos], m_aui8PBVer[ui32SlotIdx] );
			nPos += sizeof(T_ItemID);
		}

		ApplySkills(ui32SlotIdx, reinterpret_cast<CHARA_SLOT_SKILL*>( &pExtInfo[nPos] ));
		nPos += sizeof(CHARA_SLOT_SKILL);
		
		// vv3 - 체크
		ApplyEquipment( ui32SlotIdx, pChara->GetParts( CHAR_EQUIPMENT_PARTS_CHARA ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_HEAD ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_BERET ) );
		
		if( ui16ExtSize < nPos + sizeof(INT64) )
			return RELAY_ENCRYPTKEY_DEFAULT;

		m_ai64UID[ui32SlotIdx] = *(INT64*)&pExtInfo[nPos];
		nPos += sizeof(INT64);
		
		ApplyDesignation( ui32SlotIdx, (UINT8*)&pExtInfo[ nPos ] );
		nPos += sizeof( UINT8 ) * MAX_EQUIP_USER_TITLE;
	}
	if( IsTrainingRoom() )
	{
		INT32 i32AISlotFirst = (i32FirstSlot + 1) % 2;		
		for( INT32 i = 0 ; i < pReq->m_ui8AICount ; i ++ )
		{
			m_bAI[ i32AISlotFirst ] = TRUE;

			i32AISlotFirst += 2;
		}
	}

	// Net Group ID
	SetGroupIndex( pInfo->_GroupIdx );
	
	m_ui32FlagSendEnable	 = 0;
	m_ui32IntrusionUsers	 = 0;

	// 키 값 생성
	m_ui32EncryptKey	= (UINT32)rand();

	ShuffleHitPart( m_aui8HitPartIndex, m_ui32EncryptKey );
	
	// 룸 생성시간 설정
	m_ui32UdpRecvTime	= i3ThreadTimer::GetServerTime();
	m_r32StartTime		= GetGameTime();
	
	i3mem::FillZero( m_pr32LastPacketTime,	sizeof( m_pr32LastPacketTime ));		// 패킷 검사용 수신 시간 리셋
	i3mem::FillZero( m_ar32IntrusionTimes,	sizeof( m_ar32IntrusionTimes ));		// 난입 시간 버퍼 리셋
	i3mem::FillZero( m_ar32OffsetTime,		sizeof( m_ar32OffsetTime ));			// 옵셋 시간 리셋

	m_r32LastCheckTime = GetGameTime();

	m_bGMPause = FALSE;
	
	// 포트 설정 
	m_sPort	= pInfo->_Port;			// 룸에 할당된 포트
	
	// 소켓 인덱스
	m_i32SocketIdx = i32SocketIdx;	// 통신에 사용할 소켓(인덱스)
	
	// stageinfo를 보낼 준비를 한다.
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( GetBit( m_ui32CurrentUsers, i ) == TRUE )
		{
			SetIntrusion( i );
			m_ar32IntrusionTimes[ i ] = 0.1f; // 0보다 큰 값.
		} // 시작 유저
	} // 슬롯
		
	// 해킹데이터 리셋
	m_pHMS->ResetOnRoomCreate( m_ui32StageID, m_ui8AvailableWeapon );
		
	switch( m_stageType )
	{
	case STAGE_MODE_DEATHMATCH:
		RoundEndChk = &CDediRoom::RoundEndChkDeathMatch;
		break;
	case STAGE_MODE_BOMB:
		RoundEndChk = &CDediRoom::RoundEndChkBomb;

		switch( m_ui8MaxPerson )
		{
		case 16:
			m_r32BombInstallDelayTime	= 5.5f;
			m_r32BombUnInstallDelayTime	= 7.f;
			m_r32BombExplosionDelayTime	= 42.5f;
			break;
		case 10:
			m_r32BombInstallDelayTime	= 2.f;
			m_r32BombUnInstallDelayTime	= 4.f;
			m_r32BombExplosionDelayTime	= 25.f;
			break;
		}
		break;
	case STAGE_MODE_DESTROY:
		RoundEndChk = &CDediRoom::RoundEndChkDestroy;
		break;
	case STAGE_MODE_ANNIHILATION:
		RoundEndChk = &CDediRoom::RoundEndChkAnnihilation;
		break;
	case STAGE_MODE_DEFENCE:
		RoundEndChk = &CDediRoom::RoundEndChkDefence;
		break;	
	case STAGE_MODE_ESCAPE:
		RoundEndChk = &CDediRoom::RoundEndChkEscape;
		break;		
	case STAGE_MODE_TUTORIAL:
		RoundEndChk = &CDediRoom::RoundEndChkTutorial;
		break;
	case STAGE_MODE_CROSSCOUNT:
		RoundEndChk = &CDediRoom::RoundEndChkCrossCount;
		break;
	case STAGE_MODE_CONVOY:
		RoundEndChk = &CDediRoom::RoundEndChkConvoy;
		break;
	case STAGE_MODE_RUN_AWAY:
		RoundEndChk = &CDediRoom::RoundEndChkRunAway;
		break;	
	default:
		return RELAY_ENCRYPTKEY_DEFAULT;
	}

	if( IsTrainingRoom() )
	{
		RoundEndChk = &CDediRoom::RoundEndChkTraining;
	}

	if( StageID::IsMultiWeaponSlot(m_ui32StageID) )
	{
		SetMultiWeaponSlot( );
	}
	else
	{
		SetSingleWeaponSlot( );
	}

	m_RoundMissionState = MISSION_STATE_PRE_BATTLE;

	_InitGameObjects();
	
	m_eState			 = RGS_USE; // 가장 마지막에 셋팅되어야 한다.

	return m_ui32EncryptKey;
}

// SYNC DATA를 리셋한다
// 라운드가 끝날 때마다 리셋시켜야 한다
// Multicast 용도
void CDediRoom::ResetSyncData_C()
{
	I3TRACE("call ResetSyncData\n");
	
	//------------------------------------------------------
	// 플래그
	//------------------------------------------------------
	ResetFlagAll();				// stop hack checking
	m_ui32FlagSendEnable = 0;	// stop send 
	
	// misssion
	m_MissionState	= NET_MISSION_NONE;
	m_MissionArea	= BOMB_AREA_A;
	m_bArmDone		= FALSE;
	m_MissionIntruder.Reset();				// stageinfo mission

	// --------------------------------------------------------------
	// Reset Game Reference Table
	// --------------------------------------------------------------
	_ResetRespawnObjects();
	_ResetMisssionObjects();
	
	// --------------------------------------------------------------
	// SYNC Data : 
	// --------------------------------------------------------------

	// ------------------------------------------------------------------------
	// 게임 캐릭터 정보 버퍼 : 0으로 초기화 하고 기본값을 설정합니다

	for( INT32 i = 0; i < SLOT_MAX_COUNT; ++i )
	{
		m_pGameCharacters[i].ResetBattleInfo();
		getDetectRate(i)->Reset();

		m_CharCastPacket[i].Reset();
		m_AssistInfo[i].Reset();
	}
	
	i3mem::FillZero( m_bCastCheck_WeaponSync,						sizeof( m_bCastCheck_WeaponSync			));		
	i3mem::FillZero( m_pCastCheck_Chara,							sizeof( m_pCastCheck_Chara				));		
	i3mem::FillZero( m_pCastPacket_Chara_State,						sizeof( m_pCastPacket_Chara_State			));
	i3mem::FillZero( m_pCastPacket_Chara_PosRot,					sizeof( m_pCastPacket_Chara_PosRot		));
	i3mem::FillZero( m_pCastPacket_Chara_Weapon,					sizeof( m_pCastPacket_Chara_Weapon		));
	i3mem::FillZero( m_pCastPacket_Chara_WeaponParam,				sizeof( m_pCastPacket_Chara_WeaponParam	));
	i3mem::FillZero( m_pCastPacket_Chara_Life,						sizeof( m_pCastPacket_Chara_Life			));

	i3mem::FillZero( m_pCastPacket_Chara_HitCount_ByChara,			sizeof( m_pCastPacket_Chara_HitCount_ByChara		));
	i3mem::FillZero( m_pCastPacket_Chara_HitCount_ByExplosion,		sizeof( m_pCastPacket_Chara_HitCount_ByExplosion	));
	i3mem::FillZero( m_pCastPacket_Chara_HitCount_ByObject,			sizeof( m_pCastPacket_Chara_HitCount_ByObject		));
	i3mem::FillZero( m_pCastPacket_Chara_Hit_ByChara,				sizeof( m_pCastPacket_Chara_Hit_ByChara			));
	i3mem::FillZero( m_pCastPacket_Chara_Hit_ByExplosion,			sizeof( m_pCastPacket_Chara_Hit_ByExplosion		));
	i3mem::FillZero( m_pCastPacket_Chara_Hit_ByObject,				sizeof( m_pCastPacket_Chara_Hit_ByObject			));	
 
	i3mem::FillZero( m_pCastPacket_Chara_Suffering,					sizeof( m_pCastPacket_Chara_Suffering		));
	i3mem::FillZero( m_pCastPacket_Chara_Death,						sizeof( m_pCastPacket_Chara_Death			));
	i3mem::FillZero( m_TeamKillCount,								sizeof( m_TeamKillCount					));


	// -----------------------------------------------------
	// chara data
	// -----------------------------------------------------
	m_ui32FlagHackingReportSent = 0;

	i3mem::FillZero( m_aHackType,									sizeof( m_aHackType) );	
	i3mem::FillZero( m_aui8SendLife,								sizeof( m_aui8SendLife));
	i3mem::FillZero( m_pKillInfo,									sizeof( m_pKillInfo ));	
	i3mem::FillZero( m_ar32LastDeathTimes,							sizeof( m_ar32LastDeathTimes ));
	i3mem::FillZero( m_aui32RespawnIdx,								sizeof( m_aui32RespawnIdx ));
	i3mem::FillZero( m_aui32RespawnCount,							sizeof( m_aui32RespawnCount ));

	i3mem::FillZero( m_ar32InvincibleTime,							sizeof( m_ar32InvincibleTime ));
	i3mem::FillZero( m_ar32FlyBeginTime,							sizeof( m_ar32FlyBeginTime ));
	
	i3mem::FillZero( m_ar32AttackBeginTime,							sizeof( m_ar32AttackBeginTime ));
	i3mem::FillZero( m_ar32HowlBeginTime,							sizeof( m_ar32HowlBeginTime ));
	i3mem::FillZero( m_ar32TouchDownBeginTime,						sizeof( m_ar32TouchDownBeginTime ));
	i3mem::FillZero( m_r32TriggerTime,								sizeof( m_r32TriggerTime));
	i3mem::FillZero( m_r32PosCheckTime,								sizeof( m_r32PosCheckTime));
	i3mem::FillZero( m_bKickUserBattle,								sizeof( m_bKickUserBattle));
	i3mem::FillZero( m_ar32TimeOldSpeed,							sizeof( m_ar32TimeOldSpeed));	
	i3mem::FillZero( m_aui32TouchDownCount,							sizeof( m_aui32TouchDownCount));
	i3mem::FillZero( m_r32LastWeaponSyncTime,						sizeof( m_r32LastWeaponSyncTime ));
	i3mem::FillZero( m_vPosOldSpeed,								sizeof( m_vPosOldSpeed));

	i3mem::FillZero( m_r32OnLoadPacketTime,							sizeof(m_r32OnLoadPacketTime));

	i3mem::Fill( m_aui16LastGrenadeIdx,			0xffff,				sizeof( m_aui16LastGrenadeIdx));
	i3mem::Fill( m_bCheckOnLoad,				FALSE,				sizeof( m_bCheckOnLoad));

	for ( INT32 i = 0; i < m_i32GameObjectCount; i++)
	{
		m_aGameObjects[i].Reset();
	}

	for( INT32 i = 0 ; i < m_i32GameObjectGroupCount ; i++ )
	{
		m_pGameObjectGroup[i].Reset();
	}

	m_pObjectUseItemMgr.Reset();

	// ------------------------------------------------------------------------
	// 스테이지 오브젝트 카운터:
	// ------------------------------------------------------------------------
	// 단, P2P_SUB_HEAD_GRENADE와 P2P_SUB_HEAD_DROPEDWEAPON은 MAX 만큼 보낸다.

	i3mem::FillZero( m_pStageObj_Count, sizeof( m_pStageObj_Count) );
	m_pStageObj_Count[ P2P_SUB_HEAD_NPC ]					= 0;
	m_pStageObj_Count[ P2P_SUB_HEAD_STAGEINFO_MISSION ]		= 1;						// 예외처리 (폭파모드에서만 사용)	
	
	// ------------------------------------------------------------------------
	// 멀티 캐스트:
	// ------------------------------------------------------------------------
	i3mem::FillZero( m_pNextSend_Idx, sizeof( m_pNextSend_Idx ) ); // NEXT 전송 인덱스
	
	// ------------------------------------------------------------------------
	//  reset HMS
	// ------------------------------------------------------------------------
	
	m_pStageInfoTimer->Init();
	m_pDroppedWeaponMgr->Reset( GetGameTime() );
	m_pThrowWeaponMgr->Reset();

	I3TRACE("*********** RESET SYNC ************** \n");
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//

// 여러 스레드에서 접근한다. 개선 필요
void CDediRoom::Revoke_C( INT32 i32GroupIdx )
{
	SetGroupState( RGS_READY );
}

void CDediRoom::DeleteData( INT32 i32GroupIdx )
{
	// 메모리 돌려주기
	CUdpBuffer* pRecvBuffer = NULL;
	INT32 i32RemainPacketCount = g_pDataArchive->GetCount( i32GroupIdx);
	for(INT32 i = 0; i < i32RemainPacketCount; i++)
	{
		pRecvBuffer = NULL;
		g_pDataArchive->Pop( i32GroupIdx, (i3ElementBase**)&pRecvBuffer);
		if( pRecvBuffer != NULL )
		{
			g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::Enter_C( P_BATTLE_USER_ENTER_REQ* pReq )
{
	DSMSG("user %d is trying to join group %d\n", pReq->_cSlotIdx, pReq->_wGroupIdx );

	UINT32 ui32SlotIdx = pReq->_cSlotIdx;
	CCharacter* pChara = GetGameCharacter( ui32SlotIdx );

	if( ! pChara )
	{
		DSWARNING("[ERROR] JoinGroup SlotIdx Error:%d\n", ui32SlotIdx );
		return FALSE;
	}
	
	if( RGS_USE != m_eState )
	{
		DSWARNING("[ERROR] JoinGroup State Error: %d, Slot:%d\n", m_eState, ui32SlotIdx );
		return FALSE;
	}

	if( m_pDediMember[ ui32SlotIdx ].GetPlaying() )
	{
		DSWARNING("[ERROR] JoinGroup Play State Error: Slot:%d\n", ui32SlotIdx );
		return FALSE;
	}
	
	m_cs.Lock();

	// -------------------------------------------------------------------------------
	// set packet data
	// -------------------------------------------------------------------------------
	
	// 마지막 패킷을 받은 시간
	m_ui32UdpRecvTime = i3ThreadTimer::GetServerTime();

	ResetPlayDataOnce   ( ui32SlotIdx );					// play data

	m_pDediMember[ ui32SlotIdx ].Init();

	pChara->Reset();
	
	m_ai64UID			[ ui32SlotIdx ]	= pReq->_i64UID;

	m_aui8PBVer[ui32SlotIdx] = g_pConfig->RegistPBVersion(pReq->m_ui8PBVer);

	// -------------------------------------
	// Chara ID & ITEM
	// -------------------------------------
	// vv3 - 체크
	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		pChara->SetParts( i, pReq->m_TPartsID[i], m_aui8PBVer[ui32SlotIdx] );
	}

	pChara->SetCharClass( static_cast<CHARA_CLASS>(pReq->m_CharSkills.m_PrimarySkill) );

	// vv3 - 체크
	ApplyEquipment	( ui32SlotIdx, pChara->GetParts( CHAR_EQUIPMENT_PARTS_CHARA ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_HEAD ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_BERET) );
	ApplySkills(pReq->_cSlotIdx, &pReq->m_CharSkills);
	ApplyDesignation( ui32SlotIdx, pReq->_aui8Designation );
	
	// ----------------------------------------------------------------
	// multicast reset
	// ----------------------------------------------------------------
	ResetSendEnable( ui32SlotIdx );
	m_pCastPacket_Chara_Life[ ui32SlotIdx ].iLife = 0;
	//m_pCastPacket_Chara_Def [ ui32SlotIdx ].Reset();
	
	// --------------------------------------------------------------------------------
	// user
	// --------------------------------------------------------------------------------
	
	ResetFlag		( ui32SlotIdx );					// stop hack checker
	ResetHackFlagForLog(ui32SlotIdx);
	m_pHMS->ResetOnEnter( ui32SlotIdx );	// reset hack data
	
	// -------------------------------------
	// Death & HP 셋팅 : 난입, 방생성
	// -------------------------------------
	
	SetIntrusion ( ui32SlotIdx );
	REAL32 r32GameTime = GetGameTime();
	m_ar32IntrusionTimes[ ui32SlotIdx ] = r32GameTime;	// for send stageinfo
	m_ar32LastDeathTimes[ ui32SlotIdx ] = 0.0f;			// death time 
	m_pr32LastPacketTime[ ui32SlotIdx ] = 0.0f;			// 강제로 리셋시키고, check packet에서 재설정.
	m_ar32OffsetTime	[ ui32SlotIdx ] = 0.0f;	

	
	m_pr32LastPacketTime		  [ ui32SlotIdx ]   = r32GameTime;

	// --------------------------------------------------------------------------------
	// chara
	// --------------------------------------------------------------------------------
	
	pChara->SetSlotItem( pReq->_iItem );
		
	// --------------------------------------------------------------------------------
	// wait udp
	// --------------------------------------------------------------------------------
	
	SetBit   ( &m_ui32CurrentUsers,		ui32SlotIdx ); 	// add to current user

	if ( pReq->_bHackOff )
		SetBit( &m_ui32BitHackOffUser, ui32SlotIdx );
	
	m_ar32IntrusionTimes[ ui32SlotIdx ] = r32GameTime;	// 최초 stageinfo를 보내기 위한 타이밍을 잡기 위함
	
	
	// -------------------------------------
	// 해킹 상태
	// -------------------------------------
	ResetHackType(ui32SlotIdx);
	//SetHackType			 ( ui32SlotIdx, HACK_TYPE_NO );
	SetHackingReportState( ui32SlotIdx, FALSE );

	m_cs.Unlock();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- udp를 통해서 호출 됨
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::UdpJoinGroup( N_GAME_HEADER* pHeader, N_GAME_RELAY_HOLEPUNCH* pInfo, GAME_ROOM_OPTION* pOption, UINT32 ui32IP, UINT16 ui16Port )
{

	UINT32 ui32SlotIdx = pInfo->cSlotIdx;

	// Slot 번호 검사
	if( !IS_VALID_MEMBER_RANGE( ui32SlotIdx ) )
	{
		DSWARNING("[ERROR] JoinGroup SlotIdx:%d\n", ui32SlotIdx );
		return FALSE;
	}
	
	// 룸 상태 검사
	if( RGS_USE != m_eState )	
	{
		DSWARNING("[ERROR] JoinGroup State Error:%d, Slot:%d\n", m_eState, ui32SlotIdx );
		return FALSE;
	}
	
	// EncryptKey 유효성 검사
	if( !CheckEncryptKey( pInfo->wEncryptKey ) )
	{
		DSWARNING("[ERROR] JoinGroup Encrypt OrgKey:%d, UserKey:%d, IP:%d, Port:%d\n", 
					m_ui32EncryptKey, pInfo->wEncryptKey, ui32IP, ui16Port );
		return FALSE;
	}

	// uid 검사
	if( pHeader->_ui8UID != (UINT8)(GetUID( ui32SlotIdx ) % 256) )
	{
		DSWARNING("[ERROR] JoinGroup UID 1byte: slot:%d, client:%d, server:%d\n", 
					ui32SlotIdx, pHeader->_ui8UID, GetUID( ui32SlotIdx ) % 256 );
		return FALSE;
	}
	
	
	m_cs.Lock();

	// 마지막 패킷을 받은 시간을 의미
	m_ui32UdpRecvTime = i3ThreadTimer::GetServerTime();

	// 맴버를 등록한다. 맴버가 비어있지 않으면 FALSE;
	if( FALSE == m_pDediMember[ ui32SlotIdx ].InsertContext( ui32IP, ui16Port ) )
	{
		m_cs.Unlock();
		return FALSE;
	}

	I3TRACE("Hole punching packet time = %f\n", pHeader->_GameTime );

	REAL32 r32GameTime = GetGameTime();

	m_pr32LastPacketTime[ ui32SlotIdx ] = pHeader->_GameTime; // packet time 초기화 (클라이언트 패킷 타임. 0.0임)
	m_ar32OffsetTime	[ ui32SlotIdx ] = r32GameTime - pHeader->_GameTime;

	// stage info를 보내야 한다
	SetIntrusion( ui32SlotIdx );

	pOption->m_ui8Mode						= static_cast<UINT8>(m_stageType);
	switch( m_stageType )
	{
	case STAGE_MODE_BOMB:
		{
			pOption->m_r16BombInstallDelayTime		= m_r32BombInstallDelayTime;
			pOption->m_r16BombUnInstallDelayTime	= m_r32BombUnInstallDelayTime;
			pOption->m_r16BombExplosionDelayTime	= m_r32BombExplosionDelayTime;
		}
		break;
	}

	m_cs.Unlock();

	return TRUE;
}

void CDediRoom::ReportUDPVersionError( UINT32 ui32SlotIdx  )
{
	if( ui32SlotIdx >= SLOT_MAX_COUNT ) return;
	if( IsHackUser( ui32SlotIdx ) ) return;

	m_cs.Lock();

	// Hack Checker Main에서 판단한 해킹
	SetHackType( ui32SlotIdx, HACK_TYPE_UDP_VERSION );

	m_ui32TcpSendFlag |= DS_SEND_CHEATING_REPORT;

	m_cs.Unlock();
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::Withdraw( UINT8 ui8SlotIdx )
{
	if( RGS_INVALID == m_eState )						return FALSE;

	if( ! IS_VALID_MEMBER_RANGE(ui8SlotIdx) )				return FALSE;

	if( m_pDediMember[ ui8SlotIdx ].DeleteContext() )		return TRUE;

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::Withdraw_C( UINT8 ui8SlotIdx )
{
	if( RGS_INVALID == m_eState )					return FALSE;

	if( ui8SlotIdx >= SLOT_MAX_COUNT )				return FALSE;

	m_pDediMember[ ui8SlotIdx ].SetPlaying( FALSE );

	return TRUE;
}


BOOL CDediRoom::Leave_C( UINT32 ui32SlotIdx )
{
	if( RGS_INVALID == m_eState )					return FALSE;
	if( ui32SlotIdx >= SLOT_MAX_COUNT )				return FALSE;

	m_cs.Lock();

	// ----------------------------------------------------------------
	// Hack Control
	// ----------------------------------------------------------------
	ResetFlag( ui32SlotIdx );						// hack check 끔
	ResetHackFlagForLog(ui32SlotIdx);
	m_pHMS->ResetOnLeave( ui32SlotIdx );			// hms 정리
	ResetSendEnable ( ui32SlotIdx );					
	
	// ----------------------------------------------------------------
	// User Control
	// ----------------------------------------------------------------

	ClearBit( &m_ui32CurrentUsers ,ui32SlotIdx );
	ClearBit( &m_ui32BitHackOffUser, ui32SlotIdx );

	m_pDediMember[ ui32SlotIdx ].Init();

	// ----------------------------------------------------------------
	// chara
	// ----------------------------------------------------------------
	
	m_pGameCharacters[ ui32SlotIdx ].Reset();
	
	// ----------------------------------------------------------------
	// multicast
	// ----------------------------------------------------------------
	m_pCastPacket_Chara_Life[ ui32SlotIdx ].iLife = 0;
	//m_pCastPacket_Chara_Def [ ui32SlotIdx ].Reset();

	// ----------------------------------------------------------------
	// Battle Play Data
	// ----------------------------------------------------------------
	ResetPlayDataOnce( ui32SlotIdx );
	//퇴장하는 유저가 가진 어시스트 정보 초기화
	m_AssistInfo[ui32SlotIdx].Reset();

	//퇴장한 유저가 남긴 어시스트 정보 초기화.
	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (FALSE == m_pDediMember[i].GetPlaying()) continue;
		m_AssistInfo[i].SlotReset(ui32SlotIdx);
	}
	
	m_cs.Unlock();


	
	return TRUE;
}

void CDediRoom::ResetPlayDataOnce( UINT32 ui32SlotIdx )
{
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
	if( ui32SlotIdx < SLOT_MAX_COUNT )
	{
		// ---------------------------------------------------
		// controlled object, animation object 소유권을 제거
		// M197, RPG7 Box
		// ---------------------------------------------------
		UINT16 ui16NetIdx;
		for( INT32 i = 0; i < m_i32AnimationObjectCount; i++ )
		{
			ui16NetIdx = m_pNetIndex_Obj_Anim[ i ];

			if( m_aGameObjects[ ui16NetIdx ].getEventSender() == (UINT8)ui32SlotIdx )
			{
				m_aGameObjects[ ui16NetIdx ].ResetUse();
			}
		}

		for( INT32 i = 0; i < m_i32ControlledObjectCount; i++ )
		{
			ui16NetIdx = m_pNetIndex_Obj_Controlled[ i ];

			if( (UINT32)m_aGameObjects[ ui16NetIdx ].getEnterCharaIndex() == ui32SlotIdx )
			{
				m_aGameObjects[ ui16NetIdx ].ResetUse();
			}
		}
	
		// ----------------------------------------------------
		// Play Data
		// ----------------------------------------------------
		m_ar32FlyBeginTime		[ ui32SlotIdx ] = 0.0f;
		m_ar32AttackBeginTime	[ ui32SlotIdx ] = 0.0f;
		m_ar32HowlBeginTime		[ ui32SlotIdx ] = 0.0f;
		m_ar32TouchDownBeginTime[ ui32SlotIdx ] = 0.0f;
		
		m_DestructionData.m_ObjectSendDamageInfo._nUserDamage[ ui32SlotIdx ] = 0;
		m_DefenceData.getSendDamageInfo()->setUserDamage( 0, ui32SlotIdx, 0 ); // tankA
		m_DefenceData.getSendDamageInfo()->setUserDamage( 1, ui32SlotIdx, 0 ); // tankB

		_ResetThrownWeaponInfo( ui32SlotIdx );	// 던져진 수류탄 정보 리셋
		m_aui16LastGrenadeIdx[ ui32SlotIdx ] = (UINT16)-1;
			
		// ----------------------------------------------------
		// User Data
		// ----------------------------------------------------
		
		m_pr32LastPacketTime[ ui32SlotIdx ] = 0.0f;
		m_ar32LastDeathTimes[ ui32SlotIdx ] = 0.0f; 
		m_ar32IntrusionTimes[ ui32SlotIdx ] = 0.0f;
		m_aui32RespawnIdx	[ ui32SlotIdx ] = 0;
		m_aui32RespawnCount	[ ui32SlotIdx ] = 0;
		m_aui32TouchDownCount[ui32SlotIdx ]	= 0;
		m_ar32InvincibleTime[ ui32SlotIdx ] = 0.0f;

		m_ar32DisigDFDRate  [ ui32SlotIdx ] = 0.0f;
		m_ar32BombRangePlus	[ ui32SlotIdx ] = 0.0f;
		m_aui8SendLife		[ ui32SlotIdx ] = 0;

		m_aCharaInfo		[ ui32SlotIdx ].Clear();
		m_aHelmet			[ ui32SlotIdx ].ClearAll();
		m_aBeret			[ ui32SlotIdx ].ClearAll();

		m_ai64UID			[ ui32SlotIdx ] = 0LL;
		m_aui8PBVer[ui32SlotIdx] = g_pConfig->RegistPBVersion(g_pConfig->m_eServerVersion);

	
		m_bCheckOnLoad[ ui32SlotIdx ] = FALSE;

		m_ar32TimeOldSpeed[ ui32SlotIdx ] = 0.0f;
		m_vPosOldSpeed[ ui32SlotIdx ].Reset();
	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::CheckMember( UINT8 ui8SlotIdx, UINT32 ui32IP, UINT16 ui16Port )
{
	if( !IS_VALID_MEMBER_RANGE( ui8SlotIdx) )		return FALSE;
	if( !m_pDediMember[ ui8SlotIdx ].IsMember() )	return FALSE;
	if( !m_pDediMember[ ui8SlotIdx ].GetPlaying() )
	{
		m_pDediMember[ ui8SlotIdx ].DeleteContext();
		return FALSE;
	}
	if( !m_pDediMember[ ui8SlotIdx ].IsSameAddress( ui32IP, ui16Port ) )
	{
		//m_pDediMember[ ui8SlotIdx ].DeleteContext();
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- udp UDPPROTOCOL_CN_INFO 처리할 때 호출 됨
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::GetRelayTarget( INT32 i32Thread, CUdpBuffer* pRecvBuffer )
{
	// -------------------------------------------------------------------
	// 클라이언트 검사
	if( !IS_VALID_MEMBER_RANGE(pRecvBuffer->GetSlotIdx()))
	{
		DSWARNING( "[ERROR] GetRelayTarget Slot : %d", pRecvBuffer->GetSlotIdx() );
		return FALSE;
	}
	if( !IS_VALID_MEMBER_RANGE(pRecvBuffer->GetTargetIdx()) )
	{
		DSWARNING( "[ERROR] GetRelayTarget TargetIdx : %d", pRecvBuffer->GetTargetIdx() );
		return FALSE;
	}
	if( !CheckEncryptKey( pRecvBuffer->GetEncryptKey()) )
	{
		DSWARNING( "[ERROR] GetRelayTarget Encrypt OrgKey:%d, SrcKey:%d, IP:%d, Port:%d", 
			m_ui32EncryptKey, pRecvBuffer->GetEncryptKey(), m_pDediMember[ pRecvBuffer->GetSlotIdx() ].GetIp(), m_pDediMember[ pRecvBuffer->GetSlotIdx() ].GetPort() );
		return FALSE;
	}

	// 마지막 패킷을 받은 시간을 의미
	m_ui32UdpRecvTime = i3ThreadTimer::GetServerTime();

	pRecvBuffer->SetAddress( m_pDediMember[ pRecvBuffer->GetTargetIdx() ].GetIp(), m_pDediMember[ pRecvBuffer->GetTargetIdx() ].GetPort() );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL CDediRoom::CheckEncryptKey( UINT32 wEncryptKey )
{
	return ((RELAY_ENCRYPTKEY_DEFAULT != wEncryptKey) && (m_ui32EncryptKey == wEncryptKey));
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//
BOOL  CDediRoom::_InitGameObjects()
{
	if( NULL == g_pMapManager )			return FALSE;

	CMapData* pMap = g_pMapManager->GetMap( m_ui32StageID );	// map이 없을 수 있다
	if( NULL == pMap )					return FALSE;
	
	// Object 카운트
	m_i32GameObjectCount				= pMap->GetGameObjectCount();
	for( INT32 i = 0 ; i < m_i32GameObjectCount ; i++ )
	{
		pMap->GetGameObjectList()[i].CopyTo( &m_aGameObjects[i] );
	}
	
	m_i32AnimationObjectCount			= pMap->GetAnimationObjectCount();
	m_i32ControlledObjectCount			= pMap->GetControlledObjectCount();

	// Object Group 카운트
	m_i32GameObjectGroupCount			= pMap->GetGameObjectGroupCount();
	CDSObjectGroupIdx* pObjectGroupIdx	= pMap->GetGameObjectGroupList();

	for( INT32 i = 0 ; i < m_i32GameObjectGroupCount ; i++ )
	{
		m_pGameObjectGroup[ i ].SetGroup( &pObjectGroupIdx[i], m_aGameObjects );
	}

	// BattleItem Use 세팅
	m_pObjectUseItemMgr.SetObjectCount(pMap->GetBattleUseItemObjectCount());
	m_pObjectUseItemMgr.SetObjectList(m_aGameObjects, pMap->GetBattleUseItemObjectIdx());

	// ------------------------------------------------------------------------
	// 리소스 값으로 오버라이트: 추가 커스텀 밸류도 여기서 셋팅.
	// ------------------------------------------------------------------------
	m_ObjectManager.Reset();
	m_AvailableWeapon.Reset();

	// 초장에 RPG7을 추가해야 합니다.
	if (m_pObjectUseItemMgr.GetObjectInRPG7() == true)
	{
		m_AvailableWeapon.Insert(Weapon::GetItemID(WEAPON_CLASS_ROCKET_LAUNCHER, ROCKET_LAUNCHER_RPG7));
	}

	INT32 i32IndexStatic		= 0;
	INT32 i32IndexAnim			= 0;
	INT32 i32IndexControlled	= 0;
	INT32 i32IndexMove			= 0;
	m_i32RunAwayObjectCount		= 0;

	T_ItemID TAvailableWeapon;
	WEAPON_SLOT_TYPE eSlot;
	for ( INT32 i = 0; i < m_i32GameObjectCount; i++)
	{
		m_ObjectManager.AddObject( i, &m_aGameObjects[i] );

		m_aGameObjects[i].Reset();

		// 맵에서 구할 수 있는 무기 세팅
		if( ChangeAvailableWeapon( (EWEAPON_TYPE)m_aGameObjects[i].m_eWeaponType, TAvailableWeapon, eSlot ) )
		{

			m_AvailableWeapon.Insert( TAvailableWeapon );
		}

		// 정리할 필요 : red 1, blue 0로 통일할 것. (5월 3일 breakdown)
		// 공통 처리
		if( m_aGameObjects[i].GetSType() == OBJECT_S_TYPE_TARGET )
		{
			if ( m_aGameObjects[i].m_i8GroupIdx == 0) // blue team
			{
				m_DestructionData.m_pObjectBlue = &m_aGameObjects[i];
			}
			else
			{
				m_DestructionData.m_pObjectRed  = &m_aGameObjects[i];
			}
		}

		switch( m_aGameObjects[i].m_i16NetType )
		{
		case I3_NETWORK_GAMEOBJECT_TYPE_STATIC:			// 고정되어 있다
			{
				// controlled obj의 경우 예외처리 해야 함. 
				m_pNetIndex_Obj_Static  [ i32IndexStatic ]     = m_aGameObjects[i].m_i16NetIdx;
				i32IndexStatic++;
			}
			break;
		case I3_NETWORK_GAMEOBJECT_TYPE_ANIM:
			{
				m_aGameObjects[i].Reset();
				
				// outpost riding 예외 : riding 헬기는 outpost에만 나옴
				if ( m_aGameObjects[i].GetSType() == OBJECT_S_TYPE_RIDING_HELICOPTER )
				{
					// outpost 헬기 예외처리 (calc key path에서 destruction data 구조체 이용)
					m_DestructionData.m_pObjectRed = &m_aGameObjects[i];
				}
				// respawn 헬리콥터 예외: 팀 인덱스가 다름
				else if ( m_aGameObjects[i].GetSType() == OBJECT_S_TYPE_RESPAWN_HELICOPTER)
				{
					if ( m_aGameObjects[i].m_i8GroupIdx == 1) 
					{
						m_DestructionData.m_pObjectBlue = &m_aGameObjects[i];
					}
					else
					{
						m_DestructionData.m_pObjectRed  = &m_aGameObjects[i];
					}
				}
				else if( m_aGameObjects[i].GetSType() == OBJECT_S_TYPE_REPAIR_TRAIN )
				{
					m_pRepairTrain = &m_aGameObjects[i];
				}

				m_aGameObjects[ i ].m_bPathKeyChanged = true;
				m_pNetIndex_Obj_Anim  [ i32IndexAnim ]				= m_aGameObjects[i].m_i16NetIdx;
				
				I3TRACE("INITIAL ANIMATION OBJECT : net index = %d\n", m_pNetIndex_Obj_Anim[ i32IndexAnim ] );
				i32IndexAnim++;
			}
			break;
		default: break;
		}

		switch( m_aGameObjects[i].GetObjectUseType() )
		{
		case OBJECT_USE_TYPE_HAVE:
			{
				m_pConvoyObject = &m_aGameObjects[i];
				m_pNetIndex_Obj_Move[ i32IndexMove ]				= m_aGameObjects[i].m_i16NetIdx;
				i32IndexMove++;
			}
			break;
		case OBJECT_USE_TYPE_SWITCH:
			{
				m_pRunAwayObject[ m_i32RunAwayObjectCount ]			= &m_aGameObjects[i];
				m_i32RunAwayObjectCount++;
			}
			break;
		case OBJECT_USE_TYPE_RESPAWN:
			{
				m_pNetIndex_Obj_Controlled [ i32IndexControlled ]	= m_aGameObjects[i].m_i16NetIdx;
				i32IndexControlled++;
			}
			break;
		}
	}

	// ------------------------------------------------------------------------
	// 오브젝트 카운터:
	// ------------------------------------------------------------------------
	m_i32CountOfStatic		= i32IndexStatic;
	m_i32CountOfAnim		= i32IndexAnim; 
	m_i32CountOfControlled	= i32IndexControlled;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function: 
//
// Description:
//	- 
//		- 
//      - 
//		- 
// Arguments:
//	- 
//		- 
// Return:
//	- 
//

BOOL CDediRoom::_ResetMisssionObjects()
{
	UINT32 nTargetCount = 0;

	if ( !IsDestroyMissionMode() &&
		 !IsDefenceMissionMode() &&  
		 !IsBombMissionMode()

	) return TRUE;

	m_DestructionData.reset();
	m_DefenceData.reset();
	
	for ( INT32 i = 0; i < m_i32GameObjectCount; i++)
	{
		if( m_aGameObjects[i].m_iSType & OBJECT_S_TYPE_TARGET )
		{
			if( IsDestroyMissionMode() )
			{
				if ( m_aGameObjects[i].m_iSType == OBJECT_S_TYPE_RESPAWN_HELICOPTER)
				{
					// 헬기는 순서가 바뀌어 있다.
					if( m_DestructionData.m_pObjectBlue == NULL)
					{
						m_DestructionData.m_pObjectBlue = &m_aGameObjects[i];
						nTargetCount++;
					}
					else if( m_DestructionData.m_pObjectRed == NULL )
					{
						m_DestructionData.m_pObjectRed = &m_aGameObjects[i];
						nTargetCount++;
						break;
					}
				}
				else
				{
					// 순서대로 2개만 셋팅
					if( m_DestructionData.m_pObjectRed == NULL)
					{
						m_DestructionData.m_pObjectRed = &m_aGameObjects[i];
						nTargetCount++;
					}
					else if( m_DestructionData.m_pObjectBlue == NULL )
					{
						m_DestructionData.m_pObjectBlue = &m_aGameObjects[i];
						nTargetCount++;
						break;
					}
				}
			}
			else if ( IsBombMissionMode() )
			{
				if ( m_aGameObjects[i].m_iSType == OBJECT_S_TYPE_RIDING_HELICOPTER ) 
				{	// outpost에서만 사용됨 
					// 1개 밖에 없음 
					
					/*
					Outpost 헬기 = OBJECT_S_TYPE_RIDING_HELICOPTER
					상태1. user가 타고 있지 않으면 path key 0 (idle) broadcast.
					상태2. user가 타면(character/use object 패킷) 랜덤하게 path1-path3 중 선택 broadcast.
					상태3. user가 타고있는 중이면 animation local time broadcast : 다른 애니메이션 오브젝트와 공통.
					상태4. play time을 감시하고 있다가 duration이 종료되면 (내린 상태로 전환) path key 0(idle) broadcast.
					*/

					m_aGameObjects[i].setPathKey( 0 ); // idle
					m_DestructionData.m_pObjectRed = &m_aGameObjects[i]; // 애니메이션을 위해 헬기 포인터 보관.
				}

			}
			else if( IsDefenceMissionMode() )
			{
				// 순서대로 2개만 셋팅
				if( m_DefenceData.getDefenceObj(1) == NULL)
				{
					I3TRACE("Tank1:%d\n", i);
					m_DefenceData.setDefenceObj(1, &m_aGameObjects[i] );
					nTargetCount++;
				}
				else if( m_DefenceData.getDefenceObj(0) == NULL )
				{
					I3TRACE("Tank2:%d\n", i);
					m_DefenceData.setDefenceObj(0, &m_aGameObjects[i] );
					nTargetCount++;
					break;
				}
			}
		}
	}

	return TRUE;
}

void CDediRoom::_ResetThrownWeaponInfo( const UINT32 ui32SlotIdx )
{ 
	if( SLOT_MAX_COUNT <= ui32SlotIdx )		return;
	
	UINT32 ui32EndSlotIdx = ( ui32SlotIdx + 1) * MAX_THROWWEAPON_SLOP;
	for( UINT32 ui32NetSlotIdx = ui32SlotIdx * MAX_THROWWEAPON_SLOP ; ui32NetSlotIdx < ui32EndSlotIdx ; ui32NetSlotIdx++)
	{
		m_aThrownGrenade[ ui32NetSlotIdx ].Reset();
	}
}

BOOL CDediRoom::_ResetRespawnObjects()
{
	I3ASSERT( m_pRespawnMgr != NULL );
	m_pRespawnMgr->Reset();

	if( g_pMapManager )
	{
		CMapData*	pMap = g_pMapManager->GetMap( m_ui32StageID );
		if( pMap ) // map이 없을 수 있다
		{
			INT32 i32TotalRespawnCount		= pMap->GetTotalRespawnObjectCount();
			DS_RESPAWN_OBJECT*	pRespawnObj = pMap->GetTotalRespawnObjectList();
			for( INT32 i = 0; i < i32TotalRespawnCount; i++, pRespawnObj++ )
			{
				NxVec3 pos = NxVec3( pRespawnObj->m_vPos[0], pRespawnObj->m_vPos[1], pRespawnObj->m_vPos[2] );
				if( ! m_pRespawnMgr->AddRespawnPosition( pRespawnObj->m_ui8RespawnStyle, pos ) )
				{
					I3ASSERT(0);
					DSERROR("respawn object setup error!\n");
					return FALSE;
				}
				
			}

			I3TRACE("RED  RESPAWN POSITION COUNT:%d\n", m_pRespawnMgr->GetRespawnCount(0));
			I3TRACE("BLUE RESPAWN POSITION COUNT:%d\n", m_pRespawnMgr->GetRespawnCount(1));
			I3TRACE("RED NPC RESPAWN POSITION COUNT:%d\n", m_pRespawnMgr->GetRespawnCount(2));
			I3TRACE("BLUE NPC RESPAWN POSITION COUNT:%d\n", m_pRespawnMgr->GetRespawnCount(3));
		}
		return TRUE;
	}
	
	return FALSE;
}


// --------------------------------------------------------------------------------
// 게임 캐릭터, 오브젝트 업데이트 
// --------------------------------------------------------------------------------

void	CDediRoom::UpdateReferenceTable( UINT32 ui32SlotIdx, UINT32 uInfoFlag, REAL32 r32PacketTime )
{
	m_cs.Lock();

	if( uInfoFlag & P2P_INFO_FLAG_HITBYCHARA )
		ProcessHits(ui32SlotIdx, GHBT_CHARA);

	if( uInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION )
		ProcessHits(ui32SlotIdx, GHBT_EXPLOSION);
	
	if( uInfoFlag & P2P_INFO_FLAG_HITBYOBJECT )
		ProcessHits(ui32SlotIdx, GHBT_OBJECT);

	if( uInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE )
		_Process_Suicide( ui32SlotIdx );
	
	if ( uInfoFlag & P2P_INFO_FLAG_MISSION ) 
		_Process_Mission( ui32SlotIdx, r32PacketTime );
	
	m_cs.Unlock();
}

INT32	CDediRoom::_FindListIdx( UINT8 ui8Type, INT32 i32NetIdx )
{
	INT32	i32Count = 0;
	UINT16* pNetIdx = NULL;
	switch( ui8Type )
	{
	case I3_NETWORK_GAMEOBJECT_TYPE_STATIC:
		{
			i32Count  = m_i32CountOfStatic;
			pNetIdx = &m_pNetIndex_Obj_Static[0];
			while ( i32Count-- )
			{
				if( *pNetIdx == i32NetIdx )
				{
					return (INT32)( pNetIdx - &m_pNetIndex_Obj_Static[0] );
				}
				pNetIdx++;
			} 
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_ANIM:
		{
			i32Count  =  m_i32CountOfAnim;
			pNetIdx   = &m_pNetIndex_Obj_Anim[0];
			while (i32Count--)
			{
				if( *pNetIdx == i32NetIdx )
				{
					return (INT32)( pNetIdx - &m_pNetIndex_Obj_Anim[0] );
				}
				pNetIdx++;
			}
		}
		break;
	default:
		break;
	}

	return -1;
}

void CDediRoom::RecalculateHitCountByCharacter(UINT32 ui32SlotIdx, INT32& i32HitCount)
{
	if( i32HitCount <= 1 )
		return;

	N_PCINFO_HIT_DEDICATED2* pHitByChar = m_pCastPacket_Chara_Hit_ByChara[ ui32SlotIdx ];
	N_PCINFO_HIT_DEFAULT* pHitDefault	= &pHitByChar->_tDefault;
	N_PCINFO_HIT_WEAPON* pHitWeapon		= &pHitByChar->_tWeapon;

	if (!pHitWeapon)
	{
		DSERROR("[ERROR][CDediRoom::RecalculateHitCountByCharacter] pHitWeapon Is NULL\n");
		i32HitCount =0;
		return;
	}

	WeaponInfo* pWeaponBase = Table::GetExtWeaponInfo( pHitWeapon->GetWeaponID() );

	if (!pWeaponBase)
	{
		DSERROR("[ERROR][CDediRoom::RecalculateHitCountByCharacter] Weapon Info Is NULL Class : %d Number : %d \n", pHitWeapon->GetWeaponClass(),pHitWeapon->GetWeaponNumber());
		i32HitCount =0;
		return;
	}

	if( pHitDefault->GetHitType() == GHT_CHARA && pWeaponBase->m_ui8FireBulletCount < i32HitCount )
	{

		I3TRACE("[ERROR][CDediRoom::RecalculateHitCountByCharacter] ui32HitCount = %d in hit by chara\n", i32HitCount);
		I3TRACE("Class : %d Number : %d \n", pHitWeapon->GetWeaponClass(),pHitWeapon->GetWeaponNumber());
		DSERROR("[ERROR][CDediRoom::RecalculateHitCountByCharacter] ui32HitCount = %d in hit by chara\n", i32HitCount);
		DSERROR("Class : %d Number : %d \n", pHitWeapon->GetWeaponClass(),pHitWeapon->GetWeaponNumber());
		i32HitCount =0;
		return;
	}


	WEAPON_CLASS_TYPE eWeaponClass = static_cast<WEAPON_CLASS_TYPE>( pHitWeapon->GetWeaponClass() );


	switch( eWeaponClass )
	{
	case WEAPON_CLASS_SHOTGUN:
	case WEAPON_CLASS_DUALSHOTGUN:
		{
			if( pHitDefault->GetHitType() == GHT_OBJECT )
				break;

			REAL32 rDistance = pHitByChar->GetDistance();

			i32HitCount = min(CalcShotgunHitCount(rDistance), i32HitCount);
		}

		break;
	case WEAPON_CLASS_SNIPER:
		{
			INT32 i32LastHitChar = 0xFF;
			INT32 i32TempHitCnt = 0;
			for( INT32 i = 0; i < i32HitCount; ++i )
			{
				N_PCINFO_HIT_DEFAULT* pHitDefault = &pHitByChar[i]._tDefault;
				if( pHitDefault->GetHitType() == GHT_CHARA )
				{
					if( pHitDefault->GetIndex() == i32LastHitChar ) 
						break;

					i32LastHitChar = pHitDefault->GetIndex();
				}

				i32TempHitCnt++;
			}

			i32HitCount = i32TempHitCnt;
		}
		break;
	default:

		break;
	}
}

void CDediRoom::RecalculateHitCountByExplosion(UINT32 ui32SlotIdx, INT32& i32HitCount)
{
	N_PCINFO_HIT_WEAPON* pHitWeapon = &m_pCastPacket_Chara_Hit_ByExplosion[ ui32SlotIdx ]->_stWeapon;

	switch( pHitWeapon->GetWeaponClass() )
	{
	case WEAPON_CLASS_THROWING_GRENADE:
		{
			if( !IsWPSmoke(pHitWeapon->GetWeaponClass()) )
				break;

			INT32 i32LastHitChar = 0xFF;
			INT32 i32TempHitCnt = 0;

			N_PCINFO_HIT_BYEXPLOSION* pHitExplosion = m_pCastPacket_Chara_Hit_ByExplosion[ ui32SlotIdx ];
			for( INT32 i = 0; i < i32HitCount; ++i )
			{
				N_PCINFO_HIT_DEFAULT* pHitDefault = &pHitExplosion[i]._tDefault;
				if( pHitDefault->GetHitType() == GHT_CHARA )
				{
					if( pHitDefault->GetIndex() == i32LastHitChar )
						break;

					i32LastHitChar = pHitDefault->GetIndex();
				}

				i32TempHitCnt++;
			}

			i32HitCount = i32TempHitCnt;
		}
		break;

	case WEAPON_CLASS_KNIFE:
	case WEAPON_CLASS_HANDGUN:
	case WEAPON_CLASS_ASSAULT:
	case WEAPON_CLASS_SMG:
	case WEAPON_CLASS_SNIPER:
	case WEAPON_CLASS_SHOTGUN:
	case WEAPON_CLASS_CIC:
	case WEAPON_CLASS_DUALHANDGUN:
	case WEAPON_CLASS_DUALKNIFE:
	case WEAPON_CLASS_OBJECT_GUN:
	case WEAPON_CLASS_DUALSMG:
	case WEAPON_CLASS_DUALSHOTGUN:
		{
			i32HitCount = 0;
			DSERROR("ERR. weapon class error in hit by explosion : weapon=%d:%d\n", pHitWeapon->GetWeaponClass(), pHitWeapon->GetWeaponNumber());
		}
		break;
	}
}

void CDediRoom::CheckHackByDeathInfo(UINT32 ui32SlotIdx, UINT32 ui32TargetIdx, N_PCINFO_HIT_DEDICATED2* pHitByChar)
{
	//월샷 오픈후 추후 치팅 감지 기능 추가하도록 합니다.
	//N_PCINFO_HIT_WEAPON* pHitWeapon = &pHitByChar->_tWeapon;
	/*
	// ----------------------------
	// 1. gravity
	// 추후 필요없음. 
	// ----------------------------
	if( GetUserAlive( ui32SlotIdx ) && !GetUserOnLoad( ui32SlotIdx ) )
	{
		if( _CheckGravityHack(ui32SlotIdx, &pHitByChar->_stFirePos) ) 
		{
			EnqueueKickUser( ui32SlotIdx, HACK_TYPE_GRAVITY );
			return;
		}
	}
	*/

	return; 
}

void CDediRoom::PacketDataReset( UINT32 ui32SlotIdx )
{
	m_CharCastPacket[ ui32SlotIdx ].ResetData();
}

void CDediRoom::ApplyCharacterDamage(UINT32 ui32SlotIdx, GAMEINFO_HIT_BYTYPE eHitByType, CHARA_DEATH_TYPE eDeathType, VEC3D v3dDirection, N_PCINFO_HIT_DEFAULT* pHitDefault, INT32 i32DamageResult,N_PCINFO_HIT_WEAPON* pHitWeapon, T_ItemID TItemID )
{
	UINT32 ui32TargetIdx = pHitDefault->GetIndex();
	if( !GetMember(ui32TargetIdx)->GetPlaying() )
		return;

	if( !GetUserAlive(ui32TargetIdx) )
		return;

	if( eHitByType == GHBT_CHARA && !GetUserAlive( ui32SlotIdx ) )
		return;

	if( pHitDefault->GetHelmet() & NET_HELMET_FLAG_PROTECT )
		return;

	UINT32 ui32P2PFlag = 0;
	switch( eHitByType )
	{
	case GHBT_CHARA:		ui32P2PFlag = P2P_INFO_FLAG_HITBYCHARA;			break;
	case GHBT_EXPLOSION:	ui32P2PFlag = P2P_INFO_FLAG_HITBYEXPLOSION;		break;
	case GHBT_OBJECT:		ui32P2PFlag = P2P_INFO_FLAG_HITBYOBJECT;		break;
	}

	if( pHitWeapon )
	{
		if( IsMedicalKit(pHitWeapon->GetWeaponClass()) )
		{
			m_pGameCharacters[ui32TargetIdx].AddHP(40);
			SendLife(ui32TargetIdx, ui32P2PFlag);
			return;
		}

		if( ui32SlotIdx != ui32TargetIdx && (ui32SlotIdx % TEAM_COUNT == ui32TargetIdx % TEAM_COUNT) )
			return;
	}

	if( IsEscapeMissionMode() || IsCrossCountMode() )
	{
		if( IsTRex(ui32TargetIdx) || IsTouchDown(ui32TargetIdx) )
			return;

		if( eHitByType == GHBT_EXPLOSION )
		{
			switch( eDeathType )
			{
			case CHARA_DEATH_BUFF:
				{
					if( IsElite(ui32TargetIdx) && ui32SlotIdx == ui32TargetIdx )
					{
						m_pGameCharacters[ ui32TargetIdx ].AddHP(30);
						SendLife(ui32TargetIdx, ui32P2PFlag);

						return;
					}
				}
				break;

			case CHARA_DEATH_HOWL:
				m_ar32HowlBeginTime[ui32TargetIdx] = GetGameTime();
				break;
			}
		}
	}

	if( ui32SlotIdx == ui32TargetIdx && pHitWeapon->GetWeaponID() == MAKE_ITEM_ID( ITEM_TYPE_PRIMARY, WEAPON_CLASS_DINO, 16 ) )
	{
		// 본인의 애시드 자폭스킬로 인한 사망
		m_pGameCharacters[ui32TargetIdx].SaveHP();
		SetUserDeath( ui32TargetIdx );

		GAMETOCONTEXT_INFO_CHARA_DEATH stDeath;
		stDeath.Reset();
		stDeath._pHitDir = &v3dDirection;

		INT32 assistidx = m_AssistInfo[ui32TargetIdx].GetAssistidx(ui32SlotIdx);

		_BuildDeathInfoByHit(&stDeath, ui32SlotIdx, pHitDefault, eDeathType, pHitWeapon, TItemID, assistidx);
		_Write_Chara_Death(&stDeath);
	}
	else if( m_pGameCharacters[ui32TargetIdx].SubHP(i32DamageResult) <= 0  )
	{
		// 체력이 0이되어 사망
		SetUserDeath( ui32TargetIdx );

		GAMETOCONTEXT_INFO_CHARA_DEATH stDeath;
		stDeath.Reset();
		stDeath._pHitDir = &v3dDirection;

		INT32 assistidx = m_AssistInfo[ui32TargetIdx].GetAssistidx(ui32SlotIdx);

		_BuildDeathInfoByHit(&stDeath, ui32SlotIdx, pHitDefault, eDeathType, pHitWeapon, TItemID, assistidx);
		_Write_Chara_Death(&stDeath);
		
		if( ui32SlotIdx != ui32TargetIdx )
			++m_TeamKillCount[ui32SlotIdx % TEAM_COUNT];
	}
	else
	{
		if( eHitByType == GHBT_EXPLOSION || eHitByType == GHBT_OBJECT )
		{
			N_PCINFO_SUFFER* pSuffering = &m_pCastPacket_Chara_Suffering[ ui32TargetIdx ];
			pSuffering->Reset();
			pSuffering->SetAttackerIndex( static_cast<UINT8>(ui32SlotIdx) );
			pSuffering->SetDeathType( static_cast<UINT8>(eDeathType) );
			pSuffering->SetHitPart( pHitDefault->GetHitPart() );
			pSuffering->SetNeedDamageMotion( 1 );

			getChara_Check(ui32TargetIdx)->nP2PInfoFlag |= P2P_INFO_FLAG_CS_SUFFERING;
		}

		if (ui32SlotIdx != ui32TargetIdx)
			m_AssistInfo[ui32TargetIdx].SetAssist(ui32SlotIdx, i32DamageResult); //자기 수류탄 데미지 제외.
		SendLife(ui32TargetIdx, ui32P2PFlag);
	}	
}

void CDediRoom::ApplyObjectDamage(UINT32 ui32SlotIdx, N_PCINFO_HIT_DEFAULT* pHitDefault,INT32 i32DamageResult,N_PCINFO_HIT_WEAPON* pHitWeapon)
{
	UINT32 ui32TargetIdx = pHitDefault->GetIndex();
	if( !IS_VALID_GAME_OBJECT(ui32TargetIdx) )
	{
		DSERROR("[ERROR][CDediRoom::ApplyDamageByObject] Invalide target index. (Target Index : %d)\n", ui32TargetIdx);
		return; 
	}

	INT32 i32NetIndex = _FindListIdx(pHitDefault->GetHitPart(), ui32TargetIdx);
	if( i32NetIndex != INVALID_OBJ_IDX )
	{
		UINT8 ui8ExplosionState = m_aGameObjects[ui32TargetIdx].getState();
		INT16 i16Damage = min((INT16)i32DamageResult, m_aGameObjects[ui32TargetIdx].getHP());
		if( 0 < i16Damage && m_aGameObjects[ui32TargetIdx].SubHP(i16Damage) <= 0 )
		{
			if( m_aGameObjects[ui32TargetIdx].m_iSType == OBJECT_S_TYPE_EXPLOSIVE_OBJ )
			{
				if( pHitDefault->GetHitbyObjectExplosion() != 0 )
				{
					for( INT32 i = 0; i < SLOT_MAX_COUNT; ++i )
					{
						if( (pHitDefault->GetHitbyObjectExplosion() >> i) & 0x01 )
						{
							if( !GetUserAlive( i ) ) continue;

							if( _CheckObjectExplosionRange(ui32TargetIdx, i) )
								continue;

							DeathObjectExplosion(ui32SlotIdx, i, ui32TargetIdx, pHitWeapon);
						}
					}
				}
			}
		}

		if( IsDestroyMissionMode() )
		{
			if( !m_DestructionData.m_pObjectRed || !m_DestructionData.m_pObjectBlue )
			{
				DSERROR("m_pObjectRed or m_pObjectBlue is NULL. Revoke Room. StageID:%d\n", m_ui32StageID);
				SetGroupState( RGS_READY ); 
				return;
			}

			if( ui32TargetIdx == static_cast<UINT32>( m_DestructionData.m_pObjectBlue->m_i16NetIdx) || ui32TargetIdx == static_cast<UINT32>( m_DestructionData.m_pObjectRed->m_i16NetIdx) )
			{
				m_DestructionData.m_ObjectSendDamageInfo._nBlueObjectHP = m_DestructionData.m_pObjectBlue->getHP();
				m_DestructionData.m_ObjectSendDamageInfo._nRedObjectHP  = m_DestructionData.m_pObjectRed->getHP();
				m_DestructionData.m_ObjectSendDamageInfo._nUserDamage[ui32SlotIdx] += i16Damage;

				m_ar32AttackBeginTime[ui32SlotIdx] = GetGameTime();
				m_ui32TcpSendFlag |= DS_SEND_MISSION_DESTRUCTION;
			}
		}

		switch( pHitDefault->GetHitPart() )
		{
		case I3_NETWORK_GAMEOBJECT_TYPE_STATIC:
			{
				m_aGameObjects[ui32TargetIdx].SetEventUser( (UINT8)ui32SlotIdx );
			}
			break;

		case I3_NETWORK_GAMEOBJECT_TYPE_ANIM:
			{
				if( isHelicopter(m_aGameObjects[ui32TargetIdx].GetSType()) )
				{
					//반파 상태에 대한 데미지 설정
					//헬리스팟의 경우 반파시 헬기 내 캐릭터 사망!  아웃포스트의 경우 반파시 데미지 없이 완파시 사망
					if( (DS_EXPLOSION_STATE_HALF_EXPLOSION == m_aGameObjects[ui32TargetIdx].getState()) && (ui8ExplosionState != m_aGameObjects[ui32TargetIdx].getState()) )
					{
						if(m_aGameObjects[ui32TargetIdx].GetSType() != OBJECT_S_TYPE_RIDING_HELICOPTER)
						{
							ProcessHelicopterExplosion( ui32SlotIdx, ui32TargetIdx, pHitWeapon );
						}
					}
					//완파 상태에 대한 데미지설정
					//완파 상태일 경우 헬기탑승유저 모두 사망
					if ( m_aGameObjects[ui32TargetIdx].getHP() == 0)
					{
						ProcessHelicopterExplosion( ui32SlotIdx, ui32TargetIdx, pHitWeapon );
					}

				}

				m_aGameObjects[ui32TargetIdx].SetEventUser( (UINT8)ui32SlotIdx );
			}
			break;
		}
	}
	else
	{
		if( !IsDefenceMissionMode() )
			return;

		if( m_DefenceData.getDefenceObj(0) == NULL || m_DefenceData.getDefenceObj(1) == NULL )
		{
			SetGroupState( RGS_READY );

			DSERROR("getDefenceObj(0) or getDefenceObj(1) is NULL. Revoke Room. StageID:%d\n", m_ui32StageID );
			return;
		}

		for( INT32 i = 0; i < MAX_OBJECT_COUNT; ++i )
		{
			if( static_cast<UINT32>(m_DefenceData.getDefenceObj(i)->m_i16NetIdx) != ui32TargetIdx )
				continue;

			INT16 i16Damage = min((INT16)i32DamageResult, m_DefenceData.getDefenceObj(i)->getHP());
			if( i16Damage <= 0 )
				return;

			m_DefenceData.getSendDamageInfo()->setUserDamage(i, ui32SlotIdx, m_DefenceData.getSendDamageInfo()->getUserDamage(i, ui32SlotIdx) + i16Damage);
			m_DefenceData.getSendDamageInfo()->setObjectHP(i, m_DefenceData.getDefenceObj(i)->SubHP(i16Damage));
			m_DefenceData.getSendDamageInfo()->setObjectHP(i == 0 ? 1 : 0, m_DefenceData.getDefenceObj(i == 0 ? 1 : 0)->getHP());

			m_ar32AttackBeginTime[ ui32SlotIdx ] = GetGameTime();
			m_ui32TcpSendFlag |= DS_SEND_MISSION_DEFENCE;
		}
	}
}

INT32 CDediRoom::CalculateDamageCharacter(UINT32 ui32SlotIdx, REAL32 r32Distance, N_PCINFO_HIT_DEFAULT* pHitDefault, N_PCINFO_HIT_WEAPON* pWeapon, CHARA_DEATH_TYPE eDeathType, REAL32* pResult)
{
	WeaponInfo* pWeaponBase = Table::GetExtWeaponInfo( pWeapon->GetWeaponID() );

	if( !pWeaponBase || !pWeaponBase->IsValid() )
	{
		DSERROR("[ERROR][CDediRoom::CalculateDamage] Not found weapon data. (Class : %d, Number : %d)\n", pWeapon->GetWeaponClass(), pWeapon->GetWeaponNumber());
		return 0;
	}

	UINT32 ui32TartgetIdx = pHitDefault->GetIndex();
	if( !IS_VALID_MEMBER_RANGE(ui32TartgetIdx) )
	{
		DSERROR("[ERROR][CDediRoom::CalculateDamage] Invalid index. (Char Index : %d)\n", pHitDefault->GetIndex());
		return 0;
	}

	CCharacter* pAttackChar = &m_pGameCharacters[ui32SlotIdx];
	if( !m_pDediMember[ui32SlotIdx].GetPlaying() )
		return 0;

	CCharacter* pTargetChar = &m_pGameCharacters[ui32TartgetIdx];
	if( !m_pDediMember[ui32TartgetIdx].GetPlaying() )
		return 0;
	
	//오브젝트탑승시 로컬좌표로 계산되기때문에
	//거리 체크를 할 수 없어 임의로 거리 계산
	//추후 수정
	if (pTargetChar->GetOnLoad())
		r32Distance =0.1f;
	
	REAL32 r32WeaponRange = (3.402823466E+38);

	if( FALSE == Table::IsThrowWeapon( pWeapon ) )
	{
		r32WeaponRange = pWeaponBase->GetWeaponRange( pWeapon->IsExtension(), (WEAPON::ATTACK_TYPE)pWeapon->GetAttackType() );
		r32WeaponRange *= (1.0f + pAttackChar->GetRangeRate())  ;

		// ADDITIONAL_RANGE 1.0f 
		// Hit 계산시 발 밑을 중심으로 거리가 계산되지만 실제 히트는 몸에 맞은 위치이므로 무기 Range보다 높은 값이 들어올 수 있어
		// 보정 범위까지 허용하도록 한다
		if(pWeapon->GetWeaponClass() == WEAPON_CLASS_DINO)
		{
			//공룡의 경우(물기공격시) 히트하는 공격체가 앞으로 다가가기 때문에 실제 범위보다 먼위치를 문다 
			if( r32WeaponRange + ADDITIONAL_RANGE_DINO < r32Distance )
			{
				return 0;
			}
		}
		else if( r32WeaponRange + ADDITIONAL_RANGE < r32Distance )
		{
			return 0;
		}
	}

	// 크리티컬 히트 여부 확인

	if (pAttackChar->IsAbleToCritialWeapon(pWeaponBase->m_ui8CriticalChanceRange))
	{
		pAttackChar->IncHitCount();

		if( pHitDefault->GetCritical() )
		{
			if( !pAttackChar->IsAbleToCritialHit(pWeaponBase->m_ui8CriticalChanceRange) )
				return 0;

			pAttackChar->IncCritialHitCount();
		}
	}

	// 타격 캐릭터 추가 데미지 (장비)
	REAL32 r32IncreasePlus = pAttackChar->GetDamageIncPlus(pWeaponBase->GetWeaponUsage(), pWeaponBase->GetWeaponClass(), m_aui8PBVer[ui32SlotIdx]);

	// 무기 데미지(무기 기본 데미지 + 랜덤 데미지 + 크리티컬 데미지)
	REAL32 r32WeaponDamage = static_cast<REAL32>( pWeaponBase->GetWeaponDamage( pHitDefault->GetCritical(), pWeapon->IsExtension(), static_cast<WEAPON::ATTACK_TYPE>(pWeapon->GetAttackType()), r32IncreasePlus, m_aui8PBVer[ui32SlotIdx]) );

	// 거리별 데미지 감소율
	REAL32 r32DPDRate = pWeaponBase->GetDPDRate( pWeapon->IsExtension(), static_cast<WEAPON::ATTACK_TYPE>(pWeapon->GetAttackType()) ) * ( 1.0f + GetDesigDFDRate(ui32SlotIdx) );
	REAL32 r32DistanceRate = 1.0f - (r32Distance / r32WeaponRange) * min(1.0f, r32DPDRate );

	// 타격 캐릭터 데미지 증가율(장비, 아이템, 스킬 or 호칭)
	REAL32 r32IncreaseRate = pAttackChar->GetDamageIncRate(pWeaponBase->GetWeaponUsage(), pWeaponBase->GetWeaponClass(), m_aui8PBVer[ui32SlotIdx] );
	
	// 피격 캐릭터 데미지 감소율(장비, 스킬 or 호칭)
	REAL32 r32ReduceRate = pTargetChar->GetDefenceRate( NetIdxToHitPart(pHitDefault->GetHitPart()), m_aui8PBVer[ui32TartgetIdx] );

	// 부위별 데미지 배율
	REAL32 r32HitPartRate = pTargetChar->GetDamageRateByHitPart( NetIdxToHitPart(pHitDefault->GetHitPart()) );

	// 헤드 헌터 일경우 부위별 데미지 배율 변경
	SetHeadHunterHitPartRate(eDeathType, pHitDefault, &r32HitPartRate);
	SetChaosHitPartRate(&r32HitPartRate);

	REAL32 r32FinalDamage = static_cast<INT32>( r32WeaponDamage * r32IncreaseRate * r32DistanceRate * r32ReduceRate + 0.5f ) * r32HitPartRate;		// +0.5f 는 소수점 첫째자리 반올림 처리

	// 아래 Result 관련 코드는 테스트 로그용이므로 테스트 완료 후 삭제
	pResult[0] = r32WeaponDamage;
	pResult[1] = r32IncreaseRate;
	pResult[2] = r32DistanceRate;
	pResult[3] = r32ReduceRate;
	pResult[4] = r32HitPartRate;

	if( IsHeadHunterMode( eDeathType ) == TRUE ) r32FinalDamage = static_cast<INT32>( min(1.0f, r32FinalDamage) );

	return static_cast<INT32>( max(1.0f, r32FinalDamage) );
}


INT32 CDediRoom::CalculateDamageObject(UINT32 ui32SlotIdx, REAL32 r32Distance, N_PCINFO_HIT_DEFAULT* pHitDefault, N_PCINFO_HIT_WEAPON* pWeapon, REAL32* pResult)
{
	WeaponInfo* pWeaponBase = Table::GetExtWeaponInfo( pWeapon->GetWeaponID() );

	if( !pWeaponBase || !pWeaponBase->IsValid() )
	{
		DSERROR("[ERROR][CDediRoom::CalculateDamage] Not found weapon data. (Class : %d, Number : %d)\n", pWeapon->GetWeaponClass(), pWeapon->GetWeaponNumber());
		return 0;
	}

	UINT32 ui32TartgetIdx = pHitDefault->GetIndex();
	if( !IS_VALID_GAME_OBJECT(ui32TartgetIdx) )
	{
		DSERROR("[ERROR][CDediRoom::CalculateDamage] Invalid index. (Char Index : %d)\n", pHitDefault->GetIndex());
		return 0;
	}

	CCharacter* pAttackChar = &m_pGameCharacters[ui32SlotIdx];
	if( !m_pDediMember[ui32SlotIdx].GetPlaying() )
	{
		DSERROR("[ERROR][CDediRoom::CalculateDamage] not Playing %d \n", ui32SlotIdx);
		return 0;
	}
	CDSObject* pTargetObject = &m_aGameObjects[ui32TartgetIdx];
	if( pTargetObject->getState() == DS_EXPLOSION_STATE_EXPLOSION )
 	{
// 		HACK_LOG hi;
// 		hi.AddInfo("[ERROR][CDediRoom::CalculateDamage]\t DS_EXPLOSION_STATE_EXPLOSION\n");
// 
// 		char* msg = hi.MakeLogString(this,ui32SlotIdx,HACK_TYPE_FIRE_SPEED);
// 		if( msg) DSERROR(msg);
		return 0;
	}

	if (pAttackChar->GetOnLoad())
		r32Distance =1.0f;

	REAL32 r32WeaponRange = pWeaponBase->GetWeaponRange( pWeapon->IsExtension(), static_cast<WEAPON::ATTACK_TYPE>(pWeapon->GetAttackType()) ) * (1.0f + pAttackChar->GetRangeRate());
	// ADDITIONAL_RANGE 1.0f 
	// Hit 계산시 발 밑을 중심으로 거리가 계산되지만 실제 히트는 몸에 맞은 위치이므로 무기 Range보다 높은 값이 들어올 수 있어
	// 보정 범위까지 허용하도록 한다
	if( r32WeaponRange + ADDITIONAL_RANGE < r32Distance )
	{
		I3TRACE("[ERROR][CDediRoom::CalculateDamage] Out of range. (Range : %f, Distance : %f)\n", r32WeaponRange, r32Distance);
		return 0;
	}

	// 타격 캐릭터 추가 데미지 (장비)
	REAL32 r32IncreasePlus = pAttackChar->GetDamageIncPlus(pWeaponBase->GetWeaponUsage(), pWeaponBase->GetWeaponClass(), m_aui8PBVer[ui32SlotIdx]);

	// 무기 데미지(무기 기본 데미지 + 랜덤 데미지 + 크리티컬 데미지)
	REAL32 r32WeaponDamage = static_cast<REAL32>( pWeaponBase->GetWeaponDamage( pHitDefault->GetCritical(), pWeapon->IsExtension(), static_cast<WEAPON::ATTACK_TYPE>(pWeapon->GetAttackType()), r32IncreasePlus, m_aui8PBVer[ui32SlotIdx]) );

	// 타격 캐릭터 데미지 증가율(장비, 아이템, 스킬 or 호칭)
	REAL32 r32DamageIncreaseRate = (1.0f + pAttackChar->GetDamageIncRate(pWeaponBase->GetWeaponUsage(), pWeaponBase->GetWeaponClass(), m_aui8PBVer[ui32SlotIdx]) );

	REAL32 r32Damage = r32WeaponDamage * r32DamageIncreaseRate + 0.5f;		// +0.5f 는 소수점 첫째자리 반올림 처리

	// 테스트용 로그. 테스트 완료 후 삭제
	pResult[0] = r32WeaponDamage;
	pResult[1] = r32DamageIncreaseRate;

	return static_cast<INT32>( max(1.0f, r32Damage) );
}

void		CDediRoom::SetHeadHunterHitPartRate(CHARA_DEATH_TYPE eDeathType, N_PCINFO_HIT_DEFAULT* pHitDefault, REAL32* r32HitPartRate)
{
	if( TRUE			!= IsHeadHunterMode( eDeathType ) ) return;

	CHARA_HIT_PART eCharHitPart = NetIdxToHitPart(pHitDefault->GetHitPart());

	if( CHARA_HIT_HEAD		== eCharHitPart ) return;
	if( CHARA_HIT_HEAD_OLD	== eCharHitPart ) return; 
	if( CHARA_HIT_JAW		== eCharHitPart ) return;
	
	*r32HitPartRate = HEAD_HUNTER_HIT_PARTS_NOT_HEAD_RATE;

}

void		CDediRoom::SetChaosHitPartRate(REAL32* r32HitPartRate)
{
	if (TRUE != IsChaosMode()) return;

	*r32HitPartRate = CHAOS_HIT_PARTS_HEAD_RATE;

}

void CDediRoom::ProcessHits(UINT32 ui32SlotIdx, GAMEINFO_HIT_BYTYPE eHitByType)
{
	if( IsHackUser(ui32SlotIdx) )
		return;

	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return;

	if( !m_pDediMember[ui32SlotIdx].GetPlaying() )
		return;

	INT32 i32HitCount = GetHitCount(eHitByType, ui32SlotIdx);
	if( i32HitCount <= 0 || MAX_FIREHIT_COUNT <= i32HitCount )
		return;
	
#if defined (DEDI_HITCOUNT)
	
	switch(eHitByType)
	{
	case GHBT_CHARA:
		RecalculateHitCountByCharacter(ui32SlotIdx,i32HitCount);
		break;
	case GHBT_EXPLOSION:
		//RecalculateHitCountByExplosion(ui32SlotIdx,i32HitCount);
		break;
	default:
		I3TRACE("Object Hit \n");
		break;
	}

#endif
	
	for( INT32 i = 0; i < i32HitCount; ++i )
	{
		N_PCINFO_HIT_DEFAULT* pHitDefault = GetHitDefault(eHitByType, ui32SlotIdx, i);
		if( !pHitDefault )
			continue;

		REAL32 r32Distance = GetDistance(eHitByType, ui32SlotIdx, i, &m_pCastPacket_Chara_PosRot[pHitDefault->GetIndex()]._Pos,pHitDefault);
		INT32 i32ClientDamage = static_cast<INT32>( pHitDefault->GetDamage() );

		N_PCINFO_HIT_WEAPON* pHitWeapon = GetHitWeapon(eHitByType, ui32SlotIdx, i);
		//I3TRACE( "Hit Weapon ItemID = %d\n",pHitWeapon->GetWeaponID() );

		// 테스트용 변수, 테스트 완료 후 삭제
		REAL32 ar32Result[5] = {0.0f, };
		INT32 i32ServerDamage = 0;
		INT32 i32DamageResult = 0;
		i32DamageResult = i32ClientDamage;

		switch( pHitDefault->GetHitType() )
		{
		case GHT_CHARA:
			{
				if( NetIdxToHitPart(pHitDefault->GetHitPart()) <= CHARA_HIT_UNKNOWN || CHARA_HIT_MAXCOUNT <= NetIdxToHitPart(pHitDefault->GetHitPart()) )
				{
					DSERROR("[ERROR][CDediRoom::ProcessHits] Invalid hit part. (Hit Part : %d)\n", NetIdxToHitPart(pHitDefault->GetHitPart()));
					continue;
				}

				CHARA_DEATH_TYPE eDeathType = GetDeathType(eHitByType, ui32SlotIdx, i);

				i32ServerDamage = CalculateDamageCharacter(ui32SlotIdx, r32Distance, pHitDefault, pHitWeapon, eDeathType, ar32Result);

				T_ItemID TItemID = pHitWeapon->GetWeaponID();

				WeaponInfo* pWeaponBase = Table::GetExtWeaponInfo( pHitWeapon->GetWeaponID() );

				if( i32ServerDamage < i32ClientDamage )
				{
					if (pWeaponBase && !pWeaponBase->UseRandomDamage())
					{
						if ( abs(i32ClientDamage - i32ServerDamage) > 20 )
							DSERROR("[Damage] Client Damage : %d\t Final Damage : %d\t Weapon Damage : %f\t Increase Rate : %f\t Distance Rate : %f\t Reduce Rate : %f\t Part Rate : %f\n", i32ClientDamage, i32ServerDamage, ar32Result[0], ar32Result[1], ar32Result[2], ar32Result[3], ar32Result[4]);
					}
					I3TRACE("[Damage] Client Damage : %d\t Final Damage : %d\t Weapon Damage : %f\t Increase Rate : %f\t Distance Rate : %f\t Reduce Rate : %f\t Part Rate : %f\n", i32ClientDamage, i32ServerDamage, ar32Result[0], ar32Result[1], ar32Result[2], ar32Result[3], ar32Result[4]);

					//서버데미지 적용
					i32DamageResult = i32ServerDamage;
				}

				// 클라이언트에서 사용한 무기 본체의 ItemID가 아닌 듀얼무기, 유탄 등의 ItemID를 보내는 문제가 있어 처리한 코드 입니다. ( ex : 듀얼 크리스, RPG7... ) 
				//TItemID = pWeaponBase->GetAttackItemID( pHitWeapon->IsExtension() );

				ApplyCharacterDamage(ui32SlotIdx, eHitByType, eDeathType, GetHitDirection(eHitByType, ui32SlotIdx, i), pHitDefault, i32DamageResult, pHitWeapon, TItemID);
			}
			break;

		case GHT_OBJECT:
			{
				i32ServerDamage = CalculateDamageObject(ui32SlotIdx, r32Distance, pHitDefault, pHitWeapon, ar32Result);
				if( i32ServerDamage < i32ClientDamage )
				{
					if ( i32ClientDamage - i32ServerDamage > 10 && i32ServerDamage != 0)
					{
						if ( abs(i32ClientDamage - i32ServerDamage) > 20 )
							DSERROR("[Damage] Client Damage : %d\t Final Damage : %d\t Weapon Damage : %f\t Increase Rate : %f\n", i32ClientDamage, i32ServerDamage, ar32Result[0], ar32Result[1]);
						//서버데미지 적용
					}
					i32DamageResult = i32ServerDamage;
				}

				ApplyObjectDamage(ui32SlotIdx, pHitDefault,i32DamageResult, pHitWeapon);
			}			
			break;
		}
	}

	if( eHitByType == GHBT_EXPLOSION )
	{
		N_PCINFO_HIT_BYEXPLOSION* pHit = m_pCastPacket_Chara_Hit_ByExplosion[ ui32SlotIdx ];
		SetLastExplosionNetIdx(ui32SlotIdx, pHit->GetNetSlotIdx());
	}
	else if( eHitByType == GHBT_OBJECT )
	{
		N_PCINFO_HIT_BYOBJECT* pHit = m_pCastPacket_Chara_Hit_ByObject[ ui32SlotIdx ];
		INT32 i32ByObjectIdx = pHit->GetByObjectIndex();
		if( m_i32GameObjectCount <= i32ByObjectIdx )
			return;

		m_aGameObjects[i32ByObjectIdx].setState( DS_EXPLOSION_STATE_DAMAGE_APPLIED );
	}
}

void	CDediRoom::_Process_Suicide( UINT32 ui32SlotIdx )
{
	if( ! IS_VALID_MEMBER_RANGE(ui32SlotIdx) ) return;

	CCharacter * pChara = GetGameCharacter( ui32SlotIdx );

	if( NULL == pChara )	return;

	// 공룡전 예외처리
	if ( (UINT16)pChara->GetHP() == (UINT16)INFINIY_HP ) return;

	// 이미 죽은 캐릭터에 대한 처리는 하지 않는다.
	if( !pChara->GetAlive() ) return;
		
	GAMEINFO_SUICIDEDAMAGE* pSuicideDamage = &m_CharCastPacket[ ui32SlotIdx ].m_SuicideDamage;

	GAMETOCONTEXT_INFO_CHARA_DEATH deathInfo;
	deathInfo.Reset();
		
	// 패킷 하나에 들어 있는 모든 데미지 누적
	for(INT32 i = 0; i < pSuicideDamage->getCount(); i++ )
	{	
		// HP 감소
		if( pChara->SubHP( pSuicideDamage->getDamage(i) ) <= 0 )
		{
			SetUserDeath( ui32SlotIdx );		

			// deathInfo init. 멤버 변수 스코프 주의
			deathInfo.Reset();
			deathInfo._pHitDir			= (VEC3D*)pSuicideDamage->getHitDirection(i);					// 스코프 주의 : _pHitDir은 pDeathInfo과 scope가 같아야 함
			// index
			deathInfo._iKillidx			= (INT32)ui32SlotIdx;
			deathInfo._iDeathidx		= (INT32)ui32SlotIdx;
			deathInfo._ui32WeaponID		= pSuicideDamage->getWeaponID(i);
			// death type
			deathInfo._nDeathType		= (CHARA_DEATH_TYPE) pSuicideDamage->getDeathType(i);
			deathInfo._nHitPart			= (CHARA_HIT_PART)	 pSuicideDamage->getHitPart(i);		// net info
			deathInfo._iByObjectIdx		= pSuicideDamage->getKillerIndex(i);
			
			INT32 assistidx = m_AssistInfo[ui32SlotIdx].GetAssistidx(ui32SlotIdx);
			deathInfo._iAssistIdx = assistidx;
		
			_Write_Chara_Death( &deathInfo ); // 내부에서 tcp와 udp 처리 (death, life 0 추가)

			return;
		}
	} // for

	// 캐릭터가 살아 있는 경우 iLife와 suffering 추가
	// suffering은 자살시 사운드 이펙트 때문에 예외처리한 것임
	{
		// merge life
		getChara_Life( ui32SlotIdx )->iLife = pChara->GetHP();
		
		// merge suffering
		N_PCINFO_SUFFER* pSuffering = &m_pCastPacket_Chara_Suffering[ ui32SlotIdx ];

		pSuffering->Reset();
		pSuffering->SetAttackerIndex( (UINT8)ui32SlotIdx );
		pSuffering->SetDeathType( pSuicideDamage->getDeathType(0) );
		pSuffering->SetHitPart( pSuicideDamage->getHitPart(0) ); // 변환 없음
		pSuffering->SetNeedDamageMotion( 0 );

		// check flag
		getChara_Check( ui32SlotIdx )->nP2PInfoFlag |= P2P_INFO_FLAG_CS_SUFFERING;
		getChara_Check( ui32SlotIdx )->bCheckBroadcast = true;

		SendLife( ui32SlotIdx, P2P_INFO_FLAG_SUICIDEDAMAGE );
	}
}
void	CDediRoom::_Process_Mission( UINT32 ui32SlotIdx, REAL32 r32PacketTime )
{
	if( !IsBombMissionMode() ) return;

	//N_PCINFO_MISSION* pMission = &m_pCastPacket_Chara_Mission[ ui32SlotIdx ];

	GAMEINFO_MISSION* pMission = &m_CharCastPacket[ ui32SlotIdx ].m_Mission;

	m_MissionState =  pMission->GetState();
	// 폭파 미션에서 c4에 대한 sync.
	// 설치중, 설치완료, 설치실패, 해체중, 해체완료, 해체 실패
	// 설치시 캐릭터 포지션이 폭탄 포지션임.

	switch ( m_MissionState )
	{
	case NET_MISSION_TRIGGER_ARM_START:
		{
			if( m_r32TriggerTime[ui32SlotIdx] == 0.0f )
			{
				m_r32TriggerTime[ui32SlotIdx] = pMission->GetStartTime();
				break;
			}
			else
			{
				if( m_r32TriggerTime[ui32SlotIdx] != pMission->GetStartTime() )
				{
					pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
					m_MissionState =  pMission->GetState();
					m_r32TriggerTime[ui32SlotIdx] = 0.0f;
					break;
				}
				else
				{
					UINT32 ui32SlotItem = GetGameCharacter( ui32SlotIdx )->GetSlotItem();
					REAL32 r32FinishTime = m_r32BombInstallDelayTime;

					// C4 스피드 킷 아이템이 있을 경우 ( 5:5 모드인 경우 적용되지 않습니다 - 2015.02.26 임준현 )
					if( ( m_ui8MaxPerson != 10 ) &&
						( ui32SlotItem & SLOT_ITEM_C4_SPEED_KIT ) )		r32FinishTime += VALUE_CASH_ITEM_C4SPEEDKIT_INSTALL;

					// 미션 값을 완료로 변경 합니다. m_bArmDone 값은 Process_Mission() 에서 변경함.
					if( r32PacketTime > r32FinishTime + pMission->GetStartTime() )
					{
						I3TRACE("[%d] ARM DONE PacketTime : %2f , StartTime : %2f , FinishTime : %2f\n", ui32SlotIdx,r32PacketTime,pMission->GetStartTime(), r32FinishTime );
						pMission->SetState( NET_MISSION_TRIGGER_ARM_DONE );
						m_MissionState = pMission->GetState();
					}
					else break;
				}
			}
		}
		//break; // 주석이 맞음
	case NET_MISSION_TRIGGER_ARM_DONE:
		{
			if( m_bArmDone == FALSE )
			{
				if( m_r32TriggerTime[ui32SlotIdx] != pMission->GetStartTime() )	return;

				m_r32BombInstallTime = m_r32BattleTime;
				m_MissionArea  = pMission->GetArea();

				m_bArmDone	  = TRUE;	// 해체 된 후에는 라운드가 끝난다 : 여러번 설치 패킷을 날리지 않기 위함
				m_MissionIntruder.setInstallMode( 1 );
				m_MissionIntruder.setInstallerIndex( (INT32)ui32SlotIdx );
				VEC3D vecTemp;
				UDP_UTIL::ConvertNet3DtoVec3D( &vecTemp, &getChara_PosRot(ui32SlotIdx)->_Pos );
				m_MissionIntruder.setInstallPosition( &vecTemp );

				m_ui32TcpSendFlag |= DS_SEND_MISSION_TRIGGER;

				I3TRACE("### BOMB INSTALLED : slot=%d, state=%d, area=%d\n", ui32SlotIdx, m_MissionState, m_MissionArea );

				i3mem::FillZero( m_r32TriggerTime, sizeof(m_r32TriggerTime));

				if( (STAGE_UID_STORMTUBE == GetStageUID()) &&
					(STAGE_MODE_BOMB == StageID::GetStageMode(GetStageID())) &&
					(BOMB_AREA_B == m_MissionArea) )
				{
					m_pRepairTrain->setPathKey( 1 );
					m_pRepairTrain->m_bPathKeyChanged	= TRUE;
				}
			}
		}
		break;
	case NET_MISSION_TRIGGER_DISARM_START:
		{
			if( m_r32TriggerTime[ui32SlotIdx] == 0.0f )
			{
				m_r32TriggerTime[ui32SlotIdx] = pMission->GetStartTime();
				break;
			}
			else
			{
				if( m_r32TriggerTime[ui32SlotIdx] != pMission->GetStartTime() )
				{
					pMission->SetState( NET_MISSION_TRIGGER_ARM_CANCEL );
					m_MissionState = pMission->GetState();
					m_r32TriggerTime[ui32SlotIdx] = 0.0f;
					break;
				}
				else
				{
					UINT32 ui32SlotItem = GetGameCharacter( ui32SlotIdx )->GetSlotItem();
					REAL32 r32FinishTime = m_r32BombUnInstallDelayTime;

					// C4 스피드 킷 아이템이 있을 경우 ( 5:5 모드인 경우 적용되지 않습니다 - 2015.02.26 임준현 )
					if( ( m_ui8MaxPerson != 10 ) &&
						( ui32SlotItem & SLOT_ITEM_C4_SPEED_KIT ) )		r32FinishTime += VALUE_CASH_ITEM_C4SPEEDKIT_DISARM;

					// 미션 값을 완료로 변경 합니다. m_bArmDone 값은 Process_Mission() 에서 변경함.
					if( r32PacketTime > r32FinishTime + pMission->GetStartTime() )
					{
						I3TRACE("[%d] ARM DONE PacketTime : %2f , StartTime : %2f , FinishTime : %2f\n", ui32SlotIdx,r32PacketTime,pMission->GetStartTime(), r32FinishTime );
						pMission->SetState( NET_MISSION_TRIGGER_DISARM_DONE );						
						m_MissionState = pMission->GetState();
					}
					else break;
				}
			}
		}
		//break; // 주석이 맞음
	case NET_MISSION_TRIGGER_DISARM_DONE:
		{			
			if( m_bArmDone )
			{
				if( m_r32TriggerTime[ui32SlotIdx] != pMission->GetStartTime() )	return;

				m_MissionIntruder.setInstallMode( 0 );
				m_MissionIntruder.setInstallerIndex( (INT32)ui32SlotIdx );
				VEC3D vecTemp;
				UDP_UTIL::ConvertNet3DtoVec3D( &vecTemp, &getChara_PosRot(ui32SlotIdx)->_Pos );
				m_MissionIntruder.setInstallPosition( &vecTemp );

				m_ui32TcpSendFlag |= DS_SEND_MISSION_TRIGGER;

				I3TRACE("### BOMB UNINSTALLED : slot=%d, state=%d, area=%d\n", ui32SlotIdx, m_MissionState, m_MissionArea );

				i3mem::FillZero( m_r32TriggerTime, sizeof(m_r32TriggerTime));
			}
		}
		break;
	case NET_MISSION_TRIGGER_ARM_CANCEL:
	case NET_MISSION_TRIGGER_DISARM_CANCEL:
		{
			m_r32TriggerTime[ui32SlotIdx] = 0.0f;
		}
		break;
	case NET_MISSION_TRIGGER_ARM_FAIL:
	case NET_MISSION_TRIGGER_DISARM_FAIL:
		{
			m_r32TriggerTime[ui32SlotIdx] = 0.0f;
		}
		break;
	default:
		{
			return;
		}
		break;
	}; // switch
}

// 자살 패킷 처리시 캐릭터가 죽지 않았다면 hp 동기화를 위해 life를 여러번 보낸다.
void	CDediRoom::SendLifeForSync()
{
	CCharacter* pChara = NULL;
	
	for (UINT32 ui32SlotIdx = 0; ui32SlotIdx < SLOT_MAX_COUNT; ui32SlotIdx++)
	{
		if( GetBit( m_ui32CurrentUsers, ui32SlotIdx ) )
		{
			pChara = GetGameCharacter(ui32SlotIdx);
			
			if( pChara->GetHP() > 0 && _GetSendLife(ui32SlotIdx) > 0 )
			{
				N_PCINFO_LIFE* pLife = getChara_Life( ui32SlotIdx );	
				pLife->iLife = pChara->GetHP();
				getChara_Check( ui32SlotIdx )->nP2PInfoFlag |= P2P_INFO_FLAG_CS_LIFE; 
				getChara_Check( ui32SlotIdx )->bCheckBroadcast = true;

				_DecSendLife( ui32SlotIdx );
			}
			else
			{
				_ResetSendLife( ui32SlotIdx ); // death 1초후에 화면에 max hp 나오는 것 방지
			}
		} // if current users
	} // for
}

// --------------------------------------------------------------------------------
// 오브젝트 동기화 : 시간 단위 동기화가 필요한 오브젝트는 여기서 동기화 시킵니다
// 약 60ms 마다 동작
// --------------------------------------------------------------------------------
void	CDediRoom::UpdateSyncObject( REAL32 r32GameTime )
{
	_Sync_Obj_Anim( r32GameTime );
	
	_Sync_Obj_Intrusion( r32GameTime );	// 난입이 발생한 경우, 난입 유저에게 stageinfo 패킷을 전송합니다
	
	for( INT32 i = 0 ; i < m_i32GameObjectGroupCount ; i++ )
	{
		m_pGameObjectGroup[i].Update();
	}

	// Battle Use Item Update 위치
	m_pObjectUseItemMgr.Update();
}

void CDediRoom::_Sync_Obj_Intrusion( REAL32 r32GameTime )
{
	UINT32	ui32NextType;
	INT32	i32NextIndex;
	CUdpBuffer*  pSendBuffer = NULL;
	CDediMember* pMember = NULL;
	
	if( !m_ui32IntrusionUsers ) return;
	
	g_pUdpBufferPool->GetBuffer(&pSendBuffer); 
	if( pSendBuffer == NULL)
	{
		DSERROR("[CDediRoom::_Sync_Obj_Intrusion] Get Buffer Error\n");
		return;						
	}

	
	ui32NextType = P2P_SUB_HEAD_STAGEINFO_CHARA;
	i32NextIndex = 0;

	do 
	{
		UINT32 ui32Written = MakeStageInfoPacket(pSendBuffer->GetBuffer(), UDP_SEND_PACKET_SIZE, &ui32NextType, &i32NextIndex, r32GameTime );
		if( ui32Written == 0 ) break;
		
		for ( UINT32 ui32SlotIdx = 0; ui32SlotIdx < SLOT_MAX_COUNT; ui32SlotIdx++ )
		{
			if ( GetIntrusion( ui32SlotIdx ) == FALSE )  continue; // check slot state
			if ( IsSendEnable( ui32SlotIdx ) == FALSE )  continue; // check position received.
			if (GetGameTime() - m_ar32IntrusionTimes[ui32SlotIdx]< 10.f)continue;
				


			if (IsDestroyMissionMode())
			{
				P_BATTLE_M_DESTRUCTION_HP_ACK	destructionInfo;
				i3mem::FillZero( &destructionInfo, sizeof(destructionInfo) );
				destructionInfo._RoomInfo	 = m_RoomInfo;
				m_DestructionData.m_ObjectSendDamageInfo.CopyTo( &destructionInfo._DestructionInfo );
				destructionInfo._DestructionInfo._nBlueObjectHP = m_DestructionData.m_pObjectBlue->getHP();
				destructionInfo._DestructionInfo._nRedObjectHP =  m_DestructionData.m_pObjectRed->getHP();
				// 리턴값 체크하지 않음. 실제 send 할 때, 실패시 로그 남김
				g_pModuleCast->Send_PROTOCOL_BATTLE_M_DESTRUCTION_HP_ACK( GetTaskIndex(), &destructionInfo );
				m_r32LastSentTimeToControlServer = GetGameTime();
			}
			else if (IsDefenceMissionMode())
			{
				P_BATTLE_M_DEFENCE_ACK	defenceInfo;
				i3mem::FillZero( &defenceInfo, sizeof(defenceInfo) );
				defenceInfo._RoomInfo	 = m_RoomInfo;
				defenceInfo._DefenceInfo = *m_DefenceData.getSendDamageInfo();
				defenceInfo._DefenceInfo.setObjectHP( 0, m_DefenceData.getDefenceObj(0)->getHP() );
				defenceInfo._DefenceInfo.setObjectHP( 1, m_DefenceData.getDefenceObj(1)->getHP() );

				// 리턴값 체크하지 않음. 실제 send 할 때, 실패시 로그 남김
				g_pModuleCast->Send_PROTOCOL_BATTLE_M_DEFENCE_ACK( GetTaskIndex(), &defenceInfo );
				m_r32LastSentTimeToControlServer = GetGameTime();
			}

			// send
			pMember = GetMember( ui32SlotIdx );
			if( ! SendUdpData((char*)pSendBuffer->GetBuffer(), ui32Written, GetSocketIndex(), pMember->GetIp(), pMember->GetPort() ) )
			{
				DSWARNING("[CDediRoom::_Sync_Obj_Intrusion] Error! STAGEINFO-SendUdpData(%d)\n", GetLastError());
				break;
			} 
			
			// done?
			if( ui32NextType == P2P_SUB_HEAD_STAGEINFO_MISSION && i32NextIndex == 1 ) 
			{
				ResetIntrusion ( ui32SlotIdx );
				m_ar32IntrusionTimes[ ui32SlotIdx ] = 0.0f;
				DSMSG( "[%d] %f SEND STAGE INFO OK\n", ui32SlotIdx, GetGameTime());
			}
		}

	} while ( ui32NextType != P2P_SUB_HEAD_STAGEINFO_MISSION || i32NextIndex != 1 ); // end condition

	g_pUdpBufferPool->ReleaseBuffer( pSendBuffer );
}

// test code
void CDediRoom::_Sync_Obj_RespawnHelicopter( REAL32 r32GameTime, CDSObject* pObject ) 
{
	REAL32  r32Duration				= 0.0f;	// 리소스로부터 읽은 값
	REAL32	r32ElapsedTime			= 0.0f; // 게임타임에서 마지막 전송시간을 뺀 값
	UINT8	ui8PathKey				= 0;	// 헬리콥터 예외처리
	
	//1. key path가 변경된 경우 처리 (처음 실행)
	if ( pObject->m_bPathKeyChanged == TRUE )
	{
		pObject->m_r32StartTime			= r32GameTime;
		pObject->m_r32LastSendTime		= r32GameTime;
		pObject->m_bPathKeyChanged		= FALSE;
		pObject->m_bUDPSend				= TRUE;
		pObject->setKeyFrame( 0.f );
	}
	else
	{
		ui8PathKey  = pObject->getPathKey();					
		r32Duration = pObject->m_pDurations[ ui8PathKey ];
	
		// 2. 최근 전송후 경과 시간
		r32ElapsedTime			= r32GameTime - pObject->m_r32LastSendTime;
		REAL32 r32FrameKeyTime	= r32GameTime - pObject->m_r32StartTime;

		// 3.1 플레이 시간이 애니메이션 시간보다 길다면 path key 리셋
		if( r32FrameKeyTime > r32Duration )
		{
			r32FrameKeyTime				= pObject->m_pDurations[ ui8PathKey ];
			pObject->m_bPathKeyChanged	= TRUE;

			pObject->setPathKey( static_cast<UINT8>( CalcAnimObjPath( m_aui8HitPartIndex, GetRoundNumber(), pObject->m_i8TeamIdx, pObject->getPathKey() ) ) );
			// 다음 프레임에서 처리 (2013-02-15 오브젝트가 개별적으로 셋팅되기 때문에 옛날 코드에 붙여도 된다)
		}

		// 3. 애니메이션 플레이 시간
		pObject->setKeyFrame( r32FrameKeyTime );
	}
}

// case1: 포트아카바 container (기본형)
// case2: helispot respawn 헬기: duration이 끝나면 path 결정, 그룹 단위 path 관리 
// case3: outpost  riding 헬기: 액션 키에 의한 path 결정. ( 주의: breakdown의 헬기는 static object임. animation으로 처리하면 hit 패킷에서 문제 발생)
// case4: storm tube repair train : outpost 헬기와 동작방식 유사. 단, start time은 C4가 AREA_B에 설치 시각. (TCP로 제어한답니다..5월4일)
// case5: breakdown 발전기는 animation이다 (hp sync 때문)

void CDediRoom::_Sync_Obj_Anim (REAL32 r32GameTime)
{
	REAL32  r32Duration				= 0.0f;	// 리소스로부터 읽은 값
	REAL32	r32ElapsedTime			= 0.0f; // 게임타임에서 마지막 전송시간을 뺀 값

	UINT16		ui16NetIndex;
	CDSObject*	pObject;
	
	if ( r32GameTime < DS_FIRST_ANIMATION_SYNC_TIME ) return;

	for( INT32 i32Idx = 0; i32Idx < m_i32CountOfAnim; i32Idx++ )
	{
		ui16NetIndex	= m_pNetIndex_Obj_Anim[ i32Idx ];
		pObject			= &m_aGameObjects[ ui16NetIndex ];

		if( pObject->GetSType() == OBJECT_S_TYPE_RESPAWN_HELICOPTER)
		{
			_Sync_Obj_RespawnHelicopter(r32GameTime, pObject);
			continue;
		}
		
		//storm tube test
		
		// key path가 변경된 경우 처리 (처음 실행)
		if ( pObject->m_bPathKeyChanged == TRUE )
		{
			// init play info
			pObject->m_bPathKeyChanged		= FALSE;
			pObject->m_r32StartTime			= r32GameTime;
			pObject->m_r32LastSendTime		= r32GameTime;
			pObject->m_bUDPSend				= TRUE;
			pObject->setKeyFrame( 0.0f );
		}
		else
		{
			r32Duration				= pObject->m_pDurations[ pObject->getPathKey() ];
			// 3. 애니메이션 플레이 시간 계산
			REAL32 r32FrameKeyTime	= r32GameTime - pObject->m_r32StartTime;

			// 3.1 플레이 시간이 애니메이션 시간보다 길다면 path key 리셋 (헬기만 해당)
			
			// 플레이 시간이 애니메이션 길이보다 긴 경우
			if ( r32FrameKeyTime > r32Duration )
			{
				r32FrameKeyTime					= r32Duration;
				pObject->m_bPathKeyChanged		= TRUE;									// 다음 프레임에서 처리

				// Outpost 헬기: 플레이 시간이 끝나면 사용 해제
				if( pObject->GetAnimRepeat() )
				{	// 반복되는 에니메이션
					pObject->m_bUDPSend			= TRUE;
					pObject->m_r32LastSendTime	= r32GameTime;
					pObject->setPathKey( CalcPathKey( pObject, FALSE ) );		// idle 상태
				}
				else
				{	// 단발성 에니메이션
					pObject->setPathKey( CalcPathKey( pObject, TRUE ) );						// idle 상태
					pObject->ResetUse();													// 무조건 사용을 해제
					continue;
				}
			}		

			pObject->setKeyFrame( r32FrameKeyTime );

			// 2. 최근 전송후 경과 시간
			r32ElapsedTime			= r32GameTime - pObject->m_r32LastSendTime;
			if ( GetStageUID() == STAGE_UID_STORMTUBE && 
				 r32ElapsedTime > 5.0f &&
				 pObject->getKeyFrame() + 5.0f < r32Duration )
			{
 				pObject->m_bUDPSend					= TRUE;
				pObject->m_r32LastSendTime			= r32GameTime;

				I3TRACE("idx=%d, elapsed_time=%f, play_time=%f, game_time=%f\n", i32Idx, r32ElapsedTime, pObject->getKeyFrame(), r32GameTime );
			}
		} // send
	} // for
}

// --------------------------------------------------------------------------------
// 컨트롤 서버에 전송하기 위한 인터페이스 함수
// --------------------------------------------------------------------------------
BOOL	CDediRoom::SendRequestToControlServer()
{
	if( m_ui32TcpSendFlag & DS_SEND_MISSION_TRIGGER	 )		_SendControl_MissionTrigger(); 
	if( m_ui32TcpSendFlag & DS_SEND_USER_KILL		 )		_SendControl_KillDeathInfo(); 
	if( m_ui32TcpSendFlag & DS_SEND_MISSION_DESTRUCTION )	_SendControl_Destruction();
	if( m_ui32TcpSendFlag & DS_SEND_MISSION_DEFENCE)		_SendControl_Defence();
	if( m_ui32TcpSendFlag & DS_SEND_CHEATING_REPORT )		_SendControl_HackReport();

	return TRUE;
}

// --------------------------------------------------------------------------------
// 컨트롤 서버에 전송하기 위한 내부 함수들
// --------------------------------------------------------------------------------

BOOL	CDediRoom::_SendControl_KillDeathInfo()
{
	INT32 WorkIdx  = GetTaskIndex();
	BOOL  bMyDeath = FALSE;
	
	if( m_ui32TcpSendFlag & DS_SEND_USER_KILL )
	{
		DEATH_INFO_CHARA	data[MAX_KILLUSER_COUNT];
		DEATH_INFO_HEADER	head;
		
		i3mem::FillZero( &head, sizeof(head));
		i3mem::FillZero( &data, sizeof(data) );

		head._nKillType	= 0; // default
		head._nCount	= 0; // init

		// CGameContex::SendDeathChara() 참고
		
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			bMyDeath = FALSE;

			if( m_pKillInfo[ i ].m_KillerAgainCount > 0 )
			{
				head._nKillType			= (UINT8) SET_KILLTYPE_LOW( head._nKillType, m_pKillInfo[i].m_KillType );
				if( IsAcid( i ) && CHARA_DEATH_B == GET_DEATHINFO_DEATH_TYPE(m_pKillInfo[i].m_KillBuffer[0]) ) // 애시드가 낙하 데미지로 죽었을 때는 데스처리 되도록 폭발한 경우에만 체크
				{
					head._nKillType		= (UINT8) SET_KILLTYPE_HIGH( head._nKillType, CHARA_KILL_HIGH_BOMBING );
				}

				head._nCount			= (UINT8) m_pKillInfo[i].m_KillerAgainCount;
				// KillMark5
				head._WeaponInfo		= m_pKillInfo[i].m_KillWeaponInfo;
				head._nUserIdx			= (UINT8) i;
				head._nByObjectIdx		= (UINT8) m_pKillInfo[i].m_KillObjectIdx;

				// 죽인 유저의 위치
				N_PCINFO_POSROTATION* pos_rot = getChara_PosRot(i);
				N_POS* pAttacker = &pos_rot->_Pos;
				VEC3D  vAttacker;
				UDP_UTIL::ConvertNet3DtoVec3D( &vAttacker, pAttacker );
				
				head._KillPosition[0]	= vAttacker.x;
				head._KillPosition[1]	= vAttacker.y;
				head._KillPosition[2]	= vAttacker.z;

				I3ASSERT( m_pKillInfo[i].m_KillerAgainCount <= MAX_KILLUSER_COUNT );

				for(INT32 j = 0; j < m_pKillInfo[ i].m_KillerAgainCount; j++ )
				{
					if( GET_DEATHINFO_IDX_TYPE( m_pKillInfo[ i].m_KillBuffer[ j ] ) == (UINT32)i )
					{	// 자살한 경우만 체크
						bMyDeath = TRUE;
					}
					data[ j ]._ui8Weapon		= m_pKillInfo[ i].m_ui8Weapon[ j ];
					data[ j ]._DeathInfo		= m_pKillInfo[ i].m_KillBuffer[ j ];
					data[ j ]._DeathPosition[0] = m_pKillInfo[ i].m_KillPositionBuf[ j ][0];
					data[ j ]._DeathPosition[1] = m_pKillInfo[ i].m_KillPositionBuf[ j ][1];
					data[ j ]._DeathPosition[2] = m_pKillInfo[ i].m_KillPositionBuf[ j ][2];
					data[ j ]._ui8AssistIdx		= m_pKillInfo[i].m_ui8AssistIdx[j];
				} // for

				if( bMyDeath && (m_pKillInfo[ i].m_KillerAgainCount == 2))
				{	// MassKill이 아닙니다.
					head._nKillType	= SET_KILLTYPE_LOW( head._nKillType, CHARA_KILL_DEFAULT );
				}

				// flag reset
				m_pKillInfo[ i].m_KillerAgainCount = 0;
				// send
				P_BATTLE_ROOM_KILL_ACK ack;
				ack._RoomInfo = m_RoomInfo;
				ack._killHeader = head;
				
				// 리턴값 체크하지 않음. 실제 send 할 때, 실패시 로그 남김
				g_pModuleCast->Send_PROTOCOL_BATTLE_KILL_ACK( WorkIdx, &ack, data, GetRoundNumber() );
			}
		} // for

		m_ui32TcpSendFlag &= (~DS_SEND_USER_KILL);

		return TRUE;
	}
	
	return FALSE;
}

void	CDediRoom::_SendControl_MissionTrigger()
{
	INT32 i32WorkIdx  = GetTaskIndex();
	REAL32 r32GameTime = GetGameTime();
	
	UINT32 i32SendFlag = DS_SEND_MISSION_TRIGGER;
	if( m_ui32TcpSendFlag & i32SendFlag ) // C4 밖에 없음
	{
		//	NET_MISSION_TRIGGER_ARM_DONE  
		if ( m_MissionIntruder.getInstallMode() == 1 )
		{
			// 8월 29일 주석처리
			//if ( m_r32LastSentTimeToControlServer + 1.0f < fGameTime )
			{
				P_BATTLE_M_BOMB_INSTALL_ACK bomb;
				i3mem::FillZero( &bomb, sizeof( bomb ) );

				bomb._RoomInfo		= m_RoomInfo;
				bomb._iSlotIdx		= m_MissionIntruder.getInstallerIndex();
				bomb._BombPosition	= (UINT8)m_MissionArea;
				bomb._BombVecPos[0]	= m_MissionIntruder.getInstallPosition()->x;
				bomb._BombVecPos[1]	= m_MissionIntruder.getInstallPosition()->y;
				bomb._BombVecPos[2]	= m_MissionIntruder.getInstallPosition()->z;
				
				// 리턴값 체크하지 않음. 실제 send 할 때, 실패시 로그 남김
				g_pModuleCast->Send_PROTOCOL_BATTLE_M_BOMB_INSTALL_ACK( i32WorkIdx, &bomb );	
			}
		}
		//	NET_MISSION_TRIGGER_DISARM_DONE
		else if( m_MissionIntruder.getInstallMode() == 0 )
		{
			// 8월 29일 주석처리
			//if ( m_r32LastSentTimeToControlServer + 1.0f < fGameTime )
			{
				P_BATTLE_M_BOMB_UNINSTALL_ACK bomb;
				i3mem::FillZero( &bomb, sizeof( bomb ) );

				bomb._RoomInfo		= m_RoomInfo;
				bomb._iSlotIdx		= m_MissionIntruder.getInstallerIndex();
				
				// 리턴값 체크하지 않음. 실제 send 할 때, 실패시 로그 남김
				g_pModuleCast->Send_PROTOCOL_BATTLE_M_BOMB_UNINSTALL_ACK( i32WorkIdx, &bomb );
			}
		}
	}

	m_ui32TcpSendFlag &= ~i32SendFlag;
	m_r32LastSentTimeToControlServer = r32GameTime;

	return ;
}



BOOL	CDediRoom::_SendControl_Destruction()
{
	INT32  i32WorkIdx  = GetTaskIndex();
	REAL32 r32GameTime = GetGameTime();
	
	if( m_ui32TcpSendFlag & DS_SEND_MISSION_DESTRUCTION )
	{
		if ( m_r32LastSentTimeToControlServer + 1.0f < r32GameTime )
		{
			P_BATTLE_M_DESTRUCTION_HP_ACK	destructionInfo;
			i3mem::FillZero( &destructionInfo, sizeof(destructionInfo));
			destructionInfo._RoomInfo = m_RoomInfo;
			destructionInfo._DestructionInfo = m_DestructionData.m_ObjectSendDamageInfo;
			
			// 리턴값 체크하지 않음. 실제 send 할 때, 실패시 로그 남김
			g_pModuleCast->Send_PROTOCOL_BATTLE_M_DESTRUCTION_HP_ACK( i32WorkIdx, &destructionInfo );
	
			m_ui32TcpSendFlag &= (~DS_SEND_MISSION_DESTRUCTION);
			m_r32LastSentTimeToControlServer = r32GameTime;
			return TRUE; 
		}
		
	}
	
	return FALSE;
}

BOOL	CDediRoom::_SendControl_Defence()
{
	INT32 i32WorkIdx	= GetTaskIndex();
	REAL32 r32GameTime	= GetGameTime();

	if( m_ui32TcpSendFlag & DS_SEND_MISSION_DEFENCE )
	{
		if ( m_r32LastSentTimeToControlServer + 1.0f < r32GameTime )
		{
			P_BATTLE_M_DEFENCE_ACK	defenceInfo;
			i3mem::FillZero( &defenceInfo, sizeof(defenceInfo) );
			defenceInfo._RoomInfo	 = m_RoomInfo;
			defenceInfo._DefenceInfo = *m_DefenceData.getSendDamageInfo();
			
			// 리턴값 체크하지 않음. 실제 send 할 때, 실패시 로그 남김
			g_pModuleCast->Send_PROTOCOL_BATTLE_M_DEFENCE_ACK( i32WorkIdx, &defenceInfo );
			
			m_ui32TcpSendFlag &= (~DS_SEND_MISSION_DEFENCE);
			m_r32LastSentTimeToControlServer = r32GameTime;
			return TRUE; 
		}
	}
	
	return FALSE;
}

BOOL	CDediRoom::_SendControl_HackReport()
{
	INT32 i32WorkIdx = GetTaskIndex();
	
	if( m_ui32TcpSendFlag & DS_SEND_CHEATING_REPORT )
	{
		m_ui32TcpSendFlag &= (~DS_SEND_CHEATING_REPORT);

		P_BATTLE_HACK_USER_ACK hackInfo;
		P_BATTLE_HACK_INFO	   hackUser[SLOT_MAX_COUNT]; 


		hackInfo._RoomInfo	= GetRoomInfo();
		hackInfo._uCount	= 0;

		for( UINT32 ui32SlotIdx = 0; ui32SlotIdx < SLOT_MAX_COUNT; ui32SlotIdx++ )
		{
			if( IsHackUser( ui32SlotIdx ) && !GetHackingReportSent( ui32SlotIdx ) )
			{
				SetHackingReportState( ui32SlotIdx, TRUE );

				hackUser[ hackInfo._uCount ]._i64UID   = GetUID( ui32SlotIdx );
				hackUser[ hackInfo._uCount ]._uSlotIdx = ui32SlotIdx;
				hackUser[ hackInfo._uCount ]._uReason  = GetHackType( ui32SlotIdx );
				
				hackInfo._uCount++;
			}
		
		}
		
		if( hackInfo._uCount > 0 )
		{
			g_pModuleCast->Send_PROTOCOL_BATTLE_HACK_USER_ACK( i32WorkIdx, &hackInfo, hackUser );
			return TRUE; 
		}
	}

	return FALSE;
}

void CDediRoom::_QueDeathChara(UINT32 ui32SlotIdx, UINT8 ui8DeathWeapon, UINT32 ui32DeathPacket, INT32 i32Type, UINT32 ui32WeaponInfo, VEC3D * pDeathPos, INT32 i32ObjectIdx, INT32 AssistIdx)
{
	I3ASSERT(ui32SlotIdx < SLOT_MAX_COUNT);
	m_pKillInfo[ui32SlotIdx].QueKillingInfo(ui8DeathWeapon, ui32DeathPacket, i32Type, ui32WeaponInfo, pDeathPos, i32ObjectIdx, AssistIdx);
}

void CDediRoom::_Write_Chara_Death( GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo ) 
{
	UINT32 nTemp = 0;
	if( pDeathInfo->_iKillidx == INVALID_SLOT_IDX )
	{
		DSERROR("[CDediRoom::_Write_Chara_Death] Invalid killer index.\n");
		return;
	}

	BOUNDCHK( pDeathInfo->_iDeathidx, SLOT_MAX_COUNT );

	// 네트워크가 끊어진 경우라면 처리하지 않는다
	if( GetMember( pDeathInfo->_iDeathidx )->GetPlaying() == FALSE ) return;

	// A.I. 전은 방장이 death packet을 보낸다.
	if( IsTrainingRoom() ) return;

	{
		//0x00		0		0
		//	message	State	Death

		nTemp = 0;
		nTemp = SET_DEATHINFO_IDX_TYPE( nTemp, pDeathInfo->_iDeathidx );
		nTemp = SET_DEATHINFO_DEATH_TYPE( nTemp, pDeathInfo->_nDeathType );

		N_PCINFO_WEAPON* pWeaponInfo = &m_pCastPacket_Chara_Weapon[ pDeathInfo->_iDeathidx ];   // 죽은 사람의 무기

		// Defence Object를 공격중인지 확인
		if( ( pDeathInfo->_iKillidx != pDeathInfo->_iDeathidx ) &&
			IsAttackingMissionObj( pDeathInfo->_iDeathidx) ) 
		{
			nTemp = SET_DEATHINFO_ENEMY_TYPE( nTemp, CHARA_DEATH_ENEMY_STATE_DEFENCE );
		}
		else if ( (pDeathInfo->_iKillidx != pDeathInfo->_iDeathidx ) 
			&& (IsTRex( pDeathInfo->_iKillidx ) == FALSE )
			&& (pDeathInfo->IsSameClass(WEAPON_CLASS_DINO)  ) 
			&& IsDamageByHowl( pDeathInfo->_iDeathidx ) )
		{
			nTemp = SET_DEATHINFO_MY_TYPE( nTemp, CHARA_DEATH_MY_STATE_ASSIST );
		}
		else if( (pDeathInfo->_iKillidx != pDeathInfo->_iDeathidx ) 
			&& (pDeathInfo->IsSameClass(WEAPON_CLASS_GRENADESHELL) && pDeathInfo->IsSameNumber(GRENADESHELL_POISON) ) // 9: GetItemIndex("GRENADESHELL_POISON")
			&& (IsTRex( pDeathInfo->_iKillidx ) == FALSE )
			&& IsDamageByHowl( pDeathInfo->_iDeathidx ) )
		{
			nTemp = SET_DEATHINFO_MY_TYPE( nTemp, CHARA_DEATH_MY_STATE_ASSIST );
		}

		if( pDeathInfo->_iKillidx != pDeathInfo->_iDeathidx && IsEscapeNearKill(pDeathInfo->_iDeathidx) )
			nTemp = SET_DEATHINFO_ENEMY_TYPE( nTemp, CHARA_DEATH_ENEMY_STATE_ESCAPE );

		// TCP로 MS에 알려줍니다.
		// 죽은 사람 포지션
		VEC3D vPos;
		N_PCINFO_POSROTATION* pNetPosRot = getChara_PosRot( pDeathInfo->_iDeathidx );
		UDP_UTIL::ConvertNet3DtoVec3D( &vPos, &pNetPosRot->_Pos );

		I3TRACE("DEATH POS: (%f, %f, %f)\n", vPos.x, vPos.y, vPos.z );

		UINT8 ui8DeathWeapon = (UINT8)pWeaponInfo->GetWeaponClass();
		I3TRACE("CDediRoom::_Write_Chara_Death() WeaponID =%d\n", pDeathInfo->_ui32WeaponID );

		m_AssistInfo[pDeathInfo->_iDeathidx].Reset();
		_QueDeathChara(pDeathInfo->_iKillidx, ui8DeathWeapon, nTemp, pDeathInfo->_nDeathType, pDeathInfo->_ui32WeaponID, &vPos, pDeathInfo->_iByObjectIdx, pDeathInfo->_iAssistIdx);
	}

	// UDP용  
	{
		I3TRACE("Death Index = %d\n", pDeathInfo->_iDeathidx );
		N_PCINFO_LIFE*		pLife  = getChara_Life ( pDeathInfo->_iDeathidx );
		N_PCINFO_DEATH*		pDeath = &m_pCastPacket_Chara_Death[ pDeathInfo->_iDeathidx ];

		pLife->iLife = 0;

		// 1byte (0x0F :DeathType, 0xF0 : ShooterIndex)
		pDeath->DeathInfo	= (pDeathInfo->_nDeathType & 0x0f) | ( (UINT8)pDeathInfo->_iKillidx << 4);
		pDeath->HitPart		= (UINT8) pDeathInfo->_nHitPart;
		pDeath->_WeaponItemID	= pDeathInfo->_ui32WeaponID;
		// 히트 방향 계산
		if ( pDeathInfo->_pHitDir == NULL )
		{
			VEC3D vKillerPos, vDeathPos, vDir;
			UDP_UTIL::ConvertNet3DtoVec3D( &vKillerPos, &getChara_PosRot( pDeathInfo->_iKillidx )->_Pos );
			UDP_UTIL::ConvertNet3DtoVec3D( &vDeathPos,  &getChara_PosRot( pDeathInfo->_iDeathidx)->_Pos );
			i3Vector::Sub(&vDir, &vDeathPos, &vKillerPos);
			i3Vector::Normalize(&vDir, &vDir);
			UDP_UTIL::ConvertVec3DtoNet3D( &pDeath->_HitDir, &vDir );
		}
		else
		{	// suicide_damage 의 경우 direction이 계산되어져 있다
			UDP_UTIL::ConvertVec3DtoNet3D( &pDeath->_HitDir, pDeathInfo->_pHitDir );
		}

		// death
		CASTCHECK_USER* pCheck = getChara_Check( pDeathInfo->_iDeathidx );			 
		pCheck->nP2PInfoFlag |= (P2P_INFO_FLAG_CS_DEATH | P2P_INFO_FLAG_CS_LIFE);
		pCheck->bCheckBroadcast =  true;
	}
}

BOOL CDediRoom::IsEscapeNearKill( UINT32 ui32DeathIdx )
{
	if( !IS_VALID_MEMBER_RANGE( ui32DeathIdx ) ) return FALSE;

	//피해자가 공룡이면 리턴 시킵니다.
	if( IsRaptorBase( ui32DeathIdx ))
		return FALSE;

	//피해자의 포탈번호를 가져옵니다.
	N_PCINFO_POSROTATION* pos = getChara_PosRot( ui32DeathIdx );
	INT32 Sector = pos->iSectorIdx;

	//스테이지 정보를 가져옵니다.
	STAGE_ID StageID = GetStageID();
	
	if ( STAGE_MODE_ESCAPE != StageID::GetStageMode(StageID) )
		return FALSE;

	switch( StageID::GetStageUID(StageID) )
	{
	case STAGE_UID_BREEDING_NEST :
		if (Sector == 3)
		{
			return TRUE;
		}
		else
			return FALSE;
		break;
	case  STAGE_UID_DUPTOWN :
		if (Sector == 12)
		{
			return TRUE;
		}
		else
			return FALSE;

		break;

	case STAGE_UID_DINOBREAKDOWN :
		if (Sector == 8)
		{
			return TRUE;
		}
		else
			return FALSE;

		break;
	case STAGE_UID_DINORUINS:
		if (Sector == 3)
		{
			return TRUE;
		}
		else
			return FALSE;
		break;
	default:
		return FALSE;
	}

}	

BOOL	CDediRoom::IsRaptorBase( UINT32 ui32SlotIdx )
{
	if( !IS_VALID_MEMBER_RANGE( ui32SlotIdx ) ) return FALSE;

	// vv3 - 체크
	switch ( m_pGameCharacters[ ui32SlotIdx ].GetParts( CHAR_EQUIPMENT_PARTS_CHARA ) )
	{
		// case에 해당하는 것은 모두 true임
	case 1500503:// 랩터
	case 1500504:// 스팅
	case 1500506:// Tank
	case 1500501:// Acid
	case 1500502:// Elite
	case 1500511:// CC Raptor
	case 1500512:// CC Sting
	case 1500513:// CC Acid
	case 1500514:// CC RaptorMercury 
	case 1500515:// CC StingMars
	case 1500516:// CC AcidVulcan
		return TRUE;
		break;
	default:
		return FALSE;
		break;
	};
}


BOOL	CDediRoom::IsAcid( UINT32  ui32SlotIdx)
{
	if( !IS_VALID_MEMBER_RANGE( ui32SlotIdx ) ) return FALSE;

	// vv3 - 체크
	return ( 1500501 == m_pGameCharacters[ ui32SlotIdx ].GetParts( CHAR_EQUIPMENT_PARTS_CHARA ) || 
			1500513 == m_pGameCharacters[ ui32SlotIdx ].GetParts( CHAR_EQUIPMENT_PARTS_CHARA ) ||
			1500516 == m_pGameCharacters[ ui32SlotIdx ].GetParts( CHAR_EQUIPMENT_PARTS_CHARA ) ); // CC AcidVulcan
}


BOOL	CDediRoom::IsTRex( UINT32	ui32SlotIdx )
{
	if( !IS_VALID_MEMBER_RANGE( ui32SlotIdx ) ) return FALSE;

	// vv3 - 체크
	return ( 1500505 == m_pGameCharacters[ ui32SlotIdx ].GetParts( CHAR_EQUIPMENT_PARTS_CHARA ) );
}

BOOL	CDediRoom::IsElite( UINT32 ui32SlotIdx )
{
	if( !IS_VALID_MEMBER_RANGE( ui32SlotIdx ) ) return FALSE;

	// vv3 - 체크
	return ( 1500502 == m_pGameCharacters[ ui32SlotIdx ].GetParts( CHAR_EQUIPMENT_PARTS_CHARA ) );
}

// OBJECT_S_TYPE_RESPAWN_HELICOPTER
// OBJECT_S_TYPE_RIDING_HELICOPTER
UINT8	CDediRoom::CalcPathKey( CDSObject* pObj, BOOL bFinish )
{
	UINT8 ui8PathKey = 0;
	
	switch( pObj->m_iSType )
	{
	case OBJECT_S_TYPE_RESPAWN_HELICOPTER:
		{
			UINT8 ui8AniGroup = GetRoundNumber() % 2;

			m_ui8CalcCount++;							// 0, 1, 2, 3
			if( 4 <= m_ui8CalcCount )	m_ui8CalcCount = 0;

			if( pObj->m_i8GroupIdx == 1 ) // blue team
			{
				ui8PathKey = 3;
			}
			else // red team
			{
				ui8PathKey = 1;
			}

			switch( m_ui8CalcCount )
			{
			case 1:
			case 2:
				ui8PathKey += 1;											// 1, 2 번째 돌때는 다른 위치로 돌도록
				break;
			}
			if( 0 == ui8AniGroup )		ui8PathKey += 4;					// 짝수 라운드
		}
		break;
	case OBJECT_S_TYPE_RIDING_HELICOPTER:
		{
			if( bFinish )
			{
				ui8PathKey = 0;
			}
			else
			{
				ui8PathKey = (rand() % 3) + 1; // 1에서 3까지 유효값 (클라이언트 state가 3개 밖에 없다)
			}
		}
		break;
	case OBJECT_S_TYPE_REPAIR_TRAIN:
		{
			if( bFinish )
			{
				switch( pObj->getPathKey() )
				{
				case 1:	
				case 2: ui8PathKey = 2;	break;
				}
			}
			else
			{
				ui8PathKey = 1;
			}
		}
		break;
	default:
		{
			ui8PathKey = 0;
		}
		break;
	}

	return ui8PathKey;
}

// StageInfo를 전송한다
// LastType과 LastIndex를 보고 계속 진행여부를 판단해야 함.
UINT32 CDediRoom::MakeStageInfoPacket( char* pOutBuf, UINT32 ui32BufLen, /*IN OUT*/ UINT32* pui32NextType, /*IN OUT*/ INT32* pi32NextIndex, REAL32 r32GameTime ) 
{
	N_GAME_HEADER* pHead			= (N_GAME_HEADER*) pOutBuf;
	INT32	i32Idx					= 0;							// slot, objectIdx, ...
	UINT32	ui32Written				= 0;							//  _Write...함수의 리턴 값
	UINT16	ui32SendPos				= sizeof( N_GAME_HEADER) ;		// write position 조정
	
	// for object
	UINT32	ui32Count				= 0;		// 오브젝트 개수 (디버그 용)
	UINT8	ui8State				= 0;		// 오브젝트에 따라 상태값은 다르다
	BOOL	bSend					= FALSE;
	// character hp/pos
	N_GAME_SUBHEAD*	 pUserHeader	= NULL;
	// object hp
	CDSObject*	 pObj				= NULL;
	
	// buffer length 조정
	ui32BufLen						-= sizeof ( N_GAME_HEADER );

	while ( ui32SendPos < ui32BufLen )
	{
		switch ( *pui32NextType )
		{
		case P2P_SUB_HEAD_STAGEINFO_CHARA:
			{
				for( i32Idx = *pi32NextIndex; i32Idx < SLOT_MAX_COUNT; i32Idx++ )
				{
					// end checking
					if( UDP_UTIL::IsPackSize( P2P_SUB_HEAD_STAGEINFO_CHARA, ui32SendPos, (UINT16)ui32BufLen ) == false)
					{
						*pi32NextIndex = i32Idx;
						*pui32NextType = P2P_SUB_HEAD_STAGEINFO_CHARA;
						goto write_header;
					}
					
					if( GetMember(i32Idx)->GetPlaying() )
					{
						// life를 보낸다
						pUserHeader = (N_GAME_SUBHEAD*)&pOutBuf[ui32SendPos ];					// save pos
						ui32SendPos += sizeof(N_GAME_SUBHEAD);

						N_PCINFO_DEF2  charaDef;

						//charaDef.iInfoFlag = ( P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CS_LIFE);	// def
						charaDef.iInfoFlag = P2P_INFO_FLAG_CS_LIFE;
						ui32SendPos += charaDef.CopyToPacket( &pOutBuf[ ui32SendPos ] );
						
						//ui32SendPos += m_pCastPacket_Chara_PosRot[ i32Idx ].CopyToPacket( &pOutBuf[ ui32SendPos ] );	//pos
						ui32SendPos += m_pCastPacket_Chara_Life[ i32Idx ].CopyToPacket  ( &pOutBuf[ ui32SendPos ] );	//life
						
						// build header
						ui32Written = (UINT16)( &pOutBuf[ui32SendPos] - (char*)pUserHeader);
						pUserHeader->_Code  = ui32Written;
						pUserHeader->_Index = (UINT16) i32Idx;
						pUserHeader->_Type  = P2P_SUB_HEAD_STAGEINFO_CHARA;

						ui32BufLen -= ui32Written;
						//I3TRACE("added stageinfo: chara[%d] hp=%d\n", i32Idx, m_pCastPacket_Chara_Life[ i32Idx ].iLife );
					}
				}
			}
			
			*pui32NextType = P2P_SUB_HEAD_OBJECT;
			*pi32NextIndex = 0;
			
			break;
		case P2P_SUB_HEAD_OBJECT:
			{
				for( i32Idx = *pi32NextIndex; i32Idx < m_i32GameObjectCount; i32Idx++ )
				{
					pObj			= &m_aGameObjects[ i32Idx ];

					if( FALSE == pObj->MakePacket( &pOutBuf[ui32SendPos], ui32BufLen, ui32Written, SEND_PACKET_TYPE_SYNC ) )
					{
						goto write_header;
					}

					ui32BufLen		-= ui32Written;
					ui32SendPos		+= ui32Written;
					
					*pi32NextIndex	= i32Idx + 1;

					bSend			= TRUE;
				}
				*pui32NextType = P2P_SUB_HEAD_STAGEINFO_MISSION;
				*pi32NextIndex = 0;
			}
			break;
		case P2P_SUB_HEAD_STAGEINFO_MISSION:
			{
				ui32Count = 0;
				// 1개만 사용된다
				for ( i32Idx = *pi32NextIndex; i32Idx < 1; i32Idx++ )
				{
					if( UDP_UTIL::IsPackSize( P2P_SUB_HEAD_STAGEINFO_MISSION, ui32SendPos, (UINT16)ui32BufLen ) == false)
					{
						goto write_header;
					}

					ui8State  = m_MissionIntruder.getInstallMode();
					if( ui8State == 0)
					{
						*pi32NextIndex = i32Idx + 1;
						break;
					}

					ui32Written = m_MissionIntruder.PackFull( &pOutBuf[ ui32SendPos], ui32BufLen, P2P_SUB_HEAD_STAGEINFO_MISSION, m_MissionIntruder.getInstallerIndex() );
					ui32BufLen  -= ui32Written;
					ui32SendPos += ui32Written;

					*pi32NextIndex = i32Idx + 1;
					ui32Count++;
					bSend = TRUE;
				}

				*pui32NextType = P2P_SUB_HEAD_STAGEINFO_MISSION;
				*pi32NextIndex = 1;
				
				if ( ui32Count > 0)
				{
					I3TRACE( "add intrude stageinfo mission ok.(count:%d) : BOMB MISSION ONLY \n", ui32Count );
				}
				// 마지막 오브젝트.
				goto write_header;
			}
			break;
		default: // error
			break;
		} // switch
	} // while
	
write_header:
	
	if ( bSend )
	{
		I3ASSERT( ui32SendPos > sizeof(N_GAME_HEADER) );

		// 헤더
		pHead->Reset();
		
		pHead->_ProtocolID  = UDPPROTOCOL_CS_INFO;
		pHead->_Index		= SLOT_DEDICATION_IDX;	// 방장인덱스는 항상 0xff. dedication client는 방을 만든 유저 인덱스를 유지해야 함.
		pHead->_GameTime	= r32GameTime;
		pHead->_RoundNum	= m_ui8RoundNumber;
		pHead->_Size		= ui32SendPos;
	
		return ui32SendPos;
	}

	return 0;
}

//  공룡상태가 바뀌면 날려줌.  
BOOL CDediRoom::SetDinoInfo_C( P_BATTLE_M_DINO_INFO_REQ * pDinoInfo )
{
	UINT16 iHP = 0; 
	UINT32 ui32SlotIdx = pDinoInfo->_iSlotIdx;

	if( !( IsEscapeMissionMode() || IsCrossCountMode() ) )return FALSE; 
	if( !IS_VALID_MEMBER_RANGE( ui32SlotIdx ) ) return FALSE;
	
	CCharacter * pChara = GetGameCharacter( ui32SlotIdx );
	m_cs.Lock();
	
	ResetFlag( ui32SlotIdx );
	ResetHackFlagForLog(ui32SlotIdx);

	// vv3 - 체크
	for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; ++i )
	{
		pChara->SetParts( i, pDinoInfo->m_TPartsID[i], m_aui8PBVer[ui32SlotIdx] );
	}

	pChara->SetSlotItem( pDinoInfo->_ui32Item );

	// vv3 - 체크
	ApplyEquipment	( ui32SlotIdx, pChara->GetParts( CHAR_EQUIPMENT_PARTS_CHARA ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_HEAD ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_BERET ) );
	ApplyDesignation( ui32SlotIdx, pDinoInfo->_aui8Designation );

	//-------------------------------------------------
	// apply new hp
	//-------------------------------------------------
	iHP = pChara->GetCharaMaxHP( m_aui8PBVer[ui32SlotIdx] ); 
	pChara->ResetBattleInfo();
	m_pCastPacket_Chara_Life[ ui32SlotIdx ].iLife = iHP;				// life
	m_ar32LastDeathTimes	[ ui32SlotIdx ] = 0.0f;						// death

	pChara->SetAlive( TRUE );
	m_cs.Unlock();

	return TRUE;
}

void	CDediRoom::SetQACommand_C( P_BATTLE_QA_AUTO_REQ* pQAData )
{
	switch( pQAData->_ui8CommandId )
	{
	case QA_AUTO_COMMAND_ID_KILL_COUNT:
		{
			INT32 i32IncKillCount = pQAData->_i32Arg[0];	// 증가시킬 킬카운트

			QA_Command_KillCount(pQAData->_ui8SlotIdx,i32IncKillCount);
		}
		break;

	case QA_AUTO_COMMAND_ID_CHARACTER_MOVE:
		{
			// 캐릭터 이동 : 
			INT32 i32SlotIdx = pQAData->_i32Arg[0];			// 이동시킬 유저의 슬롯번호
			REAL32 r32x = *((REAL32*)&pQAData->_i32Arg[1]);	// 이동시킬 X좌표
			REAL32 r32y = *((REAL32*)&pQAData->_i32Arg[2]);	// 이동시킬 Y좌표
			REAL32 r32z = *((REAL32*)&pQAData->_i32Arg[3]);	// 이동시킬 Z좌표

			VEC3D vPos;
			i3Vector::Set(&vPos,r32x,r32y,r32z);
			QA_Command_Move_User(i32SlotIdx,&vPos);
			
		}
		break;

	case QA_AUTO_COMMAND_ID_DAMAGE_OBJECT:
		{
			INT32 i32Team = pQAData->_i32Arg[0];			// 데미지줄 오브젝트 팀 (레드/블루)
			INT32 i32Damage = pQAData->_i32Arg[1];			// 데미지수치
			
			QA_Command_Damage_Object(pQAData->_ui8SlotIdx,i32Team,i32Damage);
		}
		break;

	}
}


// round info 
void	CDediRoom::SetRoundInfo_C( P_BATTLE_ROUND_REQ* pRoundInfo )
{
	CCharacter * pChara;
	for ( UINT32 ui32SlotIdx = 0; ui32SlotIdx < SLOT_MAX_COUNT; ui32SlotIdx++ )
	{
		pChara = &m_pGameCharacters[ui32SlotIdx];
		// vv3 - 체크
		memcpy_s( pChara->GetAllParts(), sizeof(UINT32) * CHAR_EQUIPMENT_PARTS_COUNT, pRoundInfo->m_TPartsID[ui32SlotIdx], sizeof(pRoundInfo->m_TPartsID[ui32SlotIdx]) );
		pChara->SetSlotItem( pRoundInfo->_aui32Item[ui32SlotIdx] );
	}

	m_ui32FlagValidItem = 0;

	// vv3 - 체크
	for ( UINT32 ui32SlotIdx = 0; ui32SlotIdx < SLOT_MAX_COUNT; ui32SlotIdx++ )
	{
		pChara = &m_pGameCharacters[ui32SlotIdx];
		if( pChara->GetParts( CHAR_EQUIPMENT_PARTS_CHARA ) )
		{
			ApplyEquipment	( ui32SlotIdx, pChara->GetParts( CHAR_EQUIPMENT_PARTS_CHARA ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_HEAD ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_BERET ) );
			ApplyDesignation( ui32SlotIdx, pRoundInfo->_aui8Designation[ui32SlotIdx]);
			_ResetThrownWeaponInfo( ui32SlotIdx );
		}
	}
}

BOOL	CDediRoom::SetUserEquip_C ( STRUCT_BATTLE_SLOT_EQUIPMENT_REQ* pEquipInfo )
{	
	UINT32 ui32SlotIdx = pEquipInfo->m_ui8SlotIdx;
	CCharacter* pChara = GetGameCharacter( ui32SlotIdx );
	if( pChara == NULL ) return FALSE;
	// set max bullet
	pChara->SetSlotItem( pEquipInfo->m_Struct.m_EquipItem );
	pChara->InitBullets( m_aui8PBVer[ui32SlotIdx] );	

	// vv3 - 체크
	ApplyEquipment	( ui32SlotIdx, pChara->GetParts( CHAR_EQUIPMENT_PARTS_CHARA ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_HEAD ), pChara->GetParts( CHAR_EQUIPMENT_PARTS_BERET ) );
	ApplyDesignation( ui32SlotIdx, pEquipInfo->m_Struct.m_aui8EquipUserTitle );

	return TRUE;
}

BOOL CDediRoom::CheckRespawnTime( UINT32 ui32SlotIdx, UINT32 ui32SlotItem, REAL32 r32GameTime )
{
	if( m_ar32LastDeathTimes[ui32SlotIdx] > I3_EPS ) // 최초 리스폰인 경우는 시간 검사가 필요 없으므로
	{
		REAL32 r32RespawnMinTime = RESPAWN_MIN_TIME;

		if( ui32SlotItem & SLOT_ITEM_SHORT_RESPAWN_100 )	r32RespawnMinTime = 0.0f;
		else if( ui32SlotItem & SLOT_ITEM_SHORT_RESPAWN_50 )r32RespawnMinTime -= RESPAWN_MIN_TIME * VALUE_CASH_ITEM_SHORT_RESPAWN_50; // 0.5f
		else if( ui32SlotItem & SLOT_ITEM_SHORT_RESPAWN_30 )r32RespawnMinTime -= RESPAWN_MIN_TIME * VALUE_CASH_ITEM_SHORT_RESPAWN_30;
		else if( ui32SlotItem & SLOT_ITEM_SHORT_RESPAWN_20 )r32RespawnMinTime -= RESPAWN_MIN_TIME * VALUE_CASH_ITEM_SHORT_RESPAWN_20;

		if( r32RespawnMinTime < 0.0f )	r32RespawnMinTime = 0.0f;

		if( r32GameTime - m_ar32LastDeathTimes[ui32SlotIdx] < r32RespawnMinTime )
		{
			// count check
			if( m_pHMS->GetHackCount( ui32SlotIdx, HACK_TYPE_RESPAWN_TIME ) > MAX_HACK_RESPAWN_TIME_COUNT )
			{
				I3TRACE( "Respawn Time Hack !!\n" );
				EnqueueKickUser( ui32SlotIdx, HACK_TYPE_RESPAWN_TIME );
				return FALSE;
			}
		}
	}
	return TRUE;
}

// 주무기를 설정합니다
// 안들어 올수 있음 (받은 경우에만 패킷 정보로 overwrite )
BOOL CDediRoom::Respawn_C( P_BATTLE_RESPAWN_REQ* pRespawn )
{
	UINT32 ui32SlotIdx = pRespawn->_cSlotIdx;
	CCharacter* pChara = GetGameCharacter( ui32SlotIdx );
	if( pChara == NULL ) return FALSE; 

	REAL32 r32GameTime = GetGameTime();
	UINT32 ui32SlotItem = pChara->GetSlotItem();

	// 리스폰 타임 검사
	if( g_pConfig->GetCheckLevel(HACK_TYPE_RESPAWN_TIME) )
	{
		if( !CheckRespawnTime( ui32SlotIdx, ui32SlotItem, r32GameTime ) )
			return FALSE;
	}

	// hp 회복
	pChara->ResetBattleInfo();
	pChara->SetNaturalRPGGet(FALSE);
	getChara_Life( ui32SlotIdx )->iLife = pChara->GetHP();
	
	// 웨폰 정보 셋팅
	for( INT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; ++i )
	{
		pChara->InitWeapon( pRespawn->_pUseWeaponList[i], i );
	}
	pChara->InitBullets( m_aui8PBVer[ui32SlotIdx] ); // set max bullet

	// 웨폰 ID 셋팅
	pChara->setRespawnWeapon( pRespawn->_pUseWeaponList[ WEAPON_SLOT_PRIMARY ], WEAPON_SLOT_PRIMARY );
	pChara->setRespawnWeapon( pRespawn->_pUseWeaponList[ WEAPON_SLOT_SECONDARY ], WEAPON_SLOT_SECONDARY );

	// 웨폰 ID 변환
	UINT32 ui32WeaponID = pRespawn->_pUseWeaponList[ WEAPON_SLOT_PRIMARY ];
	
	N_PCINFO_WEAPON w;
	w.Reset();
	w.SetWeaponID( ui32WeaponID );
	SetChara_Weapon( ui32SlotIdx, &w );
	I3TRACE("%d: init primary weapon by resapwn(class:%d,number:%d)\n", pRespawn->_cSlotIdx, w.GetWeaponClass(), w.GetWeaponNumber());
	
	// 리스폰 인덱스 결정
	// 훈련전에서는 NPC 캐릭터, 인간 캐릭터 리스폰이 발생하지 않음(2012-01-09)
	I3ASSERT( IsTrainingRoom() == FALSE ); 
	
	// 리스폰 인덱스
	m_aui32RespawnIdx[ ui32SlotIdx ] = pRespawn->_i32TotalRespawnIdx;

	m_aui32RespawnCount[ ui32SlotIdx ] = pRespawn->_i32RespawnCount;

	// 현재시간 + 유저의 무적 시간을 더한 값을 저장하여 이 시간까지는 Hit 받지 않도록 처리하기 위해 추가
	REAL32 r32MinTime = INVINCIBILITY_MIN_TIME;

	if (IsChaosMode() == TRUE) r32MinTime = INVINCIBILITY_MIN_TIME_CHAOS_MODE;
	if( ui32SlotItem & SLOT_ITEM_INCREASE_INVINCIBLE_TIME ) r32MinTime += VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME;

	m_ar32InvincibleTime[ ui32SlotIdx ] = r32GameTime + r32MinTime;
	
	// C4 설치/해제 정보 초기화
	m_r32TriggerTime[ ui32SlotIdx ] = 0.0f;

	// 리스폰 하자마자는 서버에서 1회 세팅해줍니다.
	m_r32PosCheckTime[ ui32SlotIdx ] = r32GameTime;
	//어시스트 정보 리셋
	m_AssistInfo[ui32SlotIdx].Reset();
	
	// -------------------------------------------------
	// 플래그 셋팅
	// -------------------------------------------------
	
	ResetFlag( ui32SlotIdx );

	pChara->SetAlive( TRUE );
	return TRUE;
}

//// 기타 : 낙사, Fast Object
//BOOL	CDediRoom::_BuildDeathInfoBySuicide ( GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo,  UINT32 ui32SlotIdx, GAMEINFO_SUICIDEDAMAGE* pSuicideDamage ) 
//{
//	if( IS_VALID_MEMBER_RANGE(ui32SlotIdx) == FALSE)	return FALSE;
//	if( pDeathInfo == NULL || pSuicideDamage == NULL )	return FALSE;
//	
//	UDP_UTIL::ConvertNet3DtoVec3D( pDeathInfo->_pHitDir, &pSuicideDamage->getHitDirection() ); // 스코프 주의 : _pHitDir은 pDeathInfo과 scope가 같아야 함
//
//	// index
//	pDeathInfo->_iKillidx			= (INT32)ui32SlotIdx;
//	pDeathInfo->_iDeathidx			= (INT32)ui32SlotIdx;
//	// weapon
//	I3TRACE("CDediRoom::_BuildDeathInfoBySuicide() WeaponID =%d\n", pSuicideDamage->GetWeaponID() );
//	pDeathInfo->_ui32WeaponID		= pSuicideDamage->GetWeaponID();
//
//	// death type
//	pDeathInfo->_nDeathType			= (CHARA_DEATH_TYPE) pSuicideDamage->GetDeath();
//	pDeathInfo->_nHitPart			= (CHARA_HIT_PART)	 pSuicideDamage->GetHitPart(); // net info
//	
//	pDeathInfo->_iByObjectIdx		= pSuicideDamage->GetAttackerIndex();
//	
//	return TRUE;
//}

// 아웃포스트 헬기 조종사를 죽이기 위한 예외처리
BOOL	CDediRoom::_BuildDeathInfoForPilot( GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo,  UINT32 ui32SlotIdx, UINT32 ui32Pilot, N_PCINFO_HIT_WEAPON* pWeapon )
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )	return FALSE;
	if( !IS_VALID_MEMBER_RANGE(ui32Pilot) )		return FALSE;
	if( pDeathInfo == NULL ) return FALSE;
	
	// index
	pDeathInfo->_iKillidx			= (INT32)ui32SlotIdx;
	pDeathInfo->_iDeathidx			= (INT32)ui32Pilot;
	
	// weapon
	if( pWeapon )
	{
		I3TRACE("CDediRoom::_BuildDeathInfoForPilot() - Weapon != NULL ,WeaponID =%d\n", pWeapon->GetWeaponID() );
		pDeathInfo->_ui32WeaponID		= pWeapon->GetWeaponID();
	}
	else
	{
		// Killer의 무기를 Weapon Sync에서 얻음
		N_PCINFO_WEAPON* pWeaponInfo	= &m_pCastPacket_Chara_Weapon[ ui32SlotIdx ];
		I3TRACE("CDediRoom::_BuildDeathInfoForPilot() WeaponID =%d\n", pWeaponInfo->GetWeaponID() );
		pDeathInfo->_ui32WeaponID		= pWeaponInfo->GetWeaponID();
	}
	// deaty type
	pDeathInfo->_nDeathType			= CHARA_DEATH_O;
	pDeathInfo->_nHitPart			= (CHARA_HIT_PART)HitPartToNetIdx( CHARA_HIT_ROOT ); // net info

	return TRUE;
}

BOOL	CDediRoom::SetUserDeath( UINT32 ui32SlotIdx )
{
	CCharacter* pChara = GetGameCharacter( ui32SlotIdx );
	if( !pChara ) return FALSE;

	// ------------------------------------------------------
	// 컨트롤 플래그
	// ------------------------------------------------------
	ResetFlag( ui32SlotIdx );						// flag
	
	// ------------------------------------------------------
	// chara
	// ------------------------------------------------------
	pChara->SetAlive( FALSE );
	pChara->SetHP(0);										// HP만 리셋해야 함
	if( pChara->GetHaveObject() )
	{
		//////////////////////////////////////////////////////////////////////////////
		HACK_LOG hl;
		N_POS nPos;
		N_POS* pnPos = &nPos;
		N_PCINFO_POSROTATION *pCharaPos = &m_pCastPacket_Chara_PosRot[ ui32SlotIdx ];

		// 케릭터의 위치로 오브젝트를 옮깁니다.
		i3mem::Copy( pnPos, pCharaPos->GetPos(), sizeof(N_POS) );

		RaycastResult vResult;
		if (DoRayCast(&vResult, pCharaPos->GetPos()))
		{
			// Y 값을 보정합니다.
			pnPos->y = ((REAL16)i3Vector::GetY(&vResult.m_vIntersect));

			if (IsOverflow(pnPos))
			{
				VEC3D vPos;
				UDP_UTIL::ConvertNet3DtoVec3D(&vPos, pnPos);

				/*hl.AddInfo("ERROR Usurpation Pos1\ X :%f\t Y:%f\t Z:%f\t UID:[%I64d] SlotIdx:[%d]\n", pnPos->x, pnPos->y, pnPos->z, GetUID(ui32SlotIdx), ui32SlotIdx);
				char* msg = hl.MakeLogString(this, ui32SlotIdx, HACK_TYPE_USURPATION);
				if (msg) DSHACK(msg);*/

				// 수박 초기리스폰 위치로 돌립니다.
				pnPos = NULL;
			}
		}

		pChara->DeleteAllObject( pnPos );
	}
	SetHackType( ui32SlotIdx, HACK_TYPE_NO );				// 프리징 해제
	SetHackingReportState( ui32SlotIdx, FALSE );
	
	m_ar32LastDeathTimes [ ui32SlotIdx ] = GetGameTime();	// 죽은 시간 기록 

	// ------------------------------------------------------
	// death packet
	// ------------------------------------------------------
	m_ui32TcpSendFlag |= DS_SEND_USER_KILL;

	return TRUE;
}

BOOL CDediRoom::_BuildDeathInfoByHit(GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEFAULT* pHit, CHARA_DEATH_TYPE deathType, N_PCINFO_HIT_WEAPON* pWeapon, T_ItemID TItemID, INT32 AssistIdx)
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return FALSE;

	if( pDeathInfo == NULL || pHit == NULL )
		return FALSE;

	pDeathInfo->_iKillidx			= (INT32)ui32SlotIdx;							// Attacker
	pDeathInfo->_iDeathidx			= (INT32)pHit->GetIndex();						// Death
	pDeathInfo->_ui32WeaponID		= TItemID;
	pDeathInfo->_nDeathType			= deathType;									// Hit Type
	pDeathInfo->_nHitPart			= (CHARA_HIT_PART)pHit->GetHitPart();			// Hit Part : net info
	pDeathInfo->_iAssistIdx			= AssistIdx;
	
	return TRUE;
}

//오브젝트 폭팔 데스를 위한 함수
BOOL CDediRoom::_BuildDeathInfoByObjectExplosion(GAMETOCONTEXT_INFO_CHARA_DEATH* pDeathInfo,  UINT32 ui32SlotIdx, N_PCINFO_HIT_WEAPON* pWeapon )	
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )	
		return FALSE;

	if( pDeathInfo == NULL )
		return FALSE;

	if( pWeapon )
	{
		I3TRACE("CDediRoom::_BuildDeathInfoByObjectExplosion() - Weapon != NULL ,WeaponID =%d\n", pWeapon->GetWeaponID() );
		pDeathInfo->_ui32WeaponID		= pWeapon->GetWeaponID();
	}
	else
	{
		N_PCINFO_WEAPON* pWeaponInfo = &m_pCastPacket_Chara_Weapon[ ui32SlotIdx ];
		I3TRACE("CDediRoom::_BuildDeathInfoByObjectExplosion() - Weapon != NULL ,WeaponID =%d\n", pWeaponInfo->GetWeaponID() );	
		pDeathInfo->_ui32WeaponID		= pWeaponInfo->GetWeaponID();
	}

	pDeathInfo->_nDeathType			= CHARA_DEATH_O;	// Hit Type
	pDeathInfo->_nHitPart			= CHARA_HIT_ROOT;	// Hit Part : net info

	return TRUE;
}

// task processor에서 warning level로 패킷의 ip와 port를 남긴다.
BOOL	CDediRoom::CheckPacket( N_GAME_HEADER* pHeader, UINT32 ui32IP, UINT16 ui16Port )
{
	BOOL bRet = TRUE;
	
	m_cs.Lock();

	if( bRet )
	{// uid
		UINT32 ui32SlotIdx = pHeader->_Index;
		if( pHeader->_ui8UID != (UINT8)(GetUID( ui32SlotIdx ) % 256) )
		{
			DSMSG("[CDediRoom::CheckPacket] Check SlotIdx!!! UID 1byte: slot:%d, client:%d, server:%d\n", ui32SlotIdx, pHeader->_ui8UID, GetUID( ui32SlotIdx ) % 256 );
			bRet = FALSE;
		}
	}

	// member
	bRet = CheckMember( pHeader->_Index, ui32IP, ui16Port );
	if( bRet == FALSE )
	{
		DSMSG("[CDediRoom::CheckPacket] CheckMember fail (group:%d, slot:%d, IP:%d)\n", GetGroupIndex(), pHeader->_Index, ui32IP );
	}
	
	if( bRet )
	{// protocol
		if( ( pHeader->_ProtocolID & 0x7F ) != UDPPROTOCOL_CN_INFO )
		{
			DSMSG("[CDediRoom::CheckPacket] not UDPPROTOCOL_CN_INFO (U:%d)\n", pHeader->_Index );
			bRet = FALSE;
		}
	}
	
	if( bRet )
	{// round number
		if( pHeader->_RoundNum != GetRoundNumber() )
		{
			DSMSG("[CDediRoom::CheckPacket] Invalid Round Number. (U:%d, R:%d), battle=%d\n", pHeader->_Index, pHeader->_RoundNum, GetRoundNumber() );
			bRet = FALSE;
		}
	}
	
	if( bRet )
	{//packet time
		
		REAL32 r32Time0 = m_pr32LastPacketTime[ pHeader->_Index ]; // old time
		REAL32 r32Time1 = pHeader->_GameTime;	// new time
		
		if( r32Time0 < I3_EPS && r32Time1 > I3_EPS )
		{//init	
			I3TRACE("[CDediRoom::CheckPacket] m_pr32LastPacketTime Initialized by CheckPacket()\n");
			m_pr32LastPacketTime[ pHeader->_Index ] = r32Time1; 
			m_ar32OffsetTime	[ pHeader->_Index ] = GetGameTime() - r32Time1;
		}
		else if( r32Time1 < I3_EPS || r32Time1 < r32Time0 )
		{//error
			DSMSG("[CDediRoom::CheckPacket] Invalid Packet Time(G:%d,U:%d,Time0:%.1f,Time1:%.1f)\n", m_i16GroupIdx, pHeader->_Index, r32Time0, r32Time1);
			bRet = FALSE;
		}
		else
		{//success
			m_pr32LastPacketTime[ pHeader->_Index ] = r32Time1; // new time
		}
	}
	
	if( bRet )
	{//item 
		if( GetBit( m_ui32FlagValidItem, pHeader->_Index ) == FALSE )
		{	 
			DSMSG("[CDediRoom::CheckPacket] drop packet : invalid equipment. check resource (uid:%I64d)\n", GetUID(pHeader->_Index));
			bRet = FALSE;
		}
	}
	
	m_cs.Unlock();

	// 이 단계에서 수신 가능.
	// 송신가능은 첫번째 position을 받은 시점으로 변경(2012-03-22)
	
	return bRet;
}

// TRUE : Hacking
BOOL CDediRoom::_CheckHeadShotHack( UINT32 ui32SlotIdx, N_PCINFO_HIT_DEFAULT* pHitDefault )
{
	if( !g_pConfig->GetCheckLevel( HACK_TYPE_HEADSHOT )) return FALSE;
	
	// 헤드샷 데미지 검사
	CHARA_HIT_PART hitPart = NetIdxToHitPart( (CHARA_HIT_PART)pHitDefault->GetHitPart() );
	
	if( hitPart == CHARA_HIT_HEAD_OLD ||  hitPart == CHARA_HIT_HEAD ) 
	{
		UINT32  ui32PacketDamage = pHitDefault->GetDamage();
		
		//I3TRACE("[CDediRoom::_CheckHeadShotHack] packet head shot damage=%d\n", ui32PacketDamage);
		
		if( ui32PacketDamage > 255 ) return TRUE;

		if ( IsChaosMode() ) return FALSE; // 맥스치 255 넘어가는게 아니면 카오스 모드는 전부 정상 처리합니다.

		UINT32 ui32RespawnCount0 = m_aui32RespawnCount[ ui32SlotIdx ];
		UINT32 ui32RespawnCount1 = m_aui32RespawnCount[ ui32SlotIdx ] + 1;
		UINT32 ui32RespawnCount2 = m_aui32RespawnCount[ ui32SlotIdx ] - 1;
	
		if (	( ui32PacketDamage != CalcHeadShotDamage( m_aui8HitPartIndex, sizeof(m_aui8HitPartIndex),  ui32RespawnCount0, ui32SlotIdx )) &&
				( ui32PacketDamage != CalcHeadShotDamage( m_aui8HitPartIndex, sizeof(m_aui8HitPartIndex),  ui32RespawnCount1, ui32SlotIdx )) &&
			    ( ui32PacketDamage != CalcHeadShotDamage( m_aui8HitPartIndex, sizeof(m_aui8HitPartIndex),  ui32RespawnCount2, ui32SlotIdx )) 
			)
		{
			//DSERROR("head shot hack\n");
			return TRUE;
		}
	}
	
	return FALSE;
}

// TRUE : Hacking

BOOL CDediRoom::_CheckObjectExplosionRange(UINT32 ui32ObjectIdx , UINT32 ui32SlotIdx)
{
	VEC3D vCharaPos, vObjPos;
	VEC3D vDir;

	UDP_UTIL::ConvertNet3DtoVec3D( &vCharaPos, &getChara_PosRot(ui32SlotIdx)->_Pos );
	*((NxVec3*)&vObjPos) = m_aGameObjects[ ui32ObjectIdx ].m_vCenterPos;

	i3Vector::Sub( &vDir, &vObjPos, &vCharaPos );

	I3TRACE("오브젝트 좌표 x : %f y : %f z:%f \n", vObjPos.x,vObjPos.y,vObjPos.z);
	I3TRACE("캐릭터  좌표 x : %f y : %f z:%f \n", vCharaPos.x,vCharaPos.y,vCharaPos.z);

	REAL32 r32LengthSqr =  i3Vector::LengthSq( &vDir );
	I3TRACE("fLengthSqr  :  %f \n", r32LengthSqr);
	
	if( 25.f < r32LengthSqr )
	{
		DSERROR("Object Explosion Error \n");
		return TRUE;
	}

	return FALSE;
}

// TRUE : Hacking

BOOL CDediRoom::_CheckEnableHelicopterExplosion(UINT32 ui32SlotIdx,UINT16 ui16ObjectIdx)
{
	if (m_pGameCharacters[ui32SlotIdx].GetOnLoad() && m_pGameCharacters[ui32SlotIdx].GetOnLoadIndex() == ui16ObjectIdx)
	{
		return FALSE;
	}
	return TRUE;
}




// TRUE : Hacking
BOOL   CDediRoom::_CheckGravityHack( UINT32 ui32SlotIdx, N_POS_R* pKillerPos )
{
	I3ASSERT( IsOverflow( pKillerPos ) == FALSE ); // hit packet의 포지션 검사됨.
	
	STAGE_UID eStageUID = GetStageUID();
	if( eStageUID == STAGE_UID_TUTORIAL ) return FALSE;
	else if( eStageUID == STAGE_UID_FACTORYRUINS ) return FALSE;

	if( g_pConfig->GetCheckLevel(HACK_TYPE_GRAVITY) )
	{
		NxVec3 NxPos = NxVec3 ( pKillerPos->GetX(), pKillerPos->GetY(), pKillerPos->GetZ() );

		if( !g_StageBounds[eStageUID].contain(NxPos) )
		{
			{//hack log	
				HACK_LOG hl;
				hl.AddInfo( "HACK_TYPE_GRAVITY\t Pos(%.2f %.2f \t %.2f)\n", NxPos.x, NxPos.y, NxPos.z);
				char* msg = hl.MakeLogString( this, ui32SlotIdx, HACK_TYPE_GRAVITY );
				if( msg) DSHACK(msg);
			}

			return TRUE;
		}
	}
	
	return FALSE;
}

// TRUE: hacking
BOOL   CDediRoom::CheckTouchDown( UINT32 ui32SlotIdx, UINT16 ui32ObjectIdx)
{
	CDSObject* pObjectInfo = GetGameObject( ui32ObjectIdx);

	if( pObjectInfo != NULL )
	{
		if (pObjectInfo->GetSType() ==  OBJECT_S_TYPE_ESCAPE )
		{

			VEC3D vCharaPos, vObjPos;
			VEC3D vDir;
			UDP_UTIL::ConvertNet3DtoVec3D( &vCharaPos, &getChara_PosRot(ui32SlotIdx)->_Pos );
			*((NxVec3*)&vObjPos) = m_aGameObjects[ ui32ObjectIdx ].m_vCenterPos;

			i3Vector::Sub( &vDir, &vObjPos, &vCharaPos );

			REAL32 r32LengthSqr =  i3Vector::LengthSq( &vDir );


			if (r32LengthSqr < 9.f)
				return FALSE;

		}

	}

	return TRUE;
}

									
BOOL	CDediRoom::_EnqueueRaycastRequest	( UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pFinalHit )  // send msg to map
{
	// cc_mode
	// 공룡일 경우 howling, explosion 공격이 아닌 경우에는 wall shot test 하도록 수정할 것.	
	CMapData* pMap = g_pMapManager->GetMap( m_ui32StageID );
	if( pMap == NULL ) return FALSE;

	return pMap->EnqueueRaycastRequest( GetGroupIndex(), ui32SlotIdx, pFinalHit );
}


// task에서 해킹 판단을 한 경우
void	CDediRoom::EnqueueKickUser( UINT32 ui32SlotIdx, DS_HACK_TYPE HackType )
{
	I3ASSERT( HackType != HACK_TYPE_NO );
	
	if ( IsHackUser( ui32SlotIdx ) )
	{
		return;
	}
	
	DSERROR("MSG. user %I64d is hack user : DS_HACK_TYPE=%d\n", GetUID(ui32SlotIdx), HackType );

	// set hack info
	m_cs.Lock();
	
	SetHackType( ui32SlotIdx, HackType ); 
	m_ui32TcpSendFlag |= DS_SEND_CHEATING_REPORT;
	
	m_cs.Unlock();
	
	return;
}

void	CDediRoom::CheckClientTime( REAL32 r32GameTime )
{
	if( ! g_pConfig->GetCheckLevel(HACK_TYPE_CLIENT_TIME) ) return;
	if( m_r32LastCheckTime + 10.0f > r32GameTime )	return;
	
	m_r32LastCheckTime = r32GameTime;
	
	for( UINT32 ui32SlotIdx = 0; ui32SlotIdx < SLOT_MAX_COUNT; ui32SlotIdx++ )
	{
		if( !IsSendEnable(ui32SlotIdx)) continue;
		if( IsHackUser(ui32SlotIdx)) continue;
		
		REAL32 r32TimeDiff = m_pr32LastPacketTime[ui32SlotIdx] - ( r32GameTime - m_ar32OffsetTime[ui32SlotIdx] );
		
		//I3TRACE("[%d] CheckClientTime TIME DIFF = %f\n", ui32SlotIdx, fTimeDiff );
		
		if( r32TimeDiff > 50.0f )
		{
			HACK_LOG hl;
			hl.AddInfo(" last_packet_time=%.1f, game_time=%.1f, offset_time=%.1f, fTimeDiff=%.1f", 
							m_pr32LastPacketTime[ui32SlotIdx],
							r32GameTime,
							m_ar32OffsetTime[ui32SlotIdx],
							r32TimeDiff );
			char* msg = hl.MakeLogString( this, ui32SlotIdx, HACK_TYPE_CLIENT_TIME );
			if( msg) DSERROR(msg);
			
			EnqueueKickUser( ui32SlotIdx, HACK_TYPE_CLIENT_TIME ); 
		}
	}
}

// 이부분은 게임서버/클라 수정 이후에 실제로 끊도록 수정할 것.
// CGameCharaManager::_GetRespawnPosition (수정:2012-01-12, 2012-03-07 )
BOOL	CDediRoom::IsValidRespawnPos ( UINT32 ui32SlotIdx, UINT32 ui32TotalRespawnIdx )
{

	//QA커맨드
	if(IsQaCommandCheatUser(ui32SlotIdx)) return TRUE;
		
	//헬리스팟은 검사대상에서 제외(움직이는 헬리콥터 때문인듯. 가능하다면 차후 수정요망.)
	if( GetStageUID() == STAGE_UID_HELISPOT ) return TRUE;

	INT32 i32Team = ui32SlotIdx % 2;

	if( m_i32TeamChanged )
	{
		i32Team = !i32Team;
	}

	INT32  nRandConst = m_pRespawnMgr->GetRespawnCount( i32Team ); // red/blue only
	INT32  nRand = 0;

	I3ASSERT( nRandConst > 0 );
	if( nRandConst == 0 )
	{
		//로그 계속 찍혀도 할 수 없음.
		DSERROR("CHECK RESOURCE(*I3SRPN)...STAGE_UID:%d\n", GetStageUID());
		return TRUE; 
	}
		
	if( IsBombMissionMode() || IsAnnihilationMode() )
	{
		INT32 roomIdx		= m_RoomInfo._sRoom;
		INT32 roundCount	= GetRoundNumber();
		
		nRand				= ( roomIdx + roundCount + ui32SlotIdx / 2)	% nRandConst;
	}
	else
	{
		nRand		= ( ui32TotalRespawnIdx / 2 ) % nRandConst;
	}
	
	I3TRACE("CHARA RESPAWN INDEX = %d, ui32TotalRespawnIdx=%d\n", nRand, ui32TotalRespawnIdx );
	
	const NxVec3* pRespawnPos = m_pRespawnMgr->GetRespawnPosition( i32Team, nRand );
	if( pRespawnPos ) 
	{	
		NxVec3 vRespawnPos(*pRespawnPos);
		vRespawnPos.y += 0.2f; 
		// 월드에 붙여서 리스폰시 캐릭터가 월드에 뚫리는 문제가 발생합니다.
		// 월드에서 20센티 띄워서 리스폰 시킵니다. (클라주석)

		//I3TRACE("[%d]RESPAWN POS:%f,%f,%f, CHARA POS:%f,%f,%f\n", ui32SlotIdx, vRespawnPos.x, vRespawnPos.y, vRespawnPos.z);
		BOOL bResult = m_respawnState.CheckRespawnPos( ui32SlotIdx, &vRespawnPos, m_aui32RespawnCount[ui32SlotIdx] );
		if( !bResult )
		{
			if( m_pHMS->GetHackCount(ui32SlotIdx, HACK_TYPE_RESPAWN_POSITION ) > MAX_HACK_RESPAWN_POS_COUNT )
			{
				CCharacter*	pChara	= GetGameCharacter( ui32SlotIdx );
				N_POS*				pPos	= &pChara->GetPosRot()->_Pos;
				
				VEC3D vCharaPos;
				UDP_UTIL::ConvertNet3DtoVec3D( &vCharaPos, pPos );
				
				HACK_LOG hl;
				hl.AddInfo( "HACK_TYPE_RESPAWN_POSITION\t Respawn idx=%d\t nRand :%d\t respawn pos=(%.2f,%.2f,%.2f)\t Last chara pos=(%.2f,%.2f,%.2f)\n", 
							ui32TotalRespawnIdx,nRand,
							vRespawnPos.x, vRespawnPos.y, vRespawnPos.z, 
							vCharaPos.x, vCharaPos.y, vCharaPos.z );

				char* msg = hl.MakeLogString( this, ui32SlotIdx, HACK_TYPE_RESPAWN_POSITION );
				if( msg ) DSHACK(msg);

				return FALSE;
			}
		}
		return TRUE;
	}
	else
	{
		DSERROR("[m_pRespawnMgr->GetRespawnPosition() == NULL] STAGE_UID:%d, slot:%d, total respawn index=%d\n", GetStageUID(), ui32SlotIdx, ui32TotalRespawnIdx );
	}
				
	return TRUE;
}

// Round Number setup후에 호출해야 한다.
void	CDediRoom::ResetRespawnHelicopter() 
{
	// 초기 path
	// new version

	if( m_DestructionData.m_pObjectBlue != NULL )
	{
		m_DestructionData.m_pObjectBlue->setPathKey( static_cast<UINT8>( CalcAnimObjPath(m_aui8HitPartIndex, GetRoundNumber(), TEAM_BLUE, -1) ) );
	}
	
	if( m_DestructionData.m_pObjectRed != NULL )
	{		
		m_DestructionData.m_pObjectRed->setPathKey( static_cast<UINT8>( CalcAnimObjPath(m_aui8HitPartIndex, GetRoundNumber(), TEAM_RED, -1) ) );
	}
}

void CDediRoom::ApplySkills(UINT32 ui32SlotIdx, CHARA_SLOT_SKILL* pSkills)
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return;

	m_pGameCharacters[ui32SlotIdx].ApplySkills(pSkills);
}
// DFD Rate, Bome Range 만 적용
void CDediRoom::ApplyDesignation( UINT32 ui32SlotIdx, UINT8* pui8DesignationInfo )
{
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
	if( ui32SlotIdx < SLOT_MAX_COUNT )
	{
		m_ar32DisigDFDRate [ ui32SlotIdx ] = m_ar32BombRangePlus[ ui32SlotIdx ] = m_ar32DesignationSpeed[ui32SlotIdx] = 0.0f;
			
		for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++ )
		{
			I3TRACE("Designation[%d]=%d\n", i, pui8DesignationInfo[ i ] );
			
			// + 효과
			if		( pui8DesignationInfo[i] == 44 ) m_ar32BombRangePlus[ ui32SlotIdx ] += 0.7f; //lv1
			else if	( pui8DesignationInfo[i] == 43 ) m_ar32BombRangePlus[ ui32SlotIdx ] += 0.7f; //lv1
			else if	( pui8DesignationInfo[i] == 42 ) m_ar32BombRangePlus[ ui32SlotIdx ] += 0.7f; //lv1
			else if ( pui8DesignationInfo[i] == 29 ) m_ar32DesignationSpeed[ui32SlotIdx]+= 0.2f; //lv3
			else if ( pui8DesignationInfo[i] == 28 ) m_ar32DesignationSpeed[ui32SlotIdx]+= 0.2f; //lv2
			else if ( pui8DesignationInfo[i] == 27 ) m_ar32DesignationSpeed[ui32SlotIdx]+= 0.1f; //lv2
			else if ( pui8DesignationInfo[i] == 26 ) m_ar32DesignationSpeed[ui32SlotIdx]+= 0.1f; //lv1
			else if	( pui8DesignationInfo[i] == 19 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (-40.0f * 0.01f); //lv4
			else if ( pui8DesignationInfo[i] == 18 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (-30.0f * 0.01f); //lv3
			else if ( pui8DesignationInfo[i] == 17 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (-30.0f * 0.01f); //lv3
			else if ( pui8DesignationInfo[i] == 16 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (-20.0f * 0.01f); //lv2
			else if ( pui8DesignationInfo[i] == 15 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (-20.0f * 0.01f); //lv2
			else if ( pui8DesignationInfo[i] == 14 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (-10.0f * 0.01f); //lv1
			else if ( pui8DesignationInfo[i] ==  6 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (-10.0f * 0.01f); //lv1
			// - 효과
			if		( pui8DesignationInfo[i] == 13 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (40.0f * 0.01f); //lv4
			else if ( pui8DesignationInfo[i] == 12 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (20.0f * 0.01f); //lv2
			else if ( pui8DesignationInfo[i] == 11 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (30.0f * 0.01f); //lv3
			else if ( pui8DesignationInfo[i] == 10 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (10.0f * 0.01f); //lv1
			else if ( pui8DesignationInfo[i] ==  9 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (20.0f * 0.01f); //lv2
			else if ( pui8DesignationInfo[i] ==  5 ) m_ar32DisigDFDRate[ ui32SlotIdx ] += (10.0f * 0.01f); //lv1
		}

		I3TRACE("m_ar32DisigDFDRate[%d]=%f, m_ar32BombRangePlus[%d]=%f\n", 
						ui32SlotIdx, m_ar32DisigDFDRate[ ui32SlotIdx ],
						ui32SlotIdx, m_ar32BombRangePlus[ ui32SlotIdx ] );
	}
}
BOOL CDediRoom::ApplyEquipment	( UINT32 ui32SlotIdx, UINT32 ui32CharaID, UINT32 ui32HelmetID, UINT32 ui32BeretID )
{
	I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
		
	BOOL bValid = TRUE;
	
	if( ui32CharaID )
	{
		if( !CharacterTable::GetInstance().Find( ui32CharaID, m_aCharaInfo[ ui32SlotIdx ] ))
		{
			DSERROR("ERR. character not found! uid:%I64d, charaID:%d\n", GetUID( ui32SlotIdx ), ui32CharaID );
			bValid = FALSE;
		}

		if( bValid && ui32HelmetID && !EquipmentTable::GetInstance().Find( ui32HelmetID, m_aHelmet[ ui32SlotIdx ], (PB_VER)m_aui8PBVer[ui32SlotIdx] ))
		{
			DSERROR("ERR. equipment not found! uid:%I64d, equipmentID:%d\n", GetUID( ui32SlotIdx ), ui32HelmetID );
			bValid = FALSE;
		}

		if( bValid ) 
		{
			SetBit( &m_ui32FlagValidItem, ui32SlotIdx );
		}
	}
	else
	{
		bValid = FALSE;
	}

	if( ! bValid )
	{
		m_aCharaInfo[ ui32SlotIdx ].Clear();
		m_aHelmet[ ui32SlotIdx ].ClearAll();
		m_aBeret[ ui32SlotIdx ].ClearAll();
		
		ClearBit( &m_ui32FlagValidItem, ui32SlotIdx );
		
		if( g_pConfig->GetCheckLevel( HACK_TYPE_ITEM ) )
		{
			HACK_LOG hl;
			hl.AddInfo("HACK_TYPE_ITEM\t CharaID=%d\t HelmetID=%d\t BeretID=%d\n", ui32CharaID, ui32HelmetID, ui32BeretID );
			char* msg = hl.MakeLogString( this, ui32SlotIdx, HACK_TYPE_ITEM );
			if( msg ) DSHACK( msg );

			EnqueueKickUser( ui32SlotIdx, HACK_TYPE_ITEM );
		}
	}
	
	return bValid;
}

// send life : suicide, hit by explosion으로 죽지 않은 경우 send를 여러번 보내서 클라이언트와 즉시 hp를 sync한다.
//	Note.	다치고 바로 죽은 경우에는 HP 0을 보낸 이후에 max HP가 전송된다. 이는 서버에서 1초후에 HP를 
//			강제로 max hp로 회복 시키기 때문이다.
void CDediRoom::SendLife(UINT32 ui32SlotIdx, UINT32 uInfoFlag)
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return;

	UINT32 ui32Count = 1;
	if( uInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION )
		ui32Count =  DS_SEND_LIFE_COUNT;
	else if( uInfoFlag  & P2P_INFO_FLAG_SUICIDEDAMAGE )
		ui32Count =  DS_SEND_LIFE_COUNT;

	N_PCINFO_LIFE* pLife = getChara_Life( ui32SlotIdx );	
	pLife->iLife = m_pGameCharacters[ ui32SlotIdx ].GetHP();

	getChara_Check( ui32SlotIdx )->nP2PInfoFlag |= P2P_INFO_FLAG_CS_LIFE; 
	getChara_Check( ui32SlotIdx )->bCheckBroadcast = true;

	_SetSendLife( ui32SlotIdx, ui32Count );
}


// REF: CGameCharaCameraContext::OnUpdate()
CHARA_STATE_FOR_CAM		CDediRoom::_GetCharaStateForCam( UINT32 ui32State )
{
	UINT8 ui8Lower, ui8Body;
	
	UDP_UTIL::ConvertNetActiontoChara( NULL, NULL, NULL, NULL, &ui8Body, NULL, NULL, &ui8Lower, NULL, NULL, NULL, ui32State);
	// body 인지 body2인지 확인
	
	CHARA_STATE_FOR_CAM nNewTargetState = CSFC_UNKNOWN;

	if( (CHARA_ACTION_LOWER)ui8Lower == CHARA_LOWER_RUN || (CHARA_ACTION_LOWER)ui8Lower == CHARA_LOWER_WALK )
	{
		if( (CHARA_ACTION_BODYLOWER)ui8Body == CHARA_BODYLOWER_STAND )	//	서 있으면
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( (CHARA_ACTION_BODYLOWER)ui8Body == CHARA_BODYLOWER_CROUCH )	//	앉아 있으면
		{
			nNewTargetState = CSFC_SITMOVE;
		}
	}
	else if( (CHARA_ACTION_LOWER)ui8Lower == CHARA_LOWER_JUMP )
	{
		nNewTargetState = CSFC_JUMP;
	}
	else
	{
		if( (CHARA_ACTION_BODYLOWER)ui8Body == CHARA_BODYLOWER_STAND )	//	서 있으면
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( (CHARA_ACTION_BODYLOWER)ui8Body == CHARA_BODYLOWER_CROUCH )	//	앉아 있으면
		{
			nNewTargetState = CSFC_SIT;
		}
	}
	
	return nNewTargetState;
}

INT32 CDediRoom::CalcShotgunHitCount(REAL32 rDistance)
{
	UINT32 ui32DistanceLV = getShotGunDistanceLv(rDistance);
	if( HIT_DISTANCE_MAX <= ui32DistanceLV )
		ui32DistanceLV = HIT_DISTANCE_MAX - 1;

	return ShotGunHit[ui32DistanceLV];
}


UINT32  CDediRoom::getShotGunDistanceLv(REAL32 rDistance)
{

	UINT32 ui32Distance;

	ui32Distance = ((UINT32)rDistance>>2);
	
	I3TRACE("거리 : %f 샷건 레벨 반환: %d\n ", rDistance, ui32Distance);

	return ui32Distance;

}


void	CDediRoom::WriteWallShotResult( DS_CHECK_WALLSHOT_RESULT* pResult ) // 함수이름. EnqueuHackReport..로 통일
{
	// hack user가 battle 중인지 확인. roud num, respawn count check.
	if( _IsHackUserInBattle( pResult->m_ui8ByteUID, pResult->m_ui8SlotIdx, pResult->m_ui8RoundNum, pResult->m_ui8RespawnCount ) )
	{
		EnqueueKickUser( pResult->m_ui8SlotIdx, (DS_HACK_TYPE)pResult->m_ui32Result );
	}
}

void CDediRoom::KickUserBattle( INT32 i32WorkIdx, UINT32 ui32SlotIdx, INT64 i64UID )
{
	return;
	m_bKickUserBattle[ui32SlotIdx] = TRUE;
	g_pModuleCast->Send_PROTOCOL_BATTLE_KICK_USER_BATTLE_ACK( i32WorkIdx, &GetRoomInfo(), ui32SlotIdx, i64UID );
}

INT32 CDediRoom::GMPause_C()
{
	// C4 설치, 해제 중인지 검사합니다.
	switch( m_MissionState )
	{
	case NET_MISSION_TRIGGER_ARM_START:		// C4 설치중에는 PAUSE 불가
	case NET_MISSION_TRIGGER_DISARM_START:	// C4 해제중에는 PAUSE 불가
	case NET_MISSION_TRIGGER_DISARM_DONE:	// C4 해제완료후에는 PAUSE 불가
		return EVENT_ERROR_GM_PAUSE_FAIL_REASON_C4_INSTALL;
	default:
		break;
	}

	// Pause 모드 ON
	m_bGMPause = TRUE;
	//i3mem::FillZero( m_aui32RecvOnPause, sizeof( m_aui32RecvOnPause ) );

	//// Pause가 되었을때 해킹검사를 위해 플레이 중인 유저들의 좌표를 저장합니다.
	//m_ui32PauseMember = 0;
	//for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	//{
	//	if ( !GetMember(i)->GetPlaying() )			continue;

	//	m_ui32PauseMember |= (1 << i);
	//	m_ar32PausePosX[i] = (REAL32)getChara_PosRot(i)->_Pos.x;
	//	m_ar32PausePosZ[i] = (REAL32)getChara_PosRot(i)->_Pos.z;
	//}
 
	return EVENT_ERROR_SUCCESS;
}

void CDediRoom::GMResume_C()
{
	// Pause 모드 OFF
	m_bGMPause = FALSE;
}

void CDediRoom::DeathObjectExplosion(UINT32 ui32KillSlotIdx, UINT32 ui32DeathSlotIdx, UINT32 ui32ObjectIdx, N_PCINFO_HIT_WEAPON* pWeapon)
{
	if( 0 < m_pGameCharacters[ui32DeathSlotIdx].SubHP(255) )
		return;

	SetUserDeath( ui32DeathSlotIdx ); // death table 적용 + 시간 + hp

	GAMETOCONTEXT_INFO_CHARA_DEATH deathInfo;
	deathInfo.Reset();
	VEC3D vDir;
	i3Vector::Set( &vDir, 0.0f, 0.0f, 0.0f);
	deathInfo._pHitDir = &vDir;
	deathInfo._iKillidx = ui32KillSlotIdx;
	deathInfo._iDeathidx = ui32DeathSlotIdx;
	deathInfo._iByObjectIdx= ui32ObjectIdx;
	_BuildDeathInfoByObjectExplosion( &deathInfo, ui32DeathSlotIdx, pWeapon );

	INT32 assistidx = m_AssistInfo[ui32DeathSlotIdx].GetAssistidx(ui32KillSlotIdx);	
	deathInfo._iAssistIdx = assistidx;

	_Write_Chara_Death( &deathInfo );
}

//void CDediRoom::CheckGMPausePos( UINT32 ui32SlotIdx )
//{
//	if ( !m_bGMPause )											return;
//	if ( !( m_ui32PauseMember & (1 << ui32SlotIdx) ) )			return;
//	if ( !g_pConfig->GetCheckLevel( HACK_TYPE_PAUSE_MOVE ) )	return;
//
//	REAL32 r32XDis = (REAL32)getChara_PosRot(ui32SlotIdx)->_Pos.x - m_ar32PausePosX[ui32SlotIdx];
//	REAL32 r32ZDis = (REAL32)getChara_PosRot(ui32SlotIdx)->_Pos.z - m_ar32PausePosZ[ui32SlotIdx];
//
//	if ( r32XDis*r32XDis + r32ZDis*r32ZDis > GM_PAUSE_MOVE_DISTANCE*GM_PAUSE_MOVE_DISTANCE )
//	{
//		m_ar32PausePosX[ui32SlotIdx] = (REAL32)getChara_PosRot(ui32SlotIdx)->_Pos.x;
//		m_ar32PausePosZ[ui32SlotIdx] = (REAL32)getChara_PosRot(ui32SlotIdx)->_Pos.z;
//		EnqueueKickUser( ui32SlotIdx, HACK_TYPE_PAUSE_MOVE ); 
//	}
//}

//void CDediRoom::CheckGMPauseShoot( UINT32 ui32SlotIdx )
//{
//	if ( !g_pConfig->GetCheckLevel( HACK_TYPE_PAUSE_SHOOT ) )	return;
//
//	m_aui32RecvOnPause[ui32SlotIdx]++;
//	if ( m_aui32RecvOnPause[ui32SlotIdx] > GM_PAUSE_SHOOT_COUNT )
//	{
//		m_aui32RecvOnPause[ui32SlotIdx] = 0;
//		EnqueueKickUser( ui32SlotIdx, HACK_TYPE_PAUSE_SHOOT );
//	}
//}

TEAM_TYPE CDediRoom::GetAllDeathTeam(void)
{
	BOOL bBlueAllDeath	= TRUE;
	BOOL bRedAllDeath	= TRUE;

	for (INT32 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		if (!GetUserAlive(i)) continue;

		if (m_i32TeamChanged == i % 2)	bRedAllDeath	= FALSE;
		else							bBlueAllDeath	= FALSE;
	}

	if (TRUE == bRedAllDeath && TRUE == bBlueAllDeath)	return TEAM_BOTH;	// if문 순서가 변경되면 안 됩니다. 
	if (TRUE == bRedAllDeath)							return TEAM_RED;
	if (TRUE == bBlueAllDeath)							return TEAM_BLUE;

	return TEAM_NONE;

}

BOOL CDediRoom::RoundEndChkAnnihilation()
{
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;
	UINT32 ui32NowTime = static_cast<UINT32>( m_r32BattleTime - m_r32RoundStartTime );

	if( ui32NowTime > g_TeamTimeTable[ui8EndTimeIdx] )
	{
		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		m_ui8RoundWinTeam	= TEAM_DRAW;
		return TRUE;
	}

	// 라운드 시작 후 5초 동안은 끝나지 못하게 함. 유저 리스폰 시간을 기다림
	if( ui32NowTime < 5 ) return FALSE;

	switch (GetAllDeathTeam())
	{
	case TEAM_BOTH:
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_DRAW;
		return TRUE;

	case TEAM_RED:
		if (m_bCanSoloPlay)	break;
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_BLUE_WIN;
		return TRUE;

	case TEAM_BLUE:
		if (m_bCanSoloPlay)	break;
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_RED_WIN;
		return TRUE;
	}

	return FALSE;
}

BOOL CDediRoom::RoundEndChkBomb()
{
	BOOL bRoundEnd = FALSE;
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;
	UINT32 ui32NowTime = static_cast<UINT32>( m_r32BattleTime - m_r32RoundStartTime );

	// 라운드 시작 후 5초 동안은 끝나지 못하게 함. 유저 리스폰 시간을 기다림
	if( ui32NowTime < 5 ) return FALSE;

	if( -1 != m_MissionIntruder.getInstallerIndex() )
	{
		// UnInstall
		if( m_MissionIntruder.getInstallMode() == 0 )
		{
			m_ui8RoundEndType	= MISSION_END_UNINSTALL;
			m_ui8RoundWinTeam	= TEAM_BLUE_WIN;
			bRoundEnd			= TRUE;
		}
		else	// Install
		{
			if( m_r32BattleTime - m_r32BombInstallTime > m_r32BombExplosionDelayTime )
			{
				m_ui8RoundEndType	= MISSION_END_BOMBFIRE;
				m_ui8RoundWinTeam	= TEAM_RED_WIN;
				bRoundEnd			= TRUE;
			}
		}
	}

	if( FALSE == bRoundEnd )
	{
		if( ui32NowTime > g_TeamTimeTable[ui8EndTimeIdx] )
		{
			m_ui8RoundEndType	= MISSION_END_TIMEOUT;
			m_ui8RoundWinTeam	= TEAM_BLUE_WIN;
			bRoundEnd			= TRUE;
		}
	}

	if( FALSE == bRoundEnd )
	{
		switch( GetAllDeathTeam() )
		{
		case TEAM_BOTH:
			if( SERVER_VALUE_UNKNOWN == m_MissionIntruder.getInstallerIndex() )
			{	// 폭탄 설치가 되어있지 않으면 블루 승
				m_ui8RoundWinTeam = TEAM_BLUE_WIN;
				m_ui8RoundEndType = MISSION_END_ALLDEATH;
				bRoundEnd = TRUE;
			}
			//현재 남은 시간 > 폭탄 터질 시간e	
			else if( (g_TeamTimeTable[ui8EndTimeIdx] - ui32NowTime) > ( m_r32BombExplosionDelayTime - (m_r32BattleTime - m_r32BombInstallTime) ) )
			{	// 폭탄이 터질 시간이 남아있으면 레드 승
				m_ui8RoundWinTeam = TEAM_RED_WIN;
				m_ui8RoundEndType = MISSION_END_ALLDEATH;
				bRoundEnd = TRUE;
			}
			else
			{	// 아니면 블루 승
				m_ui8RoundWinTeam = TEAM_BLUE_WIN;
				m_ui8RoundEndType = MISSION_END_ALLDEATH;
				bRoundEnd = TRUE;
			}
			break;

		case TEAM_RED:
			if (m_bCanSoloPlay)	break;
			if (SERVER_VALUE_UNKNOWN != m_MissionIntruder.getInstallerIndex())	break;
			// 폭탄이 설치되어 있지 않으면 게임 끝
			m_ui8RoundWinTeam = TEAM_BLUE_WIN;
			m_ui8RoundEndType = MISSION_END_ALLDEATH;
			bRoundEnd = TRUE;
			break;

		case TEAM_BLUE:
			if (m_bCanSoloPlay)	break;
			m_ui8RoundWinTeam = TEAM_RED_WIN;
			m_ui8RoundEndType = MISSION_END_ALLDEATH;
			bRoundEnd = TRUE;
			break;
		}

	}

	// 아직 종료 시점이 되지 않았다.
	if( FALSE == bRoundEnd ) return FALSE;

	return TRUE;
}

BOOL CDediRoom::RoundEndChkCrossCount()
{
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;

	if( m_r32BattleTime > g_TeamTimeTable[ui8EndTimeIdx] + m_r32RoundStartTime )
	{
		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		m_ui8RoundWinTeam	= TEAM_DRAW;
		return TRUE;
	}

	return FALSE;
}

BOOL CDediRoom::RoundEndChkDeathMatch()
{
	UINT8 ui8EndKillIdx = m_ui8RoomSubType & 0x0F; 
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;

	if( m_r32BattleTime - m_r32RoundStartTime > g_TeamTimeTable[ui8EndTimeIdx] )
	{
		m_ui8RoundWinTeam = TEAM_DRAW;

		if( m_TeamKillCount[ TEAM_RED ] > m_TeamKillCount[ TEAM_BLUE ] )	m_ui8RoundWinTeam = TEAM_RED_WIN;
		if( m_TeamKillCount[ TEAM_RED ] < m_TeamKillCount[ TEAM_BLUE ] )	m_ui8RoundWinTeam = TEAM_BLUE_WIN;

		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		return TRUE;
	}

	if( IsTrainingRoom() )	return FALSE;

	if( g_TeamKillCountTable[ui8EndKillIdx] <= m_TeamKillCount[ TEAM_RED  ] ||
		g_TeamKillCountTable[ui8EndKillIdx] <= m_TeamKillCount[ TEAM_BLUE ] )
	{
		m_ui8RoundWinTeam		= TEAM_DRAW;

		if( m_TeamKillCount[ TEAM_RED ] > m_TeamKillCount[ TEAM_BLUE ] )	m_ui8RoundWinTeam = TEAM_RED_WIN;
		if( m_TeamKillCount[ TEAM_RED ] < m_TeamKillCount[ TEAM_BLUE ] )	m_ui8RoundWinTeam = TEAM_BLUE_WIN;

		m_ui8RoundEndType	= MISSION_END_MAX_KILL;
		return TRUE;
	}

	return FALSE;
}

BOOL CDediRoom::RoundEndChkDefence()
{
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;

	if( m_r32BattleTime > g_TeamTimeTable[ui8EndTimeIdx] + m_r32RoundStartTime )
	{
		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		m_ui8RoundWinTeam	= TEAM_BLUE_WIN;
		return TRUE;
	}

	// 오브젝트 객체를 얻어오지 못한 경우 타임아웃으로만 끝내버립니다.
	if( NULL == m_DefenceData.getDefenceObj(0) ||
		NULL == m_DefenceData.getDefenceObj(1) )
	{
		return FALSE;
	}

	if( 0 == m_DefenceData.getDefenceObj(0)->getHP() &&
		0 == m_DefenceData.getDefenceObj(1)->getHP() )
	{
		m_ui8RoundEndType	= MISSION_END_DEFENCE_DESTROY;
		m_ui8RoundWinTeam	= TEAM_RED_WIN;
		return TRUE;
	}

	return FALSE;
}

BOOL CDediRoom::RoundEndChkDestroy()
{
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;

	// 오브젝트 객체를 얻어오지 못한 경우 타임아웃으로만 끝내버립니다.
	if( NULL == m_DestructionData.m_pObjectRed ||
		NULL == m_DestructionData.m_pObjectBlue )
	{
		if( m_r32BattleTime > g_TeamTimeTable[ui8EndTimeIdx] + m_r32RoundStartTime )
		{
			m_ui8RoundEndType	= MISSION_END_TIMEOUT;
			m_ui8RoundWinTeam	= TEAM_DRAW;
			return TRUE;
		}

		return FALSE;
	}

	if( 0 == m_DestructionData.m_pObjectRed->getHP() )
	{
		m_ui8RoundEndType	= MISSION_END_GENERATOR_DESTROY;
		m_ui8RoundWinTeam	= TEAM_BLUE_WIN;
		return TRUE;
	}

	if( 0 == m_DestructionData.m_pObjectBlue->getHP() )
	{
		m_ui8RoundEndType	= MISSION_END_GENERATOR_DESTROY;
		m_ui8RoundWinTeam	= TEAM_RED_WIN;
		return TRUE;
	}

	if( m_r32BattleTime > g_TeamTimeTable[ui8EndTimeIdx] + m_r32RoundStartTime )
	{
		REAL32 r32Red	= (REAL32)( (REAL32)m_DestructionData.m_pObjectRed->getHP() / (REAL32)m_DestructionData.m_pObjectRed->GetMaxHP() );
		REAL32 r32Blue	= (REAL32)( (REAL32)m_DestructionData.m_pObjectBlue->getHP() / (REAL32)m_DestructionData.m_pObjectBlue->GetMaxHP() );

		m_ui8RoundEndType	= MISSION_END_TIMEOUT;

		if( r32Red > r32Blue )
		{
			m_ui8RoundWinTeam	= TEAM_RED_WIN;
		}
		else if( r32Blue > r32Red )
		{
			m_ui8RoundWinTeam	= TEAM_BLUE_WIN;
		}
		else
		{
			m_ui8RoundWinTeam	= TEAM_DRAW;
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CDediRoom::RoundEndChkEscape()
{
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;

	if( m_r32BattleTime > g_TeamTimeTable[ui8EndTimeIdx] + m_r32RoundStartTime )
	{
		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		m_ui8RoundWinTeam	= TEAM_DRAW;
		return TRUE;
	}

	return FALSE;

}

BOOL CDediRoom::RoundEndChkTraining()
{
	// AI 모드의 경우 KillCount로 끝나지 않는다.
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;

	if( m_r32BattleTime - m_r32RoundStartTime > g_TeamTimeTable[ui8EndTimeIdx] )
	{
		m_ui8RoundWinTeam = TEAM_DRAW;

		if( m_TeamKillCount[ TEAM_RED ] > m_TeamKillCount[ TEAM_BLUE ] )	m_ui8RoundWinTeam = TEAM_RED_WIN;
		if( m_TeamKillCount[ TEAM_RED ] < m_TeamKillCount[ TEAM_BLUE ] )	m_ui8RoundWinTeam = TEAM_BLUE_WIN;

		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		return TRUE;
	}

	return FALSE;
}

BOOL CDediRoom::RoundEndChkTutorial()
{
	// 튜토리얼 모드는 게임서버에서 판단하도록 합니다.
	return FALSE;
}

BOOL CDediRoom::RoundEndChkConvoy()
{
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;
	UINT32 ui32NowTime = static_cast<UINT32>( m_r32BattleTime - m_r32RoundStartTime );

	if( ui32NowTime > g_TeamTimeTable[ui8EndTimeIdx] )
	{	// 시간 초과시 블루팀(탈취물 방어팀) 승리
		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		m_ui8RoundWinTeam	= TEAM_BLUE_WIN;
		return TRUE;
	}

	// 라운드 시작 후 5초 동안은 끝나지 못하게 함. 유저 리스폰 시간을 기다림
	if( ui32NowTime < 5 ) return FALSE;

	if( NULL != m_pConvoyObject &&
		OBJECT_POS_STATE_GOAL == m_pConvoyObject->m_ui8PosState )
	{	// 탈취물이 목표점에 도달했다면 레드팀 승리.
		m_ui8RoundEndType	= MISSION_END_USURPATION;
		m_ui8RoundWinTeam	= TEAM_RED_WIN;
		return TRUE;
	}	

	switch (GetAllDeathTeam())
	{
	case TEAM_BOTH:
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_BLUE_WIN;
		return TRUE;

	case TEAM_RED:
		if (m_bCanSoloPlay)	break;
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_BLUE_WIN;
		return TRUE;

	case TEAM_BLUE:
		if (m_bCanSoloPlay)	break;
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_RED_WIN;
		return TRUE;
	}

	return FALSE;
}

BOOL CDediRoom::RoundEndChkRunAway()
{
	UINT8 ui8EndTimeIdx	= (m_ui8RoomSubType & 0xF0) >> 4;
	UINT32 ui32NowTime = static_cast<UINT32>( m_r32BattleTime - m_r32RoundStartTime );

	if( ui32NowTime > g_TeamTimeTable[ui8EndTimeIdx] )
	{	// 시간 초과시 블루팀(탈취물 방어팀) 승리
		m_ui8RoundEndType	= MISSION_END_TIMEOUT;
		m_ui8RoundWinTeam	= TEAM_BLUE_WIN;
		return TRUE;
	}

	// 라운드 시작 후 5초 동안은 끝나지 못하게 함. 유저 리스폰 시간을 기다림
	if( ui32NowTime < 5 ) return FALSE;

	CDSObject* pObject;
	for( INT32 i = 0 ; i < m_i32RunAwayObjectCount ; i++ )
	{
		pObject = m_pRunAwayObject[ i ];

		if( OBJECT_POS_STATE_GOAL == pObject->m_ui8PosState )
		{	// 탈취물이 목표점에 도달했다면 레드팀 승리.
			m_ui8RoundEndType	= MISSION_END_USURPATION;
			m_ui8RoundWinTeam	= TEAM_RED_WIN;
			return TRUE;
		}	
	}

	switch (GetAllDeathTeam())
	{
	case TEAM_BOTH:
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_BLUE_WIN;
		return TRUE;

	case TEAM_RED:
		if (m_bCanSoloPlay)	break;
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_BLUE_WIN;
		return TRUE;

	case TEAM_BLUE:
		if (m_bCanSoloPlay)	break;
		m_ui8RoundEndType = MISSION_END_ALLDEATH;
		m_ui8RoundWinTeam = TEAM_RED_WIN;
		return TRUE;
	}

	return FALSE;
}

INT32 CDediRoom::GetHitCount(GAMEINFO_HIT_BYTYPE eByHitType, UINT32 ui32SlotIdx)
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return 0;

	switch( eByHitType )
	{
	case GHBT_CHARA:
		return m_pCastPacket_Chara_HitCount_ByChara[ui32SlotIdx];
	case GHBT_EXPLOSION:
		return m_pCastPacket_Chara_HitCount_ByExplosion[ui32SlotIdx];
	case GHBT_OBJECT:
		return m_pCastPacket_Chara_HitCount_ByObject[ui32SlotIdx];
	}

	return 0;
}

N_PCINFO_HIT_DEFAULT* CDediRoom::GetHitDefault(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex)
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return NULL;

	if( i32HitIndex < 0 || MAX_FIREHIT_COUNT <= i32HitIndex )
		return NULL;

	switch( eHitByType )
	{
	case GHBT_CHARA:
		return &m_pCastPacket_Chara_Hit_ByChara[ui32SlotIdx][i32HitIndex]._tDefault;
	case GHBT_EXPLOSION:
		return &m_pCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i32HitIndex]._tDefault;
	case GHBT_OBJECT:
		return &m_pCastPacket_Chara_Hit_ByObject[ui32SlotIdx][i32HitIndex]._tDefault;
	}

	return NULL;
}

N_PCINFO_HIT_WEAPON* CDediRoom::GetHitWeapon(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex)
{
	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return NULL;

	switch( eHitByType )
	{
	case GHBT_CHARA:
		{
			if( i32HitIndex < 0 || MAX_FIREHIT_COUNT <= i32HitIndex )
				return NULL;

			return &m_pCastPacket_Chara_Hit_ByChara[ui32SlotIdx][i32HitIndex]._tWeapon;
		}
	case GHBT_EXPLOSION:
		{
			return &m_pCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i32HitIndex]._stWeapon;
		}
	}

	return NULL;
}

REAL32 CDediRoom::GetDistance(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex, N_POS* pPosition, N_PCINFO_HIT_DEFAULT* pHitDefault)
{	
	VEC3D v3dAttackCharPos;
	switch( eHitByType )
	{
	case GHBT_CHARA:
		{
			v3dAttackCharPos.x = m_pCastPacket_Chara_PosRot[ui32SlotIdx]._Pos.x;
			v3dAttackCharPos.y = m_pCastPacket_Chara_PosRot[ui32SlotIdx]._Pos.y;
			v3dAttackCharPos.z = m_pCastPacket_Chara_PosRot[ui32SlotIdx]._Pos.z;
		}
		break;

	case GHBT_EXPLOSION:
		{
			v3dAttackCharPos.x = m_pCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i32HitIndex]._stExplosionPos.x;
			v3dAttackCharPos.y = m_pCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i32HitIndex]._stExplosionPos.y;
			v3dAttackCharPos.z = m_pCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i32HitIndex]._stExplosionPos.z;
		}
		break;

	case GHBT_OBJECT:
		{
			v3dAttackCharPos.x = m_aGameObjects[ui32SlotIdx].m_vCenterPos.x;
			v3dAttackCharPos.y = m_aGameObjects[ui32SlotIdx].m_vCenterPos.y;
			v3dAttackCharPos.z = m_aGameObjects[ui32SlotIdx].m_vCenterPos.z;
		}
		break;
	}
	
	VEC3D v3dTargetCharPos;

	if (pHitDefault->GetHitType() == GHT_CHARA)
	{
		v3dTargetCharPos.x = pPosition->x;
		v3dTargetCharPos.y = pPosition->y;
		v3dTargetCharPos.z = pPosition->z;
	}
	else
	{
		//오브젝트를 공격했을경우 클라이언트 패킷에 담겨져있는
		//히트 위치 정보를 가져옵니다.
		if (eHitByType ==GHBT_EXPLOSION )
		{
			N_PCINFO_HIT_BYEXPLOSION* pHitByExplosion = m_pCastPacket_Chara_Hit_ByExplosion[ ui32SlotIdx ];
			if(pHitByExplosion)
				UDP_UTIL::ConvertNet3DtoVec3D(&v3dTargetCharPos,&pHitByExplosion->_stHitPos);
		}
		else if (eHitByType == GHBT_CHARA)
		{
			N_PCINFO_HIT_DEDICATED2* pHitByChar = m_pCastPacket_Chara_Hit_ByChara[ ui32SlotIdx ];
			if(pHitByChar)
			{
				v3dTargetCharPos.x = pHitByChar->_HitPos.x;
				v3dTargetCharPos.y = pHitByChar->_HitPos.y;
				v3dTargetCharPos.z = pHitByChar->_HitPos.z;

			}
		}
	}

	VEC3D v3dDirection;
	i3Vector::Sub(&v3dDirection, &v3dAttackCharPos, &v3dTargetCharPos);
	return i3Vector::Length(&v3dDirection);
}

CHARA_DEATH_TYPE CDediRoom::GetDeathType(GAMEINFO_HIT_BYTYPE eHitByType, UINT32 ui32SlotIdx, INT32 i32HitIndex)
{
	if( eHitByType == GHBT_CHARA )
	{
		if( i32HitIndex < 0 || MAX_FIREHIT_COUNT <= i32HitIndex )
			return CHARA_DEATH_UN;

		CHARA_HIT_PART eHitPart = NetIdxToHitPart( m_pCastPacket_Chara_Hit_ByChara[ui32SlotIdx][i32HitIndex]._tDefault.GetHitPart() );
		if( eHitPart == CHARA_HIT_HEAD_OLD || eHitPart == CHARA_HIT_HEAD || eHitPart == CHARA_HIT_JAW )
			return CHARA_DEATH_H;
		else
			return CHARA_DEATH_A;
	}
	else if( eHitByType == GHBT_EXPLOSION )
	{
		return static_cast<CHARA_DEATH_TYPE>( m_pCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i32HitIndex].GetDeathType() );
	}
	else if( eHitByType == GHBT_OBJECT )
	{
		return static_cast<CHARA_DEATH_TYPE>( m_pCastPacket_Chara_Hit_ByObject[ui32SlotIdx][i32HitIndex].GetDeathType() );
	}

	return CHARA_DEATH_UN;
}

VEC3D CDediRoom::GetHitDirection(GAMEINFO_HIT_BYTYPE eHitByType, INT32 ui32SlotIdx, INT32 i32HitIndex)
{
	VEC3D vec3dDirection;
	i3Vector::Set(&vec3dDirection, 0.0f, 0.0f, 0.0f);

	if( !IS_VALID_MEMBER_RANGE(ui32SlotIdx) )
		return vec3dDirection;

	if( i32HitIndex < 0 || MAX_FIREHIT_COUNT <= i32HitIndex )
		return vec3dDirection;

	if( eHitByType == GHBT_CHARA )
	{
		N_PCINFO_HIT_DEDICATED2* pHitByChar = &m_pCastPacket_Chara_Hit_ByChara[ui32SlotIdx][i32HitIndex];

		VEC3D vec3dFirePos, vec3dHitPos;
		i3Vector::Set(&vec3dFirePos, pHitByChar->_stFirePos.x, pHitByChar->_stFirePos.y, pHitByChar->_stFirePos.z);		
		i3Vector::Set(&vec3dHitPos, pHitByChar->_HitPos.x, pHitByChar->_HitPos.y, pHitByChar->_HitPos.z);
		i3Vector::Sub(&vec3dDirection, &vec3dHitPos, &vec3dFirePos);
		
	}
	else if( eHitByType == GHBT_EXPLOSION )
	{
		UDP_UTIL::ConvertNet3DtoVec3D(&vec3dDirection, &m_pCastPacket_Chara_Hit_ByExplosion[ui32SlotIdx][i32HitIndex]._Direction);
	}
	else if( eHitByType == GHBT_OBJECT )
	{
		UDP_UTIL::ConvertNet3DtoVec3D(&vec3dDirection, &m_pCastPacket_Chara_Hit_ByObject[ui32SlotIdx][i32HitIndex]._Direction);
	}

	i3Vector::Normalize(&vec3dDirection, &vec3dDirection);

	return vec3dDirection;
}

void CDediRoom::ProcessHelicopterExplosion(UINT32 ui32SlotIdx,UINT32 ui32TargetIdx,N_PCINFO_HIT_WEAPON* pHitWeapon)
{
	for( INT32 j = 0; j < SLOT_MAX_COUNT; ++j )
	{
		if( !GetUserAlive( j ) || _CheckEnableHelicopterExplosion(j, static_cast<UINT16>(ui32TargetIdx)) )
			continue;

		m_aGameObjects[ui32TargetIdx].ResetUse();
		DeathObjectExplosion(ui32SlotIdx, j, ui32TargetIdx, pHitWeapon);
	}
}

BOOL CDediRoom::CheckUseWeapon( UINT32 ui32SlotIdx, T_ItemID TItemID )
{
	if( m_AvailableWeapon.IsUse( TItemID ) )						return TRUE;
	if( m_pGameCharacters[ ui32SlotIdx ].IsEquipWeapon( TItemID ) )	return TRUE;

	return FALSE;
}

void CDediRoom::QA_Command_Damage_Object(UINT8 ui8SlotIdx, INT32 ui32Team, INT32 i32Damage)
{

	if( IsDestroyMissionMode() )
	{
		if( !m_DestructionData.m_pObjectRed || !m_DestructionData.m_pObjectBlue )
			return;
	
		switch(ui32Team)
		{
		case  TEAM_RED: 
			m_DestructionData.m_pObjectRed->SubHP((INT16)i32Damage);
			break;
		case  TEAM_BLUE:
			m_DestructionData.m_pObjectBlue->SubHP((INT16)i32Damage);
			break;
		default:
			return;
			break;

		}
		m_DestructionData.m_ObjectSendDamageInfo._nRedObjectHP	=  m_DestructionData.m_pObjectRed->getHP();;
		m_DestructionData.m_ObjectSendDamageInfo._nBlueObjectHP =  m_DestructionData.m_pObjectBlue->getHP();;

		m_DestructionData.m_ObjectSendDamageInfo._nUserDamage[ui8SlotIdx] += i32Damage;
		m_ar32AttackBeginTime[ui8SlotIdx] = GetGameTime();
		m_ui32TcpSendFlag |= DS_SEND_MISSION_DESTRUCTION;
	}
	else if (IsDefenceMissionMode())
	{
		if( !m_DefenceData.getDefenceObj(0) || !m_DefenceData.getDefenceObj(1) )
			return;
	
		switch(ui32Team)
		{
		case  0: 
			m_DefenceData.getDefenceObj(0)->SubHP((INT16)i32Damage);
			m_DefenceData.getSendDamageInfo()->setUserDamage(0, ui8SlotIdx, m_DefenceData.getSendDamageInfo()->getUserDamage(0, ui8SlotIdx) + (INT16)i32Damage);
			break;
		case  1:
			m_DefenceData.getDefenceObj(1)->SubHP((INT16)i32Damage);
			m_DefenceData.getSendDamageInfo()->setUserDamage(1, ui8SlotIdx, m_DefenceData.getSendDamageInfo()->getUserDamage(1, ui8SlotIdx) + (INT16)i32Damage);
			break;
		default:
			return;
			break;

		}
	
		m_DefenceData.getSendDamageInfo()->setObjectHP(0, m_DefenceData.getDefenceObj(0)->getHP());
		m_DefenceData.getSendDamageInfo()->setObjectHP(1, m_DefenceData.getDefenceObj(1)->getHP());

		m_ar32AttackBeginTime[ui8SlotIdx] = GetGameTime();
		m_ui32TcpSendFlag |= DS_SEND_MISSION_DEFENCE;

	}
	else if (IsBombMissionMode())
	{

		if ( STAGE_UID_OUTPOST == GetStageUID() )
		{
			N_PCINFO_HIT_WEAPON *pHitWeapon = NULL;
			
			//아웃포스트 헬기 번호 136 해당 오브젝트 전용이므로 하드코딩
			// pHitWeapon 의 경우 NULL 일 경우 새로운 무기를 생성해주므로 문제없음
			if( isHelicopter(m_aGameObjects[136].GetSType()) )
			{
				m_aGameObjects[136].SubHP( i32Damage );
				
				INT32 ui32HalfExplosionHp = static_cast<INT32>( m_aGameObjects[136].GetMaxHP() * 0.3f );

				//반파 상태에 대한 데미지 설정
				//헬리스팟의 경우 반파시 헬기 내 캐릭터 사망!  아웃포스트의 경우 반파시 데미지 없이 완파시 사망
				if( m_aGameObjects[136].getState() == DS_EXPLOSION_STATE_NORMAL && m_aGameObjects[ 136 ].getHP() < ui32HalfExplosionHp)
				{
					m_aGameObjects[136].setState( DS_EXPLOSION_STATE_HALF_EXPLOSION );

				}
				//완파 상태에 대한 데미지설정
				//완파 상태일 경우 헬기탑승유저 모두 사망
				if ( m_aGameObjects[136].getHP() == 0)
				{
					ProcessHelicopterExplosion(ui8SlotIdx,136,pHitWeapon);
				}

			}

			m_aGameObjects[1].SetEventUser( ui8SlotIdx );
		}
	}
}

void CDediRoom::QA_Command_KillCount(UINT8 ui8SlotIdx,INT32 i32KillCnt)
{
	if (IsDeathMode())
	{
		INT8 i8TeamCount = ui8SlotIdx%2;
		m_TeamKillCount[i8TeamCount] = i32KillCnt;
	}
}

void CDediRoom::QA_Command_Move_User(INT32 i32SlotIdx,VEC3D *pPos)
{
	UDP_UTIL::ConvertVec3DtoNet3D(&m_pCastPacket_Chara_CommandMove[i32SlotIdx].vPos,pPos);

		// check flag
	getChara_Check( i32SlotIdx )->nP2PInfoFlag |= P2P_INFO_FLAG_MOVE_POS;
	getChara_Check( i32SlotIdx )->bCheckBroadcast = true;

}

void CDediRoom::SetRoundNumber( UINT8 ui8RoundNumber )
{
	m_ui8RoundNumber = ui8RoundNumber;
	m_i32TeamChanged = 0;
	
	if ( GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE( m_ui8InfoFlag ) )
		{
			if ( m_ui8RoundNumber >= g_MissionRoundCountTable[ (m_ui8RoomSubType & 0x0F) ])
				m_i32TeamChanged = 1;
		}

	I3TRACE( "Set Round : %d\n", m_ui8RoundNumber );
}



// TRUE : Hacking
void   CDediRoom::_CheckGravityMove( UINT32 ui32SlotIdx, N_POS* pUserPos )
{

	UINT8 ui8CheckLevel = g_pConfig->GetCheckLevel(HACK_TYPE_GRAVITY_MOVE);
	
	// 로그 수집이 끝나고 릴리즈 할경우 코드 변경 필요
	//if( ui8CheckLevel <= getDetectRate(ui32SlotIdx)->IncreaseDetectRateCnt(HACK_TYPE_GRAVITY_MOVE)) return;
		
	STAGE_UID eStageUID = GetStageUID();
	if( eStageUID == STAGE_UID_TUTORIAL ) return;
	if (TRUE == IsSpaceMode())	return;
	NxVec3 NxPos = NxVec3 ( pUserPos->x, pUserPos->y, pUserPos->z );

	//리스폰 전 캐릭터의 좌표를 5200으로 보냄
	//정확한 결과 측정을 위해 추가

	if(NxPos.y < 4000.f)
	{
		if( !g_StageBounds[eStageUID].contain(NxPos))
		{
			HACK_LOG hl;

			//로그 수집용 코드 
			if (ui8CheckLevel > 0)
			{	

				hl.AddInfo(  "HACK_TYPE_GRAVITY_MOVE!\t Stage:%d\t X :%f\t Y:%f\t Z:%f\t UID:[%I64d]\n", eStageUID,NxPos.x,NxPos.y,NxPos.z,GetUID(ui32SlotIdx));
				char* msg = hl.MakeLogString( this, ui32SlotIdx, HACK_TYPE_GRAVITY_MOVE);
				if( msg) DSHACK(msg);

				EnqueueKickUser( ui32SlotIdx, HACK_TYPE_GRAVITY_MOVE);
			}
			else
			{
				if (!GetBit(m_ui32GravityFlag,ui32SlotIdx))
				{
					SetBit(&m_ui32GravityFlag,ui32SlotIdx);
					hl.AddInfo(  "HACK_TYPE_GRAVITY_MOVE!\t Stage:%d\t X :%f\t Y:%f\t Z:%f\t UID:[%I64d]\n", eStageUID,NxPos.x,NxPos.y,NxPos.z,GetUID(ui32SlotIdx));
					char* msg = hl.MakeLogString( this, ui32SlotIdx, HACK_TYPE_GRAVITY_MOVE);
					if( msg) DSHACK(msg);

				}
			}

		}
		else
		{
			if(GetBit(m_ui32GravityFlag,ui32SlotIdx))
				ClearBit(&m_ui32GravityFlag,ui32SlotIdx);
		}
	}

}


void CDediRoom::TouchDown_C(UINT32 ui32SlotIdx)
{
	if( FALSE == GetUserAlive( ui32SlotIdx ) )	return;

	CCharacter* pChara = GetGameCharacter( ui32SlotIdx );
	if( NULL == pChara )						return;

	switch( m_stageType )
	{ 
	case STAGE_MODE_ESCAPE:
		{
			m_cs.Lock();
			{
				++m_aui32TouchDownCount[ui32SlotIdx];
				// 게임 서버에 알림
				g_pModuleCast->Send_PROTOCOL_BATTLE_TouchDown( GetTaskIndex(), &GetRoomInfo(), ui32SlotIdx );

				// save old data
				UINT32	ui32WeaponList[ WEAPON_SLOT_COUNT ];
				ui32WeaponList[ WEAPON_SLOT_PRIMARY ]	= pChara->getRespawnWeapon( WEAPON_SLOT_PRIMARY );
				ui32WeaponList[ WEAPON_SLOT_SECONDARY ]= pChara->getRespawnWeapon( WEAPON_SLOT_SECONDARY );

				for( INT32 i = WEAPON_SLOT_MELEE; i < WEAPON_SLOT_COUNT; ++i )
				{
					ui32WeaponList[ i ] = pChara->GetEquipWeaponID( static_cast<UINT8>(i) );
				}

				pChara->ResetBattleInfo(); // HP 회복
				getChara_Life( ui32SlotIdx )->iLife = pChara->GetHP();

				// 터치 다운시 respawn_c 로 들어가지 않아 이곳에서 기존 리스폰 무기를 들게 해준다.
				for( INT32 i = WEAPON_SLOT_PRIMARY; i < WEAPON_SLOT_COUNT; ++i )
				{
					pChara->InitWeapon( ui32WeaponList[ i ], static_cast<UINT8>(i) );
				}

				m_ar32TouchDownBeginTime[ ui32SlotIdx ] = GetGameTime();
				m_ar32LastDeathTimes[ ui32SlotIdx ] = GetGameTime();

				// set max bullet
				pChara->InitBullets( m_aui8PBVer[ui32SlotIdx] );																					

				ResetFlagEscape( ui32SlotIdx );
				pChara->SetAlive( TRUE );
				//어시스트 정보 리셋
				m_AssistInfo[ui32SlotIdx].Reset();
			}
			m_cs.Unlock();
		}
		break;
	case STAGE_MODE_CONVOY :
		{
			if( OBJECT_POS_STATE_ON != m_pConvoyObject->m_ui8PosState )	return;

			if( pChara->HaveObject( m_pConvoyObject ) )
			{
				m_pConvoyObject->m_ui8PosState = OBJECT_POS_STATE_GOAL;

				g_pModuleCast->Send_PROTOCOL_BATTLE_TouchDown( GetTaskIndex(), &GetRoomInfo(), ui32SlotIdx );
			}
		}
		break;
	case STAGE_MODE_RUN_AWAY:
		{
			CDSObject* pObject;
			for( INT32 i = 0 ; i < m_i32RunAwayObjectCount ; i++ )
			{
				pObject = m_pRunAwayObject[ i ];

				if( OBJECT_POS_STATE_ON != pObject->m_ui8PosState )		continue;
				if( FALSE == pObject->IsUsed() )						continue;

				pObject->m_ui8PosState = OBJECT_POS_STATE_GOAL;
			}
		}
		break;
	default : 
		break;
	}
}

bool	CDediRoom::IsMaxDepth(COctreeNode *pNode)
{
	if (pNode == NULL)
		return false;

	//마지막 깊이가 아닐경우 false
	if(pNode->GetDepth() == MAX_SUBDIVIED)
		return true;

	return false;
}

bool	CDediRoom::SearchRaycastOnNode(VEC3D *pStart, VEC3D *pEnd, VEC3D *pDir,RaycastResult *pRay)
{

	CMapData* pMap = g_pMapManager->GetMap( m_ui32StageID );

	if (pMap ==NULL) return false;
	if (pMap->getOctTree() == NULL) return false;


	//레이시작점 노드 검색 
	COctreeNode *pNode = pMap->getOctTree()->GetNode(pStart);

	INT32 StartNodeIndex = pNode->GetNodeIndex();


	if (IsMaxDepth(pNode))
	{
		if (pNode->DoRaycast(pStart,pEnd,pRay,pMap->getOctTree()->GetWorld()))
			return true;
	}

	//레이 도착점 노드 검색 
	pNode = pMap->getOctTree()->GetNode(pEnd);

	INT32 EndNodeIndex = pNode->GetNodeIndex();

	INT32 TempNodeIndex =0;

	VEC3D ScalePos;


	if (StartNodeIndex != EndNodeIndex)
	{
		for (INT32 i=1; i< 5; i++)
		{
			i3Vector::Lerp(&ScalePos,pStart,pEnd,0.25f*i);

			pNode = pMap->getOctTree()->GetNode(&ScalePos);

			if (!IsMaxDepth(pNode))
				continue;	

			UINT16 NodeIndex = pNode->GetNodeIndex();

			// RayStart와 같은 노드일 경우 다음턴
			if (NodeIndex == StartNodeIndex)
				continue;

			// RayEnd와 같은 노드일 경우 처리 리턴
			if (NodeIndex == EndNodeIndex)
			{
				if (pNode->DoRaycast(pStart,pEnd,pRay,pMap->getOctTree()->GetWorld()))
					return true;

				return false;
			}

			// 중간 Node 계산
			if (TempNodeIndex != NodeIndex)
			{	
				TempNodeIndex = NodeIndex;
				if (pNode->DoRaycast(pStart,pEnd,pRay,pMap->getOctTree()->GetWorld()))
					return true;
				//		I3TRACE("중간Node 계산 진입 %d 단계 \n", i);
			}

		}

	}

	return false;


}

BOOL CDediRoom::DoRayCast( RaycastResult *pOut, N_POS* pPos) 
{

	VEC3D vStartPos,vEndPos,vDir;
	
	UDP_UTIL::ConvertNet3DtoVec3D(&vStartPos,pPos);
	i3Vector::Copy(&vEndPos,&vStartPos);
	i3Vector::AddY(&vEndPos,-20.f);
	i3Vector::Sub(&vDir,&vStartPos,&vEndPos);
	i3Vector::Normalize(&vDir,&vDir);
	i3Vector::AddY(&vStartPos,0.4f);

	if(SearchRaycastOnNode(&vStartPos,&vEndPos,&vDir,pOut))
	{
		I3TRACE("===============================\n");
		I3TRACE("X : %f,  Y: %f  z: %f  \n", pOut->m_vIntersect.x,pOut->m_vIntersect.y,pOut->m_vIntersect.z);
		I3TRACE("===============================\n");
	
		return TRUE;
	}

	return FALSE;

}
BOOL CDediRoom::isHackUserForLog(UINT32 ui32SlotIdx,DS_HACK_TYPE type)
{
	switch(type)
	{
	case HACK_TYPE_FIRE_SPEED:
		if(GetBit(m_ui32FireSpeedHackFlag,ui32SlotIdx))
			return TRUE;
		break;
	case HACK_TYPE_SPEED:
		if(GetBit(	m_ui32SpeedHackFlag,ui32SlotIdx))
			return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}

void CDediRoom::SetHackUserForLog(UINT32 ui32SlotIdx,DS_HACK_TYPE type)
{
	switch(type)
	{
	case HACK_TYPE_FIRE_SPEED:
		SetBit(&m_ui32FireSpeedHackFlag,ui32SlotIdx);
		break;
	case HACK_TYPE_SPEED:
		SetBit(&m_ui32SpeedHackFlag,ui32SlotIdx);
		break;
	default:
		break;
	}
}

void CDediRoom::ResetHackFlagForLog(UINT32 ui32SlotIdx)
{
	ClearBit(&m_ui32FireSpeedHackFlag,ui32SlotIdx);
	ClearBit(&m_ui32SpeedHackFlag,ui32SlotIdx);
}

BOOL CDediRoom::IsInvincibleTime( UINT32 ui32SlotIdx )
{
	if( m_r32BattleTime < m_ar32InvincibleTime[ ui32SlotIdx ] )		return TRUE;

	return FALSE;
}

INT32 CDediRoom::GetPlayingUserCount(void)
{
	INT32 i32PlayingUser = 0;
	for( INT32 i = 0; i < SLOT_MAX_COUNT ; ++i)
	{ 
		if (m_pDediMember[i].GetPlaying()) ++i32PlayingUser;
	}
	return i32PlayingUser;
}

void CDediRoom::SetMultiWeaponSlot()
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_pGameCharacters[i].SetMultiWeaponSlot(g_pConfig->m_ui8MultiWeaponCount);
	}
}
void CDediRoom::SetSingleWeaponSlot()
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		m_pGameCharacters[i].SetSingleWeaponSlot();
	}
}