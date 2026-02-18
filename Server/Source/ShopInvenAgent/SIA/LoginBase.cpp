#include "pch.h"
#include "LoginBase.h"
#include "ModuleTask.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((CLoginBase*)pPointer)->OnRunning();
	return 0;
}

CLoginBase::CLoginBase(void)
{
	m_hThread		= NULL;
	m_bRunning		= FALSE;
	m_pPushBuffer	= NULL;
	m_pPopBuffer	= NULL;
}

CLoginBase::~CLoginBase(void)
{
	OnDestroy(); 
}

BOOL CLoginBase::OnCreate( INT32 i32idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx )
{
	m_i32Idx				= i32idx;
	m_ui8LinkMethod			= ui8MethodIdx;
	m_i32WorkMaxCount		= ui32BufferMaxCount;

	m_pPushBuffer			= i3RingBuffer::NewObject();
	if( NULL == m_pPushBuffer )														return FALSE;
	if( FALSE == m_pPushBuffer->Create( sizeof(LOGIN_INPUT), m_i32WorkMaxCount ))	return FALSE;

	m_pPopBuffer			= i3RingBuffer::NewObject();
	if( NULL == m_pPopBuffer )														return FALSE;
	if( FALSE == m_pPopBuffer->Create( sizeof(LOGIN_OUTPUT), m_i32WorkMaxCount ))	return FALSE;

	if( FALSE == _Create() )														return FALSE;

	// Create Thread
	m_hThread  = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc, this, 0, (DWORD *)&m_ui32ThreadID );
	if( NULL == m_hThread )															return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_LINK) )				return FALSE;

	return TRUE;
}

void CLoginBase::OnDestroy(void)
{	
	m_bRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	_Destroy();

	I3_SAFE_RELEASE( m_pPopBuffer );
	I3_SAFE_RELEASE( m_pPushBuffer );
}

BOOL CLoginBase::_Create()
{
	return TRUE;
}

void CLoginBase::_Destroy()
{
}

INT32 CLoginBase::Push_T( LOGIN_INPUT* pInputData )
{
	if( RING_BUFFER_SUCCESS != m_pPushBuffer->Push( pInputData ) )	return -1;
	return m_pPushBuffer->GetBufferCount();
}

BOOL CLoginBase::_WorkProcess( LOGIN_INPUT *pInput, LOGIN_OUTPUT* pOutput )
{pInput;pOutput;
	return FALSE; 
}

void CLoginBase::_LoginExtension( LOGIN_INPUT *pInput, LOGIN_OUTPUT * pOutput )
{	pInput;	pOutput;
//	if( g_pConfig->m_bPCCafeActive )
//	{
//		UINT8 ui8Cafe;
//		if( g_pModulePcList->IsPcCafe( pInput->_Ip, ui8Cafe ) )
//		{
//			pOutput->_pcCafe	= ui8Cafe;
//		}
//		else
//		{
//			pOutput->_pcCafe	= PC_NOT_CAFE;
//		}
//	}
//	else
//	{
//		pOutput->_pcCafe		= PC_NOT_CAFE;
//	}
}

UINT32 CLoginBase::OnRunning()
{
	wchar_t wstrTemp[ MAX_PATH ];
	i3String::Format(wstrTemp, MAX_PATH, L"LOGIN%02d", m_i32Idx );
	g_pLog->AddLogThread(wstrTemp, 0, m_ui32ThreadID );

	INT32			i32Count;
	INT32			i32WorkCount; 
	LOGIN_INPUT*	pLoginIn;
	LOGIN_OUTPUT*	pLoginOut;
	BOOL			bWorking;

	m_bRunning = TRUE;
	while( m_bRunning )
	{
		// Thread 에서 추가 작업 하실게 있으면 이곳에서 작업해 주세요.
		bWorking = OnRunningEx();

		i32WorkCount = m_pPushBuffer->GetBufferCount();
		if( 0 < i32WorkCount )
		{
			for( i32Count = 0 ; i32Count < i32WorkCount ; i32Count++ )
			{
				// 패킷 처리 테스트코드
				LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
				QueryPerformanceFrequency(&stQP_Freq_Begin);
				QueryPerformanceCounter(&stQP_Count_Begin);

				pLoginIn	= (LOGIN_INPUT*)m_pPushBuffer->Pop();
				pLoginOut	= (LOGIN_OUTPUT*)m_pPopBuffer->PushPointer();
				if( NULL == pLoginOut )
				{
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[%d] Login OutBuffer Full Type:%d", m_i32Idx, pLoginIn->m_eCommandType);
					bWorking = true;
					break;
				}

				pLoginOut->m_ui8PBVer				= pLoginIn->m_ui8PBVer;
				pLoginOut->m_i32TransServerIdx		= pLoginIn->m_i32TransServerIdx;
				pLoginOut->m_eCommandType			= pLoginIn->m_eCommandType;
				pLoginOut->m_ui32LoginTime			= GetTickCount();
				pLoginOut->m_i32MainLoginBufferIdx	= pLoginIn->m_i32MainLoginBufferIdx;
				pLoginOut->m_ui8LinkMethod			= pLoginIn->m_ui8LinkMethod;
				pLoginOut->m_ui32AuthKey			= pLoginIn->m_ui32AuthKey;

				if( _WorkProcess( pLoginIn, pLoginOut ) )
				{
					m_pPopBuffer->PushPointerIdx();
				}

				// 패킷 처리 테스트코드
				QueryPerformanceCounter(&stQP_Count_End);
				stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart;

				REAL64 r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart;

				g_pStatistics->AddStat(STAT_TYPE_LINK_LOGIN, m_i32Idx, r64ElepseTime);

				if(r64ElepseTime > CHECK_TIMEOUT) // 500ms
				{
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"[%d] Worker Type:%d, Time Over : %.6lf (sec ) / Count : %d", m_i32Idx, pLoginIn->m_eCommandType, r64ElepseTime, i32WorkCount);
				}
				m_pPushBuffer->PopIdx();
				bWorking = true; 
			}
		}

		if( false == bWorking )	::WaitForSingleObject( m_hThread, 1 );
	}
	return 0; 
}

BOOL CLoginBase::OnRunningEx()
{
	return FALSE;
}