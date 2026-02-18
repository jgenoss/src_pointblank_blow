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
	Literal>::type generic_erase_iter_end(Literal ctn, InIt pos)
	{
		// 이 버전은 길이값을 구할 필요가 없다.
		Literal nc_pos = Literal(pos);
		typedef typename i3::iterator_value<Literal>::type value_type;
		*nc_pos = value_type();
		return nc_pos;														
	}

	//
	// erase(iter, iter)가 필요함..없다면, copy_swap으로 대신 처리한다..
	//
	namespace detail
	{
		template<class Seq, class InIt, bool has_erase>
		struct generic_erase_iter_end_impl;

		template<class Seq, class InIt>
		struct generic_erase_iter_end_impl<Seq, InIt, true>
		{
			typedef typename i3::range_iterator<Seq>::type	result_iterator;

			static inline result_iterator call(Seq& s, InIt pos)
			{
				return s.erase( pos, s.end() );
			}
		};

		template<class Seq, class InIt>
		struct generic_erase_iter_end_impl<Seq, InIt, false>
		{
			typedef typename i3::range_iterator<Seq>::type	result_iterator;

			static inline result_iterator call(Seq& s, InIt pos)
			{
				// 컨테이너 자체가 삭제가 불가능한 것일 가능성이 높다.  다른 종류의 컨테이너를 동원한다.
				// erase를 쓰지 않아도 처리 가능하므로..다른 컨테이너는 동원하지 않는다..
				Seq tmp( i3::begin(s), pos );
				i3::copy_or_swap(s, tmp);
				return s.end();
			}
		};

	}

	template<class Seq, class InIt> inline typename i3::lazy_disable_if_iterator<Seq,
		i3::range_iterator<Seq> >::type generic_erase_iter_end(Seq& s, InIt pos)
	{
		return detail::generic_erase_iter_end_impl<Seq, InIt, 
			i3::has_iterators_erase_mem_fun<Seq>::value>::call(s, pos);
	}


}