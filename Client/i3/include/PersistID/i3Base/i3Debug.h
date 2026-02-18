#if !defined(__I3_DEBUG_H)
#define __I3_DEBUG_H

#include "i3CommonType.h"
#include "i3Macro.h"
#include "i3ErrorHandler.h"

extern void *g_pi3DebugPointer;
extern INT32 g_ni3DebugIndex;

#if defined(I3_WINDOWS) || defined( I3_WINCE )
	I3_EXPORT_BASE	int DebugBox( HWND Owner, const char * lpszText, ...);
	I3_EXPORT_BASE	int	NotifyBox( HWND hwnd, const char * lpszText, ...);
#else
	I3_EXPORT_BASE	int DebugBox( void* pNull, const char * lpszText, ...);
	I3_EXPORT_BASE	int NotifyBox( void* pNull, const char * lpszText, ...);
#endif

#define I3DEBUG_PORT				5556

//여기에는 없지만. I3Memory에서 UDP 덤프용으로 5557 번을 사요하겠습니다. 성백 (차후 변경되면 삭제 하겠음)

	typedef struct _tagDebugMsg {
		short int	m_Level;
		char		m_szModule[32];
		char		m_szMsg[222];
	} i3DEBUGMSG;

	extern char DebugServerIP[];
	I3_EXPORT_BASE void i3Log( const char * pszModule, const char *pszFormat, ...);

	typedef void (*i3LogCallBackFunc)( char* szMsg );
	I3_EXPORT_BASE void i3SetLogCallBackFunc( i3LogCallBackFunc pFunc );


I3_EXPORT_BASE	void I3PRINTF( const char *format, ...);

#if defined( I3_DEBUG) || !defined( I3_NO_PROFILE)
	#define I3TRACE								I3PRINTF
#else
	#if defined( I3_COMPILER_VC)
		#define I3TRACE							__noop
	#else
		#define I3TRACE(...)					((void)0)
	#endif
#endif

#define		I3TRACE_LINE(X)						I3TRACE3( "%s (%d) : %s\n", __FILE__, __LINE__, X );


#if defined( I3_DEBUG)
	#if defined( I3_WINDOWS)
		#define I3ASSERT(__C)\
		{\
			if( !(__C))\
			{\
			i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_FATAL);\
			i3Error::Log( "Asserted : %s", #__C);\
			}\
		}
		#define I3ASSERT_0\
		{\
				{	_asm { int 3 }; }\
		}
	#elif defined( I3_WINCE)
		#define I3ASSERT(__C)\
		{\
			if( !(__C))\
			{\
			i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_FATAL);\
			i3Error::Log( "Asserted : %s", #__C);\
			}\
		}
		#define I3ASSERT_0				(void)0
	#elif defined( I3_WIPI)
		#define I3ASSERT(__C)\
			{\
				if( !(__C))\
				{\
				I3TRACE2( "ASSERTION - %s, %d", __FILE__, __LINE__);\
				MC_knlExit( MC_knlGetCurProgramID());\
				}\
			}
		#define I3ASSERT_0\
			{\
				I3TRACE2( "ASSERTION - %s, %d", __FILE__, __LINE__);\
				MC_knlExit( MC_knlGetCurProgramID());\
			}
	#else
		#define I3ASSERT(__C)\
		{\
			if( !(__C))\
			{\
				i3Log( "ASSERTION", "%s, %d", __FILE__, __LINE__);\
				exit( -1);\
			}\
		}
		#define I3ASSERT_0\
		{\
				DebugBox( NULL, "%s (%d)", __FILE__, __LINE__);\
				exit( -1);\
		}
	#endif
#else
	//#define I3ASSERT(a)					(void)0
	#if defined( I3_WINDOWS)
			#define I3ASSERT(__C)\
			{\
				if( !(__C))\
				{\
					i3Error::SetEnv( NULL, __LINE__, __FUNCSIG__, I3LOG_NOTICE);\
					i3Error::Log( "Asserted : %s", #__C);\
				}\
			}
	#endif
	#define I3ASSERT_0					(void)0
#endif

#if defined( I3_DEBUG)
	I3_EXPORT_BASE void	I3DUMP( char * pBuf, INT32 Len, INT32 Width = 16);
	I3_EXPORT_BASE void	I3DUMP2( char * pBuf, INT32 Len, INT32 Width = 8);
	I3_EXPORT_BASE void	I3DUMP4( char * pBuf, INT32 Len, INT32 Width = 4);
#endif

#define		I3TRACE0( a)			I3TRACE( a)
#define		I3TRACE1( a, b)\
	{\
		char conv[128];\
		sprintf( (PSTR) conv, (PSTR) a, b);\
		I3TRACE( conv);\
	}

#define		I3TRACE2( a, b, c)\
	{\
		char conv[256];\
		sprintf( (PSTR) conv, (PSTR) a, b, c);\
		I3TRACE( conv);\
	}

#define		I3TRACE3( a, b, c, d)\
	{\
		char conv[256];\
		sprintf( (PSTR) conv, (PSTR) a, b, c, d);\
		I3TRACE( conv);\
	}

#define		I3TRACE4( a, b, c, d, e)\
	{\
		char conv[256];\
		sprintf( (PSTR) conv, (PSTR) a, b, c, d, e);\
		I3TRACE( conv);\
	}

#define		I3TRACE5( a, b, c, d, e, f)\
	{\
		char conv[256];\
		sprintf( (PSTR) conv, (PSTR) a, b, c, d, e, f);\
		I3TRACE( conv);\
	}

#define		I3TRACE6( a, b, c, d, e, f, g)\
	{\
		char conv[256];\
		sprintf( (PSTR) conv, (PSTR) a, b, c, d, e, f, g);\
		I3TRACE( conv);\
	}

#define		I3TRACE7( a, b, c, d, e, f, g, h)\
	{\
		char conv[256];\
		sprintf( (PSTR) conv, (PSTR) a, b, c, d, e, f, g, h);\
		I3TRACE( conv);\
	}

#if 1
#define CHECK_TRACE	{ I3TRACE2( "TRACE - %s (%d)\n", __FILE__, __LINE__ );	}
#else
#define CHECK_TRACE
#endif 

#endif //__JCDEBUG_H

