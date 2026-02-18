#include "pch.h"
#include "RoomInCrossCount.h"
#include "ModuleContextNC.h"

I3_CLASS_INSTANCE( CRoomInCrossCount, CRoomBase );

CRoomInCrossCount::CRoomInCrossCount()
{
}

CRoomInCrossCount::~CRoomInCrossCount()
{
	OnDestroy();
}

BOOL CRoomInCrossCount::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{

	// 보너스 경험치/포인트 (공통 설정사항)
	m_stGetExpPointV1._i16WinExp		= 120;
	m_stGetExpPointV1._i16LoseExp		= 80;
	m_stGetExpPointV1._i16DrawExp		= 100;
	m_stGetExpPointV1._i16WinPoint		= 100;
	m_stGetExpPointV1._i16LosePoint		= 60;
	m_stGetExpPointV1._i16DrawPoint		= 80;

	m_stKillExpPoint._i16KillExp		= 8;
	m_stKillExpPoint._i16KillPoint		= 4;
	m_stKillExpPoint._i16AssistExp		= 2;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 250;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 80;
	m_stExpPointValues._stRoundExp._i16AssistPerRound1Round			= 36;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 2	* 10 );
	m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 105	* 10 );
	m_stExpPointValues._stRoundExp._i16rHighKillFactor				= (INT16)( 1	* 10 );

	m_stExpPointValues._stRoundExp._i16HeadshotScore				= 2;
	m_stExpPointValues._stRoundExp._i16ChainKillScore				= 2;
	m_stExpPointValues._stRoundExp._i16ChainMeleeScore				= 2;
	m_stExpPointValues._stRoundExp._i16HeadshotMax					= 10;
	m_stExpPointValues._stRoundExp._i16ChainKillMax					= 20;
	m_stExpPointValues._stRoundExp._i16ChainMeleeMax				= 10;

	INT16 i16UserCountValueExp[SLOT_MAX_COUNT]							= { 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 2, 2, 1, 1 };
	i3mem::Copy( m_stExpPointValues._stRoundExp._i16rUserCountValue, i16UserCountValueExp, sizeof(INT16)*SLOT_MAX_COUNT );

	m_stExpPointValues._stRoundExpPointRate._i16rWinExp				= (INT16)( 1	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rDrawExp			= (INT16)( 0.8	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rLoseExp			= (INT16)( 0.7	* 10 );

	m_stExpPointValues._stExpPoint._i16rExpPerRound					= (INT16)( 1	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerKill					= (INT16)( 2.5	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerMission				= (INT16)( 1	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerAssist				= (INT16)( 3	* 10 );

	m_stExpPointValues._stMemberRate._bUseMemberRate				= TRUE;
	m_stExpPointValues._stMemberRate._i16MemberRangeEnd				= 6;
	m_stExpPointValues._stMemberRate._i16rMemberRangeRate			= 9;
	m_stExpPointValues._stMemberRate._i16rmemberMinRate				= 3;
	// 랭크 점수는 *100 해서 적으세요.
	INT16 i16RankPoint[SLOT_MAX_COUNT]								= { 8300, 7670, 7040, 6410, 5780, 5150, 4520, 3890, 3260, 2636, 2000, 1430, 950, 560, 260, 050 };
	i3mem::Copy( m_stExpPointValues._stRoundPoint._i16r2RankPoint, i16RankPoint, sizeof(INT16)*SLOT_MAX_COUNT );
	// 인원 점수는 * 10 해서 적으세요.
	INT16 i16UserCountValuePoint[SLOT_MAX_COUNT]					= { 128, 128, 112, 112, 96, 96, 80, 80, 64, 64, 48, 48, 32, 32, 16, 16 };
	i3mem::Copy( m_stExpPointValues._stRoundPoint._i16rUserCountValue, i16UserCountValuePoint, sizeof(INT16)*SLOT_MAX_COUNT );

	m_stExpPointValues._stRoundPoint._i16HeadshotScore				= 5;
	m_stExpPointValues._stRoundPoint._i16ChainKillScore				= 4;
	m_stExpPointValues._stRoundPoint._i16ChainMeleeScore			= 2;
	m_stExpPointValues._stRoundPoint._i16HeadshotMax				= 20;
	m_stExpPointValues._stRoundPoint._i16ChainKillMax				= 20;
	m_stExpPointValues._stRoundPoint._i16ChainMeleeMax				= 20;

	m_stExpPointValues._stRoundPoint._i16BonusScore					= 1;
	INT16 i16BonusMaxCount[MISSION_POINT_MAX_COUNT]					= {20, 40, 60, 80, 100, 120};
	i3mem::Copy( m_stExpPointValues._stRoundPoint._i16BonusMaxCount, i16BonusMaxCount, sizeof(INT16)*MISSION_POINT_MAX_COUNT );

	m_stExpPointValues._stRoundExpPointRate._i16r2WinPoint			= (INT16)( 0.8	* 100 );
	m_stExpPointValues._stRoundExpPointRate._i16r2DrawPoint			= (INT16)( 0.75	* 100 );
	m_stExpPointValues._stRoundExpPointRate._i16r2LosePoint			= (INT16)( 0.7	* 100 );

	m_ui8ScoreKill						= 1;
	m_ui8DinoTeam						= 0;

	m_eStartOption						= ROOM_START_COUNTDOWN;
	m_eRespawnOption					= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption					= ROOM_CHATTING_ALL;
	m_eEndOption						= ROOM_END_MOD;


	m_pr32SubUserCount					= new REAL32*[ SLOT_MAX_COUNT + 1];
	for( INT32 i = 1 ; i < SLOT_MAX_COUNT + 1; i++ )
	{
		m_pr32SubUserCount[i]			= new REAL32[ SLOT_MAX_COUNT ];
		i3mem::FillZero( m_pr32SubUserCount[i], sizeof(REAL32)*SLOT_MAX_COUNT );
	}

	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.101f, 0.301f, 0.301f, 0.501f, 0.501f, 0.701f, 0.701f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f  };
	REAL32 pr32SubUserCountVS4[SLOT_MAX_COUNT]		= { 0.101f, 0.301f, 0.701f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f, };

	i3mem::Copy( m_pr32SubUserCount[ 16 ],		pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCount[ 8 ],		pr32SubUserCountVS4,	sizeof(REAL32)*SLOT_MAX_COUNT );


	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInCrossCount::OnDestroy()
{
}

BOOL CRoomInCrossCount::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_MODE_CROSSCOUNT;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= 2;

	// Dino Info
	m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
	i3mem::Fill( m_pInfo->_MiDinoInfo._uiRaptorSlotIdx, 255, sizeof(UINT8)*RAPTOR_TOTAL_CNT );
	i3mem::FillZero( m_pInfo->_abDinoStateUpdate,	sizeof(bool)*SLOT_MAX_COUNT );
	i3mem::FillZero( m_pInfo->_aui32DInoCharInfo,	sizeof(UINT32)*SLOT_MAX_COUNT );

	m_pInfo->m_eReadyOption			= ROOM_READY_BOTH_TEAM;
	m_pInfo->m_i8PersonAtLeast		= 1;
	m_pInfo->m_eJoinOption			= ROOM_JOIN_SEQUENCE;
	m_pInfo->m_ePreStartOption		= ROOM_PRESTART_ALL_WAIT;

	return TRUE;
}

void CRoomInCrossCount::UserKillBounsV1( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )
{
	// Assist를 했는지
	if( CHARA_DEATH_MY_STATE_ASSIST == ui32DeathMyState )
	{
		if( SLOT_MAX_COUNT > m_pInfo->_MiDinoInfo._uiTRexSlotIdx )
		{
			m_pi16MatAssist[ m_pInfo->_MiDinoInfo._uiTRexSlotIdx ]++;		// 티라노 기여도
		}
		m_pi16MatAssist[ i32SlotIdx ]++;									// 어시스트 킬
	}

	m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreKill;
}

UINT16 CRoomInCrossCount::UserKillBounsV2( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle ) 
{
	UINT16 ui16GainPoint = CRoomBase::UserKillBounsV2(i32SlotIdx, ui32DeathType, ui32DeathMyState, ui32DeathEnemyState, ui32RTBattle);
	
	// Assist를 했는지
	if( CHARA_DEATH_MY_STATE_ASSIST == ui32DeathMyState )
	{
		if( SLOT_MAX_COUNT > m_pInfo->_MiDinoInfo._uiTRexSlotIdx )
		{
			m_pi16MatAssist[ m_pInfo->_MiDinoInfo._uiTRexSlotIdx ]++;		// 티라노 기여도
		}
		m_pi16MatAssist[ i32SlotIdx ]++;									// 어시스트 킬
	}

	m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreKill;

	return ui16GainPoint;
}

BOOL CRoomInCrossCount::OnUpdatePreBattle( UINT32 ui32RTState, UINT32 ui32RTServer )
{
	if( (ui32RTState + MISSION_PREBATTLE_TIME) > ui32RTServer ) return FALSE;

	return TRUE;
}

void CRoomInCrossCount::SetMatchStart()
{
	i3mem::FillZero( m_pi16MatAssist, sizeof(INT16)*SLOT_MAX_COUNT );
}

void CRoomInCrossCount::SetRoundStart()
{
	_InitUserState();
}

BOOL CRoomInCrossCount::CheckRoundEnd( UINT32 ui32RTServer )
{	
	return TRUE;
}

BOOL CRoomInCrossCount::CheckMatchEnd()
{
	if( (UINT32)(m_pInfo->_InfoAdd._NowRoundCount) >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInCrossCount::GetWinTeam()
{
	if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount > m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	return TEAM_RED_WIN;
	if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount < m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

void CRoomInCrossCount::GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint )
{
	CRoomBase::GetBasicExpPoint( i32Win, pstGetExpPoint );

	// 비겼을 경우 킬 카운트가 없으면 Draw 0 처리
	if( TEAM_DRAW != i32Win )							return;
	if( 0 != m_pInfo->_aTeamScoreTotal[0].m_KillCount )	return;
	if( 0 != m_pInfo->_aTeamScoreTotal[1].m_KillCount )	return;

	pstGetExpPoint->_i16DrawExp		= 0;
	pstGetExpPoint->_i16DrawPoint	= 0;
}

REAL32 CRoomInCrossCount::GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count )
{
	REAL32 r32Rate = (REAL32)i32Count / ((REAL32)i32AllCount*0.5f);

	if( r32Rate < 1 ) r32Rate = 1.0f;

	return 2.0f-r32Rate;
}

REAL32 CRoomInCrossCount::GetSubPercentOfUnusualEnd( INT32 i32Win )
{
	return 1.0f;
}

void CRoomInCrossCount::AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->m_ui16RoundStartUser	= m_pInfo->_ui16RoundStartUser;
	pPacket->m_ui8NowRoundCount		= m_pInfo->_InfoAdd._NowRoundCount;
	pPacket->m_ui16TotalRoundCount.SetValue( m_pInfo->_aui16TotalRoundCount, 0, TEAM_COUNT );
	pPacket->m_MiDinoInfo			= m_pInfo->_MiDinoInfo;
}

void CRoomInCrossCount::AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession )
{
	pPacket->WriteData( &m_pInfo->_MiDinoInfo,				sizeof(DINO_ROOM_INFO) );
}

void CRoomInCrossCount::_InitUserState()
{
	m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
	i3mem::Fill( m_pInfo->_MiDinoInfo._uiRaptorSlotIdx, 255,	sizeof(UINT8)*RAPTOR_TOTAL_CNT );
	i3mem::FillZero( m_pInfo->_abDinoStateUpdate,				sizeof(bool)*SLOT_MAX_COUNT );
	i3mem::FillZero( m_pInfo->_aui32DInoCharInfo,				sizeof(UINT32)*SLOT_MAX_COUNT );

	
	 m_ui8DinoTeam = GetAtkDefChange() ? 1 : 0;
	UINT32 ui32Idx			= 0;

	m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
		
		for( INT32 i = m_ui8DinoTeam; i < SLOT_MAX_COUNT ; i+=2 )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_aSlotInfo[i]._State ) continue;

			ITEM_INFO DinoInfo;
			m_pInfo->_pSession[i]->m_MultiSlot.GetCommonEquipment( CHAR_EQUIPMENT_COMMON_DINO, &DinoInfo);

			// vv3 - check
			m_pInfo->_MiDinoInfo._uiRaptorSlotIdx[ui32Idx]	= (UINT8)i;

			// 추후 주석 부분으로 변경 예정 
			m_pInfo->_aui32DInoCharInfo[i]					= m_pInfo->_pSession[i]->m_aTBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ].m_TItemID;

			ui32Idx++;
		}

	// init 에서 공룡으로 설정된 유저는 배틀 시작시 한번에 공룡 상태를 전달하므로 m_pInfo->bDinoStateUpdate를 갱신하지 않는다.

	m_bNewTrex = false;
}

bool CRoomInCrossCount::CheckTrex( void )
{
	if( m_pInfo->_i32MiStateType != MISSION_STATE_BATTLE ) return FALSE;

	// 유저 누적 정보 초기화

	// 공룡 출현 조건
	DINO_ROOM_INFO * pDinoInfo = &m_pInfo->_MiDinoInfo;	

	//랩터 나간 유저 처리 
	for(INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++)
	{
		if( pDinoInfo->_uiRaptorSlotIdx[i] == 255)continue; 
		if( SLOT_STATE_BATTLE == m_pInfo->_aSlotInfo[ pDinoInfo->_uiRaptorSlotIdx[i]]._State )continue; 
		//나간 유저의 슬롯은 빈 슬롯이 되므로 공룡정보가 의미 없다.
		//m_pInfo->_ui32DInoCharInfo[pDinoInfo->_uiRaptorSlotIdx[i]] = 0;
		//m_pInfo->bDinoStateUpdate[pDinoInfo->_uiRaptorSlotIdx[i]] = true;
		pDinoInfo->_uiRaptorSlotIdx[i] =255; 
	}

	return TRUE; 
}

void CRoomInCrossCount::UpdateUserState( INT32 i32Idx )
{
	if(m_ui8DinoTeam != (i32Idx %2))return;

	//새로들어온 유저처리 
	DINO_ROOM_INFO * pDinoInfo = &m_pInfo->_MiDinoInfo;	

	bool bMakeNewRaptor = true ; 
	//랩터로 줘야 합니다. 
	for(INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++)
	{
		if( pDinoInfo->_uiRaptorSlotIdx[i] == i32Idx)
		{
			bMakeNewRaptor = false; 
		}; 
	}

	if(bMakeNewRaptor)
	{
		for(INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++)
		{
			if( pDinoInfo->_uiRaptorSlotIdx[i] == 255)
			{
				pDinoInfo->_uiRaptorSlotIdx[i]		= (UINT8)i32Idx; 
				m_pInfo->_abDinoStateUpdate[i32Idx]	= true;

				// vv3 - check
				// 추후 주석 부분으로 변경 예정
				m_pInfo->_aui32DInoCharInfo[i32Idx] = m_pInfo->_pSession[i32Idx]->m_aTBattleEquipParts[ CHAR_EQUIPMENT_PARTS_CHARA ].m_TItemID;
				break; 
			} 
		}
	}

	//for( int i = 0; i < RAPTOR_TOTAL_CNT; i++ )
	//	I3TRACE( "\t (%d) Raptor : %d\n", i32Idx, pDinoInfo->_uiRaptorSlotIdx[i]);
	//I3TRACE( "\t (%d) TRex : %d\n", i32Idx, pDinoInfo->_uiTRexSlotIdx );
	return; 
}

BOOL CRoomInCrossCount::CheckVoteTime( UINT32 ui32ServerTime )
{
	if ( (UINT32)(m_pInfo->_InfoAdd._NowRoundCount) + 1 == m_ui32RoundCount )
	{
		UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
		if( (m_pInfo->_ui32RTRoundStart + g_TeamTimeTable[ui8EndTimeIdx]) <= ui32ServerTime + VOTE_FINISH_TIME )
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CRoomInCrossCount::GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer )
{
	UINT8	ui8EndTimeIdx		= (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	pRoomNowScore->ScoreType	= ROOM_SCORE_KILLCOUNT_TYPE;
	pRoomNowScore->PlayTime = g_TeamTimeTable[ui8EndTimeIdx] - (ui32RTServer - m_pInfo->_ui32RTRoundStart);
			
	for(INT32 Loop = 0; Loop < TEAM_COUNT; Loop++)
	{
		pRoomNowScore->Score[Loop] = m_pInfo->_aTeamScoreTotal[Loop].m_KillCount;
	}
}

void CRoomInCrossCount::CommandIncreaseKillCount( INT32 i32SlotIdx, INT32 i32KillCount )
{
	m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += (UINT16)(m_ui8ScoreKill*i32KillCount);
}