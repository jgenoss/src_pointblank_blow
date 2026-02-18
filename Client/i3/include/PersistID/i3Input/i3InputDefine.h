#if !defined( I3_INPUT_DEFINE_H)
#define I3_INPUT_DEFINE_H

#define	I3I_CONTROLLER_MAX			4

#if (defined( I3_WINDOWS) || defined( I3_WINCE)) && defined( I3_DLL) 
	#if defined( I3_INPUT_BUILD)
		#define I3_EXPORT_INPUT			__declspec( dllexport)
		#define I3_EXTERN_INPUT			extern
	#else
		#define I3_EXPORT_INPUT			__declspec( dllimport)
		#define	I3_EXTERN_INPUT			__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_INPUT)
		#define I3_EXPORT_INPUT
		#define	I3_EXTERN_INPUT		extern
	#endif
#endif

#endif
