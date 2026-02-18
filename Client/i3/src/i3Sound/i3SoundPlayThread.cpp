#include "stdafx.h"
#include "i3SoundPlayThread.h"
#include "i3SoundGlobal.h"

#if defined( I3SND_DSOUND)

#include "i3SoundPlayStateDx.h"

I3_CLASS_INSTANCE( i3SoundPlayThread);
//#define PROFILE_PLAYTHREAD

i3SoundPlayThread::i3SoundPlayThread(void)
{
	m_bExit = false;
	m_Signal.Create( true, false);
	m_pReqPlayHead	= nullptr;
	m_pUpdateHead	= nullptr;

	m_pPlayMutex	= nullptr;
	m_pUpdateMutex	= nullptr;
}

i3SoundPlayThread::~i3SoundPlayThread(void)
{
	I3_SAFE_RELEASE( m_pPlayMutex);
	I3_SAFE_RELEASE( m_pUpdateMutex);
}

UINT32 i3SoundPlayThread::OnRunning( void * pUserData)
{
	I3ASSERT( m_pReqPlayHead != nullptr);
	I3ASSERT( m_pUpdateHead != nullptr);

	//bool	bNeedUpdateListener = false;

	#if defined( PROFILE_PLAYTHREAD)
		INT64	s_Elapse = 0;
	#endif

	while( m_bExit == false)
	{
		m_Signal.WaitFor();
		m_Signal.Reset();

		if( g_pSoundCtx->GetPendingEnable())
		{
			continue;
		}

		#if defined( PROFILE_PLAYTHREAD)
			INT64	localtime = i3Timer::GetProfileTime();
			INT32	nReqCount = 0;

			INT64	elapse = localtime - s_Elapse;

			bool bPlayed = false;
		#endif

		i3SoundPlayStateDx * pNext	= m_pReqPlayHead->getNext();
		i3SoundPlayStateDx * pStart	= pNext;
		UINT32	nGlobalRequest		= i3SoundPlayStateDx::getGlobalRequest();
		

		while( pNext != nullptr)
		{
			if( nGlobalRequest & I3SND_RT_REQ_STOP)
			{
				pNext->Stop();
			}
			else
			{
				if( pNext->getRequest() & I3SND_RT_REQ_PLAY)
				{
					if( !pNext->CMD_Play())
					{
						I3TRACE("Failed Play Sound: %s\n", pNext->getPlayInfo()->getSound()->GetName());
						pNext->Stop();
					}
				}
				else if( pNext->getRequest() & I3SND_RT_REQ_REPLAY)
				{
					if( !pNext->CMD_Replay())
					{
						I3TRACE("Failed RePlay Sound: %s\n", pNext->getPlayInfo()->getSound()->GetName());
						pNext->Stop();
					}
				}
			}

			#if defined( PROFILE_PLAYTHREAD)
			nReqCount++;
			#endif

			
			{
				i3SoundPlayStateDx * playnext = nullptr;

				m_pPlayMutex->Enter();
				{
					playnext = pNext->getNext();
				}
				m_pPlayMutex->Leave();

				if( playnext == nullptr)
				{
					m_pUpdateMutex->Enter();
					{
						i3SoundPlayStateDx * pUpdate = m_pUpdateHead;
						while( pUpdate != nullptr && pUpdate->getNext() != nullptr)
						{
							pUpdate = pUpdate->getNext();
						}
						pUpdate->SetNext( pStart);
					}
					m_pUpdateMutex->Leave();

					m_pPlayMutex->Enter();
					m_pReqPlayHead->SetNext( nullptr);
					m_pPlayMutex->Leave();

					pNext = nullptr;
				}
				else
				{
					pNext = playnext;
				}
			}

			#if defined( PROFILE_PLAYTHREAD)
			bPlayed = true;
			#endif
		}

		#if defined( PROFILE_PLAYTHREAD)
			INT64	curtime = i3Timer::GetProfileTime();
			REAL32	localelapse = (REAL32)( (curtime - localtime) * i3Timer::GetProfileResolution());

			s_Elapse = curtime;

			if( bPlayed)
			{
				I3TRACE("elapse: %f - localelapse: %04f, req: %d\n", (REAL32)(elapse  * i3Timer::GetProfileResolution()), localelapse, nReqCount);
			}
		#endif
	}

	return 0;
}

#endif