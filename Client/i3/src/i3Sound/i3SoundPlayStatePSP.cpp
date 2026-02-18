#include "stdafx.h"

#ifdef I3G_PSP

#include "i3SoundPSP.h"
#include "i3SoundPlayStatePSP.h"

I3_CONCRETE_CLASS_INSTANCE( i3SoundPlayStatePSP, i3SoundPlayState );

#define ALLADSR (SCE_SAS_ATTACK_VALID|SCE_SAS_DECAY_VALID |SCE_SAS_SUSTAIN_VALID|SCE_SAS_RELEASE_VALID)

i3SoundPlayStatePSP::i3SoundPlayStatePSP(void)
{
	m_pSoundBuffer = NULL;
}	

i3SoundPlayStatePSP::~i3SoundPlayStatePSP(void)
{
	m_pSoundBuffer = NULL;
}

BOOL	i3SoundPlayStatePSP::play(i3Sound * pSound)
{
	INT32	nResult;
	BOOL	bLoopPlay;
	if( pSound == NULL )
	{
		//
		I3TRACE("Not a Valid Sound Data");	
		return FALSE;
	} 

	m_pSoundBuffer = (char*)((i3SoundPSP*)pSound)->GetSoundBuffer();

	if( m_pSoundBuffer == NULL )
	{
		I3TRACE("No Sound Data\n");
		return FALSE;
	}

	//Get Loop Mode
	if((m_nState & I3SND_SOUND_MASK_LOOPPLAY) == I3SND_LOOPMODE_LOOPING)	bLoopPlay = TRUE;

	//SetVoice
	nResult = sceSasSetVoice( m_nIndex, (const void*)m_pSoundBuffer, pSound->GetDataLength(), bLoopPlay);
	if( nResult != SCE_OK )
	{
		I3TRACE("sceSasSetVoice() Returned ErrorCode : %X\n", nResult );
		return FALSE;
	}

	//SetADSR
	if( bLoopPlay )
	{
		nResult = sceSasSetADSR( m_nIndex, ALLADSR, 0x40000000, 0, 0, 0x20000000 );
		if( nResult != SCE_OK )	I3TRACE("sceSasSetADSR returned Error: %X\n", nResult );
	}
	else
	{
		nResult = sceSasSetADSR( m_nIndex, ALLADSR, 0x40000000, 100, 100, 0x20000000 );
		if( nResult != SCE_OK )	I3TRACE("sceSasSetADSR returned Error: %X\n", nResult );
	}

	sceSasSetPitch( 0x1000 );

	//SetVolume
	sceSasSetVolume( m_nIndex, SCE_SAS_VOLUME_MAX, SCE_SAS_VOLUME_MAX, 0, 0 );

	//KeyOn
	nResult = sceSasSetKeyOn( m_nIndex );
	if( nResult != SCE_OK )
	{
		I3TRACE("sceSasSetKeyOn() Returned Error Code: %X\n");
		return FALSE;
	}

	AddState( I3SND_PLAYSTATE_PLAY );
	SubState( I3SND_PLAYSTATE_READY);
	return TRUE;
}

void i3SoundPlayStatePSP::stop()
{
	I3ASSERT( g_pDS != NULL );
	if( m_lpDSBuffer == NULL )	return ;

	HRESULT hr;

	if( FAILED( hr = m_lpDSBuffer->Stop()))
	{
		I3TRACE("FAILED m_lpDSBuffer->Stop(): %X\n", hr);
		return ;
	}

	SubState( I3SND_PLAYSTATE_PLAY);
	AddState( I3SND_PLAYSTATE_READY);
}

void i3SoundPlayStatePSP::pause()
{

}

void i3SoundPlayStatePSP::resume()
{

}

void i3SoundPlayStatePSP::Update(void * pData)
{

}

/////////////////////////////////////////////////////////////////////
i3SoundSasThread::i3SoundSasThread()
{
	INT32	MemIndex;
	for( INT32	i = 0; i < I3SND_SAMPLE_SIDE ; ++i)
	{
		MemIndex = i3MemAlloc( sizeof( INT16 ) * I3SND_SAMPLE_SIZE * 2 );
		m_pcmBuf[i]	= i3MemLock( MemIndex );
	}
}

i3SoundSasThread::~i3SoundSasThread()
{
	for( INT32	i = 0; i < I3SND_SAMPLE_SIDE ; ++i)
	{
		i3MemFree( m_pcmBuf[i] );
	}
}

BOOL i3SoundSasThread::Create( char *pszName , void * pUserData )
{
	return i3Thread::Create( pszName, 0, 4096, pUserData, PRIORITY_NORMAL);
}

void i3SoundSasThread::OnRunning( void * pUserData )
{
	volatile INT32	iSide = 0;
	INT32	Rv =0;
	BOOL	bLoop = TRUE;

#ifdef I3_DEBUG
	I3TRACE("---PSP__SOUND___THREAD___STARTING___\n");
#endif

	while( bLoop )
	{
		Rv = sceSasCore( m_pcmBuf[iSide] );
		#ifdef I3_DEBUG
			if( rv < 0 ) I3TRACE("Error sceSasCore i3SoundThreadPSP %p \n", Rv );
		#endif

		Rv = sceWaveAudioWriteBlocking( I3SND_WAVOUT_CHANNEL_EFFECT, I3SND_WAV_VOLUME_MAX, I3SND_WAV_VOLUME_MAX);
	}

#ifdef I3_DEBUG
	I3TRACE("---PSP__SOUND___THREAD___Stop___\n");
#endif
}
#endif