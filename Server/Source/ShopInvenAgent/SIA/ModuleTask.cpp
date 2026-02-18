#include "pch.h"
#include "ModuleTask.h"
#include "MOduleNet.h"

CModuleTask * g_pModuleTask = NULL; 

static UINT32 _DefThreadProc( void * pPointer )
{
	pPointer;
	g_pModuleTask->OnWorking();
	return 0;
}

CModuleTask::CModuleTask()
{
	m_hThread				= NULL;
	m_bIsRunning			= FALSE;

	m_ppModuleLogin			= NULL;
	m_ppModuleBilling		= NULL;
	m_pModulePCCafe			= NULL;
	m_pModuleAccountTrans	= NULL;
	m_pMainBufferBuy		= NULL;
	m_pMainBufferLogin		= NULL;
}

CModuleTask::~CModuleTask()
{
	OnDestroy(); 
}

INT32 CModuleTask::OnCreate_M(void)
{
	ResetCounter(); 
	m_ui32LogFileHour = i3ThreadTimer::LocalTime().GetHour();

	//LoginMethodCount 설정값이 잘못된 경우 서버를 종료합니다.
	if ( 0 == g_pConfig->m_ui8LoginMethodCount ) return EVENT_FAIL_CREATE_LOGIN;
	
	m_ppModuleLogin = (CLoginManager **)i3MemAlloc(sizeof(CLoginManager*) * LINK_METHOD_MAX);
	i3mem::FillZero(m_ppModuleLogin, sizeof(CLoginManager*) * LINK_METHOD_MAX);
	for( UINT8 i = 0; i < g_pConfig->m_ui8LoginMethodCount; ++i )
	{
		LINK_METHOD	eLogin = g_pConfig->m_aeLoginMethod[i];

		//Login 연동설정이 중복된경우 가장 처음 설정한 것만 적용 합니다.
		if ( NULL != m_ppModuleLogin[eLogin] ) continue;

		m_ppModuleLogin[eLogin] = new CLoginManager;
		if ( NULL == m_ppModuleLogin[eLogin] )					return EVENT_FAIL_CREATE_LOGIN;
		if ( FALSE == m_ppModuleLogin[eLogin]->OnCreate(i) )	return EVENT_FAIL_CREATE_LOGIN;
	}

	//BillingMethodCount 설정값이 잘못된 경우 서버를 종료합니다.
	if ( 0 == g_pConfig->m_ui8BillingMethodCount ) return EVENT_FAIL_CREATE_LINK;

	m_ppModuleBilling = (CBillingManager **)i3MemAlloc(sizeof(CBillingManager*) * LINK_METHOD_MAX);
	i3mem::FillZero(m_ppModuleBilling, sizeof(CBillingManager*) * LINK_METHOD_MAX);

	for( UINT8 i = 0; i < g_pConfig->m_ui8BillingMethodCount; ++i )
	{
		LINK_METHOD	eLogin = g_pConfig->m_aeBillingMethod[i];

		//Billing 연동설정이 중복된경우 가장 처음 설정한 것만 적용 합니다.
		if (NULL != m_ppModuleBilling[eLogin]) continue;

		m_ppModuleBilling[eLogin] = new CBillingManager;
		if ( NULL == m_ppModuleBilling[eLogin] )				return EVENT_FAIL_CREATE_LINK;
		if ( FALSE == m_ppModuleBilling[eLogin]->Create(i) )	return EVENT_FAIL_CREATE_LINK;
	}
	
	m_pModulePCCafe			= new CPCCafeManager;
	if( NULL == m_pModulePCCafe )																		return EVENT_FAIL_CREATE_PCCAFE;
	if( FALSE == m_pModulePCCafe->Create() )															return EVENT_FAIL_CREATE_PCCAFE;

	m_pModuleAccountTrans	= new CAccountTrans; 
	if( NULL == m_pModuleAccountTrans )																	return EVENT_FAIL_CREATE_LINK;
	if( FALSE == m_pModuleAccountTrans->Create())														return EVENT_FAIL_CREATE_LINK;

	m_pMainBufferBuy		= new CMainStreamBuffer_Buy;
	if( NULL == m_pMainBufferBuy )																		return EVENT_FAIL_CREATE_BUY_BUFFER;
	if( FALSE == m_pMainBufferBuy->CreateBuffer(
		g_pConfig->m_i32BillingThreadCount*g_pConfig->m_i32BillingBufferCount ))						return EVENT_FAIL_CREATE_BUY_BUFFER;

	m_pMainBufferLogin		= new CMainStreamBuffer_Login;
	if( NULL == m_pMainBufferLogin )																	return EVENT_FAIL_CREATE_LOGIN_BUFFER;
	if( EV_FAILED( m_pMainBufferLogin->CreateBuffer( 
		g_pConfig->m_i32LoginThreadCount*g_pConfig->m_i32LogInBufferCount ) ) )							return EVENT_FAIL_CREATE_LOGIN_BUFFER;

	//Create Thread 
	m_hThread = CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc ,this ,0 ,(DWORD *)&m_ui32ThreadID);
	if(m_hThread == NULL)
	{
		return SERVER_ERROR_TASK; 
	}

	//Set Thread Priority
	if( !SetThreadPriority( m_hThread, THREAD_PRIORITY_TASK) )
	{
		return SERVER_ERROR_TASK; 
	}

	return EVENT_SUCCESS; 
}

bool CModuleTask::OnDestroy(void)
{
	//Destroy Thread 
	m_bIsRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );
	for (INT32 i = 0; i < LINK_METHOD_MAX; ++i)
	{
		I3_SAFE_DELETE( m_ppModuleLogin[i] );
		I3_SAFE_DELETE( m_ppModuleBilling[i] );
	}

	I3MEM_SAFE_FREE( m_ppModuleLogin );
	I3MEM_SAFE_FREE( m_ppModuleBilling );
	I3_SAFE_DELETE( m_pModulePCCafe );	
	I3_SAFE_DELETE( m_pModuleAccountTrans );
	I3_SAFE_DELETE( m_pMainBufferBuy );
	I3_SAFE_DELETE( m_pMainBufferLogin );

	return true; 
}

void CModuleTask::OnWorking(void)
{
	g_pLog->AddLogThread( L"TASK", 0, m_ui32ThreadID );

	//잠시 더미로 만들어요. 	
	BOOL bWork; 
	INT32	i32ProfileLogHour = i3ThreadTimer::LocalTime().GetHour();

	m_bIsRunning = TRUE;
	while( m_bIsRunning )
	{
		bWork = FALSE; 

		////////////////////////////////////////////////////////////////////////////////
		//1. Packet Parshing 
		UINT32 ui32ServerIdx;
		if( 0 < g_pModuleNet->GetRecvCount() )
		{
			NET_PACKET_BUFFER * pPacket = g_pModuleNet->RecvPop( &ui32ServerIdx );
			_ParshingNet(ui32ServerIdx, pPacket);
			g_pModuleNet->RecvPopIdx();

			bWork = TRUE; 
			m_ui32WorkCount_Net++; 
		}
		
		// Login
		for ( UINT8 i = 0; i < LINK_METHOD_MAX; ++i )
		{
			if (NULL == m_ppModuleLogin[i]) continue;

			if (0 < m_ppModuleLogin[i]->PopCount_T())
			{
				LOGIN_OUTPUT* pLoginOut = m_ppModuleLogin[i]->Pop_T();
				m_pMainBufferLogin->SetRecvResult(pLoginOut);			// _ParshingLogin( pLoginOut ) 대신 사용
				m_ppModuleLogin[i]->PopIdx_T();
				
			}
		}

		// Billing
		for ( UINT8 i = 0; i < LINK_METHOD_MAX; ++i )
		{
			if (NULL == m_ppModuleBilling[i]) continue;

		
			if (0 < m_ppModuleBilling[i]->PopCount_T())
			{
				BILLING_OUTPUT* pLinkOut = (BILLING_OUTPUT*)m_ppModuleBilling[i]->PopData_T();
				_ParshingBilling( pLinkOut, i );
				m_ppModuleBilling[i]->PopIdx_T();		
				bWork = TRUE;
				m_ui32WorkCount_Billing++;			
			}
		}

		// PCCafe
		if( 0 < m_pModulePCCafe->PopCount_T() )
		{
			PCCAFE_OUTPUT* pPCCafeOut = m_pModulePCCafe->PopData_T();
			_ParshingPCCafe( pPCCafeOut );
			m_pModulePCCafe->PopIdx_T();
		}
		
		if ( m_pMainBufferBuy->UpdateBuffer() )
		{
			bWork = TRUE;
		}

		if ( m_pMainBufferLogin->UpdateBuffer() )
		{
			bWork = TRUE;
		}

		//한일이 없다면 잠시 휴식
		if(bWork)
		{
			m_ui32WorkCount++;
		}
		else
		{
			m_ui32SleepCount++;
			::WaitForSingleObject( m_hThread, 1 );
		}

		if ( i32ProfileLogHour != i3ThreadTimer::LocalTime().GetHour() )
		{	// 1시간동안 링크 처리건수 및 타임 기록
			i32ProfileLogHour = i3ThreadTimer::LocalTime().GetHour();

			STAT_DATA	astStat[STAT_TYPE_MAX];
			i3mem::FillZero(astStat, sizeof(STAT_DATA) * STAT_TYPE_MAX);

			if (g_pStatistics)
			{
				g_pStatistics->GetAllStat(astStat);
				g_pStatistics->Reset();
			}

			// 로그기록
			g_pLog->WriteLog(WARNING_TYPE_NORMAL, WARNING_NO, L"[STATISTICS]\r\n"
				L"========== COPY DOWN =========== \r\n"
				L"{\"WORKTIME\": \r\n"
				L"{\"Date\":\"%04d-%02d-%02d\",\r\n"
				L"\"Data\":{\r\n"
				L"\t\"Type\":\"%d\",\r\n"			 
				L"\t\"Count\":\"%I64d\",\r\n"		 
				L"\t\"Time\":\"%.6lf\"},{\r\n"
				L"\t\"Type\":\"%d\",\r\n"
				L"\t\"Count\":\"%I64d\",\r\n"
				L"\t\"Time\":\"%.6lf\"},{\r\n"
				L"\t\"Type\":\"%d\",\r\n"
				L"\t\"Count\":\"%I64d\",\r\n"
				L"\t\"Time\":\"%.6lf\" } \r\n" 
				L"}}\r\n"
				L"========== COPY UP =========== \r\n", 
				i3ThreadTimer::LocalTime().GetYear(),
				i3ThreadTimer::LocalTime().GetMonth(),
				i3ThreadTimer::LocalTime().GetDay(),
				STAT_TYPE_LINK_LOGIN,
				astStat[STAT_TYPE_LINK_LOGIN].m_ui64Count,
				astStat[STAT_TYPE_LINK_LOGIN].m_r64Time,
				STAT_TYPE_LINK_BILLING,
				astStat[STAT_TYPE_LINK_BILLING].m_ui64Count,
				astStat[STAT_TYPE_LINK_BILLING].m_r64Time,
				STAT_TYPE_LINK_PC_CAFE,
				astStat[STAT_TYPE_LINK_PC_CAFE].m_ui64Count,
				astStat[STAT_TYPE_LINK_PC_CAFE].m_r64Time
			);

		}
	}
	return; 
}

 