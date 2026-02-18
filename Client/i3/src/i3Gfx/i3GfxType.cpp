// i3GFXType.cpp : source file that includes just the standard includes
// i3GFX.pch will be the pre-compiled header
// i3GFXType.obj will contain the pre-compiled type information

#include "i3GfxType.h"

#if defined( I3_PSP)
#if defined( I3_COMPILER_GCC)
//SCE_MODULE_INFO( i3Gfx, 0, 1, 0);
//SCE_LIB_DECLARE( i3Gfx, 0);
#else
//SCE_LIB_VERSION_CHECK_OFF( i3Gfx);
#endif
#endif
