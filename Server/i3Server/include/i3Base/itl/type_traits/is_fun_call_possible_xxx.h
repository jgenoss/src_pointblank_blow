
#if !defined(pp_is_iterating)

#ifndef is_fun_call_possible_xxx_h
#define is_fun_call_possible_xxx_h

#include "detail/is_fun_call_possible_common.h"

#define is_fun_call_possible_xxx_limit 20

#endif // is_fun_call_possible_xxx_h

// no include guard !!


#define     pp_trait_name		pp_tuple_elem(2, 0, is_fun_call_possible_xxx)
#define		pp_function_name	pp_tuple_elem(2, 1, is_fun_call_possible_xxx)


namespace i3
{

	namespace detail
	{
		no_type pp_function_name(...);			// 아무런 인자나...

		template<class F>
		struct pp_cat(pp_trait_name, _impl) : false_type {};
	}
	
}


#define pp_iteration_params_1 (3, (0, is_fun_call_possible_xxx_limit, "is_fun_call_possible_xxx.h") )		// 0부터 반복임..
#include pp_iterate()

namespace i3
{
	template<class F>
	struct pp_trait_name : detail::pp_cat(pp_trait_name, _impl)<F> {};
}


#undef		pp_function_name
#undef		pp_trait_name
#undef		is_fun_call_possible_xxx


#else // pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<class R pp_enum_trailing_params(i_, class P)>
		struct pp_cat(pp_trait_name, _impl)<R (pp_enum_params(i_,P))> 
		{
			friend  no_type pp_function_name(...);

			#define pp_param_repeat(z, n, d)	*(typename add_pointer<pp_cat(P, n)>::type)0

			static const bool value = sizeof(yes_type) == sizeof(
				return_value_check<R>::test( (
				pp_function_name( pp_enum(i_, pp_param_repeat,~) ), void_exp_result() ) ) );
			#undef pp_param_repeat
			
			typedef mpl::bool_<value>		type;
		};
	}
}

#undef	i_

#endif // pp_is_iterating
