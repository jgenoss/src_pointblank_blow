#pragma once


#include "../../itl/algorithm/lexicographical_compare.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"
#include "is_less_filechar.h"

namespace i3
{
	struct generic_less_filepath
	{
		template<class Rng1, class Rng2> inline
		bool operator()(const Rng1& lhs, const Rng2& rhs) const
		{
			return i3::lexicographical_compare(literal_begin(lhs), literal_end(lhs),
				literal_begin(rhs), literal_end(rhs), is_less_filechar());
		}
	};

}