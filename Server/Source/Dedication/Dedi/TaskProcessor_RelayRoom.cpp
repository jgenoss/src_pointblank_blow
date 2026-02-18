#include "pch.h"
#include "TaskProcessor.h"
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
#include "HMSParser.h"		// HMS
#include "DediUdpChecker.h" // check packet contents 
// task processor
#include "DroppedWeaponMgr.h"
#include "ModuleControl.h"
#include "ModuleCast.h"
#include "ServerStatistics.h"

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
void CTaskProcessor::ProcessRoom_Relay( INT32 i32TaskThreadIdx, CDediRoom* pRoom, INT32 i32GroupIdx, UINT32 ui32Delta ) 
{
	I3ASSERT( g_pRoomManager );	// room manager
	I3ASSERT( g_pDataArchive );	// received packet storage
	
	INT32 i32RemainPacketCount = 0; 
	
	CUdpBuffer * pRecvBuffer = NULL;
	
	//----------------------------------------------------------------------------------
	//패킷 처리
	i32RemainPacketCount = g_pDataArchive->GetCount( i32GroupIdx );
	
	if( i32RemainPacketCount > 0)
	{
		// 패킷을 받은 시간 (패킷을 받은 시간이 5분 이상 지나면 방 폭파)
		pRoom->SetUdpRecvTime(); 
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
		if( FALSE == pRoom->CheckEncryptKey( pRecvBuffer->GetEncryptKey() ) )
		{
			DSWARNING("[Relay_ProcessRoomData] INCRYPT KEY Group:%d, OrgKey:%d, UserKey:%d, IP:%d, Port:%d\n", 
				pRecvBuffer->GetGroupIdx(), pRoom->GetEncryptKey(), pRecvBuffer->GetEncryptKey(), pRecvBuffer->GetIp(), pRecvBuffer->GetPort() );
			
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
		//	HMSParser::RelayPacketParsing( i32TaskThreadIdx, pRoom, ui32GroupIdx, pHeader->_Index, &pPacket[i32ReadPos], pHeader->_Size - sizeof(N_GAME_HEADER), r32PacketTime );
		//}
		//------------------------------------------------------------------------------------

		// 방안의 유저들에게 전달
		for ( INT32 k = 0 ; k < SLOT_MAX_COUNT ; k++ )
		{
			if( k == pRecvBuffer->GetSlotIdx() )					continue;	// 방장은 받지 않는다.

			CDediMember* pMember = pRoom->GetMember( k );
			if( !pRoom->CheckMember( (UINT8)k, pMember->GetIp(), pMember->GetPort() ) ) continue;

			pRecvBuffer->SetAddress( pMember->GetIp(), pMember->GetPort() );
			
			// send packet
			EchoPacket( pRecvBuffer );
		}

		// 별도 파싱 작업 없으므로 카운트만 증가 시킵니다.
		g_pStatistics->AddPacketProcess(i32TaskThreadIdx, (REAL64)0.f);

		// 사용이 끝난 udp buffer를 release
		g_pUdpBufferPool->ReleaseBuffer( pRecvBuffer);
	}

	if( MISSION_STATE_BATTLE == pRoom->m_RoundMissionState )
	{
		if( TRUE == (pRoom->*pRoom->RoundEndChk)() )
		{
			pRoom->m_RoundMissionState = MISSION_STATE_PRE_BATTLE;
			g_pModuleCast->Send_PROTOCOL_BATTLE_RoundEnd( i32TaskThreadIdx, &pRoom->GetRoomInfo(), pRoom->GetRoundEndType(), pRoom->GetRoundWinTeam() );
		}
	}
}

