#pragma once

#include "itl/range/literal_range.h"
#include "i3OverloadingMacro.h"

I3_EXPORT_BASE	void I3PRINTF( const i3::literal_range& format);
I3_EXPORT_BASE	void I3PRINTF( const i3::wliteral_range& format);

#if defined( I3_DEBUG) || !defined( I3_NO_PROFILE)
#define I3TRACE(fmt, ...)				I3PRINTF(fmt, __VA_ARGS__)
#else
#if defined( I3_COMPILER_VC)
#define I3TRACE(fmt, ...)				__noop
#else
#define I3TRACE(fmt, ...)				((void)0)
#endif
#endif

#define		I3TRACE_LINE(X)				I3TRACE( "%s (%d) : %s\n", __FILE__, __LINE__, X );

// 아래 I3TRACE_N구현은 이미 호환성문제를 위해서만 남겨두어진것으로 보이며, 
// 따라서 더이상 쓰지 않도록 한다..
#define		I3TRACE0(a)						I3TRACE(a)
#define		I3TRACE1(a,b)					I3TRACE(a,b)
#define		I3TRACE2(a,b,c)					I3TRACE(a,b,c)
#define		I3TRACE3(a,b,c,d)				I3TRACE(a,b,c,d)
#define		I3TRACE4(a,b,c,d,e)				I3TRACE(a,b,c,d,e)
#define		I3TRACE5(a,b,c,d,e,f)			I3TRACE(a,b,c,d,e,f)
#define		I3TRACE6(a,b,c,d,e,f,g)			I3TRACE(a,b,c,d,e,f,g)
#define		I3TRACE7(a,b,c,d,e,f,g,h)		I3TRACE(a,b,c,d,e,f,g,h)

#if defined( I3_DEBUG)
	#if defined( I3_WINDOWS)
	#define I3ASSERT(__C)\
			{\
			if( !(__C))\
				{\
				I3assertForCoverity();\
				i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_FATAL);\
				i3Error::Log( "Asserted : %s", #__C);\
				}\
			}
	#define I3ASSERT_0\
			{\
					{	__debugbreak(); }\
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
			DebugBox( nullptr, "%s (%d)", __FILE__, __LINE__);\
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
					i3Error::SetEnv( nullptr, __LINE__, __FUNCSIG__, I3LOG_NOTICE);\
					i3Error::Log( "Asserted : %s", #__C);\
					}\
				}
	#endif
	#define I3ASSERT_0					(void)0
#endif

#if 1
#define CHECK_TRACE	{ I3TRACE2( "TRACE - %s (%d)\n", __FILE__, __LINE__ );	}
#else
#define CHECK_TRACE
#endif 


#define I3ASSERT_RETURN(Cond, ...)  I3_OVERLOADING_MACRO(I3ASSERT_RETURN, Cond, __VA_ARGS__)

#define I3ASSERT_RETURN1(Cond) \
	{ \
		if (!(Cond)) \
		{ \
			I3ASSERT(Cond); \
			return; \
		} \
	}

#define I3ASSERT_RETURN2(Cond, ReturnValue) \
	{ \
		if (!(Cond)) \
		{ \
			I3ASSERT(Cond); \
			return (ReturnValue); \
		} \
	}


//#ifdef I3_DEBUG
#define I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
//#endif
