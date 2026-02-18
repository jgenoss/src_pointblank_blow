#pragma once

#include "../../itl/range/algorithm/equal.h"
#include "../../itl/range/as_literal.h"
#include "is_equal_filechar.h"

namespace i3
{
	struct generic_equal_filepath
	{
		template<class Rng1, class Rng2> inline
		bool operator()(const Rng1& lhs, const Rng2& rhs) const
		{
			return i3::range::equal(as_literal(lhs), as_literal(rhs), is_equal_filechar());
		}
	};

}
