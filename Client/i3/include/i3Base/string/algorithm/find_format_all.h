#pragma once

#include "detail/find_format_all_impl.h"

namespace i3
{
	
	template<class OutIt, class Range, class Finder, class Formatter> inline
	OutIt find_format_all_copy(OutIt out, const Range& input, Finder finder, Formatter formatter)
	{
		iterator_range<typename range_const_iterator<Range>::type> lit_input(as_literal(input));
		
		return detail::find_format_all_copy_impl(out, lit_input, finder, formatter, finder(begin(lit_input), end(lit_input) ));
	}
	
	template<class Seq, class Finder, class Formatter> inline
	Seq	find_format_all_copy(const Seq& input, Finder finder, Formatter formatter)
	{
		return detail::find_format_all_copy_impl(input, finder, formatter, finder(begin(input), end(input)));
	}
	
	template<class Seq, class Finder, class Formatter> inline
	void find_format_all(Seq& inout, Finder finder, Formatter formatter)
	{
		return detail::find_format_all_impl(inout, finder, formatter, finder(begin(inout), end(inout) ));
	}
	
	
}