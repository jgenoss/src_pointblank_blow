#include "pch.h"
#include "UserSession.h"
#include "Room.h"
#include "ModuleClan.h"
#include "ModuleTrans.h"

BOOL	CUserSession::_PacketParsingCheat(i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL& bIsKilled)
{
	if( !CheckSpecialAbility( SA_CHEAT ) ) return FALSE;

	BOOL		bSendMessage = FALSE;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID(); 

	switch( ProtocolID )
	{
	case PROTOCOL_CHEAT_MEDAL_COMMAND_REQ:	// 메달 Command. :
		{ 
			CHECKVALID_NON_BATTLE;	// 배틀중이면 리턴.

			PACKET_MEDAL_COMMAND_REQ	PacketReq;

			pPacket->ReadData( &PacketReq,	sizeof(PACKET_MEDAL_COMMAND_REQ) );		

			PACKET_MEDAL_COMMAND_ACK	PacketAck;
			PacketAck.m_i32Rv = EVENT_ERROR_SUCCESS;

			switch ( PacketReq.m_MedalCommand.m_ui8CommandType )
			{
			case MEDAL_COMMAND_TYPE_CHANGE_MEDALSET: // 메달 셋트 교체.
				{
					MEDAL_SET_TYPE eSetType = (MEDAL_SET_TYPE)PacketReq.m_MedalCommand.m_ui8MedalType; 
				
					MQF_MEDAL_SET_INFO* pSetInfo = g_pContextMain->GetMedalSet( PacketReq.m_MedalCommand.m_ui16Idx );
					if( NULL == pSetInfo ) return FALSE;

					// 이벤트 셋트는 활성화 여부도 체크
					if( eSetType == MEDAL_SET_TYPE_EVENT && ! g_pContextMain->IsEnableEventMedalSet(pSetInfo->m_ui16Idx ) ) return FALSE;
					
					RefreshCurMedalSet( (UINT8)FindCurMedalSetSlot(eSetType), eSetType, PacketReq.m_MedalCommand.m_ui16Idx , MEDAL_SET_GET_TYPE_REFRESH );	 
					return FALSE; // ACK 줄필요 없습니다.
				}
				break;
			case MEDAL_COMMAND_TYPE_COUNT:	// 메달 카운트 증가
				{
					switch ( PacketReq.m_MedalCommand.m_ui8MedalType )
					{
					case MEDAL_TYPE_NORMAL:	// 일반 메달
						{
							MEDAL_GT* pUserMedal;
							// 유저가 메달을 소지 했는지.
							pUserMedal = GetMedal_UserDataAll(PacketReq.m_MedalCommand.m_ui16Idx) ;
							if( NULL == pUserMedal )	// 메달 찾기 실패
							{
								return FALSE;
							}
							pUserMedal->m_Medal.m_ui16Count = PacketReq.m_MedalCommand.m_ui16Count;

							pUserMedal->m_ui8IsSave = 1;
							ChangeDataBaseInfo(SAVE_DATABASE_MEDAL); 
						}
						break;

					case MEDAL_TYPE_SET:	// 세트.
						{
							MQF_MEDAL_SET_INFO* pMQFMedalSet =  g_pContextMain->GetMedalSet_ByMedalIdx( PacketReq.m_MedalCommand.m_ui16Idx );
							if( NULL == pMQFMedalSet )	// 메달 셋트 찾기 실패.
							{
								return FALSE;
							}		

							// 유저의 진행중인 메달 셋트에서 검색.
							CUR_MEDAL_SET*	pUserCurMedalSet = GetCurMedalSet_UserData(pMQFMedalSet->m_ui16Idx) ;
							if( NULL == pUserCurMedalSet )		// 검색 실패
							{
								return FALSE;
							}

							// 몇번째 메달인지 검색
							UINT8 ui8ArrIdx = g_pContextMain->GetSetMedalArrIdx_ByMedalIdx( PacketReq.m_MedalCommand.m_ui16Idx);
							if( -1 == ui8ArrIdx )	// 몇번째 메달인지 검색 실패
							{
								return FALSE;
							}

							pUserCurMedalSet->m_ui16Count[ui8ArrIdx-1] = PacketReq.m_MedalCommand.m_ui16Count;
							ChangeDataBaseInfo( SAVE_DATABASE_CUR_MEDAL_SET );
						}
						break;

					default:
						return FALSE;

					}

					// 처리된 결과 ACK 전송
					bSendMessage = TRUE;
					i3mem::Copy( &PacketAck.m_MedalCommand, &PacketReq.m_MedalCommand, sizeof( MEDAL_COMMAND ) );

					pSendPacket->SetProtocol( PROTOCOL_CHEAT_MEDAL_COMMAND_ACK );
					pSendPacket->WriteData(&PacketAck,	sizeof(PACKET_MEDAL_COMMAND_ACK) );
				}
				break;
			default :
				return FALSE; 
			}
		}
		break;
	// 킬 카운트 증가 커맨드
	case PROTOCOL_CHEAT_INCREASE_KILL_COUNT_REQ:
		{
			CHECKVALID_LOBBY;

			INT32 i32IncKillCount;
			UINT32 ui32WeaponID;

			pPacket->ReadData( &i32IncKillCount, sizeof( INT32 ) );
			pPacket->ReadData( &ui32WeaponID, sizeof( UINT32 ) );

			if ( GAME_TASK_BATTLE == GetPosition() )
				m_pRoom->IncreaseKillCount( m_WorkThreadIdx, m_i32SlotIdx, i32IncKillCount, ui32WeaponID );
		}
		break;
	// TS이벤트 시간 단축 커맨드
	case PROTOCOL_CHEAT_REDUCE_TS_EVENT_REQ:
		{
			CHECKVALID_LOBBY;

			INT32 i32DecTSMinute;

			pPacket->ReadData( &i32DecTSMinute, sizeof( INT32 ) );

			switch( GetPosition() )
			{
			case GAME_TASK_LOBBY:
			case GAME_TASK_READY_ROOM:
				{
					DaliyPlayReward( m_WorkThreadIdx, TRUE, i32DecTSMinute*60 );
				
					bSendMessage = TRUE;
					pSendPacket->SetProtocol( PROTOCOL_CHEAT_REDUCE_TS_EVENT_ACK );
					pSendPacket->WriteData( &m_UserInfoDaily, sizeof(USER_INFO_DAILY) );
				}
				break;
			}
		}
		break;
	// 1인 인게임 플레이 가능 커맨드
	case PROTOCOL_CHEAT_PLAY_SOLO_REQ:
		{
			CHECKVALID_LOBBY;

			if ( GAME_TASK_READY_ROOM == GetPosition() )
			{
				m_pRoom->EnableSoloPlay();

				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CHEAT_PLAY_SOLO_ACK );
			}
		}
		break;
	// 캐릭터 이동
	case PROTOCOL_CHEAT_CHARACTER_TELEPORT_REQ:
		{
			CHECKVALID_LOBBY;

			INT32 i32SlotIdx;
			REAL32 r32pos[3];

			pPacket->ReadData( &i32SlotIdx, sizeof( INT32 ) );
			pPacket->ReadData( r32pos, sizeof( REAL32 )*3 ); 
			
			if ( GAME_TASK_BATTLE == GetPosition() )
				m_pRoom->TeleportCharacter( m_WorkThreadIdx, m_i32SlotIdx, i32SlotIdx, r32pos );
		}
		break;
	// 게임오브젝트에 데미지주기
	case PROTOCOL_CHEAT_DAMAGE_GAME_OBJECT_REQ:
		{
			CHECKVALID_LOBBY;

			INT32 i32TeamIdx;
			INT32 i32Damage;

			pPacket->ReadData( &i32TeamIdx, sizeof( INT32 ) );
			pPacket->ReadData( &i32Damage, sizeof( INT32 ) );
			
			if ( GAME_TASK_BATTLE == GetPosition() )
				m_pRoom->DamageToObject( m_WorkThreadIdx, m_i32SlotIdx, i32TeamIdx, i32Damage );
		}
		break;
	// 핵탐지 OFF
	case PROTOCOL_CHEAT_DETECT_HACK_OFF_REQ:
		{
			CHECKVALID_LOBBY;

			m_ui8QAAutoFlag ^= QA_AUTO_HACK_DETECT_OFF;
		}
		break;
	// 일정시간이후에 GM Pause 걸기
	case PROTOCOL_CHEAT_TIMER_GM_PAUSE_REQ:
		{
			CHECKVALID_LOBBY;

			UINT32 ui32Second;

			pPacket->ReadData( &ui32Second, sizeof( UINT32 ) );
			if ( ui32Second == 0 || ui32Second > 10000 ) ui32Second = 5;
			
			if ( GAME_TASK_BATTLE == GetPosition() )
				m_pRoom->TimerGMPause( m_WorkThreadIdx, ui32Second );
		}
		break;

	case PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECK_HAVE_MATCH_TEAM;
 
			if( FALSE == g_pModuleClan->SendClanCommandMatchTeamPoint( m_WorkThreadIdx,  this  ) )
			{
				INT32 i32MatchingPoint = 0;
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CHEAT_CLAN_WAR_MATCHING_POINT_ACK );
				pSendPacket->WriteData( &i32MatchingPoint, sizeof(INT32) );
			}			
		}
		break;
	case PROTOCOL_CHEAT_CLAN_WAR_RESULT_REQ:
		{
			CHECKVALID_ROOM;
			CHECK_HAVE_MATCH_TEAM;

			if( !m_pRoom ) break;

			T_MatchingID	TMatchingID = m_pRoom->GetClanMatchingID();

			TEAM_TYPE	eWinTeam;
			pPacket->ReadData( &eWinTeam, sizeof( TEAM_TYPE ) );
 
			if( FALSE == g_pModuleClan->SendClanCommandMatchResult( m_WorkThreadIdx, this,  eWinTeam, TMatchingID   ) )
			{
				double	ResultClanPoint[TEAM_COUNT] = {0.f,};	
				 
				bSendMessage = TRUE;
				pSendPacket->SetProtocol( PROTOCOL_CHEAT_CLAN_WAR_RESULT_ACK );
				pSendPacket->WriteData( &eWinTeam,			sizeof(TEAM_TYPE) );
				pSendPacket->WriteData( ResultClanPoint,	sizeof(double)*TEAM_COUNT );
			}			 
		}
		break;
	case PROTOCOL_CHEAT_ITEM_AUTH_REQ:
		{
			CHECKVALID_CHANNEL;
			CHECKVALID_NON_BATTLE;

			INT32 i32Temp = EVENT_ERROR_FAIL;
			if (false == CheckAuthLevel(AUTH_LEVEL::AUTH_LEVEL_GM))
			{
				i32Temp = EVENT_ERROR_FAIL;
			}
			else
			{
				T_ItemDBIdx TWareDBIdx = 0;
				TLoopCnt	TLoopCount = 10;
				UINT8		ui8CharaSlotIdx = 0xFF;

				pPacket->ReadData(&TWareDBIdx, sizeof(T_ItemDBIdx));
				pPacket->ReadData(&TLoopCount, sizeof(TLoopCnt));

				i32Temp = EVENT_ERROR_SUCCESS;
				INVEN_BUFFER* pBuffer = GetItemByWareDBIdx(TWareDBIdx);
				if (nullptr == pBuffer)									i32Temp = EVENT_ERROR_FAIL;
				else
				{
					if (ITEM_ATTR_TYPE_BUY != pBuffer->_ui8ItemType)	i32Temp = EVENT_ERROR_FAIL;
					else if (0 == pBuffer->_ui32ItemArg)				i32Temp = EVENT_ERROR_FAIL;
					else if (0 == pBuffer->_ui32ItemID)					i32Temp = EVENT_ERROR_FAIL;
					else if (TLoopCount < GM_CHEAT_RANDOMBOX_MIN)		i32Temp = EVENT_ERROR_FAIL;
					else if (GM_CHEAT_RANDOMBOX_MAX < TLoopCount)		i32Temp = EVENT_ERROR_FAIL;
					// 아이템 개수 부족
					else if (false == CheckExistItemInvenByItemID(pBuffer->_ui32ItemID))
						i32Temp = EVENT_ERROR_RS_START_FAIL_NOT_ENOUGH_ITEM;
					else if (pBuffer->_ui32ItemArg < TLoopCount)		i32Temp = EVENT_ERROR_RS_START_FAIL_NOT_ENOUGH_ITEM;	
				}

				if (EV_SUCCESSED(i32Temp))
				{
					i32Temp = _SetItemAbilityAuth(pBuffer);
					if (EV_SUCCESSED(i32Temp))
					{
						INT32 ui32ItemType = GET_ITEM_TYPE(pBuffer->_ui32ItemID);
						UINT16 ui16InvenCount = m_Inventory.GetInventoryCount();
						switch (ui32ItemType)
						{
							// 랜덤박스는 개수제만 존재합니다.
						case ITEM_TYPE_WRAP_COUNT:
							if (MAX_INVEN_COUNT - MAX_SHOP_GOODS_ITEM_COUNT < ui16InvenCount)
								i32Temp = EVENT_ERROR_EVENT_BUY_GOODS_INVENTORY_FULL;
							break;
						default:
							// 개수제가 아니면 오류를 리턴합니다.
							i32Temp = EVENT_ERROR_FAIL;
							break;
						}
					}

					if (EV_SUCCESSED(i32Temp))
					{
						if (FALSE == g_pModuleTrans->AuthSendAuthCountItem(m_WorkThreadIdx, this, TWareDBIdx, 1, TLoopCount, ui8CharaSlotIdx, pBuffer->_ui32ItemID))
							i32Temp = EVENT_ERROR_FAIL;
					}
				}

			} // GM 여부 체크

			if (EV_FAILED(i32Temp))
			{
				bSendMessage = TRUE;
				pSendPacket->SetProtocol(PROTOCOL_CHEAT_ITEM_AUTH_ACK);
				pSendPacket->WriteData(&i32Temp, sizeof(INT32));
			}
		}
		break;
	default:
		return FALSE; 
	}

	return bSendMessage;
}