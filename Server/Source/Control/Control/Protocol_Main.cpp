#include "pch.h"
#include "UserManager.h"
#include "UserSession.h"
#include "LoadOptionDB.h"

#define SERVER_START	1
#define SERVER_STOP		2

#define PROTOCOL_BASE_HEART_BIT_UPDATESERVER_REQ 2575

void CSession::PacketParsing( i3NetworkPacket* pPacket )
{
	PROTOCOL		ProtocolID;
	BOOL			bSendMessage = FALSE;
	
	i3NetworkPacket SendPacket;
	SendPacket.Clear();

	wchar_t	wstrMessage[LOG_STRING_LEN];

	ProtocolID = pPacket->GetProtocolID();

	switch( ProtocolID )
	{
	case PROTOCOL_BASE_HEART_BIT_UPDATESERVER_REQ	:
	case PROTOCOL_BASE_HEART_BIT_REQ			:
		{
			bSendMessage = TRUE;
			SendPacket.SetProtocol( PROTOCOL_BASE_HEART_BIT_ACK );
		}
		break;
	case PROTOCOL_ASC_ADMIN_GAMESERVER_INFO_REQ	:
		{
			bSendMessage = TRUE; 
			SendPacket.SetProtocol( PROTOCOL_ASC_ADMIN_GAMESERVER_INFO_REQ );
		}
		break; 
	case PROTOCOL_BASE_GAME_SERVER_STATE_REQ	:
		{
			bSendMessage = TRUE;
			SendPacket.SetProtocol( PROTOCOL_BASE_GAME_SERVER_STATE_ACK );
			SendPacket.WriteData( &g_pControl->m_ui32GameServerCount,			sizeof( UINT32 ));
			GAME_SERVER_INFO pServerInfo[ MAX_SERVER_COUNT ];
			for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
			{
				pServerInfo[i]._IsActive			= g_pControl->m_aGameServerList[i]._i32IsActive;
				pServerInfo[i]._Arg0				= g_pControl->m_aGameServerList[i]._i32Arg0;
				pServerInfo[i]._AllType				= g_pControl->m_aGameServerList[i]._AllType;
				pServerInfo[i]._OutIP				= g_pControl->m_aGameServerList[i]._OutIP;
				pServerInfo[i]._OutPort				= g_pControl->m_aGameServerList[i]._OutPort;
				pServerInfo[i]._ui16MaxUserCount	= g_pControl->m_aGameServerList[i]._ui16MaxUserCount;
				i3mem::Copy( pServerInfo[i].m_aui8ChannelType, g_pControl->m_aGameServerList[i]._EachType, sizeof(UINT8) * GAME_CHANNEL_COUNT );				
			}
			SendPacket.WriteData( pServerInfo,	(UINT16)(sizeof(GAME_SERVER_INFO)*g_pControl->m_ui32GameServerCount) );

			UINT16 ui16ChannelUserCount;
			for( UINT32 i = 0 ; i < g_pControl->m_ui32GameServerCount ; i++ )
			{
				for( INT32 j = 0 ; j < GAME_CHANNEL_COUNT ; j++ )
				{
					ui16ChannelUserCount = static_cast<UINT16>( g_pControl->m_aGameServerList[i]._UserCount[j] );
					SendPacket.WriteData( &ui16ChannelUserCount,	sizeof(UINT16) );
				}
			}
		}
		break; 
	case PROTOCOL_ASC_CONFIRMATION_REQ:			//컨넥후 처음 던지는 메시지 자신이 서버 인지: break;  유저인지를 보냄.
		{
			INT32	i32ConnectType;
			INT32	i32Rv = 0;

			pPacket->ReadData(&i32ConnectType, sizeof(INT32)); //IsType

			SendPacket.SetProtocol( PROTOCOL_ASC_CONFIRMATION_ACK );

            switch( i32ConnectType )
			{
			case ASC_CONNECT_TYPE_SERVER	: //서버 
				{
					_PacketConfirmation( pPacket );			
				}
				break; 
			case ASC_CONNECT_TYPE_ADMIN		: //관제툴 
				{
					UINT64	ui64MacAddress;
					pPacket->ReadData( m_strId,			sizeof(wchar_t)*NET_ID_SIZE );
					pPacket->ReadData( m_strPass,		sizeof(wchar_t)*NET_PW_SIZE );
					pPacket->ReadData( &ui64MacAddress,	sizeof(UINT64) );
					
					m_strId[NET_ID_SIZE	- 1] = 0;
					m_strPass[NET_PW_SIZE	- 1] = 0;

					// 1. 아이디 검사를 합니다.
					if( FALSE == g_pControl->CheckMacAddress( ui64MacAddress ) )
					{
						_KillUser();
						
						g_pLog->WriteLog( L"[ADMI] LogIn Confirmation Fail - MacAddress" );

						i32Rv = -1;
						bSendMessage = TRUE;
						SendPacket.WriteData(&i32Rv,sizeof(INT32));
					}
					else if( 0 > g_pLoadOptionDB->UserLogIn(m_i32Idx, m_strId, m_strPass) )
					{
						i32Rv = -1; 
						_KillUser();
						g_pLog->WriteLog( L"[ADMI] LogIn Confirmation Fail - Buffer Full" );

						bSendMessage = TRUE;
						SendPacket.WriteData(&i32Rv,sizeof(INT32));
					}
					else
					{
						g_pLog->WriteLog( L"[ADMI] LogIn Confirmation - Session:%d, ID:%s", m_i32Idx, m_strId );
					}
				}
				break;
			case ASC_CONNECT_TYPE_WEB:
				{
					m_i8Auth_Service	= 1; 
					m_i32ConnectType	= ASC_CONNECT_TYPE_ADMIN; 

					g_pSessionManager->SendConfirmationMessage( this, L"WebTool", 0, 1 );
				}
				break;
			default: 
				{
					_KillUser();
					i32Rv = -1; 
					bSendMessage = TRUE;
					SendPacket.WriteData(&i32Rv,sizeof(INT32));
				}
				break; 
			}
		}
		break;
	case PROTOCOL_ASC_NEW_MD5_REQ:
		{
			if(m_ui32LastMD5 == 0)
			{
				pPacket->ReadData(&g_pControl->m_strKey1, NET_MD5_KEY_SIZE);
				g_pControl->m_strKey2[NET_MD5_KEY_SIZE] = '\0';
				m_ui32LastMD5 = 1;
			}
			else
			{
				pPacket->ReadData(&g_pControl->m_strKey2, NET_MD5_KEY_SIZE);
				g_pControl->m_strKey1[NET_MD5_KEY_SIZE] = '\0';

				m_ui32LastMD5 = 0;
			}
			//Send Game Server 
			g_pSessionManager->SendMD5Key();

			//Send Control Tool 
			bSendMessage = TRUE;
			INT32 i32Result = EVENT_ERROR_SUCCESS;
			SendPacket.SetProtocol( PROTOCOL_ASC_SET_MD5_KEY_ACK);
			SendPacket.WriteData(&i32Result,	sizeof(INT32));
			SendPacketMessage(&SendPacket);
		}
		break;
	case PROTOCOL_ASC_STATE_REQ: //상태 받기 (서버용)
		{
			if(m_i32ConnectType !=  ASC_CONNECT_TYPE_SERVER)
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_STATE_REQ Fail." );
				break; 
			}

			P_SERVER_STATE_REQ StatePacket;

#ifdef OLD_UPDATESERVER
			if (m_i32ServerType != ASC_SERVER_UPDATER)
			{
				pPacket->ReadData( &StatePacket, sizeof(P_SERVER_STATE_REQ) );
			}
			else
			{
				INT32	i32CpuQuery, i32HandleQuery, i32SessionCount, i32SuperUserIdx;
				pPacket->ReadData( &i32CpuQuery,		sizeof(INT32) );
				pPacket->ReadData( &i32HandleQuery,		sizeof(INT32) );
				pPacket->ReadData( &i32SessionCount,	sizeof(INT32) );
				pPacket->ReadData( &i32SuperUserIdx,	sizeof(INT32) );
				StatePacket._ui32Active			= SERVER_ACTIVE_STATE_CONNECT;
				StatePacket._i32Cpu				= i32CpuQuery;
				StatePacket._i32Handle			= i32HandleQuery;
				StatePacket._i32Arg0			= i32SessionCount;
				StatePacket._i32Arg1			= i32SuperUserIdx;
				StatePacket._i32Arg2			= 0;
				StatePacket._i32Arg3			= 0;
				StatePacket._i32Arg4			= 0;

				for( INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
				{
					StatePacket._ui32WarningInfo[i]		= 0;
				}
			}
#else
			pPacket->ReadData( &StatePacket, sizeof(P_SERVER_STATE_REQ) ); 
#endif

			SERVER_INFO_COMMON* pServerInfo;
			switch( m_i32ServerType )
			{
			case ASC_SERVER_GAME :
				{
					pPacket->ReadData(&g_pControl->m_aGameServerList[m_i32ServerIdx]._UserCount,	sizeof(INT32) * GAME_CHANNEL_COUNT);	
					pPacket->ReadData(&g_pControl->m_aui32RecvHeartBit2Count[m_i32ServerIdx],		sizeof(UINT32));	// HeartBit2 

					pServerInfo = (SERVER_INFO_COMMON*)&g_pControl->m_aGameServerList[m_i32ServerIdx];
				}
				break; 
			case ASC_SERVER_BATTLE	:
				{
					pServerInfo = (SERVER_INFO_COMMON*)&g_pControl->m_aBattleServerList[m_i32ServerIdx];
				}
				break; 
			case ASC_SERVER_CAST :
				{
					pServerInfo = (SERVER_INFO_COMMON*)&g_pControl->m_CastServerList[m_i32ServerIdx];
				}
				break;
			case ASC_SERVER_UPDATER:
				{
					pServerInfo = (SERVER_INFO_COMMON*)&g_pControl->m_aUpdateServerList[m_i32ServerIdx];
				}
				break;
			default:
				{
					pServerInfo = (SERVER_INFO_COMMON*)&g_pControl->m_aCommonServerList[m_i32ServerIdx];
				}
				break; 
			}

			//기존 소스 살려야함. 차후 삭제 
			pServerInfo->_i32IsActive		= StatePacket._ui32Active;
			pServerInfo->_i32Cpu			= StatePacket._i32Cpu;
			pServerInfo->_i32Handle			= StatePacket._i32Handle;
			pServerInfo->_i32Arg0			= StatePacket._i32Arg0;
			pServerInfo->_i32Arg1			= StatePacket._i32Arg1;
			pServerInfo->_i32Arg2			= StatePacket._i32Arg2;
			pServerInfo->_i32Arg3			= StatePacket._i32Arg3;
			pServerInfo->_i32Arg4			= StatePacket._i32Arg4;

			for( INT32 i = 0 ; i < WARNING_TYPE_COUNT; i++)
			{
				pServerInfo->_ui32WarningInfo[i]		= StatePacket._ui32WarningInfo[i];
			}

			g_pControl->UpdateServerState(); // Warning값이 갱신이 됫는지 확인하기위해 사용되는함수.
		}
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_ALL_START_REQ:
		{
			g_pSessionManager->AllStartServer();
		}
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_ALL_STOP_REQ:
		{
			if( FALSE == _CheckAdminAuth() )
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_ADMIN_SERVER_ALL_STOP_REQ Fail - Auth");
				break;
			}

			g_pSessionManager->AllStopServer();

			i3String::Format( wstrMessage, LOG_STRING_LEN, L"All Stop Server Start [ ID:%s, IP:%s ]", m_strId, m_wstrIpString );
			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );
		}
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_START_REQ:	//서버 시작  (관리자용)
		{
			if( FALSE == _CheckAdminAuth() )
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_ADMIN_SERVER_START_REQ Fail - Auth");
				break;
			}

			UINT8 ui8Type;
			INT32 i32Idx;
			pPacket->ReadData( &ui8Type,	sizeof(UINT8) );
			pPacket->ReadData( &i32Idx,		sizeof(INT32) );

			if( ASC_SERVER_COUNT < ui8Type )
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_ADMIN_SERVER_START_REQ Fail - Type");
				break;
			}

			BOOL Rv = g_pSessionManager->StartServer(this, ui8Type, i32Idx);

			if(Rv)
			{
				i3String::Format( wstrMessage, LOG_STRING_LEN, L"Start Server Success [ ID:%s, IP:%s, Server:%s, Number:%d ]", m_strId, m_wstrIpString, g_pControl->m_astrServiceName[ ui8Type ], i32Idx );
			}
			else 
			{
				i3String::Format( wstrMessage, LOG_STRING_LEN, L"Start Server Fail [ ID:%s, IP:%s, Server:%s, Number:%d ]", m_strId, m_wstrIpString, g_pControl->m_astrServiceName[ ui8Type ], i32Idx );
			}

			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );
		}
		break; 		
	case PROTOCOL_ASC_ADMIN_SERVER_STOP_REQ:	//서버 종료   (관리자용)
		{
			if(m_i32ConnectType !=  ASC_CONNECT_TYPE_ADMIN)
			{
				_KillUser();
				g_pLog->WriteLog( L"PROTOCOL_ASC_ADMIN_SERVER_STOP_REQ fail.");
				break; 
			}

			UINT8 ui8Type;
			INT32 i32Idx;
			pPacket->ReadData( &ui8Type,	sizeof(UINT8) );
			pPacket->ReadData( &i32Idx,		sizeof(INT32) );

			if( ASC_SERVER_COUNT < ui8Type )
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_ADMIN_SERVER_START_REQ Fail - Type");
				break;
			}

			BOOL Rv = g_pSessionManager->StopServer(this, ui8Type, i32Idx); 

			if(Rv)
			{
				i3String::Format( wstrMessage, LOG_STRING_LEN, L"Stop Server Success [ ID:%s, IP:%s, Server:%s, Number:%d ]", m_strId, m_wstrIpString, g_pControl->m_astrServiceName[ ui8Type ], i32Idx );
			}
			else 
			{
				i3String::Format( wstrMessage, LOG_STRING_LEN, L"Stop Server Fail [ ID:%s, IP:%s, Server:%s, Number:%d ]", m_strId, m_wstrIpString, g_pControl->m_astrServiceName[ ui8Type ], i32Idx );
			}

			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );
		}
		break;

	case PROTOCOL_ASC_ADMIN_KICK_TRANS_USER_REQ		: 
		{
			if( FALSE == _CheckAdminAuth() )
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_ADMIN_KICK_TRANS_USER_REQ Fail - Auth");
				break;
			}
			
			g_pSessionManager->SendTransUserKick();

			i3String::Format( wstrMessage, LOG_STRING_LEN, L"Trans User Kick Start [ ID:%s, IP:%s ]", m_strId, m_wstrIpString );
			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );
		}
		break;
	case PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_REQ:
		{
			if( FALSE == _CheckAdminAuth() )
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_ADMIN_SERVER_KICK_ALLUSER_REQ Fail - Auth");
				break;
			}

			g_pSessionManager->SendAllUserDisconnect();

			i3String::Format( wstrMessage, LOG_STRING_LEN, L"All User Kick Start [ ID:%s, IP:%s ]", m_strId, m_wstrIpString );
			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );
		}
		break; 
	case PROTOCOL_ASC_ADMIN_SERVER_KICK_ONEUSER_REQ:
		{
			if( FALSE == _CheckAdminAuth() )
			{
				_KillUser();
				g_pLog->WriteLog( L"[MAIN] PROTOCOL_ASC_ADMIN_SERVER_KICK_ONEUSER_REQ Fail - Auth");
				break;
			}

			INT32 i32Idx;
			pPacket->ReadData(&i32Idx,		sizeof(INT32));		//CPU 

			g_pSessionManager->SendOneUserDisconnect( i32Idx );

			i3String::Format( wstrMessage, LOG_STRING_LEN, L"One User Kick Start [ ID:%s, IP:%s, Number:%d ]", m_strId, m_wstrIpString, i32Idx );
			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );
		}
		break;
	case PROTOCOL_ASC_SET_MD5_KEY_REQ			:
		{
			if(m_i32ConnectType !=  ASC_CONNECT_TYPE_ADMIN)
			{
				_KillUser();
				g_pLog->WriteLog( L"PROTOCOL_ASC_SET_MD5_KEY_REQ Fail.");
				break; 
			}

			//SET
			char	m_TempKey[NET_MD5_KEY_SIZE+1]; 

			pPacket->ReadData(m_TempKey, NET_MD5_KEY_SIZE);
			m_TempKey[ NET_MD5_KEY_SIZE ] = '\0';

			if(i3String::Compare(m_TempKey, g_pControl->m_strKey1) != 0)
			{
				i3String::Copy(g_pControl->m_strKey1, m_TempKey, NET_MD5_KEY_SIZE+1);
				m_ui32LastMD5 = 1;
			}

			pPacket->ReadData(m_TempKey, NET_MD5_KEY_SIZE);
			m_TempKey[ NET_MD5_KEY_SIZE ] = '\0';
			if(i3String::Compare(m_TempKey, g_pControl->m_strKey2) != 0)
			{
				i3String::Copy(g_pControl->m_strKey2, m_TempKey, NET_MD5_KEY_SIZE+1);
				m_ui32LastMD5 = 0;
			}
			

			i3String::Format( wstrMessage, LOG_STRING_LEN, L"MD5 KEY UPDATE 1[%s]", g_pControl->m_strKey1);
			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );

			i3String::Format( wstrMessage, LOG_STRING_LEN, L"MD5 KEY UPDATE 2[%s]", g_pControl->m_strKey2);
			g_pSessionManager->SendControlAppMessage( wstrMessage );
			g_pLog->WriteLog( wstrMessage );

			//Send Game Server 
			g_pSessionManager->SendMD5Key(); 

			//Send Control Tool 
			bSendMessage = TRUE;
			INT32 nResult = EVENT_ERROR_SUCCESS;
			SendPacket.SetProtocol( PROTOCOL_ASC_SET_MD5_KEY_ACK);
			SendPacket.WriteData(&nResult,	sizeof(INT32));
		}
		break; 
	case PROTOCOL_ASC_GET_MD5_KEY_REQ			: 
		{
			if(m_i32ConnectType !=  ASC_CONNECT_TYPE_ADMIN)
			{
				_KillUser();
				g_pLog->WriteLog( L"PROTOCOL_ASC_SET_MD5_KEY_REQ Fail.");
				break;
			}

			//Send Control Tool 
			bSendMessage = TRUE;
			SendPacket.SetProtocol( PROTOCOL_ASC_GET_MD5_KEY_ACK);
			SendPacket.WriteData(g_pControl->m_strKey1,	NET_MD5_KEY_SIZE);
			SendPacket.WriteData(g_pControl->m_strKey2,	NET_MD5_KEY_SIZE);
		}
		break; 
	case PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_REQ	: //메시지 보내기 (관리툴에서 보낸다.)
		{
			if (m_i32ConnectType != ASC_CONNECT_TYPE_ADMIN)
			{
				_KillUser();
				g_pLog->WriteLog(L"PROTOCOL_ASC_ADMIN_SERVER_ANNOUNCE_REQ Fail.");
				break;
			}

			UINT32	ui32strLen;
			INT32	i32Type;
			pPacket->ReadData(&i32Type, sizeof(INT32));
			TTCHAR strNoticeMessage[LOG_STRING_LEN];
			pPacket->ReadData(&ui32strLen, sizeof(UINT32));
			if (ui32strLen > LOG_STRING_LEN - 1)return;
			pPacket->ReadData(strNoticeMessage, sizeof(TTCHAR) * ui32strLen);
			strNoticeMessage[ui32strLen] = 0;
			g_pSessionManager->SendNoticeMessage(i32Type, strNoticeMessage, ui32strLen);

			strNoticeMessage[400] = '\0';

			switch (i32Type)
			{
			case 1:
				i3String::Format(wstrMessage, LOG_STRING_LEN, L"Lobby Notice Send [ ID:%s, IP:%s, Text:%S ]", m_strId, m_wstrIpString, strNoticeMessage);
				break;
			case 2:
				i3String::Format(wstrMessage, LOG_STRING_LEN, L"Emergency Notice [ ID:%s, IP:%s, Text:%s ]", m_strId, m_wstrIpString, strNoticeMessage);
				break;
			default:
				i3String::Format(wstrMessage, LOG_STRING_LEN, L"Notice Error [ ID:%s, IP:%s, Text:%S ]", m_strId, m_wstrIpString, strNoticeMessage);
				break;
			}
			g_pSessionManager->SendControlAppMessage(wstrMessage);
			g_pLog->WriteLog(wstrMessage);
		}
		break;
	case PROTOCOL_ASC_WEB_GM_KICK_ACK :
		{
			INT32	i32WebSessionIdx;
			INT32	i32Rv;

			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i32Rv,				sizeof(INT32) );

			g_pSessionManager->RecvKickUserWebAck( i32WebSessionIdx, i32Rv );
		}
		break;
	case PROTOCOL_ASC_WEB_USER_INFO_LOCATION_ACK :
		{
			INT32	i32WebSessionIdx;
			INT32	i32Rv;
			INT8	i8ServerIdx = 0;
			INT8	i8ChannelNum = 0;
			INT32	i32RoomIdx = 0;

			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i32Rv,				sizeof(INT32) );
			pPacket->ReadData( &i8ServerIdx,		sizeof(INT8)  );
			pPacket->ReadData( &i8ChannelNum,		sizeof(INT8)  );
			pPacket->ReadData( &i32RoomIdx,			sizeof(INT32) );

			g_pSessionManager->RecvUserInfoLocationWebAck( i32WebSessionIdx, i32Rv, i8ServerIdx, i8ChannelNum, i32RoomIdx );
		}
		break;
	case PROTOCOL_ASC_WEB_CLAN_INFO_BATTLE_ACK :
		{
			INT32	i32WebSessionIdx;
			INT32	i32Rv;
			INT8	i8Count;

			pPacket->ReadData( &i32WebSessionIdx,	sizeof(INT32) );
			pPacket->ReadData( &i32Rv,				sizeof(INT32) );
			pPacket->ReadData( &i8Count,			sizeof(INT8)  );

			g_pSessionManager->RecvClanInfoBattleWebAck( i32WebSessionIdx, i32Rv, i8Count );
		}
		break;
	case PROTOCOL_ASC_GAMEGUARD_UPDATE_REQ:
		{
			// 속해있는 게임 서버에 게임 가드를 재로딩할 것을 요청한다.
		}
		break;
	case PROTOCOL_ASC_CHANGE_HACKCHECKRATIO_NTF: // Change Hack Check ratio ( ControlTool->ControSvr->DediSvr )
	{
		// Get Changed Hack Check Ratio 
		ChangedHackCheckRatio sHackCheckRatio;

		pPacket->ReadData(&sHackCheckRatio, sizeof(ChangedHackCheckRatio));

		const char* pBuffer = pPacket->GetPacketBuffer();
		INT32 i32Size = pPacket->GetPacketSize();

		switch (sHackCheckRatio.m_ui8ServerType)
		{
		case ASC_SERVER_GAME:
		{
			if (sHackCheckRatio.m_ui8HackCheckType < GS_HACK_TYPE_MAX)
			{
				g_pControl->m_pGameHackCheckRatioList[sHackCheckRatio.m_ui8ServerIdx].m_aui8HackCheckRatio[sHackCheckRatio.m_ui8HackCheckType] = sHackCheckRatio.m_ui8ChangedCheckRatio;

				// Send to Dedication server
				g_pSessionManager->SendToGameServer(sHackCheckRatio.m_ui8ServerIdx, pBuffer, i32Size);
			}
			else
			{
				return;
			}
		}
		break;
		case ASC_SERVER_BATTLE:
		{
			if (sHackCheckRatio.m_ui8HackCheckType < HACK_TYPE_MAX)
			{
				g_pControl->m_pDediHackCheckRatioList[sHackCheckRatio.m_ui8ServerIdx].m_aui8HackCheckRatio[sHackCheckRatio.m_ui8HackCheckType] = sHackCheckRatio.m_ui8ChangedCheckRatio;

				// Send to Dedication server
				g_pSessionManager->SendToDediServer(sHackCheckRatio.m_ui8ServerIdx, pBuffer, i32Size);
			}
			else
			{
				return;
			}
		}
		break;
		default:
			return;
		}

		// Send to All control tools 
		g_pSessionManager->SendToAllControlTools(pBuffer, i32Size);
	}
	break;
	case PROTOCOL_ASC_NOTALLOW_ROOMCREATION_NTF: // Not allow Room creation because of server shut down...(ControlTool->ControSvr->DediSvr)
		{
			const char* pBuffer	= pPacket->GetPacketBuffer(); 
			INT32 i32Size = pPacket->GetPacketSize();  
		
			// Send to Dedication Server
			g_pSessionManager->SendCastServer( 0, pBuffer, i32Size );

			// Send to all control tools 
			g_pSessionManager->SendToAllControlTools(pBuffer, i32Size);
		}
		break;

	case PROTOCOL_ASC_NORMAL_DISCONNECT_REQ:	// 서버 정상 종료
		{
			UINT8 ServerType = 0;
			INT32 i32destroyReason = 0;

			m_bIsNormalDisconnect = TRUE;

			pPacket->ReadData( &ServerType, sizeof(UINT8));
			pPacket->ReadData( &i32destroyReason, sizeof(INT32));

			if(ServerType > ASC_SERVER_COUNT) // ASC_SERVER_NONE(255)인 경우도 체크 됨
			{
				g_pLog->WriteLog( L"[PROTOCOL_ASC_NORMAL_DISCONNECT_REQ] Server Type : %d is Wrong", ServerType );
			}

			if(i32destroyReason != SERVER_ERROR_NO)
			{
				g_pSessionManager->SendServerError(ServerType, i32destroyReason);
				g_pLog->WriteLog( L"[PROTOCOL_ASC_NORMAL_DISCONNECT_REQ] Server Type : %d, Error Reason : %d", ServerType, i32destroyReason );
			}

#ifdef I3_DEBUG
			g_pLog->WriteLog( L"[PROTOCOL_ASC_NORMAL_DISCONNECT_REQ] Server Type : %d", m_i32ServerType );
#endif
		}
		break;

	case PROTOCOL_ASC_SERVER_LOADING_RESULT:	// 서버 로딩 완료
		{
			i3NetworkPacket		SendControlToolPacket;
			INT32				i32LoadingResult;
			UINT8				ui8ServerState;

			pPacket->ReadData( &i32LoadingResult, sizeof(INT32) );

			if ( EVENT_ERROR_SUCCESS == i32LoadingResult )	ui8ServerState = ASC_ICON_CONNECTION;
			else											ui8ServerState = ASC_ICON_WARNING;

			SERVER_INFO_COMMON* pServerInfo = g_pControl->GetServerInfo( (ASC_SERVER_TYPE)m_i32ServerType, m_i32ServerIdx );
			// 해당 서버 On 상태로 변경
			pServerInfo->_i32IsActive = ui8ServerState;

			if ( i32LoadingResult != EVENT_ERROR_SUCCESS)
			{
				// 컨트롤툴에 서버인덱스랑 결과 보냄
				g_pSessionManager->SendServerError(m_i32ServerIdx, NULL);

				g_pLog->WriteLog( L"[PROTOCOL_ASC_SERVER_LOADING_RESULT] Server Loading Failed (Server Type : %d, Error Code : %d", m_i32ServerType, i32LoadingResult );
			}
		}
		break;

	case PROTOCOL_SS_GACHA_NOTICE_REQ:
		{
			if( !g_pControl->IsContentsEnabled(CONTENTS_GACHA) )	break;

			CGachaShop* pGachaShop = g_pControl->GetGachaShop();

			UINT8 ui8NoticeCount = (UINT8)pGachaShop->GetNoticeCount();

			bSendMessage = TRUE;
			SendPacket.SetProtocol( PROTOCOL_SS_GACHA_NOTICE_ACK );
			SendPacket.WriteData(&ui8NoticeCount, sizeof(UINT8));

			for( INT32 i = 0; i < ui8NoticeCount; ++i )
			{
				PACKET_GACHA_NOTICE stNotice;
				pGachaShop->MakePacketGachaNotice(i, &stNotice); 
				SendPacket.WriteData(&stNotice, sizeof(PACKET_GACHA_NOTICE)); 
			}
		}
		break;

	case PROTOCOL_SS_GACHA_ITEM_INFO_REQ:
		{
			if( !g_pControl->IsContentsEnabled(CONTENTS_GACHA) )	break;

			CGachaShop* pGachaShop = g_pControl->GetGachaShop();
			
			GACHA_GROUP eGachaGroup;
			pPacket->ReadData( &eGachaGroup, sizeof(GACHA_GROUP) );

			if( eGachaGroup <= GACHA_GROUP_NONE ) break;
			if( eGachaGroup >= GACHA_GROUP_COUNT ) break;
			
			SS_GACHA_GROUP_INFO stGachaGroup;
			pGachaShop->MakePacketGachaInfo(eGachaGroup, &stGachaGroup);

			bool bActive = true;
			if( GACHA_GROUP_NONE == stGachaGroup.m_eGachaGroup )
			{
				bActive = false;
			}

			bSendMessage = TRUE;
			SendPacket.SetProtocol(PROTOCOL_SS_GACHA_ITEM_INFO_ACK);
			SendPacket.WriteData(&eGachaGroup,								sizeof(GACHA_GROUP));
			SendPacket.WriteData(&bActive,									sizeof(bool));  
			if( bActive )
			{
				//SendPacket.WriteData(&stGachaGroup.m_ui32PaymentGoodsID,	sizeof(UINT32));
				SendPacket.WriteData(&stGachaGroup.m_ui32LuckyGoodsID,		sizeof(UINT32));
				SendPacket.WriteData(&stGachaGroup.m_i32LuckyProbability,	sizeof(INT32));
				//SendPacket.WriteData(&stGachaGroup.m_i32GloryCount,			sizeof(INT32));

				SendPacket.WriteData(&stGachaGroup.m_i32WinItemCount,		sizeof(INT32));
				SendPacket.WriteData(stGachaGroup.m_arWinItems,				(UINT16)( sizeof(SS_GACHA_ITEM_INFO)*stGachaGroup.m_i32WinItemCount ));

				SendPacket.WriteData(&stGachaGroup.m_i32RandomItemCnt,		sizeof(INT32));
				SendPacket.WriteData(stGachaGroup.m_arRandomGoodsIDs,		(UINT16)( sizeof(UINT32)*stGachaGroup.m_i32RandomItemCnt ));
			}
		}
		break;

	default: 
		{
			_KillUser();
			g_pLog->WriteLog( L"[%s] Error Unknown Main Packet ID = %d", m_wstrIpString, ProtocolID );
		}
		break;
	}

	if( bSendMessage )
	{
		SendPacketMessage( &SendPacket );
	}

	m_ui32LastRecvST = i3ThreadTimer::GetServerTime();

	return;

}
 