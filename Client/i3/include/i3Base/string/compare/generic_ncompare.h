#pragma once

#include "../../itl/range/algorithm/compare.h"
#include "../../itl/range/as_literal.h"

namespace i3
{
	// 함수의 형태를 띠어야 한다..
	template<class Rng1, class Rng2> inline
	int	generic_ncompare( const Rng1& lhs, const Rng2& rhs, std::size_t n )
	{
		typedef typename i3::get_iterator_range_type<const Rng1>::type	range1_type;
		typedef typename i3::get_iterator_range_type<const Rng2>::type	range2_type;
		typedef typename range1_type::difference_type					diff_t;


		range1_type r1 = as_literal(lhs);
		range2_type r2 = as_literal(rhs);
		if (diff_t(n) < r1.size() ) r1.advance_end( -diff_t(r1.size() - n) );
		if (diff_t(n) < r2.size() ) r2.advance_end( -diff_t(r2.size() - n) );
		return i3::range::compare(r1, r2);	
	}
}
