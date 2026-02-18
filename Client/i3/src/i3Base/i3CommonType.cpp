// i3CommonType.cpp : source file that includes just the standard includes
// i3Base.pch will be the pre-compiled header
// i3CommonType.obj will contain the pre-compiled type information

#include "i3CommonType.h"

#if defined( I3_PSP)
#if defined( I3_COMPILER_GCC)
//SCE_MODULE_INFO( i3Base, 0, 1, 0);
//SCE_LIB_DECLARE( i3Base, 0);
#else
//SCE_LIB_VERSION_CHECK_OFF( i3Base);
#endif
#endif
