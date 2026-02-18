
// no include guard

#define		pp_trait_name		pp_tuple_elem(3, 0, has_mem_op_xxx)
#define		pp_member_name		pp_tuple_elem(3, 1, has_mem_op_xxx)
#define		pp_fixed_arity		pp_tuple_elem(3, 2, has_mem_op_xxx)

#pragma warning(disable : 4510 4610)

namespace i3
{
	namespace detail
	{
		template<class T>
		struct pp_cat(pp_trait_name,_impl)
		{
			struct base_mixin { void pp_member_name( pp_enum_datas(pp_fixed_arity, const base_mixin&) ){} };
			struct functor_wrap : public T, public base_mixin {};
			
			template<class U, U t> class helper;
			template<class U> 
			static no_type	test(U*, helper<void (base_mixin::*)( pp_enum_datas(pp_fixed_arity, const base_mixin&) ), &U::pp_member_name>* = 0);
			static yes_type test(...);
			static const bool value = (sizeof(yes_type) == sizeof(test(static_cast<functor_wrap*>(0))));
			typedef mpl::bool_<value>		type;
		};

	}

	template<class T>
	struct pp_trait_name : mpl::and_< is_class_or_union<T>, detail::pp_cat(pp_trait_name,_impl)<T> > {};
}


#pragma warning(default : 4510 4610)

#undef pp_fixed_arity
#undef pp_member_name
#undef pp_trait_name
#undef has_mem_op_xxx
