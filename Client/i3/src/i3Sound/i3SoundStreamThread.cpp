#include "stdafx.h"
#include "i3SoundStreamThread.h"

#if defined( I3SND_DSOUND)
#include "i3SoundPlayStateDx.h"

I3_CLASS_INSTANCE( i3SoundStreamThread);

i3SoundStreamThread::i3SoundStreamThread(void)
{
	m_bExit = false;
	m_Signal.Create( true, false);
	m_pState = nullptr;
}

i3SoundStreamThread::~i3SoundStreamThread(void)
{
	m_pState = nullptr;
}

UINT32 i3SoundStreamThread::OnRunning( void * pUserData)
{
	I3ASSERT( m_pState != nullptr); 

	i3SoundPlayStateDx * pState = (i3SoundPlayStateDx *)m_pState;
	I3ASSERT( pState->IsTypeOf( i3SoundPlayState::GetClassMeta()) == true);

	while( m_bExit == false)
	{
		m_Signal.WaitFor(128);

		pState->UpdateStream();

		m_Signal.Reset();
	}

	return 0;
}
#endif //if defined( I3SND_DSOUND)