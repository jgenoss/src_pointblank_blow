#if !defined(pp_is_iterating)


namespace i3
{
	namespace ft
	{
		namespace detail
		{
			
			#define		pp_iteration_params_1	(3, (0, ft_arity_limit, "pp_ft_synthesize_impl_function.h"))				// 1단계 파일반복..
			#include	pp_iterate()
		}
	}
}

#else // pp_is_iterating

			#define i_			pp_frame_iteration(1)					// arity의 수..
			#define seq_size_	pp_inc(i_)								//
			
			
			template<>
			struct synthesize_impl_o< pp_ft_flags, pp_ft_cc_id, seq_size_> 
			{	
				template<class Seq> 
				struct synthesize_impl_i 
				{
					typedef typename mpl::begin<Seq>::type		iter_0;

					#define pp_ft_repeat_param(z, n ,d)	\
					typedef typename mpl::next<pp_cat(iter_,n)>::type	pp_cat(iter_, pp_inc(n));
					pp_repeat(i_, pp_ft_repeat_param,~)
					#undef  pp_ft_repeat_param

					typedef typename mpl::deref<iter_0>::type	R;

					#define pp_ft_repeat_param(z, n, d) \
					typedef typename mpl::deref<pp_cat(iter_, pp_inc(n))>::type		pp_cat(P, n);
					pp_repeat(i_, pp_ft_repeat_param,~)
					#undef  pp_ft_repeat_param

					typedef R pp_ft_cc type ( pp_enum_params(i_, P) pp_ft_variadic ) pp_ft_cv;

				};	
			};

			#undef seq_size_
			#undef i_
			


#endif // pp_is_iterating