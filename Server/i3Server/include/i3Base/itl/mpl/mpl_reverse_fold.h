#pragma once

#include "mpl_begin_end.h"
#include "mpl_O1_size.h"
#include "mpl_arg.h"
#include "aux_/mpl_reverse_fold_impl.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class State = na, class BackwardOp = na, class ForwardOp = arg<1> >
		struct reverse_fold
		{
			typedef typename aux::reverse_fold_impl< O1_size<Seq>::value, typename begin<Seq>::type, typename end<Seq>::type, 
				State, BackwardOp, ForwardOp>::state type;
		};
	}
}


#define na_spec (3, reverse_fold)
#include "aux_/mpl_na_spec.h"

