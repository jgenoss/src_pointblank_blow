#pragma once

namespace i3
{
	template<class Object>
	struct has_mem_eq_op;
}

#define		has_mem_op_xxx (has_mem_eq_op, operator==, 1)
#include	"has_mem_op_xxx.h"


