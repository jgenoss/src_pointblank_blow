#pragma once

#include "mpl_find_if.h"
#include "mpl_same_as.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Seq = na, class T = na>
		struct find : find_if<Seq, same_as<T> > {};
	}
}

#define na_spec (2, find)
#include "aux_/mpl_na_spec.h"
