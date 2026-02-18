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

class i3SoundContextFMOD : public i3SoundContext
{
	I3_CLASS_DEFINE( i3SoundContextFMOD);
protected:

	FMOD::System		*	m_pFMODSystem;
	FMOD::Reverb		*	m_pReverb;

	void					InitFMOD(void);
	void					FinishFMOD(void);
	BOOL					CreateFMOD( I3SND_CONTEXTCREATE_PARAM * pInitParam);

	static DWORD			m_hMainThreadID;

	virtual void			_UpdateListenerParam(void);
	virtual void			_UpdateMuteState(void);

public:
	i3SoundContextFMOD(void);
	virtual ~i3SoundContextFMOD(void);

	virtual BOOL				Create( I3SND_CONTEXTCREATE_PARAM * pInitParam);
	virtual void				OnDrive( REAL32 tm);

	virtual char *				ReportPerformance(void);

	virtual void				SetTypeVolume( UINT32 nType, UINT8 nVolume);
	virtual void				setMainVolume( UINT8 nVolume);

	virtual FMOD::System*		getFMODSystem(void)		{	return m_pFMODSystem;}

	virtual	INT32				ChannelGroup_Add( const char * pszChannelName);
	virtual bool				ChannelGroup_SetVolume( INT32 idx, INT32 volume);
	virtual bool				ChannelGroup_Stop( INT32 idx);
	virtual bool				ChannelGroup_Pause( INT32 idx, bool bPause);
	virtual INT32				ChannelGroup_GetNumChannels( INT32 idx);
	virtual bool				ChannelGroup_SetPitch( INT32 idx, REAL32 nPitch);

	virtual void	*			GetChannelGroup( INT32 idx);

	void						CreateReverb(void);

	static FMOD_RESULT F_CALLBACK  _ChannelEndProc( FMOD_CHANNEL *  channel, FMOD_CHANNEL_CALLBACKTYPE  type, int  command, unsigned int  commanddata1, unsigned int  commanddata2);
};

#endif
#endif