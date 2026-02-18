#if !defined( __I3_PHYSICSTYPE_H)
#define __I3_PHYSICSTYPE_H

#include "i3base.h"
#include "i3Gfx.h"
#include "i3Scene.h"

#include "i3PhysicsDefine.h"

#if defined( I3_WINDOWS) && defined( I3_DLL)
	#if defined( I3_PHYSICS_BUILD)
		#define I3_EXPORT_PHYSICS		__declspec( dllexport)
		#define I3_EXTERN_PHYSICS		extern
	#else
		#define I3_EXPORT_PHYSICS		__declspec( dllimport)
		#define I3_EXTERN_PHYSICS		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_PHYSICS)
		#define	I3_EXPORT_PHYSICS
		#define I3_EXTERN_PHYSICS		extern
	#endif
#endif

#endif	// __I3_PHYSICSTYPE_H
