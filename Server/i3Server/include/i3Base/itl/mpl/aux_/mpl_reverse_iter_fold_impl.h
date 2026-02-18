#if !defined(pp_is_iterating)

#ifndef mpl_reverse_iter_fold_impl_h
#define mpl_reverse_iter_fold_impl_h

#include "../mpl_next_prior.h"
#include "../mpl_apply.h"
#include "../mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<long N, class First, class Last, class State, class BackwardOp, class ForwardOp> 
			struct reverse_iter_fold_impl;
		}
	}
}


#define aux_iter_fold_forward_step(unused, i, unused2) \
	typedef typename apply2< ForwardOp, pp_cat(fwd_state, i), pp_cat(iter, i) >::type pp_cat(fwd_state, pp_inc(i)); \
	typedef typename mpl::next<pp_cat(iter, i)>::type	pp_cat(iter, pp_inc(i));

#define aux_iter_fold_backward_step_func(i) \
	typedef typename apply2<BackwardOp, pp_cat(bkwd_state, i), pp_cat(iter, pp_dec(i)) >::type pp_cat(bkwd_state, pp_dec(i));

#define aux_iter_fold_backward_step(unused, i, j) aux_iter_fold_backward_step_func(pp_sub_d(1, j, i))

// 파일반복
#define		pp_iteration_params_1	(3, (0, mpl_unrolling_limit, "mpl_reverse_iter_fold_impl.h"))		// 0 ~ 10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()
//

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<long N, class First, class Last, class State, class BackwardOp, class ForwardOp> 
			struct reverse_iter_fold_impl
			{
				typedef First	iter0;
				typedef State	fwd_state0;
				
				pp_repeat( mpl_unrolling_limit, aux_iter_fold_forward_step, unused)
				
				typedef reverse_iter_fold_impl<	( ( N - mpl_unrolling_limit) < 0 ? 0 : N - mpl_unrolling_limit )
												, pp_cat(iter, mpl_unrolling_limit)
												, Last
												, pp_cat(fwd_state, mpl_unrolling_limit)
												, BackwardOp
												, ForwardOp > nested_chunk;
				
				typedef typename nested_chunk::state pp_cat(bkwd_state, mpl_unrolling_limit);
				
				pp_repeat( mpl_unrolling_limit, aux_iter_fold_backward_step, mpl_unrolling_limit )

				typedef bkwd_state0							state;
				typedef typename nested_chunk::iterator		iterator;
			};

			template<class First, class Last, class State, class BackwardOp, class ForwardOp> 
			struct reverse_iter_fold_impl<-1,First,Last,State,BackwardOp,ForwardOp>
			{
				typedef reverse_iter_fold_impl<-1, typename mpl::next<First>::type, Last
					, typename apply2<ForwardOp,State, First>::type, BackwardOp, ForwardOp	> nested_step;
				typedef typename apply2<BackwardOp, typename nested_step::state, First>::type state;
				typedef typename nested_step::iterator iterator;
			};

			template<class Last, class State, class BackwardOp, class ForwardOp> 
			struct reverse_iter_fold_impl<-1,Last,Last,State,BackwardOp,ForwardOp>
			{
				typedef State state;
				typedef Last iterator;
			};
		}
	}
}



#undef aux_iter_fold_backward_step
#undef aux_iter_fold_backward_step_func
#undef aux_iter_fold_forward_step

#endif


#else // pp_is_iterating



#define i_				pp_frame_iteration(1)

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<typename First, typename Last, typename State, typename BackwardOp, typename ForwardOp>
			struct reverse_iter_fold_impl<i_,First,Last,State,BackwardOp,ForwardOp>
			{
				typedef First iter0;
				typedef State fwd_state0;

				pp_repeat(i_, aux_iter_fold_forward_step, unused)

				typedef pp_cat(fwd_state,i_) pp_cat(bkwd_state,i_);

				pp_repeat(i_, aux_iter_fold_backward_step, i_)

				typedef bkwd_state0 state;
				typedef pp_cat(iter,i_) iterator;
			};
		}
	}
}

#undef	i_



#endif // pp_is_iterating
