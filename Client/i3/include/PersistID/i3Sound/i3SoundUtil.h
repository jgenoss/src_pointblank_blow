#ifndef I3_SOUND_UTIL_H__
#define I3_SOUND_UTIL_H__

#include "i3SoundGlobal.h"

#define nBNTOCC( n )			(((n>>24)&0xff) | ((n>>16)&0xff)<<8 | ((n>>8)&0xff)<<16 | ((n&0xff)<<24));


INT32	I3_EXPORT_SOUND I3SNDUtil_CalculateBufferSize( UINT32 srcFormat, UINT32 dstFormat, INT32 nSrcLength);
BOOL	I3_EXPORT_SOUND I3SNDUtil_ConvertSound(UINT32 srcFormat,INT32	nSrcLength, char *pSrc, UINT32 dstFormat, INT32	nDstLength, char *pDst);
BOOL	I3_EXPORT_SOUND I3SNDUtil_Convert_WAV2VAG(char * pSrc, INT32	nSrcLength, char * pDst);
BOOL	I3_EXPORT_SOUND I3SNDUtil_Convert_VAG2WAV(char * pSrc, INT32	nSrcLength, char * pDst);
void	I3_EXPORT_SOUND I3SNDUtil_FindDecodeFactor( INT16 * pSamples,REAL32	*rSamples);
void	I3_EXPORT_SOUND I3SNDUtil_Pack_VAG(REAL32 * pSamples, INT16 * pFourBit);

UINT32	I3_EXPORT_SOUND I3SNDUtil_LoadFileInfo( i3Stream * pStream, I3SND_INFOHEADER * pInfo);
UINT32	I3_EXPORT_SOUND I3SNDUtil_LoadWaveFileFormat(const char * szSndName, WAVEFORMATEX * waveFormatEx, HMMIO hMMIO = NULL);

#endif //I3_SOUND_UTIL_H__
