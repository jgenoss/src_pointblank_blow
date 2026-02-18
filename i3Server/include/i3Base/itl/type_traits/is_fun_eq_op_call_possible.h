#pragma once

namespace i3
{
	template<class Function>
	struct is_fun_eq_op_call_possible;
}

#define is_fun_op_call_possible_xxx ( is_fun_eq_op_call_possible, operator==, 2 )

#include "is_fun_op_call_possible_xxx.h"

