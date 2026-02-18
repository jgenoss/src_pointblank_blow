#pragma once

#include "../utility/blank.h"
#include "ft_is_callable_builtin.h"

namespace i3
{
	namespace ft
	{
		template<class T> 
		struct function_arity : mpl::if_< is_callable_builtin<T>, typename components<T>::function_arity, blank>::type {};

		template<class T>
		struct safe_function_arity : components<T>::function_arity {};

	}
}

