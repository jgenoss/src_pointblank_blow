
#ifndef is_fun_op_call_possible_xxx_h
#define is_fun_op_call_possible_xxx_h

#include "detail/is_fun_call_possible_common.h"

#endif // is_fun_op_call_possible_xxx_h

// no include guard !!

#define     pp_trait_name		pp_tuple_elem(3, 0, is_fun_op_call_possible_xxx)
#define		pp_function_name	pp_tuple_elem(3, 1, is_fun_op_call_possible_xxx)
#define		pp_fixed_arity		pp_tuple_elem(3, 2, is_fun_op_call_possible_xxx)


namespace i3
{
	namespace detail
	{
		namespace pp_cat(pp_trait_name,_ns)
		{
			struct any { template <class T> any(const T&); };
		
			no_type		pp_function_name( pp_enum_datas(pp_fixed_arity, any) ); 
		
			template<class F>
			struct pp_cat(pp_trait_name, _impl) : false_type {};
		}
	}
	
	template<class F>
	struct pp_trait_name : detail::pp_cat(pp_trait_name,_ns)::pp_cat(pp_trait_name, _impl)<F> {};
}

namespace i3
{
	namespace detail
	{
		namespace pp_cat(pp_trait_name,_ns)
		{
			template<class R pp_enum_trailing_params(pp_fixed_arity, class P)>
			struct pp_cat(pp_trait_name, _impl)<R (pp_enum_params(pp_fixed_arity,P))> 
			{
				
				#define pp_param_repeat(z, n, d)	*(typename add_pointer<pp_cat(P, n)>::type)0

				static const bool value = sizeof(yes_type) == sizeof(
					return_value_check<R>::test( (
					pp_function_name( pp_enum(pp_fixed_arity, pp_param_repeat,~) ), void_exp_result() ) ) );
				#undef pp_param_repeat

				typedef mpl::bool_<value>		type;
			};
		}
	}
}

#undef		pp_fixed_arity	
#undef		pp_function_name
#undef		pp_trait_name
#undef		is_fun_op_call_possible_xxx
