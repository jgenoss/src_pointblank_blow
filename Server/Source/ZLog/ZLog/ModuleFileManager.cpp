#include "pch.h"
#include "ModuleFileManager.h"
#include "ModuleConfig.h"
#include "ModuleDBUploader.h"
#include <io.h>

static unsigned __stdcall _DefThreadProc( void* pData )
{
	((CModuleFileManager*)pData)->Working();
	_endthreadex(0);
	return 0;
}

CModuleFileManager::CModuleFileManager()
{
	m_hThread				= NULL;
	m_bIsRunning			= false;

	m_ui32ServerCount		= 0;
	m_ui32ThreadID			= 0;

	m_pADO					= nullptr;
	
	m_i32FileListEmptyCount	= 0;
	m_i32FileListWaitCount	= 0;

	m_i32UploaderCount		= 0;
	m_pUploader				= nullptr;

	m_i32IDListIdx			= 0;

	m_pZlogDBConfig			= nullptr;
	
	i3mem::FillZero( m_aFileList, sizeof(m_aFileList) );
}

CModuleFileManager::~CModuleFileManager()
{
	Destroy();
}

bool CModuleFileManager::Create( UINT32 ui32ServerCount, INT32 i32UploaderCount, char* strLogPath )
{
	// 로그를 기록할 기본 폴더
	strcpy_s( m_strLogPath, MAX_PATH, strLogPath );
	// Server Count
	m_ui32ServerCount	= ui32ServerCount;
	// Uploader Count 쓰레드가 많아도 MAX_SIZE_DB_UPLOAD_THREAD 개 이상 넘기지 않습니다.
	m_i32UploaderCount = i32UploaderCount;
	if( MAX_SIZE_DB_UPLOAD_THREAD < m_i32UploaderCount ) m_i32UploaderCount = MAX_SIZE_DB_UPLOAD_THREAD;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// For File Write
	// CreateDB
	m_pADO = CADODatabase::NewObject();
	if( NULL == m_pADO )
		return false;

	m_pZlogDBConfig = g_pConfig->GetDatabaseConfig("ZLog");
	if( !m_pZlogDBConfig )
		return false;

	if( !m_pADO->Connect(m_pZlogDBConfig->GetDataSource(), m_pZlogDBConfig->GetCatalog(), m_pZlogDBConfig->GetAccount(), m_pZlogDBConfig->GetPassword()) )
		return false;

	// 최초 생성시 트랜젝션을 한번 비워줍니다.(MSSQL_2008 동작안됨)
#ifndef USE_MSSQL_2008
	if( FALSE == m_pADO->ExecuteQuery( "BACKUP LOG [ZLog] WITH TRUNCATE_ONLY" ) )				return false;
#endif

	// Create Uploader
	if( 0 < m_i32UploaderCount )
	{
		m_pUploader = new CModuleDBUploader[ m_i32UploaderCount ];
		if( NULL == m_pUploader )																return false;
		for( INT32 i = 0 ; i < m_i32UploaderCount ; i++ )
		{
			if( false == m_pUploader[i].Create( i ) )											return false;
		}
	}

	i3mem::FillZero( m_aFileList, sizeof(FILE_LIST) * MAX_WAIT_SIZE );
	
	//업로드 중이였던 로그파일을 우선 찾아서 리스트에 등록합니다.
	_CreateFileList();
	
	// Create Thread
	m_hThread	= (HANDLE)_beginthreadex( NULL, 0, _DefThreadProc, this, 0, &m_ui32ThreadID );
	if( NULL == m_hThread )																		return false;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL ) )						return false;

	return true;
}

void CModuleFileManager::Destroy()
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );
	CloseHandle( m_hThread );

	I3_SAFE_DELETE_ARRAY( m_pUploader );
	I3_SAFE_RELEASE( m_pADO );
}

void CModuleFileManager::Working()
{	
	g_pLog->AddLogThread( L"FILE", 0, m_ui32ThreadID );

	INT32 i32Worked;
	m_bIsRunning = true;
	while( m_bIsRunning )
	{
		i32Worked = 0;
						
		i32Worked += _UpdateFileList();		
		i32Worked += _CheckUploader();

		// 2016-11-23 채종현 : 파일은 여유를 두면서 찾아도 무방합니다. 
		// (CPU율 점유율 확 올라감. Sleep에 의존하기 보다는 이쪽 로직 *특히 UpdateFileList* 개선 필요)
		if ( 0 < i32Worked ) ::WaitForSingleObject(m_hThread, 120);
	}
}

INT32 CModuleFileManager::_GetIDListIdx()
{
	m_i32IDListIdx++;
	if( 0x7FFFFFFE < m_i32IDListIdx ) m_i32IDListIdx = 0;
	return m_i32IDListIdx;
}

BOOL CModuleFileManager::_SetNextUploadFile( INT32 i32Idx )
{
	struct _finddata_t	sc_file;
	char strSerachFolder[ MAX_PATH ];
	strcpy_s( strSerachFolder, MAX_PATH, m_strLogPath );
	strcat_s( strSerachFolder, MAX_PATH, "\\*.zlog" );

	intptr_t hFile = _findfirst( strSerachFolder, &sc_file );
	if( -1L == hFile )
	{
		_findclose(hFile);
		return FALSE;
	}
	
	m_aFileList[i32Idx].m_i32FileID	= _GetIDListIdx();
	m_aFileList[i32Idx].m_eFileState	= FILELIST_STATE_WAIT;

	char strFileName[MAX_PATH];
	i3String::Format(strFileName, MAX_PATH, "%s\\%s", m_strLogPath, sc_file.name);
	ChangeFileExtension(strFileName, "upload", MAX_PATH );

	MultiByteToWideChar(CP_ACP, NULL, strFileName, -1, m_aFileList[i32Idx].m_wstrFileName, MAX_PATH);
			
	++m_i32FileListWaitCount;

	_findclose(hFile);

	return TRUE;
}

INT32 CModuleFileManager::_CreateFileList()
{
	struct _finddata_t	sc_file;
	char strSerachFolder[ MAX_PATH ];	
	strcpy_s( strSerachFolder, MAX_PATH, m_strLogPath );
	strcat_s( strSerachFolder, MAX_PATH, "\\*.upload" );

	intptr_t hFile = _findfirst( strSerachFolder, &sc_file );
	if( -1L == hFile )
	{
		_findclose(hFile);	
		m_i32FileListEmptyCount = MAX_WAIT_SIZE;
		return 0;
	}

	INT32 i32Idx = 0;

	
	do
	{
		// ".", ".." 는 검색 대상에서 제외 시킵니다.
		if( '.' == sc_file.name[0] ) continue;

		m_aFileList[i32Idx].m_i32FileID		= _GetIDListIdx();
		m_aFileList[i32Idx].m_eFileState	= FILELIST_STATE_WAIT;

		char strFileName[MAX_PATH];
		i3String::Format(strFileName, MAX_PATH, "%s\\%s", m_strLogPath, sc_file.name);
		MultiByteToWideChar(CP_ACP, NULL, strFileName, -1, m_aFileList[i32Idx].m_wstrFileName, MAX_PATH);

		++m_i32FileListWaitCount;
		++i32Idx;

		if( MAX_WAIT_SIZE == i32Idx ) break;
			
	}
	while( 0 == _findnext( hFile, &sc_file ) );

	if( MAX_WAIT_SIZE != i32Idx )
	{
		for( INT32 i = i32Idx ; i < MAX_WAIT_SIZE ; ++i)
		{
			m_aFileList[i].m_eFileState = FILELIST_STATE_EMPTY;
			++m_i32FileListEmptyCount;
		}
	}

	_findclose(hFile);

	return m_i32FileListWaitCount;

}

INT32 CModuleFileManager::_UpdateFileList()
{
	if ( m_i32FileListEmptyCount == 0 ) return 0; //갱신할 파일이 없으므로 함수를 종료한다.
	
	
	//리스트에서 빈칸을 찾는다. 없으면 업데이트를 종료한다.
	for( UINT32 i = 0 ; i < MAX_WAIT_SIZE ; ++i )
	{
		if( FILELIST_STATE_EMPTY != m_aFileList[i].m_eFileState ) continue;

		//해당인덱스에 신규 파일을 할당해준다.
		if( _SetNextUploadFile(i) )
			m_i32FileListEmptyCount--;
	}

	return 1;
}

void CModuleFileManager::_MakeDBTable( INT32 i32Idx )
{
	wchar_t wstrTableName[11];

	//파일 이름에서 테이블 네임을 추출한다.
	if ( FALSE == GetDateFromFileName( m_aFileList[i32Idx].m_wstrFileName, wstrTableName, 11 ) )
	{
		// 로그남깁니다.
		g_pLog->WriteLog(L"[ERR] Make DB Table Fail GetName : File=%s", m_aFileList[i32Idx].m_wstrFileName);
		return;
	}

	// DB가 없으면 만듭니다.
	wchar_t wstrSQL[ MAX_PATH ];
	i3String::Format(wstrSQL, MAX_PATH, L"EXEC usp_ZLog_Create_Table '%s'", wstrTableName);
	if( FALSE == m_pADO->ExecuteSP(wstrSQL) )
	{
		// 로그남깁니다.
		g_pLog->WriteLog( L"[ERR] Make DB Table Fail / Reason : %08x / EXEC SP : %s", m_pADO->GethrLastError(), wstrSQL);
		_ReConnect();
	}
	else
	{
		g_pLog->WriteLog( L"[SUC] Make DBTable - Name:%s\n", wstrTableName);
	}

	// DB Name이 바뀔 일이 있으면 이 부분을 수정해야 함.
	// 지금은 폴더이름과 DB이름이 같기때문에 이 상태를 유지. - 2012.08.22 김용수
	//strcpy( pFolderList->m_strDBName, strTableName );
}

INT32 CModuleFileManager::_CheckUploader()
{
	INT32 i32Worked = 0;
	for( INT32 i = 0 ; i < m_i32UploaderCount ; i++ )
	{
		i32Worked = 0;

		switch( m_pUploader[i].GetState() )
		{
		default:			continue;
		case DB_UPDATER_STATE_WAITING:
			{
				// 새로운 일감을 할당해 줍니다.
				_StartFileUpload( i );
			}
			break;
		case DB_UPDATER_STATE_ENDING:
			{
				// 파일을 지워줍니다.
				_DeleteFile( i );
				i32Worked = 1;
			}
			break;
		}
	}
	return i32Worked;
}

void CModuleFileManager::_StartFileUpload( INT32 i32Idx )
{
	if( 0 == m_i32FileListWaitCount ) return;	//업로드 대기중인 파일이 없으면 함수를 종료한다.

	
	INT32 i32FileIdx = -1;
	for( INT32 i = 0 ; i < MAX_WAIT_SIZE ; i++ )
	{
		if( FILELIST_STATE_WAIT != m_aFileList[i].m_eFileState ) continue;
		i32FileIdx = i;
		break;
	}

	if( -1 == i32FileIdx )
	{
		// m_i32FolderWaitCount 가 0이 아닐때 m_bUsed 가 모두 true 이면 코딩상의 오류입니다.
		// 로그남깁니다.
		g_pLog->WriteLog( L"[ERR] Start Upload Not Found Idle File." );
		return;
	}

	FILE_LIST* pFileList = &m_aFileList[i32FileIdx];

	// 현재 사용중이라는 mark 를 합니다.
	pFileList->m_eFileState	= FILELIST_STATE_UPLOAD;

	_MakeDBTable(i32FileIdx);
	m_pUploader[i32Idx].SetFile( pFileList->m_wstrFileName, pFileList->m_i32FileID );
	
	m_i32FileListWaitCount--;
}

void CModuleFileManager::_DeleteFile( INT32 i32Idx )
{
	// 사용했던 Folder 를 찾습니다.
	INT32 i32FileIdx = -1;

	for( INT32 i = 0 ; i < MAX_WAIT_SIZE ; i++ )
	{
		if( m_aFileList[i].m_i32FileID != m_pUploader[i32Idx].GetFileID() ) continue;
		i32FileIdx = i;
	}
	if( -1 == i32FileIdx )
	{
		g_pLog->WriteLog( L"[ERR] Delete File Find Fail." );
		return;
	}

	FILE_LIST* pFileList = &m_aFileList[ i32FileIdx ];

	// File 을 지웁니다.
	if( m_pUploader[i32Idx].GetFinish() )
	{
		//I3TRACE( "Delete File: %s\n", m_pUploader[i32Idx].GetFileName() );
		if( FALSE == DeleteFileW( m_pUploader[i32Idx].GetFileName() ) )
		{
			g_pLog->WriteLog( L"[ERR] Delete File Fail : UpIdx=%d, FileIdx=%d, File=%s", i32Idx, i32FileIdx, m_pUploader[i32Idx].GetFileName() );
			// 삭제된 파일은 리스트에서 비움으로 처리합니다.
			pFileList->m_eFileState = FILELIST_STATE_EMPTY;

			// 우선 폴더를 사용 가능 상태로 만듭니다.
			m_i32FileListEmptyCount++;

			// UPloader 를 대기 상태로 만들어 주고
			m_pUploader[i32Idx].SetState(DB_UPDATER_STATE_WAITING);
			return;
		}

		g_pLog->WriteLog( L"[LOG]Delete File: %s\n", m_pUploader[i32Idx].GetFileName() );
	}
	else
	{
		g_pLog->WriteLog( L"[ERR] Delete File No Finish : FileName=%s", m_pUploader[i32Idx].GetFileName() );
	}

	// 삭제된 파일은 리스트에서 비움으로 처리합니다.
	pFileList->m_eFileState = FILELIST_STATE_EMPTY;
	
	// 우선 폴더를 사용 가능 상태로 만듭니다.
	m_i32FileListEmptyCount++;

		
	// UPloader 를 대기 상태로 만들어 주고
	//I3TRACE( "Waiting File :%d\n", i32Idx );
	m_pUploader[i32Idx].SetState( DB_UPDATER_STATE_WAITING );
}

void CModuleFileManager::_ReConnect()
{	
	if( m_pADO->IsConnected() )	g_pLog->WriteLog( L"[ERR] ReConnect DBManager CONNECTED");
	else						g_pLog->WriteLog( L"[ERR] ReConnect DBManager NOT CONNECTED");

	if (FALSE == m_pADO->Close())
	{
		g_pLog->WriteLog(L"[ERR] ReConnect DBManager Close Error / Reason: %08x", m_pADO->GethrLastError());
		return;
	}


	if( m_pADO->Connect(m_pZlogDBConfig->GetDataSource(), m_pZlogDBConfig->GetCatalog(), m_pZlogDBConfig->GetAccount(), m_pZlogDBConfig->GetPassword()) ) 
	{
		g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_LEVEL_DB, L"[SUC] ReConnect DBManager" );
	}
	else
	{
		g_pLog->WriteLog( WARNING_TYPE_NOTICE, WARNING_LEVEL_DB, L"[ERR] ReConnect DBManager / Reason: %08x", m_pADO->GethrLastError());
	}
}