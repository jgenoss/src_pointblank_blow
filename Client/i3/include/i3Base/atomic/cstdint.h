#pragma once

// C99에 존재해도, MSVC에 없는 정수타입계열이 있음..

namespace i3
{
	typedef signed char     int8_t;
    typedef signed char     int_least8_t;
    typedef signed char     int_fast8_t;

	typedef unsigned char   uint8_t;
    typedef unsigned char   uint_least8_t;
    typedef unsigned char   uint_fast8_t;
	 
	typedef short           int16_t;
    typedef short           int_least16_t;
    typedef short           int_fast16_t;

    typedef unsigned short  uint16_t;
    typedef unsigned short  uint_least16_t;
    typedef unsigned short  uint_fast16_t;
	
	typedef long            int32_t;
    typedef long            int_least32_t;
    typedef long            int_fast32_t;

    typedef unsigned long   uint32_t;
    typedef unsigned long   uint_least32_t;
    typedef unsigned long   uint_fast32_t;

	typedef __int64             intmax_t;
    typedef unsigned __int64    uintmax_t;

	typedef __int64             int64_t;
	typedef __int64             int_least64_t;
	typedef __int64             int_fast64_t;

	typedef unsigned __int64    uint64_t;
	typedef unsigned __int64    uint_least64_t;
	typedef unsigned __int64    uint_fast64_t;
		
}

#if (_MSC_VER < 1700)			// VS 2012
#define INT8_C(value)     value##i8
#define INT16_C(value)    value##i16
#define INT32_C(value)    value##i32
#define INT64_C(value)    value##i64

#define UINT8_C(value)    value##ui8
#define UINT16_C(value)   value##ui16
#define UINT32_C(value)   value##ui32
#define UINT64_C(value)   value##ui64

#define INTMAX_C(value)   value##i64
#define UINTMAX_C(value)  value##ui64
#endif