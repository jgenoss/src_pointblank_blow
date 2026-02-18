#pragma once

#include "has_trivial_assign.h"

namespace i3
{
	template <class T>
	struct has_nothrow_assign : integral_constant<bool, 
		ice_or< has_trivial_assign<T>::value, __has_nothrow_assign(T)>::value > {};
}

