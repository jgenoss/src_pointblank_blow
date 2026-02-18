#include "i3FrameworkPCH.h"
#include "i3VirtualTextureIO_Thread.h"
#include "i3VirtualTextureIO.h"


I3_CLASS_INSTANCE( i3VirtualTextureIO_Thread);


i3VirtualTextureIO_Thread::~i3VirtualTextureIO_Thread(void)
{
	m_bExitRequest = true;

	if( m_pSignal_Queue != nullptr)
		m_pSignal_Queue->Raise();	// 이 경우 종료된다.

	m_State = THREAD_STOPPED;
	
	while(m_bExitResult == false)
	{
		::Sleep(10);			// 

		if( m_pSignal_Queue != nullptr)
			m_pSignal_Queue->Raise();	// 이 경우 종료된다.
	}

}

UINT32 i3VirtualTextureIO_Thread::OnRunning( void * pData)
{
	I3ASSERT( m_pSignal_Complete != nullptr);
	I3ASSERT( m_pSignal_Queue != nullptr);

	m_bExitResult = false;

	while( m_pIOManager != nullptr)
	{
		m_pSignal_Queue->WaitFor();
		
		if (m_bExitRequest)
			break;

		while( m_pIOManager->_IsEmpty() == false)
		{
			if( m_pIOManager->_Kick())
			{
				m_pIOManager->_WaitForCompletion();
			}
		}

		m_pSignal_Queue->Reset();
	}
	
	m_bExitResult = true;
	return 0;
}
