#pragma once

#include "../type_traits/has_iterators_erase_mem_fun.h"
#include "detail/enable_if_iterator.h"
#include "iterator.h"
#include "value_type.h"
#include "../utility/copy_or_swap.h"

namespace i3
{
	// 이 버전의 리턴값은 반복자 말고는 방법이 없는데...인수가 컨테이너라면 반복자를 찾을 이유가 
	// 많지 않아 보인다.. 하지만, 일단은 리턴값을 모두 공통으로 반복자로 정한다...
	// 모든 리턴값은 지운범위 다음값 ( last위치)의 값이다....

	template<class Literal, class InIt> inline typename i3::enable_if_iterator<Literal,
	Literal>::type generic_erase_iterators(Literal ctn, InIt first, InIt last)
	{
		//1. begin~first까지는 유지됨..
		//2. last~ 널앞값까지를 first에 카피..
		typedef typename i3::iterator_value<Literal>::type	value_type;
		InIt orig_end_it = ctn;
		i3::advance(orig_end_it, i3::char_traits<value_type>::length( &(*ctn) ) );
				
		Literal new_end_it = i3::copy( last, orig_end_it, first );		 
		*new_end_it = value_type();	// 널값처리 필요..
		return first;	// 지우고 난뒤 첫위치..(무효화되지 않으므로 이대로 종료)
	}

	//
	// erase(iter, iter)가 필요함..없다면, copy_swap으로 대신 처리한다..
	//
	namespace detail
	{
		template<class Seq, class InIt, bool has_erase>
		struct generic_erase_iterators_impl;

		template<class Seq, class InIt>
		struct generic_erase_iterators_impl<Seq, InIt, true>
		{
			typedef typename i3::range_iterator<Seq>::type	result_iterator;
			static inline result_iterator call(Seq& s, InIt first, InIt last)
			{
				return s.erase( first, last);
			}
		};

		template<class Seq, class InIt>
		struct generic_erase_iterators_impl<Seq, InIt, false>
		{
			typedef typename i3::range_iterator<Seq>::type	result_iterator;

			static inline result_iterator call(Seq& s, InIt first, InIt last)
			{
				// 컨테이너 자체가 삭제가 불가능한 것일 가능성이 높다.  다른 종류의 컨테이너를 동원한다.
				typedef typename i3::range_value<Seq>::type			value_type;
				typedef typename i3::range_difference<Seq>::type	diff_t;
				
				diff_t first_offset = i3::distance(i3::begin(s), first);
				diff_t last_offset  = i3::distance(i3::begin(s), last);

				// 벡터의 삭제는 first 이전의 반복자는 무효화되지 않으며, 재할당은 없다.
				i3::vector<value_type> tmp_vec(i3::begin(s), i3::end(s) );	// allocator도 필요할수 있지만, 우선 이렇게만 처리..
				tmp_vec.erase(tmp_vec.begin() + first_offset, tmp_vec.begin() + last_offset);
				
				Seq tmp( tmp_vec.begin(), tmp_vec.end() );
				i3::copy_or_swap(s, tmp);
				
				// 시간이 없어서, 일단 표준 advance를 쓴다.. (표준 advance가 임시변수를 만드므로 좀 불편하다)
				result_iterator result = s.begin();
				i3::advance(result, first_offset);
				return result;
				//
			}
		};

	}


	template<class Seq, class InIt> inline typename i3::lazy_disable_if_iterator<Seq,
	i3::range_iterator<Seq> >::type generic_erase_iterators(Seq& s, InIt first, InIt last)
	{
		return detail::generic_erase_iterators_impl<Seq, InIt, 
			i3::has_iterators_erase_mem_fun<Seq>::value>::call(s, first, last);
	}


}