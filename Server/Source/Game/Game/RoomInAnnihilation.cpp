#include "pch.h"
#include "RoomInAnnihilation.h"

I3_CLASS_INSTANCE( CRoomInAnnihilation, CRoomBase );

CRoomInAnnihilation::CRoomInAnnihilation()
{
}

CRoomInAnnihilation::~CRoomInAnnihilation()
{
	OnDestroy();
}

BOOL CRoomInAnnihilation::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{

	if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
	{
		// 보너스 경험치/포인트 (공통 설정사항)
		m_stGetExpPointV1._i16WinExp		= 120;
		m_stGetExpPointV1._i16LoseExp		= 60;
		m_stGetExpPointV1._i16DrawExp		= 0;
		m_stGetExpPointV1._i16WinPoint		= 100;
		m_stGetExpPointV1._i16LosePoint		= 50;
		m_stGetExpPointV1._i16DrawPoint		= 0;

		m_stKillExpPoint._i16KillExp		= 15;
		m_stKillExpPoint._i16KillPoint		= 10;
		m_stKillExpPoint._i16AssistExp		= 2;
		m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
		m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
		m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
		m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
		m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
		m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

		// 보너스 경험치/포인트 (모드 전용)
		m_ui8BonusKillTargetCount			= 5;
		m_ui8BonusKillTargetCount4VS4		= 3;
		m_ui8BonusKillExp					= 20;
		m_ui8BonusKillExp4VS4				= 20;
	}
	else
	{
		m_stExpPointValues._stRoundExp._i16Round						= 30;
		m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 20;
		m_stExpPointValues._stRoundExp._i16AssistPerRound1Round			= 8;
		m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 1.5	* 10 );
		m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 1	* 10 );
		m_stExpPointValues._stRoundExp._i16rHighKillFactor				= (INT16)( 0.8	* 10 );

		m_stExpPointValues._stRoundExp._i16HeadshotScore				= 1;
		m_stExpPointValues._stRoundExp._i16ChainKillScore				= 1;
		m_stExpPointValues._stRoundExp._i16ChainMeleeScore				= 1;
		m_stExpPointValues._stRoundExp._i16HeadshotMax					= 5;
		m_stExpPointValues._stRoundExp._i16ChainKillMax					= 5;
		m_stExpPointValues._stRoundExp._i16ChainMeleeMax				= 5;

		INT16 i16UserCountValueExp[SLOT_MAX_COUNT]							= { 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 2, 2, 1, 1 };
		i3mem::Copy( m_stExpPointValues._stRoundExp._i16rUserCountValue, i16UserCountValueExp, sizeof(INT16)*SLOT_MAX_COUNT );

		m_stExpPointValues._stRoundExpPointRate._i16rWinExp				= (INT16)( 1	* 10 );
		m_stExpPointValues._stRoundExpPointRate._i16rDrawExp			= (INT16)( 0.8	* 10 );
		m_stExpPointValues._stRoundExpPointRate._i16rLoseExp			= (INT16)( 0.7	* 10 );

		m_stExpPointValues._stExpPoint._i16rExpPerRound					= (INT16)( 1.5	* 10 );
		m_stExpPointValues._stExpPoint._i16rExpPerKill					= (INT16)( 3	* 10 );
		m_stExpPointValues._stExpPoint._i16rExpPerMission				= (INT16)( 2	* 10 );
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
	}


	m_eStartOption						= ROOM_START_COUNTDOWN;
	m_eRespawnOption					= ROOM_RESPAWN_NONE;
	m_eChattingOption					= ROOM_CHATTING_GHOST;
	m_eEndOption						= ROOM_END_ROUND;

	if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
	{
		m_pr32SubUserCount					= new REAL32 * [ SLOT_MAX_COUNT + 1 ];
		for( INT32 i = 1 ; i < SLOT_MAX_COUNT + 1 ; i++ )
		{
			m_pr32SubUserCount[i]			= new REAL32[ SLOT_MAX_COUNT ];
			i3mem::FillZero( m_pr32SubUserCount[i], sizeof(REAL32)*SLOT_MAX_COUNT );
		}

		REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.4501f, 0.4501f, 0.4501f, 0.5501f, 0.5501f, 0.6501f, 0.6501f, 0.7501f, 0.7501f, 0.8501f, 0.8501f, 0.901f, 0.901f, 0.9501f, 0.9501f, 1.01f};
		REAL32 pr32SubUserCountVS4[SLOT_MAX_COUNT]		= { 0.4001f, 0.4001f, 0.4001f, 0.6501f, 0.6501f, 0.9001f, 0.9001f, 1.001f,  };

		i3mem::Copy( m_pr32SubUserCount[ 16 ],		pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
		i3mem::Copy( m_pr32SubUserCount[ 8 ],		pr32SubUserCountVS4,	sizeof(REAL32)*SLOT_MAX_COUNT );
	}

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInAnnihilation::OnDestroy()
{
}

BOOL CRoomInAnnihilation::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->m_ui8CountDownSec		= COUNT_DOWN_TIME;
	m_pInfo->_i32StageType			= STAGE_MODE_ANNIHILATION;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= g_MissionRoundCountTable[ (m_pInfo->_InfoAdd._SubType & 0x0F) ];

	m_pInfo->m_eReadyOption			= ROOM_READY_BOTH_TEAM;
	m_pInfo->m_i8PersonAtLeast		= 1;
	m_pInfo->m_eJoinOption			= ROOM_JOIN_SEQUENCE;
	m_pInfo->m_ePreStartOption		= ROOM_PRESTART_ALL_WAIT;

	return TRUE;
}

void CRoomInAnnihilation::SetMatchStart()
{
	i3mem::FillZero( m_ui8AccMissionKill, sizeof(UINT8)*SLOT_MAX_COUNT );
}

void CRoomInAnnihilation::UserKillBounsV1( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )
{
	UINT16 ui16MissionKillCount;
	//UINT16 ui16MissionBonusExp;

	if ( 8 == g_pContextMain->GetMaxPerson( m_pInfo->_InfoBasic._StageID ))
	{
		ui16MissionKillCount	= m_ui8BonusKillTargetCount4VS4;
	}
	else
	{
		ui16MissionKillCount	= m_ui8BonusKillTargetCount;
	}

	if(m_pInfo->_aUserScoreRound[ i32SlotIdx ].m_KillCount == ui16MissionKillCount )
	{
		m_ui8AccMissionKill[i32SlotIdx]++;
		m_pInfo->_aui8AccMissionCount[i32SlotIdx]++;
	}
}

BOOL CRoomInAnnihilation::CheckRoundEnd( UINT32 ui32RTServer )
{
	//전적 올리기
	if( TEAM_DRAW == m_pInfo->_ui8RoundWinTeam )
	{// Draw 처리
		m_pInfo->_aui16TotalRoundCount[TEAM_RED]++;
		m_pInfo->_aui16TotalRoundCount[TEAM_BLUE]++;
	}
	else
	{
		m_pInfo->_aui16TotalRoundCount[ m_pInfo->_ui8RoundWinTeam ]++;
	}

	return TRUE; 
}

BOOL CRoomInAnnihilation::CheckMatchEnd()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] >= m_ui32RoundCount )	return TRUE;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInAnnihilation::GetWinTeam()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] > m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_RED_WIN;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] < m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

REAL32 CRoomInAnnihilation::GetSubPercentOfUnusualEnd( INT32 i32Win )
{
	REAL32 r32SubPercent = 1.f;	

	switch( m_ui32RoundCount )
	{
	case 3: 
		if(		m_pInfo->_InfoAdd._NowRoundCount > 2 ) r32SubPercent = 1.0f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 1 ) r32SubPercent = 0.1f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 0 ) r32SubPercent = 0.1f; 
		break; 
	case 5: 
		if(		m_pInfo->_InfoAdd._NowRoundCount > 4 ) r32SubPercent = 1.0f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 3 ) r32SubPercent = 0.3f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 2 ) r32SubPercent = 0.2f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 1 ) r32SubPercent = 0.1f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 0 ) r32SubPercent = 0.1f; 
		break; 
	case 7: 					
		if(		m_pInfo->_InfoAdd._NowRoundCount > 6 ) r32SubPercent = 1.0f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 5 ) r32SubPercent = 0.5f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 4 ) r32SubPercent = 0.4f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 3 ) r32SubPercent = 0.3f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 2 ) r32SubPercent = 0.2f; 									
		else if(m_pInfo->_InfoAdd._NowRoundCount > 1 ) r32SubPercent = 0.1f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 0 ) r32SubPercent = 0.1f; 
		break; 
	case 9: 
		if(		m_pInfo->_InfoAdd._NowRoundCount > 8 ) r32SubPercent = 1.0f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 7 ) r32SubPercent = 0.7f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 6 ) r32SubPercent = 0.6f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 5 ) r32SubPercent = 0.5f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 4 ) r32SubPercent = 0.4f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 3 ) r32SubPercent = 0.3f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 2 ) r32SubPercent = 0.2f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 1 ) r32SubPercent = 0.1f; 
		else if(m_pInfo->_InfoAdd._NowRoundCount > 0 ) r32SubPercent = 0.1f; 
		break; 
	default: break; 
	}	
	return r32SubPercent;
}

UINT16 CRoomInAnnihilation::GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp )
{
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	UINT16	ui16Exp = ui16AccExp;

	ui16Exp	= ui16Exp + (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount*pstKillExpPoint->_i16KillExp);

	if ( 8 == g_pContextMain->GetMaxPerson(m_pInfo->_InfoBasic._StageID))
		ui16Exp = ui16Exp + (m_ui8AccMissionKill[i32SlotIdx]*m_ui8BonusKillExp4VS4);
	else
		ui16Exp = ui16Exp + (m_ui8AccMissionKill[i32SlotIdx]*m_ui8BonusKillExp);

	return ui16Exp;
}

void CRoomInAnnihilation::GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer )
{
	pRoomNowScore->ScoreType	= ROOM_SCORE_ROUNDCOUNT_TYPE;
	pRoomNowScore->PlayTime		= 0;

	for(INT32 Loop = 0; Loop < TEAM_COUNT; Loop++)
	{
		pRoomNowScore->Score[Loop] = m_pInfo->_aui16TotalRoundCount[Loop];
	}
}

void CRoomInAnnihilation::AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->m_ui16TotalRoundCount.SetValue( m_pInfo->_aui16TotalRoundCount, 0, TEAM_COUNT );
	pPacket->m_ui16RoundStartUser	= m_pInfo->_ui16RoundStartUser;
	pPacket->m_ui8NowRoundCount		= m_pInfo->_InfoAdd._NowRoundCount;
}

STAGE_ID CRoomInAnnihilation::GetRandMap( STAGE_ID eStageID )
{
	return eStageID;
}