#pragma once

#include "is_void.h"
#include "is_scalar.h"
#include "ice_and.h"
#include "ice_or.h"



namespace i3
{
	template <class T> struct is_pod : integral_constant<bool, 
		ice_or<is_scalar<T>::value, is_void<T>::value, ice_and<__is_pod(T),__has_trivial_constructor(T)>::value >::value> {};

	template <typename T, std::size_t sz> struct is_pod<T[sz]> : is_pod<T>{ };
	template <> struct is_pod<void> : true_type	{};
	template <> struct is_pod<void const> : true_type {};
	template <> struct is_pod<void volatile> : true_type {};
	template <> struct is_pod<void const volatile> : true_type {};
	
}