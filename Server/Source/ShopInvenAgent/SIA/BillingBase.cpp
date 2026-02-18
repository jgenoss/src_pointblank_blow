#include "pch.h"
#include "BillingBase.h"
#include "ModuleTask.h"

#define BILLING_WORK_LOG_TIME	10*60

static UINT32 _DefThreadProc( void * pPointer )
{
	((CBillingBase*)pPointer)->OnBaseRunning();
	return 0;
}

CBillingBase::CBillingBase()
{
	m_bRunning		= FALSE;

	m_pPushBuffer	= NULL;
	m_pPopBuffer	= NULL;
}

CBillingBase::~CBillingBase()
{
	ReleaseThread();
	OnDestroy();
}

BOOL CBillingBase::OnCreate( INT32 i32Idx, UINT8 ui8MethodIdx )
{
	m_i32Idx = i32Idx;
	m_ui8LinkMethod = ui8MethodIdx;

	m_pPushBuffer = i3RingBuffer::NewObject();
	if( NULL == m_pPushBuffer )																			return FALSE;
	if( FALSE == m_pPushBuffer->Create( sizeof(BILLING_INPUT), g_pConfig->m_i32BillingBufferCount ))	return FALSE;

	m_pPopBuffer = i3RingBuffer::NewObject();
	if( NULL == m_pPopBuffer )																			return FALSE;
	if( FALSE == m_pPopBuffer->Create( sizeof(BILLING_OUTPUT), g_pConfig->m_i32BillingBufferCount ))	return FALSE;

	m_hThread  = CreateThread( NULL, 4096, (LPTHREAD_START_ROUTINE)_DefThreadProc, this, 0, (DWORD *)&m_ui32ThreadID );
	if( NULL == m_hThread )																				return FALSE;
	if( FALSE == SetThreadPriority( m_hThread, THREAD_PRIORITY_LINK) )									return FALSE;

	return TRUE;
}

void CBillingBase::OnDestroy()
{
	I3_SAFE_RELEASE( m_pPopBuffer );
	I3_SAFE_RELEASE( m_pPushBuffer );
}

void CBillingBase::ReleaseThread()
{
	m_bRunning = FALSE;
	::WaitForSingleObject( m_hThread, INFINITE );
}

void CBillingBase::OnBaseRunning()
{
	wchar_t wstrTemp[ MAX_PATH ];
	i3String::Format(wstrTemp, MAX_PATH, L"BILLING%02d", m_i32Idx );
	g_pLog->AddLogThread(wstrTemp, 0, m_ui32ThreadID );

	BOOL			bWorking;
	BOOL			bOutBufferUse;
	BILLING_INPUT * pInput;
	BILLING_OUTPUT* pOutput;

	m_bRunning = TRUE;
	while( m_bRunning )
	{
		bWorking = FALSE;
		if( 0 < m_pPushBuffer->GetBufferCount() )
		{

			pInput  = (BILLING_INPUT*)m_pPushBuffer->Pop();
			pOutput = (BILLING_OUTPUT*)m_pPopBuffer->PushPointer();
			if( pOutput )
			{

				i3mem::Fill( pOutput->m_ai32Rv, -1, sizeof( INT32 ) * MAX_SHOP_GOODS_BUY_COUNT );
			
				pOutput->m_ui32ServerIdx		= pInput->m_ui32ServerIdx;
				pOutput->m_ui32MainBufferIdx	= pInput->m_ui32MainBufferIdx;
				pOutput->m_eLinkType			= pInput->m_eLinkType;
				pOutput->m_TUID					= pInput->m_TUID;
				pOutput->m_ui8GoodsCount		= pInput->m_ui8GoodsCount;

				// 구매가 아니라면 GoodsCount는 1로 고정시킵니다.
				if( LINK_TYPE_BUY_GOODS != pOutput->m_eLinkType ) pOutput->m_ui8GoodsCount = 1;

				// 패킷 처리 테스트코드
				LARGE_INTEGER stQP_Freq_Begin, stQP_Count_Begin, stQP_Count_End, stQP_Result;
				QueryPerformanceFrequency(&stQP_Freq_Begin);
				QueryPerformanceCounter(&stQP_Count_Begin);

				bOutBufferUse = FALSE;
				for( UINT8 i = 0 ; i < pOutput->m_ui8GoodsCount ; i++ )
				{
					bOutBufferUse = _WorkProcess( pInput, pOutput, i );
				}

				if( bOutBufferUse )
				{
					m_pPopBuffer->PushPointerIdx();
				}

				m_pPushBuffer->PopIdx();

				bWorking = TRUE;

				// Link End Time

				// 패킷 처리 테스트코드
				QueryPerformanceCounter(&stQP_Count_End);
				stQP_Result.QuadPart = (LONGLONG)stQP_Count_End.QuadPart - stQP_Count_Begin.QuadPart;

				REAL64 r64ElepseTime = (REAL64)stQP_Result.QuadPart / (REAL64)stQP_Freq_Begin.QuadPart;

				g_pStatistics->AddStat(STAT_TYPE_LINK_BILLING, m_i32Idx, r64ElepseTime);
				
				if(r64ElepseTime > CHECK_TIMEOUT)
				{
					g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_AUTH, L"---[%d] LinkBase Time Over (Sec) : %.6lf ", m_i32Idx, r64ElepseTime);
				}
			}
			else
			{
				g_pLog->WriteLog( WARNING_TYPE_NORMAL, WARNING_TASK, L"[ERROR] Link Output Buffer Full 1" );
			}

		}
		if( OnRunning() )
		{
			bWorking = TRUE;
		}

		if( FALSE == bWorking ) ::WaitForSingleObject( m_hThread, 1 );
	}
}

BOOL CBillingBase::OnRunning()
{
	return FALSE;
}

BOOL CBillingBase::_WorkProcess( BILLING_INPUT * pInput, BILLING_OUTPUT* pOutput, UINT8 ui8GoodsIdx )
{	pInput;	pOutput;	ui8GoodsIdx;
	return FALSE;
}
