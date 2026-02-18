#pragma once

//
// 구 i3String안에 함수가 있어서 호환성을 위해 추가할 필요가 있음..
// 
#include "../../itl/range/iterator.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_first_not_of.h"
#include "../../itl/range/algorithm/find_not.h"


namespace i3
{
	template<class Range, class DelimitRange> inline
	typename i3::range_iterator<const Range>::type generic_find_first_not_of(const Range& input, const DelimitRange& delimit)
	{
		return range::find_first_not_of(i3::as_literal(input), i3::as_literal(delimit));	
	}
	
	template<class Range> inline
	typename i3::range_iterator<const Range>::type generic_find_first_not_of(const Range& input, typename i3::range_value<Range>::type delimit_c)
	{
		return range::find_not(i3::as_literal(input), delimit_c);
	}

	template<class InIt, class DelimitRange> inline
	typename InIt generic_find_first_not_of( InIt beg, InIt end, const DelimitRange& delimit)
	{
		return range::find_first_not_of(i3::iterator_range<InIt>(beg, end), i3::as_literal(delimit));
	}
		
	template<class InIt> inline
	typename InIt generic_find_first_not_of( InIt beg, InIt end, typename i3::iterator_value<InIt>::type delimit_c)
	{
		return i3::find_not(beg, end, delimit_c);
	}

}

// 아래는 구버전..  인터페이스 이름이 너무 일반적이라서 구현과 네이밍을 교체했다..

// #include "detail/strtok_delimit_functor.h"
// #include "../../itl/algorithm/find_if.h"
/*
	template<class Range, class DelimitRange> inline
	typename i3::range_iterator<const Range>::type find_first_not_of( 
		const Range& input, const DelimitRange& delimit)
	{
		i3::iterator_range<typename i3::range_iterator<const Range>::type> input_range = i3::as_literal(input);
		return i3::find_if(i3::begin(input_range), i3::end(input_range), 
			i3::not1( detail::make_strtok_delimit_functor( i3::as_literal(delimit) ) ) );
	}
	
	template<class Range> inline
	typename i3::range_iterator<const Range>::type find_first_not_of( 
		const Range& input, typename i3::range_value<Range>::type delimit_c)
	{
		i3::iterator_range<typename i3::range_iterator<const Range>::type> input_range = i3::as_literal(input);
		return i3::find_if(i3::begin(input_range), i3::end(input_range), 
			i3::not1( detail::make_strtok_delimit_c_functor( delimit_c) ) );
	}
	
	/////////////// 

	template<class InIt, class DelimitRange> inline
	typename InIt find_first_not_of( 
		InIt beg, InIt end, const DelimitRange& delimit)
	{
		return i3::find_if(beg, end, i3::not1( detail::make_strtok_delimit_functor( i3::as_literal(delimit) ) ) );
	}
	
	template<class InIt> inline
	typename InIt find_first_not_of( 
		InIt beg, InIt end, typename i3::iterator_value<InIt>::type delimit_c)
	{
		return i3::find_if(beg, end, i3::not1( detail::make_strtok_delimit_c_functor( delimit_c) ) );
	}
*/
