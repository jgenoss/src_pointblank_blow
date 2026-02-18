#pragma once

#include "integral_constant.h"

namespace i3
{
	template <typename T> struct is_void : false_type {};
	template <> struct is_void<void> : true_type {};
	template <> struct is_void<void const>	: true_type {};
	template <> struct is_void<void volatile>	: true_type {};
	template <> struct is_void<void const volatile>	: true_type {};
}
