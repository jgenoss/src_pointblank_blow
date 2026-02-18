#pragma once

#include "integral_constant.h"

namespace i3
{
#pragma warning(push)
#pragma warning(disable : 4121 4512)	// alignment is sensitive to packing

	namespace detail 
	{
#pragma warning(push)
#pragma warning(disable : 4324)			// structure was padded due to __declspec(align())
		template <typename T>
		struct alignment_of_hack {	char c;	T t; alignment_of_hack(); };
#pragma warning(pop)

		template <unsigned val1, unsigned val2>
		struct alignment_logic 	{	static const std::size_t value = ( val1 < val2 ) ? val1 : val2;	 }; 	// 더 작은 값을 선택..
	}

	template <typename T>
	struct alignment_of : integral_constant<std::size_t, 
		detail::alignment_logic< sizeof(detail::alignment_of_hack<T>) - sizeof(T), __alignof(T) >::value > {};

	template <typename T> struct alignment_of<T&> : alignment_of<T*> {};
	template <> struct alignment_of<void> : integral_constant<std::size_t, 0> {};
	template <> struct alignment_of<void const> : integral_constant<std::size_t, 0> {};
	template <> struct alignment_of<void volatile> : integral_constant<std::size_t, 0> {};
	template <> struct alignment_of<void const volatile> : integral_constant<std::size_t, 0> {};

#pragma warning(pop)

}
