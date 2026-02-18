#include "pch.h"
#include "PCCafeBase.h"
#include "ModuleTask.h"

static UINT32 _DefThreadProc( void * pPointer )
{
	((CPCCafeBase*)pPointer)->OnBaseRunning();
	return 0;
}

CPCCafeBase::CPCCafeBase()
{
	m_bRunning		= FALSE;

	m_pPushBuffer	= NULL;
	m_pPopBuffer	= NULL;
}

CPCCafeBase::~CPCCafeBase()
{
	ReleaseThread();
	OnDestroy();
}

BOOL CPCCafeBase::OnCreate( INT32 i32Idx )
{
	m_i32Idx = i32Idx;

	m_pPushBuffer = i3RingBuffer::NewObject();
	if( NULL == m_pPushBuffer )																		return FALSE;
	if( FALSE == m_pPushBuffer->Create( sizeof(PCCAFE_INPUT), g_pConfig->m_i32PCCafeBufferCount ))	return FALSE;

	m_pPopBuffer = i3RingBuffer::NewObject();
	if( NULL == m_pPopBuffer )																		return FALSE;
	if( FALSE == m_pPopBuffer->Create( sizeof(PCCAFE_OUTPUT), g_pConfig->m_i32PCCafeBufferCount ))	return FALSE;

	m_hThread  = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc, this, 0, (DWORD *)&m_ui32ThreadID );
	if( NULL == m_hThread )																			return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_LINK) )								return FALSE;

	return TRUE;
}

void CPCCafeBase::OnDestroy()
{
	m_bRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );

	I3_SAFE_RELEASE( m_pPopBuffer );
	I3_SAFE_RELEASE( m_pPushBuffer );
}

void CPCCafeBase::ReleaseThread()
{
	m_bRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );
}

void CPCCafeBase::OnBaseRunning()
{
	wchar_t wstrTemp[ MAX_PATH ];
	i3String::Format(wstrTemp, MAX_PATH, L"PC%02d", m_i32Idx );
	g_pLog->AddLogThread(wstrTemp, 0, m_ui32ThreadID );

	BOOL			bWorking;
	BOOL			bOutBufferUse;
	PCCAFE_INPUT * pInput;
	PCCAFE_OUTPUT* pOutput;
	
	m_bRunning = TRUE;
	while( m_bRunning )
	{
		bWorking = FALSE;
		if( 0 < m_pPushBuffer->GetBufferCount() )
		{
			
			pInput  = (PCCAFE_INPUT*)m_pPushBuffer->Pop();
			pOutput = (PCCAFE_OUTPUT*)m_pPopBuffer->PushPointer();
			if( pOutput )
			{
				// 패킷 처리 테스트코드
				LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
				QueryPerformanceFrequency(&stQP_Freq_Begin);
				QueryPerformanceCounter(&stQP_Count_Begin);

				pOutput->m_i32LoginBufferIdx = pInput->m_i32LoginBufferIdx;
				bOutBufferUse = _WorkProcess( pInput, pOutput );

				if( bOutBufferUse )
					m_pPopBuffer->PushPointerIdx();

				m_pPushBuffer->PopIdx();

				bWorking = TRUE;

				// Link End Time
				// 패킷 처리 테스트코드
				QueryPerformanceCounter(&stQP_Count_End);
				stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart;

				REAL64 r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart;

				g_pStatistics->AddStat(STAT_TYPE_LINK_PC_CAFE, m_i32Idx, r64ElepseTime);

				if (r64ElepseTime > CHECK_TIMEOUT)
				{
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"---[%d]PCCafe Time Over : %.6lf", m_i32Idx, r64ElepseTime);
				}
			}
			else
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_TASK, L"[ERROR] Link Output Buffer Full 1" );
			}
		}
		OnRunning();

		if( FALSE == bWorking ) ::WaitForSingleObject( m_hThread, 1 );
	}
}

BOOL CPCCafeBase::OnRunning()
{
	return FALSE;
}

BOOL CPCCafeBase::_WorkProcess( PCCAFE_INPUT * pInput, PCCAFE_OUTPUT* pOutput )
{	pInput;	pOutput;
	return FALSE;
}
