#pragma once

namespace i3
{
	template<class T>
	struct has_pointer_to_mem;
}

#define has_mem_xxx (has_pointer_to_mem, pointer_to)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T>
	struct has_pointer_to_mem_fun_impl;
}

#define has_mem_fun_xxx (has_pointer_to_mem_fun_impl, pointer_to, 1, has_pointer_to_mem)
#include "has_mem_fun_xxx.h"

namespace i3
{
	template<class T, class F>
	struct is_pointer_to_mem_fun_call_possible_impl;
}

#define is_mem_call_possible_xxx (is_pointer_to_mem_fun_call_possible_impl, pointer_to, has_pointer_to_mem)
#include "is_mem_call_possible_xxx.h"

namespace i3
{
	template<class T, class R, class Arg>
	struct is_pointer_to_static_mem_fun_call_possible 
	{
		typedef typename mpl::and_< 
			has_pointer_to_mem<T>,
			mpl::not_<has_pointer_to_mem_fun_impl<T> >, 
			is_pointer_to_mem_fun_call_possible_impl<T, R (Arg) > >::type   type;
		static const bool value = type::value;
	};
}


