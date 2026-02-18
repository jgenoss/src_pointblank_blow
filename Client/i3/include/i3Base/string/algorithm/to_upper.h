#pragma once


// 영문자만 고려한 것..  일반적으로 toupper/tolower 표준함수가 퍼포먼스문제가 좀 있다고 한다..

#include "../../itl/range/as_literal.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/algorithm/transform.h"

#include "../../itl/iterator/transform_iterator.h"

#include "detail/case_conv_fast_functor.h"

namespace i3
{
	//		
	template<class OutIt, class Range> inline 
	typename iterator_return<OutIt>::type to_upper_copy(OutIt Output, const Range& Input)
	{
		typename iterator_return<OutIt>::type it = range::transform( i3::as_literal(Input), Output, to_upper_functor_fast<typename range_value<Range>::type>());
		*it = 0;			// 널값처리 필요..
		return it;
	}

	template<class Seq, class Range> inline 
	typename non_iterator_return<Seq>::type to_upper_copy(Seq& Output, const Range& Input)
	{
		return range::transform( i3::as_literal(Input), Output, to_upper_functor_fast<typename range_value<Range>::type>());
	}


	template<class Seq> inline
	Seq to_upper_copy( const Seq& Input)
	{
		to_upper_functor_fast<typename range_value<Seq>::type> fn();
		return Seq( i3::make_transform_iterator( begin(Input), fn), 
			i3::make_transform_iterator( end(Input), fn) );
	}

	template<class InOutRange> inline
	void to_upper(InOutRange& InOut)
	{
		range::inplace_transform(i3::as_literal(InOut), to_upper_functor_fast<typename range_value<InOutRange>::type>() );
	}

}
