#if !defined( _I3SOUND_STREAM_H__)
#define _I3SOUND_STREAM_H__

#include "i3Sound.h"

#if defined( I3SND_DSOUND)
	#include "i3SoundDx.h"
#elif defined( I3SND_FMOD)
	#include "i3SoundFMOD.h"
#endif

typedef struct tag_position_set
{
	UINT32						index;				//	포지션 정보의 인덱스 넘버
	UINT32						option;				//	옵션
	UINT32						start_pos;			//	시작 부분의 샘플 넘버
	UINT32						loop_start_pos;		//	루핑할 부분의 샘플 넘버
	UINT32						size;				//	전체 샘플 수
	struct tag_position_set		*next;				//	다음 포지션 정보의 주소

} position_set;

class I3_EXPORT_SOUND i3SoundStream : public i3SoundPlatform
{
	I3_ABSTRACT_CLASS_DEFINE( i3SoundStream);

protected:

public:
	i3SoundStream(void);
	virtual ~i3SoundStream(void);

	virtual BOOL	Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce);
	virtual void	OnCreate();

};

#endif