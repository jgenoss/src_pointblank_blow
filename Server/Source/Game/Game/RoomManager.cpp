#include "pch.h"
#include "Room.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "ModuleContextNc.h"

CRoomManager * g_pRoomManager = NULL; 

I3_CLASS_INSTANCE(CRoomManager, i3ElementBase); 

#define CHECKVAL_ROOM_ID	if( i32RoomIdx == SERVER_VALUE_UNKNOWN )\
{\
	SendErrorMessage( EVENT_ERROR_GAME_WRONG_CHANNEL ); \
	return; \
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
CRoomManager::CRoomManager()
{
	//For Channel 
	m_i32UseRoomCount		= 0;
	m_ui32ChannelCount		= 0;
	m_ui32EachChannelMaxCount	= 0; 
	m_pChannelRoomList		= NULL;			
	m_pcsChannelRoom		= NULL;		
	m_pChannelResRoomList	= NULL; 
	m_i32AccRoomCount		= 0; 
	m_InfoSide				= NULL;
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
CRoomManager::~CRoomManager()
{
	OnDestroy(); 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoomManager::OnCreate(void)		
{	
	//////////////////////////////////////////////////////////////////////////////////////////
	//Set Value
	m_ui32ChannelCount			= GAME_CHANNEL_COUNT; 
	m_ui32EachChannelMaxCount	= g_pContextMain->m_ui32ChannelSessionCount;

	//////////////////////////////////////////////////////////////////////////////////////////
	//Create Room Resource 
	m_pChannelResRoomList = (i3List **) i3MemAlloc( sizeof( i3List * )  * m_ui32ChannelCount ); 
	CRoom * pRoom; 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++ )
	{
		m_pChannelResRoomList[i] = i3List::NewObject(); 
		m_pChannelResRoomList[i]->SetOptmizeEnableWhenClear(FALSE); 
		m_pChannelResRoomList[i]->SetAllocationUnit(m_ui32EachChannelMaxCount);
		for(UINT32 j = 0; j < m_ui32EachChannelMaxCount; j++)
		{
			pRoom  = CRoom::NewObject(); 			
			pRoom->OnCreate( i, j );
			m_pChannelResRoomList[i]->Add( pRoom );
		}
	}

	//Create Change RoomBuffer
	m_pChangeRoomListTime = (UINT32 *) i3MemAlloc( sizeof(UINT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_pChangeRoomListTime, sizeof(UINT32) * m_ui32ChannelCount ); 
	m_pRoomListUpdateTime = (UINT32 *) i3MemAlloc( sizeof(UINT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_pRoomListUpdateTime, sizeof(UINT32) * m_ui32ChannelCount ); 


	//////////////////////////////////////////////////////////////////////////////////////////
	//Create Channel 	
	m_pChannelRoomList	= (i3List**)  i3MemAlloc( sizeof( i3List * ) * m_ui32ChannelCount ); 
	m_pcsChannelRoom	= (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_pChannelRoomList[i]	= i3List::NewObject();		
		m_pChannelRoomList[i]->SetOptmizeEnableWhenClear(FALSE); 
		m_pChannelRoomList[i]->SetAllocationUnit(m_ui32EachChannelMaxCount); 
		m_pcsChannelRoom[i]		= i3Mutex::NewObject(); 
	}	

	//////////////////////////////////////////////////////////////////////////////////////////
	//Create RoomInfo 
	//Side 0
	m_pRoomRoomInfo0	= (ROOM_INFO_BASIC **)	i3MemAlloc( sizeof( ROOM_INFO_BASIC *) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++) 
	{
		m_pRoomRoomInfo0[i] = (ROOM_INFO_BASIC *)i3MemAlloc( sizeof( ROOM_INFO_BASIC *) * m_ui32EachChannelMaxCount ); 
	}	
	m_pRoomCount0		= (UINT32 *)i3MemAlloc( sizeof( UINT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_pRoomCount0, sizeof( UINT32) * m_ui32ChannelCount ); 

	//Side 1 
	m_pRoomRoomInfo1	= (ROOM_INFO_BASIC **)	i3MemAlloc( sizeof( ROOM_INFO_BASIC *) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++) 
	{
		m_pRoomRoomInfo1[i] = (ROOM_INFO_BASIC *)i3MemAlloc( sizeof( ROOM_INFO_BASIC ) * m_ui32EachChannelMaxCount ); 
	}
	m_pRoomCount1		= (UINT32 *)i3MemAlloc( sizeof( UINT32) * m_ui32ChannelCount );
	i3mem::FillZero( m_pRoomCount1, sizeof( UINT32) * m_ui32ChannelCount );

	m_InfoSide = (INT32 *)i3MemAlloc( sizeof( INT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_InfoSide, sizeof( INT32) * m_ui32ChannelCount ); 

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoomManager::OnDestroy(void)
{	
	I3MEM_SAFE_FREE( m_pRoomListUpdateTime );
	I3MEM_SAFE_FREE( m_pChangeRoomListTime );

	//For Channel 
	for( UINT32 i =0 ; i < m_ui32ChannelCount; i++)
	{
		I3_SAFE_RELEASE( m_pChannelRoomList[i] ); 
		I3_SAFE_RELEASE( m_pcsChannelRoom[i] ); 
	}
	I3MEM_SAFE_FREE( m_pChannelRoomList ); 	
	I3MEM_SAFE_FREE( m_pcsChannelRoom ); 	

	//For Resource
	CRoom * pRoom;
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		for(UINT32 j = 0; j < m_ui32EachChannelMaxCount; j++)
		{
			pRoom = (CRoom * ) m_pChannelResRoomList[i]->GetItem(j); 
			I3_SAFE_RELEASE( pRoom ); 
		}
		I3_SAFE_RELEASE( m_pChannelResRoomList[i] ); 
	}
	I3MEM_SAFE_FREE( m_pChannelResRoomList ); 

	//For RoomInfo 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++) 
	{
		I3MEM_SAFE_FREE( m_pRoomRoomInfo0[i] );
		I3MEM_SAFE_FREE( m_pRoomRoomInfo1[i] );

	}

	I3MEM_SAFE_FREE( m_pRoomRoomInfo0 );
	I3MEM_SAFE_FREE( m_pRoomCount0 ); 

	I3MEM_SAFE_FREE( m_pRoomRoomInfo1 ); 
	I3MEM_SAFE_FREE( m_pRoomCount1 );
	I3MEM_SAFE_FREE( m_InfoSide ); 

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoomManager::OnSendRoomList(CUserSession * pSession, INT32 i32Channel, LOBBY_USER_INFO* pLobbyUserInfo, UINT32 ui32UserCount )
{	
	if(i32Channel == SERVER_VALUE_UNKNOWN || i32Channel >= GAME_CHANNEL_COUNT )return; 

	ROOM_INFO_BASIC **	pRoomInfo;
	UINT32				ui32RoomCount;
	UINT32				ui32PageNum = pSession->m_ui32UpdateRoomListPage;
	INT32				i32UserPageNum = pSession->m_UpdateUserListPage;

	if(m_InfoSide[i32Channel] == 0)
	{
		pRoomInfo = (ROOM_INFO_BASIC **)m_pRoomRoomInfo0[i32Channel];
		ui32RoomCount = m_pRoomCount0[i32Channel];
	}
	else 
	{
		pRoomInfo = (ROOM_INFO_BASIC **)m_pRoomRoomInfo1[i32Channel];
		ui32RoomCount = m_pRoomCount1[i32Channel]; 
	}

	UINT32 ui32PageCount = (UINT32)(ui32RoomCount / SEND_MAX_ROOM_COUNT);

	if( (ui32RoomCount % SEND_MAX_ROOM_COUNT) > 0) ui32PageCount++; 
	if( ui32PageNum > ui32PageCount) ui32PageNum = 0;

	UINT32 ui32StartIdx	= ui32PageNum * SEND_MAX_ROOM_COUNT;
	UINT32 ui32EndIdx	= ui32StartIdx + SEND_MAX_ROOM_COUNT;
	if(ui32EndIdx > ui32RoomCount)ui32EndIdx = ui32RoomCount;
	UINT32 ui32PacketRoomCount = ui32EndIdx - ui32StartIdx;

	i3NetworkPacket RoomListPacket( PROTOCOL_LOBBY_GET_ROOMLIST_ACK );
	RoomListPacket.WriteData( &ui32RoomCount,		sizeof(INT32));
	RoomListPacket.WriteData( &ui32PageNum,			sizeof(INT32));
	RoomListPacket.WriteData( &ui32PacketRoomCount,	sizeof(INT32));

	for(UINT32 i = ui32StartIdx; i < ui32EndIdx; i++)
	{		
		RoomListPacket.WriteData( pRoomInfo[i], sizeof(ROOM_INFO_BASIC));
	}

	////////////////////////////////////////////////////////////////////////
	// UserList

	ui32PageCount = ui32UserCount / SEND_MAX_LOBBY_USER_COUNT;

	if( (ui32UserCount % SEND_MAX_LOBBY_USER_COUNT) > 0) ui32PageCount++;
	if( i32UserPageNum > (INT32)ui32PageCount) i32UserPageNum = 0;

	ui32StartIdx	= i32UserPageNum * SEND_MAX_LOBBY_USER_COUNT;
	ui32EndIdx		= ui32StartIdx + SEND_MAX_LOBBY_USER_COUNT;
	if(ui32EndIdx > ui32UserCount)		ui32EndIdx = ui32UserCount;
	UINT32 PacketUserCount = ui32EndIdx - ui32StartIdx;
	
	RoomListPacket.WriteData( &ui32UserCount,		sizeof(UINT32));
	RoomListPacket.WriteData( &i32UserPageNum,	sizeof(INT32));
	RoomListPacket.WriteData( &PacketUserCount, sizeof(INT32));

	CUserSession * pSessionLobby;
	for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
	{
		pSessionLobby = (CUserSession*)pLobbyUserInfo[ i ].pSession;

		RoomListPacket.WriteData( &pSessionLobby->m_SessionIdx,						sizeof(INT32) );
		RoomListPacket.WriteData( &pSessionLobby->m_UserInfoBasic.m_ui32ClanMark,	sizeof(UINT32));
		RoomListPacket.WriteData( pSessionLobby->m_UserInfoBasic.m_strClanName,		sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
		RoomListPacket.WriteData( &pSessionLobby->m_ui32ViewRank,					sizeof(UINT8));
		RoomListPacket.WriteData( &pSessionLobby->m_UserInfoBasic.m_ui8ESportID,	sizeof(UINT8));
		RoomListPacket.WriteData( pSessionLobby->m_UserInfoBasic.m_strNick,			sizeof(TTCHAR) * NET_NICK_NAME_SIZE );
		RoomListPacket.WriteData( &pSessionLobby->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8) );
		RoomListPacket.WriteData( &pSessionLobby->m_ui8Country,						sizeof(UINT8));
	}

	// 현재 PacketSize 는 1234 입니다. 2017.01.03 빵돌이 노탁구
	pSession->SendPacketMessage( &RoomListPacket );

	//I3TRACE("@@@@ Protocol -  PROTOCOL_LOBBY_GET_ROOMLIST_ACK\n"); 

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoomManager::OnUpdate(void)
{	
	CRoom		*	pRoom;
	INT32			i32UseRoomCount; 
	INT32			i32InsertInfoSide	= 0;
	ROOM_INFO_BASIC	**	ppInsertRoomInfo	= NULL;
	UINT32		*	pui32InsertRoomCount	= NULL;
	UINT32			ui32ATNowTime		= i3ThreadTimer::GetServerTime();
	BOOL			bUpdateRoomList		= FALSE;

	//방이 변경되었는지 확인을 합니다. 
	if( (m_pChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel] > m_pRoomListUpdateTime[g_pContextMain->m_ui32UpdateChannel]) 
		||	(m_pChannelRoomList[g_pContextMain->m_ui32UpdateChannel]->GetCount() != GetChannelUseRoomCount(g_pContextMain->m_ui32UpdateChannel)) )
	{ 
		m_pRoomListUpdateTime[g_pContextMain->m_ui32UpdateChannel] = m_pChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel]; 
		bUpdateRoomList = TRUE;
	}

	//땜빵입니다. //5초마다 리스트 업데이트 합니다. 
	if( (bUpdateRoomList == FALSE) && ( (m_pChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel] + 5) < ui32ATNowTime) )
	{
		m_pRoomListUpdateTime[g_pContextMain->m_ui32UpdateChannel] = m_pChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel] = ui32ATNowTime; 
		bUpdateRoomList = TRUE;
	}

	//Pre Update RoomList 
	if(bUpdateRoomList)
	{
		if(m_InfoSide[g_pContextMain->m_ui32UpdateChannel] == 0)i32InsertInfoSide = 1; 
		if(i32InsertInfoSide == 0)
		{
			ppInsertRoomInfo		= (ROOM_INFO_BASIC **)m_pRoomRoomInfo0[g_pContextMain->m_ui32UpdateChannel];
			pui32InsertRoomCount	= &m_pRoomCount0[g_pContextMain->m_ui32UpdateChannel];
		}
		else 
		{
			ppInsertRoomInfo		= (ROOM_INFO_BASIC **)m_pRoomRoomInfo1[g_pContextMain->m_ui32UpdateChannel];
			pui32InsertRoomCount	= &m_pRoomCount1[g_pContextMain->m_ui32UpdateChannel];
		}
	}

	//Update Room And AddList
	i32UseRoomCount = 0; 
	for(UINT32 i = 0; i < m_ui32EachChannelMaxCount; i++)
	{
		pRoom = (CRoom*)m_pChannelResRoomList[ g_pContextMain->m_ui32UpdateChannel ]->GetItem(i);
		if( !pRoom->m_bIsCreate )continue;

		pRoom->OnUpdateRoom_M(ui32ATNowTime);	//업데이트만 합니다. 리턴값에 따라 방을 삭제하는 부분은 필요없음.

		// 튜토리얼 모드는 패스 합니다. 리스트에 포함하지 않음.
		if ( pRoom->IsTutorialMode() )continue; 

		// 클랜 매치 메이킹도 리스트에 포함하지 않습니다.
		if ( pRoom->IsMatchMaking() )continue; 

		//Update RoomList
		if(bUpdateRoomList)
		{
			ppInsertRoomInfo[i32UseRoomCount] = pRoom->GetInfo();
		}

		i32UseRoomCount++; 
	}

	//Use Room Count
	if(g_pContextMain->m_ui32UpdateChannel == 0)
	{
		m_i32AccRoomCount = m_i32AccRoomCount + i32UseRoomCount; 
		m_i32UseRoomCount = m_i32AccRoomCount; 
		m_i32AccRoomCount = 0;
	}else
	{
		m_i32AccRoomCount = m_i32AccRoomCount + i32UseRoomCount; 
	}

	//Post Upate RoomList
	if(bUpdateRoomList)
	{	
		*pui32InsertRoomCount	= i32UseRoomCount;
		m_InfoSide[g_pContextMain->m_ui32UpdateChannel] = i32InsertInfoSide; 
	}
}


/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoomManager::OnCreateRoom( CUserSession * pSession, ROOM_INFO_CREATE* pInfoCreate )
{
	INT32 i32RoomIdx = INVALID_IDX;
	INT32 i32ChannelNum	= pInfoCreate->nChannelNum;

	m_pcsChannelRoom[i32ChannelNum]->Enter();
	{
		CRoom* pRoom = NULL;
		for( INT32 i = 0; i < static_cast<INT32>(m_ui32EachChannelMaxCount); ++i )
		{
			pRoom = static_cast<CRoom*>( m_pChannelResRoomList[i32ChannelNum]->GetItem(i) );
			if( !pRoom || pRoom->m_bIsCreate )
				continue;;

			if( !pRoom->CreateRoom(pSession, pInfoCreate) )
				break;

			pRoom->m_bIsCreate = TRUE;
			i32RoomIdx = pRoom->m_Info._InfoBasic._Idx;

			break;
		}

		if( i32RoomIdx != INVALID_IDX )
		{
			if( m_pChannelRoomList[i32ChannelNum]->GetCount() < static_cast<INT32>(m_ui32EachChannelMaxCount) )
				m_pChannelRoomList[i32ChannelNum]->Add( pRoom );
		}

		m_pChangeRoomListTime[i32ChannelNum] = i3ThreadTimer::GetServerTime();
	}
	m_pcsChannelRoom[i32ChannelNum]->Leave(); 

	return i32RoomIdx; 
}

/*-----------------------------------------------------------
Name : 
Desc : 방을 삭제할때는 여기 들어와야 한다. 
-----------------------------------------------------------*/
void CRoomManager::_DestroyRoom(CRoom * pRoom)
{
	INT32 i32ChannelNum = pRoom->m_Info._ui32ChannelNum;
	m_pcsChannelRoom[i32ChannelNum]->Enter();
	{
		m_pChannelRoomList[i32ChannelNum]->Remove( pRoom );
		pRoom->m_bIsCreate = FALSE;
		m_pChangeRoomListTime[i32ChannelNum] = i3ThreadTimer::GetServerTime();
	}
	m_pcsChannelRoom[i32ChannelNum]->Leave();
}


INT32 CRoomManager::OnQuickJoinRoom(CUserSession * pSession, INT32 i32Channel)
{
	// 빠른 입장 설정 - 기획 및 소스 수정 2011.08.01 - 서동권

	INT32 i32EnterRoomIdx = SERVER_VALUE_UNKNOWN; 
	ROOM_INFO_BASIC **	pRoomInfoList; 
	INT32			i32RoomCount;

	if(m_InfoSide[i32Channel] == 0)
	{
		pRoomInfoList = (ROOM_INFO_BASIC **)m_pRoomRoomInfo0[i32Channel]; 
		i32RoomCount = m_pRoomCount0[i32Channel]; 
	}
	else 
	{
		pRoomInfoList = (ROOM_INFO_BASIC **)m_pRoomRoomInfo1[i32Channel]; 
		i32RoomCount = m_pRoomCount1[i32Channel]; 
	} 

	//ROOM_INFO_BASIC * pRoomInfo; 
	CRoom * pRoom;

	// 만약 빠른입장 설정값이 없거나 설정값에 해당하는 방이 존재하지 않으면 대기중인 방 중에 인원이 가장 많은 꽉차지 않은 방에 들어갑니다.
	// 그러기 위해 내용을 저장할 변수들 입니다. - 서동권
	INT32 ui32PlanB_RoomIdx = SERVER_VALUE_UNKNOWN;

	//1. 대기중인 방을 찾는다. 
	for( UINT32 j = 0; j < m_ui32EachChannelMaxCount; ++j )
	{
		pRoom = (CRoom*)m_pChannelResRoomList[ i32Channel ]->GetItem(j);

		if( !pRoom->m_bIsCreate )																continue;	//생성된방

		if( GET_ROOM_INFO_PRIVATE_MODE( pRoom->m_Info._InfoBasic._InfoFlag ))					continue;	//비밀 방 
		if( pRoom->m_Info._InfoBasic._PersonNow == pRoom->m_Info._InfoBasic._PersonPlayerMax)	continue;	//유저 카운트
		if( pRoom->m_Info._InfoBasic._State != ROOM_STATE_READY)								continue;	//대기중인방


		i32EnterRoomIdx = pRoom->m_Info._InfoBasic._Idx; 

		if(i32EnterRoomIdx != SERVER_VALUE_UNKNOWN)		break; // 방을 찾았으면
	}

	// 설정값이 없거나 해당하는 방이 없으면 현재 대기중인 방 중에서 가장 인원수가 많고 빈자리가 있으며 대기중인 방에 접속을 시도합니다. - 서동권
	if(i32EnterRoomIdx == SERVER_VALUE_UNKNOWN && ui32PlanB_RoomIdx != SERVER_VALUE_UNKNOWN )		i32EnterRoomIdx = ui32PlanB_RoomIdx;

	if(i32EnterRoomIdx == SERVER_VALUE_UNKNOWN) return EVENT_ERROR_FAIL; 

	pRoom = (CRoom *)m_pChannelResRoomList[i32Channel]->GetItem(i32EnterRoomIdx);
	ROOM_INFO_JOIN	InfoJoin;
	InfoJoin.m_ui8EnterType = S2MO_ROOM_ENTER_QUICK_JOIN;

	return pRoom->Enter( pSession->m_WorkThreadIdx, pSession, &InfoJoin );
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoomManager::OnJoinRoom( INT32 i32ThreadIdx, CUserSession * pSession, INT32 i32ChannelNum, UINT32 ui32RoomIdx, ROOM_INFO_JOIN* pInfoJoin)
{
	if( ui32RoomIdx >= g_pContextMain->m_ui32ChannelSessionCount )
		return EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM;

	CRoom* pRoom = static_cast<CRoom*>( m_pChannelResRoomList[i32ChannelNum]->GetItem(ui32RoomIdx) );
	if( !pRoom )
		return EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM;

	if( pRoom->m_bIsCreate == FALSE )
		return EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM;

	if( g_pContextMain->CheckAASBlockON( AAS_BLOCK_JOIN_ROOM ) && pSession->CheckAAS() )
		return EVENT_ERROR_AAS_JOIN_ROOM;

	return pRoom->Enter(i32ThreadIdx, pSession, pInfoJoin); 
}


/*-----------------------------------------------------------
Name : 
Desc : 방에서 나가기 -- 메인쓰레드, 워킹쓰레드에서 호출 가능 
-----------------------------------------------------------*/
INT32 CRoomManager::OnLeaveRoom(CUserSession * pSession, INT32 i32ChannelNum, INT32 i32WorkThread, INT32 i32EndType )
{
	// Check Channel Idx
	if( 0 >	i32ChannelNum )																return EVENT_ERROR_FAIL;
	if( (INT32)m_ui32ChannelCount <= i32ChannelNum )									return EVENT_ERROR_FAIL;
	// Check Room Idx
	if( 0 > pSession->m_i32RoomIdx )												return EVENT_ERROR_FAIL;
	if( pSession->m_i32RoomIdx >= (INT32)g_pContextMain->m_ui32ChannelSessionCount )	return EVENT_ERROR_FAIL;

	CRoom * pRoom = (CRoom*)m_pChannelResRoomList[i32ChannelNum]->GetItem( pSession->m_i32RoomIdx );
	if( NULL == pRoom )																return EVENT_ERROR_FAIL;
	if( pRoom->LeaveLobby_MU(pSession, i32WorkThread, i32EndType ) == DESTROY_ROOM )
	{
		_DestroyRoom(pRoom);
	}

	if (ROOM_STATE_BATTLE == pRoom->GetRoomState() && pSession->GetMerPenaltyFlag())
	{	// 용병이 이탈한후 로비로 나갈때, 룸이 배틀중이라면 이탈 페널티를 받습니다. 기획의도.
		// 실제로 이탈 페널티를 부여하는 유일한 곳입니다.
		pSession->SetMerPenaltyTime(i32WorkThread);
	}

	return EVENT_ERROR_SUCCESS; 
}

void CRoomManager::Dump(void)
{
	return; 
}



// 빠른입장 방검색
INT32 CRoomManager::SearchQuickJoinRoom(CUserSession * pSession, QUICKJOIN_SEARCH_INFO* pInfo  )
{
	// 가독성을 높이기 위해!.
	STAGE_UID			eStageUID		= pInfo->m_eStageUID;	// 유저가 선택한 스테이지.
	UINT32				ui32Rule		= pInfo->m_ui32Rule;		// 유저가 선택한 스테이지 타입.
	
	// 클라이언트에서 보낸값이 이상한 경우. 해킹입니다.	
	if( eStageUID >= STAGE_UID_MAX // MAX 값 초과.
		|| ( STAGE_UID_NONE == eStageUID && 0 == ui32Rule ) ) // 아무값도 셋팅 안됨. 
	{
		return EVENT_ERROR_QUICK_JOIN_WRONG_INFO; 
	}

	bool bAllCondition = true;	// 유저가 스테이지,스테이지 타입/옵션 모든 조건을 선택했는지.
	if( STAGE_UID_NONE == eStageUID || 0 == ui32Rule )	
		bAllCondition = false;

	CRoom * pRoom;
	INT32 i32RoomIdx;
	
	// 1. 유저의 포지션이 로비이면 자신의 채널부터, 그외는 랜덤, 
	UINT32 ui32ChannelIdx;
	if( GAME_TASK_LOBBY == pSession->GetPosition() ) 
		ui32ChannelIdx = (UINT32)pSession->GetChannelNumber();
	else 
		ui32ChannelIdx = RAND_DK(m_ui32ChannelCount);	
	
	// 2. 채널 수 만큼 루프.
	for( UINT32 ChannelLoop = 0 ; ChannelLoop < m_ui32ChannelCount ; ++ChannelLoop, ++ui32ChannelIdx )
	{
		if( ui32ChannelIdx >= m_ui32ChannelCount ) 
			ui32ChannelIdx = 0;

		// 클랜 채널은 검색 대상에서 제외합니다.
		UINT8 ui8ChannelType =  g_pContextMain->m_pChannelType[ ui32ChannelIdx ];
		if( CHANNEL_TYPE_CLAN == ui8ChannelType || CHANNEL_TYPE_PCCAFE_CLAN == ui8ChannelType ) continue;
		
		pInfo->m_ui8ChannelType = ui8ChannelType;

		// 채널 입장조건에 만족하는지 체크.
		if( EV_FAILED( pSession->CheckEnterChannel( ui32ChannelIdx ) ) ) continue;
		
		// 검색 시작할 방 인덱스를 랜덤하게 고릅니다.
		i32RoomIdx = RAND_DK(m_ui32EachChannelMaxCount);

		// 3. 해당 채널의 방 수 만큼 루프.
		for( UINT32 RoomLoop = 0; RoomLoop < m_ui32EachChannelMaxCount; ++RoomLoop, ++i32RoomIdx )
		{
			if( (UINT32)i32RoomIdx >= m_ui32EachChannelMaxCount ) 
				i32RoomIdx = 0;

			pRoom = (CRoom*)m_pChannelResRoomList[ ui32ChannelIdx ]->GetItem(i32RoomIdx);

			if( NULL == pRoom )											continue;	
			if( !pRoom->m_bIsCreate )									continue;	//생성되지 않은방
			if( ROOM_STATE_EMPTY == pRoom->m_Info._InfoBasic._State )	continue;	//깨진방

			//if( pRoom->m_Info._InfoBasic._PersonPlayerMax != 16 )		continue;	// minsik : PBBUG-15643 해당 이슈로 인해 기획 변경.

			if( GET_ROOM_INFO_PRIVATE_MODE( pRoom->m_Info._InfoBasic._InfoFlag ))					continue;	// 비밀 방 
			if( pRoom->m_Info._InfoBasic._PersonNow == pRoom->m_Info._InfoBasic._PersonPlayerMax)	continue;	// 꽉찬 방 
			if( 1 == pRoom->m_Info._InfoAdd._LockInterEnter )										continue;	// 난입 불가방

			STAGE_UID	eRStageUID		= pRoom->m_StageUID;
			UINT32		ui32RoomRule	= StageID::GetRule( pRoom->m_Info._InfoBasic._StageID );

			// 챌린지(AI) 방은 STAGE_OPTION이 일치하지 않으면 continue.
			//if (STAGE_MASK_TYPE_CHALLENGE == pRoom->m_Info._InfoBasic._StageMask && STAGE_MASK_TYPE_CHALLENGE != eUStageOption) continue;

			// 조건 선택에 상관없이 스테이지, 스테이지 타입이 일치하지 않으면 continue.( 스테이지 옵션만 오는 경우는 없다. )
			if( eStageUID != eRStageUID && ui32Rule != ui32RoomRule )  continue;

			// 모든 조건 선택시 일치하면 바로 함수 종료.
			if( bAllCondition && eStageUID == eRStageUID && ui32Rule == ui32RoomRule && g_CheckStageOptionWithRoom( &pRoom->m_Info._InfoBasic ) )
			{
				pInfo->m_i32ChannelIdx	= ui32ChannelIdx;
				pInfo->m_i32RoomIdx		= i32RoomIdx;
				pInfo->m_ui8RoomState	= pRoom->m_Info._InfoBasic._State;
				return EVENT_ERROR_SUCCESS;
			}

			if( !bAllCondition )// 한가지 조건만 선택한 경우
			{
				// 스테이지 체크.
				if( eStageUID != STAGE_UID_NONE && eStageUID != eRStageUID ) continue;

				// 스테이지 타입 체크.
				if( ui32Rule != 0 && ui32Rule != ui32RoomRule )  continue;

				// 스테이지 옵션체크 
				if( FALSE == g_CheckStageOptionWithRoom( &pRoom->m_Info._InfoBasic ) ) continue;

				// 차선책 방 정보 담고 함수 종료.
				pInfo->m_i32ChannelIdx	= ui32ChannelIdx;
				pInfo->m_i32RoomIdx		= i32RoomIdx;
				pInfo->m_ui8RoomState	= pRoom->m_Info._InfoBasic._State;
				pInfo->m_RoomInfo2ndBest.m_QuickJoin.m_ui32StageID = ui32Rule + static_cast<UINT32>(eStageUID);
				pRoom->GetRoomNowScore( &pInfo->m_RoomInfo2ndBest.m_NowScore );

				return EVENT_ERROR_QUICK_JOIN_2ND_BEST;

			}
		
			// 모든 조건 선택한 경우.
			// 루프 다 돌때 까지 모든 조건 일치 하는 방을 못찾으면 가장 마지막 차선책 방 정보를 가집니다.
			pInfo->m_i32ChannelIdx	= ui32ChannelIdx;
			pInfo->m_i32RoomIdx		= i32RoomIdx;
			pInfo->m_ui8RoomState	= pRoom->m_Info._InfoBasic._State;
			pInfo->m_RoomInfo2ndBest.m_QuickJoin.m_ui32StageID = ui32Rule + static_cast<UINT32>(eStageUID);
			pRoom->GetRoomNowScore( &pInfo->m_RoomInfo2ndBest.m_NowScore );

		}
	}

	// 방 검색 실패.
	if( QUICK_JOIN_SEARCH_FAIL_IDX == pInfo->m_i32ChannelIdx ) 
		return EVENT_ERROR_QUICK_JOIN_FAIL_SEARCH_ROOM;

	return EVENT_ERROR_QUICK_JOIN_2ND_BEST;
}