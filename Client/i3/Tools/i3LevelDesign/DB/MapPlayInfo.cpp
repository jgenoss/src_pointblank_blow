#include "pch.h"
#include "MapPlayInfo.h"

CMapPlayInfo::CMapPlayInfo()
{
	m_pDB		= NULL;
	m_eDBState	= DB_STATE_NONE;
}

CMapPlayInfo::~CMapPlayInfo()
{
	Destroy();
}

BOOL CMapPlayInfo::Create( char* strConnectFile )
{
	m_pDB = new CADODatabase;
	if( FALSE == m_pDB->Connect( strConnectFile ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMapPlayInfo::Create( char* strDataSource, char* strID, char* strPW )
{
	m_pDB = new CADODatabase;
	if( FALSE == m_pDB->Connect( strDataSource, "ZLog", strID, strPW ) )
	{
		return FALSE;
	}

	return TRUE;
}

void CMapPlayInfo::Destroy()
{
	if( m_pDB )
	{
		delete m_pDB;
		m_pDB = NULL;
	}
}

BOOL CMapPlayInfo::GetRoomList( COleDateTime* ptmStart, COleDateTime* ptmEnd, INT32* pi32Count, PLAY_ROOM_LIST** pRoomList )
{
	if( FALSE == QueryRoomList( ptmStart, ptmEnd ) )			return FALSE;

	*pi32Count = GetRoomListCount();
	if( -1 == *pi32Count )										return FALSE;

	*pRoomList = new PLAY_ROOM_LIST[ *pi32Count ];
	if( FALSE == *pRoomList )									return FALSE;

	if( FALSE == GetRoomListInfo( *pRoomList, *pi32Count ) )	return FALSE;

	return TRUE;
}

BOOL CMapPlayInfo::QueryRoomList( COleDateTime* ptmStart, COleDateTime* ptmEnd, UINT32 ui32MapID )
{
	char strQuery[ 4096 ];
	strQuery[0] = '\0';
	
	// 날짜 차이 구하기
	COleDateTimeSpan tmSpan = *ptmEnd - *ptmStart;
	
	COleDateTime tmStart = *ptmStart;

	strcpy_s( strQuery, 4096, "SELECT * FROM ( " );

	char strTemp[ MAX_PATH ];
	for( int i = 0 ; i <= tmSpan.GetTotalDays() ; i++ )
	{
		sprintf_s( strTemp, MAX_PATH, "SELECT Time, lData1, iData2, iData3, iData4, iData5, iData6, iData7, iData8, iData9, iData10 FROM TZLogPB_1401_%04d_%02d_%02d", tmStart.GetYear(), tmStart.GetMonth(), tmStart.GetDay() );
		
		if( 0 < ui32MapID )
		{
			sprintf_s( strTemp, MAX_PATH, "%s iData2 = %d", strTemp, ui32MapID );
		}

		// 1일 더하기
		tmStart += 1;

		strcat_s( strQuery, 4096, strTemp );

		if( i < tmSpan.GetTotalDays() )
		{
			strcat_s( strQuery, 4096, " UNION ALL " );
		}
	}

	strcat_s( strQuery, 4096, " ) A" );
	
	if( FALSE == m_pDB->ExecuteSP( strQuery ) )
	{
		return FALSE;
	}

	m_eDBState = DB_STATE_ROOMLIST;
	return TRUE;
}

INT32 CMapPlayInfo::GetRoomListCount()
{
	if( DB_STATE_ROOMLIST != m_eDBState )	return -1;
	if (m_pDB->GetRecordCount() > 0)
		m_pDB->MoveFirst();
	return m_pDB->GetRecordCount();
}

BOOL CMapPlayInfo::GetRoomListInfo( PLAY_ROOM_LIST* pRoomList, INT32 i32BuffSize )
{
	if( DB_STATE_ROOMLIST != m_eDBState )	return FALSE;

	INT32 i32Count = 0;
	while( FALSE == m_pDB->IsEOF() )
	{
		pRoomList->m_dtTime						= m_pDB->GetDateData(0);
		pRoomList->m_ui64BattleUID				= m_pDB->GetINT64Data(1);
		pRoomList->m_ui32MapID					= m_pDB->GetINT32Data(2);
		pRoomList->m_eWinner					= (BATTLE_TEAM)m_pDB->GetINT32Data(3);
		//pRoomList->m_eStageType					= (STAGE_TYPE)m_pDB->GetINT32Data(4);
		//pRoomList->m_eStageMask					= (STAGE_MASK_TYPE)m_pDB->GetINT32Data(5);
		pRoomList->m_eIsClan					= (IS_CLAN)m_pDB->GetINT32Data(6);
		_ChangeDateToUINT32( m_pDB->GetUINT32Data(7), &pRoomList->m_dtBattleStartDate );
		pRoomList->m_ui32BattlePlayTimeToSec	= m_pDB->GetINT32Data(8);
		pRoomList->m_eRoundWinReason			= (ROUND_WIN_REASON)m_pDB->GetINT32Data(9);
		pRoomList->m_ui32RoomSetting			= m_pDB->GetINT32Data(10);
		_MakeLocation( pRoomList->m_ui64BattleUID, &pRoomList->m_Location );

		pRoomList++;
		i32Count++;

		if( i32BuffSize <= i32Count ) break;
		m_pDB->MoveNext();
	}
	return TRUE;
}

BOOL CMapPlayInfo::GetKillList( COleDateTime* ptmStart, COleDateTime* ptmEnd, UINT64& ui64BattleUID, INT32* pi32Count, PLAY_KILL_LIST** pKillList )
{
	if( FALSE == QueryKillList( ptmStart, ptmEnd, ui64BattleUID ) )	return FALSE;

	*pi32Count = GetKillListCount();
	if( -1 == *pi32Count )										return FALSE;

	*pKillList = new PLAY_KILL_LIST[ *pi32Count ];
	if( FALSE == *pKillList )									return FALSE;

	if( FALSE == GetKillListInfo( *pKillList, *pi32Count ) )	return FALSE;

	return TRUE;

}

BOOL CMapPlayInfo::QueryKillList( COleDateTime* ptmStart, COleDateTime* ptmEnd, UINT64& ui64BattleUID )
{
	char strQuery[ 4096 ];
	strQuery[0] = '\0';
	
	// 날짜 차이 구하기
	COleDateTimeSpan tmSpan = *ptmEnd - *ptmStart;
	
	COleDateTime tmStart = *ptmStart;

	strcpy_s( strQuery, 4096, "SELECT * FROM ( " );

	char strTemp[ MAX_PATH ];
	for( int i = 0 ; i <= tmSpan.GetTotalDays() ; i++ )
	{
		sprintf_s( strTemp, MAX_PATH, "SELECT Time, lData1, lData2, lData3, iData2, iData3, iData4, iData5, iData6, iData7, iData8, iData9, sData1, sData2 FROM TZLogPB_1503_%04d_%02d_%02d WHERE lData3 = %I64d", tmStart.GetYear(), tmStart.GetMonth(), tmStart.GetDay(), ui64BattleUID );
		
		// 1일 더하기
		tmStart += 1;

		strcat_s( strQuery, 4096, strTemp );

		if( i < tmSpan.GetTotalDays() )
		{
			strcat_s( strQuery, 4096, " UNION ALL " );
		}
	}

	strcat_s( strQuery, 4096, " ) A" );
	
	if( FALSE == m_pDB->ExecuteSP( strQuery ) )
	{
		return FALSE;
	}

	m_eDBState = DB_STATE_KILLINFO;

	return TRUE;
}

INT32 CMapPlayInfo::GetKillListCount()
{
	if( DB_STATE_KILLINFO != m_eDBState )	return -1;
	if( m_pDB->GetRecordCount() > 0)
		m_pDB->MoveFirst();
	return m_pDB->GetRecordCount();
}

BOOL CMapPlayInfo::GetKillListInfo( PLAY_KILL_LIST* pKillInfo, INT32 i32BuffSize )
{
	if( DB_STATE_KILLINFO != m_eDBState )	return FALSE;

	char strPos[ MAX_PATH ];
	INT32 i32Count = 0;
	while( FALSE == m_pDB->IsEOF() )
	{
		pKillInfo->m_dtTime					= m_pDB->GetDateData(0);
		pKillInfo->m_ui64KillerUID			= m_pDB->GetINT64Data(1);
		pKillInfo->m_ui64DeathUID			= m_pDB->GetINT64Data(2);
		pKillInfo->m_ui64BattleUID			= m_pDB->GetINT64Data(3);
		pKillInfo->m_ui32MapID				= m_pDB->GetINT32Data(4);
		pKillInfo->m_ui32KillerWeaponID		= m_pDB->GetINT32Data(5);
		pKillInfo->m_ui16KillerRank			= m_pDB->GetINT32Data(6);
		pKillInfo->m_ui16DeathRank			= m_pDB->GetINT32Data(7);
		pKillInfo->m_eKillerTeam			= (BATTLE_TEAM)m_pDB->GetINT32Data(8);
		pKillInfo->m_eDeathTeam				= (BATTLE_TEAM)m_pDB->GetINT32Data(9);
		pKillInfo->m_ui32PlayTime			= m_pDB->GetINT32Data(10);
		pKillInfo->m_eDeathType				= (DEATH_TYPE)m_pDB->GetINT32Data(11);
		m_pDB->GetTextData( 12, strPos, MAX_PATH );
		_ChangePosToText( &pKillInfo->m_vec3KillPos, strPos );
		m_pDB->GetTextData( 13, strPos, MAX_PATH );
		_ChangePosToText( &pKillInfo->m_vec3DeathPos, strPos );

		pKillInfo++;
		i32Count++;

		if( i32BuffSize <= i32Count ) break;
		m_pDB->MoveNext();
	}
	return TRUE;
}		

BOOL CMapPlayInfo::_ChangePosToText( D3DVECTOR* pv3Pos, char* strText )
{
	char strPos[ MAX_PATH ];
	
	int i32InputType	= 0;
	int i32Idx			= 0;
	while( *strText )
	{
		if( '/' == *strText )
		{
			strPos[ i32Idx ] = '\0';
			i32Idx = 0;

			_InsertPos( pv3Pos, strPos, i32InputType );
			i32InputType++;
		}
		else
		{
			strPos[ i32Idx ] = *strText;
			i32Idx++;
		}

		strText++;
	}
	strPos[ i32Idx ] = '\0';
	_InsertPos( pv3Pos, strPos, i32InputType );

	return TRUE;
}

void CMapPlayInfo::_InsertPos( D3DVECTOR* pv3Pos, char* strPos, int i32InputType )
{
	_CRT_FLOAT* pCrtFloat;

	switch( i32InputType )
	{
	case 0:	pCrtFloat = (_CRT_FLOAT*)&(pv3Pos->x);	break;
	case 1: pCrtFloat = (_CRT_FLOAT*)&(pv3Pos->y);	break;
	case 2: pCrtFloat = (_CRT_FLOAT*)&(pv3Pos->z);	break;
	default: return;
	}
	_atoflt( pCrtFloat, strPos );
}

void CMapPlayInfo::_ChangeDateToUINT32( UINT32 ui32Date, COleDateTime* pdtTime )
{
	pdtTime->SetDateTime( 
		(2000 + (ui32Date / 100000000)), 
		((ui32Date % 100000000)/1000000),
		((ui32Date % 1000000)/10000),
		((ui32Date % 10000)/100),
		((ui32Date % 100)),
		0 );
}

void CMapPlayInfo::_MakeLocation( UINT64 ui64BattleUID, LOCATION_INFO* pLocation )
{
	UINT32 ui32Location = (ui64BattleUID >> 32);

	pLocation->m_ui8ServerIdx	= (ui32Location >> 24 ) & 0xFF;
	pLocation->m_ui8ChannelIdx	= (ui32Location >> 16 ) & 0xFF;
	pLocation->m_ui16RoomIdx	= ui32Location & 0xFFFF;
}