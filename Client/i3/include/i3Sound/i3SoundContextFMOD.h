#if !defined( __I3_SOUND_CONTEXT_FMOD_H)
#define __I3_SOUND_CONTEXT_FMOD_H

#include "i3SoundConfig.h"
#include "i3SoundContext.h"

#if defined( I3SND_FMOD)
#include "fmod.hpp"

enum I3SND_CHANNEL_GROUP
{
	I3SND_CHANNEL_UNKNOWN = -1,
	I3SND_CHANNEL_MASTER = 0,
	I3SND_CHANNEL_UI,
	I3SND_CHANNEL_BATTLE,

	I3SND_CHANNEL_MAX,
};

class I3_EXPORT_SOUND i3SoundContextFMOD : public i3SoundContext
{
//	I3_CLASS_DEFINE( i3SoundContextFMOD);
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3SoundContextFMOD, i3SoundContext, i3::cms_concrete_fmod::value);
protected:

	FMOD::System		*	m_pFMODSystem = nullptr;
	FMOD::Reverb		*	m_pReverb = nullptr;

	void					InitFMOD(void);
	void					FinishFMOD(void);
	bool					CreateFMOD( I3SND_CONTEXTCREATE_PARAM * pInitParam);

	static DWORD			m_hMainThreadID;

	virtual void			_UpdateListenerParam(void);
	virtual void			_UpdateMuteState(void);

public:
	virtual bool				Create( I3SND_CONTEXTCREATE_PARAM * pInitParam) override;
	virtual void				OnDrive( REAL32 tm) override;

	virtual i3::string			ReportPerformance(void) override;

	virtual void				SetTypeVolume( UINT32 nType, UINT8 nVolume) override;
	virtual void				setMainVolume( UINT8 nVolume) override;

	FMOD::System*				getFMODSystem(void)		{	return m_pFMODSystem;}

	virtual	INT32				ChannelGroup_Add( const char * pszChannelName) override;
	virtual bool				ChannelGroup_SetVolume( INT32 idx, INT32 volume) override;
	virtual bool				ChannelGroup_Stop( INT32 idx) override;
	virtual bool				ChannelGroup_Pause( INT32 idx, bool bPause) override;
	virtual INT32				ChannelGroup_GetNumChannels( INT32 idx) override;
	virtual bool				ChannelGroup_SetPitch( INT32 idx, REAL32 nPitch) override;

	virtual void	*			GetChannelGroup( INT32 idx) override;

	void						CreateReverb(void);

	static FMOD_RESULT F_CALLBACK  _ChannelEndProc( FMOD_CHANNEL *  channel, FMOD_CHANNEL_CALLBACKTYPE  type, int  command, unsigned int  commanddata1, unsigned int  commanddata2);
};

#endif
#endif