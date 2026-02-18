#ifndef I3_SOUNDFILE_VAG_H__
#define I3_SOUNDFILE_VAG_H__

typedef struct tag_SONY_ADPCMheader
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
} I3SND_VAGHEADER;

#endif // I3_SOUNDFILE_VAG_H__
