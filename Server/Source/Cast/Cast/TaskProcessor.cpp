#include "pch.h"
#include "TaskProcessor.h"

I3_CLASS_INSTANCE(CTaskProcessor, i3Thread);

CTaskProcessor* g_pTaskProcessor = NULL;
INT32	i32CreateCnt = 0;

CTaskProcessor::CTaskProcessor()
{
	m_bIsRunning				= FALSE;
	m_pSessionManager			= NULL;
	m_pServerContext			= NULL;
}

CTaskProcessor::~CTaskProcessor()
{
	m_bIsRunning = FALSE;

	i3Thread::WaitForFinish();

	I3_SAFE_RELEASE( m_pServerContext );
	I3_SAFE_RELEASE( m_pSessionManager );
}

BOOL CTaskProcessor::Create( UINT32 ui32DataSize )
{
	m_pSessionManager = CUserSessionManager::NewObject();
	if( ( NULL == m_pSessionManager ) ||
		( FALSE == m_pSessionManager->OnCreate( g_pConfig->GetGameServerCount(), g_pConfig->GetBattleServerCount(), g_pConfig->GetIOCPThreadCount() ) ) )
	{
		g_pLog->WriteLog( L"[FAIL] Server Session Create"  );
		return FALSE;
	}
	g_pLog->WriteLog( L"[SUCC] Server Session Create"  );

	UINT8 ui8TimeOut = 5;
	m_pServerContext = i3NetworkServerContext::NewObject();
	if( (NULL == m_pServerContext) || 
		(FALSE == m_pServerContext->OnCreate( 1, &g_pConfig->m_ui32IP, &g_pConfig->m_ui16Port, &ui8TimeOut, g_pConfig->GetIOCPThreadCount(), m_pSessionManager ) ))
	{
		g_pLog->WriteLog( L"[FAIL] Server Context Create"  );
		return SERVER_ERROR_CONTEXT;
	}
	g_pLog->WriteLog( L"[SUCC] Server Context Create"  );

	//Add LogThread
	for (INT32 i = 0; i < g_pConfig->GetIOCPThreadCount(); ++i)
	{
		UINT32 ui32ThreadID = m_pServerContext->GetCompletionHandler()->GetIocpWorker(i)->GetThreadID();

		wchar_t wstrLog[MAX_PATH];
		i3String::Format(wstrLog, MAX_PATH, L"US%02d", i);

		g_pLog->AddLogThread(wstrLog, 0, (DWORD)ui32ThreadID);
	}

	if (FALSE == i3Thread::Create("TaskProcessor", 0, 4096, NULL, PRIORITY_HIGHEST))
	{
		return FALSE;
	}	
	
	return TRUE;
}

UINT32 CTaskProcessor::OnRunning(void* pUserData)
{
	g_pLog->AddLogThread(L"TASK", 0, m_ThreadID);

	m_bIsRunning = TRUE;
	BOOL bIsWorking;

	while(m_bIsRunning)
	{
		bIsWorking = FALSE;

		m_pSessionManager->OnUpdate();

		// Packet Ã³¸®
		if( _TaskProcessorPacket() )
		{
			bIsWorking = TRUE;
		}

		if( FALSE == bIsWorking )
		{
			::WaitForSingleObject( m_Handle, 1 );
		}

		
	}

	return 0;
}
