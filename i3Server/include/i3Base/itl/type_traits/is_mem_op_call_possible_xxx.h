
#ifndef is_mem_op_call_possible_xxx_h
#define is_mem_op_call_possible_xxx_h

// 원본 출처는 데이브아브라함스의 메일내용 : 러시아쪽 출처
// http://groups.google.com/group/comp.lang.c++.moderated/msg/e5fbc9305539f699
// can be called와 유사하나, 리턴타입과 인수를 모두 갖춘 함수형으로 점검하므로 더 일반적...

#include "detail/is_mem_call_possible_common.h"
#include "../template_utility/ignore_copy.h"

#endif // is_mem_op_call_possible_xxx_h

// no include guard..

//
#define		pp_trait_name		pp_tuple_elem(4, 0, is_mem_op_call_possible_xxx)
#define		pp_member_name		pp_tuple_elem(4, 1, is_mem_op_call_possible_xxx)
#define		pp_has_trait		pp_tuple_elem(4, 2, is_mem_op_call_possible_xxx)		// has류의 특질을 요구함..
#define		pp_fixed_arity		pp_tuple_elem(4, 3, is_mem_op_call_possible_xxx)	
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
				using T::pp_member_name;	no_type  pp_member_name( pp_enum_datas(pp_fixed_arity, ignore_copy) ) const;
			};
			
			typedef typename clone_constness<T, type_wrap>::type		test_type; 
		};

		template<class T, class F>
		struct pp_cat(pp_trait_name,_impl) : false_type {};

	}
}

namespace i3
{
	// has_trait에서 참조형을 걸러내기 때문에, 다른부분에서 참조형 회피를 할 필요성은 없다..
	template<class T, class F>
	struct pp_trait_name : mpl::and_< pp_has_trait<T>, detail::pp_cat(pp_trait_name,_impl)<T, F> > {};
}


namespace i3
{
	namespace detail
	{
		
		template<class T, class R pp_enum_trailing_params(pp_fixed_arity, class P) >
		struct pp_cat(pp_trait_name,_impl)<T, R (pp_enum_params(pp_fixed_arity,P)) > : pp_cat(pp_trait_name,_impl_base)<T>
		{
			#define pp_param_repeat(z, n, d)	*(typename add_pointer<pp_cat(P, n)>::type)0			

			static const bool value = sizeof(yes_type) == sizeof(
				return_value_check<R>::test( (
				static_cast<test_type*>(0)->pp_member_name( pp_enum(pp_fixed_arity, pp_param_repeat,~) ), void_exp_result() ) ) );
			#undef pp_param_repeat
			
			typedef mpl::bool_<value>			type;

		};
	}
}


#undef		pp_fixed_arity
#undef		pp_has_trait
#undef		pp_member_name
#undef		pp_trait_name
#undef		is_mem_op_call_possible_xxx
