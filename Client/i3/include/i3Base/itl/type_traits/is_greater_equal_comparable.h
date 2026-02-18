#pragma once


namespace i3
{
	template<class T, class U = T> struct is_greater_equal_comparable;
}

#define is_binary_op_xxx	(is_greater_equal_comparable,>=, bool)
#include "is_binary_op_xxx.h"

