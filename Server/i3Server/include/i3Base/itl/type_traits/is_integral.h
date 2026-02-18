#pragma once

#include "integral_constant.h"

namespace i3
{
	template<class T> struct is_integral : false_type {};
	template<> struct is_integral<unsigned char> : true_type {};
	template<> struct is_integral<unsigned short> : true_type {};
	template<> struct is_integral<unsigned int> : true_type {};
	template<> struct is_integral<unsigned long> : true_type {};
	template<> struct is_integral<signed char> : true_type {};
	template<> struct is_integral<signed short> : true_type {};
	template<> struct is_integral<signed int> : true_type {};
	template<> struct is_integral<signed long> : true_type {};

	template<> struct is_integral<bool> : true_type {};
	template<> struct is_integral<char> : true_type {};

#ifdef _NATIVE_WCHAR_T_DEFINED
	template<> struct is_integral<wchar_t> : true_type {};
#endif	

	template<> struct is_integral<unsigned long long> : true_type {};
	template<> struct is_integral<long long> : true_type {};
		
	
}