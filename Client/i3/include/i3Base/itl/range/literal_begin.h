#pragma once

// as_literal이 항상 강제되는것이 문제라서... literal_begin()과 literal_end()를 추가한다..
// 여러번 중첩활용되는 것이라면 as_literal/iterator_range을 쓰면 되고, 1회성이라면 iterator_range대신 이것들을 쓴다..

#include "begin.h"
#include "../utility/is_arg_c_str.h"

namespace i3
{
	namespace range_detail
	{
		template<class T> inline
		T* make_literal_begin( T* const r, arg_true_type) 
		{
			return r;			
		}

		template<class T> inline
		typename range_iterator<T>::type make_literal_begin( T& r, arg_false_type) 
		{
			return i3::begin( r );
		}	
	}
	
	template<class Range> inline 
	typename range_iterator<Range>::type literal_begin( Range& r)
	{
		return range_detail::make_literal_begin( r, i3::is_arg_c_str(r) );
	}

	template<class Range> inline 
	typename range_iterator<const Range>::type literal_begin( const Range& r)
	{
		return range_detail::make_literal_begin( r, i3::is_arg_c_str(r) );
	}

	template<class C, std::size_t sz> inline 
	typename range_iterator<C*>::type literal_begin( C (&arr)[sz] )
	{
		return range_detail::make_literal_begin( arr, i3::is_arg_c_str(arr) );
	}

	template<class C, std::size_t sz> inline 
	typename range_iterator<const C*>::type literal_begin(const C (&arr)[sz] )
	{
		return range_detail::make_literal_begin( arr, i3::is_arg_c_str(arr) );
	}
}
