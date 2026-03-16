

// no include guard

#define		pp_trait_name		pp_tuple_elem(3, 0, is_binary_op_xxx)
#define		pp_op_name			pp_tuple_elem(3, 1, is_binary_op_xxx)
#define		pp_op_result_type	pp_tuple_elem(3, 2, is_binary_op_xxx)

namespace i3
{
	namespace detail
	{
		namespace pp_cat(pp_trait_name,_ns)
		{	
			struct tag{};	
			struct any { template <class T> any(const T&); };
		
			tag operator pp_op_name ( const any&, const any& );

			yes_type		pp_cat(is_binary_op_, pp_op_result_type)(pp_op_result_type);
			no_type			pp_cat(is_binary_op_, pp_op_result_type)(...);
			
			template<class T, class U>
			struct pp_cat(pp_trait_name, _impl)
			{
				static T make_T();
				static U make_U();
				static const bool value = sizeof( pp_cat(is_binary_op_, pp_op_result_type)( make_T() pp_op_name make_U() )) == sizeof(yes_type);
			};
		}
	}
	
	template<class T, class U>
	struct pp_trait_name : integral_constant<bool, detail::pp_cat(pp_trait_name,_ns)::pp_cat(pp_trait_name, _impl)<T, U>::value > {};
	
}

#undef pp_op_result_type
#undef pp_op_name
#undef pp_trait_name
#undef is_binary_op_xxx
