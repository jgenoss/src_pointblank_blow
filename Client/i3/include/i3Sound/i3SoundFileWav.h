#ifndef I3_SOUNDFILE_WAV_H__
#define I3_SOUNDFILE_WAV_H__

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)						// [test required] 패킹사이즈 누락으로 추가함. 2017.02.14. soon9
#endif

namespace i3
{
	namespace pack
	{
		struct PACKED SOUND_WAV_HEADER
		{
			char	cRIFF[4] = { 'R', 'I', 'F', 'F' };			// must be 'RIFF'
			UINT32	length = 0;				// bytes + 44 -8
			char	cWave[8] = { 'W', 'A', 'V', 'E', 'f', 'm', 't' };			// 'WAVEfmt '
			INT32	nChunkData = 0;			// chunk data size;
			INT16	nWavFormat = 0;			// PCM/Uncompressed (1)
			INT16	nChannels = 0;			// mono/stereo
			INT32	nSampleRate = 0;		// sample frequency
			INT32	nBytesPerSec = 0;		// 
			INT16	nBlockAlign = 0;		//
			INT16	nBitsPerSample = 0;		// bits per sample
			char	cdata[4] = { 'd', 'a', 't', 'a' };			// 'data'
			INT32	nDataLength = 0;		// cbsize
		};
	}
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

#endif // I3_SOUNDFILE_WAV_H__
//
//struct WAVEFORMAT {
//    WORD    wFormatTag;        /* format type */
//    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
//    DWORD   nSamplesPerSec;    /* sample rate */
//    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
//    WORD    nBlockAlign;       /* block size of data */
//} ;
//
//
//	/* specific waveform format structure for PCM data */
//struct PCMWAVEFORMAT {
//    WAVEFORMAT  wf;
//    WORD        wBitsPerSample;
//} ;
//
//struct WAVEFORMATEX
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
//} ;