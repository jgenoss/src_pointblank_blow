#pragma once

//
//  컨테이너 기준으로 clear() 멤버 함수가 존재하면 그것을 쓴다..
//  존재하지 않으면, 컴파일 오류를 낸다..
//

// 
//  반복자/포인터의 경우는 첫번째인수만 기본값으로 셋팅한다.
//

#include "../type_traits/has_size_mem_fun.h"

namespace i3
{
	template<class It> inline
	typename enable_if_iterator<It, size_t>::type generic_size(It i)
	{
		typedef typename i3::iterator_value<It>::type	value_type;
		return i3::char_traits<value_type>::length(&*i);
	}
	
	namespace detail
	{
		struct generic_size_with_mem_fun
		{
			template<class Seq> inline 
			static size_t call(Seq& s) {	return s.size();	}
		};
		
		struct generic_size_with_no_mem_fun
		{
			template<class Seq> inline
			static size_t call(Seq& s) { compile_assert_msg(false, container_must_have_size_mem_fun); return 0; }
		};
	}

	template<class Seq> inline 
	typename disable_if_iterator<Seq, size_t>::type generic_size(Seq& s)
	{
		typedef typename mpl::if_<i3::has_size_mem_fun<Seq>, 
								detail::generic_size_with_mem_fun, 
								detail::generic_size_with_no_mem_fun>::type	size_functor;
		return size_functor::call(s);
	}
	
}