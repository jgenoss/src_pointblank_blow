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
	m_i32UseRoomCount			= 0;
	m_ui32ChannelCount			= 0;		
	m_ui32EachChannelMaxCount	= 0; 
	m_ppChannelRoomList			= NULL;			
	m_ppcsChannelRoom			= NULL;		
	m_ppChannelResRoomList		= NULL; 
	m_i32AccRoomCount			= 0; 
	m_pi32InfoSide				= NULL;
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
	m_ppChannelResRoomList = (i3List **) i3MemAlloc( sizeof( i3List * )  * m_ui32ChannelCount ); 
	CRoom * pRoom; 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++ )
	{
		m_ppChannelResRoomList[i] = i3List::NewObject(); 
		m_ppChannelResRoomList[i]->SetOptmizeEnableWhenClear(FALSE); 
		m_ppChannelResRoomList[i]->SetAllocationUnit(m_ui32EachChannelMaxCount);
		for(UINT32 j = 0; j < m_ui32EachChannelMaxCount; j++)
		{
			pRoom  = CRoom::NewObject(); 			
			pRoom->OnCreate( i, j );
			m_ppChannelResRoomList[i]->Add( pRoom );
		}
	}

	//Create Change RoomBuffer
	m_pui32ChangeRoomListTime = (UINT32 *) i3MemAlloc( sizeof(UINT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_pui32ChangeRoomListTime, sizeof(UINT32) * m_ui32ChannelCount ); 
	m_pui32RoomListUpdateTime = (UINT32 *) i3MemAlloc( sizeof(UINT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_pui32RoomListUpdateTime, sizeof(UINT32) * m_ui32ChannelCount ); 


	//////////////////////////////////////////////////////////////////////////////////////////
	//Create Channel 	
	m_ppChannelRoomList	= (i3List**)  i3MemAlloc( sizeof( i3List * ) * m_ui32ChannelCount ); 
	m_ppcsChannelRoom	= (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_ppChannelRoomList[i]	= i3List::NewObject();		
		m_ppChannelRoomList[i]->SetOptmizeEnableWhenClear(FALSE); 
		m_ppChannelRoomList[i]->SetAllocationUnit(m_ui32EachChannelMaxCount); 
		m_ppcsChannelRoom[i]		= i3Mutex::NewObject(); 
	}	

	//////////////////////////////////////////////////////////////////////////////////////////
	//Create RoomInfo 
	//Side 0
	m_ppRoomRoomInfo0	= (ROOM_INFO_BASIC **)	i3MemAlloc( sizeof( ROOM_INFO_BASIC *) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++) 
	{
		m_ppRoomRoomInfo0[i] = (ROOM_INFO_BASIC *)i3MemAlloc( sizeof( ROOM_INFO_BASIC *) * m_ui32EachChannelMaxCount ); 
	}	
	m_pui32RoomCount0		= (UINT32 *)i3MemAlloc( sizeof( UINT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_pui32RoomCount0, sizeof( UINT32) * m_ui32ChannelCount ); 

	//Side 1 
	m_ppRoomRoomInfo1	= (ROOM_INFO_BASIC **)	i3MemAlloc( sizeof( ROOM_INFO_BASIC *) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++) 
	{
		m_ppRoomRoomInfo1[i] = (ROOM_INFO_BASIC *)i3MemAlloc( sizeof( ROOM_INFO_BASIC *) * m_ui32EachChannelMaxCount ); 
	}
	m_pui32RoomCount1		= (UINT32 *)i3MemAlloc( sizeof( UINT32) * m_ui32ChannelCount );
	i3mem::FillZero( m_pui32RoomCount1, sizeof( UINT32) * m_ui32ChannelCount );

	m_pi32InfoSide = (INT32 *)i3MemAlloc( sizeof( INT32) * m_ui32ChannelCount ); 
	i3mem::FillZero( m_pi32InfoSide, sizeof( INT32) * m_ui32ChannelCount ); 

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoomManager::OnDestroy(void)
{	
	I3MEM_SAFE_FREE( m_pui32RoomListUpdateTime );
	I3MEM_SAFE_FREE( m_pui32ChangeRoomListTime );

	//For Channel 
	for( UINT32 i =0 ; i < m_ui32ChannelCount; i++)
	{
		I3_SAFE_RELEASE( m_ppChannelRoomList[i] ); 
		I3_SAFE_RELEASE( m_ppcsChannelRoom[i] ); 
	}
	I3MEM_SAFE_FREE( m_ppChannelRoomList ); 	
	I3MEM_SAFE_FREE( m_ppcsChannelRoom ); 	

	//For Resource
	CRoom * pRoom;
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		for(UINT32 j = 0; j < m_ui32EachChannelMaxCount; j++)
		{
			pRoom = (CRoom * ) m_ppChannelResRoomList[i]->GetItem(j); 
			I3_SAFE_RELEASE( pRoom ); 
		}
		I3_SAFE_RELEASE( m_ppChannelResRoomList[i] ); 
	}
	I3MEM_SAFE_FREE( m_ppChannelResRoomList ); 

	//For RoomInfo 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++) 
	{
		I3MEM_SAFE_FREE( m_ppRoomRoomInfo0[i] );
		I3MEM_SAFE_FREE( m_ppRoomRoomInfo1[i] );

	}

	I3MEM_SAFE_FREE( m_ppRoomRoomInfo0 );
	I3MEM_SAFE_FREE( m_pui32RoomCount0 ); 

	I3MEM_SAFE_FREE( m_ppRoomRoomInfo1 ); 
	I3MEM_SAFE_FREE( m_pui32RoomCount1 );
	I3MEM_SAFE_FREE( m_pi32InfoSide ); 

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoomManager::OnSendRoomList(CUserSession * pSession, INT32 i32Channel, LOBBY_USER_INFO* pLobbyUserInfo, UINT32 ui32UserCount )
{		
	ROOM_INFO_BASIC **	pRoomInfo;
	UINT32				ui32RoomCount;
	UINT32				ui32PageNum = pSession->m_ui32UpdateRoomListPage;
	INT32				i32UserPageNum = pSession->m_ui32UpdateUserListPage;

	if(i32Channel == SERVER_VALUE_UNKNOWN)return; 

	if(m_pi32InfoSide[i32Channel] == 0)
	{
		pRoomInfo = (ROOM_INFO_BASIC **)m_ppRoomRoomInfo0[i32Channel];
		ui32RoomCount = m_pui32RoomCount0[i32Channel];
	}
	else 
	{
		pRoomInfo = (ROOM_INFO_BASIC **)m_ppRoomRoomInfo1[i32Channel]; 
		ui32RoomCount = m_pui32RoomCount1[i32Channel]; 
	}

	UINT32 ui32PageCount = (UINT32)(ui32RoomCount / SEND_MAX_ROOM_COUNT);

	if( (ui32RoomCount % SEND_MAX_ROOM_COUNT) > 0) ui32PageCount++; 
	if( ui32PageNum > ui32RoomCount ) ui32PageNum = 0;

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
	if( i32UserPageNum > (INT32)ui32UserCount ) i32UserPageNum = 0;

	ui32StartIdx	= i32UserPageNum * SEND_MAX_LOBBY_USER_COUNT; 
	ui32EndIdx		= ui32StartIdx + SEND_MAX_LOBBY_USER_COUNT; 		
	if(ui32EndIdx > ui32UserCount)		ui32EndIdx = ui32UserCount; 
	UINT32 ui32PacketUserCount = ui32EndIdx - ui32StartIdx; 
	
	RoomListPacket.WriteData( &ui32UserCount,		sizeof(UINT32));
	RoomListPacket.WriteData( &i32UserPageNum,		sizeof(INT32));
	RoomListPacket.WriteData( &ui32PacketUserCount, sizeof(INT32));

	CUserSession * pSessionLobby;
	for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
	{
		pSessionLobby = (CUserSession*)pLobbyUserInfo[ i ].pSession;

		RoomListPacket.WriteData( &pSessionLobby->m_SessionIdx,					sizeof(INT32) );
		RoomListPacket.WriteData( &pSessionLobby->m_UserInfoBasic._clanMark,	sizeof(UINT32));
		RoomListPacket.WriteData( pSessionLobby->m_UserInfoBasic._clanName,		NET_CLAN_NAME_SIZE);
		RoomListPacket.WriteData( &pSessionLobby->m_UserInfoBasic._rank,		sizeof(UINT8));
		RoomListPacket.WriteData( &pSessionLobby->m_UserInfoBasic._eSportID,	sizeof(UINT8));
		RoomListPacket.WriteData( pSessionLobby->m_UserInfoBasic._nick,			NET_NICK_NAME_SIZE );
		RoomListPacket.WriteData( &pSessionLobby->m_UserInfoBasic._nickcolor,	sizeof(UINT8) );
		RoomListPacket.WriteData( &pSessionLobby->m_ui8OtherCountry,			sizeof(UINT8));
	}

	// 현재 PacketSize 는 1053 입니다. 20110819 김현우
	pSession->SendPacketMessage( &RoomListPacket );

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
	INT32			i32InsertInfoSide		= 0;	
	ROOM_INFO_BASIC	**	ppInsertRoomInfo	= NULL;
	UINT32		*	pui32InsertRoomCount	= NULL;
	UINT32			ui32NowTime				= g_pContextMain->GetServerTime(); 	
	BOOL			bUpdateRoomList			= FALSE;

	//방이 변경되었는지 확인을 합니다. 
	if( (m_pui32ChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel] > m_pui32RoomListUpdateTime[g_pContextMain->m_ui32UpdateChannel]) 
		||	(m_ppChannelRoomList[g_pContextMain->m_ui32UpdateChannel]->GetCount() != GetChannelUseRoomCount(g_pContextMain->m_ui32UpdateChannel)) )
	{ 
		m_pui32RoomListUpdateTime[g_pContextMain->m_ui32UpdateChannel] = m_pui32ChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel]; 
		bUpdateRoomList = TRUE;
	}

	//땜빵입니다. //5초마다 리스트 업데이트 합니다. 
	if( (bUpdateRoomList == FALSE) && ( (m_pui32ChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel] + 5) < ui32NowTime) )
	{
		m_pui32RoomListUpdateTime[g_pContextMain->m_ui32UpdateChannel] = m_pui32ChangeRoomListTime[g_pContextMain->m_ui32UpdateChannel] = ui32NowTime; 
		bUpdateRoomList = TRUE;
	}

	//Pre Update RoomList 
	if(bUpdateRoomList)
	{
		if(m_pi32InfoSide[g_pContextMain->m_ui32UpdateChannel] == 0)i32InsertInfoSide = 1; 
		if(i32InsertInfoSide == 0)
		{
			ppInsertRoomInfo		= (ROOM_INFO_BASIC **)m_ppRoomRoomInfo0[g_pContextMain->m_ui32UpdateChannel];
			pui32InsertRoomCount	= &m_pui32RoomCount0[g_pContextMain->m_ui32UpdateChannel]; 
		}
		else 
		{
			ppInsertRoomInfo		= (ROOM_INFO_BASIC **)m_ppRoomRoomInfo1[g_pContextMain->m_ui32UpdateChannel];
			pui32InsertRoomCount	= &m_pui32RoomCount1[g_pContextMain->m_ui32UpdateChannel];
		}
	}

	//Update Room And AddList
	i32UseRoomCount = 0; 
	for(UINT32 i = 0; i < m_ui32EachChannelMaxCount; i++)
	{
		pRoom = (CRoom*)m_ppChannelResRoomList[ g_pContextMain->m_ui32UpdateChannel ]->GetItem(i);
		if( !pRoom->m_bIsCreate )continue;

		pRoom->OnUpdateRoom_M(ui32NowTime);	//업데이트만 합니다. 리턴값에 따라 방을 삭제하는 부분은 필요없음.

		//투토리얼 모드는 패스 합니다. 리스트에 포함하지 않음.
		if ( GET_STAGETYPE( pRoom->m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL )continue; 

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
		m_pi32InfoSide[g_pContextMain->m_ui32UpdateChannel] = i32InsertInfoSide; 
	}
}


/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoomManager::OnCreateRoom( CUserSession * pSession, ROOM_INFO_CREATE* pInfoCreate )
{
	INT32		i32RoomIdx		= EVENT_ERROR_FAIL; 
	INT32		i32ChannelNum	= pInfoCreate->nChannelNum;

	m_ppcsChannelRoom[i32ChannelNum]->Enter();
	{
		//Find Empty Room 
		CRoom * pRoom = NULL;
		for(UINT32 i = 0; i < m_ui32EachChannelMaxCount; i++)
		{
			pRoom = (CRoom*)m_ppChannelResRoomList[i32ChannelNum]->GetItem(i);
			if( pRoom->m_bIsCreate ) continue;;

			if( FALSE == pRoom->CreateRoom(pSession, pInfoCreate ) ) break;

			pRoom->m_bIsCreate = TRUE;
			i32RoomIdx = pRoom->m_Info._InfoBasic._Idx;

			break; 
		}

		if(i32RoomIdx != EVENT_ERROR_FAIL)
		{
			//Add Room 
			if(m_ppChannelRoomList[i32ChannelNum]->GetCount() < (INT32)m_ui32EachChannelMaxCount)
			{
				m_ppChannelRoomList[i32ChannelNum]->Add( pRoom );
			}
		}

		m_pui32ChangeRoomListTime[i32ChannelNum] = g_pContextMain->GetServerTime(); 
	}
	m_ppcsChannelRoom[i32ChannelNum]->Leave(); 

	return i32RoomIdx; 
}

/*-----------------------------------------------------------
Name : 
Desc : 방을 삭제할때는 여기 들어와야 한다. 
-----------------------------------------------------------*/
void CRoomManager::_DestroyRoom(CRoom * pRoom)
{
	INT32 i32ChannelNum = pRoom->m_Info._ui32ChannelNum;
	m_ppcsChannelRoom[i32ChannelNum]->Enter();
	{		
		m_ppChannelRoomList[i32ChannelNum]->Remove( pRoom );
		pRoom->m_bIsCreate = FALSE;
		m_pui32ChangeRoomListTime[i32ChannelNum] = g_pContextMain->GetServerTime(); 
	}
	m_ppcsChannelRoom[i32ChannelNum]->Leave();
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/

BOOL CRoomManager::CheckRoomMode( UINT8 ui8Mode, CRoom * pRoom )
{
	// 스테이지 모드와 스테이지 타입을 한번에 확인 합니다. 빠른입장 설정에서는 모드와 타입을 하나의 모드로 보기 때문에...
	// 서동권 2011.07.30
	switch( ui8Mode )
	{
	case STAGE_MODE_DEATHMATCH :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_DEATHMATCH )				return TRUE;
		}
		break;
	case STAGE_MODE_BOMB :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_BOMB )					return TRUE;
		}
		break;
	case STAGE_MODE_DESTROY :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_DESTROY )					return TRUE;
		}
		break;
	case STAGE_MODE_DEFENCE :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_DEFENCE )					return TRUE;
		}
		break;
	case STAGE_MODE_DINO :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_ESCAPE )					return TRUE;
		}
		break;
	case STAGE_MODE_ANNIHILATION :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_ANNIHILATION )			return TRUE;
		}
		break;
	case STAGE_MODE_DOMINATION :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_DOMINATION )				return TRUE;
		}
		break;
	case STAGE_MODE_KNUCKLE :
		{
			if( pRoom->m_Info._InfoBasic._StageMask == STAGE_MASK_TYPE_KNUCKLE )	return TRUE;
		}
		break;
	case STAGE_MODE_SHOTGUN :
		{
			if( pRoom->m_Info._InfoBasic._StageMask == STAGE_MASK_TYPE_SHOTGUN )	return TRUE;
		}
		break;
	case STAGE_MODE_SUDDENDEATH :
		{
			if( pRoom->m_Info._i32StageType == STAGE_TYPE_SUDDEN_DEATH )			return TRUE;
		}
		break;
	case STAGE_MODE_HEADHUNTER :
		{
			if( pRoom->m_Info._InfoBasic._StageMask == STAGE_MASK_TYPE_HEADHUNTER ) return TRUE;
		}
		break;
	case STAGE_MODE_HEADKILLER :
		{
			if( pRoom->m_Info._InfoBasic._StageMask == STAGE_MASK_TYPE_HEADHUNTER ) return TRUE;
		}
		break;
	case STAGE_MODE_AI :
		{
			if( pRoom->m_Info._InfoBasic._StageMask == STAGE_MASK_TYPE_TRAINING )	return TRUE;
		}
		break;
	case STAGE_MODE_CLANMATCH :
		{
			if( pRoom->m_Info._InfoBasic._StageMask == STAGE_MASK_TYPE_CLANMATCH )	return TRUE;
		}
		break;
	case STAGE_MODE_ALL :
		{
			return TRUE;
		}
		break;
	default :
		{
			return FALSE;
		}
		break;
	}

	return FALSE;
}

INT32 CRoomManager::OnQuickJoinRoom(CUserSession * pSession, INT32 i32Channel)
{
	// 빠른 입장 설정 - 기획 및 소스 수정 2011.08.01 - 서동권

	INT32 i32EnterRoomIdx = SERVER_VALUE_UNKNOWN; 
	ROOM_INFO_BASIC **	ppRoomInfoList; 
	INT32			RoomCount;

	if(m_pi32InfoSide[i32Channel] == 0)
	{
		ppRoomInfoList = (ROOM_INFO_BASIC **)m_ppRoomRoomInfo0[i32Channel]; 
		RoomCount = m_pui32RoomCount0[i32Channel]; 
	}
	else 
	{
		ppRoomInfoList = (ROOM_INFO_BASIC **)m_ppRoomRoomInfo1[i32Channel]; 
		RoomCount = m_pui32RoomCount1[i32Channel]; 
	} 

	//ROOM_INFO_BASIC * pRoomInfo; 
	CRoom * pRoom;

	// 만약 빠른입장 설정값이 없거나 설정값에 해당하는 방이 존재하지 않으면 대기중인 방 중에 인원이 가장 많은 꽉차지 않은 방에 들어갑니다.
	// 그러기 위해 내용을 저장할 변수들 입니다. - 서동권
	UINT8  ui8RoomPlayer = 0;
	INT32 i32PlanB_RoomIdx = SERVER_VALUE_UNKNOWN;

	
#if defined( USE_QUICK_JOIN )
	for( UINT8 i = 0; i < MAX_QUICK_INFO; ++i )
	{
#endif

		//1. 대기중인 방을 찾는다. 
		for( UINT32 j = 0; j < m_ui32EachChannelMaxCount; ++j )
		{
			pRoom = (CRoom*)m_ppChannelResRoomList[ i32Channel ]->GetItem(j);

			if( !pRoom->m_bIsCreate )																continue;	//생성된방

			if( GET_ROOM_INFO_PRIVATE_MODE( pRoom->m_Info._InfoBasic._InfoFlag ))					continue;	//비밀 방 
#ifdef DEF_OBSERVER_MODE
			if( pRoom->m_Info._InfoBasic._PersonNow == pRoom->m_Info._InfoBasic._PersonPlayerMax)	continue;	//유저 카운트
#else
			if( pRoom->m_Info._InfoBasic._PersonNow == pRoom->m_Info._InfoBasic._PersonPlayerMax)	continue;	//유저 카운트
#endif
			if( pRoom->m_Info._InfoBasic._State != ROOM_STATE_READY)								continue;	//대기중인방

#if defined( USE_QUICK_JOIN )
			if( pRoom->m_Info._InfoBasic._PersonNow > ui8RoomPlayer )
			{   // 혹시 빠른 입장 설정값에 만족하는 결과가 없는것을 대비하여 차선책을 준비 합니다.
				ui8RoomPlayer = pRoom->m_Info._InfoBasic._PersonNow;
				i32PlanB_RoomIdx = pRoom->m_Info._InfoBasic._Idx;
			}

			if( !CheckRoomMode( pSession->m_stQuickInfo._ui8Mode[ i ], pRoom ) )				continue;	// 빠른입장 설정 모드와 다를경우

			// 맵 설정이 STAGE_ORD_NA 일 경우는 모든맵으로 처리
			if( pSession->m_stQuickInfo._ui8Map[ i ] != STAGE_ORD_NA && pSession->m_stQuickInfo._ui8Map[ i ] != g_GetStageID_ForMission( GET_STAGEORD( pRoom->m_Info._InfoBasic._StageID ) ) )	continue;	// 빠른입장 설정 맵과 다를경우
#endif
			i32EnterRoomIdx = pRoom->m_Info._InfoBasic._Idx; 
			
#if defined( USE_QUICK_JOIN )
			break; 
		}
#endif

		if(i32EnterRoomIdx != SERVER_VALUE_UNKNOWN)		break; // 방을 찾았으면
	}

	// 설정값이 없거나 해당하는 방이 없으면 현재 대기중인 방 중에서 가장 인원수가 많고 빈자리가 있으며 대기중인 방에 접속을 시도합니다. - 서동권
	if(i32EnterRoomIdx == SERVER_VALUE_UNKNOWN && i32PlanB_RoomIdx != SERVER_VALUE_UNKNOWN )		i32EnterRoomIdx = i32PlanB_RoomIdx;

	if(i32EnterRoomIdx == SERVER_VALUE_UNKNOWN) return EVENT_ERROR_FAIL; 

	pRoom = (CRoom *)m_ppChannelResRoomList[i32Channel]->GetItem(i32EnterRoomIdx);
	ROOM_INFO_JOIN	InfoJoin;
	InfoJoin.Reset();

	return pRoom->Enter( pSession, &InfoJoin );
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoomManager::OnJoinRoom(CUserSession * pSession, INT32 i32ChannelNum, UINT32 ui32RoomIdx, ROOM_INFO_JOIN* pInfoJoin)
{
	if( ui32RoomIdx >= g_pContextMain->m_ui32ChannelSessionCount )	return EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM;

	CRoom * pRoom = (CRoom *)m_ppChannelResRoomList[ i32ChannelNum ]->GetItem( ui32RoomIdx );
	if( NULL == pRoom )											return EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM;
	if( FALSE == pRoom->m_bIsCreate )							return EVENT_ERROR_EVENT_JOIN_ROOM_NOT_EXIST_ROOM;

	return pRoom->Enter( pSession, pInfoJoin ); 
}


/*-----------------------------------------------------------
Name : 
Desc : 방에서 나가기 -- 메인쓰레드, 워킹쓰레드에서 호출 가능 
-----------------------------------------------------------*/
INT32 CRoomManager::OnLeaveRoom(CUserSession * pSession, INT32 i32ChannelNum, INT32 i32WorkThread, INT32 i32EndType )
{
	// Check Channel Idx
	if( 0 >	i32ChannelNum )															return EVENT_ERROR_FAIL;
	if( (INT32)m_ui32ChannelCount <= i32ChannelNum )								return EVENT_ERROR_FAIL;
	// Check Room Idx
	if( 0 > pSession->m_i32RoomIdx )												return EVENT_ERROR_FAIL;
	if( pSession->m_i32RoomIdx >= (INT32)g_pContextMain->m_ui32ChannelSessionCount )	return EVENT_ERROR_FAIL;

	CRoom * pRoom = (CRoom*)m_ppChannelResRoomList[i32ChannelNum]->GetItem( pSession->m_i32RoomIdx );
	if( NULL == pRoom )																return EVENT_ERROR_FAIL;
	if( pRoom->LeaveLobby_MU(pSession, i32WorkThread, i32EndType ) == DESTROY_ROOM )
	{
		_DestroyRoom(pRoom);
	}

	return EVENT_ERROR_SUCCESS; 
}

void CRoomManager::Dump(void)
{
	return; 
}
