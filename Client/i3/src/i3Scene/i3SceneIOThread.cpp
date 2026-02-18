#include "i3SceneDef.h"
#include "i3SceneIOThread.h"
#include "i3SceneFile.h"
#include "i3TextureBindAttr.h"
#include "i3Scene.h"
#include "i3Base/string/compare/generic_is_equal.h"

I3_CLASS_INSTANCE( i3SceneIOThread);

i3SceneIOThread::~i3SceneIOThread(void)
{
	if( m_pQueue != nullptr)
	{
		m_bExit = true;
		m_pQueue->ForceRaise();	// 이 경우 종료된다.
		m_State = THREAD_STOPPED;
		
		while(m_bExitResult == false)
		{
			::Sleep(10);
		}
	}

	I3_SAFE_RELEASE( m_pQueue);
}

void i3SceneIOThread::SetQueue( i3SceneIOQueue * pQueue)
{
	I3_REF_CHANGE( m_pQueue, pQueue);
}

UINT32 i3SceneIOThread::OnRunning( void * pData)
{
	m_bExitResult = false;

	while( m_bExit == false)
	{
		INT32 idx = m_pQueue->WaitIO();
		
		if (m_bExit)
			break;

		if( idx == -1 )			// Error 상황 or 종료 상황
		{
			m_bExitResult = true;
			return 0xFFFFFFFF;
		}

		IOTICKET * pTicket = m_pQueue->getTicket( idx);

		{
			i3SceneFile file;
			char szFull[ MAX_PATH];

			file.setBackgroundLoadEnable( !pTicket->m_bLoadedDX );

			if( i3System::isAbsolutePath( pTicket->m_szPath))
			{
				i3::safe_string_copy( szFull, pTicket->m_szPath, _countof( szFull));
			}
			else
			{
				if (i3::generic_is_equal(i3ResourceFile::GetWorkingFolder(), "") )
					i3::snprintf( szFull, _countof( szFull), "%s", pTicket->m_szPath);
				else
					i3::snprintf( szFull, _countof( szFull), "%s/%s", i3ResourceFile::GetWorkingFolder(), pTicket->m_szPath);
			}

			if( file.Load( szFull, pTicket->m_bCommonRes) == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "no exist file %s", szFull);
				m_bExitResult = true;
				m_pQueue->EndLoad( idx, nullptr);				// getSceneGraphInfo()의 값이 nullptr인 경우, Error로 간주한다.
				return STREAM_ERR;
			}

			// 성공
			m_pQueue->EndLoad( idx, file.getSceneGraphInfo());		// getSceneGraphInfo()의 값이 nullptr인 경우, Error로 간주한다.
		}

	}
	
	m_bExitResult = true;
	return 0;
}