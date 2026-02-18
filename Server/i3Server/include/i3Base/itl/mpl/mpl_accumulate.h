#pragma once

#include "mpl_fold.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class State = na, class ForwardOp = na>
		struct accumulate : fold<Seq, State, ForwardOp>{};
	}
}

#define na_spec (3, accumulate)
#include "aux_/mpl_na_spec.h"
