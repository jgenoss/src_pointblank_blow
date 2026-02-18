#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleTrans.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingBase( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Rv;
	INT32		i32Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_BASE_GAME_SERVER_STATE_REQ	:
		{
			// CHECKVALID 없음
			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_BASE_GAME_SERVER_STATE_ACK );
			pSendPacket->WriteData( &g_pContextMain->m_ui32GameServerCount, sizeof(UINT32)); 
			pSendPacket->WriteData( g_pContextMain->m_aGameServerInfo,	(UINT16)(sizeof(GAME_SERVER_INFO) * g_pContextMain->m_ui32GameServerCount) ); 
		}
		break;
	case PROTOCOL_BASE_LOGIN_REQ				:					//0823
		{
			CHECKVALID_LOGIN;
			m_bLogin = TRUE;
			
			if( 0 != m_ui32LogInST )
			{
				bisKilled		= TRUE;		//보내는거 없이 여기서 바로 .
				m_ui32KillReason	= KILL_REASON_LOGTIMEOUT;
				break;
			}

			SC_VERSION	ClientVersion;
			INT8	ui8LocalCode;
			UINT64	ui64EncryptKey	= 0;
			UINT8	ui8IDLenght = 0;
			UINT8	ui8PWLenght = 0;
			char	strID[ NET_ID_SIZE ];
			char	strPasswd[ NET_PW_SIZE ];
			char	strMD5[NET_MD5_KEY_SIZE+1]; 
			bool	bCheckedID = true;
		

			pPacket->ReadData(&ClientVersion,			sizeof(SC_VERSION));
			pPacket->ReadData(&ui8LocalCode,			sizeof( UINT8 ) );
			pPacket->ReadData(&ui8IDLenght,				sizeof( UINT8 ) );	LENGTH_CHECK( ui8IDLenght, NET_ID_SIZE );
			pPacket->ReadData(&ui8PWLenght,				sizeof( UINT8 ) );	LENGTH_CHECK( ui8PWLenght, NET_PW_SIZE );
			pPacket->ReadData(strID,					ui8IDLenght );
			pPacket->ReadData(strPasswd,				ui8PWLenght );
			pPacket->ReadData(&m_ui64MacAddress,		sizeof(INT64) );
			pPacket->ReadData(&m_bIsRealIp,				sizeof(bool) );
			pPacket->ReadData(&m_UdpNetInfo.m_nLocalIP,	sizeof(UINT32) );
			pPacket->ReadData(&ui64EncryptKey,			sizeof(UINT64) );
			pPacket->ReadData(strMD5,					NET_MD5_KEY_SIZE ); 

#if defined (USE_INDONESIA_PCBANG)
			// 인도네시아에서 사용하는 GWarnet 레지스트리 값 + 활성화 맥 어드레스 값 ( PC방 )
			INT64 i64ActiveMacAddress;
			if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )
			{
				pPacket->ReadData(m_strNesiaPCbangKey,		MAX_INDONESIA_PCBANG_KEY		); 
				pPacket->ReadData(&m_ui8NesiaPCbangHWInfo,	sizeof(m_ui8NesiaPCbangHWInfo)	); 
				pPacket->ReadData(&m_dwNesiaPCbangHWKey,	sizeof(DWORD)					); 
				pPacket->ReadData(&i64ActiveMacAddress,		sizeof(INT64)					);
			}
#endif
			strMD5[NET_MD5_KEY_SIZE]	= 0;
			strID[NET_ID_SIZE - 1]		= '\0';
			strPasswd[NET_PW_SIZE - 1]	= '\0';

#ifdef USE_LOG_KILLREASON
			char strTemp[ MAX_PATH ];
			i3String::Format( strTemp, MAX_PATH, "[RECV]PROTOCOL_BASE_LOGIN_REQ(%d)[UID:%I64d, ver(%d.%d.%d.%d)]", ProtocolID, m_i64UID, VER_GAME, VER_BUILD, VER_SC_PROTOCOL, VER_SS_PROTOCOL );
			g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
#endif

			// 접속한 클라이언트의 버전이 서버와 맞지 않을 경우
			if ( (ClientVersion._ui8VerGame != g_pContextMain->m_GameVersion._ui8VerGame) ||
				(ClientVersion._ui16VerBuild != g_pContextMain->m_GameVersion._ui16VerBuild) ||
				(ClientVersion._ui16VerSC != g_pContextMain->m_ClientVersion._ui16VerSC) )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN_ERROR, ProtocolID, KILL_REASON_VER_ERROR );
				break;
			}

#if !defined( BUILD_MATCH_VERSION )	// 대회빌드용 빌드 옵션 추가	2011.08.30 안국정
#if !defined( BUILD_DIST_VERSION )
			m_ui8LocaleCode = ui8LocalCode;
#endif
#endif
			// 각 나라별 로그인 아이디 필터 적용
			switch( m_ui8LocaleCode )
			{
			case LOCALE_CODE_INDONESIA:
				{
					char* pstrID = m_strId;
					for( INT32 i = 0 ; i < ui8IDLenght ; i++ )
					{
						if( ( 'a' <= strID[i] && 'z' >= strID[i] ) ||
							( 'A' <= strID[i] && 'Z' >= strID[i] ) ||
							( '0' <= strID[i] && '9' >= strID[i] ) ||
							( '_' == strID[i] ) ||
							( '-' == strID[i] ) ||
							( '\0' == strID[i] ) )
						{
							*pstrID = strID[i];
							pstrID++;
						}
						else
						{
							_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_INVALID_ID, ProtocolID, KILL_REASON_HACK_PACKET );
							bCheckedID = false;
							break;
						}
					}
					*pstrID = '\0';
				}
				break;
			/* 협의 후 처리
			case LOCALE_CODE_TURKEY:
				{
					char* pstrID = m_strId;
					for( INT32 i = 0 ; i < ui8IDLenght ; i++ )
					{
						if( '+' == strID[i] )
						{
							LogIn_MAU( m_WorkThreadIdx, EVENT_ERROR_EVENT_LOG_IN_ID_FAIL, NULL, NULL );
							break;
						}
					}

					i3String::Copy( m_strId, strID, NET_ID_SIZE );
				}*/
			//case LOCALE_CODE_NORTHAMERICA:
			//case LOCALE_CODE_PHILIPPINES:
			case LOCALE_CODE_BRAZIL:
				{
					char* pstrID = m_strId;
					for( INT32 i = 0 ; i < ui8IDLenght ; i++ )
					{
// 						if( ( 'a' <= strID[i] && 'z' >= strID[i] ) ||
// 							( 'A' <= strID[i] && 'Z' >= strID[i] ) ||
// 							( '0' <= strID[i] && '9' >= strID[i] ) )
						if( strID[i] != '\\' )
						{
							*pstrID = strID[i];
							pstrID++;
						}
					}
					*pstrID = '\0';
				}
				break;
			default:
				{
					i3String::Copy( m_strId, strID, NET_ID_SIZE );
				}
				break;
			}

			//ID 체크에서 실패하였을 경우 처리를 종료합니다.
			if( false == bCheckedID )	break;

#if defined( BUILD_DIST_VERSION )
			if( g_pContextMain->m_bMacAddressCheck ) 
			{
				// Dist 버전에서만 사용됩니다.
				bool bLoginAppr = true;
				switch( m_ui8LocaleCode )
				{
				case LOCALE_CODE_INDONESIA:
					// MD5 Key 가 0으로 세팅이 되어있다면 Control 서버 Config 에서 Not Use 로 설정이 되어있는 것, QA서버이므로 아래 검사를 넘억바니다. - 서동권
					if( i3String::Compare( "0", g_pContextMain->m_astrMD5Key[0] ) == 0) break;

					if( !g_pContextMain->CheckMD5Key( strMD5 ) ) 
					{
						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, PROTOCOL_BASE_LOGIN_REQ, KILL_REASON_MD5_ERROR );
						bLoginAppr = false;
						break;
					}
					//break;		// 밑에 값도 체크해야합니다.
				case LOCALE_CODE_TURKEY			:
				case LOCALE_CODE_CHINA			:
				case LOCALE_CODE_TAIWAN			:
				case LOCALE_CODE_JAPAN			:
				case LOCALE_CODE_BRAZIL			:
				case LOCALE_CODE_NORTHAMERICA	:
				case LOCALE_CODE_ITALY			:
				case LOCALE_CODE_ENGLISH		:
				case LOCALE_CODE_MIDDLE_EAST	:
				case LOCALE_CODE_PHILIPPINES	:
					{
						UINT32	ui32Key1, ui32Key2;

						DataDecript( ui64EncryptKey, &ui32Key1, &ui32Key2 );

						INT64	i64MacAddress = (INT64)ui32Key1 << 32 | ui32Key2;

						if( m_ui64MacAddress != i64MacAddress )
						{
							// 불법 프로그램으로 간주하고 실패 Ack 를 보냅니다.
							bSendMessage = TRUE;
							i32Rv = EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME | 0x2;	// 불법프로그램으로 끊겠다는 메시지입니다.
							pSendPacket->SetProtocol( PROTOCOL_BASE_LOGIN_ACK );
							pSendPacket->WriteData( &i32Rv,	sizeof(INT32));

							char szTemp[ MAX_PATH ] = { '\0', };	
							i3String::Format( szTemp, MAX_PATH, "PROTOCOL_BASE_LOGIN_ACK");
							_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, ProtocolID, KILL_REASON_HACK_PACKET, szTemp );

							i3String::Format( szTemp, MAX_PATH, "Login Key Error ID: %s, %I64u", m_strId, ui64EncryptKey );
							g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, szTemp );

							bLoginAppr = false;
						}

#if defined (USE_INDONESIA_PCBANG)
						// 인도네시아의 경우 활성화 된 맥어드레스 값을 피시방에서 사용하여야 함.
						// 그런데 런처에서는 활성화 된 맥어드레스 값이 아닌 무조건 첫번째 맥어드레스를 보내와서
						// 검사에서는 첫번째 맥어드레스를 사용하고 그다음은 활성화 값으로 바꿈.
						if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )	m_ui64MacAddress = i64ActiveMacAddress;
#endif
					}
					break;
				}

				if( false == bLoginAppr ) break;
			}
#endif

			UINT32 ui32Value	= GET_LOGIN_KEY_VALUE( m_ui32LoginKey );
			ui32Value++;
			m_ui32LoginKey		= SET_LOGIN_KEY( m_WorkThreadIdx, ui32Value );

			if( g_pModuleTrans->AuthSendLogIn( m_WorkThreadIdx, this, strPasswd ))
			{
				m_ui32LogInST = g_pContextMain->GetServerTime();
			}
			else
			{
				LogIn_MAU( m_WorkThreadIdx, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1, NULL, NULL, NULL );				
			}
		}
		break;
	case PROTOCOL_BASE_LOGIN_WEBKEY_REQ		:						//0823
		{
			CHECKVALID_LOGIN;
			m_bLogin = TRUE;

			if( 0 != m_ui32LogInST )
			{
				bisKilled		= TRUE;		//보내는거 없이 여기서 바로 .
				m_ui32KillReason	= KILL_REASON_LOGTIMEOUT;
				break;
			}

			SC_VERSION	ClientVersion;

			pPacket->ReadData(&ClientVersion,			sizeof(SC_VERSION));
			pPacket->ReadData(m_strWebKey,					NET_SESSION_KEY_SIZE); 
			pPacket->ReadData(&m_ui64MacAddress,			sizeof(INT64) ); 
			pPacket->ReadData(&m_bIsRealIp,				sizeof(bool) );
			pPacket->ReadData(&m_UdpNetInfo.m_nLocalIP,	sizeof(UINT32) );

			m_strWebKey[NET_SESSION_KEY_SIZE]	= '\0';
			m_strId[NET_ID_SIZE - 1]			= '\0';

			// 접속한 클라이언트의 버전이 서버와 맞지 않을 경우
			if ( (ClientVersion._ui8VerGame != g_pContextMain->m_GameVersion._ui8VerGame) ||
				(ClientVersion._ui16VerBuild != g_pContextMain->m_GameVersion._ui16VerBuild) ||
				(ClientVersion._ui16VerSC != g_pContextMain->m_ClientVersion._ui16VerSC) )
			{
				char strTemp[ MAX_PATH ];
				i3String::Format( strTemp, MAX_PATH, "[Wrong Version][ver(%d.%d.%d.%d)]", ClientVersion._ui8VerGame, ClientVersion._ui16VerBuild, ClientVersion._ui16VerSC, VER_SS_PROTOCOL );
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, ProtocolID, KILL_REASON_VER_ERROR, strTemp );
				break;
			}


			UINT32 ui32Value	= GET_LOGIN_KEY_VALUE( m_ui32LoginKey );
			ui32Value++;
			m_ui32LoginKey		= SET_LOGIN_KEY( m_WorkThreadIdx, ui32Value );

			if( g_pModuleTrans->AuthSendLogInWEB( m_WorkThreadIdx, this ) )
			{
				m_ui32LogInST = g_pContextMain->GetServerTime();
			}
			else
			{
				LogIn_MAU( m_WorkThreadIdx, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1, NULL, NULL, NULL );				
			}
		}
		break;
	case PROTOCOL_BASE_LOGIN_WEBKEY_RUSSIA_REQ	:
		{
			CHECKVALID_LOGIN;
			m_bLogin = TRUE;

			if( 0 != m_ui32LogInST )
			{
				bisKilled		= TRUE;		//보내는거 없이 여기서 바로 .
				m_ui32KillReason	= KILL_REASON_LOGTIMEOUT;
				break;
			}
			
			char	strPass[ MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN ];
			SC_VERSION	ClientVersion;
			
			pPacket->ReadData(&ClientVersion,				sizeof(SC_VERSION));
			pPacket->ReadData( m_strId,						MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN );
			pPacket->ReadData( strPass,						MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN );			
			pPacket->ReadData( &m_ui64MacAddress,			sizeof(INT64) );
			pPacket->ReadData( &m_bIsRealIp,				sizeof(bool) );
			pPacket->ReadData( &m_UdpNetInfo.m_nLocalIP,	sizeof(UINT32) );

			m_strId[ MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN-1 ] = '\0';
			strPass[ MAX_ID_PASS_FOR_RUSSIA_WEB_LOGIN-1 ] = '\0';

			// 접속한 클라이언트의 버전이 서버와 맞지 않을 경우
			if ( (ClientVersion._ui8VerGame != g_pContextMain->m_GameVersion._ui8VerGame) ||
				(ClientVersion._ui16VerBuild != g_pContextMain->m_GameVersion._ui16VerBuild) ||
				(ClientVersion._ui16VerSC != g_pContextMain->m_ClientVersion._ui16VerSC) )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, ProtocolID, KILL_REASON_VER_ERROR );
				break;
			}

			UINT32 ui32Value	= GET_LOGIN_KEY_VALUE( m_ui32LoginKey );
			ui32Value++;
			m_ui32LoginKey		= SET_LOGIN_KEY( m_WorkThreadIdx, ui32Value );

			if( g_pModuleTrans->AuthSendLogInRussia( m_WorkThreadIdx, this, strPass ))
			{
				m_ui32LogInST = g_pContextMain->GetServerTime();
			}
			else
			{
				LogIn_MAU( m_WorkThreadIdx, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1, NULL, NULL, NULL );				
			}
		}
		break;
	case PROTOCOL_BASE_LOGIN_THAI_REQ	:
		{
			CHECKVALID_LOGIN;
			m_bLogin = TRUE;

			if( 0 != m_ui32LogInST )
			{
				bisKilled		= TRUE;		//보내는거 없이 여기서 바로 .
				m_ui32KillReason	= KILL_REASON_LOGTIMEOUT;
				break;
			}
			INT16	i16TokenSize = 0;
			char	Token[NET_ENCRYPTED_TOKEN_SIZE+1] = {0,};
			SC_VERSION	ClientVersion;

			pPacket->ReadData(&ClientVersion,			sizeof(SC_VERSION));
			pPacket->ReadData(&i16TokenSize,			sizeof(INT16) );
			if( i16TokenSize > NET_ENCRYPTED_TOKEN_SIZE || i16TokenSize < 0 )
			{
				// 정상적인 패킷인데 여기서 걸려서 종료된다면 토큰을 받는 버퍼 크기 조정 필요
				bisKilled		= TRUE;
				m_ui32KillReason	= KILL_REASON_LOGTIMEOUT;
				break;
			}

			pPacket->ReadData(Token,					sizeof(char)*i16TokenSize ); 
			pPacket->ReadData(&m_ui64MacAddress,		sizeof(INT64) ); 
			pPacket->ReadData(&m_bIsRealIp,				sizeof(bool) );
			pPacket->ReadData(&m_UdpNetInfo.m_nLocalIP,	sizeof(UINT32) );

			Token[NET_ENCRYPTED_TOKEN_SIZE]		= 0;
			m_strWebKey[NET_SESSION_KEY_SIZE]		= '\0';
			m_strId[NET_ID_SIZE - 1]			= '\0';

			if ( (ClientVersion._ui8VerGame != g_pContextMain->m_GameVersion._ui8VerGame) ||
				(ClientVersion._ui16VerBuild != g_pContextMain->m_GameVersion._ui16VerBuild) ||
				(ClientVersion._ui16VerSC != g_pContextMain->m_ClientVersion._ui16VerSC) )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, ProtocolID, KILL_REASON_VER_ERROR );
				break;
			}

			UINT32 ui32Value	= GET_LOGIN_KEY_VALUE( m_ui32LoginKey );
			ui32Value++;
			m_ui32LoginKey		= SET_LOGIN_KEY( m_WorkThreadIdx, ui32Value );

			if( g_pModuleTrans->AuthSendLogInThai( m_WorkThreadIdx, this, i16TokenSize, Token ) )
			{
				m_ui32LogInST = g_pContextMain->GetServerTime();
			}
			else
			{
				LogIn_MAU( m_WorkThreadIdx, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1, NULL, NULL, NULL );				
			}
		}
		break;

		// 준영 추가
	case PROTOCOL_BASE_LOGOUT_REQ	:
		{
			// 로그 아웃 처리
			m_bCheckLogout = TRUE;
			
			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_BASE_LOGOUT_ACK );
		}
		break;
	case PROTOCOL_BASE_GET_MYINFO_REQ		:							// 자신의 정보 받기 -- 로그인 후 한번만 받습니다.
		{
			CHECKVALID_DBINDEX;

			SetPosition( GAME_TASK_CHANNEL_LIST );

			if( g_pModuleTrans->AuthSendGetMyInfo( m_WorkThreadIdx, this ) )
			{
				m_ui32ATInfo = g_pContextMain->GetServerTime();
			}
			else
			{
				bSendMessage = TRUE;

				i32Rv = EVENT_ERROR_EVENT_TRANS;
				pSendPacket->SetProtocol( PROTOCOL_BASE_GET_MYINFO_ACK );
				pSendPacket->WriteData(&i32Rv,		sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_BASE_GET_OPTION_REQ		:							// 게임 옵션 받기 -- 로그인 후 한번만 받습니다.
		{
			CHECKVALID_DBINDEX;

			TYPE_RING_IN RingIn;
			RingIn._i64UID			= m_i64UID;
			RingIn._i32SessionIdx	= m_SessionIdx;
			RingIn._ui8Type			= RING_TYPE_OPTION_GET;

			i32Rv = g_pDataBase->RingInputDB( m_WorkThreadIdx, &RingIn );
			if( RINGBUFFER_SUCCESS != i32Rv )
			{
				bSendMessage = TRUE;

				pSendPacket->SetProtocol( PROTOCOL_BASE_GET_OPTION_ACK );
				pSendPacket->WriteData(&i32Rv,		sizeof(INT32) );
			}
		}
		break;
	case PROTOCOL_BASE_ENTER_PASS_REQ		:
		{
			UINT8	ui8Size;
			char	strPass[ NET_SERVER_PW ];
			pPacket->ReadData(&ui8Size,					sizeof(UINT8) );			LENGTH_CHECK( ui8Size, NET_SERVER_PW );
			pPacket->ReadData(strPass,					ui8Size );
			strPass[ NET_SERVER_PW -1 ] = '\0';

			i32Rv = EVENT_ERROR_SUCCESS;
			if( 0 != i3String::Compare( g_pContextMain->m_strEnterPass, strPass ) )
			{
				i32Rv = EVENT_ERROR_FAIL;
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, ProtocolID);
			}

			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_BASE_ENTER_PASS_ACK );
			pSendPacket->WriteData(&i32Rv,		sizeof(INT32) );
		}
		break;
	case PROTOCOL_BASE_USER_ENTER_REQ		:						//0823
		{
			//Load
			if( GAME_TASK_NONE <  GetPosition() ) 
			{
				bisKilled		= TRUE;
				m_ui32KillReason	= KILL_REASON_HACK_PACKET;
				break;
			}

			// Trans 서버로부터 받은 PROTOCOL_BASE_USER_ENTER_ACK 값이 성공일 경우에만 포지션 세팅하도록 수정.
			//SetPosition( GAME_TASK_CHANNEL_LIST );

			UINT8	ui8IDLenght;
			pPacket->ReadData(&ui8IDLenght,				sizeof(UINT8) );			LENGTH_CHECK( ui8IDLenght, NET_ID_SIZE );
			pPacket->ReadData(m_strId,					ui8IDLenght	);
			pPacket->ReadData(&m_i64UID,				sizeof(TUID) );
			pPacket->ReadData(&m_bIsRealIp,				sizeof(bool) );
			pPacket->ReadData(&m_UdpNetInfo.m_nLocalIP,	sizeof(UINT32) );
			m_strId[NET_ID_SIZE - 1]	= '\0';

			// 완료가 되었다면 서버 이동한다는 사실을 중계서버에 알린다.
			if( FALSE == g_pModuleTrans->AuthSendServerEnter(	m_WorkThreadIdx, this ) )
			{
				i32Temp = EVENT_ERROR_FAIL;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_BASE_USER_ENTER_ACK );
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
			}
			I3TRACE( "ENTER USER REQ\n" );
		}
		break; 
	case PROTOCOL_BASE_USER_LEAVE_REQ		:						//0823
		{
			CHECKVALID_DBINDEX;

			if( !m_bMyinfo ) // 서버 이동 전에 유저 정보를 안받았으면 이동이 안되도록 합니다.
			{
				i32Temp = EVENT_ERROR_FAIL;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_BASE_USER_LEAVE_ACK );
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
				break;
			}

			m_bCheckLogout = TRUE;	// 서버 이동이므로.

			//서버를 이동한다는 메시지를 각 상위 서버에 보냄  
			if( FALSE == g_pModuleTrans->AuthSendServerLeave(	m_WorkThreadIdx, this ) )
			{
				i32Temp = EVENT_ERROR_FAIL;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_BASE_USER_LEAVE_ACK );
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
				
				char strTemp[MAX_PATH];
				i3String::Format( strTemp, MAX_PATH, "#### PROTOCOL_BASE_USER_LEAVE_REQ ChangeDBFlage :%d", m_ui8ChangeDBFlage );
				g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
			}
		}
		break;

	case PROTOCOL_BASE_GET_CHANNELLIST_REQ	:							//채널 정보 보내기			0306 0823
		{
			CHECKVALID_DBINDEX;
			g_pUserManager->SendListInfoPaket(this);
		}
		break; 	
	case PROTOCOL_BASE_SELECT_CHANNEL_REQ	:							//채널 선택하기 			0306 0823
		{
			CHECKVALID_MYINFO; 

			if( m_i32ChannelNum != SERVER_VALUE_UNKNOWN)
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_SAME_REQ, ProtocolID ); 
				return FALSE;
			}

			pPacket->ReadData(&UTemp, sizeof(UINT32));
			i32Temp = _SelectChannel( UTemp );

			//SendMessage 
			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_BASE_SELECT_CHANNEL_ACK ); 
			pSendPacket->WriteData(&i32Temp, sizeof(INT32));

			if( EV_SUCCESSED( i32Temp ) )
			{
				UINT16 strLen = (UINT16)i3String::Length( g_pContextMain->m_strNormalNotice ); 
				pSendPacket->WriteData(&strLen, sizeof(UINT16));
				if(strLen >= NOTICE_STRING_MAX_COUNT) strLen = NOTICE_STRING_MAX_COUNT - 2;							//에러 입니다. 방어 코드 
				pSendPacket->WriteData( g_pContextMain->m_strNormalNotice, (strLen + 1));				

				m_i32ChannelNum		= i32Temp; 
				m_i32LogChannelNum	= ((g_pContextMain->m_i32ServerIdx*GAME_CHANNEL_COUNT) + (m_i32ChannelNum+1));
			}
		}
		break;
	case PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ	:			//080303 받는 패킷과 수정해서 넣어야 함..에러 일때 에러도 처리해야함
		{
			TUID UID;
			pPacket->ReadData(&UID, sizeof(TUID));

			if( FALSE == g_pModuleTrans->AuthSendUserRecord(m_WorkThreadIdx, this, UID ) )
			{
				USER_INFO_RECORD UserRecord;
				i3mem::FillZero( &UserRecord, sizeof( USER_INFO_RECORD));

				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_BASE_GET_RECORD_INFO_DB_ACK );
				pSendPacket->WriteData(&UserRecord, sizeof(USER_INFO_RECORD));
			}
		}
		break; 
	case PROTOCOL_BASE_GET_RECORD_INFO_SESSION_REQ	:		//레코드 정보 요청하기
		{
			CHECKVALID_DBINDEX;
			USER_INFO_RECORD * pRecord;
			pPacket->ReadData(&UTemp, sizeof(UINT32));
			pRecord = g_pUserManager->OnGetRecord(UTemp);
	
			if(pRecord != NULL)
			{
				bSendMessage = TRUE;
				i32Temp = EVENT_ERROR_SUCCESS; 
				pSendPacket->SetProtocol( PROTOCOL_BASE_GET_RECORD_INFO_SESSION_ACK );
				pSendPacket->WriteData(pRecord, sizeof(USER_INFO_RECORD) );
			}
		}		
		break;
	case PROTOCOL_BASE_RANK_UP_REQ:		// 사용안함
		{
		}
	case PROTOCOL_BASE_CHATTING_REQ			:	//채팅							확인 0331		싸이즈 조정할수있음 INT8 사이즈로.
		{			
			CHECKVALID_CHANNEL; 

			char pStr[MAX_CHATTING_COUNT]; 
			UINT16 ui16Type; 
			UINT16 ui16ChatSize;
			pPacket->ReadData( &ui16Type,		sizeof(UINT16));
			pPacket->ReadData( &ui16ChatSize,	sizeof(UINT16));
			if(ui16ChatSize > MAX_CHATTING_COUNT - 1)return FALSE;
			pPacket->ReadData( pStr,	ui16ChatSize );	
			pStr[MAX_CHATTING_COUNT - 1] = 0; 

			if( m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
			{//방 
				if (_CheckDuplicateChat(pStr, ZLOG_CHATTING_TYPE_ROOM, PROTOCOL_BASE_CHATTING_ACK) == CHAT_LIMIT_NORMAL)
				{
					m_pRoom->Chatting_U( this->m_WorkThreadIdx, this, pStr, ui16Type );
					break;
				}
			}
			else
			{//대기실
				if (_CheckDuplicateChat(pStr, ZLOG_CHATTING_TYPE_LOBBY, PROTOCOL_BASE_CHATTING_ACK) == CHAT_LIMIT_NORMAL)
				{
					g_pUserManager->OnSendLobbyChatting(this, pStr, ui16ChatSize);
					break;
				}
			}
		}
		break; 
	case PROTOCOL_BASE_OPTION_SAVE_REQ					: 
		{
			CHECKVALID_DBINDEX;

			TYPE_RING_IN RingIn;
			RingIn._i64UID			= m_i64UID;
			RingIn._i32SessionIdx	= m_SessionIdx;
			RingIn._ui8Type			= RING_TYPE_OPTION_SET;

			pPacket->ReadData( &RingIn._ui32ChangeFlag,			sizeof(UINT32) );

			if( FALSE == ( 0x10000000 & RingIn._ui32ChangeFlag ) )		// 0x10000000 일때는 Default 로 변경
			{			
				if( 0x00000001 & RingIn._ui32ChangeFlag )
				{	//Basic
					pPacket->ReadData(&RingIn._NetEnvSet,		sizeof(NET_ENV_SET));
				}
				if( 0x00000002 & RingIn._ui32ChangeFlag )
				{	//Key
					pPacket->ReadData( RingIn._aKeyMap,			sizeof(RingIn._aKeyMap) );				
				}
				if( 0x00000004 & RingIn._ui32ChangeFlag )
				{	//Macro
					UINT8 ui8MacroSize;
					for(INT32 i = 0; i < MACRO_MAX_COUNT; i++)
					{
						pPacket->ReadData( &ui8MacroSize,		sizeof(UINT8) );		LENGTH_CHECK( ui8MacroSize, MACRO_MAX_LENGHT );
						pPacket->ReadData( RingIn._astrMacro[i],	ui8MacroSize); 
					}
				}
			}

			i32Rv = g_pDataBase->RingInputDB( m_WorkThreadIdx, &RingIn );
			if( RINGBUFFER_SUCCESS != i32Rv )
			{
				bSendMessage = TRUE;

				pSendPacket->SetProtocol( PROTOCOL_BASE_GET_OPTION_ACK );
				pSendPacket->WriteData(&i32Rv,		sizeof(INT32) );
			}
		}
		break; 
	case PROTOCOL_BASE_GAMEGUARD_ACK					:		//게임 가드 용 080303
		{
			CHECKVALID_DBINDEX; 
			
			SC_VERSION	ClientVersion;
			pPacket->ReadData(&ClientVersion, sizeof(SC_VERSION));

			// 접속한 클라이언트의 버전이 서버와 맞지 않을 경우
			if ( (ClientVersion._ui8VerGame != g_pContextMain->m_GameVersion._ui8VerGame) ||
				(ClientVersion._ui16VerBuild != g_pContextMain->m_GameVersion._ui16VerBuild) ||
				(ClientVersion._ui16VerSC != g_pContextMain->m_ClientVersion._ui16VerSC) )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, ProtocolID, KILL_REASON_VER_ERROR );
				break;
			}

			switch( g_pContextMain->m_i32GameGuard )
			{
			case GAMEGUARD_NPROTECT:
				#ifdef USE_NPROTECT
				{
					UINT32 ui32PacketLength;
					pPacket->ReadData(&ui32PacketLength, sizeof( UINT32 ) );
					if( 4096 < ui32PacketLength )
					{
						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAMEGUARD_ERROR, ProtocolID, KILL_REASON_GAME_GUARD );
						break;
					}
					pPacket->ReadData(&m_pCsa.packet, ui32PacketLength);
					i32Temp = m_pCsa.Check( ui32PacketLength );
					if( i32Temp >= 3000 )
					{
					//	switch(Temp)
					//	{
					//	case 1:		break;			//메모리 할당 실패
					//	case 2:		break;			//ggauth##.dll 로드 실패
					//	case 3:		break;			//ggauth##.dll의 Export 함수 가져오기 실패
					//	case 4:		break;			//ggauth##.dll이 초기화되기 전에 Export 함수 호출하였음
					//	case 5:		break;			//csauth2.cfg 파일로부터 설정 로딩 실패.
					//	case 10:	break;			//함수 호출 시 invalid parameter 전달
					//	case 11:	break;			//인증 Query에 대한 클라이언트의 응답 없음
					//	case 12:	break;			//클라이언트의 인증 프로토콜 버전 틀림
					//	case 13:	break;			//인증 Query에 대한 클라이언트의 응답값이 틀림
					//	case 14:	break;			//게임가드 버전 검사 실패. 최신버전의 게임가드를 사용하지 않음.* csa 객체를 처음사용할 때 GetAuthQuery() 를 연속으로 2번 호출 시 발생
					//	case 101:	break;			//Gamemon 버전이 틀림
					//	case 102:	break;			//Gamemon 버전 확인위한 코드값이 틀림				
					//	}

						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD );
					}
					else
					{
						m_ui32GameGuardRecvST	= g_pContextMain->GetServerTime();
						m_ui32GameGuardSendST	= m_ui32GameGuardRecvST;
					}
				}
				#endif
				break;
			case GAMEGUARD_HACKSHIELD:
				#ifdef USE_AHNHS
				{
					//AHNHS_TRANS_BUFFER stResponseBuf; // 클라이언트로부터 수신한 응답 버퍼
					pPacket->ReadData( &m_stResuestBuf.nLength, sizeof( UINT16 ) );
					if( m_stResuestBuf.nLength < ANTICPX_TRANS_BUFFER_MAX )
					{
						pPacket->ReadData( m_stResuestBuf.byBuffer, m_stResuestBuf.nLength );
						// 서동권
						DWORD dwRet = 0; // 세부적인 에러값 - 이전 사용하던 switch 리스트 값들
						DWORD dwRetEx = 0; // ANTICPX_RECOMMAND_CLOSE_SESSION, ANTICPX_RECOMMAND_KEEP_SESSION, ERROR_ANTICPXSVR_VERIFYEX_EXCEPTION

						dwRetEx = _AhnHS_VerifyResponseEx( m_hClient, m_stResuestBuf.byBuffer, m_stResuestBuf.nLength, &dwRet );

						if( dwRetEx == ANTICPX_RECOMMAND_CLOSE_SESSION )
						{
							//게임 클라이언트에서 해킹 행위가 감지되었으므로,
							//게임 클라이언트와의 접속을 차단하도록 합니다.
							char strTemp[512];
							i3String::Format(strTemp, 512, "HackShield Error %x", dwRet );

							// 기존에는 즉시 종료시키지 않고 m_iGameGuardRecvST,m_iGameGuardSendST 시간을 갱신시키지 않아 타임아웃으로 종료되게 하였지만,
							// 안랩측 요청으로 즉시 종료되도록 수정 합니다.
							_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAMEGUARD_ERROR, ProtocolID, KILL_REASON_GAME_GUARD, strTemp );
						}

						m_ui32GameGuardRecvST	= g_pContextMain->GetServerTime();
						m_ui32GameGuardSendST	= m_ui32GameGuardRecvST;


						//ui32Ret = _AhnHS_VerifyResponse( m_hClient, m_stResuestBuf.byBuffer, m_stResuestBuf.nLength );

						//switch( ui32Ret )
						//{
						//case ERROR_ANTICPXSVR_BAD_MESSAGE:
						//case ERROR_ANTICPXSVR_REPLY_ATTACK:
						//case ERROR_ANTICPXSVR_HSHIELD_FILE_ATTACK:
						//case ERROR_ANTICPXSVR_CLIENT_FILE_ATTACK:
						//case ERROR_ANTICPXSVR_MEMORY_ATTACK:
						//case ERROR_ANTICPXSVR_NANOENGINE_FILE_ATTACK:
						//case ERROR_ANTICPXSVR_UNKNOWN_CLIENT:
						//case ERROR_ANTICPXSVR_INVALID_HACKSHIELD_VERSION:
						//case ERROR_ANTICPXSVR_INVALID_ENGINE_VERSION:
						//case ERROR_ANTICPXSVR_ABNORMAL_HACKSHIELD_STATUS:
						//	{
						//		//게임 클라이언트에서 해킹 행위가 감지되었으므로,
						//		//게임 클라이언트와의 접속을 차단하도록 합니다.
						//		char strTemp[512];
						//		i3String::Format(strTemp, "HackShield Error %x", ui32Ret );
						//		g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp);

						//		//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD );
						//	}
						//	break;
						//default:
						//	{
						//		m_iGameGuardRecvST	= g_pContextMain->GetServerTime();
						//		m_iGameGuardSendST	= m_iGameGuardRecvST;
						//	}
						//	break;
						//}

						++m_ui32GameGuardCheckCount;
					}
					else
					{
						//g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, "HackShield Buffer Over");

						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD, "HackShield Buffer Over" );
					}
				}
				#endif
				break;
			case GAMEGUARD_XTRAP:
				#ifdef USE_XTRAP
				{
					// XTrap_CS_Step3
					char pXTrapBuffer[ XTRAP_SETINFO_PACKETBUFF_SIZE ];
					
					pPacket->ReadData( pXTrapBuffer, XTRAP_SETINFO_PACKETBUFF_SIZE );

					UINT32 ui32Res = XTrap_CS_Step3( m_pXtrapSessionBuf, pXTrapBuffer ) ;

					m_ui32GameGuardRecvST	= g_pContextMain->GetServerTime();
					m_ui32GameGuardSendST	= m_ui32GameGuardRecvST;

					//switch( ui32Res )
					//{
					//case XTRAP_API_RETURN_OK		:	// 성공
					//	{
					//		m_iGameGuardRecvST	= g_pContextMain->GetServerTime();
					//		m_iGameGuardSendST	= g_pContextMain->GetServerTime();
					//	}
					//	break;
					//case XTRAP_API_RETURN_ERROR					:	// 오류 발생
					//case XTRAP_API_RETURN_UNKNOWN				:	// 알 수 없는 오류 발생
					//case XTRAP_API_RETURN_INVALID_PARAMETER	:	// 잘못된 매개변수 정보
					//case XTRAP_API_RETURN_INVALID_CRC			:	// CheckSum 오류
					//case XTRAP_API_RETURN_TIMEOUT				:	// 시간 내에 응답이 없음
					//case XTRAP_API_RETURN_DETECTHACK			:	// Hack 탐
					//default:
					//	{
					//		char szTemp[ MAX_PATH ];
					//		i3String::Format( szTemp, MAX_PATH, "[ERROR]XTrap_CS_Step3 / res : %x", ui32Res );
					//		_SendErrorMessage( m_WorkThreadIdx, ui32Res, ProtocolID, KILL_REASON_GAME_GUARD );	// 바로 연결 종료하지 X.
					//	}
					//	break;
					//}
				}
				#endif
				break;
			case GAMEGUARD_GPK:
				#ifdef USE_GPK
				{
					//UINT8 ui8Len = 0;
					unsigned char strData[ GPK_SETINFO_PACKETBUFF_SIZE ];
					
					//pPacket->ReadData( &ui8Len, sizeof(UINT8) );
					pPacket->ReadData( strData, GPK_SETINFO_PACKETBUFF_SIZE );
					if(FALSE == g_pSvrDynCode->Decode( strData, GPK_SETINFO_PACKETBUFF_SIZE, m_i32CodeIdx ))
					{
						// i3String::Format 찍기 위해 혹시나 값 넘어갈까봐 '\0' 넣어줍니다.
						strData[GPK_SETINFO_PACKETBUFF_SIZE_NP] = '\0';
						char strTemp[MAX_PATH];
						sprintf_s(strTemp, MAX_PATH, "[GPK][Decode Error] pData[4](%c),cord(%s)", strData[4], strData);
						g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp);

						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD );
					}

					/*char strTemp[MAX_PATH];
					sprintf_s(strTemp, MAX_PATH, "[GPK][Decode OK] len(%d), cord(%s)", ui8Len, strData);
					g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp);*/

					m_ui32GameGuardRecvST	= g_pContextMain->GetServerTime();
					m_ui32GameGuardSendST	= m_ui32GameGuardRecvST;

				}
				#endif
				break;
			}
		}
		break;
	case PROTOCOL_BASE_GAMEGUARD_FAIL_NOACK		:
		{
			// 브라질 X-Trap 에서 문제가 있을시 클라에서 보냄 ( 해킹탐지나 X-Trap 자체 문제 발생시 ) - 서동권
			CHECKVALID_DBINDEX;

			_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD );

			INT32 i32Result;

			pPacket->ReadData(&i32Result,	sizeof(UINT32));

			char strLog[128];
			i3String::Format( strLog, 128, "[ X-Trap Error ] UID : %I64d, Result : %d", m_i64UID, i32Result );
			g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strLog);
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_REQ			: 		break;	//080303
	case PROTOCOL_BASE_REFRESH_MEDALSYSTEM_REQ	: 
		{
			CHECKVALID_DBINDEX; 
		}		
		break; 
	//임무카드 
	case PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_REQ:		//실행중인 Quest를 종료하고 Chagne가 들어온다.
		{
			CHECKVALID_DBINDEX;
			UINT8	ui8CardSet, ui8Card;
			UINT16	ui16Quest;
			
			pPacket->ReadData(&ui8CardSet,	sizeof(UINT8));			// 카드셋 0,1,2,3
			pPacket->ReadData(&ui8Card,		sizeof(UINT8));			// 카드셋의 카드 0,1,2,3,4,5,6,7,8,9
			pPacket->ReadData(&ui16Quest,	sizeof(UINT16));		// 퀘스트 진행 상태 모두 완료 FFFF

			//변경된 임무카드 세팅을 합니다. 
			if( (ui8CardSet < MAX_CARDSET_PER_USER) && (ui8Card < MAX_CARD_PER_CARDSET) )
			{
				if( (m_QuestIngInfo.m_ActiveIndexUse					!= ui8CardSet) ||		//카드셋이 변경되거나.
					(m_QuestIngInfo.m_pActiveIndexCardSet[ui8CardSet]		!= ui8Card) )			//카드인덱스가 다르면.
				{
					// 변경된 카드로 셋팅
					m_QuestIngInfo.m_ActiveIndexUse						= ui8CardSet; 
					m_QuestIngInfo.m_pActiveIndexCardSet[ui8CardSet]		= ui8Card;  
					ChangeDataBaseInfo(SAVE_DATABASE_MEDAL);
				}
			}
		}
		break; 
	case PROTOCOL_BASE_QUEST_BUY_CARD_SET_REQ	:			//080304		
		{//카드셋 구입하기 
			CHECKVALID_CHANNEL;
			CHECKVALID_SHOP;

			UINT8 ui8CardType; 
			pPacket->ReadData(&ui8CardType, sizeof(UINT8)); 

			// 할인쿠폰으로 인하여 추가. 추후에 캐쉬 임무카드가 생기면 일반 아이템 구입 부분처럼 할인쿠폰 정보도 패킷으로 받아야 합니다.
			UINT32	ui32CouponID = 0;
			INT64	i64CouponIdx = 0;
			//pPacket->ReadData( &ui32CouponID,	sizeof(UINT32) );
      		
			i32Temp = EVENT_ERROR_SUCCESS;

			INT8 i8QuestSlotIdx = _FindInsertQuestSlot( ui8CardType );
			if(	QUEST_CARDSET_TYPE_BUY_START > ui8CardType )						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( QUEST_CARDSET_TYPE_BUY_END < ui8CardType )						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( QUEST_CARDSET_TYPE_EVENT == ui8CardType )						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( EV_FAILED( i8QuestSlotIdx ) )									i32Temp = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; 
			else if( QUEST_CARDSET_TYPE_DINO_TUTORIAL == ui8CardType || QUEST_CARDSET_TYPE_HUMAN_TUTORIAL == ui8CardType )	// 공룡/인간 튜토리얼 예외 처리
			{
				//if ( (m_UserInfoBasic._ui8TutorialIng & (0x10 << (iCardType - QUEST_CARDSET_TYPE_TUTORIAL))) == 0 )
				//{
					// 구입 성공 처리
					g_pContextMain->GetQuest( ui8CardType, i8QuestSlotIdx, &m_QuestIngInfo, m_aaui8QuestCtxActive[ i8QuestSlotIdx ], m_ui8LocaleCode );
					//m_UserInfoBasic._ui8TutorialIng = (m_UserInfoBasic._ui8TutorialIng | (0x10 << (iCardType - QUEST_CARDSET_TYPE_TUTORIAL)));
					ChangeDataBaseInfo(SAVE_DATABASE_MEDAL | SAVE_DATABASE_BASEINFO );

					bSendMessage = TRUE; 
					pSendPacket->SetProtocol(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK);
					pSendPacket->WriteData(&i32Temp,								sizeof(INT32)); 
					pSendPacket->WriteData(&m_UserInfoBasic._point,				sizeof(UINT32)); 
					pSendPacket->WriteData(&i8QuestSlotIdx,						sizeof(INT8));				
					pSendPacket->WriteData(&m_QuestIngInfo,						sizeof(QUESTING_INFO)); 

				//}
				//else
				//{
				//	Temp = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL;
				//}
			}
			else
			{  
				// 해당 임무카드에 맞는 GoodsID를 만듭니다.
				UINT32 ui32GoodsID = MAKE_GOODS_FLAG( GOODS_CLASS_QUESTCARD , ui8CardType);  

				// 추후에 캐쉬 임무 카드가 추가되면 수정 되어야 됩니다.
				UINT8 ui8BuyType = GOODS_BUY_POINT;

				//switch( iCardType ) 
				//{
				//case QUEST_CARDSET_TYPE_GOLD :
				//case QUEST_CARDSET_TYPE_ASSAULT :			
				//case QUEST_CARDSET_TYPE_BACKUP :		
				//case QUEST_CARDSET_TYPE_INFILTRATION :
				//case QUEST_CARDSET_TYPE_SPECIAL	:	
				//case QUEST_CARDSET_TYPE_DEFCON :		
				//case QUEST_CARDSET_TYPE_COMMISSIONED_O :
				//case QUEST_CARDSET_TYPE_COMPANY_O :
				//case QUEST_CARDSET_TYPE_FIELD_O :		
				//case QUEST_CARDSET_TYPE_EVENT :		
				//case QUEST_CARDSET_TYPE_DINO_2ND :			
				//case QUEST_CARDSET_TYPE_HUMAN_2ND :
				//	ui8BuyType = GOODS_BUY_POINT;
				//	break;
				//}

				INT32 i32Result = _BuyGoods( ui32GoodsID, ui8BuyType, ui32CouponID, i64CouponIdx );

				if( EV_FAILED( i32Result ))	// 구입 실패
				{	 
					i32Temp = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; 
					
					switch( i32Result )
					{
					case EVENT_ERROR_EVENT_AUTH_START_PRICE :	// 돈 부족
						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_PRICE_FAIL; 
						break;	 
					case EVENT_ERROR_EVENT_BUY_GOODS_PARAMETER_GOODS :	// 상품이 없다.
					case EVENT_ERROR_FAIL :
						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_NO_GOODS;
						break;
					case EVENT_ERROR_EVENT_BUY_GOODS_OVERLOAD:	// 트랜스 서버 연결 오류
						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_TRANS_ERROR;
						break;
					default	:									// 알수 없는 이유
						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; 
						break;
					} 
				}
			}

			if( EV_FAILED( i32Temp ))	// 구입 실패
			{
				bSendMessage = TRUE; 
				pSendPacket->SetProtocol(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK);
				pSendPacket->WriteData(&i32Temp,									sizeof(INT32)); 
			}
			 
		}
		break; 
		/*
	case PROTOCOL_BASE_QUEST_BUY_CARD_SET_REQ	:			//080304		
		{//카드셋 구입하기 
			CHECKVALID_CHANNEL;

			UINT8 iCardType; 
			pPacket->ReadData(&iCardType, sizeof(UINT8)); 
      		
			Temp = EVENT_ERROR_SUCCESS;

			INT8 i8QuestSlotIdx = 0;
			if(	QUEST_CARDSET_TYPE_BUY_START > iCardType )							Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( QUEST_CARDSET_TYPE_BUY_END < iCardType )						Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( QUEST_CARDSET_TYPE_EVENT == iCardType )						Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( m_UserInfoBasic._point < g_QuestCardSetBuyPoint[iCardType] )	Temp = EVENT_ERROR_EVENT_BUY_CARD_PRICE_FAIL;
			else
			{
				i8QuestSlotIdx = _FindInsertQuestSlot( iCardType );
				if( EV_SUCCESSED( i8QuestSlotIdx ) )
				{
					// 돈 차감
					if( m_UserInfoBasic._point > g_QuestCardSetBuyPoint[iCardType])	m_UserInfoBasic._point = m_UserInfoBasic._point - g_QuestCardSetBuyPoint[iCardType];
					else															m_UserInfoBasic._point = 0;
					g_pContextMain->GetQuest( iCardType, i8QuestSlotIdx, &m_QuestIngInfo, m_pQuestCtxActive[ i8QuestSlotIdx ], m_ui8LocaleCode );

					ChangeDataBaseInfo(SAVE_DATABASE_MEDAL | SAVE_DATABASE_BASEINFO );
				}
				else
				{
					Temp = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; 
				}	
			}

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK);
			pSendPacket->WriteData(&Temp,									sizeof(INT32)); 
			if(EV_SUCCESSED( Temp ))
			{
				pSendPacket->WriteData(&m_UserInfoBasic._point,				sizeof(UINT32)); 
				pSendPacket->WriteData(&i8QuestSlotIdx,						sizeof(INT8));				
				pSendPacket->WriteData(&m_QuestIngInfo,						sizeof(QUESTING_INFO)); 
			}
			
		}
		break; 
		*/
	case PROTOCOL_BASE_QUEST_DELETE_CARD_SET_REQ:		//080309
		{
			CHECKVALID_CHANNEL;
			UINT8 ui8CardSet; 
			pPacket->ReadData(&ui8CardSet, sizeof(UINT8)); 
      		bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK);
			if(ui8CardSet < MAX_CARDSET_PER_USER)
			{
				// _DeleteQuestCardSet 에서 해당값이 지워지기 때문에 먼저 보냅니다.
				g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_QUEST_DELETE, this, &m_QuestIngInfo.m_pCardSetType[ui8CardSet], &m_QuestIngInfo.m_pActiveIndexCard[ui8CardSet] );

				i32Temp = EVENT_ERROR_SUCCESS;
				_DeleteQuestCardSet(ui8CardSet);
			}
			else
			{
				i32Temp = EVENT_ERROR_EVENT_DELETE_CARD_PARAMETER_FAIL; 
			}
			pSendPacket->WriteData(&i32Temp,				sizeof(INT32));
			pSendPacket->WriteData(&m_QuestIngInfo,		sizeof(QUESTING_INFO));
			ChangeDataBaseInfo(SAVE_DATABASE_MEDAL); 
		}
		break; 
	case PROTOCOL_BASE_USER_TITLE_CHANGE_REQ:		//081224
		{
			CHECKVALID_DBINDEX; 

			UINT8 ui8userTitleId;
			pPacket->ReadData(&ui8userTitleId, sizeof(UINT8));

			if( ui8userTitleId > 44 ) 
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE;
			}

			if( 0 == GET_USERTITLE( m_ui64GainUserTitle, ui8userTitleId ))
			{
				LOG_GET_TITLE stLogGetTitle;
				if( _ChangeUserTitle( ui8userTitleId, &stLogGetTitle ) )
				{
					// 획득 호칭에 따른 슬롯 확장
					_SetCalcLimitTitleCount();

					// 호칭 획득에 따른 아이템 추가
					_ExceptionTitleItem( m_WorkThreadIdx, ui8userTitleId );

					//메달 정보 보내기 
					i3NetworkPacket medalPacket( PROTOCOL_BASE_MEDAL_GET_INFO_ACK );
					medalPacket.WriteData(&m_i64UID,			sizeof(TUID));
					medalPacket.WriteData(&m_ui32Miniature,		sizeof(UINT32));
					medalPacket.WriteData(&m_ui32Insignia,		sizeof(UINT32));
					medalPacket.WriteData(&m_ui32Order,			sizeof(UINT32));
					medalPacket.WriteData(&m_ui32Master,		sizeof(UINT32));
					SendPacketMessage(&medalPacket); 

					// 히어로 호칭 획득 시, 베레모 추가
					UINT32 ui32ItemID = 0;
					switch(ui8userTitleId)
					{
					case 13:	ui32ItemID = MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_ASSAULT);	break;
					case 19:	ui32ItemID = MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_RIFLE);		break;
					case 25:	ui32ItemID = MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SMG);		break;
					case 34:	ui32ItemID = MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_HANDGUN);	break;
					case 39:	ui32ItemID = MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SHOTGUN);	break;
					default:	// Do nothing					
						break;
					}
					if( 0 < ui32ItemID )
					{
						// 필요한 베레모가 있으면 추가 시켜줍니다.
						if( _ExceptionCharaInsert( ui32ItemID ) )
						{
							// 클라이언트에 보내기 위한 버퍼에 세팅
							INVEN_BUFFER_SET TempInvenSet;
							TempInvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_INSERT;
							_ExceptionInsert( &TempInvenSet.aCharaBuffer[0], ui32ItemID );
							TempInvenSet.ui32CharaCount++;
							// 아이템 전달
							SendChangeInven( &TempInvenSet );
						}
					}

					i32Rv = EVENT_ERROR_SUCCESS;
					g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_TITLE_BUY, this, &ui8userTitleId, &stLogGetTitle ); 
				}
				else
				{
					i32Rv = EVENT_ERROR_USER_TITLE_REQUIREMENT_FAIL;
				}
			}
			else
			{
				i32Rv = EVENT_ERROR_USER_TITLE_ALREADY_GAIN_FAIL;
			}

			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_BASE_USER_TITLE_CHANGE_ACK);
			pSendPacket->WriteData( &i32Rv,						sizeof(INT32));
			pSendPacket->WriteData( &m_i32LimitUserTitleCount,	sizeof(INT32));
			ChangeDataBaseInfo(SAVE_DATABASE_MEDAL); 
		}
		break;
	case PROTOCOL_BASE_USER_TITLE_EQUIP_REQ:			//081224
		{
			CHECKVALID_DBINDEX; 

			UINT8 ui8equipTitleSlotIdx;
			UINT8 ui8userTitleId;

			pPacket->ReadData(&ui8equipTitleSlotIdx,	sizeof(UINT8));
			pPacket->ReadData(&ui8userTitleId,			sizeof(UINT8));

			if( (ui8userTitleId >= MAX_USER_TITLE) || (ui8equipTitleSlotIdx >= MAX_EQUIP_USER_TITLE) ) 
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE;
			}

			// 호칭 장비
			INT32 i32result = EVENT_ERROR_FAIL;
			// 보유중인 호칭인가?
			//if ( m_GainUserTitle[userTitleId] == 1)
			if( 1 == GET_USERTITLE( m_ui64GainUserTitle, ui8userTitleId ))
			{
				// 장비가 가능한 슬롯인가?
				if (ui8equipTitleSlotIdx < m_i32LimitUserTitleCount)
				{
					// 베레모 장비중이면 해제
					if ((m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 13 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_ASSAULT))	||
						(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 19 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_RIFLE))	||
						(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 25 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SMG))		||
						(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 39 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SHOTGUN))	||
						(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 34 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_HANDGUN)) 	)
					{
						m_UserInfoEquipmentChara.SetItemNum( 0 );
						
						_SendEquipment();
						ChangeDataBaseInfo(SAVE_DATABASE_EQUIPMENT);
					}


					m_aui8EquipUserTitle[ui8equipTitleSlotIdx] = ui8userTitleId;
					i32result = EVENT_ERROR_SUCCESS;
				}
			}
		
			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_BASE_USER_TITLE_EQUIP_ACK);
			pSendPacket->WriteData(&i32result, sizeof(INT32));
			ChangeDataBaseInfo(SAVE_DATABASE_MEDAL); 
		}
		break;
	case PROTOCOL_BASE_USER_TITLE_RELEASE_REQ:	//081224
		{
			CHECKVALID_DBINDEX; 

			UINT8 ui8equipTitleSlotIdx;
			UINT8 ui8userTitleId;

			pPacket->ReadData(&ui8equipTitleSlotIdx, sizeof(UINT8));
			pPacket->ReadData(&ui8userTitleId, sizeof(UINT8));


			// 호칭 해제
			INT32 i32result = EVENT_ERROR_FAIL;

			if (ui8equipTitleSlotIdx < m_i32LimitUserTitleCount)
			{
				// 베레모 장비중이면 해제
				if ((m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 13 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_ASSAULT))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 19 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_RIFLE))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 25 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SMG))		||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 39 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_SHOTGUN))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == 34 && m_UserInfoEquipmentChara.GetItemNum() == MAKE_ITEM_FLAG(ITEM_CLASS_HEAD, CHARACTER_CLASS_ITEM, CHARACTER_TEAM_RB, ITEM_RES_ID_BERET_HANDGUN)) 
					)
				{
					m_UserInfoEquipmentChara.SetItemNum( 0 );
					
					_SendEquipment();
					ChangeDataBaseInfo(SAVE_DATABASE_EQUIPMENT);
				}


				m_aui8EquipUserTitle[ui8equipTitleSlotIdx] = 0;
				i32result = EVENT_ERROR_SUCCESS;
			}

			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_BASE_USER_TITLE_RELEASE_ACK);
			pSendPacket->WriteData(&i32result, sizeof(INT32));
			ChangeDataBaseInfo(SAVE_DATABASE_MEDAL); 
		}
		break;
	case PROTOCOL_BASE_QUEST_FINISH_NOTICE_REQ		: 
		{
			CHECKVALID_ROOM;
			m_pRoom->SetQuestEndFlag( m_i32SlotIdx ); //내부에서 패킷을 보냅니다. 
			ChangeDataBaseInfo(SAVE_DATABASE_MEDAL); 
		}
		break; 
	case PROTOCOL_BASE_MEDAL_GET_INFO_REQ:
		{
			CHECKVALID_CHANNEL;

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_MEDAL_GET_INFO_ACK);
			pSendPacket->WriteData(&m_i64UID,			sizeof(TUID) );
			pSendPacket->WriteData(&m_ui32Miniature,	sizeof(UINT32));
			pSendPacket->WriteData(&m_ui32Insignia,		sizeof(UINT32));
			pSendPacket->WriteData(&m_ui32Order,		sizeof(UINT32));
			pSendPacket->WriteData(&m_ui32Master,		sizeof(UINT32));
		}
		break;
	case PROTOCOL_BASE_NOT_USE_UPDATE_SERVER:
		{
			bisKilled 	= TRUE; 
			m_ui32KillReason 	= KILL_REASON_UNKNOWN_PACKET;

			g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, "Not Updater User Login");
		}
		break;
	case PROTOCOL_BASE_HACKING_CHECK_KICK_REQ:
		{
			INT32	i32CheckCode;

			pPacket->ReadData( &i32CheckCode, sizeof(INT32) );

			char strLog[256];
			i3String::Format( strLog, 256, "Client check code - UID: %I64d, CheckCode: %d", m_i64UID, i32CheckCode );
			g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strLog );

			if( i32CheckCode >= 1000 )
			{
				// User Kick
				bisKilled 	= TRUE; 
				m_ui32KillReason	= KILL_REASON_CLIENT_HACK_CHECK;
			}
		}
		break;
	case PROTOCOL_BASE_RANKUP_ITEM_LIST_REQ: // 랭크업 아이템 요청
		{
			CHECKVALID_DBINDEX; 

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_RANKUP_ITEM_LIST_ACK);
			pSendPacket->WriteData(&g_pContextMain->m_aRankUpItem,	sizeof(g_pContextMain->m_aRankUpItem) );
		}
		break;
	case PROTOCOL_BASE_DAILY_RECORD_REQ: // 당일전적요청
		{
			CHECKVALID_DBINDEX; 

			m_UserInfoDaily._match = m_UserInfoDaily._win + m_UserInfoDaily._lose + m_UserInfoDaily._draw;

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_DAILY_RECORD_ACK);
			pSendPacket->WriteData(&m_UserInfoDaily,	sizeof(USER_INFO_DAILY) );
		}
		break;

	case PROTOCOL_BASE_ATTENDANCE_INFO_REQ:
		{
			CHECKVALID_DBINDEX; 

			g_pModuleTrans->AuthSendAttendanceInfoReq( m_WorkThreadIdx, m_i64UID );
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_REQ:
		{
			CHECKVALID_MYINFO;

			UINT32 ui32AttendanceNum;
			pPacket->ReadData( &ui32AttendanceNum,		sizeof( UINT32 ) );
			g_pModuleTrans->AuthSendAttendanceReq( m_WorkThreadIdx, m_i64UID, ui32AttendanceNum );
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ:
		{
			CHECKVALID_MYINFO;

			UINT32 ui32AttendanceNum;
			UINT8 ui8AttendanceItemNum;

			pPacket->ReadData( &ui32AttendanceNum,		sizeof( UINT32 ) );
			pPacket->ReadData( &ui8AttendanceItemNum,	sizeof( UINT8 ) );

			g_pModuleTrans->AuthSendAttendanceItemReq( m_WorkThreadIdx, m_i64UID, ui32AttendanceNum, ui8AttendanceItemNum );		
		}
		break;
	case PROTOCOL_BASE_GET_USER_LIST_REQ:
		{
			m_bRequestUserList	= TRUE;
		}
		break;
	case PROTOCOL_BASE_SERVER_USER_NOT_FIND:
		{	// 고의 적인 코드입니다. 자세한 내용은 김현우에게 물어보세요.
			char strPass[ 32 ];
			pPacket->ReadData( strPass,		32 );
			strPass[ 31 ] = '\0';

			if( 0 == i3String::Compare( strPass, "rlatjdqorrlagusdnqkfh^%$#@!" ) )
			{
				g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, NULL, "[ERROR] Not Find User. timeout." );

				// 로그 찍을 수 있도록 Sleep 줍니다.
				Sleep( 1000 );

				// 고의 적인 코드입니다. 자세한 내용은 김현우에게 물어보세요.
				char* pTemp = NULL;
				pTemp[0] = 1;
			}
			else
			{
				char strTemp[256];
				i3String::Format( strTemp, 256, "ChServer Fail:%s", strPass );
				g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, NULL, strTemp );
			}
			// 고의 적인 코드입니다. 자세한 내용은 김현우에게 물어보세요.
		}
		break;
	case PROTOCOL_BASE_SERVER_INFO_REQ:
		{
			char strLog[ MAX_PATH ];
			i3String::Format( strLog, MAX_PATH, "Ver-%d.%d.%d.%d, %s, Date-%s %s", VER_GAME, VER_BUILD, VER_SC_PROTOCOL, VER_SS_PROTOCOL, BUILD_VER, __DATE__, __TIME__ );
			INT16 i16Size = (INT16)i3String::Length( strLog );

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_SERVER_INFO_ACK);
			pSendPacket->WriteData( &m_ui8LocaleCode,	sizeof(UINT8) );
			pSendPacket->WriteData( &i16Size,			sizeof(INT16) );
			pSendPacket->WriteData( strLog,				i16Size);
		}
		break;
		//////////////////////////////////////////////////////////////////////////
		// Attack Packet(USE_SERVER_WATCH_CONTENTS)
		//////////////////////////////////////////////////////////////////////////
	case PROTOCOL_BASE_HEART_BIT_2:
		{
			// 해당 기능은 임시로 막아둡니다.
			break;
			if( false == m_bIsRecvHeartBit2 )	{ m_bIsRecvHeartBit2 = true; }
		#ifdef USE_SERVER_WATCH_CONTENTS
			PACKET_H_TCP_CLIENT_AT_PACKET		PacketH;
			PACKET_D_TCP_CLIENT_AT_PACKET		PacketD;

			srand(g_pContextMain->GetServerTime());
			UINT8 ui8AttackCnt = (rand() % MAX_AT_TYPE);
			if(MAX_AT_TYPE < ui8AttackCnt) { ui8AttackCnt = 0; }
			PacketH.m_len = strlen(g_strAtData[ui8AttackCnt]);
			i3String::Copy( PacketD.m_strData, g_strAtData[ui8AttackCnt], PacketH.m_len);

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_HEART_BIT_2);
			pSendPacket->WriteData( &PacketH,				sizeof(PACKET_H_TCP_CLIENT_AT_PACKET) );
			pSendPacket->WriteData( &PacketD.m_strData,	sizeof(PacketH.m_len) );
		#endif
		}
		break;
	case PROTOCOL_BASE_READY_HEART_BIT_REQ:
		{
		#ifdef USE_SERVER_WATCH_CONTENTS
			GAME_SERVER_VERSION GameVersion;
			GameVersion._vGSVInfo	= VER_WATCH_PACKET_GS;
			GameVersion._LocaleCode = LOCALE_NUMBER;
			GameVersion._vGame		= VER_GAME;
			GameVersion._vBuild		= VER_BUILD;
			GameVersion._vSC		= VER_SC_PROTOCOL;
			GameVersion._vSS		= VER_SS_PROTOCOL;
			GameVersion._vUDPMain	= VER_UDP_PROTOCOL_MAIN;
			GameVersion._vUDPSub	= VER_UDP_PROTOCOL_SUB;
			GameVersion._GameGuard	= g_pContextMain->m_i32GameGuard;

			i3String::Copy( GameVersion._vDate, __DATE__, sizeof( GameVersion._vDate ) );
			i3String::Copy( GameVersion._vTime, __TIME__, sizeof( GameVersion._vTime ) );
			i3String::Copy( GameVersion._vBuildType, BUILD_VER, sizeof( GameVersion._vBuildType ) );

			i3NetworkPacket SendPacket( PROTOCOL_BASE_READY_HEART_BIT_ACK );
			SendPacket.WriteData( &g_pContextMain->m_ui32GameServerCount,	sizeof(UINT32) );
			SendPacket.WriteData( &GameVersion,								sizeof(GAME_SERVER_VERSION) );

			SendPacketMessage(&SendPacket);
		#endif
		}
		break;
	case PROTOCOL_BASE_SECURITYID_DUPLICATE_REQ:
		{
			CHECKVALID_DBINDEX;
			
			UINT8	ui8IDSize;
			char	strSecurityID[NET_ID_SIZE];
			
			pPacket->ReadData( &ui8IDSize,		sizeof( UINT8 ) );	LENGTH_CHECK( ui8IDSize, NET_ID_SIZE );
			pPacket->ReadData( strSecurityID,	ui8IDSize );

			if ( _CheckSID_ID( strSecurityID, ui8IDSize ) )
			{
				g_pModuleTrans->AuthSendCheckSID( m_WorkThreadIdx, this, ui8IDSize, strSecurityID );
			}
			else
			{
				i3NetworkPacket SendPacket( PROTOCOL_BASE_SECURITYID_DUPLICATE_ACK );
				INT32 i32ErrRv = EVENT_ERROR_SECURITYID_ALREADY_EXIST;
				SendPacket.WriteData( &i32ErrRv, sizeof( INT32 ) );
				SendPacketMessage( &SendPacket );
			}
		}
		break;
	case PROTOCOL_BASE_SECURITYID_CREATE_REQ:
		{
			CHECKVALID_DBINDEX;
			
			UINT8	ui8IDSize;
			UINT8	ui8PWSize;
			char	strSecurityID[NET_ID_SIZE];
			char	strSecurityPW[NET_PW_SIZE];
			
			pPacket->ReadData( &ui8IDSize,		sizeof( UINT8 ) );	LENGTH_CHECK( ui8IDSize, NET_ID_SIZE );
			pPacket->ReadData( strSecurityID,	ui8IDSize );
			pPacket->ReadData( &ui8PWSize,		sizeof( UINT8 ) );	LENGTH_CHECK( ui8PWSize, NET_PW_SIZE );
			pPacket->ReadData( strSecurityPW,	ui8PWSize );

			if ( _CheckSID_Password( ui8PWSize ) &&
				_CheckSID_ID( strSecurityID, ui8IDSize ) )
			{
				g_pModuleTrans->AuthSendCreateSID( m_WorkThreadIdx, this, ui8IDSize, strSecurityID, ui8PWSize, strSecurityPW );
			}
			else
			{
				i3NetworkPacket SendPacket( PROTOCOL_BASE_SECURITYID_CREATE_ACK );
				INT32 i32ErrRv = EVENT_ERROR_SECURITYID_CREATE_FAIL;
				SendPacket.WriteData( &i32ErrRv, sizeof( INT32 ) );
				SendPacketMessage( &SendPacket );
			}
		}
		break;
#ifdef USE_SECURITYID_TEST_MODE
	case PROTOCOL_BASE_SECURITYID_TEST_CREATE_REQ:
		{
			CHECKVALID_LOGIN;

			g_pModuleTrans->AuthSendTestCreateSID( m_WorkThreadIdx );
		}
		break;
#endif
	case PROTOCOL_BASE_GET_UID_LOBBY_REQ:
		{
			CHECKVALID_CHANNEL;

			if( FALSE == IsGMUser() )
			{
				// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			INT32 i32SessionIdx;
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );

			CUserSession * pUserSession = g_pUserManager->GetSession( i32SessionIdx );

			INT32	i32Result = EVENT_ERROR_FAIL;
			TUID	T_UID;
			if( pUserSession )
			{
				i32Result	= EVENT_ERROR_SUCCESS;
				T_UID		= pUserSession->GetUID();
			}

			bSendMessage = TRUE;			
			pSendPacket->SetProtocol( PROTOCOL_BASE_GET_UID_LOBBY_ACK );
			pSendPacket->WriteData( &i32Result,		sizeof(INT32) );
			pSendPacket->WriteData( &T_UID,			sizeof(TUID) );
		}
		break;
	case PROTOCOL_BASE_GET_UID_ROOM_REQ:
		{
			CHECKVALID_ROOM;

			if( FALSE == IsGMUser() )
			{
				// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			UINT8 ui8SlotIdx;
			pPacket->ReadData( &ui8SlotIdx,		sizeof(UINT8) );

			INT32	i32Result = EVENT_ERROR_FAIL;
			TUID	T_UID;
			if( SLOT_MAX_COUNT > ui8SlotIdx )
			{
				CUserSession * pUserSession = m_pRoom->GetSession( ui8SlotIdx );

				if( pUserSession )
				{
					i32Result	= EVENT_ERROR_SUCCESS;
					T_UID		= pUserSession->GetUID();
				}
			}

			bSendMessage = TRUE;			
			pSendPacket->SetProtocol( PROTOCOL_BASE_GET_UID_ROOM_ACK );
			pSendPacket->WriteData( &i32Result,		sizeof(INT32) );
			pSendPacket->WriteData( &T_UID,			sizeof(TUID) );
		}
		break;
//////////////////////////////////////////////////////////////////////////
	default:
			char strTemp[256];
			i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled 	= TRUE; 
			m_ui32KillReason 	= KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}
