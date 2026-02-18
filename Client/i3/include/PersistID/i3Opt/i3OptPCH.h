#if !defined( __I3_OPT_H)
#define __I3_OPT_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Scene.h"

#if defined( I3_WINDOWS) && defined( I3_DLL)
	#if defined( I3_OPT_BUILD)
		#define I3_EXPORT_OPT		__declspec( dllexport)
		#define I3_EXTERN_OPT		extern
	#else
		#define I3_EXPORT_OPT		__declspec( dllimport)
		#define I3_EXTERN_OPT		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_OPT)
		#define	I3_EXPORT_OPT
		#define I3_EXTERN_OPT		extern
	#endif
#endif

#endif