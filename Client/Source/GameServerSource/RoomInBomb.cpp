#include "pch.h"
#include "RoomInBomb.h"

I3_CLASS_INSTANCE( CRoomInBomb, CRoomBase );

CRoomInBomb::CRoomInBomb()
{
	m_ui16BombTime			= 42;		//Temp
}

CRoomInBomb::~CRoomInBomb()
{
	OnDestroy();
}

BOOL CRoomInBomb::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
#ifndef NEW_EXPPOINT
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stGetExpPoint._i16WinExp			= 160;
	m_stGetExpPoint._i16LoseExp			= 80;
	m_stGetExpPoint._i16DrawExp			= 0;
	m_stGetExpPoint._i16WinPoint		= 120;
	m_stGetExpPoint._i16LosePoint		= 60;
	m_stGetExpPoint._i16DrawPoint		= 0;

	m_stKillExpPoint._i16KillExp		= 20;
	m_stKillExpPoint._i16KillPoint		= 20;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

	// 보너스 경험치/포인트 (모드 전용)
	m_ui8BonusBombInstall				= 40;
	m_ui8BonusBombUninstall				= 40;
#else
	m_stExpPointValues._stRoundExp._i16Round						= 30;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 6;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 2	* 10 );
	m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 1.5	* 10 );
	m_stExpPointValues._stRoundExp._i16rHighKillFactor				= (INT16)( 1	* 10 );

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

	// 보너스 경험치/포인트 (모드 전용)
	m_ui8BonusBombMax					= 5;
	m_ui8BonusBombExp					= 5;
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

void CRoomInBomb::OnDestroy()
{
}

BOOL CRoomInBomb::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_TYPE_BOMB;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= g_MissionRoundCountTable[ (m_pInfo->_InfoAdd._SubType & 0x0F) ];

	return TRUE;
}

void CRoomInBomb::SetMatchStart()
{
	i3mem::FillZero( m_aui8AccMissionBombInstallCount, sizeof(m_aui8AccMissionBombInstallCount) );
	i3mem::FillZero( m_aui8AccMissionBombUninstallCount, sizeof(m_aui8AccMissionBombUninstallCount) );
}

void CRoomInBomb::SetRoundStart()
{
	m_i32BombUnInstallSlotIdx	= SERVER_VALUE_UNKNOWN;
	m_i32BombInstallSlotIdx		= SERVER_VALUE_UNKNOWN;
}

bool CRoomInBomb::RoundStart( UINT32 ui32RTBattle )
{
#ifdef USE_BOMB_AUTO_TEAM_CHANGE
	if ( GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE( m_pInfo->_InfoBasic._InfoFlag ) )
	{
		if ( m_ui32RoundCount <= m_pInfo->_InfoAdd._NowRoundCount )
			return true;
	}
#endif
	return false;
}

BOOL CRoomInBomb::CheckRoundEnd( UINT32 ui32ServerTime )
{
	BOOL bRoundEnd = FALSE; 

	m_pInfo->_InfoAdd._NowIngTime	= ui32ServerTime - m_pInfo->_ui32BattleStartTime; 
	UINT32 ui32BombNowIngTime		= ui32ServerTime - m_ui32BombInstallTime;

	//미션이 성공 했는지 체크 
	if( SERVER_VALUE_UNKNOWN != m_i32BombUnInstallSlotIdx ) 
	{	// 폭탄 해제가 완료 되었는지 확인
		m_pInfo->_ui8RoundWinTeam		= TEAM_BLUE_WIN;
		m_pInfo->_ui8MiEndType			= MISSION_END_UNINSTALL; 
		bRoundEnd						= TRUE;
	}
	else if( SERVER_VALUE_UNKNOWN != m_i32BombInstallSlotIdx )
	{
		if( ui32BombNowIngTime > m_ui16BombTime )
		{
			m_pInfo->_ui8RoundWinTeam	= TEAM_RED_WIN;
			m_pInfo->_ui8MiEndType		= MISSION_END_BOMBFIRE; 
			bRoundEnd					= TRUE;	
		}
	}

	UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4;
	//시간 체크
	if( FALSE == bRoundEnd  )
	{
		if( m_pInfo->_InfoAdd._NowIngTime > g_TeamTimeTable[ui8EndTimeIdx] )
		{
			m_pInfo->_ui8MiEndType			= MISSION_END_TIMEOUT;
			m_pInfo->_ui8RoundWinTeam		= TEAM_BLUE_WIN;
			bRoundEnd						= TRUE;
		}
	}

	//한쪽 편이 다 죽었는지 체크
	if( FALSE == bRoundEnd  )
	{
		UINT16 ui16CheckBlueTeam	= BLUE_TEAM_MASK & m_pInfo->_ui16RoundStartUser;
		UINT16 ui16CheckRedTeam		= RED_TEAM_MASK & m_pInfo->_ui16RoundStartUser;

		if( m_pInfo->_ui16RoundDeathUser == m_pInfo->_ui16RoundStartUser )
		{	// 양쪽이 다 죽은 상태에서
			if( SERVER_VALUE_UNKNOWN == m_i32BombInstallSlotIdx )
			{	// 폭탄 설치가 되어있지 않으면 블루 승
				m_pInfo->_ui8RoundWinTeam	= TEAM_BLUE_WIN;
				m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH;
				bRoundEnd					= TRUE;
			}
			//현재 남은 시간 > 폭탄 터질 시간
			else if( (g_TeamTimeTable[ui8EndTimeIdx] - m_pInfo->_InfoAdd._NowIngTime ) > ( m_ui16BombTime - ui32BombNowIngTime ) )
			{	// 폭탄이 터질 시간이 남아있으면 레드 승
				m_pInfo->_ui8RoundWinTeam	= TEAM_RED_WIN;
				m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH;
				bRoundEnd					= TRUE;
			}
			else
			{	// 아니면 블루 승
				m_pInfo->_ui8RoundWinTeam	= TEAM_BLUE_WIN;
				m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH;
				bRoundEnd					= TRUE;
			}
		}
		else if( ( ui16CheckBlueTeam & m_pInfo->_ui16RoundDeathUser ) == ui16CheckBlueTeam )
		{
			m_pInfo->_ui8RoundWinTeam	= TEAM_RED_WIN;
			m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH;
			bRoundEnd					= TRUE;
		}
		else if( ( ui16CheckRedTeam & m_pInfo->_ui16RoundDeathUser ) == ui16CheckRedTeam )
		{
			if( SERVER_VALUE_UNKNOWN == m_i32BombInstallSlotIdx )
			{	//폭탄이 설치 되어 있지 않으면 게임끝.
				m_pInfo->_ui8RoundWinTeam	= TEAM_BLUE_WIN;
				m_pInfo->_ui8MiEndType		= MISSION_END_ALLDEATH;
				bRoundEnd					= TRUE;
			}
		}
	}

	// 아직 종료 시점이 되지 않았다.
	if( FALSE == bRoundEnd ) return FALSE;

	//전적 올리기
	m_pInfo->_aui16TotalRoundCount[ m_pInfo->_ui8RoundWinTeam ]++;

	return TRUE;
}

BOOL CRoomInBomb::CheckMatchEnd()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] >= m_ui32RoundCount )	return TRUE;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInBomb::GetWinTeam()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] > m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_RED_WIN;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] < m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

#ifdef NEW_EXPPOINT
INT16 CRoomInBomb::GetMissionExp( INT32 i32SlotIdx )
{
	INT16 i16MissionExp = CRoomBase::GetMissionExp(i32SlotIdx);
	INT8 i8MissionBomb = m_aui8AccMissionBombInstallCount[i32SlotIdx] + m_aui8AccMissionBombUninstallCount[i32SlotIdx];

	i16MissionExp += MIN( m_ui8BonusBombMax, i8MissionBomb ) * m_ui8BonusBombExp;

	return i16MissionExp;
}
#endif

#ifndef NEW_EXPPOINT
REAL32 CRoomInBomb::GetSubPercentOfUnusualEnd( INT32 i32Win )
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

#ifndef NEW_EXPPOINT
UINT16 CRoomInBomb::GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp )
{
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	//UINT16	ui16Exp = ui16AccExp;

	//ui16Exp	= ui16Exp + (m_pInfo->_pM_UserScore[i32SlotIdx].m_KillCount*pstKillExpPoint->_i16KillExp);

	UINT16	ui16Exp = CRoomBase::GetMatchAccExp( i32SlotIdx, ui16AccExp );

	ui16Exp = ui16Exp + (m_aui8AccMissionBombInstallCount[i32SlotIdx]*m_ui8BonusBombInstall);
	ui16Exp = ui16Exp + (m_aui8AccMissionBombUninstallCount[i32SlotIdx]*m_ui8BonusBombUninstall);

	return ui16Exp;
}

UINT16 CRoomInBomb::GetMatchAccPoint( INT32 i32SlotIdx, UINT16 ui16AccPoint )
{
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	//UINT16	ui16Point = ui16AccPoint;

	//ui16Point = ui16Point + (m_pInfo->_pM_UserScore[i32SlotIdx].m_KillCount*pstKillExpPoint->_i16KillPoint);

	UINT16	ui16Point = CRoomBase::GetMatchAccPoint( i32SlotIdx, ui16AccPoint );

	ui16Point = ui16Point + (m_aui8AccMissionBombInstallCount[i32SlotIdx]*m_ui8BonusBombInstall);
	ui16Point = ui16Point + (m_aui8AccMissionBombUninstallCount[i32SlotIdx]*m_ui8BonusBombUninstall);

	return ui16Point;
}
#endif

void CRoomInBomb::AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->WriteData( m_pInfo->_aui16TotalRoundCount,		(sizeof(UINT16) * TEAM_COUNT));
	pPacket->WriteData( &m_pInfo->_ui16RoundStartUser,		sizeof(UINT16) );
}

BOOL CRoomInBomb::MissionBombInstall( INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos, UINT32 ui32RTBattle )
{
	if( SERVER_VALUE_UNKNOWN	!= m_i32BombInstallSlotIdx )return FALSE;

	m_i32BombInstallSlotIdx		= i32SlotIdx; 
	m_ui32BombInstallTime		= 
#ifdef USE_GM_PAUSE
									ui32RTBattle;
#else
									g_pContextMain->GetServerTime(); 
#endif

	m_pInfo->_aui8AccMissionCount[m_i32BombInstallSlotIdx]++;
	m_aui8AccMissionBombInstallCount[m_i32BombInstallSlotIdx]++;

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_ACK ); 
	SendPacket.WriteData( &m_i32BombInstallSlotIdx,	sizeof(INT32) );
	SendPacket.WriteData( &ui8BombPosition,			sizeof(UINT8) );
	SendPacket.WriteData( &m_ui16BombTime,			sizeof(UINT16));
	SendPacket.WriteData( pr32BombVecPos,			sizeof(REAL32) * 3);	// 2009.06.22 정순구 C4 설치 위치 추가
	_SendPacketBattlePlayer( &SendPacket );

	return TRUE;
}

BOOL CRoomInBomb::MissionBombUnInstall( INT32 i32SlotIdx )
{
	if( SERVER_VALUE_UNKNOWN	== m_i32BombInstallSlotIdx )				return FALSE;
	if( SERVER_VALUE_UNKNOWN	!= m_i32BombUnInstallSlotIdx )				return FALSE;

	//m_i32BombInstallSlotIdx		= SERVER_VALUE_UNKNOWN; 
	m_i32BombUnInstallSlotIdx	= i32SlotIdx;

	m_pInfo->_aui8AccMissionCount[m_i32BombUnInstallSlotIdx]++;
	m_aui8AccMissionBombUninstallCount[m_i32BombUnInstallSlotIdx]++;

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_ACK ); 
	SendPacket.WriteData(&m_i32BombUnInstallSlotIdx,	sizeof(INT32) );
	_SendPacketBattlePlayer( &SendPacket );

	return TRUE;
}