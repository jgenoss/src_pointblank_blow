#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleClan.h"
#include "ModuleTrans.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingRoom( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &isKilled )
{
	INT32		i32Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_ROOM_LEAVE_ROOM_REQ			:						//방에서 나가기 완료	확인 0315	0829
		{
			CHECKVALID_ROOM; 
			g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum, m_WorkThreadIdx, ROOM_ENDTYPE_NONE);
			
			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_ROOM_LEAVE_ROOM_ACK);	

			// 클랜서버에 알려준다.
			g_pModuleClan->ClanSendRoomLeave(m_WorkThreadIdx, this );
		}
		break; 
	case PROTOCOL_ROOM_GET_PLAYERINFO_REQ	:		//방에 있는 사람의 정보 보기	확인	0331	0829	도전과제로 확장 가능성 있음 
		{
			CHECKVALID_ROOM;
			pPacket->ReadData(&UTemp , sizeof(UINT32));
			if(UTemp < SLOT_MAX_COUNT )
			{
				m_pRoom->SendPlayerInfo( this, UTemp); //내부에서 패킷을 보내요.
			}
		}
		break; 
	case PROTOCOL_ROOM_CHANGE_ROOM_ALLINFO_REQ:
		{
			CHECKVALID_ROOM; 
			ROOM_INFO_BASIC InfoBasic;
			ROOM_INFO_ADD	InfoAdd; 
			INT8 i8enemyAiCount;
			INT8 i8difficultyLevel;

			pPacket->ReadData(&InfoBasic,			sizeof(ROOM_INFO_BASIC));
			pPacket->ReadData(&InfoAdd,				sizeof(ROOM_INFO_ADD));
			pPacket->ReadData(&i8enemyAiCount,		sizeof(INT8));
			pPacket->ReadData(&i8difficultyLevel,	sizeof(INT8));
 			
			if(EVENT_ERROR_SUCCESS ==  m_pRoom->ChangeRoomInfo(this, &InfoBasic, i8enemyAiCount, i8difficultyLevel))
			{
				m_pRoom->_SendRoomInfo(NULL);
			}
			if(EVENT_ERROR_SUCCESS == m_pRoom->ChangeRoomInfoOption(this, &InfoAdd))
			{
				m_pRoom->_SendRoomInfoOption();
			}
		}
		break;
	case PROTOCOL_ROOM_CHANGE_ROOMINFO_REQ	:		//방정보 변경			확인	0331	내부에서 방정보 확인해야 함.
		{
			CHECKVALID_ROOM; 
			ROOM_INFO_BASIC InfoBasic;
			INT8 i8enemyAiCount;
			INT8 i8difficultyLevel;

			pPacket->ReadData(&InfoBasic,			sizeof(ROOM_INFO_BASIC));			
			pPacket->ReadData(&i8enemyAiCount,		sizeof(INT8));
			pPacket->ReadData(&i8difficultyLevel,	sizeof(INT8));

			if(EVENT_ERROR_SUCCESS ==  m_pRoom->ChangeRoomInfo(this, &InfoBasic, i8enemyAiCount, i8difficultyLevel))
			{
				m_pRoom->_SendRoomInfo(NULL);
			}
		}
		break;
	case PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_REQ:		//방정보 변경하기 
		{
			CHECKVALID_ROOM; 			
			ROOM_INFO_ADD	InfoAdd; 			
			pPacket->ReadData(&InfoAdd,	sizeof(ROOM_INFO_ADD));
			if(EVENT_ERROR_SUCCESS == m_pRoom->ChangeRoomInfoOption(this, &InfoAdd))
			{
				m_pRoom->_SendRoomInfoOption();
			}
		}
		break; 
	case PROTOCOL_ROOM_CHANGE_PASSWD_REQ		:		//패스워드 변경			확인 0331		//널값을 넣어야 할까?????
		{
			CHECKVALID_ROOM; 
            char strNewPasswd[NET_ROOM_PW + 1];
			pPacket->ReadData( strNewPasswd,	NET_ROOM_PW );
			strNewPasswd[NET_ROOM_PW] = 0;
			m_pRoom->ChangePasswd(this, strNewPasswd); 
		}
		break; 
	case PROTOCOL_ROOM_INFO_ENTER_REQ			:		//방에서 정보보기로 이동합니다. 080303 //이름을 좀 변경해야할듯.
		{
			CHECKVALID_CHANNEL; 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveInfo(this, TRUE); 
			}
			m_i32SubTask	= GAME_TASK_SUB_INFO; 
			bSendMessage= TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_ROOM_INFO_ENTER_ACK);
		}
		break;
	case PROTOCOL_ROOM_INFO_LEAVE_REQ			://정보 보기에서 나가기 080303		//이름을 변경해야 할듯.
		{
			CHECKVALID_CHANNEL; 
			if(m_i32RoomIdx > SERVER_VALUE_UNKNOWN)
			{
				m_pRoom->MoveInfo(this, FALSE); 
			}
			bSendMessage= TRUE; 
			m_i32SubTask	= GAME_TASK_SUB_NONE; 
			pSendPacket->SetProtocol(PROTOCOL_ROOM_INFO_LEAVE_ACK);
		}
		break;
	case PROTOCOL_ROOM_CHANGE_SLOT_REQ		:		//슬롯 상태 변경 방장만 가능 확인 0331
		{
			CHECKVALID_ROOM; 
			UINT32	ui32SlotIdx;
			BOOL	bOpen; 
			pPacket->ReadData( &i32Temp,	sizeof( INT32 ) );
			if( i32Temp & 0x10000000)  bOpen = TRUE; 
			else					bOpen = FALSE; 		
			ui32SlotIdx = i32Temp & 0x0000FFFF; 


			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_ROOM_CHANGE_SLOT_ACK); 

			i32Temp = EVENT_ERROR_FAIL; 
			if( ui32SlotIdx < SLOT_MAX_COUNT)
			{
				i32Temp = m_pRoom->ChangeSlot_U(this, ui32SlotIdx, bOpen);
			}

			pSendPacket->WriteData(&i32Temp, sizeof(INT32) ); 
		}
		break;
	case PROTOCOL_ROOM_CHANGE_TEAM_REQ		: //팀변경						확인 0331
		{
			CHECKVALID_ROOM;

			INT32 i32NewTeam;
			pPacket->ReadData( &i32NewTeam,	sizeof( INT32 ) );	

			m_pRoom->ChangeTeam( this, i32NewTeam );
		}
		break;
	case PROTOCOL_ROOM_REQUEST_MAIN_REQ				:	//080309
		{
			CHECKVALID_ROOM;
			m_pRoom->RequestMain(this);					//내부에서 패킷을 보냅니다.
		}
		break;
	case PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_REQ		:	//080309
		{
			CHECKVALID_ROOM;
			m_pRoom->RequestMainChange(this);			//내부에서 패킷을 보냅니다. 
		}
		break;
	case PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_REQ	:	//080309
		{
			CHECKVALID_ROOM;
			UINT32 ui32SlotIdx; 
			pPacket->ReadData(&ui32SlotIdx, sizeof(UINT32));
			m_pRoom->RequestMainChangeSlot(this, ui32SlotIdx); //내부에서 패킷을 보냅니다.
		}
		break;	
	case PROTOCOL_ROOM_CHECK_MAIN_REQ				:	//080309
		{
			CHECKVALID_ROOM;
			i32Temp = m_pRoom->CheckMainChage(this);

			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_ROOM_CHECK_MAIN_ACK);
			pSendPacket->WriteData(&i32Temp, sizeof(INT32));
		}
		break; 
	case PROTOCOL_ROOM_TEAM_CHANGE_REQ				:	//080417
		{
			CHECKVALID_ROOM;	
			m_pRoom->RequestTeamChange(this);
		}
		break;
	case PROTOCOL_ROOM_GET_LOBBY_USER_LIST_REQ	:			//유저 리스트 요청하기 		//0823
		{
			CHECKVALID_ROOM; 			
			g_pUserManager->OnSendLobbyUserList(this, m_i32ChannelNum);	
		}
		break; 
	case PROTOCOL_ROOM_INVITE_LOBBY_USER_REQ		:		//로비에 있는 유저 초대하기 //0823
		{
			CHECKVALID_ROOM;
			UINT32 ui32Count; 			

			pPacket->ReadData(&ui32Count, sizeof(UINT32));
			if(ui32Count > MAX_INVITE_COUNT)
			{
				I3TRACE( "Invite Max Count Error");
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE;
			}
			
			for(UINT32 i = 0; i < ui32Count;i++)
			{
				pPacket->ReadData(&UTemp, sizeof(UINT32));
				if( UTemp < ((UINT32)g_pContextMain->m_i32SessionCount) )
				{
					g_pUserManager->OnInviteLobbyUser(this, m_i32ChannelNum, UTemp, m_pRoom->m_strPasswd); 
				}
			}	
		}
		break;
	case PROTOCOL_ROOM_EQUIPMENT_TEMP_WEAPON_REQ:
		{
			//CHECKVALID_CHANNEL;
		}
		break;
	case PROTOCOL_ROOM_LOADING_START_REQ:
		{
			CHECKVALID_ROOM;

			UINT8	ui8Length;
			char	strMapName[ STAGE_CODE_NAME_COUNT ];
			pPacket->ReadData(&ui8Length,	sizeof(INT8));	LENGTH_CHECK( ui8Length, STAGE_CODE_NAME_COUNT );
			pPacket->ReadData(strMapName,	ui8Length);
			strMapName[ STAGE_CODE_NAME_COUNT-1 ] = '\0';

			if( (SERVER_VALUE_UNKNOWN != m_i32RoomIdx) && m_pRoom)
			{
				if( m_pRoom->CheckLoadingStart_U( strMapName ) )
				{
					m_ui8LoadingStart = LOAD_START;
				}
				else
				{
					_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
					break;
				}
			}
		}
		break;
	case PROTOCOL_ROOM_GM_KICK_USER_REQ:
		{
			CHECKVALID_ROOM;
			INT8 i8Slot;

			if( (FALSE == m_bGMUser) && (FALSE == m_bMODUser) )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			pPacket->ReadData(&i8Slot, sizeof(INT8));

			if( (SERVER_VALUE_UNKNOWN != m_i32RoomIdx) && m_pRoom)
			{
				m_pRoom->GM_KickUser_U( this, i8Slot );
			}
		}
		break;
	case PROTOCOL_ROOM_GM_EXIT_USER_REQ:
		{
			CHECKVALID_ROOM;
			INT8 i8Slot;

			if( (FALSE == m_bGMUser) && (FALSE == m_bMODUser) )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			pPacket->ReadData(&i8Slot, sizeof(INT8));

			if( (SERVER_VALUE_UNKNOWN != m_i32RoomIdx) && m_pRoom)
			{
				m_pRoom->GM_ExitUser_U( this, i8Slot );
			}
		}
		break;
	case PROTOCOL_ROOM_GM_BLOCK_USER_REQ:
		{
			CHECKVALID_ROOM;
			INT8 i8Slot;
			UINT8 ui8Commentlength;
			char strComment[GM_COMMENT_SIZE+1];

			if( (FALSE == m_bGMUser) && (FALSE == m_bMODUser) )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			pPacket->ReadData(&i8Slot, sizeof(INT8));
			pPacket->ReadData(&ui8Commentlength, sizeof(UINT8));
			pPacket->ReadData(strComment, ui8Commentlength);
			strComment[ui8Commentlength] = '\0';

			if( (SERVER_VALUE_UNKNOWN != m_i32RoomIdx) && m_pRoom)
			{
				m_pRoom->GM_BlockUser_U( this, i8Slot, strComment );
			}
		}
		break;
	case PROTOCOL_ROOM_GM_DESTROY_ROOM_REQ:
		{
			CHECKVALID_ROOM;
			
			if( (FALSE == m_bGMUser) && (FALSE == m_bMODUser) )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			if( (SERVER_VALUE_UNKNOWN != m_i32RoomIdx) && m_pRoom)
			{
				m_pRoom->GM_DestroyRoom_U( this );
			}
		}
		break;
	case PROTOCOL_ROOM_TS_EVENT_REQ:
		{
			char strLog[256];
			if(m_UserInfoDaily._bGetItem == TRUE)
			{
				i3String::Format( strLog, 256, "Event Item is already gaining users - UserNick : %s", m_UserInfoBasic._nick );
				g_pModuleLogFile->Write_M_Log(m_WorkThreadIdx, this, strLog);

				break;
			}

			if( FALSE == m_bCompleteTS )
			{
				i3String::Format( strLog, 256, "PROTOCOL_ROOM_TS_EVENT_REQ HACK 1 - UserNick : %s", m_UserInfoBasic._nick );
				g_pModuleLogFile->Write_M_Log(m_WorkThreadIdx, this, strLog);

				break;
			}

			UINT32 ui32GoodsID = 0;

			pPacket->ReadData(&ui32GoodsID, sizeof(UINT32));
			// 선택된 아이템 데이터 읽기

			if ( ui32GoodsID != m_aui32TSGoodsID[0] && ui32GoodsID != m_aui32TSGoodsID[1] )
			{
				i3String::Format( strLog, 256, "PROTOCOL_ROOM_TS_EVENT_REQ HACK 2 - UserNick : %s", m_UserInfoBasic._nick );
				g_pModuleLogFile->Write_M_Log(m_WorkThreadIdx, this, strLog);

				break;
			}

			m_bCompleteTS = FALSE;
		
			SHOP_MATCHING_DATA*	  pMatching = g_pContextMain->GetShop()->GetMatchingAllData();
			
			if(ui32GoodsID != 0)
			{
				UINT32 ui32ItemID = 0;
				UINT32 ui32ItemArg = 0;

				UINT32	ui32MatchingCount	= g_pContextMain->GetShop()->GetMatchingCount();
				for( UINT32 k = 0; k < ui32MatchingCount; ++k )
				{
					if( pMatching[k]._ui32GoodsID != ui32GoodsID) continue;

					ui32ItemID	= pMatching[k]._ui32ItemID;
					ui32ItemArg = pMatching[k]._ui32ItemArg;
					break;
				}	
				if(ui32ItemID != 0 && ui32ItemArg != 0)
				{
					// 유저가 선택한 아이템 인벤토리에 지급
					if(g_pModuleTrans->AuthSendInsertItem(m_WorkThreadIdx, this, ITEM_INSERT_REQUEST_TS_BONUS, ui32ItemID, ui32ItemArg) == TRUE)
					{
						// 아이템 지급이 되면 플레이타임을 -100으로 설정
						m_UserInfoDaily._bGetItem = TRUE;
						// 이벤트
						UINT32 EventDate = ((g_pContextMain->m_i32Year - 2000) * 100000000) + (g_pContextMain->m_i32Month * 1000000) + (g_pContextMain->m_i32Day * 10000) + (g_pContextMain->m_i32TSInitHour * 100);
						if( EventDate != m_UserInfoDaily._EventDate)
						{
							i3mem::FillZero(&m_UserInfoDaily, sizeof(USER_INFO_DAILY));

							m_UserInfoDaily._EventDate = EventDate;
						}
					}
				}
			}
		}
		break;
	default:
		{
			char strTemp[256];
			i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			isKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
		}
		break;
	}

	return bSendMessage;
}