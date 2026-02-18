#pragma once


#include "../../itl/range/algorithm/compare.h"
#include "../../itl/range/as_n_literal.h"
#include "generic_compare_filepath.h"

namespace i3
{
	// 함수의 형태를 띠어야 한다..
	template<class Rng1, class Rng2> inline
	int	generic_ncompare_filepath(const Rng1& lhs, const Rng2& rhs, std::size_t n)
	{
		typedef typename i3::range_value<const Rng1>::type Ch;
	
		return i3::range::compare(as_n_literal(lhs, n),
								  as_n_literal(rhs, n),
								to_lower_filepath_functor_fast<Ch>());
	}
}
