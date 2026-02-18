#pragma once

#include "integral_constant.h"

namespace i3
{
	template <class T> struct is_floating_point : false_type {};
	template <> struct is_floating_point<float> : true_type {};
	template <> struct is_floating_point<double> : true_type {};
	template <> struct is_floating_point<long double> : true_type {};

	template <class T> struct is_float : false_type {};
	template <> struct is_float<float> : true_type {};
	template <> struct is_float<double> : true_type {};
	template <> struct is_float<long double> : true_type {};

}
