#include "pch.h"
#include "GeneralRankup.h"

I3_CLASS_INSTANCE( CGeneralRankUP, i3Thread );

CGeneralRankUP::CGeneralRankUP(void)
{
	m_bIsRunning		= FALSE;
	m_pUserDBConfig		= NULL;
	m_pDB				= NULL;
	m_pGeneralRankData	= NULL;	

	m_i32StartHour		= 0;
	m_i32RunDay			= 0;
}

CGeneralRankUP::~CGeneralRankUP(void)
{
	OnDestroy(); 
}

void CGeneralRankUP::OnDestroy(void)
{
	m_bIsRunning = FALSE;
	i3Thread::WaitForFinish();

	//Release	
	I3MEM_SAFE_FREE(m_pGeneralRankData);
	I3_SAFE_DELETE(m_pOutBuffer);
	I3_SAFE_RELEASE( m_pDB ); 
}

BOOL CGeneralRankUP::OnCreate()
{	
	if( false == g_pConfig->IsContentsEnabled( CONTENTS_RANKING ) )	return TRUE;

	switch( g_pConfig->m_eServerVersion )
	{
	case PB_VER_V10:
		i3mem::Copy( m_ExpValue, g_ExpValueV1, sizeof(UINT32) * DEF_RANK_COUNT );
		break;
	default:
		i3mem::Copy( m_ExpValue, g_ExpValueV2, sizeof(UINT32) * DEF_RANK_COUNT );
		break;
	}
	
	// 장성 랭크업은 Trans서버 버전 기준으로 갑니다.( 유저 기준 X )
	for( INT32 i = 0 ; i < GENERAL_RANK_COUNT ; i++ )
	{
		m_ui32Count[ i ]	= g_pConfig->m_i32GeneralRank[ i ];
		m_ui32Exp[ i ]		= m_ExpValue[ MIN_LIMIT_RANK + i ];
		m_ui32Rank[ i ]		= MIN_LIMIT_RANK + i;
	}

	m_i32StartHour					= g_pConfig->m_i32GeneralRankStartTime;

	m_ui32TotalRankCount = 0;
	for( INT32 i = 0 ; i < GENERAL_RANK_COUNT ; i++ )
	{
		m_ui32TotalRankCount		+= m_ui32Count[ i ];
	}

	m_eRunState						= GENERAL_RANK_STATE_WAIT;

	// 메모리에 담을 RankData 생성
	m_pGeneralRankData	= (GENERAL_RANK_DATA*)i3MemAlloc( sizeof(GENERAL_RANK_DATA) * m_ui32TotalRankCount );
	if( NULL == m_pGeneralRankData )			return FALSE;
		
	m_pUserDBConfig = g_pConfig->GetDatabaseConfig( "PBUser" );
	if( FALSE == m_pUserDBConfig )				return FALSE;

	//1. DataBase Connect 
	m_pDB = CADODatabase::NewObject(); 
	if( NULL == m_pDB )							return FALSE;
	if( FALSE == m_pDB->Connect( m_pUserDBConfig->GetDataSource(), m_pUserDBConfig->GetCatalog(), m_pUserDBConfig->GetAccount(), m_pUserDBConfig->GetPassword()) )
	{
		g_pLog->WriteLog( L"[CGeneralRankUP::OnCreate] PBUser Connect Failed"); 
		return FALSE; 
	}

	// 작업시간을 체크하는 스레드 생성
	if( FALSE == i3Thread::Create("CGeneralRankUP", 0, 4096, NULL, PRIORITY_NORMAL) )
	{
		g_pLog->WriteLog( L"[CGeneralRankUP::OnCreate] i3Thread Failed"); 
		return FALSE;
	}

	//----------------------------------------------------
	//Create Buffer 
	m_pOutBuffer	= new S2RingBuffer;
	if( NULL == m_pOutBuffer )																		return false;
	if( false == m_pOutBuffer->Create( sizeof(GENERAL_RANK_RESULT), m_ui32TotalRankCount * 2) )		return false;

	return TRUE; 
}

UINT32 CGeneralRankUP::OnRunning( void * pUserData)
{
	g_pLog->AddLogThread( L"GERU", 0, m_ThreadID );

	BOOL bContinue;

	UINT32 ui32DealyTime;
	m_bIsRunning = TRUE;
	while( m_bIsRunning )
	{
		ui32DealyTime	= 1;
		switch( m_eRunState )
		{
		case GENERAL_RANK_STATE_WAIT:
			{
				if( (m_i32StartHour	!= i3ThreadTimer::LocalTime().GetHour()) ||		// 아직 시간이 되지 않았거나
					(m_i32RunDay	== i3ThreadTimer::LocalTime().GetDay()) )		// 금일 이미 수행 했을 경우 continue
				{
					ui32DealyTime = 1000;
					break;		
				}
				m_i32RunDay		= i3ThreadTimer::LocalTime().GetDay();
				m_eRunState		= GENERAL_RANK_STATE_GET_DB;
			}
			break;
		case GENERAL_RANK_STATE_GET_DB:
			{
				m_i32GeneralRankDataCount	= 0;
				m_i32UpdateGeneralRankIdx	= 0;

				g_pLog->WriteLog(L"[CGeneralRankUP] Start");	// General Rank 변경 시작
				if( _GetGeneralRankUser() )
				{
					m_eRunState = GENERAL_RANK_STATE_READ;
				}
				else
				{
					g_pLog->WriteLog( L"[EROR] _GetGeneralRankUser Fail" );
					m_eRunState = GENERAL_RANK_STATE_WAIT;
				}
			}
			break;
		case GENERAL_RANK_STATE_READ:
			{
				_ReadGeneralRankUser( bContinue );
				if( FALSE == bContinue )
				{
					m_eRunState = GENERAL_RANK_STATE_CHECK_RANK;
				}
			}
			break;
		case GENERAL_RANK_STATE_CHECK_RANK:
			{
				_CheckGeneral();
				m_eRunState = GENERAL_RANK_STATE_READ_DEMOTION;
			}
			break;
		case GENERAL_RANK_STATE_READ_DEMOTION:
			{
				if( _GetDemotionGeneralUser() )
				{
					m_eRunState = GENERAL_RANK_STATE_WRITE_DEMOTION_RANK;
				}
				else
				{
					g_pLog->WriteLog( L"[EROR] _GetDemotionGeneralUser Fail" );
					m_eRunState = GENERAL_RANK_STATE_WAIT;
				}
			}
			break;
		case GENERAL_RANK_STATE_WRITE_DEMOTION_RANK:
			{
				if( FALSE == _WriteGeneralDemotion( bContinue ) )
				{
					ui32DealyTime = 1000;
					break;
				}

				if( FALSE == bContinue )
				{
					m_eRunState = GENERAL_RANK_STATE_WRITE_BRIGADIER_RANK;
				}
			}
			break;
		case GENERAL_RANK_STATE_WRITE_BRIGADIER_RANK:
			{
				if( _UpdateBrigadierRank() )
				{
					m_eRunState		= GENERAL_RANK_STATE_WRITE;
					m_i32WriteIdx	= 0;
				}
				else
				{
					g_pLog->WriteLog( L"[EROR] _UpdateBrigadierRank Fail" );
					m_eRunState = GENERAL_RANK_STATE_WAIT;
				}
			}
			break;
		case GENERAL_RANK_STATE_WRITE:
			{
				if( FALSE == _UpdateRankUP( bContinue ) )
				{
					g_pLog->WriteLog( L"[EROR] _UpdateBrigadierRank Fail" );
					m_eRunState = GENERAL_RANK_STATE_WAIT;
					break;
				}
				if( FALSE == bContinue )
				{
					g_pLog->WriteLog(L"[CGeneralRankUP] End");	// General Rank 변경 완료
					m_eRunState = GENERAL_RANK_STATE_WAIT;
				}
			}
			break;
		}

		::WaitForSingleObject( m_Handle, ui32DealyTime );
	}
	return 0; 
}

BOOL CGeneralRankUP::_GetGeneralRankUser()
{
	wchar_t	wstrQuery[ SQL_STRING_COUNT ];
	
	S2DB::MakeQueryW( wstrQuery, L"usp_Basic_sel_General",
		DB_DATA_TYPE_INT,			m_ui32TotalRankCount,
		DB_DATA_TYPE_INT,			m_ExpValue[ MIN_LIMIT_RANK ],
		DB_DATA_TYPE_INT,			m_ExpValue[ MAX_LIMIT_RANK ],
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrQuery ) )
	{
		ErrorDBRankUP( wstrQuery );
		return FALSE; 
	}
	
	if( 0 == m_pDB->GetRecordCount() )
	{
		return TRUE;
	}
	m_pDB->MoveFirst();

	return TRUE;
}

void CGeneralRankUP::_ReadGeneralRankUser( BOOL& bContinue )
{
	if( m_pDB->IsEOF() )
	{
		bContinue = FALSE;
		return;
	}
	
	GENERAL_RANK_DATA* pRankData = &m_pGeneralRankData[ m_i32GeneralRankDataCount ];

	pRankData->m_TUID				= m_pDB->GetINT64Data(0);
	pRankData->m_ui32Exp			= m_pDB->GetINT32Data(1);
	pRankData->m_ui32Rank			= m_pDB->GetINT32Data(2);
	m_i32GeneralRankDataCount++;

	m_pDB->MoveNext();

	bContinue = TRUE;
}

BOOL CGeneralRankUP::_CheckGeneral()
{
	INT32	i32RankIdx		= GENERAL_RANK_50;
	UINT32	ui32RankCount	= 0;
	BOOL	bContinue		= TRUE;

	for( INT32 i = 0 ; i < m_i32GeneralRankDataCount ; i++ )
	{
		if( m_ui32Count[ i32RankIdx ] <= ui32RankCount )
		{
			// 인원수에서 넘침
			g_pLog->WriteLog(L"[_CheckGeneral] Rank : %d, Count : %d", i32RankIdx + MIN_LIMIT_RANK, ui32RankCount);
			ui32RankCount = 0;
			i32RankIdx--;
			if( 0 > i32RankIdx )	break;
		}
		while( m_pGeneralRankData[ i ].m_ui32Exp < m_ui32Exp[ i32RankIdx ] )
		{
			// Exp가 딸림.
			g_pLog->WriteLog(L"[_CheckGeneral] Rank : %d, Count : %d", i32RankIdx + MIN_LIMIT_RANK, ui32RankCount);
			ui32RankCount = 0;
			i32RankIdx--;
			if( 0 > i32RankIdx ) bContinue = FALSE;;
		}

		if( FALSE == bContinue )	break;

		ui32RankCount++;
		m_i32UpdateGeneralRankIdx++;
		m_pGeneralRankData[ i ].m_ui32ChangeRank = m_ui32Rank[ i32RankIdx ];
	}

	// 마지막 체크 같으면 최대 인원 안에 들어서 중간에 나온 경우입니다.
	if (m_i32UpdateGeneralRankIdx == m_i32GeneralRankDataCount)
		g_pLog->WriteLog(L"[_CheckGeneral] Rank : %d, Count : %d", i32RankIdx + MIN_LIMIT_RANK, ui32RankCount);

	return TRUE;
}

BOOL CGeneralRankUP::_GetDemotionGeneralUser()
{
	wchar_t	wstrQuery[ SQL_STRING_COUNT ];
	
	S2DB::MakeQueryW( wstrQuery, L"usp_UserInfo_sel_DemotionGeneralUser",					
		DB_DATA_TYPE_INT,			m_pGeneralRankData[ m_i32UpdateGeneralRankIdx-1 ].m_ui32Exp,
		DB_DATA_TYPE_INT,			RANK_46,
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrQuery ) )
	{
		ErrorDBRankUP( wstrQuery );
		return FALSE; 
	}
	if( 0 == m_pDB->GetRecordCount() )
	{
		return TRUE;
	}
	m_pDB->MoveFirst();
	
	return TRUE;
}
	
BOOL CGeneralRankUP::_WriteGeneralDemotion( BOOL& bContinue )
{
	GENERAL_RANK_RESULT*	pResult = (GENERAL_RANK_RESULT*)m_pOutBuffer->PushPointer();
	if( NULL == pResult ) 
	{
		g_pLog->WriteLog( L"[CGeneralRankUP::_WriteGeneralDemotion]GENERAL_RESULT_BUFFER_FULL");
		return FALSE;
	}

	bContinue = FALSE;	

	if( m_pDB->IsEOF() ) return TRUE;

	pResult->m_TUID				= m_pDB->GetINT64Data(0);
	pResult->m_ui32ChangeRank	= RANK_46;

	m_pOutBuffer->PushPointerIdx();

	m_pDB->MoveNext();

	bContinue = TRUE;
	
	return TRUE;

}

BOOL CGeneralRankUP::_UpdateBrigadierRank()
{
	wchar_t	wstrQuery[ SQL_STRING_COUNT ];
	S2DB::MakeQueryW( wstrQuery, L"usp_Basic_upd_Brigadier",
		DB_DATA_TYPE_INT,			m_ExpValue[ MIN_LIMIT_RANK ],
		DB_DATA_TYPE_INT,			m_pGeneralRankData[ m_i32UpdateGeneralRankIdx-1 ].m_ui32Exp,
		DB_DATA_TYPE_INT,			RANK_46,
		DB_DATA_TYPE_END );
	if( FALSE == m_pDB->ExecuteSP( wstrQuery ) )
	{
		ErrorDBRankUP( wstrQuery );
		return FALSE; 
	}	
	if( 1 != m_pDB->GetRecordCount() )
	{
		g_pLog->WriteLog( wstrQuery );
		return FALSE;
	}	
	if( 0 != m_pDB->GetINT32Data(0))
	{
		g_pLog->WriteLog( wstrQuery );
		return FALSE;
	}

	return TRUE;
}

BOOL CGeneralRankUP::_UpdateRankUP( BOOL& bContinue )
{
	wchar_t	wstrQuery[ SQL_STRING_COUNT ];

	GENERAL_RANK_RESULT*	pResult = (GENERAL_RANK_RESULT*)m_pOutBuffer->PushPointer();
	if( NULL == pResult )
	{
		g_pLog->WriteLog( L"[CGeneralRankUP::_UpdateRankUP]GENERAL_RESULT_BUFFER_FULL");
		return EVENT_ERROR_GENERAL_RANKUP_OUT_BUFFER_FULL;
	}
	
	bContinue = TRUE;

	GENERAL_RANK_DATA* pRankData = &m_pGeneralRankData[ m_i32WriteIdx ];

	if( pRankData->m_ui32Rank != pRankData->m_ui32ChangeRank )
	{
		S2DB::MakeQueryW( wstrQuery, L"usp_Basic_upd_General",
			DB_DATA_TYPE_BIGINT,		pRankData->m_TUID,
			DB_DATA_TYPE_INT,			pRankData->m_ui32ChangeRank,
			DB_DATA_TYPE_END );
		if( FALSE == m_pDB->ExecuteSP( wstrQuery ) )
		{
			ErrorDBRankUP( wstrQuery );
			return FALSE; 
		}
		if( 1 != m_pDB->GetRecordCount() )
		{
			g_pLog->WriteLog( wstrQuery );
			return FALSE;
		}
		if( 0 != m_pDB->GetINT32Data(0))
		{
			g_pLog->WriteLog( wstrQuery );
			return FALSE;
		}

		pResult->m_TUID				= pRankData->m_TUID;
		pResult->m_ui32ChangeRank	= pRankData->m_ui32ChangeRank;

		m_pOutBuffer->PushPointerIdx();
	}
	
	m_i32WriteIdx++;
	if( m_i32WriteIdx >= m_i32UpdateGeneralRankIdx )
	{
		bContinue = FALSE;
	}

	return TRUE;
}

void CGeneralRankUP::ErrorDBRankUP( wchar_t* wstrLog )
{
	g_pLog->WriteLog( L"[ErrorDBRankUP] %s, Error Code : %08x", wstrLog, m_pDB->GethrLastError() );
	
	HRESULT EventType = m_pDB->ReConnect(
																	m_pUserDBConfig->GetDataSource(),
																	m_pUserDBConfig->GetCatalog(),
																	m_pUserDBConfig->GetAccount(),
																	m_pUserDBConfig->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBRankUP] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBRankUP] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CGeneralRankUP::SetStartHour(INT32 starthour)
{
	if (starthour < 0 || 24 < starthour) return;

	m_i32StartHour = starthour;

	// 이걸 초기화 해줘야 실시간으로 바꿀 타이밍에 바뀐다.
	// 미리 돌아버린 날이면 시간을 바꿔도 안돌기 때문에.
	m_i32RunDay = 0;
}
