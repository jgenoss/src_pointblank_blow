#ifndef I3_SOUND_PLAYSTATE_FMOD_H__
#define I3_SOUND_PLAYSTATE_FMOD_H__

#if defined( I3SND_FMOD )
#include "i3Sound.h"
#include "i3SoundPlayState.h"

class I3_EXPORT_SOUND i3SoundPlayStateFMOD :	public i3SoundPlayState
{
	I3_CLASS_DEFINE( i3SoundPlayStateFMOD );

protected:
	FMOD::Channel *		m_pChannel;

	FMOD::Sound *		getFMODSound(void);

public:
	i3SoundPlayStateFMOD(void);
	~i3SoundPlayStateFMOD(void);

public:
	virtual bool Play( i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 GrpIdx = 0);
	virtual bool Replay( i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 GrpIdx = 0);

	virtual void Set3DParams(void);

	virtual void GetPlayPointer(WCHAR32 * pPointer);

	virtual bool Stop( bool bForce = false);
	virtual bool Pause(void);
	virtual bool Mute(bool bMute);
	virtual bool Resume(void);
	virtual bool SetVolume( INT32 vol);
	virtual bool SetPitch( INT32 pitch);
	virtual bool UpdatePos( MATRIX * pMat);
	virtual bool Drive(void);

	virtual bool UpdateMuteState(void);

	bool		 SetChannelGroup( INT32 Grpidx);

public:
};

#endif	// if defined ( I3SND_DSOUND )
#endif	//