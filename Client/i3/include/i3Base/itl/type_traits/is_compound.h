#pragma once

#include "is_fundamental.h"
#include "ice_not.h"

namespace i3
{
	template<class T> struct is_compound : integral_constant<bool, ice_not<is_fundamental<T>::value>::value> {};
}
