#pragma once

#include "has_size_type.h"

namespace i3
{
	template<class T>
	struct has_reserve_mem;
}

#define has_mem_xxx (has_reserve_mem, reserve)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T, class F>
	struct is_reserve_mem_fun_call_possible;
}

#define is_mem_call_possible_xxx (is_reserve_mem_fun_call_possible, reserve, has_reserve_mem)
#include "is_mem_call_possible_xxx.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_reserve_mem_fun_impl : 
			is_reserve_mem_fun_call_possible< T, void (typename T::size_type)>
		{
		};
	}

	template<class T>
	struct has_reserve_mem_fun : mpl::eval_if< has_size_type<T>,
		detail::has_reserve_mem_fun_impl<T>, false_type>
	{
		static const bool value = type::value;
	};
	
}
