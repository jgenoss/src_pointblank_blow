#pragma once

namespace i3
{
	template <class T>
	struct has_clear_mem;
}

#define has_mem_xxx		(has_clear_mem, clear)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T, class F>
	struct is_clear_mem_fun_call_possible;
}

#define is_mem_call_possible_xxx (is_clear_mem_fun_call_possible, clear, has_clear_mem)
#include "is_mem_call_possible_xxx.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_clear_mem_fun_impl : 
			is_clear_mem_fun_call_possible<T, void ()> 
		{
		};
	}

	template<class T>
	struct has_clear_mem_fun : detail::has_clear_mem_fun_impl<T> {};

}
