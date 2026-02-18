#pragma once

#include "has_value_type.h"

namespace i3
{
	template <class T>
	struct has_push_back_mem;
}

#define has_mem_xxx		(has_push_back_mem, push_back)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T, class F>
	struct is_push_back_mem_fun_call_possible;
}

#define is_mem_call_possible_xxx (is_push_back_mem_fun_call_possible, push_back, has_push_back_mem)
#include "is_mem_call_possible_xxx.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_push_back_mem_fun_impl : 
			is_push_back_mem_fun_call_possible<T, void (typename T::value_type)> 
		{
		};
	}

	template<class T>
	struct has_push_back_mem_fun : mpl::eval_if< 
		has_value_type<T>, 
		detail::has_push_back_mem_fun_impl<T>, 
		false_type> {};
	
}
