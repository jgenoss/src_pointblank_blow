#pragma once

namespace i3
{
	namespace detail
	{
		template<class T> struct is_valid_atomic_integral : false_type {};
	
		template<> struct is_valid_atomic_integral<char> : true_type {};
		template<> struct is_valid_atomic_integral<unsigned char> : true_type {};
		template<> struct is_valid_atomic_integral<signed char> : true_type {};
		
		template<> struct is_valid_atomic_integral<unsigned short> : true_type {};
		template<> struct is_valid_atomic_integral<signed short> : true_type {};
		
		template<> struct is_valid_atomic_integral<unsigned int> : true_type {};
		template<> struct is_valid_atomic_integral<signed int> : true_type {};
		
		template<> struct is_valid_atomic_integral<unsigned long> : true_type {};
		template<> struct is_valid_atomic_integral<long> : true_type {};
		
		template<> struct is_valid_atomic_integral<unsigned long long> : true_type {};
		template<> struct is_valid_atomic_integral<signed long long> : true_type {};
	}
		
}