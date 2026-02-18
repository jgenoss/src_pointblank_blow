#pragma once

// 함수가 아닌 것을 함수형으로 전환한다..
// 함수형이 아닌 함수타입들도 함수형으로 전환한다..
#include "ft_function_type.h"
#include "../mpl/mpl_single_view.h"

namespace i3
{
	namespace ft
	{
		
		template<class T>
		struct any_to_function_type 
		{
			typedef typename mpl::eval_if<	is_callable_builtin<T>,	
											function_type<T>, 
											function_type< mpl::single_view<T> > >::type type;
		};
	}
}
