// TaskProcessorImpl.cpp
//
// YouJong Ha
//	Last update : 2011-09-22 (yyyy:mm:dd)
//	
// Description:
//	- Task Processor의 세부 구현 파일.
//	- Task scheduling 작업은 상위 모듈인 task processor에서 구현되어 있고, 
//		이 파일에서는 할당된 룸 1개에 대한 세부적인 작업을 구현한다.
//
//	- 작업 데이터는 g_pDataArchive->Pop(groupIdx, ..)를 통해서 얻을 수 있으며
//	- 룸에 참가하고 있는 멤버는 그룹을 먼저 얻고 - eg. g_pRoomManager->GetGroup(groupIdx)
//	  멤버를 얻는 형태로 접근 가능하다.			 - eg. group->GetMember(slotIdx)
//	 
// Dependency: 
//				
//
#include "pch.h"
// frame work
#include "iocpServer.h"		// send 
#include "iocpWorkerImpl.h"	// send packet
#include "UdpBufferPool.h"	// memory pool
#include "PacketLocker.h"	// data archive / send queue
// group
#include "GroupMaker.h"
#include "DediGroup.h"
#include "DediMember.h"
// packet
#include "DediUdpBuilder.h" // UDPPacketBuilder
#include "DediUdpParser.h"  // UDPPacketParser
#include "HMSParser.h"		// HMS
#include "DediUdpChecker.h" // check packet contents 
// task processor
#include "TaskProcessorImpl.h"
#include "TaskProcessor.h"
#include "DroppedWeaponMgr.h"

#include "ModuleControl.h"


// client update process ( DriveGame() )
// 1. tcp update : update the game context directly or PushEventGame 
//		: queue : m_RecvEvent[] 
//		: pop   : GetEventGame()
//				where,	CStageBattle::ProcessGameContext()
//						CStageLogin::ProcessGameContext()
//						CStageReady::_GameEventPump()
//						UIPhaseLogin::ProcessGameEvent()
//						UIMainFrame::__GameEventPump()
// 2. udp update : update characters directly 
//		: send:  tNet을 update하고 send flag를 on 
//		: recv:  use recv buffer queue.
// (3. 난입처리)
// 4. mission update
// 5. quest update

void ProcessRoomData( INT32 i32TaskIdx, CDediGroup* pGroup, INT32 i32GroupIdx )
{
	INT32 i32RemainPacketCount  = 0; 
	INT32 i32ReadPos			= 0; 
	CUdpBuffer * pRecvBuffer	= NULL;
	CUdpBuffer * pSendBuffer	= NULL;
	REAL32	r32GameTime			= 0.0f;

	pGroup->UpdateBattleTime();

	// *********************************************************************************
	// PARSE UDP PACKET
	// *********************************************************************************

	//----------------------------------------------------------------------------------
	// 패킷 처리
	//----------------------------------------------------------------------------------

	i32RemainPacketCount = g_pDataArchive->GetCount( i32GroupIdx );
	if (i32RemainPacketCount > 0 )
	{
		// 패킷을 받은 시간 (마지막 패킷을 받은 시간 이후 5분 이상 지나면 방 없어짐)
		pGroup->SetUdpRecvTime(); 
	}
	
	UINT32 ui32FireActionFlag = 0;

	for(INT32 i = 0; i < i32RemainPacketCount; i++)
	{
		if( !g_pDataArchive->Pop( i32GroupIdx, (i3ElementBase**)&pRecvBuffer))
		{
			// 룸의 개수가 스레드의 정수배가 아닐 경우 동시에 접근하는 경우가 있다 
			DSWARNING("[g_pDataArchive->Pop] \n");
			continue; 
		}

		if( pRecvBuffer == NULL )
		{
			DSWARNING("[pRecvBuffer == NULL] \n" );
			continue; 
		}
		
		if( FALSE == pGroup->CheckEncryptKey( pRecvBuffer->GetEncryptKey() ) )
		{
			DSWARNING("pRecvBuffer->GetEncryptKey() != pGroup->GetEncryptKey() \n" );
			g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
			continue;
		}

		if( GetPacketAt() )
		{
			g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
			continue;
		}
		
		//------------------------------------------------------------------------------------
		//파싱
		i32ReadPos = 0; 
		char * pPacket = pRecvBuffer->GetBuffer(); 
		
		//1. 패킷 헤더 분리 
		N_GAME_HEADER * pHeader = (N_GAME_HEADER*)pPacket; 
		i32ReadPos += sizeof( N_GAME_HEADER );
		
		// 패킷 검사
		if( ! pGroup->CheckPacket( pHeader, pRecvBuffer->GetIp(), pRecvBuffer->GetPort()) )
		{
			// 패킷 오류시 check packet에서 로그 찍음
			g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
			continue;	
		}
		
		if ( pHeader->_ProtocolID & 0x80 )
		{
			// 핑 정보일 경우 4바이트 skip
			i32ReadPos += sizeof(REAL32);
		}
		
		INT32 i32DataSize = pHeader->_Size - i32ReadPos;

		
		// set task index
		pGroup->SetTaskIndex( i32TaskIdx ); // 파서 파라미터에 taskIdx 삭제 예정.

#ifdef ROUND_CONTROL_BY_DEDI
		if( pGroup->m_RoundMissionState == MISSION_STATE_BATTLE )
		{
#endif
			// 패킷 검사
			if( UDPChecker::GamePacketParsing( pGroup, &pPacket[i32ReadPos], i32DataSize, pHeader ) )
			{
				// 핵 체크
				if( HMSParser::GamePacketParsing( pGroup, &pPacket[i32ReadPos], i32DataSize, pHeader ) )
				{
					// 패킷 처리
					UDPParser::GamePacketParsing( pGroup, &pPacket[i32ReadPos], i32DataSize, pHeader, &ui32FireActionFlag );
				}
			}
#ifdef ROUND_CONTROL_BY_DEDI
		}
#endif

		//패킷 돌려주기
		g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
	}
	
#ifndef I3_DEBUG
	pGroup->CheckPosPacketTime( i32TaskIdx );
#endif

	// *********************************************************************************
	// ROOM STATE UPDATE
	// *********************************************************************************

	if( pGroup->GetRoundNumber() == 0xff )
	{	// 라운드 시작 안됨
		return;
	}

	//----------------------------------------------------------------------------------
	// 컨트롤 서버에 요청을 전송합니다.
	//----------------------------------------------------------------------------------
	pGroup->SendRequestToControlServer();

	//----------------------------------------------------------------------------------
	// 스테이지 update
	//----------------------------------------------------------------------------------
	
	r32GameTime = pGroup->GetGameTime();
	pGroup->UpdateSyncObject ( r32GameTime );
	pGroup->SendLifeForSync();
	
	if( pGroup->IsDeathMode() || pGroup->IsDestroyMissionMode() )
	{
		pGroup->GetDroppedWeaponMgr()->DecreaseRemainTime( r32GameTime );
	}

	//----------------------------------------------------------------------------------
	// 타이머를 사용하는 해킹 검사
	//----------------------------------------------------------------------------------

	pGroup->CheckClientTime		( r32GameTime );
	pGroup->CheckRespawnPos		( r32GameTime );    

	// *********************************************************************************
	// MULTICAST
	// *********************************************************************************

	//----------------------------------------------------------------------------------
	//멀티 캐스트 패킷 준비
	//----------------------------------------------------------------------------------

	// round number가 설정되어 있지 않으면 send하지 않는다.
	if( pGroup->GetRoundNumber() == INVALID_ROUND_NUMBER ) return;

	// Get Send Buffer
	g_pUdpBufferPool->GetBuffer( &pSendBuffer ); 
	if( pSendBuffer == NULL )
	{
		DSERROR("Send Buffer Error iTI = %d iGroupIdx = %d\n", i32TaskIdx, i32GroupIdx );
		return;						// 크리티컬한 문제입니다. 
	}


	BROADCASTINFOEX broadcastInfo;
	memset(&broadcastInfo, 0, sizeof( broadcastInfo ));

	broadcastInfo.m_r32GameTime			= r32GameTime; 
	broadcastInfo.m_ui32StageID			= pGroup->GetMyRoomStageID();
	broadcastInfo.m_ui32RoundNum		= pGroup->GetRoundNumber();

	// 게임 캐릭터 정보 버퍼
	broadcastInfo.m_pCastCheck_bWeaponSync			= pGroup->getChara_bWeaponSync(0);
	broadcastInfo.m_pCastCheck_Chara				= pGroup->getChara_Check(0)		;
#ifndef CHANGE_PROCESS_DEATH
	for( INT32 i = 0; i < SLOT_MAX_COUNT; ++i )
	{
		if( FALSE == broadcastInfo.m_pCastCheck_Chara[ i ].bCheckBroadcast ) continue;
		if( !pGroup->GetUserAlive(i) )
		{
			broadcastInfo.m_pCastCheck_bWeaponSync[ i ] = FALSE;
			continue;
		}

		if( broadcastInfo.m_pCastCheck_bWeaponSync[ i ] )
		{
			pGroup->m_r32LastWeaponSyncTime[ i ] = r32GameTime;
			continue;
		}

		if( pGroup->m_r32LastWeaponSyncTime[ i ] + 1.0f < r32GameTime )
		{
			broadcastInfo.m_pCastCheck_bWeaponSync[ i ] = TRUE;
			pGroup->m_r32LastWeaponSyncTime[ i ] = r32GameTime;
		}
	}
#endif
	broadcastInfo.m_pCastPacket_Chara_Def			= pGroup->getChara_Def(0)		;
	broadcastInfo.m_pCastPacket_Chara_State			= pGroup->getChara_State(0)		;
	broadcastInfo.m_pCastPacket_Chara_PosRot		= pGroup->getChara_PosRot(0)	;
	broadcastInfo.m_pCastPacket_Chara_ActionKey		= pGroup->getChara_ActionKey(0)	;
	broadcastInfo.m_pCastPacket_Chara_OnLoadObject	= pGroup->getChara_OnLoadObject(0);
	broadcastInfo.m_pCastPacket_Chara_SyncObject	= pGroup->getChara_SyncObject(0);
	broadcastInfo.m_pCastPacket_Chara_Radio			= pGroup->getChara_RadioChat(0)	;
	broadcastInfo.m_pCastPacket_Chara_Weapon		= pGroup->getChara_Weapon(0)	;
	broadcastInfo.m_pCastPacket_Chara_WeaponParam	= pGroup->getChara_WeaponParam(0);
	broadcastInfo.m_pCastPacket_Chara_Life			= pGroup->getChara_Life(0)		;
	broadcastInfo.m_pCastPacket_Chara_Bullet		= pGroup->getChara_Bullet(0)	;
	
	broadcastInfo.m_pCastPacket_Chara_HitCount_ByChara		= pGroup->getChara_HitCount_ByChara(0);
	broadcastInfo.m_pCastPacket_Chara_HitCount_ByExplosion	= pGroup->getChara_HitCount_ByExplosion(0);
	broadcastInfo.m_pCastPacket_Chara_HitCount_ByObject		= pGroup->getChara_HitCount_ByObject(0);

	broadcastInfo.m_ppCastPacket_Chara_Hit_ByChara	   = (N_PCINFO_HIT_DEDICATED2(*)[MAX_FIREHIT_COUNT])	pGroup->getChara_Hit_ByChara(0);
	broadcastInfo.m_ppCastPacket_Chara_Hit_ByExplosion = (N_PCINFO_HIT_BYEXPLOSION(*)[MAX_FIREHIT_COUNT])	pGroup->getChara_Hit_ByExplosion(0);
	broadcastInfo.m_ppCastPacket_Chara_Hit_ByObject	   = (N_PCINFO_HIT_BYOBJECT(*)[MAX_FIREHIT_COUNT])		pGroup->getChara_Hit_ByObject(0);
	
	broadcastInfo.m_pCastPacket_Chara_Suffering		= pGroup->getChara_Suffering(0)	;		// 피격입니다.
	broadcastInfo.m_pCastPacket_Chara_Death			= pGroup->getChara_Death(0)		;
	broadcastInfo.m_pCastPacket_Chara_SuicideCount	= pGroup->getChara_SuicideCount(0)	;	// 데미지 카운트
	broadcastInfo.m_ppCastPacket_Chara_Suicide		= (N_PCINFO_SUICIDEDMG(*)[MAX_SUICIDE_COUNT])pGroup->getChara_Suicide(0);
	broadcastInfo.m_pCastPacket_Chara_Mission		= pGroup->getChara_Mission(0)	;
	broadcastInfo.m_pCastPacket_Chara_DropW			= pGroup->getChara_DropWeapon(0);
	broadcastInfo.m_pCastPacket_Chara_GetW			= pGroup->getChara_GetWeapon(0)	;

#ifdef CHANGE_PROCESS_DEATH
	broadcastInfo.m_pCastPacket_Chara_Kill_Count	= pGroup->getChara_KillCount(0);
	broadcastInfo.m_ppCastPacket_Chara_Kill			= (N_PCINFO_KILL(*)[PACKET_REPEAT_COUNT]) pGroup->getChara_Kill(0);
#endif

	// 수류탄
	broadcastInfo.m_pbCastCheck_Grenade				= pGroup->getCheckGrenade(0)	;
	broadcastInfo.m_pCastPacket_Grenade				= pGroup->getGrenade(0)			;
	// 게임 오브젝트
	broadcastInfo.m_pbCastCheck_Obj_Static			= pGroup->getCheckObj_Static(0)		;
	broadcastInfo.m_pCastPacket_Obj_Static			= pGroup->getObj_Static(0)			;
	broadcastInfo.m_pNetIndex_Obj_Static			= pGroup->getNetIndex_Obj_Static(0)	;
	broadcastInfo.m_pGameObjects					= pGroup->getObj_StaticObject(0)	; // 참조용
	broadcastInfo.m_pbCastCheck_Obj_Anim			= pGroup->getCheckObj_Anim(0)		;
	broadcastInfo.m_pCastPacket_Obj_Anim			= pGroup->getObj_Anim(0)			; // ani sync
	broadcastInfo.m_pAnimationObjects				= pGroup->getObj_AnimationObject(0) ; // stage info
	broadcastInfo.m_pNetIndex_Obj_Anim				= pGroup->getNetIndex_Obj_Anim(0)	;
	broadcastInfo.m_pbCastCheck_DroppedWeapon		= pGroup->getCheckDroppedWeapon(0)	;
	broadcastInfo.m_pCastPacket_DroppedWeapon		= pGroup->getDroppedWeapon(0)		;
	broadcastInfo.m_pbCastCheck_Obj_Controlled		= pGroup->getCheckObj_Controlled(0)	;
	broadcastInfo.m_pControlledObjects				= pGroup->getObj_Controlled(0);
	broadcastInfo.m_pMissionIntruder				= pGroup->getMissionIntruder()		;
	// 인덱스
	broadcastInfo.m_pNextSend_Idx					= pGroup->getNextSend_Idx(0);
	// 패킷의 남는 공간에 stageinfo를 채우기 위함
	broadcastInfo.m_pStageObj_Count					= pGroup->getStageObj_Count(0);
	// 전송시간
	broadcastInfo.m_pr32LastSendTime_Anim			= pGroup->getLastSendTime_Anim(0);

	pSendBuffer->Init();

#if defined(CHANGE_PROCESS_DEATH) || defined(CHANGE_PROCESS_RESPAWN)
	INT32 ui32WrittenBytes = UDPCSBuilder::ServerPacketPacking( pSendBuffer->GetBuffer(), pSendBuffer->GetBufferSize(), &broadcastInfo, pGroup->getStageInfoTimer(), pGroup ); 
#else
	INT32 ui32WrittenBytes = UDPCSBuilder::ServerPacketPacking( pSendBuffer->GetBuffer(), pSendBuffer->GetBufferSize(), &broadcastInfo, pGroup->getStageInfoTimer() ); 
#endif
	
	//----------------------------------------------------------------------------------
	//유저들에게 패킷 보내기 
	if ( ui32WrittenBytes > 0 && ui32WrittenBytes < UDP_SEND_PACKET_SIZE )
	{
		// 패킷 구성 결과 결과 셋팅
		memcpy_s( pGroup->getNextSend_Idx(0), sizeof(INT32) * P2P_SUB_HEAD_COUNT,	
								broadcastInfo.m_pNextSend_Idx, sizeof(INT32) * P2P_SUB_HEAD_COUNT)	;

		for( UINT32 ui32SlotIdx = 0 ; ui32SlotIdx < SLOT_MAX_COUNT ; ui32SlotIdx++ )
		{
			//I3TRACE("%d send enable=%d\n", ui32SlotIdx, (BOOL)pGroup->IsSendEnable( ui32SlotIdx ));
			if( !pGroup->IsSendEnable( ui32SlotIdx ) ) continue;
			CDediMember* pMember = pGroup->GetMember( ui32SlotIdx );
			if( !pGroup->CheckMember( (UINT8)ui32SlotIdx, pMember->GetIp(), pMember->GetPort() ) ) continue;
			if( !pGroup->CheckValidItem( ui32SlotIdx )) continue;
			
			pSendBuffer->SetSocketIdx ( pGroup->GetSocketIndex() );
			pSendBuffer->SetAddress( pMember->GetIp(), pMember->GetPort() );
			pSendBuffer->SetBufferSize( ui32WrittenBytes );
			
			EchoPacket( pSendBuffer );
		}
	} // if (written bytes > 0)

	g_pUdpBufferPool->ReleaseBuffer( pSendBuffer );

#ifdef ROUND_CONTROL_BY_DEDI
	if( MISSION_STATE_BATTLE == pGroup->m_RoundMissionState )
	{
		if( TRUE == (pGroup->*pGroup->RoundEndChk)() )
		{
#ifdef CHANGE_PROCESS_RESPAWN
			pGroup->ResetCharAlive();
#endif
			pGroup->m_RoundMissionState = MISSION_STATE_PRE_BATTLE;
			g_pModuleControl->Send_PROTOCOL_BATTLE_RoundEnd( i32TaskIdx, &pGroup->GetRoomInfo(), pGroup->GetRoundEndType(), pGroup->GetRoundWinTeam() );
		}
	}
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// Function: ProcessRoomData
//
// Description:
//	- Data Archive에 누적된 패킷을 꺼내서 send queue를 구성한다 
//		- room index는 상위 iocpWorker.cpp에서 스케쥴하기 때문에 한 개의 룸에
//			에 대해서만 작업을 수행한다(수정).
//      - Send Queue index는 
//			가능한 인덱스는 ioThreadCount(config.ini->10..32)이고, 상위 모듈에서 할당.
// Arguments:

//	- i32SendQueueIdx	: send 버퍼의 index (iocpWorker 쓰레드의 수만큼 미리 할당되어 있다)
//	- i32GroupIdx		: room index
//	- ui32Delta			: 이전 호출과 현재 호출 사이의 간격(ms)
//	- bfCheckList		: DS_CHECKLIST_WORLD_TEST, DS_CHECKLIST_CHARACTER_INFO...(dediDef.h)
//	 
void Relay_ProcessRoomData( INT32 i32TaskThreadIdx, CDediGroup* pGroup, INT32 i32GroupIdx, UINT32 ui32Delta ) 
{
	I3ASSERT( g_pRoomManager );	// room manager
	I3ASSERT( g_pDataArchive );	// received packet storage
	
	INT32 i32RemainPacketCount = 0; 
	
	CUdpBuffer * pRecvBuffer = NULL;
	

	pGroup->UpdateBattleTime();
	//----------------------------------------------------------------------------------
	//패킷 처리
	i32RemainPacketCount = g_pDataArchive->GetCount( i32GroupIdx );
	
	if( i32RemainPacketCount > 0)
	{
		// 패킷을 받은 시간 (패킷을 받은 시간이 5분 이상 지나면 방 폭파)
		pGroup->SetUdpRecvTime(); 
	}
	
	for( INT32 i = 0; i < i32RemainPacketCount; i++)
	{
		if( !g_pDataArchive->Pop( i32GroupIdx, (i3ElementBase**)&pRecvBuffer))
		{
			// 룸의 개수가 스레드의 정수배가 아닐 경우 동시에 접근하는 경우가 있다 
			DSWARNING("[g_pDataArchive->Pop] \n");
			continue; 
		}

		if( pRecvBuffer == NULL )
		{
			DSWARNING("[pRecvBuffer == NULL] \n" );
			continue; 
		}
		
		// 비밀 키값이 맞는지 확인
		if( FALSE == pGroup->CheckEncryptKey( pRecvBuffer->GetEncryptKey() ) )
		{
			DSWARNING("[Relay_ProcessRoomData] INCRYPT KEY Group:%d, OrgKey:%d, UserKey:%d, IP:%d, Port:%d\n", 
				pRecvBuffer->GetGroupIdx(), pGroup->GetEncryptKey(), pRecvBuffer->GetEncryptKey(), pRecvBuffer->GetIp(), pRecvBuffer->GetPort() );
			
			g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
			continue;
		}

		//------------------------------------------------------------------------------------
		// 데스 패킷 파싱
		//INT32  i32ReadPos	= 0; 
		//char*  pPacket		= pRecvBuffer->GetBuffer(); 
		//REAL32 r32PacketTime;
		//
		////1. 패킷 헤더 분리 
		//N_GAME_HEADER * pHeader = (N_GAME_HEADER*)pPacket; 
		//r32PacketTime = pHeader->_GameTime;

		//i32ReadPos += sizeof( N_GAME_HEADER );

		////-- 핑 체크용 패킷인지 확인 한다. 
		//if( ( pHeader->_ProtocolID & 0x7F ) == UDPPROTOCOL_CS_INFO )
		//{
		//	if ( pHeader->_ProtocolID & 0x80 )
		//	{
		//		// 핑 정보일 경우 4바이트 skip
		//		i32ReadPos += sizeof(REAL32);
		//	}
		//	
		//	// 데스패킷을 검사한다
		//	HMSParser::RelayPacketParsing( i32TaskThreadIdx, pGroup, ui32GroupIdx, pHeader->_Index, &pPacket[i32ReadPos], pHeader->_Size - sizeof(N_GAME_HEADER), r32PacketTime );
		//}
		//------------------------------------------------------------------------------------

		// 방안의 유저들에게 전달
		for ( INT32 k = 0 ; k < SLOT_MAX_COUNT ; k++ )
		{
			if( k == pRecvBuffer->GetSlotIdx() )					continue;	// 방장은 받지 않는다.

			CDediMember* pMember = pGroup->GetMember( k );
			if( !pGroup->CheckMember( (UINT8)k, pMember->GetIp(), pMember->GetPort() ) ) continue;

			pRecvBuffer->SetAddress( pMember->GetIp(), pMember->GetPort() );
			
			// send packet
			EchoPacket( pRecvBuffer );
		}

		// 사용이 끝난 udp buffer를 release
		g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer);
	}


#ifdef ROUND_CONTROL_BY_DEDI
	if( MISSION_STATE_BATTLE == pGroup->m_RoundMissionState )
	{
		if( TRUE == (pGroup->*pGroup->RoundEndChk)() )
		{
#ifdef CHANGE_PROCESS_RESPAWN
			pGroup->ResetCharAlive();
#endif
			pGroup->m_RoundMissionState = MISSION_STATE_PRE_BATTLE;
			g_pModuleControl->Send_PROTOCOL_BATTLE_RoundEnd( i32TaskThreadIdx, &pGroup->GetRoomInfo(), pGroup->GetRoundEndType(), pGroup->GetRoundWinTeam() );
		}
	}
#endif

}

