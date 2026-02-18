#include "pch.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"

#include "StageBattle/UserContext.h"
#include "StageBattle/NetworkContext.h"
#include "StageBattle/DinoModeContext.h"

#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "ProfileDef.h"
#include "Network/GameNetworkManager.h"

#include "StageBattle.h"
#include "StageBattle/AIModeContext.h"
#include "UtilBattle.h"
#include "GameMissionManager.h"
#include "BattleSyncTimer.h"

#include "StageBattle/ScoreContext.h"

#include "GameMaster.h"

#include "WeaponLRU_Manager.h"


#ifndef NC_BUILD
#define DUAL_CLIENT
#endif


void PING::Init( void)
{
	m_fPingRecvTime	= 0.0f;
	m_rPingCheckTime = 0.0f;
	m_rPingSendTime	= 0.0f;

	Reset();
}

void PING::Reset( void)
{
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_Pings[ i ]	= 1;
		m_rPingList[i]	= 0.0f;
		m_PingTimes[i] = 0;
	}
}

void PING::RecvPing( INT32 iUserIdx, REAL32 rSendTime, REAL32 elapsedTime)
{
	//1. m_rPingSendTime 과 같은 핑이 왔는지 확인 아니라면
	REAL32 PingTimeC;
	REAL32 PingTimeS;
	
	if( m_rPingSendTime == rSendTime)
	{	// 경과 시간을 더하고 평균을 계산
		PingTimeC = elapsedTime - m_rPingSendTime;
		PingTimeS = elapsedTime - m_rPingSendTime;
	}
	else
	{	// 0.4초 delay를 더하고 평균을 구한다.
		PingTimeC	= 0.4f;
		PingTimeS	= 5.f;
	}

		m_rPingList[ iUserIdx ]	= (m_rPingList[ iUserIdx ] + PingTimeC ) * 0.5f;
		m_PingTimes[ iUserIdx ]	= (UINT16)( PingTimeS * 1000);
}


BattleServerContext::BattleServerContext() : m_ServerType(BATTLE_SERVER_TYPE_NONE), m_bP2PHost(false)
	, m_bP2PActivate(false), m_bMoveStage(false), m_fCheckRespawnaAck( 0.f), m_bCheckRespawnaReq(false)
	, m_bWaitLoadForTeamChange(false), m_TimeCheck(0), m_BaseTime(0)
{
	m_PingInfo.Init();
}

BattleServerContext::~BattleServerContext()
{

}

bool BattleServerContext::IsUseDedicationMode()const
{
	//AI전이면 P2P나 릴레이로 동작한다. 
	if (m_ServerType != BATTLE_SERVER_TYPE_DEDICATION) return false;
	return ( !MyRoomInfoContext::i()->getStage()->IsAiMode() );		
}

void BattleServerContext::InitP2PData(void)
{
	BattleServerContext::SetPingInfo()->Reset();

	NetworkContext::i()->InitP2P();
	
#if !defined( I3_NO_PROFILE )
	NETPROFILE::UDPReset(CGameNetworkManager::i()->GetProfileData()); 
#endif

	m_bMoveStage = false;
}



void BattleServerContext::CreateBattle( bool bLoaded /*=false*/ )
{
	// 패킷 정보등을 초기화
	InitP2PData();

	if( bLoaded )
		return;

#ifdef DUAL_CLIENT
	if( BattleSlotContext::i()->getMainSlotIdx() != BattleSlotContext::i()->getMySlotIdx() )
	{
		NetworkContext::i()->SetGamePort((NetworkContext::i()->GetGamePort()+(UINT16)BattleSlotContext::i()->getMySlotIdx()+1));
	}
	GlobalFunc::PB_TRACE("m_GamePort %d \n", NetworkContext::i()->GetGamePort());
#endif

	//Create UDP
	CGameNetworkManager::i()->CreateUDPSocket( NetworkContext::i()->GetGamePort());

	SetP2PActivate();

	BattleSyncTimer::i()->Start();

	// 랜덤 코드 생성
	ISTEST_S
	{
		g_pRand->Create( 0, 65536 );
	}
	ISTEST_E

#if defined( NETTEST_EVENT )
	I3TRACE( "[CGameContext::P2PLoadBattle]\n" );
#endif
}

void BattleServerContext::P2PIntrudeBattle( void)
{
	if( BattleSlotContext::i()->IsRoomMaster() )
	{
		BattleServerContext::i()->SetP2PHost();		// 방장 세팅
	}
	else
	{
		BattleServerContext::i()->SetP2PNonHost();
	}


	CGameNetworkManager::i()->IntrudeBattle( BattleServerContext::i()->IsP2PHost());

	// 랜덤 코드 생성
	g_pRand->Create( MyRoomInfoContext::i()->getMyRoomIndex(), 65536 );

#if defined( NETTEST_EVENT )
	I3TRACE( "[CGameContext::P2PIntrudeBattle()] - Host : %d\n", (INT32) IsP2PHost() );
#endif
}

// 게임이 끝나면 초기화 해줘야됩니다.
void BattleServerContext::P2PEndBattle( void )
{
	INT32 i;

	if( BattleServerContext::i()->IsUseBattleServer() )
	{
		CGameNetworkManager::i()->RelayHolePunchEndSend( NetworkContext::i()->relayServerIp, NetworkContext::i()->relayServerPort);

		if( !IsMovingStage() )
		{
			NetworkContext::i()->relayServerIp = 0;
			NetworkContext::i()->relayServerPort = 0;
		}
	}

	if( !IsMovingStage() )
	{
		SetServerType(BATTLE_SERVER_TYPE_NONE);
	}

	SetP2PDeactivate();

	I3PRINTLOG(I3LOG_NOTICE, "============ EndBattle =============");

	// P2P 패킷 타임아웃 카운트 로그 남기기
#if defined( I3_DEBUG)
	CGameNetworkManager::i()->RoundEndDump( BattleSyncTimer::i()->GetTimeElapsed());
#endif

	BattleSyncTimer::i()->Stop();

	I3PRINTLOG(I3LOG_NOTICE, "_InitUserData()" );
	for( i=0; i < USER_SLOT_COUNT; i++)
	{
		_InitUserData( i );
	}

	NetworkContext::i()->EndBattle();

	I3PRINTLOG(I3LOG_NOTICE, "g_pRand->Destroy()" );
	g_pRand->Destroy();


	I3PRINTLOG(I3LOG_NOTICE, "CGameMissionManager->EndMission()" );
	CGameMissionManager::i()->EndMission();

	//Socket Release
	CGameNetworkManager::i()->DestroyUDPSocket();

	I3PRINTLOG(I3LOG_NOTICE, "ProfileNetwork()" );
	ProfileNetwork();

#if defined( NETTEST_EVENT )
	I3TRACE( "[CGameContext::P2PGameEnd]\n" );
#endif

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
		GameEventSender::i()->SetEventOnce(EVENT_ENTER_LOBBY);

	I3PRINTLOG(I3LOG_NOTICE, "CGameContext::P2PEndBattle() Exited...." );
}

//
void BattleServerContext::P2PHostChange( INT32 nNewHostIdx )
{
	// 드랍된 무기를 모두 없애줍니다.
	switch( MyRoomInfoContext::i()->GetMyRoomStageID() )
	{
	case STAGE_MODE_DEATHMATCH	:
	case STAGE_MODE_DESTROY		:
	case STAGE_MODE_DEFENCE		:
									
		g_pWeaponManager->RemoveAllDropWeapon(); break;

	case STAGE_MODE_BOMB		 : break;		//Drop Weapon C4가 사라짐. AllDropWeapon을 하면 문제가 생김
	case STAGE_MODE_ANNIHILATION : break;
	case STAGE_MODE_ESCAPE		 : break;

	default: break;
	}

	BattleSlotContext::i()->setMainSlotIdx(nNewHostIdx);

	if( BattleSlotContext::i()->IsRoomMaster() )
	{
		// 자신이 방장이다..
		BattleServerContext::i()->SetP2PHost();

		// 방장은 물리 오브젝트를 켜야 합니다.
		CStageBattle * pStage = g_pFramework->GetStageBattle();
		I3ASSERT( pStage != nullptr );
		pStage->SetObjectPhysixEnable( true);
	}

	CGameNetworkManager::i()->ChangeHostProcess( BattleSlotContext::i()->getMainSlotIdx(), 
		BattleServerContext::i()->IsP2PHost(), BattleServerContext::i()->IsUseBattleServer() );

	I3PRINTLOG(I3LOG_NOTICE, "============ Change Host =============");

#if defined( NETTEST_EVENT )
	I3TRACE( "[GameContext::P2PHostLeave()] HostIdx : %d, MySlotIdx : %d\n", m_MainSlotIdx, m_MySlotIdx );
#endif
}

void BattleServerContext::P2PMyStartBattle( void)
{
	I3_BOUNDCHK( BattleSlotContext::i()->getMySlotIdx(), SLOT_MAX_COUNT);

	if( g_pCharaManager->getCharaResState( BattleSlotContext::i()->getMySlotIdx()) == CGameCharaManager::RESSTATE_LOADCMD )
	{
		BattleSlotContext::i()->setCharaType( BattleSlotContext::i()->getMySlotIdx(), CHARA::TYPE_PLAYER);

		// NET_CHARA_INFO에 설정
		NET_CHARA_INFO	* pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( BattleSlotContext::i()->getMySlotIdx());
		CCharaInfoContext::i()->GetNetCharaInfo( pNetCharaInfo );

		//미리 캐릭터를 예약해둔다..
		Avatar * pAvatar = g_pCharaManager->CreateAvatarPlayer( (AvatarLoadedCallback) sAfterLoadChara, BattleSlotContext::i()->getMySlotIdx(), pNetCharaInfo, CHARA::TYPE_PLAYER);
		// 팀이바꼈다, 팀바꿔서 재로딩
		if(ScoreContext::i()->IsAutoTeamChangeExecuted())
			pAvatar = g_pCharaManager->ChangeTeamAvatar((AvatarLoadedCallback)sAfterLoadChara, BattleSlotContext::i()->getMySlotIdx(), CHARA::TYPE_PLAYER);

		if( pAvatar != nullptr)
		{
			// 무기들을 BG 로딩한다.
			for( INT32 i = 0; i < WEAPON_CHARA_SLOT_MAX; i++)
			{
				T_ItemID itemID = pNetCharaInfo->getWeaponItemID( (WEAPON_SLOT_TYPE) i);
				g_pWeaponManager->PushLoadWeapon( itemID, true);
				pAvatar->PreloadAnimationByWeapon( itemID);
			}
		}


#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if( !g_pConfig->m_bNetworking )	// Local Mode
		{
			g_pCharaManager->CreateAvatarPlayer( (AvatarLoadedCallback) sAfterLoadChara, 1, pNetCharaInfo, CHARA::TYPE_PLAYER_NET);
			BattleSlotContext::i()->setSlotState( 1, SLOT_STATE_BATTLE);
		}
#endif
	}
}

void BattleServerContext::ChangeTeam(void)
{
	INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();
	I3_BOUNDCHK( mySlotIdx, SLOT_MAX_COUNT);

	if( g_pCharaManager->getCharaTeamChange( mySlotIdx) == false)
	{
		g_pCharaManager->ChangeTeamAvatar( (AvatarLoadedCallback) sAfterLoadChara, mySlotIdx, CHARA::TYPE_PLAYER);
		m_bWaitLoadForTeamChange = true;
	}

	for(INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++)
	{
		if( (i != mySlotIdx) && (g_pCharaManager->getCharaTeamChange(i) == false) )
		{
			g_pCharaManager->ChangeTeamAvatar( (AvatarLoadedCallback) sAfterLoadChara, i, CHARA::TYPE_PLAYER_NET);
		}
	}

	// 팀교체시 다시한번 설치 팀인지 아닌지는 세팅
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if( (myPlayer != nullptr) && (myPlayer->getCharaTeam() & CHARACTER_TEAM_RED))
		CGameMissionManager::i()->setMyMissionObject( MY_MISSION_OBJECT_OFFENSIVE );
	else
		CGameMissionManager::i()->setMyMissionObject( MY_MISSION_OBJECT_DEFENSIVE );

	LoadingAnotherCharacter();
}

bool BattleServerContext::WaitLoadingMyAvatar( void)
{
	Avatar * pPlayer = (Avatar*) g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( pPlayer != nullptr);

	if( pPlayer->GetLoadedParts() == false)
	{
		return false;
	}

	return true;
}

void BattleServerContext::Battle_EntranceAnotherUser( bool bVisible, bool bForceCreateAvatar)
{
	INT32 i;

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( (i != BattleSlotContext::i()->GetMySlotIdxEx() && (BattleSlotContext::i()->getSlotInfo( i)->_State >= SLOT_STATE_LOAD)) )
		{
			// 로딩 요청이 있으면 로드
			if(	g_pCharaManager->getCharaResState( i) == CGameCharaManager::RESSTATE_LOADCMD )
			{
				BattleSlotContext::i()->setCharaType( i, CHARA::TYPE_PLAYER_NET);

				Avatar * pAvatar = g_pCharaManager->CreateAvatarPlayer( (AvatarLoadedCallback)sAfterLoadChara, i,
					BattleSlotContext::i()->setNetCharaInfo( i),
					CHARA::TYPE_PLAYER_NET, true, bForceCreateAvatar);

				// 공수교대후 난입한유저인데 팀이바꼈다, 팀바꿔서 재로딩
				if(ScoreContext::i()->IsAutoTeamChangeExecuted())
					pAvatar = g_pCharaManager->ChangeTeamAvatar((AvatarLoadedCallback)sAfterLoadChara, i, CHARA::TYPE_PLAYER_NET);

				if(pAvatar != nullptr)
				{
					// 플레이중이면 Visible On
					if( UserContext::i()->IsPlayingSlot( i) && bVisible)
					{
						pAvatar->Cmd_Visible( true);
						pAvatar->Cmd_Idle();
					}
					else
					{// 생성중에는 Visible을 끈다.
						pAvatar->Cmd_Visible( false);
					}

					// 무기들을 BG 로딩한다.
					for( INT32 j = 0; j < WEAPON_CHARA_SLOT_MAX; j++)
					{
						T_ItemID itemID = BattleSlotContext::i()->setNetCharaInfo( i)->getWeaponItemID( (WEAPON_SLOT_TYPE) j);
						g_pWeaponManager->PushLoadWeapon( itemID, true);
						pAvatar->PreloadAnimationByWeapon( itemID);
					}
				}
			}
		}
	}

	// Timer를 위한 초기 세팅
	m_TimeCheck = 0;
	m_TimeCheck = static_cast<UINT32>(timeGetTime());

	// 스테이지 로딩 타임에는 AI를 등록하지 않는다.
	if( MyRoomInfoContext::i()->getStage()->IsAiMode()
		&& (BattleSlotContext::i()->GetMySlotIdxEx() != BattleSlotContext::i()->getMainSlotIdx()) )
	{
		// Client이지만 AI캐릭터를 등록해야한다.
		INT32 team = (BattleSlotContext::i()->GetMySlotIdxEx() % 2);

		for( i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if( (i % 2) != team && (i / 2) < AIModeContext::i()->EnemyCount)
			{// AI전 AI Character이지만 Client라면
				if( g_pCharaManager->getCharaResState( i) == CGameCharaManager::RESSTATE_NONE )
				{
					// 리소스 매니져에서 Bind할 Ai를 로드합니다.
					g_pCharaManager->LoadAiBindData( AIModeContext::i()->StartLevel, i / 2, static_cast<TEAM_TYPE>(i % TEAM_COUNT) );
					AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData( i / 2);

					// AI는 Parts 정보를 설정하지 않은 상태이다..
					NET_CHARA_INFO * pCharaInfo = BattleSlotContext::i()->setNetCharaInfo( i );
					for( INT32 j = 0 ; j < WEAPON_SLOT_COUNT ; j++ )
					{
						pCharaInfo->setEndurance( j, 100);
					}

					CHARA_PARTS_INFO * pPartsInfo = const_cast<CHARA_PARTS_INFO*>(pCharaInfo->getCharaParts());
					// vv3 - check
					pPartsInfo->SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA, pBindData->_Parts.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA) );

					// 무기 설정
					util_battle::CheckAIData( pBindData);


					pCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, pBindData->_primaryWeaponName, -1, __FILE__, __LINE__);
					pCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, pBindData->_secondaryWeaponName, -1, __FILE__, __LINE__);
					pCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, pBindData->_meleeWeaponName, -1, __FILE__, __LINE__);
					pCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, pBindData->_throwWeaponName, -1, __FILE__, __LINE__);
					pCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, pBindData->_itemWeaponName, -1, __FILE__, __LINE__);

					g_pCharaManager->setCharaResState( i, CGameCharaManager::RESSTATE_LOADCMD);
					I3TRACE( "EntranceUser Req AI Chara Load %d\n", i);

					BattleSlotContext::i()->setCharaType( i, CHARA::TYPE_PLAYER_NET);
					Avatar * pAvatar = g_pCharaManager->CreateAvatarPlayer( (AvatarLoadedCallback)sAfterLoadChara, i,
						BattleSlotContext::i()->setNetCharaInfo( i),
						CHARA::TYPE_PLAYER_NET, true, bForceCreateAvatar);

					if( pAvatar != nullptr)
					{
						// 무기들을 BG 로딩한다.
						for( INT32 j = 0; j < WEAPON_CHARA_SLOT_MAX; j++)
						{
							T_ItemID itemID = BattleSlotContext::i()->setNetCharaInfo( i)->getWeaponItemID( (WEAPON_SLOT_TYPE) j);
							g_pWeaponManager->PushLoadWeapon( itemID, true);
							pAvatar->PreloadAnimationByWeapon( itemID);
						}
					}

					GameEventReceiver::i()->PushReceivedGameEvent( EVENT_RESPAWN_BATTLE_FOR_AI, i);
				}
			}
		}
	}

}

void BattleServerContext::P2PPreStartBattleUser( INT32 idx )
{
	
	CGameNetworkManager::i()->PreStartBattleProcess( idx, BattleServerContext::i()->IsP2PHost(), BattleServerContext::i()->IsUseBattleServer() );

	if( BattleServerContext::i()->IsUseBattleServer() )
	{
		if( BattleServerContext::i()->IsP2PHost() == true )
		{//방장일때
			if( idx == BattleSlotContext::i()->GetMySlotIdxEx() )
			{
				if( BattleSlotContext::i()->getSlotInfo( idx )->_State != SLOT_STATE_BATTLE )
				{
					NetworkContext::i()->relayServerState = UDP_STATE_SENDRECV;
				}
			}
		}
		else if( idx == BattleSlotContext::i()->GetMySlotIdxEx() )
		{//방장이 아닐때 자신인지 아닌지.
			//자신의 것이면.. 방장과 홀펀칭을 시작합니다.
			NetworkContext::i()->relayServerState = UDP_STATE_SENDRECV;
		}
	}
	else
	{
		if( idx == BattleSlotContext::i()->GetMySlotIdxEx() )
		{
			if( BattleSlotContext::i()->getSlotInfo( idx )->_State != SLOT_STATE_BATTLE )
			{
				// 방장 자신은 바로 게임을 시작합니다.
				GameEventSender::i()->SetEvent( EVENT_START_BATTLE );
			}
			I3PRINTLOG(I3LOG_NOTICE, "============ PreStartBattle Ack=============");
		}
	}

#if defined( NETTEST_EVENT )
	I3TRACE( "[GameContext::P2PPreStartBattleUser] %d\n", idx );
#endif
}

bool BattleServerContext::P2PStartBattleUser( INT32 idx, bool bStart )
{
	CGameNetworkManager::i()->StartBattleProcess( idx, BattleSlotContext::i()->GetMySlotIdxEx(), BattleServerContext::i()->IsP2PHost());

	AIModeContext::i()->UserScore[idx] = 0;

	if( idx == BattleSlotContext::i()->GetMySlotIdxEx() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "CGameContext::P2PStartBattleUser() Enter idx %d m_MySlotIdx %d", idx, BattleSlotContext::i()->GetMySlotIdxEx());

		CGameCameraManager::i()->ResetCameraMgr();

		if(
#if defined( DEF_OBSERVER_MODE)
			// Observer이면 난입 처리
			BattleSlotContext::i()->isObserverMe() ||
#endif
			( UserContext::i()->IsPlayingSlot(BattleSlotContext::i()->GetMySlotIdxEx()) == false) )
		{	
			// 자신이 난입입니다.
			//m_MyIntrudeState = MY_INTRUDE_STATE_INTRUDE;
			IntrudeContext::i()->Start();
			I3PRINTLOG(I3LOG_NOTICE, "내가 난입했어요 Setting Start");

			// SetVisible( false)로 변경 박명진 - 2008. 7. 17
			CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( idx);
			if( pPlayer != nullptr)
			{
				// 자신이 난입일 경우 Skin에 달려있는 눈의 이펙트도 함께 꺼준다.
				// SetEyeEnable은 Skin에 눈의 이펙트가 있을 경우에만 끕니다. Jinsik.kim(2014.12.15)
				pPlayer->SetEyeEnable(false);
				pPlayer->SetVisible( false);

				pPlayer->addCharaStateMask( CHARA_STATE_DEATH );
				pPlayer->Cmd_ApplyCrosshair();
			}

			if ( IntrudeContext::i()->GetInterEnter() == MY_INTRUDE_STATE_PAUSED )
			{
				CGameMaster::i()->SetPause(true);
			}

			I3TRACE( "StartBattle : 난입 처리입니다." );
		}

		// 상대방 캐릭터 유무에 따라 Death 처리
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( i);
			if( pPlayer != nullptr && UserContext::i()->IsPlayingSlot(i))
			{
				if (BattleSlotContext::i()->GetMySlotIdxEx() == i) continue;
				pPlayer->removeCharaStateMask( CHARA_STATE_DEATH);
			}
		}

		// 자기 자신이 공룡이 걸렸다면, 먼저 만들어 놓은 인간 캐릭을 삭제하고 공룡으로 바꾼다.
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ||
			MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) )
		{
			CHARA_RES_ID rType = DinoModeContext::i()->GetDinoResType( idx);
			if( rType != CHARA_RES_ID_UNKNOWN )
			{
				g_pCharaManager->RemoveCharaAddDinoChara(idx, rType);
			}
		}

#if defined( DEF_OBSERVER_MODE)
		// Observer는 Respawn하지 않는다.
		if( BattleSlotContext::i()->isObserverMe())
			bFlag = false;
#endif

		I3PRINTLOG(I3LOG_NOTICE, "CGameContext::P2PStartBattleUser() Leave");
	}
	else
	{	// 다른 유저가 스타트
		// visible을 키자
		I3TRACE( "[CGameContext::P2PIntrudeBattle] Enter - %s\n", BattleSlotContext::i()->getNickForSlot( idx));

		if( UserContext::i()->IsPlayingSlot(idx) && BattleSlotContext::i()->IsGMObserverForClient(idx) == false )
		{
			BattleSlotContext::i()->setCharaType( idx, CHARA::TYPE_PLAYER_NET);

			// 공룡이 걸렸다면,  공룡으로 바꾼다.
			bool bDino = false;
			if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ||
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
			{
				CHARA_RES_ID rType = DinoModeContext::i()->GetDinoResType(idx);
				if( rType != CHARA_RES_ID_UNKNOWN )
				{
					g_pCharaManager->RemoveCharaAddDinoChara(idx, rType);
					bDino = true;
				}
			}

			if( bDino == false)
			{
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);
				if( pChara != nullptr && pChara->IsDino() )
				{
					g_pCharaManager->RemoveChara( idx);	// 이전 라운드에 공룡을 삭제
				}

				if( pChara == nullptr)	// 생성이 안된 경우만
				{
					g_pCharaManager->setCharaResState( idx, CGameCharaManager::RESSTATE_LOADCMD);
					pChara = g_pCharaManager->CreateAvatarPlayer( (AvatarLoadedCallback)sAfterLoadChara, idx,
						BattleSlotContext::i()->setNetCharaInfo( idx), CHARA::TYPE_PLAYER_NET);
					I3ASSERT( pChara != nullptr);

					// 팀이바꼈다, 팀바꿔서 재로딩
					if(ScoreContext::i()->IsAutoTeamChangeExecuted())
					{						
						// pChara = 
						g_pCharaManager->ChangeTeamAvatar((AvatarLoadedCallback)sAfterLoadChara, idx, CHARA::TYPE_PLAYER_NET);
						// 앞부분에 생성된 pChara는 ChangeTeamAvatar호출이후 로딩상태에있을수있습니다.
						// 기본파츠가아닌경우 쓰레드 로딩을 하기떄문, 이경우 리턴되는 pChara는 nullptr이 될수있습니다.(로딩줄)
					}	
					
				}
			}

			g_pCharaManager->RespawnChara( idx, true, true, !IntrudeContext::i()->IsIntrudeStart());
			P2PRespawnChara( idx);
		}
		else
		{
			//다른 유저 난입 상태 처리 부분.
			//2014-08-28 다른 유저가 난입했을 대 관전 모드 상태를 등록해줍니다. ENG_김대영
			CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( idx);
			if( pPlayer != nullptr)
			{
				pPlayer->addCharaStateMask( CHARA_STATE_OBSERVER );
			}
			
			if( !UserContext::i()->IsBattleSlot(idx) )
			{
				CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(idx);
				pChara->Cmd_Visible(false);
			}
			
		}
	}

	return true;
}

void BattleServerContext::sAfterLoadChara( void * pAvatar, INT32 idx /*= -1*/, bool bChangeParts /*= false*/, AI_BIND_DATA * pAIBind)
{
	CGameCharaBase * pChara = nullptr;

	if( (g_pFramework == nullptr) ||
		(g_pFramework->IsBattleStage() == false) )
		return;

	if( BattleServerContext::i()->IsP2PActivate() == false)
		return;

	const NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->getNetCharaInfo( idx, CHARA::Type2BufferType( BattleSlotContext::i()->getCharaType( idx)));

	if( idx == BattleSlotContext::i()->getMySlotIdx() )
	{
		if( g_pCharaManager->getCharaByNetIdx( idx) == nullptr )
		{
			// vv3 - check
			CHARA_RES_ID CharaResID = CHARA::ItemID2ResID( pNetCharaInfo->getCharaParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));

 			pChara  = g_pCharaManager->AddChara( idx, CharaResID );
			I3ASSERT(pChara != nullptr);
		}

		UserContext::i()->Death[idx] = false;
		if( BattleServerContext::i()->IsP2PHost() )
		{
			CGameNetworkManager::i()->StartBattleUserProcess( idx);
		}
	}
	else
	{
		// vv3 - check
		CHARA_RES_ID CharaResID = CHARA::ItemID2ResID( pNetCharaInfo->getCharaParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));

		pChara = g_pCharaManager->AddChara( idx, CharaResID, false, BattleSlotContext::i()->getCharaType( idx) );
		I3ASSERT(pChara != nullptr);
	}

	g_pCharaManager->setCharaResState(idx, CGameCharaManager::RESSTATE_LOADED);

	// AI Character는 Visible On
	if( pChara != nullptr && pChara->isAIToClient())
	{
		GameEventReceiver::i()->PushReceivedGameEvent( EVENT_RESPAWN_BATTLE_FOR_AI, pChara->getCharaNetIndex());
	}
}


void BattleServerContext::P2PGiveupBattleUser( INT32 idx )
{
#if defined( NETTEST_EVENT )
	I3TRACE( "[CGameContext::P2PGiveupBattleUser] %d\n", idx );
#endif

	if( idx != BattleSlotContext::i()->GetMySlotIdxEx() )
	{
		g_pCharaManager->RemoveChara( idx );

		CGameNetworkManager::i()->UserLeaveBattleProcess( idx, BattleServerContext::i()->IsUseBattleServer() );

		_InitUserData( idx );
	}
}

void BattleServerContext::P2PRespawnChara( INT32 iSlotIdx)
{
	I3_BOUNDCHK( iSlotIdx, SLOT_MAX_COUNT);
	CGameNetworkManager::i()->InitUser( iSlotIdx);

	// 리스폰 시에 무기 사용률 적중시킵니다.
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( iSlotIdx);
	if( pChara != nullptr)
	{
		WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
		if( pWeapon != nullptr)
			WeaponLRU_Manager::i()->addCount(pWeapon->getWeaponInfo()->GetItemID());
	}
}


bool	BattleServerContext::IsMovingStage(void) const			
{ 
	return m_bMoveStage;		
}


void BattleServerContext::_InitUserData( INT32 idx )
{
#if defined( DEF_OBSERVER_MODE)
	if( idx >= SLOT_MAX_COUNT)
	{	// Observer data를 초기화합니다.
		_InitUserDataForObserver( idx);
		return;
	}
#endif

	I3_BOUNDCHK( idx, SLOT_MAX_COUNT);

	BattleSlotContext::i()->setGameInfo_Chara( idx )->Reset();

	// 유저에 대한 수류탄 정보 리셋
	for( INT32 i = 0; i < MAX_THROWWEAPON_SLOP; i++)
	{
		BattleObjContext::i()->setGameInfo_ThrowWeapon( idx, i)->Reset();
	}

	// 유저에 대한 C5 정보 리셋
	/*{
		GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon( idx);
		pInfo->Reset();
	}*/

	
	CGameNetworkManager::i()->InitUser( idx);
}


#if defined( DEF_OBSERVER_MODE)
void BattleServerContext::_InitUserDataForObserver( INT32 idx)
{
	I3_BOUNDCHK( idx, USER_SLOT_COUNT);

	NetworkContext::i()->UdpHoleState[idx] = UDP_STATE_NONE;
	NetworkContext::i()->UdpHoleType[idx] = UDP_TYPE_NONE;

	// UDP 정보 리셋
	NetworkContext::i()->NetInfo[idx].Reset( idx);
}
#endif

//-------------------------------------------------------------------------------------------------------
void BattleServerContext::ProfileNetwork( void )
{
	/*
	#if !defined( I3_NO_PROFILE )
	m_ProfileData.m_nSendCountPerSec		= (UINT32)((REAL32)m_ProfileData.m_nTotalSendCount / m_ProfileData.m_fProfileTime);
	m_ProfileData.m_nRecvCountPerSec		= (UINT32)((REAL32)m_ProfileData.m_nTotalRecvCount / m_ProfileData.m_fProfileTime);

	m_ProfileData.m_nSendBytesPerSec		= (UINT32)((REAL32)m_ProfileData.m_nTotalSendBytes / m_ProfileData.m_fProfileTime);
	m_ProfileData.m_nRecvBytesPerSec		= (UINT32)((REAL32)m_ProfileData.m_nTotalRecvBytes / m_ProfileData.m_fProfileTime);

	if( m_ProfileData.m_nTotalSendCount )
	m_ProfileData.m_nSendBytesPerOne		= m_ProfileData.m_nTotalSendBytes / m_ProfileData.m_nTotalSendCount;
	if( m_ProfileData.m_nTotalRecvCount )
	m_ProfileData.m_nRecvBytesPerOne		= m_ProfileData.m_nTotalRecvBytes / m_ProfileData.m_nTotalRecvCount;

	I3PRINTLOG(I3LOG_NOTICE, "-----------------------Network Profile-- %d ---------------------", (INT32) m_ProfileData.m_fProfileTime);
	I3PRINTLOG(I3LOG_NOTICE, "------- Send -------" );
	I3PRINTLOG(I3LOG_NOTICE, "- Total Send Count       : %d", m_ProfileData.m_nTotalSendCount );
	I3PRINTLOG(I3LOG_NOTICE, "-- Send Count per Sec.   : %d", m_ProfileData.m_nSendCountPerSec );
	I3PRINTLOG(I3LOG_NOTICE, "- Total Send Bytes       : %d", m_ProfileData.m_nTotalSendBytes );
	I3PRINTLOG(I3LOG_NOTICE, "-- Send Size per Sec.    : %d", m_ProfileData.m_nSendBytesPerSec );
	I3PRINTLOG(I3LOG_NOTICE, "-- Send Size per Packet. : %d", m_ProfileData.m_nSendBytesPerOne );
	I3PRINTLOG(I3LOG_NOTICE, "-- Send Max, Min         : ( %d, %d )", m_ProfileData.m_nSendBytesMax, m_ProfileData.m_nSendBytesMin );
	I3PRINTLOG(I3LOG_NOTICE, "------- Recv -------");
	I3PRINTLOG(I3LOG_NOTICE, "- Total Recv Count       : %d", m_ProfileData.m_nTotalRecvCount );
	I3PRINTLOG(I3LOG_NOTICE, "-- Recv Count per Sec.   : %d", m_ProfileData.m_nRecvCountPerSec );
	I3PRINTLOG(I3LOG_NOTICE, "- Total Recv Bytes       : %d", m_ProfileData.m_nTotalRecvBytes );
	I3PRINTLOG(I3LOG_NOTICE, "-- Recv Size per Sec.    : %d", m_ProfileData.m_nRecvBytesPerSec );
	I3PRINTLOG(I3LOG_NOTICE, "-- Recv Size per Packet. : %d", m_ProfileData.m_nRecvBytesPerOne );
	I3PRINTLOG(I3LOG_NOTICE, "-- Recv Max, Min         : ( %d, %d )", m_ProfileData.m_nRecvBytesMax, m_ProfileData.m_nRecvBytesMin );
	I3PRINTLOG(I3LOG_NOTICE, "----------------------------------------------------------------" );
	I3PRINTLOG(I3LOG_NOTICE, "----- Packet Errors.-----" );
	I3PRINTLOG(I3LOG_NOTICE, "- InvalidIdx( %d ), WrongData( %d ), InvalidAddr( %d ), LostTime( %d )", m_ProfileData.m_nInvalidIndex, m_ProfileData.m_nWrongPacket, m_ProfileData.m_nInvalidAddr, m_ProfileData.m_nLostTime );
	I3PRINTLOG(I3LOG_NOTICE, "----------------------------------------------------------------" );
	#endif
	*/
}



void BattleServerContext::InitRoundData( void )
{
	INT32	i;

	//미션 리셋
	CGameMissionManager::i()->Reset();

	//모든 오브젝트 HP 세팅(네트워크 정보 리셋)
	for( i = 0; i < OBJ_TOTAL_COUNT; i++ )
	{
		GAMEINFO_OBJECT * pObjInfo = BattleObjContext::i()->setGameInfo_Object(i);
		if( pObjInfo->_pObjectRef != nullptr )
		{
			I3_BOUNDCHK( pObjInfo->_pObjectRef->getMaxHP(), 65536);
			pObjInfo->_tNetData.setHP( (UINT16) pObjInfo->_pObjectRef->getMaxHP());
		}
	}

	//드롭된 무기 정보 리셋			(네트워크 정보 리셋)
	for( i = 0; i < MAX_DROPEDWEAPON_COUNT; i++ ) 
		BattleObjContext::i()->setGameInfo_DropedWeapon( i )->Reset();

	BattleObjContext::i()->SetDropedWeaponCount(0);

	BattleObjContext::i()->ResetGameInfo_ThrowWeapon();

#ifdef DOMI_STING_POISON_GRENADE
	BattleObjContext::i()->setDomiDinoGrenades()->DeleteAll();
#endif

	// 2009.06.11 초기화 버그 수정 dfly79
	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		BattleSlotContext::i()->setGameInfo_Chara(i, CHARA::BUFFER_USER)->Reset();
	}

	for( i = 0; i < MAX_COUNT_DOMI_DINO; i++ )
	{
		BattleSlotContext::i()->setGameInfo_Chara(i, CHARA::BUFFER_NEW_AI)->Reset();
	}


	CGameNetworkManager::i()->InitRound();

	// 버그 수정. 다음 라운드에서 미션용 패킷이 보내집니다.
	// PreStart와 RoundStart의 갭으로 인한 문제..	2009.06.16	dfly79
	if( BattleSlotContext::i()->GetMySlotIdxEx() != -1)
		BattleSlotContext::i()->setGameInfo_Chara(BattleSlotContext::i()->GetMySlotIdxEx(), CHARA::BUFFER_USER)->_tNetData._tMissionData.Reset();
}


void BattleServerContext::InitMulticastInfo( INT32 idx )
{
	if( BattleServerContext::IsP2PHost() )
	{
		CGameNetworkManager::i()->Respawn( idx);
	}
}

// revision 31898
void BattleServerContext::OnUpdate( REAL32 rDeltaTime)
{
	if (IsRespawnReq())
	{
		//10초안에 PROTOCOL_BATTLE_RESPAWN_ACK를 받지 못할경우 레디룸으로 되돌려 보냅니다.
		m_fCheckRespawnaAck -= rDeltaTime;
		
		//
		if (m_fCheckRespawnaAck < -10.f)
		{
			INT32 nArg = BattleSlotContext::i()->getMySlotIdx();
			I3TRACE("PROTOCOL_BATTLE_RESPAWN_ACK 받지못함\n");					
			GameEventSender::i()->SetEvent( EVENT_GIVEUP_BATTLE, &nArg );
			setRespawnReq(false);
			setRespawnAckTime(0.f);
		}
	}

	if( m_bWaitLoadForTeamChange)
	{
		if( WaitLoadingMyAvatar() )
			m_bWaitLoadForTeamChange = false;
	}
}

bool BattleServerContext::LoadingAnotherCharacter(void)
{
	// 상대방의 캐릭터를 로딩하는 모드
	//if( bPreloadAnotherUser )
	if( _AnotherCharaTimer() )
	{
		bool bUseAvatar = true;

		if( g_pEnvSet->m_bDisplayOtherParts == false && 
			MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEATHMATCH) )
		{
			bUseAvatar = false;	// 상대편 파츠 보기가 Off
		}

		if( g_pEnvSet->isFixedViewCharacter() == true )
		{	// 상대방 캐릭터 고정
			bUseAvatar = false;	// 파츠 보기는 OFF
		}

		if( bUseAvatar)
		{
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if( i == BattleSlotContext::i()->getMySlotIdx()) continue;

				Avatar * pPlayer = (Avatar*) g_pCharaManager->getCharaByNetIdx( i);
				if( pPlayer != nullptr)
				{
					if( pPlayer->GetLoadedParts() == false)
					{
						return false;
					}
				}
			}

			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if( i == BattleSlotContext::i()->getMySlotIdx()) continue;
				Avatar * pPlayer = (Avatar*) g_pCharaManager->getCharaByNetIdx( i);
				if( pPlayer != nullptr)
				{
					pPlayer->ResetChara();
				}
			}
		}
	}

	return true;
}

bool BattleServerContext::_AnotherCharaTimer()
{
	// 여기에 타이머를 두어서 10초까지 완전 로드가 필요함.
	// Domination 모드와 AI 모드는 1분으로 세팅하면 될 듯 싶음.
	// 미리 들어와 있는 유저 캐릭터를 완전히 로딩해야 하는 경우
	if( MyRoomInfoContext::i()->getStage()->IsAiMode() )
		m_BaseTime = 60;
	else
		m_BaseTime = 10;

	if( m_TimeCheck + (m_BaseTime * 1000) > timeGetTime() )
		return true;
	else
		return false;
}