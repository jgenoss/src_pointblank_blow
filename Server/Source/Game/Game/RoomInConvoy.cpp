#include "pch.h"
#include "RoomInConvoy.h"

I3_CLASS_INSTANCE( CRoomInConvoy, CRoomBase );

CRoomInConvoy::CRoomInConvoy()
{

}

CRoomInConvoy::~CRoomInConvoy()
{
	OnDestroy();
}

BOOL CRoomInConvoy::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{ 

	m_stGetExpPointV1._i16WinExp	= 160;
	m_stGetExpPointV1._i16LoseExp	= 80;
	m_stGetExpPointV1._i16DrawExp	= 0;
	m_stGetExpPointV1._i16WinPoint	= 120;
	m_stGetExpPointV1._i16LosePoint = 60;
	m_stGetExpPointV1._i16DrawPoint = 0;

	m_stKillExpPoint._i16KillExp	= 20;
	m_stKillExpPoint._i16KillPoint	= 20;
	m_stKillExpPoint._i16AssistExp	= 2;
	m_stKillExpPoint._i16MultiPt		= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Head	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Slug	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Stop	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.4001f);
	m_stKillExpPoint._i16MultiPt_Mass	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);
	m_stKillExpPoint._i16MultiPt_Pier	= (UINT16)(m_stKillExpPoint._i16KillPoint*0.2001f);

	// 보너스 경험치/포인트 (공통 설정사항)
	m_stExpPointValues._stRoundExp._i16Round						= 30;
	m_stExpPointValues._stRoundExp._i16KillPerRound1Round			= 6;
	m_stExpPointValues._stRoundExp._i16AssistPerRound1Round			= 3;
	m_stExpPointValues._stRoundExp._i16rLowKillFactor				= (INT16)( 2	* 10 );
	m_stExpPointValues._stRoundExp._i16rMidKillFactor				= (INT16)( 1.5	* 10 );
	m_stExpPointValues._stRoundExp._i16rHighKillFactor				= (INT16)( 1	* 10 );

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

	m_stExpPointValues._stExpPoint._i16rExpPerRound		= (INT16)(1.7 * 10);
	m_stExpPointValues._stExpPoint._i16rExpPerKill		= (INT16)(3 * 10);
	m_stExpPointValues._stExpPoint._i16rExpPerMission	= (INT16)(2 * 10);
	m_stExpPointValues._stExpPoint._i16rExpPerAssist	= (INT16)(3 * 10);

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


	// 보너스 경험치/포인트 (모드 전용)
	m_ui8BonusTouchDownMax								= 3;	// 추후에 정규모드로 전환시 15로 수정해야 합니다. 2014.06.12 노준영
	m_ui8BonusTouchDownExp								= 5;

	m_eStartOption						= ROOM_START_COUNTDOWN;
	m_eRespawnOption					= ROOM_RESPAWN_NONE;
	m_eChattingOption					= ROOM_CHATTING_GHOST;
	m_eEndOption						= ROOM_END_ROUND;

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInConvoy::OnDestroy()
{
}

BOOL CRoomInConvoy::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_MODE_CONVOY;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= g_MissionRoundCountTable[ (m_pInfo->_InfoAdd._SubType & 0x0F) ];

	m_pInfo->m_eReadyOption			= ROOM_READY_BOTH_TEAM;
	m_pInfo->m_i8PersonAtLeast		= 1;
	m_pInfo->m_eJoinOption			= ROOM_JOIN_SEQUENCE;
	m_pInfo->m_ePreStartOption		= ROOM_PRESTART_ALL_WAIT;

	return TRUE;
}

BOOL CRoomInConvoy::OnUpdatePreBattle( UINT32 ui32RTState, UINT32 ui32RTServer )
{
	if( 0 == m_pInfo->_InfoAdd._NowRoundCount )
	{	// 1라운드는 탈출모드와 동일하게 HUD 팝업 대기 시간.
		if( (ui32RTState + MISSION_PREBATTLE_TIME) > ui32RTServer )
		{
			return FALSE;
		}
	}
	else
	{	// 2라운드 부터는 폭파모드와 동일하게 자동 공수교대 시간 체크
		return CRoomBase::OnUpdatePreBattle(ui32RTState, ui32RTServer);
	}
	return TRUE;
}

void CRoomInConvoy::SetMatchStart()
{
	i3mem::FillZero( m_ui8AccTouchDownCount, sizeof(UINT8)*SLOT_MAX_COUNT );
}

void CRoomInConvoy::SetRoundStart()
{

}

BOOL CRoomInConvoy::CheckRoundEnd( UINT32 ui32RTServer )
{	
	//전적 올리기
	if (TEAM_DRAW == m_pInfo->_ui8RoundWinTeam)
	{// Draw 처리
		m_pInfo->_aui16TotalRoundCount[TEAM_RED]++;
		m_pInfo->_aui16TotalRoundCount[TEAM_BLUE]++;
	}
	else
	{
		m_pInfo->_aui16TotalRoundCount[m_pInfo->_ui8RoundWinTeam]++;
	}

	return TRUE;
}

BOOL CRoomInConvoy::CheckMatchEnd()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] >= m_ui32RoundCount )	
	{
		return TRUE;
	}
	if( m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] >= m_ui32RoundCount )	
	{
		return TRUE;
	}


	return FALSE;
}

INT32 CRoomInConvoy::GetWinTeam()
{
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] > m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_RED_WIN;
	if( m_pInfo->_aui16TotalRoundCount[TEAM_RED] < m_pInfo->_aui16TotalRoundCount[TEAM_BLUE] )	return TEAM_BLUE_WIN;

	return TEAM_DRAW;
}

// 추후 정규모드로 전환시 적용예정.
INT16 CRoomInConvoy::GetMissionExp(INT32 i32SlotIdx)
{
	INT16 i16MissionExp = CRoomBase::GetMissionExp(i32SlotIdx);
	UINT8 ui8TouchDownCount = m_ui8AccTouchDownCount[i32SlotIdx];

	i16MissionExp += MIN(m_ui8BonusTouchDownMax, ui8TouchDownCount) * m_ui8BonusTouchDownExp;

	return i16MissionExp;
}

void CRoomInConvoy::GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint )
{
	CRoomBase::GetBasicExpPoint( i32Win, pstGetExpPoint );

	// 비겼을 경우 킬 카운트가 없으면 Draw 0 처리
	if( TEAM_DRAW != i32Win )							return;
	if( 0 != m_pInfo->_aTeamScoreTotal[0].m_KillCount )	return;
	if( 0 != m_pInfo->_aTeamScoreTotal[1].m_KillCount )	return;

	pstGetExpPoint->_i16DrawExp		= 0;
	pstGetExpPoint->_i16DrawPoint	= 0;
}

REAL32 CRoomInConvoy::GetSubPercentOfUnusualEnd( INT32 i32Win )
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
 
void CRoomInConvoy::AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle )
{
	pPacket->m_ui16TotalRoundCount.SetValue( m_pInfo->_aui16TotalRoundCount, 0, TEAM_COUNT );
	pPacket->m_ui16RoundStartUser	= m_pInfo->_ui16RoundStartUser;
	pPacket->m_ui8NowRoundCount		= m_pInfo->_InfoAdd._NowRoundCount;
} 

void CRoomInConvoy::GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer )
{
	pRoomNowScore->ScoreType	= ROOM_SCORE_ROUNDCOUNT_TYPE;
	pRoomNowScore->PlayTime		= 0;

	for(INT32 Loop = 0; Loop < TEAM_COUNT; Loop++)
	{
		pRoomNowScore->Score[Loop] = m_pInfo->_aui16TotalRoundCount[Loop];
	}
}

BOOL CRoomInConvoy::MissionTouchDown( CUserSession * pSession, INT32 i32SlotIdx, UINT32 ui32RTBattle )
{
	if( MISSION_STATE_BATTLE != m_pInfo->_i32MiStateType )			return FALSE;		//배틀중이 아님 
	if( (0 > i32SlotIdx) || (SLOT_MAX_COUNT <= i32SlotIdx) )	 	return FALSE;
 
	++m_ui8AccTouchDownCount[i32SlotIdx]; 

	// 클라이언트팀 요청으로 막습니다.
	INT16	i16TouchDownCount = m_ui8AccTouchDownCount[i32SlotIdx];

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_TOUCHDOWN_ACK );
	SendPacket.WriteData( m_pInfo->_aui16TotalRoundCount,		(sizeof(UINT16) * TEAM_COUNT) );
	SendPacket.WriteData( &i32SlotIdx,							sizeof(INT32));
	SendPacket.WriteData( &i16TouchDownCount,					sizeof(INT16));
	_SendPacketBattlePlayer( &SendPacket );

	// 미션 수행이 안되어야 하기때문에 일부러 FALSE로 리턴합니다
	return FALSE;	
}



void CRoomInConvoy::CheckDefaultWeapon(ITEM_INFO* aWeaponList, INT32& i32CheckSkipFlag,  INT32& i32WeaponUseStop, INT32 i32SlotIdx  ) 
{
	INT32 i32AtkDefChange = GetAtkDefChange() ? 1 : 0;

	switch( StageID::GetStageMode(m_pInfo->_InfoBasic._StageID) )
	{
	case  STAGE_MODE_CONVOY :	// 홀리데이
		{	
			switch (StageID::GetStageUID(m_pInfo->_InfoBasic._StageID))
			{
			case STAGE_UID_HOLIDAY:
			{
				for (INT32 i = 0; i < CHAR_EQUIPMENT_WEAPON_COUNT; ++i)
				{
					i32CheckSkipFlag |= 0x01 << i;
					i32WeaponUseStop |= 0x01 << i;
				}

				switch (aWeaponList[CHAR_EQUIPMENT_WEAPON_MELEE].m_TItemID)
				{
				case 301061: // Chinese Cleaver Seized
					break;
				case 301058:				// Chinese Cleaver. 판매용 중식칼 장착시 일반 중식칼로 바꿔줍니다. 기획의도.
					aWeaponList[CHAR_EQUIPMENT_WEAPON_MELEE].m_TItemID = 301061;	// Chinese Cleaver Seized
					break;
				default:
					if ((i32SlotIdx % 2) == i32AtkDefChange) aWeaponList[CHAR_EQUIPMENT_WEAPON_MELEE].m_TItemID = 301098;	//	레드팀이면 HairDryer 
					else aWeaponList[CHAR_EQUIPMENT_WEAPON_MELEE].m_TItemID = 301053;	// 블루팀이면 FryingPan
					break;
				}
			}
			default:
				break;
			}
			
		}
		break;
	}

	return;	
}

UINT16 CRoomInConvoy::GetMatchAccExp(INT32 i32SlotIdx, UINT16 ui16AccExp)
{
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	UINT16	ui16Exp = CRoomBase::GetMatchAccExp(i32SlotIdx, ui16AccExp);

	ui16Exp = ui16Exp + (MIN(m_ui8BonusTouchDownMax, m_ui8AccTouchDownCount[i32SlotIdx])  * m_ui8BonusTouchDownExp);

	return ui16Exp;
}

UINT16 CRoomInConvoy::GetMatchAccPoint(INT32 i32SlotIdx, UINT16 ui16AccPoint)
{
	TYPE_ROOM_KILL_EXPPOINT	*pstKillExpPoint;
	pstKillExpPoint = GetKillExpPoint();

	UINT16	ui16Point = CRoomBase::GetMatchAccPoint(i32SlotIdx, ui16AccPoint);

	ui16Point = ui16Point + (MIN(m_ui8BonusTouchDownMax, m_ui8AccTouchDownCount[i32SlotIdx])  * m_ui8BonusTouchDownExp);
	
	return ui16Point;
}
