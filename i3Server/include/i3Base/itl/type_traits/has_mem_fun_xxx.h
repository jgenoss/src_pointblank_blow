
// no include guard

//
//  멤버함수이며, 인수의 갯수만 받아들여 확인함...(멤버함수 판단과 인수의 갯수로 판별..)
//  그리고, has_mem_xxx가 선행되야함...
// 

#define		pp_trait_name		pp_tuple_elem(4, 0, has_mem_fun_xxx)
#define		pp_member_name		pp_tuple_elem(4, 1, has_mem_fun_xxx)
#define		pp_fixed_arity		pp_tuple_elem(4, 2, has_mem_fun_xxx)
#define		pp_has_mem_trait	pp_tuple_elem(4, 3, has_mem_fun_xxx)

#pragma warning(disable : 4510 4610)

namespace i3
{
	namespace detail
	{
		template<class T>
		struct pp_cat(pp_trait_name,_impl)
		{
			template<class U, class R pp_enum_trailing_params(pp_fixed_arity, class A)>
			static yes_type test( R (U::*)( pp_enum_params(pp_fixed_arity, A) ) );
			
			template<class U, class R pp_enum_trailing_params(pp_fixed_arity, class A)>
			static yes_type test( R (U::*)( pp_enum_params(pp_fixed_arity, A) ) const );
			
			template<class U> static no_type  test(U);
			
			static const bool value = sizeof(yes_type) == sizeof(test(&T::pp_member_name));
			typedef mpl::bool_<value>		type;
		};
	}
	
	template<class T>
	struct pp_trait_name : mpl::and_< pp_has_mem_trait<T>, detail::pp_cat(pp_trait_name,_impl)<T> > {};
		
}

#pragma warning(default : 4510 4610)

#undef pp_has_mem_trait
#undef pp_fixed_arity
#undef pp_member_name
#undef pp_trait_name
#undef has_mem_fun_xxx


