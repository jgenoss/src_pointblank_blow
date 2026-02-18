#ifndef I3_SOUNDFILE_VAG_H__
#define I3_SOUNDFILE_VAG_H__

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)						// [test required] 패킹사이즈 누락으로 추가함. 2017.02.14. soon9
#endif

namespace i3
{
	namespace pack
	{
		struct SOUND_VAG_HEADER
		{
			char	cFormat[4] = { 'V', 'A', 'G', 'p' };			// 'V','A','G','p'
			INT32	nVersion = 0;			// 0x20
			INT32	nReserved = 0;			// 0x00
			INT32	nSampleSize = 0;		// cbsize
			INT32	nSampleperSec = 0;		// sample frequency
			UINT16	volL = 0;				// base volume for Left channel //
			UINT16	volR = 0;				// base volume for Right channel //
			UINT16	pitch = 0;				// base pitch (includes fs modulation)//
			UINT16	ADSR1 = 0;				// base ADSR1//
			UINT16	ADSR2 = 0;				// base ADSR2//
			UINT16	reserved = 0;			// not in use //
			char	cName[16] = { 0 };		// vag name
		};
	}
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

#endif // I3_SOUNDFILE_VAG_H__
