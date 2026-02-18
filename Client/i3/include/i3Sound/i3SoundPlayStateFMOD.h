#ifndef I3_SOUND_PLAYSTATE_FMOD_H__
#define I3_SOUND_PLAYSTATE_FMOD_H__

#if defined( I3SND_FMOD )
#include "i3Sound.h"
#include "i3SoundPlayState.h"

class I3_EXPORT_SOUND i3SoundPlayStateFMOD :	public i3SoundPlayState
{
//	I3_CLASS_DEFINE( i3SoundPlayStateFMOD );
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3SoundPlayStateFMOD, i3SoundPlayState, i3::cms_concrete_fmod::value);

protected:
	FMOD::Channel *		m_pChannel = nullptr;

	FMOD::Sound *		getFMODSound(void);

public:
	virtual bool Play( i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 GrpIdx = 0, FMOD_CHANNEL_CALLBACK callback = nullptr) override;
	virtual bool Replay( i3SoundPlayInfo * pInfo, MATRIX * pMat, INT32 GrpIdx = 0, FMOD_CHANNEL_CALLBACK callback = nullptr) override;

	virtual void Set3DParams(void) override;

	virtual void GetPlayPointer(WCHAR32 * pPointer) override {}

	virtual bool Stop( bool bForce = false) override;
	virtual bool Pause(void) override;
	virtual bool Mute(bool bMute) override;
	virtual bool Resume(void) override;
	virtual bool SetVolume( INT32 vol) override;
	virtual bool SetPitch( INT32 pitch) override;
	virtual bool UpdatePos( MATRIX * pMat) override;
	virtual bool Drive(void) override;

	virtual bool UpdateMuteState(void) override;

	bool		 SetChannelGroup( INT32 Grpidx);

	void		PreLoadSound( void)			{ FMOD::Sound * pSnd = getFMODSound(); I3ASSERT( pSnd != nullptr); }

public:
};

#endif	// if defined ( I3SND_DSOUND )
#endif	//