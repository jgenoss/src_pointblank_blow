#pragma once

#include "../../itl/range/algorithm/find.h"

namespace i3
{
	namespace detail
	{
	
		template<class Rng> inline
		int contain_char_imp( const Rng& rng, typename range_value<Rng>::type ch )
		{
			typedef typename get_iterator_range_type<const Rng>::type range_type;
			typedef typename range_iterator<const Rng>::type range_iterator;

			range_iterator it_res = i3::range::find(rng, ch);

			if (it_res == rng.end() ) 
				return -1;

			return i3::distance(rng.begin(), it_res);
		}
	}

	template<class Rng> inline
	int contain_char( const Rng& rng, typename range_value<Rng>::type ch )
	{
		return detail::contain_char_imp(i3::as_literal(rng), ch);
	}
		
	
}
