#pragma once

#include "is_pod.h"
#include "ice_or.h"

namespace i3
{
	template <class T>
	struct has_trivial_destructor : integral_constant<bool, 
		ice_or< is_pod<T>::value, __has_trivial_destructor(T) >::value> {}; 
}
