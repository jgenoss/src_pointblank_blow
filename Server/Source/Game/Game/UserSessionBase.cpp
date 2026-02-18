#include "pch.h"
#include "UserSession.h"
#include "DllInjection_AhnHS.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ModuleMessenger.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingBase( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Rv;
	INT32		i32Temp;
	UINT32		ui32UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE;
	//char*		pPacketContents = pPacket->GetFieldBuffer() + sizeof(UINT16);

	UINT64		ui64DXFunc[MAX_USG_DX_FUNC];
	ui64DXFunc[0] = 0;
	ui64DXFunc[1] = 0;

	switch( ProtocolID )
	{
		// 준영 추가
	case PROTOCOL_BASE_LOGOUT_REQ	:
		{
			// 로그 아웃 처리
			m_bCheckLogout = TRUE;
			
			PACKET_BASE_LOGOUT_ACK Send;
			S2MOSendEvent( &Send );
		}
		break;
	case PROTOCOL_BASE_GET_USER_INFO_REQ		:							// 자신의 정보 받기 -- 로그인 후 한번만 받습니다.
		{
			CHECKVALID_LOGIN;

			PACKET_BASE_GET_USER_INFO_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			if( g_pModuleTrans->AuthSendGetUserInfo( m_WorkThreadIdx, this ) )
			{
				m_ui32ATInfo = i3ThreadTimer::GetServerTime();
			}
			else
			{
				PACKET_BASE_GET_USER_INFO_ACK Packet;
				Packet.m_TResult = EVENT_ERROR_EVENT_TRANS;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_BASE_GET_INVEN_INFO_REQ		:
		{
			CHECKVALID_LOGIN;		

			PACKET_BASE_GET_INVEN_INFO_ACK Packet;
			Packet.m_TResult			= EVENT_ERROR_SUCCESS;

			UINT16			ui16InvenCount;
			INVEN_BUFFER	Inventory[MAX_INVEN_COUNT];
			m_Inventory.GetInventory( &ui16InvenCount, Inventory );
			Packet.m_aInventory.SetValue( Inventory, 0, ui16InvenCount );			
			bSendMessage = TRUE;
			Packet.Packing_T( pSendPacket->GetPacketBuffer() );					
		}
		break;
	case PROTOCOL_BASE_GET_SYSTEM_INFO_REQ:
		{
			CHECKVALID_LOGIN;

			PACKET_BASE_GET_SYSTEM_INFO_REQ Recv;
			S2MORecvPacket(&Recv, pPacket->GetPacketBuffer());

			PACKET_BASE_GET_SYSTEM_INFO_ACK Packet;

			g_pContextMain->GetContentsStateFlag(&Packet.m_ContentsFlag);
			Packet.m_bNewRepair					= g_pContextMain->GetContentsVersion( CONTENTS_REPARE_DURABILITY )==REPARE_DURABILITY_VER_2?true:false;			
			Packet.m_aRankUpItem.SetValue( g_pContextMain->m_aRankUpItem, 0, MAX_RANK_COUNT );
			Packet.m_ui8RSEnable				= (UINT8)0;			
			Packet.m_strReportFtpAddress		= g_pContextMain->m_strReportFtpAddress;
			Packet.m_ui16ReportFtpPort			= g_pContextMain->m_ui16ReportFtpPort;

			Packet.m_bGiftEnable				= g_pContextMain->IsContentsEnabled( CONTENTS_GIFT );
			Packet.m_bCashRecharge				= g_pContextMain->IsContentsEnabled( CONTENTS_CASH_RECHARGE );
			
			Packet.m_strBillingURL				= g_pContextMain->m_astrBillingURL[Recv.m_ui8LinkMethod];

			//Packet.m_ui32GameServerCount		= g_pContextMain->m_ui32GameServerCount;
			Packet.m_aGameServerInfo.SetValue( g_pContextMain->m_aGameServerInfo, 0, g_pContextMain->m_ui32GameServerCount );

			Packet.m_ui32DailyGiftGauge.SetValue( g_pContextMain->m_DailyGift.m_ui32PlayTime, 0, DAILY_GIFT_TYPE_MAX );

			//i3mem::Copy( Packet.m_ui8ChannelType, g_pContextMain->m_pChannelType,						sizeof(UINT8)*GAME_CHANNEL_COUNT );
			//Packet.m_ui8ActiveQuestCount		= g_pContextMain->m_ui8ActiveQuestCount;
			Packet.m_pui32ActiveQuest.SetValue( g_pContextMain->m_pui32ActiveQuest, 0, g_pContextMain->m_ui8ActiveQuestCount );

			for(UINT32 i = 0 ; i < QUEST_CARDSET_TYPE_COUNT ; i++)
			{
				if( (Packet.m_pui32ActiveQuest[0] & 1<<i) != 0)
					Packet.m_ui32QuestCardGoodsID[i] = g_pContextMain->m_ui32QuestCardGoodsID[i];
				else
					Packet.m_ui32QuestCardGoodsID[i] = 0;
			}		
						
			Packet.m_ui16UDPPort				= g_pContextMain->m_ui16ClientPort;
			
			//Packet.m_strRankingURL[0] = '\0';
			
			CLAN_MATCH_SEASON	aClanMatchSeason[SEASON_TYPE_COUNT];
			for( INT32 i = 0; i < SEASON_TYPE_COUNT ; ++i )
			{ 
				g_pContextMain->MakePacketClanMatchSeason((SEASON_TYPE)i,&aClanMatchSeason[i]); 
			}
			
			i3mem::Copy( &Packet.m_ClanMatchSeason, &aClanMatchSeason[SEASON_TYPE_ONGOING], sizeof( CLAN_MATCH_SEASON ) );
			
			CLAN_MATCH_SEASON_EXT* pClanMatch = &Packet.m_ClanMatchSeason;
			pClanMatch->m_ui32PreSeasonStartDate	= aClanMatchSeason[SEASON_TYPE_PRE].m_ui32StartDate;
			pClanMatch->m_ui32PreSeasonEndDate		= aClanMatchSeason[SEASON_TYPE_PRE].m_ui32EndDate;
			
			Packet.m_aGiftBuyRanking				= *g_pContextMain->GetGiftBuyRanking();
			
			Packet.m_ui8TagCount = g_pContextMain->m_ui8TagCount;
			Packet.m_ui8TagPriority.SetValue( g_pContextMain->m_ui8TagPriority, 0, SHOP_TAG_COUNT );
			Packet.m_bGiftShopEnable = g_pContextMain->IsContentsEnabled(CONTENTS_GIFTSHOP);
			Packet.m_ui8MultiWeaponCount = g_pContextMain->m_ui8MultiWeaponCount;

			bSendMessage = TRUE;
			Packet.Packing_T( pSendPacket->GetPacketBuffer() );

			PACKET_NOTICE_ACK NoticePacket;
		
			NoticePacket.m_strSlidingNotice = g_pContextMain->m_strSlidingNotice;
			NoticePacket.m_i32SlidingRGB = g_pContextMain->m_i32SlidingRGB;		
			NoticePacket.m_strChatNotice = g_pContextMain->m_strChatNotice;
			NoticePacket.m_i32ChatRGB = g_pContextMain->m_i32ChatRGB;
			S2MOSendEvent(&NoticePacket);

			SendBannerURLList();
			i3NetworkPacket SendPacket(PROTOCOL_BASE_BOOSTEVENT_INFO_ACK);
			UINT8  ui8Enable = 0;
			REAL32 r32Exp = 0;
			REAL32 r32Point = 0;

			g_pContextMain->m_UserEvent.GetRoomBonus(m_ui8LocaleCode, EVENT_BONUS_RANDOM_MAP, 0, &r32Exp, &r32Point);
			
			if (0 < g_pContextMain->m_ui16RandomMapCount)
			{
				ui8Enable = 1;
				SendPacket.WriteData(&ui8Enable, sizeof(UINT8));
				SendPacket.WriteData(&r32Exp, sizeof(REAL32));
				SendPacket.WriteData(&r32Point, sizeof(REAL32));
			}
			else
			{
				SendPacket.WriteData(&ui8Enable, sizeof(UINT8));
			}
		
			SendPacketMessage(&SendPacket);
		}
		break;
	case PROTOCOL_BASE_GET_OPTION_REQ		:							// 게임 옵션 받기 -- 로그인 후 한번만 받습니다.
		{
			CHECKVALID_LOGIN;

			TYPE_RING_IN RingIn;
			RingIn._i64UID			= m_i64UID;
			RingIn._i32SessionIdx	= m_SessionIdx;
			RingIn._ui8Type			= RING_TYPE_OPTION_GET;

			i32Rv = g_pDataBase->RingInputDB( m_WorkThreadIdx, &RingIn );
			if( RINGBUFFER_SUCCESS != i32Rv )
			{
				PACKET_BASE_GET_OPTION_ACK Packet;
				Packet.m_TResult			= i32Rv;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_BASE_USER_ENTER_REQ		:						//0823
		{
			CHECKVALID_CONNECT;

			PACKET_BASE_USER_ENTER_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			SetPosition( GAME_TASK_CHANNEL );

			m_i64UID = Recv.m_TUID;
			Recv.m_strID.GetValue( m_strId, NET_ID_SIZE );

			// 완료가 되었다면 서버 이동한다는 사실을 중계서버에 알린다.
			if( FALSE == g_pModuleTrans->AuthSendServerEnter(	m_WorkThreadIdx, this ) )
			{
				PACKET_BASE_USER_ENTER_ACK Packet;
				Packet.m_TResult = EVENT_ERROR_FAIL;
				S2MOSendEvent( &Packet );
				break;
			}

			g_pModuleClan->LoadMerPenalty( m_WorkThreadIdx, this);

			switch( g_pContextMain->m_i32GameGuard )
			{
			case GAMEGUARD_XIGNCODE:
				{
					g_pXignCode->Connect( m_i64UID, m_SessionIdx, m_ConIp, m_strId );	

					// 임시 로그 확인용. 추후 제거.
					if( m_i64UID == 89375500 || m_i64UID == 89375584 || m_i64UID == 139237906 )
					{
						g_pLog->WriteLogSession( this, L"#### XignCode Connect () /  uid : %I64d \n", m_i64UID );
					}
				}
				break;
			}
		}
		break; 
	case PROTOCOL_BASE_USER_LEAVE_REQ		:						//0823
		{
			CHECKVALID_INFO;

			m_bCheckLogout = TRUE;	// 서버 이동이므로.

			if( 0 < m_ui32ATKillDelay )
			{	// 잘림 대기중에 서버이동하려는 경우 강제로 자릅니다.
				_SendErrorMessage( m_WorkThreadIdx, m_ui32ErrorCode, 0, m_ui32KillReason, m_wstrHackNotice );
				return FALSE; 
			}
			//서버를 이동한다는 메시지를 각 상위 서버에 보냄  
			if( FALSE == g_pModuleTrans->AuthSendServerLeave(	m_WorkThreadIdx, this ) )
			{
				PACKET_BASE_USER_LEAVE_ACK Packet;
				Packet.m_TResult = EVENT_ERROR_FAIL;
				S2MOSendEvent( &Packet );
				
				g_pLog->WriteLogSession( this, L"#### PROTOCOL_BASE_USER_LEAVE_REQ ChangeDBFlage :%d", m_i32ChangeDBFlage );
			}
		}
		break;
	case PROTOCOL_BASE_ENTER_PASS_REQ		:
		{
			PACKET_BASE_ENTER_PASS_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			char strPass[ NET_SERVER_PW ];
			Recv.m_strPass.GetValue( strPass, NET_SERVER_PW );

			i32Rv = EVENT_ERROR_SUCCESS;
			if( 0 != i3String::Compare( g_pContextMain->m_strEnterPass, strPass ) )
			{
				i32Rv = EVENT_ERROR_FAIL;
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, ProtocolID);
			}

			PACKET_BASE_ENTER_PASS_ACK Packet;
			Packet.m_TResult = i32Rv;
			S2MOSendEvent( &Packet );
		}
		break;
	case PROTOCOL_BASE_CREATE_NICK_REQ:
		{
			CHECKVALID_CHANNEL;

			if( 0 != m_UserInfoBasic.m_strNick[0] ) 
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID ); 
				return FALSE; 
			}

			PACKET_BASE_CREATE_NICK_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
			
			TTCHAR strNickName[ NET_NICK_NAME_SIZE ];
			Recv.m_strNickName.GetValue( strNickName, NET_NICK_NAME_SIZE );

			T_RESULT	TResult;
			
			TResult = _CheckUseNickName( strNickName );

			if( EV_SUCCESSED( TResult ) )
			{	// 완료가 되었다면 서버 이동한다는 사실을 중계서버에 알린다.
				if( FALSE == g_pModuleTrans->AuthSendCreateNickName(	m_WorkThreadIdx, this, strNickName ) )
				{
					TResult = EVENT_ERROR_FAIL;
				}
			}

			if( EV_FAILED( TResult ) )
			{
				PACKET_BASE_CREATE_NICK_ACK Packet;
				Packet.m_TResult = TResult;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_BASE_GET_CHANNELLIST_REQ	:							//채널 정보 보내기			0306 0823
		{
			CHECKVALID_INFO;

			PACKET_BASE_GET_CHANNELLIST_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			INT32 i32ServerIdx = Recv.m_i32ServerIdx;
			if( i32ServerIdx < 0 || i32ServerIdx >= (INT32)g_pContextMain->m_ui32GameServerCount )	// 해킹입니다.
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_USER_HACK_PACKET, ProtocolID, KILL_REASON_HACK_PACKET  ); 
				return FALSE;
			}

			g_pUserManager->SendListInfoPaket( i32ServerIdx, this );
		}
		break; 	
	case PROTOCOL_BASE_SELECT_CHANNEL_REQ	:							//채널 선택하기 			0306 0823
		{
			CHECKVALID_INFO; 

			if( m_i32ChannelNum != SERVER_VALUE_UNKNOWN)
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_SAME_REQ, ProtocolID ); 
				return FALSE;
			}

			PACKET_SELECT_CHANNEL_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );
			
			PACKET_SELECT_CHANNEL_ACK Packet;
			Packet.m_TResult = CheckEnterChannel( Recv.m_ui16ChannelIdx );
			if( EV_SUCCESSED( Packet.m_TResult ) )
			{
				Packet.m_TResult = g_pUserManager->OnEnterChannel( this, Recv.m_ui16ChannelIdx );
			}
			else if( EVENT_ERROR_CLAN_WAR_MERCENAY == Packet.m_TResult )
			{
				m_i32SelectChannelNum = Recv.m_ui16ChannelIdx;
				break;	 // 클랜 미가입자가 클랜채널을 선택한 경우, PROTOCOL_CLAN_WAR_REGIST_MERCENARY_ACK 에서 입장 처리 합니다.
			}

			if( EV_SUCCESSED( Packet.m_TResult ) )
			{
				Packet.m_ui16ChannelNum		= Recv.m_ui16ChannelIdx;
				Packet.m_strNormalNotice	= g_pContextMain->m_strNormalNotice;

				m_i32ChannelNum				= Recv.m_ui16ChannelIdx; 
				m_i32LogChannelNum			= ((g_pContextMain->m_i32ServerIdx*GAME_CHANNEL_COUNT) + (m_i32ChannelNum+1));				
			}

			S2MOSendEvent( &Packet );
		}
		break;

	case PROTOCOL_BASE_GET_RECORD_INFO_DB_REQ	:			//080303 받는 패킷과 수정해서 넣어야 함..에러 일때 에러도 처리해야함
		{
			CHECKVALID_INFO;

			T_UID UID;
			pPacket->ReadData(&UID, sizeof(T_UID));

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
			CHECKVALID_INFO;

			USER_INFO_RECORD * pRecord;
			pPacket->ReadData(&ui32UTemp, sizeof(UINT32));
			pRecord = g_pUserManager->OnGetRecord(ui32UTemp);
	
			if(pRecord != NULL)
			{
				bSendMessage = TRUE;
				i32Temp = EVENT_ERROR_SUCCESS; 
				pSendPacket->SetProtocol( PROTOCOL_BASE_GET_RECORD_INFO_SESSION_ACK );
				pSendPacket->WriteData(pRecord, sizeof(USER_INFO_RECORD) );
			}
		}		
		break;
	case PROTOCOL_BASE_CHATTING_REQ			:	//채팅							확인 0331		싸이즈 조정할수있음 INT8 사이즈로.
		{			
			CHECKVALID_LOBBY; 

			//채팅금지 종료시간을 지나지 않을경우 채팅을 무시합니다.
			if (GetChatBlockDate() > i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI())
				break;

			TTCHAR strChat[MAX_CHATTING_COUNT]; 
			UINT16 ui16Type; 
			UINT16 ui16ChatSize;
			pPacket->ReadData( &ui16Type,		sizeof(UINT16));
			pPacket->ReadData( &ui16ChatSize,	sizeof(UINT16));
			if(ui16ChatSize > MAX_CHATTING_COUNT - 1)return FALSE;
			pPacket->ReadData( strChat,			sizeof(TTCHAR) * ui16ChatSize );	
			strChat[MAX_CHATTING_COUNT - 1] = 0; 

			if( m_i32RoomIdx != SERVER_VALUE_UNKNOWN )
			{//방 
				if ( _CheckChat(strChat, ZLOG_CHATTING_TYPE_ROOM, ui16ChatSize, PROTOCOL_BASE_CHATTING_ACK) )
				{
					m_pRoom->Chatting_U( this->m_WorkThreadIdx, this, strChat, ui16Type );
					break;
				}
			}
			else
			{//대기실
				if ( _CheckChat(strChat, ZLOG_CHATTING_TYPE_LOBBY, ui16ChatSize, PROTOCOL_BASE_CHATTING_ACK) )
				{
					g_pUserManager->OnSendLobbyChatting(this, strChat, ui16ChatSize);
					break;
				}
			}
		}
		break; 
	case PROTOCOL_BASE_OPTION_SAVE_REQ					: 
		{
			if (SERVER_VALUE_UNKNOWN == m_i64UID ) break;

			//CHECKVALID_INFO;

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
					
					// 유저 스코프 타입 저장.
					m_ui8UserScopeType = UINT8((RingIn._NetEnvSet.m_GamePlayState & 0x00ff0000) >> 16);
				}
				if( 0x00000002 & RingIn._ui32ChangeFlag )
				{	//Key
					pPacket->ReadData( RingIn._KeyMapArray,		(sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT) );				
				}
				if( 0x00000004 & RingIn._ui32ChangeFlag )
				{	//Macro
					UINT8 ui8MacroSize;
					for(INT32 i = 0; i < MACRO_MAX_COUNT; i++)
					{
						pPacket->ReadData( &ui8MacroSize,		sizeof(UINT8) );		LENGTH_CHECK( ui8MacroSize, MACRO_MAX_LENGHT );
						pPacket->ReadData( RingIn._szMacro[i],	sizeof(TTCHAR) * ui8MacroSize); 
					}
				}
			}

			i32Rv = g_pDataBase->RingInputDB( m_WorkThreadIdx, &RingIn );
			if( RINGBUFFER_SUCCESS != i32Rv )
			{
				bSendMessage = TRUE;

				pSendPacket->SetProtocol( PROTOCOL_BASE_OPTION_SAVE_ACK );
				pSendPacket->WriteData(&i32Rv,		sizeof(INT32) );
			}
		}
		break; 
	case PROTOCOL_BASE_GAMEGUARD_ACK					:		//게임 가드 용 080303
		{
			//CHECKVALID_CONNECT; 

			SC_VERSION	ClientVersion;
			HACK_CHECK_REALTIME	stUSGRealTime;

			pPacket->ReadData(&stUSGRealTime, sizeof(HACK_CHECK_REALTIME)); // AHC_RESULT
			pPacket->ReadData(&ClientVersion, sizeof(SC_VERSION));

			UINT64 ui64RealTimeDX[4] = { 0, };
			ui64RealTimeDX[0] = stUSGRealTime.m_ui64DXFunction[0];
			ui64RealTimeDX[1] = stUSGRealTime.m_ui64DXFunction[1];
			ui64RealTimeDX[2] = stUSGRealTime.m_ui64DXDevice[0];
			ui64RealTimeDX[3] = stUSGRealTime.m_ui64DXDevice[1];
				
			_CheckUlsanGuard(m_WorkThreadIdx, ProtocolID, &stUSGRealTime, ui64RealTimeDX, m_strId);
			

			// 접속한 클라이언트의 버전이 서버와 맞지 않을 경우
			if( FALSE == g_pContextMain->m_ClientVersion.VersionCompare( &ClientVersion ) )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_LOGIN_INVALID_VERSION, ProtocolID, KILL_REASON_VER_ERROR );
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
						m_ui32ATGameGuardRecv	= g_pContextMain->GetServerTime();
						m_ui32ATGameGuardSend	= m_ui32ATGameGuardRecv;
					}
				}
				#endif
				break;
			case GAMEGUARD_HACKSHIELD:
				{
					wchar_t wstrTemp[MAX_PATH];

					//AHNHS_TRANS_BUFFER stResponseBuf; // 클라이언트로부터 수신한 응답 버퍼
					pPacket->ReadData( &m_stResuestBuf.nLength, sizeof( UINT16 ) );
					if( m_stResuestBuf.nLength < ANTICPX_TRANS_BUFFER_MAX )
					{
						pPacket->ReadData( m_stResuestBuf.byBuffer, m_stResuestBuf.nLength );
						// 서동권
						DWORD dwRet = 0; // 세부적인 에러값 - 이전 사용하던 switch 리스트 값들
						DWORD dwRetEx = 0; // ANTICPX_RECOMMAND_CLOSE_SESSION, ANTICPX_RECOMMAND_KEEP_SESSION, ERROR_ANTICPXSVR_VERIFYEX_EXCEPTION
 
						if ( ANTICPX_INVALID_HANDLE_VALUE == m_hClient )
						{
							g_pLog->WriteLogSession( this, L"[ERROR] AhnHS_VerifyResponseEx INVALID HANDLE : %I64d", m_i64UID );
						}
						else
						{
							dwRetEx = g_pAhnHS_VerifyResponseEx( m_hClient, m_stResuestBuf.byBuffer, m_stResuestBuf.nLength, &dwRet );
							if( dwRetEx == ANTICPX_RECOMMAND_CLOSE_SESSION )
							{
								//게임 클라이언트에서 해킹 행위가 감지되었으므로,
								//게임 클라이언트와의 접속을 차단하도록 합니다. 
								i3String::Format( wstrTemp, MAX_PATH, L"HackShield Detect Hacking : %I64d / Ret : %x ", m_i64UID, dwRet );

								// 기존에는 즉시 종료시키지 않고 m_iGameGuardRecvST,m_iGameGuardSendST 시간을 갱신시키지 않아 타임아웃으로 종료되게 하였지만,
								// 안랩측 요청으로 즉시 종료되도록 수정 합니다.
								_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD, wstrTemp );
							}
						}

						m_ui32ATGameGuardRecv	= i3ThreadTimer::GetServerTime();
						m_ui32ATGameGuardSend	= m_ui32ATGameGuardRecv;
					}
					else
					{
						_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD, L"HackShield Buffer Over" );
					}
				}
				break;
			case GAMEGUARD_XTRAP:
				{
					// XTrap_CS_Step3
					char pXTrapBuffer[ XTRAP_SETINFO_PACKETBUFF_SIZE ];
					
					pPacket->ReadData( pXTrapBuffer, XTRAP_SETINFO_PACKETBUFF_SIZE );

					// XTrap 에서 모든걸 처리하기로 했기 떄문에 결과 값으로 체크하는 부분 삭제 합니다.
					XTrap_CS_Step3( m_pXtrapSessionBuf, pXTrapBuffer );
					//UINT32 ui32Res = XTrap_CS_Step3( m_pXtrapSessionBuf, pXTrapBuffer );

					m_ui32ATGameGuardRecv	= i3ThreadTimer::GetServerTime();
					m_ui32ATGameGuardSend	= m_ui32ATGameGuardRecv;

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
					//		char strTemp[ MAX_PATH ];
					//		i3String::Format( strTemp, MAX_PATH, "[ERROR]XTrap_CS_Step3 / res : %x", ui32Res );
					//		_SendErrorMessage( m_WorkThreadIdx, ui32Res, ProtocolID, KILL_REASON_GAME_GUARD );	// 바로 연결 종료하지 X.
					//	}
					//	break;
					//}
				}
				break;
			case GAMEGUARD_GPK:
				break;

			case GAMEGUARD_XIGNCODE:
				{
					char pData[XIGNCODE_PACKET_SIZE];

					pPacket->ReadData( pData, sizeof( char ) * XIGNCODE_PACKET_SIZE );

					g_pXignCode->Recive( m_i64UID, pData, XIGNCODE_PACKET_SIZE );
				}
				break;
			case GAMEGUARD_USG:
				{
					m_ui32ATGameGuardSend = i3ThreadTimer::GetServerTime();
					m_ui32ATGameGuardRecv = m_ui32ATGameGuardSend;
				}
				break;
			}
		}
		break;
	case PROTOCOL_BASE_GAMEGUARD_FAIL_NOACK		:
		{
			// 브라질 X-Trap 에서 문제가 있을시 클라에서 보냄 ( 해킹탐지나 X-Trap 자체 문제 발생시 ) - 서동권
			CHECKVALID_CONNECT;

			_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_GAME_GUARD );

			INT32 i32Result;

			pPacket->ReadData(&i32Result,	sizeof(UINT32));

			g_pLog->WriteLogSession( this, L"[ X-Trap Error ] UID : %I64d, Result : %d", m_i64UID, i32Result );
		}
		break;
	case PROTOCOL_BASE_HEART_BIT_REQ			: 		break;	//080303
	case PROTOCOL_BASE_REFRESH_MEDALSYSTEM_REQ	: 
		{
			CHECKVALID_CONNECT; 
		}		
		break; 
	//임무카드 
	case PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_REQ:		//실행중인 Quest를 종료하고 Chagne가 들어온다.
		{
			CHECKVALID_INFO;

			UINT8	ui8CardSet, ui8Card;
			UINT16	ui16Quest;
			
			pPacket->ReadData(&ui8CardSet,	sizeof(UINT8));			// 카드셋 0,1,2,3
			pPacket->ReadData(&ui8Card,		sizeof(UINT8));			// 카드셋의 카드 0,1,2,3,4,5,6,7,8,9
			pPacket->ReadData(&ui16Quest,	sizeof(UINT16));		// 퀘스트 진행 상태 모두 완료 FFFF

			//변경된 임무카드 세팅을 합니다. 
			if( (ui8CardSet < MAX_CARDSET_PER_USER) && (ui8Card < MAX_CARD_PER_CARDSET) )
			{
				if( (m_QuestIngInfo.m_ActiveIndexUse					!= ui8CardSet) ||		//카드셋이 변경되거나.
					(m_QuestIngInfo.m_pActiveIndexCardSet[ui8CardSet]	!= ui8Card) )			//카드인덱스가 다르면.
				{
					// 변경된 카드로 셋팅
					m_QuestIngInfo.m_ActiveIndexUse						= ui8CardSet;
					m_QuestIngInfo.m_pActiveIndexCardSet[ui8CardSet]		= ui8Card;
					ChangeDataBaseInfo(SAVE_DATABASE_QUEST);
				}
			}
		}
		break; 
	case PROTOCOL_BASE_QUEST_BUY_CARD_SET_REQ	:			//080304		
		{//카드셋 구입하기 
			CHECKVALID_LOBBY;
			CHECKVALID_SHOP;

			UINT8 iCardType; 
			pPacket->ReadData(&iCardType, sizeof(UINT8)); 

			// 할인쿠폰으로 인하여 추가. 추후에 캐쉬 임무카드가 생기면 일반 아이템 구입 부분처럼 할인쿠폰 정보도 패킷으로 받아야 합니다.
			

			i32Temp = EVENT_ERROR_SUCCESS;

			INT8 i8QuestSlotIdx = _FindInsertQuestSlot( iCardType );
			if(	QUEST_CARDSET_TYPE_BUY_START > iCardType )							i32Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( QUEST_CARDSET_TYPE_BUY_END < iCardType )						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( QUEST_CARDSET_TYPE_EVENT == iCardType )						i32Temp = EVENT_ERROR_EVENT_BUY_CARD_PARAMETER_FAIL;
			else if( EV_FAILED( i8QuestSlotIdx ) )									i32Temp = EVENT_ERROR_EVENT_BUY_CARD_UNKOWN_FAIL; 
			else if( QUEST_CARDSET_TYPE_DINO_TUTORIAL == iCardType || QUEST_CARDSET_TYPE_HUMAN_TUTORIAL == iCardType )	// 공룡/인간 튜토리얼 예외 처리
			{
				//if ( (m_UserInfoBasic.m_ui8TutorialIng & (0x10 << (iCardType - QUEST_CARDSET_TYPE_TUTORIAL))) == 0 )
				//{
					// 구입 성공 처리
					g_pContextMain->GetQuest( iCardType, i8QuestSlotIdx, &m_QuestIngInfo, m_aaui8QuestCtxActive[ i8QuestSlotIdx ], m_ui8LocaleCode );
					//m_UserInfoBasic.m_ui8TutorialIng = (m_UserInfoBasic.m_ui8TutorialIng | (0x10 << (iCardType - QUEST_CARDSET_TYPE_TUTORIAL)));
					ChangeDataBaseInfo(SAVE_DATABASE_QUEST | SAVE_DATABASE_BASEINFO );

					bSendMessage = TRUE; 
					pSendPacket->SetProtocol(PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK);
					pSendPacket->WriteData(&i32Temp,							sizeof(INT32));
					pSendPacket->WriteData(&m_UserInfoBasic.m_ui32Point,				sizeof(UINT32)); 
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
				UINT32 ui32GoodsID = MAKE_GOODS_FLAG( ITEM_TYPE_QUESTCARD , iCardType);  

				// 추후에 캐쉬 임무 카드가 추가되면 수정 되어야 됩니다.
				UINT8 ui8BuyType = GOODS_BUY_POINT;

				BUY_BASKET stBasket;
				stBasket.m_ui32GoodsID = ui32GoodsID;
				stBasket.m_ui8BuyType = ui8BuyType;
				stBasket.m_ui32CouponID = 0; 
				stBasket.m_TCouponIdx = 0;

				INT32 i32Result = _BuyGoods( 1, &stBasket );

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

	case PROTOCOL_BASE_QUEST_DELETE_CARD_SET_REQ:		//080309
		{
			CHECKVALID_LOBBY;
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
			pSendPacket->WriteData(&i32Temp,			sizeof(INT32));
			pSendPacket->WriteData(&m_QuestIngInfo,		sizeof(QUESTING_INFO)); 			
			ChangeDataBaseInfo(SAVE_DATABASE_QUEST); 
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
					medalPacket.WriteData(&m_i64UID,			sizeof(T_UID));
					medalPacket.WriteData(&m_ui32Miniature,		sizeof(UINT32));
					medalPacket.WriteData(&m_ui32Insignia,		sizeof(UINT32));
					medalPacket.WriteData(&m_ui32Order,			sizeof(UINT32));
					medalPacket.WriteData(&m_ui32Master,		sizeof(UINT32));
					SendPacketMessage(&medalPacket); 

					// 히어로 호칭 획득 시, 베레모 추가
					UINT32 ui32ItemID = 0;
					switch(ui8userTitleId)
					{
					case 13:	ui32ItemID = BERET_ASSAULT_ITEMID;	break;
					case 19:	ui32ItemID = BERET_RIFLE_ITEMID;	break;
					case 25:	ui32ItemID = BERET_SMG_ITEMID;		break;
					case 34:	ui32ItemID = BERET_HANDGUN_ITEMID;	break;
					case 39:	ui32ItemID = BERET_SHOTGUN_ITEMID;	break;
					default:	// Do nothing					
						break;
					}
					if( 0 < ui32ItemID )
					{
						// 필요한 베레모가 있으면 추가 시켜줍니다.
						INVEN_BUFFER_SET InvenSet;
						InvenSet.ui8ChangeType = ITEM_CHANGE_TYPE_INSERT;
						_ExceptionInsert( ui32ItemID, ITEM_ATTR_TYPE_BASIC, &InvenSet );

						// 변경 아이템 전달
						SendChangeInven( &InvenSet );
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
			ChangeDataBaseInfo(SAVE_DATABASE_QUEST); 
		}
		break;
	case PROTOCOL_BASE_USER_TITLE_EQUIP_REQ:			//081224
		{
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
			INT32 result = EVENT_ERROR_SUCCESS;
			// 보유중인 호칭인가?
			//if ( m_GainUserTitle[userTitleId] == 1)
			if( 1 == GET_USERTITLE( m_ui64GainUserTitle, ui8userTitleId ))
			{
				INT32 i32Categoryidx = g_UserTitleContext[ui8userTitleId]._category;

				if (ui8equipTitleSlotIdx >= m_i32LimitUserTitleCount) result = EVENT_ERROR_TITLE_EQUIP_SLOTIDX;

				for( INT32 i = 0; i < m_i32LimitUserTitleCount; ++i )
				{
					//장착중인 호칭으로는 장착 요청이 올 수 없습니다.
					if( m_aui8EquipUserTitle[i] == ui8userTitleId )
					{
						_SendErrorMessage(m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID); 
						return FALSE;
					}
					
					//장착할 슬롯에 대해서는 중복검사를 생략합니다.
					if( (INT32)ui8equipTitleSlotIdx == i ) continue;
										
					if ( i32Categoryidx == g_UserTitleContext[m_aui8EquipUserTitle[i]]._category )
					{
						result = EVENT_ERROR_TITLE_EQUIP_DUPLICATE_CATEGORY;
						break;
						
					}
				}
				// 장비가 가능한 슬롯인가?
				if (EV_SUCCESSED(result) && ui8equipTitleSlotIdx < m_i32LimitUserTitleCount)
				{
					ITEM_INFO ItemInfo;					

					INT32 i32TeamSlot[TEAM_COUNT];
					i32TeamSlot[TEAM_RED] = m_MultiSlot.GetTeamSlot( TEAM_RED );
					i32TeamSlot[TEAM_BLUE] = m_MultiSlot.GetTeamSlot( TEAM_BLUE );

					m_MultiSlot.GetCharEquipment( i32TeamSlot[TEAM_RED], CHAR_EQUIPMENT_BERET, &ItemInfo );
					
					if ((m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_ASSAULT_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_ASSAULT))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_RIFLE_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SNIPER))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_SMG_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SUBMACHINE))||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_SHOTGUN_TITLE &&ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SHOTGUN))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_HANDGUN_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_HANDGUN)) 	)
					{
						for (INT32 i = 0; i < S2MO_MULTI_SLOT_COUNT; ++i)
						{
							if (FALSE == m_MultiSlot.IsActiveChar(i, MULTI_SLOT_CHECK_USING)) continue;

							m_MultiSlot.SetCharEquipment(i, CHAR_EQUIPMENT_BERET, 0, 0);
						}
						//S2MOSendEvent( &Packet2 );
						ChangeCharaDataBaseInfo( ( (UINT64)0x01 << i32TeamSlot[TEAM_RED] ) | ( (UINT64)0x01 << i32TeamSlot[TEAM_BLUE] ) );
					}

					m_aui8EquipUserTitle[ui8equipTitleSlotIdx] = ui8userTitleId;
					result = EVENT_ERROR_SUCCESS;
				}
			}
		
			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_BASE_USER_TITLE_EQUIP_ACK);
			pSendPacket->WriteData(&result, sizeof(INT32));
			ChangeDataBaseInfo(SAVE_DATABASE_QUEST); 
		}
		break;
	case PROTOCOL_BASE_USER_TITLE_RELEASE_REQ:	//081224
		{
			UINT8 ui8equipTitleSlotIdx;
			UINT8 ui8userTitleId;

			pPacket->ReadData(&ui8equipTitleSlotIdx, sizeof(UINT8));
			pPacket->ReadData(&ui8userTitleId, sizeof(UINT8));


			// 호칭 해제
			INT32 result = EVENT_ERROR_FAIL;

			if (ui8equipTitleSlotIdx < m_i32LimitUserTitleCount)
			{
				ITEM_INFO ItemInfo;

				INT32 i32TeamSlot[TEAM_COUNT];
				i32TeamSlot[TEAM_RED] = m_MultiSlot.GetTeamSlot( TEAM_RED );
				i32TeamSlot[TEAM_BLUE] = m_MultiSlot.GetTeamSlot( TEAM_BLUE );
				
				m_MultiSlot.GetCharEquipment( i32TeamSlot[TEAM_RED], CHAR_EQUIPMENT_BERET, &ItemInfo );
				
				// 베레모 장비중이면 해제
				if ((m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_ASSAULT_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_ASSAULT))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_RIFLE_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SNIPER))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_SMG_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SUBMACHINE))||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_SHOTGUN_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_SHOTGUN))	||
					(m_aui8EquipUserTitle[ui8equipTitleSlotIdx] == BERET_HANDGUN_TITLE && ItemInfo.m_TItemID == MAKE_ITEM_ID( ITEM_TYPE_BERET, CHARACTER_TEAM_NONE, BERET_RES_ID_HANDGUN)) 	)
				{
					for(INT32 i = 0 ; i < S2MO_MULTI_SLOT_COUNT; ++i )
					{
						if (FALSE == m_MultiSlot.IsActiveChar(i, MULTI_SLOT_CHECK_USING)) continue;
						
						m_MultiSlot.SetCharEquipment( i, CHAR_EQUIPMENT_BERET, 0, 0 );
					}						
					ChangeCharaDataBaseInfo( ( (UINT64)0x01 << i32TeamSlot[TEAM_RED] ) | ( (UINT64)0x01 << i32TeamSlot[TEAM_BLUE] ) );
				}

				m_aui8EquipUserTitle[ui8equipTitleSlotIdx] = 0;
				result = EVENT_ERROR_SUCCESS;
			}

			bSendMessage = TRUE;
			pSendPacket->SetProtocol(PROTOCOL_BASE_USER_TITLE_RELEASE_ACK);
			pSendPacket->WriteData(&result, sizeof(INT32));
			ChangeDataBaseInfo(SAVE_DATABASE_QUEST); 
		}
		break;
	case PROTOCOL_BASE_QUEST_FINISH_NOTICE_REQ		: 
		{
			CHECKVALID_ROOM;
			m_pRoom->SetQuestEndFlag( m_i32SlotIdx ); //내부에서 패킷을 보냅니다. 
			ChangeDataBaseInfo(SAVE_DATABASE_QUEST); 
		}
		break; 
	case PROTOCOL_BASE_MEDAL_GET_INFO_REQ:
		{
			CHECKVALID_LOBBY;

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_MEDAL_GET_INFO_ACK);
			pSendPacket->WriteData(&m_i64UID,			sizeof(T_UID) );
			pSendPacket->WriteData(&m_ui32Miniature,	sizeof(UINT32));
			pSendPacket->WriteData(&m_ui32Insignia,		sizeof(UINT32));
			pSendPacket->WriteData(&m_ui32Order,		sizeof(UINT32));
			pSendPacket->WriteData(&m_ui32Master,		sizeof(UINT32));
		}
		break;
	case PROTOCOL_BASE_NOT_USE_UPDATE_SERVER:
		{
			bisKilled 		= TRUE; 
			m_ui32KillReason= KILL_REASON_UNKNOWN_PACKET;

			g_pLog->WriteLogSession( this, L"Not Updater User Login");
		}
		break;
	case PROTOCOL_BASE_HACKING_CHECK_KICK_REQ:
		{
			INT32	i32CheckCode;

			pPacket->ReadData( &i32CheckCode, sizeof(INT32) );

			g_pLog->WriteLogSession( this, L"Client check code - UID: %I64d, CheckCode: %d", m_i64UID, i32CheckCode );

			if( i32CheckCode >= 1000 )
			{
				// User Kick
				bisKilled 		= TRUE; 
				m_ui32KillReason= KILL_REASON_CLIENT_HACK_CHECK;
			}
		}
		break;
	case PROTOCOL_BASE_DAILY_RECORD_REQ: // 당일전적요청
		{
			CHECKVALID_INFO;

			m_UserInfoDaily._match = m_UserInfoDaily._win + m_UserInfoDaily._lose + m_UserInfoDaily._draw;

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_DAILY_RECORD_ACK);
			pSendPacket->WriteData(&m_UserInfoDaily,	sizeof(USER_INFO_DAILY) );
		}
		break;

	case PROTOCOL_BASE_ATTENDANCE_REQ:
		{
			CHECKVALID_INFO;

			UINT32 ui32AttendanceNum;
			UINT8 ui8AttendanceType;

			pPacket->ReadData( &ui32AttendanceNum,		sizeof( UINT32 ) );
			pPacket->ReadData( &ui8AttendanceType,		sizeof( UINT8 ) );

			if( ui8AttendanceType >= ATTENDANCE_TYPE_MAX )
			{
				ATTENDANCE_USER_SVR stAttendance;
				g_pUserManager->GetAttendanceCheck(m_WorkThreadIdx, m_i64UID, EVENT_ERROR_ATTENDANCE_WRONG_TYPE, &stAttendance);
				break;
			}

			g_pModuleTrans->AuthSendAttendanceReq( m_WorkThreadIdx, m_i64UID, ui32AttendanceNum, ui8AttendanceType );
		}
		break;
	case PROTOCOL_BASE_ATTENDANCE_CLEAR_ITEM_REQ:
		{
			CHECKVALID_INFO;

			UINT32 ui32AttendanceNum;
			UINT8 ui8AttendanceItemNum;
			UINT8 ui8AttendanceType;

			pPacket->ReadData( &ui32AttendanceNum,		sizeof( UINT32 ) );
			pPacket->ReadData( &ui8AttendanceItemNum,	sizeof( UINT8 ) );
			pPacket->ReadData( &ui8AttendanceType,		sizeof( UINT8 ) );

			if( ui8AttendanceType >= ATTENDANCE_TYPE_MAX )
			{
				g_pUserManager->GetAttendanceResult( m_WorkThreadIdx, m_i64UID, EVENT_ERROR_ATTENDANCE_WRONG_TYPE );
				break;
			}
			if( ui8AttendanceItemNum >= ATTENDANCE_CHOICE_MAX_COUNT )
			{	// 선택 가능한 아이템 종류를 초과하였나 검사
				g_pUserManager->GetAttendanceResult( m_WorkThreadIdx, m_i64UID, EVENT_ERROR_ATTENDANCE_REWARD_RANGE);
				break;
			}

			g_pModuleTrans->AuthSendAttendanceItemReq( m_WorkThreadIdx, m_i64UID, ui32AttendanceNum, ui8AttendanceItemNum, ui8AttendanceType );
		}
		break;
	case PROTOCOL_BASE_GET_USER_LIST_REQ:
		{
			CHECKVALID_LOBBY;
			m_bRequestUserList	= TRUE;
		}
		break;
	case PROTOCOL_BASE_SERVER_USER_NOT_FIND:
		{	// 고의 적인 코드입니다. 자세한 내용은 김현우에게 물어보세요.
			wchar_t wstrPass[ 32 ];
			pPacket->ReadData( wstrPass,	sizeof(wchar_t)*32 );
			wstrPass[ 31 ] = '\0';

			if( 0 == i3String::Compare( wstrPass, L"rlatjdqorrlagusdnqkfh^%$#@!" ) )
			{
				g_pLog->WriteLog( L"[ERROR] Not Find User. timeout." );

				// 로그 찍을 수 있도록 Sleep 줍니다.
				Sleep( 1000 );

				// 고의 적인 코드입니다. 자세한 내용은 김현우에게 물어보세요.
				char* pTemp = NULL;
				pTemp[0] = 1;
			}
			else
			{
				g_pLog->WriteLog( L"ChServer Fail:%s", wstrPass );
			}
			// 고의 적인 코드입니다. 자세한 내용은 김현우에게 물어보세요.
		}
		break;
	case PROTOCOL_BASE_SERVER_INFO_REQ:
		{
			char strLog[ MAX_PATH ];
			i3String::Format( strLog, MAX_PATH, "Ver-%d.%d.%d.%d, %s, Date-%s %s", VER_GAME, VER_SC_PROTOCOL, VER_DATE, VER_SS_PROTOCOL, BUILD_VER, __DATE__, __TIME__ );
			INT16 i16Size = (INT16)i3String::Length( strLog );

			bSendMessage = TRUE; 
			pSendPacket->SetProtocol(PROTOCOL_BASE_SERVER_INFO_ACK);
			pSendPacket->WriteData( &m_ui8LocaleCode,	sizeof(UINT8) );
			pSendPacket->WriteData( &i16Size,			sizeof(INT16) );
			pSendPacket->WriteData( strLog,				i16Size);
		}
		break;
	case PROTOCOL_BASE_BATTLE_LOG:	// 배틀 종료시 유저가 보내는 패킷. 플레이 시 발사,히트 등 전투에 대한 내용이 들어있다. AutoAim 판단 여부를 위해 추가.
		{
			USER_BATTLE_LOG stLog;

			pPacket->ReadData( &stLog, sizeof(stLog) );

			if( stLog.m_ui8HitPosChk > 2 ||
				stLog.m_i32AimChk >= 50 )
			{
				g_pLog->WriteLogSession( this, L"PROTOCOL_BASE_BATTLE_LOG = KillCnt : %d, FireCnt : %d, DetectCnt : %d, Accuracy : %f,  AimChk : %d, HitPosChk : %d, BattleTime : %d, StageType : %d, UserCnt : %d",
					stLog.m_ui8Kill, stLog.m_i32FireCnt, stLog.m_i32Detect, stLog.m_rAccuracy, stLog.m_i32AimChk, stLog.m_ui8HitPosChk, m_ui32LastPlayTime, m_eLastStageType, m_ui32LastRoomUserCount );
			}
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	// 
	//////////////////////////////////////////////////////////////////////////
	case PROTOCOL_BASE_HEART_BIT_2:
		{
			// 해당 기능은 임시로 막아둡니다.
			break;
		//	if( false == m_bIsRecvHeartBit2 )	{ m_bIsRecvHeartBit2 = true; }
		//#ifdef USE_SERVER_WATCH_CONTENTS
		//	
		//	UINT16 ui16len = MAX_AT_PACKET;
		//	char strData[ MAX_AT_PACKET ];
		//	for( INT32 i = 0 ; i < ui16len ; i++ )
		//	{
		//		strData[i] = (UINT8)rand();
		//	}

		//	bSendMessage = TRUE; 
		//	pSendPacket->SetProtocol(PROTOCOL_BASE_HEART_BIT_2);
		//	pSendPacket->WriteData( &ui16len,			sizeof(UINT16) );
		//	pSendPacket->WriteData( strData,			sizeof(ui16len) );
		//#endif
		}
		break;
	case PROTOCOL_BASE_READY_HEART_BIT_REQ:
		{
		#ifdef USE_SERVER_WATCH_CONTENTS
			GAME_SERVER_VERSION GameVersion;
			GameVersion._vGSVInfo	= VER_WATCH_PACKET_GS;
			GameVersion._LocaleCode = LOCALE_NUMBER;
			GameVersion._vGame		= VER_GAME;
			GameVersion._vBuild		= VER_DATE;
			GameVersion._vSC		= VER_SC_PROTOCOL;
			GameVersion._vSS		= VER_SS_PROTOCOL;
			GameVersion._vUDPMain	= VER_UDP_PROTOCOL_MAIN;
			GameVersion._vUDPSub	= VER_UDP_PROTOCOL_SUB;
			GameVersion._GameGuard	= (UINT8)g_pContextMain->m_i32GameGuard;

			strcpy_s( GameVersion._vDate,		sizeof( GameVersion._vDate ), __DATE__ );
			strcpy_s( GameVersion._vTime,		sizeof( GameVersion._vTime ), __TIME__ );
			strcpy_s( GameVersion._vBuildType,	sizeof( GameVersion._vBuildType ), BUILD_VER );

			i3NetworkPacket SendPacket( PROTOCOL_BASE_READY_HEART_BIT_ACK );
			SendPacket.WriteData( &g_pContextMain->m_ui32GameServerCount,	sizeof(UINT32) );
			SendPacket.WriteData( &GameVersion,								sizeof(GAME_SERVER_VERSION) );

			SendPacketMessage(&SendPacket);
		#endif
		}
		break;
	case PROTOCOL_BASE_GUIDE_COMPLETE_REQ:
		{
			CHECKVALID_CHANNEL;

			UINT16	ui16GuideEventIdx;
			pPacket->ReadData( &ui16GuideEventIdx,	sizeof(UINT16));

			// 1. 가이드 이벤트 인덱스 범위 체크.
			if( 0 > ui16GuideEventIdx || GUIDE_EVENT_MAX <= ui16GuideEventIdx ) 
			{
				break;
			}
			
			// 2. 이미 받았는지 체크
			UINT64 ui64CheckFlag = (UINT64)0x1 << ui16GuideEventIdx;
			if( m_UserInfoBasic.m_ui64GuideComplete &  ui64CheckFlag   ) 
			{
				break;
			}

			// 포인트 지급.
			UINT32 ui32GetPoint = g_aui32GuidePoint[ ui16GuideEventIdx ];
			m_UserInfoBasic.m_ui32Point += ui32GetPoint;

			// Log 기록.
			INT32 i32GetRoute = GET_POINT_ROUTE_GUIDE;
			g_pModuleContextNC->LogSendMessage( m_WorkThreadIdx, LOG_MESSAGE_U_GET_POINT, this, &ui32GetPoint, &i32GetRoute );

			// Guide complete Update
			m_UserInfoBasic.m_ui64GuideComplete |= ui64CheckFlag;

			// DB 저장.
			ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO );

			// Send
			PACKET_BASE_GUIDE_COMPLETE_ACK Packet;
			Packet.m_TResult			= EVENT_ERROR_SUCCESS;
			Packet.m_ui16GuideEventIdx	= ui16GuideEventIdx;
			Packet.m_ui64GuideComplete	= m_UserInfoBasic.m_ui64GuideComplete;
			Packet.m_ui32GetPoint		= ui32GetPoint;
			Packet.m_ui32Point			= m_UserInfoBasic.m_ui32Point;
			S2MOSendEvent( &Packet );
		}
		break;
	
	case PROTOCOL_BASE_GET_USER_INFO_LOBBY_REQ:
		{
			CHECKVALID_LOBBY;
			
			PACKET_BASE_GET_USER_DETAIL_INFO_ACK Packet;

			INT32 i32SessionIdx;
			pPacket->ReadData( &i32SessionIdx, sizeof(INT32) );

			CUserSession * pUserSession = g_pUserManager->GetSession( i32SessionIdx );
			if( pUserSession )
			{				
				Packet.m_TResult				= EVENT_ERROR_SUCCESS;				
				_SetUserDetailInfo( &Packet.m_UserDetailInfo, pUserSession );
				
			}
			else
			{
				Packet.m_TResult				= EVENT_ERROR_FAIL;
				i3mem::FillZero( &Packet.m_UserDetailInfo, sizeof(S2_USER_DETAIL_INFO) );
			}

			S2MOSendEvent( &Packet );		
		}
		break;
	case PROTOCOL_BASE_GET_USER_INFO_ROOM_REQ:
		{
			CHECKVALID_ROOM
			
			PACKET_BASE_GET_USER_DETAIL_INFO_ACK Packet;

			INT32 i32SlotIdx;
			pPacket->ReadData( &i32SlotIdx, sizeof(INT32) );

			CUserSession * pUserSession = m_pRoom->GetUserSession( i32SlotIdx );
			if( pUserSession )
			{
				Packet.m_TResult				= EVENT_ERROR_SUCCESS;
				_SetUserDetailInfo( &Packet.m_UserDetailInfo, pUserSession );
			}
			else
			{			
				Packet.m_TResult				= EVENT_ERROR_FAIL;
				i3mem::FillZero( &Packet.m_UserDetailInfo, sizeof(S2_USER_DETAIL_INFO) );
			}

			S2MOSendEvent( &Packet );
		}
		break;

	case PROTOCOL_BASE_GET_USER_INFO_NICK_REQ:
		{
			CHECKVALID_LOBBY;

			TTCHAR strNickname[ NET_NICK_NAME_SIZE ];
			pPacket->ReadData( strNickname, sizeof(TTCHAR) * NET_NICK_NAME_SIZE );

			strNickname[ NET_NICK_NAME_SIZE - 1 ] = '\0';

			if( FALSE == g_pModuleTrans->AuthSendBasicUserInfoNick( m_WorkThreadIdx, this, strNickname ) )
			{
				PACKET_BASE_GET_USER_DETAIL_INFO_ACK Packet;
				Packet.m_TResult				= EVENT_ERROR_FAIL;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	
	case PROTOCOL_BASE_GET_USER_INFO_UID_REQ:
		{
			CHECKVALID_LOBBY;

			T_UID TUID;
			pPacket->ReadData( &TUID, sizeof(T_UID) );

			// 유저의 기본 정보 요청
			if( FALSE == g_pModuleTrans->AuthSendBasicUserInfoUID( m_WorkThreadIdx, this, TUID ) )
			{
				PACKET_BASE_GET_USER_DETAIL_INFO_ACK Packet;
				Packet.m_TResult				= EVENT_ERROR_FAIL;
				S2MOSendEvent( &Packet );
			}
		}
		break;
	case PROTOCOL_BASE_GET_UID_LOBBY_REQ:
		{
			CHECKVALID_LOBBY;

			if( FALSE == CheckAuthLevel() )
			{
				// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			INT32 i32SessionIdx;
			pPacket->ReadData( &i32SessionIdx,		sizeof(INT32) );

			PACKET_BASE_GET_UID_LOBBY_ACK Packet;
			Packet.m_TResult		= EVENT_ERROR_FAIL;

			CUserSession * pUserSession = g_pUserManager->GetSession( i32SessionIdx );
			if( pUserSession )
			{
				Packet.m_TResult	= EVENT_ERROR_SUCCESS;
				Packet.m_TUID		= pUserSession->GetUID();
			}
			S2MOSendEvent( &Packet );
		}
		break;
	case PROTOCOL_BASE_GET_UID_ROOM_REQ:
		{
			CHECKVALID_ROOM;

			if( FALSE == CheckAuthLevel() )
			{
				// GM이나 MOD가 아니면 게임 종료시킨다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			UINT8 ui8SlotIdx;
			pPacket->ReadData( &ui8SlotIdx,		sizeof(UINT8) );

			PACKET_BASE_GET_UID_ROOM_ACK Packet;
			Packet.m_TResult		= EVENT_ERROR_FAIL;
			if( SLOT_MAX_COUNT > ui8SlotIdx )
			{
				CUserSession * pUserSession = m_pRoom->GetSession( ui8SlotIdx );

				if( pUserSession )
				{
					Packet.m_TResult	= EVENT_ERROR_SUCCESS;
					Packet.m_TUID		= pUserSession->GetUID();
				}
			}
			S2MOSendEvent( &Packet );
		}
		break;

	case PROTOCOL_COMMUNITY_USER_INVITED_REQ:
		{
			CHECKVALID_ROOM;
			PACKET_COMMUNITY_USER_INVITED_REQ	PacketReq;			
			pPacket->ReadData(&PacketReq,		sizeof(PACKET_COMMUNITY_USER_INVITED_REQ));
			PacketReq.m_SessionIdx				= m_SessionIdx;
			PacketReq.m_i64UID					= m_i64UID;
			g_pModuleTrans->CommunitySendInviteUser( m_WorkThreadIdx, &PacketReq);
		}
		break;

	case PROTOCOL_BASE_GET_USER_SUBTASK_REQ:
		{
			PACKET_BASE_GET_USER_SUBTASK_REQ Recv;
			S2MORecvPacket( &Recv, pPacket->GetPacketBuffer() );

			PACKET_BASE_GET_USER_SUBTASK_ACK Packet;
			Packet.m_TResult		= EVENT_ERROR_SUCCESS;
			Packet.m_i32SessionIdx	= Recv.m_i32SessionIdx;

			CUserSession * pUserSession = g_pUserManager->GetSession( Recv.m_i32SessionIdx );
			if( pUserSession )
			{
				Packet.m_ui8SubTask = (UINT8)pUserSession->GetSubTask();
			}
			else 
			{
				Packet.m_TResult		= EVENT_ERROR_FIND_USER_NOT_FOUND;
			}
			S2MOSendEvent( &Packet );
		}
		break;
	case PROTOCOL_BASE_CLIENT_END_TYPE_REQ:
		{
			CHECKVALID_DBINDEX;

			pPacket->ReadData(m_strEndReason, MAX_CLIENT_END_REASON);
			m_strEndReason[MAX_CLIENT_END_REASON - 1] = '\0';
		}
		break;
	case PROTOCOL_BASE_MAP_LIST_REQ:
		{
			PACKET_BASE_MAP_RULELIST_ACK RuleListAck;

			RuleListAck.m_aRuleData.SetValue( g_pContextMain->m_aRuleData, 0, g_pContextMain->m_ui8RuleTotalCount );
			S2MOSendEvent( &RuleListAck );
			
			UINT16	ui16CurItemCount	= 0;
			UINT16	ui16StartIdx		= 0;
			UINT16	ui16RemainCount		= g_pContextMain->m_ui16StageMatchingCount;
			bool	bSendContinue		= true;

			while (bSendContinue)
			{
				PACKET_BASE_MAP_MATCHINGLIST_ACK MatchingListAck;

				ui16StartIdx += ui16CurItemCount;
				if (ui16RemainCount > STAGE_MATCHING_SEND_MAX ) 
				{
					ui16CurItemCount = STAGE_MATCHING_SEND_MAX;
					MatchingListAck.m_bLastMatchingList = false;
				}
				else
				{
					ui16CurItemCount = ui16RemainCount;
					bSendContinue = false;
					MatchingListAck.m_bLastMatchingList = true;
				}
				ui16RemainCount -= ui16CurItemCount;
								
				MatchingListAck.m_ui16StageMatchingCount	= g_pContextMain->m_ui16StageMatchingCount;
				MatchingListAck.m_ui16StartIdx				= ui16StartIdx;
				MatchingListAck.m_aStageMatching.SetValue( &g_pContextMain->m_aStageMatching[ui16StartIdx], 0, ui16CurItemCount );
				S2MOSendEvent(&MatchingListAck);

			}

			CHECKVALID_LOGIN;
		}
		break;
	case PROTOCOL_BASE_URL_LIST_REQ:
		{
			UINT8 ui8URLVersion = 0;
			pPacket->ReadData(&ui8URLVersion, sizeof(UINT8));
			bSendMessage = TRUE;
			if (g_pContextMain->m_ui8URLVersion != ui8URLVersion)
			{
				pSendPacket->SetProtocol(PROTOCOL_BASE_URL_LIST_ACK);
				pSendPacket->WriteData(&g_pContextMain->m_ui8URLVersion, sizeof(UINT8));
				pSendPacket->WriteData(&g_pContextMain->m_ui8URLListCount, sizeof(UINT8));

				UINT16		ui16Len = 0;
				URL_LIST	*pulURLList = nullptr;

				for (INT32 i = 0; i < g_pContextMain->m_ui8URLListCount; i++)
				{
					pulURLList = &g_pContextMain->m_ulURLList[i];
					ui16Len = (UINT16)(i3String::Length(pulURLList->m_strURL));
					pSendPacket->WriteData(&ui16Len, sizeof(UINT16));
					pSendPacket->WriteData(&pulURLList->m_i32Type, sizeof(INT32));
					pSendPacket->WriteData(&pulURLList->m_i32Data, sizeof(INT32));
					pSendPacket->WriteData(pulURLList->m_strURL, sizeof(wchar_t) *ui16Len);
				}
			}
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	default:
			char strTemp[256];
			sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled 		= TRUE; 
			m_ui32KillReason= KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}