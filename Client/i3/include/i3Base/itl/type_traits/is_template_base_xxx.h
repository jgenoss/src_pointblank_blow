
// no include guard

#define		pp_trait_name				pp_tuple_elem(3, 0, is_template_base_xxx)
#define		pp_full_template_name		pp_tuple_elem(3, 1, is_template_base_xxx)
#define		pp_type_arity				pp_tuple_elem(3, 2, is_template_base_xxx)

// 
namespace i3
{
	namespace detail
	{
		// 전형적인 변환트릭을 사용한다...
		
		template<class T>
		struct pp_cat(pp_trait_name,_impl)
		{
			static T		MakeFrom();
			
			template< pp_enum_params(pp_type_arity, class U)>
			static yes_type test( pp_full_template_name< pp_enum_params(pp_type_arity, U) >, 
				typename enable_if<is_base_of<pp_full_template_name< pp_enum_params(pp_type_arity, U) >, T>, int>::type = 0);
			static no_type	test(...);
			
			static const bool value =	sizeof(yes_type) == sizeof( test(MakeFrom()) );

			typedef mpl::bool_<value> type;
		};
	}

	template<class T>
	struct pp_trait_name : mpl::and_< is_class_or_union<T>, detail::pp_cat(pp_trait_name,_impl)<T> > {};
		
}

#undef pp_type_arity
#undef pp_full_template_name
#undef pp_trait_name
#undef is_template_base_xxx

