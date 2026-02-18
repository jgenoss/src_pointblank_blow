#include "pch.h"
#include "UserManager.h"
#include "UserSession.h"
#include "WebManager.h"
#include "LoadOptionDB.h"

#define SERVER_START	1
#define SERVER_STOP		2

CSessionManager * g_pSessionManager = NULL;

I3_CLASS_INSTANCE( CSessionManager, i3ElementBase);

CSessionManager::CSessionManager()
{
}

CSessionManager::~CSessionManager()
{
	OnDestroy();
}

BOOL CSessionManager::OnCreate(void)
{	
	//1. Make TCP Socket 
	m_bAllServerStart	= FALSE;
	m_bAllServerStop	= FALSE;
	m_i32ServerStartStep = -1;

	m_timeout.tv_sec  = 0;
	m_timeout.tv_usec = 5;
	if(!m_AcceptSocket.OnCreate( g_pControl->m_ui32IP, g_pControl->m_ui16Port) )
	{
		return FALSE;
	}

	//2. Make Server Session
	CSession * pSession;
	for(UINT32 i = 0; i < ((g_pControl->GetAllServerCount() + g_pControl->GetSuperUserCount() ) * 3); i++) //잘못된 연결이 있을수 있 Session은 3배로 만들어서 대기합니다.
	{
		pSession = CSession::NewObject();
		pSession->m_i32Idx = i;
		m_SessionList.Add(pSession);
	}

	return TRUE; 
}

BOOL CSessionManager::OnDestroy(void)
{
	m_AcceptSocket.OnDestroy(); 

	CSession * pSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++) 
	{
		pSession = (CSession*)m_SessionList.GetItem(i); 
		I3_SAFE_RELEASE(pSession);
	}

	return TRUE;
}

/*----------------------------------------------------------
Name : OnUpdate 
Desc : 유저들을 상태를확인 합니다. 
----------------------------------------------------------*/
#define RECV_TIME_OUT			60*5			//5분입니다. 
#define KILL_USER_TIME			5

void CSessionManager::OnUpdate(void)
{
	wchar_t	wstrMessage[ MAX_PATH ];

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	{
		//1. connect 
		INT32	i32AddrSize;
		SOCKET	SocketAccept;
		struct	sockaddr_in FromAddr; 
		i32AddrSize = sizeof(sockaddr_in);
		SocketAccept = m_AcceptSocket.AcceptConnection( &m_timeout, &FromAddr, &i32AddrSize );

		if(SocketAccept != INVALID_SOCKET)
		{
			//Save Address Infomation 
			char * pIpString = NULL; 
			struct in_addr	ConIp; 
			ConIp.s_addr	=	FromAddr.sin_addr.s_addr;
			pIpString		=	inet_ntoa( ConIp );

			CSession * pNewSession = NULL;
			for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
			{
				pNewSession = (CSession*)m_SessionList.GetItem(i);
				if( !pNewSession->GetIsActive()) break;
				pNewSession = NULL;
			}

			if(pNewSession == NULL)
			{
				::shutdown(SocketAccept, SD_BOTH);
				::closesocket(SocketAccept);
			}
			else
			{
				pNewSession->OnCreate(SocketAccept, ConIp.s_addr , pIpString );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	{
		CSession * pSession; 
		//2. Update Server 	
		for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
		{
			pSession = (CSession*)m_SessionList.GetItem(i);
			if( !pSession->GetIsActive())
			{
				continue;
			}

			pSession->SelectEvent(); 

			//Kill Command Session 
			if(pSession->m_ui32IsKillST != 0)
			{
				if((pSession->m_ui32IsKillST + KILL_USER_TIME) < i3ThreadTimer::GetServerTime())
				{
					g_pLog->WriteLog( L"Kill Session : Kill Command" );
					pSession->OnDestroy();
				}
			}

			//Kill TimOut Session 
			if( (pSession->m_ui32LastRecvST + RECV_TIME_OUT) < i3ThreadTimer::GetServerTime() )
			{
				// TIME OUT으로 메일 전송했으므로 이값은 TRUE로. 이중 메일( DOWN ) 전송 방지.
				pSession->m_bIsNormalDisconnect = TRUE;

				g_pLog->WriteLog( L"[%d]Kill Server Session : RecvTimeOut", pSession->m_i32ServerIdx );
				pSession->OnDestroy();
			}
		}
	}


	for( UINT32 i = 0 ; i < WARNING_TYPE_COUNT; i++ ) // 컨트롤 서버만 Warning 일경우가 있기때문에 알려줘야 포맷할수있다.
	{
		if( WARNING_LEVEL_NO != g_pLog->GetWarning().GetWarningInfo(i) ) g_pControl->m_bUpdateServerState = TRUE;
	}
	
	g_pControl->MakeWarningMessage(); // 각서버 Waring Format

	//관제툴 인증하는 부분
	{
		CSession * pSession; 
		if( g_pLoadOptionDB->GetUserLogInCount() >0)
		{
			OUT_DB_SUPER_USER * pOut = g_pLoadOptionDB->GetUserLogIn(); 

			pSession = (CSession*)m_SessionList.GetItem( pOut->iSessionIdx );

			if( pSession->IsConnected() )
			{
				pSession->m_i8Auth_Service	= pOut->i8Service; 
				pSession->m_i32ConnectType	= ASC_CONNECT_TYPE_ADMIN; 

				SendConfirmationMessage( pSession, pOut->wstrID, pOut->i32Rv, pOut->i8Service );

				g_pLog->WriteLog( L"[ADMI] LogIn Confirmation Result - Rv:%d, Session:%d", pOut->i32Rv, pOut->iSessionIdx );//, pOut->strID );
			}

			g_pLoadOptionDB->GetUserLogInIdx();
		}
	}

	// Repeat Notice
	for( INT32 i = 0 ; i < NOTICE_COUNT ; i++ )
	{
		TYPE_RENOTICE* pReNotice = &g_pControl->m_aReNotice[i];

		if( FALSE == pReNotice->_bNoticeActive ) continue;

		// 이전에 공지를 보냈던 시간에 공지반복시간을 더해도 현재시간에 이르지 못하면 공지를 보내지 않고 패스(continue)합니다.
		if( pReNotice->_ui32NoticeSendTime + pReNotice->_ui32NoticeReTime > i3ThreadTimer::GetServerTime() ) continue;

		// 공지 시작,끝 범위 안에 들어있는지 판별합니다.
		if ( pReNotice->_ui32BeginTime > i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() ) continue;
		if ( pReNotice->_ui32FinishTime < i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI() ) continue;

		// 실제로 공지를 보냅니다.
		INT32 i32Len = (INT32)( i3String::Length( pReNotice->_strNotice ) +1);
		SendNoticeMessage( NOTICE_TYPE_EMERGENCY, pReNotice->_strNotice, i32Len );

		// 공지를 보냈으므로 현재시간을 공지를 보냈던 시간으로 기록합니다.
		pReNotice->_ui32NoticeSendTime = i3ThreadTimer::GetServerTime();
	}

	// Config Reload
	if( g_pControl->m_bReloadConfig )
	{
		g_pControl->m_bReloadConfig = FALSE;
		_SendGameReConfig();
		_SendTransReConfig();
		_SendDediReConfig();
	}
	
	{
		// All Server Stop
		if( m_bAllServerStop )
		{
			switch( m_i32ServerStep )
			{
			case 0:	// All User Kick
				{
					i3String::Format( wstrMessage, MAX_PATH, L"[All Server Stop] All User Kick Start" );
					SendControlAppMessage( wstrMessage );
					g_pLog->WriteLog( wstrMessage );

					SendAllUserDisconnect();
					m_i32ServerStep++;
				}
				break;
			case 1:	// Waiting for User Kick
				{
					BOOL bUserDisConnect = TRUE;
					for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
					{
						if( 0 == g_pControl->m_aGameServerList[i]._i32Arg0 ) continue;
						
						bUserDisConnect = FALSE;
					}

					if( bUserDisConnect )
					{
						m_ui32ServerDelayTime = i3ThreadTimer::GetServerTime();
						m_i32ServerStep++;
					}
				}
				break;
			case 2: // Teans User Kick
				{
					if( m_ui32ServerDelayTime + 20 < i3ThreadTimer::GetServerTime() )	// 20초를 더 기다린 후에 Trans UserKick 을 수행한다.
					{
						i3String::Format( wstrMessage, MAX_PATH, L"[All Server Stop] Trans User Kick Start" );
						SendControlAppMessage( wstrMessage );
						g_pLog->WriteLog( wstrMessage );

						SendTransUserKick();
						m_i32ServerStep++;
					}
				}
				break;
			case 3: // Waiting for Trans User Kick & Total Server Stop
				{
					for(UINT32 i = 0; i < g_pControl->m_ui32CommonServerCount; i++)
					{
						if( ASC_SERVER_TRANS != g_pControl->m_aCommonServerList[i]._ui8Type ) continue;

						if( 0 == g_pControl->m_aCommonServerList[i]._i32Arg1 )
						{
							i3String::Format( wstrMessage, MAX_PATH, L"[All Server Stop] Total Server Stop Start" );
							SendControlAppMessage( wstrMessage );
							g_pLog->WriteLog( wstrMessage );

							TotalStopServer();
							m_i32ServerStep++;
							m_bAllServerStop = FALSE;
						}
					}
				}
				break;
			}
		}

		{
			if(m_bAllServerStart)
			{
				UINT16 ui16Port = 39180;

				switch(m_i32ServerStartStep)
				{
				case ASC_SERVER_ZLOG:
					{
						i3String::Format( wstrMessage, MAX_PATH, L"[All Server Start] Start Process" );
						SendControlAppMessage( wstrMessage );
						g_pLog->WriteLog( wstrMessage );

						for( UINT32 i = 0 ; i < g_pControl->m_ui32CommonServerCount ; i++ )
						{
							if(g_pControl->m_aCommonServerList[i]._ui8Type == ASC_SERVER_ZLOG)
							{
								_SendServiceMessage( g_pControl->m_aCommonServerList[i]._ui32Ip, ui16Port, g_pControl->m_aCommonServerList[i]._ui8Type, SERVER_START );
								m_i32ServerStartStep = ASC_SERVER_SIA;
							}
						}
					}
					break;

				case ASC_SERVER_SIA:
					{
						for( UINT32 i = 0 ; i < g_pControl->m_ui32CommonServerCount ; i++ )
						{
							if(g_pControl->m_aCommonServerList[i]._ui8Type == ASC_SERVER_SIA)
							{
								_SendServiceMessage( g_pControl->m_aCommonServerList[i]._ui32Ip, ui16Port, g_pControl->m_aCommonServerList[i]._ui8Type, SERVER_START );
								m_i32ServerStartStep = ASC_SERVER_TRANS;
							}
						}
					}
					break;

				case ASC_SERVER_TRANS:
					{
						for( UINT32 i = 0 ; i < g_pControl->m_ui32CommonServerCount ; i++ )
						{
							if(g_pControl->m_aCommonServerList[i]._ui8Type == ASC_SERVER_TRANS)
							{
								_SendServiceMessage( g_pControl->m_aCommonServerList[i]._ui32Ip, ui16Port, g_pControl->m_aCommonServerList[i]._ui8Type, SERVER_START );
								m_i32ServerStartStep = ASC_SERVER_CLAN;
							}
						}
					}
					break;

				case ASC_SERVER_CLAN:
					{
						for( UINT32 i = 0 ; i < g_pControl->m_ui32CommonServerCount ; i++ )
						{
							if(g_pControl->m_aCommonServerList[i]._ui8Type == ASC_SERVER_CLAN)
							{
								_SendServiceMessage( g_pControl->m_aCommonServerList[i]._ui32Ip, ui16Port, g_pControl->m_aCommonServerList[i]._ui8Type, SERVER_START );
								m_i32ServerStartStep = ASC_SERVER_MESSENGER;
							}
						}
					}
					break;

				case ASC_SERVER_MESSENGER:
					{
						for( UINT32 i = 0 ; i < g_pControl->m_ui32CommonServerCount ; i++ )
						{
							if(g_pControl->m_aCommonServerList[i]._ui8Type == ASC_SERVER_MESSENGER)
							{
								_SendServiceMessage( g_pControl->m_aCommonServerList[i]._ui32Ip, ui16Port, g_pControl->m_aCommonServerList[i]._ui8Type, SERVER_START );
								m_i32ServerStartStep = ASC_SERVER_GAME;
							}
						}
					}
					break;

				case ASC_SERVER_GAME:
					{
						for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
						{
							_SendServiceMessage( g_pControl->m_aGameServerList[i]._ui32Ip, ui16Port, ASC_SERVER_GAME, SERVER_START );
						}

						m_i32ServerStartStep = ASC_SERVER_BATTLE;
					}
					break;

				case ASC_SERVER_BATTLE:
					{
						for( UINT32 i = 0 ; i < g_pControl->m_ui32BattleServerCount ; i++ )
						{
							if(g_pControl->m_aBattleServerList[i]._ui8Type == BATTLE_SERVER_TYPE_DEDICATION)
							{
								_SendServiceMessage( g_pControl->m_aBattleServerList[i]._ui32Ip, ui16Port, ASC_SERVER_BATTLE+1, SERVER_START );
							}
							else
							{
								_SendServiceMessage( g_pControl->m_aBattleServerList[i]._ui32Ip, ui16Port, ASC_SERVER_BATTLE, SERVER_START );
							}
						}

						i3String::Format( wstrMessage, MAX_PATH, L"[All Server Start] Process end" );
						SendControlAppMessage( wstrMessage );
						g_pLog->WriteLog( wstrMessage );

						m_i32ServerStartStep = -1;
						m_bAllServerStart = FALSE;
					}
					break;
				}
			}
		}
	}

	return;
}

void CSessionManager::_SendGameReConfig()
{
	i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIG_RELOAD_ACK );

	PacketMakeGameReConfig( &SendPacket );

	PacketMakeBannerURL(&SendPacket);

	CSession * pUserSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( NULL == pUserSession )										continue;
		if( FALSE == pUserSession->GetIsActive() )						continue;
		if( ASC_CONNECT_TYPE_SERVER != pUserSession->m_i32ConnectType )	continue;
		if( ASC_SERVER_GAME != pUserSession->m_i32ServerType )			continue;

		pUserSession->SendPacketMessage(&SendPacket);
	}

	return;
}

void CSessionManager::_SendTransReConfig()
{
	i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIG_RELOAD_ACK );

	PacketMakeTransReConfig( &SendPacket );

	CSession * pUserSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( NULL == pUserSession )										continue;
		if( FALSE == pUserSession->GetIsActive() )						continue;
		if( ASC_CONNECT_TYPE_SERVER != pUserSession->m_i32ConnectType )	continue;
		if( ASC_SERVER_TRANS != pUserSession->m_i32ServerType )			continue;

		pUserSession->SendPacketMessage(&SendPacket);
	}

	return; 
}

void CSessionManager::_SendDediReConfig()
{
	i3NetworkPacket SendPacket( PROTOCOL_ASC_CONFIG_RELOAD_ACK );

	PacketMakeDediReConfig( &SendPacket );

	CSession * pUserSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( NULL == pUserSession )										continue;
		if( FALSE == pUserSession->GetIsActive() )						continue;
		if( ASC_CONNECT_TYPE_SERVER != pUserSession->m_i32ConnectType )	continue;
		if( ASC_SERVER_BATTLE != pUserSession->m_i32ServerType )		continue;
		if( g_pControl->m_aBattleServerList[pUserSession->m_i32ServerIdx]._ui8Type != BATTLE_SERVER_TYPE_DEDICATION) continue; 
		pUserSession->SendPacketMessage(&SendPacket);
	}

	return;
}

void CSessionManager::PacketMakeGameReConfig( i3NetworkPacket* pSendPacket )
{
	UINT8	ui8ClanCreateRank = (UINT8)g_pControl->m_i32ClanCreateRank;
	pSendPacket->WriteData( &ui8ClanCreateRank,							sizeof(UINT8) );
	pSendPacket->WriteData( &g_pControl->m_i32ClanCreatePoint,			sizeof(INT32) );
	pSendPacket->WriteData( &g_pControl->m_ui16BoostEventCount,			sizeof(UINT16) );
	pSendPacket->WriteData( g_pControl->m_stBoostEvent,					(UINT16)(sizeof(BOOST_EVENT)*g_pControl->m_ui16BoostEventCount) );
	pSendPacket->WriteData( g_pControl->m_aRankUpItem,					sizeof(RANKUP_ITEM) * MAX_RANK_COUNT);
	pSendPacket->WriteData( &g_pControl->m_aTSEvent,					sizeof(TS_EVENT_INFO)*TS_EVENT_COUNT);

	UINT8	ui8GameServerCount = (UINT8)g_pControl->m_ui32GameServerCount; // 게임서버 최대개수가 100개임으로 1바이트 만큼만 사이즈를 잡아서 보내면됩니다.
	pSendPacket->WriteData( &ui8GameServerCount,	sizeof(UINT8) );

	for( UINT8 i = 0 ; i < ui8GameServerCount ; i++ )
	{
		pSendPacket->WriteData( &g_pControl->m_aGameServerList[ i ]._ui16NowUserCount, sizeof(UINT16)  );
	}
		
	pSendPacket->WriteData( &g_pControl->m_ui8TagCount,						sizeof(UINT8 ) );
	pSendPacket->WriteData( g_pControl->m_ui8TagPriority,					sizeof(UINT8 ) * g_pControl->m_ui8TagCount );

	return;
}

void CSessionManager::PacketMakeTransReConfig( i3NetworkPacket* pSendPacket )
{
	UINT32 ui32NewUserPoint = (UINT32)g_pControl->m_i32NewUserPoint;
	pSendPacket->WriteData( &ui32NewUserPoint,							sizeof(UINT32) );

	pSendPacket->WriteData( g_pControl->m_i32GeneralRank,				sizeof(INT32)*GENERAL_RANK_COUNT );
	pSendPacket->WriteData( &g_pControl->m_i32GeneralRankStartTime,		sizeof(INT32) );

	pSendPacket->WriteData( &g_pControl->m_ui16NewUserEventCount,		sizeof(UINT16) );
	pSendPacket->WriteData( g_pControl->m_stNewUserEvent,				(UINT16)(sizeof(NEW_USER_EVENT)*g_pControl->m_ui16NewUserEventCount) );

	pSendPacket->WriteData( &g_pControl->m_ui16LoginUserEventCount,		sizeof(UINT16) );
	pSendPacket->WriteData( g_pControl->m_stLoginUserEvent,				(UINT16)(sizeof(LOGIN_USER_EVENT)*g_pControl->m_ui16LoginUserEventCount) );

	pSendPacket->WriteData( &g_pControl->m_ui8IPBlockIPCount,			sizeof(UINT8) );
	pSendPacket->WriteData( g_pControl->m_stIPBlockIPOpen,				(UINT16)(sizeof(IPBLOCK_IP)*g_pControl->m_ui8IPBlockIPCount) );

	pSendPacket->WriteData( &g_pControl->m_ui8IPBlockNationCount,		sizeof(UINT8) );
	pSendPacket->WriteData( g_pControl->m_ui8IPBlockNationOpen,			(UINT16)(sizeof(UINT8)*g_pControl->m_ui8IPBlockNationCount) );
}

void CSessionManager::PacketMakeDediReConfig( i3NetworkPacket* pSendPacket )
{
	UINT32 ui32SIAVersion = g_pControl->GetSIAVersion();
	pSendPacket->WriteData( &ui32SIAVersion,		sizeof(UINT32) );
}

void CSessionManager::PacketMakeBannerURL(i3NetworkPacket* pSendPacket)
{
	//Set URL List 
	pSendPacket->WriteData(&g_pControl->m_ui8URLVersion, sizeof(UINT8));
	pSendPacket->WriteData(&g_pControl->m_ui8URLListCount, sizeof(UINT8));

	UINT16		ui16Len = 0;
	URL_LIST	*pulURLList = nullptr;

	for (INT32 i = 0; i < g_pControl->m_ui8URLListCount; i++)
	{
		pulURLList = &g_pControl->m_ulURLList[i];
		ui16Len = (UINT16)(i3String::Length(pulURLList->m_strURL));
		pSendPacket->WriteData(&ui16Len, sizeof(UINT16));
		pSendPacket->WriteData(&pulURLList->m_i32Type, sizeof(INT32));
		pSendPacket->WriteData(&pulURLList->m_i32Data, sizeof(INT32));
		pSendPacket->WriteData(pulURLList->m_strURL, sizeof(wchar_t) *ui16Len);
	}
}

/*----------------------------------------------------------
Name : SendServerLog 
Desc : 메시지를 보낸다. 
----------------------------------------------------------*/
void CSessionManager::SendServerLog(void)
{
	UINT32 i;

	i3NetworkPacket SendPacket( PROTOCOL_ASC_STATE_ACK );
	SendPacket.WriteData( &g_pControl->m_i32CCUMaxCount_Day,		sizeof(INT32) );
	SendPacket.WriteData( &g_pControl->m_ui32CCUMaxTime_Day,		sizeof(UINT32) );
	SendPacket.WriteData( &g_pControl->m_i32CCUNowCount,			sizeof(UINT32) );
	SendPacket.WriteData( &g_pControl->m_ui32CCUNowTime,			sizeof(UINT32) );

	for( i = 0 ; i < g_pControl->m_ui32CommonServerCount; i++ )
	{
		SendPacket.WriteData( &g_pControl->m_aCommonServerList[i],	sizeof(SERVER_INFO_COMMON) );
	}
	for( i = 0 ; i < g_pControl->m_ui32GameServerCount; i++ )
	{
		SendPacket.WriteData( &g_pControl->m_aGameServerList[i],	sizeof(SERVER_INFO_COMMON) );
	}
	for( i = 0 ; i < g_pControl->m_ui32BattleServerCount; i++ )
	{
		SendPacket.WriteData( &g_pControl->m_aBattleServerList[i],	sizeof(SERVER_INFO_COMMON) );
	}
	for( i = 0 ; i < g_pControl->m_ui32CastServerCount; i++ )
	{
		SendPacket.WriteData( &g_pControl->m_CastServerList[i],		sizeof(SERVER_INFO_COMMON) );
	}
	for( i = 0 ; i < g_pControl->m_ui32UpdateServerCount; i++ )
	{
		SendPacket.WriteData( &g_pControl->m_aUpdateServerList[i],		sizeof(SERVER_INFO_COMMON) );
	}
	SendPacket.WriteData(g_pControl->m_pui32DayHackCount,			(UINT16)(sizeof(UINT32) * HACK_TYPE_MAX));

	CSession * pUserSession;
	for( INT32 i32Idx = 0 ; i32Idx < m_SessionList.GetCount() ; i32Idx++ )
	{
		pUserSession = (CSession*)m_SessionList.GetItem( i32Idx );
		if( FALSE == pUserSession->GetIsActive() )						continue;
		if( ASC_CONNECT_TYPE_ADMIN != pUserSession->m_i32ConnectType )	continue;
		pUserSession->SendPacketMessage(&SendPacket);
	}

	return;
}


void CSessionManager::SendServerError(INT32 i32ServerIdx, INT32 i32DestroyReason)
{
	CSession * pUserSession; 
	i3NetworkPacket SendPacket(PROTOCOL_ASC_SERVER_LOADING_RESULT);	

	SendPacket.WriteData(&i32ServerIdx,		sizeof(INT32));
	SendPacket.WriteData(&i32DestroyReason, sizeof(INT32));

	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( !pUserSession->GetIsActive())continue; 
		if( pUserSession->m_i32ConnectType != ASC_CONNECT_TYPE_ADMIN )continue;
		pUserSession->SendPacketMessage(&SendPacket);
	}

	return;
}

void CSessionManager::SendAllUserDisconnect(void)
{
	CSession * pUserSession; 
	i3NetworkPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_ACK );

	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( FALSE == pUserSession->GetIsActive())						continue;
		if( ASC_CONNECT_TYPE_ADMIN == pUserSession->m_i32ConnectType )	continue;
		if( ASC_SERVER_GAME != pUserSession->m_i32ServerType )			continue;
		pUserSession->SendPacketMessage(&SendPacket);
	}
	return; 
}

void CSessionManager::SendOneUserDisconnect(UINT32 i32Idx)
{
	if( i32Idx >= g_pControl->m_ui32GameServerCount )		return;

	CSession * pUserSession = (CSession*)g_pControl->m_aGameServerList[i32Idx]._pSession;
	if( NULL == pUserSession )
	{
		g_pLog->WriteLog( L"SendOneUserDisconnect Session");
		return;
	}
	if( !pUserSession->GetIsActive())
	{
		g_pLog->WriteLog( L"SendOneUserDisconnect Active ");
		return;
	}
	if( pUserSession->m_i32ServerType != ASC_SERVER_GAME)
	{
		g_pLog->WriteLog( L"SendOneUserDisconnect Type:%d", pUserSession->m_i32ServerType );
		return;
	}

	i3NetworkPacket SendPacket( PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_ACK );
	pUserSession->SendPacketMessage(&SendPacket);

	return;
}

void CSessionManager::SendNoticeMessage(INT32 i32type, TTCHAR * pstrMessage, INT32 i32strLen, INT32 i32RGB)
{
	CSession * pUserSession; 
	i3NetworkPacket SendPacket(PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_ACK);
	SendPacket.WriteData(&i32type,		sizeof(INT32));
	SendPacket.WriteData(&i32strLen,	sizeof(INT32));
	SendPacket.WriteData(pstrMessage,	sizeof(TTCHAR) * (UINT16)i32strLen);
	SendPacket.WriteData(&i32RGB, sizeof(INT32));

	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( !pUserSession->GetIsActive())continue; 
		if( pUserSession->m_i32ConnectType == ASC_CONNECT_TYPE_ADMIN )continue;
		if( pUserSession->m_i32ServerType != ASC_SERVER_GAME)continue;
		pUserSession->SendPacketMessage(&SendPacket);
	}
	return;
}

void CSessionManager::SendMD5Key(void)
{
	CSession * pUserSession; 

	i3NetworkPacket SendPacket( PROTOCOL_ASC_SET_MD5_KEY_ACK );
	SendPacket.WriteData(g_pControl->m_strKey1,	NET_MD5_KEY_SIZE);
	SendPacket.WriteData(g_pControl->m_strKey2,	NET_MD5_KEY_SIZE);

	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( !pUserSession->GetIsActive())continue; 
		if( pUserSession->m_i32ConnectType == ASC_CONNECT_TYPE_ADMIN )continue;
		if( pUserSession->m_i32ServerType != ASC_SERVER_GAME)continue;
		pUserSession->SendPacketMessage(&SendPacket);
	}
	return;
}

void CSessionManager::SendTransUserKick(void)
{
	CSession * pUserSession; 
	i3NetworkPacket SendPacket(PROTOCOL_ASC_ADMIN_KICK_TRANS_USER_ACK);
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( !pUserSession->GetIsActive())continue; 
		if( pUserSession->m_i32ConnectType == ASC_CONNECT_TYPE_ADMIN )	continue;
		if( pUserSession->m_i32ServerType != ASC_SERVER_TRANS)			continue;
		pUserSession->SendPacketMessage(&SendPacket);
	}

	return;
}

// 모든 Control App에 메시지를 보낸다.
void CSessionManager::SendControlAppMessage( wchar_t * strMessage )
{
	INT32 i32Len = (INT32)(i3String::Length( strMessage )+1);

	CSession * pUserSession;
	i3NetworkPacket SendPacket(PROTOCOL_ASC_ADMIN_CONTROL_ANNOUNCE_REQ);
	SendPacket.WriteData( &i32Len,		(UINT16)sizeof(INT32) );
	SendPacket.WriteData( strMessage,	(UINT16)(sizeof(wchar_t)*i32Len) );

	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( !pUserSession->GetIsActive())continue; 
		if( pUserSession->m_i32ConnectType != ASC_CONNECT_TYPE_ADMIN )continue;
		pUserSession->SendPacketMessage(&SendPacket);
	}
}

// Send hackCheck ratio to all control tools
void CSessionManager::SendToAllControlTools(const char * pData, INT32 i32DataSize)
{
	CSession * pUserSession = NULL;

	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if( !pUserSession->GetIsActive())continue; 
		if( pUserSession->m_i32ConnectType != ASC_CONNECT_TYPE_ADMIN )continue;
		pUserSession->SendPacketMessage(pData, i32DataSize);
	}
}

void CSessionManager::TotalStopServer()
{
	UINT16 ui16Port = 39180;

	for( UINT32 i = 0 ; i < g_pControl->m_ui32CommonServerCount ; i++ )
	{
		_SendServiceMessage( g_pControl->m_aCommonServerList[i]._ui32Ip, ui16Port, g_pControl->m_aCommonServerList[i]._ui8Type, SERVER_STOP );
	}

	for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
	{
		_SendServiceMessage( g_pControl->m_aGameServerList[i]._ui32Ip, ui16Port, ASC_SERVER_GAME, SERVER_STOP );
	}

	for( UINT32 i = 0 ; i < g_pControl->m_ui32BattleServerCount ; i++ )
	{
		if(g_pControl->m_aBattleServerList[i]._ui8Type == BATTLE_SERVER_TYPE_DEDICATION)
		{
			_SendServiceMessage( g_pControl->m_aBattleServerList[i]._ui32Ip, ui16Port, ASC_SERVER_BATTLE+1, SERVER_STOP );
		}
		else
		{
			_SendServiceMessage( g_pControl->m_aBattleServerList[i]._ui32Ip, ui16Port, ASC_SERVER_BATTLE, SERVER_STOP );
		}
	}
	for (UINT32 i = 0; i < g_pControl->m_ui32CastServerCount; i++)
	{
		_SendServiceMessage(g_pControl->m_CastServerList[i]._ui32Ip, ui16Port, ASC_SERVER_CAST, SERVER_STOP);
	}

	for( UINT32 i = 0 ; i < g_pControl->m_ui32UpdateServerCount ; i++ )
	{
		_SendServiceMessage( g_pControl->m_aUpdateServerList[i]._ui32Ip, ui16Port, ASC_SERVER_UPDATER, SERVER_STOP );
	}

	_SendServiceMessage( g_pControl->m_ui32IP, ui16Port, ASC_SERVER_CONTROL, SERVER_STOP );
}

void CSessionManager::_SendServiceMessage( UINT32 ui32IP, UINT16 ui16Port, UINT8 ui8Type, INT8 i8State  )
{
	//새로운 SC 적용
	i3NetworkClientSocket	SCSocket;

	if( SCSocket.OnCreate(ui32IP, ui16Port) )
	{
		Sleep(10);
		
		wchar_t				strIp[32];

		i3Net::GetIPToLongW( ui32IP, strIp );

		P_SC_SERVER_OPER_REQ SetPacket;
		SetPacket._i8ServerOper = i8State;
		i3String::Copy( SetPacket._strIp,			strIp,									IP_ADDR_SIZE );
		i3String::Copy( SetPacket._strServerName,	g_pControl->m_astrServiceName[ui8Type], SERVICE_NAME_COUNT );

		i3NetworkPacket SendPacket;	
		SendPacket.SetProtocol(PROTOCOL_SC_SERVER_OPER_REQ);
		SendPacket.WriteData(&SetPacket, sizeof(P_SC_SERVER_OPER_REQ));
		SCSocket.SendPacketMessage(&SendPacket);

		Sleep(1);
		g_pLog->WriteLog( L"[MAIN] Connect Service Success");
	}
	else
	{
		g_pLog->WriteLog( L"[MAIN] Connect Service Fail");
	}
}

//서버 Start
void CSessionManager::AllStartServer()
{
	m_i32ServerStartStep = ASC_SERVER_ZLOG;
	m_bAllServerStart = TRUE;
}

void CSessionManager::AllStopServer()
{
	m_i32ServerStep = 0;
	m_bAllServerStop = TRUE;
}

BOOL CSessionManager::StartServer(CSession * pSession, UINT8 ui8Type, UINT32 i32Idx )
{
	BOOL Rv = TRUE;

	UINT32	ui32IP;
	UINT16	ui16Port = 39180;

	switch( ui8Type )
	{
	case ASC_SERVER_GAME	:
		if( i32Idx >= g_pControl->m_ui32GameServerCount )	return FALSE;
		ui32IP		= g_pControl->m_aGameServerList[i32Idx]._ui32Ip;
		break; 
	case ASC_SERVER_BATTLE	:
		if( i32Idx >= g_pControl->m_ui32BattleServerCount )	return FALSE;
		ui32IP		= g_pControl->m_aBattleServerList[i32Idx]._ui32Ip;
		break;
	case ASC_SERVER_UPDATER	:
		if( i32Idx >= g_pControl->m_ui32UpdateServerCount )	return FALSE;
		ui32IP		= g_pControl->m_aUpdateServerList[i32Idx]._ui32Ip;
		break; 
	case ASC_SERVER_CAST:
		if (i32Idx >= g_pControl->m_ui32CastServerCount)	return FALSE;
		ui32IP = g_pControl->m_CastServerList[i32Idx]._ui32Ip;
		break;
	default					:
		if( i32Idx >= g_pControl->m_ui32CommonServerCount )	return FALSE;
		ui32IP		= g_pControl->m_aCommonServerList[i32Idx]._ui32Ip;
	}	

	_SendServiceMessage( ui32IP, ui16Port, ui8Type, SERVER_START );	

	return Rv;
}
//서버 Stop
BOOL CSessionManager::StopServer(CSession * pSession, UINT8 ui8Type, UINT32 i32Idx )
{
	BOOL bRv = TRUE;

	UINT32 ui32IP;
	UINT16 ui16Port = 39180;

	switch( ui8Type )
	{
	case ASC_SERVER_GAME	: 
		if( i32Idx >= g_pControl->m_ui32GameServerCount )	return FALSE;
		ui32IP	= g_pControl->m_aGameServerList[i32Idx]._ui32Ip;
		break; 
	case ASC_SERVER_BATTLE	:
		if( i32Idx >= g_pControl->m_ui32BattleServerCount )	return FALSE;
		ui32IP	= g_pControl->m_aBattleServerList[i32Idx]._ui32Ip;
		break; 
	case ASC_SERVER_UPDATER	: 
		if( i32Idx >= g_pControl->m_ui32UpdateServerCount )	return FALSE;
		ui32IP	= g_pControl->m_aUpdateServerList[i32Idx]._ui32Ip;
		break; 
	case ASC_SERVER_CAST:
		if (i32Idx >= g_pControl->m_ui32CastServerCount)	return FALSE;
		ui32IP = g_pControl->m_CastServerList[i32Idx]._ui32Ip;
		break;
	default					:
		if( i32Idx >= g_pControl->m_ui32CommonServerCount )	return FALSE;
		ui32IP	= g_pControl->m_aCommonServerList[i32Idx]._ui32Ip;
	}	

	_SendServiceMessage( ui32IP, ui16Port, ui8Type, SERVER_STOP );
	return bRv;
}

void CSessionManager::SendToDediServer(UINT8 iServer, const char * pData, INT32 iDataSize)
{
	SERVER_INFO_BATTLE * pBattleInfo = &g_pControl->m_aBattleServerList[iServer];
	if (pBattleInfo->_ui8Type != BATTLE_SERVER_TYPE_DEDICATION) return;
	CSession * pBattleServer = (CSession *)pBattleInfo->_pSession;

	if (NULL == pBattleServer)					return;
	if (FALSE == pBattleServer->GetIsActive())	return;

	pBattleServer->SendPacketMessage(pData, iDataSize);
	return;
}

void CSessionManager::SendCastServer( UINT8 iServer, const char * pData, INT32 iDataSize )
{
	SERVER_INFO_CAST * pServerInfo = &g_pControl->m_CastServerList[ iServer ];
	if (pServerInfo->_ui8Type != ASC_SERVER_CAST ) return;

	CSession * pUserSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if(	pUserSession->GetIsActive()								&&
			pUserSession->m_i32ConnectType == ASC_CONNECT_TYPE_SERVER	&&
			pUserSession->m_i32ServerType == ASC_SERVER_CAST )
		{
			pUserSession->SendPacketMessage( pData, iDataSize );
		}
	}
}

//BOOL CSessionManager::DedicationFillWeaponList( N_DSERVER_WEAPON_LIST* pListReq) // 무기리스트 요청
//{
//	UINT32  pui32WeaponList[DS_MAX_WEAPON_LIST] = { 0, };
//	UINT32	ui32WeaponCount = 0;
//	
//	// 무기 리스트 얻기 (count와 list의 일관성을 위해 한번에 읽어야 한다)
//	if( ! g_pControl->GetWeaponList( pui32WeaponList, sizeof(pui32WeaponList),  &ui32WeaponCount) ) return FALSE;
//
//	// 요청 범위 체크
//	INT32 i32StartIndex = pListReq->m_i32StartIndex; // copy
//	if( i32StartIndex < 0) i32StartIndex = 0;
//	if( i32StartIndex > INT32(ui32WeaponCount - 1) ) return FALSE;
//
//	// 전송할 크기 결정
//	UINT32 ui32SendItems = ui32WeaponCount - i32StartIndex;
//	ui32SendItems = ( ui32SendItems < DS_MAX_WEAPONS_PER_PACKET ) ? ui32SendItems : DS_MAX_WEAPONS_PER_PACKET;
//	if( ui32SendItems < 1 ) return FALSE;
//
//	// 컨트롤 서버에서 전송하는 weapon list의 모든 멤버는 valid해야 한다.
//	pListReq->m_i32StartIndex		= i32StartIndex;
//	pListReq->m_ui32ItemCount		= ui32SendItems;
//	pListReq->m_ui32TotalItemCount	= ui32WeaponCount;
//	pListReq->m_ui32SIAVersion		= g_pControl->GetSIAVersion();
//
//	memcpy_s( pListReq->m_weaponList, sizeof(pListReq->m_weaponList), 
//			  pui32WeaponList + i32StartIndex, ui32SendItems * sizeof(UINT32) );
//
//	return TRUE;
//}


void CSessionManager::SendToGameServer( UINT8 ui8GameServer, const char * pData, INT32 i32DataSize )
{
	SERVER_INFO_GAME * pGameControl	= &g_pControl->m_aGameServerList[ ui8GameServer ];
	CSession * pUserSession = (CSession*)pGameControl->_pSession;

	if( NULL == pUserSession )					return;
	if( FALSE == pUserSession->GetIsActive() )	return;

	pUserSession->SendPacketMessage(pData, i32DataSize);

	return;
}

void CSessionManager::SendKickUserWebReq(INT32 i32WebSessionIdx, INT8 i8Count, INT64 *pi64UID)
{
	INT32 i32Rv = EVENT_ERROR_FAIL;

	i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_GM_KICK_REQ );
	SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );
	SendPacket.WriteData( &i8Count,				sizeof(INT8) );
	SendPacket.WriteData( pi64UID,				sizeof(INT64)*i8Count );

	// 트랜스 서버로 KICK 요청 전달
	CSession * pUserSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if(	pUserSession->GetIsActive()								&&
			pUserSession->m_i32ConnectType == ASC_CONNECT_TYPE_SERVER	&&
			pUserSession->m_i32ServerType == ASC_SERVER_TRANS			)
		{
			pUserSession->SendPacketMessage(&SendPacket);
			i32Rv = EVENT_ERROR_SUCCESS;
		}
	}

	// 트랜스 서버로 요청 전달에 실패했으면 실패 결과 전달
	if( EV_FAILED(i32Rv) )
	{
		g_pWebSessionManager->RecvKickUserWebAck( i32WebSessionIdx, i32Rv );
	}
}

void CSessionManager::RecvKickUserWebAck(INT32 i32WebSessionIdx, INT32 i32Rv)
{
	g_pWebSessionManager->RecvKickUserWebAck( i32WebSessionIdx, i32Rv );
}

void CSessionManager::SendUserInfoLocationWebReq(INT32 i32WebSessionIdx, INT64 i64UID)
{
	INT32 i32Rv = EVENT_ERROR_FAIL;

	i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_USER_INFO_LOCATION_REQ );
	SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );
	SendPacket.WriteData( &i64UID,				sizeof(INT64) );

	// 트랜스 서버로 유저 위치 정보 요청 전달
	CSession * pUserSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if(	pUserSession->GetIsActive()								&&
			pUserSession->m_i32ConnectType == ASC_CONNECT_TYPE_SERVER	&&
			pUserSession->m_i32ServerType == ASC_SERVER_TRANS			)
		{
			pUserSession->SendPacketMessage(&SendPacket);
			i32Rv = EVENT_ERROR_SUCCESS;
		}
	}

	// 트랜스 서버로 요청 전달에 실패했으면 실패 결과 전달
	if( EV_FAILED(i32Rv) )
	{
		g_pWebSessionManager->RecvUserInfoLocationWebAck( i32WebSessionIdx, i32Rv );
	}
}

void CSessionManager::RecvUserInfoLocationWebAck(INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8ServerIdx, INT8 i8ChannelNum, INT32 i32RoomIdx)
{
	g_pWebSessionManager->RecvUserInfoLocationWebAck( i32WebSessionIdx, i32Rv, i8ServerIdx, i8ChannelNum, i32RoomIdx );
}

void CSessionManager::SendClanInfoBattleWebReq(INT32 i32WebSessionIdx, UINT32 ui32CID)
{
	INT32 i32Rv = EVENT_ERROR_FAIL;

	i3NetworkPacket SendPacket( PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_REQ );
	SendPacket.WriteData( &i32WebSessionIdx,	sizeof(INT32) );
	SendPacket.WriteData( &ui32CID,				sizeof(UINT32) );

	// 클랜 서버로 클랜전 여부 요청 전달
	CSession * pUserSession; 
	for(INT32 i = 0; i < m_SessionList.GetCount(); i++)
	{
		pUserSession = (CSession*)m_SessionList.GetItem(i);
		if(	pUserSession->GetIsActive()								&&
			pUserSession->m_i32ConnectType == ASC_CONNECT_TYPE_SERVER	&&
			pUserSession->m_i32ServerType == ASC_SERVER_CLAN			)
		{
			pUserSession->SendPacketMessage(&SendPacket);
			i32Rv = EVENT_ERROR_SUCCESS;
		}
	}

	// 트랜스 서버로 요청 전달에 실패했으면 실패 결과 전달
	if( EV_FAILED(i32Rv) )
	{
		g_pWebSessionManager->RecvClanInfoBattleWebAck( i32WebSessionIdx, i32Rv );
	}
}

void CSessionManager::RecvClanInfoBattleWebAck(INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8Count)
{
	g_pWebSessionManager->RecvClanInfoBattleWebAck( i32WebSessionIdx, i32Rv, i8Count );
}

void CSessionManager::SendConfirmationMessage( CSession* pSession, wchar_t* strID, INT32 i32Result, INT8 i8Service )
{
	i3NetworkPacket SendPacket;	
	SendPacket.Clear();
	SendPacket.SetProtocol( PROTOCOL_ASC_CONFIRMATION_ACK );
	SendPacket.WriteData( &i32Result,										sizeof(INT32) );
	if( EV_SUCCESSED( i32Result ) )
	{
		SendPacket.WriteData( &g_pControl->m_ui32CommonServerCount,			sizeof(UINT32) );
		SendPacket.WriteData( &g_pControl->m_ui32GameServerCount,			sizeof(UINT32) );
		SendPacket.WriteData( &g_pControl->m_ui32BattleServerCount,			sizeof(UINT32) );
		SendPacket.WriteData( &g_pControl->m_ui32CastServerCount,			sizeof(UINT32) );
		SendPacket.WriteData( &g_pControl->m_ui32UpdateServerCount,			sizeof(UINT32) );
		SendPacket.WriteData( &i8Service,									sizeof(INT8) );

		// HackCheck Ratio 전송
		SendPacket.WriteData(g_pControl->m_pDediHackCheckRatioList, (UINT16)(sizeof(ServerHackCheckRatio) * g_pControl->m_ui32BattleServerCount));

		// Room 생성 허용 여부 전송
		SendPacket.WriteData( g_pControl->m_pui8NotAllowRoomCreationList,	(UINT16)(sizeof(INT8) * g_pControl->m_ui32BattleServerCount) );

		// HackCheck Ratio 전송 - Game
		SendPacket.WriteData(g_pControl->m_pGameHackCheckRatioList, (UINT16)(sizeof(GSHackCheckRatio) * g_pControl->m_ui32GameServerCount));

		// ControlTool 로 전송
		wchar_t wstrMessage[ MAX_PATH ];
		i3String::Format( wstrMessage, MAX_PATH, L"Login Admin [ ID:%s, Auth:%d ]", strID, i8Service );
		SendControlAppMessage( wstrMessage );
	}
	pSession->SendPacketMessage( &SendPacket ); 
}