#pragma once

#ifdef BUILDMODE_DLL

#ifdef SERVER_EXPORTS
	#define	SERVER_EXPORT_BASE	__declspec( dllexport)
#else
	#define	SERVER_EXPORT_BASE	__declspec( dllimport)
#endif

#else
	#define SERVER_EXPORT_BASE
#endif