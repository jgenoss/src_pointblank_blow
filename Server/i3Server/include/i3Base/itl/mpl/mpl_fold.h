#pragma once

#include "mpl_begin_end.h"
#include "mpl_O1_size.h"
#include "aux_/mpl_fold_impl.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class State = na, class ForwardOp = na>
		struct fold
		{
			typedef typename aux::fold_impl<mpl::O1_size<Seq>::value, typename begin<Seq>::type, typename end<Seq>::type, State, ForwardOp>::state type;
		};
	}
}


#define na_spec (3, fold)
#include "aux_/mpl_na_spec.h"

