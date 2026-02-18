#include "pch.h"
#include "UserSession.h"
#include "ModuleTrans.h"

BOOL CUserSession::_PacketParsingLogin( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	PROTOCOL	ProtocolID		= pPacket->GetProtocolID();	
	BOOL		bSendMessage	= FALSE; 

	UINT64		ui64DXFunc[4];
	ui64DXFunc[0] = 0;
	ui64DXFunc[1] = 1;
	ui64DXFunc[2] = 2;
	ui64DXFunc[3] = 3;

		
	// Connect 상태가 아닐 경우 종료
	CHECKVALID_CONNECT;

	switch( ProtocolID )
	{
	case PROTOCOL_LOGIN_REQ	:
		{
			PACKET_LOGIN_REQ Recv;

			// 패킷을 받기 전에 0,0으로 초기화 해야 합니다.
			// 생성자에서 받게 되면 안되는 이유는 클라에서도 사용합니다.....
			SC_VERSION	stClientVersion;
			stClientVersion._ui8VerGame = 0;
			stClientVersion._ui16VerSC = 0;
			Recv.m_stVersion = stClientVersion;

			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
	
			char	strID[ NET_ID_SIZE ];
			char	strPasswd[ NET_PW_SIZE ];
			Recv.m_strID.GetValue( strID, NET_ID_SIZE );
			Recv.m_strPasswd.GetValue( strPasswd, NET_PW_SIZE );
			m_MacAddress = Recv.m_i64MacAddress;

			m_ui8PBVer = g_pContextMain->RegistPBVersion(Recv.m_ui8PBVer);
			
			UINT16	ui16SessionKeySize	= 0;
			char	pSessionKey[ NET_SESSION_KEY_SIZE ];
			INT32	i32SessionKey;
			Recv.m_pSessionKey.GetValue( pSessionKey, NET_SESSION_KEY_SIZE, &i32SessionKey );
			ui16SessionKeySize = (UINT16)i32SessionKey;

			m_stPCSpec = Recv.m_stPCSpec;

			stClientVersion = Recv.m_stVersion;
			// 접속한 클라이언트의 버전이 서버와 맞지 않을 경우
			if( FALSE == g_pContextMain->m_ClientVersion.VersionCompare( &stClientVersion ) )
			{
				wchar_t wstrTemp[ MAX_PATH ];
				i3String::Format( wstrTemp, MAX_PATH, L"[Wrong Version][ver(%d.%d.%d)]", stClientVersion._ui8VerGame, stClientVersion._ui16VerSC, VER_SS_PROTOCOL );
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN_INVALID_VERSION, ProtocolID, KILL_REASON_VER_ERROR,  wstrTemp );
				return FALSE;
			}

			if (Recv.m_ui8LinkMethod >= LINK_METHOD_MAX)
			{
				wchar_t strTemp[MAX_PATH];
				i3String::Format( strTemp, MAX_PATH, L"[Wrong LinkMethod][Value:%d]", Recv.m_ui8LinkMethod );
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT_LOG_IN_PACKET, ProtocolID, KILL_REASON_HACK_PACKET, strTemp);
				return FALSE;
			}

			if (_CheckUlsanGuard(m_WorkThreadIdx, ProtocolID, &Recv.m_stHackCheckLogin, ui64DXFunc, strID))
			{
				break;
			}
			
			// Locale Setting
			if( LOCALE_CODE_NONE == g_pContextMain->m_ui8LocalCode )
			{
				if (Recv.m_ui8LocalCode >= LOCALE_CODE_MAX) m_ui8LocaleCode = g_pContextMain->m_ui8LocalCode;
				else m_ui8LocaleCode = Recv.m_ui8LocalCode;
			}

			// 닉네임 검출 조건
			// 브라질이 따로 존재하여 추가합니다.
			switch (m_ui8LocaleCode)
			{
			case LOCALE_CODE_BRAZIL:
				{
					char* pstrID = m_strId;
					INT32 ui8IDLenght = i3String::Length(m_strId);
					for (INT32 i = 0; i < ui8IDLenght; i++)
					{
						//if( ( 'a' <= strID[i] && 'z' >= strID[i] ) ||
						//	( 'A' <= strID[i] && 'Z' >= strID[i] ) ||
						//	( '0' <= strID[i] && '9' >= strID[i] ) )
						if (strID[i] != '\\')
						{
							*pstrID = strID[i];
							pstrID++;
						}
					}
					*pstrID = '\0';
				}
				break;
			}

#if defined( BUILD_DIST_VERSION )
			if( g_pContextMain->m_bMacAddressCheck ) 
			{
				// Dist 버전에서만 사용됩니다.
				switch( m_ui8LocaleCode )
				{
				case LOCALE_CODE_NONE:
				case LOCALE_CODE_MATCH:
				case LOCALE_CODE_BRAZIL:
					break;
			
				case LOCALE_CODE_INDONESIA:
					// MD5 Key 가 0으로 세팅이 되어있다면 Control 서버 Config 에서 Not Use 로 설정이 되어있는 것, QA서버이므로 아래 검사를 넘억바니다. - 서동권
					if( i3String::Compare( "0", g_pContextMain->m_strMD5Key[0] ) == 0) break;

					char strMD5[ NET_MD5_KEY_SIZE ];
					Recv.m_strMD5.GetValue( strMD5, NET_MD5_KEY_SIZE );
					if( !g_pContextMain->CheckMD5Key( strMD5 ) ) 
					{
						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, PROTOCOL_LOGIN_REQ, KILL_REASON_MD5_ERROR );
						return FALSE;
					}
					//break;		// 밑에 값도 체크해야합니다.
				default:
					{
						UINT32	ui32Key1, ui32Key2;

						DataDecript( Recv.m_ui64EncryptKey, &ui32Key1, &ui32Key2 );

						UINT64	ui64MacAddress = (UINT64)ui32Key1 << 32 | ui32Key2;

						if( m_MacAddress != ui64MacAddress )
						{
							// 불법 프로그램으로 간주하고 실패 Ack 를 보냅니다.
							bSendMessage = TRUE;
							INT32 i32Rv = EVENT_ERROR_EVENT_LOG_IN_BLOCK_GAME | 0x2;	// 불법프로그램으로 끊겠다는 메시지입니다.
							pSendPacket->SetProtocol( PROTOCOL_LOGIN_ACK );
							pSendPacket->WriteData( &i32Rv,	sizeof(INT32));
														
							_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN, ProtocolID, KILL_REASON_HACK_PACKET, L"PROTOCOL_BASE_LOGIN_ACK" );

							g_pLog->WriteLogSession( this, L"Login Key Error RecvEncryptKey : %I64u, RecvMacAddress : %I64u, DecryptMacAddress : %I64u",
								Recv.m_ui64EncryptKey, m_MacAddress, ui64MacAddress );

							return FALSE;
						}

						// 인도네시아의 경우 활성화 된 맥어드레스 값을 피시방에서 사용하여야 함.
						// 그런데 런처에서는 활성화 된 맥어드레스 값이 아닌 무조건 첫번째 맥어드레스를 보내와서
						// 검사에서는 첫번째 맥어드레스를 사용하고 그다음은 활성화 값으로 바꿈.
						if( m_ui8LocaleCode == LOCALE_CODE_INDONESIA )	m_MacAddress = Recv.m_i64ActiveMacAddress;
					}
					break;
				}
			}
#endif

			UINT32 ui32Value	= GET_LOGIN_KEY_VALUE( m_ui32LoginKey );
			ui32Value++;
			m_ui32LoginKey		= SET_LOGIN_KEY( m_WorkThreadIdx, ui32Value );				

			if( g_pModuleTrans->AuthSendLogIn( m_WorkThreadIdx, this, strID, strPasswd, ui16SessionKeySize, pSessionKey, Recv.m_ui8LinkMethod ) )
			{
				SetPosition( GAME_TASK_LOGIN_ING );
				m_ui32ATLogin = i3ThreadTimer::GetServerTime();
			}
			else
			{
				LogIn_MAU( m_WorkThreadIdx, EVENT_ERROR_EVENT_LOG_IN_TIME_OUT1, 0, NULL );
			}
		}
		break;
	default:
		{
			char strTemp[256];
			sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled 		= TRUE; 
			m_ui32KillReason= KILL_REASON_UNKNOWN_PACKET;
			break;
		}
	}

	return bSendMessage;
}