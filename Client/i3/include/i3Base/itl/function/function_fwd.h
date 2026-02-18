#pragma once

#include "../type_traits/remove_pointer.h"

namespace i3
{
	
	// 디폴트 매개 변수가 함수형에서 함수포인터형으로 희한하게 강제 변형되기 때문에, 이 함수의 포인터를 제거한다..
	
	template<class F = i3::remove_pointer<void()>::type >
	class function;

	namespace detail
	{
		template<class, class, class> class functor_fun;	
		template<class, class, class> class functor_fun_ref; 
		template<class, class, class> class functor_mem_fun_ptr;
		template<class, class, class> class functor_mem_fun_ref;
		template<class, class, class, class> class functor_obj_mem_fun;
	}
}

#define friend_function	\
	template<class, class, class> friend class ::i3::detail::functor_fun;	\
	template<class, class, class> friend class ::i3::detail::functor_fun_ref; \
	template<class, class, class> friend class ::i3::detail::functor_mem_fun_ptr; \
	template<class, class, class> friend class ::i3::detail::functor_mem_fun_ref; \
	template<class, class, class, class> friend class ::i3::detail::functor_obj_mem_fun