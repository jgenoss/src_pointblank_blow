#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class Iterator = na> struct deref {	typedef typename Iterator::type		type;	};
	}
}

#define na_spec (1, deref)
#include "aux_/mpl_na_spec.h"
