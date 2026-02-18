#if !defined( __I3_SND_UPDATETHREAD_H)
#define __I3_SND_UPDATETHREAD_H

#if defined( I3SND_DSOUND)
#include "i3SoundPlayStateDx.h"

class I3_EXPORT_SOUND i3SoundUpdateThread: public i3Thread
{
	I3_CLASS_DEFINE( i3SoundUpdateThread);
protected:
	i3Signal		m_Signal;
	bool			m_bExit;

	i3SoundPlayStateDx *	m_pUpdateHead;
	i3SoundPlayStateDx *	m_pReadyHead;

	i3Mutex			*	m_pUpdateMutex;
	i3Mutex			*	m_pReadyMutex;

public:
	i3SoundUpdateThread(void);
	virtual ~i3SoundUpdateThread(void);

	void	setUpdateStateListHead( i3SoundPlayStateDx * pHead)			{ m_pUpdateHead = pHead; }
	void	setReadyStateListHead( i3SoundPlayStateDx * pHead)			{ m_pReadyHead = pHead; }

	void	setUpdateMutex( i3Mutex * pMutex)							{ I3_REF_CHANGE( m_pUpdateMutex, pMutex);}
	void	setReadyMutex( i3Mutex * pMutex)							{ I3_REF_CHANGE( m_pReadyMutex, pMutex);}

	void	WakeUp(void)												{ m_Signal.Raise(); }

	void	MoveToReadyList( i3SoundPlayStateDx * pState);

	virtual UINT32	OnRunning( void * pUserData);
};

#endif

#endif
