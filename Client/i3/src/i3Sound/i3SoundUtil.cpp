#include "stdafx.h"
#include "i3SoundUtil.h"
#include "i3Base/string/compare/generic_is_niequal.h"

static INT32 I3SND_CONV_BUFFER_SIZE	= 128 * 28;

static REAL32 MultiCoef[5][2] = 
{
	{ 0.0f, 0.0f },
	{  -60.0f / 64.0f, 0.0f },
	{ -115.0f / 64.0f, 52.0f / 64.0f },
	{  -98.0f / 64.0f, 55.0f / 64.0f },
	{ -122.0f / 64.0f, 60.0f / 64.0f } 
};

static REAL32	DeMultiCoef[5][2] = 
{
	{ 0.0f, 0.0f },
	{  60.0f / 64.0f, 0.0f },
	{ 115.0f / 64.0f, -52.0f / 64.0f },
	{  98.0f / 64.0f, -55.0f / 64.0f },
	{ 122.0f / 64.0f, -60.0f / 64.0f } 
};

static INT32 nDecodeFactor = 0;
static INT32 nShiftFactor = 0;

INT32 I3SNDUtil_CalculateBufferSize( UINT32 srcFormat, UINT32 dstFormat, INT32	nSrcLength)
{
	I3ASSERT( nSrcLength > 0 );

	INT32	nSampleSize;
	INT32	nSize;

	switch( srcFormat )
	{
		case I3SND_CMN_FORMAT_WAV:
		{
			switch( dstFormat )
			{
				case I3SND_CMN_FORMAT_I3SND:
				case I3SND_CMN_FORMAT_VAG:
				{
					nSampleSize = nSrcLength>>1;
					nSize = nSampleSize/28;
					if( nSampleSize%28 )	nSize++;
					nSize = 16 * ( nSize + 2 );
					return nSize;
				}
				break;

				case I3SND_CMN_FORMAT_WAV:
				{
					return nSrcLength;
				}
				break;

				default:
					break;
			}
		}
		break;

		case I3SND_CMN_FORMAT_I3SND:
		case I3SND_CMN_FORMAT_VAG:
		{
			switch( dstFormat)
			{
				case I3SND_CMN_FORMAT_WAV:
				{
					nSampleSize = nSrcLength>>4;
					if( nSrcLength%16 )	nSampleSize++;

					nSampleSize -= 2;
					nSampleSize *= 28 * 2;
					return nSampleSize;
				}break;

				case I3SND_CMN_FORMAT_I3SND:
				case I3SND_CMN_FORMAT_VAG:
				{
					return nSrcLength;
				}
				break;

				default:
					break;
			}
		}
		break;

		default:	break;
	}

	return 0;
}

bool I3SNDUtil_ConvertSound( UINT32 srcFormat, INT32	nSrcLength, char * pSrc,
									 UINT32 dstFormat, INT32	nDstLength, char * pDst)
{
	switch( srcFormat )
	{
		case I3SND_CMN_FORMAT_WAV:
		{
			switch( dstFormat )
			{
				case I3SND_CMN_FORMAT_I3SND:// i3snd_sound_format_vag와 같은 작업을 합니다.
				case I3SND_CMN_FORMAT_VAG:
				{
					return I3SNDUtil_Convert_WAV2VAG( pSrc, nSrcLength, pDst );
				}
				break;

				default:
					break;
			}
		}
		break;

		case I3SND_CMN_FORMAT_I3SND:// i3snd_sound_format_vag와 같은 작업을 합니다.
		case I3SND_CMN_FORMAT_VAG:
		{
			switch( dstFormat)
			{
				case I3SND_CMN_FORMAT_WAV:
				{
					return I3SNDUtil_Convert_VAG2WAV(  pSrc, nSrcLength, pDst );
				}break;

				default:
					break;
			}
		}
		break;

		default:
			break;
	}

	return false;
}

//루프되는 구간이 정해져 있는 vag음원이어야한다면 nFlags는 2가 default
//루프 시작블럭의 nFlags는 6, 루프음의 종료 블럭은 3
//vag는 루프 종료블럭 이후는 decoding하지 않으므로 종료 블럭을 만나면 디코딩종료
//***streamming의 경우는 다시 고려해볼것 **
bool I3SNDUtil_Convert_WAV2VAG(char * pSrc, INT32	nSrcLength, char * pDst)
{
	nDecodeFactor = 0;
	nShiftFactor = 0;

	INT32	nSampleSize = nSrcLength>>1;
	INT32	nSize = nSampleSize/28;
	if( nSampleSize%28 )	nSize ++;
	nSize = 16 * ( nSize + 2 );

	INT16	pBuffer[128 * 28];
	REAL32	rSamples[28];
	INT16	FourBit[28];
	INT32	i;
	INT32	nFlags = 0;
	char	Factors = 0;

	//
	for( INT32	nn = 0 ; nn < 16; ++nn)
	{
		*pDst++ = 0;
	}
	nSampleSize -= 16;

	//////////////////////////////////////////////////////////////////////////////
	while( nSampleSize > 0 )
	{
		nSize = ( nSampleSize >= I3SND_CONV_BUFFER_SIZE ) ?	I3SND_CONV_BUFFER_SIZE : nSampleSize;

		memcpy( pBuffer, pSrc, nSize * sizeof(INT16));

		i = nSize/28;

		if( nSize%28 )	//16바이트의 1블럭중 14byte가 14개의 sample로서 저장되게되는데.(1byte당 2sample)
		{				//28로 나누어떨어지지 않는 음성데이터의경우 뒷부분은 0으로 처리
			for( INT32 j = nSize%28; j<28; j++ )
			{
				pBuffer[28*i+j] = 0;
			}
			i++;
		}

		for( INT32	nCount = 0; nCount< i ; nCount++)
		{
			INT16 * pSample = pBuffer + nCount * 28;//28샘플 단위로 

			I3SNDUtil_FindDecodeFactor( pSample, rSamples );

			I3SNDUtil_Pack_VAG( rSamples, FourBit);

			Factors = (char)(nDecodeFactor<<4 | nShiftFactor);

			*pDst++ = Factors;
			*pDst++ = (char)nFlags;

			for(INT32 k = 0; k<28; k+= 2)
			{
				Factors = (char)(((FourBit[k+1]>>8)& 0xf0 ) | ( (FourBit[k] >> 12 ) & 0xf ));
				*pDst++ = Factors;	//
			}

			nSampleSize -= 28;

			if( nSampleSize < 28 )	nFlags = 1;
		}
	}

	Factors = (char)( nDecodeFactor<<4 | nShiftFactor);
	*pDst++ = Factors;		//

	Factors = (char)7;
	*pDst++ = Factors;

	Factors = 0x77;
	for( INT32	nzero = 0 ; nzero < 14; ++nzero)
	{
		*pDst++ = Factors;
	}

	return true;
}

bool I3SNDUtil_Convert_VAG2WAV(char * pSrc, INT32	nSrcLength, char * pDst)
{
    static REAL32	s_1 = 0.0;
    static REAL32	s_2 = 0.0;

	char			Temp;
	INT32			nFlags;
	INT32			i,d,s;
	INT32			nSrcCount = 0;
	INT32			nDstCount = 0;
	REAL32			samples[28];

	nDecodeFactor	= 0;
	nShiftFactor	= 0;
	bool	bEndDecode = false;
	bool	bContinue = true;

	//
	pSrc += 16;//vag파일의 header 48바이트 이후 16바이트는 0으로 채워져있습니다.
	nSrcCount += 16;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while( bContinue ) 
	{
 		Temp = *pSrc++;
		nDecodeFactor = (INT32)Temp;

        nShiftFactor = nDecodeFactor & 0xf;
        nDecodeFactor >>= 4;

		Temp = *pSrc++;
		nFlags = (INT32)Temp;		// flags

		//OneShot Play VagFile의경우 마지막 블럭 이전의 블럭에 플래그가 1이되고
		//vageditor 2000년버전 툴은 nShiftFactor == 7이면, 2004년 버전은 nFlags == 7 이면 디코딩을 종료합니다.
		//결국 종료 플래그는 이전블럭의 플래그가 1이나 3이되어야 나올수 있으므로 종료위치는 nFlag == 1 이나 nFlags == 3인경우에만 유효합니다.
		//Loop Play인경우 마지막 블럭의 nFlags가 3이고 블럭 디코딩을 마치면 종료합니다.
		if( bEndDecode == true )
		{
			break;
		}

		//nFlags가 1이면 다음블럭에서 디코딩을 종료하고, 3이면 현재 블럭을 마친후 종료합니다.
		if(( nFlags == 1 ) || ( nFlags == 3 ))
		{
			bEndDecode = true;
		}

		//File에 직접 덮어쓰는방식이아닌 메모리를 이용하여 convert하는 방법이므로
		//전해진 메모리의 양보다 큰경우는 강제로 디코딩을 종료합니다.
		//oneshot의경우는 마지막블럭의 nFlags까지, loop는 마지막블럭의 끝까지 decode
		if( nSrcCount >= nSrcLength - 16)
		{
			break; 
		}

        for ( i = 0; i < 28; i += 2 ) 
		{
			Temp = *pSrc++;
			d = (INT32)Temp;

            s = ( d & 0xf ) << 12;

            if ( s & 0x8000 )  s |= 0xffff0000;
            samples[i] = (REAL32) ( s >> nShiftFactor );

            s = ( d & 0xf0 ) << 8;
            if ( s & 0x8000 )  s |= 0xffff0000;

            samples[i+1] = (REAL32) ( s >> nShiftFactor );
        }

		nSrcCount += 16;

        for ( i = 0; i < 28; i++ ) 
		{
            samples[i] = samples[i] + s_1 * DeMultiCoef[nDecodeFactor][0] + s_2 * DeMultiCoef[nDecodeFactor][1];

            s_2 = s_1;

            s_1 = samples[i];

            d = (INT32) ( samples[i] + 0.5 );

			*pDst++ = (UINT8)(d & 0xff);
			*pDst++ = (UINT8)(d >> 8);
			nDstCount += 2;
        }

		//Loop Play의경우 루프 종료 블럭flag입니다.
		if( nFlags == 3 )
		{
			break;
		}
    }

	return true;
}
//
void	I3SNDUtil_FindDecodeFactor( INT16 * pSamples,REAL32	*rSamples)
{
	static REAL32	_s_1	= 0.0f;
	static REAL32	_s_2	= 0.0f;

	REAL32	rBuffer[28][5];
	REAL32	rMin = 1e10;
	REAL32	rMax[5];
	REAL32	ds;

	INT32	nMin2;
	INT32	nShiftMask;

	REAL32	s_0 = 0.0f;
	REAL32	s_1 = 0.0f;
	REAL32	s_2 = 0.0f;

	for(INT32	i = 0; i < 5; i++)
	{
		rMax[i] = 0.0f;
		s_1 = _s_1;
		s_2 = _s_2;

		for( INT32	j = 0; j<28; ++j)
		{
			s_0 = (REAL32)pSamples[j];

			if( s_0 > 30719.0f )	s_0 = 30719.0f;
			if( s_0 < -30720.0f )	s_0 = -30720.0f;

			ds = s_0 + s_1 * MultiCoef[i][0] + s_2 * MultiCoef[i][1];
			rBuffer[j][i] = ds;

			if( fabs(ds) > rMax[i] )	rMax[i] = fabs( ds );

			s_2 = s_1;// s[t-2]
			s_1 = s_0;// s[t-1]
		}

		if( rMax[i] < rMin )
		{
			rMin = rMax[i];
			nDecodeFactor = i;
		}

		if( rMin <= 7 )
		{
			nDecodeFactor = 0;
			break;
		}
	}

	_s_1 = s_1;
	_s_2 = s_2;

	for(INT32	n = 0; n< 28; n++)
	{
		rSamples[n] = rBuffer[n][nDecodeFactor];
	}

	nMin2 = (INT32)rMin;

	nShiftMask = 0x4000;

	nShiftFactor = 0;

	while( nShiftFactor < 12 )
	{
		if( nShiftMask & ( nMin2 + ( nShiftMask >> 3)))	break;

		nShiftFactor++;

		nShiftMask = nShiftMask>>1;
	}
}

void	I3SNDUtil_Pack_VAG(REAL32 * pSamples, INT16 * pFourBit)
{
	REAL32	ds;
	REAL32	s_0 = 0.0f;
	static REAL32	s_1 = 0.0f;
	static REAL32	s_2 = 0.0f;


	INT32	di;
	
	for( INT32	i = 0 ; i < 28 ; ++i)
	{
		s_0 = pSamples[i] + s_1 * MultiCoef[nDecodeFactor][0] + s_2 * MultiCoef[nDecodeFactor][1];

		ds = s_0 * (REAL32)(1<< nShiftFactor);

		di = ((INT32)ds + 0x800) & 0xfffff000;

		if( di > 32767 )	di = 32767;
		if( di < -32768 )	di = -32768;

		pFourBit[i] = (short) di;

		di = di>>nShiftFactor;

		s_2 = s_1;

		s_1 = (REAL32)di - s_0;
	}

}


static UINT32 I3SNDUtil_LoadWaveHeader( i3Stream * pStream, i3::pack::SOUND_INFO_HEADER * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	UINT32 Result, Rc = 0;
	INT32	nTotalSize;	//Header 의 8바이트( file id, sizeinfo)를 제외한 총 파일크기
	{
		Result = pStream->Read( &nTotalSize, sizeof( INT32));
		I3_CHKIO( Result);
	}

	char wavFmt[8];
	{//Chk fmt  --> ext파일을 지원하게 되면 읽는 방법을 변경시킬수 있습니다.
		Rc = pStream->Read( wavFmt, 8);//sizeof( I3SND_WAVHEADER ));
		I3_CHKIO( Result);
		Result += Rc;

//		if( i3String::NCompare( wavFmt, "WAVEfmt ", 8) != 0)
		if (i3::generic_is_niequal(wavFmt, "WAVEfmt ", 8) == false)
		{
			I3TRACE("not a supported file\n");
			return STREAM_ERR;
		}
	}

	INT32	nChunkData;
	{
		Rc = pStream->Read( &nChunkData, sizeof( INT32));//sizeof( I3SND_WAVHEADER ));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	PCMWAVEFORMAT wav;
	{
		Rc = pStream->Read( &wav, sizeof( PCMWAVEFORMAT));
		I3_CHKIO( Result);
		Result += Rc;

		if( wav.wf.wFormatTag != WAVE_FORMAT_PCM)
		{
			I3TRACE("Load WavHeaderInfo failed\n");
			return STREAM_ERR;
		}

		pInfo->m_nFormat			= wav.wf.wFormatTag;
		pInfo->m_nAvgBytesPerSec	= wav.wf.nAvgBytesPerSec;
		pInfo->m_nChannels			= wav.wf.nChannels;
		pInfo->m_nSampleRate		= wav.wf.nSamplesPerSec;
		pInfo->m_nBlockAlign		= wav.wf.nBlockAlign;
		pInfo->m_nBitsPerSample		= (wav.wf.nBlockAlign / wav.wf.nChannels) << 3;

		if( nChunkData > sizeof( PCMWAVEFORMAT))
		{
			INT32 nRemain = nChunkData - sizeof( PCMWAVEFORMAT);

			pStream->SetPosition( nRemain, STREAM_CURRENT);

			Result += nRemain;
		}
	}

	{// Check "data" tag
		char tag[4];
		Rc = pStream->Read( tag, 4);
		I3_CHKIO( Result);
		Result += Rc;

		/*
		if( i3String::NCompare( tag, "data", 4) != 0)
		{
			I3TRACE("Load WavHeaderInfo failed\n");
			return STREAM_ERR;
		}*/
	}

	INT32 nDataLength;
	{
		Rc = pStream->Read( &nDataLength, sizeof( INT32));
		I3_CHKIO( Result);
		Result += Rc;
	}

	pInfo->m_nDataSize = nDataLength;

	return Result;
}

static UINT32 I3SNDUtil_LoadVagHeader( i3Stream * pFile, i3::pack::SOUND_INFO_HEADER * pInfo)
{
	return STREAM_ERR;
}

static UINT32 I3SNDUtil_Loadi3SndHeaer( i3Stream * pFile, i3::pack::SOUND_INFO_HEADER * pInfo)
{
	return STREAM_ERR;
}

UINT32 I3SNDUtil_LoadFileInfo( i3Stream * pStream, i3::pack::SOUND_INFO_HEADER * pInfo)
{
	char ID[5];
	UINT32 Result, Rc = 0;

	Result = pStream->Read( ID, 4);
	ID[4] = '\0';

	I3_CHKIO( Result);

//	if( i3String::NCompare( ID, "RIFF", 4) == 0)
	if (i3::generic_is_niequal(ID, "RIFF", 4) )
	{
		Rc = I3SNDUtil_LoadWaveHeader( pStream, pInfo);
	}
//	else if( i3String::NCompare( ID, "VAGp", 4) == 0)
	else if (i3::generic_is_niequal(ID, "VAGp", 4) )
	{
		Rc = I3SNDUtil_LoadVagHeader( pStream, pInfo);
	}
//	else if( i3String::NCompare( ID, "I3SF", 4) == 0)
	else if (i3::generic_is_niequal(ID, "I3SF", 4) )
	{
		Rc = I3SNDUtil_Loadi3SndHeaer( pStream, pInfo);
	}
	else
	{
		I3TRACE("Not a valid sound file\n");
	}

	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	I3SNDUtil_LoadWaveFileFormat( const char * szSndName, WAVEFORMATEX * waveFormatEx, HMMIO hMMIO)
{
	MMCKINFO		mmCkInfoRIFF; 
	MMCKINFO		mmCkInfoChunk; 

	if( hMMIO == nullptr && szSndName == nullptr)
		return STREAM_ERR;

	if( hMMIO == nullptr)
	{
		// Open the wave file. 
		hMMIO = mmioOpen((LPSTR)szSndName, nullptr, MMIO_READ | MMIO_ALLOCBUF);
		if (hMMIO == nullptr)	
			return STREAM_ERR;
	}

	// RIFF chunk를 찾아간다.
	mmCkInfoRIFF.fccType = mmioFOURCC ('W', 'A', 'V', 'E'); 
	if (mmioDescend(hMMIO, &mmCkInfoRIFF, nullptr, MMIO_FINDRIFF) != MMSYSERR_NOERROR) 
	{
		return STREAM_ERR;
	}
	
	// format chunk를 찾아간다.
	mmCkInfoChunk.ckid = mmioFOURCC ('f', 'm', 't', ' '); 	
	if (mmioDescend(hMMIO, &mmCkInfoChunk, &mmCkInfoRIFF, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) 
	{
		return STREAM_ERR;
	}
 				
	// WAVEFORMATEX 구조체로 부터 포맷정보를 읽어온다.
	if (mmioRead(hMMIO, (char*)waveFormatEx, sizeof(WAVEFORMATEX)) == -1)
	{
		return STREAM_ERR;
	}

	// Ascend out of the format chunk. 
	if (mmioAscend(hMMIO, &mmCkInfoChunk, 0) != MMSYSERR_NOERROR) 
	{
		return STREAM_ERR; 
	}
	
	// data chunk로 찾아간다.
	mmCkInfoChunk.ckid = mmioFOURCC ('d', 'a', 't', 'a'); 
	if (mmioDescend(hMMIO, &mmCkInfoChunk, &mmCkInfoRIFF, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) 
	{
		return STREAM_ERR;
	}

	// WAV 데이터의 크기를 저장해둔다. 
	DWORD waveSize = mmCkInfoChunk.cksize; 

	//mmioClose( hMMIO, 0);

	return (UINT32)waveSize;
}
