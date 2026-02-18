#if !defined(pp_is_iterating)
#ifndef mpl_iter_fold_impl_h
#define mpl_iter_fold_impl_h

#include "../mpl_next_prior.h"
#include "../mpl_apply.h"
#include "../mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< int N, class First, class Last, class State, class ForwardOp> 
			struct iter_fold_impl;
		}
	}
}

#define		pp_iteration_params_1	(3, (0, mpl_unrolling_limit, "mpl_iter_fold_impl.h"))		// 0 ~ 10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<int N, class First, class Last, class State, class ForwardOp>
			struct iter_fold_impl
			{
				typedef iter_fold_impl<mpl_unrolling_limit, First, Last, State, ForwardOp> chunk_;

				typedef iter_fold_impl< ( (N - mpl_unrolling_limit) < 0 ? 0 : N - mpl_unrolling_limit )
										, typename chunk_::iterator, Last, typename chunk_::state, ForwardOp> res_;
				typedef typename res_::state state;
				typedef typename res_::iterator iterator;
			};

			template<class First, class Last, class State, class ForwardOp>
			struct iter_fold_impl<-1,First,Last,State,ForwardOp >
				: iter_fold_impl<-1, typename mpl::next<First>::type, Last, typename apply2< ForwardOp,State,First >::type, ForwardOp>{};

			template<class Last, class State, class ForwardOp>
			struct iter_fold_impl< -1,Last,Last,State,ForwardOp >{	typedef State state;	typedef Last iterator;	};
			
		}
	}
}

#endif


#else // pp_is_iterating

#if			pp_iteration_depth() == 1

#define		i_			pp_frame_iteration(1)


namespace i3
{
	namespace mpl 
	{
		namespace aux
		{
			template<class First, class Last, class State, class ForwardOp> 
			struct iter_fold_impl< i_, First, Last, State, ForwardOp>
			{
				typedef First iter0;
				typedef State state0;
				
				#if i_ > 0
				#define		pp_iteration_params_2	(3, (1, i_, "mpl_iter_fold_impl.h"))
				#include	pp_iterate()
				#endif

				typedef pp_cat(state, i_)	state;
				typedef pp_cat(iter, i_)	iterator;
			};

		}
	}
}

#undef		i_

#elif		pp_iteration_depth() == 2
	
#define		j_			pp_frame_iteration(2)
#define		dec_j_		pp_dec(j_)		

				typedef typename apply2<ForwardOp, pp_cat(state, dec_j_), pp_cat(iter, dec_j_)>::type pp_cat(state, j_);
				typedef typename mpl::next<pp_cat(iter, dec_j_)>::type	pp_cat(iter, j_);

#undef		dec_j_
#undef		j_

#endif	//pp_iteration_depth

#endif // pp_is_iterating
