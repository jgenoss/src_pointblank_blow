#pragma once



#include "../../itl/range/algorithm/compare.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/value_type.h"
#include "is_less_filechar.h"

namespace i3
{
	template<class C>
	struct to_lower_filepath_functor_fast : i3::unary_function<C, C>
	{
		enum { ALPHABET_INTERVAL = 'a' - 'A', };

		inline C operator()(C ch) const
		{
			return (ch < C('A') || ch > C('Z')) 
				? ( (ch == C('\\') ) 
					? C('/')
					: ch )
				: C(ch + ALPHABET_INTERVAL);
		}
	};
	
	template<class Rng1, class Rng2> inline
	int	generic_compare_filepath(const Rng1& lhs, const Rng2& rhs)
	{
		typedef typename i3::range_value<const Rng1>::type Ch;
		return i3::range::compare(as_literal(lhs), as_literal(rhs), to_lower_filepath_functor_fast<Ch>() );
	}
}

