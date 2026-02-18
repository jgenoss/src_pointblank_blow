#include "pch.h"
#include "TaskProcessor.h"
#include "pch.h"
// frame work
#include "iocpServer.h"		// send 
#include "iocpWorkerImpl.h"	// send packet
#include "UdpBufferPool.h"	// memory pool
#include "PacketLocker.h"	// data archive / send queue
// group
#include "GroupMaker.h"
#include "DediRoom.h"
#include "DediMember.h"
// packet
#include "DediUdpBuilder.h" // UDPPacketBuilder
#include "DediUdpParser.h"  // UDPPacketParser
//#include "DediUdpParser2.h"  // UDPPacketParser
#include "HMSParser.h"		// HMS
#include "DediUdpChecker.h" // check packet contents 
// task processor
#include "DroppedWeaponMgr.h"
#include "ModuleControl.h"
#include "ModuleCast.h"
#include "ServerStatistics.h"


#define __PACKET_PARSING_ORG__

void CTaskProcessor::ProcessRoom( INT32 i32TaskIdx, CDediRoom* pRoom, INT32 i32GroupIdx )
{
	INT32 i32RemainPacketCount  = 0; 
	INT32 i32ReadPos			= 0; 
	CUdpBuffer * pRecvBuffer	= NULL;
	CUdpBuffer * pSendBuffer	= NULL;
	REAL32	r32GameTime			= 0.0f;


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
		pRoom->SetUdpRecvTime(); 
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
		
		if( FALSE == pRoom->CheckEncryptKey( pRecvBuffer->GetEncryptKey() ) )
		{
			DSWARNING("pRecvBuffer->GetEncryptKey() != pRoom->GetEncryptKey() \n" );
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
		
		RECV_PACKET_INFO RecvPacketInfo;
		//1. 패킷 헤더 분리 
		RecvPacketInfo.m_pHeader = (N_GAME_HEADER*)pPacket;
		i32ReadPos += sizeof( N_GAME_HEADER );
		
		// 패킷 검사
		if( ! pRoom->CheckPacket( RecvPacketInfo.m_pHeader, pRecvBuffer->GetIp(), pRecvBuffer->GetPort()) )
		{
			// 패킷 오류시 check packet에서 로그 찍음
			g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
			continue;	
		}
		
		if ( RecvPacketInfo.m_pHeader->_ProtocolID & 0x80 )
		{
			// 핑 정보일 경우 4바이트 skip
			i32ReadPos += sizeof(REAL32);
		}
		
		INT32 i32DataSize = RecvPacketInfo.m_pHeader->_Size - i32ReadPos;
				
		// set task index
		pRoom->SetTaskIndex( i32TaskIdx ); // 파서 파라미터에 taskIdx 삭제 예정.

		// 패킷 처리 테스트코드
		LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
		QueryPerformanceFrequency(&stQP_Freq_Begin);
		QueryPerformanceCounter(&stQP_Count_Begin);

		if( pRoom->m_RoundMissionState == MISSION_STATE_BATTLE )
		{
			// 패킷 검사
			if( UDPChecker::GamePacketParsing( pRoom, &pPacket[i32ReadPos], i32DataSize, &RecvPacketInfo ) )
			{
				// 핵 체크
				if( HMSParser::GamePacketParsing( pRoom, &RecvPacketInfo ) )
				{
					// 패킷 처리
#ifdef __PACKET_PARSING_ORG__
					UDPParser::GamePacketParsing( pRoom, &RecvPacketInfo, &ui32FireActionFlag );
#else
					UDPParser2::GamePacketParsing( pRoom, &RecvPacketInfo, &ui32FireActionFlag );
#endif
				}
			}
		}

		// 패킷 처리 테스트코드
		QueryPerformanceCounter(&stQP_Count_End);
		stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart;

		REAL64 r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart;

		g_pStatistics->AddPacketProcess(i32TaskIdx, r64ElepseTime);

		//패킷 돌려주기
		g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer );
	}
	
#ifndef I3_DEBUG
	pRoom->CheckPosPacketTime( i32TaskIdx );
#endif

	// *********************************************************************************
	// ROOM STATE UPDATE
	// *********************************************************************************

	if( pRoom->GetRoundNumber() == 0xff )
	{	// 라운드 시작 안됨
		return;
	}

	//----------------------------------------------------------------------------------
	// 컨트롤 서버에 요청을 전송합니다.
	//----------------------------------------------------------------------------------
	pRoom->SendRequestToControlServer();

	//----------------------------------------------------------------------------------
	// 스테이지 update
	//----------------------------------------------------------------------------------
	
	r32GameTime = pRoom->GetGameTime();
	pRoom->UpdateSyncObject ( r32GameTime );
	pRoom->SendLifeForSync();
	
	//if( pRoom->IsDeathMode() || pRoom->IsDestroyMissionMode() )
	//{
	//	pRoom->GetDroppedWeaponMgr()->DecreaseRemainTime( r32GameTime );
	//}

	//----------------------------------------------------------------------------------
	// 타이머를 사용하는 해킹 검사
	//----------------------------------------------------------------------------------

	pRoom->CheckClientTime		( r32GameTime );

	// *********************************************************************************
	// MULTICAST
	// *********************************************************************************

	//----------------------------------------------------------------------------------
	//멀티 캐스트 패킷 준비
	//----------------------------------------------------------------------------------

	// round number가 설정되어 있지 않으면 send하지 않는다.
	if( pRoom->GetRoundNumber() == INVALID_ROUND_NUMBER ) return;

	// Get Send Buffer
	g_pUdpBufferPool->GetBuffer( &pSendBuffer ); 
	if( pSendBuffer == NULL )
	{
		DSERROR("Send Buffer Error iTI = %d iGroupIdx = %d\n", i32TaskIdx, i32GroupIdx );
		return;						// 크리티컬한 문제입니다. 
	}
	
	DS_BROADCASTINFOEX * pBroadCastInfo = pRoom->GetBroadCastInfo();

	pBroadCastInfo->SettingBasicInfo( r32GameTime, pRoom->GetRoundNumber() );

	for( INT32 i = 0; i < SLOT_MAX_COUNT; ++i )
	{
		if( FALSE == pBroadCastInfo->m_pCastCheck_Chara[ i ].bCheckBroadcast ) continue;
		if( !pRoom->GetUserAlive(i) )
		{
			pBroadCastInfo->m_pCastCheck_bWeaponSync[ i ] = FALSE;
			continue;
		}
		if( pBroadCastInfo->m_pCastCheck_bWeaponSync[ i ] )
		{
			pRoom->m_r32LastWeaponSyncTime[ i ] = r32GameTime;
			continue;
		}
		if( pRoom->m_r32LastWeaponSyncTime[ i ] + 1.0f < r32GameTime )
		{
			pBroadCastInfo->m_pCastCheck_bWeaponSync[ i ] = TRUE;
			pRoom->m_r32LastWeaponSyncTime[ i ] = r32GameTime;
		}
	}

	pSendBuffer->Init();

	INT32 i32EventBytes = sizeof(N_GAME_HEADER);
	i32EventBytes		+= UDPCSBuilder::PacketPackingEvent( pSendBuffer->GetBuffer()+i32EventBytes, UDP_SEND_PACKET_SIZE - i32EventBytes, pBroadCastInfo, pRoom ); 
	
	//----------------------------------------------------------------------------------
	//유저들에게 패킷 보내기 	
	// 패킷 구성 결과 결과 셋팅
	memcpy_s( pRoom->getNextSend_Idx(0), sizeof(INT32) * P2P_SUB_HEAD_COUNT, pBroadCastInfo->m_pNextSend_Idx, sizeof(INT32) * P2P_SUB_HEAD_COUNT)	;

	CDediMember* pMember;
	INT32 i32WrittenBytes;

	for( INT32 i32SlotIdx = 0 ; i32SlotIdx < SLOT_MAX_COUNT ; i32SlotIdx++ )
	{
		if( FALSE == pRoom->IsSendEnable( i32SlotIdx ) )												continue;
		pMember = pRoom->GetMember( i32SlotIdx );
		if( FALSE == pRoom->CheckMember( (UINT8)i32SlotIdx, pMember->GetIp(), pMember->GetPort() ) )	continue;
		if( FALSE == pRoom->CheckValidItem( i32SlotIdx ))												continue;

		i32WrittenBytes = i32EventBytes;

#ifdef __USE_PACKET_OPTIMIZE__
		i32WrittenBytes += UDPCSBuilder::PacketPackingSync( pSendBuffer->GetBuffer() + i32EventBytes, pSendBuffer->GetBufferSize() - i32EventBytes, pBroadCastInfo, pRoom, i32SlotIdx );
#endif

		if( sizeof(N_GAME_HEADER) >= i32WrittenBytes )													continue;

		UDPCSBuilder::PacketPackingHead( pSendBuffer->GetBuffer(), i32WrittenBytes, pBroadCastInfo );
		
		if (i32WrittenBytes >= UDP_SEND_PACKET_SIZE)
		{
			DSMSG("WrittenBytes >= UDP_SEND_PACKET_SIZE size : %d\n", i32WrittenBytes);
		}

		pSendBuffer->SetSocketIdx ( pRoom->GetSocketIndex() );
		pSendBuffer->SetAddress( pMember->GetIp(), pMember->GetPort() );
	
		EchoPacket( pSendBuffer );
	}

	g_pUdpBufferPool->ReleaseBuffer( pSendBuffer );

	if( MISSION_STATE_BATTLE == pRoom->m_RoundMissionState )
	{
		if( TRUE == (pRoom->*pRoom->RoundEndChk)() )
		{
			pRoom->m_RoundMissionState = MISSION_STATE_PRE_BATTLE;
			g_pModuleCast->Send_PROTOCOL_BATTLE_RoundEnd( i32TaskIdx, &pRoom->GetRoomInfo(), pRoom->GetRoundEndType(), pRoom->GetRoundWinTeam() );
		}
	}

}
