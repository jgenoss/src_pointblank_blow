#include "pch.h"
#include "DataBase.h"
#include "UserSession.h"
#include "ModuleDataBase.h"

CDataBase * g_pDataBase = NULL;

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
	i3String::Copy(m_strUdlFilePath, pstrUdlFilePath, SERVER_STRING_COUNT ); 
	//////////////////////////////////////////////////////////////////////////////////////
	//1. DataBase Connect 
	m_i32WorkThreadCount = i32WorkThreadCount;
	// Option Thread 용
	m_DBUser._pDB	= CADODatabase::NewObject(); 
	if( !m_DBUser._pDB->Connect( m_strUdlFilePath ) )
	{
		g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[CDataBase::OnCreate] User DB Connect Fail"); 
		return FALSE; 
	}

	m_ppRingIn = new NSM_CRingBuffer*[ m_i32WorkThreadCount ];
	for( INT32 i = 0 ; i < m_i32WorkThreadCount ; i++ )
	{
		m_ppRingIn[i] = new NSM_CRingBuffer;
		m_ppRingIn[i]->OnCreate( sizeof(TYPE_RING_IN), 500 );
	}
	m_pRingOut = new NSM_CRingBuffer;
	m_pRingOut->OnCreate( sizeof(TYPE_RING_OUT), 500 ); 

	if( !i3Thread::Create("DataBase", 0, 4096, NULL, PRIORITY_NORMAL) )		return FALSE;

	g_pModuleLogFile->Write_M_Log(MAIN_THREAD_IDX, NULL, "[CDataBase::OnCreate] DataBase Connect Success"); 
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
					char strLog[ MAX_PATH ];
					i3String::Format( strLog, MAX_PATH,"[CDataBase::OnRunning()] DB Time Over : %d,  pIn->_ui8Type : %d ", dwNewTime, pIn->_ui8Type );
					g_pModuleLogFile->Write_M_Log( MODULE_DATABASE_THREAD_IDX, NULL, strLog, WARNING_DATABASE_TIMEOUT ); 
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

	INT32 i32Idx = (INT32)(pIn->_i64UID % INVENTORY_TABLE_COUNT);

	if( 0x10000000 & pIn->_ui32ChangeFlag )
	{	// 0x10000000 일때는 Default 로 변경 합니다.
		i3String::Format( pDataBaseInfo->_pQuery, SQL_STRING_COUNT, USP_20110, i32Idx, pIn->_i64UID, ENV_SETTING_DEFAULT );
		if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
		{
			ReConnectDataBase();
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
	else
	{
		char strBinaryTemp[ SQL_STRING_COUNT ];
		char szEnvBasic[ SQL_STRING_COUNT ] = {0,};
		char szEnvkey[ SQL_STRING_COUNT ] = {0,};
		char szMacrochat[MACRO_MAX_COUNT][ SQL_STRING_COUNT ] = {0,};

		// 옵션이 변경 되었다고 저장합니다.
		// 저장할 것들만 정리하여 한번에 Update 합니다.
		i3String::Format( pDataBaseInfo->_pQuery, SQL_STRING_COUNT, USP_20100, i32Idx, pIn->_i64UID, ENV_SETTING_CHANGE );

		if( 0x00000001 & pIn->_ui32ChangeFlag )
		{	// 유저 정보를 저장합니다.			
			String2Binary( strBinaryTemp, (char*)&pIn->_NetEnvSet, sizeof(NET_ENV_SET) );									// 바이너리로 변환
			if(0 == strBinaryTemp[0]) 
			{
				i3String::Copy( szEnvBasic, ",'none'", SQL_STRING_COUNT );
			}
			else
			{
				i3String::Format( szEnvBasic, SQL_STRING_COUNT, ",'%s'", strBinaryTemp );
			}
			i3String::Concat( pDataBaseInfo->_pQuery, szEnvBasic );
		}
		else
		{
			i3String::Copy( szEnvBasic, ",'none'", SQL_STRING_COUNT );
			i3String::Concat( pDataBaseInfo->_pQuery, szEnvBasic );
		}
		if( 0x00000002 & pIn->_ui32ChangeFlag )
		{	// 키 정보를 저장합니다.
			String2Binary( strBinaryTemp, (char*)pIn->_aKeyMap, (sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT) );		// 바이너리로 변환
			if(0 == strBinaryTemp[0]) 
			{
				i3String::Copy( szEnvkey, ",'none'", SQL_STRING_COUNT );
			}
			else
			{
				i3String::Format( szEnvkey, SQL_STRING_COUNT, ",'%s'", strBinaryTemp ); 
			}			
			i3String::Concat( pDataBaseInfo->_pQuery, szEnvkey );
		}
		else
		{
			i3String::Copy( szEnvkey, ",'none'", SQL_STRING_COUNT );
			i3String::Concat( pDataBaseInfo->_pQuery, szEnvkey );
		}
		if( 0x00000004 & pIn->_ui32ChangeFlag )
		{	//메크로를 저장합니다.
			char TempStr[CONVERT_STRING_COUNT];			
			for(INT32 i = 0; i < MACRO_MAX_COUNT; i++)
			{	
				if(0 == pIn->_astrMacro[i][0]) 
				{
					i3String::Copy( szMacrochat[i], ",'none'", SQL_STRING_COUNT );
				}
				else
				{
					pDataBaseInfo->_pDB->ConvertString( TempStr, CONVERT_STRING_COUNT, pIn->_astrMacro[i] );
					i3String::Format( szMacrochat[i], SQL_STRING_COUNT, ",'%s'", TempStr );					
				}
				i3String::Concat( pDataBaseInfo->_pQuery, szMacrochat[i] );
			}
		}
		else
		{
			for(INT32 i = 0; i < MACRO_MAX_COUNT; i++)
			{
				i3String::Copy( szMacrochat[i], ",'none'", SQL_STRING_COUNT );
				i3String::Concat( pDataBaseInfo->_pQuery, szMacrochat[i] );
			}
		}

		// 모든 정보를 모아 한번에 Update 칩니다.
		if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
		{
			ReConnectDataBase();
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

	i3String::Format( pDataBaseInfo->_pQuery, SQL_STRING_COUNT, USP_20200, i32Idx, pIn->_i64UID );
	if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
	{
		ReConnectDataBase();
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
		i3String::Format( pDataBaseInfo->_pQuery, SQL_STRING_COUNT, USP_20210, i32Idx, pIn->_i64UID );
		if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
		{
			ReConnectDataBase();
			return;
		}
		if(1 != pDataBaseInfo->_pDB->GetRecordCount())
		{
			ReConnectDataLog();
		}

		//Set Key Option 
		pDataBaseInfo->_pDB->GetBinaryData(0, &pOut->_NetEnvSet,	sizeof(NET_ENV_SET));
		pDataBaseInfo->_pDB->GetBinaryData(1, &pOut->_aKeyMap,	sizeof(GAME_KEY_STRUCT)*GAME_KEY_MAP_COUNT);

		//Set Macro 
		pDataBaseInfo->_pDB->GetTextData(2, pOut->_astrMacro[0] , 256);
		pDataBaseInfo->_pDB->GetTextData(3, pOut->_astrMacro[1] , 256);
		pDataBaseInfo->_pDB->GetTextData(4, pOut->_astrMacro[2] , 256);
		pDataBaseInfo->_pDB->GetTextData(5, pOut->_astrMacro[3] , 256);
		pDataBaseInfo->_pDB->GetTextData(6, pOut->_astrMacro[4] , 256);
	}
	else
	{
		i3String::Format( pDataBaseInfo->_pQuery, SQL_STRING_COUNT, USP_20220, i32Idx, pIn->_i64UID );
		if( !pDataBaseInfo->_pDB->ExecuteSP( pDataBaseInfo->_pQuery ) )
		{
			ReConnectDataBase();
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

	if( 1003040000 > g_pContextMain->GetInvenTime() )
	{
		// 이벤트
		i3String::Format( pDataBaseInfo->_pQuery, SQL_STRING_COUNT, USP_20230, pIn->_i64UID );
		if( !pDataBaseInfo->_pDB->ExecuteSP(pDataBaseInfo->_pQuery) )		
		{
			ReConnectDataBase();
			return;
		}

		if( 0 == pDataBaseInfo->_pDB->GetRecordCount() )
		{
			pOut->_ui8Event = 1;
			i3String::Format( pDataBaseInfo->_pQuery, SQL_STRING_COUNT, USP_20240, pIn->_i64UID );
			if( !pDataBaseInfo->_pDB->ExecuteSP(pDataBaseInfo->_pQuery) )		
			{
				ReConnectDataBase();
				return;
			}
			if(0 == pDataBaseInfo->_pDB->GetRecordCount())
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
		else
		{
			pOut->_ui8Event = 0;
		}
	}
	else
	{
		pOut->_ui8Event = 0;
	}

}

void CDataBase::String2Binary( char* pOut, char* pIn, INT32 i32Size )
{
	int i;
	*pOut = '0';	pOut++;
	*pOut = 'x';	pOut++;

	for( i = 0 ; i < i32Size ; i++ )
	{
		*pOut = (pIn[ i ] & 0xF0) >> 4 ;
		if( *pOut < 10 )	*pOut += '0';
		else				*pOut += 0x37;	// ('A'-10)
		pOut++;

		*pOut = (pIn[ i ] & 0x0F) ;
		if( *pOut < 10 )	*pOut += '0';
		else				*pOut += 0x37;	// ('A'-10)
		pOut++;
		
	}
	*(pOut) = '\0';
}

void CDataBase::ReConnectDataBase( void )
{
	char szLog[ SQL_STRING_COUNT + 20 ];
	i3String::Format( szLog, SQL_STRING_COUNT + 20, "[DB_ERROR] %s", m_DBUser._pQuery );	
	g_pModuleLogFile->Write_M_Log( MODULE_DATABASE_THREAD_IDX, NULL, szLog ); 

	if( m_DBUser._pDB->IsConnected() )
	{
		g_pModuleLogFile->Write_M_Log(MODULE_DATABASE_THREAD_IDX, NULL, "ReConnectDataBase CONNECTED", WARNING_DATABASE_RECONNECT ); 
	}
	else
	{
		g_pModuleLogFile->Write_M_Log(MODULE_DATABASE_THREAD_IDX, NULL, "ReConnectDataBase RECONNECT", WARNING_DATABASE_RECONNECT ); 
	}

	m_DBUser._pDB->Close();
	I3_SAFE_RELEASE( m_DBUser._pDB );

	m_DBUser._pDB	= CADODatabase::NewObject();
	m_DBUser._pDB->Connect( m_strUdlFilePath );
}

void CDataBase::ReConnectDataLog( void )
{
	char strLog[ SQL_STRING_COUNT + 20 ];
	i3String::Format( strLog, SQL_STRING_COUNT + 20, "[DB_ERROR] %s", m_DBUser._pQuery );
	g_pModuleLogFile->Write_M_Log( MODULE_DATABASE_THREAD_IDX, NULL, strLog ); 
}