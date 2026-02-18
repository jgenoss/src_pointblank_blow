#pragma once

#include "../../type_traits/integral_constant.h"

namespace i3
{
	namespace detail
	{

		template<class T>
		struct is_char_size_impl : integral_constant<bool, sizeof(T) == sizeof(char)> {};

		template<class T>
		struct is_wchar_size_impl : integral_constant<bool, sizeof(T) == sizeof(wchar_t)> {};

		template<class T>
		struct is_char_wchar_size_impl : integral_constant<bool, (sizeof(T) == sizeof(char) || sizeof(T) == sizeof(wchar_t)) > {};


		template<class T1, class T2>
		struct is_char_size_impl2 : integral_constant<bool, sizeof(T1) == sizeof(char) && 
															sizeof(T2) == sizeof(char) > {};

		template<class T1, class T2>
		struct is_wchar_size_impl2 : integral_constant<bool, sizeof(T1) == sizeof(wchar_t) &&
															 sizeof(T2) == sizeof(wchar_t) > {};

		template<class T1, class T2>
		struct is_char_wchar_size_impl2 : integral_constant<bool, ( (sizeof(T1) == sizeof(char) &&
																sizeof(T2) == sizeof(char) )
															|| (sizeof(T1) == sizeof(wchar_t) &&
															    sizeof(T2) == sizeof(wchar_t) ))> {};


	}
}
