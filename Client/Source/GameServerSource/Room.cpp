#include "pch.h"
#include "Room.h"
#include "UserSession.h"
#include "USerManager.h"
#include "ModuleLogFile.h"
#include "ModuleControl.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleMessenger.h"
#include "ModuleContextNc.h"

#include "RoomInAnnihilation.h"
#include "RoomInBomb.h"
#include "RoomInEscape.h"
#include "RoomInDeathmatch.h"
#include "RoomInDefence.h"
#include "RoomInDestroy.h"
#include "RoomInSuddenDeath.h"
#include "RoomInHeadHunter.h"
#include "RoomInHeadKiller.h"
#include "RoomInTraining.h"	
#include "RoomInTutorial.h"
#include "RoomInCrossCount.h"

#include "RoomManager.h"

// 중력핵 공용(common에서 dedi와 함께 사용)전역함수		2011.09.08 안국정
extern void g_CommonGetMapBounds( STAGE_ORDINAL_ID OrdStageID, REAL32* pr32Bound );

I3_CLASS_INSTANCE(CRoom, i3ElementBase); 

REAL32	DifferenceXYZ( REAL32 r32A, REAL32 r32B )
{
	if( r32A > r32B )
	{
		return r32A-r32B;
	}
	else
	{
		return r32B-r32A;
	}
}

CRoom::CRoom()
{
	m_pRoomMode				= NULL;
	m_pAnnihilation			= NULL;
	m_pBomb					= NULL;
	m_pEscape				= NULL;
	m_pDeathmatch			= NULL;
	m_pDefence				= NULL;
	m_pDestroy				= NULL;
	m_pSuddenDeath			= NULL;
	m_pHeadHunter			= NULL;
	m_pHeadKiller			= NULL;
	m_pTraining				= NULL;
	m_pTutorial				= NULL;
	m_pCrossCount			= NULL;

	m_ui32OldCountdownTime	= 0;
}

CRoom::~CRoom(void)
{
	OnDestroy();
}

BOOL CRoom::OnCreate( UINT32 ui32ChannelNum, UINT32 ui32RoomNum )
{
	m_Info._ui32ChannelNum	= ui32ChannelNum;
	m_Info._InfoBasic._Idx	= ui32RoomNum;
	m_bIsCreate				= FALSE;

	//Set Nc LogServer 
	m_ui32LogChannelNum		= ((g_pContextMain->m_i32ServerIdx*GAME_CHANNEL_COUNT) + (m_Info._ui32ChannelNum+1));
	m_ui32LogUniqueNumber	= (g_pContextMain->m_i32ServerIdx << 24) | (ui32ChannelNum << 16) | ui32RoomNum;

	// Mode Setting
	m_pAnnihilation = CRoomInAnnihilation::NewObject();
	if( NULL == m_pAnnihilation ) return FALSE;
	if( FALSE == m_pAnnihilation->OnCreate( &m_Info ) )		return FALSE;

	m_pBomb = CRoomInBomb::NewObject();
	if( NULL == m_pBomb ) return FALSE;
	if( FALSE == m_pBomb->OnCreate( &m_Info ) )				return FALSE;

	m_pEscape = CRoomInEscape::NewObject();
	if( NULL == m_pEscape ) return FALSE;
	if( FALSE == m_pEscape->OnCreate( &m_Info ) )			return FALSE;

	m_pDeathmatch = CRoomInDeathmatch::NewObject();
	if( NULL == m_pDeathmatch ) return FALSE;
	if( FALSE == m_pDeathmatch->OnCreate( &m_Info ) )		return FALSE;

	m_pDefence = CRoomInDefence::NewObject();
	if( NULL == m_pDefence ) return FALSE;
	if( FALSE == m_pDefence->OnCreate( &m_Info ) )			return FALSE;

	m_pDestroy = CRoomInDestroy::NewObject();
	if( NULL == m_pDestroy ) return FALSE;
	if( FALSE == m_pDestroy->OnCreate( &m_Info ) )			return FALSE;

	m_pSuddenDeath = CRoomInSuddenDeath::NewObject();
	if( NULL == m_pSuddenDeath ) return FALSE;
	if( FALSE == m_pSuddenDeath->OnCreate( &m_Info ) )		return FALSE;

	m_pHeadHunter = CRoomInHeadHunter::NewObject();
	if( NULL == m_pHeadHunter ) return FALSE;
	if( FALSE == m_pHeadHunter->OnCreate( &m_Info ) )		return FALSE;

	m_pHeadKiller = CRoomInHeadKiller::NewObject();
	if( NULL == m_pHeadKiller ) return FALSE;
	if( FALSE == m_pHeadKiller->OnCreate( &m_Info ) )		return FALSE;

	m_pTraining = CRoomInTraining::NewObject();
	if( NULL == m_pTraining )								return FALSE;
	if( FALSE == m_pTraining->OnCreate( &m_Info ) )			return FALSE;

	m_pTutorial = CRoomInTutorial::NewObject();
	if( NULL == m_pTutorial )								return FALSE;
	if( FALSE == m_pTutorial->OnCreate( &m_Info ) )			return FALSE;

	m_pCrossCount = CRoomInCrossCount::NewObject();
	if( NULL == m_pCrossCount )								return FALSE;
	if( FALSE == m_pCrossCount->OnCreate( &m_Info ) )		return FALSE;

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
	I3_SAFE_RELEASE( m_pSuddenDeath );
	I3_SAFE_RELEASE( m_pHeadHunter );
	I3_SAFE_RELEASE( m_pHeadKiller );
	I3_SAFE_RELEASE( m_pTraining );
	I3_SAFE_RELEASE( m_pTutorial );
	I3_SAFE_RELEASE( m_pCrossCount );
}

BOOL CRoom::CreateRoom( CUserSession * pSession, ROOM_INFO_CREATE* pInfoCreate )
{	
	// 방장 소환핵을 막기위한 임시 방편 - 서동권
	m_ui8Hack_Count = 0;

	ROOM_INFO_BASIC * pRoomInfo = &(pInfoCreate->RoomInfoBasic);

	//Init Value 
#ifdef DEF_OBSERVER_MODE
	for( INT32 i = 0 ; i < SLOT_ALL_COUNT ; i++ )
	{
		m_Info._SlotInfo[i].Reset();
		m_aRoomUserInfo[i].Reset();
	}
#else
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		m_Info._SlotInfo[i].Reset();
		m_aRoomUserInfo[i].Reset();
	}
#endif

	//Set Basic Info
	m_Info._InfoBasic._Ping						= 5;
	m_Info._InfoBasic._PersonNow				= 1;

	if(pRoomInfo->_PersonPlayerMax <= 16)		m_Info._InfoBasic._PersonPlayerMax = pRoomInfo->_PersonPlayerMax;
	else									m_Info._InfoBasic._PersonPlayerMax = SLOT_MAX_COUNT;

#ifdef DEF_OBSERVER_MODE
	m_Info._InfoBasic._PersonObserver			= 0;
	m_Info._InfoBasic._PersonHidden				= 0;
	if( ROOM_INFO_FLAG_OBSERVER_MODE == GET_ROOM_INFO_OBSERVER_MODE(m_Info._InfoBasic._State) )
	{
		m_Info._InfoBasic._PersonObserverMax	= SLOT_MAX_OBSERVER_COUNT;
	}
	else
	{
		m_Info._InfoBasic._PersonObserverMax	= 0;
	}
	m_Info._InfoBasic._PersonHiddenMax			= SLOT_MAX_HIDDEN_OBSERVER_COUNT;
#endif
	m_Info._i32MainSlotIdx						= 0;	
	//m_Info._InfoBasic._StageID				= pRoomInfo->_StageID;
	m_Info._InfoBasic._StageMask				= pRoomInfo->_StageMask;
	m_Info._InfoBasic._InfoFlag					= pRoomInfo->_InfoFlag;
	m_Info._InfoBasic._WeaponFlag				= pRoomInfo->_WeaponFlag;	
	m_Info._InfoBasic._State					= ROOM_STATE_READY;
	if(NULL != pRoomInfo->_Title) { i3String::Copy( m_Info._InfoBasic._Title, pRoomInfo->_Title, NET_ROOM_NAME_SIZE ); }
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
		m_strPasswd[0]					= '\0';
	}

	// 클랜 매치메이킹
	m_Info._aui32BattleClanIdx[0]		= pInfoCreate->wMatchTeamIdx;
	m_Info._aui32BattleClanIdx[1]		= pInfoCreate->wMatchReqTeamIdx;

	// 방정보 갱신
	if(EVENT_ERROR_SUCCESS != ChangeRoomInfo(pSession, pRoomInfo, pInfoCreate->enemyAiCount, pInfoCreate->difficultyLevel)) { return FALSE; }
	if(EVENT_ERROR_SUCCESS != ChangeRoomInfoOption(pSession, &pInfoCreate->RoomInfoAdd)) { return FALSE; }

	//Set Common Value	
	m_ui32CreateTime					= g_pContextMain->GetServerTime();
	m_bIsBattle							= FALSE;
	m_i8WinTeam							= TEAM_DRAW;//TEAM_RED_WIN;
	m_i32TotalRespawnIdx				= 0;
	m_i8StartCountdown					= 0;

	m_Info._aui16TotalRoundCount[0]		= 0;
	m_Info._aui16TotalRoundCount[1]		= 0;

	m_i8KickUserInsertPoint				= 0;
	i3mem::FillZero( m_aKickUserList, sizeof( m_aKickUserList ) );

	ResetBattleGroupInfo(); 

	//Set Main Slot Info
	m_Info._pSession[0]					= pSession;
	m_Info._SlotInfo[0]._State			= SLOT_STATE_NORMAL;
	m_Info._SlotInfo[0]._Rank			= (UINT8)pSession->m_UserInfoBasic._rank;
	m_Info._SlotInfo[0]._pcCafe			= pSession->m_UserInfoBasic._pcCafe;
	m_Info._SlotInfo[0]._eSportID		= pSession->m_UserInfoBasic._eSportID;
	m_Info._SlotInfo[0]._clanidx		= pSession->m_UserInfoBasic._clanidx;
	m_Info._SlotInfo[0]._clanstate		= pSession->m_UserInfoBasic._clanstate;
	m_Info._SlotInfo[0]._clanExpRank	= pSession->m_UserInfoBasic._clanExpRank; 
	m_Info._SlotInfo[0]._clanMark		= pSession->m_UserInfoBasic._clanMark;
	m_Info._SlotInfo[0]._ui8NationalCode	= pSession->m_ui8OtherCountry;
	InsertSlotItem( pSession, &m_Info._SlotInfo[0] );			// 아이템 정보 담기	
	m_aRoomUserInfo[0]._ui8Ping		= pSession->m_ui8Ping;	// 방장 ping 저장
	pSession->m_ui8LoadingStart			= LOAD_START_CREATE;	// Ready 2회 입력시 무시함

	i3mem::Copy( m_Info._SlotInfo[0]._ClanName, pSession->m_UserInfoBasic._clanName, NET_CLAN_NAME_SIZE );

	// Log 용
	m_ui32LogBattleCount				= 0;
	m_i64LogMainUserDbIdx				= pSession->m_i64UID; 

	_ResetTimeOut();

	// 모든 스테이지 검사 후 마지막에 호출 부탁 드립니다.
	if( FALSE == m_pRoomMode->SetRoomOption( pRoomInfo->_StageID ) ) return FALSE;

	// Channel 속성
	switch( g_pContextMain->m_pui8ChannelType[ m_Info._ui32ChannelNum ] )
	{
	default:
		if(pRoomInfo->_PersonPlayerMax <= 16)		m_Info._InfoBasic._PersonPlayerMax = pRoomInfo->_PersonPlayerMax;
		else									m_Info._InfoBasic._PersonPlayerMax = SLOT_MAX_COUNT;

		m_Info._aui32BattleClanIdx[0]		= 0;
		m_Info._aui32BattleClanIdx[1]		= 0;

		m_i8IsClanMatch = ZLOG_CLAN_NOT;
		break;
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN:
		{	// 클랜 채널일 경우에
			// 매치 메이킹의 경우 클라이언트에서 방생성 인원을 제한합니다.
			// 비매치 메이킹의 경우 기본 슬롯개수를 10개로 생성합니다.
			if( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask )
			{	// 매치 메이킹 일떄
				if(pRoomInfo->_PersonPlayerMax <= 16)		m_Info._InfoBasic._PersonPlayerMax = pRoomInfo->_PersonPlayerMax;
				else									m_Info._InfoBasic._PersonPlayerMax = SLOT_MAX_COUNT;
				// 클랜 Idx 저장(이미 해두었음)
				//m_Info._ui32BattleClanIdx[0]		= ui32MatchTeamIdx;
				//m_Info._ui32BattleClanIdx[1]		= ui32MatchReqTeamIdx;
				m_i8IsClanMatch = ZLOG_CLAN_MATCHING;
			}
			else	// 비 매치 메이킹 일떄
			{	// 5개 이상의 슬롯을 닫아준다.
				m_Info._InfoBasic._PersonPlayerMax	= SLOT_CLAN_MAX_COUNT;
				// 클랜 Idx 저장
				m_Info._aui32BattleClanIdx[0]		= 0;
				m_Info._aui32BattleClanIdx[1]		= 0;

				m_i8IsClanMatch = ZLOG_CLAN_NORMAL;
			}				
		}
		break;
	}

	if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
	{
		// 슬롯이 닫혔으므로 맥스를 바꿔준다.
		//m_Info._InfoBasic._PersonPlayerMax	= 8;

		// 일반유저에게만 적용
		for( UINT32 i = 0 ; i < SLOT_MAX_COUNT; i++ )
		{
			m_Info.m_aui32AILevel[ i ] = 0;
		}
	}
	// 4:4일 경우는 Max를 바꿔준다.
	if( STAGE_SLOT_MODE_4VS4 == GET_STAGESLOT( m_Info._InfoBasic._StageID ) )
	{
		if( pRoomInfo->_PersonPlayerMax < 8 )
			m_Info._InfoBasic._PersonPlayerMax= pRoomInfo->_PersonPlayerMax;
		else
			m_Info._InfoBasic._PersonPlayerMax	= 8;
	}

	// 닫힌 슬롯 만큼 슬롯을 닫아준다.
	for( INT32 i = m_Info._InfoBasic._PersonPlayerMax ; i < SLOT_MAX_COUNT; i++ )
	{
		m_Info._SlotInfo[i]._State		= SLOT_STATE_CLOSE;
	}
#ifdef DEF_OBSERVER_MODE
	// Observer Slot
	for(INT32 i = (SLOT_MAX_COUNT + m_Info._InfoBasic._PersonObserverMax); i < SLOT_ROOM_VIEW_COUNT; i++)
	{
		m_Info._SlotInfo[i]._State		= SLOT_STATE_CLOSE;
	}
	// hidden Slot
	for(INT32 i = (SLOT_MAX_COUNT + m_Info._InfoBasic._PersonHiddenMax); i < SLOT_ALL_COUNT; i++)
	{
		m_Info._SlotInfo[i]._State		= SLOT_STATE_CLOSE;
	}
#endif

	// 보급상자
	m_ui16BattleEndUserFlag = 0;	
	i3mem::FillZero( m_ai8PresentSupplyBox, sizeof( m_ai8PresentSupplyBox ) );
	m_ui32SupplyBoxResultReq = 0;
	m_bGMPause = FALSE;

 	return TRUE;
}

// 무기 핵 체크 - 서동권
void CRoom::AddHackCheckWeapon( CUserSession * pSession )
{
	UINT32 ui32Weapon;
	bool bHave;

	m_csSession.Enter(); 
	{	// 주무기, 보조무기만 버릴수 있으므로 두개만 따로 저장합니다.
		for( INT32 i = 0 ; i < WEAPON_USAGE_MELEE ; i++ )	// i = WEAPON_USAGE_PRIMARY ; i <= WEAPON_USAGE_SECONDARY ; i++
		{
			ui32Weapon = pSession->m_aui32UseWeaponList[i];

			if( m_ui8RoomWeaponListCount < HACK_CHECK_WEAPON_COUNT )
			{
				bHave = false;	
				for( UINT32 j = 0; j < m_ui8RoomWeaponListCount; ++j )
				{
					if( m_aui32RoomWeaponList[j] == ui32Weapon )
					{
						bHave = true;
						break;
					}
				}

				if(!bHave)
				{
					m_aui32RoomWeaponList[m_ui8RoomWeaponListCount] = ui32Weapon; 
					m_ui8RoomWeaponListCount++;
				}
			}
		}
	}
	m_csSession.Leave(); 

}

INT32 CRoom::Enter(CUserSession * pSession, ROOM_INFO_JOIN* pInfoJoin)
{
	// 방 입장에 성공 여부와 상관없이 입장 요청을 했다는거 자체가 방에 있지 않은거니 0으로 바꿔줍니다. - 서동권
	pSession->m_ui32RoomBattleCount = 0;

	if( (FALSE == pSession->IsGMUser()) && (FALSE == pSession->IsMODUser()) )
	{	// GM이나 MOD가 아닐 경우에만 체크
		//난입이 불가능한 방
		if( (ROOM_STATE_READY != m_Info._InfoBasic._State) && (1 == m_Info._InfoAdd._LockInterEnter) )
		{
			return EVENT_ERROR_EVENT_JOIN_ROOM_LOCK_INTERENTER; 
		}
		// 클랜 매치 메이킹 일때는 입장 불가
		if( ((CHANNEL_TYPE_CLAN == g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum]) || (CHANNEL_TYPE_PCCAFE_CLAN == g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum]))	&&
			( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask ) )
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


	INT32 i32Slot = EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL; 

	m_csSession.Enter(); 
	{
		if( m_Info._InfoBasic._PersonNow < m_Info._InfoBasic._PersonPlayerMax )
		{
			////////////////////////////////////////////////////////////
			INT32 i32Start = 0;	// 시작
			INT32 i32Incre = 1;	// 증가값

			switch(g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum])
			{	
			default:	break;
			case CHANNEL_TYPE_PCCAFE_CLAN:
			case CHANNEL_TYPE_CLAN		:
				_EnterTypeClan( i32Start, i32Incre, pSession, pInfoJoin );					
				break;
			}
			////////////////////////////////////////////////////////////

			for( INT32 i = i32Start; i < SLOT_MAX_COUNT; i += i32Incre )
			{
				if( SLOT_STATE_EMPTY != m_Info._SlotInfo[i]._State ) continue;

				i32Slot = i;
				m_Info._SlotInfo[i32Slot]._State			= (UINT8)GetStateSub( pSession->GetSubTask()); //SLOT_STATE_NORMAL;
				m_Info._InfoBasic._PersonNow++; 
				break;
			}
		}
#ifdef DEF_OBSERVER_MODE		
		else if(m_Info._InfoBasic._PersonObserver < m_Info._InfoBasic._PersonObserverMax)	
		{// 일반슬롯이 꽉찼을때 옵저버 슬롯으로 입장
			for( INT32 i = SLOT_MAX_COUNT; i < SLOT_ROOM_VIEW_COUNT; i ++ )
			{
				if( SLOT_STATE_EMPTY != m_Info._SlotInfo[i]._State ) continue;

				i32Slot = i;
				m_Info._SlotInfo[i32Slot]._State			= (UINT8)GetStateSub( pSession->GetSubTask()); //SLOT_STATE_NORMAL;
				m_Info._InfoBasic._PersonObserver++; 
				break;
			}
		}		
		else if((m_Info._InfoBasic._PersonHidden < m_Info._InfoBasic._PersonHiddenMax) && (TRUE == pSession->IsGMUser()))
		{// 일반슬롯이 꽉찼고 옵저버 슬롯도 꽉찼을때 히든 슬롯으로 입장(GM 유저만 가능)
			for( INT32 i = SLOT_ROOM_VIEW_COUNT; i < SLOT_ALL_COUNT; i ++ )
			{
				if( SLOT_STATE_EMPTY != m_Info._SlotInfo[i]._State ) continue;

				i32Slot = i;
				m_Info._SlotInfo[i32Slot]._State			= (UINT8)GetStateSub( pSession->GetSubTask()); //SLOT_STATE_NORMAL;
				m_Info._InfoBasic._PersonHidden++; 
				break;
			}
		}
#endif
	}
	m_csSession.Leave();

	// 남는 슬롯이 있는지 체크
	if( EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL == i32Slot )	return EVENT_ERROR_EVENT_JOIN_ROOM_SLOT_FULL;

	// 해당 슬롯에 정보를 체워줍니다.
	m_Info._pSession[i32Slot]					= pSession;
	m_Info._SlotInfo[i32Slot]._Rank				= (UINT8)pSession->m_UserInfoBasic._rank;
	m_Info._SlotInfo[i32Slot]._pcCafe			= pSession->m_UserInfoBasic._pcCafe;
	m_Info._SlotInfo[i32Slot]._eSportID			= pSession->m_UserInfoBasic._eSportID;
	m_Info._SlotInfo[i32Slot]._clanidx			= pSession->m_UserInfoBasic._clanidx;
	m_Info._SlotInfo[i32Slot]._clanstate		= pSession->m_UserInfoBasic._clanstate;
	m_Info._SlotInfo[i32Slot]._clanExpRank		= pSession->m_UserInfoBasic._clanExpRank;
	m_Info._SlotInfo[i32Slot]._clanMark			= pSession->m_UserInfoBasic._clanMark;
	m_Info._SlotInfo[i32Slot]._ui8NationalCode	= pSession->m_ui8OtherCountry;
	InsertSlotItem( pSession, &m_Info._SlotInfo[i32Slot] );		// 아이템 정보 담기

	m_aRoomUserInfo[i32Slot]._ui8Ping			= pSession->m_ui8Ping;

	i3mem::Copy( m_Info._SlotInfo[i32Slot]._ClanName, pSession->m_UserInfoBasic._clanName, NET_CLAN_NAME_SIZE );

	pSession->m_i32RoomIdx						= m_Info._InfoBasic._Idx; 
	pSession->m_i32SlotIdx						= i32Slot; 
	pSession->m_pRoom							= this; 
	pSession->m_bForceRoomLeave					= FALSE; 
	pSession->m_ui8LoadingStart					= LOAD_START_ENTER;

	// 인벤토리 정리작업
	pSession->CheckInventory_MAU(); 

	// 다른 서버에 알림	
	g_pModuleMessenger->MessengerSendFriendRoomEnter( pSession->m_WorkThreadIdx, pSession, (UINT8)m_Info._ui32ChannelNum, (UINT16)m_Info._InfoBasic._Idx); 
	g_pModuleClan->ClanSendRoomEnter( pSession->m_WorkThreadIdx, pSession, m_Info._InfoBasic._Idx );
		
	pSession->SetPosition(GAME_TASK_READY_ROOM);

	i3NetworkPacket SendPacket( PROTOCOL_LOBBY_JOIN_ROOM_ACK );
	SendPacket.WriteData( &m_Info._InfoBasic._Idx,					sizeof(INT32) );
	SendPacket.WriteData( &i32Slot,									sizeof(INT32) );			
	SendPacket.WriteData( &m_Info._InfoBasic,						sizeof(ROOM_INFO_BASIC) );
	SendPacket.WriteData( &m_Info._InfoAdd,							sizeof(ROOM_INFO_ADD) );
	SendPacket.WriteData( m_strPasswd,								NET_ROOM_PW );	// 비번방 초대,따라가기 비번제공을 위해 사용
	SendPacket.WriteData( &m_i8StartCountdown,						sizeof(UINT8) );
	SendPacket.WriteData( &m_Info._i32MainSlotIdx,					sizeof(INT32) );
	SendPacket.WriteData( m_Info._SlotInfo,							sizeof(m_Info._SlotInfo) );
	SendPacket.WriteData( &m_Info._InfoBasic._PersonNow,			sizeof(UINT8) );
#ifdef DEF_OBSERVER_MODE
	for(INT32 i = 0; i < SLOT_ALL_COUNT; i++)
#else
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
#endif
	{
		if( m_Info._SlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;

		UINT8 ui8NickSize = i3String::Length( m_Info._pSession[i]->GetNickName() ) + 1;
		SendPacket.WriteData( &i,									sizeof(UINT8) );
		SendPacket.WriteData( &ui8NickSize,							sizeof(UINT8) );
		SendPacket.WriteData( m_Info._pSession[i]->GetNickName(),	ui8NickSize );
		SendPacket.WriteData( &m_Info._pSession[i]->m_UserInfoBasic._nickcolor,	sizeof(UINT8) );
	}

	// 훈련전이라면 추가 패킷
	if ( ( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask ) ||
		 ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_DOMINATION ) ) 
	{
		SendPacket.WriteData(&m_Info.m_i8enemyAiCount,				sizeof(INT8));
		SendPacket.WriteData(&m_Info.m_i8difficultyLevel,			sizeof(INT8));
	}
		
	pSession->SendPacketMessage( &SendPacket );

	i3NetworkPacket Packet(PROTOCOL_ROOM_GET_SLOTONEINFO_ACK);
	Packet.WriteData( &i32Slot,						sizeof(INT32) );
	Packet.WriteData( &m_Info._SlotInfo[i32Slot],	sizeof(SLOT_INFO) );
	Packet.WriteData( pSession->GetNickName(),		NET_NICK_NAME_SIZE ); 	//의심
	Packet.WriteData( &pSession->m_UserInfoBasic._nickcolor,	sizeof(UINT8) );

#ifdef DEF_OBSERVER_MODE
	for( INT32 i = 0; i < SLOT_ALL_COUNT; i++)
#else
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
#endif
		if( m_Info._SlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;
		if( i32Slot == i )										continue;

		m_Info._pSession[i]->SendPacketMessage( &Packet );
	}
	
	return i32Slot; 
}

/*-----------------------------------------------------------
Name : _EnterTypeClan
Desc : 레디룸으로 들어갈떄_클랜채널
-----------------------------------------------------------*/
void CRoom::_EnterTypeClan( INT32 & i32Start, INT32 & i32Incre, CUserSession * pSession, ROOM_INFO_JOIN* pInfoJoin  )
{	
	if( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask )
	{	// 클랜 레더 매치전일경우
		i32Incre = 2;
		if( m_Info._aui32BattleClanIdx[ 0 ] == pInfoJoin->wMatchTeam )
		{
			i32Start = 0;
		}
		else
		{
			i32Start = 1;
		}
	}
	else
	{	// 클랜 매치전일경우
		INT32 ai32ClanIdx[2] = {0, 0};
		INT32 ai32ClanCount[2] = {0, 0};

		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			INT32 i32Team = i % TEAM_COUNT;

			if( m_Info._SlotInfo[i]._State == SLOT_STATE_CLOSE ) continue;
			ai32ClanCount[ i32Team ]++;

			if( m_Info._SlotInfo[i]._State == SLOT_STATE_EMPTY ) continue;

			ai32ClanCount[ i32Team ]--;

			if( ai32ClanIdx[ i32Team ] == 0 )
			{
				ai32ClanIdx[ i32Team ] = m_Info._SlotInfo[i]._clanidx;
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
				else if( (UINT32)ai32ClanIdx[ i ] == pSession->m_UserInfoBasic._clanidx )
				{
					i32Start = i;
					i32Incre = 2;
					break;
				}
			}
		}
	}
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
	if( m_Info._SlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP )		return EVENT_ERROR_FAIL;	// 방에 없는 유저 (State <= SLOT_STATE_CLOSE 로 체크해야 맞으나 속도상 이득을 위해.. )

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
		pSession->m_ui8Ping = m_aRoomUserInfo[i32SlotIdx]._ui8Ping;	// 핑을 넣어준다.

		// 방의 인원을 차감해 줍니다.
		m_Info._InfoBasic._PersonNow--;
		if ( m_Info._InfoBasic._PersonNow > 250 )
		{
			// 255 가 나올 경우를 대비하여 땜빵코드..  2011.10.17 안국정	10.21 수정
			char strTemp[ MAX_PATH ];
			i3String::Format( strTemp, MAX_PATH, "LeaveLobby_MU 1Byte Underflow - PersonNow:%d, ID:%s(%I64d), Channel:%d, RoomIdx:%d, InfoBasicIdx:%d, StageID:%u, Title:%s, Ping:%d",
				m_Info._InfoBasic._PersonNow,
				pSession->m_strId,
				pSession->m_i64UID,
				pSession->m_i32ChannelNum,
				pSession->m_i32RoomIdx,
				m_Info._InfoBasic._Idx,
				m_Info._InfoBasic._StageID,
				m_Info._InfoBasic._Title,
				m_Info._InfoBasic._Ping
			);
			g_pModuleLogFile->Write_M_Log( i32WorkThread, pSession, strTemp );
		}

		// 방 삭제가 아니면 해당 슬롯을 초기화 해 줍니다.
		if( DESTROY_ROOM != i32Rv )
		{
			m_Info._SlotInfo[i32SlotIdx].Reset();
			m_aRoomUserInfo[i32SlotIdx].Reset();
			_SendSlotInfo( NULL );
		}
	}
	m_csSession.Leave();

	g_pModuleMessenger->MessengerSendFriendRoomLeave(i32WorkThread, pSession);

	switch(g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum])
	{
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN: 
		if( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask )
		{
			g_pModuleClan->ClanSendRoomLeaveResult( i32WorkThread, pSession );
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
BOOL CRoom::CheckGameTime(CUserSession * pSession, INT32 i32GameTime, UINT8 ui8Ping, UINT8 ui8RoundCount  )
{
	char strTemp[MAX_PATH];

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: break; 
	case BATTLE_SERVER_TYPE_DEDICATION	:	
		if( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )
		{
			m_aRoomUserInfo[ pSession->m_i32SlotIdx ]._ui8Ping = ui8Ping; 
		}
		break; 
	default: return EVENT_ERROR_ROOM;	break; 
	}

	if( ROOM_STATE_BATTLE != m_Info._InfoBasic._State )	return TRUE;

	UINT8	ui8EndTimeIdx		= (m_Info._InfoAdd._SubType & 0xF0) >> 4; 
	INT32	i32RemainTime = 0;

	switch( m_Info._i32StageType )
	{
	case STAGE_TYPE_ESCAPE			: 
	case STAGE_TYPE_ANNIHILATION	:
	case STAGE_TYPE_BOMB			:
	case STAGE_TYPE_DESTROY			:
	case STAGE_TYPE_DEFENCE			:
		if(m_Info._i32MiStateType != MISSION_STATE_BATTLE) return TRUE;
	case STAGE_TYPE_DEATHMATCH		:
		i32RemainTime	= g_TeamTimeTable[ui8EndTimeIdx] - (
#ifdef USE_GM_PAUSE
																GetRoomBattleTime()
#else
																g_pContextMain->GetServerTime()
#endif
																								- m_Info._ui32BattleStartTime);
		break;
	}

	//I3TRACE( "Check Time : %d, %d\n", iRemainTime, GameTime );
	//10초에서 20초로 변경했습니다. 0923		20초에서 30초로 변경 1008
	//유저의 시간이 더빠르면 처리 합니다. 
	INT32 i32Diff = i32RemainTime - i32GameTime;
	//if( 0 > i32Diff )			i32Diff = -i32Diff;	// 절대값으로 변경.
	if( i32Diff > 30 )
	{
		i3String::Format( strTemp, MAX_PATH, "EndTimeIdx:%u, TeamTimeTable:%u, RTBattle:%u, i32Diff:%d, iRemainTime:%d, GameTime:%d, StageType:%d",
			ui8EndTimeIdx,
			g_TeamTimeTable[ui8EndTimeIdx],
#ifdef USE_GM_PAUSE
			GetRoomBattleTime()
#else
			g_pContextMain->GetServerTime()
#endif
											- m_Info._ui32BattleStartTime,
			i32Diff,
			i32RemainTime,
			i32GameTime,
			m_Info._i32StageType
		);
		g_pModuleLogFile->Write_M_Log( pSession->m_WorkThreadIdx, pSession, strTemp );

		return FALSE;
	}

	

	return TRUE;
}

BOOL CRoom::CheckLoadingStart_U( char* pstrMapName )
{
	if( 0 != i3String::Compare( g_pContextMain->GetStageName( (STAGE_ID)(m_Info._InfoBasic._StageID) ), pstrMapName )) return FALSE;

	return TRUE;
}

void CRoom::_UserPenalty( INT32 i32WorkIdx, CUserSession * pSession, BOOL bFreeMove )
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx; 
	switch(g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum])
	{
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN		:
		{	// 클랜 채널이라면 클랜 결과를 클랜서버에 우선 전달한다.
			CLAN_RESULT			ClanResult;
			CLAN_RESULT_USER	ClanResultUser;

			ClanResultUser._ClanIdx	= pSession->m_UserInfoBasic._clanidx;
			ClanResultUser._i64UID	= pSession->m_i64UID;
			ClanResultUser._WinLose = 2;
			ClanResultUser._Death	= 0;
			ClanResultUser._Exp		= 0;
			ClanResultUser._Head	= 0;
			ClanResultUser._Kill	= 0;

			// AI 모드와 튜토리얼의 경우 전적을 반영하지 않는다.
			if ( ( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )	&&
				 ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL ) )
			{
				g_pModuleClan->ClanSendBattleResult( i32WorkIdx, 0, 0, &ClanResult, 1, &ClanResultUser );
			}
		}
		// break; <-- 없는게 맞음
	default:

		// 어뷰징 체크 - 게임 도중에 나간유저의 배틀 시작 시간 초기화
		pSession->m_ui32BattleStartTime = 0;
		pSession->m_ui32BattleStartInvenTime = 0;
		pSession->m_ui32PlayTime = 0;

		
		// 자유 이동권이 있거나 전장이동권이 있으면 패널티 제로 & 킬뎃은 계산
		if(  (-1 != pSession->GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_FREE_MOVE, CASHITEM_DAY_NO) )) ||  
			 (-1 != pSession->GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_FREE_MOVE_EV, CASHITEM_DAY_NO) )) || bFreeMove)
		{
			if( (m_Info._aui16AccExp[i32SlotIdx] == 0) && (m_Info._aM_UserScore[i32SlotIdx].m_DeathCount == 0) )
			{//전적이 하나도 없는 유저
				m_Info._aui16AccExp[i32SlotIdx]	= 0;
				m_Info._aui16AccPoint[i32SlotIdx]	= 0;
			}

			//REAL32 rBattleCountEndPercent	= 1.0f;		//인원수에 따른 카운트
			//REAL32 rAddPercent				= 1.0f;		//비정상 종료 일때 
			//rBattleCountEndPercent	= _GetBattleCountEndPercent();
			//rAddPercent				= _GetAddPercent( iWin );

			//m_Info._ui16AccExp[SlotIdx]		= (UINT16)(m_Info._ui16AccExp[SlotIdx]	* rAddPercent * rBattleCountEndPercent);
			//m_Info._ui16AccPoint[SlotIdx]		= (UINT16)(m_Info._ui16AccPoint[SlotIdx]	* rAddPercent * rBattleCountEndPercent);

			//if( GET_STAGEMASK_TRAINING( m_Info._InfoBasic._StageMask ) )
			//{
			//	m_Info._ui16AccExp[SlotIdx] = (UINT16)(m_Info._ui16AccTrainingScore[SlotIdx] * 0.20001f);
			//	m_Info._ui16AccPoint[SlotIdx] = (UINT16)(m_Info._ui16AccTrainingScore[SlotIdx] * 0.50001f);
			//}

			//_ResultUpdateUser_M( WorkIdx, SlotIdx );
		}
		else
		{	
			// AI 모드와 튜토리얼의 경우 이탈횟수 계산하지 않는다.
			if ( (STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )		&&
				 ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )	&&
				 ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_DOMINATION ))
			{
				UINT32 ui32Penalty = 0;

				// 패널티를 받아야한다
				switch( m_Info._pSession[i32SlotIdx]->m_ui8LocaleCode )
				{				
				case LOCALE_CODE_MATCH			:		ui32Penalty = 0;		break;
				case LOCALE_CODE_CHINA			:		ui32Penalty = 50;		break;
				default							:		ui32Penalty = 200;		break;
				}

				if(pSession->m_UserInfoBasic._point > ui32Penalty)
				{
					pSession->m_UserInfoBasic._point = pSession->m_UserInfoBasic._point - ui32Penalty;							
				}
				else
				{
					ui32Penalty = pSession->m_UserInfoBasic._point;
					pSession->m_UserInfoBasic._point = 0;
				}

				INT32 i32GetRoute	= 7;
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
	INT32 i32SlotIdx = pSession->m_i32SlotIdx; 

	if( 0 > i32SlotIdx )										return -1;			// 올바른 슬롯인지.
	if( SLOT_MAX_COUNT <= i32SlotIdx )							return -1;

	return _GiveUpBattle( i32WorkThreadIdx, pSession, bFreeMove, ROOM_ENDTYPE_GIVEUP ); 
}

/*-----------------------------------------------------------
Name : GiveUpBattle
Desc : battle중 방으로 나갈때  추후 다시 한번 고려해야할 패킷 던지는것과..
-----------------------------------------------------------*/
INT32 CRoom::_GiveUpBattle( INT32 i32WorkIdx, CUserSession * pSession, BOOL bFreeMove, INT32 i32EndType )
{
	// SlotIdx 가 올바른 값인지는 미리 계산하고 들어와야 합니다.
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;
	if( 0 > i32SlotIdx || SLOT_MAX_COUNT <= i32SlotIdx )		return -1;			// SlotIdx 범위 확인
	if( SLOT_STATE_LOAD > m_Info._SlotInfo[i32SlotIdx]._State )	return -1;			// 게임중인 유저인지
	
	if( SLOT_STATE_BATTLE ==  m_Info._SlotInfo[i32SlotIdx]._State )
	{
		if( ROOM_ENDTYPE_VOTE != i32EndType )
		{	// 강퇴로 나가는 유저는 페널티를 적용하지 않습니다.
			_UserPenalty( i32WorkIdx, pSession, bFreeMove );
		}
		else
		{
			pSession->m_ui32PlayTime =
#ifdef USE_GM_PAUSE
										GetRoomBattleTime()
#else
										g_pContextMain->GetServerTime()
#endif
																			- pSession->m_ui32BattleStartTime;
			pSession->m_ui32LoginBattleTime					+= pSession->m_ui32PlayTime;
			pSession->m_UserInfoBasic._ui32TotalBattleTime	+= pSession->m_ui32PlayTime;
		}

		// 사용한 무기를 인증하는 루틴을 보냅니다.
		_SendUsedWeapon( i32WorkIdx, pSession );

		g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_R_BATTLE_END, pSession, this, &i32EndType );
	}

	m_csSession.Enter();
	{
		pSession->m_bIsBattleComplete					= FALSE;
		pSession->m_ui8LoadingStart						= LOAD_START_GIVEUP_BATTLE_END;
		m_Info._SlotInfo[i32SlotIdx]._State				= SLOT_STATE_NORMAL; 
		_SendSlotInfo(NULL);

		m_Info._ui32FirstBattleStartUser				&= ~(0x1 << i32SlotIdx);
		m_Info._ui16RoundStartUser						=  (UINT16)( (UINT32)m_Info._ui16RoundStartUser & ~(0x01UL << i32SlotIdx));
		if(FALSE == m_Info._SlotInfo[i32SlotIdx]._bIsGmObserverMode) { m_Info._ui16BattleUser =  (UINT16)( (UINT32)m_Info._ui16BattleUser & ~(0x01UL << i32SlotIdx)); }
		m_ui16QuestEndFlag								=  (UINT16)( (UINT32)m_ui16QuestEndFlag			& ~(0x01UL << i32SlotIdx));

		//Set Record
		m_Info._aM_UserScore[i32SlotIdx].m_DeathCount	= 0;
		m_Info._aM_UserScore[i32SlotIdx].m_KillCount	= 0; 
		m_Info._aUserScoreRound[i32SlotIdx].m_DeathCount= 0;
		m_Info._aUserScoreRound[i32SlotIdx].m_KillCount	= 0;
		m_Info._aui16M_Headshot[i32SlotIdx]				= 0;
		m_Info._aui16AccExp[i32SlotIdx]					= 0; 
		m_Info._aui16AccPoint[i32SlotIdx]				= 0;
		m_Info._aui16AccTrainingScore[i32SlotIdx]		= 0;
		m_Info._aui8AccMissionCount[i32SlotIdx]			= 0;
		m_Info._aui8A_Mission[i32SlotIdx]				= 0;
		m_Info.m_aui8A_KillCount[i32SlotIdx]			= 0; 
		m_Info.m_aui8A_HeadShot[i32SlotIdx]				= 0;
		m_Info.m_aui8A_SlugShot[i32SlotIdx]				= 0;
#ifdef NEW_EXPPOINT
		m_Info._aui16Acc_KillExp[i32SlotIdx]			= 0;
#endif
		m_Info._abDinoStateUpdate[i32SlotIdx]			= false;
		m_Info._aui32DInoCharInfo[i32SlotIdx]			= 0;
		m_Info._aui32RespawnCount[i32SlotIdx]			= 0;

		m_pRoomMode->UserGiveUpBattle( i32SlotIdx );

		_RemoveTimeOutFlag( i32SlotIdx );						//타임 아웃이 걸려 있으면

#ifndef USE_SOLO_PLAY
		if ( (STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )		||
			 (GET_STAGETYPE( m_Info._InfoBasic._StageID) == STAGE_TYPE_DOMINATION) )
#endif
		{
			UINT16 ui16TeamMask = 0;
			if( (m_Info._i32MainSlotIdx % TEAM_COUNT ) == TEAM_RED )
				ui16TeamMask = BLUE_TEAM_MASK;
			else
				ui16TeamMask = RED_TEAM_MASK;

			if(FALSE == m_Info._SlotInfo[i32SlotIdx]._bIsGmObserverMode) { m_Info._ui16BattleUser |= ui16TeamMask; }			
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
			g_pModuleControl->BattleUserLeave( i32WorkIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, i32SlotIdx, pSession );
		}

		I3ASSERT( i32SlotIdx != -1);

		//SendData
		i3NetworkPacket Packet( PROTOCOL_BATTLE_GIVEUPBATTLE_ACK );
		Packet.WriteData( &i32SlotIdx,	sizeof(INT32));
		Packet.WriteData( &pSession->m_UserInfoBasic._exp,			sizeof(UINT32));			//경험치
		Packet.WriteData( &pSession->m_UserInfoBasic._rank,			sizeof(UINT32));			//계급
		Packet.WriteData( &pSession->m_UserInfoBasic._point,		sizeof(UINT32));			//계급
		Packet.WriteData( &pSession->m_UserInfoRecord._dis,			sizeof(UINT32));			//DisConnect 
		Packet.WriteData( &pSession->m_UserInfoRecord._sdis,		sizeof(UINT32));			//DisConnect Count
		//Packet.WriteData( &pSession->m_UserChallengeIngInfo,		sizeof(CHALLENGE_ING_INFO));//도전과제 데이터 
		m_pRoomMode->AddGiveUpBattlePacket( &Packet, pSession );
		_SendPacketAllPlayer(&Packet);
	}
	m_csSession.Leave();

	pSession->SetPosition(GAME_TASK_READY_ROOM);

#ifdef USE_GM_PAUSE
	if ( GMResume( i32WorkIdx, i32SlotIdx ) > 0 )
		SendGMResume();
#endif

	return EVENT_ERROR_SUCCESS; 
}

void CRoom::SendGMResume()
{
	static INT8 i8Ret = EVENT_ERROR_SUCCESS_1;
	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_GM_RESUME_ACK );
	SendPacket.WriteData( &i8Ret, sizeof( INT8 ) );
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
	if( (ROOM_STATE_COUNTDOWN < m_Info._InfoBasic._State) && (m_Info._SlotInfo[i32SlotIdx]._State >= SLOT_STATE_READY) )
		return EVENT_ERROR_SUCCESS;

	if( m_Info._i32MainSlotIdx == i32SlotIdx )
	{// 방장일 경우		
		BOOL bStartLoding = FALSE;

		//서버 체크 
		switch( g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum] )
		{
		case CHANNEL_TYPE_PCCAFE_CLAN:
		case CHANNEL_TYPE_CLAN		: 
			{	// Clan Channel 일때 체크
				i32Rv = _ReadyBattleClan();
				if( EV_SUCCESSED( i32Rv ) )
				{
					bStartLoding = TRUE;
				}
			}
			break;
		default:
			{	// 기본 서버일때				
				// 방장이 GM이고, 옵저버 모드이면 시작하지 않습니다.
				if( TRUE == m_Info._SlotInfo[m_Info._i32MainSlotIdx]._bIsGmObserverMode )
				{
					BOOL bMainTeam		= FALSE;
					BOOL bMainOtherTeam	= FALSE;
					// 방장의 팀 찾기
					INT32 i32MainTeam = (m_Info._i32MainSlotIdx) % TEAM_COUNT;
					for( INT32 i = i32MainTeam ; i < SLOT_MAX_COUNT ; i+=2 )
					{					
						if( SLOT_STATE_READY != m_Info._SlotInfo[i]._State ) continue;
						if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode ) continue;
						//상대편이 한명이라도 있으면 시작합니다.
						bMainTeam = TRUE;
						break;
					}
					// 상대편찾기
					INT32 i32MainOtherTeam = (m_Info._i32MainSlotIdx+1) % TEAM_COUNT;
					for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i+=2 )
					{					
						if( SLOT_STATE_READY != m_Info._SlotInfo[i]._State ) continue;
						if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode ) continue;
						//상대편이 한명이라도 있으면 시작합니다.
						bMainOtherTeam = TRUE;
						break;
					}

					if(bMainTeam && bMainOtherTeam) { bStartLoding = TRUE; }
				}
				else
				{
					// 방장의 팀 찾기
					INT32 i32MainOtherTeam = (m_Info._i32MainSlotIdx+1) % TEAM_COUNT;
					for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i+=2 )
					{					
						if( SLOT_STATE_READY != m_Info._SlotInfo[i]._State ) continue;
						if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode ) continue;
						//상대편이 한명이라도 있으면 시작합니다.
						bStartLoding = TRUE;
						break;
					}
				}

				if( bStartLoding == FALSE )
				{
					i32Rv = EVENT_ERROR_NO_READY_TEAM;
				}
			}
			break;
		}

		if ( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
		{	// 훈련전일 경우 바로 시작
			bStartLoding = TRUE;
		}

		// 튜토리얼 맵인경우 바로 시작
		if ( ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL )	||
			 ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_DOMINATION ) )
		{
			bStartLoding = TRUE;
		}

#ifdef USE_SOLO_PLAY
		// 솔로 플레이
		bStartLoding = TRUE;
#endif

		//성백 : 이건 뭔가요? 필요없을듯.
		if( FALSE == m_pRoomMode->CheckReady() )
		{
			bStartLoding = FALSE;
		}

		//카운트 중간에 멈춤. 
		if( ROOM_STATE_COUNTDOWN == m_Info._InfoBasic._State )
		{
			//_SendSlotInfo( NULL ); //<- 이거 필요할까? 
			bStartLoding = FALSE;
		}

		if( bStartLoding )
		{
			i32Rv = EVENT_ERROR_SUCCESS;
			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_READY; 

			// 4대4의 경우 초과 유저에 대해 Ready 상태를 푼다
			if (STAGE_SLOT_MODE_4VS4 == GET_STAGESLOT(m_Info._InfoBasic._StageID))
			{
				UnreadySlotFor4vs4();
			}
			else
			{
				if( ( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )	&&
					 STAGE_TYPE_DOMINATION != GET_STAGETYPE(m_Info._InfoBasic._StageID)	)
				{
					// ! praptor - 4대4의 경우 팀밸런스 일단 무시합니다
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
			}
			_SendSlotInfo( NULL );
		}
		else
		{
			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
			i3NetworkPacket Packet( PROTOCOL_BATTLE_READYBATTLE_ACK );
			Packet.WriteData( &i32Rv, sizeof(INT32) );
			pSession->SendPacketMessage( &Packet );
		}
	}
	else
	{// 방장이 아닐 경우
		//레디 버튼을 클릭할 수 있는지 검사합니다. 
		switch( m_ui8BattleType )
		{
		case BATTLE_SERVER_TYPE_P2P			: 
			{
				if( m_Info._pSession[m_Info._i32MainSlotIdx]->IsRealIp() || pSession->IsRealIp() || //둘중 한명이라도 리얼아이피가 있던가. 
					(m_Info._pSession[m_Info._i32MainSlotIdx]->m_UdpNetInfo.m_nRealIP == pSession->m_UdpNetInfo.m_nRealIP))//같은 네트웍에 있으면
				{
					break; 
				}

				//Error 
				i32Rv = EVENT_ERROR_NO_START_FOR_UNDER_NAT;  
				i3NetworkPacket Packet( PROTOCOL_BATTLE_READYBATTLE_ACK );
				Packet.WriteData( &i32Rv, sizeof(INT32) );
				pSession->SendPacketMessage( &Packet );
				return i32Rv;
			}
			break; 
		case BATTLE_SERVER_TYPE_RELAY		: 
		case BATTLE_SERVER_TYPE_DEDICATION	:
		default								: 
			break; 
		}

		//같이 게임을 할 수 있는 유저 입니다. 
		if( m_Info._InfoBasic._State <= ROOM_STATE_COUNTDOWN )
		{
			//상태 변경 
			if( m_Info._SlotInfo[i32SlotIdx]._State == SLOT_STATE_READY )
				m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
			else
				m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_READY; 

			_SendSlotInfo( NULL );
		}
		else
		{//난입 입니다. 
			switch( g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum] )
			{
			case CHANNEL_TYPE_PCCAFE_CLAN:
			case CHANNEL_TYPE_CLAN		: 
				{
					if( ( STAGE_MASK_TYPE_CLANMATCH != m_Info._InfoBasic._StageMask ) )
					{
						if( m_Info._aui32BattleClanIdx[ i32SlotIdx % TEAM_COUNT ] != pSession->GetUserInfoBasic()->_clanidx )
						{
							i32Rv = EVENT_ERROR_NO_CLAN_TEAM;  
							i3NetworkPacket Packet( PROTOCOL_BATTLE_READYBATTLE_ACK );
							Packet.WriteData( &i32Rv, sizeof(INT32) );
							pSession->SendPacketMessage( &Packet );
							return i32Rv; 
						}
					}
				}
				break;
			default:
				break;
			}

			// 4대4의 경우 초과 유저에 대해 난입 금지
			if( STAGE_SLOT_MODE_4VS4 == GET_STAGESLOT(m_Info._InfoBasic._StageID) )
			{
				if (!IsAllowJoinFor4vs4(pSession))
				{
					i32Rv = EVENT_ERROR_NO_CLAN_TEAM;  // 이부분은 수정되어야 할것 같습니다. 
					i3NetworkPacket Packet( PROTOCOL_BATTLE_READYBATTLE_ACK );
					Packet.WriteData( &i32Rv, sizeof(INT32) );
					pSession->SendPacketMessage( &Packet );
					return i32Rv; 
				}					
			}

			//게임 시작 
			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_LOAD;
			if( ( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )	&&
					 STAGE_TYPE_DOMINATION != GET_STAGETYPE(m_Info._InfoBasic._StageID)	)
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
				g_pModuleControl->BattleUserEnter(pSession->m_WorkThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, i32SlotIdx, m_Info._SlotInfo[i32SlotIdx]._Item, pSession); 
			}
			
			_SendSlotInfo( NULL );	//유저들의 정보를 보낸다. 

			//난입 이므로 로딩을 합니다. 
			i32Rv = SLOT_STATE_LOAD; 
			i3NetworkPacket Packet( PROTOCOL_BATTLE_READYBATTLE_ACK );
			Packet.WriteData( &i32Rv,								sizeof(INT32) );
			Packet.WriteData( &m_Info._InfoBasic._StageID,			sizeof(UINT32) );
			Packet.WriteData( &pSession->m_UseCharaList,			sizeof(USER_INFO_EQUIPMENT_CHARA ) );
			pSession->SendPacketMessage( &Packet );
		}
	}

	return i32Rv; 
}

void	CRoom::GM_KickUser_U( CUserSession * pSession, INT8 i8Slot )
{
#ifdef DEF_OBSERVER_MODE
	if( (i8Slot < 0) || (i8Slot >= SLOT_ROOM_VIEW_COUNT) )	return; 
#else
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )		return; 
#endif
	if( i8Slot == pSession->m_i32SlotIdx )					return;
	if( NULL == m_Info._pSession[i8Slot] )					return;

	switch( m_Info._SlotInfo[ i8Slot ]._State )
	{
	case SLOT_STATE_SHOP:
	case SLOT_STATE_INFO:
	case SLOT_STATE_CLAN:
	case SLOT_STATE_INVENTORY:
	case SLOT_STATE_NORMAL:
	case SLOT_STATE_READY:
	case SLOT_STATE_RS:
		{
		}
		break;
	case SLOT_STATE_BATTLE:
		{
			// 로그 기록 - 서동권
			_GiveUpBattle( pSession->m_WorkThreadIdx, m_Info._pSession[i8Slot], FALSE, ROOM_ENDTYPE_GM_KICK );
			UINT8 ui8GMKick = 2;
			g_pModuleContextNC->LogSendMessage( pSession->m_WorkThreadIdx, LOG_MESSAGE_U_R_KICK_LEAVE_ROOM, pSession, &ui8GMKick);
		}
		break;
	default:
		return;
	}

	//강퇴합니다. 
	i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_KICK_PLAYER);  
	m_Info._pSession[i8Slot]->SendPacketMessage(&SendPacket); 
	m_Info._pSession[i8Slot]->m_bForceRoomLeave = TRUE;
}

void	CRoom::GM_ExitUser_U( CUserSession * pSession, INT8 i8Slot )
{
#ifdef DEF_OBSERVER_MODE
	if( (i8Slot < 0) || (i8Slot >= SLOT_ROOM_VIEW_COUNT) )	return; 
#else
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )		return; 
#endif
	if( i8Slot == pSession->m_i32SlotIdx )					return;
	if( NULL == m_Info._pSession[i8Slot] )					return;

	m_Info._pSession[i8Slot]->_SendErrorMessage( pSession->m_WorkThreadIdx, EVENT_ERROR_HACKING_EXIT_USER, PROTOCOL_ROOM_GM_EXIT_USER_REQ, KILL_REASON_HACK_PACKET );
}

void	CRoom::GM_BlockUser_U( CUserSession * pSession, INT8 i8Slot, const char *strComment )
{
#ifdef DEF_OBSERVER_MODE
	if( (i8Slot < 0) || (i8Slot >= SLOT_ROOM_VIEW_COUNT) )	return; 
#else
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )		return; 
#endif
	if( i8Slot == pSession->m_i32SlotIdx )					return;
	if( NULL == m_Info._pSession[i8Slot] )					return;

	// 블록 요청을 Trans 서버로 보냅니다.
	g_pModuleTrans->TransSendGMBlockUser(pSession->m_WorkThreadIdx, m_Info._pSession[i8Slot]->m_i64UID, strComment);
}

void	CRoom::GM_DestroyRoom_U( CUserSession * pSession )
{
	for(INT32 i = 0 ; i < SLOT_MAX_COUNT; i++)
	{
		switch( m_Info._SlotInfo[ i ]._State )
		{
		case SLOT_STATE_SHOP:
		case SLOT_STATE_INFO:
		case SLOT_STATE_CLAN:
		case SLOT_STATE_INVENTORY:
		case SLOT_STATE_NORMAL:
		case SLOT_STATE_READY:
		case SLOT_STATE_RS:
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
			m_Info._pSession[ i ]->SendPacketMessage(&SendPacket); 
			m_Info._pSession[ i ]->m_bForceRoomLeave = TRUE;
		}
	}
}


BOOL	CRoom::GetUserItemInfo( CUserSession * pSession, INT8 i8Slot )
{
	if( i8Slot == pSession->m_i32SlotIdx )					return FALSE;
	if( (i8Slot < 0) || (i8Slot >= SLOT_MAX_COUNT) )		return FALSE;
	if( NULL == m_Info._pSession[i8Slot] )					return FALSE;
	if( SLOT_STATE_SHOP >  m_Info._SlotInfo[i8Slot]._State )return FALSE; 

	CUserSession * pSessionUser = m_Info._pSession[i8Slot];

	if( NULL != pSessionUser ) pSessionUser->SendMyItemInfo( pSession, VIEW_USER_ITEM_INFO_TYPE_ROOM );

	return TRUE;
}

INT32	CRoom::_ReadyBattleClan()
{
	INT32 ai32ClanIdx[ TEAM_COUNT ] = {0,};
	INT32 ai32Count[ TEAM_COUNT ] = {0,};
	BOOL bStart = FALSE;

	if( STAGE_MASK_TYPE_CLANMATCH != m_Info._InfoBasic._StageMask )
	{
		for(INT32 i = 0 ; i < SLOT_MAX_COUNT; i++ )
		{
			if( m_Info._SlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;
			else if( m_Info._pSession[i]->GetUserInfoBasic()->_clanstate == 0 )		// 클랜에 가입되어있지 않은 사람이 있으면 게임 시작 불가
			{
				return EVENT_ERROR_NO_CLAN_TEAM;
			}
			else
			{
				INT32 i32TeamIdx = i % TEAM_COUNT;

				if( m_Info._i32MainSlotIdx != i && m_Info._SlotInfo[i]._State >= SLOT_STATE_READY )
				{
					bStart = TRUE;
				}
				if( ai32ClanIdx[ i32TeamIdx ] == 0 || ai32ClanIdx[ i32TeamIdx ] == m_Info._SlotInfo[i]._clanidx )
				{
					ai32ClanIdx[ i32TeamIdx ]	= m_Info._pSession[i]->GetUserInfoBasic()->_clanidx;
					ai32Count[ i32TeamIdx ]++;
				}
			}
		}
#ifndef CLAN_MATCH_SOLO_PLAY
		INT32 i32ClanCount = 4;
#else
		INT32 i32ClanCount = 1;
#endif
		if( bStart == FALSE )
		{
			return  EVENT_ERROR_NO_READY_TEAM;
		}
		else if( ai32Count[ 0 ] < i32ClanCount || ai32Count[ 1 ] < i32ClanCount )
		{
			return  EVENT_ERROR_NO_CLAN_TEAM_NOTFULL;
		}
		
		m_Info._aui32BattleClanIdx[0] = ai32ClanIdx[ 0 ];
		m_Info._aui32BattleClanIdx[1] = ai32ClanIdx[ 1 ];
	}
	else
	{
#ifndef CLAN_MATCH_SOLO_PLAY
		// 일반 유저 모두가 레뒤 상태여야 시작됩니다.
		// 옵저버 유저와 히든 유저는 검사하지 않는다.
		if( m_Info._InfoBasic._PersonPlayerMax != m_Info._InfoBasic._PersonNow )	return EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY;

		for(INT32 i = 0 ; i < SLOT_MAX_COUNT; i++)
		{
			if( i == m_Info._i32MainSlotIdx ) continue;
			if( SLOT_STATE_CLOSE >= m_Info._SlotInfo[i]._State )	continue;

			if( SLOT_STATE_READY != m_Info._SlotInfo[i]._State )
			{
				return EVENT_ERROR_NO_CLAN_TEAM_NOT_ALL_READY;
			}
		}
#endif
	}
	return EVENT_ERROR_SUCCESS;
}

/*-----------------------------------------------------------
Name : PreStart Battle 
Desc : 로딩이 마무리 되고 Client간 HolePunching을 합니다. 방장은 바로 게임 시작..클라이언트들은 로딩이 끝난후에 들어와야 한다. 
-----------------------------------------------------------*/
INT32 CRoom::PreStartBattle( CUserSession * pSession)
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx;

	// 로딩 유저가 아니면 패킷을 받을 수 없습니다.
	if( SLOT_STATE_LOAD != m_Info._SlotInfo[i32SlotIdx]._State )
	{
		_SendStartBattle( pSession, FALSE );
		return 0;
	}

	//방장이 로딩이 완료후 홀펀칭이 가능한 시점.
	switch( m_Info._InfoBasic._State )
	{
	case ROOM_STATE_BATTLE_END	:
	case ROOM_STATE_READY		: 
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL; 
		_SendSlotInfo( NULL );
		_SendPreStartBattle( pSession, FALSE);
		break; 
	case ROOM_STATE_LOADING: //방장이 로딩중이므로 기달려야 합니다.
		if( m_Info._pSession[ m_Info._i32MainSlotIdx ]->IsRealIp() ||
			( m_Info._pSession[ m_Info._i32MainSlotIdx ]->m_UdpNetInfo.m_nNATState == NAT_STATE_FULL_CONE_NAT ) ||
			( (m_Info._i32MainSlotIdx != i32SlotIdx ) && (m_Info._pSession[m_Info._i32MainSlotIdx]->m_UdpNetInfo.m_nRealIP == pSession->m_UdpNetInfo.m_nRealIP) ) )
		{	//방장이 리얼 IP 나 FullCone 이거나 같은 아이피에 있일 경우에는 바로 게임 시작 준비
			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_PRESTART;
			_SendSlotInfo( NULL );
		}
		else
		{	// 훈련전 상황
			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_PRESTART;
			_SendSlotInfo( NULL );
		}
		break; 
	case ROOM_STATE_PRE_BATTLE	:	
	case ROOM_STATE_BATTLE		:
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_PRESTART;
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
		
	// 유저 핵 체크를 위한 부분 - 서동권
	pSession->m_ui8Hack_Count = 0;
	pSession->m_ui8MHack_Count = 0;

	// 유저 플레이 타임을 기록하기 위한 부분 - 서동권
	pSession->m_ui32PlayTime					= 0;
	pSession->m_ui32BattleStartTime				= 
#ifdef USE_GM_PAUSE
													0;
#else
													g_pContextMain->GetServerTime();
#endif
	pSession->m_ui32BattleStartInvenTime		= g_pContextMain->GetInvenTime();
	//for( INT32 i = 0 ; i < WEAPON_USAGE_COUNT ; i++ )
	//{
	//	pSession->m_EnduranceUsedWeapon[i].m_ui32UsedTime	= g_pContextMain->GetServerTime();
	//}

	++pSession->m_ui32RoomBattleCount;

	pSession->m_ui32AccActualBattleTime			= 0;
	pSession->m_ui32RespawnTime					= 0;

	// battle 하는 맵의 중력핵 좌표6개를 공용전역함수로부터 얻어옵니다.	2011.09.08 안국정
	g_CommonGetMapBounds( GET_STAGEORD(m_Info._InfoBasic._StageID), m_ar32Boundry );

	INT32 i32Slot = pSession->m_i32SlotIdx; 

	if( i32Slot >= SLOT_MAX_COUNT || i32Slot <= SERVER_VALUE_UNKNOWN )	return EVENT_ERROR_FAIL;

	m_abReSpawn[i32Slot]	= false; 
	m_abDeath[i32Slot]		= false; 
	m_abAlive[i32Slot]		= false;

	switch(m_Info._InfoBasic._State) 
	{
	case ROOM_STATE_PRE_BATTLE:
		{
			m_Info._SlotInfo[i32Slot]._State = SLOT_STATE_BATTLE_READY;	//상태 변경 
			_SendSlotInfo(NULL);
		}
		break; 
	case ROOM_STATE_BATTLE:
		{
			if( m_Info._SlotInfo[i32Slot]._State != SLOT_STATE_PRESTART )
			{

				m_Info._SlotInfo[i32Slot]._State = SLOT_STATE_NORMAL; 
				_SendSlotInfo(NULL);
				_SendStartBattle(m_Info._pSession[i32Slot], FALSE);
				break;
			}
			if(FALSE == m_Info._SlotInfo[i32Slot]._bIsGmObserverMode) { m_Info._ui16BattleUser |= (0x01 << i32Slot); }			
			m_Info._pSession[i32Slot]->m_ui32BattleCheckTime = 
#ifdef USE_GM_PAUSE
																GetRoomBattleTime();
#else
																g_pContextMain->GetServerTime();
#endif
			m_Info._SlotInfo[i32Slot]._State				= SLOT_STATE_BATTLE;
			i3mem::FillZero( m_Info._pSession[i32Slot]->m_aui8QuestCtxActiveNow, sizeof(UINT8)*MAX_QUEST_PER_CARD_NEW );
			m_Info._pSession[i32Slot]->m_ui16ActiveIndexCardNow = 0;
			SetUserMissionCard( m_Info._pSession[i32Slot], ACTION_ID_STAGE_INTERCEPT );

			_SendSlotInfo(NULL);
			_SendStartBattle(m_Info._pSession[i32Slot], TRUE, true );
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
			m_Info._SlotInfo[i32Slot]._State = SLOT_STATE_NORMAL; 
			_SendSlotInfo(NULL);
			_SendStartBattle(m_Info._pSession[i32Slot], FALSE);
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
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_INVENTORY;	
	}
	else
	{
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;	
		InsertSlotItem( pSession, &m_Info._SlotInfo[i32SlotIdx] );		// 아이템 정보 담기
	}
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveShop(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 
	if(bEnter)
	{
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_SHOP;
	}
	else
	{
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
		InsertSlotItem( pSession, &m_Info._SlotInfo[i32SlotIdx] );		// 아이템 정보 담기
	}
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveInfo(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 
	if(bEnter)
	{
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_INFO;
	}
	else
	{
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
	}
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveClan(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 
	if(bEnter)		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_CLAN;
	else			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
	_SendSlotInfo( NULL ); 
}

void CRoom::MoveRS(		CUserSession * pSession , BOOL bEnter )
{
	INT32	i32SlotIdx = pSession->m_i32SlotIdx; 
	if(bEnter)		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_RS;
	else			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
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
		if( m_Info._SlotInfo[i]._State == SLOT_STATE_CLAN ) continue;;

		if( (m_Info._SlotInfo[i]._State > SLOT_STATE_CLOSE) && 
			(m_Info._SlotInfo[i]._State < SLOT_STATE_LOAD) )
		{
			m_Info._pSession[i]->SendPacketMessage(pPacket);
		}
	}
#ifdef DEF_OBSERVER_MODE
	// 옵저버 유저는 팀채팅을 받지 못하게, 히든 유저는 모든 채팅을 받도록 한다.
	for( INT32 Loop = SLOT_MAX_COUNT; Loop < SLOT_ALL_COUNT; Loop++ )
	{
		if( m_Info._SlotInfo[Loop]._State == SLOT_STATE_CLAN ) continue;;

		if( (m_Info._SlotInfo[Loop]._State > SLOT_STATE_CLOSE) && 
			(m_Info._SlotInfo[Loop]._State < SLOT_STATE_LOAD) )
		{
			if(i32TeamIdx != SERVER_VALUE_UNKNOWN && SLOT_MAX_COUNT <= Loop && SLOT_ROOM_VIEW_COUNT > Loop && (FALSE == m_Info._pSession[Loop]->IsGMUser())) continue;
			m_Info._pSession[Loop]->SendPacketMessage(pPacket);
		}
	}
#endif
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
			if( m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE)
			{
				m_Info._pSession[i]->SendPacketMessage(pPacket);
			}
		}
	}else 
	{
		for(i = i32TeamIdx; i < SLOT_MAX_COUNT; i += 2)
		{
			if( m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE )
			{
				m_Info._pSession[i]->SendPacketMessage(pPacket);
			}
		}
	}

#ifdef DEF_OBSERVER_MODE
	//옵저버, 히든 유저
	for(i = SLOT_MAX_COUNT; i < SLOT_ALL_COUNT; i ++)
	{
		if( m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE)
		{
			if(i32TeamIdx != SERVER_VALUE_UNKNOWN && SLOT_MAX_COUNT <= i && SLOT_ROOM_VIEW_COUNT > i && (FALSE == m_Info._pSession[i]->IsGMUser())) continue;
			m_Info._pSession[i]->SendPacketMessage(pPacket);
		}
	}
#endif

	return; 
}

void CRoom::SendPlayerInfo(	CUserSession * pSession, INT32 i32SlotIdx )
{
	i3NetworkPacket SendPacket( PROTOCOL_ROOM_GET_PLAYERINFO_ACK ); 

	if( m_Info._SlotInfo[i32SlotIdx]._State > SLOT_STATE_CLOSE)
	{
		CUserSession * pInfoSession = m_Info._pSession[ i32SlotIdx ]; 
		SendPacket.WriteData( &i32SlotIdx, sizeof(INT32) );
		SendPacket.WriteData( pInfoSession->GetUserInfoBasic(),			sizeof(USER_INFO_BASIC)				); 
		SendPacket.WriteData( &pInfoSession->m_UserInfoRecord,			sizeof(USER_INFO_RECORD)			); 
		SendPacket.WriteData( &pInfoSession->m_UserInfoEquipmentChara,	sizeof(USER_INFO_EQUIPMENT_CHARA)	); 		
		SendPacket.WriteData( &pInfoSession->m_UserInfoEquipmentWeapon,	sizeof(USER_INFO_EQUIPMENT_WEAPON)	);
		SendPacket.WriteData( pInfoSession->m_aui8EquipUserTitle,		sizeof(UINT8) * MAX_EQUIP_USER_TITLE);
	}
	else
	{
		i32SlotIdx = SERVER_VALUE_UNKNOWN; 
		SendPacket.WriteData( &i32SlotIdx, sizeof(INT32));
	}
	pSession->SendPacketMessage( &SendPacket ); 

	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendPacketAllPlayer(i3NetworkPacket * pPacket)
{
	INT32 i; 
#ifdef DEF_OBSERVER_MODE
	for(i = 0; i < SLOT_ALL_COUNT; i ++)
#else
	for(i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
	{
		if( m_Info._SlotInfo[i]._State > SLOT_STATE_CLOSE )
		{
			if( NULL == m_Info._pSession[i] ) continue;
			m_Info._pSession[i]->SendPacketMessage(pPacket);
		}
	}
	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendPacketAnotherPlayer( i3NetworkPacket * pPacket, CUserSession * pSession )
{
	INT32 i; 
#ifdef DEF_OBSERVER_MODE
	for(i = 0; i < SLOT_ALL_COUNT; i ++)
#else
	for(i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
	{
		if( m_Info._SlotInfo[i]._State > SLOT_STATE_CLOSE )
		{   
			if( NULL == m_Info._pSession[i] )		continue;
			if( pSession == m_Info._pSession[i] )	continue;
			m_Info._pSession[i]->SendPacketMessage(pPacket);
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
		Packet.WriteData( &i32SlotIdx,		sizeof( INT32 ) );
		pSession->SendPacketMessage( &Packet );
		return; 
	}

	//2. 배틀 여부에 대한 정보가 없을때.  
	if(m_bIsBattleRoomCreated == false)
	{//실패 입니다. 
		bSuccess = FALSE; 
		i3NetworkPacket Packet( PROTOCOL_BATTLE_PRESTARTBATTLE_ACK );
		Packet.WriteData( &bSuccess,	sizeof( BOOL ) );
		Packet.WriteData( &i32SlotIdx,		sizeof( INT32 ) );
		pSession->SendPacketMessage( &Packet );
		return; 
	}

	//튜토리얼 예외처리 
	if( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL )
	{
		bSuccess		= TRUE; 
		i32SlotIdx		= 0; 
		m_ui8BattleType = BATTLE_SERVER_TYPE_P2P; 
	}

	// 성공 패킷 
	i3NetworkPacket Packet( PROTOCOL_BATTLE_PRESTARTBATTLE_ACK );
	Packet.WriteData( &bSuccess,				sizeof(BOOL)); 
	Packet.WriteData( &i32SlotIdx,				sizeof(INT32));
	Packet.WriteData( &m_ui8BattleType,			sizeof(UINT8));
	Packet.WriteData( &m_Info._pSession[m_Info._i32MainSlotIdx]->m_UdpNetInfo,	sizeof( NET_UDP_INFO ) );
	Packet.WriteData( &m_Info._pSession[i32SlotIdx]->m_UdpNetInfo,				sizeof( NET_UDP_INFO ) );
	Packet.WriteData( &m_ui32BattleServerIP,	sizeof(UINT32));
	Packet.WriteData( &m_ui16BattleServerPort,	sizeof(UINT16));
	Packet.WriteData( &m_ui32BattleRoomIdx,		sizeof(UINT32));
	Packet.WriteData( &m_ui32BattleEncryptKey,	sizeof(UINT32));

	if(m_Info._i32MainSlotIdx == i32SlotIdx)
	{
		// _SHUFFLE_HIT_PART
		UINT8 aui8HitPartIndex[ CHARA_HIT_MAXCOUNT ];
		ShuffleHitPart( aui8HitPartIndex, m_ui32BattleEncryptKey );
		Packet.WriteData( aui8HitPartIndex, sizeof(aui8HitPartIndex));

		pSession->SendPacketMessage( &Packet );
	}
	else
	{
		m_Info._pSession[m_Info._i32MainSlotIdx]->SendPacketMessage( &Packet );

		// _SHUFFLE_HIT_PART
		UINT8 aui8HitPartIndex[ CHARA_HIT_MAXCOUNT ];
		ShuffleHitPart( aui8HitPartIndex, m_ui32BattleEncryptKey );
		Packet.WriteData( aui8HitPartIndex, sizeof(aui8HitPartIndex));
		
		pSession->SendPacketMessage( &Packet );
	}


	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendStartBattle(CUserSession * pSession, BOOL bSuccess, bool bNoStartRound )
{
	//1. Set Basic Packet 
	i3NetworkPacket Packet( PROTOCOL_BATTLE_STARTBATTLE_ACK );	
	Packet.WriteData( &bSuccess,					sizeof(BOOL) );		// 가능한지
	Packet.WriteData( &pSession->m_i32SlotIdx,		sizeof(INT32) );	// 시작한 유저의 슬롯 번호 

	if(bSuccess)
	{//성공 
		if ( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
		{
			i3NetworkPacket packet2(PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK);
			packet2.WriteData(&m_Info.m_i8currentDifficultyLevel,	sizeof(INT8));
			packet2.WriteData( m_Info.m_aui32AILevel,				sizeof( m_Info.m_aui32AILevel ) );
			_SendPacketBattlePlayer(&packet2);
		}

		//2. 난입한 유저인지 확인
		UINT8 ui8IsInterEnter = 1; 
		if( (m_Info._ui32FirstBattleStartUser & (0x1 << pSession->m_i32SlotIdx)) )	ui8IsInterEnter = 0;
#ifdef USE_GM_PAUSE
		if( ui8IsInterEnter == 1 && m_bGMPause )
		{
			pSession->m_ui32ATCanRespawnPacket = g_pContextMain->GetServerTime();
			ui8IsInterEnter = 2;
		}
#endif

		//3. 게임중인 사람들의 장비 정보를 받는다. 
		if( bNoStartRound )
		{
			_SendEquipmentInfo(pSession);
		}

		Packet.WriteData( &pSession->m_UseCharaList,			sizeof(USER_INFO_EQUIPMENT_CHARA ) );		// 이거 아직 확정 없다.  ????????????????????????????		
		Packet.WriteData( pSession->m_aui8EquipUserTitle,		sizeof(UINT8) * MAX_EQUIP_USER_TITLE);
		Packet.WriteData( &ui8IsInterEnter,						sizeof(UINT8) );							//  중간에 들어가는지
		// 난입한 사람들을 위한 추가 패킷. 이곳에서 게임시작에 필요한 메시지를 추가해줍니다.
		m_pRoomMode->AddStartBattlePacket( &Packet, pSession, GetRoomBattleTime() );

		// 어뷰징 체크 - 난입한 유저의 배틀 시작 시간 설정
		pSession->m_ui32BattleStartTime	=
#ifdef USE_GM_PAUSE
											GetRoomBattleTime();
#else
											g_pContextMain->GetServerTime();
#endif
		pSession->m_ui32BattleStartInvenTime					= g_pContextMain->GetInvenTime();
		pSession->m_bFirstObserver				= TRUE;

		//모두에게 보냄 
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
		{
			if( m_Info._SlotInfo[i]._State > SLOT_STATE_READY )
			{
				m_Info._pSession[i]->SendPacketMessage( &Packet );
			}
		}

		if(  bNoStartRound )
		{
			switch( m_Info._i32MiStateType )
			{
			case MISSION_STATE_PRE_BATTLE :
			{
				i3NetworkPacket SendPacket2( PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK );
				SendPacket2.WriteData( &m_Info._ui16RoundStartUser, sizeof(UINT16) );
				SendPacket2.WriteData( &m_Info._MiDinoInfo, sizeof(DINO_ROOM_INFO) ); //사이즈가 크지 않아 그냥 보낸다.
				pSession->SendPacketMessage( &SendPacket2 );
			}
			break;
			case MISSION_STATE_BATTLE:
			{
				UINT8 ui8EndTimeIdx = (m_Info._InfoAdd._SubType & 0xF0) >> 4;
				UINT32 ui32RemainTime = g_TeamTimeTable[ui8EndTimeIdx] - (
#ifdef USE_GM_PAUSE
																			GetRoomBattleTime()
#else
																			g_pContextMain->GetServerTime() 
#endif
																											- m_Info._ui32BattleStartTime );

				i3NetworkPacket SendPacket2( PROTOCOL_BATTLE_MISSION_ROUND_START_ACK );
				SendPacket2.WriteData( &m_Info._InfoAdd._NowRoundCount, sizeof(UINT8) );
				SendPacket2.WriteData( &ui32RemainTime, sizeof(UINT32)); // 게임 남은 타임
				SendPacket2.WriteData( &m_Info._ui16RoundStartUser, sizeof(UINT16) );
				pSession->SendPacketMessage( &SendPacket2 );
			}
			break;
			}
		}

		pSession->SetPosition(GAME_TASK_BATTLE);

	}
	else
	{//자신만 받음 
		pSession->SendPacketMessage( &Packet );
	}

	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::_SendEquipmentInfo(	CUserSession * pSession )
{
	i3NetworkPacket Packet( PROTOCOL_BATTLE_EQUIPMENT_INFO_ACK  );
	INT32	i32EmptyIdx = SERVER_VALUE_UNKNOWN;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( m_Info._SlotInfo[i]._State > SLOT_STATE_READY )
		{
			CUserSession* pSe = m_Info._pSession[i];
			Packet.WriteData( &i,	sizeof( INT32 ) );
			Packet.WriteData( &pSe->m_UseCharaList,				sizeof( pSe->m_UseCharaList ) );
			Packet.WriteData( pSe->m_aui32UseWeaponList,			sizeof( pSe->m_aui32UseWeaponList ) );
			Packet.WriteData( pSe->m_aui8UseWeaponEndurance,	sizeof( pSe->m_aui8UseWeaponEndurance ) );
			Packet.WriteData( pSe->m_aui8EquipUserTitle,		sizeof( pSe->m_aui8EquipUserTitle ) );
		}
		else
		{
			Packet.WriteData( &i32EmptyIdx,	sizeof( INT32 ) );
		}
	}

	pSession->SendPacketMessage( &Packet );
	return; 
}

void CRoom::SendSlotEquipmentInfo( INT32 i32ThreadIdx, INT32 i32Slot, UINT16 ui16ChangeFlag )
{
	if( 0 > i32Slot )												return;		//슬롯 인덱스가 정확한지.
	if( SLOT_MAX_COUNT <= i32Slot )									return;		//슬롯 인덱스가 정확한지.

	GC_PROTOCOL_SLOT_EQUIPMENT Struct;
	Struct.m_i32SlotIdx					= i32Slot;
	Struct.m_ui16ChangeFlag				= ui16ChangeFlag;

	CUserSession* pSe = m_Info._pSession[i32Slot];

	Struct.m_EquipmentWeapon._noprim	= pSe->m_aui32UseWeaponList[ WEAPON_USAGE_PRIMARY ];
	Struct.m_EquipmentWeapon._nosub		= pSe->m_aui32UseWeaponList[ WEAPON_USAGE_SECONDARY ];
	Struct.m_EquipmentWeapon._nomelee	= pSe->m_aui32UseWeaponList[ WEAPON_USAGE_MELEE ];
	Struct.m_EquipmentWeapon._nothrow	= pSe->m_aui32UseWeaponList[ WEAPON_USAGE_THROWING ];
	Struct.m_EquipmentWeapon._noitem	= pSe->m_aui32UseWeaponList[ WEAPON_USAGE_ITEM ];
	I3ASSERT( sizeof( Struct.m_EquipmentChara ) == sizeof( pSe->m_UseCharaList ) );
	i3mem::Copy( &Struct.m_EquipmentChara,		&pSe->m_UseCharaList,			sizeof(Struct.m_EquipmentChara) );
	I3ASSERT( sizeof( Struct.m_aui8WeaponEndurance ) == sizeof( pSe->m_aui8UseWeaponEndurance ) );
	i3mem::Copy( Struct.m_aui8WeaponEndurance,	pSe->m_aui8UseWeaponEndurance,	sizeof(Struct.m_aui8WeaponEndurance) );
	I3ASSERT( sizeof( Struct.m_aui8EquipUserTitle ) == sizeof( pSe->m_aui8EquipUserTitle ) );
	i3mem::Copy( Struct.m_aui8EquipUserTitle,	pSe->m_aui8EquipUserTitle,		sizeof(Struct.m_aui8EquipUserTitle) );
	Struct.m_EquipItem					= m_Info._SlotInfo[i32Slot]._Item;

	if ( isDedicationMode() )
	{
		// 컨트롤서버->데디케이션서버->컨트롤서버->게임서버를 거쳐 클라이언트로 보낸다.
		g_pModuleControl->BattleWeaponInfo( i32ThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, i32Slot, &Struct );
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
	i3NetworkPacket Packet(PROTOCOL_ROOM_GET_SLOTINFO_ACK);
	Packet.WriteData( &m_Info._i32MainSlotIdx,	sizeof(INT32) );
	Packet.WriteData( m_Info._SlotInfo,			sizeof( m_Info._SlotInfo )  );

	if(pSession == NULL)	_SendPacketAllPlayer(&Packet);
	else					pSession->SendPacketMessage(&Packet);

	return; 
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
void CRoom::CheckMainClient_U(CUserSession * pSession )
{
	//타임 아웃이 들어 갔는지 체크 합니다. 
	if( m_ui32MCTimeOutStartTime < 1 )
	{	
		//타임 세팅
		m_ui32MCTimeOutStartTime	= g_pContextMain->GetServerTime();
		m_ui16MCTimeOutFlg			= 0;
		m_ui16MCTimeOutStartUserFlg	= 0; 

		//시작하는 유저를 세팅 
#ifdef DEF_OBSERVER_MODE
		for(INT32 i = 0; i < SLOT_ALL_COUNT; i ++)
#else
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
		{
			if( (m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE) && (i != m_Info._i32MainSlotIdx) )
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
	if(m_ui32MCTimeOutStartTime > 0) 
	{//자신 플래그를 0으로 변경합니다. 
#ifdef DEF_OBSERVER_MODE
		INT32 i16Temp = 0; 
#else
		INT16 i16Temp = 0; 
#endif
		i16Temp = 1 << i32SlotIdx; 
		i16Temp = ~i16Temp; 
		m_ui16MCTimeOutStartUserFlg	= m_ui16MCTimeOutStartUserFlg	& i16Temp; 
		m_ui16MCTimeOutFlg			= m_ui16MCTimeOutFlg			& i16Temp; 
	}

	return; 
}

/*----------------------------------------------------------
Desc : 게임중에 UDP를 보내지 않는 유저를 방에서 나가게 한다. 방장이 보낸다. 호출된 유저는 방에서 나가게 된다. 
----------------------------------------------------------*/
void CRoom::GameKickNormalClient_U(CUserSession * pSession, UINT32 ui32SlotIdx)
{
	if( pSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )			return;		//방장인지.

	if( 0 > ui32SlotIdx )											return;		//슬롯 인덱스가 정확한지.
	if( ui32SlotIdx >= SLOT_MAX_COUNT)								return;		//슬롯 인덱스가 정확한지.
	if( m_Info._SlotInfo[ui32SlotIdx]._State != SLOT_STATE_BATTLE)	return; 	//게임중인 유저인지.

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
	if( m_Info._SlotInfo[ui32SlotIdx]._State != SLOT_STATE_BATTLE)	return; 

	//로그 배틀에서 나가기
	_GiveUpBattle( MAIN_THREAD_IDX, m_Info._pSession[ui32SlotIdx], FALSE, ROOM_ENDTYPE_HACKING );

	//메시지 보내기 
	i3NetworkPacket Packet( PROTOCOL_BATTLE_TIMEOUTCLIENT_ACK );
	m_Info._pSession[ui32SlotIdx]->SendPacketMessage( &Packet );

	return; 
}

INT32 CRoom::ChangeRoomInfo(CUserSession * pSession, ROOM_INFO_BASIC * pRoomInfo, INT8 i8enemyAiCount, INT8 i8difficultyLevel)
{
	INT32 i32Rv = EVENT_ERROR_ROOM_NO_AUTHORITY; 

	
	if( ((0 == m_Info._i32MainSlotIdx) || (m_Info._i32MainSlotIdx == pSession->m_i32SlotIdx)) && m_Info._InfoBasic._State == ROOM_STATE_READY )
	{
		// 플레이 가능한 스테이지이면 룸 모드 설정
		if( CheckPlayableStage( pRoomInfo->_StageID, pRoomInfo->_WeaponFlag, pRoomInfo->_StageMask ) )
		{
			i32Rv = SetRoomMode( pRoomInfo->_StageID, pRoomInfo->_StageMask );
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
		if( GET_STAGETYPE(m_Info._InfoBasic._StageID) != GET_STAGETYPE(pRoomInfo->_StageID) )
		{
			//레디 상태를 모두 풀어준다. 
#ifdef DEF_OBSERVER_MODE
			for(INT32 i = 0; i < SLOT_ALL_COUNT; i ++)
#else
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
			{
				if( m_Info._SlotInfo[i]._State == SLOT_STATE_READY )
				{
					m_Info._SlotInfo[i]._State = SLOT_STATE_NORMAL; 
				}
			}
			_SendSlotInfo(NULL); 
		}

		// Channel 속성(클랜매치 중에 방정보를 잘못 전달하면 에러처리한다)		
		//switch( g_pContextMain->m_pChannelType[ m_Info._ui32ChannelNum ] )
		//{
		//case CHANNEL_TYPE_CLAN:
		//	{	// 클랜 채널일 경우에
		//		if( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask && STAGE_MASK_TYPE_CLANMATCH != pRoomInfo->_StageMask)
		//		{	// 매치 메이킹 일떄
		//			return EVENT_ERROR_ROOM_NO_AUTHORITY;
		//		}
		//	}
		//	break;
		//}

		m_Info._InfoBasic._StageMask		= pRoomInfo->_StageMask;

		//Set Title
		m_Info._InfoBasic._InfoFlag = pRoomInfo->_InfoFlag;
		i3mem::Copy( m_Info._InfoBasic._Title , pRoomInfo->_Title, NET_ROOM_NAME_SIZE ); 
		m_Info._InfoBasic._Title[NET_ROOM_NAME_SIZE - 1] = 0;
		// 패스워드 변경은 [PROTOCOL_ROOM_CHANGE_PASSWD_REQ]를 사용
		if( m_strPasswd[0] != 0 )	m_Info._InfoBasic._InfoFlag |= ROOM_INFO_FLAG_PRIVATE_MODE;
		else						m_Info._InfoBasic._InfoFlag &= ~ROOM_INFO_FLAG_PRIVATE_MODE;
		
		m_Info._InfoBasic._StageID			= pRoomInfo->_StageID;
		m_Info._i32StageType				= GET_STAGETYPE(m_Info._InfoBasic._StageID); 

		// AI 레벨 설정
		m_Info.m_i8difficultyLevel			= i8difficultyLevel;
		m_Info.m_i8currentDifficultyLevel		= i8difficultyLevel;
		m_Info.m_i8enemyAiCount				= i8enemyAiCount;

		//3. 총기 검사
		if( m_Info._InfoBasic._WeaponFlag	!= pRoomInfo->_WeaponFlag )		//무기 
		{
			//유저들의상태를 모두 변경해서 다시 보냄 
#ifdef DEF_OBSERVER_MODE
			for(INT32 i = 0; i < SLOT_ALL_COUNT; i ++)
#else
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
			{
				if( m_Info._SlotInfo[i]._State == SLOT_STATE_READY )
				{
					m_Info._SlotInfo[i]._State = SLOT_STATE_NORMAL; 
				}
			}
			m_Info._InfoBasic._WeaponFlag = pRoomInfo->_WeaponFlag;
		}

		// StageMask 체크
		if( m_Info._InfoBasic._WeaponFlag & LOCK_SNIPER )		m_Info._InfoBasic._StageMask = STAGE_MASK_TYPE_SNIPER;		// 스나이퍼전 검사
		else if( m_Info._InfoBasic._WeaponFlag & LOCK_SHOTGUN )	m_Info._InfoBasic._StageMask = STAGE_MASK_TYPE_SHOTGUN;		// 샷건전 검사		
		else if( m_Info._InfoBasic._WeaponFlag & LOCK_KNUCKLE )	m_Info._InfoBasic._StageMask = STAGE_MASK_TYPE_KNUCKLE;		// 너클전 검사

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

INT32 CRoom::ChangeRoomInfoOption(CUserSession * pSession, ROOM_INFO_ADD * pInfoAdd)
{
	if( NULL == pSession || NULL == pInfoAdd || NULL == m_pRoomMode )	return	EVENT_ERROR_FAIL;

	INT32 i32Rv = EVENT_ERROR_ROOM_NO_AUTHORITY; 
	if( ((0 == m_Info._i32MainSlotIdx) || (m_Info._i32MainSlotIdx == pSession->m_i32SlotIdx)) && m_Info._InfoBasic._State == ROOM_STATE_READY )	
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
		switch( g_pContextMain->m_pui8ChannelType[ m_Info._ui32ChannelNum ] )
		{
		default:
			m_i8IsClanMatch = ZLOG_CLAN_NOT;
			break;
		case CHANNEL_TYPE_PCCAFE_CLAN:
		case CHANNEL_TYPE_CLAN:
			{	// 클랜 채널일 경우에
				if( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask )
				{	// 매치 메이킹 일떄
					// 방 제한
					if(m_Info._InfoAdd._SubType == 0x00)	m_Info._InfoAdd._SubType = 0x02;

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
			break;
		}

		// 스테이지 검사.. 순서 바뀌면 안됩니다.. 여기까지..
		////////////////////////////////////////////////////////////////////////////////

		// 난입 가능한 방인지 체크
		if( m_Info._InfoAdd._LockInterEnter )						m_Info._InfoBasic._InfoFlag	|= ROOM_INFO_FLAG_INTER_ENTER;
		else														m_Info._InfoBasic._InfoFlag	&= ~ROOM_INFO_FLAG_INTER_ENTER;
		// 팀밸런스 체크
		if( m_Info._InfoAdd._LockTeamBalance )						m_Info._InfoBasic._InfoFlag	|= ROOM_INFO_FLAG_TEAM_BALANCE;
		else														m_Info._InfoBasic._InfoFlag	&= ~ROOM_INFO_FLAG_TEAM_BALANCE;

		if( m_Info._pSession[m_Info._i32MainSlotIdx]->IsRealIp() )	m_Info._InfoBasic._InfoFlag |= ROOM_INFO_FLAG_REAL_IP;
		else														m_Info._InfoBasic._InfoFlag &= ~ROOM_INFO_FLAG_REAL_IP;

		//if( m_Info._InfoAdd._LockWeapon & 0x10 )
		//	m_Info._InfoBasic._InfoFlag = m_Info._InfoBasic._InfoFlag | ROOM_INFO_FLAG_WEAPON_RPG7;
		//else 
		//	m_Info._InfoBasic._InfoFlag = m_Info._InfoBasic._InfoFlag & ~ROOM_INFO_FLAG_WEAPON_RPG7;		

		// 라운드 변경을 위해 반드시 처리되어야 합니다.
		if( FALSE == m_pRoomMode->SetRoomOption( m_Info._InfoBasic._StageID ) )
		{
			return EVENT_ERROR_ROOM_NO_AUTHORITY;
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

	if ( ( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask ) ||
		 ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_DOMINATION ) ) 
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

	if( m_Info._SlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP)
	{
		i32SlotIdx = EVENT_ERROR_FAIL; 
		Packet.WriteData(&i32SlotIdx, sizeof( INT32));
	}
	else
	{
		Packet.WriteData( &i32SlotIdx, sizeof( INT32 ));
		Packet.WriteData( m_Info._pSession[i32SlotIdx]->GetNickName(), NET_NICK_NAME_SIZE ); 	//의심
		Packet.WriteData( &m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._nickcolor, sizeof(UINT8) );
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

	if( m_Info._SlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP)
	{
		i32SlotIdx = EVENT_ERROR_FAIL; 
		Packet.WriteData(&i32SlotIdx, sizeof( INT32));
	}
	else
	{
		m_Info._SlotInfo[ i32SlotIdx ]._Rank = (UINT8)m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._rank;

		Packet.WriteData(&i32SlotIdx, sizeof( INT32 ));
		Packet.WriteData(&m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._rank, sizeof(UINT32) ); 	//의심
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

	if( m_Info._SlotInfo[i32SlotIdx]._State < SLOT_STATE_SHOP)
	{
		i32SlotIdx = EVENT_ERROR_FAIL; 
		Packet.WriteData(&i32SlotIdx, sizeof( INT32));
	}
	else
	{
		Packet.WriteData(&i32SlotIdx, sizeof( INT32 ));
		Packet.WriteData(&m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._nickcolor, sizeof(UINT8) ); 	//의심
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
/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::ChangePasswd( CUserSession * pSession, char * pstrNewPasswd )
{
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)return EVENT_ERROR_ROOM_NO_AUTHORITY; 

	if( pstrNewPasswd[0] == '\0' )
	{
		m_Info._InfoBasic._InfoFlag &= ~ROOM_INFO_FLAG_PRIVATE_MODE; 
		m_strPasswd[0] = '\0';
	}
	else
	{
		i3mem::Copy( m_strPasswd, pstrNewPasswd, NET_ROOM_PW);
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
		// AI(훈련전)이면 핑값 처리함.
		if( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask ) { return EVENT_ERROR_ROOM_NO_AUTHORITY; }		
		break; 
	default: return EVENT_ERROR_ROOM;	break; 
	}

#ifdef DEF_OBSERVER_MODE
	for( INT32 i = 0 ; i < SLOT_ALL_COUNT ; i++ )
#else
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
#endif
	{
		m_aRoomUserInfo[i]._ui8Ping = pui8Ping[i]; 
	}

	return EVENT_ERROR_SUCCESS; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::ChangePing_U( CUserSession * pSession, UINT8 ui8Ping )
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx; 
	m_aRoomUserInfo[i32SlotIdx]._ui8Ping = ui8Ping; 
	return EVENT_ERROR_SUCCESS; 
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

#ifdef DEF_OBSERVER_MODE
	for( INT32 i = 0 ; i < SLOT_ALL_COUNT ; i++ )
#else
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
#endif
	{
		SendPing.WriteData( &m_aRoomUserInfo[i]._ui8Ping, sizeof(UINT8) ); 

		if( m_Info._SlotInfo[i]._State != SLOT_STATE_BATTLE ) continue;
		i32TempPing += m_aRoomUserInfo[i]._ui8Ping; 
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
void CRoom::Chatting_U( INT32 i32ThreadIdx, CUserSession * pSession, char * pstrString, UINT16 ui16Type)
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;
	INT32 i32StrLen = i3String::Length(pstrString) + 1; 

	if( i32SlotIdx < 0 || i32SlotIdx >= SLOT_MAX_COUNT )	return;

	i3NetworkPacket SendPacket( PROTOCOL_ROOM_CHATTING_ACK ); 
	SendPacket.WriteData( &ui16Type,	sizeof(UINT16) ); 
	SendPacket.WriteData( &i32SlotIdx,	sizeof(INT32) );		//i32SlotIdx
	SendPacket.WriteData( &i32StrLen,	sizeof(INT32) );		//Str Length
	SendPacket.WriteData( pstrString,	i32StrLen );

	UINT16 ui16SendUser = 0;

	// GM User 의 경우 모든 메시지를 다 받도록 한다.
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State )	continue;
		if( FALSE == m_Info._pSession[i]->IsGMUser() )			continue;
			
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

	if( SLOT_STATE_BATTLE == m_Info._SlotInfo[i32SlotIdx]._State )
	{	// 유저가 게임중일 때
		switch( m_pRoomMode->GetChattingOption() )
		{
		case ROOM_CHATTING_ALL:
			{
				for( INT32 i = i32Start ; i < SLOT_MAX_COUNT ; i += i32Incre )
				{
					if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State ) continue;
						
					ui16SendUser |= (1 << i);
				}
			}
			break;
		case ROOM_CHATTING_GHOST:
			{
				//1. 내상태 점검
				BOOL bSenderObservation = TRUE; 			
				if( IS_USER_START( i32SlotIdx ) )
				{
					if( IS_USER_DEATH( i32SlotIdx ) )	bSenderObservation = TRUE; 
					else								bSenderObservation = FALSE; 
				}

				BOOL bRecverObservation;
				for( INT32 i = i32Start ; i < SLOT_MAX_COUNT ; i += i32Incre )
				{
					if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State ) continue;

					if( MISSION_STATE_BATTLE == m_Info._i32MiStateType )
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
			if( m_Info._SlotInfo[i]._State == SLOT_STATE_CLAN )		continue;;

			if( m_Info._SlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;
			if(	m_Info._SlotInfo[i]._State >= SLOT_STATE_LOAD )		continue;
				
			ui16SendUser |= (1 << i);
		}
	}

	// On 되어있는 유저들만 전달 합니다.
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( 0 == ( (1 << i) & ui16SendUser ) )		continue;;

		m_Info._pSession[i]->SendPacketMessage( &SendPacket );
	}

#if defined( LOCALE_NORTHAMERICA ) || defined( LOCALE_RUSSIA )
	INT32 i32ChattingType = ZLOG_CHATTING_TYPE_ROOM;
	g_pModuleContextNC->LogSendMessage( i32ThreadIdx, LOG_MESSAGE_U_CHATTING, pSession, &i32ChattingType, pstrString );
#endif
	return; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
INT32 CRoom::ChangeSlot_U( CUserSession * pSession, INT32 i32SlotIdx, BOOL bOpen )
{	
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)return EVENT_ERROR_ROOM_NO_AUTHORITY; 

	if(bOpen)	//열기
	{
#ifdef DEF_OBSERVER_MODE
		// 일반 슬롯
		if(0 <= i32SlotIdx && SLOT_MAX_COUNT > i32SlotIdx)
		{
			// 4vs4 모드에서 뒤쪽 8명 슬롯은 열 수 없다.
			if (STAGE_SLOT_MODE_4VS4 == GET_STAGESLOT(m_Info._InfoBasic._StageID))
			{
				if (8 <= i32SlotIdx)
				{
					return EVENT_ERROR_ROOM_NO_AUTHORITY;
				}
			}

			if(m_Info._SlotInfo[i32SlotIdx]._State == SLOT_STATE_CLOSE)
			{
				m_Info._SlotInfo[i32SlotIdx].Reset();
				m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_EMPTY;
				m_Info._InfoBasic._PersonPlayerMax++; 
			}
		}
		// 옵저버 슬롯
		else if(SLOT_MAX_COUNT <= i32SlotIdx && SLOT_ROOM_VIEW_COUNT > i32SlotIdx)
		{
			if(m_Info._SlotInfo[i32SlotIdx]._State == SLOT_STATE_CLOSE)
			{
				m_Info._SlotInfo[i32SlotIdx].Reset();
				m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_EMPTY;
				m_Info._InfoBasic._PersonObserverMax++; 
			}
		}
#else
		// 4vs4 모드에서 뒤쪽 8명 슬롯은 열 수 없다.
		if (STAGE_SLOT_MODE_4VS4 == GET_STAGESLOT(m_Info._InfoBasic._StageID))
		{
			if (8 <= i32SlotIdx)
			{
				return EVENT_ERROR_ROOM_NO_AUTHORITY;
			}
		}

		if(m_Info._SlotInfo[i32SlotIdx]._State == SLOT_STATE_CLOSE)
		{
			m_Info._SlotInfo[i32SlotIdx].Reset();
			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_EMPTY;
			m_Info._InfoBasic._PersonPlayerMax++; 
		}
#endif
	}
	else		//닫기
	{		
		if( m_Info._SlotInfo[i32SlotIdx]._State > SLOT_STATE_CLOSE)
		{
			//로그 
			//g_pModuleContextNC->LogSendMessage( pSession->m_WorkThreadIdx, LOG_MESSAGE_U_R_KICK_LEAVE_ROOM, pSession, this); 이녀석은 게임중 강제 퇴장입니다.

			// 클랜 매치 메이킹전 이라면 강퇴 할수 없다.
			if( (CHANNEL_TYPE_CLAN != g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum] &&
				CHANNEL_TYPE_PCCAFE_CLAN !=g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum])	||
				STAGE_MASK_TYPE_CLANMATCH != m_Info._InfoBasic._StageMask ) 
			{
				//강퇴합니다. 
				i3NetworkPacket SendPacket(PROTOCOL_SERVER_MESSAGE_KICK_PLAYER); 
				m_Info._pSession[i32SlotIdx]->SendPacketMessage(&SendPacket); 
				m_Info._pSession[i32SlotIdx]->m_bForceRoomLeave = TRUE; 
			}
		}
		else
		{
#ifdef DEF_OBSERVER_MODE
			// 일반 슬롯
			if(0 <= i32SlotIdx && SLOT_MAX_COUNT > i32SlotIdx)
			{
				if(m_Info._SlotInfo[i32SlotIdx]._State != SLOT_STATE_CLOSE)
				{
					m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_CLOSE; 
					m_Info._InfoBasic._PersonPlayerMax--; 
				}
			}
			// 옵저버 슬롯
			else if(SLOT_MAX_COUNT <= i32SlotIdx && SLOT_ROOM_VIEW_COUNT > i32SlotIdx)
			{
				if(m_Info._SlotInfo[i32SlotIdx]._State != SLOT_STATE_CLOSE)
				{
					m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_CLOSE; 
					m_Info._InfoBasic._PersonObserverMax--; 
				}
			}
#else
			if(m_Info._SlotInfo[i32SlotIdx]._State != SLOT_STATE_CLOSE)
			{
				m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_CLOSE; 
				m_Info._InfoBasic._PersonPlayerMax--; 
			}
#endif
		}
	}
	_SendSlotInfo( NULL ); 
	return EVENT_ERROR_SUCCESS; 
}

/*-----------------------------------------------------------
Name : 
Desc : 
-----------------------------------------------------------*/
BOOL CRoom::_CheckRespawn( INT32 SlotIdx )
{
	// 2012-09-06 서동권

	UINT32	ui32Count = m_Info._InfoAdd._NowRoundCount + m_Info._aM_UserScore[SlotIdx].m_DeathCount;

	switch( m_Info._i32StageType )
	{
	// 한번 죽으면 리스폰이 안되는 라운드전에서는 라운드 카운트 보다 리스폰 횟수가 많을 수 없다.
	case STAGE_TYPE_ANNIHILATION	:
	case STAGE_TYPE_BOMB			:
		if( m_Info._aui32RespawnCount[SlotIdx] > m_Info._InfoAdd._NowRoundCount )
			return FALSE;
		break;
	// 라운드가 끝나기 전까지 리스폰이 되는 라운드전에서는 라운드 카운트 + 데스 카운트 보다 리스폰 횟수가 많을 수 없다.
	case STAGE_TYPE_CROSSCOUNT		:
	case STAGE_TYPE_ESCAPE			:
		ui32Count += m_Info._ai32AcidBombingCount[SlotIdx]; // 애시드로 자폭한적이 있다면 자폭횟수를 더해줍니다. 애시드 자폭은 DeathCount 를 증가시키지 않기 때문 
	case STAGE_TYPE_DESTROY			:
	case STAGE_TYPE_DEFENCE			:
		if( m_Info._aui32RespawnCount[SlotIdx] > ui32Count )
			return FALSE;
		break;
	// 데스매치에서는 데스보다 리스폰 횟수가 많을 수 없다.
	case STAGE_TYPE_TUTORIAL		:
	case STAGE_TYPE_DEATHMATCH		:
		if( m_Info._aui32RespawnCount[SlotIdx] > m_Info._aM_UserScore[SlotIdx].m_DeathCount )
			return FALSE;
		break;
	}

	return TRUE;
}

INT32 CRoom::ReSpawnBattle_U( CUserSession* pSession ) 
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;

	if( SLOT_MAX_COUNT <= i32SlotIdx )									return EVENT_ERROR_SUCCESS;
	if( false == m_pRoomMode->CheckRespawn( i32SlotIdx ) )				return EVENT_ERROR_SUCCESS;
	// 리스폰 가능 상태인지 체크
	if( m_abAlive[ i32SlotIdx ] )										return EVENT_ERROR_SUCCESS;
	//배틀중이 아니면 리스폰을 보내지 않는다. 0923 클라이언트 크래쉬로 추가된 코드
	if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i32SlotIdx]._State )		return EVENT_ERROR_SUCCESS;

	// 연속킬 Reset
	m_Info.m_aui8A_KillCount[ i32SlotIdx ]		= 0;
	m_Info.m_aui8A_HeadShot[ i32SlotIdx ]		= 0;
	m_Info.m_aui8A_SlugShot[ i32SlotIdx ]		= 0;
	m_Info._aui8A_Mission[i32SlotIdx]	= 0;
	m_Info._aui32RespawnCount[i32SlotIdx]++;			// Add Respawn Count
	pSession->m_ui32GrenadeCount		= 0;

	ReSpawnReq( i32SlotIdx );

	// Grenade Check
	bool bIncreaseGrenade = false;
	if( SLOT_ITEM_INCREASE_GRENADE_SLOT & m_Info._SlotInfo[ i32SlotIdx ]._Item	)
	{
		bIncreaseGrenade = true;
	}
	switch( pSession->m_aui32UseWeaponList[ WEAPON_USAGE_THROWING ] )
	{
	case 803007008:	// K-413 Dual Grenade
	case 803007018: // K-413 Dual Grenade Gold
		if( bIncreaseGrenade )	pSession->m_ui32GrenadeMaxCount = 4;
		else					pSession->m_ui32GrenadeMaxCount = 2;
		break;
	case 803007009:	// Chocolate Grenade
		if( bIncreaseGrenade )	pSession->m_ui32GrenadeMaxCount = 3;
		else					pSession->m_ui32GrenadeMaxCount = 2;
		break;
	case 803007001:	// k-400
	case 803007004:	// C-5
	case 803007006:	// C-5 D
	case 803007019: // ketupat
		if( bIncreaseGrenade )	pSession->m_ui32GrenadeMaxCount = 2;
		else					pSession->m_ui32GrenadeMaxCount = 1;
		break;	
	default:
		pSession->m_ui32GrenadeMaxCount = 1;
		break;
	}

	//로그 리스폰
	if( LOCALE_CODE_THAILAND != pSession->m_ui8LocaleCode )
	{
		g_pModuleContextNC->LogSendMessage( pSession->m_WorkThreadIdx, LOG_MESSAGE_U_R_RESPAWN, pSession, this ); 
	}

	// 무기 핵 체크 - 서동권
	// 무기 리스트를 추가합니다.
	AddHackCheckWeapon( pSession );

	return EVENT_ERROR_SUCCESS; 
}

BOOL CRoom::ReSpawnBattleForAi_U( CUserSession * pSession, INT32 i32SlotIdx)
{		
	if( pSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )					return FALSE;		// 방장인지 체크
	if( 0 > i32SlotIdx || (SLOT_MAX_COUNT-1) < i32SlotIdx )					return FALSE;		// Slot번호가 정상인지 확인
	if( ( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )		&&
		( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_DOMINATION ) )	return FALSE;		// AI전인지 확인
	if( (i32SlotIdx % 2) == (m_Info._i32MainSlotIdx%2) )					return FALSE;		// AI 슬롯인지 확인

	if( false == m_pRoomMode->CheckRespawnAi( i32SlotIdx ) )				return TRUE;		// Ai 부활 가능한지 확인

	m_Info.m_aui32AILevel[i32SlotIdx]			= m_Info.m_i8currentDifficultyLevel;
	// 연속킬 Reset
	m_Info.m_aui8A_KillCount[ i32SlotIdx ]		= 0;
	m_Info.m_aui8A_HeadShot[ i32SlotIdx ]		= 0; 
	m_Info.m_aui8A_SlugShot[ i32SlotIdx ]		= 0; 
	m_Info._aui8A_Mission[i32SlotIdx]			= 0;
	m_Info._aui32RespawnCount[ i32SlotIdx ]++;		// Add Respawn Count
	SET_USER_ALIVE( i32SlotIdx );					// 유저 Alive 처리

	//Packet 보내기 
	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_RESPAWN_FOR_AI_ACK);
	SendPacket.WriteData( &i32SlotIdx,		sizeof(INT32) );
	m_pRoomMode->AddRespawnAiPacket( &SendPacket, i32SlotIdx );
	_SendPacketBattlePlayer(&SendPacket);

	m_i32TotalRespawnIdx++;
	return TRUE;
}

void CRoom::GMPauseResult_C( INT32 i32Result )
{
	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_GM_PAUSE_ACK );
	SendPacket.WriteData( &i32Result, sizeof( INT32 ) );

	if ( EV_SUCCESSED( i32Result ) )
	{
		m_bGMPause = TRUE;
		m_ui32ATPauseBegin = g_pContextMain->GetServerTime();
		SendPacket.WriteData( &m_i32PauseUserIdx, sizeof( INT32 ) );
		_SendPacketBattlePlayer( &SendPacket );
	}
	else
	{
		m_Info._pSession[m_i32PauseUserIdx]->SendPacketMessage( &SendPacket );
	}
}

/*----------------------------------------------------------
Name : 
Desc : 
----------------------------------------------------------*/
// 무기 핵 체크 - 서동권
BOOL CRoom::CheckDeathWeapon( CUserSession* pSession, UINT32 ui32OrgWeaponID )
{
	if( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL )		return TRUE; // 튜토리얼 모드는 넘어갑니다.
	if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )					return TRUE; // AI 모드는 넘어갑니다.

	UINT32 ui32WeaponID = ui32OrgWeaponID;
	switch( ui32OrgWeaponID )
	{
	case 100003047:													// 맵 한정으로 주어지는 기본 무기 K2 -- 언제 사용하는지 모르겠네요..
		{
			return TRUE;
		}
		break;
	case 300005025:													// 맵 한정으로 주어지는 기본 무기 저격
		{
			if( GET_ROOM_INFO_WEAPON_SNIPER( m_Info._InfoBasic._WeaponFlag ) )		return TRUE;
			else																	return FALSE;
		}
		break;
	case 400006007:													// 맵 한정으로 주어지는 기본 무기 샷건
		{
			if( GET_ROOM_INFO_WEAPON_SHOTGUN( m_Info._InfoBasic._WeaponFlag ) )		return TRUE;
			else																	return FALSE;
		}
		break;
	case 702023001:													// 맵 한정으로 주어지는 기본 무기 너클이지만 팔기도 합니다..
		{
			if( GET_ROOM_INFO_WEAPON_KNUCKLE( m_Info._InfoBasic._WeaponFlag ) )		return TRUE;
		}
		break;
	case 1600016002:												// 맵 한정으로 주어지는 기본 무기 RPG
		{
			if( GET_ROOM_INFO_WEAPON_RPG7( m_Info._InfoBasic._WeaponFlag ) )		return TRUE;
			else																	return FALSE;
		}
		break;
	case 3011003:													// RPG - 사용 가능한 맵인경우 통과
		{
			STAGE_ORDINAL_ID ui32StageID = GET_STAGEORD( m_Info._InfoBasic._StageID );
			if( ui32StageID == STAGE_ORD_OUTPOST )									return TRUE;
			else if( ui32StageID == STAGE_ORD_HELISPOT )							return TRUE;
			else if( ui32StageID == STAGE_ORD_BLACKPANTHER )						return TRUE;
			else if( ui32StageID == STAGE_ORD_SAINTMANSION )						return TRUE;
			else																	return FALSE;
		}
		break;
	case 3011001:													// K2 유탄
		{
			STAGE_ORDINAL_ID ui32StageID = GET_STAGEORD( m_Info._InfoBasic._StageID );
			if( ui32StageID == STAGE_ORD_BLACKPANTHER )								return TRUE;
			else ui32WeaponID = 100003011;			break;	// K2 유탄
		}
		break;
	case 17001:																		return TRUE;	// 헬기 머신건
	case 1011003:																	return TRUE;	// 헬기에 타고있는 적을 RPG 로 죽였을때
	case 702001001:																	return TRUE;	// 자살일때는 이 아이템으로..
	case 200018003:		ui32WeaponID = 200004031;			break;	// Kriss S.V D 는 듀얼과 ItemID가 다르게 온다.
	
	case 3011005:		ui32WeaponID = 100003044;			break;	// M4 SR-16 Lv3	
	case 3011006:		ui32WeaponID = 100003045;			break;	// M4 SR-16 F.C
	case 3011007:		ui32WeaponID = 100003052;			break;	// FAMAS G2 M203
	case 3011008:		ui32WeaponID = 100003058;			break;	// AK47 F.C
	case 3011010:		ui32WeaponID = 100026001;			break;	// Compound Bow
	case 3011011:		ui32WeaponID = 601002026;			break;	// HK69 
	case 602013001:		ui32WeaponID = 601013001;			break;	// P99&HAK 
	case 602013002:		ui32WeaponID = 601013002;			break;	// P99&HAK Sl.
	case 602013003:		ui32WeaponID = 601013003;			break;	// P99&HAK D.
	case 602013004:		ui32WeaponID = 601013004;			break;	// P99&HAK Sl D.
	case 702001006:		ui32WeaponID = 702001007;			break;	// Mini Axe
	case 702001020:		ui32WeaponID = 702001018;			break;	// 탄도단검
	}

	I3TRACE( "Weapon:%d, Org:%d\n", ui32WeaponID, ui32OrgWeaponID );
	INT32 i32ItemUsage = GET_ITEM_FLAG_USAGE( ui32WeaponID );
	// 기본적으로 현재 사용중인 아이템인지 한번 체크합니다.
	switch( i32ItemUsage )
	{
	case WEAPON_USAGE_PRIMARY:
		if( ui32WeaponID == pSession->m_aui32UseWeaponList[ WEAPON_USAGE_PRIMARY ] )								return TRUE;
		else if( pSession->m_aui32UseWeaponList[ WEAPON_USAGE_PRIMARY ] == 200004052 && ui32WeaponID == 200004047 )	return TRUE;
		break;
	case WEAPON_USAGE_SECONDARY:	if( ui32WeaponID == pSession->m_aui32UseWeaponList[ WEAPON_USAGE_SECONDARY ] )	return TRUE;
		break;
	case WEAPON_USAGE_MELEE:
		if( ui32WeaponID == pSession->m_aui32UseWeaponList[ WEAPON_USAGE_MELEE ] )									return TRUE;
		break;
	case WEAPON_USAGE_THROWING:		
		if( ui32WeaponID == pSession->m_aui32UseWeaponList[ WEAPON_USAGE_THROWING ] )								return TRUE;
		else if( pSession->m_aui32UseWeaponList[ WEAPON_USAGE_THROWING ] == 803007006 && ui32WeaponID == 803007004 )	return TRUE;	// C-5 D 는 C-5 로 옴.. ㅡㅡ;; 이게 뭐야..
		break;
	case WEAPON_USAGE_ITEM:			if( ui32WeaponID == pSession->m_aui32UseWeaponList[ WEAPON_USAGE_ITEM ] )		return TRUE;
		break;
	}

	// 가지고 있지 않은 무기일 경우 자세하게 체크 합니다.
	switch( i32ItemUsage )
	{
	case WEAPON_USAGE_PRIMARY:
	case WEAPON_USAGE_SECONDARY:
		// 내가 or 다른 유저가 사용했던 무기를 주웠는지 확인 합니다.
		for( UINT32 i = 0; i < m_ui8RoomWeaponListCount; ++i )
		{
			if( ui32WeaponID == m_aui32RoomWeaponList[i] )							return TRUE;	// 해킹무기 아님
		}		
		break;
	case WEAPON_USAGE_MELEE:
	case WEAPON_USAGE_THROWING:
	case WEAPON_USAGE_ITEM:
		// 이 무기의 경우 내것이 아니면 사용 할 수 없습니다.
		for( INT32 i = 0 ; i < pSession->m_i32InvWeaponCount ; i++ )
		{
			if( ui32WeaponID == pSession->m_aInvWeapon[i]._ui32ItemID )				return TRUE;
		}
		break;
	}

	switch( GET_ITEM_FLAG_TYPE( ui32WeaponID ) )
	{
	case WEAPON_CLASS_DINO:
	case WEAPON_CLASS_DINO_LEFT_SCRATCH:
	case WEAPON_CLASS_TREX:
	case WEAPON_CLASS_STING:
	case WEAPON_CLASS_ELITE_RIGHT_SCRATCH:
	case WEAPON_CLASS_ELITE_LEFT_SCRATCH:
		return TRUE;
	}

	return FALSE;
}

INT32 CRoom::DeathBattle( UINT32 ui32ThreadIdx, CUserSession * pMainSession, DEATH_INFO_HEADER * pHeader, DEATH_INFO_CHARA * pData, UINT8 ui8RoundNum )
{	
	// 배틀 중이 아니면 데스 패킷이 의미 없다.
#ifdef USE_RESPAWN_SKIP
	if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )	return EVENT_ERROR_SUCCESS;
#endif
	if( m_Info._InfoBasic._State != ROOM_STATE_BATTLE )		return EVENT_ERROR_SUCCESS;
	if( m_Info._InfoAdd._NowRoundCount != ui8RoundNum )		return EVENT_ERROR_SUCCESS;

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return EVENT_ERROR_ROOM_NO_AUTHORITY; 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )	return EVENT_ERROR_ROOM_NO_AUTHORITY;// 방장만이 보낼수 있습니다.
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		//AI 전만 예외처리 합니다. 
		if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
		{
			if(pMainSession == NULL)return EVENT_ERROR_ROOM_NO_AUTHORITY; 
			if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return EVENT_ERROR_ROOM_NO_AUTHORITY;// 방장만이 보낼수 있습니다.
		}
		else 
		{
			pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		}
		break; 
	default: return EVENT_ERROR_ROOM;	break; 
	}

	//Set Type
	BOOL	bPiercingShot	= FALSE;
	BOOL	bMassKill		= FALSE;
	BOOL	bBombing		= FALSE;
	UINT8	ui8KillTypeHigh = GET_KILLTYPE_HIGH( pHeader->_nKillType );
	UINT8	ui8KillTypeLow	= GET_KILLTYPE_LOW( pHeader->_nKillType );

	// High 체크
	if( CHARA_KILL_HIGH_BOMBING & ui8KillTypeHigh )
	{
		bBombing = TRUE;
	}
	// Low 체크
	switch( ui8KillTypeLow )
	{
	case CHARA_KILL_DEFAULT		: break;								//Normal  이 부분은 그냥 셋 합니다.
	case CHARA_KILL_PIERCING	: bPiercingShot		= TRUE;		m_Info._aui16M_PiercingShot[ pHeader->_nUserIdx ]++;	break;	//Piercing
	case CHARA_KILL_MASS		: bMassKill			= TRUE;		m_Info._aui16M_MassKill[ pHeader->_nUserIdx ]++;		break;	//Mass
	default:
		I3TRACE("[CRoom::DeathBattle] Kill Type Error \n");
		return EVENT_ERROR_FAIL;
	}

#ifndef NEW_EXPPOINT
	TYPE_ROOM_KILL_EXPPOINT* pKillExpPoint = m_pRoomMode->GetKillExpPoint();
#endif
	UINT16	ui16GainPoint	= 0;

	//0x00		0		0		0		0		0		0 
	//message	Now		Pre		Weapon	ui32State	Kill	Death
	INT32	i32KillIdx		= pHeader->_nUserIdx; 
	if (i32KillIdx < 0 || i32KillIdx >= SLOT_MAX_COUNT) return EVENT_ERROR_SUCCESS;
	UINT32	ui32WeaponNum	= pHeader->_WeaponInfo;
	UINT8	ui8PreKillCount	= m_Info.m_aui8A_KillCount[i32KillIdx];
	BOOL	bMassKillMission = FALSE; // for문에서 MassKill 을 계속 체크해서 한번만 체크하기 위해 추가하였습니다. - 2011.05.19 - 서동권

	//무기에 대해서 검사하는 것이 있어야 합니다. 기존에 있는 무기인지를 검사하는 무기 
	UINT32 ui32Weapon_Slot	= GET_ITEM_FLAG_USAGE( ui32WeaponNum );
	UINT32 ui32Weapon_Class = GET_ITEM_FLAG_TYPE( ui32WeaponNum );
	UINT32 ui32Weapon_Num   = GET_ITEM_FLAG_NUMBER( ui32WeaponNum );

	if( ( WEAPON_CLASS_COUNT <= ui32Weapon_Class ) || ( WEAPON_USAGE_COUNT <= ui32Weapon_Slot ) )
	{
		I3TRACE("[CRoom::DeathBattle] Weapon Info Error \n"); 		
		return EVENT_ERROR_FAIL; 
	}

	char strHackLog[ MAX_PATH ];
	bool bHackUser = false;


	/*
	// Grenade Count Check
	if( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )
	{
		// WP Smoke 들은 예외처리 합니다. - 서동권
		if( WEAPON_CLASS_THROWING == ui32Weapon_Class && ui32WeaponNum != 904007005 && ui32WeaponNum != 904007007 && ui32WeaponNum != 904007012 && ui32WeaponNum != 904007017 )
		{
			m_Info._pSession[i32KillIdx]->m_ui32GrenadeCount++;
			if( m_Info._pSession[i32KillIdx]->m_ui32GrenadeCount > m_Info._pSession[i32KillIdx]->m_ui32GrenadeMaxCount )
			{
				i3String::Format( strHackLog, "Grenade Count Error : UID:%I64d", m_Info._pSession[ i32KillIdx ]->m_i64UID );
				bHackUser = true;
			}
		}
	}
	*/

	if( (g_pContextMain->m_i8HackCheck & 0x4) && (m_ui8BattleType != BATTLE_SERVER_TYPE_DEDICATION) ) // 게임서버에서는 듀얼 무기를 검사할 수 없습니다(yjha)
	{
		if( FALSE == CheckDeathWeapon( m_Info._pSession[i32KillIdx], ui32WeaponNum ) )
		{
			i3String::Format(strHackLog, MAX_PATH, "DeathBattle - Hack User Kick, Hack Type : 4, UID : %I64d , Stage Name : %s , Channel : %d , Weapon : %d )", m_Info._pSession[ i32KillIdx ]->m_i64UID , g_pContextMain->GetStageName( (STAGE_ID)m_Info._InfoBasic._StageID ) , m_Info._pSession[ i32KillIdx ]->GetChannelNumber(), ui32WeaponNum );
			bHackUser = true;
		}
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
		// 0xF0000000		0x0F000000				0x00FFFF00	0x000000F0		0x0000000F
		// ui32DeathMyState	ui32DeathEnemyState		message,	ui32DeathType,	DeathIdx		
		ui32DeathMyState	= GET_DEATHINFO_MY_TYPE(	pData[i]._DeathInfo );
		ui32DeathEnemyState	= GET_DEATHINFO_ENEMY_TYPE( pData[i]._DeathInfo );
		ui32DeathType		= GET_DEATHINFO_DEATH_TYPE(	pData[i]._DeathInfo );
		i32DeathIdx			= GET_DEATHINFO_IDX_TYPE(	pData[i]._DeathInfo );
		if (i32DeathIdx < 0 || i32DeathIdx >= SLOT_MAX_COUNT) return EVENT_ERROR_SUCCESS;

		if( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )
		{
			m_Info._pSession[i32DeathIdx]->m_ui32BattleDeathTime =
#ifdef USE_GM_PAUSE
																	GetRoomBattleTime();
#else
																	g_pContextMain->GetServerTime();
#endif
		}

		// 계속 리스폰 되는 모드는 살아있는 시간만 배틀 타임으로 계산
		if( m_pRoomMode->GetRespawnOption() == ROOM_RESPAWN_CONTINUE )
		{
			if( m_Info._pSession[i32DeathIdx]->m_ui32RespawnTime > 0 )
			{
				m_Info._pSession[i32DeathIdx]->m_ui32AccActualBattleTime +=
#ifdef USE_GM_PAUSE
																			GetRoomBattleTime()
#else
																			g_pContextMain->GetServerTime()
#endif
																											- m_Info._pSession[i32DeathIdx]->m_ui32RespawnTime;
			}
		}

		i8DeathWeaponClass	= pData[i]._ui8Weapon;
		m_aKilledUserWeapon[i32KillIdx]._ai8WeaponClass[m_aKilledUserWeapon[i32KillIdx]._i8Idx] = i8DeathWeaponClass;
		m_aKilledUserWeapon[i32KillIdx].IncreaseIdx();

		BOOL bChainStoppper	= FALSE;
		BOOL bHeadShot		= FALSE;
		BOOL bChainHeadShot = FALSE;
		BOOL bChainSlugger	= FALSE;
		BOOL bSuicide		= FALSE;
		bUpdateData			= TRUE;

		// 죽어있는 유저는 또 죽일수 없다.
		if( IS_USER_DEATH( i32DeathIdx ) || m_abDeath[i32DeathIdx] )				return EVENT_ERROR_SUCCESS;

		// 라운드 게임일경우 라운드 종료 시점에 킬 처리를 하지 않습니다.
		if(  MISSION_STATE_BATTLE != m_Info._i32MiStateType )
		{
			bUpdateData = FALSE;
		}

		//킬인덱스 확인 
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i32KillIdx]._State )
		{
			// AI 모드의 경우 AI NPC가 죽일때 패킷을 던진다.
			if( (STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )	&&
				GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_DOMINATION )
			{
				I3TRACE("[CRoom::DeathBattle] KillIdx  Error \n"); 				
				bUpdateData = FALSE; 
			}
		}

		//데쓰 인덱스 확인 
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i32DeathIdx]._State )
		{
			// AI 모드의 경우 AI NPC가 죽을때 패킷을 던진다.
			if( (STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )	&&
				GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_DOMINATION )
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
		{//개인 점수 계산 

			//로그 킬 데스 					
			LOG_KILL_DEATH_INFO	LogInfo; 			
			LogInfo.ui32WeaponNum	= ui32WeaponNum; 
			LogInfo.m_ui32DeathType	= ui32DeathType;

			LogInfo.r32Kill_X		= pHeader->_KillPosition[0]; 
			LogInfo.r32Kill_Y		= pHeader->_KillPosition[1]; 
			LogInfo.r32Kill_Z		= pHeader->_KillPosition[2]; 
			LogInfo.r32Death_X		= pData[i]._DeathPosition[0];
			LogInfo.r32Death_Y		= pData[i]._DeathPosition[1];
			LogInfo.r32Death_Z		= pData[i]._DeathPosition[2];

			if( (STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )	&&
				GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_DOMINATION )
			{
				// Death 로그에 Kill 과 Death 같이 묶어서 남김.
				g_pModuleContextNC->LogSendMessage(	ui32ThreadIdx, LOG_MESSAGE_U_R_DEATH, m_Info._pSession[i32DeathIdx], m_Info._pSession[i32KillIdx], this, &LogInfo );
			}

			//Kill Idx 자살이면 계산하지 않음 
			if( i32KillIdx == i32DeathIdx )			
			{//자살
				bSuicide = TRUE;		//자살
			}
			else
			{	
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_KILL,					 &ui32WeaponNum );
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_KILL_WEAPONCLASS,		 &m_aKilledUserWeapon[i32KillIdx] );
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_DOUBLE_KILL_WEAPONCLASS, &m_aKilledUserWeapon[i32KillIdx] );
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_TRIPLE_KILL_WEAPONCLASS, &m_aKilledUserWeapon[i32KillIdx] );
				SetUserMissionCard( m_Info._pSession[i32DeathIdx], ACTION_ID_DEATH );

				if( m_Info._i32StageType == STAGE_TYPE_ESCAPE && m_Info._MiDinoInfo.IsDinoIdx(i32KillIdx) )
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
							// AI 모드 Headshot Count 점수
							m_Info._aui16AccTrainingScore[ i32KillIdx ] += 4;
						}
						else
						{	// AI 모드 Headshot Count 점수
							m_Info._aui16AccTrainingScore[ i32KillIdx ] += 2;
						}

						bHeadShot = TRUE;
						m_Info.m_aui8A_HeadShot[ i32KillIdx ]++;										// 헤드샷 기록
						m_Info._aui16M_Headshot[ i32KillIdx ]++;
					}
					break;			
				default					:	I3TRACE("DeathBattle Error Errrrrrrror \n"); break; 
				}

				if( WEAPON_USAGE_SECONDARY == ui32Weapon_Slot )
				{	// AI 모드 보조 무기 점수
					m_Info._aui16AccTrainingScore[ i32KillIdx ] += 1;	
				}
				else if ( WEAPON_USAGE_THROWING == ui32Weapon_Slot )
				{	// AI 모드 투척 무기 점수
					m_Info._aui16AccTrainingScore[ i32KillIdx ] += 2;
				}
				//2. Check Slug 				
				if( WEAPON_USAGE_MELEE == ui32Weapon_Slot )
				{
					if( 0 < m_Info.m_aui8A_SlugShot[ i32KillIdx ] )
					{
						bChainSlugger = TRUE;

						m_Info._aui16M_ChainSlugger[ i32KillIdx ]++;
						// AI 모드 Slug Count 점수
						m_Info._aui16AccTrainingScore[ i32KillIdx ] += 5;
					}
					else
					{
						// AI 모드 Slug Count 점수( 보태기)
						m_Info._aui16AccTrainingScore[ i32KillIdx ] += 2;
					}

					m_Info.m_aui8A_SlugShot[ i32KillIdx ]++;

					// AI 모드 근접 무기 점수(기본)
					m_Info._aui16AccTrainingScore[ i32KillIdx ] += 3;
				}
				else
				{
					m_Info.m_aui8A_SlugShot[ i32KillIdx ] = 0; 
				}

				m_Info._aM_UserScore[ i32KillIdx ].m_KillCount++;				//Add KillIndex
				m_Info._aUserScoreRound[ i32KillIdx ].m_KillCount++;

				// AI 모드 기본 킬 점수
				if (TRAINING_DIFFICULTY_LEVEL_NONE < m_Info.m_i8currentDifficultyLevel && m_Info.m_i8currentDifficultyLevel <= TRAINING_DIFFICULTY_LEVEL_HIGH)
				{
					m_Info._aui16AccTrainingScore[ i32KillIdx ] += (UINT16)g_ai32trainingKillPoint[m_Info.m_i8currentDifficultyLevel];
				}
				else
				{
					m_Info._aui16AccTrainingScore[ i32KillIdx ] += 10;
				}

				//Cal Money 먼저 계산 되어야함. 위치 중요
				switch( m_Info.m_aui8A_KillCount[i32KillIdx] )
				{
				case 1:
					{
						m_Info._aui16M_AccKill_1[ i32KillIdx ]++;
#ifndef NEW_EXPPOINT
						ui16GainPoint	+= pKillExpPoint->_i16MultiPt;
#endif

						// AI 모드 Kill Count 점수
						m_Info._aui16AccTrainingScore[ i32KillIdx ] += 2;

						SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_DOUBLE_KILL, &ui32WeaponNum );
					}
					break; 
				case 2:	
					{
						m_Info._aui16M_AccKill_2[ i32KillIdx ]++;
#ifndef NEW_EXPPOINT
						ui16GainPoint	+= (pKillExpPoint->_i16MultiPt*2);
#endif

						// AI 모드 Kill Count 점수
						m_Info._aui16AccTrainingScore[ i32KillIdx ] += 4;

						SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_TRIPLE_KILL, &ui32WeaponNum );
					}
					break; 
				case 3:	
				case 4:	
					{
						m_Info._aui16M_AccKill_3[ i32KillIdx ]++;
#ifndef NEW_EXPPOINT
						ui16GainPoint	+= (pKillExpPoint->_i16MultiPt*3);
#endif

						// AI 모드 Kill Count 점수
						m_Info._aui16AccTrainingScore[ i32KillIdx ] += 7;

						SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINKILLER );
					}
					break; 
				}

				// 각 모드별 추가 Kill Bouns
#ifdef NEW_EXPPOINT
				ui16GainPoint = 
#endif
								m_pRoomMode->UserKillBouns( i32KillIdx, ui32DeathType, ui32DeathMyState, ui32DeathEnemyState, GetRoomBattleTime() );

				//Set Max Count Number				
				m_Info.m_aui8A_KillCount[i32KillIdx]++;
				if( m_Info.m_aui8A_KillCount[i32KillIdx] > 4 )	m_Info.m_aui8A_KillCount[i32KillIdx] = 4;

				//Chain Stopper
				if( m_Info.m_aui8A_KillCount[ i32DeathIdx ] == 4)
				{
					bChainStoppper = TRUE;
					// AI 모드 Chain stopper Count 점수
					m_Info._aui16AccTrainingScore[ i32KillIdx ] += 5;
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

				if(FALSE == m_Info._SlotInfo[i32DeathIdx]._bIsGmObserverMode)
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
				}
			}
			else
			{	// 자살일 경우
				if( false == bBombing )
				{	// 엑시드의 경우 폭발로 인해 자신도 함께 죽은 경우 Death 를 증가 시키지 않습니다.
					//Team 전적 계산
					if(FALSE == m_Info._SlotInfo[i32DeathIdx]._bIsGmObserverMode)
					{
						INT32 i32TeamIdx = i32DeathIdx % 2;
						m_Info._aTeamScoreTotal[ i32TeamIdx ].m_DeathCount++;
						m_Info._aTeamScoreRound[ i32TeamIdx ].m_DeathCount++;

						//죽은 유저 점수 계산
						m_Info._aM_UserScore[ i32DeathIdx ].m_DeathCount++;
						m_Info._aUserScoreRound[ i32DeathIdx ].m_DeathCount++;
					}
				}
				else
				{
					// 애시드의 경우 Death 를 증가시키지 않고 Bombing Count 를 세도록 합니다. - 서동권
					m_Info._ai32AcidBombingCount[ i32DeathIdx ]++;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////
		//Set Data
		pData[i]._DeathInfo = pData[i]._DeathInfo & 0xFF0000FF; 

		// 1. Message 	
		if( bPiercingShot )
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_PIERCINGSHOT		; 
#ifndef NEW_EXPPOINT
			ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Pier;
#endif

			m_Info._aui16AccTrainingScore[ i32KillIdx ] += 7;

			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_PIERCING );
		}
		if( bMassKill ) 
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_MASSKILL			; 
#ifndef NEW_EXPPOINT
			ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Mass;
#endif

			m_Info._aui16AccTrainingScore[ i32KillIdx ] += 7;

			// 한명이 여러명을 죽여 MassKill 을 냈을경우 연속적으로 들어오므로
			// 임무완료 체크로는 한번만 할수 있도록 BOOL 값을 줘서 한번만 체크하게 합니다.
			// 2011.05.19 서동권
			if( bMassKillMission == FALSE ) SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_MASS_KILL ); 
			bMassKillMission = TRUE;
		}
		if( bChainStoppper ) 
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_CHAINSTOPPER		; 
#ifndef NEW_EXPPOINT
			ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Stop;
#endif

			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINSTOPPER );
		}
		if( bHeadShot )
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_HEADSHOT;

			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_HEADSHOT, &ui32WeaponNum );
		}
		if( bChainHeadShot ) //****
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_CHAINHEADSHOT	; 
#ifndef NEW_EXPPOINT
			ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Head;
#endif

			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINHEADSHOT );
		}
		if( bChainSlugger ) //****
		{
			pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_CHAINSLUGGER		;
#ifndef NEW_EXPPOINT
			ui16GainPoint	+= pKillExpPoint->_i16MultiPt_Slug;
#endif

			SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_CHAINSLUGGER );
		}

		// Death 시 나의 상태
		switch( ui32DeathMyState )
		{
		case CHARA_DEATH_MY_STATE_ASSIST:
			{
				pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_ASSISTKILL;
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_KILL_ASSIST );
			}
			break;
		}
		// Death 시 적의 상태
		switch( ui32DeathEnemyState )
		{
		case CHARA_DEATH_ENEMY_STATE_DEFENCE:
			{	// 오브젝트 공격 중인 적을 사살
				pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_OBJECTDEFENCE; 	
				SetUserMissionCard( m_Info._pSession[i32KillIdx], ACTION_ID_PROTECT );
			}
			break;
		case CHARA_DEATH_ENEMY_STATE_ESCAPE:
			{
				// 탈출미션 탈출지점 근처에 접근한 적을 죽임
				pData[i]._DeathInfo = pData[i]._DeathInfo | KILLING_MESSAGE_INTERCEPT; 
				I3TRACE("인터셉트 성공");
			}
			break;
		}

		ui32AccMessage = ui32AccMessage |  pData[i]._DeathInfo; 

		m_abDeath[i32DeathIdx] = true;
	}

	if( bHackUser )
	{
#if defined (I3_DEBUG)
		SendNotifyHackUser( i32KillIdx, HACK_CHECK_TYPE_SERVER_G, ui32ThreadIdx, 100000, m_Info._pSession[ i32KillIdx ]->m_ui8LocaleCode ); //<- 이소스는 필요없을수 있어서 아무값이나 넣었음. 
		g_pModuleLogFile->Write_M_Log( ui32ThreadIdx, m_Info._pSession[ i32KillIdx ], strHackLog );
#else
		if( m_Info._pSession[ i32KillIdx ]->m_ui32ATSendKill == 0 )
		{
			UINT8 ui8RandTime = rand() % HACK_USER_DISCONNECT_TIME;
			m_Info._pSession[ i32KillIdx ]->m_ui32ATSendKill		= g_pContextMain->GetServerTime() + ui8RandTime;
			m_Info._pSession[ i32KillIdx ]->m_ui32KillReason		= KILL_REASON_HACK_SERVER_G;
			m_Info._pSession[ i32KillIdx ]->m_ui32HackKillReason	= 0;
			i3String::Format( m_Info._pSession[ i32KillIdx ]->m_strHackNotice, DEF_SIZE_HACK_NOTICE, "%s, RandTime:%d", strHackLog, ui8RandTime );
		}
#endif
	}
	//--------------------------------------------------------------------------------
	//모든 사람들에게 메시지를 보낸다. 	 Death - Kill
	i3NetworkPacket SendPacket(PROTOCOL_BATTLE_DEATH_ACK); 
	SendPacket.WriteData( pHeader,					sizeof(DEATH_INFO_HEADER)); 
	SendPacket.WriteData( pData,					(sizeof(DEATH_INFO_CHARA)	* pHeader->_nCount)); 
	SendPacket.WriteData( m_Info._aTeamScoreTotal,	sizeof(m_Info._aTeamScoreTotal)); 
	SendPacket.WriteData( m_Info._aM_UserScore,		sizeof(m_Info._aM_UserScore)); 
	SendPacket.WriteData( &ui8PreKillCount,			sizeof(UINT8));

	if ( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
	{
		SendPacket.WriteData( &m_Info._aui16AccTrainingScore[i32KillIdx],	sizeof(UINT16));
	}
	else
	{
		SendPacket.WriteData( &ui16GainPoint,	sizeof(UINT16) );
	}

	m_pRoomMode->AddDeathBattlePacket(&SendPacket);

	_SendPacketBattlePlayer(&SendPacket);

	return EVENT_ERROR_SUCCESS; 
}

UINT16 CRoom::_ResultUpdate_M( INT32 i32Win, BOOL bNormalEnd )
{
	UINT16		ui16BattleEndUser = 0x0000;	// 정상 종료 User 확인
	INT32		ai32WinTeam[ TEAM_COUNT ];
	INT32		ai32LoseTeam[ TEAM_COUNT ];
	INT32		ai32DrawTeam[ TEAM_COUNT ];

	// ClanBattle 를 위한 변수
	UINT8				ui8BattleUserCount = 0;
	CLAN_RESULT			aClanResult[ TEAM_COUNT ];
	CLAN_RESULT_USER	aClanResultUser[ SLOT_MAX_COUNT ];
	i3mem::FillZero( aClanResult, sizeof(aClanResult) );

	REAL32 r32BattleCountEndPercent		= 1.0f;		//인원수에 따른 카운트(킬/데스 가중치 Red) 
	REAL32 r32BattleCountEndPercent2	= 1.0f;		//방어미션, 공룡미션일때 킬/데스 가중치 Blue
	REAL32 r32BattleCountEndPercent3	= 1.0f;		//방어미션 일때 임무 가중치 Red
	REAL32 r32BattleCountEndPercent4	= 1.0f;		//방어미션 일때 임무 가중치 Blue
	REAL32 r32SubPercent				= 1.0f;		//비정상 종료 일때

	m_Info._ui32BattleEndTime = g_pContextMain->GetServerTime();

	//1. 인원수에 따른 차감내용
#ifndef NEW_EXPPOINT
	switch( m_Info._i32StageType )
	{
	case STAGE_TYPE_DEFENCE			:
		{
			r32BattleCountEndPercent	= _GetBattleCountEndPercentTeam(0);//Red
			r32BattleCountEndPercent2	= _GetBattleCountEndPercentTeam(1);//Blue

			r32BattleCountEndPercent3	= _GetBattleCountEndPercentTeam2(0);//Red
			r32BattleCountEndPercent4	= _GetBattleCountEndPercentTeam2(1);//Blue
		}
		break;
	case STAGE_TYPE_DESTROY			:
	case STAGE_TYPE_ESCAPE			: 
	case STAGE_TYPE_CROSSCOUNT		:
		{
			r32BattleCountEndPercent	= _GetBattleCountEndPercentTeam(0);//Red
			r32BattleCountEndPercent2	= _GetBattleCountEndPercentTeam(1);//Blue
		}
		break;	
	default:
		{
			r32BattleCountEndPercent	= _GetBattleCountEndPercent();
		}
		break;
	}
#else
	r32BattleCountEndPercent		= _GetBattleCountEndPercentTeam(0);//Red
	r32BattleCountEndPercent2		= _GetBattleCountEndPercentTeam(1);//Blue

	REAL32 r32BattleModeInvariable	= _GetBattleModeInvariable();
#endif

#ifdef USE_LOG
	char szTemp[MAX_PATH];
	i3String::Format( szTemp, "[Mode(%d)] rBattleCountEndPercent(%.3f),rBattleCountEndPercent2(%.3f),rBattleCountEndPercent3(%.3f),rBattleCountEndPercent4(%.3f)",m_Info._i32StageType, r32BattleCountEndPercent, r32BattleCountEndPercent2, r32BattleCountEndPercent3, r32BattleCountEndPercent4);
	g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, szTemp);
#endif

#ifndef NEW_EXPPOINT
	//2. 비정상 종료일때 차감내용
	if( FALSE == bNormalEnd )
	{
		r32SubPercent = m_pRoomMode->GetSubPercentOfUnusualEnd( i32Win );
	}
#endif

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

	INT8	i8BattleUserCount = 0;

	for(INT i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( m_Info._SlotInfo[i]._State != SLOT_STATE_BATTLE ) continue;
		if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode ) continue;

		m_Info._aui16AccExp[i] = m_pRoomMode->GetMatchAccExp(i, m_Info._aui16AccExp[i]);
		m_Info._aui16AccPoint[i] = m_pRoomMode->GetMatchAccPoint(i, m_Info._aui16AccPoint[i]);
		i8BattleUserCount++;
	}

	INT8 ai8Ranking[SLOT_MAX_COUNT] = {0,};

	m_pRoomMode->GetRanking( ai8Ranking, i8BattleUserCount, GET_RANKING_RAWEXP );
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode )
		{
			ui16BattleEndUser = ui16BattleEndUser | (0x01 << i); 
			continue;
		}

		if( m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE )
		{
			// 자동 팀 밸런스를 위해 승패 보너스를 추가하지 않은 경험치 순위 비율을 기록 (소수점 세자리 (*1000))
			m_Info._pSession[i]->m_ui16PrevExpRank = (ai8Ranking[i]*1000)/i8BattleUserCount;
			m_Info._pSession[i]->m_ui32PlayTime =
#ifdef USE_GM_PAUSE
													GetRoomBattleTime() 
#else
													g_pContextMain->GetServerTime()
#endif
																					- m_Info._pSession[i]->m_ui32BattleStartTime;
			m_Info._pSession[i]->m_ui32LoginBattleTime					+= m_Info._pSession[i]->m_ui32PlayTime;
			m_Info._pSession[i]->m_UserInfoBasic._ui32TotalBattleTime	+= m_Info._pSession[i]->m_ui32PlayTime;

			// 실제 배틀에 참여했던 시간을 누적해서 기록
			//m_Info._pSession[i]->m_UserInfoBasic._ui32ActualBattleTime += m_Info._pSession[i]->m_ui32AccActualBattleTime;
			
			// 플레이 시간 누적
			TS_EVENT_INFO stTSEventInfo;
			if( true == g_pContextMain->GetTSEventInfo(m_Info._pSession[i]->m_UserInfoDaily._EventDate, &stTSEventInfo) )
			{

				// 해당 이벤트 판별 후 이전 날짜의 이벤트 처리
				// 수정해야함 
				if(	(STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask) && 
					(GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL) &&
					(GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_DOMINATION) )
				{
					m_Info._pSession[i]->m_UserInfoDaily._playtime += m_Info._pSession[i]->m_ui32PlayTime - m_Info._pSession[i]->m_ui32FirstObserverTime;
					m_Info._pSession[i]->m_bFirstObserver = FALSE;
				}


				// 선택된 이벤트의 UID가 존재하면
				if(stTSEventInfo._ui32UID != 0 && m_Info._pSession[i]->m_UserInfoDaily._bGetItem == FALSE)
				{
					if(m_Info._pSession[i]->m_UserInfoDaily._playtime >= stTSEventInfo._ui8TSType * 60)
					{
						TS_EVENT_ITEM_INFO	stTSItem;

						i3mem::FillZero(&stTSItem, sizeof(TS_EVENT_ITEM_INFO));

						i3mem::Copy(stTSItem._strTSName,  stTSEventInfo._strTSName, MAX_TS_ITEM_STRING); 
						m_Info._pSession[i]->m_aui32TSGoodsID[0] = stTSItem._i32GoodsID[0] = stTSEventInfo.stGoodsList[g_pContextMain->m_ui8GoodsOrder]._ui32GoodsID[0];
						m_Info._pSession[i]->m_aui32TSGoodsID[1] = stTSItem._i32GoodsID[1] = stTSEventInfo.stGoodsList[g_pContextMain->m_ui8GoodsOrder]._ui32GoodsID[1];
						stTSItem._i32PlayTime	= m_Info._pSession[i]->m_UserInfoDaily._playtime;
						stTSItem._ui8Type		= TS_EVENT_SEND_ITEM_LIST;
						m_Info._pSession[i]->m_bCompleteTS = TRUE;

						// 클라이언트에 TS 이벤트 정보 전송
						i3NetworkPacket	SendPacket( PROTOCOL_ROOM_TS_EVENT_ACK );		
						SendPacket.WriteData( &stTSItem, sizeof(TS_EVENT_ITEM_INFO) );
						m_Info._pSession[i]->SendMessageA(&SendPacket);
					}
					else
					{
						TS_EVENT_ITEM_INFO	stTSItem;
						i3mem::FillZero(&stTSItem, sizeof(TS_EVENT_ITEM_INFO));

						stTSItem._i32PlayTime = (stTSEventInfo._ui8TSType * 60) - m_Info._pSession[i]->m_UserInfoDaily._playtime;
						stTSItem._ui8Type	  = TS_EVENT_REMAIN_TIME;

						i3NetworkPacket	SendPacket( PROTOCOL_ROOM_TS_EVENT_ACK );		
						SendPacket.WriteData( &stTSItem, sizeof(TS_EVENT_ITEM_INFO) );
						m_Info._pSession[i]->SendMessageA(&SendPacket);
					}
				}
			}



				/*SHOP_MATCHING_DATA*	  pMatching = g_pContextMain->GetShop()->GetMatchingAllData();

				for(int j = 0; j < 2; j++)
				{
					if(g_pContextMain->m_stSelectedTSEvent._ui32GoodsID[j] != 0)
					{
						UINT32 ui32ItemID = 0;
						UINT32 ui32ItemArg = 0;

						UINT32	ui32MatchingCount	= g_pContextMain->GetShop()->GetMatchingCount();
						for( UINT32 k = 0; k < ui32MatchingCount; ++k )
						{
							if( pMatching[k]._ui32GoodsID != g_pContextMain->m_stSelectedTSEvent._ui32GoodsID[j]) continue;

							ui32ItemID	= pMatching[k]._ui32ItemID;
							ui32ItemArg = pMatching[k]._ui32ItemArg;
							break;
						}	
						if(ui32ItemID != 0 && ui32ItemArg != 0)
						{
							g_pModuleTrans->AuthSendInsertItem(m_Info._pSession[i]->m_WorkThreadIdx, m_Info._pSession[i], ITEM_INSERT_REQUEST_TS_BONUS, ui32ItemID, ui32ItemArg);
							m_Info._pSession[i]->m_UserInfoDaily._playtime = -100;
						}
					}
				}*/

			// 어뷰징 체크 
			UINT32	ui32PlayTime = 0;

			if( TRUE == g_pContextMain->m_bAbusingCheck )
			{
				ui32PlayTime	=  
#ifdef USE_GM_PAUSE
									GetRoomBattleTime()
#else
									g_pContextMain->GetServerTime() 
#endif
																	- m_Info._pSession[i]->m_ui32BattleStartTime ; // 플레이 시간 = 현재 시간 - 배틀 시작 시간
				
				if( ui32PlayTime > ABUSING_CHECK_MIN_PLAY_TIME  )  _CheckAbusing( i, ui32PlayTime );	 // 플레이 타임 체크
			}
			else	
			{ 
				m_Info._pSession[i]->m_ui8AbusingLevel		= ABUSING_LEVEL_NONE; 
				m_Info._pSession[i]->m_ui32AbusingPopupTime	= 0;
			}


			ui16BattleEndUser = ui16BattleEndUser | (0x01 << i); 

			if(	(STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask) && 
				(GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL) &&
				(GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_DOMINATION) )
			{			
				i32TeamIdx = i % 2;

				SetUserMissionCard( m_Info._pSession[i], ACTION_ID_WIN, &ai32WinTeam[i32TeamIdx] );
				SetUserMissionCard( m_Info._pSession[i], ACTION_ID_DEFEAT, &ai32LoseTeam[i32TeamIdx] );

				// 승패수
				m_Info._pSession[i]->m_UserInfoRecord._win			+= ai32WinTeam[ i32TeamIdx ]; 
				m_Info._pSession[i]->m_UserInfoRecord._lose			+= ai32LoseTeam[ i32TeamIdx ]; 
				m_Info._pSession[i]->m_UserInfoRecord._draw			+= ai32DrawTeam[ i32TeamIdx ]; 
				// 시즌 승패수
				m_Info._pSession[i]->m_UserInfoRecord._swin			+= ai32WinTeam[ i32TeamIdx ]; 
				m_Info._pSession[i]->m_UserInfoRecord._slose		+= ai32LoseTeam[ i32TeamIdx ]; 
				m_Info._pSession[i]->m_UserInfoRecord._sdraw		+= ai32DrawTeam[ i32TeamIdx ];

				// 당일 승패
				m_Info._pSession[i]->m_UserInfoDaily._win			+= ai32WinTeam[ i32TeamIdx ]; 
				m_Info._pSession[i]->m_UserInfoDaily._lose			+= ai32LoseTeam[ i32TeamIdx ]; 
				m_Info._pSession[i]->m_UserInfoDaily._draw			+= ai32DrawTeam[ i32TeamIdx ]; 
				//m_Info._pSession[i]->m_UserInfoDaily._match		+= i32WinTeam[ TeamIdx ] + i32LoseTeam[ TeamIdx ] + i32DrawTeam[ TeamIdx ]; 


				// 매치카운트는 증가
				m_Info._pSession[i]->m_UserInfoRecord._match++; 
				m_Info._pSession[i]->m_UserInfoRecord._smatch++; 
				m_Info._pSession[i]->m_UserInfoRecord._dmatch++;
				m_Info._pSession[i]->m_UserInfoRecord._sdmatch++;

				// 매치킬데스
				m_Info._pSession[i]->m_UserInfoRecord._killcount	+= m_Info._aM_UserScore[i].m_KillCount; 
				m_Info._pSession[i]->m_UserInfoRecord._death		+= m_Info._aM_UserScore[i].m_DeathCount;
				m_Info._pSession[i]->m_UserInfoRecord._headshot		+= m_Info._aui16M_Headshot[i];
				// 시즌매치 킬데스
				m_Info._pSession[i]->m_UserInfoRecord._skillcount	+= m_Info._aM_UserScore[i].m_KillCount; 
				m_Info._pSession[i]->m_UserInfoRecord._sdeath		+= m_Info._aM_UserScore[i].m_DeathCount;
				m_Info._pSession[i]->m_UserInfoRecord._sheadshot	+= m_Info._aui16M_Headshot[i];
				// d 매치 킬카운트
				m_Info._pSession[i]->m_UserInfoRecord._dkillcount	+= m_Info._aM_UserScore[i].m_KillCount;
				m_Info._pSession[i]->m_UserInfoRecord._sdkillcount	+= m_Info._aM_UserScore[i].m_KillCount;

				// 당일 킬데스
				m_Info._pSession[i]->m_UserInfoDaily._killcount		+= m_Info._aM_UserScore[i].m_KillCount; 
				m_Info._pSession[i]->m_UserInfoDaily._death			+= m_Info._aM_UserScore[i].m_DeathCount;
				m_Info._pSession[i]->m_UserInfoDaily._headshot		+= m_Info._aui16M_Headshot[i];

				// 모드별로 점수 적용
#ifndef NEW_EXPPOINT
				switch( m_Info._i32StageType )
				{
				case STAGE_TYPE_DEFENCE:
					{
						if(0 == i32TeamIdx)
						{//Red
							m_Info._aui16AccExp[i]			= (INT16)(m_Info._aui16AccExp[i] * r32BattleCountEndPercent3);
							m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccExp[i], r32SubPercent * r32BattleCountEndPercent, bNormalEnd, GetRoomBattleTime() );
							m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * r32BattleCountEndPercent, bNormalEnd, GetRoomBattleTime() );
						}
						else
						{//Blue
							m_Info._aui16AccExp[i]			= (INT16)(m_Info._aui16AccExp[i] * r32BattleCountEndPercent4);
							m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccExp[i], r32SubPercent * r32BattleCountEndPercent2, bNormalEnd, GetRoomBattleTime() );
							m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * r32BattleCountEndPercent2, bNormalEnd, GetRoomBattleTime() );
						}
					}
					break;
				case STAGE_TYPE_DESTROY:
				case STAGE_TYPE_ESCAPE: 
				case STAGE_TYPE_CROSSCOUNT:
					{
						if(0 == i32TeamIdx)
						{//Red
							m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccExp[i], r32SubPercent * r32BattleCountEndPercent, bNormalEnd, GetRoomBattleTime() );
							m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * r32BattleCountEndPercent, bNormalEnd, GetRoomBattleTime() );
						}
						else
						{//Blue
							m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccExp[i], r32SubPercent * r32BattleCountEndPercent2, bNormalEnd, GetRoomBattleTime() );
							m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * r32BattleCountEndPercent2, bNormalEnd, GetRoomBattleTime() );
						}
					}
					break;
				default:
					{
						m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccExp[i], r32SubPercent * r32BattleCountEndPercent, bNormalEnd, GetRoomBattleTime() );
						m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32SubPercent * r32BattleCountEndPercent, bNormalEnd, GetRoomBattleTime() );
					}
					break;
				}
#else
				if(0 == i32TeamIdx)
				{//Red
					m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccExp[i], r32BattleCountEndPercent, bNormalEnd, GetRoomBattleTime() );
					m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32BattleModeInvariable, bNormalEnd, ai8Ranking, i8BattleUserCount, GetRoomBattleTime() );
				}
				else
				{//Blue
					m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccExp[i], r32BattleCountEndPercent2, bNormalEnd, GetRoomBattleTime() );
					m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i, m_Info._aui16AccPoint[i], r32BattleModeInvariable, bNormalEnd, ai8Ranking, i8BattleUserCount, GetRoomBattleTime() );
				}
#endif

				// 임무카드 완료 처리
				if( (bNormalEnd == TRUE) || (i32Win == (i % TEAM_COUNT)) )
					m_Info._pSession[i]->CheckQuestItem2();
			}
			else
			{	// AI 모드 점수 변환
				//m_Info._ui16AccExp[i]	= (UINT16)(m_Info._ui16AccTrainingScore[i]	* 0.02001f);
				//m_Info._ui16AccPoint[i] = (UINT16)(m_Info._ui16AccTrainingScore[i]	* 0.04001f);
				m_Info._aui16AccExp[i]			= m_pRoomMode->GetMatchEndExp( i32Win, i, m_Info._aui16AccTrainingScore[i], 0.02001f, bNormalEnd, GetRoomBattleTime() );
				m_Info._aui16AccPoint[i]		= m_pRoomMode->GetMatchEndPoint( i32Win, i,
#ifndef NEW_EXPPOINT
																							m_Info._aui16AccTrainingScore[i], 0.04001f, bNormalEnd, GetRoomBattleTime() );
#else
																							m_Info._aui16AccPoint[i], r32BattleModeInvariable, bNormalEnd, ai8Ranking, i8BattleUserCount, GetRoomBattleTime() );
#endif
			}

			if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
			{
				// AI전(=챌린지모드,=트레이닝모드) 경험치, 포인트 상한선 300으로 고정
				if( m_Info._aui16AccExp[i] > 300 )		m_Info._aui16AccExp[i] = 300;
				if( m_Info._aui16AccPoint[i] > 300 )	m_Info._aui16AccPoint[i] = 300;
			}
			else
			{
				// 경험치, 포인트 상한선 2000으로 고정
				if( m_Info._aui16AccExp[i] > 2000 )		m_Info._aui16AccExp[i] = 2000;
				if( m_Info._aui16AccPoint[i] > 2000 )	m_Info._aui16AccPoint[i] = 2000;
			}

			// 기존 이벤트 처리를 따로 관리합니다.
			_ResultUpdateUser_M( MAIN_THREAD_IDX, i );

			// 어뷰징 페널티 부여
			if( TRUE == g_pContextMain->m_bAbusingCheck )
			{
				if( ui32PlayTime > ABUSING_CHECK_MIN_PLAY_TIME  )  _AbusingPenalty( i, ui32PlayTime );	// 플레이 타임 체크
			}

			// 사용한 무기를 인증하는 루틴을 보냅니다.
			_SendUsedWeapon( MAIN_THREAD_IDX, m_Info._pSession[i] );

			// ClanBattle 를 위한 루틴 // 20080422
			switch(g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum])
			{
			default: 
				break;
			case CHANNEL_TYPE_PCCAFE_CLAN:
			case CHANNEL_TYPE_CLAN: 
				{
					// Clan 에 대한 기록
					aClanResult[ i32TeamIdx ]._ClanIdx	= m_Info._aui32BattleClanIdx[ i32TeamIdx ];
					aClanResult[ i32TeamIdx ]._Exp		+= m_Info._aui16AccExp[i] + m_Info._aui16EventExp[i];
					aClanResult[ i32TeamIdx ]._Match	= 1;

					if( ai32WinTeam[i32TeamIdx] )
					{
						aClanResult[ i32TeamIdx ]._Win = 1;
						aClanResult[ i32TeamIdx ]._Lose = 0;
						aClanResultUser[ ui8BattleUserCount ]._WinLose	= 3;
					}
					else if( ai32LoseTeam[i32TeamIdx] )
					{
						aClanResult[ i32TeamIdx ]._Win = 0;
						aClanResult[ i32TeamIdx ]._Lose = 1;
						aClanResultUser[ ui8BattleUserCount ]._WinLose	= 2;
					}
					else
					{
						aClanResultUser[ ui8BattleUserCount ]._WinLose	= 1;
					}

					// User 에 대한 기록					
					aClanResultUser[ ui8BattleUserCount ]._ClanIdx	= m_Info._pSession[i]->GetUserInfoBasic()->_clanidx;
					aClanResultUser[ ui8BattleUserCount ]._i64UID	= m_Info._pSession[i]->m_i64UID;
					aClanResultUser[ ui8BattleUserCount ]._Exp		= m_Info._aui16AccExp[i] + m_Info._aui16EventExp[i];
					aClanResultUser[ ui8BattleUserCount ]._Kill		= m_Info._aM_UserScore[i].m_KillCount;
					aClanResultUser[ ui8BattleUserCount ]._Death	= m_Info._aM_UserScore[i].m_DeathCount;
					aClanResultUser[ ui8BattleUserCount ]._Head		= m_Info._aui16M_Headshot[i];

					ui8BattleUserCount++;
				}
				break; 
			}
		}
	}

	// ClanBattle 를 위한 루틴 // 20080422
	switch(g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum])
	{
	default: 
		break;
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN: 	
		{
			// AI 모드와 튜토리얼의 경우 전적을 반영하지 않는다.
			// 동일 클랜끼리 매치 경우도 클랜의 전적은 남기지 않고 클랜 맴버의 전적만 반영한다.
			if ( ( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )	&&
				 ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL ) )
			{
				// 게임을 정상 종료 하지 않았을 경우에 대한 처리입니다.
				for( INT32 i = 0 ; i < TEAM_COUNT ; i++ )
				{
					if( aClanResult[ i ]._ClanIdx != m_Info._aui32BattleClanIdx[ i ] )
					{
						aClanResult[ i ]._ClanIdx	= m_Info._aui32BattleClanIdx[ i ];
						aClanResult[ i ]._Match		= 1;
						aClanResult[ i ]._Win		= 0;
						aClanResult[ i ]._Exp		= 0;
						aClanResult[ i ]._Lose		= 1;
					}
				}

				UINT8 cMatch = 0;
				if( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask )	cMatch = 1;
				else															cMatch = 0;

				g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_CLAN_MATCHING, NULL, this, aClanResult );

				g_pModuleClan->ClanSendBattleResult( MAIN_THREAD_IDX, cMatch, TEAM_COUNT, aClanResult, ui8BattleUserCount, aClanResultUser );
			}
		}
		break; 
	}

	// 보급상자 지급 확인
	_PresentSupplyBox_M();

	return ui16BattleEndUser; 
}

float CRoom::_GetUsePositionWeight( UINT32 ui32ItemID, UINT8 ui8UsedCount )
{
	// 무기별 가중치
	REAL32 r32Weight = 1.f;
	INT32 i32Class = GET_ITEM_FLAG_CLASS( ui32ItemID );
	switch( i32Class )
	{
	case ITEM_CLASS_ASSAULT:	
	case ITEM_CLASS_SMG:	
	case ITEM_CLASS_SNIPER:	
	case ITEM_CLASS_SHOTGUN:
	case ITEM_CLASS_MACHINE:
		r32Weight = 0.7f;
		break;
	case ITEM_CLASS_HANDGUN:
		r32Weight = 0.2f;
		break;
	case ITEM_CLASS_KNIFE:
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
	pSession->CheckEndurance();
	if( 0 == pSession->m_ui16UsedItemCount )	return;

	// AI전일경우 내구도 10%만 소모
	UINT8 ui8AIE = 1;
	if( m_Info._InfoBasic._StageMask == STAGE_MASK_TYPE_TRAINING )
		ui8AIE = 10;
	
	USED_ITEM aUsedItemList[ MAX_INVEN_COUNT ];
	for( INT32 i = 0 ; i < pSession->m_ui16UsedItemCount ; i++ )
	{
		aUsedItemList[i].m_i64UsedItemID		= pSession->m_aUsedItemList[i].m_i64UsedItemID;

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
					if( GET_USED_CHECK_SLOT(ui8CheckSlotInfo) >= WEAPON_USAGE_MISSION ) break;
					ui8UsedCount = pSession->m_aui8WeaponUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) ];
					break;
				case USED_CHECK_CHARA:
					ui8UsedCount = pSession->m_aui8CharaUseCount[ GET_USED_CHECK_SLOT(ui8CheckSlotInfo) - (WEAPON_USAGE_COUNT-1) ];
					break;
				}

				aUsedItemList[i].m_ui16UsedDurablity	= (UINT16)(((REAL32)pSession->m_aUsedItemList[i].m_ui32UsedDurablity * _GetUsePositionWeight( pSession->m_aUsedItemList[i].m_ui32ItemID, ui8UsedCount )) / ui8AIE);
			}
			break;
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
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State ) continue;
		if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode ) continue;

		i32AllCount++;

		if( i32Team == i32TeamIdx) i32Count++;  
	}

	return m_pRoomMode->GetSubPercentOfUserCountTeam( i32Team, i32AllCount, i32Count );
}

#ifdef NEW_EXPPOINT
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

	// AI 챌린지 모드
	if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
		r32Invariable = 0.1f;

	return r32Invariable;
}
#endif

#ifndef NEW_EXPPOINT
REAL32 CRoom::_GetBattleCountEndPercent()
{
	INT32  i32Count = 0; 
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
	{
		if( SLOT_STATE_BATTLE == m_Info._SlotInfo[i]._State ) i32Count++;  
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
		if( SLOT_STATE_BATTLE == m_Info._SlotInfo[i]._State && i32Team == i32TeamIdx) i32Count++;  
	}

	return m_pRoomMode->GetSubPercentOfUserCount2( i32Count );
}
#endif

void CRoom::_ResultUpdateUser_M( INT32 i32WorkIdx, INT32 i32SlotIdx )
{
	CUserSession* pSession = m_Info._pSession[i32SlotIdx];

	INT32 i32TeamIdx = i32SlotIdx % 2;

	// 리셋 
	m_Info._aui8ResultIcon[i32SlotIdx]	= 0x00;
	m_Info._aui16EventExp[i32SlotIdx]	= 0;
	m_Info._aui16EventPoint[i32SlotIdx]	= 0;

	// 튜토리얼은 보너스 경험치/포인트 지급하지 않음
	if( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL )
	{
		pSession->m_UserInfoBasic._exp += m_Info._aui16AccExp[i32SlotIdx]; 
		pSession->m_UserInfoBasic._point += m_Info._aui16AccPoint[i32SlotIdx];

		pSession->m_UserInfoDaily._exp += m_Info._aui16AccExp[i32SlotIdx]; 
		pSession->m_UserInfoDaily._point += m_Info._aui16AccPoint[i32SlotIdx];

		INT32 i32GetRoute = 1;
		g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_GET_POINT, pSession, &m_Info._aui16AccPoint[i32SlotIdx], &i32GetRoute );

		if( (FALSE == pSession->IsGMUser()) && (FALSE == pSession->IsMODUser()) )
		{
			// 계급 변화가 있는지 확인
			pSession->CheckRankUp( i32WorkIdx );
		}

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
	
	TYPE_ROOM_BONUS* pRoomBouns = NULL;

	// 서버 보너스
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_SERVER );
	if( pRoomBouns )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
		r32AddExpEvent	+= pRoomBouns->_r32Exp;
		r32AddPotEvent	+= pRoomBouns->_r32Point;
	}

	// 국가 추가 부스트
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_NATION, (INT32)pSession->m_ui8OtherCountry );
	if( pRoomBouns )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
		r32AddExpEvent	+= pRoomBouns->_r32Exp;
		r32AddPotEvent	+= pRoomBouns->_r32Point;
	}

	//PC방 추가 경험치, 포인트
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_CAFE, (INT32)pSession->m_UserInfoBasic._pcCafe );
	if( pRoomBouns )
	{
		switch( pSession->m_UserInfoBasic._pcCafe )
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
				if( STAGE_MASK_TYPE_CLANMATCH == m_Info._InfoBasic._StageMask )
					r32AddExpPCCafe	+= 0.10001f;
			}
			break;
		}

		r32AddExpPCCafe	+= pRoomBouns->_r32Exp;
		r32AddPotPCCafe	+= pRoomBouns->_r32Point;
	}

	// Channel 추가 경험치, 포인트
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_CHANNEL, (INT32)g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum] );
	if( pRoomBouns )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
		r32AddExpEvent	+= pRoomBouns->_r32Exp;
		r32AddPotEvent	+= pRoomBouns->_r32Point;
	}
	// 게릴라 이벤트 보너스
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_NORMAL );
	if( pRoomBouns )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
		r32AddExpEvent	+= pRoomBouns->_r32Exp;
		r32AddPotEvent	+= pRoomBouns->_r32Point;
	}

	// 모드 추가 경험치
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_MODE, (INT32)GET_STAGETYPE(m_Info._InfoBasic._StageID) );
	if( pRoomBouns )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
		r32AddExpEvent	+= pRoomBouns->_r32Exp;
		r32AddPotEvent	+= pRoomBouns->_r32Point;
	}

	// 특수전 추가 경험치
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_STAGE_MASK, m_Info._InfoBasic._StageMask );
	if( pRoomBouns )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
		r32AddExpEvent	+= pRoomBouns->_r32Exp;
		r32AddPotEvent	+= pRoomBouns->_r32Point;
	}

	// STAGE 추가 경험치
	pRoomBouns = g_pContextMain->GetRoomBonus( EVENT_ROOM_BONUS_STAGE, (INT32)GET_STAGEORD(m_Info._InfoBasic._StageID) );
	if( pRoomBouns )
	{
		m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT;
		r32AddExpEvent	+= pRoomBouns->_r32Exp;
		r32AddPotEvent	+= pRoomBouns->_r32Point;
	}

	//돈주고 산 캐릭터 아이템 - 대회(세일) 빌드에는 적용하지 않는다.
	if( LOCALE_CODE_ENGLISH != m_Info._pSession[i32SlotIdx]->m_ui8LocaleCode )
	{
		if(i32TeamIdx == TEAM_RED)
		{
			switch( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentChara.GetRedNum() )
			{
				// 디폭스
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,	CHARACTER_RES_ID_D_FOX):
				{
					r32AddExpItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
				// 바이퍼레드
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,	CHARACTER_RES_ID_FEMALE_VIPERRED):
				{
					r32AddPotItem	+= 0.30001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
				}
				break;
				// 강화 디폭스
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,	CHARACTER_RES_ID_REINFORCED_COMBO_D_FOX):
				{
					r32AddExpItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
				// 강화 바이퍼레드
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,	CHARACTER_RES_ID_REINFORCED_COMBO_VIPERRED):
				{
					r32AddPotItem	+= 0.30001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
				}
				break;
				// 타란튤라 GRS
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,	CHARACTER_RES_ID_FEMALE_RED_GRS):
				{
					r32AddExpItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
				// 레드불스 GRS
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_RED,	CHARACTER_RES_ID_MALE_RED_GRS):
				{
					r32AddPotItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
				}
				break;
			}
		}
		else
		{
			switch( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentChara.GetBlueNum() )
			{
				// 레오파트
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_LEOPARD):
				{
					r32AddExpItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
				// 하이드
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_FEMALE_HIDE):
				{
					r32AddPotItem	+= 0.30001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
				// 강화 레오파트
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_REINFORCED_COMBO_LEOPARD):
				{
					r32AddExpItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
				}
				break;
				// 강화 하이드
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_REINFORCED_COMBO_HIDE):
				{
					r32AddPotItem	+= 0.30001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
				// 킨아이즈 GRS
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_FEMALE_BLUE_GRS):
				{
					r32AddExpItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
				// 액시드폴 GRS
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_MALE_BLUE_GRS):
				{
					r32AddPotItem	+= 0.20001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}
				break;
			case MAKE_ITEM_FLAG(ITEM_CLASS_CHARA, CHARACTER_CLASS_CHARA_TYPE, CHARACTER_TEAM_BLUE,	CHARACTER_RES_ID_LEOPARD_BOPE):
				{
					r32AddExpItem	+= 0.15001f;
					r32AddPotItem	+= 0.15001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
				}				
				break;
			}
		}
	}

	// 주무기류
	switch( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim )
	{
	case 100003024:			// M4A1 G+ 30%			//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_M4A1_G_EXP_PT_30):
		{
			INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.30001f;
			r32AddPotItem	+= 0.30001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
		}
		break;
	case 100003026:			// SG550 Point 10%		//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_SG550_MB_EXP_PT_10):
		{
			INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.10001f;
			r32AddPotItem	+= 0.10001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
		}
		break;	
	case 100003025:			// AK 47 Silver 20%		//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_AK47_S_POINT15):
	case 100003030:			// SG550 MB 20+			//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_SG550_MB_EXP_PT_20):
	case 100003034:			// G36C Sl +			//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_G36C_SILVER_EXP_PT_20):
	case 100003033:			// AK Sopmode + 20		//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_AK_SOPMOD_EXP_PT_20):
	case 400006009:			// SPAS15 + 20			//MAKE_ITEM_FLAG(ITEM_CLASS_SHOTGUN, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SHOTGUN, WEAPON_NUM_SHOTGUN_SPAS15_SILVER_EXP_PT_20):
	case 300005013:			// L11A1 S +20			//MAKE_ITEM_FLAG(ITEM_CLASS_SNIPER, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SNIPER, WEAPON_NUM_SNIPER_L115A1_S_EXP_PT_20 ):
	// case 200004027:			// P90MC Dot Camo.		//MAKE_ITEM_FLAG(ITEM_CLASS_SMG, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SMG, WEAPON_NUM_SMG_P90MC_DOTSIGHT_CAMO):
		{
			INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.20001f;
			r32AddPotItem	+= 0.20001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
		}
		break;
	case 100003037:			// AUG A3 G.			//MAKE_ITEM_FLAG(ITEM_CLASS_ASSERT, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_ASSAULT, WEAPON_NUM_ASSAULT_AUG_A3_GOLD):
		{
			// AUG A3 G. 의 보너스에서 인도네시아는 제외 합니다. - 서동권
			//if( m_Info._pSession[iSlotIdx]->m_ui8LocaleCode == LOCALE_CODE_INDONESIA )	break;
			switch(m_Info._pSession[i32SlotIdx]->m_ui8LocaleCode)
			{
			case LOCALE_CODE_INDONESIA:
			case LOCALE_CODE_TURKEY:
				break;
			default:
				{
					INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
					if( 0 == pItem->_ui32ItemArg ) break;

					r32AddExpItem	+= 0.05001f;
					m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
				}
				break;
			}
		}
		break;
	case 300005015:			// L115A1 G.			//MAKE_ITEM_FLAG(ITEM_CLASS_SNIPER, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SNIPER, WEAPON_NUM_SNIPER_L115A1_GOLD):
	case 200004026:			// Kriss S.V. G.		//MAKE_ITEM_FLAG(ITEM_CLASS_SMG, WEAPON_USAGE_PRIMARY, WEAPON_CLASS_SMG, WEAPON_NUM_SMG_KRISSSUPERV_GOLD):
	// case 100003041:			// AK Sopmod KingCobra
		{
			INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.05001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	case 100003121:	// M4A1 PBIC2013
	case 100003122: // AK-47 PBIC2013
		{
			INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.15001f;
			r32AddPotItem	+= 0.15001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
		}
		break;
	}

	// 태국에서만 적용되는 특정주무기 착용시 추가경험치, 추가포인트 적용 코드	2011.06.03 안국정
	switch( m_Info._pSession[i32SlotIdx]->m_ui8LocaleCode )
	{
	case LOCALE_CODE_THAILAND		:
		switch( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim )
		{
		case 100003061:			// Pindad SS2-V4 PS G.		5% 경험치 추가 획득
		case 100003064:			// FAMAS G2 Commando G.
		case 200004049:			// Pindad SS1-R5 Carbine G. 
		case 200018013:			// Dual Mini Uzi G.	
			{
				INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
				if ( 0 == pItem->_ui32ItemArg ) break;

				r32AddExpItem	+= 0.05001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
		case 100003062:			// FAMAS G2 Commando ES.	5% 경험치와 5% 포인트 추가 획득
		case 100003063:			// AUG-A3 ES.
		case 200004050:			// Kriss SV ES.
		case 300005033:			// L115A1 ES.
		case 100003065:			// AUG BLUE
			{
				INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._noprim );
				if ( 0 == pItem->_ui32ItemArg ) break;

				r32AddExpItem	+= 0.05001f;
				r32AddPotItem	+= 0.05001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
		}
	}

	// 보조무기류
	switch( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._nosub )
	{
	case 601002010:			// Colt Python Gold + Exp & Point 10%
		{
			INVEN_BUFFER* pItem = m_Info._pSession[i32SlotIdx]->GetItemByItemID( m_Info._pSession[i32SlotIdx]->m_UserInfoEquipmentWeapon._nosub );
			if( 0 == pItem->_ui32ItemArg ) break;

			r32AddExpItem	+= 0.10001f;
			r32AddPotItem	+= 0.10001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
		}
		break;
	}

	//	근접무기
	//	투척무기	
	//	투척 아이템
	//	미션 아이템

	// 사용중인 증가권 확인
	for(INT32 itemIdx = 0; itemIdx < m_Info._pSession[i32SlotIdx]->m_i32InvItemCount; itemIdx++)
	{
		if (m_Info._pSession[i32SlotIdx]->m_aInvItem[itemIdx]._ui8ItemType != ITEM_ATTR_TYPE_AUTH)	continue;
		switch(m_Info._pSession[i32SlotIdx]->m_aInvItem[itemIdx]._ui32ItemID)
		{
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_130, CASHITEM_DAY_NO):
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_130_EV, CASHITEM_DAY_NO):
			r32AddExpItem	+= 0.30001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 	
			break;
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_130, CASHITEM_DAY_NO):
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_130_EV, CASHITEM_DAY_NO):
			r32AddPotItem	+= 0.30001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
			break;
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_110, CASHITEM_DAY_NO):
			r32AddExpItem	+= 0.10001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 
			break;
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_150, CASHITEM_DAY_NO):
			r32AddExpItem	+= 0.50001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			break;
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_200, CASHITEM_DAY_NO):
			r32AddExpItem	+= 1.00001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			break;
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_200, CASHITEM_DAY_NO):
			r32AddPotItem	+= 1.00001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			break;
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_150, CASHITEM_DAY_NO):
			r32AddPotItem	+= 0.50001f;
			m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM; 	
			break;
			// 시간대별 보너스
			// 12 ~ 15
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_X2_12PM, CASHITEM_DAY_NO):			// 12PM~3PM 경험치x2 
			if(  12 <= g_pContextMain->GetServerTimeHour() && 15 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddExpItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 15 ~ 18
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_X2_3PM, CASHITEM_DAY_NO):			// 3PM~6PM 경험치x2 
			if( 15 <= g_pContextMain->GetServerTimeHour() && 18 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddExpItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 18 ~ 21
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_X2_6PM, CASHITEM_DAY_NO):			// 6PM~9PM 경험치x2
			if( 18 <= g_pContextMain->GetServerTimeHour() && 21 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddExpItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 21 ~ 0
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_X2_9PM, CASHITEM_DAY_NO):			// 9PM~12PM 경험치x2
			if( 21 <= g_pContextMain->GetServerTimeHour() && 23 >= g_pContextMain->GetServerTimeHour() )
			{
				r32AddExpItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 12 ~ 15
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_X2_12PM, CASHITEM_DAY_NO):			// 12PM~3PM 포인트x2
			if( 12 <= g_pContextMain->GetServerTimeHour() && 15 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddPotItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 15 ~ 18
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_X2_3PM, CASHITEM_DAY_NO):			// 3PM~6PM 포인트x2
			if( 15 <= g_pContextMain->GetServerTimeHour() && 18 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddPotItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 18 ~ 21
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_X2_6PM, CASHITEM_DAY_NO):			// 6PM~9PM 포인트x2
			if( 18 <= g_pContextMain->GetServerTimeHour() && 21 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddPotItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 21 ~ 0
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_PT_PLUS_X2_9PM, CASHITEM_DAY_NO):			// 9PM~12PM 포인트x2
			if( 21 <= g_pContextMain->GetServerTimeHour() && 23 >= g_pContextMain->GetServerTimeHour() )
			{
				r32AddPotItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 0 ~ 6
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_200_0AM_6AM, CASHITEM_DAY_NO):		// 0AM~6AM 경험치x2
			if( 0 <= g_pContextMain->GetServerTimeHour() && 6 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddExpItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
			// 6 ~ 12
		case MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_XP_PLUS_200_6AM_12PM, CASHITEM_DAY_NO):		// 6AM~12PM 경험치x2
			if( 6 <= g_pContextMain->GetServerTimeHour() && 12 > g_pContextMain->GetServerTimeHour() )
			{
				r32AddExpItem	+= 1.00001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_ITEM;
			}
			break;
		}
	}

	switch( m_Info._pSession[i32SlotIdx]->m_ui8LocaleCode )
	{
	case LOCALE_CODE_THAILAND		:
		{
			//// 태국 Esport 레벨에 따른 보상 

			switch ( m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._eSportID ) 
			{
			case ESPORT_ID_LEVEL_SUBSTITUTE2 :	// EXP + 5%
			case ESPORT_ID_LEVEL_STARTER1 :		// EXP + 5%
				r32AddExpEvent	+= 0.05001f;		
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 	
				break;

			case ESPORT_ID_LEVEL_STARTER2 :		// EXP + 5% , POINT + 5%
			case ESPORT_ID_LEVEL_STAR1 :		// EXP + 5% , POINT + 5%
			case ESPORT_ID_LEVEL_STAR2 :		// EXP + 5% , POINT + 5%
			case ESPORT_ID_LEVEL_STAR3 :		// EXP + 5% , POINT + 5%
			case ESPORT_ID_LEVEL_SUPERSTAR1 :	// EXP + 5% , POINT + 5%
			case ESPORT_ID_LEVEL_SUPERSTAR2 :	// EXP + 5% , POINT + 5%
			case ESPORT_ID_LEVEL_SUPERSTAR3 :	// EXP + 5% , POINT + 5%
			case ESPORT_ID_LEVEL_SUPERSTAR4 :	// EXP + 5% , POINT + 5%
				r32AddExpEvent	+= 0.05001f;
				r32AddPotEvent	+= 0.05001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;

			case ESPORT_ID_LEVEL_WORLDCLASS1 :	// EXP + 10% , POINT + 5%
			case ESPORT_ID_LEVEL_WORLDCLASS2 :	// EXP + 10% , POINT + 5%
			case ESPORT_ID_LEVEL_WORLDCLASS3 :	// EXP + 10% , POINT + 5%
				r32AddExpEvent	+= 0.10001f;
				r32AddPotEvent	+= 0.05001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;

			case ESPORT_ID_LEVEL_LEGEND1 :		// EXP + 10% , POINT + 10%
			case ESPORT_ID_LEVEL_LEGEND2 :		// EXP + 10% , POINT + 10%
			case ESPORT_ID_LEVEL_HALLOFFAME :	// EXP + 10% , POINT + 10%
				r32AddExpEvent	+= 0.10001f;
				r32AddPotEvent	+= 0.10001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;
			default : break;
			}
		}
		break;
	case LOCALE_CODE_TURKEY :
		{
			// 초보계급 빠른 레벨업 이벤트
			switch( m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._rank )
			{
			case RANK_00:
			case RANK_01:
			case RANK_02:
			case RANK_03:
				r32AddExpEvent	+= 0.30001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;
			case RANK_04:
			case RANK_05:
				r32AddExpEvent	+= 0.50001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;
			}
		}
		break;
	case LOCALE_CODE_TAIWAN :
		{
			// 초보계급 빠른 레벨업 이벤트
			switch( m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._rank )
			{
			case RANK_00:
			case RANK_01:
				r32AddExpEvent	+= 1.00001f;	// 100%
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;
			case RANK_02:
			case RANK_03:
			case RANK_04:
			case RANK_05:
				r32AddExpEvent	+= 0.80001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;
			case RANK_06:
				r32AddExpEvent	+= 0.50001f;
				m_Info._aui8ResultIcon[i32SlotIdx] |= RESULT_ICON_EVENT; 
				break;
			}
		}
		break;
	}

	REAL32 r32AddExp	= 0.0001f;
	REAL32 r32AddPoint	= 0.0001f;

	if( 0.001f < r32AddExpItem )	{	m_aSlotBonus[ i32SlotIdx ].ui16ExpItem		= (UINT16)(m_Info._aui16AccExp[i32SlotIdx]		* r32AddExpItem);	r32AddExp		+= r32AddExpItem;	}
	if( 0.001f < r32AddPotItem )	{	m_aSlotBonus[ i32SlotIdx ].ui16PointItem	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx]	* r32AddPotItem);	r32AddPoint	+= r32AddPotItem;	}

	if( 0.001f < r32AddExpPCCafe )	{	m_aSlotBonus[ i32SlotIdx ].ui16ExpPCCafe	= (UINT16)(m_Info._aui16AccExp[i32SlotIdx]		* r32AddExpPCCafe);	r32AddExp		+= r32AddExpPCCafe;	}
	if( 0.001f < r32AddPotPCCafe )	{	m_aSlotBonus[ i32SlotIdx ].ui16PointPCCafe	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx]	* r32AddPotPCCafe);	r32AddPoint	+= r32AddPotPCCafe;	}

	if( 0.001f < r32AddExpEvent )	{	m_aSlotBonus[ i32SlotIdx ].ui16ExpEvent		= (UINT16)(m_Info._aui16AccExp[i32SlotIdx]		* r32AddExpEvent);	r32AddExp		+= r32AddExpEvent;	}
	if( 0.001f < r32AddPotEvent )	{	m_aSlotBonus[ i32SlotIdx ].ui16PointEvent	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx]	* r32AddPotEvent);	r32AddPoint	+= r32AddPotEvent;	}

	m_Info._aui16EventExp[i32SlotIdx]	= (UINT16)(m_Info._aui16AccExp[i32SlotIdx] * r32AddExp); 
	m_Info._aui16EventPoint[i32SlotIdx]	= (UINT16)(m_Info._aui16AccPoint[i32SlotIdx] * r32AddPoint);

	// 어뷰저가 아닐때만 exp와 point를 적용합니다.
	if(  ABUSING_LEVEL_NONE == m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel )
	{
		UINT32 ui32Point = m_Info._aui16AccPoint[i32SlotIdx]	+ m_Info._aui16EventPoint[i32SlotIdx];

		m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._exp	+= m_Info._aui16AccExp[i32SlotIdx]		+ m_Info._aui16EventExp[i32SlotIdx]; 
		m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._point	+= ui32Point;

		m_Info._pSession[i32SlotIdx]->m_UserInfoDaily._exp	+= m_Info._aui16AccExp[i32SlotIdx]		+ m_Info._aui16EventExp[i32SlotIdx];
		m_Info._pSession[i32SlotIdx]->m_UserInfoDaily._point	+= ui32Point;

		// 배틀에서 얻은 포인트 ZLog 기록
		INT32 i32GetRoute = 1;
		g_pModuleContextNC->LogSendMessage( i32WorkIdx, LOG_MESSAGE_U_GET_POINT, m_Info._pSession[i32SlotIdx], &ui32Point, &i32GetRoute );
	}

	if( (FALSE == m_Info._pSession[i32SlotIdx]->IsGMUser()) && (FALSE == m_Info._pSession[i32SlotIdx]->IsMODUser()) )
	{
		// 계급 변화가 있는지 확인
		m_Info._pSession[i32SlotIdx]->CheckRankUp( i32WorkIdx );
	}

	// 이부분이 있어야 DB에 저장 됩니다. 
	m_Info._pSession[i32SlotIdx]->ChangeDataBaseInfo(SAVE_DATABASE_RECORD | SAVE_DATABASE_BASEINFO);

	return; 
}

void CRoom::_SendMainChange(void)
{
	// 방장 소환핵을 막기위한 임시 방편 - 서동권
	m_ui8Hack_Count = 0;

	//Make Network Info 
#ifdef DEF_OBSERVER_MODE
	NET_UDP_INFO NetInfo[SLOT_ALL_COUNT]; 	

	for(INT32 i = 0; i < SLOT_ALL_COUNT; i++)
#else
	NET_UDP_INFO NetInfo[SLOT_MAX_COUNT]; 	

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
#endif
	{
		if( m_Info._SlotInfo[i]._State > SLOT_STATE_READY)
		{
			i3mem::Copy( &NetInfo[i], &m_Info._pSession[i]->m_UdpNetInfo, sizeof(NET_UDP_INFO) ); 			
		}
		else
		{
			i3mem::FillZero( &NetInfo[i], sizeof(NET_UDP_INFO) ); 
		}
	}

	i3NetworkPacket Packet( PROTOCOL_BATTLE_LEAVEP2PSERVER_ACK );	
	Packet.WriteData( &m_Info._i32MainSlotIdx,		sizeof(INT32)); 
	Packet.WriteData( NetInfo,				sizeof(NetInfo));
#ifdef DEF_OBSERVER_MODE 
	for(INT32 i = 0; i < SLOT_ALL_COUNT; i++)
#else
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
#endif
	{
		if(m_Info._SlotInfo[i]._State > SLOT_STATE_CLOSE)
		{
			if( m_Info._SlotInfo[i]._State > SLOT_STATE_READY)
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
	INT32 i32Rv = SERVER_VALUE_UNKNOWN; 

	i32Rv = _CheckMainBest( eSlotState );

	if( SERVER_VALUE_UNKNOWN == i32Rv ) return SERVER_VALUE_UNKNOWN;

	m_Info._i32MainSlotIdx	= i32Rv;
	m_i64LogMainUserDbIdx = m_Info._pSession[ i32Rv ]->m_i64UID;

	_SetMainSlot();

	return i32Rv;
}

void	CRoom::_SetMainSlot()
{
	//Set Main Name
	if( m_Info._SlotInfo[ m_Info._i32MainSlotIdx ]._State == SLOT_STATE_READY )	m_Info._SlotInfo[ m_Info._i32MainSlotIdx ]._State = SLOT_STATE_NORMAL; 
	i3mem::Copy( m_Info._InfoAdd._Nick, m_Info._pSession[ m_Info._i32MainSlotIdx ]->GetNickName(), NET_NICK_NAME_SIZE);
#ifdef USE_COLOR_NICK2
	m_Info._InfoAdd._NicklColor = m_Info._pSession[ m_Info._i32MainSlotIdx ]->GetNickColor();
#endif

	//방장이 리얼아피인지?
	if( m_Info._pSession[m_Info._i32MainSlotIdx]->IsRealIp() )
		m_Info._InfoBasic._InfoFlag |= ROOM_INFO_FLAG_REAL_IP;
	else
		m_Info._InfoBasic._InfoFlag &= ~ROOM_INFO_FLAG_REAL_IP;

}

void CRoom::_OnUpdateRoom_Ready_M( UINT32 ui32ServerTime )
{
	if( SLOT_STATE_READY != m_Info._SlotInfo[m_Info._i32MainSlotIdx]._State )	return;

	BOOL bStartLoding = FALSE; 
	if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
	{
		bStartLoding	= TRUE;
	}
	else if( m_pRoomMode->CheckLoadingStart() )
	{
		bStartLoding	= TRUE;
	}

#ifdef USE_SOLO_PLAY
	bStartLoding = TRUE;
#endif
	if( bStartLoding )
	{
		switch( m_pRoomMode->GetStartOption() )
		{
		case ROOM_START_DIRECT:
			_SetState_M( ROOM_STATE_LOADING, ui32ServerTime );
			break;
		case ROOM_START_COUNTDOWN:
			_SetState_M( ROOM_STATE_COUNTDOWN, ui32ServerTime );
			break;
		}
	}
	else
	{
		m_Info._SlotInfo[ m_Info._i32MainSlotIdx ]._State = SLOT_STATE_NORMAL;
		_SendSlotInfo(NULL);
	}
}

void CRoom::_OnUpdateRoom_Countdown_M( UINT32 ui32ServerTime )
{
	// 방장이 Ready를 풀었다면, 카운트다운을 중지한다.
	if( SLOT_STATE_READY > m_Info._SlotInfo[ m_Info._i32MainSlotIdx ]._State )
	{
		_SetState_M( ROOM_STATE_READY, ui32ServerTime );
		m_i8StartCountdown	= 0;
		_SendBattleCountdown( -1 );
		return;
	}

	// 상대팀의 Ready가 한명도 없다면카운트다운을 중지한다.
	BOOL beOpponent = FALSE;
	INT32 i32MainOtherTeam = (m_Info._i32MainSlotIdx+1) % TEAM_COUNT;	// 방장의 다른 팀을 구합니다.
	for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i += 2 )
	{
		if( SLOT_STATE_READY <= m_Info._SlotInfo[i]._State )
		{
			beOpponent = TRUE;
			break;
		}
	}
#ifdef USE_SOLO_PLAY
	// 솔로 플레이
	beOpponent = TRUE;
#endif
	if( FALSE == beOpponent )
	{
		_SetState_M( ROOM_STATE_READY, ui32ServerTime );
		m_i8StartCountdown	= 0;
		_SendBattleCountdown( -2 );

		// 방장 슬롯을 Normal 로 변경 시켜 줍니다.
		m_Info._SlotInfo[m_Info._i32MainSlotIdx]._State = SLOT_STATE_NORMAL;
		return;
	}

	// 카운트다운
	if( m_ui32OldCountdownTime != ui32ServerTime )
	{				
		if( m_i8StartCountdown < 1 )
		{
			_SetState_M( ROOM_STATE_LOADING, ui32ServerTime );
			return;
		}

		m_i8StartCountdown--;
		m_ui32OldCountdownTime = ui32ServerTime;
	}
}

void CRoom::_OnUpdateRoom_Loading_M( UINT32 ui32ServerTime )
{
	bool bIsBattle = true;

	// 방장이 로딩 끝날때까지 기다립니다. // 추후에 데디가 들어가면 다른 식으로 변경 해야합니다. 20110725 김현우
	if( SLOT_STATE_PRESTART == m_Info._SlotInfo[m_Info._i32MainSlotIdx]._State )
	{
		if( m_bIsBattleRoomCreated )		// false 라면 배틀서버에서 방을 생성하지 못한겁니다. or 응답이 아예 오지 않았거나.
		{	
			_SetState_M( ROOM_STATE_PRE_BATTLE, ui32ServerTime );
		}
		else
		{
			bIsBattle = false;
		}
	}

	// LOADING_TIME_OUT(60)초 동안 방장이 로딩이 완료 되지 않으면 방을 폭파 시킵니다.
	if( (m_ui32StateStartTime + LOADING_TIME_OUT) <= ui32ServerTime )
	{
		bIsBattle = false;
	}

	if( false == bIsBattle )
	{	//모두 방으로 돌아간다.	// Ready 에서 SlotInfo 를 보낼 방법을 찾아봅시다.
		_SetState_M( ROOM_STATE_READY, ui32ServerTime );
		_SendSlotInfo(NULL);
	}
}

void CRoom::_OnUpdateRoom_PreBattle_M( UINT32 ui32ServerTime )
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//모든 유저들이 정상적으로 시작
	bool bIsBattle		= true;
	bool bStartMatch	= false;
	
	if( SLOT_STATE_BATTLE_READY == m_Info._SlotInfo[m_Info._i32MainSlotIdx]._State ) 
	{	//방장이 시작할 상태가 되었을때
		bStartMatch = m_pRoomMode->CheckMatchStart();

		if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
		{	// 훈련전일경우 혼자서도 시작할 수 있다.
			bStartMatch = true; 
		}

#if defined USE_SOLO_PLAY
		bStartMatch = true;
#endif
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 게임 참가 유저들이 모두 나갈 경우, 룸상태를 다시 대기상태로 변경
	INT32 i32ReadyCount = 0;
	for(INT32 Loop = 0; Loop < SLOT_MAX_COUNT; Loop++)
	{
		if( SLOT_STATE_READY < m_Info._SlotInfo[Loop]._State )
		{
			i32ReadyCount++;
			break;
		}
	}
	if(0 == i32ReadyCount) { bIsBattle = false; }

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//정상적으로 시작하지 못함.. 못들어온 유저들은 타임아웃으로 처리 한다. 
	if( (m_ui32StateStartTime + PREGAME_TIME_OUT) <= ui32ServerTime )
	{
		// 방장이 준비가 되지 않았기 때문에 무조건 방을 폭파합니다.
		if( SLOT_STATE_BATTLE_READY != m_Info._SlotInfo[m_Info._i32MainSlotIdx]._State ) 
		{
			bIsBattle = false;
		}
		else
		{
			switch( m_pRoomMode->GetPreStartOption() )
			{
			case ROOM_PRESTART_ONE_WAIT:
				{
					bIsBattle = false;
					break;
				}
				break; 
			case ROOM_PRESTART_ALL_WAIT:
				{
					bool bOtherTeamReady = false;

					INT32 i32MainOtherTeam = (m_Info._i32MainSlotIdx+1) % TEAM_COUNT;	// 방장의 다른 팀을 구합니다.
					for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i += 2 )
					{
						if( SLOT_STATE_BATTLE_READY != m_Info._SlotInfo[i]._State ) continue;
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
#ifdef DEF_OBSERVER_MODE
						for( INT32 i = 0 ; i < SLOT_ALL_COUNT ; i++ )
#else
						for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
#endif
						{
							// 아직 홀펀칭이 완료 되지 않은 유저는 방으로 보냄
							if( SLOT_STATE_NORMAL < m_Info._SlotInfo[i]._State && SLOT_STATE_BATTLE_READY > m_Info._SlotInfo[i]._State )
							{
								m_Info._SlotInfo[i]._State = SLOT_STATE_NORMAL; 
								m_Info._pSession[i]->SendPacketMessage(&SendKickPacket);					//킥 메시지를 보낸다. 
							}
						}
						bStartMatch = true;
					}
					else
					{	//End Mission 						
						bIsBattle = false;
					}
				}
				break; 
			}
		}
	}

	if( bStartMatch )
	{	// 모두 완료 되었다면 시작합니다.
		_SetState_M( ROOM_STATE_BATTLE, ui32ServerTime );
		_SendSlotInfo( NULL );

		//배틀 시작 패킷을 보낸다.
#ifdef DEF_OBSERVER_MODE
		for(INT32 i = 0; i < SLOT_ALL_COUNT; i++)
#else
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
#endif
		{
			if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State ) continue;

			_SendStartBattle( m_Info._pSession[i], TRUE );
			g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_R_BATTLE_START, m_Info._pSession[i], this );
		}

		// 라운드전일때 배틀 시작 메시지를 보낸다. 
		m_Info._i32MiStateType = MISSION_STATE_UNKNOWN; 
		_SetMissionState_M( MISSION_STATE_PRE_BATTLE );
	}

	if( false == bIsBattle )
	{
		_SetState_M( ROOM_STATE_READY, ui32ServerTime );
		_SendSlotInfo(NULL);
	}
}

void CRoom::_OnUpdateRoom_Battle_M( UINT32 ui32ServerTime )
{
#ifdef USE_GM_PAUSE
	_UpdateBattleTime();

	if( GetRoomBattleTime() > BATTLE_TIME_OUT )
	{	// 1시간이 지나면 배틀을 중지 합니다.  //미션전 시간이 변경되면 수정 해야합니다. 				
		m_bIsBattle = FALSE; 
	}

	if ( m_bGMPause )
	{
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			m_Info._pSession[i]->m_ui32ATLastRecv = g_pContextMain->GetServerTime();
	}

	//해킹 유저를 판단합니다.
	INT32 i32CheckTime = (INT32)(GetRoomBattleTime() - 60); 
#else
	if( (m_ui32StateStartTime + BATTLE_TIME_OUT) <= ui32ServerTime )
	{	// 1시간이 지나면 배틀을 중지 합니다.  //미션전 시간이 변경되면 수정 해야합니다. 				
		m_bIsBattle = FALSE; 
	}

	//해킹 유저를 판단합니다.
	UINT32 ui32CheckTime = g_pContextMain->GetServerTime() - 60; 
#endif

	if( MISSION_STATE_BATTLE == m_Info._i32MiStateType )
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i ++ )
		{
			if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State )					continue;
#ifdef USE_GM_PAUSE
			if( (INT32)m_Info._pSession[i]->m_ui32BattleCheckTime > i32CheckTime )	continue;
#else
			if( m_Info._pSession[i]->m_ui32BattleCheckTime > ui32CheckTime )		continue;
#endif
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
			m_i8WinTeam = TEAM_BLUE_WIN;
		}
		else if( 0 == (m_Info._ui16BattleUser & BLUE_TEAM_MASK) && (m_Info._ui16BattleUser & RED_TEAM_MASK) )
		{	// Blue 팀만 다 나갔을 경우
			m_i8WinTeam = TEAM_RED_WIN;
		}
		else
		{	// Draw 일 경우 이긴팀을 찾아줍니다.
			m_i8WinTeam = m_pRoomMode->GetWinTeam();
		}

#ifdef NEW_EXPPOINT
		// 현재 진행중이던 라운드의 데이터들 처리
		m_pRoomMode->SetRoundDataCount();
#endif		

		m_ui16BattleEndUserFlag  = _ResultUpdate_M( m_i8WinTeam, FALSE );

		_SetState_M( ROOM_STATE_BATTLE_END, ui32ServerTime );
		_SendSlotInfo(NULL);
		return;
	}

	//체크 정상 종료
#ifdef USE_GM_PAUSE
	m_bIsBattleEnd = _OnUpdateRoundMany_M();
#else
	m_bIsBattleEnd = _OnUpdateRoundMany_M( ui32ServerTime );
#endif

	//정상적인 배틀 종료 입니다. 
	if( m_bIsBattleEnd )
	{
		m_i8WinTeam = m_pRoomMode->GetWinTeam();
		m_ui16BattleEndUserFlag = _ResultUpdate_M( m_i8WinTeam, TRUE );			// 결과 업데이트 
		_SetState_M( ROOM_STATE_BATTLE_END, ui32ServerTime );					// 상태 변경
		_SendSlotInfo(NULL);
	}

	//Send Ping 
	if( m_ui32SendPingTime + BATTLE_SEND_PING_TIME < g_pContextMain->GetServerTime() )
	{
		_SendPing_M(); 
		m_ui32SendPingTime = g_pContextMain->GetServerTime(); 
	}

	//Death 
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i ++ )
	{
		if( m_abDeath[i] == false )continue; 
		
		m_abDeath[i]			= false; 
		m_abAlive[i]			= false;
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

	//Respawn 
	CUserSession * pSession; 
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i ++ )
	{
#ifdef USE_RESPAWN_SKIP
		if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )	continue;
#endif
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State )	continue;
		if( m_abReSpawn[i] == false )							continue;
		m_abReSpawn[i]	= false; 
		m_abAlive[i]	= true;
		SET_USER_ALIVE( i );					// 유저 Alive 처리

		if(m_Info._i32MiStateType == MISSION_STATE_BATTLE)m_pRoomMode->UpdateUserState( i );


		//---------------------------------------------------------------------------------------------------

		
		pSession = m_Info._pSession[i];

		if(pSession->m_bFirstObserver == TRUE)
		{
			pSession->m_ui32FirstObserverTime =
#ifdef USE_GM_PAUSE
												GetRoomBattleTime()
#else
												g_pContextMain->GetServerTime() 
#endif
																				- pSession->m_ui32BattleStartTime;
			pSession->m_bFirstObserver = FALSE;
		}
		// 데디 사용중일땐 데디에 주무기 아이템ID를 전송합니다. 2011.12.15 안국정		
		SendRespawnToDedi( MAIN_THREAD_IDX, i, pSession->m_aui32UseWeaponList, m_i32TotalRespawnIdx, pSession->m_i32LogD_RespawnCount );
		
		i3NetworkPacket SendPacket( PROTOCOL_BATTLE_RESPAWN_ACK );
		SendPacket.WriteData( &i,									sizeof(INT32) );						//Slot Idx 
		SendPacket.WriteData( &m_i32TotalRespawnIdx,				sizeof(INT32) );						//Respawn Idx 
		SendPacket.WriteData( &pSession->m_i32LogD_RespawnCount,	sizeof(INT32) );						//Respawn Idx 
		SendPacket.WriteData( pSession->m_aui32UseWeaponList,		sizeof(UINT32) * WEAPON_USAGE_COUNT );	//Weapon Idx
		SendPacket.WriteData( pSession->m_aui8UseWeaponEndurance,	sizeof(UINT8)*WEAPON_USAGE_COUNT );		//Weapon Arg
		SendPacket.WriteData( &pSession->m_UseCharaList,			sizeof(USER_INFO_EQUIPMENT_CHARA) );	// 장비한 캐릭터 정보
		m_pRoomMode->AddRespawnPacket( &SendPacket, pSession );
		_SendPacketBattlePlayer( &SendPacket );
		I3TRACE("RespawnAck (Room:%d, SlotIdx:%d)\n", pSession->m_i32RoomIdx, i );

		// 리스폰 시간 기록
		pSession->m_ui32RespawnTime =
#ifdef USE_GM_PAUSE
										GetRoomBattleTime();
#else
										g_pContextMain->GetServerTime();
#endif

		m_i32TotalRespawnIdx++;
	}

	//Dino Info Send 
	for( INT32 i = 0; i < SLOT_MAX_COUNT; i ++ )
	{
		if( m_Info._abDinoStateUpdate[i] )
		{
			if(isUseBattleServer())
				g_pModuleControl->BattleChangeDinoInfo(MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx, i, &m_Info );
			

			m_Info._abDinoStateUpdate[i] = false; 
		}
	}

	// 투표관련 추가 2011.05.25, 05.26 안국정
	// 계속 반복적으로 투표시간이 지났는지, 미투표유저가 나갔는지 검사합니다.
	// 투표중인 상태에서 투표가 종료되었을 경우에만 TRUE를 리턴하고
	// 투표중이 아니거나, 투표중이지만 투표가 진행되고 있는 중이면 FALSE를 리턴합니다.
	
	if ( !m_bVoting ) return;
	// 잔여 투표가능자가 0인지 혹은 제한 시간이 다 되었는지 검사
#ifdef USE_GM_PAUSE
	if ( 0 == m_ui32VoteState || GetRoomBattleTime() >= m_ui32BeginTime + VOTE_POSSIBLE_TIME )
#else
	if ( 0 == m_ui32VoteState || g_pContextMain->GetServerTime() >= m_ui32BeginTime + VOTE_POSSIBLE_TIME )
#endif
	{
		CloseVote();
	}
	// 피신고자가 나갔는지 검사합니다. 나갔다면 투표 종료
	else if	( m_Info._SlotInfo[m_i32TargetSlot]._State != SLOT_STATE_BATTLE )
	{
		m_bVoting	= FALSE;

		// 피신고인을 제외한 모든 유저에게 피신고자의 Room Leave로 인해 투표가 중단되었음을 통보하느 패킷을 전송합니다.
		for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_NOTIFY_KICKVOTE_CANCEL_ACK );
			if ( m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE )
			{
				m_Info._pSession[i]->SendPacketMessage( &SendPacket );
			}
		}
	}
}

#ifdef USE_GM_PAUSE
BOOL CRoom::_OnUpdateRoundMany_M()
#else
BOOL CRoom::_OnUpdateRoundMany_M( UINT32 ui32ServerTime )
#endif
{
	BOOL	bIsBattleEnd  = FALSE;
	switch( m_Info._i32MiStateType )
	{
	case MISSION_STATE_PRE_BATTLE	:
		{
			if( m_pRoomMode->OnUpdatePreBattle( m_ui32BattleStateTime, 
#ifdef USE_GM_PAUSE
																		GetRoomBattleTime()
#else
																		ui32ServerTime
#endif
																							) )
			{
				_SetMissionState_M( MISSION_STATE_BATTLE ); 
			}
		}
		break; 
	case MISSION_STATE_BATTLE		: 
		{
			if( m_pRoomMode->CheckRoundEnd( 
#ifdef USE_GM_PAUSE
											GetRoomBattleTime()
#else
											ui32ServerTime
#endif
																) )
			{	// Round 종료
#ifdef NEW_EXPPOINT
				m_pRoomMode->SetRoundDataCount();
#endif
				_SetMissionState_M( MISSION_STATE_FREE );
			}
		}
		break; 
	case MISSION_STATE_FREE		:
		{
			// 게임에 참여중인 유저의 캐릭터 장비에 대해 사용 정지 상태로 전환한다
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				if( m_Info._SlotInfo[i]._State != SLOT_STATE_BATTLE ) continue;

				m_Info._pSession[i]->CharUseAllStop();
			}

			if( m_pRoomMode->CheckMatchEnd() )
			{	// 모든 라운드가 종료되어 게임을 종료합니다.
				_SetMissionState_M( MISSION_STATE_UNKNOWN );  
				bIsBattleEnd = TRUE; //게임 엔드
			}
			else
			{
				if( (m_ui32BattleStateTime + MISSION_FREE_TIME) <=
#ifdef USE_GM_PAUSE
																	GetRoomBattleTime()
#else
																	ui32ServerTime
#endif
																						)
				{
					_SetMissionState_M( MISSION_STATE_RESULT );
				}
			}
		}
		break; 
	case MISSION_STATE_RESULT	:
		if( (m_ui32BattleStateTime + MISSION_RESULT_TIME)	<=
#ifdef USE_GM_PAUSE
																GetRoomBattleTime()
#else
																ui32ServerTime
#endif
																					)
		{
			_SetMissionState_M( MISSION_STATE_PRE_BATTLE ); 
		}
		break;
	default: break; 
	}
	return bIsBattleEnd;
}

BOOL CRoom::OnUpdateRoom_M( UINT32 ui32ServerTime )
{
	BOOL	bRv = FALSE;

	switch( m_Info._InfoBasic._State )
	{
	case ROOM_STATE_READY	: 
		{
			_OnUpdateRoom_Ready_M( ui32ServerTime );
		}
		break; 
	case ROOM_STATE_COUNTDOWN:
		{
			_OnUpdateRoom_Countdown_M( ui32ServerTime );
		}
		break;
	case ROOM_STATE_LOADING	: 
		{
			_OnUpdateRoom_Loading_M( ui32ServerTime );
		}
		break;
	case ROOM_STATE_PRE_BATTLE	:
		{
			_OnUpdateRoom_PreBattle_M( ui32ServerTime );
		}
		break; 
	case ROOM_STATE_BATTLE	: 
		{
			_OnUpdateRoom_Battle_M( ui32ServerTime );
		}
		break; 
	default: 
		{
			_SetState_M( ROOM_STATE_READY, ui32ServerTime );
			_SendSlotInfo(NULL);
		}
		break; 
	}

	// 게임 모드별 업데이트 주기적으로 실행할 업데이트 처리
	m_pRoomMode->periodicUpdateRoomState( 
#ifdef USE_GM_PAUSE
											GetRoomBattleTime()
#else
											ui32ServerTime 
#endif
																	);

	//////////////////////////////////////////////////////////////////////////////////
	// Check Time Out User; 
	if( m_ui32MCTimeOutStartTime > 0) 
	{			
		if( (m_ui32MCTimeOutStartTime + ROOM_CHECK_MAINCLIENT_TIMEOUT) < ui32ServerTime )
		{		
			if( (m_ui16MCTimeOutStartUserFlg & m_ui16MCTimeOutFlg) ==  m_ui16MCTimeOutStartUserFlg)
			{//방장이 타임아웃으로 나감	
				_BattleTimeOutClient_M( m_Info._i32MainSlotIdx );
			}
			else
			{//유저가 타임아웃으로 나감 
#ifdef DEF_OBSERVER_MODE
				UINT32 ui16Clientflag;
				for(INT32 i = 0; i < SLOT_ALL_COUNT; i ++)
#else
				UINT16 ui16Clientflag;
				for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
				{
					ui16Clientflag = 1; 
					ui16Clientflag = ui16Clientflag << i; 

					if( ui16Clientflag & m_ui16MCTimeOutFlg )
					{
						_BattleTimeOutClient_M( i );
					}
				}
			}
			_ResetTimeOut(); 
		}
	}

	return bRv; 
}

void CRoom::_SendEndBattle_M(UINT8 ui8Win, UINT16 ui16BattleEndUserFlag )
{
	i3NetworkPacket EndPacket; 

	// 방장 소환핵을 막기위한 임시 방편 - 서동권
	m_ui8Hack_Count = 0;
	// 무기 핵 체크 - 서동권
	// 배틀이 끝나면 무기리스트를 초기화 해줍니다. ( 카운트를 초기하고 리스트값들은 초기화 안합니다. )
	m_ui8RoomWeaponListCount = 0;

	//결과 보내기 
#ifdef DEF_OBSERVER_MODE
	for(INT32 i = 0; i < SLOT_ALL_COUNT; i ++)
#else
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
	{
		if( m_Info._SlotInfo[i]._State >= SLOT_STATE_LOAD )
		{
			EndPacket.Clear(); 
			EndPacket.SetProtocol( PROTOCOL_BATTLE_ENDBATTLE_ACK ); 			
			EndPacket.WriteData( &ui8Win,									sizeof(UINT8) );					//결과 화면을 위해서 보내는 데이터
			EndPacket.WriteData( &ui16BattleEndUserFlag,					sizeof(UINT16) );					//결과 화면을 위해서 보내는 데이터
			EndPacket.WriteData( &m_ui16QuestEndFlag,						sizeof(UINT16) );					//결과 화면을 위해서 보내는 데이터
			EndPacket.WriteData( m_Info._aui16AccExp,						sizeof(m_Info._aui16AccExp)  );		//결과 화면을 위해서 보내는 데이터	
			EndPacket.WriteData( m_Info._aui16AccPoint,						sizeof(m_Info._aui16AccPoint) );	//결과 화면을 위해서 보내는 데이터
			EndPacket.WriteData( m_Info._aui16EventExp,						sizeof(m_Info._aui16EventExp) );	//결과 화면을 위해서 보내는 데이터	
			EndPacket.WriteData( m_Info._aui16EventPoint,					sizeof(m_Info._aui16EventPoint) );	//결과 화면을 위해서 보내는 데이터
			EndPacket.WriteData( m_Info._aui8ResultIcon,					sizeof(m_Info._aui8ResultIcon) );	//결과 화면을 위해서 보내는 데이터
			EndPacket.WriteData( m_Info._pSession[i]->GetUserInfoBasic(),	sizeof(USER_INFO_BASIC) );			//유저 데이터
			EndPacket.WriteData( &m_Info._pSession[i]->m_UserInfoRecord,	sizeof(USER_INFO_RECORD));			//유저 데이터 
			
			I3TRACE("[PROTOCOL_BATTLE_ENDBATTLE_ACK] DEBUG Nick[%s],Win[%d],BattleEndUserFlag[%d],QuestEndFlag[%d],AccExp[%d],AccPoint[%d],EventExp[%d],EventPoint[%d],ResultIcon[%d],\n",
				m_Info._pSession[i]->m_UserInfoBasic._nick,ui8Win,ui16BattleEndUserFlag,m_ui16QuestEndFlag,m_Info._aui16AccExp,m_Info._aui16AccPoint,m_Info._aui16EventExp,m_Info._aui16EventPoint,m_Info._aui8ResultIcon);
			//GIP
			if( i % 2 == 0)	
			{
				//몇개 예외 처리 필요 캐릭터 넘버중 1만 남자입니다.  새로 캐릭터가 들어가면 추가 해야함.
				if((m_Info._pSession[i]->m_UserInfoEquipmentChara.GetRedNum() & 0x0000000F) == 1 )
					m_Info._pSession[i]->m_i32GIP_LastUseGender = 1;	//남자 
				else 
					m_Info._pSession[i]->m_i32GIP_LastUseGender = 2;	//여자 
				m_Info._pSession[i]->m_i32GIP_LastUseTeam = 1;//RED
			}
			else
			{
				if((m_Info._pSession[i]->m_UserInfoEquipmentChara.GetBlueNum() & 0x0000000F) == 1 )
					m_Info._pSession[i]->m_i32GIP_LastUseGender = 1;	//남자 
				else
					m_Info._pSession[i]->m_i32GIP_LastUseGender = 2;	//여자 				
				m_Info._pSession[i]->m_i32GIP_LastUseTeam = 2;//BLUE
			}

			if(m_Info._i32StageType == STAGE_TYPE_BOMB || m_Info._i32StageType == STAGE_TYPE_ANNIHILATION ||
				m_Info._i32StageType == STAGE_TYPE_ESCAPE || m_Info._i32StageType == STAGE_TYPE_CROSSCOUNT )
			{
				EndPacket.WriteData( m_Info._aui16TotalRoundCount,	sizeof( m_Info._aui16TotalRoundCount ) );
				EndPacket.WriteData( m_Info._aui8AccMissionCount,	sizeof( m_Info._aui8AccMissionCount ) );
			}


			if ( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
			{
				EndPacket.WriteData( m_Info._aui16AccTrainingScore,				sizeof( m_Info._aui16AccTrainingScore ) );				//결과 화면을 위해서 보내는 데이터	
			}

			EndPacket.WriteData( &m_Info._pSession[i]->m_ui8AbusingLevel,		sizeof(UINT8) );	// 어뷰징 레벨
			EndPacket.WriteData( &m_Info._pSession[i]->m_ui32AbusingPopupTime,	sizeof(UINT32) );	// 어뷰징 팝업 시간

			// 보급상자 지급 여부
			EndPacket.WriteData( m_ai8PresentSupplyBox,							sizeof( m_ai8PresentSupplyBox ) );

			m_Info._pSession[i]->SendPacketMessage(&EndPacket); 


			// 진급 포인트 시스템 추가 - 발송후 초기화
			if( m_Info._pSession[i]->m_bIsRankUp )
			{
				UINT32 ui32Rank = m_Info._pSession[i]->m_UserInfoBasic._rank;
				// 임시 계급을 사용중이면 임시 계급 변경
				if( -1 != m_Info._pSession[i]->GetCheckCashitem( MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_DISGUISE_RANK, CASHITEM_DAY_NO) ) )
				{
					ui32Rank = m_Info._pSession[i]->m_UserInfoInvItemData._wRank;
				}

				// 메신저와 클랜의 계급을 변경
				m_Info._pSession[i]->SetRankUp(MAIN_THREAD_IDX);

				i3NetworkPacket packet(PROTOCOL_BASE_RANK_UP_ACK);
				packet.WriteData(&ui32Rank,													sizeof(UINT32));
				packet.WriteData(&(m_Info._pSession[i]->GetUserInfoBasic()->_keep_rank),	sizeof(UINT32));
				packet.WriteData(&m_Info._pSession[i]->m_ui32rankUpPoint,					sizeof(UINT32));
				m_Info._pSession[i]->SendPacketMessage(&packet);

				m_Info._pSession[i]->m_bIsRankUp	= FALSE;
				m_Info._pSession[i]->m_ui32rankUpPoint	= 0;
			}
		}		
	}
}

// 상대팀 슬롯에 빈자리가 있으면 슬롯을 위치한다.
INT32 CRoom::ChangeTeam( CUserSession * pSession, INT32 i32NewTeam )
{
	INT32 i32SlotIdx = pSession->m_i32SlotIdx;

	// Slot Check
	if( 0 > i32SlotIdx )											return -1;
	if( SLOT_MAX_COUNT <= i32SlotIdx )								return -1;
	// 기본 상태가 아니면 실패처리 
	if( SLOT_STATE_NORMAL != m_Info._SlotInfo[i32SlotIdx]._State )	return -1;
	//  현재는 2팀밖에 없으므로..
	if( (0 != i32NewTeam) && (1 != i32NewTeam) )					return -1;
	// 같은 팀으로 이동은 의미 없음
	if( i32NewTeam == (i32SlotIdx % TEAM_COUNT) )					return -1;

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
		if( SLOT_STATE_EMPTY != m_Info._SlotInfo[i]._State ) continue;

		//Set New Slot 
		i3mem::Copy( &m_Info._SlotInfo[i],	&m_Info._SlotInfo[i32SlotIdx],	sizeof(SLOT_INFO) );
		m_Info._pSession[i]					= m_Info._pSession[i32SlotIdx];
		m_Info._pSession[i]->m_i32SlotIdx	= i;
		i3mem::Copy( &m_aRoomUserInfo[i],	&m_aRoomUserInfo[i32SlotIdx], sizeof(ROOM_USER_INFO) );

		//Set Old Slot
		m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_EMPTY;

		//Change Main Slot Idx
		if( i32SlotIdx == m_Info._i32MainSlotIdx )	m_Info._i32MainSlotIdx = i;
		
		ChangeInfo.ui8NewSlot		= (UINT8)i;
		ChangeInfo.ui8NewSlotState	= (UINT8)m_Info._SlotInfo[i]._State;
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
#ifdef DEF_OBSERVER_MODE
// 옵저버 슬롯에 빈자리가 있으면 슬롯을 위치한다.
INT32 CRoom::ChangeObserverSlot(CUserSession * pSession, INT32 Slotflag)
{
	INT32 Loop = 0;
	INT32 MaxLoop = 0;
	INT32 SlotIdx = pSession->m_i32SlotIdx; 
	if(1 == Slotflag)
	{// 옵저버 슬롯으로 이동
		if(SLOT_MAX_COUNT <= SlotIdx && SLOT_ALL_COUNT > SlotIdx)return SlotIdx;
	}
	else
	{// 일반 슬롯으로 이동
		if(0 <= SlotIdx && SLOT_MAX_COUNT >= SlotIdx)return SlotIdx;
	}	

	BOOL bSendSlotInfo = FALSE; 

	//새로운 슬롯을 찾는다.
	m_csSession.Enter(); 
	{
		if(1 == Slotflag)
		{// 옵저버 슬롯으로 이동(히든슬롯으로는 못들어감)
			Loop = SLOT_MAX_COUNT;
			MaxLoop = SLOT_ROOM_VIEW_COUNT;
		}	
		else
		{// 일반슬롯으로 이동
			Loop = 0;
			MaxLoop = SLOT_MAX_COUNT;
		}

		for(INT32 i = Loop; i < MaxLoop; i++)
		{
			if(m_Info._SlotInfo[i]._State == SLOT_STATE_EMPTY)
			{
				//Set New Slot 
				m_Info._pSession[i]					= m_Info._pSession[SlotIdx];
				m_Info._pSession[i]->m_i32SlotIdx	= i;
				i3mem::Copy( &m_Info._SlotInfo[i],	&m_Info._SlotInfo[SlotIdx],	sizeof(SLOT_INFO) );
				i3mem::Copy( &m_aRoomUserInfo[i],	&m_aRoomUserInfo[SlotIdx], 	sizeof(ROOM_USER_INFO) );

				//Change Main Slot Idx
				if(SlotIdx == m_Info._i32MainSlotIdx)	m_Info._i32MainSlotIdx = i; 

				//Set Old Slot
				m_Info._SlotInfo[SlotIdx].Reset();
				m_Info._SlotInfo[SlotIdx]._State = SLOT_STATE_EMPTY;
				m_aRoomUserInfo[SlotIdx].Reset();
				bSendSlotInfo = TRUE; 
				SlotIdx = i;
				break; 
			}
		}

	}
	m_csSession.Leave(); 

	if(bSendSlotInfo)
	{
		_SendNickName( NULL, SlotIdx, TRUE);
		_SendSlotInfo( NULL );
	}
	return SlotIdx; 
}

// 히든 슬롯에 빈자리가 있으며 GM이 이동가능(다른 GM이 있으면 이동 불가)
INT32 CRoom::ChangeHiddenSlot(CUserSession * pSession, INT32 Slotflag)
{
	INT32 Loop = 0;
	INT32 MaxLoop = 0;
	INT32 SlotIdx = pSession->m_i32SlotIdx; 
	if(FALSE == pSession->IsGMUser() || SLOT_ROOM_VIEW_COUNT == SlotIdx)return SlotIdx;

	BOOL bSendSlotInfo = FALSE; 

	//새로운 슬롯을 찾는다.
	m_csSession.Enter(); 
	{
		if(1 == Slotflag)
		{// 히든 슬롯으로 이동(히든슬롯으로는 못들어감)
			Loop = SLOT_ROOM_VIEW_COUNT;
			MaxLoop = SLOT_ALL_COUNT;
		}	
		else
		{// 일반슬롯으로 이동
			Loop = 0;
			MaxLoop = SLOT_MAX_COUNT;
		}

		for(INT32 i = Loop; i < MaxLoop; i++)
		{
			if( m_Info._SlotInfo[i]._State == SLOT_STATE_EMPTY)
			{
				//Set New Slot 
				m_Info._pSession[i]					= m_Info._pSession[SlotIdx];
				m_Info._pSession[i]->m_i32SlotIdx	= i;
				i3mem::Copy( &m_Info._SlotInfo[i],		&m_Info._SlotInfo[SlotIdx],		sizeof(SLOT_INFO) );
				i3mem::Copy( &m_aRoomUserInfo[i],	&m_aRoomUserInfo[SlotIdx], sizeof(ROOM_USER_INFO) );

				//Change Main Slot Idx
				if(SlotIdx == m_Info._i32MainSlotIdx)m_Info._i32MainSlotIdx = i; 

				//Set Old Slot
				m_Info._SlotInfo[SlotIdx].Reset();
				m_Info._SlotInfo[SlotIdx]._State = SLOT_STATE_EMPTY;
				m_aRoomUserInfo[SlotIdx].Reset();
				bSendSlotInfo = TRUE; 
				SlotIdx = i;
				break; 
			}
		}
	}
	m_csSession.Leave(); 

	if(bSendSlotInfo)
	{
		_SendNickName( NULL, SlotIdx, TRUE);
		_SendSlotInfo( NULL );
	}
	return SlotIdx; 
}
#endif

void CRoom::_SetState_M( UINT8 ui8State, UINT32 ui32ServerTime )
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

			if( ROOM_STATE_BATTLE_END == ui8State || ROOM_STATE_READY == ui8State )
			{
				if(isUseBattleServer())
					g_pModuleControl->BattleDestroy( MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx ); 
				ResetBattleGroupInfo();
			}
		}
	}

	switch(ui8State)
	{
	case ROOM_STATE_READY	: 	
		{
			// COUNTDOWN 에서 돌아왔을 경우에는 별다른 액션을 취하지 않습니다.
			if( ROOM_STATE_COUNTDOWN == m_Info._InfoBasic._State )	break;
						
			//만약 게임중의 상태가 있으면 바로 레디상태로 보낸다. 
			UINT32 ui32Reason = EVENT_ERROR_FIRST_HOLE;
#ifdef DEF_OBSERVER_MODE
			for(INT32 i = 0; i < SLOT_ALL_COUNT; i++)
#else
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
#endif
			{
				if( SLOT_STATE_READY > m_Info._SlotInfo[i]._State ) continue;
				
				m_Info._SlotInfo[i]._State = SLOT_STATE_NORMAL; 
				i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER );
				SendPacket.WriteData( &ui32Reason,	sizeof(UINT32) ); 
				m_Info._pSession[i]->SendPacketMessage(&SendPacket);
			}	// -- 이 전 패킷과 합칠수 있는지 확인합니다. --
		}
		break; 
	case ROOM_STATE_COUNTDOWN:
		{
			m_ui32OldCountdownTime	= ui32ServerTime;
			m_i8StartCountdown		= 5;

			i3NetworkPacket SendPacket(PROTOCOL_BATTLE_START_COUNTDOWN_ACK);
			SendPacket.WriteData( &m_i8StartCountdown, sizeof(INT8) );
			_SendPacketAllPlayer( &SendPacket );
		}
		break;
	case ROOM_STATE_LOADING	:
		{	
			m_bIsBattle				= TRUE;

			INT32	i32Temp = SLOT_STATE_LOAD; 
			m_Info._ui32FirstBattleStartUser	= 0;
			m_ui16QuestEndFlag			= 0x00;

#ifdef DEF_OBSERVER_MODE
			for( INT32 i = 0 ; i < SLOT_ALL_COUNT ; i++ )
#else
			for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
#endif
			{
				if( m_Info._SlotInfo[i]._State  > SLOT_STATE_NORMAL )
				{
					i3NetworkPacket Packet( PROTOCOL_BATTLE_READYBATTLE_ACK );
					Packet.WriteData( &i32Temp,						sizeof(INT32) );
					Packet.WriteData( &m_Info._InfoBasic._StageID,	sizeof(UINT32) );
					Packet.WriteData( &m_Info._pSession[i]->m_UseCharaList,	sizeof(USER_INFO_EQUIPMENT_CHARA ) );	  
					m_Info._ui32FirstBattleStartUser		|= (0x1 << i);  
					m_Info._pSession[i]->SendPacketMessage( &Packet );
					m_Info._SlotInfo[i]._State		= i32Temp;	
				}
			}
			_SendSlotInfo(NULL);

			m_bIsBattleRoomCreated	= false; 
			//배틀을 시작합니다. 
			if( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )
			{
				g_pModuleControl->BattleCreate( MAIN_THREAD_IDX, &m_Info, m_Info._ui32FirstBattleStartUser ); 
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
#ifdef DEF_OBSERVER_MODE
			for(INT32 i = 0 ; i < SLOT_ALL_COUNT; i++)
#else
			for(INT32 i = 0 ; i < SLOT_MAX_COUNT; i++)
#endif
			{
				if( m_Info._SlotInfo[i]._State == SLOT_STATE_PRESTART )
				{
					_SendPreStartBattle(m_Info._pSession[i], TRUE ); 
				}
			}
		}
		break; 
	case ROOM_STATE_BATTLE	: 
		{
			// 게임 최초 시작 시간 - ZLog 에 기록하기 위해서 필요합니다. - 서동권
			m_Info._ui32GameStartDate	= g_pContextMain->GetInvenTime();
			m_Info._ui32GameStartTime	= g_pContextMain->GetServerTime();

			m_Info._ui16BattleUser		= 0;		// 게임에 들어온 유저
			m_Info._ui16RoundDeathUser	= 0;		// 죽은 유저
			m_i32TotalRespawnIdx		= 0;
			m_Info._ui8MiEndType		= MISSION_END_TYPE_NONE;
#ifdef USE_GM_PAUSE
			_InitBattleTime();
#endif

			//Set Score & Record & Point
			i3mem::FillZero( m_aSlotBonus,					sizeof( SLOT_BONUS ) * SLOT_MAX_COUNT );
			i3mem::FillZero( m_Info._aui32RespawnCount,		sizeof( m_Info._aui32RespawnCount ) );
			i3mem::FillZero( m_Info._aTeamScoreTotal,		sizeof( m_Info._aTeamScoreTotal ) );
			i3mem::FillZero( m_Info._aM_UserScore,			sizeof( m_Info._aM_UserScore ) );		
			i3mem::FillZero( m_Info._aui16M_PiercingShot,	sizeof( m_Info._aui16M_PiercingShot ) );
			i3mem::FillZero( m_Info._aui16M_MassKill,		sizeof( m_Info._aui16M_MassKill ) );
			i3mem::FillZero( m_Info._aui16M_ChainStoppper,	sizeof( m_Info._aui16M_ChainStoppper ) );
			i3mem::FillZero( m_Info._aui16M_ChainHeadShot,	sizeof( m_Info._aui16M_ChainHeadShot ) );
			i3mem::FillZero( m_Info._aui16M_ChainSlugger,	sizeof( m_Info._aui16M_ChainSlugger ) );
			i3mem::FillZero( m_Info._aui16M_AccKill_1,		sizeof( m_Info._aui16M_AccKill_1 ) );	
			i3mem::FillZero( m_Info._aui16M_AccKill_2,		sizeof( m_Info._aui16M_AccKill_2 ) );	
			i3mem::FillZero( m_Info._aui16M_AccKill_3,		sizeof( m_Info._aui16M_AccKill_3 ) );	
			i3mem::FillZero( m_Info._aui16M_Headshot,		sizeof( m_Info._aui16M_Headshot ) );
			i3mem::FillZero( m_Info._aui16AccExp,			sizeof( m_Info._aui16AccExp ) );		
			i3mem::FillZero( m_Info._aui16AccPoint,			sizeof( m_Info._aui16AccPoint ) );			
			i3mem::FillZero( m_Info._aui16AccTrainingScore,	sizeof( m_Info._aui16AccTrainingScore ) );
			i3mem::FillZero( m_Info.m_aui8A_HeadShot,		sizeof( m_Info.m_aui8A_HeadShot ) );
			i3mem::FillZero( m_Info.m_aui8A_SlugShot,		sizeof( m_Info.m_aui8A_SlugShot ) );	
			i3mem::FillZero( m_Info.m_aui8A_KillCount,		sizeof( m_Info.m_aui8A_KillCount ) );		
			i3mem::FillZero( m_Info._aui8AccMissionCount,	sizeof( m_Info._aui8AccMissionCount ) );
			i3mem::FillZero( m_Info._abDinoStateUpdate,		sizeof( m_Info._abDinoStateUpdate ) );	
			i3mem::FillZero( m_Info._aui32DInoCharInfo,		sizeof( m_Info._aui32DInoCharInfo ) );	
			i3mem::FillZero( m_Info._ai32AcidBombingCount,	sizeof( m_Info._ai32AcidBombingCount ) );

#ifdef NEW_EXPPOINT
			i3mem::FillZero( m_Info._aui16Acc_KillExp,		sizeof( m_Info._aui16Acc_KillExp ) );
#endif

			for(INT32 i = 0; i< SLOT_MAX_COUNT; i++)
			{
				m_abReSpawn[i] = false; 
				m_abDeath[i] = false; 

				if( SLOT_STATE_BATTLE_READY != m_Info._SlotInfo[i]._State ) continue;
				
				if(FALSE == m_Info._SlotInfo[i]._bIsGmObserverMode) { m_Info._ui16BattleUser |= (0x01 << i); }
				m_Info._pSession[i]->m_ui32BattleCheckTime		=
#ifdef USE_GM_PAUSE
																	GetRoomBattleTime();
#else
																	ui32ServerTime;
#endif
				m_Info._SlotInfo[i]._State						= SLOT_STATE_BATTLE;
				i3mem::FillZero( m_Info._pSession[i]->m_aui8QuestCtxActiveNow, sizeof(UINT8)*MAX_QUEST_PER_CARD_NEW );
				m_Info._pSession[i]->m_ui16ActiveIndexCardNow	= 0;

				// 임무카드
				SetUserMissionCard( m_Info._pSession[i], ACTION_ID_STAGE_ENTER );
				// 어뷰징 체크 - 배틀 시작 시간 설정.
				m_Info._pSession[i]->m_ui32BattleStartTime =
#ifdef USE_GM_PAUSE
																0;
#else
																ui32ServerTime;
#endif
				m_abAlive[i]			= false;
			}

			m_pRoomMode->SetMatchStart();

			// 훈련전 설정
			if ( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
			{
				m_Info.m_i8currentDifficultyLevel = m_Info.m_i8difficultyLevel;

				for( UINT32 i = 0 ; i < SLOT_MAX_COUNT; i++ )
				{
					m_Info.m_aui32AILevel[ i ] = m_Info.m_i8currentDifficultyLevel;
				}
			}

			// 보급상자
			i3mem::FillZero( m_ai8PresentSupplyBox, sizeof(INT8)*SLOT_MAX_COUNT );
			m_ui32SupplyBoxResultReq = 0;

			//로그 배틀 시작
			g_pContextMain->SetStartBattleTime_NC_LOG(); 

			m_ui32SendPingTime		= ui32ServerTime;
			m_ui16BattleUser		= m_Info._ui16BattleUser;
		}
		break;
	case ROOM_STATE_BATTLE_END	: 
		{
#ifdef USE_GM_PAUSE
			if ( m_bGMPause )
			{
				m_bGMPause = FALSE;
				g_pModuleControl->BattleGMPause( MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx, false );
				SendGMResume();
			}
#endif

			_SendEndBattle_M( m_i8WinTeam, m_ui16BattleEndUserFlag );							// 엔드 패킷 보내기 	

			//Add BattleCount
			m_ui32LogBattleCount++; 

			//TimeOut Check 
			_ResetTimeOut(); 

			//For Mission
			m_Info._InfoAdd._NowRoundCount	= 0;
			m_Info._InfoAdd._NowIngTime		= 0; 
			for(INT32 i = 0; i < TEAM_COUNT; i++)m_Info._aui16TotalRoundCount[i] = 0;

			m_bGMPause = FALSE;

			//로그 배틀 종료
			g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_R_BATTLE_END_ROOM, NULL, this); 



			//Set User State 
#ifdef DEF_OBSERVER_MODE
			for(INT32 i = 0; i < SLOT_ALL_COUNT; i ++)
#else
			for(INT32 i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
			{
				m_aRoomUserInfo[i]._bChangeMain = FALSE;

				if( m_Info._SlotInfo[i]._State > SLOT_STATE_NORMAL )//11.25일 변경
				{
					m_Info._pSession[i]->m_bIsBattleComplete	= TRUE;
					m_Info._SlotInfo[i]._State					= SLOT_STATE_NORMAL;
					m_Info._pSession[i]->m_ui8LoadingStart		= LOAD_START_BATTLE_END;
					m_Info._pSession[i]->SetPosition(GAME_TASK_READY_ROOM);
					INT32 i32EndType;

					if(m_i8WinTeam == TEAM_DRAW)
					{
						i32EndType = ROOM_ENDTYPE_DRAW;
					}
					else
					{
						if(m_i8WinTeam == (i%2))
						{
							i32EndType = ROOM_ENDTYPE_WIN;
						}
						else
						{
							i32EndType = ROOM_ENDTYPE_LOSE;
						}
					}

					g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_U_R_BATTLE_END, m_Info._pSession[i], this, &i32EndType );

					m_Info._pSession[i]->CheckInventory_MAU(); 

					//Set Rank
					m_Info._SlotInfo[i]._Rank	 = m_Info._pSession[i]->m_UserInfoBasic._rank;

					// 어뷰저는 강제로 로비로 이동 시킨다.	
					if ( ABUSING_LEVEL_NONE < m_Info._pSession[i]->m_ui8AbusingLevel ) 
					{
						g_pRoomManager->OnLeaveRoom(m_Info._pSession[i], m_Info._pSession[i]->m_i32ChannelNum, MAIN_THREAD_IDX, ROOM_ENDTYPE_HACKING );
					}

					// 어뷰징 정보 초기화

					m_Info._pSession[i]->m_ui32AbusingPopupTime = 0;
					m_Info._pSession[i]->m_ui32BattleStartTime	= 0;
					m_Info._pSession[i]->m_ui32PlayTime			= 0;
				}	
			}

			//Set State 강제로 레디로 변경 
			ui8State = ROOM_STATE_READY; 
		}
		break; 
	}

	m_Info._InfoBasic._State	= ui8State;
	m_ui32StateStartTime		= ui32ServerTime;
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
			if ( m_Info._InfoAdd._NowRoundCount < 64 )
				m_aui32RecordRoundTime[m_Info._InfoAdd._NowRoundCount] = g_pContextMain->GetServerTime();
			m_Info._InfoAdd._NowRoundCount++; 
			m_Info._ui32BattleStartTime =
#ifdef USE_GM_PAUSE
											GetRoomBattleTime();
#else
											g_pContextMain->GetServerTime();
#endif
			//0. 남은 시간 계산 	
			UINT8	ui8EndTimeIdx	= (m_Info._InfoAdd._SubType & 0xF0) >> 4; 
			UINT32	ui32RemainTime	= g_TeamTimeTable[ui8EndTimeIdx]; 

			bool bTeamChange = m_pRoomMode->RoundStart( GetRoomBattleTime() );

			//라운드 시작 패킷 
			if(isUseBattleServer())
				g_pModuleControl->BattleRoundStart( MAIN_THREAD_IDX, &m_BattleRoomInfo, m_ui32BattleRoomIdx, &m_Info, bTeamChange );

			//Send Message
			i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_ROUND_START_ACK );
			SendPacket.WriteData( &m_Info._InfoAdd._NowRoundCount,	sizeof(UINT8) );
			SendPacket.WriteData( &ui32RemainTime,					sizeof(UINT32));	
			SendPacket.WriteData( &m_Info._ui16RoundStartUser,		sizeof(UINT16) );
			_SendPacketBattlePlayer( &SendPacket );
		}
		break; 
	case MISSION_STATE_FREE		: 
		{
			//Send Message 
			i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_ROUND_END_ACK );
			SendPacket.WriteData( &m_Info._ui8RoundWinTeam,			sizeof(UINT8) );
			SendPacket.WriteData( &m_Info._ui8MiEndType,			sizeof(UINT8) );
			SendPacket.WriteData( &m_Info._aui16TotalRoundCount,	sizeof( m_Info._aui16TotalRoundCount ));
			_SendPacketBattlePlayer( &SendPacket );

			// 라운드가 끝날때 그 라운드에서 실제 플레이한 시간 계산
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State ) continue;

				if( m_Info._pSession[i]->m_ui32RespawnTime > 0 )
					m_Info._pSession[i]->m_ui32AccActualBattleTime +=
#ifdef USE_GM_PAUSE
																		GetRoomBattleTime()
#else
																		g_pContextMain->GetServerTime()
#endif
																										- m_Info._pSession[i]->m_ui32RespawnTime;
			}
		}
		break; 
	case MISSION_STATE_RESULT	: 
		{

		}
		break; 
	default: break; 
	}

	m_Info._i32MiStateType	= ui8State; 
	m_ui32BattleStateTime	=
#ifdef USE_GM_PAUSE
								GetRoomBattleTime();
#else
								g_pContextMain->GetServerTime(); 
#endif
	return; 
}

void CRoom::_SetMissionStatePreBattle()
{
	m_Info._ui16RoundStartUser  = 0;		// 게임을 시작한 유저 
	m_Info._ui16RoundDeathUser	= 0;		// 죽은 유저

	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State ) continue;
		m_Info._pSession[i]->m_ui32BattleCheckTime =
#ifdef USE_GM_PAUSE
													GetRoomBattleTime();
#else
													g_pContextMain->GetServerTime();
#endif

		m_Info._ui16RoundStartUser	|= (0x01 << i);
		m_abAlive[i]					= false;
	}

	i3mem::FillZero( m_Info._aui32RespawnCount,		sizeof( m_Info._aui32RespawnCount ) );
	i3mem::FillZero( m_abReSpawn,					sizeof(	m_abReSpawn ) );	
	i3mem::FillZero( m_abDeath,						sizeof(	m_abDeath ) );				
	i3mem::FillZero( m_Info._aUserScoreRound,		sizeof(	m_Info._aUserScoreRound ) );
	i3mem::FillZero( m_Info._aTeamScoreRound,		sizeof(	m_Info._aTeamScoreRound ) );	
	i3mem::FillZero( m_Info.m_aui8A_KillCount,		sizeof(	m_Info.m_aui8A_KillCount ) );	
	i3mem::FillZero( m_Info.m_aui8A_HeadShot,		sizeof(	m_Info.m_aui8A_HeadShot ) );	
	i3mem::FillZero( m_Info.m_aui8A_SlugShot,		sizeof(	m_Info.m_aui8A_SlugShot ) );	
	i3mem::FillZero( m_Info._ai32AcidBombingCount,	sizeof(	m_Info._ai32AcidBombingCount ) );


	//Check Round Start User;
	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_ROUND_PRE_START_ACK );
	SendPacket.WriteData( &m_Info._ui16RoundStartUser,	sizeof(UINT16) );
	SendPacket.WriteData( &m_Info._MiDinoInfo,			sizeof(DINO_ROOM_INFO) );		//사이즈가 크지 않아 그냥 보낸다. 
	_SendPacketBattlePlayer( &SendPacket );

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
void CRoom::UnreadySlotFor4vs4(void)
{
	m_csSession.Enter();

	// Ready 해제 통보 sessionIdx
	INT32 i32unreadyCount = 0;
	INT32 ai32unreadySessionIdx[SLOT_MAX_COUNT];

	// 방장을 제외한 각 팀 참여자수를 계산한다 - 4 대 4
	INT32 i32redLimitCount = 4;
	INT32 i32blueLimitCount = 4;

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
		if (SLOT_STATE_READY <= m_Info._SlotInfo[i]._State && m_Info._pSession[i] != NULL)
		{
			// 방장은 무조건 참가.
			if (i == m_Info._i32MainSlotIdx)
			{
				continue;
			}

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
				m_Info._SlotInfo[i]._State = SLOT_STATE_NORMAL;

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

BOOL CRoom::IsAllowJoinFor4vs4(CUserSession * pSession)
{
	INT32 i32playedTeamSlotCount = 0;


	m_csSession.Enter();

	// 자신의 팀 첫 슬롯계산
	INT32 startIdx = pSession->m_i32SlotIdx % 2;

	for(INT32 i = startIdx; i < SLOT_MAX_COUNT; i += 2)
	{
		// 게임에 참여중인 팀원수를 계산한다.
		if (SLOT_STATE_READY <= m_Info._SlotInfo[i]._State && m_Info._pSession[i] != NULL)
		{
			i32playedTeamSlotCount++;
		}
	}

	m_csSession.Leave();


	// 4명까지만 참여할 수 있다.
	if (i32playedTeamSlotCount < 4)
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
	INT32 aai32beingTable[TEAM_COUNT][SLOT_MAX_COUNT / 2];

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		// 유저가 있는 슬롯
		if( (SLOT_STATE_READY <= m_Info._SlotInfo[i]._State) && (m_Info._pSession[i] != NULL) )
		{
			if (0 == i % 2)
			{
				aai32beingTable[0][i32redCount] = i;
				i32redCount++;
			}
			else
			{
				aai32beingTable[1][i32blueCount] = i;
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
		INT32 i32currentTeam	= i32redCount < i32blueCount ? 1 : 0;
		INT32 i32changeTeam		= i32redCount < i32blueCount ? 0 : 1;
		INT32 i32endIdx			= MAX(i32redCount, i32blueCount) - 1;

		for(INT32 j = 0; j < i32balance; j++)
		{
			INT32 i32slotIdx = aai32beingTable[i32currentTeam][i32endIdx];

			// 이동할 팀의 비어있는 슬롯을 찾는다.
			for(INT32 i = i32changeTeam; i < SLOT_MAX_COUNT; i += 2)
			{
				if (SLOT_STATE_EMPTY == m_Info._SlotInfo[i]._State)
				{
					// SlotInfo 복사
					i3mem::Copy(&m_Info._SlotInfo[i], &m_Info._SlotInfo[i32slotIdx], sizeof(SLOT_INFO));
					m_Info._SlotInfo[i32slotIdx].Reset();
					m_Info._SlotInfo[i32slotIdx]._State = SLOT_STATE_EMPTY;

					// Session 복사
					m_Info._pSession[i] = m_Info._pSession[i32slotIdx];
					m_Info._pSession[i]->m_i32SlotIdx = i;
					
					// 방장이면 방장 idx 이동
					if (i32slotIdx == m_Info._i32MainSlotIdx)
					{
						m_Info._i32MainSlotIdx = i;
					}

					// 이동이 발생하였다.
					aChangeInfo[ui8MoveCount].ui8OldSlot		= i32slotIdx;
					aChangeInfo[ui8MoveCount].ui8NewSlot		= i;
					aChangeInfo[ui8MoveCount].ui8OldSlotState	= SLOT_STATE_EMPTY;
					aChangeInfo[ui8MoveCount].ui8NewSlotState	= m_Info._SlotInfo[i]._State;
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
		UINT8 ui8MainSlot	= m_Info._i32MainSlotIdx;

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

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		// 유저가 있는 슬롯
		if( (SLOT_STATE_READY <= m_Info._SlotInfo[i]._State) && (m_Info._pSession[i] != NULL) )
		{
			i32UserCount++;

			// 이후 킬데스 비교를 위해 비율을 미리 계산해둠
			CUserSession *pUserSession	= m_Info._pSession[i];
			if( pUserSession->m_UserInfoRecord._death == 0 && pUserSession->m_UserInfoRecord._killcount == 0)
				aui32KillDeath[i] = 0;
			else
				aui32KillDeath[i] = (pUserSession->m_UserInfoRecord._killcount*10000) / (pUserSession->m_UserInfoRecord._killcount + pUserSession->m_UserInfoRecord._death);		// 킬/데스(%)
		}

		if( SLOT_STATE_CLOSE == m_Info._SlotInfo[i]._State )
		{
			m_Info._SlotInfo[i]._State = SLOT_STATE_EMPTY;
			m_Info._InfoBasic._PersonPlayerMax++; 
			bChangeSlotState = true;
		}
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
	ROOM_SLOT_CHANGE_INFO aChangeInfo[SLOT_MAX_COUNT];

	// 유저 수 짝수일때, 홀수일때에 따라 배치 순서 설정
	if (0 < i32UserCount%2 )
	{
		pi8SlotSequence = g_ai8SlotSequenceforOdd;
	}
	else
	{
		pi8SlotSequence = g_ai8SlotSequenceforEven;
	}

	for( INT32 i = 0; i < i32UserCount; i++ )
	{
		INT32 i32OrgSlot = pi8SlotSequence[i];
		INT32 i32DestSlot = pi8SlotSequence[i];

		// 남은 유저 순위 확인
		CUserSession *pOrgUserSession	= 0;
		UINT32 ui32OrgExp				= 0xFFFFFFFF;	// 작을수록 우선배치하므로 최대값으로 초기화
		UINT32 ui32OrgKillDeath			= 0;
		for( INT32 j = i; j < SLOT_MAX_COUNT; j++ )
		{
			if( (SLOT_STATE_READY <= m_Info._SlotInfo[pi8SlotSequence[j]]._State) && (m_Info._pSession[pi8SlotSequence[j]] != NULL) )
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
			if( m_Info._SlotInfo[i32DestSlot]._State <= SLOT_STATE_CLOSE )
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
			i3mem::Copy( &TempSlotInfo,						&m_Info._SlotInfo[i32DestSlot],	sizeof(SLOT_INFO) );
			i3mem::Copy( &m_Info._SlotInfo[i32DestSlot],	&m_Info._SlotInfo[i32OrgSlot],	sizeof(SLOT_INFO) );
			i3mem::Copy( &m_Info._SlotInfo[i32OrgSlot],		&TempSlotInfo,					sizeof(SLOT_INFO) );

			// KillDeath 비율도 슬롯 따라 교환
			UINT32 ui32KillDeathTemp	= aui32KillDeath[i32DestSlot];
			aui32KillDeath[i32DestSlot]	= aui32KillDeath[i32OrgSlot];
			aui32KillDeath[i32OrgSlot]	= ui32KillDeathTemp;

			// 방장이면 방장 idx 이동
			if (i32OrgSlot == m_Info._i32MainSlotIdx)			m_Info._i32MainSlotIdx = i32DestSlot;
			else if (i32DestSlot == m_Info._i32MainSlotIdx)		m_Info._i32MainSlotIdx = i32OrgSlot;

			// 이동이 발생하였다.
			aChangeInfo[ui8MoveCount].ui8OldSlot = i32OrgSlot;
			aChangeInfo[ui8MoveCount].ui8NewSlot = i32DestSlot;
			aChangeInfo[ui8MoveCount].ui8OldSlotState = m_Info._SlotInfo[i32OrgSlot]._State;
			aChangeInfo[ui8MoveCount].ui8NewSlotState = m_Info._SlotInfo[i32DestSlot]._State;
			ui8MoveCount++;
		}
	}

	m_csSession.Leave();


	// 이동이 일어났으면 패킷전달
	if (0 < ui8MoveCount)
	{
		UINT8 ui8MoveReason = 1;
		UINT8 ui8MainSlot	= m_Info._i32MainSlotIdx;

		i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
		Packet.WriteData( &ui8MoveReason,	sizeof(UINT8) );
		Packet.WriteData( &ui8MainSlot,		sizeof(UINT8) );
		Packet.WriteData( &ui8MoveCount,	sizeof(UINT8) );
		Packet.WriteData( aChangeInfo,		(UINT16)(sizeof(ROOM_SLOT_CHANGE_INFO) * ui8MoveCount) );
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
		if( (SLOT_STATE_READY <= m_Info._SlotInfo[i]._State) && (m_Info._pSession[i] != NULL) )
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

		if (SLOT_STATE_EMPTY == m_Info._SlotInfo[i]._State)
		{
			// Session 복사
			m_Info._pSession[i]					= m_Info._pSession[i32SlotIdx];
			m_Info._pSession[i]->m_i32SlotIdx	= i;

			// SlotInfo 복사
			i3mem::Copy(&m_Info._SlotInfo[i], &m_Info._SlotInfo[i32SlotIdx], sizeof(SLOT_INFO));
			m_Info._SlotInfo[i32SlotIdx].Reset();
			m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_EMPTY;

			// 방장이면 방장 idx 이동
			if (i32SlotIdx == m_Info._i32MainSlotIdx)
			{
				m_Info._i32MainSlotIdx = i;
			}

			// 이동이 발생하였다.
			aChangeInfo[ui8MoveCount].ui8OldSlot = i32SlotIdx;
			aChangeInfo[ui8MoveCount].ui8NewSlot = i;
			aChangeInfo[ui8MoveCount].ui8OldSlotState = SLOT_STATE_EMPTY;
			aChangeInfo[ui8MoveCount].ui8NewSlotState = m_Info._SlotInfo[i]._State;
			ui8MoveCount++;
			break;
		}
	}

	m_csSession.Leave();

	// 이동이 일어났으면 패킷전달
	if (0 < ui8MoveCount)
	{
		UINT8 ui8MoveReason = 1;
		UINT8 ui8MainSlot	= m_Info._i32MainSlotIdx;

		i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
		Packet.WriteData( &ui8MoveReason,	sizeof(UINT8));
		Packet.WriteData( &ui8MainSlot,		sizeof(UINT8));
		Packet.WriteData( &ui8MoveCount,	sizeof(UINT8));
		Packet.WriteData( aChangeInfo,		(UINT16)(sizeof(ROOM_SLOT_CHANGE_INFO) * ui8MoveCount));
		_SendPacketAllPlayer( &Packet );
	}
}

void CRoom::MissionBombInstall(CUserSession * pMainSession, INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos)
{
	// 미션상태가 전투중이 아니면 폭탄 설치 불가
	if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )
	{
		// 튜토리얼은 예외처리
		if( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )
			return; 	
	}

	if( ( i32SlotIdx < 0 ) || ( SLOT_MAX_COUNT <= i32SlotIdx ) ) return;
	if( SLOT_STATE_BATTLE	 != m_Info._SlotInfo[i32SlotIdx]._State )return;

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return; //잘못된 방을 찾은겁니다. 시간차로 나올수 있습니다. 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return ;
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:
		if( pMainSession != NULL &&	// pMainSession 가 NULL 일 경우는 Dedi 가 보낸 것
			GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )	return;
		pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		break;
	default: return;	break; 
	}

	if( m_pRoomMode->MissionBombInstall( i32SlotIdx, ui8BombPosition, pr32BombVecPos, GetRoomBattleTime() ) )
	{
		SetUserMissionCard( GetSession(i32SlotIdx), ACTION_ID_C4_PLANT );
	}
}

void CRoom::MissionBombUnInstall(CUserSession * pMainSession, INT32 i32SlotIdx)
{
	if( MISSION_STATE_BATTLE	!= m_Info._i32MiStateType )
	{
		// 튜토리얼은 예외처리
		if( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )
			return; 	
	}
	if( ( i32SlotIdx < 0 ) || ( SLOT_MAX_COUNT <= i32SlotIdx ) ) return;
	if( SLOT_STATE_BATTLE		!= m_Info._SlotInfo[i32SlotIdx]._State )return; 

	switch(m_ui8BattleType)
	{
	case BATTLE_SERVER_TYPE_P2P			:
	case BATTLE_SERVER_TYPE_RELAY		: 
		if(pMainSession == NULL)return; //잘못된 방을 찾은겁니다. 시간차로 나올수 있습니다. 
		if(pMainSession->m_i32SlotIdx != m_Info._i32MainSlotIdx )return ;
		break; 
	case BATTLE_SERVER_TYPE_DEDICATION:	
		if( pMainSession != NULL &&	// pMainSession 가 NULL 일 경우는 Dedi 가 보낸 것
			GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )	return;
		pMainSession = m_Info._pSession[m_Info._i32MainSlotIdx];
		break; 
	default: return; break; 
	}

	if( m_pRoomMode->MissionBombUnInstall( i32SlotIdx ) )
	{
		SetUserMissionCard( GetSession(i32SlotIdx), ACTION_ID_C4_DEFUSE );
	}
}

void CRoom::MissionObjectDestroy(CUserSession * pMainSession, DESTRUCTION_INFO * pInfo)
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
	Struct.m_EquipmentWeapon._noprim	= pSession->m_aui32UseWeaponList[ WEAPON_USAGE_PRIMARY ];
	Struct.m_EquipmentWeapon._nosub		= pSession->m_aui32UseWeaponList[ WEAPON_USAGE_SECONDARY ];
	Struct.m_EquipmentWeapon._nomelee	= pSession->m_aui32UseWeaponList[ WEAPON_USAGE_MELEE ];
	Struct.m_EquipmentWeapon._nothrow	= pSession->m_aui32UseWeaponList[ WEAPON_USAGE_THROWING ];
	Struct.m_EquipmentWeapon._noitem	= pSession->m_aui32UseWeaponList[ WEAPON_USAGE_ITEM ];
	
	I3ASSERT( sizeof( Struct.m_EquipmentChara ) == sizeof( pSession->m_UseCharaList ) );
	I3ASSERT( sizeof( Struct.m_aui8WeaponEndurance ) == sizeof( pSession->m_aui8UseWeaponEndurance ) );
	I3ASSERT( sizeof( Struct.m_aui8EquipUserTitle ) == sizeof( pSession->m_aui8EquipUserTitle ) );

	i3mem::Copy( &Struct.m_EquipmentChara,		&pSession->m_UseCharaList,				sizeof(Struct.m_EquipmentChara) );
	i3mem::Copy( Struct.m_aui8WeaponEndurance,	pSession->m_aui8UseWeaponEndurance,		sizeof(Struct.m_aui8WeaponEndurance) );
	i3mem::Copy( Struct.m_aui8EquipUserTitle,	pSession->m_aui8EquipUserTitle,			sizeof(Struct.m_aui8EquipUserTitle) );
	Struct.m_EquipItem					= ui32EquipItem;

	i3NetworkPacket Packet( PROTOCOL_BATTLE_SLOT_EQUIPMENT_ACK  );
	Packet.WriteData( &Struct, sizeof(GC_PROTOCOL_SLOT_EQUIPMENT) );
	_SendPacketBattlePlayer( &Packet );
}

INT32 CRoom::GMPause( CUserSession* pSession )
{
	if ( m_bGMPause )										return EVENT_ERROR_GM_PAUSE_FAIL_REASON_ALREADY_PAUSE;
	if ( !pSession->IsGMUser() )							return EVENT_ERROR_GM_PAUSE_FAIL_REASON_U_R_NOT_GM;
	if ( g_pContextMain->GetServerTime() < m_ui32ATPauseBegin + NEXT_GM_PAUSE_TIME )return EVENT_ERROR_GM_PAUSE_FAIL_REASON_TIME_LIMIT;

	m_i32PauseUserIdx = pSession->m_i32SlotIdx;
	g_pModuleControl->BattleGMPause( pSession->m_WorkThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, true );

	return EVENT_ERROR_SUCCESS;
}

INT32 CRoom::GMResume( INT32 i32WorkIdx, INT32 i32SlotIdx )
{
	if ( !m_bGMPause )										return EVENT_ERROR_GM_RESUME_FAIL_REASON_NOT_PAUSE;
	if ( m_i32PauseUserIdx != i32SlotIdx )					return EVENT_ERROR_GM_RESUME_FAIL_REASON_U_R_NOT_LOCKER;

	m_bGMPause = FALSE;
	g_pModuleControl->BattleGMPause( i32WorkIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, false );

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
			SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN_GAME_MAKER);
			break;
		case 3:
			SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN_HATTRICK);
			break;
		case 2:
			SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN_ACE_ATTACKER);
			break;
		case 1:
			SetUserMissionCard( pSession, ACTION_ID_TOUCHDOWN);
			break;
		}
	}
}

void CRoom::MissionDeathBlow( CUserSession * pSession, DEATHBLOW_INFO * pInfo )
{
	SetUserMissionCard( pSession, ACTION_ID_DEATHBLOW, pInfo);
}

void CRoom::RequestTeamChange(CUserSession * pSession)
{
	if (pSession->m_i32SlotIdx != m_Info._i32MainSlotIdx)	return;	// 방장이 아님
	if (ROOM_STATE_READY != m_Info._InfoBasic._State)		return; // 대기상태 아님

	UINT8					ui8MoveCount = 0;
	ROOM_SLOT_CHANGE_INFO	aChangeInfo[ SLOT_MAX_COUNT ];

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

			swap( m_aRoomUserInfo[i], m_aRoomUserInfo[i+1] );

			if (SLOT_STATE_CLOSE < m_Info._SlotInfo[i+1]._State && m_Info._pSession[i+1] != NULL)
			{
				m_Info._pSession[i]->m_i32SlotIdx	= i;
			}

			if (SLOT_STATE_CLOSE < m_Info._SlotInfo[i]._State && m_Info._pSession[i] != NULL)
			{
				m_Info._pSession[i+1]->m_i32SlotIdx = i + 1;
			}

			// Swap slotInfo
			SLOT_INFO tempSlotInfo;
			i3mem::Copy( &tempSlotInfo,				&m_Info._SlotInfo[i],		sizeof(SLOT_INFO));
			i3mem::Copy( &m_Info._SlotInfo[i],		&m_Info._SlotInfo[i + 1],	sizeof(SLOT_INFO));
			i3mem::Copy( &m_Info._SlotInfo[i + 1],	&tempSlotInfo,				sizeof(SLOT_INFO));

			// 레디 풀기
			if (SLOT_STATE_READY == m_Info._SlotInfo[i]._State)			m_Info._SlotInfo[i]._State		= SLOT_STATE_NORMAL;
			if (SLOT_STATE_READY == m_Info._SlotInfo[i + 1]._State)		m_Info._SlotInfo[i + 1]._State	= SLOT_STATE_NORMAL;

			aChangeInfo[ ui8MoveCount ].ui8OldSlot		= i;
			aChangeInfo[ ui8MoveCount ].ui8NewSlot		= i+1;
			aChangeInfo[ ui8MoveCount ].ui8OldSlotState	= m_Info._SlotInfo[i]._State;	// 이곳에 내려왔을때는 이미 변경된 상태이므로 바뀐 슬롯의 상태를 보내야합니다.
			aChangeInfo[ ui8MoveCount ].ui8NewSlotState	= m_Info._SlotInfo[i+1]._State;
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
	switch(g_pContextMain->m_pui8ChannelType[m_Info._ui32ChannelNum])
	{
	default: 
		break;
	case CHANNEL_TYPE_PCCAFE_CLAN:
	case CHANNEL_TYPE_CLAN: 
		{
			INT32 i32Temp = m_Info._aui32BattleClanIdx[0];
			m_Info._aui32BattleClanIdx[0] = m_Info._aui32BattleClanIdx[1];
			m_Info._aui32BattleClanIdx[1] = i32Temp;
		}
		break;
	}
	// 팀 슬롯 변경

	UINT8 ui8MoveReason = 2;
	UINT8 ui8MainSlot	= m_Info._i32MainSlotIdx;

	i3NetworkPacket Packet(PROTOCOL_ROOM_TEAM_BALANCE_ACK);
	Packet.WriteData( &ui8MoveReason,	sizeof(UINT8)  );
	Packet.WriteData( &ui8MainSlot,		sizeof(UINT8)  );
	Packet.WriteData( &ui8MoveCount,	sizeof(UINT8)  );
	Packet.WriteData( aChangeInfo,		sizeof(ROOM_SLOT_CHANGE_INFO)*ui8MoveCount );
	_SendPacketAllPlayer( &Packet );
}

void CRoom::DestroyRoom(void)
{
	i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY); 
	_SendPacketAllPlayer(&SendPacket); 
}

void CRoom::SendNoticeMessage( char * pstrMessage, INT32 i32len )
{
	INT32 i32Type = NOTICE_TYPE_EMERGENCY; 
	i3NetworkPacket SendPacket( PROTOCOL_SERVER_MESSAGE_ANNOUNCE );
	SendPacket.WriteData(&i32Type,		sizeof(INT32));
	SendPacket.WriteData(&i32len,		sizeof(UINT32));
	SendPacket.WriteData(pstrMessage,	(UINT16)i32len);
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

	if( ROOM_STATE_READY != m_Info._InfoBasic._State )		return;
	if( m_aRoomUserInfo[i32SlotIdx]._bChangeMain )			return;

	INT32 i32Rv = EVENT_ERROR_FAIL;
	m_csSession.Enter();
	{
		m_aRoomUserInfo[i32SlotIdx]._bChangeMain = TRUE;

		INT32 i32Count = 0;
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			if( false == m_aRoomUserInfo[i]._bChangeMain )	continue;
			i32Count++;
		}

		if( i32Count > (m_Info._InfoBasic._PersonNow/2) )
		{
			i32Rv = _ChangeMain( SLOT_STATE_CLOSE );

			if( EV_SUCCESSED( i32Rv ) )
			{
				m_aRoomUserInfo[m_Info._i32MainSlotIdx]._bAlreadyMain = TRUE;
			}
		}
	}
	m_csSession.Leave();

	if( EV_SUCCESSED( i32Rv ) )
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			m_aRoomUserInfo[i]._bChangeMain = FALSE;
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
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)	return;			// 잘못된 패킷

	INT32 i32Rv;
	m_csSession.Enter();
	{
		i32Rv = _ChangeMain( SLOT_STATE_CLOSE );

		if( EV_SUCCESSED( i32Rv ) )
		{	// Rv는 무조건 정상값이다. 따로 확인 필요없다.
			m_aRoomUserInfo[m_Info._i32MainSlotIdx]._bAlreadyMain = TRUE;
		}
	}
	m_csSession.Leave();

	if( EV_SUCCESSED( i32Rv ) )
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			m_aRoomUserInfo[i]._bChangeMain = FALSE;
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

void CRoom::RequestMainChangeSlot(CUserSession * pSession, UINT32 i32SlotIdx)
{
	if(m_Info._InfoBasic._State != ROOM_STATE_READY)		return; 
	if(m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)	return; 
	if(i32SlotIdx >= SLOT_MAX_COUNT)						return; 

	BOOL bRv = TRUE;
	m_csSession.Enter(); 
	{
		if (SLOT_STATE_CLOSE < m_Info._SlotInfo[i32SlotIdx]._State)
		{
			if (SLOT_STATE_READY == m_Info._SlotInfo[i32SlotIdx]._State)	m_Info._SlotInfo[i32SlotIdx]._State = SLOT_STATE_NORMAL;
			m_Info._i32MainSlotIdx = i32SlotIdx;
			// 방장을 한번 했습니다.
			m_aRoomUserInfo[ m_Info._i32MainSlotIdx ]._bAlreadyMain = TRUE;

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
			m_aRoomUserInfo[i]._bChangeMain = FALSE;
		}

		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK );
		Packet.WriteData(&m_Info._i32MainSlotIdx, sizeof(INT32)); 
		_SendPacketAllPlayer(&Packet); 	
		_SendSlotInfo( NULL );
	}
	else
	{
		UINT32 ui32Temp = EVENT_ERROR_FAIL; 
		i3NetworkPacket Packet( PROTOCOL_ROOM_REQUEST_MAIN_CHANGE_WHO_ACK );
		Packet.WriteData(&ui32Temp, sizeof(INT32)); 
		m_Info._pSession[m_Info._i32MainSlotIdx]->SendPacketMessage(&Packet);
	}

	return ; 
}

INT32 CRoom::_CheckMainBest( SLOT_STATE eSlotState )
{
	bool	bRemain = false;		// 방에 인원이 남아있는지 체크
	bool	bIsRealIP;
	INT8	ai8Ping[3];
	INT8	i8State			= 0;	// 0 - Normal, 1 - RealIP, 2 - PCRoom
	INT32	i32MainSlot		= SERVER_VALUE_UNKNOWN;
	
	i3mem::Fill( ai8Ping, -1, sizeof(INT8)*3 );

	if( g_pContextMain->m_i8HackCheck & 0x2 ) // 핵 체크 기능을 켰다면 - 서동권
	{
		m_ui8Hack_Count = 0;

		UINT16	ui16KillScore	= 0;
		INT32	i32Main			= SERVER_VALUE_UNKNOWN;

		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if( m_Info._i32MainSlotIdx == i )				continue;	// 현재 방장을 제외한다.
			if( m_Info._SlotInfo[i]._State <= eSlotState )	continue;	// 슬롯이 닫혀있다.
			if( m_Info._pSession[i] == NULL )				continue;	// Session 이 NULL 이다.

			bRemain = true;
			
			if( ui16KillScore < m_Info._aM_UserScore[ i ].m_KillCount )
			{
				i32Main = i;
				ui16KillScore =  m_Info._aM_UserScore[ i ].m_KillCount;
			}
		}

		if( i32Main != SERVER_VALUE_UNKNOWN )		i32MainSlot = i32Main;
	}
	else
	{
		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			if( m_Info._i32MainSlotIdx == i )				continue;	// 현재 방장을 제외한다.
			if( m_Info._SlotInfo[i]._State <= eSlotState )	continue;	// 슬롯이 닫혀있다.
			if( m_Info._pSession[i] == NULL )				continue;	// Session 이 NULL 이다.

			bRemain = true;

			bIsRealIP = m_Info._pSession[i]->IsRealIp();

			if( PC_NOT_CAFE != m_Info._pSession[i]->m_UserInfoBasic._pcCafe )	//피씨방
			{
				if( ai8Ping[0] < m_aRoomUserInfo[i]._ui8Ping )
				{
					ai8Ping[0]	= m_aRoomUserInfo[i]._ui8Ping;	// 최고 핑 저장
					i8State		= 2;							// PCRoom 찾았음
					i32MainSlot = i;
				}
			}
			else if( (i8State < 2) && bIsRealIP )
			{
				if( ai8Ping[1] < m_aRoomUserInfo[i]._ui8Ping )
				{
					ai8Ping[1]	= m_aRoomUserInfo[i]._ui8Ping;	// 최고 핑 저장
					i8State		= 1;							// RealIP 찾았음
					i32MainSlot = i;
				}
			}
			else if( i8State < 1)
			{
				if( ai8Ping[2] < m_aRoomUserInfo[i]._ui8Ping )
				{
					ai8Ping[2]	= m_aRoomUserInfo[i]._ui8Ping;	// 최고 핑 저장
					i8State		= 0;							// 그냥 아무나
					i32MainSlot = i;
				}
			}
		}
	}

	if( false == bRemain )						return SERVER_VALUE_UNKNOWN;		// 방에 인원이 없으면 바로 실패처리
	if( SERVER_VALUE_UNKNOWN != i32MainSlot )	return i32MainSlot;

	// 방장을 잡았던 유저는 다시 방장을 잡지 않도록 해줍니다.
	// 그러나 모두가 한번씩 방장을 잡았을수도 있어서 이럴 경우 다시한번 체크합니다.
	// 밑에 코드를 윗 코드에 if 문으로 합치려고 했으나 방장을 한번도 잡지 않은 방이 그리 많지 않을것이라 생각하며 
	// 속도상 이득을 보기 위해 for 문을 한번 더 돌렸습니다.. 20100702 김현우

	i8State			= 0;	// 0 - Normal, 1 - RealIP, 2 - PCRoom
	i32MainSlot		= SERVER_VALUE_UNKNOWN;
	i3mem::Fill( ai8Ping, -1, sizeof(INT8)*3 );

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( m_Info._i32MainSlotIdx == i )						continue;	// 현재 방장을 제외한다.
		if( m_Info._SlotInfo[i]._State <= SLOT_STATE_CLOSE )	continue;	// 슬롯이 닫혀있다.
		if( m_Info._pSession[i] == NULL )						continue;	// Session 이 NULL 이다.

		bIsRealIP = m_Info._pSession[i]->IsRealIp();

		if( PC_NOT_CAFE != m_Info._pSession[i]->m_UserInfoBasic._pcCafe )	//피씨방
		{
			if( ai8Ping[0] < m_aRoomUserInfo[i]._ui8Ping )
			{
				ai8Ping[0]	= m_aRoomUserInfo[i]._ui8Ping;	// 최고 핑 저장
				i8State		= 2;							// PCRoom 찾았음
				i32MainSlot = i;
			}
		}
		else if( (i8State < 2) && bIsRealIP )
		{
			if( ai8Ping[1] < m_aRoomUserInfo[i]._ui8Ping )
			{
				ai8Ping[1]	= m_aRoomUserInfo[i]._ui8Ping;	// 최고 핑 저장
				i8State		= 1;							// RealIP 찾았음
				i32MainSlot = i;
			}
		}
		else if( i8State < 1)
		{
			if( ai8Ping[2] < m_aRoomUserInfo[i]._ui8Ping )
			{
				ai8Ping[2]	= m_aRoomUserInfo[i]._ui8Ping;	// 최고 핑 저장
				i8State		= 0;							// 그냥 아무나
				i32MainSlot = i;
			}
		}
	}

	return i32MainSlot;
}

void CRoom::SetBattleGroupInfo( INT32 i32Result, P_BATTLE_ROOM_CREATE_ACK * pAckInfo )
{	
	if( !EV_SUCCESSED( i32Result ) )ResetBattleGroupInfo(); 	

	//Set Value 
	m_bIsBattleRoomCreated		= true;
	m_ui8BattleType				= pAckInfo->_BattleType; 
	m_ui32BattleRoomIdx			= pAckInfo->_GroupIdx;
	m_ui32BattleEncryptKey		= pAckInfo->_Key;
	m_BattleRoomInfo.Copy( &pAckInfo->_RoomInfo );
	m_ui32BattleServerIP		= pAckInfo->_Ip;
	m_ui16BattleServerPort		= pAckInfo->_Port;

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
	if ( STAGE_MASK_TYPE_TRAINING != m_Info._InfoBasic._StageMask )
	{
		// 훈련전전용
		I3TRACE("[WARNING] CRoom::ChangeDifficultyLevel() - Not training mode call");
		return;
	}

	if (m_Info._i32MainSlotIdx != pSession->m_i32SlotIdx)
	{
		// 방장전용
		I3TRACE("[WARNING] CRoom::ChangeDifficultyLevel() - Not host call");
		return;
	}

	// 난이도 상승
	m_Info.m_i8currentDifficultyLevel = MIN( m_Info.m_i8currentDifficultyLevel + 1, TRAINING_DIFFICULTY_LEVEL_HIGH );

	// 전송
	i3NetworkPacket packet(PROTOCOL_BATTLE_CHANGE_DIFFICULTY_LEVEL_ACK);
	packet.WriteData(&m_Info.m_i8currentDifficultyLevel, sizeof(INT8));
	_SendPacketBattlePlayer(&packet);
}

void CRoom::InsertSlotItem( CUserSession * pSession, SLOT_INFO* pSlotInfo )
{
	UINT8 ui8Respawn			= 0;	// Respawn Item 계산을 위한 변수
	pSlotInfo->_Item			= 0;
	for( INT32 i = 0 ; i < pSession->m_i32InvItemCount ; i++ )
	{
		// 발렌타인 이벤트. Vallentine Chocolate
		if( pSession->m_aInvItem[i]._ui32ItemID == 1301101000 )
		{	// Vallentine Chocolate만 예외 처리.
			pSlotInfo->_Item	|= SLOT_ITEM_VALENTINE_CHOCOLATE;
		}

		// Ketupat 아이템. 발렌타인 초콜렛과 중복 사용할 수 없다.
		if( pSession->m_aInvItem[i]._ui32ItemID == 1301091000 )
		{
			pSlotInfo->_Item	|= SLOT_ITEM_KETUPAT;
		}
		
		if( ITEM_CLASS_MAINTENANCE != GET_ITEM_FLAG_CLASS( pSession->m_aInvItem[i]._ui32ItemID ) ) continue;

		switch( GET_ITEM_FLAG_TYPE( pSession->m_aInvItem[i]._ui32ItemID ) )
		{
		case CASHITEM_GROUP_AMMO_UP:
			pSlotInfo->_Item	|= SLOT_ITEM_AMMO_UP;
			break;
		case CASHITEM_GROUP_GET_DROPPED_WEAPON:
		case CASHITEM_GROUP_GET_DROPPED_WEAPON_EV:
			pSlotInfo->_Item	|= SLOT_ITEM_PICKUP_WEAPON;
			break;
		case CASHITEM_GROUP_QUICK_CHANGE_WEAPON:
		case CASHITEM_GROUP_QUICK_CHANGE_WEAPON_EV:
			pSlotInfo->_Item	|= SLOT_ITEM_QUICK_CHANGE_WEAPON;
			break;
		case CASHITEM_GROUP_QUICK_CHANGE_MAGAZINE:
		case CASHITEM_GROUP_QUICK_CHANGE_MAGAZINE_EV:
			pSlotInfo->_Item	|= SLOT_ITEM_QUICK_CHANGE_MAGAZINE;
			break;
		case CASHITEM_GROUP_INCREASE_INVINCIBLE_TIME:
		case CASHITEM_GROUP_INCREASE_INVINCIBLE_TIME_EV:
			pSlotInfo->_Item	|= SLOT_ITEM_INCREASE_INVINCIBLE_TIME;
			break;		
		case CASHITEM_GROUP_FULLMETALJACKETAMMO:
		case CASHITEM_GROUP_FULLMETALJACKETAMMO_EV:
			pSlotInfo->_Item	|= SLOT_ITEM_FULLMETALJACKETAMMO;
			break;
		case CASHITEM_GROUP_HOLLOW_POINT_AMMO:
		case CASHITEM_GROUP_HOLLOW_POINT_AMMO_EV:
			pSlotInfo->_Item	|= SLOT_ITEM_HOLLOW_POINT_AMMO;
			break;
		case CASHITEM_GROUP_C4_SPEED_KIT:
			pSlotInfo->_Item	|= SLOT_ITEM_C4_SPEED_KIT;
			break;
		case CASHITEM_GROUP_INCREASE_GRENADE_SLOT:
			pSlotInfo->_Item	|= SLOT_ITEM_INCREASE_GRENADE_SLOT;
			break;
		case CASHITEM_GROUP_JACKETED_HELLOW_POINT_AMMO:
			pSlotInfo->_Item	|= SLOT_ITEM_JACKETED_HELLOW_POINT_AMMO;
			break;
		case CASHITEM_GROUP_MEGA_HP5:
			pSlotInfo->_Item	|= SLOT_ITEM_MEGA_HP5;
			break;
		case CASHITEM_GROUP_MEGA_HP10:
			pSlotInfo->_Item	|= SLOT_ITEM_MEGA_HP10;
			break;
		case CASHITEM_GROUP_BULLET_PROOF_VEST:
			pSlotInfo->_Item	|= SLOT_ITEM_BULLET_PROOF_VEST;
			break;
		case CASHITEM_GROUP_BULLET_PROOF_VEST_PLUS:
			pSlotInfo->_Item	|= SLOT_ITEM_BULLET_PROOF_VEST_PLUS;
			break;
		case CASHITEM_GROUP_HOLLOW_POINT_AMMO_PLUS:
			pSlotInfo->_Item	|= SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS;
			break;
		case CASHITEM_GROUP_BULLET_PROOF_VEST_METAL:
			pSlotInfo->_Item	|= SLOT_ITEM_BULLET_PROOF_VEST_METAL;
			break;	
		case CASHITEM_GROUP_BULLET_PROOF_VEST_GM:
			pSlotInfo->_Item	|= SLOT_ITEM_BULLET_PROOF_VEST_GM;
			break;
		/*case CASHITEM_GROUP_VALENTINE_CHOCOLATE:
			pSlotInfo->_Item	|= SLOT_ITEM_VALENTINE_CHOCOLATE;
			break;*/
		case CASHITEM_GROUP_SHORT_RESPAWN_20:
		case CASHITEM_GROUP_SHORT_RESPAWN_20_EV:
			pSlotInfo->_Item	|= SLOT_ITEM_SHORT_RESPAWN_20;
			ui8Respawn			|= USE_SHORT_RESPAWN_20;
			break;
		case CASHITEM_GROUP_SHORT_RESPAWN	:
		case CASHITEM_GROUP_SHORT_RESPAWN_30_EV	:
			pSlotInfo->_Item	|= SLOT_ITEM_SHORT_RESPAWN_30;
			ui8Respawn			|= USE_SHORT_RESPAWN_30;
			break;
		case CASHITEM_GROUP_SHORT_RESPAWN_50	:
			pSlotInfo->_Item	|= SLOT_ITEM_SHORT_RESPAWN_50;
			ui8Respawn			|= USE_SHORT_RESPAWN_50;
			break;
		case CASHITEM_GROUP_SHORT_RESPAWN_100	:
			pSlotInfo->_Item	|= SLOT_ITEM_SHORT_RESPAWN_100;
			ui8Respawn			|= USE_SHORT_RESPAWN_100;
			break;
		} 
	}

	if( SLOT_ITEM_MEGA_HP10 & pSlotInfo->_Item )				pSlotInfo->_Item &= ~SLOT_ITEM_MEGA_HP5;
	if( SLOT_ITEM_BULLET_PROOF_VEST_PLUS & pSlotInfo->_Item )	pSlotInfo->_Item &= ~SLOT_ITEM_BULLET_PROOF_VEST;
	if( SLOT_ITEM_HOLLOW_POINT_AMMO_PLUS & pSlotInfo->_Item )	pSlotInfo->_Item &= ~SLOT_ITEM_HOLLOW_POINT_AMMO;
	if( SLOT_ITEM_BULLET_PROOF_VEST_GM	 & pSlotInfo->_Item )	pSlotInfo->_Item &= ~(SLOT_ITEM_BULLET_PROOF_VEST_PLUS|SLOT_ITEM_BULLET_PROOF_VEST);
	if( SLOT_ITEM_VALENTINE_CHOCOLATE & pSlotInfo->_Item )		pSlotInfo->_Item &= ~SLOT_ITEM_KETUPAT;

	// Respawn 계산 처리
	if( ui8Respawn & USE_SHORT_RESPAWN_100 )				pSession->m_ui32RespawnDelay = RESPAWN_TIME_SHORT_100;
	else if( ui8Respawn & USE_SHORT_RESPAWN_50 )			pSession->m_ui32RespawnDelay = RESPAWN_TIME_SHORT_50;
	else if( ui8Respawn & USE_SHORT_RESPAWN_30 )			pSession->m_ui32RespawnDelay = RESPAWN_TIME_SHORT_30;
	else if( ui8Respawn & USE_SHORT_RESPAWN_20 )			pSession->m_ui32RespawnDelay = RESPAWN_TIME_SHORT_20;
	else													pSession->m_ui32RespawnDelay = RESPAWN_TIME_NO;
}

// LogD 용 Get 함수입니다.
void CRoom::GetRoomInfoLogD( INT32* pi32RedCount, INT32* pi32BlueCount, char * pstrRedNick, char * pstrBlueNick )
{
	*pi32RedCount	= 0;
	*pi32BlueCount	= 0;
	pstrRedNick[0]		= '\0'; 
	pstrBlueNick[0]	= '\0'; 	

	char	strLogDNick[NET_NICK_NAME_SIZE];
	char	strTempNick[NET_NICK_NAME_SIZE + 1];

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[i]._State ) continue;

		g_pModuleContextNC->LogChangeString(m_Info._pSession[i]->m_UserInfoBasic._nick, strLogDNick, NET_NICK_NAME_SIZE); 
		i3String::Format( strTempNick, NET_NICK_NAME_SIZE + 1, "%s", strLogDNick); 
		strTempNick[NET_NICK_NAME_SIZE] = 0; //방어 코드 

		if(i % 2)
		{	//블루 팀
			*pi32BlueCount = *pi32BlueCount + 1;
			i3String::Concat(pstrBlueNick, "[");
			i3String::Concat(pstrBlueNick, strTempNick);
			i3String::Concat(pstrBlueNick, "]");
		}
		else 
		{	//레드 팀
			*pi32RedCount = *pi32RedCount + 1;
			i3String::Concat(pstrRedNick, "[");
			i3String::Concat(pstrRedNick, strTempNick);
			i3String::Concat(pstrRedNick, "]");
		}
	}

	return; 
}

#ifdef DEF_OBSERVER_MODE
void CRoom::GetObsRoomInfoLogD( INT32* pi32ObsCount, INT32* pi32HiddenCount, char * pObsNick, char * pHiddenNick )
{
	*pi32ObsCount		= 0;
	*pi32HiddenCount	= 0;
	pObsNick[0]			= '\0'; 
	pHiddenNick[0]		= '\0'; 	

	char	strLogDNick[NET_NICK_NAME_SIZE];
	char	TempNick[NET_NICK_NAME_SIZE + 1];

	// 옵저버,히든 유저
	for(INT32 Loop = SLOT_MAX_COUNT; Loop < SLOT_ALL_COUNT; Loop++)
	{
		if( SLOT_STATE_BATTLE != m_Info._SlotInfo[Loop]._State ) continue;

		g_pModuleContextNC->LogChangeString(m_Info._pSession[Loop]->m_UserInfoBasic._nick, strLogDNick, NET_NICK_NAME_SIZE); 
		i3String::Format( TempNick, "%s", strLogDNick); 
		TempNick[NET_NICK_NAME_SIZE] = 0; //방어 코드 

		if(SLOT_ROOM_VIEW_COUNT > Loop)
		{//옵저버
			*pi32ObsCount = *pi32ObsCount + 1;
			i3String::Concat(pObsNick, "[");
			i3String::Concat(pObsNick, TempNick);
			i3String::Concat(pObsNick, "]");
		}
		else 
		{//히든
			*pi32HiddenCount = *pi32HiddenCount + 1;
			i3String::Concat(pHiddenNick, "[");
			i3String::Concat(pHiddenNick, TempNick);
			i3String::Concat(pHiddenNick, "]");
		}
	}

	return; 
}
#endif
void CRoom::GetRoomMissionLogD( INT32* pi32RedCount, INT32* pi32BlueCount )
{
	*pi32RedCount	= 0; 
	*pi32BlueCount	= 0;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( m_Info._SlotInfo[i]._State != SLOT_STATE_BATTLE ) continue;

		if(i % 2)
		{	//블루 팀
			*pi32BlueCount = *pi32BlueCount + m_Info._aui8AccMissionCount[i];
		}
		else 
		{	//레드 팀
			*pi32RedCount = *pi32RedCount + m_Info._aui8AccMissionCount[i];
		}
	}
}

void CRoom::SetUserMissionCard( CUserSession* pSession, ACTION_ID eAttionID, void* pPointer )
{	
	// AI전일 경우 임무 처리 안함
	if( STAGE_MASK_TYPE_TRAINING == m_Info._InfoBasic._StageMask )
		return;

	// 튜토리얼일 경우 임무 처리 안함
	if ( (GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL)	||
		 (GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_DOMINATION)	)
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
		pQuest		= &g_pContextMain->m_aaQuestEvent[ pSession->m_ui8LocaleCode ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
		break;
	default:
		pQuest		= &g_pContextMain->m_aaQuest[ ui8CardType ][ ui8CardSet*MAX_QUEST_PER_CARD_NEW ];
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
			ui8QuestCtxActiveIdx = (i | 0xF0);
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
	STAGE_ORDINAL_ID ui32StageID = g_GetStageID_ForMission( (STAGE_ORDINAL_ID)pQuest->m_Arg[ 1 ] );

	// Stage 체크 - Stage 가 맞지 않으면 실패처리
	if( (STAGE_ORD_NA != ui32StageID) && ( ui32StageID != g_GetStageID_ForMission( GET_STAGEORD( m_Info._InfoBasic._StageID ) ) ) )	return FALSE;

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
			UINT32 ui32ItemType		= GET_ITEM_FLAG_CLASS( ui32WeaponInfo );
			UINT32 ui32ItemClass	= g_GetWeaponClass_ForMission( GET_ITEM_FLAG_TYPE( ui32WeaponInfo ) );
			UINT32 ui32ItemNum		= GET_ITEM_FLAG_NUMBER( ui32WeaponInfo );

			if( pQuest->m_Arg[3] > 0 && pQuest-> m_Arg[3] != ui32ItemClass )  return FALSE;
							
			if( pQuest-> m_Arg[3] == WEAPON_CLASS_THROWING ) 
			{	// 투척무기 예외처리 
				if( ui32ItemType == ITEM_CLASS_GRENADE )	return TRUE;   
			}

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
			UINT32 ui32ItemType		= GET_ITEM_FLAG_CLASS( ui32WeaponInfo );
			UINT32 ui32ItemClass	= g_GetWeaponClass_ForMission( GET_ITEM_FLAG_TYPE( ui32WeaponInfo ) );
			UINT32 ui32ItemNum		= GET_ITEM_FLAG_NUMBER( ui32WeaponInfo );

			if( pQuest->m_Arg[3] > 0 && pQuest-> m_Arg[3] != ui32ItemClass )  return FALSE;
							
			if( pQuest-> m_Arg[3] == WEAPON_CLASS_THROWING ) 
			{	// 투척무기 예외처리 
				if( ui32ItemType == ITEM_CLASS_GRENADE )	return TRUE;   
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
			
			UINT8 ui8LoopCount = eAttionID - ACTION_ID_KILL_WEAPONCLASS + 1;
			
			for( UINT8 i = 0; i < ui8LoopCount ; i++ )
			{
				i8WeaponInfoIdx -= 1;

				if(i8WeaponInfoIdx < 0) i8WeaponInfoIdx = MAX_USED_WEAPON_LIST-1;

				INT8 i8WeaponClass		= g_GetWeaponClass_ForMission( stKilledUserWeapon->_ai8WeaponClass[i8WeaponInfoIdx] );

				if(pQuest->m_Arg[3] > 0 && pQuest->m_Arg[3] != i8WeaponClass  )	return FALSE;	// 이쪽으로 들어오면 안된다 (특정무기 사용자를 죽여야 하는데... 어느 무기인지 안정해져있으면 안된다...) 
			}
		}
		break;
	}

	return TRUE;
}



static int compare( UINT32* arg1,  UINT32* arg2 )
{
	return *arg1 - *arg2;
}

void CRoom::_PresentSupplyBox_M()
{
	USER_RANK_INFO aUserRankInfo[SLOT_MAX_COUNT];
	i3mem::FillZero( aUserRankInfo, sizeof(aUserRankInfo) );

	INT8	i8Ranking[SLOT_MAX_COUNT];
	i3mem::FillZero( i8Ranking, sizeof(i8Ranking) );
	INT8	i8BattleUserCount = 0;

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode ) continue;

		if( m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE )
		{
			i8BattleUserCount++;
		}
	}

	if( i8BattleUserCount < 2 )
		return;

	// 순위 정렬
	m_pRoomMode->GetRanking( i8Ranking, i8BattleUserCount );

	INT8	i8Count = 0;
	for(INT i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( TRUE == m_Info._SlotInfo[i]._bIsGmObserverMode ) continue;

		if( m_Info._SlotInfo[i]._State == SLOT_STATE_BATTLE )
		{
			aUserRankInfo[i8Count].i8SlotIdx	= i;
			aUserRankInfo[i8Count].i64UID		= m_Info._pSession[i]->m_i64UID;
			aUserRankInfo[i8Count].i8Ranking	= i8Ranking[i];
			i8Count++;
		}
	}

	m_ui32SupplyBoxResultReq = 
#ifdef USE_GM_PAUSE
								GetRoomBattleTime();
#else
								g_pContextMain->GetServerTime();
#endif
	g_pModuleTrans->BattleSendSupplyBoxResultReq( MAIN_THREAD_IDX, m_Info._ui32ChannelNum, m_Info._InfoBasic._Idx, m_ui32SupplyBoxResultReq, i8BattleUserCount, aUserRankInfo );
}

void CRoom::SetPresentSupplyBox( UINT32 ui32RoomStateCheckTime, INT8 i8BattleMemberCount, USER_PRESENTED_INFO *pUserPresentInfo )
{
	if( i8BattleMemberCount > SLOT_MAX_COUNT ) return;

	// 결과 요청 시간을 비교해서 결과 대기 상태인지 확인
	if( ui32RoomStateCheckTime == m_ui32SupplyBoxResultReq )
	{
		for( INT8 i = 0; i < i8BattleMemberCount; i++ )
		{
			if( pUserPresentInfo[i].i8SlotIdx <= SERVER_VALUE_UNKNOWN )							continue;
			if( pUserPresentInfo[i].i8SlotIdx >= SLOT_MAX_COUNT )								continue;
			if( m_Info._SlotInfo[pUserPresentInfo[i].i8SlotIdx]._State != SLOT_STATE_BATTLE )	continue;

			m_ai8PresentSupplyBox[pUserPresentInfo[i].i8SlotIdx] = pUserPresentInfo[i].i8Presented;

			i3NetworkPacket	SendPacket( PROTOCOL_BASE_SUPPLY_BOX_PRESENT_ACK );
			SendPacket.WriteData( &pUserPresentInfo[i].i8Presented,			sizeof(INT8) );
			SendPacket.WriteData( &pUserPresentInfo[i].ui32ItemID,			sizeof(UINT32) );
			m_Info._pSession[pUserPresentInfo[i].i8SlotIdx]->SendPacketMessage( &SendPacket );
		}

		m_ui32SupplyBoxResultReq = 0;
	}
}

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
	if ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_TUTORIAL	||
		 GET_STAGETYPE( m_Info._InfoBasic._StageID ) == STAGE_TYPE_DOMINATION )
		 return TRUE;

	// 리스폰 가능한 시간 = 죽은 시간 + 리스폰 시간 
	UINT32 ui32PossibleRespawnTime = pSession->m_ui32BattleDeathTime + pSession->m_ui32RespawnDelay ;

	// 현재 시간( 리스폰 요청 패킷 보낸시간) 이 리스폰 가능 시간 보다 크다면 정상.
	if( g_pContextMain->GetServerTime() >= ui32PossibleRespawnTime )				return TRUE;

	char strTemp[MAX_PATH];
	i3String::Format( strTemp, MAX_PATH, "[CheckRespawnHacking()] %I64d, DeathTime : %d / RespawnTime : %d / NowTime : %d / nick : %s", pSession->m_i64UID, pSession->m_ui32BattleDeathTime,  pSession->m_ui32RespawnDelay, g_pContextMain->GetServerTime() ,  pSession->m_UserInfoBasic._nick );
	g_pModuleLogFile->Write_M_Log(pSession->m_WorkThreadIdx, pSession, strTemp);

	// 현재 시간( 리스폰 요청 패킷 보낸시간) 이 리스폰 가능 시간 보다 작다면 리스폰 해킹
	return TRUE;
}

void CRoom::_CheckAbusing( INT32 i32SlotIdx, UINT32 ui32PlayTime )
{
	if( 1 > ui32PlayTime )	return; // 총 게임시간이 0이하면 어뷰징 체크를 하지 않습니다.

	// 어뷰징 체크 및 에뷰징 레벨 설정
	UINT32	ui32PopupTime		= 0;
	UINT8	ui8AbusingLevel		= ABUSING_LEVEL_NONE; 
	
	UINT32	ui32ExpPerSec = m_Info._aui16AccExp[i32SlotIdx] * 100 / ui32PlayTime ;	// 초당 획득 EXP ( REAL32 로 계산하지 않기위해 100을 곱합니다. )

	if( ui32ExpPerSec >= CHECK_NORMAL_ABUSING_EXP )
	{
		m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._ui32AbusingCount++; // 어뷰징 카운트 증가
		m_Info._pSession[i32SlotIdx]->ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO );

		ui8AbusingLevel = ABUSING_LEVEL_NORMAL; 	

		UINT32	ui32PopupTimeBase	= ( ui32ExpPerSec - CHECK_NORMAL_ABUSING_EXP ) * ui32PlayTime ;

		UINT32	ui32AbusingCalValue = 0;
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
			m_Info._pSession[i32SlotIdx]->m_UserInfoBasic._ui32AbusingCount > 1 ) 
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

void CRoom::_AbusingPenalty( INT32 i32SlotIdx, UINT32 ui32PlayTime )
{
	if( 1 > ui32PlayTime )	return; // 총 게임시간이 0이하면 어뷰징 체크를 하지 않습니다.

	// 어뷰징 레벨별 페널티 적용
	switch( m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel )
	{
	case ABUSING_LEVEL_NONE :	// 페널티 X
		break;
	case ABUSING_LEVEL_NORMAL :	 
	case ABUSING_LEVEL_STRONG :	 
		{
			// 일반 제재 유저는 팝업 시간이 최소 팝업 시간 미만이면 최소 팝업 시간으로 고정
			if ( ABUSING_LEVEL_NORMAL == m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel && m_Info._pSession[i32SlotIdx]->m_ui32AbusingPopupTime < NORMAL_ABUSING_MIN_POPUP_TIME ) 
			{
				m_Info._pSession[i32SlotIdx]->m_ui32AbusingPopupTime = NORMAL_ABUSING_MIN_POPUP_TIME;
			}

			// 획득한 EXP, POINT를 보류한다.
			m_Info._pSession[i32SlotIdx]->m_ui32HoldAccExp = m_Info._aui16AccExp[i32SlotIdx] + m_Info._aui16EventExp[i32SlotIdx];
			m_Info._pSession[i32SlotIdx]->m_ui32HoldAccPoint = m_Info._aui16AccPoint[i32SlotIdx] + m_Info._aui16EventPoint[i32SlotIdx];
			m_Info._aui16AccExp[i32SlotIdx]	= 0;	
			m_Info._aui16AccPoint[i32SlotIdx]	= 0;
			m_Info._aui16EventExp[i32SlotIdx] = 0;
			m_Info._aui16EventPoint[i32SlotIdx] = 0;
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

	// 어뷰징 체크 로그저장 - 어뷰징 체크된 유저에 한해서.  
	if( ABUSING_LEVEL_NONE < m_Info._pSession[i32SlotIdx]->m_ui8AbusingLevel )
	{ 
		g_pModuleContextNC->LogSendMessage( MAIN_THREAD_IDX, LOG_MESSAGE_R_ABUSER_CHECK, m_Info._pSession[i32SlotIdx], this, &i32SlotIdx );
	}

}

void CRoom::MissionTutorialRoundEnd( CUserSession* pSession )
{
	if ( GET_STAGETYPE( m_Info._InfoBasic._StageID ) != STAGE_TYPE_TUTORIAL )		return;
	if( MISSION_STATE_BATTLE != m_Info._i32MiStateType )							return;
	if( SLOT_STATE_BATTLE	 != m_Info._SlotInfo[pSession->m_i32SlotIdx]._State )	return;

	if( ! m_pRoomMode->MissionTutorialRoundEnd( pSession ) )
	{
	}
}

BOOL CRoom::CheckPlayableStage(UINT32 ui32StageID, UINT8 ui8WeaponFlag, UINT8 ui8StageMask)
{
	// 유효한 모드인지 검사
	if( g_pContextMain->m_aStageDefaultActive[ GET_STAGETYPE( ui32StageID ) ] == STAGE_ORD_NA )
		return FALSE;

	// 열려있는 스테이지인지 검사
	UINT32 ui32TempStageID	= ui32StageID;
	// deathmatch 와 맵을 공유하는 모드는 데스매치 맵으로 검사합니다.
	switch(GET_STAGETYPE( ui32TempStageID ))
	{
	case STAGE_TYPE_ANNIHILATION	:			// 섬멸전맵은 DeathMatch 와 동일한 맵을 열기 때문에 맵 검사시 DeathMatch 로 변경하여 작업합니다.
		ui32TempStageID = SET_STAGETYPE( ui32TempStageID, STAGE_TYPE_DEATHMATCH );
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_ANNIHILATION ) )	return FALSE;
		break;
	case STAGE_TYPE_SUDDEN_DEATH	:			// SuddenDeath맵은 DeathMatch 와 동일한 맵을 열기 때문에 맵 검사시 DeathMatch 로 변경하여 작업합니다.
		ui32TempStageID = SET_STAGETYPE( ui32TempStageID, STAGE_TYPE_DEATHMATCH );
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_SUDDENDEATH ) )	return FALSE;
		break;
	case STAGE_TYPE_HEAD_HUNTER		:
		ui32TempStageID = SET_STAGETYPE( ui32TempStageID, STAGE_TYPE_DEATHMATCH );
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_HEADHUNTER ) )	return FALSE;
		break;
	case STAGE_TYPE_HEAD_KILLER		:
		ui32TempStageID = SET_STAGETYPE( ui32TempStageID, STAGE_TYPE_DEATHMATCH );
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_HEADKILLER ) )	return FALSE;
		break;
	}

	// 유효한 스테이지인지 검사
	if( FALSE == g_pContextMain->IsActiveStage( (STAGE_ID)ui32TempStageID ) )								return FALSE;

	// Sniper 가능한 맵인지
	if( GET_ROOM_INFO_WEAPON_SNIPER( ui8WeaponFlag ) )
	{
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_SNIPER ) )		return FALSE;
	}
	// Shotgun 가능한 맵인지
	if( GET_ROOM_INFO_WEAPON_SHOTGUN( ui8WeaponFlag ) )
	{
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_SHOTGUN ) )		return FALSE;
	}
	// RPG 가능한 맵인지
	if( GET_ROOM_INFO_WEAPON_RPG7( ui8WeaponFlag ) )
	{
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_RPG ) )			return FALSE;
	}
	// Knuckle 가능한 맵인지
	if( GET_ROOM_INFO_WEAPON_KNUCKLE( ui8WeaponFlag ) )
	{
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_KNUCKLE ) )		return FALSE;
	}
	// 훈련전 가능한 맵인지
	if( STAGE_MASK_TYPE_TRAINING == ui8StageMask )
	{
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_AI ) )			return FALSE;
	}
	// Head Hunter 가능한 맵인지
	if( STAGE_MASK_TYPE_HEADHUNTER == ui8StageMask )
	{
		if( FALSE == g_pContextMain->IsPlayStage( (STAGE_ID)ui32TempStageID, STAGE_OPTION_HEADHUNTER ) )	return FALSE;
	}

	return TRUE;
}

INT32 CRoom::SetRoomMode(UINT32 ui32StageID, UINT8 ui8StageMask)
{
	switch(GET_STAGETYPE( ui32StageID ))
	{
	case STAGE_TYPE_DEATHMATCH		:
		m_pRoomMode = m_pDeathmatch;	
		break; 
	case STAGE_TYPE_ANNIHILATION	: 
		m_pRoomMode = m_pAnnihilation;
		break; 
	case STAGE_TYPE_BOMB			: 	
		m_pRoomMode = m_pBomb;
		break; 
	case STAGE_TYPE_DESTROY			:
		m_pRoomMode = m_pDestroy;
		break; 
	case STAGE_TYPE_DEFENCE			:
		m_pRoomMode = m_pDefence;
		break;
	case STAGE_TYPE_ESCAPE			: 
		m_pRoomMode = m_pEscape;
		break; 
	case STAGE_TYPE_SUDDEN_DEATH	:
		m_pRoomMode = m_pSuddenDeath;
		break; 
	case STAGE_TYPE_HEAD_HUNTER		:
		m_pRoomMode = m_pHeadHunter;
		break;
	case STAGE_TYPE_HEAD_KILLER		:
		m_pRoomMode = m_pHeadKiller;
		break;
	case STAGE_TYPE_TUTORIAL		:
		m_pRoomMode = m_pTutorial;
		break;
#ifdef USE_CC_MODE
	case STAGE_TYPE_CROSSCOUNT		:
		m_pRoomMode = m_pCrossCount;
		break;
#endif
	default							:// 잘못된 정보
		return EVENT_ERROR_ROOM_NO_AUTHORITY;
	}

	if( STAGE_MASK_TYPE_TRAINING == ui8StageMask )
	{
		m_pRoomMode = m_pTraining;
	}

	return EVENT_ERROR_SUCCESS;
}

void	CRoom::SendNotifyHackUser( INT32 i32SlotIdx, UINT8 ui8Type, INT32 i32ThreadIdx, UINT32 ui32HackType, UINT8 ui8LocaleCode )
{
	if( 0 > i32SlotIdx || i32SlotIdx >= SLOT_MAX_COUNT )		return;		//슬롯 인덱스가 정확한지.

	UINT8 i8HackType; 
	if( LOCALE_CODE_INDONESIA == ui8LocaleCode )
	{
		switch( ui8Type )
		{
		case	HACK_CHECK_TYPE_CLIENT		: 
			{
				if( m_Info._SlotInfo[i32SlotIdx]._State > SLOT_STATE_READY )
				{
					Chatting_U( i32ThreadIdx, m_Info._pSession[i32SlotIdx], "User tersebut diusir dari game karena menggunakan Program illegal.", CHATTING_TYPE_ALL );

					m_Info._pSession[i32SlotIdx]->_SendErrorMessage( i32ThreadIdx, EVENT_ERROR_HACKING_EXIT_USER, 0, KILL_REASON_HACK_PACKET );
				}
			}
			break; 
		case	HACK_CHECK_TYPE_SERVER_G	: 
		case	HACK_CHECK_TYPE_SERVER_D	: 
		default								: 
			{
				i3NetworkPacket	SendPacket( PROTOCOL_BATTLE_NOTIFY_HACK_USER_ACK );
				SendPacket.WriteData( &i32SlotIdx,		sizeof(INT32) );	// 해킹 유저의 슬롯 인덱스
				SendPacket.WriteData( &ui8Type,			sizeof(UINT8)); 
				SendPacket.WriteData( &ui32HackType,	sizeof(UINT32)); 

				INT32 i; 
#ifdef DEF_OBSERVER_MODE
				for(i = 0; i < SLOT_ALL_COUNT; i ++)
#else
				for(i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
				{
					if( m_Info._SlotInfo[i]._State > SLOT_STATE_READY )
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
		SendPacket.WriteData( &i32SlotIdx,		sizeof(INT32) );	// 해킹 유저의 슬롯 인덱스
		SendPacket.WriteData( &ui8Type,			sizeof(UINT8)); 
		SendPacket.WriteData( &ui32HackType,	sizeof(UINT32)); 

		INT32 i; 
#ifdef DEF_OBSERVER_MODE
		for(i = 0; i < SLOT_ALL_COUNT; i ++)
#else
		for(i = 0; i < SLOT_MAX_COUNT; i ++)
#endif
		{
			if( m_Info._SlotInfo[i]._State > SLOT_STATE_READY )
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

	g_pModuleControl->BattleRespawn( i32ThreadIdx, &m_BattleRoomInfo, m_ui32BattleRoomIdx, i32SlotIdx, pui32WeaponList, i32RespawnIdx, i32RespawnCount );
}
