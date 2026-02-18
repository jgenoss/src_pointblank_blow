#if !defined(pp_is_iterating)

#ifndef any_fun_h
#define any_fun_h

#include "../../preprocessor/ext/pp_enum_params_cat_params.h"
#include "../../preprocessor/repetition/pp_enum_trailing_params.h"
#include "../../type_traits/make_nonconst_referred.h"
#include "../../mpl/mpl_multi_if.h"

#include "../../function_types/ft_is_member_function_pointer.h"
#include "../../function_types/ft_is_function_pointer.h"
#include "../../function_types/ft_function_arity.h"
#include "../../type_traits/is_functor_call_possible.h"

namespace i3
{
	namespace detail
	{

		class any_fun : public i3::class_common_pool<any_fun>
		{
		public:
			virtual any_fun*				clone() const = 0;
			virtual const std::type_info&	function_typeid() const = 0;

			virtual ~any_fun() {}
			
			template<class R>
			R invoke() const;

			template<class R, class P0>
			R invoke(typename i3::make_parameter<P0>::type p0) const;			// 인수추론을 쓰면 안된다. 
			
#define any_fun_repeat_fn(z, n, d) \
			template<class R, pp_enum_params(n, class P)> \
			R invoke( pp_enum_params_cat_params(n, typename i3::make_parameter<P, >::type, p) ) const;
			pp_repeat_from_to(2, pp_inc(ft_arity_limit), any_fun_repeat_fn, ~)
#undef any_fun_repeat_fn


		};
	

		template<class F, class FT> __forceinline
		any_fun*	create_any_fun(F f)
		{
			typedef typename i3::make_nonconst_referred<F>::type	test_type;
			typedef typename i3::unwrap_reference<test_type>::type	unwrap_type;

			typedef typename mpl::multi_eval_if< 

				ft::is_member_function_pointer<test_type, ft::non_variadic>, 
				mpl::if_< detail_any_function::is_mem_fun_class_pointer<FT>, detail::any_function_mem_fun_ptr_tag, 
				detail::any_function_mem_fun_ref_tag>,
				ft::is_function_pointer<test_type, ft::non_variadic>, i3::identity< detail::any_function_ptr_tag>, 
				mpl::and_<is_reference_wrapper<test_type>, is_functor_call_possible<test_type, FT> >, 
				i3::identity<detail::any_function_obj_ref_tag>,
				is_functor_call_possible<test_type, FT>, i3::identity< detail::any_function_ptr_tag>,
				i3::identity<detail::any_function_error_tag> >::type		tag;

			compile_assert_pred( ( mpl::not_< is_same< tag, detail::any_function_error_tag> > ) );

			typedef typename ft::result_type<FT>::type		R;
			static const std::size_t arity = ft::function_arity<FT>::value;

			return new detail::any_fun_impl< test_type, FT, tag, arity>(f); 
		}

	}
	
}

#define pp_iteration_params_1 (3, (0, ft_arity_limit, "any_fun.h") )
#include pp_iterate()

#endif		// any_fun_h

#else		// pp_is_iterating

#define i_						pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<class R pp_enum_trailing_params(i_, class P)>	__forceinline
		R any_fun::invoke( pp_enum_params_cat_params( i_, typename i3::make_parameter<P, >::type, p) ) const
		{
			return static_cast<const any_fun_impl_base< R (pp_enum_params(i_, P) ) >&>(*this)(pp_enum_params(i_, p) );
		}
	}
}


#undef i_

#endif		// pp_is_iterating
