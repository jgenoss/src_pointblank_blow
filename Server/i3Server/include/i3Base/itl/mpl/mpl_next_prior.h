#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template<class T = na> struct next {	typedef typename T::next	type;	};
		template<class T = na> struct prior {	typedef typename T::prior	type;	};
	}
}

#define na_spec (1, next)
#include "aux_/mpl_na_spec.h"

#define na_spec (1, prior)
#include "aux_/mpl_na_spec.h"


