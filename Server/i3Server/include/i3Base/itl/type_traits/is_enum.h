#pragma once

#include "is_convertible.h"
#include "add_reference.h"
#include "is_arithmetic.h"
#include "is_reference.h"
#include "is_class.h"
#include "is_array.h"

#include "../mpl/mpl_if.h"
#include "../mpl/mpl_or.h"

namespace i3
{
	namespace detail
	{
		struct int_convertible { int_convertible(int); };

		template<class T>
		struct is_enum_helper : is_convertible< typename add_reference<T>::type, int_convertible > {};
	}

	template <class T>
	struct is_enum : mpl::if_< 
						mpl::or_< is_arithmetic<T>, is_reference<T>, is_class_or_union<T>, is_array<T> >, 
						false_type, detail::is_enum_helper<T> >::type {};


	template <> struct is_enum<void> : false_type {};
	template <> struct is_enum<void const> : false_type {};
	template <> struct is_enum<void volatile> : false_type {};
	template <> struct is_enum<void const volatile> : false_type {};
	
}