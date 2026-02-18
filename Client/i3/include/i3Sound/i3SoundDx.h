////////////////////////////////////////////////////////////////////////////////////////
//	FileName	: i3SoundDx.h
//
//	Description	: i3SoundDx Header File
//
//	2005/11/10 ~	raja
////////////////////////////////////////////////////////////////////////////////////////
#ifndef I3_SOUND_DX_H__
#define I3_SOUND_DX_H__

#include "i3Sound.h"

#if defined( I3SND_DSOUND)

#define	I3SND_MAX_INSTANCEBUFFER_COUNT	128

class I3_EXPORT_SOUND i3SoundDx :	public i3Sound
{
//	I3_CLASS_DEFINE( i3SoundDx );
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3SoundDx, i3Sound, i3::cms_concrete_dsound::value);

protected:
	INT32					m_nbufCount;
	I3SND_BUFINFO			m_pbufArray[I3SND_MAX_INSTANCEBUFFER_COUNT];

protected:
	LPDIRECTSOUNDBUFFER		_CreateDupBuffer( LPDIRECTSOUNDBUFFER lpSrc, UINT32 nCreateFlag);

public:
	i3SoundDx(void);
	virtual ~i3SoundDx(void);

	virtual	bool				Create( UINT32	nFormat, UINT32	nChannels, UINT32	nSampleRate, UINT32	nBitsPerSample, UINT32	nSampleSize);
	virtual bool				Create( char * pszPath);
	
	virtual I3SND_BUFINFO	*	GetFreeBuffer( UINT32 nCreateFlag);
	I3SND_BUFINFO	* 			GetSoundBuffer( UINT32 nCreateFlag);

	LPDIRECTSOUNDBUFFER			CreateSoundBuffer( UINT32 nCreateFlag);

	virtual 	void			FillBuffer( LPDIRECTSOUNDBUFFER pBuffer);
	virtual		void			ClearBuffers(void);
};

#define i3SoundPlatform			i3SoundDx

#endif //#if defined( I3SND_DSOUND)

#endif //I3_SOUND_DX_H__