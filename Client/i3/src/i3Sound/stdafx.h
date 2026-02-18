// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#if defined( I3_WINDOWS)
	#pragma once
	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
	#include <windows.h>
	#include <mmsystem.h>
#endif

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3SoundConfig.h"
#include "i3SoundDef.h"

#if defined( I3SND_DSOUND)
	#include "./ogg/ogg.h"
	#include "./ogg/os_types.h"
	#include "./ogg/vorbisfile.h"
	#include "./ogg/vorbisenc.h"
	#include "./ogg/codec.h"

	I3_EXTERN_SOUND LPDIRECTSOUND8		g_pDS;
#elif defined( I3SND_FMOD)
	#include "fmod.hpp"
#endif
