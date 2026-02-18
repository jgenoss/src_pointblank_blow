#include "stdafx.h"
#include "i3SoundFile.h"
#include "i3SoundUtil.h"
#include "i3SoundFileWav.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

UINT32	  i3SoundFile::LoadWav( i3Stream * pStream, i3Sound * pSound )
{
	UINT32	Result, Rc;
	I3ASSERT( pStream != nullptr );
	I3ASSERT( pSound != nullptr );

	i3::pack::SOUND_INFO_HEADER Info;

	Result = I3SNDUtil_LoadFileInfo( pStream, &Info);
	I3_CHKIO( Result);

	///////////////////////////////////////////////////////////////////////////////////////
	INT32	nRuntimeDataLength = I3SNDUtil_CalculateBufferSize( I3SND_CMN_FORMAT_WAV, Info.m_nFormat, Info.m_nDataSize );

	if ( pSound->Create( I3SND_CMN_FORMAT_WAV, Info.m_nChannels, Info.m_nSampleRate,
			Info.m_nBitsPerSample, nRuntimeDataLength ) == false)
	{
		I3TRACE("FAILED CREATE SOUND\n");
		return STREAM_ERR;
	}

	char * pData = (char*)i3MemAlloc( Info.m_nDataSize);
	i3mem::FillZero( pData, Info.m_nDataSize);
	{
		Rc = pStream->Read( pData, Info.m_nDataSize);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( pSound->GetFormat() == I3SND_CMN_FORMAT_WAV )
	{
		pSound->SetData( pData);
	}
	else
	{
		char * pRuntimeData = (char *)i3MemAlloc( nRuntimeDataLength);
		i3mem::FillZero( pRuntimeData, nRuntimeDataLength);

		pSound->MakeRuntimeForm( pRuntimeData, pData, Info.m_nDataSize);

		pSound->SetData( pRuntimeData);

		I3MEM_SAFE_FREE_POINTER( pData);
	}
 
	return Result;
}

//ÀÛ¾÷Áß..
UINT32	  i3SoundFile::SaveWav( i3Stream * pStream, i3Sound * pSound )
{
	UINT32 Result, Rc;
	//Save Header
	i3::pack::SOUND_WAV_HEADER wavHeader;
	
	wavHeader.nWavFormat	= 1;//WAVE_FORMAT_PCM/UNCOMPRESSED
	wavHeader.nChunkData	= 16;

	wavHeader.nChannels		= (INT16)pSound->GetChannels();
	wavHeader.nBitsPerSample= (INT16)pSound->GetBitsPerSample();
	wavHeader.nBlockAlign	= (INT16)wavHeader.nChannels * wavHeader.nBitsPerSample>>3;
	wavHeader.nSampleRate	= pSound->GetSampleRate();
	wavHeader.nBytesPerSec	= wavHeader.nSampleRate * wavHeader.nBlockAlign;
	wavHeader.nDataLength	= I3SNDUtil_CalculateBufferSize( pSound->GetFormat(), I3SND_CMN_FORMAT_WAV, pSound->GetDataLength() );
	wavHeader.length		= wavHeader.nDataLength + 44 - 8;

	Result = pStream->Write( &wavHeader, sizeof( wavHeader ));
	I3_CHKIO( Result);

    char * pSrcBuf = nullptr;

	//save Data
	if( pSound->GetFormat() == I3SND_CMN_FORMAT_WAV )
	{
		pSrcBuf = (char*)pSound->getData();
		Rc = pStream->Write( pSrcBuf, wavHeader.nDataLength );
		I3_CHKIO( Rc);
		Result += Rc;
	}
	else
	{
		if( pSound->GetPersistFormat() == I3SND_CMN_FORMAT_WAV )
		{
			pSrcBuf = (char*)pSound->getData();

			Rc = pStream->Write(pSrcBuf, wavHeader.nDataLength );
			I3_CHKIO( Rc);
			Result += Rc;
		}
		else
		{
			pSrcBuf			= (char*)pSound->getData();
			char *  pBuf	= (char*)i3MemAlloc( wavHeader.nDataLength );

			I3SNDUtil_ConvertSound( pSound->GetFormat(), pSound->GetDataLength(), pSrcBuf,
				I3SND_CMN_FORMAT_WAV, wavHeader.nDataLength, pBuf );

			Rc = pStream->Write( pBuf, wavHeader.nDataLength);
			I3_CHKIO( Rc);
			Result += Rc;

			i3MemFree( pBuf );
		}
	}

	return Result;
}

