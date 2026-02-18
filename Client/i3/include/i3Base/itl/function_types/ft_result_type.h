#pragma once

#include "../utility/blank.h"
#include "ft_is_callable_builtin.h"
#include "../mpl/mpl_at.h"

namespace i3
{
	namespace ft
	{
		template< class T > struct result_type;

		namespace detail
		{
			template<class T> struct result_type_impl : mpl::at_c< typename components<T>::types, 0 > {};
		}

		template<class T> struct result_type : mpl::if_<is_callable_builtin<T>, 
														detail::result_type_impl<T>, 
														blank >::type {};
	}
}

