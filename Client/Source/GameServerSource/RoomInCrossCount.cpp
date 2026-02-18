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
#ifndef NEW_EXPPOINT
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stGetExpPoint._i16WinExp			= 120;
	m_stGetExpPoint._i16LoseExp			= 80;
	m_stGetExpPoint._i16DrawExp			= 100;
	m_stGetExpPoint._i16WinPoint		= 100;
	m_stGetExpPoint._i16LosePoint		= 60;
	m_stGetExpPoint._i16DrawPoint		= 80;

	m_stKillExpPoint._i16KillExp		= 8;
	m_stKillExpPoint._i16KillPoint		= 4;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

#else
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 250;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 80;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 2	* 10 );
	m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 105	* 10 );
	m_stExpPointValues._stRoundExp._i16rHighKillFactor				= (INT16)( 1	* 10 );

	m_stExpPointValues._stRoundExp._i16HeadshotScore				= 2;
	m_stExpPointValues._stRoundExp._i16ChainKillScore				= 2;
	m_stExpPointValues._stRoundExp._i16ChainMeleeScore				= 2;
	m_stExpPointValues._stRoundExp._i16HeadshotMax					= 10;
	m_stExpPointValues._stRoundExp._i16ChainKillMax					= 20;
	m_stExpPointValues._stRoundExp._i16ChainMeleeMax				= 10;

	INT16 ai16UserCountValueExp[SLOT_MAX_COUNT]						= { 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 2, 2, 1, 1 };
	i3mem::Copy( m_stExpPointValues._stRoundExp._ai16rUserCountValue, ai16UserCountValueExp, sizeof(INT16)*SLOT_MAX_COUNT );

	m_stExpPointValues._stRoundExpPointRate._i16rWinExp				= (INT16)( 1	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rDrawExp			= (INT16)( 0.8	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rLoseExp			= (INT16)( 0.7	* 10 );

	m_stExpPointValues._stExpPoint._i16rExpPerRound					= (INT16)( 1	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerKill					= (INT16)( 2.5	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerMission				= (INT16)( 1	* 10 );

	m_stExpPointValues._stMemberRate._bUseMemberRate				= TRUE;
	m_stExpPointValues._stMemberRate._i16MemberRangeEnd				= 6;
	m_stExpPointValues._stMemberRate._i16rMemberRangeRate			= 9;
	m_stExpPointValues._stMemberRate._i16rmemberMinRate				= 3;
																	// 랭크 점수는 *100 해서 적으세요.
	INT16 ai16RankPoint[SLOT_MAX_COUNT]								= { 1000, 900, 700, 650, 600, 550, 500, 450, 400, 350, 300, 250, 200, 150, 100, 050 };
	i3mem::Copy( m_stExpPointValues._stRoundPoint._ai16r2RankPoint, ai16RankPoint, sizeof(INT16)*SLOT_MAX_COUNT );
																	// 인원 점수는 * 10 해서 적으세요.
	INT16 ai16UserCountValuePoint[SLOT_MAX_COUNT]					= { 128, 128, 112, 112, 96, 96, 80, 80, 64, 64, 48, 48, 32, 32, 16, 16 };
	i3mem::Copy( m_stExpPointValues._stRoundPoint._ai16rUserCountValue, ai16UserCountValuePoint, sizeof(INT16)*SLOT_MAX_COUNT );

	m_stExpPointValues._stRoundPoint._i16HeadshotScore				= 5;
	m_stExpPointValues._stRoundPoint._i16ChainKillScore				= 4;
	m_stExpPointValues._stRoundPoint._i16ChainMeleeScore			= 2;
	m_stExpPointValues._stRoundPoint._i16HeadshotMax				= 20;
	m_stExpPointValues._stRoundPoint._i16ChainKillMax				= 20;
	m_stExpPointValues._stRoundPoint._i16ChainMeleeMax				= 20;

	m_stExpPointValues._stRoundPoint._i16BonusScore					= 1;
	INT16 ai16BonusMaxCount[MISSION_POINT_MAX_COUNT]				= {20, 40, 60, 80, 100, 120};
	i3mem::Copy( m_stExpPointValues._stRoundPoint._ai16BonusMaxCount, ai16BonusMaxCount, sizeof(INT16)*MISSION_POINT_MAX_COUNT );

	m_stExpPointValues._stRoundExpPointRate._i16r2WinPoint			= (INT16)( 0.8	* 100 );
	m_stExpPointValues._stRoundExpPointRate._i16r2DrawPoint			= (INT16)( 0.75	* 100 );
	m_stExpPointValues._stRoundExpPointRate._i16r2LosePoint			= (INT16)( 0.7	* 100 );

#endif

	m_ui8ScoreKill						= 1;

	m_eStartOption						= ROOM_START_COUNTDOWN;
	m_ePreStartOption					= ROOM_PRESTART_ALL_WAIT;
	m_eRespawnOption					= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption					= ROOM_CHATTING_ALL;
	m_eEndOption						= ROOM_END_MOD;

#ifndef NEW_EXPPOINT
	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.101f, 0.301f, 0.301f, 0.501f, 0.501f, 0.701f, 0.701f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f  };
	REAL32 pr32SubUserCountVS4[SLOT4VS4_MAX_COUNT]	= { 0.101f, 0.301f, 0.701f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f };

	i3mem::Copy( m_pr32SubUserCount, pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCountVS4, pr32SubUserCountVS4, sizeof(REAL32)*SLOT4VS4_MAX_COUNT );
#endif

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInCrossCount::OnDestroy()
{
}

BOOL CRoomInCrossCount::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_TYPE_CROSSCOUNT;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= 2;

	// Dino Info
	m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
	i3mem::Fill( m_pInfo->_MiDinoInfo._uiRaptorSlotIdx, 255, sizeof(UINT8)*RAPTOR_TOTAL_CNT );
	i3mem::FillZero( m_pInfo->_abDinoStateUpdate, sizeof(m_pInfo->_abDinoStateUpdate) );
	i3mem::FillZero( m_pInfo->_aui32DInoCharInfo, sizeof(m_pInfo->_aui32DInoCharInfo) );

	return TRUE;
}

#ifndef NEW_EXPPOINT
void
#else
UINT16 
#endif
	CRoomInCrossCount::UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )
{
#ifdef NEW_EXPPOINT
	UINT16 ui16GainPoint = CRoomBase::UserKillBouns(i32SlotIdx, ui32DeathType, ui32DeathMyState, ui32DeathEnemyState, ui32RTBattle);
#endif

	// Assist를 했는지
	if( CHARA_DEATH_MY_STATE_ASSIST == ui32DeathMyState )
	{
		if( 255 > m_pInfo->_MiDinoInfo._uiTRexSlotIdx )
		{
			m_pi16MatAssist[ m_pInfo->_MiDinoInfo._uiTRexSlotIdx ]++;		// 티라노 기여도
		}
		m_pi16MatAssist[ i32SlotIdx ]++;									// 어시스트 킬
	}

	m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreKill;

	UINT8 IDinoTeam = (m_pInfo->_InfoAdd._NowRoundCount+1) % 2;
	if(IDinoTeam == (i32SlotIdx % TEAM_COUNT))
	{
		m_pInfo->_aui16M_KillHuman[i32SlotIdx]++;
	}

#ifdef NEW_EXPPOINT
	return ui16GainPoint;
#endif
}

BOOL CRoomInCrossCount::OnUpdatePreBattle( UINT32 ui32StateTime, UINT32 ui32ServerTime )
{
	if( (ui32StateTime + MISSION_PREBATTLE_TIME) > ui32ServerTime ) return FALSE;

	return TRUE;
}

void CRoomInCrossCount::SetMatchStart()
{
	i3mem::FillZero( m_pi16MatAssist, sizeof(m_pi16MatAssist) );
}

void CRoomInCrossCount::SetRoundStart()
{
	_InitUserState();
}

BOOL CRoomInCrossCount::CheckRoundEnd( UINT32 ui32ServerTime )
{
	BOOL bRoundEnd = FALSE; 

	m_pInfo->_InfoAdd._NowIngTime = ui32ServerTime - m_pInfo->_ui32BattleStartTime; 

	//시간 체크 
	UINT8 ui8EndKillIdx = m_pInfo->_InfoAdd._SubType & 0x0F; 
	UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	if( (m_pInfo->_ui32BattleStartTime + g_TeamTimeTable[ui8EndTimeIdx]) < ui32ServerTime )
	//if( (m_pInfo->_ui32BattleStartTime + 30) < ui32ServerTime )
	{
		m_pInfo->_ui8MiEndType		= MISSION_END_TIMEOUT; 
		m_pInfo->_ui8RoundWinTeam	= TEAM_DRAW;
		bRoundEnd					= TRUE; 
	}

	if( FALSE == bRoundEnd ) return FALSE;

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

#ifndef NEW_EXPPOINT
void CRoomInCrossCount::GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint )
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

#endif

void CRoomInCrossCount::AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	UINT8 ui8NowRoundCount = m_pInfo->_InfoAdd._NowRoundCount;
	pPacket->WriteData( m_pInfo->_aui16TotalRoundCount,		(sizeof(UINT16) * TEAM_COUNT));
	pPacket->WriteData( &ui8NowRoundCount,					sizeof(UINT8) );
	pPacket->WriteData( &m_pInfo->_ui16RoundStartUser,		sizeof(UINT16) );
	pPacket->WriteData( &m_pInfo->_MiDinoInfo,				sizeof(DINO_ROOM_INFO) );

}

void CRoomInCrossCount::AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession )
{
	pPacket->WriteData( &m_pInfo->_MiDinoInfo,				sizeof(DINO_ROOM_INFO) );
}

void CRoomInCrossCount::AddDeathBattlePacket( i3NetworkPacket* pPacket )
{
	pPacket->WriteData( m_pInfo->_aui16TotalRoundCount, (sizeof(UINT16) * TEAM_COUNT) );
}

void CRoomInCrossCount::_InitUserState()
{
	INT32 i32RedUserCount = 0;
	INT32 i32BlueUserCount = 0;
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;

		if( 0 == (i%2) )	i32RedUserCount++;
		else				i32BlueUserCount++;
	}

	m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
	i3mem::Fill( m_pInfo->_MiDinoInfo._uiRaptorSlotIdx, 255,	sizeof(UINT8)*RAPTOR_TOTAL_CNT );
	i3mem::FillZero( m_pInfo->_abDinoStateUpdate,	sizeof(m_pInfo->_abDinoStateUpdate) );
	i3mem::FillZero( m_pInfo->_aui32DInoCharInfo,	sizeof(m_pInfo->_aui32DInoCharInfo) );
	if( 0 == m_pInfo->_InfoAdd._NowRoundCount )
	{
		UINT32 ui32Idx	= 0;

		m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
		
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i+=2 )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
			
			m_pInfo->_MiDinoInfo._uiRaptorSlotIdx[ui32Idx]	= i;
			m_pInfo->_aui32DInoCharInfo[i] = m_pInfo->_pSession[i]->m_UseCharaList.GetDinoNum();

			ui32Idx++;
		}
	}
	else
	{
		UINT32 ui32Idx	= 0;

		m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
		
		for( INT32 i = 1 ; i < SLOT_MAX_COUNT ; i+=2 )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
			
			m_pInfo->_MiDinoInfo._uiRaptorSlotIdx[ui32Idx]	= i;
			m_pInfo->_aui32DInoCharInfo[i] = m_pInfo->_pSession[i]->m_UseCharaList.GetDinoNum();

			ui32Idx++;
		}
	}

	// init 에서 공룡으로 설정된 유저는 배틀 시작시 한번에 공룡 상태를 전달하므로 m_pInfo->bDinoStateUpdate를 갱신하지 않는다.

	m_bNewTrex = false;

	//for( int i = 0; i < RAPTOR_TOTAL_CNT; i++ )
	//	I3TRACE( "\t Init Raptor : %d\n", m_pInfo->_MiDinoInfo._uiRaptorSlotIdx[i]);
	//I3TRACE( "\t Init TRex : %d\n", m_pInfo->_MiDinoInfo._uiTRexSlotIdx );
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
		if( SLOT_STATE_BATTLE == m_pInfo->_SlotInfo[ pDinoInfo->_uiRaptorSlotIdx[i]]._State )continue; 
		//나간 유저의 슬롯은 빈 슬롯이 되므로 공룡정보가 의미 없다.
		//m_pInfo->_ui32DInoCharInfo[pDinoInfo->_uiRaptorSlotIdx[i]] = 0;
		//m_pInfo->bDinoStateUpdate[pDinoInfo->_uiRaptorSlotIdx[i]] = true;
		pDinoInfo->_uiRaptorSlotIdx[i] =255; 
	}

	return TRUE; 
}

void CRoomInCrossCount::UpdateUserState( INT32 i32Idx )
{
	// 라운드가 시작되기 전이면 다음 라운드 시작을 기준으로 맞춘다.
	UINT8 ui8Round = m_pInfo->_InfoAdd._NowRoundCount;
	
	//공룡팀인덱스를 구한다. 
	UINT8 IDinoTeam = (ui8Round+1) % 2;

	if(IDinoTeam != (i32Idx %2))return; 

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
				pDinoInfo->_uiRaptorSlotIdx[i] = i32Idx; 
				m_pInfo->_abDinoStateUpdate[i32Idx] = true; 
				m_pInfo->_aui32DInoCharInfo[i32Idx] = m_pInfo->_pSession[i32Idx]->m_UseCharaList.GetDinoNum();
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
		if( (m_pInfo->_ui32BattleStartTime + g_TeamTimeTable[ui8EndTimeIdx]) <= ui32ServerTime + VOTE_FINISH_TIME )
		{
			return FALSE;
		}
	}

	return TRUE;
}
