#pragma once

#include "ft_is_callable_builtin.h"

namespace i3
{
	namespace ft
	{
		template<class T>
		struct to_sequence 
		{
			typedef typename mpl::eval_if<	is_callable_builtin<T>, 
											to_sequence< components<T> >,
											mpl::identity<T> >::type	type;
		};

		template<class T, class U>
		struct to_sequence< components<T, U> >
		{
			typedef typename components<T, U>::types	type;		// mpl::vector_n으로 전환
		};
	}
}
