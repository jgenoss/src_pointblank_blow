#pragma once

#include "aux_/mpl_iter_fold_if_impl.h"
#include "aux_/mpl_iter_apply.h"
#include "mpl_and.h"
#include "mpl_not.h"
#include "mpl_begin_end.h"
#include "mpl_next_prior.h"
#include "mpl_always.h"
#include "mpl_bool.h"
#include "mpl_lambda.h"
#include "mpl_bind.h"
#include "mpl_apply.h"
#include "mpl_void.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Pred, class LastIterator1, class LastIterator2>
			struct equal_pred
			{
				template<class Iterator2, class Iterator1>
				struct apply
				{
					typedef typename and_< not_< is_same<Iterator1,LastIterator1> >, not_< is_same<Iterator2,LastIterator2> >
						, aux::iter_apply2<Pred,Iterator1,Iterator2> >::type type;
				};
			};

			template<class Seq1, class Seq2, class Pred>
			struct equal_impl
			{
				typedef typename begin<Seq1>::type first1_;
				typedef typename begin<Seq2>::type first2_;
				typedef typename end<Seq1>::type last1_;
				typedef typename end<Seq2>::type last2_;

				typedef aux::iter_fold_if_impl<first1_, first2_, next<>, protect< aux::equal_pred<Pred,last1_,last2_> >
					, void_, always<false_>	> fold_;

				typedef typename fold_::iterator iter1_;
				typedef typename fold_::state iter2_;
				typedef and_< is_same<iter1_,last1_>, is_same<iter2_,last2_> > result_;

				typedef typename result_::type type;
			};

		}

		template<class Seq1 = na, class Seq2 = na, class Pred = is_same<_,_> >
		struct equal : aux::equal_impl<Seq1,Seq2,Pred>::type {};
	}
}

#define na_spec (2, equal)
#include "aux_/mpl_na_spec.h"
