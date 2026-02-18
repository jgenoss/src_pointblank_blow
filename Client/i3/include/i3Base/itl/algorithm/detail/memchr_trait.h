#pragma once

#include "../../iterator/iterator_traits.h"
#include "../../type_traits/is_integral.h"
#include "../../type_traits/is_convertible.h"
#include "../../type_traits/is_pointer.h"

#include "is_char_wchar_size_impl.h"

namespace i3
{
	namespace detail
	{
		
		template<class ItVal, class T, class Pred>
		struct is_memchr_wmemchr_convertible_impl : mpl::if_< mpl::and_<is_integral<ItVal>, is_convertible<T, ItVal> >,
																  Pred, false_type>
		{
		};

		template<class InIt, class T>
		struct is_memchr_possible_impl 
		{
			typedef typename i3::iterator_value<InIt>::type value_type;
			typedef typename is_memchr_wmemchr_convertible_impl<value_type, T, is_char_size_impl<value_type> >::type type;
		};

		template<class InIt, class T>
		struct is_wmemchr_possible_impl 
		{
			typedef typename i3::iterator_value<InIt>::type value_type;
			typedef typename is_memchr_wmemchr_convertible_impl<value_type, T, is_wchar_size_impl<value_type> >::type type;
		};			
		
		template<class InIt, class T>
		struct is_memchr_wmemchr_possible_impl
		{
			typedef typename i3::iterator_value<InIt>::type value_type;
			typedef typename is_memchr_wmemchr_convertible_impl<value_type, T, is_char_wchar_size_impl<value_type> >::type type;
		};	
		
		template<class InIt, class T>
		struct is_memchr_enable_it_v
		{
			typedef typename mpl::eval_if< is_pointer<InIt>, is_memchr_possible_impl<InIt, T>, false_type>::type type;
			static const bool value = type::value;
		};

		template<class InIt, class T>
		struct is_wmemchr_enable_it_v
		{
			typedef typename mpl::eval_if< is_pointer<InIt>, is_wmemchr_possible_impl<InIt, T>, false_type>::type type;
			static const bool value = type::value;
		};

		template<class InIt, class T>
		struct is_memchr_wmemchr_enable_it_v
		{
			typedef typename mpl::eval_if< is_pointer<InIt>, is_memchr_wmemchr_possible_impl<InIt, T>, false_type>::type type;
			static const bool value = type::value;
		};

		template<class SrcIt, class DelimIt>
		struct is_memchr_enable_it_it : is_memchr_enable_it_v<DelimIt, typename i3::iterator_value<SrcIt>::type>
		{
		};

		template<class SrcIt, class DelimIt>
		struct is_wmemchr_enable_it_it : is_wmemchr_enable_it_v<DelimIt, typename i3::iterator_value<SrcIt>::type>
		{
		};
		
		template<class SrcIt, class DelimIt>
		struct is_memchr_wmemchr_enable_it_it : is_memchr_wmemchr_enable_it_v<DelimIt, typename i3::iterator_value<SrcIt>::type>
		{
		};

	}
	
}