#pragma once

// as_literal이 항상 강제되는것이 문제라서... literal_begin()과 literal_end()를 추가한다..
// 여러번 중첩활용되는 것이라면 as_literal/iterator_range을 쓰면 되고, 1회성이라면 iterator_range대신 이것들을 쓴다..

#include "end.h"
#include "../utility/is_arg_c_str.h"
#include "../type_traits/remove_cv.h"

namespace i3
{
	namespace range_detail
	{
		template<class T> inline
		T* make_literal_end( T* const r, arg_true_type) 
		{
			typedef typename i3::remove_cv<T>::type non_cvT;
			return r + i3::char_traits<non_cvT>::length(r) ;			
		}

		template<class T> inline
		typename range_iterator<T>::type make_literal_end( T& r, arg_false_type) 
		{
			return i3::end( r );
		}	
	}

	template<class Range> inline 
	typename range_iterator<Range>::type literal_end( Range& r)
	{
		return range_detail::make_literal_end( r, i3::is_arg_c_str(r) );
	}

	template<class Range> inline 
	typename range_iterator<const Range>::type literal_end( const Range& r)
	{
		return range_detail::make_literal_end( r, i3::is_arg_c_str(r) );
	}

	template<class C, std::size_t sz> inline 
	typename range_iterator<C*>::type literal_end( C (&arr)[sz] )
	{
		return range_detail::make_literal_end( arr, i3::is_arg_c_str(arr) );
	}

	template<class C, std::size_t sz> inline 
	typename range_iterator<const C*>::type literal_end(const C (&arr)[sz] )
	{
		return range_detail::make_literal_end( arr, i3::is_arg_c_str(arr) );
	}
}
