#pragma once

#include "find_fileext_iterator.h"

namespace i3
{
	namespace detail
	{
		template<class Range> inline
		i3::iterator_range< typename i3::range_iterator<const Range>::type > 
		extract_fileext_iterator_range_imp(const Range& path)
		{
			return i3::iterator_range<typename i3::range_iterator<const Range>::type>( find_fileext_iterator_imp(path), path.end() );
		}
	}
	
	template<class Range> inline
	i3::iterator_range< typename i3::range_iterator<const Range>::type > 
	extract_fileext_iterator_range(const Range& path)
	{
		return detail::extract_fileext_iterator_range_imp(i3::as_literal(path));
	}
}
