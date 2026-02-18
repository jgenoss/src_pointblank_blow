#include "pch.h"
#include "ModuleDBUploader.h"

#include "ModuleConfig.h"

static unsigned __stdcall _DefThreadProc( void* pData )
{
	((CModuleDBUploader*)pData)->Working();
	_endthreadex(0);
	return 0;
}

CModuleDBUploader::CModuleDBUploader()
{
	m_hThread			= NULL;
	m_bIsRunning		= false;
	m_bFinish			= false;

	m_i32ThreadIdx		= 0;
	m_ui32ThreadID		= 0;

	m_pADO				= nullptr;

	m_eDBUpdaterState	= DB_UPDATER_STATE_WAITING;

	m_hFile				= INVALID_HANDLE_VALUE;	
	m_i32FileID			= 0;

	m_pReadPoint		= nullptr;
	m_pLineStartPoint   = nullptr;

	m_i32ReadIdx		= 0;
	m_i32ReadPoint		= 0;
	m_i32ReadFileIdx	= 0;
	m_i32ReadStartIdx	= 0;

	m_ui32CheckTime		= 0;
	m_i32UploadCount	= 0;

	m_pZLogDBConfig		= nullptr;

	m_eReconnectReason  = DB_RECONNECT_REASON_NONE;
}

CModuleDBUploader::~CModuleDBUploader()
{
	Destroy();
}

bool CModuleDBUploader::Create( INT32 i32ThreadIdx )
{
	m_i32ThreadIdx = i32ThreadIdx;

	strcpy_s( m_strColumnName[COLUMN_ACT_TIME], 10, "Time" );		strcpy_s( m_strColumnName[COLUMN_LOG_ID], 10, "ID" );			strcpy_s( m_strColumnName[COLUMN_LOCALE_CODE], 10, "Locale" );
	strcpy_s( m_strColumnName[COLUMN_L_DATA01], 10, "lData1" );		strcpy_s( m_strColumnName[COLUMN_L_DATA02], 10, "lData2" );		strcpy_s( m_strColumnName[COLUMN_L_DATA03], 10, "lData3" );

	strcpy_s( m_strColumnName[COLUMN_I_DATA01], 10, "iData1" );		strcpy_s( m_strColumnName[COLUMN_I_DATA02], 10, "iData2" );		strcpy_s( m_strColumnName[COLUMN_I_DATA03], 10, "iData3" );
	strcpy_s( m_strColumnName[COLUMN_I_DATA04], 10, "iData4" );		strcpy_s( m_strColumnName[COLUMN_I_DATA05], 10, "iData5" );		strcpy_s( m_strColumnName[COLUMN_I_DATA06], 10, "iData6" );
	strcpy_s( m_strColumnName[COLUMN_I_DATA07], 10, "iData7" );		strcpy_s( m_strColumnName[COLUMN_I_DATA08], 10, "iData8" );		strcpy_s( m_strColumnName[COLUMN_I_DATA09], 10, "iData9" );
	strcpy_s( m_strColumnName[COLUMN_I_DATA10], 10, "iData10" );	strcpy_s( m_strColumnName[COLUMN_I_DATA11], 10, "iData11" );	strcpy_s( m_strColumnName[COLUMN_I_DATA12], 10, "iData12" );
	strcpy_s( m_strColumnName[COLUMN_I_DATA13], 10, "iData13" );	strcpy_s( m_strColumnName[COLUMN_I_DATA14], 10, "iData14" );	strcpy_s( m_strColumnName[COLUMN_I_DATA15], 10, "iData15" );
	strcpy_s( m_strColumnName[COLUMN_I_DATA16], 10, "iData16" );	strcpy_s( m_strColumnName[COLUMN_I_DATA17], 10, "iData17" );

	strcpy_s( m_strColumnName[COLUMN_S_DATA01],"sData1" );		strcpy_s( m_strColumnName[COLUMN_S_DATA02],"sData2" );		strcpy_s( m_strColumnName[COLUMN_S_DATA03],"sData3" );
	strcpy_s( m_strColumnName[COLUMN_S_DATA04],"sData4" );		strcpy_s( m_strColumnName[COLUMN_S_DATA05],"sData5" );		strcpy_s( m_strColumnName[COLUMN_S_DATA06],"sData6" );
	strcpy_s( m_strColumnName[COLUMN_S_DATA07],"sData7" );		strcpy_s( m_strColumnName[COLUMN_S_DATA08],"sData8" );		strcpy_s( m_strColumnName[COLUMN_S_DATA09],"sData9" );
	strcpy_s( m_strColumnName[COLUMN_S_DATA10],"sData10" );

	for( INT32 i = 0 ; i < COLUMN_COUNT ; i++ )	m_strColumnSize[i] = (INT32)strlen( m_strColumnName[i] );

	//Create Database
	m_pADO = CADODatabase::NewObject();
	if( NULL == m_pADO )
		return false;

	m_pZLogDBConfig = g_pConfig->GetDatabaseConfig("ZLog");
	if( !m_pZLogDBConfig )
		return false;

	if( FALSE == m_pADO->Connect(m_pZLogDBConfig->GetDataSource(), m_pZLogDBConfig->GetCatalog(), m_pZLogDBConfig->GetAccount(), m_pZLogDBConfig->GetPassword()) )
		return false;

	// 3. 스레드 생성
	m_hThread	= (HANDLE)_beginthreadex( NULL, 0, _DefThreadProc, this, 0, &m_ui32ThreadID );
	if( NULL == m_hThread )																			return false;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL ) )							return false;

	return true;
}

void CModuleDBUploader::Destroy()
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );
	CloseHandle( m_hThread );

	I3_SAFE_RELEASE( m_pADO );
}

bool CModuleDBUploader::SetFile(wchar_t* wstrFileName, INT32 i32FileID )
{
	i3String::Copy(m_wstrFileName,wstrFileName, MAX_PATH);
	
	m_i32FileID			= i32FileID;

	m_i32UploadCount	= 0;
	m_ui32CheckTime		= GetTickCount();
	m_bFinish			= false;

	m_eDBUpdaterState	= DB_UPDATER_STATE_RUNNING_FILE_OPEN;

	//TRACE( "Write File :%s\n", m_strFileName );
	return true;
}

void CModuleDBUploader::Working()
{
	wchar_t wstrInit[ MAX_PATH ];
	i3String::Format(wstrInit, MAX_PATH, L"UP%02d", m_i32ThreadIdx );
	g_pLog->AddLogThread(wstrInit, 0, m_ui32ThreadID );

	INT32 i32Worked = 0;
	m_bIsRunning = true;
	while( m_bIsRunning )
	{
		switch( m_eDBUpdaterState )
		{
		case DB_UPDATER_STATE_WAITING:
			{
				WaitForSingleObject( m_hThread, 100 );
			}
			break;
		case DB_UPDATER_STATE_RUNNING_FILE_OPEN:
			{
				// 파일을 Open 합니다.
				if( false == _OpenFile() )
				{
					// File Open 에 실패하여 예외 처리 해줍니다.
					if ( m_hFile )
					{
						::CloseHandle( m_hFile );
						m_hFile = NULL;
					}
					m_eDBUpdaterState = DB_UPDATER_STATE_ENDING;
					break;
				}
			}
			//break;	// <<-- 없습니다.
		case DB_UPDATER_STATE_RUNNING_FILE_READ:
			{
				if( _ReadFile() )
				{
					m_eDBUpdaterState = DB_UPDATER_STATE_RUNNING_DB_TRANS;
				}
				else
				{
					if (m_hFile)
					{
						::CloseHandle(m_hFile);
						m_hFile = NULL;
					}
					m_eDBUpdaterState = DB_UPDATER_STATE_ENDING;
					break;
				}
			}
			//break;	// <<-- 없습니다.
		case DB_UPDATER_STATE_RUNNING_DB_TRANS:
			{
				m_pReadPoint	= m_pReadBuffer;
				if( m_pADO->BeginTrans() )
				{
					m_eDBUpdaterState = DB_UPDATER_STATE_RUNNING_DB_WRITE;
				}
				else
				{
					m_eReconnectReason	= DB_RECONNECT_REASON_BEGINTRANS;
					m_eDBUpdaterState	= DB_UPDATER_STATE_DB_ERROR;
					break;
				}
			}
			//break;	// <<-- 없습니다.
		case DB_UPDATER_STATE_RUNNING_DB_WRITE:
			{
				if( false == _WriteDB() )
				{	// 다시 파일을 읽어와야 합니다.
					m_eDBUpdaterState = DB_UPDATER_STATE_RUNNING_FILE_READ;
				}
			}
			break;
		case DB_UPDATER_STATE_ENDING:
			{			
				WaitForSingleObject( m_hThread, 100 );
			}
			break;
		case DB_UPDATER_STATE_DB_ERROR:
			{
				_ReConnect();
			}
			break;
		}

		++i32Worked;
		if( 16 < i32Worked )
		{	// 16 fps 마다 한번씩 쉽니다.
			i32Worked = 0;
			::WaitForSingleObject( m_hThread, 1 );
		}
	}
	// 서버 종료시 여태껏 기록한 부분까지 저장합니다.
	if( DB_UPDATER_STATE_RUNNING_DB_WRITE == m_eDBUpdaterState )
	{
		_CommitFile( m_pReadPoint );
	}
}

bool CModuleDBUploader::_CommitFile( char* pPoint )
{
	if( FALSE == m_pADO->CommitTrans() )
	{
		m_eReconnectReason	= DB_RECONNECT_REASON_COMMITTRANS;
		m_eDBUpdaterState	= DB_UPDATER_STATE_DB_ERROR;
		return false;
	}
	m_i32ReadPoint += (m_pLineStartPoint- m_pReadBuffer);
	
	// 파일이 끝나지 않았는데 종료했습니다.다시 로드 합니다.
	m_i32ReadStartIdx = (INT32)((m_pReadBuffer+MAX_FILE_BUFFER_SIZE) - pPoint - 1);
	memmove( m_pReadBuffer, pPoint, m_i32ReadStartIdx );
	
	INT32 i32Size = 0;
	char strBuffer[ 16 ];
	i3String::Format( strBuffer, 16, "%010d\r\n", m_i32ReadPoint );
	if(INVALID_SET_FILE_POINTER == SetFilePointer( m_hFile, 0, 0, FILE_BEGIN ) )
	{
		g_pLog->WriteLog( L"[ERR][%03d] Read File Point Error 1 : File=%s", m_i32ThreadIdx, m_wstrFileName );
		return false;
	}
	if( FALSE == ::WriteFile( m_hFile, strBuffer, 12, (LPDWORD)&i32Size, NULL ) )
	{
		g_pLog->WriteLog( L"[ERR][%03d] Write File ReadPoint Error : File=%s", m_i32ThreadIdx, m_wstrFileName );
		return false;
	}
	if(INVALID_SET_FILE_POINTER == SetFilePointer( m_hFile, m_i32ReadFileIdx, 0, FILE_BEGIN ) )
	{
		g_pLog->WriteLog( L"[ERR][%03d] Read File Point Error 2 : File=%s", m_i32ThreadIdx, m_wstrFileName );
		return false;
	}
	
	return true;
}

bool CModuleDBUploader::_OpenFile()
{
	// 파일을 Open 합니다.
	m_hFile = ::CreateFileW( m_wstrFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( INVALID_HANDLE_VALUE == m_hFile )
	{
		// 파일이 없습니다. 작업을 종료 합니다.
		g_pLog->WriteLog( L"[ERR][%03d] Craete File Fail : File=%s", m_i32ThreadIdx, m_wstrFileName);
		
		return false;
	}
	// 맨 처음 한줄을 읽어 DB에 쓰던 파일인지 확인 합니다.
	INT32 i32Size = 0;
	char strReadSize[ 12 ];
	if( FALSE == ::ReadFile( m_hFile, strReadSize, 12, (LPDWORD)&i32Size, NULL ) )
	{
		// 파일 읽기에 실패 했습니다.
		g_pLog->WriteLog( L"[ERR][%03d] Create Read File Fail : File=%s", m_i32ThreadIdx, m_wstrFileName);

		return false;
	}
	if( 0 >= i32Size )
	{	// 내용이 없습니다. 종료합니다.
		g_pLog->WriteLog( L"[ERR][%03d] Create No Contents : File=%s", m_i32ThreadIdx, m_wstrFileName);

		return false;
	}
	strReadSize[ 10 ] = '\0';
	
	m_i32ReadStartIdx	= 0;
	m_i32ReadFileIdx	= atoi( strReadSize );
	m_i32ReadPoint		= m_i32ReadFileIdx;
	
	if(INVALID_SET_FILE_POINTER == SetFilePointer( m_hFile, m_i32ReadFileIdx, 0, FILE_BEGIN ) )
	{
		g_pLog->WriteLog( L"[ERR][%03d] Create File Point Error : File=%s", m_i32ThreadIdx, m_wstrFileName);

		return false;
	}
	return true;
}

bool CModuleDBUploader::_ReadFile()
{
	// 메모리에 읽습니다.
	INT32 i32Size = 0;

	// 파일을 읽어 옵니다.
	if( FALSE == ::ReadFile( m_hFile, &m_pReadBuffer[m_i32ReadStartIdx], MAX_FILE_BUFFER_SIZE-m_i32ReadStartIdx-1, (LPDWORD)&i32Size, NULL ) )
	{
		// 파일 읽기에 실패 했습니다. 끝냅니다.
		g_pLog->WriteLog( L"[ERR][%03d] Read File Fail : File=%s", m_i32ThreadIdx, m_wstrFileName );

		return false;
	}
	if( 0 >= i32Size )
	{	// 내용이 없습니다. 종료합니다.	
		m_bFinish = true;

		g_pLog->WriteLog( L"[LOG][%03d] Upload Complete Time:%d, Count:%d\n", m_i32ThreadIdx, GetTickCount()-m_ui32CheckTime, m_i32UploadCount );

		return false;
	}
	// 포인트 기록
	m_pReadBuffer[ m_i32ReadStartIdx+i32Size ] = '\0';
	m_i32ReadFileIdx	+= i32Size;

	return true;
}

bool CModuleDBUploader::_WriteDB()
{
	if( _GetNextLine() )
	{
		_MakeQuery();
	}
	else
	{
		if( _CommitFile( m_pLineStartPoint ) )
		{
			return false;
		}
	}
	return true;
}

bool CModuleDBUploader::_GetNextLine()
{
	char* pLineBuffer = m_strLine;
	if( '\n' == (*m_pReadPoint)) m_pReadPoint++;
	m_pLineStartPoint = m_pReadPoint;
	
	INT32 i32Size = 0;
	while( '\r' != (*m_pReadPoint) )
	{
		// '\0' 이면 끝냅니다.
		if( '\0' == *m_pReadPoint )			return false;

		*pLineBuffer = *m_pReadPoint;

		pLineBuffer++;
		m_pReadPoint++;

		i32Size++;
		if( MAX_FILE_LINE_SIZE < i32Size )	break;		
	}

	*pLineBuffer		= '\0';
	m_pReadPoint++;
	
	if( '\0' != *(m_pReadPoint) )
	{
		m_pReadPoint++;	//<-- \r\n 으로 끝나기 때문에 포인터를 더 뒤로 이동 시킵니다.
	}

	return true;
}

void CModuleDBUploader::_MakeQuery()
{
	char* pLineBuffer		= m_strLine;
	char* pQueryColumn		= m_strQueryColumn;
	char* pQueryContents	= m_strQueryContents;

	// Null Check
	if( '\0' == *pLineBuffer )	return;

	// StringIdx
	INT32 i32DataIdx	= ((INT32)*pLineBuffer) - 'a';	pLineBuffer++;
	INT32 i32StringIdx	= ((INT32)*pLineBuffer) - 'a';	pLineBuffer++;

	// Null Check
	if( '\0' == *pLineBuffer )	return;
	
	//////////////////////////////////////////////////////
	// Time
	// Column
	i3mem::Copy( pQueryColumn, m_strColumnName[ COLUMN_ACT_TIME ], m_strColumnSize[ COLUMN_ACT_TIME ] );	// 4자 Time
	pQueryColumn += m_strColumnSize[ COLUMN_ACT_TIME ];
	*pQueryColumn = ',';		pQueryColumn++;
	// Contents
	*pQueryContents = '\'';		pQueryContents++;
	i3mem::Copy( pQueryContents, pLineBuffer, 22 );						// 22자 11/07/2011 13:30:47.00
	pQueryContents += 22;
	*pQueryContents = '\'';		pQueryContents++;
	*pQueryContents = ',';		pQueryContents++;

	// Table Time
	char strDate[ 32 ];
	i3mem::Copy( strDate, &pLineBuffer[6], 4 );
	i3mem::Copy( &strDate[5], pLineBuffer, 2 );
	i3mem::Copy( &strDate[8], &pLineBuffer[3], 2 );
	strDate[4] = '_';
	strDate[7] = '_';
	strDate[10] = '\0';

	pLineBuffer += 24;

	// 이 뒤부터는 &,를 검사해서 확인합니다.
	INT32 i32ColumnIdx	= COLUMN_LOG_ID;

	INT32 i32ColumCount = COLUMN_LOG_ID;
	i3mem::Copy( pQueryColumn, m_strColumnName[ COLUMN_LOG_ID ], m_strColumnSize[ COLUMN_LOG_ID ] );
	pQueryColumn += m_strColumnSize[ COLUMN_LOG_ID ];

	while( '\0' != *pLineBuffer )
	{
		if( ('&' == *pLineBuffer) && (',' == *(pLineBuffer+1)) )
		{
			pLineBuffer += 2;	// "&,"를 건너 뜀니다.

			if( i32ColumCount >= i32StringIdx )
			{
				*pQueryContents = '\'';
				pQueryContents++;
			}

			// Query 쉼표
			*pQueryColumn = ',';			pQueryColumn++;
			*pQueryContents = ',';			pQueryContents++;

			i32ColumnIdx++;
			i32ColumCount++;
			if (COLUMN_COUNT == i32ColumnIdx)	return;

			if( i32ColumCount == i32DataIdx )
			{
				i32ColumnIdx = COLUMN_I_DATA01;
			}
			else if( i32ColumCount == i32StringIdx )
			{	// 이게 같다면 그때부터 String 입니다.
				i32ColumnIdx = COLUMN_S_DATA01;
				*pQueryContents = 'N';		pQueryContents++;
				*pQueryContents = '\'';		pQueryContents++;
			}
			else if( i32ColumCount > i32StringIdx )
			{
				*pQueryContents = 'N';		pQueryContents++;
				*pQueryContents = '\'';		pQueryContents++;
			}
			
			memcpy( pQueryColumn, m_strColumnName[ i32ColumnIdx ], m_strColumnSize[ i32ColumnIdx ] );
			pQueryColumn += m_strColumnSize[ i32ColumnIdx ];
			continue;
		}

		if( '\'' == *pLineBuffer )
		{	// ' 예외 처리
			*pQueryContents = '\'';			pQueryContents++;
		}
		*pQueryContents = *pLineBuffer;

		pQueryContents++;
		pLineBuffer++;
	}
	if( i32StringIdx <= i32ColumCount )
	{
		*pQueryContents = '\'';
		pQueryContents++;
	}
	*pQueryColumn	= '\0';
	*pQueryContents	= '\0';

	char strID[ 5 ];
	char* pID = strID;
	INT32 i32Idx = 25;
	INT32 i32IDidx = 0;
	while( ',' != m_strQueryContents[ i32Idx ] )
	{
		*pID = m_strQueryContents[ i32Idx ];
		pID++;
		i32IDidx++;
		if (5 == i32IDidx) return;
		i32Idx++;
	}
	*pID = '\0';

	i3String::Format( m_strInsertQuery, MAX_DB_QUERY_SIZE, "INSERT INTO TZLogPB_%s_%s(%s)VALUES(%s)", strID, strDate, m_strQueryColumn, m_strQueryContents );
	//TRACE( "%s", m_strInsertQuery );
	m_i32UploadCount++;
	if( FALSE == m_pADO->ExecuteQuery( m_strInsertQuery ) )
	{
		// 이 경우 깔끔한 처리를 위하여 로그만 남깁니다.
		wchar_t wstrInsertQuery[MAX_DB_QUERY_SIZE];
		MultiByteToWideChar(CP_ACP, NULL, m_strInsertQuery, -1, wstrInsertQuery, MAX_DB_QUERY_SIZE);

		g_pLog->WriteLog( L"[ERR][%03d] Insert Fail / Reason: %08x / Query=%s", m_i32ThreadIdx, m_pADO->GethrLastError(), wstrInsertQuery);
	}	
}

void CModuleDBUploader::_ReConnect()
{
	switch( m_eReconnectReason )
	{
	case DB_RECONNECT_REASON_BEGINTRANS:
		if( m_pADO->BeginTrans() )
		{
			m_eDBUpdaterState = DB_UPDATER_STATE_RUNNING_DB_WRITE;
			return;
		}
		break;
	case DB_RECONNECT_REASON_COMMITTRANS:
		m_eDBUpdaterState = DB_UPDATER_STATE_RUNNING_FILE_READ;
		if( _CommitFile( m_pLineStartPoint ) )
		{
			return;
		}
		else
		{
			// 만약에 실패를 한다면 처음부터 다시 읽어야 하는겁니다.
			m_eDBUpdaterState	= DB_UPDATER_STATE_RUNNING_DB_TRANS;
		}
		break;
	}
	if( m_pADO->IsConnected() )
	{
		g_pLog->WriteLog( L"[ERR][%03d] ReConnect DBUploader CONNECTED", m_i32ThreadIdx );
	}
	else
	{
		g_pLog->WriteLog( L"[ERR][%03d] ReConnect DBUploader NOT CONNECTED", m_i32ThreadIdx );
	}

	if (FALSE == m_pADO->Close())
	{
		g_pLog->WriteLog(L"[ERR][%03d] ReConnect DBUploader Close Error / Reason: %08x", m_i32ThreadIdx, m_pADO->GethrLastError());
		return;
	}

	if( FALSE == m_pADO->Connect(m_pZLogDBConfig->GetDataSource(), m_pZLogDBConfig->GetCatalog(), m_pZLogDBConfig->GetAccount(), m_pZLogDBConfig->GetPassword()) ) 
	{
		g_pLog->WriteLog( L"[ERR][%03d] ReConnect DBUploader / Reason: %08x ", m_i32ThreadIdx, m_pADO->GethrLastError());
		return;
	}
	else
	{
		g_pLog->WriteLog( L"[SUC][%03d] ReConnect DBUploader", m_i32ThreadIdx );
		return;
	}
}