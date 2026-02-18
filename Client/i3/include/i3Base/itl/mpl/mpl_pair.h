#pragma once

#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template< class T1 = na, class T2 = na >
		struct pair {	typedef pair type;	typedef T1 first;	typedef T2 second;	};

		template< class P = na >
		struct first {	typedef typename P::first type;	};

		template< class P = na >
		struct second {	typedef typename P::second type; };
		
	}
}


#define na_spec (2, pair)
#include "aux_/mpl_na_spec.h"

#define na_spec (1, first)
#include "aux_/mpl_na_spec.h"

#define na_spec (1, second)
#include "aux_/mpl_na_spec.h"

