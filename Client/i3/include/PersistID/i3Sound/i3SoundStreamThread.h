#if !defined( I3SOUND_STREAM_THREAD_H__)
#define I3SOUND_STREAM_THREAD_H__

#if defined( I3SND_DSOUND)
class I3_EXPORT_SOUND i3SoundStreamThread : public i3Thread
{
	I3_CLASS_DEFINE( i3SoundStreamThread);
protected:
	i3Signal			m_Signal;
	void			*	m_pState;

	bool				m_bExit;

public:
	i3SoundStreamThread(void);
	virtual ~i3SoundStreamThread(void);

	void	setState( void * pState)			{ m_pState = pState; }

	void	WakeUp(void)								{ m_Signal.Raise(); }

	virtual UINT32	OnRunning( void * pUserData);
};
#endif // if defined( I3SND_DSOUND)

#endif