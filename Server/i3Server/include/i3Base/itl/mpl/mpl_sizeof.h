#pragma once

#include "mpl_size_t.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class T = na>
		struct sizeof_ : mpl::size_t< sizeof(T) > {};
	}
}

#define na_spec (1, sizeof_)
#include "aux_/mpl_na_spec.h"
