#pragma once

#include "mpl_less.h"
#include "mpl_if.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class N1 = na, class N2 = na>
		struct min : if_< less<N1,N2>,N1,N2 > {};

		template<class N1 = na, class N2 = na>
		struct max : if_< less<N1,N2>,N2,N1 > {};
	}
}

#define na_spec (2, min)
#include "aux_/mpl_na_spec.h"

#define na_spec (2, max)
#include "aux_/mpl_na_spec.h"
