#ifndef __NSM_HEADER_H__
#define __NSM_HEADER_H__

//----------------------------------------------------------------------
#include <windows.h>
#include <winsock2.h>

//----------------------------------------------------------------------
typedef unsigned char			UINT8;
typedef signed short int		INT16;
typedef unsigned short int		UINT16;
typedef __int64					INT64;
typedef unsigned __int64		UINT64;
typedef float 					REAL32;
typedef double 					REAL64;
typedef long					FIXED32;
typedef char *					PSTR;
typedef unsigned int			OBJREF;
typedef unsigned __int64		OBJREF64;
typedef unsigned short      	WORD;
typedef WCHAR					WCHAR16;
typedef unsigned long			WCHAR32;

//----------------------------------------------------------------------
#ifdef NSMDLL_EXPORTS
	#define	EXPORT_BASE	__declspec( dllexport)
#else
	#define	EXPORT_BASE	__declspec( dllimport)
#endif

//----------------------------------------------------------------------
#define	MIN( a, b)				( (a) > (b) ? (b) : (a))
#define	MAX( a, b)				( (a) > (b) ? (a) : (b))

#endif
