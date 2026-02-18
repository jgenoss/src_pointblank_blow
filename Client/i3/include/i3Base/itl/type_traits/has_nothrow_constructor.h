#pragma once

#include "has_trivial_constructor.h"

namespace i3
{
	template <class T>
	struct has_nothrow_constructor : integral_constant<bool,
		ice_or< has_trivial_constructor<T>::value, __has_nothrow_constructor(T)>::value > {};

	template <class T>
	struct has_nothrow_default_constructor : has_nothrow_constructor<T> {};
}
