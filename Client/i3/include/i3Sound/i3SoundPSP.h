#ifndef I3_SOUND_PSP_H__
#define I3_SOUND_PSP_H__

#include "i3Sound.h"

#if defined( I3G_PSP )

#include <libwave.h>
#include <libsas.h>

class I3_EXPORT_SOUND	i3SoundPSP :	public i3Sound
{
	I3_CLASS_DEFINE( i3SoundPSP );

protected:
	//Volatile Memember
	i3MemoryBuffer	*	m_pBuffer;
	char			*	m_pLockedData;

public:
	i3SoundPSP(void);
	virtual ~i3SoundPSP(void);

public:
	virtual	BOOL		Create( I3SND_SOUND_FORMAT	nFormat, UINT32	nChannels, UINT32	nSampleRate, UINT32	nBitsPerSample, UINT32	nSampleSize);
	virtual BOOL		Create( char * pszPath );

public:
	void				SetSoundBuffer( i3MemoryBuffer	* pBuffer );
	char*				GetSoundBuffer() { return m_pLockedData;}

public:
	virtual char *		Lock(void);
	virtual void		Unlock(void);
};

#endif	//if defined ( I3G_PSP )
#endif	//ifndef I3_SOUND_PSP_H__
