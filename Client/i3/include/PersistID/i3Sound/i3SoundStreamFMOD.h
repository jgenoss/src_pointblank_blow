#if !defined( _I3SOUND_STREAM_FMOD_H__)
#define _I3SOUND_STREAM_FMOD_H__

#if defined( I3SND_FMOD)
	#include "i3SoundFMOD.h"
	#include "i3SoundStream.h"

class I3_EXPORT_SOUND i3SoundStreamFMOD : public i3SoundStream
{
	I3_CLASS_DEFINE( i3SoundStreamFMOD);
protected:

public:
	i3SoundStreamFMOD(void);
	virtual ~i3SoundStreamFMOD();

	virtual FMOD::Sound *		Get3DSound( bool bEnableCreate = true, bool bHeadRelative = false);
	virtual FMOD::Sound *		Get2DSound( bool bEnableCreate = true);

	virtual BOOL	Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce);
};

#endif //

#endif