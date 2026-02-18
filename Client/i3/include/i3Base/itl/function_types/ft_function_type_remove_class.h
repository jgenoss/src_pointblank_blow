#pragma once

#include "ft_function_type.h"
#include "../mpl/mpl_erase.h"

namespace i3
{
	namespace ft
	{
		namespace detail
		{
			template<class Components, class Tag>
			struct function_type_remove_class_impl
			{
				typedef	typename mpl::next<typename mpl::begin<Components>::type>::type	erase_iter;
				typedef typename mpl::erase<Components, erase_iter>::type				res_seq;
				typedef typename function_type<res_seq, Tag>::type						type;
			};
		}

		template<class Types, class Tag = null_tag>				// 순차열/Components/함수 .. (순차열을 먼저 전환해 버리면 클래스타입을 알수없음)
		struct function_type_remove_class
		{
			typedef typename mpl::eval_if<	is_callable_builtin<Types>,
									function_type_remove_class< components<Types>, Tag >,
									function_type<Types, Tag> >::type type;
		};
		
		template<class T, class U, class Tag>
		struct function_type_remove_class< components<T, U>, Tag >
		{
			typedef typename mpl::eval_if<	represents<components<T, U>, member_pointer_tag>, 
							detail::function_type_remove_class_impl<components<T, U>, Tag >,
							function_type< components<T, U>, Tag> >::type type;
		};

	}
}	