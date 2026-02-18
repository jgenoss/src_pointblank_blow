#pragma once

//
// CRT 래핑 제거하고....공개소스에서 짜집기해 적절이 수정함...(atof를 키워드로 구글검색하면 꽤 나온다..)
//

#include "../../itl/range/value_type.h"
#include "../../itl/range/iterator.h"
#include "../../itl/range/as_literal.h"

#include <limits>

namespace i3
{
	namespace detail
	{
		template<class Flt, class Range>
		Flt atof_impl(const Range& rng)
		{
			if ( rng.empty() ) return Flt();

			typedef typename i3::range_iterator<Range>::type	It;
			typedef typename i3::range_value<Range>::type		Ch;

			It it_beg = rng.begin();	
			It it_end = rng.end();

			// 1. 빈공간 스킵
			for (  ; it_beg != it_end ; ++it_beg )  
			{
				if (*it_beg != Ch(' ') && *it_beg != Ch('\t') )
					break;
			}		
			if ( it_beg == it_end  ) return Flt();
		
			// 2. 부호 판별.
			Flt neg = Flt(1.0);
			if ( *it_beg == Ch('-') ) { neg = Flt(-1.0); ++it_beg; }
			else
			if ( *it_beg == Ch('+') ) { ++it_beg; }
			
			if ( it_beg == it_end) return Flt();

			// 3. 소수점 위자리.

			Flt result = Flt();

			for ( ; it_beg != it_end ; ++it_beg ) 
			{
				if ( *it_beg < Ch('0') || *it_beg > Ch('9') )
					break;

				result = (result * Flt(10.0)) + Flt(*it_beg - Ch('0'));
			}
			
			if ( it_beg == it_end ) return result * neg;
			
			// 4. 소수점 아래 자리.
			if ( *it_beg == Ch('.') )		
			{
				++it_beg;
				if ( it_beg == it_end ) return result * neg;

				Flt scale = Flt(0.1);
				
				for ( ; it_beg != it_end ; ++it_beg )
				{
					if ( *it_beg < Ch('0') || *it_beg > Ch('9') )
						break;

					result += Flt(*it_beg - Ch('0')) * scale;
					scale *= Flt(0.1);
				}
				
				if ( it_beg == it_end ) return result*neg;
			}
			
			// 5. 지수 계산
			if (*it_beg == Ch('e') || *it_beg == Ch('E') || *it_beg == Ch('d') || *it_beg == Ch('D'))
			{
				++it_beg;
				if ( it_beg == it_end ) return result * neg;
				
				bool negE = false;

				if (*it_beg == Ch('-') ) { negE = true; ++it_beg; }
				else
				if (*it_beg == Ch('+') ) { ++it_beg; }

				if ( it_beg == it_end ) return result * neg;
				
				int e = 0;
				
				for ( ; it_beg != it_end ; ++it_beg )
				{
					if ( *it_beg < Ch('0') || *it_beg > Ch('9') )
						break;
					e = ( e*10 ) + (*it_beg - Ch('0'));
				}
				
				if (negE) e = -e;
				
				// 타입 범위 점검..
				if (e > std::numeric_limits<Flt>::max_exponent10 )
					e = std::numeric_limits<Flt>::max_exponent10;
				else
				if (e < std::numeric_limits<Flt>::min_exponent10 )
					e = std::numeric_limits<Flt>::min_exponent10;
				
				result *= ::pow(Flt(10.0), e);
			}
			
			return result * neg;
		}

	}

	template<class Range> __forceinline
	double atod(const Range& rng)
	{
		return detail::atof_impl<double>(i3::as_literal(rng) );
	}

	template<class Range> __forceinline
	float  atof(const Range& rng)
	{
		return detail::atof_impl<float>(i3::as_literal(rng) );
	}

	
}