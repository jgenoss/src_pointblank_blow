#ifndef __I3_PLATFORM_H
#define __I3_PLATFORM_H

enum I3_PLATFORM
{
	I3_PLATFORM_UNKNOWN = 0,
	I3_PLATFORM_WINDOWS,	// PC, DirectX
	I3_PLATFORM_XBOX,		// Xbox
	I3_PLATFORM_XENON,		// Xbox2
	I3_PLATFORM_PS2,		// PlayStation 2
	I3_PLATFORM_PS3,		// PlayStation 3
	I3_PLATFORM_PSP,		// PlayStation Portable
	I3_PLATFORM_WIPI,		// KTF/SKT 3D GamePhone		Defined by KOMET
	I3_PLATFORM_WINCE		// WinCE
};

#if defined(_WINDOWS)
	#ifndef I3_WINDOWS
		#define I3_WINDOWS
	#endif

//	#ifndef I3_DLL
//		#define I3_DLL
//	#endif

#elif defined(_XBOX)
	#ifndef I3_XBOX
		#define I3_XBOX
	#endif
#elif defined(_LINUX)	
	#ifndef I3_LINUX
		#define I3_LINUX
	#endif
#elif defined(_PS2)
	#ifndef I3_PS2
		#define I3_PS2
	#endif
#elif defined(_WINCE)
	#ifndef I3_WINCE
		#define I3_WINCE
	#endif	
#endif

#if defined(I3_WINDOWS) || defined(I3_WINCE)
	// #pragma message ("Target platform is Windows.")
	#if defined(_XBOX) || defined(I3_XBOX) || defined(_LINUX) || defined(I3_LINUX) || defined(_PS2) || defined(I3_PS2)
		#error "Another platform is defined simultaneously."
	#endif

	#if defined( I3_DLL)
		#if defined( I3_BASE_BUILD)
			#define	I3_EXPORT_BASE		__declspec( dllexport)
			#define I3_EXTERN_BASE		extern
		#else
			#define I3_EXPORT_BASE		__declspec( dllimport)
			#define I3_EXTERN_BASE		__declspec( dllimport)
		#endif

		#define I3_EXPORT_TEMPLATE		__declspec( dllexport)
	#endif

	#if defined(I3_WINDOWS)
		#define I3_PHYSX
	#endif

	#if defined(I3_WINCE)
		#ifndef I3G_OGLES
			#define I3G_OGLES
		#endif

		#ifndef I3G_G10
			#define I3G_G10
		#endif
	#endif

#elif defined(I3_XBOX)
	// #pragma message ("Target platform is Xbox.")
	#if defined(_WINDOWS) || defined (I3_WINDOWS) || defined(_LINUX) || defined(I3_LINUX) || defined(_PS2) || defined(I3_PS2)
		#error "Another platform is defined simultaneously."
	#endif
	#ifndef I3G_XBOX
		#define I3G_XBOX
	#endif

	#define I3_EXPORT_TEMPLATE
#elif defined(I3_LINUX)
	// #pragma message ("Target platform is Linux.")
	#if defined(_WINDOWS) || defined (I3_WINDOWS) || defined(_XBOX) || defined(I3_XBOX) || defined(_PS2) || defined(I3_PS2)
		#error "Another platform is defined simultaneously."
	#endif

	#define I3_EXPORT_TEMPLATE
#elif defined(I3_PS2)
	// #pragma message ("Target platform is Playstation2.")
	#if defined(_WINDOWS) || defined (I3_WINDOWS) || defined (_XBOX) || defined(I3_XBOX) || defined(_LINUX) || defined(I3_LINUX)
		#error "Another platform is defined simultaneously."
	#endif
	#ifndef I3G_PS2
		#define I3G_PS2
	#endif

	#define I3_EXPORT_TEMPLATE
#elif defined( I3_PSP)
	#if !defined( I3G_PSP)
		#define I3G_PSP
	#endif

	#define I3_EXPORT_TEMPLATE
// WIPI 환경에서 Global(Static)으로 정의된 Class들은 Constructor/Destructor가 호출되지
// 않는다.
#elif defined( I3_WIPI)
	#if !defined( I3G_OGLES)
		#define I3G_OGLES		
	#endif

	#if !defined( I3_NO_STATIC_CLASSMETA)
		#define I3_NO_STATIC_CLASSMETA
	#endif
	
	#define I3_EXPORT_TEMPLATE
#else
	#error "You must select target platform by defining one of I3_WINDOWS, I3_XBOX, I3_LINUX and I3_PS2."
#endif

#if defined(I3G_DX)
	// #pragma message ("Graphics APIs is DirectX.")
	#if defined(I3G_OGLES) || defined(I3G_XBOX) || defined(I3G_PS2)
		#error "Another Graphics APIs is defined."
	#endif
	
	#define	I3_GFXCAPS_TRANSFORMED_VERTEX			0

#elif defined(I3G_XBOX)
	// #pragma message ("Graphics APIs is Xbox SDK.")
	#if defined(I3G_DX) || defined(I3G_OGLES) || defined(I3G_PS2)
		#error "Another Graphics APIs is defined."
	#endif

	#define	I3_GFXCAPS_TRANSFORMED_VERTEX			1

#elif defined(I3G_PS2)
	// #pragma message ("Graphics APIs is PS2 SDK.")
	#if defined(I3G_DX) || defined(I3G_OGLES) || defined(I3G_XBOX)
		#error "Another Graphics APIs is defined."
	#endif

	#define	I3_GFXCAPS_TRANSFORMED_VERTEX			1

#elif defined( I3G_PSP)
	#if defined(I3G_DX) || defined(I3G_OGLES) || defined(I3G_XBOX) || defined( I3G_PS2)
		#error "Another Graphics APIs is defined."
	#endif

	#define	I3_GFXCAPS_TRANSFORMED_VERTEX			1

#elif defined( I3G_OGLES)
	#if defined(I3G_DX) ||  defined(I3G_XBOX) || defined( I3G_PS2) || defined( I3G_PSP)
		#error "Another Graphics APIs is defined."
	#endif

	#define	I3_GFXCAPS_TRANSFORMED_VERTEX			0
#else
	#error "You must select graphics APIs by defining one of I3G_DX, I3G_OGL, I3G_XBOX and I3G_PS2."
#endif

#if !defined( I3_EXPORT_BASE)
	#define	I3_EXPORT_BASE
	#define I3_EXTERN_BASE				extern
	#define I3_EXPORT_TEMPLATE
#endif

#endif
