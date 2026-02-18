#ifndef I3_SOUNDFILE_WAV_H__
#define I3_SOUNDFILE_WAV_H__

typedef struct PACKED tag_WavHeader
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
}I3SND_WAVHEADER;

#endif // I3_SOUNDFILE_WAV_H__
//
//typedef struct waveformat_tag {
//    WORD    wFormatTag;        /* format type */
//    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
//    DWORD   nSamplesPerSec;    /* sample rate */
//    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
//    WORD    nBlockAlign;       /* block size of data */
//} WAVEFORMAT;
//
//
//	/* specific waveform format structure for PCM data */
//typedef struct pcmwaveformat_tag {
//    WAVEFORMAT  wf;
//    WORD        wBitsPerSample;
//} PCMWAVEFORMAT;
//
//typedef struct tWAVEFORMATEX
//{
//    WORD    wFormatTag;        /* format type */
//    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
//    DWORD   nSamplesPerSec;    /* sample rate */
//    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
//    WORD    nBlockAlign;       /* block size of data */
//    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
//    WORD    cbSize;            /* The count in bytes of the size of
//                                    extra information (after cbSize) */
//
//} WAVEFORMATEX;