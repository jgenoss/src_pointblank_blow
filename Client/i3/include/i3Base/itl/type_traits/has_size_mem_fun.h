#pragma once

namespace i3
{
	template <class T>
	struct has_size_mem;
}

#define has_mem_xxx		(has_size_mem, size)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T>
	struct has_size_mem_fun;
}

#define has_mem_fun_xxx (has_size_mem_fun, size, 0, has_size_mem)
#include "has_mem_fun_xxx.h"

/*
#include "is_mem_call_possible_xxx.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_size_mem_fun_impl : 
			is_size_mem_fun_call_possible<T, std::size_t()> 
		{
		};
	}

	template<class T>
	struct has_size_mem_fun : detail::has_size_mem_fun_impl<T> {};

}
*/