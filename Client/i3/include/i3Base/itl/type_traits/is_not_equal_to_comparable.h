#pragma once


namespace i3
{
	template<class T, class U = T> struct is_not_equal_to_comparable;
}

#define is_binary_op_xxx	(is_not_equal_to_comparable,!=, bool)
#include "is_binary_op_xxx.h"

