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

BOOL	CUserSession::_PacketParsingBattle( i3NetworkPacket* pPacket, i3NetworkPacket* pSendPacket, BOOL &bisKilled )
{
	INT32		i32Temp;
	UINT32		UTemp;
	PROTOCOL	ProtocolID = pPacket->GetProtocolID();	
	BOOL		bSendMessage = FALSE; 
	INT8 i8Fail		= -1;

	switch( ProtocolID )
	{ 
	case PROTOCOL_BATTLE_TIMERSYNC_REQ		:								//081224
		{//방에 있다면, 클라이언트의 한 프레임으로 패킷이 날아오는 경우가 있습니다. CHECKVALID_ROOM으로 처리하지 못함
#ifdef USE_GM_PAUSE
			CHECKVALID_ROOM_GM_PAUSE;
#else
			CHECKVALID_ROOM;
#endif

			INT32	i32GameTime; 
			REAL32	r32Speed;
			UINT8	ui8RoundCount;
			UINT8	ui8Ping;
			UINT32	ui32HackType; 
			pPacket->ReadData(&i32GameTime,		sizeof(INT32));
			pPacket->ReadData(&r32Speed,		sizeof(REAL32));
			pPacket->ReadData(&ui8RoundCount,	sizeof(UINT8));
			pPacket->ReadData(&ui8Ping,			sizeof(UINT8));
			pPacket->ReadData(&ui32HackType,	sizeof(UINT32));

			INT32	i32Diff = 0;
			BOOL	bCheck = FALSE;
			char	strTemp[MAX_PATH];
			UINT8	ui8RandTime = rand() % HACK_USER_DISCONNECT_TIME;

			if( m_ui32ATSendKill == 0 )
			{
				if( !m_pRoom->CheckGameTime(this, i32GameTime, ui8Ping, ui8RoundCount) )
				{
					bCheck			= TRUE;

					i3String::Format( strTemp, MAX_PATH, "TIMERSYNC Time ( UID:%I64d, Time:%d, Speed:%f, RoundCnt:%d, ServerRoundCnt:%d, ServerRed : %d, ServerBlue : %d, StageType : %d )",
						m_i64UID, i32GameTime, r32Speed, ui8RoundCount, m_pRoom->m_Info._InfoAdd._NowRoundCount, m_pRoom->m_Info._aui16TotalRoundCount[TEAM_RED], m_pRoom->m_Info._aui16TotalRoundCount[TEAM_BLUE], m_pRoom->m_Info._i32StageType );
				}

				// Speed 해킹 유저 체크 스피드가 11.f 이상이면 자릅니다.
				// 초기화 값으로 날라오는것도 확인하기 위해서. 
				if( (0.9f > r32Speed) || (11.f < r32Speed) )
				{
					bCheck			= TRUE;

					i3String::Format( strTemp, MAX_PATH, "TIMERSYNC Speed ( UID:%I64d, Speed:%f, Time:%d )", m_i64UID, r32Speed, i32GameTime );
				}
			}

			if( bCheck )
			{
#if defined (I3_DEBUG)
				m_pRoom->SendNotifyHackUser( m_i32SlotIdx, HACK_CHECK_TYPE_CLIENT, m_WorkThreadIdx, ui32HackType, m_ui8LocaleCode);
				char szTempTail[MAX_PATH];
				i3String::Format( szTempTail, MAX_PATH, "CLIENT:%d", ui32HackType );
				i3String::Concat( strTemp, szTempTail );
				//i3String::Format( szTemp, "TIMERSYNC Speed ( UID:%I64d, Speed:%f, Time:%d )", m_i64UID, r32Speed, i32GameTime );
				g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
#else
				if( m_ui32ATSendKill == 0 )
				{
					m_ui32ATSendKill = g_pContextMain->GetServerTime() + ui8RandTime;
					m_ui32KillReason = KILL_REASON_HACK_CLIENT;
					m_ui32HackKillReason = ui32HackType;
					i3String::Format( m_strHackNotice, DEF_SIZE_HACK_NOTICE, "%s, CLIENT:%d , RandTime:%d", strTemp, ui32HackType, ui8RandTime );
				}
#endif
				// 이미 위쪽에서 SendErrorMessage 를 호출하게 되어있으므로 
				//bSendMessage	= TRUE;
				//pSendPacket->SetProtocol( PROTOCOL_BATTLE_TIMERSYNC_ACK );
			}

			m_ui32BattleCheckTime =
#ifdef USE_GM_PAUSE
									m_pRoom->GetRoomBattleTime();
#else
									g_pContextMain->GetServerTime(); 
#endif
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

			BOOL	bIsObserverMode = FALSE ;	
			pPacket->ReadData(&bIsObserverMode,		sizeof(BOOL));	// 옵저버 모드 설정 여부.

			if( (TRUE == m_bGMUser) || (TRUE == m_bMODUser)  )	// GM이나 MOD라면.
			{
				m_pRoom->SetObserverMode( m_i32SlotIdx, bIsObserverMode ) ;	
			}
			else if ( TRUE == bIsObserverMode  ) // GM이 아닌데 옵저버 모드를 설정했다면 kick.
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_GAME_WRONG_DATA, ProtocolID );
				break;
			}

			// 배틀에서 사용하는 Weapon List 세팅 
			i3mem::Copy( m_aui32UseWeaponList, &m_UserInfoEquipmentWeapon, sizeof(UINT32)*WEAPON_USAGE_MISSION );
			i3mem::Copy( &m_UseCharaList, &m_UserInfoEquipmentChara, sizeof(USER_INFO_EQUIPMENT_CHARA) );

			m_ui16UsedItemCount	= 0;			// 개수제 사용무기 체크 카운트
			i3mem::FillZero( m_aUsedItemList,		sizeof(m_aUsedItemList) );
			i3mem::FillZero( m_aui8WeaponUseCount,	sizeof(m_aui8WeaponUseCount) );
			i3mem::FillZero( m_aui8CharaUseCount,	sizeof(UINT8)* (EQMIPEMENT_SLOT_COUNT - (WEAPON_USAGE_COUNT-1)) );
			
			// 무기 내구도 갱신
			if( GET_STAGETYPE( m_pRoom->m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL )
			{	// 튜토리얼 모드는 무조건 시작 합니다.
				m_pRoom->ReadyBattle_U(this);
			}
			else
			{
				UINT32 ui32InvenTime = g_pContextMain->GetInvenTime();
				BOOL bWeaponChk = _CheckWeaponEquip( FALSE, m_pRoom->GetInfo()->_WeaponFlag, m_aui32UseWeaponList, ui32InvenTime );
				BOOL bCharChk = EV_SUCCESSED( _CheckCharEquip( FALSE, (m_i32SlotIdx % TEAM_COUNT), &m_UseCharaList, ui32InvenTime ) );

#ifdef I3_DEBUG
				if ( g_pContextMain->m_bPassEquipCheck )
				{
					bWeaponChk = TRUE;
					bCharChk = TRUE;
				}
#endif
 				if( bWeaponChk && bCharChk )
				{	// 무기 검사, 케릭터 검사
					m_pRoom->ReadyBattle_U(this);
				}
				else 
				{
					bSendMessage = TRUE; 
					i32Temp = EVENT_ERROR_EVENT_READY_WEAPON_EQUIP;
					pSendPacket->SetProtocol( PROTOCOL_BATTLE_READYBATTLE_ACK );
					pSendPacket->WriteData( &i32Temp, sizeof(INT32) );				
				}
			}
		}
		break;
	case PROTOCOL_BATTLE_PRESTARTBATTLE_REQ	:		//로딩후 홀펀칭 시작	0829
		{
			CHECKVALID_ROOM;
			UINT32 ui32Stage;
			pPacket->ReadData( &ui32Stage, sizeof(UINT32) );

			if( LOAD_START != m_ui8LoadingStart )
			{	// Loading 패킷을 보내지 않았습니다.
				if( ui32Stage == m_pRoom->m_Info._InfoBasic._StageID)
				{
					switch( m_pRoom->m_Info._InfoBasic._State )
					{
					case ROOM_STATE_BATTLE_END	:
					case ROOM_STATE_READY		: 
						// 로드된 스테이지와 현재 스테이지가 같은지 확인
						m_pRoom->PreStartBattle( this );//내부에서 패킷을 보냄
						break;
					default:
						bisKilled = TRUE;
						m_ui32KillReason = KILL_REASON_HACK_PACKET;
						break;
					}
				}
				else
				{
					bisKilled = TRUE;		// 스테이지가 다른 스테이지입니다.
					m_ui32KillReason = KILL_REASON_HACK_PACKET;

				#ifdef USE_LOG_KILLREASON
					char strTemp[MAX_PATH];
					i3String::Format( strTemp, MAX_PATH, "[BATTLE_PRESTARTBATTLE_REQ] Invalid Stage Loading( UID:%I64d, Nick: %s, Stage : (New: %d, Old: %d) )", m_i64UID, m_UserInfoBasic._nick, ui32Stage, m_pRoom->m_Info._InfoBasic._StageID );
					g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
				#endif
				}

			#ifdef USE_LOG_KILLREASON
				char strTemp[MAX_PATH];
				i3String::Format( strTemp, MAX_PATH, "[BATTLE_PRESTARTBATTLE_REQ] Not Send Loading Packet( UID:%I64d, Nick: %s, State: %d", m_i64UID, m_UserInfoBasic._nick, m_ui8LoadingStart );
				g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
			#endif
				break;
			}

			if( ui32Stage == m_pRoom->m_Info._InfoBasic._StageID )
			{	
				// 로드된 스테이지와 현재 스테이지가 같은지 확인
				m_pRoom->PreStartBattle( this );//내부에서 패킷을 보냄
			}
			else
			{
				bisKilled = TRUE;		// 스테이지가 다른 스테이지입니다.
				m_ui32KillReason = KILL_REASON_HACK_PACKET;

#ifdef USE_LOG_KILLREASON
				char strTemp[MAX_PATH];
				i3String::Format( strTemp, MAX_PATH, "[BATTLE_PRESTARTBATTLE_REQ] Invalid Stage Loading( UID:%I64d, Nick: %s, Stage : (New: %d, Old: %d) )", m_i64UID, m_UserInfoBasic._nick, ui32Stage, m_pRoom->m_Info._InfoBasic._StageID );
				g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );
#endif
			}
		}
		break;
	case PROTOCOL_BATTLE_STARTBATTLE_REQ		:		//전투 시작			0829
		{
			CHECKVALID_ROOM;
			m_i32LogD_RespawnCount = 0;
			m_pRoom->StartBattle_U( this ); //내부에서 패킷을 보냄 

			// 발렌타인 이벤트- Vallentine Chocolate 인증
			INVEN_BUFFER* pInven = GetItemByItemID( 1301101000 );
			if( pInven )
			{
				_SetUsedItem( pInven->_i64ItemWareDBIdx, 1301101000, USED_ITEM_TYPE_ONE, 1, 0 );
				//g_pModuleTrans->AuthSendAuthItem(m_WorkThreadIdx, this, pInven->_ui32ItemWareDBIdx, 1 );
			}
			else
			{	// 케투팻
				pInven = GetItemByItemID( 1301091000 );

				if( pInven )
					_SetUsedItem( pInven->_i64ItemWareDBIdx, 1301091000, USED_ITEM_TYPE_ONE, 1, 0 );
			}
		}
		break; 
	case PROTOCOL_BATTLE_GIVEUPBATTLE_REQ		:		//전투 포기			0829
		{
#ifdef USE_GM_PAUSE
			CHECKVALID_ROOM_GM_PAUSE;
#else
			CHECKVALID_ROOM;
#endif
			BOOL bFreeMove = FALSE;

			// 전장 이동권을 사용했는지를 체크
			i32Temp = EVENT_ERROR_SUCCESS;
			TWareDxIdx TDBIdx;
			pPacket->ReadData(&TDBIdx, sizeof(TWareDxIdx));

			if( 0 < TDBIdx )
			{
				INVEN_BUFFER* pInven = GetItemByWareDBIdx( TDBIdx );
				if( pInven )
				{
					// 아이템 인증
					g_pModuleTrans->AuthSendAuthItem(m_WorkThreadIdx, this, pInven->_i64ItemWareDBIdx, 1 );

					// 전장 이동권 사용
					bFreeMove = TRUE;
				}
				else 
				{
					I3TRACE( "AUTH FAIL : Item Inventory Error ");
				}
			}

//#if defined (LOCALE_INDONESIA)	// 춘절 이벤트 적용(2월 1일 ~ 2월 22일)
//			if( 1102220000 > g_pContextMain->GetInvenTime() )
//			{
//				bFreeMove = TRUE;
//			}
//#endif

			// 만약 전장 이동권이 들어간다면, 여기서 아이템 인증이 들어갑니다.
			if( EV_FAILED( m_pRoom->GiveUpBattle_U( m_WorkThreadIdx, this, bFreeMove ) ) )
			{
				bSendMessage = TRUE;

				INT32 i32Temp = EVENT_ERROR_FAIL;
				pSendPacket->SetProtocol( PROTOCOL_BATTLE_GIVEUPBATTLE_ACK );
				pSendPacket->WriteData( &i32Temp,	sizeof(INT32) );
			}


		}
		break; 
		//확인	0331	이정보를 확인해야 합니다. 확인하지 않으면..같은방 모든 클라이언트 모두 끝장.
	case PROTOCOL_BATTLE_RESPAWN_REQ			:		//리스폰
		{
#ifdef USE_GM_PAUSE
			CHECKVALID_ROOM_GM_PAUSE;
			
			if ( m_pRoom->IsGMPause() )
			{
				UINT32 ui32ATValue = g_pContextMain->GetServerTime();
				if ( ui32ATValue < m_ui32ATCanRespawnPacket || m_ui32ATCanRespawnPacket + 3 < ui32ATValue )
					return FALSE;
			}
#else
			CHECKVALID_ROOM;
#endif

			m_i32LogD_RespawnCount++;

			UINT32						aui32UseWeaponList[ WEAPON_USAGE_COUNT ];
			USER_INFO_EQUIPMENT_CHARA	stEquipmentData;
			UINT8 ui8WeaponFlag;
			pPacket->ReadData( aui32UseWeaponList,	sizeof( aui32UseWeaponList ) );
			pPacket->ReadData( &stEquipmentData,	sizeof( USER_INFO_EQUIPMENT_CHARA ));
			pPacket->ReadData( &ui8WeaponFlag,		sizeof( UINT8 ));

			if(NULL == m_pRoom) 
			{
				bisKilled		= TRUE;
				m_ui32KillReason	= KILL_REASON_HACK_PACKET;

				char strTemp[ MAX_PATH ];
				i3String::Format( strTemp, MAX_PATH, "[RECV]PROTOCOL_BATTLE_RESPAWN_REQ[UID:%I64d,Invalid Room]", m_i64UID );
				g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );

				break;
			}
		#ifdef USE_RESPAWN_SKIP
			else if( MISSION_STATE_BATTLE != m_pRoom->m_Info._i32MiStateType )	break;
		#endif

			if(GET_STAGETYPE( m_pRoom->m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL)
			{
				m_pRoom->ReSpawnBattle_U( this ); 
			}
			else 
			{
				// 내구도 사용 여부 체크
				CheckEndurance();
				// 해킹 체크
				BOOL bWEFlag = _CheckWeaponEquip( TRUE, m_pRoom->GetInfo()->_WeaponFlag, aui32UseWeaponList, m_ui32BattleStartInvenTime );
				INT32 i32CheckCharEquipRet = _CheckCharEquip( TRUE, (m_i32SlotIdx % TEAM_COUNT), &stEquipmentData, m_ui32BattleStartInvenTime );
				BOOL bRHFlag = m_pRoom->CheckRespawnHacking( this );

#ifdef I3_DEBUG
				if ( g_pContextMain->m_bPassEquipCheck )
				{
					bWEFlag = TRUE;
					i32CheckCharEquipRet = EVENT_ERROR_SUCCESS;
					ui8WeaponFlag = m_pRoom->m_Info._InfoBasic._WeaponFlag;
					bRHFlag = TRUE;
				}
#endif
				if( bWEFlag &&	// 무기 내구도 갱신
					EV_SUCCESSED( i32CheckCharEquipRet ) &&
					(ui8WeaponFlag == m_pRoom->m_Info._InfoBasic._WeaponFlag) &&
					bRHFlag ) // 리스폰 해킹 방지
				{
					I3ASSERT( sizeof( m_aui32UseWeaponList ) == sizeof( aui32UseWeaponList ) );
					i3mem::Copy( m_aui32UseWeaponList, aui32UseWeaponList, sizeof( m_aui32UseWeaponList ) );
					// 공룡이 사용되는 모드에서 공룡이 바뀌면 데디에 알려야함
					if( ( m_pRoom->m_Info._i32StageType == STAGE_TYPE_ESCAPE || m_pRoom->m_Info._i32StageType == STAGE_TYPE_CROSSCOUNT ) &&
						(stEquipmentData.GetDinoNum() != m_UseCharaList.GetDinoNum() )
					  )
					{
						for( INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++ )
						{
							// 해당 유저가 일반 공룡이 아니면 패스
							if( m_pRoom->m_Info._MiDinoInfo._uiRaptorSlotIdx[i] != m_i32SlotIdx ) continue;

							m_pRoom->m_Info._abDinoStateUpdate[m_i32SlotIdx] = true; 
							m_pRoom->m_Info._aui32DInoCharInfo[m_i32SlotIdx] = stEquipmentData.GetDinoNum();
						}
					}
					i3mem::Copy( &m_UseCharaList, &stEquipmentData, sizeof(USER_INFO_EQUIPMENT_CHARA) );
					m_pRoom->ReSpawnBattle_U( this ); 
				}
				else
				{
					bisKilled			= TRUE;
					m_ui32KillReason	= KILL_REASON_HACK_PACKET;

					char strTemp[ MAX_PATH ];
					i3String::Format( strTemp, MAX_PATH, "[RECV]PROTOCOL_BATTLE_RESPAWN_REQ[UID:%I64d, WE(%d)/i32CheckCharEquipRet(%d)/RH(%d)]", m_i64UID, bWEFlag, i32CheckCharEquipRet, bRHFlag );
					g_pModuleLogFile->Write_M_Log( m_WorkThreadIdx, this, strTemp );

					break;
				}
			}
		}
		break;
	case PROTOCOL_BATTLE_RESPAWN_FOR_AI_REQ:				//081224
		{
			CHECKVALID_ROOM;

			INT32 i32SlotIdx;
			pPacket->ReadData(&i32SlotIdx,	sizeof(INT32));
			if( FALSE == m_pRoom->ReSpawnBattleForAi_U( this, i32SlotIdx ) )
			{
				this->_SendErrorMessage( this->m_WorkThreadIdx, EVENT_ERROR_EVENT, ProtocolID, KILL_REASON_HACK_PACKET );
			}
		}
		break;
	case PROTOCOL_BATTLE_SLOT_EQUIPMENT_REQ:
		{
			CHECKVALID_ROOM;

			UINT32 ui32ItemID;
			pPacket->ReadData( &ui32ItemID,	sizeof(INT32) );

			_EquipmentInBattle( ui32ItemID );
		}
		break;
	case PROTOCOL_BATTLE_DEATH_REQ				:		//게임중 사망			확인	0331		
		{
			CHECKVALID_ROOM;
	
			if( STAGE_MASK_TYPE_TRAINING != m_pRoom->m_Info._InfoBasic._StageMask && m_pRoom->GetBattleType() != BATTLE_SERVER_TYPE_RELAY && GET_STAGETYPE( m_pRoom->m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )
			{
				// 이곳은 챌린지 모드일 경우만 클라이언트에서 보내는 프로토콜이므로 챌린지 모드가 아니면 해킹
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, "PROTOCOL_BATTLE_DEATH_REQ Hacking" );
				break;
			}

			DEATH_INFO_HEADER	KillHeader;
			DEATH_INFO_CHARA	pData[MAX_KILLUSER_COUNT];
			//UINT8				ui8RoundNum;
			
			pPacket->ReadData(&KillHeader,			sizeof(DEATH_INFO_HEADER));
			// 킬 카운트가 잘못되었는지 검사
			if( KillHeader._nCount > MAX_KILLUSER_COUNT)
			{
				//최대 값을 넘어갔다.
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, "Kill Count Over" );
				break;
			}

			pPacket->ReadData(pData,  (sizeof(DEATH_INFO_CHARA) * KillHeader._nCount));
			//pPacket->ReadData(&ui8RoundNum, sizeof(UINT8) ); 

			// 이곳으로 오는 경우는 킬 처리가 무조건 되므로 라운드 번호를 직접 넘겨줍니다. ( 클라이언트에서 받지 않음 )
			if( EV_FAILED( m_pRoom->DeathBattle( m_WorkThreadIdx, this, &KillHeader, pData, m_pRoom->m_Info._InfoAdd._NowRoundCount) ) )			 //내부에서 패킷을 보냄..
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID );
			}
		}
		break;
	case PROTOCOL_BATTLE_TIMEOUTCLIENT_REQ		:	//리턴 패킷 없음,  방장이 보냄		//080303
		{
			CHECKVALID_ROOM;
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
		#ifdef DEF_OBSERVER_MODE
			UINT8	aui8Ping[SLOT_ALL_COUNT]; 
		#else
			UINT8	aui8Ping[SLOT_MAX_COUNT]; 
		#endif
			pPacket->ReadData(aui8Ping, sizeof(aui8Ping) );
			m_pRoom->ChangePing_CS(this,  aui8Ping);
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_REQ		:		//	방장	확인	0331		에러시 다른 메시지를 방장에게 보내줘야 한다. 
		{
			CHECKVALID_ROOM;
			UINT8 ui8BombPosition; 
			REAL32 ar32BombVecPos[3];

			pPacket->ReadData(&UTemp,			sizeof(UINT32));
			pPacket->ReadData(&ui8BombPosition,	sizeof(UINT8)); 
			pPacket->ReadData(ar32BombVecPos,	sizeof(ar32BombVecPos));	// 2009.06.22 정순구 C4 설치 위치 추가
			if(UTemp < SLOT_MAX_COUNT )	
			{
				m_pRoom->MissionBombInstall(this, UTemp, ui8BombPosition, ar32BombVecPos);
			}
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_REQ	:			//	방장	확인	0331		에러시 다른메시지를 방장에게 보내줘야 한다. 
		{
			CHECKVALID_ROOM;

			pPacket->ReadData(&UTemp, sizeof(UINT32));
			if(UTemp < SLOT_MAX_COUNT ) 
			{
				m_pRoom->MissionBombUnInstall(this, UTemp);
			}
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_REQ		:		//	방장		080303 패킷이 얼마나 자주 오는지 확인해야 합니다.
		{
			CHECKVALID_ROOM;

			DESTRUCTION_INFO Destruction_Info; 
			pPacket->ReadData(&Destruction_Info, sizeof(DESTRUCTION_INFO));

			if( m_pRoom->GetBattleType() != BATTLE_SERVER_TYPE_DEDICATION )
				m_pRoom->MissionObjectDestroy(this, &Destruction_Info);
			else
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, "Hack Packet - Destroy Mode" );
			}
		}
		break; 
	case PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_REQ		:		//방장 
		{
			CHECKVALID_ROOM;

			DEFENCE_INFO Defence_Info; 
			pPacket->ReadData(&Defence_Info, sizeof(DEFENCE_INFO));
			I3TRACE("[PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_REQ] DEBUG obj[%d][%d],User0[%d][%d],User1[%d][%d],User2[%d][%d],User3[%d][%d]\n", 
					Defence_Info._nObjectHP[0],Defence_Info._nObjectHP[1],
					Defence_Info._nUserDamage[0][0],Defence_Info._nUserDamage[1][0],
					Defence_Info._nUserDamage[0][1],Defence_Info._nUserDamage[1][1],
					Defence_Info._nUserDamage[0][2],Defence_Info._nUserDamage[1][2],
					Defence_Info._nUserDamage[0][3],Defence_Info._nUserDamage[1][3]); 

			if( m_pRoom->GetBattleType() != BATTLE_SERVER_TYPE_DEDICATION )
				m_pRoom->MissionObjectDefence(this, &Defence_Info);
			else
			{
				_SendErrorMessage( m_WorkThreadIdx, EVENT_ERROR_FAIL, ProtocolID, KILL_REASON_HACK_PACKET, "Hack Packet - Defense Mode" );
			}
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
	case PROTOCOL_BATTLE_HOLE_CHECK_REQ					:		// 내부에서 패킷을 보내요.	이거 사용하나?
		{
			CHECKVALID_ROOM;	
			m_pRoom->HoleCheck( this ); 
		}
		break;
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

			UINT8	ui8Vote;	// 찬성표 또는 반대표

			pPacket->ReadData( &ui8Vote, sizeof(UINT8) );

			m_pRoom->GetVote( m_i32SlotIdx, ui8Vote, m_WorkThreadIdx );
		}
		break;
#ifdef USE_GM_PAUSE
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
#endif

	case PROTOCOL_BATTLE_REPORTSCREENSHOT_REQ:
		{
			BOOL bCheck = FALSE;

			pSendPacket->SetProtocol( PROTOCOL_BATTLE_REPORTSCREENSHOT_ACK );

			if( m_ui8ReportCount < MAX_REPORT_SCREENSHOT_COUNT )
			{
				UINT32 ui32InvenTime = g_pContextMain->GetInvenTime();
				bCheck = TRUE;
				++m_ui8ReportCount;
				pSendPacket->WriteData( &bCheck,								sizeof(BOOL) );
				pSendPacket->WriteData( &ui32InvenTime,							sizeof(UINT32) );
				pSendPacket->WriteData( g_pContextMain->m_strReportFtpAddress,	MAX_STRING_COUNT );
				pSendPacket->WriteData( &g_pContextMain->m_ui32ReportFtpPort,	sizeof(UINT32) );
			}
			else	pSendPacket->WriteData( &bCheck,							sizeof(BOOL) );

			bSendMessage = TRUE; 
		}
		break;
	default:
			char strTemp[256];
			i3String::Format( strTemp, 256, "UnKnown Packet - %d", ProtocolID );
			I3TRACE( strTemp );
			I3TRACE( "UnKnown Packet - Kill User"); 
			bisKilled = TRUE; 
			m_ui32KillReason = KILL_REASON_UNKNOWN_PACKET;
			break;
	}

	return bSendMessage;
}