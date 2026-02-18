#if !defined( __I3_GIC_ALL_H)
#define __I3_GIC_ALL_H

#if defined( I3_WINDOWS)
	#if defined( I3_GIC_BUILD)
		#define	I3_EXPORT_GIC		__declspec( dllexport)
	#else
		#define	I3_EXPORT_GIC		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_GIC)
		#define	I3_EXPORT_GIC
	#endif
#endif

#endif
