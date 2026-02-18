#pragma once

#include "ft_function_type.h"
#include "ft_function_pointer.h"
#include "ft_function_reference.h"
#include "ft_member_function_pointer.h"
#include "ft_is_function.h"
#include "ft_is_function_pointer.h"
#include "ft_is_function_reference.h"
#include "ft_is_member_function_pointer.h"

#include "../mpl/mpl_transform_view.h"
#include "../mpl/mpl_joint_view.h"

//
// 모든 종류의 함수타입 인수를 메타함수로 변경한뒤 새 함수형으로 리턴...
// 
//
namespace i3
{
	namespace ft
	{
		template<class F, class ParameterTransform>
		struct modify_function_parameters
		{
			typedef components<F>									components_type;
			typedef typename components_type::types					sequence_type;
			typedef typename mpl::begin<sequence_type>::type		begin;
			typedef typename mpl::next<begin>::type					next;
			typedef typename mpl::eval_if< is_member_pointer<F>, mpl::next<next>, mpl::identity<next> >::type	param_begin;
			typedef typename mpl::end<sequence_type>::type														param_end;
			
			typedef mpl::iterator_range<begin, param_begin>		non_param_range;
			typedef mpl::iterator_range<param_begin, param_end> param_range;
			typedef mpl::transform_view< param_range, ParameterTransform>	params_transforms;
			typedef mpl::joint_view<non_param_range, params_transforms>		result_sequence;

			typedef typename mpl::multi_if< is_function<F>, function_type< result_sequence, components_type>,
											is_function_pointer<F>, function_pointer<result_sequence, components_type>,
											is_function_reference<F>, function_reference<result_sequence, components_type>,
											is_member_function_pointer<F>, member_function_pointer<result_sequence, components_type>,
											mpl::identity<F> >::type		synthesize_type;
			
			typedef typename synthesize_type::type		type;
		};

	}
}
