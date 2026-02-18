#if !defined(pp_is_iterating)
#ifndef mpl_advance_backward_h
#define mpl_advance_backward_h

#include "../mpl_next_prior.h"
#include "../mpl_apply_wrap.h"
#include "../mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<long N> struct advance_backward;
		}
	}
}

#define		pp_iteration_params_1	(3, (0, mpl_unrolling_limit, "mpl_advance_backward.h"))				// 1단계 파일반복..
#include	pp_iterate()

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< long N >
			struct advance_backward
			{
				template< typename Iterator > 
				struct apply
				{
					typedef typename apply_wrap1<advance_backward<mpl_unrolling_limit>, Iterator>::type chunk_result_;
					typedef typename apply_wrap1<advance_backward<((N - mpl_unrolling_limit) < 0 ? 0 : N - mpl_unrolling_limit)>, chunk_result_>::type type;
				};
			};
		}
	}
}


#endif
#else

#if			pp_iteration_depth() == 1

#define		i_			pp_frame_iteration(1)


namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<> struct advance_backward<i_>
			{
				template<class Iterator> struct apply
				{
					typedef 	Iterator	iter0;
					#if i_ > 0
					#define		pp_iteration_params_2	(3, (1, i_, "mpl_advance_backward.h"))
					#include	pp_iterate()
					#endif
					typedef		pp_cat(iter, i_)	type;
				};
			};
		}
	}
}

#undef		i_

#elif			pp_iteration_depth() == 2
	
#define		j_			pp_frame_iteration(2)

					typedef typename prior< pp_cat(iter, pp_dec(j_)) >::type		pp_cat(iter, j_);

#undef j_
#endif	//pp_iteration_depth
#endif	// pp_is_iterating
