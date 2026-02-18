#pragma once

#include "../../itl/range/algorithm//lexicographical_compare.h"
#include "../../itl/range/as_n_literal.h"
#include "is_less_filechar.h"

namespace i3
{

	template<class Rng1, class Rng2> inline
	bool generic_is_nless_filepath(const Rng1& lhs, const Rng2& rhs, std::size_t n)
	{
		return i3::range::lexicographical_compare(
			i3::as_n_literal(lhs, n), 
			i3::as_n_literal(rhs, n), 
			is_less_filechar());
	}

}
