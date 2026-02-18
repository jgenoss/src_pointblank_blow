#pragma once

namespace i3
{
	template<class T>
	struct has_insert_mem;
}


#define has_mem_xxx		(has_insert_mem, insert)
#include "has_mem_xxx.h"

namespace i3
{
	template<class T, class F>
	struct is_insert_mem_fun_call_possible;
}

#define is_mem_call_possible_xxx (is_insert_mem_fun_call_possible, insert, has_insert_mem)
#include "is_mem_call_possible_xxx.h"
