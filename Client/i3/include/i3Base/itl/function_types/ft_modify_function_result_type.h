#pragma once

#include "ft_result_type.h"
#include "ft_parameter_types.h"

#include "ft_function_type_joint.h"
#include "ft_function_pointer_joint.h"
#include "ft_function_reference_joint.h"
#include "ft_member_function_pointer_joint.h"
#include "ft_member_object_pointer_joint.h"
#include "ft_is_function.h"
#include "ft_is_function_pointer.h"
#include "ft_is_function_reference.h"
#include "ft_is_member_function_pointer.h"
#include "ft_is_member_object_pointer.h"
#include "../mpl/mpl_apply.h"

namespace i3
{
	namespace ft
	{
		template<class F, class ResultTransform>
		struct modify_function_result_type
		{
			typedef components<F>									components_type;
			typedef typename result_type<F>::type					old_result_type;
			typedef typename parameter_types<F>::type				parameter_types;
			typedef typename mpl::apply1<ResultTransform, old_result_type>::type	new_result_type;

			typedef typename mpl::multi_if< 
				is_function<F>, function_type_joint< new_result_type, parameter_types, components_type>,
				is_function_pointer<F>, function_pointer_joint< new_result_type, parameter_types, components_type>,
				is_function_reference<F>, function_reference_joint< new_result_type, parameter_types, components_type>,
				is_member_function_pointer<F>, member_function_pointer_joint< new_result_type, parameter_types, components_type>,
				is_member_object_pointer<F>, member_object_pointer_joint<new_result_type, parameter_types>,
				mpl::identity<F> >::type		synthesize_type;

			typedef typename synthesize_type::type		type;
		};
	}
}