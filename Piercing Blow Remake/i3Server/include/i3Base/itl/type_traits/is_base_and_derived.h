#pragma once

#include "remove_cv.h"
#include "is_same.h"

namespace i3
{
	namespace detail
	{
		template <class B, class D>
		struct is_base_and_derived_impl
		{
			typedef typename remove_cv<B>::type ncvB;
			typedef typename remove_cv<D>::type ncvD;
			static const bool value = (__is_base_of(B,D) && !is_same<ncvB,ncvD>::value);
		};
	}

	template<class base, class derived>
	struct is_base_and_derived : integral_constant<bool, detail::is_base_and_derived_impl<base, derived>::value> {};
	
	template<class base, class derived> struct is_base_and_derived<base&, derived> : false_type {}; 
	template<class base, class derived> struct is_base_and_derived<base, derived&> : false_type {}; 
	template<class base, class derived> struct is_base_and_derived<base&, derived&> : false_type {}; 

}
