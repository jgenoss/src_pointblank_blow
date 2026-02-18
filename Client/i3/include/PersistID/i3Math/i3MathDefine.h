#if !defined( __I3_MATH_DEFINE_H)
#define __I3_MATH_DEFINE_H

#include "i3MathType.h"
#include "i3MathGlobalVariable.h"
#include "limits"
 
#if defined( I3_PSP)
#include <math.h>
//#define VFPU_ASM_MATH
#endif

#if defined( I3_WIPI) || defined( I3_WINCE )		//	05.8.1 by KOMET

	#ifdef sin
	#undef sin
	#endif
	
	#ifdef sinh
	#undef sinh
	#endif

	#ifdef cos
	#undef cos
	#endif
	
	#ifdef cosh
	#undef cosh
	#endif

	#ifdef tan
	#undef tan
	#endif
	
	#ifdef tanh
	#undef tanh
	#endif

	#ifdef atan
	#undef atan
	#endif

	#ifdef acos
	#undef acos
	#endif

	#ifdef asin
	#undef asin
	#endif

	#ifdef log10
	#undef log10
	#endif

	#ifdef log
	#undef log
	#endif

	#ifdef pow
	#undef pow
	#endif

	#ifdef exp
	#undef exp
	#endif

	#include <math.h>	
	
#endif

#if defined( I3_WINCE )
//#define fmodf			(REAL32)fmod
#define sqrtf			(REAL32)sqrt
#define sinf			(REAL32)sin
#define cosf			(REAL32)cos
#define tanf			(REAL32)tan
#define asinf			(REAL32)asin
#define acosf			(REAL32)acos
#define atanf			(REAL32)atan
#define atan2f			(REAL32)atan2
#endif

#define		USE_TABLE

namespace i3Math
{
	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 288.975 ms
	// Vfpu unUse: 146.330 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		abs( REAL32 val)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)  || defined( I3_WIPI) || defined( I3_WINCE )
		register UINT32 * pUint = (UINT32 *) & val;

		*pUint &= 0x7FFFFFFF;

		return val;
		#endif 
	}

	inline
	INT32		abs( INT32 val)
	{
		return val >= 0 ? val : -val;
	}

	// Encription용 Integer Add 함수
	// 속도는 매우 느리지만, Add 연산의 Code Pattern을 감추기 위한 용도로 사용할 수 있다.
	inline
	INT32 softAdd( INT32 a, INT32 b)
	{
		register INT32 i, mask = 1, borrow = 0;
		register INT32 v = 0;
		register INT32 bit;

		for( i = 0; i < 32; i++)
		{
			bit = (a & mask) ^ (b & mask);
			v |= bit ^ ((1 & borrow) << i);

			if( borrow)
				borrow = ((a & mask) | (b & mask)) >> i;
			else
				borrow = ((a & mask) & (b & mask)) >> i;

			mask = mask << 1;
		}

		return v;
	}


#if !defined( I3_WIPI_GIGA )
	inline
	FIXED32		abs( FIXED32 val)
	{
		return val >= 0 ? val : -val;
	}
#endif

	inline
	FIXED32	fmod( FIXED32 val, FIXED32 mod)
	{
		return i3FXD::Mod( val, mod);
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 471.799 ms
	// Vfpu unUse: 903.787 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		fmod( REAL32 val, REAL32 mod)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PS2)  || defined( I3_WIPI)
		return fmodf( val, mod);
		#elif defined( I3_WINCE )
		return (REAL32)::fmod( (double)val, (double)mod );
		#elif defined( I3_PSP)
			REAL32 rst;

			__asm__ (
					".set push\n"
					".set noreorder\n"
					"mfc1		$8,		%1\n"
					"mtv		$8,		s000\n"
					"mfc1		$8,		%2\n"
					"mtv		$8,		s001\n"
					"vrcp.s		s002,	s001\n"			// s002 = 1 / mod
					"vmul.s		s003,	s000, s002\n"	// s001 = s000(val) * s002
					"vf2iz.s	s003,	s003,	0\n"	// truncate to 0
					"vi2f.s		s003,	s003,	0\n"
					"vmul.s		s001,	s003,	s001\n"
					"vsub.s		s000,	s000,	s001\n"
					"mfv		$8,		s000\n"			// t0   = s000
					"mtc1		$8,		%0\n"			// d    = t0
					".set pop\n"
					: "=&f" (rst)
					: "f" (val), "f" (mod)
					: "$8"
					);

			return rst;
		#endif 
	}

	inline
	REAL32	Mul( REAL32 val1, REAL32 val2)
	{
		return val1 * val2;
	}

	inline
	FIXED32	Mul( FIXED32 val1, FIXED32 val2)
	{
		return i3FXD::Mul( val1, val2);
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 367.780 ms
	// Vfpu unUse: 255.610 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32	div( REAL32 val1, REAL32 val2)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX)  || defined( I3_WIPI) || defined( I3_WINCE )
			return val1 / val2;
		#elif defined( I3_PSP)
			REAL32 rst;
			__asm__ (
				".set		push\n"
				".set		noreorder\n"
				"mfc1		$8,		%1\n"
				"mfc1		$9,		%2\n"
				"mtv		$8,		s000\n"
				"mtv		$9,		s001\n"
				"vrcp.s		s002,	s001\n"
				"vmul.s		s000,	s000,	s002\n"
				"mfv		$8,		s000\n"
				"mtc1		$8,		%0\n"
				".set	pop\n"
				:	"=&f" (rst)
				:	"f" (val1),	"f" (val2)
				:	"$8", "$9"
				);
			return rst;
		#endif
	}

	inline
	FIXED32	div( FIXED32 val1, FIXED32 val2)
	{
		return i3FXD::Div( val1, val2);
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 334.862 ms
	// Vfpu unUse: 196.479 ms ~ 264.053 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		Min( REAL32 r1, REAL32 r2)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_WIPI) || defined( I3_PSP) || defined( I3_WINCE )
			return (r1 > r2) ? r2 : r1;
		#endif
	}

	inline
	FIXED32		Min( FIXED32 r1, FIXED32 r2)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_WIPI) || defined( I3_WINCE )
			return (r1 > r2) ? r2 : r1;
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 334.862 ms
	// Vfpu unUse: 196.479 ms ~ 264.053 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		Max( REAL32 r1, REAL32 r2)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_WIPI) || defined( I3_WINCE )
			return (r1 > r2) ? r1 : r2;
		/*#elif defined( I3_PSP)
			REAL32 rst;
			__asm__ (
				".set		push\n"
				".set		noreorder\n"
				"mfc1		$8,		%1\n"
				"mfc1		$9,		%2\n"
				"mtv		$8,		s000\n"
				"mtv		$9,		s001\n"
				"vmax.s		s000,	s000,	s001\n"
				"mfv		$8,		s000\n"
				"mtc1		$8,		%0\n"
				".set	pop\n"
				:	"=&f" (rst)
				:	"f" (r1),	"f" (r2)
				:	"$8", "$9"
				);
			return rst;*/
		#endif
	}

	inline
	INT32		Max( INT32 r1, INT32 r2)
	{
		return (r1 > r2) ? r1 : r2;
	}

	inline
	FIXED32		Max( FIXED32 r1, FIXED32 r2)
	{
		return (r1 > r2) ? r1 : r2;
	}

	inline
	BOOL		IsNegative( REAL32 val)
	{
		register UINT32 * pUint = (UINT32 *) & val;

		return (*pUint & 0x80000000);
	}

	inline
	BOOL		IsNegative( FIXED32 val)
	{
		register UINT32 * pUint = (UINT32 *) & val;

		return (*pUint & 0x80000000);
	}

	inline
	BOOL		IsSame( REAL32 val1, REAL32 val2)
	{
		return (i3Math::abs( val1 - val2) <= I3_EPS);
	}

	inline
	REAL32		log( REAL32 x)
	{
		return ::log( x);
	}

	inline
	REAL32		log( REAL32 x, REAL32 base)
	{
		return log(x) / log( base);
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 333.461 ms
	// Vfpu unUse: 580.919 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		sin( REAL32 theta)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = (INT32)(theta * 41721.51340188181089983337037398f) >> 9;	// (512.0f / I3_2PI)

				return g_SinTable[ idx & 0x1FF];
			#else
				return sinf( theta);
			#endif

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcst.s			s001, 0x05\n"			// s001 = 2/PI
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"vsin.s			s002, s000\n"			// s000 = sin(s000)
					"mfv			$8,  s002\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(theta)
					: "$8"
				);
			#else
				d = sinf(theta);
			#endif
				return (d);			
		#endif
	}

	inline
	FIXED32	sin( FIXED32 theta)
	{
		#if defined( USE_TABLE)
			INT32 idx = (i3FXD::Div( theta, 51472) >> 10);	// (511.0f / (I3_2PI * 65536))

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( sinf( i3FXD::ToReal( theta)));
		#endif
	}

	inline
	REAL32	sin_libm( REAL32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return sinf( rad);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_SinR( i3FXD::FromReal( rad));
			#else
				return sinf( rad);
			#endif
		#endif
	}

	inline
	FIXED32	sin_libm( FIXED32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( sinf( i3FXD::ToReal( rad)));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_SinR( rad);
			#else
				return i3FXD::FromReal( sinf( i3FXD::ToReal( rad)));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 306.741 ms
	// Vfpu unUse: 639.433 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32	vsin( REAL32 t)	
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = (INT32)( i3FXD::FromReal(t) >> 9);

				return g_SinTable[ idx & 0x1FF];
			#else
				return i3Math::sin( t * I3_PI2);
			#endif

		#elif defined( I3_PSP)
			float d;

			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vsin.s			s000, s000\n"			// s000 = sin(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(t)
					: "$8"
				);
			#else
				d = i3Math::sin( t * I3_PI2);
			#endif

			return (d);		
		#endif
	}

	inline
	REAL32		vsin_libm( REAL32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return sinf( t * I3_PI2);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return i3FXD::ToReal( OEMC_gMU_SinR( i3FXD::FromReal( t * I3_PI2)));
			#else
				return sinf( t * I3_PI2);
			#endif
		#endif
	}

	inline
	FIXED32		vsin( FIXED32 t)
	{
		#if defined( USE_TABLE)
			INT32 idx = t >> 9;

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( vsin( i3FXD::ToReal( t)));
		#endif
	}

	inline
	FIXED32		vsin_libm( FIXED32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( sinf( i3FXD::ToReal( i3FXD::Mul( t, 102944))));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_SinR( i3FXD::Mul( t, 102944));
			#else
				return i3FXD::FromReal( sinf( i3FXD::ToReal( i3FXD::Mul( t, 102944))));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 334.912 ms
	// Vfpu unUse: 654.291 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		cos( REAL32 theta)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = ((INT32)(theta * 41721.51340188181089983337037398f) >> 9) + 128;	// (512.0f / I3_2PI)

				return g_SinTable[ idx & 0x1FF];
			#else
				return cosf( theta);
			#endif

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcst.s			s001, 0x05\n"			// s001 = 2/PI
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"vcos.s			s000, s000\n"			// s000 = cos(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(theta)
					: "$8"
				);
			#else
				d = cosf(theta);
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32	cos( FIXED32 theta)
	{
		#if defined( USE_TABLE)
			INT32 idx = (i3FXD::Div( theta, 51472) >> 10) + 128;	// (511.0f / (I3_2PI * 65536))

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( cosf( i3FXD::ToReal( theta)));
		#endif
	}

	inline
	REAL32	cos_libm( REAL32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return cosf( rad);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return i3FXD::ToReal( OEMC_gMU_CosR( i3FXD::FromReal( rad)));
			#else
				return cosf( rad);
			#endif
		#endif
	}

	inline
	FIXED32	cos_libm( FIXED32 rad)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( cosf( i3FXD::ToReal( rad)));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_CosR( rad);
			#else
				return i3FXD::FromReal( cosf( i3FXD::ToReal( rad)));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 307.594 ms
	// Vfpu unUse: 710.092 ms
	/////////////////////////////////////////////////////////////////
	// sin() 함수와 유사한 기능을 하나,
	// Radian 값이 아닌, 0~90 Degree 값을 0~1로 표현한 값으로
	// 인자를 받아들인다.
	inline
	REAL32		vcos( REAL32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI)  || defined( I3_WINCE )
			#if defined( USE_TABLE)
				INT32 idx = (INT32)(t * 127.0f) + 128;

				return g_SinTable[ idx & 0x1FF];
			#else
				return cosf( t * I3_PI2);
			#endif

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcos.s			s000, s000\n"			// s000 = cos(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(t)
					: "$8"
				);
			#else
				d = cosf( t * (I3_PI * 0.5f));
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32 vcos( FIXED32 t)
	{
		#if defined( USE_TABLE)
			INT32 idx = (t >> 9) + 128;

			return g_FXDSinTable[ idx & 0x1FF];
		#else
			return i3FXD::FromReal( vcos( i3FXD::ToReal( t)));
		#endif
	}

	inline
	REAL32	vcos_libm( REAL32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return cosf( t * I3_PI2);
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return i3FXD::ToReal( OEMC_gMU_CosR( i3FXD::FromReal( t * I3_PI2)));
			#else
				return cosf( t * I3_PI2);
			#endif
		#endif
	}

	inline
	FIXED32	vcos_libm( FIXED32 t)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_PS2)
			return i3FXD::FromReal( cosf( i3FXD::ToReal( t) * I3_PI2));
		#elif defined( I3_WIPI) || defined( I3_WINCE )
			#if defined( I3_WIPI_GIGA)
				return OEMC_gMU_CosR( i3FXD::Mul( t, 102944));
			#else
				return i3FXD::FromReal( cosf( i3FXD::ToReal( t) * I3_PI2));
			#endif
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 416.699 ms
	// Vfpu unUse: 804.779 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		tan( REAL32 theta)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return tanf( theta);

		#elif defined( I3_PSP)
				float d;
			#if defined( VFPU_ASM_MATH)
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vcst.s			s001, 0x05\n"			// s001 = 2/PI
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"vrot.p			c002, s000, [c, s]\n"	// s002 = cos(s) , s003 = sin(s)
					"vdiv.s			s000, s003, s002\n"		// s000 = s003 / s002 = tan(s)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(theta)
					: "$8"
				);
			#else
				d = tanf(theta);
			#endif
				return (d);	
		#endif
	}
	/////////////////////////////////////////////////////////////////
	// 2005.6.20 by KOMET (Math에 없어서 추가함) 
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		atan2( REAL32 x, REAL32 y)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return atan2f( x, y);

		#elif defined( I3_PSP)
			return atan2f( x, y);

		#endif
	}
	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 348.903 ms
	// Vfpu unUse: 1081.037 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		acos( REAL32 radian)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return acosf( radian);

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vasin.s		s000, s000\n"			// s000 = asin(s000)
					"vcst.s			s001, 0x08\n"			// s001 = PI/2
					"vocp.s			s000, s000\n"			// s000 = 1 - s000 = 1 - asin(s) = acos
					"vmul.s			s000, s000, s001\n"		// s000 = s000 * s001
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(radian)
					: "$8"
				);
			#else
				d = acosf(radian);
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32		acos( FIXED32 radian)
	{
		return i3FXD::FromReal( acosf( i3FXD::ToReal(radian)));
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 305.899 ms
	// Vfpu unUse: 1862.623 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		Sqrt( REAL32 val)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return sqrtf( val);

		#elif defined( I3_PSP)
				float d;
			//#if defined( VFPU_ASM_MATH)
			#if 1
				__asm__ (
					".set			push\n"					// save assember option
					".set			noreorder\n"			// suppress reordering
					"mfc1			$8,  %1\n"				// t0   = s
					"mtv			$8,  s000\n"			// s000 = t0
					"vsqrt.s		s000, s000\n"			// s000 = sqrt(s000)
					"mfv			$8,  s000\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"					// restore assember option
					: "=f"(d)
					: "f"(val)
					: "$8"
				);
			#else
				d = sqrtf(val);
			#endif
				return (d);
		#endif
	}

	inline
	FIXED32		Sqrt( FIXED32 val)
	{
		return i3FXD::FromReal( sqrtf( i3FXD::ToReal( val)));
	}

	inline
	REAL32		Clamp( REAL32 val, REAL32 fmin, REAL32 fmax)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_WIPI) || defined( I3_WINCE )
			if( val < fmin)		return fmin;
			if( val > fmax)		return fmax;

			return val;
		#endif
	}

	inline
	FIXED32		Clamp( FIXED32 val, FIXED32 fmin, FIXED32 fmax)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_WIPI) || defined( I3_WINCE )
			if( val < fmin)		return fmin;
			if( val > fmax)		return fmax;

			return val;
		#endif
	}

	inline
	REAL32		Reciprocal( REAL32 val)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_WIPI) || defined( I3_WINCE )
			return 1.0f / val;
		#endif
	}

	inline
	FIXED32		Reciprocal( FIXED32 val)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_PSP) || defined( I3_WIPI) || defined( I3_WINCE )
		return i3FXD::Div( 65536, val);
		#endif
	}

	inline
	INT32		Rand(void)
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return rand();

		#elif defined( I3_PSP)
			return rand(); 

		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 265.928 ms
	// Vfpu unUse: 363.861 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		Randf(void) // return randomed 0.0 to 1.0
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI)  || defined( I3_WINCE )
			return 0.0002441f * (REAL32)(rand()&0x00000fff);// x/4095 = 

		#elif defined( I3_PSP)
			REAL32 rst;
			__asm__ (
					".set			push\n"
					".set			noreorder\n"
					"vone.s			s000\n"
					"vrndf1.s		s001\n"
					"vsub.s			s001, s001, s000\n"
					"mfv			$8,  s001\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"
					: "=&f"(rst)
					:
					: "$8"
				);
			return rst;
			//return 0.0002441f * (REAL32)(rand()&0x00000fff);
		#endif
	}

	/////////////////////////////////////////////////////////////////
	// 2005.3.2 Lene3 
	// Test Result (1000*1000 loop)
	// Vfpu Use: 309.938 ms
	// Vfpu unUse: 385.563 ms
	/////////////////////////////////////////////////////////////////
	inline
	REAL32		Randf2(void) // return randomed -1.0 to 1.0
	{
		#if defined( I3_WINDOWS) || defined( I3_XBOX) || defined( I3_WIPI) || defined( I3_WINCE )
			return 4.8851978505129457743038593063019e-4f * (REAL32)(rand()&0x00000fff) - 1.f;

		#elif defined( I3_PSP)
			REAL32 rst;
			__asm__ (
					".set			push\n"
					".set			noreorder\n"
					"vfim.s			s000, 3.0\n"		// s002 = 0.998047
					"vrndf2.s		s001\n"
					"vsub.s			s001, s001, s000\n"
					"mfv			$8,  s001\n"			// t0   = s000
					"mtc1			$8,  %0\n"				// d    = t0
					".set			pop\n"
					: "=&f"(rst)
					:
					: "$8"
				);
			return rst;
			//return 4.8851978505129457743038593063019e-4f * (REAL32)(rand()&0x00000fff) - 1.f;
		#endif
	}

	inline
	REAL32	Infinite(void)			{	return std::numeric_limits<float>::infinity(); }
};

#endif
