#pragma once

#include "../type_traits/has_iterators_erase_mem_fun.h"
#include "detail/enable_if_iterator.h"
#include "iterator.h"
#include "value_type.h"
#include "../utility/copy_or_swap.h"

namespace i3
{
	//
	// erase함수지원 컨테이너가 아니라면, generic_erase_iterators버전보다 아래 함수가 더 낫다..
	//

	template<class Literal, class InIt> inline typename i3::enable_if_iterator<Literal,
	Literal>::type generic_erase_begin_iter(Literal ctn, InIt pos)
	{
		i3::iterator_range<Literal> rng = i3::as_literal(ctn);		// 문자열로 간주..
		Literal end_it = i3::copy(pos, end(rng), begin(rng) );				// 시작 - pos까지 지울것임.. 
																			// i3::copy류면 충분할것 같다.
		typedef typename i3::iterator_value<Literal>::type value_type;
		*end_it = value_type();												// 널값처리 필요..
		return ctn;															// 지우고난 다음값..
	}
	
	//
	// erase(iter, iter)가 필요함..없다면, copy_swap으로 대신 처리한다..
	//
	namespace detail
	{
		template<class Seq, class InIt, bool has_erase>
		struct generic_erase_begin_iter_impl;

		template<class Seq, class InIt>
		struct generic_erase_begin_iter_impl<Seq, InIt, true>
		{
			typedef typename i3::range_iterator<Seq>::type	result_iterator;

			static inline result_iterator call(Seq& s, InIt pos)
			{
				return s.erase( begin(s), pos);
			}
		};

		template<class Seq, class InIt>
		struct generic_erase_begin_iter_impl<Seq, InIt, false>
		{
			typedef typename i3::range_iterator<Seq>::type	result_iterator;

			static inline result_iterator call(Seq& s, InIt pos)
			{
				// 컨테이너 자체가 삭제가 불가능한 것일 가능성이 높다.  다른 종류의 컨테이너를 동원한다.
				// erase를 쓰지 않아도 처리 가능하므로..다른 컨테이너는 동원하지 않는다..
				Seq tmp( pos, i3::end(s) );
				i3::copy_or_swap(s, tmp);
				return s.begin();
			}
		};
		
	}


	template<class Seq, class InIt> inline typename i3::lazy_disable_if_iterator<Seq,
	i3::range_iterator<Seq> >::type generic_erase_begin_iter(Seq& s, InIt pos)
	{
		return detail::generic_erase_begin_iter_impl<Seq, InIt, 
			i3::has_iterators_erase_mem_fun<Seq>::value>::call(s, pos);
	}
	
	
}