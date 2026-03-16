#pragma once

#include <cstddef>
#include "integral_constant.h"

namespace i3
{
	template <class T> struct is_array : false_type {};
	template <class T, std::size_t N> struct is_array<T[N]> : true_type {};
	template <class T, std::size_t N> struct is_array<T const [N]> : true_type {};
	template <class T, std::size_t N> struct is_array<T volatile [N]> : true_type {};
	template <class T, std::size_t N> struct is_array<T const volatile [N]> : true_type {};

	template <class T> struct is_array<T []> : true_type {};
	template <class T> struct is_array<T const []> : true_type {};
	template <class T> struct is_array<T volatile []> : true_type {};
	template <class T> struct is_array<T const volatile []> : true_type {};
}