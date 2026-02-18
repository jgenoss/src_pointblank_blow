#pragma once

namespace i3
{
	template<class Object, class Function>
	struct is_mem_eq_op_call_possible;
}

#include "has_mem_eq_op.h"

#define is_mem_op_call_possible_xxx (is_mem_eq_op_call_possible, operator==, has_mem_eq_op, 1)

#include "is_mem_op_call_possible_xxx.h"

