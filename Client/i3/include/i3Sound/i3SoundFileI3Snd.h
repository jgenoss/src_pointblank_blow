#ifndef I3_SOUNDFILE_I3SND_H__
#define I3_SOUNDFILE_I3SND_H__

// 1channel 
// 16bits
// 44.1khz

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가함. 2017.02.14. soon9
#endif

namespace i3
{
	namespace pack
	{
		struct SOUND_I3SND_HEADER
		{
			UINT8	m_ID[4] = { 'S', 'N', 'F', '1' };
			INT32	m_nNativeFormat = 0;
		};//vag Header와 
	}
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

#endif
