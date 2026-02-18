#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingLobby( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{
	INT32		i32Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_LOBBY_ENTER_REQ		:					//로비로 들어가기	0122	0829
		{
			CHECKVALID_CHANNEL; 

			//1. 방에 있으면 방에서 나가기 방어 코드 
			g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum, m_WorkThreadIdx, ROOM_ENDTYPE_NONE);

			//2. 룸리스트를 보낸다. 
			m_bRequestRoomList		= FALSE;
			m_ui32UpdateRoomListPage	= 100;  // 0번 부터 받기 위해서 큰수를 사용했습니다. 
			m_ui32UpdateUserListPage	= 100;  // 0번 부터 받기 위해서 큰수를 사용했습니다.  

			//3. 포지션 세팅
			SetPosition(GAME_TASK_LOBBY);

			//4. 정상적인 메시지를 보낸다. 
			bSendMessage = TRUE;
			i32Temp = EVENT_ERROR_SUCCESS; 
			pSendPacket->SetProtocol( PROTOCOL_LOBBY_ENTER_ACK );
			pSendPacket->WriteData(&i32Temp, sizeof(INT32) );

			// 클랜서버에 알려준다.
			g_pModuleClan->ClanSendChannelEnter(m_WorkThreadIdx, this, m_i32ChannelNum );

			// 메신저 서버에 Channel Num을 알려준다.
			// 현재는 사용안함. 필요시 사용 예정.
		#ifdef USE_UNDECIDED_DESIGN_TEST
			g_pModuleMessenger->MessengerSendChannelEnter(m_WorkThreadIdx, this, m_i64UID, m_i32ChannelNum );
		#endif
		}
		break;
	case PROTOCOL_LOBBY_LEAVE_REQ	:					//0829
		{
			CHECKVALID_CHANNEL;
			if(m_i32RoomIdx != SERVER_VALUE_UNKNOWN)
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE;
			}

			//채널에서 나가기 
			g_pUserManager->OnLeaveChannel( this, m_i32ChannelNum);
			SetPosition(GAME_TASK_CHANNEL_LIST); 			//순서 중요
			m_i32ChannelNum		= SERVER_VALUE_UNKNOWN;
			m_i32LogChannelNum	= SERVER_VALUE_UNKNOWN;
			
			//메시지 보내기 
			i32Temp = 0;
			bSendMessage = TRUE;			
			pSendPacket->SetProtocol( PROTOCOL_LOBBY_LEAVE_ACK );
			pSendPacket->WriteData(&i32Temp, sizeof(INT32) );

			// 클랜서버에 알려준다.
			g_pModuleClan->ClanSendChannelLeave(m_WorkThreadIdx, this );

			// 메신저 서버에 Channel Num을 알려준다.
			// 현재는 사용안함. 필요시 사용 예정.
		#ifdef USE_UNDECIDED_DESIGN_TEST
			g_pModuleMessenger->MessengerSendChannelEnter(m_WorkThreadIdx, this, m_i64UID );
		#endif
		}
		break; 
	case PROTOCOL_LOBBY_CREATE_NICK_NAME_REQ	:		//닉네임 만들기		0122  0829
		{
			CHECKVALID_CHANNEL;

			if( 0 != m_UserInfoBasic._nick[0] ) 
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE; 
			}
			
			UINT8	ui8NickSize;
			char	strNickName[ NET_NICK_NAME_SIZE ];

			pPacket->ReadData( &ui8NickSize,	sizeof(UINT8) );	LENGTH_CHECK( ui8NickSize, NET_NICK_NAME_SIZE );
			pPacket->ReadData( strNickName,		ui8NickSize );
			strNickName[NET_NICK_NAME_SIZE - 1] = 0; 


			switch (m_ui8LocaleCode)
			{
			case LOCALE_CODE_TURKEY:		// 터키의 경우 닉네임 앞부분을 GM이나 MOD_로 생성할 수 없음 - 2012-09-07 김용수
				{
					if ( (strNickName[0] == 'G') && (strNickName[1] == 'M') )
					{
						i32Temp = EVENT_ERROR_FAIL;
						bSendMessage= TRUE;
						pSendPacket->SetProtocol( PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK );
						pSendPacket->WriteData(&i32Temp, sizeof(INT32));
						break;
					}
					if ( (strNickName[0] == 'M') && (strNickName[1] == 'O') && (strNickName[2] == 'D') && (strNickName[3] == '_') )
					{
						i32Temp = EVENT_ERROR_FAIL;
						bSendMessage= TRUE;
						pSendPacket->SetProtocol( PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK );
						pSendPacket->WriteData(&i32Temp, sizeof(INT32));
						break;
					}
				}
				break;

			// 클라팀 요청으로 일단 국가코드 없이 세팅되도록 수정합니다.
			// 2월빌드로 머지할 때는 지울 예정입니다.
			default:
				{
					if ( (strNickName[0] == 'G') && (strNickName[1] == 'M') )
					{
						i32Temp = EVENT_ERROR_FAIL;
						bSendMessage= TRUE;
						pSendPacket->SetProtocol( PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK );
						pSendPacket->WriteData(&i32Temp, sizeof(INT32));
						break;
					}
					if ( (strNickName[0] == 'M') && (strNickName[1] == 'O') && (strNickName[2] == 'D') && (strNickName[3] == '_') )
					{
						i32Temp = EVENT_ERROR_FAIL;
						bSendMessage= TRUE;
						pSendPacket->SetProtocol( PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK );
						pSendPacket->WriteData(&i32Temp, sizeof(INT32));
						break;
					}
				}
				break;
			}


			// 완료가 되었다면 서버 이동한다는 사실을 중계서버에 알린다.
			if( FALSE == g_pModuleTrans->AuthSendCreateNickName(	m_WorkThreadIdx, this, strNickName ) )
			{
				i32Temp = EVENT_ERROR_FAIL;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_LOBBY_CREATE_NICK_NAME_ACK );
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
			}
		}
		break;
	case PROTOCOL_LOBBY_CREATE_ROOM_REQ		:							//방 만들기 요청 	완료	확인 0315	0829
		{
			CHECKVALID_CHANNEL; 

			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
				return FALSE; 
			}

			ROOM_INFO_CREATE InfoCreate;
			InfoCreate.Reset();
			
			pPacket->ReadData(&InfoCreate.RoomInfoBasic,	sizeof(ROOM_INFO_BASIC));		//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
			pPacket->ReadData(&InfoCreate.RoomInfoAdd,		sizeof(ROOM_INFO_ADD));			//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
			pPacket->ReadData(InfoCreate.szRoomPw,			NET_ROOM_PW);					//Room.cpp에서 내용 확인			
			InfoCreate.RoomInfoBasic._Title[ NET_ROOM_NAME_SIZE-1 ]	= '\0';
			InfoCreate.RoomInfoAdd._Nick[ NET_NICK_NAME_SIZE-1 ]	= '\0';

			if ( ( STAGE_MASK_TYPE_TRAINING == InfoCreate.RoomInfoBasic._StageMask ) ||
				 ( GET_STAGETYPE( InfoCreate.RoomInfoBasic._StageID ) == STAGE_TYPE_DOMINATION ) ) 
			{	// 훈련전이라면 적에 대한 값도 받는다.
				pPacket->ReadData(&InfoCreate.enemyAiCount,		sizeof(INT8));
				pPacket->ReadData(&InfoCreate.difficultyLevel,	sizeof(INT8));
			}

			_CreateRoom( &InfoCreate );
		}
		break;
	
	case PROTOCOL_LOBBY_GET_ROOMLIST_REQ		:							//로비에서 방정보 요청하기 확인 0731	0829
		{
			CHECKVALID_CHANNEL; 
			m_bRequestRoomList = TRUE;
		}
		break; 
	case PROTOCOL_LOBBY_JOIN_ROOM_REQ			:							//방에 들어가기					0331	0829
		{
			CHECKVALID_CHANNEL; 

			if( m_i32RoomIdx > SERVER_VALUE_UNKNOWN )
			{
				// 이 경우 연속적인 더블클릭으로 가능할것으로 보여 무시하겠습니다.
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
				return FALSE; 
			}

			UINT32			ui32RoomIdx; 
			ROOM_INFO_JOIN	InfoJoin;
			InfoJoin.Reset();
			
			pPacket->ReadData( &ui32RoomIdx,		sizeof(UINT32) );
			pPacket->ReadData( InfoJoin.szRoomPw,	NET_ROOM_PW);
			pPacket->ReadData( &InfoJoin.cForce,	sizeof(UINT8) );

			i32Temp = g_pRoomManager->OnJoinRoom( this, m_i32ChannelNum, ui32RoomIdx, &InfoJoin );

			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_LOBBY_JOIN_ROOM_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );				//방에 들어가지 못하는 내용이 들어가 있음 
			}
		}
		break;
	case PROTOCOL_LOBBY_QUICKJOIN_SETTING_REQ			: // 빠른 입장 설정 요청 - 서동권 2011.08.04
		{
#if defined( USE_QUICK_JOIN )
			// 빠른 입장 설정된 구조체를 받아서 해당 유저 정보에 입력하면 끝. 로그아웃시 해당 정보를 DB에 저장 - SaveUserInfo

			CHECKVALID_CHANNEL;

			QUICK_JOIN_INFO	st_QuickInfo;

			pPacket->ReadData( &st_QuickInfo, sizeof( QUICK_JOIN_INFO ) );

			for( UINT8 i = 0; i < MAX_QUICK_INFO; ++i )
			{
				m_stQuickInfo._ui8Map[ i ] = st_QuickInfo._ui8Map[ i ];
				m_stQuickInfo._ui8Mode[ i ] = st_QuickInfo._ui8Mode[ i ];
			}

			ChangeDataBaseInfo( SAVE_DATABASE_QUICK_JOIN );
#endif
		}
		break;
	case PROTOCOL_LOBBY_QUICKJOIN_ROOM_REQ			: //퀵조인					확인	0331	0829
		{
			// 빠른 입장 리뉴얼 - 서동권 2011.08.04
			CHECKVALID_CHANNEL; 

			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
				return FALSE; 
			}

			i32Temp = g_pRoomManager->OnQuickJoinRoom(this, m_i32ChannelNum); 

			if( EV_FAILED( i32Temp ) )
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_LOBBY_QUICKJOIN_ROOM_ACK );
				pSendPacket->WriteData( &i32Temp, sizeof(INT32) );				//방에 들어가지 못하는 내용이 들어가 있음 
			}
		}
		break;
	case PROTOCOL_LOBBY_GET_ROOMINFOADD_REQ		:		//080303
		{
			CHECKVALID_CHANNEL;
			pPacket->ReadData(&UTemp, sizeof(UINT32)); 

			//300보다 작은지를 검사합니다. 유저수 만큼 방을 만들었습니다.
			if( UTemp < g_pContextMain->m_ui32ChannelSessionCount ) 
			{
				CRoom * pRoom; 
				pRoom = g_pRoomManager->GetRoom( m_i32ChannelNum, UTemp );
				bSendMessage= TRUE;
				pSendPacket->SetProtocol(PROTOCOL_LOBBY_GET_ROOMINFOADD_ACK);
				pSendPacket->WriteData(&pRoom->m_Info._InfoAdd, sizeof(ROOM_INFO_ADD));
			}
			else
			{
				I3TRACE("Broken Packet"); 
			}
		}
		break; 
	case PROTOCOL_LOBBY_CREATE_TRAINING_REQ:
		{
			// 사용하지 않습니다.
			//CHECKVALID_CHANNEL;

			//if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			//{
			//	_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
			//	return FALSE; 
			//}

			//// 초보채널에서만 사격장 모드를 만들수 있다.
			//if (CHANNEL_TYPE_BEGIN1 == g_pContextMain->m_pChannelType[m_i32ChannelNum]|| CHANNEL_TYPE_BEGIN2 == g_pContextMain->m_pChannelType[m_i32ChannelNum])
			//{
			//	ROOM_INFO_CREATE InfoCreate;
			//	InfoCreate.Reset();

			//	i3String::Copy(InfoCreate.RoomInfoBasic._Title, "초보 채널 사격장");
			//	InfoCreate.RoomInfoBasic._StageID = getFirstStageByType(STAGE_TYPE_DEATHMATCH);
			//	InfoCreate.szRoomPw[0]	= 0;
			//	InfoCreate.RoomInfoBasic._StageMask |= STAGE_FLAG_TRAINING;
			//	InfoCreate.nChannelNum	= m_i32ChannelNum;

			//	Temp = g_pRoomManager->OnCreateRoom(this, &InfoCreate );

			//	if(Temp > SERVER_VALUE_UNKNOWN)
			//	{
			//		SetPosition(GAME_TASK_READY_ROOM); 
			//		m_pRoom		= g_pRoomManager->GetRoom(m_i32ChannelNum, Temp);
			//		m_i32RoomIdx	= Temp;
			//		m_i32SlotIdx	= 0;	
			//		m_bForceRoomLeave = FALSE;
				
			//		g_pModuleMessenger->MessengerSendFriendRoomEnter(m_WorkThreadIdx, this, (UINT8)m_i32ChannelNum, (UINT16)m_i32RoomIdx);
			//		g_pContextMain->SetCreateRoomTime_NC_LOG();
			//	}
			//	else
			//	{
			//		m_i32RoomIdx = SERVER_VALUE_UNKNOWN;
			//		m_i32SlotIdx = SERVER_VALUE_UNKNOWN;
			//	}
			//}
			//else
			//{
			//	Temp = EVENT_ERROR_EVENT_CREATE_TRAINING_WRONG_CHANNEL;
			//}

			//bSendMessage = TRUE;
			//pSendPacket->SetProtocol(PROTOCOL_LOBBY_CREATE_TRAINING_ACK);
			//pSendPacket->WriteData(&Temp, sizeof(INT32) );
			//if(Temp > SERVER_VALUE_UNKNOWN)
			//{
			//	pSendPacket->WriteData(&m_pRoom->m_Info._InfoBasic,	sizeof(ROOM_INFO_BASIC) );
			//	pSendPacket->WriteData(&m_pRoom->m_Info._InfoAdd,		sizeof(ROOM_INFO_ADD) );

			//	// 클랜 서버에 알려줌
			//	g_pModuleClan->ClanSendRoomEnter(m_WorkThreadIdx, this, m_i32RoomIdx );
			//}
		}
		break;
	case PROTOCOL_LOBBY_GM_EXIT_USER_REQ:
		{
			CHECKVALID_CHANNEL;

			INT32 i32SessionIdx = -1;
			UINT8 ui8Nicklength;
			char strNickName[NET_NICK_NAME_SIZE];
			
			if( (FALSE == m_bGMUser) && (FALSE == m_bMODUser) )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}
 
			pPacket->ReadData(&i32SessionIdx, sizeof(INT32));	
			pPacket->ReadData(&ui8Nicklength, sizeof(UINT8));		LENGTH_CHECK( ui8Nicklength, NET_NICK_NAME_SIZE-1 );
			pPacket->ReadData(strNickName, ui8Nicklength);	
			strNickName[ui8Nicklength] = '\0';				

			// 받아온 SessinIdx 와 NickName을 서버에서도 체크한다.
			CUserSession * pUserSession = g_pUserManager->GetSession( i32SessionIdx );
			if( NULL == pUserSession ) break;
			if( 0 != i3String::Compare( pUserSession->GetNickName(), strNickName ) ) break;

			pUserSession->_SendErrorMessage( this->m_WorkThreadIdx, EVENT_ERROR_HACKING_EXIT_USER, PROTOCOL_LOBBY_GM_EXIT_USER_REQ, KILL_REASON_HACK_PACKET );
		}
		break;
 
	case PROTOCOL_LOBBY_ABUSING_POPUP_END_REQ:	//어뷰징(Abusing) 팝업 시간을 다 기다린경우 클라이언트가 보냄.
		{
			CHECKVALID_CHANNEL;

			// 어뷰징 레벨 체크. 어뷰저가 아닌데 이패킷을 보내면 Kick
			if(  ABUSING_LEVEL_NORMAL > m_ui8AbusingLevel || ABUSING_LEVEL_STRONGEST < m_ui8AbusingLevel )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_ABUSING_CHECK_ERROR, PROTOCOL_LOBBY_ABUSING_POPUP_END_REQ, KILL_REASON_HACK_PACKET );
			}


			// 보류했던 Exp와 Point를 적용한다.
			m_UserInfoBasic._exp += m_ui32HoldAccExp;
			m_UserInfoBasic._point += m_ui32HoldAccPoint;

			if( (FALSE == IsGMUser()) && (FALSE == IsMODUser()) )
			{
				// 계급 변화가 있는지 확인
				CheckRankUp( m_WorkThreadIdx );
			}

			// 이부분이 있어야 DB에 저장 됩니다. 
			ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);


			bSendMessage = TRUE;			
			pSendPacket->SetProtocol( PROTOCOL_LOBBY_ABUSING_POPUP_END_ACK );
			pSendPacket->WriteData(&m_ui32HoldAccExp, sizeof(UINT32) );
			pSendPacket->WriteData(&m_ui32HoldAccPoint, sizeof(UINT32) );

			// 전송한 보류 Exp, Point는 초기화
			m_ui32HoldAccExp = 0;
			m_ui32HoldAccPoint = 0;
		}
		break;
 
	case PROTOCOL_LOBBY_VIEW_USER_ITEM_REQ			: // 로비에서 상대방 장비,아이템 보기 요청
		{
			CHECKVALID_CHANNEL;		

			INT32 i32UserSessionIdx;	// 상대방
			pPacket->ReadData(&i32UserSessionIdx, sizeof(INT32));	

			CUserSession * pUserSession = g_pUserManager->GetSession( i32UserSessionIdx );

			if( NULL != pUserSession )
			{
				pUserSession->SendMyItemInfo( this, VIEW_USER_ITEM_INFO_TYPE_LOBBY );
			}
		}
		break;

	case PROTOCOL_LOBBY_SET_BIRTHDAY_REQ			:
		{
			CHECKVALID_CHANNEL;

			if( 0 != m_ui32BirthDay ) 
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE; 
			}
			
			UINT32	ui32BirthDay;

			pPacket->ReadData( &ui32BirthDay,	sizeof(UINT32) );

			// 완료가 되었다면 서버 이동한다는 사실을 중계서버에 알린다.
			if( FALSE == g_pModuleTrans->AuthSendSetBirthDay(	m_WorkThreadIdx, this, ui32BirthDay ) )
			{
				i32Temp = EVENT_ERROR_FAIL;
				bSendMessage= TRUE;
				pSendPacket->SetProtocol( PROTOCOL_LOBBY_SET_BIRTHDAY_ACK );
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
			}
		}
		break;

	default:
		char strTemp[256];
		i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
		I3TRACE( strTemp );
		I3TRACE("UnKnown Packet - Kill User"); 
		isKilled = TRUE; 
		m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
		break;
	}

	return bSendMessage;
}