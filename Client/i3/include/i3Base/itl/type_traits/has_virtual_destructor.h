#pragma once

#include "integral_constant.h"

namespace i3
{

	template <class T>
	struct has_virtual_destructor : integral_constant<bool, __has_virtual_destructor(T)> {};

}
