#include "pch.h"
#include "RoomInSuddendeath.h"

I3_CLASS_INSTANCE( CRoomInSuddenDeath, CRoomBase );

CRoomInSuddenDeath::CRoomInSuddenDeath()
{
}

CRoomInSuddenDeath::~CRoomInSuddenDeath()
{
	OnDestroy();
}

BOOL CRoomInSuddenDeath::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
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
#endif

	m_eStartOption						= ROOM_START_COUNTDOWN;
	m_ePreStartOption					= ROOM_PRESTART_ALL_WAIT;
	m_eRespawnOption					= ROOM_RESPAWN_NONE;
	m_eChattingOption					= ROOM_CHATTING_GHOST;
	m_eEndOption						= ROOM_END_ROUND;

#ifndef NEW_EXPPOINT
	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.4501f, 0.4501f, 0.4501f, 0.5501f, 0.5501f, 0.6501f, 0.6501f, 0.7501f, 0.7501f, 0.8501f, 0.8501f, 0.901f, 0.901f, 0.9501f, 0.9501f, 1.01f};
	REAL32 pr32SubUserCountVS4[SLOT4VS4_MAX_COUNT]	= { 0.4001f, 0.4001f, 0.4001f, 0.6501f, 0.6501f, 0.9001f, 0.9001f, 1.001f };

	i3mem::Copy( m_pr32SubUserCount, pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCountVS4, pr32SubUserCountVS4, sizeof(REAL32)*SLOT4VS4_MAX_COUNT );
#endif

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInSuddenDeath::OnDestroy()
{
}

BOOL CRoomInSuddenDeath::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_TYPE_SUDDEN_DEATH;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= g_MissionRoundCountTable[ (m_pInfo->_InfoAdd._SubType & 0x0F) ];

	return TRUE;
}

BOOL CRoomInSuddenDeath::CheckRoundEnd( UINT32 ui32ServerTime )
{
	BOOL bRoundEnd = FALSE; 

	m_pInfo->_InfoAdd._NowIngTime = ui32ServerTime - m_pInfo->_ui32BattleStartTime; 

	//시간 체크 
	UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	if( (m_pInfo->_ui32BattleStartTime + g_TeamTimeTable[ui8EndTimeIdx]) < ui32ServerTime )
	{
		m_pInfo->_ui8RoundWinTeam	= TEAM_DRAW;
		m_pInfo->_ui8MiEndType		= MISSION_END_TIMEOUT;
		bRoundEnd					= TRUE; 
	}

	//한쪽 편이 다 죽었는지 체크 
	if( FALSE == bRoundEnd  )
	{
		if( m_pInfo->_ui16RoundDeathUser == m_pInfo->_ui16RoundStartUser )
		{	// 모든 유저가 죽었을때
			m_pInfo->_ui8RoundWinTeam	= TEAM_DRAW; 
			m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH; 
			bRoundEnd					= TRUE; 
		}
		else
		{
			UINT16 ui16CheckBlueTeam	= BLUE_TEAM_MASK & m_pInfo->_ui16RoundStartUser;
			UINT16 ui16CheckRedTeam		= RED_TEAM_MASK & m_pInfo->_ui16RoundStartUser;

			if( (ui16CheckBlueTeam & m_pInfo->_ui16RoundDeathUser ) == ui16CheckBlueTeam )
			{
				m_pInfo->_ui8RoundWinTeam	= TEAM_RED; 
				m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH; 
				bRoundEnd					= TRUE; 
			}
			else if( (ui16CheckRedTeam & m_pInfo->_ui16RoundDeathUser ) == ui16CheckRedTeam )
			{
				m_pInfo->_ui8RoundWinTeam	= TEAM_BLUE;
				m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH;
				bRoundEnd					= TRUE;
			}
		}
	}

	// 아직 종료 시점이 되지 않았다.
	if( FALSE == bRoundEnd ) return FALSE;

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

BOOL CRoomInSuddenDeath::CheckMatchEnd()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] >= m_ui32RoundCount )	return TRUE;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInSuddenDeath::GetWinTeam()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] > m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_RED_WIN;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] < m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

#ifndef NEW_EXPPOINT
REAL32 CRoomInSuddenDeath::GetSubPercentOfUnusualEnd( INT32 i32Win )
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
#endif

void CRoomInSuddenDeath::AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->WriteData( m_pInfo->_aui16TotalRoundCount,		(sizeof(UINT16) * TEAM_COUNT));
	pPacket->WriteData( &m_pInfo->_ui16RoundStartUser,		sizeof(UINT16) );
}