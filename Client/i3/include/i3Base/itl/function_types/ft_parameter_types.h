#pragma once

#include "../mpl/mpl_pop_front.h"
#include "../utility/blank.h"
#include "ft_is_callable_builtin.h"

// parameter_types의 경우, 그 자체가 순차열이므로 리턴type이 꼭 필요한 것이 아님을 유의할 것...

namespace i3
{
	namespace ft
	{
		using mpl::placeholders::_;

		template< class T, class ClassTypeTransform = add_reference<_> >
		struct parameter_types;

		namespace detail
		{
		  template<class T, class ClassTypeTransform> 
		  struct parameter_types_impl : mpl::pop_front< typename components<T,ClassTypeTransform>::types >::type {};
		}

		template<class T, class ClassTypeTransform> 
		struct parameter_types : mpl::if_<	is_callable_builtin<T>, 
											detail::parameter_types_impl<T,ClassTypeTransform>, 
											blank>::type {};

	}
}