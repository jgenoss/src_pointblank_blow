#pragma once

#include "mpl_copy.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class Inserter = na> struct reverse : reverse_copy<Seq, Inserter> {};
	}
}

#define na_spec (1, reverse)
#include "aux_/mpl_na_spec.h"

