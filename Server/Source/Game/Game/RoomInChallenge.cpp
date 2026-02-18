#include "pch.h"
#include "RoomInChallenge.h"

I3_CLASS_INSTANCE( CRoomInChallenge, CRoomBase );

CRoomInChallenge::CRoomInChallenge()
{
}

CRoomInChallenge::~CRoomInChallenge()
{
	OnDestroy();
}

BOOL CRoomInChallenge::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
	m_stGetExpPointV1._i16WinExp		= 80;
	m_stGetExpPointV1._i16LoseExp		= 20;
	m_stGetExpPointV1._i16DrawExp		= 50;
	m_stGetExpPointV1._i16WinPoint		= 80;
	m_stGetExpPointV1._i16LosePoint		= 20;
	m_stGetExpPointV1._i16DrawPoint		= 50;

	m_stKillExpPoint._i16KillExp		= 10;
	m_stKillExpPoint._i16KillPoint		= 5;
	m_stKillExpPoint._i16AssistExp		= 2;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 170;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 80;
	m_stExpPointValues._stRoundExp._i16AssistPerRound1Round			= 36;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 2	* 10 );
	m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 1.5	* 10 );
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
	m_stExpPointValues._stExpPoint._i16rExpPerKill					= (INT16)( 2	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerMission				= (INT16)( 1	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerAssist				= (INT16)( 2	* 10 );

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


	m_eStartOption						= ROOM_START_DIRECT;
	m_eRespawnOption					= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption					= ROOM_CHATTING_ALL;
	m_eEndOption						= ROOM_END_MOD;

	m_pr32SubUserCount					= new REAL32*[SLOT_MAX_COUNT + 1];
	for( INT32 i = 1 ; i < SLOT_MAX_COUNT + 1; i++ )
	{
		m_pr32SubUserCount[i]			= new REAL32[ SLOT_MAX_COUNT ];
		i3mem::FillZero( m_pr32SubUserCount[i], sizeof(REAL32)*SLOT_MAX_COUNT );
	}

	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.4501f, 0.4501f, 0.4501f, 0.5501f, 0.5501f, 0.6501f, 0.6501f, 0.7501f, 0.7501f, 0.8501f, 0.8501f, 0.901f, 0.901f, 0.9501f, 0.9501f, 1.01f};
	REAL32 pr32SubUserCountVS4[SLOT_MAX_COUNT]		= { 0.4001f, 0.4001f, 0.4001f, 0.6501f, 0.6501f, 0.9001f, 0.9001f, 1.001f, };

	i3mem::Copy( m_pr32SubUserCount[ 16 ],		pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCount[ 8 ],		pr32SubUserCountVS4,	sizeof(REAL32)*SLOT_MAX_COUNT );

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInChallenge::OnDestroy()
{
}

BOOL CRoomInChallenge::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_MODE_DEATHMATCH;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= 1;

	m_pInfo->m_eReadyOption			= ROOM_READY_ONE_TEAM;
	m_pInfo->m_i8PersonAtLeast		= 1;
	m_pInfo->m_eJoinOption			= ROOM_JOIN_SEQUENCE_USER_FOLLOW;
	m_pInfo->m_ePreStartOption		= ROOM_PRESTART_DIRECT;

	return TRUE;
}

BOOL CRoomInChallenge::CheckRoundEnd( UINT32 ui32RTServer )
{
	return TRUE;
}

BOOL CRoomInChallenge::CheckMatchEnd()
{
	if( (UINT32)(m_pInfo->_InfoAdd._NowRoundCount) >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInChallenge::GetWinTeam()
{
	if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount > m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	return TEAM_RED_WIN;
	if( m_pInfo->_aTeamScoreTotal[TEAM_RED].m_KillCount < m_pInfo->_aTeamScoreTotal[TEAM_BLUE].m_KillCount )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

void CRoomInChallenge::GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint )
{
	CRoomBase::GetBasicExpPoint( i32Win, pstGetExpPoint );

	// 비겼을 경우 킬 카운트가 없으면 Draw 0 처리
	if( TEAM_DRAW != i32Win )							return;
	if( 0 != m_pInfo->_aTeamScoreTotal[0].m_KillCount )	return;
	if( 0 != m_pInfo->_aTeamScoreTotal[1].m_KillCount )	return;

	pstGetExpPoint->_i16DrawExp		= 0;
	pstGetExpPoint->_i16DrawPoint	= 0;
}

REAL32 CRoomInChallenge::GetSubPercentOfUnusualEnd( INT32 i32Win )
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

UINT16 CRoomInChallenge::GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle )
{
	return (UINT16)(ui16AccExp * r32AdjustPercent);
}

BOOL CRoomInChallenge::CheckVoteTime( UINT32 ui32ServerTime )
{
	if ( (UINT32)(m_pInfo->_InfoAdd._NowRoundCount) + 1 == m_ui32RoundCount )
	{
		UINT8 ui8EndTimeIdx	= (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 

		if( g_TeamTimeTable[ui8EndTimeIdx] + m_pInfo->_ui32RTRoundStart < ui32ServerTime + VOTE_FINISH_TIME )
		{
			return FALSE;
		}
	}

	return TRUE;
}


void CRoomInChallenge::AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->m_ui8NowRoundCount		= 1;
}