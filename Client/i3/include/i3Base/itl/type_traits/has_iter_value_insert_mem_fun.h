#pragma once


#include "is_insert_mem_fun_call_possible.h"
#include "has_iterator.h"
#include "has_value_type.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_iter_value_insert_mem_fun_impl : 
			is_insert_mem_fun_call_possible<T, void (typename T::iterator, typename T::value_type)> 
		{
		};
	}

	template<class T>
	struct has_iter_value_insert_mem_fun : mpl::eval_if< 
		mpl::and_<has_value_type<T>, has_iterator<T> >, 
		detail::has_iter_value_insert_mem_fun_impl<T>, 
		false_type> {};

}
