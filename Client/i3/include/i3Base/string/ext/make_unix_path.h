#pragma once

// MakeUnixPath의 일반화..
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/replace.h"
#include "../../itl/range/value_type.h"

namespace i3
{

	template<class Range> inline typename i3::disable_if_iterator<Range>::type
	make_unix_path(Range& path)
	{
		typedef typename i3::range_value<Range>::type		value_type;
		range::replace(path, value_type('\\'), value_type('/'));
	}
	
	template<class Lit> inline typename i3::enable_if_iterator<Lit>::type
	make_unix_path(Lit path)
	{
		typedef typename i3::iterator_value<Lit>::type		value_type;
		range::replace(path, value_type('\\'), value_type('/'));
	}	
	
}