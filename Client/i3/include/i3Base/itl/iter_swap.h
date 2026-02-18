#pragma once

#include "mpl/mpl_and.h"

namespace i3
{
	namespace detail
	{
		
		template <typename Iter>
		static void iter_swap_impl(Iter one, Iter two, const i3::mpl::false_&)
		{
			typedef typename i3::iterator_traits<Iter>::value_type v_t;
			v_t v = *one;
			*one = *two;
			*two = v;
		}

		template <typename Iter>
		static void iter_swap_impl(Iter one, Iter two, const i3::mpl::true_&)
		{
			using std::swap;
			swap(*one, *two);
		}

	}
	// 부스트 버전..
	template <typename Iter1, typename Iter2>
	inline void iter_swap(Iter1 iter1, Iter2 iter2 )
	{
		typedef typename i3::iterator_traits<Iter1>::reference r1_t;
		typedef typename i3::iterator_traits<Iter2>::reference r2_t;
		typedef typename mpl::and_< is_reference<r1_t>, is_reference<r2_t>, is_same<r1_t, r2_t> >::type		result_type;

		detail::iter_swap_impl(iter1, iter2, result_type() );
	}
	
}