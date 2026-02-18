#ifndef I3_SOUND_GLOBAL_H__
#define I3_SOUND_GLOBAL_H__

#include "i3SoundContext.h"

I3_EXTERN_SOUND i3SoundContext	*	g_pSoundCtx;

#if defined( I3SND_FMOD)
extern FMOD::System *				g_pFMODSystem;
#endif

#endif//
