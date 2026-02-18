#pragma once

//
// i3String::Compare의 일반화..
//

#include "../../itl/range/algorithm/compare.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/value_type.h"
#include "../algorithm/detail/case_conv_fast_functor.h"

namespace i3
{
	template<class Rng1, class Rng2> inline
	int	generic_icompare( const Rng1& lhs, const Rng2& rhs )
	{
		typedef typename i3::range_value<const Rng1>::type Ch;
		return i3::range::compare(as_literal(lhs), as_literal(rhs), to_lower_functor_fast<Ch>() );	
	}
}
