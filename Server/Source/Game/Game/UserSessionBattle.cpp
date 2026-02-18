#include "pch.h"
#include "UserSession.h"
#include "RoomManager.h"
#include "UdpHolePunching.h"
// #include "ModuleLogFile.h"
#include "DataBase.h"
#include "Usermanager.h"
#include "ServerContext.h"
#include "Room.h"
#include "ModuleTrans.h"
#include "ModuleContextNc.h"

BOOL	CUserSession::_PacketParsingBattle( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	static INT8 i8Fail		= -1;
	PROTOCOL	ProtocolID	= pPacket->GetProtocolID();	
	BOOL		bSendMessage= FALSE; 
	INT32		i32Temp;
	UINT32		ui32UTemp;

	switch( ProtocolID )
	{ 
	case PROTOCOL_BATTLE_CHEAT_MESSAGE_REQ :
		break;
	case PROTOCOL_BATTLE_TIMERSYNC_REQ		:								//081224
		{//방에 있다면, 클라이언트의 한 프레임으로 패킷이 날아오는 경우가 있습니다. CHECKVALID_ROOM으로 처리하지 못함
			CHECKVALID_ROOM_GM_PAUSE;

			INT32	i32GameTime; 
			REAL32	r32Speed;
			UINT8	ui8RoundCount;
			UINT8	ui8Ping;
			UINT32	ui32HackType;
			UINT16	ui16PingTime = 0;

			pPacket->ReadData(&i32GameTime,		sizeof(INT32));
			pPacket->ReadData(&r32Speed,		sizeof(REAL32));
			pPacket->ReadData(&ui8RoundCount,	sizeof(UINT8));
			pPacket->ReadData(&ui8Ping,			sizeof(UINT8));
			pPacket->ReadData(&ui32HackType,	sizeof(UINT8));
			pPacket->ReadData(&ui16PingTime,	sizeof(UINT16));		// 1000 = 1초

			BOOL	bCheck = FALSE;
			wchar_t	wstrTemp[MAX_PATH];
			wstrTemp[0] = '\0';

			if( m_ui32ATKillDelay == 0 && !( m_ui8QAAutoFlag & QA_AUTO_HACK_DETECT_OFF ) )
			{
				if( !m_pRoom->CheckGameTime(this, i32GameTime, ui8Ping, ui16PingTime, ui8RoundCount) )
				{
					bCheck			= TRUE;

					//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_HACKING_EXIT_USER, 0, KILL_REASON_HACK_CLIENT );
					i3String::Format( wstrTemp, MAX_PATH, L"TIMERSYNC Time ( UID:%I64d, Time:%d, Speed:%f, RoundCount:%d, ServerRoundCount:%d, ServerRed : %d, ServerBlue : %d )",
					m_i64UID, i32GameTime, r32Speed, ui8RoundCount, m_pRoom->m_Info._InfoAdd._NowRoundCount, m_pRoom->m_Info._aui16TotalRoundCount[TEAM_RED], m_pRoom->m_Info._aui16TotalRoundCount[TEAM_BLUE] );
				}

				// Speed 해킹 유저 체크 스피드가 11.f 이상이면 자릅니다.
				// 초기화 값으로 날라오는것도 확인하기 위해서. 
				if( (0.9f > r32Speed) || (11.f < r32Speed) )
				{
					bCheck			= TRUE;

					//i3String::Format( strTemp, MAX_PATH, "CLIENT:%d , RandTime:%d", iHackType, ui8RandTime );
					i3String::Format( wstrTemp, MAX_PATH, L"TIMERSYNC Speed ( UID:%I64d, Speed:%f, Time:%d )", m_i64UID, r32Speed, i32GameTime );
				}
			

				if( bCheck )
				{
#if defined (I3_DEBUG)
					m_pRoom->SendNotifyHackUser( m_i32SlotIdx, HACK_CHECK_TYPE_CLIENT, m_WorkThreadIdx, ui32HackType, m_ui8LocaleCode);
					g_pLog->WriteLogSession( this, L"%s, CLIENT:%d", wstrTemp, ui32HackType );
#else
					i3String::Format( wstrTemp, MAX_PATH, L"%s, CLIENT:%d", wstrTemp, ui32HackType );
					//g_pLog->WriteLogSession( this, wstrTemp );
					//DisconnectUserDelay( ((UINT32)EVENT_ERROR_HACKING_USER) | ui32HackType, KILL_REASON_HACK_CLIENT, wstrTemp );
#endif
					// 이미 위쪽에서 SendErrorMessage 를 호출하게 되어있으므로 
					//bSendMessage	= TRUE;
					//pSendPacket->SetProtocol( PROTOCOL_BATTLE_TIMERSYNC_ACK );
				}
			}

			m_ui32RTBattleCheck = m_pRoom->GetRoomBattleTime();
		}
		break;
	case PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_REQ:						//081224
		{
			CHECKVALID_ROOM;
			m_pRoom->ChangeDifficultyLevel(this);
		}
		break;
	case PROTOCOL_BATTLE_READYBATTLE_REQ		:		//전투 준비 로딩시작		Room에서 패킷을 보내요.	0829
		{
			CHECKVALID_ROOM;

			bool		bIsObserverMode = FALSE;
			T_ItemDBIdx	TItemDBIdx;

			pPacket->ReadData( &bIsObserverMode,	sizeof(bool) );				// 옵저버 모드 설정 여부.
			pPacket->ReadData( &TItemDBIdx,			sizeof(T_ItemDBIdx) );		// 배틀 시작시 아이템 사용

			if ( !CheckAuthLevel() && true == bIsObserverMode  ) 
			{	// GM이 아닌데 옵저버 모드를 설정했다면 kick.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			if( m_pRoom->IsTutorialMode() )
			{	// 튜토리얼 모드는 무조건 시작 합니다.
				m_pRoom->ReadyBattle_U(this);
				m_ui32BattleStartInvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
				break;
			}

			if( CheckAuthLevel() )
			{
				m_pRoom->SetObserverMode( m_i32SlotIdx, bIsObserverMode );	
			} 
			else 
			{
				if ( g_pContextMain->CheckAASBlockON( AAS_BLOCK_READY_START ) && CheckAAS() )
				{
					PACKET_BATTLE_READYBATTLE_ACK Packet;
					Packet.m_TResult = EVENT_ERROR_AAS_READY;
					S2MOSendEvent( &Packet );
					break;
				}
			}

			m_ui16UsedItemCount	= 0;			// 개수제 사용무기 체크 카운트
			i3mem::FillZero( m_aUsedItemList,		sizeof(USED_ITEM_LIST)*MAX_INVEN_COUNT );
			i3mem::FillZero( m_aui8WeaponUseCount,	sizeof(UINT8)*CHAR_EQUIPMENT_WEAPON_COUNT );
			i3mem::FillZero( m_aui8CharaUseCount,	sizeof(UINT8)*CHAR_EQUIPMENT_PARTS_COUNT );

			UINT32 ui32InvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();

			INVEN_BUFFER aWeaponInven[CHAR_EQUIPMENT_WEAPON_COUNT];
			INVEN_BUFFER aCharaInven[CHAR_EQUIPMENT_PARTS_COUNT];

			ITEM_INFO aCharEquipment[ CHAR_EQUIPMENT_COUNT ];

			INT32 i32Team = m_pRoom->GetNowTeam(m_i32SlotIdx);

			m_pDesign->GetMainCharEquipmentAll(&m_MultiSlot, aCharEquipment, i32Team);
			for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; i++ )
			{
				m_aTBattleEquipWeapon[i].m_TItemDBIdx	= aCharEquipment[i].m_TItemDBIdx;
				m_aTBattleEquipWeapon[i].m_TItemID		= aCharEquipment[i].m_TItemID;
			}

			INT32 i32PartsCount;
			// vv3 - check
			for( i32PartsCount = 0 ; i32PartsCount < CHAR_EQUIPMENT_PARTS_COUNT ; i32PartsCount++ )
			{
				m_aTBattleEquipParts[i32PartsCount].m_TItemDBIdx	= aCharEquipment[i32PartsCount+CHAR_EQUIPMENT_WEAPON_COUNT].m_TItemDBIdx;
				m_aTBattleEquipParts[i32PartsCount].m_TItemID		= aCharEquipment[i32PartsCount+CHAR_EQUIPMENT_WEAPON_COUNT].m_TItemID; 
			}

			ITEM_INFO aCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
			m_MultiSlot.GetCommonEquipmentAll( aCommonEquips );

			if( m_pRoom->IsDinoUser(m_i32SlotIdx) )
			{
				m_aTBattleEquipParts[CHAR_EQUIPMENT_PARTS_CHARA].m_TItemDBIdx	= aCommonEquips[CHAR_EQUIPMENT_COMMON_DINO].m_TItemDBIdx;
				m_aTBattleEquipParts[CHAR_EQUIPMENT_PARTS_CHARA].m_TItemID		= aCommonEquips[CHAR_EQUIPMENT_COMMON_DINO].m_TItemID;
			}
			else
			{
				m_aTBattleEquipParts[CHAR_EQUIPMENT_PARTS_CHARA].m_TItemDBIdx	= aCharEquipment[CHAR_EQUIPMENT_CHARA].m_TItemDBIdx;	// 클라와 맞춥니다.
				m_aTBattleEquipParts[CHAR_EQUIPMENT_PARTS_CHARA].m_TItemID		= aCharEquipment[CHAR_EQUIPMENT_CHARA].m_TItemID;
			}

			INT32 i32CheckSkipFlag = 0;

			if( EV_SUCCESSED(_CheckWeaponEquip( FALSE, m_pRoom->GetInfo()->_WeaponFlag, m_aTBattleEquipWeapon, aWeaponInven, ui32InvenTime )) &&
				EV_SUCCESSED(_CheckCharEquip( FALSE, (UINT32)i32Team, m_aTBattleEquipParts, aCharaInven, ui32InvenTime, i32CheckSkipFlag )) )
			{	// 착용 중인 무기 검사, 케릭터 검사 
				m_pRoom->ReadyBattle_U(this);
				m_ui32BattleStartInvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
			}
			else 
			{
				PACKET_BATTLE_READYBATTLE_ACK Packet;
				Packet.m_TResult = EVENT_ERROR_EVENT_READY_WEAPON_EQUIP;
				S2MOSendEvent( &Packet );
			}

		}
		break;
	case PROTOCOL_BATTLE_PRESTARTBATTLE_REQ	:		//로딩후 홀펀칭 시작	0829
		{
			CHECKVALID_ROOM;
						
			INT32 i32HackCheck = 0;

			UINT32 ui32Stage;

			pPacket->ReadData( &ui32Stage, sizeof(UINT32) );

			if( LOAD_START == m_ui8LoadingStart )
			{
				if( ui32Stage == m_pRoom->m_Info._InfoBasic._StageID )
				{	
					// 로드된 스테이지와 현재 스테이지가 같은지 확인
					m_pRoom->PreStartBattle( this );//내부에서 패킷을 보냄
					break;
				}
				
				// 스테이지가 다른 스테이지입니다.
				i32HackCheck = 1;
			}

			if( 0 == i32HackCheck )
			{
				if( ui32Stage == m_pRoom->m_Info._InfoBasic._StageID)
				{
					switch( m_pRoom->m_Info._InfoBasic._State )
					{
					case ROOM_STATE_BATTLE_RESULT	:
					case ROOM_STATE_BATTLE_END	:
					case ROOM_STATE_READY		: 
						// 로드된 스테이지와 현재 스테이지가 같은지 확인
						m_pRoom->PreStartBattle( this );//내부에서 패킷을 보냄
						return FALSE;
					default:
						i32HackCheck = 2;
						break;
					}
				}
				else
				{	// 스테이지가 다른 스테이지입니다.
					i32HackCheck = 3;
				}				 
			}

			wchar_t wstrLog[MAX_STRING_COUNT];
			i3String::Format( wstrLog, MAX_STRING_COUNT, L"[PROTOCOL_BATTLE_PRESTARTBATTLE_REQ] i32HackTpye:%d", i32HackCheck );
			_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, ProtocolID, KILL_REASON_HACK_PACKET, wstrLog );

		}
		break;
	case PROTOCOL_BATTLE_STARTBATTLE_REQ		:		//전투 시작			0829
		{
			CHECKVALID_ROOM;
			
			m_i32RespawnCount = 0;
			m_pRoom->StartBattle_U( this ); //내부에서 패킷을 보냄 

			UINT32 ui32ValentineChocolateID;
			UINT32 ui32ketupatID;
			
			// 발렌타인 이벤트- Vallentine Chocolate 인증
			ui32ValentineChocolateID	= 1800101;
			ui32ketupatID				= 1800091;

			INVEN_BUFFER* pInven = GetItemByItemID( ui32ValentineChocolateID );
			if( pInven )
			{
				_SetUsedItem( pInven->_TItemWareDBIdx, ui32ValentineChocolateID, USED_ITEM_TYPE_ONE, 1, 0 );
			}
			else
			{	// 케투팻
				pInven = GetItemByItemID( ui32ketupatID );

				if( pInven )
					_SetUsedItem( pInven->_TItemWareDBIdx, ui32ketupatID, USED_ITEM_TYPE_ONE, 1, 0 );
			}


			// 레디버튼 누르고 난뒤 장착하고 있는 아이템의 기간이 만료 될수 있으므로 INVEN_CHECK_DEL_PERIOD_NO_EQUIP로 호출합니다.
			CheckInventory_MAU( m_WorkThreadIdx, INVEN_CHECK_DEL_PERIOD_NO_EQUIP );

			SetMerPenaltyFlag(false);
		}
		break; 
	case PROTOCOL_BATTLE_GIVEUPBATTLE_REQ		:		//전투 포기			0829
		{
			CHECKVALID_ROOM_GM_PAUSE;
			BOOL bFreeMove = FALSE;

			// 전장 이동권을 사용했는지를 체크
			i32Temp = EVENT_ERROR_SUCCESS;
			T_ItemDBIdx TItemDBIdx;
			pPacket->ReadData(&TItemDBIdx, sizeof(T_ItemDBIdx));

			INVEN_BUFFER* pInven = GetItemByWareDBIdx( TItemDBIdx );
			if( pInven )
			{
				// 아이템 인증
				g_pModuleTrans->AuthSendAuthItem(m_WorkThreadIdx, this, pInven->_TItemWareDBIdx, 1 );

				// 전장 이동권 사용
				bFreeMove = TRUE;
			}
			else 
			{
				I3TRACE( "AUTH FAIL : Item Inventory Error\n");
			}

//#if defined (LOCALE_INDONESIA)	// 춘절 이벤트 적용(2월 1일 ~ 2월 22일)
//			if( 1102220000 > g_pContextMain->GetStandDate() )
//			{
//				bFreeMove = TRUE;
//			}
//#endif

			// 만약 전장 이동권이 들어간다면, 여기서 아이템 인증이 들어갑니다.
			if( EV_FAILED( m_pRoom->GiveUpBattle_U( m_WorkThreadIdx, this, bFreeMove ) ) )
			{
				bSendMessage = TRUE;

				i32Temp = EVENT_ERROR_FAIL;
				pSendPacket->SetProtocol( PROTOCOL_BATTLE_GIVEUPBATTLE_ACK );
				pSendPacket->WriteData( &i32Temp,	sizeof(INT32) );
			}
		}
		break; 
		//확인	0331	이정보를 확인해야 합니다. 확인하지 않으면..같은방 모든 클라이언트 모두 끝장.
	case PROTOCOL_BATTLE_RESPAWN_REQ			:		//리스폰 
		{
			CHECKVALID_ROOM_GM_PAUSE;

			if( !m_pRoom )
			{
				bisKilled = TRUE;
				m_ui32KillReason= KILL_REASON_HACK_PACKET;  
				g_pLog->WriteLogSession( this, L"[RECV]PROTOCOL_BATTLE_RESPAWN_REQ[UID:%I64d,Invalid Room]", m_i64UID ); 
				break;
			}

			if( m_pRoom->IsGMPause() )
			{
				UINT32 ui32ATValue = i3ThreadTimer::GetServerTime();
				if ( ui32ATValue < m_ui32ATCanRespawnPacket || m_ui32ATCanRespawnPacket + 3 < ui32ATValue )
					return FALSE;
			}

#ifdef USE_RESPAWN_SKIP
			if( m_pRoom->m_Info._i32MiStateType != MISSION_STATE_BATTLE ) break;
#endif
			
			
			if( m_pRoom->IsTutorialMode() )
			{
				m_pRoom->ReSpawnBattle_U( this ); 
				break;
			}

			ITEM_INFO					TBattleEquipWeapon[ CHAR_EQUIPMENT_WEAPON_COUNT ];
			ITEM_INFO					TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_COUNT ];

			// vv3 - check
			UINT8 ui8WeaponFlag;
			pPacket->ReadData( TBattleEquipWeapon,	sizeof(ITEM_INFO)*CHAR_EQUIPMENT_WEAPON_COUNT );
			pPacket->ReadData( TBattleEquipParts,	sizeof(ITEM_INFO)*CHAR_EQUIPMENT_PARTS_COUNT);
			pPacket->ReadData( &ui8WeaponFlag,		sizeof(UINT8));

			// 내구도 사용 여부 체크
			CheckEndurance();

			INT32 i32CheckSkipWeaponFlag	= 0;
			INT32 i32CheckSkipCharFlag = 0; 

			// vv3 - check. 추후제거 
			//i32CheckSkipCharFlag |= (0x01<<CHAR_EQUIPMENT_PARTS_CHARA);

			// 최초 리스폰 이후부터 한번 검사한 장비에 대해서는 더 이상 검사 하지 않습니다.
			if( 0 < m_i32RespawnCount )
			{
				for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; ++i )
				{
					if( TBattleEquipWeapon[i].m_TItemDBIdx == m_aTBattleEquipWeapon[i].m_TItemDBIdx )	i32CheckSkipWeaponFlag |= (0x01<<i);
				}
				for( INT32 k = 0 ; k < CHAR_EQUIPMENT_PARTS_COUNT ; ++k )
				{
					if( TBattleEquipParts[k].m_TItemDBIdx == m_aTBattleEquipParts[k].m_TItemDBIdx )	i32CheckSkipCharFlag |= (0x01<<k);
				}
			}

			// 해킹 체크
			INVEN_BUFFER aWeaponInven[CHAR_EQUIPMENT_WEAPON_COUNT];
			INVEN_BUFFER aCharaInven[CHAR_EQUIPMENT_PARTS_COUNT];

			INT32 i32AtkDefChange = m_pRoom->m_pRoomMode->GetAtkDefChange() ? 1 : 0;

			INT32 i32WeaponCheck = _CheckWeaponEquip( TRUE, m_pRoom->GetInfo()->_WeaponFlag, TBattleEquipWeapon, aWeaponInven, m_ui32BattleStartInvenTime, i32CheckSkipWeaponFlag );
			INT32 i32CharCheck = _CheckCharEquip( TRUE, ((m_i32SlotIdx + i32AtkDefChange) % TEAM_COUNT), TBattleEquipParts, aCharaInven, m_ui32BattleStartInvenTime, i32CheckSkipCharFlag );
			BOOL bRHFlag = m_pRoom->CheckRespawnHacking( this );

			if( EV_FAILED(i32WeaponCheck) || EV_FAILED(i32CharCheck) || !bRHFlag || ui8WeaponFlag != m_pRoom->m_Info._InfoBasic._WeaponFlag )
			{
				wchar_t wstrLog[MAX_STRING_COUNT];
				i3String::Format( wstrLog, MAX_STRING_COUNT, L"[PROTOCOL_BATTLE_RESPAWN_REQ] WeaponCheck:%d/CharaCheck:%d/RespCheck:%d/UWF:%d/RWF:%d", 
					i32WeaponCheck, i32CharCheck, bRHFlag, ui8WeaponFlag, m_pRoom->m_Info._InfoBasic._WeaponFlag );
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, ProtocolID, KILL_REASON_HACK_PACKET, wstrLog );

				break;
			}

			// 공룡이 사용되는 모드에서 공룡이 바뀌면 데디에 알려야함
			if( ( m_pRoom->m_Info._i32StageType == STAGE_MODE_ESCAPE || m_pRoom->m_Info._i32StageType == STAGE_MODE_CROSSCOUNT ) )
			{
				// vv3 - check. 추후수정. m_TItemID 말고 m_TItemDBIdx로 변경 예정
				if( TBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ].m_TItemDBIdx != m_aTBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ].m_TItemDBIdx ) 
				{
					for( INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++ )
					{
						// 해당 유저가 일반 공룡이 아니면 패스
						if( m_pRoom->m_Info._MiDinoInfo._uiRaptorSlotIdx[i] != m_i32SlotIdx ) continue; 
						m_pRoom->m_Info._abDinoStateUpdate[m_i32SlotIdx] = true;

						// vv3 - check. 추후 주석 처리된 부분으로 변경 예정 
						m_pRoom->m_Info._aui32DInoCharInfo[m_i32SlotIdx] = aCharaInven[CHAR_EQUIPMENT_PARTS_CHARA]._ui32ItemID;
					}
				}
			}

			_SetWeaponEquip( aWeaponInven );
			_SetCharEquip( aCharaInven );

			m_pRoom->ReSpawnBattle_U( this ); 
			m_i32RespawnCount++;
		}
		break;

	case PROTOCOL_BATTLE_RESPAWN_FOR_AI_REQ:				//081224
		{
			CHECKVALID_ROOM;

			INT32 i32SlotIdx;
			pPacket->ReadData(&i32SlotIdx,	sizeof(INT32));
			INT32 i32Ret = m_pRoom->ReSpawnBattleForAi_U( this, i32SlotIdx );
			if( i32Ret < 0 )
			{
				wchar_t wstrLog[MAX_STRING_COUNT];
				i3String::Format( wstrLog, MAX_STRING_COUNT, L"[PROTOCOL_BATTLE_RESPAWN_FOR_AI_REQ] i32Ret:%d", i32Ret );
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_EVENT, ProtocolID, KILL_REASON_HACK_PACKET, wstrLog );
			}
		}
		break;
	case PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ:
		{
			CHECKVALID_BATTLE;

			ITEM_INFO stItemInfo;
			pPacket->ReadData( &stItemInfo,	sizeof(ITEM_INFO) );

			_EquipmentInBattle( &stItemInfo );
		}
		break;
	//case PROTOCOL_BATTLE_PICKUP_WEAPON_REQ		: //삭제 처리 해야합니다. 이번에 함.
		//	이정보를 확인해야 합니다. 확인하지 않으면..같은방 모든 클라이언트 모두 끝장.
		// 차후 UDP로 버그가 많아지면 처리합니다. 
		//CHECKVALID_ROOM; 
		//{			
		//	UINT32 SlotNum; 
		//	UINT32 Weapon; 
		//	m_pRoom->PickupWeapon_U( this, SlotNum, Weapon );
		//}
	//	break; 
	case PROTOCOL_BATTLE_DEATH_REQ				:		//게임중 사망			확인	0331		
		{
			CHECKVALID_ROOM;

			DEATH_INFO_HEADER	KillHeader;
			DEATH_INFO_CHARA	aData[MAX_KILLUSER_COUNT];
			UINT8				ui8RoundNum = 0;

			if( m_pRoom->GetBattleType() == BATTLE_SERVER_TYPE_DEDICATION )
			{
				if( !m_pRoom->IsChallengeMode() &&
					!m_pRoom->IsTutorialMode() )
				{
					// 이곳은 챌린지 모드일 경우만 클라이언트에서 보내는 프로토콜이므로 챌린지 모드가 아니면 해킹
					_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, L"PROTOCOL_BATTLE_DEATH_REQ Hacking" );
					break;
				}
				else	ui8RoundNum = 1;
			}
			
			pPacket->ReadData(&KillHeader,			sizeof(DEATH_INFO_HEADER));
			// 킬 카운트가 잘못되었는지 검사
			if( KillHeader._nCount > MAX_KILLUSER_COUNT)
			{
				//최대 값을 넘어갔다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, L"Kill Count Over" );
				break;
			}

			pPacket->ReadData(aData,  (sizeof(DEATH_INFO_CHARA) * KillHeader._nCount));

			INT32 i32Rv = m_pRoom->DeathBattle(m_WorkThreadIdx, this, &KillHeader, aData, ui8RoundNum);
			if( EV_FAILED(i32Rv) )	//내부에서 패킷을 보냄..
			{
				wchar_t	wstrTemp[MAX_PATH];
				i3String::Format( wstrTemp, MAX_PATH, L"PROTOCOL_BATTLE_DEATH_REQ Fail / Rv : %d", i32Rv);

				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, wstrTemp);
			}
		}
		break;
	case PROTOCOL_BATTLE_TIMEOUTCLIENT_REQ		:	//리턴 패킷 없음,  방장이 보냄		//080303
		{
			CHECKVALID_ROOM;
			
			if( m_pRoom->GetBattleType() == BATTLE_SERVER_TYPE_DEDICATION && !m_pRoom->IsChallengeMode() )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, L"Hack Packet - GameKickNormalClient_U()" );
				break;
			}

			pPacket->ReadData( &i32Temp,	sizeof( INT32 ) );	
			m_pRoom->GameKickNormalClient_U( this , i32Temp );
		}
		break;
	case PROTOCOL_BATTLE_TIMEOUTMAINCLIENT_REQ	:	//일반 클라이언트들이 보냄 (방장이 아닌유저)	//080303
		{
			CHECKVALID_ROOM;
			m_pRoom->CheckMainClient_U( this );
		}
		break; 
	case PROTOCOL_BATTLE_SENDPING_REQ		:		//Ping	//080303
		{
			CHECKVALID_ROOM;
			UINT8	Ping[SLOT_MAX_COUNT]; 
			pPacket->ReadData(Ping, sizeof(UINT8) * SLOT_MAX_COUNT);
			m_pRoom->ChangePing_CS(this,  Ping);
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_REQ		:		//	방장	확인	0331		에러시 다른 메시지를 방장에게 보내줘야 한다. 
		{
			CHECKVALID_ROOM;
			UINT8 ui8BombPosition; 
			REAL32 ar32BombVecPos[3];

			if( m_pRoom->GetBattleType() == BATTLE_SERVER_TYPE_DEDICATION )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, L"Hack Packet - Mission Bomb Install" );
				break;
			}

			pPacket->ReadData(&ui32UTemp,		sizeof(UINT32));
			pPacket->ReadData(&ui8BombPosition,	sizeof(UINT8)); 
			pPacket->ReadData(ar32BombVecPos,	sizeof(REAL32) * 3);	// 2009.06.22 정순구 C4 설치 위치 추가
			if(ui32UTemp < SLOT_MAX_COUNT )	
			{
				m_pRoom->MissionBombInstall(m_WorkThreadIdx, this, ui32UTemp, ui8BombPosition, ar32BombVecPos);
			}
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_REQ	:			//	방장	확인	0331		에러시 다른메시지를 방장에게 보내줘야 한다. 
		{
			CHECKVALID_ROOM;

			if( m_pRoom->GetBattleType() == BATTLE_SERVER_TYPE_DEDICATION )
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, L"Hack Packet - Mission Bomb Uninstall" );
				break;
			}

			pPacket->ReadData(&ui32UTemp, sizeof(UINT32));
			if(ui32UTemp < SLOT_MAX_COUNT ) 
			{
				m_pRoom->MissionBombUnInstall(m_WorkThreadIdx, this, ui32UTemp);
			}
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_REQ		:		//	방장		080303 패킷이 얼마나 자주 오는지 확인해야 합니다.
		{
			CHECKVALID_ROOM;

			DESTRUCTION_INFO Destruction_Info; 
			pPacket->ReadData(&Destruction_Info, sizeof(DESTRUCTION_INFO));

			if( m_pRoom->GetBattleType() != BATTLE_SERVER_TYPE_DEDICATION )
				m_pRoom->MissionObjectDestroy(m_WorkThreadIdx, this, &Destruction_Info);
			else
			{
				//_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, "Hack Packet - Destroy Mode" );
			}
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_REQ		:		//방장 
		{
			//CHECKVALID_ROOM;

			//DEFENCE_INFO Defence_Info; 
			//pPacket->ReadData(&Defence_Info, sizeof(DEFENCE_INFO));
			//I3TRACE("[PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_REQ] DEBUG obj[%d][%d],User0[%d][%d],User1[%d][%d],User2[%d][%d],User3[%d][%d]\n", 
			//		Defence_Info._nObjectHP[0],Defence_Info._nObjectHP[1],
			//		Defence_Info._nUserDamage[0][0],Defence_Info._nUserDamage[1][0],
			//		Defence_Info._nUserDamage[0][1],Defence_Info._nUserDamage[1][1],
			//		Defence_Info._nUserDamage[0][2],Defence_Info._nUserDamage[1][2],
			//		Defence_Info._nUserDamage[0][3],Defence_Info._nUserDamage[1][3]); 
			//
			//if( m_pRoom->GetBattleType() != BATTLE_SERVER_TYPE_DEDICATION )
			//	m_pRoom->MissionObjectDefence(this, &Defence_Info);
			//else
			//{
			//	_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, "Hack Packet - Defense Mode" );
			//}
		}
		break;
	case PROTOCOL_BATTLE_MISSION_DEATHBLOW_REQ			:			//모든 클라이언트 공룡 임무 (DEATH BLOW)
		{
			CHECKVALID_ROOM;

			DEATHBLOW_INFO	DeathBlowInfo;
			pPacket->ReadData(&DeathBlowInfo, sizeof(DEATHBLOW_INFO));
			m_pRoom->MissionDeathBlow( this, &DeathBlowInfo );
		}
		break;
	//case PROTOCOL_BATTLE_HOLE_CHECK_REQ					:		// 내부에서 패킷을 보내요.	이거 사용하나?
	//	{
	//		CHECKVALID_ROOM;	
	//		m_pRoom->HoleCheck( this ); 
	//	}
	//	break;
	case PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_REQ	:
		{
			CHECKVALID_ROOM;

			m_pRoom->MissionTutorialRoundEnd( this );
		}
		break;

	// 강제탈퇴투표 2011.05.25 안국정
	// 클라이언트로 부터 강제탈퇴투표 요청이 들어왔습니다.
	// 투표 가능한지 판별하여 
	// 가능하면 > 전 클라이언트에 투표시작 통보
	// 불가능하면 > 투표요청한 클라이언트에만 투표불가 통보합니다.
	case PROTOCOL_BATTLE_SUGGEST_KICKVOTE_REQ :
		{
			CHECKVALID_ROOM;

			switch (m_ui8LocaleCode)
			{
			default: break;
			case LOCALE_CODE_INDONESIA:
			case LOCALE_CODE_THAILAND:
			case LOCALE_CODE_TURKEY:
				return FALSE;
			}

			UINT8	ui8KickUserSlot;	// 피신고인 슬롯번호
			UINT8	ui8KickReason;		// 투표 제안 사유번호 ( enum FORCEREMOVE_CAUSE )

			pPacket->ReadData( &ui8KickUserSlot,	sizeof(UINT8) );
			pPacket->ReadData( &ui8KickReason,		sizeof(UINT8) );

			if ( ui8KickUserSlot < SLOT_MAX_COUNT )
			{
				INT32 iRv = m_pRoom->DecideVote( this, ui8KickUserSlot, ui8KickReason );
				if ( EVENT_ERROR_FAIL <= iRv )
				{
					pSendPacket->SetProtocol( PROTOCOL_BATTLE_SUGGEST_KICKVOTE_ACK );
					pSendPacket->WriteData( &iRv, sizeof(INT32) );	

					bSendMessage = TRUE; 
				}
			}
		}
		break;
	// 클라이언트로부터 표가 도착했습니다.
	case PROTOCOL_BATTLE_VOTE_KICKVOTE_REQ :
		{
			CHECKVALID_ROOM;

			switch (m_ui8LocaleCode)
			{
			default: break; 
			case LOCALE_CODE_INDONESIA:
			case LOCALE_CODE_THAILAND:
			case LOCALE_CODE_TURKEY:
				return FALSE; 
			}

			UINT8	ui8Vote;	// 찬성표 또는 반대표

			pPacket->ReadData( &ui8Vote, sizeof(UINT8) );

			m_pRoom->GetVote( m_i32SlotIdx, ui8Vote );
		}
		break;
	case PROTOCOL_BATTLE_GM_PAUSE_REQ:
		{
			CHECKVALID_ROOM_GM_PAUSE;

			INT32 i32Ret = m_pRoom->GMPause( this );
			if ( EV_FAILED( i32Ret ) )
			{
				pSendPacket->SetProtocol( PROTOCOL_BATTLE_GM_PAUSE_ACK );
				pSendPacket->WriteData( &i32Ret,	sizeof( INT32 ) );
				bSendMessage = TRUE;
			}
		}
		break;
	case PROTOCOL_BATTLE_GM_RESUME_REQ:
		{
			CHECKVALID_ROOM_GM_PAUSE;

			INT32 i32Ret = m_pRoom->GMResume( m_WorkThreadIdx, m_i32SlotIdx );
			if ( EV_SUCCESSED( i32Ret ) )
			{
				m_pRoom->SendGMResume();
			}
			else
			{
				pSendPacket->SetProtocol( PROTOCOL_BATTLE_GM_RESUME_ACK );
				pSendPacket->WriteData( &i32Ret,	sizeof( INT32 ) );
				bSendMessage = TRUE;
			}
		}
		break;
	case PROTOCOL_BATTLE_REPORTSCREENSHOT_REQ:
		{
			UINT32 ui32Port;	// 동권이가 16만 써도 되는데 32을 써서 변경 합니다.
			BOOL bCheck = FALSE;

			pSendPacket->SetProtocol( PROTOCOL_BATTLE_REPORTSCREENSHOT_ACK );

			if( m_UserInfoServer.m_ui8ReportCount < MAX_REPORT_SCREENSHOT_COUNT )
			{
				ui32Port = g_pContextMain->m_ui16ReportFtpPort;

				UINT32 ui32InvenTime = i3ThreadTimer::StandTime().GetDateTimeYYMMDDHHMI();
				bCheck = TRUE;
				++m_UserInfoServer.m_ui8ReportCount;
				pSendPacket->WriteData( &bCheck,								sizeof(BOOL) );
				pSendPacket->WriteData( &ui32InvenTime,							sizeof(UINT32) );
				pSendPacket->WriteData( g_pContextMain->m_strReportFtpAddress,	MAX_REPORT_FTP_ADDRESS );
				pSendPacket->WriteData( &ui32Port,								sizeof(UINT32) );
			}
			else	pSendPacket->WriteData( &bCheck,							sizeof(BOOL) );

			bSendMessage = TRUE; 
		}
		break;
	case PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_REQ:
		{
			CHECKVALID_ROOM;			

			//////////////////////////////////////////////////////////////////////////
			PACKET_BATTLE_NEW_JOIN_ROOM_SCORE_ACK	PacketAck;

			if( NULL == m_pRoom ) break;

			m_pRoom->GetRoomNowScore(&PacketAck.m_RoomNowScore);

			bSendMessage = TRUE;
			pSendPacket->SetProtocol( PROTOCOL_BATTLE_NEW_JOIN_ROOM_SCORE_ACK );			
			pSendPacket->WriteData( &PacketAck, sizeof(PACKET_BATTLE_NEW_JOIN_ROOM_SCORE_ACK) );			
		}
		break;
	case PROTOCOL_BATTLE_USER_SOPETYPE_REQ:
		{
			CHECKVALID_BATTLE;
			pPacket->ReadData(&m_ui8UserScopeType, sizeof(UINT8));

			m_pRoom->SendUserScopeTypeInfo(m_ui8UserScopeType, m_i32SlotIdx, this);
		}
		break;
	default:
			char strTemp[256];
			sprintf( strTemp, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}