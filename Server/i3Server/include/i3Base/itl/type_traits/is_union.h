#pragma once

#include "remove_cv.h"
#include "integral_constant.h"

namespace i3
{
	template <class T> struct is_union : integral_constant<bool, __is_union( typename remove_cv<T>::type )> {};
}
