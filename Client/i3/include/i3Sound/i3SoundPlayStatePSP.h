#ifndef I3_SOUND_PLAYSTATE_PSP_H__
#define I3_SOUND_PLAYSTATE_PSP_H__

#if defined( I3G_PSP)
#include "i3Sound.h"
#include "i3SoundPlayState.h"

class i3SoundPlayStatePSP :	public i3SoundPlayState
{
	I3_CLASS_DEFINE( i3SoundPlayStatePSP );

public:
	i3SoundPlayStatePSP(void);
	~i3SoundPlayStatePSP(void);

protected:
	//Volatile Member
	char		*	m_pSoundBuffer;

public:
	virtual BOOL play(i3Sound * pSound);
	virtual void stop();
	virtual void pause();
	virtual void resume();
	virtual void Update(void * pData);
};

class I3_EXPORT_SOUND i3SoundSasThread:	public i3Thread
{
	I3_CLASS_DEFINE( i3SoundSasThread );

protected:
	static INT16 * m_pcmBuf[I3SND_SAMPLE_SIDE];

public:
	i3SoundSasThread(void);
	~i3SoundSasThread(void);
	
public:
	virtual BOOL	Create( char *pszName , void * pUserData );
	virtual UINT32	OnRunning( void * pUserData );
};

#endif	// if defined ( I3G_PSP )
#endif	// ifndef I3_SOUND_PLAYSTATE_PSP_H__
