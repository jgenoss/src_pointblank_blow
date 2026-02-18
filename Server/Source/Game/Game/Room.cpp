#include "pch.h"
#include "Room.h"
#include "UserSession.h"
#include "USerManager.h"
#include "ModuleControl.h"
#include "ModuleCast.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"

#include "RoomInAnnihilation.h"
#include "RoomInBomb.h"
#include "RoomInEscape.h"
#include "RoomInChallenge.h"
#include "RoomInDeathmatch.h"
#include "RoomInDefence.h"
#include "RoomInDestroy.h"
#include "RoomInTutorial.h"
#include "RoomInCrossCount.h"
#include "RoomInConvoy.h"
#include "RoomInRunAway.h"
#include "RoomInSpace.h"

#include "RoomManager.h"

bool isContainedClanIndexInList(const TEAM_CLAN_INDEX_LIST & idxList, const UINT32 ui32CheckIndex)
{
	for (UINT8 i = 0; i < idxList.ui8ClanIdxCount; ++i)
	{
		if (ui32CheckIndex == idxList.aClanIdx[i])
		{
			return  true;
		}
	}
	return false;
}

I3_CLASS_INSTANCE(CRoom, i3ElementBase); 

CRoom::CRoom()
{
	m_pRoomMode				= NULL;
	m_pAnnihilation			= NULL;
	m_pBomb					= NULL;
	m_pEscape				= NULL;
	m_pDeathmatch			= NULL;
	m_pDefence				= NULL;
	m_pDestroy				= NULL;		
	m_pChallenge			= NULL;
	m_pTutorial				= NULL;
	m_pCrossCount			= NULL;
	m_pConvoy				= NULL;
	m_pRunAway				= NULL;
	m_pSpace				= NULL;

	m_ui32ATCountDownR		= 0;

	m_pRandMap				= NULL;
}

CRoom::~CRoom(void)
{
	OnDestroy();
}

BOOL CRoom::OnCreate( UINT32 ui32ChannelNum, UINT32 ui32RoomNum )
{
	i3mem::FillZero(m_ui32RandomState, sizeof(m_ui32RandomState));

	for (int i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		m_ui32RandomState[i] = (rand() % 10000 + GetCurrentThreadId());
	}

	m_ui8RandomIndex = 0;

	m_Info._ui32ChannelNum	= ui32ChannelNum;
	m_Info._InfoBasic._Idx	= ui32RoomNum;
	m_bIsCreate				= FALSE;

	m_ui8ChannelType		= g_pContextMain->m_pChannelType[m_Info._ui32ChannelNum];

	//Set Nc LogServer 
	m_i32LogChannelNum		= ((g_pContextMain->m_i32ServerIdx*GAME_CHANNEL_COUNT) + (m_Info._ui32ChannelNum+1));
	m_ui32LogUniqueNumber	= (g_pContextMain->m_i32ServerIdx << 24) | (ui32ChannelNum << 16) | ui32RoomNum;

	m_pRandMap = i3LinearRand::NewObject();
	if( NULL == m_pRandMap )									return FALSE;
	if( FALSE == m_pRandMap->Create( STAGE_UID_MAX, FALSE ) )	return FALSE;

	// Mode Setting
	m_pAnnihilation = CRoomInAnnihilation::NewObject();
	if( NULL == m_pAnnihilation ) return FALSE;
	if( FALSE == m_pAnnihilation->OnCreate( &m_Info ) )		return FALSE;

	m_pBomb			= CRoomInBomb::NewObject();
	if( NULL == m_pBomb ) return FALSE;
	if( FALSE == m_pBomb->OnCreate( &m_Info ) )				return FALSE;

	m_pEscape		= CRoomInEscape::NewObject();
	if( NULL == m_pEscape ) return FALSE;
	if( FALSE == m_pEscape->OnCreate( &m_Info ) )			return FALSE;

	m_pDeathmatch	= CRoomInDeathmatch::NewObject();
	if( NULL == m_pDeathmatch ) return FALSE;
	if( FALSE == m_pDeathmatch->OnCreate( &m_Info ) )		return FALSE;

	m_pDefence		= CRoomInDefence::NewObject();
	if( NULL == m_pDefence ) return FALSE;
	if( FALSE == m_pDefence->OnCreate( &m_Info ) )			return FALSE;

	m_pDestroy		= CRoomInDestroy::NewObject();
	if( NULL == m_pDestroy ) return FALSE;
	if( FALSE == m_pDestroy->OnCreate( &m_Info ) )			return FALSE;
		
	m_pChallenge	= CRoomInChallenge::NewObject();
	if( NULL == m_pChallenge )								return FALSE;
	if( FALSE == m_pChallenge->OnCreate( &m_Info ) )		return FALSE;

	m_pTutorial		= CRoomInTutorial::NewObject();
	if( NULL == m_pTutorial )								return FALSE;
	if( FALSE == m_pTutorial->OnCreate( &m_Info ) )			return FALSE;

	m_pCrossCount	= CRoomInCrossCount::NewObject();
	if( NULL == m_pCrossCount )								return FALSE;
	if( FALSE == m_pCrossCount->OnCreate( &m_Info ) )		return FALSE;

	m_pConvoy		= CRoomInConvoy::NewObject();	 
	if( NULL == m_pConvoy )									return FALSE;
	if( FALSE == m_pConvoy->OnCreate( &m_Info ) )			return FALSE;

	m_pRunAway		= CRoomInRunAway::NewObject();	 
	if( NULL == m_pRunAway )								return FALSE;
	if( FALSE == m_pRunAway->OnCreate( &m_Info ) )			return FALSE;
	
	m_pSpace		= CRoomInSpace::NewObject();
	if (NULL == m_pSpace)									return FALSE;
	if (FALSE == m_pSpace->OnCreate(&m_Info))				return FALSE;

	// 크래쉬 하는 문제 땜빵 처리입니다.
	CUserSession* pSession = g_pUserManager->GetSession( 0 );
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		m_Info._pSession[ i ] = pSession;
	}

	return TRUE;
}

void CRoom::OnDestroy()
{
	I3_SAFE_RELEASE( m_pAnnihilation );
	I3_SAFE_RELEASE( m_pBomb );
	I3_SAFE_RELEASE( m_pEscape );
	I3_SAFE_RELEASE( m_pDeathmatch );
	I3_SAFE_RELEASE( m_pDefence );
	I3_SAFE_RELEASE( m_pDestroy );		
	I3_SAFE_RELEASE( m_pChallenge );
	I3_SAFE_RELEASE( m_pTutorial );
	I3_SAFE_RELEASE( m_pCrossCount );
	I3_SAFE_RELEASE( m_pConvoy );
	I3_SAFE_RELEASE( m_pRunAway );
	I3_SAFE_RELEASE( m_pSpace );

	I3_SAFE_RELEASE( m_pRandMap );
}

BOOL CRoom::CreateRoom( CUserSession * pSession, ROOM_INFO_CREATE* pInfoCreate )
{
	ROOM_INFO_BASIC * pRoomInfo = &(pInfoCreate->RoomInfoBasic);

	//Init Value 
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		m_Info._aSlotInfo[i].Reset( SLOT_STATE_EMPTY );
		m_stRoomUserInfo[i].Reset();
	}

	//Set Basic Info
	m_Info._InfoBasic._Ping						= 5;
	m_Info._InfoBasic._PersonNow				= 1;
	m_ui32TimerToPause							= 0;

	m_StageUID									= StageID::GetStageUID( pRoomInfo->_StageID );
	m_Info._i32MainSlotIdx						= 0;	
	//m_Info._InfoBasic._StageID				= pRoomInfo->_StageID;	
	m_Info._InfoBasic._InfoFlag					= pRoomInfo->_InfoFlag;
	m_Info._InfoBasic._WeaponFlag				= pRoomInfo->_WeaponFlag;	
	m_Info._InfoBasic._State					= ROOM_STATE_READY;
	m_Info._InfoBasic._IsClanMatch				= pRoomInfo->_IsClanMatch;
	m_Info._InfoBasic._OptionFlag				= pRoomInfo->_OptionFlag;
	i3String::Copy( m_Info._InfoBasic._Title, pRoomInfo->_Title, NET_ROOM_NAME_SIZE );
	m_Info._InfoBasic._Title[ NET_ROOM_NAME_SIZE-1 ]	= '\0';

	// InfoFlag 체크
	//비밀방 유무
	if( GET_ROOM_INFO_PRIVATE_MODE( m_Info._InfoBasic._InfoFlag ) &&
		('\0' != pInfoCreate->szRoomPw[0]) ) 
	{
		m_Info._InfoBasic._InfoFlag |= ROOM_INFO_FLAG_PRIVATE_MODE;
		i3mem::Copy( m_strPasswd, pInfoCreate->szRoomPw,  NET_ROOM_PW );
	}
	else
	{
		m_Info._InfoBasic._InfoFlag &= ~ROOM_INFO_FLAG_PRIVATE_MODE;
		m_strPasswd[0]							= '\0';
	}

	// 클랜 매치메이킹
	m_TMatchingID = pInfoCreate->m_TMatchingID;

	i3mem::Copy( m_Info._aui32BattleClanIdx, pInfoCreate->m_ui32ClanIdx, sizeof( UINT32 ) * TEAM_COUNT );
	i3mem::Copy( m_Info.m_TMatchTeamID, pInfoCreate->m_TMatchTeamID, sizeof( T_TeamID ) * TEAM_COUNT );

	// 방정보 갱신
	if(EVENT_ERROR_SUCCESS != ChangeRoomInfo(pSession, pRoomInfo, pInfoCreate->enemyAiCount, pInfoCreate->difficultyLevel, TRUE )) { return FALSE; }
	if(EVENT_ERROR_SUCCESS != ChangeRoomInfoOption(pSession, &pInfoCreate->RoomInfoAdd, TRUE )) { return FALSE; }

	//Set Common Value	
	m_bIsBattle							= FALSE;
	m_i32WinTeam						= TEAM_DRAW;//TEAM_RED_WIN;
	m_i32TotalRespawnIdx				= 0;
	m_i8StartCountdown					= 0;

	m_Info._aui16TotalRoundCount[0]		= 0;
	m_Info._aui16TotalRoundCount[1]		= 0;

	m_i8KickUserInsertPoint				= 0;
	i3mem::FillZero( m_aKickUserList, sizeof(T_UID)*KICK_LIST_IN_ROOM_MAX );

	ResetBattleGroupInfo(); 

	//Set Main Slot Info
	m_Info._pSession[0]					= pSession;
	m_Info._aSlotInfo[0]._State			= SLOT_STATE_NORMAL;
	m_Info._aSlotInfo[0]._Rank			= (UINT8)pSession->m_ui32ViewRank;
	m_Info._aSlotInfo[0]._pcCafe		= pSession->m_UserInfoBasic.m_ui8PCCafe;
	m_Info._aSlotInfo[0]._eSportID		= pSession->m_UserInfoBasic.m_ui8ESportID;
	m_Info._aSlotInfo[0]._clanidx		= pSession->m_UserInfoBasic.m_ui32Clanidx;
	m_Info._aSlotInfo[0]._clanstate		= pSession->m_UserInfoBasic.m_ui32ClanState;
	m_Info._aSlotInfo[0]._clanExpRank	= pSession->m_UserInfoBasic.m_ui8ClanExpRank;
	m_Info._aSlotInfo[0]._clanMark		= pSession->m_UserInfoBasic.m_ui32ClanMark; 
	m_Info._aSlotInfo[0]._Item			= pSession->GetUseItemInRoom();					// 아이템 정보 담기	
	m_stRoomUserInfo[0]._ui8Ping		= pSession->m_ui8Ping;	// 방장 ping 저장
	m_Info._aSlotInfo[0]._ui8NationalCode	=pSession->m_ui8Country; 

	pSession->m_ui8LoadingStart			= LOAD_START_CREATE;	// Ready 2회 입력시 무시함

	ITEM_INFO CharItemInfo;

	if (pSession->m_MultiSlot.GetCharEquipment((INT32)pInfoCreate->m_ui8CharMainSlotIdx, CHAR_EQUIPMENT_CHARA, &CharItemInfo))
	{
		if (pSession->m_pDesign->SetTeamSlot(pSession->m_MultiSlot, pInfoCreate->m_ui8CharMainSlotIdx, CharItemInfo.m_TItemID))
		{
			pSession->ChangeDataBaseInfo(SAVE_DATABASE_CHARA_COMMON);
		}
	}

	i3mem::Copy( m_Info._aSlotInfo[0]._ClanName, pSession->m_UserInfoBasic.m_strClanName, NET_CLAN_NAME_SIZE );

	// Log 용
	m_ui32LogBattleCount				= 0;
	m_i64LogMainUserDbIdx				= pSession->m_i64UID; 

	_ResetTimeOut();

	// 모든 스테이지 검사 후 마지막에 호출 부탁 드립니다.
	if( FALSE == m_pRoomMode->SetRoomOption( pRoomInfo->_StageID ) ) return FALSE;

	const bool bClanChannel = IsClanChannel();
	if ( bClanChannel )
	{
		if( IsMatchMaking() )
		{	// 매치 메이킹 일때
			m_i8IsClanMatch				 = ZLOG_CLAN_MATCHING;
			SetRoomInfoMatchMaking( m_Info );
		}
		else	// 비 매치 메이킹 일때
		{	
			// 클랜 Idx 저장
			i3mem::FillZero( m_Info._aui32BattleClanIdx,	sizeof(UINT32) * TEAM_COUNT);
			i3mem::FillZero( m_Info.m_TMatchTeamID,			sizeof(T_TeamID) * TEAM_COUNT);
			m_i8IsClanMatch				= ZLOG_CLAN_NORMAL;
			// 위 ChangeRoomInfoOption에서 이미 호출하지만, 혼돈 방지를 위해서 한 번 더..
			SetRoomInfoNonMatchMaking( m_Info );	
		}
	}
	else
	{
		i3mem::FillZero( m_Info._aui32BattleClanIdx,	sizeof(UINT32) * TEAM_COUNT);
		i3mem::FillZero( m_Info.m_TMatchTeamID,			sizeof(T_TeamID) * TEAM_COUNT);
		m_i8IsClanMatch					= ZLOG_CLAN_NOT;
	}

	if( IsChallengeMode() )
	{
		// 일반유저에게만 적용
		for( UINT32 i = 0 ; i < SLOT_MAX_COUNT; i++ )
		{
			m_Info.m_aui32AILevel[ i ] = 0;
		}
	}
	// 인원수 모드에 따라 Max를 바꿔준다.
	UINT8 ui8PersonPlayerMax = g_pContextMain->GetMaxPerson( m_Info._InfoBasic._StageID );

	if ( 0 == ui8PersonPlayerMax ) return FALSE;
	if (pRoomInfo->_PersonPlayerMax < ui8PersonPlayerMax)
	{
		m_Info._InfoBasic._PersonPlayerMax = pRoomInfo->_PersonPlayerMax;
	}
	else
	{
		m_Info._InfoBasic._PersonPlayerMax = ui8PersonPlayerMax;
	}

	// 닫힌 슬롯 만큼 슬롯을 닫아준다.
	for (INT32 i = m_Info._InfoBasic._PersonPlayerMax; i < SLOT_MAX_COUNT; i++)
	{
		m_Info._aSlotInfo[i]._State = SLOT_STATE_CLOSE;
	}
	m_bGMPause			= FALSE;
	m_ui8AtkDefChange	= 0;
	m_ui32RoomBattleTime = 0;

 	return TRUE;
}

INT32 CRoom::Enter(INT32 i32ThreadIdx,	CUserSession * pSession, ROOM_INFO_JOIN* pInfoJoin)
{
	// 방 입장에 성공 여부와 상관없이 입장 요청을 했다는거 자체가 방에 있지 않은거니 0으로 바꿔줍니다. - 서동권
	pSession->m_ui32RoomBattleCount = 0;

	if( false == pSession->CheckAuthLevel() )
	{	// GM이나 MOD가 아닐 경우에만 체크
		//난입이 불가능한 방
		if( (ROOM_STATE_READY != m_Info._InfoBasic._State) && (1 == m_Info._InfoAdd._LockInterEnter) )
		{
			return EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER; 
		}
		// 클랜 매치 메이킹 일때는 입장 불가
		if( ((CHANNEL_TYPE_CLAN == m_ui8ChannelType) || (CHANNEL_TYPE_PCCAFE_CLAN == m_ui8ChannelType))	&&
			IsMatchMaking() )
		{
			if( FALSE == pInfoJoin->bMatch )
			{
				return EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER;
			}
		}
		//1. 비밀방 비밀번호 검사. ( 초대가 아닐 경우 )
		if( 1 != pInfoJoin->cForce && GET_ROOM_INFO_PRIVATE_MODE( m_Info._InfoBasic._InfoFlag ) )
		{
			for( INT32 i = 0 ; i < NET_ROOM_PW ; i++ )
			{
				if( m_strPasswd[i] != pInfoJoin->szRoomPw[i] )
				{
					return EVENT_ERROR_EVENT_JOIN_ROOM_WRONG_PASSWORD;
				}
			}
		}

	}

	// 강퇴당한 유저였는지 확인 합니다. 어떤 search가 빠를까요.. 10개 밖에 안되므로 우선은~~
	bool bVoteUser = false;
	for( INT32 i = 0 ; i < KICK_LIST_IN_ROOM_MAX ; i++ )
	{
		if( m_aKickUserList[i] == pSession->m_i64UID )
		{
			bVoteUser = true;
			break;
		}
	}

	if( bVoteUser )	return EVENT_ERROR_ROOM_JOIN_VOTE;

	// 슬롯번호 얻기(함수에서 m_csSession 사용)
	UINT8 ui8SlotState = (UINT8)GetStateSub( pSession->GetSubTask());
	INT32 i32Slot = _AllocateSlot( pInfoJoin->wMatchTeam, pSession->m_UserInfoBasic.m_ui32Clanidx, ui8SlotState );

	// 남는 슬롯이 있는지 체크
	if( EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL == i32Slot )	
	{
		return EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL;
	}

	// 해당 슬롯에 정보를 체워줍니다. SLOT_STATE는 _AllocateSlot 함수에서 채움.
	m_Info._pSession[i32Slot]					= pSession;						
	m_Info._aSlotInfo[i32Slot]._Rank			= (UINT8)pSession->m_ui32ViewRank;
	m_Info._aSlotInfo[i32Slot]._pcCafe			= pSession->m_UserInfoBasic.m_ui8PCCafe;
	m_Info._aSlotInfo[i32Slot]._eSportID		= pSession->m_UserInfoBasic.m_ui8ESportID;
	m_Info._aSlotInfo[i32Slot]._clanidx			= pSession->m_UserInfoBasic.m_ui32Clanidx;
	m_Info._aSlotInfo[i32Slot]._clanstate		= pSession->m_UserInfoBasic.m_ui32ClanState;
	m_Info._aSlotInfo[i32Slot]._clanExpRank		= pSession->m_UserInfoBasic.m_ui8ClanExpRank;
	m_Info._aSlotInfo[i32Slot]._clanMark		= pSession->m_UserInfoBasic.m_ui32ClanMark;
	m_Info._aSlotInfo[i32Slot]._ui8NationalCode = pSession->m_ui8Country; 
	m_Info._aSlotInfo[i32Slot]._Item			= pSession->GetUseItemInRoom();					// 아이템 정보 담기	

	m_stRoomUserInfo[i32Slot]._ui8Ping			= pSession->m_ui8Ping;

	i3mem::Copy( m_Info._aSlotInfo[i32Slot]._ClanName, pSession->m_UserInfoBasic.m_strClanName, NET_CLAN_NAME_SIZE );

	pSession->m_i32RoomIdx						= m_Info._InfoBasic._Idx; 
	pSession->m_i32SlotIdx						= i32Slot; 
	pSession->m_pRoom							= this; 
	pSession->m_bForceRoomLeave					= FALSE; 
	pSession->m_ui8LoadingStart					= LOAD_START_ENTER;

	ITEM_INFO CharItemInfo;

	if (pSession->m_MultiSlot.GetCharEquipment((INT32)pInfoJoin->m_ui8CharMainSlotIdx, CHAR_EQUIPMENT_CHARA, &CharItemInfo))
	{
		if (pSession->m_pDesign->SetTeamSlot(pSession->m_MultiSlot, pInfoJoin->m_ui8CharMainSlotIdx, CharItemInfo.m_TItemID))
		{
			pSession->ChangeDataBaseInfo(SAVE_DATABASE_CHARA_COMMON);
		}
	}

	// 인벤토리 정리작업
	pSession->CheckInventory_MAU(i32ThreadIdx); 

	// 다른 서버에 알림	
	g_pModuleMessenger->MessengerSendFriendRoomEnter( i32ThreadIdx, pSession, (UINT8)m_Info._ui32ChannelNum, (UINT16)m_Info._InfoBasic._Idx); 
	g_pModuleClan->ClanSendRoomEnter( i32ThreadIdx, pSession, m_Info._InfoBasic._Idx, IsMatchMaking() );
	g_pModuleTrans->AuthSendRoomEnter( i32ThreadIdx, pSession );
		
	pSession->SetPosition(GAME_TASK_READY_ROOM);

	// 새로 입장한 유저에게 보냅니다. ( 기존 방 인원들 슬롯 정보 )
	PACKET_ROOM_JOIN_ACK Packet;
	Packet.m_TResult = EVENT_ERROR_SUCCESS;
	Packet.m_ui8Slot = (UINT8)i32Slot;
	Packet.m_i8StartCountDown = m_i8StartCountdown;
	Packet.m_ui8MainSlotIdx = (UINT8)m_Info._i32MainSlotIdx;
	Packet.m_ui8PersonNow = m_Info._InfoBasic._PersonNow;
	Packet.m_i8EnemyAiCount = m_Info.m_i8enemyAiCount;
	Packet.m_i8DifficultyLevel = m_Info.m_i8difficultyLevel;
	i3mem::Copy(&Packet.m_RoomInfoBasic, &m_Info._InfoBasic, sizeof(ROOM_INFO_BASIC));
	i3mem::Copy(&Packet.m_RoomInfoAdd, &m_Info._InfoAdd, sizeof(ROOM_INFO_ADD));
	Packet.m_strRoomPasswd.SetValue(m_strPasswd, 0, NET_ROOM_PW);
	Packet.m_ui8EnterType = pInfoJoin->m_ui8EnterType;

	S2_SLOT_TOTAL_INFO* pPacketSlot;
	CUserSession*		pSlotSession;
	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		pPacketSlot = &Packet.m_aSlotTotalInfo[i];
		pSlotSession = m_Info._pSession[i];

		i3mem::Copy(pPacketSlot, &m_Info._aSlotInfo[i], sizeof(SLOT_INFO));
		pPacketSlot->m_ui8SlotIdx = (UINT8)i;
		pPacketSlot->m_ui8NickColor = pSlotSession->m_UserInfoInvItemData._ui8NickColor;
		pPacketSlot->m_ui16MuzzleFlashColor = pSlotSession->m_UserInfoInvItemData._ui16MuzzleFlashColor;
		pPacketSlot->m_ui8MerClass = pSlotSession->GetMerClass();
		pPacketSlot->m_ui8MerRank = pSlotSession->GetMerRank();
		i3String::Copy(pPacketSlot->m_strNickName, pSlotSession->GetNickName(), NET_NICK_NAME_SIZE);

	}
	pSession->S2MOSendEvent(&Packet);
	//
	// 기존 방 인원들에게 보냅니다.( 새로 입장한 유저의 슬롯 정보 )
	PACKET_ROOM_GET_SLOTONEINFO_ACK TransPacket;
	pPacketSlot = &TransPacket.m_SlotTotalInfo;

	i3mem::Copy( pPacketSlot, &m_Info._aSlotInfo[i32Slot], sizeof(SLOT_INFO) );
	
	pPacketSlot->m_ui8SlotIdx			= (UINT8)i32Slot;
	pPacketSlot->m_ui8NickColor			= pSession->m_UserInfoInvItemData._ui8NickColor;
	pPacketSlot->m_ui16MuzzleFlashColor = pSession->m_UserInfoInvItemData._ui16MuzzleFlashColor;
	pPacketSlot->m_ui8MerClass			= pSession->GetMerClass();
	pPacketSlot->m_ui8MerRank			= pSession->GetMerRank();
	i3String::Copy( pPacketSlot->m_strNickName, pSession->GetNickName(), NET_NICK_NAME_SIZE );

	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( m_Info._aSlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;
		if( i32Slot == i )										continue;

		m_Info._pSession[i]->S2MOSendEvent( &TransPacket );
	}
	
	return i32Slot; 
}

/*-----------------------------------------------------------
Name : Leave
Desc : 레디룸에서 로비로 나갈때 
// 직접 사용하지 마시고 RoomManager::OnLeaveRoom 로 호출 해주세요.
-----------------------------------------------------------*/
INT32 CRoom::LeaveLobby_MU( CUserSession * pSession, INT32 i32WorkThread, INT32 i32EndType )
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;
	if( 0 > i32SlotIdx )											return EVENT_ERROR_FAIL;
	if( SLOT_MAX_COUNT <= i32SlotIdx )								return EVENT_ERROR_FAIL;

	// 게임 중이라면 게임을 먼저 종료 시킵니다.
	_GiveUpBattle( i32WorkThread, pSession, FALSE, i32EndType ); 

	// 방에서 나가기
	if( m_Info._aSlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP )		return EVENT_ERROR_FAIL;	// 방에 없는 유저 (State <= SLOT_STATE_CLOSE 로 체크해야 맞으나 속도상 이득을 위해.. )
	 
	INT32 i32Rv = EVENT_ERROR_SUCCESS;

	m_csSession.Enter();
	{
		if( m_Info._i32MainSlotIdx == i32SlotIdx )//방장입니다. 
		{	//1. 방장 권한을 다른 사람에게 넘겨요. 
			if( SERVER_VALUE_UNKNOWN == _ChangeMain( SLOT_STATE_CLOSE ) )
			{//방삭제 되었습니다. 					
				i32Rv = DESTROY_ROOM;
			}
		}
		pSession->m_ui8Ping = m_stRoomUserInfo[i32SlotIdx]._ui8Ping;	// 핑을 넣어준다.

		// 방의 인원을 차감해 줍니다.
		m_Info._InfoBasic._PersonNow--;
		if ( m_Info._InfoBasic._PersonNow > 250 )
		{
			// 255 가 나올 경우 로그 기록. 2014.04.15 노준영
			g_pLog->WriteLogSession( pSession, L"LeaveLobby_MU Underflow / TIdx:%d / PN:%d / PM:%d / UID:%I64d / Ch:%d / RoomIdx:%d / Main:%d / Slot:%d / SlotState:%d / EndType:%d / Rv:%x",
				i32WorkThread,
				m_Info._InfoBasic._PersonNow, 
				m_Info._InfoBasic._PersonPlayerMax, 
				pSession->m_i64UID,
				pSession->m_i32ChannelNum,
				pSession->m_i32RoomIdx,
				m_Info._i32MainSlotIdx,
				i32SlotIdx,
				m_Info._aSlotInfo[i32SlotIdx]._State,
				i32EndType,
				i32Rv );

			m_Info._InfoBasic._PersonNow = 0;	// 예외처리 합니다.
		}

		// 방 삭제가 아니면 해당 슬롯을 초기화 해 줍니다.
		if( DESTROY_ROOM != i32Rv )
		{
			m_Info._aSlotInfo[i32SlotIdx].Reset( SLOT_STATE_EMPTY );
			m_stRoomUserInfo[i32SlotIdx].Reset();

			// 누가 나간다면 방장 검색 요청 초기화 합니다.
			for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if (m_Info._aSlotInfo[i]._State <= SLOT_STATE_CLOSE)	continue;
				if (i32SlotIdx == i)									continue;
				m_stRoomUserInfo[i]._bChangeMain = false;
			}
			_SendSlotInfo( NULL );
		}
	}
	m_csSession.Leave();

	g_pModuleMessenger->MessengerSendFriendRoomLeave(i32WorkThread, pSession);

	switch( m_ui8ChannelType )
	{
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN: 
		if( IsMatchMaking() )
		{
			g_pModuleClan->SendClanMatchLeaveRoom(i32WorkThread, pSession, m_TMatchingID, i32Rv==DESTROY_ROOM ? true:false, m_Info._InfoBasic._State );
		}
		break;
	}

	pSession->m_i32RoomIdx = SERVER_VALUE_UNKNOWN; 
	pSession->m_i32SlotIdx = SERVER_VALUE_UNKNOWN;
	pSession->SetPosition(GAME_TASK_LOBBY);

	return i32Rv; 
}

/*-----------------------------------------------------------
Name : 
Desc : 스피드 핵 유저 판단 하기 
-----------------------------------------------------------*/
BOOL CRoom::CheckGameTime(CUserSession * pSession, INT32 i32GameTime, UINT8 ui8Ping, UINT16 ui16PingTime, UINT8 ui8RoundCount  )
{
	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: break; 
	case BATTLE_SERVER_TYPE_DEDICATION	:
		if (!IsChallengeMode())
		{
			if (0 < ui8Ping && 6 > ui8Ping)
			{
				m_stRoomUserInfo[pSession->m_i32SlotIdx]._ui8Ping = ui8Ping;

				if (PING_MAX_TIME >= ui16PingTime)
				{
					pSession->AddPingTime(ui16PingTime);
				}
			}
		}
		break; 
	default: return EVENT_ERROR_ROOM;	break; 
	}
	
	if( ROOM_STATE_BATTLE != m_Info._InfoBasic._State )	return TRUE;

	UINT8	ui8EndTimeIdx	= (m_Info._InfoAdd._SubType & 0xF0) >> 4; 
	INT32	i32RemainTime	= 0;

	switch( m_Info._i32StageType )
	{
	case STAGE_MODE_CONVOY		:
	case STAGE_MODE_ESCAPE			: 
	case STAGE_MODE_ANNIHILATION	:
	case STAGE_MODE_BOMB			:
	case STAGE_MODE_DESTROY			:
	case STAGE_MODE_DEFENCE			:
		if(m_Info._i32MiStateType != MISSION_STATE_BATTLE) return TRUE;
	case STAGE_MODE_DEATHMATCH		:
		i32RemainTime	= static_cast<INT32>( g_TeamTimeTable[ui8EndTimeIdx] - (GetRoomBattleTime() - m_Info._ui32RTRoundStart) );
		break;
	}

	INT16 i16RoundDiff = static_cast<INT16>( m_Info._InfoAdd._NowRoundCount - ui8RoundCount );

	//다음라운드가 이미 시작한 상태에서 10초동안은 이전라운드에 보낸 TIMESYNC인 경우 무시합니다.
	if ( i16RoundDiff == 1 && GetRoomBattleTime() - m_Info._ui32RTRoundStart < 10 ) return TRUE;

	//라운드 차이가 나는 경우 해킹으로 간주합니다.
	//if ( i16RoundDiff != 0 ) return FALSE;

	//I3TRACE( "Check Time : %d, %d\n", iRemainTime, GameTime );
	//10초에서 20초로 변경했습니다. 0923		20초에서 30초로 변경 1008
	//유저의 시간이 더빠르면 처리 합니다. 
	INT32 i32Diff = i32RemainTime - i32GameTime;
	//if( 0 > i32Diff )			i32Diff = -i32Diff;	// 절대값으로 변경.
	if( i32Diff > 30 )			return FALSE;

	
	return TRUE;
}

void CRoom::_UserPenalty( INT32 i32WorkIdx, CUserSession * pSession, INT32 i32EndType, BOOL bFreeMove )
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx; 
	switch( m_ui8ChannelType )
	{
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN		:
		{	// 클랜 채널이라면 클랜 결과를 클랜서버에 우선 전달한다.
			CLAN_RESULT			ClanResult			= {0,};
			CLAN_DETAIL_RESULT	ClanDetailResult	= {0,};
			CLAN_RESULT_USER	ClanResultUser		= {0,};
			
			ClanResultUser._i64UID	= pSession->m_i64UID;
			ClanResultUser._WinLose = (UINT8)ROOM_ENDTYPE_LOSE;
			ClanResultUser._Death	= 0;
			ClanResultUser._Exp		= 0;
			ClanResultUser._Head	= 0;
			ClanResultUser._Kill	= 0;
			ClanResultUser._Assist  = 0;
			ClanResultUser._ClanIdx	= pSession->m_UserInfoBasic.m_ui32Clanidx;
			ClanResultUser.m_TMerID = pSession->GetMercenaryId();

			// 챌린지모드와 튜토리얼의 경우 전적을 반영하지 않는다.
			if ( !IsChallengeMode() && !IsTutorialMode() )
			{
				g_pModuleClan->ClanSendBattleResult( i32WorkIdx, 0, &ClanResult, &ClanDetailResult, 1, &ClanResultUser );
			}
		}
		// break; <-- 없는게 맞음
	default:

		// 어뷰징 체크 - 게임 도중에 나간유저의 배틀 시작 시간 초기화
		pSession->m_ui32RTBattleStart = 0;
		pSession->m_ui32BattleStartInvenTime = 0;
		pSession->m_ui32PlayTime = 0;

		
		// 자유 이동권이 있거나 전장이동권이 있으면 패널티 제로 & 킬뎃은 계산
		if(  (NULL != pSession->GetItemByItemID(CASHITEM_ITEMID_FREE_MOVE)) ||
			 (NULL != pSession->GetItemByItemID(CASHITEM_ITEMID_FREE_MOVE_EV)) || bFreeMove)
		{
			if( (m_Info._aui16AccExp[i32SlotIdx] == 0) && (m_Info._aM_UserScore[i32SlotIdx].m_DeathCount == 0) )
			{//전적이 하나도 없는 유저
				m_Info._aui16AccExp[i32SlotIdx]	= 0;
				m_Info._aui16AccPoint[i32SlotIdx]= 0;
			}

			// 강제퇴장, 전장이동권 사용시만 일일 플레이 타임 적용
			DaliyPlayTime( i32WorkIdx, i32SlotIdx, bFreeMove );
						
		}
		else
		{	
			// 챌린지모드와 튜토리얼의 경우 이탈횟수 계산하지 않는다.
			if ( !IsChallengeMode() &&
				!IsTutorialMode() )
			{
				UINT32 ui32Penalty = 0;

				// 패널티를 받아야한다
				switch( m_Info._pSession[i32SlotIdx]->m_ui8LocaleCode )
				{				
				case LOCALE_CODE_MATCH			:		ui32Penalty = 0;		break;
				case LOCALE_CODE_CHINA			:		ui32Penalty = 50;		break;
				default							:		ui32Penalty = 200;		break;
				}

				if(pSession->m_UserInfoBasic.m_ui32Point > ui32Penalty)
				{
					pSession->m_UserInfoBasic.m_ui32Point = pSession->m_UserInfoBasic.m_ui32Point - ui32Penalty;							
				}
				else
				{
					ui32Penalty = pSession->m_UserInfoBasic.m_ui32Point;
					pSession->m_UserInfoBasic.m_ui32Point = 0;
				}

				g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_PENALTY, pSession, &ui32Penalty );

				pSession->m_UserInfoRecord._dis++; 
				pSession->m_UserInfoRecord._sdis++;
			}
		}
		break;		
	}

	pSession->ChangeDataBaseInfo( SAVE_DATABASE_BASEINFO | SAVE_DATABASE_RECORD );
	return; 
}

INT32 CRoom::GiveUpBattle_U( INT32 i32WorkThreadIdx, CUserSession * pSession, BOOL bFreeMove)//워크 쓰레드에서만 호출 됩니다. 
{	
	return _GiveUpBattle( i32WorkThreadIdx, pSession, bFreeMove, ROOM_ENDTYPE_GIVEUP ); 
}

void CRoom::KickUser_Dedi( CUserSession * pSession )
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx; 

	if( 0 > i32SlotIdx )											return;			// 올바른 슬롯인지.
	if( SLOT_MAX_COUNT <= i32SlotIdx )								return;

	_GiveUpBattle( MAIN_THREAD_IDX, pSession, FALSE, ROOM_ENDTYPE_DEDI_KICK ); 
}

/*-----------------------------------------------------------
Name : GiveUpBattle
Desc : battle중 방으로 나갈때  추후 다시 한번 고려해야할 패킷 던지는것과..
-----------------------------------------------------------*/
INT32 CRoom::_GiveUpBattle( INT32 i32WorkIdx, CUserSession * pSession, BOOL bFreeMove, INT32 i32EndType )
{
	// SlotIdx 가 올바른 값인지는 미리 계산하고 들어와야 합니다.
	// -> GM_Kick 등 사용하는 다른 부분들에서 Room 의 SlotInfo 배열에 참조되는 값은 비교하나
	// 막상 SlotInfo 에서 얻어온 Session 의 SlotIdx 는 검사를 안하고 있었음. ( 막상 Session 의 SlotIdx 를 사용함 )
	// 그렇기에 일일이 검사하는 부분을 추가하지 않기 위해 이곳에 검사하는 예외처리를 합니다.
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;
	if( 0 > i32SlotIdx || SLOT_MAX_COUNT <= i32SlotIdx )				return -1;			// SlotIdx 범위 확인
	if( SLOT_STATE_LOAD > m_Info._aSlotInfo[i32SlotIdx]._State )		return -1;			// 게임중인 유저인지

	if( SLOT_STATE_BATTLE ==  m_Info._aSlotInfo[i32SlotIdx]._State )
	{
		// 배틀 종료 시 배틀에 있던 유저수 저장
		pSession->SetLastUserCount( &m_Info );
		pSession->m_ui32LastPlayTime	= GetRoomBattleTime() - pSession->m_ui32RTBattleStart;

		if( ROOM_ENDTYPE_VOTE		!= i32EndType &&
			ROOM_ENDTYPE_DEDI_KICK	!= i32EndType )
		{	// 강퇴로 나가는 유저는 페널티를 적용하지 않습니다.
			_UserPenalty( i32WorkIdx, pSession, i32EndType, bFreeMove );
			pSession->SetMerPenaltyFlag(true);	// 용병 패널티 적용
		}
		else
		{
			pSession->m_ui32PlayTime							= GetRoomBattleTime() - pSession->m_ui32RTBattleStart;
			pSession->m_UserInfoServer.m_ui32LoginBattleTime	+= pSession->m_ui32PlayTime;
			pSession->m_UserInfoServer.m_ui32TotalBattleTime	+= pSession->m_ui32PlayTime;
		}

		// 사용한 무기를 인증하는 루틴을 보냅니다.
		_SendUsedWeapon( i32WorkIdx, pSession );

		g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_R_BATTLE_END, pSession, this, &i32EndType );
		m_Info.m_ui8PlayRoundCound[i32SlotIdx] = 0; //플레이한 라운드 초기화
	}

	m_csSession.Enter();
	{
		pSession->m_bIsBattleComplete					= FALSE;
		pSession->m_ui8LoadingStart						= LOAD_START_GIVEUP_BATTLE_END;
		m_Info._aSlotInfo[i32SlotIdx]._State			= SLOT_STATE_NORMAL; 
		_SendSlotInfo(NULL);

		m_Info._ui32FirstBattleStartUser				&= ~(0x1 << i32SlotIdx);
		m_Info._ui16RoundStartUser						=  (UINT16)( (UINT32)m_Info._ui16RoundStartUser & ~(0x01UL << i32SlotIdx));
		if(FALSE == m_Info._aSlotInfo[i32SlotIdx]._bIsGmObserverMode) { m_Info._ui16BattleUser =  (UINT16)( (UINT32)m_Info._ui16BattleUser & ~(0x01UL << i32SlotIdx)); }
		m_ui16QuestEndFlag								=  (UINT16)( (UINT32)m_ui16QuestEndFlag			& ~(0x01UL << i32SlotIdx));

		//Set Record
		m_Info._aM_UserScore[i32SlotIdx].m_DeathCount	= 0;
		m_Info._aM_UserScore[i32SlotIdx].m_KillCount	= 0;
		m_Info._aM_UserScore[i32SlotIdx].m_AssistCount = 0;
		m_Info._aUserScoreRound[i32SlotIdx].m_DeathCount= 0;
		m_Info._aUserScoreRound[i32SlotIdx].m_KillCount	= 0;
		m_Info._aUserScoreRound[i32SlotIdx].m_AssistCount = 0;
		m_Info._aui16M_Headshot[i32SlotIdx]				= 0;

		m_Info._aui16AccExp[i32SlotIdx]					= 0;
		m_Info._aui16AccPoint[i32SlotIdx]				= 0;
		m_Info._aui16AccChallengeScore[i32SlotIdx]		= 0;
		m_Info._aui8AccMissionCount[i32SlotIdx]			= 0;
		m_Info._aui8A_Mission[i32SlotIdx]				= 0;
		m_Info.m_aui8A_KillCount[i32SlotIdx]			= 0;
		m_Info.m_aui8A_HeadShot[i32SlotIdx]				= 0;
		m_Info.m_aui8A_SlugShot[i32SlotIdx]				= 0;
		m_Info._aui16Acc_KillExp[i32SlotIdx]			= 0;
		m_Info._aui16Acc_AssistExp[i32SlotIdx] = 0;
		m_Info._abDinoStateUpdate[i32SlotIdx]			= false;
		m_Info._aui32DInoCharInfo[i32SlotIdx]			= 0;
		m_Info._aui32RespawnCount[i32SlotIdx]			= 0;
		m_ui8MultiKillCount[i32SlotIdx]					= 0;

		m_pRoomMode->UserGiveUpBattle( i32SlotIdx );

		_RemoveTimeOutFlag( i32SlotIdx );						//타임 아웃이 걸려 있으면

		if ( IsChallengeMode() )
		{
			UINT16 ui16TeamMask = ( (m_Info._i32MainSlotIdx % TEAM_COUNT ) == TEAM_RED ) ? BLUE_TEAM_MASK : RED_TEAM_MASK;

			if(FALSE == m_Info._aSlotInfo[i32SlotIdx]._bIsGmObserverMode)
			{
				m_Info._ui16BattleUser |= ui16TeamMask;
			}
		}

		if( (m_Info._ui16BattleUser & RED_TEAM_MASK) && (m_Info._ui16BattleUser & BLUE_TEAM_MASK) )
		{
			//방장을 변경
			if( m_Info._i32MainSlotIdx == i32SlotIdx )
			{
				if( _ChangeMain( SLOT_STATE_LOAD ) == SERVER_VALUE_UNKNOWN )
				{	//방장을 할 사람이 없다.	//Battle 파기
					//1인용으로 풀었을때만 사용합니다. 정상적으로 들어올수 없음. 
					m_bIsBattle = FALSE; // 이전에 걸려야 합니다. 
				}
				else
				{//방장이 변경 
					_SendMainChange();
				}
			}
		}
		else
		{
			//내부에서 상태 변경.
			m_bIsBattle = FALSE; 
		}

		// 성백 배틀 : 유저 나갈때 
		if( m_bIsBattleRoomCreated && isUseBattleServer() )
		{
			g_pModuleCast->BattleUserLeave( i32WorkIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT8)i32SlotIdx, pSession );
		}

		I3ASSERT( i32SlotIdx != -1);

		//SendData
		i3NetworkPacket Packet( PROTOCOL_BATTLE_GIVEUPBATTLE_ACK );
		Packet.WriteData( &i32SlotIdx,	sizeof(INT32));
		Packet.WriteData( &pSession->m_UserInfoBasic.m_ui32Exp,			sizeof(UINT32));			//경험치
		Packet.WriteData( &pSession->m_UserInfoBasic.m_ui32Rank,		sizeof(UINT32));			//계급
		Packet.WriteData( &pSession->m_UserInfoBasic.m_ui32Point,		sizeof(UINT32));			//포인트
		Packet.WriteData( &pSession->m_UserInfoRecord._dis,				sizeof(UINT32));			//DisConnect 
		Packet.WriteData( &pSession->m_UserInfoRecord._sdis,			sizeof(UINT32));			//DisConnect Count
		Packet.WriteData( &pSession->m_UserInfoDaily._DGPlaytime,		sizeof(UINT32));			
		Packet.WriteData( &pSession->m_UserInfoDaily._ui8GetItem,		sizeof(UINT8));				
		//Packet.WriteData( &pSession->m_UserChallengeIngInfo,		sizeof(CHALLENGE_ING_INFO));//도전과제 데이터
		m_pRoomMode->AddGiveUpBattlePacket( &Packet, pSession );
		_SendPacketAllPlayer(&Packet);
	}
	m_csSession.Leave();

	pSession->SetPosition(GAME_TASK_READY_ROOM);

	if ( EV_SUCCESSED( GMResume( i32WorkIdx, i32SlotIdx ) ) )
		SendGMResume();

	return EVENT_ERROR_SUCCESS; 
}

void CRoom::SendGMResume()
{
	static INT32 i32Ret = EVENT_ERROR_SUCCESS;
	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_GM_RESUME_ACK );
	SendPacket.WriteData( &i32Ret, sizeof( INT32 ) );
	_SendPacketBattlePlayer( &SendPacket );
}

/*-----------------------------------------------------------
Name : Ready Battle 
Desc : 레디룸에서 Battle을 시작할때 
-----------------------------------------------------------*/
INT32 CRoom::ReadyBattle_U(CUserSession * pSession)
{	
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 	
	INT32	i32Rv = EVENT_ERROR_SUCCESS; 

	// 이미 방이 시작했는데 레디 취소 버튼을 눌렀을 경우 이벤트를 무시합니다.
	if( (ROOM_STATE_COUNTDOWN_R < m_Info._InfoBasic._State) && (m_Info._aSlotInfo[i32SlotIdx]._State >= SLOT_STATE_READY) )
		return EVENT_ERROR_SUCCESS;

	if( m_Info._i32MainSlotIdx == i32SlotIdx )
	{// 방장일 경우		
		i32Rv = CheckUserReady();
		BOOL bStartLoading = EV_SUCCESSED( i32Rv );

		//카운트 중간에 멈춤. 
		if( ROOM_STATE_COUNTDOWN_R == m_Info._InfoBasic._State )
		{
			//_SendSlotInfo( NULL ); //<- 이거 필요할까? 
			bStartLoading = FALSE;
		}

		if( bStartLoading )
		{
			i32Rv = EVENT_ERROR_SUCCESS;
			m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_READY;

			// 인원수 제한 게임일 경우 초과 유저에 대해 Ready 상태를 푼다
			UnreadySlot();
			
			if( FALSE == IsChallengeMode() )
			{
				// TeamBalance - StartBattle
				if (1 == m_Info._InfoAdd._LockTeamBalance)
				{
					ChangeTeamForBalance();
				}
				else if (2 == m_Info._InfoAdd._LockTeamBalance)
				{
					ChangeTeamForBalance2();
				}
			}
			_SendSlotInfo( NULL );
		}
		else
		{
			m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
			PACKET_BATTLE_READYBATTLE_ACK Packet;
			Packet.m_TResult = i32Rv;
			pSession->S2MOSendEvent( &Packet );
		}
	}
	else
	{// 방장이 아닐 경우
		//같이 게임을 할 수 있는 유저 입니다. 
		if( m_Info._InfoBasic._State <= ROOM_STATE_COUNTDOWN_R )
		{
			//상태 변경 
			if( m_Info._aSlotInfo[i32SlotIdx]._State == SLOT_STATE_READY )
				m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
			else
				m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_READY;

			_SendSlotInfo( NULL );
		}
		else
		{//난입 입니다. 
			switch( g_pContextMain->m_pChannelType[m_Info._ui32ChannelNum] )
			{
			case CHANNEL_TYPE_PCCAFE_CLAN:
			case CHANNEL_TYPE_CLAN		: 
				{
					if( false == IsMatchMaking() )
					{
						if( m_Info._aui32BattleClanIdx[ i32SlotIdx % TEAM_COUNT ] != pSession->GetUserInfoBasic()->m_ui32Clanidx )
						{
							PACKET_BATTLE_READYBATTLE_ACK Packet;
							Packet.m_TResult = EVENT_ERROR_NO_CLAN_TEAM;
							pSession->S2MOSendEvent( &Packet );
							return i32Rv; 
						}
					}
					// 매치메이킹도 난입 가능하게 기획 수정합니다. 2015.05.16 어깨깡패
					//else return i32Rv;	// 매치 메이킹은 난입 불가.
				}
				break;
			default:
				break;
			}

			// 4대4의 경우 초과 유저에 대해 난입 금지
			if ( FALSE == IsAllowJoin(pSession))
			{
				PACKET_BATTLE_READYBATTLE_ACK Packet;	// 이부분은 수정되어야 할것 같습니다. 
				Packet.m_TResult = EVENT_ERROR_NO_CLAN_TEAM;
				pSession->S2MOSendEvent( &Packet );
				return i32Rv; 
			}

			//게임 시작 
			m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_LOAD;
			if( !IsChallengeMode() )
			{
				// TeamBalance - 난입
				if (0 < m_Info._InfoAdd._LockTeamBalance)
				{
					ChangeTeamForBalance(pSession, m_Info._InfoAdd._LockTeamBalance);
					i32SlotIdx = pSession->m_i32SlotIdx;
				}
			}
			
			//컨트롤 서버에 패킷을 보냅니다. 
			if( isUseBattleServer() )
			{
				g_pModuleCast->BattleUserEnter(pSession->m_WorkThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT8)i32SlotIdx, m_Info._aSlotInfo[i32SlotIdx]._Item, pSession); 
			}
			
			_SendSlotInfo( NULL );	//유저들의 정보를 보낸다. 

			PACKET_BATTLE_READYBATTLE_ACK ReadyPacket;
			ReadyPacket.m_TResult		= SLOT_STATE_LOAD;
			pSession->S2MOSendEvent( &ReadyPacket );

			// 유저 스코프타입 세팅.
			_SetUsersScopeType();

			// 게임 시작 패킷 전달
			_SendPacketStartGame( pSession );

			// 난입 유저에 대한 패킷 전달
			S2MO_CHAR_BASE_INFO		CharBaseInfo;
			ITEM_INFO				CharItemInfo;
			INT32					i32Team = GetNowTeam(i32SlotIdx);

			PACKET_BATTLE_START_GAME_TRANS_ACK TransPacket;
			S2_CHAR_BATTLE_INFO*	pBattleEquip = &TransPacket.m_stBattleInfo;
			pBattleEquip->m_ui8SlotIdx				= (UINT8)i32SlotIdx;
			pSession->m_pDesign->GetMainCharBaseInfo( &pSession->m_MultiSlot, &CharBaseInfo, i32Team );
			pSession->m_pDesign->GetMainCharEquipment(&pSession->m_MultiSlot, CHAR_EQUIPMENT_CHARA, &CharItemInfo, i32Team );
			pBattleEquip->m_TCharItemID				= CharItemInfo.m_TItemID;
			TransPacket.m_ui8UserScopeType			= pSession->m_ui8UserScopeType;

			for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; ++i )
			{
				pBattleEquip->m_TBattleEquipWeapon[i] = pSession->m_aTBattleEquipWeapon[i].m_TItemID;
			}

			for( INT32 k = 0 ; k < CHAR_EQUIPMENT_PARTS_COUNT ; ++k )
			{
				pBattleEquip->m_TBattleEquipParts[k] = pSession->m_aTBattleEquipParts[k].m_TItemID;
			}
			i3mem::Copy( pBattleEquip->m_ui8UseWeaponEndurance,	pSession->m_aUseWeaponEndurance,	sizeof(UINT8)*CHAR_EQUIPMENT_WEAPON_COUNT );
			i3mem::Copy( pBattleEquip->m_ui8EquipUserTitle,		pSession->m_aui8EquipUserTitle,		sizeof(UINT8)*MAX_EQUIP_USER_TITLE );
			pSession->m_pDesign->GetMainCharSkillInfo(&pSession->m_MultiSlot, &pBattleEquip->m_SkillInfo, i32Team );

			_SendS2PacketAnotherPlayer( &TransPacket, pSession, SLOT_STATE_READY );
		}
	}

	return i32Rv; 
}

void CRoom::_SendPacketStartGame( CUserSession * pSession )
{
	S2_CHAR_BATTLE_INFO*	pBattleEquip;
	CUserSession *			pUserSession;
	ITEM_INFO				CharItemInfo;
	INT32					i32Team;

	PACKET_BATTLE_START_GAME_ACK Packet;
	Packet.m_ui32StageID	= m_Info._InfoBasic._StageID;
	Packet.m_aui8UsersScopeType = m_Info.m_aui8UsersScopeType;
	
	UINT8 ui8Count		= 0;
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_NORMAL < m_Info._aSlotInfo[i]._State )
		{
			pUserSession	= m_Info._pSession[i];
			pBattleEquip	= &Packet.m_stBattleInfo[ ui8Count ];

			i32Team = GetNowTeam(i);
			INT32 i32CharaSlotIdx = pUserSession->m_pDesign->GetCharaSlotByTeam(i32Team, pUserSession );

			pUserSession->m_MultiSlot.GetCharEquipment(i32CharaSlotIdx,  CHAR_EQUIPMENT_CHARA, &CharItemInfo );
			
			pBattleEquip->m_ui8SlotIdx				= (UINT8)i;
			pBattleEquip->m_TCharItemID				= CharItemInfo.m_TItemID;

			for( INT32 w = 0 ; w < CHAR_EQUIPMENT_WEAPON_COUNT ; ++w )
			{
				pBattleEquip->m_TBattleEquipWeapon[w] = pUserSession->m_aTBattleEquipWeapon[w].m_TItemID;
			}
			// vv3 - check
			for( INT32 p = 0 ; p < CHAR_EQUIPMENT_PARTS_COUNT ; ++p )
			{	
				pBattleEquip->m_TBattleEquipParts[p] = pUserSession->m_aTBattleEquipParts[p].m_TItemID;
			}
			
			i3mem::Copy( pBattleEquip->m_ui8UseWeaponEndurance,	pUserSession->m_aUseWeaponEndurance,	sizeof(UINT8)*CHAR_EQUIPMENT_WEAPON_COUNT );
			i3mem::Copy( pBattleEquip->m_ui8EquipUserTitle,		pUserSession->m_aui8EquipUserTitle,		sizeof(UINT8)*MAX_EQUIP_USER_TITLE );
			pUserSession->m_MultiSlot.GetCharSkillInfo(i32CharaSlotIdx, &pBattleEquip->m_SkillInfo );
			ui8Count++;
		}
	}
	if( pSession )	pSession->S2MOSendEvent( &Packet );
	else			_SendS2PacketAllPlayer( &Packet, SLOT_STATE_NORMAL );
}

void	CRoom::GM_KickUser_U( CUserSession * pSession, INT8 i8Slot )
{
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )		return; 
	if( i8Slot == pSession->m_i32SlotIdx )					return;
	if( NULL == m_Info._pSession[i8Slot] )					return;

	switch( m_Info._aSlotInfo[ i8Slot ]._State )
	{
	case SLOT_STATE_SHOP:
	case SLOT_STATE_INFO:
	case SLOT_STATE_CLAN:
	case SLOT_STATE_INVENTORY:
	case SLOT_STATE_NORMAL:
	case SLOT_STATE_READY:
	case SLOT_STATE_GACHA:
	case SLOT_STATE_GIFTSHOP:
		break;

	case SLOT_STATE_BATTLE:
		{
			// 로그 기록 - 서동권
			_GiveUpBattle( pSession->m_WorkThreadIdx, m_Info._pSession[i8Slot], FALSE, ROOM_ENDTYPE_GM_KICK );

			LOG_KICK	stLog;
			i3mem::FillZero(&stLog, sizeof(LOG_KICK));
			stLog.m_eType = USER_KICK_TYPE_GM;
			stLog.m_eReason = FORCEREMOVE_NONE;
			g_pModuleContextNC->LogSendMessage(pSession->m_WorkThreadIdx, LOG_MESSAGE_U_R_KICK_LEAVE_ROOM, pSession, &stLog );
		}
		break;

	default:
		return;
	}

	//강퇴합니다. 
	i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_KICK_PLAYER);  
	
	//GM에 의한 Kick여부를 확인하기 위해 bool값 변수를 추가로 보냅니다.
	bool			bIsGMKick	=	TRUE;
	SendPacket.WriteData( &bIsGMKick, sizeof(bool) );

	m_Info._pSession[i8Slot]->SendPacketMessage(&SendPacket); 
	m_Info._pSession[i8Slot]->m_bForceRoomLeave = TRUE;
}

void	CRoom::GM_ExitUser_U( CUserSession * pSession, INT8 i8Slot )
{
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )		return; 
	if( i8Slot == pSession->m_i32SlotIdx )					return;
	if( NULL == m_Info._pSession[i8Slot] )					return;

	m_Info._pSession[i8Slot]->_SendErrorMessage( pSession->m_WorkThreadIdx, EVENT_ERROR_GM_KICK_USER, PROTOCOL_ROOM_GM_EXIT_USER_REQ, KILL_REASON_HACK_PACKET );
}

void	CRoom::GM_BlockUser_U( CUserSession * pSession, INT8 i8Slot, const TTCHAR *strComment )
{
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )		return; 
	if( i8Slot == pSession->m_i32SlotIdx )					return;
	if( NULL == m_Info._pSession[i8Slot] )					return;

	// 블록 요청을 Trans 서버로 보냅니다.
	g_pModuleTrans->TransSendGMBlockUser(pSession->m_WorkThreadIdx, m_Info._pSession[i8Slot]->m_i64UID, strComment);
}

void	CRoom::GM_DestroyRoom_U( CUserSession * pSession )
{
	for(INT32 i = 0 ; i < SLOT_MAX_COUNT; i++)
	{
		switch( m_Info._aSlotInfo[ i ]._State )
		{
		case SLOT_STATE_SHOP:
		case SLOT_STATE_CLAN:
		case SLOT_STATE_INVENTORY:
		case SLOT_STATE_NORMAL:
		case SLOT_STATE_READY:
		case SLOT_STATE_GACHA:
		case SLOT_STATE_GIFTSHOP:
			{
			}
			break;
		case SLOT_STATE_BATTLE:
			{
				_GiveUpBattle( pSession->m_WorkThreadIdx, m_Info._pSession[ i ], FALSE, ROOM_ENDTYPE_GM_KICK );
			}
			break;
		default:
			return;
		}

		//강퇴합니다. 
		if( NULL != m_Info._pSession[ i ] )
		{
			i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_KICK_PLAYER);  

			//GM에 의한 Kick여부를 확인하기 위해 bool값 변수를 추가로 보냅니다.
			bool			bIsGMKick	=	TRUE;
			SendPacket.WriteData( &bIsGMKick, sizeof(bool) );

			m_Info._pSession[ i ]->SendPacketMessage(&SendPacket); 
			m_Info._pSession[ i ]->m_bForceRoomLeave = TRUE;
		}
	}
}


BOOL	CRoom::GetUserItemInfo( CUserSession * pSession, INT8 i8Slot )
{
	if( i8Slot == pSession->m_i32SlotIdx )						return FALSE;
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )			return FALSE;
	if( NULL == m_Info._pSession[i8Slot] )						return FALSE;
	if( SLOT_STATE_SHOP > m_Info._aSlotInfo[i8Slot]._State )	return FALSE;

	CUserSession * pSessionUser = m_Info._pSession[i8Slot];

	if( NULL != pSessionUser ) pSessionUser->SendMyItemInfo( pSession, VIEW_USER_ITEM_INFO_TYPE_ROOM );

	return TRUE;
}

/*-----------------------------------------------------------
Name : PreStart Battle 
Desc : 로딩이 마무리 되고 Client간 HolePunching을 합니다. 방장은 바로 게임 시작..클라이언트들은 로딩이 끝난후에 들어와야 한다. 
-----------------------------------------------------------*/
INT32 CRoom::PreStartBattle( CUserSession * pSession)
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx;

	// 로딩 유저가 아니면 패킷을 받을 수 없습니다.
	if( SLOT_STATE_LOAD != m_Info._aSlotInfo[i32SlotIdx]._State )
	{
		_SendStartBattle( pSession, EVENT_ERROR_FAIL );
		return 0;
	}

	//방장이 로딩이 완료후 홀펀칭이 가능한 시점.
	switch( m_Info._InfoBasic._State )
	{
	case ROOM_STATE_BATTLE_RESULT	:
	case ROOM_STATE_BATTLE_END	:
	case ROOM_STATE_READY		: 
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
		_SendSlotInfo( NULL );
		_SendPreStartBattle( pSession, FALSE);
		break; 
	case ROOM_STATE_LOADING: //방장이 로딩중이므로 기달려야 합니다.
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_PRESTART;
		_SendSlotInfo( NULL );
		break; 
	case ROOM_STATE_PRE_BATTLE	:
	case ROOM_STATE_COUNTDOWN_B	:
	case ROOM_STATE_BATTLE		:
		// 이부분 현재 상태를 체크 해야 함
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_PRESTART;
		_SendPreStartBattle( pSession, TRUE );
		_SendSlotInfo( NULL );
		break;
	}

	return 1;
}

/*-----------------------------------------------------------
Name : Start Battle 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::StartBattle_U(CUserSession * pSession)
{	
	if( NULL == pSession )	return EVENT_ERROR_FAIL;
	// 유저 플레이 타임을 기록하기 위한 부분 - 서동권
	pSession->m_ui32PlayTime				= 0;
	pSession->m_ui32RTBattleStart			= 0;
	//for( INT32 i = 0 ; i < WEAPON_USAGE_COUNT ; i++ )
	//{
	//	pSession->m_EnduranceUsedWeapon[i].m_ui32UsedTime	= g_pContextMain->GetServerTime();
	//}

	++pSession->m_ui32RoomBattleCount;

	pSession->m_ui32RTAccActualBattle		= 0;
	pSession->m_ui32RTRespawn				= 0;
	pSession->m_ui32ReVoteTime				= 0;
	pSession->m_ui32GuardTime				= 0;

	INT32 i32Slot = pSession->m_i32SlotIdx; 

	if( i32Slot >= SLOT_MAX_COUNT || i32Slot <= SERVER_VALUE_UNKNOWN )	return EVENT_ERROR_FAIL;

	m_bReSpawnFlag[i32Slot]	= false; 
	m_bDeathFlag[i32Slot]	= false;
	m_bAlive[i32Slot]		= false;
	m_bKickBattle[i32Slot]	= false;

	switch(m_Info._InfoBasic._State) 
	{
	case ROOM_STATE_PRE_BATTLE:
		{
			m_Info._aSlotInfo[i32Slot]._State = SLOT_STATE_BATTLE_READY;	//상태 변경 
			_SendSlotInfo(NULL);
		}
		break; 
	case ROOM_STATE_COUNTDOWN_B:
		{
			m_Info._aSlotInfo[i32Slot]._State = SLOT_STATE_BATTLE_READY;	//상태 변경 
			_SendSlotInfo(NULL);

			//난입 유저는 카운트 다운이 보이지 않습니다.
			//UINT8 ui8CountDownTime = (UINT8)(g_pContextMain->GetServerTime() - m_ui32ATCountDownB);
			//i3NetworkPacket CountDownPacket(PROTOCOL_BATTLE_COUNT_DOWN_ACK); 
			//CountDownPacket.WriteData( &ui8CountDownTime,	(sizeof(UINT8)));
			//m_Info._pSession[i32Slot]->SendPacketMessage( &CountDownPacket );
		}
		break;
	case ROOM_STATE_BATTLE:
		{
			if( m_Info._aSlotInfo[i32Slot]._State != SLOT_STATE_PRESTART )
			{
				m_Info._aSlotInfo[i32Slot]._State = SLOT_STATE_NORMAL;
				_SendSlotInfo(NULL);
				_SendStartBattle(m_Info._pSession[i32Slot], EVENT_ERROR_FAIL );
				break;
			}

			if(FALSE == m_Info._aSlotInfo[i32Slot]._bIsGmObserverMode)
			{
				m_Info._ui16BattleUser |= (0x01 << i32Slot);
			}
			m_Info._pSession[i32Slot]->m_ui32RTBattleCheck	= GetRoomBattleTime();
			m_Info._aSlotInfo[i32Slot]._State				= SLOT_STATE_BATTLE;
			i3mem::FillZero( m_Info._pSession[i32Slot]->m_aui8QuestCtxActiveNow, sizeof(UINT8)*MAX_QUEST_PER_CARD_NEW );
			m_Info._pSession[i32Slot]->m_ui16ActiveIndexCardNow = 0;
						
			MedalMissionCheck( pSession->m_WorkThreadIdx, pSession, MEDAL_MC_ACTION_ID_STAGE_INTERCEPT );					
			SetUserMissionCard( m_Info._pSession[i32Slot], ACTION_ID_STAGE_INTERCEPT );
			
			_SendSlotInfo(NULL);
			_SendStartBattle(m_Info._pSession[i32Slot], EVENT_ERROR_SUCCESS, true );
			g_pModuleContextNC->LogSendMessage( pSession->m_WorkThreadIdx, LOG_MESSAGE_U_R_BATTLE_START, pSession, this ); 

			//난입한 유저들에게는 점수도 보내 줘야 합니다. 
			{
				i3NetworkPacket RecordPacket(PROTOCOL_BATTLE_RECORD_ACK); 
				RecordPacket.WriteData( m_Info._aTeamScoreTotal,	(sizeof(SCORE_INFO) * TEAM_COUNT)); 
				RecordPacket.WriteData( m_Info._aM_UserScore,	(sizeof(SCORE_INFO) * SLOT_MAX_COUNT)); 
				pSession->SendPacketMessage(&RecordPacket);
			}
		}
		break; 
	default:
		{
			m_Info._aSlotInfo[i32Slot]._State = SLOT_STATE_NORMAL;
			_SendSlotInfo(NULL);
			_SendStartBattle(m_Info._pSession[i32Slot], EVENT_ERROR_FAIL );
		}
		break; 
	}

	return EVENT_ERROR_SUCCESS; 
}

void CRoom::MoveInventory(	CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 
	if(bEnter)
	{
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_INVENTORY;
	}
	else
	{
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
		m_Info._aSlotInfo[i32SlotIdx]._Item	= pSession->GetUseItemInRoom();					// 아이템 정보 담기	
	}
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveShop(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 
	if(bEnter)
	{
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_SHOP;
	}
	else
	{
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
		m_Info._aSlotInfo[i32SlotIdx]._Item	= pSession->GetUseItemInRoom();					// 아이템 정보 담기
	}
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveInfo(		CUserSession * pSession , BOOL bEnter )
{	
	INT32	i32SlotIdx = pSession->m_i32SlotIdx;

	m_Info._aSlotInfo[i32SlotIdx]._State = (UINT8)(bEnter ? SLOT_STATE_INFO : SLOT_STATE_NORMAL);
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveClan(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx;

	m_Info._aSlotInfo[i32SlotIdx]._State = (UINT8)(bEnter ? SLOT_STATE_CLAN : SLOT_STATE_NORMAL);
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveRS(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx;

	m_Info._aSlotInfo[i32SlotIdx]._State = (UINT8)(bEnter ? SLOT_STATE_GACHA : SLOT_STATE_NORMAL);
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveGiftShop(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 
	m_Info._aSlotInfo[i32SlotIdx]._State = (UINT8)(bEnter ? SLOT_STATE_GIFTSHOP : SLOT_STATE_NORMAL);
	_SendSlotInfo( NULL ); 
}


/*----------------------------------------------------------
Name : 
Desc : 룸에서 일반 유저 채팅 
----------------------------------------------------------*/
void CRoom::_SendPacketReadyRoomPlayer( i3NetworkPacket	* pPacket, INT32 i32TeamIdx )
{
	INT32 i32StartIdx = 0;
	INT32 i32Increase = 1;

	if(i32TeamIdx != SERVER_VALUE_UNKNOWN)
	{
		i32StartIdx = i32TeamIdx;
		i32Increase = 2;
	}

	for( INT32 i = i32StartIdx; i < SLOT_MAX_COUNT; i += i32Increase )
	{	// 클랜에 있을때는 클랜 체팅으로 인해 메시지를 보내지 않습니다.
		if( m_Info._aSlotInfo[i]._State == SLOT_STATE_CLAN ) continue;;

		if( (m_Info._aSlotInfo[i]._State > SLOT_STATE_CLOSE) && 
			(m_Info._aSlotInfo[i]._State < SLOT_STATE_LOAD) )
		{
			m_Info._pSession[i]->SendPacketMessage(pPacket);
		}
	}
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendPacketBattlePlayer( i3NetworkPacket * pPacket, INT32 i32TeamIdx )
{
	INT32 i; 

	if(i32TeamIdx == SERVER_VALUE_UNKNOWN)
	{
		for(i = 0; i < SLOT_MAX_COUNT; i ++)
		{
			if( m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE)
			{
				m_Info._pSession[i]->SendPacketMessage(pPacket);
			}
		}
	}else 
	{
		for(i = i32TeamIdx; i < SLOT_MAX_COUNT; i += 2)
		{
			if( m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE )
			{
				m_Info._pSession[i]->SendPacketMessage(pPacket);
			}
		}
	}

	return; 
}

void CRoom::SendPlayerInfo(	CUserSession * pSession, INT32 i32SlotIdx )
{
	i3NetworkPacket SendPacket( PROTOCOL_ROOM_GET_PLAYERINFO_ACK ); 

	PACKET_ROOM_GET_PLAYERINFO_ACK Packet;

	if( m_Info._aSlotInfo[i32SlotIdx]._State > SLOT_STATE_CLOSE)
	{		
		CUserSession * pInfoSession = m_Info._pSession[ i32SlotIdx ];

		Packet.m_TResult			= EVENT_ERROR_SUCCESS;
		Packet.m_ui8SlotIdx			= (UINT8)i32SlotIdx;
		Packet.m_UserBasicInfo		= *pInfoSession->GetUserInfoBasic(); 
		Packet.m_UserRecordInfo		= pInfoSession->m_UserInfoRecord;
		Packet.m_aui8EquipUserTitle.SetValue( pInfoSession->m_aui8EquipUserTitle, 0, MAX_EQUIP_USER_TITLE );
		pInfoSession->m_pDesign->GetMainCharEquipInfo(&pInfoSession->m_MultiSlot, &Packet.m_CharEquipInfo, i32SlotIdx % TEAM_COUNT);
		ITEM_INFO pCommonEquips[ CHAR_EQUIPMENT_COMMON_COUNT ];
		pInfoSession->m_MultiSlot.GetCommonEquipmentAll( pCommonEquips );
		Packet.m_aCommonEquips.SetValue( pCommonEquips, 0, CHAR_EQUIPMENT_COMMON_COUNT );
	}
	else
	{
		Packet.m_TResult		= SERVER_VALUE_UNKNOWN;
	}
	pSession->S2MOSendEvent( &Packet ); 

	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendPacketAllPlayer( i3NetworkPacket * pPacket, SLOT_STATE eLimitState )
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		if( eLimitState < m_Info._aSlotInfo[i]._State )
		{
			if( NULL == m_Info._pSession[i] ) continue;
			m_Info._pSession[i]->SendPacketMessage(pPacket);
		}
	}
	return; 
}

void CRoom::_SendS2PacketAllPlayer( S2MOPacketBase * pPacket, SLOT_STATE eLimitState )
{
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		if( eLimitState < m_Info._aSlotInfo[i]._State )
		{
			if( NULL == m_Info._pSession[i] ) continue;
			m_Info._pSession[i]->S2MOSendEvent( pPacket );
		}
	}
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendPacketAnotherPlayer( i3NetworkPacket * pPacket, CUserSession * pSession, SLOT_STATE eLimitState )
{
	INT32 i; 
	for(i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		if( eLimitState < m_Info._aSlotInfo[i]._State )
		{   
			if( NULL == m_Info._pSession[i] )		continue;
			if( pSession == m_Info._pSession[i] )	continue;
			m_Info._pSession[i]->SendPacketMessage(pPacket);
		}
	}
	return; 
}

void CRoom::_SendS2PacketAnotherPlayer( S2MOPacketBase * pPacket, CUserSession * pSession, SLOT_STATE eLimitState )
{
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( eLimitState < m_Info._aSlotInfo[i]._State )
		{   
			if( NULL == m_Info._pSession[i] )		continue;
			if( pSession == m_Info._pSession[i] )	continue;
			m_Info._pSession[i]->S2MOSendEvent( pPacket );
		}
	}
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendPreStartBattle(CUserSession * pSession, BOOL bSuccess)
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx; 

	if( i32SlotIdx < 0 ) return;	// SlotIdx가 음수이면 리턴.

	//1. 실패로 패킷을 받았을때. 
	if(bSuccess == FALSE)
	{
		//실패 입니다. 
		i3NetworkPacket Packet( PROTOCOL_BATTLE_PRESTARTBATTLE_ACK );
		Packet.WriteData( &bSuccess,	sizeof( BOOL ) );
		Packet.WriteData( &i32SlotIdx,	sizeof( INT32 ) );
		pSession->SendPacketMessage( &Packet );
		return; 
	}

	//2. 배틀 여부에 대한 정보가 없을때.  
	if(m_bIsBattleRoomCreated == false)
	{//실패 입니다. 
		bSuccess = FALSE; 
		i3NetworkPacket Packet( PROTOCOL_BATTLE_PRESTARTBATTLE_ACK );
		Packet.WriteData( &bSuccess,	sizeof( BOOL ) );
		Packet.WriteData( &i32SlotIdx,	sizeof( INT32 ) );
		pSession->SendPacketMessage( &Packet );
		return; 
	}

	//튜토리얼 예외처리 
	if( IsTutorialMode() )
	{
		bSuccess		= TRUE; 
		i32SlotIdx		= 0; 
		m_ui8BattleType = BATTLE_SERVER_TYPE_P2P; 
	}

	// 성공 패킷 
	//I3TRACE( "PROTOCOL_BATTLE_PRESTARTBATTLE_ACK - SlotIdx(%d)\n", pSession->m_i32SlotIdx );
	i3NetworkPacket Packet( PROTOCOL_BATTLE_PRESTARTBATTLE_ACK );
	Packet.WriteData( &bSuccess,			sizeof(BOOL)); 
	Packet.WriteData( &i32SlotIdx,			sizeof(INT32));
	Packet.WriteData( &m_ui8BattleType,		sizeof(UINT8));
	Packet.WriteData( &m_ui32BattleServerIP,	sizeof(UINT32));
	Packet.WriteData( &m_ui16BattleServerPort,	sizeof(UINT16));
	Packet.WriteData( &m_ui32BattleRoomIdx,	sizeof(UINT32));
	Packet.WriteData( &m_ui32BattleEncryptKey,	sizeof(UINT32));

	UINT8 aui8HitPartIndex[ CHARA_HIT_MAXCOUNT ];
	ShuffleHitPart( aui8HitPartIndex, m_ui32BattleEncryptKey );
	Packet.WriteData( aui8HitPartIndex, sizeof(aui8HitPartIndex));

	pSession->SendPacketMessage( &Packet );



	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendStartBattle(CUserSession * pSession, INT32 i32Result, bool bNoStartRound )		
{
	//1. Set Basic Packet 
	PACKET_BATTLE_STARTBATTLE_ACK Packet;
	Packet.m_TResult		= i32Result;
	Packet.m_ui8SlotIdx		= (UINT8)pSession->m_i32SlotIdx;

	if( EV_SUCCESSED( i32Result ) )
	{//성공 
		if ( IsChallengeMode() )
		{
			i3NetworkPacket packet2(PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK);
			packet2.WriteData(&m_Info.m_i8currentDifficultyLevel,	sizeof(INT8));
			packet2.WriteData( m_Info.m_aui32AILevel,			sizeof(UINT32)*SLOT_MAX_COUNT);
			pSession->SendPacketMessage( &packet2 );
		}

		//2. 난입한 유저인지 확인
		UINT8 IsInterEnter = 1; 
		if( (m_Info._ui32FirstBattleStartUser & (0x1 << pSession->m_i32SlotIdx)) )	IsInterEnter = 0;
		if( IsInterEnter == 1 && m_bGMPause )
		{
			pSession->m_ui32ATCanRespawnPacket = i3ThreadTimer::GetServerTime();
			IsInterEnter = 2;
		}
		Packet.m_ui8InterEnter = IsInterEnter;
		Packet.m_ui16BattleUser = m_Info._ui16BattleUser;

		// 난입한 사람들을 위한 추가 패킷. 이곳에서 게임시작에 필요한 메시지를 추가해줍니다.
		m_pRoomMode->AddStartBattlePacket( &Packet, pSession, GetRoomBattleTime() );

		// 어뷰징 체크 - 난입한 유저의 배틀 시작 시간 설정
		pSession->m_ui32RTBattleStart			= GetRoomBattleTime();
		pSession->m_eLastStageType				= StageID::GetStageMode(m_Info._InfoBasic._StageID);
		
		pSession->m_bFirstObserver				= TRUE;

		//모두에게 보냄 
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
		{
			if( m_Info._aSlotInfo[i]._State > SLOT_STATE_READY )
			{
				m_Info._pSession[i]->S2MOSendEvent( &Packet );
			}
		}

		if(  bNoStartRound )
		{
			switch( m_Info._i32MiStateType )
			{
				// 인게임 카운트가 들어가면서 더이상 필요 없는 코드입니다.
			//case MISSION_STATE_PRE_BATTLE :
			//	{
			//		i3NetworkPacket SendPacket2( PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK );
			//		SendPacket2.WriteData( &m_Info._ui16RoundStartUser, sizeof(UINT16) );
			//		SendPacket2.WriteData( &m_Info._MiDinoInfo, sizeof(DINO_ROOM_INFO) ); //사이즈가 크지 않아 그냥 보낸다.
			//		pSession->SendPacketMessage( &SendPacket2 );
			//	}
			//	break;
			case MISSION_STATE_BATTLE:
				{
					UINT8 ui8EndTimeIdx = (m_Info._InfoAdd._SubType & 0xF0) >> 4;
					UINT32 ui32RemainTime = g_TeamTimeTable[ui8EndTimeIdx] - (GetRoomBattleTime() - m_Info._ui32RTRoundStart );

					i3NetworkPacket SendPacket2( PROTOCOL_BATTLE_MISSION_ROUND_START_ACK );
					SendPacket2.WriteData( &m_Info._InfoAdd._NowRoundCount,	sizeof(UINT8) );
					SendPacket2.WriteData( &ui32RemainTime,					sizeof(UINT32) ); // 게임 남은 타임
					SendPacket2.WriteData( &m_Info._ui16RoundStartUser,		sizeof(UINT16) );
					SendPacket2.WriteData( &m_ui8AtkDefChange,				sizeof(UINT8) );
					pSession->SendPacketMessage( &SendPacket2 );
				}
				break;
			}
		}

		pSession->SetPosition(GAME_TASK_BATTLE);

	}
	else
	{//자신만 받음 
		pSession->S2MOSendEvent( &Packet );
	}

	return; 
}

void CRoom::SendSlotEquipmentInfo( INT32 i32ThreadIdx, INT32 i32Slot, UINT16 ui16ChangeFlag )
{
	if( 0 > i32Slot )												return;		//슬롯 인덱스가 정확한지.
	if( SLOT_MAX_COUNT <= i32Slot )									return;		//슬롯 인덱스가 정확한지.

	GC_PROTOCOL_SLOT_EQUIPMENT Struct;
	Struct.m_i32SlotIdx					= i32Slot;
	Struct.m_ui16ChangeFlag				= ui16ChangeFlag;
	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; ++i )
	{
		Struct.m_EquipmentWeapon.m_TItemID[i] = m_Info._pSession[i32Slot]->m_aTBattleEquipWeapon[i].m_TItemID;
	}
	// vv3 - check
	S2MO_CHAR_BASE_INFO		CharBaseInfo;

	INT32 i32AtkDefChange = 0;
	
	if(m_pRoomMode) i32AtkDefChange = m_pRoomMode->GetAtkDefChange() ? 1 : 0;

	m_Info._pSession[i32Slot]->m_pDesign->GetMainCharBaseInfo(&m_Info._pSession[i32Slot]->m_MultiSlot, &CharBaseInfo, (i32AtkDefChange + i32Slot) % TEAM_COUNT);
	for( INT32 k = 0 ; k < CHAR_EQUIPMENT_PARTS_COUNT ; ++k )
	{
		Struct.m_EquipmentChara.SetEquipNum((CHAR_EQUIPMENT_PARTS)k, m_Info._pSession[i32Slot]->m_aTBattleEquipParts[k].m_TItemID );
	}
	i3mem::Copy( Struct.m_aui8WeaponEndurance,	m_Info._pSession[i32Slot]->m_aUseWeaponEndurance,	sizeof(UINT8)*CHAR_EQUIPMENT_WEAPON_COUNT );
	i3mem::Copy( Struct.m_aui8EquipUserTitle,	m_Info._pSession[i32Slot]->m_aui8EquipUserTitle,	sizeof(UINT8) * MAX_EQUIP_USER_TITLE );
	Struct.m_EquipItem					= m_Info._aSlotInfo[i32Slot]._Item;

	if ( isDedicationMode() )
	{
		// 컨트롤서버->데디케이션서버->컨트롤서버->게임서버를 거쳐 클라이언트로 보낸다.
		g_pModuleCast->BattleWeaponInfo( i32ThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT8)i32Slot, &Struct );
	}
	else
	{
		// 클라이언트로 바로 보낸다.
		i3NetworkPacket Packet( PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK  );
		Packet.WriteData( &Struct, sizeof(GC_PROTOCOL_SLOT_EQUIPMENT) );
		_SendPacketBattlePlayer( &Packet );
	}
}

void CRoom::_SendBattleCountdown( INT8 i8StartCountdown )
{
	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_START_COUNTDOWN_ACK);
	SendPacket.WriteData( &i8StartCountdown, sizeof(INT8) );
	_SendPacketAllPlayer(&SendPacket);
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendSlotInfo( CUserSession * pSession )
{
	/*PACKET_ROOM_GET_SLOTINFO_ACK Packet;
	Packet.m_ui8MainSlotIdx = (UINT8)m_Info._i32MainSlotIdx;
	Packet.m_aSlotInfo.SetValue( m_Info._aSlotInfo, 0, SLOT_MAX_COUNT );*/

	i3NetworkPacket SendPacket(PROTOCOL_ROOM_GET_SLOTINFO_ACK);

	SendPacket.WriteData(&m_Info._i32MainSlotIdx, sizeof(UINT8));

	UINT16 ui16ValSize = 0;
	ui16ValSize = sizeof(m_Info._aSlotInfo[0])
		- sizeof(m_Info._aSlotInfo[0]._ui8NationalCode)
		- sizeof(m_Info._aSlotInfo[0]._bIsGmObserverMode)
		- sizeof(m_Info._aSlotInfo[0]._ClanName);
	UINT16 ui16StrSize = 0;
	for (UINT8 i=0; i<SLOT_MAX_COUNT; ++i)
	{
		SendPacket.WriteData(&ui16ValSize, sizeof(UINT16));
		SendPacket.WriteData(&m_Info._aSlotInfo[i], ui16ValSize);
		
		ui16StrSize = UINT16(wcslen(m_Info._aSlotInfo[i]._ClanName)*2);
		SendPacket.WriteData(&ui16StrSize, sizeof(UINT16));
		SendPacket.WriteData(&m_Info._aSlotInfo[i]._ClanName, ui16StrSize);

		SendPacket.WriteData(&m_Info._aSlotInfo[i]._bIsGmObserverMode, sizeof(bool));
		SendPacket.WriteData(&m_Info._aSlotInfo[i]._ui8NationalCode, sizeof(UINT8));
	}

	if (pSession == NULL)
		_SendPacketAllPlayer(&SendPacket);
	else					
		pSession->SendPacketMessage(&SendPacket);

	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::CheckMainClient_U(CUserSession * pSession )
{
	//타임 아웃이 들어 갔는지 체크 합니다. 
	if( m_ui32ATMCTimeOutStart < 1 )
	{	
		//타임 세팅
		m_ui32ATMCTimeOutStart		= i3ThreadTimer::GetServerTime();
		m_ui16MCTimeOutFlg			= 0;
		m_ui16MCTimeOutStartUserFlg	= 0; 

		//시작하는 유저를 세팅 
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
		{
			if( (m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE) && (i != m_Info._i32MainSlotIdx) )
				m_ui16MCTimeOutStartUserFlg	= m_ui16MCTimeOutStartUserFlg | (1 << i); 
		}
	}

	m_ui16MCTimeOutFlg =  m_ui16MCTimeOutFlg | (1 << pSession->m_i32SlotIdx); 

	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_RemoveTimeOutFlag(INT32 i32SlotIdx)
{	
	if(m_ui32ATMCTimeOutStart > 0) 
	{//자신 플래그를 0으로 변경합니다. 
		INT16 Temp = 0; 
		Temp = 1 << i32SlotIdx; 
		Temp = ~Temp; 
		m_ui16MCTimeOutStartUserFlg	= m_ui16MCTimeOutStartUserFlg	& Temp; 
		m_ui16MCTimeOutFlg			= m_ui16MCTimeOutFlg			& Temp; 
	}

	return; 
}

/*----------------------------------------------------------
Desc : 게임중에 UDP를 보내지 않는 유저를 방에서 나가게 한다. 방장이 보낸다. 호출된 유저는 방에서 나가게 된다. 
----------------------------------------------------------*/
void CRoom::GameKickNormalClient_U(CUserSession * pSession, UINT32 ui32SlotIdx)
{
	if( pSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )			return;		//방장인지.

	// SlotIdx 가 UINT 라서 0 이하값을 검사할 필요가 없음.
	//if( 0 > SlotIdx )												return;		//슬롯 인덱스가 정확한지.
	if( ui32SlotIdx >= SLOT_MAX_COUNT)									return;	//슬롯 인덱스가 정확한지.
	if( m_Info._aSlotInfo[ui32SlotIdx]._State != SLOT_STATE_BATTLE)		return; //게임중인 유저인지.

	if( EVENT_ERROR_SUCCESS == _GiveUpBattle( pSession->m_WorkThreadIdx, m_Info._pSession[ui32SlotIdx], FALSE, ROOM_ENDTYPE_HACKING) )
	{
		//메시지 보내기 
		i3NetworkPacket Packet( PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK );
		m_Info._pSession[ui32SlotIdx]->SendPacketMessage( &Packet );
	}

	return; 
}


/*----------------------------------------------------------
Desc : 방정보 변경 내부에서 값 확인후 보내줘야 한다. 쓰레기 값을 처리 해야 함. //메인 쓰레드에서만 호출됨 
----------------------------------------------------------*/
void CRoom::_BattleTimeOutClient_M( UINT32 ui32SlotIdx )
{
	if( 0 > ui32SlotIdx )											return; 
	if( ui32SlotIdx >= SLOT_MAX_COUNT)								return; 
	if( m_Info._aSlotInfo[ui32SlotIdx]._State != SLOT_STATE_BATTLE)	return;

	//로그 배틀에서 나가기
	_GiveUpBattle( MAIN_THREAD_IDX, m_Info._pSession[ui32SlotIdx], FALSE, ROOM_ENDTYPE_HACKING );

	//메시지 보내기 
	i3NetworkPacket Packet( PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK );
	m_Info._pSession[ui32SlotIdx]->SendPacketMessage( &Packet );

	return; 
}

INT32 CRoom::ChangeRoomInfo(CUserSession * pSession, ROOM_INFO_BASIC * pRoomInfo, INT8 i8enemyAiCount, INT8 i8difficultyLevel, BOOL bIsRoomCreate )
{
	INT32 i32Rv = EVENT_ERROR_ROOM_NO_AUTHORITY; 

	
	if( ( bIsRoomCreate || (m_Info._i32MainSlotIdx == pSession->m_i32SlotIdx)) && m_Info._InfoBasic._State == ROOM_STATE_READY )
	{
		// 플레이 가능한 스테이지이면 룸 모드 설정
		if( g_pContextMain->IsPlayStage( pRoomInfo->_StageID ) )
		{
			i32Rv = SetRoomMode( pRoomInfo->_StageID );
			if( i32Rv != EVENT_ERROR_SUCCESS )
			{
				return i32Rv;
			}
		}
		else
			return EVENT_ERROR_ROOM_NO_AUTHORITY;

		if( m_pRoomMode == NULL )
			return EVENT_ERROR_ROOM_NO_AUTHORITY;

		// 스테이지를 변경하는 경우
		if( StageID::GetStageMode(m_Info._InfoBasic._StageID) != StageID::GetStageMode(pRoomInfo->_StageID) )
		{
			//레디 상태를 모두 풀어준다. 
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
			{
				if( m_Info._aSlotInfo[i]._State == SLOT_STATE_READY )
				{
					m_Info._aSlotInfo[i]._State = SLOT_STATE_NORMAL;
				}
			}
			_SendSlotInfo(NULL); 
		}
		
		//Set Title
		m_Info._InfoBasic._InfoFlag = pRoomInfo->_InfoFlag;
		i3mem::Copy( m_Info._InfoBasic._Title , pRoomInfo->_Title, NET_ROOM_NAME_SIZE ); 
		m_Info._InfoBasic._Title[NET_ROOM_NAME_SIZE - 1] = 0;
		// 패스워드 변경은 [PROTOCOL_ROOM_CHANGE_PASSWD_REQ]를 사용
		if( m_strPasswd[0] != 0 )	m_Info._InfoBasic._InfoFlag |= ROOM_INFO_FLAG_PRIVATE_MODE;
		else						m_Info._InfoBasic._InfoFlag &= ~ROOM_INFO_FLAG_PRIVATE_MODE;
		
		m_Info._InfoBasic._StageID			= pRoomInfo->_StageID;
		m_StageUID							= StageID::GetStageUID( pRoomInfo->_StageID );
		m_Info._i32StageType				= StageID::GetStageMode(m_Info._InfoBasic._StageID);
		m_Info._InfoBasic._OptionFlag		= pRoomInfo->_OptionFlag;

		if( false == g_pContextMain->IsContentsEnabled(CONTENTS_TEAM_CHANGE) )	// 20140807 김현우 싱말을 위한 긴급 땜빵코드입니다.
		{
			m_Info._InfoBasic._InfoFlag		&= ~ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE;
		}

		// AI 레벨 설정
		m_Info.m_i8difficultyLevel			= i8difficultyLevel;
		m_Info.m_i8currentDifficultyLevel	= i8difficultyLevel;
		m_Info.m_i8enemyAiCount				= i8enemyAiCount;

		//3. 총기 검사
		if( m_Info._InfoBasic._WeaponFlag	!= pRoomInfo->_WeaponFlag )		//무기 
		{
			//유저들의상태를 모두 변경해서 다시 보냄 
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
			{
				if( m_Info._aSlotInfo[i]._State == SLOT_STATE_READY )
				{
					m_Info._aSlotInfo[i]._State = SLOT_STATE_NORMAL;
				}
			}
			m_Info._InfoBasic._WeaponFlag = pRoomInfo->_WeaponFlag;
		}

#ifdef USE_COLOR_NICK2
		m_Info._InfoAdd._NicklColor					= pSession->GetNickColor();
#endif

		// ROOM_INFO_ADD 갱신
		if( FALSE == m_pRoomMode->SetRoomOption( pRoomInfo->_StageID ) )
		{
			return EVENT_ERROR_ROOM_NO_AUTHORITY;
		}
		
		i32Rv = EVENT_ERROR_SUCCESS; 
	}

	return i32Rv; 
}

INT32 CRoom::ChangeRoomInfoOption(CUserSession * pSession, ROOM_INFO_ADD * pInfoAdd, BOOL bIsRoomCreate )
{
	if( NULL == pSession || NULL == pInfoAdd || NULL == m_pRoomMode )	return	EVENT_ERROR_FAIL;

	INT32 i32Rv = EVENT_ERROR_ROOM_NO_AUTHORITY; 
	
	if( ( bIsRoomCreate || (m_Info._i32MainSlotIdx == pSession->m_i32SlotIdx)) && 
		m_Info._InfoBasic._State == ROOM_STATE_READY )	
	{
		if( m_pRoomMode->CheckSubType( pInfoAdd->_SubType ) == FALSE )	return EVENT_ERROR_ROOM_NO_AUTHORITY;

		// Set Add Info
		i3mem::Copy( &m_Info._InfoAdd, pInfoAdd, sizeof(ROOM_INFO_ADD) );
		m_Info._InfoAdd._NowRoundCount				= 0;
		m_Info._InfoAdd._NowIngTime					= 0;
		if(NULL != pSession->GetNickName()) { i3String::Copy( m_Info._InfoAdd._Nick, pSession->GetNickName(), NET_NICK_NAME_SIZE ); }	
		m_Info._InfoAdd._Nick[ NET_NICK_NAME_SIZE-1 ]		= '\0';
		
#ifdef USE_COLOR_NICK2
		m_Info._InfoAdd._NicklColor					= pSession->GetNickColor();
#endif
		// Channel 속성
		const bool bIsClanChannel = IsClanChannel();
		const bool bIsMatchMaking = IsMatchMaking();
		if ( bIsClanChannel )
		{	
			if( bIsMatchMaking )	// 매치 메이킹 일떄
			{	
				// 방 제한
				if( m_Info._InfoAdd._SubType == 0x00 )	
					m_Info._InfoAdd._SubType = 0x02;

				m_Info._InfoAdd._LockInterEnter		= 0;				// 난입 제한
				m_Info._InfoAdd._LockObserver		= 0;				// 관전제한

				m_i8IsClanMatch = ZLOG_CLAN_MATCHING;
			}
			else	// 비 매치 메이킹 일떄
			{	// 5개 이상의 슬롯을 닫아준다.
				// 방 제한
				m_Info._InfoAdd._LockInterEnter		= 0x01;				// 난입 가능함

				m_i8IsClanMatch = ZLOG_CLAN_NORMAL;
			}
			m_Info._InfoAdd._LockTeamBalance		= 0;				// 기본 팀 밸런스 없음
		}
		else
		{
			m_i8IsClanMatch = ZLOG_CLAN_NOT;
		}

		// 스테이지 검사.. 순서 바뀌면 안됩니다.. 여기까지..
		////////////////////////////////////////////////////////////////////////////////
		// 난입 가능한 방인지 체크
		if( m_Info._InfoAdd._LockInterEnter )						m_Info._InfoBasic._InfoFlag	|= ROOM_INFO_FLAG_INTER_ENTER;
		else														m_Info._InfoBasic._InfoFlag	&= ~ROOM_INFO_FLAG_INTER_ENTER;
		// 팀밸런스 체크
		if( m_Info._InfoAdd._LockTeamBalance )						m_Info._InfoBasic._InfoFlag	|= ROOM_INFO_FLAG_TEAM_BALANCE;
		else														m_Info._InfoBasic._InfoFlag	&= ~ROOM_INFO_FLAG_TEAM_BALANCE;

		if( false == g_pContextMain->IsContentsEnabled(CONTENTS_TEAM_CHANGE) )	// 20140807 김현우 싱말을 위한 긴급 땜빵코드입니다.
		{
			m_Info._InfoBasic._InfoFlag		&= ~ROOM_INFO_FLAG_ATK_DEF_AUTO_CHANGE;
		}

		// 라운드 변경을 위해 반드시 처리되어야 합니다.
		if( FALSE == m_pRoomMode->SetRoomOption( m_Info._InfoBasic._StageID ) )
		{
			return EVENT_ERROR_ROOM_NO_AUTHORITY;
		}

		/* 상단에서 SetRoomOption 호출이 모두 끝난 이후에 클랜 룸 설정을 합니다.
		 SetRoomOption 함수에서 ready option을 조절 해버립니다. 일부 경우는 Option이 채널 타입에 따라 강제로 조절 될 필요가 있습니다 (비매치메이킹 등) */
		if ( bIsClanChannel )
		{	/* 만약 매치 메이킹도 방정보 다시 변경할 필요가 있다면 SetRoomInfoMatchMaking 호출 해줍시다.
			   그리고, 외부에서 SetRoomInfoNonMatchMaking, SetRoomInfoMatchMaking 호출하는 부분을 다 제거 합니다. */
			if ( false == bIsMatchMaking )
			{
				SetRoomInfoNonMatchMaking( m_Info );
			}
		}

		i32Rv = EVENT_ERROR_SUCCESS; 
	}
	return i32Rv;
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendRoomInfo(CUserSession * pSession)
{	
	i3NetworkPacket Packet(PROTOCOL_ROOM_CHANGE_ROOMINFO_ACK); 
	Packet.WriteData( &m_Info._InfoBasic, sizeof(ROOM_INFO_BASIC) ); 

	Packet.WriteData( &m_Info._InfoAdd,	sizeof(ROOM_INFO_ADD) ); 

	if ( IsChallengeMode() )
	{
		Packet.WriteData(&m_Info.m_i8enemyAiCount,	sizeof(INT8));
		Packet.WriteData(&m_Info.m_i8difficultyLevel, sizeof(INT8));
	}

	if( pSession == NULL )	_SendPacketAllPlayer( &Packet );
	else					pSession->SendPacketMessage( &Packet );

	return;		
}

void CRoom::_SendRoomInfoOption()
{		
	i3NetworkPacket SendPacket(PROTOCOL_ROOM_CHANGE_ROOM_OPTIONINFO_ACK); 
	//SendPacket.WriteData(&bChangeWeapon,	sizeof(BOOL)); 
	SendPacket.WriteData(&m_Info._InfoAdd,		sizeof(ROOM_INFO_ADD)); 

	_SendPacketAllPlayer(&SendPacket);

	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendNickName(CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll)
{
	i3NetworkPacket Packet(PROTOCOL_ROOM_GET_NICKNAME_ACK); 

	if( m_Info._aSlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP)
	{
		i32SlotIdx = EVENT_ERROR_FAIL; 
		Packet.WriteData(&i32SlotIdx, sizeof( INT32));
	}
	else
	{
		Packet.WriteData( &i32SlotIdx, sizeof( INT32 ));
		Packet.WriteData( m_Info._pSession[i32SlotIdx]->GetNickName(), NET_NICK_NAME_SIZE ); 	//의심
		Packet.WriteData( &m_Info._pSession[i32SlotIdx]->m_UserInfoInvItemData._ui8NickColor, sizeof(UINT8) );
	}

	if( bAll )
	{
		_SendPacketAnotherPlayer( &Packet, pSession );
	}	
	else
	{
		pSession->SendPacketMessage( &Packet );
	}
	return; 
}

void CRoom::_SendRank(CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll)
{
	i3NetworkPacket Packet(PROTOCOL_ROOM_GET_RANK_ACK); 

	if( m_Info._aSlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP)
	{  
		i32SlotIdx = EVENT_ERROR_FAIL; 
		Packet.WriteData(&i32SlotIdx, sizeof( INT32));
	}
	else
	{
		m_Info._aSlotInfo[ i32SlotIdx ]._Rank = (UINT8)m_Info._pSession[i32SlotIdx]->m_ui32ViewRank;

		Packet.WriteData(&i32SlotIdx, sizeof( INT32 ));
		Packet.WriteData(&m_Info._pSession[i32SlotIdx]->m_ui32ViewRank, sizeof(UINT32) ); 	//의심
	}

	if( bAll )
	{
		_SendPacketAllPlayer( &Packet );
	}	
	else
	{
		pSession->SendPacketMessage( &Packet );
	}
	return; 
}

void CRoom::_SendChangeColorNick(CUserSession * pSession, INT32 i32SlotIdx, BOOL bAll)
{
	i3NetworkPacket Packet(PROTOCOL_ROOM_GET_COLOR_NICK_ACK); 

	if( m_Info._aSlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP)
	{
		i32SlotIdx = EVENT_ERROR_FAIL; 
		Packet.WriteData(&i32SlotIdx, sizeof( INT32));
	}
	else
	{
		Packet.WriteData(&i32SlotIdx, sizeof( INT32 ));
		Packet.WriteData(&m_Info._pSession[i32SlotIdx]->m_UserInfoInvItemData._ui8NickColor, sizeof(UINT8) ); 	//의심
	}

	if( bAll )
	{
		_SendPacketAllPlayer( &Packet );
	}	
	else
	{
		pSession->SendPacketMessage( &Packet );
	}
	return; 
}

void CRoom::_SendChangeColorMuzzleFlash( INT32 i32SlotIdx, BOOL bAll)
{
	i3NetworkPacket Packet(PROTOCOL_ROOM_GET_COLOR_MUZZLE_FLASH_ACK); 

	if( m_Info._aSlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP)
	{
		i32SlotIdx = EVENT_ERROR_FAIL; 
		Packet.WriteData(&i32SlotIdx, sizeof( INT32));
	}
	else
	{
		Packet.WriteData(&i32SlotIdx, sizeof( INT32 ));
		Packet.WriteData(&m_Info._pSession[i32SlotIdx]->m_UserInfoInvItemData._ui16MuzzleFlashColor, sizeof(UINT16) ); 	//의심
	}

	if( bAll )
	{
		_SendPacketAllPlayer( &Packet );
	}	
	else
	{
		m_Info._pSession[i32SlotIdx]->SendPacketMessage( &Packet );
	}
	return; 
}
/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::ChangePasswd( CUserSession * pSession, char * pNewPasswd )
{
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)return EVENT_ERROR_ROOM_NO_AUTHORITY; 

	if( pNewPasswd[0] == '\0' )
	{
		m_Info._InfoBasic._InfoFlag &= ~ROOM_INFO_FLAG_PRIVATE_MODE; 
		m_strPasswd[0] = '\0';
	}
	else
	{
		i3mem::Copy( m_strPasswd, pNewPasswd, NET_ROOM_PW);
		m_Info._InfoBasic._InfoFlag |= ROOM_INFO_FLAG_PRIVATE_MODE; 
	}

	i3NetworkPacket SendPacket( PROTOCOL_ROOM_CHANGE_PASSWD_ACK ); 	
	SendPacket.WriteData( m_strPasswd, NET_ROOM_PW ); 
	_SendPacketAllPlayer( &SendPacket ); 

	return EVENT_ERROR_SUCCESS; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::ChangePing_CS( CUserSession * pSession, UINT8 * pui8Ping)
{
	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)return EVENT_ERROR_ROOM_NO_AUTHORITY; 
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		// 챌린지모드이면 핑값 처리함.
		if( !IsChallengeMode() ) { return EVENT_ERROR_ROOM_NO_AUTHORITY; }
		break; 
	default: return EVENT_ERROR_ROOM;	break; 
	}

	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		m_stRoomUserInfo[i]._ui8Ping = pui8Ping[i]; 
	}

	return EVENT_ERROR_SUCCESS; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::ChangePing_U( CUserSession * pSession, UINT8 ui8Ping )
{
	INT32 SlotIdx = pSession->m_i32SlotIdx; 
	m_stRoomUserInfo[SlotIdx]._ui8Ping = ui8Ping; 
	return EVENT_ERROR_SUCCESS; 
}

BOOL CRoom::ChangeMainSlotIdx( CUserSession * pSession )
{
	//INT32 i32SlotIdx = pSession->m_MultiSlot.GetMainSlot();

	return TRUE;
}
/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoom::_SendPing_M(void)
{
	INT32 i32TempPing	= 0; 
	INT32 i32Count		= 0; 

	i3NetworkPacket SendPing( PROTOCOL_BATTLE_SENDPING_ACK );

	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		SendPing.WriteData( &m_stRoomUserInfo[i]._ui8Ping, sizeof(UINT8) ); 

		if( m_Info._aSlotInfo[i]._State != SLOT_STATE_BATTLE ) continue;
		i32TempPing += m_stRoomUserInfo[i]._ui8Ping; 
		i32Count++; 
	}

	if(i32Count > 0) m_Info._InfoBasic._Ping = (UINT8)(i32TempPing / i32Count); 

	_SendPacketBattlePlayer( &SendPing ); 
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
void CRoom::Chatting_U( INT32 i32ThreadIdx, CUserSession * pSession, TTCHAR * pString, UINT16 ui16Type)
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;
	if( 0 > i32SlotIdx )				return;
	if( SLOT_MAX_COUNT <= i32SlotIdx )	return;

	INT32 i32StrLen = i3String::Length(pString) + 1;
	bool  bIsGMUser = pSession->CheckAuthLevel();

	i3NetworkPacket SendPacket( PROTOCOL_ROOM_CHATTING_ACK ); 
	SendPacket.WriteData( &ui16Type,	sizeof(UINT16) ); 
	SendPacket.WriteData( &i32SlotIdx,	sizeof(INT32) );		//i32SlotIdx
	SendPacket.WriteData( &bIsGMUser,	sizeof(bool) );
	SendPacket.WriteData( &i32StrLen,	sizeof(INT32) );		//Str Length
	SendPacket.WriteData( pString,		sizeof(TTCHAR) * (UINT16)i32StrLen );			//

	UINT16 ui16SendUser = 0;

	// GM User 의 경우 모든 메시지를 다 받도록 한다.
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State )	continue;
		if( false == m_Info._pSession[i]->CheckAuthLevel() )	continue;
			
		ui16SendUser |= (1 << i);
	}

	INT32 i32Start = 0;
	INT32 i32Incre = 1;

	if( CHATTING_TYPE_TEAM == ui16Type )
	{	// 팀 채팅
		i32Start = (i32SlotIdx % TEAM_COUNT);
		i32Incre = TEAM_COUNT;
	}
	else
	{	//전체 보내기
		i32Start = 0;
		i32Incre = 1;
	}

	if( SLOT_STATE_BATTLE == m_Info._aSlotInfo[i32SlotIdx]._State )
	{	// 유저가 게임중일 때
		switch( m_pRoomMode->GetChattingOption() )
		{
		case ROOM_CHATTING_ALL:
			{
				for( INT32 i = i32Start ; i < SLOT_MAX_COUNT ; i += i32Incre )
				{
					if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State ) continue;
						
					ui16SendUser |= (1 << i);
				}
			}
			break;
		case ROOM_CHATTING_GHOST:
			{
				//1. 내상태 점검
				BOOL bSenderObservation = TRUE;
				BOOL bGM = GetSession( i32SlotIdx )->CheckAuthLevel( AUTH_LEVEL_GM );

				if( IS_USER_START( i32SlotIdx ) )
				{
					if( IS_USER_DEATH( i32SlotIdx ) )	bSenderObservation = TRUE; 
					else								bSenderObservation = FALSE; 
				}

				BOOL bRecverObservation;
				for( INT32 i = i32Start ; i < SLOT_MAX_COUNT ; i += i32Incre )
				{
					if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State ) continue;

					if( MISSION_STATE_BATTLE == m_Info._i32MiStateType && !bGM )
					{	// 고스트 채팅은 배틀 중일 때만 사용 가능 합니다.
						bRecverObservation = TRUE; 
						if( IS_USER_START( i ) )
						{
							if( IS_USER_DEATH( i ) )		bRecverObservation = TRUE; 
							else							bRecverObservation = FALSE; 
						}
						if( bSenderObservation != bRecverObservation )			continue;
					}
					ui16SendUser |= (1 << i);
				}
			}
			break;
		}
	}
	else 
	{
		for( INT32 i = i32Start; i < SLOT_MAX_COUNT; i += i32Incre )
		{	// 클랜에 있을때는 클랜 체팅으로 인해 메시지를 보내지 않습니다.
			if( m_Info._aSlotInfo[i]._State == SLOT_STATE_CLAN )	continue;;

			if( m_Info._aSlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;
			if(	m_Info._aSlotInfo[i]._State >= SLOT_STATE_LOAD )	continue;
				
			ui16SendUser |= (1 << i);
		}
	}

	// On 되어있는 유저들만 전달 합니다.
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( 0 == ( (1 << i) & ui16SendUser ) )		continue;;

		m_Info._pSession[i]->SendPacketMessage( &SendPacket );
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if( pSession->m_ui8LocaleCode == LOCALE_CODE_RUSSIA )
	{
		INT32 i32ChattingType = ZLOG_CHATTING_TYPE_ROOM;
		g_pModuleContextNC->LogSendMessage(i32ThreadIdx, LOG_MESSAGE_U_CHATTING, pSession, &i32ChattingType, pString);
	}
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::ChangeSlot_U( CUserSession * pSession, INT32 i32SlotIdx, BOOL bOpen )
{	
	if( IsMatchMaking() ) return EVENT_ERROR_MATCH_MAKING_ROOM;

	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)return EVENT_ERROR_ROOM_NO_AUTHORITY; 

	if(bOpen)	//열기
	{
		// 맥스 인원수 보다 많은 슬롯은 열 수 없다.
		if( ( g_pContextMain->GetMaxPerson(m_Info._InfoBasic._StageID) ) <= i32SlotIdx)
		{
			return EVENT_ERROR_ROOM_NO_AUTHORITY;
		}

		if(m_Info._aSlotInfo[i32SlotIdx]._State == SLOT_STATE_CLOSE)
		{
			m_Info._aSlotInfo[i32SlotIdx].Reset( SLOT_STATE_EMPTY );
			m_Info._InfoBasic._PersonPlayerMax++; 
		}
	}
	else		//닫기
	{
		if( m_Info._aSlotInfo[i32SlotIdx]._State > SLOT_STATE_CLOSE)
		{
			//로그 
			//g_pModuleContextNC->LogSendMessage( pSession->m_WorkThreadIdx, LOG_MESSAGE_U_R_KICK_LEAVE_ROOM, pSession, this); 이녀석은 게임중 강제 퇴장입니다.

			//강퇴합니다. 
			i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_KICK_PLAYER); 

			//GM에 의한 Kick여부를 확인하기 위해 bool값 변수를 추가로 보냅니다.
			bool			bIsGMKick	=	FALSE;
			SendPacket.WriteData( &bIsGMKick, sizeof(bool) );

			m_Info._pSession[i32SlotIdx]->SendPacketMessage(&SendPacket); 
			m_Info._pSession[i32SlotIdx]->m_bForceRoomLeave = TRUE; 

		}
		else
		{
			if(m_Info._aSlotInfo[i32SlotIdx]._State != SLOT_STATE_CLOSE)
			{
				m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_CLOSE;
				m_Info._InfoBasic._PersonPlayerMax--; 
			}
		}
	}
	_SendSlotInfo( NULL ); 
	return EVENT_ERROR_SUCCESS; 
}

INT32 CRoom::PickupWeapon_U(	CUserSession * pSession, UINT32 ui32SlotNum, UINT32 ui32Weapon)
{
	//i3NetworkPacket SendPacket( PROTOCOL_BATTLE_PICKUP_WEAPON_ACK );
	//_SendPacketBattlePlayer( &SendPacket );
	return -1; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
BOOL CRoom::_CheckRespawn( INT32 i32SlotIdx )
{
	// 2012-09-06 서동권

	UINT32	ui32Count = m_Info._InfoAdd._NowRoundCount + m_Info._aM_UserScore[i32SlotIdx].m_DeathCount;

	switch( m_Info._i32StageType )
	{
	// 한번 죽으면 리스폰이 안되는 라운드전에서는 라운드 카운트 보다 리스폰 횟수가 많을 수 없다.
	case STAGE_MODE_CONVOY	:
	case STAGE_MODE_ANNIHILATION	:
	case STAGE_MODE_BOMB			:
		if( m_Info._aui32RespawnCount[i32SlotIdx] > m_Info._InfoAdd._NowRoundCount )
			return FALSE;
		break;
	// 라운드가 끝나기 전까지 리스폰이 되는 라운드전에서는 라운드 카운트 + 데스 카운트 보다 리스폰 횟수가 많을 수 없다.
	case STAGE_MODE_CROSSCOUNT		:
	case STAGE_MODE_ESCAPE			:
		ui32Count += m_Info._ai32AcidBombingCount[i32SlotIdx]; // 애시드로 자폭한적이 있다면 자폭횟수를 더해줍니다. 애시드 자폭은 DeathCount 를 증가시키지 않기 때문 
	case STAGE_MODE_DESTROY			:
	case STAGE_MODE_DEFENCE			:
		if( m_Info._aui32RespawnCount[i32SlotIdx] > ui32Count )
			return FALSE;
		break;
	// 데스매치에서는 데스보다 리스폰 횟수가 많을 수 없다.
	case STAGE_MODE_TUTORIAL		:
	case STAGE_MODE_DEATHMATCH		:
		if( m_Info._aui32RespawnCount[i32SlotIdx] > m_Info._aM_UserScore[i32SlotIdx].m_DeathCount )
			return FALSE;
		break;
	}

	return TRUE;
}


bool CRoom::IsClanChannel() const
{
	switch( m_ui8ChannelType )
	{
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN:
		return true;
	default:
		break;
	}
	return false;
}

INT32 CRoom::ReSpawnBattle_U( CUserSession* pSession ) 
{
	//I3TRACE("RespawnReq (Room:%d, SlotIdx:%d, State:%d, RespawnState:%d)\n", pSession->m_i32RoomIdx, pSession->m_i32SlotIdx, m_Info._SlotInfo[pSession->m_i32SlotIdx]._State, m_pRoomMode->CheckRespawn( pSession->m_i32SlotIdx ) );

	INT32 i32SlotIdx = pSession->m_i32SlotIdx;

	if( SLOT_MAX_COUNT <= i32SlotIdx )									return EVENT_ERROR_SUCCESS;
	if( false == m_pRoomMode->CheckRespawn( i32SlotIdx ) )				return EVENT_ERROR_SUCCESS;
	// 리스폰 가능 상태인지 체크
	if( m_bAlive[ i32SlotIdx ] )										return EVENT_ERROR_SUCCESS;
	//if( FALSE == _CheckRespawn( i32SlotIdx ) )							return EVENT_ERROR_SUCCESS;

	//배틀중이 아니면 리스폰을 보내지 않는다. 0923 클라이언트 크래쉬로 추가된 코드
	if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i32SlotIdx]._State )		return EVENT_ERROR_SUCCESS;

	// 연속킬 Reset
	m_Info.m_aui8A_KillCount[ i32SlotIdx ]		= 0;
	m_Info.m_aui8A_HeadShot[ i32SlotIdx ]		= 0;
	m_Info.m_aui8A_SlugShot[ i32SlotIdx ]		= 0;
	m_Info._aui8A_Mission[i32SlotIdx]			= 0;
	m_Info._aui32RespawnCount[i32SlotIdx]++;			// Add Respawn Count
	m_ui8MultiKillCount[i32SlotIdx]				= 0;
	ReSpawnReq( i32SlotIdx );

	//로그 리스폰
	if( LOCALE_CODE_THAILAND != pSession->m_ui8LocaleCode )
	{
		//g_pModuleContextNC->LogSendMessage( pSession->m_WorkThreadIdx, LOG_MESSAGE_U_R_RESPAWN, pSession, this ); 
	}

	//I3TRACE("RespawnReq OK (Room:%d, SlotIdx:%d)\n", pSession->m_i32RoomIdx, i32SlotIdx );

	return EVENT_ERROR_SUCCESS; 
}

INT32 CRoom::ReSpawnBattleForAi_U( CUserSession * pSession, INT32 i32SlotIdx)
{		
	if( pSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )					return -1;		// 방장인지 체크
	if( 0 > i32SlotIdx || (SLOT_MAX_COUNT-1) < i32SlotIdx )					return -2;		// Slot번호가 정상인지 확인
	if( !IsChallengeMode() )												return -3;		// 챌린지모드인지 확인
	if( (i32SlotIdx % 2) == (m_Info._i32MainSlotIdx%2) )					return -4;		// AI 슬롯인지 확인

	if( false == m_pRoomMode->CheckRespawnAi( i32SlotIdx ) )				return 1;		// Ai 부활 가능한지 확인

	m_Info.m_aui32AILevel[i32SlotIdx]			= m_Info.m_i8currentDifficultyLevel;
	// 연속킬 Reset
	m_Info.m_aui8A_KillCount[ i32SlotIdx ]		= 0;
	m_Info.m_aui8A_HeadShot[ i32SlotIdx ]		= 0;
	m_Info.m_aui8A_SlugShot[ i32SlotIdx ]		= 0;
	m_Info._aui8A_Mission[i32SlotIdx]	= 0;
	m_Info._aui32RespawnCount[ i32SlotIdx ]++;		// Add Respawn Count
	SET_USER_ALIVE( i32SlotIdx );					// 유저 Alive 처리
	//ReSpawnReq( i32SlotIdx );

	//Packet 보내기 
	// vv3 - check
	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK);
	SendPacket.WriteData( &i32SlotIdx,		sizeof(INT32) );
	m_pRoomMode->AddRespawnAiPacket( &SendPacket, i32SlotIdx );
	_SendPacketBattlePlayer(&SendPacket);

	m_i32TotalRespawnIdx++;
	return 1;
}

void CRoom::GMPauseResult_C( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_GM_PAUSE_ACK );
	SendPacket.WriteData( &i32Result, sizeof( INT32 ) );

	if ( EV_SUCCESSED( i32Result ) )
	{
		m_bGMPause = TRUE;
		m_ui32ATPauseBegin = i3ThreadTimer::GetServerTime();
		SendPacket.WriteData( &m_i32PauseUserIdx, sizeof( INT32 ) );
		_SendPacketBattlePlayer( &SendPacket );
	}
	else
	{
		m_Info._pSession[m_i32PauseUserIdx]->SendPacketMessage( &SendPacket );
	}
}

INT32 CRoom::DeathBattle( INT32 iThreadIdx, CUserSession * pMainSession, DEATH_INFO_HEADER * pHeader, DEATH_INFO_CHARA * pData, UINT8 ui8RoundNum )
{	
	// 배틀 중이 아니면 데스 패킷이 의미 없다.
#ifdef USE_RESPAWN_SKIP
	if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )	return EVENT_ERROR_SUCCESS;
#endif
	if( m_Info._InfoBasic._State != ROOM_STATE_BATTLE )		return EVENT_ERROR_SUCCESS;

	// Relay 상태일때는 RoundNum 을 검사하지 않는다.
	if( m_Info._InfoAdd._NowRoundCount != ui8RoundNum &&
		GetBattleType() == BATTLE_SERVER_TYPE_DEDICATION )	return EVENT_ERROR_SUCCESS;

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return -1; 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )	return -2;// 방장만이 보낼수 있습니다.
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		//챌린지모드만 예외처리 합니다. 
		if( IsChallengeMode() )
		{
			if(pMainSession == NULL)return -4; 
			if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return -3;// 방장만이 보낼수 있습니다.
		}
		else 
		{
			pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		}
		break; 
	default: return -5;	break; 
	}

	//Set Type
	BOOL bPiercingShot	= FALSE;
	BOOL bMassKill		= FALSE;
	bool bBombing		= FALSE;
	UINT8	ui8KillTypeHigh = GET_KILLTYPE_HIGH( pHeader->_nKillType );
	UINT8	ui8KillTypeLow	= GET_KILLTYPE_LOW( pHeader->_nKillType );

	INT32	i32KillIdx		= pHeader->_nUserIdx; 
	if( i32KillIdx < 0 || i32KillIdx >= SLOT_MAX_COUNT) return EVENT_ERROR_SUCCESS;

	// High 체크
	if( CHARA_KILL_HIGH_BOMBING & ui8KillTypeHigh )
	{
		bBombing = TRUE;
	}
	// Low 체크
	switch( ui8KillTypeLow )
	{
	case CHARA_KILL_DEFAULT		: break;								//Normal  이 부분은 그냥 셋 합니다.
	case CHARA_KILL_PIERCING	: bPiercingShot		= TRUE;		m_Info._aui16M_PiercingShot[ i32KillIdx ]++;	break;	//Piercing
	case CHARA_KILL_MASS		: bMassKill			= TRUE;		m_Info._aui16M_MassKill[ i32KillIdx ]++;		break;	//Mass
	default:
		I3TRACE("[CRoom::DeathBattle] Kill Type Error \n");
		return -6;
	}

	TYPE_ROOM_KILL_EXPPOINT* pKillExpPoint = m_pRoomMode->GetKillExpPoint();
	UINT16	ui16GainPoint	= 0;

	if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i32KillIdx]._State )
	{
		// 챌린지모드의 경우 AI NPC가 죽일때 패킷을 던진다.
		if( !IsChallengeMode() )
		{
			I3TRACE("[CRoom::DeathBattle] KillIdx  Error \n");
			return -7;
		}
	}

	UINT32	ui32WeaponNum	= pHeader->_WeaponInfo;
	UINT8	ui8PreKillCount	= m_Info.m_aui8A_KillCount[i32KillIdx];	

	if( ui32WeaponNum == MAKE_ITEM_ID( ITEM_TYPE_PRIMARY, WEAPON_CLASS_DINO, 16 ) )
	{
		bBombing = TRUE;
	}

	//무기에 대해서 검사하는 것이 있어야 합니다. 기존에 있는 무기인지를 검사하는 무기 
	UINT32 ui32Weapon_Slot	= Weapon::GetUsageFromItemID(ui32WeaponNum );
	UINT32 ui32Weapon_Class = GET_ITEM_SUBTYPE( ui32WeaponNum );
	//UINT32 ui32Weapon_Num   = GET_ITEM_FLAG_NUMBER( ui32WeaponNum );

	if( ( WEAPON_CLASS_COUNT <= ui32Weapon_Class ) || ( CHAR_EQUIPMENT_WEAPON_COUNT <= ui32Weapon_Slot ) )
	{
		I3TRACE("[CRoom::DeathBattle] Weapon Info Error \n");
		return -8; 
	}

	UINT32 ui32DeathMyState;
	UINT32 ui32DeathEnemyState;
	UINT32 ui32AccMessage = 0x00000000; 
	UINT32 ui32DeathType; 
	INT32 i32DeathIdx = 0;	
	INT8 i8DeathWeaponClass = 0;

	BOOL bUpdateData; 
	for( INT32 i = 0 ; i < pHeader->_nCount ; i++ )
	{
		ui32DeathMyState	= GET_DEATHINFO_MY_TYPE(	pData[i]._DeathInfo );
		ui32DeathEnemyState	= GET_DEATHINFO_ENEMY_TYPE( pData[i]._DeathInfo );
		ui32DeathType		= GET_DEATHINFO_DEATH_TYPE(	pData[i]._DeathInfo );
		i32DeathIdx			= GET_DEATHINFO_IDX_TYPE(	pData[i]._DeathInfo );
		if (i32DeathIdx < 0 || i32DeathIdx >= SLOT_MAX_COUNT) return EVENT_ERROR_SUCCESS;

		if( !IsChallengeMode() )
		{
			m_Info._pSession[i32DeathIdx]->m_ui32RTBattleDeath = GetRoomBattleTime();
		}

		// 계속 리스폰 되는 모드는 살아있는 시간만 배틀 타임으로 계산
		if( m_pRoomMode->GetRespawnOption() == ROOM_RESPAWN_CONTINUE )
		{
			if( m_Info._pSession[i32DeathIdx]->m_ui32RTRespawn > 0 )
				m_Info._pSession[i32DeathIdx]->m_ui32RTAccActualBattle += GetRoomBattleTime() - m_Info._pSession[i32DeathIdx]->m_ui32RTRespawn;
		}

		i8DeathWeaponClass	= pData[i]._ui8Weapon;
		m_stKilledUserWeapon[i32KillIdx]._i8WeaponClass[m_stKilledUserWeapon[i32KillIdx]._i8Idx] = i8DeathWeaponClass;
		m_stKilledUserWeapon[i32KillIdx].IncreaseIdx();

		BOOL bChainStoppper	= FALSE;
		BOOL bHeadShot		= FALSE;
		BOOL bChainHeadShot = FALSE;
		BOOL bChainSlugger	= FALSE;
		BOOL bSuicide		= FALSE;
		bUpdateData			= TRUE;

		// 죽어있는 유저는 또 죽일수 없다.
		if( IS_USER_DEATH( i32DeathIdx ) || m_bDeathFlag[i32DeathIdx] )				return EVENT_ERROR_SUCCESS;

		//데쓰 인덱스 확인 
		if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i32DeathIdx]._State )
		{
			// 챌린지모드의 경우 AI NPC가 죽을때 패킷을 던진다.
			if( !IsChallengeMode() )
			{
				I3TRACE("[CRoom::DeathBattle] i32DeathIdx Error \n");
				bUpdateData = FALSE;
			}
		}

		//같은 편이 오브젝트로 죽였을때.
		if( i32KillIdx != i32DeathIdx )
		{
			if((i32KillIdx % 2) == (i32DeathIdx % 2))	bUpdateData = FALSE;
		}

		if( bUpdateData )
		{	
			LOG_KILL_DEATH_INFO	LogInfo;
			LogInfo.m_ui32WeaponNum		= ui32WeaponNum; 
			LogInfo.m_ui32DeathType		= ui32DeathType;

			LogInfo.m_r32Kill_X			= pHeader->_KillPosition[0]; 
			LogInfo.m_r32Kill_Y			= pHeader->_KillPosition[1]; 
			LogInfo.m_r32Kill_Z			= pHeader->_KillPosition[2]; 
			LogInfo.m_r32Death_X		= pData[i]._DeathPosition[0];
			LogInfo.m_r32Death_Y		= pData[i]._DeathPosition[1];
			LogInfo.m_r32Death_Z		= pData[i]._DeathPosition[2];

			if( !IsChallengeMode() )
				g_pModuleContextNC->LogSendMessage(	iThreadIdx, LOG_MESSAGE_U_R_DEATH, m_Info._pSession[i32DeathIdx], m_Info._pSession[i32KillIdx], this, &LogInfo );


			if( i32KillIdx == i32DeathIdx )
			{
				bSuicide = TRUE;
			}
			else
			{
				
				MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_KILL,  0,  m_Info._pSession[i32DeathIdx]);
				MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_USE_WEAPON_KILL,  ui32WeaponNum/*죽일때 사용한 무기*/ );
				MedalMissionCheck( iThreadIdx, m_Info._pSession[i32DeathIdx],  MEDAL_MC_ACTION_ID_DEATH );
								
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_KILL,					 &ui32WeaponNum );
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_KILL_WEAPONCLASS,		 &m_stKilledUserWeapon[i32KillIdx] );
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_DOUBLE_KILL_WEAPONCLASS, &m_stKilledUserWeapon[i32KillIdx] );
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_TRIPLE_KILL_WEAPONCLASS, &m_stKilledUserWeapon[i32KillIdx] );
				SetUserMissionCard( m_Info._pSession[i32DeathIdx], ACTION_ID_DEATH );
				
				if( m_Info._i32StageType == STAGE_MODE_ESCAPE && m_Info._MiDinoInfo.IsDinoIdx((UINT8)i32KillIdx) )
				{
					SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_KILL_MAN,			 &ui32WeaponNum );
				}
				
				
				//--------------------------------------------------------------------------------------------------
				//킬 유저 점수 계산 
				//--------------------------------------------------------------------------------------------------
				//1. Check HeadShot 
				switch(ui32DeathType)
				{
				case CHARA_DEATH_A		:					// 일반적인 무기로 죽을때 
				case CHARA_DEATH_B		:					// C4로 죽을때 															
				case CHARA_DEATH_F		:					// 떨어져서 죽는거다. 
				case CHARA_DEATH_O		:					// 폭탄이 터져서 죽는거다. 
					{
						m_Info.m_aui8A_HeadShot[ i32KillIdx ] = 0;
					}
					break;		
				case CHARA_DEATH_H		:					// 헤드샷
					{
						if( m_Info.m_aui8A_HeadShot[ i32KillIdx] )
						{	// 체인 헤드샷
							bChainHeadShot= TRUE;
							// 챌린지 모드 Headshot Count 점수
							m_Info._aui16AccChallengeScore[ i32KillIdx ] += 4;
						}
						else
						{	// 챌린지 모드 Headshot Count 점수
							m_Info._aui16AccChallengeScore[ i32KillIdx ] += 2;
						}

						bHeadShot = TRUE;
						m_Info.m_aui8A_HeadShot[ i32KillIdx ]++;										// 헤드샷 기록
						m_Info._aui16M_Headshot[ i32KillIdx ]++;
					}
					break;
				default					:	I3TRACE("DeathBattle Error Errrrrrrror \n"); break; 
				}

				if( CHAR_EQUIPMENT_WEAPON_SECONDARY == ui32Weapon_Slot )
				{	// 챌린지 모드 보조 무기 점수
					m_Info._aui16AccChallengeScore[ i32KillIdx ] += 1;
				}
				else if ( CHAR_EQUIPMENT_WEAPON_THROWING1 == ui32Weapon_Slot )
				{	// 챌린지 모드 투척 무기 점수
					m_Info._aui16AccChallengeScore[ i32KillIdx ] += 2;
				}
				//2. Check Slug
				if( CHAR_EQUIPMENT_WEAPON_MELEE == ui32Weapon_Slot )
				{
					if( 0 < m_Info.m_aui8A_SlugShot[ i32KillIdx ] )
					{
						bChainSlugger = TRUE;

						m_Info._aui16M_ChainSlugger[ i32KillIdx ]++;
						// 챌린지 모드 Slug Count 점수
						m_Info._aui16AccChallengeScore[ i32KillIdx ] += 5;
					}
					else
					{
						// 챌린지 모드 Slug Count 점수( 보태기)
						m_Info._aui16AccChallengeScore[ i32KillIdx ] += 2;
					}

					m_Info.m_aui8A_SlugShot[ i32KillIdx ]++;

					// 챌린지 모드 근접 무기 점수(기본)
					m_Info._aui16AccChallengeScore[ i32KillIdx ] += 3;
				}
				else
				{
					m_Info.m_aui8A_SlugShot[ i32KillIdx ] = 0; 
				}

				m_Info._aM_UserScore[ i32KillIdx ].m_KillCount++;				//Add KillIndex
				m_Info._aUserScoreRound[ i32KillIdx ].m_KillCount++;

				// 챌린지 모드 기본 킬 점수
				if (CHALLENGE_DIFFICULTY_LEVEL_NONE < m_Info.m_i8currentDifficultyLevel && m_Info.m_i8currentDifficultyLevel <= CHALLENGE_DIFFICULTY_LEVEL_HIGH)
				{
					m_Info._aui16AccChallengeScore[ i32KillIdx ] += (UINT16)g_ai32ChallengeKillPoint[m_Info.m_i8currentDifficultyLevel];
				}
				else
				{
					m_Info._aui16AccChallengeScore[ i32KillIdx ] += 10;
				}

				//Cal Money 먼저 계산 되어야함. 위치 중요
				switch( m_Info.m_aui8A_KillCount[i32KillIdx] )
				{
				case 1:
					{
						m_Info._aui16M_AccKill_1[ i32KillIdx ]++;
						if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
						{							
							ui16GainPoint	+= pKillExpPoint->_i16MultiPt;
						}

						// 챌린지 모드 Kill Count 점수
						m_Info._aui16AccChallengeScore[ i32KillIdx ] += 2;
						
						MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_DOUBLE_KILL );											
						SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_DOUBLE_KILL, &ui32WeaponNum );
						
					}
					break; 
				case 2:	
					{
						m_Info._aui16M_AccKill_2[ i32KillIdx ]++;
						if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
						{							
							ui16GainPoint	+= (pKillExpPoint->_i16MultiPt*2);
						}

						// 챌린지 모드 Kill Count 점수
						m_Info._aui16AccChallengeScore[ i32KillIdx ] += 4;
						
						MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_TRIPLE_KILL );						
						SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_TRIPLE_KILL, &ui32WeaponNum );
						
					}
					break; 
				case 3:	
				case 4:	
					{
						m_Info._aui16M_AccKill_3[ i32KillIdx ]++;
						if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
						{							
							ui16GainPoint	+= (pKillExpPoint->_i16MultiPt*3);
						}

						// 챌린지 모드 Kill Count 점수
						m_Info._aui16AccChallengeScore[ i32KillIdx ] += 7;
						
						MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_CHAINKILLER );												
						SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINKILLER );
						
					}
					break; 
				}

				// 각 모드별 추가 Kill Bouns
				if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
				{
					m_pRoomMode->UserKillBounsV1( i32KillIdx, ui32DeathType, ui32DeathMyState, ui32DeathEnemyState, GetRoomBattleTime() );
				}
				else
				{
					ui16GainPoint = m_pRoomMode->UserKillBounsV2( i32KillIdx, ui32DeathType, ui32DeathMyState, ui32DeathEnemyState, GetRoomBattleTime() );
				}

				// 최다 멀티킬 계산. 더블킬 이상부터. AI는 Pass.
				if( 0 < m_Info._pSession[i32KillIdx]->m_i64UID ) 
				{
					m_ui8MultiKillCount[i32KillIdx]++;
					m_ui8MultiKillCount[i32KillIdx] = MIN( m_ui8MultiKillCount[i32KillIdx] , MAX_MULTI_KILL_COUNT );

					if( m_ui8MultiKillCount[i32KillIdx] > 1 &&  m_ui8MultiKillCount[i32KillIdx] >= m_stMultiKill.m_ui8MaxMultiKillCount )
					{
						m_stMultiKill.m_ui8MaxMultiKillCount = m_ui8MultiKillCount[i32KillIdx];
						m_stMultiKill.m_ui16MultiKillUser |= 1 << i32KillIdx;
					}
				}

				//Set Max Count Number
				m_Info.m_aui8A_KillCount[i32KillIdx]++;
				if( m_Info.m_aui8A_KillCount[i32KillIdx] > 4 )	m_Info.m_aui8A_KillCount[i32KillIdx] = 4;

				//Chain Stopper
				if( m_Info.m_aui8A_KillCount[ i32DeathIdx ] == 4)
				{
					bChainStoppper = TRUE;
					// 챌린지 모드 Chain stopper Count 점수
					m_Info._aui16AccChallengeScore[ i32KillIdx ] += 5;
				}
			}

			//--------------------------------------------------------------------------------------------------
			// Kill, Death 계산
			if( i32KillIdx != i32DeathIdx )
			{	// 다른 사람을 죽였을 경우
				//Team 전적 계산	
				INT32 i32TeamIdx = i32KillIdx % 2;
				m_Info._aTeamScoreTotal[ i32TeamIdx ].m_KillCount++;
				m_Info._aTeamScoreRound[ i32TeamIdx ].m_KillCount++;

				if(FALSE == m_Info._aSlotInfo[i32DeathIdx]._bIsGmObserverMode)
				{
					if( i32TeamIdx == 0 )
					{
						m_Info._aTeamScoreTotal[ 1 ].m_DeathCount++;
						m_Info._aTeamScoreRound[ 1 ].m_DeathCount++;
					}
					else
					{
						m_Info._aTeamScoreTotal[ 0 ].m_DeathCount++;
						m_Info._aTeamScoreRound[ 0 ].m_DeathCount++;
					}
				
					//죽은 유저 점수 계산
					m_Info._aM_UserScore[ i32DeathIdx ].m_DeathCount++;
					m_Info._aUserScoreRound[ i32DeathIdx ].m_DeathCount++;

					if (i32KillIdx != pData[i]._ui8AssistIdx)
					{
						m_Info._aM_UserScore[pData[i]._ui8AssistIdx].m_AssistCount++;
						m_Info._aUserScoreRound[pData[i]._ui8AssistIdx].m_AssistCount++;
					}
				}
			}
			else
			{	// 자살일 경우
				if( false == bBombing )
				{	// 엑시드의 경우 폭발로 인해 자신도 함께 죽은 경우 Death 를 증가 시키지 않습니다.
					//Team 전적 계산
					if(FALSE == m_Info._aSlotInfo[i32DeathIdx]._bIsGmObserverMode)
					{
						INT32 i32TeamIdx = i32DeathIdx % 2;
						m_Info._aTeamScoreTotal[ i32TeamIdx ].m_DeathCount++;
						m_Info._aTeamScoreRound[ i32TeamIdx ].m_DeathCount++;
	
						//죽은 유저 점수 계산
						m_Info._aM_UserScore[ i32DeathIdx ].m_DeathCount++;// 매치킬데스
						m_Info._aUserScoreRound[ i32DeathIdx ].m_DeathCount++;
					}
				}
				else
				{
					// 애시드의 경우 Death 를 증가시키지 않고 Bombing Count 를 세도록 합니다. - 서동권
					m_Info._ai32AcidBombingCount[ i32DeathIdx ]++;
				}


				if (i32KillIdx != pData[i]._ui8AssistIdx)
				{
					m_Info._aM_UserScore[pData[i]._ui8AssistIdx].m_AssistCount++;
					m_Info._aUserScoreRound[pData[i]._ui8AssistIdx].m_AssistCount++;
				}
			}
		}		

		////////////////////////////////////////////////////////////////////////
		//Set Data
		pData[i]._DeathInfo = pData[i]._DeathInfo & 0xFF0000FF; 

		// 1. Message 	
		if( bPiercingShot )
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_PIERCINGSHOT;
			if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
			{
				ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Pier;
			}
			m_Info._aui16AccChallengeScore[ i32KillIdx ] += 7;
		}
		if( bMassKill ) 
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_MASSKILL			; 
			if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
			{				
				ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Mass;
			}
			m_Info._aui16AccChallengeScore[ i32KillIdx ] += 7;
		}
		if( bChainStoppper ) 
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_CHAINSTOPPER		; 
			if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
			{
				ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Stop;
			}
			
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_CHAINSTOPPER );			
			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINSTOPPER );
			
		}
		if( bHeadShot )
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_HEADSHOT;
			
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_HEADSHOT );			
			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_HEADSHOT, &ui32WeaponNum );
			
		}
		if( bChainHeadShot ) //****
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_CHAINHEADSHOT	; 
			if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
			{
				ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Head;
			}
			
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_CHAINHEADSHOT );			
			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINHEADSHOT );
			
		}
		if( bChainSlugger ) //****
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_CHAINSLUGGER		;
			if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
			{			
				ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Slug;
			}
			
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_CHAINSLUGGER );			
			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINSLUGGER );
			
		}

		switch( ui32DeathMyState )
		{
		case CHARA_DEATH_MY_STATE_ASSIST:
			{
				pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_ASSISTKILL;
				
				MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_KILL_ASSIST );								
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_KILL_ASSIST );
				
			}
			break;
		}

		switch( ui32DeathEnemyState )
		{
		case CHARA_DEATH_ENEMY_STATE_DEFENCE:
			{	// 오브젝트 공격 중인 적을 사살
				pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_OBJECTDEFENCE; 	
				
				MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_PROTECT );								
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_PROTECT );
				
			}
			break;
		case CHARA_DEATH_ENEMY_STATE_ESCAPE:
			{
				// 탈출미션 탈출지점 근처에 접근한 적을 죽임 - 공룡이 인간을 죽일때만 적용 되어야 함.
				UINT8 IDinoTeam = m_pRoomMode->GetAtkDefChange() ? 1 : 0;

				if( ( m_Info._pSession[i32KillIdx]->m_i32SlotIdx % TEAM_COUNT ) == IDinoTeam )
					pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_INTERCEPT;
			}
			break;
		}

		ui32AccMessage = ui32AccMessage |  pData[i]._DeathInfo; 

		m_bDeathFlag[i32DeathIdx] = true;
	}
	
	{
		i3NetworkPacket SendPacket(PROTOCOL_BATTLE_DEATH_ACK); 
		SendPacket.WriteData( pHeader,					sizeof(DEATH_INFO_HEADER)); 
		SendPacket.WriteData( pData,					(sizeof(DEATH_INFO_CHARA)	* pHeader->_nCount)); 
		SendPacket.WriteData( m_Info._aTeamScoreTotal,	(sizeof(SCORE_INFO)			* TEAM_COUNT)); 
		SendPacket.WriteData( m_Info._aM_UserScore,		(sizeof(SCORE_INFO)			* SLOT_MAX_COUNT)); 
		SendPacket.WriteData( &ui8PreKillCount,			sizeof(UINT8));

		if ( IsChallengeMode() )
			SendPacket.WriteData( &m_Info._aui16AccChallengeScore[i32KillIdx], sizeof(UINT16));
		else
			SendPacket.WriteData( &ui16GainPoint, sizeof(UINT16) );

		SendPacket.WriteData( m_Info._aui16TotalRoundCount, (sizeof(UINT16) * TEAM_COUNT) );

		_SendPacketBattlePlayer(&SendPacket);
	}

	// 한번만 체크되어야 할 미션들.
	if( bMassKill) 
	{
		
		MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_MASS_KILL );		
		SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_MASS_KILL );
		
	}

	if( bPiercingShot )
	{
		
		MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_PIERCING );				
		SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_PIERCING );
		
	}

	return EVENT_ERROR_SUCCESS; 
}

INT32 CRoom::ProcessMissionCardAndMedal(INT32 iThreadIdx, DEATH_INFO_HEADER * pDeathHeader, DEATH_INFO_CHARA* pDeathInfos)
{
#ifdef USE_RESPAWN_SKIP
	if( m_Info._i32MiStateType != MISSION_STATE_BATTLE )
		return EVENT_ERROR_SUCCESS;
#endif
	if( m_Info._InfoBasic._State != ROOM_STATE_BATTLE )
		return EVENT_ERROR_SUCCESS;

	INT32 i32KillIdx = pDeathHeader->_nUserIdx;
	if( i32KillIdx < 0 || SLOT_MAX_COUNT <= i32KillIdx )
		return false;

	if( m_Info._aSlotInfo[i32KillIdx]._State != SLOT_STATE_BATTLE )
	{
		if( !IsChallengeMode() )
		{
			I3TRACE("[CRoom::DeathBattle] KillIdx  Error \n");
			return false;
		}
	}

	UINT32 ui32KillerWeapon	= pDeathHeader->_WeaponInfo;
	if( WEAPON_CLASS_COUNT <= GET_ITEM_SUBTYPE(ui32KillerWeapon) || WEAPON_SLOT_COUNT <= GET_ITEM_TYPE(ui32KillerWeapon) )
	{
		I3TRACE("[CRoom::DeathBattle] Weapon Info Error \n");
		return false;
	}

	bool bProcessMassKill = false;
	for( INT32 i = 0 ; i < pDeathHeader->_nCount; ++i )
	{	
		INT32 i32DeathIdx = GET_DEATHINFO_IDX_TYPE(pDeathInfos[i]._DeathInfo);
		if( i32DeathIdx < 0 || SLOT_MAX_COUNT <= i32DeathIdx )
			return false;

		if( !IsChallengeMode() )
			m_Info._pSession[i32DeathIdx]->m_ui32RTBattleDeath = GetRoomBattleTime();

		if( m_pRoomMode->GetRespawnOption() == ROOM_RESPAWN_CONTINUE )
		{
			if( 0 < m_Info._pSession[i32DeathIdx]->m_ui32RTRespawn )
				m_Info._pSession[i32DeathIdx]->m_ui32RTAccActualBattle += GetRoomBattleTime() - m_Info._pSession[i32DeathIdx]->m_ui32RTRespawn;
		}

		m_stKilledUserWeapon[i32KillIdx]._i8WeaponClass[m_stKilledUserWeapon[i32KillIdx]._i8Idx] = pDeathInfos[i]._ui8Weapon;
		m_stKilledUserWeapon[i32KillIdx].IncreaseIdx();

		if( IS_USER_DEATH(i32DeathIdx) || m_bDeathFlag[i32DeathIdx] )
			return false;

		if( (pDeathInfos[i]._DeathInfo & KILLING_MESSAGE_MASSKILL) && bProcessMassKill )
		{
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_MASS_KILL );

			bProcessMassKill = true;
		}

		if( pDeathInfos[i]._DeathInfo & KILLING_MESSAGE_PIERCINGSHOT )
		{
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_PIERCING );
		}

		if( pDeathInfos[i]._DeathInfo & KILLING_MESSAGE_CHAINSTOPPER )
		{
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_CHAINSTOPPER );
		}

		if( pDeathInfos[i]._DeathInfo & KILLING_MESSAGE_HEADSHOT )
		{
			MedalMissionCheck(iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_HEADSHOT);
		}

		if( pDeathInfos[i]._DeathInfo & KILLING_MESSAGE_CHAINHEADSHOT )
		{
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_CHAINHEADSHOT );
		}

		if( pDeathInfos[i]._DeathInfo & KILLING_MESSAGE_CHAINSLUGGER )
		{
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_CHAINSLUGGER );
		}

		if( GET_DEATHINFO_MY_TYPE(pDeathInfos[i]._DeathInfo) == CHARA_DEATH_MY_STATE_ASSIST )
		{
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_KILL_ASSIST );
		}

		if( GET_DEATHINFO_ENEMY_TYPE(pDeathInfos[i]._DeathInfo) == CHARA_DEATH_ENEMY_STATE_DEFENCE )
		{
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_PROTECT );
		}

		m_bDeathFlag[i32DeathIdx] = true;

		if( m_Info._aSlotInfo[i32DeathIdx]._State != SLOT_STATE_BATTLE )
		{
			if( !IsChallengeMode() )
			{
				I3TRACE("[CRoom::DeathBattle] i32DeathIdx Error \n");
				continue;
			}
		}
		
		if( i32KillIdx == i32DeathIdx )
			continue;

		if( (i32KillIdx % TEAM_COUNT) == (i32DeathIdx % TEAM_COUNT) )
			continue;

		MedalMissionCheck(iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_KILL, 0, m_Info._pSession[i32DeathIdx]);
		MedalMissionCheck(iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_USE_WEAPON_KILL, ui32KillerWeapon);
		MedalMissionCheck(iThreadIdx, m_Info._pSession[i32DeathIdx], MEDAL_MC_ACTION_ID_DEATH);

		switch( m_Info.m_aui8A_KillCount[i32KillIdx] )
		{
		case 1:
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx], MEDAL_MC_ACTION_ID_DOUBLE_KILL);
			break;

		case 2:	
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_TRIPLE_KILL);
			break;

		case 3:	
		case 4:
			MedalMissionCheck( iThreadIdx, m_Info._pSession[i32KillIdx],  MEDAL_MC_ACTION_ID_CHAINKILLER );
			break; 
		}
	}

	return true; 
}

UINT16 CRoom::_ResultUpdate_M( INT32 i32Win, BOOL bNormalEnd )
{
	UINT16 ui16BattleEndUser = 0x0000;	// 정상 종료 User 확인
	//USER_INFO_RECORD		RecordValue[TEAM_COUNT];
	INT32					ai32WinTeam[ TEAM_COUNT ];
	INT32					ai32LoseTeam[ TEAM_COUNT ];
	INT32					ai32DrawTeam[ TEAM_COUNT ];

	// ClanBattle 를 위한 변수
	UINT8				ui8BattleUserCount = 0;
	CLAN_RESULT			aClanResult[ TEAM_COUNT ];
	CLAN_RESULT_USER	aClanResultUser[ SLOT_MAX_COUNT ];
	i3mem::FillZero( aClanResult, sizeof(CLAN_RESULT) * TEAM_COUNT);
	
	// 0 : 인원수에 따른 카운트(킬/데스 가중치 Red) 
	// 1 : 방어미션, 공룡미션일때 킬/데스 가중치 Blue
	// 2 : 방어미션 일때 임무 가중치 Red
	// 3 : 방어미션 일때 임무 가중치 Blue
	REAL32 rArrBattleCountEndPercent[4] = { 1.0f, 1.0f, 1.0f, 1.0f };		
	REAL32 r32SubPercent				= 1.0f;		//비정상 종료 일때
	REAL32 r32BattleModeInvariable		= 1.0f;
	REAL32 r32AsymExpPercent			= 1.0f;
	REAL32 r32AsymPointPercent			= 1.0f;

	INT32 i32AllUser					= 0;
	INT32 i32TeamUser[TEAM_BOTH]		= { 0, 0 };

	_GetBattleUserCountAllTeam( i32AllUser, i32TeamUser[TEAM_RED], i32TeamUser[TEAM_BLUE]);

	if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
	{
		//1. 인원수에 따른 차감내용
		switch( m_Info._i32StageType )
		{
		case STAGE_MODE_DEFENCE			:
			{
				// _GetBattleCountEndPercentTeam(0);//Red
				rArrBattleCountEndPercent[TEAM_RED]			= m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_RED, i32AllUser, i32TeamUser[TEAM_RED]); // RED
				rArrBattleCountEndPercent[TEAM_BLUE]		= m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_BLUE, i32AllUser, i32TeamUser[TEAM_BLUE]); // BLUE

				rArrBattleCountEndPercent[TEAM_RED + 2]		= m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_RED, i32AllUser, i32TeamUser[TEAM_RED]); // RED
				rArrBattleCountEndPercent[TEAM_BLUE + 2]	= m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_BLUE, i32AllUser, i32TeamUser[TEAM_BLUE]); // BLUE
			}
			break;
		case STAGE_MODE_DESTROY			:
		case STAGE_MODE_ESCAPE			: 
		case STAGE_MODE_CROSSCOUNT		:
			{
				rArrBattleCountEndPercent[TEAM_RED] = m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_RED, i32AllUser, i32TeamUser[TEAM_RED]); // RED
				rArrBattleCountEndPercent[TEAM_BLUE] = m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_BLUE, i32AllUser, i32TeamUser[TEAM_BLUE]); // BLUE
			}
			break;	
		default:
			{
				rArrBattleCountEndPercent[0] = _GetBattleCountEndPercent();
			}
			break;
		}
	}
	else
	{
		rArrBattleCountEndPercent[TEAM_RED]  = m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_RED, i32AllUser, i32TeamUser[TEAM_RED]); // RED
		rArrBattleCountEndPercent[TEAM_BLUE] = m_pRoomMode->GetSubPercentOfUserCountTeam(TEAM_BLUE, i32AllUser, i32TeamUser[TEAM_BLUE]); // BLUE

		r32BattleModeInvariable		= _GetBattleModeInvariable();
	}

	if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
	{
		//2. 비정상 종료일때 차감내용
		if( FALSE == bNormalEnd )
		{
			r32SubPercent = m_pRoomMode->GetSubPercentOfUnusualEnd( i32Win );
		}
	}
	//#endif

	//1. Set Win Count	
	switch(i32Win)
	{
	case TEAM_RED_WIN :		//레드 팀 이김
		ai32WinTeam[TEAM_RED]	= ai32LoseTeam[TEAM_BLUE]	= 1; 
		ai32WinTeam[TEAM_BLUE]	= ai32LoseTeam[TEAM_RED]	= 0; 
		ai32DrawTeam[TEAM_RED]	= ai32DrawTeam[TEAM_BLUE]	= 0;
		break;
	case TEAM_BLUE_WIN :	//블루 팀 이김 
		ai32WinTeam[TEAM_RED]	= ai32LoseTeam[TEAM_BLUE]	= 0; 
		ai32WinTeam[TEAM_BLUE]	= ai32LoseTeam[TEAM_RED]	= 1; 
		ai32DrawTeam[TEAM_RED]	= ai32DrawTeam[TEAM_BLUE]	= 0;
		break;
	default:				//싱글 //비김
		ai32WinTeam[TEAM_RED]	= ai32WinTeam[TEAM_BLUE]	= 0; 
		ai32LoseTeam[TEAM_RED]	= ai32LoseTeam[TEAM_BLUE]	= 0; 
		ai32DrawTeam[TEAM_RED]	= ai32DrawTeam[TEAM_BLUE]	= 1; 
		break; 
	}

	//2. Change State & Update Record
	INT32 i32TeamIdx = 0; 
	INT32 i32EnemyTeamIdx = 0; 

	INT8	i8BattleUserCount = 0;

	for(INT i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( m_Info._aSlotInfo[i]._State != SLOT_STATE_BATTLE ) continue;
		if( TRUE == m_Info._aSlotInfo[i]._bIsGmObserverMode ) continue;

		m_Info._aui16AccExp[i]		= m_pRoomMode->GetMatchAccExp(i, m_Info._aui16AccExp[i]);
		m_Info._aui16AccPoint[i]	= m_pRoomMode->GetMatchAccPoint(i, m_Info._aui16AccPoint[i]);
		i8BattleUserCount++;
	}

	INT8 ai8Ranking[SLOT_MAX_COUNT] = {0,};
	//UINT32 ui32EventDate = ((g_pContextMain->m_i32Year - 2000) * 100000000) + (g_pContextMain->m_i32Month * 1000000) + (g_pContextMain->m_i32Day * 10000) + (DAILY_RECORD_INIT_TIME_HOUR * 100);

	m_pRoomMode->GetRanking( ai8Ranking, i8BattleUserCount, GET_RANKING_RAWEXP );


	for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		if( TRUE == m_Info._aSlotInfo[i]._bIsGmObserverMode )
		{
			ui16BattleEndUser = ui16BattleEndUser | (0x01 << i); 
			continue;
		}

		if( m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE )
		{
			S2MO_CHAR_BASE_INFO stBaseInfo;

			// 자동 팀 밸런스를 위해 승패 보너스를 추가하지 않은 경험치 순위 비율을 기록 (소수점 세자리 (*1000))
			m_Info._pSession[i]->m_ui16PrevExpRank = (ai8Ranking[i]*1000)/i8BattleUserCount;
			
			m_Info._pSession[i]->SetLastUserCount( &m_Info );
			m_Info._pSession[i]->m_ui32PlayTime							= GetRoomBattleTime() - m_Info._pSession[i]->m_ui32RTBattleStart;
			m_Info._pSession[i]->m_ui32LastPlayTime						= m_Info._pSession[i]->m_ui32PlayTime;
			m_Info._pSession[i]->m_UserInfoServer.m_ui32LoginBattleTime	+= m_Info._pSession[i]->m_ui32PlayTime;
			m_Info._pSession[i]->m_UserInfoServer.m_ui32TotalBattleTime	+= m_Info._pSession[i]->m_ui32PlayTime;


			//플레이한 캐릭터에 배틀타임 누적
			INT32 i32MainSlot = m_Info._pSession[i]->m_pDesign->GetCharaSlotByTeam(i % TEAM_COUNT, m_Info._pSession[i]);
			m_Info._pSession[i]->m_MultiSlot.GetCharBaseInfo(i32MainSlot, &stBaseInfo );
			stBaseInfo.m_ui32BattleTime								+= m_Info._pSession[i]->m_ui32PlayTime;
			stBaseInfo.m_ui32SkillRelTime							+= m_Info._pSession[i]->m_ui32PlayTime;		//플레이 시간만큼 메인스킬 개방시간을 추가한다.(부스터 추가분은 _ResultUpdateUser_M 에서 가산함)
			m_Info._pSession[i]->m_MultiSlot.SetCharBaseInfo(i32MainSlot, &stBaseInfo);
			m_Info._pSession[i]->ChangeCharaDataBaseInfo( ( (UINT64)0x1 << i32MainSlot ) );

			// 실제 배틀에 참여했던 시간을 누적해서 기록
			//m_Info._pSession[i]->m_UserInfoBasic._ui32ActualBattleTime += m_Info._pSession[i]->m_ui32AccActualBattleTime;
			
			DaliyPlayTime( MAIN_THREAD_IDX, i, TRUE );

			// 어뷰징 체크 
			UINT32	ui32PlayTime = 0;
			ui32PlayTime	=  GetRoomBattleTime() - m_Info._pSession[i]->m_ui32RTBattleStart ; // 플레이 시간 = 현재 시간 - 배틀 시작 시간
			if( ui32PlayTime > ABUSING_CHECK_MIN_PLAY_TIME  )  _CheckAbusing( i, ui32PlayTime );	 // 플레이 타임 체크

			ui16BattleEndUser = ui16BattleEndUser | (0x01 << i); 

			if(	!IsChallengeMode() && !IsTutorialMode() && STAGE_EVENT_SEASON != g_pContextMain->GetStageEvent(m_Info._InfoBasic._StageID) )
			{
				i32TeamIdx = i % 2;				// 자기팀
				i32EnemyTeamIdx	= (i+1) % 2;	// 상대팀

				r32AsymExpPercent		= m_pRoomMode->GetPercentOfAsymmetryUserExp(i32TeamIdx, i32AllUser, i32TeamUser[i32TeamIdx]);
				r32AsymPointPercent		= m_pRoomMode->GetPercentOfAsymmetryUserPoint(i32TeamIdx, i32AllUser, i32TeamUser[i32TeamIdx]);

				if( 1 == ai32WinTeam[i32TeamIdx] ) // 자기 팀이 승리.
				{					
					MedalMissionCheck( MAIN_THREAD_IDX, m_Info._pSession[i],  MEDAL_MC_ACTION_ID_WIN );					
					SetUserMissionCard( m_Info._pSession[i], ACTION_ID_WIN, &ai32WinTeam[i32TeamIdx] );					
				}
				else if( 1 == ai32LoseTeam[i32TeamIdx] ) // 자기팀이 패배
				{
					MedalMissionCheck( MAIN_THREAD_IDX, m_Info._pSession[i], MEDAL_MC_ACTION_ID_DEFEAT);					
					SetUserMissionCard( m_Info._pSession[i], ACTION_ID_DEFEAT, &ai32LoseTeam[i32TeamIdx] );					
				}
				else if( 1 == ai32DrawTeam[i32TeamIdx] ) // 자기팀이 비김.
				{	
					MedalMissionCheck( MAIN_THREAD_IDX, m_Info._pSession[i], MEDAL_MC_ACTION_ID_DRAW);
				}

				if( !IsHeadHunterMode() && !IsChaosMode() && !IsSpaceMode())
				{
					// 승패수
					m_Info._pSession[i]->m_UserInfoRecord._win			+= ai32WinTeam[ i32TeamIdx ]; 
					m_Info._pSession[i]->m_UserInfoRecord._lose			+= ai32LoseTeam[ i32TeamIdx ]; 
					m_Info._pSession[i]->m_UserInfoRecord._draw			+= ai32DrawTeam[ i32TeamIdx ]; 
					// 시즌 승패수
					m_Info._pSession[i]->m_UserInfoRecord._swin			+= ai32WinTeam[ i32TeamIdx ]; 
					m_Info._pSession[i]->m_UserInfoRecord._slose		+= ai32LoseTeam[ i32TeamIdx ]; 
					m_Info._pSession[i]->m_UserInfoRecord._sdraw		+= ai32DrawTeam[ i32TeamIdx ];

					// 당일 승패
					m_Info._pSession[i]->m_UserInfoDaily._win			+= (UINT16)ai32WinTeam[ i32TeamIdx ]; 
					m_Info._pSession[i]->m_UserInfoDaily._lose			+= (UINT16)ai32LoseTeam[ i32TeamIdx ]; 
					m_Info._pSession[i]->m_UserInfoDaily._draw			+= (UINT16)ai32DrawTeam[ i32TeamIdx ]; 
					//m_Info._pSession[i]->m_UserInfoDaily._match			+= i32WinTeam[ TeamIdx ] + i32LoseTeam[ TeamIdx ] + i32DrawTeam[ TeamIdx ]; 


					// 매치카운트는 증가
					m_Info._pSession[i]->m_UserInfoRecord._match++; 
					m_Info._pSession[i]->m_UserInfoRecord._smatch++; 
					m_Info._pSession[i]->m_UserInfoRecord._dmatch++;
					m_Info._pSession[i]->m_UserInfoRecord._sdmatch++;

					// 매치킬데스
					m_Info._pSession[i]->m_UserInfoRecord._killcount	+= m_Info._aM_UserScore[i].m_KillCount; 
					m_Info._pSession[i]->m_UserInfoRecord._death		+= m_Info._aM_UserScore[i].m_DeathCount;
					m_Info._pSession[i]->m_UserInfoRecord._headshot		+= m_Info._aui16M_Headshot[i];
					m_Info._pSession[i]->m_UserInfoRecord._AssistCount += m_Info._aM_UserScore[i].m_AssistCount;
					// 시즌매치 킬데스
					m_Info._pSession[i]->m_UserInfoRecord._skillcount	+= m_Info._aM_UserScore[i].m_KillCount; 
					m_Info._pSession[i]->m_UserInfoRecord._sdeath		+= m_Info._aM_UserScore[i].m_DeathCount;
					m_Info._pSession[i]->m_UserInfoRecord._sheadshot	+= m_Info._aui16M_Headshot[i];
					m_Info._pSession[i]->m_UserInfoRecord._sAssistCount += m_Info._aM_UserScore[i].m_AssistCount;
					// d 매치 킬카운트
					m_Info._pSession[i]->m_UserInfoRecord._dkillcount	+= m_Info._aM_UserScore[i].m_KillCount;
					m_Info._pSession[i]->m_UserInfoRecord._sdkillcount	+= m_Info._aM_UserScore[i].m_KillCount;

					// 당일 킬데스
					m_Info._pSession[i]->m_UserInfoDaily._killcount		+= m_Info._aM_UserScore[i].m_KillCount; 
					m_Info._pSession[i]->m_UserInfoDaily._death			+= m_Info._aM_UserScore[i].m_DeathCount;
					m_Info._pSession[i]->m_UserInfoDaily._headshot		+= m_Info._aui16M_Headshot[i];
					m_Info._pSession[i]->m_UserInfoDaily._Assist += m_Info._aM_UserScore[i].m_AssistCount;

				}

				// 모드별로 점수 적용
				if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
				{
					switch( m_Info._i32StageType )
					{						
					case STAGE_MODE_DEFENCE:
						{
							m_Info._aui16AccExp[i] = (INT16)(m_Info._aui16AccExp[i] * rArrBattleCountEndPercent[i32TeamIdx + 2]);
							m_Info._aui16AccExp[i] = m_pRoomMode->GetMatchEndExp    ( i32Win, i, m_Info._aui16AccExp[i],   r32SubPercent * rArrBattleCountEndPercent[i32TeamIdx], r32AsymExpPercent,  bNormalEnd, GetRoomBattleTime());
							m_Info._aui16AccPoint[i] = m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * rArrBattleCountEndPercent[i32TeamIdx], r32AsymPointPercent, bNormalEnd, GetRoomBattleTime());
						}
						break;					
					case STAGE_MODE_DESTROY:
					case STAGE_MODE_ESCAPE: 
					case STAGE_MODE_CROSSCOUNT:
						{
							m_Info._aui16AccExp[i]   = m_pRoomMode->GetMatchEndExp  ( i32Win, i, m_Info._aui16AccExp[i],   r32SubPercent * rArrBattleCountEndPercent[i32TeamIdx], r32AsymExpPercent,   bNormalEnd, GetRoomBattleTime());
							m_Info._aui16AccPoint[i] = m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * rArrBattleCountEndPercent[i32TeamIdx], r32AsymPointPercent, bNormalEnd, GetRoomBattleTime());
						}
						break;
					default:
						{
							m_Info._aui16AccExp[i]		= m_pRoomMode->GetMatchEndExp  ( i32Win, i, m_Info._aui16AccExp[i],   r32SubPercent * rArrBattleCountEndPercent[0], r32AsymExpPercent,   bNormalEnd, GetRoomBattleTime() );
							m_Info._aui16AccPoint[i]	= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * rArrBattleCountEndPercent[0], r32AsymPointPercent, bNormalEnd, GetRoomBattleTime() );
						}
						break;
					}
				}
				else
				{
					m_Info._aui16AccExp[i]		= m_pRoomMode->GetMatchEndExp  ( i32Win, i, m_Info._aui16AccExp[i], rArrBattleCountEndPercent[i32TeamIdx], r32AsymExpPercent, bNormalEnd, GetRoomBattleTime() );
					m_Info._aui16AccPoint[i]	= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32BattleModeInvariable, r32AsymPointPercent, bNormalEnd, ai8Ranking, i8BattleUserCount, GetRoomBattleTime() );
				}
				//#endif

				if( (bNormalEnd == TRUE) || (i32Win == (i % TEAM_COUNT)) )
					m_Info._pSession[i]->CheckQuestItem2();
				
			}
			else
			{	// 챌린지 모드 점수 변환				
				m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccChallengeScore[i], 0.02001f, 1.0f, bNormalEnd, GetRoomBattleTime() );
		
				if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
				{
					m_Info._aui16AccPoint[i]			= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccChallengeScore[i], 0.04001f, 1.0f, bNormalEnd, GetRoomBattleTime() );
				}

				else
				{
					m_Info._aui16AccPoint[i]			= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32BattleModeInvariable, 1.0f, bNormalEnd, ai8Ranking, i8BattleUserCount, GetRoomBattleTime() );
				}
			}
						
			// 기존 이벤트 처리를 따로 관리합니다.
			_ResultUpdateUser_M( MAIN_THREAD_IDX, i );

			// 사용한 무기를 인증하는 루틴을 보냅니다.
			_SendUsedWeapon( MAIN_THREAD_IDX, m_Info._pSession[i] );

			// ClanBattle 를 위한 루틴 // 20080422
			switch( m_ui8ChannelType )
			{
			default: 
				break;
			case CHANNEL_TYPE_PCCAFE_CLAN:
			case CHANNEL_TYPE_CLAN: 
				{
					// Clan 에 대한 기록
					aClanResult[ i32TeamIdx ]._ClanIdx			= m_Info._aui32BattleClanIdx[ i32TeamIdx ];
					aClanResult[ i32TeamIdx ]._RoundWinCount	= (UINT8)m_Info._aui16TotalRoundCount[ i32TeamIdx ];

					aClanResult[ i32TeamIdx ]._Exp				+= m_Info._aui16AccExp[i] + m_Info._aui16EventExp[i];
					
					if( ai32WinTeam[i32TeamIdx] )
					{
						if (STAGE_MODE_DEATHMATCH == m_Info._i32StageType)
						{	// 단체전은 승패 여부를 라운드 카운트로 표시합니다.
							aClanResult[i32TeamIdx]._RoundWinCount = 1;
						}

						aClanResult[ i32TeamIdx ]._Win = 1;
						aClanResult[ i32TeamIdx ]._Lose = 0;
						aClanResultUser[ ui8BattleUserCount ]._WinLose	= (UINT8)ROOM_ENDTYPE_WIN;
					}
					else if( ai32LoseTeam[i32TeamIdx] )
					{
						aClanResult[ i32TeamIdx ]._Win = 0;
						aClanResult[ i32TeamIdx ]._Lose = 1;
						aClanResultUser[ ui8BattleUserCount ]._WinLose	= (UINT8)ROOM_ENDTYPE_LOSE;
					}
					else
					{
						aClanResultUser[ ui8BattleUserCount ]._WinLose	= (UINT8)ROOM_ENDTYPE_DRAW;
					}

					// User 에 대한 기록/스킬부스터 사용여부에 따른 스킬 개방시간 추가 적립
					aClanResultUser[ ui8BattleUserCount ]._ui8TeamIdx	= (UINT8)i32TeamIdx;
					aClanResultUser[ ui8BattleUserCount ]._ClanIdx		= m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Clanidx;
					aClanResultUser[ ui8BattleUserCount ]._i64UID		= m_Info._pSession[i]->m_i64UID;
					aClanResultUser[ ui8BattleUserCount ]._Exp			= m_Info._aui16AccExp[i] + m_Info._aui16EventExp[i];
					aClanResultUser[ ui8BattleUserCount ]._Kill			= m_Info._aM_UserScore[i].m_KillCount;
					aClanResultUser[ ui8BattleUserCount ]._Death		= m_Info._aM_UserScore[i].m_DeathCount;
					aClanResultUser[ui8BattleUserCount]._Assist			= m_Info._aM_UserScore[i].m_AssistCount;
					aClanResultUser[ ui8BattleUserCount ]._Head			= m_Info._aui16M_Headshot[i];
					aClanResultUser[ ui8BattleUserCount ].m_TMerID		=  m_Info._pSession[i]->GetMercenaryId();
					

					ui8BattleUserCount++;
				}
				break; 
			}
		}
	}

	// ClanBattle 를 위한 루틴 // 20080422
	switch( m_ui8ChannelType )
	{
	default: 
		break;
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN:
		{
			// 챌린지 모드와 튜토리얼의 경우 전적을 반영하지 않는다.
			// 동일 클랜끼리 매치 경우도 클랜의 전적은 남기지 않고 클랜 맴버의 전적만 반영한다.
			if ( IsChallengeMode() || IsTutorialMode() || IsConvoyMode() || IsHeadHunterMode() || IsChaosMode() || IsSpaceMode())
				break;

			// 팀의 클랜 Idx 셋팅.
			for( INT32 i = 0 ; i < TEAM_COUNT ; i++ )
			{
				if( 0 < aClanResult[ i ]._ClanIdx ) continue;

				if( 0 < m_Info._aui32BattleClanIdx[ i ] )
				{	// 팀원 모두 이탈한 경우. 패.
					aClanResult[ i ]._ClanIdx = m_Info._aui32BattleClanIdx[ i ];
					aClanResult[ i ]._Win		= 0;
					aClanResult[ i ]._Exp		= 0;
					aClanResult[ i ]._Lose		= 1;
					continue;
				}

				// 남은 유저중 슬롯이 가장빠른 유저의 클랜 Idx로 설정합니다.
				for(INT32 k = 0; k < SLOT_MAX_COUNT; k++)
				{
					if( m_Info._aSlotInfo[k]._State != SLOT_STATE_BATTLE ) continue;

					if( i != (k%2) ) continue;

					if( 0 == m_Info._pSession[k]->GetUserInfoBasic()->m_ui32Clanidx ) continue;

					aClanResult[ i ]._ClanIdx = m_Info._pSession[k]->GetUserInfoBasic()->m_ui32Clanidx; 
					break;
				}

			}

			INT32 i32Matchmaking = IsMatchMaking();
			g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_CLAN_MATCHING, NULL, this, aClanResult, &i32Matchmaking );


			CLAN_DETAIL_RESULT					ClanDetailResult;
			ClanDetailResult.m_ui8StageMode		= (UINT8)m_Info._i32StageType;
			ClanDetailResult.m_ui32StageID		= m_Info._InfoBasic._StageID;
			ClanDetailResult.m_TMatchingID		= m_TMatchingID;
			ClanDetailResult.m_ui8CurPlayer		= ui8BattleUserCount; 
			ClanDetailResult.m_ui8MaxPlayer		= m_Info._InfoBasic._PersonPlayerMax; 
			ClanDetailResult.m_ui8IsMatchMaking = IsMatchMaking() ? 1 : 0;

			g_pModuleClan->ClanSendBattleResult( MAIN_THREAD_IDX, TEAM_COUNT, aClanResult, &ClanDetailResult, ui8BattleUserCount, aClanResultUser );

		}
		break; 
	}

	return ui16BattleEndUser; 
}

REAL32 CRoom::_GetUsePositionWeight( UINT32 ui32ItemID, UINT8 ui8UsedCount )
{
	// 무기별 가중치
	REAL32 r32Weight = 1.f;
	INT32 i32Class = GET_ITEM_TYPE( ui32ItemID );
	switch( i32Class )
	{
	case ITEM_TYPE_PRIMARY:	
		r32Weight = 0.7f;
		break;
	case ITEM_TYPE_SECONDARY:
		r32Weight = 0.2f;
		break;
	case ITEM_TYPE_MELEE:
		r32Weight = 0.1f;
		break;
	default:	
		r32Weight = 1.f;
		break;
	}

	switch( ui8UsedCount )
	{
	case 0:
	case 1:
		r32Weight *= 1.f;
		break;
	case 2:
		r32Weight *= 1.2f;
		break;
	case 3:
		r32Weight *= 1.4f;
		break;
	default:
		r32Weight *= 1.5f;
		break;
	}
	return r32Weight;
}

void CRoom::_SendUsedWeapon( INT32 i32ThreadIdx, CUserSession* pSession )
{
	USED_ITEM aUsedItemList[ MAX_INVEN_COUNT ];
	pSession->CheckEndurance();
	if( 0 < pSession->m_ui16UsedItemCount )
	{
		// 챌린지모드일경우 내구도 10%만 소모
		UINT8 ui8AIE = 1;
		if( IsChallengeMode() )
			ui8AIE = 10;
		
		
		for( INT32 i = 0 ; i < pSession->m_ui16UsedItemCount ; i++ )
		{
			aUsedItemList[i].m_TUsedItemDBIdx		= pSession->m_aUsedItemList[i].m_TUsedItemDBIdx;

			switch( pSession->m_aUsedItemList[i].m_ui8Type )
			{
			case USED_ITEM_TYPE_ONE:
				{
					aUsedItemList[i].m_ui16UsedDurablity	= 1;
				}
				break;
			case USED_ITEM_TYPE_ENDURANCE:
				{
					// 사용 개수별 가중치
					UINT8 ui8UsedCount = 1;

					UINT8 ui8CheckSlotInfo = pSession->m_aUsedItemList[i].m_ui8CheckSlotInfo;
					switch( GET_USED_CHECK_TYPE(ui8CheckSlotInfo) )
					{
					case USED_CHECK_WEAPON:
						if( GET_USED_CHECK_SLOT(ui8CheckSlotInfo) >= CHAR_EQUIPMENT_WEAPON_COUNT ) break;
						ui8UsedCount = pSession->m_aui8WeaponUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) ];
						break;
					case USED_CHECK_CHARA:
						ui8UsedCount = pSession->m_aui8CharaUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) ];
						break;
					}

					aUsedItemList[i].m_ui16UsedDurablity	= (UINT16)(((REAL32)pSession->m_aUsedItemList[i].m_ui32UsedDurablity * _GetUsePositionWeight( pSession->m_aUsedItemList[i].m_ui32ItemID, ui8UsedCount )) / ui8AIE);
				}
				break;
			}
		}
	}

	g_pModuleTrans->AuthSendUsedWeapon( i32ThreadIdx, pSession, pSession->m_ui16UsedItemCount, aUsedItemList );
}

REAL32 CRoom::_GetBattleCountEndPercentTeam(INT32 i32Team)
{
	INT32	i32AllCount	= 0;
	INT32	i32Count	= 0; 	
	INT32	i32TeamIdx	= 0;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		i32TeamIdx = i % 2;
		if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State ) continue;
		if( TRUE == m_Info._aSlotInfo[i]._bIsGmObserverMode ) continue;

		i32AllCount++;

		if( i32Team == i32TeamIdx) i32Count++;  
	}

	return m_pRoomMode->GetSubPercentOfUserCountTeam( i32Team, i32AllCount, i32Count );
}

REAL32 CRoom::_GetBattleModeInvariable()
{
	REAL32 r32Invariable = 1.0f;
	UINT8 ui8WeaponFlag = m_Info._InfoBasic._WeaponFlag;

	// 주무기 제한되는 모드
	if( ui8WeaponFlag & LOCK_SHOTGUN || ui8WeaponFlag & LOCK_SNIPER )
		r32Invariable = 0.8f;
	// 보조무기로 제한되는 모드
	else if( ((ui8WeaponFlag & LOCK_PRIMARY) != LOCK_PRIMARY) && ((ui8WeaponFlag & LOCK_SECONDARY) == LOCK_SECONDARY) )
		r32Invariable = 0.4f;
	// 근접무기로 제한되는 모드
	else if( ui8WeaponFlag & LOCK_KNUCKLE )
		r32Invariable = 0.2f;
	else if( (ui8WeaponFlag & LOCK_MELEE) && ((ui8WeaponFlag & LOCK_PRIMARY) != LOCK_PRIMARY) && ((ui8WeaponFlag & LOCK_SECONDARY) != LOCK_SECONDARY) )
		r32Invariable = 0.2f;

	// 챌린지 모드
	if( IsChallengeMode() )
		r32Invariable = 0.1f;

	return r32Invariable;
}

void CRoom::_GetBattleUserCountInfo(INT32 i32InTeam, INT32 & i32OutAllUser, INT32 & i32OutTeamUser)
{
	INT32	i32AllCount		= 0;
	INT32	i32TeamCount	= 0;
	INT32	i32TeamIdx		= 0;

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		i32TeamIdx = i % 2;
		if (SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State)	 continue;
		if (m_Info._aSlotInfo[i]._bIsGmObserverMode)			 continue;

		i32AllCount++;

		if (i32InTeam == i32TeamIdx) i32TeamCount++;
	}

	i32OutAllUser	= i32AllCount;
	i32OutTeamUser	= i32TeamCount;
}

void CRoom::_GetBattleUserCountAllTeam(INT32 & i32OutAllUser, INT32 & i32OutRedTeamUser, INT32 & i32OutBlueTeamUser)
{
	INT32	i32AllCount			= 0;
	INT32	i32RedTeamCount		= 0;
	INT32	i32BlueTeamCount	= 0;
	INT32	i32TeamIdx			= 0;

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		i32TeamIdx = i % 2;
		if (SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State)	continue;
		if (m_Info._aSlotInfo[i]._bIsGmObserverMode)			continue;

		i32AllCount++;

		if (TEAM_RED == i32TeamIdx) ++i32RedTeamCount;
		else						++i32BlueTeamCount;
	}

	i32OutAllUser		 = i32AllCount;
	i32OutRedTeamUser	 = i32RedTeamCount;
	i32OutBlueTeamUser	 = i32BlueTeamCount;
}

REAL32 CRoom::_GetBattleCountEndPercent()
{
	INT32  i32Count = 0; 
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		if( SLOT_STATE_BATTLE == m_Info._aSlotInfo[i]._State ) i32Count++;  
	}

	return m_pRoomMode->GetSubPercentOfUserCount( i32Count );
}

REAL32 CRoom::_GetBattleCountEndPercentTeam2(INT32 i32Team)
{
	INT32	i32Count	= 0; 	
	INT32	i32TeamIdx	= 0;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		i32TeamIdx = i % 2;
		if( SLOT_STATE_BATTLE == m_Info._aSlotInfo[i]._State && i32Team == i32TeamIdx) i32Count++;  
	}

	return m_pRoomMode->GetSubPercentOfUserCount2( i32Count );
}

REAL32 CRoom::_GetBattleCountEndPercentAsymmetryPoint(INT32 i32Team)
{
	INT32	i32AllCount		= 0;
	INT32	i32TeamCount	= 0;
	INT32	i32TeamIdx		= 0;

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		i32TeamIdx = i % 2;
		if (SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State)	 continue;
		if (m_Info._aSlotInfo[i]._bIsGmObserverMode)			 continue;

		i32AllCount++;

		if (i32Team == i32TeamIdx) i32TeamCount++;
	}

	return m_pRoomMode->GetPercentOfAsymmetryUserPoint(i32Team, i32AllCount, i32TeamCount);
}

REAL32 CRoom::_GetBattleCountEndPercentAsymmetryExp(INT32 i32Team)
{
	INT32	i32AllCount = 0;
	INT32	i32TeamCount = 0;
	INT32	i32TeamIdx = 0;

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		i32TeamIdx = i % 2;
		if (SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State)	 continue;
		if (m_Info._aSlotInfo[i]._bIsGmObserverMode)			 continue;

		i32AllCount++;

		if (i32Team == i32TeamIdx) i32TeamCount++;
	}

	return m_pRoomMode->GetPercentOfAsymmetryUserExp(i32Team, i32AllCount, i32TeamCount);
}

void CRoom::_ResultUpdateUser_M( INT32 i32WorkIdx, INT32 i32SlotIdx )
{
	CUserSession* pSession = m_Info._pSession[i32SlotIdx];

	INT32 i32TeamIdx = i32SlotIdx % TEAM_COUNT;

	// aCharEquips
	T_ItemID				TItemID;
	ITEM_INFO				ItemInfo; 
	S2MO_CHAR_EQUIP_INFO	CharEquipInfo;
	pSession->m_pDesign->GetMainCharEquipInfo(&pSession->m_MultiSlot, &CharEquipInfo, i32TeamIdx); 	// m_MultiSlot.GetMainCharEquipInfo( &CharEquipInfo );

	// 리셋 
	m_Info._aui8ResultIcon[i32SlotIdx]	= 0x00;
	m_Info._aui16EventExp[i32SlotIdx]	= 0;
	m_Info._aui16EventPoint[i32SlotIdx]	= 0;

	// 튜토리얼은 보너스 경험치/포인트 지급하지 않음
	if( IsTutorialMode() )
	{
		pSession->m_UserInfoBasic.m_ui32Exp		+= m_Info._aui16AccExp[i32SlotIdx];
		pSession->m_UserInfoBasic.m_ui32Point	+= m_Info._aui16AccPoint[i32SlotIdx];

		pSession->m_UserInfoDaily._exp		+= m_Info._aui16AccExp[i32SlotIdx];
		pSession->m_UserInfoDaily._point	+= m_Info._aui16AccPoint[i32SlotIdx];

		INT32 i32GetRoute = GET_POINT_ROUTE_GAMEPLAY;
		g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_GET_POINT, pSession, &m_Info._aui16AccPoint[i32SlotIdx], &i32GetRoute );

		// 랭크업 체크
		pSession->CheckRankUp( i32WorkIdx );

		// 이부분이 있어야 DB에 저장 됩니다. 
		pSession->ChangeDataBaseInfo(SAVE_DATABASE_RECORD | SAVE_DATABASE_BASEINFO);

		return; 
	}

	//-----------------------------------------------------------------------------------------------------------
	//이벤트 시작 
	REAL32 r32AddExpItem	= 0.0001f;
	REAL32 r32AddPotItem	= 0.0001f;
	REAL32 r32AddExpPCCafe	= 0.0001f;
	REAL32 r32AddPotPCCafe	= 0.0001f;
	REAL32 r32AddExpEvent	= 0.0001f;
	REAL32 r32AddPotEvent	= 0.0001f;
	REAL32 r32AddSkillItem	= 0.0001f;
	
	// 국가 추가 부스트
	if( g_pContextMain->GetRoomBonus( pSession->m_ui8LocaleCode, EVENT_BONUS_NATION, (UINT32)pSession->m_ui8Country, &r32AddExpEvent, &r32AddPotEvent ) )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
	}

	//PC방 추가 경험치, 포인트
	if( g_pContextMain->GetRoomBonus( pSession->m_ui8LocaleCode, EVENT_BONUS_PC_CAFE, (UINT32)pSession->m_UserInfoBasic.m_ui8PCCafe, &r32AddExpPCCafe, &r32AddPotPCCafe ) )
	{
		switch( (INT32)pSession->m_UserInfoBasic.m_ui8PCCafe )
		{
		case PC_NORMAL_CAFE:			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_PC;			break;
		case PC_PREMIUM_CAFE:			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_PC_PLUS;		break;
		case PC_VACATION_EVENT_ITEM:	m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_PC_HOUSE;		break;
		case PC_GARENA_GCA:				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_GARENA_GCA;	break;
		case PC_GARENA_GCA_PLUS:		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_GARENA_GCA;	break;
		case PC_INDO_SILVER:
		case PC_INDO_GOLD:
			{
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_GWARNET;
				// 클랜전일 경우 추가 10% 더 -_-;;
				if( IsMatchMaking() )
					r32AddExpPCCafe	+= 0.10001f;
			}
			break;
		}
	}

	// Channel 추가 경험치, 포인트
	if( g_pContextMain->GetRoomBonus( pSession->m_ui8LocaleCode, EVENT_BONUS_CHANNEL, (UINT32)m_ui8ChannelType, &r32AddExpEvent, &r32AddPotEvent ) )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
	}
	// 게릴라 이벤트 보너스
	if( g_pContextMain->GetRoomBonus( pSession->m_ui8LocaleCode, EVENT_BONUS_NORMAL, 0, &r32AddExpEvent, &r32AddPotEvent ) )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
	}

	// Rule 추가 경험치
	if( g_pContextMain->GetRoomBonus( pSession->m_ui8LocaleCode, EVENT_BONUS_RULE, (INT32)g_pContextMain->GetRuleIdx(m_Info._InfoBasic._StageID), &r32AddExpEvent, &r32AddPotEvent ) )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
	}
	
	// STAGE 추가 경험치
	if( g_pContextMain->GetRoomBonus( pSession->m_ui8LocaleCode, EVENT_BONUS_STAGE, (UINT32)StageID::GetStageUID(m_Info._InfoBasic._StageID), &r32AddExpEvent, &r32AddPotEvent ) )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
	}

	// 특정 Rule-STAGE 추가 경험치
	if (g_pContextMain->GetRoomBonus(pSession->m_ui8LocaleCode, EVENT_BONUS_MATCHING, m_Info._InfoBasic._StageID, &r32AddExpEvent, &r32AddPotEvent))
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
	}

	// Rank별 추가 경험치
	if( g_pContextMain->GetRoomBonus( pSession->m_ui8LocaleCode, EVENT_BONUS_RANK, (UINT32)pSession->m_UserInfoBasic.m_ui32Rank, &r32AddExpEvent, &r32AddPotEvent ) )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
	}

	// 랜덤맵 보너스 경험치 포인트 주기.
	if (0 < g_pContextMain->m_ui16RandomMapCount)
	{
		if (GET_ROOM_INFO_RANDOM_MODE(m_Info._InfoBasic._InfoFlag)
			&& g_pContextMain->GetRoomBonus(pSession->m_ui8LocaleCode, EVENT_BONUS_RANDOM_MAP, 0, &r32AddExpEvent, &r32AddPotEvent))
		{
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
		}
	}
	
	//돈주고 산 캐릭터 아이템 - 대회(세일) 빌드에는 적용하지 않는다.
	if ( m_Info._pSession[i32SlotIdx]->m_pDesign->GetCharaBonus(i32TeamIdx, &m_Info._pSession[i32SlotIdx]->m_MultiSlot, r32AddExpItem, r32AddPotItem))
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
	}

	if (GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(m_Info._InfoBasic._InfoFlag)) // 공수 교대 모드가 On일경우 양쪽 케릭터 추가 보상을 모두 받습니다.
	{
		if (m_Info._pSession[i32SlotIdx]->m_pDesign->GetCharaBonus( (i32TeamIdx + 1) % TEAM_COUNT , &m_Info._pSession[i32SlotIdx]->m_MultiSlot, r32AddExpItem, r32AddPotItem))
		{
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
	}

	// 주무기류
	CharEquipInfo.GetCharEquipmentInfo( CHAR_EQUIPMENT_PRIMARY, &ItemInfo );
	TItemID = ItemInfo.m_TItemID;
	switch( TItemID )
	{
	case 103024:			// M4A1 G+ 30%			//MAKE_ITEM_ID( ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_M4A1_G_EXP_PT_30):
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if( pItem == NULL ) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.30001f;
			r32AddPotItem	+= 0.30001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	case 103026:			// SG550 Point 10%		//MAKE_ITEM_ID( ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_SG550_MB_EXP_PT_10):
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if (pItem == NULL) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.10001f;
			r32AddPotItem	+= 0.10001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;	
	case 103025:			// AK 47 Silver 20%		//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_AK47_S_POINT15):
	case 103030:			// SG550 MB 20+			//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_SG550_MB_EXP_PT_20):
	case 103034:			// G36C Sl +			//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_G36C_SILVER_EXP_PT_20):
	case 103033:			// AK Sopmode + 20		//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_AK_SOPMOD_EXP_PT_20):
	case 106009:			// SPAS15 + 20			//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SHOTGUN, WEAPON_NUM_SHOTGUN_SPAS15_SILVER_EXP_PT_20):
	case 105013:			// L11A1 S +20			//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, WEAPON_NUM_SNIPER_L115A1_S_EXP_PT_20 ):
	// case 200004027:			// P90MC Dot Camo.		//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, WEAPON_NUM_SMG_P90MC_DOTSIGHT_CAMO):
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if (pItem == NULL) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.20001f;
			r32AddPotItem	+= 0.20001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	case 103037:			// AUG A3 G.			//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_AUG_A3_GOLD):
		{
			// AUG A3 G. 의 보너스에서 인도네시아는 제외 합니다. - 서동권
			//if( m_Info._pSession[iSlotIdx]->m_ui8LocaleCode == LOCALE_CODE_INDONESIA )	break;
			switch(pSession->m_ui8LocaleCode)
			{
			case LOCALE_CODE_INDONESIA:
			case LOCALE_CODE_TURKEY:
				break;
			default:
				{
					INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
					if (pItem == NULL) break;
					if( 0 == pItem->_ui32ItemArg ) break;

					r32AddExpItem	+= 0.05001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
				}
				break;
			}			
		}
		break;
	case 105015:			// L115A1 G.			//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, WEAPON_NUM_SNIPER_L115A1_GOLD):
	case 104026:			// Kriss S.V. G.		//MAKE_ITEM_ID(ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, WEAPON_NUM_SMG_KRISSSUPERV_GOLD):
	// case 100003041:			// AK Sopmod KingCobra
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if (pItem == NULL) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.05001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	case 103121:	// M4A1 PBIC2013
	case 103122: // AK-47 PBIC2013
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if (pItem == NULL) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.15001f;
			r32AddPotItem	+= 0.15001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	case 104132:		// Kriss S.V. Mid-Night
	case 104149:		// Kriss S.V. Mid-Night No Logo
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if (pItem == NULL) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.10001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	case 103151:		// TAR-21 BR Camo(+15% Exp)
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if (pItem == NULL) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.15001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	}

	// 태국에서만 적용되는 특정주무기 착용시 추가경험치, 추가포인트 적용 코드	2011.06.03 안국정
	switch( pSession->m_ui8LocaleCode )
	{
	case LOCALE_CODE_THAILAND		:
		switch( TItemID )
		{
		case 103061:			// Pindad SS2-V4 PS G.		5% 경험치 추가 획득
		case 103064:			// FAMAS G2 Commando G.
		case 104049:			// Pindad SS1-R5 Carbine G. 
		case 118013:			// Dual Mini Uzi G.	
			{
				INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
				if (pItem == NULL) break;
				if ( 0 == pItem->_ui32ItemArg ) break;

				r32AddExpItem	+= 0.05001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
		case 103062:			// FAMAS G2 Commando ES.	5% 경험치와 5% 포인트 추가 획득
		case 103063:			// AUG-A3 ES.
		case 104050:			// Kriss SV ES.
		case 105033:			// L115A1 ES.
		case 103065:			// AUG BLUE
			{
				INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
				if (pItem == NULL) break;
				if ( 0 == pItem->_ui32ItemArg ) break;

				r32AddExpItem	+= 0.05001f;
				r32AddPotItem	+= 0.05001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
		}
	}

	// 보조무기류
	CharEquipInfo.GetCharEquipmentInfo( CHAR_EQUIPMENT_SECONDARY, &ItemInfo );
	TItemID = ItemInfo.m_TItemID;
	switch( TItemID )
	{
	case 202010:			// Colt Python Gold + Exp & Point 10%																														
		{
			INVEN_BUFFER* pItem = pSession->GetItemByItemID( TItemID );
			if (pItem == NULL) break;
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.10001f;
			r32AddPotItem	+= 0.10001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	}

	// 스킨 파츠 보너스
	if (pSession->m_pDesign->GetSkinBonus(&pSession->m_MultiSlot, r32AddExpItem, r32AddPotItem))
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
	}

	//	근접무기
	//	투척무기	
	//	투척 아이템
	//	미션 아이템

	

	// 아이템에 대한 보너스 유무 체크
	pSession->GetItemBonusInRoom( i3ThreadTimer::LocalTime().GetHour(), r32AddExpItem, r32AddPotItem, r32AddSkillItem, m_Info._aui8ResultIcon[i32SlotIdx] );	

	//모드별 정의된 획득 상한선을 넘을 경우 Exp / 포인트 획득량을 제한합니다.
	UINT16 ui16AccLimit = 2000;
	BOOL	bIsNormalMode;

	switch( StageID::GetAIMode( m_Info._InfoBasic._StageID ) )
	{
	case AI_USE_CHALLENGE:
	case AI_USE_MADNESS:
	case AI_USE_DIEHARD:
		ui16AccLimit = 300;
		bIsNormalMode = FALSE;
		break;
	default:
		ui16AccLimit = 2000;
		bIsNormalMode = TRUE;
		break;
	}

	if( m_Info._aui16AccExp[i32SlotIdx] > ui16AccLimit )		m_Info._aui16AccExp[i32SlotIdx]		= ui16AccLimit;
	if( m_Info._aui16AccPoint[i32SlotIdx] > ui16AccLimit )		m_Info._aui16AccPoint[i32SlotIdx]	= ui16AccLimit;

	REAL32 rAddExp		= 0.0001f;
	REAL32 rAddPoint	= 0.0001f;

	if( 0.001f < r32AddExpItem )	{	m_aSlotBonus[ i32SlotIdx ].ui16Exp[ TYPE_BATTLE_RESULT_EVENT_ITEM ]		= (UINT16)(m_Info._aui16AccExp[i32SlotIdx]		* r32AddExpItem);	rAddExp		+= r32AddExpItem;	}
	if( 0.001f < r32AddPotItem )	{	m_aSlotBonus[ i32SlotIdx ].ui16Point[ TYPE_BATTLE_RESULT_EVENT_ITEM ]	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx]	* r32AddPotItem);	rAddPoint	+= r32AddPotItem;	}

	if( 0.001f < r32AddExpPCCafe )	{	m_aSlotBonus[ i32SlotIdx ].ui16Exp[ TYPE_BATTLE_RESULT_EVENT_PCCAFE ]	= (UINT16)(m_Info._aui16AccExp[i32SlotIdx]		* r32AddExpPCCafe);	rAddExp		+= r32AddExpPCCafe;	}
	if( 0.001f < r32AddPotPCCafe )	{	m_aSlotBonus[ i32SlotIdx ].ui16Point[ TYPE_BATTLE_RESULT_EVENT_PCCAFE ]	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx]	* r32AddPotPCCafe);	rAddPoint	+= r32AddPotPCCafe;	}

	if( 0.001f < r32AddExpEvent )	{	m_aSlotBonus[ i32SlotIdx ].ui16Exp[ TYPE_BATTLE_RESULT_EVENT_EVENT ]	= (UINT16)(m_Info._aui16AccExp[i32SlotIdx]		* r32AddExpEvent);	rAddExp		+= r32AddExpEvent;	}
	if( 0.001f < r32AddPotEvent )	{	m_aSlotBonus[ i32SlotIdx ].ui16Point[ TYPE_BATTLE_RESULT_EVENT_EVENT ]	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx]	* r32AddPotEvent);	rAddPoint	+= r32AddPotEvent;	}

	m_Info._aui16EventExp[i32SlotIdx]	= (UINT16)(m_Info._aui16AccExp[i32SlotIdx] * rAddExp);
	m_Info._aui16EventPoint[i32SlotIdx]	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx]	* rAddPoint);

	if( bIsNormalMode == FALSE )
	{
		// AI전(=챌린지모드,=트레이닝모드) 경험치, 포인트 상한선 300으로 고정
		if( m_Info._aui16AccExp[i32SlotIdx] == ui16AccLimit )											m_Info._aui16EventExp[i32SlotIdx]	= 0;
		if( m_Info._aui16AccExp[i32SlotIdx] + m_Info._aui16EventExp[i32SlotIdx] > ui16AccLimit )		m_Info._aui16EventExp[i32SlotIdx]	= ui16AccLimit - m_Info._aui16AccExp[i32SlotIdx];
		
		if( m_Info._aui16AccPoint[i32SlotIdx] == ui16AccLimit )											m_Info._aui16EventPoint[i32SlotIdx]	= 0;
		if( m_Info._aui16AccPoint[i32SlotIdx] + m_Info._aui16EventPoint[i32SlotIdx] > ui16AccLimit )	m_Info._aui16EventPoint[i32SlotIdx]	= ui16AccLimit - m_Info._aui16AccPoint[i32SlotIdx];
	}
	
	// 어뷰저가 아닐때만 exp와 point를 적용합니다.
	// 어뷰징 레벨별 페널티 적용
	switch( pSession->m_ui8AbusingLevel )
	{
	case ABUSING_LEVEL_NORMAL :	 
	case ABUSING_LEVEL_STRONG :	 
		{
			// 일반 제재 유저는 팝업 시간이 최소 팝업 시간 미만이면 최소 팝업 시간으로 고정
			if ( ABUSING_LEVEL_NORMAL == pSession->m_ui8AbusingLevel && pSession->m_ui32AbusingPopupTime < NORMAL_ABUSING_MIN_POPUP_TIME ) 
			{
				pSession->m_ui32AbusingPopupTime = NORMAL_ABUSING_MIN_POPUP_TIME;
			}

			// 획득한 EXP, POINT를 보류한다.
			pSession->m_ui32HoldAccExp			= m_Info._aui16AccExp[i32SlotIdx] + m_Info._aui16EventExp[i32SlotIdx];
			pSession->m_ui32HoldAccPoint		= m_Info._aui16AccPoint[i32SlotIdx] + m_Info._aui16EventPoint[i32SlotIdx];
			m_Info._aui16AccExp[i32SlotIdx]		= 0;
			m_Info._aui16AccPoint[i32SlotIdx]	= 0;
			m_Info._aui16EventExp[i32SlotIdx]	= 0;
			m_Info._aui16EventPoint[i32SlotIdx]	= 0;
		}
		break;
	case ABUSING_LEVEL_STRONGEST :	// EXP,POINT 지급 X
		{
			m_Info._aui16AccExp[i32SlotIdx]		= 0;
			m_Info._aui16AccPoint[i32SlotIdx]	= 0;
			m_Info._aui16EventExp[i32SlotIdx]	= 0;
			m_Info._aui16EventPoint[i32SlotIdx]	= 0;
		}
		break;
	}

	UINT32 ui32Exp		= m_Info._aui16AccExp[i32SlotIdx] + m_Info._aui16EventExp[i32SlotIdx];
	UINT32 ui32Point	= m_Info._aui16AccPoint[i32SlotIdx]	+ m_Info._aui16EventPoint[i32SlotIdx];

	//스킬부스터 사용여부에 따른 스킬 개방시간 추가 적립
	S2MO_CHAR_BASE_INFO stBaseInfo;
	m_Info._pSession[i32SlotIdx]->m_pDesign->GetMainCharBaseInfo(&m_Info._pSession[i32SlotIdx]->m_MultiSlot, &stBaseInfo, i32SlotIdx % TEAM_COUNT); 
	stBaseInfo.m_ui32SkillRelTime					+= (UINT32)( m_Info._pSession[i32SlotIdx]->m_ui32PlayTime * r32AddSkillItem );		//부스터 추가분 만큼 메인스킬 개방시간을 추가한다.
	m_Info._pSession[i32SlotIdx]->m_pDesign->SetMainCharBaseInfo(&m_Info._pSession[i32SlotIdx]->m_MultiSlot, &stBaseInfo, i32SlotIdx % TEAM_COUNT);	//m_MultiSlot.SetMainCharBaseInfo(&stBaseInfo);

	pSession->m_UserInfoBasic.m_ui32Exp				+= ui32Exp;
	pSession->m_UserInfoBasic.m_ui32Point			+= ui32Point;

	pSession->m_UserInfoDaily._exp					+= ui32Exp;
	pSession->m_UserInfoDaily._point				+= ui32Point;

	pSession->m_UserInfoLastGame.m_ui32Exp			= ui32Exp;
	pSession->m_UserInfoLastGame.m_ui32Point		= ui32Point;

	pSession->m_UserInfoLastGame.m_ui32StageID		= m_Info._InfoBasic._StageID;	
	pSession->m_UserInfoLastGame.m_ui16Killcount	= m_Info._aM_UserScore[i32SlotIdx].m_KillCount;
	pSession->m_UserInfoLastGame.m_ui16Death		= m_Info._aM_UserScore[i32SlotIdx].m_DeathCount;
	pSession->m_UserInfoLastGame.m_ui16Assist		= m_Info._aM_UserScore[i32SlotIdx].m_AssistCount;

	if( 0 < ui32Point )
	{	// 배틀에서 얻은 포인트 ZLog 기록
		INT32 i32GetRoute = GET_POINT_ROUTE_GAMEPLAY;
		g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_GET_POINT, pSession, &ui32Point, &i32GetRoute );
	}

	// 어뷰징 체크 로그저장 - 어뷰징 체크된 유저에 한해서.  
	if( ABUSING_LEVEL_NONE < pSession->m_ui8AbusingLevel )
	{ 
		g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_R_ABUSER_CHECK, pSession, this, &i32SlotIdx );
	}

	// 랭크업 체크.
	pSession->CheckRankUp( i32WorkIdx );

	// 이부분이 있어야 DB에 저장 됩니다. 
	pSession->ChangeDataBaseInfo(SAVE_DATABASE_RECORD | SAVE_DATABASE_BASEINFO);

	// 용병 페넡티 플래그 해제
	pSession->SetMerPenaltyFlag(false);

	return; 
}

void CRoom::_SendMainChange(void)
{
	//Make Network Info 
	i3NetworkPacket Packet( PROTOCOL_BATTLE_LEAVEP2PSERVER_ACK );
	Packet.WriteData( &m_Info._i32MainSlotIdx,		sizeof(INT32));
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if(m_Info._aSlotInfo[i]._State > SLOT_STATE_CLOSE)
		{
			if( m_Info._aSlotInfo[i]._State > SLOT_STATE_READY)
			{//게임중인사람
				m_Info._pSession[i]->SendPacketMessage(&Packet); 
			}
			else
			{//게임중이 아닌사람
				_SendSlotInfo(m_Info._pSession[i]); 
			}
		}
	}

	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 방장이 나갔을때 새로운 방장 선출 리턴 값이 SERVER_VALUE_UNKNOWN 이면 실패입니다. 
여기에서 새로운 방장이 나오면 방의 모든 인원들에게 새로운 정보를 알려줘야 합니다.
-----------------------------------------------------------*/
INT32 CRoom::_ChangeMain( SLOT_STATE eSlotState )
{
	INT32 i32Rv = _CheckMainBest( eSlotState ); 

	if( SERVER_VALUE_UNKNOWN == i32Rv ) return SERVER_VALUE_UNKNOWN;

	m_Info._i32MainSlotIdx	= i32Rv;
	m_i64LogMainUserDbIdx	= m_Info._pSession[ i32Rv ]->m_i64UID;

	_SetMainSlot();

	return i32Rv;
}

void	CRoom::_SetMainSlot()
{
	//Set Main Name
	if( m_Info._aSlotInfo[ m_Info._i32MainSlotIdx ]._State == SLOT_STATE_READY )	m_Info._aSlotInfo[ m_Info._i32MainSlotIdx ]._State = SLOT_STATE_NORMAL; 
	i3mem::Copy( m_Info._InfoAdd._Nick, m_Info._pSession[ m_Info._i32MainSlotIdx ]->GetNickName(), NET_NICK_NAME_SIZE);
#ifdef USE_COLOR_NICK2
	m_Info._InfoAdd._NicklColor = m_Info._pSession[ m_Info._i32MainSlotIdx ]->GetNickColor();
#endif
}

void CRoom::_OnUpdateRoom_Ready_M( UINT32 ui32ATServer )
{
	if( SLOT_STATE_READY != m_Info._aSlotInfo[m_Info._i32MainSlotIdx]._State )	return;
	// 방장이 레디를 했으면, 방 시작 조건 검사를 완료한 것이므로

	switch( m_pRoomMode->GetStartOption() )
	{
	case ROOM_START_DIRECT:
		_SetState_M( ROOM_STATE_LOADING, ui32ATServer );
		break;
	case ROOM_START_COUNTDOWN:
		_SetState_M( ROOM_STATE_COUNTDOWN_R, ui32ATServer );
		break;
	}
}

void CRoom::_OnUpdateRoom_CountDownR_M( UINT32 ui32ATServer )
{
	// 방장이 Ready를 풀었다면, 카운트다운을 중지한다.
	if( SLOT_STATE_READY > m_Info._aSlotInfo[ m_Info._i32MainSlotIdx ]._State )
	{
		_SetState_M( ROOM_STATE_READY, ui32ATServer );
		m_i8StartCountdown	= 0;
		_SendBattleCountdown( -1 );
		return;
	}

	// 카운트다운중에 게임시작조건에 미치지 못하게 변경되면, 카운트다운을 중지한다.
	if( EV_FAILED( CheckUserReady() ) )
	{
		_SetState_M( ROOM_STATE_READY, ui32ATServer );
		m_i8StartCountdown	= 0;
		_SendBattleCountdown( -2 );

		// 방장 슬롯을 Normal 로 변경 시켜 줍니다.
		m_Info._aSlotInfo[m_Info._i32MainSlotIdx]._State = SLOT_STATE_NORMAL;
		return;
	}

	// 카운트다운
	if( m_ui32ATCountDownR != ui32ATServer )
	{				
		if( m_i8StartCountdown < 1 )
		{
			_SetState_M( ROOM_STATE_LOADING, ui32ATServer );
			return;
		}

		m_i8StartCountdown--;
		m_ui32ATCountDownR = ui32ATServer;
	}
}

void CRoom::_OnUpdateRoom_Loading_M( UINT32 ui32ATServer )
{
	bool bIsBattle = true;

	// 방장이 로딩 끝날때까지 기다립니다. // 추후에 데디가 들어가면 다른 식으로 변경 해야합니다. 20110725 김현우
	if( SLOT_STATE_PRESTART == m_Info._aSlotInfo[m_Info._i32MainSlotIdx]._State )
	{
		if( m_bIsBattleRoomCreated )		// false 라면 배틀서버에서 방을 생성하지 못한겁니다. or 응답이 아예 오지 않았거나.
		{	
			_SetState_M( ROOM_STATE_PRE_BATTLE, ui32ATServer );
		}
		else
		{
			bIsBattle = false;
		}
	}

	// LOADING_TIME_OUT(60)초 동안 방장이 로딩이 완료 되지 않으면 방을 폭파 시킵니다.
	if( (m_ui32ATRoomState + LOADING_TIME_OUT) <= ui32ATServer )
	{
		bIsBattle = false;
	}

	if( false == bIsBattle )
	{	//모두 방으로 돌아간다.	// Ready 에서 SlotInfo 를 보낼 방법을 찾아봅시다.
		_SetState_M( ROOM_STATE_READY, ui32ATServer );
		_SendSlotInfo(NULL);
	}
}

void CRoom::_OnUpdateRoom_PreBattle_M( UINT32 ui32ATServer )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//모든 유저들이 정상적으로 시작
	bool IsBattle		= false;
	bool bStartMatch	= false;
	
	if( SLOT_STATE_BATTLE_READY == m_Info._aSlotInfo[m_Info._i32MainSlotIdx]._State ) 
	{	//방장이 시작할 상태가 되었을때
		bStartMatch = CheckMatchStart();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 게임 참가 유저들이 모두 나갈 경우, 룸상태를 다시 대기상태로 변경
	for(INT32 Loop = 0; Loop < SLOT_MAX_COUNT; Loop++)
	{
		if( SLOT_STATE_READY < m_Info._aSlotInfo[Loop]._State )
		{
			IsBattle = true;
			break;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//정상적으로 시작하지 못함.. 못들어온 유저들은 타임아웃으로 처리 한다. 
	if( (m_ui32ATRoomState + PREGAME_TIME_OUT) <= ui32ATServer )
	{
		// 방장이 준비가 되지 않았기 때문에 무조건 방을 폭파합니다.
		if( SLOT_STATE_BATTLE_READY != m_Info._aSlotInfo[m_Info._i32MainSlotIdx]._State ) 
		{
			IsBattle = false;
		}
		else
		{
			switch( m_Info.m_ePreStartOption )
			{
			case ROOM_PRESTART_ONE_WAIT:
				{
					IsBattle = false;
					break;
				}
				break; 
			case ROOM_PRESTART_ALL_WAIT:
				{
					bool bOtherTeamReady = false;

					INT32 i32MainOtherTeam = (m_Info._i32MainSlotIdx+1) % TEAM_COUNT;	// 방장의 다른 팀을 구합니다.
					for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i += 2 )
					{
						if( SLOT_STATE_BATTLE_READY != m_Info._aSlotInfo[i]._State ) continue;
						// 상대방중 한명도 레디 상태가 아니라면 게임을 그냥 끝내줍니다.
						bOtherTeamReady = true;
						break;
					}

					if( bOtherTeamReady )
					{
						//Start Mission 
						INT32 i32WhyKick = EVENT_ERROR_FIRST_HOLE;
						i3NetworkPacket SendKickPacket( PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER ); 
						SendKickPacket.WriteData( &i32WhyKick, sizeof(INT32) );
						for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
						{
							// 아직 홀펀칭이 완료 되지 않은 유저는 방으로 보냄
							if( SLOT_STATE_NORMAL < m_Info._aSlotInfo[i]._State && SLOT_STATE_BATTLE_READY > m_Info._aSlotInfo[i]._State )
							{
								m_Info._aSlotInfo[i]._State = SLOT_STATE_NORMAL; 
								m_Info._pSession[i]->SendPacketMessage(&SendKickPacket);					//킥 메시지를 보낸다. 

								if (LOCALE_CODE_RUSSIA == m_Info._pSession[i]->m_ui8LocaleCode)
								{
									g_pLog->WriteLogSession(m_Info._pSession[i], L"EVENT_ERROR_FIRST_HOLE11");
								}
							}
						}
						bStartMatch = true;
					}
					else
					{	//End Mission
						IsBattle = false;
					}
				}
				break; 
			}
		}
	}

	if( bStartMatch )
	{	// 모두 완료 되었다면 시작합니다.

		switch( m_Info._i32StageType )
		{
		default: break;
			// 폭파, 설멸전은 COUNT_DOWN 시작.
		case STAGE_MODE_ANNIHILATION	:
		case STAGE_MODE_BOMB			:
			_SetState_M( ROOM_STATE_COUNTDOWN_B, ui32ATServer );
			return;
			break;
		}

		_SetState_M( ROOM_STATE_BATTLE, ui32ATServer );
		_SendSlotInfo( NULL );

		//배틀 시작 패킷을 보낸다.
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State ) continue;

			_SendStartBattle( m_Info._pSession[i], EVENT_ERROR_SUCCESS );
			g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_R_BATTLE_START, m_Info._pSession[i], this );
		}

		// 라운드전일때 배틀 시작 메시지를 보낸다. 
		m_Info._i32MiStateType = MISSION_STATE_UNKNOWN;
		_SetMissionState_M( MISSION_STATE_PRE_BATTLE );
	}

	if( false == IsBattle )
	{
		_SetState_M( ROOM_STATE_READY, ui32ATServer );
		_SendSlotInfo(NULL);
	}
}

void CRoom::_OnUpdateRoom_CountDownB_M( UINT32 ui32ATServer )
{
	if( m_ui32ATCountDownB > ui32ATServer )  return;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 게임 참가 유저들이 모두 나갈 경우, 룸상태를 다시 대기상태로 변경
	bool IsBattle		= false;
	for(INT32 Loop = 0; Loop < SLOT_MAX_COUNT; Loop++)
	{
		if( SLOT_STATE_READY < m_Info._aSlotInfo[Loop]._State )
		{
			IsBattle = true;
			break;
		}
	}
	if( false == IsBattle )
	{
		_SetState_M( ROOM_STATE_READY, ui32ATServer );
		_SendSlotInfo(NULL);
		return;
	}


	//////////////////////////////////////////////////////////////////////////////////////
	// 정상 배틀 시작
	_SetState_M( ROOM_STATE_BATTLE, ui32ATServer );
	_SendSlotInfo( NULL );


	//배틀 시작 패킷을 보낸다.
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State ) continue;

		_SendStartBattle( m_Info._pSession[i], EVENT_ERROR_SUCCESS );
		g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_R_BATTLE_START, m_Info._pSession[i], this );
	}

	// 라운드전일때 배틀 시작 메시지를 보낸다. 
	m_Info._i32MiStateType = MISSION_STATE_UNKNOWN;
	_SetMissionState_M( MISSION_STATE_PRE_BATTLE );

	return;
}


void CRoom::_OnUpdateRoom_Battle_M( UINT32 ui32ATServer )
{
	_UpdateBattleTime();

	if( GetRoomBattleTime() > BATTLE_TIME_OUT )
	{
		// 3시간이 지나면 배틀을 중지 합니다.  //미션전 시간이 변경되면 수정 해야합니다.
		m_bIsBattle = FALSE;
	}

#ifdef I3_DEBUG
	if ( m_ui32TimerToPause != 0 && m_ui32TimerToPause < i3ThreadTimer::GetServerTimeMilli() )
	{
		m_ui32TimerToPause = 0;
		
		m_i32PauseUserIdx = -1;
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			if ( m_Info._aSlotInfo[i]._bIsGmObserverMode && SLOT_STATE_BATTLE == m_Info._aSlotInfo[i]._State )
			{
				m_i32PauseUserIdx = i;
				break;
			}
		}
		if ( -1 != m_i32PauseUserIdx )
		{
			g_pModuleCast->BattleGMPause( MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx, true );
		}
	}
#endif

	// GM Pause중이면 클라이언트가 어떤 TCP도 보내지 않으므로, KILL_REASON_NORMAL_TIMEOUT으로 짤리는 것을 방지하기 위한 코드입니다.
	// 원래는 이 코드없이 클라이언트에서 GM Pause중이라도 PROTOCOL_BATTLE_TIMERSYNC_REQ를 보내도록 하여 처리하려고 하였으나,
	// 클라이언트 소스 구조상 어려운 작업이라고 하여, 하기의 코드로 대체하였습니다. 추후에 클라이언트가 GM Pause중이라도 PROTOCOL_BATTLE_TIMERSYNC_REQ
	// 패킷을 보내도록 수정되면 하단의 if문은 제거되어야 합니다. 서버:안국정, 클라:박기성	2013.09.03
	if ( m_bGMPause )
	{
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			m_Info._pSession[i]->m_ui32ATLastRecv = i3ThreadTimer::GetServerTime();
	}

	//해킹 유저를 판단합니다.
	INT32 i32CheckTime = (INT32)(GetRoomBattleTime() - 60);

	if( MISSION_STATE_BATTLE == m_Info._i32MiStateType )
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i ++ )
		{
			if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State )					continue;
			if( (INT32)m_Info._pSession[i]->m_ui32RTBattleCheck > i32CheckTime )	continue;
#ifndef I3_DEBUG
			m_Info._pSession[i]->_SendErrorMessage( MAIN_THREAD_IDX, EVENT_ERROR_HACKING_EXIT_USER, 0, KILL_REASON_USER_CHECK_TIME );
#endif
		}
	}

	//비정상 종료 
	if( FALSE == m_bIsBattle )
	{	// 알수 없는 이유로 게임이 종료되었습니다.
		m_bIsBattleEnd = FALSE;

		if( 0 == (m_Info._ui16BattleUser & RED_TEAM_MASK) && (m_Info._ui16BattleUser & BLUE_TEAM_MASK) )
		{	// Red 팀만 다 나갔을 경우
			m_i32WinTeam = TEAM_BLUE_WIN;
		}
		else if( 0 == (m_Info._ui16BattleUser & BLUE_TEAM_MASK) && (m_Info._ui16BattleUser & RED_TEAM_MASK) )
		{	// Blue 팀만 다 나갔을 경우
			m_i32WinTeam = TEAM_RED_WIN;
		}
		else
		{	// Draw 일 경우 이긴팀을 찾아줍니다.
			m_i32WinTeam = m_pRoomMode->GetWinTeam();
		}

		if( 1 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
		{
			// 현재 진행중이던 라운드의 데이터들 처리
			m_pRoomMode->SetRoundDataCount();
		}		

		m_ui16BattleEndUserFlag  = _ResultUpdate_M( m_i32WinTeam, FALSE );

		//_SendEndBattle_M( m_i32WinTeam, iBattleEndUserFlag );
		_SetState_M( ROOM_STATE_BATTLE_RESULT, ui32ATServer );
		//_SendSlotInfo( NULL );
		return;
	}

	//체크 정상 종료
	m_bIsBattleEnd = _OnUpdateRoundMany_M();

	//정상적인 배틀 종료 입니다. 
	if( m_bIsBattleEnd )
	{
		m_i32WinTeam = m_pRoomMode->GetWinTeam();
		m_ui16BattleEndUserFlag = _ResultUpdate_M( m_i32WinTeam, TRUE );	// 결과 업데이트 
		//_SendEndBattle_M( m_i32WinTeam, ui16BattleEndUserFlag );		// 엔드 패킷 보내기 				
		_SetState_M( ROOM_STATE_BATTLE_RESULT, ui32ATServer );			// 상태 변경
		//_SendSlotInfo(NULL);
	}

	//Send Ping 
	if( m_ui32ATSendPing + BATTLE_SEND_PING_TIME < i3ThreadTimer::GetServerTime() )
	{
		_SendPing_M(); 
		m_ui32ATSendPing = i3ThreadTimer::GetServerTime();
	}

	//Death 
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i ++ )
	{
		if( m_bDeathFlag[i] == false )
			continue; 

		m_bDeathFlag[i] = false;
		m_bAlive[i]		= false;
		SET_USER_DEATH(i); 
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//아래부터는 순서를 지켜 주세요. 

	if(m_ui16BattleUser != m_Info._ui16BattleUser )
	{
		if(m_pRoomMode->CheckTrex())
		{
			m_ui16BattleUser = m_Info._ui16BattleUser; 
		}
	}

#ifdef USE_RESPAWN_SKIP
	if( MISSION_STATE_BATTLE == m_Info._i32MiStateType )
#endif
	{
		//Respawn 
		CUserSession * pSession; 
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i ++ )
		{
			
			if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State )		continue;
			if( m_bReSpawnFlag[i] == false )							continue;

			m_bReSpawnFlag[i]	= false;
			m_bAlive[i]			= true;
			SET_USER_ALIVE( i );					// 유저 Alive 처리

			if(m_Info._i32MiStateType == MISSION_STATE_BATTLE)m_pRoomMode->UpdateUserState( i );


			//---------------------------------------------------------------------------------------------------

			
			pSession = m_Info._pSession[i];

			if(pSession->m_bFirstObserver == TRUE)
			{
				pSession->m_ui32FirstObserverTime = GetRoomBattleTime() - pSession->m_ui32RTBattleStart;
				pSession->m_bFirstObserver = FALSE;
			}
			// 데디 사용중일땐 데디에 주무기 아이템ID를 전송합니다. 2011.12.15 안국정
			//I3ASSERT( pSession->m_UseWeaponList[WEAPON_USAGE_PRIMARY] != 0 );

			INT32 i32RespawnIdx = m_i32TotalRespawnIdx++;
			
			T_ItemID aWeaponList[CHAR_EQUIPMENT_WEAPON_COUNT];
			T_ItemID aPartsList[CHAR_EQUIPMENT_PARTS_COUNT];
			for( INT32 w = 0 ; w < CHAR_EQUIPMENT_WEAPON_COUNT; ++w )
			{
				aWeaponList[w] = pSession->m_aTBattleEquipWeapon[w].m_TItemID;
			}
			// vv3 - check
			for( INT32 p = 0 ; p < CHAR_EQUIPMENT_PARTS_COUNT; ++p )
			{
				aPartsList[p] = pSession->m_aTBattleEquipParts[p].m_TItemID;
			}

			SendRespawnToDedi( MAIN_THREAD_IDX, i, aWeaponList, i32RespawnIdx, pSession->m_i32RespawnCount );

			i3NetworkPacket SendPacket( PROTOCOL_BATTLE_RESPAWN_ACK ); 
			SendPacket.WriteData( &i,									sizeof(INT32) );								//Slot Idx 
			SendPacket.WriteData( &i32RespawnIdx,						sizeof(INT32) );								//Respawn Idx 
			SendPacket.WriteData( &pSession->m_i32RespawnCount,			sizeof(INT32) );								//Respawn Idx 
			SendPacket.WriteData( aWeaponList,							sizeof(T_ItemID)*CHAR_EQUIPMENT_WEAPON_COUNT );	//Weapon Idx
			SendPacket.WriteData( pSession->m_aUseWeaponEndurance,		sizeof(UINT8)*CHAR_EQUIPMENT_WEAPON_COUNT );	//Weapon Arg
			// vv3 - check
			SendPacket.WriteData( aPartsList,							sizeof(USER_INFO_EQUIPMENT_CHARA) );			// 장비한 캐릭터 정보
			m_pRoomMode->AddRespawnPacket( &SendPacket, pSession );

			_SendPacketBattlePlayer( &SendPacket );
			    
			I3TRACE("RespawnAck (Room:%d, SlotIdx:%d)\n", pSession->m_i32RoomIdx, i );

			// 리스폰 시간 기록
			pSession->m_ui32RTRespawn = GetRoomBattleTime();
		}
	}

	//Dino Info Send 
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i ++ )
	{
		if( m_Info._abDinoStateUpdate[i] )
		{
			if( isUseBattleServer() )
			{
				g_pModuleCast->BattleChangeDinoInfo(MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT16)i, &m_Info ); //._ui32DInoCharInfo[i], m_Info._pSession[i]);
			}

			m_Info._abDinoStateUpdate[i] = false; 
		}
	}

	// 투표관련 추가 2011.05.25, 05.26 안국정
	// 계속 반복적으로 투표시간이 지났는지, 미투표유저가 나갔는지 검사합니다.
	// 투표중인 상태에서 투표가 종료되었을 경우에만 TRUE를 리턴하고
	// 투표중이 아니거나, 투표중이지만 투표가 진행되고 있는 중이면 FALSE를 리턴합니다.

	if ( m_bVoting )
	{
		INT32 k;
		for( k = 0; k < SLOT_MAX_COUNT; k++ )
			if ( 1 == m_aui8VoteRight[k] ) break;

		// 잔여 투표가능자가 0인지 혹은 제한 시간이 다 되었는지 검사
		if ( k == SLOT_MAX_COUNT || GetRoomBattleTime() >= m_ui32RTVoteBegin + VOTE_POSSIBLE_TIME )
		{
			CloseVote();
		}
		// 피신고자가 나갔는지 검사합니다. 나갔다면 투표 종료
		else if	( m_Info._aSlotInfo[m_i32TargetSlot]._State != SLOT_STATE_BATTLE )
		{
			m_bVoting	= FALSE;

			// 피신고인을 제외한 모든 유저에게 피신고자의 Room Leave로 인해 투표가 중단되었음을 통보하느 패킷을 전송합니다.
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_ACK );
				if ( m_Info._aSlotInfo[i]._State == SLOT_STATE_BATTLE )
				{
					m_Info._pSession[i]->SendPacketMessage( &SendPacket );
				}
			}
		}
	}

	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( m_bKickBattle[i] )
		{
			KickUser_Dedi( m_Info._pSession[i] );
			m_bKickBattle[i] = false;
		}
	}
}

void CRoom::_OnUpdateRoom_Battle_End_M( UINT32 ui32ATServer )
{
	// 응답 결과가 너무 지연되면 결과 없이 종료 				
	//m_i32WinTeam = m_pRoomMode->GetWinTeam();
	//_SendEndBattle_M( m_i32WinTeam, m_ui16BattleEndUserFlag );							// 엔드 패킷 보내기 				
	_SetState_M( ROOM_STATE_BATTLE_END, ui32ATServer );											// 상태 변경
	_SendSlotInfo(NULL);
	return; 

}

BOOL CRoom::_OnUpdateRoundMany_M()
{
	BOOL	bIsBattleEnd  = FALSE;
	switch( m_Info._i32MiStateType )
	{
	case MISSION_STATE_PRE_BATTLE	:
		{
			if( m_pRoomMode->OnUpdatePreBattle( m_ui32RTMissionRoundState, GetRoomBattleTime() ) )
			{
				_SetMissionState_M( MISSION_STATE_BATTLE );   
			}
		}
		break; 
	case MISSION_STATE_BATTLE		: 
		{
			UINT32 ui32RTServer = GetRoomBattleTime();
			//배틀중이므로 진행시간을 갱신해줍니다.
			m_Info._InfoAdd._NowIngTime = static_cast<UINT16>(ui32RTServer - m_Info._ui32RTRoundStart);

			//  튜토리얼은 예외적으로 게임 서버에서 처리			
			if( !IsTutorialMode() ) break;
			if( m_pRoomMode->CheckRoundEnd( ui32RTServer ) )
			{	// Round 종료
				if( 1 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
				{
					m_pRoomMode->SetRoundDataCount();
				}
				_SetMissionState_M( MISSION_STATE_FREE );
			}
		}
		break; 
	case MISSION_STATE_FREE		:
		{
			// 게임에 참여중인 유저의 캐릭터 장비에 대해 사용 정지 상태로 전환한다
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				if( m_Info._aSlotInfo[i]._State != SLOT_STATE_BATTLE ) continue;

				m_Info._pSession[i]->CharUseAllStop();
			}

			if( m_pRoomMode->CheckMatchEnd() )
			{	// 모든 라운드가 종료되어 게임을 종료합니다.
				_SetMissionState_M( MISSION_STATE_UNKNOWN );  
				bIsBattleEnd = TRUE; //게임 엔드
			}
			else
			{
				if( (m_ui32RTMissionRoundState + MISSION_FREE_TIME) <= GetRoomBattleTime() )
				{
					_SetMissionState_M( MISSION_STATE_RESULT );
				}
			}
		}
		break; 
	case MISSION_STATE_RESULT	:
		if( (m_ui32RTMissionRoundState + MISSION_RESULT_TIME) <= GetRoomBattleTime() )
		{
			m_Info._ui16RoundStartUser = 0;
			for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
			{				
				if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State ) continue;
				
				m_Info._ui16RoundStartUser	|= (0x01 << i);
			}
			_SetMissionState_M( MISSION_STATE_PRE_BATTLE ); 
		}
		break;
	default: break; 
	}
	return bIsBattleEnd;
}

BOOL CRoom::OnUpdateRoom_M( UINT32 ui32ATServer )
{
	BOOL	Rv = FALSE; 	

	switch( m_Info._InfoBasic._State )
	{
	case ROOM_STATE_READY	: 
		{
			_OnUpdateRoom_Ready_M( ui32ATServer );
		}
		break; 
	case ROOM_STATE_COUNTDOWN_R:
		{
			_OnUpdateRoom_CountDownR_M( ui32ATServer );
		}
		break;
	case ROOM_STATE_LOADING	: 
		{
			_OnUpdateRoom_Loading_M( ui32ATServer );
		}
		break;
	case ROOM_STATE_PRE_BATTLE	:
		{
			_OnUpdateRoom_PreBattle_M( ui32ATServer );
		}
		break; 
	case ROOM_STATE_COUNTDOWN_B:
		{
			_OnUpdateRoom_CountDownB_M( ui32ATServer );
		}
		break; 
	case ROOM_STATE_BATTLE	: 
		{
			_OnUpdateRoom_Battle_M( ui32ATServer );
		}
		break; 
	case ROOM_STATE_BATTLE_RESULT	:
		{
			_OnUpdateRoom_Battle_End_M( ui32ATServer );
		}
		break;
	default: 
		{			
			_SetState_M( ROOM_STATE_READY, ui32ATServer );
			_SendSlotInfo(NULL);
		}
		break; 
	}

	// 게임 모드별 업데이트 주기적으로 실행할 업데이트 처리
	m_pRoomMode->periodicUpdateRoomState( GetRoomRealBattleTime() );

	//////////////////////////////////////////////////////////////////////////////////
	// Check Time Out User; 
	if( m_ui32ATMCTimeOutStart > 0) 
	{			
		if( (m_ui32ATMCTimeOutStart + ROOM_CHECK_MAINCLIENT_TIMEOUT) < ui32ATServer )
		{		
			if( (m_ui16MCTimeOutStartUserFlg & m_ui16MCTimeOutFlg) ==  m_ui16MCTimeOutStartUserFlg)
			{//방장이 타임아웃으로 나감	
				_BattleTimeOutClient_M( m_Info._i32MainSlotIdx );
			}
			else
			{//유저가 타임아웃으로 나감 
				UINT16 Clientflag;
				for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
				{
					Clientflag = 1; 
					Clientflag = Clientflag << i; 

					if( Clientflag & m_ui16MCTimeOutFlg )
					{
						_BattleTimeOutClient_M( i );
					}			 
				}
			}
			_ResetTimeOut(); 
		}		
	}

	return Rv; 
}

void CRoom::_SendEndBattle_M(UINT8 ui8Win, UINT16 ui16BattleEndUserFlag )
{
	//PACKET_BATTLE_ENDBATTLE_ACK Packet; 

	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_ENDBATTLE_ACK);

	SendPacket.WriteData(&ui16BattleEndUserFlag, sizeof(UINT16));
	SendPacket.WriteData(&ui8Win, sizeof(UINT8));

	SendPacket.WriteData(&m_Info._aui16AccExp, sizeof(UINT16)*SLOT_MAX_COUNT);
	SendPacket.WriteData(&m_Info._aui16AccPoint, sizeof(UINT16)*SLOT_MAX_COUNT);
	SendPacket.WriteData(&m_Info._aui8ResultIcon, sizeof(UINT8)*SLOT_MAX_COUNT);

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		SendPacket.WriteData(m_aSlotBonus[i].ui16Exp, sizeof(UINT16)*TYPE_BATTLE_RESULT_EVENT_COUNT);
	}

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		SendPacket.WriteData(m_aSlotBonus[i].ui16Point, sizeof(UINT16)*TYPE_BATTLE_RESULT_EVENT_COUNT);
	}

	SendPacket.WriteData(&m_ui16QuestEndFlag, sizeof(UINT16));

	if (m_Info._i32StageType == STAGE_MODE_BOMB || m_Info._i32StageType == STAGE_MODE_ANNIHILATION || m_Info._i32StageType == STAGE_MODE_DESTROY ||
		m_Info._i32StageType == STAGE_MODE_ESCAPE || m_Info._i32StageType == STAGE_MODE_CROSSCOUNT || m_Info._i32StageType == STAGE_MODE_CONVOY ||
		m_Info._i32StageType == STAGE_MODE_DEFENCE)
	{
		SendPacket.WriteData(&m_Info._aui16TotalRoundCount, sizeof(UINT16) * TEAM_COUNT);
		SendPacket.WriteData(&m_Info._aui8AccMissionCount, sizeof(UINT8) * SLOT_MAX_COUNT);
	}

	if (IsChallengeMode())
	{
		SendPacket.WriteData(&m_Info._aui16AccChallengeScore, sizeof(UINT16) * SLOT_MAX_COUNT);
	}

	SendPacket.WriteData(&m_stMultiKill, sizeof(MULTI_KILL_INFO));

	//결과 보내기

	UINT8 ui8StrSize = 0;
	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (SLOT_STATE_LOAD > m_Info._aSlotInfo[i]._State) continue;

		i3NetworkPacket SendPacket2(PROTOCOL_BATTLE_ENDBATTLE_ACK);
		SendPacket2.CopyToBuffer(SendPacket.GetPacketBuffer(), SendPacket.GetReceivedSize(), 0);

		S2MO_CHAR_BASE_INFO	stBaseInfo;

		ui8StrSize = UINT8(wcslen(m_Info._pSession[i]->GetUserInfoBasic()->m_strNick) * 2);
		SendPacket2.WriteData((UINT16)SendPacket2.GetReceivedSize(), &ui8StrSize, sizeof(UINT8));
		SendPacket2.WriteData(m_Info._pSession[i]->GetUserInfoBasic()->m_strNick, ui8StrSize);

		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Rank, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32KeepRank, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Point, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Exp, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32AbusingCount, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8TutorialIng, sizeof(UINT8));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui64GuideComplete, sizeof(UINT64));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8AuthLevel, sizeof(UINT8));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8SpecialAbility, sizeof(UINT8));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Coin, sizeof(UINT32));

		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Cash, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Clanidx, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32ClanState, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32NCCoin, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32HappyCoin, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8PCCafe, sizeof(UINT8));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8ESportID, sizeof(UINT8));

		ui8StrSize = UINT8(wcslen(m_Info._pSession[i]->GetUserInfoBasic()->m_strClanName)*2);
		SendPacket2.WriteData(&ui8StrSize, sizeof(UINT8));
		SendPacket2.WriteData(m_Info._pSession[i]->GetUserInfoBasic()->m_strClanName, ui8StrSize);

		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8ClanExpRank, sizeof(UINT8));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8ClanPerRank, sizeof(UINT8));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui32ClanMark, sizeof(UINT32));
		SendPacket2.WriteData(&m_Info._pSession[i]->GetUserInfoBasic()->m_ui8ClanNamecolor, sizeof(UINT8));

		SendPacket2.WriteData(&m_Info._pSession[i]->m_UserInfoRecord, sizeof(USER_INFO_RECORD));
		SendPacket2.WriteData(&m_Info._pSession[i]->m_UserInfoDaily, sizeof(USER_INFO_DAILY));

		SendPacket2.WriteData(&m_Info._pSession[i]->m_ui8AbusingLevel, sizeof(UINT8));
		SendPacket2.WriteData(&m_Info._pSession[i]->m_ui32AbusingPopupTime, sizeof(UINT32));

		UINT8 ui8AvatarSlot = (UINT8)m_Info._pSession[i]->m_pDesign->GetCharaSlotByTeam(i%TEAM_COUNT, m_Info._pSession[i]);
		SendPacket2.WriteData(&ui8AvatarSlot, sizeof(UINT8));
		
		m_Info._pSession[i]->m_pDesign->GetMainCharBaseInfo(&m_Info._pSession[i]->m_MultiSlot, &stBaseInfo, i % TEAM_COUNT); 	//m_MultiSlot.GetMainCharBaseInfo(&stBaseInfo);
		SendPacket2.WriteData(&stBaseInfo.m_ui32BattleTime, sizeof(UINT32));
		SendPacket2.WriteData(&stBaseInfo.m_ui32SkillRelTime, sizeof(UINT32));

		m_Info._pSession[i]->SendPacketMessage(&SendPacket2);
		// 랭크업 체크.
		m_Info._pSession[i]->CheckRankUp(MAIN_THREAD_IDX);
	}
}

// 상대팀 슬롯에 빈자리가 있으면 슬롯을 위치한다.
INT32 CRoom::ChangeTeam( CUserSession * pSession, INT32 i32NewTeam )
{
	if( IsMatchMaking() )											return -1;

	INT32 i32SlotIdx = pSession->m_i32SlotIdx;

	// Slot Check
	if( 0 > i32SlotIdx )											return -1;
	if( SLOT_MAX_COUNT <= i32SlotIdx )								return -1;
	// 기본 상태가 아니면 실패처리 
	if( SLOT_STATE_NORMAL != m_Info._aSlotInfo[i32SlotIdx]._State )	return -1;
	//  현재는 2팀밖에 없으므로..
	if( (0 != i32NewTeam) && (1 != i32NewTeam) )					return -1;
	// 같은 팀으로 이동은 의미 없음
	if( i32NewTeam == (i32SlotIdx % TEAM_COUNT) )					return -1;
	// 조인옵션이 기존유저팀을 따르는 것일때는 팀 변경을 할수없다.
	if( ROOM_JOIN_SEQUENCE_USER_FOLLOW == m_Info.m_eJoinOption )	return -1;

	UINT8					ui8MoveCount = 1;
	ROOM_SLOT_CHANGE_INFO	ChangeInfo;

	ChangeInfo.ui8OldSlot		= (UINT8)i32SlotIdx;
	ChangeInfo.ui8OldSlotState	= (UINT8)SLOT_STATE_EMPTY;

	bool bChange = false;
	//새로운 슬롯을 찾는다.
	INT32 i32StartIdx = i32NewTeam;
	m_csSession.Enter();
	for( INT32 i = i32StartIdx ; i < SLOT_MAX_COUNT ; i += TEAM_COUNT )
	{
		if( SLOT_STATE_EMPTY != m_Info._aSlotInfo[i]._State ) continue;

		//Set New Slot 
		i3mem::Copy( &m_Info._aSlotInfo[i],	&m_Info._aSlotInfo[i32SlotIdx],	sizeof(SLOT_INFO) );
		m_Info._pSession[i]					= m_Info._pSession[i32SlotIdx];
		m_Info._pSession[i]->m_i32SlotIdx	= i;
		i3mem::Copy( &m_stRoomUserInfo[i],	&m_stRoomUserInfo[i32SlotIdx], sizeof(ROOM_USER_INFO) );

		//Set Old Slot
		m_Info._aSlotInfo[i32SlotIdx]._State = SLOT_STATE_EMPTY;

		//Change Main Slot Idx
		if( i32SlotIdx == m_Info._i32MainSlotIdx )	m_Info._i32MainSlotIdx = i;
		
		ChangeInfo.ui8NewSlot		= (UINT8)i;
		ChangeInfo.ui8NewSlotState	= (UINT8)m_Info._aSlotInfo[i]._State;
		i32SlotIdx					= i;
		bChange						= true;
		break;
	}
	m_csSession.Leave(); 

	if( false == bChange )		return -1;

	pSession->m_i32SlotIdx	= i32SlotIdx;

	UINT8 ui8MoveReason = 0;
	UINT8 ui8MainSlot	= (UINT8)m_Info._i32MainSlotIdx;

	i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
	Packet.WriteData( &ui8MoveReason,	sizeof(UINT8)  );
	Packet.WriteData( &ui8MainSlot,		sizeof(UINT8)  );
	Packet.WriteData( &ui8MoveCount,	sizeof(UINT8)  );
	Packet.WriteData( &ChangeInfo,		sizeof(ROOM_SLOT_CHANGE_INFO) );
	_SendPacketAllPlayer( &Packet );

	return i32SlotIdx;
}

void CRoom::_SetState_M( UINT8 ui8State, UINT32 ui32ATServer )
{
	if(m_Info._InfoBasic._State == ui8State)
	{
		I3TRACE("[CRoom::_SetState_M] Same State %d \n", ui8State);
		return;
	}

	//방 파괴에 대한 패킷을 보내는 부분 
	//이 부분 다시 확인이 필요합나다. 
	if( m_bIsBattleRoomCreated  )
	{
		if(ROOM_STATE_LOADING < m_Info._InfoBasic._State)
		{
			if( ROOM_STATE_BATTLE_RESULT == ui8State || ROOM_STATE_BATTLE_END == ui8State || ROOM_STATE_READY == ui8State )
			{
				if(isUseBattleServer())
				{
					g_pModuleCast->BattleDestroy( MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx );
				}
				ResetBattleGroupInfo();
			}
		}
	}

	switch(ui8State)
	{
	case ROOM_STATE_READY	: 	
		{
			// COUNTDOWN 에서 돌아왔을 경우에는 별다른 액션을 취하지 않습니다.
			if( ROOM_STATE_COUNTDOWN_R == m_Info._InfoBasic._State )	break;
						
			//만약 게임중의 상태가 있으면 바로 레디상태로 보낸다. 
			UINT32 Reason = (UINT32)EVENT_ERROR_FIRST_HOLE;
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if( SLOT_STATE_READY > m_Info._aSlotInfo[i]._State ) continue;
				
				m_Info._aSlotInfo[i]._State = SLOT_STATE_NORMAL; 
				i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER );
				SendPacket.WriteData( &Reason,	sizeof(UINT32) ); 
				m_Info._pSession[i]->SendPacketMessage(&SendPacket);

				if (LOCALE_CODE_RUSSIA == m_Info._pSession[i]->m_ui8LocaleCode)
				{
					g_pLog->WriteLogSession(m_Info._pSession[i], L"EVENT_ERROR_FIRST_HOLE22");
				}
			}	// -- 이 전 패킷과 합칠수 있는지 확인합니다. --
		}
		break; 
	case ROOM_STATE_COUNTDOWN_R:
		{
			m_ui32ATCountDownR		= ui32ATServer;
			m_i8StartCountdown		= 5;

			i3NetworkPacket SendPacket(PROTOCOL_BATTLE_START_COUNTDOWN_ACK);
			SendPacket.WriteData( &m_i8StartCountdown, sizeof(INT8) );
			_SendPacketAllPlayer( &SendPacket );
		}
		break;
	case ROOM_STATE_LOADING	:
		{	
			m_bIsBattle				= TRUE;

			m_Info._ui32FirstBattleStartUser	= 0;
			m_ui16QuestEndFlag					= 0x00;
			
			m_Info._ui32HackOffUser				= 0;

			for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
			{
				if( SLOT_STATE_NORMAL < m_Info._aSlotInfo[i]._State )
				{
					m_Info._ui32FirstBattleStartUser		|= (0x1 << i);
					m_Info._aSlotInfo[i]._State				= (UINT8)SLOT_STATE_LOAD;
					if( m_Info._pSession[i]->m_ui8QAAutoFlag & QA_AUTO_HACK_DETECT_OFF )
						m_Info._ui32HackOffUser				|= (0x1 << i);
				}
			}
			_GetRandMap();
			_SetUsersScopeType();
			_SendPacketStartGame( NULL );
			_SendSlotInfo(NULL);

			m_bIsBattleRoomCreated	= false; 
			//배틀을 시작합니다. 
			if( !IsTutorialMode() )
			{
				g_pModuleCast->BattleCreate( MAIN_THREAD_IDX, &m_Info, m_Info._ui32FirstBattleStartUser ); 
			}
			else 
			{
				m_bIsBattleRoomCreated	= true; 
				m_ui8BattleType			= BATTLE_SERVER_TYPE_P2P;
			}
		}
		break;
	case ROOM_STATE_PRE_BATTLE	: 
		{
			// 그게 아니라면 그냥 홀펀칭을 시작해라
			for(INT32 i = 0 ; i < SLOT_MAX_COUNT; i++)
			{
				if( m_Info._aSlotInfo[i]._State == SLOT_STATE_PRESTART )
				{
					_SendPreStartBattle(m_Info._pSession[i], TRUE ); 
				}
			}
		}
		break; 
	case ROOM_STATE_COUNTDOWN_B:
		{
			UINT8	ui8CountDownTime = m_pRoomMode->GetCountDownTime();
			m_ui32ATCountDownB = ui32ATServer +  ui8CountDownTime;

			for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
			{
				if( m_Info._aSlotInfo[i]._State > SLOT_STATE_RENDEZVOUS )
				{
					if( NULL == m_Info._pSession[i] ) continue;

					i3NetworkPacket CountDownPacket(PROTOCOL_BATTLE_COUNT_DOWN_ACK); 
					CountDownPacket.WriteData( &ui8CountDownTime,		sizeof(UINT8) );

					m_Info._pSession[i]->SendPacketMessage(&CountDownPacket);
				}
			}
		}
		break;
	case ROOM_STATE_BATTLE	: 
		{
			// 게임 최초 시작 시간 - ZLog 에 기록하기 위해서 필요합니다. - 서동권
			m_Info._ui32GameStartDate	= i3ThreadTimer::LocalTime().GetDateTimeYYMMDDHHMI();

			m_Info._ui16BattleUser		= 0;		// 게임에 들어온 유저
			m_Info._ui16RoundDeathUser	= 0;		// 죽은 유저
			m_i32TotalRespawnIdx		= 0;
			m_Info._ui8MiEndType		= MISSION_END_TYPE_NONE;

			// 유니크한 배틀 타임
			m_Info.m_ui64BattleUID		= ((UINT64)m_ui32LogUniqueNumber << 32) + i3ThreadTimer::LocalTime().GetDate();
			_InitBattleTime();

			//Set Score & Record & Point
			i3mem::FillZero( m_aSlotBonus,					sizeof( SLOT_BONUS ) * SLOT_MAX_COUNT );
			i3mem::FillZero( m_Info._aui32RespawnCount,		(sizeof(INT32)*SLOT_MAX_COUNT ) );
			i3mem::FillZero( m_Info._aTeamScoreTotal,		(sizeof(SCORE_INFO) * TEAM_COUNT) );
			i3mem::FillZero( m_Info._aM_UserScore,			(sizeof(SCORE_INFO) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_PiercingShot,	(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_MassKill,		(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_ChainStoppper,	(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_ChainHeadShot,	(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_ChainSlugger,	(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_AccKill_1,		(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_AccKill_2,		(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_AccKill_3,		(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16M_Headshot,		(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16AccExp,			(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16AccPoint,			(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui16AccChallengeScore,	(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info.m_aui8A_HeadShot,		(sizeof(UINT8) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info.m_aui8A_SlugShot,		(sizeof(UINT8) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info.m_aui8A_KillCount,		(sizeof(UINT8) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui8AccMissionCount,	(sizeof(UINT8) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._abDinoStateUpdate,		(sizeof(bool) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._aui32DInoCharInfo,		(sizeof(UINT32) * SLOT_MAX_COUNT) );
			i3mem::FillZero( m_Info._ai32AcidBombingCount,	(sizeof(INT32) * SLOT_MAX_COUNT) );

			i3mem::FillZero( m_ui8MultiKillCount,			sizeof( UINT8 ) * SLOT_MAX_COUNT );
			i3mem::FillZero( &m_stMultiKill,				sizeof( MULTI_KILL_INFO ) );

			i3mem::FillZero( m_Info._aui16Acc_KillExp,		(sizeof(UINT16) * SLOT_MAX_COUNT) );
			i3mem::FillZero(m_Info._aui16Acc_AssistExp,		(sizeof(UINT16) * SLOT_MAX_COUNT));

			for(INT32 i = 0; i< SLOT_MAX_COUNT; i++)
			{
				m_bReSpawnFlag[i]	= false; 
				m_bDeathFlag[i]		= false;
				
				if( SLOT_STATE_BATTLE_READY != m_Info._aSlotInfo[i]._State ) continue;
				
				if(FALSE == m_Info._aSlotInfo[i]._bIsGmObserverMode)
				{ 
					m_Info._ui16BattleUser |= (0x01 << i); 
				}
				m_Info._pSession[i]->m_ui32RTBattleCheck	= GetRoomBattleTime();
				m_Info._aSlotInfo[i]._State					= SLOT_STATE_BATTLE;
				i3mem::FillZero( m_Info._pSession[i]->m_aui8QuestCtxActiveNow, sizeof(UINT8)*MAX_QUEST_PER_CARD_NEW );
				m_Info._pSession[i]->m_ui16ActiveIndexCardNow	= 0;

				MedalMissionCheck( MAIN_THREAD_IDX, m_Info._pSession[i], MEDAL_MC_ACTION_ID_STAGE_ENTER );				
				SetUserMissionCard( m_Info._pSession[i], ACTION_ID_STAGE_ENTER );
				
				// 어뷰징 체크 - 배틀 시작 시간 설정.
				m_Info._pSession[i]->m_ui32RTBattleStart = 0;
				m_bAlive[i]			= false;
			}

			m_Info._ui16RoundStartUser						= (UINT16)m_Info._ui32FirstBattleStartUser;

			m_pRoomMode->SetMatchStart();

			// 챌린지모드 설정
			if ( IsChallengeMode() )
			{
				m_Info.m_i8currentDifficultyLevel = m_Info.m_i8difficultyLevel;

				for( UINT32 i = 0 ; i < SLOT_MAX_COUNT; i++ )
				{
					m_Info.m_aui32AILevel[ i ] = m_Info.m_i8currentDifficultyLevel;
				}
			}

			m_bVoting = FALSE; // 피신고자가 투표 도중 나갈시 게임이 종료되면 투표 진행상태가 초기화 되지않아 새로운 게임시작시 별도로 초기화 시켜줍니다.

			//로그 배틀 시작
			m_ui32ATSendPing	= ui32ATServer;
			m_ui16BattleUser	= m_Info._ui16BattleUser;
		}
		break;
	case ROOM_STATE_BATTLE_END	: 
		{
			if ( m_bGMPause )
			{
				m_bGMPause = FALSE;
				g_pModuleCast->BattleGMPause( MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx, false );
				SendGMResume();
			}

			_SendEndBattle_M( (UINT8)m_i32WinTeam, m_ui16BattleEndUserFlag );							// 엔드 패킷 보내기 	

			//Add BattleCount
			m_ui32LogBattleCount++; 

			//TimeOut Check 
			_ResetTimeOut(); 

			//For Mission
			m_Info._InfoAdd._NowRoundCount	= 0;
			m_Info._InfoAdd._NowIngTime		= 0;
			m_ui8AtkDefChange				= 0;
			i3mem::FillZero( m_Info._aui16TotalRoundCount,	sizeof(UINT16)*TEAM_COUNT );
			i3mem::FillZero( m_Info._aTeamScoreTotal,		sizeof(SCORE_INFO)*TEAM_COUNT );

			//로그 배틀 종료
			g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_R_BATTLE_END_ROOM, NULL, this); 

			//Set User State 
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
			{
				m_stRoomUserInfo[i]._bChangeMain = FALSE;

				if( m_Info._aSlotInfo[i]._State > SLOT_STATE_NORMAL )//11.25일 변경
				{
					m_Info._pSession[i]->m_bIsBattleComplete	= TRUE;
					m_Info._aSlotInfo[i]._State					= SLOT_STATE_NORMAL;
					m_Info._pSession[i]->m_ui8LoadingStart		= LOAD_START_BATTLE_END;
					m_Info._pSession[i]->SetPosition(GAME_TASK_READY_ROOM);
					INT32 i32EndType;

					if(m_i32WinTeam == TEAM_DRAW)
					{
						i32EndType = ROOM_ENDTYPE_DRAW;
					}
					else
					{
						if(m_i32WinTeam == (i%2))
						{
							i32EndType = ROOM_ENDTYPE_WIN;
						}
						else
						{
							i32EndType = ROOM_ENDTYPE_LOSE;
						}
					}

					g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_R_BATTLE_END, m_Info._pSession[i], this, &i32EndType );
					m_Info.m_ui8PlayRoundCound[i] = 0;//플레이한 라운드 초기화

					INT32 i32GetExp		= m_Info._aui16AccExp[i] + m_Info._aui16EventExp[i]; 
					if( i32GetExp > 0 ) 
					{
						INT32 i32GetExpRoute = GET_EXP_ROUTE_GAMEPLAY;
						g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_GET_EXP, m_Info._pSession[i], &i32GetExp, &i32GetExpRoute );
					}

					m_Info._pSession[i]->CheckInventory_MAU(MAIN_THREAD_IDX);

					//Set Rank
					m_Info._aSlotInfo[i]._Rank	 = (UINT8)m_Info._pSession[i]->m_ui32ViewRank;

					// 어뷰저는 강제로 로비로 이동 시킨다.	
					if ( ABUSING_LEVEL_NONE < m_Info._pSession[i]->m_ui8AbusingLevel ) 
					{
						g_pRoomManager->OnLeaveRoom(m_Info._pSession[i], m_Info._pSession[i]->m_i32ChannelNum, MAIN_THREAD_IDX, ROOM_ENDTYPE_HACKING );
					}

					// 어뷰징 정보 초기화

					m_Info._pSession[i]->m_ui32AbusingPopupTime = 0;
					m_Info._pSession[i]->m_ui32RTBattleStart	= 0;
					m_Info._pSession[i]->m_ui32PlayTime			= 0;
				
				}	
			}
			//Set State 강제로 레디로 변경 
			ui8State = ROOM_STATE_READY; 
			m_ui32RoomBattleTime = 0; //방 플레이타임 초기화
		}
		break; 
	}

	m_Info._InfoBasic._State	= ui8State;
	m_ui32ATRoomState			= ui32ATServer;	
	_SendRoomInfo(NULL);  

	return; 
}

void CRoom::_SetMissionState_M(UINT8 ui8State)
{
	if(m_Info._i32MiStateType == ui8State)
	{
		I3TRACE("[CRoom::SetMissionState] Same State %d \n", ui8State); 
		return; 
	}

	switch(ui8State)
	{
	case MISSION_STATE_PRE_BATTLE	: 
		{
			m_pRoomMode->SetRoundStart();

			_SetMissionStatePreBattle();							// PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK 전송

		}		
		break; 
	case MISSION_STATE_BATTLE		: 
		{
			m_Info._InfoAdd._NowRoundCount++;

			m_Info._ui32RTRoundStart = GetRoomBattleTime();
			//0. 남은 시간 계산 	
			UINT8	ui8EndTimeIdx	= (m_Info._InfoAdd._SubType & 0xF0) >> 4;
			UINT32	ui32RemainTime	= g_TeamTimeTable[ui8EndTimeIdx];

			if ( STAGE_MODE_ESCAPE != m_Info._i32StageType )	m_pRoomMode->RoundStart( GetRoomBattleTime() );
			else  m_pRoomMode->RoundStart( GetRoomRealBattleTime() );
			//유저 플레이 라운드 수 올리기.
			for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
			{
				if (SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State) continue;
				m_Info.m_ui8PlayRoundCound[i] += 1;
			}
			//라운드 시작 패킷 
			switch( m_ui8BattleType ) 
			{
			default :
			case BATTLE_SERVER_TYPE_RELAY :
				{
					i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_ROUND_START_ACK );
					SendPacket.WriteData( &m_Info._InfoAdd._NowRoundCount,	sizeof(UINT8) );
					SendPacket.WriteData( &ui32RemainTime,					sizeof(UINT32));
					SendPacket.WriteData( &m_Info._ui16RoundStartUser,		sizeof(UINT16) );
					SendPacket.WriteData( &m_ui8AtkDefChange,				sizeof(UINT8) );
					_SendPacketBattlePlayer( &SendPacket ); 

					if( !isUseBattleServer() ) break;
				}
				// break;  주석이 맞음.
			case BATTLE_SERVER_TYPE_DEDICATION :
				g_pModuleCast->BattleRoundStart( MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx, &m_Info, m_ui8AtkDefChange );
				break;
			}
		}
		break; 
	case MISSION_STATE_FREE		: 
		{
			//Send Message	
			i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_ROUND_END_ACK );
			SendPacket.WriteData( &m_Info._ui8RoundWinTeam,			sizeof(UINT8) );	//m_Info._ui8MiEndType
			SendPacket.WriteData( &m_Info._ui8MiEndType,			sizeof(UINT8) );
			SendPacket.WriteData( &m_Info._aui16TotalRoundCount,	(sizeof(UINT16)*TEAM_COUNT));
			_SendPacketBattlePlayer( &SendPacket );

			// 라운드가 끝날때 그 라운드에서 실제 플레이한 시간 계산
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[i]._State ) continue;

				if( m_Info._pSession[i]->m_ui32RTRespawn > 0 )
					m_Info._pSession[i]->m_ui32RTAccActualBattle += GetRoomBattleTime() - m_Info._pSession[i]->m_ui32RTRespawn;
			}
		}
		break; 
	case MISSION_STATE_RESULT	: 
		{

		}
		break; 
	default: break; 
	}

	m_Info._i32MiStateType		= ui8State; 
	m_ui32RTMissionRoundState	= GetRoomBattleTime();
	return; 
}

void CRoom::_SetMissionStatePreBattle()
{
	m_Info._ui16RoundDeathUser	= 0;	// 죽은 유저
	m_ui8AtkDefChange = m_pRoomMode->GetAtkDefChange();

	T_ItemID ui32CharitemId[SLOT_MAX_COUNT];

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++) // 전체 데이터를 만들고 전달해야 하기때문에 for문을 분리시켰습니다.
	{
		ui32CharitemId[i] = GetCharItemId(i);
	}

	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK);
	SendPacket.WriteData(&m_Info._ui16RoundStartUser, sizeof(UINT16));
	SendPacket.WriteData(&m_Info._MiDinoInfo, sizeof(DINO_ROOM_INFO));		//사이즈가 크지 않아 그냥 보낸다. 
	SendPacket.WriteData(&m_ui8AtkDefChange, sizeof(UINT8));

	if (ATK_DEF_CHANGING & m_ui8AtkDefChange)
	{
		SendPacket.WriteData(ui32CharitemId, sizeof(T_ItemID) * SLOT_MAX_COUNT);
	}

	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( 0 == ((0x01 << i) & m_Info._ui16RoundStartUser) )		continue;
		
		m_Info._pSession[i]->m_ui32RTBattleCheck = GetRoomBattleTime();

		m_bAlive[i]					= false;

		m_Info._pSession[i]->SendPacketMessage( &SendPacket );
	}

	i3mem::FillZero( m_Info._aui32RespawnCount,		(sizeof(INT32)*SLOT_MAX_COUNT ) );
	i3mem::FillZero( m_bReSpawnFlag,				sizeof(bool)*SLOT_MAX_COUNT );
	i3mem::FillZero( m_bDeathFlag,					sizeof(bool)*SLOT_MAX_COUNT );
	i3mem::FillZero( m_Info._aUserScoreRound,		sizeof(SCORE_INFO)*SLOT_MAX_COUNT );
	i3mem::FillZero( m_Info._aTeamScoreRound,		sizeof(SCORE_INFO)*TEAM_COUNT );
	i3mem::FillZero( m_Info.m_aui8A_KillCount,		(sizeof(UINT8) * SLOT_MAX_COUNT) );	//킬카운트 
	i3mem::FillZero( m_Info.m_aui8A_HeadShot,		(sizeof(UINT8) * SLOT_MAX_COUNT) );	//헤드샷 
	i3mem::FillZero( m_Info.m_aui8A_SlugShot,		(sizeof(UINT8) * SLOT_MAX_COUNT) );	//슬러그킬 	
	i3mem::FillZero( m_Info._ai32AcidBombingCount,	(sizeof(INT32) * SLOT_MAX_COUNT) );

	return; 
}	

void CRoom::HoleCheck( CUserSession * pSession )
{	
	if( m_Info._InfoBasic._State < ROOM_STATE_PRE_BATTLE )	return;
	INT32 SlotIdx = pSession->m_i32SlotIdx;	

	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_HOLE_CHECK_ACK); 
	SendPacket.WriteData( &SlotIdx, sizeof(INT32)); 

	//1. Send 방장과 자신에게 메시지 보내기 
	m_Info._pSession[m_Info._i32MainSlotIdx]->SendPacketMessage( &SendPacket ); 
	m_Info._pSession[SlotIdx]->SendPacketMessage(	&SendPacket ); 
	return; 
}

// 일반 유저만 처리됨. 옵저버,히든유저는 해당사항 없음
void CRoom::UnreadySlot(void)
{

	m_csSession.Enter();

	// Ready 해제 통보 sessionIdx
	INT32 i32unreadyCount = 0;
	INT32 ai32unreadySessionIdx[SLOT_MAX_COUNT];

	// 방장을 제외한 각 팀 참여자수를 계산한다
	INT32 i32MaxPersonPerTeam = g_pContextMain->GetMaxPerson(m_Info._InfoBasic._StageID);
	INT32 i32redLimitCount		= i32MaxPersonPerTeam / TEAM_COUNT;
	INT32 i32blueLimitCount		= i32MaxPersonPerTeam / TEAM_COUNT;

	if (m_Info._i32MainSlotIdx % 2 == 0)
	{
		i32redLimitCount--;
	}
	else
	{
		i32blueLimitCount--;
	}

	// 참여자 제한에 걸리면 ready 상태를 해제한다.
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (SLOT_STATE_READY <= m_Info._aSlotInfo[i]._State && m_Info._pSession[i] != NULL)
		{
			// 방장은 무조건 참가.
			if (i == m_Info._i32MainSlotIdx)	continue;

			// 참가 제한에 걸리면 Ready 상태 해제 결정
			BOOL unready = FALSE;

			if (i % 2 == 0)
			{
				if (0 < i32redLimitCount)
				{
					i32redLimitCount--;
				}
				else
				{
					unready = TRUE;
				}				
			}
			else
			{
				if (0 < i32blueLimitCount)
				{
					i32blueLimitCount--;
				}
				else
				{
					unready = TRUE;
				}
			}

			// ready 해제
			if (unready)
			{
				m_Info._aSlotInfo[i]._State = SLOT_STATE_NORMAL;

				ai32unreadySessionIdx[i32unreadyCount] = i;
				i32unreadyCount++;
			}
		}
	}

	m_csSession.Leave();


	// ready 상태 해제 통보
	for(INT32 i = 0; i < i32unreadyCount; i++)
	{
		CUserSession* session = m_Info._pSession[ ai32unreadySessionIdx[i] ];

		if (session != NULL)
		{
			i3NetworkPacket packet(PROTOCOL_ROOM_UNREADY_4VS4_ACK);
			session->SendPacketMessage(&packet);
		}
	}
}

BOOL CRoom::IsAllowJoin(CUserSession * pSession)
{
	INT32 i32playedTeamSlotCount = 0;


	m_csSession.Enter();

	// 자신의 팀 첫 슬롯계산
	INT32 startIdx = pSession->m_i32SlotIdx % 2;

	for(INT32 i = startIdx; i < SLOT_MAX_COUNT; i += 2)
	{
		// 게임에 참여중인 팀원수를 계산한다.
		if (SLOT_STATE_READY <= m_Info._aSlotInfo[i]._State && m_Info._pSession[i] != NULL)
		{
			i32playedTeamSlotCount++;
		}
	}

	m_csSession.Leave();

	INT32 i32MaxPlayUser = g_pContextMain->GetMaxPerson(m_Info._InfoBasic._StageID);

	if ( -1 == i32MaxPlayUser ) return FALSE;

	if ( i32playedTeamSlotCount < i32MaxPlayUser / TEAM_COUNT )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CRoom::ChangeTeamForBalance(void)
{
	m_csSession.Enter();

	// 각 팀의 수를 센다
	INT32 i32redCount = 0;
	INT32 i32blueCount = 0;
	INT32 ai32beingTable[TEAM_COUNT][SLOT_MAX_COUNT / 2];

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		// 유저가 있는 슬롯
		if( (SLOT_STATE_READY <= m_Info._aSlotInfo[i]._State) && (m_Info._pSession[i] != NULL) )
		{
			if (0 == i % 2)
			{				
				ai32beingTable[0][i32redCount] = i;
				i32redCount++;
			}
			else
			{
				ai32beingTable[1][i32blueCount] = i;
				i32blueCount++;
			}
		}
	}

	// 밸런싱 - 2명 이상 차이가 나면 조절한다.
	INT32 i32balance = i32redCount - i32blueCount;
	if( i32balance < 0 ) i32balance = -i32balance;	// 절대값.
	i32balance /= 2;

	UINT8					ui8MoveCount = 0;
	ROOM_SLOT_CHANGE_INFO	aChangeInfo[ SLOT_MAX_COUNT ];

	if (0 < i32balance)
	{
		INT32 i32currentTeam= i32redCount < i32blueCount ? 1 : 0;
		INT32 i32changeTeam	= i32redCount < i32blueCount ? 0 : 1;
		INT32 i32endIdx		= MAX(i32redCount, i32blueCount) - 1;

		for(INT32 j = 0; j < i32balance; j++)
		{
			INT32 i32slotIdx = ai32beingTable[i32currentTeam][i32endIdx];

			// 이동할 팀의 비어있는 슬롯을 찾는다.
			for(INT32 i = i32changeTeam; i < SLOT_MAX_COUNT; i += 2)
			{
				if (SLOT_STATE_EMPTY == m_Info._aSlotInfo[i]._State)
				{
					// SlotInfo 복사
					i3mem::Copy(&m_Info._aSlotInfo[i], &m_Info._aSlotInfo[i32slotIdx], sizeof(SLOT_INFO));
					m_Info._aSlotInfo[i32slotIdx].Reset( SLOT_STATE_EMPTY );

					// Session 복사
					m_Info._pSession[i] = m_Info._pSession[i32slotIdx];
					m_Info._pSession[i]->m_i32SlotIdx = i;
					
					// 방장이면 방장 idx 이동
					if (i32slotIdx == m_Info._i32MainSlotIdx)
					{
						m_Info._i32MainSlotIdx = i;
					}

					// 이동이 발생하였다.
					aChangeInfo[ui8MoveCount].ui8OldSlot		= (UINT8)i32slotIdx;
					aChangeInfo[ui8MoveCount].ui8NewSlot		= (UINT8)i;
					aChangeInfo[ui8MoveCount].ui8OldSlotState	= SLOT_STATE_EMPTY;
					aChangeInfo[ui8MoveCount].ui8NewSlotState	= m_Info._aSlotInfo[i]._State;
					ui8MoveCount++;
					break;
				}
			}
			
			i32endIdx--;
		}
	}

	m_csSession.Leave();

	// 이동이 일어났으면 패킷전달
	if (0 < ui8MoveCount)
	{
		UINT8 ui8MoveReason = 1;
		UINT8 ui8MainSlot	= (UINT8)m_Info._i32MainSlotIdx;

		i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
		Packet.WriteData( &ui8MoveReason,	sizeof(UINT8) );
		Packet.WriteData( &ui8MainSlot,		sizeof(UINT8) );
		Packet.WriteData( &ui8MoveCount,	sizeof(UINT8) );
		Packet.WriteData( aChangeInfo,		(UINT16)(sizeof(ROOM_SLOT_CHANGE_INFO) * ui8MoveCount) );
		_SendPacketAllPlayer( &Packet );
	}
}

void CRoom::ChangeTeamForBalance2( )
{
	m_csSession.Enter();

	INT32		i32UserCount = 0;
	INT8 *		pi8SlotSequence;

	BOOL		bChangeSlotState = false;

	UINT32		aui32KillDeath[SLOT_MAX_COUNT] = {0,};

	UINT8		ui8SlotMaxCount = g_pContextMain->GetMaxPerson(m_Info._InfoBasic._StageID);
	for(INT32 i = 0; i < ui8SlotMaxCount; i++)
	{
		// 유저가 있는 슬롯
		if( (SLOT_STATE_READY <= m_Info._aSlotInfo[i]._State) && (m_Info._pSession[i] != NULL) )
		{
			i32UserCount++;

			// 이후 킬데스 비교를 위해 비율을 미리 계산해둠
			CUserSession *pUserSession	= m_Info._pSession[i];
			if( pUserSession->m_UserInfoRecord._death == 0 && pUserSession->m_UserInfoRecord._killcount == 0)
				aui32KillDeath[i] = 0;
			else
				aui32KillDeath[i] = (pUserSession->m_UserInfoRecord._killcount*10000) / (pUserSession->m_UserInfoRecord._killcount + pUserSession->m_UserInfoRecord._death);		// 킬/데스(%)
		}

	/*	if( SLOT_STATE_CLOSE == m_Info._aSlotInfo[i]._State )
		{
			m_Info._aSlotInfo[i]._State = SLOT_STATE_EMPTY;
			if (m_Info._InfoBasic._PersonPlayerMax < SLOT_MAX_COUNT)
			{
				m_Info._InfoBasic._PersonPlayerMax++;
				bChangeSlotState = true;
			}
		}*/
	}

	if( bChangeSlotState )
	{
		_SendSlotInfo(NULL);
	}

	// 유저수가 4명 이하면 팀 바꾸지 않는다.
	if( i32UserCount < 4 )
	{
		m_csSession.Leave();
		return;
	}

	UINT8 ui8MoveCount = 0;
	ROOM_SLOT_CHANGE_INFO pChangeInfo[SLOT_MAX_COUNT];

	// 유저 수 짝수일때, 홀수일때에 따라 배치 순서 설정
	if (0 < i32UserCount%2 )
	{
		pi8SlotSequence = g_SlotSequenceforOdd;
	}
	else
	{
		pi8SlotSequence = g_SlotSequenceforEven;
	}

	for( INT32 i = 0; i < i32UserCount; i++ )
	{
		INT32 i32OrgSlot = pi8SlotSequence[i];
		INT32 i32DestSlot = pi8SlotSequence[i];

		// 남은 유저 순위 확인
		CUserSession *pOrgUserSession	= 0;
		UINT32 ui32OrgExp					= 0xFFFFFFFF;	// 작을수록 우선배치하므로 최대값으로 초기화
		UINT32 ui32OrgKillDeath				= 0;
		for( INT32 j = i; j < SLOT_MAX_COUNT; j++ )
		{
			if( (SLOT_STATE_READY <= m_Info._aSlotInfo[pi8SlotSequence[j]]._State) && (m_Info._pSession[pi8SlotSequence[j]] != NULL) )
			{

				CUserSession *pCheckUserSession	= m_Info._pSession[pi8SlotSequence[j]];

				UINT32 ui32CheckExp					= pCheckUserSession->m_ui16PrevExpRank;			// 이전 방에서 플레이한 경험치 순위비율
				UINT32 ui32CheckKillDeath = aui32KillDeath[pi8SlotSequence[j]];

				bool bDoSwap = false;
				if( ui32OrgExp > ui32CheckExp )						bDoSwap = true;
				else if( ui32OrgExp == ui32CheckExp &&
						 ui32OrgKillDeath < ui32CheckKillDeath	)	bDoSwap = true;

				if( bDoSwap )
				{
					pOrgUserSession	= pCheckUserSession;
					ui32OrgKillDeath	= ui32CheckKillDeath;
					ui32OrgExp			= ui32CheckExp;

					i32OrgSlot = pi8SlotSequence[j];
				}
			}
		}

		if( i32OrgSlot != i32DestSlot )
		{
			// pSession 스왑
			if( m_Info._aSlotInfo[i32DestSlot]._State <= SLOT_STATE_CLOSE )
			{
				// 이동할 슬롯이 비어있으면 스왑할 필요 없다. 내 정보만 이동시킨다.
				m_Info._pSession[i32DestSlot]		= m_Info._pSession[i32OrgSlot];
				m_Info._pSession[i32DestSlot]->m_i32SlotIdx = i32DestSlot;
			}
			else
			{
				CUserSession *pTempUserSession	= m_Info._pSession[i32DestSlot];
				m_Info._pSession[i32DestSlot]		= m_Info._pSession[i32OrgSlot];
				m_Info._pSession[i32OrgSlot]		= pTempUserSession;
				m_Info._pSession[i32OrgSlot]->m_i32SlotIdx = i32OrgSlot;
				m_Info._pSession[i32DestSlot]->m_i32SlotIdx = i32DestSlot;
			}

			// m_Info_SlotInfo 스왑
			SLOT_INFO	TempSlotInfo;
			i3mem::Copy( &TempSlotInfo, &m_Info._aSlotInfo[i32DestSlot], sizeof(SLOT_INFO) );
			i3mem::Copy( &m_Info._aSlotInfo[i32DestSlot], &m_Info._aSlotInfo[i32OrgSlot], sizeof(SLOT_INFO) );
			i3mem::Copy( &m_Info._aSlotInfo[i32OrgSlot],  &TempSlotInfo, sizeof(SLOT_INFO) );

			// KillDeath 비율도 슬롯 따라 교환
			UINT32 ui32KillDeathTemp = aui32KillDeath[i32DestSlot];
			aui32KillDeath[i32DestSlot] = aui32KillDeath[i32OrgSlot];
			aui32KillDeath[i32OrgSlot] = ui32KillDeathTemp;

			// 방장이면 방장 idx 이동
			if (i32OrgSlot == m_Info._i32MainSlotIdx)			m_Info._i32MainSlotIdx = i32DestSlot;
			else if (i32DestSlot == m_Info._i32MainSlotIdx)		m_Info._i32MainSlotIdx = i32OrgSlot;

			// 이동이 발생하였다.
			pChangeInfo[ui8MoveCount].ui8OldSlot		= (UINT8)i32OrgSlot;
			pChangeInfo[ui8MoveCount].ui8NewSlot		= (UINT8)i32DestSlot;
			pChangeInfo[ui8MoveCount].ui8OldSlotState	= m_Info._aSlotInfo[i32OrgSlot]._State;
			pChangeInfo[ui8MoveCount].ui8NewSlotState	= m_Info._aSlotInfo[i32DestSlot]._State;
			ui8MoveCount++;
		}
	}

	m_csSession.Leave();


	// 이동이 일어났으면 패킷전달
	if (0 < ui8MoveCount)
	{
		UINT8 ui8MoveReason = 1;
		UINT8 ui8MainSlot	= (UINT8)m_Info._i32MainSlotIdx;

		i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
		Packet.WriteData( &ui8MoveReason,	sizeof(UINT8) );
		Packet.WriteData( &ui8MainSlot,		sizeof(UINT8) );
		Packet.WriteData( &ui8MoveCount,	sizeof(UINT8) );
		Packet.WriteData( pChangeInfo,		(UINT16)(sizeof(ROOM_SLOT_CHANGE_INFO) * ui8MoveCount) );
		_SendPacketAllPlayer( &Packet );
	}
}

void CRoom::SendChangeNickname(INT32 i32slotIdx)
{
	// 닉네임 변경권 사용으로 인해, 닉네임이 변경되었을 경우
	// 방에 있다면 스롯 닉네임 변경을 전송한다.
	_SendNickName(NULL, i32slotIdx, TRUE);
}

void CRoom::SendChangeRank(INT32 i32slotIdx)
{
	// 위장 계급 변경권 사용으로 인해, 계급이 변경되었을 경우
	// 방에 있다면 스롯 계급 변경을 전송한다.
	_SendRank(NULL, i32slotIdx, TRUE);
}

void CRoom::SendChangeColorNick(INT32 i32slotIdx)
{
	// 위장 계급 변경권 사용으로 인해, 계급이 변경되었을 경우
	// 방에 있다면 스롯 계급 변경을 전송한다.
	_SendChangeColorNick(NULL, i32slotIdx, TRUE);
}

void CRoom::SendChangeColorMuzzleFlash(INT32 i32slotIdx)
{
	// 총구 화염 컬러 변경권 사용혹은 변경한 경우
	// 해당 정보를 같은 방에 있는 유저들에게 전송한다.
	_SendChangeColorMuzzleFlash( i32slotIdx, TRUE);
}

INT32 CRoom::GetNowTeam(INT32 i32SlotIdx)
{
	if ( 0 > i32SlotIdx )				return -1;
	if (SLOT_MAX_COUNT <= i32SlotIdx)	return -1;

	INT32 i32Team = (0 < m_ui8AtkDefChange) ? 1 : 0;

	return (i32Team + i32SlotIdx) % TEAM_COUNT;
}

void CRoom::ChangeTeamForBalance(CUserSession * pSession, UINT8 ui8Option)
{
	m_csSession.Enter();

	if (NULL == pSession)	
	{
		m_csSession.Leave();
		return;	
	}

	// 각 팀의 수를 센다
	INT32 i32RedCount = 0;
	INT32 i32BlueCount = 0;

	UINT16 ui16RedPrevExpRank = 0;
	UINT16 ui16BluePrevExpRank = 0;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		// 유저가 있는 슬롯
		if( i == pSession->m_i32SlotIdx ) continue;
		if( (SLOT_STATE_READY <= m_Info._aSlotInfo[i]._State) && (m_Info._pSession[i] != NULL) )
		{
			if (0 == i % 2)
			{				
				i32RedCount++;
				ui16RedPrevExpRank += m_Info._pSession[i]->m_ui16PrevExpRank;
			}
			else
			{
				i32BlueCount++;
				ui16BluePrevExpRank += m_Info._pSession[i]->m_ui16PrevExpRank;
			}
		}
	}

	UINT8 ui8MoveCount = 0;
	ROOM_SLOT_CHANGE_INFO aChangeInfo[SLOT_MAX_COUNT];

	INT32 i32SlotIdx = pSession->m_i32SlotIdx;
	INT32 i32ChangeTeam = i32SlotIdx%TEAM_COUNT;
	if (i32RedCount != i32BlueCount )
	{
		// 인원 차이가 나면 인원이 적은 팀으로 시작한다.
		i32ChangeTeam = i32RedCount < i32BlueCount ? 0 : 1;
	}
	else if( ui8Option == 2 )
	{
		// 옵션2일때 - 양팀 인원이 같으면 평균 등수/인원수 수치가 높은 팀으로 시작한다.
		if( ui16RedPrevExpRank > ui16BluePrevExpRank )
			i32ChangeTeam = 0;
		else if( ui16RedPrevExpRank < ui16BluePrevExpRank )
			i32ChangeTeam = 1;
		else
		{
			m_csSession.Leave();
			return;
		}
	}
	else
	{
		m_csSession.Leave();
		return;
	}

	// 이동할 팀의 비어있는 슬롯을 찾는다.
	for(INT32 i = i32ChangeTeam; i < SLOT_MAX_COUNT; i += 2)
	{
		if( i == i32SlotIdx ) break;	// 내가 이미 있는 슬롯

		if (SLOT_STATE_EMPTY == m_Info._aSlotInfo[i]._State)
		{
			// Session 복사
			m_Info._pSession[i]					= m_Info._pSession[i32SlotIdx];
			m_Info._pSession[i]->m_i32SlotIdx	= i;

			// SlotInfo 복사
			i3mem::Copy(&m_Info._aSlotInfo[i], &m_Info._aSlotInfo[i32SlotIdx], sizeof(SLOT_INFO));
			m_Info._aSlotInfo[i32SlotIdx].Reset( SLOT_STATE_EMPTY );

			// 방장이면 방장 idx 이동
			if (i32SlotIdx == m_Info._i32MainSlotIdx)
			{
				m_Info._i32MainSlotIdx = i;
			}

			// 이동이 발생하였다.
			aChangeInfo[ui8MoveCount].ui8OldSlot		= (UINT8)i32SlotIdx;
			aChangeInfo[ui8MoveCount].ui8NewSlot		= (UINT8)i;
			aChangeInfo[ui8MoveCount].ui8OldSlotState	= SLOT_STATE_EMPTY;
			aChangeInfo[ui8MoveCount].ui8NewSlotState	= m_Info._aSlotInfo[i]._State;
			ui8MoveCount++;
			break;
		}
	}

	m_csSession.Leave();

	// 이동이 일어났으면 패킷전달
	if (0 < ui8MoveCount)
	{
		UINT8 ui8MoveReason = 1;
		UINT8 ui8MainSlot	= (UINT8)m_Info._i32MainSlotIdx;

		i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
		Packet.WriteData( &ui8MoveReason,	sizeof(UINT8));
		Packet.WriteData( &ui8MainSlot,		sizeof(UINT8));
		Packet.WriteData( &ui8MoveCount,	sizeof(UINT8));
		Packet.WriteData( aChangeInfo,		(UINT16)(sizeof(ROOM_SLOT_CHANGE_INFO) * ui8MoveCount));
		_SendPacketAllPlayer( &Packet );
	}
}

void CRoom::MissionBombInstall( INT32 i32ThreadIdx, CUserSession * pMainSession, INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos)
{
	// 미션상태가 전투중이 아니면 폭탄 설치 불가
	if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )
	{
		// 튜토리얼은 예외처리
		if( !IsTutorialMode() )
			return;
	}
	if( ( i32SlotIdx < 0 ) || ( SLOT_MAX_COUNT <= i32SlotIdx ) ) return;
	if( SLOT_STATE_BATTLE	 != m_Info._aSlotInfo[i32SlotIdx]._State )return;

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return; //잘못된 방을 찾은겁니다. 시간차로 나올수 있습니다. 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return ;
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		break; 
	default: return;	break; 
	}

	if( m_pRoomMode->MissionBombInstall( i32SlotIdx, ui8BombPosition, pr32BombVecPos, GetRoomBattleTime() ) )
	{		
		MedalMissionCheck( i32ThreadIdx, GetSession(i32SlotIdx), MEDAL_MC_ACTION_ID_C4_PLANT );
		SetUserMissionCard( GetSession(i32SlotIdx), ACTION_ID_C4_PLANT );		
	}
}

void CRoom::MissionBombUnInstall(INT32 i32ThreadIdx, CUserSession * pMainSession, INT32 i32SlotIdx)
{
	if( MISSION_STATE_BATTLE	!= m_Info._i32MiStateType )
	{
		// 튜토리얼은 예외처리
		if( !IsTutorialMode() )
			return;
	}
	if( ( i32SlotIdx < 0 ) || ( SLOT_MAX_COUNT <= i32SlotIdx ) ) return;
	if( SLOT_STATE_BATTLE		!= m_Info._aSlotInfo[i32SlotIdx]._State )return; 

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return; //잘못된 방을 찾은겁니다. 시간차로 나올수 있습니다. 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return ;
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		break; 
	default: return; break; 
	}

	if( m_pRoomMode->MissionBombUnInstall( i32SlotIdx ) )
	{
		MedalMissionCheck( i32ThreadIdx, GetSession(i32SlotIdx), MEDAL_MC_ACTION_ID_C4_DEFUSE );
		SetUserMissionCard( GetSession(i32SlotIdx), ACTION_ID_C4_DEFUSE );
	}
}

void CRoom::MissionObjectDestroy(INT32 i32ThreadIdx,CUserSession * pMainSession, DESTRUCTION_INFO * pInfo)
{
	if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )return;

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return; //잘못된 방을 찾은겁니다. 시간차로 나올수 있습니다. 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return ;
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		break; 
	default: return; break; 
	}

	if( m_pRoomMode->MissionObjectDestroy( pInfo ) )
	{
		MedalMissionCheck( i32ThreadIdx, pMainSession, MEDAL_MC_ACTION_ID_DESTROY );
		SetUserMissionCard( pMainSession, ACTION_ID_DESTROY );
	}

	return; 
}

void CRoom::MissionObjectDefence( CUserSession * pMainSession, DEFENCE_INFO * pInfo )
{
	if( MISSION_STATE_BATTLE	!= m_Info._i32MiStateType )		return;		//배틀중이 아님 

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return; //잘못된 방을 찾은겁니다. 시간차로 나올수 있습니다. 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return ;
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		break; 
	default: return; break; 
	}

	m_pRoomMode->MissionObjectDefence( pInfo );

	return; 
}

void CRoom::BattleSlotEquipment( CUserSession* pSession, UINT8 ui8SlotIdx, UINT16 ui16ChangeFlag, UINT32 ui32EquipItem )
{
	GC_PROTOCOL_SLOT_EQUIPMENT Struct;
	Struct.m_i32SlotIdx					= ui8SlotIdx;
	Struct.m_ui16ChangeFlag				= ui16ChangeFlag;

	for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT; ++i )
	{
		Struct.m_EquipmentWeapon.m_TItemID[i] = pSession->m_aTBattleEquipWeapon[i].m_TItemID;
	}
	// vv3 - check
	for( INT32 k = 0 ; k < CHAR_EQUIPMENT_PARTS_COUNT; ++k )
	{
		Struct.m_EquipmentChara.SetEquipNum( (CHAR_EQUIPMENT_PARTS)k, pSession->m_aTBattleEquipParts[k].m_TItemID );
	}

	i3mem::Copy( Struct.m_aui8WeaponEndurance,	pSession->m_aUseWeaponEndurance,		sizeof(UINT8)*CHAR_EQUIPMENT_WEAPON_COUNT );
	i3mem::Copy( Struct.m_aui8EquipUserTitle,	pSession->m_aui8EquipUserTitle,			sizeof(UINT8) * MAX_EQUIP_USER_TITLE );
	Struct.m_EquipItem					= ui32EquipItem;

	i3NetworkPacket Packet( PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK  );
	Packet.WriteData( &Struct, sizeof(GC_PROTOCOL_SLOT_EQUIPMENT) );
	_SendPacketBattlePlayer( &Packet );
}

INT32 CRoom::GMPause( CUserSession* pSession )
{
	if ( m_bGMPause )										return EVENT_ERROR_GM_PAUSE_FAIL_REASON_ALREADY_PAUSE;
	if ( !pSession->CheckAuthLevel( AUTH_LEVEL_GM ) )		return EVENT_ERROR_GM_PAUSE_FAIL_REASON_U_R_NOT_GM;
	if ( i3ThreadTimer::GetServerTime() < m_ui32ATPauseBegin + NEXT_GM_PAUSE_TIME )return EVENT_ERROR_GM_PAUSE_FAIL_REASON_TIME_LIMIT;

	m_i32PauseUserIdx = pSession->m_i32SlotIdx;
	g_pModuleCast->BattleGMPause( pSession->m_WorkThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, true );

	return EVENT_ERROR_SUCCESS;
}

INT32 CRoom::GMResume( INT32 i32WorkIdx, INT32 i32SlotIdx )
{
	if ( !m_bGMPause )										return EVENT_ERROR_GM_RESUME_FAIL_REASON_NOT_PAUSE;
	if ( m_i32PauseUserIdx != i32SlotIdx )					return EVENT_ERROR_GM_RESUME_FAIL_REASON_U_R_NOT_LOCKER;

	m_bGMPause = FALSE;
	g_pModuleCast->BattleGMPause( i32WorkIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, false );

	return EVENT_ERROR_SUCCESS;
}

void CRoom::MissionTouchDown( CUserSession * pSession )
{
	if( m_pRoomMode->MissionTouchDown( pSession, pSession->m_i32SlotIdx, GetRoomBattleTime() ) )
	{
		UINT8 ui8TouchdownChainCount = m_pRoomMode->GetTouchDownChainCount( pSession->m_i32SlotIdx );
		switch( ui8TouchdownChainCount )
		{
		case 4:
			{
				MedalMissionCheck(MODULE_CAST_THREAD_IDX, pSession, MEDAL_MC_ACTION_ID_TOUCHDOWN_GAME_MAKER );
				SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN_GAME_MAKER);
			}
			break;
		case 3:
			{
				MedalMissionCheck(MODULE_CAST_THREAD_IDX, pSession, MEDAL_MC_ACTION_ID_TOUCHDOWN_HATTRICK );
				SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN_HATTRICK);
			}
			break;
		case 2:
			{
				MedalMissionCheck(MODULE_CAST_THREAD_IDX, pSession, MEDAL_MC_ACTION_ID_TOUCHDOWN_ACE_ATTACKER );
				SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN_ACE_ATTACKER);
			}
			break;
		case 1:
			{
				MedalMissionCheck(MODULE_CAST_THREAD_IDX, pSession, MEDAL_MC_ACTION_ID_TOUCHDOWN );
				SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN);
			}
			break;
		}
	}
}

void CRoom::MissionDeathBlow( CUserSession * pSession, DEATHBLOW_INFO * pInfo )
{
	MedalMissionCheck( pSession->m_WorkThreadIdx, pSession, MEDAL_MC_ACTION_ID_DEATHBLOW );
	SetUserMissionCard( pSession, ACTION_ID_DEATHBLOW, pInfo);
}

void CRoom::RequestTeamChange(CUserSession * pSession)
{
	if (pSession->m_i32SlotIdx != m_Info._i32MainSlotIdx)	return;	// 방장이 아님
	if (ROOM_STATE_READY != m_Info._InfoBasic._State)		return; // 대기상태 아님

	UINT8					ui8MoveCount = 0;
	ROOM_SLOT_CHANGE_INFO	pChangeInfo[ SLOT_MAX_COUNT ];

	// 각 팀 슬롯을 교환한다.
	m_csSession.Enter();
	{
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i += 2)
		{
			// Swap session
			CUserSession* pTempSession;
			pTempSession			= m_Info._pSession[i];
			m_Info._pSession[i]		= m_Info._pSession[i+1];
			m_Info._pSession[i+1]	= pTempSession;

			swap( m_stRoomUserInfo[i], m_stRoomUserInfo[i+1] );

			if (SLOT_STATE_CLOSE < m_Info._aSlotInfo[i+1]._State && m_Info._pSession[i+1] != NULL)
			{
				m_Info._pSession[i]->m_i32SlotIdx	= i;
			}

			if (SLOT_STATE_CLOSE < m_Info._aSlotInfo[i]._State && m_Info._pSession[i] != NULL)
			{
				m_Info._pSession[i+1]->m_i32SlotIdx = i + 1;
			}

			// Swap slotInfo
			SLOT_INFO tempSlotInfo;
			i3mem::Copy( &tempSlotInfo,				&m_Info._aSlotInfo[i],		sizeof(SLOT_INFO));
			i3mem::Copy( &m_Info._aSlotInfo[i],		&m_Info._aSlotInfo[i + 1],	sizeof(SLOT_INFO));
			i3mem::Copy( &m_Info._aSlotInfo[i + 1],	&tempSlotInfo,				sizeof(SLOT_INFO));

			// 레디 풀기
			if (SLOT_STATE_READY == m_Info._aSlotInfo[i]._State)			m_Info._aSlotInfo[i]._State		= SLOT_STATE_NORMAL;
			if (SLOT_STATE_READY == m_Info._aSlotInfo[i + 1]._State)		m_Info._aSlotInfo[i + 1]._State	= SLOT_STATE_NORMAL;

			pChangeInfo[ ui8MoveCount ].ui8OldSlot		= (UINT8)i;
			pChangeInfo[ ui8MoveCount ].ui8NewSlot		= (UINT8)(i+1);
			pChangeInfo[ ui8MoveCount ].ui8OldSlotState	= m_Info._aSlotInfo[i]._State;	// 이곳에 내려왔을때는 이미 변경된 상태이므로 바뀐 슬롯의 상태를 보내야합니다.
			pChangeInfo[ ui8MoveCount ].ui8NewSlotState	= m_Info._aSlotInfo[i+1]._State;
			ui8MoveCount++;
		}

		// 방장 인덱스를 마주보는 슬롯으로 이동
		if (0 == m_Info._i32MainSlotIdx % 2)
		{
			m_Info._i32MainSlotIdx++;
		}
		else
		{
			m_Info._i32MainSlotIdx--;
		}
	}
	m_csSession.Leave(); 

	// 팀 슬롯 변경
	switch( m_ui8ChannelType )
	{
	default: 
		break;
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN: 
		{
			INT32 i32Temp = m_Info._aui32BattleClanIdx[0];
			m_Info._aui32BattleClanIdx[0] = m_Info._aui32BattleClanIdx[1];
			m_Info._aui32BattleClanIdx[1] = i32Temp;
 
			T_TeamID TTempID = m_Info.m_TMatchTeamID[0];
			m_Info.m_TMatchTeamID[0] = m_Info.m_TMatchTeamID[1];
			m_Info.m_TMatchTeamID[1] = TTempID;
		}
		break;
	}
	// 팀 슬롯 변경

	UINT8 ui8MoveReason = 2;
	UINT8 ui8MainSlot	= (UINT8)m_Info._i32MainSlotIdx;

	i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
	Packet.WriteData( &ui8MoveReason,	sizeof(UINT8)  );
	Packet.WriteData( &ui8MainSlot,		sizeof(UINT8)  );
	Packet.WriteData( &ui8MoveCount,	sizeof(UINT8)  );
	Packet.WriteData( pChangeInfo,		sizeof(ROOM_SLOT_CHANGE_INFO)*ui8MoveCount );
	_SendPacketAllPlayer( &Packet );


	//// Client에 변경사항 통보
	//_SendSlotInfo( NULL );

	//for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	//{
	//	if (SLOT_STATE_CLOSE < m_Info._SlotInfo[i]._State && m_Info._pSession[i] != NULL)
	//	{
	//		_SendNickName( NULL, i, TRUE);

	//		UINT8 doTeamBalance = 2;

	//		i3NetworkPacket packet(PROTOCOL_ROOM_CHANGE_TEAM_ACK);
	//		packet.WriteData(&i, sizeof(INT32));
	//		packet.WriteData(&doTeamBalance, sizeof(UINT8));
	//		m_Info._pSession[i]->SendPacketMessage(&packet);
	//	}
	//}
}

void CRoom::DestroyRoom(void)
{
	i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY); 
	_SendPacketAllPlayer(&SendPacket); 
}

void CRoom::SendNoticeMessage( char * strMessage, INT32 i32len )
{
	INT32 Type = NOTICE_TYPE_EMERGENCY; 
	i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_ANNOUNCE );
	SendPacket.WriteData(&Type,			sizeof(INT32));
	SendPacket.WriteData(&i32len,		sizeof(UINT32));
	SendPacket.WriteData(strMessage,	(UINT16)i32len);
	_SendPacketAllPlayer(&SendPacket);
	return; 
}

INT32 CRoom::CheckMainChage(CUserSession * pSession)
{		
	if(m_Info._InfoBasic._State != ROOM_STATE_READY)		return EVENT_ERROR_FAIL;
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)	return EVENT_ERROR_FAIL;

	m_csSession.Enter();
	INT32 i32Rv = _CheckMainBest( SLOT_STATE_CLOSE );
	m_csSession.Leave();
	return i32Rv;
}

void CRoom::RequestMain(CUserSession * pSession)
{	
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;

	if( ROOM_STATE_READY != m_Info._InfoBasic._State )	return; 
	if( m_stRoomUserInfo[i32SlotIdx]._bChangeMain )		return;

	INT32 i32Rv = EVENT_ERROR_FAIL;
	m_csSession.Enter();
	{
		m_stRoomUserInfo[i32SlotIdx]._bChangeMain = TRUE;

		INT32 i32Count = 0;
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			if( false == m_stRoomUserInfo[i]._bChangeMain )	continue;
			i32Count++;
		}

		// 기획 의도: 방장을 제외하고 모든 유저가 방장 검색을 요청해야 방장이 변경 된다.
		if ( (m_Info._InfoBasic._PersonNow - 1) <= i32Count )	
		{
			i32Rv = _ChangeMain( SLOT_STATE_CLOSE );

			if( EV_SUCCESSED( i32Rv ) )
			{
				m_stRoomUserInfo[m_Info._i32MainSlotIdx]._bAlreadyMain = TRUE;
			}
		}
	}
	m_csSession.Leave();

	if( EV_SUCCESSED( i32Rv ) )
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			m_stRoomUserInfo[i]._bChangeMain = FALSE;
		}

		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK );
		Packet.WriteData(&m_Info._i32MainSlotIdx, sizeof(INT32)); 
		_SendPacketAllPlayer(&Packet); 	
		_SendSlotInfo( NULL );
	}
	else
	{
		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_ACK );
		Packet.WriteData(&i32SlotIdx, sizeof(INT32));
		//모든 유저에게 패킷을 보낸다. 
		_SendPacketAllPlayer(&Packet); 
	}

	return; 
}

void CRoom::RequestMainChange(CUserSession * pSession)
{	
	if(m_Info._InfoBasic._State	!= ROOM_STATE_READY)		return;			// 잘못된 패킷
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)	return;			// 방장인지 확인

	INT32 i32Rv;
	m_csSession.Enter();
	{
		i32Rv = _ChangeMain( SLOT_STATE_CLOSE );

		if( EV_SUCCESSED( i32Rv ) )
		{	// Rv는 무조건 정상값이다. 따로 확인 필요없다.
			m_stRoomUserInfo[m_Info._i32MainSlotIdx]._bAlreadyMain = TRUE;
		}
	}
	m_csSession.Leave();

	if( EV_SUCCESSED( i32Rv ) )
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			m_stRoomUserInfo[i]._bChangeMain = FALSE;
		}

		//모든 유저에게 메시지를 보냄 
		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK );
		Packet.WriteData(&m_Info._i32MainSlotIdx, sizeof(INT32)); 
		_SendPacketAllPlayer(&Packet);				// 방장이 바뀌었다는 통보
		_SendSlotInfo( NULL );						// slot정보 변경
	}
	else
	{	//방장에게 실패 통보
		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_ACK );
		Packet.WriteData(&i32Rv, sizeof(INT32)); 
		m_Info._pSession[m_Info._i32MainSlotIdx]->SendPacketMessage(&Packet);
	}

	return; 
}

void CRoom::RequestMainChangeSlot(CUserSession * pSession, UINT32 ui32SlotIdx)
{
	if(m_Info._InfoBasic._State != ROOM_STATE_READY)		return;
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)	return;
	if(ui32SlotIdx >= SLOT_MAX_COUNT)						return;

	BOOL bRv = TRUE;
	m_csSession.Enter(); 
	{
		if (SLOT_STATE_CLOSE < m_Info._aSlotInfo[ui32SlotIdx]._State)
		{
			if (SLOT_STATE_READY == m_Info._aSlotInfo[ui32SlotIdx]._State)	m_Info._aSlotInfo[ui32SlotIdx]._State = SLOT_STATE_NORMAL;
			m_Info._i32MainSlotIdx = ui32SlotIdx;
			// 방장을 한번 했습니다.
			m_stRoomUserInfo[ m_Info._i32MainSlotIdx ]._bAlreadyMain = TRUE;

			_SetMainSlot();
		}
		else
		{
			bRv = FALSE; 
		}
	}
	m_csSession.Leave(); 

	//패킷 보내기
	if(bRv)
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			m_stRoomUserInfo[i]._bChangeMain = FALSE;
		}

		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK );
		Packet.WriteData(&m_Info._i32MainSlotIdx, sizeof(INT32)); 
		_SendPacketAllPlayer(&Packet); 	
		_SendSlotInfo( NULL );
	}
	else
	{
		UINT32 ui32Temp = (UINT32)EVENT_ERROR_FAIL; 
		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK );
		Packet.WriteData(&ui32Temp, sizeof(INT32)); 
		m_Info._pSession[m_Info._i32MainSlotIdx]->SendPacketMessage(&Packet);
	}

	return ; 
}

INT32 CRoom::_CheckMainBest( SLOT_STATE eSlotState )
{
	INT32	i32MainSlot		= SERVER_VALUE_UNKNOWN;
	
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( m_Info._i32MainSlotIdx == i )				continue;	// 현재 방장을 제외한다.
		if( m_Info._aSlotInfo[i]._State <= eSlotState )	continue;	// 슬롯이 닫혀있다.
		if( m_Info._pSession[i] == NULL )				continue;	// Session 이 NULL 이다.

		i32MainSlot = i;
		break;
	}

	return i32MainSlot;
}

void CRoom::SetBattleGroupInfo( P_BATTLE_ROOM_CREATE_ACK * pAckInfo )
{	
	if( !EV_SUCCESSED( pAckInfo->m_i32Result ) )
	{
		ResetBattleGroupInfo();
		return;
	}

	//Set Value 
	m_bIsBattleRoomCreated	= true;
	m_ui8BattleType			= pAckInfo->_BattleType; 
	m_ui32BattleRoomIdx		= pAckInfo->_GroupIdx;
	m_ui32BattleEncryptKey	= pAckInfo->_Key;
	m_BattleRoomInfo.Copy( &pAckInfo->_RoomInfo );
	m_ui32BattleServerIP	= pAckInfo->_Ip;
	m_ui16BattleServerPort	= pAckInfo->_Port;

	//패킷을 보내야 한다. 
	return; 
}

void CRoom::ResetBattleGroupInfo(void)
{
	m_bIsBattleRoomCreated	= false;
	m_ui8BattleType			= BATTLE_SERVER_TYPE_NONE; 

	return; 
}

void CRoom::ChangeDifficultyLevel(CUserSession * pSession)
{
	if ( !IsChallengeMode() )
	{
		// 챌린지모드 전용
		I3TRACE("[WARNING] CRoom::ChangeDifficultyLevel() - Not challenge mode call");
		return;
	}

	if (m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)
	{
		// 방장전용
		I3TRACE("[WARNING] CRoom::ChangeDifficultyLevel() - Not host call");
		return;
	}

	// 난이도 상승
	m_Info.m_i8currentDifficultyLevel = MIN( m_Info.m_i8currentDifficultyLevel + 1, CHALLENGE_DIFFICULTY_LEVEL_HIGH );

	// 전송
	i3NetworkPacket packet(PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK);
	packet.WriteData(&m_Info.m_i8currentDifficultyLevel, sizeof(INT8));
	_SendPacketBattlePlayer(&packet);
}

//static int compare( UINT32* arg1,  UINT32* arg2 )
//{
//	return *arg1 - *arg2;
//}

void CRoom::SendAnnounceMessage(i3NetworkPacket *pPacket)
{
	_SendPacketAllPlayer( pPacket );
}

// 리스폰 해킹 방지
BOOL CRoom::CheckRespawnHacking( CUserSession* pSession )
{
	// 데디케이션에서 메시지가 늦게 오는 문제가 있어서 데디케이션에서는 막습니다.
	if( isDedicationMode() )		return TRUE;

	// 튜토리얼 일 경우
	if ( IsTutorialMode() || IsChallengeMode())
		 return TRUE;

	// 리스폰 가능한 시간 = 죽은 시간 + 리스폰 시간 
	UINT32 ui32PossibleRespawnTime = pSession->m_ui32RTBattleDeath + pSession->m_ui32RespawnDelay ;

	// 현재 시간( 리스폰 요청 패킷 보낸시간) 이 리스폰 가능 시간 보다 크다면 정상.
	if( i3ThreadTimer::GetServerTime() >= ui32PossibleRespawnTime )				return TRUE;

//#ifdef I3_DEBUG
	g_pLog->WriteLogSession( pSession, L"[CheckRespawnHacking()] %I64d, DeathTime : %d / RespawnTime : %d / NowTime : %d / nick : %s", 
		pSession->m_i64UID, pSession->m_ui32RTBattleDeath,  pSession->m_ui32RespawnDelay, i3ThreadTimer::GetServerTime() ,  pSession->m_UserInfoBasic.m_strNick );
//#endif

	// 현재 시간( 리스폰 요청 패킷 보낸시간) 이 리스폰 가능 시간 보다 작다면 리스폰 해킹
//	return FALSE;
	return TRUE;
}

void CRoom::_CheckAbusing( INT32 i32SlotIdx, UINT32 ui32PlayTime )
{
	if( FALSE == g_pContextMain->m_bAbusingCheck )	return;	// 어뷰징 체크가 꺼져있음
	if( 1 > ui32PlayTime )							return; // 총 게임시간이 0이하면 어뷰징 체크를 하지 않습니다.

	// 어뷰징 체크 및 에뷰징 레벨 설정
	UINT32	ui32PopupTime		= 0;
	UINT8	ui8AbusingLevel		= ABUSING_LEVEL_NONE; 
	
	UINT32	ui32ExpPerSec = m_Info._aui16AccExp[i32SlotIdx] * 100 / ui32PlayTime ;	// 초당 획득 EXP ( REAL32 로 계산하지 않기위해 100을 곱합니다. )

	if( ui32ExpPerSec >= CHECK_NORMAL_ABUSING_EXP )
	{
		m_Info._pSession[i32SlotIdx]->m_UserInfoBasic.m_ui32AbusingCount++; // 어뷰징 카운트 증가
		m_Info._pSession[i32SlotIdx]->ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO );

		ui8AbusingLevel = ABUSING_LEVEL_NORMAL; 	

		UINT32	ui32PopupTimeBase	= ( ui32ExpPerSec - CHECK_NORMAL_ABUSING_EXP ) * ui32PlayTime ;

		INT8	i8AbusingCheck = -1;	// 배수

		// 일반 어뷰저 팝업 시간 계산
		if		( ui32PlayTime < ABUSING_CHECK_COMPARE_TIME_1 )	i8AbusingCheck = ABUSING_CHECK_NORMAL_1 ;
		else if	( ui32PlayTime < ABUSING_CHECK_COMPARE_TIME_2 )	i8AbusingCheck = ABUSING_CHECK_NORMAL_2 ;
		else if	( ui32PlayTime < ABUSING_CHECK_COMPARE_TIME_3 )	i8AbusingCheck = ABUSING_CHECK_NORMAL_3 ;
		else													i8AbusingCheck = ABUSING_CHECK_NORMAL_4 ;
		if( i8AbusingCheck > -1 ) ui32PopupTime = ui32PopupTimeBase / g_ui32AbusingCalValue[i8AbusingCheck];

		// 팝업 시간이 600초 초과 또는 초당 EXP 획득량이 6.1초과 또는 이전에 한번이라도 어뷰징 한적이 있으면 강화 제재 유저로 분류.
		if( ui32PopupTime > NORMAL_ABUSING_MAX_POPUP_TIME	|| 
			CHECK_STRONG_ABUSING_EXP <= ui32ExpPerSec		|| 
			m_Info._pSession[i32SlotIdx]->m_UserInfoBasic.m_ui32AbusingCount > 1 ) 
		{
			ui8AbusingLevel = ABUSING_LEVEL_STRONG; 

			// 강화 어뷰저는 팝업 시간 계산
			if		( ui32PlayTime < ABUSING_CHECK_COMPARE_TIME_1 )	i8AbusingCheck = ABUSING_CHECK_STRONG_1 ;
			else if	( ui32PlayTime < ABUSING_CHECK_COMPARE_TIME_2 )	i8AbusingCheck = ABUSING_CHECK_STRONG_2 ;
			else if	( ui32PlayTime < ABUSING_CHECK_COMPARE_TIME_3 )	i8AbusingCheck = ABUSING_CHECK_STRONG_3 ;
			else													i8AbusingCheck = ABUSING_CHECK_STRONG_4 ;
			if( i8AbusingCheck > -1 ) ui32PopupTime = ui32PopupTimeBase / g_ui32AbusingCalValue[i8AbusingCheck];

			// 팝업 시간이 900초 초과시 초강화 유저로 분류.
			if ( ui32PopupTime > STRONG_ABUSING_MAX_POPUP_TIME )	ui8AbusingLevel = ABUSING_LEVEL_STRONGEST; 
		}		
	}

	m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel			= ui8AbusingLevel; 
	m_Info._pSession[i32SlotIdx]->m_ui32AbusingPopupTime	= ui32PopupTime;
}

//void CRoom::_AbusingPenalty( INT32 i32SlotIdx, UINT32 ui32PlayTime )
//{
//	if( 1 > ui32PlayTime )	return; // 총 게임시간이 0이하면 어뷰징 체크를 하지 않습니다.
//
//	// 어뷰징 레벨별 페널티 적용
//	switch( m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel )
//	{
//	case ABUSING_LEVEL_NONE :		// 페널티 X
//		break;
//	case ABUSING_LEVEL_NORMAL :	 
//	case ABUSING_LEVEL_STRONG :	 
//		{
//			// 일반 제재 유저는 팝업 시간이 최소 팝업 시간 미만이면 최소 팝업 시간으로 고정
//			if ( ABUSING_LEVEL_NORMAL == m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel && m_Info._pSession[i32SlotIdx]->m_ui32AbusingPopupTime < NORMAL_ABUSING_MIN_POPUP_TIME ) 
//			{
//				m_Info._pSession[i32SlotIdx]->m_ui32AbusingPopupTime = NORMAL_ABUSING_MIN_POPUP_TIME;
//			}
//
//			// 획득한 EXP, POINT를 보류한다.
//			m_Info._pSession[i32SlotIdx]->m_ui32HoldAccExp		= m_Info._ui16AccExp[i32SlotIdx] + m_Info._ui16EventExp[i32SlotIdx];
//			m_Info._pSession[i32SlotIdx]->m_ui32HoldAccPoint	= m_Info._ui16AccPoint[i32SlotIdx] + m_Info._ui16EventPoint[i32SlotIdx];
//			m_Info._ui16AccExp[i32SlotIdx]		= 0;	
//			m_Info._ui16AccPoint[i32SlotIdx]	= 0;
//			m_Info._ui16EventExp[i32SlotIdx]	= 0;
//			m_Info._ui16EventPoint[i32SlotIdx]	= 0;
//		}
//		break;
//	case ABUSING_LEVEL_STRONGEST :	// EXP,POINT 지급 X
//		{
//			m_Info._ui16AccExp[i32SlotIdx]		= 0;	
//			m_Info._ui16AccPoint[i32SlotIdx]	= 0;
//			m_Info._ui16EventExp[i32SlotIdx]	= 0;
//			m_Info._ui16EventPoint[i32SlotIdx]	= 0;
//		}
//		break;
//	}
//
//	// 어뷰징 체크 로그저장 - 어뷰징 체크된 유저에 한해서.  
//	if( ABUSING_LEVEL_NONE < m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel )
//	{ 
//		g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_R_ABUSER_CHECK, m_Info._pSession[i32SlotIdx], this, &i32SlotIdx );
//	}
//}

void CRoom::MissionTutorialRoundEnd( CUserSession* pSession )
{
	if( !IsTutorialMode() )														return;
	if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )						return;
	if( SLOT_STATE_BATTLE != m_Info._aSlotInfo[pSession->m_i32SlotIdx]._State )	return;

	if( ! m_pRoomMode->MissionTutorialRoundEnd( pSession ) )
	{
		// 
	}
}

INT32 CRoom::SetRoomMode( UINT32 ui32StageID )
{
	switch(StageID::GetStageMode( ui32StageID ))
	{
	case STAGE_MODE_DEATHMATCH		:
		m_pRoomMode = m_pDeathmatch;	
		break; 
	case STAGE_MODE_ANNIHILATION	: 
		m_pRoomMode = m_pAnnihilation;
		break; 
	case STAGE_MODE_BOMB			: 	
		m_pRoomMode = m_pBomb;
		break; 
	case STAGE_MODE_DESTROY			:
		m_pRoomMode = m_pDestroy;
		break; 
	case STAGE_MODE_DEFENCE			:
		m_pRoomMode = m_pDefence;
		break;
	case STAGE_MODE_ESCAPE			: 
		m_pRoomMode = m_pEscape;
		break; 	
	case STAGE_MODE_TUTORIAL		:
		m_pRoomMode = m_pTutorial;
		break;
	case STAGE_MODE_CROSSCOUNT		:
		m_pRoomMode = m_pCrossCount;
		break;
	case STAGE_MODE_CONVOY			:
		m_pRoomMode = m_pConvoy;
		break;
	case STAGE_MODE_RUN_AWAY		:
		m_pRoomMode = m_pRunAway;
		break;
	default						:// 잘못된 정보
		return EVENT_ERROR_ROOM_NO_AUTHORITY;
	}

	// Space Mode 이면 Space로 변경합니다.
	if (StageID::IsSpaceMode(ui32StageID) == true)
	{
		m_pRoomMode = m_pSpace;
	}

	//AI를 사용중인경우 챌린지로 설정한다.
	if( StageID::GetAIMode(ui32StageID) != AI_USE_NO )
	{
		m_pRoomMode = m_pChallenge;
	}

	return EVENT_ERROR_SUCCESS;
}

UINT32	CRoom::WELL512_Random(UINT32 ui32State[], UINT8& ui8Index)
{
	UINT32 a, b, c, d;

	a = ui32State[ui8Index];
	c = ui32State[(ui8Index + 13) & 15];
	b = a ^ c ^ (a << 16) ^ (c << 15);
	c = ui32State[(ui8Index + 9) & 15];
	c ^= (c >> 11);
	a = ui32State[ui8Index] = b ^ c;
	d = a ^ ((a << 5) & 0xda442d24U);
	ui8Index = (ui8Index + 15) & 15;
	a = ui32State[ui8Index];
	ui32State[ui8Index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);

	return ui32State[ui8Index];
}

void	CRoom::SendNotifyHackUser( INT32 i32SlotIdx, UINT8 ui8Type, INT32 i32ThreadIdx, UINT32 ui32HackType, UINT8 ui8LocaleCode )
{
	if( 0 > i32SlotIdx || i32SlotIdx >= SLOT_MAX_COUNT )		return;		//슬롯 인덱스가 정확한지.

	if( LOCALE_CODE_INDONESIA == ui8LocaleCode )
	{
		switch( ui8Type )
		{
		case	HACK_CHECK_TYPE_CLIENT		: 
			{
				if( m_Info._aSlotInfo[i32SlotIdx]._State > SLOT_STATE_READY )
				{
					Chatting_U( i32ThreadIdx, m_Info._pSession[i32SlotIdx], L"User tersebut diusir dari game karena menggunakan Program illegal.", CHATTING_TYPE_ALL );

					m_Info._pSession[i32SlotIdx]->_SendErrorMessage( i32ThreadIdx, EVENT_ERROR_HACKING_EXIT_USER, 0, KILL_REASON_HACK_PACKET );
				}
			}
			break; 
		case	HACK_CHECK_TYPE_SERVER_G	: 
		case	HACK_CHECK_TYPE_SERVER_D	: 
		default								: 
			{
				i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_NOTIFY_HACK_USER_ACK );
				SendPacket.WriteData( &i32SlotIdx,	sizeof(INT32) );	// 해킹 유저의 슬롯 인덱스
				SendPacket.WriteData( &ui8Type,		sizeof(UINT8)); 
				SendPacket.WriteData( &ui32HackType,sizeof(UINT32)); 

				for( INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
				{
					if( m_Info._aSlotInfo[i]._State > SLOT_STATE_READY )
					{
						if( NULL == m_Info._pSession[i] ) continue;
						m_Info._pSession[i]->SendPacketMessage(&SendPacket);
					}
				}
			}
			break; 
		}
	}
	else
	{

		i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_NOTIFY_HACK_USER_ACK );
		SendPacket.WriteData( &i32SlotIdx,	sizeof(INT32) );	// 해킹 유저의 슬롯 인덱스
		SendPacket.WriteData( &ui8Type,		sizeof(UINT8)); 
		SendPacket.WriteData( &ui32HackType,sizeof(UINT32)); 

		for( INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
		{
			if( m_Info._aSlotInfo[i]._State > SLOT_STATE_READY )
			{
				if( NULL == m_Info._pSession[i] ) continue;
				m_Info._pSession[i]->SendPacketMessage(&SendPacket);
			}
		}
	}
	return; 
}

void CRoom::SendRespawnToDedi( INT32 i32ThreadIdx, INT32 i32SlotIdx, UINT32 * pui32WeaponList, INT32 i32RespawnIdx, INT32 i32RespawnCount )
{
	if ( !isDedicationMode() ) return;

	g_pModuleCast->BattleRespawn( i32ThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT8)i32SlotIdx, pui32WeaponList, i32RespawnIdx, i32RespawnCount );
}


void	CRoom::MedalMissionCheck(  INT32 i32ThreadIdx, CUserSession * pUser, MEDAL_MC_ACTION_ID eActionID , UINT32 ui32UseItemID, CUserSession * pTargetUser ) 
{
	// 메달 시스템 비활성화시 강제 리턴
	if( !g_pContextMain->IsContentsEnabled( CONTENTS_MEDAL ) )	 return;

	// 챌린지모드, 튜토리얼 처리 안함
	if(	IsTutorialMode() || IsChallengeMode() ) return;

	// 나오면 안됨.
	if( NULL == pUser  )
	{
		I3ASSERT(FALSE);
		return;
	}

	if (pUser->m_ui8LocaleCode >= LOCALE_CODE_MAX) return;

	// AI는 건너뛴다.
	if( 1 > pUser->m_i64UID ) return;

	MEDAL_MC_CHECK_POSITION eCheckPosition = g_pContextMain->GetMMCPositionByActionID( eActionID );
	
	MMC_INFO MMCInfo;
	MQF_MEDAL_INFO* pMqfMedal;
	UINT16	ui16CompleteMedalCount = 0;
	UINT32	ui32DBSaveFlag = 0;
	MEDAL_GT* pUserMedal;

	// 미션달성(카운팅) 된 메달 정보를 담을 구조체.
	MEDAL_STATE	MedalComplete[MAX_MEDAL_COUNT];
	i3mem::FillZero( MedalComplete, sizeof( MEDAL_STATE ) * MAX_MEDAL_COUNT );

	// 1. 일반 메달.
	// 체크해야할 일반 메달 갯수. MQF 파일 기준.
	MQF_ALL_MEDAL_INFO*	pServerMQF = &g_pContextMain->m_MQFMedal[pUser->m_ui8LocaleCode];
	UINT16 ui16MedalCount = pServerMQF->m_aui16MQFMedalCount[eCheckPosition];
	
	for( UINT16 MedalArrIdx = 0; MedalArrIdx < ui16MedalCount ; MedalArrIdx++ )
	{	// 주의 : 게임서버가 로드한 메달 정보와 유저가 DB에서 로드한 메달 정보가 메달 고유 인덱스의 오름차순으로 정렬되어있다는 가정하에 진행됩니다.  
		
		pMqfMedal = &pServerMQF->m_aMedalInfo[eCheckPosition][MedalArrIdx];
		pUserMedal = &pUser->m_MedalAllInfo.m_aMedal[eCheckPosition][MedalArrIdx];

		if( NULL == pMqfMedal || NULL == pUserMedal ) // 나오면 안됨.
		{
			I3ASSERT(FALSE);
			continue;
		}

		// 액션이 다르면 continue;
		if( eActionID != pMqfMedal->m_eActionID  ) continue;	
		
		// Lv4까지 모두 달성했다면 카운팅 하지 않는다.
		if(  pUserMedal->m_Medal.m_ui16Count >= pMqfMedal->m_ui16Count[MEDAL_LEVEL_MAX-1] ) continue;

		MMCInfo.m_pMQFMedalInfo = pMqfMedal;
		MMCInfo.m_ui32UseItemID = ui32UseItemID;
		MMCInfo.m_eActionID		= eActionID;		

		if( TRUE == CheckMissionActionID_Category(&MMCInfo, pUser, pTargetUser  ) ) 
		{
			// 미션 달성
			pUserMedal->m_Medal.m_ui16Count++;

			MedalComplete[ui16CompleteMedalCount].m_ui8MedalType = MEDAL_TYPE_NORMAL;				
			MedalComplete[ui16CompleteMedalCount].m_ui16MedalIdx = pMqfMedal->m_ui16Idx ;			
			MedalComplete[ui16CompleteMedalCount].m_ui16Count	= pUserMedal->m_Medal.m_ui16Count;

			pUserMedal->m_ui8IsSave = 1;	// DB 저장 ㄱㄱ
			ui16CompleteMedalCount++;
			ui32DBSaveFlag |= SAVE_DATABASE_MEDAL;


			// 레벨 달성시 ZLog에 기록.
			LOG_MEDAL_LEVELUP_INFO LogInfo;
			LogInfo.m_ui16CompareCount	= pUserMedal->m_Medal.m_ui16Count;
			LogInfo.m_ui16MedalIdx		= pMqfMedal->m_ui16Idx;
			LogInfo.m_ui16MedalSetIdx	= 0;
			LogInfo.m_eMedalType		= MEDAL_TYPE_NORMAL;
			LogInfo.m_eMedalSetType		= MEDAL_SET_TYPE_NORMAL;
			g_pModuleContextNC->LogMedalLevelUp(i32ThreadIdx,pUser, &LogInfo );
			
		}
	}


	// 2. 진행중인 메달 셋트
	MQF_MEDAL_SET_INFO* pMQFMedalSetInfo = NULL;
	for( INT32 MedalSetArrIdx = 0; MedalSetArrIdx < MAX_CUR_MEDAL_SET_COUNT ; MedalSetArrIdx++ )
	{
		// 슬롯이 비어있으면 건너뛴다.
		if( INIT_MEDAL_NUMBER == pUser->m_MedalAllInfo.m_aCurMedalSet[MedalSetArrIdx].m_ui16Idx ||
			MEDAL_SET_TYPE_NORMAL == pUser->m_MedalAllInfo.m_aCurMedalSet[MedalSetArrIdx].m_ui8Type ) continue;

		// 메달 셋트내 활성화된 메달 갯수 검색
		CUR_MEDAL_SET* pCurUserMedalSet = &pUser->m_MedalAllInfo.m_aCurMedalSet[MedalSetArrIdx];	// 진행중인 유저의 메달 셋트
		if( NULL == pCurUserMedalSet ) continue;
	
		// 이벤트 메달 셋트는 서버에서 진행중이 아니라면 카운팅 하지 않는다.
		if( MEDAL_SET_TYPE_EVENT ==  pCurUserMedalSet->m_ui8Type && !g_pContextMain->IsEnableEventMedalSet() )  continue;

		pMQFMedalSetInfo = g_pContextMain->GetMedalSet( pCurUserMedalSet->m_ui16Idx );
		if( NULL == pMQFMedalSetInfo ) 
		{
			// 나올수 없는 상황
			I3ASSERT(FALSE);
			continue; 
		}

		for( UINT8 A_MedalIdx = 0; A_MedalIdx <  pMQFMedalSetInfo->m_ui8ActiveMedalCount ; A_MedalIdx++ )
		{
			pMqfMedal = &pMQFMedalSetInfo->m_aMedalInfo[A_MedalIdx];
			if( NULL == pMqfMedal ) // 나오면 안됨.
			{
				I3ASSERT(FALSE);
				continue;
			}

			//  액션이 다르면 continue;
			if(  eActionID != pMqfMedal->m_eActionID  ) continue;	

			// 메달 셋트는 Lv1이 한계입니다.
			// Lv1까지 모두 달성했다면 카운팅 하지 않는다. 
			if(  pCurUserMedalSet->m_ui16Count[A_MedalIdx] >= pMqfMedal->m_ui16Count[MEDAL_LEVEL_MAX_SET-1] ) continue;

			MMCInfo.m_pMQFMedalInfo = pMqfMedal;
			MMCInfo.m_ui32UseItemID = ui32UseItemID;
			MMCInfo.m_eActionID		= eActionID;	

			if( TRUE == CheckMissionActionID_Category(&MMCInfo,  pUser, pTargetUser ) ) 
			{
				pCurUserMedalSet->m_ui16Count[A_MedalIdx]++;	// 유저가 소지한 메달의 카운트 증가

				MedalComplete[ui16CompleteMedalCount].m_ui8MedalType = MEDAL_TYPE_SET;				
				MedalComplete[ui16CompleteMedalCount].m_ui16MedalIdx = pMqfMedal->m_ui16Idx ;			
				MedalComplete[ui16CompleteMedalCount].m_ui16Count = pCurUserMedalSet->m_ui16Count[A_MedalIdx];

				ui16CompleteMedalCount++;
				ui32DBSaveFlag |= SAVE_DATABASE_CUR_MEDAL_SET;

				// 레벨 달성시 ZLog에 기록.
				LOG_MEDAL_LEVELUP_INFO LogInfo;
				LogInfo.m_ui16CompareCount	= pCurUserMedalSet->m_ui16Count[A_MedalIdx];
				LogInfo.m_ui16MedalIdx		= pMqfMedal->m_ui16Idx;
				LogInfo.m_ui16MedalSetIdx	= pCurUserMedalSet->m_ui16Idx;
				LogInfo.m_eMedalType		= MEDAL_TYPE_SET;
				LogInfo.m_eMedalSetType		= (MEDAL_SET_TYPE)pCurUserMedalSet->m_ui8Type;
				g_pModuleContextNC->LogMedalLevelUp(i32ThreadIdx,pUser, &LogInfo );
			}
		}
	}

	// 미션 달성.
	if( ui16CompleteMedalCount > 0 )
	{
		// 미션 달성( +1 ) 내용을 해당 유저에게 보낸rr다.
		i3NetworkPacket Packet( PROTOCOL_MISSION_COMPLETE_ACK );
		PACKET_H_MISSION_COMPLETE_ACK		PacketAck;
		
		PacketAck.m_ui16CompleteMedalCount = ui16CompleteMedalCount;
		Packet.WriteData( &PacketAck,		sizeof(PACKET_H_MISSION_COMPLETE_ACK) );
		Packet.WriteData( MedalComplete,	sizeof(MEDAL_STATE) * ui16CompleteMedalCount);
		pUser->SendPacketMessage( &Packet );

		pUser->ChangeDataBaseInfo( ui32DBSaveFlag );
	}
	
	return;
}

BOOL CRoom::CheckMissionActionID_Category( MMC_INFO* pMMCInfo, CUserSession * pUser, CUserSession * pTargetUser ) 
{
	if( NULL == pMMCInfo )  // 나오면 안됨.
	{
		I3ASSERT(FALSE);
		return FALSE;
	}

	UINT32 ui32CategoryFlag = 0;
	// 액션 아이디별로 체크 항목 설정.
	switch( pMMCInfo->m_eActionID)
	{	
	case MEDAL_MC_ACTION_ID_KILL:						
		ui32CategoryFlag |= ( MCC_FLAG_TEAM | MCC_FLAG_TARGET | MCC_FLAG_FIELD ); 
		break;
	case MEDAL_MC_ACTION_ID_C4_PLANT:					
	case MEDAL_MC_ACTION_ID_C4_DEFUSE:					
	case MEDAL_MC_ACTION_ID_STAGE_ENTER:				
	case MEDAL_MC_ACTION_ID_STAGE_INTERCEPT:			
	case MEDAL_MC_ACTION_ID_DEATH:						
	case MEDAL_MC_ACTION_ID_HEADSHOT:					
	case MEDAL_MC_ACTION_ID_DOUBLE_KILL:				
	case MEDAL_MC_ACTION_ID_PIERCING:					
	case MEDAL_MC_ACTION_ID_TRIPLE_KILL:				
	case MEDAL_MC_ACTION_ID_MASS_KILL:					
	case MEDAL_MC_ACTION_ID_DESTROY:					
	case MEDAL_MC_ACTION_ID_CHAINKILLER:				
	case MEDAL_MC_ACTION_ID_CHAINSLUGGER:				
	case MEDAL_MC_ACTION_ID_CHAINSTOPPER:				
	case MEDAL_MC_ACTION_ID_CHAINHEADSHOT:				
	case MEDAL_MC_ACTION_ID_PROTECT:					
	case MEDAL_MC_ACTION_ID_DEATHBLOW:					
	case MEDAL_MC_ACTION_ID_TOUCHDOWN:					
	case MEDAL_MC_ACTION_ID_TOUCHDOWN_ACE_ATTACKER:		
	case MEDAL_MC_ACTION_ID_TOUCHDOWN_HATTRICK:			
	case MEDAL_MC_ACTION_ID_TOUCHDOWN_GAME_MAKER:		
	case MEDAL_MC_ACTION_ID_KILL_ASSIST:				
	case MEDAL_MC_ACTION_ID_DRAW:						
	case MEDAL_MC_ACTION_ID_WIN:						
	case MEDAL_MC_ACTION_ID_DEFEAT:						
		ui32CategoryFlag |= ( MCC_FLAG_TEAM |  MCC_FLAG_FIELD ); 
		break;
	case MEDAL_MC_ACTION_ID_USE_WEAPON_KILL:			
		ui32CategoryFlag |= ( MCC_FLAG_TEAM | MCC_FLAG_ITEM | MCC_FLAG_FIELD );  
		break;
	default : 
		return FALSE; 
		break;
	}

	// 1. 팀 구분 체크.
	if(  ui32CategoryFlag & MCC_FLAG_TEAM )
	{
		switch ( pMMCInfo->m_pMQFMedalInfo->m_eTeam )
		{
		case MEDAL_MC_TEAM_RED : 
			{
				if( TEAM_RED != ( pUser->m_i32SlotIdx % 2 ) ) return FALSE;
			}
			break;
		case MEDAL_MC_TEAM_BLUE : 
			{
				if( TEAM_BLUE != ( pUser->m_i32SlotIdx % 2 ) ) return FALSE;
			}
			break;
		case MEDAL_MC_TEAM_HUMAN : 
			{
				if( m_Info._i32StageType != STAGE_MODE_ESCAPE && m_Info._i32StageType != STAGE_MODE_CROSSCOUNT ) return FALSE;
				if( TRUE == m_Info._MiDinoInfo.IsDinoIdx((UINT8)pUser->m_i32SlotIdx) )  return FALSE;
			}
			break;
		case MEDAL_MC_TEAM_DINO : 
			{
				if( m_Info._i32StageType != STAGE_MODE_ESCAPE && m_Info._i32StageType != STAGE_MODE_CROSSCOUNT ) return FALSE;
				if( FALSE == m_Info._MiDinoInfo.IsDinoIdx((UINT8)pUser->m_i32SlotIdx) )  return FALSE;
			}
			break; 
		}

	}

	// 2. 제거 대상.
	if( ui32CategoryFlag & MCC_FLAG_TARGET )
	{
		if( pTargetUser ) // QA커맨드를 사용하면 타겟이 없을수 있음. 그럴때는 랭크검사 생략
		{
			RANK_COUNT	eDeathUserRank = (RANK_COUNT)pTargetUser->m_UserInfoBasic.m_ui32Rank;

			if( 0 != pMMCInfo->m_pMQFMedalInfo->m_eTargetRankEnd && ( eDeathUserRank < pMMCInfo->m_pMQFMedalInfo->m_eTargetRankBegin ||
				eDeathUserRank >  pMMCInfo->m_pMQFMedalInfo->m_eTargetRankEnd ) ) return FALSE;
		}
	}



	// 4. 필드
	if( ui32CategoryFlag & MCC_FLAG_FIELD )
	{
		switch( pMMCInfo->m_pMQFMedalInfo->m_eField)
		{
		case MEDAL_MCF_FIELD_STAGE :
			{ 
				if( pMMCInfo->m_pMQFMedalInfo->m_eStage != m_Info._i32StageType ) return FALSE;
			}
			break;
		case MEDAL_MCF_FIELD_WEAPON :
			{
				if (pMMCInfo->m_pMQFMedalInfo->m_eWeapon != StageID::GetWeaponRestriction(m_Info._InfoBasic._StageID)) return FALSE;
			}
			break;
		case MEDAL_MCF_FIELD_MAP :
			{
				if( pMMCInfo->m_pMQFMedalInfo->m_eMap != m_StageUID ) return FALSE;
			}
			break;
		}
	}


	// 5. 난이도
	if( ui32CategoryFlag & MCC_FLAG_LEVEL )
	{
		// 모드가 챌린지모드거나 스테이지 타입이 제압일때, 현재 난이도가 미션 난이도 보다 낮으면 FALSE
		if( ( IsChallengeMode() )
			&& pMMCInfo->m_pMQFMedalInfo->m_ui8StageLevel > m_Info.m_i8difficultyLevel ) return FALSE;
	}


	// 6. 아이템 
	if( ui32CategoryFlag & MCC_FLAG_ITEM )
	{
		ITEM_TYPE eItemType = (ITEM_TYPE)GET_ITEM_TYPE( pMMCInfo->m_ui32UseItemID );
		switch( pMMCInfo->m_pMQFMedalInfo->m_eItem)
		{
		case MEDAL_MCF_ITEM_TYPE :
			{ 
				if( pMMCInfo->m_pMQFMedalInfo->m_eUseItemType != eItemType ) return FALSE;
			}
			break;
		case MEDAL_MCF_WEAPON_TYPE :
			{ 
				if( eItemType < ITEM_TYPE_PRIMARY || eItemType > ITEM_TYPE_THROWING2 || pMMCInfo->m_pMQFMedalInfo->m_eWeaponType != (WEAPON_CLASS_TYPE)GET_ITEM_SUBTYPE( pMMCInfo->m_ui32UseItemID )  ) return FALSE;
			}
			break;
		case MEDAL_MCF_ITEM_ID :
			{
				if( pMMCInfo->m_pMQFMedalInfo->m_ui32UseItemID != pMMCInfo->m_ui32UseItemID ) return FALSE;
			}
			break;
		}
	}


	// 여기 까지 통과했으면 성공,
	return TRUE;
}
void CRoom::SetUserMissionCard( CUserSession* pSession, ACTION_ID eAttionID, void* pPointer )
{
	if( FALSE == g_pContextMain->IsContentsEnabled(CONTENTS_MISSION_CARD))	return;
	// AI전일 경우 임무 처리 안함
	if( TRUE == IsChallengeMode() )	return;

	// 튜토리얼일 경우 임무 처리 안함
	if ( (StageID::GetStageMode( m_Info._InfoBasic._StageID ) == STAGE_MODE_TUTORIAL) )
		return;

	if( pSession == NULL )
		return;

	QUESTING_INFO* pQuestInfo= &pSession->m_QuestIngInfo;

	UINT8	ui8Use			= pQuestInfo->m_ActiveIndexUse;						// 진행중인 카드셋
	UINT8	ui8CardType		= pQuestInfo->m_pCardSetType[ui8Use];				// 카드 타입
	UINT8	ui8CardSet		= pQuestInfo->m_pActiveIndexCardSet[ui8Use];		// 카드셋에서 진행중인 카드
	UINT16	ui16QuestActive	= pQuestInfo->m_pActiveIndexCard[ui8Use][ui8CardSet];

	if( ui8CardType == 0 )			return;	// 임무카드가 없다.

	if( ui8CardType >= QUEST_CARDSET_TYPE_COUNT )
	{
		I3TRACE("CardType is wrong. UID:%d, ActiveIng:%d, %d/%d/%d/%d, %d/%d/%d/%d\n", pSession->m_i64UID, pQuestInfo->m_ActiveIndexUse, pQuestInfo->m_pCardSetType[0], pQuestInfo->m_pCardSetType[1], pQuestInfo->m_pCardSetType[2], pQuestInfo->m_pCardSetType[3], pQuestInfo->m_pActiveIndexCardSet[0], pQuestInfo->m_pActiveIndexCardSet[1], pQuestInfo->m_pActiveIndexCardSet[2], pQuestInfo->m_pActiveIndexCardSet[3] );
		return;
	}

	QUEST_CTX*				pQuest			= NULL;
	UINT8*					pui8QuestCtxActive	= NULL;

	switch( ui8CardType )
	{	// 현재 사용중인 임무카드를 얻어옵니다.
	case QUEST_CARDSET_TYPE_EVENT:
		pQuest		= &g_pContextMain->m_pQuestEvent[ pSession->m_ui8LocaleCode ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
		break;
	default:
		pQuest		= &g_pContextMain->m_pQuest[ ui8CardType ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
		break;
	}
	pui8QuestCtxActive = &pSession->m_aaui8QuestCtxActive[ ui8Use ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];

	if( NULL == pQuest )				return;
	if( NULL == pui8QuestCtxActive )	return;

	for( INT32 i = 0 ; i < MAX_QUEST_PER_CARD_NEW ; i++ )
	{
		UINT8 ui8QuestCtxActiveIdx	= 0;
		UINT16 ui16QuestComplete	= 0;
		// 이미 완료된 임무인지 확인
		if( 0xF & (ui16QuestActive >> (i*4)) )																continue;
		//if( (pSession->m_pQuestCtxActiveNow[i] + pSession->m_pQuestCtxActive[ui8Use][(ui8CardSet*MAX_CARD_PER_CARDSET)+i]) >= pQuest[i].m_Arg[2] )	continue;
		if( 0xF & (pSession->m_ui16ActiveIndexCardNow >> (i*4)) )											continue;

		if( eAttionID != pQuest[i].m_Arg[0] )																continue;
		// 이곳에서 처리
		if( FALSE == CheckMissionCardComplete( &pQuest[i], eAttionID, pPointer ) )							continue;

		// 임무 수행 확인, 완료 카운트에 도달했는지도 확인 (완료 확인은 완료했을 때 한번만 보내진다)
		pSession->m_aui8QuestCtxActiveNow[ i ]++;
		if( (pSession->m_aui8QuestCtxActiveNow[i] + pSession->m_aaui8QuestCtxActive[ui8Use][(ui8CardSet*MAX_QUEST_PER_CARD_NEW)+i]) >= pQuest[i].m_Arg[2] )	
		{
			ui16QuestComplete = 0xF << (i*4);
			pSession->m_ui16ActiveIndexCardNow |= ui16QuestComplete;
		}

		UINT8 ui8QuestCtxActive;
		ui8QuestCtxActive = pSession->m_aui8QuestCtxActiveNow[i] + pSession->m_aaui8QuestCtxActive[ui8Use][(ui8CardSet*MAX_QUEST_PER_CARD_NEW)+i];

		if( ui16QuestComplete > 0 )
			ui8QuestCtxActiveIdx = (UINT8)(i | 0xF0);
		else
			ui8QuestCtxActiveIdx = (i & 0x0F);

		i3NetworkPacket SendPacket( PROTOCOL_BASE_QUEST_CHANGE_ACK );
		SendPacket.WriteData( &ui8QuestCtxActiveIdx, sizeof(UINT8) );		// Finish : 0xF0, Index : 0x0F
		SendPacket.WriteData( &ui8QuestCtxActive, sizeof(UINT8) );			// Count
		pSession->SendPacketMessage( &SendPacket );
	}
	////0 - Action, 1 - Stage, 2 - Count, 3 - Acc(누적, 연속), 4 - Team, 5 - 카운트	
}
BOOL CRoom::CheckMissionCardComplete( QUEST_CTX* pQuest, ACTION_ID eAttionID, void* pPointer )
{
	//// Stage 체크 - Stage 가 맞지 않으면 실패처리
	STAGE_UID ui32StageUID = static_cast<STAGE_UID>(pQuest->m_Arg[1]);

	// Stage 체크 - Stage 가 맞지 않으면 실패처리
	if( ( STAGE_UID_NONE != ui32StageUID ) && ( ui32StageUID != StageID::GetStageUID( m_Info._InfoBasic._StageID ) ) )	return FALSE;

	switch( eAttionID )
	{
	////////////////////////////////////////////////////
	// 미구현 Action은 실패처리
	case ACTION_ID_HIT_HEAD_20				:			// 머리 명중률 20% 이상
	case ACTION_ID_HIT_HEAD_30				:			// 머리 명중률 30% 이상
	case ACTION_ID_HIT_CHEST_20				:			// 가슴 명중률 20% 이상
	case ACTION_ID_HIT_CHEST_30				:			// 가슴 명중률 30% 이상
	case ACTION_ID_HIT_ARM_20				:			// 팔 명중률 20% 이상
	case ACTION_ID_HIT_ARM_30				:			// 팔 명중률 30% 이상
	case ACTION_ID_HIT_LEG_20				:			// 다리 명중률 20% 이상
	case ACTION_ID_HIT_LEG_30				:			// 다리 명중률 30% 이상 
	case ACTION_ID_ENFORCE_EXIT				:			// 강제퇴장
	case ACTION_ID_RANK_1					:			// 최종 결과 순위 1
	case ACTION_ID_RANK_2					:			// 최종 결과 순위 2
	case ACTION_ID_RANK_3					:			// 최종 결과 순위 3
	case ACTION_ID_RANK_4					:			// 최종 결과 순위 4
	case ACTION_ID_RANK_5					:			// 최종 결과 순위 5
	case ACTION_ID_RANK_6					:			// 최종 결과 순위 6
	case ACTION_ID_RANK_7					:			// 최종 결과 순위 7
	case ACTION_ID_RANK_8					:			// 최종 결과 순위 8
	case ACTION_ID_USE_WEAPON				:			// 특정 무기 사용
		return FALSE;	

	case ACTION_ID_WIN						:			// 승리
	case ACTION_ID_DEFEAT					:			// 패배
		{
			if( pPointer == NULL ) return FALSE;

			UINT8 ui8WinLose = *((UINT8*)pPointer);	// 승리, 패배시 넘어오는 팀 인덱스가 다릅니다.
			if( !ui8WinLose )	return FALSE;
		}
		break;

	case ACTION_ID_DOUBLE_KILL				:			// Double Kill 달성
	case ACTION_ID_HEADSHOT					:			// Head shot 달성
	case ACTION_ID_KILL						:			// Kill
	case ACTION_ID_TRIPLE_KILL				:			// Triple Kill 달성
	case ACTION_ID_KILL_MAN					:			// 인간을 죽여라
		{
			if( pPointer == NULL ) return FALSE;

			UINT32 ui32WeaponInfo	= *((UINT32*)pPointer);			
			UINT32 ui32ItemClass	= g_GetWeaponClass_ForMission(GET_ITEM_SUBTYPE(ui32WeaponInfo));
			UINT32 ui32ItemNum		= GET_ITEM_NUMBER( ui32WeaponInfo );

			if( pQuest->m_Arg[3] > 0 && pQuest-> m_Arg[3] != ui32ItemClass )  return FALSE;		

			// m_Arg[3]과 Weapon Class와의 컨버팅이 필요할수도.. 확인바람
			if( pQuest->m_Arg[4] > 0 && pQuest->m_Arg[4] != ui32ItemNum ) return FALSE;  
		}
		break;
	case ACTION_ID_DEATHBLOW				:			// 데스블로우
		{
			if( pPointer == NULL ) return FALSE;

			if( m_Info._MiDinoInfo._uiTRexSlotIdx != ((DEATHBLOW_INFO*)pPointer)->_nTrexIdx )
				return FALSE;

			UINT32 ui32WeaponInfo	= ((DEATHBLOW_INFO*)pPointer)->_WeaponInfo;
			UINT32 ui32ItemType		= GET_ITEM_TYPE( ui32WeaponInfo );
			UINT32 ui32ItemClass	= g_GetWeaponClass_ForMission( GET_ITEM_SUBTYPE(ui32WeaponInfo));
			UINT32 ui32ItemNum		= GET_ITEM_NUMBER( ui32WeaponInfo );

			if( pQuest->m_Arg[3] > 0 && pQuest-> m_Arg[3] != ui32ItemClass )  return FALSE;
							
			if( pQuest-> m_Arg[3] == WEAPON_CLASS_THROWING_GRENADE ||
				pQuest-> m_Arg[3] == WEAPON_CLASS_THROWING_CONTAIN )
			{	// 투척무기 예외처리 
				if( ui32ItemType == ITEM_TYPE_THROWING1 )	return TRUE;
				if( ui32ItemType == ITEM_TYPE_THROWING2 )	return TRUE;   
			}

			// m_Arg[3]과 Weapon Class와의 컨버팅이 필요할수도.. 확인바람
			if( pQuest->m_Arg[4] > 0 && pQuest->m_Arg[4] != ui32ItemNum ) return FALSE;  
		}
		break;
	case ACTION_ID_KILL_WEAPONCLASS			:			// 특정클래스 무기 사용중인 유저 킬
	case ACTION_ID_DOUBLE_KILL_WEAPONCLASS	:			// 특정클래스 무기 사용중인 유저 더블킬
	case ACTION_ID_TRIPLE_KILL_WEAPONCLASS	:			// 특정클래스 무기 사용중인 유저 트리플킬
		{
			if( pPointer == NULL ) return FALSE;

			USED_WEAPON *stKilledUserWeapon = (USED_WEAPON *)pPointer;
			INT8   i8WeaponInfoIdx  = stKilledUserWeapon->_i8Idx;
			
			UINT8 ui8LoopCount = (UINT8)(eAttionID - ACTION_ID_KILL_WEAPONCLASS + 1);
			
			for( UINT8 i = 0; i < ui8LoopCount ; i++ )
			{
				i8WeaponInfoIdx -= 1;

				if(i8WeaponInfoIdx < 0) i8WeaponInfoIdx = MAX_USED_WEAPON_LIST-1;

				INT8 i8WeaponClass		= (INT8)g_GetWeaponClass_ForMission( stKilledUserWeapon->_i8WeaponClass[i8WeaponInfoIdx] );

				if(pQuest->m_Arg[3] > 0 && pQuest->m_Arg[3] != i8WeaponClass  )	return FALSE;	// 이쪽으로 들어오면 안된다 (특정무기 사용자를 죽여야 하는데... 어느 무기인지 안정해져있으면 안된다...) 
			}
		}
		break;
	}

	return TRUE;
}

// 일일 플레이 시간 누적 & 판수보상
void	CRoom::DaliyPlayTime( INT32 i32WorkIdx, INT32 i32SlotIdx, BOOL bFreeMove ) 
{
	CUserSession* pSession = m_Info._pSession[ i32SlotIdx ];

	// 플레이 시간 누적
	if(	IsChallengeMode() )		return;
	if( IsTutorialMode() )		return;

	// 전장 이동권 사용중인 상태에서 난입후 이탈시 음수발생 할 수 있습니다. 2015.02.09 노준영 
	INT32 i32PlayTime = pSession->m_ui32PlayTime - pSession->m_ui32FirstObserverTime;
	i32PlayTime = MAX( i32PlayTime, 0 );
	
	pSession->DaliyPlayReward( i32WorkIdx, bFreeMove, (UINT32)i32PlayTime );
}

CUserSession* CRoom::GetUserSession( INT32 i32SlotIdx )
{
	if( SLOT_STATE_CLOSE >= m_Info._aSlotInfo[ i32SlotIdx ]._State )	return NULL;

	return m_Info._pSession[ i32SlotIdx ];
}

void CRoom::SetRoundStart()
{
	m_Info._ui32RTRoundStart = GetRoomBattleTime();
	UINT8	ui8EndTimeIdx	= (m_Info._InfoAdd._SubType & 0xF0) >> 4; 
	UINT32	ui32RemainTime	= g_TeamTimeTable[ui8EndTimeIdx]; 

	//Send Message
	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_ROUND_START_ACK );
	SendPacket.WriteData( &m_Info._InfoAdd._NowRoundCount,	sizeof(UINT8) );
	SendPacket.WriteData( &ui32RemainTime,					sizeof(UINT32));
	SendPacket.WriteData( &m_Info._ui16RoundStartUser,		sizeof(UINT16) );
	SendPacket.WriteData( &m_ui8AtkDefChange,				sizeof(UINT8) );
	_SendPacketBattlePlayer( &SendPacket );
}

void CRoom::SetRoundEnd( UINT8 ui8RoundEndType, UINT8 ui8RoundWinTeam )
{
	m_Info._ui8MiEndType		= ui8RoundEndType;
	m_Info._ui8RoundWinTeam		= m_pRoomMode->GetWinTeamCurRound( ui8RoundWinTeam );

	if( m_pRoomMode->CheckRoundEnd( GetRoomBattleTime() ) )

	if( 1 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
	{
		m_pRoomMode->SetRoundDataCount();
	}

	_SetMissionState_M( MISSION_STATE_FREE );

}

//////////////////////////////////////////////////////////////////////////
// 난입유저 스코어출력
void CRoom::GetRoomNowScore(ROOM_NOW_SCORE* pRoomNowScore)
{
	switch( StageID::GetAIMode( m_Info._InfoBasic._StageID ) )
	{
	case AI_USE_CHALLENGE:
	case AI_USE_MADNESS:
	case AI_USE_DIEHARD:
		{
			UINT8	EndTimeIdx			= (m_Info._InfoAdd._SubType & 0xF0) >> 4; 
			pRoomNowScore->ScoreType	= ROOM_SCORE_AI_TYPE;
			pRoomNowScore->PlayTime		= g_TeamTimeTable[EndTimeIdx] - GetRoomBattleTime();
			pRoomNowScore->Score[0]		= m_Info.m_i8currentDifficultyLevel;
			return;
		}
		break;
	}

	m_pRoomMode->GetRoomNowScore( pRoomNowScore, GetRoomBattleTime() );
}

void CRoom::IncreaseKillCount( INT32 i32WorkIdx, INT32 i32UserIdx, INT32 i32IncKillCount, UINT32 ui32WeaponID )
{
	// 1. 데디케이션 서버에 통지
	INT32 ai32Arg[ MAX_COUNT_QA_AUTO_ARG ];
	ai32Arg[ 0 ] = i32IncKillCount;
	g_pModuleCast->BattleQAAuto( i32WorkIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT8)i32UserIdx, (UINT8)QA_AUTO_COMMAND_ID_KILL_COUNT, ai32Arg );

	// 2. 킬카운트값 증가(++)를 Control쓰레드에서 수행하는데(챌린지,튜토리얼 모드 제외), 여기의 User쓰레드에서 증가(+=)하는 것과 충돌이 있을수 있음.
	m_Info._aM_UserScore[ i32UserIdx ].m_KillCount					+= (UINT16)i32IncKillCount;					// 개인 스코어
	m_Info._aUserScoreRound[ i32UserIdx ].m_KillCount				+= (UINT16)i32IncKillCount;				// 라운드 개인 스코어
	m_Info._aTeamScoreTotal[ i32UserIdx % TEAM_COUNT ].m_KillCount	+= (UINT16)i32IncKillCount;	// 팀 스코어
	m_Info._aTeamScoreRound[ i32UserIdx % TEAM_COUNT ].m_KillCount	+= (UINT16)i32IncKillCount;	// 라운드 팀 스코어
	m_pRoomMode->CommandIncreaseKillCount( i32UserIdx, i32IncKillCount );				// 모드에 따른 추가 사항(점수 등) 적용

	// 3. 미션카드, 메달 적용
	CUserSession* pSession = m_Info._pSession[i32UserIdx];
	for( INT32 i = 0; i < i32IncKillCount; i++ )
	{
		MedalMissionCheck( i32WorkIdx, pSession, MEDAL_MC_ACTION_ID_KILL, 0 );
		MedalMissionCheck( i32WorkIdx, pSession, MEDAL_MC_ACTION_ID_USE_WEAPON_KILL, ui32WeaponID );
	}

	// 4. 클라이언트에 변경된 킬카운트 통지
	i3NetworkPacket SendPacket( PROTOCOL_CHEAT_INCREASE_KILL_COUNT_ACK );
	SendPacket.WriteData( m_Info._aTeamScoreTotal,			sizeof(SCORE_INFO)*TEAM_COUNT);
	SendPacket.WriteData( m_Info._aM_UserScore,				sizeof(SCORE_INFO)*SLOT_MAX_COUNT);
	SendPacket.WriteData( m_Info._aui16TotalRoundCount,		sizeof(UINT16)*TEAM_COUNT);
	_SendPacketBattlePlayer(&SendPacket);
}

void CRoom::TeleportCharacter( INT32 i32WorkIdx, INT32 i32UserIdx, INT32 i32TargetIdx, REAL32* pr32Pos )
{
	INT32 ai32Arg[ MAX_COUNT_QA_AUTO_ARG ];
	ai32Arg[ 0 ] = i32TargetIdx;
	i3mem::Copy( &ai32Arg[ 1 ], pr32Pos, sizeof( REAL32 ) * 3 );
	
	g_pModuleCast->BattleQAAuto( i32WorkIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT8)i32UserIdx, (UINT8)QA_AUTO_COMMAND_ID_CHARACTER_MOVE, ai32Arg );
}

void CRoom::DamageToObject( INT32 i32WorkIdx, INT32 i32UserIdx, INT32 i32Team, INT32 i32Damage )
{
	INT32 ai32Arg[ MAX_COUNT_QA_AUTO_ARG ];
	ai32Arg[ 0 ] = i32Team;
	ai32Arg[ 1 ] = i32Damage;
	
	g_pModuleCast->BattleQAAuto( i32WorkIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, (UINT8)i32UserIdx, (UINT8)QA_AUTO_COMMAND_ID_DAMAGE_OBJECT, ai32Arg );
}

void CRoom::TimerGMPause( INT32 i32WorkIdx, UINT32 ui32Second )
{
	m_ui32TimerToPause = i3ThreadTimer::GetServerTimeMilli() + ui32Second*1000;
}

bool CRoom::CheckMatchStart()
{
	switch( m_Info.m_ePreStartOption )
	{
	case ROOM_PRESTART_DIRECT:
		return true;
	case ROOM_PRESTART_ONE_WAIT:
		{
			INT32 i32MainOtherTeam = (m_Info._i32MainSlotIdx+1) % TEAM_COUNT;	// 방장의 다른 팀을 구합니다.
			for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i += 2 )
			{
				if( SLOT_STATE_BATTLE_READY == m_Info._aSlotInfo[i]._State )
				{
					return true;
				}
			}
		}
		break;
	case ROOM_PRESTART_ALL_WAIT:
		{
			UINT32	ui32CheckStartUser		= 0;
			for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
			{
				if( SLOT_STATE_BATTLE_READY == m_Info._aSlotInfo[i]._State )	ui32CheckStartUser |= (0x1 << i); 
			}
			if( (ui32CheckStartUser & m_Info._ui32FirstBattleStartUser) == m_Info._ui32FirstBattleStartUser )
			{
				return true;
			}
		}
		break; 
	}
	return false;
}

void CRoom::EnableSoloPlay()
{
	m_Info.m_eReadyOption		= ROOM_READY_ONE_TEAM;
	m_Info.m_i8PersonAtLeast	= 1;
	m_Info.m_ePreStartOption	= ROOM_PRESTART_DIRECT;
}

INT32 CRoom::CheckUserReady()
{
	if( IsTutorialMode() || IsChallengeMode()) return EVENT_ERROR_SUCCESS;

	INT32 i32Team[TEAM_COUNT] = { 0, };

	switch( m_Info.m_eReadyOption )
	{
	case ROOM_READY_ONE_TEAM:	// 한 팀이상 "요구조건 이상의 인원"(m_i8PersonAtLeast)이 레디해야 합니다.
	case ROOM_READY_BOTH_TEAM:	// 양팀 모두 "요구조건 이상의 인원"(m_i8PersonAtLeast)이 레디해야 합니다.
		{
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				if ( SLOT_STATE_READY == m_Info._aSlotInfo[i]._State || i == m_Info._i32MainSlotIdx )
					i32Team[i % TEAM_COUNT]++;
			}
			
			INT8 i8Success = 0;
			if ( i32Team[TEAM_RED] >= m_Info.m_i8PersonAtLeast )	i8Success++;
			if ( i32Team[TEAM_BLUE] >= m_Info.m_i8PersonAtLeast )	i8Success++;

			if ( i8Success <= m_Info.m_eReadyOption )				return EVENT_ERROR_NO_READY_TEAM;
		}
		break;

	// 일반 유저 모두가 레디 상태여야 시작됩니다. (매치 메이킹도 이 곳에 해당함)
	case ROOM_READY_ALL_READY:	
		{
#ifndef CLAN_MATCH_MAKING
			if( m_Info._InfoBasic._PersonPlayerMax != m_Info._InfoBasic._PersonNow )	return EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY;
#endif

			for(INT32 i = 0 ; i < SLOT_MAX_COUNT; i++)
			{
				if( i == m_Info._i32MainSlotIdx ) continue;
				if( SLOT_STATE_CLOSE >= m_Info._aSlotInfo[i]._State )	continue;

				if( SLOT_STATE_READY != m_Info._aSlotInfo[i]._State )	return EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY;
			}
		}
		break;

	case ROOM_READY_BOTH_TEAM_CLAN:	// 비 매치 메이킹 일때
		{
			TEAM_CLAN_INDEX_LIST	idxList[ TEAM_COUNT ];
			i3mem::FillZero( idxList, sizeof(TEAM_CLAN_INDEX_LIST) * TEAM_COUNT );
			INT32  ai32ValidCount[ TEAM_COUNT ] = { 0, };
			INT32  ai32MaxPerson [ TEAM_COUNT ] = { 0, };

			for( INT32 i = 0 ; i < SLOT_MAX_COUNT; i++ )
			{
				if( m_Info._aSlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;

				if( m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Clanidx == 0 )	
				{	// 클랜에 가입되어있지 않은 사람이 있으면 게임 시작 불가
					return EVENT_ERROR_NO_CLAN_TEAM;
				}

				if( m_Info._i32MainSlotIdx != i && m_Info._aSlotInfo[i]._State < SLOT_STATE_READY )
				{	// 방장을 제외하고 레디 중이 아니면, 시작 불가
					return EVENT_ERROR_NO_READY_TEAM;
				}

				INT32 nTeamIdx		=  i	  % TEAM_COUNT;
				INT32 nEnemyTeamIdx = (i + 1) % TEAM_COUNT;
				++ai32MaxPerson[ nTeamIdx ];	// 모든 유저 수를 체크합니다.

				TEAM_CLAN_INDEX_LIST & enemyIdxList = idxList[ nEnemyTeamIdx ];
				TEAM_CLAN_INDEX_LIST & teamIdxList  = idxList[ nTeamIdx ];
				const UINT32 userClanIndex          = m_Info._aSlotInfo[i]._clanidx;

				if ( isContainedClanIndexInList( enemyIdxList, userClanIndex ) )
				{	// 적팀에 동일한 클랜이 존재하면 시작 불가 조건입니다. 
					// (기획의도 : 비매치 메이킹은 같은 팀에 여러 클랜이 존재 할 수 있음. 따라서 별도로 체크하지 않도록 함)
					return  EVENT_ERROR_NO_CLAN_DUPLE_CLAN_TEAM;
				}

				// 조건에 맞음
				++ai32ValidCount[ nTeamIdx ];

				const UINT8 idxCount = teamIdxList.ui8ClanIdxCount;
				// 그냥 userClanIndex 넣어도 될 것 같은데, 기존에 저렇게 되어있었으니 그대로 유지합니다.
				teamIdxList.aClanIdx[ idxCount ] = m_Info._pSession[i]->GetUserInfoBasic()->m_ui32Clanidx;
				teamIdxList.ui8ClanIdxCount++;

			}

			for ( INT32 i = 0; i < (INT32)TEAM_COUNT; ++i)
			{
				// 일단은 이 곳에 걸릴 일은 없지만, 추후에 다양한 유효성 검사에서 실패하는 데에 걸러 낼 수 있습니다.
				if ( ai32ValidCount[i] != ai32MaxPerson[i] )	
					return  EVENT_ERROR_NO_CLAN_TEAM;
	
				if ( ai32ValidCount[i] < m_Info.m_i8PersonAtLeast )
					return  EVENT_ERROR_NO_CLAN_TEAM_NOTFULL;
			}
			
		
			m_Info._aui32BattleClanIdx[0] = idxList[0].aClanIdx[0];
			m_Info._aui32BattleClanIdx[1] = idxList[1].aClanIdx[0];
		}
		break;
	}

	return EVENT_ERROR_SUCCESS;
}

// 주의: 이 함수의 선두/말미에 크리티컬 섹션을 Enter/Leave하기 때문에, 중간에 return을 사용해서는 안됩니다.	2014.4.3
INT32 CRoom::_AllocateSlot( UINT32 ui32MatchTeam, UINT32 ui32ClanIdx, UINT8 ui8SlotState )
{
	INT32 i32Slot = EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL;

	m_csSession.Enter();

	if( m_Info._InfoBasic._PersonNow < m_Info._InfoBasic._PersonPlayerMax )
	{
		INT32 i32Start = 0;	// 시작
		INT32 i32Incre = 1;	// 증가값

		switch( m_Info.m_eJoinOption )
		{
		case ROOM_JOIN_SEQUENCE:
			{
				i32Start = 0;
				i32Incre = 1;
			}
			break;
		case ROOM_JOIN_SEQUENCE_USER_FOLLOW:
			{
				INT32 i;
				for( i = 0; i < SLOT_MAX_COUNT; i++ )
				{
					if( SLOT_STATE_CLOSE < m_Info._aSlotInfo[i]._State ) break;
				}

				i32Start = i % TEAM_COUNT;
				i32Incre = TEAM_COUNT;
			}
			break;
		case ROOM_JOIN_CLANMATCH:
			{
				INT32 ai32ClanIdx[TEAM_COUNT] = {0, 0};
				INT32 ai32ClanCount[TEAM_COUNT] = {0, 0};

				for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
				{
					INT32 nTeam = i % TEAM_COUNT;

					if( m_Info._aSlotInfo[i]._State == SLOT_STATE_CLOSE ) continue;
					ai32ClanCount[ nTeam ]++;

					if( m_Info._aSlotInfo[i]._State == SLOT_STATE_EMPTY ) continue;

					ai32ClanCount[ nTeam ]--;

					if( ai32ClanIdx[ nTeam ] == 0 )
					{
						ai32ClanIdx[ nTeam ] = m_Info._aSlotInfo[i]._clanidx;
					}
				}

				for( INT32 i = 0 ; i < TEAM_COUNT ; i++ )
				{
					if( ai32ClanCount[ i ] != 0 )
					{
						if( ai32ClanIdx[ i ] == 0 )
						{
							i32Start = i;
							i32Incre = 2;
						}
						else if( (UINT32)ai32ClanIdx[ i ] == ui32ClanIdx )
						{
							i32Start = i;
							i32Incre = 2;
							break;
						}
					}
				}
			}
			break;
		case ROOM_JOIN_CLANMATCH_MAKING:
			{ 
				i32Start = ( m_Info.m_TMatchTeamID[ TEAM_RED ] == ui32MatchTeam ) ? 0 : 1; 
				i32Incre = 2;
			}
			break;
		default:
			{
				i32Start = SLOT_MAX_COUNT;
				i32Incre = 0;
			}
			break;
		}

		// 간단한 예외처리를 합니다.
		if( i32Start < 0 || i32Incre < 1 )
		{
			m_csSession.Leave();
			return i32Slot;
		}

		for( INT32 i = i32Start; i < SLOT_MAX_COUNT; i += i32Incre )
		{
			if( SLOT_STATE_EMPTY != m_Info._aSlotInfo[i]._State ) continue;

			i32Slot = i;
			m_Info._InfoBasic._PersonNow++; 
			m_Info._aSlotInfo[i32Slot]._State			= ui8SlotState;
			break;
		}
	}

	m_csSession.Leave();

	return i32Slot;
}

void CRoom::_GetRandMap()
{
	// 랜덤 맵이 설정 되어있는지 체크
	if (GET_ROOM_INFO_RANDOM_MODE(m_Info._InfoBasic._InfoFlag)
		&& 0 < g_pContextMain->m_ui16RandomMapCount)
	{
		UINT16 ui16TotalWeight = RANDOM_MAP_MAX_PERCENTAGE;
		UINT16 ui16RandomIdx = 0;
		UINT16 ui16RandomWeight = WELL512_Random(m_ui32RandomState, m_ui8RandomIndex) % ui16TotalWeight;
		UINT16 ui16StartIdx = 0;
		UINT16 ui16EndIdx = 0;

		// 랜덤맵 범위 찾기(룰, 총 인원).
		SetRandomMapStartEndIdx(ui16StartIdx, ui16EndIdx);
		ui16RandomIdx = ui16StartIdx;	// Percentage가 정상적이지 않을 경우에 잘못된 정보를 넣지 않기 위한 세팅.

		// 랜덤맵 인덱스 가져오기.
		for (UINT16 i = ui16StartIdx; i <= ui16EndIdx; i++)
		{
			if (ui16RandomWeight < g_pContextMain->m_aRandomMapData[i].m_ui16Percentage)
			{
				ui16RandomIdx = i;
				break;
			}
			ui16RandomWeight -= g_pContextMain->m_aRandomMapData[i].m_ui16Percentage;
		}


		const UINT32 ui32stageUID = g_pContextMain->m_aRandomMapData[ui16RandomIdx].m_ui8StageUID;
		UINT32 ui32Rule = g_pContextMain->m_aRuleData[g_pContextMain->m_aRandomMapData[ui16RandomIdx].m_ui8RuleIdx].m_ui32Rule;

		m_Info._InfoBasic._StageID = (ui32Rule |= (ui32stageUID & 0xFF));
	}

	m_StageUID = StageID::GetStageUID(m_Info._InfoBasic._StageID);

#ifdef I3_DEBUG
	g_pLog->WriteLog(L"RandomMap StageUID : %d, StageID : %d, OptionFlag : %d", m_StageUID, m_Info._InfoBasic._StageID, m_Info._InfoBasic._OptionFlag);
#endif
}

void CRoom::SetRandomMapStartEndIdx(UINT16& ui16StartIdx, UINT16& ui16EndIdx)
{
	const INT32 i32RuleIdx = g_pContextMain->GetRuleIdx(m_Info._InfoBasic._StageID);

	if (-1 == i32RuleIdx)
		return;

	UINT8 ui8MaxPerson = 0;

	if (g_isIncludedEnumFlag<RANDOM_MAX_PERSON_TYPE>(m_Info._InfoBasic._OptionFlag, RANDOM_MAX_PERSON_TYPE::TYPE_RANDOM_MAX_PERSON16))
		ui8MaxPerson = TYPE_RANDOM_MAX_PERSON16;
	else if (g_isIncludedEnumFlag<RANDOM_MAX_PERSON_TYPE>(m_Info._InfoBasic._OptionFlag, RANDOM_MAX_PERSON_TYPE::TYPE_RANDOM_MAX_PERSON10))
		ui8MaxPerson = TYPE_RANDOM_MAX_PERSON10;
	else if (g_isIncludedEnumFlag<RANDOM_MAX_PERSON_TYPE>(m_Info._InfoBasic._OptionFlag, RANDOM_MAX_PERSON_TYPE::TYPE_RANDOM_MAX_PERSON8))
		ui8MaxPerson = TYPE_RANDOM_MAX_PERSON8;

	if (TYPE_RANDOM_MAX_PERSON_END > ui8MaxPerson)
	{
		ui16StartIdx = g_pContextMain->m_aRandomMapIdxData[i32RuleIdx].m_aStartIdx[ui8MaxPerson];
		ui16EndIdx = g_pContextMain->m_aRandomMapIdxData[i32RuleIdx].m_aEndIdx[ui8MaxPerson];

#ifdef I3_DEBUG
		g_pLog->WriteLog(L"RandomMap ui16StartIdx : %d, ui16EndIdx : %d, ui32RuleIdx : %d, ui8MaxPerson : %d", ui16StartIdx, ui16EndIdx, i32RuleIdx, ui8MaxPerson);
#endif
	}
}

void CRoom::_SetUsersScopeType()
{
	for (UINT8 i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		m_Info.m_aui8UsersScopeType[i] = m_Info._pSession[i]->m_ui8UserScopeType;
	}
}

void CRoom::ChangePCCafeValue( INT32 m_i32SlotIdx, UINT8 ui8PCCafe )
{ 
	m_Info._aSlotInfo[m_i32SlotIdx]._pcCafe = ui8PCCafe;
	_SendSlotInfo( NULL );
}

void CRoom::CheckWeaponFlag(ITEM_INFO* aWeaponList, INT32& i32CheckSkipFlag, INT32& i32WeaponUseStop )	 
{
	for( INT32 i = 0; i < 8; ++i )	// GET_ROOM_INFO_WEAPON_KNUCKLE 까지.
	{
		INT32 i32CheckFlag = 0x01<<i;

		if( !( m_Info._InfoBasic._WeaponFlag & i32CheckFlag) ) continue;

		switch( (i32CheckFlag) )
		{
		case LOCK_KNUCKLE : // GET_ROOM_INFO_WEAPON_KNUCKLE
			{
				i32CheckSkipFlag |= (0x01 << CHAR_EQUIPMENT_WEAPON_PRIMARY) | (0x01 << CHAR_EQUIPMENT_WEAPON_SECONDARY);
				switch( aWeaponList[ CHAR_EQUIPMENT_WEAPON_MELEE ].m_TItemID )
				{
				case 323001:				// Fist
					i32CheckSkipFlag |= (0x01 << CHAR_EQUIPMENT_WEAPON_MELEE);
					i32WeaponUseStop |= (0x01 << CHAR_EQUIPMENT_WEAPON_MELEE);
					break;
				}
			} 
			break;
		}
	}

	return;
}

T_ItemID CRoom::GetCharItemId(INT32 i32SlotIdx)
{
	ITEM_INFO CharItemInfo;
	CUserSession * pTargetSession = m_Info._pSession[i32SlotIdx];
	INT32 i32Team = GetNowTeam(i32SlotIdx);

	if (NULL == pTargetSession)				return 0;
	if (-1 == i32Team)						return 0;
	if (NULL == pTargetSession->m_pDesign)	return 0;

	if (FALSE == pTargetSession->m_pDesign->GetMainCharEquipment(&pTargetSession->m_MultiSlot, CHAR_EQUIPMENT_CHARA, &CharItemInfo, i32Team))
	{
		return 0;
	}

	return CharItemInfo.m_TItemID;
}

void CRoom::SendUserScopeTypeInfo(UINT8 ui8ScopeType, UINT32 ui32SlotIdx, CUserSession* pUserSession)
{
	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_USER_SOPETYPE_ACK);
	SendPacket.WriteData(&ui32SlotIdx, sizeof(UINT32));
	SendPacket.WriteData(&ui8ScopeType, sizeof(UINT8));
	_SendPacketAnotherPlayer(&SendPacket, pUserSession);
}
