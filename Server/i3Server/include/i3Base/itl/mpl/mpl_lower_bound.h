#pragma once

#include "mpl_less.h"
#include "mpl_lambda.h"
#include "mpl_na.h"
#include "mpl_minus.h"
#include "mpl_divides.h"
#include "mpl_size.h"
#include "mpl_advance.h"
#include "mpl_begin_end.h"
#include "mpl_long.h"
#include "mpl_if.h"
#include "mpl_next_prior.h"
#include "mpl_deref.h"
#include "mpl_apply.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Distance, class Pred, class T, class DeferredIterator>
			struct lower_bound_step_impl;

			template<class Distance, class Pred, class T, class DeferredIterator>
			struct lower_bound_step
			{
				typedef typename eval_if<Distance, lower_bound_step_impl<Distance,Pred,T,DeferredIterator>, DeferredIterator>::type type;
			};
    
			template<class Distance, class Pred, class T, class DeferredIterator>
			struct lower_bound_step_impl
			{
				typedef typename divides< Distance, long_<2> >::type offset_;
				typedef typename DeferredIterator::type iter_;
				typedef typename advance< iter_,offset_ >::type middle_;
				typedef typename apply2<Pred, typename deref<middle_>::type, T>::type cond_;

				typedef typename prior< minus< Distance, offset_> >::type step_;
				typedef lower_bound_step< offset_,Pred,T,DeferredIterator > step_forward_;
				typedef lower_bound_step< step_,Pred,T,next<middle_> > step_backward_;
				typedef typename eval_if< cond_, step_backward_, step_forward_>::type type;
			};
		}

		template<class Seq = na, class T = na, class Pred = less<> >
		struct lower_bound
		{
		 private:
			typedef typename lambda<Pred>::type pred_;
			typedef typename size<Seq>::type size_;
		 public:
			typedef typename aux::lower_bound_step<size_,pred_,T,begin<Seq>	>::type type;
		};

	}
}

#define na_spec (2, lower_bound)
#include "aux_/mpl_na_spec.h"
