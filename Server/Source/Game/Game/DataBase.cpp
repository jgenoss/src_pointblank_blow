#include "pch.h"
#include "DataBase.h"
#include "UserSession.h"
//#include "Ado.h"
// #include "ModuleLogFile.h"
#include "ModuleDataBase.h"

CDataBase * g_pDataBase = NULL;

#ifdef I3_DEBUG
#define DATABASE_BUFFER						50
#else
#define DATABASE_BUFFER						500
#endif

I3_CLASS_INSTANCE( CDataBase, i3Thread);

CDataBase::CDataBase(void)
{
	m_bRunning		= FALSE;
}

CDataBase::~CDataBase(void)
{
	OnDestroy(); 
}

BOOL CDataBase::OnCreate(INT32 i32WorkThreadCount, char * pstrUdlFilePath)
{	
	//////////////////////////////////////////////////////////////////////////////////////
	//1. DataBase Connect 
	m_i32WorkThreadCount = i32WorkThreadCount;
	// Option Thread 용

	m_pInfoDBConfig = g_pContextMain->GetDatabaseConfig("PBInfo");
	if( !m_pInfoDBConfig )
		return FALSE;

	m_DBUser._pDB	= CADODatabase::NewObject(); 
	if( !m_DBUser._pDB->Connect(m_pInfoDBConfig->GetDataSource(), m_pInfoDBConfig->GetCatalog(), m_pInfoDBConfig->GetAccount(), m_pInfoDBConfig->GetPassword()) )
	{
		g_pLog->WriteLog( L"[CDataBase::OnCreate] User DB Connect Fail"); 
		return FALSE; 
	}

	m_ppRingIn = new NSM_CRingBuffer*[ m_i32WorkThreadCount ];
	for( INT32 i = 0 ; i < m_i32WorkThreadCount ; i++ )
	{
		m_ppRingIn[i] = new NSM_CRingBuffer;
		m_ppRingIn[i]->OnCreate( sizeof(TYPE_RING_IN), DATABASE_BUFFER);
	}
	m_pRingOut = new NSM_CRingBuffer;
	m_pRingOut->OnCreate( sizeof(TYPE_RING_OUT), DATABASE_BUFFER);

	if( !i3Thread::Create("DataBase", 0, 4096, NULL, PRIORITY_NORMAL) )		return FALSE;
	
	g_pLog->WriteLog( L"[CDataBase::OnCreate] DataBase Connect Success"); 
	return TRUE; 
}

void CDataBase::OnDestroy(void)
{	
	//Release & Disconnect DataBase
	m_bRunning = FALSE;
	i3Thread::WaitForFinish();

	if( m_ppRingIn )
	{
		for( INT32 i = 0 ; i < m_i32WorkThreadCount ; i++ )
		{
			I3_SAFE_DELETE( m_ppRingIn[ i ] );
		}
		I3_SAFE_DELETE_ARRAY( m_ppRingIn );
	}
	I3_SAFE_DELETE( m_pRingOut );

	I3_SAFE_RELEASE( m_DBUser._pDB );

	return; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT32 CDataBase::OnRunning( void * pUserData)
{
	g_pLog->AddLogThread( L"DB", 0, m_ThreadID ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

	m_bRunning		= TRUE;

	INT32 i32WorkingCount;
	INT32 i32Count;
	INT32 i, j;

	while( m_bRunning )
	{
		i32WorkingCount	= 0;
		for( i = 0 ; i < m_i32WorkThreadCount ; i++ )
		{
			i32Count = m_ppRingIn[ i ]->GetBufferCount();
			for( j = 0 ; j < i32Count ; j++ )
			{			
				TYPE_RING_IN* pIn = (TYPE_RING_IN*)m_ppRingIn[ i ]->Pop();

				DWORD dwOldTime = GetTickCount();

				switch( pIn->_ui8Type )
				{
				case RING_TYPE_OPTION_SET:
					{
						SetOption( pIn );
					}
					break;
				case RING_TYPE_OPTION_GET:
					{
						TYPE_RING_OUT RingOut;
						GetOption( pIn, &RingOut );
						m_pRingOut->Push( &RingOut );
					}
					break;
				}

				DWORD dwNewTime = GetTickCount() - dwOldTime;
				if( dwNewTime > DEF_DB_LIMIT_TIME )
				{	// DB TIMEOVER
					g_pLog->WriteLog( L"[CDataBase::OnRunning()] DB Time Over : %d,  pIn->_ui8Type : %d ", dwNewTime, pIn->_ui8Type ); 
				}

				m_ppRingIn[ i ]->PopIdx();
			}
			i32WorkingCount += i32Count;
		}

		if( 0 == i32WorkingCount ) 
		{	// 한일이 없으면 1ms 휴식.
			::WaitForSingleObject( m_Handle, 1 );
		}
	}

	return 0;
}

INT32 CDataBase::RingInputDB( INT32 i32WorkThread, TYPE_RING_IN* pIn )
{
	return m_ppRingIn[ i32WorkThread ]->Push( pIn );
}

void CDataBase::SetOption( TYPE_RING_IN* pIn )
{
	DB_QUERY_INFO * pDataBaseInfo = &m_DBUser;

	if( 0x10000000 & pIn->_ui32ChangeFlag )
	{	// 0x10000000 일때는 Default 로 변경 합니다.
		S2DB::MakeQueryW( pDataBaseInfo->_pQuery, L"usp_Environment_ins",
			DB_DATA_TYPE_BIGINT,			pIn->_i64UID,
			DB_DATA_TYPE_TINYINT,			ENV_SETTING_DEFAULT,
			DB_DATA_TYPE_END);
	}
	else
	{
		wchar_t wstrTemp[ SQL_STRING_COUNT ];
		wchar_t	wstrBinaryTemp[ SQL_STRING_COUNT ];
		wchar_t	wstrConvert[CONVERT_STRING_COUNT];

		// 옵션이 변경 되었다고 저장합니다.
		// 저장할 것들만 정리하여 한번에 Update 합니다.
		S2DB::MakeQueryW( pDataBaseInfo->_pQuery, L"usp_Environment_ins",
			DB_DATA_TYPE_BIGINT,			pIn->_i64UID,
			DB_DATA_TYPE_TINYINT,			ENV_SETTING_CHANGE,
			DB_DATA_TYPE_END);

		// ENV Set
		if( 0x00000001 & pIn->_ui32ChangeFlag )
		{	// 유저 정보를 저장합니다.			
			S2DB::ConvertStringToBinary( wstrBinaryTemp, SQL_STRING_COUNT, (char*)&pIn->_NetEnvSet, sizeof(NET_ENV_SET) );									// 바이너리로 변환
			i3String::Format( wstrTemp, SQL_STRING_COUNT, L",'%s'", wstrBinaryTemp );
		}
		else
		{
			i3String::Copy( wstrTemp, L",NULL", SQL_STRING_COUNT );			
		}
		i3String::Concat( pDataBaseInfo->_pQuery, wstrTemp );

		// Key
		if( 0x00000002 & pIn->_ui32ChangeFlag )
		{	// 키 정보를 저장합니다.
			S2DB::ConvertStringToBinary( wstrBinaryTemp, SQL_STRING_COUNT, (char*)pIn->_KeyMapArray, (sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT) );		// 바이너리로 변환
			i3String::Format( wstrTemp, SQL_STRING_COUNT, L",'%s'", wstrBinaryTemp );
		}
		else
		{
			i3String::Copy( wstrTemp, L",NULL", SQL_STRING_COUNT );
		}
		i3String::Concat( pDataBaseInfo->_pQuery, wstrTemp );

		if( 0x00000004 & pIn->_ui32ChangeFlag )
		{	//메크로를 저장합니다.
			for(INT32 i = 0; i < MACRO_MAX_COUNT; i++)
			{	
				S2DB::ConvertString( wstrConvert, CONVERT_STRING_COUNT, pIn->_szMacro[i] );

				i3String::Format( wstrTemp, SQL_STRING_COUNT, L",N'%s'", wstrConvert );

				i3String::Concat( pDataBaseInfo->_pQuery, wstrTemp );
			}
		}
	}
	if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
	{
		ErrorDB();
		return; 
	}
	if(1 != pDataBaseInfo->_pDB->GetRecordCount())
	{
		ReConnectDataLog();
	}
	INT32 ECode = pDataBaseInfo->_pDB->GetINT32Data(0);

	if(1 != ECode)
	{
		ReConnectDataLog();
	}
}

void CDataBase::GetOption( TYPE_RING_IN* pIn, TYPE_RING_OUT* pOut )
{
	//DB select
	DB_QUERY_INFO * pDataBaseInfo = &m_DBUser;

	// 기본 넣어줘야하는 정보
	pOut->_ui8DefaultKey	= ENV_SETTING_DEFAULT;
	pOut->_i64UID			= pIn->_i64UID;
	pOut->_i32SessionIdx	= pIn->_i32SessionIdx;

	INT32 i32Idx = (INT32)(pIn->_i64UID % INVENTORY_TABLE_COUNT);

	S2DB::MakeQueryW( pDataBaseInfo->_pQuery, L"usp_Environment_sel_Envdefault",
			DB_DATA_TYPE_INT,				i32Idx,
			DB_DATA_TYPE_BIGINT,			pIn->_i64UID,
			DB_DATA_TYPE_END);

	if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
	{
		ErrorDB();
		return; 
	}
	if( 1 == pDataBaseInfo->_pDB->GetRecordCount() )
	{
		// 키 옵션 세팅
		pOut->_ui8DefaultKey = pDataBaseInfo->_pDB->GetUINT8Data(0);

		//Get Key Option Data
		if( ENV_SETTING_DEFAULT == pOut->_ui8DefaultKey )		//Set Default Key //아직 아무것도 할 필요없다. 
			return;

		// 변경된 옵션이 있다.
		S2DB::MakeQueryW( pDataBaseInfo->_pQuery, L"usp_Environment_sel",
			DB_DATA_TYPE_INT,				i32Idx,
			DB_DATA_TYPE_BIGINT,			pIn->_i64UID,
			DB_DATA_TYPE_END);
		if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
		{
			ErrorDB();
			return;
		}
		if(1 != pDataBaseInfo->_pDB->GetRecordCount())
		{
			ReConnectDataLog();
		}

		//Set Key Option 
		pDataBaseInfo->_pDB->GetBinaryData(0, &pOut->_NetEnvSet,	sizeof(NET_ENV_SET));
		pDataBaseInfo->_pDB->GetBinaryData(1, &pOut->_KeyMapArray,	sizeof(GAME_KEY_STRUCT)*GAME_KEY_MAP_COUNT);

		//Set Macro 
		pDataBaseInfo->_pDB->GetTextData(2, pOut->_szMacro[0] , 256);
		pDataBaseInfo->_pDB->GetTextData(3, pOut->_szMacro[1] , 256);
		pDataBaseInfo->_pDB->GetTextData(4, pOut->_szMacro[2] , 256);
		pDataBaseInfo->_pDB->GetTextData(5, pOut->_szMacro[3] , 256);
		pDataBaseInfo->_pDB->GetTextData(6, pOut->_szMacro[4] , 256);
	}
	else
	{
		S2DB::MakeQueryW( pDataBaseInfo->_pQuery, L"usp_Environment_ins_UID",
			DB_DATA_TYPE_INT,				i32Idx,
			DB_DATA_TYPE_BIGINT,			pIn->_i64UID,
			DB_DATA_TYPE_END);
		if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
		{
			ErrorDB();
			return;
		}
		if(1 != pDataBaseInfo->_pDB->GetRecordCount())
		{
			ReConnectDataLog();
			return;
		}
		if(1 != pDataBaseInfo->_pDB->GetINT32Data(0))
		{
			ReConnectDataLog();
			return;
		}
	}
}

void CDataBase::ErrorDB( void )
{
	g_pLog->WriteLog( L"[ErrorDB] %s, Error Code : %08x", m_DBUser._pQuery, m_DBUser._pDB->GethrLastError() ); 
	HRESULT EventType = m_DBUser._pDB->ReConnect(
																	m_pInfoDBConfig->GetDataSource(),
																	m_pInfoDBConfig->GetCatalog(),
																	m_pInfoDBConfig->GetAccount(),
																	m_pInfoDBConfig->GetPassword()
																);
	
	switch( EventType )
	{
	case EVENT_ERROR_EVENT_LOG_IN_NOT_CONNECT_DB:		// 리커넥을 시도하였으나 실패 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ErrorDBMess] RECONNECT FAILE" );
		break;
	case EVENT_ERROR_SUCCESS:							// 리커넥을 시도하여 성공 헸을 경우
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[ErrorDBMess] RECONNECT SUCCEED" );
		break;
	case EVENT_ERROR_FAIL:								// 리커넥을 시도하지 않았을 경우
		
		break;
	}
}

void CDataBase::ReConnectDataLog( void )
{
	g_pLog->WriteLog( L"[DB_ERROR] %s", m_DBUser._pQuery ); 
}