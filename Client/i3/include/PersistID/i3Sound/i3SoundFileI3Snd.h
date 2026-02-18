#ifndef I3_SOUNDFILE_I3SND_H__
#define I3_SOUNDFILE_I3SND_H__

// 1channel 
// 16bits
// 44.1khz

typedef struct tag_i3SndHeader
{
	UINT8	m_ID[4];
	INT32	m_nNativeFormat;
} I3SND_I3SNDHEADER;//vag Header¿Í 
#endif
