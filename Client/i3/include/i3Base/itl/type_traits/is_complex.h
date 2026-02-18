#pragma once

#include "is_convertible.h"
#include <complex>

namespace i3
{
	namespace detail
	{
		struct is_convertible_from_tester
		{
			template<class T> is_convertible_from_tester(const std::complex<T>&);	// »ý¼ºÀÚ..
		};
	}

	template<class T> struct is_complex : is_convertible<T, detail::is_convertible_from_tester> {};
}
