#include "stdafx.h"
#include "i3SoundGlobal.h"

i3SoundContext	*	g_pSoundCtx		= nullptr;

#if defined( I3SND_FMOD)
FMOD::System *				g_pFMODSystem = nullptr;
#endif