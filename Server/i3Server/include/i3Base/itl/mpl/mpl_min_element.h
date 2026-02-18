#pragma once

#include "mpl_max_element.h"
#include "mpl_not.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class Pred = less<_,_> >
		struct min_element : max_element<Seq, mpl::not_<Pred> > {};
	}
}

#define na_spec (1, min_element)
#include "aux_/mpl_na_spec.h"

