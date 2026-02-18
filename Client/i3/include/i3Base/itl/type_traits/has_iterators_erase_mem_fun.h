#pragma once

#include "has_iterator.h"

namespace i3
{
	template<class T>
	struct has_erase_mem;
}

#define has_mem_xxx		(has_erase_mem, erase)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T, class F>
	struct is_iterators_erase_mem_fun_call_possible;
}

#define is_mem_call_possible_xxx (is_iterators_erase_mem_fun_call_possible, erase, has_erase_mem)
#include "is_mem_call_possible_xxx.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_iterators_erase_mem_fun_impl : 
			is_iterators_erase_mem_fun_call_possible<T, void (typename T::iterator, typename T::iterator)> 
		{
		};
	}

	template<class T>
	struct has_iterators_erase_mem_fun : mpl::eval_if< 
		has_iterator<T>, 
		detail::has_iterators_erase_mem_fun_impl<T>, 
		false_type> {};

}
