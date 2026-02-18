#include "pch.h"
#include "XTrapModule.h"
// #include "ModuleLogFile.h"

static UINT32 _DefThreadProc(void * pPointer)
{
	((CXTrapModule*)pPointer)->OnRunning(); 
	return 0; 
}

CXTrapModule::CXTrapModule()
{
	m_hThread			= NULL;
	m_bIsRunning		= FALSE;
	m_hChangeEvnet		= INVALID_HANDLE_VALUE;

	m_i32XTrapMapIdx	= 0;
	memset( m_ui8XTrapMap, 0, sizeof(UINT8)*CSFILE_NUM*XTRAP_CS4_BUFSIZE_MAP );
}

CXTrapModule::~CXTrapModule()
{
	OnDestroy();
}

BOOL	CXTrapModule::OnCreate( char* strFileName )
{
	strcpy( m_strFileName, strFileName );
	
	char strDrive[4];
	char strFile[MAX_PATH];
	char strDir[MAX_PATH];
	char strExt[MAX_PATH];

	_tsplitpath_s( m_strFileName, strDrive, 4, strDir, MAX_PATH, strFile, MAX_PATH, strExt, MAX_PATH);
	i3String::Format( m_strDirName, MAX_PATH, "%s%s", strDrive, strDir );

	if( FALSE == _FileOpen() )												return FALSE;

	// Load Dll
	UINT32 ui32Res = XTrap_S_LoadDll();
	if ( 0 != ui32Res ) 
	{
		I3TRACE("XTrap_S_LoadDll() failed \n"); 
		return FALSE;
	}
	
	// Xtrap Start
	ui32Res = XTrap_S_Start( XTRAP_TIMEOUT, CSFILE_NUM, m_ui8XTrapMap, NULL );
	switch( ui32Res )
	{
	case XTRAP_API_RETURN_OK:
	case XTRAP_API_RETURN_INVALID_FILEVERSION:			// 실패는 아닙니다.
		break;
	default:
		{
			I3TRACE("XTrap_S_Start() failed \n"); 
			return FALSE;
		}
	}

	//----------------------------------------------------------------
	// Xtrap TheMeDa
	//----------------------------------------------------------------
	// 현재 WiseLogic측에서 TheMeDa 업데이트를 하지 않아서
	// 아직 동작하지 않는 관계로 주석처리 합니다.
	//											2012.11.30 김용수
	//----------------------------------------------------------------

	// WiseLogic 측에서 TheMida 업데이트를 진행하는 것이 아닌 우리 클라이언트 파일이 TheMida 를 안쓰고 있어서 뺀것이고,
	// 핵실드 사용 국가 외에 국가에도 모두 사용하도록 하여 추가합니다.
	// 추가로 값 확인 처리가 되어있지 않아 추가합니다. - 서동권
	if( 0 != XTrap_S_SetActiveCode(XTRAP_ACTIVE_CODE_LEVEL2) )	return FALSE;

	//----------------------------------------------------------------

	DWORD dwThreadID;
	m_hThread = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc , this, 0, (DWORD *)&dwThreadID );
	if( NULL == m_hThread )													return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL) )	return FALSE;
	
	return TRUE;
}

void	CXTrapModule::OnDestroy()
{
	m_bIsRunning = FALSE;
	FindCloseChangeNotification( m_hChangeEvnet );
	::WaitForSingleObject( m_hThread, INFINITE );

	XTrap_S_FreeDll();
}

void	CXTrapModule::OnRunning()
{
	g_pLog->AddLogThread( L"MDXTR", 0, GetCurrentThreadId() ); // WriteLog시 자동으로 WorkCounter를 찾아주기때문에 반환 값을 따로 보관하지 않습니다. 

	m_bIsRunning = TRUE;

	while( m_bIsRunning )
	{
		g_pLog->WriteLog( L"[GAMEGUARD]Check Dir" );

		m_hChangeEvnet = FindFirstChangeNotification( m_strDirName, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE );
		if( INVALID_HANDLE_VALUE == m_hChangeEvnet )
		{
			g_pLog->WriteLog( L"[GAMEGUARD]FindFirstChangeNotification Fail" );

			::WaitForSingleObject( m_hThread, 1 );
			continue;
		}

		::WaitForSingleObject( m_hChangeEvnet, INFINITE );
		if( FALSE == m_bIsRunning ) break;

		::WaitForSingleObject( m_hThread, 30000 );
		// Debug 모드일때 폴더에 있는 svn 으로 인해 가끔씩 ChangeEvent 가 발생할때가 있습니다.

		if( _FileOpen() )
		{
			g_pLog->WriteLog( L"[GAMEGUARD]File Open Success count:%d", m_i32XTrapMapIdx );
		}
		else
		{
			g_pLog->WriteLog( L"[GAMEGUARD]File Open Fail" );
		}

		FindNextChangeNotification( m_hChangeEvnet );
	}
}

BOOL	CXTrapModule::_FileOpen()
{
	for( INT32 i = m_i32XTrapMapIdx-1 ; i >= 0 ; i-- )
	{
		memcpy( m_ui8XTrapMap[i+1], m_ui8XTrapMap[i], sizeof(UINT8)*XTRAP_CS4_BUFSIZE_MAP );
	}

	FILE	*fl;
	fl = fopen( m_strFileName, "rb");
	if( NULL == fl )
	{
		I3TRACE("Xtrap Mapfile1 Load failed \n");
		
		return FALSE;
	}
	fread( m_ui8XTrapMap[0], XTRAP_CS4_BUFSIZE_MAP, 1, fl );
	fclose(fl);

	if( m_i32XTrapMapIdx < (CSFILE_NUM-1) )
	{
		m_i32XTrapMapIdx++;
	}

	return TRUE;
}
