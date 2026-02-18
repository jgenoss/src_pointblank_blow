#include "pch.h"
#include "RoomInDefence.h"

I3_CLASS_INSTANCE( CRoomInDefence, CRoomBase );

CRoomInDefence::CRoomInDefence()
{
}

CRoomInDefence::~CRoomInDefence()
{
	OnDestroy();
}

BOOL CRoomInDefence::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
	// 보너스 경험치/포인트 (공통 설정사항)
	m_stGetExpPointV1._i16WinExp		= 100;
	m_stGetExpPointV1._i16LoseExp		= 50;
	m_stGetExpPointV1._i16DrawExp		= 0;
	m_stGetExpPointV1._i16WinPoint		= 50;
	m_stGetExpPointV1._i16LosePoint		= 20;
	m_stGetExpPointV1._i16DrawPoint		= 0;

	m_stKillExpPoint._i16KillExp		= 2;
	m_stKillExpPoint._i16KillPoint		= 5;
	m_stKillExpPoint._i16AssistExp		= 2;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 30;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 20;
	m_stExpPointValues._stRoundExp._i16AssistPerRound1Round			= 4;
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
	m_stExpPointValues._stExpPoint._i16rExpPerKill					= (INT16)( 2	* 10 );
	m_stExpPointValues._stExpPoint._i16rExpPerMission				= (INT16)( 1.5	* 10 );
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

	// 보너스 경험치/포인트 (모드 전용)
	m_ui8BonusDamageMax								= 200;
	m_ui8rBonusDamageScore							= (UINT8)( 0.1 * 10 );
	m_ui8BonusDefenceMax							= 5;
	m_ui8BonusDefenceScore							= 10;
	m_ui8BonusHPMax									= 200;
	m_ui8rBonusHPScore								= (UINT8)( 0.5 * 10 );


	m_eStartOption						= ROOM_START_DIRECT;
	m_eRespawnOption					= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption					= ROOM_CHATTING_ALL;
	m_eEndOption						= ROOM_END_ROUND;


	REAL32 pr32SubUserCountTwo[SLOT_MAX_COUNT]				= { 0.101f, 0.301f, 0.301f, 0.501f, 0.501f, 0.701f, 0.701f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f  };
	REAL32 pr32SubUserCountTwoVS4[SLOT4VS4_MAX_COUNT]		= { 0.301f, 0.501f, 0.701f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f };
	i3mem::Copy( m_pr32SubUserCountTwo,		pr32SubUserCountTwo,	sizeof(REAL32)*SLOT_MAX_COUNT );		// 
	i3mem::Copy( m_pr32SubUserCountTwoVS4,	pr32SubUserCountTwoVS4,	sizeof(REAL32)*SLOT4VS4_MAX_COUNT );	// 

	m_pr32SubUserCount					= new REAL32*[SLOT_MAX_COUNT + 1];
	for( INT32 i = 1 ; i < SLOT_MAX_COUNT + 1; i++ )
	{
		m_pr32SubUserCount[i]			= new REAL32[ SLOT_MAX_COUNT ];
		i3mem::FillZero( m_pr32SubUserCount[i], sizeof(REAL32)*SLOT_MAX_COUNT );
	}

	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 0.301f, 0.501f, 0.501f, 0.701f, 0.701f, 0.901f, 0.901f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f  };
	REAL32 pr32SubUserCountVS4[SLOT_MAX_COUNT]		= { 0.501f, 0.701f, 0.901f, 1.01f, 0.0f, 0.0f, 0.0f, 0.0f, };

	i3mem::Copy( m_pr32SubUserCount[16],		pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCount[8],			pr32SubUserCountVS4,	sizeof(REAL32)*SLOT_MAX_COUNT );


	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInDefence::OnDestroy()
{
}

BOOL CRoomInDefence::CheckSubType( UINT8 ui8SubType )
{
	// 모드별로 플레이 가능한 승리조건인지, 제한시간이 맞는지 확인
	UINT8 ui8RoundEndTimeIdx = (ui8SubType & 0xF0) >> 4;

	if( g_TeamTimeTable[ui8RoundEndTimeIdx] != 180 ) return FALSE;

//#ifndef _DEBUG
	if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
	{
		UINT8 ui8EndRoundCount = (ui8SubType & 0x0F);

		if( ui8EndRoundCount >= (sizeof(g_MissionRoundCountTable)/sizeof(UINT32))-1 ) return FALSE;
	}
//#endif

	return TRUE;
}

BOOL CRoomInDefence::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_MODE_DEFENCE;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= g_MissionRoundCountTable[ (m_pInfo->_InfoAdd._SubType & 0x0F) ];

	switch (StageID::GetStageUID(m_pInfo->_InfoBasic._StageID))
	{
	case STAGE_UID_BLACKPANTHER:
		m_pui16MiObjectMaxHp[MISSION_FIRST_DEFENCE_OBJECT] = 6000;
		m_pui16MiObjectMaxHp[MISSION_SECOND_DEFENCE_OBJECT] = 9000;
		break;
	}

	m_pInfo->m_eReadyOption			= ROOM_READY_BOTH_TEAM;
	m_pInfo->m_i8PersonAtLeast		= 1;
	m_pInfo->m_eJoinOption			= ROOM_JOIN_SEQUENCE;
	m_pInfo->m_ePreStartOption		= ROOM_PRESTART_ALL_WAIT;

	return TRUE;
}

BOOL CRoomInDefence::UserGiveUpBattle( INT32 i32SlotIdx )
{
	m_MiDefenceInfo._nUserDamage[MISSION_FIRST_DEFENCE_OBJECT][i32SlotIdx]	= 0;	 //방어미션
	m_MiDefenceInfo._nUserDamage[MISSION_SECOND_DEFENCE_OBJECT][i32SlotIdx]	= 0;	 //방어미션
	return TRUE;
}

void CRoomInDefence::UserKillBounsV1( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )
{
	// 오브젝트 공격중인 유저를 죽였을때
	if( CHARA_DEATH_ENEMY_STATE_DEFENCE == ui32DeathEnemyState )
		m_ui8AccMissionDefence[ i32SlotIdx ]++;
}

UINT16 CRoomInDefence::UserKillBounsV2( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )
{
	UINT16 ui16GainPoint = CRoomBase::UserKillBounsV2(i32SlotIdx, ui32DeathType, ui32DeathMyState, ui32DeathEnemyState, ui32RTBattle);

	// 오브젝트 공격중인 유저를 죽였을때
	if( CHARA_DEATH_ENEMY_STATE_DEFENCE == ui32DeathEnemyState )
		m_ui8AccMissionDefence[ i32SlotIdx ]++;

	return ui16GainPoint;
}

void CRoomInDefence::SetMatchStart()
{
	i3mem::FillZero( m_ui8AccMissionDefence, sizeof(UINT8)*SLOT_MAX_COUNT );
	i3mem::FillZero( m_ui32AccMissionUserDamage, sizeof(UINT32)*MAX_OBJECT_COUNT*SLOT_MAX_COUNT );

	i3mem::FillZero( m_ui32AccMissionDefenceScore, sizeof(UINT32)*MAX_OBJECT_COUNT*SLOT_MAX_COUNT );

	i3mem::FillZero( m_ui16AccMissionDefenceRemain, sizeof(UINT16)*MAX_OBJECT_COUNT );
}

void CRoomInDefence::SetRoundStart()
{
	m_MiDefenceInfo.Reset();
	i3mem::Copy( m_MiDefenceInfo._nObjectHP, m_pui16MiObjectMaxHp, sizeof(UINT16)*MAX_OBJECT_COUNT  );
}

BOOL CRoomInDefence::CheckRoundEnd( UINT32 ui32RTServer )
{
	//전적 올리기
	//파괴 오브젝트 포인트 올리기		
	//해킹 판단하기 
	UINT16 ui16FirstTotalDamage			= 0; 
	UINT16 ui16SecondTotalDamage		= 0;
	UINT16 ui16CurrentBattleUserBlue	= 0;	// 블루팀의 팀킬수를 인원수만큼 늘려서 블루유저의 킬수가 0이 되지 않도록 하기위해 사용함.
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( i % TEAM_COUNT == TEAM_RED)		
		{
			ui16FirstTotalDamage	+= m_MiDefenceInfo._nUserDamage[MISSION_FIRST_DEFENCE_OBJECT][i];//RED
			ui16SecondTotalDamage	+= m_MiDefenceInfo._nUserDamage[MISSION_SECOND_DEFENCE_OBJECT][i];//RED
		}
		else
		{// 블루의 유저가 배틀에 있는 유저가 아니면 해당 유저가 팀킬수에 누적시킨 킬수를 제거한다.
			if( SLOT_STATE_BATTLE != m_pInfo->_aSlotInfo[i]._State )
			{
				m_pInfo->_aTeamScoreRound[TEAM_BLUE].m_KillCount -= m_pInfo->_aUserScoreRound[i].m_KillCount;
			}
			else
			{
				ui16CurrentBattleUserBlue++;
			}
		}
	}

	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_aSlotInfo[i]._State ) continue;
		if( i % TEAM_COUNT == TEAM_RED)
		{	
			m_ui32AccMissionUserDamage[MISSION_FIRST_DEFENCE_OBJECT][i]  += m_MiDefenceInfo._nUserDamage[MISSION_FIRST_DEFENCE_OBJECT][i];
			m_ui32AccMissionUserDamage[MISSION_SECOND_DEFENCE_OBJECT][i] += m_MiDefenceInfo._nUserDamage[MISSION_SECOND_DEFENCE_OBJECT][i];
		}


		else
		{
			if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
			{	//if( 0 < m_pInfo->_pTeamScoreRound[i].m_KillCount )
				{
					if( SLOT_STATE_BATTLE != m_pInfo->_aSlotInfo[i]._State ) continue;

					m_ui32AccMissionDefenceScore[MISSION_FIRST_DEFENCE_OBJECT][i]  += (UINT16) (( ui16FirstTotalDamage * 150 * (m_pInfo->_aUserScoreRound[i].m_KillCount+1) ) / (m_pInfo->_aTeamScoreRound[TEAM_BLUE].m_KillCount+ui16CurrentBattleUserBlue));
					m_ui32AccMissionDefenceScore[MISSION_SECOND_DEFENCE_OBJECT][i] += (UINT16) (( ui16SecondTotalDamage * 100 * (m_pInfo->_aUserScoreRound[i].m_KillCount+1) ) / (m_pInfo->_aTeamScoreRound[TEAM_BLUE].m_KillCount+ui16CurrentBattleUserBlue));
				}
			}
		}

	}

	if(m_pInfo->_ui8RoundWinTeam != TEAM_DRAW)	//무승부 - 방어미션에서 무승부는 없음
	{
		m_pInfo->_aui16TotalRoundCount[m_pInfo->_ui8RoundWinTeam]++;
	}

	//해킹 판단하기 
	UINT16 aui16RedTotalDamage[MAX_OBJECT_COUNT];
	for( INT32 i = 0 ; i < MAX_OBJECT_COUNT ; i++ )
	{
		aui16RedTotalDamage[i] = 0;
		for( INT32 j = TEAM_RED ; j < SLOT_MAX_COUNT ; j += 2 )
		{
			aui16RedTotalDamage[i]  += m_MiDefenceInfo._nUserDamage[i][j];//RED
		}
		// 20% 여유를 둔다.		
		if(aui16RedTotalDamage[i] > (m_pui16MiObjectMaxHp[i]*1.2f) )
		{
			for(INT32 i32SlotIdx = 0; i32SlotIdx < SLOT_MAX_COUNT; i32SlotIdx++)
			{
				if( SLOT_STATE_BATTLE != m_pInfo->_aSlotInfo[ i32SlotIdx ]._State ) continue;

				m_ui8AccMissionDefence[ i32SlotIdx ] = 0;
				m_ui32AccMissionUserDamage[MISSION_FIRST_DEFENCE_OBJECT][ i32SlotIdx ] = 0;
				m_ui32AccMissionUserDamage[MISSION_SECOND_DEFENCE_OBJECT][ i32SlotIdx ] = 0;
				
				if( 0 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
				{
					m_ui32AccMissionDefenceScore[MISSION_FIRST_DEFENCE_OBJECT][ i32SlotIdx ] = 0;
					m_ui32AccMissionDefenceScore[MISSION_SECOND_DEFENCE_OBJECT][ i32SlotIdx ] = 0;
				}
				
				m_pInfo->_aM_UserScore[ i32SlotIdx ].m_KillCount = 0;
			}
			g_pLog->WriteLog( L"DEFENCE MISSION ERROR"); 
		}
	}

	if( 1 == g_pContextMain->GetContentsVersion ( CONTENTS_CALCULATE_POINT ) )
	{

		m_ui16AccMissionDefenceRemain[MISSION_FIRST_DEFENCE_OBJECT] += m_pui16MiObjectMaxHp[MISSION_FIRST_DEFENCE_OBJECT] 
		- aui16RedTotalDamage[MISSION_FIRST_DEFENCE_OBJECT];
		m_ui16AccMissionDefenceRemain[MISSION_SECOND_DEFENCE_OBJECT] += m_pui16MiObjectMaxHp[MISSION_SECOND_DEFENCE_OBJECT] 
		- aui16RedTotalDamage[MISSION_SECOND_DEFENCE_OBJECT];
	}
	//#endif
 
	return TRUE;
}

BOOL CRoomInDefence::CheckMatchEnd()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] >= m_ui32RoundCount )	return TRUE;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] >= m_ui32RoundCount )	return TRUE;

	return FALSE;
}

INT32 CRoomInDefence::GetWinTeam()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] > m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_RED_WIN;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] < m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

REAL32 CRoomInDefence::GetSubPercentOfUserCount2( INT32 i32Count )
{
	i32Count -= 1;	// 인원수를 배열로 변경하기 때문에 1을 빼줍니다.

	if( 0 > i32Count )					return 0.f;

	if (8 == g_pContextMain->GetMaxPerson(m_pInfo->_InfoBasic._StageID))
	{	// 4vs4 일 경우 다른 테이블을 사용합니다.
		if (SLOT4VS4_MAX_COUNT <= i32Count)	return 1.f;
		else									return m_pr32SubUserCountTwoVS4[i32Count];
	}
	else
	{
		if( SLOT_MAX_COUNT <= i32Count )		return 1.f;
		else									return m_pr32SubUserCountTwo[ i32Count ];
	}
}

REAL32 CRoomInDefence::GetSubPercentOfUnusualEnd( INT32 i32Win )
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

void CRoomInDefence::AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->m_ui16TotalRoundCount.SetValue( m_pInfo->_aui16TotalRoundCount, 0, TEAM_COUNT );
}

BOOL CRoomInDefence::MissionObjectDefence( DEFENCE_INFO* pInfo )
{	
	DEFENCE_INFO stDefenceInfo;
	m_MiDefenceInfo.CopyTo(&stDefenceInfo);	
	pInfo->CopyTo(&m_MiDefenceInfo);	//카피

	if ( GET_ROOM_INFO_ATK_DEF_AUTO_CHANGE(m_pInfo->_InfoBasic._InfoFlag) )
	{
		UINT8 ui8Changed = 0;
		UINT32 ui32CurRound = m_pInfo->_InfoAdd._NowRoundCount;
		if ( ui32CurRound == m_ui32RoundCount || ui32CurRound > m_ui32RoundCount )
			ui8Changed = 1;

		stDefenceInfo.CopyToBlueDefenceInfo(&m_MiDefenceInfo, ui8Changed);
	}

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_DEFENCE_INFO_ACK );
	SendPacket.WriteData( &m_MiDefenceInfo,	sizeof(DEFENCE_INFO) );
	_SendPacketBattlePlayer( &SendPacket );

	return TRUE;
}

UINT16 CRoomInDefence::GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp )
{
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	UINT16	ui16Exp = ui16AccExp;

	ui16Exp	= ui16Exp + (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount*pstKillExpPoint->_i16KillExp);

	// 오브젝트 공격중인 유저를 죽였을때 보터스 경험치
	ui16Exp = ui16Exp + (m_ui8AccMissionDefence[i32SlotIdx] * 10);

	// 오브젝트에 준 데미지에 따른 추가 경험치
	ui16Exp = ui16Exp + (UINT16)((m_ui32AccMissionUserDamage[MISSION_FIRST_DEFENCE_OBJECT][i32SlotIdx] * 100) / m_pui16MiObjectMaxHp[MISSION_FIRST_DEFENCE_OBJECT]);
	ui16Exp = ui16Exp + (UINT16)((m_ui32AccMissionUserDamage[MISSION_SECOND_DEFENCE_OBJECT][i32SlotIdx] * 150) / m_pui16MiObjectMaxHp[MISSION_SECOND_DEFENCE_OBJECT]);

	// 오브젝트에 준 데미지에 대한 추가 경험치를 상대팀에도 배분
	ui16Exp = ui16Exp + (UINT16)(m_ui32AccMissionDefenceScore[MISSION_FIRST_DEFENCE_OBJECT][i32SlotIdx] / m_pui16MiObjectMaxHp[MISSION_FIRST_DEFENCE_OBJECT]);
	ui16Exp = ui16Exp + (UINT16)(m_ui32AccMissionDefenceScore[MISSION_SECOND_DEFENCE_OBJECT][i32SlotIdx] / m_pui16MiObjectMaxHp[MISSION_SECOND_DEFENCE_OBJECT]);

	return ui16Exp;
}

UINT16 CRoomInDefence::GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 rAdjustPercent, REAL32 rAsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle )
{
	if( ExistRecord(i32SlotIdx ) == false ) return 0;

	UINT16	ui16Exp;

	TYPE_ROOM_GET_EXPPOINT_V1	stGetExpPoint;
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	GetBasicExpPoint(i32Win, &stGetExpPoint);
	pstKillExpPoint = GetKillExpPoint();

	switch( i32Win )
	{
	case TEAM_RED_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_RED )
				ui16Exp	= stGetExpPoint._i16WinExp;
			else
				ui16Exp = stGetExpPoint._i16LoseExp;
		}
		break;
	case TEAM_BLUE_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_BLUE )
				ui16Exp	= stGetExpPoint._i16WinExp;
			else
				ui16Exp = stGetExpPoint._i16LoseExp;
		}
		break;
	default: // draw / single
		{
			ui16Exp = stGetExpPoint._i16DrawExp;
		}
	}

	ui16Exp	= ui16AccExp + (UINT16)(ui16Exp * rAdjustPercent);	// (임무 경험치*팀원 숫자별 가중치)+(방어 미션 킬/데스 경험치*팀원 숫자별 가중치) == (임무 경험치+방어 미션 킬/데스 경험치)*팀원 숫자별 가중치

	return ui16Exp;
}

//#else
INT16 CRoomInDefence::GetMissionExp( INT32 i32SlotIdx )
{
	INT16 i16MissionExp = CRoomBase::GetMissionExp(i32SlotIdx);

	i16MissionExp += MIN( m_ui8BonusDefenceMax, m_ui8AccMissionDefence[i32SlotIdx] ) * m_ui8BonusDefenceScore;

	UINT32 ui32Damage = (m_ui32AccMissionUserDamage[MISSION_FIRST_DEFENCE_OBJECT][i32SlotIdx]
						+ m_ui32AccMissionUserDamage[MISSION_SECOND_DEFENCE_OBJECT][i32SlotIdx]);
	i16MissionExp += (INT16)(( MIN( m_ui8BonusDamageMax, ui32Damage ) * m_ui8rBonusDamageScore ) / 10);

	UINT16 ui16RemainHP = m_ui16AccMissionDefenceRemain[MISSION_FIRST_DEFENCE_OBJECT]
						  + m_ui16AccMissionDefenceRemain[MISSION_SECOND_DEFENCE_OBJECT];
	i16MissionExp += ( MIN( m_ui8BonusHPMax, ui16RemainHP ) * m_ui8rBonusHPScore ) / 10;

	return i16MissionExp;
}
//#endif

void CRoomInDefence::GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer )
{
	pRoomNowScore->ScoreType	= ROOM_SCORE_ROUNDCOUNT_TYPE;
	pRoomNowScore->PlayTime		= 0;

	for(INT32 Loop = 0; Loop < TEAM_COUNT; Loop++)
	{
		pRoomNowScore->Score[Loop] = m_pInfo->_aui16TotalRoundCount[Loop];
	}
}