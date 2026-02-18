#include "stdafx.h"
#include "stdio.h"
#include "i3SoundStreamFMOD.h"
#include "i3SoundGlobal.h"

#if defined( I3SND_FMOD)

#include "i3SoundStreamFMOD.h"

I3_CLASS_INSTANCE( i3SoundStreamFMOD);

bool i3SoundStreamFMOD::Create( const char * pszFileName, WCHAR32 nBufferSize, WCHAR32 nGap, WCHAR32 nReadOnce)
{
	i3SoundStream::Create( pszFileName, nBufferSize, nGap, nReadOnce);

	// Streaming이기 때문에 아무런 Loading이 필요없다.

	return true;
}

FMOD::Sound *	i3SoundStreamFMOD::Get3DSound( bool bEnableCreate, bool bHeadRelative)
{
	if( m_p3DSound == nullptr)
	{
		FMOD_CREATESOUNDEXINFO info;
		//FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_3D | FMOD_OPENMEMORY);
		//FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_3D | FMOD_CREATESTREAM | FMOD_3D_LINEARROLLOFF);
		FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_3D | FMOD_CREATESTREAM | FMOD_3D_LOGROLLOFF | FMOD_LOOP_NORMAL);

		i3mem::FillZero( &info, sizeof(info));
		info.cbsize = sizeof(info);
		info.length = m_nDataSize;

		if( bHeadRelative)
			mode = (FMOD_MODE)((UINT32) mode | FMOD_3D_HEADRELATIVE);

		// 아직 3D Sound가 재생된 적이 없기 때문에 생성한다.
		FMOD_RESULT rv = g_pFMODSystem->createSound( GetName(), mode, &info, &m_p3DSound);
		ERRCHECK( rv);
	}

	return m_p3DSound;
}

FMOD::Sound *	i3SoundStreamFMOD::Get2DSound( bool bEnableCreate)
{
	if( m_p2DSound == nullptr)
	{
		FMOD_CREATESOUNDEXINFO info;
		FMOD_MODE mode = (FMOD_MODE)( FMOD_DEFAULT | FMOD_2D | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL);

		i3mem::FillZero( &info, sizeof(info));
		info.cbsize = sizeof(info);
		info.length = m_nDataSize;

		// 아직 3D Sound가 재생된 적이 없기 때문에 생성한다.
		FMOD_RESULT rv = g_pFMODSystem->createSound( GetName(), mode, &info, &m_p2DSound);
		ERRCHECK( rv);
	}

	return m_p2DSound;
}

#endif
