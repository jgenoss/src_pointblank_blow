#pragma once

#include "../ext/is_white_space.h"
#include "../../itl/algorithm/find_if.h"
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"

namespace i3
{
	namespace detail
	{
		struct find_non_whitespace_fn
		{
			inline bool operator()(int c) const { return !i3::is_white_space(c); }
		};
		
		struct find_whitespace_fn
		{
			inline bool operator()(int c) const { return i3::is_white_space(c); }
		};

	}

	template<class Rng> inline
	typename i3::range_iterator<const Rng>::type	find_non_whitespace(const Rng& rng)
	{
		return i3::find_if(	i3::literal_begin(rng), i3::literal_end(rng), detail::find_non_whitespace_fn());
	}
		
	template<class It>
	It	find_non_whitespace(It beg, It end)
	{
		return i3::find_if(	beg, end, detail::find_non_whitespace_fn());
	}
	
	template<class It>
	It find_whitespace(It beg, It end)
	{
		return i3::find_if(	beg, end, detail::find_whitespace_fn());
	}

}