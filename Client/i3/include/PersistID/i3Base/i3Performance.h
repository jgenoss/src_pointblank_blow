#ifndef __I3_PERFORMANCE_H__
#define __I3_PERFORMANCE_H__

#include "i3CommonType.h"
#include "i3Timer.h"
#include "i3Macro.h"

#if !defined( I3_NO_PROFILE)

I3_EXPORT_BASE void INIT_PROFILE(void);
I3_EXPORT_BASE void SET_PROFILE( INT32 slot, const char * pszStr);
I3_EXPORT_BASE void START_PROFILE( INT32 slot);
I3_EXPORT_BASE void END_PROFILE( INT32 slot);
I3_EXPORT_BASE void RESET_PROFILE( bool bClearHistory = false);
I3_EXPORT_BASE void REPORT_PROFILE( char * pszDest = NULL);
I3_EXPORT_BASE bool CHECK_PROFILE( INT32 slot, double sec);

#define		__SP(a)						START_PROFILE( a)
#define		__EP(a)						END_PROFILE( a)
#define		__CP(a, sec)				CHECK_PROFILE( a, sec)

#else

	#if defined( I3_COMPILER_VC)
		#define SET_PROFILE( a, b)				__noop
		#define START_PROFILE( a)				__noop
		#define END_PROFILE( a)					__noop
		#define RESET_PROFILE()					__noop
		#define REPORT_PROFILE()				__noop

		#define	__SP(a)							__noop
		#define __EP(a)							__noop
	#elif defined( I3_COMPILER_GCC)
		#define SET_PROFILE( a, b)				((void) 0)
		#define START_PROFILE( a)				((void) 0)
		#define END_PROFILE( a)					((void) 0)
		#define RESET_PROFILE()					((void) 0)
		#define REPORT_PROFILE()				((void) 0)
		#define __SP(a)							((void) 0)
		#define __EP(a)							((void) 0)
	#endif

#endif


#endif //__I3_PERFORMANCE_H__
