#pragma once

#include "is_member_function_pointer.h"
#include "integral_constant.h"

namespace i3
{
	template<class T> struct is_member_pointer : is_member_function_pointer<T> {};
	template <class T, class U> struct is_member_pointer<U T::*> : true_type {};
	template <class T, class U> struct is_member_pointer<U T::* const> : true_type {};
	template <class T, class U> struct is_member_pointer<U T::* volatile> : true_type {};
	template <class T, class U> struct is_member_pointer<U T::* const volatile> : true_type {};

}