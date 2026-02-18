#pragma once

#include "mpl_begin_end.h"
#include "mpl_O1_size.h"
#include "mpl_lambda.h"
#include "aux_/mpl_iter_fold_impl.h"
#include "mpl_na.h"


namespace i3
{
	namespace mpl
	{
		
		template<class Seq = na, class State = na, class ForwardOp = na>
		struct iter_fold
		{
			typedef typename aux::iter_fold_impl< mpl::O1_size<Seq>::value, typename begin<Seq>::type
												, typename end<Seq>::type, State, typename lambda<ForwardOp>::type>::state type;
		};
	}
}


#define na_spec (3, iter_fold)
#include "aux_/mpl_na_spec.h"
