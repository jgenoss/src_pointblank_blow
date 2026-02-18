#if !defined(pp_is_iterating)


#ifndef is_mem_call_possible_xxx_h
#define is_mem_call_possible_xxx_h

// 원본 출처는 데이브아브라함스의 메일내용 : 러시아쪽 출처
// http://groups.google.com/group/comp.lang.c++.moderated/msg/e5fbc9305539f699
// can be called와 유사하나, 리턴타입과 인수를 모두 갖춘 함수형으로 점검하므로 더 일반적...
// (functor_can_be_called는 리턴타입을 요구하지 않음)
// 함수객체의 호출 오퍼레이터를 질의한다...
#include "detail/is_mem_call_possible_common.h"
#include "../preprocessor/pp_enum.h"
#include "../preprocessor/repetition/pp_enum_trailing_params.h"

#include "add_pointer.h"

#define  is_mem_call_possible_xxx_limit		10

#endif // is_member_call_pec_h

// no include guard..

//
#define		pp_trait_name		pp_tuple_elem(3, 0, is_mem_call_possible_xxx)
#define		pp_member_name		pp_tuple_elem(3, 1, is_mem_call_possible_xxx)
#define		pp_has_trait		pp_tuple_elem(3, 2, is_mem_call_possible_xxx)		// has류의 특질을 요구함..
//

namespace i3
{
	namespace detail
	{
	
		template<class T>
		struct pp_cat(pp_trait_name,_impl_base)
		{
			struct type_wrap : public T 
			{
				using T::pp_member_name;	no_type  pp_member_name(...) const;
			};
			
			typedef typename clone_constness<T, type_wrap>::type		test_type; 
		};

		template<class T, class F>
		struct pp_cat(pp_trait_name,_impl) : false_type {};

	}
}

#define pp_iteration_params_1 (3, (0, is_mem_call_possible_xxx_limit, "is_mem_call_possible_xxx.h") )		// 0부터 반복임..
#include pp_iterate()

namespace i3
{
	// has_trait에서 참조형을 걸러내기 때문에, 다른부분에서 참조형 회피를 할 필요성은 없다..
	template<class T, class F>
	struct pp_trait_name : mpl::and_< pp_has_trait<T>, detail::pp_cat(pp_trait_name,_impl)<T, F> > {};
}


#undef		pp_has_trait
#undef		pp_member_name
#undef		pp_trait_name
#undef		is_mem_call_possible_xxx

#else // pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		
		template<class T, class R pp_enum_trailing_params(i_, class P) >
		struct pp_cat(pp_trait_name,_impl)<T, R (pp_enum_params(i_,P)) > : pp_cat(pp_trait_name,_impl_base)<T>
		{
			#define pp_param_repeat(z, n, d)	*(typename add_pointer<pp_cat(P, n)>::type)0			

			static const bool value = sizeof(yes_type) == sizeof(
				return_value_check<R>::test( (
				static_cast<test_type*>(0)->pp_member_name( pp_enum(i_, pp_param_repeat,~) ), void_exp_result() ) ) );
			#undef pp_param_repeat
			
			typedef mpl::bool_<value>			type;

		};
	}
}

#undef	i_

#endif // pp_is_iterating
