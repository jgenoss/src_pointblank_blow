#pragma once

#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/rfind.h"

namespace i3
{
	namespace detail
	{
		template<class Range> inline
		typename i3::range_iterator<const Range>::type find_fileext_iterator_imp(const Range& path)
		{
			typedef typename range_iterator<const Range>::type	iterator;
			typedef typename range_value<const Range>::type		Ch;

			iterator it = i3::range::rfind(path, Ch('.') );
			return it == path.end() ? it : ++it;			
		}
	}

	template<class Range> inline
	typename i3::range_iterator<const Range>::type find_fileext_iterator(const Range& path)
	{
		return detail::find_fileext_iterator_imp(i3::as_literal(path) );
	}
}
