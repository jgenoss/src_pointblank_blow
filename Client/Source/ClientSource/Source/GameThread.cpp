#include "pch.h"
#include "GameThread.h"

//I3_ABSTRACT_CLASS_INSTANCE( CGameThread, i3Thread);
I3_CLASS_INSTANCE( CGameThread);

CGameThread::CGameThread() : m_bRun(true), m_pSignal(nullptr), m_bExitResult(true)
	, m_SignalTime(INFINITE)
{	
}

CGameThread::~CGameThread()
{
	Destroy();
}

/*virtual*/ bool CGameThread::Create( char * pszName, UINT32 Attr /*= 0*/, UINT32 StackSize /*= 4096*/, void * pUserData /*= nullptr*/, INT32 priority /*= 3*/)
{
	m_pSignal = i3Signal::new_object();
	m_pSignal->Create();
	m_pSignal->Reset();

	return i3Thread::Create( pszName, Attr, StackSize, pUserData, priority);
}

void CGameThread::Destroy( void)
{
	m_bRun = false;
	if( m_pSignal != nullptr)
		m_pSignal->Raise();

	while( m_bExitResult == false)
	{
		i3Framework::UpdateDXForThread();
		::Sleep(10);
	}

	m_State = THREAD_STOPPED;

	I3_SAFE_RELEASE( m_pSignal);
}

/*virtual*/ UINT32 CGameThread::OnRunning( void * pData)
{
	m_bExitResult = false;
	while( m_bRun)
	{
		m_pSignal->WaitFor( m_SignalTime);

		if( m_bRun)
		{
			Run( pData);
		}

		m_pSignal->Reset();
	}

	m_bExitResult = true;
	return 0;
}

void CGameThread::AfterRunning( void * pUserData)
{
	i3Thread::AfterRunning( pUserData);
}

void CGameThread::Wakeup( void)
{
	I3ASSERT( m_pSignal != nullptr);
	m_pSignal->Raise();
}

void CGameThread::Pause( void)
{
	I3ASSERT( m_pSignal != nullptr);
	m_pSignal->Reset();

	I3ASSERT( this->Suspend() );
}

void CGameThread::Stop( void)
{
	m_bRun = false;
}
