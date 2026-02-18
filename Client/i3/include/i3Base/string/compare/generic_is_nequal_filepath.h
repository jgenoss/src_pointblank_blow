#pragma once


#include "../../itl/range/algorithm/equal.h"
#include "../../itl/range/as_n_literal.h"
#include "is_equal_filechar.h"

namespace i3
{
	// 함수의 형태를 띠어야 한다..
	template<class Rng1, class Rng2> inline
	bool	generic_is_nequal_filepath(const Rng1& lhs, const Rng2& rhs, std::size_t n)
	{
		return i3::range::equal(i3::as_n_literal(lhs, n), 
								i3::as_n_literal(rhs, n), is_equal_filechar());
	}
}
