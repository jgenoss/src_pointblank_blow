#pragma once

// reserve member function이 있으면 호출하고, 그렇지 않으면 no op 처리한다..

#include "../type_traits/has_reserve_mem_fun.h"
#include "detail/enable_if_iterator.h"

namespace i3
{
	template<class It> inline typename enable_if_iterator<It>::type
	generic_reserve(It it, std::size_t num) 
	{
	}
	
	namespace detail
	{
		struct generic_reserve_with_mem_fun
		{
			template<class Seq> inline
			static void call(Seq& s, std::size_t num) {  s.reserve(num);  }
		};
		
		struct generic_reserve_with_no_mem_fun
		{
			template<class Seq> inline
			static void call(Seq& , std::size_t ) {}
		};
	}

	template<class Seq> inline typename disable_if_iterator<Seq>::type
	generic_reserve(Seq& s, std::size_t num)
	{
		typedef typename mpl::if_<i3::has_reserve_mem_fun<Seq>, 
								  detail::generic_reserve_with_mem_fun,
								  detail::generic_reserve_with_no_mem_fun>::type reserve_functor;
		reserve_functor::call(s, num);
	}

}
