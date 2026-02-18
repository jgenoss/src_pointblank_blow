#pragma once

#include "is_member_pointer.h"

namespace i3
{
	namespace detail
	{

		template<class T>
		struct is_member_object_pointer_impl
		{
			static const bool value = mpl::and_< is_member_pointer<T>, mpl::not_< is_member_function_pointer<T> > >::value;
		};

	}
	template<class T>
	struct is_member_object_pointer : integral_constant<bool, detail::is_member_object_pointer_impl<T>::value> {};

}
