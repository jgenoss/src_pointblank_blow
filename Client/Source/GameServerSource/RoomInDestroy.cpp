#include "pch.h"
#include "RoomInDestroy.h"

I3_CLASS_INSTANCE( CRoomInDestroy, CRoomBase );

CRoomInDestroy::CRoomInDestroy()
{
}

CRoomInDestroy::~CRoomInDestroy()
{
	OnDestroy();
}

BOOL CRoomInDestroy::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
#ifndef NEW_EXPPOINT
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stGetExpPoint._i16WinExp			= 100;
	m_stGetExpPoint._i16LoseExp			= 60;
	m_stGetExpPoint._i16DrawExp			= 0;
	m_stGetExpPoint._i16WinPoint		= 50;
	m_stGetExpPoint._i16LosePoint		= 20;
	m_stGetExpPoint._i16DrawPoint		= 0;

	m_stKillExpPoint._i16KillExp		= 2;
	m_stKillExpPoint._i16KillPoint		= 5;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

	// 보너스 경험치/포인트 (모드 전용)
	m_ui8BonusDestroyExp				= 50;
#else
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 30;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 20;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 1.5	* 10 );
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
	m_stExpPointValues._stExpPoint._i16rExpPerMission				= (INT16)( 1.5	* 10 );

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
	m_ui8BonusDestroyMax				= 7;
	m_ui8BonusDestroyScore				= 10;
	m_ui8BonusDamageMax					= 200;
	m_ui8rBonusDamageScore				= (UINT8)( 0.2 * 10 );
#endif

	m_eStartOption						= ROOM_START_DIRECT;
	m_ePreStartOption					= ROOM_PRESTART_ALL_WAIT;
	m_eRespawnOption					= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption					= ROOM_CHATTING_ALL;
	m_eEndOption						= ROOM_END_ROUND;

#ifndef NEW_EXPPOINT
	/*REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.4501f, 0.4501f, 0.4501f, 0.5501f, 0.5501f, 0.6501f, 0.6501f, 0.7501f, 0.7501f, 0.8501f, 0.8501f, 0.901f, 0.901f, 0.9501f, 0.9501f, 1.01f};
	REAL32 pr32SubUserCountVS4[SLOT4VS4_MAX_COUNT]	= { 0.4001f, 0.4001f, 0.4001f, 0.6501f, 0.6501f, 0.9001f, 0.9001f, 1.001f };*/
	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]				= { 0.201f, 0.301f, 0.401f, 0.501f, 0.601f, 0.701f, 0.801f, 0.901f, 1.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f};
	REAL32 pr32SubUserCountVS4[SLOT4VS4_MAX_COUNT]		= { 0.301f, 0.501f, 0.701f, 1.01f, 0.01f, 0.01f, 0.01f, 0.01f };

	i3mem::Copy( m_pr32SubUserCount, pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCountVS4, pr32SubUserCountVS4, sizeof(REAL32)*SLOT4VS4_MAX_COUNT );
#endif

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInDestroy::OnDestroy()
{
}

BOOL CRoomInDestroy::SetRoomOption( UINT32	ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_TYPE_DESTROY;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= g_MissionRoundCountTable[ (m_pInfo->_InfoAdd._SubType & 0x0F) ];
	
	switch( GET_STAGEORD( m_pInfo->_InfoBasic._StageID ) )
	{
	// 파괴 미션
	case STAGE_ORD_GIRAN		: 
	case STAGE_ORD_GIRAN2		:
		m_ui16MiObjectMaxHp_Red	 = 8000;
		m_ui16MiObjectMaxHp_Blue = 9000;
		break; 			
	case STAGE_ORD_HELISPOT		: 
		m_ui16MiObjectMaxHp_Red	 = 12000;
		m_ui16MiObjectMaxHp_Blue = 12000;
		break; 
	case STAGE_ORD_BREAKDOWN	: 
	default:
		m_ui16MiObjectMaxHp_Red	 = 6000;
		m_ui16MiObjectMaxHp_Blue = 6000;
		break;
	}

	return TRUE;
}

BOOL CRoomInDestroy::UserGiveUpBattle( INT32 i32SlotIdx )
{
	m_MiDestructionInfo._nUserDamage[ i32SlotIdx ]	= 0;	//파괴미션
	return TRUE;
}

void CRoomInDestroy::SetMatchStart()
{
	i3mem::FillZero( m_ui8AccMissionDestroy, sizeof(m_ui8AccMissionDestroy) );
	i3mem::FillZero( m_ui16AccMissionDamageScore, sizeof(m_ui16AccMissionDamageScore) );
}

void CRoomInDestroy::SetRoundStart()
{
	m_MiDestructionInfo.Reset();
	m_MiDestructionInfo._nRedObjectHP  = m_ui16MiObjectMaxHp_Red;
	m_MiDestructionInfo._nBlueObjectHP = m_ui16MiObjectMaxHp_Blue; 
}

BOOL CRoomInDestroy::CheckRoundEnd( UINT32 ui32ServerTime )
{
	BOOL bRoundEnd = FALSE; 

	m_pInfo->_InfoAdd._NowIngTime = ui32ServerTime - m_pInfo->_ui32BattleStartTime; 

	//오브젝트 파괴 체크 
	if( 0 == m_MiDestructionInfo._nRedObjectHP )
	{
		m_pInfo->_ui8MiEndType		= MISSION_END_GENERATOR_DESTROY;
		m_pInfo->_ui8RoundWinTeam	= TEAM_BLUE_WIN;					//블루팀 승리 

		//블루팀에게 폭파 경험치를 준다. 
		for( INT32 i = TEAM_BLUE ; i < SLOT_MAX_COUNT ; i += 2 )
		{
			if( 0 < m_MiDestructionInfo._nUserDamage[i] )	m_ui8AccMissionDestroy[i]++;
		}

		bRoundEnd	 = TRUE;
	}
	else if( 0 == m_MiDestructionInfo._nBlueObjectHP )
	{
		m_pInfo->_ui8MiEndType		= MISSION_END_GENERATOR_DESTROY; 
		m_pInfo->_ui8RoundWinTeam	= TEAM_RED_WIN;	//레드팀 승리 

		//레드팀에게 폭파 경험치를 준다. 
		for( INT32 i = TEAM_RED ; i < SLOT_MAX_COUNT ; i += 2 )
		{
			if( 0 < m_MiDestructionInfo._nUserDamage[i] )	m_ui8AccMissionDestroy[i]++;
		}
		bRoundEnd	 = TRUE; 
	}

	//시간 체크 
	UINT8	ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	if( (FALSE == bRoundEnd) && ((m_pInfo->_ui32BattleStartTime + g_TeamTimeTable[ui8EndTimeIdx]) < ui32ServerTime) )
	{	//TTP버그로 긴급수정
		REAL32 r32Red =  (REAL32)((REAL32)m_MiDestructionInfo._nRedObjectHP / (REAL32)m_ui16MiObjectMaxHp_Red);	
		REAL32 r32Blue = (REAL32)((REAL32)m_MiDestructionInfo._nBlueObjectHP / (REAL32)m_ui16MiObjectMaxHp_Blue);	

		m_pInfo->_ui8MiEndType = MISSION_END_TIMEOUT; 
		if( r32Red > r32Blue )
		{
			m_pInfo->_ui8RoundWinTeam  = TEAM_RED_WIN;//레드팀 승리 
		}
		else if( r32Red < r32Blue )
		{
			m_pInfo->_ui8RoundWinTeam  = TEAM_BLUE_WIN;//블루팀 승리
		}
		else
		{
			m_pInfo->_ui8RoundWinTeam  = TEAM_DRAW;//무승부
		}
		bRoundEnd = TRUE;
	}

	// 아직 종료 시점이 되지 않았다.
	if( FALSE == bRoundEnd ) return FALSE;

	//전적 올리기
	//파괴 오브젝트 포인트 올리기
	
	//// 파괴 미션 오브젝트 추가 점수 변경
	REAL32 ar32ScoreRate[SLOT_MAX_COUNT + 1] = { 
		0.0f, 0.0f,	// 0,1명	- 게임진행불가
		0.3f, 0.3f,	// 2,3명	- 30% 획득
		0.4f, 0.4f,	// 4,5명	- 40% 획득
		0.5f, 0.5f,	// 6,7명	- 50% 획득
		0.6f, 0.6f,	// 8,9명	- 60% 획득
		0.7f, 0.7f,	// 10,11명	- 70% 획득
		0.8f, 0.8f,	// 12,13명	- 80% 획득
		0.9f, 0.9f,	// 14,15명	- 90% 획득
		1.0f		// 16명		- 100% 획득
	};
	// 방 인원에 따라 최종 경험치 획득비율을 결정합니다.
	INT32 i32BattleUser = 0;
	for( INT32 i = 0; i < SLOT_MAX_COUNT ; i++ )
	{
		if ( SLOT_STATE_BATTLE == m_pInfo->_SlotInfo[i]._State ) i32BattleUser++;
	}
	REAL32 ScoreRate = ar32ScoreRate[ i32BattleUser ];

	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;

		if( (i % TEAM_COUNT) == TEAM_RED )	m_ui16AccMissionDamageScore[i] += (INT16)((REAL32)m_MiDestructionInfo._nUserDamage[i] * ScoreRate ); //RED
		else								m_ui16AccMissionDamageScore[i] += (INT16)((REAL32)m_MiDestructionInfo._nUserDamage[i] * ScoreRate ); 	//BLUE
	}

	if( TEAM_DRAW != m_pInfo->_ui8RoundWinTeam )
	{	// 무승부가 아닐때
		m_pInfo->_aui16TotalRoundCount[ m_pInfo->_ui8RoundWinTeam ]++;
	}

	//해킹 판단하기 
	UINT16 ui16RedTotalDamage  = 0; 
	UINT16 ui16BlueTotalDamage = 0; 

	for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;

		if( (i % TEAM_COUNT) == TEAM_RED)	ui16RedTotalDamage  += m_MiDestructionInfo._nUserDamage[i];//RED
		else								ui16BlueTotalDamage += m_MiDestructionInfo._nUserDamage[i];//BLUE
	}

	// 20% 여유를 둔다.
	if( ui16BlueTotalDamage > (m_ui16MiObjectMaxHp_Red*1.2f) || ui16RedTotalDamage > (m_ui16MiObjectMaxHp_Blue*1.2f) )
	{
		for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;

			m_ui8AccMissionDestroy[i] = 0;
			m_ui16AccMissionDamageScore[i] = 0;
			m_pInfo->_aM_UserScore[i].m_KillCount = 0;
		}
		g_pModuleLogFile->Write_M_Log( MAIN_THREAD_IDX, NULL, "DESTROY MISSION ERROR" ); 
	}

	return TRUE;
}

BOOL CRoomInDestroy::CheckMatchEnd()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] >= m_ui32RoundCount )	return TRUE;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInDestroy::GetWinTeam()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] > m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_RED_WIN;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] < m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

#ifndef NEW_EXPPOINT
REAL32 CRoomInDestroy::GetSubPercentOfUnusualEnd( INT32 i32Win )
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

UINT16 CRoomInDestroy::GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp )
{
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	UINT16	ui16Exp = ui16AccExp;

	ui16Exp	= ui16Exp + (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount*pstKillExpPoint->_i16KillExp);

	// 파괴 임무 성공에 대한 보너스
	ui16Exp = ui16Exp + (m_ui8AccMissionDestroy[i32SlotIdx] * m_ui8BonusDestroyExp);

	// 파괴 임무 수행에 대한 보너스
	UINT16 ui16ObjectHP;
	if( (i32SlotIdx % TEAM_COUNT) == TEAM_RED )	ui16ObjectHP = m_ui16MiObjectMaxHp_Blue;	//RED
	else										ui16ObjectHP = m_ui16MiObjectMaxHp_Red; 	//BLUE
	ui16Exp = ui16Exp + (UINT16)((m_ui16AccMissionDamageScore[i32SlotIdx] * 50) / ui16ObjectHP);

	return ui16Exp;
}
#else
INT16 CRoomInDestroy::GetMissionExp( INT32 i32SlotIdx )
{
	INT16 i16MissionExp = CRoomBase::GetMissionExp(i32SlotIdx);

	i16MissionExp += MIN( m_ui8BonusDestroyMax, m_ui8AccMissionDestroy[i32SlotIdx] ) * m_ui8BonusDestroyScore;

	i16MissionExp += ( MIN( m_ui8BonusDamageMax, m_ui16AccMissionDamageScore[i32SlotIdx] ) * m_ui8rBonusDamageScore ) / 10;

	return i16MissionExp;
}
#endif

void CRoomInDestroy::AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->WriteData( m_pInfo->_aui16TotalRoundCount,		sizeof(m_pInfo->_aui16TotalRoundCount) );
	pPacket->WriteData( &m_MiDestructionInfo,				sizeof(m_MiDestructionInfo));
}

BOOL CRoomInDestroy::MissionObjectDestroy(DESTRUCTION_INFO* pInfo )
{
	pInfo->CopyTo( &m_MiDestructionInfo );	//카피

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_GENERATOR_INFO_ACK );
	SendPacket.WriteData( &m_MiDestructionInfo,	sizeof(DESTRUCTION_INFO) );
	_SendPacketBattlePlayer( &SendPacket );

	return TRUE;
}