#pragma once


#include "../../function_types/ft_parameter_types.h"
#include "../../function_types/ft_function_type_joint.h"


namespace i3
{
	namespace detail_any_function
	{

		template<class F>
		struct conv_fun
		{
			typedef typename i3::ft::parameter_types<F, add_pointer<mpl::_> >		param_sequence;
			typedef typename i3::ft::result_type<F>::type							result_type;
			typedef typename i3::ft::function_type_joint<result_type, param_sequence>::type type;
		};

		template<class FT, std::size_t idx>
		struct 	get_elem
		{
			typedef typename i3::ft::parameter_types<FT>		param_sequence; 			
			typedef typename mpl::at_c<param_sequence, idx>::type	type;
		};

		template<class FT>
		struct 	get_elem2
		{
			template<std::size_t idx>
			struct apply
			{
				typedef typename i3::ft::parameter_types<FT>		param_sequence; 			
				typedef typename mpl::at_c<param_sequence, idx>::type	type;
			};
		};

		template<class FT, std::size_t idx>
		struct 	get_param
		{
			typedef typename get_elem<FT, idx>::type					element_type;
			typedef typename i3::make_parameter<element_type>::type		type;
		};

		template<class FT>
		struct get_param2
		{
			template<std::size_t idx>
			struct apply 
			{
				typedef typename get_elem<FT, idx>::type					element_type;
				typedef typename i3::make_parameter<element_type>::type		type;
			};
		};

		template<class FT>
		struct is_mem_fun_class_pointer
		{
			typedef typename get_elem<FT, 0>::type				element_type;
			typedef typename i3::is_pointer<element_type>::type	type;
			static const bool value = type::value;
		};
	}
}