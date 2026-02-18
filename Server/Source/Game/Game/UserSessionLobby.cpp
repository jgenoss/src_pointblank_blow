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

BOOL	CUserSession::_PacketParsingLobby( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	UINT32		ui32UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_LOBBY_ENTER_REQ		:					//로비로 들어가기	0122	0829
		{
			CHECKVALID_CHANNEL;

			pPacket->ReadData(&m_ui8UserScopeType, sizeof(UINT8));

			if( SERVER_VALUE_UNKNOWN == m_i32ChannelNum )
			{
				// 방에 있다면, 방에서 내보냅니다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID, KILL_REASON_HACK_PACKET, L"ChannelIdx Is NULL" ); 
				return FALSE;
			}

			bSendMessage = TRUE;
			LobbyEnterReq( m_WorkThreadIdx, pSendPacket );	
		}
		break;
	case PROTOCOL_LOBBY_LEAVE_REQ	:					//0829
		{
			if( GAME_TASK_LOBBY > m_i32MainTask || SERVER_VALUE_UNKNOWN == m_i32ChannelNum )
			{
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_WRONG_POSITION_CHANNEL, ProtocolID );  
				return FALSE; 
			}

			if(m_i32RoomIdx != SERVER_VALUE_UNKNOWN)
			{	// 짜르기 않고 무시합니다.
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE;
			}

			//채널에서 나가기 
			g_pUserManager->OnLeaveChannel( this, m_i32ChannelNum);
			_OnLeaveChnnel();
			
			//메시지 보내기 
			i32Temp = 0;
			bSendMessage = TRUE;			
			pSendPacket->SetProtocol( PROTOCOL_LOBBY_LEAVE_ACK );
			pSendPacket->WriteData(&i32Temp, sizeof(INT32) );
		}
		break;
	
	case PROTOCOL_LOBBY_GET_ROOMLIST_REQ		:							//로비에서 방정보 요청하기 확인 0731	0829
		{
			CHECKVALID_LOBBY; 
			m_bRequestRoomList = TRUE;
		}
		break; 
	case PROTOCOL_LOBBY_QUICKJOIN_SETTING_REQ			: // 빠른 입장 설정 요청 - 서동권 2011.08.04
		{

		}
		break;
	case PROTOCOL_LOBBY_QUICKJOIN_ROOM_REQ			: //퀵조인					확인	0331	0829
		{
			// 빠른 입장 리뉴얼 - 서동권 2011.08.04
			CHECKVALID_LOBBY; 

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
			CHECKVALID_LOBBY;
			pPacket->ReadData(&ui32UTemp, sizeof(UINT32)); 

			//300보다 작은지를 검사합니다. 유저수 만큼 방을 만들었습니다.
			if( ui32UTemp < g_pContextMain->m_ui32ChannelSessionCount ) 
			{
				CRoom * pRoom; 
				pRoom = g_pRoomManager->GetRoom( m_i32ChannelNum, ui32UTemp );
				if( NULL == pRoom ) break;
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
			//	InfoCreate.RoomInfoBasic._StageID = getFirstStageByType(STAGE_MODE_DEATHMATCH);
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
			//	g_pModuleClan->ClanSendRoomEnter(m_WorkThreadIdx, this, m_i32RoomIdx, IsMatchMaking() );
			//}
		}
		break;
	case PROTOCOL_LOBBY_GM_EXIT_USER_REQ:
		{
			CHECKVALID_LOBBY;

			INT32 i32SessionIdx = -1;
			UINT8 ui8Nicklength;
			TTCHAR strNickName[NET_NICK_NAME_SIZE];
			
			if( false == CheckAuthLevel() )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}
 
			pPacket->ReadData(&i32SessionIdx,	sizeof(INT32));	
			pPacket->ReadData(&ui8Nicklength,	sizeof(UINT8));		LENGTH_CHECK( ui8Nicklength, NET_NICK_NAME_SIZE-1 );
			pPacket->ReadData(strNickName,		sizeof(TTCHAR) * ui8Nicklength);	
			strNickName[ui8Nicklength] = '\0';				

			// 받아온 SessinIdx 와 NickName을 서버에서도 체크한다.
			CUserSession * pUserSession = g_pUserManager->GetSession( i32SessionIdx );
			if( NULL == pUserSession ) break;
			if( 0 != i3String::Compare( pUserSession->GetNickName(), strNickName ) ) break;

			pUserSession->_SendErrorMessage( this->m_WorkThreadIdx, EVENT_ERROR_GM_KICK_USER, PROTOCOL_LOBBY_GM_EXIT_USER_REQ, KILL_REASON_HACK_PACKET );
		}
		break;
 
	case PROTOCOL_LOBBY_ABUSING_POPUP_END_REQ:	//어뷰징(Abusing) 팝업 시간을 다 기다린경우 클라이언트가 보냄.
		{
			CHECKVALID_LOBBY;

			// 어뷰징 레벨 체크. 어뷰저가 아닌데 이패킷을 보내면 Kick
			if(  ABUSING_LEVEL_NORMAL > m_ui8AbusingLevel || ABUSING_LEVEL_STRONGEST < m_ui8AbusingLevel )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_ABUSING_CHECK_ERROR, PROTOCOL_LOBBY_ABUSING_POPUP_END_REQ, KILL_REASON_HACK_PACKET );
			}


			// 보류했던 Exp와 Point를 적용한다.
			m_UserInfoBasic.m_ui32Exp += m_ui32HoldAccExp;
			m_UserInfoBasic.m_ui32Point += m_ui32HoldAccPoint;

			// 랭크업 체크
			CheckRankUp( m_WorkThreadIdx );

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
			CHECKVALID_LOBBY;		

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

	case PROTOCOL_LOBBY_GM_GET_UID_REQ:
		{
			CHECKVALID_LOBBY

			if( false == CheckAuthLevel() )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			UINT8	ui8UserCount;
			INT32	i32Sessionidx[255];		//Count의 자료형에 따라 배열의 크기를 변경해 줍니다.
			pPacket->ReadData( &ui8UserCount, sizeof(UINT8) );
			pPacket->ReadData( i32Sessionidx, sizeof(INT32) * ui8UserCount );

			pSendPacket->SetProtocol( PROTOCOL_LOBBY_GM_GET_UID_ACK );
			pSendPacket->WriteData(&ui8UserCount, sizeof(INT8));

			
			for( UINT8 i = 0 ; i < ui8UserCount ; ++i )
			{
				CUserSession*	pUserSession = g_pUserManager->GetSession( i32Sessionidx[i] );
				T_UID			UID = 0;
				
				//각 세션인덱스가 유효한 경우 해당 세션의 UID를 보낸다.
				//세션이 유효하지 않은 경우 초기화값인 0을 보냅니다.
				if( NULL != pUserSession )
				{
					UID = pUserSession->m_i64UID;
				}

				pSendPacket->WriteData( &UID, sizeof(T_UID) );
			}

		}
		break;

	default:
		char strTemp[256];
		sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
		I3TRACE( strTemp );
		I3TRACE("UnKnown Packet - Kill User"); 
		bisKilled = TRUE; 
		m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
		break;
	}

	return bSendMessage;
}