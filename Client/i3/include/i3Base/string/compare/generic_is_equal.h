#pragma once

#include "../../itl/range/algorithm/equal.h"
#include "../../itl/range/as_literal.h"


namespace i3
{
	template<class Rng1, class Rng2> inline
	bool	generic_is_equal( const Rng1& lhs, const Rng2& rhs )
	{
		return i3::range::equal(as_literal(lhs), as_literal(rhs) );	
	}
}
