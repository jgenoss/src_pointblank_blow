#pragma once

#include "is_member_pointer.h"
#include "remove_cv.h"
#include "integral_constant.h"
#include "../mpl/mpl_and.h"
#include "../mpl/mpl_not.h"

namespace i3
{
	namespace detail
	{

		template <class T> struct is_pointer_helper : false_type {};
		template <class T> struct is_pointer_helper<T*> : true_type	{};

		template<class T> struct is_pointer_impl : 
			integral_constant<bool, mpl::and_< is_pointer_helper< typename remove_cv<T>::type >, mpl::not_<is_member_pointer<T> > >::value> {};
	}
	
	template<class T> struct is_pointer : detail::is_pointer_impl<T> {};

}
