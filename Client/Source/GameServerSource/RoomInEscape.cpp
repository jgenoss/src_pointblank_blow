#include "pch.h"
#include "RoomInEscape.h"
#include "ModuleContextNC.h"

I3_CLASS_INSTANCE( CRoomInEscape, CRoomBase );

CRoomInEscape::CRoomInEscape()
{
}

CRoomInEscape::~CRoomInEscape()
{
	OnDestroy();
}

BOOL CRoomInEscape::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
#ifndef NEW_EXPPOINT
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stGetExpPoint._i16WinExp			= 270;
	m_stGetExpPoint._i16LoseExp			= 170;
	m_stGetExpPoint._i16DrawExp			= 200;
	m_stGetExpPoint._i16WinPoint		= 150;
	m_stGetExpPoint._i16LosePoint		= 90;
	m_stGetExpPoint._i16DrawPoint		= 110;

	m_stKillExpPoint._i16KillExp		= 0;
	m_stKillExpPoint._i16KillPoint		= 0;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

	// 보너스 경험치/포인트 (모드 전용)
	m_ui8BonusEscapePoint				= 1;			
	m_ui8BonusEscapeExp					= 5;			
	m_ui8BonusChainEscapePoint			= 3;		
	m_ui8BonusKillPoint					= 1;			
	m_ui8BonusKillExp					= 2;				
	m_ui8BonusInterceptPoint			= 3;		

	m_ui8BonusMaxEscapePoint			= 10;
	m_ui8BonusMaxEscapeExp				= 50;
	m_ui8BonusMaxChainEscapePoint		= 12;
	m_ui8BonusMaxKillPoint				= 20;
	m_ui8BonusMaxKillExp				= 40;
	m_ui8BonusMaxInterceptPoint			= 12;
#else
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 60;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 15;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 2	* 10 );
	m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 1	* 10 );
	m_stExpPointValues._stRoundExp._i16rHighKillFactor				= (INT16)( 0.8	* 10 );

	m_stExpPointValues._stRoundExp._i16HeadshotScore				= 1;
	m_stExpPointValues._stRoundExp._i16ChainKillScore				= 1;
	m_stExpPointValues._stRoundExp._i16ChainMeleeScore				= 1;
	m_stExpPointValues._stRoundExp._i16HeadshotMax					= 5;
	m_stExpPointValues._stRoundExp._i16ChainKillMax					= 5;
	m_stExpPointValues._stRoundExp._i16ChainMeleeMax				= 5;

	INT16 ai16UserCountValueExp[SLOT_MAX_COUNT]						= { 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 2, 2, 1, 1 };
	i3mem::Copy( m_stExpPointValues._stRoundExp._ai16rUserCountValue, ai16UserCountValueExp, sizeof(INT16)*SLOT_MAX_COUNT );

	m_stExpPointValues._stRoundExpPointRate._i16rWinExp				= (INT16)( 1	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rDrawExp			= (INT16)( 0.8	* 10 );
	m_stExpPointValues._stRoundExpPointRate._i16rLoseExp			= (INT16)( 0.7	* 10 );

	m_stExpPointValues._stExpPoint._i16rExpPerRound					= (INT16)( 1.5	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerKill					= (INT16)( 2	* 10 );
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

	// 보너스 경험치/포인트 (모드 전용)
	m_ui8BonusEscapeMax					= 10;
	m_ui8BonusEscapeScore				= 5;
	m_ui8BonusInterceptMax				= 4;
	m_ui8BonusInterceptScore			= 3;
	m_ui8BonusDiffMax					= 5;
	m_ui8BonusDiffStandard				= 10;
	m_ui8BonusDiffScore					= 5;
#endif

	m_ui8ScoreKill						= 1;
	m_ui8ScoreIntercept					= 2;
	m_ui8ScoreBlock						= 5;
	m_ui8ScoreEscape					= 5;
	m_ui8ScoreAceattacker				= 7;
	m_ui8ScoreHattrick					= 10;

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

	m_i16TouchDownFlag = 0;

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInEscape::OnDestroy()
{
}

BOOL CRoomInEscape::CheckSubType( UINT8 ui8SubType )
{
	// 모드별로 플레이 가능한 승리조건인지, 제한시간이 맞는지 확인
	UINT8 ui8RoundEndTimeIdx = (ui8SubType & 0xF0) >> 4;

	if( ui8RoundEndTimeIdx == 1 || ui8RoundEndTimeIdx == 3 ) return TRUE;

	return FALSE;
}

BOOL CRoomInEscape::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_TYPE_ESCAPE;
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
	CRoomInEscape::UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )
{
#ifdef NEW_EXPPOINT
	UINT16 ui16GainPoint = CRoomBase::UserKillBouns(i32SlotIdx, ui32DeathType, ui32DeathMyState, ui32DeathEnemyState, ui32RTBattle);
#endif

	// Assist를 했는지
	if( CHARA_DEATH_MY_STATE_ASSIST & ui32DeathMyState )
	{
		if( 255 > m_pInfo->_MiDinoInfo._uiTRexSlotIdx )
		{
			m_aui16MatAssist[ m_pInfo->_MiDinoInfo._uiTRexSlotIdx ]++;		// 티라노 기여도
		}
		m_aui16MatAssist[ i32SlotIdx ]++;									// 어시스트 킬
	}

	UINT8 IDinoTeam = (m_pInfo->_InfoAdd._NowRoundCount+1) % 2;
	if(IDinoTeam == (i32SlotIdx % TEAM_COUNT))
	{
		m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreKill;
		m_pInfo->_aui16M_KillHuman[i32SlotIdx]++;
	}

	// 탈출지점 근처에서 킬한경우
	if( ui32DeathEnemyState & CHARA_DEATH_ENEMY_STATE_ESCAPE )
	{
		m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreIntercept;
		m_pInfo->_aui16M_Intercept[i32SlotIdx]++;
	}

#ifdef NEW_EXPPOINT
	return ui16GainPoint;
#endif
}

BOOL CRoomInEscape::OnUpdatePreBattle( UINT32 ui32StateTime, UINT32 ui32ServerTime )
{
	if( (ui32StateTime + MISSION_PREBATTLE_TIME) > ui32ServerTime ) return FALSE;

	return TRUE;
}

void CRoomInEscape::SetMatchStart()
{
	i3mem::FillZero( m_aui16MatAssist, sizeof(m_aui16MatAssist) );
}

void CRoomInEscape::SetRoundStart()
{
	i3mem::FillZero( m_aui16RouTouchDownCount, sizeof(m_aui16RouTouchDownCount) );

	_InitUserState();
}

bool CRoomInEscape::RoundStart( UINT32 ui32RTBattle )
{
	TouchdownBlockTimeReset( 
#ifdef USE_GM_PAUSE
							ui32RTBattle
#else
							g_pContextMain->GetServerTime()
#endif
															);
	return false;
}

BOOL CRoomInEscape::CheckRoundEnd( UINT32 ui32ServerTime )
{
	BOOL bRoundEnd = FALSE; 

	m_pInfo->_InfoAdd._NowIngTime = ui32ServerTime - m_pInfo->_ui32BattleStartTime; 

	//시간 체크 
	UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	if( (m_pInfo->_ui32BattleStartTime + g_TeamTimeTable[ui8EndTimeIdx]) < ui32ServerTime )
	{
		m_pInfo->_ui8MiEndType		= MISSION_END_TIMEOUT; 
		m_pInfo->_ui8RoundWinTeam	= TEAM_DRAW;
		bRoundEnd					= TRUE; 
	}

	if( FALSE == bRoundEnd ) return FALSE;

	return TRUE;
}

BOOL CRoomInEscape::CheckMatchEnd()
{
	if( (UINT32)(m_pInfo->_InfoAdd._NowRoundCount) >= m_ui32RoundCount )		return TRUE;

	return FALSE;
}

INT32 CRoomInEscape::GetWinTeam()
{
	// 점수가 높은쪽이 승리
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] > m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_RED_WIN;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] < m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

#ifndef NEW_EXPPOINT
void CRoomInEscape::GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint )
{
	i3mem::Copy( pstGetExpPoint, &m_stGetExpPoint, sizeof(TYPE_ROOM_GET_EXPPOINT) );
}

REAL32 CRoomInEscape::GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count )
{
	REAL32 r32Rate = (REAL32)i32Count / ((REAL32)i32AllCount*0.5f);

	if( r32Rate < 1 ) r32Rate = 1.0f;

	return 2.0f-r32Rate;
}

REAL32 CRoomInEscape::GetSubPercentOfUnusualEnd( INT32 i32Win )
{
	return 1.0f;
}

UINT16 CRoomInEscape::GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp )
{
	UINT16	ui16Exp = ui16AccExp;

	ui16Exp += ( ( m_pInfo->_aui8AccMissionCount[i32SlotIdx] > m_ui8BonusMaxEscapeExp ) ? m_ui8BonusMaxEscapeExp : m_pInfo->_aui8AccMissionCount[i32SlotIdx] ) * m_ui8BonusEscapeExp;
	ui16Exp += ( ( m_pInfo->_aui16M_KillHuman[i32SlotIdx] > m_ui8BonusMaxKillExp ) ? m_ui8BonusMaxKillExp : m_pInfo->_aui16M_KillHuman[i32SlotIdx] ) * m_ui8BonusKillExp;

	return ui16Exp;
}

UINT16 CRoomInEscape::GetMatchAccPoint( INT32 i32SlotIdx, UINT16 ui16AccPoint )
{
	UINT16	ui16Point = ui16AccPoint;

	ui16Point += ( ( m_pInfo->_aui16M_ChainTouchdown[i32SlotIdx] > m_ui8BonusMaxChainEscapePoint ) ? m_ui8BonusMaxChainEscapePoint : m_pInfo->_aui16M_ChainTouchdown[i32SlotIdx] ) * m_ui8BonusMaxChainEscapePoint;
	ui16Point += ( ( m_pInfo->_aui16M_Intercept[i32SlotIdx] > m_ui8BonusMaxInterceptPoint ) ? m_ui8BonusMaxInterceptPoint : m_pInfo->_aui16M_Intercept[i32SlotIdx] ) * m_ui8BonusInterceptPoint;
	ui16Point += ( ( m_pInfo->_aui8AccMissionCount[i32SlotIdx] > m_ui8BonusMaxEscapePoint ) ? m_ui8BonusMaxEscapePoint : m_pInfo->_aui8AccMissionCount[i32SlotIdx] ) * m_ui8BonusEscapePoint;
	ui16Point += ( ( m_pInfo->_aui16M_KillHuman[i32SlotIdx] > m_ui8BonusMaxKillPoint ) ? m_ui8BonusMaxKillPoint : m_pInfo->_aui16M_KillHuman[i32SlotIdx] ) * m_ui8BonusKillPoint;

	return ui16Point;
}
#else
INT16 CRoomInEscape::GetMissionExp( INT32 i32SlotIdx )
{
	INT16 i16MissionExp = CRoomBase::GetMissionExp(i32SlotIdx);

	i16MissionExp += MIN( m_ui8BonusEscapeMax, m_pInfo->_aui8AccMissionCount[i32SlotIdx] ) * m_ui8BonusEscapeScore;

	i16MissionExp += MIN( m_ui8BonusInterceptMax, m_pInfo->_aui16M_Intercept[i32SlotIdx] ) * m_ui8BonusInterceptScore;

	return i16MissionExp;
}
#endif

UINT16 CRoomInEscape::GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 rAdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle )
{
	if( ExistRecord(i32SlotIdx ) == false ) return 0;

	UINT16	ui16Exp = ui16AccExp;

#ifndef NEW_EXPPOINT
	REAL32	rContributionOfEscape = 1.0f;

	TYPE_ROOM_GET_EXPPOINT stGetExpPoint;
	GetBasicExpPoint(i32Win, &stGetExpPoint);

	switch( i32Win )
	{
	case TEAM_RED_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_RED )
				ui16Exp	+= stGetExpPoint._i16WinExp;
			else
				ui16Exp += stGetExpPoint._i16LoseExp;
		}
		break;
	case TEAM_BLUE_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_BLUE )
				ui16Exp	+= stGetExpPoint._i16WinExp;
			else
				ui16Exp += stGetExpPoint._i16LoseExp;
		}
		break;
	default: // draw / single
		{
			ui16Exp += stGetExpPoint._i16DrawExp;
		}
	}

	// 이긴 팀이면 승점 차이로 보너스를 더 준다.
	if( i32Win == (i32SlotIdx % 2) )
	{
		UINT16 ui16ScoreDiff = m_pInfo->_aui16TotalRoundCount[i32SlotIdx % 2] - m_pInfo->_aui16TotalRoundCount[i32SlotIdx+1 % 2];
		if( ui16ScoreDiff > 50 )
			ui16Exp += 30;
		else if ( ui16ScoreDiff > 20 )
			ui16Exp += 10;
	}

	UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	if( (m_pInfo->_InfoAdd._NowRoundCount) < m_ui32RoundCount || m_pInfo->_InfoAdd._NowIngTime < g_TeamTimeTable[ui8EndTimeIdx] )
	{
		// 비정상 종료일 경우
		// 플레이 시간에 따라 획득량이 달라짐
		UINT32 ui32MaxPlayTime = g_TeamTimeTable[ui8EndTimeIdx]*m_ui32RoundCount;
		UINT32 ui32PlayTime =
#ifdef USE_GM_PAUSE
								ui32RTBattle
#else
								g_pContextMain->GetServerTime()
#endif
																- m_pInfo->_pSession[i32SlotIdx]->m_ui32BattleStartTime;
		if( ui32MaxPlayTime < ui32PlayTime )		ui32PlayTime = ui32MaxPlayTime;

		UINT16 ui16TimeSubRate;

		ui16TimeSubRate = (ui32PlayTime*100)/ui32MaxPlayTime;
		ui16TimeSubRate = ui16TimeSubRate * ui16TimeSubRate;

		ui16Exp = (ui16Exp * ui16TimeSubRate) / 10000;
	}
	else
	{
		// 정상 종료일 경우
		// 인원 비중만 적용
		ui16Exp	= (UINT16)(ui16Exp * rAdjustPercent);
	}
#else
	INT8 i8ScoreDiff = m_pInfo->_aui16TotalRoundCount[i32SlotIdx%TEAM_COUNT] - m_pInfo->_aui16TotalRoundCount[(i32SlotIdx+1)%TEAM_COUNT];
	
	if( i8ScoreDiff > 0 )
	{
		ui16Exp += (m_pstExtPointValues->_stExpPoint._i16rExpPerMission * ( MIN(m_ui8BonusDiffMax, (i8ScoreDiff/m_ui8BonusDiffStandard)) * m_ui8BonusDiffScore )) / 10;
	}

	ui16Exp = CRoomBase::GetMatchEndExp( i32Win, i32SlotIdx, ui16Exp, rAdjustPercent, bNormalEnd, ui32RTBattle );
#endif

	return ui16Exp;
}

#ifndef NEW_EXPPOINT
UINT16 CRoomInEscape::GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 rAdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle )
{
	if( ExistRecord(i32SlotIdx ) == false ) return 0;

	UINT16	ui16Point = ui16AccPoint;
	REAL32	rContributionOfKill = 1.0f;
	UINT32	ui32TeamKill = 0;

	TYPE_ROOM_GET_EXPPOINT stGetExpPoint;
	GetBasicExpPoint(i32Win, &stGetExpPoint);

	switch( i32Win )
	{
	case TEAM_RED_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_RED )
				ui16Point += stGetExpPoint._i16WinPoint;
			else
				ui16Point += stGetExpPoint._i16LosePoint;
		}
		break;
	case TEAM_BLUE_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_BLUE )
				ui16Point += stGetExpPoint._i16WinPoint;
			else
				ui16Point += stGetExpPoint._i16LosePoint;
		}
		break;
	default: // draw / single
		{
			ui16Point += stGetExpPoint._i16DrawPoint;
		}
	}

	UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	if( (m_pInfo->_InfoAdd._NowRoundCount) < m_ui32RoundCount || m_pInfo->_InfoAdd._NowIngTime < g_TeamTimeTable[ui8EndTimeIdx] )
	{
		// 비정상 종료일 경우
		// 플레이 시간에 따라 획득량이 달라짐
		UINT32 ui32MaxPlayTime = g_TeamTimeTable[ui8EndTimeIdx]*m_ui32RoundCount;
		UINT32 ui32PlayTime = 
#ifdef USE_GM_PAUSE
								ui32RTBattle
#else
								g_pContextMain->GetServerTime()
#endif
																- m_pInfo->_pSession[i32SlotIdx]->m_ui32BattleStartTime;
		if( ui32MaxPlayTime < ui32PlayTime )		ui32PlayTime = ui32MaxPlayTime;

		UINT16 ui16TimeSubRate = (ui32PlayTime*100)/ui32MaxPlayTime;
		ui16TimeSubRate = ui16TimeSubRate * ui16TimeSubRate;

		ui16Point = (ui16Point * ui16TimeSubRate) / 10000;
	}
	else
	{
		// 정상 종료일 경우
		// 인원 비중만 적용
		ui16Point	= (UINT16)(ui16Point * rAdjustPercent);
	}

	return ui16Point;
}
#endif

void CRoomInEscape::AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	UINT8 ui8NowRoundCount = m_pInfo->_InfoAdd._NowRoundCount;
	pPacket->WriteData( m_pInfo->_aui16TotalRoundCount,		(sizeof(UINT16) * TEAM_COUNT));
	pPacket->WriteData( &ui8NowRoundCount,					sizeof(UINT8) );
	pPacket->WriteData( &m_pInfo->_ui16RoundStartUser,		sizeof(UINT16) );
	pPacket->WriteData( &m_pInfo->_MiDinoInfo,				sizeof(DINO_ROOM_INFO) );

	UINT8 BlockBonusRemain = m_ui32BlockBonusTime -
#ifdef USE_GM_PAUSE
													ui32RTBattle;
#else
													g_pContextMain->GetServerTime();
#endif
	pPacket->WriteData( &BlockBonusRemain,					sizeof(UINT8) );
}

void CRoomInEscape::AddGiveUpBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession )
{
//	pPacket->WriteData( &m_pInfo->_MiDinoInfo,				sizeof(DINO_ROOM_INFO) );
}

void CRoomInEscape::AddRespawnPacket( i3NetworkPacket* pPacket, CUserSession * pSession )
{
	pPacket->WriteData( &m_pInfo->_MiDinoInfo,				sizeof(DINO_ROOM_INFO) );
}

void CRoomInEscape::AddDeathBattlePacket( i3NetworkPacket* pPacket )
{
	pPacket->WriteData( m_pInfo->_aui16TotalRoundCount, (sizeof(UINT16) * TEAM_COUNT) );
}

void CRoomInEscape::_InitUserState()
{
	INT32 i32RedUserCount = 0;
	INT32 i32BlueUserCount = 0;
	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
		if( TRUE == m_pInfo->_SlotInfo[i]._bIsGmObserverMode ) continue;

		if( 0 == (i%2) )	i32RedUserCount++;
		else				i32BlueUserCount++;
	}

	m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
	i3mem::Fill( m_pInfo->_MiDinoInfo._uiRaptorSlotIdx, 255,	sizeof(UINT8)*RAPTOR_TOTAL_CNT );
	i3mem::FillZero( m_pInfo->_abDinoStateUpdate,	sizeof(m_pInfo->_abDinoStateUpdate) );
	i3mem::FillZero( m_pInfo->_aui32DInoCharInfo,	sizeof(m_pInfo->_aui32DInoCharInfo) );
	if( 0 == m_pInfo->_InfoAdd._NowRoundCount )
	{
		UINT32 ui32TRex	= 255;
		UINT32 ui32Idx	= 0;
		if( 2 > i32RedUserCount )
		{
			m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
		}
		else
		{
			ui32TRex = (rand() % (i32RedUserCount));
		}
		
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i+=2 )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
			if( TRUE == m_pInfo->_SlotInfo[i]._bIsGmObserverMode ) continue;
			
			if( ui32TRex == ui32Idx )	
			{
				m_pInfo->_MiDinoInfo._uiTRexSlotIdx			= i;
				m_pInfo->_aui32DInoCharInfo[i] = MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_TRex);
			}
			else
			{
				m_pInfo->_MiDinoInfo._uiRaptorSlotIdx[ui32Idx]	= i;
				m_pInfo->_aui32DInoCharInfo[i] = m_pInfo->_pSession[i]->m_UseCharaList.GetDinoNum();
			}
			ui32Idx++;
		}
	}
	else
	{
		UINT32 ui32TRex	= 255;
		UINT32 ui32Idx	= 0;
		if( 2 > i32BlueUserCount )
		{
			m_pInfo->_MiDinoInfo._uiTRexSlotIdx = 255;
		}
		else
		{
			ui32TRex = (rand() % (i32BlueUserCount));
		}
		
		for( INT32 i = 1 ; i < SLOT_MAX_COUNT ; i+=2 )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
			if( TRUE == m_pInfo->_SlotInfo[i]._bIsGmObserverMode ) continue;

			if( ui32TRex == ui32Idx )
			{
				m_pInfo->_MiDinoInfo._uiTRexSlotIdx			= i;
				m_pInfo->_aui32DInoCharInfo[i] = MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_TRex);
			}
			else
			{
				m_pInfo->_MiDinoInfo._uiRaptorSlotIdx[ui32Idx]	= i;
				m_pInfo->_aui32DInoCharInfo[i] = m_pInfo->_pSession[i]->m_UseCharaList.GetDinoNum();
			}
			ui32Idx++;
		}
	}

	// init 에서 공룡으로 설정된 유저는 배틀 시작시 한번에 공룡 상태를 전달하므로 m_pInfo->bDinoStateUpdate를 갱신하지 않는다.

	m_bNewTrex = false;

	//for( int i = 0; i < RAPTOR_TOTAL_CNT; i++ )
	//	I3TRACE( "\t Init Raptor : %d\n", m_pInfo->_MiDinoInfo._uiRaptorSlotIdx[i]);
	//I3TRACE( "\t Init TRex : %d\n", m_pInfo->_MiDinoInfo._uiTRexSlotIdx );
}

bool CRoomInEscape::CheckTrex( void )
{
	if( m_pInfo->_i32MiStateType != MISSION_STATE_BATTLE ) return FALSE;

	// 유저 누적 정보 초기화

	// 공룡 출현 조건
	DINO_ROOM_INFO * pDinoInfo = &m_pInfo->_MiDinoInfo;	
	INT8	ai8PlayUserCount[2];

	// 라운드가 시작되기 전이면 다음 라운드 시작을 기준으로 맞춘다.
	UINT8 ui8Round = m_pInfo->_InfoAdd._NowRoundCount;
	
	//공룡팀인덱스를 구한다. 
	UINT8 ui8DinoTeam = (ui8Round+1) % 2; 

	// Rex 출현 조건 확인위해 인원수 계산
	ai8PlayUserCount[0] = 0; 
	ai8PlayUserCount[1] = 0; 

	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
		ai8PlayUserCount[i%2] = ai8PlayUserCount[i%2] + 1; 
	}


	//Trex가 필요한지 체크합니다. 
	if(pDinoInfo->_uiTRexSlotIdx == 255) 
	{
		//새로 들어온 유저처리 
		if(ai8PlayUserCount[ui8DinoTeam] > 1)m_bNewTrex = true; 
	}
	else 
	{
		//나간 유저 처리 
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[pDinoInfo->_uiTRexSlotIdx]._State )
		{
			//나간 유저의 슬롯은 빈 슬롯이 되므로 공룡정보가 의미 없다.
			//m_pInfo->_ui32DInoCharInfo[pDinoInfo->_uiTRexSlotIdx] = 0;
			//m_pInfo->bDinoStateUpdate[pDinoInfo->_uiTRexSlotIdx] = true;
			pDinoInfo->_uiTRexSlotIdx = 255; 
			if(ai8PlayUserCount[ui8DinoTeam] > 1)m_bNewTrex = true; 
		}
	}

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

	//새로 들어온 랩터 처리는 리스폰에서 합니다. 

	//I3TRACE("\t Check Trex (%d)\n", m_bNewTrex);

	return TRUE; 
}

void CRoomInEscape::UpdateUserState( INT32 i32Idx )
{
	m_ui8A_TouchDownChainCount[i32Idx] = 0;

	// 라운드가 시작되기 전이면 다음 라운드 시작을 기준으로 맞춘다.
	UINT8 ui8Round = m_pInfo->_InfoAdd._NowRoundCount;
	
	//공룡팀인덱스를 구한다. 
	UINT8 IDinoTeam = (ui8Round+1) % 2;

	if(IDinoTeam != (i32Idx %2))return; 

	//새로들어온 유저처리 
	DINO_ROOM_INFO * pDinoInfo = &m_pInfo->_MiDinoInfo;	

	if( m_bNewTrex )
	{
		//Trex로 선택 되었습니다. 
		pDinoInfo->_uiTRexSlotIdx = i32Idx; 
		m_bNewTrex = false; 

		//리스폰한 공룡이 랙터이었으면 그 값 삭제합니다. 
		for(INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++)
		{
			if( pDinoInfo->_uiRaptorSlotIdx[i] != i32Idx)continue; 
			pDinoInfo->_uiRaptorSlotIdx[i] =255; 
			break; 
		}

		m_pInfo->_abDinoStateUpdate[i32Idx] = true; 
		m_pInfo->_aui32DInoCharInfo[i32Idx] = MAKE_ITEM_FLAG(ITEM_CLASS_CHARA,		CHARACTER_CLASS_CHARA_DINO,		CHARACTER_TEAM_RB,		CHARACTER_RES_ID_DINO_TRex);
	}
	else 
	{
		bool bMakeNewRaptor = true ; 
		//랩터로 줘야 합니다. 
		for(INT32 i = 0; i < RAPTOR_TOTAL_CNT; i++)
		{
			if( pDinoInfo->_uiRaptorSlotIdx[i] == i32Idx)
			{
				bMakeNewRaptor = false; 
			}; 
		}

		if( pDinoInfo->_uiTRexSlotIdx == i32Idx )
			bMakeNewRaptor = false;

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
	}

	//for( int i = 0; i < RAPTOR_TOTAL_CNT; i++ )
	//	I3TRACE( "\t (%d) Raptor : %d\n", i32Idx, pDinoInfo->_uiRaptorSlotIdx[i]);
	//I3TRACE( "\t (%d) TRex : %d\n", i32Idx, pDinoInfo->_uiTRexSlotIdx );
	return; 
}

void CRoomInEscape::periodicUpdateRoomState( UINT32 ui32ServerTime )
{
	if( m_pInfo->_InfoBasic._State == ROOM_STATE_BATTLE && m_pInfo->_i32MiStateType == MISSION_STATE_BATTLE )
	{
		// 터치다운 한 유저가 없으면 다시 계산할 필요 없다.
		if( m_i16TouchDownFlag > 0 )
		{
			m_i16TouchDownFlag = 0;

			i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_COUNT_ACK );
			SendPacket.WriteData( m_pInfo->_aui16TotalRoundCount,		sizeof(m_pInfo->_aui16TotalRoundCount) );
			_SendPacketBattlePlayer( &SendPacket );
		}

		// 방어 점수
		if( m_ui32BlockBonusTime <= ui32ServerTime)
		{
			UINT8 IDinoTeam = (m_pInfo->_InfoAdd._NowRoundCount+1) % TEAM_COUNT;
			m_pInfo->_aui16TotalRoundCount[IDinoTeam] += m_ui8ScoreBlock;

			m_ui8LastTouchDownState = TOUCHDOWN_BLOCK_STATE_FULL;
			i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_BLOCK_ACK );
			SendPacket.WriteData( &m_ui8LastTouchDownState,				sizeof(UINT8) );
			SendPacket.WriteData( m_pInfo->_aui16TotalRoundCount,		sizeof(m_pInfo->_aui16TotalRoundCount) );
			_SendPacketBattlePlayer( &SendPacket );

			TouchdownBlockTimeReset(ui32ServerTime);

			//I3TRACE("방어점수 획득");
		}
	}
}

BOOL CRoomInEscape::MissionTouchDown( CUserSession * pSession, INT32 i32SlotIdx, UINT32 ui32RTBattle )
{
	if( MISSION_STATE_BATTLE != m_pInfo->_i32MiStateType )			return FALSE;		//배틀중이 아님 
	if( (-1 > i32SlotIdx) || (SLOT_MAX_COUNT <= i32SlotIdx) )		return FALSE;

	// 터치다운 시간
	TouchdownBlockTimeReset( 
#ifdef USE_GM_PAUSE
							ui32RTBattle
#else
							g_pContextMain->GetServerTime()
#endif
															);

	// 개인 정보만 갱신하고 전체 카운트는 메인스레드에서 변경
	m_pInfo->_aui8AccMissionCount[ i32SlotIdx ]++;
	m_aui16RouTouchDownCount[ i32SlotIdx ]++;
	m_pInfo->_aui8A_Mission[i32SlotIdx]++;
	m_ui8A_TouchDownChainCount[i32SlotIdx]++;

	pSession->m_ui32GrenadeCount		= 0;	// 리스폰 루틴을 거치지 않고 리스폰을 진행하여서

	if( m_ui8A_TouchDownChainCount[i32SlotIdx] > 4 )	m_ui8A_TouchDownChainCount[i32SlotIdx] = 4;		// 최대 4회 (Game Maker)

	// Score 갱신
	switch( m_ui8A_TouchDownChainCount[i32SlotIdx] )
	{
	case 4:
	case 3:
		m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreHattrick;
		m_pInfo->_aui16M_ChainTouchdown[i32SlotIdx]++;
		break;
	case 2:
		m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreAceattacker;
		m_pInfo->_aui16M_ChainTouchdown[i32SlotIdx]++;
		break;
	case 1:
		m_pInfo->_aui16TotalRoundCount[(i32SlotIdx % TEAM_COUNT)] += m_ui8ScoreEscape;
		break;
	}

	// ZLog 에 탈출을 남김 - 서동권
	g_pModuleContextNC->LogSendMessage( MODULE_CONTROL_THREAD_IDX, LOG_MESSAGE_U_R_ESCAPE, pSession, &m_aui16RouTouchDownCount[i32SlotIdx], &m_ui8A_TouchDownChainCount[i32SlotIdx] );

	// 토탈카운트를 다시 계산하도록 플래그 갱신
	m_i16TouchDownFlag |= 0x0001 << i32SlotIdx;

	UINT16	ui16TouchDownCount = (UINT16)m_pInfo->_aui8A_Mission[i32SlotIdx];	// 기존 패킷에 16비트로 쓰고 있어서... 

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK );
	SendPacket.WriteData( m_pInfo->_aui16TotalRoundCount,		(sizeof(UINT16) * TEAM_COUNT) );
	SendPacket.WriteData( &i32SlotIdx,							sizeof(INT32));
	SendPacket.WriteData( &ui16TouchDownCount,	sizeof(INT16));
	_SendPacketBattlePlayer( &SendPacket );

	return TRUE;	
}

BOOL CRoomInEscape::CheckVoteTime( UINT32 ui32ServerTime )
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
