#pragma once

#include "integral_constant.h"

namespace i3
{
	template <class T, class U> struct is_same : false_type {}; 
	template <class T> struct is_same<T, T> : true_type {}; 
}
