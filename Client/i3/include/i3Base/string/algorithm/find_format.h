#pragma once

#include "detail/find_format_impl.h"

namespace i3
{
	
	//
	// 부스트 스트링 알고리즘 인터페이스인데...함수명칭과 인수명칭이 너무 안좋다...직접 쓰지 말것..
	// input원본 문자열에서, finder 구간만큼이 카피되는 함수이다..
	// 인수로 들어온 finder는 여기서 모두 실행되고, 반복자구간을 detail함수로 넘김..
	//

	template<class OutIt, class Range, class Finder, class Formatter> inline 
	OutIt find_format_copy(OutIt out, const Range& input, Finder finder, Formatter formatter)
	{
		iterator_range< typename range_const_iterator<Range>::type > lit_input(as_literal(input));

		return detail::find_format_copy_impl(out, lit_input, formatter, finder( begin(lit_input), end(lit_input) ) );
	}
	
	template<class Seq, class Finder, class Formatter> inline 
	Seq	find_format_copy(const Seq& input, Finder finder, Formatter formatter)
	{
		return detail::find_format_copy_impl(input, formatter, finder( begin(input), end(input) ) );
	}
	
	// 이 버전이 가독성이 제일 높다..
	template<class Seq, class Finder, class Formatter> inline
	void find_format(Seq& inout, Finder finder, Formatter formatter)
	{
		detail::find_format_impl(inout, formatter, finder(begin(inout), end(inout) ) );
	}
	
	
	
}