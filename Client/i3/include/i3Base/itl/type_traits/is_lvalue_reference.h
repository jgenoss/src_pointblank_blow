#pragma once


#include "integral_constant.h"

namespace i3
{
	template<class T>	struct is_lvalue_reference : false_type {};
	template<class T>	struct is_lvalue_reference<T&> : true_type {};
}

