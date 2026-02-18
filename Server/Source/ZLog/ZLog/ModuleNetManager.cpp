#include "pch.h"
#include "ModuleNetManager.h"
#include "ModuleFileManager.h"
#include "ModuleNetServer.h"
#include <process.h>

static unsigned __stdcall _DefThreadProc( void * pPointer )
{
	((CModuleNetManager*)pPointer)->Working();
	_endthreadex(0);
	return 0;
}

CModuleNetManager::CModuleNetManager()
{
	m_hThread			= NULL;
	m_bIsRunning		= false;

	m_hServerSocket		= INVALID_SOCKET;
 
	m_ui32ServerIP		= 0;
	m_ui16ServerPort	= 0;

	m_ui32ThreadID		= 0;

	m_i32RecvCount		= 0;
	m_pui32RecvIP		= nullptr;

	m_ppNetServer		= nullptr;
}

CModuleNetManager::~CModuleNetManager()
{
	Destroy();
}

bool CModuleNetManager::Create( UINT32 ui32ServerIP, UINT16 ui16Port, INT32 i32ServerCount, UINT32 * pui32RecvIP, CModuleFileManager* pFileManager  )
{
	m_ui32ServerIP		= ui32ServerIP;
	m_ui16ServerPort	= ui16Port;

	m_i32RecvCount		= i32ServerCount;

	// Set Recv IP List
	m_pui32RecvIP		= new UINT32[ m_i32RecvCount ];
	if( NULL == m_pui32RecvIP )																return false;
	i3mem::Copy( m_pui32RecvIP, pui32RecvIP, sizeof(UINT32)*m_i32RecvCount );
	
	// Create Server Module
	m_ppNetServer	= new CModuleNetServer*[ m_i32RecvCount ];
	if( NULL == m_ppNetServer )																return false;
	for( INT32 i = 0; i < m_i32RecvCount; i++ )
	{
		m_ppNetServer[i]	= CModuleNetServer::NewObject();
		if( NULL == m_ppNetServer[i] )														return false;
		if( false == m_ppNetServer[i]->Create( i, pFileManager ) )							return false;
	}

	// Create Listen Socket 
	if( FALSE == m_sServerSocket.OnCreate( m_ui32ServerIP, m_ui16ServerPort ) )				return FALSE;

	// Make Thread 
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, _DefThreadProc, this, 0, &m_ui32ThreadID);
	if( NULL == m_hThread )																	return false; 	
	if( false == SetThreadPriority( m_hThread, THREAD_PRIORITY_NORMAL) )					return false; 

	return true; 
}

void CModuleNetManager::Destroy(void)
{
	m_bIsRunning = false;
	::WaitForSingleObject( m_hThread, INFINITE );
	CloseHandle( m_hThread );

	if( m_ppNetServer )
	{
		for( INT32 i = 0 ; i < m_i32RecvCount ; i++ )
		{
			I3_SAFE_RELEASE( m_ppNetServer[i] );
		}
		I3_SAFE_DELETE_ARRAY( m_ppNetServer );
	}
	I3_SAFE_DELETE_ARRAY( m_pui32RecvIP );

	m_sServerSocket.OnDestroy();
}

void CModuleNetManager::Working()
{
	g_pLog->AddLogThread( L"NETM", 0, m_ui32ThreadID );

	SOCKET hAcceptSocket; 
	struct sockaddr_in FromAddr;
	struct timeval timeout;
	INT32 i32AddrSize;
	bool	bWorked;
	wchar_t wstrIP[MAX_PATH];

	m_bIsRunning = true;
	while( m_bIsRunning )
	{
		bWorked = false;

		timeout.tv_sec  = 0;
		timeout.tv_usec = 1;
		i32AddrSize = sizeof(sockaddr_in);
		hAcceptSocket = m_sServerSocket.AcceptConnection( &timeout, &FromAddr, &i32AddrSize ); 
		if( INVALID_SOCKET == hAcceptSocket )	goto e_Exit;

		bWorked = true;

		UINT32 ui32ConnectIp = FromAddr.sin_addr.s_addr;

		INT32 i32ServerIdx = -1;
		for( INT32 i = 0 ; i < m_i32RecvCount ; ++i )
		{
			// 해당 IP에서 연결되지 않은 인덱스를 찾는다.
			if( m_pui32RecvIP[i] != ui32ConnectIp )		continue;
			if( m_ppNetServer[i]->IsConnected() )		continue;
			i32ServerIdx = i;
			break;
		}

		struct in_addr ip;
		ip.s_addr = ui32ConnectIp;

		MultiByteToWideChar(CP_ACP, NULL, inet_ntoa(ip), -1, wstrIP, MAX_PATH);

		// loop 를 다 돌았지만 IP 가 없다.. 해킹?
		if( -1 == i32ServerIdx )
		{
			i3Net::DestroySocket( hAcceptSocket ); 
			g_pLog->WriteLog( L"[Err] Unknown Server Connect : IP=%s", wstrIP);
			goto e_Exit;
		}

		// 이미 접속 중이라면.. 기존 네트웍을 끊어 버리고 새로운 Log 를 받을 준비를 합니다. 생각좀 해봅니다..
		if( m_ppNetServer[ i32ServerIdx ]->IsConnected() )
		{
			i3Net::DestroySocket( hAcceptSocket );
			g_pLog->WriteLog( L"[Err] Server Connected : Id=%d, IP=%s", i32ServerIdx, wstrIP);
			goto e_Exit;
		}

		// Socket 를 할당해 줍니다.
		if( false == m_ppNetServer[ i32ServerIdx ]->SetServer( hAcceptSocket ) )
		{
			i3Net::DestroySocket( hAcceptSocket );
			g_pLog->WriteLog( L"[Err] Server Connect Fail :  Id=%d, IP=%s", i32ServerIdx, wstrIP);
			goto e_Exit;
		}

		// 성공입니다. Message 받을 준비를 합니다.
		g_pLog->WriteLog( L"[SUC] Server Connect : Id=%d, IP=%s", i32ServerIdx, wstrIP);

e_Exit:
		::WaitForSingleObject(m_hThread, 1);
	}
}