#pragma once

#include <locale>
#include "../../itl/range/as_literal.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/algorithm/transform.h"

#include "../../itl/iterator/transform_iterator.h"

namespace i3
{
	//
	// 표준 tolower/toupper함수는 영문이외의 글자처리때문에 퍼포먼스가 다소 떨어지는 것 유의..
	// 따라서, 영문만 처리할 것이라면, 간단한 함수를 따로 마련하는게 훨씬 좋을 것임..
	//
	namespace detail
	{
	
		template<class C>
		struct to_upper_locale_functor : i3::unary_function<C, C>
		{
			to_upper_locale_functor(const std::locale& loc) : loc(loc) {} const std::locale& loc;
			C operator()(C ch) const { return std::toupper<C>(ch, loc); }
		};
		
	}

//		
	template<class OutIt, class Range> inline 
	typename iterator_return<OutIt>::type to_upper_locale_copy(OutIt Output, const Range& Input, const std::locale& Loc = std::locale())
	{
		typename iterator_return<OutIt>::type it = range::transform( i3::as_literal(Input), Output, detail::to_upper_locale_functor<typename range_value<Range>::type>(Loc));
		*it = 0;		// 널값처리 필요..
		return it;
	}

	template<class Seq, class Range> inline 
	typename non_iterator_return<Seq>::type to_upper_locale_copy(Seq& Output, const Range& Input, const std::locale& Loc = std::locale())
	{
		return range::transform( i3::as_literal(Input), Output, detail::to_upper_locale_functor<typename range_value<Range>::type>(Loc));
	}


	template<class Seq> inline
	Seq to_upper_locale_copy( const Seq& Input, const std::locale& Loc = std::locale())
	{
		detail::to_upper_locale_functor<typename range_value<Seq>::type> fn(Loc);
		return Seq( i3::make_transform_iterator( begin(Input), fn), 
			i3::make_transform_iterator( end(Input), fn) );
	}

	template<class InOutRange> inline
	void to_upper_locale(InOutRange& InOut, const std::locale& Loc = std::locale())
	{
		range::inplace_transform(i3::as_literal(InOut), detail::to_upper_locale_functor<typename range_value<InOutRange>::type>(Loc) );
	}


}
