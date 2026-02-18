#pragma once

namespace i3
{
	template<class T>
	struct has_assign_mem;
}

#define has_mem_xxx		(has_assign_mem, assign)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T, class F>
	struct is_assign_mem_fun_call_possible;
}

#define is_mem_call_possible_xxx (is_assign_mem_fun_call_possible, assign, has_assign_mem)
#include "is_mem_call_possible_xxx.h"

////////////////////////////////////////////////

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_iterator_assign_mem_fun_impl : 
			is_assign_mem_fun_call_possible<T, void (typename T::iterator, typename T::iterator)>
		{
		};
	}

	template<class T>
	struct has_iterator_assign_mem_fun : mpl::eval_if< has_iterator<T>, 
													 detail::has_iterator_assign_mem_fun_impl<T>, 
													 false_type> {};
}

