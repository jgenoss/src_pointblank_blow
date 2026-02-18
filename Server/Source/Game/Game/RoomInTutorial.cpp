#include "pch.h"
#include "RoomInTutorial.h"

I3_CLASS_INSTANCE( CRoomInTutorial, CRoomBase );

CRoomInTutorial::CRoomInTutorial()
{
	m_i32BombUnInstallSlotIdx	= SERVER_VALUE_UNKNOWN;
	m_i32BombInstallSlotIdx		= SERVER_VALUE_UNKNOWN;
	m_ui32BombTime				= 42;		//Temp
}

CRoomInTutorial::~CRoomInTutorial()
{
	OnDestroy();
}

BOOL CRoomInTutorial::OnCreate( TYPE_ROOM_INFO* pRoomInfo )
{
	m_eStartOption						= ROOM_START_DIRECT;
	m_eRespawnOption					= ROOM_RESPAWN_CONTINUE;
	m_eChattingOption					= ROOM_CHATTING_ALL;
	m_eEndOption						= ROOM_END_MOD;

	m_pr32SubUserCount					= new REAL32*[ SLOT_MAX_COUNT + 1 ];
	for( INT32 i = 1 ; i < SLOT_MAX_COUNT + 1 ; i++ )
	{
		m_pr32SubUserCount[i]			= new REAL32[ SLOT_MAX_COUNT ];
		i3mem::FillZero( m_pr32SubUserCount[i], sizeof(REAL32)*SLOT_MAX_COUNT );
	}

	REAL32 pr32SubUserCount[SLOT_MAX_COUNT]			= { 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f};
	REAL32 pr32SubUserCountVS4[SLOT_MAX_COUNT]		= { 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, 1.01f, };

	i3mem::Copy( m_pr32SubUserCount[ 16 ],		pr32SubUserCount,		sizeof(REAL32)*SLOT_MAX_COUNT );
	i3mem::Copy( m_pr32SubUserCount[ 8 ],		pr32SubUserCountVS4,	sizeof(REAL32)*SLOT_MAX_COUNT );

	return CRoomBase::OnCreate( pRoomInfo );
}

void CRoomInTutorial::OnDestroy()
{
}

BOOL CRoomInTutorial::SetRoomOption( UINT32 ui32StageID )
{
	m_pInfo->_i32StageType			= STAGE_MODE_TUTORIAL;
	m_pInfo->_InfoBasic._StageID	= ui32StageID;

	m_ui32RoundCount				= 2;

	m_i32BombUnInstallSlotIdx		= SERVER_VALUE_UNKNOWN;
	m_i32BombInstallSlotIdx			= SERVER_VALUE_UNKNOWN;
	m_bRoundEnd						= FALSE;
	m_bMatchEnd						= FALSE;

	m_bIsFirst						= FALSE;

	m_pInfo->m_eReadyOption			= ROOM_READY_ONE_TEAM;
	m_pInfo->m_i8PersonAtLeast		= 1;
	m_pInfo->m_eJoinOption			= ROOM_JOIN_IMPOSSIBLE;
	m_pInfo->m_ePreStartOption		= ROOM_PRESTART_DIRECT;

	return TRUE;
}

BOOL CRoomInTutorial::CheckRoundEnd( UINT32 ui32RTServer )
{
	m_pInfo->_InfoAdd._NowIngTime = (UINT16)(ui32RTServer - m_pInfo->_ui32RTRoundStart);
	INT32 i32SlotIdx = m_pInfo->_i32MainSlotIdx;

	if( m_bRoundEnd	)
	{
		if( m_pInfo->_aSlotInfo[i32SlotIdx]._State != SLOT_STATE_BATTLE ) return FALSE;

		m_bRoundEnd = FALSE;

		m_pInfo->_ui8MiEndType = MISSION_END_TUTORIAL_COMPLETE;
		return TRUE;
	}
	UINT8 ui8EndTimeIdx	= (m_pInfo->_InfoAdd._SubType & 0xF0) >> 4; 
	if( m_pInfo->_InfoAdd._NowIngTime > g_TeamTimeTable[ui8EndTimeIdx] )
	{
		// 시간이 지나도록 못 깼으면 로비로 나간다
		m_pInfo->_ui8MiEndType = MISSION_END_TIMEOUT;
		m_bMatchEnd = TRUE;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CRoomInTutorial::CheckMatchEnd()
{
	INT32 i32SlotIdx = m_pInfo->_i32MainSlotIdx;

	if( m_bMatchEnd )
	{
		if( m_pInfo->_aSlotInfo[i32SlotIdx]._State != SLOT_STATE_BATTLE ) return FALSE;

		m_bMatchEnd = FALSE;

		return TRUE;
	}
	if(  m_pInfo->_InfoAdd._NowRoundCount >= m_ui32RoundCount )
		return TRUE;

	return FALSE;
}

INT32 CRoomInTutorial::GetWinTeam()
{
	// 방장의 팀이 이김
	return m_pInfo->_i32MainSlotIdx % 2;
}

void CRoomInTutorial::GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint )
{
	CRoomBase::GetBasicExpPoint( i32Win, pstGetExpPoint );
}

REAL32 CRoomInTutorial::GetSubPercentOfUnusualEnd( INT32 i32Win )
{
	REAL32 r32SubPercent = 1.f;
	
	return r32SubPercent;
}

UINT16 CRoomInTutorial::GetMatchEndExp( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle )
{
	if( m_bIsFirst )
	{
		if( m_pInfo->_InfoAdd._NowIngTime < 60 )			return 150;
		else if ( m_pInfo->_InfoAdd._NowIngTime < 90 )		return 100;
		else												return 50;
	}
	else
	{
		return 10;
	}
}

UINT16 CRoomInTutorial::GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle )
{
	if( m_bIsFirst )
	{
		if( m_pInfo->_InfoAdd._NowIngTime < 60 )			return 150;
		else if ( m_pInfo->_InfoAdd._NowIngTime < 90 )		return 100;
		else												return 50;
	}
	else
	{
		return 10;
	}
}

BOOL CRoomInTutorial::MissionBombInstall( INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos, UINT32 ui32RTBattle )
{
	if( SERVER_VALUE_UNKNOWN	!= m_i32BombInstallSlotIdx )return FALSE;

	m_i32BombInstallSlotIdx	= i32SlotIdx; 
	m_ui32RTBombInstall		= ui32RTBattle;

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_BOMB_INSTALL_ACK ); 
	SendPacket.WriteData( &m_i32BombInstallSlotIdx,	sizeof(INT32) );
	SendPacket.WriteData( &ui8BombPosition,			sizeof(UINT8) );
	SendPacket.WriteData( &m_ui32BombTime,			sizeof(UINT16));
	SendPacket.WriteData( pr32BombVecPos,			sizeof(REAL32) * 3);	// 2009.06.22 정순구 C4 설치 위치 추가
    _SendPacketBattlePlayer( &SendPacket );

	return TRUE;
}

BOOL CRoomInTutorial::MissionBombUnInstall( INT32 i32SlotIdx )
{
	if( SERVER_VALUE_UNKNOWN	== m_i32BombInstallSlotIdx )				return FALSE;

	m_i32BombInstallSlotIdx		= SERVER_VALUE_UNKNOWN; 
	m_i32BombUnInstallSlotIdx	= i32SlotIdx;

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_BOMB_UNINSTALL_ACK ); 
	SendPacket.WriteData(&m_i32BombUnInstallSlotIdx,	sizeof(INT32) );	
    _SendPacketBattlePlayer( &SendPacket );

	return TRUE;
}

BOOL CRoomInTutorial::MissionTutorialRoundEnd( CUserSession* pSession)
{
	if( m_pInfo->_i32MainSlotIdx != pSession->m_i32SlotIdx ) return FALSE;

	switch( pSession->m_UserInfoBasic.m_ui8TutorialIng & 0x01 )
	{
	case TUTORIAL_TRAINING:
		m_bRoundEnd = TRUE;
		m_bMatchEnd = TRUE;
		break;
	case TUTORIAL_TIMEATTACK:
		m_bRoundEnd = TRUE;
		m_bMatchEnd = TRUE;
		break;
	}

	if( (pSession->m_UserInfoBasic.m_ui8TutorialIng & 0x01) == 0 )
	{
		// 경험치, 포인트 지급할 위치
		
		
		 pSession->m_UserInfoBasic.m_ui8TutorialIng |= 0x01;
		 pSession->ChangeDataBaseInfo(SAVE_DATABASE_BASEINFO);

		 m_bIsFirst = TRUE;		 
	}

	i3NetworkPacket SendPacket( PROTOCOL_BATTLE_MISSION_TUTORIAL_ROUND_END_ACK );
	SendPacket.WriteData( &pSession->m_UserInfoBasic.m_ui8TutorialIng, sizeof(UINT8) );
	SendPacket.WriteData( &m_pInfo->_InfoAdd._NowIngTime, sizeof(UINT16) );
	pSession->SendPacketMessage( &SendPacket );

	return TRUE;
}
