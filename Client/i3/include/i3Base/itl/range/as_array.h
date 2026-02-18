#pragma once

#include "iterator_range.h"

namespace i3
{
	template< class Range> inline
	iterator_range< typename range_iterator<Range>::type > as_array( Range& r)
	{
		return i3::make_iterator_range(r);
	}
	
	template< class Range> inline
	iterator_range< typename range_iterator<const Range>::type > as_array(const Range& r)
	{
		return i3::make_iterator_range(r);
	}

}