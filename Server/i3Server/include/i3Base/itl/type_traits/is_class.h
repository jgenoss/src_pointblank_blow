#pragma once

#include "is_union.h"
#include "integral_constant.h"
#include "yes_no_type.h"

#include "../mpl/mpl_and.h"
#include "../mpl/mpl_not.h"

namespace i3
{
	namespace detail
	{
		template <class T> static yes_type is_class_tester(void(T::*)(void));
		template <class T> static no_type is_class_tester(...);
	}

	template <class T> struct is_class_or_union : 
		integral_constant<bool, (sizeof(detail::is_class_tester<T>(0)) == sizeof(yes_type) ) > {};

//	template <class T> struct is_class : 
//		integral_constant<bool, mpl::and_<is_class_or_union<T>, mpl::not_< is_union<T> > >::value > {};
	
	template <class T> struct is_class : integral_constant<bool, __is_class(T)> {};

		
}
