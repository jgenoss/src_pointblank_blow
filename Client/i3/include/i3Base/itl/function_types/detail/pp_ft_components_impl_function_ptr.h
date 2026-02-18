#if !defined(pp_is_iterating)


namespace i3
{
	namespace ft
	{
		namespace detail
		{
			#define		pp_iteration_params_1	(3, (0, ft_arity_limit, "pp_ft_components_impl_function_ptr.h"))				// 1단계 파일반복..
			#include	pp_iterate()
		}
	}
}

#else // pp_is_iterating
			#define i_		pp_frame_iteration(1)

			template<class R pp_enum_trailing_params(i_, class T), class L>
			struct components_impl< R (pp_ft_cc *) ( pp_enum_params(i_, T) pp_ft_variadic ) pp_ft_cv, L > :
				bits_tag< encode_bits<pp_ft_flags, pp_ft_cc_id>::value, pp_ft_mask_full> 
			{
				typedef components_mpl_sequence_tag tag;
				typedef mpl::integral_c<std::size_t, i_>		function_arity;
				typedef pp_cat(mpl::vector, pp_inc(i_))< R pp_enum_trailing_params(i_, T) > types;
			};



			#undef i_
			


#endif // pp_is_iterating