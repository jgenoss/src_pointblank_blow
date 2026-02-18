#pragma once
#include "../../iterator/iterator_traits.h"
#include "../../type_traits/is_integral.h"
#include "../../type_traits/is_pointer.h"

#include "is_char_wchar_size_impl.h"

namespace i3
{
	namespace detail
	{
		
		template<class ItVal1, class ItVal2, class Pred>
		struct is_memcmp_wmemcmp_comparable_impl : 
			mpl::if_< mpl::and_<is_integral<ItVal1>, is_integral<ItVal2> >,
			Pred, false_type>
		{
		};


		template<class It1, class It2>
		struct is_memcmp_impl
		{
			typedef typename i3::iterator_value<It1>::type v1;
			typedef typename i3::iterator_value<It1>::type v2;
			typedef typename is_memcmp_wmemcmp_comparable_impl<v1, v2, is_char_size_impl2<v1, v2> >::type type;
		};

		template<class It1, class It2>
		struct is_wmemcmp_impl
		{
			typedef typename i3::iterator_value<It1>::type v1;
			typedef typename i3::iterator_value<It1>::type v2;
			typedef typename is_memcmp_wmemcmp_comparable_impl<v1, v2, is_wchar_size_impl2<v1, v2> >::type type;
		};
		
		template<class It1, class It2>
		struct is_memcmp_wmemcmp_impl
		{
			typedef typename i3::iterator_value<It1>::type v1;
			typedef typename i3::iterator_value<It1>::type v2;
			typedef typename is_memcmp_wmemcmp_comparable_impl<v1, v2, is_char_wchar_size_impl2<v1, v2> >::type type;
		};

		template<class It1, class It2>
		struct is_memcmp_enable : mpl::eval_if< mpl::and_< is_pointer<It1>, is_pointer<It2> >,
												is_memcmp_impl<It1, It2>, false_type>
		{
			static const bool value = type::value;
		};

		template<class It1, class It2>
		struct is_wmemcmp_enable : mpl::eval_if< mpl::and_< is_pointer<It1>, is_pointer<It2> >,
												is_wmemcmp_impl<It1, It2>, false_type>
		{
			static const bool value = type::value;
		};

		template<class It1, class It2>
		struct is_memcmp_wmemcmp_enable : mpl::eval_if< mpl::and_< is_pointer<It1>, is_pointer<It2> >,
												is_memcmp_wmemcmp_impl<It1, It2>, false_type>
		{
			static const bool value = type::value;
		};

	}
}