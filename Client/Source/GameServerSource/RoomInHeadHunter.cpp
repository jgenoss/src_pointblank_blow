#include "pch.h"
#include "RoomInHeadHunter.h"

I3_CLASS_INSTANCE( CRoomInHeadHunter, CRoomBase );

CRoomInHeadHunter::CRoomInHeadHunter()
{
}

CRoomInHeadHunter::~CRoomInHeadHunter()
{
	OnDestroy();
}

BOOL CRoomInHeadHunter::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
#ifndef NEW_EXPPOINT
	m_stGetExpPoint._i16WinExp			= 80;
	m_stGetExpPoint._i16LoseExp			= 20;
	m_stGetExpPoint._i16DrawExp			= 50;
	m_stGetExpPoint._i16WinPoint		= 80;
	m_stGetExpPoint._i16LosePoint		= 20;
	m_stGetExpPoint._i16DrawPoint		= 50;

	m_stKillExpPoint._i16KillExp		= 10;
	m_stKillExpPoint._i16KillPoint		= 5;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
#else
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 175;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 70;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 2.5	* 10 );
	m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 1.2	* 10 );
	m_stExpPointValues._stRoundExp._i16rHighKillFactor				= (INT16)( 0.8	* 10 );

	m_stExpPointValues._stRoundExp._i16HeadshotScore				= 1;
	m_stExpPointValues._stRoundExp._i16ChainKillScore				= 2;
	m_stExpPointValues._stRoundExp._i16ChainMeleeScore				= 2;
	m_stExpPointValues._stRoundExp._i16HeadshotMax					= 40;
	m_stExpPointValues._stRoundExp._i16ChainKillMax					= 5;
	m_stExpPointValues._stRoundExp._i16ChainMeleeMax				= 5;

	INT16 ai16UserCountValueExp[SLOT_MAX_COUNT]						= { 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 2, 2, 1, 1 };
	i3mem::Copy( m_stExpPointValues._stRoundExp._ai16rUserCountValue, ai16UserCountValueExp, sizeof(INT16)*SLOT_MAX_COUNT );

	m_stExpPointValues._stRoundExpPointRate._i16rWinExp				= (INT16)( 1	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rDrawExp			= (INT16)( 0.8	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rLoseExp			= (INT16)( 0.7	* 10 );

	m_stExpPointValues._stExpPoint._i16rExpPerRound					= (INT16)( 1.7	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerKill					= (INT16)( 3	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerMission				= (INT16)( 2	* 10 );

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

	m_eStartOption						= ROOM_START_DIRECT;
	m_ePreStartOption					= ROOM_PRESTART_ONE_WAIT;
	m_eRespawnOption					= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption					= ROOM_CHATTING_ALL;
	m_eEndOption						= ROOM_END_KILLCOUNT;

#ifndef NEW_EXPPOINT
	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.4501f, 0.4501f, 0.4501f, 0.5501f, 0.5501f, 0.6501f, 0.6501f, 0.7501f, 0.7501f, 0.8501f, 0.8501f, 0.901f, 0.901f, 0.9501f, 0.9501f, 1.01f};
	REAL32 pr32SubUserCountVS4[SLOT4VS4_MAX_COUNT]	= { 0.4001f, 0.4001f, 0.4001f, 0.6501f, 0.6501f, 0.9001f, 0.9001f, 1.001f };

	i3mem::Copy( m_pr32SubUserCount, pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCountVS4, pr32SubUserCountVS4, sizeof(REAL32)*SLOT4VS4_MAX_COUNT );
#endif

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInHeadHunter::OnDestroy()
{
}

BOOL CRoomInHeadHunter::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_TYPE_DEATHMATCH;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= 1;

	return TRUE;
}

BOOL CRoomInHeadHunter::CheckReady()
{
	//if( g_pContextMain->m_ui8MR_HeadHunter > m_pInfo->_SlotInfo[m_pInfo->_i32MainSlotIdx]._Rank )	return FALSE;

	return TRUE;
}

BOOL CRoomInHeadHunter::CheckRoundEnd( UINT32 ui32ServerTime )
{
	m_pInfo->_InfoAdd._NowIngTime = ui32ServerTime - m_pInfo->_ui32BattleStartTime; 

	m_pInfo->_aui16TotalRoundCount[TEAM_RED]		= m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount;
	m_pInfo->_aui16TotalRoundCount[TEAM_BLUE]	= m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount;

	UINT8 ui8EndKillIdx = m_pInfo->_InfoAdd._SubType & 0x0F; 
	UINT8 ui8EndTimeIdx	= (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	
	m_pInfo->_InfoAdd._NowIngTime = ui32ServerTime - m_pInfo->_ui32BattleStartTime; 

	// Check Time 			
	if( m_pInfo->_InfoAdd._NowIngTime > g_TeamTimeTable[ui8EndTimeIdx] )
	{
		if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount > m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	m_pInfo->_ui8RoundWinTeam = TEAM_RED_WIN;
		if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount < m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	m_pInfo->_ui8RoundWinTeam = TEAM_BLUE_WIN;
		m_pInfo->_ui8MiEndType		= MISSION_END_TIMEOUT;  
		return TRUE;
	}

	// AI 모드의 경우 KillCount로 끝나지 않는다.
	if( STAGE_MASK_TYPE_TRAINING == m_pInfo->_InfoBasic._StageMask )					return FALSE;

	//Check kill Count
	if( g_TeamKillCountTable[ui8EndKillIdx] <= m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount ||
		g_TeamKillCountTable[ui8EndKillIdx] <= m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )
	{
		if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount > m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	m_pInfo->_ui8RoundWinTeam = TEAM_RED_WIN;
		if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount < m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	m_pInfo->_ui8RoundWinTeam = TEAM_BLUE_WIN;
		m_pInfo->_ui8MiEndType		= MISSION_END_TIMEOUT;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CRoomInHeadHunter::CheckMatchEnd()
{
	if( (UINT32)(m_pInfo->_InfoAdd._NowRoundCount) >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInHeadHunter::GetWinTeam()
{
	if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount > m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	return TEAM_RED_WIN;
	if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount < m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

#ifndef NEW_EXPPOINT
REAL32 CRoomInHeadHunter::GetSubPercentOfUnusualEnd( INT32 i32Win )
{
	REAL32 r32SubPercent = 1.f;
	
	UINT8 ui8EndKillIdx = (m_pInfo->_InfoAdd._SubType & 0x0F);
	if( TEAM_DRAW != i32Win )//방어코드 들어올 일 없음
	{
		r32SubPercent = ((REAL32)m_pInfo->_aTeamScoreTotal[i32Win].m_KillCount / (REAL32)g_TeamKillCountTable[ui8EndKillIdx] );
	}
	
	if(r32SubPercent > 0.8f)		r32SubPercent = 0.5f; 
	else if(r32SubPercent > 0.6f)	r32SubPercent = 0.4f; 
	else if(r32SubPercent > 0.4f)	r32SubPercent = 0.3f; 
	else if(r32SubPercent > 0.2f)	r32SubPercent = 0.2f; 
	else r32SubPercent = 0.1f;

	return r32SubPercent;
}
#endif

BOOL CRoomInHeadHunter::CheckVoteTime( UINT32 ui32ServerTime )
{
	if ( (UINT32)(m_pInfo->_InfoAdd._NowRoundCount) + 1 == m_ui32RoundCount )
	{
		UINT8 ui8EndTimeIdx	= (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
		if( g_TeamTimeTable[ui8EndTimeIdx] + m_pInfo->_ui32BattleStartTime < ui32ServerTime + VOTE_FINISH_TIME )
		{
			return FALSE;
		}
	}

	return TRUE;
}