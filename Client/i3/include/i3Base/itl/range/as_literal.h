#pragma once

#include "iterator_range.h"
#include "../utility/is_arg_c_str.h"
#include "../type_traits/remove_cv.h"

namespace i3
{
	namespace range_detail
	{
		template<class Range, class Ret>
		struct enable_if_c_str : i3::enable_if< 
			mpl::or_< i3::is_convertible<Range, const char*>, i3::is_convertible<Range, const wchar_t*> >,
			Ret> {};

		template<class Range, class Ret>
		struct disable_if_c_str : i3::disable_if< 
			mpl::or_< i3::is_convertible<Range, const char*>, i3::is_convertible<Range, const wchar_t*> >,
			Ret> {};


		template<class T> inline
		iterator_range<T*> make_range( T* const r, arg_true_type )							// 암시적 형변환에 의한 인수추론은 되지 않는다.
		{
			typedef typename i3::remove_cv<T>::type non_cvT;
			return iterator_range<T*>( r, r + i3::char_traits<non_cvT>::length(r) );			
		}

		template<class T> inline
		iterator_range<typename range_iterator<T>::type > make_range( T& r, arg_false_type ) 
		{
			return iterator_range<typename range_iterator<T>::type >(r);
		}
		
		template<class T> inline
		iterator_range<reverse_iterator<T*> > make_reverse_range(T* const r, arg_true_type)			// 암시적 형변환에 의한 인수추론은 되지 않는다.
		{
			typedef typename i3::remove_cv<T>::type non_cvT;
			return iterator_range<reverse_iterator<T*> >( 
				reverse_iterator<T*>(r + i3::char_traits<non_cvT>::length(r)), reverse_iterator<T*>(r) );			
		}

		template<class T> inline
		iterator_range<typename range_reverse_iterator<T>::type > make_reverse_range( T& r, arg_false_type ) 
		{
			return iterator_range<typename range_reverse_iterator<T>::type >(i3::rbegin(r), i3::rend(r) );
		}

	}
	
	template<class Range> inline	typename range_detail::disable_if_c_str< Range,
	iterator_range<typename range_iterator<Range>::type> >::type as_literal( Range& r)
	{
		return iterator_range<typename range_iterator<Range>::type>(r);
	}
	
	template<class Range> inline	typename range_detail::disable_if_c_str< const Range,
	iterator_range<typename range_iterator<const Range>::type> >::type as_literal( const Range& r)
	{
		return iterator_range<typename range_iterator<const Range>::type>(r);
	}

	template<class Range> inline	typename range_detail::enable_if_c_str< Range,
	iterator_range<typename range_iterator<Range>::type> >::type as_literal( Range& r)
	{
		return range_detail::make_range(r, arg_true);
	}

	template<class Range> inline	typename range_detail::enable_if_c_str< const Range,
	iterator_range<typename range_iterator<const Range>::type> >::type as_literal( const Range& r)
	{
		return range_detail::make_range(r, arg_true);
	}

	template<class T> inline	
	iterator_range<T*> as_literal( T* r)
	{
		typedef typename i3::remove_cv<T>::type non_cvT;
		return iterator_range<T*>( r, r + i3::char_traits<non_cvT>::length(r) );	
	}
	

//// 이하부터 확장형

	template<class Range> inline typename range_detail::disable_if_c_str< Range,
	iterator_range<typename range_reverse_iterator<Range>::type> >::type as_reverse_literal( Range& r)
	{
		return iterator_range<typename range_reverse_iterator<Range>::type>( i3::rbegin(r), i3::rend(r) );
	}

	template<class Range> inline typename range_detail::disable_if_c_str< const Range,
	iterator_range<typename range_reverse_iterator<const Range>::type> >::type as_reverse_literal(const Range& r)
	{
		return iterator_range<typename range_reverse_iterator<const Range>::type>( i3::rbegin(r), i3::rend(r) );
	}

	template<class Range> inline typename range_detail::enable_if_c_str< Range,
	iterator_range<typename range_reverse_iterator<Range>::type> >::type as_reverse_literal( Range& r)
	{
		return range_detail::make_reverse_range(r, arg_true);
	}

	template<class Range> inline typename range_detail::enable_if_c_str< const Range,
	iterator_range<typename range_reverse_iterator<const Range>::type> >::type as_reverse_literal(const Range& r)
	{
		return range_detail::make_reverse_range(r, arg_true);
	}
	
	template<class T> inline	
	iterator_range< reverse_iterator<T*> > as_reverse_literal( T* r)
	{
		typedef typename i3::remove_cv<T>::type non_cvT;
		return iterator_range< reverse_iterator<T*> >( 
			reverse_iterator<T*>(r + i3::char_traits<non_cvT>::length(r)), reverse_iterator<T*>(r) );	
	}

}
