#if !defined( __I3_MATH_DEFINE_H)
#define __I3_MATH_DEFINE_H

//#include "i3MathType.h"
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

	inline
	REAL32 relativeEPS( REAL32 a, REAL32 epsilon) 
	{
		REAL32 r32EpsilonABS = i3Math::abs( a * epsilon );
		return (r32EpsilonABS>epsilon? r32EpsilonABS:epsilon);
	}

	inline
	bool isAlike( REAL32 a, REAL32 b, REAL32 epsilon = I3_EPS_REAL32) 
	{
		if(a == b) 
			return true;

		REAL32 relEps = relativeEPS( a, epsilon);
		return (a-relEps <= b) && (b <= a+relEps);
	}

#ifdef I3_CHECK_FLOAT_VALUE_FOR_PHYSICX
	inline
	bool isValid( REAL32 val)
	{
		bool valid = (0 == ((_FPCLASS_SNAN | _FPCLASS_QNAN | _FPCLASS_NINF | _FPCLASS_PINF) & _fpclass(val) ));

		if( *((UINT32 *) &val) == 0xCDCDCDCD)
			return false;

		if( *((UINT32 *) &val) == 0xCCCCCCCC)
			return false;

		if( *((UINT32 *) &val) == 0xfeeefeee)
			return false;

		if( *((UINT32 *) &val) == 0xFDFDFDFD)
			return false;

		if( *((UINT32 *) &val) == 0xDDDDDDDD)
			return false;

		return valid;
	}
#endif

	I3_EXPORT_MATH INT32	GetNearestPowerOf2( INT32 val);

	// 입력된 값을 -PI ~ +PI 사의의 값으로 바꾸어 준다.
	inline
	REAL32 NormalizeAngle( REAL32 rRadian )
	{
		if( rRadian <= -I3_PI )
		{
			while( rRadian <= -I3_PI )
			{
				rRadian += I3_2PI;
			}
		}
		else if( rRadian > I3_PI )
		{
			while( rRadian > I3_PI )
			{
				rRadian -= I3_2PI;
			}
		}

		return rRadian;
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
		I3ASSERT( val2);

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
