#if !defined( I3_SG_SCENE_DEF_H)
#define I3_SG_SCENE_DEF_H

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Input.h"
//#include "i3SoundH.h"
#include "i3AttrID.h"

// 함수 추적 디버깅 기능 활성화
//#define TRACE_I3_SCENE

// Compact node procedure

#if defined( I3_COMPILER_PRODG)
#pragma diag_suppress=using_pch
#pragma diag_suppress=creating_pch
#endif

#if (defined( I3_WINDOWS) || defined(I3_WINCE))&& defined( I3_DLL)
	#if defined( I3_SCENE_BUILD)
		#define	I3_EXPORT_SCENE		__declspec( dllexport)
	#else
		#define	I3_EXPORT_SCENE		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_SCENE)
		#define	I3_EXPORT_SCENE
	#endif
#endif

//#define NO_DISPLAYLIST_RENDER
#endif
