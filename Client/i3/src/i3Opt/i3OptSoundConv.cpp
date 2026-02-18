#include "i3OptPCH.h"
#include "i3OptSoundConv.h"

#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( i3OptSoundConv);

#define BUFFER_SIZE	128 * 28

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

#define nBNTOCC( n )			(((n>>24)&0xff) | ((n>>16)&0xff)<<8 | ((n>>8)&0xff)<<16 | ((n&0xff)<<24));

i3OptSoundConv::i3OptSoundConv(void)
{
	m_Class = CLASS_SOUND;
}

bool  i3OptSoundConv::ConvertSound( I3SND_FORMAT DestFormat, const char * szSrcFileName)
{
	i3FileStream File;
	if( !File.Open( szSrcFileName , STREAM_READ | STREAM_SHAREREAD ))	return false;

	switch( DestFormat )
	{
	case I3SND_FORMAT_RIFF_PCM_16MONO:
		{
			ADPCMHEADER ah;
			if( ReadADPCMHeader( &File, &ah ))
			{
				return ConvertADPCM2PCM( &File, &ah);
			}
		}
		break;
	case I3SND_FORMAT_ADPCM_MONO:
		{
			PCMHEADER	ph;
			if( ReadPCMHeader( &File, &ph ))
			{
				return ConvertPCM2ADPCM( &File, &ph );
			}
		}
		break;
	default:
		{
			I3TRACE("not a invalid format\n");
		}
		break;

	}

	I3TRACE("Not a Audio File\n");
	return false;
}

char * i3OptSoundConv::ConvertSound( I3SND_FORMAT SrcFormat, I3SND_FORMAT DestFormat, char *pBuffer)
{
	return nullptr;
}

bool	i3OptSoundConv::ReadPCMHeader( i3FileStream * pSrcStream, PCMHEADER * ph)
{
	I3ASSERT( ph != nullptr );
	I3ASSERT( pSrcStream != nullptr );

	pSrcStream->SetPosition( 0, STREAM_BEGIN);
	pSrcStream->Read( ph, 44 );

	if( i3::generic_is_niequal( ph->cRIFF, "RIFF", 4) == false )
	{
		I3TRACE("Not a RIFF File\n");
		return false;
	}

	if( i3::generic_is_niequal( ph->cWave, "WAVEfmt ", 8 ) == false)
	{
		I3TRACE("Not a WAVE File\n");
		return false;
	}

	if( ph->nWavFormat != 1 )
	{
		I3TRACE("No PCM\n");
		return false;
	}
	
	if( ph->nChannels != 1 )
	{
		I3TRACE("must be mono\n");
		return false;
	}

	if( ph->nBitsPerSample != 16 )
	{
		I3TRACE("Only 16Bit Samples\n");
		return false;
	}

	if( ph->nBlockAlign != (2 * ph->nChannels) )
	{
		I3TRACE("Only 4Byte Block Align\n");
		return false;
	}

	if(i3::generic_is_niequal( ph->cdata, "data", 4 ) == false )
	{
		I3TRACE("no data Chunk\n");
		return false;
	}

	return true;
}

bool	i3OptSoundConv::WritePCMHeader( i3FileStream * pStream, ADPCMHEADER * ah )
{
	I3ASSERT( pStream != nullptr );

	PCMHEADER ph;
	i3mem::FillZero( &ph, sizeof( PCMHEADER ));

	strncpy( ph.cRIFF, "RIFF", sizeof(ph.cRIFF) -1);
	
	ph.nDataLength = nBNTOCC( ah->nSampleSize);//ah->nSampleSize = (((ph.nDataLength/2)/28)+2)*16
											   //16바이트 단위로 28sample과 factor/flag를 가지는 1블럭의 갯수

	ph.nDataLength >>= 4;		// 16을 나누고
	ph.nDataLength -= 2;
	ph.nDataLength *= 28 * 2;	//실제 28sample로 나누어떨어지지 않는 마지막부분은 0으로 채워져있음.

	ph.length = 44 - 8 + ph.nDataLength;//datasize + 44 - 8;

	memcpy( ph.cWave, "WAVEfmt ", sizeof(ph.cWave));
	ph.nChunkData		= 16;			// ChunkData Size;
	ph.nWavFormat		= 1;			// PCM/Uncompressed;
	ph.nChannels		= 1;			// Mono
	ph.nSampleRate		= 44100;
	ph.nBytesPerSec		= 44100 * 2;	// if(ph.nChannels > 1)( ph.nBytesPerSec * ph.nChannels);
	ph.nBlockAlign		= 2;			// if(ph.nChannels>1)( ph.nBlockAlign * ph.nChannels);
	ph.nBitsPerSample	= 16;			// 16bit
	memcpy( ph.cdata, "data", sizeof( ph.cdata));
	

	pStream->SetPosition( 0, STREAM_BEGIN);
	pStream->Write( &ph, sizeof( PCMHEADER ) );

	return true;
}

bool	i3OptSoundConv::ReadADPCMHeader( i3FileStream * pStream, ADPCMHEADER * ah)
{
	pStream->SetPosition( 0, STREAM_BEGIN);
	pStream->Read( ah, sizeof( ADPCMHEADER ));

	if( i3::generic_is_niequal( ah->cFormat, "VAGp", 4) == false)
	{
		I3TRACE("NOT a ADPCM-VAG FILE\n");
		return false;
	}
	return true;
}

bool	i3OptSoundConv::WriteADPCMHeader( i3FileStream * pStream, PCMHEADER * ph)
{
	ADPCMHEADER ah;
	i3mem::FillZero( &ah, sizeof( ADPCMHEADER ));

	memcpy( ah.cFormat, "VAGp", sizeof(ah.cFormat));
	ah.nVersion = nBNTOCC( 0x20 );
	ah.nReserved = 0x00;

	INT32	nSize = ph->nDataLength>>1;							//DataSize 4
	nSize = nSize/28;
	if( ph->nDataLength%28 )	nSize ++;
	nSize = 16 * ( nSize + 2 );
	ah.nSampleSize = nBNTOCC( nSize );

	ah.nSampleperSec = nBNTOCC( ph->nSampleRate);				//SamplingFrequency 4

	char szFileName[MAX_PATH];
	i3::safe_string_copy( szFileName, pStream->GetFileName(), MAX_PATH );
	i3::remove_ext( szFileName );							//name 16
	i3::safe_string_copy( ah.cName, szFileName, 16 );
																//이름이 16바이트가 넘는경우를 위해 이름부터 16바이트이후의
	i3mem::FillZero( ah.cReserved2, 16 );							//reserved 16을 0으로 셋

	pStream->SetPosition( 0, STREAM_BEGIN);
	pStream->Write( &ah, sizeof( ADPCMHEADER ));
	return true;
}


void	i3OptSoundConv::FilePutInt(INT32	d, i3FileStream * pStream)
{
	char c;

	c = (char)(d>>24);
	pStream->Write( &c,1);

	c = (char)(d>>16);
	pStream->Write( &c,1);

	c = (char)(d>>8);
	pStream->Write( &c,1);

	c = (char)d;
	pStream->Write( &c,1);
}

bool i3OptSoundConv::ConvertPCM2ADPCM( i3FileStream * pStream, PCMHEADER * ph )
{
	I3ASSERT( pStream != nullptr );
	I3ASSERT( ph	!= nullptr );

	//Set File Name
	char szFileName[MAX_PATH];
	strncpy( szFileName, pStream->GetFileName(), sizeof(szFileName)-1);
	i3::remove_ext( szFileName );
	strncat( szFileName, ".vag", sizeof(szFileName)-1 );

	//Open Stream
	i3FileStream VagStream;
	if( VagStream.Create( szFileName , STREAM_WRITE ) == false)
		return false;

	WriteADPCMHeader( &VagStream, ph );

	INT32	nSampleSize = ph->nDataLength>>1;
	INT32	nSize = nSampleSize/28;
	if( nSampleSize%28 )	nSize ++;
	nSize = 16 * ( nSize + 2 );

	INT16	pBuffer[BUFFER_SIZE];
	REAL32	rSamples[28];
	INT16	FourBit[28];
	INT32	i;
	INT32	nFlags = 0;
	char	Factors = 0;

	pStream->SetPosition( 44, STREAM_BEGIN);
	//////////////////////////////////////////////////////////////////////////////
	while( nSampleSize > 0 )
	{
		nSize = ( nSampleSize >= BUFFER_SIZE ) ?	BUFFER_SIZE : nSampleSize;

		pStream->Read( pBuffer, nSize * sizeof(INT16) );

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

			FindDecodeFactor( pSample, rSamples );

			Pack_ADPCM_4MONO( rSamples, FourBit);

			Factors = (char)(m_nDecodeFactor<<4 | m_nShiftFactor);

			VagStream.Write( &Factors, 1);	//
			VagStream.Write( &nFlags, 1);	//

			for(INT32 k = 0; k<28; k+= 2)
			{
				Factors = (char)(((FourBit[k+1]>>8)& 0xf0 ) | ( (FourBit[k] >> 12 ) & 0xf ));
				VagStream.Write(&Factors, 1);	//
			}

			nSampleSize -= 28;

			if( nSampleSize < 28 )	nFlags = 1;
		}
	}

	Factors = (char)( m_nDecodeFactor<<4 | m_nShiftFactor);
	VagStream.Write(&Factors,1);		//
	Factors = (char)7;
	VagStream.Write(&Factors,1);		//
	Factors = 0;
	for( INT32	nzero = 0 ; nzero < 14; ++nzero)
	{
		VagStream.Write(&Factors,1);			//
	}
	VagStream.Close();

	return true;
}

bool i3OptSoundConv::ConvertADPCM2PCM( i3FileStream * pStream, ADPCMHEADER * ah )
{
	//Set File Name
	char szFileName[MAX_PATH];
	strncpy( szFileName, pStream->GetFileName(), sizeof( szFileName)-1);
	i3::remove_ext( szFileName );
	strncat( szFileName, "mod.wav", sizeof( szFileName)-1 );

	//Open Stream
	i3FileStream VagStream;

	if( VagStream.Create( szFileName , STREAM_WRITE ) == false)
		return false;

	WritePCMHeader( &VagStream, ah );

	char	Temp;
	INT32	nFlags;
	INT32	i,d,s;
	REAL32	samples[28];
    static REAL32 s_1 = 0.0;
    static REAL32 s_2 = 0.0;

	pStream->SetPosition( 64, STREAM_BEGIN );//48바이트가 헤더라고 되어있지만 실질pcm데이터는 64바이트 이후부터 시작된다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bool bLoop = true;
	while( bLoop )	// avoid to warning
	{
		pStream->Read( &Temp, 1 );
		m_nDecodeFactor = (INT32)Temp;

        m_nShiftFactor = m_nDecodeFactor & 0xf;
        m_nDecodeFactor >>= 4;

		pStream->Read( &Temp, 1 );
		nFlags = (INT32)Temp;		// flags

        if ( nFlags == 7 )   break;              

        for ( i = 0; i < 28; i += 2 ) 
		{
			pStream->Read( &Temp, 1);
			d = (INT32)Temp;

            s = ( d & 0xf ) << 12;

            if ( s & 0x8000 )  s |= 0xffff0000;
            samples[i] = (REAL32) ( s >> m_nShiftFactor );

            s = ( d & 0xf0 ) << 8;
            if ( s & 0x8000 )  s |= 0xffff0000;

            samples[i+1] = (REAL32) ( s >> m_nShiftFactor );
        }

        for ( i = 0; i < 28; i++ ) 
		{
            samples[i] = samples[i] + s_1 * DeMultiCoef[m_nDecodeFactor][0] + s_2 * DeMultiCoef[m_nDecodeFactor][1];

            s_2 = s_1;

            s_1 = samples[i];

            d = (int) ( samples[i] + 0.5 );

			char data;
			data =  (char)(d & 0xff);

			VagStream.Write( &data, 1);
			data =  (char)(d >> 8);
			VagStream.Write( &data, 1);
        }
    }

	VagStream.Close();
	return true;
}
//
void	i3OptSoundConv::FindDecodeFactor( INT16 * pSamples,REAL32	*rSamples)
{
	static REAL32	_s_1 = 0.0f;
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
			m_nDecodeFactor = i;
		}

		if( rMin <= 7 )
		{
			m_nDecodeFactor = 0;
			break;
		}
	}

	_s_1 = s_1;
	_s_2 = s_2;

	for(INT32	n = 0; n< 28; n++)
	{
		rSamples[n] = rBuffer[n][m_nDecodeFactor];
	}

	nMin2 = (INT32)rMin;

	nShiftMask = 0x4000;

	m_nShiftFactor = 0;

	while( m_nShiftFactor < 12 )
	{
		if( nShiftMask & ( nMin2 + ( nShiftMask >> 3)))	break;

		m_nShiftFactor++;

		nShiftMask = nShiftMask>>1;
	}
}

void	i3OptSoundConv::Pack_ADPCM_4MONO(REAL32 * pSamples, INT16 * pFourBit)
{
	REAL32	ds;
	REAL32	s_0 = 0.0f;
	static REAL32	s_1 = 0.0f;
	static REAL32	s_2 = 0.0f;


	INT32	di;
	
	for( INT32	i = 0 ; i < 28 ; ++i)
	{
		s_0 = pSamples[i] + s_1 * MultiCoef[m_nDecodeFactor][0] + s_2 * MultiCoef[m_nDecodeFactor][1];

		ds = s_0 * (REAL32)(1<< m_nShiftFactor);

		di = ((INT32)ds + 0x800) & 0xfffff000;

		if( di > 32767 )	di = 32767;
		if( di < -32768 )	di = -32768;

		pFourBit[i] = (short) di;

		di = di>>m_nShiftFactor;

		s_2 = s_1;

		s_1 = (REAL32)di - s_0;
	}

}
