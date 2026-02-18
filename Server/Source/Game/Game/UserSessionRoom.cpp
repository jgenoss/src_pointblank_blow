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

BOOL	CUserSession::_PacketParsingRoom( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	UINT32		ui32UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 

	switch( ProtocolID )
	{
	case PROTOCOL_ROOM_CREATE_REQ		:							//방 만들기 요청 	완료	확인 0315	0829
		{
			CHECKVALID_LOBBY; 

			if( m_i32RoomIdx > SERVER_VALUE_UNKNOWN )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
				return FALSE; 
			}

			if (IsMercenary())
			{
				INT32 TResult = EVENT_ERROR_CLAN_WAR_MERCENAY;

				i3NetworkPacket SendPacket(PROTOCOL_ROOM_CREATE_ACK);
				SendPacket.WriteData(&TResult, sizeof(INT32)); 
				SendPacketMessage(&SendPacket);

				break;
			}
			
			ROOM_INFO_CREATE InfoCreate;
			InfoCreate.Reset();
			
			pPacket->ReadData(&InfoCreate.RoomInfoBasic,				sizeof(ROOM_INFO_BASIC));		//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
			pPacket->ReadData(&InfoCreate.RoomInfoAdd,				sizeof(ROOM_INFO_ADD));			//Room.cpp에서 내용 확인, 내용이 이상해도 내부에서 기본값으로 생성
			pPacket->ReadData(InfoCreate.szRoomPw,					NET_ROOM_PW);					//Room.cpp에서 내용 확인
			pPacket->ReadData(&InfoCreate.enemyAiCount,				sizeof(INT8));
			pPacket->ReadData(&InfoCreate.difficultyLevel,			sizeof(INT8));
			pPacket->ReadData(&InfoCreate.m_ui8CharMainSlotIdx,		sizeof(UINT8));
			InfoCreate.RoomInfoBasic._Title[ NET_ROOM_NAME_SIZE-1 ]	= '\0';
			InfoCreate.RoomInfoAdd._Nick[ NET_NICK_NAME_SIZE-1 ]	= '\0';

			CreateRoom( m_WorkThreadIdx, &InfoCreate );
		}
		break;
	case PROTOCOL_ROOM_JOIN_REQ			:							//방에 들어가기					0331	0829
		{
			CHECKVALID_LOBBY; 

			if( m_i32RoomIdx > SERVER_VALUE_UNKNOWN )
			{
				// 이 경우 연속적인 더블클릭으로 가능할것으로 보여 무시하겠습니다.
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_POSITION, ProtocolID );
				return FALSE; 
			}

			PACKET_ROOM_JOIN_ACK Packet;

			// 매치메이킹 룸은 입장가능해야하므로, g_pRoomManager->OnJoinRoom() 함수밖에서 체크합니다.
			if( IsMercenary() ) 
			{
				Packet.m_TResult = EVENT_ERROR_CLAN_WAR_MERCENAY;
				S2MOSendEvent( &Packet );
				break;
			}

			UINT32			ui32RoomIdx; 
			ROOM_INFO_JOIN	InfoJoin;			
			pPacket->ReadData( &ui32RoomIdx,						sizeof(UINT32) );
			pPacket->ReadData( InfoJoin.szRoomPw,					NET_ROOM_PW);
			pPacket->ReadData( &InfoJoin.cForce,					sizeof(UINT8) );
			pPacket->ReadData( &InfoJoin.m_ui8CharMainSlotIdx,		sizeof(UINT8) );

			i32Temp = g_pRoomManager->OnJoinRoom( m_WorkThreadIdx, this, m_i32ChannelNum, ui32RoomIdx, &InfoJoin );

			if( EV_FAILED( i32Temp ) )
			{
				Packet.m_TResult = i32Temp;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_ROOM_LEAVE_ROOM_REQ			:						//방에서 나가기 완료	확인 0315	0829
		{
			CHECKVALID_ROOM; 
			g_pRoomManager->OnLeaveRoom(this, m_i32ChannelNum, m_WorkThreadIdx, ROOM_ENDTYPE_NONE);
			
			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_ROOM_LEAVE_ROOM_ACK);	

			// 클랜서버에 알려준다.
			g_pModuleClan->ClanSendRoomLeave(m_WorkThreadIdx, this );
			g_pModuleTrans->AuthSendRoomLeave(m_WorkThreadIdx, this );
		}
		break; 
	case PROTOCOL_ROOM_CHARA_SHIFT_POS_REQ:
		{
			PACKET_ROOM_CHARA_SHIFT_POS_ACK Packet;
			Packet.m_TResult	= EVENT_ERROR_SUCCESS;

			UINT8 aui8CharSlotIdx[TEAM_COUNT];

			pPacket->ReadData( aui8CharSlotIdx , sizeof(UINT8) * TEAM_COUNT );

			INT32 i32TeamCount = m_pDesign->GetActiveTeamCount();
			for( INT32 i = 0; i < i32TeamCount ; ++ i )
			{
				if( FALSE == m_MultiSlot.SetTeamSlot( aui8CharSlotIdx[i] ) )
				{
					Packet.m_TResult	= EVENT_ERROR_INVENTORY_LEAVE_MAIN_SLOT;
					break;
				}

				Packet.m_ui8SlotIdx[i] = aui8CharSlotIdx[i];
				ChangeDataBaseInfo(SAVE_DATABASE_CHARA_COMMON);
			}
					
			S2MOSendEvent( &Packet );
		}
		break;
	case PROTOCOL_ROOM_GET_PLAYERINFO_REQ	:		//방에 있는 사람의 정보 보기	확인	0331	0829	도전과제로 확장 가능성 있음 
		{
			CHECKVALID_ROOM;
			pPacket->ReadData(&ui32UTemp , sizeof(UINT32));
			if(ui32UTemp < SLOT_MAX_COUNT )
			{
				m_pRoom->SendPlayerInfo( this, ui32UTemp); //내부에서 패킷을 보내요.
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
			INT32 i32RoomInforResult;
			INT32 i32RoomOptionResult;

			pPacket->ReadData(&InfoBasic,			sizeof(ROOM_INFO_BASIC));
			pPacket->ReadData(&InfoAdd,				sizeof(ROOM_INFO_ADD));
			pPacket->ReadData(&i8enemyAiCount,		sizeof(INT8));
			pPacket->ReadData(&i8difficultyLevel,	sizeof(INT8));
 			

			i32RoomInforResult  = m_pRoom->ChangeRoomInfo(this, &InfoBasic, i8enemyAiCount, i8difficultyLevel, FALSE);
			i32RoomOptionResult = m_pRoom->ChangeRoomInfoOption(this, &InfoAdd, FALSE);

			if (EVENT_ERROR_SUCCESS == i32RoomInforResult) m_pRoom->_SendRoomInfo(NULL);
			if (EVENT_ERROR_SUCCESS == i32RoomOptionResult) m_pRoom->_SendRoomInfoOption();
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

			if(EVENT_ERROR_SUCCESS ==  m_pRoom->ChangeRoomInfo(this, &InfoBasic, i8enemyAiCount, i8difficultyLevel, FALSE ))
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
			if(EVENT_ERROR_SUCCESS == m_pRoom->ChangeRoomInfoOption(this, &InfoAdd, FALSE ))
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
	case PROTOCOL_ROOM_PERSONAL_TEAM_CHANGE_REQ		: //팀변경						확인 0331
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
	case PROTOCOL_ROOM_MASTER_TEAM_CHANGE_REQ		:	//080417
		{
			CHECKVALID_ROOM;
			m_pRoom->RequestTeamChange(this);
		}
		break;
	case PROTOCOL_ROOM_GET_LOBBY_USER_LIST_REQ	:			//유저 리스트 요청하기 		//0823
		{
			CHECKVALID_ROOM;
			UINT8 ui8UserListReq = 0;

			pPacket->ReadData(&ui8UserListReq, sizeof(UINT8));
			if (USER_LIST_REQ_START == ui8UserListReq) m_UpdateUserListPage = 0;

			m_bRequestUserList = TRUE;			
		}
		break; 
	case PROTOCOL_ROOM_INVITE_LOBBY_USER_LIST_REQ:
		{
			CHECKVALID_ROOM;
			g_pUserManager->OnSendInviteLobbyUserList(this, m_i32ChannelNum);
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
				pPacket->ReadData(&ui32UTemp, sizeof(UINT32));
				if( ui32UTemp < ((UINT32)g_pContextMain->m_i32SessionCount) )
				{
					g_pUserManager->OnInviteLobbyUser(this, m_i32ChannelNum, ui32UTemp, m_pRoom->m_strPasswd); 
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
				m_ui8LoadingStart = LOAD_START;
			}
		}
		break;
	case PROTOCOL_ROOM_GM_KICK_USER_REQ:
		{
			CHECKVALID_ROOM;
			INT8 i8Slot;

			if( false == CheckAuthLevel() )
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

			if( false == CheckAuthLevel() )
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
			TTCHAR strComment[GM_COMMENT_SIZE+1];

			if( false == CheckAuthLevel() )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			pPacket->ReadData(&i8Slot,				sizeof(INT8));
			pPacket->ReadData(&ui8Commentlength,	sizeof(UINT8));
			pPacket->ReadData(strComment,			sizeof(TTCHAR) * ui8Commentlength);
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
			
			if( false == CheckAuthLevel() )
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
			// v1.0 용 TS. 사용하지 않음.
		}
		break;
	case PROTOCOL_ROOM_GM_GET_UID_REQ:
		{
			CHECKVALID_ROOM

			if( false == CheckAuthLevel() )
			{	// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			UINT8	ui8UserCount;
			UINT8	aui8SlotIdx[128];		//Count의 자료형에 따라 배열의 크기를 변경해 줍니다.
			pPacket->ReadData( &ui8UserCount, sizeof(UINT8) );
			pPacket->ReadData( aui8SlotIdx, sizeof(UINT8) * ui8UserCount );

			pSendPacket->SetProtocol( PROTOCOL_ROOM_GM_GET_UID_ACK );
			pSendPacket->WriteData(&ui8UserCount, sizeof(INT8));

			for( UINT8 i = 0 ; i < ui8UserCount ; ++i )
			{
				T_UID	UID;

				//슬롯이 비어있거나 닫혀있으면 UID값을 0으로 보낸다.
				if( m_pRoom->m_Info._aSlotInfo[aui8SlotIdx[i]]._State < SLOT_STATE_SHOP )
				{
					UID = 0;
				}
				else
				{//유저가 있는 슬롯이면 해당 유저의 UID를 확인하여 보낸다.
					UID = m_pRoom->m_Info._pSession[aui8SlotIdx[i]]->m_i64UID;
				}

				pSendPacket->WriteData( &UID, sizeof(T_UID) );
			}
		}
		break;
	case PROTOCOL_ROOM_GET_USER_EQUIPMENT_REQ:
		{
			CHECKVALID_ROOM

			UINT8 ui8SlotIdx;
			pPacket->ReadData( &ui8SlotIdx, sizeof(UINT8) );

			// vv3 - check
			PACKET_ROOM_GET_USER_EQUIPMENT_ACK Packet;

			CUserSession* pUserSession = m_pRoom->GetUserSession( ui8SlotIdx );
			if( pUserSession )
			{
				S2MO_CHAR_EQUIP_INFO CharEquipInfo;
				ITEM_INFO pItemInfo[ CHAR_EQUIPMENT_COUNT ];

				pUserSession->m_pDesign->GetMainCharEquipmentAll(&pUserSession->m_MultiSlot, pItemInfo, ui8SlotIdx % TEAM_COUNT);  //m_MultiSlot.GetCharEquipmentAll(i32MainSlot, ItemInfo);

				Packet.m_TResult		= EVENT_ERROR_SUCCESS;
				Packet.m_ui8SlotIdx		= ui8SlotIdx;
				
				for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; i++ )
				{
					Packet.m_TBattleEquipWeapon[ i ]	= pItemInfo[ i ].m_TItemID;
				}
				for( INT32 i = 0 ; i < CHAR_EQUIPMENT_PARTS_COUNT ; i++ )
				{
					Packet.m_TBattleEquipParts[ i ]		= pItemInfo[ CHAR_EQUIPMENT_WEAPON_COUNT+i ].m_TItemID;
				}

				// 팀별 캐릭터 ItemID 셋팅
				for (INT32 i = 0; i < TEAM_COUNT; ++i)
				{
					ITEM_INFO	ItemInfo;
					INT32 i32Slot = pUserSession->m_MultiSlot.GetTeamSlot((TEAM_TYPE)i);
					pUserSession->m_MultiSlot.GetCharEquipment(i32Slot, CHAR_EQUIPMENT_CHARA, &ItemInfo);
					Packet.m_TCharItemID[i] = ItemInfo.m_TItemID;
				}
			}
			else
			{
				Packet.m_TResult	= EVENT_ERROR_NOT_FIND_USER;
			}

			S2MOSendEvent( &Packet );
		}
		break;
	default:
		{
			char strTemp[256];
			sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled		= TRUE; 
			m_ui32KillReason= KILL_REASON_UNKNOWN_PACKET;
		}
		break;
	}

	return bSendMessage;
}