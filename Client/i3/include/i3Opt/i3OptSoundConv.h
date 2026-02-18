#if !defined( __I3_OPT_SOUND_CONV_H)
#define __I3_OPT_SOUND_CONV_H

#include "i3sceneoptimizer.h"

enum	I3SND_FORMAT
{
	I3SND_FORMAT_RIFF_PCM_16MONO,	//RIFF 16BIT PCM WAVE
	I3SND_FORMAT_ADPCM_MONO,		//VAG
	I3SND_FORMAT_MAX,
};

struct ADPCMHEADER
{
	char	cFormat[4];			// 'V','A','G','p'
	INT32	nVersion;			// 0x20
	INT32	nReserved;			// 0x00
	INT32	nSampleSize;		// cbsize
	INT32	nSampleperSec;		// sample frequency
	UINT16	volL;				// base volume for Left channel //
	UINT16	volR;				// base volume for Right channel //
	UINT16	pitch;				// base pitch (includes fs modulation)//
	UINT16	ADSR1;				// base ADSR1//
	UINT16	ADSR2;				// base ADSR2//
	UINT16	reserved;			// not in use //
	char	cName[16];			// vag name
	char	cReserved2[16];		// fill 0 -> 원래 header 48byte이후 16바이트는 0
} ;

 struct PCMHEADER
{
	char	cRIFF[4];			// must be 'RIFF'
	UINT32	length;				// bytes + 44 -8
	char	cWave[8];			// 'WAVEfmt '
	INT32	nChunkData;			// chunk data size;
	INT16	nWavFormat;			// PCM/Uncompressed (1)
	INT16	nChannels;			// mono/stereo
	INT32	nSampleRate;		// sample frequency
	INT32	nBytesPerSec;		// 
	INT16	nBlockAlign;		//
	INT16	nBitsPerSample;		// bits per sample
	char	cdata[4];			// 'data'
	INT32	nDataLength;		// cbsize
} ;//44byte


class I3_EXPORT_OPT i3OptSoundConv :	public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptSoundConv, i3SceneOptimizer );

public:
	i3OptSoundConv(void);

protected:
	INT32	m_nDecodeFactor = 0;
	INT32	m_nShiftFactor = 0;

public:
	bool 	ConvertSound( I3SND_FORMAT DestFormat, const char * szSrcFileName);						//파일을 통한 변환
	char *	ConvertSound( I3SND_FORMAT DestFormat, I3SND_FORMAT SrcFormat, char * pSrcBuf );		//버퍼 레벨에서의 변환

public:
	bool	ReadPCMHeader( i3FileStream * pStream, PCMHEADER * ph );
	bool	ReadADPCMHeader( i3FileStream * pStream, ADPCMHEADER * ah ); 

	bool	WritePCMHeader( i3FileStream * pStream, ADPCMHEADER * ah );
	bool	WriteADPCMHeader( i3FileStream * pStream, PCMHEADER * ph );

protected:
	//char *	ConvertPCM2ADPCM( char * pSrcBuffer, ADPCMHEADER * ah);
	//char *  ConvertADPCM2PCM( char * pSrcBuffer, PCMHEADER	* ph);
	bool	ConvertPCM2ADPCM( i3FileStream * pStream, PCMHEADER * ah );
	bool	ConvertADPCM2PCM( i3FileStream * pStream, ADPCMHEADER * ph);
	//
	void	FindDecodeFactor( INT16 *pBuffer, REAL32* rBuffer);
	void	Pack_ADPCM_4MONO( REAL32 *pSamples, INT16 *pFourBit);
	void	FilePutInt(INT32	d, i3FileStream * pStream);
	//void	PutInt( INT32	d, char * pBuff);
};

#endif