#pragma once

#include "first_finder.h"
#include "last_finder.h"		//이 헤더의 함수들은 분리하는게 좋겠다.

namespace i3
{
	// 대상 스트링에 대해 finder를 실행해서 결과범위를 리턴..
	// 기본 함수이기 때문에 실제로 쓰일일은 없어보인다..
	template<class Range, class Finder> inline
	iterator_range<typename range_iterator<Range>::type>
	find( Range& input, const Finder& finder)
	{
		iterator_range<typename range_iterator<Range>::type> lit_input(as_literal(input));
		return finder(begin(lit_input), end(lit_input));
	}
	
	template<class Range1, class Range2> inline
	iterator_range<typename range_iterator<Range1>::type>
	find_first(Range1& input, const Range2& search)
	{
		return find(input, first_finder(search));
	}
	
	template<class Range1, class Range2> inline
	iterator_range<typename range_iterator<Range1>::type>
	ifind_first(Range1& input, const Range2& search)
	{
		return find(input, first_finder(search, is_iequal()));
	}
	
	template<class Range1, class Range2> inline
	iterator_range<typename range_iterator<Range1>::type>
	find_last(Range1& input, const Range2& search)
	{
		return find(input, last_finder(search));
	}

	template<class Range1, class Range2> inline
	iterator_range<typename range_iterator<Range1>::type>
	ifind_last(Range1& input, const Range2& search)
	{
		return find(input, last_finder(search, is_iequal()));
	}

}
