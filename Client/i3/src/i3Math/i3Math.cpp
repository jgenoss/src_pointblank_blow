#include "i3MathType.h"
#include "i3Vector.h"
#include "i3Math.h"

//
// 실수 상등 함수 3개를 추가함...  IsAlike도 나쁘진 않은듯하나 ULP버전도 없고 해서...
//

// 출처 : https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

// 검색해보니..가장 쉽게 찾아진거..

namespace i3Math
{
	//
	// relEPS는 유효숫자 수준에서 처리할 EPS이고, absEPS보다 더 작은 값이 오는게 보통이라고 함...
	//

	bool isAlike_WithAbsRelEPS(REAL32 a, REAL32 b, REAL32 absEPS, REAL32 relEPS)
	{
		float diff = fabs(a - b);
		if (diff <= absEPS)
			return true;

		a = fabs(a);	b = fabs(b);
		float largest = (b > a) ? b : a;
		return (diff <= largest * relEPS);
	}


	bool isAlike_WithAbsRelEPS(REAL32 a, REAL32 b, REAL32 abs_rel_EPS)
	{
		return isAlike_WithAbsRelEPS(a, b, abs_rel_EPS, abs_rel_EPS);
	}

	namespace
	{
		union Float_t
		{
			Float_t(float num = 0.0f) : f(num) {}
			// Portable extraction of components.
			bool Negative() const { return i < 0; }
			int32_t RawMantissa() const { return i & ((1 << 23) - 1); }
			int32_t RawExponent() const { return (i >> 23) & 0xFF; }

			int32_t i;
			float f;
#ifdef _DEBUG
			struct
			{   // Bitfields for exploration. Do not use in production code.
				uint32_t mantissa : 23;
				uint32_t exponent : 8;
				uint32_t sign : 1;
			} parts;
#endif
		};
	}

	// ULP는 실수값을 메모리상 정수값으로 볼때의 간격을 의미하고, 1~4 정도로 보통 쓴다고 함..

	bool isAlike_WithAbsEPSULP(REAL32 a, REAL32 b, REAL32 absEPS, int unitsInLastPlace)
	{
		// Check if the numbers are really close -- needed
		// when comparing numbers near zero.
		float absDiff = fabs(a - b);
		if (absDiff <= absEPS)
			return true;

		Float_t uA(a);
		Float_t uB(b);

		// Different signs means they do not match.
		if (uA.Negative() != uB.Negative())
			return false;

		// Find the difference in ULPs.
		int ulpsDiff = abs(uA.i - uB.i);
		return (ulpsDiff <= unitsInLastPlace);
	}
	
}