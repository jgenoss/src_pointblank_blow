#pragma once

#include "is_pod.h"
#include "ice_or.h"
#include "integral_constant.h"

namespace i3
{
	
	template <class T>
	struct has_trivial_constructor : integral_constant<bool, 
		ice_or< is_pod<T>::value, __has_trivial_constructor(T) >::value> {};

	template <class T>
	struct has_trivial_default_constructor : has_trivial_constructor<T> {};

	
}