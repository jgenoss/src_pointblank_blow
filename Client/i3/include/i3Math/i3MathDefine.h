#if !defined( __I3_MATH_DEFINE_H)
#define __I3_MATH_DEFINE_H

#include "i3MathGlobalVariable.h"
#include "limits"

#define		USE_TABLE

namespace i3Math
{
	inline
	REAL32		abs( REAL32 val)
	{
		register UINT32 * pUint = (UINT32 *) & val;

		*pUint &= 0x7FFFFFFF;

		return val;
	}

	inline
	INT32		abs( INT32 val)
	{
		return val >= 0 ? val : -val;
	}

	inline
	REAL32 relativeEPS( REAL32 a, REAL32 epsilon) 
	{
		return max( i3Math::abs( a * epsilon), epsilon);
	}

	inline
	bool isAlike( REAL32 a, REAL32 b, REAL32 epsilon = I3_EPS_REAL32) 
	{
		if(a == b) 
			return true;

		REAL32 relEps = relativeEPS( a, epsilon);
		return (a-relEps <= b) && (b <= a+relEps);
	}

	I3_EXPORT_MATH
	bool isAlike_WithAbsRelEPS(REAL32 a, REAL32 b, REAL32 absEPS, REAL32 relEPS);

	I3_EXPORT_MATH
	bool isAlike_WithAbsRelEPS(REAL32 a, REAL32 b, REAL32 abs_rel_EPS);

	I3_EXPORT_MATH
	bool isAlike_WithAbsEPSULP(REAL32 a, REAL32 b, REAL32 absEPS, int unitsInLastPlace);





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

	inline
	REAL32		fmod( REAL32 val, REAL32 mod)
	{
		return fmodf( val, mod);
	}

	inline
	REAL32	Mul( REAL32 val1, REAL32 val2)
	{
		return val1 * val2;
	}

	inline
	REAL32	div( REAL32 val1, REAL32 val2)
	{
		I3ASSERT( val2);

		return val1 / val2;
	}

	inline
	REAL32		Min( REAL32 r1, REAL32 r2)
	{
		return (r1 > r2) ? r2 : r1;
	}

	inline
	REAL32		Max( REAL32 r1, REAL32 r2)
	{
		return (r1 > r2) ? r1 : r2;
	}

	inline
	INT32		Max( INT32 r1, INT32 r2)
	{
		return (r1 > r2) ? r1 : r2;
	}

	inline
	bool		IsNegative( REAL32 val)
	{
		register UINT32 * pUint = (UINT32 *) & val;

		return (*pUint & 0x80000000) != 0;
	}

	inline
	bool		IsSame( REAL32 val1, REAL32 val2)
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
		if( val < fmin)		return fmin;
		if( val > fmax)		return fmax;

		return val;
	}

	inline
	REAL32		Reciprocal( REAL32 val)
	{
		return 1.0f / val;
	}

	inline
	INT32		Rand(void)
	{
		return rand();
	}

	inline
	REAL32		Randf(void) // return randomed 0.0 to 1.0
	{
		return (1.0f/4095.0f) * (REAL32)(rand() & 0x00000fff);// x/4095 = 
	}

	inline
	REAL32		Randf2(void) // return randomed -1.0 to 1.0
	{
		return 4.8851978505129457743038593063019e-4f * (REAL32)(rand()&0x00000fff) - 1.f;
	}

	inline
	REAL32	Infinite(void)			{	return std::numeric_limits<float>::infinity(); }

	inline
	REAL32		sin( REAL32 theta)
	{
		#if defined( USE_TABLE)
			INT32 idx = (INT32)(theta * 41721.51340188181089983337037398f) >> 9;	// (512.0f / I3_2PI)

			return g_SinTable[ idx & 0x1FF];
		#else
			return sinf( theta);
		#endif
	}

	inline
	REAL32	sin_libm( REAL32 rad)
	{
		return sinf( rad);
	}

	inline
	REAL32	vsin( REAL32 t)	
	{
		#if defined( USE_TABLE)
			INT32 idx = (INT32)( i3FXD::FromReal(t) >> 9);

			return g_SinTable[ idx & 0x1FF];
		#else
			return i3Math::sin( t * I3_PI2);
		#endif
	}

	inline
	REAL32		vsin_libm( REAL32 t)
	{
		return sinf( t * I3_PI2);
	}

	inline
	REAL32		cos( REAL32 theta)
	{
		#if defined( USE_TABLE)
			INT32 idx = ((INT32)(theta * 41721.51340188181089983337037398f) >> 9) + 128;	// (512.0f / I3_2PI)

			return g_SinTable[ idx & 0x1FF];
		#else
			return cosf( theta);
		#endif
	}

	inline
	REAL32	cos_libm( REAL32 rad)
	{
		return cosf( rad);
	}

	/////////////////////////////////////////////////////////////////
	// sin() 함수와 유사한 기능을 하나,
	// Radian 값이 아닌, 0~90 Degree 값을 0~1로 표현한 값으로
	// 인자를 받아들인다.
	inline
	REAL32		vcos( REAL32 t)
	{
		#if defined( USE_TABLE)
			INT32 idx = (INT32)(t * 127.0f) + 128;

			return g_SinTable[ idx & 0x1FF];
		#else
			return cosf( t * I3_PI2);
		#endif
	}

	inline
	REAL32	vcos_libm( REAL32 t)
	{
		return cosf( t * I3_PI2);
	}

	inline
	REAL32		tan( REAL32 theta)
	{
		return tanf( theta);
	}
	
	inline
	REAL32		atan2( REAL32 x, REAL32 y)
	{
		return atan2f( x, y);
	}

	inline
	REAL32		acos( REAL32 radian)
	{
		return acosf( radian);
	}

	inline
	REAL32		Sqrt( REAL32 val)
	{
		return sqrtf( val);
	}

	inline REAL32	NormalSquare( REAL32 val)
	{
		return val * val;
	}

};

#endif
