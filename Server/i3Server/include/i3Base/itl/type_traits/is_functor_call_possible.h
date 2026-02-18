#if !defined(pp_is_iterating)


#ifndef is_functor_call_possible_h
#define is_functor_call_possible_h

// 원본 출처는 데이브아브라함스의 메일내용 : 러시아쪽 출처
// http://groups.google.com/group/comp.lang.c++.moderated/msg/e5fbc9305539f699
// can be called와 유사하나, 리턴타입과 인수를 모두 갖춘 함수형으로 점검하므로 더 일반적...
// (functor_can_be_called는 리턴타입을 요구하지 않음)
// 함수객체의 호출 오퍼레이터를 질의한다...

#define  is_functor_call_possible_limit		20

#include "has_functor_op.h"
#include "detail/is_mem_call_possible_common.h"

namespace i3
{
	namespace detail
	{
	
		template<class T>
		struct is_functor_call_possible_impl_base
		{
			struct type_wrap : public T 
			{
				using T::operator();	no_type  operator()(...) const;
			};
			
			typedef typename clone_constness<T, type_wrap>::type		test_type; 
		};

		template<class T, class F>
		struct is_functor_call_possible_impl : false_type {};

	}
}

#define pp_iteration_params_1 (3, (0, is_functor_call_possible_limit, "is_functor_call_possible.h") )		// 0부터 반복임..
#include pp_iterate()

namespace i3
{
	// has_functor_op에서 참조형을 걸러내기 때문에, 다른부분에서 참조형 회피를 할 필요성은 없다..

	template<class T, class F>
	struct is_functor_call_possible : mpl::and_< has_functor_op<T>, detail::is_functor_call_possible_impl<T, F> > {};

}

#endif // is_functor_call_possible_h

#else // pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		
		template<class T, class R pp_enum_trailing_params(i_, class P) >
		struct is_functor_call_possible_impl<T, R (pp_enum_params(i_,P)) > : is_functor_call_possible_impl_base<T>
		{
			#define pp_param_repeat(z, n, d)	*(typename add_pointer<pp_cat(P, n)>::type)0			

			static const bool value = sizeof(yes_type) == sizeof(
				return_value_check<R>::test( (
				static_cast<test_type*>(0)->operator()( pp_enum(i_, pp_param_repeat,~) ), void_exp_result() ) ) );
			#undef pp_param_repeat
			
			typedef mpl::bool_<value>			type;

		};
	}
}

#undef	i_

#endif // pp_is_iterating
