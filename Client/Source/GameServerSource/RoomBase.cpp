#include "pch.h"
#include "RoomBase.h"
#include "UserSession.h"
#include "ModuleTrans.h"
#include "ModuleClan.h"
#include "ModuleLogFile.h"

I3_CLASS_INSTANCE( CRoomBase, i3ElementBase );

CRoomBase::CRoomBase()
{
#ifndef NEW_EXPPOINT
	i3mem::FillZero( &m_stGetExpPoint, sizeof(TYPE_ROOM_GET_EXPPOINT) );
	i3mem::FillZero( &m_stKillExpPoint, sizeof(TYPE_ROOM_KILL_EXPPOINT) );
#else
	i3mem::FillZero( &m_stExpPointValues,	sizeof(TYPE_ROOM_GET_EXPPOINT_VALUES) );
//	i3mem::FillZero( &m_stExpPointValues4VS4,	sizeof(TYPE_ROOM_GET_EXPPOINT_VALUES) );
#endif

#ifndef NEW_EXPPOINT
	i3mem::FillZero( m_pr32SubUserCount, sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::FillZero( m_pr32SubUserCountVS4, sizeof(REAL32)*SLOT4VS4_MAX_COUNT );
#endif

	m_eStartOption		= ROOM_START_DIRECT;
	m_ePreStartOption	= ROOM_PRESTART_ONE_WAIT;
	m_eRespawnOption	= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption	= ROOM_CHATTING_ALL;
	m_eEndOption		= ROOM_END_MOD;

	m_ui32RoundCount	= 1;
	m_ui32RespawnCount	= 1;

#ifdef NEW_EXPPOINT
	m_pstExtPointValues = &m_stExpPointValues;
#endif
}

CRoomBase::~CRoomBase()
{
}

BOOL CRoomBase::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
	m_pInfo			= pRoomInfo;

	return TRUE;
}

void CRoomBase::OnDestroy()
{
}

BOOL CRoomBase::CheckSubType( UINT8 ui8SubType )
{
	// 모드별로 플레이 가능한 승리조건인지, 제한시간이 맞는지 확인
	UINT8 ui8RoundEndTimeIdx = (ui8SubType & 0xF0) >> 4;

#ifndef _DEBUG
	if( ui8RoundEndTimeIdx >= (sizeof(g_TeamTimeTable)/sizeof(UINT32))-2 ) return FALSE;		// 무한과 20초는 테스트용이므로 설정테이블의 마지막 2개는 제외

	if( m_eEndOption == ROOM_END_KILLCOUNT )
	{
		UINT8 ui8RoundEndKillCount = (ui8SubType & 0x0F);

		if( ui8RoundEndKillCount >= (sizeof(g_TeamKillCountTable)/sizeof(UINT32))-1 ) return FALSE;
	}
	else if( m_eEndOption == ROOM_END_ROUND )
	{
		UINT8 ui8EndRoundCount = (ui8SubType & 0x0F);

		if( ui8EndRoundCount >= (sizeof(g_MissionRoundCountTable)/sizeof(UINT32))-1 ) return FALSE;
	}
#endif

	return TRUE;
}

void CRoomBase::UserMatchEndBouns( INT32 i32SlotIdx )
{
}

bool CRoomBase::CheckRespawn( INT32 i32SlotIdx )
{
	switch( m_eRespawnOption )
	{
	case ROOM_RESPAWN_CONTINUE:
		return true;
	case ROOM_RESPAWN_NONE:
		// 게임 시작한 유저에 대해서 최초 한번만 리스폰 가능합니다.
		if( m_pInfo->_ui16RoundStartUser & (0x1 << i32SlotIdx) )
		{
			if( 1 > m_pInfo->_aui32RespawnCount[i32SlotIdx] )
			{
				return true;
			}
		}
		return false;
	case ROOM_RESPAWN_LIMIT:
		// 리스폰 제한 - m_i32RespawnCount 수보다 많게는 리스폰 할 수 없다.
		if( m_ui32RespawnCount > m_pInfo->_aui32RespawnCount[i32SlotIdx] )
		{
			return true;
		}
		return false;
	}
	return false;
}

bool CRoomBase::CheckLoadingStart()
{
	INT32 i32MainOtherTeam = (m_pInfo->_i32MainSlotIdx+1) % TEAM_COUNT;
	for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i += 2 )
	{
		if( SLOT_STATE_READY != m_pInfo->_SlotInfo[i]._State ) continue;	// Ready 한 유저중에서
		return true;													// 상대편이 한명이라도 있으면 시작합니다.
	}
	return false;
}

bool CRoomBase::CheckMatchStart()
{
	switch( m_ePreStartOption )
	{
	case ROOM_PRESTART_DIRECT:
		return true;
	case ROOM_PRESTART_ONE_WAIT:
		{
			INT32 i32MainOtherTeam = (m_pInfo->_i32MainSlotIdx+1) % TEAM_COUNT;	// 방장의 다른 팀을 구합니다.
			for( INT32 i = i32MainOtherTeam ; i < SLOT_MAX_COUNT ; i += 2 )
			{
				if( SLOT_STATE_BATTLE_READY == m_pInfo->_SlotInfo[i]._State )
				{
					return true;
				}
			}
		}
		break;
	case ROOM_PRESTART_ALL_WAIT:
		{
			UINT32	ui32CheckStartUser		= 0;
#ifdef DEF_OBSERVER_MODE
			for( INT32 i = 0 ; i < SLOT_ALL_COUNT ; i++ )
#else
			for( INT32 i = 0 ; i < SLOT_MAX_COUNT ; i++ )
#endif
			{
				if( SLOT_STATE_BATTLE_READY == m_pInfo->_SlotInfo[i]._State )	ui32CheckStartUser |= (0x1 << i); 
			}
			if( (ui32CheckStartUser & m_pInfo->_ui32FirstBattleStartUser) == m_pInfo->_ui32FirstBattleStartUser )
			{
				return true;
			}
		}
		break; 
	}
	return false;
}

#ifndef NEW_EXPPOINT
void CRoomBase::GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT* pstGetExpPoint )
{
	i3mem::Copy( pstGetExpPoint, &m_stGetExpPoint, sizeof(TYPE_ROOM_GET_EXPPOINT) );
}
#else
INT16 CRoomBase::GetLimitKillCount()
{
	return m_pstExtPointValues->_stRoundExp._i16KillPerRound1Round;
}

INT16 CRoomBase::GetKillExp( INT32 i32SlotIdx )
{
	INT16 i16LimitKillCount = GetLimitKillCount();

	INT16 i16NowRoundKillCount = m_pInfo->_aUserScoreRound[i32SlotIdx].m_KillCount;
	INT16 i16KillExp = 0;
	INT16 i16LowRangeKillCount = (INT16) (i16LimitKillCount * 0.3);
	INT16 i16MidRangeKillCount = (INT16) (i16LimitKillCount * 0.7) - i16LowRangeKillCount;
	INT16 i16HighRangeKillCount = i16LimitKillCount - (i16LowRangeKillCount + i16MidRangeKillCount);

	// Low Range
	i16KillExp += MIN( i16LowRangeKillCount, i16NowRoundKillCount ) * m_pstExtPointValues->_stRoundExp._i16rLowKillFactor;
	i16NowRoundKillCount -= i16LowRangeKillCount;

	// Middle Range
	if( i16NowRoundKillCount > 0 )
	{
		i16KillExp += MIN( i16MidRangeKillCount, i16NowRoundKillCount ) * m_pstExtPointValues->_stRoundExp._i16rMidKillFactor;
		i16NowRoundKillCount -= i16MidRangeKillCount;
	}

	// HighRange
	if( i16NowRoundKillCount > 0 )
	{
		i16KillExp += MIN( i16HighRangeKillCount, i16NowRoundKillCount ) * m_pstExtPointValues->_stRoundExp._i16rHighKillFactor;
	}

	return i16KillExp;
}

INT16 CRoomBase::GetMissionExp( INT32 i32SlotIdx )
{
	INT16 i16MissionExp = 0;

	// HeadShot
	i16MissionExp += MIN( m_pstExtPointValues->_stRoundExp._i16HeadshotMax, m_pInfo->_aui16M_Headshot[i32SlotIdx] ) * m_pstExtPointValues->_stRoundExp._i16HeadshotScore;

	// ChainKill
	INT16 i16ChainKillCount = m_pInfo->_aui16M_AccKill_1[i32SlotIdx] + m_pInfo->_aui16M_AccKill_2[i32SlotIdx] + m_pInfo->_aui16M_AccKill_3[i32SlotIdx];
	i16MissionExp += MIN( m_pstExtPointValues->_stRoundExp._i16ChainKillMax, i16ChainKillCount ) * m_pstExtPointValues->_stRoundExp._i16ChainKillScore;

	// ChainMelee
	i16MissionExp += MIN( m_pstExtPointValues->_stRoundExp._i16ChainMeleeMax, m_pInfo->_aui16M_ChainSlugger[i32SlotIdx] ) * m_pstExtPointValues->_stRoundExp._i16ChainMeleeScore;

	return i16MissionExp;
}

INT16 CRoomBase::GetRoundExp( INT32 i32SlotIdx, INT32 i32Win, BOOL bNormalEnd )
{
	INT16 i16RoundExp = 0;

	i16RoundExp = m_pstExtPointValues->_stRoundExp._i16Round * m_pInfo->_InfoAdd._NowRoundCount;

	return i16RoundExp;
}

INT16 CRoomBase::GetMissionPoint( INT32 i32SlotIdx )
{
	UINT16 ui16PlayTimeCheckIdx = (UINT16)(m_pInfo->_pSession[i32SlotIdx]->m_ui32PlayTime / 5);
	if( ui16PlayTimeCheckIdx >= MISSION_POINT_MAX_COUNT ) ui16PlayTimeCheckIdx = MISSION_POINT_MAX_COUNT-1;
	INT16 i16MissionPoint = 0;
	INT16 i16MissionCount = 0;

	//// HeadShot
	//i16MissionPoint += MIN( m_pstExtPointValues->_stRoundPoint._i16HeadshotMax, m_pInfo->_pui16M_Headshot[i32SlotIdx] ) * m_pstExtPointValues->_stRoundPoint._i16HeadshotScore;

	//// ChainKill
	INT16 i16ChainKillCount = m_pInfo->_aui16M_AccKill_1[i32SlotIdx] + m_pInfo->_aui16M_AccKill_2[i32SlotIdx] + m_pInfo->_aui16M_AccKill_3[i32SlotIdx];
	//i16MissionPoint += MIN( m_pstExtPointValues->_stRoundPoint._i16ChainKillMax, i16ChainKillCount ) * m_pstExtPointValues->_stRoundPoint._i16ChainKillScore;

	//// ChainMelee
	//i16MissionPoint += MIN( m_pstExtPointValues->_stRoundPoint._i16ChainMeleeMax, m_pInfo->_pui16M_ChainSlugger[i32SlotIdx] ) * m_pstExtPointValues->_stRoundPoint._i16ChainMeleeScore;

	i16MissionCount = m_pInfo->_aui16M_Headshot[i32SlotIdx] + i16ChainKillCount + m_pInfo->_aui16M_ChainSlugger[i32SlotIdx];
	i16MissionPoint += MIN( m_pstExtPointValues->_stRoundPoint._ai16BonusMaxCount[ui16PlayTimeCheckIdx], i16MissionCount ) * m_pstExtPointValues->_stRoundPoint._i16BonusScore;

	return i16MissionPoint;
}

// 라운드 마다의 기록이 필요한 데이터들 처리
void CRoomBase::SetRoundDataCount()
{
	for( INT8 i = 0; i < SLOT_MAX_COUNT; i++ )
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;

		m_pInfo->_aui16Acc_KillExp[i] += GetKillExp(i);
	}
}
#endif

#ifndef NEW_EXPPOINT
REAL32 CRoomBase::GetSubPercentOfUserCount( INT32 i32Count )
{
	i32Count -= 1;	// 인원수를 배열로 변경하기 때문에 1을 빼줍니다.

	if( 0 > i32Count )					return 0.f;

	if( STAGE_SLOT_MODE_4VS4 == GET_STAGESLOT( m_pInfo->_InfoBasic._StageID ) )
	{	// 4vs4 일 경우 다른 테이블을 사용합니다.
		if( SLOT4VS4_MAX_COUNT <= i32Count )	return 1.f;
		else									return m_pr32SubUserCountVS4[ i32Count ];
	}
	else
	{
		if( SLOT_MAX_COUNT <= i32Count )		return 1.f;
		else									return m_pr32SubUserCount[ i32Count ];
	}
}

REAL32 CRoomBase::GetSubPercentOfUserCount2( INT32 i32Count )
{
	return 1.f;
}
#endif

REAL32 CRoomBase::GetSubPercentOfUserCountTeam( INT32 i32Team, INT32 i32AllCount, INT32 i32Count )
{
#ifndef NEW_EXPPOINT
	return GetSubPercentOfUserCount(i32Count);
#else
	if( i32AllCount == 0 ) return 0;

	REAL32 r32MemberRate = ((REAL32)m_stExpPointValues._stRoundExp._ai16rUserCountValue[SLOT_MAX_COUNT-i32AllCount])/10;

	//if( m_pstExtPointValues->_stMemberRate._bUseMemberRate )
	//{
	//	INT16 i16MemberMax = 0;
	//	if( STAGE_SLOT_MODE_4VS4 == GET_STAGESLOT( m_pInfo->_InfoBasic._StageID ) )
	//		i16MemberMax = SLOT4VS4_MAX_COUNT;
	//	else
	//		i16MemberMax = SLOT_MAX_COUNT;

	//	if( i16MemberMax <= i32AllCount ) r32MemberRate = 1.f;
	//	else if( m_pstExtPointValues->_stMemberRate._i16MemberRangeEnd <= i32AllCount ) r32MemberRate = ((REAL32)m_pstExtPointValues->_stMemberRate._i16rMemberRangeRate) / 10;
	//	else r32MemberRate = ((REAL32)m_pstExtPointValues->_stMemberRate._i16rmemberMinRate) / 10;
	//}

	return r32MemberRate;
#endif
}

#ifdef NEW_EXPPOINT
UINT16 CRoomBase::UserKillBouns( INT32 i32SlotIdx, UINT32 ui32DeathType, UINT32 ui32DeathMyState, UINT32 ui32DeathEnemyState, UINT32 ui32RTBattle )
{
	//UINT16 ui16GainPoint = 0;

	//// HeadShot
	//if( ui32DeathType == CHARA_DEATH_H )
	//{
	//	ui16GainPoint += ( m_pstExtPointValues->_stRoundPoint._i16HeadshotMax > m_pInfo->_pui16M_Headshot[i32SlotIdx] ) ? m_pstExtPointValues->_stRoundPoint._i16HeadshotScore : 0;
	//}

	//// ChainKill
	//if( m_pInfo->m_ui8A_KillCount[i32SlotIdx] > 0 )
	//{
	//	INT16 i16ChainKillCount = m_pInfo->_pui16M_AccKill_1[i32SlotIdx] + m_pInfo->_pui16M_AccKill_2[i32SlotIdx] + m_pInfo->_pui16M_AccKill_3[i32SlotIdx];
	//	ui16GainPoint += ( m_pstExtPointValues->_stRoundPoint._i16ChainKillMax > i16ChainKillCount ) ? m_pstExtPointValues->_stRoundPoint._i16ChainKillScore : 0;
	//}

	//// ChainMelee
	//if( m_pInfo->m_ui8A_SlugShot[i32SlotIdx] > 0 )
	//{
	//	ui16GainPoint += ( m_pstExtPointValues->_stRoundPoint._i16ChainMeleeMax > m_pInfo->_pui16M_ChainSlugger[i32SlotIdx] ) ? m_pstExtPointValues->_stRoundPoint._i16ChainMeleeScore : 0;
	//}

	//return ui16GainPoint;	

	UINT16 ui16PlayTimeCheckIdx = (UINT16)((
#ifdef USE_GM_PAUSE
											ui32RTBattle 
#else
											g_pContextMain->GetServerTime()
#endif
																	- m_pInfo->_pSession[i32SlotIdx]->m_ui32BattleStartTime) / 5);
	if( ui16PlayTimeCheckIdx >= MISSION_POINT_MAX_COUNT ) ui16PlayTimeCheckIdx = MISSION_POINT_MAX_COUNT-1;
	INT16 ui16GainPoint = 0;
	INT16 i16MissionCount = 0;
	INT16 i16GainCount = 0;

	INT16 i16ChainKillCount = m_pInfo->_aui16M_AccKill_1[i32SlotIdx] + m_pInfo->_aui16M_AccKill_2[i32SlotIdx] + m_pInfo->_aui16M_AccKill_3[i32SlotIdx];
	i16MissionCount = m_pInfo->_aui16M_Headshot[i32SlotIdx] + i16ChainKillCount + m_pInfo->_aui16M_ChainSlugger[i32SlotIdx];

	// HeadShot
	if( ui32DeathType == CHARA_DEATH_H )
	{
		ui16GainPoint += ( m_pstExtPointValues->_stRoundPoint._ai16BonusMaxCount[ui16PlayTimeCheckIdx] > i16MissionCount+i16GainCount ) ? m_pstExtPointValues->_stRoundPoint._i16BonusScore : 0;
		i16GainCount++;
	}

	// ChainKill
	if( m_pInfo->m_aui8A_KillCount[i32SlotIdx] > 0 )
	{
		ui16GainPoint += ( m_pstExtPointValues->_stRoundPoint._ai16BonusMaxCount[ui16PlayTimeCheckIdx] > i16MissionCount+i16GainCount ) ? m_pstExtPointValues->_stRoundPoint._i16BonusScore : 0;
		i16GainCount++;
	}

	// ChainMelee
	if( m_pInfo->m_aui8A_SlugShot[i32SlotIdx] > 0 )
	{
		ui16GainPoint += ( m_pstExtPointValues->_stRoundPoint._ai16BonusMaxCount[ui16PlayTimeCheckIdx] > i16MissionCount+i16GainCount ) ? m_pstExtPointValues->_stRoundPoint._i16BonusScore : 0;
		i16GainCount++;
	}

	return ui16GainPoint;
}
#endif

UINT16 CRoomBase::GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp )
{
	UINT16	ui16Exp = ui16AccExp;

#ifndef NEW_EXPPOINT
	// 매치 종료시 한번에 계산
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	ui16Exp	= ui16Exp + (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount*pstKillExpPoint->_i16KillExp);
#else
	// 라운드 종료시마다 계산
	ui16Exp += m_pstExtPointValues->_stExpPoint._i16rExpPerKill * m_pInfo->_aui16Acc_KillExp[i32SlotIdx];
	ui16Exp += m_pstExtPointValues->_stExpPoint._i16rExpPerMission * GetMissionExp( i32SlotIdx ) * 10;
	ui16Exp /= 100;
#endif

	return ui16Exp;
}

UINT16 CRoomBase::GetMatchAccPoint( INT32 i32SlotIdx, UINT16 ui16AccPoint )
{
	UINT16	ui16Point = ui16AccPoint;

#ifndef NEW_EXPPOINT
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	ui16Point = ui16Point + (m_pInfo->_aM_UserScore[i32SlotIdx].m_KillCount*pstKillExpPoint->_i16KillPoint);

	// 멀티킬
	ui16Point = ui16Point + (m_pInfo->_aui16M_AccKill_1[i32SlotIdx]*pstKillExpPoint->_i16MultiPt);
	ui16Point = ui16Point + (m_pInfo->_aui16M_AccKill_2[i32SlotIdx]*pstKillExpPoint->_i16MultiPt*2);
	ui16Point = ui16Point + (m_pInfo->_aui16M_AccKill_3[i32SlotIdx]*pstKillExpPoint->_i16MultiPt*3);

	// 피어싱샷
	ui16Point = ui16Point + (m_pInfo->_aui16M_PiercingShot[i32SlotIdx]*pstKillExpPoint->_i16MultiPt_Pier);

	// 멀티킬
	ui16Point = ui16Point + (m_pInfo->_aui16M_MassKill[i32SlotIdx]*pstKillExpPoint->_i16MultiPt_Mass);

	// Chain Stopper
	ui16Point = ui16Point + (m_pInfo->_aui16M_ChainStoppper[i32SlotIdx]*pstKillExpPoint->_i16MultiPt_Stop);

	// Chain head shot
	ui16Point = ui16Point + (m_pInfo->_aui16M_ChainHeadShot[i32SlotIdx]*pstKillExpPoint->_i16MultiPt_Head);

	// Chain Slugger
	ui16Point = ui16Point + (m_pInfo->_aui16M_ChainSlugger[i32SlotIdx]*pstKillExpPoint->_i16MultiPt_Slug);
#else
	ui16Point = GetMissionPoint( i32SlotIdx );
#endif

	return ui16Point;
}

UINT16 CRoomBase::GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 rAdjustPercent, BOOL bNormalEnd, UINT32 ui32RTBattle )
{
	if( ExistRecord(i32SlotIdx ) == false ) return 0;

	UINT16	ui16Exp;

#ifndef NEW_EXPPOINT
	TYPE_ROOM_GET_EXPPOINT	stGetExpPoint;
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	GetBasicExpPoint(i32Win, &stGetExpPoint);
	pstKillExpPoint = GetKillExpPoint();

	switch( i32Win )
	{
	case TEAM_RED_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_RED )
				ui16Exp	= ui16AccExp + stGetExpPoint._i16WinExp;
			else
				ui16Exp = ui16AccExp + stGetExpPoint._i16LoseExp;
		}
		break;
	case TEAM_BLUE_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_BLUE )
				ui16Exp	= ui16AccExp + stGetExpPoint._i16WinExp;
			else
				ui16Exp = ui16AccExp + stGetExpPoint._i16LoseExp;
		}
		break;
	default: // draw / single
		{
			ui16Exp = ui16AccExp + stGetExpPoint._i16DrawExp;
		}
	}

	ui16Exp	= (UINT16)(ui16Exp * rAdjustPercent);
#else
	UINT32 ui32Exp = ui16AccExp*100;							// 다른 REAL 값 INT로 바꿔 계산하는것들과 균형을 위해 10 곱해줌
	UINT32 ui32RoundExp = 0;
	ui32RoundExp += (m_pstExtPointValues->_stExpPoint._i16rExpPerRound * GetRoundExp( i32SlotIdx, i32Win, bNormalEnd ));

	switch( i32Win )
	{
	case TEAM_RED_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_RED )
				ui32RoundExp	*= m_pstExtPointValues->_stRoundExpPointRate._i16rWinExp;
			else
				ui32RoundExp	*= m_pstExtPointValues->_stRoundExpPointRate._i16rLoseExp;
		}
		break;
	case TEAM_BLUE_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_BLUE )
				ui32RoundExp	*= m_pstExtPointValues->_stRoundExpPointRate._i16rWinExp;
			else
				ui32RoundExp	*= m_pstExtPointValues->_stRoundExpPointRate._i16rLoseExp;
		}
		break;
	default: // draw / single
		{
			ui32RoundExp	*= m_pstExtPointValues->_stRoundExpPointRate._i16rDrawExp;
		}
	}
	ui32Exp += ui32RoundExp;

	// 인원비중 적용
	ui32Exp	= (UINT32)(ui32Exp * rAdjustPercent);
	
	ui16Exp = (UINT16)(ui32Exp/100);
#endif

	return ui16Exp;
}

UINT16 CRoomBase::GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 rAdjustPercent, BOOL bNormalEnd,
#ifndef NEW_EXPPOINT
																																UINT32 ui32RTBattle )
#else
																																INT8 * pi8Ranking, INT8 i8BattleUserCount, UINT32 ui32RTBattle )
#endif
{
	if( ExistRecord(i32SlotIdx ) == false ) return 0;

	UINT16	ui16Point;

#ifndef NEW_EXPPOINT
	TYPE_ROOM_GET_EXPPOINT	stGetExpPoint;
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	GetBasicExpPoint(i32Win, &stGetExpPoint);
	pstKillExpPoint = GetKillExpPoint();

	switch( i32Win )
	{
	case TEAM_RED_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_RED )
				ui16Point	= ui16AccPoint + stGetExpPoint._i16WinPoint;
			else
				ui16Point = ui16AccPoint + stGetExpPoint._i16LosePoint;
		}
		break;
	case TEAM_BLUE_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_BLUE )
				ui16Point	= ui16AccPoint + stGetExpPoint._i16WinPoint;
			else
				ui16Point = ui16AccPoint + stGetExpPoint._i16LosePoint;
		}
		break;
	default: // draw / single
		{
			ui16Point = ui16AccPoint + stGetExpPoint._i16DrawPoint;
		}
	}

	ui16Point	= (UINT16)(ui16Point * rAdjustPercent);
#else
	INT8  i8Rank = ((SLOT_MAX_COUNT - i8BattleUserCount) + (pi8Ranking[i32SlotIdx]-1));				// 뒤에서부터 등수 매김
	INT32 ui32Point = ui16AccPoint*100;															// 다른 REAL 값 INT로 바꿔 계산하는것들과 균형을 위해 10 곱해줌
	ui32Point += m_pstExtPointValues->_stRoundPoint._ai16r2RankPoint[i8Rank];					// 등수 점수
	ui32Point += m_pstExtPointValues->_stRoundPoint._ai16rUserCountValue[SLOT_MAX_COUNT-i8BattleUserCount]*10;	// 인원 점수

	// 승패 점수
	switch( i32Win )
	{
	case TEAM_RED_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_RED )
				ui32Point	+= m_pInfo->_pSession[i32SlotIdx]->m_ui32PlayTime * m_pstExtPointValues->_stRoundExpPointRate._i16r2WinPoint;
			else
				ui32Point	+= m_pInfo->_pSession[i32SlotIdx]->m_ui32PlayTime * m_pstExtPointValues->_stRoundExpPointRate._i16r2LosePoint;
		}
		break;
	case TEAM_BLUE_WIN:
		{
			if( (i32SlotIdx % 2) == TEAM_BLUE )
				ui32Point	+= m_pInfo->_pSession[i32SlotIdx]->m_ui32PlayTime * m_pstExtPointValues->_stRoundExpPointRate._i16r2WinPoint;
			else
				ui32Point	+= m_pInfo->_pSession[i32SlotIdx]->m_ui32PlayTime * m_pstExtPointValues->_stRoundExpPointRate._i16r2LosePoint;
		}
		break;
	default: // draw / single
		{
			ui32Point	+= m_pInfo->_pSession[i32SlotIdx]->m_ui32PlayTime * m_pstExtPointValues->_stRoundExpPointRate._i16r2DrawPoint;
		}
	}

	ui16Point = ui32Point / 100;
	// 보상공식 1초에 포인트 1 받는 공식에서 2초에 1 받는 것으로 수정되었습니다.
	// (풀방기준 m_ui32AccActualBattleTime이 10분이면 기본 300포인트)
	// (인게임내에서 시간적용되지않는 예외사항(리스폰) 고려할것)
	ui16Point = (UINT16)(ui16Point / POINT_PER_SECOND);		
	// 모드에 따른 가중치적용
	ui16Point = (UINT16)(ui16Point * rAdjustPercent);

#endif

	return ui16Point;
}

void CRoomBase::_SendPacketBattlePlayer( i3NetworkPacket* pPacket, INT32 i32TeamIdx )
{
	INT32 i; 

	if( SERVER_VALUE_UNKNOWN == i32TeamIdx )
	{
		for( i = 0 ; i < SLOT_MAX_COUNT ; i++ )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
			
			m_pInfo->_pSession[i]->SendPacketMessage( pPacket );
		}
	}
	else 
	{
		for( i = i32TeamIdx ; i < SLOT_MAX_COUNT ; i += 2 )
		{
			if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
			
			m_pInfo->_pSession[i]->SendPacketMessage( pPacket );
		}
	}

#ifdef DEF_OBSERVER_MODE
	//옵저버, 히든 유저
	for( i = SLOT_MAX_COUNT ; i < SLOT_ALL_COUNT ; i++ )
	{
		if( SLOT_STATE_BATTLE != m_pInfo->_SlotInfo[i]._State ) continue;
		{
			//if(iTeamIdx != SERVER_VALUE_UNKNOWN && SLOT_MAX_COUNT <= Loop && SLOT_ROOM_VIEW_COUNT > Loop && (FALSE == m_Info._pSession[Loop]->IsGMUser())) continue;
			m_pInfo->_pSession[i]->SendPacketMessage( pPacket );
		}
	}
#endif
}

void CRoomBase::GetRanking( INT8 *i8Ranking, INT8 i8BattleUserCount, GET_RANKING_TYPE rankingType )
{
	// 순위 정렬
	BOOL	bCheckFlag[SLOT_MAX_COUNT] = {0,};
	INT8	i8RankCount		= 1;
	INT8	i8Target;
	INT8	i8PrevTarget	= -1;

	for(INT32 i = 0; i < i8BattleUserCount; i++)
	{
		i8Target = -1;

		// 남은 유저중 1순위 확인
		for(INT32 j = 0; j < SLOT_MAX_COUNT; j++)
		{
			if( (m_pInfo->_SlotInfo[j]._State != SLOT_STATE_BATTLE) || bCheckFlag[j] ) continue;
			if( TRUE == m_pInfo->_SlotInfo[j]._bIsGmObserverMode ) continue;
			
			if( i8Target < 0 )
			{
				i8Target = j;
				continue;
			}

			switch( rankingType )
			{
			case GET_RANKING_KILLDEATH:
				{
				if( m_pInfo->_aM_UserScore[j].m_KillCount > m_pInfo->_aM_UserScore[i8Target].m_KillCount )
					i8Target = j;
				else if( m_pInfo->_aM_UserScore[j].m_KillCount == m_pInfo->_aM_UserScore[i8Target].m_KillCount	&&
					  m_pInfo->_aM_UserScore[j].m_DeathCount < m_pInfo->_aM_UserScore[i8Target].m_DeathCount )
					i8Target = j;
				}
				break;
			case GET_RANKING_RAWEXP:
				{
					if( m_pInfo->_aui16AccExp[j] > m_pInfo->_aui16AccExp[i8Target] )
						i8Target = j;
				}
				break;
			}
		}

		if( i8Target < 0 ) continue;

		// 순위 기록 ( 이전 순위자와 동점일 경우 같은 순위로 확인 )
		if( i8PrevTarget < 0 )
			i8Ranking[i8Target] = i8RankCount;
		else
		{
			switch( rankingType )
			{
			case GET_RANKING_KILLDEATH:
				{
					if( m_pInfo->_aM_UserScore[i8PrevTarget].m_KillCount  == m_pInfo->_aM_UserScore[i8Target].m_KillCount	&&
						m_pInfo->_aM_UserScore[i8PrevTarget].m_DeathCount == m_pInfo->_aM_UserScore[i8Target].m_DeathCount	)
						i8Ranking[i8Target] = i8Ranking[i8PrevTarget];
					else
						i8Ranking[i8Target] = i8RankCount;
				}
				break;
			case GET_RANKING_RAWEXP:
				{
					if( m_pInfo->_aui16AccExp[i8PrevTarget] == m_pInfo->_aui16AccExp[i8Target] )
						i8Ranking[i8Target] = i8Ranking[i8PrevTarget];
					else
						i8Ranking[i8Target] = i8RankCount;
				}
				break;
			default:
				// 순위 기준이 없는 경우는 없지만.. 있다면 모든 유저 꼴찌로...
				i8Ranking[i8Target] = i8BattleUserCount;
				break;
			}
		}

		bCheckFlag[i8Target] = TRUE;
		i8RankCount++;
		i8PrevTarget = i8Target;
	}
}

BOOL CRoomBase::CheckVoteTime( UINT32 ui32ServerTime )
{
	// 마지막 라운드인지 체크
	if ( m_pInfo->_aui16TotalRoundCount[TEAM_RED] + 1 == m_ui32RoundCount && m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] + 1 == m_ui32RoundCount )
	{
		// 마지막 라운드라면 종료시간이 VOTE_FINISH_TIME만큼 남았는지 검사
		UINT8 ui8EndTimeIdx = (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4;
#ifdef USE_GM_PAUSE
		if ( ui32ServerTime > g_TeamTimeTable[ui8EndTimeIdx] - VOTE_FINISH_TIME ) 
#else
		if ((m_pInfo->_ui32BattleStartTime + g_TeamTimeTable[ui8EndTimeIdx]) <= ui32ServerTime + VOTE_FINISH_TIME ) 
#endif
		{
			// VOTE_FINISH_TIME만큼 밖에 남지 않았다면 투표 불가
			return FALSE;
		}
	}

	return TRUE;
}
