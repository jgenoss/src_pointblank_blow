#pragma once

#include "is_volatile.h"
#include "is_pod.h"
#include "ice_and.h"
#include "ice_or.h"
#include "ice_not.h"

namespace i3
{
	
	template <class T> 
	struct has_trivial_copy : integral_constant< bool,
		ice_and<ice_or< is_pod<T>::value, __has_trivial_copy(T) >::value,
				ice_not< is_volatile<T>::value >::value >::value > {};
	
	template <class T> 
	struct has_trivial_copy_constructor : has_trivial_copy<T> {};
	
	
}