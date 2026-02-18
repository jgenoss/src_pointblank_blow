#pragma once


#include "../../itl/range/iterator.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_last_not_of.h"
#include "../../itl/range/algorithm/rfind_not.h"


namespace i3
{
	template<class Range, class DelimitRange> inline
	typename i3::range_iterator<const Range>::type generic_find_last_not_of(const Range& input, const DelimitRange& delimit)
	{
		return range::find_last_not_of(i3::as_literal(input), i3::as_literal(delimit));	
	}

	template<class Range> inline
	typename i3::range_iterator<const Range>::type generic_find_last_not_of(const Range& input, typename i3::range_value<Range>::type delimit_c)
	{
		return range::rfind_not(i3::as_literal(input), delimit_c);
	}

	template<class InIt, class DelimitRange> inline
	typename InIt generic_find_last_not_of( InIt beg, InIt end, const DelimitRange& delimit)
	{
		return range::find_last_not_of(i3::iterator_range<InIt>(beg, end), i3::as_literal(delimit));
	}

	template<class InIt> inline
	typename InIt generic_find_last_not_of( InIt beg, InIt end, typename i3::iterator_value<InIt>::type delimit_c)
	{
		return i3::rfind_not(beg, end, delimit_c);
	}

}
