#pragma once

#include "mpl_begin_end.h"
#include "mpl_O1_size.h"
#include "mpl_arg.h"
#include "mpl_lambda.h"
#include "aux_/mpl_reverse_iter_fold_impl.h"
#include "mpl_na.h"


namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class State = na, class BackwardOp = na, class ForwardOp = arg<1> >
		struct reverse_iter_fold
		{
			typedef typename aux::reverse_iter_fold_impl<
				  O1_size<Seq>::value, typename begin<Seq>::type, typename end<Seq>::type, State, 
				  typename lambda<BackwardOp>::type, typename lambda<ForwardOp>::type>::state type;
		};

	}
}

#define na_spec (3, reverse_iter_fold)
#include "aux_/mpl_na_spec.h"
