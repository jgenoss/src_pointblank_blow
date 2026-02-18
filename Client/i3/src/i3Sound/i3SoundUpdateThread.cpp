#include "stdafx.h"
#include "i3SoundUpdateThread.h"
#include "i3SoundGlobal.h"

#if defined( I3SND_DSOUND)

I3_CLASS_INSTANCE( i3SoundUpdateThread);
//#define PROFILE_PLAYTHREAD

i3SoundUpdateThread::i3SoundUpdateThread(void)
{
	m_bExit = false;
	m_Signal.Create( true, false);
	m_pUpdateHead	= nullptr;
	m_pReadyHead	= nullptr;

	m_pUpdateMutex	= nullptr;
	m_pReadyMutex	= nullptr;
}

i3SoundUpdateThread::~i3SoundUpdateThread(void)
{
	I3_SAFE_RELEASE( m_pUpdateMutex);
	I3_SAFE_RELEASE( m_pReadyMutex);
}

void	i3SoundUpdateThread::MoveToReadyList( i3SoundPlayStateDx * pState)
{
	m_pUpdateMutex->Enter();

	{
		if( pState->getPrev() != nullptr)
		{
			pState->getPrev()->SetNext( pState->getNext());
		}
	}
	m_pUpdateMutex->Leave();

	pState->SetNext( nullptr);

	m_pReadyMutex->Enter();
	{
		i3SoundPlayStateDx * pNext = (i3SoundPlayStateDx *) m_pReadyHead;
		while( (pNext != nullptr) && ( pNext->getNext() != nullptr))
		{
			pNext = pNext->getNext();
		}

		pNext->SetNext( pState);
	}
	m_pReadyMutex->Leave();
}

UINT32 i3SoundUpdateThread::OnRunning( void * pUserData)
{
	I3ASSERT( m_pUpdateHead != nullptr);
	I3ASSERT( m_pReadyHead != nullptr);

	#if defined( PROFILE_PLAYTHREAD)
	static INT64 s_ProfileTime = 0;
	#endif

	while( m_bExit == false)
	{
		m_Signal.WaitFor();
		m_Signal.Reset();

		if( g_pSoundCtx->GetPendingEnable())
			continue;

		i3SoundPlayStateDx * pNext	= m_pUpdateHead->getNext();

		UINT32	nGlobalRequest		= i3SoundPlayStateDx::getGlobalRequest();

		#if defined( PROFILE_PLAYTHREAD)
			INT32	nUpdateCount = 0;
			INT64	localtime = i3Timer::GetProfileTime();
			INT32	nReqCount = 0;
		#endif

		while( pNext != nullptr)
		{
			#if defined( PROFILE_PLAYTHREAD)
				nUpdateCount++;
			#endif

			//Request가 있는경우는 soundcontext에서 강제 wakeup이 일어난 경우,
			if( pNext->isRequestEnable() || (nGlobalRequest != I3SND_RT_REQ_NONE))
			{
				UINT32 Req = pNext->getRequest();

				#if defined( PROFILE_PLAYTHREAD)
					nReqCount++;
				#endif

				//Stop
				if(( Req & I3SND_RT_REQ_STOP) || ( nGlobalRequest & I3SND_RT_REQ_STOP))
				{
					pNext->Stop();

					i3SoundPlayStateDx * pOldNext = pNext->getNext();

					MoveToReadyList( pNext);

					pNext = pOldNext;

					continue;
				}

				if( !pNext->isPlaying() )
				{
					pNext->setRequest( I3SND_RT_REQ_NONE);
					m_pUpdateMutex->Enter();
					pNext = pNext->getNext();
					m_pUpdateMutex->Leave();
					continue;
				}

				//Pause
				if(( Req & I3SND_RT_REQ_PAUSE) || ( nGlobalRequest & I3SND_RT_REQ_PAUSE))
				{
					pNext->CMD_Pause();
				}

				//Resume
				if(( Req & I3SND_RT_REQ_RESUME) || ( nGlobalRequest & I3SND_RT_REQ_RESUME))
				{
					pNext->CMD_Resume();
				}

				//pitch
				if( nGlobalRequest & I3SND_RT_REQ_PITCH)
				{
					pNext->SetPitch( -1);
				}

				if( Req & I3SND_RT_REQ_PITCH)
				{
					pNext->CMD_SetPitch( );
				}

				//volume
				if( nGlobalRequest & I3SND_RT_REQ_VOLUME)
				{
					pNext->SetVolume(-1);
				}

				if( Req & I3SND_RT_REQ_VOLUME)
				{
					pNext->CMD_SetVolume();
				}

				//Position
				if( Req & I3SND_RT_REQ_POSITION)
				{
					pNext->CMD_UpdatePos();

					//bNeedUpdateListener = true;
				}
			}

			if( pNext->isFree())
			{
				pNext->Stop();

				i3SoundPlayStateDx * pOldNext = pNext->getNext();

				MoveToReadyList( pNext);

				pNext = pOldNext;
			}
			else
			{
				if( pNext->Drive() == false)
				{
					pNext->Stop();
				}

				pNext = pNext->getNext();
			}
		}

		i3SoundPlayStateDx::setGlobalRequest( I3SND_RT_REQ_NONE);

		g_pSoundCtx->UpdateListener();

		#if defined( PROFILE_PLAYTHREAD)
		
		INT32 nCnt = 0;
		INT32 nUpCnt = 0;
		INT32 nPlayCnt = 0;

		m_pReadyMutex->Enter();
		pNext = m_pReadyHead->getNext();
		while( pNext != nullptr)
		{
			nCnt++;
			pNext = pNext->getNext();
		}
		m_pReadyMutex->Leave();

		m_pUpdateMutex->Enter();
		pNext = m_pUpdateHead->getNext();
		while( pNext != nullptr)
		{
			nUpCnt++;
			pNext = pNext->getNext();
		}
		m_pUpdateMutex->Leave();

		pNext = g_pSoundCtx->getPlayHead()->getNext();
		while( pNext != nullptr)
		{
			nPlayCnt++;
			pNext = pNext->getNext();
		}

			INT64	curtime = i3Timer::GetProfileTime();
			REAL32	localelapse = (REAL32)( (curtime - localtime) * i3Timer::GetProfileResolution());
			REAL32	elapse = (REAL32)((localtime - s_ProfileTime) * i3Timer::GetProfileResolution());

			I3TRACE("UPDATE - elapse: %04f, localelapse: : %04f, req: %d, RC: %d, UC:%d, PC:%d\n", elapse, localelapse, nReqCount, nCnt, nUpCnt, nPlayCnt);

			s_ProfileTime = curtime;
		#endif
		
	}

	return 0;
}

#endif
