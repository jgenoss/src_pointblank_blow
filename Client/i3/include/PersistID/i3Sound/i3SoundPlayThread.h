#if !defined( __I3_SND_PLAYTHREAD_H)
#define __I3_SND_PLAYTHREAD_H

#if defined( I3SND_DSOUND)
#include "i3SoundPlayStateDx.h"

class I3_EXPORT_SOUND i3SoundPlayThread : public i3Thread
{
	I3_CLASS_DEFINE( i3SoundPlayThread);
protected:
	i3Signal		m_Signal;
	bool			m_bExit;

	i3SoundPlayStateDx *	m_pReqPlayHead;
	i3SoundPlayStateDx *	m_pUpdateHead;

	i3Mutex			*	m_pPlayMutex;
	i3Mutex			*	m_pUpdateMutex;

public:
	i3SoundPlayThread(void);
	virtual ~i3SoundPlayThread(void);

	void	setPlayStateListHead( i3SoundPlayStateDx * pHead)			{ m_pReqPlayHead = pHead; }
	void	setUpdateStateListHead( i3SoundPlayStateDx * pHead)		{ m_pUpdateHead = pHead; }

	void	setPlayMutex( i3Mutex * pMutex)							{ I3_REF_CHANGE( m_pPlayMutex, pMutex);}
	void	setUpdateMutex( i3Mutex * pMutex)						{ I3_REF_CHANGE( m_pUpdateMutex, pMutex);}

	void	WakeUp(void)								{ m_Signal.Raise(); }

	virtual UINT32	OnRunning( void * pUserData);
};
#endif

#endif
