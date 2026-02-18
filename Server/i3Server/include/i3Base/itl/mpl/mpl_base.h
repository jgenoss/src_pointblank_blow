#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class T = na>
		struct base {	typedef typename T::base type;	};
	}
}

#define na_spec (1, base)
#include "aux_/mpl_na_spec.h"
