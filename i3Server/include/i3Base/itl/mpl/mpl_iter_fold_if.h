#pragma once

#include "mpl_begin_end.h"
#include "mpl_logical.h"
#include "mpl_always.h"
#include "mpl_if.h"
#include "mpl_pair.h"
#include "mpl_apply.h"
#include "aux_/mpl_iter_fold_if_impl.h"
#include "mpl_na.h"


namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class Pred, class LastIterator >
			struct iter_fold_if_pred
			{
				template< class State, class Iterator > 
				struct apply : and_< not_< is_same<Iterator,LastIterator> >, apply1<Pred,Iterator> > {};
			};
		}

		template<class Seq = na, class State = na, class ForwardOp = na, class ForwardPred = na, class BackwardOp = na, class BackwardPred = na>
		struct iter_fold_if
		{
			typedef typename begin<Seq>::type first_;
			typedef typename end<Seq>::type last_;

			typedef typename eval_if<is_na<BackwardPred>, if_< is_na<BackwardOp>, always<false_>, always<true_> >, identity<BackwardPred> >::type backward_pred_;


			struct result_ : 
				aux::iter_fold_if_impl<first_, State, ForwardOp, protect< aux::iter_fold_if_pred< ForwardPred,last_ > >, BackwardOp, backward_pred_> {};

		public:	
			typedef pair<typename result_::state, typename result_::iterator> type;
		};

	}
}


#define na_spec (6, iter_fold_if)
#include "aux_/mpl_na_spec.h"
