#pragma once

#include "is_base_and_derived.h"
#include "is_class.h"
#include "ice_or.h"
#include "ice_and.h"

namespace i3
{
	namespace detail
	{
		template<class B, class D>
		struct is_base_of_impl
		{
			typedef typename remove_cv<B>::type ncvB;
			typedef typename remove_cv<D>::type ncvD;
			static const bool value = ice_or<
				detail::is_base_and_derived_impl<ncvB, ncvD>::value, 
				ice_and<is_same<ncvB, ncvD>::value, is_class<ncvB>::value>::value >::value;
		};
	}

	template<class base, class derived>
	struct is_base_of : integral_constant<bool, detail::is_base_of_impl<base, derived>::value> {};

	template<class base, class derived> struct is_base_of<base&, derived> : false_type {}; 
	template<class base, class derived> struct is_base_of<base, derived&> : false_type {}; 
	template<class base, class derived> struct is_base_of<base&, derived&> : false_type {}; 

}