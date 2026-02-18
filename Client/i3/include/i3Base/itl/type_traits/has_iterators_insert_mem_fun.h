#pragma once


#include "is_insert_mem_fun_call_possible.h"
#include "has_iterator.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_iterators_insert_mem_fun_impl : 
			is_insert_mem_fun_call_possible<T, void (typename T::iterator, typename T::iterator, typename T::iterator)> 
		{
		};
	}

	template<class T>
	struct has_iterators_insert_mem_fun : mpl::eval_if< 
		has_iterator<T>, 
		detail::has_iterators_insert_mem_fun_impl<T>, 
		false_type> {};

}
