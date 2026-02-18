#pragma once

#include "is_pod.h"
#include "is_const.h"
#include "is_volatile.h"
#include "ice_and.h"
#include "ice_or.h"
#include "ice_not.h"
#include "integral_constant.h"

namespace i3
{
	template <class T> 
	struct has_trivial_assign : integral_constant< bool, 
		ice_and<ice_or< is_pod<T>::value, __has_trivial_assign(T) >::value, 
				ice_not< is_const<T>::value >::value,
				ice_not< is_volatile<T>::value >::value >::value > {};
}
