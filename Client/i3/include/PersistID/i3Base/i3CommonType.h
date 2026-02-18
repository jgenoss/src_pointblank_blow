// i3CommonType.h

#ifndef __I3_COMMON_TYPE_H
#define __I3_COMMON_TYPE_H

// what?

#include "i3Base/i3Platform.h"

#if defined( I3_WINDOWS )
	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

	#if !defined( _WIN32_WINNT)
		#define	_WIN32_WINNT		0x0501
	#endif

	#include <windows.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <xmmintrin.h>			// mmx/sse

	#include <dbghelp.h>			// minidump 20070420 김현우
	#include <io.h>					// 20070504 김현우

#elif defined( I3_XBOX )
	#include <xtl.h>

#elif defined( I3_PSP)
	#include <stdio.h>
	#include <stdlib.h>
	#include <ctype.h>
	#include <malloc.h>
	#include <string.h>
	#include <psptypes.h>
	#include <libgu.h>
	#include <iofilemgr.h>
	#include <kerror.h>
	#include <rtcsvc.h>
	#include <thread.h>
	#include <moduleexport.h>

#elif  defined( I3_WIPI)
	// #pragma GCC java_exceptions

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>

	#if defined( I3_WIPI_GIGA)
		#ifndef FEATURE_GIGA
			#define FEATURE_GIGA
		#endif

		#include	"MCgiga_types.h"
		#include	"MCgiga_comdef.h"
		#include	"MCgCOP.h"
		#include	"MCg2D.h"
		#include	"MCg3D.h"
		#include	"MCgMU.h"
		#include	"WIPIheader.h"
		#include	"ApiLinkGIga.h"
		#include	"MCApiGiga.h"
	#else
		#include	"WIPIHeader.h"
		#include	"MCGame.h"
		#include	"MCegl.h"

		// KTF SDK 버그 회피용
		#undef inline
		#define	inline	__inline
	#endif

	// MCknl.h의 이통사별 함수 정의
	#include "i3WIPIknl.h"

#elif defined( I3_WINCE )
	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//	#if !defined( _WIN32_WINNT)
//		#define	_WIN32_WINNT		0x0501
//	#endif

	#include <windows.h>
	#include <stdio.h>
	#include <stdlib.h>

#endif
/////////////////////////////////////////////////////////////
// PC
#ifdef I3_WINDOWS

	typedef unsigned char			UINT8;

	typedef signed short int		INT16;
	typedef unsigned short int		UINT16;

	typedef __int64					INT64;
	typedef unsigned __int64		UINT64;

	typedef float 					REAL32;
	typedef double 					REAL64;
	typedef __m128					REAL128;

	typedef long					FIXED32;

	typedef char *					PSTR;
	typedef unsigned int			OBJREF;
	typedef unsigned __int64		OBJREF64;

	typedef WCHAR					WCHAR16;
	typedef unsigned long			WCHAR32;


	typedef int						I3MEM;
#endif
/////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////
// XBOX
#ifdef I3_XBOX

	typedef char					INT8;
	typedef unsigned char			UINT8;

	typedef signed short int		INT16;
	typedef unsigned short int		UINT16;

	typedef float 					REAL32;
	typedef double 					REAL64;
	typedef __m128					REAL128;

	typedef long					FIXED32;

	typedef	char *					PSTR;
	typedef unsigned int			OBJREF;

	typedef unsigned short int		WCHAR16;
	typedef unsigned long			WCHAR32;

	typedef int						I3MEM; 
#endif
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// PSP
#ifdef I3_PSP
	typedef char					INT8;
	typedef unsigned char			UINT8;

	typedef signed short int		INT16;
	typedef unsigned short int		UINT16;

	typedef int						INT32;
	typedef unsigned int			UINT32;

	typedef long long				INT64;
	typedef unsigned long long		UINT64;

	typedef float 					REAL32;
	typedef double 					REAL64;

	typedef long					FIXED32;

	typedef	char *					PSTR;
	typedef unsigned long			OBJREF;

	typedef unsigned short int		WCHAR16;
	typedef unsigned long			WCHAR32;

	typedef int						I3MEM;
#endif

/////////////////////////////////////////////////////////////
// WIPI
#ifdef I3_WIPI
	#if !defined( I3_WIPI_GIGA)
	typedef char					INT8;
	typedef unsigned char			UINT8;

	typedef signed short int		INT16;
	typedef unsigned short int		UINT16;

	typedef int						INT32;
	typedef unsigned int			UINT32;
	#endif
	
	typedef long long				INT64;
	typedef unsigned long long		UINT64;

	typedef float 					REAL32;
	typedef double 					REAL64;

	typedef long					FIXED32;

	typedef	M_Char *				PSTR;
	typedef unsigned long			OBJREF;

	typedef unsigned short int		WCHAR16;
	typedef unsigned long			WCHAR32;

	typedef int						I3MEM;
	typedef GLfixed					I3FIXED;
#endif

/////////////////////////////////////////////////////////////
// WINCE
#ifdef I3_WINCE
	typedef unsigned char			UINT8;

	typedef signed short int		INT16;
	typedef unsigned short int		UINT16;

	typedef __int64					INT64;
	typedef unsigned __int64		UINT64;

	typedef float 					REAL32;
	typedef double 					REAL64;
//	typedef __m128					REAL128;

	typedef long					FIXED32;

	typedef char *					PSTR;
	typedef unsigned int			OBJREF;
	typedef unsigned __int64		OBJREF64;

	typedef WCHAR					WCHAR16;
	typedef unsigned long			WCHAR32;


	typedef int						I3MEM;
#endif

#if defined( ALIGN1)
	#undef		ALIGN1
#endif

#if defined( ALIGN2)
#undef		ALIGN2
#endif

#if defined( ALIGN4)
#undef		ALIGN4
#endif

#if defined( ALIGN8)
#undef		ALIGN8
#endif

#if defined( ALIGN16)
#undef		ALIGN16
#endif

#if defined( ALIGN32)
#undef		ALIGN32
#endif

#if defined( ALIGN64)
#undef		ALIGN64
#endif

#if defined( ALIGN128)
#undef		ALIGN128
#endif

#if defined( PACKED)
#undef		PACKED
#endif

#if defined( BEGIN_INLINE)
#undef		BEGIN_INLINE
#endif

#if defined( END_INLINE)
#undef		END_INLINE
#endif


/////////////////////////////////////////////////////////////
// Alignment
#if defined( I3_COMPILER_VC)
	#define		ALIGN1
	#define		ALIGN2
	#define		ALIGN4
	#define		ALIGN8
	#define		ALIGN16
	#define		ALIGN32
	#define		ALIGN64
	#define		ALIGN128
	#define		PACKED
	#define		BEGIN_INLINE		__forceinline
	#define		END_INLINE
	#define		NOTHROW

	#pragma warning( disable : 4201 )
	#pragma warning( disable : 4324 )

#elif defined( I3_COMPILER_GCC)
	#define		ALIGN1				__attribute__ ((aligned (1)))
	#define		ALIGN2				__attribute__ ((aligned (2)))
	#define		ALIGN4				__attribute__ ((aligned (4)))
	#define		ALIGN8				__attribute__ ((aligned (8)))
	#define		ALIGN16				__attribute__ ((aligned (16)))
	#define		ALIGN32				__attribute__ ((aligned (32)))
	#define		ALIGN64				__attribute__ ((aligned (64)))
	#define		ALIGN128			__attribute__ ((aligned (128)))
	#define		PACKED				__attribute__ ((packed))
	#define		BEGIN_INLINE		inline
	#define		END_INLINE			__attribute__ ((always_inline))
	#define		NOTHROW				__attribute__ ((nothrow))
#elif defined( I3_COMPILER_PRODG)
	#define		ALIGN1				__attribute__ ((aligned (1)))
	#define		ALIGN2				__attribute__ ((aligned (2)))
	#define		ALIGN4				__attribute__ ((aligned (4)))
	#define		ALIGN8				__attribute__ ((aligned (8)))
	#define		ALIGN16				__attribute__ ((aligned (16)))
	#define		ALIGN32				__attribute__ ((aligned (32)))
	#define		ALIGN64				__attribute__ ((aligned (64)))
	#define		ALIGN128			__attribute__ ((aligned (128)))
	#define		PACKED				__attribute__ ((packed))
	#define		BEGIN_INLINE		inline
	#define		END_INLINE			__attribute__ ((always_inline))
	#define		NOTHROW				__attribute__ ((nothrow))

	// Disable Warning 613 -
	// overloaded virtual function "Baseclass::Func" is only partially overridden in class "Derived Class"
	#pragma diag_suppress=partial_override		 

	// Disable Warning 999
	// function "Baseclass::Func(...)" is hidden by "DerivedClass::Func" -- virtual function override intended?
	#pragma diag_suppress=virtual_function_decl_hidden

	#pragma diag_suppress=using_pch
	#pragma diag_suppress=creating_pch

#endif

#define		I3_INLINE			inline

/////////////////////////////////////////////////////////////
// Useful defines
#ifndef BOOL
	#if !defined( I3_WIPI_GIGA)
	typedef int						BOOL;
	#endif 

	#define							FALSE				0
	#define							TRUE				1
#endif

#if defined( I3_FIXED_POINT)
	typedef FIXED32					RT_REAL32;
	#define		__RT(a)				((FIXED32)(((a) * 65536.0f) + 0.5f))
	#define		__PT(a)				(((REAL32)(a)) * 0.152587890625E-4f)
	#define		__RTI(a)			((a) << 16) 
	#define		__PTI(a)			((a) >> 16)
	#define		__RT_ZERO			0
	#define		__RT_0				0
	#define		__RT_ONE			65536
	#define		__RT_1				65536
#else
	typedef REAL32					RT_REAL32;
	#define		__RT(a)				(a)
	#define		__PT(a)				(a)
	#define		__RTI(a)			((REAL32)(a))
	#define		__PTI(a)			((INT32) (a))
	#define		__RT_ZERO			0.0f
	#define		__RT_0				0.0f
	#define		__RT_ONE			1.0f
	#define		__RT_1				1.0f
#endif

#ifndef MAX_PATH
	#define		MAX_PATH			256
#endif

#ifdef I3_DEBUG 
#else
#define I3_NO_PROFILE
#endif

#endif
