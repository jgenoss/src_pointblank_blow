#include "stdafx.h"

#if defined( I3SND_DSOUND)

#include "i3SoundDx.h"
#include "i3SoundFile.h"
#include "i3SoundGlobal.h"

#include "i3Input/dxerr.h"

I3_CLASS_INSTANCE( i3SoundDx);

i3SoundDx::i3SoundDx(void)
{
	//Runtime Format
	m_nFormat			= I3SND_CMN_FORMAT_WAV;

	m_nbufCount			= 0;
	i3mem::FillZero( m_pbufArray, sizeof( I3SND_BUFINFO) * I3SND_MAX_INSTANCEBUFFER_COUNT);
}

i3SoundDx::~i3SoundDx(void)
{

}

void	i3SoundDx::ClearBuffers(void)
{
	INT32 i;
	for( i = 0; i < m_nbufCount; ++i)
	{
		I3SND_BUFINFO * pBuf = &m_pbufArray[i];

		if( pBuf != nullptr)
		{
			I3_SAFE_RELEASE( pBuf->lpBuffer);
		}
	}

	m_nbufCount			= 0;
	i3mem::FillZero( m_pbufArray, sizeof( I3SND_BUFINFO) * I3SND_MAX_INSTANCEBUFFER_COUNT);
}

bool	i3SoundDx::Create( UINT32 nFormat, UINT32	nChannels, UINT32	nSampleRate, UINT32	nBitsPerSample, UINT32	nSampleSize)
{
	if( g_pDS == nullptr)
	{
		return false;
	}

	i3Sound::Create( nFormat, nChannels, nSampleRate, nBitsPerSample, nSampleSize );

	return true;
}

LPDIRECTSOUNDBUFFER i3SoundDx::CreateSoundBuffer( UINT32 CreateFlag)
{
	I3ASSERT( g_pSoundCtx != nullptr);

	LPDIRECTSOUNDBUFFER pBuffer;
	HRESULT hr;

	// SetFormat
	WAVEFORMATEX wfx;
	i3mem::FillZero( &wfx, sizeof( WAVEFORMATEX ));

	wfx.wFormatTag		= WAVE_FORMAT_PCM;
	wfx.nChannels		= (WORD)m_nChannels;
	wfx.nSamplesPerSec	= m_nSampleRate;
	wfx.wBitsPerSample	= (WORD)m_nBitsPerSample;
	wfx.nBlockAlign		= (WORD)m_nBlockAlign;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

	DSBUFFERDESC				dsbd;
	i3mem::FillZero( &dsbd, sizeof( DSBUFFERDESC));

	dsbd.dwSize			= sizeof( DSBUFFERDESC);
	dsbd.dwFlags		= CreateFlag;

	if(( CreateFlag & DSBCAPS_CTRL3D) == 0)
	{
		dsbd.guid3DAlgorithm	= GUID_NULL;
	}
	else
	{
		dsbd.guid3DAlgorithm	= DS3DALG_NO_VIRTUALIZATION;
	}

	UINT32 nLength = m_nDataSize;
	if(( CreateFlag & DSBCAPS_CTRLFX) != 0)
	{
		nLength		= (INT32)(wfx.nAvgBytesPerSec * DSBSIZE_FX_MIN * 0.001f);
	}
	else
	{
		nLength	   = (INT32)(wfx.nAvgBytesPerSec * DSBSIZE_MIN * 0.001f);
	}

	dsbd.dwBufferBytes = ( nLength > m_nDataSize) ? nLength : m_nDataSize;
	dsbd.lpwfxFormat   = &wfx;

	//////
	hr = g_pDS->CreateSoundBuffer( &dsbd, &pBuffer, NULL);
		
	if( FAILED( hr ))
	{
		DXTRACE_ERR("FAILED CREATE SOUND BUFFER\n", hr);
		I3TRACE("NAME: %s\n", GetName());
		return nullptr;
	}

	//I3TRACE("CreateDupbuffer:__NewBuffer\n");
	FillBuffer( pBuffer);

	return pBuffer;	
}

void i3SoundDx::FillBuffer( LPDIRECTSOUNDBUFFER pBuffer)
{
	HRESULT hr;
	char * pDst;
	UINT32	nLockedSize = 0;

	if( FAILED(hr = pBuffer->Lock( 0, m_nDataSize, (LPVOID*)&pDst, (LPDWORD)&nLockedSize, nullptr, nullptr, 0L )))
	{
		I3PRINTLOG(I3LOG_FATAL,  "pBuffer->Lock(): hr : %X", hr );
	}

	UINT32 nCopySize = nLockedSize;
	if( m_nDataSize < nLockedSize)
	{
		i3mem::FillZero( pDst, nLockedSize);
		nCopySize = m_nDataSize;
	}

	i3mem::Copy( pDst, m_pData, nCopySize);

	pBuffer->Unlock( pDst, nLockedSize, nullptr, 0L );
}

LPDIRECTSOUNDBUFFER i3SoundDx::_CreateDupBuffer(LPDIRECTSOUNDBUFFER lpSrc, UINT32 CreateFlag)
{
	HRESULT hr;
	LPDIRECTSOUNDBUFFER pBuffer = nullptr;

	I3ASSERT( lpSrc != nullptr);

	if(( CreateFlag & DSBCAPS_CTRLFX) != 0)
	{
		pBuffer = CreateSoundBuffer( CreateFlag);
	}
	else
	{
		hr = g_pDS->DuplicateSoundBuffer( lpSrc, &pBuffer);
		if( FAILED( hr))
		{
			I3PRINTLOG(I3LOG_FATAL,  "Failed g_pDS->DuplicateSoundBuffer(): hr : %X", hr );
			return nullptr;
		}
	}

	return pBuffer;
}


I3SND_BUFINFO * i3SoundDx::GetSoundBuffer(UINT32 nCreateFlag)
{
	return GetFreeBuffer( nCreateFlag);
}

//#define DUMP_BUFARRAY_STATE

I3SND_BUFINFO * i3SoundDx::GetFreeBuffer(UINT32 nCreateFlag)
{
	INT32 i;
	INT32 nidx_SameCreateFlag = -1;

	// 기존에 할당되어 있는 Buffer들 중에 Play 중이 아닌 것을 찾는다.

#if defined( DUMP_BUFARRAY_STATE)
	INT32 nUsedCnt = 0, nUnusedCnt = 0, InValidCnt = 0;

	for( i = 0; i < m_nbufCount; ++i)
	{
		I3SND_BUFINFO * pBuffer = &m_pbufArray[i];

		if( (pBuffer != nullptr) && (pBuffer->lpBuffer != nullptr) )
		{
			if( ! pBuffer->isUsed)
			{
				nUnusedCnt++;
			}
			else
			{
				nUsedCnt++;
			}
		}
		else
		{
			InValidCnt++;
		}
	}

	I3TRACE("%s : All: %d, Used: %d, Unused: %d, Invalid: %d\n", GetName(), m_nbufCount,nUsedCnt, nUnusedCnt, InValidCnt);
#endif

	// 기존에 할당되어 있는 Buffer들 중에 Play 중이 아닌 것을 찾는다.
	for( i = 0; i < m_nbufCount; ++i)
	{
		I3SND_BUFINFO * pBuffer = &m_pbufArray[i];

		if( (pBuffer != nullptr) && (pBuffer->lpBuffer != nullptr) && ( pBuffer->nCreateFlag == nCreateFlag))
		{
			nidx_SameCreateFlag = i;

			if( ! pBuffer->isUsed)
			{
				pBuffer->isUsed = true;

				return pBuffer;
			}
		}
		else
			continue;
	}

	if( m_nbufCount >= I3SND_MAX_INSTANCEBUFFER_COUNT)
	{
		I3PRINTLOG(I3LOG_NOTICE, "한가지의 소리가 개체수 제한을 넘어섰습니다. 프로그램팀에 전달 부탁드립니다.: %s", GetName());
		return nullptr;
	}

	// 새로운 Buffer를 생성한다.
	{
		LPDIRECTSOUNDBUFFER lpBuffer = nullptr;
		
		if( nidx_SameCreateFlag != -1)
		{
			I3SND_BUFINFO * pSameBuffer = &m_pbufArray[nidx_SameCreateFlag];

			lpBuffer = _CreateDupBuffer( pSameBuffer->lpBuffer, nCreateFlag);
		}
		else
		{
			lpBuffer = CreateSoundBuffer( nCreateFlag);
		}

		if( lpBuffer != nullptr)
		{
			//I3ASSERT( lpBuffer != nullptr);

			I3SND_BUFINFO * pBuff = &m_pbufArray[m_nbufCount];

			pBuff->isUsed		= true;
			pBuff->lpBuffer		= lpBuffer;
			pBuff->nCreateFlag	= nCreateFlag;
			pBuff->isStopping   = false;

			m_nbufCount++;

			return pBuff;
		}

		return nullptr;
	}
}

bool	i3SoundDx::Create( char * pszPath )
{
	i3SoundFile File;

	if( File.Load( pszPath, this ) == false)
	{
		return false;
	}

	return true;
}

#endif // #if defined( I3SND_DSOUND)