#pragma once


namespace i3
{
	template<class T, class U = T> struct is_less_equal_comparable;
}

#define is_binary_op_xxx	(is_less_equal_comparable,<=, bool)
#include "is_binary_op_xxx.h"

