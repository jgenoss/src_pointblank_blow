#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Iterator = na>
		struct iterator_category { typedef typename Iterator::category type; };
	}
}

#define na_spec (1, iterator_category)
#include "aux_/mpl_na_spec.h"
