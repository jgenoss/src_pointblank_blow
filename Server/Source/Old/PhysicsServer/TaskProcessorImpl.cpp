// TaskProcessorImpl.cpp
//
// YouJong Ha
//	Last update : 2010-10-12 (yyyy:mm:dd)
//	
// Description:
//	- Task Processor의 세부 구현 파일.
//	- Task scheduling 작업은 상위 모듈인 task processor에서 구현되어 있고, 
//		이 파일에서는 할당된 룸 1개에 대한 세부적인 작업을 구현한다.
//
//	- 작업 데이터는 g_pDataArchive->Pop(groupIdx, ..)를 통해서 얻을 수 있으며
//	- 룸에 참가하고 있는 멤버는 그룹을 먼저 얻고 - eg. g_pGroupMaker->GetGroup(groupIdx)
//	  멤버를 얻는 형태로 접근 가능하다.			 - eg. group->GetMember(slotIdx)
//	 
// Dependency: 
//				
//
#include "pch.h"
#include "TaskProcessorImpl.h"
#include "TaskProcessor.h"
#include "UdpBufferPool.h"
#include "UdpBuffer.h"
#include "..\..\CommonSource\UDPPacketParser.h" // UDP_UTIL을 사용하기 위함

#include "MapData.h"
#include "MapMgr.h"
#include "TrackBuffer.h"
#include "UdpService.h"

inline
REAL32 distance_squared( VEC3D* v1, VEC3D* v2)
{
	return	( v2->x - v1->x ) * ( v2->x - v1->x ) +
			( v2->y - v1->y ) * ( v2->y - v1->y ) +
			( v2->z - v1->z ) * ( v2->z - v1->z );

}


void ProcessRecvPacket( INT32 i32ThreadIdx, CUdpBuffer* pRecvBuf ) 
{
	I3TRACE("Process...\n");
	
	CUdpBuffer * pSendBuf = NULL;

	PHYSICS_PACKET_HEADER*		pHead	 = (PHYSICS_PACKET_HEADER*)pRecvBuf->GetBuffer();
	N_PCINFO_HIT_DEDICATED2*	pHitInfo = (N_PCINFO_HIT_DEDICATED2*)( pRecvBuf->GetBuffer() + sizeof(PHYSICS_PACKET_HEADER) );
	NxRay		 ray;
	NxRaycastHit hit;
	
	// get scene query
	STAGEORD stageOrd = (STAGEORD)pHead->m_ui8StageOrd;

	CMapData* pMap	  = g_pMapMgr->GetMap( stageOrd );
	if( pMap == NULL )
	{
		DSERROR("pMap == NULL\n");
		return;
	}
	
	// send queue
	g_pUdpBufferPool->GetBuffer(&pSendBuf);
	if( pSendBuf == NULL )
	{
		DSERROR("GetBuffer\n");
		return;
	}

	UINT32* pui32Result = (UINT32*)( pSendBuf->GetBuffer() + sizeof( PHYSICS_PACKET_HEADER ) );
	UINT32 ui32WallShotCount = 0;
	UINT32 ui32RayCount = g_pConfig->m_ui32RayCount;
	//// test beg
	//pHitInfo->_FirePos.x =12.90;
	//pHitInfo->_FirePos.y = 2.92;
	//pHitInfo->_FirePos.z = -17.54;
	//pHitInfo->_HitPos.x = -7.47;
	//pHitInfo->_HitPos.y = 4.11;
	//pHitInfo->_HitPos.z = -2.63;
	//// test end
	NxVec3 vFire	= NxVec3( pHitInfo->_FirePos.x, pHitInfo->_FirePos.y, pHitInfo->_FirePos.z );
	NxVec3 vTarget	= NxVec3( pHitInfo->_HitPos.x, pHitInfo->_HitPos.y, pHitInfo->_HitPos.z );
	
	I3TRACE("vFire  :(%f,%f,%f)\n",	vFire.x, vFire.y, vFire.z );
	I3TRACE("vTarget:(%f,%f,%f)\n",	vTarget.x, vTarget.y, vTarget.z );
	
	// Fire와 Target으로부터 Normal vector 계산
	NxVec3 Eye(  vTarget.x - vFire.x, vTarget.y - vFire.y, vTarget.z - vFire.z  );
	NxVec3 N = Eye.cross( NxVec3( 0.0f, 1.0f, 0.0f) );
	N.normalize();
	
	NxVec3 vOffset;  // Fire를 중심으로 추가적인 ray를 생성하기 위한 옵셋
	ui32WallShotCount = 0;
			
	for( UINT32 i = 0; i < ui32RayCount; i++ ) 
	{
		if( ui32RayCount == 1 )
		{
			vOffset.zero();
		}
		else if( ui32RayCount == 3) // 상하로 레이를 2개 생성한다.
		{
			switch( i )
			{
			case 0: // Y + 0.5
				vOffset = NxVec3( 0.0f,  0.5f, 0.0f);	break;
			case 1: // Y - 0.5
				vOffset = NxVec3( 0.0f, -0.5f, 0.0f);	break;
			case 2: // ORIGINAL
				vOffset = NxVec3( 0.0f,  0.0f, 0.0f);	break;
			} // switch
		}
		else if( ui32RayCount == 5) // 상하좌우로 추가 레이를 4개 생성한다.
		{
			switch( i )
			{
			case 0: // Y + 1
				vOffset = NxVec3( 0.0f,  0.5f, 0.0f);	break;
			case 1: // Y - 1
				vOffset = NxVec3( 0.0f, -0.5f, 0.0f);	break;
			case 2: // +N
				vOffset =  N * 0.5f; break;
			case 3: // -N
				vOffset =  N * -0.5f; break;
			case 4: // ORIGINAL
				vOffset.zero(); break;
			} // switch
		}

		// move start point
		NxVec3 vStart = vFire + vOffset;
	
		// build ray
		ray.orig = NxVec3( vStart.x, vStart.y, vStart.z );
		ray.dir	 = vTarget - vStart;
		ray.dir.normalize();
		
		if( pMap->CheckRaycastHit( ray, vFire.distanceSquared( vTarget ) ) == FALSE )
		{
			ui32WallShotCount++;
		}
		
		g_pStatistics->IncrementRequests();
	}
	
	// 결과
	*pui32Result = HACK_TYPE_NO;
	if( ui32WallShotCount == ui32RayCount) // 전부 월샷 판정이면 월샷.
	{
		g_pStatistics->IncrementWallShots();

		*pui32Result = HACK_TYPE_WALLSHOT;
		char szCodeName[MAX_PATH] = { '\0', };
		::getStageCodeName(szCodeName, (STAGE_ID)pHead->m_ui8StageOrd );
		DSERROR("WALL SHOT: stage=%s, group=%d, user=%d fire:(%.2f, %.2f, %.2f) hit:(%.2f, %.2f, %.2f)\n",
				szCodeName, pHead->m_ui16GroupIdx, pHead->m_ui8SlotIdx, 
				vFire.x,  vFire.y, vFire.z,
				vTarget.x, vTarget.y, vTarget.z );
	}
	
	// fill header
	PHYSICS_PACKET_HEADER* pSendHead = (PHYSICS_PACKET_HEADER*)pSendBuf->GetBuffer();
	memcpy_s( pSendHead, sizeof(PHYSICS_PACKET_HEADER), pHead, sizeof(PHYSICS_PACKET_HEADER));
	
	pSendBuf->SetBufferSize( sizeof( PHYSICS_PACKET_HEADER ) + sizeof(UINT32) );
	pSendBuf->SetAddress( pRecvBuf->GetAddress(), *pRecvBuf->GetAddressSize() );

	int rc = g_pUdpService->SendPacket( pSendBuf );
	if( SOCKET_ERROR == rc )
	{
		if ( WSAGetLastError() != WSAEWOULDBLOCK )
		{
			if( g_pSendQueue->Push( pSendBuf ) == FALSE )
			{
				g_pUdpBufferPool->ReleaseBuffer( pSendBuf );
			}
			else
			{
				DSERROR("PENDING: g_pSendQeue->Push OK.\n");
			}
			pSendBuf = NULL;
		}
	}
	else
	{
		DSMSG("[UDPS] SENT %d Bytes OK\n", rc);
	}

	if( pSendBuf ) g_pUdpBufferPool->ReleaseBuffer( pSendBuf );
}
