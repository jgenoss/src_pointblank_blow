#include "pch.h"
#include "UserManager.h"
// #include "ModuleLogFile.h"
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
	m_pChannelUserList		= NULL; 
	m_pcsChanneUser			= NULL;

	//로비 유저 
	m_pLobbyUserList		= NULL; 
	m_pcsLobbyUser			= NULL; 

	//채팅 
	m_pcsLobbyChatting		= NULL;		
	m_pLobbyUserInfoSide	= NULL;
	m_ppLobbyUserList0		= NULL;
	m_pLobbyUserCount0		= NULL;
	m_ppLobbyUserList1		= NULL;
	m_pLobbyUserCount1		= NULL;
    
	//Packet 
	m_i32UserListPacketNum	= 0;

	m_i32OriMaxSessionIdx	= 0;

	m_ui32ChannelCount		= 0;
	m_ui32EachChannelMaxCount= 0;
	m_i32TimeLogInUser		= 0;

	m_i32SendAliveUserIdx	= 0;
	m_i32SendAliveUserCycle	= 0;

	m_i32ActiveCount		= 0;

	//m_i32LastUpdate			= -1;

	i3mem::FillZero( m_ui32LinkLoginCount, sizeof(UINT32)*DEF_TRANS_LINK_COUNT_MAX );
	i3mem::FillZero( m_ui32LinkLoginTimeArg, sizeof(UINT32)*DEF_TRANS_LINK_COUNT_MAX );
}

CUserSessionManager::~CUserSessionManager()
{
	OnDestroy(); 
}

BOOL CUserSessionManager::OnCreate(i3List * pSessionList)
{
	if( FALSE == i3NetworkSessionManager::OnCreate( pSessionList ) )		return FALSE;

	//Set Value 
	m_i32SessionCheckIdx		= 0;
	m_i32OriMaxSessionIdx		= m_MaxSessionIdx;
	m_ui32ChannelCount			= GAME_CHANNEL_COUNT;
	m_ui32EachChannelMaxCount	= g_pContextMain->m_ui32ChannelSessionCount;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Channel User 
	m_pChannelUserMaxFlag	= (BOOL*)i3MemAlloc( sizeof( BOOL ) * m_ui32ChannelCount );
	m_pChannelUserList		= (i3List**)  i3MemAlloc( sizeof( i3List * ) * m_ui32ChannelCount );
	m_pcsChanneUser			= (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount );
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_pChannelUserList[i]		= i3List::NewObject();
		m_pChannelUserList[i]->SetOptmizeEnableWhenClear(FALSE);
		m_pChannelUserList[i]->SetAllocationUnit( m_ui32EachChannelMaxCount );
		m_pcsChanneUser[i]			= i3Mutex::NewObject(); 
		m_pChannelUserMaxFlag[i]	= FALSE; 
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Lobby User
	m_pLobbyUserList	= (i3List**)  i3MemAlloc( sizeof( i3List * ) * m_ui32ChannelCount );
	m_pcsLobbyUser		= (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount );
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_pLobbyUserList[i]	= i3List::NewObject();		
		m_pLobbyUserList[i]->SetOptmizeEnableWhenClear(FALSE); 
		m_pLobbyUserList[i]->SetAllocationUnit( m_ui32EachChannelMaxCount );
		m_pcsLobbyUser[i]	= i3Mutex::NewObject(); 
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//Create Lobby User
	m_pcsLobbyChatting = (i3Mutex**) i3MemAlloc( sizeof( i3Mutex* ) * m_ui32ChannelCount );
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)m_pcsLobbyChatting[i] = i3Mutex::NewObject();

	m_pLobbyUserInfoSide = (INT32*) i3MemAlloc(sizeof(INT32) * m_ui32ChannelCount);
	i3mem::FillZero( m_pLobbyUserInfoSide, (sizeof(INT32) * m_ui32ChannelCount) );

	m_ppLobbyUserList0 = (LOBBY_USER_INFO **) i3MemAlloc(sizeof(LOBBY_USER_INFO*) * m_ui32ChannelCount );
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_ppLobbyUserList0[i] = (LOBBY_USER_INFO *) i3MemAlloc(sizeof(LOBBY_USER_INFO) * m_ui32EachChannelMaxCount);
	}
	m_pLobbyUserCount0 = (UINT32*) i3MemAlloc(sizeof(UINT32) * m_ui32ChannelCount);
	i3mem::FillZero( m_pLobbyUserCount0, (sizeof(UINT32) * m_ui32ChannelCount) );

	m_ppLobbyUserList1 = (LOBBY_USER_INFO **) i3MemAlloc(sizeof(LOBBY_USER_INFO *) * m_ui32ChannelCount); 
	for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		m_ppLobbyUserList1[i] = (LOBBY_USER_INFO *) i3MemAlloc(sizeof(LOBBY_USER_INFO) * m_ui32EachChannelMaxCount);
	}
	m_pLobbyUserCount1 = (UINT32*) i3MemAlloc(sizeof(UINT32) * m_ui32ChannelCount);
	i3mem::FillZero( m_pLobbyUserCount1, (sizeof(UINT32) * m_ui32ChannelCount) );

	for( UINT32 i32ServerIdx = 0 ; i32ServerIdx < g_pContextMain->m_ui32GameServerCount ; i32ServerIdx++ )
	{
		for( INT32 i32Buffer = 0 ; i32Buffer < 2 ; i32Buffer++ )
		{
			m_stChannelList[ i32ServerIdx ][i32Buffer ].m_ui8ChannelCount	= (UINT8)m_ui32ChannelCount;
			m_stChannelList[ i32ServerIdx ][i32Buffer ].m_ui16MaxUserCount	= (UINT16)g_pContextMain->m_aui16InitChannelCount[i32ServerIdx];
		}
	}
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
	m_i32AnnouncedUserIdx	= m_MaxSessionIdx;
	m_ui32AnnounceLastSend = 0;

	m_pRingAnnounceMain = new NSM_CRingBuffer;
	if( m_pRingAnnounceMain ) m_pRingAnnounceMain->OnCreate( sizeof(ANNOUNCE_MESSAGE), ANNOUNCE_MESSAGE_BUFFER_COUNT );

	m_pRingAnnounceControl = new NSM_CRingBuffer;
	if( m_pRingAnnounceControl ) m_pRingAnnounceControl->OnCreate( sizeof(ANNOUNCE_MESSAGE), ANNOUNCE_MESSAGE_BUFFER_COUNT );

	m_pRingAnnounceTrans = new NSM_CRingBuffer;
	if( m_pRingAnnounceTrans ) m_pRingAnnounceTrans->OnCreate( sizeof(ANNOUNCE_MESSAGE), ANNOUNCE_MESSAGE_BUFFER_COUNT );

	// 패킷 처리 테스트코드
	m_r64ParseTimeHigh = 0;
	m_ui16ParseTimeHighProtocol = 0;
	m_bParseTimeReset = FALSE;

	return TRUE; 
}

BOOL CUserSessionManager::OnDestroy(void)
{
	////////////////////////////////////////////////////////
	//Release Each Channel List
	for( UINT32 i =0 ; i < m_ui32ChannelCount; i++)
	{
		I3_SAFE_RELEASE( m_pChannelUserList[i] ); 
		I3_SAFE_RELEASE( m_pcsChanneUser[i] ); 
	}
	I3MEM_SAFE_FREE( m_pChannelUserList ); 	
	I3MEM_SAFE_FREE( m_pcsChanneUser ); 
	I3MEM_SAFE_FREE( m_pChannelUserMaxFlag );


	////////////////////////////////////////////////////////
	//Release Each Lobby List 
	for( UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		I3_SAFE_RELEASE( m_pLobbyUserList[i] ); 
		I3_SAFE_RELEASE( m_pcsLobbyUser[i] ); 
	}
	I3MEM_SAFE_FREE( m_pLobbyUserList ); 
	I3MEM_SAFE_FREE( m_pcsLobbyUser ); 

	////////////////////////////////////////////////////////
	//Chatting Lobby User List 
	for( UINT32 i = 0; i < m_ui32ChannelCount; i++)
	{
		I3MEM_SAFE_FREE( m_ppLobbyUserList0[i] ); 
		I3MEM_SAFE_FREE( m_ppLobbyUserList1[i] ); 
		I3_SAFE_RELEASE( m_pcsLobbyChatting[i] ); 
	}
	I3MEM_SAFE_FREE( m_ppLobbyUserList0 ); 
	I3MEM_SAFE_FREE( m_pLobbyUserCount0 ); 
	I3MEM_SAFE_FREE( m_ppLobbyUserList1 ); 
	I3MEM_SAFE_FREE( m_pLobbyUserCount1 ); 
	I3MEM_SAFE_FREE( m_pLobbyUserInfoSide ); 
	I3MEM_SAFE_FREE( m_pcsLobbyChatting ); 
	
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
void CUserSessionManager::SendListInfoPaket( INT32 i32ServerIdx, CUserSession * pUser )
{
	CHANNEL_LIST* pChannelList = &m_stChannelList[ i32ServerIdx ][ m_i32UserListPacketNum ];
	UINT32 ui32EachChannelMaxCount = g_pContextMain->m_ui16NowChannelCount[i32ServerIdx];

	PACKET_BASE_GET_CHANNELLIST_ACK Packet;
	Packet.m_ui8ChannelCount	= pChannelList->m_ui8ChannelCount;
	Packet.m_ui16MaxUserCount	= (UINT16)ui32EachChannelMaxCount;
	Packet.m_ui16NowUserCount.SetValue( pChannelList->m_ui16NowUserCount, 0, GAME_CHANNEL_COUNT );
	UINT8				ui8ChannelBonusCount;
	TYPE_CHANNEL_BONUS	stChannelBonus[ CHANNEL_TYPE_MAX ];
	g_pContextMain->GetChannelBonusInfo( pUser->m_ui8LocaleCode, &ui8ChannelBonusCount, stChannelBonus );
	Packet.m_stChannelBonus.SetValue( stChannelBonus, 0, ui8ChannelBonusCount );
	pUser->S2MOSendEvent( &Packet );
}

/*-----------------------------------------------------------
Name : 
Desc : 크리티컬 섹션 필요없음..정보만 가지고 오는것이기 때문.
-----------------------------------------------------------*/
void CUserSessionManager::_UpdateUserListPacket(void)
{
	INT32 i32NextPacketNum = 0; 
	if( m_i32UserListPacketNum == 0) i32NextPacketNum = 1; 

	CHANNEL_LIST* pChannelList;
	UINT32 ui32EachChannelMaxCount;

	for( UINT32 i32ServerIdx = 0 ; i32ServerIdx < g_pContextMain->m_ui32GameServerCount ; i32ServerIdx++ )
	{
		pChannelList = &m_stChannelList[ i32ServerIdx ][i32NextPacketNum ];

		if( i32ServerIdx == (UINT32)g_pContextMain->m_i32ServerIdx )
		{
			ui32EachChannelMaxCount = g_pContextMain->m_ui16NowChannelCount[i32ServerIdx];

			//각 채널의 카운드를 채크합니다. 
			INT32 i32Count; 
			for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
			{
				i32Count = m_pChannelUserList[i]->GetCount();
				if(i32Count == (INT32)ui32EachChannelMaxCount)
				{
					m_pChannelUserMaxFlag[i] = TRUE; 
				}

				if(m_pChannelUserMaxFlag[i])
				{
					if( i32Count > (INT32)g_pContextMain->m_ui32ChannelCountPer )
					{
						i32Count = ui32EachChannelMaxCount; 
					}
					else
					{
						m_pChannelUserMaxFlag[i] = FALSE; 
					}
				}
				pChannelList->m_ui16NowUserCount[ i ] = (UINT16)i32Count;
			}
		}
		else
		{
			for(UINT32 i = 0; i < m_ui32ChannelCount; i++)
			{
				pChannelList->m_ui16NowUserCount[ i ] = g_pContextMain->m_ui16ChannelUserCount[ i32ServerIdx ][i];
			}
		}
	}

	m_i32UserListPacketNum = i32NextPacketNum; 
	return;
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CUserSessionManager::OnEnterChannel( CUserSession * pUser, UINT32 Channel )
{
	pUser->m_i32ChannelNum		= SERVER_VALUE_UNKNOWN;
	pUser->m_i32LogChannelNum	= SERVER_VALUE_UNKNOWN;

	INT32 i32EnterChannel = EVENT_ERROR_SERVER_USER_FULL; 

	INT32 i32ServerIdx = g_pContextMain->m_i32ServerIdx;
	UINT32 ui32EachChannelMaxCount = g_pContextMain->m_ui16NowChannelCount[i32ServerIdx];

	m_pcsChanneUser[Channel]->Enter(); 
	{
		if( m_pChannelUserList[Channel]->GetCount() < (INT32)ui32EachChannelMaxCount)
		{
			m_pChannelUserList[Channel]->Add(pUser);
			i32EnterChannel = EVENT_ERROR_SUCCESS;
		}		
	}
	m_pcsChanneUser[Channel]->Leave(); 

	return i32EnterChannel; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CUserSessionManager::OnLeaveChannel( CUserSession * pUser, UINT32 Channel)
{	
	//Check 
	//if(Channel > m_iChannelCount)return;
	m_pcsChanneUser[Channel]->Enter(); 
	{
		m_pChannelUserList[Channel]->Remove(pUser); 
	}
	m_pcsChanneUser[Channel]->Leave(); 

	return; 
} 

void CUserSessionManager::OnEnterLobby( CUserSession * pSession, UINT32 Channel )
{
	m_pcsLobbyUser[Channel]->Enter(); 
	{
		m_pLobbyUserList[Channel]->Add(pSession); 
	}
	m_pcsLobbyUser[Channel]->Leave(); 

	return; 
}

void CUserSessionManager::OnLeaveLobby( CUserSession * pSession, UINT32 Channel )
{
	m_pcsLobbyUser[Channel]->Enter(); 
	{
		m_pLobbyUserList[Channel]->Remove(pSession);
	}
	m_pcsLobbyUser[Channel]->Leave(); 

	return; 
}

/*----------------------------------------------------------
Name : OnUpdate 
Desc : 유저들을 상태를확인 합니다. 
----------------------------------------------------------*/
void CUserSessionManager::OnUpdate(void)
{
	CUserSession * pUserSession; 

	UINT32 ui32ATNow				= i3ThreadTimer::GetServerTime();
	UINT32 ui32ATCheckServer		= ui32ATNow - SESSION_CHECK_TIMEOUT_NORMAL;
	UINT32 ui32ATCheckServerChannel	= ui32ATNow - SESSION_CHECK_TIMEOUT_CHANNEL;
	UINT32 ui32ATCheckServerLogIn	= ui32ATNow - SESSION_CHECK_TIMEOUT_LOGIN;
	UINT32 ui32ATCheckServerNotLogIn= ui32ATNow - SESSION_CHECK_TIMEOUT_NOT_LOGIN;
	UINT32 ui32ATCheckServerGetInfo	= ui32ATNow - SESSION_CHECK_TIMEOUT_GETINFO;
	//UINT32 ui32ATKillUser 			= ui32ATNow - SESSION_DISCONNECT_TIME; 
	UINT32 ui32ATGameGuardSend		= ui32ATNow - m_ui32SessionGameGuardUpdateTime;
	UINT32 ui32ATGameGuardTimeOut	= ui32ATNow - SESSION_CHECK_TIMEOUT_GAMEGUARD;

	INT32 i32RoomPageNum			= g_pRoomManager->GetChannelUseRoomCount( g_pContextMain->m_ui32UpdateChannel ); 

	i32RoomPageNum = INT32(i32RoomPageNum / SEND_MAX_ROOM_COUNT);
	if((i32RoomPageNum % SEND_MAX_ROOM_COUNT) ==0)i32RoomPageNum--; 
	if(i32RoomPageNum < 0)i32RoomPageNum = 0;

	//로비 유저를 위한 선 작업
	INT32 i32LobbyUserUpdateSide = m_pLobbyUserInfoSide[g_pContextMain->m_ui32UpdateChannel];
	LOBBY_USER_INFO * pLobbyUserInfo; 	
	UINT32 * pLobbyUserInfCount;
	LOBBY_USER_INFO * pLobbyUserInfo2;
	UINT32 * pLobbyUserInfCount2;
	if(m_pLobbyUserInfoSide[g_pContextMain->m_ui32UpdateChannel] == 0)
	{				
		i32LobbyUserUpdateSide = 1; 
		pLobbyUserInfCount	= m_pLobbyUserCount1; 
		pLobbyUserInfo		= m_ppLobbyUserList1[g_pContextMain->m_ui32UpdateChannel];

		// 현재 사용되고 있는 로비 유저를 석택합니다.
		pLobbyUserInfCount2	= m_pLobbyUserCount0;
		pLobbyUserInfo2		= m_ppLobbyUserList0[g_pContextMain->m_ui32UpdateChannel];
	}
	else
	{
		i32LobbyUserUpdateSide = 0; 
		pLobbyUserInfCount	= m_pLobbyUserCount0; 
		pLobbyUserInfo		= m_ppLobbyUserList0[g_pContextMain->m_ui32UpdateChannel]; 

		// 현재 사용되고 있는 로비 유저를 석택합니다.
		pLobbyUserInfCount2	= m_pLobbyUserCount1; 
		pLobbyUserInfo2		= m_ppLobbyUserList1[g_pContextMain->m_ui32UpdateChannel];
	}
	INT32 LobbyUserCount = 0; 

	// UserList
	INT32 i32UserPageNum = pLobbyUserInfCount2[ g_pContextMain->m_ui32UpdateChannel ] / SEND_MAX_LOBBY_USER_COUNT;
	if( (pLobbyUserInfCount2[ g_pContextMain->m_ui32UpdateChannel ] % SEND_MAX_LOBBY_USER_COUNT) == 0 ) i32UserPageNum--;
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
	for(INT32 i = 0; i < m_i32OriMaxSessionIdx; i++)
	{
		pUserSession = (CUserSession*)m_pSessionList[i];

		if( FALSE == pUserSession->GetIsActive() )continue; 

		// 패킷 처리 테스트코드
		if( m_bParseTimeReset )
		{
			pUserSession->m_r64ParseTimeHigh = 0.f;
			pUserSession->m_ui16ParseTimeHighProtocol = 0;
		}
		else if( pUserSession->m_r64ParseTimeHigh > m_r64ParseTimeHigh)
		{
			m_r64ParseTimeHigh = pUserSession->m_r64ParseTimeHigh;
			m_ui16ParseTimeHighProtocol = pUserSession->m_ui16ParseTimeHighProtocol;
		}

		//0. Check Hacking User
		if( pUserSession->m_ui32ATKillDelay > 0 && !pUserSession->m_bHackKill )
		{
			if (pUserSession->m_ui32ATKillDelay <= ui32ATNow )
			{
				pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, pUserSession->m_ui32ErrorCode, 0, pUserSession->m_ui32KillReason, pUserSession->m_wstrHackNotice );
				pUserSession->m_bHackKill = TRUE;
			}
			continue;
		}

		//1. DisConnect User
		if(pUserSession->m_ui32ATIsKill > 0)
		{
			if (pUserSession->m_ui32ATIsKill <= ui32ATNow)
			{	// 게임서버일때 한번에 킬 해주는 유저를 5 명으로 제한합니다.
				if( ( SERVER_TYPE_GAME == g_pContextMain->m_i32Gateway ) && ( i32KillUser > 5 ) ) continue;

				DisConnectSession(pUserSession, TRUE);

				i32KillUser++;
			}
		}

		//너무 많은 패킷을 던지는지 확인하기 
		if( pUserSession->m_iPacketSecondCount > 40 )
		{
			pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_EVENT , 0, KILL_REASON_PACKET_COUNT, L"PacketSecondCount > 40" );
			continue;
		}

		//2. Normal User Time Out 
		if( pUserSession->m_ui32ATLastRecv < ui32ATCheckServer )
		{
#ifndef I3_DEBUG
			pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_EVENT, 0, KILL_REASON_NORMAL_TIMEOUT );
			continue;
#endif
		}	

		//3. LogIn Time Out For Auth Server Connect 
		switch( pUserSession->GetPosition() )
		{
		// 접속 후 로그인을 던지지 않은 유저
		case GAME_TASK_CONNECT:
			{
				if( pUserSession->m_ui32ATLastRecv < ui32ATCheckServerNotLogIn )
				{
					pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_LOGIN_ERROR, 0, KILL_REASON_LOGTIMEOUT, L"LOGIN FAIL Not Login" );
					continue; 
				}
			}
			break;
		// 현재 로그인 대기중
		case GAME_TASK_LOGIN_ING:
			{
				if( pUserSession->m_ui32ATLoginWaitSend < ui32ATNow )
				{
					pUserSession->m_ui32ATLoginWaitSend = ui32ATNow + SESSION_CHECK_LOGINWAIT_SENDTIME;

					UINT8 ui8ThreadIdx		= pUserSession->m_ui8LoginLinkThreadIdx;
					INT32 i32WaitCount		= 0;
					if( pUserSession->m_ui32LoginWaitCount > m_ui32LinkLoginCount[ ui8ThreadIdx ] )
					{
						i32WaitCount = pUserSession->m_ui32LoginWaitCount - m_ui32LinkLoginCount[ ui8ThreadIdx ];
					}
					UINT32 ui32LoginTimeArg = m_ui32LinkLoginTimeArg[ ui8ThreadIdx ] * i32WaitCount;

					i3NetworkPacket SendPacket( PROTOCOL_BASE_LOGIN_WAIT_ACK );
					SendPacket.WriteData( &i32WaitCount,		sizeof(INT32) );
					SendPacket.WriteData( &ui32LoginTimeArg ,	sizeof(UINT32) );
					pUserSession->SendPacketMessage( &SendPacket );
				}
				// 로그인 요청이 들어간 후 SESSION_CHECK_TIMEOUT_LOGIN 시간동안 응답이 없을 경우 접속을 끊어 줍니다.
				if( pUserSession->m_ui32ATLogin < ui32ATCheckServerLogIn )
				{
					m_i32TimeLogInUser++;
					pUserSession->LogIn_MAU( MAIN_THREAD_IDX, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT2, 0, NULL );	//이부분 메인에서 호출하나.. 에러이기때문에 내부에서 아무런일도 하지 않음
					continue;
				}
			}
			break;
		}

		// Get Info Time Out
		if( 0 < pUserSession->m_ui32ATInfo )
		{
			if( pUserSession->m_ui32ATInfo < ui32ATCheckServerGetInfo )
			{
				pUserSession->SendMyInfo( MAIN_THREAD_IDX, EVENT_ERROR_FAIL );
			}
		}

		//4. Channel Time Out User 
		// 기획실 요청으로 잠시 막음 2008-10-07
		INT32 i32UserChannelNum = pUserSession->GetChannelNumber();
		if( i32UserChannelNum == SERVER_VALUE_UNKNOWN )
		{					
			if( pUserSession->m_ui32ATLastRecv < ui32ATCheckServerChannel )
			{
				pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_CHANNEL_TIMEOUT, 0, KILL_REASON_CHANNELTIMEOUT );
				continue;
			}
		}
	
		//-------------------------------------------------------
		m_i32ActiveCount++;

		//GameGUard Check
		if( FALSE == pUserSession->CheckGameGuard_M( ui32ATGameGuardTimeOut, ui32ATGameGuardSend ) )
			continue;

		// HeartBit2 집계(USE_SERVER_WATCH_CONTENTS)
		if( true == pUserSession->m_bIsRecvHeartBit2 ) { m_ui32RecvHeartBit2Count++; }

		// 하루이용량 제한 방식일 경우 리셋시간이 지났으면 유저의 컨넥타임을 0으로 만듭니다.
		if ( g_pContextMain->m_AASOption.m_ui8AASEnableFlag & AAS_METHOD_DAILY_USAGE )
		{
			if ( g_pContextMain->m_ui16AASCurTime < pUserSession->m_ui16AASConnectTime )
			{
				pUserSession->m_ui16AASAccMin		= 0;
				pUserSession->m_ui16AASConnectTime	= 0;
			}
		}

		// AAS기능이 ON되어 있고, 금지내용중에 ‘로그인되어 있다면 강제로그아웃’이 있는지 검사합니다.
		if ( g_pContextMain->CheckAASBlockON( AAS_BLOCK_FORCE_LOGOUT ) )
		{
			// 유저상태가 채널진입, 로비, 레디룸, 인게임 상태일때만 검사한다.
			if ( pUserSession->GetPosition() < GAME_TASK_INFO )		continue;

			// 유저 강제로 로그아웃시킵니다.
			AAS_CATCH_METHOD eResult = pUserSession->CheckAAS();
			switch( eResult )
			{
			case CATCH_DAILY_USAGE:
			case CATCH_APPOINTED:
				pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_AAS_FORCE_LOGOUT, 0, KILL_REASON_AAS );
				break;
			case CATCH_ACCOUNT_APPOINTED:
				pUserSession->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_AAS_ACCOUNT_APPOINTED_FORCE_LOGOUT, 0, KILL_REASON_AAS );
				break;
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//100ms마다 한번씩  들어옵니다. 		
		// 땡빵코드 입니다. 차후 m_iEachChannelMaxCount 이걸로 만들어야 합니다. 
		if( i32UserChannelNum != (INT32)g_pContextMain->m_ui32UpdateChannel)continue;
		
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// 클랜 매치 팀 카운트 전송
		pUserSession->SendClanMatchTeamCount(i32UserChannelNum);

		//로비에 있는 사람을 체크후 채널별 방리스트를 보냅니다.
		if( pUserSession->IsInLobby() )
		{
			//로비에 있는 유저들을 리스트에 넣는 작업을 합니다. 			
			pLobbyUserInfo[LobbyUserCount].pSession = pUserSession; 
			LobbyUserCount++; 

			//방리스트를 요청한 유저가 있으면 방 리스트를 보내준다. 
			if( pUserSession->m_bRequestRoomList )
			{
				// RoomList
				pUserSession->m_ui32UpdateRoomListPage++;
				if( pUserSession->m_ui32UpdateRoomListPage > (UINT32)i32RoomPageNum ) pUserSession->m_ui32UpdateRoomListPage = 0;

				// UserList
				pUserSession->m_UpdateUserListPage++;
				if( pUserSession->m_UpdateUserListPage > (UINT32)i32UserPageNum ) pUserSession->m_UpdateUserListPage = 0;
				
				g_pRoomManager->OnSendRoomList(pUserSession, i32UserChannelNum, pLobbyUserInfo2, (pLobbyUserInfCount2[ i32UserChannelNum ]) ); 				
				pUserSession->m_bRequestRoomList = FALSE; 
			}
		}

		if (pUserSession->m_bRequestUserList)
		{
			if (GAME_TASK_READY_ROOM == pUserSession->GetPosition())
			{
				if (pUserSession->m_UpdateUserListPage > (UINT32)i32UserPageNum) pUserSession->m_UpdateUserListPage = 0;
				OnSendLobbyUserList(pUserSession, i32UserChannelNum, pLobbyUserInfo2, (pLobbyUserInfCount2[i32UserChannelNum]));
				pUserSession->m_bRequestUserList = FALSE;
			}
		}

		//채팅금지가 끝난 유저들에게 채팅금지 종료 메세지를 보낸다.
		if ( pUserSession->GetChatBlockDate() > 1001010000 && 
			 pUserSession->GetChatBlockDate() < i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() )
		{
			pUserSession->SendGMChatUserNoti( GMCHAT_PENALTY_CHAT, 0 );

			pUserSession->SetChatBlockDate(1001010000);
			pUserSession->ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);
		}
	}

	// 패킷 처리 테스트코드
	if( m_bParseTimeReset )
	{
		m_r64ParseTimeHigh = 0.f;
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

		CUserSession * pAliveUserSessionList[SEND_ALIVEPACKET_PACK];
		UINT8		   ui8AliveUserCount = 0;
		for( INT32 i = 0; i < SEND_ALIVEPACKET_PACK; i++ )
		{
			pUserSession = (CUserSession*)m_pSessionList[ m_i32SendAliveUserIdx ];
			T_UID i64UID = pUserSession->m_i64UID; 

			if( pUserSession->GetIsActive() &&
				i64UID != SERVER_VALUE_UNKNOWN &&
				pUserSession->m_ui32ATIsKill == 0 )
			{
				// 트랜스 서버 부하를 줄이기 위해 Alive 패킷을 보낼 대상을 묶어서 전달
				pAliveUserSessionList[ui8AliveUserCount++] = pUserSession;

				if (!g_pModuleMessenger->MessengerAliveUser(MAIN_THREAD_IDX,	m_i32SendAliveUserIdx, i64UID, pUserSession->GetNickName()))
				{
					g_pLog->WriteLogSession( pUserSession, L"(MessengerAliveUser)#### ALIVE PACKET ChangeDBFlage :%d ", pUserSession->m_i32ChangeDBFlage ) ; 
				}
			}

			m_i32SendAliveUserIdx++; 
			if(m_i32SendAliveUserIdx >= m_MaxSessionIdx)m_i32SendAliveUserIdx = 0; 
		}

		if( ui8AliveUserCount > 0 )
		{
			if( !g_pModuleTrans->AuthSendAliveUser(MAIN_THREAD_IDX, ui8AliveUserCount, pAliveUserSessionList ) )
			{
				for( INT32 i = 0; i < ui8AliveUserCount; i++ )
				{
					g_pLog->WriteLogSession( pUserSession, L"#### ALIVE PACKET ChangeDBFlage :%d ", pAliveUserSessionList[i]->m_i32ChangeDBFlage ) ; 
				}
			}
		}
	}
	else
	{
		m_i32SendAliveUserCycle++;
	}

	pLobbyUserInfCount[g_pContextMain->m_ui32UpdateChannel]   = LobbyUserCount; 
 	m_pLobbyUserInfoSide[g_pContextMain->m_ui32UpdateChannel] = i32LobbyUserUpdateSide;

	m_i32SessionCheckIdx += SESSION_CHECK_COUNT; 
	if( m_i32SessionCheckIdx >= m_MaxSessionIdx )m_i32SessionCheckIdx = 0; 

	//Update 
	_UpdateUserListPacket();

	// 1분 동안 가장 늦게 처리된 프로토콜 기록
	if (i3ThreadTimer::LocalTime().GetMinute() != m_i32UpdateTimeMM)
	{	
		m_i32UpdateTimeMM = i3ThreadTimer::LocalTime().GetMinute();
		// 처리한 패킷이 아예 없는 경우 기록하지 않습니다.
		if (0 < m_ui16ParseTimeHighProtocol && 0.f < m_r64ParseTimeHigh)
		{
			m_i32UpdateTimeMM = i3ThreadTimer::LocalTime().GetMinute();
			g_pLog->WriteLog(L"--------- USER PACKET MOST HIGH TIME PROTOCOL : %d / TIME(Sec) : %.6lf", m_ui16ParseTimeHighProtocol, m_r64ParseTimeHigh);
		}
		m_bParseTimeReset = TRUE;
	}

	return; 
}

void CUserSessionManager::_SendOptionPacket( TYPE_RING_OUT* pOut )
{
	CUserSession* pSession = GetSession( pOut->_i32SessionIdx, pOut->_i64UID );
	if( NULL == pSession )	return;

	PACKET_BASE_GET_OPTION_ACK Packet;
	Packet.m_TResult			= EVENT_ERROR_SUCCESS;
	Packet.m_ui8DefaultKey		= pOut->_ui8DefaultKey;
	if( ENV_SETTING_DEFAULT != pOut->_ui8DefaultKey )
	{
		Packet.m_NetEnvSet		= pOut->_NetEnvSet;
		Packet.m_KeyMapArray.SetValue( pOut->_KeyMapArray, 0, GAME_KEY_MAP_COUNT );
		for( INT32 i = 0 ; i < MACRO_MAX_COUNT ; i++ )
		{
			Packet.m_strMacro[i] = pOut->_szMacro[i];
		}
	}

	pSession->S2MOSendEvent( &Packet );
}

/*----------------------------------------------------------
Name : 
Desc :  
----------------------------------------------------------*/
void CUserSessionManager::OnSendLobbyChatting( CUserSession * pSender, TTCHAR * pMessage, UINT16 ui16size)
{
	//채팅 사용함.
	INT32				i32ChannelNum = pSender->m_i32ChannelNum;
	
	m_pcsLobbyChatting[i32ChannelNum]->Enter();
	{
		LOBBY_USER_INFO	*	pLobbyUserList; 
		UINT32				ui32LobbyUserCount;

		if( m_pLobbyUserInfoSide == 0)
		{
			pLobbyUserList = m_ppLobbyUserList0[i32ChannelNum];
			ui32LobbyUserCount = m_pLobbyUserCount0[i32ChannelNum];
		}
		else 
		{
			pLobbyUserList = m_ppLobbyUserList1[i32ChannelNum];
			ui32LobbyUserCount = m_pLobbyUserCount1[i32ChannelNum];
		}

		UINT8 ui8NickSize = (UINT8)i3String::Length( pSender->m_UserInfoBasic.m_strNick ) + 1;
		bool  bIsGMUser	  = pSender->CheckAuthLevel();

		i3NetworkPacket LobbyChat(PROTOCOL_LOBBY_CHATTING_ACK);
		LobbyChat.WriteData(&pSender->m_SessionIdx,					sizeof(INT32));
		LobbyChat.WriteData(&ui8NickSize,							sizeof(UINT8));
		LobbyChat.WriteData(pSender->m_UserInfoBasic.m_strNick,		sizeof(TTCHAR) * ui8NickSize );
		LobbyChat.WriteData(&pSender->m_UserInfoInvItemData._ui8NickColor,	sizeof(UINT8));
		LobbyChat.WriteData(&bIsGMUser,								sizeof(bool));
		LobbyChat.WriteData(&ui16size,								sizeof(UINT16));
		LobbyChat.WriteData(pMessage,								sizeof(TTCHAR) * ui16size); 

		if( pSender->m_ui8LocaleCode == LOCALE_CODE_RUSSIA )
		{
			INT32 i32ChattingType = ZLOG_CHATTING_TYPE_LOBBY;
			g_pModuleContextNC->LogSendMessage(pSender->m_WorkThreadIdx, LOG_MESSAGE_U_CHATTING, pSender, &i32ChattingType, pMessage);
		}
		
		for(UINT32 i = 0; i < ui32LobbyUserCount; i++)
		{
			// 클랜 매치 중인 놈들에게는 보내지 않음
			if( pLobbyUserList[i].pSession->IsJoinMatchTeam() ) continue;

			pLobbyUserList[i].pSession->SendPacketMessage(&LobbyChat); 
		}
	}
	m_pcsLobbyChatting[i32ChannelNum]->Leave(); 
	
	return; 
}

// ----------------------------------------------------------
// Name : LogInUser_A
// Desc : Auth 서버로 부터 로그인을 받는다. 
// ----------------------------------------------------------
CUserSession *  CUserSessionManager::LogInUser_A( INT32 i32SessionIdx, INT32 i32Rv, UINT32 ui32AuthKey, UINT32 ui32BlockDate, LOGIN_INFO* pLoginInfo )
{
	// Session 이 정상인지 체크 합니다.
	if( (0 > i32SessionIdx) || (i32SessionIdx >= m_MaxSessionIdx) )
	{
		g_pLog->WriteLog( L"[ERROR] LogInUser_A : <Break Session> Session:%d, rv:%d", i32SessionIdx, i32Rv );
		return NULL; 
	}

	CUserSession * pUserSession = (CUserSession*)m_pSessionList[ i32SessionIdx ];
	if( NULL == pUserSession )
	{
		g_pLog->WriteLog( L"[ERROR] LogInUser_A : <NULL Session> Session:%d, rv:%d", i32SessionIdx, i32Rv );

		if( EV_SUCCESSED( i32Rv ) )
		{
			g_pModuleTrans->TransSendAccountKick( MODULE_TRANS_THREAD_IDX, pLoginInfo->m_TUID );
		}
		return NULL;
	}

	if( FALSE == pUserSession->GetIsActive() )
	{
		g_pLog->WriteLog( L"[ERROR] LogInUser_A : <Not Active> Session:%d, rv:%d", i32SessionIdx, i32Rv );
		return NULL;
	}

	// 로그인 인증 키값을 체크합니다.
	if( pUserSession->m_ui32LoginKey != ui32AuthKey )			
	{	// 에러값 리턴해 줍니다.
		g_pLog->WriteLog( L"[ERROR] LogInUser_A : <Not AuthKey> Session:%d, OrgKey:%d, NewKey:%d", i32SessionIdx, pUserSession->m_ui32LoginKey, ui32AuthKey );

		i32Rv = EVENT_ERROR_LOGIN_BREAK_SESSION;
	}

	pUserSession->LogIn_MAU( MODULE_TRANS_THREAD_IDX, i32Rv, ui32BlockDate, pLoginInfo );

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
	if( i32SessionIdx >= m_MaxSessionIdx)	return;

	CUserSession * pUserSession = (CUserSession*)m_pSessionList[ i32SessionIdx ];
	if( NULL == pUserSession )								return;
	if( FALSE == pUserSession->GetIsActive() )				return;
	// 로그인 인증 키값을 체크합니다.
	if( pUserSession->m_ui32LoginKey != ui32AuthKey )		return;

	// 데이터를 저장합니다.
	if( DEF_TRANS_LINK_COUNT_MAX > ui8LinkThreadIdx )
	{
		pUserSession->m_ui32ATLoginWaitSend		= i3ThreadTimer::GetServerTime() + SESSION_CHECK_LOGINWAIT_SENDTIME;
		pUserSession->m_ui8LoginLinkThreadIdx	= ui8LinkThreadIdx;
		pUserSession->m_ui32LoginWaitCount		= m_ui32LinkLoginCount[ ui8LinkThreadIdx ] + i32WaitCount;
	}

	// Wait 패킷 전송
	i3NetworkPacket SendPacket( PROTOCOL_BASE_LOGIN_WAIT_ACK );
	SendPacket.WriteData( &i32WaitCount,		sizeof(INT32) );
	SendPacket.WriteData( &ui32LoginTimeArg,	sizeof(UINT32) );
	pUserSession->SendPacketMessage( &SendPacket );
}

// ----------------------------------------------------------
// Name : UserServerEnter_A
// Desc : 서버이동 후 아이템 정보 받기
// ----------------------------------------------------------
void CUserSessionManager::UserServerEnter_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, i3NetworkPacket* pPacket )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->ServerEnter_A( i32Rv, pPacket );
}

// ----------------------------------------------------------
// Name : UserServerLeave_A
// Desc : 서버이동 후 아이템 정보 받기
// ----------------------------------------------------------
void CUserSessionManager::UserServerLeave_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->ServerLeave_A( i32Rv );
}

/*----------------------------------------------------------
Name : KickUser_Module
Desc : Auth 서버로 부터 강퇴를 받는다. 
----------------------------------------------------------*/
void CUserSessionManager::KickUser_A( INT32 i32SessionIdx, T_UID i64UID, UINT8 ui8Reason )
{
	CUserSession * pUserSession = GetSession( i32SessionIdx, i64UID );
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
void CUserSessionManager::GetAttendanceResult( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->SendAttendanceItemResult( i32Rv );
}

// ----------------------------------------------------------
// Name : GetAttendanceResult
// Desc : 출석체크 이벤트 출석 요청에 대한 결과값을 받습니다. - 서동권
// ----------------------------------------------------------
void CUserSessionManager::GetAttendanceCheck( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, ATTENDANCE_USER_SVR* stAttendance )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	pSession->SendAttendanceCheckResult( i32Rv, stAttendance );
}

void CUserSessionManager::DestroyRoom_A( INT32 i32SessionIdx )
{
	if(i32SessionIdx >= m_MaxSessionIdx)
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
	if(i32SessionIdx >= m_MaxSessionIdx)
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

void CUserSessionManager::SendRoomNoticeMessage_A(INT32 i32SessionIdx, char * strMessage, UINT32 ui32len)
{
	if(i32SessionIdx >= m_MaxSessionIdx)
	{
		I3TRACE("[CUserSessionManager::SendRoomNoticeMessage_A]Wrong Session Idx"); 
		return; 
	}

	CUserSession * pUserSession; 
	pUserSession = (CUserSession*)m_pSessionList[i32SessionIdx];
	
	if( pUserSession->GetIsActive() && (pUserSession->m_i32RoomIdx != SERVER_VALUE_UNKNOWN) )
	{
		//메시지 보내기 
		pUserSession->m_pRoom->SendNoticeMessage(strMessage, ui32len); 
	}

	return; 
}

/*----------------------------------------------------------
Name : OnSendLobbyUserList
Desc : 유저리스트 요청하기
----------------------------------------------------------*/
void CUserSessionManager::OnSendLobbyUserList(CUserSession * pSession, INT32 i32Channel, LOBBY_USER_INFO* pLobbyUserInfo, UINT32 ui32UserCount)
{
	i3NetworkPacket SendPacket( PROTOCOL_ROOM_GET_LOBBY_USER_LIST_ACK );

	// UserList
	UINT32 ui32PageCount = ui32UserCount / SEND_MAX_LOBBY_USER_COUNT;
	INT32 i32UserPageNum = pSession->m_UpdateUserListPage;

	if ((ui32UserCount % SEND_MAX_LOBBY_USER_COUNT) > 0) ui32PageCount++;
	if (i32UserPageNum > (INT32)ui32PageCount) i32UserPageNum = 0;

	UINT32 ui32StartIdx = i32UserPageNum * SEND_MAX_LOBBY_USER_COUNT;
	UINT32 ui32EndIdx = ui32StartIdx + SEND_MAX_LOBBY_USER_COUNT;
	if (ui32EndIdx > ui32UserCount)		ui32EndIdx = ui32UserCount;
	UINT32 PacketUserCount = ui32EndIdx - ui32StartIdx;

	SendPacket.WriteData(&ui32UserCount, sizeof(UINT32));
	SendPacket.WriteData(&i32UserPageNum, sizeof(INT32));
	SendPacket.WriteData(&PacketUserCount, sizeof(INT32));

	CUserSession * pSessionLobby;
	for (UINT32 i = ui32StartIdx; i < ui32EndIdx; i++)
	{
		pSessionLobby = (CUserSession*)pLobbyUserInfo[i].pSession;

		SendPacket.WriteData(&pSessionLobby->m_SessionIdx, sizeof(INT32));
		SendPacket.WriteData(&pSessionLobby->m_UserInfoBasic.m_ui32ClanMark, sizeof(UINT32));
		SendPacket.WriteData(pSessionLobby->m_UserInfoBasic.m_strClanName, sizeof(TTCHAR) * NET_CLAN_NAME_SIZE);
		SendPacket.WriteData(&pSessionLobby->m_ui32ViewRank, sizeof(UINT8));		// 클라이언트와 맞추기 위해 의도적으로 UINT8로 설정함
		SendPacket.WriteData(&pSessionLobby->m_UserInfoBasic.m_ui8ESportID, sizeof(UINT8));
		SendPacket.WriteData(pSessionLobby->m_UserInfoBasic.m_strNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
		SendPacket.WriteData(&pSessionLobby->m_UserInfoInvItemData._ui8NickColor, sizeof(UINT8));
		SendPacket.WriteData(&pSessionLobby->m_ui8Country, sizeof(UINT8));
	}

	pSession->m_UpdateUserListPage++;
	pSession->SendPacketMessage(&SendPacket);
	return; 
}
//초대유저 목록 보내주기
void CUserSessionManager::OnSendInviteLobbyUserList(CUserSession * pSession, UINT32 ui32Channel)
{
	i3NetworkPacket SendPacket(PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_ACK);

	UINT8 ui8NickLength;
	CUserSession * pLobbySession;
	INT32 i32UserCount = 0;
	CUserSession * pUserList[MAX_INVITE_COUNT];

	m_pcsLobbyUser[ui32Channel]->Enter();
	{
		//PreOBT에 랜덤하게 나오는 유저리스트를 위해서 만듬
		if (m_pLobbyUserList[ui32Channel]->GetCount() > 0)
		{
			INT32 i32Idx = rand() % m_pLobbyUserList[ui32Channel]->GetCount();

			//보낼수 있는 유저를 검사합니다.
			for (INT32 i = 0; i < m_pLobbyUserList[ui32Channel]->GetCount(); i++)
			{
				pLobbySession = (CUserSession*)m_pLobbyUserList[ui32Channel]->GetItem(i32Idx);
				if (pLobbySession->m_UserInfoBasic.m_strNick[0] != 0 && !pLobbySession->IsJoinMatchTeam() && !pLobbySession->IsMercenary())
				{
					pUserList[i32UserCount] = pLobbySession;
					i32UserCount++;
					if (i32UserCount >= MAX_INVITE_COUNT)break;
				}

				i32Idx++;
				if (i32Idx >= m_pLobbyUserList[ui32Channel]->GetCount())i32Idx = 0;
			}
		}
	}
	m_pcsLobbyUser[ui32Channel]->Leave();


	//패킷만들기
	SendPacket.WriteData(&i32UserCount, sizeof(UINT32));			//카운트 
	for (INT32 i = 0; i < i32UserCount; i++)
	{
		SendPacket.WriteData(&pUserList[i]->m_SessionIdx, sizeof(INT32));		//IDX
		SendPacket.WriteData(&pUserList[i]->m_ui32ViewRank, sizeof(INT32));		//Rank : 클라이언트와 맞추기 위해 의도적으로 UINT32로 설정함
		ui8NickLength = (UINT8)i3String::Length(pUserList[i]->m_UserInfoBasic.m_strNick) + 1;	//Nick Name
		SendPacket.WriteData(&ui8NickLength, sizeof(UINT8));		//Nick Name
		SendPacket.WriteData(pUserList[i]->m_UserInfoBasic.m_strNick, sizeof(TTCHAR) * ui8NickLength);			//Nick Name
#ifdef USE_COLOR_NICK2
		SendPacket.WriteData(&pUserList[i]->m_UserInfoInvItemData._ui8NickColor, sizeof(UINT8));			//Nick Name
#endif
	}

	pSession->SendPacketMessage(&SendPacket);

	return;

}

/*----------------------------------------------------------
Name : OnInviteLobbyUser
Desc : 초대하기 
----------------------------------------------------------*/
void CUserSessionManager::OnInviteLobbyUser( CUserSession * pSession, UINT32 Channel, UINT32 ui32InvitedUserIdx, char * pPass)
{
	CUserSession * pInvitedSession = (CUserSession *)m_pSessionList[ ui32InvitedUserIdx ]; 

	//초청 메시지를 보낼수 있는지를 검사합니다. 
	if( !pSession->GetIsActive() )return;									//살아있는지 	
	if( pSession->m_i32ChannelNum != pInvitedSession->m_i32ChannelNum )return;	//채널에 같은지 
	if( pInvitedSession->GetPosition() != GAME_TASK_LOBBY)return;			//초대 받은 사람이 로비에 있는 확인
	if( pInvitedSession->IsMercenary() ) return;							// 용병이면 실패

	i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_INVITED );
	SendPacket.WriteData( pSession->m_UserInfoBasic.m_strNick,	sizeof(TTCHAR) * NET_NICK_NAME_SIZE );	//Wirte Nick Name
	SendPacket.WriteData( &pSession->m_i32RoomIdx,				sizeof(INT32) );		//RoomIdx
	SendPacket.WriteData( &pSession->m_i64UID,					sizeof(T_UID) );		//RoomIdx
	SendPacket.WriteData( pPass,								NET_ROOM_PW);			//Room Passwd
	pInvitedSession->SendPacketMessage( &SendPacket );

	return; 
}

void CUserSessionManager::_OnInsertItem( CUserSession * pSession, INVEN_BUFFER* pInvenItem, INVEN_BUFFER_SET* pInvenSet )
{
	pSession->OnInsertItem( pInvenItem, pInvenSet );
}

UINT32 CUserSessionManager::_OnDeleteItem( INT32 i32ThreadIdx, CUserSession * pSession, T_ItemDBIdx TItemWareIdx, INVEN_BUFFER_SET* pInvenSet )
{
	return pSession->OnDeleteItem( i32ThreadIdx, TItemWareIdx, pInvenSet );
}



/*----------------------------------------------------------
Name : OnGoodsBuy_A
Desc : 구입하기
----------------------------------------------------------*/
void CUserSessionManager::OnGoodsBuy_A(	INT32 i32SessionIdx, T_UID i64UID, UINT32 ui32Point, UINT32 ui32Cash, UINT8 ui8GoodsCount, DB_GOODS* pstDBGoods, UINT8 ui8Action  )	// 구입하기 
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if(pSession == NULL)	return;

	INT32	i32BuyReqFlag = 0;		// 구매 요청한 상품.
	INT32	i32SuccessFlag = 0;		// 실제 성공한 상품.

	S2MO_SHOP_BUY_RESULT	aBuyResult[MAX_SHOP_GOODS_BUY_COUNT];
	INVEN_BUFFER_SET		InvenSet; 
	i3mem::FillZero( &InvenSet, sizeof( INVEN_BUFFER_SET ) );
	InvenSet.ui8ChangeType = ITEM_INSERT_REQUEST_BUY;

	if( ITEM_TYPE_QUESTCARD == GET_GOODS_FLAG_TYPE( pstDBGoods[0].m_TGoodsID ) )
	{
		// 성공 실패 여부에 따라 금액을 먼저 지급합니다.
		if(EV_SUCCESSED( pstDBGoods[0].m_i32Rv ) )
		{
			pSession->m_UserInfoBasic.m_ui32Cash		= ui32Cash;
			if( ui32Cash > pSession->m_UserInfoBasic.m_ui32NCCoin )
			{
				pSession->m_UserInfoBasic.m_ui32HappyCoin = ui32Cash - pSession->m_UserInfoBasic.m_ui32NCCoin;
			}
			else
			{
				pSession->m_UserInfoBasic.m_ui32HappyCoin = 0;
				pSession->m_UserInfoBasic.m_ui32NCCoin = ui32Cash;
			}
		}
		else
		{
			CShop* pShop = g_pContextMain->GetShop();
			SHOP_GOODS_BASE* pGoods = pShop->GetGoodsData( pstDBGoods[0].m_TGoodsID );
			if( NULL != pGoods )
			{
				switch( pstDBGoods[0].m_ui8BuyType )
				{
				case GOODS_BUY_POINT:
					{
						pSession->m_UserInfoBasic.m_ui32Point	+= ui32Point;
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
		pSession->SendBuyQuestCardResult( pstDBGoods[0].m_i32Rv, pstDBGoods[0].m_TGoodsID, MODULE_TRANS_THREAD_IDX, pstDBGoods[0].m_stDBItem.m_aItem[0]._TItemWareDBIdx , pstDBGoods[0].m_ui8BuyType );
	}
	else
	{
		for( UINT8 i = 0 ; i < ui8GoodsCount ; i++ )
		{
			aBuyResult[i].m_i32Rv		= pstDBGoods[i].m_i32Rv;
			aBuyResult[i].m_TGoodsID	= pstDBGoods[i].m_TGoodsID;
			aBuyResult[i].m_ui8Action	= ui8Action;
			i32BuyReqFlag |= 0x1 << i;

			if( EV_FAILED( pstDBGoods[i].m_i32Rv )) continue; 

			i32SuccessFlag |= 0x1 << i;

			// 유저 인벤토리 업데이트.
			for( UINT8 k = 0 ; k < pstDBGoods[i].m_stDBItem.m_ui8ItemCount ; k++ )
			{
				INVEN_BUFFER InvenBuffer;
				i3mem::Copy( &InvenBuffer, &pstDBGoods[i].m_stDBItem.m_aItem[k], sizeof( INVEN_BUFFER ) );
				pSession->OnInsertItem( &InvenBuffer, &InvenSet );

				// 구매 미션 체크
				pSession->MedalMissionCheck_Shop( MODULE_TRANS_THREAD_IDX, (UINT8)pstDBGoods[i].m_ui8BuyType, pstDBGoods[i].m_stDBItem.m_aItem[k]._ui32ItemID );
			}

			//ZLog 에 기록 상점 로그 보내기 (구입)
			INT32 i32SpendType = (INT32)SPEND_TYPE_BUY_GOODS;
			g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_SHOP_BUY, pSession, &pstDBGoods[i].m_TGoodsID, &pstDBGoods[i].m_ui8BuyType, &i32SpendType );
			// 마지막 캐시 아이템 구입 날짜 저장
			if( GOODS_BUY_CASH == pstDBGoods[i].m_ui8BuyType  )		pSession->m_UserInfoServer.m_ui32LastBuyCash = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
		}

		if( i32SuccessFlag > 0 ) // 상품 구매가 1건 이라도 성공했다면,
		{
			pSession->SendChangeInven( &InvenSet );		// 추가되는 아이템을 전달 합니다.

			// 캐쉬 UPDATE.
			pSession->m_UserInfoBasic.m_ui32Cash		= ui32Cash; 
			if( ui32Cash > pSession->m_UserInfoBasic.m_ui32NCCoin )
			{
				pSession->m_UserInfoBasic.m_ui32HappyCoin = ui32Cash - pSession->m_UserInfoBasic.m_ui32NCCoin;
			}
			else
			{
				pSession->m_UserInfoBasic.m_ui32HappyCoin = 0;
				pSession->m_UserInfoBasic.m_ui32NCCoin = ui32Cash;
			}
		}

		if( i32SuccessFlag != i32BuyReqFlag )	// 부분 성공 또는 전체 실패시 포인트 롤백.
		{
			pSession->m_UserInfoBasic.m_ui32Point	+= ui32Point;
		}

		pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
		
		// Packet Send.
		PACKET_SHOP_BUY_ACK Packet;
		Packet.m_ui32InvenTime	= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
		Packet.m_ui32Point		= pSession->m_UserInfoBasic.m_ui32Point;
		Packet.m_ui32Cash		= pSession->m_UserInfoBasic.m_ui32Cash;
		Packet.m_aBuyResult.SetValue( aBuyResult, 0, ui8GoodsCount );

		pSession->S2MOSendEvent( &Packet );
	}

	return; 
}

void CUserSessionManager::OnGoodsGift_A( CUserSession * pSession,  PACKET_AUTH_SHOP_GOODS_GIFT_ACK* pPacket  )
{
	INT32	i32BuyReqFlag = 0;		// 구매 요청한 상품.
	INT32	i32SuccessFlag = 0;		// 실제 성공한 상품.

	S2MO_SHOP_BUY_RESULT	aBuyResult[MAX_SHOP_GOODS_BUY_COUNT];
	
	for( UINT8 i = 0 ; i < pPacket->m_aResult.GetCount() ; ++i )
	{
		aBuyResult[i].m_i32Rv		= pPacket->m_aResult[i].m_i32Rv;
		aBuyResult[i].m_TGoodsID	= pPacket->m_aResult[i].m_TGoodsID;
	
		i32BuyReqFlag |= 0x1 << i;

		if( EV_FAILED( pPacket->m_aResult[i].m_i32Rv )) continue; 

		i32SuccessFlag |= 0x1 << i;
		
		//ZLog 에 기록 상점 로그 보내기 (구입)
		INT32 i32SpendType = (INT32)SPEND_TYPE_BUY_GIFT;
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_SHOP_BUY, pSession, &pPacket->m_aResult[i].m_TGoodsID, &pPacket->m_aResult[i].m_ui8Action, &i32SpendType );

		// 마지막 캐시 아이템 구입 날짜 저장
		if( GOODS_BUY_CASH == pPacket->m_aResult[i].m_ui8Action  )		pSession->m_UserInfoServer.m_ui32LastBuyCash = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
	}
	
	if( i32SuccessFlag > 0 ) // 상품 구매가 1건 이라도 성공했다면,
	{
		// 캐쉬 UPDATE.
		pSession->m_UserInfoBasic.m_ui32Cash		= pPacket->m_ui32Cash; 
		if( pPacket->m_ui32Cash > pSession->m_UserInfoBasic.m_ui32NCCoin )
		{
			pSession->m_UserInfoBasic.m_ui32HappyCoin = pPacket->m_ui32Cash - pSession->m_UserInfoBasic.m_ui32NCCoin;
		}
		else
		{
			pSession->m_UserInfoBasic.m_ui32HappyCoin = 0;
			pSession->m_UserInfoBasic.m_ui32NCCoin = pPacket->m_ui32Cash;
		}
	}
	
	if( i32SuccessFlag != i32BuyReqFlag )	// 부분 성공 또는 전체 실패시 포인트 롤백.
	{
		pSession->m_UserInfoBasic.m_ui32Point	+= pPacket->m_ui32Point;
	}

	pSession->ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO );

	pPacket->m_ui32InvenTime	= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
	pPacket->m_ui32Point		= pSession->m_UserInfoBasic.m_ui32Point;
	pPacket->m_ui32Cash			= pSession->m_UserInfoBasic.m_ui32Cash;

	pSession->S2MOSendEvent( pPacket );

	return;
}


void CUserSessionManager::OnGoodsGiftFail_A(	CUserSession * pSession, PACKET_AUTH_SHOP_GOODS_GIFT_REQ* pPacketReq, INT32 i32Rv  )
{
	PACKET_AUTH_SHOP_GOODS_GIFT_ACK		PacketAck;
	
	PacketAck.m_ui32InvenTime	= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
	PacketAck.m_ui32Point		= pSession->m_UserInfoBasic.m_ui32Point;
	PacketAck.m_ui32Cash		= pSession->m_UserInfoBasic.m_ui32Cash;

	for( UINT8 i = 0 ; i < pPacketReq->m_aBuyGift.GetCount() ; ++i )
	{
		PacketAck.m_aResult[i].m_i32Rv		= i32Rv;
		PacketAck.m_aResult[i].m_TGoodsID	= pPacketReq->m_aBuyGift[i].m_ui32GoodsID;
	}
	
	pSession->S2MOSendEvent( &PacketAck );

	return;
}

void CUserSessionManager::OnGiftAuth_A( CUserSession * pSession, UINT8 ui8Type, UINT8 ui8Count, AUTH_GIFT_INVEN* paAuthGift )
{
	PACKET_AUTH_SHOP_AUTH_GIFT_ACK	PacketAck;
	PacketAck.m_ui8Type = ui8Type;
	for( UINT8 i = 0 ; i < ui8Count ; ++i )
	{
		i3mem::Copy( &PacketAck.m_aResult[i], &paAuthGift[i], sizeof( AUTH_GIFT ) );
	}

	// 유저 인벤토리 업데이트.
	if( TYPE_SHOP_GIFT_AUTH == PacketAck.m_ui8Type && NULL != paAuthGift )
	{
		INVEN_BUFFER_SET InvenSet;
		InvenSet.ui8ChangeType = ITEM_INSERT_REQUEST_GIFT_AUTH;
		InvenSet.m_ui16InvenCount = 0;

		for( UINT8 i = 0 ; i < ui8Count ; ++i )
		{
			for( UINT8 k = 0 ; k < paAuthGift->m_ui8ItemCount ; ++k )
			{
				if( EV_FAILED( PacketAck.m_aResult[i].m_i32Rv )) continue;
				if (FALSE == pSession->CheckExistItemInvenByItemID(paAuthGift[i].m_Inven[k]._ui32ItemID)) continue;

				pSession->OnInsertItem( &paAuthGift[i].m_Inven[k], &InvenSet ); 
			}
		}

		pSession->SendChangeInven( &InvenSet );		// 추가되는 아이템을 전달 합니다.
	}

	pSession->S2MOSendEvent( &PacketAck );
}


void CUserSessionManager::OnGiftAuthFail_A(	CUserSession * pSession, PACKET_AUTH_SHOP_AUTH_GIFT_REQ* pPacketReq, INT32 i32Rv )
{
	PACKET_AUTH_SHOP_AUTH_GIFT_ACK	PacketAck;
	PacketAck.m_ui8Type = pPacketReq->m_ui8Type;

	for( UINT8 i = 0 ; i < pPacketReq->m_aGiftIdx.GetCount() ; ++i )
	{
		PacketAck.m_aResult[i].m_i32Rv	= i32Rv;
		PacketAck.m_aResult[i].m_TDBIdx = pPacketReq->m_aGiftIdx[i];
	}

	pSession->S2MOSendEvent( &PacketAck );
	return;
}

void CUserSessionManager::OnGetGift_A( CUserSession * pSession, T_RESULT TResult, PACKET_AUTH_SHOP_GET_GIFTLIST_ACK* pPacket )
{
	pPacket->m_ui32InvenTime	= i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
	pPacket->m_TResult			= TResult; 
	pSession->S2MOSendEvent( pPacket );
}

void CUserSessionManager::OnInsertItem_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, INT8 i8RequestType, UINT8 ui8ItemCount, INVEN_BUFFER_DB *pItem , UINT32 ui32CapsuleItemID)
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;

	if(EV_SUCCESSED( i32Rv ))
	{
		for( UINT8 i = 0 ; i < ui8ItemCount ; i++ )
		{
			INVEN_BUFFER InvenBuffer;
			i3mem::Copy( &InvenBuffer, &pItem[i], sizeof( INVEN_BUFFER ) );
			if (TRUE == pSession->CheckExistItemInvenByItemID(InvenBuffer._ui32ItemID))
			{
				pSession->OnInsertItem(&InvenBuffer, &InvenSet);

				InvenSet.ui8ChangeType = ITEM_INSERT_REQUEST_AUTH;
				_SetItemAbilityAuth_A(pSession, InvenBuffer._ui32ItemID, InvenBuffer._ui32ItemArg);
			}
			if( ITEM_INSERT_REQUEST_BUY != i8RequestType )
			{
				InvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_INSERT;
				g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_GET_ITEM, pSession, &InvenBuffer, &i8RequestType, &pItem[i]._ui32AddItemArg, &ui32CapsuleItemID);
			}
		}

		pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

		// 클라이언트에 바뀐 부분을 알려준다. 
		pSession->SendChangeInven( &InvenSet );	
	}

	return;  
}

void CUserSessionManager::OnDeleteItem_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, T_ItemDBIdx TItemWareIdx )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;
	UINT32 ui32DeleteItemID = 0;

	if(EV_SUCCESSED( i32Rv ))
	{
		ui32DeleteItemID = _OnDeleteItem( MODULE_TRANS_THREAD_IDX, pSession, TItemWareIdx, &InvenSet );

		pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );
	}

	switch( ui32DeleteItemID )
	{
	case CASHITEM_ITEMID_CLAN_XP_PLUS_150:
	case CASHITEM_ITEMID_CLAN_XP_PLUS_150_EV:
		// 클랜경험치150% - 삭제된 아이템이 클랜 아이템이면 클랜 서버에 전달
		g_pModuleClan->ClanSendItemDelete( MODULE_TRANS_THREAD_IDX, pSession, ui32DeleteItemID );

		break;
	case CASHITEM_ITEMID_COLOR_CLAN:
		// 클랜명색상 - 삭제된 아이템이 클랜 아이템이면 클랜 서버에 전달
		g_pModuleClan->ClanSendItemDelete( MODULE_TRANS_THREAD_IDX, pSession, ui32DeleteItemID );
		break;
	}

	pSession->CheckEquipment();


	i3NetworkPacket SendPacket( PROTOCOL_AUTH_SHOP_DELETE_ITEM_ACK );
	SendPacket.WriteData( &i32Rv,						sizeof(INT32));
	if(EV_SUCCESSED( i32Rv ))
	{
		SendPacket.WriteData( &TItemWareIdx,		sizeof(T_ItemDBIdx));
		g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_ITEM_DELETE, pSession, &ui32DeleteItemID ); 
	}
	pSession->SendPacketMessage( &SendPacket );

	return; 
}

void CUserSessionManager::OnAuthUsedWeapon_A( INT32 i32SessionIdx, T_UID i64UID, UINT16 ui16InvenCount, INVEN_BUFFER* pInvenItem )
{
	CUserSession * pSession = GetSession(i32SessionIdx, i64UID); 
	if(pSession == NULL)	return;
	
	INVEN_BUFFER_SET InvenSet;
	InvenSet.ui8ChangeType = ITEM_INSERT_REQUEST_AUTH;

	for( UINT32 i = 0 ; i < ui16InvenCount ; i++ )
	{
		if( 0 == pInvenItem[i]._ui32ItemID )
		{	// 아이템이 만료되어 삭제
			_OnDeleteItem( MODULE_TRANS_THREAD_IDX, pSession, pInvenItem[i]._TItemWareDBIdx, &InvenSet );
		}
		else
		{
			_OnInsertItem( pSession, &pInvenItem[i], &InvenSet );
		}
	}

	pSession->CheckEquipment();

	pSession->SendChangeInven( &InvenSet );
}

void CUserSessionManager::OnItemAuth_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem )
{
	CUserSession * pSession = GetSession(i32SessionIdx, i64UID); 
	if(pSession == NULL)	return;
	
	INVEN_BUFFER_SET InvenSet;

	if(EV_SUCCESSED( i32Rv ))
	{
		UINT32 ui32ItemID = pInvenItem->_ui32ItemID;
		if( 0 == pInvenItem->_ui32ItemID )
		{	// 아이템이 만료되어 삭제
			ui32ItemID = pSession->OnDeleteItem( MODULE_TRANS_THREAD_IDX, pInvenItem->_TItemWareDBIdx, &InvenSet );
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
	SendPacket.WriteData( &i32Rv,					sizeof(INT32));
	if(EV_SUCCESSED( i32Rv ))
	{
		UINT32	ui32InvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
		SendPacket.WriteData( &ui32InvenTime,		sizeof(UINT32));
		SendPacket.WriteData( pInvenItem,			sizeof(INVEN_BUFFER));
	}
	pSession->SendPacketMessage( &SendPacket );

	return; 
}

void CUserSessionManager::OnItemCheatAuth_A(INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, INVEN_BUFFER* pInvenItem)
{
	CUserSession * pSession = GetSession(i32SessionIdx, i64UID);
	if (pSession == nullptr)	return;

	INVEN_BUFFER_SET InvenSet;
	if (EV_SUCCESSED(i32Rv))
	{
		UINT32 ui32ItemID = pInvenItem->_ui32ItemID;
		if (0 == pInvenItem->_ui32ItemID)
		{	// 아이템이 만료되어 삭제
			ui32ItemID = pSession->OnDeleteItem(MODULE_TRANS_THREAD_IDX, pInvenItem->_TItemWareDBIdx, &InvenSet);
		}
		else
		{
			pSession->OnInsertItem(pInvenItem, &InvenSet);
		}

		pSession->CheckEquipment();
	}

	i3NetworkPacket SendPacket(PROTOCOL_CHEAT_ITEM_AUTH_ACK);
	SendPacket.WriteData(&i32Rv, sizeof(INT32));
	if (EV_SUCCESSED(i32Rv))
	{
		UINT32	ui32InvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
		SendPacket.WriteData(&ui32InvenTime, sizeof(UINT32));
		SendPacket.WriteData(pInvenItem, sizeof(INVEN_BUFFER));
	}
	pSession->SendPacketMessage(&SendPacket);

	return;
}

/*----------------------------------------------------------
Name : OnInviteUserServer
Desc : 유저 초대(다른서버)
----------------------------------------------------------*/
void CUserSessionManager::OnInviteUserServer(PACKET_TRANS_COMMUNITY_USER_INVITED_REQUEST_ACK* pPacketAck)
{
	CUserSession * pInvitedSession = GetSession(pPacketAck->m_i32SessionIdx, pPacketAck->m_i64UID); 	

	if(pInvitedSession !=NULL)
	{
		i3NetworkPacket SendPacket( PROTOCOL_COMMUNITY_USER_INVITED_REQUEST_ACK );
		PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK	PacketAck;
		PacketAck.m_ConnectState = pPacketAck->m_ConnectState;
		i3mem::Copy(PacketAck.m_strNick, pPacketAck->m_strNick, sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
		PacketAck.m_strNick[NET_NICK_NAME_SIZE-1] = '\0';
		SendPacket.WriteData(&PacketAck, sizeof(PACKET_COMMUNITY_USER_INVITED_REQUEST_ACK)); 	
		pInvitedSession->SendPacketMessage( &SendPacket );						
	}
	return; 
}

void CUserSessionManager::OnSendNoticeMessage( INT32 i32ThreadIdx, INT32 i32Type, TTCHAR * strMessage, INT32 i32RGB)
{		

	PACKET_MESSAGE_ANNOUNCE_ACK SendPacket;
	SendPacket.m_i32Type		= i32Type;
	SendPacket.m_strChatMessage	= strMessage;
	SendPacket.m_i32RGB = i32RGB;	
	
	AnnounceMessagePush( i32ThreadIdx, SendPacket.GetProtocol(), SendPacket );

	return; 
}

void CUserSessionManager::OnSendAllUser( i3NetworkPacket* pPacket )
{
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
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

	for(INT32 i = 0 ; i < m_i32OriMaxSessionIdx ; i++)
	{
		pUserSession = (CUserSession*)m_pSessionList[i];
		if( NULL == pUserSession )					continue;
		if( FALSE == pUserSession->GetIsActive())	continue;

		// ModuleControlSocket 에서 호출
		pUserSession->DisconnectUserDelay( EVENT_ERROR_EVENT, KILL_REASON_USERKICK );
	}
	return; 
}

void	CUserSessionManager::_SetItemAbilityAuth_A( CUserSession * pSession, UINT32 ui32ItemID, UINT32 ui32ItemArg )
{

	if( ITEM_TYPE_MAINTENANCE == GET_ITEM_TYPE( ui32ItemID ) )
	{
		switch( ui32ItemID )
		{
		// 컬러 채팅
		case CASHITEM_ITEMID_COLOR_CHATTING:
			{
				pSession->InvItemData();
			}
			break;

		// 클랜 150%
		case CASHITEM_ITEMID_CLAN_XP_PLUS_150:
		case CASHITEM_ITEMID_CLAN_XP_PLUS_150_EV:
			{	// 클랜서버에 알려준다.

				// ItemID를 12로 시작하도록 변경해서 보냅니다. 2011.03.22 노준영
				ui32ItemID = CASHITEM_ITEMID_CLAN_XP_PLUS_150;
				g_pModuleClan->ClanSendExpUpRequest( MODULE_TRANS_THREAD_IDX, pSession, ui32ItemID, ui32ItemArg );
			}		
			break;
		// 클랜 컬러
		case CASHITEM_ITEMID_COLOR_CLAN:
			{	// 클랜 컬러는 후에 추가 작업
				
				// 동일한 기능의 이벤트 아이템이 없으므로 원래ID 그대로 보냅니다. 2013.10.23 임준현				
				g_pModuleClan->ClanSendClanColorRequest(MODULE_TRANS_THREAD_IDX, pSession, ui32ItemID, ui32ItemArg, 0 );
			}
			break;
		}
	}
	else if( ITEM_TYPE_WRAP_PERIOD == GET_ITEM_TYPE( ui32ItemID ) )
	{
		// 기간제 판매용이라 기간제로 바꿔서 체크합니다.
		// 기간제는 999개가 넘어가면 여기도 수정 필요....
		UINT32 ui32MaintenenceID = MAKE_ITEM_ID(ITEM_TYPE_MAINTENANCE, CASHITEM_DAY_NO, GET_ITEM_NUMBER(ui32ItemID));
		switch( ui32MaintenenceID )
		{
		// 위장 계급
		case CASHITEM_ITEMID_DISGUISE_RANK:
		case CASHITEM_ITEMID_DISGUISE_RANK_APRIL_FOOL:
			{					
				// 계급 바꿔준다.
				pSession->SetChangeRank( MODULE_TRANS_THREAD_IDX, ui32ItemID );				
			}
			break;
		// 임시 닉네임
		case CASHITEM_ITEMID_DISGUISE_NICK:
			{					
				// 닉네임 바꿔준다.
				pSession->SetChangeNick( MODULE_TRANS_THREAD_IDX, pSession->m_UserInfoBasic.m_strNick, pSession->m_UserInfoInvItemData._cNick );
			}
			break;

		// 컬러 크로스헤어
		case CASHITEM_ITEMID_COLOR_CROSSHAIR:
			{
				pSession->m_UserInfoInvItemData._ui8Item |= SALE_ITEM_COLOR_CROSSHAIR;
				pSession->InvItemData();
			}
			break;
		
		// 컬러 닉네임
		case CASHITEM_ITEMID_COLOR_NICK:
			{
				pSession->SetChangeColorNick( MODULE_TRANS_THREAD_IDX, pSession->m_UserInfoInvItemData._ui8NickColor );
			}
			break;
		// 총구 화염 컬러 변경권
		case CASHITEM_ITEMID_COLOR_MUZZLE_FLASH:
			{
				pSession->SetChangeColorMuzzleFlash();
			}			
		}
	
	}
	else if( ITEM_TYPE_WRAP_COUNT == GET_ITEM_TYPE( ui32ItemID ) )
	{
		switch( ui32ItemID )
		{
		case CASHITEM_ITEMID_MAIN_SKILL_ALLOPEN:
			{
				//스킬타임 실제 적용도 현위치에서 수행하는게 효율과 버그의 소지가 적습니다.
				//이를 위해서는 PROTOCOL_AUTH_SHOP_ITEM_AUTH_DATA_REQ 처리 루틴을 PROTOCOL_AUTH_SHOP_ITEM_AUTH_REQ와
				//분리하여 적용해야 합니다.
				pSession->ChangeCharaDataBaseInfo(0xFFFFFFFFFFFFFFFF);
			}
		}
	}
	
}

void CUserSessionManager::OnSendChannelUser( INT32 i32ChannelNum, i3NetworkPacket* pPacket )
{
	CUserSession * pUserSession; 
	for(INT32 i = 0; i < m_MaxSessionIdx; i++)
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

void CUserSessionManager::AnnounceMessagePush( INT32 i32ThreadIdx, UINT32 ui32Protocol, S2MOPacketBase & pstPacket, UINT8 ui8Target, UINT32 ui32ChannelNum, UINT32 ui32RoomIdx, UINT32 ui32SessionIdx )
{
	NSM_CRingBuffer * pRingBuffer = NULL;

	if( pstPacket.GetPacketSize() > ANNOUNCE_MESSAGE_BUFFER_SIZE )	
	{ 
		g_pLog->WriteLog( L"Over sized Announce message requested (Protocol:%d)", ui32Protocol);
		return; 
	}

	switch(i32ThreadIdx)
	{
	case MAIN_THREAD_IDX:
		{
			if( m_pRingAnnounceMain->GetEmptyBufferCount() == 0 )
			{
				g_pLog->WriteLog( L"Announce message buffer full (Protocol:%d)", ui32Protocol);
				return;
			}

			pRingBuffer = m_pRingAnnounceMain;
		}
		break;
	case MODULE_CONTROL_THREAD_IDX:
		{
			if( m_pRingAnnounceControl->GetEmptyBufferCount() == 0 )
			{
				g_pLog->WriteLog( L"Announce message buffer full (Protocol:%d)", ui32Protocol);
				return;
			}

			pRingBuffer = m_pRingAnnounceControl;
		}
		break;
	case MODULE_TRANS_THREAD_IDX:
		{
			if( m_pRingAnnounceTrans->GetEmptyBufferCount() == 0 )
			{
				g_pLog->WriteLog( L"Announce message buffer full (Protocol:%d)", ui32Protocol);
				return;
			}

			pRingBuffer = m_pRingAnnounceTrans;
		}
		break;
	}

	// 쓰레드에 이상한 값이오면 크래쉬 합니다.
	if( NULL == pRingBuffer )	return;

	ANNOUNCE_MESSAGE *	pMessage;
	char				pBuffer[ANNOUNCE_MESSAGE_BUFFER_SIZE];

	pMessage = (ANNOUNCE_MESSAGE *) pRingBuffer->PushPointer();
	if( pMessage == NULL ) return;

	S2MOMakePacket( &pstPacket, pBuffer );
	
	pMessage->ui32Protocol		= ui32Protocol;
	pMessage->ui8Target			= ui8Target;
	pMessage->ui32ChannelNum	= ui32ChannelNum;
	pMessage->ui32RoomIdx		= ui32RoomIdx;
	pMessage->ui32SessionIdx	= ui32SessionIdx;
	pMessage->ui32BufferSize		= pstPacket.GetPacketSize() + 4;		//PacketSize, Protocol 크기만큼 더해줍니다.
	i3mem::Copy( pMessage->pBuffer, pBuffer, pMessage->ui32BufferSize );

	pRingBuffer->PushPointerIdx();
}

void CUserSessionManager::AnnounceMessageCollect(NSM_CRingBuffer *pRingBuffer)
{
	INT32 i32Count =  pRingBuffer->GetBufferCount();
	for( INT32 i = 0 ; i < i32Count; i++ )
	{
		if( m_pRingAnnounceMessage->GetEmptyBufferCount() == 0)
		{
			g_pLog->WriteLog( L"Announce message buffer full" );
			return;
		}

		i3mem::Copy( m_pRingAnnounceMessage->PushPointer(), pRingBuffer->Pop(), sizeof(ANNOUNCE_MESSAGE) );
		m_pRingAnnounceMessage->PushPointerIdx();
		pRingBuffer->PopIdx();
	}
}

bool CUserSessionManager::OnSendAnnounceMessage( ANNOUNCE_MESSAGE * messageInfo )
{
	// 설정된 메시지를 다 보냈으면 TRUE 리턴, 보내야 할 대상이 남아있으면 FALSE 리턴.

	// 보내는 중인 메시지가 없었으면 새로 들어온 메시지를 전달 패킷에 채운다.
	if( m_bSendAnnounce == FALSE )
	{
		m_AnnounceMessage.Clear();
				
		i3mem::Copy( m_AnnounceMessage.GetPacketBuffer(), messageInfo->pBuffer, messageInfo->ui32BufferSize );

		m_bSendAnnounce = TRUE;
		m_i32AnnouncedUserIdx = 0;
	}

	// 보내는 범위에 따라 패킷 전송
	bool bSendEnd = FALSE;
	switch( messageInfo->ui8Target )
	{
	case ANNOUNCE_MESSAGE_ALL:
		{
			// #define SEND_COUNT_ANNOUNCE_MESAGE 수만큼 한번에 전달
			INT8 i8Count = 0;

			if( m_i32AnnouncedUserIdx < m_MaxSessionIdx) 
			{
				for(m_i32AnnouncedUserIdx; m_i32AnnouncedUserIdx < m_MaxSessionIdx; m_i32AnnouncedUserIdx++)
				{
					if(i8Count >= SEND_COUNT_ANNOUNCE_MESSAGE)break;
					CUserSession *	pUserSession = (CUserSession*)m_pSessionList[m_i32AnnouncedUserIdx];
					if( FALSE == pUserSession->GetIsActive() )			continue; 
					if(pUserSession->m_ui32ATIsKill > 0)				continue;
					if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)	continue; 					
					pUserSession->SendPacketMessage( &m_AnnounceMessage );

					i8Count++; 
				}
			}
			
			// 모든 유저에게 보내졌으면
			if( m_i32AnnouncedUserIdx >= m_MaxSessionIdx )
				bSendEnd = TRUE;
		}
		break;
	case ANNOUNCE_MESSAGE_CHANNEL:
		{
			// #define SEND_COUNT_ANNOUNCE_MESAGE 수만큼 한번에 전달
			if( messageInfo->ui32ChannelNum > GAME_CHANNEL_COUNT )	{ bSendEnd = TRUE; break; }

			INT8 i8Count = 0;

			if( m_i32AnnouncedUserIdx < m_MaxSessionIdx) 
			{
				for(m_i32AnnouncedUserIdx; m_i32AnnouncedUserIdx < m_MaxSessionIdx; m_i32AnnouncedUserIdx++)
				{
					if(i8Count >= SEND_COUNT_ANNOUNCE_MESSAGE)break;
					CUserSession *	pUserSession = (CUserSession*)m_pSessionList[m_i32AnnouncedUserIdx];
					if( FALSE == pUserSession->GetIsActive() )							continue; 
					if(pUserSession->m_ui32ATIsKill > 0)								continue;
					if( pUserSession->m_i64UID == SERVER_VALUE_UNKNOWN)					continue; 
					if( pUserSession->m_i32ChannelNum != (INT32)messageInfo->ui32ChannelNum )	continue;
					pUserSession->SendPacketMessage(&m_AnnounceMessage);
					i8Count++; 
				}
			}
			
			// 모든 유저에게 보내졌으면
			if( m_i32AnnouncedUserIdx >= m_MaxSessionIdx )
				bSendEnd = TRUE;
		}
		break;
	case ANNOUNCE_MESSAGE_ROOM:
		{
			// 한 방에 있는 유저에게 모두 전달
			if( messageInfo->ui32RoomIdx >= g_pContextMain->m_ui32ChannelSessionCount )	{ bSendEnd = TRUE; break; }

			CRoom* pRoom = g_pRoomManager->GetRoom(messageInfo->ui32ChannelNum, messageInfo->ui32RoomIdx);
			if( NULL == pRoom ) break;
			if( pRoom->m_bIsCreate )
				pRoom->SendAnnounceMessage( &m_AnnounceMessage );

			bSendEnd = TRUE;
		}
		break;
	case ANNOUNCE_MESSAGE_USER:
		{
			if( messageInfo->ui32SessionIdx >= (UINT32)m_MaxSessionIdx )	{ bSendEnd = TRUE; break; }

			CUserSession * pUserSession = (CUserSession*)m_pSessionList[ messageInfo->ui32SessionIdx ];
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

void CUserSessionManager::UserSetBirthDay_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, UINT32 ui32BirthDay )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if( pSession == NULL ) return;

	pSession->SetBirthDay_A( i32Rv, ui32BirthDay );


}

void CUserSessionManager::OnMedalReward_A( INT32 i32SessionIdx, T_UID i64UID, INT32 i32Rv, UINT8 ui8ItemCount, INVEN_BUFFER_DB *pItem,  MEDAL_REWARD_DETAIL_INFO* pMedalReward )
{
	CUserSession * pSession = GetSession( i32SessionIdx, i64UID );
	if (pSession == NULL)	return;

	INVEN_BUFFER_SET InvenSet;

	if(EV_SUCCESSED( i32Rv ))
	{
		for( UINT8 i = 0 ; i < ui8ItemCount ; i++ )
		{
			INVEN_BUFFER InvenBuffer;
			i3mem::Copy( &InvenBuffer, &pItem[i], sizeof( INVEN_BUFFER ) );
			INT8 i8RequestType = ITEM_INSERT_REQUEST_MEDAL_REWARD;
			if (TRUE == pSession->CheckExistItemInvenByItemID(InvenBuffer._ui32ItemID))
			{
				pSession->OnInsertItem(&InvenBuffer, &InvenSet);
				InvenSet.ui8ChangeType = ITEM_INSERT_REQUEST_AUTH;
				_SetItemAbilityAuth_A(pSession, InvenBuffer._ui32ItemID, InvenBuffer._ui32ItemArg);
				InvenSet.ui8ChangeType = ITEM_INSERT_REQUEST_MEDAL_REWARD;
			}
			UINT32 ui32CapsuleItemID = 0;
			g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_GET_ITEM, pSession, &InvenBuffer, &i8RequestType, &pItem[i]._ui32AddItemArg, &ui32CapsuleItemID);
		}

		// 클라이언트에 바뀐 부분을 알려준다. 
		pSession->SendChangeInven( &InvenSet );
	}

	INT32 i32GetRoute = GET_POINT_ROUTE_MEDAL_REWARD;

	// 보낼 패킷.
	PACKET_D_GET_MEDAL_REWARD		PacketAckD[MAX_SHOP_DB_ITEM_COUNT];
	i3mem::FillZero( PacketAckD, sizeof( PACKET_D_GET_MEDAL_REWARD )*MAX_SHOP_DB_ITEM_COUNT) ;
	PACKET_H_GET_MEDAL_REWARD_ACK	PacketAckH;
	i3mem::FillZero( &PacketAckH, sizeof( PACKET_H_GET_MEDAL_REWARD_ACK )) ;
	PacketAckH.m_ui8Count = ui8ItemCount;
	PacketAckH.m_bLastPacket = true;

	for( UINT8 i = 0 ; i < ui8ItemCount; ++i )
	{
		PacketAckD[i].m_i32Rv = i32Rv;

		PacketAckD[i].m_aMedalReward.m_ui8MedalType		= pMedalReward[i].m_ui8MedalType; 
		PacketAckD[i].m_aMedalReward.m_ui16MedalIdx		= pMedalReward[i].m_ui16MedalIdx; 
		PacketAckD[i].m_aItem.m_TItemID					= pItem[i]._ui32ItemID;
		PacketAckD[i].m_aItem.m_TItemArg				= pItem[i]._ui32ItemArg; 

		if( MEDAL_TYPE_SET == pMedalReward[i].m_ui8MedalType ) PacketAckD[i].m_aMedalReward.m_ui8MedalLevel	= 1;
		else PacketAckD[i].m_aMedalReward.m_ui8MedalLevel		= pMedalReward[i].m_ui8Level;		

		if(EV_SUCCESSED( PacketAckD[i].m_i32Rv ))	// 보상 아이템 지급 성공시 Exp, Point 지급.
		{
			pSession->m_UserInfoBasic.m_ui32Exp			+= pMedalReward[i].m_ui32Exp;
			pSession->m_UserInfoBasic.m_ui32Point		+= pMedalReward[i].m_ui32Point;
			pSession->m_ui32Master						+= pMedalReward[i].m_ui16MasterMedal; 
				
			if( pMedalReward[i].m_ui32Point > 0 ) 
			{	// 포인트 획득시 로그 남긴다. 
				g_pModuleContextNC->LogSendMessage( MODULE_TRANS_THREAD_IDX, LOG_MESSAGE_U_GET_POINT, pSession, &pMedalReward[i].m_ui32Point, &i32GetRoute );
			}

			if(  pMedalReward[i].m_ui32Exp > 0)
			{	// 랭크업 체크.
				pSession->CheckRankUp( MODULE_TRANS_THREAD_IDX );
			}

			pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );	
			pSession->ChangeDataBaseInfo( SAVE_DATABASE_MEDAL_BASIC );	
			
			continue;
		}

		// 보상 아이템 지급 실패. 보상 지급 여부 플래그를 롤백합니다.
		UINT8 Flag = 1 << (pMedalReward[i].m_ui8Level-1);
		switch ( pMedalReward[i].m_ui8MedalType ) 
		{
		case MEDAL_TYPE_NORMAL:
			{
				MEDAL_GT* pUserMedal = pSession->GetMedal_UserDataAll( pMedalReward[i].m_ui16MedalIdx );
				if( NULL == pUserMedal )	continue;

				pUserMedal->m_Medal.m_ui8GetReward &= ~Flag;
				pUserMedal->m_ui8IsSave = 1;
				pSession->ChangeDataBaseInfo( SAVE_DATABASE_MEDAL );  
			}
			break; 
		case MEDAL_TYPE_SET:
			{
				MQF_MEDAL_SET_INFO* pMQFMedalSet =  g_pContextMain->GetMedalSet_ByMedalIdx( pMedalReward[i].m_ui16MedalIdx );
				if( NULL == pMQFMedalSet )	continue;

				CUR_MEDAL_SET* pUserCurMedalSet = pSession->GetCurMedalSet_UserData( pMQFMedalSet->m_ui16Idx);
				if( NULL == pUserCurMedalSet ) continue;

				pUserCurMedalSet->m_ui8GetReward &= ~Flag;
				pSession->ChangeDataBaseInfo( SAVE_DATABASE_CUR_MEDAL_SET );   
			}
			break;
		default: 
			continue;
		}
	}

	PacketAckH.m_ui32Exp			= pSession->m_UserInfoBasic.m_ui32Exp;
	PacketAckH.m_ui32Point			= pSession->m_UserInfoBasic.m_ui32Point;
	PacketAckH.m_ui16MasterMedal	= (UINT16)pSession->m_ui32Master;

	pSession->SendGetMedalReward( &PacketAckH, PacketAckD );

	return; 
}

void	CUserSessionManager::OnSendClanMatchTeamAllUser( INT32 i32CurMember, i3NetworkPacket* pRecvPacket, S2MOPacketBase* pSendPacket )
{
	for( INT32 i = 0 ;  i < i32CurMember ; ++i )
	{
		T_UID TMemberUID = 0;
		INT32 i32SessionIdx = -1;
		pRecvPacket->ReadData(&TMemberUID,			sizeof(T_UID));
		pRecvPacket->ReadData(&i32SessionIdx,		sizeof(INT32));

		CUserSession* pMemberSession = GetSession(i32SessionIdx,TMemberUID ); 
		if( !pMemberSession ) continue;

		pMemberSession->S2MOSendEvent(pSendPacket);

	}
	return;
}

void	CUserSessionManager::OnSendClanMatchRoomLeave( T_UID TLeaderUID, INT32 i32CurMember, i3NetworkPacket* pRecvPacket, INT32 i32ChannelIdx  )
{
	i3NetworkPacket SendPacket(PROTOCOL_CLAN_WAR_FORCE_MATCH_ROOM_LEAVE_ACK);

	for( INT32 i = 0 ;  i < i32CurMember ; ++i )
	{
		T_UID TMemberUID = 0;
		INT32 i32SessionIdx = -1;
		pRecvPacket->ReadData(&TMemberUID,				sizeof(T_UID));
		pRecvPacket->ReadData(&i32SessionIdx,			sizeof(INT32));

		if( TLeaderUID == TMemberUID ) continue;	// 리더는 이미 방 퇴장처리가 끝난상태.

		CUserSession* pMemberSession = GetSession(i32SessionIdx,TMemberUID); 
		if( !pMemberSession ) continue;

		if( SERVER_VALUE_UNKNOWN == pMemberSession->m_i32ChannelNum ) continue;
		if( SERVER_VALUE_UNKNOWN == pMemberSession->m_i32RoomIdx ) continue;
		if( SERVER_VALUE_UNKNOWN == pMemberSession->m_i32SlotIdx ) continue;
		if( SERVER_VALUE_UNKNOWN == pMemberSession->m_i32SlotIdx ) continue;
		if( GAME_TASK_BATTLE	== pMemberSession->GetPosition() && pMemberSession->m_pRoom->IsGMPause()) continue;

		//GM에 의한 Kick여부를 확인하기 위해 bool값 변수를 추가로 보냅니다.
		bool			bIsGMKick	=	FALSE;
		SendPacket.WriteData( &bIsGMKick, sizeof(bool) );

		pMemberSession->SendPacketMessage(&SendPacket); 
		pMemberSession->m_bForceRoomLeave = TRUE; 
		pMemberSession->SetForceLeaveMatchRoom(true);

		// 리더에 의해 퇴장 당한 경우이므로 이탈 페널티를 초기화합니다.
		pMemberSession->SetMerPenaltyFlag(false);

		// 워크, 메인 쓰레드에서만 호출 가능하므로 사용하지 않습니다.
		//g_pRoomManager->OnLeaveRoom(pMemberSession, i32ChannelIdx, MODULE_CLAN_THREAD_IDX, ROOM_ENDTYPE_NONE);
	}
	return;
}
 
 
