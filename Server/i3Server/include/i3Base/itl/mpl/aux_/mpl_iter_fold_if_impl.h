#pragma once

#include "../mpl_identity.h"
#include "../mpl_next_prior.h"
#include "../mpl_if.h"
#include "../mpl_apply.h"
#include "../mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class Iterator, class State >
			struct iter_fold_if_null_step {		typedef State state;	typedef Iterator iterator;	};

			template< bool >
			struct iter_fold_if_step_impl
			{
				template< class Iterator, class State, class StateOp, class IteratorOp>
				struct result_
				{
					typedef typename apply2<StateOp,State,Iterator>::type state;
					typedef typename IteratorOp::type iterator;
				};
			};

			template<>
			struct iter_fold_if_step_impl<false>
			{
				template< class Iterator, class State, class StateOp, class IteratorOp >
				struct result_
				{
					typedef State state;
					typedef Iterator iterator;
				};
			};

			template< class Iterator, class State, class ForwardOp, class Pred>
			struct iter_fold_if_forward_step
			{
				typedef typename apply2<Pred,State,Iterator>::type not_last;
				typedef typename iter_fold_if_step_impl<not_last::value>::template result_<Iterator,State,ForwardOp,mpl::next<Iterator> > impl_;
				typedef typename impl_::state state;
				typedef typename impl_::iterator iterator;
			};

			template<class Iterator, class State, class BackwardOp, class Pred>
			struct iter_fold_if_backward_step
			{
				typedef typename apply2<Pred,State,Iterator>::type not_last;
				typedef typename iter_fold_if_step_impl<not_last::value>::template result_<Iterator,State,BackwardOp, identity<Iterator> > impl_;
				typedef typename impl_::state state;
				typedef typename impl_::iterator iterator;
			};
			

			#define aux_iter_fold_forward_step(unused, i, unused2) \
				typedef iter_fold_if_forward_step<typename pp_cat(forward_step,i)::iterator, typename pp_cat(forward_step,i)::state, ForwardOp, ForwardPred> \
				pp_cat(forward_step, pp_inc(i)); 
    
			#define aux_iter_fold_backward_step_func(i) \
				typedef iter_fold_if_backward_step<typename pp_cat(forward_step,pp_dec(i))::iterator, typename pp_cat(backward_step,i)::state, BackwardOp, BackwardPred> \
				pp_cat(backward_step,pp_dec(i));
    
			#define aux_iter_fold_backward_step(unused, i, unused2) aux_iter_fold_backward_step_func(pp_sub_d(1,mpl_unrolling_limit,i))

    
			template<class Iterator, class State, class ForwardOp, class ForwardPred, class BackwardOp, class BackwardPred >
			struct iter_fold_if_impl
			{
			 private:
				typedef iter_fold_if_null_step<Iterator,State> forward_step0;

				pp_repeat(mpl_unrolling_limit, aux_iter_fold_forward_step, unused)
			    
				typedef typename if_< typename pp_cat(forward_step, mpl_unrolling_limit)::not_last
									, iter_fold_if_impl< typename pp_cat(forward_step, mpl_unrolling_limit)::iterator
														, typename pp_cat(forward_step, mpl_unrolling_limit)::state
														, ForwardOp
														, ForwardPred
														, BackwardOp
														, BackwardPred
														>
									, iter_fold_if_null_step< typename pp_cat(forward_step, mpl_unrolling_limit)::iterator
														, typename pp_cat(forward_step, mpl_unrolling_limit)::state
														>
									>::type pp_cat(backward_step, mpl_unrolling_limit);

				pp_repeat(mpl_unrolling_limit, aux_iter_fold_backward_step, unused)

			 public:
				typedef typename backward_step0::state state;
				typedef typename pp_cat(backward_step, mpl_unrolling_limit)::iterator iterator;
			};

			#undef aux_iter_fold_backward_step
			#undef aux_iter_fold_backward_step_func
			#undef aux_iter_fold_forward_step

		}
	}
}
