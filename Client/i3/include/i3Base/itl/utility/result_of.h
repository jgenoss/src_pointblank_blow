#pragma once

#define result_of_arity_limit 10

#include "../type_traits/has_result_type.h"
#include "../preprocessor/repetition/pp_enum_trailing_params.h"

//
//  함수객체, 함수포인터와 함께 오버로딩될 인수를 넣으면 리턴타입을 알려주는 메타traits..
//  tr1은 제약이 있어서, 그다지 유용하지 않을것 같다...
//

namespace i3
{
	template<class F> struct result_of;
	template<class F> struct tr1_result_of;

	namespace detail
	{
		template<class F> struct result_of_void_impl { typedef void type; };
		template<class R> struct result_of_void_impl< R (*)() > { typedef R type; };
		template<class R> struct result_of_void_impl< R (&)() > { typedef R type; }; 

		template<class F, class FArgs, bool HasResultType> struct tr1_result_of_impl;

		template<class F, class FArgs> 
		struct tr1_result_of_impl<F, FArgs, true>  {	typedef typename F::result_type type; };	// result_type존재시 이것을 리턴.

		template<class FArgs> struct is_function_with_no_args : mpl::false_ {};
		template<class F> struct is_function_with_no_args< F(void) > : mpl::true_ {};

		template<class F, class FArgs>
		struct result_of_nested_result : F::template result<FArgs> {};
		
		template<class F, class FArgs>
		struct tr1_result_of_impl<F, FArgs, false> :	mpl::if_< is_function_with_no_args<FArgs>, result_of_void_impl<F>, 
			result_of_nested_result<F, FArgs> >::type {};
	}

}

#define pp_iteration_params_1 (3, (0, result_of_arity_limit, "detail/result_of_iterate.h")) 
#include pp_iterate()
