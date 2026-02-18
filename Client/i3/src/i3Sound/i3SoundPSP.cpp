#include "stdafx.h"

#if defined( I3G_PSP)

#include "i3SoundPSP.h"
#include "i3SoundFile.h"

I3_CONCRETE_CLASS_INSTANCE( i3SoundPSP, i3Sound);

i3SoundPSP::i3SoundPSP(void)
{
	m_pBuffer		= NULL;
	m_pLockedData	= NULL;
}

i3SoundPSP::~i3SoundPSP(void)
{
	I3_SAFE_RELEASE( m_pBuffer );
	m_pLockedData	= NULL;
}

BOOL	i3SoundPSP::Create( I3SND_SOUND_FORMAT	nFormat, UINT32	nChannels, UINT32	nSampleRate, UINT32	nBitsPerSample, UINT32	nSampleSize)
{
	i3Sound::Create( nFormat, nChannels, nSampleRate, nBitsPerSample, nSampleSize );

	return TRUE;
}

BOOL	i3SoundPSP::Create( char * pszPath )
{
	i3SoundFile File;

	if( File.Load( pszPath, this ) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

void	i3SoundPSP::SetSoundBuffer( i3MemoryBuffer * pBuffer )
{
	if( m_pBuffer != NULL )
	{
		I3TRACE("DSBUFFER is already exist\n");
		I3_SAFE_RELEASE( m_pBuffer );
	}

	m_pBuffer		= pBuffer;
	m_nDataSize		= m_pBuffer->GetSize();
	Lock();
}

char*	i3SoundPSP::Lock(void)
{
	if( m_pBuffer == NULL )
	{
		I3TRACE("Buffer is NULL\n");
		return NULL;
	}

	m_pLockedData = (char*)m_pBuffer->Lock();

	return m_pLockedData;
}

void	i3SoundPSP::Unlock(void)
{
	if( m_pBuffer == NULL )
	{
		I3TRACE("Buffer is NULL\n");
		return ;
	}

	m_pBuffer->Unlock();
}

#endif //defined( I3G_PSP)
