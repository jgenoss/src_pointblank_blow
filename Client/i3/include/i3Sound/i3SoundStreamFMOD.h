#if !defined( _I3SOUND_STREAM_FMOD_H__)
#define _I3SOUND_STREAM_FMOD_H__

#if defined( I3SND_FMOD)
	#include "i3SoundFMOD.h"
	#include "i3SoundStream.h"

class I3_EXPORT_SOUND i3SoundStreamFMOD : public i3SoundStream
{
	I3_EXPORT_CLASS_DEFINE_BY_TYPE(i3SoundStreamFMOD, i3SoundStream, i3::cms_concrete_fmod::value );

public:
	virtual FMOD::Sound *		Get3DSound( bool bEnableCreate = true, bool bHeadRelative = false) override;
	virtual FMOD::Sound *		Get2DSound( bool bEnableCreate = true) override;

	virtual bool	Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce) override;
};

#endif //

#endif