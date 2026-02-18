////////////////////////////////////////////////////////////////////////////////////////
//	FileName	: i3SoundFMOD.h
//
//	Description	: i3SoundFMOD Header File
//
//	2005/11/10 ~	raja
////////////////////////////////////////////////////////////////////////////////////////
#ifndef I3_SOUND_FMOD_H__
#define I3_SOUND_FMOD_H__

#include "i3Sound.h"

#if defined( I3SND_FMOD)

#include "fmod.hpp"

class I3_EXPORT_SOUND i3SoundFMOD :	public i3Sound
{
	I3_CLASS_DEFINE( i3SoundFMOD );
protected:

	FMOD::Sound	 *		m_p3DSound;
	FMOD::Sound *		m_p2DSound;

	UINT32				m_nDataLength;	//i3SoundFMOD에서는 m_nDataSize는 메모리버퍼크기이고 m_nDataLength가 실제 사운드데이터의 길이를 나타냄

public:
	i3SoundFMOD(void);
	virtual ~i3SoundFMOD(void);

	virtual FMOD::Sound *		Get3DSound( bool bEnableCreate = true, bool bHeadRelative = false);
	virtual FMOD::Sound *		Get2DSound( bool bEnableCreate = true);

	virtual REAL32		GetDuration();
	virtual BOOL		Create( char * pszPath);

	static const char * getErrorMsg( FMOD_RESULT code);
};

#define i3SoundPlatform			i3SoundFMOD

#endif //#if defined( I3SND_DSOUND)

#endif //I3_SOUND_DX_H__