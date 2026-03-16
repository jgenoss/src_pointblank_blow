#pragma once

#include "has_trivial_copy.h"

namespace i3
{
	template <class T>
	struct has_nothrow_copy : integral_constant<bool, 
		ice_or< has_trivial_copy<T>::value, __has_nothrow_copy(T)>::value > {};
	
	template <class T>
	struct has_nothrow_copy_constructor : has_nothrow_copy<T> {};
}
