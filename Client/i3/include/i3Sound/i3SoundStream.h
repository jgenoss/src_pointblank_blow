#if !defined( _I3SOUND_STREAM_H__)
#define _I3SOUND_STREAM_H__

#include "i3Sound.h"

#if defined( I3SND_DSOUND)
	#include "i3SoundDx.h"
#elif defined( I3SND_FMOD)
	#include "i3SoundFMOD.h"
#endif

struct position_set
{
	UINT32						index = 0;				//	포지션 정보의 인덱스 넘버
	UINT32						option = 0;				//	옵션
	UINT32						start_pos = 0;			//	시작 부분의 샘플 넘버
	UINT32						loop_start_pos = 0;		//	루핑할 부분의 샘플 넘버
	UINT32						size = 0;				//	전체 샘플 수
	struct position_set			*next = nullptr;		//	다음 포지션 정보의 주소

} ;

class I3_EXPORT_SOUND i3SoundStream : public i3SoundPlatform
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3SoundStream, i3SoundPlatform);

public:
	virtual bool	Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce);
	virtual void	OnCreate() {}

};

#endif