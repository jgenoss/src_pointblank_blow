#include "pch.h"
#include "UserManager.h"
#include "RoomManager.h"
#include "Room.h"
#include "DataBase.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"

CUserSessionManager * g_pUserManager = NULL; 

I3_CLASS_INSTANCE( CUserSessionManager, i3NetworkSessionManager);

CUserSessionManager::CUserSessionManager()
{
	//채널 유저 
	m_pChannelUserMaxFlag	= NULL; 
	m_ppChannelUserList		= NULL; 
	m_ppcsChanneUser		= NULL;

	//로비 유저 
	m_ppLobbyUserList		= NULL; 
	m_ppcsLobbyUser			= NULL; 

	//채팅 
	m_ppcsLobbyChatting		= NULL;	
	m_pi32LobbyUserInfoSide	= NULL;
	m_ppLobbyUserList0		= NULL;
	m_pui32LobbyUserCount0	= NULL;
	m_ppLobbyUserList1		= NULL;
	m_pui32LobbyUserCount1	= NULL;
    
	//Packet 
	m_i32UserListPacketNum	= 0; 	

	m_ui32ChannelCount		= 0; 
	m_ui32EachChannelMaxCount	= 0; 
	m_i32TimeLogInUser		= 0; 

	m_i32SendAliveUserIdx	= 0; 
	m_i32SendAliveUserCycle	= 0;

	m_i32ActiveCount		= 0;

	m_i32LastUpdate			= -1;

	i3mem::FillZero( m_aui32LinkLoginCount, sizeof( m_aui32LinkLoginCount ) );
	i3mem::FillZero( m_aui32LinkLoginTimeArg, sizeof( m_aui32LinkLoginTimeArg ) );
}

CUserSessionManager::~CUserSessionManager()
{
	OnDestroy(); 
}

BOOL CUserSessionManager::OnCreate(i3List * pSessionList)
{
	//Set Value 
	m_i32SessionCheckIdx		= 0;
	m_ui32ChannelCount			= GAME_CHANNEL_COUNT; 
	m_ui32EachChannelMaxCount	= g_pContextMain->m_ui32ChannelSessionCount; 
	m_ui32ChannelCountPer		= (UINT32)(m_ui32EachChannelMaxCount * 95 * 0.01f);	// 한번 Full이 찬 상태에서 95%이하 전에는 초기화가 안됩니다.

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Channel User 
	m_pChannelUserMaxFlag	= (BOOL*)i3MemAlloc( sizeof( BOOL ) * m_ui32ChannelCount ); 
	m_ppChannelUserList		= (i3List**)  i3MemAlloc( sizeof( i3List * ) * m_ui32ChannelCount ); 
	m_ppcsChanneUser		= (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_ppChannelUserList[i]		= i3List::NewObject();		
		m_ppChannelUserList[i]->SetOptmizeEnableWhenClear(FALSE); 
		m_ppChannelUserList[i]->SetAllocationUnit( m_ui32EachChannelMaxCount ); 
		m_ppcsChanneUser[i]			= i3Mutex::NewObject(); 
		m_pChannelUserMaxFlag[i]	= FALSE; 
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Lobby User
	m_ppLobbyUserList	= (i3List**)  i3MemAlloc( sizeof( i3List * ) * m_ui32ChannelCount );
	m_ppcsLobbyUser		= (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_ppLobbyUserList[i]= i3List::NewObject();		
		m_ppLobbyUserList[i]->SetOptmizeEnableWhenClear(FALSE); 
		m_ppLobbyUserList[i]->SetAllocationUnit( m_ui32EachChannelMaxCount ); 
		m_ppcsLobbyUser[i]	= i3Mutex::NewObject(); 
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Lobby User
	m_ppcsLobbyChatting = (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)m_ppcsLobbyChatting[i] = i3Mutex::NewObject(); 

	m_pi32LobbyUserInfoSide = (INT32*) i3MemAlloc(sizeof(INT32) * m_ui32ChannelCount); 
	i3mem::FillZero( m_pi32LobbyUserInfoSide, (sizeof(INT32) * m_ui32ChannelCount) ); 

	m_ppLobbyUserList0 = (LOBBY_USER_INFO **) i3MemAlloc(sizeof(LOBBY_USER_INFO*) * m_ui32ChannelCount ); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_ppLobbyUserList0[i] = (LOBBY_USER_INFO *) i3MemAlloc(sizeof(LOBBY_USER_INFO) * m_ui32EachChannelMaxCount); 
	}
	m_pui32LobbyUserCount0 = (UINT32*) i3MemAlloc(sizeof(UINT32) * m_ui32ChannelCount); 
	i3mem::FillZero( m_pui32LobbyUserCount0, (sizeof(UINT32) * m_ui32ChannelCount) ); 

	m_ppLobbyUserList1 = (LOBBY_USER_INFO **) i3MemAlloc(sizeof(LOBBY_USER_INFO *) * m_ui32ChannelCount); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_ppLobbyUserList1[i] = (LOBBY_USER_INFO *) i3MemAlloc(sizeof(LOBBY_USER_INFO) * m_ui32EachChannelMaxCount); 
	}
	m_pui32LobbyUserCount1 = (UINT32*) i3MemAlloc(sizeof(UINT32) * m_ui32ChannelCount);
	i3mem::FillZero( m_pui32LobbyUserCount1, (sizeof(UINT32) * m_ui32ChannelCount) ); 

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Channel Packet
	_UpdateUserListPacket(); 	

	switch( g_pContextMain->m_i32GameGuard )
	{
		case GAMEGUARD_NPROTECT:		m_ui32SessionGameGuardUpdateTime = 180;		break;	// 3분 마다 업데이트;		
		case GAMEGUARD_HACKSHIELD:		m_ui32SessionGameGuardUpdateTime = 60;		break;	// 1분 마다 업데이트;		
		case GAMEGUARD_XTRAP:			m_ui32SessionGameGuardUpdateTime = 20;		break;	// 20초 마다 업데이트;		
		case GAMEGUARD_GPK:				m_ui32SessionGameGuardUpdateTime = 120;		break;	// 2분 마다 업데이트;		
		default:						m_ui32SessionGameGuardUpdateTime = 60;		break;	// 1분 마다 업데이트;		
	}

	// announce buffer
	m_pRingAnnounceMessage = new NSM_CRingBuffer;
	if( m_pRingAnnounceMessage ) m_pRingAnnounceMessage->OnCreate( sizeof(ANNOUNCE_MESSAGE), ANNOUNCE_MESSAGE_BUFFER_COUNT*2 );
	m_AnnounceMessage.Clear();
	m_bSendAnnounce = FALSE;
	m_i32AnnouncedUserIdx = g_pContextMain->m_i32SessionCount;
	m_ui32AnnounceLastSend = 0;

	m_pRingAnnounceMain = new NSM_CRingBuffer;
	if( m_pRingAnnounceMain ) m_pRingAnnounceMain->OnCreate( sizeof(ANNOUNCE_MESSAGE), ANNOUNCE_MESSAGE_BUFFER_COUNT );

	m_pRingAnnounceControl = new NSM_CRingBuffer;
	if( m_pRingAnnounceControl ) m_pRingAnnounceControl->OnCreate( sizeof(ANNOUNCE_MESSAGE), ANNOUNCE_MESSAGE_BUFFER_COUNT );

	m_pRingAnnounceTrans = new NSM_CRingBuffer;
	if( m_pRingAnnounceTrans ) m_pRingAnnounceTrans->OnCreate( sizeof(ANNOUNCE_MESSAGE), ANNOUNCE_MESSAGE_BUFFER_COUNT );

	// 패킷 처리 테스트코드
	m_dwParseTimeHigh = 0;
	m_ui16ParseTimeHighProtocol = 0;
	m_bParseTimeReset = FALSE;

	return i3NetworkSessionManager::OnCreate( pSessionList ); 
}

BOOL CUserSessionManager::OnDestroy(void)
{
	////////////////////////////////////////////////////////
	//Release Each Channel List
	for( UINT32 i =0 ; i < m_ui32ChannelCount; i++)
	{
		I3_SAFE_RELEASE( m_ppChannelUserList[i] ); 
		I3_SAFE_RELEASE( m_ppcsChanneUser[i] ); 
	}
	I3MEM_SAFE_FREE( m_ppChannelUserList ); 	
	I3MEM_SAFE_FREE( m_ppcsChanneUser ); 
	I3MEM_SAFE_FREE( m_pChannelUserMaxFlag );


	////////////////////////////////////////////////////////
	//Release Each Lobby List 
	for( UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		I3_SAFE_RELEASE( m_ppLobbyUserList[i] ); 
		I3_SAFE_RELEASE( m_ppcsLobbyUser[i] ); 
	}
	I3MEM_SAFE_FREE( m_ppLobbyUserList ); 
	I3MEM_SAFE_FREE( m_ppcsLobbyUser ); 

	////////////////////////////////////////////////////////
	//Chatting Lobby User List 
	for( UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppLobbyUserList0[i] ); 
		I3MEM_SAFE_FREE( m_ppLobbyUserList1[i] ); 
		I3_SAFE_RELEASE( m_ppcsLobbyChatting[i] ); 
	}
	I3MEM_SAFE_FREE( m_ppLobbyUserList0 ); 
	I3MEM_SAFE_FREE( m_pui32LobbyUserCount0 ); 
	I3MEM_SAFE_FREE( m_ppLobbyUserList1 ); 
	I3MEM_SAFE_FREE( m_pui32LobbyUserCount1 ); 
	I3MEM_SAFE_FREE( m_pi32LobbyUserInfoSide ); 
	I3MEM_SAFE_FREE( m_ppcsLobbyChatting ); 
	
	// anounce buffer
	I3_SAFE_DELETE( m_pRingAnnounceMessage );
	I3_SAFE_DELETE( m_pRingAnnounceMain );
	I3_SAFE_DELETE( m_pRingAnnounceControl );
	I3_SAFE_DELETE( m_pRingAnnounceTrans );
	
	return i3NetworkSessionManager::OnDestroy(); 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSessionManager::SendListInfoPaket( CUserSession * pUser )
{
	pUser->SendPacketMessage( &m_aUserListPacket[m_i32UserListPacketNum] ); 
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 크리티컬 섹션 필요없음..정보만 가지고 오는것이기 때문.
-----------------------------------------------------------*/
void CUserSessionManager::_UpdateUserListPacket(void)
{
	INT32 i32NextPacketNum = 0; 
	if( m_i32UserListPacketNum == 0) i32NextPacketNum = 1; 

	//ListInfo Packet
	m_aUserListPacket[i32NextPacketNum].Clear();
	m_aUserListPacket[i32NextPacketNum].SetProtocol( PROTOCOL_BASE_GET_CHANNELLIST_ACK );

	m_aUserListPacket[i32NextPacketNum].WriteData(&m_ui32ChannelCount,			sizeof(INT32)); 
	m_aUserListPacket[i32NextPacketNum].WriteData(&m_ui32EachChannelMaxCount,	sizeof(INT32)); 

	//각 채널의 카운드를 채크합니다. 
	INT32 i32Count; 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		i32Count = m_ppChannelUserList[i]->GetCount();
		if(i32Count == (INT32)m_ui32EachChannelMaxCount)
		{
			m_pChannelUserMaxFlag[i] = TRUE; 
		}

		if(m_pChannelUserMaxFlag[i])
		{
			if( i32Count > (INT32)m_ui32ChannelCountPer )
			{
				i32Count = m_ui32EachChannelMaxCount; 
			}
			else
			{
				m_pChannelUserMaxFlag[i] = FALSE; 
			}
		}
        
		m_aUserListPacket[i32NextPacketNum].WriteData(&i32Count,	sizeof(INT32)); 
	}

	m_i32UserListPacketNum = i32NextPacketNum; 
	return;
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CUserSessionManager::OnEnterChannel( CUserSession * pUser, UINT32 ui32Channel )
{
	INT32 i32EnterChannel = EVENT_ERROR_SERVER_USER_FULL; 
	m_ppcsChanneUser[ui32Channel]->Enter(); 
	{
		if( m_ppChannelUserList[ui32Channel]->GetCount() < (INT32)m_ui32EachChannelMaxCount)
		{
			m_ppChannelUserList[ui32Channel]->Add(pUser);
			i32EnterChannel = ui32Channel; 
		}
	}
	m_ppcsChanneUser[ui32Channel]->Leave(); 

	return i32EnterChannel; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSessionManager::OnLeaveChannel( CUserSession * pUser, UINT32 ui32Channel)
{	
	//Check 
	//if(Channel > m_iChannelCount)return;
	m_ppcsChanneUser[ui32Channel]->Enter(); 
	{
		m_ppChannelUserList[ui32Channel]->Remove(pUser); 
	}
	m_ppcsChanneUser[ui32Channel]->Leave(); 

	return; 
}

void CUserSessionManager::OnEnterLobby( CUserSession * pSession, UINT32 ui32Channel )
{
	m_ppcsLobbyUser[ui32Channel]->Enter(); 
	{
		m_ppLobbyUserList[ui32Channel]->Add(pSession); 
	}
	m_ppcsLobbyUser[ui32Channel]->Leave(); 

	return; 
}

void CUserSessionManager::OnLeaveLobby( CUserSession * pSession, UINT32 ui32Channel )
{
	m_ppcsLobbyUser[ui32Channel]->Enter(); 
	{
		m_ppLobbyUserList[ui32Channel]->Remove(pSession);
	}
	m_ppcsLobbyUser[ui32Channel]->Leave(); 

	return; 
}

/*----------------------------------------------------------
Name : OnUpdate 
Desc : 유저들을 상태를확인 합니다. 
----------------------------------------------------------*/
void CUserSessionManager::OnUpdate(void)
{
	CUserSession * pUserSession; 

	UINT32 ui32NowTime					= g_pContextMain->GetServerTime(); 
	UINT32 ui32CheckServerTime			= ui32NowTime - SESSION_CHECK_TIMEOUT_NORMAL;
	UINT32 ui32CheckServerTimeChannel	= ui32NowTime - SESSION_CHECK_TIMEOUT_CHANNEL;
	UINT32 ui32CheckServerTimeLogIn		= ui32NowTime - SESSION_CHECK_TIMEOUT_LOGIN;
	UINT32 ui32CheckServerTimeNotLogIn	= ui32NowTime - SESSION_CHECK_TIMEOUT_NOT_LOGIN;
	UINT32 ui32CheckServerTimeGetInfo	= ui32NowTime - SESSION_CHECK_TIMEOUT_GETINFO;
	UINT32 ui32KillUserTime 			= ui32NowTime - SESSION_DISCONNECT_TIME; 
	UINT32 ui32GameGuardSendTime		= ui32NowTime - m_ui32SessionGameGuardUpdateTime;
	UINT32 ui32GameGuardTimeOut			= ui32NowTime - SESSION_CHECK_TIMEOUT_GAMEGUARD;

	INT32 i32RoomPageNum				= g_pRoomManager->GetChannelUseRoomCount( g_pContextMain->m_ui32UpdateChannel ); 

	i32RoomPageNum = INT32(i32RoomPageNum / SEND_MAX_ROOM_COUNT);
	if((i32RoomPageNum % SEND_MAX_ROOM_COUNT) ==0)i32RoomPageNum--; 
	if(i32RoomPageNum < 0)i32RoomPageNum = 0;

	//로비 유저를 위한 선 작업
	INT32 i32LobbyUserUpdateSide = m_pi32LobbyUserInfoSide[g_pContextMain->m_ui32UpdateChannel];
	LOBBY_USER_INFO * pLobbyUserInfo; 	
	UINT32 * pui32LobbyUserInfCount;
	LOBBY_USER_INFO * pLobbyUserInfo2;
	UINT32 * pui32LobbyUserInfCount2;
	if(m_pi32LobbyUserInfoSide[g_pContextMain->m_ui32UpdateChannel] == 0)
	{				
		i32LobbyUserUpdateSide = 1; 
		pui32LobbyUserInfCount	= m_pui32LobbyUserCount1; 
		pLobbyUserInfo		= m_ppLobbyUserList1[g_pContextMain->m_ui32UpdateChannel];

		// 현재 사용되고 있는 로비 유저를 석택합니다.
		pui32LobbyUserInfCount2	= m_pui32LobbyUserCount0;
		pLobbyUserInfo2		= m_ppLobbyUserList0[g_pContextMain->m_ui32UpdateChannel];
	}
	else
	{
		i32LobbyUserUpdateSide = 0; 
		pui32LobbyUserInfCount	= m_pui32LobbyUserCount0; 
		pLobbyUserInfo		= m_ppLobbyUserList0[g_pContextMain->m_ui32UpdateChannel]; 

		// 현재 사용되고 있는 로비 유저를 석택합니다.
		pui32LobbyUserInfCount2	= m_pui32LobbyUserCount1; 
		pLobbyUserInfo2		= m_ppLobbyUserList1[g_pContextMain->m_ui32UpdateChannel];
	}
	INT32 i32LobbyUserCount = 0; 

	// UserList
	INT32 i32UserPageNum = pui32LobbyUserInfCount2[ g_pContextMain->m_ui32UpdateChannel ] / SEND_MAX_LOBBY_USER_COUNT;
	if( (pui32LobbyUserInfCount2[ g_pContextMain->m_ui32UpdateChannel ] % SEND_MAX_LOBBY_USER_COUNT) == 0 ) i32UserPageNum--;
	if(i32UserPageNum < 0) i32UserPageNum = 0;

	m_i32ActiveCount = 0;
	INT32 i32KillUser = 0;

	// Announce Buffer
	AnnounceMessageCollect(m_pRingAnnounceMain);
	AnnounceMessageCollect(m_pRingAnnounceControl);
	AnnounceMessageCollect(m_pRingAnnounceTrans);
	if( m_pRingAnnounceMessage->GetBufferCount() > 0 )
	{
		if( OnSendAnnounceMessage( (ANNOUNCE_MESSAGE *)m_pRingAnnounceMessage->Pop() ) )
		{
			// 대상이 되는 모든 유저에게 보냈으면 해당 메시지 제거
			m_pRingAnnounceMessage->PopIdx();
		}
	}

	// HeartBit2 Init(USE_SERVER_WATCH_CONTENTS)
	m_ui32RecvHeartBit2Count	= 0;

	// g_pContextMain->m_SessionCount 유저 카운트입니다. 3000명을 생각한다면.. 좀더 수정해서 해야 합니다.
	// 유저는 룸과 틀린방식으로 만들어요. 
	for(INT32 i = 0; i < g_pContextMain->m_i32SessionCount; i++)
	{
		pUserSession = (CUserSession*)m_pSessionList[i];

		if( FALSE == pUserSession->GetIsActive() )continue; 

		// 패킷 처리 테스트코드
		if( m_bParseTimeReset )
		{
			pUserSession->m_dwParseTimeHigh = 0;
			pUserSession->m_ui16ParseTimeHighProtocol = 0;
		}
		else if( pUserSession->m_dwParseTimeHigh > m_dwParseTimeHigh )
		{
			m_dwParseTimeHigh = pUserSession->m_dwParseTimeHigh;
			m_ui16ParseTimeHighProtocol = pUserSession->m_ui16ParseTimeHighProtocol;
		}

		//0. Check Hacking User
		if( pUserSession->m_ui32ATSendKill > 0 && !pUserSession->m_bHackKill )
		{
			if (pUserSession->m_ui32ATSendKill <= ui32KillUserTime )
			{
				pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_HACKING_EXIT_USER, 0, pUserSession->m_ui32KillReason, pUserSession->m_strHackNotice );
				pUserSession->m_bHackKill = TRUE;
			}
			continue;
		}

		//1. DisConnect User
		if(pUserSession->m_ui32ATIsKill > 0)
		{
			if (pUserSession->m_ui32ATIsKill <= ui32KillUserTime)
			{	// 게임서버일때 한번에 킬 해주는 유저를 5 명으로 제한합니다.
				if( ( SERVER_TYPE_GAME == g_pContextMain->m_i32Gateway ) && ( i32KillUser > 5 ) ) continue;

				DisConnectSession(pUserSession, TRUE);

				i32KillUser++;
			}
			continue; 
		}

		//너무 많은 패킷을 던지는지 확인하기 
		if( pUserSession->m_ui32PacketSecondCount > 40 )
		{
			pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_EVENT , 0, KILL_REASON_PACKET_COUNT, "PacketSecondCount > 40" );
			continue;
		}

		//2. Normal User Time Out 
		if( pUserSession->m_ui32ATLastRecv < ui32CheckServerTime )
		{
#ifndef I3_DEBUG
			pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_EVENT, 0, KILL_REASON_NORMAL_TIMEOUT );
			continue;
#endif
		}	

		//3. LogIn Time Out For Auth Server Connect 
		if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)
		{
			if( pUserSession->m_ui32LogInST > 0)
			{
				// 로그인 Wait 중인 유저일 경우 SESSION_CHECK_LOGINWAIT_SENDTIME 초에 한번씩 씽크용 패킷을 날려줍니다.
#ifdef LOGIN_DELAY
				if( pUserSession->m_ui32LoginWaitSendTime < ui32NowTime )
				{
					pUserSession->m_ui32LoginWaitSendTime = ui32NowTime + SESSION_CHECK_LOGINWAIT_SENDTIME;

					UINT8 ui8ThreadIdx		= pUserSession->m_ui8LoginLinkThreadIdx;
					INT32 i32WaitCount		= 0;
					if( pUserSession->m_ui32LoginWaitCount > m_aui32LinkLoginCount[ ui8ThreadIdx ] )
					{
						i32WaitCount = pUserSession->m_ui32LoginWaitCount - m_aui32LinkLoginCount[ ui8ThreadIdx ];
					}
					UINT32 ui32LoginTimeArg = (m_aui32LinkLoginTimeArg[ ui8ThreadIdx ] / 500) * i32WaitCount;

					i3NetworkPacket SendPacket( PROTOCOL_BASE_LOGIN_WAIT_ACK );
					SendPacket.WriteData( &i32WaitCount,		sizeof(INT32) );
					SendPacket.WriteData( &ui32LoginTimeArg ,	sizeof(UINT32) );
					pUserSession->SendPacketMessage( &SendPacket );
				}
#endif
				//로그인 요청이 들어간 유저 
				if( pUserSession->m_ui32LogInST < ui32CheckServerTimeLogIn )
				{
					m_i32TimeLogInUser++;
					pUserSession->LogIn_MAU( MAIN_THREAD_IDX, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2, NULL, NULL, NULL);	//이부분 메인에서 호출하나.. 에러이기때문에 내부에서 아무런일도 하지 않음
					continue;
				}
			}
			else
			{//로그인 요청이 들어가지 않은 유저 (커넥션 후 로그인 요청이 느린 유저 포함)
				if( pUserSession->m_ui32ATLastRecv < ui32CheckServerTimeNotLogIn )
				{
					char strTemp[MAX_PATH];
					i3String::Format( strTemp, MAX_PATH, "LOGIN FAIL : UID[%I64d],ID[%s] ", pUserSession->m_i64UID, pUserSession->m_strId );
					pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_LOGIN_ERROR, 0, KILL_REASON_LOGTIMEOUT, strTemp );
					continue; 
				}
			}
			continue;
		}

		// Get Info Time Out
		if( 0 < pUserSession->m_ui32ATInfo )
		{
			if( pUserSession->m_ui32ATInfo < ui32CheckServerTimeGetInfo )
			{
				pUserSession->SendMyInfo( MAIN_THREAD_IDX, EVENT_ERROR_FAIL );
			}
		}

		//4. Channel Time Out User 
		// 기획실 요청으로 잠시 막음 2008-10-07
		if( pUserSession->GetChannelNumber() == SERVER_VALUE_UNKNOWN )
		{					
			if( pUserSession->m_ui32ATLastRecv < ui32CheckServerTimeChannel )
			{
				pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_CHANNEL_TIMEOUT, 0, KILL_REASON_CHANNELTIMEOUT );
				continue;
			}
		}
	
		//-------------------------------------------------------
		m_i32ActiveCount++;

		//GameGUard Check
		if( FALSE == pUserSession->CheckGameGuard_M( ui32GameGuardTimeOut, ui32GameGuardSendTime ) ) continue;

		// HeartBit2 집계(USE_SERVER_WATCH_CONTENTS)
		if( true == pUserSession->m_bIsRecvHeartBit2 ) { m_ui32RecvHeartBit2Count++; }

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//100ms마다 한번씩  들어옵니다. 		
		// 땡빵코드 입니다. 차후 m_iEachChannelMaxCount 이걸로 만들어야 합니다. 
		if( pUserSession->GetChannelNumber() != (INT32)g_pContextMain->m_ui32UpdateChannel)continue;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		//로비에 있는 사람을 체크후 채널별 방리스트를 보냅니다.
		if( pUserSession->GetPosition() == GAME_TASK_LOBBY )
		{
			//로비에 있는 유저들을 리스트에 넣는 작업을 합니다. 			
			pLobbyUserInfo[i32LobbyUserCount].pSession = pUserSession; 
			i32LobbyUserCount++; 

			//방리스트를 요청한 유저가 있으면 방 리스트를 보내준다. 
			if( pUserSession->m_bRequestRoomList )
			{
				// RoomList
				pUserSession->m_ui32UpdateRoomListPage++; 			
				if( pUserSession->m_ui32UpdateRoomListPage > (UINT32)i32RoomPageNum ) pUserSession->m_ui32UpdateRoomListPage = 0;

				// UserList
				pUserSession->m_ui32UpdateUserListPage++;
				if( pUserSession->m_ui32UpdateUserListPage > (UINT32)i32UserPageNum ) pUserSession->m_ui32UpdateUserListPage = 0;
				
				g_pRoomManager->OnSendRoomList(pUserSession, pUserSession->GetChannelNumber(), pLobbyUserInfo2, (pui32LobbyUserInfCount2[ pUserSession->GetChannelNumber() ]) ); 				
				pUserSession->m_bRequestRoomList = FALSE; 
			}
		}
	}

	if(g_pContextMain->m_i32Hour == g_pContextMain->m_i32TSInitHour && g_pContextMain->m_i32Minute == g_pContextMain->m_i32TSInitMin)
	{
		if(g_pContextMain->m_i32Minute != m_i32LastUpdate)
		{
			for(INT32 i = 0; i < g_pContextMain->m_i32SessionCount; i++)
			{
				pUserSession = (CUserSession*)m_pSessionList[i];

				if( FALSE == pUserSession->m_UserInfoDaily._bGetItem ) continue;

				UINT32 ui32EventDate = ((g_pContextMain->m_i32Year - 2000) * 100000000) + (g_pContextMain->m_i32Month * 1000000) + (g_pContextMain->m_i32Day * 10000) + (g_pContextMain->m_i32TSInitHour * 100);

				i3mem::FillZero(&pUserSession->m_UserInfoDaily, sizeof(USER_INFO_DAILY));
				pUserSession->m_UserInfoDaily._EventDate = ui32EventDate;
			}
			m_i32LastUpdate = g_pContextMain->m_i32Minute;
		}
	}
	else if(g_pContextMain->m_i32Hour == g_pContextMain->m_i32TSInitHour && g_pContextMain->m_i32Minute == g_pContextMain->m_i32TSInitMin + 1)
	{
		m_i32LastUpdate = -1;
	}
	// 패킷 처리 테스트코드
	if( m_bParseTimeReset )
	{
		m_dwParseTimeHigh = 0;
		m_ui16ParseTimeHighProtocol = 0;
		m_bParseTimeReset = FALSE;;
	}

	// Option 정보 클라이언트에 보내주기
	{
		NSM_CRingBuffer* pRingOut = g_pDataBase->GetRingOutBuffer();
		INT32 i32Count = pRingOut->GetBufferCount();
		for( INT32 i = 0 ; i < i32Count ; i++ )
		{
			TYPE_RING_OUT* pOut = (TYPE_RING_OUT*)pRingOut->Pop();
			_SendOptionPacket( pOut );
			pRingOut->PopIdx();
		}
	}

	if( SEND_ALIVEPACKET_PERIOD <= m_i32SendAliveUserCycle )
	{
		m_i32SendAliveUserCycle = 0;

		CUserSession * apAliveUserSessionList[SEND_ALIVEPACKET_PACK];
		UINT8		   ui8AliveUserCount = 0;
		for( INT32 i = 0; i < SEND_ALIVEPACKET_PACK; i++ )
		{
			pUserSession = (CUserSession*)m_pSessionList[ m_i32SendAliveUserIdx ];
			TUID i64UID = pUserSession->m_i64UID; 

			if( pUserSession->GetIsActive() &&
				i64UID != SERVER_VALUE_UNKNOWN &&
				pUserSession->m_ui32ATIsKill == 0 )
			{
				// 트랜스 서버 부하를 줄이기 위해 Alive 패킷을 보낼 대상을 묶어서 전달
				apAliveUserSessionList[ui8AliveUserCount++] = pUserSession;

				if (!g_pModuleMessenger->MessengerAliveUser(MAIN_THREAD_IDX,	m_i32SendAliveUserIdx, i64UID, pUserSession->GetNickName()))
				{
					char strTemp[MAX_PATH];
					i3String::Format( strTemp, MAX_PATH, "(MessengerAliveUser)#### ALIVE PACKET ChangeDBFlage :%d ", pUserSession->m_ui8ChangeDBFlage );
					g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, pUserSession, strTemp ) ; 
				}
			}

			m_i32SendAliveUserIdx++; 
			if(m_i32SendAliveUserIdx >= g_pContextMain->m_i32SessionCount)m_i32SendAliveUserIdx = 0; 
		}

		if( ui8AliveUserCount > 0 )
		{
			if( !g_pModuleTrans->AuthSendAliveUser(MAIN_THREAD_IDX, ui8AliveUserCount, apAliveUserSessionList ) )
			{
				char strTemp[MAX_PATH];
				for( INT32 i = 0; i < ui8AliveUserCount; i++ )
				{
					i3String::Format( strTemp, MAX_PATH, "#### ALIVE PACKET ChangeDBFlage :%d ", apAliveUserSessionList[i]->m_ui8ChangeDBFlage );
					g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, pUserSession, strTemp ) ; 
				}
			}
		}
	}
	else
	{
		m_i32SendAliveUserCycle++;
	}

	pui32LobbyUserInfCount[g_pContextMain->m_ui32UpdateChannel]   = i32LobbyUserCount; 
 	m_pi32LobbyUserInfoSide[g_pContextMain->m_ui32UpdateChannel] = i32LobbyUserUpdateSide;

	m_i32SessionCheckIdx += SESSION_CHECK_COUNT; 
	if( m_i32SessionCheckIdx >= g_pContextMain->m_i32SessionCount )m_i32SessionCheckIdx = 0; 

	//Update 
	_UpdateUserListPacket();

	return; 
}

void CUserSessionManager::_SendOptionPacket( TYPE_RING_OUT* pOut )
{
	CUserSession* pSession = GetSession( pOut->_i32SessionIdx, pOut->_i64UID );
	if( NULL == pSession )	return;

	INT32 i32Rv = EVENT_ERROR_SUCCESS;
	i3NetworkPacket SendPacket;
	SendPacket.SetProtocol( PROTOCOL_BASE_GET_OPTION_ACK );
	SendPacket.WriteData( &i32Rv,						sizeof(INT32) );
	SendPacket.WriteData( &pOut->_ui8DefaultKey,		sizeof(UINT8) );
	if( ENV_SETTING_DEFAULT != pOut->_ui8DefaultKey )
	{
		SendPacket.WriteData( &pOut->_NetEnvSet,		sizeof(NET_ENV_SET) );
		SendPacket.WriteData( &pOut->_aKeyMap,			sizeof( pOut->_aKeyMap ) );

		UINT8 ui8Size;
		for( INT32 i = 0 ; i < MACRO_MAX_COUNT ; i++ )
		{
			ui8Size = (UINT8)i3String::Length( pOut->_astrMacro[i] ) + 1;
			SendPacket.WriteData( &ui8Size,				sizeof(UINT8) );
			SendPacket.WriteData( pOut->_astrMacro[i],	ui8Size );
		}
	}

	pSession->SendPacketMessage( &SendPacket );
}

/*----------------------------------------------------------
Name : 
Desc :  
----------------------------------------------------------*/
void CUserSessionManager::OnSendLobbyChatting( CUserSession * pSender, char * pstrMessage, UINT16 ui16size)
{
	//채팅 사용함.
	INT32				i32ChannelNum = pSender->m_i32ChannelNum; 	
	
	m_ppcsLobbyChatting[i32ChannelNum]->Enter(); 
	{
		LOBBY_USER_INFO	*	pLobbyUserList; 
		UINT32				ui32LobbyUserCount; 	

		if( m_pi32LobbyUserInfoSide == 0)
		{
			pLobbyUserList = m_ppLobbyUserList0[i32ChannelNum]; 
			ui32LobbyUserCount = m_pui32LobbyUserCount0[i32ChannelNum]; 
		}
		else 
		{
			pLobbyUserList = m_ppLobbyUserList1[i32ChannelNum]; 
			ui32LobbyUserCount = m_pui32LobbyUserCount1[i32ChannelNum]; 
		}

		UINT8 ui8NickSize = (UINT8)i3String::Length( pSender->m_UserInfoBasic._nick ) + 1;
		i3NetworkPacket LobbyChat(PROTOCOL_LOBBY_CHATTING_ACK);
		LobbyChat.WriteData(&pSender->m_SessionIdx,					sizeof(INT32));
		LobbyChat.WriteData(&ui8NickSize,							sizeof(UINT8));
		LobbyChat.WriteData(pSender->m_UserInfoBasic._nick,			ui8NickSize );
		LobbyChat.WriteData(&pSender->m_UserInfoBasic._nickcolor,	sizeof(UINT8));
		LobbyChat.WriteData(&ui16size,								sizeof(UINT16)); 		
		LobbyChat.WriteData(pstrMessage,							ui16size); 		

#if defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_RUSSIA )
		INT32 i32ChattingType = ZLOG_CHATTING_TYPE_LOBBY;
		g_pModuleContextNC->LogSendMessage( pSender->m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING, pSender, &i32ChattingType, pstrMessage );
#endif

		for(UINT32 i = 0; i < ui32LobbyUserCount; i++)
		{
			// 클랜 매치 중인 놈들에게는 보내지 않음
			if( pLobbyUserList[i].pSession->m_bMatchRoom ) continue;

			pLobbyUserList[i].pSession->SendPacketMessage(&LobbyChat); 
		}
	}
	m_ppcsLobbyChatting[i32ChannelNum]->Leave(); 
	
	return; 
}

// ----------------------------------------------------------
// Name : LogInUser_A
// Desc : Auth 서버로 부터 로그인을 받는다. 
// ----------------------------------------------------------
CUserSession *  CUserSessionManager::LogInUser_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32AuthKey, char* pstrId, char* pstrCustom, char* strUID, UINT8 ui8LinkThreadIdx, UINT32 ui32LoginDelayTime, UINT8 ui8Popup )
{
	// Session 이 정상인지 체크 합니다.
	if( (0 > i32SessionIdx) || (i32SessionIdx >= g_pContextMain->m_i32SessionCount) )
	{
		char strTemp[MAX_PATH];
		i3String::Format( strTemp, MAX_PATH, "[ERROR] LogInUser_A : <Break Session> Session:%d, iDbIdx:%I64d, rv:%d", i32SessionIdx, i64UID, i32Rv );
		g_pModuleLogFile->Write_M_Log( MODULE_TRANS_THREAD_IDX, NULL, strTemp );
		return NULL; 
	}

	CUserSession * pUserSession = (CUserSession*)m_pSessionList[ i32SessionIdx ];
	if( NULL == pUserSession )
	{
		char strTemp[MAX_PATH];
		i3String::Format( strTemp, MAX_PATH, "[ERROR] LogInUser_A : <NULL Session> Session:%d, iDbIdx:%I64d, rv:%d", i32SessionIdx, i64UID, i32Rv );
		g_pModuleLogFile->Write_M_Log( MODULE_TRANS_THREAD_IDX, NULL, strTemp );

		if( EV_SUCCESSED( i32Rv ) )
		{
			g_pModuleTrans->TransSendAccountKick( MODULE_TRANS_THREAD_IDX, i64UID );
		}
		return NULL;
	}

	if( FALSE == pUserSession->GetIsActive() )
	{
		char strTemp[MAX_PATH];
		i3String::Format( strTemp, MAX_PATH, "[ERROR] LogInUser_A : <Not Active> Session:%d, iDbIdx:%I64d, rv:%d", i32SessionIdx, i64UID, i32Rv );
		g_pModuleLogFile->Write_M_Log( MODULE_TRANS_THREAD_IDX, NULL, strTemp );
		return NULL;
	}

	// 로그인 인증 키값을 체크합니다.
	if( pUserSession->m_ui32LoginKey != ui32AuthKey )			
	{	// 에러값 리턴해 줍니다.
		char strTemp[MAX_PATH];
		i3String::Format( strTemp, MAX_PATH, "[ERROR] LogInUser_A : <Not AuthKey> Session:%d, iDbIdx:%I64d, OrgKey:%d, NewKey:%d", i32SessionIdx, i64UID, pUserSession->m_ui32LoginKey, ui32AuthKey );
		g_pModuleLogFile->Write_M_Log( MODULE_TRANS_THREAD_IDX, NULL, strTemp );

		i32Rv = EVENT_ERROR_LOGIN_BREAK_SESSION;
	}

	pUserSession->LogIn_MAU( MODULE_TRANS_THREAD_IDX, i32Rv, i64UID, pstrId, pstrCustom, strUID, ui8LinkThreadIdx, ui32LoginDelayTime, ui8Popup );

	return pUserSession; 
}

// ----------------------------------------------------------
// Name : LogInWaitUser_A
// Desc : Auth 서버로 부터 로그인을 받는다. 
// ----------------------------------------------------------
void CUserSessionManager::LogInWaitUser_A( INT32 i32SessionIdx, UINT32 ui32AuthKey, INT32 i32WaitCount, UINT32 ui32LoginTimeArg, UINT8 ui8LinkThreadIdx )
{
	// Session 이 정상인지 체크 합니다.
	if( 0 > i32SessionIdx )									return;
	if( i32SessionIdx >= g_pContextMain->m_i32SessionCount)	return;

	CUserSession * pUserSession = (CUserSession*)m_pSessionList[ i32SessionIdx ];
	if( NULL == pUserSession )								return;
	if( FALSE == pUserSession->GetIsActive() )				return;
	// 로그인 인증 키값을 체크합니다.
	if( pUserSession->m_ui32LoginKey != ui32AuthKey )		return;

	// 데이터를 저장합니다.
	if( DEF_TRANS_LINK_COUNT_MAX > ui8LinkThreadIdx )
	{
		pUserSession->m_ui32LoginWaitSendTime	= g_pContextMain->GetServerTime() + SESSION_CHECK_LOGINWAIT_SENDTIME;
		pUserSession->m_ui8LoginLinkThreadIdx	= ui8LinkThreadIdx;
		pUserSession->m_ui32LoginWaitCount		= m_aui32LinkLoginCount[ ui8LinkThreadIdx ] + i32WaitCount;
	}

	// Wait 패킷 전송
	i3NetworkPacket SendPacket( PROTOCOL_BASE_LOGIN_WAIT_ACK );
	SendPacket.WriteData( &i32WaitCount,		sizeof(INT32) );
	SendPacket.WriteData( &ui32LoginTimeArg,	sizeof(UINT32) );
	pUserSession->SendPacketMessage( &SendPacket );
}

// ----------------------------------------------------------
// Name : GetMyInfo_A
// Desc : Auth 서버로 부터 케릭터 정보를 받는다. 
// ----------------------------------------------------------
void CUserSessionManager::GetMyInfo_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, i3NetworkPacket* pPacket )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->GetMyInfo_A( MODULE_TRANS_THREAD_IDX, i32Rv, pPacket );
}

// ----------------------------------------------------------
// Name : UserServerEnter_A
// Desc : 서버이동 후 아이템 정보 받기
// ----------------------------------------------------------
void CUserSessionManager::UserServerEnter_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, i3NetworkPacket* pPacket )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->ServerEnter_A( i32Rv, pPacket );
}

// ----------------------------------------------------------
// Name : UserServerLeave_A
// Desc : 서버이동 후 아이템 정보 받기
// ----------------------------------------------------------
void CUserSessionManager::UserServerLeave_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->ServerLeave_A( i32Rv );
}

// ----------------------------------------------------------
// Name : UserServerLeave_A
// Desc : 서버이동 후 아이템 정보 받기
// ----------------------------------------------------------
void CUserSessionManager::UserCreateNickName_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, char* pstrNickName )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->CreateNickName_A( i32Rv, pstrNickName );
}

/*----------------------------------------------------------
Name : KickUser_Module
Desc : Auth 서버로 부터 강퇴를 받는다. 
----------------------------------------------------------*/
void CUserSessionManager::KickUser_A( INT32 iSessionIdx, TUID i64UID, UINT8 ui8Reason )
{
	CUserSession * pUserSession = GetSession( iSessionIdx, i64UID );
	if( NULL == pUserSession )
	{
		g_pModuleTrans->TransSendAccountKick( MODULE_TRANS_THREAD_IDX, i64UID );
		return;
	}

	pUserSession->KickUser_Module(i64UID, ui8Reason);
}

// ----------------------------------------------------------
// Name : GetAttendanceResult
// Desc : 출석체크 이벤트 완료 보상 요청에 대한 결과값을 받습니다. - 서동권
// ----------------------------------------------------------
void CUserSessionManager::GetAttendanceResult( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->SendAttendanceItemResult( i32Rv );
}

// ----------------------------------------------------------
// Name : GetAttendanceResult
// Desc : 출석체크 이벤트 정보 요청에 대한 결과값을 받습니다. - 서동권
// ----------------------------------------------------------
void CUserSessionManager::GetAttendanceInfo( INT32 i32SessionIdx, TUID i64UID, ATTENDANCE_USER* pstAttendance, ATTENDANCE_INFO* pstInfo )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->SendAttendanceInfoResult( pstAttendance, pstInfo );
}

// ----------------------------------------------------------
// Name : GetAttendanceResult
// Desc : 출석체크 이벤트 출석 요청에 대한 결과값을 받습니다. - 서동권
// ----------------------------------------------------------
void CUserSessionManager::GetAttendanceCheck( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, ATTENDANCE_USER* pAttendance )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->SendAttendanceCheckResult( i32Rv, pAttendance );
}

void CUserSessionManager::DestroyRoom_A( INT32 i32SessionIdx )
{
	if(i32SessionIdx >= g_pContextMain->m_i32SessionCount)
	{
		I3TRACE("[CUserSessionManager::DestroyRoom_A]Wrong Session Idx"); 
		return; 
	}
	CUserSession * pUserSession; 
	pUserSession = (CUserSession*)m_pSessionList[i32SessionIdx];
	
	if( pUserSession->GetIsActive() && (pUserSession->m_i32RoomIdx != SERVER_VALUE_UNKNOWN) )
	{
		// 이방을 폭파 시킨다.
		pUserSession->m_pRoom->DestroyRoom(); 
	}

	return; 
}

void CUserSessionManager::SendUserNowInfo_A(INT32 i32SessionIdx )
{
	if(i32SessionIdx >= g_pContextMain->m_i32SessionCount)
	{
		I3TRACE("[CUserSessionManager::GetUserNowInfo_M]Wrong Session Idx"); 
		return; 
	}

	CUserSession * pUserSession; 

	pUserSession = (CUserSession*)m_pSessionList[i32SessionIdx];
	
	if( pUserSession->GetIsActive() )
	{
		g_pModuleTrans->AuthSendUserNowInfo( MODULE_TRANS_THREAD_IDX, i32SessionIdx, pUserSession->m_i32ChannelNum, pUserSession->m_i32RoomIdx, pUserSession->m_i32SlotIdx); 
	}
	return; 
}

void CUserSessionManager::SendRoomNoticeMessage_A(INT32 i32SessionIdx, char * pstrMessage, UINT32 ui32len)
{
	if(i32SessionIdx >= g_pContextMain->m_i32SessionCount)
	{
		I3TRACE("[CUserSessionManager::SendRoomNoticeMessage_A]Wrong Session Idx"); 
		return; 
	}

	CUserSession * pUserSession; 
	pUserSession = (CUserSession*)m_pSessionList[i32SessionIdx];
	
	if( pUserSession->GetIsActive() && (pUserSession->m_i32RoomIdx != SERVER_VALUE_UNKNOWN) )
	{
		//메시지 보내기 
		pUserSession->m_pRoom->SendNoticeMessage(pstrMessage, ui32len); 
	}

	return; 
}

/*----------------------------------------------------------
Name : OnSendLobbyUserList
Desc : 유저리스트 요청하기
----------------------------------------------------------*/
void CUserSessionManager::OnSendLobbyUserList(CUserSession * pSession, UINT32 ui32Channel)
{
	i3NetworkPacket SendPacket( PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK );

	UINT8 ui8NickLength;  
	CUserSession * pLobbySession; 
	INT32 i32UserCount = 0;						
	CUserSession * apUserList[MAX_INVITE_COUNT];	

	m_ppcsLobbyUser[ui32Channel]->Enter(); 
	{				
		//PreOBT에 랜덤하게 나오는 유저리스트를 위해서 만듬
		if( m_ppLobbyUserList[ui32Channel]->GetCount() > 0)
		{
			INT32 i32Idx = rand() % m_ppLobbyUserList[ui32Channel]->GetCount(); 

			//보낼수 있는 유저를 검사합니다.
			for(INT32 i = 0; i < m_ppLobbyUserList[ui32Channel]->GetCount(); i++)
			{
				pLobbySession	= (CUserSession*) m_ppLobbyUserList[ui32Channel]->GetItem(i32Idx); 
				if( pLobbySession->m_UserInfoBasic._nick[0] != 0 && pLobbySession->m_bMatchRoom == FALSE )
				{
					apUserList[i32UserCount] = pLobbySession; 
					i32UserCount++;				
					if(i32UserCount >= MAX_INVITE_COUNT)break; 
				}

				i32Idx++; 
				if( i32Idx >= m_ppLobbyUserList[ui32Channel]->GetCount() )i32Idx = 0;
			}
		}
	}
	m_ppcsLobbyUser[ui32Channel]->Leave(); 


	//패킷만들기
	SendPacket.WriteData( &i32UserCount, sizeof( UINT32 ) );			//카운트 
	for(INT32 i = 0; i < i32UserCount; i++)
	{
		SendPacket.WriteData(&apUserList[i]->m_SessionIdx,				sizeof(INT32) );		//IDX
		SendPacket.WriteData(&apUserList[i]->m_UserInfoBasic._rank,		sizeof(INT32) );		//Rank
		ui8NickLength		= (UINT8)i3String::Length( apUserList[i]->m_UserInfoBasic._nick ) + 1;		//Nick Name
		SendPacket.WriteData(&ui8NickLength,							sizeof(UINT8) );		//Nick Name
		SendPacket.WriteData(apUserList[i]->m_UserInfoBasic._nick,		ui8NickLength);			//Nick Name
#ifdef USE_COLOR_NICK2
		SendPacket.WriteData(&apUserList[i]->m_UserInfoBasic._nickcolor,	sizeof(UINT8));				//Nick Name
#endif
	}

	pSession->SendPacketMessage( &SendPacket ); 

	return; 
}

/*----------------------------------------------------------
Name : OnSendLobbyUserList
Desc : 유저리스트 요청하기
----------------------------------------------------------*/
/*void CUserSessionManager::OnSendLobbyUserListNew(CUserSession * pSession, UINT32 ui32Channel, i3NetworkPacket* pPacket)
{
	CUserSession * pLobbySession; 
	INT32 i32UserCount = 0;						
	CUserSession * apUserList[MAX_INVITE_COUNT];	

	m_ppcsLobbyUser[ui32Channel]->Enter(); 
	{				
		if( m_ppLobbyUserList[ui32Channel]->GetCount() > 0)
		{

			//보낼수 있는 유저를 검사합니다.
			for(INT32 i = 0; i < m_ppLobbyUserList[ui32Channel]->GetCount(); i++)
			{
				pLobbySession	= (CUserSession*) m_ppLobbyUserList[ui32Channel]->GetItem(i); 
				if( pLobbySession->m_UserInfoBasic._nick[0] != 0 && pLobbySession->m_bMatchRoom == FALSE )
				{
					apUserList[i32UserCount] = pLobbySession; 
					i32UserCount++;				
					if(i32UserCount >= MAX_INVITE_COUNT)break; 
				}
			}
		}
	}
	m_ppcsLobbyUser[ui32Channel]->Leave(); 

	INT32	 i32UserPageNum = pSession->m_ui32UpdateUserListPage;

	UINT32	 ui32PageCount	 = i32UserCount / SEND_MAX_LOBBY_USER_COUNT;

	if( (i32UserCount % SEND_MAX_LOBBY_USER_COUNT) > 0) ui32PageCount++; 
	if( i32UserPageNum > (INT32)i32UserCount ) i32UserPageNum = 0;

	UINT32 ui32StartIdx	= i32UserPageNum * SEND_MAX_LOBBY_USER_COUNT; 
	UINT32 ui32EndIdx	= ui32StartIdx + SEND_MAX_LOBBY_USER_COUNT; 		
	if(ui32EndIdx > i32UserCount)		ui32EndIdx = i32UserCount; 
	UINT32 ui32PacketUserCount = ui32EndIdx - ui32StartIdx; 
	
	pPacket->WriteData( &i32UserCount,			sizeof(UINT32));
	pPacket->WriteData( &i32UserPageNum,		sizeof(INT32));
	pPacket->WriteData( &ui32PacketUserCount,	sizeof(INT32));

	for( UINT32 i = ui32StartIdx ; i < ui32EndIdx ; i++ )
	{
		pPacket->WriteData( &apUserList[i]->m_SessionIdx,				sizeof(INT32) );
		pPacket->WriteData( &apUserList[i]->m_UserInfoBasic._clanMark,	sizeof(UINT32));
		pPacket->WriteData( apUserList[i]->m_UserInfoBasic._clanName,	NET_CLAN_NAME_SIZE);
		pPacket->WriteData( &apUserList[i]->m_UserInfoBasic._rank,		sizeof(UINT8));
		pPacket->WriteData( &apUserList[i]->m_UserInfoBasic._eSportID,	sizeof(UINT8));
		pPacket->WriteData( apUserList[i]->m_UserInfoBasic._nick,		NET_NICK_NAME_SIZE );
		pPacket->WriteData( &apUserList[i]->m_UserInfoBasic._nickcolor,	sizeof(UINT8) );
	}

	return; 
}*/

/*----------------------------------------------------------
Name : OnInviteLobbyUser
Desc : 초대하기 
----------------------------------------------------------*/
void CUserSessionManager::OnInviteLobbyUser( CUserSession * pSession, UINT32 ui32Channel, UINT32 ui32InvitedUserIdx, char * pPass)
{
	CUserSession * pInvitedSession = (CUserSession *)m_pSessionList[ ui32InvitedUserIdx ]; 

	//초청 메시지를 보낼수 있는지를 검사합니다. 
	if( !pSession->GetIsActive() )return;									//살아있는지 	
	if( pSession->m_i32ChannelNum != pInvitedSession->m_i32ChannelNum )return;	//채널에 같은지 
	if( pInvitedSession->GetPosition() != GAME_TASK_LOBBY)return;			//초대 받은 사람이 로비에 있는 확인

	i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_INVITED );
	SendPacket.WriteData( pSession->m_UserInfoBasic._nick,	NET_NICK_NAME_SIZE );	//Wirte Nick Name
	SendPacket.WriteData( &pSession->m_i32RoomIdx,			sizeof(INT32) );		//RoomIdx
	SendPacket.WriteData( &pSession->m_i64UID,				sizeof(TUID) );			//RoomIdx
	SendPacket.WriteData( pPass,							NET_ROOM_PW);			//Room Passwd
	pInvitedSession->SendPacketMessage( &SendPacket );

	return; 
}

void CUserSessionManager::_OnInsertItem( CUserSession * pSession, INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet )
{
	pSession->OnInsertItem( pInvenItem, pInvenSet );
}

UINT32 CUserSessionManager::_OnDeleteItem( CUserSession * pSession, TUID i64ItemWareIdx, INVEN_BUFFER_SET* pInvenSet )
{
	return pSession->OnDeleteItem( i64ItemWareIdx, pInvenSet );
}



/*----------------------------------------------------------
Name : OnGoodsBuy_A
Desc : 구입하기
----------------------------------------------------------*/
void CUserSessionManager::OnGoodsBuy_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32GoodsID, UINT8 ui8BuyType, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem, UINT32 ui32Point, UINT32 ui32Cash, char* pstrRedirectUrl )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if(pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet; 
	
	// 성공 실패 여부에 따라 금액을 먼저 지급합니다.
	if( EV_SUCCESSED( i32Rv ) )
	{
		pSession->m_UserInfoBasic._cash		= ui32Cash;
		if( ui32Cash > pSession->m_UserInfoBasic._NCCoin )
		{
			pSession->m_UserInfoBasic._HappyCoin = ui32Cash - pSession->m_UserInfoBasic._NCCoin;
		}
		else
		{
			pSession->m_UserInfoBasic._HappyCoin = 0;
			pSession->m_UserInfoBasic._NCCoin = ui32Cash;
		}
	}
	else
	{
		CShop* pShop = g_pContextMain->GetShop();
		SHOP_GOODS_BASE* pGoods = pShop->GetGoodsData( ui32GoodsID );
		if( NULL != pGoods )
		{
			switch( ui8BuyType )
			{
			case GOODS_BUY_POINT:
				{
					pSession->m_UserInfoBasic._point	+= pGoods->_ui32Point;
				}
				break;
			case GOODS_BUY_CASH:
				{
				}
				break;
			}
		}
	}

	pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	if( GOODS_CLASS_QUESTCARD == GET_GOODS_FLAG_CLASS( ui32GoodsID ) )
	{	// 임무카드 인지 확인 합니다. 임무카드관련한 건 안에서 알아서 처리합니다.
		// 차후에 임무카드 결재 시스템이 추가 되면 변경할 예정입니다. 20110427 노준영
		pSession->SendBuyQuestCardResult( i32Rv, ui32GoodsID, MODULE_TRANS_THREAD_IDX, pInvenItem->_i64ItemWareDBIdx, ui8BuyType );
	}
	else 
	{
		if( EV_SUCCESSED( i32Rv ) )
		{
			// 어떤 아이템을 샀는지 확인 합니다.
			for( UINT32 i = 0 ; i < ui16ItemCount ; i++ )
			{
				pSession->OnInsertItem( &pInvenItem[ i ], &InvenSet );

				if( LOCALE_CODE_THAILAND != pSession->m_ui8LocaleCode ) 
				{
					UINT32 ui32ItemID = 0;
					UINT32 ui32ItemArg = 0;
					SHOP_MATCHING_DATA*		pMatching = g_pContextMain->GetShop()->GetMatchingAllData();
					UINT32	ui32MatchingCount	= g_pContextMain->GetShop()->GetMatchingCount();
					for( UINT32 i = 0; i < ui32MatchingCount; ++i )
					{
						if( pMatching[i]._ui32GoodsID != ui32GoodsID ) continue;

						ui32ItemID	= pMatching[i]._ui32ItemID;
						ui32ItemArg = pMatching[i]._ui32ItemArg;
						break;
					}
				}
			}

			//ZLog 에 기록 상점 로그 보내기 (구입)
			UINT8 ui8SpendType = 1;
			g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_SHOP_BUY, pSession, &ui32GoodsID, &ui8BuyType, &ui8SpendType );
			// 마지막 캐시 아이템 구입 날짜 저장
			if( GOODS_BUY_CASH == ui8BuyType  )		pSession->m_UserInfoBasic._ui32LastBuyCash = g_pContextMain->GetInvenTime();
		}

		UINT32 ui32InvenTime = g_pContextMain->GetInvenTime();
		i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_GOODS_BUY_ACK );
		SendPacket.WriteData( &i32Rv,									sizeof(INT32));	
		SendPacket.WriteData( &ui32InvenTime,							sizeof(UINT32));
		if(EV_SUCCESSED( i32Rv ))
		{
			SendPacket.WriteData( &InvenSet.ui32CharaCount,				sizeof(UINT32));
			SendPacket.WriteData( &InvenSet.ui32WeaponCount,			sizeof(UINT32));
			SendPacket.WriteData( &InvenSet.ui32ItemCount,				sizeof(UINT32));

			if( InvenSet.ui32CharaCount )
				SendPacket.WriteData( InvenSet.aCharaBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32CharaCount) );
			if( InvenSet.ui32WeaponCount )
				SendPacket.WriteData( InvenSet.aWeaponBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32WeaponCount) );
			if( InvenSet.ui32ItemCount )
				SendPacket.WriteData( InvenSet.aItemBuffer,				(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32ItemCount) );

			SendPacket.WriteData( &pSession->m_UserInfoBasic._point,	sizeof(UINT32));
			SendPacket.WriteData( &pSession->m_UserInfoBasic._cash,		sizeof(UINT32)); 
		}

		pSession->SendPacketMessage( &SendPacket );
	}
	return; 
}

void CUserSessionManager::OnGoodsGift_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32GoodsID, UINT32 ui32Point, UINT32 ui32Cash )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	if(EV_SUCCESSED( i32Rv ))
	{
		pSession->ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO );
	}
	else
	{
		CShop* pShop = g_pContextMain->GetShop();
		SHOP_GOODS_BASE* pGoods = pShop->GetGoodsData( ui32GoodsID );
		if( NULL != pGoods )
		{
			pSession->m_UserInfoBasic._point	+= pGoods->_ui32Point;
		}
	}

	i3NetworkPacket sendPacket(PROTOCOL_AUTH_SHOP_GOODS_GIFT_ACK);
	sendPacket.WriteData(&i32Rv, sizeof(INT32));
	if(EV_SUCCESSED( i32Rv ))
	{
		sendPacket.WriteData(&pSession->m_UserInfoBasic._point, sizeof(UINT32));
		sendPacket.WriteData(&pSession->m_UserInfoBasic._cash,  sizeof(UINT32));		
	}

	pSession->SendPacketMessage(&sendPacket);
}

void CUserSessionManager::OnGiftAuth_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;

	if(EV_SUCCESSED( i32Rv ))
	{
		for( UINT32 i = 0 ; i < ui16ItemCount ; i++ )
		{
			_OnInsertItem( pSession, &pInvenItem[ i ], &InvenSet );
		}
	}

	i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_AUTH_GIFT_ACK );
	SendPacket.WriteData( &i32Rv,										sizeof(INT32));	
	if(EV_SUCCESSED( i32Rv ))
	{		
		char			szTemp[ MAX_PATH ];
		INVEN_BUFFER	pInvItem[MAX_INVEN_COUNT];
		INVEN_BUFFER	pInvCoupon[MAX_COUPON_INVEN_COUNT];
		INT32			ui32ItemCount = 0;
		UINT32			ui32CouponCount = 0;

		for( UINT32 i = 0; i < InvenSet.ui32ItemCount; ++i )
		{
			if( GET_ITEM_FLAG_CLASS( InvenSet.aItemBuffer[i]._ui32ItemID ) == ITEM_CLASS_SALECOUPON )
			{
				if( ui32CouponCount >= MAX_INVEN_COUNT )
				{
					i3String::Format( szTemp, MAX_PATH, "[ OnGiftAuth_A ] Error SaleCoupon Count Over UID : %I64d", i64UID );
					g_pModuleLogFile->Write_M_Log( MODULE_TRANS_THREAD_IDX, NULL, szTemp );
					continue;	// 최대 개수를 초과한 경우 넘어갑니다.
				}
				i3mem::Copy( &pInvCoupon[ ui32CouponCount ], &InvenSet.aItemBuffer[i], sizeof( INVEN_BUFFER ) );
				++ui32CouponCount;
			}
			else
			{
				if( ui32ItemCount >= MAX_INVEN_COUNT )
				{
					i3String::Format( szTemp, MAX_PATH, "[ OnGiftAuth_A ] Error InvenItem Count Over UID : %I64d", i64UID );
					g_pModuleLogFile->Write_M_Log( MODULE_TRANS_THREAD_IDX, NULL, szTemp );
					continue;	// 최대 개수를 초과한 경우 넘어갑니다.
				}
				i3mem::Copy( &pInvItem[ ui32ItemCount ], &InvenSet.aItemBuffer[i], sizeof( INVEN_BUFFER ) );
				++ui32ItemCount;
			}
		}

		SendPacket.WriteData( &InvenSet.ui32CharaCount,				sizeof(UINT32));
		SendPacket.WriteData( &InvenSet.ui32WeaponCount,			sizeof(UINT32));
		SendPacket.WriteData( &ui32ItemCount,						sizeof(UINT32) );	// Inventory Item Count
		SendPacket.WriteData( &ui32CouponCount,						sizeof(UINT32) );	// GiftInventory SaleCoupon Count

		if( InvenSet.ui32CharaCount )
			SendPacket.WriteData( InvenSet.aCharaBuffer,				(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32CharaCount) );
		if( InvenSet.ui32WeaponCount )
			SendPacket.WriteData( InvenSet.aWeaponBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32WeaponCount) );
		if( ui32ItemCount )
			SendPacket.WriteData( pInvItem,							(UINT16)(sizeof(INVEN_BUFFER)*ui32ItemCount) );
		if( ui32CouponCount )
			SendPacket.WriteData( pInvCoupon,						(UINT16)(sizeof(INVEN_BUFFER)*ui32CouponCount) );
	}
	pSession->SendPacketMessage( &SendPacket );
}

void CUserSessionManager::OnInsertItem_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, INT8 i8RequestType, UINT32 ui32InsertArg, INVEN_BUFFER* pInvenItem )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;
	InvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_AUTH;

	if(EV_SUCCESSED( i32Rv ))
	{
		pSession->OnInsertItem( pInvenItem, &InvenSet );

		_SetItemAbilityAuth_A( pSession, pInvenItem->_ui32ItemID, pInvenItem->_ui32ItemArg );

		pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

		if( ITEM_INSERT_REQUEST_BUY != i8RequestType )
		{
			InvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_INSERT;
			g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_GET_ITEM, pSession, pInvenItem, &i8RequestType, &ui32InsertArg );
		}
	}

	// 클라이언트에 바뀐 부분을 알려준다.
	pSession->SendChangeInven( &InvenSet );
	return; 
}

void CUserSessionManager::OnDeleteItem_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, TWareDxIdx i64ItemWareIdx )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;
	UINT32 ui32DeleteItemID = 0;

	if(EV_SUCCESSED( i32Rv ))
	{
		ui32DeleteItemID = _OnDeleteItem( pSession, i64ItemWareIdx, &InvenSet );

		pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
	}

	switch( ui32DeleteItemID )
	{
	case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_CLAN_XP_PLUS_150, CASHITEM_DAY_NO):
	case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_CLAN_XP_PLUS_150_EV, CASHITEM_DAY_NO):
		// 클랜경험치150% - 삭제된 아이템이 클랜 아이템이면 클랜 서버에 전달
		g_pModuleClan->ClanSendItemDelete( MODULE_TRANS_THREAD_IDX, pSession, ui32DeleteItemID );
		break;
	case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_COLOR_CLAN, CASHITEM_DAY_NO):
		// 클랜명색상 - 삭제된 아이템이 클랜 아이템이면 클랜 서버에 전달
		g_pModuleClan->ClanSendItemDelete( MODULE_TRANS_THREAD_IDX, pSession, ui32DeleteItemID );
		break;
	}

	pSession->CheckEquipment();

	i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK );
	SendPacket.WriteData( &i32Rv,						sizeof(INT32));
	if(EV_SUCCESSED( i32Rv ))
	{
		SendPacket.WriteData( &i64ItemWareIdx,		sizeof(TWareDxIdx));
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_ITEM_DELETE, pSession, &ui32DeleteItemID ); 
	}
	pSession->SendPacketMessage( &SendPacket );

	return; 
}

void CUserSessionManager::OnItemRepair_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem, UINT32 ui32Point, UINT32 ui32Cash )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;

	if(EV_SUCCESSED( i32Rv ))
	{		
		_OnInsertItem( pSession, pInvenItem, &InvenSet );

		pSession->m_UserInfoBasic._cash		= ui32Cash;

		pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
	}
	else
	{
		pSession->m_UserInfoBasic._point	+= ui32Point;
	}

	i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_ITEM_REPAIR_ACK );
	SendPacket.WriteData( &i32Rv,									sizeof(INT32));	
	if(EV_SUCCESSED( i32Rv ))
	{
		SendPacket.WriteData( &InvenSet.ui32CharaCount,				sizeof(UINT32));
		SendPacket.WriteData( &InvenSet.ui32WeaponCount,			sizeof(UINT32));
		SendPacket.WriteData( &InvenSet.ui32ItemCount,				sizeof(UINT32));

		if( InvenSet.ui32CharaCount )
			SendPacket.WriteData( InvenSet.aCharaBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32CharaCount) );
		if( InvenSet.ui32WeaponCount )
			SendPacket.WriteData( InvenSet.aWeaponBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32WeaponCount) );		
		if( InvenSet.ui32ItemCount )
			SendPacket.WriteData( InvenSet.aItemBuffer,				(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32ItemCount) );

		SendPacket.WriteData( &pSession->m_UserInfoBasic._point,	sizeof(UINT32));
		SendPacket.WriteData( &pSession->m_UserInfoBasic._cash,		sizeof(UINT32)); 
	}
	pSession->SendPacketMessage( &SendPacket );

	//상점 로그 보내기 (구입) // LogD 현우
	//g_pModuleContextNC->LogSendShopBuy(MODULE_TRANS_THREAD_IDX, pSession, sGameGoodsId, dBuyTId ); 
	return; 
}

void CUserSessionManager::OnAuthUsedWeapon_A( INT32 i32SessionIdx, TUID i64UID, UINT16 ui16InvenCount, INVEN_BUFFER* pInvenItem )
{
	CUserSession * pSession = GetSession(i32SessionIdx, i64UID); 
	if(pSession == NULL)	return;
	
	INVEN_BUFFER_SET InvenSet;
	InvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_USE;

	for( UINT32 i = 0 ; i < ui16InvenCount ; i++ )
	{
		if( 0 == pInvenItem[i]._ui32ItemID )
		{	// 아이템이 만료되어 삭제
			_OnDeleteItem( pSession, pInvenItem[i]._i64ItemWareDBIdx, &InvenSet );
		}
		else
		{
			_OnInsertItem( pSession, &pInvenItem[i], &InvenSet );
		}
	}

	pSession->CheckEquipment();

	pSession->SendChangeInven( &InvenSet );
}

void CUserSessionManager::OnItemAuth_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem )
{
	CUserSession * pSession = GetSession(i32SessionIdx, i64UID); 
	if(pSession == NULL)	return;
	
	INVEN_BUFFER_SET InvenSet;

	if(EV_SUCCESSED( i32Rv ))
	{
		UINT32 ui32ItemID = pInvenItem->_ui32ItemID;
		if( 0 == pInvenItem->_ui32ItemID )
		{	// 아이템이 만료되어 삭제
			ui32ItemID = pSession->OnDeleteItem( pInvenItem->_i64ItemWareDBIdx, &InvenSet );
		}
		else
		{
			pSession->OnInsertItem( pInvenItem, &InvenSet );
		}
		_SetItemAbilityAuth_A( pSession, ui32ItemID, pInvenItem->_ui32ItemArg );

		pSession->CheckEquipment();

		// 아이템 인증일 경우 ZLog 기록
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_INVEN_USE, pSession, &ui32ItemID, pInvenItem ); 
	}

	i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_ITEM_AUTH_ACK );
	SendPacket.WriteData( &i32Rv,				sizeof(INT32));
	if(EV_SUCCESSED( i32Rv ))
	{
		UINT32	ui32InvenTime = g_pContextMain->GetInvenTime();
		SendPacket.WriteData( &ui32InvenTime,		sizeof(UINT32));
		SendPacket.WriteData( pInvenItem,			sizeof(INVEN_BUFFER));
	}
	pSession->SendPacketMessage( &SendPacket );

	return; 
}

void CUserSessionManager::OnGiftList_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, USER_GIFT_LIST UserGiftList )
{
	CUserSession * pSession = GetSession(i32SessionIdx, i64UID);
	if(pSession == NULL)	return;

	UINT32 ui32InvenTime = g_pContextMain->GetInvenTime();

	i3NetworkPacket sendPacket(PROTOCOL_AUTH_SHOP_GET_GIFTLIST_ACK);
	sendPacket.WriteData(&ui32InvenTime,	sizeof(UINT32));

	// SIA에서 부터 내려온 사이즈를 맞추기 위해 임시 값을 보냅니다. 의미 없습니다.
	UINT64 ui64Temp = 0;
	sendPacket.WriteData(&ui64Temp,			sizeof(UINT64));

	sendPacket.WriteData(&i32Rv,			sizeof(INT32));

	if(EV_SUCCESSED( i32Rv ))
	{
		UINT8	ui8TotalGiftCount	= UserGiftList.m_ui8TotalGiftCount;	// 총 선물 갯수
		ui8TotalGiftCount = MIN( ui8TotalGiftCount, MAX_SHOP_USER_GIFT_COUNT ); 
		UINT32	ui8SendGiftCount	= UserGiftList.m_ui8SendGiftCount;	// 한 패킷에 보내는 선물 갯수
		UINT32	ui8StartIdx			= UserGiftList.m_ui8StartIdx;			// 선물 배열 인덱스.( 시작 )

		sendPacket.WriteData( &ui8TotalGiftCount,		sizeof( UINT8 ) );
		sendPacket.WriteData( &ui8SendGiftCount,		sizeof( UINT8 ) );
		sendPacket.WriteData( &ui8StartIdx,				sizeof( UINT8 ) );

		UINT8 ui8SenderNickSize = 0;
		UINT8 ui8MessageSize  = 0;

		for( INT32 i = 0 ; i < ui8SendGiftCount ; i++ )
		{
			// 혹시 모르니
			if( ui8StartIdx >= MAX_SHOP_USER_GIFT_COUNT ) break;
			sendPacket.WriteData( &UserGiftList.m_Gift[ui8StartIdx]._i32GiftWareDBIdx,	sizeof(TGiftDxIdx));		// 선물 DB Idx.
			sendPacket.WriteData( &UserGiftList.m_Gift[ui8StartIdx]._ui32GoodsID,		sizeof(UINT32));			// 선물 GoodsID.
			sendPacket.WriteData( &UserGiftList.m_Gift[ui8StartIdx]._eDeliver,			sizeof(ENUM_TYPE_GIFT_DELIVER));	// 선물 지급 여부
			sendPacket.WriteData( &UserGiftList.m_Gift[ui8StartIdx]._ui32Expire,		sizeof(UINT32));		// 선물 만료 날짜.

			ui8SenderNickSize = (UINT8)(i3String::Length(UserGiftList.m_Gift[ui8StartIdx]._strSenderNick ) + 1 );
			ui8SenderNickSize = MIN( ui8SenderNickSize, MAX_USER_NICK );
			ui8MessageSize = (UINT8)(i3String::Length(UserGiftList.m_Gift[ui8StartIdx]._strMessage ) + 1 );
			ui8MessageSize = MIN( ui8MessageSize, MAX_GIFT_MESSAGE_SIZE );

			sendPacket.WriteData( &ui8SenderNickSize,				sizeof(UINT8));		// 선물 보낸 사람 닉네임 사이즈.
			sendPacket.WriteData( UserGiftList.m_Gift[ui8StartIdx]._strSenderNick,		ui8SenderNickSize );		// 선물 보낸 사람 닉네임.
			sendPacket.WriteData( &ui8MessageSize,					sizeof(UINT8));		// 선물 보낸 사람 닉네임 사이즈.
			sendPacket.WriteData( UserGiftList.m_Gift[ui8StartIdx]._strMessage,			ui8MessageSize );				// 선물 메시지.

			ui8StartIdx++;
		}
	}
	pSession->SendPacketMessage(&sendPacket);
}


void CUserSessionManager::OnGiftNotify_A(INT32 i32SessionIdx, TUID i64UID, GIFT_BUFFER GiftInfo )
{
	CUserSession * pSession = GetSession(i32SessionIdx, i64UID);
	if(pSession == NULL)	return;

	i3NetworkPacket sendPacket(PROTOCOL_AUTH_SHOP_NOTIFY_GIFT_ACK);


	sendPacket.WriteData( &GiftInfo._i32GiftWareDBIdx,	sizeof(TGiftDxIdx));
	sendPacket.WriteData( &GiftInfo._ui32GoodsID,		sizeof(UINT32));
	sendPacket.WriteData( &GiftInfo._eDeliver,			sizeof(ENUM_TYPE_GIFT_DELIVER));
	sendPacket.WriteData( &GiftInfo._ui32Expire,		sizeof(UINT32));

	UINT8 ui8SenderNickSize = (UINT8)( i3String::Length(GiftInfo._strSenderNick) + 1 ) ;	MIN( ui8SenderNickSize,	NET_NICK_NAME_SIZE );
	sendPacket.WriteData( &ui8SenderNickSize,			sizeof(UINT8));
	sendPacket.WriteData( GiftInfo._strSenderNick,		ui8SenderNickSize);

	UINT8 ui8MessageSize = (UINT8)( i3String::Length(GiftInfo._strMessage) + 1 ) ;	MIN( ui8MessageSize,	MAX_GIFT_MESSAGE_SIZE );
	sendPacket.WriteData( &ui8MessageSize,				sizeof(UINT8));
	sendPacket.WriteData( GiftInfo._strMessage,			ui8MessageSize);

	pSession->SendPacketMessage(&sendPacket);
}

/*----------------------------------------------------------
Name : OnFriendInviteUser
Desc : 친구 초대하기 (다른서버)
----------------------------------------------------------*/
void CUserSessionManager::OnFriendInviteUserServer(UINT32 ui32SessionIdx, TUID i64UID, UINT8 ui8FriendIdx)	
{
	CUserSession * pInvitedSession = GetSession(ui32SessionIdx, i64UID); 	

	if(pInvitedSession !=NULL)
	{
		i3NetworkPacket SendPacket( PROTOCOL_AUTH_FRIEND_INVITED_REQUEST_ACK );
		SendPacket.WriteData(&ui8FriendIdx, sizeof(UINT8)); 	
		pInvitedSession->SendPacketMessage( &SendPacket );						
	}
	return; 
}

void CUserSessionManager::OnSendNoticeMessage( INT32 i32ThreadIdx, INT32 i32Type, char * pstrMessage, UINT32 ui32len)
{	
	i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_ANNOUNCE); 
	SendPacket.WriteData(&i32Type,		sizeof(INT32)); 
	SendPacket.WriteData(&ui32len,		sizeof(UINT32)); 
	SendPacket.WriteData(pstrMessage,	(UINT16)ui32len); 

	//OnSendAllUser( &SendPacket );
	AnnounceMessagePush( i32ThreadIdx, SendPacket.GetProtocolID(), SendPacket.GetDataFieldSize(), SendPacket.GetFieldBuffer() );

	return; 
}

void CUserSessionManager::OnSendAllUser( i3NetworkPacket* pPacket )
{
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < g_pContextMain->m_i32SessionCount; i++)
	{
		pUserSession = (CUserSession*)m_pSessionList[i];

		if(!pUserSession->GetIsActive())					continue;
		//1. DisConnect User
		if(pUserSession->m_ui32ATIsKill > 0)				continue;
		//2. LogIn Time Out For Auth Server Connect 
		if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)	continue; 
		//4 Send Message
		pUserSession->SendPacketMessage( pPacket ); 
	}
	return; 
}

void CUserSessionManager::KickAllSession(void)
{
	m_MaxSessionIdx		= 0;	// 더이상 유저가 붙지 않게 하기 위해 추가하였습니다.

	CUserSession * pUserSession; 

	for(INT32 i = 0; i < g_pContextMain->m_i32SessionCount; i++)
	{
		pUserSession = (CUserSession*)m_pSessionList[i];
		if( NULL == pUserSession )					continue;
		if( FALSE == pUserSession->GetIsActive())	continue;

		// ModuleControlSocket 에서 호출
		pUserSession->_SendErrorMessage( MODULE_CONTROL_THREAD_IDX, EVENT_ERROR_EVENT, 0, KILL_REASON_USERKICK );
	}
	return; 
}

void	CUserSessionManager::_SetItemAbilityAuth_A( CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32ItemArg )
{
	switch( GET_ITEM_FLAG_TYPE( ui32ItemID ) )
	{
	// 위장 계급
	case CASHITEM_GROUP_DISGUISE_RANK:
		{	
			if( ITEM_CLASS_EXPENDABLE != GET_ITEM_FLAG_CLASS( ui32ItemID ) )	return;
			// 계급 바꿔준다.
			pSession->SetChangeRank( MODULE_TRANS_THREAD_IDX, pSession->m_UserInfoBasic._rank, pSession->m_UserInfoInvItemData._wRank );
		}
		break;
	// 임시 닉네임
	case CASHITEM_GROUP_DISGUISE_NICK:
		{	
			if( ITEM_CLASS_EXPENDABLE != GET_ITEM_FLAG_CLASS( ui32ItemID ) )	return;
			// 닉네임 바꿔준다.
			pSession->SetChangeNick( MODULE_TRANS_THREAD_IDX, pSession->m_UserInfoBasic._nick, pSession->m_UserInfoInvItemData._cNick );
		}
		break;
	// 컬러 채팅
	case CASHITEM_GROUP_COLOR_CHATTING:
		{
			if( ITEM_CLASS_MAINTENANCE != GET_ITEM_FLAG_CLASS( ui32ItemID ) )	return;

			pSession->InvItemData();
		}
		break;
	// 컬러 닉네임
	case CASHITEM_GROUP_COLOR_NICK:
		{
			if( ITEM_CLASS_MAINTENANCE != GET_ITEM_FLAG_CLASS( ui32ItemID ) )	return;

			//pSession->InvItemData();
			// 닉네임 칼라
			pSession->SetChangeColorNick( MODULE_TRANS_THREAD_IDX, pSession->m_UserInfoBasic._nickcolor );
		}
		break;
	// 컬러 크로스헤어
	case CASHITEM_GROUP_COLOR_CROSSHAIR:
		{
			if( ITEM_CLASS_EXPENDABLE != GET_ITEM_FLAG_CLASS( ui32ItemID ) )	return;

			pSession->m_UserInfoInvItemData._ui8Item |= SALE_ITEM_COLOR_CROSSHAIR;
			pSession->InvItemData();
		}
		break;
	// 클랜 150%
	case CASHITEM_GROUP_CLAN_XP_PLUS_150:
	case CASHITEM_GROUP_CLAN_XP_PLUS_150_EV:
		{	// 클랜서버에 알려준다.

			if( ITEM_CLASS_MAINTENANCE != GET_ITEM_FLAG_CLASS( ui32ItemID ) )	return;

			// ItemID를 12로 시작하도록 변경해서 보냅니다. 2011.03.22 노준영
			ui32ItemID = MAKE_ITEM_FLAG( ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE , CASHITEM_GROUP_CLAN_XP_PLUS_150 , CASHITEM_DAY_NO );
			g_pModuleClan->ClanSendExpUpRequest( MODULE_TRANS_THREAD_IDX, pSession, ui32ItemID, ui32ItemArg );
		}		
		break;
	// 클랜 컬러
	case CASHITEM_GROUP_COLOR_CLAN:
		{	// 클랜 컬러는 후에 추가 작업
			if( ITEM_CLASS_MAINTENANCE != GET_ITEM_FLAG_CLASS( ui32ItemID ) )	return;

			// ItemID를 12로 시작하도록 변경해서 보냅니다. 2011.03.22 노준영
			ui32ItemID = MAKE_ITEM_FLAG( ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE , CASHITEM_GROUP_COLOR_CLAN , CASHITEM_DAY_NO );
			g_pModuleClan->ClanSendClanColorRequest(MODULE_TRANS_THREAD_IDX, pSession, ui32ItemID, ui32ItemArg, 0 );
		}
		break;
	}
}

void CUserSessionManager::OnSendSupplyBoxAnnounceMessage( INT32 i32ThreadIdx, SUPPLY_BOX_ANNOUNCE_SETTING *pstSupplyBoxAnnounce)
{
	AnnounceMessagePush( i32ThreadIdx, PROTOCOL_BASE_SUPPLY_BOX_ANNOUNCE_ACK, sizeof(SUPPLY_BOX_ANNOUNCE_SETTING), (const char *) pstSupplyBoxAnnounce );

	return; 
}

void CUserSessionManager::OnSendSupplyBoxPresentMessage( INT32 i32ThreadIdx, UINT8 ui8PresentMessage, UINT32 ui32ServerIdx, UINT32 ui32ChannelNum, UINT32 ui32RoomIdx, UINT32 ui32ItemID, UINT8 ui8Size, char *strNick)
{
	i3NetworkPacket	SendPacket( PROTOCOL_BASE_SUPPLY_BOX_PRESENT_MESSAGE_ACK );
	SendPacket.WriteData( &ui8PresentMessage,	sizeof(UINT8) );
	SendPacket.WriteData( &ui32ServerIdx,		sizeof(UINT32) );
	SendPacket.WriteData( &ui32ChannelNum,		sizeof(UINT32) );
	SendPacket.WriteData( &ui32RoomIdx,			sizeof(UINT32) );
	SendPacket.WriteData( &ui32ItemID,			sizeof(UINT32) );
	SendPacket.WriteData( &ui8Size,				sizeof(UINT8) );
	SendPacket.WriteData( strNick,				ui8Size	);

	switch( ui8PresentMessage )
	{
	case SUPPLY_BOX_MESSAGE_ALL :
	case SUPPLY_BOX_MESSAGE_SERVER :
		AnnounceMessagePush( i32ThreadIdx, SendPacket.GetProtocolID(), SendPacket.GetDataFieldSize(), SendPacket.GetFieldBuffer() );
		break;
	case SUPPLY_BOX_MESSAGE_CHANNEL :
		AnnounceMessagePush( i32ThreadIdx, SendPacket.GetProtocolID(), SendPacket.GetDataFieldSize(), SendPacket.GetFieldBuffer(), ANNOUNCE_MESSAGE_CHANNEL, ui32ChannelNum );
		break;
	case SUPPLY_BOX_MESSAGE_ROOM :
		AnnounceMessagePush( i32ThreadIdx, SendPacket.GetProtocolID(), SendPacket.GetDataFieldSize(), SendPacket.GetFieldBuffer(), ANNOUNCE_MESSAGE_ROOM, ui32ChannelNum, ui32RoomIdx );
		break;
	}
}

void CUserSessionManager::OnSendChannelUser( INT32 i32ChannelNum, i3NetworkPacket* pPacket )
{
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < g_pContextMain->m_i32SessionCount; i++)
	{
		pUserSession = (CUserSession*)m_pSessionList[i];

		if(!pUserSession->GetIsActive())					continue;
		//1. DisConnect User
		if(pUserSession->m_ui32ATIsKill > 0)				continue;
		//2. LogIn Time Out For Auth Server Connect 
		if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)	continue; 
		//3. Channel Check
		if( pUserSession->m_i32ChannelNum != i32ChannelNum ) continue;
		//4 Send Message
		pUserSession->SendPacketMessage( pPacket );
	}
	return; 
}

void CUserSessionManager::AnnounceMessagePush( INT32 i32ThreadIdx, UINT32 ui32Protocol, UINT32 ui32BufferSize, const char * pBuffer, UINT8 ui8Target, UINT32 ui32ChannelNum, UINT32 ui32RoomIdx, UINT32 ui32SessionIdx )
{
	NSM_CRingBuffer * pRingBuffer = NULL;

	char strLog[256];
	if( ui32BufferSize > ANNOUNCE_MESSAGE_BUFFER_SIZE )	
	{ 
		i3String::Format(strLog, 256, "Over sized Announce message requested (Protocol:%d)", ui32Protocol);
		g_pModuleLogFile->Write_M_Log(i32ThreadIdx, NULL, strLog);
		return; 
	}

	switch(i32ThreadIdx)
	{
	case MAIN_THREAD_IDX:
		{
			if( m_pRingAnnounceMain->GetEmptyBufferCount() == 0 )
			{
				i3String::Format(strLog, 256, "Announce message buffer full (Protocol:%d)", ui32Protocol);
				g_pModuleLogFile->Write_M_Log(i32ThreadIdx, NULL, strLog);
				return;
			}

			pRingBuffer = m_pRingAnnounceMain;
		}
		break;
	case MODULE_CONTROL_THREAD_IDX:
		{
			if( m_pRingAnnounceControl->GetEmptyBufferCount() == 0 )
			{
				i3String::Format(strLog, 256, "Announce message buffer full (Protocol:%d)", ui32Protocol);
				g_pModuleLogFile->Write_M_Log(i32ThreadIdx, NULL, strLog);
				return;
			}

			pRingBuffer = m_pRingAnnounceControl;
		}
		break;
	case MODULE_TRANS_THREAD_IDX:
		{
			if( m_pRingAnnounceTrans->GetEmptyBufferCount() == 0 )
			{
				i3String::Format(strLog, 256, "Announce message buffer full (Protocol:%d)", ui32Protocol);
				g_pModuleLogFile->Write_M_Log(i32ThreadIdx, NULL, strLog);
				return;
			}

			pRingBuffer = m_pRingAnnounceTrans;
		}
		break;
	}

	// 쓰레드에 이상한 값이오면 크래쉬 합니다.
	if( NULL == pRingBuffer )	return;

	ANNOUNCE_MESSAGE * pMessage;
	pMessage = (ANNOUNCE_MESSAGE *) pRingBuffer->PushPointer();
	if( pMessage == NULL ) return;

	pMessage->ui32Protocol		= ui32Protocol;
	pMessage->ui8Target			= ui8Target;
	pMessage->ui32ChannelNum	= ui32ChannelNum;
	pMessage->ui32RoomIdx		= ui32RoomIdx;
	pMessage->ui32SessionIdx	= ui32SessionIdx;
	pMessage->ui32BufferSize	= ui32BufferSize;
	i3mem::Copy( pMessage->strBuffer, pBuffer, ui32BufferSize );

	pRingBuffer->PushPointerIdx();
}

void CUserSessionManager::AnnounceMessageCollect(NSM_CRingBuffer *pRingBuffer)
{
	INT32 i32Count =  pRingBuffer->GetBufferCount();
	for( INT32 i = 0 ; i < i32Count; i++ )
	{
		if( m_pRingAnnounceMessage->GetEmptyBufferCount() == 0)
		{
			char strLog[256];
			i3String::Format(strLog, 256, "Announce message buffer full");
			g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, strLog);
			return;
		}

		i3mem::Copy( m_pRingAnnounceMessage->PushPointer(), pRingBuffer->Pop(), sizeof(ANNOUNCE_MESSAGE) );
		m_pRingAnnounceMessage->PushPointerIdx();
		pRingBuffer->PopIdx();
	}
}

bool CUserSessionManager::OnSendAnnounceMessage( ANNOUNCE_MESSAGE * pmessageInfo )
{
	// 설정된 메시지를 다 보냈으면 TRUE 리턴, 보내야 할 대상이 남아있으면 FALSE 리턴.

	// 보내는 중인 메시지가 없었으면 새로 들어온 메시지를 전달 패킷에 채운다.
	if( m_bSendAnnounce == FALSE )
	{
		m_AnnounceMessage.Clear();

		m_AnnounceMessage.SetProtocol( pmessageInfo->ui32Protocol );

		m_AnnounceMessage.WriteData( pmessageInfo->strBuffer, pmessageInfo->ui32BufferSize );

		m_bSendAnnounce = TRUE;
		m_i32AnnouncedUserIdx = 0;
	}

	// 보내는 범위에 따라 패킷 전송
	bool bSendEnd = FALSE;
	switch( pmessageInfo->ui8Target )
	{
	case ANNOUNCE_MESSAGE_ALL:
		{
			// #define SEND_COUNT_ANNOUNCE_MESAGE 수만큼 한번에 전달
			INT8 i8Count = 0;

			if( m_i32AnnouncedUserIdx < g_pContextMain->m_i32SessionCount) 
			{
				for(m_i32AnnouncedUserIdx; m_i32AnnouncedUserIdx < g_pContextMain->m_i32SessionCount; m_i32AnnouncedUserIdx++)
				{
					if(i8Count >= SEND_COUNT_ANNOUNCE_MESSAGE)break;
					CUserSession *	pUserSession = (CUserSession*)m_pSessionList[m_i32AnnouncedUserIdx];
					if( FALSE == pUserSession->GetIsActive() )			continue; 
					if(pUserSession->m_ui32ATIsKill > 0)				continue;
					if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)	continue; 
					pUserSession->SendPacketMessage(&m_AnnounceMessage);
					i8Count++; 
				}
			}
			
			// 모든 유저에게 보내졌으면
			if( m_i32AnnouncedUserIdx >= g_pContextMain->m_i32SessionCount )	
				bSendEnd = TRUE;
		}
		break;
	case ANNOUNCE_MESSAGE_CHANNEL:
		{
			// #define SEND_COUNT_ANNOUNCE_MESAGE 수만큼 한번에 전달
			if( pmessageInfo->ui32ChannelNum > GAME_CHANNEL_COUNT )	{ bSendEnd = TRUE; break; }

			INT8 i8Count = 0;

			if( m_i32AnnouncedUserIdx < g_pContextMain->m_i32SessionCount) 
			{
				for(m_i32AnnouncedUserIdx; m_i32AnnouncedUserIdx < g_pContextMain->m_i32SessionCount; m_i32AnnouncedUserIdx++)
				{
					if(i8Count >= SEND_COUNT_ANNOUNCE_MESSAGE)break;
					CUserSession *	pUserSession = (CUserSession*)m_pSessionList[m_i32AnnouncedUserIdx];
					if( FALSE == pUserSession->GetIsActive() )							continue; 
					if(pUserSession->m_ui32ATIsKill > 0)								continue;
					if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)					continue; 
					if( pUserSession->m_i32ChannelNum != pmessageInfo->ui32ChannelNum )	continue;
					pUserSession->SendPacketMessage(&m_AnnounceMessage);
					i8Count++; 
				}
			}
			
			// 모든 유저에게 보내졌으면
			if( m_i32AnnouncedUserIdx >= g_pContextMain->m_i32SessionCount )
				bSendEnd = TRUE;
		}
		break;
	case ANNOUNCE_MESSAGE_ROOM:
		{
			// 한 방에 있는 유저에게 모두 전달
			if( pmessageInfo->ui32RoomIdx >= g_pContextMain->m_ui32ChannelSessionCount )	{ bSendEnd = TRUE; break; }

			CRoom* pRoom = g_pRoomManager->GetRoom(pmessageInfo->ui32ChannelNum, pmessageInfo->ui32RoomIdx);
			if( pRoom->m_bIsCreate )
				pRoom->SendAnnounceMessage( &m_AnnounceMessage );

			bSendEnd = TRUE;
		}
		break;
	case ANNOUNCE_MESSAGE_USER:
		{
			if( pmessageInfo->ui32SessionIdx >= (UINT32)g_pContextMain->m_i32SessionCount )	{ bSendEnd = TRUE; break; }

			CUserSession * pUserSession = (CUserSession*)m_pSessionList[ pmessageInfo->ui32SessionIdx ];
			if( FALSE == pUserSession->GetIsActive() )				{ bSendEnd = TRUE; break; }
			if( pUserSession->m_ui32ATIsKill > 0)					{ bSendEnd = TRUE; break; }
			if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)		{ bSendEnd = TRUE; break; }
			pUserSession->SendPacketMessage(&m_AnnounceMessage);

			bSendEnd = TRUE;
		}
		break;
	default:
		bSendEnd = TRUE;
	}

	if( bSendEnd )
	{
		m_AnnounceMessage.Clear();
		m_bSendAnnounce = FALSE;
		return TRUE;
	}

	return FALSE;
}

/*----------------------------------------------------------
Name : OnCouponAwardItem_A
Desc : 쿠폰번호를 이용한 보상 아이템 내려받기
----------------------------------------------------------*/
void CUserSessionManager::OnCouponAwardItem_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32ItemID, char* pstrCouponNum, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;

	// 임무카드를 보상으로 줄 수도 있는가?
	// 튜토리얼,휴먼,공룡튜토리얼,이벤트카드는 무료지급이기 때문에 대상에서 제외됨
	// bQuestCard가 False일 경우 복구를 위해서는 게임서버의 로그를 확인할 필요가 있음
	// bool bQuestCard = pSession->SendBuyQuestCardResult( Rv, ui32GoodsID, MODULE_TRANS_THREAD_IDX, pInvenItem->_i64ItemWareDBIdx, 0, 0 );
	bool bQuestCard = false;

	if(EV_SUCCESSED( i32Rv ))
	{
		for( UINT32 i = 0 ; i < ui16ItemCount ; i++ )
		{
			_OnInsertItem( pSession, &pInvenItem[ i ], &InvenSet );
		}
		
		//쿠폰 번호를 통한 보상 아이템 로그 남기기(성공한 경우에만 로그를 남긴다)
		//g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_COUPON_AWARD_ITEM, pSession, &ui32ItemID, strCouponNum );
	}

	pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	if( false == bQuestCard )
	{	// Quest 카드를 보상으로 받은 경우는 제외
		UINT32 ui32InvenTime = g_pContextMain->GetInvenTime();
		i3NetworkPacket SendPacket( PROTOCOL_AUTH_COUPON_AWARD_ITEM_ACK );
		SendPacket.WriteData( &i32Rv,									sizeof(INT32));	
		SendPacket.WriteData( &ui32InvenTime,							sizeof(UINT32));
		if(EV_SUCCESSED( i32Rv ))
		{
			SendPacket.WriteData( &InvenSet.ui32CharaCount,				sizeof(UINT32));
			SendPacket.WriteData( &InvenSet.ui32WeaponCount,			sizeof(UINT32));
			SendPacket.WriteData( &InvenSet.ui32ItemCount,				sizeof(UINT32));

			if( InvenSet.ui32CharaCount )
				SendPacket.WriteData( InvenSet.aCharaBuffer,				(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32CharaCount) );
			if( InvenSet.ui32WeaponCount )
				SendPacket.WriteData( InvenSet.aWeaponBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32WeaponCount) );
			if( InvenSet.ui32ItemCount )
				SendPacket.WriteData( InvenSet.aItemBuffer,				(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32ItemCount) );
		}

		pSession->SendPacketMessage( &SendPacket );
	}

	char strLog[256];
	i3String::Format( strLog, 256, "PROTOCOL_AUTH_COUPON_AWARD_ITEM_ACK(RV:%d)",i32Rv);
	g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL, strLog);

	return; 
}


/*----------------------------------------------------------
Name : OnRSGoodsBuy_A
Desc : 구입하기
----------------------------------------------------------*/

void CUserSessionManager::OnRSGoodsBuy_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, RS_TAB eTabIdx, UINT32 ui32TabGoodsID, UINT32 ui32PrizeGoodsID, UINT16 ui16ItemCount, INVEN_BUFFER* pInvenItem,  UINT32 ui32Cash, char* pstrRedirectUrl, RS_ITEM_GRADE eItemGrade, UINT8 ui8RsIdx,  UINT8 ui8TabUseCount, UINT32 ui32RSLastBaseInfoTime, RS_BASE_INFO* pRsBaseInfo, RS_RESULT_TYPE eResultType )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if(pSession == NULL)	return;

#ifdef I3_DEBUG
	// 테스트용 로그 - 뽑기 실패 이유 로그 남기기.
	if( EV_FAILED( i32Rv ) )
	{
		char strLog[MAX_PATH];
		char strReason[MAX_PATH];
		switch ( i32Rv ) 
		{
		case EVENT_ERROR_RS_START_FAIL_ALREADY_PLAY :  i3String::Copy( strReason,"RS_START_FAIL_ERROR_ALREADY_PLAY", MAX_PATH ); break;
		case EVENT_ERROR_RS_START_FAIL_WRONG_ROULETTE :  i3String::Copy( strReason,"RS_START_FAIL_ERROR_WRONG_ROULETTE", MAX_PATH ); break;
		case EVENT_ERROR_RS_START_FAIL_CASH_BALANCE :  i3String::Copy( strReason,"RS_START_FAIL_ERROR_CASH_BALANCE", MAX_PATH ); break;
		case EVENT_ERROR_RS_START_FAIL_RS_ST_NULL	 :  i3String::Copy( strReason,"RS_START_FAIL_ERROR_RS_ST_NULL", MAX_PATH ); break;
		case  EVENT_ERROR_RS_START_FAIL_RS_INSERT_FALSE	 :  i3String::Copy( strReason,"RS_START_FAIL_ERROR_RS_INSERT_FALSE", MAX_PATH ); break;
		case EVENT_ERROR_RS_START_FAIL_WRONG_PRIZE_GOODSID		 :  i3String::Copy( strReason,"RS_START_FAIL_ERROR_WRONG_PRIZE_GOODSID", MAX_PATH ); break;
		case EVENT_ERROR_RS_FAIL_NETWORK  :  i3String::Copy( strReason,"RS_START_FAIL_ERROR_NETWORK", MAX_PATH ); break;
		default : i3String::Copy( strReason,"RS_START_FAIL_ERROR_UNKNOWN", MAX_PATH ); break;
		}
		i3String::Format( strLog, MAX_PATH, "[OnRSGoodsBuy_A()] RS RESULT FAIL / Reason : %s / UIO : %I64d ",strReason, i64UID);
		g_pModuleLogFile->Write_M_Log(MODULE_TRANS_THREAD_IDX,NULL, strLog);
	}
#endif

	INVEN_BUFFER_SET InvenSet; 
	
	// 성공 실패 여부에 따라 금액을 먼저 지급합니다.
	if( EV_SUCCESSED( i32Rv ) )
	{
		pSession->m_UserInfoBasic._cash		= ui32Cash;
		if( ui32Cash > pSession->m_UserInfoBasic._NCCoin )
		{
			pSession->m_UserInfoBasic._HappyCoin = ui32Cash - pSession->m_UserInfoBasic._NCCoin;
		}
		else
		{
			pSession->m_UserInfoBasic._HappyCoin = 0;
			pSession->m_UserInfoBasic._NCCoin = ui32Cash;
		}
	}

	pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

	if( EV_SUCCESSED( i32Rv ) )
	{
		// 어떤 아이템을 샀는지 확인 합니다.
		for( UINT32 i = 0 ; i < ui16ItemCount ; i++ )
		{
			if( ITEM_CLASS_POINT != GET_ITEM_FLAG_CLASS ( pInvenItem[ i ]._ui32ItemID )  )	// 포인트 아이템은 건너 뛴다.
				pSession->OnInsertItem( &pInvenItem[ i ], &InvenSet );
		}

		//ZLog 에 기록 상점 로그 보내기 (구입)
		UINT8 ui8SpendType = (UINT8)eResultType;
		UINT8 ui8BuyType = GOODS_BUY_RS ;
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_SHOP_BUY, pSession, &ui32TabGoodsID, &ui8BuyType, &ui8SpendType, &ui32PrizeGoodsID );

		// 마지막 캐시 아이템 구입 날짜 저장
		pSession->m_UserInfoBasic._ui32LastBuyCash = g_pContextMain->GetInvenTime();

	}
	
	i3NetworkPacket SendPacket( PROTOCOL_RS_ROULETTE_RESULT_ACK );
	SendPacket.WriteData( &i32Rv,								sizeof(INT32));	
	if(EV_SUCCESSED( i32Rv ))
	{
		SendPacket.WriteData( &eTabIdx,								sizeof(RS_TAB));		// 탭번호
		SendPacket.WriteData( &eItemGrade,							sizeof(RS_ITEM_GRADE));	// 등급(별갯수)
		SendPacket.WriteData( &ui32PrizeGoodsID,					sizeof(UINT32));		// 당첨 상품 GoodsID
		SendPacket.WriteData( &ui8RsIdx,							sizeof(UINT8));			// Rs Idx
		SendPacket.WriteData( &ui8TabUseCount,						sizeof(UINT8));			// 탭 이용 횟수.
		SendPacket.WriteData( &pSession->m_UserInfoBasic._cash,		sizeof(UINT32));		// 캐쉬잔액	
		SendPacket.WriteData( &InvenSet.ui32CharaCount,				sizeof(UINT32));
		SendPacket.WriteData( &InvenSet.ui32WeaponCount,			sizeof(UINT32));
		if( GOODS_CLASS_POINT == GET_GOODS_FLAG_CLASS( ui32PrizeGoodsID ) ) // 당첨 상품이 포인트면 인벤 정보를 보내지 않는다.
		{
			 InvenSet.ui32ItemCount = 0;
		}
		SendPacket.WriteData( &InvenSet.ui32ItemCount,				sizeof(UINT32));

		if( InvenSet.ui32CharaCount )
			SendPacket.WriteData( InvenSet.aCharaBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32CharaCount) );
		if( InvenSet.ui32WeaponCount )
			SendPacket.WriteData( InvenSet.aWeaponBuffer,			(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32WeaponCount) );
		if( InvenSet.ui32ItemCount )
			SendPacket.WriteData( InvenSet.aItemBuffer,				(UINT16)(sizeof(INVEN_BUFFER)*InvenSet.ui32ItemCount) );

		SendPacket.WriteData( &ui32RSLastBaseInfoTime,				sizeof(UINT32));		// 기본정보 변동사항 마지막 갱신 날짜.

		SendPacket.WriteData( pRsBaseInfo,							sizeof(RS_BASE_INFO));	// 뽑기 상점 기본 정보.  
		
	} 

	pSession->SendPacketMessage( &SendPacket );

	return; 
}

void CUserSessionManager::OnRSEnter_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT8 ui8Tab1UseCount, UINT8 ui8Tab2UseCount, UINT8 ui8Tab3UseCount, RS_BASE_INFO* pRsBaseInfo )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if(pSession == NULL)	return;

	i3NetworkPacket SendPacket( PROTOCOL_RS_ENTER_ACK );
	SendPacket.WriteData( &i32Rv,				sizeof(INT32));	
	if(EV_SUCCESSED( i32Rv ))
	{
		SendPacket.WriteData( &ui8Tab1UseCount,			sizeof(UINT8));	// 1번탭 뽑기 이용 횟수.
		SendPacket.WriteData( &ui8Tab2UseCount,			sizeof(UINT8));	// 2번탭 뽑기 이용 횟수.
		SendPacket.WriteData( &ui8Tab3UseCount,			sizeof(UINT8));	// 3번탭 뽑기 이용 횟수.
		SendPacket.WriteData( pRsBaseInfo,				sizeof(RS_BASE_INFO));	// 뽑기 상점 기본 정보.
	}

	pSession->SendPacketMessage( &SendPacket );

	return;
}
// 서동권 - 효율성을 위해 방식 변경
//void CUserSessionManager::OnRSSendItemInfo_A(INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, RS_ALL_ITEM_INFO* pItemInfo )
//{
//	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
//	if(pSession == NULL)	return;
//
//	i3NetworkPacket SendPacket( PROTOCOL_RS_ITEM_INFO_ACK );
//	SendPacket.WriteData( &i32Rv,				sizeof(INT32));	
//	if(EV_SUCCESSED( i32Rv ))
//	{
//		SendPacket.WriteData( pItemInfo,					sizeof(RS_ALL_ITEM_INFO));	// 뽑기 상점 아이템 정보.
//	}
//
//	pSession->SendPacketMessage( &SendPacket );
//
//	return;
//}

void CUserSessionManager::UserSetBirthDay_A( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv, UINT32 ui32BirthDay )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if( pSession == NULL ) return;

	pSession->SetBirthDay_A( i32Rv, ui32BirthDay );

}

// 2차계정 ID 중복여부확인 결과를 클라이언트에 전송
void CUserSessionManager::SendRetCheckSID( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if(pSession == NULL)	return;

	i3NetworkPacket SendPacket( PROTOCOL_BASE_SECURITYID_DUPLICATE_ACK );
	SendPacket.WriteData( &i32Rv,	sizeof( INT32 ) );
	pSession->SendPacketMessage( &SendPacket );
}

// 2차예정 ID 생성확인 결과를 클라이언트에 전송
void CUserSessionManager::SendRetCreateSID( INT32 i32SessionIdx, TUID i64UID, INT32 i32Rv )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if(pSession == NULL)	return;

	i3NetworkPacket SendPacket( PROTOCOL_BASE_SECURITYID_CREATE_ACK );
	SendPacket.WriteData( &i32Rv,	sizeof( INT32 ) );
	pSession->SendPacketMessage( &SendPacket );
}