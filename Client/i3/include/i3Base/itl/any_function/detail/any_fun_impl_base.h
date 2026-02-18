#pragma once

#include "any_fun.h"
#include "../../preprocessor/pp_repeat_from_to.h"

namespace i3
{
	namespace detail
	{
		template<class FT>
		struct any_fun_impl_base;

		template<class R>
		struct any_fun_impl_base< R (void) > : any_fun 
		{ 
			typedef R										result_type;
			virtual R operator()() const = 0; 
			virtual const std::type_info&	function_typeid() const { return typeid(R (void));  }
		};
		
		template<class R, class A0>
		struct any_fun_impl_base< R (A0) > : any_fun 
		{ 
			typedef R										result_type;
			typedef typename i3::make_parameter<A0>::type	P0;
			virtual R operator()(P0) const = 0;
			virtual const std::type_info&	function_typeid() const { return typeid(R (A0));  }
		};
		
#define any_fun_impl_base_repeat_fn(z, n, d) \
		template<class R, pp_enum_params(n, class A)> \
		struct any_fun_impl_base< R ( pp_enum_params(n,A) ) > : any_fun \
		{ \
			typedef R										result_type; \
			pp_repeat_binary_params_with_a_cat(n, typedef typename i3::make_parameter<A,>::type P, ;) \
			virtual R operator()(pp_enum_params(n,P)) const = 0; \
			virtual const std::type_info&	function_typeid() const { return typeid( R (pp_enum_params(n, A)) );  } \
		};

		pp_repeat_from_to(2, pp_inc(ft_arity_limit), any_fun_impl_base_repeat_fn, ~)
#undef any_fun_impl_base_repeat_fn		


	}

}

