// i3GraphicType.h

#ifndef __I3_GRAPHIC_TYPE_H
#define __I3_GRAPHIC_TYPE_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3GfxDefine.h"

#if (defined( I3_WINDOWS) || defined( I3_WINCE)) && defined( I3_DLL)
	#if defined( I3_GFX_BUILD)
		#define I3_EXPORT_GFX		__declspec( dllexport)
		#define I3_EXTERN_GFX		extern
	#else
		#define I3_EXPORT_GFX		__declspec( dllimport)
		#define I3_EXTERN_GFX		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_GFX)
		#define	I3_EXPORT_GFX
		#define I3_EXTERN_GFX		extern
	#endif
#endif

#endif
